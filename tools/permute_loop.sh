#!/usr/bin/env bash
# Run the source-shape search indefinitely and publish results to a git branch.
#
#   tools/permute_loop.sh                 # uses all cores, pushes to permute/<host>
#   JOBS=16 CHUNK=20 tools/permute_loop.sh
#
# Each round merges the latest tool branch (UPSTREAM), searches the next CHUNK
# functions losing the most bytes, applies improvements to src/, and commits and
# pushes them with the batch journal. When every function has been tried, it
# re-measures with `make match`, commits matching.json and starts a new pass with
# another seed. Stop with Ctrl-C; rerunning resumes from the journal.
set -u
cd "$(dirname "$0")/.."
UPSTREAM=${UPSTREAM:-claude/awesome-ptolemy-jug22a}
BRANCH=${BRANCH:-permute/$(hostname -s)}
JOBS=${JOBS:-$(nproc)}
CHUNK=${CHUNK:-10}
ITERATIONS=${ITERATIONS:-300}
MAX_SIZE=${MAX_SIZE:-8000}
JOURNAL=build/permute/batch.jsonl

log() { printf '%s %s\n' "$(date '+%F %T')" "$*" | tee -a build/permute/loop.log; }

python3 -c 'import clang.cindex' 2>/dev/null || python3 -m pip install --user libclang
mkdir -p build/permute permute
if [ -n "$(git status --porcelain --untracked-files=no)" ]; then
    echo "Commit or stash tracked changes first." >&2
    exit 1
fi
git fetch -q origin "$UPSTREAM"
if git ls-remote --exit-code -q origin "$BRANCH" >/dev/null; then
    git fetch -q origin "$BRANCH" && git checkout -q -B "$BRANCH" "origin/$BRANCH"
else
    git checkout -q -B "$BRANCH" "origin/$UPSTREAM"
fi
[ -f permute/batch.jsonl ] && [ ! -f "$JOURNAL" ] && cp permute/batch.jsonl "$JOURNAL"
SEED=$(cat permute/seed 2>/dev/null || echo 1)

publish() {
    cp "$JOURNAL" permute/batch.jsonl 2>/dev/null
    echo "$SEED" > permute/seed
    git add -A src permute matching.json README.md
    if ! git diff --cached --quiet; then
        git commit -q -m "$1"
        for delay in 2 4 8 16; do git push -q -u origin "$BRANCH" && break; sleep "$delay"; done
    fi
}

while true; do
    # Pick up tool fixes; a conflicting merge is skipped this round.
    if git fetch -q origin "$UPSTREAM" && ! git merge -q --no-edit "origin/$UPSTREAM" >/dev/null 2>&1; then
        git merge --abort 2>/dev/null
        log "merge of $UPSTREAM conflicted; continuing without it"
    fi
    before=$(wc -l < "$JOURNAL" 2>/dev/null || echo 0)
    python3 tools/permute.py --batch "$CHUNK" --iterations "$ITERATIONS" --jobs "$JOBS" \
        --max-size "$MAX_SIZE" --seed "$SEED" --apply 2>&1 | tee -a build/permute/loop.log
    after=$(wc -l < "$JOURNAL" 2>/dev/null || echo 0)
    summary=$(tail -n $((after - before)) "$JOURNAL" 2>/dev/null | python3 -c '
import json, sys
rows = [json.loads(l) for l in sys.stdin if l.strip()]
gain = sum(r["size"] * (r["best"] - r["start"]) / 100 for r in rows if "best" in r)
exact = sum(1 for r in rows if r.get("exact"))
print(f"{len(rows)} functions, +{gain:.0f} weighted bytes, {exact} exact")')
    log "round: $summary"
    publish "Source-shape search: $summary"
    if [ "$after" -eq "$before" ]; then
        log "pass $SEED finished; re-measuring"
        if make -j"$JOBS" match >> build/permute/loop.log 2>&1; then
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
