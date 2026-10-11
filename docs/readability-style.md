# Readability pass: style guide and acceptance bar

The readability pass turned the verified sources into code that reads like the GameCube decompilation
(zeldaret/tww) without changing a single verified behaviour. Its foundation is the GameCube-style source
support: `tools/verify/autoglue.py`, the `gabi.h` additions and `wwhd_src/include/gc_api/` (see
`tools/verify/README.md`, "GameCube-style source"). The GameCube decompilation is used as a local
reference checkout at `./tww` (git-ignored).

## 1. Reference units (read one before starting)

| Kind | Unit | Source | Functions | Lint before → after (rawcall / rawmem / addr / scaffold) |
|---|---|---|---:|---|
| medium NPC | `d_a_npc_nz` (+ `d_a_npc_nz_playerpos`) | [d_a_npc_nz.cpp](../wwhd_src/d/actor/d_a_npc_nz.cpp), [header](../wwhd_src/include/d/actor/d_a_npc_nz.h) | 65 | 198 / 274 / 73 / 130 → 0 / 0 / 17 / 0 |
| small object | `d_a_obj_lpalm` | [d_a_obj_lpalm.cpp](../wwhd_src/d/actor/d_a_obj_lpalm.cpp), [header](../wwhd_src/include/d/actor/d_a_obj_lpalm.h) | 13 | 48 / 44 / 28 / 26 → 0 / 0 / 4 / 0 |
| HD-only (no tww source) | `hd_text_tag` | [hd_text_tag.cpp](../wwhd_src/d/hd_text_tag.cpp), [d/hd_text_tag.h](../wwhd_src/include/d/hd_text_tag.h) | 10 | 10 / 64 / 11 / 20 → 0 / 0 / 4 / 0 |
| engine (f_pc) | `f_pc_base` | [f_pc_base.cpp](../wwhd_src/f_pc/f_pc_base.cpp), [f_pc/f_pc_base.h](../wwhd_src/include/f_pc/f_pc_base.h) | 12 | 31 / 53 / 21 / 24 → 0 / 0 / 5 / 0 |

All four pass `readable_check` (10k inputs, seeds 1 and 7, coverage equal to the source before the rewrite).
What remains in `addr` is the per-TU data the glue block and `gc_tu` must name by address.

## 2. What MUST NOT change

These are what the verifier keys on, or what the native build depends on. A rewrite that needs to
change one of them is not a readability change.

1. **Behaviour.** Every function computes the same results, memory effects and call sequence (targets,
   argument registers, stack objects) as before, for every input. "Looks equivalent" is not enough: the
   acceptance bar (section 5) proves it.
2. **The function set and the addresses.** Every function keeps its WWHD address: a `/* ADDR */`
   marker on the line before it (GameCube style, autoglue) or `WWHD_FUNC(ADDR, ...)` + `VERIFY(ADDR, ...)`
   (raw style). No function is dropped, merged, split or moved to another unit's source;
   `readable_check` compares the set with the reference per unit spec.
3. **The unit spec files** (`tools/verify/units/<unit>.txt` and companions `units/<unit>/*.txt`):
   steering, callee facts, `real`, `rec`, `stackfill`, `footprint` lines are the inputs of the
   verification. Unchanged by default; a change needs `--allow-spec-change`, a stated reason and a
   review (never weaken: no removed steering, no smaller sizes, no `noret`/`nolivein` without proof).
4. **Signatures as the harness sees them.** Parameter types type the generated inputs and the return
   type selects the compared register: keep widths and signedness (`u8` vs `bool`, `s16`, `f32` vs `f64`,
   `void` vs a used result). A different parameter type shows up as a coverage change in `readable_check`.
5. **Layouts.** Field offsets and sizes stay byte-identical; every layout struct keeps (or gains)
   `WWHD_OFFSET` / `WWHD_SIZE` asserts. Renaming and typing a field is fine; moving it is not.
6. **The semantics that stay visible on purpose:** `fmadds`/`fnmsubs` (GHS fused them), `ftoi` for
   float→int, the operand order of float arithmetic whose NaN payload can reach memory, `f64` where HD
   compares unrounded (`cXyz::abs`), struct copy kind (word copy vs float copy, e.g. `cXyz`/`Quaternion`
   assignment), load/store order relative to calls (clobbering), the per-TU addresses (`gc_tu`, glue
   `str`/`data` lines), `gabi::Local` sizes, `// HD:` differences, the GHS constructor/destructor code.
