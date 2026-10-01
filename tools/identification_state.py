"""Bounded legacy-state migration and exact candidate digest deltas.

Full function/data evidence lives in independently hashed candidate JSONL files.
Persistent state holds canonical row digests; prior hypotheses remain comparisons,
never confirmation anchors.
"""
from __future__ import annotations
import hashlib
import json
import math
from pathlib import Path

LEGACY_CANDIDATE_STORAGE = 'function-digest-v1'
CANDIDATE_STORAGE = 'candidate-digest-v2'
SUMMARY_FIELDS = ('pc', 'android', 'kind', 'status', 'rank', 'round', 'score', 'supports')


def _digest(value):
    result = hashlib.sha256()
    for part in json.JSONEncoder(sort_keys=True, separators=(',', ':'), ensure_ascii=False).iterencode(value):
        result.update(part.encode('utf-8'))
    return result.hexdigest()


def compact_candidate(row):
    """Preserve full-row identity while dropping repeated dependency evidence."""
    if not isinstance(row, dict) or any(not isinstance(row.get(key), str) or not row[key] for key in ('pc', 'android')):
        raise ValueError('Candidate needs nonempty pc/android identities')
    result = {key: row[key] for key in SUMMARY_FIELDS if key in row}
    digest = row.get('row_digest')
    if 'row_digest' in row:
        if not isinstance(digest, str) or len(digest) != 64 or any(c not in '0123456789abcdef' for c in digest):
            raise ValueError('Invalid candidate row_digest')
        if set(row) - set(SUMMARY_FIELDS) - {'row_digest'}:
            raise ValueError('Compact candidate contains unexpected evidence fields')
        for key in ('kind', 'status'):
            if key in row and (not isinstance(row[key], str) or not row[key]):
                raise ValueError('Invalid compact candidate ' + key)
        for key in ('rank', 'round', 'supports'):
            # Retrieval fallback candidates have not entered a propagation round.
            if key == 'round' and key in row and row[key] is None:
                continue
            if key in row and (not isinstance(row[key], int) or isinstance(row[key], bool) or row[key] < 0):
                raise ValueError('Invalid compact candidate ' + key)
        if 'score' in row and (not isinstance(row['score'], (int, float)) or isinstance(row['score'], bool) or not math.isfinite(row['score'])):
            raise ValueError('Invalid compact candidate score')
    result['row_digest'] = digest or _digest(row)
    return result


def _candidates_by_identity(rows):
    result = {}
    for row in rows:
        compact = compact_candidate(row)
        key = (compact['pc'], compact['android'])
        if key in result:
            raise ValueError('Duplicate candidate identity')
        result[key] = compact
    return result


def candidate_delta(previous, current):
    """Compare full or compact cohorts using the canonical full-row digest."""
    old, new = _candidates_by_identity(previous), _candidates_by_identity(current)
    return {'added': [new[k] for k in sorted(new.keys() - old.keys())],
            'removed': [old[k] for k in sorted(old.keys() - new.keys())],
            'changed': [{'before': old[k], 'after': new[k]} for k in sorted(old.keys() & new.keys())
                        if old[k]['row_digest'] != new[k]['row_digest']]}


def _object(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            raise ValueError('Duplicate JSON member')
        result[key] = value
    return result


def _constant(value):
    raise ValueError('Nonfinite JSON value: ' + value)


class _Reader:
    def __init__(self, stream, row_limit, member_limit):
        self.stream, self.buffer = stream, ''
        self.eof = False
        self.row_limit, self.member_limit = row_limit, member_limit
        self.decoder = json.JSONDecoder(object_pairs_hook=_object, parse_constant=_constant)
        self.cohort_formats = {}

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
            except RecursionError as error:
                raise ValueError('Identification state nesting exceeds decoder limit') from error

    def array(self, key):
        self.token('[')
        result, seen, formats = [], set(), set()
        retained_size = 2
        self.cohort_formats[key] = formats
        self.whitespace()
        if self.buffer.startswith(']'):
            self.token(']')
            return result
        while True:
            row = self.value(self.row_limit)
            compact = compact_candidate(row)
            identity = (compact['pc'], compact['android'])
            if identity in seen:
                raise ValueError('Duplicate candidate identity')
            seen.add(identity)
            formats.add('compact' if 'row_digest' in row else 'full')
            # Only the summaries count toward the retained cohort limit. Full
            # legacy evidence is consumed one bounded row at a time, then freed.
            retained_size += sum(len(part.encode('utf-8')) for part in json.JSONEncoder(ensure_ascii=False, separators=(',', ':')).iterencode(compact)) + bool(result)
            if retained_size > self.member_limit:
                raise ValueError('Compact candidate cohort exceeds bounded retained-state limit')
            result.append(compact)
            self.whitespace()
            if self.buffer.startswith(']'):
                self.token(']')
                return result
            self.token(',')


def load_previous_state(path, *, row_limit=1024 * 1024, member_limit=16 * 1024 * 1024):
    """Stream legacy or digest state without retaining full evidence cohorts.

    Each decoded JSON row is limited to1MiB characters. Both canonical compact
    cohorts and other members are limited to16MiB; full legacy cohort size does
    not affect the retained-state bound. Stored fingerprint/provenance remains
    unchanged during migration; newly published compact state is fingerprinted
    over its own representation. Malformed/unsupported storage fails explicitly.
    """
    if row_limit <= 0 or member_limit <= 0:
        raise ValueError('Identification state limits must be positive')
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
                    result[key] = reader.array(key) if key in ('function_candidates', 'data_candidates') else reader.value(member_limit)
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
    if storage not in (None, LEGACY_CANDIDATE_STORAGE, CANDIDATE_STORAGE):
        raise ValueError('Unsupported identification candidate storage')
    for key, formats in reader.cohort_formats.items():
        expected = 'compact' if storage == CANDIDATE_STORAGE or (storage == LEGACY_CANDIDATE_STORAGE and key == 'function_candidates') else 'full' if storage == LEGACY_CANDIDATE_STORAGE else None
        if expected and formats - {expected}:
            raise ValueError(f'Identification state member {key}: row format disagrees with candidate storage')
    result['candidate_storage'] = CANDIDATE_STORAGE
    return result
