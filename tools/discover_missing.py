#!/usr/bin/env python3
"""Read-only, bounded native CFG/provider discovery; no naming or score changes."""
import argparse
import bisect
import collections
import hashlib
import json
from pathlib import Path
import re
import struct
import sys

import capstone
from capstone.x86 import X86_OP_IMM, X86_OP_MEM, X86_OP_REG

if __package__:
    from .binary import COFF, PE
    from .lib import ROOT, load_symbols, number
else:
    repository = next((parent for parent in Path(__file__).resolve().parents
                       if (parent / "tools/binary.py").is_file() and
                       (parent / "config/project.json").is_file()), None)
    if repository is None:
        raise RuntimeError("Cannot locate project tools; run from the repository checkout")
    sys.path.insert(0, str(repository))
    from tools.binary import COFF, PE
    from tools.lib import ROOT, load_symbols, number


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def verify_snapshots(snapshots, label):
    if any(digest(path) != expected for path, expected in snapshots.items()):
        raise ValueError(f"{label} changed during discovery")


def guarded_table(pe, insn, decoded, owners, start, limit, trusted_extent):
    """Recognize only contiguous CMP index,N / JA default / JMP [index*4+VA]."""
    operand = insn.operands[0]
    if (insn.mnemonic != "jmp" or operand.type != X86_OP_MEM or operand.size != 4 or
            operand.mem.segment or operand.mem.base or
            not operand.mem.index or operand.mem.scale != 4):
        return None
    table = {"branch": insn.address, "table": operand.mem.disp & 0xffffffff,
             "index_register": insn.reg_name(operand.mem.index), "status": "candidate_unresolved"}
    if not trusted_extent:
        return {**table, "reason": "untrusted_function_extent"}
    previous = [row for row in decoded.values() if row.address + row.size == insn.address]
    if len(previous) != 1 or previous[0].mnemonic != "ja":
        return {**table, "reason": "unsupported_range_guard"}
    branch = previous[0]
    previous = [row for row in decoded.values() if row.address + row.size == branch.address]
    if len(previous) != 1:
        return {**table, "reason": "unsupported_range_compare"}
    compare = previous[0]
    if (compare.mnemonic != "cmp" or len(compare.operands) != 2 or
            compare.operands[0].type != X86_OP_REG or compare.operands[0].reg != operand.mem.index or
            compare.operands[1].type != X86_OP_IMM or branch.operands[0].type != X86_OP_IMM):
        return {**table, "reason": "unsupported_range_compare"}
    count = compare.operands[1].imm + 1
    default = branch.operands[0].imm & 0xffffffff
    table.update(guard_compare=compare.address, guard_branch=branch.address,
                 default_target=default, entry_count=count)
    if not 1 <= count <= 256:
        return {**table, "reason": "table_entry_budget"}
    if default == insn.address:
        return {**table, "reason": "default_reenters_dispatch"}
    if not start <= default < limit:
        return {**table, "reason": "default_outside_parent"}
    end = table["table"] + 4 * count
    if not start <= table["table"] < end <= limit:
        return {**table, "reason": "table_outside_parent"}
    if any(byte in owners for byte in range(table["table"], end)):
        return {**table, "reason": "table_overlaps_decoded_code"}
    try:
        raw = pe.read_va(table["table"], 4 * count)
    except ValueError:
        return {**table, "reason": "unreadable_table"}
    targets = list(struct.unpack("<" + "I" * count, raw))
    table["targets"] = targets
    if any(not start <= target < limit or table["table"] <= target < end for target in targets):
        return {**table, "reason": "case_target_outside_parent_or_inside_table"}
    if any(target in owners and target not in decoded for target in targets):
        return {**table, "reason": "case_target_inside_instruction"}
    return {**table, "status": "guard_validated_pending_full_cfg"}


