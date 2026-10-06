# Review notes

Findings that deserve a later review: behaviour we explain but have not fully proven, original-engine
undefined behaviour the candidates cannot reproduce, and harness artefacts. One entry per item:

- **Where**: addresses / units / files
- **Known**: what the evidence shows
- **Assumed / unproven**: what we rely on without proof
- **How to check later**: the experiment or test that would settle it

---

## 1. Link joint-save rotation bytes (game-test finding 2, 2026-10-04)

- **Where**: 025E410C (m_pbCalc calc method, vtable 10058C98, created by 025E3FB4);
  daPy_jointBeforeCallback / jointBeforeCB 023D6B30 (d_a_player_main #01); Link actor (process 0xA8)
  joint-save records at 0x6D50 + jnt*0x20 for joints 2, 31, 36: rotation fields 0x6D9C/9E/A0,
  0x713C/3E/40, 0x71DC/DE/E0.
- **Known**: 025E410C builds a J3DTransformInfo on its stack at SP+8; it writes scale (SP+8..0x10) and
  translation (SP+0x1C..0x24) but never the rotation s16s (SP+0x14..0x19) or the padding, and passes the
  info as r5 to `this->0xB0` (daPy_jointBeforeCallback 023D7194). jointBeforeCB copies the whole info,
  rotation included, into the joint-save records of exactly those three joints. So the original engine
  stores uninitialised stack bytes there. Swapping in setLegAngle or footBgCheck (d_a_player_main #03)
  only changes which leftover data sits at that stack depth; both match the disassembly line by line and
  pass 10k at seeds 1/7. Original-engine UB, not a decompilation bug.
- **Assumed / unproven**: nothing reads those Euler rotation fields; drawing uses the quaternion
  (jointAfterCB copies the values back into the persistent transform-info array, 025E4944). Game test:
  Link trace, camera, RNG, sound and save identical.
- **How to check later**: game-test read-watch on the three rotation fields (and the persistent array they
  are copied to) across scenarios; any read outside jointBefore/AfterCB would make this a real difference.
- **Game-test digest mask**: `A8:6D9C:6, A8:713C:6, A8:71DC:6`.

## 2. daWarphr_c::draw uninitialised stack on the camera==NULL path

- **Where**: d_a_warphr 024DAA1C (daWarphr_c::draw), and draw_wrapper 024DAC58; projection emitter
  matrix at +0x3D0. Annotated in the source.
- **Known**: when the demo camera getter 025283F8 returns NULL, the original still uses its eye cXyz
  (SP+8..0x10) and angle.y (SP+0x22), which were never written — the same in the GameCube source
  (sp1C/sp08 uninitialised when dComIfGp_demo_getCamera() is NULL). The candidate's gabi::Locals start
  zeroed, so it builds an identity translation / yaw 0.
- **Assumed / unproven**: visual only (the projection emitter's matrix in Hyrule); Link, camera, RNG,
  sound, save identical in the game test.
- **How to check later**: game-test frame comparison of the warp projection effect with camera == NULL;
  confirm nothing else reads +0x3D0.
- **Game-test**: masked in the digest.

## 3. footBgCheck -rec replay mismatch

- **Where**: d_a_player_main #03 footBgCheck (d_a_player_main_03.cpp), recorded-call replay
  (`verify.py -rec`) at cLib_addCalcAngleS, call #19: r4 0DA8 vs 1716.
- **Known**: the generated-input verification of footBgCheck passes 10k at seeds 1/7 and the code matches
  the original line by line. The original keeps its upper/lower s16 arrays adjacent on the stack
  (SP+0x44 / SP+0x48); the candidate uses separate gabi::Locals.
- **Assumed / unproven**: the mismatch is a replay-mapping artefact (recorded stack-pointer patches mapped
  to the wrong candidate Local because the original's two arrays are adjacent), not a candidate bug.
- **How to check later**: replay with a candidate that places upper/lower in one 8-byte Local at the
  original's relative offsets; if the mismatch disappears, it is the mapping. Alternatively extend the
  replay patch mapping to resolve the best-matching argument pointer per byte range.

## 4. Oship mutant 3282: particle setter vs actor+0x5C0 increment order

- **Where**: d_a_oship CheckTargetHit 023C714C; smoke particle setter 025A847C (selector 0, ID 0x3E1, fixed follow
  callback vtable 100523C8) → 02821448, 0281E958 (emitter create, resource virtual getters), 02829F10 (texture init);
  actor+0x5C0..5C3 (smoke count).
- **Known**: the setter's actor-owned arguments (callback 548+20*count, rotation
  5A8+6*count, position E24, scale 330) are disjoint from 5C0..5C3; the setter copies the position into its own frame
  and passes no actor pointer to the SDK; follow-callback slots 3C/44 touch only callback fields; ID 0x3E1 skips the
  environment-colour helper.
- **Assumed / unproven** (2026-10-04): the particle SDK's manager/pool/resource code and the texture
  initialisation never read or write actor+0x5C0 during this synchronous call, so swapping the setter with the
  increment is unobservable; mutant 3282 counted as equivalent.
- **How to check later**: game-test read/write watchpoint on the Oship actor's +0x5C0..5C3 while 025A847C runs
  (any hit falsifies the contract).

## 5. npc_bj1 "probably equivalent" survivors

- **Where**: d_a_npc_bj1.
- **Assumed / unproven**: (a) the 0x10016C08 → 0x10016C09 literal mutant is equivalent assuming the byte after that
  0.0 constant is zero; (b) a dropped reload of the morph pointer (+0x44C) is equivalent assuming no callee changes it.
- **How to check later**: (a) read the image byte at 0x10016C0C..; (b) check the callees between the two loads for
  writes to +0x44C (or a game-test write watch).

## 6. m_Do_graphic mutant 29: blur-byte reset vs SDK buffer resize 027F0148

- **Where**: m_Do_graphic; SDK buffer resize 027F0148 using SDK statics
  101F9868/6C/70 and the standard heap selected at 104A244C.
- **Assumed / unproven** (2026-10-04): no allocator reached through the standard heap reads or writes
  the blur byte during the resize, so moving the reset across the call is unobservable; mutant 29 counted equivalent.
- **How to check later**: game-test read/write watch on the blur byte while 027F0148 runs.

## 7. Inferred TU attribution: m_Do_DVDError (025E2114) and m_Do_Reset (025F295C)

- **Where**: two standalone static initializers verified with the player-npc unit.
- **Assumed / unproven** (2026-10-04): the TU names come from HD adjacency only (025E2114 follows the
  m_Do_audio companions 025E20F0/20F8/2100; 025F295C follows the printf initializer 025F28C8 and precedes vibration
  025F29F0). Behaviour is verified; only the file names are inferred.
- **How to check later**: any symbol, assert string or RTTI reference tying either initializer to a source file;
  if it contradicts, rename the files (no re-verification needed).

## 8. d_wpot_water mutation 50: early zero of the water-callback object before its initializer

- **Where**: d_wpot_water; static object 104873AC, initializer 025D41F0 (assigns vtable
  10056DC0), SDK bootstrap 028ED6F8/028EAB2C, registration leaf 028F026C (head 104DA130).
- **Assumed / unproven** (2026-10-05): nothing reads 104873AC+0xAC between the static zero loop and the
  initializer, so the mutant's extra early zero (later overwritten) is unobservable; counted equivalent.
- **How to check later**: game-test read watch on 104873AC..+0xB0 from boot until 025D41F0 runs.

## 9. Demo00 mutations 691/777/779 and 939/991: pointer reloads across SDK calls

- **Where**: d_a_demo00; ModelCreate 025E38E0 (constructor 027F3E04, setup 027F4404);
  lighting 02562F5C; string callbacks 02122914; actor+0x3FC (auxiliary pointer), actor+0x3F4 (model pointer),
  actor Tev packet actor+0x110..0x2D8.
- **Assumed / unproven** (2026-10-05): ModelCreate never writes actor+0x3FC (it never receives the actor);
  lighting writes at most the supplied model/material graph and the Tev packet (0x1C8 bytes), not actor+0x3F4.
  The reordered/dropped reloads are therefore equivalent.
- **How to check later**: game-test write watches on actor+0x3FC across 025E38E0 and on actor+0x3F4 across 02562F5C.

## 10. d_a_st (ST) sample survivors 1343/1344 (unclassified) and 1590 (scoped equivalence)

- **Where**: d_a_st; the bounded 126-mutant sample:
  111 detected, 11 scoped source-equivalent, open 1343/1344/1590 (+ optional fixture 6360).
- **Assumed / unproven**: 1343/1344 were never classified (their preparation was stopped by an automatic review and
  not retried); 1590 is accepted on a scoped equivalence reason without a precise semantic observer.
- **How to check later**: re-run the three mutants at 10k with the `mockfield` fixtures for 02518DB0/025192A8
  and classify each as killed or equivalent with a one-line reason.

## 11. d_ovlp_fade4 mutation 28: temporary scene+0x34A value before the play-singleton getter

- **Where**: d_ovlp_fade4 constructor; play singleton 104753A8, getter 025200D4 (returns 1046F0B0).
- **Assumed / unproven** (2026-10-05): fade4 is only constructed after the play singleton exists, so
  025200D4 takes its no-callee path and nothing reads scene+0x34A before it is overwritten; mutant 28 equivalent.
- **How to check later**: game-test assert that 104753A8 != 0 whenever the fade4 constructor runs.

## 12. d_a_bwd Move2922: m18CC[0]=0x96 store vs anm_init(0x10, …) order in s_fly

- **Where**: d_a_bwd s_fly; anm_init reads mpHeadMorf (+0x3DC) and calls
  getObjectRes and mDoExt_McaMorf::setAnm; bwd_class+0x1B50 (m18CC[0]).
- **Assumed / unproven** (2026-10-05): the morf, the archive resources and the sound/BAS objects reached
  by anm_init never alias bwd_class+0x1B50 (object separation), so the swap is unobservable; counted equivalent.
- **How to check later**: game-test write watch on bwd+0x1B50 across anm_init in s_fly.

## 13. Harness limit: OSBlockMove from a stack temporary compares only 4 source bytes

- **Where**: hd_text_unit: tag buffers built on the stack and copied with OSBlockMove (C0009988).
- **Assumed / unproven**: only the first 4 bytes of the stack source (default r4 pointee) are compared at the call; later
  words of the copied tag are not checked (mutating tag words +6/+8 survives; +0 is caught). One mutation survivor in
  hd_text_unit is classified under this limit.
- **How to check later**: add `callee C0009988 r4=<size>` facts (or a size-from-r5 pointee rule for memmove-like imports)
  and re-run the hd_text_unit mutation sample.

## 14. Demo00 contract extension: getRes 02606900 does not write actor+0x3FC

- **Where**: d_a_demo00 createHeap 0211F35C, source line ~405: reload order of
  namedDemoResource vs `actor->auxiliary` (actor+0x3FC) around dRes_control_c::getRes 02606900 (merged d_resorce unit).
- **Assumed / unproven** (2026-10-05, extends #9 group 691/777/779): getRes only reads the resource
  control tables and returns a resource pointer; it never receives the actor, so actor+0x3FC is unchanged across it;
  the swap is equivalent.
- **How to check later**: game-test write watch on actor+0x3FC across 02606900 in Demo00 createHeap.
