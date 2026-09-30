"""Bounded migration of legacy identification state and exact compact deltas.

Function evidence lives in function-candidates.jsonl; persistent state stores its
canonical digest. Data candidates remain full because their bounded cohort is
small. Prior hypotheses are comparisons, never confirmation anchors.
"""
from __future__ import annotations
import hashlib
import json
from pathlib import Path

CANDIDATE_STORAGE = 'function-digest-v1'
SUMMARY_FIELDS = ('pc', 'android', 'kind', 'status', 'rank', 'round', 'score', 'supports')


def _digest(value):
    result = hashlib.sha256()
    for part in json.JSONEncoder(sort_keys=True, separators=(',', ':'), ensure_ascii=False).iterencode(value):
        result.update(part.encode('utf-8'))
    return result.hexdigest()


def compact_candidate(row):
    """Preserve full-row identity while dropping repeated dependency evidence."""
    result = {key: row[key] for key in SUMMARY_FIELDS if key in row}
    digest = row.get('row_digest')
    if digest is not None:
        if not isinstance(digest, str) or len(digest) != 64 or any(c not in '0123456789abcdef' for c in digest):
            raise ValueError('Invalid candidate row_digest')
        if set(row) - set(SUMMARY_FIELDS) - {'row_digest'}:
            raise ValueError('Compact candidate contains unexpected evidence fields')
    result['row_digest'] = digest or _digest(row)
    return result


def candidate_delta(previous, current):
    old = {(row['pc'], row['android']): compact_candidate(row) for row in previous}
    new = {(row['pc'], row['android']): compact_candidate(row) for row in current}
    return {'added': [new[k] for k in sorted(new.keys() - old.keys())],
            'removed': [old[k] for k in sorted(old.keys() - new.keys())],
            'changed': [{'before': old[k], 'after': new[k]} for k in sorted(old.keys() & new.keys())
                        if old[k]['row_digest'] != new[k]['row_digest']]}


class _Reader:
    def __init__(self, stream, row_limit, member_limit):
        self.stream, self.buffer = stream, ''
        self.eof = False
        self.row_limit, self.member_limit = row_limit, member_limit
        self.decoder = json.JSONDecoder()

    def fill(self):
        chunk = self.stream.read(65536)
        self.buffer += chunk
        self.eof = not chunk

    def whitespace(self):
        while True:
            self.buffer = self.buffer.lstrip()
            if self.buffer or self.eof:
                return
            self.fill()

    def token(self, expected):
        self.whitespace()
        if not self.buffer.startswith(expected):
            raise ValueError('Malformed identification state: expected ' + expected)
        self.buffer = self.buffer[len(expected):]

    def value(self, limit):
        self.whitespace()
        while True:
            try:
                value, end = self.decoder.raw_decode(self.buffer)
                # Numbers can finish prematurely at a read boundary.
                if end == len(self.buffer) and not self.eof:
                    self.fill()
                    continue
                if end > limit:
                    raise ValueError('Identification state value exceeds bounded migration limit')
                self.buffer = self.buffer[end:]
                return value
            except json.JSONDecodeError as error:
                if len(self.buffer) > limit:
                    raise ValueError('Identification state value exceeds bounded migration limit') from error
                if self.eof:
                    raise ValueError('Malformed identification state') from error
                self.fill()

    def array(self, compact=True):
        self.token('[')
        result = []
        retained_size = 0
        self.whitespace()
        if self.buffer.startswith(']'):
            self.token(']')
            return result
        while True:
            row = self.value(self.row_limit)
            if not isinstance(row, dict):
                raise ValueError('Function candidate must be an object')
            if not compact:
                retained_size += sum(len(part.encode('utf-8')) for part in json.JSONEncoder(ensure_ascii=False, separators=(',', ':')).iterencode(row))
                if retained_size > self.member_limit:
                    raise ValueError('Full data cohort exceeds bounded retained-state limit')
            result.append(compact_candidate(row) if compact else row)
            self.whitespace()
            if self.buffer.startswith(']'):
                self.token(']')
                return result
            self.token(',')


def load_previous_state(path, *, row_limit=1024 * 1024, member_limit=16 * 1024 * 1024):
    """Read legacy or compact state without retaining its full function array.

    Limits apply to decoded JSON characters. A legacy function row may use at
    most 1 MiB; data rows also stream individually with a 16 MiB canonical cohort cap;
    other members at most 16 MiB.
    Oversized evidence fails explicitly instead of consuming unbounded memory.
    """
    with Path(path).open(encoding='utf-8') as stream:
        reader = _Reader(stream, row_limit, member_limit)
        reader.token('{')
        result = {}
        reader.whitespace()
        if not reader.buffer.startswith('}'):
            while True:
                key = reader.value(row_limit)
                if not isinstance(key, str) or key in result:
                    raise ValueError('Invalid or duplicate state member')
                reader.token(':')
                try:
                    if key in ('function_candidates', 'data_candidates'):
                        result[key] = reader.array(compact=key == 'function_candidates')
                    else:
                        result[key] = reader.value(member_limit)
                except ValueError as error:
                    raise ValueError(f'Identification state member {key}: {error}') from error
                reader.whitespace()
                if reader.buffer.startswith('}'):
                    break
                reader.token(',')
        reader.token('}')
        reader.whitespace()
        if reader.buffer:
            raise ValueError('Trailing identification state content')
    storage = result.get('candidate_storage')
    if storage not in (None, CANDIDATE_STORAGE):
        raise ValueError('Unsupported identification candidate storage')
    result['candidate_storage'] = CANDIDATE_STORAGE
    return result
