"""Joint graph research orchestration using the existing identification scorer.

The run context owns snapshots and atomic publication. No output here accepts a
symbol binding, and previous tentative state is never used as a new anchor.
"""
from __future__ import annotations

import copy
import csv
import json
from collections import defaultdict
from pathlib import Path

if __package__:
    from . import identification_graph as graph
    from . import identification_review as review
    from . import identification_state as persistent_state
    from . import lib
else:
    import identification_graph as graph
    import identification_review as review
    import identification_state as persistent_state
    import lib


def _json(path, value):
    with Path(path).open("w") as stream:
        json.dump(value, stream, sort_keys=True, indent=2)
        stream.write("\n")


def _csv(path, rows, fields):
    with Path(path).open("w", newline="") as stream:
        writer = csv.DictWriter(stream, fieldnames=fields, extrasaction="ignore")
        writer.writeheader()
        for row in rows:
            writer.writerow({key: json.dumps(row[key], sort_keys=True) if isinstance(row[key], (dict, list)) else row[key]
                             for key in fields if key in row})


def _tentative_data_exports(directory, rows):
    """Scalar filtering plus byte-addressable full lineage, without a second encoding."""
    fields = ["pc", "android", "pc_address", "android_address", "rank", "score", "supports",
              "evidence_users_total", "seed_function_users", "inferred_function_users",
              "accepted_function_root_count", "reviewed_data_root_count", "mutual_top",
              "android_margin", "pc_margin", "pc_extent", "pc_analysis_extent", "android_extent",
              "boundary_status", "status", "eligible_as_reviewed_anchor", "channel",
              "evidence_record", "evidence_byte_offset", "evidence_byte_length"]

    def records():
        with (directory / "tentative-function-data-candidates.jsonl").open("wb") as stream:
            for index, row in enumerate(rows, 1):
                offset = stream.tell()
                raw = (json.dumps(row, sort_keys=True) + "\n").encode("utf-8")
                stream.write(raw)
                scalar = {key: row[key] for key in fields if key in row}
                observations = [item["function_observation"]["status"] for item in row["evidence"]]
                scalar.update(seed_function_users=observations.count("seed"),
                              inferred_function_users=observations.count("inferred-mutual-top"),
                              accepted_function_root_count=sum(root.startswith("function:") for root in row["support_roots"]),
                              reviewed_data_root_count=sum(root.startswith("reviewed-data:") for root in row["support_roots"]),
                              evidence_record=index, evidence_byte_offset=offset, evidence_byte_length=len(raw))
                yield scalar

    _csv(directory / "tentative-function-data-candidates.csv", records(), fields)


def _register_pair(run, path):
    path = run.add_raw_input("joint_graph_pair", path)
    doc = json.loads(path.read_text())
    for side in ("pc", "android"):
        record = doc[side]
        filename = record if isinstance(record, str) else record["manifest"]
        manifest_path = run.add_raw_input("joint_" + side + "_manifest", path.parent / filename)
        manifest = json.loads(manifest_path.read_text())
        for collection in ("nodes", "edges"):
            run.add_raw_input("joint_" + side + "_" + collection,
                              manifest_path.parent / manifest.get(collection + "_file", collection + ".jsonl"))
        supplied = doc.get(side + "_binary")
        if supplied:
            original = path.parent / supplied
        elif side == "pc":
            original = run.raw_inputs.get("retail", lib.ROOT / run.target["exe"])
        else:
            original = lib.ROOT.parent / "saga/res/libTTapp.so"
        run.add_raw_input("joint_" + side + "_binary", original)
        if lib.sha256(original) != manifest["binary_sha256"]:
            raise ValueError(f"{side} graph does not describe the supplied original")
        if side == "pc":
            # Pair paths may relocate the supported original, not change its
            # identity or configured image boundaries.
            lib.retail({**run.target, "exe": str(original.resolve())})
    for filename in ("identification_graph.py", "identification_joint.py", "identification.py", "identification_review.py", "identification_state.py"):
        run.add_raw_input("tool_" + filename, Path(__file__).with_name(filename))
    pair = graph.load_graph_pair(path)
    for side in ("pc", "android"):
        if hasattr(pair[side], "index_path"):
            run.add_raw_input("joint_" + side + "_index", pair[side].index_path)
    return pair


def _map_data_pairs(pair, symbol_rows, scorer):
    """Unique exact compatible accepted-map/ELF names, preserving object aliases."""
    raw = []
    for node in pair["android"].data.values():
        if node["kind"] != "data-object":
            continue
        for alias in node.get("aliases", []):
            raw.append((node["id"], alias.get("name", "") if isinstance(alias, dict) else alias))
    pretty = scorer.demangle([name for _, name in raw])
    android_by_name = defaultdict(set)
    for (identifier, _), name in zip(raw, pretty):
        android_by_name[scorer.base_name(name)].add(identifier)
    pc_by_name = defaultdict(set)
    rows = [row for row in symbol_rows if row["address"] is not None
            and row["address"] not in pair["pc"].functions]
    pc_pretty = scorer.demangle([row["symbol"] for row in rows])
    for row, name in zip(rows, pc_pretty):
        try:
            pc_identifier = pair["pc"].resolve(row["address"], "data")
        except ValueError:
            continue
        pc_by_name[scorer.base_name(name)].add(pc_identifier)
    records = []
    for name in sorted(set(android_by_name) & set(pc_by_name)):
        if len(android_by_name[name]) == len(pc_by_name[name]) == 1:
            records.append({"pc": next(iter(pc_by_name[name])), "android": next(iter(android_by_name[name])),
                            "kind": "data", "origin": "accepted-map-unique-name", "name": name})
    return [{"pc": p, "android": a, "kind": "data", "origin": "accepted-map-unique-name"}
            for p, a in graph.normalize_pairs(pair, records, "data")]


