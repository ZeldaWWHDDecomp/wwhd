# tools/verify: verified decompilation of WWHD

Readable C++ for WWHD (`cking.rpx`) functions whose **behaviour is checked against the
recompiled original** (`build/gen`), function by function, without running the game.

- The harness, unit generator and ABI layer (`include/gabi.h`) live here; the recording taps are
  `runtime/src/verify_tap.cpp` in the recompilation port.
- The verified source is in `wwhd_src/` (see `wwhd_src/README.md`).
- Nothing derived from the game is part of this repository: `build/` (recompiled code, the RPX image
  dump, names) and all recordings (`*.tap`, they hold game memory) are generated locally from your
  own copy of the game and are git-ignored. Two unit files (`d_grass`, `d_wood`) name a recording
  directory `tools/verify/recordings/<unit>` with a `rec` line; when it does not exist, the unit runs
  with generated inputs only (see "Recorded" under Inputs for how to record).

## Quick start

```sh
python3 tools/recomp/recomp.py game/code/cking.rpx build/gen        # once
python3 tools/verify/verify.py d_a_kamome -n 3000                   # build + run a unit
python3 tools/verify/mutate.py d_a_kamome --n 1000 --max 500        # how sensitive is it?
python3 tools/verify/wdis.py 021861D4                               # disassembly with names
```

Output, one line per function:

```
addr      function                    generated   recorded coverage  first difference
021861D4  daKamome_Execute            10000/10000   120/120  337/409  ok
```

- `generated`: generated inputs that passed / run.
- `recorded`: recorded calls that passed. `(-k)` counts recordings the original's replay could
  not reproduce; those are skipped.
- `coverage`: basic blocks of the original reached.

Harness options:

| Option | Meaning |
|---|---|
| `-n N` | generated inputs per function |
| `-seed S` | seed for the generated inputs |
| `-rec DIR` | recorded inputs from `DIR/ADDR/*.tap` |
| `-only ADDR` | test one function only |
| `-v`, `-v -v`, `-v -v -v` | failures; also uncovered blocks; also full memory diffs |
| `-trace SEED` | off unless given. Prints every top-level access and call of one generated input (a dictionary pre-run seed, `SEED0*7777+i`, is traced the same way, headed `trace prerun LABEL (dictionary pre-run i)`; 2026-11-01, self-test `traceprerun_test.py`); also each call made inside a `real` callee (`nested call TARGET r3=.. r4=.. r5=.. r6=..`, `(real)` when it runs for real, then `nested return TARGET r3=..`), each steered allocation with its size and alignment (`[nested ]alloc TARGET -> PTR (size S align A)`), each `mockset`/`mocksurfacesize` effect, and on an abort its reason plus the chain of `real` callees it happened in (`aborted (original side): REASON [inside real A B]`). 2026-10-24 (AGL9/AGL#8 requests) |
| `-trace SEED` net effect | (2026-10-25) a traced input also prints each side's net memory effect, the exact map the comparison uses, as `net effect (original|candidate side): N bytes` followed by address ranges with their bytes. Nothing extra is read or written (AGL#4 request). |
| `-watch ADDR[:LEN]` (verify.py / harness option) | prints every guest byte access to [ADDR, ADDR+LEN) (LEN default 4) at any depth, including setups and nested `real` callees, to stderr: `watch R|W|P ADDR VALUE side=orig|cand depth=N in=<innermost real function> seed=S` (P = a mock or recorded effect). Only guest accesses are reported: the harness's own reads (argument capture, vtable scans, mock models; `peek8`) are skipped (2026-10-26). Runs unsharded; use with `-n 1` (or `-range`). For finding which original routine touches an address, e.g. a setup writing through a NULL object (AGL#18). 2026-10-26; self-test `watch_test.py` |
| `-nulltrap` (verify.py / harness option; `mutate.py --nulltrap`) | console DSI emulation: any guest load or store of a byte in page 0 (EA 0x0000..0x0FFF) by the tested function or a `real` callee (any depth) ends that side's run at once, as a DSI would. One side faulting is an ordinary difference, `trap: page-0 DSI by the candidate only (load 00000004, depth 0)`; both sides faulting at different accesses (kind or EA) compare only their common call prefix (the shorter side's calls; a difference there reads `call #k ... (before the page-0 fault; the sides faulted at different accesses: ...)`): the input crashes the console either way, so neither the faulting access nor the net effect is compared, and such inputs are counted per function on stderr (`nulltrap: ADDR: N input(s) ended in page-0 faults on both sides at different accesses ...`; 2026-10-26, AGL#13). Both sides faulting at the same access compare like a `stopat` end: calls, traps and net effect up to the fault, no return value, and without the dying stack frames and linkage words [entry SP - 0x10000, entry SP + 8) (the console crashed: the tested function's and its callees' frames are not observable, and GHS schedules frame stores and the prologue's LR save around the faulting access; even under `footprint`). The caller's frame from entry SP + 8 (parameter area) on and all other memory still compare (2026-10-26, AGL#4). Without it page 0 is ordinary guest memory, and a NULL walk `for (p = at(0); p != at(4n); ++p)` never meets its end (at(0) is the host nullptr, at(4n) is PPC_MEM_BASE + 4n, so the host pointer walks guest EAs 0, 4, 8, ... past 4n) until a budget aborts it. The original faulting is reported on stderr (`nulltrap: ADDR: the original took a page-0 DSI on N input(s); first: ...`: the inputs drive the original into page 0, a fixture defect), and a `setup`/`postsetup` routine touching page 0 exits 2 (`nulltrap: setup ADDR touched page 0 (...)`). Harness reads (argument capture, mock models) and mock/recorded effects never trap. Same as the unit directive `nulltrap` or a `# verify: -nulltrap` header. 2026-10-26 (AGL#13); self-test `nulltrap_test.py` |
| `-nulltrap`: candidate-only instruction-fetch faults (indirect calls to page-0 / non-code targets) | with `-nulltrap` (or the unit directive `nulltrap`) an indirect call or branch (`bctrl`/`bctr`/`blrl` in the original or a `real` callee, `gabi::call_ptr`/`tail_ptr` in the candidate; at any depth) to a target that is not a code address is checked against the original: the original's such calls keep the generic mock (as without the option), and the candidate may make the same calls (same target value, at most as often per target, on the same input) with the same mock. Any other candidate call to such a target is a console ISI: it ends the candidate's run like a page-0 DSI (the same trap event: Result::dsi with kind `X`, the target as the address, the depth and innermost `real` function) and reads `trap: page-0/invalid instruction fetch by the candidate only (target 00000000, depth 1 in 0279E57C)`. The original's targets are part of its result (also in `-refcache` records), so cached and live originals decide alike; `-trace` prints each decision (`fetch TARGET (side, depth N): ...`). **Code address** (harness.cpp `code_target`, the low two bits ignored as by the CPU): not page 0 (0x0000..0x0FFF), and either (1) inside a loaded image section below 0x10000000 (the RPX code segment: `.syscall` 02000000 and `.text` 02000020..028F87F4; `.rodata`/`.data` start at 0x10000000), (2) a function import slot (`build/gen/imports.json` kind `f`, emitted by mkunit.py as `unit_import_slots`; data import slots are not code), or (3) a target the unit declares callable: any callee entry (game code, imports, `callee ADDR fixture` mock-only targets) or a target named by a mock spec line (`ret`, `fret`, `fretf64`, `retarg`, `nestedret`, `mockset`, `mockzero`, `mocklookup`, `mockmove`, `mocksurfacesize`, `mockfield`, `alloc`, `stopat`, `observe*`, ...). Direct calls (`bl`, `gabi::call`) have static targets and are not checked. **Page-0 targets on both sides:** a call or branch to a page-0 target (word address 0x0000..0x0FFF: a NULL function pointer or vtable slot) is not checked against the original: it is an ISI on EITHER side and ends that side's run exactly like a page-0 DSI (kind `X`, the target as the address) with the same classification: candidate-only = difference (`... by the candidate only (target 00000000, ...)`), original-only = fixture defect / no evidence (`... by the original only ...`, counted in `nulltrap: ADDR: the original took a page-0 DSI on N input(s); first: fetch 00000000 ...`), both sides at the same target = compared up to there; inside a `setup`/`postsetup` it is a fixture error (exit 2); `-p0exclude` notes the original's page-0 call like its page-0 access. Self-test `page0call_test.py`. **Limits:** the original never takes a fetch fault at a non-code target at 0x1000 and above, so an input that drives the original itself through such a pointer is not reported (unsteered memory gives function pointers random values, which the harness cannot tell from program state); and a candidate that dispatches through the same non-code target as the original keeps the generic mock. Without `-nulltrap` such calls get the generic mock as before (identical results). 2026-10-29; self-test `codetarget_test.py` |
| `-p0exclude` (harness option; nulltrap_compare.py passes it on its side without -nulltrap) | zero-page rule for the plain side: the harness notes, without trapping, whether the ORIGINAL's run touched page 0 (the same guest loads/stores -nulltrap would trap; not its setups, not harness peeks). A difference on such an input comes after that access and is no evidence. It is counted like an input aborted on both sides (the row gets the ` (some inputs aborted on both sides)` note) and reported once per process on stderr: `p0exclude: ADDR: N difference(s) after the original's page-0 access excluded (zero-page rule); first: ...`. nulltrap_compare sums these lines and prints `nulltrap_compare: seed S ADDR: N plain difference(s) after the original's page-0 access excluded ...`; they are also noted in the coverage file. A difference on an input where the original did not touch page 0 stays a FAIL, which matters for `all` functions, whose plain inputs differ from the -nulltrap side's. Refused (exit 2) together with a command-line -nulltrap or -refcache. A unit with the `nulltrap` directive traps page 0 in every run, nulltrap_compare's plain side included, so there the option is switched off with the note `p0exclude: off: the unit's nulltrap directive ...` (2026-10-31, lib2_agl_1). Self-test `p0exclude_test.py` |
| `-fno-finite-loops` (compile flag of every verification build: `verify.FP`, so verify.py, mutate.py and its schema builds, mutcode.py and objcmp.py; C and C++, clang >= 12) | C++ (and C11 for a loop whose condition is not a constant) lets the compiler assume that a loop without side effects terminates (the forward-progress rule), so clang deleted a side-effect-free loop that never ends and the mutant returned normally: AGL#18 mutant 91 / global 1709 (drop `last = link->mNext;` from the read-only trailing walk of `releaseRange`, RangePoolRelease.cpp:109) survived at n 10000 in its own -O1 build, while its schema build happened to keep the loop (abort); ruled RI91 as a harness limitation. With the flag such a loop stays a loop, so the candidate's run is ended by the CPU time budget (`only one side aborted: time budget exceeded (30 s CPU in one run)`, mutate class `abort`) or by the mutant's per-run timeout (class `timeout`): the non-termination evidence a D (non-termination) class needs, the same in standalone and schema builds. A mutant whose only change makes a loop infinite also no longer compiles to the unmutated object (no identical-code EQ). Loops that terminate run as before: unmutated gates (nulltrap_compare --archive n 1000, seeds 1 and 7, plain and -nulltrap, lib2_agl_1/4/11/12, lib2_sead_15, lib2_hdtu_lib) are byte-identical to the build without it. The flag is in the `fpflags.stamp`, so old objects rebuild, and in the vcache key, so cached rows miss once. 2026-10-29; self-test `finiteloops_test.py` |
| `-guardstrict` (verify.py option; host-index wrap diagnostic) and the direct-guest-access crash message | `-guardstrict` builds the unit with `GABI_GUEST_GUARD=2` (objects and executable in `<unit dir>/guardstrict`, so the normal build is untouched; no cached rows). Every be<>/gptr<> access whose host address is neither in the guest window [PPC_MEM_BASE, +4 GB) nor NULL-based below 4 GB reaches its guest address only through ea()'s 32-bit wrap: a u32 index on a >= 2-byte element (`keys[count - 1]` with count 0: host +8 GB for u16), a negative int index below the window, or a NULL-based index past 4 GB (`at<T>(0)[i]`). The console computes the same EA, so results are right, but a NULL-based one past 4 GB is outside the guard's fast range, and whether its host address is a host object (stack, heap, the executable at ~4 GB) depends on the load address: a false guard trip in some runs or shards. Each site prints once per function and host pc: `guardstrict: 02759928 candidate side: be<4 bytes> at host H (guest EA by the 32-bit wrap: KIND[; outside the guard's fast range: may trip the guard depending on the load address]), KIND one of `index wrap above/below the guest window` (>= 1 MB past the window: the index pattern, or a generated index), `NULL-based past 4 GB`, `NULL-based negative index`, `top wrap: small offset past EA FFFFFFFF` / `bottom wrap: small negative offset below EA 0` (a member offset from a pointer near the ends of the 32-bit space, usually a generated input pointer: an input artifact, not a source pattern), seed S, depth D; host pc P (symbol); source line: atos ...`, and the run closes with per-function counts (`guardstrict: ADDR: N host-wrapped access(es) at K site(s)`) or `guardstrict: no host-wrapped be<>/gptr<> access`. Rows are unchanged (same guard decision). Fix: guest EA arithmetic (`sead::elementAt`, `at<T>(base + i * size)`). **Crash message (every build):** guest memory is not mapped at PPC_MEM_BASE, so a SIGSEGV/SIGBUS at an address in the guest window (or below 4 GB) is candidate code that accessed guest memory directly: a raw member or array of a guest object (AGL#7: `nativeTexture[0x9C]` on a `u8` array member; fix: a be<u8> view) or a raw host pointer. After the usual `host signal` line the harness prints `verify: the fault address A is guest EA: a direct host access to guest memory (...); host pc P (symbol); source line: atos ...`. 2026-10-29; self-test `hostwrap_test.py` |
| `-observe-pointees` (harness / verify.py / nulltrap_compare.py option, also `--observe-pointees`; `mutate.py --observe-pointees`; or a `# verify: -observe-pointees` header) | opt-in (2026-10-28). At every top-level mocked call, each argument register the comparison checks (the callee's argument mask) that holds an undeclared pointer into a real heap block or a static object also has that object's bytes compared, original against candidate at the same call index. Heap blocks: the sead ExpHeaps named by `observeheap ADDR` spec lines or `-observe-heap ADDR` options; with none, the shared agl world's 16 MB root heap at 0x6A000000 whenever a valid ExpHeap header is there (object at the start of its arena, plausible size, used-block list inside the arena), so world specs need no extra line; walked through their ExpHeap children and used-block lists, so the bounds are the allocator's own (block data = header + 0x10, size from the header); a pointer inside a child heap of another kind (FrameHeap) or outside any block (free space, a header) is not observed. Static objects: `build/verify/statics.tsv` (`mkstatics.py`, built by verify.py: every relocation target in .rodata/.data/.bss plus the section bounds; the object is [greatest boundary <= pointer, next boundary)). Stack pointers keep their own rules. A declared pointee always wins: any `callee TARGET rN=...` fact on that register (including `rN=0` and `rN=heapSIZE`), a dynamic extent, or an `observearg`/`observeptr` on it keeps exactly its declared semantics. A difference is an ordinary FAIL row: `call #2 C0006A40 GX2SetVertexUniformReg: r5=610201E0 -> heap block 61020000 size 0x400 differs at +0x1E3 (610201E3): original 3F candidate 2F (-observe-pointees; ...)`, or `... bounds differ: ...` when the two sides' blocks differ. Objects larger than the cap are compared in a cap-sized window around the pointer and the message says `(truncated to the cap)`. **Baseline first:** a run with the switch is creditable only when the unmutated baseline passes with it in the exact configuration (mutate.py's baseline gate runs with it; receipts and the vcache key carry the option). A baseline that fails with it usually means the block holds bytes the callee never reads (padding, the original's temporaries, a field written later): narrow that argument with an explicit fact on that register: `callee TARGET rN=heapSIZE` (only the SIZE bytes the callee reads), `callee TARGET rN=0` (the pointer value only), or a (per-site) `observearg` span. Self-test `observepointees_test.py` |
| `-observe-pointees-cap BYTES` | the largest object compared whole (default 0x10000 = 64 KB; 4 bytes to 16 MB) |
| `-refcache-write DIR`, `-refcache DIR`, `-refcache-key K`, `-refcache-check`, `-refcache-digest` (harness; mutate.py sets them) | the reference-side cache (2026-10-28): the writing run stores, per function and generated input, everything the original's run leaves for the rest of the input (the Result: exit registers, call records with every observed span, net effect, float marks, `final` bytes, abort/stop/DSI state, strays, traps; the stack-guard bytes; the coverage counts and semantic-effect statistics it added), zero-run encoded, one file per function, written only when every generated input of the function ran (no -runlist subset, no -range, not stopped early); inputs on which the original hit a time limit are stored as "run live". A reading run takes the original's side of each generated input from the file instead of running it; the candidate run, the comparison and the printed rows are unchanged. The header carries the key, n, seed, function, block count and comparison switches: any mismatch, a truncated or missing file runs that function's original live. Recorded (.tap) inputs always run live. `-refcache-check` runs the original anyway and exits 97 when the record differs from the live run or does not decode back to it. Since 2026-10-29 (RFC3) the header also carries the function's constant dictionary, so a reader whose generated inputs would differ runs that function live. Self-tests `mutate_refcache_test.py`, `mutate_refcache_order_test.py` (a function's records are the same in a whole-unit run and run alone) |
| `-snapcache-write DIR`, `-snapcache DIR`, `-snapcache-key K`, `-snapcache-check` (harness; mutate.py sets them) | the setup-snapshot cache. With `setupsnapshot` every process used to run each function's setups for real four times or more (the two capture runs, the first input of the run and every `-snapcheck`'th input): ~90% of a mutant run on agl world specs. The setups run only original code (unit.c `real_X` and the harness's mocks; candidate code runs only from the top-level call of its own row, mutscope.py), so their result is the same for every mutant of a campaign. The writing run (mutate.py's baseline) stores per function `ADDR-TAG.snap` (TAG: a hash of the configuration header, so several configurations share a directory; it points by payload hash to the accepted snapshot in a content-addressed `world-HASH.blob`, written once per distinct snapshot: the functions of a world spec, and specs over the same world, share it, ~13 MB for the agl world; a refused snapshot writes nothing) and `ADDR-TAG-sSEED.val` (per validation input, delta-encoded against the snapshot and the input's own generated bytes, pages exactly as predicted cost one byte: ~33 KB instead of ~27 MB per record for the agl world, with the hash of the reconstructed plain state; the complete post-setup state the original side's real setups left: memory incl. the scratch stack window, run counters, stack-guard bytes, statistics; recorded after the run's own check against the snapshot passed; inputs whose setups made a non-code fetch or that hit a time limit are not recorded). A reading run takes the snapshot from the file instead of the capture runs and installs a validation input's record on both sides instead of running the setups; a validation input without a record (e.g. the first input of a `-runlist` subset) runs them as before. Key: `-snapcache-key` (mutate.py: sha256 of the unit.c/FP-runtime/harness/support objects, the expanded spec, the harness options without -n/-seed, the recordings) + a hash of the guest state after start-up (image, copy/blob bytes, fills, the unit's function/callee tables) + the comparison switches; the `.val` header adds the input seed, the function's constant dictionary and the hash of the snapshot. The snapshot is seed-free (the capture already proves it input-independent), the validation states are not. Any mismatch, a missing file, or a payload whose hash does not match: that function's setups run live (stderr note). Off (note `snapcache: off under ...`) with `-nofastreset`, `-trace`, `-watch`, `-dsiout`, `-mutrec` or `WWHD_VERIFY_FASTRESET_CHECK=1`. `-snapcache-check` runs everything live and compares the capture with the cached snapshot (byte for byte at the same seed) and each live validation setup run (either side) with its record; a difference or an undecodable file exits 98 (mutate.py: `invalid`). Rows, coverage and stderr (apart from `snapcache:` lines) are the same as a live run. A reading run's last stderr line: `snapcache: F function(s) took the cached snapshot, L captured it live; V validation input(s) took the recorded setups, W ran them live`. Self-test `snapcache_test.py` |
| `-fastreset` / `-nofastreset` (harness option; env `WWHD_VERIFY_FASTRESET=1`/`0`, which also reaches verify.py, nulltrap_compare.py, readable_check.py and mutate.py runs) | per-input memory reset for `setupsnapshot` units (2026-10-28). Without it every generated input installs the whole captured post-setup state into fresh pages, rebases all of it (initial value := current, write bits cleared), scans all of it for the net effect and unmaps it again: for the AGL world (~7 MB of materialized snapshot bytes) that was ~95% of an input's CPU time (AGL#12 applyCount: ~19 ms CPU per input, the tested function ~2%). With it the snapshot is built once per function as template pages (already in the rebased state) that stay across inputs; every page access marks the page, and only marked pages are restored (template pages) or cleared/dropped (all others) before the next run, so each run starts from the same state as before. A run that ends between the install and the rebase (abort in a postsetup) gets the snapshot's own initial values and write bits back. On setupsnapshot validation inputs the candidate side installs the original side's complete post-setup state of the same input (scratch stack window included) instead of running the setups a second time (same code on the same input state; `WWHD_VERIFY_FASTRESET_CHECK=1` runs them anyway and exits 3 if the states differ; `-trace`/`-watch` runs keep running them). The leading absolute `fill`/`copy`/`blob` lines of a function (the 16 MB heap fill and 16 MB blob of the world) are built once per function instead of per input. Results (rows, counts, coverage, first differences, stderr notes, mutant classes) are identical with and without it. On by default (2026-10-28, after validation); `-nofastreset` or `WWHD_VERIFY_FASTRESET=0` switch it off. Fixed 2026-10-29: under `-watch`/`-trace`/`WWHD_VERIFY_FASTRESET_CHECK=1` the candidate's real setup run is compared with the original's state, and that check exited 3 with `(run counters)` whenever the tested function reloads a float-stored NaN word as an integer (the 8-slot GHS value-copy ring `V.fcopy`): the per-input reset cleared only the ring's count, so never-read slots kept the previous run's values. The reset now clears the ring (results unchanged: those slots are never read), and the message names the counter (`run counters: fcopy`). Self-tests `fastreset_test.py`, `watchfastreset_test.py` |
| `-snapcheck N` (harness option) | setupsnapshot validation period: besides the first input of a run (each shard), every input whose index is a multiple of N runs the setups for real and checks the snapshot (default 500, as before; 0 = first input only). After `-fastreset` these real setup runs (~1.7 s CPU each for the AGL world) are most of an AGL run's cost. A different N changes which inputs see the real setups' leftover bytes in the scratch stack window below entry SP - 0x8000, so it is not guaranteed result-identical for functions that read that deep into uninitialised stack (2026-10-28) |
| `-srcopt O0\|O1\|O2\|O3` (verify.py) | build the unit's own `src` sources at that optimisation level (default O2; the recompiled originals, glue and harness stay at O2), into separately named objects and a separate binary (`verify.O1`), never mixed with the O2 cache. `mutate.py` compiles variants at O1, so `-srcopt O1` on the unchanged source is the matched control for a variant's result. 2026-10-24 |
| `verify.py UNIT -srcopt-only SRC [-srcopt O1]` | only the unit source SRC at the -srcopt level (default O1), every other source, the originals and the harness at O2: exactly mutate.py's build of a mutant (the mutated TU at O1 in an O2 link), so it is the matched control for that TU's mutants. Separate object/executable names (`.O1.o`, `verify.O1-<src>`), never the O2 cache. A SRC that is not a `src` of the unit exits. 2026-10-25 (AGL#17); self-test `srcopt_only_test.py` |
| `verify.py UNIT -buildj N` | compile the unit's objects with N parallel compiler processes (the same commands and objects as the serial build; a failing compile exits with its code). Mind the machine's load table when choosing N. 2026-10-25 (AGL#18 request) |
| `verify.py UNIT -unitsplit N` (also `WWHD_UNIT_SPLIT=N`, spec line `unitsplit N`) | compile the generated unit.c as N translation units (`unit_shared.h` with every declaration + `unit.p0.c`..`unit.pN-1.c`, contiguous size-balanced runs of the generated functions; the stubs and tables in the last part; listed in `<unit dir>/unit.parts`): the same compiler flags per TU, the same link, the same rows, a fraction of the clang memory. Option > environment > spec; N = 1 = no split. Strictly opt-in: without a request nothing is split, unless `WWHD_UNIT_SPLIT_AUTO_MB=M` (M > 0; default 0 = never) asks for ceil(size / (M/2 MB)) parts above M MB. A large profile declares `unitsplit N` in its spec. unit.c is still written unchanged. Parts compile one after the other; `-buildj N` runs them in parallel. mutate.py and the refcache key take the part objects. 2026-11-02 (J3D start-up profile: 54 MB unit.c, 6 GB clang); self-test `unitsplit_test.py` |
| mkunit.py memory | mkunit builds each reached function's dataflow graph once (its two Dataflows share one Cfg cache, was built twice), keeps the per-instruction register sets and successor/call lists as shared interned values, does not keep the generated text of every reached function, and writes unit.c line by line instead of joining it into one string. Output byte-identical. Measured on the J3D start-up profile the J3D lighting diagnostic profile (37,836 callees, 352 imports, unit.c 167 MB, split 5): peak RSS 6.47 GB before, 2.39 GB after, wall 144 s -> 86 s. No-change proof `mkunit_nochange.py BASE_TREE OUT_DIR UNIT[:SPLIT]...` (every file mkunit writes, SHA-256, plus both peak RSS) |
| `-noprogress`, `-progress-interval S`, `-stuck S`, `-hanglimit S` (verify.py / harness options; env `WWHD_VERIFY_PROGRESS=0`, `WWHD_VERIFY_PROGRESS_INTERVAL`, `WWHD_VERIFY_STUCK`, `WWHD_VERIFY_HANG_LIMIT`) | progress reporting, on by default (2026-10-27): every S s (default 10) one `[progress] harness pid P: function k/N ADDR NAME, generated d/t, all D/T inputs, R inputs/s, ETA, last seed S` line on stderr; `[progress] STUCK on input seed=S (generated input i) of ADDR NAME for T (candidate side, ...)` when one input runs longer than `-stuck` s (default 60, 0 = never); a per-run wall-clock hang limit (default 120 s, at least twice the CPU time budget unless set explicitly; 0 = off) that ends a run still going but not computing (under 5% of a CPU and its thread found not runnable in >= 95% of the once-a-second samples: blocked or sleeping, which the CPU time budget cannot see; a run starved of CPU under heavy load stays runnable and is never ended by it, 2026-10-27, self-test `hanglimit_test.py`) like the 30 s CPU time budget does (an abort, `wall-clock limit exceeded (N s in one run)`); a computing loop stays with the CPU time budget, so no verdict depends on the load, and a `[progress] HANG ADDR NAME gen seed=S (input i): candidate side: REASON (1-min load L)` line for every input a time limit ended (candidate side only: the row is a FAIL; original side: the inputs drive the original into a loop, steer them). See "Progress, stuck inputs and hangs" below. stdout never changes; `-noprogress` gives the old stderr byte for byte. Self-test `progress_test.py` |
| `-noclobber` | mocked callees never modify memory |
| `-strictnan` | compare NaN payloads bit for bit |

## Scratch on the RAM disk (recommended for every run, 2026-10-26)

Mutation campaigns and verify runs write about 1 MB per compiled mutant (`mutantN.cpp/.o`, the linked `mutantN`)
plus each unit's build (`unit.c`, which mkunit rewrites on every run, `unit.c.o` and the `verify` executable, relinked
every run). Put all of that on a RAM disk, so only receipts, logs and archives reach the SSD:

```sh
export WWHD_VERIFY_SCRATCH=$RAMDISK   # unit build/output directories (verify.py, mutate.py, verify_all.py, readable_check.py); missing folder = error
export WWHD_MUT_SCRATCH=$RAMDISK      # mutate.py's per-mutant files, in a per-run subdirectory (or --scratch DIR); missing folder = error
```

Mount it once per boot (16 GB; RAM is used only as files are written):

```sh
diskutil erasevolume HFS+ ramdisk $(hdiutil attach -nomount ram://33554432)   # then set RAMDISK to the mounted volume
mkdir -p $RAMDISK/.fseventsd
touch $RAMDISK/.fseventsd/no_log $RAMDISK/.metadata_never_index
diskutil unmount $RAMDISK && diskutil mount "$(basename "$RAMDISK")"   # remount: fseventsd now honours no_log
```

Check with `df -h $RAMDISK`. If a variable (or `--scratch DIR`) is set but its folder is missing or not
writable, every tool (verify.py, verify_all.py, mutate.py, cascade.py, nulltrap_compare.py, readable_check.py) stops at
once with exit 2 and an error naming the variable and the path, and writes nothing; the folder is never created, so a
typo leaves no stray folder (2026-11-02, self-test `scratch_missing_test.py`). Unset, the tools build in
`build/verify/<unit>` as always. If the folder is only low on space (verify side under 1 GB, mutate side under 200 MB or
3x the baseline executable), the tools build in `build/verify/<unit>` and print one note on stderr; results are the
same either way. Nothing on the volume needs to be kept: unit directories
(`$RAMDISK/<worktree>-<hash>/<unit>`) are a rebuildable cache, and mutate removes its per-run directory
at exit. To clean up, delete your own `<worktree>-<hash>` directory (or a leftover `<unit>-<pid>-*` run directory of a
killed campaign). `hdiutil detach $RAMDISK` drops the whole volume, so only do that when no one is running.
Details: "Unit builds on a RAM disk" below and `mutate.py --scratch` in the workflow.

## Progress, stuck inputs and hangs (2026-10-27)

Long runs show whether they progress or hang. All of it is on by default and goes only to stderr (lines starting with
`[progress] `) and to progress files; stdout, receipts and the logs callers keep are unchanged.

- **harness** (`src/harness.cpp`): a watchdog thread (all signals blocked; started after start-up and after
  `-forkserver`'s fork) prints the progress line every `WWHD_VERIFY_PROGRESS_INTERVAL` s (10), a `STUCK` line per input
  over `WWHD_VERIFY_STUCK` s (60; again at every further multiple), and enforces the wall-clock hang limit
  `WWHD_VERIFY_HANG_LIMIT` (120 s per run, i.e. one side of one input) for a run that is not computing (under 5% of a CPU
  since it started and its thread not runnable in >= 95% of the watchdog's once-a-second samples: blocked or sleeping;
  a run starved of CPU under heavy load stays runnable and is never ended by it): SIGUSR1 to the main thread, whose handler ends the run exactly like the CPU time
  budget. A computing run, also a two-instruction guest loop, is ended by the CPU time budget (ITIMER_VIRTUAL, 30 s CPU,
  `timebudget`) as before, so no verdict depends on the machine's load; macOS delivers
  spurious ITIMER_VIRTUAL expiries once a process has a second thread, so its handler now checks the CPU time really used
  and re-arms the timer on an early one (no early aborts; a genuine expiry is unchanged). A run of inputs that each hit a
  time limit (e.g. a candidate that loops on every input: 30 s per input, hours per function, and the row only prints at
  the end of the function) now shows `HANG` lines and `N hang(s)` in the progress line within the first minute.
- **verify.py** takes the four options above and passes them on (environment); it keeps `verify-PID.json` (unit, phase
  build/run, harness PIDs, summed progress). With `-shards K` the shards print only `STUCK`/`HANG`, and verify.py prints one
  merged line (`verify UNIT: K shards, function k/N (slowest shard), d/t inputs, ...`).
- **verify_all.py, readable_check.py, nulltrap_compare.py** keep their captured logs exactly as before (their verify.py
  children print no progress lines, `WWHD_VERIFY_PROGRESS_STDERR=0`); they print `[progress] <tool> <spec> seed S: ...`
  from the child's progress file instead, plus `verify_all: k/N units done` / `readable_check: run k/N: SPEC seed S`.
- **mutate.py**: `[progress] mutate UNIT: mutants d/N done, kills K, survived S, broken B, timeouts T, R mutants/s, ETA`
  and `mutate-PID.json`; `--no-progress` turns it off. Its harness runs always run without progress and without the
  wall-clock hang limit, so receipts, verdicts and the adaptive per-mutant timeout are exactly as before.
- **Files**: `<unit build dir>/progress/<tool>-PID.json`, i.e. on the `WWHD_VERIFY_SCRATCH` RAM disk when set (rewritten
  every interval; mutate without it but with `WWHD_MUT_SCRATCH`: its per-run scratch directory), else on the SSD build
  directory and then rewritten at most every 30 s. Each has an `updated` time stamp; a symlink in
  `/tmp/wwhd-verify-progress` (`WWHD_PROGRESS_REGISTRY`) finds it by PID. A process removes its file when it ends normally.
- **`progress.py [PID | RUN_DIR | SCRATCH_DIR | UNIT] [--json]`** prints the current state: by PID (a verify.py or mutate.py
  PID includes its harness children), by unit/run/scratch directory, or everything in the registry without arguments.
  `updated 5m ago` on a live process means it stopped reporting; `(process gone)` marks a run that died.

## Auditing a landed ledger against its receipts (receipt_audit.py, 2026-11-02)

`receipt_audit.py --ledger TSV --receipts DIR... [--versions REPO:REF...] [--idmap DIRSUBSTR=TSV...] [--exclude DIR=REASON...]
[--ruled-basis PREFIX...] [--function-pin]` checks every `mutant*.json` below the receipt roots against the text it compiled
(its `mutantN.cpp`): `current` (the landed source with the row's edit at the row's span), `stale` (an earlier version from
the `--versions` history, named by blob and commit), or `error` (no version, or the landed text with the edit elsewhere than
the ledger's span); `excluded`, `unreadable`, `unjoined` and `no-companion` are listed too, so no receipt is skipped silently.
Receipts join ledger rows by function and mutation text (the ` @file:line` token dropped), through an `--idmap` for
campaign-local ids. Every credited row (D, EQ, RI, NAMED_GAP, SYNC) needs a current receipt of its class with at least
`--gate-min-n` inputs on a `--gate-seeds` seed, or a ruled basis named in its `class_source`; `--function-pin` admits a stale
receipt whose version has the row's function text unchanged, reported apart as `pin`. An EQ, NAMED_GAP or RI row with a
current clean difference is `CONTRADICTED` unless `--ruled-difference ROW=REASON` names it (a named row without one is an
error). `--object-pin` admits a stale receipt only when the row's function compiles to identical object code in its version
and the landed one (that version's headers, this tree's glue and flags); `--exclude-spec` and `--exclude-receipt` drop
`--exclude-receipt PATH=REASON` takes an absolute path (one file) or a path relative to any receipt root (every receipt whose path ends in /PATH), so a ledger's exclusions can live in its archive. `@FILE` reads arguments from FILE, one per line.
receipts with a reason. Unbacked or contradicted rows, error receipts and id-map mismatches exit 1. It replaces the
identity check of the campaign script integrate_receipts.py, which compared every receipt with one pinned source hash and
logged and skipped the rest. Self-test `receipt_audit_test.py` (`--old SCRIPT` runs the same fixture through that script).

## Result cache for re-checks (2026-10-27)

`verify.py` reuses a function's result row when nothing that decides it changed: re-gates after a fold, a gate set run
again after an unrelated edit. On by default whenever `WWHD_VERIFY_SCRATCH` is usable; the cache lives in
`$WWHD_VERIFY_SCRATCH/vcache` (RAM disk only, shared by all worktrees because keys are content hashes), capped at
`WWHD_VERIFY_CACHE_MAX_MB` (default 256) with least-recently-used eviction.

- **Key** (per unit spec, function and options; `tools/verify/vcache.py` has the full list): the function's compiled
  candidate code as the closure of its entry thunk over the unit's `src` objects (Mach-O atoms and relocations; the
  linker map, `verify[.O1].map` beside the executable, resolves symbols across objects), every object's static
  initialisers, and all of unit.c.o (originals, `real` callees, setup/postsetup chains); the expanded spec (includes
  and shared world files inlined), `blob` file contents and the function's recordings; the tool version (all of
  tools/verify except units/ and recordings/), harness/espresso/support objects, build/gen (path, names, sizes, times:
  the NaN-exact gen is its own directory), image.bin/livein.tsv/frames.tsv/build/*.tsv, clang/clang++ versions,
  compile flags and -srcopt; the harness options that reach the run (-n, -seed, -nulltrap, -v, -strictnan, ...), the
  shard count, and the WWHD_* environment except progress/scratch/slot/cache variables. A relocation the reader cannot
  follow exactly makes that function's key cover all unit sources (stderr says so): uncertainty only costs hits.
  Consequence: editing one function misses that function and every function whose code reaches it natively (a direct
  C++ call to another decompiled function), and hits the rest; a spec line, tool file, gen or flag change misses all.
- **Output**: cached stdout blocks (rows, and with `-v`/`-v -v` the FAIL and uncovered-block lines) are byte-identical
  to the fresh run that stored them, in address order with the fresh rows; exit codes are computed the same way. stderr
  gets the function's recorded notes (`nulltrap: ADDR: ...`, setupsnapshot refusals), one `vcache: N of M function(s)
  cached, K to run` line and a `vcache: ADDR cached (key ...)` line per cached row. Only the misses run (`-only`).
  Sharded `-v` runs cache each function's merged block (per-input lines, row, uncovered blocks). `-dsiout FILE`
  records are cached per function and appended to FILE on a hit (nulltrap_compare unites them per function);
  `-runlist FILE` runs key each function on its own runlist lines, so both nulltrap_compare phases use the cache.
  Runs with `-trace`, `-watch`, `-range`, `-list`, `-stopfirst`, `-mutrec`, `-forkserver` or `-image` are never cached.
- **Off / archive**: `--no-cache` or `WWHD_VERIFY_CACHE=0` runs uncached. **Final and archive gates must run uncached:**
  `--archive` or `WWHD_VERIFY_ARCHIVE=1` makes verify.py compute every row itself (`verify: archive mode: ...` on
  stderr); `nulltrap_compare.py` passes `--archive` through, `readable_check.py` (merge gates) always sets it, and for
  verify_all.py set the variable. mutate.py runs its harnesses directly and never uses the cache.
- **Resume a killed gate** (2026-10-31): `--resume DIR` (verify.py; nulltrap_compare.py passes it on with one part
  directory per seed and side, the page-0 side's keyed on the -dsiout content). This campaign's own checkpoints, also
  in archive mode:
  - Each harness process stores a function's part (stdout lines, stderr notes, -dsiout records) as soon as the function
    is done. A sharded run stores one part per shard.
  - Parts are written atomically, with vcache's full key plus the harness executable's hash, the shard index, count
    and range, and a content hash.
  - A rerun with the same DIR takes a function only when all its parts are present and intact. It merges them as the
    shards are merged and prints `verify: resume DIR: ...` plus one `resume: ADDR from DIR` line per function.
    nulltrap_compare adds `nulltrap_compare: resume: ...` per seed and side, and with `--logdir` writes
    SPEC.resume.json.
  - Any key change (code, spec, tools, options, shard count, result-affecting WWHD_* variables, harness) is a miss, and
    so is a missing, partial or damaged part. The function then runs.
  - Off for `-guardstrict` and `-refcache-write` runs. DIR is never evicted or cleaned.
  - Self-test `resume_test.py`.
- The harness side: `-listfn` (candidate addresses and entry-thunk link addresses) and, under
  `WWHD_VERIFY_CACHE_MARKS=1`, `@VC< ADDR` / `@VC> ADDR` lines around each function on stdout and stderr, which
  verify.py removes. Self-test `vcache_test.py`.

## What "verified" means

For each input, the original and the candidate run from the same state. They must agree on:

1. **The return value**, as the candidate's C++ signature types it: r3 (masked to its width) or f1.
2. **The net memory effect**: every byte whose final value differs from its initial value. The
   scratch stack below the entry SP is excluded; the caller's frame above it is included.
   With `setup`/`poke`/`postsetup` lines (generated inputs), "initial" is the state after all of them: every
   byte as it stands when the tested function starts (setup writes, poked values, mock effects inside setups)
   is the baseline (identical on both sides), so the net effect holds only what the tested function changes,
   and a failure's "unchanged" means unchanged since that point. A function that rewrites a poked value with
   the same value makes no change. (2026-10-25 setups; 2026-10-26 extended to pokes and every materialized
   byte: before, poked bytes kept their pre-poke initial value, so dropping a redundant store of a poked value
   was a false difference. Self-test `poke_baseline_test.py`.) Recorded inputs keep the entry snapshot.
3. **The call sequence**, call by call:
   - the target (direct, through a function pointer, or an import);
   - the argument registers the callee reads;
   - stack arguments;
   - the contents of stack objects passed by pointer (pointers inside them are compared
     relative to the object).

The original is the game's recompiled code, extracted from `build/gen` and compiled with
`include/vm_gen.h`, which routes every load, store and call through the harness. Every other
WWHD function is **not executed**. Calls to it are recorded and answered by:

- a mock (generated inputs);
- the recorded result and memory effects (recorded inputs);
- or the real code, for pure helpers listed as `real ADDR`.

So each function is tested in isolation, and the test is independent of whether its callees
have been decompiled.

### Memory model (`src/harness.cpp`)

- Guest memory is sparse and materialised lazily. The first access to a byte takes its value
  from, in order:
  1. the recorded snapshot or the `field` specs;
  2. the RPX image (`.rodata`/`.data`/`.text`, dumped by `mkimage.py`);
  3. zero for the stack below the entry SP;
  4. a deterministic typed random word: 0, small ints, small per-byte values, floats, pointers
     into the random heap, s16 pairs, -1 patterns, random bits.
- Any pointer the code follows is therefore valid, and both sides see the same contents.
- Writes are tracked per byte.
- **Clobbering**: on half of the generated inputs, every mocked call starts a new *epoch*. In
  each epoch every heap/global word may have been rewritten (1 in 4, deterministic per address
  and epoch). A candidate that caches a field across a call, reloads one the original kept, or
  moves a store across a call therefore fails, because a real callee could have changed that
  memory. The other half of the inputs keeps memory stable, so deep paths stay reachable.
- **Storage (2026-10-06).** A touched guest page costs 1.5 KB (have/written bitmaps, chunk table); its bytes
  live in 64-byte chunks created on first touch, and a chunk's per-byte clobber epochs only once one of its
  bytes can be clobbered (elsewhere the epoch never influences a value). Pages, chunks and epochs come from an
  arena that is reset after every input (extra chunks unmapped), and each run's call records are moved, not
  copied, with their rarely used parts (stack-object bytes, stack arguments, observed globals, framexact
  state) allocated only when present. Results are bit-identical to the earlier 25 KB pages; a pathological
  input that walks random memory to the 20M-access budget needs ~5-8 GB instead of ~39 GB.
- **Call budget (2026-10-24).** A run that makes more than 20000 guest calls (top level and inside `real` callees) aborts with `call budget exceeded`. A loop whose count comes from generated data (zl1_heap setBtp, unscoped dungeon-map witnesses) otherwise made ~400k mocked calls far below the access budget; each call opens a clobber epoch that every later byte load re-checks, so such inputs took hours and gigabytes. As with the access budget, an abort is never a detected difference. A unit whose functions legitimately make more calls raises its own budget with `callbudget N` (unit file, 1000..100000000 since 2026-10-26 for original setups such as the AGL#3 ShaderMgr archive; d_a_obj_apzl and d_a_bwdg use 200000).
- **Time budget (2026-10-24).** Each run (setups, pokes, postsetups and the function, on either side) gets 30 s of process CPU time (`ITIMER_VIRTUAL`, so machine load does not shorten it). Past that the input aborts with `time budget exceeded (30 s CPU in one run)`; `-trace` names the side. This catches loops that make neither guest calls nor memory accesses (hd_picto_pictograph_alias_order: the original spun 12 h inside 026FCB00). Aborts never count as detected.
- **`WWHD_MEMSTAT=1`** (diagnostic, off by default): stderr lines `memstat-progress` every 1M guest accesses
  and `memstat orig|cand seed=... pages=... arena=... calls=... budget=... fmarks=... dmarks=... net=...` per run (fmarks/dmarks:
  f32/f64 float-store marks at exit, net: bytes in the net effect; added 2026-10-26), to find inputs that walk
  memory. Such inputs end "aborted on both sides" and are never compared: fix their input domain in the unit
  file (`arg!` / `ret!` / `stable`), as done for d_a_bwdg 021045E8 and d_a_sail 024616AC.
  Since 2026-11-02 also one `memstat-func ADDR rss=... footprint=... peak=...` line at the start of each
  tested function (the process's resident size, footprint and peak so far, and the sizes and capacities of the state that
  outlives an input), so growth across the functions of one process shows. Measured with it: the per-input vectors
  orig_seq/orig_lr kept the capacity of the largest original run (400 MB after a 28M-call dictionary pre-run of
  lighting_lightfix 0276C8B8) and the last setupsnapshot (g_snap, g_valstate) stayed alive through every later function;
  both now end with their run / function. The larger part was libmalloc's large-entry cache: the 28M call records (vector
  growth by doubling) were freed but kept dirty ("MALLOC_LARGE (empty)" 5.5 GB in vmmap, 66 MB live). A run's call records
  are now one mmap'd block sized by its call budget (only written pages are resident; unmapped with its Result), and the
  other large per-run vectors (orig_seq/orig_lr, the net effect) map blocks of 1 MB and more the same way (BigAlloc).
  Measured on all 510 functions of lighting_lightfix in one process (n2): footprint at function start max 5776 -> 174 MB,
  process peak 10.5 -> 5.2 GB. Open: that 5.2 GB is inside 0276C8B8 itself, ~28M recorded top-level calls x 160 bytes per
  call record, allowed by the spec's `callbudget 30000000` (the call budget times the record size bounds any run's records).
  Known run-to-run difference, not memory state: lighting_lightfix 0276FA58 "only one side aborted" names the 30 s CPU
  budget in one run and the page budget in another (a race between the two budgets; it already varies between two runs
  of the same harness).
- **Per-run cost after large setups (2026-10-26, AGL#18 request).** A setup chain that leaves ~0.3 MB of materialized state
  and ~38k float-store marks (AGL#18 accessors256: original D0474 over 256 records) made every run, not the function, the
  cost: the float marks were copied into two `std::set`s per run and side (~75k node allocations per input, ~85% of the
  CPU) and the post-setup baseline was rebased byte by byte. The marks are now ascending lists, built only without
  `-strictnan` (the only mode that reads them, for the NaN-payload tolerance), and the rebase copies whole 64-byte chunks.
  Results are unchanged (self-test `nanmark_test.py` pins the tolerance); accessors256 at n=1000: 27.2 s -> 2.7 s CPU.

### Callee facts (`funcdb.py`, `mkunit.py`)

Derived from the generated C of the whole program:

| Fact | Used for | How it is derived |
|---|---|---|
| Argument registers a callee reads | which registers are compared at the call | live-in analysis, interprocedural; varargs prologues and paired-single `ps1` reads excluded; the GameCube signature is added, never subtracted, because a matcher name can be wrong |
| Volatile registers a callee may write | which registers a mock writes | may-def analysis. GHS allocates registers across calls, so callers keep live values in "volatile" registers the callee does not touch; a mock must not clobber them |
| Shape of the r3 result (zero/sign-extended width, constant) | mock results of the same shape | GHS callers do not re-extend a `u8` result |
| Stack arguments | stack comparison | from the GameCube signature |
| Pointee sizes | stack-object comparison | from the signature; constructors' `this` storage is not compared (it is not an input) |

### Inputs

- **Generated:** argument registers typed from the signature, memory filled lazily as above.
  Steering lives in the unit file:
  - `field FUNC|* rN+OFF|ABS TYPE LO HI`: value ranges for fields and globals, with the edges
    drawn more often;
  - `ret ADDR LO HI`: result domains of callees (e.g. `dComIfG_resLoad` 0..5, `fopAc_IsActor` 0..1).
  
  Generated floats come partly from a dictionary of the `.rodata` constants the original reads,
  so `<` vs `<=` boundaries are hit. The dictionary's pre-runs always run with mocked-call clobbering off (2026-10-29:
  they used to inherit the switch from whatever the process ran before, so a function's generated inputs differed between
  a whole-unit run and a run of that function alone; AGL#17 027C93A8, AGL#1 0276DE44/02770564, AGL#4 0277C578/0277EBF4,
  AGL#16 027C2DF0/027C356C, AGL#18 027CD1E4/027CE130).
- **Recorded:** real calls captured in the running game.
  1. `mktap.py ADDR...` adds a git-ignored `tools/recomp/hooks_tap.txt` and re-runs the
     recompiler.
  2. It then writes `build/gen/code_tap.c`: instrumented copies `tap_ADDR` whose own loads,
     stores and calls are logged.
  3. With `WWHD_TAP=dir` set (optionally `WWHD_TAP_N`, `WWHD_TAP_EVERY`, `WWHD_TAP_AFTER`), each
     call is written to `dir/ADDR/n.tap`. Recording is off by default, and `mktap.py --clean`
     restores the normal build.
  4. The harness rebuilds each recording into a replay:
     - the snapshot is the first value read at each address;
     - callee effects are values F reads that changed since it last saw them; they are attached
       to the preceding call;
     - first reads of F's own frame after a call are callee outputs.
  5. Before a recording is used, the original's replay must reproduce it: the same calls, the
     same writes, the same result, and no reads outside the snapshot.

## Writing candidate source (`include/gabi.h`)

```cpp
struct daMtoge_c : fopAc_ac_c {          // WWHD layout, be<T> fields, explicit padding
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3C0 */ be<f32> mHeightOffset;
};
WWHD_OFFSET(daMtoge_c, mHeightOffset, 0x3C0);

/* 021E01B8 */
BOOL daMtoge_actionUp(daMtoge_c* i_this) {
    WWHD_FUNC(0x021E01B8, BOOL, i_this);   // first statement: address, return type, arguments
    cLib_chaseF(&i_this->speedF, 30.0f, 4.0f);
    ...
}
VERIFY(0x021E01B8, daMtoge_actionUp);
```

### API

- **Structures:** `T*` is a token for a guest address (`PPC_MEM_BASE + EA`).
  - Fields are `be<T>` (big-endian, accessed through the harness). Pointers are `gptr<T>`
    (32-bit). `gabi::at<T>(ea)` and `gabi::ea(p)` convert.
  - Layout structs are never copied (`be` has no copy constructor).
- **Calls to other WWHD functions:** `gabi::call<R>(addr, args...)` follows the EABI:
  - integers and pointers in r3..r10, then on the stack;
  - floats in f1..f8;
  - result from r3 or f1, typed by `R`.
  
  Bindings with real names live in `wwhd_src/include/bindings.h`.
- **Calls between decompiled functions** are written as natural C++ calls. `WWHD_FUNC` turns a
  nested call into a guest call to that function's address, so every function is tested alone.
  A native build can route the address to the implementation.
- **Locals whose address is passed to guest code** use `gabi::Local<T>`, guest stack storage.
  Unit-file lines may be up to 16 KB; a longer line stops the run with an error (it used to be cut silently at 511 characters).
  A `gabi::Local` declared inside a loop body is re-created (and re-zeroed) on every iteration, while GHS gives the temporary one frame slot that keeps its contents across iterations: declare such Locals before the loop.

### Rules learned in the pilot (each one was a real failure first)

**Floats** (the recompiler's semantics, `tools/recomp/ppc2c.py`):
- `f32` arithmetic is IEEE single, which is exactly fadds/fmuls/fdivs.
- Where GHS contracted `a*b+c` into `fmadds`, write `gabi::fmadds(a, b, c)`. The recompiler
  computes it in double and rounds once more to single, which is neither a fused nor an
  unfused single result. Compile with `-ffp-contract=off`.
- Watch the operand order of fused terms: `x*x + z*z` became `fmadds(x, x, z*z)`.
- Float-to-integer conversion is `gabi::ftoi` (fctiwz: truncating, saturating, NaN → INT_MIN).
  A float passed to an `s16` parameter is `(s16)gabi::ftoi(f)`; C++ conversion is undefined
  out of range.
- A float loaded into an FPR (lfs) and used in arithmetic quiets a signalling NaN; `be<f32>` reads model that. **But a pure copy (`lfs` followed directly by `stfs`, no arithmetic) keeps a signalling NaN bit-exact in the recompiled original** (the single→double→single round trip is folded), so write such copies as 32-bit bit copies (e.g. `gabi::store<u32>(dst, gabi::load<u32>(src))`), not through an `f32` value (found independently in d_a_btd hahen_set, d_kankyo setLightTevColorType and c_m3d_g_lin; 2026-10-05).
  - GHS copies a `cXyz` struct with integer loads (bit-exact, `cXyz::copy`), but goes through
    FPRs when it computes with it.
  - Matrix copies load all twelve values, then store them (`mtx_copy`); this matters when the
    source and destination overlap.
- NaN payload tolerance applies only to f1 results and to memory words that one side last wrote with a float store (`stfs` / `store<f32>`), or with a 32-bit integer copy of such a word (GHS struct copies); integer words that happen to look like NaNs (two s16, flags) must match bit for bit.
- NaN-vs-NaN results are unspecified: which payload survives depends on the host compiler's
  operand order, in the original too. The harness treats two NaNs as equal. With `-strictnan`,
  2 of 10000 kamome_bgcheck inputs differ only in NaN payloads.
- GHS branches on the negated comparison, so with NaNs `a <= b` is not `!(a > b)`. Write the
  form the code tests.

**Ordering**:
- Under clobbering, a field must be loaded where the original loads it relative to calls.
- C++ argument evaluation order is unspecified, so sequence explicitly:
  `p = i_this->mpMorf; env = dKy_getEnvlight(); f(env, model(p))`.
- Stores before or after a call must stay on the same side of it.

**GHS / HD artefacts that appear in source**:
- **Constructors allocate when `this == NULL`:** `new dBgW()` is `dBgW::dBgW(NULL)`.
- **HD classes have a virtual destructor:** an actor's vtable pointer is at +0xB4, written by
  `fopAcM_ct`. Inline sub-object constructors write their vtables.
- **Function-local statics are initialised on first use:** a guard word plus memcpy from `.data`
  (`l_action` in `daMtoge_Execute`).
- **String literals are not pooled:** each use of `"Kamome"` has its own address.
- **Resource names are `sead::SafeString` temporaries** `{const char*, vtable}`, with one
  SafeString vtable per translation unit.
- **GameCube globals became singletons behind accessors:** `dComIfGp_get()` (0x025200D4) and
  `dKy_getEnvlight()` (0x02555D0C); save info is at `*(0x101F84DC) + 0x20`.
- **Address-of a model's base matrix is null-preserving** (`J3DModel_getBaseTRMtx`).

**Matcher names are hypotheses.** A binding's types are checked by the harness. A wrong name
(e.g. `02555D0C` named `setLightTevColorType`, which is the env-light accessor) shows up as a
call/argument mismatch.

### Automatic native frames (2026-10-05)

Every decompiled function entered from guest code now gets the original's stack frame, so a
swapped function leaves the guest stack as the original does (game test: stale stack bytes reached
the empty slots of cking.sav through `initdata_to_card`, under a ~20-deep main-thread chain).

- `frames_all.py` reads every function's prologue from the binary (up to its first branch) into
  `build/verify/frames.tsv` (verify.py regenerates it with build/gen): frame size (`stwu r1,-N`),
  back chain, where LR is saved, and the callee-saved registers saved with their entry values
  (`stmw`, `stw rN`, `stfd fN`, GHS's `ps_merge10`+`stfs` second half, `psq_st`). Frameless
  functions that only tail-branch (`b`/`bctr`) get size 0 and flag `tail`. Not described (today's
  behaviour, listed by `frames_all.py --list`): 7 shared epilogue stubs; no stwux / r12 frames exist.
- `native_frames.cpp` loads the table (`$WWHD_FRAMES`, default `build/verify/frames.tsv`;
  `WWHD_NATIVE_FRAMES=0` turns the whole mechanism off). The harness includes it; a native or
  game-test build links it (gabi.h has a weak null default: no table, no change).
- `WWHD_FUNC` constructs a `gabi::AutoFrame`: the prologue stores are replayed at the original's sp
  (from `cpu->r`/`cpu->f`/`cpu->lr` at entry); r1 stays at the entry sp during the body (stack
  arguments at r1+8 and hand-built frames keep working), and do_call starts every callee at the
  original's sp while r1 == entry. For a `tail` function every call runs at the entry sp (like
  `gabi::tail_ptr`).
- `gabi::Local` objects keep their established placement (r1 lowered by the Local, entry - size):
  many candidates are written against it (hand-emulated linkage words, one big Local as the frame,
  strcpy overflows into the caller's linkage), so while a Local lives its callees run below it as
  before. `WWHD_NATIVE_LOCALS=1` instead places Locals inside the original's frame, upward from its
  first address-taken object (frames.tsv `o:OFF`, 4-aligned); a Local that does not fit is reported
  (stderr `[nframe]` and build/verify/frame_fallbacks.tsv). In a 3-input survey of all units that
  mode broke 18 candidates (d_a_st, d_a_beam, d_a_ship, ...) and left 423 Locals in 421 functions
  that do not fit, so it is off. Stack arguments go to the frame's parameter area sp + 8... when no
  Local lives, as GHS does. `gabi::NativeFrame<SIZE>` of the same size is a view of the automatic frame.
- `gabi::FrameLocal<T> obj(OFF)` (2026-10-06): a stack object at the original's frame offset sp+OFF
  (from the disassembly) inside the automatic frame: r1 stays, callees keep the original's sp, the
  bytes are not initialised; without an automatic frame it is a gabi::Local. `gabi::native_sp()`
  gives the original's sp for frame stores the original makes (e.g. hd_main's lfd conversion
  scratch). Used where a candidate holds a stack object while calling on the main-thread chain:
  f_pc_create_iter method/judge (filter at sp+8), hd_main (all its objects); `footprint` gates them.
- Not covered: the content of the original's own temporaries/locals in the frame (only Locals the
  candidate declares, at its own offsets), the callee-saved register *values* the original holds
  while it calls (callees save the candidate's caller's values), and the LR a callee sees (it is
  the candidate's caller's return address, not the original's return address after its `bl`):
  for the frame-exact state loader (same_chain) r1 and every back chain now match; the LR slot a
  callee stores at [sp+4] does not yet.
- Checks: `-calleesp` (every callee must start at the original's sp) and `-footprint` (also the
  whole frame, sizes from the table) on the verify command line survey a unit; the unit directive
  `footprint FUNC SIZE` makes it a gate for one function.

## GameCube-style source (opt-in: `autoglue.py`, `wwhd_src/include/gc_api`)

A unit can be written like the GameCube decompilation (zeldaret/tww): class methods, GameCube API
names with real types, string literals, enums and PTMF tables, and only a `/* ADDR */` comment
above each function. Example: `wwhd_src/d/actor/d_a_npc_nz.cpp` (readability proof of concept).
Units written with `WWHD_FUNC`/`VERIFY` are unaffected.

```cpp
/* 022A400C */
void daNpc_Nz_c::checkOrder() {
    if (eventInfo.checkCommandTalk() && (mTalkMode == 1 || mTalkMode == 2)) {
        if (mTalkMode == 2) modeProcInit(1);
        mTalkMode = 0;
    }
}
```

**Opt-in:** the file ends with a glue comment block `/* WWHD glue ... */` listing the
translation unit's data:

| Glue line | Meaning |
|---|---|
| `str ADDR ["source text"] [in FUNC]` | a string literal's guest copy (content read from the RPX image; `"source text"` when the source spells it differently, e.g. `JUT_ASSERT` prints NULL as `(0)`; `in FUNC` for duplicates). Texts are read as C++ literals (escapes, UTF-8); a Japanese literal may be written in UTF-8 (`"振動効果"`) or as the Shift-JIS bytes (`"\x90U..."`): both match the image. Without source text the registration takes the bytes of the source's own literal for that text. The registration is byte-exact (control and non-ASCII bytes as octal escapes) |
| `data NAME ADDR [guard GADDR] [nocheck]` | a `static` table / object / file-scope static at that address; `guard`: function-local static with a GHS guard word. Initialisers are compared with the image at build time (PTMFs `&C::m` against the marked methods, strings by content, numbers by the element layout parsed from the headers; a number may be an enum constant of the source or its headers, `leaves_particle::ID_AK_SN_FALLENLEAFFAN00`, or a constant expression of them) |
| `ext ADDR SIGNATURE [callback]` | a function of another unit called from this one (e.g. `ext 021842B8 void daItemBase_c::hide()`): bound by address. A member declared `static` in its class takes no `this`; reference parameters are passed as addresses. `callback`: the source also passes the function by name (e.g. a heap callback `fopAcM_entrySolidHeap(this, useHeapInit, size)`), so the binding is registered for `gabi::guest_fn` |

**What `autoglue.py` does** (called by `verify.py`, `mutate.py`; `mkunit.py`/`tu.py` read the
markers; output in `build/verify/<unit>/glue/`, line numbers kept):
- inserts `WWHD_GLUE_FUNC(addr, R, [this,] args)` after the `{` of each marked function (same
  as `WWHD_FUNC`, and records the function under test) and `VERIFY` at the end; a marked
  constructor `X::X()` gets a guest wrapper `X__ct(X* self)` (GHS: allocates when `self` is NULL);
  a member function declared `static` in its class gets no `this`; a reference parameter
  (`const cXy& p`) is passed on as its address (the guest ABI); const member functions register
  as they are (`f32 GetLen(const cXy&) const`); an overloaded method (declared more than once in its
  class, defined here or in another source) is registered by a typed cast and shown as
  `&C::f(types)` (`VERIFY_NAMED`);
- turns class-typed locals (`cXyz pos(a);`, `cXyz d = a - b;`, `dBgS_GndChk chk;`) into guest stack
  slots (`gabi::LocalOf`/`gabi::Stack`; construction/destruction hooks `gabi_construct` /
  `gabi_destroy`, found by ADL);
- turns `static` tables and statics with a `data` line into views of their guest copies;
- replaces `#include "assets/X.h"` by the header that `assets.py` generated from your game (see below);
- registers the strings.
Namespaces (2026-10-21): a marked free function defined inside `namespace A { ... }` is registered by its qualified
name (`A::f`; shown as `f`), so a same-named global such as libc `bcopy` cannot capture it; a definition
`R A::B::f(...)` whose qualifier is a namespace (declared in the source or an included header) is a free function
(no `this`). Self-test `autoglue_ns_test.py`.
Speed (2026-10-26): `mask()` and the per-header namespace scan are memoised (pure functions of the text) and the
include closure per include lines (reused while every header's mtime and size are unchanged), so the headers are no
longer re-read and re-lexed ~100 times per transform; `mutate.py` transforms every mutant, which cost up to 2.3 s CPU
per mutant on large sources (now ~0.1 s). Same output; self-test `autoglue_cache_test.py`.

**Model data (`assets.py`, `assets.txt`):** as in zeldaret/tww, model data that the game embeds in
its code image (vertex positions, texture coordinates, vertex/index lists; textures) is not written
into the sources. `assets.txt` lists each header with the declaration it holds and the image address;
`assets.py` writes the headers from `build/verify/image.bin` (your `game/code/cking.rpx`) into
`build/verify/include/assets/` (verify.py does this before every build; they are git-ignored and never
committed). A source includes one (`#include "assets/l_pos__d_a_sail.h"`) and keeps its `data` line;
autoglue expands the include on one line and checks the table like any other. Code-like tables
(animation, parameter, mode, collision, HIO defaults, strings, math tables, vertex formats and counts)
stay literal in the source.

**gabi additions (opt-in, backward compatible):** a host string literal passed to a guest call
becomes its guest address (`gabi::guest_str`, by content); a decompiled function passed by name
becomes its address (`gabi::guest_fn`, VERIFY registry); `gabi::ptmf<F>` + `operator->*` (GHS
pointer-to-member dispatch from guest tables); `gabi::Into<T>` (values returned through a hidden
result pointer: `cXyz d = a - b;`); `gabi::Stack`, `gabi::LocalOf`, `gabi::local_static`,
`gabi::object_at`.

**`gc_api/`:** one header per GameCube header with the GameCube names mapped to WWHD (HD
differences marked `HD:`), `gc_api/tu.h` for the per-TU vtables (`gc_tu`, defined once by the
unit). Shared layouts gained GameCube-named members: `fopAc_ac_c::eventInfo`/`attention_info`,
`dNpc_JntCtrl_c`, `mDoMtx_stack_c`, `cXyz` (`operator-`/`+`, `*=`, `absXZ`, `Zero`),
`JSystem/J3DGraph.h` (J3DModel/ModelData/Joint/Shape/Material, `j3dSys`), `dPa_smokeEcallBack`,
`daTsubo::Act_c::pos_init`, `msg_class` (HD message manager).

Semantics that stay visible in source: fused float ops (`fmadds`, `fnmsubs`), float->int
(`ftoi`), and HD differences (`// HD: ...`).

**Readability pass:** style guide and acceptance bar in
[docs/readability-style.md](../../docs/readability-style.md); tools `readable_check.sh` (function set,
spec files, 10k seeds 1/7, coverage vs the reference commit, `--ref`), `readable_lint.py`, `objcmp.py` (object
identity for header changes), `readable_names.py` (named-call pass, `wwhd_src/include/fn_names.h`).
Descriptive renames of address-named sources go into `readable_renames.tsv` (`old<TAB>new`, same commit
as the `git mv` and the spec `src` change): `readable_check` then accepts the spec (function set still
compared with the old file), `objcmp` compares the new file with the old one; self-test
`readable_renames_test.py` (an unmapped rename must fail). Functions that move from one unit spec to another go into `readable_moves.tsv`
(`ADDR<TAB>old spec<TAB>new spec`, same commit as the move): check 1 accepts the moved function set, a spec removed in
HEAD passes when all its functions are listed as moved out, and the moved function is verified with its new spec and
compared with the old spec's REF coverage; self-test `readable_moves_test.py`. `readable_check --static-only` runs only the
function-set and spec checks. `readable_check` shards every run (`--shards K`, else `WWHD_SHARDS`, else 4 at load
<= 30 and 2 above, starting only at load <= `READABLE_LOAD_MAX`, default 55) and runs scoped companion specs
with their published `-only` set: `readable_scopes.tsv` (`spec<TAB>ADDR[,ADDR...]  # source`) or a
`run with -only ADDR` / `readable-scope: ADDR` comment in the spec header (`--unscoped` for diagnostics).

## Workflow for one actor (scales to many in parallel)

1. **Pick the unit.** One GameCube translation unit. List the WWHD functions of its range:
   `build/names.tsv` plus the unnamed functions in between.
2. **Layout.**
   - Start from the GameCube header shifted by the fopAc_ac_c delta (+0x11C) and fix it from
     the disassembly (`wdis.py`).
   - Put the layout in `wwhd_src/include/...`, with `WWHD_OFFSET` asserts.
3. **Port** each GameCube function. Add bindings for its callees (address + types), with
   `WWHD_FUNC`/`VERIFY`.
4. **Unit file** `tools/verify/units/<unit>.txt`:
   - `src` lines;
   - `field`/`ret` steering until the coverage column is close to full;
   - `callee ADDR rN=SIZE` for stack objects (`rN=255`: output storage, not compared).
5. **Run** `verify.py <unit> -n 3000`.
   - A failure prints the first difference and the input (`-trace SEED` for a generated one).
   - Fix the candidate until all generated inputs pass.
6. **Record:**
   - `mktap.py --unit <unit>`;
   - build the game in a scratch clone;
   - run a scripted session with `WWHD_TAP=dir`;
   - `verify.py <unit> -rec dir`.
7. **Mutation test.** `mutate.py <unit>` should leave only equivalent mutants (swapped
   independent stores and the like). A non-equivalent survivor means an input gap: add steering.
   **Per-mutant scope (2026-10-27, default):** each mutant runs only the functions whose rows can execute its changed
   code (harness `-only`; with `--fork` the same set as a `-runlist` of `ADDR all` lines). The harness runs candidate
   code only for the row's own function: every marked function starts with the nested-call check
   (`WWHD_FUNC`/`WWHD_GLUE_FUNC`: a nested call goes to the harness by address), and `real`/`setup`/`postsetup`
   targets run the ORIGINAL (`real_ADDR` from build/gen), never the candidate. So the set (`tools/verify/mutscope.py`)
   is: a change in a marked function's body after that check → that function; in an unmarked helper → every marked
   function naming it, directly or through other helpers; code touching a host static (a file-scope variable without
   a glue `data` line, not const) → also every function naming that static. The whole campaign scope is used when
   this cannot be bounded: constructor bodies and init lists (no nested check), code in front of `WWHD_FUNC`, anything
   outside a function definition, a name used in file-scope code (static initialisers, host tables, macros), a helper
   used in an included header, operator/destructor helpers, and a helper (or a touched host static) reaching a function listed `real`
   or as a `setup`/`postsetup` target in the spec (kept conservative). Since 2026-10-30 a change in such a
   listed function's OWN checked body scopes to that function like any other (before: the whole campaign scope): outside
   its own row it runs as the original, and the nested-call check keeps every other call away from the candidate body. Recorded mutants (`--rerun`) are located by their
   difference from today's source. Since 2026-10-30 `--rerun` also works with `--schemata`, `--fork` and
   `--reach-skip`: each recorded mutantN.cpp becomes one edit of today's source (the span where it differs; text before
   and after it unchanged, so the built text is the recorded one), and the schema builds, fork servers and reach markers
   work as in an enumerated campaign (a mutant no input of the spec reaches is `survived` with reach `not reached`, never
   run, never a kill). The unmutated schema check reads the baseline's original-side records (`-refcache`; the setups
   and the reach recording stay live; `WWHD_MUT_SCHEMA_CHECK_NOCACHE=1` runs the original live). Self-test
   `mutate_rerun_fast_test.py`. With `--reach-skip`, functions the marker build saw run the site are always added
   (stderr note, receipt `scope_widened_by_reach`). Receipts name `scope` (the functions, or `whole` plus
   `scope_reason`); the summary prints how many mutants ran scoped. The baseline gate and the timeout baseline still
   run the whole campaign scope. `--whole-unit` restores the old behaviour. Self-test `mutscope_test.py`.
   **-stopfirst on every mutant run (2026-10-27, default; before only with `--reach-skip`):** the first row whose first
   difference is an output difference ends the run, a row whose first difference is an abort ends that row only, so
   the difference/abort split and every verdict are unchanged (a passing run has no differing row to stop at). A scoped
   run no longer gets the early end from another row's kill, so without it a mutant that loops on most inputs of its
   own row ran to the timeout (AGL#10 #266/#276: killed at input 0, then about 1 h at the 2x retry). `--no-stopfirst`
   runs every row to the end; the baseline gate always runs every row.
   **Identical-code check (2026-10-28, default; `--no-tce` switches it off; `mutcode.py`):** every mutant is compiled
   exactly like a per-mutant build (clang++ -std=c++20 -O1 -w, the verify FP flags and include paths, the autoglue /
   `#line` text) under the unit source's own file name (in `mutantN.tu/`; the name reaches the object only as the
   static-initialiser symbol `__GLOBAL__sub_I_<name>`), and the whole object file is compared byte for byte with the
   unmutated source compiled the same way (clang's objects carry no time stamp and, without -g, no path). Identical
   bytes = identical code, data, relocations and symbol references: the mutant is closed as `equivalent-identical-code`
   (receipt `reason: compiles to identical machine code`, `object_sha256`), never linked or run, and counted apart from
   kills and survivors (not in the kill rate). Mutants whose objects are identical to EACH OTHER and that run the same
   configuration (function scope and input selection) run once: the first is the representative, the others take its
   outcome with its run fields and name it (`tce_same_code_as`). The check runs after the host-symbol checks, so
   `broken` stays `broken`. Schema mutants (`--schemata`) have no object of their own: their variant is compiled for
   the check only when the run did not kill them (survived or timeout; the run's fields stay in the receipt as
   `run_class`), and before the run with `--ids` (a survivor replay, where the compile costs less than the run).
   Survivor replays with `--rerun` build per mutant, so the check is free there.
   **Schema survivors are confirmed by their own build:**
   a schema executable can fold a read of an uninitialised local (e.g. a dropped memcpy into an f64) differently from
   the mutant's own build (AGL#8 2086/2825 survived the schema run and died on input 1 of their own build). So a schema
   mutant that survives (and is not identical code) is built and run again like a per-mutant build; that run's receipt
   replaces the schema run's and carries `schema_survivor_rerun`. Kills count as usual; survivors stay survived. The
   cost is one build and run per survivor. Self-test: `mutate_schemata_test.py`.
   **Reference-side cache (2026-10-28, default; `--no-refcache` switches it off):** the campaign's unmutated baseline
   run (same spec, -n, seed, header flags, nulltrap, scope, recordings) also writes the original's per-input results
   (harness `-refcache-write`, a per-campaign directory next to the mutant scratch, removed at exit; key = sha256 of the
   unit.c/FP-runtime/harness/support objects, the expanded spec, the harness options and the recordings), and every
   mutant run reads them (`-refcache`) instead of running the original again. Rows, trap/NULL handling and totals are
   exactly those of a run without it; the summary prints the hit rate. `WWHD_REFCACHE_CHECK=1` adds `-refcache-check`
   to every mutant run (validation: the original runs anyway and every record is compared with it). The baseline
   timeout measurement never reads the cache. It is skipped when less than 2 GB is free where it would be made;
   `WWHD_REFCACHE_MAX_MB` (default 1024) caps one function's file.
   **Digest records (2026-10-31, fasttools2, default; `--refcache-full` writes the full records):** full records hold
   the original's net memory effect and every call per input, 1-7 GB per function at n10000 on AGL#19 banks/commands/
   shader specs, so those functions ran their original next to every mutant (about 2.8x slower). A digest record (harness
   `-refcache-digest`, magic RFD1) keeps per input only what the candidate's run and the outcome need exactly (each
   top-level call's target/kind/LR/dead-register mask, V.orig_stack as runs, coverage, statistics, exit registers,
   abort/stop/DSI/model-rejection state, `why`, strays, traps, bad fetch targets) plus compare_full's argument masks per
   call and a 128-bit hash of the rest (the calls' compared parts under those masks, stack-object pointers relative,
   the net effect, the `final` bytes): typically 0.3-1 KB per input. A reading run takes the original from the record
   when the candidate's part hashes the same (the comparison then runs on the exact part plus the candidate's heavy
   part), needs no comparison when a side aborted without a mock-model rejection, and otherwise (a heavy difference,
   other masks, a model rejection, `-v -v -v`) runs the original live after the candidate (coverage and statistics not
   counted twice) and compares in full: verdicts, rows and notes are those of an all-live run. The writer prints how
   many inputs the unmutated candidate matches; the campaign summary prints matched / abort-only / live-fallback counts.
   `-refcache-check` checks the exact part, side and heavy hash (under the recorded masks) against the live original;
   `-refcache-digest-miss` (self-test) forces the live fallback everywhere. Self-test `refcache_digest_test.py`.
   **Setup-snapshot cache:** for
   `setupsnapshot` specs the baseline also writes each function's snapshot and its validation inputs' post-setup states
   (harness `-snapcache-write`, files `ADDR-TAG.snap`, `world-HASH.blob`, `ADDR-TAG-sSEED.val` in the reference-side cache directory: removed with it,
   and kept with it in a `--baseline-cache` entry), and every
   mutant run loads them (`-snapcache`) instead of running the setups; rows are the same. `WWHD_SNAPCACHE_CHECK=1` adds
   `-snapcache-check` to every mutant run (validation; exit 98 = `invalid`); `WWHD_SNAPCACHE_MAX_MB` (default 1024) caps
   one file. The summary prints the snapshot and validation-state hit counts.
   **`--observe-pointees`** passes harness `-observe-pointees` to the baseline and every mutant run (see the option table).
   **Baseline cache:** a survivor
   cascade runs one campaign per companion spec, and each ran its baseline gate (and wrote its reference-side records)
   again on every resumed or repeated cascade. With the option a PASSING baseline is recorded once in `DIR/<key>/`
   (receipt.json: verdict, exit code, the harness stdout the campaign takes its rows from, wall time and 1-minute load
   after it, the sha256 of each record file; `refcache/`: the records), and a later campaign with the same key takes the
   verdict, rows and records from there (stderr `cached baseline KEY from TIME, not run again`; receipts carry
   `baseline_cache: {key, use: hit|stored|ran}`). The key (`tools/verify/basecache.py`) is exact: the full baseline
   harness command, the verify executable's bytes (every linked object), the spec file and expanded spec text, the
   run-time files (image.bin, livein/frames/statics.tsv, build/*.tsv, ctor_sizes.tsv/stage_names.txt, `blob` game
   files, recordings), the tool version (tools/verify except units/recordings), gen, compiler, OS, the WWHD_*
   environment except progress/scratch/slot/cache variables, and the reference-side cache key. A failing baseline is
   never recorded (a lookup also re-checks the stored run), so a campaign on a failing baseline stays void; with
   `--allow-failing-baseline` exclusions nothing is recorded. A changed record file makes the entry unused. The timeout
   policy starts from the recorded wall time at its recorded load (measured again when the load has moved by more than
   half, as before). Entries are never changed or removed by the tools (point DIR into the cascade's output directory;
   `basecache.py ls DIR` lists them with sizes; records are as large as the campaign's, up to a few hundred MB). A
   per-key flock makes a second campaign with the same key wait and then reuse it. `--baseline-only` builds, runs the
   gate (recording it) and exits 0/3 before any mutant; `basecache.py prefetch --cache DIR [--parallel 2] [--load-max
   85] [--log-dir D] SPEC... -- MUTATE_ARGS` (or `basecache.prefetch()`) runs all companions' baselines that way up front,
   at most 2 at a time and only below the load limit, so the passes then hit. Self-test `basecache_test.py`.
   Proof (2026-10-27, AGL#10, 150 survivors stratified + 200 kills of the archived campaign, n 1000, seed 1,
   --nulltrap): scoped (mean 1.2 of 129 functions) and whole-unit give the same verdict for every mutant.
   Sharded serial phases (2026-10-31, fasttools2): the baseline gate, the `--schemata` check of each schema executable
   and a `--timeout-remeasure` measurement run as `-range` parts in parallel (`--shards K`, default `-j`, at least 50
   inputs per part; WWHD_MUT_SHARDS=1 or `--shards 1` for one process), merged as `verify.py -shards` merges
   (`verify.run_range_shards`); the record parts (`ADDR.ref.LO-HI`) and the setup-snapshot validation-state parts
   (`ADDR-TAG-sSEED.val.LO-HI`, new) are joined (`refcache.join_shard_dir`), the check's `.reach` parts concatenated.
   The per-mutant timeout's base is the parts' summed wall time. A part's first input of a `setupsnapshot` function runs
   its setups for real (a validation input) only in that part: its original side then carries the setups' stack bytes,
   so it is left unrecorded and readers run it live; a reader likewise runs live the first input of a `-runlist` subset
   that validates only there (it read a record made where the input was plain). Self-test `mutate_shard_test.py`.
   `--rerun` (2026-10-31, fasttools2): the baseline gate, and with it the reference-side records, the setup snapshots
   and the timeout base, runs only the union of the rerun mutants' per-mutant scopes (the function list comes from the
   harness's `-list`, so the scopes are the ones an unscoped campaign computes); the whole campaign scope runs when one
   mutant's scope cannot be bounded, with `--whole-unit` / `--allow-failing-baseline`, or with
   WWHD_MUT_RERUN_BASELINE_WHOLE=1. The receipts name `baseline_scope`. A function's rows and records do not depend on
   what ran before it in the process (dictionary fix 2026-10-29): self-test `mutate_rerun_scope_test.py` compares rows and
   record files of scoped and full harness runs and the verdicts of scoped and whole rerun baselines. A function outside
   the union no longer voids the pass when it fails unmutated (no rerun mutant can change it, no mutant run includes it).
   Before that, each mutant ran the whole unit (helpers can be shared). Each run has a per-run limit. A run that hits the limit is
   reported as `timeout` and not counted as killed. Since 2026-10-26 the limit is max(60 s, 10x the wall time of the
   unmutated unit's own run at the campaign's `--n` and scope), measured at the start and again between mutants when
   the 1-minute load has moved by more than half (since 2026-10-30, in a background thread: the workers no
   longer wait for it; until it finishes the last limit scaled up by the load ratio, never down, applies), and a run over it is retried once at twice the limit before it
   counts as `timeout` (at load 85-139 a clean lib2_sead_8 run took 27 s wall for 12 s CPU, so the old fixed 60 s
   classed 88 of 300 good mutants as timeouts). Since 2026-10-31 (fasttools2) the limit is measured once only: the
   baseline gate's own run (or a `--baseline-cache` entry's recorded wall time and load) is the base for the whole
   campaign and the limit follows the 1-minute load up (never down) from there; the background re-measurement re-ran the
   whole unmutated baseline (AGL#19 seed-1 n10000 replay: 2-4 extra full baselines per big spec). `--timeout-remeasure`
   brings the re-measurement back. The limit used is printed and kept in the receipts
   (`timeouts_tried`). `--timeout N` fixes the limit at N seconds with no retry (the old behaviour; self-test
   `mutate_timeout_test.py`); `cascade.py --timeout N` passes it to every pass (self-test `cascade_timeout_test.py`).
   A timeout is never a kill. If a run still times out, steer the slow function first (a loop bound read after a mocked call needs `stable`, or a pinned `this` via `arg!`).
   Only one `mutate.py` per unit and build directory at a time: runs share `build/verify/<unit>`. Since 2026-10-26 a
   second campaign waits for the first (an flock on `.<unit>.mutate.lock` next to the unit's build directory, with
   one stderr note); the mutate self-tests each build in their own `WWHD_VERIFY_SCRATCH` temp directory, so they can
   run in parallel.
   The mutant object replaces the mutated source's object by exact file name (`vec.cpp.o`, never
   `mtxvec.cpp.o`); two sources of one unit with the same file name stop the run (self-test
   `mutate_objsel_test.py`, 2026-10-21).
   `--only ADDR,...` sets the test scope: every mutant run checks only those functions (harness `-only`) and
   mutants are made only in them (`--func` must be a subset); for specs that link support sources the unit does
   not own (self-test `mutate_only_test.py`, 2026-10-21).
   The summary splits "killed" by cause: `difference` (a function row differs: an observed output
   difference), `abort` (only the mutant side aborted), `signal` (host signal or crash) and `invalid` (no
   differing row: a harness or spec error). Only `difference` counts as observed without review.
   `--receipts DIR` keeps one record per mutant: `mutantN.cpp` (the mutated source) and `mutantN.json`
   (function, hunk, compile/link/run return codes, stdout/stderr tails, class), so a final campaign can
   name every ID (self-test `mutate_classify_test.py`, 2026-10-22).
   Since 2026-10-27 receipts are kept by default: in `<scratch>/_receipts/<unit>-<date>-<time>-<pid>` (the
   `WWHD_VERIFY_SCRATCH` tree directory, else `WWHD_MUT_SCRATCH`, else `build/verify`; stderr names the directory, archive
   it as before); `--receipts DIR` chooses the directory, `--no-receipts` keeps none.
   **Survivor cascade:** reruns a main campaign's alive mutants (their recorded mutantN.cpp, same IDs, `mutate.py SPEC --rerun`) on each companion spec whose scope holds them (`src` of the spec, `-only` functions), in order; a kill (the zero-page kill rule: a clean difference or a candidate-only fault; original-only/both-sides faults and aborts are `difference-page0` and stay alive) removes the mutant. Single stage at `--n2` (1000) by default; `--two-stage`: stage 1 at `--n1` (100), stage 2 the stage-1 survivors at `--n2` on the same companions; the inputs of n1 are the first of n2, so the final alive set equals the single-stage one (`--single-stage` is accepted as a no-op alias). OUT/alive.json keeps each mutant's history ([stage, spec, class]) and first killer; OUT/progress.json makes a rerun resume after the last finished pass; receipts in OUT/stage1|stage2/rc-SPEC. Load gate `--load-max` (90), optional `--slot-dir/--slots` job slots, and the opt-in `--baseline-cache`/`--prefetch`/`--refcache-from`. Proof (AGL#19 banks x4 and AGL#7 render/projection/camera x4, 60 survivors each, seeds 1 and 7): identical final alive sets and first killers; all kills happened in stage 1. Without schemata/reach on --rerun each pass compiles every mutant again, so two-stage was slower on these specs (AGL#19 270-333 s vs 210-223 s): it pays off only once schemata/reach on --rerun ends the per-pass rebuilds, or on specs with long n1000 runs. `--fast` gives the same classes (914 per-pass results on the same 240 survivors, single and two-stage: identical, 29 `not reached` instead of survived) but was 1.6-5x slower there: a schema executable's survivor run took 41.6 s where the individually built mutant took 16.1 s (lib2_agl_19_banks_release, n1000), and these passes hold only 9-40 mutants, so the saved compiles do not make up for it; keep it for passes with many mutants and short runs. Self-test `cascade_test.py`.
   **Baseline gate (2026-10-27, AGL#8: 3,165/3,165 "killed" on a baseline whose row 0279345C already failed):** before
   any mutant, mutate.py runs the unmutated unit in the campaign's exact configuration (spec, `--n`, input seed, spec-header
   flags, `--nulltrap`, `--only` scope, recordings). If it exits nonzero or any function row is not ok, it prints
   `mutate.py: the unmutated baseline (...) does not pass: exit 1, K of N function row(s) not ok`, every failing row, and
   `mutate.py: no mutant was run. A campaign on a failing baseline is void ...`, and exits 3. A campaign whose unmutated
   baseline fails is void: no kill counts; fix the spec or candidate and rerun. `--allow-failing-baseline` instead
   leaves the failing functions out (not tested, not mutated), reruns the baseline without them, and names them in
   every receipt (`baseline_excluded`). With `--schemata`, every schema build failing unmutated (while the baseline
   passes) also stops the campaign. A passing row that ends in `ok (some inputs aborted on both sides)` no longer counts
   as a differing row in the classification. Self-test `mutate_baseline_test.py`.
   `--seed S` orders the mutant selection and, since 2026-10-25, is also the harness input seed (`-seed S` on every
   mutant run; before that every campaign ran generator seed 1 whatever `--seed` said); `--input-seed S` sets the input
   seed separately. `--rerun DIR [--rerun-class survived]` reruns exactly the mutants recorded in receipts DIR of those
   classes: the recorded `mutantN.cpp` (same variant, same ID, whatever today's enumeration or `--max`), with this
   run's `--n`/`--input-seed`/`-j`; receipts of the rerun name `rerun_of`. Survivor lines print `ADDR #ID mutation`.
   Rerun of a campaign's survivors at both seeds:
   `mutate.py UNIT --rerun A --n 10000 --input-seed 1 -j 8 --receipts A-s1` and the same with `--input-seed 7`
   (self-test `mutate_rerun_test.py`).
   Receipt-free replay: `--ids ID,...` with the campaign's own arguments (same `--seed`, `--max`, `--func`/`--only`,
   unchanged sources) regenerates the same enumeration and runs only those IDs (the `#ID` printed on survivor lines),
   e.g. `mutate.py UNIT --only F --seed 1 --max 400 --ids 17,203 --n 10000 --input-seed 7 -j 8`. An ID outside the
   campaign exits (self-test `mutate_ids_test.py`, 2026-10-26).
   `--scratch DIR` (or env `WWHD_MUT_SCRATCH=DIR`; the option wins) builds the per-mutant scratch files
   (`mutantN.cpp`, `mutantN.o`, the `mutantN` executable) in a per-run subdirectory of DIR, e.g. a RAM disk, instead of
   `build/verify/<unit>`; results, receipts and the baseline build stay where they are. DIR missing or not writable:
   one stderr note and the build directory as before; less free space than 3x the baseline executable (at least
   200 MB; `WWHD_MUT_SCRATCH_MIN_FREE=BYTES` overrides) or out of space mid-mutant: that mutant goes to the build
   directory (self-test `mutate_scratch_test.py`, 2026-10-26).
   Helpers above the first marked function (2026-10-26, AGL#1): in a GameCube-style (glue) source, file-local
   functions defined before the first `/* ADDR */` function (e.g. EnvironmentManagerBlend.cpp's blend* helpers) were
   never mutated. They are now (`mutate.preamble_helpers`: top-level definitions only, namespace and `extern "C"`
   blocks transparent; types, globals and declarations stay out), each attributed to the first marked function that
   uses it directly or through other helpers (in scope for `--func`/`--only` when any user is; a helper no marked
   function uses is left out). Survivor lines say `(helper NAME)`. Existing IDs are unchanged: the helper mutants are
   shuffled separately and follow the old campaign's IDs, sampled at the old pool's rate (`--max` / pool size), so
   `--ids` replays of older campaigns still work. `--no-preamble-helpers` gives the old enumeration (self-test
   `mutate_helpers_test.py`). lib2 sources: 241 such helpers, 3331 mutants.
   Host symbols (2026-10-26, AGL#13): a mutant that drops or moves a declaration such as `u32 index = ...;` makes the
   later use bind to the host libc `index()` (strings.h); it compiles on the host only and used to be classed
   killed/signal. A mutant object that references a C symbol the unmutated source (compiled the same way, -O1) does
   not, and whose name is spelled as an identifier in the mutated source, is now `broken` (stage `hostsym`, the
   symbols in the receipt). Compiler-made libc calls (a loop turned into bzero) are not spelled in the source and do
   not count. When the use is folded away (a swap of `s16 index = ...;` with `if (index == 0) return ...;`: clang
   folds the compare with libc `index` to false, so the object references nothing new), the warning still names it:
   for statement drops and swaps a `-fsyntax-only` compile (without `-w`) collects the functions named in "function
   'NAME'" warnings, and a name the unmutated source does not produce and the mutated source spells is a host symbol
   too. `--allow-host-symbols` restores the old behaviour. The checks are `mutate.new_host_symbols(base_obj,
   mutant_obj, source_text)` and `mutate.host_function_values(base_cpp, mutant_cpp, cflags, source_text)` (self-test
   `mutate_hostsym_test.py`).
   `--reach-skip` (2026-10-26, opt-in) runs each mutant only where it can make a difference. A marker build of each
   source (`mutschema.py`: every schema-able site wrapped as in `src/mutschema.h`, none live, same -O1 and FP flags)
   runs once unmutated with `WWHD_MUT_REC=1 -mutrec FILE` and records which sites each input runs, per function and
   input index (setup-phase hits count for all of that function's inputs; start-up/static-initialiser hits disable the
   skip for that mutant). A mutant then runs with harness `-runlist` (only the functions and inputs that ran its
   site) and `-stopfirst` (stop at the first row whose first difference is an output difference; a row whose first
   difference is an abort stops that row only, so the abort/difference split is kept). A mutant no input reaches is
   reported as a survivor `(not reached)` without being built. Statements the schema cannot rewrite (declarations,
   assignments to a local declared without an initializer) get a marker-only site, `(void)WWHD_MUT(K);` before the
   line: it records when the statement runs and never changes the program; such a mutant is still built on its own.
   Mutants without any marker (bodies of braceless ifs, sites the schema compile rejects, `--rerun`) run every input. Assumes, like `-shards`, that inputs are independent of each
   other; the marker build must pass unmutated or its mutants run every input. Self-tests `mutschema_test.py`,
   `mutate_reach_test.py`.
   `--schemata` (2026-10-26, opt-in) runs the mutants on those same schema builds instead of compiling and linking each
   one: one -O1 object per source and layer holds every schema-able mutant (`src/mutschema.h`; sites that overlap
   without nesting, such as adjacent swaps, go to separate layers), and the harness makes mutant K live from
   `WWHD_MUT_ACTIVE=K` (set before static initialisers run). A mutant is put in a schema only when the compiled text of
   the mutant equals the original's with that one site replaced, its site has a rewrite that keeps the surrounding
   parse, and the schema compiles with it (`-Werror=vla`: a literal in an array bound must not turn it into a VLA);
   every schema executable must pass unmutated (2026-10-28: these unmutated checks, one per schema executable with its
   reach recording, run -j at a time, each under a `--pool` slot; drops, messages and reach records follow the sorted
   executable order exactly as the serial loop did - self-test `mutate_schemaj_test.py`, serial vs parallel identical,
   per-schema fallback and all-fail stop included; since 2026-10-29 the test reads the executables as groups of
   mutant IDs from `WWHD_MUT_SCHEMA_DUMP=FILE`, a self-test hook that writes them after the schema builds, instead of a
   fixed count: how many executables a campaign gets and which mutants share one depend on the unit's sources, and a
   RuntimeIntegerFormat.cpp change had turned 7 into 6; since 2026-10-30 each check runs only the rows of its members'
   per-mutant scopes - the union of `mutscope` scopes, so helper callers and host-static users stay in - and the whole
   campaign scope as soon as one member's scope cannot be bounded or with `--whole-unit`; a member's site runs only in
   its scope's rows, so the reach records and classes are the same, and the check writes no cache;
   `WWHD_MUT_SCHEMA_CHECK_WHOLE=1` runs every row as before; the log line `schema check scope:` gives the planned row
   runs, `schema check took` the time and the rows run; self-test `mutate_schemascope_test.py`, every row vs scoped:
   identical receipts and reach records, 21 instead of 312 row runs on lib2_sead_15). All other mutants are built one by one as before; receipts of schema
   runs name the executable (`schema`). An `&&`/`||` flip is rewritten over the whole logical-and-expression around
   it (to the closest `||` on each side), so the schema groups the mutant as its textual form does: `&&` binds
   tighter than `||`, and before 2026-10-28 the first `&&` of `a && b && c` ran as `(a || b) && c` instead of
   `a || (b && c)` (likewise the second `||` of `a || b || c`; SEAD#16 #1039). The other operator flips stay within
   one precedence level (`==`/`!=`, `<`/`<=`/`>`/`>=`, `+`/`-`) and are rewritten in place. Self-tests
   `mutschema_test.py` (each site kind: the schema prints exactly what the mutant built on its own prints),
   `mutschema_prec_test.py` (&&/|| chains, mixed chains, comparison and +/- chains: schema = textual mutant on inputs
   that tell the groupings apart), `mutate_schemata_test.py`.
   `--fork` (with `--schemata`; 2026-10-26, opt-in) starts each schema executable once per worker thread as a fork
   server (harness `-forkserver`: image, specs and static initialisers load once; each stdin line `RUN K OUT RUNLIST`
   forks a child that makes mutant K live, takes RUNLIST as its `-runlist` and writes `OUT.out`/`OUT.err`; replies
   `PID p` and `DONE rc`, rc minus the signal for a crash). A run over the time limit has its child killed and gets the
   same retry as a normal run. Mutants whose site ran in a static initialiser (the server's `READY k,...` line), and
   every mutant of a server that fails, run as a fresh process. Receipts of forked runs carry `fork`. Self-test
   `mutate_fork_test.py`. Open servers are bounded (2026-10-27, NW-SND#6: a long campaign kept one server per thread and
   executable and passed fd 1024, `filedescriptor out of range in select()`): mutants are submitted grouped by schema
   executable (results and receipts stay in mutant order), a thread keeps at most `WWHD_MUT_FORK_SERVERS_PER_THREAD`
   (default 4) servers, an executable's servers close after its last mutant, a closed server releases both pipes, and
   the reply pipe is read with poll(), so high fds cannot crash it. Self-test `mutate_forkfd_test.py`.
   Campaigns may use `nulltrap` (unit directive, `# verify: -nulltrap`, or `mutate.py --nulltrap`): a mutant that
   dereferences NULL where the original checks first then takes a page-0 DSI, as on the console, and counts as a
   `difference` (first difference `trap: ...`) instead of an abort, a budget timeout or a survivor (2026-10-26, AGL#13).
   Refined zero-page rule (2026-10-26): inputs where the unmodified ORIGINAL touches page 0 count only up to the fault; steer
   a fixture only where -nulltrap loses something. `nulltrap_compare.py SPEC [-n 10000] [-seed 1 -seed 7] [verify options]`
   runs verify.py with and without -nulltrap and reports per function: rows, coverage, clean inputs (n minus original
   page-0 DSIs), blocks LOST under -nulltrap and LOW clean (< 10% of n); `nulltrap_compare.py --receipts DIR_WITHOUT DIR_WITH`
   lists mutants killed without --nulltrap but not with it. Verdict line `nulltrap_compare: ...: OK`, `FAIL: ...` or
   `STEER: ...`. OK is a gate pass: every row on both sides passed and nothing is lost. FAIL (exit 1, 2026-10-28; before,
   a row such as `6301/10000` still gave OK): a function row on either side has passes < total (generated or recorded;
   inputs aborted on both sides are not failures), one `FAIL seed S ADDR SIDE rows P/T: <first difference>` line per
   failing row; FAIL wins over STEER and INCOMPLETE, which the verdict line then lists after it
   (`FAIL: A; STEER: B; INCOMPLETE: C`). STEER (exit 1): every row passes but -nulltrap loses something.
   Since 2026-10-27 it runs -nulltrap first and the side without it only where that can differ: -nulltrap changes a run
   only at a page-0 access (no other memory, setup, mock, input or budget difference), so an input on which neither side
   took a page-0 DSI gives the same result either way. Harness option `-dsiout FILE` (with -nulltrap) appends per function
   the inputs a DSI ended on either side as a `-runlist` line (`ADDR i,j,...`; `ADDR all` when a pre-run of the input
   dictionary (.rodata constants the original reads) took a DSI and the same pre-runs without -nulltrap build a different
   dictionary, so every input may differ) and `# ADDR pass ...` / `# ADDR prerun N [same]` / `# ADDR done` comments. The plain
   side runs that runlist only, and its row is rebuilt from both runs (counts: the -nulltrap counts minus those inputs'
   -nulltrap passes plus their plain passes; uncovered blocks: -nulltrap's that the plain inputs did not cover either).
   Rows of functions without a page-0 input end `[plain = nulltrap (all clean)]`, others `[plain on N page-0 input(s)]`
   or `[plain run whole (a pre-run DSI changes the dictionary)]`; rows, coverage, clean counts and verdicts are those of the full
   run. `--full` runs both sides whole as before. `-shards K` shards both sides (before 2026-10-27 verify.py ignored it
   because of -v -v); the -dsiout file is written per shard and joined in shard order.
   It stops with exit 2 and no verdict when verify.py exits other than 0/1, and reports a row whose generated total is not
   n without inputs aborted on both sides as `INCOMPLETE ...` (verdict `INCOMPLETE (rerun)`, exit 2).
   Self-test `nulltrap_compare_test.py`.
   **Coverage from the gate logs, no separate coverage pass.** `--logdir DIR` keeps the gate's raw
   runs: per seed S `DIR/SPEC.sS.nulltrap.out/.err` (the -nulltrap side's verify.py -v -v output: rows, `uncovered
   blocks:` per function, per-input FAIL/unusable lines; stdout byte-identical to a separate `verify.py SPEC -n N -seed S
   -nulltrap -v -v`), `DIR/SPEC.sS.plain.out/.err` (the run without -nulltrap: page-0 inputs only, or whole), the
   `-dsiout` file, and `DIR/SPEC.sS.coverage.txt`: one line per function and side, `ADDR -nulltrap rows P/T coverage
   H/B clean C/N uncovered: b1 b2 ...` and `ADDR plain rows ... uncovered: ... [note]` (the plain row is the merged one,
   equal to a separate plain -v -v run). Archive gates run with `--archive --logdir` and cite coverage.txt (union over
   the unit's specs, every uncovered block with its reason) instead of running `verify.py -v -v` again.
   **Survivor replays reuse the gate's original side.** `--refcache-out DIR` makes the -nulltrap run also write the
   original's reference-side records (harness `-refcache-write`, also under -dsiout; a -range shard writes
   `ADDR.ref.LO-HI` and verify.py joins the parts into `ADDR.ref`, `refcache.py`; joined records over `WWHD_REFCACHE_MAX_MB` are dropped as an unsharded writer drops them) to `DIR/SPEC/sS/` with a manifest
   `refcache.json` holding mutate.py's campaign key for `mutate.py SPEC --nulltrap --n N --input-seed S` (objects,
   expanded spec, harness options in mutate.py's order, recordings, plus a hash of tools/verify/*.py). Only with
   options that leave the original's run alone (-shards, -buildj, --archive, --no-cache, -observe-pointees,
   -noprogress, -progress-interval); an earlier `sS` is renamed `sS.oldK`. `mutate.py SPEC --rerun ... --nulltrap --n N
   --input-seed S --refcache-from DIR` then uses those records when the manifest's key, n and seed equal its own
   exactly: the baseline runs only the candidate against the records (it still has to pass; its wall time sets the
   per-mutant timeout, and re-measurements read the records too), every mutant run reads them, receipts carry
   `refcache_from`. Any mismatch prints `--refcache-from ... not used: <reason>` and the campaign records its own
   baseline as before; per function the harness still checks the RFC3 header (key, n, seed, block count, switches,
   dictionary) and runs a mismatching function live. With `--baseline-cache` too: a cache hit wins (no baseline run at
   all; `--refcache-from ... not used` names it); on a miss a matching `--refcache-from` wins over a recording baseline
   and nothing is stored in the baseline cache (an entry carries its own records and a full-baseline wall time); when
   `--refcache-from` does not match, the campaign records and stores as without it. The refcache key (`refcache.key`,
   also part of the `--baseline-cache` key) includes a hash of tools/verify/*.py, so entries made by other tool code miss. `WWHD_REFCACHE_CHECK=1` validates the records against live runs.
   The gate's records are byte-identical to an unsharded write in mutate.py's configuration. Self-test
   `refcache_reuse_test.py`.
   `--pool` (2026-10-26, opt-in) shares the machine between campaigns: every mutant's build and run (and every schema
   compile) holds one of `WWHD_MUT_POOL_SLOTS` (default the core count) machine-wide slots, flocks on
   `WWHD_MUT_POOL_DIR/slot.N` (default `/tmp/wwhd-mutpool`), so -j becomes an upper bound and independent mutate.py
   processes (any worktree or clone on this code) stop oversubscribing each other. While the 1-minute load is above
   1.25x the cores (work outside the pool), a process that already holds a slot waits for another; every campaign
   keeps at least one job running. Self-test `mutate_pool_test.py`.
8. Commit the source (`wwhd_src/`) and the unit file.

**Parallelising.** Functions are verified one at a time and callees are always mocked, so
functions of one actor can be written by different people or agents at once.
- Give each worker its own source file and unit name (separate build directories).
- Workers declare local bindings for the functions they call, then merge. Calls between
  files go by address anyway.
- Shared headers and the harness need one owner.
- In the pilot, kamome's two largest functions were written by two agents in parallel while
  the harness was being extended.

## Scaling to many actors

### Tools

| Tool | Use |
|---|---|
| `progress.py [PID \| RUN_DIR \| UNIT] [--json]` | the current state of running verify / mutate runs from their progress files: function k/N, inputs done/total, inputs/s, ETA, the current seed and how long it has run, STUCK/HANG; see "Progress, stuck inputs and hangs" (2026-10-27) |
| `triage.py UNIT --receipts DIR [--class survived] [--ledger TSV] [--root CLONE] [--tree SRC] [-j N]` | mechanical first-pass triage of mutation survivors. Every check runs on the COMPILED candidate (base source and receipt mutant built like mutate.py's per-mutant build, objdump), never on source lines: **swap** (base and mutant make the same effects - gmem loads/stores, Cpu-struct accesses, guest/host calls, conditional exits - and every inverted pair commutes: disjoint offsets on one base, disjoint frame objects, a non-escaping frame object, or a frame object a call gets only in registers its livein.tsv mask does not read), **frame** (a FrameLocal offset differs: span vs frames.tsv save slots / caller frame, then per-byte liveness on the mutant's CFG: foreign read of a shifted write or a shifted read of foreign/stale data = RI; a conflict only through a callee that gets the pointer, or a frame that a `-footprint` spec or a `callee rN=SIZE` pointee compare can observe = MANUAL with that spec named), **unreadarg** (only argument-register stores differ, or stores into a frame object that reaches only unread argument registers), **cr6** (cr[k] stores before a non-variadic callee, or a variadic one whose constant format has no float conversion). Output: TSV (id, function, original VM_COV block, kind, reach, group, check, candidate EQ?/RI?/MANUAL/DEP, reason, evidence file) and a per-check / per-group summary. Candidates only: the helper and the decide. Assumes the automatic native frame is active (FrameLocal in_frame) and callees do not keep stack pointers past their return. `--tree` = the sources the receipts were made from (`git archive` of the campaign commit); a receipt from another source version is reported `stale`. Scratch stays small: base objects plus one reused object slot per worker and an evidence excerpt per survivor (`--keep-objects` keeps every mutant's object). Proof: AGL#2's ledger. Self-test `triage_test.py` |
| `decl_lint.py [-j N] [--out TSV] [--handoff TSV]` | exact declarations of guest calls (2026-11-03). A guest call puts only its declared arguments into r3../f1.. and leaves the rest as it was, so a call that declares fewer arguments than the callee reads passes the harness when the register happens to hold the right value (the caller's own entry value passed on, an earlier call's result). The lint compiles every unit source with `-DGABI_DECL_AUDIT` (gabi.h marks the guest calls deprecated, so clang reports each call site with its argument types; `-fsyntax-only`, about 3 minutes at -j 4, cached in `$WWHD_VERIFY_SCRATCH/decl_lint-<tree>`) and compares each call with the callee's definition as compiled and with the original's live-in registers (`build/verify/livein.tsv`); a missing register is classified by the original caller's dataflow: `a` (the caller's own parameter: pass it), `b` (the caller's undeclared entry value: make it a parameter), `local` (an earlier call's result or argument: pass it explicitly), `mixed`, `nosite`, `hdr`, `callee-b` are errors (exit 1, listed); `e?i` (2026-11-04: the site passes an argument the callee's definition does not take and the callee's original passes it on unchanged to its own indirect call or an import that reads it: take it and forward it; `indirect` lines in `decl_exceptions.tsv` name held registers no target of that call reads) is an error too; `c` (variadic callee), `d`/`d?` (the callee declares a parameter its original never reads / only an indirect call might read), `e`/`e?` (an argument nothing reads / only an indirect call further down might read) are counted and tolerated (the native path passes everything to d? callees and drops extra trailing `e` arguments; a callee with unresolved `e?` sites is `guest`, the round trip keeps the register). Definitions whose original reads an argument register they do not take are errors unless `decl_exceptions.tsv` names them (path-insensitive live-in over-approximations). Ext stubs are audited wherever they are called: the source, the sources linked with it (same verify unit), header inline wrappers, a stub passed as a host function pointer; stubs that make no call (`callback` lines, uses only as a guest address or in prototypes) are skipped. A decompiled function whose original still holds an entry argument register at one of its own indirect calls (bctr/bctrl, an import) that a possible target reads and that its decompiled indirect calls do not all pass (`ind`, 2026-11-04: buildLayoutScreen's virtual initialize reads r4/r5) is `guest` in the hand-off (an unresolved target or an ambiguous pairing of decompiled and original calls is unclear, i.e. guest); targets come from the vtables constructors store at the call's vtable-pointer offset (narrowed to the vtables that hold a method of the function's class, plus the vtables of its subclasses found through the constructor chains, when the call's object is its own `this` on every path; pointer-to-member calls through a constant table resolve to the table's functions), `indirect` exception lines clear hand-checked registers. Results (2026-11-04, dScnLogo_Delete): a call site that declares a result register (r3, r3/r4, f1) the callee's definition does not produce is the error `ret` when the caller's original reads that register after the call (also one the callee leaves unchanged: the decompiled caller must take the value from where it really comes from), `ret?` when the caller only returns it on to its own callers (unclear: the callee is `guest`) and tolerated `ret-unread` when no caller reads it or the decompiled callers discard it; which registers a function's callers read at its exit is a fixpoint over all of build/gen. `--handoff` writes the native-calls table: per decompiled function `exact` / `shim` / `guest` with the reasons. Self-test `decl_lint_test.py` |
| `verify_all.py [-n 10000] [-rec DIR] [UNIT...]` | re-verifies every unit (parallel); non-zero exit if anything fails. With no UNIT it leaves out `units/shared/` (include fragments) and top-level `units/_NAME/` (self-test fixtures; `units/_diag/`: diagnosis only, no credit, and mutate.py refuses those). Logs in `build/verify/<unit>/verify_all.log` |
| `tu.py UNIT` / `tu.py --summary UNIT...` | the WWHD functions of a translation unit: address, size, verified (`V`), GameCube source state (`src` decompiled, `stub` "Nonmatching" placeholder: write it from the WWHD code), evidence, names. Unnamed neighbours are marked `?`; names far outside the unit's range are flagged as probable matcher errors |
| `mkbind.py ADDR...` / `mkbind.py --callees ADDR...` | binding drafts from the GameCube signature, checked against the registers the WWHD code reads (`!!` marks disagreements: HD signature change or wrong name). `--callees` lists the unbound callees of functions |
| `sizes.py [REGEX]` | WWHD class sizes from constructors (GHS constructors allocate `sizeof` when `this == NULL`) and the vtable pointers they store |
| `fret ADDR LO HI` (unit file) | range of a callee's float result in generated mocks, edges drawn often. `ret` accepts hex: `ret 025200D4 0x54000000 0x54000000` pins the play object so its fields can be steered by absolute address. Float `field` ranges also draw their edges often (boundary comparisons) |
| `arg FUNC rN\|fN LO HI` / `v1,v2,...` (unit file) | range or pick list of an argument register of the function under test (edges drawn often; 1 input in 8 stays unsteered). `arg!` always applies (e.g. loop counts) |
| `argf64[!] FUNC\|* fN v1[,v2,...]` (unit file) | exact double inputs of an FPR: ps0 = ps1 = v, never narrowed to single (`arg` and the generated FPR inputs are single-precision values). Each v is a decimal double (`1.000000000931322574615478515625`) or `0x` + 16 hex digits of raw IEEE-754 bits (`0x7FF0000000000001`, a signalling NaN). Drawn like an `arg` pick list (1 input in 8 unsteered without `!`). Malformed lines exit 2. 2026-10-23; self-test `argf64_test.py` |
| `fretf64[!] ADDR v1[,v2,...]` (unit file) | exact double f1 results of a mocked callee: ps0 = ps1 = v, never narrowed to single (`fret` and the generated f1 results are single-precision values). Values as for `argf64` (decimal, or `0x` + 16 hex digits of raw bits). Without `!` 1 call in 8 keeps the generated result; `!` also applies inside `real` callees' nested calls. Malformed lines exit 2. 2026-10-24; self-test `fretf64_test.py` |
| `callee ADDR rN=inoutSIZE` (unit file or callee_facts.tsv) | rN points to SIZE bytes the callee reads and updates: compared at the call like `rN=SIZE`, and generated mocks fill them afterwards (like `outSIZE`). A global `out`/`inout` fill also applies when a unit line for the same key declares only a size (tev-output facts 2026-10-05: the light state dKy_tevstr_c stayed zero under mocks, hiding wrong-offset reads) |
| `ret ADDR v1,v2,...` (unit file) | pick list for a callee's r3 result |
| spec value tokens (unit file, 2026-10-27) | every operand of `arg`/`arg!`, `field`, `ret`/`ret!`, `mockfield`/`mockglobal` and `fill` (values, pick-list entries, FUNC and ADDR, `rN+OFF` offsets, registers) must parse completely as a number (integers: decimal, `0x` hex, `0` octal; float types: any C float) or the harness exits 2 with `FILE:LINE: invalid KW value "TOKEN" (not a complete number): LINE`. Before, such tokens were read as far as they were numeric: `*0x5E100008` and `ptr` steered 0, `r3+6C` used offset 6, `ret ADDR r3 V` drew 0..V, `r4-4` meant r4+0, fractions in a `ret` list became integers, and a `field` line with a non-numeric value or no FUNC was skipped. A negative offset is written `rN+0xFFFFFFFC`. Self-test `spec_tokens_test.py` |
| `field FUNC WHERE TYPE v1,v2,...` (unit file) | pick list: the field takes one of these values (single flag bits, enum values with gaps) |
| `callee ADDR rN=outSIZE` (unit file) | rN points to output-only storage of SIZE bytes: its contents before the call are not compared, and generated mocks fill it (e.g. `MtxPosition` `r4=out12`, `cXyz::operator-` `r4=out12`). `rN=255` (constructor storage) is neither compared nor filled |
| `callee ADDR rN=heapSIZE` (unit file) | rN points to SIZE bytes on the heap or in static data that the callee reads: besides the raw pointer value, the SIZE pointee bytes are compared at the call (a plain `rN=SIZE` compares the pointee only for stack pointers). For a context object passed by address to a mocked callee, so a dropped or wrong store into it is detected (JS-JParticle#2 `&dc` at the visitor exec calls, 2026-10-22; self-test `heapref_test.py`) |
| `observeheap ADDR` (unit file; also harness `-observe-heap ADDR`) | a sead ExpHeap object whose blocks bound heap pointer arguments for `-observe-pointees` (2026-10-28); inert without the switch. Not needed for the shared agl world: its root heap 0x6A000000 is the default when no heap is named |
| `callee ADDR fixture rN=...` (unit file) | ADDR is a mock-only target with no generated code, e.g. a fixture object's callback slot: the unit gets a callee entry for it (`fixture_ADDR`), exactly the registers its facts name are compared, `rN=SIZE` pointees are compared as usual, all volatiles count as clobbered, and it is never executed (always the generated mock). Without `fixture`, facts for a target outside the game code are dropped (SEAD#14 allocation-failure observer, 2026-10-23; self-test `fixture_dyn_test.py`) |
| `callee ADDR rN=dynK*rM[:CAP]` (unit file) | the pointee of rN is K*rM bytes at each call (at most CAP, default 4096; overflow-checked), compared like `rN=SIZE` (stack and, with `heap`, heap pointers); a count of 0 compares nothing beyond the count register. For SDK calls whose extent is an argument, e.g. `callee C0006A40 r5=dyn4*r4` (GX2SetVertexUniformReg: 4*count bytes; NW-LYT#3, 2026-10-23) |
| `callee ADDR rN=cstr[:MAX]` (unit file) | rN is a NUL-terminated string read by the callee: the bytes through its first NUL are compared (scan at most MAX, default 4096; an unterminated string is captured as MAX+1 bytes, so it only matches an identical unterminated one), on the stack, heap or in rodata. Use it instead of a fixed size that would overread shorter strings (NW-SND#4 file backend path, 2026-10-23) |
| `gabi::trap(ADDR)` (candidate source) | the original takes a trap instruction (`tw`/`twi`, e.g. the debugger break in nw::ut `halt` at 0286DB70): the harness counts the original's trap and the candidate's `gabi::trap` and compares the counts (`trap count: original N, candidate M`). In the game build `gabi::trap` calls the runtime's `ppc_trap` (weak default; the port owns the native debugger break). Only 0273AAFC, 0286DB54 and 028EA120 contain trap instructions (2026-10-22) |
| `# verify: -only ADDR,... [-calleesp]` (spec header comment) | the arguments a spec needs to run correctly: a companion spec that steers a few functions and links the rest as support declares its test scope here. `verify.py` adds them when the command line gives no `-only` (and prints `spec header adds ...`; `--no-header-args` runs without), `mutate.py` takes the `-only` as its `--only` scope and passes the other flags to every mutant run. Use it instead of prose such as "run with -only ..." (2026-10-22, SEAD#6; self-test `header_args_test.py`) A `# readable-scope: ADDR,...` comment (the scope `readable_check` uses) counts as the spec's `-only` when no `# verify:` line gives one (2026-10-24: unscoped runs of such specs could loop on support code, e.g. JST_adaptors in verify_all). |
| `fill FUNC\|* ADDR\|rN+OFF SIZE BYTE` (unit file) | SIZE bytes (1..0x1000000) from ADDR all hold BYTE in every generated input of FUNC (`*`: all functions), both sides. Applied before the `field` steering, so `field` lines over the same bytes win (a background plus a few targeted words). Use it instead of thousands of per-word `field` lines, e.g. a 512 KB all-0xFF table that drives a scan to its cap: `fill 027201E4 5B000000 0x80000 0xFF` (hd_inf_save_holders/cap_boundary). Add `stable` for the region if callees must not clobber it. Self-test: `fill_test.py` |
| `cr FUNC\|* BIT V[,V...]` (unit file) | entry CR bit BIT (0..31) of FUNC's generated inputs (`*`: all functions) is one of the values (0/1), picked per input; otherwise every CR bit starts clear. Use: a GHS variadic function saves f1-f8 only when the caller set CR bit 6 (cr1.eq, PPC EABI floats-in-FPRs); the candidate reads the bit as `gabi::cpu->cr[6]` at entry (m_Do_printf OSReport_Error). `cr 027EC938 6 0,1`. Malformed lines exit 2. 2026-10-21; self-test `cr_test.py` |
| `copy FUNC\|* DST\|rN+OFF SRC SIZE` (unit file) | SIZE bytes (1..0x1000000) of the loaded game image at SRC are placed at DST in every generated input of FUNC (`*`: all functions), both sides; SRC..SRC+SIZE must lie in one image section (checked after the image loads, else exit 2). Applied after `fill`, before `field` (field lines over the same bytes win). Use it for real tables/objects the code reads through a pointer you steer. 2026-10-21; self-test `copy_blob_test.py` |
| `blob FUNC\|* DST\|rN+OFF FILE OFFSET SIZE` (unit file) | the same with SIZE bytes read at run time from `game/FILE` (a path inside the `game/` link to the user's game files: no absolute path, no `..`) at OFFSET; a missing or short file exits 2. The data is never committed: `blob_guard_test.py` fails when a tracked file contains blob bytes or anything under `game/` is tracked (run it before merging a unit with blob lines). 2026-10-21; self-test `copy_blob_test.py` |
| `blobfile FUNC\|* DST\|rN+OFF FILE[:STEP...] [OFFSET SIZE]` (unit file) | like `blob`, for a file inside the game's containers: FILE is a path inside `game/`; each `:STEP` descends into the current bytes: `<yaz0>` Yaz0-decodes them, any other STEP names an entry of a SARC (by name; unnamed SFAT entries match by the SARC name hash) or an embedded file of a Wii U BFRES (index group 11), with Yaz0 data decoded first. Without OFFSET SIZE the whole result is placed (1..0x1000000 bytes). E.g. the FilterAa parameter archive `blobfile * 5A300000 content/Common/Misc/Misc.szs:Misc.bfres:wwHD_basic.baglfila`, then the original consumer runs as a `setup` (027300B4 → 0279A308). The Yaz0 decoder is byte-identical to the game's own 0275ED14 (self-test: the original, run on the stored stage archive, writes exactly the placed bytes). A bad path, missing entry, non-container step or short window exits 2. Never committed: `blob_guard_test.py` also samples blobfile lines (reference reader `blobfile.py`). 2026-10-27 (AGL#3 FilterAa); self-test `blobfile_test.py` |
| `stopat FUNC TARGET N` (unit file) | for a function that never returns (a thread loop): each run of FUNC ends at its N-th top-level call to the mocked TARGET (e.g. the blocking message receive at the top of the loop, N = 2 for one full iteration). That call is recorded and compared like every call; its return is not simulated. Both sides must reach it (a side that returns instead differs); then the call sequence and the net memory effect up to there are compared, the return value and exit registers are not. Such runs count as completed comparisons, not as aborts. Malformed lines exit 2. 2026-10-21; self-test `stopat_test.py` |
| `stopat FUNC TARGET N live REG...` (unit file) | as `stopat`, and the named registers (`r0`..`r31`, `f0`..`f31`; an FPR compares ps0 and ps1) are compared at the terminal call on both sides: for a value the original keeps live past the stop, which the call record (f1-f8 arguments) and the net effect do not show (AGL#3 0277AD60: f13/f14 read by the suffix at source 1558). A difference reads `stopat: live f13 at the terminal call: original 0/0 candidate 1/1`. Without `live` nothing changes. 2026-10-30; self-test `stoplive_test.py` |
| `stackfill` (unit file) | the scratch stack starts with deterministic random bytes instead of zeros (both sides; `gabi::Local` slots start with the same bytes): exposes stores the original makes to stack objects that a zero default would hide |
| `stackfill periodic` (unit file) | like `stackfill`, but the byte depends only on the address modulo 4: stack objects the two sides place at different, equally aligned addresses start with the same bytes (padding the code never writes) |
| `footprint FUNC SIZE` (unit file) + `gabi::NativeFrame<SIZE>` | compare FUNC's own stack footprint too: the SIZE bytes below the entry SP and the back chain/LR save above it (normally excluded). Use with `stackfill` (non-periodic), so bytes neither side writes compare equal. Needed where stale stack bytes escape: a function that copies uninitialised parts of its stack objects out (d_save initdata_to_card: padding of the init objects goes into the empty save slots), and the functions that run before it at the same depth (d_file_select fsCreate), whose frames leave those bytes. Such a candidate declares `gabi::NativeFrame<SIZE> f` (SIZE = the original's `stwu r1,-SIZE`): r1 is lowered so callees run at the original's sp, the area is NOT initialised (a `gabi::Local` is: zero/stackfill in the harness, zero in the game), objects are addressed `f.sp() + offset` as in the disassembly, and the frame's own stores are written like the original's (back chain at sp, saved registers from `cpu->r[n]`, LR at `f.entry() + 4`, paired-single temporaries). With `footprint`, generated inputs also give r14..r31 distinct values (so saved-register stores are checked), and every callee must start with the original's r1 (`footprint FUNC 0` for a function without a frame: a tail-calling dispatcher must use `gabi::tail_ptr`, not `call_ptr`, which runs the target 16 bytes lower). Game test 2026-10-05: cking.sav differed in 69 bytes of the empty slots. |
| `framexact FUNC RET` (unit file) + `gabi::call_site` / `gabi::call_ptr_site` | cross-build save states (2026-10-06): FUNC's call that returns to RET (the original's LR there) is on the guest stack when a save state is taken at the frame boundary (main loop: hd_main 020061C4, Framework_initialize 02034FD8, Framework_procFrame 02035090). At that call the harness compares LR, r14..r31, f14..f31 (ps0 and ps1) and FUNC's whole automatic frame incl. back chain, saved-register slots and its LR save word; at FUNC's exit r14..r31 and f14..f31. Generated inputs give r14..r31 and f14..f31 distinct values. The candidate writes that call as `gabi::call_site(RET, {{reg, value}...}, {{freg, ps0, ps1}...}, ADDR, args...)` (`call_ptr_site` for an indirect call): LR = RET and the callee-saved registers the original holds at that call; the enclosing automatic frame then reloads its saved registers on return like the original's epilogue (`AutoFrame` reload, only in functions that use `call_site`). A value needed after the call is taken back from its register (procFrame: `self = gabi::cpu->r[30]`), so a state load gives the snapshot's value. |
| `final FUNC ADDR SIZE` (unit file) | postcondition: SIZE bytes at ADDR are compared at the exit of the tested function FUNC, whoever wrote them (e.g. a field saved, clobbered by a mocked call, and restored) |
| `gabi::load_reserved(a)`, `gabi::store_conditional(a, v)`, `gabi::sync()` (candidate code) | lwarx / stwcx. / sync for spin locks and atomic updates in guest memory: the harness applies the reference's reservation rule on observed memory (stwcx. stores only while the reservation holds and returns cr0.eq), the game build uses the runtime's native atomics (ppc_lwarx/ppc_stwcx) and a full fence. Write the original's loops with them; dcbst/dcbf need no call (SEAD#20 request, 2026-10-23) |
| `gabi::fadds_fpr / fsubs_fpr / fmuls_fpr / fdivs_fpr(f64, f64)` (candidate code) | single-precision arithmetic on FPR operands that are not singles (e.g. an incoming f1 the original keeps as a double): the reference's `to_single(ppc_nan2(a, b, a OP b))`, fmuls rounding frC to 25 bits (round25). Declare such parameters f64 (ArgFrom<f64> keeps the whole FPR). Separate names, so existing f32 calls keep their meaning; matches the reference bit for bit on 2,000,000 random operand pairs (AGL#8 request, 2026-10-23) |
| `callcr TARGET BIT[,BIT...]` (unit file) | the listed condition-register bits (0..31; bit 6 is cr1.eq, which a caller sets or clears before a variadic call to say whether floats were passed in registers) are recorded at every call to TARGET, top-level calls only (calls made inside `real` callees are not recorded; see `observenested`), and compared like an argument: `call #K X: entry CR bit B original V candidate W (callcr)`. Opt-in per target, e.g. `callcr C00070C8 6` (NW-SND#11 request, 2026-10-23) |
| `alloc TARGET rSIZE BASE [ALIGN \| align=rN]` (unit file) | the allocator mock returns distinct aligned blocks from BASE in call order (size from rSIZE, default alignment 32), identical on both sides. With `align=rN` the alignment is the call's own argument rN, per call (e.g. `alloc C00096A0 r4 0x5A000000 align=r5` for MEMAllocFromFrmHeapEx): a block starts at the cursor rounded up to that alignment and the cursor advances by the size rounded up to it, so mixed 4/32 requests pack like a frame heap's head; a negative alignment (tail allocation) or one that is not a power of two aborts the run, as does a block past the address space. The size and alignment arguments are still compared as call arguments (NW-LYT#4 request, 2026-10-23; self-test `alloc_align_test.py`) |
| `allocfixed TARGET SIZE BASE [ALIGN]` (unit file) | like `alloc` for a factory whose request size is fixed (e.g. `allocfixed 025E38E0 0x144 0x29000000 32` for the J3DModel factory) |
| `allocnested TARGET` (unit file, after TARGET's `alloc`/`allocfixed` line) | the allocator's calls made inside `real` callees take the same deterministic blocks from the same cursor (instead of a generated pointer or a fixed `nestedret`), so constructors run as `real` get distinct, correctly sized and aligned storage. `-trace SEED` prints each nested allocation. Off unless declared. 2026-10-24; self-test `allocnested_test.py` |
| `retarg TARGET rN` (unit file) | TARGET's generated mock returns its own incoming rN (r3..r10) in r3, after its other mock effects, at top level and inside `real` callees (e.g. a read callback that reports the full requested size). Arguments and pointees are still compared as usual; a `mockmove` destination return wins for move targets. `-trace SEED` prints each such return. Off unless declared; malformed lines exit 2. 2026-10-24; self-test `retarg_test.py` |
| `setup FUNC\|* ADDR [rN=VALUE ...]` (unit file) | before the tested function FUNC (`*`: all), the `real` function ADDR runs once on both sides from a copy of the entry state with the given registers (VALUE hex or decimal), on its own stack 0x8000 below the entry SP and at nested depth (its calls are answered silently; `allocnested`/`retarg`/`nestedret` apply). Its memory effects are the starting state, e.g. an original constructor that creates a singleton the function only consumes. Lines run in file order. ADDR must be listed `real` (else exit 2). `-trace SEED` prints each setup's r3. 2026-10-24; self-test `setup_test.py` |
| `setup`/`postsetup` operand `rN=*ADDR` or `rN=*(*ADDR+OFF)` (unit file) | the register gets the big-endian word at ADDR (or the word at that word plus OFF), read when the line runs on each side: for singletons an earlier original setup allocated (AGL#3: `r3=*101F8D6C`, `r5=*(*101F8C14+0x10)`). A dereference that reads 0 exits 2. 2026-10-25; self-test `setup_deref_test.py` |
| `setup`/`postsetup` operands `sp+OFF=VALUE` and `->ADDR` (unit file) | `sp+OFF=VALUE` (OFF 8..0x100, word-aligned) writes the big-endian word at the routine's entry SP + OFF before the call: stack parameters past r10 (AGL#3: 0279ADD4 reads its ninth argument at SP+8). `->ADDR` stores the returned r3 big-endian at ADDR (word-aligned) after the call, on each side, so a later line or the tested function finds the producer's result where its own caller stores it (AGL#3: the acquired handle at `SurfacePair::mHeader`). A returned 0 exits 2. `-trace SEED` prints `setup ADDR sp+0xOFF <- VALUE` and `setup ADDR ->STORE <- r3`. 2026-10-25; self-test `setup_store_test.py` |
| `setup`/`postsetup` operands `fN=FLOAT` and `fN=*ADDR` (unit file, N = 1..13) | set FPR fN to an f32 value (a decimal literal, or the big-endian f32 word at ADDR read when the line runs) as the double an `lfs` would load, both paired-single halves. For original setters taking float arguments (AGL#18: 0278CB8C(vol, proj, shift; f1 near, f2 far, f3 fov, f4 aspect) with `f1=*PROJ+0x94`-style words). `-trace` prints `setup ADDR fN = BITS`. 2026-10-26; self-test `setup_fpr_test.py` |
| `poke FUNC\|* ADDR\|rN+OFF u8\|u16\|u32\|f32 VALUE` (unit file) | after the `setup` lines and before FUNC runs (both sides), VALUE (or one of a comma-separated choice list `v1,v2,...`, picked per input from the input seed, the same on both sides; 2026-10-25, AGL#18) is stored at ADDR (or the entry rN plus OFF), big-endian. Use it to select a state in an object a setup constructor initialised (`field` steering is applied before the setups, so it cannot). Lines apply in file order; `-trace SEED` prints each store. Malformed lines exit 2. 2026-10-24; self-test `poke_test.py` Since 2026-10-26 the base may also be `r1+OFF` (OFF >= 8): a stack-argument word in the caller's frame at the entry SP, so stack arguments can vary per input after `setupsnapshot` (self-test `poke_test.py`). |
| `postsetup FUNC\|* ADDR [rN=VALUE ...]` (unit file) | like `setup` (same ABI, own stack, nested depth, ADDR must be listed `real`), but it runs after every `poke` line and before FUNC: for an original routine whose input is state a setup constructor would clear (construct with `setup`, select state with `poke`, then run the original consumer with `postsetup`). Lines run in file order. `-trace SEED` prints `postsetup ADDR -> r3 ...`. 2026-10-24 (AGL#3); self-test `postsetup_test.py` |
| `setupsnapshot` (unit file) | the `setup` lines (not pokes, not postsetups) of each tested function run once per process instead of once per input and side: their complete resulting state (every materialized byte with its current and initial value, write bits and float marks, the `alloc` cursors and `allocfail` call counts, the per-target mock counters, the access/call budgets used, trap count). The scratch stack window below the entry SP + 8, which holds the setups' own frames, is excluded from capture, install and both comparisons (2026-10-26, AGL#18: a callee spilling an uninitialised slot left per-input `stackfill`/generated bytes there and the snapshot was refused): each input materializes those bytes from its own generation, as the net effect also ignores them is captured and installed at the start of every input on both sides; pokes, postsetups, the post-setup baseline and the function run as before, so the results are identical. A setup operand `rN=*ADDR`/`rN=*(*ADDR+OFF)`/`fN=*ADDR` whose word a per-input absolute `field` (more than one value), `fieldstr` or `poke` line of the function can set refuses the snapshot outright (`setupsnapshot: refused (setup result depends on the input: setup ADDR operand rN=*ADDR reads a per-input ...)`). Otherwise the setups run twice before the inputs under two different input seeds; if the two states differ (a setup reads a generated field, register or random memory), or an `allocfail` pick can fail a call inside the setups, it prints `setupsnapshot: refused (setup result depends on the input: first difference at ADDR)` (stderr) and keeps per-input setups. The first input of a run and every 500th input (`-snapcheck N`) run the setups for real and compare them with the snapshot; a mismatch exits 2. The CPU-time budget is not charged for snapshot setups. `-trace SEED` prints `setupsnapshot: installed (N bytes)` (or `validated`). For original setup chains that cost seconds to minutes per run (AGL#3: the ShaderMgr archive parse). 2026-10-26; self-test `setupsnapshot_test.py` |
| `include PATH` (unit file) | inserts the lines of tools/verify/units/PATH at that point (recursive; a cycle or missing file stops). verify.py, mutate.py and mkunit.py read the expanded spec (the harness gets build/verify/<unit>/spec.expanded.txt), so directive order is the textual order after expansion; `# verify:` header lines come from the including file only. For shared fixtures such as `shared/agl_world.txt`. 2026-10-26; self-test `include_test.py` |
| `include shared/agl_world.txt` (shared fixture) | a complete original-produced agl world for any agl/sead spec: 16 MB root ExpHeap at 0x6A000000 (arena record 0x6E200000, 02755FE0; registry tree 027561B4; `nestedret 02756140 0x6A000000`; real alloc/aligned-alloc/free tree), static initialisers 0274D510 (colour presets), 02759B40 (SafeString), 02756E3C, 02757190 (default axes, `stable 104A0934 0x18`), 02757B48 (core table), 02759AD8 (endian); texture cache owner *101F8DD4 (0279F2EC + 0279F474 with GPU ranges 0x70000000/0x71000000, core pinned to 0); ShaderMgr 0x6B000000 over the user's agl_resource_cafe_dev.sarc at 0x6C000000 (`blob`), child heap holder *101F8C14 (027854AC/027855BC), sGraphicsAccessManager (027B7168), owner *101F8D6C (02786418/02786520), per-core table 027B9C1C; default texture *101F8DEC (027A76C8/027A78F8), resource provider *101F8DE4 (027A145C/027A5DF0), GraphicsDevice 0x5D300000 published at *101F8B18, singleton *101F8DDC (027A0A80); GX2 models (mockdisplaylist with BYTES 0x100 since 2026-10-29: a recording that issued GX2 commands ends with size 0x100, an empty one with 0; mockretfield, mocksurfacesize, mockfetchsize/init), mockset OSBlockSet/memset, mockmove, mocklookup 02786348. Reserved ranges (never steer them in an including spec): 0x6A000000–0x6AFFFFFF heap, 0x6B000000+0x1000, 0x6C000000–0x6D019E1D blob, 0x6E200000+0xC, 0x5D300000+0x160, 0x70000000–0x71FFFFFF, 0x7EFE7000+0x10000 setup stack, 0x101F8C24+0x100, 0x104A0934+0x18. The .bss words the setups produce (13 ranges in 104A01CC..104A2097, producers named in the file) are `stable` since 2026-10-27, so a mocked call in the tested function cannot clobber them (stores stay compared); the heap-tree OSMutex body 104A0338 (mocked OSLockMutex writes it) and the global-object list head 104DA130 stay clobberable on purpose, and the core id 028FDE28 is pinned to 0 only for nested calls (a tested function that calls it pins its own answer per spec). The including spec adds `src`, its tested function, its own objects (other ranges) and `setupsnapshot` (accepted, -watch 0:0x1000 clean, 2026-10-26). Its `real` lines are global: a tested function calling a listed original reaches the original, so a spec that needs one of them mocked must not include the world. To extend: add the new original producer as `setup * ADDR r..=` with EVERY argument register it reads given explicitly (an unset register is the tested function's generated input and breaks the snapshot), add its traced closure (`-trace SEED`, `nested call .. (real)`) to the `real` lines, rerun the probe `lib2_agl_world_probe` with `-watch 00000000:0x1000` (no line) and confirm `setupsnapshot: validated`. Known gaps: the GraphicsDevice vtable word (0x5D30000C) and *101F8B18 are pokes; GX2InitTextureRegs/GX2InitSampler descriptor words stay mock output. Scalar directives (`callbudget`, `accessbudget`) are last-wins: a line after the `include` overrides the world's value. 2026-10-26 |
| `include PATH for FUNC [FUNC ...]` (unit file; function-scoped include) | the same lines as `include PATH`, but every per-function directive whose function field is `*` (`setup`, `postsetup`, `field`, `fieldstr`, `fill`, `poke`, `cr`, `copy`, `blob`, `blobfile`, `arg`/`arg!`, `argf64`/`argf64!`) is written once per FUNC with that function instead of `*`, in the order given, where the `*` line stood; lines with an explicit function and the global directives (`stable`, `real`, the mock models, `nestedret`, budgets, `stackfill`, ...) are inserted once, unchanged; nested includes are scoped too (a `for` inside a scoped include, a missing or non-hex FUNC, or a FUNC named twice stops with `invalid include line`). Use it when only some tested functions of a spec may run the shared world's producers, e.g. `include shared/agl_world.txt for 0279345C 027931EC` (AGL#8) instead of hand-made scoped copies of the world, which drift from it (the AGL#8 copies lacked the stable ranges and the `mockdisplaylist ... 0x100`). The expanded spec shows the markers as `# >>> include PATH for FUNC ...`. 2026-10-29; self-test `worldscope_test.py` |
| `accessbudget N` / `timebudget S` (unit file) | raise this unit's per-run guest memory-access budget (default 20M; 1M..1G) or CPU-time budget (default 30 s; 30..600) for original setups that legitimately need more (AGL#18: the original ShaderMgr archive setup 02786520). Exceeding them still aborts the input, and aborts never count. Use the smallest value that completes and post the measured count. 2026-10-26; self-test `budget_allocfail_test.py` |
| `opbudget BYTES` (unit file; opt-in, once per spec) | raise the per-operation byte cap of the `mockset`, `mockzero` and `mockmove` (memmove) models together (default 20,000,000 bytes; BYTES decimal or 0x-hex, 1..1073741824; 0, a larger value, a malformed line or a second `opbudget` line exits 2). A larger fill or copy still aborts the input (`memset`/`mockzero`/`memmove exceeds guest operation budget`), and aborts never count; a large fill also needs `accessbudget` (one access per byte). Under `-v` every fill or copy over 1 MB through these models is listed on stderr: `opbudget: OP TARGET dst DST size SIZE lr LR side=orig\|cand depth=N seed=S` (LR = the return address the call passes; ` (over the budget)` when it aborts), so a start-up run lists them. Without `-v` nothing new is printed. 2026-11-01; self-test `opbudget_test.py` |
| `pagebudget N` (unit file); the per-run page budget | every run (setup, pokes, function; each side of each input) may touch at most N distinct guest pages (4 KB each; default 32768 = a 128 MB span; 4096..1048576). One more aborts the input on the side that ran, with `page budget exceeded (N guest pages touched in one run)`, like the access, call and time budgets: an abort never counts as a detected difference. The count is per run, not per allocation, so both sides count alike although the candidate reuses the pages the original's run created. A harness-internal read is counted but never aborts. Why (2026-10-31, AGL#8 02794270): without -nulltrap an original that walks a NULL-based pointer chain through readable page 0 touches a new page every ~32 accesses, and its 20M access budget allowed ~600k pages at ~7 KB host memory each (5–10 GB per process, built and freed per input). That happens in nulltrap_compare's plain side and in -dsiout's non-nulltrap re-pre-run. The largest legitimate run measured (8 units incl. the agl world, AGL#18/#17, the AGL#19 command pool) touches 6150 pages. `WWHD_MEMSTAT=1` prints `runpages=` per run. Raise it only for a measured legitimate need. Self-test `pagebudget_test.py` |
| Host memory per guest page and per net effect | rows unchanged. (1) Page metadata: arena blocks 16-byte aligned (64 before), clobber epochs as 16-bit offsets from a per-chunk base (widened to 32 bits only when a chunk's epochs do not fit), float marks in a per-page side table allocated by the first mark: a fully touched page 14.7 KB -> 10.6 KB of host RSS (self-test `pagerss_test.py`: the slope over 1k..32k touched pages). (2) A run's net effect is a sorted vector (8 bytes per changed byte) instead of a std::map (~48): a setupsnapshot capture of a J3D lighting spec holds ~4.9M. (3) A `fill` line covering whole 4 KB pages shares one read-only page per fill byte (copied on a later write) instead of a 4.6 KB copy per page (16 MB zero heaps). Harness peak RSS, n20 seed 1: lib2_j3d_hdrew_1_lighting_lightfix 574 -> 321 MB, lighting_nonpositive_produced 571 -> 340 MB, ctor_world 1460 -> 874 MB, AGL world specs ~425 -> ~270 MB |
| `hostmembudget MB` (unit file) / `WWHD_VERIFY_HOSTMEM_MB=MB` (environment); the host memory budget | the harness process's resident size (RSS) is sampled at each run's safe points (every 65,536 guest accesses and every 256 newly touched guest pages, in setups, pokes and the function, either side); past MB megabytes the input aborts on the side that ran with `host memory budget exceeded (RSS N MB > MB MB in one run)`; the run's net-effect map is sampled too (every 65,536 written bytes it collects: a runaway run can leave tens of millions of them, 8 bytes of map each since 2026-11-02), and a run that passes the budget there ends as an abort with `... in one run, collecting its net effect)` (2026-11-09: a run that already aborted for another reason keeps it first and gets the cause appended, e.g. `access budget exceeded; host memory budget exceeded (RSS N MB > B MB in one run, collecting its net effect)`; a run the budget already ended inside the run keeps that reason alone; each such abort is also printed on stderr as `hostmembudget: FUNC SIDE side, gen seed=S: REASON` (the first 10 per process), and mutate.py receipts carry `hostmem_aborts` and `hostmem_cause` (`run` / `net-effect collection`); self-test `hostmembudget_net_test.py`), like the access, call, page and time budgets: an abort, never a detected difference, so a mutant whose own run blows the harness up gets an ordinary receipt (class `abort` when only the mutant side aborted) instead of the RSS watchdog killing the whole process without one (2026-11-01: AGL#16 027C0CEC #5/#56 `literal 4->5 @RenderQueue.cpp:452/453`, AGL#14 archive_members_synthetic #2 at seed 7). Default 2816 MB, just under the 3072 MB per-process watchdog (`guard.py`, the job daemon); the directive and the variable take 64..65536; the environment variable overrides the directive for every unit (0 = off). RSS depends on the process, not on the input, so such an abort is handled like a time limit: an original that hits it is never cached in `-refcache` or a `-snapcache` validation. Host code that allocates without any guest access is not sampled (the watchdog remains the backstop there). Specs that stay under the budget give byte-identical output. Self-test `hostmembudget_test.py` |
| `depthbudget N` (unit file); the nested `real` call depth budget | a run may nest at most N `real` callees (V.real_chain, top level and nested calls; default 1024; 64..2048). Entering one more aborts the input on the side that ran with `depth budget exceeded (N nested real calls)`, like the access, call, page and time budgets; an abort never counts as a detected difference. Why (2026-10-31, AGL#8 027931EC gen seed 1001489): without -nulltrap the original read page 0 and its real heap routine 027540AC called itself with r3 = 0 about 6000 deep, below the call and page budgets, until the host stack overflowed (SIGSEGV, a dead shard). `-trace` shows only the innermost 16 levels of a deep `[inside real ...]` chain. Self-test `depthbudget_test.py` |
| `nulltrap` (unit file) | the `-nulltrap` option for every run of the unit (verify.py, verify_all.py, mutate.py): page-0 guest accesses are console DSI trap events. Takes no operands A shown first difference on an input where the original itself faulted on page 0 carries `[original page-0 fault on this input: no evidence]`, also when both sides fault at the same access: such a row never kills (`cascade.row_kills`), a clean input's difference is shown instead whenever one exists, also across shards. Self-test `p0row_test.py`. |
| `strictmocks` (unit file; opt-in, 2026-11-02) | inside a setup or postsetup, a call that gets the GENERATED answer of the generic mock for an import declared value-returning (r3 or f1) or not declared ends the run with exit 2: `strictmocks: setup ADDR calls import NAME from FUNC (lr LR)`. The answer is the point of failure (the harness cannot see the caller read r3). Answers the spec states (`ret!`, `retarg`, `mockset`, osmem parts, ...) pass, and so do imports declared `importvoid`. Every other import counts as value-returning: mkunit marks all imports so, whatever their real signature. `strictmocks report` (diagnostic only; a fixture is accepted only under the stopping form): the run goes on, each (setup, import, caller, lr) prints `strictmocks report: first: ...` at its first hit and a normal exit prints the counts (`no generated import answer inside setups` when none); the run then continues on the generated answers and may fault later. Scope: imports only. Setups run `real` originals, which reach every import through an imp_ thunk that mkunit routes to the slot, so a setup never calls an import stub (self-test 6 checks the built unit.c); an internal game function a setup leaves to the generic mock is the spec author's responsibility. Off by default; unknown to older harnesses, which ignore the line. Self-test `strictmocks_test.py`. |
| `importvoid SLOT...` (unit file; opt-in, 2026-11-02) | the import's HLE in the port is void: mkunit gives it defr = deff = 0, so the generic mock leaves r3 and f1 as they were (as the port's HLE macro does) and `strictmocks` lets it pass. Give the port file:line as evidence next to the line. A SLOT that is not a function import slot (a stub, a game function), or a `ret`/`ret!`/`retarg`/`fret`/`fret!`/`fretf64`/`fretf64!`/`mockretfield`/`nestedret` line on the same import, ends mkunit with an error. Units without it are unchanged. `-trace` prints `importvoid SLOT: r3 ... f1 ...` after such a call. Self-test `strictmocks_test.py` 7. |
| `mockretfield TARGET rN OFF SHIFT MASK` (unit file) | the mock of TARGET returns `(word at rN+OFF >> SHIFT) & MASK` in r3, at top level and inside `real` callees: the port HLE's deterministic result for query imports whose result the original stores (GX2GetVertexShaderGPRs C0006390 `r3 0 0 0xFF`, …StackEntries C0006398 `r3 0 8 0xFF`, pixel C00062D0/C00062D8, geometry). 2026-10-26 (AGL#3 ShaderMgr startup); self-test `gx2model_test.py` |
| `mockdisplaylist BEGIN END GETCUR [BYTES]` (unit file) | models the GX2 display-list trio as the port HLE does (gx2_core.cpp): BEGIN (GX2BeginDisplayListEx C0006090) records r3 = start, r4 = size; END (GX2EndDisplayList C0006190) returns the bytes written and closes the recording: min(BYTES, open size) while a recording is open AND at least one GX2 import call was made into it, else 0; BYTES defaults to 0 (mocked GX2 calls emit nothing). Command condition: BEGIN resets a per-recording counter; every call of a gx2 import slot (C0006090..C0006AE8, gen/imports.json lib gx2, other than the directive's BEGIN/END/GETCUR) made while the recording is open counts, at top level and nested (inside `real` callees and setups); END returns 0 for an empty recording even with BYTES (only a recording that issued GX2 commands has a nonzero size, as on hardware: shipped ShaderMgr program recordings 027BB9E0 → 027C073C … 027BB7CC … 027C077C always issue GX2SetVertexShader/PixelShader/GeometryShader). Known limit: commands a mocked NON-import callee would issue inside the recording are not seen (the mock runs no code), so such a recording reads empty = 0; a spec that needs that size must list the callee `real`. `-trace` prints `mockdisplaylist END START -> SIZE (GX2 calls N)`. A nonzero BYTES (2026-10-29, AGL#18) models the shipped nonzero command size, so an original recorder such as 027C077C stores a nonzero list size (027BB9E0 programs: `mDisplaySize`) and draw paths take their GX2CallDisplayList arms. The directive is last-wins and also applies to setups: `shared/agl_world.txt` sets `mockdisplaylist C0006090 C0006190 C0006218 0x100` since 2026-10-29, so the world's ShaderHolder load (02786520 → 027BBB9C/027BB9E0) records every program with size 0x100; a spec that includes the world and needs another size, or the legacy zero-size model (no BYTES, e.g. a type-domain companion), puts its own line AFTER the include. GETCUR's start stays the BEGIN start (GX2GetCurrentDisplayList reports the list base, not a write cursor), so 027C077C's END + (current − base) equals END; GETCUR (GX2GetCurrentDisplayList C0006218) writes *r3 = start and *r4 = size (each only if the pointer is nonzero; 0 when none) and returns start != 0. Top level and nested. Replaces random mock outputs the original stores (AGL#3: 027C077C's list size), so setups become input-independent (setupsnapshot accepted). 2026-10-26; self-test `gx2model_test.py` (incl. the command condition on 027C073C/027C077C: empty → 0, nested and top-level GX2 call → min(BYTES, size), no BYTES → 0) |
| `allocfail TARGET K[,K...]` (unit file, after TARGET's `alloc`/`allocfixed` line) | per input one K is picked from the list (the same on both sides); the K-th steered call of TARGET in the run (1-based, top-level and nested together) returns NULL without advancing the cursor, as on an exhausted heap. K = 0 means no failure. For nested allocation-failure edges (AGL#17 027C6C54): list 0 plus each call index. `-trace` prints `alloc TARGET -> 00000000 (allocfail: call K)`. 2026-10-26; self-test `budget_allocfail_test.py` |
| `observearg TARGET[@N] [rN+OFF]+OFF2 SIZE [if ...]`, `observeptr TARGET [rN+OFF]+OFF2 SIZE` (unit file) | pointee-offset span (2026-10-30, AGL#3 Mip request): at every top-level call to TARGET the SIZE bytes (1..256) at (the word stored at rN+OFF) + OFF2 are compared, e.g. `observearg 0274D690 [r3+0x1C]+0x94 8` for the 8 bytes at *(r3+0x1C)+0x94. The bracket form is the one `mockfield` uses; `observeptr TARGET rN+OFF SIZE` is the special case `[rN+OFF]+0`. Details below the per-site `observearg` table; self-test `nestobs_test.py` |
| `observenested TARGET [SPAN SIZE]` (unit file) | calls to TARGET made inside `real` callees (2026-10-30, AGL#3 Mip/D3C request) are recorded in order with their enclosing top-level call and compared original vs candidate: number, order, target and the SIZE bytes of each SPAN (`rN+OFF` or `[rN+OFF]+OFF2`) at call entry, e.g. `observenested C00063D0 r3+0x74 4` (GX2InitColorBufferRegs' viewMip inside 027B6DDC). Details below the per-site `observearg` table; self-test `nestobs_test.py` |
| `nestedret TARGET VALUE` (unit file) | inside a `real` callee, a nested call to TARGET returns VALUE in r3 instead of a random answer (e.g. a TLS heap getter) |
| `mockfetchsize TARGET` / `mockfetchinit TARGET` (unit file; also nested inside `real` callees) | GX2 fetch-shader imports: size = runtime HLE formula of the stream count (r3); init writes 32 descriptor bytes (r3) and 16+n*16 program bytes (r4), n = r5 ≤ 16, derived from the n*32 stream bytes at r6 |
| `ret! ADDR …` / `fret! ADDR LO HI` (unit file) | like `ret`/`fret`, but always applied (no 1-in-8 escape) and also to nested calls inside `real` callees |
| outgoing linkage (always) | every guest call gets a 16-byte outgoing frame (back chain + LR save word), so a real callee's LR store at caller SP+4 never hits a `gabi::Local` payload |
| `rec DIR` (unit file) | local recordings (`DIR/ADDR/*.tap`, relative to the repository root; not distributed, skipped with a note when the directory is missing) replayed on every `verify.py` / `verify_all.py` / `mutate.py` run of the unit; an explicit `-rec` / `--rec` takes precedence. Game-test regressions (e.g. d_grass Grass_ShaderBuild 2026-10-05) stay covered this way |
| `stable ADDR SIZE` (unit file) | a steered region (e.g. stage path data at a pinned address) that callees never write: excluded from clobbering |
| `fill FUNC\|* ADDR\|rN+OFF SIZE BYTE` (unit file) | SIZE bytes (1..0x1000000) from ADDR all hold BYTE in every generated input of FUNC (`*`: all functions), both sides. Applied before the `field` steering, so `field` lines over the same bytes win (a background plus a few targeted words). Use it instead of thousands of per-word `field` lines, e.g. a 512 KB all-0xFF table that drives a scan to its cap: `fill 027201E4 5B000000 0x80000 0xFF` (hd_inf_save_holders/cap_boundary). Add `stable` for the region if callees must not clobber it. Self-test: `fill_test.py` |
| `fieldstr FUNC\|* ADDR\|rN+OFF [SIZE] "s1","s2",...\|@stages` (unit file) | one whole NUL-terminated string per input, the same on both sides (e.g. the play object's start stage name at play+0x5134). `@stages` adds the real stage list `tools/verify/stage_names.txt` (the `<name>_Stage.szs` archives); strings and `@stages` can be mixed, repeat a string to weight it; `\xNN` escapes a byte; each `@keep` pick leaves the bytes to the generator and earlier `field` picks of the same bytes (keep their targeted values in the mix). About 1 input in 16 gets the empty string and 1 in 16 keeps the generated bytes. For an absolute ADDR the SIZE bytes (default 8) are also `stable`. Use it wherever code compares a name with literals (strcmp/strncmp loops, inline SafeString compares): per-byte `field` picks almost never form a real prefix (the d_grass `kin` loop bug of 2026-10-05 went unnoticed that way) |
| `mockmove TARGET rDST rSRC rSIZE` (unit file) | (also when called from inside a `real` callee) the generated mock of TARGET (e.g. OSBlockMove C0009988) really copies SIZE bytes from SRC to DST (overlap-safe, source read before the clobber epoch) and returns DST |
| `mockset TARGET rDST rVAL rSIZE` (unit file) | the generated mock of a byte fill (OSBlockSet C0009990, memset) really stores the low byte of VAL SIZE times at DST and returns DST, after the call's clobber epoch, at top level and inside `real` callees (e.g. a constructor clearing its storage, a query clearing its stack bitmap). Arguments are still compared. Malformed lines exit 2; `-trace SEED` prints each fill. 2026-10-24 (AGL#3/AGL9 requests); self-test `mockset_test.py` |
| `mockzero TARGET rDST rSIZE` (unit file) | the generated mock of DCZeroRange (coreinit C00088C8) zeroes the cache lines it covers with the shipping port's formula (runtime/src/hle/coreinit_mem.cpp): from DST&~31 for ((DST&31)+SIZE+31)&~31 bytes, at top level (after the call's epoch) and inside `real` callees; arguments stay compared. `-trace SEED` prints `mockzero TARGET lines START size N`. 2026-10-24 (AGL#14); self-test `mockzero_test.py` |
| `mocksurfacesize TARGET` (unit file) | GX2CalcSurfaceSizeAndAlignment (C00060D0): the generated mock rewrites the 0x74-byte GX2Surface at r3 with the shipping port's own formula (`runtime/third_party/cemu/gx2_surface_calc.cpp`, linked through `src/surface_calc.cpp`): tileMode, numLevels, swizzle, imageSize, alignment, pitch, mipSize, mipOffset. The descriptor is read before the call's clobber epoch and written back whole after it, at top level and inside `real` callees (e.g. a texture initializer whose allocation size is the computed imageSize). A descriptor outside the modelled domain (dim > 7, tileMode > 16, format type 0 or unknown flag bits, aa > 3, width/height > 16384, depth 0 or > 2048, numLevels > 1 with width and height both 0 on a non-3D surface, where the level count would become 0 and imageSize/alignment/pitch stay unwritten) aborts the input on both sides; that is a harness limit, not an SDK claim. Width or height 0 is modelled: the formula clamps each level's extent to max(x >> level, 1) (LatteAddrLib.cpp:961–962, 1083, 1091), so imageSize, alignment and pitch equal the 1-pixel case and the width/height fields stay 0. Use `observearg C00060D0 r3+0 32` for the input fields as before. Malformed lines exit 2; `-trace SEED` prints imageSize/alignment/pitch. 2026-10-24 (AGL#8 request); self-test `mocksurfacesize_test.py` |
| Mock-model rejections (mocksurfacesize null/out-of-domain descriptor, mocklookup index outside the table, `alloc` alignment negative / not a power of two / size over 16 MB, fetch-shader stream count over 16) | a model's domain check still ends that side's run as an abort (the reason as before), but since 2026-10-29 the run records which call the model rejected (the top-level call is recorded before the model runs; a nested one ends inside its top-level `real` call), and the comparison uses the calls up to and including it. **One side rejected, the other ran on:** those calls are compared; a difference is reported as usual (`call #0 C00060D0 ...: r3 original 50000000 candidate 50000074`), else the rejection itself is the difference: `call #K TARGET: argument outside the modelled domain on the candidate side only (WHY)` (nested: `call #K OUTER: nested call TARGET (depth D in F) has an argument outside the modelled domain on the ... side only (WHY)`). These are ordinary FAIL rows (mutate class `difference`), where before they read `only one side aborted: ...` (class `abort`, never a kill). **Both sides rejected:** a difference in the common call prefix is reported with ` (before mock-model rejections on both sides)`; otherwise the input counts as aborted on both sides as before. The other side aborting for another reason, and rejections inside setups, keep the old abort. -refcache records carry the rejection (format RFC4). Self-test `modelreject_test.py` |
| `capture NAME SHA256` + `capturemap ADDR SIZE` + `capturecall SHA256` | one real state captured from a plain port build at the tested function's entry (port debug hook `WWHD_GUEST_CAPTURE`, private file under `$WWHD_CAPTURE_STORE`, named by SHA-256 of the decompressed bytes; never in a repository). Only bytes inside a `capturemap` window start from the capture; every other byte keeps the normal rule. `.rodata` and the port-runtime regions are refused as windows; `setup`, `postsetup`, `poke`, `field`, `fieldstr`, `fill`, `copy`, `blob` are refused in such a spec; `capturecall` hash-checks the manifest beside the capture and every `arg!` must equal the captured register (drift guard); a capture run never clobbers. Diagnostics: `WWHD_CAPTURE_READLOG=FILE` logs the capture bytes guest code read (and alone allows wider windows and no `capturecall`, to measure the read set); `WWHD_CAPTURE_GENLOG=FILE` logs every generated byte guest code read, empty for a closed read set. Self-test `capture_test.py`, no-change proof `capture_nochange.py BASE_TREE UNIT...` |
| `osmem hle` / `osmem PART...` `[dataend=HEX] [sysinfo=HEX]` (unit file; opt-in, once per spec) | the shipping port's HLE OS/MEM backend, so an ORIGINAL startup (hdAppMain 02005EA8 → Framework::run → TaskMgr root prepare) runs on the port's own answers with no poked values. `hle` = all parts; each import below is answered at top level (after the call's clobber epoch, like the other models) and inside `real` callees, setups and postsetups, and leaves the registers as the HLE does (r3 = the HLE result, unchanged for void functions; nothing else written). Parts and their port sources (zelda_wwhd `runtime/src`): **heaps** — `hle/coreinit_mem.cpp`: ExpHeap first fit line for line (`ExpHeap::alloc/take/release/largest/total_free`, head and tail allocation, any size, no 16 MB limit, alignment as the HLE rounds it), FrmHeap head/tail (`frm_alloc`), `mem_setup_heaps`' boot layout (default ExpHeap at (data_end + 0xFFF) & ~0xFFF = 0x104DB000 up to `kMem2End` 0x50000000, blocks from +0x40; MEM1 FrmHeap 0xF4000000 + 0x02000000; foreground FrmHeap 0xE0000000 + 0x02800000; `runtime.h:14-23`) with the `create_exp`/`create_frm` "EXPH"/"FRMH" header words, the imports MEMCreate/DestroyExpHeap(Ex), MEMAllocFromExpHeapEx, MEMFreeToExpHeap, MEMGetAllocatableSizeForExpHeapEx, MEMGetTotalFreeSizeForExpHeap, MEMCreate/DestroyFrmHeap(Ex), MEMAllocFromFrmHeapEx, MEMGetAllocatableSizeForFrmHeapEx, and MEMGetBaseHeapHandle by argument (0 → MEM1 0xF4000000, 1 → default 0x104DB000, 8 → foreground 0xE0000000, else 0), plus the default-heap functions behind the data imports (`default_alloc` align 0x40, `default_alloc_ex`, `default_free`). The bookkeeping is host-side as in the port and part of the run state (setupsnapshot, -fastreset, -snapcache). **data** — the loader's data-import words (`main.cpp init_data_imports`, `coreinit_mem.cpp mem_init_data_imports`, value slots C1000000..C1009FFF from gen/table.c): zero except MEMAllocFromDefaultHeap/Ex/MEMFreeToDefaultHeap = C2000000/C2000008/C2000010 (the first `register_host` addresses, `core.cpp` kHleFuncBase, 8-byte steps; calls through them reach the heap model and are code targets for -nulltrap), `__gh_FOPEN_MAX` = 20, `environ` = its slot + 0x10; `stable`. **gx2** — GX2CalcSurfaceSizeAndAlignment C00060D0 = the `mocksurfacesize` model; GX2CalcDepthBufferHiZInfo C00060B0 and GX2CalcColorBufferAuxInfo C00060A0 store big-endian 0x1000 at *r4 and 0x100 at *r5 (`gx2/gx2_resources.cpp:28-29`); GX2Init C00063A8 and GX2SetContextState C00067E0 have no guest effect (`gx2/gx2_core.cpp:627, 644`); GX2SetupContextStateEx C0006A60 writes the two host-endian (little-endian) restore words OP_SET_CONTEXT|1<<8 = 0x10A and ctx at ctx+0x9800 (`gx2_core.cpp:634-643`, `gx2/gx2_cmd.h` OP_SET_CONTEXT = 10); GX2GetContextStateDisplayList C00061F0 stores ctx+0x9800 / 8 through r4 / r5 when nonzero (`gx2_core.cpp:645-648`); GX2CalcTVSize C00060D8 / GX2CalcDRCSize C00060A8 store the scan-buffer size w*h*4*max(buffering,1) (TV 854x480 / 1280x720 / 1920x1080 by mode, DRC 854x480) at *r6 and 0 at *r7 (`gx2_core.cpp:889-899`); the state setters the startup calls (GX2SetTVBuffer, DRCBuffer, TV/DRCScale, SwapInterval, TargetChannelMasks, StencilMask, PolygonControl, DepthStencilControl, ColorControl, BlendControl, BlendConstantColor, AlphaToMask, AlphaTest) have no guest effect and keep r3 while no display list is open (`gx2_state.cpp` set_reg → `gx2_core.cpp` emit; inside a recording they keep the generic/mockdisplaylist handling). **sysinfo** — OSGetSystemInfo C0009C80 (`hle/coreinit_misc.cpp:287`) returns one 0x20-byte block: bus clock 248625000, core clock 1243125000, the rest zero. The port takes it from its runtime bump allocator, whose address depends on its boot order, so the block is at 0x6FFE0000 (`sysinfo=HEX` moves it); `stable`. **ghs** — the GHS libc lock imports (`threads.cpp:860-869`, `coreinit_misc.cpp:308`): __ghsLock/Unlock, __ghs_flock_file/funlock_file/flock_destroy, __ghs_mtx_dst/lock/unlock and __gh_set_errno have no guest effect (host locks); __ghs_flock_ptr returns and __ghs_mtx_init stores at *r3 a fresh zeroed runtime_alloc block (4 / 8 bytes, align 16), here from a fixture bump region 0x6FF00000..0x6FFE0000 (bump pointer in the run state). **bss** — the loader's zero fill of.bss (`core.cpp load_rpx`: memset of the NOBITS section 0x101FCE00..data_end 0x104DA1C8): untouched bytes start as 0, not generated words (not `stable`). **zero** — the port's zero-initialised guest memory (the 4 GB reservation is anonymous memory) for the arenas the backend hands out: default heap past its header up to 0x50000000, MEM1 and foreground past their headers, the bump region; untouched bytes start as 0. data_end (default 0x104DA1C8 = cking.rpx's.bss end, the port's `load_rpx` rule) and the.bss start are checked against the RPX by `osmem_test.py`. A modelled target named in another mock directive (`ret`/`ret!`, `alloc`, `nestedret`, `retarg`, `mockset`, …; `mocksurfacesize C00060D0` is allowed, it is the same model) exits 2, as do malformed lines and a second `osmem` line. **os** — OSSetExceptionCallback → 0 (`threads.cpp:766`), OSInitMutex / OSInitMessageQueue store "mUtX" / "mSgQ" at r3 (`threads.cpp:844-853, 956-967`), OSLockMutex / OSUnlockMutex no guest effect, OSTryLockMutex → 1 (one guest thread), FSInit / FSSetStateChangeNotification / ProcUIInit no effect, FSAddClient / SAVEInit → 0 (`hle/fs.cpp:188-194, 298`, `hle/system_stubs.cpp:69`), the nn::olv constructors the port leaves unimplemented → 0 (`core.cpp hle_unimplemented`). Not modelled: OSGetTime / OSGetSystemTime (the port answers the wall clock: a startup that stores a tick, e.g. sead::TickTime 02760DE8, stays input-dependent unless the spec pins it), the rest of the thread layer (`mockthreadspecific` covers TLS), memcpy/memset (`mockset`/`mockmove`). Default behaviour of specs without the directive is unchanged. `-trace SEED` prints each answer (`[nested ]osmem TARGET r3=.. r4=.. r5=.. -> RESULT` or `(void)`). Self-tests `osmem_test.py` (HLE numbers on the startup's own callers, HiZ outputs, rows at seeds 1/7 with and without -nulltrap, setupsnapshot/-nofastreset/-snapcache/-refcache, off by default, parts, conflicts, malformed lines) and `osmem_equiv.py` (the heap model against coreinit_mem.cpp's own ExpHeap/FrmHeap code compiled side by side, 280k random operations) |
| `osmem ... fs` + `osmemfile GUESTPATH SHA256\|absent` (unit file; opt-in, NOT part of `osmem hle`) | coreinit FS answered read-only from declared game files, as the port's `runtime/src/hle/fs.cpp` does with no content mods and no language-pack redirect: FSOpenFile (read modes only; `w`/`a`/`+` is a model rejection; a fresh handle from 1 up, `g_next_handle`, stored at *r7; FS_NOT_FOUND -6 for a declared-absent file), FSReadFile (size*count bytes from the position, r3 = bytes read / size, 0 for size 0 or an unknown handle), FSSetPosFile (0, or FS_ACCESS_ERROR -10 for an unknown handle), FSGetStat / FSGetStatFile (`fill_stat`: 0x64 bytes, flags 0x01000000, mode 0x666, size at +0x10/+0x14; -6 when absent/unknown), FSCloseFile (0), FSInitCmdBlock (0xA80 zero bytes), FSGetCwd (`write_cstr` "/vol/content" with max r6), FSGetVolumeState 1, FSSetCmdPriority / FSGetLastError(CodeForViewer) / FSDelClient / FSAddClient 0, FSInit / FSShutdown / FSSetStateChangeNotification no effect; directories (FSOpenDir/ReadDir/CloseDir) and FSWriteFile are model rejections. Paths map as `host_path_exact` (/vol/content, /vol/code, /vol/meta under `game/`, a relative path under game/content; /vol/save is refused) and match case-insensitively (`resolve_case`). Every file the model may read is declared by `osmemfile` with its SHA-256, checked when the spec loads (a file read through the `game` link, never written); `absent` declares a path that must not exist. An undeclared path is a model rejection. Open handles and positions are run state (setupsnapshot, -fastreset, -snapcache). Malformed lines, a wrong hash, a missing file with a hash, `absent` for an existing file, a path declared twice (any case) or under /vol/save exit 2. Self-tests `osmem_fs_test.py` and `osmem_fs_equiv.py` (the model against the port's fs.cpp compiled with shim headers on a temporary game tree, 24k random FS operations). Not enabled by default |
| `osmem ... uc` (unit file; opt-in, NOT part of `osmem hle`) | the system-settings imports as the port's `runtime/src/hle/coreinit_misc.cpp:315-340` answers them with no language setting: UCOpen C000A430 → 1, UCClose C000A408 → 0, UCReadSysConfig C000A448 (handle, count r4, items r5): per 0x54-byte UCSysConfig item (name[64], access +0x40, dataType +0x44, error +0x48, dataSize +0x4C, dataPtr +0x50) the value by name — cafe.language 1 (`console_language()` without WWHD_LANGUAGE or a saved setting: English), cafe.cntry_reg 49, cafe.eula_agree 1, cafe.initial_launch 2, parent.enable 0, p_acct1.network_launcher 0, any other name 0; when dataPtr and dataSize are nonzero the data is zero-filled and the value stored big-endian as u8 (size 1), u16 (size 2) or u32 (size ≥ 4; size 3 stays zero); the error word is set to 0; r3 = 0. More than 4096 items or an item over 1 MB is a model rejection. A UC import named in another mock directive exits 2. Self-tests `osmem_uc_test.py` (the original 0286D670 as setups: cntry_reg size 4 → 00000031, language size 1 → 01, parent.enable size 2 → 0000; off without `uc`; conflicts) and `osmem_uc_equiv.py` (the port's three UC HLE bodies compiled with shim headers against the model, 3000 random item arrays).  |
| `osmem ... threads` `[mainthread=HEX] [threadsched=N] [threadclock=HEX] [threadbudget=N] [threadcap=N] [threadslice=N] [threadrate=N] [threadlog=N] [ostime=SECONDS] [nodrain]` (unit file; opt-in, NOT part of `osmem hle`), harness option `-threadsched N` | a cooperative guest-thread model: original code that creates threads and waits on them (JAudio, nw::snd SoundThread/TaskThread, sead::Thread) runs for real instead of being skipped. Semantics as the port's `runtime/src/threads.cpp` (object model `src/gthr_model.h`, scheduler `src/gthr_sched.inc`): OSCreateThread (the 0x6A0-byte OSThread block written as create_thread does: tag, state, attr, id, suspend 1, priorities, stack, entry, affinity), OSResumeThread, OSExitThread, OSJoinThread, OSGetCurrentThread, OSGetCoreId (the thread's core: `core_from_affinity(attr, creator's core)`, threads.cpp:116-121; the main thread core 1), OSYieldThread, OSSleepTicks, OSSetThreadName/Affinity, OSGetThreadPriority, OSSet/GetThreadSpecific (+0x57C), OSDisable/RestoreInterrupts, OSInit/Lock/TryLock/UnlockMutex and the GHS locks (recursive, owner per thread), OSInit/Signal/Reset/WaitEvent(+WithTimeout) (auto/manual reset, wake tokens), OSInit/Send/ReceiveMessage (16-byte messages; send flag 1 block, 2 front; receive flag 1 block), OSInit/WaitRendezvous (per-core arrivals, as on Cafe OS and in the port since f5554821 (aaa5fcde, runtime/src/rendezvous.h): a waiter marks the core it runs on and waits until every core in its mask has arrived; two threads on one core are one arrival; in the start-up the core-1 main thread's `OSWaitRendezvous(104A11E0, 1)` in 02762690 waits for the core-0 audio launch thread), OSSleepThread, alarms (OSCreateAlarm, OSSetAlarm, OSSetPeriodicAlarm, OSCancelAlarm, OSSet/GetAlarmUserData as threads.cpp arm_alarm/alarm_thread: the callback (alarm, 0) runs on the alarm service thread, created at the first alarm with priority -1 on the arming thread's core, OSThread block 6FFE1800 and guest stack 6FFE3000..6FFFF000, both zeroed; a periodic alarm re-arms at when + period, or now + period when that is past; a cancelled or re-armed alarm's stale entries are dropped when they reach the head (threads.cpp alarm_thread at f5554821); OSSetPeriodicAlarm's start is an OSTime, taken back to the guest timebase as os_time::to_guest does; the virtual clock also jumps to the next live alarm; the callback must be `real` and runs as core.cpp guest_call does on the service thread's persistent Cpu: a 0x40-byte frame with its back chain stored below r1, the arguments in r3.., lr 0, r1/lr/ctr restored afterwards, the other registers kept from the previous callback), and the clocks from the model's virtual tick clock (only with `threads`): OSGetSystemTime and OSGetTick the guest timebase, OSGetTime the calendar clock = timebase + bias (os_time.h at f5554821; the bias of a repeatable run, kFixedOSTime 2026-01-01 12:00, unless `ostime=SECONDS` gives a date in seconds since 1970 as WWHD_OS_TIME does). Each import is also answered at its RPX fimport stub (slot - 0xBD70BD88, e.g. 028FDE28 OSGetCoreId). The main thread (id 0) is the harness's run context (setups and the tested call) with its OSThread block at `mainthread=` (default 6FFE1000; initial words like the port's run_main: id 1, attr 2, priority 16, entry 028EA120). **Execution:** every other guest thread runs on its own host coroutine (64 MB mmap'd host stack, touched pages only, so it counts in the RSS that the host memory budget samples) with its own Cpu and the guest stack OSCreateThread names; exactly one context runs at a time. A thread entry must be `real` (else a model rejection). Thread code runs nested (its calls are answered like calls inside `real` callees); a unit (tested) function reached from a thread runs the original on the original side and, from the tested call on, the candidate (or mutant) on the candidate side, so a mutant in thread-reached code is observable. During setups, postsetups and their drain both sides run the original: a mutant cannot change a setup on the candidate side alone, and a firing only in the drain gets no credit. Limit: a function already on a thread's stack when the tested call starts stays original until it returns (a thread loop a setup started, for the whole row); the row names them on stderr (`threads: FUNC: unit function(s) on a thread stack at the tested call: ...`). A survivor of a mutant in such a function is no evidence from that row: it needs a row where the function runs as candidate from its entry (its own top-level row, or a thread the tested call starts). Kills stay valid. The per-thread call logs start at the tested call too (the setups' calls compare nothing); the schedule digest still covers the setups. **Schedule (deterministic):** a thread runs until it blocks, sleeps, yields, joins or exits; eligible = runnable and no runnable thread on its core with a strictly higher priority; a wake or resume that makes a same-core, strictly higher-priority thread runnable switches to it at once (Cafe preemption); otherwise the next eligible thread after the current one in creation order runs (round robin); nothing eligible: the virtual clock jumps to the earliest sleep or timeout; nothing waiting on the clock either: the run aborts `guest threads: deadlock (non-termination): ...` with the thread table. **Progress clock:** the virtual clock also advances with guest progress: `threadrate` ticks (default 64) after every modelled time read (OSGetTime, OSGetSystemTime, OSGetTick) and 1 tick after any other modelled OS call of the running thread, so a thread that never blocks (the J3D start-up's busy-poller re-posts message 5 to its own queue in 0203DEEC and polls the time in 0203DDC0) still moves the clock, as time passes on the console while it spins; without this the clock moved only when every thread waited, the poll never ended and the main thread asleep in GX2WaitForVsync never woke (a livelock the console does not have). The rates: the timebase runs at the Espresso bus clock / 4 = 62.15625 MHz and the cores at 1.243125 GHz, so one tick is 20 core cycles, about one uncontended lock or queue call; a time read stands for one iteration of a time-poll loop, 64 ticks = 1280 cycles (1.03 us), so one 60 Hz frame (1036973 ticks) is about 16200 iterations. Deterministic and documented, not measured. Guest compute between OS calls costs no clock, so a compute-only spin still does not move it. Paced by OS calls rather than guest accesses on purpose: equivalent original and candidate code makes the same OS calls in the same order (the thread call logs compare them) but not the same number of guest accesses (different code generation), so an access-paced clock would make OSGetTime/OSGetTick, and the schedule, differ between equivalent sides. Not 64 ticks for every call: a flag spin (027F9994: unlock, lock, read) then moved 700000 ticks per slice, AX frames arrived faster than core 0 could run them and their audio work, and the lower-priority core-0 worker the spin waited for never ran. When progress carries the clock to a sleep, timeout or alarm, a thread this wakes on the running thread's core with a strictly higher priority runs at once (the timer interrupt's preemption, trace `preempt-timer`; not while the running thread has interrupts disabled): the main thread wakes at its vsync tick and preempts a lower-priority worker on its core instead of waiting until that worker blocks; an eligible thread this wakes on another core also runs at that tick (trace `wake-timer`: its core is free on the console, and an access-counted slice would wake it at different points on the two sides); an equal-priority thread on the running thread's core still waits until the runner blocks or yields. `threadrate=0` is the clock before (only the jumps). Self-test `guestthreads_test.py` A `clockpoll` (finishes; `clockpoll0`, threadrate=0, ends at threadbudget). **Call-log bound:** the per-thread call logs of one run hold at most `threadlog` entries over all threads (default 4,000,000, 52 bytes each, about 200 MB); one more aborts the run fail-closed with a receipt naming the thread: `guest threads: the per-thread call logs exceed N entries in one run (threadlog): T22 (entry 0203DEEC) has M of them; last call ADDR: <thread table>` (an ordinary abort receipt, so a spin cannot eat the machine). **Slice preemption:** a thread that makes `threadslice` (default 2^20) guest accesses and modelled OS calls since its last switch (OS calls count too: a spin whose only access lies inside a lock expires after an unlock as well) lets an eligible thread on ANOTHER core run (racy polls such as `while (!flag);` are switch points; never to a same-core thread, which Cafe would not time-slice; not while its interrupts are disabled). **Perturbation:** `threadsched=N` / `-threadsched N` (N != 0; the option overrides the directive) picks the next thread by a seeded hash, may switch among the eligible threads at every sync point, draws the slice length per run (2^8..2^22) and offsets the clock origin (`threadclock=`, default 0) per run: the schedule-perturbation set of the counting rule (iii). The same seed and spec give the same schedule. **Drain:** after the setups (before a snapshot capture and the baseline rebase), after the postsetups, and after the tested call the main thread waits until no other thread can run without a clock jump (the running threads' OS calls still advance the clock), so work the setups start is baseline and work the tested call starts is part of its effect (`nodrain` switches it off). **Compared:** the net effect (each started thread's guest stack excluded like the main stack: private frames), plus each guest thread's call log (every call it makes that is not run for real: target, the callee's compared argument registers with pointers into its own stack normalised, r3 after), by creation order: `guest thread T1 (entry 027625E8) call #3 C0009ED0: r5 original 00000001 candidate 00000000`. **Ends of a run:** a page-0 fault (-nulltrap) or a deadlock of the original only, off the main thread, ends the input with no evidence (counted like an abort on both sides; stderr `guestthreads: ADDR: N input(s) where only the original faulted or deadlocked off the main thread`); a candidate-only fault is a difference (`... [in guest thread T2]`); a candidate-only deadlock is `only one side aborted: guest threads: deadlock (non-termination): ...`; a deadlock on both sides is an abort on both sides. Budgets: `threadbudget` scheduler events per run (default 1,000,000; also every slice expiry, so a spin with nobody to switch to ends there) and `threadcap` threads per run (default 64) abort with the thread table (an ordinary abort receipt, never a watchdog kill); the access/call/page/depth/time budgets apply as before. **Receipts:** every function with inputs that used the model prints `guestthreads: ADDR: schedule digest D over N input(s) with guest threads (threadsched S, up to T threads, W switches, P slice preemptions)` on stderr (the original side's per-input schedule digests combined); `-trace` prints every switch, clock jump, creation, exit and modelled call (`threads T1 C0009ED0 ...`) and each side's digest. **Snapshots and caches:** guest-thread contexts are host stacks, never serialised: a `setupsnapshot` capture is refused while the setups left any guest-thread state (`setupsnapshot: refused (guest threads: the setups left guest-thread state ...)`; the setups then run live per input), -fastreset's candidate-side install and -snapcache validation records are not taken for such inputs, and a -refcache writer stores no record for an input whose original used the model (readers run it live). Without `threads` nothing changes (no-change proof: byte-identical output). Self-tests `guestthreads_test.py`, `guestthreads_equiv.py` (the object model against the port's threads.cpp/rendezvous.h at f5554821) |
| `osmem ... threads ax` (unit file; opt-in, NOT part of `osmem hle`, needs `threads`) | snd_core and the AX audio frame as the port's `runtime/src/hle/ax.cpp` (f5554821) implements them (model `src/ax_model.h`, scheduler glue in `src/gthr_sched.inc`), so the audio threads that wait for the DSP (JAudio's audio thread on its AX device final-mix callback, nw::snd on its app frame callback) run for real. Every snd_core import the game uses (AXInit/Quit, AXAcquireVoiceEx/FreeVoice, every AXSetVoice* (offsets, loop, end/loop offsets, src ratio and src, src type, volume envelope, TV and GamePad device mixes, ADPCM, ADPCM loop, low-pass, biquad, state, type), AXGetVoice* (offsets, current offset, loop count), AXComputeLpfCoefs, AXRegister/DeregisterAppFrameCallback, AXRegister/GetDeviceFinalMixCallback, AXRegisterAuxCallback, AXSetAuxReturnVolume, AXSetDeviceUpsampleStage, AXGetDeviceMode (stereo, as the port with a stereo device) and the constant answers) leaves registers and guest memory as the port's HLE does. **AXInit** allocates the 96 VPBs and the frame buffers in the port's order and alignment from the osmem runtime bump region (`runtime_alloc`) and starts the **AX frame thread** like the port (make_service_cpu: priority -1, core 0, OSThread block 6FE10000, guest stack 6FE20000..6FE40000): one frame at once, then one every 3 ms of virtual time (frame k at the AXInit tick + floor(k * 3 * 62156250 / 1000)), until AXQuit. **A frame** is frame_thread's loop body: decode, resample, envelope, filter and mix every playing voice into the TV and GamePad buses (VPB state and current-offset stores), read the game's output faders (1018EC64 -> +0x10 -> +4, faders +0x194 TV / +0x1AC GamePad; GamePad aux effects only in Off-TV play), run the aux callbacks one frame behind, the app frame callbacks, store the aux input, build the final-mix inputs (48 kHz when upsampled first) and call the TV and GamePad final-mix callbacks with their parameter blocks; callbacks run as guest code on the AX frame thread (guest_call semantics as for alarms) and must be `real`. The port's host_alloc buffers (GamePad aux) sit in the fixed, zeroed region 6FE00000..6FE10000 (a spec whose other osmem regions, mainthread= or absolute fill/copy/blob lines overlap 6FE00000..6FE40000 exits 2). **Left out**: keep_dsp_channels (the port overwrites the game's DSP overload ratio at 10170030; the console keeps the check), everything after the final-mix callbacks (host output, 5.1, the GamePad-only host mix), the stats/trace switches, the device pacing and game_clock (rate 1). Frame state is guest-thread state (setupsnapshot refused, refcache live). Self-tests `ax_test.py` (original-code carrier 02762FB4: rows, the frame thread, GamePad aux buffers zero until written, spec errors) and `ax_equiv.py` (the model against the port's ax.cpp at f5554821 in one program: random voice/callback/fader sequences and frames, results, callbacks with memory hashes, every guest byte and voice state compared) Multi-frame self-test on original code `axframes_test.py` (committed carriers `units/_axframes_test/`): `service_call` frames, arguments and register persistence measured on the guest stack, the exact frame/callback/alarm sequence on both sides, and mutants in code reached only through an AX callback and an alarm callback killed through them. |
| `osmem ... pad [padpro]` + `padsched FRAME[-LAST] BUTTONS [LX LY RX RY] [touch=X,Y]` (unit file; opt-in, NOT part of `osmem hle`) | the GamePad and Wii Remote / Pro Controller imports as the port's HLE answers them (`runtime/src/hle/system_stubs.cpp` vpad section and `hle/padscore.cpp` at f5554821; model `src/pad_model.h`), with the button data from a press schedule in the spec instead of the host's input, so an original start-up can go LOGO → title → new game with scripted presses. **Reads:** VPADRead C0007640 (chan 0, a buffer and count > 0, else error -2 and r3 0): the 0xAC-byte VPADStatus as the port writes it: hold (the scheduled buttons plus the stick-as-button bits with the VPAD library's 0.5 / 0.1 hysteresis), trig / release against the previous read, the four stick floats, the motion block of the port's virtual GamePad at rest (gyro source off: `motion/fusion.cpp` VirtualPad, constant), the touch panel (raw x = tx*3883+92, y = 4095-ty*3694-254, kept from the last touch; touched 1 / valid 0, else 0 / 3) in the three samples at +0x52, slide volume FF/FF and battery C0, error 0, r3 1 (one sample whatever the count). KPADReadEx C00081A0: in the default GamePad mode no Pro Controller is connected (error -2, r3 0, every channel); with `padpro` channel 0 is a connected Pro Controller: the 0xF0-byte KPADStatus (devType 31, format 22, hold / trig / release in URCC bits mapped from the VPAD bits, sticks, charge and cable 1), r3 1, and VPADRead keeps the touch but reports no buttons or sticks (the GamePad on the table). **Schedule:** FRAME is the 0-based ordinal of the read in the run: the n-th VPADRead (resp. KPADReadEx) that reads input, counted across setups, postsetups and the tested call, gets the line covering n; reads with no line are idle (connected, nothing pressed, sticks centred, no touch). This is the port's own record/replay unit (`crashrec::read(pad)` once per read on a logic pass); the game reads the pad once per frame (sead::TabletDevice::read 0273E3FC, sead::RemoteDevice::read 0273D8EC), and the model has no frame interpolation (every read is a full 30 Hz pass). BUTTONS: `0` / `-`, `0xHEX` (only bits the port's host input produces: mask 0x6FFFE) or names joined by `+` (A B X Y L R ZL ZR PLUS MINUS HOME UP DOWN LEFT RIGHT STICKL STICKR); sticks -1..1 (+y up), touch 0..1 from the top left. The read ordinals and the HLE's previous-read state are run state (setupsnapshot, -fastreset, -snapcache). **Constant answers:** WPADProbe C0008558 (type 253 and -1, or 31 and 0 for channel 0 with `padpro`), VPADBASEGetHeadphoneStatus 0, VPADGetTPCalibratedPoint (raw → 1280x720, the port's tp_to_screen), KPADGetMplsWorkSize 0x5FE0, WPADGetBatteryLevel 4; KPADInitEx, KPADSetMplsWorkarea, WPADEnableURCC / EnableWiiRemote / Disconnect, VPADStopMotor no effect; output-only ones answered and otherwise ignored: VPADControlMotor 0, WPADControlMotor 0, WPADCanSendStreamData 0, WPADSendStreamData / WPADControlSpeaker -1, WENCGetEncodeData 0 (no HLE in the port: `core.cpp` hle_unimplemented). **Left out** (host-side layers the console does not have; identities while their features are off): the port's input mods between the host controller and the buffer (one-screen Minus, `pad_play_rules` — in play the port holds back L unless "L turns the GamePad page" is on —, item ring, touch play, touch talk, code-mod pad filters, jump / moon jump / move speed / fast forward, the story notebook, bag and settings pages, menu sounds, player name), controller switching (`controller_reapply`), the gyro sources, rumble, the trace switches; the VPADControlMotor pattern bytes (read only for the host motor). `-trace` prints `pad VPADRead read #N (chan C, buffer B) hold H -> R` per read and `pad NAME r3=.. -> R` / `(void)` otherwise. Malformed padsched lines, overlapping reads, padsched or `padpro` without `pad`, and a pad import named in another mock directive exit 2. Without `pad` nothing changes (no-change proof: byte-identical output). Self-tests `pad_test.py` (carrier units/_pad_test/pad.txt: rows, the read ordinals through setups and setupsnapshot, the status words, idle, GamePad / Pro Controller modes, spec errors) and `pad_equiv.py` (the model against the port's own vpad / padscore HLE and motion/fusion.cpp at f5554821 compiled with shims: random schedules and call sequences of all 19 imports, r3 and every guest byte compared after each call; `--inject` checks that 32 single-line model bugs are each detected) |
| `osmem ... act` / `osmem ... dynload` (unit file; opt-in, NOT part of `osmem hle`; either or both) | the imports the J3D start-up got generated, input-dependent answers for, answered as the port's HLE at f5554821 answers them (model `src/dynload_model.h`; ). **`act`:** nn::act Initialize C00046E8 → 0, GetSlotNo C0004660 → 1 (`runtime/src/hle/system_stubs.cpp` nn_act section), and the other three the game imports: Finalize C00044E8 → 0, GetPrincipalId C0004640 → 0, GetParentalControlSlotNoEx C0004618 stores the byte 1 at r3 → 0. Only r3 is written (the generic mock wrote a generated r4 too, which flowed into later code). **`dynload`:** OSDynLoad as `hle/coreinit_misc.cpp` (OSDynLoad section) answers it: Acquire C0009A98 (name r3, out r4) drops a trailing ".rpl" (names longer than 4), stores the handle 0x70000000 | (std::hash<std::string>(name) & 0x0FFFFFFF) through r4 (the host libc++'s hash, as the port's build; "erreula" → 75C2CB5D on this Mac) → 0; FindExport C0009AC0 (handle r3, isData r4, name r5, out r6) stores the export's address through r6 → 0: per "module.symbol" key (module "?" for an unknown handle) the first lookup hands out an address, later ones the same: a function gets the next `dispatch::register_host` address (core.cpp: C2000000 + 8n, n from 3 since the boot registers the three default-heap functions first), a data export a zeroed 0x100-byte block from the osmem runtime bump region (the port's runtime_alloc); Release C0009AF8 no effect, SetAllocator C0009B00 → 0. Each also at its RPX fimport stub. **erreula:** a handed-out address is a code target (also for -nulltrap) and a call to it is answered like the port's `hle/erreula.cpp`: the viewer's state (hidden 0 / appearing 1 / visible 2 / disappearing 3), the confirm flag and the HOME-disabled sign; ErrEulaCreate → 1, Destroy / AppearError / DisappearError / Calc / Appear- / DisappearHomeNixSign change the state, the getters answer from it, the rest are constant. A symbol erreula.cpp lacks (or an unknown handle's) gets the port's logging stub: r3 = 0. In the start-up errorCreate 0286C5F8 calls Acquire("erreula.rpl") and 20 FindExports, so the slots 104B6748..104B6794 hold C2000018..C20000B0 in the game's lookup order (AppearError, Calc, Create, Destroy, ...). **Not mirrored:** the port times the fades by the host's steady_clock (appearing / disappearing end after 0.2 s, the automatic confirm 1.0 s later); here time is ErrEulaCalc calls at 30 per second (the game calls Calc once per 30 Hz frame): the 7th Calc after a state change ends a fade, the 31st after `visible` confirms. ErrEulaAppearError's log line (it reads the AppearArg's code and texts) is host output and not read. A function export of a named module other than erreula is a model rejection (the port may answer it from any of its HLE libraries); more than 4096 exports per run too. A call to an address in C2000018.. that was not handed out stays a generic mock (the port would stop: unknown address). The module/export tables, the handed-out list and the erreula state are run state (setupsnapshot, -fastreset, -snapcache). `-trace` prints `act NAME ... -> R`, `dynload NAME ... -> R (new export ADDR)` and `dynload export ADDR module.symbol ... -> R (erreula state S, decided D)`. An act / dynload import or an address in C2000018..C2008017 (the 4096 possible exports) named in another mock directive exits 2. Without the parts nothing changes (no-change proof: byte-identical output). Self-tests `dynload_test.py` (carrier units/_dynload_test/dynload.txt: ErrorViewerTask on original code, the 20 exports and the slots, the 7th / 31st Calc boundaries through setups, GetSlotNo, setupsnapshot, off without the parts, spec errors) and `dynload_equiv.py` (the model against the port's nn_act / OSDynLoad sections, erreula.cpp and core.cpp hle_find compiled with shims: random call sequences, r3, every register, every guest byte and erreula.cpp's statics compared after each call; `--inject`: planted single-line model bugs each detected) |
| `osmem ... dynload swkbd` (unit file; opt-in word, needs `dynload`) | module `swkbd.rpl` through `dynload`, by need: the game's loader 0286CD7C (LOGO phase_0 025AC214 → 026195C4 → 0286CD7C) looks up 35 names at load, all in the port's registry (`runtime/src/hle/swkbd.cpp` at f5554821), so FindExport hands out C2000018.. for each; only the export route (a) calls is answered: SwkbdCreate (slot 104B67B8; swkbd.cpp:151-156): a 0x2000-byte zeroed form buffer from the runtime bump region on the first call, r3 = 1. A call to any other swkbd export is a model rejection at the call: `osmem swkbd: call to NAME (not modelled: by need)`. A symbol the port lacks gets the logging stub. **Not mirrored:** g_need_font / g_need_predict (only rejected exports read them). Without the word a function export of module swkbd stays a rejection at FindExport, as before. `osmem swkbd` without `dynload` exits 2. Tests: `dynload_test.py` part 8 (carrier units/_dynload_test/swkbd.txt), `dynload_equiv.py` (swkbd.cpp compiled in its own unit; both must-reject cases checked; `--inject`) |
| `osmem ... procui` (unit file; opt-in, NOT part of `osmem hle`) | ProcUIProcessMessages C0007880 (also at its stub 028FBB90) → 0, PROCUI_STATUS_IN_FOREGROUND, as `runtime/src/hle/system_stubs.cpp:81` at f5554821 answers it; only r3 is written. By need: without it the import gets a generated answer every frame, so ExitRequested can run the game's exit path in `HDGameFramework::procUI` (`hd_sys_02034B14.cpp:219-224`), the way a run reached SwkbdDestroy at frame round 63. C0007880 named in another mock directive exits 2. Tests: `dynload_test.py` part 9, `dynload_equiv.py` |
| `osmem ... portconst` (unit file; opt-in, NOT part of `osmem hle`) | constant answers as the port's HLE gives them at f5554821, r3 only, also at each import's RPX stub (per-library delta), from the table in `src/portans_model.h`: GX2DrawDone 1 (`gx2/gx2_core.cpp:1363-1368`), nn::ac Connect A0000000 and Initialize 0, nn::olv Initialize A0000000, curl_global_init_mem, NSSLInit, socket_lib_init 0 (`hle/system_stubs.cpp:33,31,54,73,71,69`), IMDisableDim 0, OSEnableHomeButtonMenu 1, OSGetSharedData 0 (`hle/coreinit_misc.cpp:334,332,329`), and nn::olv SetReportTypes, InitializeParam::SetWork and its ctor 0 (no HLE: `core.cpp` hle_unimplemented). Not `nestedret` constants, which leave generated volatile registers. A listed import named in another mock directive exits 2. Not mirrored: GX2DrawDone's host wait, which takes no guest time. Note that nn::ac Connect fails as in the port, so OpMgr_initNetwork stops there. Tests: `portans_test.py` (original code), `portans_equiv.py` (the port's lines compiled with shims, the stub deltas re-measured from build/gen; `--inject`) |
| `osmem ... threads portmem` (unit file; opt-in, NOT part of `osmem hle`, needs `threads`) | argument-derived answers and memory effects as the port's HLE at f5554821 gives them (`src/portans_model.h`), r3 and guest memory only, also at each stub: GX2GPUTimeToCPUTime keeps r3/r4 (`gx2/gx2_core.cpp:1599`); GX2SampleTop/BottomGPUCycle store the clock at r3 when nonzero (`:1600-1601`; timebase::guest_now, here the thread model's clock, as OSGetSystemTime); IMIsDimEnabled stores 0 at r3 when nonzero, r3 = 0 (`hle/coreinit_misc.cpp:336`); GX2SetSurfaceSwizzle replaces byte 2 of +0x34 from r4 (`gx2/gx2_resources.cpp:24-27`); GX2SetClearDepthStencil stores (float)f1 at +0x88 and r4 & 0xFF at +0x8C (`gx2/gx2_core.cpp:1324-1328`); GX2InitDepthBufferHiZEnable no effect (`gx2/gx2_resources.cpp:31`); SAVEOpenFile answers as for a missing save: FS_NOT_FOUND -6, nothing stored (`hle/fs.cpp:452-454`, `open_file` `:235-237`), and a write or update mode is a model rejection (the port would create the file; no /vol/save input is approved). **Not yet:** GX2GetSwapStatus (host flip timing), the GX2Init* struct writers, GX2CopySurface. A listed import in another mock directive exits 2; `portmem` without `threads` exits 2. Tests: `portans_test.py`, `portans_equiv.py` |
| `mocklookup TARGET rIDX BASE COUNT` (unit file) | the generated mock of a read-only word-table lookup (a one-load helper that stays opaque, e.g. 02786348 format → GX2 format) returns in r3 the big-endian word at BASE + 4*rIDX, read from guest memory (the image), at top level and inside `real` callees; an index ≥ COUNT (hex) aborts the input with `mocklookup: index outside the declared table`. Declare the table `stable` too, so mock epochs cannot clobber it. Arguments are still compared. Malformed lines exit 2; `-trace SEED` prints each lookup. 2026-10-24 (AGL#3 request); self-test `mocklookup_test.py` |
| `mockthreadspecific SET GET THREAD [CUR]` (unit file) | the shipping port's thread-specific storage HLE (runtime/src/threads.cpp:67 kSpecific = 0x57C, :763–764) for the one current thread of every input. THREAD is a fixture-owned OSThread block (hex). The generated mock of OSSetThreadSpecific (SET, C000A090; r3 = key, r4 = value) stores r4 big-endian at THREAD+0x57C+4*key after the call's clobber epoch. OSGetThreadSpecific (GET, C0009CC8) returns the word there in r3. The optional CUR (OSGetCurrentThread, C0009BC8) returns THREAD. Each role takes a comma-separated target list, for an import and its stubs. This works at top level, inside `real` callees, and in setups/postsetups. A key ≥ 16 is a model rejection (`mockthreadspecific: thread-specific key outside 0..15`), so an input where both sides make the call aborts on both sides. Arguments and call records are compared and observed as before. The slots THREAD+0x57C..+0x5BB are kept `stable` (no mock epoch clobbers them), and they are plain guest memory, so setupsnapshot, -snapcache, -refcache and the net effect cover them with no special case. `-trace SEED` prints each set/get/current-thread answer. Malformed lines exit 2: missing or extra fields, bad hex, THREAD 0, a second THREAD, or one target in two roles. 2026-10-31; self-test `mockthreadspecific_test.py` |
| `nestedmockfield TARGET` (unit file) | TARGET's `mockfield` lines also apply when TARGET is called inside a `real` callee (they otherwise apply to top-level calls only), e.g. an SDK out-pointer helper reached through real code (GX2CalcColorBufferAuxInfo C00060A0 / GX2CalcDepthBufferHiZInfo C00060B0 writing *r4=0x1000, *r5=0x100 as the shipping port's HLE does). Off unless declared; `-trace SEED` prints each nested write. 2026-10-24 (AGL#17 request); self-test `nestedmockfield_test.py` |
| `mockretain TARGET rN SIZE` (unit file) | the generated mock of TARGET keeps the pointer it gets in rN and, on its next call in the same input, writes SIZE (1..4096) deterministic bytes through the pointer kept from the previous call (a callee that retains a descriptor or callback object across calls). Each side writes through its own kept pointer, so stack objects at different addresses compare. Exposes code that re-reads (or must re-read) such an object after a later call: MAP-dungeon_map#4 02666EA4 re-reads its local SafeString text after the second assureTermination call (2026-10-11). Top-level generated calls only |
| `mockutf16 TARGET rOUT MAX` (unit file) | the mock of a lookup like 025F8A08 fails in 1/4 of calls without writing, otherwise writes 0..MAX nonzero UTF-16 chars plus a terminator through rOUT (nothing else); use instead of an unconditional `rN=outSIZE` fact |
| `real ADDR` nested | a callee listed as `real` also runs for real when another real callee calls it (pure helpers such as PSVECNormalize inside normalizeRS) |
| `tools/verify/callee_facts.tsv` | global `callee` facts applied to every unit that calls the target (unit lines win). Any `rN=...` fact also declares rN an argument, so `rN=0` forces a plain register compare (equal raw values always match, so a scalar that lies in the stack range is not dereferenced; different stack pointers are still compared by their 4-byte pointee; encoded as pointee size 0xFFFE since 2026-10-05) |
| `callee ADDR retu=N` (unit file or callee_facts.tsv) | reviewed return fact: the mock returns an unsigned N-bit value (e.g. `retu=1` for a 0/1 bool the width analysis cannot prove) |
| `matcher_errors.tsv` (also read by mkunit: the GameCube signature of a listed address is not used for its compared registers) | function-map errors found while verifying (address, matcher name, correction, how found) |

### Shared source (`wwhd_src/include`, one owner)

- `wwhd.h`: base types, `STR(addr)`, common opaque classes.
- `f_op/f_op_actor.h`: `fopAc_ac_c` (WWHD layout), `fopEn_enemy_c`.
- `d/d_com_inf_game.h`: the play object (`dComIfGp_get()` + offsets, `PLAY_*`), save info,
  resources (`SafeString`), event manager.
- `d/d_bg_s.h`, `d/d_cc_d.h`: `dBgS_Acch`/`AcchCir`/`ObjAcch`, `dCcD_Stts`/`GObjInf`/`Sph`/`Cyl`/
  `Cps`/`Tri` (sizes unchanged from GameCube) with their out-of-line methods and inline accessors.
- `m_Do/m_Do_ext.h`: `J3DFrameCtrl` (HD: no vtable, reordered), `mDoExt_McaMorf` (HD 0xC8),
  J3DModel helpers.
- `d/d_npc.h`: `fopNpc_npc_c` (+0x11C), `dNpc_JntCtrl_c`, `dNpc_EventCut_c`.
- `bindings.h`: includes all of the above, plus the free-function bindings.

Items are marked `[v]` (used by a verified function) or `[g]` (GameCube signature/offset not yet
exercised; the harness checks it on first use).

### Per-TU copies

GHS keeps one copy of every out-of-line inline function and of every vtable per translation
unit. A call to an inline helper (destructors, `cM3dGCyl` virtuals, `dBgS_LinChk` constructors)
goes to the copy in the caller's own unit, and inline constructors store the unit's own vtable
addresses. Such helpers are bound per actor (or take the addresses as parameters), never shared
by address.

### Conventions for parallel workers

- One clone and branch per worker; it writes only its actors' `wwhd_src/d/actor/<unit>*.cpp`,
  `wwhd_src/include/d/actor/<unit>.h` and `tools/verify/units/<unit>.txt`.
- Shared headers, bindings and the harness have one owner. A worker that needs an addition keeps
  it local (`static inline` bindings in its source; local layouts of shared classes with an `_l`
  suffix) and reports it; the owner moves it into the shared headers when merging.
- Each function: `WWHD_FUNC` + `VERIFY`, at least 10,000 generated inputs passing, steering until
  the coverage column is close to full, then `mutate.py` on a sample.

## Limitations

- **Generated inputs** explore, but rare paths need steering. Block coverage is a weak proxy;
  use `mutate.py`.
  - Some uncovered blocks are infeasible: GHS duplicated dispatch tails, so a tail is only
    reached with a fixed value.
  - Mocks know callees only through types and register facts. A path that needs a callee's
    *real* behaviour (e.g. a search callback that finds something) needs `ret` steering or
    recorded inputs.
- **Recorded inputs** cover only what the session reached:
  - the title screen and the Outset pier for kamome;
  - nothing yet for d_a_mtoge (Forsaken Fortress).
  
  First reads after a call outside F's own frame are taken as initial state. Memory changed
  between two reads by another thread makes a recording unusable (counted, skipped).
- **Global state:**
  - in generated mode every global is random unless steered;
  - in recorded mode it is exactly what the function read.
  - Functions whose behaviour depends on a global being consistent with another (lists,
    heaps) need recorded inputs or `real` helpers.
- **Uninitialised stack temporaries** are zero on both sides (`gabi::Local` zeroes its slot, modelling GHS's one-slot-per-temporary frames). A candidate that forgets to
  initialise a local passed by pointer is only caught if the original wrote a non-zero value.
- **Varargs callees** compare only the arguments the candidate declares (plus the named ones).
- **Not checked:** timing, which thread runs the code, cache/sync instructions, the exact stack
  frame layout, and the order of loads between calls (unobservable).
- **Equivalence is to the recompiled code** (the recompiler's model of Espresso), not to the
  console:
  - `fmadds` is double-then-single rounding;
  - SNaN handling follows host conversions.

## Pilot results

Two actors, one object and one enemy-like NPC. Wall-clock times are for an agent working with
these tools, measured from commits and file timestamps.

### d_a_mtoge: Forsaken Fortress spikes

16 functions verified, each with 10,000 generated inputs. There are no recordings: the spikes
only appear in Forsaken Fortress.
- Not done: four compiler-generated functions in the range (`__sinit`, two deleting destructors,
  an empty virtual).
- Mutation test (`mutate.py d_a_mtoge --n 1000`): 111 mutants compiled, 107 killed. The 4
  survivors are equivalent (each swaps two independent stores), so every non-equivalent mutant
  was killed.

| Category | Count | Functions |
|---|---|---|
| Adopted unchanged apart from layout/bindings | 13 | calcMtx, CreateHeap, CheckCreateHeap¹, actionWait¹, getSwbit, actionHind/Up/Arrival/Down, Draw, IsDelete, Delete, CreateInit, daMtoge_Create |
| Adopted with edits | 2 | `daMtoge_Execute` (HD: function-local action table initialised at runtime), `create` (fopAcM_ct with the HD vtable) |

¹ Not named by the matcher; identified while porting the translation unit.

### d_a_kamome: seagull

All 20 functions of the translation unit verified:
- 10,000 generated inputs each;
- 270 recorded calls each where the function ran (two sessions: title screen and Outset pier);
- `Create`/`Delete` 20 and 8 recorded calls.

| Category | Count | Functions |
|---|---|---|
| Adopted unchanged apart from layout/bindings | 6 | anm_init, kamome_bgcheck, IsDelete, Delete, createHeap, ground_pos_move² |
| Adopted with small edits (fmadds, ftoi, evaluation order, HD null checks) | 5 | search_esa, s_a_i_sub, ko_s_sub, h_s_sub, Create |
| Differs materially in HD | 7 | s_a_d_sub (extra bait-state test), nodeCallBack (HD J3D matrix block + dirty flags), Draw (no blob shadow; USA mbNoDraw), kamome_pos_move (debug-register target), daKamome_setMtx (no mRot; HD joint callback storage), daKamome_Execute (8 inlined functions; Aryll's gull lands only near the ground), kamome_auto_move (smaller wander radius, water check on landing, keep-above-ground timer) |
| Newly decompiled (compiler-generated, no GameCube source) | 2 | `__sinit_d_a_kamome_cpp`, kamomeHIO_c deleting destructor |

² Needs `gabi::ftoi` for float→s16 arguments.

The fork agents estimate that, by statements, about 85% of Execute's GameCube source and about
75% of auto_move's carried over unchanged apart from layout.

Mutation test (`mutate.py d_a_kamome --n 400 --max 400 --rec ../rec1`, a sample including the
inlined helpers): 347 mutants compiled, 296 killed. Of the 51 survivors:

| Kind | Count | Meaning |
|---|---|---|
| Equivalent | ≈ 40 | Swaps of independent stores, zeroing a stack slot that is zero anyway, and a clamp whose `<`/`<=` boundary gives the same value |
| Input gaps | ≈ 11–17 | Mostly rarely reached states of kamome_auto_move (a dropped `anm_init`, a dropped timer store, a literal in a landing state), the radius loop of search_esa and one boundary in setMtx. More `field`/`ret` steering or longer recording sessions would close them |

Bounds: at least 95% of the non-equivalent mutants were killed, 100% in d_a_mtoge.

Effort, wall clock:

| Step | Time |
|---|---|
| Harness from scratch | about 35 min, including reading the project |
| d_a_mtoge port (16 functions) | about 10 min, plus harness fixes it exposed |
| Recording infrastructure + game build + first session | about 15 min |
| kamome's 18 small and medium functions | about 15 min |
| kamome_auto_move (4.7 KB) | 7 min (parallel agent) |
| daKamome_Execute (10.8 KB) | 14 min (parallel agent) |

Per function, after the setup: about 1 min for small functions, and 5–15 min for large ones
with heavy inlining.


NaN payloads through arithmetic: the PowerPC returns the first NaN operand (frA, then frB/frC), quieted. Where such a NaN reaches memory, write the operands in the original order and use `gabi::fadds_ppc` / `fsubs_ppc` / `fmuls_ppc` (host compilers may swap commutative operands).

**Heavy units (machine-wide lock).** `d_a_sail` and `d_a_bwdg` needed ~30-40 GB per run (until 2026-10-06: a few unsteered inputs walked random memory; with forced `this` / `operator new` steering and the compact storage they now need well under 1 GB). `verify.py`, `verify_all.py` and `mutate.py` take an exclusive `flock` on `/tmp/wwhd-verify-heavy.lock` (override with `WWHD_HEAVY_LOCK`) before running them, so runs from different clones and worktrees wait for each other instead of overlapping. Other units are not affected.

**`mockfield TARGET rN+OFF TYPE v1,v2,... | LO HI` (unit file).** A generated mock of TARGET writes a chosen value
into the object its argument rN points to (TYPE u8/s8/u16/s16/u32/s32/f32, big-endian), per call and identically
on both sides, after any `rN=outSIZE` random fill. Use it to build coherent output fixtures (e.g. a damage record
a mocked collision query fills in) without making the callee `real`. Several lines per target are allowed.
Pick lists (here and in `field`/`arg`/`ret` lines) may be up to 16 KB; they used to be cut at 255 characters.

**`mockfield TARGET [rN+OFF]+OFF2 TYPE ...` and `observeptr TARGET rN+OFF SIZE` (unit file; AGL-GX2#3 request,
2026-10-23).** For an argument that points to a descriptor whose word at OFF is itself a pointer (e.g. a callback cursor
`{u32* index, next}`): the bracket form of `mockfield` writes the chosen value through that inner pointer (+OFF2), so a
generated callback can change the caller's loop index; `observeptr` compares, at every call to TARGET, the SIZE bytes
(1..256; e.g. 96 for a va_list's register save area behind r5+8, AGL#2) the inner pointer points to (not the pointer itself; nothing when it is null). Both apply to top-level calls only; for calls made inside `real` callees use `observenested`.

**`observearg TARGET rN+OFF SIZE` (unit file; SEAD#11 request, 2026-10-23).** At every top-level call to TARGET (calls
made inside `real` callees are not recorded, so not observed) the SIZE bytes (1..256) at argument rN + OFF are compared, the argument object itself, no dereference; the raw
argument is still compared as usual. Use it for the input fields of a mixed in/out SDK record (e.g. `observearg
C00060D0 r3+0 32` and `observearg C00060D0 r3+0x30 8` for GX2CalcSurfaceSizeAndAlignment's surface descriptor) and
leave output-only fields out. Overlapping spans for the same target and register are rejected. A difference reports
the first differing byte: `argument span r3+0x0 differs at r3+0x22 (observearg)`.

**Per-call-site `observearg` (2026-10-28, AGL#12 request; self-test `observesite_test.py`).** Where one import is called
at several sites and only some of them pass a buffer whose SIZE bytes are all defined (AGL#12 027ACA30: one
GX2SetVertexUniformReg send passes a full 16-byte uniform, another a buffer where the original keeps its int-to-float
conversion temps), the span can be restricted:

| Form | Applies to |
|---|---|
| `observearg TARGET@N rN+OFF SIZE` | only the N-th top-level call to TARGET in the run (0-based, counting only TARGET's calls). Use it where the call order is fixed; a call that is skipped on some inputs (e.g. a uniform location of -1) shifts the later indices |
| `observearg TARGET rN+OFF SIZE if rK=V` | only calls where the ORIGINAL's rK (r3..r10) is V (decimal or 0x hex), e.g. `if r4=4` (count) or `if r5=0x610201E0` (a heap buffer) |
| `observearg TARGET rN+OFF SIZE if lr=ADDR` | only calls the ORIGINAL makes with return address ADDR (hex), i.e. one call site: the address after its `bl` (after the `bl` to the import thunk for `bl 028FADB8`-style wrappers). Robust against skipped calls; `-trace SEED` prints `observearg site TARGET @N lr=.. r3=.. r4=.. r5=.. r6=..` at every call of a target with a per-site line, for finding the values |

The condition is evaluated on the original's call only; the candidate's call at the same position (calls are compared in
order, and a different target there is already a difference) is then compared byte for byte whatever its own registers
hold. Unlike `final FUNC ADDR SIZE`, which sees the bytes at the function's exit, this compares what the call received,
so a candidate that writes the buffer after the send is caught. Two spans of one target and register overlap (exit 2)
when they can apply to the same call: sites equal or either unrestricted, and conditions that can hold together (only
the same condition register with different values cannot). Every token must parse completely; malformed lines exit 2
(`FILE:LINE: invalid observearg ...`). A difference reads `argument span r5+0x0 differs at r5+0xC (observearg if
lr=027817B8)` (or `(observearg @0)`).

**Pointee-offset spans and nested-call observation (2026-10-30, AGL#3 Mip/D3C request; self-test `nestobs_test.py`).**
Two forms for values an SDK call consumes that the top-level argument compare cannot see:

| Form | Records |
|---|---|
| `observearg TARGET[@N] [rN+OFF]+OFF2 SIZE [if rK=V \| if lr=ADDR]` | at each top-level call to TARGET, the SIZE bytes (1..256) at P + OFF2, where P is the 32-bit word stored at rN + OFF (r3..r10, OFF and OFF2 0..0xFFFF; `[rN]+OFF2` and `[rN+OFF]` are accepted). The pointer P itself is not compared; when rN or P is 0 nothing is read and the record is empty. `@N` and `if` work as for the plain form. Example: `observearg 0274D690 [r3+0x1C]+0x94 8` |
| `observeptr TARGET [rN+OFF]+OFF2 SIZE` | the same span (observeptr's existing form `observeptr TARGET rN+OFF SIZE` is `[rN+OFF]+0`). token `p+0x94` is not accepted: write `observeptr 0274D690 [r3+0x1C]+0x94 8` |
| `observenested TARGET` | every call to TARGET made inside a `real` callee (any depth below the tested function) during the function's run (setups and postsetups are not recorded), in call order, attached to the enclosing top-level call |
| `observenested TARGET SPAN SIZE` | as above, plus SIZE bytes (1..256) at SPAN (`rN+OFF` or `[rN+OFF]+OFF2`) at call entry for each recorded call; several lines per target add spans in line order. Example: `observenested C00063D0 r3+0x74 4` |

`observearg` and `observeptr` spans (plain or pointee-offset) are recorded at top-level calls only: the harness
answers a call made inside a `real` callee before it builds any call record. `observenested` is the route for nested calls and does not observe top-level calls to TARGET
(those already compare as calls). Several `observenested` targets share one ordered sequence per top-level call, so
their interleaving is compared too. Difference rows are ordinary FAIL rows:
`call #0 027B9D00: pointee span [r3+0x1C]+0x94 differs at [r3+0x1C]+0x97 (observearg)`,
`... pointee span [r3+0x1C]+0x94: the base pointer [r3+0x1C] is null on the candidate side only (observearg)`,
`call #0 027B9D00: nested observed calls differ in number: original 3 candidate 2 (observenested; original
[C00063D0(00000002) C00063D0(00000000) C00063D0(00000002)] candidate [C00063D0(00000000) C00063D0(00000002)])`,
`... nested observed call #0 C00063D0 GX2InitColorBufferRegs: span r3+0x74 differs at r3+0x77 (observenested; ...)` or
`... nested observed call #1: original calls X, candidate Y`. Both sequences are printed (12 calls at most, 8 bytes per
span). `-trace SEED` prints `observearg TARGET [rN+OFF]+OFF2 @N: bytes` and `observenested TARGET in call #K: bytes`
(per span a flag byte 01 present / 00 null base, then the bytes). Overlapping pointee spans of the same `[rN+OFF]`
exit 2; malformed lines exit 2. Records are part of -refcache records (the key covers the harness source).

**`mockglobal TARGET HEXADDR TYPE v1,v2,... | LO HI`.**
Like `mockfield`, but the generated mock of TARGET writes the chosen value to a fixed guest address (TYPE u8/u16/u32/f32),
identically on both sides. Models an opaque callback that may replace or clear a global (e.g. a singleton pointer)
without inventing argument registers or changing live-ins.

**`pointee TARGET rN=SIZE`.** Declares that the object rN points
to at a call of TARGET has SIZE bytes, for the undersized-Local check only. Unlike `callee TARGET rN=SIZE` it does
not declare rN an argument, so the pointee bytes are not compared at the call (use it when the callee keeps or fills
the storage but does not read its initial contents).

**Indirect-call live-in check.** At a call to a target the unit does not list (an indirect call through a guest pointer:
`call_ptr` / `bctrl`), the harness also compares the argument registers the resolved callee's code reads
(`build/verify/livein.tsv`, made by `livein_all.py`), not only those the candidate passes (game test: d_s_play omitted
the r4 type argument of an isKindOf vcall). Not compared: r3, the register holding the target or the vtable it came
from, and volatile registers still holding what the previous mocked call left there. The check is probabilistic (the
resolved target of a generated vtable is a random real function). A unit line `nolivein FUNC` skips it in FUNC for a
documented false positive.

**Stack-object size check.** A candidate `gabi::Local` passed (as any argument) to a callee with a known pointee size —
a constructor (`ctor_sizes.tsv`) or a callee with an `rN=SIZE` / `rN=outSIZE` fact — must hold that many bytes; otherwise
the run aborts with "undersized stack object" (game test 2026-10-04: initTextureScroll/Anime 0x1C vs 0x40, setSight 0x18
vs 0x28, dProcTool 12 vs 0x14, sail/buoyflag/bwdg/tapestry 0xF0 vs 0x11C).

**Sharded runs (2026-10-06).** The harness takes `-only ADDR[,ADDR...]`, `-list` (print the unit's candidate
addresses and exit), `-range A:B` (generated inputs A..B-1 only; recorded inputs only when A = 0) and `-shardout`
(one machine-readable `@SHARD` line per function: counts, covered blocks, the index of the first difference).
`verify.py UNIT -shards K` (or `WWHD_SHARDS=K`; HEAVY units default to `WWHD_HEAVY_SLOTS` shards) builds once and
runs K harness processes over the input index ranges `[i*n/K, (i+1)*n/K)`, each holding its own HEAVY slot, then
merges them into the normal output: one line per function, counts summed, coverage united, the first difference
of the lowest input index. Every input is seeded by its index alone (`seed*1000003+i`; the per-function constant
dictionary pre-run is repeated in every shard), so a sharded run prints exactly what an unsharded one prints.
Options that print per-input detail (`-trace`, `-watch`) run unsharded; so does `-v` unless `-shards K` is given
explicitly (2026-10-27): the merge then prints each function's per-input lines shard by shard (generated inputs in index
order, recorded inputs with the first shard) before its row and, with `-v -v`, its uncovered blocks from the united
coverage, as an unsharded run does. A shard must exit 0/1, end its output with `@SHARDEND N STOP` and report every
function the other shards report (2026-10-27, : a shard that ended early, the be<>-guard's exit 86, which now
fails that input instead, lost its unflushed rows and the merge printed the last functions with 3 of 4 shards' inputs); otherwise verify.py
prints `verify: INCOMPLETE sharded run: shard I (inputs A:B) killed by SIG.../exit N after K function row(s)` and no
result rows, and exits 2. The harness flushes stdout after every `@SHARD` line. Self-test `shard_merge_test.py`. Measured (d_a_bwdg): the ~30 GB of a heavy run
is not start-up cost but the per-input peak of one function (021045E8; guest pages cost ~28 KB each in the
harness and malloc keeps the high-water mark), and that function dominates the run time, so shards split its
inputs rather than the function list.

**Unit builds on a RAM disk (2026-10-26).** `WWHD_VERIFY_SCRATCH=DIR` puts each unit's build/output directory
(`unit.c`, objects, the `verify` executable, `glue/`, `spec.expanded.txt`, and mutate.py's mutant files unless
`--scratch` sends them elsewhere) in `DIR/<worktree>-<hash>/<unit>` instead of `build/verify/<unit>`, e.g. on the RAM
disk `$RAMDISK`; verify.py, mutate.py, verify_all.py and readable_check.py all use it (`verify.unit_dir`),
and the cleanups (`verify_all --clean`, readable_check without `--keep`) clear both places. DIR missing or not
writable, or less than 1 GB free (`WWHD_VERIFY_SCRATCH_MIN_FREE=BYTES`): `build/verify/<unit>` as before, one stderr
note. The shared tables (`image.bin`, `livein.tsv`, `frames.tsv`, `_support/`) and the logs callers keep
(`verify_all.log`, readable_check logs) stay in `build/verify`. The directory is a cache: after a reboot the RAM disk is
empty and the next run rebuilds. Self-tests run with the variable unset (some read `build/verify/<unit>`), except
`verify_scratch_test.py`.
The RAM disk (16 GB; RAM is only used as files are written), once per boot:
`diskutil erasevolume HFS+ ramdisk $(hdiutil attach -nomount ram://33554432)` (then set RAMDISK to the mounted volume), then
`mkdir -p $RAMDISK/.fseventsd && touch $RAMDISK/.fseventsd/no_log $RAMDISK/.metadata_never_index`
and `diskutil unmount $RAMDISK && diskutil mount "$(basename "$RAMDISK")"` (the remount makes fseventsd honour `no_log`).
Campaigns then run with `WWHD_VERIFY_SCRATCH=$RAMDISK` and `mutate.py ... --scratch $RAMDISK` (or
`WWHD_MUT_SCRATCH=$RAMDISK`); without the volume both fall back to the build directory with a note.

**Return values (2026-10-05).** `ret_values.py` writes `ret_values.tsv`: every function whose r3 (f1, r3:r4)
some original caller reads after the call (backward liveness over the caller; a callee that cannot write the
register is excluded, as GHS keeps caller values in untouched volatile registers; tail calls inherit their target's
result). For these functions the harness compares the return registers even when the candidate is declared
`void`, and mkunit rejects a `WWHD_FUNC(ADDR, void, ...)` declaration of one (game test: daNpc_Bj1_c::demo
021F563C returns the BOOL its callers test; the void candidate dropped it). A proven false positive takes a unit
line `noret FUNC # reason`. `WWHD_RETLINT=warn` turns the rejection into a warning (triage runs only).
The table records which register the callers read (r3, f1 or r3:r4), and that register is compared at return
whatever the candidate declares: a `u32` candidate of an f1 function has f1 compared (game test: Wind's Requiem,
mDoAud_tact_getBeatFrames 025E1F08). mkunit also rejects a declared return type of the wrong register class
(integer/pointer vs `f32`/`f64` vs `Pair32`/`u64`; a pair declared for an r3 function is accepted); a proven
exception takes `retclass FUNC # reason` (or `noret FUNC # reason`, which also drops the table entry).
Limits: callers that only reach a function through pointers/vtables give no evidence; a result no caller in the
binary uses is not listed.

**Import argument registers (2026-10-05).** `import_args.py` writes `import_args.tsv`: for every import, the argument
registers its runtime HLE implementation reads (`arg(c, i)`, `arg64`, `c->r[N]`, `fa`/`farg`, `c->f[N]`) and, for
C++ SDK imports, those of the mangled signature. mkunit compares these registers at every call of the import, in
addition to what the candidate declares (game test: d_a_grid renderPacket and d_a_pirate_flag called
GX2CallDisplayList without its size argument r4; the KoRL's sail was drawn with wrong colours). The HLE part is a
lower bound (helpers the body calls with `c` are not followed). Re-run `import_args.py` after changing an HLE.

**Stack guard (2026-10-05).** On the candidate side every store into the scratch stack or the caller's back
chain / LR save words (`[entry SP - 0x10000, entry SP + 8)`, which the net-effect comparison excludes) must
fall inside a live `gabi::Local` slot; otherwise the candidate aborts with "store outside any live Local at …
(nearest Local below: … size …)". Checked for the candidate's own stores from the lowest live Local upwards
(below every Local are outgoing frames, built by `do_call` or by hand) and for stores of `real` callees at or
above the caller's SP before the call (below it are the callee frames and linkage words). A byte the original
also wrote for the same input is allowed (generated pointers can point into the stack range). Mocks and
recorded effects are not checked. The static-extent part of the undersized-Local check applies only where the
original makes the same direct call at that position (a virtual call written as `gabi::call` to a loaded
address resolves to a random function in generated mode). Game test 2026-10-05: hd_res_mgr loadFile's inlined path-buffer constructor wrote f+0x193
into a 0x190-byte Local; no callee fact could see it.

**Callee-fact guard (2026-10-05).** `mkunit.py` fails when a unit file's `callee ADDR rN=SIZE` (or
`pointee`) declares a smaller object than the global `callee_facts.tsv` fact for the same ADDR/rN: unit lines
win, and too-small unit sizes had hidden undersized Locals (m_Do_ext 0x1C vs 0x40, d_operate_wind out8 vs
out12). `callee_facts.tsv` also accepts `rN=outN` (output-only storage, as in unit lines).

**Extent lint (2026-10-05).** `extent_lint.py [--all | FILE...]` lists every place the candidate source
passes a `gabi::Local` to a guest callee and compares the Local's size with how far the callee reaches
through that argument register (static: register copies, `addi` offsets, nested direct calls up to depth 4;
update-form loads and indexed accesses make it a lower bound). Hits need a look: the value loaded from a
Local is not its address.
`mkunit.py` stores the same static extent per callee and argument register in the unit's callee table
(`CalleeInfo.ext`); the harness's undersized-Local check uses it like an `rN=SIZE` fact, so a Local
smaller than what the callee's code touches aborts the candidate even when no fact declares the size.

**Host crashes** (a candidate dereferencing a host pointer, …) now print "verify: host signal N while running the
original/candidate side of ADDR" before the process dies.

**Opt-in return kinds (gabi.h).** A candidate whose C++ return type is a 6-byte aggregate
is compared on r3 and the high 16 bits of r4; one returning `gabi::Pair32` (`RET_PAIR32`, d_menu_save checksum
02721AC4) on the full r3 and r4. All other return kinds are unchanged.

## Follow-ups to the null-trap checks

- `-nulltrap` fetch faults cover calls through page-0 targets (either side, since 2026-11-02) and candidate-only calls through other non-code targets. A call through a stale or
  never-written word that still holds a valid code or method-table address (e.g. a dropped `mMethods` store, AGL#3
  02779BFC global 640 / local 116) is not a fault and stays a reviewed survivor.
