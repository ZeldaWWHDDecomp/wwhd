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
| `-trace SEED` | print every access and call of one generated input |
| `-noclobber` | mocked callees never modify memory |
| `-strictnan` | compare NaN payloads bit for bit |

## What "verified" means

For each input, the original and the candidate run from the same state. They must agree on:

1. **The return value**, as the candidate's C++ signature types it: r3 (masked to its width) or f1.
2. **The net memory effect**: every byte whose final value differs from its initial value. The
   scratch stack below the entry SP is excluded; the caller's frame above it is included.
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
- **`WWHD_MEMSTAT=1`** (diagnostic, off by default): stderr lines `memstat-progress` every 1M guest accesses
  and `memstat orig|cand seed=... pages=... arena=... calls=... budget=...` per run, to find inputs that walk
  memory. Such inputs end "aborted on both sides" and are never compared: fix their input domain in the unit
  file (`arg!` / `ret!` / `stable`), as done for d_a_bwdg 021045E8 and d_a_sail 024616AC.

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
  so `<` vs `<=` boundaries are hit.
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
   Each mutant runs the whole unit (helpers can be shared) with a per-run limit (`--timeout`, default
   60 s). A run that hits the limit is reported as `timeout` and not counted as killed: steer the slow
   function first (a loop bound read after a mocked call needs `stable`, or a pinned `this` via `arg!`).
   Only one `mutate.py` per unit at a time: runs share `build/verify/<unit>`.
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
| `verify_all.py [-n 10000] [-rec DIR] [UNIT...]` | re-verifies every unit (parallel); non-zero exit if anything fails. Logs in `build/verify/<unit>/verify_all.log` |
| `tu.py UNIT` / `tu.py --summary UNIT...` | the WWHD functions of a translation unit: address, size, verified (`V`), GameCube source state (`src` decompiled, `stub` "Nonmatching" placeholder: write it from the WWHD code), evidence, names. Unnamed neighbours are marked `?`; names far outside the unit's range are flagged as probable matcher errors |
| `mkbind.py ADDR...` / `mkbind.py --callees ADDR...` | binding drafts from the GameCube signature, checked against the registers the WWHD code reads (`!!` marks disagreements: HD signature change or wrong name). `--callees` lists the unbound callees of functions |
| `sizes.py [REGEX]` | WWHD class sizes from constructors (GHS constructors allocate `sizeof` when `this == NULL`) and the vtable pointers they store |
| `fret ADDR LO HI` (unit file) | range of a callee's float result in generated mocks, edges drawn often. `ret` accepts hex: `ret 025200D4 0x54000000 0x54000000` pins the play object so its fields can be steered by absolute address. Float `field` ranges also draw their edges often (boundary comparisons) |
| `arg FUNC rN\|fN LO HI` / `v1,v2,...` (unit file) | range or pick list of an argument register of the function under test (edges drawn often; 1 input in 8 stays unsteered). `arg!` always applies (e.g. loop counts) |
| `callee ADDR rN=inoutSIZE` (unit file or callee_facts.tsv) | rN points to SIZE bytes the callee reads and updates: compared at the call like `rN=SIZE`, and generated mocks fill them afterwards (like `outSIZE`). A global `out`/`inout` fill also applies when a unit line for the same key declares only a size (tev-output facts 2026-10-05: the light state dKy_tevstr_c stayed zero under mocks, hiding wrong-offset reads) |
| `ret ADDR v1,v2,...` (unit file) | pick list for a callee's r3 result |
| `field FUNC WHERE TYPE v1,v2,...` (unit file) | pick list: the field takes one of these values (single flag bits, enum values with gaps) |
| `callee ADDR rN=outSIZE` (unit file) | rN points to output-only storage of SIZE bytes: its contents before the call are not compared, and generated mocks fill it (e.g. `MtxPosition` `r4=out12`, `cXyz::operator-` `r4=out12`). `rN=255` (constructor storage) is neither compared nor filled |
| `stackfill` (unit file) | the scratch stack starts with deterministic random bytes instead of zeros (both sides; `gabi::Local` slots start with the same bytes): exposes stores the original makes to stack objects that a zero default would hide |
| `stackfill periodic` (unit file) | like `stackfill`, but the byte depends only on the address modulo 4: stack objects the two sides place at different, equally aligned addresses start with the same bytes (padding the code never writes) |
| `footprint FUNC SIZE` (unit file) + `gabi::NativeFrame<SIZE>` | compare FUNC's own stack footprint too: the SIZE bytes below the entry SP and the back chain/LR save above it (normally excluded). Use with `stackfill` (non-periodic), so bytes neither side writes compare equal. Needed where stale stack bytes escape: a function that copies uninitialised parts of its stack objects out (d_save initdata_to_card: padding of the init objects goes into the empty save slots), and the functions that run before it at the same depth (d_file_select fsCreate), whose frames leave those bytes. Such a candidate declares `gabi::NativeFrame<SIZE> f` (SIZE = the original's `stwu r1,-SIZE`): r1 is lowered so callees run at the original's sp, the area is NOT initialised (a `gabi::Local` is: zero/stackfill in the harness, zero in the game), objects are addressed `f.sp() + offset` as in the disassembly, and the frame's own stores are written like the original's (back chain at sp, saved registers from `cpu->r[n]`, LR at `f.entry() + 4`, paired-single temporaries). With `footprint`, generated inputs also give r14..r31 distinct values (so saved-register stores are checked), and every callee must start with the original's r1 (`footprint FUNC 0` for a function without a frame: a tail-calling dispatcher must use `gabi::tail_ptr`, not `call_ptr`, which runs the target 16 bytes lower). Game test 2026-10-05: cking.sav differed in 69 bytes of the empty slots. |
| `framexact FUNC RET` (unit file) + `gabi::call_site` / `gabi::call_ptr_site` | cross-build save states (2026-10-06): FUNC's call that returns to RET (the original's LR there) is on the guest stack when a save state is taken at the frame boundary (main loop: hd_main 020061C4, Framework_initialize 02034FD8, Framework_procFrame 02035090). At that call the harness compares LR, r14..r31, f14..f31 (ps0 and ps1) and FUNC's whole automatic frame incl. back chain, saved-register slots and its LR save word; at FUNC's exit r14..r31 and f14..f31. Generated inputs give r14..r31 and f14..f31 distinct values. The candidate writes that call as `gabi::call_site(RET, {{reg, value}...}, {{freg, ps0, ps1}...}, ADDR, args...)` (`call_ptr_site` for an indirect call): LR = RET and the callee-saved registers the original holds at that call; the enclosing automatic frame then reloads its saved registers on return like the original's epilogue (`AutoFrame` reload, only in functions that use `call_site`). A value needed after the call is taken back from its register (procFrame: `self = gabi::cpu->r[30]`), so a state load gives the snapshot's value. |
| `final FUNC ADDR SIZE` (unit file) | postcondition: SIZE bytes at ADDR are compared at the exit of the tested function FUNC, whoever wrote them (e.g. a field saved, clobbered by a mocked call, and restored) |
| `alloc TARGET rSIZE BASE [ALIGN]` (unit file) | the allocator mock returns distinct aligned blocks from BASE in call order (size from rSIZE, default alignment 32), identical on both sides |
| `allocfixed TARGET SIZE BASE [ALIGN]` (unit file) | like `alloc` for a factory whose request size is fixed (e.g. `allocfixed 025E38E0 0x144 0x29000000 32` for the J3DModel factory) |
| `nestedret TARGET VALUE` (unit file) | inside a `real` callee, a nested call to TARGET returns VALUE in r3 instead of a random answer (e.g. a TLS heap getter) |
| `mockfetchsize TARGET` / `mockfetchinit TARGET` (unit file; also nested inside `real` callees) | GX2 fetch-shader imports: size = runtime HLE formula of the stream count (r3); init writes 32 descriptor bytes (r3) and 16+n*16 program bytes (r4), n = r5 ≤ 16, derived from the n*32 stream bytes at r6 |
| `ret! ADDR …` / `fret! ADDR LO HI` (unit file) | like `ret`/`fret`, but always applied (no 1-in-8 escape) and also to nested calls inside `real` callees |
| outgoing linkage (always) | every guest call gets a 16-byte outgoing frame (back chain + LR save word), so a real callee's LR store at caller SP+4 never hits a `gabi::Local` payload |
| `rec DIR` (unit file) | local recordings (`DIR/ADDR/*.tap`, relative to the repository root; not distributed, skipped with a note when the directory is missing) replayed on every `verify.py` / `verify_all.py` / `mutate.py` run of the unit; an explicit `-rec` / `--rec` takes precedence. Game-test regressions (e.g. d_grass Grass_ShaderBuild 2026-10-05) stay covered this way |
| `stable ADDR SIZE` (unit file) | a steered region (e.g. stage path data at a pinned address) that callees never write: excluded from clobbering |
| `fieldstr FUNC\|* ADDR\|rN+OFF [SIZE] "s1","s2",...\|@stages` (unit file) | one whole NUL-terminated string per input, the same on both sides (e.g. the play object's start stage name at play+0x5134). `@stages` adds the real stage list `tools/verify/stage_names.txt` (the `<name>_Stage.szs` archives); strings and `@stages` can be mixed, repeat a string to weight it; `\xNN` escapes a byte; each `@keep` pick leaves the bytes to the generator and earlier `field` picks of the same bytes (keep their targeted values in the mix). About 1 input in 16 gets the empty string and 1 in 16 keeps the generated bytes. For an absolute ADDR the SIZE bytes (default 8) are also `stable`. Use it wherever code compares a name with literals (strcmp/strncmp loops, inline SafeString compares): per-byte `field` picks almost never form a real prefix (the d_grass `kin` loop bug of 2026-10-05 went unnoticed that way) |
| `mockmove TARGET rDST rSRC rSIZE` (unit file) | (also when called from inside a `real` callee) the generated mock of TARGET (e.g. OSBlockMove C0009988) really copies SIZE bytes from SRC to DST (overlap-safe, source read before the clobber epoch) and returns DST |
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
Options that print per-input detail (`-v`, `-trace`) run unsharded. Measured (d_a_bwdg): the ~30 GB of a heavy run
is not start-up cost but the per-input peak of one function (021045E8; guest pages cost ~28 KB each in the
harness and malloc keeps the high-water mark), and that function dominates the run time, so shards split its
inputs rather than the function list.

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