def _function_inputs(pair, scorer, source_graph=None):
    inputs = graph.scorer_inputs(pair)
    android = inputs["android"]
    keys = sorted(android)
    pretty = scorer.demangle([android[va]["raw_name"] for va in keys])
    for va, name in zip(keys, pretty):
        android[va]["name"] = name
        android[va]["source_name"] = scorer.base_name(name)
        android[va]["source_type"] = name[name.find("("):] if "(" in name else ""
    source_features, source_meta = scorer.source_overlay(android, source_graph)
    # AST data names are not direct cross-platform correspondences. Only actual
    # paired graph data may create a data channel; literals need a PC counterpart.
    source_meta["named_data_tokens_used"] = False
    source_meta["string_tokens_used"] = False
    a_out, a_in = scorer.adjacency_for_android(android)
    inputs["android_out"], inputs["android_in"] = a_out, a_in
    return inputs, source_meta


def _merge_pairs(pair, seeds, journal):
    combined = dict(seeds)
    for row in journal:
        pc, aa = graph.normalize_pairs(pair, [row])[0]
        pva, ava = pair["pc"].nodes[pc]["address"], pair["android"].nodes[aa]["address"]
        if pva in combined and combined[pva] != ava:
            raise ValueError("Reviewed function decision conflicts with accepted-map seed")
        if ava in combined.values() and combined.get(pva) != ava:
            raise ValueError("Reviewed function identity conflicts with another accepted seed")
        combined[pva] = ava
    return combined


def _reviewed_data_components(pair, data_pairs):
    """One independent observation per connected bipartite alias component."""
    rows = graph.normalize_pairs(pair, data_pairs, "data")
    parent = {}

    def find(node):
        parent.setdefault(node, node)
        while parent[node] != node:
            parent[node] = parent[parent[node]]
            node = parent[node]
        return node

    for pc, aa in rows:
        left, right = find(("pc", pc)), find(("android", aa))
        if left != right:
            parent[max(left, right)] = min(left, right)
    owners = defaultdict(list)
    for pc, aa in rows:
        owners[find(("pc", pc))].append(pc)
    # Preserve historical singleton and one-PC component labels.
    labels = {key: "reviewed-data:" + min(values) for key, values in owners.items()}
    return {(pc, aa): labels[find(("pc", pc))] for pc, aa in rows}