def trace_cfg(pe, start, limit, instruction_budget=20000, trusted_extent=False,
              direct_target_resolver=None, instruction_observer=None):
    """Follow only direct intra-bound branches; unknown indirect jumps stop paths.

    The bound is supplied evidence, not a claim that all intervening bytes are code.
    Calls are recorded, never followed here. Indirect calls retain fallthrough but
    leave the outgoing closure unknown. Only a guarded table within a trusted mapped
    extent can add case edges; no enum names or padding are inferred.
    """
    if not start < limit or instruction_budget <= 0:
        raise ValueError("Invalid native CFG bound/budget")
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    pending, decoded, owners = [start], {}, {}
    calls, tails, unknowns, returns, storage = [], [], [], [], set()
    tables, predecessors = [], collections.defaultdict(set)
    predecessors[start].add(None)

    def enqueue(source, target):
        predecessors[target].add(source)
        pending.append(target)
    while pending:
        address = pending.pop()
        if address in decoded:
            continue
        if len(decoded) >= instruction_budget:
            unknowns.append({"kind": "instruction_budget", "address": address})
            break
        if not start <= address < limit:
            unknowns.append({"kind": "flow_outside_bound", "address": address})
            continue
        if address in owners:
            unknowns.append({"kind": "overlapping_instruction", "address": address})
            continue
        try:
            raw = pe.read_va(address, min(15, limit - address))
            insn = next(md.disasm(raw, address, count=1), None)
        except ValueError as error:
            unknowns.append({"kind": "unreadable", "address": address, "detail": str(error)})
            continue
        if insn is None:
            unknowns.append({"kind": "invalid_instruction", "address": address})
            continue
        if any(byte in owners for byte in range(address, address + insn.size)):
            unknowns.append({"kind": "overlapping_instruction", "address": address})
            continue
        decoded[address] = insn
        if instruction_observer is not None:
            instruction_observer(insn)
        owners.update((byte, address) for byte in range(address, address + insn.size))
        for operand in insn.operands:
            if operand.type == X86_OP_MEM and not operand.mem.base and not operand.mem.index:
                storage.add(operand.mem.disp & 0xffffffff)
        mnemonic = insn.mnemonic
        if mnemonic.startswith("ret"):
            returns.append(address + insn.size)
            continue
        if mnemonic in ("int3", "ud2", "hlt"):
            unknowns.append({"kind": "trap_terminal", "address": address})
            continue
        if mnemonic == "call":
            if insn.operands[0].type == X86_OP_IMM:
                calls.append({"site": address, "target": (direct_target_resolver(insn) if direct_target_resolver
                              else insn.operands[0].imm & 0xffffffff)})
            else:
                unknowns.append({"kind": "indirect_call", "address": address,
                                 "operand": insn.op_str})
        if mnemonic.startswith("j") or mnemonic.startswith("loop"):
            if insn.operands[0].type != X86_OP_IMM:
                table = guarded_table(pe, insn, decoded, owners, start, limit, trusted_extent)
                if table is not None:
                    tables.append(table)
                    if table["status"] == "guard_validated_pending_full_cfg":
                        for target in set(table["targets"]):
                            enqueue(address, target)
                        continue
                unknowns.append({"kind": "indirect_branch", "address": address,
                                 "operand": insn.op_str})
                continue
            target = (direct_target_resolver(insn) if direct_target_resolver
                      else insn.operands[0].imm & 0xffffffff)
            if start <= target < limit:
                enqueue(address, target)
            elif mnemonic == "jmp":
                tails.append({"site": address, "target": target})
            else:
                unknowns.append({"kind": "conditional_branch_outside_bound",
                                 "address": address, "target": target})
            if mnemonic == "jmp":
                continue
        enqueue(address, address + insn.size)
    table_bytes = set()
    for table in tables:
        if table["status"] != "guard_validated_pending_full_cfg":
            continue
        if (predecessors[table["branch"]] != {table["guard_branch"]} or
                predecessors[table["guard_branch"]] != {table["guard_compare"]}):
            table.update(status="candidate_unresolved", reason="alternate_guard_or_dispatch_entry")
        elif any(byte in owners for byte in range(table["table"], table["table"] + 4 * table["entry_count"])):
            table.update(status="candidate_unresolved", reason="table_overlaps_reachable_code")
        else:
            table["status"] = "guarded_table_validated"
            table["qualification"] = "Exclusive predecessors in this direct CFG; external interior entries are not ruled out."
            table_bytes.update(range(table["table"], table["table"] + 4 * table["entry_count"]))
            continue
        unknowns.append({"kind": "switch_table_unresolved", "address": table["branch"],
                         "reason": table["reason"]})
    if tables and any(start <= row["target"] < limit for row in calls):
        unknowns.append({"kind": "intra_parent_call_entry_unknown", "address": start})
    if unknowns:
        # An unexplored/indirect path may hide another entry into the guard.
        for table in tables:
            if table["status"] == "guarded_table_validated":
                table.update(status="candidate_unresolved", reason="incomplete_direct_cfg")
                unknowns.append({"kind": "switch_table_unresolved", "address": table["branch"],
                                 "reason": table["reason"]})
        table_bytes.clear()
    end = max(owners, default=start - 1) + 1
    first_return = min(returns, default=None)
    return {"start": start, "bound": limit, "observed_end": end,
            "observed_span_bytes": end - start, "reachable_instruction_bytes": len(owners),
            "unvisited_bytes_inside_observed_span": end - start - len(owners),
            "unvisited_bytes_inside_bound": limit - start - len(owners),
            "instruction_count": len(decoded), "first_return_end": first_return,
            "reachable_bytes_after_first_return": sum(byte >= first_return for byte in owners) if first_return else 0,
            "direct_calls": sorted(calls, key=lambda row: row["site"]),
            "direct_tail_candidates": sorted(tails, key=lambda row: row["site"]),
            "absolute_memory_references": sorted(storage), "unknowns": unknowns,
            "switch_tables": tables, "validated_table_bytes": len(table_bytes),
            "unclassified_bytes_inside_bound": limit - start - len(owners) - len(table_bytes),
            "outgoing_indirect_resolution_complete": not unknowns,
            "qualification": "Observed CFG span is not an allocation/ownership proof; "
                             "unvisited bytes are not classified as padding or data."}


