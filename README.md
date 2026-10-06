# The Legend of Zelda: The Wind Waker HD — verified decompilation

A decompilation of the Wii U game code of *The Wind Waker HD*, written alongside the native port
([ZeldaWWHDRecomp](https://github.com/ZeldaWWHDRecomp/ZeldaWWHDRecomp)).

Every decompiled function is **verified**, not just written: a test harness runs the new C++
function and the original game function side by side on 10,000 generated inputs (two seeds), and
compares every register, return value and memory write. A unit only counts as done when all of its
functions pass, the inputs reach nearly all of the code (coverage), and a sample of deliberately
broken versions of the code ("mutants") is caught by the tests.

The GameCube decompilation ([zeldaret/tww](https://github.com/zeldaret/tww)) is the starting point
and reference; everything is checked against the HD game itself, so HD changes are found along the
way (see [HD vs. GameCube](docs/hd-vs-gc-summary.md)).

## Status (2026-10-06)

**The game code is decompiled and verified.**

| Area | Verified functions | Status |
|---|---:|---|
| Actors (enemies, NPCs, objects, bosses) | 14,581 | complete |
| Link (player actor) | 1,046 | complete |
| Engine and systems (camera, collision, stages, events, environment, weather, save data, menus, effects, …) | 4,954 | complete |
| HD-only code (GamePad UI, resource and pack loading, input, software keyboard, text layout, Miiverse) | 1,307 | complete |
| Zelda's own sound control | 180 | complete |
| **Game code in total** | **22,068 of 22,215** | **99.3%** |

The remaining 147 functions inside the game code are small helpers of Nintendo's sead library that the
compiler copied into each unit (string classes, destructors, delegate thunks). They are library code
and deliberately left out, like the ~17,500 functions of the generic libraries (JSystem, sead, NW4F,
the Cafe SDK and the C runtime): the port runs those as recompiled code, and they contain nothing
specific to Wind Waker. A few library units that were already verified are kept (494 functions).

**Verified twice.** Besides the per-function test (10,000 generated inputs on two seeds, coverage,
mutation testing), the whole decompiled game is run in place of the original: every decompiled
function is swapped into the running game, and long scripted play sessions (Outset, Windfall,
the dungeons, boss fights, sailing, the file select and saving) are compared step by step with the
original — Link, camera, random numbers, sounds, every game object, the save data and the rendered
picture. This found bugs the function tests could not see (for example grass and bushes drawn with
wrong lighting data, or an insect effect in the Forbidden Woods), which were fixed and then covered
by new checks. With all functions swapped in, the game boots and plays identically to the original.

**Checks tightened (Oct 5–6).** The in-game comparison found two bugs that the function tests had
missed: a Korok in Forest Haven would not start its conversation, and the Wind Waker's baton could not
play a song. Both came from a return value in the wrong register. The test harness now also checks:
- the arguments passed to every Cafe OS / GX2 call (the class behind the King of Red Lions' sail being
  drawn with wrong colours, which the in-game comparison found first);
- that every function returns its result in the register the original game's callers read (whole
  number or floating point), across the whole game;
- that every function uses the original's stack frame layout ("native frames").

With those, the save file written with **all 22,431 game functions swapped in** is byte-identical to
the original's, including the leftover bytes of empty save slots. A regression run of 70 gameplay
scenarios on the result is identical on every channel; the one remaining picture difference (an
invisible, at most 3/255 shading change at the sea horizon) was traced to Nintendo's own code reading
uninitialised stack memory, which the decompiled code doesn't reproduce byte for byte.

**Next:** more and longer gameplay sessions in that comparison (using real save files across the whole
story), then a readable rewrite of the code in the style of the GameCube decompilation.

## HD vs. GameCube

While verifying, every unit is compared with the GameCube version. 670 units have been compared so
far; the differences are summarised in [docs/hd-vs-gc-summary.md](docs/hd-vs-gc-summary.md) — new HD
features (Tingle Bottle, Swift Sail, Picto Box selfies, Hero Mode), gameplay changes, 64 GameCube
bugs fixed in HD, and what was removed (Tingle Tuner, blob shadows, debug code). The full list, one
entry per unit, is in [docs/hd-differences.md](docs/hd-differences.md).

## Legal

This is an unofficial fan project. It is not affiliated with, endorsed by or sponsored by Nintendo.
*The Legend of Zelda* and *The Wind Waker* are trademarks of Nintendo; they are used here only to
name the game this project is about.

This repository contains no game code, no game data, no assets and no keys. The documents here
describe the game in our own words. You need your own legally obtained copy of the game to use the
port.