def _function_retrieval(pair, inputs, seeds, mapped, data_pairs, rejected, scorer, capture_round1=True,
                        max_rounds=None):
    android, pc = inputs["android"], inputs["pc"]
    pairs, root_by_aa = dict(seeds), {}
    wrapper_aliases = inputs.get("_wrapper_root_aliases", {})
    pc_by_aa = {aa: pc for pc, aa in seeds.items()}
    data_components = _reviewed_data_components(pair, data_pairs)
    for pva, ava in seeds.items():
        root_aa = wrapper_aliases.get(ava, ava)
        root_pc = pc_by_aa[root_aa]
        root_by_aa[ava] = {f"function:{pair['pc'].resolve(root_pc, 'function')}:{pair['android'].resolve(root_aa, 'function')}"}
    rejected_va = {(pair["pc"].nodes[p]["address"], pair["android"].nodes[a]["address"])
                   for p, a in rejected if pair["pc"].nodes[p]["kind"] == pair["android"].nodes[a]["kind"] == "function"}
    proposals, rounds, round1 = [], [], None

    def score_current():
        support = scorer.score_round(android, pc, pairs, set(seeds), inputs["android_out"], inputs["android_in"],
                                     inputs["pc_out"], inputs["pc_in"], mapped, inputs.get("android_features"), inputs.get("pc_features"))
        external = graph.reviewed_data_function_support(pair, data_pairs, pairs,
                                                        reserved_pc=[va for va in mapped if va in pc], rejected=rejected)
        for aa, values in external.items():
            for pva, evidence_rows in values.items():
                if pva in mapped or (pva, aa) in rejected_va:
                    continue
                evidence = support.setdefault(aa, {}).setdefault(pva, {
                    "score": 0.0, "supports": 0, "seed_supports": 0,
                    "in_supports": 0, "out_supports": 0, "support_rows": []})
                weights = {}
                for row in evidence_rows:
                    component = data_components[(row["pc_data"], row["android_data"])]
                    weights[component] = max(weights.get(component, 0.0), row["weight"])
                evidence["score"] += sum(weights.values())
                evidence["reviewed_data_evidence"] = evidence_rows
        for aa, values in list(support.items()):
            for pva, evidence in list(values.items()):
                if (pva, aa) in rejected_va:
                    del values[pva]
                    continue
                if wrapper_aliases:
                    if __package__:
                        from .identification_wrappers import deduplicate_score
                    else:
                        from identification_wrappers import deduplicate_score
                    deduplicate_score(evidence, wrapper_aliases)
                    evidence["seed_supports"] = len({wrapper_aliases.get(neighbor, neighbor)
                                                    for direction, neighbor, pc_neighbor, _ in evidence["support_rows"]
                                                    if direction != "feature" and pc_neighbor in seeds})
                roots = set()
                for direction, neighbor, _, _ in evidence["support_rows"]:
                    if direction == "feature":
                        roots.add("literal:" + neighbor)
                    else:
                        roots.update(root_by_aa.get(neighbor, ()))
                roots.update(data_components[(row["pc_data"], row["android_data"])]
                             for row in evidence.get("reviewed_data_evidence", []))
                evidence["support_roots"] = sorted(roots)
                evidence["supports"] = len(roots)
        return support

    round_limit = scorer.MAX_ROUNDS if max_rounds is None else max_rounds
    for iteration in range(1, round_limit + 1):
        support = score_current()
        ranked, reverse = scorer.ranked_candidates(support)
        if iteration == 1 and capture_round1:
            round1 = ranked
        added = []
        for pva, aa, evidence, am, pm in scorer.eligible_propagations(ranked, reverse):
            if pva in pairs or aa in pairs.values():
                continue
            pairs[pva] = aa
            root_by_aa[aa] = set(evidence["support_roots"])
            options = ranked[aa]
            row = {"pc": pair["pc"].resolve(pva, "function"), "android": pair["android"].resolve(aa, "function"),
                   "pc_address": pva, "android_address": aa, "kind": "function",
                   "source_name": android[aa]["source_name"], "status": "inferred-mutual-top", "round": iteration,
                   "score": evidence["score"], "rank": 1, "supports": evidence["supports"],
                   "support_roots": evidence["support_roots"], "evidence": evidence,
                   "android_margin": am, "pc_margin": pm,
                   "alternatives": [{"pc_address": va, "score": item["score"]} for va, item in options[:5]],
                   **scorer.boundary_fields(pc[pva])}
            # boundary_fields emits formatted pc_address for historical CSV;
            # retain numeric addresses throughout joint evidence/state APIs.
            row["pc_address"] = pva
            proposals.append(row)
            added.append((pva, aa))
        rounds.append({"iteration": iteration, "new_pairs": len(added)})
        if not added:
            break
    final, reverse = scorer.ranked_candidates(score_current())
    rows = list(proposals)
    for aa, values in sorted(final.items()):
        for rank, (pva, evidence) in enumerate(values[:5], 1):
            row = {"pc": pair["pc"].resolve(pva, "function"), "android": pair["android"].resolve(aa, "function"),
                   "pc_address": pva, "android_address": aa, "kind": "function", "source_name": android[aa]["source_name"],
                   "status": "candidate", "round": None, "rank": rank, "score": evidence["score"],
                   "supports": evidence["supports"], "support_roots": evidence["support_roots"], "evidence": evidence,
                   "alternatives": [{"pc_address": va, "score": item["score"]} for va, item in values[:5]],
                   **scorer.boundary_fields(pc[pva])}
            # These are analysis diagnostics, not an original extent review.
            # Keep addresses numeric for joint evidence/state APIs.
            row["pc_address"] = pva
            rows.append(row)
    return {"rows": rows, "proposals": proposals, "rounds": rounds, "round1": round1 or {},
            "propagation": {"max_rounds": round_limit, "rounds_completed": len(rounds),
                            "stop_reason": "no-new-pairs" if rounds and not rounds[-1]["new_pairs"] else "round-limit"}}


def _evaluate_functions(result, held, pair):
    expected = {pair["android"].nodes[row["android"]]["address"]: pair["pc"].nodes[row["pc"]]["address"] for row in held}
    connected = top1 = top5 = 0
    for aa, pva in expected.items():
        options = result["round1"].get(aa, [])
        connected += bool(options)
        top1 += bool(options and options[0][0] == pva)
        top5 += pva in [va for va, _ in options[:5]]
    predictions = [row for row in result["proposals"] if row["android_address"] in expected]
    return {"held_out": len(expected), "round1_connected": connected, "round1_top1_correct": top1,
            "round1_top5_correct": top5, "held_out_proposed": len(predictions),
            "held_out_correct": sum(row["pc_address"] == expected[row["android_address"]] for row in predictions),
            "predictions": [{"pc_address": row["pc_address"], "android_address": row["android_address"],
                             "expected_pc_address": expected[row["android_address"]], "round": row["round"],
                             "correct": row["pc_address"] == expected[row["android_address"]]} for row in predictions]}