7. **Tooling semantics.** Harness/`gabi.h`/`autoglue.py`/`mkunit.py` changes are header units with their
   own bar (section 6), never a side effect of a unit rewrite.

## 3. Target style

Follow the reference units. In short:

- **GameCube style through autoglue.** The file ends with a `/* WWHD glue ... */` block; each function
  has only its `/* ADDR */` marker. Class methods instead of `self`-functions, real types instead of
  `u32` addresses, `this->` implicit. Functions GHS inlined into a marked function stay separate
  functions with a `/* inlined into <caller> */` comment on the line before them (autoglue then treats
  their class-typed locals as guest stack slots too).
- **Names.** Where the function exists in tww: tww's class, member, function, enum, local and parameter
  names (`i_this`, `mpBgW`, `fopAcM_SetMtx`, `dRes_INDEX_OYASHI_BDL_OYASHI_e`). Resource/joint enums
  of the archive go in the unit header when `tww/include/res` lacks them. HD-only code (no tww
  counterpart): descriptive names in the same conventions (`mBgMtx`, `bgScaleXZ`,
  `base_process_class__ct`), never addresses (`fn_022A3620`, `field_0x3BC`, `m3BC`).
  Matcher names (`build/names.tsv`) are hypotheses: a call by name goes through gc_api/bindings, where
  the harness checks the signature.
- **Constants.** Enums and named constants instead of magic numbers (process names `fpcNm_*`, event
  commands `dEvtCmd_*`, phase states `cPhs_*`, item numbers, flags). Raw literals stay for real
  tuning numbers (speeds, distances) as tww writes them.
- **Data.** Struct fields instead of offset arithmetic; tables as `static` arrays with a glue `data` line
  (initialisers are checked against the RPX); strings as literals with `str` lines; function pointers
  by name (`setCallBack(nodeCallBack)`), PTMF tables via `gabi::ptmf`.
- **Control flow.** tww's structure: `switch`, early `return`, natural loops. No `goto` ladders where a
  structured form exists; a remaining `goto` needs a one-line reason.
- **Comments.** File header (what the actor is, the verification note), `// HD:` for every behaviour
  that differs from GameCube, one line where GHS forces a non-obvious form (a temporary, an evaluation
  order, a double compare). No comments that repeat the code; no addresses in comments except markers.
- **File layout.** Header comment, includes, `gc_tu`, statics, functions in tww's source order, then a
  section `HD: compiler-generated and template code of this translation unit` (`__sinit`, deleting
  destructors, SafeString copies), then the glue block.
- **Layouts** live in the unit header (`wwhd_src/include/<path>.h`) with the GameCube→WWHD delta in its
  header comment, tww member names and `/* 0xOFF */` columns.

### Writing it (per unit)

1. Read the raw verified source, the tww source of the TU (`tww/src/...`) and the reference units.
2. Optional mechanical first pass: `python3 tools/verify/readable_names.py <src>` names every
   `gabi::call(0xADDR, ...)` through `fn::Name(...)` (`wwhd_src/include/fn_names.h`, generated once for
   all names; the harness sees the same calls, so the unit still verifies). It helps to map calls to
   gc_api names; the finished unit uses gc_api/bindings, not `fn::`.
3. Port the tww source function by function onto the WWHD layout, keeping the HD semantics of the
   verified version (it is the specification: same calls, same order, same HD extras).
4. Missing API: add it to the `gc_api/` header named after the GameCube header (one per GameCube
   header, address and `[v <unit>]` in a comment), or to the shared layout header. Append only; do not
   change existing shared declarations (that is a header unit). Run `objcmp.py` (section 6): every other
   source must stay object-identical.
5. Iterate with `verify.py <unit> -n 1000`, then run `readable_check.sh` (section 5).

### Pitfalls (each was a real failure)

- **Evaluation order.** C++ argument order is unspecified; GHS's is fixed per call site. Where a call
  and a load are both arguments, split them into statements in the original's order. The object of a
  member call is evaluated before its arguments (C++17): `g_env_light.setLightTevColorType(mModel, ...)`
  calls `dKy_getEnvlight()` first, then loads `mModel`, as HD does.
