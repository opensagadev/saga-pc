#!/usr/bin/env bash
# Run the source-shape search indefinitely and push results to main.
#
#   tools/permute_loop.sh                 # uses all cores
#   WORKERS=4 JOBS=4 CHUNK=20 tools/permute_loop.sh
#
# Each round pulls main (picking up tool changes), searches the next CHUNK
# functions losing the most bytes, applies improvements to src/, and commits
# and pushes them with the batch journal. When every function has been tried,
# it re-measures with `make match`, commits matching.json and starts a new pass
# with another seed. Stop with Ctrl-C; rerunning resumes from the journal.
set -u
cd "$(dirname "$0")/.."
BRANCH=${BRANCH:-main}
# One worker (function) per four cores, each compiling up to JOBS candidates at once.
CORES=$(nproc)
WORKERS=${WORKERS:-$(( CORES >= 8 ? CORES / 4 : 1 ))}
JOBS=${JOBS:-$(( (CORES + WORKERS - 1) / WORKERS ))}
CHUNK=${CHUNK:-$(( WORKERS * 4 ))}
ITERATIONS=${ITERATIONS:-300}
MAX_SIZE=${MAX_SIZE:-8000}
JOURNAL=build/permute/batch.jsonl

mkdir -p build/permute permute
log() { printf '%s %s\n' "$(date '+%F %T')" "$*" | tee -a build/permute/loop.log; }

python3 -c 'import clang.cindex' 2>/dev/null || python3 -m pip install --user libclang
if [ -n "$(git status --porcelain --untracked-files=no)" ]; then
    echo "Commit or stash tracked changes first." >&2
    exit 1
fi
git checkout -q "$BRANCH"
# Initialize the build's Wine prefix once, before workers compile in parallel.
make build/tools/wine/.ready >/dev/null
log "starting: $WORKERS workers x $JOBS compile jobs on $CORES cores"

sync() {
    # A conflicting pull is skipped; the next round tries again.
    if ! git pull -q --no-rebase --no-edit origin "$BRANCH" >/dev/null 2>&1; then
        git merge --abort 2>/dev/null
        log "pull of $BRANCH conflicted; continuing without it"
    fi
}

publish() {
    cp "$JOURNAL" permute/batch.jsonl 2>/dev/null
    echo "$SEED" > permute/seed
    git add -A src permute matching.json README.md
    git diff --cached --quiet && return
    git commit -q -m "$1"
    for delay in 2 4 8 16; do
        git push -q origin "HEAD:$BRANCH" && return
        sleep "$delay"
        sync
    done
    log "push failed; will retry next round"
}

sync
[ -f permute/batch.jsonl ] && [ ! -s "$JOURNAL" ] && cp permute/batch.jsonl "$JOURNAL"
touch "$JOURNAL"
SEED=$(cat permute/seed 2>/dev/null || echo 1)

while true; do
    sync
    before=$(wc -l < "$JOURNAL")
    python3 tools/permute.py --batch "$CHUNK" --iterations "$ITERATIONS" --jobs "$JOBS" --workers "$WORKERS" \
        --max-size "$MAX_SIZE" --seed "$SEED" --apply 2>&1 | tee -a build/permute/loop.log
    after=$(wc -l < "$JOURNAL")
    summary=$(tail -n $((after - before)) "$JOURNAL" | python3 -c '
import json, sys
rows = [json.loads(l) for l in sys.stdin if l.strip()]
gain = sum(r["size"] * (r["best"] - r["start"]) / 100 for r in rows if "best" in r)
exact = sum(1 for r in rows if r.get("exact"))
print(f"{len(rows)} functions, +{gain:.0f} weighted bytes, {exact} exact")')
    log "round: $summary"
    publish "Source-shape search: $summary"
    if [ "$after" -eq "$before" ]; then
        log "pass $SEED finished; re-measuring"
        if make -j"$CORES" match >> build/permute/loop.log 2>&1; then
            mv "$JOURNAL" "build/permute/batch-pass$SEED.jsonl"
            SEED=$((SEED + 1))
            : > "$JOURNAL"
            publish "Re-measure after source-shape pass $((SEED - 1))"
        else
            log "make match failed; see build/permute/loop.log. Retrying in 10 minutes"
            sleep 600
        fi
    fi
done