class ProviderIndex:
    """Actual named COFF definitions; absence is not runtime-link absence."""
    def __init__(self):
        self.definitions = collections.defaultdict(list)
        self.hashes = {}
        self.objects = {}

    def add_object(self, path):
        path = Path(path)
        obj = COFF(path)
        snapshot = hashlib.sha256(obj.data).hexdigest()
        if digest(path) != snapshot:
            raise ValueError("Producer object changed while parsing")
        self.hashes[str(path)] = snapshot
        self.objects[str(path)] = obj
        for symbol in obj.defined_symbols():
            self.definitions[symbol["name"]].append({"object": str(path),
                "object_sha256": self.hashes[str(path)], "function": bool(symbol["type"] & 0x20),
                "type": symbol["type"], "storage_class": symbol["storage_class"]})

    def classify(self, records, address, text_start, text_end):
        aliases = [row for row in records if row["address"] == address]
        providers = [{"symbol": row["symbol"], **definition} for row in aliases
                     for definition in self.definitions.get(row["symbol"], ()) if definition["function"]]
        if not text_start <= address < text_end:
            kind = "outside_text_unknown"
        elif len(providers) > 1:
            kind = "mapped_comparison_provider_ambiguous"
        elif providers:
            kind = "mapped_comparison_provider_present"
        elif aliases:
            kind = "mapped_comparison_provider_absent"
        elif any(row["address"] is not None and row["size"] and
                 row["address"] < address < row["address"] + row["size"] for row in records):
            kind = "mapped_parent_interior_unknown"
        else:
            kind = "unmapped_native_entry_candidate"
        return {"address": address, "kind": kind, "aliases": [row["symbol"] for row in aliases],
                "providers": providers,
                "qualification": "Named COFF census does not resolve arbitrary aliases, "
                                 "CRT archive selection, imports or runtime call compatibility."}



class ExtractedCode:
    """Bounded COFF bytes; unbound data relocations are never read as PE storage."""
    def __init__(self, data, base):
        self.data, self.base = data, base

    def read_va(self, address, size):
        offset = address - self.base
        if offset < 0 or offset + size > len(self.data):
            raise ValueError("Outside extracted COFF function")
        return self.data[offset:offset + size]


