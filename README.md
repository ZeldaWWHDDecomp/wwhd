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

## What is in this repository

| Path | Contents |
|---|---|
| `wwhd_src/` | the decompiled source: 22,846 verified functions in 1,076 source files, plus 298 headers with the WWHD structure layouts and bindings |
| `tools/verify/` | the differential test harness (`verify.py`, `verify_all.py`, `mutate.py`, `src/`, `include/`) and the 1,100 unit specifications (`units/`: which source files form a unit, input steering) |
| `tools/decomp/` | the function-name matcher against zeldaret/tww (the same as in the port) |
| `tools/true60/` | name-based surveys used by the port's 60 fps work (the same as in the port) |
| `docs/` | HD vs. GameCube differences, unit notes, review notes, matcher findings |

The decompilation is a **hybrid with the static recompilation**: a decompiled function calls the
functions it uses through their original addresses (`gabi::call(0xADDR, ...)`), whether those are
decompiled or not. The code therefore does not build into a game on its own; it runs, and is
verified, only together with the recompiled game that the port builds from your own copy.

### Status

| Area | Verified functions |
|---|---:|
| Actors (enemies, NPCs, objects, bosses) | 14,581 |
| Link (player actor) | 1,046 |
| Engine and systems (camera, collision, stages, events, environment, weather, save data, menus, effects, …) | 4,954 |
| HD-only code (GamePad UI, resource and pack loading, input, software keyboard, text layout, Miiverse) | 1,307 |
| Zelda's own sound control | 180 |
| **Game code** | **22,068 of 22,215 (99.3%)** |
| HD UI per-step code outside that count (HUD, GamePad map screens, minigame HUD, message windows, UI parts) | 127 |
| Library units verified along the way (JSystem including JParticle and JStudio, nw::lyt, sead, SDK adapters) | 651 |

The 147 game-code functions left out are small helpers of Nintendo's sead library that the compiler
copied into each unit (string classes, destructors, delegate thunks). Like the ~17,500 functions of the
generic libraries (JSystem, sead, NW4F, the Cafe SDK, the C runtime), they run as recompiled code in
the port and contain nothing specific to Wind Waker.

Added since the first release: the JParticle calculation side (emitter, particle, fields, block getters,
calc visitors), the JStudio timeline and adaptors, the nw::lyt animation frame step (157 library
functions), and 127 per-step functions of the HD UI screens (284 functions in 22 units).

## Disclaimer

This is an unofficial, non-commercial fan project. It is **not affiliated with, endorsed by or
sponsored by Nintendo**. *The Legend of Zelda* and *The Wind Waker* are trademarks of Nintendo; all
trademarks belong to their respective owners and are used here only to name the game this project
is about.

This repository contains **no game code (no machine code, no recompiled code), no assets, no game
data and no keys**. The recompiled original, the RPX image, function names derived from the game and
recorded inputs are all produced locally from your own copy and stay on your machine (they are
git-ignored). You need your own legally obtained copy of the game to build or verify anything.

No commercial use: there are no donations, sponsorships or paid builds of any kind.

## Building and verifying

The harness runs inside a checkout of the port, which provides the recompiler, the runtime headers
and the game-extraction tool. This release was verified against **ZeldaWWHDRecomp commit
`df2cf51bd6d02e0b310d46a5da062693bac7314f`** (v0.2.3). The port's own `tools/verify/` at that commit is
an older copy of the harness; this repository's version replaces it.

Requirements: Python 3, clang/clang++ with C++20, and what the port's README lists for extracting the
game (its "Requirements" section). Building the port itself is only needed for recording (below).

```sh
# 1. the port at the tested commit
git clone https://github.com/ZeldaWWHDRecomp/ZeldaWWHDRecomp port
git -C port checkout df2cf51bd6d02e0b310d46a5da062693bac7314f

# 2. this repository, copied over the port (replaces the port's older tools/verify)
git clone https://github.com/ZeldaWWHDDecomp/wwhd decomp
rm -rf port/tools/verify
cp -R decomp/wwhd_src port/wwhd_src
cp -R decomp/tools/verify port/tools/verify

cd port

# 3. extract your own game into game/ (see the port's README: disc image, disc key, common key)
python3 tools/wudextract.py game.wux extract game        # -> game/code/cking.rpx, ...

# 4. recompile the original game code to C (build/gen, a few seconds)
python3 tools/recomp/recomp.py game/code/cking.rpx build/gen

# 5. verify one unit, or all of them
python3 tools/verify/verify.py d_a_kamome -n 10000 -seed 1
python3 tools/verify/verify.py d_a_kamome -n 10000 -seed 7
python3 tools/verify/verify_all.py -n 10000                    # every unit, in parallel (-j JOBS)
python3 tools/verify/mutate.py d_a_kamome --n 1000 --max 200   # mutation test of one unit
```

The first run of `verify.py` also writes `build/verify/image.bin` (the RPX sections, which the
harness reads for constants and vtables), `build/verify/livein.tsv` and `build/verify/frames.tsv`; all
of it is derived from your game and stays in `build/`. The output is one line per function:

```
addr      function                         generated   recorded coverage  first difference
021E039C  daMtoge_Execute                  10000/10000        0/0      3/3  ok
```

`verify_all.py` over all units is a long, memory-hungry run (a few units need tens of GB; they take a
machine-wide lock, `WWHD_HEAVY_SLOTS`, see `tools/verify/README.md`).

**Function names and GameCube signatures (recommended).** The harness compares the argument
registers each callee reads. It derives them from the code, and adds those of the GameCube signature
when the function has been matched to zeldaret/tww. To get the same checks as the published results,
create the name tables as described in the port's README ("Optional: decompilation tools"):

```sh
git clone https://github.com/zeldaret/tww tww     # built from your own GameCube disc (see the tww README)
python3 tools/decomp/match.py game/code/cking.rpx tww build/names.tsv   # also writes build/wwhd_to_gc.tsv
```

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

While verifying, every unit is compared with the GameCube version. 774 units have been compared;
the differences of all 774 are summarised in [docs/hd-vs-gc-summary.md](docs/hd-vs-gc-summary.md) — new HD
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
projects.

CC0 covers only the contents of this repository. It does not cover the game itself or any Nintendo
data, which are not included here: you need your own legally obtained copy of the game. The port
runtime this repository builds against has its own license (see the port repository).
