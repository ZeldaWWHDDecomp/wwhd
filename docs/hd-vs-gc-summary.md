# Wind Waker HD vs. GameCube — summary of the differences

Found while verifying the decompilation: every unit was compared with the GameCube decompilation
(zeldaret/tww). This is a summary in our own words of 670 compared units. Statements marked
"probably" are interpretations; everything else is what the code shows. The full list, one entry
per unit, is in [hd-differences.md](hd-differences.md).

| Category | Entries |
|---|---:|
| Structure (layout, compiler output, code split differently) | 600 |
| Gameplay | 251 |
| Graphics | 238 |
| HD-only (code with no GameCube counterpart) | 72 |
| Fixes (GameCube bugs fixed in HD) | 64 |
| Removed | 45 |

## New in HD

- **Tingle Bottle:** an item slot unused on the GameCube gets a handler with ten message slots, and
  Link has new procedures for throwing the bottle and writing the letter.
- **Swift Sail:** a new item that shares the sail's inventory slot with a second ownership bit; the
  Windfall townspeople treat it as the sail in their dialogue.
- **Picto Box:** Link has a selfie pose with eleven facial expressions chosen with the stick. Lenzo's
  photo quests allow 12 pictures instead of 3, and one picture check was adapted to the new box.
- **First-person view:** Link can walk in it; crouching starts crawling, and a slide floor ends it.
- **Hero Mode:** enemies and objects drop no hearts.
- **GamePad:** a fourth item button in the lock-on (Z-targeting) code; Aryll's telescope scene was
  rewritten for the gyro telescope and her lines depend on the zoom level; the Dragon Roost
  mail-sorting minigame reads the new controller object.
- **Escorts that can't get stuck:** Medli and Makar have a new action that brings them back to Link
  at doors; if an escort cutscene doesn't reach its goal in time, Link is placed at the goal.
- **The sea reaches the horizon:** a ring of flat geometry extends the sea surface far beyond the wave
  grid, for the HD draw distance.
- **Engine:** an additional HD camera object, a crash report when room files are loaded, HD
  statistics counters in the save data (e.g. heart pieces), a reworked actor memory layout, and HD
  overlays (probably the HOME menu and GamePad screens) taken into account when pausing.

## Gameplay changes

- **Doors:** stone doors wait until the next room has loaded before they open; the opening sound now
  plays when the door actually starts moving.
- **Enemies:**
  - Bokoblins no longer counter-attack after being shoved out of a grab. A Bokoblin or Moblin that
    falls far out of its room now counts as defeated (on the GameCube, a room that waits for all of its
    enemies could get stuck).
  - Moblins chase Link before they throw.
  - Magtails that fell out of their room are re-created at home.
  - Morths no longer weigh Link down by toggling a "heavy" state; a counter is used instead.
- **Bosses:**
  - Kalle Demos's core can only be hit while the flower is open.
  - Molgera's larvae are knocked back away from Link.
  - Gohma has a dedicated death animation set.
- **Items:**
  - All-Purpose Bait floats and bobs on water and shrinks away at the end.
  - The Grappling Hook is faster and counts as arrived at a tighter distance.
  - Items from get-item scenes are recorded as collected when the scene ends, not on pickup.
- **Orca's sword training:** his hits count at most once per short window, and the first failed cut
  in a lesson is forgiven.
- **Smaller adjustments:**
  - Tingle's blue brother appears only after the chests on all five Tingle Statue islands are opened.
  - One raised water level in the Tower of the Gods is slightly lower.
  - Several NPCs and a rope are placed slightly differently (probably to fit the HD level geometry).
  - Seagulls wander in a smaller area.
  - Crumbling stairs drop away faster.

## GameCube bugs fixed in HD

- **Missing checks for absent objects:** the most common fix. Many places read an actor, a model or a
  collision triangle that could be missing, for example Link's ledge, climbing and wall code, the
  camera, the collision manager and many enemies and objects.
- **Reads past the end of tables:** now range-checked, e.g. for ChuChu kinds, grass types, bombable
  walls, the King of Hyrule's animations and Puppet Ganon's spider legs (the GameCube read past the leg
  array for the last four legs).
- **Real behaviour bugs:**
  - Boat race: a large time reduction could wrap the time limit around to a huge value, and the
    time-up sound was restarted every frame.
  - Koboli's rupee counter could go negative.
  - A new save file filled 15 of its 16 stage slots with uninitialised memory.
  - A missing cloth object was never freed (a memory leak) for the Forsaken Fortress flag.
  - Medli's monster sound was passed the wrong argument.

## Removed in HD

- **Tingle Tuner (Game Boy Advance link):** removed completely, including the trigger areas, mail,
  Tingle bombs and map icons.
- **Blob shadows:** the simple round shadows under bosses, the ship and stones are gone (HD draws real
  shadows).
- **Debug code:** debug drawing, debug tuning registers and the GameCube DVD-error screens.
- **GameCube memory model:** per-room memory blocks and the preloading of code modules.
- **Old 2D drawing:** minigame overlays and the menu pane animations of the GameCube 2D system
  (probably drawn by the HD layout system instead).

## Graphics (overview)

HD-specific rendering shows up throughout: extra draw lists for the HD lighting passes, texture
matrices that follow the camera on the lava in the Gohma fight, a second face texture animation for
Princess Zelda, GPU vertex buffers instead of GameCube display lists for cloth, ropes and chains, and
replacement textures in a few places.
