# The Legend of Zelda: The Wind Waker HD — verified decompilation

A decompilation of the game code of *The Wind Waker HD* (Wii U, USA, version 0, `cking.rpx`),
written function by function alongside the native port
[ZeldaWWHDRecomp](https://github.com/ZeldaWWHDRecomp/ZeldaWWHDRecomp).

Every decompiled function is **verified**, not just written. A test harness runs the new C++
function and the original game function (recompiled from your own copy of the game) side by side
and compares the return value, every memory write and every call the function makes:

- **10,000 generated inputs** per function, on two seeds;
- **coverage**: the inputs must reach nearly all basic blocks of the original;
- **mutation testing**: deliberately broken versions of the code must be caught;
- **in-game swap tests**: decompiled functions are swapped into the running game, and scripted play
  sessions are compared step by step with the original (Link, camera, random numbers, sounds, game
  objects, save data, the rendered picture).

The GameCube decompilation [zeldaret/tww](https://github.com/zeldaret/tww) is the reference for
names and structure; everything is checked against the HD game itself, so HD changes are found along
the way (see [HD vs. GameCube](#hd-vs-gamecube)).

The source is written in the readable style of zeldaret/tww: real classes and members, named fields
and constants, structured control flow and descriptive names (see
[docs/readability-style.md](docs/readability-style.md)).

## What is in this repository

| Path | Contents |
|---|---|
| `wwhd_src/` | the decompiled source: 38,828 verified functions in 2,252 source files, plus 1,925 headers with the WWHD classes, structure layouts and bindings |
| `tools/verify/` | the differential test harness (`verify.py`, `verify_all.py`, `mutate.py`, `src/`, `include/`), the readability checks (`readable_check.py`, `readable_lint.py`), the model-data extraction (`assets.py`, `assets.txt`), the mutation-testing tools (`mutate.py`, `cascade.py`, `triage.py`, `receipt_audit.py`, `nulltrap_compare.py`, caches), their self-tests (`*_test.py`, `*_equiv.py`) and the 3,486 unit specifications (`units/`: which source files form a unit, input steering, companion specs) |
| `tools/decomp/` | the function-name matcher against zeldaret/tww (the same as in the port) |
| `tools/true60/` | name-based surveys used by the port's 60 fps work (the same as in the port) |
| `docs/` | HD vs. GameCube differences, unit notes, review notes, matcher findings |

The decompilation is a **hybrid with the static recompilation**: a decompiled function calls the
functions it uses through their original addresses (`gabi::call(0xADDR, ...)`), whether those are
decompiled or not. The code therefore does not build into a game on its own; it runs, and is
verified, only together with the recompiled game that the port builds from your own copy.

### Status

The game has 39,713 functions (`cking.rpx`, USA). **38,828 of them are decompiled and verified**
(counted as the distinct function addresses marked in `wwhd_src/`, grouped by source directory):

| Area | Verified functions |
|---|---:|
| Actors (enemies, NPCs, objects, bosses) | 14,575 |
| Link (player actor) | 1,046 |
| Engine and systems (camera, collision, stages, events, environment, weather, save data, menus, effects, …) | 5,274 |
| HD-only code (resource and pack loading, input, software keyboard, text layout, and the HD UI: sea chart and dungeon map, pause menus, HUD, GamePad screens, Picto Box, Miiverse, title and ending, minigames) | 6,843 |
| Zelda's own sound control | 180 |
| Libraries (JSystem, sead, agl, NintendoWare for Cafe sound, layout and utilities, libjpeg, the message library, SDK math and GFD, compiler-generated helpers) | 10,910 |

- **The game code is complete** apart from 6 functions: the code that comes from the GameCube
  version and the HD-only code alike.
- **The HD UI is decompiled:** the sea chart and dungeon map, pause menus, HUD, GamePad screens,
  Picto Box, Miiverse, title and ending, the minigames and the UI parts framework they share (about
  5,400 functions) are verified like the rest.
- **The libraries are decompiled too** (see "Library work" below).
- **Not decompiled:** 885 functions: the NintendoWare font engine `nw::font` (481), the Green Hills C
  and C++ runtime (317) and the static wrappers of the system applets and services (error viewer,
  software keyboard, SCI, account; 81), which the port replaces with host code, and 6 functions in
  the game code. They run as recompiled code.

**Readability pass (October 2026).** The whole decompilation was rewritten from a form close to the
machine code into the tww style. Every rewritten unit was checked against the version before it
(same results on 10,000 inputs on two seeds, coverage not lower), and at the end the complete tree
was verified once more: all 1,100 unit specifications, 29,929 function runs per seed on seeds 1 and
7, all passing, with coverage identical to the version before the rewrite.
The HD UI units were written in that style from the start and then had a second readability pass
(named bindings for raw addresses, members for offset arithmetic, typed locals for stack slots); every
changed unit was checked the same way, with identical per-function results.

**Library work (October–November 2026).** The libraries linked into the game were decompiled and
verified with the same harness, in two stages:

- Stage 1: JSystem (J2D, J3D, JAudio, JKernel, JMath, JMessage, JParticle, JStage, JStudio, JSupport,
  JUtility, JGadget, JFramework), the game's libjpeg build (IJG libjpeg 8c; see [License](#license)),
  the SDK matrix library and the GFD shader-file reader, and compiler-generated destructors and helpers.
- Stage 2: sead (heaps, containers, strings, files, resources, threads, tasks and the framework), agl
  (render targets, shaders, post effects, lighting and shadows, the GX2 utilities), NintendoWare for
  Cafe sound (`nw::snd`), layout (`nw::lyt`) and utilities (`nw::ut`), the message library (LMS), the
  HD model pipeline that replaces the GameCube J3D renderer, and the bridge between the GameCube
  JAudio sound requests and the HD sound system.
- The text engine `nw::font` is not decompiled: the port replaces it.

The library units use the same checks as the game units (10,000 inputs on two seeds, coverage,
mutation testing). Their unit specifications add start-up profiles (an original start-up of the
library objects, then the tested function), directives for the original's OS and graphics calls, and
mutation receipts with an audit tool; `tools/verify/README.md` describes each directive.

## Disclaimer

This is an unofficial, non-commercial fan project. It is **not affiliated with, endorsed by or
sponsored by Nintendo**. *The Legend of Zelda* and *The Wind Waker* are trademarks of Nintendo; all
trademarks belong to their respective owners and are used here only to name the game this project
is about.

This repository contains **no game code (no machine code, no recompiled code), no game assets
(models, textures, sounds, text) and no keys**. Like zeldaret/tww, the source contains the small data
tables the code itself uses (for example math tables and animation or parameter tables); model data
that the renderer consumes (vertex positions, texture coordinates, display lists) is not included but
generated from your own copy at build time (`tools/verify/assets.py`, the same approach as tww's
asset extraction). The recompiled original, the RPX image, the generated model data, function names
derived from the game and recorded inputs are all produced locally from your own copy and stay on
your machine (they are git-ignored). You need your own legally obtained copy of the game to build
or verify anything.

No commercial use: there are no donations, sponsorships or paid builds of any kind.

## Building and verifying

The harness runs inside a checkout of the port, which provides the recompiler, the runtime headers
and the game-extraction tool. This release was verified against **ZeldaWWHDRecomp commit
`264f53ee0eca9d52d39f6ccf1a890eecec018e6e`** (v0.5.0). The port's own `tools/verify/` at that commit is
an older copy of the harness; this repository's version replaces it.

Requirements: Python 3, clang/clang++ with C++20, and what the port's README lists for extracting the
game (its "Requirements" section). Building the port itself is only needed for recording (below).

```sh
# 1. the port at the tested commit
git clone https://github.com/ZeldaWWHDRecomp/ZeldaWWHDRecomp port
git -C port checkout 264f53ee0eca9d52d39f6ccf1a890eecec018e6e

# 2. this repository, copied over the port (replaces the port's older tools/verify)
git clone https://github.com/ZeldaWWHDDecomp/wwhd decomp
rm -rf port/tools/verify
cp -R decomp/wwhd_src port/wwhd_src
cp -R decomp/tools/verify port/tools/verify

cd port

# 3. extract your own game into game/ (see the port's README: disc image, disc key, common key)
python3 tools/wudextract.py game.wux extract game        # -> game/code/cking.rpx, ...

# 4. recompile the original game code to C (build/gen, under a minute) the way the harness compares
#    against it: the plain form (WWHD_RECOMP_PLAIN=1), the PowerPC NaN rules, and without the port's
#    own game patches (its hook lists), from a copy of the recompiler
cp -R tools/recomp tools/recomp_verify && rm tools/recomp_verify/hooks*.txt && touch tools/recomp_verify/hooks.txt
WWHD_RECOMP_PLAIN=1 python3 tools/recomp_verify/recomp.py game/code/cking.rpx build/gen --ppc-nan-propagation

# 5. function names and GameCube signatures (build/names.tsv, build/wwhd_to_gc.tsv; needed by the
#    harness), from the GameCube decompilation built from your own GameCube disc (see the tww README)
git clone https://github.com/zeldaret/tww tww
python3 tools/decomp/match.py game/code/cking.rpx tww build/names.tsv

# 6. verify one unit, or all of them
python3 tools/verify/verify.py d_a_kamome -n 10000 -seed 1
python3 tools/verify/verify.py d_a_kamome -n 10000 -seed 7
python3 tools/verify/verify_all.py -n 10000                    # every unit, in parallel (-j JOBS)
python3 tools/verify/mutate.py d_a_kamome --n 1000 --max 200   # mutation test of one unit
```

The first run of `verify.py` also writes `build/verify/image.bin` (the RPX sections, which the
harness reads for constants and vtables), `build/verify/livein.tsv`, `build/verify/frames.tsv` and the
generated model-data headers in `build/verify/include/assets/` (from `tools/verify/assets.txt`); all of
it is derived from your game and stays in `build/`. The output is one line per function:

```
addr      function                         generated   recorded coverage  first difference
021E039C  daMtoge_Execute                  10000/10000        0/0      3/3  ok
```

`verify_all.py` over all units is a long, memory-hungry run (a few units need tens of GB; they take a
machine-wide lock, `WWHD_HEAVY_SLOTS`, see `tools/verify/README.md`).

**Function names and GameCube signatures (step 5).** The harness compares the argument registers each
callee reads. It derives them from the code, and adds those of the GameCube signature when the
function has been matched to zeldaret/tww; it also reads function names from `build/names.tsv` and
stops with a note when the file is missing.

`tools/verify/README.md` describes the method in detail: what "verified" means, the memory model, the
unit-file directives, and the rules the source follows.

### Recorded inputs (optional, regenerated locally)

Besides generated inputs, a function can be checked on calls recorded in the running game. Recordings
(`*.tap`) contain game memory, so none are distributed; you make your own from your copy of the game
with the port's recording taps (`runtime/src/verify_tap.cpp`):

```sh
python3 tools/verify/mktap.py --unit d_grass      # instruments every VERIFY'd function of the unit
                                                  # (writes tools/recomp/hooks_tap.txt, build/gen/code_tap.c)
cmake -S . -B build/cmake && make -C build/cmake -j4 wwhd          # build the port (see its README)
WWHD_TAP=$PWD/tools/verify/recordings/d_grass ./build/cmake/wwhd   # play the scenes you want recorded
python3 tools/verify/verify.py d_grass -n 10000   # the unit's `rec` line picks the recordings up
python3 tools/verify/mktap.py --clean             # back to the normal build
```

`WWHD_TAP_N`, `WWHD_TAP_EVERY` and `WWHD_TAP_AFTER` limit how many calls are written. Any directory
works with `-rec DIR` (`DIR/ADDR/*.tap`). Two unit files (`d_grass`, `d_wood`) name a recording
directory under `tools/verify/recordings/`; when it is missing, `verify.py` prints a note and runs the
generated inputs only. A recording is used only after the original's replay reproduces it exactly.

The in-game swap tests described above used a build of the port with the decompiled functions linked
in place of the originals; that build setup is not part of this release.

## HD vs. GameCube

While verifying, every unit is compared with the GameCube version. 838 units have been compared;
the differences of all 838 are summarised in [docs/hd-vs-gc-summary.md](docs/hd-vs-gc-summary.md) — new HD
features (Tingle Bottle, Swift Sail, Picto Box selfies, Hero Mode), gameplay changes, 64 GameCube
bugs fixed in HD, and what was removed (Tingle Tuner, blob shadows, debug code). The full list, one
entry per unit, is in [docs/hd-differences.md](docs/hd-differences.md); a few longer unit write-ups
are in [docs/unit-notes/](docs/unit-notes/).

Other documents: [docs/review-notes.md](docs/review-notes.md) (behaviour that is explained but not
fully proven, original-engine undefined behaviour, harness limits) and
[docs/decomp-notes.md](docs/decomp-notes.md) (how WWHD functions are matched to the GameCube
decompilation).

## License

The decompiled source code, the verification tools and the documentation in this repository are
released under [CC0 1.0 Universal](LICENSE) (public domain dedication), like the GameCube
decompilation of The Wind Waker ([zeldaret/tww](https://github.com/zeldaret/tww)) and other zeldaret
projects, with one exception: the libjpeg files (below).

zeldaret/tww is the reference for names and structure throughout; the JSystem units also used the
Twilight Princess decompilation ([zeldaret/tp](https://github.com/zeldaret/tp)). Both are CC0.

**libjpeg (Independent JPEG Group).** This software is based in part on the work of the Independent
JPEG Group. The files in `wwhd_src/libjpeg/` and `wwhd_src/include/libjpeg/` (except `SdkJpeg.cpp` and
`SdkJpeg.h`, the game's own wrapper) are a decompilation of the IJG libjpeg 8c build linked into the
game. They are not CC0: they remain under the IJG license, keep the IJG copyright notices of their
files, and come with the IJG README, included unaltered as
[`wwhd_src/libjpeg/README.ijg`](wwhd_src/libjpeg/README.ijg), which states the copyright, the
no-warranty notice and the conditions of use. Changes to the original IJG files: every file was
rewritten in C++ from the game binary; the WWHD build differs from IJG 8c where the binary does
(marked `// HD:`); data is stored big-endian in guest memory, and memory comes from the Cafe allocator.

CC0 covers only the contents of this repository. It does not cover the game itself or any Nintendo
data, which are not included here: you need your own legally obtained copy of the game. The port
runtime this repository builds against has its own license (see the port repository).