def provider_call_inventory(obj, symbol, records, native_cfg, instruction_observer=None):
    """Compare observed direct-call targets, never behavioral completeness.

    Resolve only exact immediate REL32 fields or same-region branch offsets.
    Decoding the original bytes avoids turning data references into E8 calls.
    Absolute/indexed data relocations and switches remain unresolved.
    """
    try:
        region = obj.extract_function(symbol)
    except ValueError as error:
        return {"status": "candidate_extent_unknown", "detail": str(error)}
    base = 0xf0000000
    data = region["bytes"]
    if not data or len(data) > 65536:
        return {"status": "candidate_extent_unknown", "detail": "empty or over 65536-byte budget"}
    bindings = collections.defaultdict(set)
    for row in records:
        if row["address"] is not None:
            bindings[row["symbol"]].add(row["address"])
    relocations = region["relocations"]
    resolved, unknowns = {}, []
    used_relocations = set()

    def target(insn):
        site = insn.address - base
        immediate = site + insn.imm_offset
        refs = [(index, row) for index, row in enumerate(relocations)
                if row["offset"] < site + insn.size and row["offset"] + 4 > site]
        info = {"candidate_offset": site}
        if refs:
            if (len(refs) != 1 or refs[0][1]["offset"] != immediate or
                    refs[0][1]["type"] != 20 or insn.imm_size != 4):
                info.update(binding="relocation_not_exact_REL32_immediate", target=None)
                unknowns.append({"kind": info["binding"], "candidate_offset": site})
                resolved[insn.address] = info
                return 0x10000000
            index, ref = refs[0]
            used_relocations.add(index)
            addend = struct.unpack_from("<I", data, immediate)[0]
            info.update(symbol=ref["symbol"], addend=addend)
            named = bindings[ref["symbol"]]
            definition = obj.symbol_by_index[ref["symbol_index"]]
            local_offset = definition["value"] - region["offset"] + addend
            local = definition["section"] == region["symbol"]["section"] and 0 <= local_offset < len(data)
            if addend == 0 and len(named) == 1:
                info.update(binding="exact_mapped_REL32", target=next(iter(named)))
            elif local:
                info.update(binding="same_region_target_native_binding_unknown", target=None,
                            local_offset=local_offset)
            else:
                info.update(binding="nonzero_addend_or_absent_ambiguous_binding", target=None)
            resolved[insn.address] = info
            if info["target"] is None:
                unknowns.append({"kind": info["binding"], "candidate_offset": site,
                                 "symbol": ref["symbol"]})
            return base + local_offset if local else (info["target"] or 0x10000000)
        raw_target = insn.operands[0].imm & 0xffffffff
        if base <= raw_target < base + len(data):
            info.update(binding="same_region_no_relocation", target=None,
                        local_offset=raw_target - base)
        else:
            info.update(binding="unrelocated_external_target_unknown", target=None)
            unknowns.append({"kind": info["binding"], "candidate_offset": site})
        resolved[insn.address] = info
        if insn.mnemonic == "call" and info["target"] is None and "local_offset" in info:
            unknowns.append({"kind": "same_region_call_native_binding_unknown", "candidate_offset": site})
        return raw_target

    cfg = trace_cfg(ExtractedCode(data, base), base, base + len(data),
                    trusted_extent=False, direct_target_resolver=target,
                    instruction_observer=instruction_observer)
    def observations(edges):
        return [resolved[row["site"]] for row in edges]
    calls = observations(cfg["direct_calls"])
    tails = observations(cfg["direct_tail_candidates"])
    native_counts = collections.Counter(row["target"] for row in native_cfg["direct_calls"])
    candidate_counts = collections.Counter(row["target"] for row in calls if row["target"] is not None)
    tail_targets = {row["target"] for row in tails if row["target"] is not None}
    differences = [{"target": address, "native_call_count": native_counts[address],
                    "candidate_call_count": candidate_counts[address],
                    "observed_as_candidate_tail": address in tail_targets}
                   for address in sorted(native_counts.keys() | candidate_counts.keys())
                   if native_counts[address] != candidate_counts[address]]
    return {"status": "review_leads_only", "symbol": symbol,
            "candidate_size": len(data), "candidate_extent_source": region["size_source"],
            "candidate_bytes_sha256": hashlib.sha256(data).hexdigest(),
            "native_direct_calls": native_cfg["direct_calls"],
            "native_direct_tail_candidates": native_cfg["direct_tail_candidates"],
            "native_unknowns": native_cfg["unknowns"],
            "candidate_direct_calls": calls, "candidate_direct_tail_candidates": tails,
            "candidate_unknowns": cfg["unknowns"] + unknowns,
            "relocations_not_used_as_direct_control_immediates": len(relocations) - len(used_relocations),
            "call_target_count_differences": differences,
            "qualification": "Differences are review leads only: inlining, tailcalls, cloning, "
                             "optimization and unknown paths/bindings may explain them. Equal direct-call "
                             "inventories do not prove control, ABI, behavior or byte equivalence. "
                             "COFF comparison extent is not an independent native function boundary."}



def storage_binding_index(records):
    by_name, by_address = collections.defaultdict(set), collections.defaultdict(set)
    for row in records:
        if row["address"] is not None:
            by_name[row["symbol"]].add(row["address"])
            by_address[row["address"]].add(row["symbol"])
    return by_name, by_address


