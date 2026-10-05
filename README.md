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

## Status (2026-10-05)

| Area | Verified functions | Status |
|---|---:|---|
| Actors (enemies, NPCs, objects, bosses) | 14,441 | ~97% — practically complete |
| Link (player actor) | 1,043 | complete |
| Engine and systems (camera, collision, stages, events, save data, menus, effects, sound control, …) | 3,338 | ~50% — in progress |
| **Game code in total** | **18,775 of 22,215** | **84.5%** |

The executable has 39,713 functions in total; the other ~17,500 are generic libraries (Nintendo's
JSystem, sead, NW4F, the Cafe SDK and the C runtime). They are **not a goal** of this project: the
port runs them as recompiled code, and they contain nothing specific to Wind Waker. A few library
units that were already verified are kept (144 functions).

**In progress:** environment and lighting, weather and rain, resource management, model and animation
helpers, text boxes, the HUD, the file select and save manager, Zelda's own sound control, and a
range of smaller engine pieces. The last actor functions (single leftovers at the ends of units) are
being finished.

**How it is built:** one verified unit at a time, each claimed on a shared board so no two workers
overlap, re-checked by a lead before it is merged.

## HD vs. GameCube

While verifying, every unit is compared with the GameCube version. 670 units have been compared so
far; the differences are summarised in [docs/hd-vs-gc-summary.md](docs/hd-vs-gc-summary.md) — new HD
features (Tingle Bottle, Swift Sail, Picto Box selfies, Hero Mode), gameplay changes, 64 GameCube
bugs fixed in HD, and what was removed (Tingle Tuner, blob shadows, debug code).

## Legal

This is an unofficial fan project. It is not affiliated with, endorsed by or sponsored by Nintendo.
*The Legend of Zelda* and *The Wind Waker* are trademarks of Nintendo; they are used here only to
name the game this project is about.

This repository contains no game code, no game data, no assets and no keys. The documents here
describe the game in our own words. You need your own legally obtained copy of the game to use the
port.
