"""Offline, exact-artifact map lookup; this tool does not unwind a stack."""
import argparse
import hashlib
import json
from pathlib import Path
import re

try:
    from .binary import PE
except ImportError:
    from binary import PE


IDENTITY = rb"(?<![a-z0-9])(?:[0-9a-f]{12}|unknown)-[0-9a-f]{12}(?![a-z0-9])"
ROW = re.compile(r"\s*([0-9a-fA-F]{4}):([0-9a-fA-F]{8})\s+(\S+)\s+"
                 r"([0-9a-fA-F]{8})\s+f\s+(?:i\s+)?(.+)")


def artifacts(executable, linker_map):
    pe = PE(executable)
    raw = Path(linker_map).read_bytes()
    text = raw.decode("utf-8")
    identities = set(re.findall(IDENTITY, pe.data))
    if len(identities) != 1:
        raise ValueError("Executable must contain exactly one Debug build identity")
    timestamp = re.search(r"Timestamp is ([0-9a-fA-F]+)", text)
    base = re.search(r"Preferred load address is ([0-9a-fA-F]+)", text)
    if not timestamp or not base or int(timestamp[1], 16) != pe.timestamp or int(base[1], 16) != pe.image_base:
        raise ValueError("Map timestamp/image base does not match executable")
    functions = []
    for line in text.splitlines():
        match = ROW.fullmatch(line)
        if match:
            segment, offset, va = int(match[1], 16), int(match[2], 16), int(match[4], 16)
            if (not 1 <= segment <= len(pe.sections)
                    or va != (pe.image_base + pe.sections[segment - 1]["virtual_address"] + offset) & 0xffffffff):
                raise ValueError("Map function section/VA disagrees with executable")
            functions.append((va, match[3], match[5].strip()))
    if not functions:
        raise ValueError("Map contains no function symbols")
    binding = dict(version=1, build_id=identities.pop().decode(),
                   executable_sha256=hashlib.sha256(pe.data).hexdigest(),
                   map_sha256=hashlib.sha256(raw).hexdigest(),
                   timestamp=pe.timestamp, image_base=pe.image_base)
    return binding, pe, sorted(set(functions))


def address(pe, functions, runtime_base, value, kind):
    if not 0 <= runtime_base <= 0xffffffff or not 0 <= value <= 0xffffffff:
        raise ValueError("Runtime addresses must be unsigned x86 values")
    rva = value - runtime_base
    result = dict(kind=kind, runtime_address=f"0x{value:08x}", status="outside-executable-code")
    section = next((s for s in pe.sections if s["characteristics"] & 0x20000000
                    and s["virtual_address"] <= rva < s["virtual_address"] + s["virtual_size"]), None)
    if section is None:
        return result
    preferred = pe.image_base + rva
    candidates = [f for f in functions if pe.image_base + section["virtual_address"] <= f[0] <= preferred]
    result.update(preferred_address=f"0x{preferred:08x}", status="no-preceding-function")
    if not candidates:
        return result
    start = candidates[-1][0]
    result.update(status="symbol-start" if start == preferred else "nearest-function-not-extent-verified",
                  offset=f"0x{preferred - start:x}",
                  symbols=[dict(name=name, object=owner) for va, name, owner in candidates if va == start])
    return result


def resolve(manifest, executable, linker_map, build_id, runtime_base, ip, stack):
    expected = json.loads(Path(manifest).read_text())
    actual, pe, functions = artifacts(executable, linker_map)
    if expected != actual or build_id != actual["build_id"]:
        raise ValueError("Artifact manifest or crash build identity mismatch")
    if len(stack) > 16:
        raise ValueError("At most 16 raw stack words are supported")
    return dict(build_id=build_id, warning="Map lookup only; no stack unwinding or source lines. "
                "Nearest symbols may precede padding or unrelated code. Stack words are unverified candidates.",
                fault_ip=address(pe, functions, runtime_base, ip, "fault-instruction"),
                stack_candidates=[address(pe, functions, runtime_base, value, "raw-stack-word-not-frame")
                                  for value in stack])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("mode", choices=("capture", "resolve"))
    parser.add_argument("--exe", required=True)
    parser.add_argument("--map", required=True, dest="linker_map")
    parser.add_argument("--manifest", required=True)
    parser.add_argument("--build-id", help="Exact build= value from the fault record")
    parser.add_argument("--module-base", type=lambda s: int(s, 0))
    parser.add_argument("--ip", type=lambda s: int(s, 0))
    parser.add_argument("--stack", type=lambda s: int(s, 0), action="append", default=[])
    args = parser.parse_args()
    try:
        if args.mode == "capture":
            binding, _, _ = artifacts(args.exe, args.linker_map)
            # Never overwrite provenance from an earlier artifact pair.
            with Path(args.manifest).open("x") as output:
                json.dump(binding, output, indent=2)
                output.write("\n")
        else:
            if args.build_id is None or args.module_base is None or args.ip is None:
                parser.error("resolve requires --build-id, --module-base and --ip from the same fault record")
            print(json.dumps(resolve(args.manifest, args.exe, args.linker_map, args.build_id,
                                     args.module_base, args.ip, args.stack), indent=2))
    except (OSError, ValueError) as error:
        parser.exit(2, f"crash_symbols: {error}\n")


if __name__ == "__main__":
    main()