def storage_observations(insn, records, region=None, base=0, bindings=None):
    """Exact instruction facts, not C types, value flow or ABI semantics."""
    by_name, by_address = bindings if bindings is not None else storage_binding_index(records)
    observations = []
    for ordinal, operand in enumerate(insn.operands):
        if operand.type != X86_OP_MEM:
            continue
        row = {"site": insn.address if region is None else insn.address - base,
               "operation": insn.mnemonic, "operand_index": ordinal,
               "memory_width_bytes": operand.size, "access_flags": operand.access,
               "load_extension": "sign_extend" if insn.mnemonic in ("movsx", "movsxd") else
                                 "zero_extend" if insn.mnemonic == "movzx" else "none_in_instruction",
               "extension_destination_width_bytes": insn.operands[0].size if
                    insn.mnemonic in ("movsx", "movsxd", "movzx") else None,
               "integer_compare_width_bytes": insn.operands[0].size if
                    insn.mnemonic in ("cmp", "test") else None,
               "binding": "unknown", "storage_va": None, "symbols": []}
        observations.append(row)
        if insn.mnemonic in ("lea", "nop"):
            row["binding"] = "address_form_not_a_storage_access"
            continue
        if operand.mem.segment or operand.mem.base or operand.mem.index:
            row["binding"] = "segmented_or_register_relative_identity_unknown"
            continue
        if not operand.access:
            row["binding"] = "instruction_memory_access_semantics_unknown"
            continue
        if region is None:
            address = operand.mem.disp & 0xffffffff
        else:
            site = insn.address - base
            displacement = site + insn.disp_offset
            refs = [ref for ref in region["relocations"] if
                    ref["offset"] < site + insn.size and ref["offset"] + 4 > site]
            if (len(refs) != 1 or refs[0]["offset"] != displacement or
                    refs[0]["type"] != 6 or insn.disp_size != 4):
                row["binding"] = "no_exact_DIR32_memory_displacement"
                continue
            ref = refs[0]
            addend = struct.unpack_from("<I", region["bytes"], displacement)[0]
            row.update(relocation_symbol=ref["symbol"], addend=addend)
            addresses = by_name.get(ref["symbol"], set())
            if addend or len(addresses) != 1:
                row["binding"] = "nonzero_addend_or_absent_ambiguous_binding"
                continue
            address = next(iter(addresses))
        aliases = sorted(by_address.get(address, set()))
        row.update(storage_va=address, symbols=aliases)
        row["binding"] = "exact_named_storage_operand" if len(aliases) == 1 else (
            "storage_alias_identity_unknown" if aliases else "unmapped_or_interior_storage_unknown")
        if len(aliases) == 1 and len(by_name[aliases[0]]) != 1:
            row["binding"] = "storage_name_address_ambiguity_unknown"
    return observations


def provider_storage_inventory(obj, symbol, records, native_cfg, native_observations):
    """Bounded review-only instruction/access counts for exact storage bindings."""
    try:
        region = obj.extract_function(symbol)
    except ValueError as error:
        return {"status": "candidate_extent_unknown", "detail": str(error)}
    observations = []
    bindings = storage_binding_index(records)
    def observe(insn):
        observations.extend(storage_observations(insn, records, region, 0xf0000000, bindings))
    calls = provider_call_inventory(obj, symbol, records, native_cfg, instruction_observer=observe)
    if calls["status"] != "review_leads_only":
        return calls
    fields = ("storage_va", "operation", "memory_width_bytes", "access_flags", "load_extension",
              "extension_destination_width_bytes", "integer_compare_width_bytes")
    def counts(rows):
        return collections.Counter(tuple(row[key] for key in fields) for row in rows
                                   if row["binding"] == "exact_named_storage_operand")
    native_counts, candidate_counts = counts(native_observations), counts(observations)
    differences = []
    for key in sorted(native_counts.keys() | candidate_counts.keys(), key=repr):
        if native_counts[key] != candidate_counts[key]:
            differences.append({**dict(zip(fields, key)), "native_count": native_counts[key],
                                "candidate_count": candidate_counts[key]})
    return {"status": "review_leads_only", "symbol": symbol,
            "candidate_size": len(region["bytes"]),
            "candidate_bytes_sha256": hashlib.sha256(region["bytes"]).hexdigest(),
            "native_observations": sorted(native_observations, key=lambda row: (row["site"], row["operand_index"])),
            "candidate_observations": sorted(observations, key=lambda row: (row["site"], row["operand_index"])),
            "native_cfg_unknowns": native_cfg["unknowns"],
            "candidate_cfg_unknowns": calls["candidate_unknowns"],
            "exact_storage_operation_count_differences": differences,
            "qualification": "Instruction observations only, not source signedness, value flow, field "
                             "layout, semantic equivalence, runtime activation or missing behavior. "
                             "Legal lowering/optimization can alter operation counts; equality is not "
                             "proof of completeness. Unknown paths/identities and aliases stay explicit. "
                             "Implicit memory effects and register value flow are not modeled. "
                             "No native allocation, score or external-entry compatibility is inferred."}


