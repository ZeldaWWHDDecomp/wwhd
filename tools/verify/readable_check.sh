#!/bin/sh
# readable_check.sh TARGET... [options]: acceptance check for a readability rewrite (see readable_check.py):
# function set + spec files unchanged vs the reference (--ref), verify.py at 10k seeds 1 and 7 on every spec
# that compiles the target's sources, coverage not below the reference; then the leftover lint (counts only).
# It runs specs one after another; running several checks at once multiplies the memory use.
HERE=$(cd "$(dirname "$0")" && pwd)
python3 "$HERE/readable_check.py" "$@"
rc=$?
srcs=""
for t in "$@"; do
  case "$t" in -*) ;; *.cpp|*.h) srcs="$srcs $t" ;; *) [ -f "$HERE/units/$t.txt" ] && srcs="$srcs $(awk '$1=="src"{for(i=2;i<=NF;i++)print $i}' "$HERE/units/$t.txt")" ;; esac
done
[ -n "$srcs" ] && python3 "$HERE/readable_lint.py" $srcs
exit $rc
