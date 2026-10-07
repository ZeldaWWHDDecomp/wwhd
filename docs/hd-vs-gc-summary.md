# Wind Waker HD vs. GameCube — summary of the differences

Found while verifying the decompilation: every unit was compared with the GameCube decompilation
(zeldaret/tww). This is a summary in our own words of all 774 compared units (snapshot 2026-10-07).
Statements marked "probably" are interpretations; everything else is what the code shows. The full
list, one entry per unit, is in [hd-differences.md](hd-differences.md).

| Category | Entries |
|---|---:|
| Structure (layout, compiler output, code split differently) | 694 |
| Gameplay | 271 |
| Graphics | 264 |
| HD-only (code with no GameCube counterpart) | 160 |
| Fixes (GameCube bugs fixed in HD) | 65 |
| Removed | 46 |

## New in HD

- **Tingle Bottle:** an item slot unused on the GameCube gets a handler with ten message slots, and
  Link has new procedures for throwing the bottle and writing the letter. Miiverse posts are
  downloaded every few minutes into floating bottles at island spots or on the sea near Link.
- **Swift Sail:** a new item that shares the sail's inventory slot with a second ownership bit; the
  Windfall townspeople treat it as the sail in their dialogue.
- **Picto Box:** Link has a selfie pose with eleven facial expressions chosen with the stick. Lenzo's
  photo quests allow 12 pictures instead of 3, and one picture check was adapted to the new box.
- **First-person view:** Link can walk in it; crouching starts crawling, and a slide floor ends it.
- **Hero Mode:** enemies and objects drop no hearts.
- **GamePad:** a fourth item button in the lock-on (Z-targeting) code; Aryll's telescope scene was
  rewritten for the gyro telescope and her lines depend on the zoom level; the Dragon Roost
  mail-sorting minigame reads the new controller object. A touch UI framework, the system keyboard
  for names, and GamePad rumble were added.
- **Escorts that can't get stuck:** Medli and Makar have a new action that brings them back to Link
  at doors; if an escort cutscene doesn't reach its goal in time, Link is placed at the goal.
- **The sea reaches the horizon:** a ring of flat geometry extends the sea surface far beyond the wave
  grid, for the HD draw distance. The distant island models also stay visible 100,000 units further
  out than on the GameCube.
- **New screens:** the auction shows a rupee meter and a rolling three-digit bid counter; the arrow
  icon shows the arrow type with an element particle.
- **Text:** per-language message files with inline tags replace the GameCube message archives;
  counters get singular/plural labels, and button symbols probably follow the controller in use.
- **Engine:** an additional HD camera object, a crash report when room files are loaded, HD
  statistics counters in the save data (e.g. heart pieces), a reworked actor memory layout, and HD
  overlays (probably the HOME menu and GamePad screens) taken into account when pausing. The whole
  Wii U system layer is new: tasks, a threaded resource manager, an error viewer, HOME menu, SpotPass,
  and a sound layer that plays the GameCube sound engine's requests with separate TV and GamePad volumes.

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
  - Hit handling has an extra damage step for one of Link's cut types under an HD-only condition
    (purpose not established).
- **Bosses:**
  - Kalle Demos's core can only be hit while the flower is open.
  - Molgera's larvae are knocked back away from Link.
  - Gohma has a dedicated death animation set.
- **Items:**
  - All-Purpose Bait floats and bobs on water and shrinks away at the end.
  - The Grappling Hook is faster and counts as arrived at a tighter distance.
  - Items from get-item scenes are recorded as collected when the scene ends, not on pickup.
  - Conducting with the Wind Waker no longer reads the control stick for melodies; they are judged
    from the beats only.
- **Sound:** a limiter stops duplicate copies of the same sound effect; the low-health alarm can be
  switched off; variants of a background track switch their muted parts instead of restarting the
  track; a few sounds get a new distance curve.
- **Orca's sword training:** his hits count at most once per short window, and the first failed cut
  in a lesson is forgiven.
- **Smaller adjustments:**
  - Tingle's blue brother appears only after the chests on all five Tingle Statue islands are opened.
  - One raised water level in the Tower of the Gods is slightly lower.
  - Several NPCs and a rope are placed slightly differently (probably to fit the HD level geometry).
  - Seagulls wander in a smaller area.
  - Crumbling stairs drop away faster.
  - The poison mist uses different particle counts and sizes in some rooms and a softer edge.
  - Dragon Roost Island gets a second distant model until a story event.

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
  - The poison mist wrapped particles on the wrong axis at one edge.

## Removed in HD

- **Tingle Tuner (Game Boy Advance link):** removed completely, including the trigger areas, mail,
  Tingle bombs and map icons.
- **Blob shadows:** the simple round shadows under bosses, the ship, the flying boomerang and stones
  are gone (HD draws real shadows).
- **Debug code:** debug drawing, debug tuning registers and the GameCube DVD-error screens.
- **GameCube memory model:** per-room memory blocks and the preloading of code modules; the distant
  island models no longer load their own archive into a private heap.
- **Old 2D drawing:** minigame overlays, the menu pane animations of the GameCube 2D system and the
  telescope screen (probably drawn by the HD layout system instead).

## Graphics (overview)