def suppressed_calls(path, pe=None, snapshots=None):
    """Find actual macro tokens, excluding comments/strings; comments are only leads."""
    path = Path(path)
    raw = path.read_bytes()
    source = raw.decode(errors="replace")
    source_hash = hashlib.sha256(raw).hexdigest()
    if snapshots is not None:
        snapshots[str(path)] = source_hash
    ignored = re.compile(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'', re.S)
    cleaned = ignored.sub(lambda match: "".join("\n" if c == "\n" else " " for c in match[0]), source)
    result = []
    for match in re.finditer(r'\bSTUBBED_NAMED\s*\(\s*(0x[0-9a-fA-F]+|[0-9]+)\s*\)', cleaned):
        line = source.count("\n", 0, match.start()) + 1
        tail = source[match.end():source.find("\n", match.end()) if "\n" in source[match.end():] else len(source)]
        site = re.search(r'\bNative\s*(?:0x)?([0-9a-fA-F]{1,8})\b', tail)
        row = {"source": str(path), "source_sha256": source_hash, "line": line,
               "target": int(match[1], 0), "comment_site": int(site[1], 16) if site else None,
               "native_opcode_target_verified": False,
               "qualification": "Source-token evidence only; not preprocessor reachability or instruction-boundary proof."}
        if pe is not None and site:
            try:
                address = row["comment_site"]
                md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
                insn = next(md.disasm(pe.read_va(address, 5), address, count=1), None)
                row["native_opcode_target_verified"] = bool(insn and insn.mnemonic == "call" and
                    insn.op_str == hex(row["target"]))
            except ValueError:
                pass
        result.append(row)
    return result



def corroborate_suppressed_calls(pe, records, leads, text_start, text_end,
                                max_bytes=65536, instruction_budget=20000, max_parents=32):
    """Add exact reachable-call evidence, never infer missing behavior.

    A source comment supplies a candidate address only. Require one containing
    mapped interval and a complete bounded direct CFG from that mapped entry.
    Nonobservations stay unknown: alternate entries and source preprocessing are
    not established by this analysis. Cache only within this pinned invocation.
    """
    if max_bytes <= 0 or instruction_budget <= 0 or max_parents <= 0:
        raise ValueError("Invalid suppressed-call CFG budget")
    cache, result = {}, []
    for lead in leads:
        row = dict(lead)
        evidence = {"status": "unknown", "reason": "no_native_comment_site",
                    "external_or_interior_entry_compatibility": "unknown_not_analyzed",
                    "qualification": "Exact mapped-root reachability only, not preprocessor "
                                     "reachability, caller ABI, runtime activation or missing behavior."}
        row["cfg_corroboration"] = evidence
        result.append(row)
        site = row.get("comment_site")
        if site is None:
            continue
        if not text_start <= site < text_end:
            evidence["reason"] = "comment_site_outside_native_text"
            continue
        parents = [record for record in records if record["address"] is not None and
                   record["size"] and text_start <= record["address"] <= site <
                   record["address"] + record["size"]]
        if len(parents) != 1:
            evidence.update(reason="mapped_parent_absent_or_ambiguous", mapped_parents=[
                {key: record[key] for key in ("symbol", "address", "size")} for record in parents])
            continue
        parent = parents[0]
        start, size = parent["address"], parent["size"]
        evidence["parent"] = {key: parent[key] for key in ("symbol", "address", "size")}
        if size > max_bytes or start + size > text_end:
            evidence["reason"] = "mapped_parent_exceeds_budget_or_text"
            continue
        key = start, size
        if key not in cache:
            if len(cache) >= max_parents:
                evidence["reason"] = "mapped_parent_count_budget_exhausted"
                continue
            cache[key] = trace_cfg(pe, start, start + size, instruction_budget=instruction_budget,
                                   trusted_extent=True)
        cfg = cache[key]
        evidence["cfg_instruction_count"] = cfg["instruction_count"]
        evidence["cfg_unknowns"] = cfg["unknowns"]
        if cfg["unknowns"]:
            evidence["reason"] = "mapped_parent_cfg_incomplete"
            continue
        matches = [call for call in cfg["direct_calls"] if call["site"] == site and
                   call["target"] == row["target"]]
        if len(matches) == 1:
            evidence.update(status="corroborated_reachable_direct_call", reason="exact_site_and_target",
                            call=matches[0])
        else:
            evidence["reason"] = "site_target_not_observed_as_reachable_direct_call"
    return result


