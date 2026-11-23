#!/bin/zsh
# pair60.sh EXE GAME SAVE OUT END [VAR=value ...]
# True60 paired-run: the same scripted scenario twice on one true60 decomp build, at 30 Hz logic (OUT/a30) and at N steps per
# 30 Hz frame (OUT/bN), then compare60.py with its "equivalent" gates. Each run is the port's run_scenario.sh (headless,
# muted, hidden windows, no host input, a copy of SAVE, traces trace.txt cam.txt se.txt rng.txt digest.txt si.bin log),
# plus WWHD_T60_STEPLOG=steps.txt (one line per logic step `step full dt`, when the build writes it).
# Inputs are scenario-time based (WWHD_TEST_STICK / WWHD_TEST_PRESS / WWHD_TEST_LOAD seconds after the origin; game
# time, not frames), so both runs see the same inputs; pass them as VAR=value arguments.
#
# Environment:
#   T60_STEPS     N of the B run (2: 60 Hz, default; 4: 120 Hz)
#   T60_ENV_A     the A run's step switch (default WWHD_T60_STEPS=1)
#   T60_ENV_B     the B run's step switch (default WWHD_T60_STEPS=$T60_STEPS; set it to what runtime reads, e.g.
#                 add WWHD_T60_AT_STEP=k to switch to 60 Hz k logic steps after the origin)
#   RUNNER        run_scenario.sh to use (default: EXE's port checkout, <checkout>/tools/gametest/run_scenario.sh,
#                 found by walking up from EXE)
#   SERIAL=1      the two runs one after the other (default: both at once, 2 game sessions)
#   T60_SYNC_LOADS  WWHD_TEST_SYNC_LOADS of both runs (default 1: async boot loads can leave
#                 one run's world a frame behind at the origin; compare60's origin-clock gate refuses such a pair)
#   CMP_ARGS      extra compare60.py arguments (e.g. "--tol pos=4 --admp-spec link.spec")
# Exit status: compare60.py's (0 equivalent, 3 equivalent under the time slack only, 1 not, 2 no origin).
set -u
exe=${1:A} game=${2:A} save=${3:A} out=${4:A} end=$5; shift 5
here=${0:A:h}
N=${T60_STEPS:-2}
envA=(${=T60_ENV_A:-WWHD_T60_STEPS=1})
envB=(${=T60_ENV_B:-WWHD_T60_STEPS=$N})
runner=${RUNNER:-}
if [ -z "$runner" ]; then
  d=${exe:h}
  while [ "$d" != / ] && [ ! -f $d/tools/gametest/run_scenario.sh ]; do d=${d:h}; done
  runner=$d/tools/gametest/run_scenario.sh
fi
[ -f "$runner" ] || { echo "pair60: no run_scenario.sh (set RUNNER)"; exit 2; }
mkdir -p $out
a=$out/a30 b=$out/b$N
# never leave a game behind: the runners kill their own game; this kills the runners
pids=()
trap 'for p in $pids; do kill $p 2>/dev/null; done' EXIT INT TERM HUP
if [ "${SERIAL:-0}" = 1 ]; then
  zsh $runner $exe $game $save $a $end WWHD_T60_STEPLOG=steps.txt WWHD_TEST_SYNC_LOADS=${T60_SYNC_LOADS:-1} $envA "$@"
  zsh $runner $exe $game $save $b $end WWHD_T60_STEPLOG=steps.txt WWHD_TEST_SYNC_LOADS=${T60_SYNC_LOADS:-1} $envB "$@"
else
  zsh $runner $exe $game $save $a $end WWHD_T60_STEPLOG=steps.txt WWHD_TEST_SYNC_LOADS=${T60_SYNC_LOADS:-1} $envA "$@" & pids+=($!)
  zsh $runner $exe $game $save $b $end WWHD_T60_STEPLOG=steps.txt WWHD_TEST_SYNC_LOADS=${T60_SYNC_LOADS:-1} $envB "$@" & pids+=($!)
  wait
fi
echo "== true60 pair (N=$N, $end s): A ${envA[*]}  B ${envB[*]}"
python3 $here/compare60.py $a $b $end ${=CMP_ARGS:-}