HD-specific rendering shows up throughout: extra draw lists for the HD lighting passes, texture
matrices that follow the camera on the lava in the Gohma fight, a second face texture animation for
Princess Zelda, GPU vertex buffers instead of GameCube display lists for cloth, ropes, chains, trees,
the boomerang's trail and every weather effect (rain, snow, ash, mist, clouds, sun, moon and lens
flare, whose geometry also changed), and replacement textures in a few places. Distant islands
switch on and off at once instead of fading.

Weather in detail: the HD weather packets own their GPU geometry, textures and shaders and release
them in their destructors (the GameCube packet destructors are empty). The sky clouds always render in
a single pass with three texture layers; the GameCube pass that depended on sun visibility or on
aiming the Picto Box is never selected. The lens flare is sixteen triangular rays plus nine textured
quads instead of eight quads plus a sixteen-part fan, and the sun and moon are expanded quads in
double buffers with two colour/size passes each. Airborne volcanic ash is drawn as four offset copies
instead of eight. In the cloud motion, the "Siren" height override applies to rooms 17 and 18
(GameCube: room 17 only) with the opposite sign (effect not checked), and the "ADMumi" stage gets an
extra height override; cloud shadows set their alpha directly instead of easing it. The precipitation
renderer only rebinds a texture when its descriptor has changed.

## HD screens: HUD, GamePad map and message windows

Apart from the HUD unit, the per-step code of the HD 2D screens is HD-only and has no GameCube
template; it was reconstructed from the binary alone. It keeps the original per-call (30 Hz) timing;
none of it contains a 60 Hz conversion.

- **HUD:** in the HUD unit (d_meter), the function at the place of the GameCube status check also runs
  the per-frame HUD dispatch and pane transitions; the unit keeps separate pending and displayed
  counts for items and magic, and decides which item icons stay visible from HUD flags, the current
  scene and the recollection state. Its fade helpers ease towards a six-step boundary. Further screen
  code handles button prompts, key sparkles, the hearts (the shown count follows the saved one after
  an eight-call delay, the shown capacity follows the saved one by one per call), boss-eye animations,
  shortcuts, the swim timer (meter blinking and six random flash panes) and the telescope counters.
- **GamePad map:** the area map is rebuilt as a seven-by-seven grid of cells with resource indices and
  a count of discovered cells, and Link's marker is placed from the pane dimensions. A shared helper
  moves towards a target with a proportional step bounded by a minimum and a maximum distance. The
  dungeon map is dragged by touch or scrolled with the stick; map icons use a distance-limited chase,
  a viewport bounds check and grid-cell matching.
- **Minigame HUD:** Battleship and the cannon game reveal entries from arrays, the boat race plays
  sounds at fixed roll-counter values, and the rupee/sword counter moves towards its target by one
  unit per call, capped at 999.
- **Message windows and text typing:** a fractional character accumulator types the text, holding a
  button speeds it up, and there are a show-all trigger and separate pause and wait timers. The
  windows also cover two- and three-choice selections, a scroll window that moves by ten units per
  call over up to three lines, numeric input in steps of one, ten or one hundred, the melody window
  that compares the player's notes with the melody beat by beat, and the save/load window that waits
  while the save manager is busy. Out-of-range indices fall back to the first slot.

## Engine libraries

- **JParticle (particle system):** the GameCube JPA1 algorithms are kept: the fields (gravity, air,
  magnet, Newton, vortex, convection, random, drag, spin), emitter volume sampling with its private
  random stream, the scale, colour, alpha and texture visitors and the resource block getters. Field
  data sits four bytes lower and the draw context uses other offsets. New in HD: each particle has an
  owning emitter pointer and GPU bookkeeping, and its initialisation clears the GPU counters and then
  runs GPU setup, the initialisation callback and a GPU update. Deletion takes two steps: a deleted
  particle is first hidden and only released two calculation passes later, when an HD helper frees its
  fourteen GPU buffers and returns it to the free list; emitters get separate deletion and
  particle-clear countdowns, and an empty emitter is terminated two passes after it becomes eligible
  for deletion. Draw initialisation caches texture descriptors and only copies a descriptor when
  selected fields differ (otherwise it just refreshes the image fields); extra textures have their own
  slots.
- **JStudio (cutscene engine):** the timeline keeps its sequence, wait and suspension behaviour, with
  the object fields four bytes earlier. The adaptors keep their sound, particle and stage update and
  fade operations, but the fade durations are converted from float to unsigned with explicit handling
  around 2^31; the particle adaptor writes the HD emitter's translation, rotation matrix, scale,
  colours and draw status, and actor, camera and light updates go through HD virtual slots.
- **NintendoWare layout (nw::lyt) instead of J2D:** the HD 2D screens are NW4F layouts. An animator
  advances its frame once per update, clears its three event bits and then clamps, wraps once or
  reflects at an end; a layout walks its animator list before its parts. The HD layout subclass
  creates its panes as HD wrapper classes, and a UI-part scheduler starts the animations of a group of
  items in sequential, reversed, centre-out, paired or indexed order with fixed, quadratic or random
  delays.
- **Model and animation helpers (m_Do_ext):** animation blending reads packed 48-byte records and
  writes 56-byte results, with separate Euler-to-matrix, quaternion-blend and Euler-to-quaternion
  paths; the matrix-to-quaternion conversion is a separate function that uses the hardware
  reciprocal-square-root estimate. Line ribbons (with a taper or per-point widths and camera-facing
  normals) are drawn through line materials; textured lines use a second shader path.
- None of these library units shows a 60 Hz timing change; they keep the original per-pass behaviour.
