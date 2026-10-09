"""Strict read-only machine=0 linker metadata; original bytes are never changed.

This special metadata decoder reuses ordinary COFF primitive bounds/section
decoding and inherited symbol graph interface. It cannot extract runtime regions.
Normal tools.binary.COFF and i386 project inputs are unaffected.
"""
from pathlib import Path
import struct
from tools.binary import COFF, _unpack, _slice, _sections


class NeutralMetadata(COFF):
    def __init__(self, path):
        self.path = Path(path)
        self.data = self.path.read_bytes()
        machine, count, _, symptr, nsyms, optsize, flags = _unpack('<HHIIIHH', self.data, 0)
        if machine != 0 or optsize or flags or not count or not symptr or not nsyms:
            raise ValueError('unsupported neutral metadata header')
        header_end = 20 + count * 40
        if symptr < header_end:
            raise ValueError('neutral symbol table overlaps headers')
        stroff = symptr + nsyms * 18
        strlen = _unpack('<I', self.data, stroff)[0]
        if strlen < 4 or stroff + strlen != len(self.data):
            raise ValueError('neutral metadata string/payload extent')
        strings = _slice(self.data, stroff, strlen)

        def name(raw):
            value = raw.split(b'\0', 1)[0]
            if not value or any(c < 33 or c > 126 for c in value):
                raise ValueError('unsupported neutral metadata name')
            return value.decode('ascii')

        def string_at(offset):
            if not 4 <= offset < len(strings) or b'\0' not in strings[offset:]:
                raise ValueError('invalid neutral metadata string index')
            return name(strings[offset:])

        self.sections = _sections(self.data, 20, count, name, coff=True)
        coverage = set(range(header_end)) | set(range(symptr, len(self.data)))
        for section in self.sections:
            flags = section['characteristics']
            # No executable/read/write/code/data/BSS/COMDAT section is permitted.
            if (section['name'] != '.drectve' or flags & ~0x00f00a00
                    or flags & 0xa00 != 0xa00 or section['relocation_count']
                    or section['relocation_offset'] or section['virtual_size']
                    or section['virtual_address']):
                raise ValueError('neutral member has runtime or unknown section')
            start, size = section['raw_offset'], section['raw_size']
            if size:
                if start < header_end or start + size > symptr:
                    raise ValueError('neutral directive section overlaps metadata')
                region = set(range(start, start + size))
                if coverage & region:
                    raise ValueError('neutral sections overlap')
                coverage |= region
            rawheader = _slice(self.data, 20 + (section['index'] - 1) * 40, 40)
            lineptr, linecount = struct.unpack_from('<I', rawheader, 28)[0], struct.unpack_from('<H', rawheader, 34)[0]
            if lineptr or linecount:
                raise ValueError('neutral member has line records')
        if len(coverage) != len(self.data):
            raise ValueError('unaccounted neutral metadata payload')
        self.symbols, self.symbol_by_index = [], {}
        index = 0
        while index < nsyms:
            raw = _slice(self.data, symptr + index * 18, 18)
            symbolname = string_at(_unpack('<I', raw, 4)[0]) if raw[:4] == bytes(4) else name(raw[:8])
            value, section, typ, storage, naux = _unpack('<IhHBB', raw, 8)
            if index + naux >= nsyms:
                raise ValueError('neutral auxiliary index outside table')
            aux = [_slice(self.data, symptr + (index + 1 + i) * 18, 18) for i in range(naux)]
            symbol = dict(index=index, name=symbolname, value=value, section=section,
                          type=typ, storage_class=storage, aux=aux)
            if typ:
                raise ValueError('neutral symbol has runtime type')
            if storage == 3 and section == -1 and symbolname in ('@comp.id', '@feat.00') and not aux:
                pass
            elif storage == 2 and section == 0 and value == 0 and not aux:
                pass
            elif storage == 105 and section == 0 and value == 0 and len(aux) == 1:
                if aux[0][8:] != bytes(10):
                    raise ValueError('neutral weak auxiliary reserved bytes')
            elif storage == 3 and 1 <= section <= count and value == 0 and symbolname == '.drectve' and not aux:
                pass
            else:
                raise ValueError('unsupported neutral metadata symbol')
            self.symbols.append(symbol)
            self.symbol_by_index[index] = symbol
            index += 1 + naux
        def follow(symbol, seen=()):
            if symbol['index'] in seen:
                raise ValueError('neutral weak alias cycle')
            if symbol['storage_class'] == 105:
                target, search = struct.unpack_from('<II', symbol['aux'][0])
                if search not in (1, 2, 3) or target not in self.symbol_by_index:
                    raise ValueError('neutral weak alias bad target/search')
                follow(self.symbol_by_index[target], seen + (symbol['index'],))
            elif symbol['storage_class'] != 2 or symbol['section'] != 0:
                raise ValueError('neutral weak alias target is not external')
        for symbol in self.symbols:
            if symbol['storage_class'] == 105:
                follow(symbol)
        if self.defined_symbols():
            raise ValueError('neutral member contains physical definitions')

    def extract_function(self, name):
        raise ValueError('neutral metadata has no runtime functions')

    def extract_data(self, name):
        raise ValueError('neutral metadata has no runtime data')