def _evaluate_data(rows, held):
    by_aa = defaultdict(list)
    for row in rows:
        by_aa[row["android"]].append(row)
    connected = top1 = top5 = proposed = correct = 0
    predictions = []
    for target in held:
        options = sorted(by_aa[target["android"]], key=lambda row: row["rank"])
        connected += bool(options)
        top1 += bool(options and options[0]["pc"] == target["pc"])
        top5 += target["pc"] in [row["pc"] for row in options[:5]]
        if options and options[0]["mutual_top"] and options[0]["android_margin"] >= 0.08 and options[0]["pc_margin"] >= 0.08:
            proposed += 1
            correct += options[0]["pc"] == target["pc"]
            predictions.append({"pc": options[0]["pc"], "android": target["android"], "expected_pc": target["pc"],
                                "correct": options[0]["pc"] == target["pc"]})
    return {"held_out": len(held), "connected": connected, "top1_correct": top1, "top5_correct": top5,
            "held_out_proposed": proposed, "held_out_correct": correct, "predictions": predictions}


def _similarity_evaluation(run, paths, selection, result, pair):
    if __package__:
        from . import bsim
    else:
        import bsim
    paths = paths if isinstance(paths, (list, tuple)) else [paths]
    channels, warnings, independent_metrics = {}, [], {}
    for index, supplied in enumerate(paths):
        path = run.add_raw_input(f"similarity_candidates_{index}", supplied)
        payload = json.loads(path.read_text())
        if payload.get("schema") == bsim.SCHEMA:
            bsim.validate_candidates(payload, pair["pc"].manifest["binary_sha256"],
                                     pair["android"].manifest["binary_sha256"])
            channel = "bsim" if "bsim" not in channels else f"bsim-{index}"
            channels[channel] = {bsim.address(query["pc_address"]): {
                "status": query["status"], "addresses": list(dict.fromkeys(bsim.address(row["android_address"]) for row in query["candidates"]))}
                                 for query in payload["queries"]}
            independent_metrics[channel] = bsim.evaluate(payload, selection)
        elif payload.get("evidence_channel") == "bindiff" and payload.get("schema_version") == 1:
            files = {row["id"]: row for row in payload.get("files", [])}
            metadata = payload.get("metadata", [])
            if not metadata:
                raise ValueError("BinDiff evidence lacks original file identities")
            primary, secondary = files.get(metadata[0].get("file1")), files.get(metadata[0].get("file2"))
            if not primary or not secondary or primary.get("hash") != pair["pc"].manifest["binary_sha256"] or secondary.get("hash") != pair["android"].manifest["binary_sha256"]:
                raise ValueError("BinDiff original identities disagree with graph pair")
            normalization = payload.get("address_normalization", {})
            pdelta, adelta = graph.address(normalization["primary_subtract"]), graph.address(normalization["secondary_subtract"])
            if pdelta != -pair["pc"].manifest["address_transform"]["delta"] or adelta != -pair["android"].manifest["address_transform"]["delta"]:
                raise ValueError("BinDiff normalization disagrees with original graph addresses")
            channel = "bindiff" if "bindiff" not in channels else f"bindiff-{index}"
            matches = defaultdict(list)
            for row in payload.get("pairs", []):
                pva, ava = graph.address(row["primary_address"]), graph.address(row["secondary_address"])
                if graph.address(row["primary_analyzed_address"]) - pdelta != pva or graph.address(row["secondary_analyzed_address"]) - adelta != ava:
                    raise ValueError("BinDiff analyzed and normalized addresses disagree")
                if pva not in pair["pc"].functions or ava not in pair["android"].functions:
                    warnings.append({"channel": channel, "pc_address": pva, "android_address": ava, "reason": "entry absent from supplied graph"})
                    continue
                for field in ("similarity", "confidence"):
                    value = row[field]
                    if not isinstance(value, (int, float)) or not 0 <= value <= 1:
                        raise ValueError("Invalid BinDiff similarity/confidence")
                matches[pva].append((ava, row["similarity"], row["confidence"]))
            channels[channel] = {pva: {"status": "ok", "addresses": list(dict.fromkeys(item[0] for item in sorted(values, key=lambda item: (-item[1], -item[2], item[0]))))}
                                 for pva, values in sorted(matches.items())}
        else:
            raise ValueError("Unsupported similarity candidate schema; expected validated BSim or BinDiff export")
    # PC queries require reverse ranks across the entire round-one candidate
    # matrix, not Android-top-five rows transposed after pruning.
    graph_by_pc = defaultdict(list)
    for ava, values in result["round1"].items():
        for pva, evidence in values:
            graph_by_pc[pva].append((ava, evidence["score"]))
    metrics, records = {}, []
    for query in selection["queries"]:
        pva = graph.address(query["pc_address"])
        grows = [aa for aa, _ in sorted(graph_by_pc[pva], key=lambda item: (-item[1], item[0]))]
        ranked = {"graph": grows}
        statuses = {"graph": "ok" if grows else "no_candidates"}
        for channel, by_pc in channels.items():
            evidence = by_pc.get(pva, {"status": "missing_query", "addresses": []})
            ranked[channel], statuses[channel] = evidence["addresses"], evidence["status"]
        combined = []
        ordered_channels = ["graph", *sorted(channels)]
        for index in range(max((len(values) for values in ranked.values()), default=0)):
            for channel in ordered_channels:
                values = ranked[channel]
                if index < len(values) and values[index] not in combined:
                    combined.append(values[index])
        ranked["combined"] = combined
        statuses["combined"] = "ok" if combined else "no_candidates"
        records.append({"pc_address": pva, "ranked": {key: values[:10] for key, values in ranked.items()}, "statuses": statuses})
        if query.get("android_address") is None:
            continue
        expected = graph.address(query["android_address"])
        groups = ["known_all", "family:" + query.get("family", "unclassified"),
                  "known_tiny" if query.get("pc_bytes", 0) <= 32 else "known_nontiny"]
        for channel, values in ranked.items():
            for group in groups:
                counts = metrics.setdefault(channel, {}).setdefault(group, {"queries": 0, "top1": 0, "top5": 0, "top10": 0, "no_candidates": 0, "failures": 0})
                counts["queries"] += 1
                counts["top1"] += bool(values and values[0] == expected)
                counts["top5"] += expected in values[:5]
                counts["top10"] += expected in values[:10]
                counts["no_candidates"] += not bool(values)
                counts["failures"] += statuses[channel] not in {"ok", "no_candidates"}
    return {"schema": "identification-similarity-evaluation-v1", "metrics": metrics,
            "independent_evaluations": independent_metrics, "queries": records,
            "out_of_graph_entries": warnings,
            "combination": "round-robin rank interleaving; scores remain separate; no candidate promotion"}