- **Hidden-result temporaries.** `cXyz d = a - b;` constructs in place (`gabi::Into`), but
  `dir = dir.normZP();` goes through a temporary in GHS: write `cXyz norm = dir.normZP(); dir = norm;`.
- **Copies.** `cXyz` and `Quaternion` assignment is a word copy (GHS integer or lfs/stfs pairs keep
  SNaNs); member-wise float assignment quiets them.
- **Double precision.** HD keeps some results in double (`cXyz::abs` → `f64`); rounding to `f32` before a
  compare changes boundary inputs.
- **Unextended results.** GHS passes a callee's `r3` on without re-extending a `u8`/`u16` result; where the
  original does, keep the full register (`u32`) in the caller (hd_text_tag `hdTextConv_c::convert`).
- **Statics.** Function-local statics with a GHS guard: `static f32 x(0.7f);` + `data x ADDR guard GADDR`.
  Constant `.data` scalars: `static s32 t = 0x9130000;` + `data t ADDR` (checked against the image).
- **Strings.** Each use may have its own guest copy (`str ... in FUNC` for duplicates); an assert text
  spelled with tww names on purpose takes `renamed` and a comment with the HD text.
- **Per-TU data.** Inline constructors store the TU's own vtables (`gc_tu`); never borrow another unit's.

## 4. Tools

| Tool | Use |
|---|---|
| `tools/verify/readable_check.sh TARGET...` | the acceptance check (unit name or source file; all specs compiling it). Section 5 |
| `tools/verify/readable_lint.py FILE... \| --all [--tsv]` | leftover counts (goto, raw calls, raw memory, `fn::`, data addresses, numbered fields/functions, hand-written scaffolding, `_l` copies). Counts, never failures |
| `tools/verify/objcmp.py --ref DIR [--same-harness] [--units] [SRC...]` | object-code identity of every unit source against a reference export (`git archive <ref> wwhd_src tools runtime/include`, plus a `build` symlink for glue sources). Section 6 |
| `tools/verify/readable_names.py SRC` | mechanical named-call pass (`fn::`) |
| `tools/verify/autoglue.py SRC` | prints the glued source the harness compiles (debugging) |

## 5. Acceptance bar for a unit

```sh
tools/verify/readable_check.sh <unit-or-source> --ref <commit before the rewrite>
```

PASS requires, for every spec that compiles the unit's sources:

1. the same function set (addresses) as the reference;
2. byte-identical spec files (or an allowed, reviewed change);
3. `verify.py` at **10,000 generated inputs with seed 1 and with seed 7**: every function passes all
   generated and recorded inputs;
4. per function, coverage not below the reference (baseline measured once per reference commit in a
   cached export under `build/readable/`).

A unit whose sources are shared with other specs (companion specs, multi-file units) is checked through
all of them automatically. Optional but recommended for rewrites that restructure control flow: a
`mutate.py <unit> --n 1000 --max 150` sample; non-equivalent survivors point at input gaps.

## 6. Bar for shared headers and tooling

Every change to shared headers (`wwhd_src/include` outside the unit's own header), `gabi.h`,
`autoglue.py` or other harness inputs:

1. `objcmp.py --ref <export of the previous head>` over all unit sources.
   - **Object-identical sources need no re-verification**: the candidate machine code and data are the
     same bytes, and the harness is deterministic per input, so every verdict is unchanged.
   - Sources that **differ or no longer build** are fixed or re-verified with `readable_check` (10k, seeds
     1 and 7) on all their specs.
2. If the harness itself (`gabi.h` runtime code, `harness.cpp`, `mkunit.py`) changes in a way that
   reaches all units (objcmp `--same-harness` isolates header from harness effects): a full
   `verify_all.py -n 1000` pass over all units, plus 10k seeds 1 and 7 on the units whose objects change
   under `--same-harness` (the semantically affected ones). Justification: a 1k pass catches any
   systematic break of the shared code path (build errors, wrong argument packing, frame handling), the
   10k runs give the full bar exactly where behaviour could differ.

## 7. Final verification

After all units were rewritten, every unit was run with `verify_all.py -n 10000` with seed 1 and with
seed 7 against the original code.
