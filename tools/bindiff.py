"""Read optional BinDiff evidence without promoting matches to accepted identities."""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import sqlite3


def import_candidates(
    database: Path, *, primary_offset: int = 0, secondary_offset: int = 0
) -> dict:
    """Normalize analyzed addresses by explicitly supplied, validated rebases.

    Offsets are subtracted. The caller must verify binary identities and offsets
    against its graph manifests before combining this evidence with other channels.
    BinDiff's selected assignment does not supply competing identity alternatives.
    """
    database = Path(database).resolve()
    with sqlite3.connect(database.as_uri() + "?mode=ro", uri=True) as connection:
        connection.row_factory = sqlite3.Row
        metadata = [dict(row) for row in connection.execute("SELECT * FROM metadata")]
        files = [dict(row) for row in connection.execute("SELECT * FROM file ORDER BY id")]
        pairs = []
        query = """
            SELECT f.address1, f.name1, f.address2, f.name2,
                   f.similarity, f.confidence, a.name AS algorithm
            FROM function f LEFT JOIN functionalgorithm a ON f.algorithm = a.id
            ORDER BY f.address1, f.address2
        """
        for row in connection.execute(query):
            primary = row["address1"] - primary_offset
            secondary = row["address2"] - secondary_offset
            if primary < 0 or secondary < 0:
                raise ValueError("Address normalization produced a negative address")
            pairs.append(
                {
                    "primary_address": hex(primary),
                    "secondary_address": hex(secondary),
                    "primary_analyzed_address": hex(row["address1"]),
                    "secondary_analyzed_address": hex(row["address2"]),
                    "primary_name": row["name1"],
                    "secondary_name": row["name2"],
                    "similarity": row["similarity"],
                    "confidence": row["confidence"],
                    "algorithm": row["algorithm"],
                    "status": "candidate",
                    "evidence_channel": "bindiff",
                }
            )
    return {
        "schema_version": 1,
        "evidence_channel": "bindiff",
        "database_sha256": hashlib.sha256(database.read_bytes()).hexdigest(),
        "metadata": metadata,
        "files": files,
        "address_normalization": {
            "primary_subtract": hex(primary_offset),
            "secondary_subtract": hex(secondary_offset),
            "validation_required": "Compare identities and rebases with graph manifests",
        },
        "limitations": [
            "Scores are independent BinDiff evidence, not identity/ABI/extent acceptance",
            "Selected matches do not represent a complete alternative shortlist",
            "Ghidra names may contain map-derived labels; holdout evaluation must remove them",
        ],
        "pairs": pairs,
    }


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("database", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--primary-offset", type=lambda value: int(value, 0), default=0)
    parser.add_argument("--secondary-offset", type=lambda value: int(value, 0), default=0)
    args = parser.parse_args()
    result = import_candidates(
        args.database,
        primary_offset=args.primary_offset,
        secondary_offset=args.secondary_offset,
    )
    # Exclusive creation preserves existing evidence snapshots.
    with args.output.open("x", encoding="utf-8") as output:
        json.dump(result, output, indent=2, sort_keys=True)
        output.write("\n")


if __name__ == "__main__":
    main()