def _generate_joint(run, joint_graph, holdout=False, holdout_mode="pair", holdout_selection=None,
                    similarity_candidates=None, pair=None, registry_descriptors=None, max_rounds=None, tentative_data_feedback=False, reviewed_wrapper_anchors=None):
    """Generate ignored research outputs; caller publishes through IdentificationRun."""
    if __package__:
        from . import identification as scorer
        from .binary import PE
    else:
        import identification as scorer
        from binary import PE
    if pair is None:
        pair = _register_pair(run, joint_graph)
    registry_metadata = {"enabled": False, "reason": "no reviewed registry descriptors supplied"}
    if registry_descriptors is not None:
        if __package__:
            from .identification_registry_run import diagnose_registry
        else:
            from identification_registry_run import diagnose_registry
        registry_metadata = diagnose_registry(run, pair, registry_descriptors)
    inputs, source_meta = _function_inputs(pair, scorer, run.source_graph)
    literal_metadata = {"enabled": False}
    literal_module = Path(__file__).with_name("identification_literals.py")
    if literal_module.exists():
        if __package__:
            from .identification_literals import literal_features
        else:
            from identification_literals import literal_features
        run.add_raw_input("tool_identification_literals.py", literal_module)
        literals = literal_features(pair, {side: run.raw_inputs["joint_" + side + "_binary"] for side in ("pc", "android")})
        inputs["pc_features"], inputs["android_features"] = literals["pc"], literals["android"]
        literal_metadata = literals["metadata"]
        reader_path = literal_metadata.get("sides", {}).get("android", {}).get("reader")
        if reader_path == "tools.binary.PE":
            reader_path = None
        if reader_path:
            run.add_raw_input("android_literal_reader", reader_path)
    mapped = {row["address"]: row for row in run.symbol_rows if row["address"] is not None}
    names_by_pc = defaultdict(set)
    for row in run.symbol_rows:
        if row["address"] is not None:
            names_by_pc[row["address"]].add(scorer.base_name(row["symbol"]))
    ambiguous = {va for va, names in names_by_pc.items() if len(names) > 1}
    seeds = scorer.unique_name_seeds(inputs["android"], inputs["pc"], mapped, ambiguous)
    journal = graph.load_decisions(run.reviewed_pairs, pair)
    seeds = _merge_pairs(pair, seeds, journal["function_pairs"])
    wrapper_metadata = None
    if reviewed_wrapper_anchors is not None:
        if __package__:
            from .identification_wrappers import load as load_wrappers
        else:
            from identification_wrappers import load as load_wrappers
        inputs, wrapper_metadata = load_wrappers(run, reviewed_wrapper_anchors, pair, inputs, seeds, journal)
        _json(run.work_dir / "reviewed-wrapper-paths.json", wrapper_metadata)
    map_data_labels = _map_data_pairs(pair, run.symbol_rows, scorer)
    data_pairs = [{"pc": p, "android": aa, "kind": "data"} for p, aa in graph.normalize_pairs(pair, journal["data_pairs"], "data")
                  if (p, aa) not in journal["rejected"]]
    data_labels = [{"pc": p, "android": aa, "kind": "data"} for p, aa in graph.normalize_pairs(pair, map_data_labels + data_pairs, "data")
                   if (p, aa) not in journal["rejected"]]
    reviewed_data_ids = set(graph.normalize_pairs(pair, data_pairs, "data"))
    for p, aa in journal["rejected"]:
        if pair["pc"].nodes[p]["kind"] == pair["android"].nodes[aa]["kind"] == "function":
            if seeds.get(pair["pc"].nodes[p]["address"]) == pair["android"].nodes[aa]["address"]:
                raise ValueError("Explicit rejection conflicts with accepted-map function seed")
    selection = None
    if holdout_selection is not None:
        selection_path = run.add_raw_input("holdout_selection", holdout_selection)
        selection = json.loads(selection_path.read_text())
        if "queries" in selection:
            pc_queries = [graph.address(row["pc_address"]) for row in selection["queries"]]
            if len(set(pc_queries)) != len(pc_queries):
                raise ValueError("Duplicate fixed evaluation query")
            for query in selection["queries"]:
                pva = graph.address(query["pc_address"])
                if query.get("android_address") is not None and seeds.get(pva) != graph.address(query["android_address"]):
                    raise ValueError("Fixed evaluation label disagrees with frozen map seed")
    rejected = journal["rejected"]
    reviewed_functions = set(graph.normalize_pairs(pair, journal["function_pairs"]))
    with (run.work_dir / "function-seeds.snapshot.jsonl").open("w") as stream:
        for pva, ava in sorted(seeds.items()):
            pc_id, aa_id = pair["pc"].resolve(pva, "function"), pair["android"].resolve(ava, "function")
            stream.write(json.dumps({"pc": pc_id, "android": aa_id, "pc_address": pva,
                                     "android_address": ava, "source_name": inputs["android"][ava]["source_name"],
                                     "origin": "reviewed-journal" if (pc_id, aa_id) in reviewed_functions else "accepted-map-unique-name",
                                     "status": "seed", "new_mapping": False}, sort_keys=True) + "\n")
    data_state, data_delta = graph.replay_state(pair, seeds, data_pairs, rejected,
                                               previous=run.previous_state,
                                               context={"map_sha256": run.inputs["symbols"]["sha256"],
                                                        "decisions_sha256": journal["fingerprint"]},
                                               reserved_data_pairs=data_labels)
    result = _function_retrieval(pair, inputs, seeds, mapped, data_pairs, rejected, scorer,
                                 capture_round1=False, max_rounds=max_rounds)
    data_rows = data_state["data_candidates"]
    tentative_data_rows = []
    if tentative_data_feedback:
        if __package__:
            from . import identification_tentative_data as tentative
        else:
            import identification_tentative_data as tentative
        run.add_raw_input("tool_identification_tentative_data.py", Path(tentative.__file__))
        tentative_data_rows = tentative.tentative_data_candidates(
            graph, pair, seeds, result["proposals"], data_pairs, rejected,
            reserved_data_pairs=data_labels)
        _tentative_data_exports(run.work_dir, tentative_data_rows)
    held_metrics = None
    evaluated_result = result
    if holdout or similarity_candidates is not None:
        if similarity_candidates is not None and (selection is None or "queries" not in selection):
            raise ValueError("Similarity comparison requires a fixed query selection")
        held_pc = selection.get("held_pc") if selection else None
        if selection and "queries" in selection:
            held_pc = [graph.address(query["pc_address"]) for query in selection["queries"] if query.get("android_address") is not None]
        families = selection.get("families") if selection else None
        held_families = selection.get("held_families") if selection else None
        if holdout_mode == "family" and families is None:
            families = {va: row["source_file"] for va, row in inputs["android"].items() if row.get("source_file")}
            if not families:
                raise ValueError("Family holdout requires a source overlay or explicit family selection")
            held_families = sorted(set(families.values()))[::4]
        split = graph.make_holdout(pair, seeds, data_labels, mode=holdout_mode, held_pc=held_pc,
                                   families=families, held_families=held_families,
                                   decisions=journal["records"], family_data_pairs=selection.get("family_data_pairs", ()) if selection else ())
        if holdout_mode == "family" and not any(row["kind"] == "function" for row in split["held"]):
            raise ValueError("Family selection hides no confirmed function labels; check family names")
        training_rejected = {graph.normalize_pairs(split["pair"], [row], row["kind"])[0]
                             for row in split["decisions"] if row["status"] == "rejected"}
        training_reviewed_data = [row for row in split["training_data_pairs"] if (row["pc"], row["android"]) in reviewed_data_ids]
        training_seeds = {split["pair"]["pc"].nodes[row["pc"]]["address"]: split["pair"]["android"].nodes[row["android"]]["address"]
                          for row in split["training_function_pairs"]}
        if selection and "queries" in selection:
            training_seeds = {pva: ava for pva, ava in training_seeds.items() if pva not in pc_queries}
        # Held function addresses must become candidates, not reserved inventory.
        held_function_vas = {pair["pc"].nodes[row["pc"]]["address"] for row in split["held"] if row["kind"] == "function"}
        reserved = {va: row for va, row in mapped.items() if va not in held_function_vas}
        if selection and "queries" in selection:
            # Unknown fixed queries also participate in retrieval; no accepted
            # PC mapping may reserve them behind the evaluation's back.
            reserved = {va: row for va, row in reserved.items() if va not in pc_queries}
        if holdout_mode == "data":
            held_rows = graph.score_data_candidates(split["pair"], training_seeds, training_reviewed_data, training_rejected,
                                                     reserved_data_pairs=split["training_data_pairs"])
            held_metrics = _evaluate_data(held_rows, split["held"])
        else:
            split_inputs = {**inputs, "pc": dict(inputs["pc"])}
            for va in held_function_vas:
                split_inputs["pc"][va] = {**inputs["pc"][va], "analysis_name": ""}
            evaluated_result = _function_retrieval(split["pair"], split_inputs, training_seeds, reserved,
                                                   training_reviewed_data, training_rejected, scorer,
                                                   max_rounds=max_rounds)
            held_metrics = _evaluate_functions(evaluated_result, split["held"], pair)
        held_metrics.update({"mode": holdout_mode, "training_function_seeds": len(training_seeds),
                             "training_data_anchors": len(training_reviewed_data),
                             "training_data_labels": len(split["training_data_pairs"]),
                             "training_rejections": len(training_rejected),
                             "alias_excluded_data_pairs": split.get("alias_excluded_data_pairs", []),
                             "warning": "Existing-label retrieval is biased; thresholds are not probability calibration.",
                             "label_channels": "Only training accepted pairs; no PC annotated-name tokens, inferred cache or accepted-symbol features."})
        _json(run.work_dir / "holdout.json", held_metrics)
    if similarity_candidates is not None:
        if holdout_mode == "data":
            raise ValueError("Function similarity cannot be evaluated as a data holdout")
        _json(run.work_dir / "similarity-evaluation.json",
              _similarity_evaluation(run, similarity_candidates, selection, evaluated_result, pair))
    fields = ["status", "round", "rank", "pc_address", "android_address", "source_name", "score", "supports",
              "support_roots", "alternatives", "pc_extent_bytes", "boundary_status"]
    _csv(run.work_dir / "hypotheses.csv", result["rows"], fields)
    _csv(run.work_dir / "data-candidates.csv", data_rows,
         ["status", "rank", "pc_address", "android_address", "score", "supports", "support_roots",
          "mutual_top", "android_margin", "pc_margin", "pc_extent", "pc_analysis_extent", "android_extent", "boundary_status", "alternatives"])
    # Both function and data hypotheses are replayed from confirmed roots on each
    # run; compare prior functions without making them seeds.
    previous_functions = (run.previous_state or {}).get("function_candidates", [])
    function_delta = persistent_state.candidate_delta(previous_functions, result["rows"])
    data_state["candidate_storage"] = persistent_state.CANDIDATE_STORAGE
    data_state["function_candidates"] = [persistent_state.compact_candidate(row) for row in result["rows"]]
    data_state["data_candidates"] = [persistent_state.compact_candidate(row) for row in data_rows]
    with (run.work_dir / "function-candidates.jsonl").open("w") as stream:
        for row in result["rows"]:
            json.dump(row, stream, sort_keys=True)
            stream.write("\n")
    with (run.work_dir / "data-candidates.jsonl").open("w") as stream:
        for row in data_rows:
            json.dump(row, stream, sort_keys=True)
            stream.write("\n")
    data_state["fingerprint"] = graph.canonical_hash({key: value for key, value in data_state.items() if key != "fingerprint"})
    _json(run.work_dir / "state.json", data_state)
    _json(run.work_dir / "delta.json", {"data": data_delta, "functions": function_delta})
    # Bound review artifact generation. Full candidate rows remain in JSONL/CSV;
    # state holds digests and a few packets supply immediate inspectable examples.
    packets = []
    for decision in journal["records"][:10]:
        statuses = {"identity": decision["status"], "abi": "reviewed" if decision.get("abi_reviewed") else "unknown",
                    "type": "reviewed" if decision.get("type_reviewed") else "unknown",
                    "extent": "reviewed" if decision.get("extent_reviewed") else "unknown",
                    "complete_ranges": decision.get("complete_ranges", []), "evidence": [decision["reason"]]}
        packets.append(review.build_evidence_packet(pair["pc"].nodes[decision["pc"]], pair["android"].nodes[decision["android"]],
                                                   pair["pc"], pair["android"], candidate={"status": decision["status"], "kind": decision["kind"]},
                                                   review=statuses, limit=100))
    for row in data_rows[:30] + result["proposals"][:20]:
        packets.append(review.build_evidence_packet(pair["pc"].nodes[row["pc"]], pair["android"].nodes[row["android"]],
                                                   pair["pc"], pair["android"], candidate=row, limit=100))
    with (run.work_dir / "evidence.jsonl").open("w") as stream:
        for packet in packets:
            json.dump(packet, stream, sort_keys=True)
            stream.write("\n")
    (run.work_dir / "review.md").write_text("# Joint identification review\n\nNo candidate is an accepted mapping.\n\n" +
                                         "\n".join(review.packet_markdown(packet) for packet in packets))
    tables_module = Path(__file__).with_name("identification_tables.py")
    if tables_module.exists():
        if __package__:
            from . import identification_tables as tables
        else:
            import identification_tables as tables
        run.add_raw_input("tool_identification_tables.py", tables_module)
        _json(run.work_dir / "tables.json", tables.table_candidates(pair, seeds, limit=100))
        _json(run.work_dir / "metadata.json", tables.metadata_candidates(pair, limit=100))
        _json(run.work_dir / "gaps.json", {side: tables.gap_inventory(pair[side], limit=200) for side in ("pc", "android")})
    pe = PE(run.raw_inputs["joint_pc_binary"])
    # A map's known size establishes an accepted inventory extent. Raw Ghidra
    # lengths and tentative candidates deliberately supply no reviewed coverage.
    entities = []
    for row in run.symbol_rows:
        if row["address"] is None:
            continue
        if not pe.image_base <= row["address"] < pe.image_base + pe.image_size:
            continue
        entities.append({"address": row["address"], "review": {
            "identity": "accepted", "extent": "reviewed" if row["size"] is not None else "unknown",
            "complete_ranges": [[row["address"], row["address"] + row["size"]]] if row["size"] is not None else []}})
    for decision in journal["records"]:
        if decision.get("stale") and decision["status"] != "rejected":
            continue
        entities.append({"id": decision["pc"], "address": pair["pc"].nodes[decision["pc"]]["address"],
                         "review": {"identity": decision["status"],
                                    "extent": "reviewed" if decision.get("extent_reviewed") else "unknown",
                                    "complete_ranges": decision.get("complete_ranges", [])}})
    _json(run.work_dir / "inventory.json", review.region_inventory(pe, entities))
    summary = {"mode": "joint-graph", "schema": "identification-joint-summary-v1",
               "pc_functions": len(pair["pc"].functions), "android_functions": len(pair["android"].functions),
               "pc_data_entities": len(pair["pc"].data), "android_data_entities": len(pair["android"].data),
               "unique_name_seeds": len(seeds), "reviewed_data_anchors": len(data_pairs),
               "map_name_data_labels": len(map_data_labels), "data_holdout_labels": len(data_labels),
               "data_candidate_pairs": len(data_rows), "function_candidate_rows": len(result["rows"]),
               "inferred_function_pairs": len(result["proposals"]), "iterations": result["rounds"],
               "propagation": result["propagation"],
               "stale_decisions": len(journal["stale"]), "rejections": len(rejected), "source_overlay": source_meta,
               "graph_fingerprints": {side: pair[side].fingerprint for side in ("pc", "android")},
               "holdout": held_metrics,
               "literal_features": literal_metadata,
               "registry_diagnostics": registry_metadata,
               "features": "direct calls, explicit journal-reviewed data users and map-independent literals when available; name-only data joins are labels, not propagation anchors; constant/initializer scoring not enabled",
               "warning": "Hypotheses never establish PC identity, type/ABI, complete extent or reconstruction coverage."}
    if tentative_data_feedback:
        summary["tentative_function_data_channel"] = {
            "enabled": True, "candidate_rows": len(tentative_data_rows),
            "score_version": "tentative-function-data-lineage-v1",
            "feeds_function_retrieval": False, "feeds_state": False}
    if wrapper_metadata is not None:
        summary["reviewed_wrapper_anchors"] = {"enabled": True, "paths": len(wrapper_metadata["paths"]), "held": len(wrapper_metadata["held"]), "raw_edges_unchanged": True, "new_independent_roots": 0}
    _json(run.work_dir / "summary.json", summary)
    run.validate_inputs()
    return summary


