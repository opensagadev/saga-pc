"""Strict i386 archive/import census, not a relocation implementation.

Every ordinary archive member still passes the existing COFF parser. Supported
short imports declare linker definitions only, never candidate source providers
or original address bindings. Unsupported records are fatal, not skipped.
"""
from pathlib import Path
import hashlib
import struct
import tempfile

from tools.binary import COFF
from tools.crt_archive import CRTArchive
from tools.candidate_metadata import NeutralMetadata


def short_import(payload):
    if len(payload) < 20:
        raise ValueError('truncated short import header')
    sig1, sig2, version, machine, timestamp, size, hint, info = struct.unpack_from('<HHHHIIHH', payload)
    if (sig1, sig2, version, machine) != (0, 0xffff, 0, 0x14c):
        raise ValueError('unsupported short import signature/version/machine')
    kind, naming = info & 3, (info >> 2) & 7
    if info >> 5 or kind > 2 or naming > 3:
        raise ValueError('unsupported short import type/name/reserved flags')
    if size != len(payload) - 20:
        raise ValueError('short import payload extent mismatch')
    parts = payload[20:].split(b'\0')
    if len(parts) != 3 or parts[-1] or not parts[0] or not parts[1]:
        raise ValueError('short import requires exact symbol and DLL strings')
    if any(any(c < 33 or c > 126 for c in p) for p in parts[:2]):
        raise ValueError('unsupported short import string encoding')
    name, dll = (p.decode('ascii') for p in parts[:2])
    if '/' in dll or '\\' in dll or ':' in dll or not dll.lower().endswith('.dll'):
        raise ValueError('short import DLL identity is not a basename')
    if naming == 0 and hint == 0:
        raise ValueError('invalid zero import ordinal')
    return dict(symbol=name, dll=dll, import_type=kind, name_type=naming,
                ordinal_or_hint=hint, timestamp=timestamp,
                # Import code provides a thunk and IAT; DATA/CONST only an IAT.
                linker_definitions=([name] if kind == 0 else []) + ['__imp_' + name],
                original_binding=None, PC_credit=0)


def all_member_census(project_objects, libraries, *, max_total_archive_bytes=128*1024*1024,
                      max_total_members=16384, max_member_bytes=8*1024*1024):
    objects = dict(project_objects)
    pins, imports, total, members = [], [], 0, 0
    for path, expected in libraries:
        path = Path(path)
        size = path.stat().st_size
        total += size
        if total > max_total_archive_bytes:
            raise ValueError('archive census cumulative byte budget')
        if hashlib.sha256(path.read_bytes()).hexdigest() != expected:
            raise ValueError('archive checksum differs from pin')
        lib = CRTArchive(path, metadata_limit=max(2*1024*1024, size),
                         member_limit=max_member_bytes, payload_limit=size,
                         member_count_limit=max_total_members, symbol_count_limit=1000000)
        members += len(lib.members)
        if members > max_total_members:
            raise ValueError('archive census cumulative member budget')
        for member in lib.members:
            payload = lib.read_member(member)
            identity = str(path.resolve()) + '::' + str(member.header_offset)
            if identity in objects or any(i['identity'] == identity for i in imports):
                raise ValueError('duplicate archive census identity')
            if payload[:4] == b'\0\0\xff\xff':
                item = short_import(payload)
                item.update(identity=identity, member_name=member.name,
                            payload_size=len(payload), payload_sha256=hashlib.sha256(payload).hexdigest())
                imports.append(item)
            else:
                with tempfile.TemporaryDirectory(prefix='saga-candidate-census-') as directory:
                    staged = Path(directory) / 'member.obj'
                    staged.write_bytes(payload)
                    try:
                        objects[identity] = NeutralMetadata(staged) if payload[:2] == bytes(2) else COFF(staged)
                    except ValueError as error:
                        raise ValueError('unsupported ordinary member ' + identity + ' (' + member.name + '): ' + str(error)) from error
        if hashlib.sha256(path.read_bytes()).hexdigest() != expected:
            raise ValueError('archive changed during complete member census')
        pins.append(dict(path=str(path.resolve()), sha256=expected, members=len(lib.members),
                         payload_bytes=lib.payload_bytes_read, metadata_bytes=lib.metadata_bytes_read))
    definitions = {name for item in imports for name in item['linker_definitions']}
    return objects, definitions, dict(
        status='COMPLETE_SUPPLIED_ORDINARY_AND_VALIDATED_SHORT_IMPORT_MEMBERS_NOT_LINK_SEARCH_CLOSURE',
        libraries=pins, archive_members=members, archive_bytes=total,
        short_imports=imports, unsupported_members_skipped=0)