def discover(pe, records, providers, roots, text_start, text_end, suppressed=(), depth=1,
             max_roots=32, max_bytes=65536, compare_calls=False, compare_storage=False):
    if depth < 0 or max_roots <= 0 or max_bytes <= 0:
        raise ValueError("Invalid discovery limits")
    starts = sorted({row["address"] for row in records if row["address"] is not None and
                     text_start <= row["address"] < text_end})
    pending, seen, result = [(root, 0) for root in reversed(roots)], set(), []
    storage_bindings = storage_binding_index(records) if compare_storage else None
    while pending and len(result) < max_roots:
        start, level = pending.pop()
        if start in seen:
            continue
        seen.add(start)
        status = providers.classify(records, start, text_start, text_end)
        if not text_start <= start < text_end:
            result.append({"provider": status, "unknowns": [{"kind": "root_outside_text"}]})
            continue
        sizes = {row["size"] for row in records if row["address"] == start and row["size"]}
        next_index = bisect.bisect_right(starts, start)
        next_start = starts[next_index] if next_index < len(starts) else text_end
        declared = next(iter(sizes)) if len(sizes) == 1 else None
        bound = min(text_end, start + max_bytes, start + declared if declared else next_start)
        native_observations = []
        def observe_native(insn):
            native_observations.extend(storage_observations(insn, records, bindings=storage_bindings))
        cfg = trace_cfg(pe, start, bound, instruction_observer=observe_native if compare_storage else None,
                        trusted_extent=bool(declared and
                        len(sizes) == 1 and start + declared == bound))
        if declared and start + declared > bound:
            cfg["unknowns"].append({"kind": "mapped_extent_truncated_by_budget_or_text", "address": bound})
        if len(sizes) > 1:
            cfg["unknowns"].append({"kind": "ambiguous_mapped_extents", "sizes": sorted(sizes)})
        cfg["outgoing_indirect_resolution_complete"] = not cfg["unknowns"]
        leads = [row for row in suppressed if row["target"] == start]
        category = "missing_dispatch_candidate" if leads and status["kind"] == "mapped_comparison_provider_present" else (
            "suppressed_body_provider_absent_candidate" if leads and not status["providers"] else status["kind"])
        item = {"provider": status, "category": category, "declared_sizes": sorted(sizes),
                "bound_basis": "mapped_extent" if declared else "next_mapped_start_or_budget",
                "depth": level, "cfg": cfg, "suppressed_source_leads": leads}
        if compare_calls:
            if len(status["providers"]) == 1:
                provider = status["providers"][0]
                inventory = provider_call_inventory(providers.objects[provider["object"]],
                                                    provider["symbol"], records, cfg)
                inventory["native_extent_trusted"] = bool(declared and len(sizes) == 1 and
                                                          start + declared == bound)
                item["provider_call_inventory"] = inventory
            else:
                item["provider_call_inventory"] = {"status": "candidate_provider_absent_or_ambiguous"}
        if compare_storage:
            if len(status["providers"]) == 1:
                provider = status["providers"][0]
                inventory = provider_storage_inventory(providers.objects[provider["object"]],
                    provider["symbol"], records, cfg, native_observations)
                inventory["native_extent_trusted"] = bool(declared and len(sizes) == 1 and
                                                          start + declared == bound)
                item["provider_storage_inventory"] = inventory
            else:
                item["provider_storage_inventory"] = {"status": "candidate_provider_absent_or_ambiguous"}
        item["absolute_memory_reference_candidates"] = []
        for address in cfg["absolute_memory_references"]:
            aliases = [row["symbol"] for row in records if row["address"] == address]
            data = [{"symbol": name, **definition} for name in aliases
                    for definition in providers.definitions.get(name, ()) if not definition["function"]]
            item["absolute_memory_reference_candidates"].append({
                "address": address, "aliases": aliases, "named_data_providers": data,
                "resolution": "mapped_identity_only" if aliases else "unmapped_or_interior_unknown",
                "qualification": "No allocation size, initialization, interior-alias or storage closure is inferred."})
        item["edges"] = []
        for edge in cfg["direct_calls"] + cfg["direct_tail_candidates"]:
            target = edge["target"]
            target_status = providers.classify(records, target, text_start, text_end)
            item["edges"].append({**edge, "provider": target_status})
            if level < depth and target_status["kind"] in (
                    "unmapped_native_entry_candidate", "mapped_comparison_provider_absent"):
                pending.append((target, level + 1))
        result.append(item)
    return {"roots": result, "pending_roots_not_scanned": sorted({a for a, _ in pending if a not in seen}),
            "limits": {"follow_depth": depth, "max_roots": max_roots, "max_bytes_per_root": max_bytes},
            "qualification": "No names, extents, source changes, runtime closure or matching credit are inferred."}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--project-root", type=Path, default=ROOT)
    parser.add_argument("--retail", type=Path, help="Override original PE path; project hash pin still applies")
    parser.add_argument("--objects", type=Path, help="Matching COFF directory (default project build/obj)")
    parser.add_argument("--symbols", type=Path, help="Symbol CSV (default project config/symbols.csv)")
    parser.add_argument("--sources", type=Path, help="Source-token evidence root (default project src)")
    parser.add_argument("--root", action="append", required=True, help="Exact mapped symbol or native VA")
    parser.add_argument("--depth", type=int, default=1)
    parser.add_argument("--max-roots", type=int, default=32)
    parser.add_argument("--max-bytes", type=int, default=65536)
    parser.add_argument("--corroborate-suppressed-calls", action="store_true",
                        help="Add complete mapped-parent CFG evidence to source suppression leads")
    parser.add_argument("--compare-provider-storage", action="store_true",
                        help="Add exact absolute storage operand observations; review leads only")
    parser.add_argument("--compare-provider-calls", action="store_true",
                        help="Add bounded native/COFF direct-call review leads; never completeness proof")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    root = args.project_root.resolve()
    config_path = root / "config/project.json"
    config_raw = config_path.read_bytes()
    target = json.loads(config_raw)["target"]
    input_hashes = {str(config_path): hashlib.sha256(config_raw).hexdigest()}
    original = args.retail.resolve() if args.retail else root / target["exe"]
    symbols = args.symbols.resolve() if args.symbols else root / "config/symbols.csv"
    objects = args.objects.resolve() if args.objects else root / "build/obj"
    sources = args.sources.resolve() if args.sources else root / "src"
    pe = PE(original)
    if hashlib.sha256(pe.data).hexdigest() != target["sha256"]:
        raise ValueError("Original executable hash differs from project pin")
    input_hashes[str(original)] = target["sha256"]
    input_hashes[str(symbols)] = digest(symbols)
    records = load_symbols(symbols)
    verify_snapshots(input_hashes, "Native/map/config inputs")
    providers = ProviderIndex()
    for path in sorted(objects.rglob("*.obj")):
        providers.add_object(path)
    if not providers.hashes:
        raise ValueError("No COFF objects found in supplied census directory")
    before = dict(providers.hashes)
    source_hashes = {}
    source_leads = [lead for path in sorted(sources.rglob("*"))
                    if path.suffix in (".c", ".cpp") for lead in suppressed_calls(path, pe, source_hashes)]
    if args.corroborate_suppressed_calls:
        source_leads = corroborate_suppressed_calls(pe, records, source_leads,
            number(target["text_va"]), number(target["text_va"]) + number(target["text_size"]),
            args.max_bytes, max_parents=args.max_roots)
    roots = []
    for name in args.root:
        matches = {row["address"] for row in records if row["symbol"] == name and row["address"] is not None}
        if len(matches) == 1:
            roots.append(matches.pop())
        elif not matches:
            roots.append(number(name))
        else:
            raise ValueError("Ambiguous root symbol")
    result = discover(pe, records, providers, roots, number(target["text_va"]),
                      number(target["text_va"]) + number(target["text_size"]), source_leads,
                      args.depth, args.max_roots, args.max_bytes, args.compare_provider_calls,
                      args.compare_provider_storage)
    verify_snapshots(before, "Producer objects")
    verify_snapshots(source_hashes, "Source inputs")
    verify_snapshots(input_hashes, "Native/map/config inputs")
    report_path = root / "matching.json"
    report_raw = report_path.read_bytes() if report_path.is_file() else None
    report = json.loads(report_raw) if report_raw is not None else {}
    def report_key(path):
        try:
            return str(Path(path).relative_to(root))
        except ValueError:
            return None
    stale = [path for path, value in before.items() if
             report.get("objects", {}).get(report_key(path)) != value]
    census_keys = {report_key(path) for path in before}
    missing_from_census = sorted(set(report.get("objects", {})) - census_keys)
    result["provenance"] = {"project_root": str(root), "original_sha256": digest(original),
        "retail_path": str(original), "symbols_path": str(symbols),
        "objects_path": str(objects), "sources_path": str(sources),
        "map_sha256": input_hashes[str(symbols)], "source_snapshots_stable": True,
        "source_snapshot_count": len(source_hashes), "objects": len(before),
        "objectset_sha256": hashlib.sha256(json.dumps(before, sort_keys=True).encode()).hexdigest(),
        "objects_stable": True, "report_sha256": hashlib.sha256(report_raw).hexdigest() if report_raw is not None else None,
        "objects_not_bound_to_report": stale,
        "report_objects_missing_from_census": missing_from_census,
        "census_complete_for_report": bool(report.get("objects")) and not stale and not missing_from_census,
        "source_leads_qualification": "Hash-bound source tokens; preprocessor reachability is not established."}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n")
    print(f"{len(result['roots'])} bounded roots; {len(stale)} objects not bound to report; output {args.output}")


if __name__ == "__main__":
    main()
