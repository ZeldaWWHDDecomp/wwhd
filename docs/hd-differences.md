# Wind Waker HD vs. GameCube: differences found during the verified decompilation

Every unit (actor / object) verified against the WWHD binary is compared with the GameCube
decompilation (zeldaret/tww). Differences in behaviour, rendering or structure are recorded here,
one entry per unit, written when the unit was verified.

**Rules (publishable document):** describe differences in your own words — **no code** from either
version, no decompiled snippets, no data tables copied from the game. Addresses/symbol names are fine.
Mark interpretations as such ("probably …"); state only what the code shows as fact.

Categories: **Gameplay** (behaviour players can notice), **Graphics** (rendering/shading/HD effects),
**Fix** (GameCube bug or crash fixed in HD), **Structure** (layout, signatures, code moved/removed),
**Removed** (features absent in HD), **HD-only** (code with no GameCube counterpart).

**Snapshot:** 2026-10-06, 752 units compared.

## Entry format

```
### d_a_xxx (short description) — verified, <date>
- Gameplay: …
- Graphics: …
- Fix: …
- Structure: …
```

## Entries

### d_a_npc_md (Medli)
- HD-only: an extra warp action (0229310C) with no GameCube counterpart; probably lets Medli rejoin
  Link so she cannot get stranded.
- Gameplay: her ground-arrival behaviour (ARRIVAL_GND) starts in a different room state and skips an
  animation blend.
- Graphics: no blob shadow (HD draws real shadows); different mirror scale.
- Structure: "save option" mirror state and body turn added to lookBackWaist.

### d_a_hookshot (Hookshot and chain renderer) — qualified scoped audit
- The chain renderer changes from the GameCube's small J3DPacket shape and GX display lists to an HD 0xD0A0 shape with two vertex buffers, GX2/resource rendering and 300 links of 0xA8 bytes at +0x980. Later actor fields shift by 0xD25C. Core shooting, pulling and chain behavior remain recognizable; no new gameplay behavior was established.
- All 31 owned entries pass O1/O2 and signed-overflow/bounds controls at 10,000 inputs each. Explicit unsigned expressions preserve PowerPC 32-bit wrapping. The 2,748-case inventory is fully classified, with historical execution epochs and invalid cases retained separately. Final artifact identities and exact candidate splices were independently checked.
- Verification is qualified to initialized typed objects, bounded helper/API fixtures and private frames. Constructor coverage is 91/120 and renderer coverage 92/95, with explicit placement, preserved-register and disjoint resource-header assumptions for the remaining paths. Actual allocator ownership, GPU effects, callback lifetimes and unrestricted constructed states remain open. The destructor's final-memory difference uses modeled free; the allocator-header exclusion uses modeled allocation.

### d_a_kamome (seagull)
- Gameplay: smaller wander radius; a water check when landing; Aryll's gull lands only near the
  ground; a keep-above-ground timer.
- Graphics: no blob shadow.

### d_a_npc_ls1 (Aryll)
- Gameplay: telescope procedure rewritten (fits the HD GamePad gyro telescope).

### d_a_npc_zl1 (Tetra)
- Graphics: per-material eye shapes, separate draw lists and two texture-pattern animations (more
  expressive face than the single GameCube animation).

### d_a_kb (pig)
- Gameplay: the attack collider moved to mode 0x24; gravity and wall stop moved to the end of the
  update.

### d_a_sk (jungle vines)
- Gameplay: changed vine motion; 4 collision spheres.

### d_a_vrbox / d_a_vrbox2 (sky)
- Graphics: draw and update rewritten; dungeon rain handled in Execute; stage-synchronised Create.

### d_a_itembase
- Graphics: texture swap for item 0x77; rupee material flags (HD shine).

### d_a_spotbox
- Graphics: the spot-light model is no longer drawn; explicit cull box instead.

### d_a_obj_table
- Graphics: no shadow. Structure: swapped collision (dzb) indices.

### d_a_obj_hole
- Fix: null check for collision triangles added. Structure: debug-draw reduced to a static init.

### d_a_npc_bm1 (Rito)
- Fix: eInit_ATTENTION_ starts from the actor's shape angle; on GameCube the angle was uninitialised.
- Structure: HD-only byte at 0xA08; extra animation files for types 2–6; head texture animation by
  head model name.

### d_a_mtoge (Forsaken Fortress spikes)
- Structure: the action table became function-local, initialised at runtime.

### d_a_agb (Tingle Tuner)
- Removed: absent in HD (no Game Boy Advance link); no code for it.

### d_a_obj_msdan, d_a_nz (rat) and others
- Structure: GameCube decompilation has only stubs for these; written entirely from the WWHD code.

### d_a_swtact (Wind Crest, Wind's Requiem floor switch) — verified, 2026-10-02
- Structure: layout shift only (+0x11C); the answer-to-song mapping became a small byte lookup table instead of a switch, same results.
- Structure: the player's "set tact event" and "current song" queries are virtual calls through the HD player vtable; the XZ distance to the player is computed inline.
- Structure: compiler-generated functions added (static init of header statics, SafeString and actor deleting destructors, an empty SafeString virtual).

### d_a_sie_flag (Forsaken Fortress flag with cloth) — verified, 2026-10-02
- Fix: Delete now destroys the cloth packet and clears the pointer; on GameCube the packet was never freed (probably a leak fix).
- Structure: the cloth's environment-light block is HD-sized (0x1C8 bytes) with three copies of the default light data; the actor constructor is out of line.
- Structure: the wind-strength comparison takes the "adopt new wind" branch when either value is NaN (same as GameCube for normal values).

### d_a_fan (wind fan) — verified, 2026-10-02
- Gameplay: an out-of-range fan type 3 is clamped to type 0 at creation.
- Structure: the joint lookup became a name-table search for the propeller joint; the HD texture-animation entry takes the model instead of its model data; setting the sound reverb is inlined.

### d_a_lbridge (light bridge) — verified, 2026-10-02
- Structure: layout shift only (+0x11C), with HD-sized animation members; Delete inlines the actor's delete method; compiler-generated destructors and static init added.

### d_a_shutter (shutter door) — verified, 2026-10-02
- Structure: the half-width offset is computed as a multiplication by one half instead of a division; the movement targets use fused multiply-add. Behaviour unchanged apart from rounding.
- Structure: compiler-generated destructors, static init and an empty virtual added.

### d_a_obj_Vteng (Puppet Ganon curtains, intro cutscene) — verified, 2026-10-02
- Structure: layout shift only (members follow the larger HD actor base); the class has a virtual destructor, and the resource name goes through the HD string-wrapper type. The translation unit's static initialiser and destructors are compiler-generated and have no GameCube counterpart in the source.

### d_a_tag_ghostship (Ghost Ship clear-event tag) — verified, 2026-10-02
- Structure: the event-register write after clearing the ship sets the fixed final value, as in the GameCube release build (not the demo's increment). The debug-tuning object is laid out differently (its virtual table pointer moved to the end) and is constructed by an out-of-line constructor called from the static initialiser.
- Structure: the cutscene's current cut name is now checked for being missing (an assertion), and only then compared with the laugh cut; the stage change at the end passes extra arguments compared with GameCube.

### d_a_floor (Wind Temple breakable floor) — verified, 2026-10-02
- Structure: layout shift only (member offsets follow the larger HD actor base; the collision base class keeps its virtual methods in the actor's own HD virtual table). The ride callback (unnamed in the function map at 0213FD18) behaves as on GameCube: walking on the floor without the Iron Boots arms it, stepping on it with them breaks it after six frames.

### d_a_tornado (Ballad of Gales tornado) — verified, 2026-10-02
- Graphics: the per-joint callback no longer writes each bent joint matrix straight into the model; it keeps its own block of eleven matrices in the actor and copies all of them into the model once the last joint has been processed. Creation fills that block with zero matrices first. This is probably needed by the HD skinning/matrix pipeline.
- Structure: after creating the main model, an extra HD model call sets a flag on all its material entries (probably a render-state switch for the HD renderer). Animation frame limits are read through a virtual call on the animation object. Gameplay logic (wind pull, ship capture radius, leash distance to the home position, shrinking when dismissed) matches GameCube.

### d_a_tori_flag (small red flag, Flight Control Platform / Horseshoe Island) — verified, 2026-10-02
- HD-only: deleting the actor also deletes its cloth packet through the packet's virtual table; on GameCube this was implicit.
- Structure: the constructor is out of line; the debug-tuning object has an HD constructor and destructor.

### d_a_wbird (probably the bird that carries Link during the Ballad of Gales warp) — verified, 2026-10-02
- Gameplay: the flight to the start position takes 60 frames instead of 80, and the move phase uses different frame thresholds, so the warp sequence is probably slightly shorter.
- Structure: placing the player goes through a virtual call; delete returns immediately without calling a destructor.

### d_a_himo2 (Grappling Hook rope) — verified, 2026-10-02
- Gameplay: faster hook/rope speeds; the hook counts as arrived within a tighter distance; pulling back uses a different smoothing helper; the random direction choice from the debug tuning object is gone; camera and sound constants differ.
- Gameplay: a wider target search window in the "ADMumi" stage, and an extra packet setup in the "GanonK" stage.
- Graphics: the draw adds HD line/packet calls; four extra draw packets are inserted after the hook model (the later members shift accordingly).
- Structure: the candidate list for the target search holds 100 entries and is guarded by an assertion.

### d_a_arrow_lighteff (fire/ice/light arrow glow) — verified, 2026-10-03
- Graphics: the glow's particle emitters are scaled together with the arrow model, and the model scale is taken from a value in the particle manager when one exists (probably a global effect scale for the HD resolution).
- Graphics: the glow is only drawn while its draw flag is set, like the GameCube demo build, instead of always.
- Gameplay: the arrow's "hit" state ends the glow for any non-zero value, not only 1; Link's arrow-effect flag is cleared for any non-zero ownership flag.
- Structure: the fire-arrow shrink step is computed in single precision; members shifted by the larger HD actor base; the point light carries an extra HD field.

### d_a_demo_item (item held up in get-item cutscenes) — verified, 2026-10-03
- Graphics: the vertical position of the get-item sparkle is flipped for some emitter kinds (probably those using an HD coordinate convention).
- Graphics: the specular-map setup passes the model instead of its model data.
- Removed: unused scratch vectors in the particle setup.
- Structure: the bottle check is a contiguous item-number range; the position offset table is a lazily initialised static; layout shift for the HD item base.

### d_a_bmdfoot (Kalle Demos floor tentacles) — verified, 2026-10-03
- Gameplay: the boss state that sends the tentacles into their ending animation (probably the boss's defeat) is honoured from any tentacle state, and only once (GameCube: only while they were in the damage state).
- Fix: the boss search callback checks for a missing actor; the heap setup checks the animation object before its model; the joint callback guards against out-of-range joint numbers.
- Removed: the debug-register tuning offsets (all but one in the damage swing) are gone, leaving plain constants.
- Structure: larger HD lighting block shifts the later members (object grows by 0x234 bytes); the tentacle's own lighting block is copied member by member at creation.

### d_a_obj_volcano (volcano eruption object) — verified, 2026-10-03
- Structure: the actor grows from 0x504 bytes on GameCube to 0x73C bytes in HD. Larger animation and collision objects move the later state fields beyond the ordinary actor-base shift.
- Graphics: HD converts material color channels to normalized floating-point components and uses the HD material color interface. The GameCube material path uses byte alpha.

### d_a_atdoor (Tower of the Gods jail door) — verified, 2026-10-03
- Structure: layout shift only (fields +0x11C). The action table inside Execute is set up on first use instead of being static data, and the actor has a virtual destructor.

### d_a_obj_Ygush00 (spring water gush) — verified, 2026-10-03
- Structure: the type parameter is range-checked before it is stored, not stored first and reset afterwards. The heap-creation success check accepts any non-zero result instead of exactly 1. The texture and bone animation objects are bigger in HD (the actor grows from 0x2C8 to 0x4C0 bytes).
- Gameplay: the sound check against the Gryw00 water level is written so that an invalid (NaN) water level still plays the spring sound. In normal play this makes no difference.
- Graphics: the lower bound of the culling box is computed as zero times the Y scale, where GameCube uses a constant zero. Probably a compiler artefact with no visible effect.

### d_a_amiprop (rotating mesh propeller) — verified, 2026-10-03
- Structure: layout shift only (fields +0x11C; the collision members keep their GameCube sizes). When the type parameter is neither 0 nor 1, the GameCube source reads an uninitialised value. HD takes the "still turning" path and plays the propeller sound; probably just how the compiler resolved the undefined case.

### d_a_ks (Morth) — verified, 2026-10-03
- Gameplay: Morths no longer switch Link's heavy state on and off. Instead, a counter on the player goes up every frame while a Morth clings to Link (probably drives the HD slowdown/shake-off logic). A clinging Morth now follows Link's position change each frame.
- Gameplay: the fall-out check only counts frames with no ground below. The ground-code and height tests are gone.
- Gameplay: a wind hit pushes the Morth along Link's facing direction with a random spread, instead of away from the hit point.
- Structure: the wall check is set up once at creation, with a larger radius in stage "TF_02" (from a debug register), not on every ground check. In stage "GanonK" two extra model packets are set up. The class grows from 0xC8C to 0xF08 bytes (two 0xB0-byte model packets inserted).

### d_a_shutter2 (shutter door, type 2) — verified, 2026-10-03
- Structure: layout shift only (+0x118). It has its own copies of the moving-background helper functions.

### d_a_hys (crystal switch / target shutter, probably the hit-to-open eye switch) — verified, 2026-10-03
- Structure: the texture-pattern animation is much larger in HD (0x14 to 0x74 bytes). Creation masks the type with 1 when picking the resource. The four arrow-type tests in the wait mode are merged into one combined test (same result).

### d_a_swpropeller (wind-driven propeller switch) — verified, 2026-10-03
- Structure: written entirely from the HD code (the GameCube decompilation has only stubs), so there was no source-level comparison. Wind hits spin it fast; other hits give a short backwards kick that dies away; the switch toggles on the first windless frame after being blown.

### d_a_icelift (ice lift) — verified, 2026-10-03
- Structure: The HD actor occupies 0x680 bytes and includes a separate deleting destructor and SafeString virtual helpers absent from the available GameCube source function list. This is a difference in the available implementation inventory, not evidence that the helpers were newly added in HD.
- Comparison limit: The available GameCube actor implementation contains nonmatching placeholders and its header provides no member layout. HD motion, collision and rendering were reconstructed and verified, but this reference does not establish a gameplay change, graphics change, fix or removal.

### d_a_ikari (Forsaken Fortress hanging anchors) — verified, 2026-10-03
- Graphics: in the "BG" lighting variant only the red, green and blue of the actor light colour are copied into the anchor's colour register; the alpha copy of the GameCube version is gone (probably irrelevant for the shader).
- Structure: layout shift only (+0x11C); the rest (wind sway, cull box, random start phase) matches the GameCube code.

### d_a_obj_mmrr (Earth Temple light-reflecting mirror) — verified, 2026-10-03
- Structure: the two texture-scroll animations are much larger in HD (0x14 to 0x74 bytes each), so everything after them moves by +0x1DC. The triangle corners of the mirror's light-catching colliders are filled in at program start instead of being constant data; the values are the same.
- Structure: the actor's destructor does not destroy the effect callback or the animations (probably harmless).

### d_a_bg (room background models) — verified, 2026-10-03
- HD-only: a list of stages decides per model how it is drawn. Model 2 in Hyrule and Ganon's Tower is drawn into an extra list (probably for the HD lighting/bloom passes); model 1 in the great fairy fountains and model 3 in a list of about thirty stage/room pairs (Earth/Wind temples, islands, interiors, ships) get a material flag set on every material.
- Graphics: in the Forest Haven exterior ("Omori") models 1 and 3 are drawn in swapped order. Model 1 is drawn between two extra HD state calls with its own draw buffer. The room light is applied again right after creation.
- Removed: the GameCube "room heap" path (loading the room into a pre-allocated memory block) and the material-animation setup for texture/colour animations; only the .bmd model names are tried (no .bdl fallback).
- Structure: each background model entry gains a flag byte (0x10 to 0x14 bytes); "sea" and stage names are compared as engine strings rather than with strcmp. The actor delete function no longer runs the destructor (the virtual destructor does).

### d_a_shop_item (shop display item, with d_a_shop_item_static) — verified, 2026-10-03
- Gameplay: an item number of 0xFF (no item) is replaced by a green rupee before the model is chosen.
- Structure: the base draw and the cloth draw are virtual calls in HD. The cloth-type choice uses a lookup table instead of a switch. One of the two temporary position arrays in the matrix setup is gone. The static helper unit gains a centre getter that returns its result through a hidden pointer. The members move by +0x114.

### d_a_spc_item01 (special display item) — verified, 2026-10-03
- Removed: Execute no longer updates the collision shape. Probably the item needs no collision in HD.
- Structure: the draw function tail-calls the virtual base draw. Otherwise only the layout moves.

### d_a_acorn_leaf (leaf that drops a Korok acorn) — verified, 2026-10-03
- Structure: layout shift only. Delete calls the normal resource release instead of the demo-build variant, and the bone-animation object is bigger in HD.

### d_a_obj_tntrap (triangle trap) — verified, 2026-10-02

- Structure: The GameCube reference still has placeholders for the actor methods and no recovered member layout, so gameplay or rendering changes cannot yet be established from it. The HD actor has a 0xF04-byte layout, eight collision triangles with a 0x150-byte stride, and seven action descriptors. HD save and play state are obtained through singleton accessors.

### d_a_tag_waterlevel (schedule-driven water level tag) — verified, 2026-10-03
- Structure: layout shift only (+0x11C); the step clamp is computed before a single store (no behavioural change).

### d_a_obj_hfuck1 (hookshot target) — verified, 2026-10-03
- Fix: the hookshot-actor name check also tests the actor for null before reading its name.
- Structure: layout shift (+0x11C); uses the final (non-demo) collision radius and offset; no debug HIO.

### d_a_obj_eskban (bomb-breakable boulder, Earth Temple) — verified, 2026-10-03
- Fix: the hit-actor name checks (Medli push-away, bomb detection) test the actor for null first.
- Structure: layout shift (+0x118 after the HD move-BG base); the debris model emitter takes one extra HD argument.

### d_a_obj_stair (crumbling stair) — verified, 2026-10-03
- Gameplay: the fall-speed target of the collapsing stair is -175 instead of -70, so it probably drops away faster.
- Fix: the ride callback checks the riding actor for null before reading its profile name.
- Structure: layout shift (+0x118 after the HD move-BG base); the debug HIO object is still built at startup (vtable placed last).

### d_a_tag_etc (Medli flight camera / event trigger tag) — verified, 2026-10-03
- Structure: layout shift only (actor fields moved by the larger HD actor base; the action table is set up on first use).
- Removed: the delete callback no longer runs the (empty) destructor.

### d_a_title (title screen ship and logo manager) — verified, 2026-10-03
- Graphics: the 2D logo screen (panes, fades, kirakira timing) moved out of the actor into an HD title layout object; the actor only positions the smoke, wind and sparkle particles from coordinates and frame numbers that object supplies.
- Graphics: only the ship model is drawn by the actor (the subtitle and sparkle models are still loaded but not drawn here); the ship is drawn through a viewport looked up at run time and gets an extra invisible-model pass; ship placement depth and facing differ from GameCube.
- Gameplay: pressing a button during the intro jumps straight to the "entering" state instead of waiting for a second press; the "next stage" path always requests the title scene.
- Structure: the attribute table became a small object inside the title process; the 2D heap, screen and pane data are gone; the actor's delete work moved into its destructor; the wind/sparkle counter runs in every mode.
- HD-only: tells the HD title layout when the intro starts and when the game is entered; particles flip their vertical position for some emitter types.

### d_a_obj_hami2 — verified, 2026-10-03
- Structure: The available GameCube implementation bodies are nonmatching placeholders, so this audit does not establish a gameplay or graphics difference. The HD actor layout and emitted entry points were verified independently.

### d_a_obj_hami3 — verified, 2026-10-03
- Structure: HD replaces the GameCube joint-name loop with a model-data name lookup using relative name-table entries.
- Graphics: The HD joint callback explicitly marks animation-matrix data dirty; the available GameCube callback updates matrices without those explicit flags.

### d_a_obj_vgnfd — verified, 2026-10-03
- Removed: The GameCube door-opening particle path recolors returned emitters from the actor lighting color. That recoloring loop is absent from the audited HD execution path.
- Structure: Larger texture and color animation controllers expand the actor layout in HD.

### d_a_obj_vmc — verified, 2026-10-03
- Structure: The growth and hook animation controllers are substantially larger in HD, shifting later actor fields. HD drawing also combines the two animation-removal paths into a model-animation-pointer clear; the GameCube source keeps separate state-dependent removal calls. The audit found no change to the horizontal or vertical range hysteresis.

### d_a_obj_vyasi — verified, 2026-10-03
- Structure: HD uses an expanded actor and collision layout with emitted collision construction and destruction helpers. The available GameCube method bodies are placeholders, so no gameplay or rendering difference is established by this comparison.

### d_a_bmd (Kalle Demos, Forbidden Woods boss) — verified, 2026-10-03
- Gameplay: The core can only be damaged while the flower is open; in HD the hit timer is held whenever the flower is closed. Standing on the closed flower briefly hides the core. Link has to come a little closer (700 instead of 500 units) before the intro starts. When the flower closes, the core gets a short damage cooldown, and the throw angle when Kalle Demos spits Link out is mirrored compared with GameCube.
- Gameplay: The final hit also clears a play-state value and sets a blur. The core flies off on a slightly flatter arc. The heart container appears through a different disappear-effect variant, and the death cut-scene jumps straight to its next state.
- Graphics: The intro and death cut-scenes use different, mostly fixed camera positions (with debug-register offsets), a field-of-view ease at the end of the death scene, a motion blur at the climax of the intro, and an extra surprised reaction with a voice from Link. The snapshot (picto box) registration targets Makar when he is visible, using a joint found by name in his model.
- Removed: The boss's simple ground shadow is not drawn.
- Structure: The actor is larger (0xD04 bytes); one HD-only field holds Makar's snapshot joint. The large Execute, move and camera functions inline their GameCube helpers. The demo counters are printed through three debug-report calls.

### d_a_obj_plant (bush that swings when bumped) — verified, 2026-10-03
- Graphics: no blob shadow under the bush (HD draws real shadows).
- Structure: the swinging joint is looked up by name through the joint name table instead of comparing every joint name; layout shift only otherwise.

### d_a_obj_rflw (Rito flower that swings when bumped) — verified, 2026-10-03
- Structure: the swinging joint is looked up by name through the joint name table instead of comparing every joint name; layout shift only otherwise.

### d_a_demo_kmm (cutscene seagull) — verified, 2026-10-03
- HD-only: in the stage Demo08, once a global counter reaches 1000, the gull deletes itself and spawns three actors of another kind (process 0xC2) at fixed positions; probably a cutscene fix or replacement.
- Graphics: nothing is drawn while an HD global switch is set (probably a cutscene/draw toggle).
- Structure: the action table became a lazily initialised function-local static; the delete callback no longer runs the destructor.

### d_a_kmon (bell that Orca talks through) — verified, 2026-10-03
- Structure: layout shift only (bigger HD animation and background-check members); the inlined talk/action switching of Orca is unchanged.


### d_a_andsw2 (combined switch controller) — verified, 2026-10-03
- Structure: Three state fields follow the larger HD actor base, shifting by 0x11C relative to the GameCube layout; the HD actor occupies 0x3B4 bytes. HD copies its six callbacks into a guarded runtime table on first execution; the recovered GameCube source declares the same six callbacks in a statically initialized local table.
- Structure: The HD Delete wrapper returns immediately; the GameCube wrapper explicitly invokes the destructor. HD retains a separate virtual deleting destructor. No gameplay consequence is established from this wrapper difference.
- Gameplay: The compared implementations retain the switch fallback, timer scaling by fifteen, six-state event flow and creation-time rotation clearing. No gameplay change, graphics change or fix was identified in this comparison.

### d_a_obj_YLzou (Hyrule Castle Link statue) — verified, 2026-10-03
- Fix: HD stops heap creation immediately when model allocation fails, before loading and constructing the collision object. The GameCube implementation continued into collision setup after the failed allocation.
- Structure: The action member-function descriptor is eight bytes in HD instead of twelve. Fields after it move four bytes earlier than a uniform actor-base size adjustment would predict; the verified HD actor is 0x484 bytes.
- Structure: Resource requests use the HD resource-manager singleton and temporary SafeString objects. Event, vibration and lighting access goes through HD singleton accessors rather than the GameCube global objects.

### d_a_npc_de1 (Great Deku Tree) — verified, 2026-10-04
- Structure: the GameCube decompilation has only "Nonmatching" placeholders for this actor, so all 60 functions were written from the HD code; behaviour could not be compared line by line. The HD actor is 0x93C bytes on the larger HD NPC base (0x7DC); the action member-function descriptor is eight bytes.
- Structure: resources are requested through HD SafeString keys; the actor vtable is merged with the NPC base's; joint matrices are read through the HD model's matrix block, which is marked dirty when accessed.
- Structure: the skinned collision of the tree (a deforming background mesh whose vertices follow the animated model each frame) and the leaf-lift handling (up to ten lifts following branch joints, searched by process name) are present in HD; no gameplay change was established.
- HD-only: the demo accessor checks the demo actor index range and asserts that the demo object exists before looking the actor up (probably a debug-build check inlined from the HD demo header).

### d_a_npc_nz (rat shopkeeper of the hidden rat-hole shops) — verified, 2026-10-04
- Graphics: the draw no longer unlocks the first material packet or draws the root joint separately; it looks the three body materials up by name (HD sead strings) instead of by index, hides/shows them around a single HD draw call for the remaining materials, and switches the opaque/translucent draw lists by writing the play object's list pointers directly. The tail line material is much larger (0x188 bytes instead of 0x3C) and is handed to the sorted 3D-line packet chosen by a virtual query on the material.
- Structure: shop logic (offer selection, price/bottle/bomb/arrow/bait checks, direct item get vs. hand-over event) matches the GameCube version; the shop dialogue reads the shared HD message manager instead of the actor's current message process.
- Structure: layout shift +0x118 up to the smoke callback, then +0x264 after the tail line material; class size 0xCF8. The HIO object keeps its vtable at the end (0x28 bytes).

### d_a_npc_nz (readability rewrite addendum) — verified, 2026-10-04
- Gameplay: probably unchanged; the hand-over event after a purchase is ordered with flag 0 where the GameCube passes the "no partner" flag (1).
- Structure: the object archive is spelled "Nz" (GameCube "NZ"); assertion line numbers moved (211/222/225); the four search callbacks test the actor pointer for NULL before reading its name; the kill-all cut asks for the player only once; the GameCube slip in the pot/barrel callback (the barrel branch tests the pot name again) is kept.
- Structure: the two small functions at the end of the unit that were taken for the HIO destructor and message hook are this unit's copies of the engine string class's deleting destructor and termination hook (template code, HD only).

### d_a_npc_auction (auction bidders, the NPCs of the Windfall auction) — verified, 2026-10-04
- Structure: the GameCube decompilation has only "Nonmatching" placeholders for this actor, so all 46 functions were written from the HD code; behaviour could only be compared against the GameCube function list, which matches one to one (plus the usual compiler-generated thunks, constructor and static initialiser). The HD actor is 0x8B8 bytes on the larger HD NPC base (0x7DC); resource names are passed as HD two-word string objects.
- Structure: the NPC-number accessor clamps out-of-range parameters to bidder 0 and the random-costume helper maps a result equal to the range to 0 (probably GameCube behaves the same; not comparable without GameCube source).
- Graphics: the draw step registers the bidder (and its extra prop model) for the picture box and uses the HD light/TEV setup; no shadow call is made in HD (whether GameCube draws one could not be checked).

## Backfill for units merged before 2026-10-03 (drafted from commit messages, source notes and unit reports)

Some units have shallower entries (their sources carry few HD notes); entries for units whose GameCube code is only stubs mark HD claims as "probably".

### d_a_arrow_iceeff (Ice Arrow freezing effect) — verified
- Graphics: drawing is gated by a counter at 0xA3C, and each new shard model gets a flag at +0x78 (probably an HD material/draw flag); the ripple particle scale is set together with the global scale.
- Structure: class 0xBD8.

### d_a_att (Puppet Ganon hit target) — verified
- Fix: a null check before the boss's name is read.
- Structure: att_class 0x670, layout +0x11C.

### d_a_bb (Kargaroc) — verified
- Graphics: no blob shadow; the tail drawing adds an extra translation along each segment.
- Fix: bounds checks on joint numbers in the node callback and the joint table loop.
- Structure: class 0x1260 -> 0x13D8; shadow id removed, larger texture-pattern animation.

### d_a_bdkobj (Helmaroc King arena objects: stair fragments) — verified
- Gameplay: the stair fragments also move horizontally (with a wall check) and rock on the ground according to their size, with smaller random spread (spin 5000 vs 8000, offset 100 vs 200) and settle faster (2.0 vs 0.75).
- HD-only: an attack of type 9 (probably a specific weapon) on a fragment breaks it and drops an item, with a sound and an effect.
- Fix: the hit callback checks the attacker pointer before reading its process name.

### d_a_bk (Bokoblin) — verified
- Fix: a Bokoblin that falls more than 5000 units below its home now sets its defeat switch and marks itself as killed before it is deleted; on GameCube a fall of 4000 units just deleted it without the switch (probably fixes rooms that wait for all enemies to be defeated).
- Gameplay: no counter-attack after being shoved out of a grab (GameCube attacked when Link was not grabbing and the Bokoblin held a weapon); the shove-out push is three times stronger (375 vs 125) and only horizontal.
- Gameplay: the dizzy-stars effect ends at frame 35; a short spawn timer keeps a new status flag (0x4000) set for a few frames; a Bokoblin carrying a torch no longer looks for weapons to pick up.
- Gameplay: in Dragon Roost Cavern room 1 a jumping Bokoblin keeps its facing angle; hanging from a rope bridge toggles a new actor status bit (0x80000); different knock-down values when hit lying down.
- Graphics: no blob shadow (HD real shadows); body colour variants (pink / green / normal) are separate models instead of material-table swaps, likewise the weapon blur models.
- Structure: class 0x19A4 (GameCube 0x1848); shadow id shrank, an extra follow-particle callback, damage-reaction joint arrays have one more entry.

### d_a_bmdhand (Kalle Demos ceiling tentacles) — verified
- Gameplay: the targeting and hit point of each tentacle sits on line segment 6 instead of segment 10. One attack state sets an extra timer value (10).
- Fix: the boss joint table is bounds-checked, and the morph is checked before its model.
- Graphics: the tentacles are drawn with the HD 3D-line material (0x188 bytes, GameCube 0x3C).

### d_a_bomb (Link's bomb) — verified
- Graphics: the resting bomb no longer places a blob shadow under itself (HD draws real shadows); the shadow flag logic is otherwise unchanged.
- Graphics: after drawing a normal bomb, HD makes an extra call into the player code (023D9340) passing the bomb model; probably connects the bomb to the player's own material/glow handling.
- Structure: class 0xB50; render-list switching done through the play-state list slots instead of the P1-list helper.

### d_a_bomb2 (Bomb Flower bomb) — verified
- Graphics: the blob shadow that grew and shrank while the fuse burned (draw_shadow) is gone; HD only draws the model.
- Structure: Act_c 0x9A8 (environment helper 0x58).

### d_a_branch (Forest Haven tree branch) — verified
- Graphics: no per-material fog-type loop on create; different resource ids for the models.
- Structure: includes the HD-only d_a_branch_static unit (joint matrix lookup).

### d_a_bwds (Molgera larva) — verified
- Gameplay: when a larva takes damage it is knocked back away from Link (it uses Link's direction instead of the GameCube pitch/yaw). The push starts at half the strength (40 instead of 80) and lasts longer (15 instead of 7 frames). The knockback pitch field is gone.
- Gameplay: the body trail gets stiffer along its length in a different (geometric) way, there is no height correction while the larva is held by the hookshot, and the jump speeds come from different constants.
- Structure: bwds_class 0x19E4, layout shift +0x11C then +0x118 after the dropped field.

### d_a_cc (ChuChu) — verified
- Graphics: no blob shadow; the effect colour table takes its fourth entry from the particle manager and also sets the emitters' environment colour.
- Fix: an out-of-range ChuChu kind asserts and falls back to index 0 (GameCube read past the HP table).
- Structure: class 0xF10 (GameCube 0xE04); model-packet member removed, members from mBehaviorType on +0x108; follow-effect callbacks are ended instead of removed.

### d_a_dk (Helmaroc King demo model) — verified
- Gameplay: during the "Demo07" cutscene (frames 0x48E–0x672) Link's demo actor follows the bird's joint 25 and gets a special player state. This is probably the carry-off scene, done in HD by attaching Link to the bird.
- Gameplay: the actor waits for event bit 0x0310 before it does anything and deletes itself when the "zelda_fly" event starts. Some event checks moved from Execute into Create.
- Structure: debug HIO reduced to two floats, other HIO values folded in; dk_class 0xC0C.

### d_a_door10 (stone sliding door) — verified
- Gameplay: openProc waits until the destination room has finished loading before the door starts to move; GameCube moved it immediately. Probably needed because rooms load more slowly on the Wii U.
- Gameplay: the "door starts moving" sound is no longer played in openInit; it plays on the first frame the door actually moves (so it lines up with the delayed start).
- Structure: class 0x500.

### d_a_door12 (double stone door) — verified
- Gameplay: same as d_a_door10: opening waits for the destination room to be loaded, and the start sound plays when movement begins (still suppressed for the types 8/11 case, as on GameCube).
- Structure: class 0x4B8; key lock helper enlarged to 0xA0.

### d_a_dr2 (Gohma fight: Valoo's tail, lava pit, ceiling rock) — verified
- HD-only: the lava floor's texture matrix follows the camera (probably an HD shading effect on the lava).
- Fix: the found-actor name check tests the pointer first.
- Structure: GameCube layout +0x11C throughout.

### d_a_ep (torches / braziers) — verified
- Graphics: the light from the flame now flickers a little (its strength drifts randomly by about ±10%), and it is dimmer (about 65%) in two kinds of stage. The additive glow drawn over the flame on GameCube (ep_draw) is gone. The flame model sits 16 units higher (offset -2 instead of -18).
- Gameplay: on the open sea, in the area around (-300000, -300000) (probably the Forsaken Fortress sector), torches spawn no fire and no heat haze when Link is far away and outside certain heights. This is probably a performance limit for that area.
- Structure: ep_class 0xB50; two HD-only copies of the light-info block.

### d_a_esa (All-Purpose Bait) — verified
- Gameplay: bait thrown onto water floats: the water height is remembered, water counts as its own ground state, and the bait bobs up and down (random starting phase) instead of sinking away; a bait resting on ground sits 3 units higher (no +5 ground offset).
- Gameplay: when fish take the bait it disappears, with a water-pillar splash half of the time; at the end of its life the bait shrinks away instead of vanishing when the timer runs out.
- Graphics: the water ripple rate is a tunable value (+0.3) instead of a fixed 0.4; the random scale is applied to the actor too.
- Structure: new fields for water height (0x3B8) and bob phase (0x3E0).

### d_a_fallrock (falling rock) — verified
- Graphics: no simple shadow; only the model update remains in draw.

### d_a_ff (firefly) — verified
- Graphics: every firefly is a real point light that follows it, with brightness following its scale; the GameCube glow model and its depth-buffer visibility check are gone.
- Structure: class 0x5C0 (GameCube 0x480 + 0x11C shift + light block).

### d_a_fgmahou (Phantom Ganon energy ball) — verified
- Gameplay: a returned ball only reports a hit to Phantom Ganon when the hitting actor is process 0xA8 (on GameCube any attack hit counted); probably prevents other objects from triggering the volley.
- Structure: class 0x8A0 (+0x11C).

### d_a_grass (grass / tree / flower placement) — verified
- Fix: a new assertion on the grass type with a fallback to type 0, so an out-of-range type no longer reads past the offset table.
- Structure: the placement arrays became function-local statics built on first use.

### d_a_himo3 (climbable rope) — verified
- Graphics: no alpha model and no simple shadow; the rope line material is the larger HD line type; in one specific stage a new model shadow packet is set up.
- Gameplay: in one specific stage a rope placed near x = 500 is lowered by 65 units (probably a level-geometry fix).
- Structure: class 0x2634; HD model packet inserted, shadow-size field removed.

### d_a_hitobj (short-lived water attack sphere) — verified
- Removed: the "Hitobj" archive is no longer loaded or released (Create completes immediately).
- Structure: class 0x520.

### d_a_item (field items: rupees, hearts, ammo) — verified
- Gameplay: no hearts are dropped when the save reports Hero Mode (021805CC); HD-only.
- Gameplay: two new item kinds with their own behaviour. Item 0x13 runs a small state machine (thrown up, falls, then either splashes into water or vanishes in smoke) and sounds when it bounces off a wall. Item 0x14 (probably the Tingle Bottle) plays SE_TNCL_BOTTLE_GET when collected, gets a random orientation, grows and bobs on water, stays 30 frames longer, and disappears with smoke once a game flag is set.
- HD-only: new item actions. 0xD throws an item (for example a heart) towards Link: it works out speed and gravity so the item lands at a target point, and the thrown hearts can be collected after 30 frames. 0xF is an item that cannot be collected and turns into a normal item after 30 frames. 0xE follows another actor while in water. The item can also be collected by actor 0xA5 while Link has status bit 0x10000.
- Gameplay: items that start the get-item demo are now recorded as collected when the demo ends, not on pickup. The same goes for the Blue Chu Jelly switch item.
- Fix: the starting vertical speed is set to 0. On GameCube some actions read an uninitialised register for it.
- Structure: daItem_c 0x7F0 (GameCube 0x6C0), daItemBase_c 0x750 (GameCube 0x63C); the extra fields hold the new actions.

### d_a_itembase (base class of field items) — addendum, verified
- Graphics: rupees are drawn in their own translucent draw list (the GameCube used the shared mask-off/P1 lists); no blob shadow (HD shadows); the specular map gets the model instead of the model data.
- Structure: class 0x750 (GameCube 0x63C); shadow id removed.

### d_a_kaji (ship's wheel on Tetra's ship) — verified
- Graphics: a larger, offset culling box (probably so the wheel is not culled at the HD aspect ratio).

### d_a_kamome (seagull) — addendum, verified
- Gameplay: besides the smaller wander radius, the flight height range is smaller (300 instead of 500) and the approach distances are shorter; a gull only goes for thrown bird bait when the bait is in its active state.
- Graphics: the model no longer adds the extra roll/pitch offsets to its orientation (probably a straighter flight pose).

### d_a_kanban (cuttable sign) — verified
- Graphics: no blob shadow; the texture patch that printed the sign's text onto the model is gone (probably the HD signs carry the text in their own textures).
- Fix: a null check before the actor test in one search callback.
- Structure: class 0x6E0 -> 0x7F8; shadow id removed.

### d_a_kantera (lantern) — verified
- Graphics: no flame halo (alpha model) and no separate flame model or its second colour animation; the flame is offset 2 units instead of 18; the model is calculated after its base matrix is set.
- Structure: class 0x890; larger light-influence member (+4); the GameCube movement function is only a stub, so it was written from the WWHD code.

### d_a_kb (pig) — addendum, verified
- Gameplay: the pig is placed onto the ground at creation; the big pig bounces back at a fixed speed; a wall now stops the pig; the bait-demo cool-down pauses while an event runs; the attack collider switches on above speed 10 while accelerating to 15.
- Graphics: the eye animation is a texture-pattern animation (btp) instead of a texture-number table, and the body colour is chosen by loading one model per colour instead of patching a material table.
- Structure: class 0xE2C.

### d_a_ki (Keese / Fire Keese) — verified
- Graphics: the Fire Keese has its own eye texture patterns; the eye animation is a regular btp animation instead of a texture-number animator; joints and materials are looked up by name.
- Graphics: no blob shadow; a shadow packet is still set up, but only in Ganon's Tower (probably for its dark rooms).
- Structure: class 0x10BC (GameCube 0xF04), shadow id removed, damage-reaction structures grew.

### d_a_kita (Forbidden Woods hanging flower platform) — verified
- Gameplay: when Link pushes the platform, the push direction comes from Link's facing, not from the angle between Link and the platform; the tilt factors use absolute values; the drift goes through the actor's speed (eased) instead of moving the position directly.
- Structure: class 0x804.

### d_a_kui (grappling-hook posts / bell) — verified
- Gameplay: the chime and rumble when the swing hits its angle limit no longer need the extra "limit > 2000" condition, so it probably triggers more readily. Its cooldown uses an HD debug register.
- Graphics: the effect matrix is written into the model (the HD setEffectMtx takes the model).
- Structure: kui_class 0x42C.

### d_a_kytag00 (weather area tag) — verified
- Graphics: fog (moya) particle counts scale by 30 instead of 100.
- Gameplay: the switch room is the home room (or the stay room when negative) instead of always the stay room.

### d_a_kytag05 (wind cycle tag) — verified
- Fix: the wind-table index is clamped (probably to avoid reading past the table), and the new wind direction is taken before the index advances.
- Graphics: fog (moya) count set to 30 instead of 100; no null check on the demo object during the "demo41" fade.

### d_a_kytag07 (Tower of the Gods / Hyrule weather tag) — verified
- Graphics: the fog/haze count during the "g2before" storm and the matching weather mode is capped at 30 instead of 100. This probably makes the fog lighter in HD.
- Structure: the demo frame counter is a global with no null check.

### d_a_mant (Darknut / Phantom Ganon cape)
- Graphics: the HD cape uses a dedicated GX2 drawing packet with shader uniform setup, two vertex buffers, and seven sampler/texture resources. The packet draws the simulated cape mesh.
- Structure: the HD actor occupies 0x4288 bytes, including a 0x28DC-byte packet at offset 0x3B4; the local GameCube declaration occupies 0x283C bytes.
- Gameplay: the HD update combines cape simulation, ground checks, attack interactions, particles, and sound. The corresponding local GameCube simulation functions are stubs, so this reconstruction does not establish a gameplay change.
- Verification: the completed HD reconstruction passes local reference comparisons within documented initialized-object and resource/API scopes; full game resource and GPU execution remains outside those checks.

### d_a_mgameboard (Squid Hunt minigame board) — verified
- Removed: the 2D display-list objects (shot counter digits, minigame overlay, squid icons) are gone from the actor; probably the HD version draws that HUD through its own UI layer instead.
- Structure: class 0x6E8; GameCube decompilation has only stubs, so the unit was written from the WWHD code.

### d_a_mo2 (Moblin) — verified
- Gameplay: when throwing, the Moblin first runs at the player (new chase state) and the old shortcut for one mode is gone; its attack choice also considers the height difference to the target, not only the horizontal distance.
- Gameplay: a Moblin with a death switch that falls more than 5000 units below its home sets the switch and is removed; in Hyrule, a (paralysed) Moblin is kept out of the wall behind it.
- Gameplay: the special search mode of "ITest63" and "GanonJ" also applies in "kazeMB"; a Moblin frozen in a statue pose waits 5 frames unless a certain save state is reached; tuning values changed (three HIO values 10 instead of 0x18/0x16/0x1C).
- Graphics: no blob shadow; hair strands sway with a per-strand sine motion instead of following the global wind, with extra warm-up frames for statue poses; materials looked up by name; a separate model instead of a green material table for one variant; the spear trail has no blur material table.
- Structure: class 0x3038 -> 0x343C; shadow handle removed, larger btp/line-material/damage-reaction members, new |dy| field.

### d_a_mt (Magtail) — verified
- Gameplay: a Magtail removed because it fell out of its room is re-created at its home position unless its switch is set (the GameCube decompilation has only placeholders, so this is marked HD in the WWHD code but not confirmed against GameCube source).
- Graphics: no shadow; the eye btp animation is re-initialised every frame.
- Structure: class 0x1DD8; written entirely from the WWHD code (GameCube decompilation is placeholders).

### d_a_npc_ac1 (Rito postman Baito/Rito with wing arms) — verified
- Graphics: no shadow.
- HD-only: helper functions returning the arm animation resource table and the wing animation file name.
- Structure: class 0x9EC (GameCube 0x87C); shadow id removed, members +0x114; GameCube decompilation mostly stubs, written from the WWHD code.

### d_a_npc_aj1 (Sturgeon) — verified
- Graphics: no blob shadow.
- Structure: GameCube source is only stubs; all 88 functions written from the WWHD code.

### d_a_npc_ba1 (Grandma) — verified
- Gameplay: the soup dialogue no longer checks for an empty bottle in one branch (the same message is always used), and a half-full soup bottle now counts like a full one when she offers a refill.
- Gameplay: in the opening stage "Demo01" during a certain frame window the demo flags differ (probably a cutscene timing adjustment).
- Graphics: no blob shadow.
- Structure: class 0x98C (GameCube 0x81C); messages go through the HD message manager.

### d_a_npc_bm1 (Rito) — addendum, verified
- Gameplay: a Rito walking a path checks the ground below its next position; if the drop is more than 50 units it sets the HD-only flag at 0xA08 and stays where it was, so it probably cannot walk off ledges any more.
- Gameplay: without a path the "passed" flag is no longer updated in one case.
- Graphics: no blob shadow; body and arm models are always drawn without their extra material tables.

### d_a_npc_bmcon1 (Willi and Obli, Flight Control Platform) — verified
- HD-only: when the post-minigame event starts, a new player status flag (0x80000) is set on Link.
- Structure: class 0x93C; GameCube decompilation is stubs, written from the WWHD code.

### d_a_npc_bmsw (Koboli, Dragon Roost mail sorter) — verified
- Gameplay: the sorting minigame reads the HD controller object (button triggers with cursor auto-repeat) in place of GameCube's STControl stick handler.
- Gameplay: in stage "Demo10" Koboli starts out sorting (animation 5). Message 0x1A68 now also switches on the game camera.
- Fix: the minigame rupee counter can no longer go negative (a sum of zero or less becomes 0).
- Structure: class 0xD84 (GameCube 0x9E4); larger HD tev and btp blocks; pointer-to-member fields are 8 bytes.

### d_a_npc_co1 (Prince Komali, before Dragon Roost Cavern) — verified
- Graphics: Din's Pearl he holds carries a point light (set up in his wait action, removed on delete); no blob shadow. Probably HD, but the GameCube decompilation is stubs so this is not confirmed.
- Structure: class 0xA38; written from the WWHD code.

### d_a_npc_ds1 (Doc Bandam) — verified
- Graphics: colours computed through a new gamma-correction helper.
- Fix: probably an HD-introduced slip: when the room effects are deleted, the loop invalidates the six emitters after the first plus the next pointer (the texture model), not the seven emitters that were created.
- Structure: shop camera class 8 bytes smaller (PTMF size); GameCube source is a stub, written from the WWHD code.

### d_a_npc_gk1 (Mila's father, poor) — verified
- Graphics: no blob shadow.
- Structure: GameCube decompilation is stubs; layout and code written from the WWHD code.

### d_a_npc_gp1 (Maggie's father, rich) — verified
- Graphics: no shadow.
- Structure: class 0x988; message status from the HD message manager; GameCube decompilation mostly stubs, written from the WWHD code.

### d_a_npc_ho (Mrs. Marie) — verified
- Graphics: no blob shadow; the face texture-pattern animation always takes the first table entry.
- Structure: class 0x6B4 -> 0x828; shadow id removed; message handled by the message manager.

### d_a_npc_ji1 (Orca, Knight's Crest sword trainer) — verified
- Gameplay: in the sparring game, Orca's hits on Link are counted at most once per 10-frame window (new HD timer byte at 0xE64); on GameCube every collider contact counted, so probably fewer "double hits" are charged to the player.
- Gameplay: in sword lessons the first failed cut is forgiven once before the "bad cut" event plays (HD-only flag at 0x10467244, reset when an event is ordered).
- Gameplay: the item Orca hands out is created in the current room instead of the GameCube demo room (-1); the message checks use the PAL order (the 0B20 event flag is tested before the Knight's Crest count).
- Fix: the record lookup indexes its four-entry table modulo 4, and the timer setup asserts and falls back to the last level for out-of-range counts (GameCube read past the tables); probably defensive fixes.
- Graphics: no blob shadow.
- Structure: class 0xFB0 (GameCube 0xD88); messages go through the HD message manager instead of a msg_class pointer; a dead guard-sound branch is gone.

### d_a_npc_kf1 (Mila's father, the Windfall pot shop) — verified
- Graphics: no shadow.
- Structure: the selected answer and message status come from the HD message manager; GameCube decompilation mostly stubs, written from the WWHD code.

### d_a_npc_km1 (Mila, rich) — verified
- Graphics: no shadow model and no real or simple shadow.
- Structure: class 0x944; per-type tables always read entry 0.

### d_a_npc_ko1 (Joel and Zill) — verified
- Gameplay: in a two-NPC conversation the first speaker opens the message itself.
- Graphics: no shadow drawing (shadowDraw removed).
- Structure: class 0xA1C (GameCube 0x8AC; the GameCube header has no members); animation-parameter entries 0x14 bytes.

### d_a_npc_kp1 (Maggie, rich) — verified
- Graphics: no shadow.
- Structure: class 0x950; a third hand joint is looked up at heap creation (probably HD); GameCube decompilation mostly stubs, written from the WWHD code.

### d_a_npc_ls1 (Aryll) — addendum, verified
- Gameplay: the telescope check for spotting the Rito postman uses a wider window (90/30 instead of 60/-30), and Aryll's line depends on the current telescope zoom level (two different messages, 0xBCA/0xBCB).
- Gameplay: after the telescope event the scope message status is not checked and the camera is restarted on the next talk instead; the "seen" flag is only cleared while a play-state byte (+0x5BB3) is zero.
- Graphics: only the two eye materials get the eye-texture animator (looked up by name, "texmtx1"); on GameCube every material had one. No shadow.

### d_a_npc_md (Medli) — addendum, verified
- Fix: the sound reverb now reaches the monster-sound call as its fourth argument, as intended; the GameCube code passed it incorrectly.
- Gameplay: when Link is at a door she switches to the HD-only warp action instead of only refusing to follow; her final create phase waits for an actor condition bit instead of a fixed 60-frame timer.
- Gameplay: an event also sets her wall-collision circle; the tact (song) message sets a play-object flag; she is also animated on the Dragon Roost spring stage (M_Dra09) and returns to wait there after playing.
- Graphics: no real shadow.
- HD-only: a static global pointer to Medli, set on create and cleared by her destructor (probably for the warp/rejoin logic).

### d_a_npc_mn (Manny, Windfall figurine fan) — verified
- Graphics: no blob shadow; the shadow id field is removed.
- Gameplay: no next-room path lookup at creation and no path reset in one event step. The event "SpeedY"/"Gravity" values have no effect (on GameCube they were overwritten anyway).
- Structure: class 0x93C (GameCube 0x7C8), layout from mPathRun +0x174; message functions are virtual.

### d_a_npc_people (Windfall townspeople) — verified
- Gameplay: the HD Swift Sail (item 0x77) counts as having the sail in the sail-dependent dialogue.
- Gameplay: one reward branch gives item 0xFD (per the source notes, a bag upgrade) when the player does not own it yet, instead of the fixed GameCube reward.
- Gameplay: the picture check for one townsperson (UM2) compares against result 0x67 instead of 4, which fits the reworked Picto Box. The "shiver" state lasts 150 frames instead of 120, the "look" state 90 instead of 60, and the letter-reading animation window is longer.
- Fix: probably a placement fix: an UG1 placed at x = -1984 is moved to a corrected position when created.
- Graphics: no blob or real-time shadow registration (HD draws real shadows).
- Structure: class 0x920 (GameCube 0x7A9); shadow id removed, so the layout from mPathRun is +0x174; the scope-type behaviour matches the PAL version.

### d_a_npc_pf1 (Maggie's father, poor) — verified
- Graphics: no blob shadow.
- Structure: GameCube source is only stubs; all 70 functions written from the WWHD code.

### d_a_npc_photo (Lenzo, Windfall photographer) — verified
- Gameplay: the picture limit for the photo quests is 12 instead of 3 (matches the larger HD Picto Box album); a free album slot receives the day's picture.
- Gameplay: showing the Firefly bottle no longer checks for the Deluxe Picto Box and no longer hands out an item; the Deluxe Picto Box is given at a different point of the quest, and the item ids of the rewards changed (the Legendary Pictograph reward slot holds a different item).
- Gameplay: during the "PHOTO_GALLERY" event Lenzo starts from a second, HD-only position and walks to the gallery spot while he talks.
- Gameplay: two of his lines are swapped for other messages once a new HD flag is set; the "third order finished" / "not in colour" lines are gone.
- Graphics: no blob shadow.
- Structure: class 0x9DC -> 0xB50; shadow id removed, larger texture-pattern animation (+0x174), HD-only byte at 0xB4E.

### d_a_npc_pm1 (Maggie's father, poor) — verified
- Graphics: no shadow model and no real or simple shadow.
- Structure: the code is the same as Mila's father (d_a_npc_km1) with other constants; tables always read entry 0; class 0x944; the GameCube decompilation has only stubs.

### d_a_npc_roten (Traveling Merchants) — verified
- Graphics: no shadow, no per-merchant material table; all three merchants share the same display-list flags.
- Structure: class 0xB40; the animation lists are a table of nine; the get-item event reads a single item number.

### d_a_npc_sv (Salvage Corp. members) — verified
- Graphics: no shadow.
- Structure: class 0x860; GameCube source is only stubs, written from the WWHD code.

### d_a_npc_tc (Tingle and brothers) — verified
- Gameplay: the blue variant appears only after the chests on all five Tingle-statue islands are opened (GameCube used a single event bit).
- Gameplay: the wallet-size messages are gone (as in the Japanese version), probably because HD's wallet handling differs.
- Fix: a walking speed that was uninitialised on GameCube in some event cuts now uses the clamp value 0.5.
- Graphics: during the rescue scene an environment-light override fades out after about 200 frames (HD-only counter); one material set for all four characters; no shadow.
- Structure: class 0x990; HD-only frame counter at 0x7F4.

### d_a_npc_ym1 (Mesa and Abe, Outset) — verified
- Gameplay: one setup branch now runs only in the stage "Demo46".
- HD-only: an NPC placed at one specific Outset position is moved to a slightly different, globally offset position (probably a placement fix for HD geometry).
- Graphics: one eye btp for all variants; no blob shadow.
- Structure: class 0xA24.

### d_a_npc_yw1 (Sue-Belle, Outset) — verified
- Graphics: her face uses a single texture-pattern resource whatever expression number is requested; no shadow (the GameCube decompilation is mostly stubs, so these are probably but not certainly HD changes).
- Structure: class 0x940; written mostly from the WWHD code.

### d_a_npc_zk1 (Zuko, Dragon Roost postman) — verified
- Graphics: no shadow drawing.
- Structure: class 0x924 (GameCube 0x7B4); event_actionInit and event_action are empty; archive name copied into the actor. The GameCube decompilation has only stubs, so this unit was written from the WWHD code.

### d_a_npc_zl1 (Tetra) — addendum, verified
- Graphics: her two light influences use different colours and powers than on GameCube; the darkening effect is skipped in the stage "Hyrule"; in the cutscene stage "Demo37" her water ripple is shown regardless of the water check.
- Gameplay: stage-specific cutscene tweaks: in "Demo17" after frame 0x606 her animation runs at 1.5x and a demo-actor flag is cleared; type 6 hides Tetra during frame ranges of "Demo05" and "Demo23" (probably to fit retimed HD cutscenes).
- Graphics: the head and eye materials are found by name (eyes, eyebrows, "zelda_zhead" pieces) instead of fixed joint meshes, and a second texture-pattern animation drives the mouth; no material-animation sharing, no shadow.
- Structure: class 0xD74; light influence 4 bytes larger; two material indices at 0xB0C/0xB10.

### d_a_obj_adnno (unused Triforce prayer mural) — verified
- Removed: Draw no longer draws anything; GameCube drew up to 16 panels whose material depended on event flags. HD only switches the draw list and back.
- Removed: set_mtx still computes each panel's matrix but no longer stores it into the models (probably leftover after the drawing was cut). The object is unused in both versions.

### d_a_obj_barrier (Hyrule Castle barrier) — verified
- Graphics: the main barrier model is skipped while its colour-animation frame is at or below 1 (probably fully faded); GameCube always drew it when the barrier was active.
- Structure: class 0xE00; HD animation containers are much larger than on GameCube.

### d_a_obj_buoyflag (flag on buoys / barrels) — verified
- Graphics: the cloth flag and its pole are drawn through HD GPU objects: double-buffered vertex buffers, shader/uniform blocks, and the "Cloth" texture with a sampler. GameCube drew them with display lists. The cloth simulation is unchanged.
- Structure: Packet_c 0x2C64 (GameCube 0xE00), Act_c 0x3230 (GameCube 0x112C). The GameCube decompilation has only stubs, so this unit was written from the WWHD code.

### d_a_obj_demo_barrel (Forsaken Fortress escape barrel) — verified
- Fix: the demo actor is checked for null before use (GameCube dereferenced it unchecked).

### d_a_obj_doguu (Goddess statues on the Triangle Islands) — verified
- Graphics: the specular reflection matrix is handed to the HD materials through a per-model effect matrix with texture-matrix dirty bits, instead of being written into each material's texture matrix.
- Graphics: the per-frame swap of the material table (by statue variant) is no longer done in Draw; probably done once when the models are created.
- Structure: class 0xBF0 (GameCube 0x900).

### d_a_obj_dragonhead (dragon head statue that fades out) — verified
- Graphics: besides the TEV konst alpha, the fade colour is also written as floats into the HD material's constant-colour uniform (with a dirty flag), so the HD shader sees the fade.
- Structure: class 0x568.

### d_a_obj_gong (gong on Tetra's ship) — verified
- Graphics: no skin deformation for the model; the specular map is applied to the model instance instead of its data.

### d_a_obj_homensmoke (Wind Temple wall smoke and rubble) — verified
- Graphics: HD particle ids for the smoke and debris; the parent's lighting is copied member by member, and some HD members are not copied.
- Structure: virtual Act_c destructor.

### d_a_obj_hsehi1 (Tower of the Gods Command Melody monument) — verified
- Graphics: the real (projected) shadow is no longer drawn; HD only updates and draws the model.
- Structure: class 0x648.

### d_a_obj_ice (ice block over chests, enemies and rupees) — verified
- Graphics: the draw list is chosen at runtime: during one specific event the block goes into the normal list, otherwise into a separate list; GameCube always used the mask-off list. Probably keeps the block visible correctly in a cutscene.
- Graphics: the fade-out alpha is written through the HD material's shader variable table and marked dirty, instead of only setting the TEV konst colour; the reflection matrix uses the HD effect-matrix path.
- Structure: class 0x5D8.

### d_a_obj_iceisland (icy clouds around Ice Ring Isle) — verified
- Graphics: the cloud particle tint is computed colour-space aware: the ambient colour is converted to sRGB, halved and offset as on GameCube, then converted back to linear (HD-only toSrgb/toLinear helpers). On GameCube the raw value was used.
- Structure: class 0x6F4 (GameCube 0x3A0), mostly larger animation and lighting members.

### d_a_obj_ikada (sea rafts, salvage ships and Beedle's shop ship) — verified
- Graphics: HD-only water-surface patch (drawSea, 02363374): every ship type loads an extra model with a scrolling texture animation. Each frame it is placed at the water level, tilted to match the hull's waterline and tinted with the current sea colour. Probably this keeps the HD ocean from showing through the inside of the hull.
- Removed: the orange glow sphere that GameCube draws around the ship's fire/lantern particle is not drawn in HD.
- Gameplay: the player-distance checks (Beedle's ship stopping near the player, and the far-away removal check at 18000 units) use a new helper (023628CC). It takes the smaller of the distance to Link and the distance to a second play actor (play+0x5B3C, probably the boat), and it measures in 3D, where GameCube used the XZ distance to Link only.
- Structure: class 0x16A0; resources for the HD sea patch set up once per type in a static helper (02363168).

### d_a_obj_jump (iron-boots springboard) — verified
- Graphics: no blob shadow; GameCube draws one for the small type, which also needed a ground check.
- Structure: class 0x41C.

### d_a_obj_leaves (Deku Leaf pile of foliage) — verified
- Graphics: the fade-out no longer switches materials to the GameCube blend/Z-mode setup. HD calls its own translucency helper (025F0AD0) and also writes the konst colour (with the fading alpha) into the HD shader's float colour override, converted to linear colour.
- Structure: class 0x568; GameCube's unused checkCollision/registFireCollision/retire-without-particle code is absent.

### d_a_obj_movebox (pushable boxes and blocks) — verified
- Graphics: no blob shadow, which GameCube draws from the box's ground data when the type has one.
- Structure: Bgc_c 0x184, Attr_c 0x9C; static data and constructors in d_a_obj_movebox_static.cpp.

### d_a_obj_ospbox (surprise box) — verified
- Graphics: no blob shadow.
- Structure: class 0x5B4; vtable helpers in d_a_obj_ospbox_edges.cpp.

### d_a_obj_otble (brown wooden tables) — verified
- Graphics: no blob shadow.
- Removed: the attribute checks that could disable drawing (a debug switch on GameCube).
- Structure: class 0x5F4 (actor base +0x11C).

### d_a_obj_pirateship (Tetra's pirate ship) — verified
- Graphics: HD-only water-surface model ("kaizoku_minamo.bdl" with its .btk scroll), drawn under the ship and tinted with the current sea colour (converted to a linear float shader colour). It is skipped when the start stage is "Demo37". Probably this hides the HD ocean inside the hull.
- Structure: class 0x864 (MoveBG base 0x3DC); second model and texture animation added.

### d_a_obj_smplbg (Tingle Tower head) — verified
- Graphics: the far cull distance is set from a global value (40000 plus a scaled factor) instead of the GameCube attribute table, probably extending its draw distance.
- Structure: the one-entry action table became a direct call.

### d_a_obj_swlight (moon/sun light switch) — verified
- Graphics: the face/mirror fade alpha is also written into the HD shader's float colour override (linear colour), and translucency is toggled per material when alpha < 255.
- Structure: class 0x1160.

### d_a_obj_tide (rising/falling water, "Gmtw" variant) — verified
- Gameplay: in the switch-raised "Gmtw" mode the water rises to home height + 2923.5 in HD. On GameCube it is + 2948.5, so the raised water level is 25 units lower in HD.
- Structure: class 0x428; tide sound handling split into separate start/stop helpers.

### d_a_obj_vfan (Ganon's Tower barrier fan) — verified
- Graphics: uses HD particle ids 0x83CD–0x83D7 when it breaks.
- Structure: the parameter reader is out of line, one copy per unit.

### d_a_obj_warpt (warp pot) — verified
- Graphics: custom smoke particle colours (prim/env) are converted from sRGB to linear (power 2.2) before they go to the emitter.
- Graphics: no blob shadow.
- Structure: class 0x9F0; animation controller and Acch/circle layout differ.

### d_a_obj_zouK (Triforce statue) — verified
- Graphics: no simple shadow and no ground check (fields removed); the draw also stores the effect matrix in the model.

### d_a_oq (Octorok) — verified
- Gameplay: in one stage the Octorok of room 0x29 is not created once that stage's boss is beaten (marked as an HD change; the GameCube decompilation of this unit is mostly stubs, so this is probably but not certainly HD-only).
- Structure: class 0x1194; ice/fire damage-reaction blocks grew (enemyfire 0x22C); mostly written from the WWHD code.

### d_a_player (player base and particle callback) — scoped verified
- Base size grows from 0x320 to 0x43C, with inspected fields shifted by 0x11C. The matrix-follow callback resets its emitter; room matching, the 60-frame button quake and wind response retain the inspected behavior. HD uses a warm singleton getter and a read-only quake pattern. No gameplay change was established from the layout differences.
- All 42 owned entries pass canonical and bounded actual-helper O1/O2 controls at 10,000 inputs each, covering 68/68 blocks. The 405-case inventory contains 244 verifier-contract differences, 110 compile failures, 49 scoped equivalents and two invalid accesses. Twenty-three additional missing-return probes fail the strict compiler check and receive no runtime credit. Adding flag value 1 exposed two previously surviving guard mutations; 167 affected cases were replayed, with 238 unaffected historical cases carried explicitly.
- Scope requires initialized typed objects, finite math, disjoint private frames and warm getters. The selected quake path uses the motor; its direction argument remains an API-contract observation. Live particle allocation, callback dispatch, derived-player construction and lifecycle producer graphs remain open.

### d_a_ph (Peahat / Seahat) — verified
- Graphics: no shadow is drawn (only a leftover of the shadow code remains).
- HD-only: when a boomerang (or similar hit) stops the propeller, a global byte at 0x101EACB7 is set and an extra function (025E1FD8) is called; role unknown, probably feedback for another system (e.g. HUD or controller rumble).
- Structure: class 0x12D8, layout shift +0x11C, enemy-fire block 4 bytes larger; written from the WWHD code.

### d_a_salvage (salvage point light pillars) — verified
- Graphics: the per-point fading alpha is also written into the HD shader's float colour override (linear colour), not only into the GX konst colour.
- Removed: GameCube's debug draw call at the start of Draw.
- Structure: class 0x42C; salvage registry is shared static data.

### d_a_sk2 (swaying lily-pad lift, Forbidden Woods) — verified
- HD-only: the deforming collision is driven by CPU vertex deformation from a "<model>.cvtx" resource instead of passing the animated model to the collision; the sway motion is updated once more in Execute.
- Structure: the actor is constructed before the resource load (as in the USA version).

### d_a_ssk (stone tentacle) — verified
- Gameplay: the rock fades in and out through a new wobble amplitude, and its collider is active only while a new "active" flag is set; start angles are random instead of a stored wobble direction; both retract paths play the sound and smoke.
- Graphics: the tentacle grows by translation instead of base scale; each model is drawn only while visible.
- Structure: class 0x7EC; wobble-direction and joint-position arrays removed.

### d_a_swc00 (switch trigger cylinder) — verified
- HD-only: a global byte (0x101D5F45) is tested first; while set, the trigger acts as if the player were outside. Role unknown.

### d_a_swhit0 (crystal switch) — verified
- Graphics: the material colour is written into the HD material colour block as floats, not into GX colour registers.
- Structure: the timer getter is called once; the explicit destructor call is removed.

### d_a_tbox (treasure chest) — verified
- Graphics: for the "env" chest (the one that appears with a distortion effect), the placeholder texture "__dummy" in Dalways is replaced by an HD graphics-system image (texture slot 6 of the renderer) instead of the GameCube frame-buffer copy, and the model texture patch step is gone.
- Graphics: the invisible chest's shimmer is driven through named shader parameters ("indirect_mtx0a"/"indirect_mtx0b") instead of GX indirect-texture matrices.
- Structure: class 0xB08; the texture code moved to its own part file (d_a_tbox_env.cpp).

### d_a_tsubo (pots, skulls, barrels and other small carriables) — verified
- Graphics: pot-shatter particles swap primary and environment colours (probably to match the HD particle shading); type 7 sets a flag on one named material; no simple shadow.
- Fix: the object type and the item action are clamped to 0..15 and the break-effect index is asserted (GameCube indexed tables without bounds); one GameCube one-past-the-end read is kept.
- Structure: class 0x93C; environment process id 0x17 (GameCube 0x19).

### d_a_vrbox / d_a_vrbox2 (sky) — addendum, verified
- Graphics: the sky is drawn through a dedicated HD sky layer with its own projection (near 0.0001, far 16000); sky colours go to materials looked up by name plus an extra HD colour register scaled by the environment light.
- Graphics: stage-specific offsets: the Forest Haven ("A_mori") sky is raised by 30000; on "sea_E" the sky is lifted with wider fields of view (probably for the widescreen / GamePad view); in "M_NewD2" and "Siren" the false-sea colour is taken from the HD layer's clear colour.
- Graphics: one cloud layer is scrolled by another layer's animation at full speed; scroll speed is computed in Execute and applied in Draw.

### d_a_ykgr (Dragon Roost heat haze) — verified
- Graphics: the distortion is drawn in one GX2 indirect-matrix call. Its matrix value is halved once more than on GameCube, so the haze is probably weaker, or the halving makes up for different HD scaling.
- Structure: daYkgr_c 0x3F0.

## Structure-only units

### d_a_alldie (switch when all room enemies are dead) — verified
- Structure: action byte at 0x3AC (GameCube 0x290), layout +0x11C; virtual destructor.

### d_a_andsw0 (switch AND logic) — verified
- Structure: no behaviour change; only the HD layout of the read Bokoblin field.

### d_a_bita (Gohma fight wooden platforms) — verified
- Structure: class 0x568; only signature and inlining changes.

### d_a_boss_item (boss heart container drop) — verified
- Structure: class 0x3B4; unchanged apart from the HD constructor.

### d_a_demo_dk (Valoo demo model) — verified
- Structure: class 0x3EC; resources released with the normal delete instead of the demo delete.

### d_a_disappear (enemy death puff) — verified
- Structure: class 0x3BC; no behavioural differences.

### d_a_dr (Valoo, overworld) — verified
- Structure: class 0x3E8; movement inlined into Execute, no behavioural differences found.

### d_a_dummy (dummy actor) — verified
- Structure: class 0x3E4; only layout and virtual destructor.

### d_a_fallrock_tag (falling rock spawner) — verified
- Structure: daFallRockTag_c 0x3BC; no dynamic module link/unlink (no cDyl in HD); table data folded into constants.

### d_a_hot_floor (heat-floor particles) — verified
- Structure: class 0x5B4; brk/btk animation members grew (0x78/0x74).

### d_a_jbo (Baba Bud) — verified
- Structure: jbo_class 0x554, layout +0x11C; jbo_move inlined into Execute.

### d_a_klft (Forbidden Woods lift) — verified
- Structure: GameCube source is only stubs; 14 functions written from the WWHD code.

### d_a_kn (crab) — verified
- Structure: kn_class 0x658; keeps the USA stop-on-wall behaviour; HD archive order.

### d_a_knob00 (regular doors with knobs) — verified
- Structure: class 0x5AC; behaviour, including the pirate-ship door check, matches GameCube.

### d_a_kokiie (Forbidden Woods hanging flower house) — verified
- Structure: kokiie_class 0x4C0 (layout +0x11C); keeps the USA lower bound (-1500); debug registers kept.

### d_a_komore (foliage light shafts) — verified
- Structure: class 0x45C; btk animation block 0x74 in HD.

### d_a_kt (small bird / insect "kt") — verified
- Structure: class 0x420; the GameCube frame-control fields at 0x29C..0x2B8 are gone (+0x100 from the wing model on).

### d_a_kytag01 (wave influence tag) — verified
- Structure: only environment-light offsets and the stage-name comparison changed.

### d_a_kytag02 (wind rail tag) — scoped verified
- The wind-rail behavior is retained: select the nearest XZ path point, take the next-point direction, and scale strength by 1/100. Point argument 3 falls back to path argument 0 when it is 0xFF. Deletion clears the wind override. No gameplay change was established.
- Actor size grows from 0x2A0 to 0x3BC. Path and wind fields move from 0x290/0x294 to 0x3AC/0x3B0; HD uses singleton getters and includes static initialization and deleting-destructor glue. The endpoint comparison still uses the initial path's point count even when a later path wins, matching the GameCube behavior.
- Verification covers initialized finite path graphs with valid segment pairs, disjoint private storage, warm getters and finite math. Seven entries pass O1/O2 at 10,000 inputs each, including five bounded actual-getter/math companion profiles. The 283-case ledger contains 136 verifier-contract differences, 45 scoped equivalents, 98 compile failures and four invalid protocol cases. Cold manager initialization, live lookup/resource lifetimes and unrestricted mixed-count paths remain outside this claim.

### d_a_kytag03 (contrast switch tag) — verified
- Structure: class 0x3C8; resources released with the normal delete instead of the demo delete.

### d_a_kytag04 (colour pattern switch tag) — verified
- Structure: kytag04_class 0x3C0, layout +0x11C; otherwise unchanged.

### d_a_kytag06 (rain/thunder event tag) — verified
- Structure: kytag06_class 0x3B4, layout +0x11C; otherwise unchanged.

### d_a_lamp (wall lamp) — verified
- Structure: class 0x584; the point-light block is 4 bytes larger; heap callback folded into the create function.

### d_a_lwood (tree) — verified
- Structure: the leaf joint is looked up by name (GameCube compares strings over every joint); the wind-speed helper returns a pointer to a static.

### d_a_magma (magma floor registration) — verified
- Structure: class 0x3B4; the destructor became virtual.

### d_a_mflft (Forbidden Woods rope-hung platform) — verified
- Structure: mflft_class 0x9E4; HD 3D-line material 0x188 (GameCube 0x3C).

### d_a_mmusic (Makar's music) — verified
- Structure: no behavioural differences found; HD virtual destructor.

### d_a_msw (swinging chain platform) — verified
- Structure: class 0x99C; layout shift only.

### d_a_npc_bms1 (Bomb-Master Cannon, Windfall bomb shop) — verified
- Structure: head-animation helper 0x20 (GameCube 0x22); shop camera 0x50 with 8-byte pointer-to-member; layout +0x11C then +0x170/+0x168. The GameCube decompilation has only stubs.

### d_a_npc_kk1 (Mila, poor) — verified
- Structure: class 0x824 -> 0xAD0; GameCube source is only stubs, written from the WWHD code.

### d_a_npc_ob1 (Rose, Outset) — verified
- Structure: class 0x97C; GameCube decompilation is empty, written from the WWHD code.

### d_a_nzg (rat hole) — verified
- Structure: class 0x574; resources released with resDelete instead of resDeleteDemo.

### d_a_obj_AjavW (water in Jabun's cave) — verified
- Structure: no background collision is created when the model is missing.

### d_a_obj_akabe (invisible collision wall) — verified
- Structure: Act_c 0x3F0; same behaviour as GameCube, heap sizes read from a table.

### d_a_obj_ashut (metal bars, Pirate Ship rope minigame) — verified
- Structure: Act_c 0x404; GameCube decompilation has only stubs, so the unit was written from the WWHD code.

### d_a_obj_auzu (Big Octo / Jabun whirlpool) — verified
- Structure: Act_c 0x43C (GameCube 0x2C0); about 0x60 more than the usual header growth, from the larger HD texture-animation object.

### d_a_obj_aygr (lookout tower with ladder) — verified
- Structure: the GameCube decompilation has only stubs, so this unit was written entirely from the WWHD code.

### d_a_obj_balancelift (Tower of the Gods scales) — verified
- Structure: class 0x770; GameCube decompilation has mostly stubs, so the unit was written from the WWHD code.

### d_a_obj_barrel2 (floating barrel and sea mine) — verified
- Structure: Act_c 0x5C4; same modes, effects and item handling as GameCube.

### d_a_obj_bscurtain (Beedle's shop curtain) — verified
- Structure: class 0x3BC; HIO block 16 bytes; drawing unchanged.

### d_a_obj_buoyrace (race buoy) — verified
- Structure: Act_c 0x3F8 (GameCube 0x2DC, only the usual header growth).

### d_a_obj_cafelmp (Cafe Bar lamp) — verified
- Structure: class 0x3B8; drawing unchanged.

### d_a_obj_coming (floating barrel spawner) — verified

- Structure: The actor grows from 0x2D4 bytes on GameCube to 0x3F0 bytes in HD. Its five controller records retain their size and mode, timer and process-identifier fields while moving with the larger actor base.

### d_a_obj_correct (puzzle solution trigger) — verified
- Structure: Act_c 0x3C4 (GameCube 0x2A8, only the usual header growth).

### d_a_obj_dmgroom (damaged room demo object) — verified
- Structure: HD brk animation 0x78; model pointer tested directly.

### d_a_obj_doguu_demo (Goddess statue, cutscene version) — verified
- Structure: Act_c 0x3F4; drawing unchanged.

### d_a_obj_drift (floating tree platform) — verified
- Structure: Act_c 0x5F4; flower and current logic unchanged.

### d_a_obj_eayogn (bridge) — verified
- Structure: Act_c 0x3BC; drawing unchanged.

### d_a_obj_ebomzo (bird statue knocked over by bombs) — verified
- Structure: no behavioural differences found.

### d_a_obj_eff (smoke effects for barrels, stools, skulls, pine cones, boxes) — verified
- Structure: Act_c 0x3B4, smoke callback 0x24; same six effect types.

### d_a_obj_ekskz (Gale Isle wind statue) — verified
- Structure: class 0x930; GameCube source is only stubs, written from the WWHD code.

### d_a_obj_ferris (Windfall Ferris wheel) — verified
- Structure: Act_c 0x1A94; drawing of the six parts unchanged.

### d_a_obj_flame (flame pillar) — verified
- Structure: Act_c 0x61C; GameCube decompilation has only stubs, so the unit was written from the WWHD code.

### d_a_obj_ganonbed (bed, Puppet Ganon intro) — verified
- Structure: class 0x3BC; drawing unchanged.

### d_a_obj_gaship (Forsaken Fortress Ganon room exterior) — verified
- Structure: Act_c 0x3EC; drawing unchanged.

### d_a_obj_gaship2 (destroyed Ganon room, Forsaken Fortress 3) — verified
- Structure: Act_c 0x3EC; drawing unchanged.

### d_a_obj_gnnbtltaki (Ganondorf fight background waterfalls, during fight) — verified
- Structure: class 0x430; texture-animation object 0x74 in HD.

### d_a_obj_gnndemotakie (Ganondorf fight central waterfall, after fight) — verified
- Structure: class 0x42C (GameCube 0x2B0), extra growth from the larger HD texture-animation object.

### d_a_obj_gnndemotakis (Ganondorf fight background waterfalls, before fight) — verified
- Structure: class 0x430 (GameCube 0x2B4), extra growth from the larger HD texture-animation object.

### d_a_obj_gryw00 (Dragon Roost water surface) — verified
- Structure: class 0x55C; water-level and particle logic unchanged.

### d_a_obj_gtaki (Ganon's tower waterfall) — verified

- Graphics: HD replaces the effect's dummy texture using a framebuffer-backed texture descriptor and texture-enable bookkeeping. GameCube uses texture-image replacement followed by model texture patching. Both versions use a framebuffer-backed effect; this comparison does not establish a visible appearance change.

### d_a_obj_hami4 (four sliding gate panels) — verified
- Structure: class 0x49C (GameCube 0x380, only the usual header growth).

### d_a_obj_hat (traveling merchants' hats) — verified
- Structure: class 0x76C; drawing unchanged.

### d_a_obj_hbrf1 (Wind Temple fan-room elevator) — verified
- Structure: Act_c 0x400; same up/down demo states.

### d_a_obj_hcbh (Tower of the Gods pillar with Companion Statue face) — verified
- Structure: class 0xEF0; GameCube decompilation has only stubs, so the unit was written from the WWHD code.

### d_a_obj_hlift (wooden platforms, Pirate Ship rope minigame) — verified
- Structure: Act_c 0x414 (GameCube 0x2FC); same modes and vibration.

### d_a_obj_htetu1 (Tower of the Gods yellow gate) — verified
- Structure: class 0x444 (splash helper 0x2C); GameCube decompilation has only stubs, so the unit was written from the WWHD code.

### d_a_obj_Itnak (unused Darknut statue) — verified
- Structure: class 0x834; GameCube decompilation is stubs, written from the WWHD code.

### d_a_obj_kanat (Forbidden Woods vine floor) — verified
- Structure: class 0x410; parameter helper out of line per unit.

### d_a_obj_kanoke (Earth Temple coffin) — verified
- Structure: the GameCube decompilation has only stubs for this unit; written entirely from the WWHD code (class 0x9AC).

### d_a_obj_ladder (drop-down ladder) — verified
- Structure: class 0x460; no behavioural differences found.

### d_a_obj_light (lighthouse / Ferris wheel light) — verified
- Structure: the GameCube decompilation has only stubs for this unit; written entirely from the WWHD code.

### d_a_obj_lpalm (palm tree) — verified
- Structure: class 0x438; the wind sway is unchanged; the collision uses its own matrix separate from the model; GameCube's attribute flags that could skip execute/draw are gone.

### d_a_obj_magmarock (Dragon Roost lava slab) — verified
- Structure: the GameCube decompilation has only stubs for this unit; written entirely from the WWHD code (class 0x768).

### d_a_obj_majyuu_door (Forsaken Fortress wooden barricade) — verified

- Structure: The actor grows from 0xF30 bytes on GameCube to 0x104C bytes in HD. Stored fields and collision components move by 0x11C bytes, while the ten collision cylinders retain their spacing. These layout changes do not by themselves establish a gameplay change.

### d_a_obj_mkie (light-sensitive breakable statue) — verified
- Structure: class 0x10DC; same behaviour; also contains the MkieK wall (d_a_obj_mkiek.cpp).

### d_a_obj_mkiek (light-vanishing wall) — verified
- Structure: class 0x5DC; brk animation block 0x78 in HD.

### d_a_obj_monument (Command Melody tablet) — verified
- Structure: class 0x3F0; the light type passed in Draw has a different number (probably the same BG type under HD's enum numbering).

### d_a_obj_msdan2 (step-row spawner) — verified
- Structure: class 0x3BC; no behavioural differences found.

### d_a_obj_msdan_sub (switch staircase step) — verified
- Structure: the heap check tests the created model pointer; otherwise layout only.

### d_a_obj_msdan_sub2 (switch-driven sliding block) — verified
- Structure: written from the WWHD code following the sibling unit; layout only.

### d_a_obj_mshokki (Forsaken Fortress dishes) — verified
- Structure: class 0x73C; same behaviour; per-dish data read from a table.

### d_a_obj_mtest (model/collision test objects) — verified
- Structure: class 0x590; same behaviour.

### d_a_obj_nest (Kargaroc nest) — verified
- Structure: vibration fields at 0x3EC–0x3FE (layout +0x118); leftover background call removed.

### d_a_obj_ohatch (Nintendo Gallery hatch) — verified
- Structure: class 0x40C; same behaviour, with the HIO tuning values fixed as constants.

### d_a_obj_ojtree (Forest Haven Deku Leaf poles) — verified
- Structure: layout shift only (model at 0x3E8).

### d_a_obj_paper (readable notice board) — verified
- Structure: class 0x534; same behaviour.

### d_a_obj_pbco (bomb-shop counter flap) — verified
- Structure: class 0x3EC; same behaviour.

### d_a_obj_pbka (Windfall bomb shop ceiling fan) — verified
- Structure: layout +0x11C only; model pointer tested directly.

### d_a_obj_pfall (rat-operated trapdoor) — verified
- Structure: class 0x6E8; same behaviour; the rat's material shapes are looked up by name instead of by index.

### d_a_obj_quake (earthquake trigger) — verified
- Structure: class 0x3C0; same behaviour.

### d_a_obj_rcloud (Dragon Roost clouds) — verified
- Structure: class 0x444; same behaviour.

### d_a_obj_rforce (rotating-force platform) — verified
- Structure: class 0x3EC; same behaviour.

### d_a_obj_roten (Zunari's shop stall) — verified
- Structure: class 0x3F0; same behaviour.

### d_a_obj_shelf (wooden shelf) — verified
- Structure: class 0x400; same behaviour.

### d_a_obj_shmrgrd (Spiked Skull Hammer switch) — verified
- Structure: class 0x9F0; same behaviour.

### d_a_obj_swflat (Tower of the Gods statue floor switch) — verified
- Structure: the GameCube decompilation has only stubs for most of this unit; written from the WWHD code (class 0x584).

### d_a_obj_swhammer (hammer switch) — verified
- Structure: class 0x718; uses the final-release actor lighting type (GameCube's demo-build selector is resolved).

### d_a_obj_swheavy (iron-boots floor switch) — verified
- Structure: class 0x45C; same behaviour.

### d_a_obj_swpush (yellow floor switch) — verified
- Structure: class 0x4B4; same behaviour.

### d_a_obj_tenmado (two-leaf shutter window) — verified
- Structure: layout shift only.

### d_a_obj_timer (switch timer) — verified
- Structure: class 0x3B8; same behaviour.

### d_a_obj_tousekiki (catapult on Tetra's ship) — verified
- Structure: class 0x55C (layout +0x11C); the actor is constructed before its resources load (as in the USA build); the offset vector is a function-local static.

### d_a_obj_tower (Tower of the Gods exterior) — verified
- Structure: class 0x3F0; same behaviour.

### d_a_obj_trap (Blade Trap) — verified
- Structure: the GameCube decompilation has only stubs for this unit; written entirely from the WWHD code.

### d_a_obj_usovmc (box with background collision) — verified
- Structure: layout shift only (model at 0x3E8).

### d_a_obj_vmsdz (Master Sword pedestal) — verified
- Structure: class 0x3B8; same behaviour.

### d_a_obj_vmsms (Master Sword in the pedestal) — verified
- Structure: class 0x3B8; same behaviour.

### d_a_obj_wood (tree unit for the wood packet) — verified
- Structure: class 0x524; the wood packet is created through the play object.

### d_a_obj_xfuta (lid object) — verified
- Structure: class 0x3E8; same behaviour.

### d_a_pt (Miniblin) — verified
- Structure: pt_class 0xEE4; enemyfire 0x22C (one HD float more); layout +0x11C, then +0x118 from the collision members. The GameCube decompilation has only stubs, so the AI was written from the WWHD code.

### d_a_race_item (Barrel Shoot / race reward items) — verified
- Structure: class 0x75C; same behaviour; static helpers in d_a_race_item_static.cpp.

### d_a_rectangle (unused dummy actor) — verified
- Structure: unchanged apart from the HD method table.

### d_a_sbox (salvaged treasure chest) — verified
- Structure: class 0x62C; same behaviour; light and animation layouts differ.

### d_a_scene_change (scene change marker) — verified
- Structure: transform matrix moved to 0x3AC (GameCube 0x290).

### d_a_seatag (sea marker tag) — verified
- Structure: the debug create/delete id registration is removed.

### d_a_shand (Forbidden Woods ceiling tentacle) — verified
- Structure: HD sound id; otherwise layout only.

### d_a_sitem (enemy item on a chain) — verified
- Structure: class 0x10C4; GameCube decompilation is stubs, written from the WWHD code (an unused static vector of unknown role).

### d_a_ss (enemy core with ten 3D-line tentacles) — verified
- Structure: ss_class 0x3E20. The GameCube decompilation has only stubs, so this unit was written entirely from the WWHD code.

### d_a_steam_tag (steam vent tag) — verified
- Structure: the steam particle is set through the normal particle group 2 call; HD particle ids.

### d_a_swattack (switch hit by a given attack type) — verified
- Structure: daSwAt_c 0x52C, layout +0x11C; otherwise unchanged.

### d_a_switem (item drop when hit by an attack type) — verified
- Structure: daSwItem_c 0x528, layout +0x11C; otherwise unchanged.

### d_a_swtdoor (Forsaken Fortress tower window doors) — verified
- Structure: resource release uses the normal delete instead of the demo variant.

### d_a_syan (unused chandelier) — verified
- Structure: class 0x6D4 (GameCube 0x5B8), +0x11C throughout.

### d_a_tag_attention (attention tag) — verified
- Structure: class 0x520 (+0x11C); no behavioural differences found.

### d_a_tag_ba1 (grandma fairy-bottle tag) — verified
- Structure: class 0x3B0; parameter table is plain data without a guard.

### d_a_tag_evsw (event bit to switch) — verified
- Structure: class 0x524; layout only.

### d_a_tag_kb_item (item dug up by an enemy) — verified
- Structure: no behavioural differences found.

### d_a_tag_kk1 (Mila chase start tag) — verified
- Structure: class 0x7E0; layout only.

### d_a_tag_ret (Deku Leaf return tag) — verified
- Structure: class 0x524; layout only.

### d_a_tag_so (seagull/jump area tag) — verified
- Structure: debug colours became function-local statics; HIO 0x10.

### d_a_tag_volcano (Fire Mountain timer tag) — verified
- Structure: class 0x3C4; same behaviour, with the timer handling split into small helpers.

### d_a_tama (NPC projectile) — verified
- Structure: no HD-specific changes beyond layout.

### d_a_tpota (waterfall basin splash) — verified
- Structure: the particle-list walk only stops at a NULL link.

### d_a_warpmj (Forsaken Fortress warp portal) — verified, 2026-10-03
- Structure: The reconstructed HD actor occupies 0x5AC bytes and uses temporary SafeString objects for resource names and singleton accessors for environment and play state. Its audited implementation includes actor destruction and SafeString virtual helpers beyond the nominal actor-method inventory.
- Comparison limit: The available GameCube substantive method bodies are nonmatching placeholders. The verified portal, animation and event behavior establishes HD behavior, but does not establish a gameplay or graphics change between versions. The HD layout alone does not prove a size change.

### d_a_sss (Dexivine grabbing vine) — verified, 2026-10-03
- Gameplay: the cut vine's wobble uses subtracted per-segment phase terms (GameCube adds them), so it probably sways in the opposite pattern.
- Gameplay: two debug registers are still read (the hand's tilt offset and the pull-strength factor); they are probably zero in the shipped game.
- Graphics: particle positions set by the vine flip their height sign for newer emitter versions (HD particle convention, probably).
- Structure: layout shift (+0x11C); the 3D line material objects are much larger in HD; the control/cut-control helpers are all inlined into one hand_move.

### d_a_obj_vtil (Tingle statues) — verified, 2026-10-03
- Structure: The inherited actor base grows from 0x290 bytes in GameCube to 0x3AC in HD. HD uses four-byte tail-branch method callbacks instead of the larger GameCube callback bodies, and emits separate statue lifetime helpers.
- Removed: The GameCube symbol list includes separate tell_agb_attack and tell_agb_sink hooks, with no separate counterparts in the complete HD actor unit. This is probably related to removing Tingle Tuner communication; inlining could also obscure individual hooks.
- Comparison limit: The available GameCube behavior bodies and actor member layout are incomplete, so this comparison does not establish other gameplay changes or a complete actor-size difference.

### d_a_obj_canon (Great Sea enemy cannon) — 36 functions verified

- Fix: firing checks whether the bomb actor was created. If creation fails, HD skips the bomb setup, recoil and firing sound; the GameCube source assumes a bomb was returned.
- Graphics: the joint callback and muzzle-position calculation obtain animation matrices through a model matrix buffer and mark that buffer dirty. The GameCube model stores and accesses the joint matrices directly. This is probably part of the HD model pipeline.
- HD-only: the debug-draw function performs lazy initialization of a small color object; the GameCube function is empty. Neither version draws debug geometry in the examined function.
- Structure: resource lookup uses a temporary SafeString object. Play, save and environment-light state are reached through HD accessors or singleton pointers. The larger actor base and changed collision representation alter the member layout. The aiming, damage handling and mode transitions retain the GameCube behavior in the examined code.


### d_a_obj_Yboil (boiling sea object) — verified, 2026-10-03

- Graphics: HD also updates normalized RGBA shader buffers after changing ambient and diffuse material colors. Alpha comes from the material. The GameCube version updates the material color registers.
- Structure: HD retains 50 models but expands the actor from 0x1498 to 0x536C bytes as its animation wrappers grow.

### d_a_obj_barrel (barrel) — verified, 2026-10-03

- Removed / Graphics: The HD barrel draw routine omits the actor’s optional distance cutoff and simple ground-shadow submission used by the GameCube routine. It always sets up lighting and draws the barrel model. This finding concerns the actor’s draw path; other rendering systems may supply a shadow.
- Structure: The collision objects and movement-state tail retain their relative layout after the enlarged actor base. Mode dispatch uses eight-byte member-function descriptors, and stage-name/resource access uses HD string and accessor interfaces.

### d_a_obj_hha (Tower of the Gods entrance waterfall) — verified, 2026-10-03
- Structure: HD stores part callbacks in smaller member-function descriptors while its texture animation controllers and lighting state are larger. The part object shrinks from 0x4C to 0x44, the gush object expands from 0x104 to 0x2F8, and the complete actor expands from 0x7C4 to 0xB84.
- Structure: the main waterfall texture-animation entry calls take the model directly in HD; the GameCube source passes its model data. The gush animation paths continue to use model data.

The audited HD state transitions, opening/closing timers, splash activation, cylinder/sphere use, and lighting categories agree with the retail GameCube reference. No confirmed gameplay, visual-result, removed-feature, or crash-fix change is claimed by this comparison. These observations describe version differences, not verification repairs.

### d_a_obj_figure (figurine gallery) — verified, 2026-10-03
- Gameplay: HD applies pedestal drawing and collision checks to figurine 0x32; the GameCube source excludes it as well as 0x40. Viewing-delay/controller updates run before message-state dispatch and camera updates after it, rather than inside the initial viewing state.
- Removed: The audited HD path omits the GameCube automatic idle figurine rotation and the skin-deformer allocation/attachment for the animated 0x40 figurine.
- Structure: Gallery material visibility and transparency ordering are incorporated into actor draw rather than a separate helper.
- Graphics: The culling-box top is 450 in HD versus 200 in the GameCube source; this does not establish a model-scale change.

### d_a_warpls (warp light shaft) — verified, 2026-10-03
- Structure: The actor grows with the larger HD actor base; its phase, model, animation, emitter and event fields shift together. HD uses SafeString helpers and singleton accessors for resource and service handling.
- HD-only: Creation explicitly asserts that the extracted warp type is below two before selecting resource tables; the provided GameCube creation body lacks that assertion.

### d_a_toge (Wind Temple floor spikes) — verified, 2026-10-03
- Structure: layout shift only (every member +0x11C, class 0x5A4); HD virtual destructor and inline collider constructors in create; resource names through SafeString.
- Structure: the "toge" joint is found through the joint name table's index lookup instead of the GameCube loop of string compares (same result).
- Structure: the spike sound helper calls the sound start without the HD null checks seen in other actors' fopAcM_seStart.

### d_a_bridge (rope bridge) — verified, 2026-10-03
- Gameplay: the bridge logic is skipped entirely while an event runs and its rumble timer is active.
- Gameplay: a player flag (probably the Iron Boots) presses planks down harder and makes a rope bridge give way faster; another player flag (probably a heavy hit) breaks the plank under the player at once.
- Graphics: the rope tops no longer sit at a fixed height above the planks; their height follows a sine over the bridge length (lower in the middle), probably so the ropes sag; bridges joined to a partner draw one side-rope segment fewer when facing one way.
- Removed: the camera-distance check that stopped drawing far bridges behind the camera, and the culling-status change on the first update.
- Structure: the first plank never gets its own plank ropes; the random plank width now depends on a different index test; an extra assertion for the chain model; the chain-hit particle scale became a lazily initialised static; the wind sway angle is summed in floating point.

### d_a_obj_mknjd (Earth God's Lyric / Wind God's Aria statues) — verified, 2026-10-03
- Graphics: the fading of the intact statue no longer changes material blend/depth modes per frame; it only shows or hides the meshes (visible while alpha is non-zero), and no sky draw list is used.
- Graphics: an extra environment-light flag is set while the Wind statue's break demo runs in the Wind Temple ("kaze", room 12) and cleared when the statue is created or the demo ends (probably an HD lighting tweak for that cutscene).
- Gameplay: hiding/showing Medli also toggles a flag on an extra object attached to her (probably her HD-only harp or wing model).
- Fix: the halves' joints are found by name lookup instead of a name loop; the shard joint table and the shard joint callback are bounds checked; the "angle" event value is null-checked.
- Structure: the message handling uses the HD global message manager (the message pointer became a "found" flag); members shifted by the larger HD movable-background actor base (GameCube offset + 0x118).

### d_a_obj_bemos — verified, 2026-10-03
- Structure: Larger BRK animation controllers and revised particle-emitter fields expand the HD actor layout. State records use the HD compiler member-dispatch format. Tuning settings and defaults are unchanged, but the tuning object stores its virtual-table pointer after the settings rather than before them.

### d_a_fire (flame obstacle) — verified, 2026-10-03
- Structure: Actor fields shift with the larger HD actor base; collision-cylinder spacing remains unchanged.
- Removed: HD omits the GameCube call to an empty normal procedure and an unused local collision-center calculation.
- Graphics/Structure: Fire stopping selects deferred particle cleanup in HD: the emitter stores pending particle references and clears them after its countdown. The GameCube helper immediately returns active and child particles to the vacancy lists. The audited Fire suppression delay, effect IDs and collision sizes remain unchanged.

### d_a_windmill (windmill) — verified, 2026-10-03

- Structure: HD preserves both windmill types, the rotation-speed threshold between attack and contact collision, and the wind-dependent blade-speed calculation. Actor storage follows the enlarged HD base and the HD model, resource-string, environment-light and draw-list interfaces.
- Graphics / Structure: The HD joint callback explicitly marks animation-matrix storage dirty before accessing the blade joint matrix. Large windmills use vertical-axis rotation and small windmills use depth-axis rotation; both copy the matrix into the global current and background-collision matrices. No confirmed gameplay change was found in these routines.

### d_a_tag_msg (message trigger) — verified, 2026-10-03
- Structure: HD uses a global message controller and status getter/setter calls instead of retaining a GameCube message-object pointer. Its pointer-lookup state advances without the GameCube search-and-wait operation; no player-visible timing change is inferred. Actor fields shift with the larger HD base, and HD uses singleton/virtual service accessors.
- Removed: The HD delete callback returns success directly rather than invoking the GameCube tag destructor; a separate virtual destructor remains.

### d_a_obj_toripost (postbox) — verified, 2026-10-03

- Graphics: HD applies gamma conversion and clamping to the swallowed-letter particle colors, while GameCube passes the color bytes directly. HD also omits the simple shadow drawn by the GameCube actor.
- Removed: this HD unit omits the movement, collision correction, and matrix update calls made by the GameCube Execute function. Its initialization also omits the wall and collision setup and gravity assignment, although the corresponding members remain in its layout.
- Structure: the actor grows from 0x8F8 to 0xA10 bytes. HD reads message selection through the global message controller, and its tuning object places the vtable after its fields.

### d_a_sail (pirate ship's sail) — verified, 2026-10-03
- Graphics: the sail is no longer drawn with GameCube display lists; the packet uploads its positions and front/back normals into HD vertex buffers every frame, sets uniform colours from the ship's light settings and draws 11 index strips per face with a "flag_default" shader. The sail texture and the "Cloth" toon ramp are bound as GPU textures. The sail's matrix is no longer multiplied by the view matrix (the shader probably does it).
- Graphics: the stick (mast) model and the sail packet are entered into different draw buffers than on GameCube (the opaque/translucent lists are switched around them).
- Gameplay: in stage "Demo46" the sail's flutter follows the wind power (both the wave amplitude and the sideways bulge are scaled by it); elsewhere it behaves as on GameCube.
- Fix: Create checks that the pirate ship was found before reading its state (GameCube reads it unconditionally).
- Fix: the cache flush covers only the current position/normal buffers (as in the GameCube USA/PAL builds; the Japanese build flushed a far too large range).
- Structure: the packet grew from about 0x1C3C to 0x2EC0 bytes for the GPU objects; the members after the normals (stick model, wind phase, furl amount) now belong to the actor; the debug tuning offsets are gone (folded into the constants); the stage-name comparison uses an HD string class.

### d_a_bwdg (Molgera's sand floor) — verified, 2026-10-03
- Graphics: the floor is drawn through a new HD render packet with GPU vertex buffers and shader constants (view matrix, tev colours); the packet is filled and flipped every frame after its draw-list entry. Only one set of vertex/normal arrays is kept instead of two alternating ones.
- Graphics: surface normals are computed from the height difference to the next grid row instead of from the height above the base level; the rotated reference normal became a precomputed function-local value.
- Structure: GameCube layout shifted by 0x120 here (an HD field after the fopAc base holds a pointer to the actor itself, probably a back-link for the packet); the packet constructor, destructor and draw/prepare functions are HD-only.

### d_a_obj_ajav (falling rock tower) — verified, 2026-10-03

- Structure: each rock-part record shrinks from 0x98 to 0x90 bytes because its two callbacks use eight-byte member-function descriptors in HD.
- Graphics: the flashing material adds normalized floating-point color conversion and a material upload in HD.
- Graphics: both water-ripple effects place particles 0.1 units above the detected water height in HD; GameCube uses that height directly.

### d_a_obj_homen (Wind Temple stone Hookshot target) — verified, 2026-10-03

- Structure: The HD actor grows from 0x808 to 0x920 bytes. Its phase and later actor fields move by 0x118 bytes, and resource lookup uses the HD two-word string wrapper.
- Graphics: The GameCube model helper conditionally allocates a deferred display list using its third flags argument. That allocation sequence is absent from the reviewed HD helper, which constructs the model using the second flags argument and a fixed entry mode. This confirms a setup difference; no resulting visual difference has been established.

### d_a_hmlif (path-following platform) — verified, 2026-10-03

- Fix: HD replaces encoded platform types outside the three supported variants with the first variant before looking up resources. GameCube uses the encoded type directly.
- Structure: The HD actor grows from 0x48C to 0x5A4 bytes, shifting corresponding platform members by 0x118 bytes. The path-state fields, collision component sizes, and resource heap budgets are retained.

### d_a_wind_tag (wind column tag with point wind and water-splash emitter) — verified, 2026-10-03
- Graphics: on the "sea" stage, the wind model's materials get extra colour setup (a TEV colour taken from each material at creation, and at draw time a konst colour with fixed alpha 50 plus a colour from the actor's light), written into HD material uniform blocks; probably to match the HD ocean lighting.
- Graphics: the splash emitter's height is mirrored (negated) when the emitter's draw-group byte is 7 or higher; probably an HD effect-system adjustment.
- Structure: when a path is attached, the first target point is always path point 1 (constant index); the emitter scale is written to two places; the texture-animation entry takes the model instead of its model data.
- Structure: compiler-generated static init and destructors added.

### d_a_bflower (bomb flower) — verified, 2026-10-03

- Structure: HD relocates the actor and collision members and expands its embedded animation wrappers. Resource lookup uses an HD string wrapper; the associated deletion and termination helpers belong to that wrapper. Player access follows the HD singleton and virtual grab-ID getter.
- Gameplay: the compared live/dead actions retain their proximity checks, bomb pickup, water handling, switches, and recovery timing. GameCube already updates the player's kept-actor record when exchanging a grabbed actor; the HD setter preserves that operation. No gameplay change was established in this comparison.

### d_a_lstair (staircase) — verified, 2026-10-03

- Structure: HD expands and relocates the embedded animation wrappers and adapts resource names and stage/player access to HD string and singleton interfaces.
- Gameplay: the compared staircase behavior retains its switch/enemy triggers, emergence delay, movement pause, animation reversal, and culling bounds. No gameplay change was established.

### d_a_pedestal (Tower of the Gods pedestal) — verified, 2026-10-03

- Graphics: HD reverses the glow emitter’s vertical position when its particle mode byte is seven or higher. GameCube always uses the supplied vertical position. This probably adapts the effect to an HD particle coordinate mode. HD also supplies the current colour-animation frame explicitly when drawing.
- Structure: The actor grows from 0x324 to 0x49C bytes. Its colour-animation wrapper grows from 0x18 to 0x78 bytes, while member-function descriptors shrink from twelve bytes to eight.
- Gameplay: The event cuts, statue save-event mappings, raising and lowering distances, and glow particle remain present. At a numerical edge, HD clamps movement on an unordered position comparison where the GameCube source’s ordered comparison would not; this is probably compiler behavior, with no intentional gameplay fix established.

### d_a_beam (beam trap) — verified, 2026-10-03

- Removed: GameCube has a dedicated timed search mode selected by parameter kind 2. In the compared HD initialization, kind 2 installs the moving-search callback instead, and the dedicated timer callback is absent from this unit. This is a callback-selection difference; its effect in individual scenes was not established.
- Structure: HD expands the actor and its animation members, uses HD model/collision layouts, and passes archive names through an HD string wrapper.

### d_a_tag_photo (photo conversation trigger) — verified, 2026-10-03

- Structure: the HD actor occupies 0x43C bytes. Its enlarged actor and event-object layout moves the Photo-specific event and dispatch fields 0x11C beyond their GameCube positions.
- Structure: HD conversation creation uses the global message manager and stores a message ID in the actor instead of the GameCube message pointer. A byte in the base actor tracks message activation; successful creation clears it and later execution enables it.
- Structure: message status is maintained by the HD message API in the play object. Event reset also updates the play object, while event completion still records the same saved event bit.
- Structure: the HD destructor calls the base actor destructor and conditionally releases the actor. The GameCube event-order hooks are empty, and the corresponding HD bodies add no ordering behavior.

### d_a_bwd (Molgera; partial: 22 of 26 functions verified, end/move/demo_camera/Execute pending) — verified, 2026-10-03
- Gameplay: when the fight starts without the boss-demo flag, Molgera waits at a height of -5000 instead of -20000 (daBwd_Create).
- Gameplay: the flight steering's pitch/roll easing step adds a debug register to its constant (fly_pos_move); probably zero in normal play.
- Gameplay: in the death sequence (end, ported but not yet verified) the body wobble phases advance every frame with smaller steps, and the clear-music stream is prepared later (USA behaviour); the attention helper actor created at spawn is deleted when the death starts.
- Graphics: the sandfall (taki) models have a model flag cleared and pushed to their material packets after creation (useHeapInit); the body segments get an extra per-segment wobble value (HD-only array) that settles once a segment hardens.
- Structure: bwd_class is 0x3ECC bytes: an extra 20-float array after the segment-hardness floats, the HD tevstr growth, and the GameCube cXyz before the "m18FC" timer is gone; the tevstr copy in Create is an inline member-wise copy; the HIO object is 0x40 bytes with its vtable last.

### d_a_am (Armos) — verified, 2026-10-03
- Graphics: the Draw function no longer registers the GameCube blob shadow under the statue; probably replaced by the HD shadow system.
- Gameplay: the voice-sound calls (awakening, jumping, eye damage, swallowing) pass a parameter of 0 where the GameCube code passed 0x42; probably only affects how the sound is mixed.
- Structure: layout shift only (every member +0x11C, size 0x11BC); medama_move, medama_atari_check and the four action functions are inlined into the execute function; the smoke callbacks are ended by a direct call instead of a virtual remove in the execute function; the fall-out-of-the-world block uses out-of-line copies of the sound-start and emitter-status helpers.
- HD-only: compiler-generated class constructor/destructor, enemyfire constructor copy, smoke-callback array constructor/destructor, string-class helpers and the static initializer are separate functions in this unit.

### d_a_bgn2 (Puppet Ganon, phase 2: the spider) — verified, 2026-10-03
- Fix: the leg ripple effect uses the 30 leg positions directly; the GameCube code offset the index by 4 and read past the end of the leg array for the last four legs. The jump-attack foot splashes likewise use legs 4, 9, …, 29 instead of 8, 13, …, 33 (the last one was out of range on GameCube).
- Graphics: the red rope hanging from the body is built with its segments 80 units apart instead of 50 (a longer rope); a fourth model packet (probably for HD shadows) is set up after the rope.
- Gameplay: the ripple effect checks its every-eighth-frame condition before doing the ground line check (same result, fewer line checks).
- Structure: bgn2_class is 0x35C8 bytes (model packets 0xB0, rope 0x188, an extra packet); the HIO object is 0x34 bytes with its vtable last; move and all action functions, the effect helpers, damage_check and ki_check are inlined into the execute function; several action functions and the execute function make an unused play-object lookup; the background-light copy in Create and the static initializer are member-wise copies of the HD light structure; when the boss's last hit lands, a play-object float is cleared together with stopping the music.
- Structure: the matcher swapped the names of the two search callbacks (0208B020 finds Puppet Ganon's main actor, 0208B070 the phase-3 actor).

### d_a_dai (pedestal NPC) — verified, 2026-10-03

- Structure: HD reads the player’s selection from its central message service. GameCube follows the NPC’s current-message pointer. HD also uses the inherited actor virtual table and a larger NPC base, with pedestal fields following that base.
- Graphics: HD creates the model through its model-entry routine and omits GameCube’s separate deferred display-list allocation. The legacy third model-creation flag passed by the actor is unused by the HD routine.

### d_a_ghostship (Ghost Ship) — verified, 2026-10-03

- Graphics: HD Draw updates the expanded material and uniform path and marks material state dirty in addition to setting alpha. The corresponding GameCube material loop only updates the alpha byte.
- Structure: HD Delete additionally calls the virtual destructors of live cloth objects and clears both actor pointers. The GameCube counterpart releases the two resource archives. The HD actor profile allocates 0x888 bytes.
- Structure: The collection-map lookup uses slot 0x23 in HD and 0x24 in GameCube. The map identities have not been decoded, so this does not establish a gameplay change.

### d_a_obj_search (searchlights) — verified, 2026-10-03
- Structure: HD places the controller flag at actor offset 0x780; the GameCube header places it at 0x664. The HD child-actor lookup uses offset 0x96C, compared with the GameCube header's child identifier at 0x84C. Those fields do not share one uniform offset change, so the later state layout cannot be recovered by shifting every GameCube member equally.
- Structure: Resource lookup uses an HD resource-controller instance and temporary SafeString objects, including their virtual tables. The GameCube resource API takes raw archive-name strings.
- Structure: HD includes an actor constructor and virtual destructor with per-unit virtual-table data; the GameCube actor base declares a nonvirtual destructor.
- Structure: The available GameCube implementation consists largely of placeholders, so this port does not establish a player-visible gameplay change, a graphics change, a removed feature, or a bug fix. The HD behaviour was recovered directly; missing GameCube source is not evidence that a feature was added in HD.


### d_a_deku_item (collectable Deku Leaf) — verified, 2026-10-03
- Structure: The collectable Deku Leaf keeps the GameCube state sequence, but its two animation objects grow from 16 bytes each to 140 bytes each in HD. Together with the larger actor base, this moves the movement and collision objects and item-event fields and increases the actor from 1,600 bytes to 2,132 bytes. Resource names use SafeString temporaries, and game, save, and event systems are reached through singleton accessors.
- Graphics: When an existing sparkle emitter is moved, HD copies the leaf position and negates its Y coordinate for emitter types 7 and above. The GameCube emitter setter in the reference header copies the position without that conditional correction.

### d_a_machine — verified, 2026-10-03

- Structure: The actor grows from 0xC94 bytes on GameCube to 0xE2C bytes in HD. The larger actor base and expanded animation object move the model, collision, path-state, and saved matrix fields.
- Graphics: HD looks up the named joint through the model name table and installs its callback by index; GameCube walks the joints and compares their names. The HD callback explicitly marks the animation-matrix buffer dirty before copying the joint matrix. These are implementation differences; no resulting visual change has been established.


### d_a_ib (Iron Ball) — verified, 2026-10-03

- Removed / Gameplay: The GameCube item callback sets 90-frame and 60-frame timers when the created item exists and isEmono returns false. HD returns success without reading or changing the item, while item creation still supplies that callback.
- Graphics / Removed: The HD draw routine omits the GameCube simple-shadow submission after draw-list restoration. It retains lighting, animation entry and model submission. This describes the actor's draw path.


### d_a_mdoor (wooden and metal bars) — verified, 2026-10-03

- Structure: The actor grows from 0x2D0 to 0x3EC bytes. Its smoke callback, loading phase, model/background pointers and event/movement fields retain their order with offsets shifted by 0x11C. HD resource lookup passes a temporary SafeString object instead of the GameCube character-string argument.
- Removed: The HD Delete method retains background-collision release, smoke cleanup and archive unloading, but omits the explicit actor-destructor call at the end of the GameCube Delete method. HD still has a separate deleting destructor in the actor virtual table.

The examined action states, opening and closing height targets, movement acceleration, smoke rate, spread and two scale vectors retain their GameCube values.

### d_a_obj_Vds (Earth Temple face statue) — verified, 2026-10-03

- Structure: HD allocates a 0x574-byte actor with a 0x3AC-byte inherited base, compared with the GC base size of 0x290. The available GC Vds header omits the actor members, so it does not establish the complete original size or individual field shifts. HD includes explicit light and animation-object lifecycle entries alongside the five standard actor callbacks.
- Structure: HD loads BCK animation data through a resource wrapper with a virtual frame-count getter and an owned resource name. GC declares the setter with animation-key pointers. This probably reflects a resource-library representation change.
- Gameplay: The available GC method bodies are placeholders, so this comparison does not establish a gameplay or constant change. The HD implementation includes the switch-actor searches, event states, paired morph and BRK animation, and two lights.

### d_a_am2 (Armos) — verified, 2026-10-03

- Structure: HD grows the actor from 0xE44 to 0xF60 bytes. The inherited base grows by 0x11C, with the actor-local fields shifted by the same amount. Collision and local effect objects retain their GC sizes. Several action routines and eye-hit handling are embedded in HD Execute; return-home remains a separate function.
- Graphics: The joint callback uses HD node wrappers and matrix-block storage in place of GC’s direct animation-matrix access. Resource lookup supplies the HD SafeString representation.
- Removed: The simple ground-shadow call used by GC when the actor is not carried is absent from the HD Draw body.
- Gameplay: HD appears to retain the GC wake-up, stun, carrying, damage, recoil and return-home states; no additional gameplay difference is established by this comparison.


### d_a_npc_kg1 (Salvatore, Squid Hunt) — verified, 2026-10-03

- Structure: The actor grows from 0x78C bytes in the supplied GameCube layout to 0x9B8 bytes in HD. Its three texture-pattern animation wrappers grow from 0x14 to 0x74 bytes each, with corresponding changes to the surrounding fields.
- Structure: HD texture-pattern animation initialization returns a Boolean success result, whereas the supplied GameCube declaration returns void. HD receives the modify argument as a raw 32-bit value rather than the GameCube Boolean parameter.

The available comparison establishes these layout and interface changes; it does not establish a gameplay change. Verification uses the accepted animation and dialogue companion fixtures.

### d_a_nh (Forest Firefly) — verified, 2026-10-03
- Graphics: HD registers a point light at the firefly, updates its position each frame, and removes it on deletion. The record starts with three 16-bit color components of 300 and a light parameter of 250; the parameter's physical meaning is unproven. Drawing advances a light blend toward 100 in steps of 10. GameCube instead derives glow strength from model material colors and submits a spherical alpha model; these operations are absent from the HD draw function. Both versions retain TEV-register animation and a separate glow transform.
- Structure: The HD actor occupies 0x844 bytes, uses an eight-byte action member pointer, and has a larger BRK animation controller. Its host-I/O object uses a Disposer subobject rather than the GameCube JORReflexible layout. HD also supplies an explicit actor constructor and a virtual destructor.
- Gameplay: Waiting, frightened escape, home return, bottle-catch requests, and bottled-firefly fading appear to retain the GameCube decisions. No change to those decisions has been established.

### d_a_mbdoor (double doors) — verified, 2026-10-03

- **Graphics:** HD simplifies the five resource-index getters to constants. The GameCube source branches on door shape, but both archive headers use the same corresponding indices, so the selected resources agree.
- **Structure:** HD allocates a 0x404-byte actor and starts the derived phase/model fields at 0x3AC, compared with 0x290 in the GameCube layout. The larger base shifts these fields by 0x11C. Resource lookups use an eight-byte SafeString with a virtual table, and the executable contains separate string cleanup helpers and an actor destructor.
- **Removed:** The GameCube delete wrapper explicitly calls the actor destructor after releasing background collision and the resource phase. HD performs the release and resource deletion and returns directly; its separate actor destructor remains present. No gameplay consequence is established by this call-structure difference.

### d_a_gy_ctrl (Gyorg controller) — verified, 2026-10-03

- **Structure:** The release GameCube actor is 0x4B4 bytes; HD is 0x5D0, matching the 0x11C actor-base expansion. Both layouts retain five spawn/child slots and sixteen radial path positions. This storage layout does not establish a restriction on authored count parameters.
- **Structure:** The four mode records shrink from 28 to 20 bytes. Each member-function descriptor shrinks from twelve to eight bytes; the controller retains its switch-wait, creation, waiting and hiding modes.
- **Structure:** HD treats any nonzero child-active byte as active, whereas the GameCube source checks for one. No change to normal gameplay is established from noncanonical flag values.
- **Removed:** HD omits unused debug-vector calculations from the draw routine. Neither compared routine contains visible rendering calls, so no visible debug rendering removal is established.
- **Verification scope:** All 34 local functions pass two 10,000-input seeds with full block coverage and the official whole-unit check. Mutation and helper-control evidence retains explicit call-mocking, fixed first-spawn input observation and constructed-state limits; recorded gameplay and full transitive child execution were not established.

### d_a_dai_item (decorative stand items) — qualified source and companion evidence

- **Structure:** HD's typed actor occupies 0x7E4 bytes. The compared derived fields move by 0x11C: the cloth selector moves from GameCube offset 0x6A0 to HD 0x7BC, and the animation play timer moves from 0x628 to 0x744. Both versions retain four cloth selector values. Selector preservation across two actual resource lookups is established only within the constructed live, disjoint resource domain; later factory and object lifetimes remain open.
- **Gameplay:** The four twelve-entry animation/stop timing tables match the supplied GameCube source. Midpoint selection, random half-width and restart at speed one/frame zero are retained. No gameplay timing change or bug fix was established.
- **Verification scope:** Initialized resource/math, animation timing and warm nonempty-wind companions have saved paired 10,000-input controls for all 39 local functions. Their constructed domains require live, disjoint objects and bounded finite arithmetic intermediates. The private cold-environment companion uses a bounded local-HLE memset bridge and excludes active-manager ownership; it remains diagnostic evidence, without native SDK equivalence or canonical harness promotion. Broad canonical creation/matrix diagnostics, factory/postcreation resource ownership and lifetime gaps remain open.

### d_a_goal_flag (finish-line flag and boat-race controller) — qualified verification

- **Structure:** The GameCube actor is 0x172C bytes with a 0x1388-byte packet at 0x290. HD is 0x2FB8 bytes with a 0x264C-byte packet at 0x3AC. HD retains the 45-vector cloth arrays after larger rendering objects; its packet's current-array selector is at 0x2648.
- **Graphics:** HD uses shader materials, uniform blocks, texture-pair storage and allocated vertex buffers in place of the GameCube GX packet. No gameplay change or specific bug fix was established.
- **Verification scope:** Thirty-five ordinary functions have saved 10,000-input passes; three additional implemented lifecycle functions have qualified evidence. Initialization requires decoded resources and successful aligned, disjoint allocations from the ambient TLS heap. Packet construction precedes the rope solid-heap callback, which does not establish packet ownership. Packet destruction requires child/free effects that preserve parent metadata; actor destruction additionally requires valid rope, base-actor and scalar-delete lifetimes. Complete allocator, OS/TLS and decoder execution remains open.

### d_a_pirate_flag (pirate ship flag) — source audit; verification incomplete

- **Structure:** HD cloth buffers begin at packet offsets 0xCC, 0x324, 0x57C and 0x7D4, compared with GameCube's 0x44, 0x29C, 0x4F4 and 0x74C. The HD constructor allocates 0x1B00 bytes; the typed header describes its cloth prefix. HD function 0x023D1EA4 takes one actor pointer and includes the cloth update; the old seven-argument GameCube match was corrected.
- **Graphics:** HD uploads alternating position/normal buffers and binds shader, uniform and texture resources before indexed GX2 draws. GameCube uses immediate GX vertex arrays. No resulting visual difference was established by this source audit.
- **Verification scope:** Twenty-four ordinary functions have preserved 10,000-input controls. Separate destructor/resource profiles cover bounded ownership contracts. GPU import receipts model runtime compatibility; they do not establish native GPU-library equivalence. The converter initializer's terminal SIGSEGV remains unaccepted, and whole-unit verification remains open.

### d_a_btd (Gohma) — layout audited; verification incomplete

- **Structure:** Actor storage grows from 0x6E94 to 0x70D8 bytes. HD adds the animation triplet at 0x3F0/0x3F4/0x3F8, selected when phase is nonzero and state is 11. The separate head triplet moves to 0x3FC/0x400/0x404. Fields require different offset shifts; HD light RGB components use halfword stores at 0x61E4/0x61E6/0x61E8.
- **Call ABI:** Particle setter 0x025A847C consumes four incoming stack words: room, two color pointers and an extra scale pointer. Source calls supply them, but frozen v28 comparison specs omit their observation. Corrected v29 specs are separate and lack completed correction controls/replays.
- **Verification scope:** Thirty-five primary functions and one damage companion have saved two-seed 10,000-input passes under stated mock domains. The campaign remains incomplete at 1,311 of 8,359 planned cases. Thirteen coverage markers, particle ABI correction checks and broader SDK/physical-state validity remain open. This read-only audit adds no runtime or gameplay verification.

### d_a_majuu_flag (Forsaken Fortress cloth flag) — layout compared; verification incomplete

- **Structure:** Actor storage grows from 0xAA0 to 0x1E3C bytes and the embedded cloth packet from 0x79C to 0x1A30. The packet moves from actor offset 0x2A0 to 0x3AC. Both layouts retain two sets of 21 positions and front/back normals plus 21 velocity vectors; HD adds rendering and ownership objects before these arrays. The draw matrix moves from packet offset 0x10 to 0x1290 and positions from 0xA0 to 0x1344.
- **Graphics:** The compared source replaces GameCube GX/TEV setup and display-list drawing with HD shader/material bindings, uniform and attribute objects, and indexed drawing. No resulting visual or gameplay difference was established.
- **Verification scope:** Of 26 implemented functions, 23 ordinary functions have saved seed-7, 10,000-input controls with 249/272 blocks and zero both-side exclusions. Setup and the packet/actor destructors remain outside those controls. Counted-array ownership, compatible heap allocation/free, callback lifetimes, graphics imports and allocation-failure states remain open. The separate 100-input Setup diagnostic has 96 matches and four both-side aborts; it is not a passing 100-input control. No whole-unit or recorded-scene verification is claimed.

### d_a_daiocta_eye — Big Octo eye

- **Gameplay:** The compared eye keeps four initial health points, the existing weapon damage amounts, bomb-hit handling, three action states, and water-dependent lock-on behavior. No gameplay change was established.
- **Graphics:** HD resolves a joint through the callback node wrapper, then routes its matrices through a matrix-buffer object and marks it dirty before access. The GameCube implementation treats the callback node as the joint and uses the model matrix array directly. Both rotate and scale the second eye joint and retain texture/color animation cleanup after drawing.
- **Structure / HD-only:** The actor grows from 0x4A0 to 0x67C bytes, including the expanded actor base and larger animation controllers. HD uses compact member-function descriptors, SafeString resource names, and the HD model/node interfaces.
- **Fix / Removed:** No specific bug fix or removed feature was established.
- **Verification scope:** Thirty local functions pass two 10,000-input seeds, an O1 candidate check, and a current shared-harness check. The sole unreached dispatch block requires a virtual descriptor absent from the three actual mode records. Survivor proofs assume initialized, distinct live game objects; recorded gameplay and complete SDK lifecycle execution were not established.

### d_a_npc_mk_static (NPC Mk path and avoidance utilities) — verified, 2026-10-03

- **Gameplay:** Route turning, nearby-player avoidance, distance thresholds and acceleration timers retain the compared GameCube behavior. No intentional gameplay change or bug fix was established.
- **Structure:** The utility remains sixteen bytes, with eight-byte path records. Actor position access uses the expanded HD actor layout. Play and temporary-save state use HD singleton layouts, and temporary ground queries use HD virtual tables and base construction and cleanup. Compiler-generated translation-unit initialization is present.
- **Graphics:** These utility functions perform no rendering.
- **Verification scope:** Fourteen local functions pass two 10,000-input seeds with all 123 original blocks reached and an additional O1 check. Mutation review covers 571 sites with 341 meaningful differences, 201 compile-invalid cases and 29 scoped equivalents. Ground-query proofs assume a warm Play singleton, standard background classes, valid finite resource graphs and distinct live object storage. Path-consumer tests use a bounded valid resource. Recorded gameplay, arbitrary subclasses and pointer aliases were not established.

### d_a_tag_md_cb (Medli/Makar event and warp triggers) — verified, 2026-10-03

- **Structure:** HD uses eight-byte member-function descriptors for action and event dispatch, compared with twelve bytes in the GameCube layout. Fields after the action descriptor therefore shift four bytes less than the expanded actor base alone suggests. The HIO virtual pointer and parameter fields are reordered.
- **Removed / Structure:** The GameCube profile Delete callback invokes the actor destructor; HD returns success directly. The separate HD virtual destructor retains HIO child release, the live-instance counter, base destruction and optional deallocation. No gameplay consequence was established from this call-structure change.
- **Structure:** HD dialogue accesses current message status through singleton functions rather than directly through a searched message object. The compared typing, displayed, continuation and closed-state decisions retain their corresponding roles.
- **Gameplay:** The compared retail event bits, switches, timers, area gates and companion carrying checks retain their existing roles. No intentional gameplay change was established.
- **Verification scope:** All 35 local functions pass two 10,000-input seeds and an O1 check, reaching all 332 original blocks. Review covers 803 primary changes and 17 additional helper probes: 540 concrete differences, 263 compile-invalid cases and 17 scoped equivalents. Equivalent-case proofs assume valid live nonwrapping object storage, distinct actor/static/scratch regions and single-threaded execution without memory-fault or read-order observations. Actual old/new action callbacks and the event setter were tested separately. Native allocator/scheduler lifetime guarantees, recorded gameplay and full transitive execution were not established.

### d_a_tag_event (event trigger tag) — verified within documented scopes, 2026-10-03

- **Gameplay:** The HD Hunt, MjHunt and SpeHunt actions first require a player and consult an added readiness check. That check tracks an actor counter and combines two play-state fields; the GameCube actions directly test distance and switches. HD creation also immediately dispatches the initialized action in Atorizk and kenroom.
- **Structure / HD-only:** The actor grows from 0x298 to 0x3B8 bytes, including the expanded actor base and a four-byte readiness counter. Password handling uses UTF-16 lookup and a buffered string with capacity for sixteen characters and a terminator. The local port preserves the original saved frame bytes because failed lookup leaves the password buffer untouched. The full boundary includes the added readiness check, static initialization and trailing virtual helpers.
- **Removed:** HD Delete returns success without the explicit destructor invocation in the GameCube callback; the HD virtual destructor remains a separate function.
- **Graphics / Fix:** Draw performs no rendering in either compared version. No intentional bug fix was established.
- **Verification scope:** All 33 functions pass two 10,000-input seeds and a current-contract review. Mutation reconciliation covers 1,014 contexts: 589 bounded fixture differences, 350 compile-invalid cases, four native-invalid cases and 71 scoped equivalents. Three password-cap proofs cover normal registered actor calls and valid private frames. Copy proofs use bounded initialized storage; the original budget diagnostics remain preserved. Arbitrary direct calls, replaced profiles, invalid lifetimes, aliased frames, raw stack/address observers and complete SDK lifecycle execution are outside these certificates. Recorded gameplay was not established.

### d_a_tag_island — island arrival and event tag

- **Gameplay:** The arrival checks, switch and event selection, dialogue, wait timers and music-lesson transitions follow the GameCube structure for ordinary finite positions and initialized actors. No explicit gameplay fix or removed feature was found.
- **Graphics:** After restoring the raft position and path targets, HD also calls the raft matrix-update routine. The GameCube tag reset shown in the source stops after restoring those fields.
- **Structure:** HD retains the tag’s field order, shifted by the larger actor base. The actor grows from 0x2B0 to 0x3CC bytes. HD’s compiled arrival-flag lookup uses a constant table, and the lesson cancellation check follows the player virtual function in the HD layout.
- **Verification:** All 38 owned functions pass 10,000 generated inputs on current shared code, including two seeds and an O1 candidate build. Coverage is 221 of 225 blocks; four remaining event-dispatch branches contradict the retained action value. Paired candidate audits and independently reviewed survivor proofs cover initialized ordinary objects and distinct live raft/path resources. Actual nested actor, vector-math and player-getter companions also pass. Warm-singleton caller comparisons retain their narrower scope; this is local harness evidence without a recorded scene or console run.


### d_a_tag_hint (hint and lighting trigger) — verified, 2026-10-03

- Gameplay: HD gives trigger type 14 a fixed vertical extent instead of scaling that extent. The actor dispatcher skips the idle action while inactive, and its timer waits while a message is active.
- Graphics: HD changes the two light palettes and their strength: the point light becomes a brighter blue, while the secondary blue light has a substantially lower strength. The principal creation and deletion paths manage the point light; the extra effect-light handling found in the GameCube paths is absent there. Fog and darkness adjustments remain.
- Structure: HD uses the expanded actor base layout and a separate virtual destructor. Stage-name comparisons use temporary string objects. The deletion wrapper performs event-bit cleanup without directly running the actor destructor.
- Verification scope: Forty local entry points passed 10,000 generated cases with two seeds at both optimization levels. Collision reasoning assumes finite valid geometry and the six identified stock background types; arbitrary subclasses and full recorded-scene lifetimes were not established.

### d_a_npc_hi1 (King of Hyrule) — verified, 2026-10-03
- Graphics: no blob-shadow draw (shadowDraw is gone, as is the shadow-id field); probably replaced by the HD real-time shadows. An extra, unused play-state accessor call sits in the draw routine.
- Fix: the bounds check when changing the animation attribute rejects index 2 as well; the GameCube check let index 2 through and read one entry past the two-entry table.
- Gameplay: the conversation-end check reads the message status from the HD message manager instead of from the message object; the outcome (reset item, restore state, re-roll the idle timer, end the event) is the same.
- Structure: actor grows from 0x7CC to 0x99C bytes (larger NPC base, texture-animation helpers grown to 0x74 bytes each, shorter member-function pointers); the eye-pattern and texture-scroll resource lookups always read their single table entry; the actor vtable is merged with the NPC base vtable; the HIO object constructor allocates itself when called without storage, and its debug field is a byte instead of a float.

### d_a_npc_kg2 (Salvatore, Windfall cannon minigame) — verified, 2026-10-03
- Structure: the GameCube decompilation has only Nonmatching placeholders, so the whole unit (layout and all 43 functions) was written from the WWHD code; the HD claims below are therefore comparisons against the GameCube function list only. Size and layout are measured from HD: NPC base of 0x7DC bytes, two 0x74-byte texture-pattern animations (face and held item) and an eight-byte action pointer.
- Graphics: the draw routine applies lighting, the face pattern and the optional held item, then registers the pictograph figure; no blob-shadow call is present, so probably the HD real-time shadows replace it.
- Gameplay: no behaviour change could be established, since the GameCube side has no matching code to compare; the minigame flow (wait, talk, event cuts, item creation) probably follows the GameCube function structure, which has the same set of functions.

### d_a_npc_sarace (Loot the sailor, boating course) — verified, 2026-10-03
- Structure: GameCube source is Nonmatching placeholders only (empty header), so the 35 functions and the 0x8B4-byte layout were written from the WWHD code. The actor carries a second animated morf for the head next to the body and tracks the process ids of two course actors. Two functions the symbol matcher mislabelled (matrix setup and event-order check) were identified by position against the GameCube function list.
- Graphics: draws body and separate head with lighting and the eye pattern on the head, then registers the pictograph figure; no blob-shadow call, probably because HD draws real shadows.
- Gameplay: not comparable in detail (no GameCube code); probably unchanged, as the function set matches the GameCube list.

### d_a_npc_ah (Old Man Ho Ho) — verified, 2026-10-03
- Graphics: no shadow registration in the draw routine (the shadow-id field is also gone); probably covered by HD real-time shadows.
- Gameplay: create follows the GameCube retail path (matrix update and collision setup at the end of init, not the demo-version order). Message handling goes through the HD message manager with a flag instead of a cached message object; the empty "typing" status case of the GameCube dialogue loop is absent, which probably has no visible effect.
- Structure: actor grows from 0x754 to 0x8C8 bytes (larger NPC base, texture-pattern helper grown from 0x14 to 0x74 bytes, shadow id removed); resource release on delete uses the ordinary delete call instead of the GameCube demo variant; the class gains a virtual destructor and photo-mode state is read from the event control block.

### d_a_warpf (boss warp light / Tower of the Gods, Earth and Wind Temple warps) — verified, 2026-10-03
- Fix: creation asserts that the stage save-table index is within the 16-entry archive table; the GameCube create indexed the table without a check.
- Graphics: in the Earth and Wind Temple variant the register-colour animations are applied to the model instance instead of the shared model data (HD animation API); the wind-effect angle comes only from the camera and no longer reads the actor's own angle (probably equivalent in practice). The GameCube quirk in the Tower of the Gods animation update (it re-tests the wrong animation pointer before playing the texture scroll) is kept unchanged.
- Gameplay: one sound start skips the actor/eye-position null checks; no other behaviour change was found across the 41 verified functions.
- Structure: the actor base gains a vtable and a compiler-generated deleting destructor; the event init/action tables use eight-byte member-function pointers; actor size 0x414 bytes.

### d_a_wz (Wizzrobe and Wind Temple mini-boss) — verified, 2026-10-03 — PARTIAL (22 functions so far)
- Structure: GameCube source is Nonmatching placeholders only, so this is written from the WWHD code; HD claims are therefore probable. Layout is measured from HD (fields shifted by the larger enemy base, size 0x10B0 vs. GameCube 0xF8C). Several small GameCube helpers (body, summon-door and damage-ball draw, parts of the action and damage logic, sea-water check, projectile action) are inlined into their HD callers. Only 22 of the unit's functions are verified so far (callbacks, draw, heaps, delete, constructors/destructors, rod sizing, background check, projectile helpers, hit check); the AI/action functions are still open.
- Graphics: the fade-in/out alpha is written into each material's colour register and additionally into an HD per-material float colour block, and the model is flagged as translucent while not fully opaque (probably so the HD renderer sorts it correctly). The draw routine registers no blob shadow; probably replaced by HD real-time shadows. Pictograph registration happens only when the Wizzrobe is at least half visible.
- Gameplay: hit and stun sounds are guarded by actor/eye-position null checks in HD; no behaviour change has been established yet in the verified part.


### d_a_bomb_static (Bomb and Bomb Flower utility functions) — 2026-10-03

- **Structure:** HD has 27 utility/template entries and a header-static initializer attributed through its native registration records. The GameCube source groups the same utility methods in a separate translation unit, without a corresponding initializer entry in that split.
- **Structure:** Bomb rest and no-gravity timers move from GameCube offsets 0x6FC/0x700 to HD 0xA7C/0xA80. Bomb Flower state moves from 0x694 to 0x88C, and its timer from 0x738 to 0x934. Both versions read the check flag as an unsigned byte; the GameCube method declares a signed 16-bit return.
- **Gameplay:** Version and ordinary Boolean parameter options retain their bit positions. HD packing preserves more raw register bits for non-Boolean arguments; a player-visible difference was not established.
- **Verification scope:** All 28 entries pass two 10,000-input seeds and the official per-unit gate, reaching all 34 native blocks. A separate ordering/alias fixture passes. The 257-change audit records 134 detected comparisons, 116 compile-invalid cases and seven scoped source-equivalent reorderings. Six detected comparisons require synthetic actor-interior aliases. External SDK/emitter lifetimes, destructor registration and recorded game execution remain outside these local certificates.

### d_a_warpfout — warp departure event tag (2026-10-04)

Structure: The HD actor dispatches five scripted warp stages through two fixed tables of direct member-function entries. Graphics: Departure effects use the HD particle controller and a packed six-byte camera angle; tests preserve the second returned angle word as well as the first. HD-only: Event and player access use the HD play singleton. These findings describe the reconstructed HD path; GameCube parity beyond names remains unconfirmed.

### d_a_tag_kf1 — pottery event tag (2026-10-04)

Structure: The reconstructed HD tag finds pottery partners through the process-layer search and an actor filter, then copies up to eight actor IDs into its partner slots. Structure: HD static initialization registers three global records before constructing the tuning object; its word loads depend on those records being populated. The GameCube source provides names and largely placeholder bodies, so broader gameplay differences remain unconfirmed.

### d_a_ygcwp — Ganon’s Castle warp portal (2026-10-04)

Graphics: The HD portal uses two BRK controllers and a particle effect for its scripted transition. Structure: Controller construction uses shared register-save/restore routines; those routines must be included when testing the actual constructor chain. Fix: The reconstruction preserves parameter normalization and animation-controller updates in the portal event path. Broader GameCube differences remain unconfirmed; the original GameCube source supplied names and layout guidance.

### d_point_wind (capsule-driven point wind) — 2026-10-04

- **Structure:** Preserves the HD reference layout for the capsule pointer and wind influence, including position, direction, radius, strength, registration index and constant-wind flag. The local routines calculate the influence and register it with the environment manager.
- **Gameplay:** Stage-name handling and capsule distance calculations follow the HD reference. No new GameCube-to-HD gameplay difference was established by this review.
- **Verification scope:** Six routines pass canonical and initialized finite controls at both optimization levels. The 402-case sensitivity review is complete within that scope; SDK constructor boundaries and game-wide startup/lifetime behavior remain qualified.

### d_a_npc_tt (Tott, the Windfall dancer) — verified, 2026-10-04
- Graphics: no blob shadow is registered in the draw function (HD renders real shadows).
- Graphics: the hair-strand ("ke") simulation adds two debug-register offsets (zero in a normal run) that the GameCube build compiled out; the strand line renderer (3D line material) is the much larger HD version.
- Structure: the talk flow drives the shared HD message manager directly instead of searching for a message process by id; the separate message-actor pointer of the GameCube build is gone.
- Structure: the eye texture-pattern animation always uses the first table entry instead of indexing by the pattern number (only one entry exists, so probably no visible change).
- Structure: layout shifts from the HD head-animation helper, shorter pointer-to-member fields, the larger texture-pattern animation and line material, and the removed shadow id; actor size 0x1120 (GameCube 0xE84); HD virtual destructor.

### d_a_arrow (Link's arrows, Zelda's light arrows) — verified, 2026-10-04
- Fix: several pointers the GameCube code used unchecked are null-checked in HD: the archer found when checking for Zelda's arrows, Ganondorf's actor in the shield-reflect code, Zelda's hand-matrix lookup, and the hit polygon's plane after a background hit.
- Gameplay: an arrow counts as entering water for any non-zero result of Link's water-effect check (GameCube: only one specific value).
- Gameplay: the Puppet Ganon light-arrow hit no longer looks up the hit polygon's plane; the hit and no-hit paths share one background line check.
- Graphics: the light-arrow sparkle emitter's vertical position is negated for newer emitter versions (probably an HD particle coordinate convention); reading a hit actor's joint matrix marks the model's matrix buffer as used.
- Structure: the "consume arrow item" sound is started without a position; the effect sound helper drops its null checks; the stage-name test for Ganon's tower compares sead strings; the current-process member pointer is the 8-byte GHS form; layout shift for the larger HD actor base (class 0x81C bytes).

### d_a_npc_p1 (Gonzo, Senza and Nudge, Tetra's pirates) — verified, 2026-10-04
- Gameplay: in the "Demo17" stage cutscene, during one window of demo frames the pirate's animation plays at double speed and a flag of its demo actor is cleared; in another window Senza is moved 20 units sideways every frame (probably timing/placement fixes for the HD version of that cutscene).
- Graphics: each of the three pirates loads its own body model; the GameCube build used one body model and swapped materials while drawing. Create also sets an explicit culling box.
- Graphics: no blob shadow is registered in the draw function.
- Structure: all talk flows (talk, speak, Nudge's speak, the minigame explanation cut, event talk) drive the shared HD message manager directly instead of searching for a message process by id; the step that waited for that search now advances immediately.
- Structure: stage-name checks ("Demo17", "sea") compare sead strings; the execute function makes an unused call to the play-object accessor.
- Structure: layout shift from the shorter pointer-to-member fields, the larger HD texture-pattern animation, background-check and head-animation helpers; actor size 0x8B4 (GameCube 0x744); HD virtual destructor; HIO objects carry their vtable at the end.

### d_a_npc_kamome (Hyoi seagull, the gull Link controls with a Hyoi Pear) — verified, 2026-10-04
- Gameplay: while not possessed, the gull no longer flies around: it waits hidden at its home position (not drawn, not moving) until Link calls it; descending to Link makes it visible again. The call ends at once if the start point is more than 500 units above Link, and the descent times out after 5 s (GameCube 30 s).
- Gameplay: in the release setting the stick turns the gull's body quickly and the flight direction follows it (GameCube: the direction turned and the body copied it); a debug register switches back to the GameCube scheme. Pitch speeds are steeper (gliding and flapping), the lower flight limit is fixed at height 0 instead of below the home position, and the allowed range is widened by a debug register.
- Gameplay: new terrain handling while flying: a ground probe below the gull pitches it up and lifts it when it gets close to the ground (it no longer sinks into water), a short forward line check stops horizontal movement when a wall is right ahead, and on the sea in room 44 the gull is turned down near one spot and kept out of a circle around another (probably map-specific fixes for HD geometry).
- Gameplay: the wait animation starts at a random frame with a random speed; the area-out turn no longer switches background checks off and on.
- Graphics: no blob shadow; the gull is not drawn while hidden. The water splash and ripple are spawned through the particle manager directly.
- Structure: one wall circle instead of two, one collision sphere (also registered with the mass-collision manager) instead of separate attack and target spheres; the "return to Link" check reads one HD pad helper; the stage-name tests compare sead strings; several values come from debug registers; the GameCube checkOrder step is gone; actor size 0xBCC (GameCube 0xC28), HD virtual destructor (Delete only returns), 8-byte member pointers.

### d_a_wz (Wizzrobe, Wind Temple mini-boss and its summon portals) — verified, 2026-10-04
- Structure: the GameCube decompilation has only placeholders for this unit, so the comparison is with the GameCube symbol layout only. Actor fields shift by +0x11C (size 0x10B0); the GameCube helpers for drawing the body, the summon door and the damage ball, the hit/idle/fire-ball actions, the summon call and the sea-water check are all inlined into Draw and Execute.
- HD-only: the mini-boss intro and defeat cutscene, the projectile light and colour values and several timers read tuning offsets from a zero-initialised debug parameter block (probably HD developer tuning registers, inactive in normal play). One flag in that block makes the Wizzrobe effectively invincible (health 127), and others hold the cutscene camera or skip steps.
- HD-only: when the summon portal is created, its model's placeholder texture "__dummy" is replaced by a system texture.
- Graphics: the body and the mini-boss's second body push their alpha into the HD material colour block (a float copy of the TEV colour) and switch to a translucent draw mode when not fully opaque. When frozen, they use the HD ice material path.
- Gameplay: a fire ball that lands on any stage other than "sea" triggers the arrow-colour/lighting change helper. On the sea stage it does not (probably to keep the sea lighting unchanged).
- Structure: two branches in the summoner's enemy table lookup compare the row index with odd values that the even row formula never produces, so they are dead code (probably a leftover from a different table layout).

### d_a_npc_bs1 (Beedle, travelling boat shop and Rock Spire special shop) — verified, 2026-10-04
- Gameplay: pressing B on a shop menu message now cancels the message straight away through the HD message manager (both message numbers are set and the manager's cancel flag is raised); the GameCube build instead showed the CHOOSE/CANCEL button prompts and waited. In the talk loop the CHOOSE/CANCEL prompts now simply follow whether the shop cursor is visible.
- Gameplay: the special-shop (second shop type) greeting register has two more greetings that lead back to the menu; an out-of-range greeting register now hits an assertion instead of returning an undefined message number.
- Graphics: no blob shadow (the GameCube shadow id member is gone); the snapshot figure type differs.
- Structure: the talk flow uses the shared HD message manager instead of a message process pointer and no longer waits for a message search; the message-state function takes a second, optional message-number pointer and returns the status as a full register. The two message-range checks are swapped in the function map (the bodies match GameCube). The buy-limit helper computes the wallet maximum inline. The second shop type asserts that its four-entry item list does not overflow. The praise/full-life event cuts add the life count to a float in the play object.
- Structure: in the beast-item exchange, an unexpected message number (only possible if memory changed under the code) uses an uninitialised beast index, which GHS takes from the register holding the actor pointer; layout shift (actor 0x9B0 bytes, GameCube 0x844), HD virtual destructor, HIO vtables at the end.

### d_a_rd (ReDead) — verified, 2026-10-04
- Gameplay: a defeated ReDead's body now stays for a random 50 to 150 frames before it vanishes (GameCube: a fixed 10 seconds); the disappear effect spawns 20 units higher and at size 8 instead of 5 (same drop type).
- Gameplay: while it holds Link, a hit that leaves him in no-damage mode now calls a separate player routine (unnamed, 023F4CB0, which plays a sound) instead of doing nothing; probably feedback for invincibility.
- Gameplay: in the hit check, a hit whose resulting attack type is 1 re-arms the hit cooldown at 3 frames when a player byte at +0x69E8 is set; probably makes follow-up hits land sooner in some player state (HD-only).
- Gameplay: the attack event is ordered with a different priority/condition value (0xFF6F instead of 0x1CF).
- Graphics: no blob shadow is drawn (HD uses real shadows); texture-SRT and TEV-register animations are cleared from the model data after drawing.
- Structure: class size 0xF1C; fields shift by +0x11C up to the animation block and by +0x1D4/+0x1D8 after it (larger HD animation controllers, enemy ice/fire helpers); virtual destructor at +0xB4; the dead-ReDead search checks for a null actor; the play object is fetched once and reused in the cry/attack handlers.

### d_a_salvage_tbox (salvaged chest and water shadow) — scoped review, 2026-10-04

- **Graphics:** GameCube submits the water-shadow strip through immediate GX position and texture-coordinate calls. The HD reconstruction prepares position/UV pairs in emitter-owned buffers, updates material state, and submits the prepared geometry through a separate draw-after callback. This establishes a rendering-path change; a visible difference has not been demonstrated.
- **Structure:** The HD depth-state constructor and consumer require a 284-byte temporary object. The earlier 16-byte local verification view was incomplete and has been corrected; it does not establish a GameCube class-size difference.
- **Verification scope:** The 27 reconstructed functions have saved bounded control and directed-witness evidence. Conditional conclusions require initialized readable storage, the stated disjoint object ranges, immutable constants and explicit mock-effect limits. Actual scene assets, allocation, callback observers and complete object lifetimes remain outside that evidence. The corrected bounded scope has been accepted.

### d_a_obj (shared actor utilities) — scoped review, 2026-10-04

- Structure: The HD particle setter has an additional leading group argument. Utility calls pass the effect ID in r5, position in r6, angles in r7 and scale in r8; trailing room, color and scalar arguments use stack words. Receiver identity is included in verification.
- Structure: The two anonymous namespace movement utilities are free functions. Reproducible verification generation requires the exact namespace to be recognized without an implicit instance argument; this is a separate framework correction, not a gameplay change.
- Verification scope: All 18 functions passed O1/O2 controls with 10,000 initialized inputs. Actual math-helper companions cover the documented finite domains; negative square-root errno/TLS paths, full SDK and allocation/audio lifetimes, and historical native stack contents remain outside this evidence. No new gameplay or visible graphics difference is established here.

### d_a_npc_cb1 (Makar, the Korok cellist) — verified, 2026-10-04
- Graphics: Makar casts no shadows of any kind in HD; the GameCube blob shadow and the simple shadows of the cello, bow and propeller are all gone together with the status gate that enabled them.
- HD-only: a new NPC action (0222487C). When Link uses a door while Makar follows him, Makar waits for the event to end and is then placed at a fixed offset beside Link in Link's room; probably keeps him from being left behind on the other side of a door. Falling out of the map also routes through this action and forces the reset to his home position.
- HD-only: the "set goal" event step (02226784) now has a body. If Link is still more than 50 units from the goal after 150 frames, Link is placed on the goal facing away from Makar; probably a safeguard against the escort cutscenes getting stuck.
- Gameplay: two event steps use fixed offsets in one mode instead of the offsets from the event data; ending the event after message 0x1526 (probably the warp scene) no longer resets the event; the "plant seed" step only asserts when its talk partner is missing.
- Fix: several null checks added (animation index range in setAnm, act name before the "WAIT" compare, the attention list actor in the seed check, joint lookup bounds in createHeap).
- Gameplay: the wall check takes the second probe's angle without checking whether that probe hit; the lock-on test also accepts a second attention flag; the minimap status values differ.
- Structure: the attention and eye positions are set in setBaseMtx instead of at the end of execute; dialogue goes through the shared HD message manager (no message process search); the actor registers itself in the global companion pointer (as Medli does) and clears it in its destructor; for the Wind Temple ("Kaze") type with the restart flag, the priest save position is also written; no host-IO child.
- Structure: layout shifts from the removed shadow id, the larger HD bone animation helper (twice) and the 8-byte pointers to member; actor size 0xB40 (GameCube 0x938).

### d_a_npc_cb1_static (Makar's statics outside the actor module) — verified, 2026-10-04
- Structure: unchanged apart from the per-unit static initialiser every HD translation unit carries; the maximum flying time is still 450 frames (15 seconds).

### d_a_fm (Floormaster) — verified, 2026-10-04
- Gameplay: the tuning value for the hand's body hit sphere is 50 instead of 80. While the player is in certain states (status bits 0x402), the sphere radius is a fixed 120 instead of 1.5 times that value.
- Gameplay: on odd frames the hand's hit sphere is centred 30 units above the Floormaster's position instead of on the hand. This probably alternates the hit test between the hole and the body.
- Gameplay: a sword or melee hit with cut type 8 or 9 sets the hit cooldown to 12 frames instead of the tuning value; probably limits multi-hits from spin-type attacks.
- Gameplay: in hide mode it reappears for any nonzero area flag (GameCube: only when the flag is exactly 1).
- Structure: class size 0x101C. Fields shift by +0x11C up to the texture animation, which grew by 0x60, so later fields shift by +0x17C. HD adds a virtual destructor.
- Structure: event starts and ends are checked by name, and events are ordered with priority 0xFFFF. The demo modes and the item preparation add assertions on a null cut name or a missing created pot. Name lookups in searches are null-checked.
- Removed: debugDraw keeps only its axis drawing; the other debug shapes are compiled out.

### d_a_npc_so (the Fishman) — verified, 2026-10-04
- Graphics: the blob shadow is gone. In its place a second animated copy of the Fishman model is created and played alongside the main one; it is placed on the water surface and flattened (y scale 0.1) while he is not tilted, and is only drawn while an HD-only flag is clear. Probably an HD stand-in for the shadow/reflection on the water.
- Gameplay: jumping out of the water adds a water-pillar effect; while jumping close to his home spot he becomes talkable.
- Gameplay: near Link's boat the camera is forced to the boat-battle mode only when Link is on the boat (or in a related state) and to the water-battle mode in another player state; the GameCube version always forces boat battle.
- Gameplay: his first meeting also accepts a second item number besides the sail.
- Gameplay: when he disappears he no longer picks a new hiding tag and moves there; he only resets and switches to hiding (the respawn probably moved elsewhere).
- HD-only: after two specific map-related messages a call into the HD map/notification object is made; the save-grid check also goes through that object.
- Gameplay: a bait that he eats is marked with a different value; a missing ship in the mini-game return only asserts.
- Fix: the event-name table index is range-checked.
- Structure: the debug draw is empty (only its colour table remains); the shape tilt uses a minimum step; the swim animation speed no longer adds a debug register; dialogue state lives in the shared HD message manager; layout shifts from the HD base NPC class and the larger texture-pattern animation (actor size 0xD5C, GameCube 0xBE4).

### d_a_npc_os (Os stone-head helper, Earth/Wind temples) — verified, 2026-10-04
- Gameplay: while Link controls the statue, returning control to Link checks a single pad button routine (02007840) instead of R or START. This is probably the HD button mapping for "return".
- Gameplay: if the statue is not in its initial room, it now takes its room number from the ground polygon under it before it stops updating.
- Graphics: no blob shadow is drawn (HD uses real shadows).
- HD-only: on creation the actor registers itself, by statue number, in a global table (via 0268A8DC); the destructor clears the entry. This is probably for the GamePad map or HD tracking of the three statues.
- HD-only: two small virtual queries were added. One is true in the wait/walk player actions while airborne, or during the throw action. The other is true during the carry action.
- Structure: messages go through the HD message manager. The message-set step no longer searches the message by ID; it returns true once the message has been started.
- Structure: class size 0x97C. Fields shift by +0x11C up to the brk animation, then by +0x178 from the collision circles (larger HD brk controller, no shadow id), and by +0x170 from the pointers to member, which are 8 bytes in HD instead of 12. The tuning object moves its vtable to the end, so its fields are 4 bytes lower; its values are unchanged.

### d_a_npc_hr (Zephos and Cyclos, the wind gods) — verified, 2026-10-04
- Graphics: Zephos and Cyclos load separate body models (GameCube used one body model and swapped in Cyclos' material table at draw time); the draw no longer swaps materials.
- Graphics: every colour written to the cloud emitters (Cyclos' cloud tint, the hit flash and its fade) first goes through a new helper that converts the 0..255 value to linear light (a 2.2 power curve, clamped to 0..1) before storing it; probably compensates for HD's linear-space rendering. Emitter scale writes fill two scale vectors.
- Structure: layout shift of the fopAc_ac_c base (+0x11C), the larger HD texture-pattern animation and the 8-byte GHS member-function pointer (size 0x940); the message is reached through the HD message manager instead of a message-process pointer; the tornado joint position lookups are inlined with a fallback to the tornado's own position when it has no model.
- Structure: the height lookup on the tornado asserts the tornado exists and returns the top joint without it; the look-back logic reads the facing angle once before choosing the look mode; the hit-flash brightness clamp sends an invalid value to 0.
- Structure: the "rapid move" sound in event movement is started at the actor's position instead of its eye position (probably no audible difference).

### d_a_npc_p2 (Zuko, Niko and Mako, Tetra's pirates) — verified, 2026-10-04
- Structure: the GameCube decompilation of this actor has only unmatched stubs, so this comparison rests on the GameCube function list and sizes; every HD function was rebuilt from the HD code. The actor (0x980 bytes) derives directly from the base actor; the HIO objects carry their vtables (child at the end, parent first).
- Graphics: the GameCube build has a separate shadow-drawing function; HD has none and its draw function registers no blob shadow (HD renders real shadows).
- Structure: talking drives the shared HD message manager directly (status queries and message set calls) instead of looking up a message process by id; the start-stage check ("Asoko") for Niko compares sead strings.
- Structure: both "nearest object" searches used by Niko's rope course (lift and rope) record the found actor in the same field, and the rope-talk cut takes the rope from there; probably the same as on GameCube, but it means the lift search result is reused if no rope is closer.
- Structure: the heap setup asserts on the first dagger model's data a second time where the second dagger is loaded (probably a copy-paste slip that also exists on GameCube); one goal-waiting mode makes an unused call to the play-object accessor.
- Structure: the animation function refreshes its morph-time table from the HIO values every call (the table lives in writable data).

### d_bg_s (background collision system dBgS) — verified, 2026-10-04
- Gameplay: GetPolyColor (polygon colour id, used e.g. for lighting/footstep colour lookups) now first asks the stage data for its stage info and returns "no colour" unless the stage type is 1; probably polygon colours are only honoured in one kind of stage in HD.
- Structure: ChkMoveBG / ChkMoveBG_NoDABg look the polygon up in the global collision system of the play object instead of the object they are called on. The GameCube asserts (index range checks, including those of the inlined per-polygon info lookups) are still compiled in. The constructors of the roof check (0x4C bytes) and sphere check (0x50 bytes) are emitted in this unit. WallCrrPos and dBgS_CrrPos::CrrPos have no HD copy in this unit (probably unused and stripped).
- Structure: matcher names corrected: 024EF218 is GetPolyId2 (not ChkPolyHSStick), 024EF398 is ChkPolyHSStick (not GetPolyId2); 024EEC9C GetPolyCamId, 024EF324 GetCamMoveBG and 024EF340 GetRoomCamId were unnamed.

### d_bg_s_acch (actor ground/wall/roof/water collision dBgS_Acch) — verified, 2026-10-04
- Fix: the triangle plane returned by the collision system is null-checked before use, in the ground check (plane copy skipped) and in the line check (a missing plane is treated like ground instead of being dereferenced).
- Structure: the per-circle wall line check uses the plain base line-check object (no ground-status override); clearing a circle's wall hit in Init also resets its polygon info; GetWallAddY takes only the vector. The NaN and range asserts on the actor position at the start and end of CrrPos are still compiled in.

### d_a_pz (Princess Zelda in Ganon's Tower) — verified, 2026-10-04
- Structure: the GameCube decompilation has only placeholder stubs for this actor, so all 84 functions were written from the HD code; behaviour could not be compared line by line. The actor grew from 0x1049 to 0x16B8 bytes (larger tev/animation objects, two extra pointer tables and four draw packets).
- Graphics: HD draws Zelda's face in several passes: four extra draw packets, with the eye/brow "damage" materials, eye and brow highlights and face-shadow materials switched visible/invisible between passes, and a final pass over all remaining materials.
- HD-only: a second texture-pattern (btp) animation alongside the first; the btp's material links are patched so it drives every material except the mouth and the two face shadows (in heap creation, demo and eye-pattern changes).
- HD-only: the eye material animation (daPz_matAnm_c) also flags the texture matrices it changes as dirty; a 16-entry table of named face materials (eyes, brows, head, hair) is resolved at heap creation.
- Structure: the matcher's "setEyeBtp" (02454520) is really setEyeBtk; the real setEyeBtp (02454114), eventOrder (02453A00) and setBowString (024540C0) were unnamed.


### d_a_coming3 (barrel challenge spawner) — verified, 2026-10-04
- Structure: layout shift only for the actor's own fields (+0x118, class 0x598 bytes); the barrel spawn helper (make_coming), the barrel's exit request and the "no enemy nearby" search are inlined into the actor's functions; the effect sound call takes the room's reverb as an extra argument.
- Gameplay: no behavioural difference found (same 3600/5000 distance limits, 30-frame delay, blue/yellow rupee rewards, state machine).

### d_a_player_main#06 (Link: demo procs tail, ladder, ledge hang, vine climb, wall sidle, crawl, grab start; 02423D10..0242DAC7) — verified, 2026-10-04
- HD-only: four extra demo procs with no GameCube counterpart (0242643C, 024265A0, 02426964, 0242705C). They replay the letter animations with a text-input step and an extra event camera that follows a thrown actor until it is far away or hidden, then reset the camera; probably the HD message bottle (Tingle Bottle) writing and throwing sequence.
- Gameplay: while crawling in first-person view, Link only turns toward a sideways stick direction and stops 45 degrees short of it (GameCube: free turning toward the stick).
- Gameplay: the ladder "can I keep climbing" probe height and the crawl auto-move wall radius are read from global tuning values instead of fixed constants (probably the same values by default).
- Fix: many background-polygon lookups (ledge hang, ledge move, vine climb correction, wall sidle, crawl side walls, climb-up ground check) now tolerate a missing polygon instead of dereferencing it; grabbing checks the held actor for NULL before testing for the heavy stone.
- Structure: the debug tuning (HIO) values are folded into constants; ground and roof check positions are written straight into the check objects; joint-matrix reads mark the model's matrices dirty; several float comparisons are restructured so NaN inputs take different branches than in the GameCube source (no practical effect).
- Structure: layout shift of daPy_lk_c as measured in phase 1; HD-only tail fields at 0x8268..0x8277 hold the followed actor, a camera angle and a height for the bottle camera.

### d_a_npc_rsh1 (Zunari, Windfall travelling merchant) — verified, 2026-10-04
- Gameplay: new dialogue before you own the sail (messages 0x2892–0x2895), gated by two HD-only save flag bits, an event bit and the clear count; one branch ends the talk, the other leads into the purchase offer (probably the HD Swift Sail offer). The sail check also accepts a second sail item (0x77, probably the Swift Sail).
- Gameplay: when Link bumps into Zunari while standing inside the shop area, the shop-out event starts instead of a normal greeting.
- Gameplay: in the shop-exit action a global byte (0x101D5F45) can skip straight to waiting (probably a skip/debug switch). The shop CHOOSE/CANCEL button prompts follow whether the shop cursor is shown.
- Graphics: no blob shadow; the talk camera's first data set is also used for the new messages.
- Structure: talk flows use the shared HD message manager (no message process pointer, no search step); pressing B on shop messages goes through one shared cancel path that sets the manager's cancel flag instead of forcing button prompts; the message-state function takes a second, optional message-number pointer and returns the status as a full register. The GameCube out-of-range attention-distance write lands on a neighbouring distance byte. The talk-area check is inlined into the order function. Layout shift (actor 0xAD0 bytes), HD virtual destructor, HIO vtable at the end.

### d_a_warpgn (warp portal to Forsaken Fortress, Gmjwp) — verified, 2026-10-04
- Graphics: after copying the room's konst colour into each material's TEV konst colour 1 (as on GameCube), HD also converts that colour to floating point (divided by 255, scaled by 1.0) and writes it into a per-material shader parameter block, flagging the material dirty; probably because GX2 has no TEV konst registers and the colour must reach the shader as a uniform.
- Gameplay: the arrival sound uses the "down" warp-out effect (as the GameCube PAL build) instead of the "up" variant used by the Japanese and US builds; the departure sound is unchanged.
- Structure: members shift with the larger HD base actor (model pointer 0x298 -> 0x3B4, class size 0x5C0); animation objects are created through out-of-line helpers instead of inline constructors; resource lookups take a string object; the particle-set call no longer uses the manager pointer it is passed; the model-create call ignores its third (display-list flags) argument in HD.
- Structure: the demo-object lookup asserts when the demo manager is missing.

### d_a_tag_light (projected light tag) — verified, 2026-10-04

- Structure: the actor fields shift by +0x11C relative to the GameCube layout; the model pointer, type, alpha, projection matrix and volume scale retain their relative positions.
- Graphics: after updating material alpha, HD converts the material colour to normalized floating-point RGBA, converts RGB to linear colour and updates the renderer material parameters. The GameCube helper only changes shape visibility and the byte alpha.
- Removed: the HD draw function omits the GameCube calls that submit the projected light-model colour and matrix; it retains environment lighting, texture animation, material-alpha updates and the ordinary model update.

### d_a_stone2 (liftable/breakable stones and skull-shaped rocks) — verified, 2026-10-04
- Removed: the draw function no longer submits the GameCube simple blob shadow under the stone (the GameCube shadow helper has no HD counterpart); probably replaced by HD's real-time shadows.
- Structure: members shift with the larger HD base actor (phase request 0x2C8 -> 0x3E0, class size 0x7C4); the background/normal draw-list switch around the model update writes the HD draw-buffer globals directly; the temporary lighting structure used for the lift-smoke colour is default-constructed in three inline blocks before its init call; resource lookups pass a string object; parameter decoding goes through an out-of-line helper and attribute lookups assert on an out-of-range stone type.
- Source: re-verified from an earlier unfinished reconstruction; the unit's trailing destructor/weak functions were added.

### d_a_btd (Gohma, boss of Dragon Roost Cavern) — verified, 2026-10-04
- Structure: supersedes the "layout audited; verification incomplete" entry above: all 36 functions of the unit now pass 10,000 generated inputs at two seeds.
- Structure: the actor grows from 0x6E94 to 0x70D8 bytes. The model/animation pointers start at +0x3D8 (GameCube +0x2BC plus the usual 0x11C base shift); later fields shift by a further 12 bytes because HD has four model/texture-animation/colour-animation sets instead of three.
- HD-only: the extra set is probably a dedicated death animation set: the three animation getters return it while Gohma is in her death action, where the GameCube getters return the phase-two set.
- Structure: the GameCube move() routine is one 12 KB function in HD with the wait, the six attack variants and the damage sequence inlined (the function map names it after damage); demo_camera is inlined into the execute function and the rock-debris helper into its caller.
- Structure: the weapon search callback (hookshot rope / boomerang) gains a null check on the actor before reading its name; several sound calls check the actor's position pointer for null first (always true in practice).
- Structure: the particle setter receives four stack arguments as on GameCube; the light colour components are stored as halfwords.

### d_a_warpdm20 (warp ring / whirlpool to Hyrule shown in the warp demo) — verified, 2026-10-04
- HD-only: when the actor is created after Zelda has awakened, HD also auto-stocks the Tingle letter (event register 0xB203) through the letter system, in addition to setting the same event bit as GameCube; probably part of HD's changed letter/Tingle handling.
- Structure: members shift by 0x11C with the larger HD base actor (phase request at 0x3AC, class size 0x418); the ripple particle is created through the generic particle setter (type 5) instead of the ship-tail wrapper; set_mtx is inlined into create-init and execute and copies the translation matrix from a shared HD buffer; demo_execute checks the demo actor id range (1..32) before asking the demo manager and toggles drawing through the draw-tag helpers; resource lookups pass a string object and assert per resource.
- Structure: check_warp and getSeaY are out-of-line but unnamed in the function map; the play-object getter is called once more than needed in check_warp (result unused).
- Source: re-verified from an earlier unfinished reconstruction; the unit steering was consolidated.

### d_a_ship (King of Red Lions: sailing, cannon, salvage crane) — verified, 2026-10-04
- Gameplay: the Swift Sail. While it is active the boat's top speed rises to 100 (GameCube 55), a dedicated "sail power up" sound plays and the wind follows the boat's heading; most speed thresholds and scales (cruise sound, sail texture scroll, forced moves in tornadoes and whirlpools, the event slowdown) are rescaled to the new top speed. Jump thresholds also changed (wind 20, forward speed 30, jump strength from the vertical speed above 20), and several animation frame thresholds moved from 7 to 9.
- Gameplay: the salvage crane's rope has 125 segments instead of 250 and winds in faster; the boat can be steered with the stick while the crane is idle.
- Gameplay: hitting an obstacle hard now also damages Link, and the crash cooldown counts down in the main update instead of the crash check.
- Gameplay: cannon aiming can be held on R with its own camera (with an inverted-axis option), and extra buttons fire the cannon and the grappling hook. HD adds a cannon-sight trajectory preview: the shot is simulated in 128 steps, each step tested against enemies (via 128 extra sphere colliders), walls (a line check every fourth step) and the water surface; a dotted line of 25 segments and an impact marker (fallpoint model with its own animation) are drawn where the shot would land.
- Gameplay: in the Tower of the Gods from room 7 on, the boat is parked at a fixed spot and not drawn (probably because Link leaves it behind there).
- Gameplay: the whirlpool's event flag is only set once Link has a bomb bag; the out-of-range handling stops the boat while it is flying.
- Graphics: a water shadow model with a scrolling texture animation replaces the GameCube real-time shadow; its rate and alpha follow the boat's state.
- Graphics: when Link looks down from the boat, HD hides the tiller instead of the whole body; the cannon and crane models scale with the mast animation.
- Graphics: in the stage "sea_E" (probably the ending sea) the textures fn_head1, fn_main1 and new_fn_eye of the boat's models are replaced with their "_D46" versions, and the actor waits for the "Demo46" resources before building its models. A texture matrix of the material "m_fn_main_hashi" is captured at creation.
- Structure: the class grows to 0xD19C bytes; the rope line object is 0x188 bytes and is followed by HD members (a beacon model also used by the sight segments, the impact marker model and its animation, the water shadow model and texture animation, the sight packet and its simulation state, the 128 sight colliders). Message selection no longer takes the message number argument; the crash check returns a result; particle callbacks are ended instead of removed; string comparisons use string objects.
- Removed: the GameCube shadow id and its real-time shadow call.
- HD-only: helpers for the cannon sight (packet create/draw, segment matrices, one simulation step), the "sea_E" texture replacement, the Tower of the Gods check, and the shadow texture rate.

### d_a_warphr (warp portal to Hyrule, Ghrwp) — verified, 2026-10-04
- Structure: layout shift only for the actor members (+0x11C with the larger HD base actor: model pointer 0x298 -> 0x3B4, class size 0x424); the HD build keeps the second (projection) emitter and the monotone warp-out effect of the non-Japanese GameCube builds, and the departure/arrival sounds match the US build.
- Structure: the warp-out cutscene reads the demo frame counter from a global instead of asking the demo manager (no null check on the manager in that path); the demo-actor and demo-camera lookups assert when the demo manager is missing; demo actors are looked up only for ids 1..32.
- Structure: resource lookups take a string object; animation objects are created through out-of-line helpers; invalidating a particle emitter also clears its stored id in HD (probably the HD inline of becomeInvalidEmitter); the "Hyrule" start-stage comparison runs through string-object helpers.
- HD-only: a static initializer for the arrival-target guard and math constants, and per-unit copies of the actor and string-object virtual helpers (compiler-generated).
- Note (not an HD difference; game test 2026-10-04): `draw` (024DAA1C) reads uninitialised stack (eye, angle.y) when the demo camera is NULL, in the original and already in the GameCube source (sp1C/sp08). The candidate yields zeros there; in the game the projection-emitter matrix (+0x3D0) differs in Hyrule. Visual only.

### d_camera (follow camera dCamera_c: all camera engines, setup and per-frame processing) — verified, 2026-10-04
- Graphics: the camera works in a 1280x720 frame: the default view, the telescope/scope views, the "point in sight" test and the actor-in-sight radius use 1280x720 instead of 640x480, and the picto box / scope wipe is mapped to that viewport (zoom start 1280 wide, view height 720).
- Graphics: a blur fade is applied while shaking; riding the boat with the boost state triggers a short motion blur with a vibration (probably the Swift Sail boost).
- Gameplay: first-person aiming also reads the GamePad gyro (pointing direction while the C stick is released, a shaped C-stick response otherwise, Y inversion from the save options); first-person aiming while swimming has its own pitch/yaw ranges and banks the view; the zoom uses the main stick's Y; the C-stick-down shortcut in first person is replaced by a GamePad button; the manual (C-stick) camera honours a "camera X inverted" save option.
- Gameplay: the follow camera's entry transition is weighted by the remaining frames for radius, latitude, yaw and fovy (GameCube: a constant ratio), the free-turn decay is 0.86 times the style value, and riding a seagull adds a debug-register controlled yaw of the centre and a fovy limit.
- Gameplay: Z-targeting flips sides by negating the angle difference, sways using the target's longitude while the player is hidden, and accepts the style's default radius directly when it is valid.
- Gameplay: conversations use an HD shot table (types 11 to 31: over-the-shoulder, side and close-up shots chosen per line, plus special cases when the partner is the ship or actor 0xB6) on top of the GameCube-style two-shot, which keeps both speakers in view by rotating away from walls and colliders.
- Fix: the shake pattern buffers are 5 bytes (the GameCube copied 4-byte buffers); wall push-outs check that the hit plane exists before using its normal.
- HD-only: the camera keeps a sead look-at camera and projection that are filled each frame; a near-plane adjustment for Hyrule and the Demo07 cutscene; smoothing of the Demo46 cut; an environment-light dependent zoom.
- Structure: the class grows from 0x78C to 0x8E0 bytes (three floats inserted at 0x14C, +4 shift from 0x158, HD members from 0x610, a larger setup block at 0x73C); all angle/globe/vector operators are out-of-line calls; the camera engines that are unfinished stubs in the GameCube decompilation (talk, tower, ride, hung, manual, tornado, vomit, shield) are now written out.

### d_a_obj_try (Try actor) — verified, 2026-10-04
- Structure: The actor grows from the documented GameCube size of 0x66C to the HD profile size of 0x7E8. The saved correction position moves from +0x63C to +0x7B8 and its angle from +0x648 to +0x7C4.
- Structure: The available GameCube actor source leaves the substantive methods as nonmatching stubs. Those stubs do not establish a gameplay or graphics difference from the HD implementation.
- Verification qualification: All 53 functions pass 10,000 generated inputs at seeds 1 and 7; coverage is 447/450 and 445/450, with three documented infeasible Wait blocks. Mutation classifications retain their generated-domain, initialized-fixture and native-validity qualifications. Broader callback and lifecycle research remains unfinished.

### d_a_obj_firewall (ring-shaped fire wall, daObjFirewall_c) — verified, 2026-10-04
- Structure: no GameCube comparison possible beyond names and order: every GameCube function of this unit is still an unmatched stub and its class has no member layout, so the HD layout (class size 0x6F8, members from 0x3AC after the larger HD base actor) was reconstructed from the HD code alone. Function set and order match the GameCube symbol list.
- Structure: resource lookups take a string object; the switch parameter is read through an out-of-line parameter helper; the burn-out colour animation reload asserts when its resource is missing.
- HD-only: a static initializer for the unit's angle/limit globals, and per-unit copies of the particle-callback constructor/destructor and an empty virtual (compiler-generated).

### d_a_obj_ftree (forest tree) — verified, 2026-10-04
- Structure: HD uses a 0x820-byte actor layout. The phase field moves from GameCube +0x29C to HD +0x3B4, the base matrix from +0x2A8 to +0x3C0, and the mode from +0x680 to +0x7F4. The offsets do not follow one uniform shift.
- Verification qualification: All 79 functions pass 10,000 generated inputs at seeds 1 and 7; coverage is 443/446 and 441/446, with three virtual-dispatch blocks infeasible for the fixed record tables. Mutation classifications retain their observer, initialized-fixture, source-domain and native-validity qualifications, including one source-qualified nontermination difference. Native resource, GPU and lifecycle research remains unfinished.

### d_a_dai_item (Windfall display-stand items: flowers, flags, pinwheel, idols, statues) — verified, 2026-10-04
- Structure: layout shift only for the actor (class 0x7E4 bytes in HD); creation, carrying, dropping, the wind-driven flags and pinwheel, the idle animation timers and the Fountain Idol particles behave as in the GameCube retail version. (Supersedes the qualified entry above for acceptance: all 39 functions pass 10,000 inputs at seeds 1 and 7.)
- Structure: the joint callbacks for the pinwheel ("top") and the Fountain Idol ("tuboko_head"/"tuboko_base") are found with a name-table index lookup instead of a loop over all joints with string compares; the callback itself is the shared daiItemNodeCallBack (as in GameCube retail, outside this unit).
- Structure: the HD model-creation helper ignores its third (display-list flag) argument; the actor still passes the GameCube values. The landing sound passes the room's reverb explicitly.

### d_a_pirate_flag (pirate ship flag, cloth simulation) — verified, 2026-10-04
- Graphics: the GameCube display-list packet is replaced by an HD GX2 renderer: double-buffered vertex buffers for the front and back faces (position + normal + texture coordinate), a shader looked up by name in the shared shader archive, two textures (the cloth texture and one from the pirate-ship archive) and up to three render passes per material; the ship's light colours are converted to float colours each frame.
- Gameplay: Execute and Draw also bail out while a status bit (4) of the parent ship's actor flags is set; the GameCube only checked the debug HIO switch (Execute) and the ship's draw flag (Draw).
- HD-only: in the cloth move function, when a play-state flag is set and the current stage is "Demo46", the wind vector is scaled by the global wind power and, unless a global switch is set, the cloth simulation runs 60 steps in one frame (probably to pre-settle the flag at the start of that cutscene).
- HD-only: compiler-generated helpers for the GX2 packet (constructor, destructor with buffer/material release, vertex-buffer construct/destruct, material-block allocation, matrix and colour copy helpers) and an initializer that builds the vertex buffers and textures.
- Structure: get_cloth_anim_factor is inlined into the HD move function (the matcher's seven-argument signature for 023D1EA4 was wrong; it takes only the actor), get_cloth_anim_sub_factor stays out of line (023D176C); the packet lives at actor +0x3D4 (HD packet size 0x1B00, positions at packet +0xCC, normals +0x324, back normals +0x57C, velocities +0x7D4).

### d_cc_d (collider classes dCcD_Stts / dCcD_GObjInf / Cps, Tri, Cyl, Sph) — verified, 2026-10-04
- Structure: layout and sizes unchanged from the GameCube; the GStts part of the status object carries its own vtable at its start. The actor lookup of a hit partner (GetAc) is an out-of-line function in HD and searches by process id directly. Destructors of all collider classes, the attack/target/common sub-object constructors and the sphere collider constructor are emitted out of line in this unit.
- Structure: the null-object assert in dCcD_GetGObjInf is still compiled in and, after the assert, the function returns null instead of dereferencing.
- Structure: matcher names corrected: 02515980 is the capsule collider destructor (not dCcD_GObjInf's; that is 02515908), 02515A70 is the cylinder collider destructor (not dBgS_Acch's), 025161D8 is GetAtHitGObj (not a second GetTgHitGObj).

### d_cc_s (collision manager dCcS) — verified, 2026-10-04
- Fix: hit-mark processing returns early when the target has no status group object (the GameCube dereferences it). When an attack/target hit is recorded, the "attacker has no actor" flag test checks the target status group for null, and the target-side bookkeeping (special-attack copy, hit process id, shield and no-actor flags) is skipped entirely when the attacker has no status group; probably a null-pointer guard.
- Structure: the camera-obstruction test builds its capsule and asks the capsule to test each collider shape (the GameCube asks each shape to test the capsule); probably a devirtualised double dispatch with the same result. The push-out distance is computed with fused multiply-adds in a different summation order, so its float rounding can differ slightly.
- Structure: the manager's destructor slot points at a runtime-error stub (probably never destroyed); DrawAfter is inlined away; the unit has its own copy of the base class's "no hit" virtual.

### d_a_gm (Mothula, mini-boss / Mothula larva) — verified, 2026-10-04
- Structure: the GameCube decompilation of this unit consists only of "Nonmatching" placeholders, so the HD code was written from the WWHD binary; differences below are structural, behaviour could not be compared line by line.
- Structure: the actor is 0x1290 bytes in HD. The GameCube helpers wing_cut_stat, fuwafuwa_set, fly_move, action_dousa, action_hane_rakka, action_uchiwa_dousa and action_totugeki have no out-of-line copy in HD; they are probably inlined into daGM_Execute, which grows to about 12 KB (GameCube about 3.3 KB) and also contains an inline particle-emitter state machine.
- Structure: body_atari_check, the Mothula hit reaction, keeps one switch over the hit attack type; the cut type of the player's sword swing decides between the weak and strong reactions as on GameCube (probably unchanged).
- Structure: daGM_Create checks the model's name and then looks up the four wing joints by a name-table index lookup to install the joint callback, instead of the GameCube joint loop with string compares (same pattern as other HD units).
- Structure: several tuning values (heights, radii, distances, frame windows, demo camera offsets) are read from a static block and added to the GameCube constants; the block is probably a leftover debug/HIO tuning block that is zero in the retail game, so the effective values are the GameCube ones.
- HD-only: out-of-line compiler helpers in the unit range: a 0x22C-byte sub-object constructor (02150578), a cosine table lookup (02151294), a static destructor (021510EC), the deleting destructor (0215400C) and an empty virtual (02154138).

### d_a_goal_flag (finish-line flag and boat-race controller) — verified, 2026-10-04
- Structure: layout shift (actor 0x2FB8 bytes instead of 0x172C; the cloth packet sits at 0x3AC and is 0x264C bytes). The flag cloth is drawn by a shader-material packet with double-buffered GX2 vertex buffers instead of a GX display list; the GameCube's separate toon-texture setup step is gone (probably folded into the material).
- Structure: the race start countdown, the result screen and the failure display are persistent HD UI objects reached through a global object, not message processes the flag creates and looks up by id. The flag resets them when a race starts and when it is deleted, gives the result screen only the finish type (the GameCube also passed remaining time and rupee count), and marks the failure display on a failed race.
- Fix: the time-limit reduction read from the race event register is applied only when it is smaller than the limit; on GameCube a large reduction would wrap the 16-bit limit to a huge value.
- Fix: stopping the timer and the time-up sound now happen once, guarded by a new "timer ended" flag (also for crossing the line backwards). On GameCube the time-up branch stopped the timer and restarted the sound every frame until the end camera event was accepted (probably an audible repeat).
- Fix: on delete the flag picks its sub-archive with the low parameter bit instead of indexing a two-entry table with the whole parameter byte (GameCube could read past the table); for parameter 255 this probably still differs from the archive chosen at creation.
- Structure: time-up is tested as "remaining time at or below zero" instead of "exactly zero"; the race-start step makes one extra unused play-object call.

### d_a_npc_fa1 (recovery fairy) — verified, 2026-10-04
- Graphics: the draw step no longer places the round blob shadow under the fairy at ground height (probably replaced by HD real-time shadows); lighting setup, model draw and the snapshot registration are unchanged.
- Structure: layout shift only otherwise (actor 0x8C0 bytes). No behavioural difference was found in the fairy types (normal, timer, area, bottle release, Baba bud, hover) or their movement modes; the code follows the GameCube source apart from the layout.

### d_a_bgn3 (Puppet Ganon, phase 3: the worm) — verified, 2026-10-04
- Gameplay: when the worm runs into a wall (while crawling, stunned or knocked back), HD turns it to face the centre of the arena; the GameCube turned it around by 180 degrees.
- Gameplay: the forward wall probe is cast from 300 units above the head instead of 100 (probably so low geometry no longer counts as a wall).
- Gameplay: while it homes in on the bait, HD also sets the body-segment trailing distance to 30 (it then eases back to the normal value); the GameCube left it alone. Probably makes the body visibly stretch while chasing bait.
- Graphics: Draw submits the hair line sets of six body segments instead of eight; the seventh segment's hair is still simulated but not drawn, and the eighth (never initialised on GameCube either) is skipped.
- Structure: the hair colour is written as fixed values every Draw instead of being copied from the HIO in Create; bgn3_class is 0x1238C bytes with ten 0x19D8-byte body segments; the HIO object is 0x58 bytes with its vtable last; move, all action functions, damage_check, the effect/sound helpers and part_control are inlined into one 11.5 KB move function; the tev copy into each segment in Create is member-wise; the last hit also clears a play-object float next to stopping the music.
- Fix: the face-hit handler null-checks the hit source before reading its name (arrow check).

### d_a_npc_btsw2 (traveling merchant Btsw2) — verified, 2026-10-04
- Graphics: the draw function no longer registers a blob shadow (the GameCube version requested one
  above the merchant every frame); the shadow-id field is gone from the actor. Probably replaced by
  HD's real shadow rendering.
- Structure: actor grows from about 0x74C to 0x8BC bytes. The fopNpc base shifts the fields by 0x118;
  the texture-pattern animation helper is 0x60 bytes larger; the action member-function pointer
  shrinks from 12 to 8 bytes (this-adjustment, vtable index, target), so the trailing path/timer/state
  fields end up 0x170 bytes later than on GameCube.
- Structure: messages, path walking, attention, look-back and the event order logic match the
  GameCube version apart from layout and inlined helpers.

### d_a_sea (the open sea surface) — verified, 2026-10-04
- Graphics: the GameCube's GX sea rendering is replaced by a GPU shader program ("wave_draw"). create sets up an index buffer for the 65x65 grid and nine vertex-buffer sets, each double-buffered (two buffers per entry, both bound to the program). The new draw function binds the program, its textures and the material's texture-matrix uniforms, and draws the grid as 64 indexed strips.
- Graphics: vertex upload and shader uniforms (sea colours, matrices) are done in daSea_Draw, when the draw is queued, instead of in the packet's draw.
- HD-only: a "skirt" of flat quads is drawn around the 65x65 wave grid out to ±450000 in x and z, probably so that the sea reaches the horizon at HD draw distances.
- Removed: the "ADMumi" stage no longer forces the sea flat.
- Gameplay: the wave grid is computed every frame even when culling is stopped (the GameCube returned early); probably no visible difference.
- Gameplay: nothing is drawn in stage "Siren" room 18.
- Structure: the HD constructors of the wave info, the packet and the vertex-buffer helpers allocate their own storage when called with a null `this` (GHS convention); the sea packet is a function-local static constructed on first use.

### d_a_obj_tapestry (burnable cloth tapestry) — verified, 2026-10-04
- Graphics: the GameCube display-list path (vertex setup, texture load, TEV stage and TEV colour-register setup, packet draw) is replaced by a GX2 path: an initializer builds double-buffered vertex buffers (two faces, 48 vertices each), material groups and textures; each frame the simulated positions and normals are uploaded into the inactive buffer, which then becomes the active one, and the surface is drawn as indexed strips with separate vertex, pixel and geometry uniform blocks.
- HD-only: compiler-generated helpers for the packet and its members (constructors/destructors of the vertex buffers, triangles, collision status, fire effects and light; a matrix copy and signed/byte colour conversion helpers) with no GameCube counterpart; the packet destructor frees the GPU vertex buffers through the heap allocator.
- Structure: actor size 0x720 (action descriptor at +0x714); the packet is 0x2520 bytes (vertex buffers at +0xA4, active-buffer index +0x9F4, two position buffers from +0x106C, acceleration +0x2384, wind +0x2424). The cloth, wind, fire and collision simulation keep the GameCube function split (the GameCube bodies are nonmatching stubs, so their logic could only be checked against HD, not compared line by line).

### d_a_saku (brown wooden barricade) — verified, 2026-10-04
- Graphics: the material alpha helper (02463408, GameCube matAlphaAnim with changeXluMaterialAlpha inlined) takes the model instead of the model data, marks the model translucent when the alpha is below 255, and besides the TEV konst colour also writes that colour (after a colour conversion, probably to the shader's colour space) into a shader uniform of each material, flagging the material for re-upload.
- HD-only: a small helper (02463354) converting an 8-bit RGBA colour to four floats, used by the material alpha path.
- Fix: the culling matrix is only taken from a model when that model exists (CreateInit and broken check for a missing model; the GameCube dereferenced it unconditionally).
- Structure: actor size 0x1024; collision cylinders keep a 0x130-byte stride; heaps at +0xF30, models at +0xF40, state per half at +0x1014, switches at +0x101C/+0x1020. Compiler-generated constructors/destructors (actor, cylinders, smoke callback) and the static HIO initializer appear as separate functions.

### d_a_auction (Windfall auction controller, the bidding event) — verified, 2026-10-04
- Gameplay: the prize pool has a fifth entry in HD: an item id unused on GameCube (0x77, probably an HD-only item) with a starting bid of 100, reusing an existing name message. It has no event flag; it counts as obtained when a particular save-file inventory slot already holds it. The random prize pick counts five candidates and can choose among the first five (GameCube: four and four).
- Gameplay: after an NPC bid, the bidder's follow-up line (next_msgStatus, message 0x1CF9) switches to its "hurry" variant only when less than 10 seconds remain; the GameCube source switches at 60 seconds.
- HD-only: three small helpers: grant an item's event flag (02059838), "is this prize already obtained" including the inventory-slot check above (0205A608), and "is any prize still available" (0205CD90), which the auctioneer NPC (d_a_npc_auction) calls when choosing its message.
- Structure: actor size 0x954; verified unchanged. Compiler-generated pieces (execute/draw/create/delete thunks, deleting destructors, a string helper's destructor and terminator, static initialiser) appear as separate functions.

### d_a_pw (Poe, lantern-carrying ghost) — verified, 2026-10-04
- Graphics: Draw sets each material's alpha from the Poe's fade value and, besides the TEV colour, writes that colour (after a colour conversion, probably into the shader's colour space) into a per-material shader uniform, flagging it for re-upload; the same pattern as other HD material-alpha paths.
- HD-only: a small helper (02449ECC) converting an 8-bit RGBA colour into four floats, used by Draw.
- Structure: actor size 0xF8C. Most GameCube functions of this unit are nonmatching placeholders, so the HD versions were written from the WWHD code and a line-by-line behavioural comparison with GameCube is limited; a "Link is possessed" flag is kept in a byte of static data (101CF0A4), probably so that only one Poe possesses Link at a time. Compiler-generated pieces (lantern/fire constructor, deleting destructor, static initialiser, an empty virtual) appear as separate functions.

### f_op_actor (actor process methods fopAc_*, fopAc_ac_c constructor/destructor) — verified, 2026-10-04
- Gameplay: HD-only rule in fopAc_Execute: an actor of the enemy group that ends up more than 15000 units below the player is deleted (unless a status bit opts out); probably a guard against enemies falling out of the world.
- Gameplay: actor execution also stops while the HD pause check reports an open system overlay (besides the menu flag), unless the actor ignores pauses.
- Structure: fopAc_ac_c is 0x3AC bytes (larger lighting block, +0x11C shift), has a C++ vtable at +0xB4 and an HD-only construction-state byte at +0x3AA (raw / constructed / destroyed); the constructor initialises three default light blocks, and fopAc_Delete runs the actor's virtual destructor itself. The "not drawn" condition bit is 0x04 (GameCube 0x10). The demo actor lookup goes through a global demo object pointer with an assert.
- Fix: fopAc_IsActor tolerates a null process.

### f_op_actor_iter (actor iterator fopAcIt_*) — verified, 2026-10-04
- Structure: layout shift only (the actor tag queue moved to 101F3328).

### f_op_actor_mng (actor manager helpers fopAcM_*) — verified, 2026-10-04
- HD-only: actor heaps are routed by fopAcM_entrySolidHeap: the estimate passed by the actor is ignored, the parent heap depends on the stage (the "sea" stage swaps the two candidate heaps) and on a handful of actors (some use the ocean heap or fixed sizes); if creating the heap fails, it retries in the other heap and prints a warning. A new inner helper (025D6174) does the size estimate / largest-free-block fallback and names each heap after its actor.
- Gameplay: fopAcM_createItemFromTable has an extra rule for action 0xD: at 40% health or less any table is replaced by the life-based heart tables. fopAcM_createIball sends the item tables 0x1D-0x1F, 0x24 and 0x2B to item table 0x29 with that action (probably so these enemies drop hearts when the player is low).
- Gameplay: fopAcM_createRaceItem turns a heart into item 1 when an HD save setting is active (probably Hero Mode, where hearts do not drop).
- Graphics: culling tests a second view (probably the GamePad screen) through the HD renderer after the J3D clipper: an actor is culled only if it is outside both views. For the predefined cull spheres the second test uses the actor's own sphere radius field (probably an oversight).
- Structure: event types are renumbered (a new type 9 is inserted after the three "show item" types; catch, treasure and change move up by one) and there is an extra talk order for the new type; fopAcM_orderItemEvent takes the hind flag as an argument; the player's crash flag and grab id are reached through virtual functions; fopAcM_createChildFromOffset asserts and fails cleanly when the parent is gone (the GameCube code dereferenced it); fopAcM_SearchByID treats id -1 as "not found"; several null-safe id lookups; GameCube fopAcM_createItemFromEnemyTable and fopAcM_isItemForIb are inlined into their callers; fopAcM_viewCutoffCheck and fpoAcM_relativePos have no HD copy in this unit.
- Fix: fopAcM_createChildFromOffset no longer crashes on a missing parent.

### d_s_play (play scene dScnPly) — verified, 2026-10-04
- HD-only: a pause check that combines the pause timer with HD system overlays (probably the HOME menu / GamePad screens); a scene mode (0..3) chosen in phase_2 ("ENDumi", "sea_E", scene 8) that selects one of four HD systems, initialised in phase_4, updated in Execute and Draw and released in Delete; an "ocean heap" created on the "sea" stage and destroyed with the scene; on the ocean stages the shadow-cloud model of the HD scene system is set up; the play scene object (0xB68) carries the new HD scene system and wind input.
- Gameplay: the scene setup no longer takes the bow away on "GTower" (only the give-back half remains; probably handled elsewhere in HD). A change to "ENDING" is not requested by the play scene; a change to "sea_E" waits for an HD system; reaching "ENDumi" sets an HD flag.
- Structure: the particle scene comes from "Particle.bfres" by name ("Pscene%03d.jpc") instead of a loaded scene command; the Link demo archive name is written into a play buffer instead of being mounted; the window is 1280x720; only the darkness HIO child is created; the message HIO has no on-screen report and no message-number search; stage names are compared through sead::SafeString temporaries; phase_compleate is followed by an extra final phase; Delete waits for an HD system before tearing down and runs the scene's destructors itself.
- Removed: REL preloading (phase_6 does nothing; the preload table only lists resources).

### d_a_grid (ship sail: cloth simulation and sail draw packet) — verified, 2026-10-04
- Graphics: the GameCube sail packet drew with GX display lists over indexed position/normal arrays and
  configured the TEV stages by hand each frame. HD replaces this with a GX2 renderer: the packet owns a
  double-buffered pool of vertex buffers (172 vertices with position, normal and texture coordinate,
  rewritten every frame from the simulated cloth points), picks precompiled shader programs from a
  cached shader archive by draw pass, uploads its colour/light uniforms and draws front and back faces
  in two passes with separate shader variants.
- Graphics: two sail textures are prepared at construction and a global flag set from a play-state
  byte chooses between them at draw time; probably the HD Swift Sail look versus the normal sail.
- HD-only: in the stage named "Siren" the sail draw returns early once a byte of the object referenced
  from the play state (+0x326) has reached 7; probably hides the sail in a late story state there.
- Structure: the draw packet grows to 0x2100 bytes and the actor to 0x2BA0 bytes; the draw packet is
  constructed inline in create, and the destructors release the vertex pool and the shader cache
  explicitly. The fade-in/fade-out alpha logic and the cloth simulation call order match the GameCube
  version apart from layout (as far as compared).

### d_a_npc_uk (Jin, Jan and Jun-Roberto, the Killer Bees of Windfall) — verified, 2026-10-04
- Graphics: the three boys use separate body models chosen once at heap creation (one per shape type); the GameCube build loaded one body model and swapped in a per-boy material table every frame in the draw function.
- Graphics: no blob shadows (the GameCube draw registered two shadows, body and head); HD renders real shadows.
- Graphics: while running (two run animations) the animation play rate now follows the movement speed, clamped between 1.0 and 1.5; the GameCube build played them at a fixed rate.
- Gameplay: the body collision cylinder radius is 30 instead of 40 (height 80 unchanged), probably so the boys can be approached more closely.
- Structure: the talk flow drives the shared HD message manager instead of a message process; layout measured from scratch (GameCube source is only placeholders): HD texture-pattern animation (0x74), 8-byte pointer-to-member, HD virtual destructor; actor size 0x874.

### d_a_npc_mk (Ivan, Outset Island hide-and-seek boy) — verified, 2026-10-04
- Gameplay: the walk and run animations play at a rate that follows Ivan's movement speed (never slower than normal); the GameCube code plays them at a fixed rate.
- Gameplay: where the GameCube code sets a strong "stick to the ground" gravity (on spawn, after a jump has landed, and in the landing step of the event jumps), HD sets the same light gravity it uses during the jump itself; probably a gentler fall when he walks off a ledge, no visible difference while he stands on flat ground.
- Graphics: no blob shadow (HD draws real shadows).
- Structure: layout shift only for the members (shadow id removed, larger texture-pattern animation, 8-byte member-function pointer; size 0x87C, GameCube 0x708). Messages go through the HD message manager instead of a message-process lookup; the whistle sounds use the HD position-less sound call; the blink animation always uses the first texture-pattern table entry.

### d_attention (Z-targeting / lock-on dAttention_c and its helpers) — verified, 2026-10-04
- Graphics: the lock-on arrow's colour animation goes through the HD bpk animation helper instead of per-material colour-animation arrays, initialised with whichever arrow animation affects the most materials.
- Graphics: while the arrow plays its "in" or "out" animation, its joints are calculated at a fixed frame (the end of "in", the start of "out") and the animation frame is then put back rounded down to a whole frame; probably the arrow shape no longer wobbles during the transition.
- Graphics: when the last target is lost, the "out" animation is not restarted if the arrow is already playing its "delete" animation (probably avoids a flicker).
- HD-only: a fourth item-button query next to the X/Y/Z ones (probably the extra GamePad item slot).
- Fix: null checks before reading actors (the B-button action query, the enemy-distance profile check).
- Structure: the attention object grew by 8 bytes (an extra zeroed member in the parameter block); HD adds value-initialising default constructors; the attention heap is created with size 0 and given a name; the flag bit 0x08000000 is no longer kept from frame to frame.
- Structure: unit boundaries: the static initialiser right after this unit's last function and two string-class helpers (024EE58C, 024EE620, 024EE634) belong to d_attention, not d_bg_s; the initialiser at 024EAF50 belongs to a function-less unit before it (probably d_att_dist, which holds the distance table).

### d_event (event control dEvt_control_c, event info dEvt_info_c) — verified, 2026-10-04
- Gameplay: new event requests are refused while a play-state value (play+0x5BAC) is zero unless their priority is 1, and nothing can be queued behind a priority-1 request at the head of the queue; probably keeps forced events from being overridden. The priority-0 assertion is gone.
- Structure: event types are renumbered: an item-show type for a fourth item button is inserted after the Z-button type (talk button 4), shifting catch, treasure, photo and change up by one; the picture-box check still only accepts the X/Y/Z types.
- Structure: the event info block has a virtual table (0x18 bytes) and an HD value-initialising constructor; the event-name query returns the event data itself (its name is at its start).
- Structure: unit boundaries: the static initialiser and string-class helpers just before the event-info constructor (0253E89C, 0253E930, 0253E944) belong to d_ev_camera; d_event ends with its initialiser at 02540474.

### d_a_player_main#02 (Link: move/wait/slide animation blending, ship ride, battle jumps, sword cut inits, fan, bow/boomerang/hookshot/rope aiming, Wind Waker wait, item triggers, grab, do/talk status, wall type probe; 023E0E50..023EDF6F) — verified, 2026-10-04
- Gameplay: a fourth item button and a fifth "ship" item slot are handled by the item-trigger, ready-item and talk-to-actor checks (fourth attention actor/entry, select-item bytes at play+0x5BBE/0x5BBF); while an HD-only player word (0x8260) is set, the X/Y/Z item triggers and item talk are ignored. Probably the GamePad item buttons.
- Gameplay: the boomerang throw is driven by the button-release bits of the pad and a hold counter (12 frames) instead of simply testing that the item button is no longer held; the throw direction is remembered in an HD-only field (0x827E) that the catch proc uses to turn Link (probably GamePad aiming).
- Gameplay: Magic Armor (dragon shield) activation tests a different save value and no longer spends magic when switched on (probably moved elsewhere in HD); the boko/enemy-weapon throw prompt is written to a separate action-status byte (play+0x5BB6) and the do/attack prompts are only set while still blank.
- Gameplay: at a grabbable/climbable wall the do-status logic sets one status (0x11) instead of the GameCube grab R-prompt plus climb/sidle do-prompt, and the sidle prompt is suppressed in stage "Obombh"; the scope proc is refused while an HD no-reset flag is set, and the Picto Box aim also starts a standing animation.
- Gameplay: a strong (spin) jump cut sets an HD-only flag and switches the attack collider's special type off (HD-only helper 023ED2B0 sets all three sword capsules' special type); the slide-speed decay is skipped in first-person view; ship-ride sway angles are clamped to ±0x400; the Wind Waker wait timer starts at -30 when not on the ship.
- Gameplay: the Deku Leaf model shows its "veinsAMat"/"veinsBMat" materials only without magic (GameCube: hides "leafAMat"); fan swing has a re-entry guard; the bow mini-game fire button reads a different button bit.
- Fix: the background-polygon lookups of the front-wall-type probe and the wall-sidle polygon test tolerate a missing polygon.
- Structure: the debug tuning (HIO) values are folded into constants (some folded sums such as y-125-50 now round differently); a few heights read global tuning values instead of constants (rope ready height, the wall re-test heights, with the top probe lowered by 0.0085); sword blur positions come from the loaded animation archive instead of a JKR read; animation ratio packs became counted arrays; curse/flame state is a global; resource names and the Deku Leaf/stage-name comparisons use sead::SafeString; many float comparisons are restructured so NaN inputs take different branches than the GameCube source (no practical effect).
- Structure: 023E47C4 is daPy_mtxPosFollowEcallBack_c::makeEmitterColor, 023E9A6C/023E9A84 are checkDrinkBottleItem/checkOpenBottleItem (all unnamed by the matcher).

### d_a_player_main#05 (Link: side step, crouch and defend, slip and slides, rolls, back jump, landings, rope swing, auto jump and falls, damage/lava/ice/electric procs, boots, not-use, and the first demo procs: tool, talk, get item, dead and the small demo poses; 02419FEC..02423D0F) — verified, 2026-10-04
- Gameplay: while crouching behind the shield, the forward/backward lean follows the stick with its sign flipped unless a save option is set (probably the camera-inversion option), and with the stick held far to the side Link now also slowly turns his body toward it (HD-only turning).
- Gameplay: the Boko-stick jump attack from a side step tests a different trigger bit (0x01, the A button in GameCube numbering) than the sword (0x02, B); on GameCube both used the sword trigger. Probably a control remap.
- Gameplay: the moment the Iron Boots toggle during the equip animation is 14 frames plus a global tuning value (GameCube: fixed frame 11).
- Gameplay: the electric-damage proc sets a per-frame reset flag (0x80000000) every frame; door-opening demos with parameter bit 2 hold a no-reset flag (0x08000000) for three frames (both HD-only, probably to suppress a check while the effect plays).
- Gameplay: the cutscene "tool" proc (scripted Link in demos) has stage-specific HD fixes: on "Demo02" the demo translation is ignored while the demo frame counter is zero; on "Demo24" Link is moved by fixed offsets in two frame windows and one parameter type no longer sets the hold flag; on "Demo32" one texture pair is replaced and certain demo frames force animation changes; on "Demo23", "Demo44" and "Demo32" the frame sync is skipped for one animation each. Looping demo animations report one frame less as their end. Demo actor ids above 0x20 are rejected and a missing demo manager asserts.
- Gameplay: on the test stage "ITest62" the slide direction is turned by the frame counter (debug code, HD-only).
- Fix: the large-damage wall check tolerates a missing wall polygon; the fan glide init does nothing when Link is already gliding.
- Removed: the talk-partner facing has no special case for the Game Boy Advance (Tingle Tuner) partner.
- Structure: debug tuning (HIO) values are folded into constants (the large-damage wall reaction uses the same values for both damage kinds); upper-body blend ratios are set for every channel of the current animation instead of one channel; the message procs use the shared HD message manager (status getter/setter, no message process), and the end-message check records a plain flag instead of the message id; the game-over object is looked up by id and its back-alpha setter receives the remaining fade ratio; the get-item and look-wait procs read camera and partner data through accessor singletons; several float comparisons are restructured so NaN inputs take different branches than in the GameCube source (no practical effect); layout shift as measured in phase 1.

### d_a_player_main#08 (Link: Boko weapon and bait procs, all sword cut procs, spin attack, jump attack, boomerang return, tact accessors, item water splash, player position setters, __sinit and the per-TU inline copies; 0243F058..02444F20) — verified, 2026-10-04
- Gameplay: throwing a held Boko weapon also plays a sword-swing sound in addition to Link's voice.
- Gameplay: the spin-attack follow-up after a jump attack is decided while Link is still in the air (the turn amount, curse and the equipped sword/Boko stick are checked during the jump and latched in an HD-only flag), not when he lands; the landing proc only reads the latch. The spin proc clears that flag when it hands over to the next mode.
- Gameplay: the tact (Wind Waker conducting) accessors test the beat-input counter for "not negative" instead of "not -1", and the tact timer cancel no longer looks at a held Korok; it reports one of two states depending on whether the timer is above or below -100 (probably a hold-to-cancel distinction).
- Fix: the spin-attack wall check tolerates a missing wall polygon (no plane lookup crash before the crash-roll proc).
- Structure: the debug tuning (HIO) values are folded into constants throughout; the front-cut's two material animations (bpk/btk) became HD animation objects whose frame is set through an evaluator callback; joint-matrix reads mark the model's matrices dirty; the sword-swing blur is a separately allocated object; the equipped-sword checks read an HD play-state byte (also used for the sword mini-game test).
- Structure: setItemWaterEffect is a static function (no `this`); the debug copies of the player position and angles are still written by the three position setters; the matcher's "daPy_lk_c::draw" at 02443EC4 is __sinit_d_a_player_main_cpp; the range also holds an unnamed tact accessor (024431E0, "tact play proc with its fifth proc variable at zero", probably HD-only) and the hookshot return / carry-offset checks with HD field offsets.
- Structure: layout shift of daPy_lk_c as measured in phase 1; one more HD-only byte at 0x69E8 (the jump-attack spin latch).

### d_a_player_main#03 (Link: proc inits for hammer/guard/rolls/food/bottle/ship/damage/death/swim, item-button and action checks, demo data, stick and button input, damage dispatch, ground/foot/slope handling; 023EDF70..02400B13) — verified, 2026-10-04
- HD-only: five functions with no GameCube counterpart: two proc inits that start the HD message-bottle throw (proc 0xDC) and letter-writing (proc 0xDB) sequences (probably the Tingle Bottle), a rupee drain applied when Link is hit while the Magic Armor is active, a damage wait-point setter, and an extra animation-weight helper used by the HD bottle/letter animations.
- Gameplay: a fourth item button exists (HD GamePad item slot): its trigger/hold bits are read from the GamePad controller object, it is assigned items through two extra save-file select slots, and it also covers sailing items (items 0x31/0x25 while on the boat, item 0x22 otherwise; probably the Tingle Bottle and the HD Swift Sail family). Pressing A while sailing with the sail item (0x78 or 0x77) also refreshes one of these slots.
- Gameplay: Hero Mode doubles the damage Link takes; with the Magic Armor active, damage costs rupees instead of hearts; the ship takes damage only in one mini-game mode; the Boko stick can be swung with the action button.
- Gameplay: the bow mini-game fires on a different held button than on GameCube; the B button also works for the boat's item use regardless of the button-action mode.
- Gameplay: two events (the Triforce chart check and the Nintendo Gallery photo event) get special demo handling: one cut is skipped, one cut ignores the event's stick value, and one gallery cut places Link at a fixed position when he is beyond a depth threshold. A byte set when ordering the Tingle-related HD event starts an "original" type demo with its own mode.
- Gameplay: room-restart, fall-exit and wind-push checks gain extra HD conditions (a room actor id restart case, an exit block flag in the play object, wind push cleared only when Link is heavy); the push/pull check moved under the action-button test and the climb check was removed from the button action chain; item-change from buttons and demo end set a short HD item-button block timer.
- Structure: debug tuning (HIO) values are folded into constants; float comparisons are restructured so that NaN inputs take different branches than in the GameCube source (no practical effect); the Link texture swap for the casual clothes now copies a list of ten texture objects instead of one texture header; demo-object lookups assert when the demo manager is missing; event and stage names are compared through string objects.
- Structure: daPy_lk_c layout shift as measured in phase 1; HD-only tail fields at 0x8260 (item-button block timer) and 0x8264 (event byte) are used here.

### d_a_player_main#01 (Link: model and heap creation, joint callbacks, draw, item models, texture/face animations, attention list, animation setters; 023D4BB8..023E0E4F) — verified, 2026-10-04
- Graphics: the eye, mouth and eyebrow animations run on HD animation objects that are attached to Link's model data, instead of per-material texture-pattern and texture-matrix animators; the affected materials are found by name (for example "mouth") rather than by fixed material numbers. Hat, face, hair, sleeve and leg materials are also selected by name when the draw code hides or shows parts of Link.
- Graphics: draw is reorganised. Every Link model gets HD draw-flag updates, the hands, hat, boots, gloves, sword, scabbard, shield and held item are entered into an HD packet registry, and in the camera-attention (first-person) view only the sleeves and legs stay visible, with colour updates switched off for the other materials. A "Demo50" demo keeps the back scabbard visible, and the firefly bottle contents ("binho") are drawn after the bottle cap.
- Graphics: the shadow is a 3D shadow model (Agb archive) placed on the highest of four ground probes around Link and faded with his height above the ground. HD only draws it while Link glides with the Deku Leaf (probably the real-time shadows cover the other cases).
- Graphics: when Link is in a "do not draw" state, HD still draws his body and equipment without effects in some player states (GameCube draws only the mirror shield light).
- Gameplay: looping animations end one frame earlier (the frame controller's end is set one frame before the animation's end).
- Gameplay: selecting the Deku Leaf no longer creates its fan model when the item type is made (probably created elsewhere in HD).
- Gameplay: the get-item message flow goes through the HD message manager and the play-wide message status; the stored message id becomes 0 instead of the created message's id.
- Fix: initSeAnime only touches the sound-animation frame controller inside its NULL check (GameCube reads it after the check unconditionally).
- Structure: animations, texture animations and the sword/bottle/Wind Waker brk/btk data come from the resident "LkAnm"/"Link" archives instead of being read into heap buffers; the item brk/btk animations are embedded objects that are re-initialised instead of pointers set to NULL.
- Structure: the animation blend packs keep one ratio per joint (42 joints), set on joint subtrees; the per-joint before/after callbacks save one quaternion and transform per joint instead of a single one, and the ship-ride arm angles moved from the before- to the after-callback (using the arm joint's matrix); for guard, grab and throw-ready animations the root rotation is replaced by the identity instead of saving and restoring the root matrix.
- Structure: debug tuning (HIO) values are constants (for example the dash-damage wait timer, the grab release blend, the low-health face threshold, gravity -2.5 and the gliding fall speed -175).
- HD-only: in the stage "GanonK" the heap creation sets up eleven extra HD objects and enables the packet registry (probably for the final battle); a 30-frame timer in the HD tail of the actor is started by the common proc init in two player states; joint callback 023D689C copies Link's joint matrices to a second model.
- Structure: layout shift of daPy_lk_c as measured in phase 1; offsets measured here include the HD per-joint flag array (0x68E2), saved quaternions (0x6AB0) and transform infos (0x6D50).

### d_a_npc_btsw (Baito, Dragon Roost postman and the mail-sorting game) — verified, 2026-10-04
- Gameplay: the sorting game's fixed camera is also switched on while the game-start message (0x1A97) is being advanced, not only for the two explanation messages as on GameCube.
- Gameplay: the letter cursor no longer reads the stick through the actor's STControl trigger logic; it reads two pad trigger words directly (left/right/up/down bit groups); when another pad state word has any of four direction bits set, only those four bits are used (probably so D-pad and stick input do not mix). At the start of the game a pad setting is applied (probably key repeat) and it is reset when the action ends (HD-only calls). The STControl member is still constructed and parameterised every frame, but no longer consulted.
- HD-only: starting and ending the minigame additionally set/clear a global byte flag and call a function on an object reached through a global (probably HD minigame HUD or controller state).
- Structure: the message-state function reads the yes/no selection from the HD message manager instead of the message process; the talk flow otherwise matches.
- Structure: the head texture pattern always uses the first table entry (the GameCube indexed it by a member that is always 0); the throw step computes the vertical aim angle once before branching; no create/delete ID registration.
- HD-only: the draw function initialises two function-local static colour words (probably the GameCube's unused colour table) behind an HIO debug flag.
- Structure: actor 0xD70 bytes (GameCube 0x9D0): fopNpc base +0x118; the letter model's light/TEV structure is larger in HD (to 0xA14); the texture-pattern helpers grow by 0x60, so each of the three letters (SwMail2_c) is 0xBC bytes (GameCube 0x60, its proc pointer to member 8 instead of 12 bytes); the action pointer to member is 8 bytes. The constructor is out of line (allocates when called with NULL), the class has an HD virtual destructor, and the HIO object (0x60) keeps its vtable at the end.

### f_op_msg_mng (message process manager helpers fopMsgM) — verified, 2026-10-04
- Structure: HD keeps only a small part of the GameCube unit (025DB2BC..025DBBDC: stage layer, create, the timer create, demo flags, pane helpers, value easing, heap helpers, the picture drawing helpers, its static initialiser and per-unit inline copies). The text/message window code of the GameCube file is gone, probably replaced by HD's own message system; 025F795C (matcher "fopMsgM_SearchByID") is part of that HD message manager, not of this unit.
- Removed: the pane helpers no longer move, resize or fade the J2D panes: the centre/size bookkeeping is kept but the set-alpha helper is empty and the translate helper ignores its offsets (probably the HD 2D layout system does the drawing).
- Structure: every pane, heap and picture helper tests its pointer first; the picture drawing helpers return early without a matrix; the create functions have their parameter builders inlined; the message heap gets a debug name.
- HD-only: a small byte-table getter (025DB5AC) and a pane lookup helper (025DB5C0).

### d_com_inf_game (game info and the dComIfG/dComIfGp/dComIfGs helpers) — verified, 2026-10-04
- Structure: the game info object is a function-local static built on first use by its accessor (025200D4) instead of a global; the unit spans 0251FD14..02525C94 (play object init/constructor first, static initialiser 02525A48 and per-unit inline copies last). The matcher's "__sinit_d_com_inf_game_cpp" (0251FD94) is the play object's constructor.
- Structure: resource loading goes through named string objects and the HD resource control; stage resources are loaded, synchronised and deleted through new helpers that also pull in extra archives for some stages (M_NewD2, Siren, sea_T, the sea and ending sea stages).
- Structure: the stage-resource path rule of the GameCube d_resorce ("ma2room" becomes "ma3room" once a story flag is set) is applied by these new helpers.
- Structure: item checks read the inventory slots directly (bag slots at their HD positions); the item that maps to a sea chart uses a different base; separate out-of-line map/compass/big-key checks per stage were added.
- Structure: the recollection (boss rematch) data is checked against the telescope slot and copied with the HD item layout; the restored maximum health is stored from its low byte only (as the original HD code does).
- HD-only: a helper building the current view's projection matrix from the HD display/camera objects (02524628).

### d_a_player_main range #07 (Link: grab procs, swimming, battle jumps/rolls, ship item procs and ship exit, rope hang/swing/climb, boomerang/bow/hookshot aiming, cut reverse, fan, Wind Waker tact procs, vomit, hammer, push/pull, bottle procs, enemy-weapon swings; 0242DAC8..0243F057) — verified, 2026-10-04
- Gameplay: rope swinging gained sideways steering: while swinging, holding the stick left or right turns Link around the rope (with a ramp-up and a decay when released). An HD-only helper (0243460C) first simulates the turned swing and casts rays along Link's body from the rope to the foot; the turn is refused when the side it turns toward would hit a wall, and it is also skipped when the roof check near the rope says there is no room.
- Gameplay: on the stage "ADMumi" the rope cannot be let go and the swing ignores stick input; on "Asoko" Link leaves a swing with less speed until event bit 0x520 is set (probably a tutorial or a story-gated area).
- Gameplay: on the ship, getting off, putting the item away and cancelling use the B-button status in the play state; the sail toggle is driven through an HD play-state byte; the ship scope proc has an HD selfie/picto mode whose facial expression is picked from the pad buttons.
- Gameplay: aiming procs (bow, boomerang, hookshot, rope) boost the normal speed by 1.2 (HD-only); push/pull uses the do button and do status instead of the GameCube's input path.
- Gameplay: Wind Waker conducting: known songs skip the play-back proc, the song-usable check is factored into an HD helper (0243AAD4), the beat countdown changed, and GamePad input is accepted (HD-only cancel trigger 02439D24).
- Gameplay: the Tingle Tuner talk event is inlined with an HD flag; bottle drinking accumulates life refill as a float, the Omori bottle timer is 54000.
- Fix: several ground/wall plane lookups tolerate a missing polygon (no crash when the plane getter returns nothing).
- Structure: the debug tuning (HIO) values are folded into constants; per-joint blend ratio tables replace the GameCube's single setRatio calls; joint matrix reads mark the model matrices dirty; stage-name tests go through HD SafeString objects.
- Structure: HD-only functions in the range: 02431028 (ship put-away trigger check), 02439D24 (tact cancel trigger), 0243AAD4 (tact song usable), 0243460C (rope swing turn check). 02439C9C is checkNpcStatus (unnamed in the map).
- Structure: layout shift of daPy_lk_c as measured in phase 1; HD-only fields used here include a swing turn speed at 0x6922.

### d_save (save data: dSv_player_*, dSv_memBit_c, dSv_event_c, dSv_zone_c, dSv_info_c) — verified, 2026-10-04
- Gameplay: a new game starts with the time of day at 150 instead of 165, probably a slightly earlier hour.
- Gameplay: the Triforce chart flags of a new game start with five charts (1, 3, 5, 6, 7) already set; probably part of the shortened HD Triforce quest.
- Gameplay: five item buttons are stored and refreshed instead of three; the bottle and bag-item helpers that act on the talk button still map only X/Y/Z.
- Gameplay: the sound mode is always set to 2 (new game and after loading a save), without the console sound-mode query; the vibration check reads a play-object setting without the rumble-support test.
- Gameplay: restarting the save (reinit) keeps the sound mode, vibration, the clear count, the picture count, one event register and two values of the HD-only save area; it no longer keeps the player name, attention type or ruby setting.
- Fix: creating the initial card data copies the one initialised stage memory to all 16 stage slots (GameCube copied uninitialised stack contents for 15 of them).
- Removed: no IPL date is written when saving; no Tingle-Tuner query for the arrive grid (only its setter remains); the map Triforce flags have no clear function; the sound-mode branch on load is gone.
- HD-only: the four "player status C" records are no longer empty: each holds a 0x70-byte copy of status, item, bag and collection data (purpose unknown, probably a saved inventory snapshot); five extra info fields (0x1290..0x129C) are cleared by init; a separate HD save area is reached through accessors at info + 0x12A0.
- Structure: unit 025B4DC0..025BAF8F (d_salvage before, d_save_init after with only its static initialiser; setInitEventBit is inlined into dSv_event_c::init). Layout: save base = *(101F84DC), info at +0x20, event flags at +0x644; 16 stage memories of 0x24, 32 zones of 0x4C; memory item limit 32 (GameCube 64); reserve 0x50 bytes; card slot 0xA94 bytes. The room zone lookup is inlined (static room table). Many small accessors the matcher did not name were identified (status/return-place init, map on/off functions, memBit init, zone constructor, bait/reserve button variants).

### d_a_daiocta (Big Octo, the body of the sea mini-boss) — verified, 2026-10-04
- Gameplay: the swallow-and-spit-out cutscene has a fallback timer: the spit-out cut arms it with 60 frames, the suction event is ended once it runs down, even if the event manager has not reported the end; probably a guard against the boat being stuck in the cutscene.
- Gameplay: when Big Octo is defeated and its switch is set, HD additionally plays the "riddle solved" jingle, but only if the switch was not already on and it is not the fairy-giving variant; the GameCube version plays no sound there.
- Fix: many lookups that the GameCube code trusted are now guarded (the ship, the whirlpool actor, the current cut name, mode numbers and joint numbers out of range); a missing ship or cut name raises an assertion and that part of the step is skipped instead of reading invalid memory.
- Removed: the debug collision display is gone; only an empty distance loop and the first-use set-up of its colours remain.
- Structure: actor size 0x5910 (GameCube 0x31C8): base actor +0x11C, one HD-only word (the cutscene timer) after the mode fields shifts everything behind it, larger HD animation controllers widen the bubble arrays; the stage change after the cutscene passes two extra HD arguments.

### d_ev_camera (event camera engines dCamera_c::*EvCamera and the event parameter helpers) — verified, 2026-10-04
- Structure: unit 025303B4..0253E948 (42 functions): the event parameter helpers (with a debug table of up to 8 named parameters inside the camera, used when the event camera is started with explicit parameters), getEvActor, all event camera engines, Start/EndEventCamera, the static initialiser 0253E89C and two per-unit inline copies (0253E930, 0253E944: the deleting destructor and an empty virtual of an inline string-holder class used for stage-name compares). The GameCube file only has "Nonmatching" stubs, so every function was written from the HD code and behaviour can only be compared where HD-specific data is involved.
- HD-only: uniformTransEvCamera has a branch for the "TACT_WINDOW" event (Wind Waker conducting window): while it runs, the eye is set 800 units around the player at a fixed height, following the player's facing and pulled in front of walls by a line check; probably for the HD conducting screen.
- HD-only: uniformTransEvCamera shifts its eye and centre sideways by 80 units for one event (staff 0x104) on the stage "Asoko" when the eye lies beyond a depth of -1000; probably a framing fix for that cutscene.
- Structure: the braking and accelerating moves (uniformBrake/uniformAccele) share the move code of uniformTrans with their own work layout; the step sums are computed once at the start.
- Structure: event cameras read HD play-state fields (the pad status at play + 0x5CD8, the player at play + 0x5B34) and the HD environment light (wind direction at env + 0xA24/0xA26, tact-wind flag at env + 0xA2C).

### d_a_bpw (Jalhalla, big Poe boss) — verified, 2026-10-04
- Graphics: the ground shadows of the boss body and of its lantern are no longer requested by the actor (probably replaced by the HD real-time shadow system); the lantern glow sphere and the fireball spheres are no longer submitted as alpha models (only their matrices are still computed, probably drawn by the HD effects instead); the per-material alpha goes through the HD TEV constant-colour block.
- Fix: the body is kept in front of a z limit after the background check (stage value + 1800), probably so it cannot leave the arena; the extra "Ganondorf" snapshot-figure registration of the GameCube draw code is gone (only the Jalhalla figure is registered, and only while visible and not carried).
- Structure: layout is the GameCube one shifted by the HD actor base (+0x11C), size 0xE9C; the shadow id field is gone and the light influence block starts there (4 bytes larger in HD). Inlining differs: body_execute (020DC704) contains the move, attack and body-press actions; Execute contains the lantern, fireball and "torituki" logic plus the vibration check; Draw contains all four draw paths. Effect callbacks use end() instead of remove(); actor-by-id lookups skip invalid ids; player demo-mode changes are inline stores.
- Gameplay: no behaviour change was found beyond the above; the separation phase thresholds (10/5/0 remaining health) come from a small table, and the camera moves in the separation/ending demo use the same targets as the original code.

### d_a_gy (Gyorg) — verified, 2026-10-04
- Structure: layout shift (every member +0x11C, size 0xFAC); the GameCube decompilation has only stubs for this unit, so the comparison was made against the GameCube assembly. The mode dispatcher reads a constant table of member-function pointers instead of copying a function-local table at its first call.
- Structure: the animation helper (dLib_setAnm) receives the second, water-surface shadow model's animation as an additional argument; probably so both models play the same animation.
- Removed: the debug drawing routine no longer computes anything; only the first-use initialisation of its colour constants remains.
- HD-only: compiler-generated constructor, deleting destructor, string-class helpers and the static initializer are separate functions in this unit.

### d_a_majuu_flag (Forsaken Fortress / pirate cloth flag) — verified, 2026-10-04
- Graphics: the GameCube packet drew the 21-vertex cloth with GX display lists each frame; HD builds a GX2 packet once at creation (a "flag_default" shader program, two materials with per-side attribute objects, double-buffered vertex/uniform buffers for front and back faces, an index buffer from a fixed 35-vertex triangle list). Every draw now also uploads the current positions and normals into the active buffer, flips the buffer, and fills the lighting colours (from the actor's or the player's light block) into shader uniforms.
- Graphics: the default flag texture is no longer an image embedded in the actor; HD loads "flag02" from the shared ProgramTexture.bfres archive. The toon ramp still comes from the "Cloth" archive and the special flags still from Matif/Vsvfg/Xhcf; HD only rebuilds a texture object when its image description changed.
- Structure: the actor base is +0x11C larger; the packet grows from 0x79C to 0x1A30 bytes (GX2 objects, matrices and the 21-vertex position/normal/back-normal/velocity arrays at its end), so the actor is 0x1E3C bytes instead of 0xAA0. The cloth move routine (get_cloth_anim_factor inlined into majuu_flag_move) and the texture setup moved out of Create into separate functions; the packet has its own out-of-line constructor and destructor that release the GX2 buffers.
- Gameplay: no behaviour change found: wind-following yaw, cloth simulation, scale per flag type, the 20 warm-up cloth steps for the Xhcf flag and the monochrome-mode skip of Execute are as on the GameCube. Probably fixed: an unknown texture type above 3 no longer reads an uninitialised load phase.

### d_a_gnd (Ganondorf, final boss) — verified, 2026-10-04
- Gameplay: after a hit on the body, HD shortens Ganondorf's hit-invulnerability timer (2 frames instead of 7) when the collision reports attack type 1 and Link's HD-only jump-attack spin latch (the player byte at 0x69E8) is set; the GameCube code has no such check. Probably lets the HD spin follow-up of a jump attack land a second hit.
- Structure: class 0x1950 (GameCube 0x15E4): the larger actor base plus the HD sizes of the animation objects, the model morph and the hair line material. The GameCube decompilation of this unit is all stubs, so the comparison was made against the GameCube assembly; GameCube's separate helpers (demo setup, ripple, attack effect set/move, hair control and root placement, the move0/attack0/attack1/attack2/attackPZ modes, the demo camera) are inlined in HD into Execute, ke_move and gnd_move.
- HD-only: the dodge handler (defence0) prints its two timers through the debug report routine every frame in its guard state (probably a no-op in retail); on the GameCube only the demo camera calls it.
- HD-only: compiler-generated deleting destructor, the string-class helpers and the static initializer are separate functions in this unit.

### d_a_mozo (Moblin statue, beam / fire trap) — verified, 2026-10-04
- Structure: actor base shift of +0x11C (GameCube fields from 0x290 start at 0x3AC); class size 0x6CC. The GameCube decompilation of this unit is mostly Nonmatching stubs, so most functions were written from the WWHD code and only compared with the GameCube structure, not instruction by instruction.
- Structure: the joint callback, beam/fire node callbacks and helper calls are inlined in HD into one joint callback function; the matrix copies to the joint are done as plain word copies.
- HD-only: compiler-generated deleting destructor, string-class helpers and the static initializer are separate functions in this unit.
- Gameplay: no behaviour change found (wait / beam search / fire search / return-to-wait states, range check against the HIO limits, beam child actors and event start as in the GameCube structure).

### d_a_obj_Yboil (boiling sea object, re-verification) — verified, 2026-10-04

- Structure: re-checked against the GameCube source; the entry above stands (HD writes normalized float colour buffers after the sea ambient/diffuse tev colours, and the actor grows because the animation wrappers grow). Creation, deletion, the respawn timer logic and the 50-model layout are otherwise unchanged.

### d_a_wall (bombable walls) — verified, 2026-10-04
- Fix: the wall type taken from the actor parameter is clamped to the last type (2) when it is 3 or more, so the archive, heap-size and model tables are no longer indexed past their ends (GameCube uses the raw byte).
- Structure: class 0x700 (state, switch and type fields at the end, 0x6C8..0x6FC); the sound effect start inlines the room reverb lookup; the HD model-create helper ignores the GameCube "different flag" argument (probably handled by the HD renderer).
- HD-only: an empty per-file SafeString termination hook (024D4954) shared by all HD translation units.

### d_a_player_main range #04 (Link: attention/look targets, hat and neck angles, item and boots models, sword/shield placement, collision setup, foot/water/aura/light effects, water ripples, execute, constructor/destructor, playerInit, makeBgWait, sight and sword-blur rendering, emitter callbacks, first move/wait/ladder/hang procs, subjective view; 02400B14..02419FEB) — verified, 2026-10-04
- HD-only: a selfie pose proc (02415BA4, probably the Picto Box self-portrait) with its end check (02415AF8): Link turns to the camera and the stick picks one of eleven face expressions.
- HD-only: two entry points other actors call to register themselves in a mirror reflection list held by Link (02415A80, 02415ABC).
- HD-only: the first person (subjective) view gained its own movement and animation helpers (02416AB4, 02416B70, 02416FA8); in that view Link can walk, a slide floor ends the view, and crouching with the stick held starts crawling.
- Graphics: the sword blur and the bow/hookshot sight are rebuilt as GX2 render objects with their own vertex buffers, shapes and named materials ("blur_default", "default_sight") instead of GameCube display lists; the blur colour comes from a small per-blur-type table.
- Graphics: eye texture scrolling writes one named texture matrix of the material instead of looping over eight; the eye and eyebrow shapes are sorted by material name instead of by Z mode.
- Graphics: the hands model's finger joints are hidden at init with zeroed matrices; effect colours used by the cut and roll trails are gamma corrected once (a new colour-to-linear helper, 02406D60); water splashes follow the sea colour and are suppressed in some camera modes.
- Graphics: Link's clothes texture is swapped by name at init: every "linktexS3TC" texture gets the casual-clothes image, the originals are kept so the hero's clothes can be restored.
- Gameplay: makeBgWait no longer looks for a treasure chest; it checks the ground under Link instead and, on a moving platform of one actor type, lowers Link onto it.
- Gameplay: the wall-catch delay doubles (14 instead of 7 frames) while one status flag is set; in two rooms (MajyuE room 0, a sea room) walking at one specific height is faster; the hammer swing has a decaying wobble; the magic shield is no longer drained in this range, it just ends when magic runs out.
- Gameplay: execute adds a heavy-state counter that gates the heavy flag, an event start from a play flag, warp/pitfall events that shrink Link's models, and "no control" now means Link is not the play's current player; the forest water expiry only swaps the bottle and sets an event register.
- Structure: daPy_lk_c grows to 0x8284 bytes; HIO tuning values are folded into constants throughout; stage-name checks use sead SafeString comparisons; J3D animation objects carry names and frame callbacks; the player constructor allocates the sword blur in a named heap scope and the destructor frees it through a virtual destructor; playerInit no longer reserves item heap dummies or loads the blur texture.
- Structure: several emitter callbacks mirror the emitter's Y for newer particle resource versions; a few NULL checks were added (slide polygon).

### d_a_agbsw0 (Tingle Tuner / GBA trigger area) — verified, 2026-10-04
- Removed: all Tingle Tuner (GBA link) behaviour: the fifteen trigger types, mail sending, Tingle bombs and map icons are gone; Draw and Delete do nothing.
- Gameplay: the actor survives only as a bomb switch (the GameCube type "T" logic): any bomb-type hit on its cylinder sets the switch, and the actor deletes itself 30 frames later, waiting while an event runs. On the GameCube the switch needed a linked GBA and a Tingle bomb; probably HD keeps these placements so puzzles that relied on Tingle bombs stay solvable with Link's own bombs.
- Structure: the class shrinks to timer, collision status and cylinder (size 0x51C); create always uses the 200 scale factor and rejects only an unset or already-set switch.

### d_a_stone — verified, 2026-10-04


**Structure:** The HD Stone actor occupies 1,964 bytes versus 1,680 bytes in the supplied GameCube layout. Its fields after the actor base move by 284 bytes. The HD unit has 51 functions, including wrappers, initialization, destructors and parameter helpers beyond the nominal match list; the following Stone2 constructor belongs to a separate actor.

**Structure:** HD passes archive names through SafeString objects and checks bounds when accessing the five-entry stone data table. The supplied GameCube source uses its resource wrapper with direct archive names and indexes the table directly.

**Gameplay:** The five stone data records retain the supplied non-demo GameCube values for physics, rotation, vibration, sound, resources, heap sizes and culling. This comparison concerns the table values; surrounding engine and rendering behavior was not established by that comparison.

### d_a_bdk (Helmaroc King, boss battle) — verified, 2026-10-04
- Gameplay: on the ground the boss turns towards Link by short hops (hop height grows with the angle, from more than about 22 degrees instead of 67); a wall hit while walking, and a new random "recover" state after a damage, make it take off; when Link stands near a fixed spot of the tower top or close to a wall, the beak attack is replaced by the wind attack.
- Gameplay: the landing approach moves at the current forward speed straight to the target and starts in the same frame the boss gets close; the drop keeps (and damps) the horizontal speed instead of easing to the point.
- Gameplay: damage: no pause timers on hits; a spin attack on the crest cools down for 3 frames (other weapons 6, hammer-type 12); a body hit shortly after touching the ground switches to the wait action in a dedicated state (10); a debug register can keep the boss at 4 health; when Link hits in one particular player state (process value 0x12) two of his fields are set (probably a hammer recoil).
- Gameplay: the mask fragments bounce lower-damped and, when struck, pick random yaw and roll targets they turn to; a line hit only stops them; the separate fragment spin of the GameCube is gone.
- Gameplay: start: the bk/boko clean-up callbacks are not run (t_down removes them instead), the fight start also waits for a play flag (+0x5BAC); t_down shakes the boss for 10 frames before it falls, drifts it to the centre with growing speed and pushes the tower's falling fragments away.
- Gameplay: cutscene cameras: the intro starts from the current camera, holds Link at a different spot and also moves an object found by a new search callback with him; the shake moves eye and target in opposite directions (faster in the start cutscene), the wall line check of the camera is gone; the death cutscene circles around the boss's position; the field of view eases in one state.
- Graphics: no real-shadow model; the mask crack stages switch joint meshes instead of a visibility animation; the foot spark emitters mirror Y for newer particle resources; feather ground height +10 capped at 9820 (GameCube +20).
- Removed: the t_landing action; the neck yaw (mF14); the GameCube REG-driven radii of the body sphere (fixed 250, 400 in the last attack).
- Structure: move, my_effect_move and daBdk_Execute absorb all their GameCube helpers (the matcher calls two of them fly and eff_hane_move); debug text reports remain (action/state, demo counters); bdk_class is 0x65E0 bytes (fopEn_enemy_c +0x11C, no shadow fields, tevstr grows by 0x118).

### c_counter — verified, 2026-10-04

- Structure: HD retains the GameCube reset-versus-increment behavior and advances the frame counter every call, using relocated global storage. Both counter words wrap as PowerPC words; no material gameplay difference was found. The full HD unit includes its adjacent compiler-generated initializer for local math storage and runtime registration; per-unit attribution follows its placement and the repeated initialization pattern.

### c_list — verified, 2026-10-04

- Structure: HD retains the GameCube three-word head, tail and size layout and the six list operations. HD duplicates the append path within one function without changing the list operation. The full unit also includes its adjacent compiler-generated initializer for local math storage and runtime registration.

### c_phase — verified, 2026-10-04

- Structure: Seven HD phase functions retain the GameCube handler-table and phase-index layout and request-state behavior. Uncompletion tail-calls reset, and the handler entry tail-calls dispatch. The full unit also includes its adjacent compiler-generated initializer; its per-unit attribution is inferred from placement and the repeated initialization pattern. No gameplay change was found in the phase functions.

### c_request — verified, 2026-10-04

- Structure: Request state retains the GameCube one-byte pending/completion flags and six-bit command payload. HD consolidates Done/Create into a single byte store and Command into a tail branch. The full unit also includes its adjacent compiler-generated initializer. No material gameplay difference was found.

### c_node — verified, 2026-10-04

- Structure: HD retains the GameCube three-word previous, payload and next node layout and list operations. Join is inlined into addition; HD addition checks null inputs through an assertion and rechecks them before writing links. The full unit includes its adjacent compiler-generated initializer for local math storage and runtime registration. No material gameplay change was found in the ordinary node operations.

### c_tree — verified, 2026-10-04

- Structure: HD retains the GameCube list-array pointer and signed list count. Removal delegates to the list helper; addition and insertion keep the upper-bound-only index check and use twelve-byte list entries. Creation initializes each list. The full unit includes its adjacent compiler-generated initializer, attributed by placement and the repeated initialization pattern. No gameplay change was found.

### c_tag — verified, 2026-10-04

- Structure: HD retains the supplied GameCube tag layout and eight operations for attaching tags to lists or trees, extracting them and clearing their in-use state. The full unit includes its trailing compiler-generated initializer and runtime registration. No gameplay or graphics change was found in the tag operations; the initialized storage’s exact header origin remains unspecified.

### c_data_tbl — verified, 2026-10-04

- Structure: Data-table name counts, pointer lists and row/column lookup retain GameCube behavior. HD constructors allocate when passed null and store virtual tables after data fields. GetIndex compares strings inline, and composite GetInf delegates by tail branch. The full unit includes its trailing compiler-generated initializer. No material lookup behavior change was found.

### c_tag_iter — verified, 2026-10-04

- Structure: HD retains the GameCube callback adapters: each loads tag payload and filter user data, then tail-calls the selected callback. The full unit includes its adjacent compiler-generated initializer. No material behavior change was found.

### c_xyz — vectors (2026-10-04)

- Structure: the vector remains three single-precision components in 12 bytes. HD supplies out-of-line arithmetic, comparison and normalization methods, a componentwise product, and an initializer for shared direction constants.
- Structure: result-producing methods accept separate destination storage; the compiled HD routines allocate 12 bytes when that destination is null. The normalization variants distinguish a zero fallback, a positive-Z fallback, an in-place update and a success result. No gameplay or graphics change was established from this unit alone.

### c_list_iter — verified, 2026-10-04

- Structure: List iterators retain the GameCube signed-size guard and default success or null result on empty lists. HD wrappers tail-delegate the list head, callback and user data to node iterators. The full unit includes its adjacent compiler-generated initializer. No material gameplay difference was found.

## c_math

Structure: HD retains table-based angle conversion and the three-state random recurrence. The contiguous HD unit includes the seed initializer and a generated TU initializer; GameCube’s second random-state initializer was not found in this range, probably removed as unused. No gameplay change identified.

### c_node_iter — verified, 2026-10-04

- Structure: HD retains GameCube method and judge traversal, caching the next-node pointer before calling the user callback. Method accumulates callback failure; judge stops on the first non-null result. The full unit includes its adjacent compiler-generated initializer. No material behavior change was found in these operations.

### c_tree_iter — verified, 2026-10-04

- Structure: HD retains the GameCube list-array traversal. Method accumulates failures from the list callback helper; judge stops at the first non-null result. Both preserve the callback and user-data arguments while advancing through twelve-byte list entries. The full unit includes its adjacent compiler-generated initializer. No material behavior change was found.

### c_bg_s_chk — verified, 2026-10-04

- Structure: HD base collision checks retain GameCube pass-check pointers, actor-ID equality guard and enabled same-actor flag. The HD constructor explicitly initializes the actor ID to zero; the supplied GameCube inline constructor leaves it unassigned. The virtual table sits at offset 0x10 in the twenty-byte base. The full unit includes its adjacent compiler-generated initializer. No demonstrated gameplay fix was found.

### c_angle — angles and spherical coordinates

The HD unit includes 56 entries: 54 angle/degree/polar/globe methods and operators, its static initializer at 02007410, and the cAngle::Adjust<float> instantiation at 020074FC. The latter sits immediately after the initializer and is directly called by cDegree::Formal; it belongs to this handoff rather than the separately claimed c_math range.

HD constructors allocate 2-byte cSAngle, 4-byte cDegree, or 8-byte cSPolar/cSGlobe objects when the supplied object address is null, and skip initialization if allocation fails. Default HD constructors initialize the angle/radius members. Returned class values use explicit output addresses, with cSPolar::Xyz allocating a 12-byte vector if its output address is null. The HD vector-to-polar conversion uses single rounded squares, double sums, conditional sqrtf calls, and cM_atan2s directly, rather than the GameCube atan2f followed by radians conversion. Radian conversion reads initialized HD globals rather than embedding pi. The initializer constructs the five static angle values and initializes local SDK registration data.

Forwarded scalar arguments preserve the full incoming integer register when HD forwards it unchanged. Angle arithmetic truncates to signed 16 bits where HD explicitly sign extends. cSGlobe::Val(copy) preserves the reference's raw float-copy bits, including a signaling NaN, before sequential angle stores; this was checked with a generated input that previously exposed unwanted quieting. Stack objects passed to callees reserve their complete 2- or 8-byte object size through the existing Local allocator.

Adjust is verified for finite values from -1440 through +1440 and finite lower/upper endpoints in [-180,-90] and [90,180], including endpoint cases. A zero/reversed period or infinite value can make the reference loop fail to terminate and is excluded from generated tests. This is a mathematical helper input qualification, not evidence for those excluded values. All other entries use the stock unrestricted generated fixtures and mocked external callees; no gameplay or native integration claim is made.

### c_m3d_g_sph

Structure: HD retains the sphere center/radius range checks and sphere/cylinder collision forwarding. The contiguous unit includes constructor, setter overloads and generated initialization; GameCube’s float-output sphere-cross overload is absent here, probably inlined or removed as unused. No gameplay change identified.

### c_bg_s_gnd_chk — verified, 2026-10-04

- Structure: HD ground-check construction retains the GameCube sixty-four-byte object, initializes position from the shared zero vector, sets the invalid process identifier and enables ground and wall flags. HD also initializes query virtual tables, default height and precheck state explicitly. The derived destructor resets its secondary virtual table, invokes the base destructor and conditionally frees storage. The full unit includes its adjacent compiler-generated initializer. No gameplay change was found.


### d_a_boko (weapon actor)

**Gameplay:** HD caches ground heights per spear trail segment, refreshes selected segments on an eight-frame schedule, and adds horizontal sine forces and length-dependent damping. Waiting adds root/tip ground queries and a vertical line check that can lower the weapon and set its pitch.

**Graphics / Removed:** This actor omits the GameCube burning-weapon dark-room alpha/spot submissions and dropped Stalfos-mace shadow submission; model and spear-line drawing remain.

**Structure:** The larger actor and animation layout moves state fields, and the spear line uses a larger material prefix before its sixteen segment states.

### d_a_player_npc — Player NPC

Structure: HD allocates a 0x608-byte Player NPC object compared with GameCube’s 0x4EC layout, while retaining name/distance search and restart-position behavior. HD adds a nonnull manager callback after partner registration. Its stage data uses the HD virtual slots; the search callback receives a 40-byte packet. The adjacent static initializer is attributed to this unit from its constants, descriptors and vtable layout; that attribution remains inferred. Native startup/lifetime behavior beyond the isolated function comparisons remains unverified.

### d_a_npc_bj1 (Koroks of the Forest Haven) — verified, 2026-10-04
- Structure: the GameCube decompilation has only placeholder bodies and no members for this actor, so the comparison is limited to layout; the HD actor is 0x9C8 bytes on a 0x7DC-byte NPC base, with the current action kept as a pointer-to-member descriptor at +0x80C (direct or virtual dispatch).
- Structure: resources are looked up through string-keyed HD resource ids, joint matrices and joint-name tables come from the HD J3D model layout, and the joint callback setter is bounds-checked in HD.
- Structure: reconstruction completed; all 125 functions of the unit verified (the eight entries directly after the range belong to d_a_npc_bm1).

### c_m3d_g_pla

Structure: HD keeps GameCube normal/point normalization and plane-angle forwarding, with constructors, plane copying and two Y intersection helpers as separate functions. The object occupies 20 bytes, with its virtual table pointer after the normal vector and plane constant. No material gameplay change identified; plane-height evaluation retains HD fused arithmetic.

### c_bg_s_lin_chk — verified, 2026-10-04

- Structure: Line checks retain GameCube endpoints, actor ID, collision flags, front/back tests and polygon-info clearing. The HD constructor allocates eighty-eight bytes when passed null and initializes class and secondary virtual tables. Reset passes a complete twelve-byte vector to the line helper; Set2 preserves ordered endpoint copies when pointers overlap. The full unit includes its adjacent compiler-generated initializer. No demonstrated gameplay fix was found.

### c_m2d — verified, 2026-10-04

- Structure: HD retains the GameCube circle-line calculation and output coordinates, with contracted arithmetic and operand order that matter when floating-point results reach overlapping output storage. The full unit includes its adjacent compiler-generated initializer. No material gameplay or graphics difference was identified.

### c_m3d_g_cyl — cylinder geometry (2026-10-04)

- Structure: the HD cylinder retains a 24-byte layout with center, radius, height and virtual table. Its setters check NaN values and coordinate limits through diagnostic calls before updating the fields; combined setters and cylinder/sphere intersection wrappers remain separate functions. No gameplay or graphics change was established from this unit alone.


### c_m3d_g_tri (triangle geometry)

**Structure:** The triangle size and vertex offsets remain the same as GameCube. HD emits the default constructor, Up, setPos and setBg as standalone functions; cylinder crossing and vertex-derived plane setup retain the GameCube behavior. A startup initializer is probably associated with this unit by its position. No gameplay or graphics change was found.

### c_m3d_g_aab — axis-aligned bounding boxes

The HD unit contains 16 entries in 02017D08..02018030: direct corner assignment, XZ containment and Y-plane queries, full/Y-only sentinel clearing, minimum/maximum expansion, vector and box expansion wrappers, Y-only updates, center calculation, radius expansion, and a per-unit static initializer at 02017FA0. The following 02018034 destructor and 02018048 allocating constructor belong to the neighboring 0x14-byte circle layout (vtable at +0xC, radius at +0x10), so are excluded from AAB.

The original GameCube source exposes three out-of-line expansion methods; HD also emits the inline box operations as callable functions and adds static SDK registration and math-global initialization. The HD AAB retains the six float coordinate fields at +0..+0x14. Min/max operations retain strict ordered comparisons, including their behavior for unordered NaN values. Source preserves the reference's component preload and sequential store order so overlapping box/vector addresses behave the same. Direct corner assignment copies raw words, preserving all float bit patterns. Radius expansion uses explicit single-precision arithmetic and the reference's NaN operand order.

The center wrapper calls vector addition followed by vector scaling with the HD half constant. Vector/box expansion wrappers preserve the register values the reference forwards after its first call. There are no indirect calls, local objects passed to callees, shared harness changes, or fixture restrictions in this unit. Verification covers stock generated reference contracts with external callees mocked; native gameplay integration is outside that evidence.

### c_m2d_g_box

Structure: HD retains box point copying and region-based distance calculation. Diagonal cases call shared distance-squared and square-root helpers. Constructor, destructor and generated initialization remain explicit; the 20-byte object stores two 2D points followed by a virtual table pointer. No gameplay change identified.

### c_bg_s — verified, 2026-10-04

- Structure: HD background-collision registry entries occupy twenty bytes rather than the supplied GameCube sixteen, with a per-entry virtual table; the registry object allocates 5,124 bytes for 256 entries and its own virtual table. Registration, removal and collision queries retain the supplied GameCube roles. The full unit includes entry construction and its adjacent compiler-generated initializer. The function matched as ShdwDraw is actually the registry constructor; no standalone shadow-draw body was found in this unit.

### f_pc_line_tag — verified, 2026-10-04

- Structure: HD line tags retain the GameCube queue, removal, movement and initialization behavior, with a list identifier following the base tag. The full unit includes its adjacent compiler-generated initializer. No material behavior change was found.

### c_m3d_g_cir — circle geometry (2026-10-04)

- Structure: the HD circle occupies 20 bytes, with a three-component center, a virtual table at offset 12 and radius at offset 16. Construction initializes a 16-byte position base, installs the circle table and zeroes the radius; the scalar setter updates all four values. No gameplay or graphics change was established from this unit alone.


### d_a_oship — Great Sea warship

**Structure:** The HD actor grows from 3,584 to 3,868 bytes, with gameplay and embedded collision fields shifted by the larger actor base; cylinder strides and the HIO object size remain unchanged. HD resource lookup constructs an SDK SafeString carrying its virtual table. The solid-heap wrapper chooses its size internally and ignores the legacy caller size hint.

**Gameplay:** The sinking event, salvage notification, event-bit setting and flag deletion retain the GameCube flow. The apparent Triforce-map index change is cancelled by the GameCube wrapper, so it does not indicate a gameplay change. No verified warship balance change was found.

### d_stage (stage/room data, room control, scene changes) — verified, 2026-10-04
- Gameplay: when Link enters a room whose room-read list names several rooms, HD creates the room scenes for every listed room that is not loaded yet; the GameCube code stopped after the first one.
- Gameplay: dStage_playerInit takes the player-list count from the chunk; an unknown start point falls back to the first entry instead of asserting; the meter process is always created and the GBA link actor (probably the Tingle Tuner) is no longer spawned.
- Gameplay: dStage_Create spawns the two sky-box processes whenever the stage has vr_sky.bmd, except in GTower and Hyrule (GameCube: whenever vr_sky.bdl exists).
- Graphics: dark rooms keep only the colour-set blend (dKy_change_colset); the player-centred spot models that lit the area around Link in dark rooms (checkDrawArea) are gone.
- Removed: the room memory blocks (create/destroy of per-room expanded heaps); the MEMA/MECO chunks are still dispatched but their handlers are empty stubs; GHS dead-stripped many never-called set/get virtuals of dStage_roomDt_c / dStage_stageDt_c.
- HD-only: dStage_dt_c_roomLoader registers a crash-dump context around the room-file parse; its callback (025C51C0) prints the room-file and stage pointers and a hex dump of the first 2 KB of the room file. A room-change counter (1047E6C0) is reset at stage creation and counted by dStage_RoomCheck.
- Structure: dStage_roomControl_c is static (statuses at 1047E6CC, 64 x 0x22C; stay numbers 1047E6C8/9; dark ratio 1047E6CA); the stage data object is embedded in the play object at +0x5150 and the room control at +0x51CC; time pass moved to a static (101EBFD8); the dark statuses are a static table (101EE5C8); dStage_roomDt_c is 0x54 bytes and dStage_stageDt_c 0x7C, with HD-style 8-byte vtable entries; the GameCube helpers dStage_KeepTresureInfoProc/KeepDoorInfoProc, dKankyo_create, dStage_roomInit, stayRoomCheck and the Ikada argument getters are inlined.

### m_Do_mtx

- Graphics / Structure: The HD matrix unit retains axis rotations, rotation composition, both look-at forms, projection/view concatenation, inverse-transpose, quaternion multiplication, angle extraction and the bounded matrix stack. Its contiguous range also contains an unnamed XYZ rotation-set entry. The supplied GameCube source has the same main algorithms; no material gameplay change was identified. HD reconstruction preserves the compiler's single-precision multiply-add grouping, ordered comparisons and sequential quaternion stores, including output/input aliases. Matrix and vector temporaries follow the HD SDK call contracts.

### f_pc_method

Structure: HD retains callback-based process method dispatch for create, execute, delete and deletion checks, including success when the method pointer is null. The contiguous unit includes its generated static initializer. No gameplay change identified.

### f_pc_layer_tag — verified, 2026-10-04

- Fix: For an explicit layer ID, HD ToQueue checks whether the stored layer pointer is null before reading its ID, then performs lookup when needed. The supplied GameCube expression dereferences that pointer without the null check.
- Structure: HD retains the 28-byte layer-tag layout and the queue/move/init roles, forwarding full list and priority register values until halfword stores. The full unit includes its adjacent compiler-generated initializer.

### f_pc_create_tag — verified, 2026-10-04

- Structure: HD retains the GameCube creation-queue tag initialization, addition and removal roles. The full unit includes its adjacent compiler-generated initializer. No material behavior change was found.

### f_pc_method_tag — verified, 2026-10-04

- Structure: HD retains GameCube method-tag queue insertion, removal, initialization and callback behavior. Do tail-calls the stored callback with its user-data argument. The full unit includes its adjacent compiler-generated initializer. No material behavior change was found.

### f_pc_delete_tag — verified, 2026-10-04

- Structure: HD retains GameCube signed deletion-timer and requeue behavior. The queue-cut operation is inlined before the deletion callback. The full unit includes its adjacent compiler-generated initializer. No material gameplay change was found.

### f_pc_deletor — process deletion scheduler

- **Structure:** HD folds the queue-admission helper into the recursive deletion routine; completion checking and the list handler remain separate entries. The unit also includes compiler-generated initialization for shared header statics.
- **Removed:** The successful delete method no longer captures the profile name or calls the GameCube profile-unload routine. It still changes the current layer, removes the line tag, deletes the process and sends the layer deletion notification.
- **Gameplay:** The request guards, recursive child cancellation, priority removal and final deletion-state transition retain the GameCube scheduling decisions; no intentional gameplay change was identified.

### f_pc_pause — process pause flags (2026-10-04)

- Structure: HD stores the process pause flags in a byte at offset 11. Enable and disable update that byte and, for a node process, pass the operation callback and mask to the child-list traversal. A separate query tests whether all requested bits are set; initialization clears the flags. No gameplay change was established from this unit alone.

### f_pc_priority

Structure: HD retains process priority changes and queue handling, with priority records embedded at process offset 0x68. Queue insertion and queue-item extraction are inlined into change and handler functions; queue removal also resets the requested priority record. The unit contains seven separate functions including its generated initializer. No gameplay change identified.

### f_pc_profile — process profile lookup

The HD unit has two entries: fpcPf_Get at 025E10E0 and the per-unit static initializer at 025E10F4..025E1184. Lookup attribution is established by fpcBs_Create at 025DD414, which calls 025E10E0 at 025DD430 before reading the returned profile's allocation sizes at +0x10 and +0x14. Initializer attribution follows TU adjacency; the previous initializer025E104C ends at025E10DC, and the next initializer starts025E1188.

GameCube fpcPf_Get loads an element through the mutable global g_fpcPf_ProfileList_p. HD indexes the embedded guest table at101F3EE0 directly, shifts the incoming integer register by two, and returns the selected32-bit profile pointer. HD does not sign-extend the incoming register within this function; normal callers supply the intended profile number. The candidate retains the raw register/address-wrap semantics and introduces no bounds checks.

HD also emits SDK registration and local math-global initialization as a static initializer. The candidate retains the exact store/call order and argument addresses. Neither entry uses indirect calls or stack objects passed to callees. Both are verified using unrestricted stock generated fixtures; external initializer calls are mocked. This is evidence for the reference function contracts, not a claim about invalid profile numbers or native gameplay integration.

### f_op_draw_tag

Structure: HD preserves draw-tag queue insertion/removal and process association, including the 1000-entry queue. Queue insertion, removal and creation use tail-call forwarding to common tag/tree operations. Generated initialization is retained. No gameplay or rendering change identified.

### c_lib — utility interpolation and matrix stack (2026-10-04)

- **Structure:** HD delegates byte filling to the Wii U OSBlockSet import; the GameCube source uses memset. The HD utility range also contains separate duplicate yaw-angle and position-offset helper bodies, whereas the inspected GameCube source has one body for each.
- **Graphics:** The matrix translation, rotation, scale, vector-position and push/pull utilities retain the GameCube matrix-stack design with 48-byte matrices. HD uses its SDK matrix routines and reloads the current matrix pointer after calls where the reference does so.
- **Structure:** The current-matrix reset matches GameCube MtxInit. The following compiler-generated static initializer is probably associated with this TU by its position in the binary; that attribution is not a matched symbol.
- No gameplay change is established by these function-isolated checks. All 33 functions pass 10,000 generated inputs at seeds 1 and 7 with 187/187 reference blocks covered.

### f_pc_create_req — process creation requests

- **Structure:** HD retains the GameCube ID lookup, cancellation, callback phase dispatch and queue lifecycle. Queue-removal and queue-admission wrappers are folded into Delete and Create; the adjacent header-static initializer is included in the verified unit.
- **Structure:** HD explicitly clears the creating flag and the byte at request offset 0x38 when allocating a request. The cancellation guard and completed/error phase decisions otherwise match the GameCube behavior.

### f_pc_stdcreate_req — standard process creation

**Structure:** HD removes the GameCube load phase and no longer frees a module after failed process allocation. The request remains96bytes, with relocated layer, phase, process-name, append and callback fields. The handler uses an iterative phase loop; completion still requires both queried booleans to equal one. A trailing SDK initializer is included by translation-unit adjacency, with that attribution marked as inferred.

### f_pc_leaf — leaf process lifecycle (2026-10-04)

- Structure: HD leaf processes retain a method-table pointer, type ID, draw inhibition byte and signed priority field. Creation takes the methods and priority from the profile, while execute, draw and deletion delegate through separate method dispatchers with the process as their argument. Successful deletion clears the type ID. No gameplay change was established from this unit alone.

### f_pc_base — process lifecycle and IDs

**Fix:** HD skips the two reserved process-ID values at wraparound; the GameCube allocator simply increments. HD also checks that both profile allocation sizes are aligned to four bytes before allocating.

**Structure / HD-only:** The common process fields retain their offsets through the parameter word. HD supplies explicit allocating constructor/destructor functions and writes a virtual-table pointer at the final word of the 0xB8-byte base object; the GameCube header uses that word for a subtype. The adjacent startup registration is probably associated with this unit; this attribution is based on address order rather than a matched function name. The lifecycle calls and creation-phase transitions otherwise retain the GameCube behavior.

### d_a_fganon (Phantom Ganon boss) — verified, 2026-10-04
- Graphics: the draw function additionally sets or clears a visibility bit on the body and sword models every frame from the "materialized" flag (GameCube mbIsMaterialized) and refreshes the models; on the GameCube that flag does not reach the draw code (appearing and vanishing was only the brightness-animation fade). Probably the HD renderer needs the models explicitly hidden while he is dematerialized.
- Structure: layout shift +0x11C up to the sword's light/colour state, which is larger in HD, so everything after it shifts by +0x234; the actor is 0xDD0 bytes (GameCube 0xB8C) with 0x10 HD-only bytes at the end. demo_camera is inlined into Execute, and the movement actions (standby, appear, disappear, defeated, fly, shot, spin) into move. The HIO object's vtable comes after its data.
- Structure: no gameplay difference found in movement, attack, damage or deletion logic beyond the layout.


### f_pc_method_iter — process method list delegation

Structure: HD retains the GameCube wrapper that delegates a list and method callback to list iteration with a null user argument. The delegated return value is preserved. The unnamed HD wrapper is identified by this argument setup and its unique tail call. The immediately following compiler initializer constructs header/math global state; assignment to this unit follows adjacency and the repeated per-unit pattern. No gameplay change was found.

### c_malloc — verified, 2026-10-04

Structure: HD retains the heap interface initialization, aligned allocation and free wrappers. Allocation forwards size and alignment through the heap virtual table; free forwards the memory pointer. Zero-size allocation and null free return early. The adjacent math initializer is included with its exact header attribution qualified. No gameplay change was found.

### f_pc_draw_priority — verified, 2026-10-04

Structure: HD retains signed 16-bit priority reads, halfword writes and initialization delegated to the setter. The priority layout remains two bytes. The adjacent math initializer follows the repeated per-unit pattern; its exact header attribution is qualified. No gameplay change was found.

### f_pc_line_iter — verified, 2026-10-04

Structure: Line iteration retains the GameCube layer-scoped callback behavior: save the current layer, switch to the process layer, invoke the callback, restore the saved layer and return the saved result. Queue iteration passes an eight-byte callback/user-data filter with null user data. The adjacent math initializer is included without asserting its exact generating header. No gameplay change was found.

### f_pc_line

- Structure: The HD process line manager creates sixteen lists with twelve bytes per list, retaining the line scheduling structure. Its adjacent compiler initializer prepares HD singleton support objects and shared angle constants. No gameplay difference was identified in these two functions.

### f_pc_node_req — node request scheduling

- **Structure:** HD inlines queue admission/removal and the three create/change/delete constructors into the request and deletion routines. The constant delete-timing phase is emitted after the header-static initializer; the re-request alias has no separate mapped body in this TU.
- **Structure:** The request template remains 100 bytes, with a monotonically incremented request ID and separate asynchronous creation and deletion phases. The handler invokes the configured execute callback, then reloads the embedded node’s next link before cancellation or deletion.
- **Gameplay:** Target-exclusion checks still recognize deletion, replacement and the reserved selection request type. No intentional gameplay change was identified from the reference comparison.


### f_pc_executor — process execution and queues

**Structure:** HD retains the eight GameCube search, execution, queue and handler operations, using the HD process and layer offsets. The two reserved process IDs still return no search result; execution still requires the ready state and pause permission. Successful queue operations update process state and recursively admit child layers for process nodes. No material gameplay change was found.

### c_cc_d

- Structure: HD retains broad-phase divide masks, collision status and object setup, double dispatch across capsule/triangle/cylinder/sphere attributes, bounding boxes and surface normals. The complete unit includes local geometry constructors, deleting destructors, no-hit defaults and its static initializer. Shape geometry follows the base bounding box and attribute virtual tables, at offset 32; capsule and triangle geometry objects occupy 32 and 56 bytes. Status initialization calls the virtual reset before assigning weight, flags, actor and process ID.
- Graphics / Fix: No material gameplay change was identified against the supplied GameCube algorithms. Reconstruction preserves HD's ordered NaN branches, saturating float-to-integer conversion and Espresso shift semantics in divide masks, plus the reference's assertion calls and argument/load ordering. The initializer also registers the SDK globals and seeds the virtual fallback center.

### f_pc_node (process nodes)

**Structure:** HD stores the node method table at +0xBC, the embedded layer at +0xC0, sixteen layer lists at +0xEC, and draw suppression at +0x1AC. The GameCube creation, layer switching, descendant creation query and deletion control flow remain recognizable. HD also emits its usual support-object static initializer. No gameplay or graphics behavior change was identified in these nine functions.

### f_pc_searcher — process search predicates

Structure: HD retains the GameCube process-name and process-ID equality judges. The name field is a signed 16-bit value; IDs use 32 bits. The unit includes a static initializer attributed by TU adjacency. No gameplay change was identified.

### f_pc_draw — process draw dispatch

Structure: HD retains GameCube pause gating, current-layer save/restore, draw callbacks and draw-handler bracketing. Leaf and node method tables use the same draw callback offset in HD, so the type-check call remains while both paths select the same slot. The unit also retains generated initialization. No gameplay or rendering change was identified.

### f_pc_load — retained HD initializer

The GameCube unit implements fpcLd_Use, fpcLd_IsLoaded, fpcLd_Free, and fpcLd_Load through cDyl_IsLinked, cDyl_Unlink, and cDyl_LinkASync. Those dynamic actor-module loader calls are absent from the corresponding HD creation paths: fpcFCtRq_Request025DE884 creates the process directly and fpcSCtRq_phase_CreateProcess025E12E0 calls fpcBs_Create without a prior dynamic-load phase or an unlink on its failure path.

The HD unit therefore contains no reconstructed loader/cache API. Its retained entry is the SDK/math static initializer025DF870..025DF900. Attribution is inferred from the native unit sequence: line creation025DF468 and initializer025DF4B4; line iteration025DF548/025DF5C0 and initializer025DF600; line tags025DF694..025DF7A8 and initializer025DF7DC; the lone initializer025DF870; then process manager methods beginning025DF904. This initializer attribution is a TU-adjacency qualification, not a symbol-map assertion.

The candidate reproduces all six registration/math stores and five direct SDK calls in their native order and passes the exact guest argument addresses. There are no indirect calls, local objects passed to callees, source-side fixture restrictions, or shared changes. Verification covers stock generated reference contracts with the SDK callees mocked; no dynamic loading implementation, native cache correctness, or gameplay claim is implied by the initializer result.


### f_pc_create_iter — creation queue callbacks and layer filtering

Structure: HD retains the GameCube creation queue method/judge wrappers, packing callback and user data into an eight-byte stack filter. Layer judgment uses a twelve-byte layer/callback/user filter, compares the requested layer against the creation request’s layer ID, and invokes the callback with resource and user arguments only on a match. The unnamed HD functions are identified by those queue, filter, and callback operations. The immediately following compiler initializer constructs header/math global state; exact assignment to this unit follows adjacency and the repeated per-unit pattern. No gameplay change was found.

### c_API_graphic — verified, 2026-10-04

Structure: Before-draw and after-draw dispatch the corresponding graphics interface callbacks. Their HD wrappers use tail dispatches and retain an adjacent compiler registration initializer with exact header attribution qualified.

Removed: The GameCube Painter forwarding wrapper has no separate function in this HD unit range. The painter callback remains in the graphics interface table; this finding concerns the forwarding wrapper.

### f_op_scene_iter — verified, 2026-10-04

Structure: HD retains scene list judgment through an eight-byte callback/user-data filter and the tag judge adapter. It returns the delegated result. The adjacent math initializer follows the repeated per-unit pattern; exact header attribution is qualified. No gameplay change was found.

### f_op_camera_mng

- Structure: The HD camera process manager reads the parameter word at offset 176 and stores the newly requested process identifier in the indexed camera table. The creation wrapper preserves the current-layer lookup before requesting creation. An adjacent compiler initializer prepares HD singleton support; its unit attribution is inferred from adjacency. No gameplay change was identified in these three functions.

### f_op_overlap (overlap process framework)

**Structure:** HD uses the method table at +0xC4, request state at +0xC8, scene ID at +0xCC, and profile method pointer at +0x24. The five lifecycle callbacks and first-creation request setup remain recognizable from GameCube. The adjacent HD support-object initializer is included, with translation-unit attribution inferred from adjacency. No gameplay or graphics behavior change was identified.

### f_op_kankyo_mng — environment processes and pillar effects

**Structure:** Environment append data retains the GameCube 0x1C-byte position, scale and parameter layout. HD forwards creation through the current process layer and retains the same allocation-failure results. Position and scale copies preserve the original component order.

**Graphics:** Water pillars still use an environment process with separate horizontal and vertical scales. Magma pillars directly submit the particle effect with a three-component scale and return the same failure-style sentinel. No gameplay change was identified in these helpers. The unnamed type-check wrapper and startup registration are probably part of this unit, based on adjacency rather than matched source names.

### f_pc_layer_iter — verified, 2026-10-04

Structure: HD retains layer-local iteration, temporary current-layer switching with restoration, and first-success judging across the layer list. Callback and user-data filters remain eight bytes; node-tree access is at layer offset sixteen. The adjacent math initializer follows the repeated header-generated pattern; its exact generating header is qualified. No gameplay change was found.

### f_op_scene_mng

- Structure: HD scene searches pass a word identifier or halfword profile name through temporary search keys. Scene creation, deletion and transition wrappers share a request dispatcher, with the last transition request retained for retries. Management retains a diagnostic on failed execution. An adjacent HD singleton initializer is attributed by location. No gameplay difference was identified in these eight functions.

### f_pc_layer (2026-10-04)

Structure: HD keeps the process-layer queue and signed16-bit creation/deletion counters; layer lookup folds the GameCube search into the layer selector. The44-byte default-layer copy remains wordwise. HD also has an adjacent compiler initialization routine for singleton support, attributed by translation-unit adjacency. Gameplay behavior of the layer bookkeeping appears unchanged.

### f_op_overlap_req (overlap request phases)

**HD-only:** Request takes a fourth argument whose low byte is retained in the request and copied into game state once task creation succeeds. This probably selects transition behavior; its purpose is unconfirmed. **Structure:** The phase machine, request flags, signed delay, peek-time countdown and task/layer pointers remain recognizable from GameCube. The adjacent HD support-object initializer is included with attribution inferred from adjacency.

### f_op_scene_req

- Structure: HD scene requests use a 116-byte request object with an overlap handle and phase state. A sentinel profile bypasses overlap creation; competing overlap requests are rejected and the allocated request is removed. Execution repeats advancing phases, and completion clears the overlap flag. An adjacent HD singleton initializer is attributed by location. No gameplay change was identified in these twelve functions.

### f_pc_creator — creation forwarding

Structure: HD retains process creation abort and handler forwarding; abort reads the creation-request pointer at process offset 0x14. The contiguous creator range also contains generated initialization. GameCube creation-status forwarding wrappers were not found here, probably inlined or removed as unused. A following independent registration cluster has an unknown TU name and is excluded. No gameplay difference was identified.


### f_op_draw_iter — drawing queue traversal

Structure: HD retains the GameCube drawing queue iterator. Begin resets the global list cursor and returns the first head; Next returns the current node’s successor or advances through later lists. GetTag skips empty lists and returns null when the signed list bound is exhausted. HD uses a twelve-byte list stride and the node successor at offset eight, consistent with the retained layout. The immediately following compiler initializer constructs header/math state; exact unit assignment follows adjacency and the repeated per-unit motif. No gameplay change was found.

### c_API — verified, 2026-10-04

Structure: The graphics interface definition retains seven roles: graphics information, creation, before-draw, after-draw, painter and the two blanking callbacks. HD has an adjacent compiler initializer performing shared math/static registration. Its attribution to this definition unit follows binary boundaries; precise header attribution is qualified. No gameplay change found.

### f_op_view — verified, 2026-10-04

Structure: HD retains the view method wrappers and creation-time profile setup. The method pointer moves from the GameCube offset 0xC0 to HD 0xC4; the profile byte moves from 0xC4 to 0xC8. Creation loads the profile pointer inline and preserves write and call order. The adjacent compiler initializer is included with its exact header attribution qualified. No gameplay change found.

### f_op_scene_tag — verified, 2026-10-04

Structure: HD retains the scene tag cut, queue insertion and creation wrappers, including the same callback data forwarding. The adjacent compiler initializer follows the repeated per-unit pattern with exact header attribution qualified. No gameplay change found.

### f_op_scene (scene lifecycle)

**Structure:** HD keeps the scene method pointer at +0x1B0 and scene tag at +0x1B4. Creation registers the tag and copies optional append parameters, while deletion updates host I/O after conditional tag removal, as in GameCube. The adjacent HD support-object initializer is included with attribution inferred from adjacency. No gameplay or graphics behavior change was identified.

### f_op_overlap_mng — verified, 2026-10-04

Structure: HD retains the single active overlap request, scene pause switching, phase completion cleanup and cancellation behavior. The manager forwards an additional request argument beyond the two GameCube arguments; its exact role is unconfirmed. Task request flags occupy the low six bits of a packed byte. The adjacent math initializer follows the header-generated pattern; exact header attribution is qualified. Removed: the successful-cancellation warning call from the GameCube source is absent. No gameplay change was identified.

### f_pc_profile_lst — embedded profile table

Structure/Removed: GameCube publishes and clears a profile-table pointer through module prolog and epilog entry points. HD profile lookup uses an embedded table directly, as recorded for f_pc_profile; those publication wrappers have no corresponding retained entries here. This unit retains a static initializer attributed by native TU adjacency. No gameplay change was identified.

### c_cc_s — collision manager

**Structure:** The retail manager keeps the GameCube layout and four fixed pointer-list capacities. HD has an allocating constructor that explicitly clears the target, contact and complete-object arrays and their counts before divide-area construction; clearing the attack array remains in Ct and DrawClear. The GameCube constructor body is empty. Collision-mask filtering, maximum-damage updates, weighted two- and three-dimensional separation, and callback ordering retain the GameCube behavior. The startup initializer is probably part of this unit based on adjacency. No gameplay or graphics change is claimed.


### f_pc_fstcreate_req — first process creation requests (2026-10-04)

- Structure: HD uses an 80-byte first-creation request that links the newly allocated process, its ID, and a callback with user data. Creation succeeds only after the process reaches the expected creation phase; failure cancels the request. The completion phase calls the saved callback with the process and user data, returning different phase results for callback failure and success. No gameplay change was established from this unit alone.


### f_op_scene_pause — scene pause propagation

Structure: HD retains the GameCube pause logic. Enable sets bits one and two for a nonnull scene. Disable clears both bits when no parent process exists; for an existing parent, it clears each bit only when the parent lacks that pause state. Null scenes return zero, while handled scenes return one. The immediately following compiler initializer constructs header/math global state; exact attribution follows adjacency and the repeated per-unit motif. No gameplay change was found.

### c_sxyz — verified, 2026-10-04

Structure: HD retains the GameCube three-component signed-short vector constructor, addition, addition assignment and float scaling. The constructor can allocate six bytes when invoked without an object. The initializer also constructs the shared zero vector. Aggregate results use two return registers; arithmetic preserves short wrapping and truncating float conversion. Exact initializer header attribution is qualified. No gameplay or graphics change was found.

### f_op_actor_tag — actor queue tags

The HD unit retains the three GameCube queue-tag operations: append a tag to the global actor queue, remove a tag through the shared tree-cut helper, and initialize a tag with its actor data pointer before returning success. HD stores its global actor queue at a fixed address and emits the append/remove wrappers as tail calls. Their return values and incoming argument registers are preserved.

HD additionally emits a per-unit static initializer for SDK registration and local math globals. The complete native unit has four entries at025DA2F8,025DA308,025DA30C,025DA330; the initializer ends025DA3C0. The preceding025DA2F4 empty destructor is already in the actor-manager unit, and the following025DA3C4 begins camera processing. The initializer association is inferred from this native adjacency.

The candidate performs the same direct calls and registration stores in native order. No layout modification, indirect call, local object passed to a callee, input restriction, or shared harness change is needed. Both seeds test the unrestricted generated reference contracts while queue and SDK callees remain mocked. This validates the wrappers, not a complete native queue lifecycle or gameplay integration.

### d_bg_s_wtr_chk

- Structure: HD water collision checks occupy 80 bytes. Construction invokes the special-group check base, sets the water filter bit and replaces four embedded virtual table pointers. The null receiver allocation path is retained. An adjacent singleton initializer is attributed by location. No gameplay change was identified in these two functions.

### f_op_camera (2026-10-04)

Fix: camera execution returns success when menu or gameplay pause suppresses execution; the GameCube reconstruction left that result uninitialized. Structure: HD method pointer is at0x228 and draw tag at0x214. Creation uses the first-create byte and profile directly, then registers the draw tag when creation completes. Draw,delete,andis-delete retain their process-method behavior. Adjacent compiler singleton initializer attribution is inferred.

### f_op_msg (message lifecycle)

**Structure:** HD uses the message type at +0xC4, draw tag at +0xC8, method table at +0xDC, and seven append words at +0xE0 through +0xF8. Creation preserves raw position words while copying append data. Pause gating and draw-queue lifecycle remain recognizable from GameCube. The adjacent support-object initializer is included with attribution inferred from adjacency. No gameplay or graphics behavior change was identified.

### m_Do_lib

- Graphics: HD projection uses 1,280 by 720 screen dimensions and 640/360 center offsets, replacing the supplied GameCube 640 by 480 and 320/240 constants. The unit also contains a camera-system projection route and renderer material/color helpers; those names are inferred from their behavior. The GameCube GX texture-initialization helper was not found in this contiguous HD unit range.
- Structure: Projection obtains game and projection/view state through accessors; camera-space conversion reads the view matrix from its HD view object. The viewport-copy helper writes forty bytes. Screen projection constructs a twenty-four-byte rectangle including disposer fields, alongside its vector and two-float output. Clipper setup and byte-order conversion retain their GameCube roles.
- Fix: The reconstruction explicitly preserves compiled-reference NaN payload selection when later partial memory effects expose depth-word bytes. This is a reference-model fidelity correction, not evidence of changed console NaN behavior; native renderer and console behavior remain outside the isolated mocked-callee gate.


### m_Do_hostIO — verified, 2026-10-04

Removed: HD child registration returns zero and child deletion has no effects. The GameCube routines managed 64 named entries and reported duplicate, full-table or invalid deletion cases; those operations are absent from these HD wrappers.

Structure: Two adjacent empty stubs are included in the bounded cluster. One has scene-deletion update call context; the other's startup role remains unknown, so neither receives an unsupported high-level name. No trailing initializer is assigned to this unit.

### d_bg_s_spl_grp_chk

- Structure: HD special-group collision checks occupy 80 bytes and connect two embedded filter objects through guest pointers. Construction sets the invalid polygon identifiers and default height bounds; reset restores the invalid identifiers, clears two status bits and copies the input height. The adjacent singleton initializer is attributed by location. No gameplay difference was identified in these four functions.

### f_op_kankyo (environment lifecycle)

**Structure:** HD places type, draw tag, method pointer and append data at +0xC4, +0xC8, +0xDC and +0xE0 respectively. Menu exceptions still permit the two environment audio process types, identified by HD IDs 21 and 24. **Fix:** HD explicitly returns success when menu or pause gating skips work; the readable GameCube source has an uninitialized local return in these paths, although this alone does not establish a console behavior change. The adjacent support-object initializer is included with attribution inferred from adjacency.

### m_Do_audio — audio compatibility entry points

- **Structure:** The HD unit contains 106 emitted functions, including the timer/frame/scene/wave routines, singleton audio API wrappers, the global audio/tact initializer, two constant-zero stubs and the audio deleting destructor. Several legacy name mappings describe backend methods even though the HD entries are singleton wrappers.
- **Removed:** GameCube stream-buffer allocation, heap fallback and local audio-resource loading/Create routines are absent from this HD cluster. Initialization now belongs to the separate backend/bootstrap rather than this local compatibility layer.
- **Gameplay:** Tact direction keeps the angle thresholds and previous-direction hysteresis, but a stick below the magnitude threshold now returns neutral directly; the GameCube main-stick path also checked held directional buttons.
- **HD-only:** One sound-start wrapper routes sound ID 0x806 to a dedicated helper instead of the general sound-start backend. The reason for that special case remains unidentified.

### d_bg_s_func — verified, 2026-10-04

Structure: HD retains object-ground and water queries, special-group membership, sea-wave maximum selection and ground material sound lookup. Ground-check temporaries occupy eighty-four bytes, combining the sixty-four-byte base with embedded filters; queries use the game-info singleton accessor and perform local destructor cleanup. Float height comparisons retain ordered behavior for NaNs. The adjacent math initializer follows the repeated header-generated pattern; exact header attribution is qualified. No gameplay change was identified.

### d_bg_s_lin_chk (2026-10-04)

Structure: HD line-check Set still forwards start/end and actor process ID to the base Set2, using the error ID for a null actor. The ID comes from actor offset4. HD emits a duplicated null check with a contradictory edge and an adjacent compiler singleton initializer, attributed by adjacency. Gameplay behavior appears unchanged.

### d_att_dist — attention distance data (2026-10-04)

- Structure: this translation unit contains distance-table data and no callable functions. The 186 GameCube records, each containing six float parameters and angle-check flags, match the corresponding HD table byte for byte. The HD enemy-distance check uses the same 28-byte record stride. Possible additional HD records were not assessed.

### m_Do_graphic — graphics state and draw boundaries

- **Structure:** HD retains dual graphics heaps and the blur, fade and monotone state operations. The verified unit includes its adjacent compiler-generated initializer and emitted display-control/color wrappers.
- **Graphics:** Fade drawing delegates to the HD display implementation instead of issuing the GameCube GX state and quad commands here. Monotone on/off only updates the state byte in this HD body; the GameCube texture-format changes and particle texture-swap calls are absent.
- **Removed:** The GameCube framebuffer/depth-buffer allocation, capture routines and long post-draw renderer implementation are absent from this HD unit. Before/after drawing instead delegates to play-owned draw structures; this finding describes the unit boundary, not absence of those features from the HD game.

Lead2026-10-04 23:23 admitted mutant29 equivalent for shipped standard heaps: SDK buffer resize does not receive blur state and no game-installed allocator observes it. This is assumed-not-proven; arbitrary custom callbacks remain excluded, optional blur-byte watch deferred. Earlier OPEN history preserved in initial question.



### c_API_controller — verified, 2026-10-04

Structure: This bounded HD cluster provides button hold/trigger state, main and sub stick coordinates, magnitude and angle, and digital lock state. Port zero uses the HD controller; other ports return inactive values. Main stick magnitude clamps at one, while sub stick magnitude uses the console square-root sequence. Fixtures cover finite physical stick values. The adjacent compiler initializer is included with qualified header attribution. No direct identity with the legacy GameCube Convert/Read/Create routines is asserted.

### m_Do_machine — verified, 2026-10-04

Structure: HD startup checks settings and creates fixed-budget heaps: a 0x02A22000 system heap, 64 KiB command heap, 576 KiB archive heap, two 10 MiB heaps, and a Zelda heap based on remaining system space minus 64 KiB. The verified startup differs from the supplied GameCube arena-sizing and platform setup sequence; absence claims are confined to this function. Its adjacent compiler initializer has qualified header attribution.

### d_bg_w_deform — verified, 2026-10-04

Structure / Removed: The HD setup body accepts collision data and correction flags, with no model argument. It retains collision-world setup and optional back-vertex allocation but omits the GameCube model-format assertion and model skin-deformation setup calls. Whether those rendering responsibilities moved elsewhere is unconfirmed. The adjacent math initializer follows the header-generated pattern; exact header attribution is qualified.

### f_pc_manager — main process-management loop

- **Removed:** The GameCube DVD-drive status checks and error-message rendering paths are absent from this HD unit.
- **Structure:** HD retains the process deletion, priority, creation, execution and drawing sequence, with optional callbacks before and after execution/drawing. Its initializer has additional float state, and the small lifecycle, pause and layer-lookup wrappers remain separate HD entries.
- **HD-only:** The management loop updates play-state pointers from the selected active entry and finishes by calling an optional graphics object. These operations are absent from the GameCube implementation inspected here.

Shared opt-in mockglobal adopted by lead9f4d6353. Canonical opaque noargcallback fixture explicitly clears/replaces renderer singleton101F8344, detecting premature-load mutant24 by a returning graphics-call argument mismatch; no fabricated callback receiver or changed live-ins. Both full12x10kseeds PASS29/29; existingfixture/mockfield and parser-rejection controls pass. Original OPEN/proposal history retained.


### m_Do_printf — diagnostic reporting

**Structure / Removed:** HD retains the error and warning counters, temporary force-enable toggles, and variadic reporting wrappers, while the larger GameCube diagnostic helpers do not have separate bodies in this HD unit. Warning output uses the platform reporting API directly; error decoration uses an SDK printing wrapper. The adjacent initializer is probably generated support for the unit’s shared diagnostic state.

### d_path (room and polygon paths)

**Structure:** HD obtains game state through its singleton accessor and retrieves stage or room path information through virtual calls. Path records remain twelve bytes and points sixteen bytes. Polygon direction remains the difference from the current point to the next point, wrapping at the final point. Assertions and range checks remain present. The adjacent support-object initializer is included with attribution inferred from adjacency. No gameplay behavior change was identified.

### d_bg_s_movebg_actor

- Structure: HD moving-background actors occupy 992 bytes and keep the collision object and model matrix after the common actor fields. Creation builds the matrix, obtains collision data through an eight-byte HD resource name object, registers the collision object and invokes the derived actor callback. Execution accepts a derived matrix output or rebuilds the fallback matrix; deletion releases registered collision state. Default callbacks and deleting destructors are retained. An adjacent singleton initializer is attributed by location.

m_Do_main retained HD TU comprises four generated entries: LOAD_COPYDATE025F1658 returns1,main01init025F1660, singleframe025F172C (reference original behindruntimehook), and adjacency-attributed headerstaticinitializer025F1788 through181B. Periodicchecker025F1654 corresponds GCmDoMch_HeapCheckAll in m_Do_machine.cpp and remains an externalcallee, not a fifth mainfunction. GCmonolithic foreverloop is split into finite HD init/frame; init publishesresetdata1048D090, setsdevelopmentflag, createsmachine/graphics/controller, launchesstubDVDcallback and restorespreviousheap using1sentinel. Frame incrementswrappingcounter, gateschecker with nonzeroperiod and remainder, then readscontroller/audio/game in order. Initializer uses its actualnonstandard byteobject offsets07D/07E. GCthread startup, timing, memcard/dynamiclink and fullLOAD_COPYDATE parsing absent from these retained HD bodies; no global gamewide-removal claim. Passactualstaticplaygetterresult explicitly to02520020 perknownr3livein; all externalcallee arguments preserved, noLocal objects/nolivein overrides. Full4x10000seeds1/7PASS11/11; no gaps. Bounded37sample26returningdifferences3sourceequivalent8compile-invalid, no crash/timeouts. Finalfixturecompiledsample rechecked; finite resetcall frontier proves mutant11 separate from developmentbyte, fixeddisjointoperations prove12/18. Externalmock boundary and adjacentinitializer attribution qualified; optionalresearch deferred.


### d_2dnumber

- Graphics: The verified contiguous HD range retains the battery-angle display and two-picture object behavior of the corresponding GameCube routines: texture-derived sizes, paired digit shadows, the same screen offsets, and the angle-to-digit and visual rotation formulas. The matcher names remain tentative, and other GameCube classes in the larger source file are outside this verified range.
- Structure: HD adds an assertion call for angles outside the expected 22.5-to-45 range, formats the digit texture names with an explicit sixteen-byte bound, and obtains the current graphics port through the HD game accessor. Both constructors and the trailing initializer are included; initializer attribution is inferred from adjacency.
- Structure: The observed HD picture submission target has no float-register live-ins in the extracted reference body. Isolated verification therefore does not establish renderer behavior for the computed draw coordinates; the reconstruction keeps the reference calculations and target unchanged, and the six related mutation equivalences are explicitly scoped to that callee boundary. Native rendering remains unproven.

### d_a_bgn (Puppet Ganon, phase 1: the marionette) — verified, 2026-10-04
- Graphics: HD draws reflections on the water floor through the actor's model packets (mDoExt_J3DModelPacketS grows from 0x14 to 0xB0 bytes and gets an HD draw helper): the head, chest and body parts, the room, the blue and red strings and the defeat rope (three extra packets), plus Keese, Morth and the hanging-rope actor found by search callbacks. The room model is only reflected; this actor no longer draws it itself.
- Graphics: Puppet Ganon also enters Link's reflection packets from a list on the player. When Link is close it leaves out his hands, ring and sword meshes, and while he is low and turned away it also leaves out the bow and hookshot meshes. This is probably meant to keep held items from showing through the floor.
- Graphics: the packet draw skips two ear meshes while a save flag is set, and when a packet flag is set it draws the eye and eyebrow meshes that are otherwise hidden.
- Graphics: the first water layer's blend, z and colour state is set through the HD material interface and the colours are uploaded to the material's uniform block. Only one display list (the opaque sky list) is switched for it.
- Graphics: the hanging blue strings are built with their segments 80 units apart (GameCube 50).
- Gameplay: in the opening cut scene Link looks up at frame 313 (GameCube 320), thunder particles flash around Link at five fixed frames, and from frame 450 the camera slowly zooms in. All cut scenes also count down a camera blur.
- Gameplay: during the transformation (action 6) the body parts are not rolled around their own axis.
- Structure: the transformation and the defeat no longer go through the HIO phase variable; Execute sets the phase directly. The defeat event bit is now set when the final explosion starts (GameCube: when the stage changes), and the stage change passes all of its arguments explicitly.
- Structure: Execute no longer calls move. The function at 0208857C (the map calls it shape_calc) is move, with size_set, damage_check, shape_calc and the tail's free swing (part_control_0Z) inlined. The rope part of action_s is a separate function (020859AC) that runs after the body. Execute inlines demo_camera and last_himo_control; Draw inlines water0_disp, room_disp, obj_disp and the draw functions of all three phases. The debug reports of the cut-scene counter and mode are still present.
- Structure: arm and tail part sizes are divided directly instead of multiplied by a reciprocal, and the tail swing uses fused multiply-adds (tiny float differences). The final hit also clears a play-object float when the music stops, as in d_a_bgn2. The GameCube's call to clear status bit 0, which had no effect, is gone.
- Structure: bgn_class is 0x15768 bytes (GameCube 0xCC94): an HD word after the enemy base, 0xB0-byte model packets, 0x1C8-byte tev structures, 0x188-byte line materials and three extra packets. When the tev structures are copied, three 0x40-byte HD blocks are skipped.

### m_Do_ext#01 — heap and model helpers

**Structure / Graphics:** HD model drawing saves and restores the shared view and projection state around each model draw, obtains camera data through the resource manager, and wraps sead-backed heaps behind JKR-compatible factory functions. Alignment and heap validation remain explicit. The HD factory adapters discard two supplied boolean arguments. View-matrix copies preserve the local reference model’s signaling-NaN bit patterns; this verification does not establish console NaN behavior. Model resources and the shared view array are treated as separate objects.

# Collision attack utilities (`d_cc_uty`)

The retained HD unit spans `02518B28` through `02519814`: player cut-bit lookup, normal attack sound lookup, two defense-sound wrappers, attack power classification, hit handling, and the TU initializer. The GameCube critical-sound lookup has no separate retained entry; its selection is inlined into hit handling. The following function begins a different unit, and is excluded.

The attack record retains the GameCube field positions and 0x1C-byte layout. HD reads player state through the play singleton, with player at +0x5B2C. Actor positions, speed, room, health and steal-item fields use HD offsets. Attack classification still distinguishes water, wind, fire, hammer, sword, bomb, rope, boomerang, arrow and the remaining actor-specific cases. Normal sound lookup now asserts on a null collision-info result and returns the default sound afterward.

Hit handling has an additional damage subtraction when the player's cut type is turn/roll and the player byte at +0x69E8 is nonzero; this condition is absent from the supplied GameCube source. The particle path goes through the HD particle controller, retains both particle IDs and their arguments, and supplies complete six-byte angle and twelve-byte scale objects. Health and pause values retain signed-byte wrap semantics. The initializer constructs the usual TU-local SDK/global support objects.

All seven entries match the recompiled reference on 10,000 generated inputs at each of seeds 1 and 7, with 200/200 basic blocks reached at seed 1 and 197/200 at seed 7. Seed 7 misses the cut-type-10 hit-sound assignment and its tail (`02518FCC`, blocks27–28), and the cut-type-16 dispatch edge (`02518FFC`, block32): these sparse combinations are reached in seed 1. A nine-change mutation sample covering all seven entries produced observable return, memory or call differences for every change. No mutation survivors remain; combined coverage is full. This is isolated-function generated-input verification with mocked callees and clobbering enabled; there are no recorded game calls. Optional broader research is deferred.

### m_Do_MemCardRWmng — unmatched GameCube card manager

The GameCube unit implements twelve save/card operations: store, restore, header/banner/icon construction, card-status setup/checking, and checksum calculation/testing/writing for game and picture data. The store/restore paths use GameCube CARDRead/CARDWrite and maintain redundant card save copies and picture slots.

The bounded HD inventory identifies no native entry for this translation unit. The current names and GameCube-to-HD mapping tables have no matching rows, and coverage metadata records twelve GameCube functions with zero matches. The HD import-name inventory contains none of CARDRead, CARDWrite, CARDGetStatus, CARDSetStatus, or CARDGetSerialNo. These observations support treating the old card API paths as unavailable in this mapping; they do not prove that every checksum or save behavior was removed from the HD game.

No arbitrary static initializer was assigned to the unit. The inspected boundary before HD audio contains the standard-create-request initializer followed directly by audio helpers and the mapped audio entry. Consequently no implementation, generated comparison, coverage, mutation result, or ready-for-merge claim is made for this unit.

The shortest remaining path is concrete native attribution or a retained address range, followed by the ordinary implementation and two-seed verification gates. Broad discovery and save/resource-lifecycle research are deferred. This inventory remains explicitly unmatched and uncertain.

### d_detect (quake and light detection)

**Structure:** HD retains a sixteen-byte quake place and a twenty-byte detector, follows the player through the game-state singleton, and tests tag-light process ID 469. Light detection transforms positions into the tag's box or cone coordinates; quake and attention checks retain the recognizable GameCube flow. Position copies preserve the reference's raw float words. The adjacent support-object initializer is included with attribution inferred from adjacency. No gameplay behavior change was identified.

### d_item_data

d_item_data: HD retains four effect table queries at 02551B90/02551BBC/02551C18/02551C48 and the large item-record scatter initializer at 02551CA4. The interleaved u16 effect table is at 101E8A74, 129 entries, sentinel 8466; getters preserve the assertion call and reload. Range checks retain the native full-register comparison. The initializer copies captured u32 fields in native order and performs the five common startup calls. GC identifies four methods plus one sinit; HD initializer ends 02552840. Neighboring startup entry 02552844 is unattributed and excluded; joint-hit begins 025528D8, owned by the root. No absent GC methods are fabricated.

### m_Do_dvd_thread — resource jobs and thread control

**Structure:** HD runs the command queue through a sead thread and uses the platform mutex APIs. The parameter object grows from 0x48 to 0x84 bytes, moving the command list and mutex while retaining the base command and callback layouts. Queue processing drains available commands rather than following the GameCube message-queue waiting loop.

**HD-only:** Main-RAM jobs retain a bounded path buffer and resolve a path string during execution; GameCube jobs keep a DVD entry number. The HD command grows to 0x230 bytes. Its completion flag, selected heap, direction and data-size reporting retain their roles. Thread and string-wrapper helpers are included by adjacency and call relationships; the startup registration is probably associated with this unit. Verification covers the local reference-call contract, without claiming native thread scheduling or resource-lifetime behavior.

### d_com_static — shared static actor interfaces (2026-10-04)

Structure: HD retains a separate cluster for actor-wide state and small interfaces, including ship bobbing, light-angle coordination, Iball registration, pig digging, item joint transforms and global initialization. The complete24-entry cluster includes unnamed methods, its smoke callback cleanup and small getters after the initializer; eight matched names were only locators.

Gameplay: The Medli flight limit remains450 frames. Pig digging checks the switch before spawning, applies item status, and either sets the switch or deletes the tag; this follows the later GameCube version. Ship bobbing retains the signed-angle threshold and sine-table lookup.

Graphics: HD retains the shared light frame counters and the standing-item joint cases, with expanded HD object offsets and a larger global TEV template layout. Removed: the two GameCube Tingle figure increment methods are absent from this HD cluster. No new gameplay change was established for the retained interfaces.

### d_bg_w_sv — verified, 2026-10-04

Structure: HD retains saved-vertex copying, horizontal position correction and full translation using triangle coordinates, retrying the three vertex orderings when a projection is degenerate. The constructor initializes both correction flags and the saved-vertex pointer in its 196-byte object. The virtual matrix-correction callback is empty. The adjacent math initializer follows the header-generated pattern; exact header attribution is qualified. No gameplay change was identified.

### d_wind_arrow

- Gameplay: The HD arrow retains the GameCube visibility conditions based on animation speed, ship riding, and event or conducting state. It follows the wind direction, keeps the same forty-unit vertical and minus-250 forward offset and 0.85 scale, and uses wind power to advance its texture animation.
- Graphics: HD obtains player state and display-list pointers through its game accessor and uses HD object offsets for matrix, model-data, and animation storage. The monotone and normal display-list branches and restoration order are preserved. The extracted cache-flush thunk resolves to the cache-store import in the reference generator; verification checks that observed import call, not native cache timing.
- Structure: Constructors, destructors, solid-heap setup and adjustment, create/delete wrappers, vtable helpers and the inferred trailing initializer are included. The model-creation target ignores the incoming third integer flag in its extracted body, so that mutation survivor is explicitly scoped to the actual callee contract. Native renderer and resource lifetime behavior remain outside the isolated verification gate.


### d_timer — frame countdown and HUD publication

Gameplay/state: HD stores the limit and remaining count in32-bit frame counters, converts the wrapped remaining counter to milliseconds by multiplying by1000 and dividing by30, and decrements it once during an active, unpaused execution. Start and stock-start use signed16-bit delay counts; stop/restart retain the pause-owner checks, while end/delete advance the state machine. Ship-race and volcano thresholds trigger sounds from their finite tables.

Structure: timer creation reads its message append directly and publishes remaining/limit/mode/timer-pointer values through the play singleton. Stock creation restores those values and starts with a short delay. Visibility and icon selection write shared HUD bytes; the verified HD lifecycle does not construct the GameCube TimerScreen heap/pane object. The local draw hooks are empty or return success. The unit ends with a compiler initializer and additional HUD float constants, with exact attribution qualified by adjacency. This inventory covers30 genuineHD functions025C5770..025C628C; unmatched GameCube GUI methods are not claimed as HD counterparts.

### c_m3d — verified, 2026-10-04

Structure: This bounded unit covers 3D distance, projection, bounds inclusion, plane and segment intersections, and sphere, cylinder, capsule and triangle collisions. HD geometry layouts and its eight-coordinate-plus-stack-double inclusion ABI are preserved. Cylinder intersection orders candidates against the line start; capsule/sphere fallback uses the HD half-scale constant. Triangle intersection retains the repeated first-edge behavior. These are observed HD behaviors; no broad GameCube-to-HD fix claim is made. Adjacent initializer attribution remains qualified.

### d_salvage — verified, 2026-10-04

Structure: HD retains two byte counters and 160 records of 56 bytes, split into 128 sea entries and 32 room entries. Registration, parameter validation, state gating, emitter invalidation, counters and XZ distance preserve the supplied GameCube behavior with HD actor offsets. Strict greater-than counter boundaries and the draw-mode getter's current-alpha byte read are preserved.

Structure: Vector result wrappers use caller-provided 12-byte storage with a 12-byte allocation fallback. The adjacent compiler initializer is included with its precise generating header qualified.

### f_ap_game — game-system scheduler and startup

**Structure:** The process-management callbacks and initial logo-scene request retain their GameCube roles. HD rearranges and packs the diagnostic-settings object, with its virtual metadata stored at the end rather than the front.

**Removed:** HostIO child creation is a constant-zero stub in HD, so registering the game-system diagnostic panel has no effect.

### d_letter — letter state (2026-10-04)

Gameplay: The HD letter interface retains all ten GameCube state operations: unsent, sent, stocked and read queries/updates, delivery of sent letters, automatic stocking of unsent letters and the delivery-state query. No gameplay change was established. Structure: State access goes through the HD game-information singleton and event registers. The adjacent compiler initializer is included in verification; its translation-unit attribution remains based on adjacency.

### d_s_actor_data_mng

- Structure: HD actor data management uses a 144-byte character table embedded in a 156-byte manager. Loading relocates the name arrays and tagged data pointers, checks the table dimensions and caches twenty-six column indexes. Tagged lookup returns each section size and pointer; name lookup continues through duplicate names until its second criterion matches. Two constructors and deleting destructors are retained. An adjacent HD singleton initializer is attributed by location.

### d_cc_mass_s — mass collision processing

Structure: HD preserves the collision mass arrays, priorities, divide-area preparation, attack/co/area hits, camera capsule results and generated initialization. HD Chk guards optional hit-info before clearing it, asserts/skips a null mass object before getters, and skips attack checks when GetGObjInf returns null while retaining co checks. Layout remains0x1a0 for manager and0x18 for entries; camera selection uses the same resultCam bits2/8 as GC. No further gameplay difference identified.

### d_material (2026-10-04)

**Graphics / Structure:** HD loads two replacement material resources and populates two reusable material objects. The GameCube version initializes texture animation and copies materials with backup/restore around drawing. HD rendering instead sets a model flag around the update or entry call and preserves the two draw buffers. Five wrapper variants include an alternate invisible-model entry. Heap creation allocates the replacement storage with a named system heap; removal clears replacement pointers and destroys it.

**Qualification:** Eight entries from setup through removal are verified. The following vector/float/bool initializer is attributed to the neighboring meter unit and excluded. Resource-manager stability follows the established play-time singleton contract. Heap reload equivalence is scoped to material initialization without reentrant create/remove during SDK material population.

### d_water_mark

- Gameplay: The HD effect keeps the GameCube ground-projected water marks, ten-effect count limit, and forty-step delayed-footprint ring with a twenty-step activation offset. It updates the projected matrix on moving ground and deletes effects when the animation stops or the ground projection fails.
- Fix: The HD matrix routine explicitly returns failure when the background system supplies no triangle plane; the corresponding supplied GameCube routine dereferences the returned plane directly.
- Graphics: The ground probe remains ten units above the effect, with a 0.1-unit matrix offset and a fifty-times-scale clip radius. HD uses its game/background accessors and object offsets for model and animation data. Resource allocation fallback and display-list calls retain their observed order.
- Structure: The complete contiguous range includes lifecycle wrappers, the inferred initializer for the shared ground-check object, and its local destructor/vtable helpers. Verification covers caller memory and call effects with external SDK/render/background code mocked; native rendering and resource lifetime are outside that gate.


### d_level_se — level sound callback lifecycle

Structure: HD retains the GameCube sound flag priority: bit8 suppresses playback, bit1 selects the volume form, bit4 selects volume plus signed reverb, and the remaining case uses position alone. The callback still returns success. Delete removes the sound object by its position and then destroys the environment-process base for a nonnull object. HD creation constructs that base and installs the derived vtable before returning completion; the GameCube create body simply returns completion. The image method table directly identifies all four callbacks, including IsDelete after the compiler initializer. Exact initializer attribution follows adjacency and the repeated per-unit motif.

### m_Do_DVDError and m_Do_Reset — retained startup entries

Removed/Structure: No GameCube DVD watcher or reset APIs have been reconstructed in these units. Each retained entry performs standard runtime startup registration. TU attribution inferred from HD adjacency (after the m_Do_audio companions 20F0/20F8/2100; after the printf initializer 28C8, before vibration 29F0); not proven by symbols or strings. These filing labels are accepted with that qualification; later symbol evidence can change the filenames.

### d_kankyo_demo — demo environment lights

Behavior: demo point-light registration retains the two light groups and special type103 parameters. HD entry additionally accepts an initial update delay. Execution consumes that delay before copying position and smoothing a randomized light value; GameCube execution only copied position. All three HD operations first obtain the lazily initialized environment singleton, including guarded-out calls. The retained unit also includes generated initialization.

### d_ky_thunder

- Graphics: HD passes a zero range to the signed rotation randomizer where the supplied GameCube source uses 4,000. It draws from the current camera position plus a stored offset, while keeping a separately computed absolute sound position; the GameCube draw uses the stored absolute model position directly. The horizontal and vertical model scales share the twenty-plus-random-sixty size in HD, with weather-state scaling and a possible horizontal sign flip; the GameCube source calculates the two scale components from different ranges.
- Structure: The solid-heap request grows from the supplied GameCube 0x4A0 to HD 0x3804 and adds the observed parent-heap and game-heap fallback sequence. SDK model, animation, camera and display-list offsets follow the HD layout. The matcher labels the heap-creation helper as Delete; the actual cleanup entry is separately identified and verified.
- Graphics: Wind-independent random side placement, the 120,000 spread range, 100,000 forward distance, 2,000 vertical range and 0.3 sound-chance threshold retain their GameCube roles. Heap adjustment uses the cache-store import observed by the reference generator. Verification covers caller memory and call effects with external SDK/render/audio code mocked; native rendering, audio and resource lifetime remain outside that gate.

### d_cam_param — camera setup and numeric helpers

**Structure / HD-only:** HD adds explicit allocating constructors and a halfword of C-stick state. Background-check settings move by four bytes to accommodate a virtual-table pointer while retaining the GameCube defaults. Camera setup grows to 0x168 bytes and embeds the C-stick and background-check helpers alongside additional tuning fields.

**Gameplay:** Style lookup, radius limits, lock-on interpolation, latitude clamping, randomized banking and vector rotations retain the GameCube algorithms in this verified range. No gameplay change is claimed. The adjacent startup initializer is probably associated with this unit; three following standalone initializers remain outside the attributed range. Angle helper objects use their actual two-byte payloads, and matrix copies match the local reference model rather than establishing console NaN behavior.

### d_wpot_water — water-pot environmental process (2026-10-04)

Gameplay: HD retains the50-point expanding ground/lava search, lava-rock and impact-object spawning, and the three water particles. Ground failure or missing persistent emitter returns creation failure. Structure: HD cleanup explicitly destroys the draw packet and process base; emitter lifetime checks read its flags and two particle counts. Graphics: The former GX state callback is replaced by an HD draw-state object and SDK state calls; translation, scaling and view-matrix concatenation remain. The full14-entry inventory includes image-confirmed callback vtable stubs and the global initializer; the two preceding compiler stubs are included with adjacent attribution qualified. Mutation50 was initially open; the normal shipped SDK no-observation contract was admitted on 2026-10-05. Its equivalence remains assumed-not-proven transitively and excludes custom/external heap observers.

### d_jnt_hit — joint collision geometry

Structure: HD retains joint-hit allocation, cylinder/sphere contact offsets, buffer selection and nearest-joint search. The model matrix-buffer layout is accessed directly and its update flag is set when a joint matrix is selected. The runtime constructor also supports allocating a 32-byte object when passed null. The debug parameter object retains 44 bytes but places its virtual table after its payload in the native layout. A generated initializer follows the methods; attribution is inferred from adjacency. No gameplay change was established.

### d_seafightgame

d_seafightgame: retained native entries are checkPutShip025BB0E0, put_ship025BB198, init025BB358, attack025BB470 and TU initializer025BB568 ending025BB5F8. HD retains the 8x8 grid, four 15-byte ship records and byte counters at7C..7E plus32-bit dead counter80. Placement preserves native float-to-integer narrowing, repeated random draws/check and store order. HD attack validates cell-derived ship index<4 and calls assertion before returning-1 for invalid ship tags, unlike the supplied GC source. GC getNearEnemy has no compiled TU symbol or attributable HD entry, so no implementation is fabricated. Preceding025BB04C startup belongs the previous region; following025BB5FC is mapped d_shop.

### d_kankyo_data

Structure: the retained environment-data APIs still return the default palette, selection, environment, skybox and three schedule tables. The fog routine copies ten adjustment values and uses the same two fog tables as GameCube, obtaining the environment singleton for each copied value in HD. The admitted scope also includes generated initialization. Other static table contents were not compared; the separate preceding registration cluster remains unattributed. No further gameplay difference identified in these executable APIs.

### d_a_yougan — lava bubble actor (removed in HD) (2026-10-05)

**Structure (actor removed):** d_a_yougan ("yougan" = lava; the GameCube lava-bubble actor, present in zeldaret/tww as d_a_yougan) does not exist in HD. cking.rpx contains no actor code for it and no strings: neither the archive name "Yougan" nor the HIO name 溶岩の気泡 (in Shift-JIS, UTF-8, EUC-JP or UTF-16) is in the image, and the execute's distinctive constant 17.591f appears nowhere. Its process/profile slot (HD process 0x018A, profile 101D2AA8) is reused by Tpota (daTpota_c::_create 024C9D44, _execute 024C9FD0). The only remains are the resource file names yougan_awa.bck/.btk/.bmd in two resource-name tables (1009B59C.., 100DE53C..), referenced by no code; the model/texture data may still exist in the HD archives. Lava and magma in HD are handled by other units (e.g. d_a_magma, merged). No functions, so no verification credit. Checked 2026-10-05 (evidence in the d_a_yougan verification archive).

### d_bg_w (dBgW: per-model collision mesh queries — wall correction, roof/water-surface/sphere checks) — verified, 2026-10-05
- Structure: dBgW keeps its GameCube size (0xBC) and member offsets; the element tables it walks have HD strides (triangle planes 0x18, chain links 8, group boxes 0x20, tree boxes 0x1C), probably because the HD classes carry their vtable pointer at the end. The bounding-box tests are calls into c_m3d/c_m3d_g_aab, as on GameCube.
- Structure: the debug assertions of the GameCube headers are compiled in (polygon/info index ranges, "index <= m_tbl_size", non-negative poly/bg index, NaN checks on the corrected position).
- Structure: the WallCrrPos family (wall correction for dBgS_CrrPos) has no body in this HD unit (probably unreferenced and stripped); dBgW has no destructor entry in its vtable.
- Gameplay: none found; the wall-correction, roof, water-surface and sphere queries follow the GameCube logic step by step.

### d_a_tn (Darknut) — verified, 2026-10-04
- Gameplay: the "fell out of the level" rule changed. On the GameCube a Darknut that drops more than 4000 units below its home position is simply deleted. In HD the check only applies to Darknuts that have a death switch, the distance is 5000 units, and before deleting itself the Darknut sets its death switch, records itself as defeated in the save data and also deletes the weapon it carries. Probably so that a Darknut knocked into a pit counts as beaten (doors or chests waiting on its switch still open) instead of silently vanishing.
- Gameplay: the "special stage" flag set at creation (probably the arena-fight search mode) also covers the HD stage kazeMB, besides the GameCube's ITest63 and GanonJ.
- HD-only: at the end of creation the Darknut sets the status flag that GameCube Darknuts only use while waiting for their spawn switch, together with a short (two-frame) timer that clears it again, when it is created in stage M_Dai room 17 or in stage Hyroom; the GameCube has no such stage checks. Probably a per-room fix for how these Darknuts are first shown or targeted.
- Graphics: armour colours are converted through a separate helper that applies a 2.2 power curve and clamps to 0..1 (probably gamma-correct tinting for HD rendering); the body material is looked up by name in the model's material table instead of by fixed index.
- Structure: the small "bomb nearby?" check used by several actions is a separate function in HD (024BAF44); the debug HIO/pad pause gate around the per-frame timer updates in Execute is gone (timers always tick). Actor size 0x1C5C; fields shift against the GameCube layout throughout.

### d_ovlp_fade2

Structure: the HD snapshot fade process occupies a 0x2A8-byte object and dispatches its four transition phases through stored member-function descriptors. It accesses play and rendering services through singleton objects. Graphics: the snapshot is wrapped in a 0x198-byte rendering object and transformed with the HD matrix stack before submission; these rendering interfaces differ from the GameCube bindings. Gameplay: the angle-crossing, byte-timer and scene-start progression is retained in the verified HD behavior; no gameplay change is established here. HD-only: unnamed lifecycle wrappers and adjacent compiler initialization are included; initializer attribution is probably this translation unit based on adjacency.

### d_vibration — 2026-10-05

**Gameplay / Structure:** Shock and quake pattern state, frame counters, pause, reset and motor-call ordering remain explicit. The HD object occupies132 bytes and initialization/destruction issue two motor-stop calls before resetting state. These are isolated call-contract observations; the hardware rumble behavior was not tested.

**HD-only / Structure:** Pattern output supports an HD controller mode using18 bytes: a length halfword followed by eight packed amplitude halfwords expanded from a32-bit pattern. Its expanded duration is capped at120. The alternate mode retains the shorter legacy format and clears the additional halfwords. The controller-mode meaning beyond its zero/nonzero selector is not asserted. Actual HD helper arguments also differ from the imported GameCube long signatures.

**Verification scope:** Seventeen functions include the nearby compiler initializer by adjacency attribution. Both final10k seeds pass; two remaining motor-stop branches require a mode greater than3 although construction only combines bits1 and2.

d_envse full retained HD TU0252FCB4..025303B3 contains8 entries: Draw,nearPath,execute,Executewrapper,IsDelete,Delete,Create and adjacency-attributedinitializer30320. HDCreate now calls kankyo base setup and installs1004CC8Cvtable; Delete calls base cleanup afteraudio deletion. The HDshore counter is a32bitwordFC withoverlapping16bit waveframeFE, selectedpathargument100; startshore requirescounterzero andactualnearest-distancefound (GCdescription omits extra distanceguard). Path helper handles16bytepoints,28byteLineobject and12byteoutputs; line'sactualvtable at24 preserved, no undersizedLocal or live-in overrides. Execute retains signedroom/camera selection, indirectroom soundlookup and type0..4audio paths. All8x10000inputsseeds1/7PASS81/81coverage. Generatedstage-query domain uses strictnullablepointer records, read-only room-vtable/sound/pathrecords markedstable (intendedstage contract); mutableactor/playclobber retained. Final52 bounded sample33returningdifferences4sourceEq15compile-invalid0OPEN, no finalcrashes/timeouts. Eq line-tail variants havefinite actualsegment/vector-math frontier reading only first24 of28byteLine with no vtable use/escape/destructor; shiftedlastbyte staysunused32byte reservedLocalstacktail. OtherEq privatecapture initializations and readonlyconstantload ordering. Initial nonstrictpointer/clobber runsSIGINT130 diagnostics excluded; functionformat corrected initialDraw mutationparser includingstructdeclaration, superseded53sample excluded; ignoredfretcsv removed (normalgeneratedfloats used). Externalcall boundary and initializerattribution remainqualified; optionalresearch deferred.


### d_s_logo

- Structure: The verified HD startup handoff performs archive-relative resource lookup and installs resource pairs through the HD game layout. Its much shorter handoff differs from the supplied GameCube cleanup entry, which also destroys logo images. The verified contiguous HD range does not include the GameCube logo drawing and progressive-selection execution flow.
- Structure: Startup phases initialize audio singletons, load resources and then poll an additional platform readiness state. The timing phase obtains its clock word through the system-information import rather than the GameCube bus-clock macro. The archive and singleton accessors follow the HD layout.
- Qualification: Verification covers caller memory and call effects with external resource, SDK and audio callbacks mocked. Two actual constructor writes to singleton globals are represented explicitly to test subsequent reloads. Native boot presentation and resource lifetime remain outside this gate; the adjacent static initializer attribution is inferred.

### d_kyeff2

Structure: the HD second weather-effect process writes its translation-unit vtable at object offset 0xB4 and retains separate draw, movement, execution, creation and deletion wrappers. Graphics: draw delegates to the second weather-rendering routine and movement delegates to the cloud update; this verified unit adds no further rendering logic. Gameplay: no gameplay change is established by these wrappers. HD-only: the adjacent compiler initialization is included, probably belonging to this translation unit by placement; its ownership remains qualified.


### d_chain — HD packet and GPU resource pipeline

GC builds a point array and renders each physical link through GX display lists. HD retains the point/scale/tev inputs and alternating quarter-turn geometry, but owns a B32C-byte packet object with 256 link packets, an A8C8 shader packet, paired GPU resources at AC2C, native index data at B0EC, and shape resources at B104/B29C. Constructor/destructor resource work and the separate render-pass entry are new; create now invokes the virtual destructor when point allocation fails. Color conversion and vector/matrix temporaries preserve actual callee sizes; native GX2 calls use canonical imports and indirect calls preserve all checked live-in arguments.

Full16 functions pass 10000 inputs at seeds1/7;272/290 blocks. The18 constructor gaps are null-interior allocation fallbacks requiring invalid-object address wrap; other15 functions cover174/174 blocks. Geometry fixtures preserve positive segment lengths and finite owner counts. Mutation SAMPLE171selected/110compiled gives96comparison detections,11source equivalences,2timeouts/1runtimefault excluded,61compile-invalid excluded. Actual shifted clears and color-copy survivors are detected by better fixtures. Source equivalence17 assumes a valid allocatedobject; equivalence25 assumes heap vertex buffers disjoint from staticmesh data. This remains isolated mocked-callee verification, with no nativeSDK/resource-lifetime proof claim. optional research deferred.


### m_Do_ext#02 — model factories and animation buffers

Structure: The HD model factory allocates a 0x144-byte object, supplies model-data and flag arguments, and selects a fixed final constructor mode. The animation wrappers use expanded 0x8C- and 0xB8-byte buffers and resource descriptor setup instead of the GameCube direct transform storage. Resource conversion preserves cached reference pointers across callbacks, and old-frame counters use reordered offsets.

Qualification: Verification keeps resource descriptor counts bounded and immutable while allowing other callback effects. Arbitrary callbacks that change these counts remain outside the verified domain. A mutation traversal timeout is retained as a diagnostic and receives no detection credit.

### d_s_title

Structure: this HD title unit reduces to the execution selector and a constant lifecycle query. The selector reads reset and service state through singleton pointers and sends a scene-change request or selects the opening scene. Gameplay: the verified HD selector routes reset state and the two special service status values to separate destinations; no GameCube behavior change is established by this local reconstruction. Neighboring logo-scene and actor-data initialization functions are separate units.


### c_bg_w — background collision world

Structure: HD obtains the current heap through an accessor before checking its type; the GameCube implementation reads the static current-heap pointer directly. The ground-wall filter in HD branches on a normal below the threshold, whereas the GameCube source uses an ordered greater-or-equal test. A NaN normal consequently reaches the common ground-test call in HD; its unordered height checks reject the result. No gameplay effect is established from this difference.

Qualification: The full37-function HD inventory ends at0200B568. The last local function at0200B4D8 performs static startup work; adjacent constructors starting0200B56C belong to c_cc_d. Verification uses bounded indices and immutable mesh metadata. Both final seeds pass11000 inputs per function with378/379 blocks; the sole gap is the null-mesh assertion path. Raw triangle-point copies preserve signaling-NaN bits. Mutation timeouts and malformed direct-call targets remain excluded, with no universal equivalence claim for changed loop counts or links.

### d_bg_w_hf — height-field collision (2026-10-05)

Structure: HD retains the height-field grid, alternating triangle normals, triangle classification and recursive block/group bounds; the reconstructed object is 0xD4 bytes. Native virtual callbacks rebuild planes and bounds when height data changes. No material gameplay difference was established against the GameCube source guidance. The adjacent static initializer is probably part of this TU; the empty position-correction virtual is confirmed by its HD vtable.


### d_throwstone — demo activity gates and model transforms

HD records the demo-update result in byte3E8 and skips scale/matrix updates when inactive; execution returns1, while GC updated unconditionally and returned0. HD drawing additionally requires the activebyte before querying eventbit0310. Creation transfers scale to the model and applies ZXYrotation rather than the GCY-only scaledmatrix. Resource/model lifecycle callbacks, the vtable destructor, and adjacent qualified initializers complete the10-functionHD inventory. The HD model factory consumes data+r4; its legacy third callsite flags register is overwritten before use by the factory.

Full10x10000 inputs seeds1/7PASS30/30coverage. Mutation SAMPLE95selected/56compiled:53comparison detections,3sourceequivalences,39compile-invalid excluded; no crashes/timeouts. A canonical runtime save singleton detects the actual one-byte shifted-pointer survivor. Eight-byte resource-name Local sizes and all argument checks retained. isolatedmocked-callee scope,optionalresearchdeferred.


### d_s_room — room archive and particle phases

Structure: HD stores the formatted room archive name in the scene object and loads its resources without the GameCube room heap readiness check. Particle selection moves from the room-data phase to the next phase, which resolves archive-relative particle data through the HD resource manager instead of waiting for and deleting a scene command. The demo archive name uses bounded formatting.

Removed: The verified HD deletion path omits the GameCube salvage reset and room-heap free-all calls. Particle removal can delay deletion by returning failure. The final creation phase does not call the GameCube map-image setup helper. Gameplay: the existing pearl and Dragon Roost event updates remain in the reconstructed flow.

Qualification: Verification covers the remaining eleven room-scene functions; two opening wrappers were already verified with the play-scene unit. The adjacent initializer attribution is inferred. External SDK/resource callbacks are mocked, apart from the exact one-instruction SafeString no-op needed to exercise string equality. Verification uses a distinct allocated game singleton and immutable literal strings; native resource lifetime remains deferred.

### d_kyeff

HD Create constructs the environment-process base and installs its vtable; Delete calls the base destructor before weather shutdown. On title stage Name, HD fixes current time to150.0 instead of deriving it from OS calendar hours. Its Execute body includes stage detection and menu blending; the menu schedule stops at the first matching entry. Palette entries grow from GC0x24 to HD0x38 and include four interpolated float fields at24..30 plus a float-derived alpha channel at34. Menu sea/mist channels are blended from their palette bytes1B..20 instead of GC's cloud-color*0.9 derivation. Native bounded SafeString wrappers replace raw strcmp; rain250/thunder1-or10 stage rules remain.

### d_boss_magma

The retained HD TU contains eight entries, from `024F6098` through `024F6DEC` exclusive: the boss-search callback, ball calculation, update, setup, a TU initializer, and three cleanup companions. The callback is identified by setup passing its address to process search and by its actor-name test. The preceding no-op at `024F6094` is outside this unit. The next entry at `024F6DEC` constructs another unit's debug object and is excluded.

Calculation preserves the boss-height shutdown, proximity response, wave restart and sine-table motion. The HD shutdown branch tests the negation of `height > threshold`, including unordered NaNs. Position, scale, base height, wave, vtable and wave increment now reside at +0xD50, +0xD5C, +0xD60, +0xD64, +0xDCC and +0xDD0. Setup retains the circular random placement, randomized scale, boss-dependent height and randomized phase/speed. Boss fields move to +0x70C0, +0x70C8 and +0x70CC. Update additionally calls an HD object-update helper after the two matrix operations.

The two 964-byte cleanup entries at `024F6664` and `024F6A28` have identical bodies in the reference, despite the matcher naming only the latter as the boss destructor. Both release expanded mesh and nested array ownership; they are included rather than represented by the GameCube class's small empty destructor. The 96-byte member cleanup at `024F6604` is referenced by their array cleanup call. Heap cleanup retains the pointer argument passed to the heap getter and then reloads the released pointer after that call, because mocks and real callees may change memory.

### d_file_error — shared screen helpers (2026-10-05)

Structure: the HD pane factory retains the GameCube MyScreen picture-block specialization: picture blocks create the custom picture type, and other blocks delegate to the generic screen factory. The matrix wrapper dispatches both stored coordinates through the screen virtual method. This verification covers three shared helpers, including a probably associated inline picture destructor; neighbouring HUD display-list methods and initialization are excluded. No gameplay difference or complete file-error UI reconstruction is claimed.


### m_Do_ext#03 — old-frame animation blending

Structure: HD converts weighted matrix records into normalized basis and quaternion transforms, blends them with prior frame storage, then sends them through animation callbacks and SDK table dispatch. The expanded animation buffers retain separate rotation, scale and translation dirty flags. Graphics: the reconstructed math preserves the paired-single reciprocal-square-root refinement and fused arithmetic order used by the local reference.

Qualification: Verification covers fixed-count animation evaluation with callbacks that do not resize descriptor arrays. Arbitrary resizing remains untested. A mutation that writes a fifth component beyond the real four-component quaternion is retained as an invalid object-range diagnostic, with no equivalence claim.

### d_s_open — opening scene

Structure: HD replaces the GameCube opening-resource phases with the native UI singleton and a global phase byte. Creation starts the UI thread in opening mode and waits for readiness. Execution retains the timer and input driven opening/title transitions. Deletion performs the HD virtual cleanup calls; scene construction clears the final storage and installs the HD tables.

### c_damagereaction — damage response

Structure: HD reserves 21 elements in each joint array while animation still visits 20, shifting the later damage-state storage. Gameplay: transformed movement and collision movement omit the vertical component; collision movement applies horizontally only above the retained mode threshold. One action clears its movement offset, and recovery resets the state to zero where GameCube used two. Fix: the initial speed copy preserves floating-point payload bits, while later arithmetic retains the native single-precision fused expressions and unordered comparisons. The reconstruction includes both large damage handlers and the complete native object storage used by their helpers.

### d_wpillar (2026-10-05)

**Graphics / Structure:** The water pillar retains joint-height deformation and animation-driven deletion. HD marks animation-matrix state before the joint callback, uses the HD model layout, and expands draw/create material handling into per-material normalized color transforms. Bomb pillars synchronize the player's two animation controls. Heap naming and embedded animation initialization also changed. Cleanup includes embedded animation and environment-object destruction. Creation retains ripple/splash effects and ground-derived lighting.

**Qualification:** Thirteen contiguous functions are verified; the next generic initializer is attributed to waterpot and excluded. The bomb color temporary overlaps a scratch float in the HD compiler's stack layout; verification preserves that alias. NaN selection is modeled against the recompiled reference, including invalid-joint assertion inputs; this is not a console floating-point claim. Model-pointer reload equivalence assumes distinct factory allocation and initialization without replacement of the outer model field.

### d_s_menu

d_s_menu retains eight HD entries: Draw025AC790, Execute025AC798, IsDelete025AD550, Delete025AD558, phase1025AD5E0, phase2025AD680, Create025AD8AC and initializer025AD90C. Native profile and phase table establish ownership. HD Draw returns1 and omits GC font rendering; Execute retains Japanese Demo/room-tag substring paths, pad navigation and scene dispatch. Phase2 preserves native endian callbacks, object cleanup and tick no-op. Execute uses a real176-byte native frame including8-byte SafeString temporaries; callbacks pass target-read registers. Neighboring SafeString support is a dependency, not an owned entry. Absent GC methods are not fabricated.

### d_s_name

- Graphics: HD initializes a 1280-by-720 viewport and scissor. It retains the supplied GameCube camera position, target, far distance and sixty-degree field of view while using HD camera and renderer layouts. Projection setup uses the observed HD aspect adjustment.
- Structure: The scene heap request grows from the supplied GameCube 0x68000 to HD 0xB0000. HD delegates name entry, file selection and formatting to separate state controllers and compares their state descriptors; the supplied GameCube scene implements much of its pane and button flow directly. HD has four startup phase handlers with additional readiness checks.
- Structure: The actual method table identifies the create, delete, execute, IsDelete and draw wrappers separately from the startup handlers; a previous boundary notice had mislabeled two handlers. The scene uses the system-information clock import for timing. The matcher’s later NameInMain address lies outside this scene range and is not included as a scene implementation claim.
- Qualification: Caller memory and call effects are verified with external SDK, UI, resource and audio code mocked. Six constructor null-subobject fallback blocks remain untested for arbitrary wrapping addresses; a proper full scene object cannot reach them. Native presentation and resource lifetime remain outside the gate; adjacent static-initializer attribution is inferred.

### d_lib — game utilities and stick-repeat control (2026-10-05)

Gameplay: Circle-path movement, water-height selection, wave rotation, animation switching, path following, stage transitions, event-bit messages and circle/fan tests retain their GC logic. HD animation setters also update an optional second morph object from a real ninth stack argument. Stick repeat uses the HD40-byte layout with its vtable at24hex, rather than assuming the GC front-vtable offsets. Structure: The full39-entry HD inventory includes virtual getters/init/repeat methods, global initialization, an HD ground-position helper and qualified adjacent compiler entries. Graphics: Axis debugging retains matrix/vector computations and lazy color initialization; fan debugging is empty in HD. Quaternion helpers retain triangle-normal rotation and interpolation. Verification covers221/222 blocks; the remaining stage-wipe reset compares a masked four-bit value with255 and is unreachable.


### d_door#01 — door-info demo and event helpers

Structure: The first nineteen HD door-info helpers retain the GameCube room selection, event lookup, player adjustment and goal-setting flow with relocated actor fields and singleton accessors. Front and back rooms occupy packed six-bit fields; door type occupies four parameter bits. Graphics: player and event goal adjustments use the HD fused arithmetic order and translation-unit trigonometric table. No gameplay change is established for this range.

Qualification: Verification checks memory and calls with external SDK/event/player callbacks mocked, using twelve-byte vector temporaries. The remaining door predicates and smoke/key/stop/message helpers are separate unfinished ranges. Native scene and renderer behavior remain deferred.

### d_a_demo00 — scripted demo actor

Structure: The HD actor retains resource-ID changes, demo command parsing and animation control from the supplied GameCube implementation. HD introduces renderer-layout-specific texture descriptor handling, named framebuffer textures and auxiliary model resources. Its model factory consumes resource data and creation flags; the former geometry flag argument is unused by that HD wrapper. The legacy solid-heap size argument is likewise unused by the HD helper. Graphics: draw paths select and restore the HD render-list globals around model submission, and model-specific setup updates material-instance flags.

Qualification: Twenty-one HD routines are verified against the local reference. Coverage is581/619 with38 individually named gaps, primarily bounded literal-string exhaustion plus unreachable dispatch/guard paths. Five pointer-reload equivalents rely on the admitted shipped SDK contracts recorded as assumed, not proven: model creation leaves the unrelated actor auxiliary field unchanged, and lighting stays within its packet/model/material storage. Custom callbacks, malformed aliases and broader native resource/lifetime behavior remain outside that claim.

### d_item (item get functions: execItemGet, item_func_*, item_getcheck_func_*, item classification, life-ball tables) — verified, 2026-10-05
- Gameplay: obtaining the Wind Waker places it on an HD-only fourth item button, and obtaining a sail places it on an HD-only fifth button; that button's displayed item is refreshed right away.
- Gameplay: obtaining the Deluxe Picto Box refreshes any X/Y/Z button that showed the old Picto Box (the GameCube refreshed buttons only for the arrow upgrades).
- HD-only: item number 0x14 (unused on the GameCube) gets a handler that runs an HD system with ten message slots (probably the Tingle Bottle) and bumps an HD statistics counter; its get check answers "no check".
- HD-only: item number 0x77 (unused on the GameCube) is probably the Swift Sail: it occupies the sail inventory slot with a second ownership bit, is placed on the sail button, and has its own get check.
- HD-only: picking up a heart piece also increments a counter in an HD statistics block of the save; picking up a dungeon map also sets a flag in another HD object (probably so the map screen can announce it).
- Structure: the 61 identical sea-chart pickup handlers call one compiler-merged helper; the empty handlers and the "no check" get checks are grouped at the end of the unit. Everything else keeps the GameCube order and logic (inventory offsets shifted for the HD save layout).

### d_mesg (message core, routing, state and controller) — verified, 2026-10-05

- Structure: the HD message block manages message-state queries, message and scope routing, renderer selection, tagged UTF-16 expansion and linked text buffers. Routing formats bounded numeric keys and passes complete native string descriptors; state handling expands tagged text and updates scope and pane state. Several helpers carry older f_op_msg_mng labels, but in HD they belong to d_mesg. The apparent SearchByID helper reads the game's message-state byte without an input id. Whether this is still the same translation unit as on GameCube is not proven.
- Structure: The verified HD control object allocates 0x74 bytes, initializes its HD control vtable and delegates base construction and destruction to the HD message SDK. Its destructor is confirmed by that control vtable despite the matcher attributing it to the demo unit. The separate mode setter writes a byte through the HD game singleton.
- Structure: The supplied GameCube constructor’s four zero line lengths, zero counters, font-size defaults from the message configuration, cleared flags and 486-unit text-box width retain their roles in the HD constructor. No gameplay change is established by these three functions.
- Qualification: native text presentation is outside the mocked-call checks.

### d_com_lib_game — common phase dispatcher

Structure: the common phase dispatcher preserves all three input pointers while repeatedly calling the phase handler until it leaves the continuation state. The reconstruction uses a loop for the GameCube source’s recursive behavior. An adjacent SDK initializer is probably associated with this translation unit; its attribution remains qualified.

### d_ovlp_fade3 — scene fade transition

Graphics: HD retains the staged snapshot, fadeout, next snapshot and fadein transition, with native renderer setup on the first frame and native composite drawing afterward. This replaces the GameCube texture-copy and packet-submission path. Fade progression uses the native global fade state and graphics fade calls. Structure: creation builds the HD packet and registers the overlap with the UI singleton; deletion releases renderer and registration state and clears the native fade state.


### d_metronome — HD metronome screen and controller

HD replaces the GC J2D pane-management structure with its UI screen, animation and state framework. The controller is0x16C bytes, with separate3/4/6-beat arrays,21 timing panes, eight animation slots and dynamic pane factories. Input/demo sequencing and rate multiplied by20 with clamping retain the GC behavior. Guide and melody particles use projected positions and explicit emitter lifetime flags. Scope covers53 native entries plus their local initialization/dispatch companions; absent GC-only methods receive no credit. Verification is qualified to supported six-note demo storage and ordinary resource names terminating before the comparison limit; long or unterminated names were not covered.


### d_shop — HD shop interaction and cursor

HD shop trigger handling obtains the global message object and accepts states7/14/15 only while its busy byte is clear. Nonnegative item navigation is suppressed while the current message-ID field is nonzero; successful navigation updates the HD message state and plays sound0x80E. The GC routine instead receives its message object and uses its simpler continuation/send helpers. HD ShopCursor construction no longer edits all model materials' Z mode. Its factory now returns immediately for failed cursor allocation or a missing model/animation, avoiding the GC null-cursor dereference after a failed model check. The cursor object grows from GC0x58 to HD0xB8 with expanded animation storage, retaining four rendered cursor pieces and restoring render lists afterward. ShopItems remains0x44 and camera action remains0x50, with HD model/base-matrix offsets used locally. Purchase conditions and the GC impossible town-flower-and-idol maximum test remain unchanged in effective behavior; the native bottle check is inlined over the four inventory slots.


### m_Do_ext#04 — morph animation and audio controllers

Structure: The HD morph controller uses 0xC8-byte controller storage, 0xA0-byte audio storage, 32-byte transform records and 16-byte quaternion records. Resource names use eight-byte SafeStrings. Constructor arguments beyond the eight integer registers come from the guest stack. Supplying animation audio data alone does not force audio allocation when its enable argument is zero. Graphics: SDK frame initialization uses the resource's virtual end frame; reverse playback starts from the resulting end frame.

Qualification: The verified fixture keeps the model-resource joint count bounded and immutable and uses the actual empty SafeString assurance method. Arbitrary subclass overrides and resource-count resizing remain outside this gate. Frame transfers preserve raw bits required by the optimized local reference; no independent console signaling-NaN claim is made.


### d_door#02 — door visibility and area predicates

Structure: The next nine HD door-info helpers retain the GameCube adjacency, front/back room choice, area check and draw-state flow through relocated actor fields and singleton accessors. Camera-relative selection uses a twelve-byte vector difference; the older front check constructs an eight-byte globe and two-byte angles. Draw state updates the existing status bits and room fields. No gameplay change is established for this range.

Qualification: Verification covers all reported branches with external SDK/event callbacks mocked and real-size vector, globe and angle temporaries. The remaining door smoke, key, stop and message routines are separate unfinished ranges. Native scene behavior remains deferred.

### d_ovlp_fade — base scene fade

Structure: HD retains the stage-sensitive fade modes and special timing, using the loaded SDK fader through its owner. Stage-name wrappers use the HD string descriptor layout. A global callback drives execution; creation installs the fade-in callback, and transition phases preserve countdown and callback ordering. The adjacent initializer attribution remains qualified.


### d_picture_box — qualified HD photo storage replacement

HD uses a twelve-photo storage class: each photo occupies a0x50040-byte record, with a separate ordering array and global storage service. Erasing a slot shifts the ordering array, while callers update the game picture count independently. The legacy match to the GC erase helper is a behavioral association; it does not establish preservation of the no-argument GC API or the old picture-box UI translation unit. Scope covers27 native storage methods and the linked registration initializer; the preceding photo-record initializer and old GC UI methods are excluded.

### d_event_data — event records and actions

Structure: event record formats and flag, staff and cut progression broadly retain the GameCube behavior. Package resource lookup constructs the HD string descriptors for the resource manager and keeps the fallback loading path. Actor creation, lighting, sound, message and director actions use the HD play-state and SDK interfaces. Actor creation preserves raw vector copies and the native angle representation. The compiled dispatch includes duplicate unreachable tails; those are retained as coverage qualifications.

### d_menu_window

d_menu_window: HD retains the place-name demo event helper inside a new PlaceName screen class rather than the GC full menu-window profile. The retained helper triggers the same awake and majyuu_shinnyuu frame milestones; HD reads the demo frame from a global, removing the GC nullable demo-object check. The HD screen owns two text/layout slots and a native state controller, with resource tags for PlaceName and transitions InAll/Wait/OutAll. Its constructor uses a92-byte object, distinct from the440-byte GC sub_ms_screen layout. Fifteen retained native entries include the screen methods and static state/tag initializer; the other GC menu/font/cloth/collect/map/save routines are not fabricated or credited.


### d_door#03 — smoke callbacks and key controller construction

Graphics: HD retains the alternating door-smoke displacement and initial emitter rate, spread and scale values. Particle creation uses the HD expanded argument list; emitter fields use relocated offsets. Structure: the smoke callback has a 0x38-byte constructor, while the adjacent key controller allocates 0xA0 bytes and initializes its animation controller, resource state and flags. Smoke teardown delegates to its callback end method. No gameplay change is established for this range.

Qualification: Five entries pass isolated caller verification with external particle, SDK and allocation callbacks mocked. Native particle lifetime remains deferred. A mutation that copies nine bytes into an eight-byte host double is excluded as an invalid local-memory operation.


### JAIGlobalParameter — global audio parameters

Structure: Retained global audio controls expose integer, byte, halfword, pointer and float parameters through setters/getters. Changing the sequence-play-track limit also doubles the sequence-control-buffer limit, as inGC. The stream-cut switch updates one bit in the audio-system state while preserving other flags.

HD-only / Removed: The decoded-stream-block setter has an empty native body. The output-mode setter stores the requested byte and retains its invalid-mode assertion/fallback path, but its driver target is also a native no-op; theGC helper's separate stream-library output-mode call is absent. OtherGC-only APIs without retained native entries are not reconstructed or credited here.

Qualification: The Dolby-center setter consumes the full incoming integer register and converts it tofloat; no narrowing is invented from the olderGC u8 signature. This is an ABI observation, not a claim that game callers intentionally supply values beyond a byte.

### d_demo — scripted scene adapters and manager (2026-10-05)

- **Gameplay:** HD retains the GameCube scene flags for actor translation, rotation, scale and animation, alongside camera, light and fog scene objects. The native paths also select animation resources and optional matching audio table entries.
- **Structure:** Verification covers 81 native entries rather than the 30 primary name locators: virtual accessors, setters, object lifetimes, manager operations and compiler support all count. The three prefix initializers and trailing initializer are probably adjacent support and remain attribution-qualified rather than asserted to be primary demo methods.
- **Fix:** Guest vector copies preserve signaling-NaN bits and the original alias-sensitive write order; host float copying was insufficient. Address-passed locals match the HD constructor and resource request sizes.
- **Validation:** Every entry passes 10,000 generated inputs at seeds 1 and 7. Thirteen remaining coverage blocks are a dominated count fallback, abnormal embedded-pointer wraparound allocation paths and bounded string-comparison exhaustion; ordinary shipped names terminate long before the bound. The bounded mutation sample closes at 108 detected differences and three explained equivalents, with eleven compile failures excluded. No broad SDK or external observer equivalence proof is claimed.

### d_a_bst (Gohdan: head, hands and their pedestals) — verified, 2026-10-05
- Graphics: Draw no longer copies the animated joints into a separate shadow model and submits no real-time shadow for it; it also sets the light/TEV colour type twice per model and calls an extra HD lighting step (probably part of the HD lighting and shadow pipeline).
- Graphics: the intro camera's first shot (cutscene state 10) aims at a point about 1000 units to the side of and 400 units below the head, adjusted by two new tuning values, instead of 250 units below the head; probably a reframing for the HD/widescreen view.
- Structure: in the defeat camera the eye tuning offsets are added after subtracting the fixed shot offsets (GameCube subtracts them); with zero tuning values this changes nothing.
- Structure: the actor grows from 0x2FE8 to 0x3334 bytes; the beam, collision-sphere and camera blocks move accordingly. Model/animation setup, attack selection, damage and the demo state machine otherwise follow the GameCube logic.

### d_menu_save — qualified HD SaveMgr compiler support (2026-10-05)

- **Structure:** The matched menu-save label covers an HD SaveMgr manager and its thread. This verified trailing range handles destruction, byte-buffer termination and member-function dispatch; literal GameCube menu-class identity remains uncertain.
- **Structure:** Member dispatch preserves the adjustment and dispatch metadata alongside incoming arguments. Heap cleanup precedes the base disposer and conditional object deletion.
- **Validation:** These eight support entries pass both 10,000-input seeds with full coverage. Fourteen valid mutations differ; compiler failures and a shifted member-descriptor load are excluded. Other SaveMgr ranges remain unfinished, and SDK/storage callbacks remain mocked.


### d_door#04 — key resources and model drawing

Structure: The twelve-entry HD key range uses eight-byte SafeString resource names, creates normal or boss-key models with fixed HD flags and initializes their skeletal animation. Key use retains the switch update, small-key count reduction and different unlock sounds. Graphics: key placement builds the shared matrix, applies its height/depth offset and copies the complete matrix into the HD model. Drawing sets environmental lighting, enters animation and updates the display list. The adjacent twelve-byte stopper constructor is included. No gameplay change is established for this range.

Qualification: Resource and model callbacks remain mocked. The resource-name callee's eight-byte read is explicit in the fixture. A malformed SafeString vtable mutation and an oversized host matrix-array mutation are excluded as invalid operations; the interior-pointer guard equivalence assumes a valid allocated actor or null. Native renderer/resource lifetime remains deferred.


### d_magma — 2026-10-05

The HD magma unit contains 42 functions covering ball and floor rendering, resource setup, path animation, packet traversal and height tests, and object cleanup. Its graphics objects and shader descriptors differ substantially from the older renderer. Ball objects occupy 3,540 bytes, floor objects 3,044 bytes, and the packet embeds eight floors plus 64 room heads. The two ball types share an embedded resource constructor while using different final virtual tables. Rendering selects material variants and invokes the HD graphics imports for display lists, constant buffers, and indexed draws.

All functions matched the isolated reference on 10,000 generated inputs at seeds 1 and 7. Coverage reached 747 of 807 blocks; the remaining blocks are wrapped-address construction fallbacks, long unterminated room-name comparisons, and a final-word-only texture difference. A bounded 80-mutation sample produced 35 observed differences, 43 compile-invalid cases excluded from detection counts, and two source-equivalent changes. The archive retains the explicit fixture scope and earlier development qualifications. These results cover isolated function behavior; native gameplay and GPU resource lifetimes remain outside this verification.


### m_Do_ext#05 — morph matrices and cleanup

Graphics: HD morph calculation gathers animated transforms, invokes the before callback, converts Euler angles to quaternions, blends prior rotation/translation/scale and writes the resulting model records. An after callback and dirty flags complete the update. Structure: error cleanup stops audio, clears owned animation/model references and dispatches model release through its virtual interface. Temporary transform records are 32 bytes and quaternions 16 bytes.

Qualification: Verification uses separately allocated transform, quaternion and SDK record buffers that retain their bases during callbacks. Only the three base-pointer slots and resource count are stable; buffer contents, flags and other callback effects remain checked. Arbitrary resizing, reentry and cross-object aliases remain untested. Historical unrestricted-alias failures are retained; the final source uses ordinary source-order math, with no added NaN-priority helper.

### d_save_init — qualified save-adjacent SDK initializer (2026-10-05)

- **Structure:** A standalone HD SDK initializer adjacent to save code registers three support nodes and initializes two stored constants. Its exact translation-unit attribution remains qualified; the GameCube event registration setup is already inlined in the separately merged HD save implementation. This entry does not extend that ownership or cover the following constant wrappers.
- **Validation:** The single native entry passes 10,000 inputs at seeds 1 and 7 with full coverage. A bounded 24-candidate mutation sample finds 13 differences and one equivalent ordering change; seven compile failures and three invented unaligned call targets are excluded. The remaining equivalence follows from immutable ROM reads commuting with a callback-free registration leaf that touches separate memory. Broader mapping and transitive SDK research remain deferred.


### JASDSPInterface — HD DSP buffers and parameters

Structure: Retained DSP-buffer operations initialize channel state, pitch, mixers, filter coefficients, wave playback, volume/delay and output buses. Global helpers allocate64 channel records of384bytes and four32byte FX records, configure FX buffers, and route boot/sync/mixer requests to the audio system. The allocation clear helper consumes word counts; FX memset consumes a byte count of160times the configured block count.

HD differences: The wave format is read from offset0 rather than the GC offset1, and the loop flag from offset0x12 rather than0x10. Wave setup clears a new readiness halfword at0x5A; a retained helper sets it. Auto-mixer setup stores both the high-byte value and its doubled low component. Volume initialization no longer takes the separate GC delay argument. IIR setup copies eight coefficients rather than four. Channel flush has an empty native body; no GC-only cache-flush behavior is invented.

Qualification: Verification covers supported wave format indices0..7. The generic format>3 path requiring at least four bytes per block remains uncovered because supported indices4..7 use one byte per block; no statement is made that unsupported formats cannot reach it.


### d_door#05 — stopper movement and resource helpers

Structure: the HD stopper keeps separate opening and closing acceleration, height targets, completion flags and room-dependent sound selection. Graphics: its model matrix combines door placement, vertical stopper movement and the reversed-facing option. These entries were verified against the HD reference; a specific gameplay change from GameCube has not been established.

## d_snap

- **Graphics / HD-only:** Snapshot capture uses color and depth textures, SDK render-state objects, a depth-buffer expansion step and a smaller composited image, with an optional post-process effect. The GameCube version configures GX directly and samples the framebuffer's alpha values. HD defaults include 1600×900 and 800×450 snapshot targets and a 560×315 preview region.
- **Structure:** The HD packet owns texture, viewport, camera and projection resources, plus two effect references; creation and cleanup therefore perform substantially more work than the GameCube packet's flag reset. Pixel judgement reads a texture through a dedicated reader and preserves the fixed 63-object registration table.
- **Gameplay:** Projected sphere/cylinder area estimation, per-photo pixel and area thresholds, duplicate special-photo suppression and nearest-object replacement remain recognizable. The HD registration path checks enablement separately from the released-shutter state.
- **Verification:** All60 identified HD functions match the local reference at10000 generated inputs with seeds1/7. Coverage includes the loaded-resource and lazy resource-allocation paths; valid object layouts exclude null-wrapped interior allocation guards, and the actual judge table does not use generic virtual PTMF dispatch.

### J3DAnimation retained frame-controller APIs

J3DAnimation retained frame-controller subset: HD stores rate/frame at0/4, start/end/loop at8/A/C and mode/state atE/F in16bytes, replacing the GC20-byte virtual object layout. Init and checkPass retain the five playback modes and half-open crossing intervals. HD update replaces repeated loop wrapping with at most one fmod call per crossed boundary, gated by a positive span; it leaves the loop-status bit clear. Native lower wrap adds the span to the remainder, while upper wrap stores the remainder directly. Stop/reset and reverse modes retain their clamps and direction changes. Other GC transform/color/material-animation functions and neighboring new resource-loader classes are not implementation or verification claims.

### d_event_manager (2026-10-05)

**Structure:** HD expands the event manager to 0x540 bytes and keeps its running-event pointer beyond the older flag area. Event, staff, cut and data records use HD-specific strides. Resource binding and event processing preserve the HD call order and reload mutable status after callbacks. Adjacent generated entries are included with an explicit attribution qualification. The verification fixture models loaded, finite resources with terminated names and acyclic data links; concurrent resource reload remains outside that qualification.

### d_cloth_packet — cloth simulation and rendering (2026-10-05)

Graphics: the HD cloth packet uses GX2 vertex buffers and shader uniforms, maintaining separate front/back normals and strip rendering. Structure: it retains spring-grid simulation and four cloth variants with material and texture caches. The reconstructed main object reaches 0xA4D bytes; its renderer uses a 0x120-byte local state object. No gameplay difference against the GameCube guidance was established. Adjacent static initialization is qualified by HD address order.

### d_gameover (2026-10-05)

**Graphics / Structure:** HD leaves the older framebuffer capture and game-over drawing entries empty. Creation initializes the death counter and transition timers without building the former GC drawing objects. Execution uses the current UI window and HD animation calls to coordinate game-over choices and play/save state. The background alpha setter updates play storage. Adjacent empty entries and SDK initializer retain an attribution qualification.

### d_meter (HUD) — verified, 2026-10-05
- Structure (head helpers): small HD helpers update visibility and menu bytes and test, set or clear byte flags; the fade-out and fade-in helpers step through their states with an easing towards a six-step boundary.
- Structure (prefix range): twenty entries initialize and animate the heart and magic panes, load selected-item textures, and derive item alpha decisions from HUD flags and current scene names. The unnamed entry labels remain qualified rather than asserting exact GameCube names.
- Gameplay (prefix range): recollection state and the named boss/interior scenes affect which item indicators remain visible. The reconstruction keeps this HD behaviour; a gameplay difference from GameCube is not established.
- Structure (middle range): The GameCube source separates status checks from the per-frame HUD execution routine. The corresponding HD entry identified by the legacy status-check label also performs the frame dispatch and pane transitions, so that label alone understates its scope. Pane-member positions differ between the two layouts. The verified range also includes resource setup, teardown, drawing, transition counters and probably HUD tuning-object constructors; the unnamed static helpers remain attributed by their bounded range. This finding covers the middle HUD slice, not the whole HUD translation unit.
- Structure (trailing controllers): twelve entries update item pulses and particle positions, action indicators, lock-on health displays, magic and currency gauges, pane alpha and photo state. The item and magic controllers keep separate pending and displayed counts; bow animation uses a shared leading timer with per-slot pulse panes; resource strings are eight-byte SafeString objects. These are observations of the HD implementation, not established GameCube differences.
- Structure (tail): five HD-local generated entries (0259CF60..0259D1B4): a factory that allocates a 0xC4-byte HUD object with its HD vtable, three deleting destructors and an empty virtual.
### d_vib_pattern — vibration patterns (2026-10-05)

Structure: all 76 motor/camera shock and quake pattern records retain the GameCube values and eight-byte format in HD. The native shock and quake consumers identify the four tables. This data-only TU adds no callable function.

### d_flower (flowers: HD packet and cutting) — verified, 2026-10-05

The HD Flower packet keeps 200 data records, 72 animation records and 64 room lists, and uses HD shader, vertex buffer, texture and GX2 draw paths alongside the original flower collision/cutting updates. The complete bounded inventory comprises the 23 functions at 02544AA4..02549118 and 23 constructors, buffer/resource helpers and weak destructors at 025491AC..02549998. The allocating packet constructor requires 0x1C654 bytes; lighting locals require 0x1C8 bytes and draw state locals require 0x11C bytes, as established directly from their HD constructors. The shared-header initializer at 02549118 is included without claiming HD-only identity. 025499A0 begins a separate small state machine and is excluded from this bounded packet inventory.

All 46 functions pass 10,000 generated cases at seeds 1 and 7. Whole-unit logs precede only the GroundCross observer expansion from 64 to 84 bytes; source and input fixtures are unchanged, and final 10,000-case ground_y companions at both seeds pass with the full 84-byte object. Coverage is 760/800 blocks (95%); named gaps are recorded separately. The bounded 80-variant sample closes with 37 clean comparison detections, 39 compile-invalid exclusions and four source-equivalent survivors, each explained individually. Development hangs and incomplete attempts remain recorded as excluded history.



### JASDSPChannel — qualified HD DSP channel pool

Structure: HD retains a64-entry DSP channel pool with28byte records, signed16bit priorities, age counters, state/flag words, callback/user-data and DSP-buffer pointers. Allocation selects a free or suitably low-priority channel, retires its prior callback, and installs the new callback/data. Selection prefers lower priorities and, on ties, strictly greater age. Per-channel updates handle completion, force-stop requests, deferred playback and callback-driven stopping; the global update walks64records then synchronizes the DSP interface.

HD differences: Channel records expand from the GC20byte layout to28bytes. Callbacks take event, DSPBuffer/null, and user data rather than the GC channel-pointer/event pair. Callback events0/1/2/3 respectively cover update, playback start, completion/stop and replacement. Priority storage is signed16bit with-1 marking availability. Legacy callback intervals and GC DSP-load-history throttling are absent from this retained native update cluster; no equivalents are fabricated. Wave-ready forwarding is retained as a separate channel helper.

Qualification: The lone legacy alloc match at028159F8 is a behavioral association; its actual HD body performs pool initialization. Native entry/behavior inventory governs this reconstruction, rather than asserting all nineteen GC methods survived with their original signatures.

### s_basic (memory fill helpers) — verified, 2026-10-04 and 2026-10-05

- Structure: unchanged apart from the standard per-unit static initialiser; the fill-with-halfword and clear helpers behave as on GameCube.

Exact HD RPX words identify `sBs_FillArea_s` at0201B664 (seven instructions) and `sBs_ClearArea` at0201B680 (two instructions). The fill halves the unsigned byte count and writes the low16 bits as sequential halfwords; zero/one byte writes nothing, and an odd trailing byte stays untouched. ClearArea sets the fill argument to zero and tail-calls the owned pure helper. This preserves the GC s_basic shape; it is not an odd-length memset.

Fresh standardO2/exactFP controls passed both functions at10,000 inputs for seeds1/7, with all5 original blocks covered. The fixed all14 eligible syntactic recipes yielded seven normal paired target-only differences, one independent-local-initialization swap proposed as scoped source equivalent, and six compile-invalid recipes; root QA remains pending. Mutation pilots use1,000 inputs per seed. Valid nonzero514byte buffer/count0..513 fixture boundaries retain zero/one/even/odd sizes and signedfill extremes; native storage/allocation premises are explicit. Historical Warp actor allocation/lifecycle obligations and all existing review STOPs remain unchanged.


### m_Do_ext#06 — dual-animation controller

Structure: the HD Morf2 controller holds two animations and transfers the current frame to both before model calculation. It stores a separate blend value and uses an empty resource-name comparison, while the earlier Morf path compares a named resource. Its constructor also forwards five stack arguments for playback, audio and model setup. These findings describe the HD implementation; a specific gameplay change from GameCube is not established.

### d_menu_save — qualified HD SaveMgr bootstrap (2026-10-05)

- **SaveMgr bootstrap/startup:** HD names the dedicated manager and worker thread explicitly, allocates separate card and stage buffers, and initializes equipment from saved acquisition flags. Item remapping and selected-slot cleanup follow the retained game logic; literal correspondence to the old menu class remains qualified. Six bootstrap entries pass both full generated seeds. Startup has repeated inlined ladders beyond its bounded loop and several dominated fallthroughs; consistent-inventory coverage is retained separately from unrestricted clobber gates.


### d_auction_screen — qualified controller flag adapter subset

HD auction actor callers identify six flag adapters0261BC08..BC97 using a controller object reached through a global pointer, with talk/slot/gauge flags at +65/+67/+6A. GC global flags and fopMsg screen creation differ. This handoff covers those six adapters only; full legacy screen class/native TU boundary remains unresolved and SDK destructorBC98 excluded. Initialized controller-chain tests pass10k seeds1/7 with6/6coverage; bounded31sample12 actual differences19compile-invalid0survivors. Initial boot-image zeros masked a shifted-global load; strengthened fixture detects it, with history retained. Expanded SDK mapping deferred.


### d_map — 2026-10-05

Structure: The HD map room object occupies 0x12C bytes, embeds its104-byte texture at+0x44 and120-byte screen drawable at+0xAC, and its initializer constructs twenty room slots with the corresponding trailing destructors. Floor lookups use ten encoded floor numbers123–132; the final height interval keeps the PowerPC unordered comparison behavior. The image query retains its ninth integer output argument on the caller stack. Virtual floor, stage and map-info queries keep the arguments their targets read. These layout and calling-convention details follow HD code; they are not inferred from GameCube class sizes.
Platform: The HD screen draw root0258DD08 and the adjacent0258E1E4 stub are empty;0258E0C4 retains a three-iteration loop with no externally visible effect. Other neutral stubs retain their exact call behavior:0258F8DC tailcalls map-mode selection rather than returning immediately. The bounded resource-loader roots02590708 and025907FC retain resource lookup calls but their final record loops perform no observable operation. Their exact GameCube method attribution remains qualified.


### d_door#06 — messages and animated door detail

Structure: HD door messages advance through explicit presentation states and can chain related messages, with selected messages also triggering vibration. The animated door detail chooses its animation from save flags and collected items, applies room sound and model lighting, and records first-use flags. Archive names use the HD string objects and bounded copy path. These entries match the HD reference; specific changes from GameCube remain unconfirmed.


### d_tree (2026-10-05)

Structure: The GameCube room data is a linked list. HD uses a counted pointer array with capacity64 for each room, changing insertion and deletion bookkeeping while retaining tree data and animation pools.

Graphics / HD-only: HD constructs GX2 material, vertex and index buffers and separate normal/shadow uniform containers. Its packet constructor prepares these resources, and a separate preparation entry fills matrix and lighting uniforms before drawing. The GameCube constructor initializes tree/animation state and angles without this resource setup, and its drawing uses the older GX path. These are confirmed renderer and storage differences; no gameplay change is inferred.

Qualification: The39-entry scope ends at025CB29C. Adjacent generic math startup025CB2A0..025CB330 is excluded. Verification uses valid bounded list metadata and an aligned full-size tree object while preserving mutable fields and callback/live-in observations. Original unaligned NaN payload reference-model failures remain archived; the final checks do not establish behavior for corrupted unaligned objects.

### d_ovlp_fade4 (2026-10-05)

**Graphics / Structure:** HD uses native UI texture and sampler wrappers for the transition draw, while packet drawing updates dimensions without the older GC framebuffer-copy sequence. Timer-based scene staging remains. Verification assumes normal construction during an active scene after play initialization; cold first-access behavior is excluded, and that initialization premise remains assumed rather than proved transitively.

### JSUList (2026-10-05)

**Structure:** HD retains the linked-list operations and uses 16-byte links with 12-byte lists. The native unit includes an unnamed list destructor and adjacent SDK initializer, the latter with an attribution qualification. No gameplay or graphics change is inferred from this low-level library unit.

### J3DUClipper — rendering frustum culling (2026-10-05)

Graphics: HD retains frustum-plane construction and sphere/box visibility tests. Its native model-based clipByBox method returns zero, while the GameCube implementation walks model joints and hides or shows shapes. Structure: HD plane and camera fields sit four bytes earlier than the GameCube class layout, probably because this native layout omits the virtual-table prefix. The adjacent initializer attribution is qualified.

### JKRHeap entry adapters (2026-10-05)

**Structure:** This qualified six-entry SDK subset resolves the current heap through thread-specific module context. Allocation and deletion retain the virtual heap dispatch and imported fallback paths. The older allocation label for the current-heap getter is misleading, and an unrelated resource-relative accessor was excluded from the heap scope. No complete heap-library reconstruction is claimed.


### d_particle — 2026-10-05

The HD particle control owns common and room resources, tracks scene reference counts and delayed releases, and connects simple effects, model emitters, and camera callbacks to the emitter manager. Its model pool has 80 slots; control storage is 0x41A4 bytes, including 25 simple callbacks of 0x290 bytes and bound camera callbacks. Effect selection retains the full HD incoming ID, group, room, and alpha register values.

Smoke rendering builds either a billboard or a six-vertex surface, while ripple, wave, and track callbacks write alternating vertex buffers. The reconstructed callbacks preserve matrix calculations, water and attribute decisions, material setup, and every indirect target argument. Local color, vector, matrix, ground, and shader objects use actual callee sizes; the smoke corner array has four eight-byte elements.

The inventory includes 133 functions, direct lighting helpers, weak callbacks identified through virtual tables, primary initialization, and the adjacent math-header initializer, whose translation-unit attribution is an inference. Every function passes 10,000 generated inputs at seeds 1 and 7; coverage reaches 966 of 982 blocks. Remaining gaps are redundant null or fallback branches and large unsigned count conversions outside bounded initialized list fixtures. NaN transport and operand-order corrections model reference behavior; they do not establish a game bug.

### J2DOrthoGraph (orthographic 2D graphics context) — verified, 2026-10-05

The HD orthographic graphics context retains the 0xD4-byte object layout, rectangle scaling and six-parameter orthographic projection setup. Its look-at operation initializes the position matrix, while its setPort delegates context setup and fills the projection matrix; unlike the GameCube routine shown in the reference source, this native setPort does not itself issue a GXSetProjection call. The bounded HD inventory contains six routines, including the adjacent static initialization routine with placement-based attribution. The GameCube drawing convenience wrappers and extra constructor/setOrtho methods were not established in this HD cluster and are not claimed as removed. Verification covers 11 of 12 blocks; the remaining constructor branch requires an invalid wrapping embedded address.


### JKRHeap thread context (2026-10-05)

**Structure:** This qualified two-entry SDK subset retrieves and replaces a heap pointer in the thread-specific context. The setter returns the previous pointer. The wrapper passes a module argument that the native getter discards before reading it; the equivalence qualification covers ordinary mapped module memory. Full heap-library behavior remains outside this subset.


### JASOuterParam (2026-10-05)

Structure: The confirmed HD ten-entry outer-parameter cluster retains the GameCube44-byte layout: two16-bit switch/update masks, six floating parameters and eight signed16-bit FIR coefficients. Parameter selectors update one float and accumulate its update bit; FIR installation copies eight coefficients and sets the filter bit in both masks. No behavioral HD change was found in this compared cluster. HD constructor allocation is handled explicitly by the native entry.

Qualification: Native word sequences identify the small accessors omitted by the legacy name mapping. The preceding and following generic startup entries are excluded; no wider audio-unit or gameplay change is inferred.


### m_Do_ext#07 — dual-animation joint calculation

Graphics: Morf2 blends two animation transforms with complementary weights and converts the resulting quaternion into each joint matrix. It keeps separate transform and quaternion buffers, falls back to local records when needed, and preserves the before/after joint callbacks. The first-frame flag selects immediate application versus gradual morphing. These are verified HD behaviors; a specific rendering change from GameCube remains unconfirmed.

## d_operate_wind

- **Structure / attribution:** The reconstructed HD scope is the modern WindControl dialog, identified by its named resources, state initializer and native constructor/vtable. It has48 entries including11 empty SDK slots; this is a qualified functional counterpart, rather than a claim that the legacy GameCube translation unit survives unchanged. The HD dialog object is164 bytes and owns a separately allocated layout object.
- **Graphics:** Named pane and animation groups distinguish the current wind from the chosen wind. InAll, Disp, OutWait and OutAll states drive the SDK layout callbacks, replacing the GameCube implementation's direct screen/pane management and alpha transitions.
- **Gameplay / input:** Eight-direction angle quantization remains. The HD code also uses point/pane hit testing, distance thresholds, held-input state and button steps to select the direction before committing it to the wind system; the point-input path is probably the touchscreen counterpart of the GameCube stick interface.
- **Verification:** All48 native entries pass10000 generated inputs at seeds1/7. Coverage is146/149; the three remaining blocks are interior-pointer allocation guards that require a wrapped valid-object address.

### d_a_npc_jb1 (Jabun) — verified, 2026-10-05
- Structure: the actor grows from the GameCube layout to 0xD50 bytes; the HD NPC base class embeds larger animation/event controllers, so all Jabun-specific fields sit in a tail block starting around 0xD2C. The constructor additionally fills a default record (vectors, angles, animation and gaze-controller fields) from a constant table before building the embedded controllers.
- Graphics: drawing follows the GameCube order (actor lighting setup, body model with its colour-register animation, then the separate light model with its own colour animation); same behaviour as GameCube (the animation remove step is inlined in HD).
- HD-only: the draw function ends with three one-time guarded initialisations of global values, run only when a global flag is set; probably function-local statics from inlined HD helper code rather than Jabun behaviour.

### J3DDrawBuffer retained native group

J3DDrawBuffer retains a thirteen-entry HD group including its static initializer, identified by native file-name assertions and the head/tail PTMF table. HD puts bucket count at0 and the array pointer at4, reversing the GC order; each packet records its owning bucket at94 in addition to next at10. Reset checks and clears backlinks while saving next before the packet reset call. Initialization retains the existing count, allocation resolves the current heap and can retain old storage on failure, and both sorting and immediate insertion use the base bucket when an index is out of range. Z sorting uses the native camera-vector call and PowerPC unsigned conversion behavior. HD adds removal and an empty query; removal preserves a native self-loop when a non-head target equals head.next. Drawing forwards packet and drawing-context arguments and preserves native head/tail bucket order. The sixteen GC TU methods are not claimed as sixteen retained HD methods.

### d_npc (2026-10-05)

**Structure / Gameplay:** The HD NPC engine uses expanded native object layouts and retains attention, head/joint control, path selection, event turning/movement and talk-state processing. The special event handling for Pnezumi, Tc and KNOB00 includes HD string comparisons and distance adjustment. Resource animation adapters use SafeString objects; raw floating field copies preserve their original bits. Native stack arguments, angular wrapping and callback reloads follow the HD bodies. Generated entries are included through the boundary before the fade engine.


### JKRDvdRipper — qualified mapped two-entry subset

Native027EAE70 streams DVD buffers and inlines Yaz0 decode;027EB5F4 loads/expands a DVD file using64-byte allocation alignment, compression-kind3 normalization to none, and an additional ninth integer decompressed-size output. Both assert JKRDvdRipper.cpp, validating sparse names against actual behavior. Only these two entries are covered; wholeSDK TU/compiler inventory remains unclaimed. Header scratch has128realbytes for64-aligned32-byte reads. Every function passes10000seeds1/7; canonical coverage197/203 plus100focusedringinputs reaches198/203, fivegaps named. Bounded120sample82 actualdifferences7scopedEq29compileinvalid2timeout excluded0OPEN. Eq34/68 retain normalnonwrapping/private-state/unobserved unpublished scratch qualifications; initial retry state-reload failure corrected, history preserved. expandedSDKmapping deferred.


### JASOscillator (2026-10-05)

Structure: HD moves oscillator state, curve selector and envelope index behind the floating fields. Its initializer uses inactive state0, rather than GameCube initialization state1. HD hold/release/fixed-stop/direct-release states use2/3/4/5 where the corresponding GC paths use3/4/5/6. The envelope retains jump, hold and stop commands, linear/nonlinear interpolation, gain and offset.

Structure: HD separates updating envelope progress from retrieving the current gain-scaled value. Starting also installs the parameter pointer and updates immediately when an attack table exists. The legacy getOffset name identifies an approximate behavioral association, not the full retained GC API; a standalone GC forceStop method is not invented for this six-entry scope.

Qualification: Verification covers bounded terminating read-only envelope assets and mutable oscillator state. Three named calc gaps concern contradictory unchanged-rate edges and a high-index overflow branch outside the bounded fixture. Generic startup02816BB8 is excluded; corrupted/cyclic asset behavior and wider audio effects remain unproven.

### JASRegisterParam — audio registers (2026-10-05)

Structure: HD retains the 0x30-byte register layout, bank/program byte access and the GameCube initialization defaults. Inheritance clears general registers and address slots while copying bank, program and pan-control fields. No audio behavior difference was established. The closing SDK initializer attribution is qualified; a separate native constructor was not established.

### d_menu_save — qualified HD SaveMgr card/checksum (2026-10-05)

- **Structure:** This range manages the dedicated HD save manager and card buffer rather than proving literal identity with the matched GameCube menu class. Its checksum returns the accumulated word sum and complement together; the caller writes both into the card record. The clock result also preserves both words.
- **Validation:** All fourteen entries pass both 10,000-input seeds with full coverage using the adopted pair-return observation. Twenty-one valid mutations differ; two malformed indirect contexts and compiler failures are excluded. SDK callbacks remain mocked. Bootstrap fixture history retains the earlier ignored constant-return line; the corrected fixture independently passes both full seeds.


### m_Do_ext#08 — material-color and texture-pattern animation

Graphics: the HD controllers maintain frame playback and install separate color and texture-pattern animation buffers into model data. Resource setup derives material and animation counts, creates SDK descriptors and reads duration through the resource virtual interface. Reverse playback starts from the initialized end frame; unordered rates take the forward initialization path. These are verified HD details; specific changes from GameCube remain unconfirmed.


### d_grass (HD packet and rendering resources)

The GameCube grass unit and the HD unit retain the blade/animation pools, per-room lists, collision responses and wind-driven blade updates. HD adds GX2 shader, uniform-block and index-buffer setup, double-buffered vertex resources and packet draw passes in place of the GameCube static arrays/display-list rendering. Its lighting object occupies 0x188 bytes; the verified reconstruction uses that HD callee footprint rather than the smaller GameCube lighting layout.

The HD inventory includes all 34 functions from 0x02549D38 through 0x0254D9A0, including the small compiler-generated constructors, destructors and empty callbacks used by the unit. The following initializer at 0x0254D9A4 uses item globals and is excluded. Grass_Update preserves the saved frustum scale with a raw 32-bit restore: an ordinary floating load/store quieted signaling NaN payloads and failed the strengthened observation gate. Verification is qualified to valid owner-managed lists, sized disjoint heap allocations and valid SDK uniform locations; the archive names the unreachable/excluded coverage paths.

### d_a_bwd (Molgera; complete: all 26 functions incl. bwd_move, bwd_demo_camera, daBwd_Execute, end) — verified, 2026-10-05
- Gameplay: while the tongue holds its post-grab position, the tongue anchor is pulled after Molgera once Molgera moves more than about 1200 units (plus a debug register) away horizontally; the GameCube code has no such tether (sita_move, inlined into daBwd_Execute).
- Gameplay: in the flying phases the hit cooldown timer is refreshed to 10 every frame, so the tongue and body hit checks of damage_check are probably skipped for the whole flight (GameCube only set it on hits).
- Gameplay: on the transition into the death sequence a play-state float is cleared (probably a boss-music or camera parameter of the HD play object).
- Structure: the action functions (start, wait, reset, sita_hit, eat_attack, fly, s_fly) are inlined into bwd_move; damage_check, the tongue control and the environment colour update are inlined into daBwd_Execute; the execute loop walks ten consecutive effect timers; the fly action makes an unused play-object lookup. Complements the earlier entry for the first 22 functions (layout shift and Create/Draw differences listed there).


### m_Do_ext#09 — texture, register-color and bone animation

Graphics: HD keeps separate controller layouts for texture matrices, register colors and bone transforms, then attaches their SDK animation buffers at distinct model fields. Bone playback uses the animation resource duration and can rebind an existing buffer. The adjacent render-state helpers construct full SDK state objects before changing and applying their flags. These are verified HD details; specific differences from GameCube remain unconfirmed.

### d_menu_save — qualified HD SaveMgr storage (2026-10-05)

- **Structure:** The HD manager separates card records, per-room stage maps, stage headers, descriptions, extra data and photo cleanup across dedicated load and write workers. Eight fixed member-function records select each worker stage. This SaveMgr identification is supported by HD strings and object tables; literal correspondence with the matched GameCube menu class remains qualified.
- **Gameplay:** Recollection saves recognize the boss scene names and preserve selected equipment around the temporary save-state update. Event and acquisition flags feed a compact progress indicator. These retained game behaviors are reconstructed from the HD implementation; a gameplay change from GameCube is not established.
- **Validation:** The complete 81-entry unit passes both 10,000-input seeds. Storage coverage leaves named bounded-string, dominated-index and unused virtual-dispatch arms; callback steering additionally covers the closed-file status path. Mutation survivors are caught by targeted inputs, removed with unused bookkeeping, or explained by exact guards and fixed descriptors. SDK and storage callbacks remain mocked. A temporary address-cutoff stop was corrected because this is HD game code.

### JAIZelAnime (Zelda animation-frame sound hooks) — verified, 2026-10-05
- Gameplay: the distance cut-off for animation sounds has two HD-only cases: sound 0x588D may be heard up to three times the normal distance while an HD-only flag of the animation object is set (cleared whenever an actor's animation sound data is re-initialised), and sound 0x59A3 up to twice the normal distance. The GameCube list of exempt sounds (gull voice, Helmaroc wings and footsteps) is unchanged.
- Gameplay: the pitch factor of animation sounds applies to every sound category except 6 and 0x12..0x16 (the GameCube source is not decompiled reliably here; probably unchanged in intent).
- HD-only: initActorAnimSound is overridden by Zelda's class (calls the base version, then clears the flag above); setAnimSound first runs an HD pointer-validity check on the position (its result is unused, probably a stripped assertion).
- Structure: the sound-control object (JAIZelBasic) fields move by +0x10 (early fields) and +0x70 (the scene/state block); the GameCube logic of startAnimSound, setSpeedModifySound and setPlayPosition is otherwise kept.

### d_kankyo (environment / lighting core: time of day, palettes, point lights, tevstr setup) — verified, 2026-10-05
- Structure: the environment light object is an HD function-local static (10475A68, 0x120C bytes, created on first use by the accessor 02555D0C) instead of a global; most GameCube fields moved (the colour/ratio block by +0x3E8). Stage palettes are 0x6C-byte records with byte colours plus HD float light parameters, and the blended environment colours are kept as byte RGBA. The tevstr grew to 0x1C8 bytes. dKy_Create inlines envcolor_init, dKy_Execute inlines dKy_event_proc, drawKankyo inlines the arrow-colour routine and setLightTevColorType inlines its per-material helper. Stage names are compared through sead SafeString temporaries.
- Removed: the nearest-point-light search for actors and the player (dKy_light_influence_id and its getters) and the effect-light K1 colour path in the material setup; CalcTevColor keeps only the effect-light lookup. The view near/far checks in the fog setup are still evaluated but no longer used.
- Graphics: every frame the point lights and effect lights are handed to a separate HD light manager, and the light positions, colours and fog colour/distances are written into HD shader parameter objects (looked up per light slot); material tev and k colours are also written as float parameters. Probably the HD deferred/forward lighting path.
- Graphics: K0 colours of actors and BG0 fade towards C0 during a day/night transition; the fog end distance is pushed out by an HD brightness value (which itself eases towards 1 in some "sea" rooms and follows the thunder flash); fog gets an alpha from the palette; vrbox kumo colour equals the fog colour in "kenroom" and "ShipD".
- Graphics: a per-room light direction table (loaded from the Stage archive) can replace the base light; the sun gets an HD height factor; the light position fades between a day and a night position (and eases while the player is in a special state, probably swimming or crawling).
- Graphics: HD time-of-day light presets: a day and a night preset, blended over hours 6-7 and 18-19 in "daylight" stages (stage types 1-4/6 except ADMumi, Mjtower, M2tower, Ocrogh, ENDumi, M_Dra09 and some rooms of Hyrule, M_NewD2, Siren, kinMB, kindan); separate presets for an event bit, an HD override and "Hyroom" (eased by an HD parameter, full strength during "Demo49").
- Gameplay: start times forced per stage when a stage loads: "A_umikz" at least 240, "sea_E" 240, "sea" room 44 back to 165 after 180 when the save lacks the item checked by the time-pass code (probably the Wind Waker), "ENDumi" 105. While time is stopped it still runs in "A_umikz" (up to 270) and "ENDumi" (up to 180).
- Gameplay: the arrow colour change became three timed colour pulses (orange, blue, green; own colours in "GTower") driven by the high nibble of the colour-change flag; the item-get light ramps faster (8 per frame to 255, power 200) and lowers the other colour ratios while active.
- Gameplay: the pselect blend in "sea" uses the slow 300-frame rate only when the pselect index changes; "ma2room" blends in 15 frames; the sword flush effect light uses power 700 with a fixed fluctuation.
- HD-only: a fixed override palette (when +0x10A2 is set), an HD constant chosen per stage ("Obshop"), the HD light-preset table and per-room light tables.


### m_Do_ext#10 — invisible-model packets and draw buffers

Graphics: the HD invisible-model controller builds packets for individual joints and draws enabled materials through a linked packet list. Its sorted entry paths derive positions from joint matrices and select separate draw buffers. Other wrappers temporarily replace the global opaque and translucent buffers around model updates, then restore them. Draw callbacks use full descriptor and render-state objects. These are verified HD behaviors; specific changes from GameCube remain unconfirmed.

### leftover-actor-functions (47 compiler-emitted and small functions at the ends of merged actor units) — verified, 2026-10-05
- Structure: almost all of these are compiler output with no GameCube source line: out-of-line copies of the always-true background-actor delete check, empty string-class virtuals and trivial string destructors, deleting destructors of the actor classes, and per-file static initializers for the header constants.
- Structure: HD gives the "static" companion files of the mini-game board, the tri-box object and the boat their own static initializers; the board's one also sets two pairs of distance constants (probably draw or culling ranges).
- Graphics: the salvage chest's shadow particle callback, when attached to an emitter, now also looks up a dedicated "salvage" shader program (loading it on first use) and assigns it to the emitter — HD-only, probably for the HD water shadow look.
- Gameplay: the boat's start-position reset (used by other actors' cutscenes) matches GameCube; HD stops two of its effect callbacks through their virtual end call instead of a direct one.

### JAIZelAtmos (Zelda ambient sound positions: sea, shore, river, waterfall, rain) — verified, 2026-10-05
- Structure: the GameCube decompilation has only placeholders for this unit, so the comparison is limited to what the HD code shows; the position lists keep their GameCube roles (sea surface points, river points, waterfall points, window points for rain) at HD-shifted offsets (+0x70 relative to the GameCube fields).
- Gameplay (HD code, probably as on the GameCube): the sea ambience mixes three layered loops (left, right, surround) from the loudest registered sea point; it is attenuated by a third while a mute flag is set and by 30% while one particular stream plays, and in one scene state four points around the camera are added automatically.
- Gameplay: a sea point list that overflows keeps 63 entries; a river list that overflows is reset to 47 entries (a fixed count, probably a leftover), and river points further than three times the sound distance are ignored; waterfalls further than four times the distance are not registered.
- Gameplay: the shore sound is quieter (half volume) in stage "Demo08" while a global demo state is 1, and two thirds otherwise unless a level value is set.
- HD-only: rain without window positions plays a different heavy-rain sound in scene 0x5C; waterfalls in scene 0x39 use their own sound; stream checks go through an HD sound manager.
- Structure: each sound unit carries its own SafeString vtable copy (deleting destructor 0201D3FC and an empty function 0201D410) used by the inlined stage-name comparison.

### d_file_select (save-data selection screen) — verified, 2026-10-05
- Structure: the HD save-data selection screen coordinates slot selection, copying, deletion and name-entry progress through linked UI states. Panel navigation links, transition vectors and alpha rates are kept separately from the save manager, and event handling builds complete SDK event objects before dispatch.
- Qualification: the reconstruction follows the HD screen layout and callback signatures; that this is still the same translation unit as the GameCube d_file_select is not proven.

### d_kankyo_wether (weather packets, wind, renderer resources) — verified, 2026-10-05
- Graphics / Structure: HD weather packets own GPU geometry, texture state and shader resources for sun glare, rain, snow, mist, floating particles, clouds and waves. Their constructors set up these resources and their destructors release arrays and renderer objects; the corresponding GameCube packet destructors are empty.
- HD-only: additional packet setup and draw routines build and bind indexed geometry and shader uniforms, so the HD unit has far more functions than the matched GameCube weather functions (163 functions, 025776F8..02585098, verified as one set).
- Graphics: the precipitation renderer re-initialises its texture descriptors every frame and only rebinds a texture when the new descriptor differs from the cached one; otherwise it only refreshes the image pointer and size.
- Structure: weather updates read environment and game state through accessors, and resource lookups use temporary string objects. The sun, precipitation, star, mist and wave state machines still select effects from stage names, weather counts and time of day; wind direction is still reported in eight compass sectors, and the global wind falls back to the room's wind table and stage-specific strengths when no custom wind is set (as on GameCube).

### d_a_boomerang (Boomerang: flight, lock-on sight, blur trail) — verified, 2026-10-05
- Graphics: the lock-on sight and the motion-blur trail are rebuilt for the Wii U GPU. Instead of GameCube display-list packets, each holds native vertex/uniform buffers and draws with GX2 calls: two trail rings of 60 segment pairs, plus sight quads with their own texture, which is re-copied only when the cached texture descriptor changed. This makes the actor about 0x264C8 bytes, and all GameCube fields after the model pointer move accordingly.
- Graphics: no blob shadow is set while the boomerang flies (HD uses real shadows). The draw instead hands the model to a Link-side list (023D9340), probably so the boomerang takes part in Link's HD shadow/lighting pass.
- HD-only: on the "GanonK" stage, createHeap also initialises an extra helper object at +0x26194 (0207FD38). Its role is unknown, probably related to the special lighting of that stage.
- HD-only: the first procMove call sets a global flag (0x1046273C). Its consumer is unknown.
- Structure: the throw logic, flight steering, lock handling, fly range (2500, or 5000 on the ship and in GanonK), rock-line callback and water-effect call match the GameCube. Execute dispatches the proc through a stored member-function pointer, as in the GameCube.

### m_Do_ext#11 — line and material helpers (2026-10-05)
- Structure: the twelve HD functions at 025E8FA8..025EA544 allocate and release line buffers, normalize signed colour components, and configure and draw line materials. Material setup captures the colour components before writing aliased destinations, and the draw state stays in the same 284-byte local across iterations. These observations describe the HD implementation; they do not establish a specific GameCube difference.

### d_drawlist (draw queues, mirror packets and depth readback) — verified, 2026-10-05
- Structure: the HD unit contains 52 entries for draw queues, depth sorting, mirror materials and textures, depth readback, and formatted diagnostic text. Mirror texture binding compares descriptor fields before updating or rebinding the texture. Projected depth selects one of 256 sort buckets, with a strict lower-bound comparison. These are HD observations, not established GameCube differences.

### d_resorce (resource archive control) — verified, 2026-10-05
- Structure: the resource bookkeeping is rebuilt around name hashes: archive and resource names are hashed with a CRC-32
  (0273B264) and kept in balanced search trees keyed by that hash, instead of the GameCube's fixed-size info arrays searched
  by name.
- Structure: names are sead safe-string objects (bounded length) rather than plain C strings; resource files are found by
  name suffix and loaded through HD model/animation/material factories.
- HD-only: per-archive heap selection: dedicated heaps for named special archives and for object archives, chosen by
  name and by the free size of each heap, created and destroyed by the control object.
- HD-only: an "extra resources" table per archive (resource names containing a marker substring are registered under
  their hash), and table-driven resource loading by archive-name hash.
- Structure: matcher error: 0273B264 is the CRC-32 helper, not dRes_control_c::getResInfoLoaded.

### m_Do_ext#12 — line ribbon updates (2026-10-05)
- Structure: the two HD update routines at 025EA548..025EB828 build ribbon vertex pairs from positions and either a scalar taper or per-point byte widths. They clamp the requested point count to capacity, form camera-facing normals, and keep the previous normal on the degenerate path. Both finish the line models and update lighting materials. These describe the HD implementation, not an established GameCube change.

### hd_ui_02000020 (HD UI input-event receiver) — verified, 2026-10-05
- HD-only: GamePad UI input receiver (no GameCube counterpart): keeps a list of touch targets, hit-tests the touch point against them, turns touch on/hold/off and button triggers into queued UI events and dispatches the input state to per-button virtual handlers.

### hd_ui_02001084 (HD UI event record and helpers) — verified, 2026-10-05
- HD-only: small GamePad UI support classes: the UI event record (kind, id, 32-byte payload), a holder that publishes the current input state globally, and a fixed short spin delay.

### hd_ui_020015C8 (HD UI widgets, pointer and event queue) — verified, 2026-10-05
- HD-only: GamePad UI framework pieces: a widget base with a global serial number, the touch-pointer object that maps raw touch coordinates to screen coordinates (centre and scale from the screen size), touch widgets that hit-test a layout pane, and a fixed-size UI event queue allocated from a heap.

### hd_ui_020021FC (HD layout screen framework) — verified, 2026-10-05
- HD-only: the GamePad/TV 2D layout screen wrapper: loads an NW4F layout (.bflyt) by name through a resource accessor, looks up its panes and animations by name into tables, and draws/releases it; plus small layout-object wrappers and a holder that attaches child objects after a sead type check.

### m_Do_ext#13 — textured line materials (2026-10-05)
- Structure: the seven HD functions at 025EB82C..025EDBBC extend the base line material with texture state, colour, lighting and line ownership. The update routines accumulate texture distance across segments and fill texture coordinates alongside ribbon positions, tangents and normals. Draw routing includes a second shader path and selects a render-state field by draw mode. These describe HD behaviour without asserting a particular GameCube difference.

### hd_ui_020032F4 (HD UI layout helpers) — verified, 2026-10-05
- HD-only: layout animation entries (named, with a playback rate, started looping or one-shot depending on the animation resource), a generic list visitor and thin wrappers around NW4F layout objects, with sead run-time type information.

### m_Do_ext#14 — material sorting and weighted positions (2026-10-05)
- Structure: the final fifteen HD entries at 025EDBC0..025EE17C manage material sort lists, weighted-position buffers, registration globals and array cleanup. Weight calculation preserves the model flag around pose updates, uses the original bone-index sequence, and accumulates transformed positions into twelve-byte outputs. These observations do not establish a specific GameCube difference.

### JAIZelInst (Wind Waker conducting sounds: metronome, beat judging, melodies) — verified, 2026-10-05
- Structure: the HD object is 0x34 bytes instead of the GameCube's 0x4C; the stick/arm-swing state at the start of the GameCube object (0x00..0x17) is gone and every later field moves down by 0x18.
- Gameplay (HD change): conducting no longer reads the control stick for melodies; the GameCube stick functions are an empty member (0202A034) and a stub that always returns "no melody" (0202A038). Melodies are judged only from the beat positions (judge/metronomePlay, unchanged in behaviour).
- Gameplay (as on the GameCube): the three beat setups (3/4, 6/4 and 4/4), the volume scaling and clamping, the eight melody patterns and the c-stick-position-to-note tables keep their GameCube contents.
- Structure: sound effects go through JAIZelBasic::seStart with the HD extra parameters (volume as the 2nd float, -1 for the unused ones).

### JAIZelSound (distance volume / pan / surround of Zelda sound effects) — verified, 2026-10-05
- Structure: JAIZelSound stays a 0x48-byte JAISound subclass with its vtable at +0x44 (10003F28); the constructor and deleting destructor are its own (0202ACCC / 0202B65C).
- Gameplay (HD change): setSeDistanceVolume uses a new inverse-distance volume curve (0202AF70, HD-only: full volume within a near radius, then falling off with distance, silenced below a floor) for a handful of sound ids (0x48DA, 0x590A, 0x69DF, 0x6A36; 0x6238 and 0x7047 at half volume with a smaller radius; 0x701D, a sea/ambient sound, uses a wide curve except in scene state 0x12). All other sounds keep the GameCube distance curve.
- Gameplay (HD change): sounds with the 0x01000000 sw-bit never go below one third volume.
- Gameplay (as on the GameCube): setDistanceVolumeCommon with the camera index 4 takes the nearest of the audio cameras; the distance modes scale the range by powers of two; setSeDistanceDolby maps the depth to the surround range (front/behind maxima, centre value).
- Structure: setSeDistancePan (0202B354) is unnamed by the matcher.

### JAIZel data TUs (JAIZelCharVoiceTable / JAIZelParam / JAIZelScene and two more table TUs) — verified, 2026-10-05
- Structure: HD has five table-only sound-control TUs (GameCube: JAIZelCharVoiceTable, JAIZelParam, JAIZelScene); three sit between JAIZelBasic and JAIZelInst, two between JAIZelInst and JAIZelSound. Their only code is the common header static initializer each JAIZel TU carries; HD therefore has two table TUs more than the GameCube file list (contents not compared, tables only).

### hd_ui_02003C58 (HD UI layout object) — verified, 2026-10-05
- HD-only: the HD UI's NW4F layout subclass: it replaces the pane factory so picture/text/window/bounding/parts panes are created as HD wrapper classes, builds parts layouts sharing one resource accessor, and binds numbered parts by name (a part named like "Name00" is matched by its base name and the index).

### hd_text_ruby (HD message tag processor: ruby, font, scale and colour tags) — verified, 2026-10-05
- HD-only: the HD message renderer handles inline tags itself instead of the GameCube JMessage control codes: a ruby tag draws the small reading text centred above its base characters with a second text writer (scaled down, never wider than the base text), font tags switch between the message fonts, scale tags change the character scale and restore it afterwards, and colour tags assemble an RGBA colour from the tag parameters.
- Structure: tags are records inside the UTF-16 message text (group, tag number, parameter size, parameters); the processor keeps a small saved state (scale and font) so an end tag can restore the writer.

### hd_text_writer (HD message tag processor object and text splitting) — verified, 2026-10-05
- HD-only: the tag processor object that the HD message windows give to the NW4F text writer; it combines the message-unit tag state with the ruby/font state, forwards rectangle calculation for both, and resets them per message.
- HD-only: helpers that walk UTF-16 message text skipping embedded tag records, to count characters, find line breaks and cut a message into pieces by line.

### hd_res_model (HD model resource: bfres + per-model shader archives) — verified, 2026-10-05
- HD-only: a resource object that loads a Wii U bfres model file and, for every model it contains, the matching shader archive (.sharcfb) from the same archive; it binds textures from a second resource when asked and releases the shader archives in its destructor. GameCube models (J3D bmd/bdl) have no such per-model shader step.

### hd_res_load_task (HD asynchronous resource loading task) — verified, 2026-10-05
- HD-only: a sead task that queues archive/resource load requests in ring buffers, loads them one at a time on its own heap, keeps a reference count per resource name and frees resources when the count drops; GameCube loads archives through dRes_control_c and the mDoDvdThd commands instead.
- HD-only: start-up loads of the permanent packs (permanent 3D data, the first permanent archive, particles, program textures, JPEG data) and the message/font resource managers are driven from here.
- Structure: names are hashed with sead's CRC32 helper for the reference-count map.

### hd_res_mgr (HD resource manager: archives by name, packs, loader threads) — verified, 2026-10-05
- HD-only: a singleton manager that keeps every loaded archive in a name-keyed map, loads object and stage archives (from loose files or from per-language permanent packs), scans packs to register the files they contain, and runs the loading on two loader threads guarded by a critical section.
- Structure: GameCube resolves archives per stage through dRes_control_c tables; HD adds this extra layer between the game's resource calls and the file system, with packs mapped by name hash.

### hd_input_eventmgr (UI input event manager: touch and pointer dispatch) — verified, 2026-10-05
- HD-only: the GamePad UI event manager: it keeps a list of UI receivers, hit-tests touch and pointer positions against them, tracks focus, grab and hover, sends press/release/drag/flick events, and continues a released drag with inertia that slows down by a friction factor until it stops.
- HD-only: it switches between touch, pointer and button input modes and remembers the last mode change so the UI can show the right cursor.

### hd_input_misc (HD UI receiver hit tests and GamePad orientation reader) — verified, 2026-10-05
- HD-only: the standard UI receiver tests a touch against its pane's rectangle transformed by the pane's global matrix, and measures distances between panes.
- HD-only: a small reader copies the GamePad orientation (a 3x3 matrix from the controller state) into an output vector when the controller reports valid motion data.

### hd_input_ctrl (HD controller manager and pointer controller) — verified, 2026-10-05
- HD-only: the controller manager decides between TV-only, GamePad and both-screen modes, owns the pointer controller (touch/stick cursor position and calibration) and the swipe cursor, and keeps the GamePad orientation with a calibration matrix that can be reset.
- HD-only: the pointer controller merges button bits of the active controller into its own held/triggered bit set (GameCube reads only the pad).

### hd_swkbd_mgr (UI input software keyboard manager) — verified, 2026-10-05
- HD-only: wraps the Wii U software keyboard used for entering names: it creates both keyboard layouts in its own work memory, copies the controller state into the keyboard each frame, opens the input form with the current text and limits, and returns the entered UTF-16 text when the player confirms.

### hd_input_cursor (HD swipe/stick direction detector) — verified, 2026-10-05
- HD-only: keeps the last five touch positions, sums their movement into a cursor vector (clamped to unit length, continued with inertia for a few frames and drawn back to the centre when released) and reports one of four directions once the vector is long enough.

### hd_screen_arrow (HD arrow-type icon screen) — verified, 2026-10-05
- HD-only: a 2D layout screen showing the current arrow type (normal, fire, ice, light) with a looping element particle on the icon; when the arrow type changes it plays a change animation and moves the old and new particles with their panes.
- Graphics: GameCube draws the arrow icon in the item HUD without particles; HD attaches a 2D particle per arrow element.

### hd_screen_auction_rupy (HD auction rupee meter and bid counter) — verified, 2026-10-05
- HD-only: the auction screen shows a rupee meter (filled up to 100) and a three-digit counter that counts towards the current bid in steps of 1, 10 or 100 depending on the distance, with a short delay between steps, a counting sound per step and an end sound.
- Graphics: the counter digits roll (see hd_screen_auction_number) instead of switching instantly.

### hd_screen_auction_number (HD auction counter digit) — verified, 2026-10-05
- HD-only: each counter digit is its own small screen that rolls from the old to the new digit over three frames per step, upwards or downwards, wrapping between 9 and 0.

### JAIZelBasic (Zelda's sound control: sound effects, bgm, streams, events) — verified, 2026-10-05
- Structure: the HD object is 0x21F4 bytes (vtable 10003A08, singleton at 101FFC78). Fields 0x20..0xAC move +0x10 against the GameCube, the sound-effect tables grow from 24 to 32 slots, the state block from 0x1F8 moves +0x70, and HD adds allocation records for the JAIZelSound arrays (+0x20E8, 16 entries) and handles for an extra sound group (+0x20B0).
- Structure: seStart and bgmStart take extra parameters in HD (seStart: pitch, volume, pan, surround and a flag; talkOut gets its fade time as a parameter). Streams are handled by an HD sound manager; checkStreamPlaying/checkPlayingStreamBgmFlag ask it instead of the JAudio stream.
- Gameplay (HD change): a new instance limiter (0201D670) stops a playing copy of a sound effect at the same position, or the highest-priority copy once a per-sound maximum is reached, and blocks a new copy near an equal-or-higher-priority one.
- Gameplay (HD change): bgm ids 0x80000105..0x80000152 are variants (track mute sets 1..3, and for one bgm a different tempo) of base bgms; starting a variant of the bgm already playing only switches its track mutes/tempo instead of restarting it. One room (scene 0x12, room 0x29) fades its bgm in by distance from a fixed point.
- Gameplay (HD change): the low-health alarm can be switched off by an HD option (save option object +0x12C0); setLinkHp changes the battle bgm with low health; bgmNowBattle starts a sea battle bgm on the sea; cbPracticePlay plays a different tune in scene 0x39; charVoicePlay skips a voice already playing; startIsleBgm keeps the sub bgm; scene 0x2D silences a few sound effects (e.g. 0x7051) in seStart.
- Gameplay (HD change): a "no camera" position far outside the world is recognised in getCameraInfo; HD checks that all 20 static waves are loaded (020278E8) before some starts.
- Structure (matcher errors): 02027168 (named check1stDynamicWave) is bgmStreamPrepare, 0202796C is probably check1stDynamicWave; 020233C8 (named checkPlayingStreamBgmFlag) is an HD stream check with an id; 025E1D08 (named JAIZelBasic) is the m_Do_audio shim mDoAud_bgmSetSwordUsing whose real method is 02028FE0. Many methods are unnamed by the matcher.

### hd_ui_02004A3C (HD layout animation set) — verified, 2026-10-05
- HD-only: per-screen layout animation sets: named NW4F animations bound into slots (by name, by pane group or as a numbered range) and attached to active channels; starting a channel stops what played there; queries for finished/stopped animations.

### m_Re_controller_pad (HD controller rumble patterns) — verified, 2026-10-05
- Structure: HD-only TU named after the Twilight Princess Wii file m_Re_controller_pad.cpp (assert strings); it replaces the GameCube m_Do_controller_pad rumble path. Four pattern players (one per pad) step through a bit pattern once per frame; a pattern resource is a 16-bit length followed by the bits.
- HD-only: when the GamePad is the rumble target, the whole pattern is handed to the GamePad motor call at the start of each run instead of being stepped bit by bit, and only the first player is used.
- Gameplay: probably a quirk — in the per-frame stepping path every bit is sent to controller channel 0 whatever the player index, and an idle player (no pattern) ends the update loop, so later players are not stepped in that frame.
- Structure: two initialiser-only TUs that follow it (header static objects only) are filed with this unit.

### hd_ui_02005978 (HD application entry and UI graphics) — verified, 2026-10-05
- HD-only: the Wii U application entry: system and heap initialisation (checks the MEM2 expanded heap), creates the sead Cafe game framework and a graphics-system heap, sizes the screen from the system's TV/GamePad resolutions and runs the root task; plus the UI graphics singleton that owns the NW4F layout graphics resource, font and draw info, and a small layout child object.

### hd_sys_02006294 (HD state machine) — verified, 2026-10-05
- HD-only: a generic state machine used across the HD code (UI screens and many actors' HD additions): states are objects created by the owner's factory, with enter/update/leave hooks and an exit result kept for the next state; the application's root-task entry also lives here.

### hd_snd_0202B6B0 (HD sound system container) — verified, 2026-10-05
- HD-only: the HD sound layer's container and archive table: six sound sub-systems created together on one heap, and a table that maps the game's sound ids (bank in the upper bits, index in the lower 10 bits) to entries of the NW4F sound data, loaded through the HD resource manager.

### hd_snd_0202BD3C (HD sound manager) — verified, 2026-10-05
- HD-only: the HD sound manager singleton: owns the sound-system container and the NW4F sound-archive player and wires the player's sub-systems, plus a small sound-handle wrapper whose operations do nothing without a handle.

### m_Do_ext — blend dispatch, heap scopes and tail helpers (2026-10-05)
- Structure: the HD blend entry attributed to mDoExt_MtxCalcAnmBlendTbl reads packed 48-byte animation records and accumulates weighted quaternion-derived rotation, translation and scale into 56-byte output records. Its pointer arguments differ from the GameCube method signature, so the name records attribution rather than signature equivalence.
- Structure: the HD animation dispatch has separate packed Euler-to-matrix, quaternion-blend and Euler-to-quaternion paths using the same 48-byte source and 56-byte destination records; game packet virtuals forward cup drawing, return material IDs, clean up nested line arrays and release line objects.
- Structure: game heap scope wrappers keep the current-heap restoration and destruction order; start-up initializes separate byte objects and game globals.
- Structure: two array-element constructors used by the line-material setup and this TU's copy of the SafeString termination hook are emitted out of line in HD.
- Structure: the rotation-matrix-to-quaternion conversion used by the animation blending (old-frame and morf2 calculations) is a separate function in HD (probably the sead quaternion helper); it picks the largest component and uses the hardware reciprocal-square-root estimate with one refinement step.

### hd_snd_0202C27C (HD JAudio to NW4F sound bridge) — verified, 2026-10-05
- HD-only: the bridge that plays the GameCube JAudio sound requests through the Wii U NW4F sound system: a JAudio sound's name is converted to its NW4F counterpart and looked up in the NW4F archive; some sounds are filtered (certain system sounds, Link's voice set during game-over handling, some demo sound ids, a few stage-specific streams); finished NW4F sounds are recycled each frame.
- Gameplay: probably two stage-specific stream substitutions (sound ids 0x5800 in Demo23 and 0x588D in Demo45 under a scene flag) replace the GameCube streams.

### hd_snd_0202DAD0 (HD sound path strings) — verified, 2026-10-05
- HD-only: accessors for the NW4F sound player objects, and five sound path strings assembled at start-up from fixed string pieces (sead string copy/append).

### hd_snd_0202EE08 (HD sound player) — verified, 2026-10-05
- HD-only: the Wii U sound player: ten named NW4F players (system SE, sequence BGM, stream BGM and sub-BGM, menu UI, GamePad-only, GamePad game, GamePad TV, item, controller) whose TV and GamePad volumes are set every frame from master faders and the current screen mode; the controller speaker gets a volume that fades with a distance value; pause/resume of all players, stop-all with exceptions, and dedicated stream handles for the Demo45 stream, the ending, the epilogue and the staff roll.

### hd_snd_020302E4 (HD sound archive) — verified, 2026-10-05
- HD-only: the HD NW4F sound archive object (path, sizes, sound groups loaded by name: the static group and the BGM/wave group) and the sound heap object.

### hd_snd_020306F0 (HD sound interface) — verified, 2026-10-05
- HD-only: the sound functions the HD game code calls: play UI sound effects by name, fade TV/GamePad output when the game mode or screen mode changes (TV only, GamePad only, both), menu-in/out ducking through the GameCube sound manager, the HD-only streams (Demo45, ending, epilogue, staff roll), and pause/resume of all sound.

### hd_snd_02031264 (HD BGM / JAudio controller) — verified, 2026-10-05
- HD-only: the controller that runs the GameCube JAudio sound manager (JAIZelBasic) inside the HD sound system: it creates and initialises it, drives its frame, feeds TV/GamePad output volumes to NW4F every frame, and can play a delayed system SE; plus the SE player used for HD UI sounds.

### hd_sinit_02031A98 (initialiser-only HD TUs) — verified, 2026-10-05
- HD-only: 27 translation units that contain no code besides the common header static initialiser; probably HD data tables.

### hd_sys_0203F8CC (HD system task) — verified, 2026-10-05
- HD-only: the game's system task (one instance): at start-up it wires the font and message managers, creates two expanded heaps for the system managers, runs the game's main initialisation and then starts two child tasks (probably the error-viewer task and the game task) through the sead task manager; when a debug-draw switch is set it also prepares full-screen viewports.
- HD-only: the part of the unit past 0x02040000 holds a forwarding thunk, the TU's header static initializer, two destructors and an empty virtual of the system task's classes, a sead runtime-type check with two lazily initialised type-info objects, and two sead task factories that allocate the task object from the current heap of the construction argument and run its constructor.

### sinit_only_tus (fifteen initializer-only translation units) — verified, 2026-10-05
- Structure: fifteen HD translation units contain no code except the common header static initializer (a zeroed object, a constant pair and two one-byte objects, each registered for destruction). They sit between TUs that already have their own initializer (around d_bg_s_lin_chk, d_cam_param x3, d_com_inf_game, d_event_data, d_grass, d_meter, d_path, d_s_open, f_op_view x2, f_pc_creator, f_pc_method_tag, m_Do_graphic); probably data-only TUs or TUs whose functions were all inlined into their callers.

### small-gap tails of merged units — verified, 2026-10-05
- Structure: the remaining unverified functions next to merged units were their compiler-generated or trivial parts: header static initializers (d_bg_s_acch second initializer, d_item_data, d_kankyo_data, d_material, d_tree, d_wpillar, f_pc_executor, m_Do_mtx), per-TU copies of the sead SafeString / FixedSafeString destructors and termination hooks (d_a_salvage_tbox, d_kyeff, d_material, d_s_menu, d_s_room, d_throwstone, m_Do_graphic), and the HD virtual deleting destructor of the salvage chest actor.
- Structure: d_a_salvage_tbox's actWaitGetItem and actWaitDummy return TRUE as on GameCube; fopCamM_Management, fopCamM_Init and fopScnM_Init stay empty as on GameCube.
- Structure: the door base constructor (dDoor_info_c) is emitted out of line in HD (0x3EC bytes, HD vtable) and called by the door actors' create functions.
- HD-only: m_Do_mtx's initializer also sets up the matrix stack and an HD quaternion stack object; m_Do_graphic has an HD graphics interface table with empty entries (probably the blanking hooks) and two empty graphics hooks called by the scene phases; the periodic heap check called from the main loop every N frames is empty (probably mDoMch_HeapCheckAll compiled out of the retail build).

### hd_sys_02032A34 (HD screen dimming) — verified, 2026-10-05
- HD-only: keeps the Wii U screen-dimming (burn-in protection) setting in sync with the system setting and switches it on/off for the game.

### hd_sys_02032D0C (HD error viewer) — verified, 2026-10-05
- HD-only: the Wii U error viewer task: watches for disc-read, storage and file-system errors, queues error codes, shows them with the system error viewer using the GamePad/controller input, pauses sound and vibration meanwhile, offers jumps to the account or system settings, and resumes once the error is resolved.

### hd_font_mgr (HD font resource manager) — verified, 2026-10-05
- HD-only: replaces the GameCube JUTResFont/ROM font setup. Seven fonts are loaded as BFFNT files from per-font archives (main, message, ruby, Zelda, pictograph, a large main font) plus the Cafe system font, which is taken from a system object instead of being loaded.
- HD-only: each font gets an alternate (fallback) character, and its glyph cell width/height are cached per font with a per-font scale (initially 1.0); the fonts are registered with the layout system under their file names.
- Structure: singleton with a sead disposer; archive and file names come from two function-local static string tables.

### hd_sys_0203E9EC (HD root task) — verified, 2026-10-05
- HD-only: the game's root task: configures the Wii U controllers (Wii Remote/Pro Controller support), opens the four content archives and combines them, mounts the save and account storage, creates the large manager heaps (sound, save, Miiverse, picture, UI graphics with the built-in font/layout shaders, file loading), then starts the resource, sound and GPU child tasks and afterwards the system and profile tasks.

### hd_sys_0203E584 (HD root heaps) — verified, 2026-10-05
- HD-only: thirteen fixed-size memory pools carved from the Wii U root heap at start-up (sound, save data, Miiverse, picture/Tingle Bottle, resources, graphics and other system areas), with accessors used by the system and root tasks.

### hd_sys_0203E1D0 (HD profile task) — verified, 2026-10-05
- HD-only: a small single-instance "profile" task started by the root task next to the system task (probably the performance/profiling overlay hook; it only forwards to the generic task update), plus shared helpers placed in front of it: the destructor of the Miiverse operation manager's table of 50 downloaded-post records and a generic callback (delegate) invoker.

### hd_sys_02035444 (HD task factory) — verified, 2026-10-05
- HD-only: the factory that creates one of the Wii U framework's root tasks; no gameplay effect.

### hd_sinit_02035FD8 (initialiser-only TU) — verified, 2026-10-05
- HD-only header statics; no behaviour.

### hd_text_conv (HD button-glyph codes in messages) — verified, 2026-10-05
- HD-only: message control characters are mapped to private-use glyph codes of the HD fonts; the glyphs for the button/stick placeholders are chosen from two variant sets by values kept in the HD message manager, probably the controller type in use (GamePad / Pro Controller layouts).

### hd_sys_0203400C (HD exception handler / panic console) — verified, 2026-10-05
- HD-only: installs a handler for crashes (DSI/ISI/program exceptions) that prints registered debug dumps to the console, and a panic(file, line, message) report; no effect on normal gameplay.

### hd_sys_02034B14 (HD ProcUI game framework) — verified, 2026-10-05
- HD-only: the Wii U application framework: ProcUI (HOME menu / background / exit) handling, re-enabling TV and GamePad output on returning to the foreground, the main frame loop and the clean shutdown.

### hd_sys_020355A4 (HD GameTask) — verified, 2026-10-05
- HD-only: the game's top-level Wii U task: creates the input/UI managers, counts HOME/system button holds, and runs the per-frame subsystem updates around the (GameCube-derived) game scene.

### hd_sys_02035B88 (HD HOME button menu) — verified, 2026-10-05
- HD-only: allows or blocks the Wii U HOME button menu, and saves/restores the screen-dimming setting when the game goes to the background.

### hd_msg_res_mgr (HD message resource manager) — verified, 2026-10-05
- HD-only: replaces the GameCube BMG message archives. Messages are MSBT files grouped in message sets listed by an MSBP project; each set is loaded per language from per-region/per-language message archives (the language chosen from the system setting).
- HD-only: lookups go by set name and label; labels that start with a digit (numeric message ids) are searched in the numbered message sets; a reverse lookup finds the numeric id of a message by its attribute id; a separate set holds unit and ruby strings.
- HD-only: layout text boxes can carry a "copy" tag that redirects the box to the label of another text pane, either in the same layout or in another layout.
- Structure: singleton with a sead disposer; up to 256 message sets.

### hd_msg_text (HD message text state) — verified, 2026-10-05
- HD-only: the per-message text state used while laying out and drawing a message: set name and label (a message number is turned into a five-digit label), the MSBT entry, a sound position, up to five line ends and widths, font and scale state, flags for waits, choices and ruby.
- HD-only: entries of one message kind are drawn with the pictograph font unless a system flag disables it; the message's text style comes from the MSBT attribute through the project's style table.
- Structure: several restart/clear variants that keep the message but reset the layout or font state.

### hd_sys_setting (HD system setting) — verified, 2026-10-05
- HD-only: a console setting read at start-up (three accepted values; anything else is treated as the first; a failed read leaves the setting marked unavailable), mirrored into a save-options byte. The message manager uses the value as its language id, so it is probably the console language/region setting.
- Structure: singleton with a sead disposer.

### hd_olv_0203B620 (HD Miiverse operation manager) — verified, 2026-10-05
- HD-only: the Miiverse network side of the Tingle Bottle: connects to the network and initialises Miiverse on a worker thread, posts a bottle message (optionally with a Picto Box/album screenshot) through the system post applet, sends "Yeah!" empathy, opens the Miiverse portal, periodically (every 5 minutes) downloads posts into bottles (requires the Tingle Bottle item), and reports errors to the error viewer.

### hd_olv_02038E24 (HD Miiverse comment manager) — verified, 2026-10-05
- HD-only: holds the Tingle Bottle Miiverse messages: up to 10 shown and 10 downloaded posts (name, topic, text or handwritten memo texture, screenshot data, counts), rotates downloaded bottles into the shown set, tracks new comments for the notification badge, picks the Miiverse topic of the current place from the stage, and hands the texts to the message system.

### hd_net_02038498 (HD SpotPass manager) — verified, 2026-10-05
- HD-only: registers the game's SpotPass background-download task on a worker thread, filling the task settings from 21 save-derived values, and re-registers it when it already exists.

### hd_olv_02036E58 (HD Miiverse bottle spots) — verified, 2026-10-05
- HD-only: places the floating Miiverse bottles: per island room a list of fixed spots read from the game archive (a random quarter of them, limited by the number of downloaded posts), and on the open sea a random position around Link inside his current sea square that has no ground below and is clear of obstacles; a debug view shows the placed bottles.

### hd_olv_02036A64 (HD JPEG encoder) — verified, 2026-10-05
- HD-only: wraps the system JPEG encoder for Miiverse screenshots (800x450 at full quality by default) and halves the quality until the picture fits the Miiverse size limit.

### hd_olv_02036084 (HD JPEG encode manager) — verified, 2026-10-05
- HD-only: a worker thread that either stores a picture into the save's picture album or decodes a downloaded Miiverse screenshot (JPEG) into a texture for display in the bottle message.

### hd_state_task (HD shared-font loading task) — verified, 2026-10-05
- HD-only: a background task with its own worker thread and message queue (states Wait, InProgress, Finish). It reads the console's shared data (probably the shared system font) and is driven by an update message; the main loop only steps it while the worker is idle. The font manager takes its Cafe system font from this object.
- Structure: singleton with a sead disposer; three state ids built by the static initialiser.

### hd_text_tag (HD message tag processor base) — verified, 2026-10-05
- HD-only: tag handling while drawing message text with the HD fonts: a font tag switches the font (a special value returns to the previous one) and sets the writer scale so that the text has the requested size in percent of the font's cell; a scale tag scales the saved scale by a percentage; an absolute scale tag; a pictograph tag advances the line by the pictograph glyph's width.
- Structure: a small tag-state object (saved scale and font) shared with the ruby text processor (hd_text_ruby).

### c_* leftovers (SComponent link-order gaps 0200752C..0201A3E4) — verified, 2026-10-05
- Structure: HD links one translation unit per SComponent header in name order, so several header-only units that have no source file on GameCube exist as real units in HD. Most contain nothing but a static initializer for shared header statics (18 such units, mostly unnamed; by position probably c_API, c_bg_s_poly_info, c_bg_s_shdw_draw and c_rnd among them).
- Structure: c_m3d_g_lin is a real HD unit (02018780..0201890C): the line-segment class's constructor, both start/end setters, the end setter and the interpolated-position helper are emitted out of line there instead of being inlined everywhere as on GameCube. The 2D circle class probably has its own unit (c_m2d_g_cir) holding its default constructor.
- HD-only: an unnamed unit between c_list_iter and c_m2d holds a small three-word class and builds one zeroed static instance of it at start-up.

### c_m3d_g_cps (capsule geometry class) — verified, 2026-10-05
- Structure: on GameCube the capsule class is header-only (all of its methods are inline); HD has a real translation unit for it (02018150..0201824C), like c_m3d_g_lin: the constructor, the deleting destructor, both Set variants (from start/end/radius and from a capsule shape record), the copy from another capsule and the usual header static initializer are emitted out of line.
- Structure: the HD constructor also sets the radius from a constant, where the GameCube constructor is empty; the class keeps its 0x20-byte size, with the vtable at 0x18 and the radius at 0x1C.

### hd_text_unit (HD message text builder) — verified, 2026-10-05
- HD-only: replaces the GameCube message control-code handling. The MSBT text with its tags is expanded into the text-box string: font, colour (palette rows chosen by the message kind), scale and pane-style tags are re-emitted as layout tags; wait, input-wait, speed, choice, ruby and capital-letter tags set the message state; sound tags play at the message's sound position with the room's reverb.
- HD-only: inserted values are written as text: the player's name (a default name when empty), numbers in ASCII or full-width digits with padding, counters with a singular or plural unit label and optional ruby (sword-game blows, letters, rupees, pendants, bombs, seeds, necklaces, Chu jelly, feathers, crests and others), times as minutes and seconds with a per-language layout, and whole messages inserted by number.
- HD-only: line breaking against the box's line limit (with an extra line for a few specific messages), removal of the breaks between lines, and a character replacement table for one font; a "line start" tag is inserted once the first visible character is reached.
- Gameplay: probably the same visible text as the GameCube for the same messages; the singular/plural units and the time layout per language are new in HD.
- Structure: the counters' unit and ruby labels are function-local static strings registered for destruction at exit (the 23 destroy helpers at the start of the TU).

### d_kankyo_rain (weather: rain, snow, ash, spores, poison mist, clouds, stars, sun/moon, lens flare, waves, thunder) — verified, 2026-10-05
- Structure: the whole translation unit (48 functions, 02563DD8..025776F8) is verified as one set. It has an extra HD-only projection helper at its head (it copies a constant parameter template, replaces two values with current environment values and hands the result to the rain renderers) and keeps its own out-of-line colour, gamma and matrix-copy helpers.
- Graphics: every weather renderer (rain streaks, spray, poison mist, spores, ash/snow, stars, waves, cloud shadows, sky clouds, sun/moon and lens flare) builds compact quads in double-buffered GPU vertex buffers with a per-particle material/uniform block, instead of sending immediate-mode vertices as on GameCube; the buffers are cleared one cache line at a time before each quad is written. Colours go through a float conversion (and a gamma step for some) before upload.
- Graphics: HD spray uses a slightly different blue and offsets the camera's vertical direction before fading; the spore renderer runs a second, darkened pass that fades particles around the player's height; cloud-shadow drawing packs only the emitted quads.
- Graphics: the sky-cloud renderer (02575B6C) always runs a single pass with three texture layers; the GameCube pass that depended on sun visibility or on aiming the Picto Box is never selected. Each layer keeps 100 particles.
- Graphics: lens flare: HD prepares sixteen triangular flare rays and nine textured quads in packet-owned buffers, where the GameCube draws eight flare quads and a separate sixteen-part fan directly through GX; colours, scales, materials and view transforms come from the weather packets instead of the old function arguments, with a shaped distance fade.
- Graphics: sun and moon are built as expanded quads in separate double buffers with two colour/size passes each; the moon is oriented by camera-relative angles with weekday texture selection, and the sun size includes the lens-distance falloff.
- Graphics: in one special stage, airborne ash is drawn as four offset copies (the GameCube uses eight) with separate faded ground reflections and a smaller single copy for settled particles; other stages use snow-style quads.
- Gameplay: the poison-mist room table differs from GameCube: pattern 0 requests 600 particles and pattern 3 requests 700, pattern 2 has a horizontal radius of 1600, and pattern 3 has a different base height and size; the room selector can be overridden by an environment byte. The mist's circular fade uses a fifth power of the normalised radius where the GameCube uses the sixteenth, so the mist edge probably looks softer.
- Fix: the poison mist's negative-z wrap now writes the z coordinate (GameCube wrote x).
- Gameplay: sky-cloud motion (0256DDF8) skips the interior wind remapping in the stage "Name", applies the "Siren" sea-level override in rooms 17 and 18 (GameCube: room 17 only; the HD value is +14101 where the GameCube source has -14101, effect not checked), adds an HD-only height override of 48000 for the "ADMumi" stage and advances a cloud-packet value by 100 each update; its initialization falls straight through into wrap and motion handling in the same update.
- Structure: cloud-shadow motion (0256BB6C): state 0 falls through from initialization into movement, whereas the GameCube case ends with a break; the fade stores alpha directly instead of easing it, and the size reads the current view's field and caps its ratio at 1. No visual or gameplay effect is inferred.
- Structure: the wind entry also contains the seabird update (thirty line slots and two bird slots); its line speed depends on the camera displacement, and custom wind changes the spawn range, vertical offset and phase advance. Trigonometry uses an interleaved sine/cosine table.
- Structure: the volcanic-ash mover keeps drifting ash and the settling-particle update in one entry (fifty settling slots visited in reverse) with a per-particle rotation phase and a distance fade; the drifting-spore (housi) mover uses 80-byte records, a separate packet fade and a combination of global wind, three sine phases and decaying point-wind velocity.
- Structure: the wave mover contains sea-level handling for six stage cases, interior wind-angle overrides and three shoreline exclusion regions.
- Structure: lens-flare motion copies the sun position into three initial slots and advances six further positions; snow setup selects stage-specific heap, archive and shader resources; thunder keeps near and far flash states, and its flash colour is kept as three 16-bit channels. Camera-relative weather placement uses explicit fused multiply-adds, which the reconstruction keeps.

### d_scope (telescope message process) — verified, 2026-10-05
- Removed: the GameCube telescope screen (scope overlay, button icons, wipe animation, message panes and their processing) is gone from this unit; the process profile only keeps five constant methods (create reports complete, the others report success). Probably the HD telescope (GamePad/gyro, see d_a_npc_ls1) is drawn by other code.
- Structure: the unit is only those five methods plus the standard per-unit static initialiser.

### d_cc_uty (collision attack utilities) — verified, 2026-10-05
- Structure: the HD unit (02518B28..02519814) holds the player cut-bit lookup, the normal attack sound lookup, two defence-sound wrappers, attack power classification, hit handling and the TU initializer. The GameCube critical-sound lookup has no separate HD function; its selection is inlined into hit handling.
- Structure: the attack record keeps the GameCube field positions and 0x1C-byte size; player state is read through the play singleton, and actor fields use HD offsets. Attack classification still distinguishes water, wind, fire, hammer, sword, bomb, rope, boomerang, arrow and the remaining actor-specific cases.
- Gameplay: hit handling has an additional damage subtraction when Link's cut type is the turn/roll type and a player byte (+0x69E8) is set; this condition is absent from the GameCube source (purpose not established).
- Structure: the normal sound lookup asserts on a missing collision-info result and then returns the default sound.
- Graphics: the hit particles go through the HD particle controller, keeping both particle ids and their arguments.

### d_wood (tree and bush helper) — verified, 2026-10-05
- Graphics: rendering differs substantially from GameCube. HD builds fixed arrays of units, animations and room membership, manages double-buffered vertex storage with separate normal and fading instances, and binds shaders, uniform blocks, texture transforms and indexed draws through the HD graphics APIs instead of GameCube display lists.
- Structure: animation state, unit placement, ground checks, collision responses, room membership and the packet update keep their GameCube roles. The HD unit has 52 functions, including the initializer and the inline graphics constructors and cleanup companions referenced by the packet arrays; the HD lighting object is larger and has embedded components.

### d_a_lod_bg (distant island models, LOD) — verified, 2026-10-05
- Structure: the GameCube actor mounts the shared LOD archive itself through a DVD command, copies the model binaries into its own expanded heap and builds solid heaps per model; HD loads the LOD models through the normal resource system by archive name (load request, sync, delete) and keeps the name in a sead string object; the shared local heap and the DVD mount state are gone.
- Graphics: distant island models switch on and off at once (alpha 0 or 255) instead of fading in and out by 16 per frame as on GameCube.
- Graphics: both distance limits are 100,000 units larger than on GameCube, and the show/hide distance is measured from the camera position (as in the European GameCube version), so the distant island models stay visible much further out (probably for the HD draw distance).
- Gameplay: besides Windfall's lighthouse beams and the Forsaken Fortress second model, HD also gives Dragon Roost Island a second distant model, loaded until a story event flag (0x3908) is set and drawn with its own lighting.
- Graphics: the draw also converts the material colour to floating point and writes it into the HD material's colour block.