def generate_joint(run, joint_graph, holdout=False, holdout_mode="pair", holdout_selection=None,
                   similarity_candidates=None, registry_descriptors=None, max_rounds=None, tentative_data_feedback=False, reviewed_wrapper_anchors=None):
    """Own/close lazy original graph indices while the run owns publication."""
    if max_rounds is not None and (isinstance(max_rounds, bool) or not isinstance(max_rounds, int)
                                   or not 1 <= max_rounds <= 64):
        raise ValueError("max_rounds must be an integer between 1 and 64")
    if not isinstance(tentative_data_feedback, bool):
        raise ValueError("tentative_data_feedback must be boolean")
    if tentative_data_feedback and (holdout or similarity_candidates is not None):
        raise ValueError("Tentative data experiment requires a separate fixed training-only holdout design")
    if reviewed_wrapper_anchors is not None and (holdout or similarity_candidates is not None or tentative_data_feedback):
        raise ValueError("Wrapper anchors require a separate training-only run; holdout/evaluation feedback is unsupported")
    pair = _register_pair(run, joint_graph)
    try:
        return _generate_joint(run, joint_graph, holdout=holdout, holdout_mode=holdout_mode,
                               holdout_selection=holdout_selection, similarity_candidates=similarity_candidates,
                               pair=pair, registry_descriptors=registry_descriptors, max_rounds=max_rounds,
                               tentative_data_feedback=tentative_data_feedback,
                               reviewed_wrapper_anchors=reviewed_wrapper_anchors)
    finally:
        for side in ("pc", "android"):
            if isinstance(pair[side], graph.IndexedGraph):
                pair[side].close()
