/**
 * true60.h (WWHD, true 60 build): the true 60 Hz timebase as the game code uses it (dt = 1/N).
 *
 * The step state itself is gabi_t60.h's (dt-1 check): t60::dt (g_dt), t60::tick30 (g_tick30), t60::steps (N), read
 * from the globals wwhd_t60_dt / wwhd_t60_tick30 / wwhd_t60_steps. In the true60 game build the port's step driver
 * (decomp build only: runtime/src) sets them; the verification harness pins them to 1 / 1 / 1 (or steps
 * them with -t60steps); GABI_T60=0 makes them constants.
 *
 * Meaning in the game:
 *   steps   N of the running 30 Hz frame: 1 (the original game; also the 30 Hz fallback during events, messages,
 *             menus, pauses and scene changes), 2 (60 Hz) or 4 (120 Hz).
 *   tick30  the step that ends a 30 Hz frame. The 30 Hz world runs on it: every unconverted process (with dt 1), the
 *             counters, audio, the scene's draw world block, particle calc. Always true at N = 1.
 *   dt      the step length the running code advances by: 1 everywhere except in converted code (a converted
 *             process's execute, a converted system between t60_set_dt(t60_step_dt) and putting the previous value
 *             back), where it is 1/N. So unconverted code reaching a shared primitive keeps the original arithmetic.
 *
 * This header adds the per-process rate gate's side table (fpcEx_Execute) and the dt switch for converted contexts; the
 * game-build-only parts (the fallback flag, the step's raw input) exist only with GABI_GAME_BUILD.
 */
#pragma once
#include "gabi_t60.h"

/* 1/N: what a converted context sets g_dt to */
inline float t60_step_dt() { return 1.0f / (float)t60::steps(); }

/* (the 60 Hz counter rule): the step counter for converted code. g_Counter.mCounter0 stays the 30 Hz frame counter (its phase
 * masks are 30 Hz decisions everywhere; cCt_Counter runs on tick30 only). This one advances by one per logic step,
 * after the step's process executes (fapGm_Execute), so at N steps per frame it advances N per 30 Hz frame and equals
 * the frame count at N = 1 (counted from boot, not reset with mCounter1). Converted code that needs a per-step phase
 * (alternating sub-steps, a 60 Hz blink) reads it; a decision "every k Wii U frames" keeps mCounter0 on tick30. Host
 * memory only, one definition for the whole program: the port's runtime (runtime/src) in the game build,
 * the harness (src/harness.cpp) in verify runs (never compared). Not an inline variable: the fast decomp build makes a
 * unit's hidden definitions local (ld -r + localize, unit_ir), so each unit would count on its own copy and only
 * f_ap_game's would ever advance (fixed 2026-11-14). Rule for this header: state shared between units is a
 * port-owned extern "C" global, never an inline or static variable (constant tables such as kT60Converted are fine). */
extern "C" {
__attribute__((visibility("default"))) extern uint32_t wwhd_t60_step_counter;
/* set latch (c_damagereaction.h t60dr_mark_set / t60dr_pending): a counter set on an in-between step of 30 Hz
 * frame `frame` (key: its guest address) whose phase waits for the next frame. Port-owned host map (no capacity limit;
 * the harness has its own for verify runs) so every enemy unit and c_damagereaction share one latch. */
__attribute__((visibility("default"))) void wwhd_t60_latch_set(uint32_t key, uint32_t frame);
/* 1 when key was set in `frame`; an older frame's entry is dropped (0) */
__attribute__((visibility("default"))) int wwhd_t60_latch_pending(uint32_t key, uint32_t frame);
}
inline uint32_t t60_step_counter() { return wwhd_t60_step_counter; }
/* (c_lib integer smoothing): use of the host table that holds each smoothed integer's frame
 * start. wwhd_t60_ismooth_peak: the most values that began a frame entry within one 30 Hz frame; wwhd_t60_ismooth_overflow:
 * entries that found no table slot within the probe limit and went to the exact overflow map (never the whole frame's
 * step on every sub-step). Port-owned (runtime/src/ logs them; the harness defines them for verify runs). */
extern "C" {
__attribute__((visibility("default"))) extern uint32_t wwhd_t60_ismooth_peak;
__attribute__((visibility("default"))) extern uint32_t wwhd_t60_ismooth_overflow;
}
inline void t60_step_counter_advance() { wwhd_t60_step_counter++; }

/* (the per-step smoothing rule): a per-frame smoothing ratio r for a step of dt original frames, 1 - (1 - r)^dt (dt steps of it
 * compound to one step of r); 0 < r < 1 only, other ratios (0, a jump to the target, odd values) stay as they are */
inline float t60_ratio(float r, float dt) {
    if (!(r > 0.0f) || !(r < 1.0f)) return r;
    return (float)(1.0 - __builtin_pow(1.0 - (double)r, (double)dt));
}

/* (the per-step chance rule): a per-step chance. A chance p per 30 Hz frame, drawn on every step of length dt, becomes
 * 1 - (1 - p)^dt (N draws at dt = 1/N fire at least once with probability p); dt = 1 returns p itself (bit-exact).
 * Use: `if (cM_rnd() < 0.02f)` -> `if (cM_rnd() < t60_chance(0.02f))` (or cM_rndChance, c_math_t60.h);
 * `if (cM_rndF(100.0f) < 30.0f)` -> `if (cM_rndF(100.0f) < t60_chance_in(30.0f, 100.0f))`. Per-event draws (a value
 * picked once when something starts) stay as they are; the RNG call count changes with N (sequences diverge, the per-step chance rule). */
inline float t60_chance_dt(float p, float dt) { return dt == 1.0f ? p : t60_ratio(p, dt); }
inline float t60_chance(float p) { return t60_chance_dt(p, t60::dt()); }
/* the threshold of `rndF(range) < threshold` (a chance threshold/range) for this step */
inline float t60_chance_in_dt(float threshold, float range, float dt) {
    if (dt == 1.0f || !(range > 0.0f)) return threshold;
    return range * t60_ratio(threshold / range, dt);
}
inline float t60_chance_in(float threshold, float range) { return t60_chance_in_dt(threshold, range, t60::dt()); }

#if GABI_T60
/* a converted context sets g_dt and puts the returned previous value back when it ends (plain calls, no scope object:
 * the source glue rewrites local objects) */
inline float t60_set_dt(float dt) {
    float previous = wwhd_t60_dt;
    wwhd_t60_dt = dt;
    return previous;
}
#else
inline float t60_set_dt(float) { return 1.0f; }
#endif

#if defined(GABI_GAME_BUILD) && GABI_T60
/* the port (runtime/src), game build only */
extern "C" {
/* the 30 Hz fallback's input: the play scene's execute sets it on a tick30 step when nothing holds the world at 30 (no
 * event, message, menu, pause, hitstop or stage change); the step driver takes it at the end of each 30 Hz frame and
 * clears it, and runs the next frame at N only when it was set */
__attribute__((visibility("default"))) extern int wwhd_t60_world60;
/* tests (WWHD_T60_TEST_CONVERT): -1 every process counts as converted, -2 the WWHD_T60_TEST_CONVERT_FROM window is not
 * open yet (nothing converted), else the first profile name of the list (0: none) */
__attribute__((visibility("default"))) extern int wwhd_t60_test_convert;
/* tests: this profile counts as converted (WWHD_T60_TEST_CONVERT=all or a comma list of profile names, e.g. "168,0x1DC":
 * Link and the camera together) */
__attribute__((visibility("default"))) int wwhd_t60_test_converted(int profName);
/* tests (WWHD_T60_TEST_UNCONVERT=<list>, no-conversion controls, the own-state proof rule): the first profile name of the list (0:
 * unset / empty, the normal case), and whether a profile is excluded: t60_converted returns false for it, kT60Converted
 * and WWHD_T60_TEST_CONVERT included (a control run of a MARKED actor without its conversion; port t60.cpp, read once) */
__attribute__((visibility("default"))) extern int wwhd_t60_test_unconvert;
__attribute__((visibility("default"))) int wwhd_t60_test_unconverted(int profName);
/* the rate gate's verdict on the last execute it was asked for: its dt, 0 when it held the process back (the port's
 * traces record executes of converted processes on in-between steps by it) */
__attribute__((visibility("default"))) extern float wwhd_t60_ran;
/* the step's raw GamePad input, polled every step (converted code; the game's own controller sees button changes on
 * tick30 steps only, each press latched once) */
struct T60Pad {
    unsigned hold, trig;
    float stick[4]; /* left x, y, right x, y */
};
__attribute__((visibility("default"))) extern T60Pad wwhd_t60_pad;
}
#endif

/* (rule 14, coupled parties): a converted process can ask to run as an unconverted one (tick30 step only,
 * dt 1, drawn interpolated) while it is coupled to an unconverted party that moves it once per 30 Hz frame: Link
 * standing on a moving background or riding the ship (dBgS::MoveBgCrrPos carries him by the carrier's whole frame
 * move once per frame; at N steps he would move in a 30 Hz staircase against his carrier). The request is made on a
 * step (each step it still applies) and takes effect from the next 30 Hz frame on: within a frame a process runs
 * either all its steps or the tick30 step only, so its state stays on the step clock. Host memory only, game build
 * only (the verification harness: never held). */
#if defined(GABI_GAME_BUILD) && GABI_T60
/* the per-frame hold lives in the port (runtime/src/: one table for every game unit; the fast build makes a
 * unit's hidden definitions local, so an inline variable here would be one copy per unit) */
extern "C" {
__attribute__((visibility("default"))) void wwhd_t60_hold30_request(unsigned procId);
__attribute__((visibility("default"))) int wwhd_t60_held(unsigned procId);
}
inline void t60_hold30_request(unsigned procId) { wwhd_t60_hold30_request(procId); }
inline bool t60_held(unsigned procId) { return wwhd_t60_held(procId) != 0; }
#else
inline void t60_hold30_request(unsigned) {}
inline bool t60_held(unsigned) { return false; }
#endif

/* input (the first-step input rule): the game's controller (sead::ControllerManager::calc) runs
 * at the start of a pass, before the step driver begins the pass's step, so it asks the port whether the pass about to
 * run is its 30 Hz frame's first step: only then it reads the GamePad (once per frame); on the frame's other passes the
 * controller keeps its state (buttons, triggers, sticks), so the tick30 world sees the frame's triggers too. Always true
 * at N = 1 and outside the game build. */
#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" {
__attribute__((visibility("default"))) int wwhd_t60_input_pass();
}
inline bool t60_input_pass() { return wwhd_t60_input_pass() != 0; }
#else
inline bool t60_input_pass() { return true; }
#endif

/* per-step input (true 60 fps input; follow-up of the first-step input rule): the controller reads the GamePad on EVERY logic step (the port
 * says whether the pass about to run is one: t60_input_step; every pass at N = 1), so presses and stick changes between
 * two 30 Hz frame boundaries reach the game at step resolution. The frame form of the read (sead/InputState.cpp): on the
 * frame's first read everything as at 30 Hz; on a later step's read (t60_in_later) the sticks and buttons are the step's,
 * the guest press / release / repeat words collect the frame's edges (their union since the frame's first read: code
 * that runs on the tick30 step only, unconverted, sees every edge of the frame once), and the frame counts (repeat
 * counts, idle frames, error frames) are left alone (they count on the frame's first read: the input rule, 30 Hz frames).
 * Converted code (dt != 1) takes a press / release from the step's own read instead (t60_in_step_word: each press is
 * consumed by exactly one step). Host memory only; game build only (the verification harness: the 30 Hz form, the step
 * words as before: the frame's words on the first step only). */
#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" {
__attribute__((visibility("default"))) int wwhd_t60_input_step();
__attribute__((visibility("default"))) extern unsigned wwhd_t60_in_read;
__attribute__((visibility("default"))) void wwhd_t60_in_edges(unsigned state, unsigned press, unsigned release, unsigned repeat);
__attribute__((visibility("default"))) int wwhd_t60_in_step_word(unsigned state, unsigned offset, unsigned* word);
}
inline bool t60_input_step() { return wwhd_t60_input_step() != 0; }
/* sead::ControllerManager::calc, each pass: read = it reads the GamePad on this pass; later = not its frame's first read */
inline void t60_in_begin(bool read, bool later) { wwhd_t60_in_read = (read ? 1u : 0u) | (read && later ? 2u : 0u); }
inline bool t60_in_later() { return (wwhd_t60_in_read & 2u) != 0; }
inline bool t60_in_read() { return (wwhd_t60_in_read & 1u) != 0; }
inline void t60_in_edges(unsigned state, unsigned press, unsigned release, unsigned repeat) {
    wwhd_t60_in_edges(state, press, release, repeat);
}
/* converted code's word (offset 0 press, 4 release, 8 repeat) of the controller state at guest address state(): the
 * edges of the step's own read (frame(): the guest word, the frame's union, taken when the state was never read). The
 * arguments are functions so that the harness form makes the original accesses only. */
template <class State, class Frame>
inline unsigned t60_in_step_word(State state, unsigned offset, Frame frame) {
    unsigned word;
    if (wwhd_t60_in_step_word(state(), offset, &word)) return word;
    return frame();
}
#else
inline bool t60_input_step() { return true; }
inline void t60_in_begin(bool, bool) {}
inline bool t60_in_later() { return false; }
inline bool t60_in_read() { return true; }
inline void t60_in_edges(unsigned, unsigned, unsigned, unsigned) {}
template <class State, class Frame>
inline unsigned t60_in_step_word(State, unsigned, Frame frame) { return t60::first_step() ? (unsigned)frame() : 0u; }
#endif

/* The converted processes (the per-process rate gate's side table, fpcEx_Execute): profile names (fpcBs mProfName) of
 * processes whose execute is written for the general step length. They execute on every step with g_dt = 1/N; every
 * other process executes on tick30 steps only, with g_dt = 1, and is drawn interpolated. A unit that converts a process
 * adds its profile name here (one line, with the unit ID). */
static constexpr short kT60Converted[] = {
    0x00CE, /* the ChuChu (cc_class; strict keyed-RNG paired runs, the strict RNG-pair rule) */
    0x00AB, /* the ship's sail cloth (daGrid_c; t60_whole form, the whole-frame form rule + addendum; pair equivalent) */
    0x0132, /* the spikes (mtoge; strict keyed-RNG paired-run pair) */
    0x0073, /* the barred door (mbdoor; strict keyed-RNG paired-run pair) */
    0x01C9, /* the floating barrel (obj_barrel2; strict pairs) */
    0x01D1, /* the buoy flag (obj_buoyflag; strict pair) */
    0x0168, /* the fairy (npc_fa1; strict pair) */
    0x00C1, /* the lantern (kantera; strict pair) */
    0x0126, /* the bomb (bomb; strict pair) */
    0x00FF, /* the field item (item; strict pair) */
    0x01C6, /* the stone (stone; strict pair) */
    0x01C5, /* the pot (tsubo; strict pair) */
    0x01C8, /* the Forsaken Fortress barrel (obj_barrel; strict pair) */
    0x014F, /* Grandma (npc_ba1; NPC form, the whole-frame form rule; pair) */
    0x0141, /* Aryll (npc_ls1; NPC form; pairs) */
    0x014B, /* Orca (npc_ob1; NPC form; pairs) */
    0x014C, /* Aj1 (npc_aj1; NPC form; pairs) */
    0x013D, /* Yw1 (npc_yw1; NPC form; pairs) */
    0x013F, /* Joel (npc_ko1; NPC form; pairs) */
    0x0140, /* Zill (npc_ko1; NPC form; pairs) */
    0x013B, /* Mesa (npc_ym1; NPC form + draw latch rule 14; pairs) */
    0x013C, /* Abe (npc_ym1; NPC form; pairs) */
    0x0142, /* pirate P1 (npc_p1; NPC form; pairs pcabin_cv / pcabin_cv2) */
    0x0143, /* pirate P2 (npc_p2; NPC form; 1-frame event transient = 30 Hz visual-motion (2) class; pcabin_cv / pcabin_cv2) */
    0x0118, /* the cafe lamp (cafelmp; per-step turn; strict pair) */
    0x01D2, /* the Tower of the Gods blocks (correct; whole form; strict pair) */
    0x0038, /* the Ice Ring dragon head (dragonhead; whole form; strict pair) */
    0x0021, /* the drifting platform (drift; whole form; strict pair) */
    0x004A, /* the test / water object (mtest Owater; whole form; strict pair) */
    0x0095, /* the hatch (ohatch; whole form; strict pair) */
    0x0054, /* the breakable box (ospbox; whole form; strict pair) */
    0x0040, /* the table (otble; whole form; strict pairs) */
    0x0106, /* the paper (paper; whole form; strict pairs) */
    0x0039, /* the pirate ship (pirateship; whole form; strict pair) */
    0x002D, /* the shelf (shelf; whole form; strict pairs) */
    0x01AA, /* Tetra (npc_zl1; NPC form, the whole-frame form rule; pair tdeck_cv B == tdeck_base B) */
    0x00A5, /* / ship_exec: the ship (daShip; strict pair, crane) */
    0x0026, /* the sea (daSea; strict pair) */
    0x00AC, /* the pirate ship's sail (sail; strict pairs) */
    0x00B2, /* the ghost ship (ghostship; strict pairs) */
    0x00B0, /* the sie flag (sie_flag; strict pairs) */
    0x00AF, /* the tori flag (tori_flag; strict pairs) */
    0x01AF, /* the anchor (ikari; strict pairs) */
    0x00AD, /* the pirate flag (pirate_flag; strict pairs) */
    0x00AE, /* the majuu flag (majuu_flag; strict pairs) */
    0x00A8, /*-*: Link (daPy_lk_c; pairs strict, gd64d) */
    0x01DC, /*-*: the camera (dCamera_c; with Link in pairs, CONV 168,0x1DC) */
    0x00A9, /* the hookshot (hookshot; strict pair, with Link) */
    0x00BD, /** the Bokoblin (bk; pair, recorded rules on) */
    0x00BC, /** the Moblin (mo2; pair, recorded rules on) */
    0x00BF, /* the Darknut (tn; pair, recorded rules on / tn#2) */
    0x0049, /* obj_lpalm (strict keyed paired runs s1/s7 N2/N4 + Link interaction, chain c) */
    0x0053, /* obj_ojtree (strict keyed paired runs s1/s7 N2/N4, chain c) */
    0x0097, /* obj_Ygush00 (strict keyed paired runs s1/s7 N2/N4, chain c) */
    0x01B6, /* vrbox2 (strict keyed paired runs s1/s7 N2/N4, admp PASS, chain c) */
    0x01B7, /* bg (keyed-aid fixed pairs strict, sound + admp PASS, chain j) */
    0x0083, /* obj_ftree (scene pairs, FROM 6, strict admp PASS, chain j) */
    0x0090, /* obj_leaves (Vochi; frame-level gate + tick30-only registration; bomb pair:  =
             * CryZe 6-StartFW COPY at kindan room 5 on the Vochi 0005FFFF, a held bomb blows it: mBlown on A's frame
             * f247, every logic word equal on every frame (tevStr colour LSBs on 2 frames: picture), Link PASS;
             *) */
    0x0187, /* wind_tag (scene pairs, FROM 6, strict admp PASS, chain j) */
    0x01D7, /* tag_waterlevel (scene pairs, FROM 6, strict admp PASS, chain j) */
    0x0128, /* bflower (scene pairs, FROM 6; admp PASS but the draw-written cc divide info
             * mSph+0x48, chain j) */
    0x0033, /* obj_volcano (scene pairs, FROM 6; logic words equal, differing words = the btk
             * anm frame copies the draw writes (+0x428/+0x44C of +0x418, +0x4A0/+0x4C4 of +0x490) and the cc divide info
             * mCyl+0x48) */
    0x0114, /* obj_Yboil (scene pairs on the keyed-aid FROM 6, strict;
             * differing words = draw copies only (s11/drawcopy.py: 300 / 263 words, 0 logic)) */
    0x0080, /* obj_light (scene pairs (WWHD_DISPLAY_HZ=0), FROM 6, strict, admp
             * PASS) */
    0x011B, /* obj_auzu (Big Octo whirlpool, type 0; pairs, FROM 6,
             * strict; differing words = the btk frame's draw copies +0x3CC/+0x3F0 of +0x3BC only) */
    0x01BB, /* tornado (sea 38, pairs, FROM 6, strict, sound / procs PASS; frame move per
             * rule 1, current.pos equal; differing words = draw copies (bck/btk frame copies of +0x3BC/+0x6B4/+0x6B8,
             * +0x300/+0x308) and mJointMtx +0x768..+0x977 written by the joint callback in the draw (B's first step =
             * A's frame on every frame)) */
    0x0105, /* spc_item01 (LinkRM front door pairs, FROM 6, strict, admp 0 differing words,
             *) */
    0x01CE, /* dai_item (Windfall pedestal items,  pairs, 14 items, FROM 6, strict;
             * differing words: +0x1C0 a tevStr draw copy of +0x138, +0x748 mBckSpeed = the central cLib_addCalc ease's
             * f32 rounding (the per-step smoothing rule, max 1.25e-3)) */
    0x01BE, /* the grappling hook / rope (himo2; its rope pair is strict yes with the rope converted) */
    0x01B0, /* boomerang (pairs with Link's hand:
             * position exact A/B on all 296 frames (throw, flight, catch), Link procs / motion strict PASS, sound / procs
             * PASS; the only failure is the camera: after the throw the follow eye settles 2.2-3.0 off (radius 59.7 vs
             * 60.2), an item) */
    0x0104, /* deku_item (Forest Haven leaf: event bit 0x1801 test poke, pairs, N 2,
             * FROM 6, strict; differing words only mAcch / mAcchCir working check positions (B - A = 1.875 every frame,
             * pos equal: the step's moved position before the ground correction, central motion) and a draw-written
             * cylinder word) */
    0x01D8, /* arrow (hand conversion pair at N 2 (Link's
             * bow wait fix), CONV Link + camera + arrow: strict equivalent; arrow proc / position / speed equal on every
             * frame (shot, flight, stuck at the same point); differing words only tevStr (draw light state, Link's
             * too), old.pos per step in flight, one 1-ulp y at frame 120;
             *) */
    0x0102, /* race_item (t60conv + hand Boating Course: warp Ocean:0:0, 32 items with their
             * floating barrels, pairs, N 2, FROM 6, strict; differing words only collision-system
             * values from the extra Move per frame (mCyl divide info, AABB y) and, for 1 item, its unconverted barrel's
             * earlier divergence (before FROM, item at dt 1)) */
    0x00DD, /* All-Purpose Bait (hand conversion pairs (ground) and (water) at N 2,
             * CONV Link + camera + esa, 13 baits over their whole life: states / timers equal every frame, water positions
             * exact (bob share), ground within 0.24; a delete requested in the same frame leaves B one step later
             * execute (one frame earlier gone); the pairs' sound / save / event-camera differences start at Link's
             * FoodThrow event (frame 61), before any bait exists) */
    0x0175, /* the Windfall / sea villagers (the whole-frame form rule NPC form; picture pending frame interpolation) */
    0x016C, /* Tott (npc_tt; the whole-frame form rule NPC form; pair B-vs-B equivalent, 2026-11-18) */
    0x0160, /* Zunari (npc_rsh1; the whole-frame form rule NPC form; pair B-vs-B equivalent) */
    0x015F, /* Doc Bandam (npc_ds1; the whole-frame form rule NPC form; pair nbds B-vs-B equivalent) */
    0x0074, /* the Deku Tree (npc_de1; NPC form, the whole-frame form rule; Forest Haven  pair at N 2,
             * SYNC_LOADS, strict, sound 9000/9000, RNG seeds equal every frame) */
    0x0150, /* the Koroks Bj1-Bj7 0x150-0x156 (npc_bj1; NPC form; same pair, all 7 present) */
    0x0151, 0x0152, 0x0153, 0x0154, 0x0155, 0x0156,
    0x016F, /* Medli (npc_md; NPC form; Earth Temple entrance  pairs (idle) / npcmdw-776
             * (Link walking, Link motion exact) / npcmdi-776 (picture next to Link equal), strict) */
    0x014E, /* Makar (npc_cb1; NPC form; Wind Temple entrance  pairs (picture
             * next to Link equal), strict) */
    0x00D7, /* the Keese (ki; pair explained: wander arrival f184 / f185 = position decision, early ship-decision (iii)) */
    0x00F7, /* the Miniblin (pt; pair explained: hit on Link f390 / f389 = position decision, early ship-decision (iii)) */
    0x004E, /* the riddle switch watcher (msdan; whole form; strict pair) */
    0x004F, /* the sinking stair steps (msdan_sub; whole form; strict pair) */
    0x01CD, /* the dishes (mshokki; whole form; strict pair) */
    0x002F, /* the gate (hami2; whole form; strict pairs) */
    0x0030, /* the gate (hami3; whole form; strict pairs) */
    0x0031, /* the gate (hami4; whole form; strict pairs) */
    0x0032, /* the lift floor (hbrf1; whole form; strict pairs) */
    0x0096, /* the breakable pillar (hcbh; whole form; strict pair) */
    0x0139, /* the forest fireflies (nh; NPC form; Forest Haven  pair npcfhks at N 2, SYNC_LOADS,
             * strict, sound 9000/9000; admp differences only in tevStr (draw-side)) */
    0x00B6, /* the sign (kanban / kanban_exec; same pair npcfhks, strict) */
    0x0148, /* the Dragon Roost Rito Bm3-Bm5 0x148-0x14A (npc_bm1; NPC form; Rito Aerie  pairs npcaers (idle)
             * / npcaerw (Link walking) and Dragon Roost Island  npchr, strict; Bm1 / Bm2 0x146 / 0x147 not present
             * in any scene paired, not marked) */
    0x0149, 0x014A,
    0x0172, /* Komali (npc_co1; NPC form; pair, strict, sound 1080/1080) */
    0x0171, /* the Great Fairy (bigelf; pairs npcbes (idle) / npcbew (Link walking), strict) */
    0x013A, /* the Os stone-head helper (npc_os; Tower of the Gods  pair npcoss, strict, sound 2880/2880) */
    0x0145, /* Tingle (npc_tc; Tingle Island  pair npctc, strict, sound 5060/5060) */
    0x016D, /* Zephos / Cyclos (npc_hr; Hr at Dragon Roost Island  pair npchr, strict) */
    0x00C8, /* the crab (kn, stage name kani; NPC form; Dragon Roost Island  pairs npckn (idle, Kn 848
             * records after the origin, admp no logic differences) and npcknw (Link walking): strict) */
    0x00A7, /* Dk, the dragon (dk; Outset  pair npcdko, FROM 1, strict, Dk present after the origin;
             * its cutscene-driven path is covered by the cutscene / event rows / 2026-11-19) */
    0x00A2, /* the bird statue (ebomzo; whole form; strict pairs) */
    0x0089, /* the iron gate (htetu1; whole form; strict pair) */
    0x0037, /* Ice Ring Isle (iceisland; whole form; strict pair) */
    0x00A6, /* the jump pad (jump; whole form; strict pairs) */
    0x0072, /* the Wind Temple propellers (windmill; whole form; strict pair; review note: AT capsule hit-flag shifts) */
    0x007A, /* the spike floor (toge; whole form; strict pair) */
    0x018A, /* the Tpota ripples (tpota; whole form; strict pair) */
    0x009F, /* the coffin (kanoke; whole form; strict pair) */
    0x00A0, /* the Eskban block (eskban; whole form; strict pair) */
    0x00A1, /* the skull-pot stand (ekskz; whole form; strict pair) */
    0x01AD, /* the bombable wall (wall; whole form; strict pair) */
    0x0075, /* the rat shop keeper (npc_nz; child of the rat hole Nzg 0xC7 mode 0; Dragon Roost Cavern
             * room 14  pair npcnz, FROM 6, strict, sound 3240/3240, Nz present after the origin 364 / 372) */
    0x0027, /* the tide water (obj_tide; Tower of the Gods  pair tides, FROM 6, strict, sound
             * 2880/2880, tide present 365 / 362 records; admp differences only tevStr (draw-side); Forsaken Fortress
             *  pair tideh: tide logic words equal, camera time slack only (the  camera difference seen
             * without the tide too); 2026-11-20 whole form whole-frame form: Helmaroc  pair tbdk (the
             * tide rises while Link stands in it: pos / matrices / timer change in A, every logic word equal, SWIM_UP at A's
             * f221, sound 56024/56024 incl. 0x183E, strict yes), bko recipe tbko (tide exact; Link's own swim-dive split
             * after a high fall =), M2tower  pair tswt (0x702D WATER_PACHA 534/533 matched, strict
             * yes)) */
    0x010D, /* coming (obj_coming, stage name Com_A, SCOB in nearly every sea room; pairs (ship in the
             * sea-20 whirlpool) and cmos44 (Outset), FROM 6, strict, admp no logic differences) */
    0x017D, /* Old Man Ho Ho (npc_ah; NPC form, the whole-frame form rule; pair B == ahs14_b B, strict) */
    0x017B, /* Mt (npc_mt; NPC form; pair B == mtfig3_b B) */
    0x017C, /* Manny (npc_mn; NPC form; pair B == mtfig3_b B) */
    0x0159, /* Beedle (npc_bs1; NPC form; pair B == obshop3_b B, strict) */
    0x0194, /* the gallery figures (obj_figure; NPC form; pair B == figa3_b B) */
    0x00C2, /* the seagull (kamome; NPC form after the per-row review; pair B == outset4_b B) */
    0x0170, /* the Killer Bees (npc_uk; NPC form; pair B == wfuk3_b B) */
    0x016E, /* Mrs. Marie (npc_ho; NPC form; pair B == nitiyou3_b B) */
    0x0178, /* Komali (npc_ac1; NPC form; pair B == etac15_b B in logic: 3 draw-written mBtpAnim
             * words differ on one step (the _draw btp entry), compare60 masks them;, Komali needs event bit 0x2E04) */
    0x0179, /* the King of Hyrule (npc_hi1; NPC form; pair B == hy2hi15_b B, 0 words) */
    0x0107, /* demo_barrel (Ktaruo, placed only in MajyuE room 0 = Forsaken Fortress 1 start; pair on
             *  (cryze-casual Start_Forsaken_Fortress_1, saved at 4400 after the fade-in), FROM 1, strict: procs
             * 152/152, sound 14734/14734, dbrl.admp 361 records, no logic differences) */
    0x008A, /* gnndemotakis (Gntakis, only GTower room 0 layer 8 = the end-game Ganondorf cutscene; pair
             * on the build with the port's WWHD_TEST_WARP_AT layer field (t1.3:GTower:0:0:8),
             * strict: procs 139/139, sound 927/927, gt.admp 1133 records, differing words only tevStr +0x110..+0x1D4) */
    0x01B9, /* doguu_demo (DoguuD, only ADMumi room 0 layer 0 by the goddess statues; pair,
             * build, FROM 1: dg.admp 451 records, no logic differences, procs 23/23, Link / camera PASS; sound
             * FAIL only on Link's idle voice 0x184B / 0x184C (linkVoiceStart, A only) = known item (2026-11-19:
             * Link long-idle procs / voice belong to idle random-draw item, not the actor) */
    0x0088, /* the Tower of the Gods water gates (hha; whole form; strict pair: water / part timers and splashes equal) */
    0x0047, /* the pitfall hole (hole; whole form; strict pair) */
    0x0081, /* the hookshot target mask (homen; whole form; strict pair; review note: collider divide-info bits, one record) */
    0x0055, /* the Earth Temple ladder (ladder; whole form; strict pair) */
    0x004C, /* the light statue key (mkiek; whole form; strict pair) */
    0x0110, /* the Earth Temple mirror (mmrr; whole form; strict pair) */
    0x007F, /* the Forsaken Fortress ship 2 (gaship2; whole form; strict pair) */
    0x008E, /* the Dragon Roost rising water (gryw00; whole form; strict pair) */
    0x01DB, /* timer (ObjTime, Asoko room 0 = the button cave; pair, build, Link
             * warped onto Kbota_00 (380, -420, -206): sw C0 -> ObjTime 00C000C8 runs its whole 3000-frame count, mode 1
             * at frame 30 .. 0 at 3030 in both; strict: procs 104/104, sound 46233/46233, Link / camera 0, ot.admp 3301
             * records, no logic differences); shared-world-state whole-execute form (2026-11-20): the class pair (Link on Kbota_00 = an unconverted switch writer) */
    0x00E3, /* Octorok (oq: small / sea / delayed / child Octoroks, their rock, the spawners; pair on
             *  (Forbidden Woods room 12, Link warped to the pond shore), N 2 keyed seq, SYNC_LOADS=1, +
             * build: oq.admp 3027 records PASS (the rock's flight and last frame exact), procs 90/90, sound
             * 1894/1894, rng 901/901, save PASS; strict FAIL only on Link anim 1.142 at 900 and the warp-frame camera
             * (frame 33): known Link / camera items; time slack PASS) */
    0x00FD, /* Sitem (hanging pods; whole form); pair on  (Forbidden Woods
             * room 9, bow on X+Y): Link warped to (-291, 6052, -7150) facing -z, an arrow at pod 21's hand -> my_break
             * (mode 6); strict: procs 90/90 (converted 00A8, 00FD, 01DC), sound 1144/1144, Link 0, sitem.admp 2696
             * records, differing words only tevStr, +0x1C0 and the line material mChain (+0x428..; draw side); the break
             * is one frame earlier in B (f118 / f119: the arrow's half step reaches the hand first, the speed-ramp position-gap rule contact).
             * (Earlier sitem pairs converted 0xFE = Hkikai1, not the Sitem.) */
    0x0058, /* the Tetra's ship shutters (ashut; whole form, background: own mBgMtx on the earlier steps; strict pair) */
    0x00BA, /* the swinging lamps (lamp; whole form; strict pair) */
    0x0078, /* the Forest Haven leaf lifts (leaflift; whole form, rideCallBack once a frame; strict pair) */
    0x0197, /* the swaying trees (lwood; whole form; strict pair) */
    0x001D, /* the floor switches (swflat; whole form, background: own mtx on the earlier steps; strict pair) */
    0x001C, /* the heavy switches (swheavy; whole form, rideCB once a frame; strict pair) */
    0x001F, /* the hammer switches (swhammer; whole form, background: own mBgMtx on the earlier steps; strict pair) */
    0x006F, /* the balance lifts (balancelift; whole form; pairs: logic equal, camera only, time slack yes - the own-state proof rule, rule) */
    0x01AC, /* the light stairs (lstair; whole form; pairs: logic equal; the Link shift is also in the unconverted control - the own-state proof rule, rule) */
    0x004B, /* the Mkie objects (mkie; whole form; pair: the Link shift and tri hit-partner words are also in the unconverted control - the own-state proof rule) */
    0x0028, /* the Tower of the Gods lifts (hmlif; whole form, background: own mBgMtx on the earlier steps; strict pair) */
    0x01C2, /* the arrow switches (hys; whole form, background: own mBgMtx on the earlier steps; strict pairs) */
    0x00FA, /* the Kui posts (kui; whole form; strict pairs) */
    0x008F, /* Jabun's cave wall (AjavW; whole form; strict pair) */
    0x0093, /* the Ganon's Tower Vteng (whole form; strict pair) */
    0x0091, /* the Hyrule castle statue (YLzou; whole form; strict pair) */
    0x002C, /* the Hyrule castle triboxes (tribox; whole form, push_pullCB once a frame; strict pair) */
    0x01CA, /* the Tower of the Gods statues (try; whole form; strict pair) */
    0x01B8, /* the ADMumi tower (tower; whole form; strict pair) */
    0x0166, /* Jb1 (Jabun; NPC form, the whole-frame form rule); pairs (layer aid
             * t1.3:Pjavdou:0:0:8, Jb1 is placed on layer 8 only; on the runtime): procs 33/33, camera
             * PASS, save PASS, rng 901/901, jb.admp 833 records, differing words only tevStr (+0x110.., written by the
             * draw's settingTevStruct from the environment light). Failures are Link's idle random-draw item only
             * (the own-state proof rule): A plays the 245-frame idle variant (A-only voice 0x184B / 0x184C), B the 140-frame one
             * (B-only Link foot sounds 0x3831 JA_SE_LK_FT_SW_SHIELD at its anim frames 32 / 38 / 71 / 78, the same
             * four starts both runs make for that variant before the origin), Link proc 4 105 frames earlier in B.
             * own-state proof check (pair jbbd): strict equivalent
             * yes (sound 850/850, procs 33/33, save PASS, no RNG difference, jb.admp: no differing logic words). */
    0x0176, /* Gk1 in the Windfall cafe bar Opub (npc_gk1; the whole-frame form rule NPC form; pair  B-vs-B equivalent) */
    0x0163, /* Kp1 in the rich man's house Orichh (npc_kp1; the whole-frame form rule NPC form; pair nlorh (with Gp1) time slack, B-vs-B equivalent) */
    0x0165, /* Gp1 in Orichh (npc_gp1; the whole-frame form rule NPC form; same pair nlorh) */
    0x0173, /* Zk1 in Atorizk (npc_zk1; the whole-frame form rule NPC form; pair  A-vs-B equivalent) */
    0x0147, /* Bm2 (Rito, daNpc_Bm1_c; NPC form, the whole-frame form rule); pair bmaer:  (Din's Pearl)
             * warped to Atorizk:0:0 (Bm2's spawn conditions 2-4 need Din's Pearl; the pre-Din  had none),
             * CONV 0x146-0x14A: two Bm2 (procs 2031 / 2032) present after the origin; procs 55/55, camera, save PASS,
             * rng 901/901, bm.admp 4998 records with no logic differences. Failures are Link's idle random-draw item
             * only (the own-state proof rule): the idle variants are swapped (A-only Link foot sounds 0x3831, Link proc 4 60 frames
             * apart). Bm1 0x146 (Atorizk Quill type 3) was not created in this scene: not marked.
             * own-state proof check (pair bmaerbd): strict
             * equivalent yes (sound 44/44, procs 55/55, save PASS, no RNG difference, bm.admp no differing logic words). */
    0x005D, /* the ice lifts (icelift; whole form, Link's frame-start position (process-order read); strict pair) */
    0x0034, /* the Wind Temple trees (vmc; whole form; strict pair) */
    0x01CB, /* the Vtil tiles (vtil; whole form; strict pair) */
    0x005F, /* the Tower of the Gods pedestals (pedestal; whole form; strict pair) */
    0x018C, /* the Master Sword chamber statues (zouK; whole form; strict pair) */
    0x0066, /* the Forbidden Woods vines (sk2; whole form; strict pair) */
    0x01AE, /* the propeller switches (swpropeller; whole form; strict pair) */
    0x0067, /* the Forbidden Woods s_turu (ssk; whole form; strict pair) */
    0x0146, /* Bm1 = Quill type 3 in Atorizk (daNpc_Bm1_c; NPC form, the whole-frame form rule): ACTR 00FFFF03 has
             * no spawn condition; createInit 021FDB44 -> init_PST_3 needs no Din's Pearl and event bit 0x1102 (set by
             * Medli's DRC talk, msg 0x1801). Pair bm1q2: state  (save with 0x1102, no Din's Pearl, warped to
             * Atorizk:0:0), CONV 0x146-0x14A, N 2, SYNC_LOADS: Bm1 present (A 1041 / B 973 records), procs 36/36,
             * save PASS, bm.admp 4505 records with no logic differences; equivalent (strict): yes. */
    0x0174, /* the trading-quest merchant RotenA (npc_roten; the whole-frame form rule NPC form; sea 34 point 1  pair nrot B-vs-B equivalent) */
    0x017A, /* the salvage ship crew (npc_sv; the whole-frame form rule NPC form, always held; sea 18 point 103  pair nsv time slack, B-vs-B equivalent) */
    0x00D8, /* the Magtail (mt; whole form, the whole-frame form rule; pairs: actor dump exact, the Link idle-procedure shift also in the Link-only control - the own-state proof rule) */
    0x0077, /** the Floormaster (fm; whole form, whole-frame form addendum; pairs = its Link-only control to the frame - the own-state proof rule) */
    0x003C, /* kaji (whole form; pair words equal, only Link's A-only 0x3831 as in the control: the own-state proof rule) */
    0x0111, /* pbka (whole form; strict pair) */
    0x0112, /* plant (whole form; strict pair) */
    0x0115, /* rflw (whole form; strict pair) */
    0x0056, /* smplbg (whole form, own mBgMtx; strict pair) */
    0x001E, /* swlight (whole form; strict pair) */
    0x0117, /* tenmado (whole form, own mBgMtx; strict pair) */
    0x001B, /* swpush (whole form, rideCB once a frame; strict pair) */
    0x011F, /* tntrap (whole form; strict pair) */
    0x0036, /* vfan (whole form; strict pair on the switch-off state gtw) */
    0x00EE, /* (bdk_exe / bdk_move / bdk_act / bdk_demo): the Helmaroc King, battle (whole form, the whole-frame form rule; M2tower  tower phase ACTION_T_FLY, pairs: the converted B = the unconverted control B (bdkx1) bit for bit on every frame; the A-vs-B differences are the control's too: Link idle variant voice 0x183E - the own-state proof rule, env light position in tevStr, camera at 330; 2026-11-20 acting pairs / tbko (Link on the lowest stair: t_fly 1 -> 2 at f37, the peck at f86, same frames, logic words equal), the 0x183E split = the tide 0x27 (fixed: tbdk strict yes)) */
    0x00EF, /* the Helmaroc King fight objects (tower blocks / poles / stairs; whole form + ride callback on the tick30 step; pair = control bit for bit; cliff n1-bdk- PASS; 2026-11-20 acting pairs / tbko: Link arms the lowest stair at f37 (health), Bdk's peck sets 0xF at f86 and it breaks, same frames, logic words equal) */
    0x005C, /* the Forbidden Woods rope lift Klft (whole form, the whole-frame form rule; ride_call_back once a frame, the rider's old.pos by the process-order read rule; pair kindan:2 strict, logic words equal) */
    0x0062, /* the Forbidden Woods leaf platform Kokiie (whole form, whole-frame form; pair kindan:5 strict, logic words equal) */
    0x01AB, /* the Tower of the Gods light bridge Gbrg00 (whole form, whole-frame form; pair Siren:4 strict, logic words equal; the switch path by N-step spec) */
    0x010B, /* the Dragon Roost lava geyser Yfire00 (whole form, whole-frame form; lifts mflft / magmarock on the same tick30 step; pair:2 strict, logic words equal) */
    0x005B, /* the Dragon Roost rope float lift Mflft (whole form, whole-frame form; ride_call_back once a frame, process-order read old.pos; pair strict, logic words equal, only +0x854 = mLineMat draw data from daMflft_Draw) */
    0x00F3, /* (bgn_action / bgn_exec / bgn_move): Puppet Ganon, phase 1 and the fight's controller (whole form, the whole-frame form rule; GanonK  battle, pair: the converted B = the unconverted control B (bgnx1) bit for bit on every frame end; the A-vs-B difference is the control's too: camera at 395 / 900) */
    0x00B3, /* the enemy warship (oship; whole form, whole-frame form addendum; pairs (the ships attack: MODE_RANGE_D 7 -> 6 -> 5) = their Link-only controls to the frame - the own-state proof rule) */
    0x00E1, /* the Big Octo (daiocta; whole form, whole-frame form addendum; pair (the octo appears: HIDE -> APPEAR -> WAIT): actor dump PASS, Link / camera / sound differences those of its Link-only control - the own-state proof rule) */
    0x00CD, /* the Morth (ks; pairs kindan:6 strict yes) */
    0x00D1, /* the Peahat / Seahat (ph; pairs sea:41 strict yes, tsh411n2 strict yes; Peahat pkd97n2 / pkd91n2 kindan:9 time slack only = the camera 4.147 f307 also in the no-conversion control - the own-state proof rule; N-step spec accepted per-step ground-check rows) */
    0x0129, /* acorn_leaf (whole form; strict pair) */
    0x012C, /* door10 (whole form; strict pair) */
    0x0135, /* dai (whole form; strict pair) */
    0x012D, /* door12 (whole form; strict pairs) */
    0x012B, /* fan (whole form, own mBgMtx; strict pair) */
    0x01A8, /* fire (whole form; strict pair; lc911n2 fails identically in the unconverted control) */
    0x00E2, /* the Big Octo's eyes (daiocta_eye; whole form, whole-frame form addendum, with the Big Octo 0x00E1; pair (CONV 0xE1+0xE2, 8 eyes moving) = the octo pair line for line, nothing beyond its Link-only control - the own-state proof rule) */
    0x00DE, /* Valoo (dr; whole form, whole-frame form addendum; pair (Adanmae): equivalent (strict), actor dump exact) */
    0x00F0, /** Gohdan (bst; whole form, whole-frame form addendum; pair (the intro demo and the fight start, no hit phase): actor dump exact, every verdict line = its Link-only control - the own-state proof rule) */
    0x00EA, /* Gohma (btd; whole form, the whole-frame form rule; warped pair = its Link-only control to the frame: Link's hit split at 892, camera from 768, admp from 1093 - the own-state proof rule; on the own-state proof check list) */
    0x00EB, /* / bmd_exec / bmd_move / bmd_camera: Kalle Demos (bmd; whole form, the whole-frame form rule; warped pair: Link, sound, procs, save, admp equal, only the camera item - the own-state proof rule) */
    0x00EC, /* Kalle Demos' hands (bmdhand; whole form; pair as bmd) */
    0x00ED, /* Kalle Demos' feet (bmdfoot; whole form; pair as bmd) */
    0x0064, /* Gohma's wooden platforms (bita; whole form; pair: procs equal) */
    0x00CA, /* the Armos Knight (amos; whole form, the whole-frame form rule addendum; pair (Siren room 22): the Armos dump exact up to Link's split at 90 (LARGE_DAMAGE -> _UP in the same frame), the rest follows from Link - the own-state proof rule; amP2_3: split at 90 gone with AQ; residual = per-step ground-check landing at 98) */
    0x00F4, /* (bgn2_exec): Puppet Ganon phase 2, the spider (whole form on the tick30 step, the whole-frame form rule; state  (bgn2 fight), pair: converted B = unconverted control B (bgn2x1) bit for bit on every frame (bgn2 / bgn dumps); the A-vs-B differences are the control's too) */
    0x00F5, /* Puppet Ganon phase 3, the snake (whole form on the tick30 step, the whole-frame form rule; state  (bgn3 fight, life-poke run), pair strict equivalent and its B = unconverted control B (bgn3x1) bit for bit on every frame (bgn3 / bgn dumps)) */
    0x00F1, /* Phantom Ganon (fganon; whole form, the whole-frame form rule addendum; pair (Bow-save state + warp GanonJ:1:0: standby, deru, fly / fly2, shot (energy ball hits Link), shot2 (fgmahou balls), spin2 with a clone, kieru) = its Link-only control in every verdict line, fganon dump discrete fields exact - the own-state proof rule) */
    0x01CF, /* the dropped / carried weapon Boko (whole form, the whole-frame form rule; pairs (the Bokoblin / Moblin carry and drop it) = their bases kbkP1_2 / kmoN1_2 to the frame: no class from the weapon) */
    0x00CB, /* the small Armos (amos2; whole form, the whole-frame form rule addendum; pair (Cave09 room 7): converted B = Link-only control B (am2X2_2) bit for bit on every frame (am2 dump, 4 actors), the A-vs-B differences are the control's (Link) - the own-state proof rule; am2P2_3: split at 56 gone with AQ; residual = per-step ground-check landing at 64) */
    0x00B5, /* Kargaroc (whole form, the whole-frame form rule addendum; pair (Siren room 18): converted B = Link-only control B (bbX1_2r) bit for bit on every frame (bb dump, 2 actors); the A-vs-B differences (Link idle pattern) are the control's - the own-state proof rule) */
    0x00CF, /* Bubble (bable; whole form, the whole-frame form rule addendum; pair (Siren room 14): converted B = Link-only control B (blX1_2) bit for bit on every frame (bl dump, 2 actors); the A-vs-B differences (Link idle pattern, the hit one frame later) are the control's - the own-state proof rule) */
    0x00D0, /* Wizzrobe (whole form, the whole-frame form rule addendum; pair (Wind Temple room 0): wz dump exact, Link strict, only the camera after the test warp (31-65) differs; converted B = Link-only control B (wzX1_2) bit for bit on every frame (wz dump, 4 actors), camera equal - the own-state proof rule) */
    0x00FC, /* (whole form, the whole-frame form rule addendum; pair (Wind Temple room 7): sss dump exact, the A-vs-B differences = the Link idle pattern; converted B = Link-only control B (sssX1_2) bit for bit on every frame (sss dump, 10 actors) - the own-state proof rule) */
    0x0087, /* obj_trap: Blade Trap (whole form, the whole-frame form rule addendum; pair (Wind Temple room 5): trap dump exact (2 traps moving), the A-vs-B differences = the Link idle pattern; converted B = Link-only control B (trapX1_2) bit for bit on every frame - the own-state proof rule) */
    0x013E, /* Orca (npc_ji1; NPC form, the whole-frame form rule; pair (Bow-save state + warp Ojhous:0:0: talk, speak, startspeak, the battle game: wait / guard / tate attack, Orca hits on Link) = its control in every verdict line, ji1 dump worst values identical - the own-state proof rule) */
    0x00BE, /* Stalfos (whole form, the whole-frame form rule addendum; pair (Wind Temple room 0): st dump within tol, Link strict, sound PASS, only the camera after the test warp and at the knockback differs; converted B = Link-only control B (stX1_4) bit for bit on every frame, Link and camera equal - the own-state proof rule) */
    0x00CC, /* Mothula (whole form, the whole-frame form rule addendum; pair (Forbidden Woods boss room 10): gm dump exact, Link strict, sound PASS, only the camera at 213-228 differs; converted B = Link-only control B (gmX1_2b) bit for bit on every frame, Link and camera equal - the own-state proof rule) */
    0x0184, /* kytag05 (frame-level gate; pairs (Adanmae, N 2, s7, strict yes): kytag05 dump 0 differing words) */
    0x008D, /* obj_rcloud (no change needed; pairs strict yes: 0 logic words with tevStr masked: +0x110..+0x1DF
             * is written by _draw (settingTevStruct, picture state) and differs from frame 70, before the conversion at frame 180
             * (N 2's env light, not rcloud); btk +0x3CC / +0x3F0 = draw copies) */
    0x01A7, /* steam_tag (point wind + capsule on every emitting step; pair strict yes: 5 frames where B's frame-end
             * record holds the first step's dCcS::Move writes, B_end[f] == A_end[f + 1]) */
    0x017F, /* kytag00 (no change needed; pairs (A_mori) / fom7n2 (Forest Haven) strict yes, kytag00 dump 0
             * differing words; pair static (mSwitchNo 0xFF: mTarget is set to 1 at create and never chases), chase proven by
             * N-step spec (N2/N4 exact) - as the no-placement rule) */
    0x0133, /* andsw0 (AND_SW0; frame-level switch logic on the first step; pair:  COPY + warp
             * M_NewD2:0:0, switches 0x34 / 0x35 poked on at 7 s (*101F84DC+0x7A1 = 0x30), FROM 6, N 2, s7: andsw0 acts on
             * A's frame f357 (mAction 0 -> 0x0A, switch 0x00 set), every logic word equal on every frame, Link + camera
             * strict; one unmatched sound 0x6103 at the window end (daBoko_c::setFlameEffect, per-frame counts equal) -
             * (acts); shared-world-state whole-execute form (2026-11-20) = the 30 Hz execute at its line place (by construction, dt-1 check; no model, empty draw, no per-step callback); class pair) */
    0x0182, /* kytag03 (the shared-world-state rule: the switch / event / player reads and the contrast writes on the tick30 step, the
             * door model per step; pair:  = cryze-100pct 15-StartET COPY at M_Dai room 11 (switch 0x4F clear),
             * switch via poke at 7 s (*101F84DC+0x7A6 = 0x80, read back: set on f211 in both); contrast on (mbIsActive 0 -> 1)
             * on f211 in A and B, every logic word equal, strict yes; control (unconverted) the same; same-frame writer
             * order by the shared-world-state construction (class)) */
    0x0183, /* kytag04 (the shared-world-state rule: the frame logic on the tick30 step; pair: M_NewD2 room 2, the
             * zone switch 0xE0 set by its writer at A's frame f225: kytag04's timer 1..5 at f225..f229, state 1 at f230 in A
             * and B, the 10 s switch poke hidden in both (re-set in the same frame), every logic word equal, strict yes;
             * the earlier first-step form failed this in) */
    0x0134, /* andsw2 (AND_SW2; shared-world-state whole-execute form (2026-11-20) = the 30 Hz execute at its line place (by construction, dt-1 check; no model, empty draw, no per-step callback); class pair) */
    0x010E, /* coming2 (Com_B; whole form on the tick30 step (t60w, no picture) = the shared-world-state whole-execute class: the 30 Hz execute at its line place (by construction, dt-1 check; no model, empty draw, no per-step callback: its safety callback runs only from its own search); class pair) */
    0x010F, /* coming3 (Com_C; whole form whole-frame form / shared-world-state, the barrel read by the process-order read rule fsPos; pair:  COPY +
             * warp sea:5:0 (ten Com_C), FROM 6, N 2, s7: the challenge barrels spawn and are followed (pos / angles change
             * in A), every logic word equal on every frame (tevStr only), Link / camera / sound strict; without the
             * fsPos read showed pos.y about 2 ahead from f182) */
    0x0014, /* alldie (ALLdie; shared-world-state whole-execute form (2026-11-20) = the 30 Hz execute at its line place (by construction, dt-1 check; no model, empty draw, no per-step callback); class pair) */
    0x01D6, /* AttTag (tag_attention; whole form; pairs A_mori strict, M_NewD2 r2 = its control - the own-state proof rule) */
    0x012A, /* SW_C00 (swc00; whole form; pair Onobuta strict) */
    0x0181, /* ky_tag2 (kytag02, the wind rail; whole form; pair A_mori strict) */
    0x0057, /* Akabe (obj_akabe; whole form; pair r2 = its control - the own-state proof rule) */
    0x0180, /* ky_tag1 (kytag01; execute returns TRUE, no code change; pair Onobuta strict) */
    0x00DC, /* Pig (whole form, carryables form held = mMode 1, the whole-frame form rule addendum; pair (Outset sea room 44 at night, en2mk3: true60 +): a pig walking (pos changes on 453 frames), kb dump exact on every frame, Link strict, camera, sound, procs, save PASS: equivalent (strict). kbP1_2 dumped no pig: "Kbota" is another profile) */
    0x00F9, /* mozo: Moblin statue (whole form, the whole-frame form rule addendum; pair (Forsaken Fortress majroom room 4, Link warped below the statue, en2mk3): the statue searches (beam position on 344 frames); A-vs-B: Link's knockback landing (accepted class per-step ground-check) at 134, the beam follows him; converted B = Link-only control B (mozoX2_2) bit for bit on every frame, Link and camera equal - the own-state proof rule) */
    0x00D4, /* Poe (whole form, the whole-frame form rule addendum; pair (Earth Temple room 13, Link warped at 4800 -1200 9600, en2mk3): two Poes wake and attack; A-vs-B: Poe hit on Link at 126 in A / 151 in B, pw words differ from 57 by camera-derived floats (lantern place from the camera eye, camera differs after the test warp); converted B = Link-only control B (pwX2_2) bit for bit on every frame (3 Poes), Link and camera equal - the own-state proof rule; re-check after the process-order read camera-eye / held-lantern fix-forward, pairs the control with 0xD4 removed from this list: converted B = control B on every frame-end record but mActorPlace x/z, where converted B = A exactly and the control is up to 0.03 off (live mid-frame eye); compare reports identical) */
    0x016A, /* Kg1, the Sploosh Kaboom shopkeeper (NPC form; pair Kaisen: talk, pay, intro, board game = its control in every verdict line - the own-state proof rule) */
    0x003E, /* GBoard, the Sploosh Kaboom board (whole form; pair = in every verdict line incl. the board dump report - the own-state proof rule) */
    0x00E0, /* ReDead (the process-order read rule: the frame-start eye / attention position, Link's checkAttentionPosAngle from the process-order read rule's frame-start eye, the tick30 dispatch reads A's head yaw; pairs (the seed), (Link fixed), rdP9_2: rd.admp PASS (the 855 mHeadAngle split gone), Link strict PASS, only the known warp-frame camera @36 (cam-tslack PASS); strict Link os77 equivalent) */
    0x00D9, /* Molgera (whole form, the whole-frame form rule; pair (bwd / bwds / bwdg converted): equivalent (strict), bwd dump exact, Molgera active (249 logic words change)) */
    0x00DF, /* Valoo's tail in the Gohma fight (whole form, whole-frame form; pair (the Gohma warp): dr2 dump exact, the tail active (16 logic words); the A-vs-B Link / camera differences (pos 5.6 @903, camera @768) are the Link-only control's (dr2X2_2): converted B = control B on every frame-end record - the own-state proof rule) */
    0x00D3, /* Jalhalla (whole form, whole-frame form; pair (M_DaiB fight): equivalent (strict), bpw dump exact on 4349 records, body + Poes active (43-129 logic words per process)) */
    0x0023, /* tag_so: TagSo (seagull / jump game marker; no code: daTag_SoExecute returns TRUE; N-step spec N 2 / 4 2000/2000 on the unchanged execute; pair (Outset sea room 44 at night, 5 TagSo, en2mk4): tagso dump exact (3606 records), Link, camera, sound, procs, save PASS: equivalent (strict)) */
    0x00A4, /* aygr: Aygr (no code: Execute = set_mtx + the matrix out, no per-frame state; N-step spec N 2 / 4 2000/2000 on the unchanged execute (two steps at dt 0.5 = one at dt 1); no pair: 2026-11-20, mark from N-step spec + the no-change reasoning) */
    0x0116, /* table: Table (no code: Execute = set_mtx + the matrix out, no per-frame state; N-step spec N 2 / 4 2000/2000 on the unchanged execute; no pair: 2026-11-20, mark from N-step spec + the no-change reasoning) */
    0x002A, /* the Dragon Roost lava slab Magrock (whole form, whole-frame form; ride_call_back once a frame; pair MiniKaz:0, the rocks active (A re-run: 27-62 logic words per rock change): logic words equal; its differing words = the Link-only control to the value (tevStr / mLighting, draw-written anim controllers, the culling NODRAW bit from the camera) - the own-state proof rule) */
    0x00BB, /* the Forest Haven fireflies (whole form, whole-frame form; pair Omori strict, 77 actors active (16 logic words each change), logic words equal) */
    0x0044, /* the rafts / ikada_h (whole form, whole-frame form; pairs strict, rafts active (23-162 words change); differing words only mTrackCallBack +0x10..+0x30, the track particle callback's own block, which differs before the conversion starts - particle state, 2026-11-20) */
    0x01BF, /* the rope RopeR (whole form, whole-frame form; pairs Abship:4 / pmjr1n2 majroom strict, ropes active (276-420 words change); differing words only mLineMat (line list link / line material) and the cc divide info of the spheres) */
    0x0109, /* the goddess statue Doguu (whole form, whole-frame form; pair sea 18 strict, its execute-played head / body / crystal anims and cc active, logic words equal; the doguu / megami demos are event-only (30 Hz fallback), 2026-11-20) */
    0x0043, /* the post box Tpost (whole form, whole-frame form; pairs strict, the look-at / joint state that follows Link active, logic words equal; the talk / mail path is event-only (30 Hz fallback), 2026-11-20) */
    0x008B, /* obj_gnndemotakie: Ganondorf's waterfall (per-step form: its execute only plays a loop btk, central; no stage placement, demo-created: rule, marked on dt-1 check + N-step spec N2/N4 + no change, no pair) */
    0x018D, /* ykgr: the Dragon Roost heat haze (whole form, the whole-frame form rule + addendum; pairs (on the frame-start table fix): equivalent (strict), the haze rate chase 0 -> 1 inside the window (frames ~280-300) equal on every frame, Link strict - the own-state proof rule) */
    0x00C6, /* the Rat (nz; crawl frame form, entry turns / bounce ease whole on the entry step, frame-start Co push, BG offset and landing flags, speedF ramp exact arc; pairs Abship:2 s7 time slack only = its Link-only control (Link motion strict PASS, camera 3.379 f251 in both - the own-state proof rule), w3tf1n2 strict yes; re-pair (wwhd-s25y) strict yes = its no-conversion control (WWHD_T60_TEST_UNCONVERT=0xC6) strict yes, nz dump: shape_angle.z / angle.z off A on 1 frame (rat 47C56A90 f243, fc64 vs fc59, 0.06 deg: the side-probe roll from a split-motion-rounded position on a BG edge - rounding note), rat 47C5A3E8 push 1.2 off from f267 (first differing words acch / push working words, pre-resolution pos within ulps, speed = A; the control drifts too - the per-step ground-check rule)) */
    0x00DB, /* Molgera's sand floor (whole form, the whole-frame form rule; pair (bwd / bwds / bwdg converted, bwdg dumped 0x4000 of 0x1DDF4): equivalent (strict), the bwdg dump exact on 1201 frames, the sand floor active (its grid words +0x22D8.. change in A)) */
    0x00D5, /* the Jump Flower (jbo; CO sphere for the tick30 Move only (mixed-pair hit-flag / one-sweep-per-frame attack), the vomit-jump flag on Link on the tick30 step (shared-world-state); active pair kindan:1 (STICK, Link launched by flower 47C2B0DC: its mode / anim words change on 70+ frames in A; link-procs 6 changes PASS, sound PASS): its link-motion / camera lines (pos 11.44 f385, camera 62.92 f342) = the no-conversion control (WWHD_T60_TEST_UNCONVERT=0xD5) line for line; converted B vs control B: link dump 0 logic words, jbo dump only mParticlePos 0x3E0 (the smoke position, nodeCallBack joint position from the model calc: draw-written) - the own-state proof rule) */
    0x00D6, /* the Boko Baba (bo; whole form, the whole-frame form rule; active pair kindan:1 (STICK 6.5-14:0:1, Link walks off the ledge, 3 Babas act: 91 / 45 / 41 logic words change in A): bo dump A == B (tevStr only); its link-motion / camera lines = the no-conversion control (WWHD_T60_TEST_UNCONVERT=0xD6): converted B vs control B 0 differing logic words in the bo and link dumps - the own-state proof rule) */
    0x002B, /* the push box (obj_movebox; whole form, the whole-frame form rule + addendum; pair (state  copy, pull inputs): the box position A == B on every frame (moves -2575 -> -2425 f100-160); the Link / camera lines (Link speed 5.8 f43, camera 4.7 f36) = its no-conversion control line for line - the own-state proof rule) */
    0x0164, /* Pf1, Maggie's father (NPC form; pair (Windfall warp sea:11:0: he walks his path, stt 3 / 4) strict equivalent and = its control in every verdict line, pf1 dump exact - the own-state proof rule) */
    0x0167, /* Kf1, Mila's father (NPC form; pair (Orichh: wait_1 / walk_1 upstairs) = its control in every verdict line (time slack, the camera 3.33 in both), kf1 dump exact - the own-state proof rule) */
    0x0094, /* hfuck1: the hookshot target Hfuck1 (whole form, whole-frame form; pair (Wind Temple room 2, 11 Hfuck1, Link idle, en2mk5): Link, camera, procs, hf dump exact, collider / Stts words active; the only A-vs-B difference = the unconverted Peahats' propeller sound 0x5857; converted B = Link-only control B (hfX1_3) on every frame-end record, same B sound series - the own-state proof rule) */
    0x00A3, /* kanat: the Forbidden Woods vine floor Kanat (whole form, whole-frame form, MoveBG own mBgMtx on the skip step; pair (kindan room 5, en2mk5, test pokes: switch 28 off in the stored kindan memory before the warp, on at 6 s): the floor breaks at 181 (particles, smoke), hides at 182, deletes itself at the smoke's end 245; equivalent (strict)) */
    0x0136, /* swattack: the hit switch SWat00 (whole form, whole-frame form; pair (Earth Temple M_Dai room 16, Link warped next to it, 3 sword swings, en2mk5): swa dump exact, hits at 198 / 228 set switch 0x78; A-vs-B = Link's sword-swing differences, converted B = Link-only control B (swaX1_3) on every frame-end record, compare reports identical - the own-state proof rule) */
    0x00F8, /* swtdoor: the Forsaken Fortress tower window doors SWTdoor (whole form, whole-frame form; cannot act in any placement: all 3 (M2tower room 0) have switch 0xFF, which isSwitch never reports, no code creates it; N-step spec = the whole gate (whole PASS, calls counted, negative control fails); static pair (3 doors): dump exact, Link / camera / procs / save PASS; its only FAIL is obj_tide's water sound) */
    0x001A, /* tag_evsw: Evsw, an event bit mirrored into a switch (whole form, whole-frame form; no actor state of its own: cannot act in its struct in any placement; N-step spec = the whole gate (whole PASS, calls counted); static pair (Wind Temple room 15, 15 Evsw): equivalent (strict), save PASS) */
    0x00AA, /* (npc_mk_static helpers inside): Ivan, the Killer Bees leader (NPC form; pair (warp sea:11:20, A presses: Ivan in visit_action walks and steps its phases, the second Mk hides) = its control in every verdict line, mk dump exact; B-vs-B mk dumps identical - the own-state proof rule) */
    0x005E, /* amiprop: the propeller Hami1 (whole form, whole-frame form; pair kaze:1:1 strict: its zone switch 0x20 poked off / on (read back), the turn on the switch edge (mPrevSwitch / mTurning / angle, 9 words change), logic words equal) */
    0x0059, /* the rope bridge (whole form, whole-frame form + the rider's old.pos as A reads it, process-order read; pairs strict 0 logic words; cd07 ed07k1n2 (bridge converted) and its control (bridge unconverted): the Bokoblin rider splits at the same record idx 114 in both and the bridge follows it (idx 117/118) in both; converted B = control B before the rider's split - the own-state proof rule (the Bokoblin hang split was re-pair after hold6: strict, bridge spec PASS = A; audit zd02b1n2 / zd03b1n2 spec PASS)) */
    0x0042, /* the Forsaken Fortress search light (whole form, whole-frame form; pair sea 1, early-game state, strict, the 3 lights sweep (67-76 words change), logic words equal) */
    0x0070, /* the shutter door Htobi1 (whole form, whole-frame form; pair Siren:1 strict: switch 7 poked off / on (read back), the switch detection and the close order (mbIsSwitch, mEventOrder) on A's frame, logic words equal; the open / close move is event-only (30 Hz fallback), 2026-11-20) */
    0x0131, /* the door knob00 (whole form, whole-frame form; pairs Ocmera, 23 doors active (mAction, open frame, event words), logic words equal; the pair's Link / camera split = its control to the value, converted B = control B on every frame-end record - the own-state proof rule; the door open demo is event-only (30 Hz fallback)) */
    0x012F, /* the Forbidden Woods door Mori1 (whole form, whole-frame form; pair kindan:13: switch 7 cleared in the saved stage memory before the warp and set at 9 s (read back), the door's switch wait -> order -> ready-open -> timer -> rise (mAction, mDelayTimer, mHeight) acting, logic words equal; the pair's Link / Bokoblin / camera / sound / procs split = the Link-only control to the value - the own-state proof rule; the open demo is event-only (30 Hz fallback)) */
    0x012E, /* atdoor: the Forsaken Fortress jail door ATdoor (whole form, whole-frame form; it cannot act anywhere: its only placement (M2tower room 0, param 0xFFFF) has switch 0xFF -> ACT_WAIT_e, whose proc returns; static pair:0:20 logic words equal, its camera split = the control to the value - the own-state proof rule; N-step spec whole N2/N4 PASS (b8 gate)) */
    0x0076, /* the Fishman (npc_so; NPC form; pair (warp sea:11:0: he swims near TagSo, modes 1 / 2 / 3) strict equivalent and = its control in every verdict line, so dump exact - the own-state proof rule) */
    0x0161, /* Mila (npc_kk1; NPC form; pair (state from the 100pct 19-WindfallCleanup save + warp sea:11:0, night: Link placed next to her, A: talk "Shh! Shhh!", her talk / look timers run) = its control in every verdict line (sound B-only starts in both), kk1 dump exact, B-vs-B identical - the own-state proof rule) */
    0x0061, /* kita: the Forbidden Woods hanging flower platform (whole form, the whole-frame form rule + addendum; Link on it held at 30 Hz as on an unconverted carrier, whole-form extension: pair (kita + Link converted): kita.admp PASS, the kita acts (46 logic words change: the ride sink, tilt and sway), B's kita = the control's (kita unconverted, Link held) on all 391 frame-end records; Link 1.77 @90 the same in the control (the stick seen a frame early, the own-state proof rule). Before the extension Link's per-step ride on the whole-form carrier split it from frame 88) */
    0x00DA, /* Molgera's larvae (pair (bwd / bwds / bwdg converted): equivalent (strict), the 3 larvae created by Molgera's tongue-damage round act (about 330 logic words change each over A's frames; B per step, 5298 records) and A == B on every record; the round's entry is STEERED by pokes in both runs (mAction 3, the pulled timer and the hit count: no hookshot route), the larvae themselves run free) */
    0x0123, /* the Boating Course goal flag Gflag (whole form, the whole-frame form rule + addendum; pair sea 48 (en2mg1) equivalent (strict), the Gflag dump exact on 901 records, the flag cloth / ropes active (1167 non-tevStr words change in A); its frame-end differences only in the flag packet's shader matrix +0xA90 / +0x9F8, written by the draw = picture state) */
    0x0144, /* npc_sarace: Sarace, the Boating Course host (NPC form; pair sea 48 equivalent (strict), the sarace dump byte-exact on 901 records, his logic active (16 non-tevStr words change in A)) */
    0x01A0, /* tag_photo: the Ocmera photo tag TagPo (whole form, the whole-frame form rule; pair Ocmera equivalent (strict), 7 TagPo dumps byte-exact; their wait-mode distance check sets the talk condition / event (0xF8, +0x438) = all non-event logic; the photo talk is event-only (30 Hz fallback)) */
    0x0119, /* Abesso's picture puzzle Apzl (whole form, the whole-frame form rule; pair Abesso (en2mg1) equivalent (strict), apzl dump exact on 1802 records (2 instances); its only non-event logic is STATE_IDLE's CANTALK condition set, the talk / puzzle play / rupee states run inside events = event-only (30 Hz fallback)) */
    0x017E, /* the Windfall auction Auction (whole form, the whole-frame form rule; event-only (30 Hz fallback): its non-event logic is stateless (executeWait at creation, empty MOVE_NORMAL, eventOrder idle at order 0), the bidding runs inside events; pair Orichh (en2mg1) strict, the auction dump exact on 901 records) */
    0x019E, /* tag_kf1: the Kf1 pottery compensation tag TagKf1 (whole form, the whole-frame form rule; it cannot act anywhere: stage name TagKf1 (rpx name table -> proc 0x19E) is placed in no stage or room (stageactors.py over all 544 Common/Stage/*.szs incl. the ACT0-ACTb / SCO / TRE layers, and over every sea room in Common/Pack/*szs*.pack: no hit; Orichh has Kf1 + Ptubo only) and no code creates proc 0x19E; static note as atdoor 0x012E; gates dt-1 check s1/s7 + whole PASS) */
    0x016B, /* Kg2, Salvatore at the cannon game (NPC form; pair (Bow-save state + warp sea:10:0, Link placed in front of him, A: attention + talk requests) = its control in every verdict line, kg2 dump exact, B-vs-B identical - the own-state proof rule) */
    0x015C, /* Koboli, the Rito mail sorter (NPC form; pair (Bow-save state + warp Atorizk:0:1, Link placed at his counter, A: talk, the sorting game's rules) = its control in every verdict line, bmsw dump exact, B-vs-B identical - the own-state proof rule) */
    0x010C, /* obj_buoyrace: the Boating Course race buoys Buoyr (whole form, the whole-frame form rule; pair stage Ocean (the Boating Course minigame stage; Gflag 00040000 makes them), en2mg1, equivalent (strict), 36 buoy dumps exact on 32436 records, 12 of them float / sway on the waves (11-18 non-tevStr words change in A each)) */
    0x007B, /* the Windfall ferris wheel Skanran (whole form, the whole-frame form rule + addendum, ride CB cbSkip; pair (en2mg4 = with the carrier extension; Link dropped into gondola 3, on the moving carrier 871 of 901 frames) Link + camera strict, the ferris dump exact on 901 records, 12 ferris words act; its one failure, sound (1738 B starts of 0x303B, obj_light's rotate sound started on every step by the marked obj_light 0x0080 at night:'s defect), is the same in its control (ferris unconverted): converted B = control B bit for bit (ferris / tagmk / life dumps, sound and camera logs) = own-state proof) */
    0x015A, /* Willi & Obli at the Flight Control Platform (NPC form; pair (Bow-save state + warp sea:14:0 next to Willi, A: talk, mode / animation changes) = its control in every verdict line, bmcon dump exact, B-vs-B identical for all 4 instances - the own-state proof rule) */
    0x0177, /* Lenzo (NPC form; pair (Bow-save state + warp Ocmera:0:0, Link placed in front of him, A: talk) = its control in every verdict line, photo dump exact, B-vs-B identical - the own-state proof rule) */
    0x015B, /* Bms1, the Bomb-Master Cannon shopkeeper (NPC form; pair (Bow-save state + warp Obombh:0:0, Link placed in front of him, A: talk) = its control in every verdict line, bms1 dump exact, B-vs-B identical - the own-state proof rule) */
    0x018B, /* Itnak: the Itnak object (whole form, the whole-frame form rule; it cannot act anywhere, 4-step scan 2026-11-20: (1) stage name Itnak (rpx name table 101EBFDC -> proc 0x18B) is placed in no stage or room: every Common/Stage/*.szs and every stage / room archive inside Common/Pack/*.pack, chunks ACT0-ACTb / TRES / SCOB / TGOB / TGSC / TGDR / Door incl. the room bfres dz; (2) raw string scan of all those archives (event / demo data included): no "Itnak"; (3) no code creates proc 0x18B (decomp grep of fopAcM_create / createChild / fpcSCtRq_Request by fpcNm name and number); (4) the rpx holds "Itnak" only as its name-table entry and its own arc / file names; static note as atdoor 0x012E / tag_kf1 0x019E; gates dt-1 check s1/s7 + whole PASS (b8 gate wv-b8.out)) */
    0x00B9, /* the torch bonbori (ep; whole form, whole-frame form; serial pair Siren:0:0 strict, 4 torches act (flicker timers / rnd / rotations), logic words equal) */
    0x0084, /* Vds: the Earth Temple Vds (whole form, whole-frame form; TGOB placement M_Dai rooms 3 / 16; serial pair M_Dai:3:3 strict, it acts (26 changing words), logic words equal) */
    0x0169, /* npc_auction: the Windfall auction NPCs Auc0-Auc7 (NPC form; pair (Bow-save state + warp Orichh:0:0 layer 1, Link placed in front of the kind-0 NPC, A: attention, talk) = its control in every verdict line, auc dump exact, B-vs-B identical - the own-state proof rule; the bidding NPCs run only inside the auction event, 30 Hz fallback) */
    0x015E, /* npc_btsw2: Btsw2, Baito on Dragon Roost (NPC form; pair (Bow-save state + warp sea:13:0, Link placed by him: he walks his path) = its control in every verdict line, btsw2 dump exact, B-vs-B identical - the own-state proof rule) */
    0x018E, /* saku: the breakable fences Ksaku / Dsaku (whole form, whole-frame form; serial pair DRC:2: Link placed at a Ksaku + sword, strict, it acts (26 changing words), spec PASS (specs/saku.admp.spec, compare60 --admp-require-spec; one collider-internal word on the hit frame, not logic)) */
    0x0122, /* hsehi1 (Hsh / Hsh2, Tower of the Gods): serial pairs (Siren:7, it acts: 54 changing words) and tt13h1n2 (Siren:13), strict yes, hsh.admp spec PASS 383 records each (specs/hsh.admp.spec, compare60 c60- --admp-require-spec; state togs from the 8-StartTogs save copy); 2026-11-21 */
    0x00C3, /* the Hyoi seagull (player form per step with the flight's frame form - steering on the tick30 step, velocity / ground checks on the first step, the frame's move exact -, NPC whole form otherwise; d_a_npc_kamome_t60.h; pairs (warp sea:10:0, Hyoi pear, X: take-over, A flaps, stick): flying kfP = its control kfX on every kam.admp word frame for frame (own diffs 0 in 1201 records; the shared 2-frame area-out event shift in both, the own-state proof rule), Link / camera / B's sound stream identical, procs 01C1 + save si.bin PASS; idle kiP = kiX, kam.admp PASS, B-vs-B identical) */
    0x0063, /* shand: the Forbidden Woods vines (whole form, whole-frame form; serial pair kindan:12 strict yes, 4 vines act (71 spec words change), shand.admp spec PASS 1532 records (compare60 pin 3 61a9be41, --admp-require-spec); same on vk12s1n2); 2026-11-21 */
    0x0071, /* shutter2: the Wind Temple shutter (whole form, whole-frame form; serial pair kaze:8 strict yes, it acts (switch edge poke: mDemoState / mStaffId / mbIsSwitch change), shutter2.admp spec PASS 383 records (pin 3, --admp-require-spec); kaze:4 vkz4s1n2 Link / camera split = its no-conversion control: own-state proof); 2026-11-21 */
    0x0130, /* doorKD / doorSH (Forbidden Woods kindan:13; whole form, whole-frame form; serial pair: kddoor.admp spec PASS 3447 records (compare60 pin 3 61a9be41, --admp-require-spec), it acts (arms / mbEnable / mAction change); strict no only Link / camera / sound / procs / save = its no-conversion control with the same numbers, Link line equal (angcmp 0 of 3577 frames): the own-state proof rule; same on vk13e1n2 / wk13ex1n2, vk13dw1n2 / wk13dwx1n2); 2026-11-21 */
    0x003D, /* Kg2's cannon game (whole form; route noon poke, Link at Kg2, CANON_GAME, 10 shots, stick aims; pair strict yes + control (UNCONVERT 0x3D) strict yes, compare60 pin 3 61a9be41, --admp-require-spec canon2 (29/41 words act); P-B vs X-B only tevStr picture words); 2026-11-21 */
    0x015D, /* Baito's mail sorting game (child of Bmsw at Atorizk when event bit 0x2701 is set; route noon poke + bit, sorting game: timer / letters / cursor act, 16/23 words; pair strict yes + control strict yes, pin 3, --admp-require-spec btsw; P-B = X-B 0 words / 1286 records); 2026-11-21 */
    0x004D, /* the melody stone (lesson route:  without the Lyric, Wind Waker on X/Y, Edaichi room 0, talk XY -> STATE_LESSON, cuts 0..7 act; pair strict yes + control strict yes, pin 3, --admp-require-spec mknjd; P-B = X-B 0 words / 774); the break demo (STATE_DEMO) is not covered by a route: reopen if it shows a split; 2026-11-21 */
    0x0124, /* treasure chests (route:  noon, Ocmera:0:1, Link 75 before the wooden chest takara ff100080 (boxCheck < 100, facings < 0x2000), A: the open demo runs, 9/18 spec words act; pair strict yes + control (UNCONVERT 0x124) strict yes, compare60 pin 3, --admp-require-spec tbox; P-B vs X-B only mSmokeEmitter +0x77C (pointer)); 2026-11-22 */
    0x0060, /* floor Hhyu1: breakable floor (route: Iron Boots poked into slot 9 + Y, kaze:9:9, Link on the middle tile, boots on -> mBreakStarted / mBreakTimer, break; pair strict yes + control (UNCONVERT 0x60, same FROM) strict yes, compare60 pin 3, --admp-require-spec floor; P-B vs X-B tevStr only); 2026-11-22 */
    0x01A6, /* fallrock: falling rocks (M_NewD2:2:1, idle 30 s: the tag drops a burst at ~20 s when the kankyo schedule bit opens, 11 rocks fall; pair strict yes + control (UNCONVERT 0x1A6, same FROM) strict yes, compare60 pin 3, --admp-require-spec fallrock; P-B = X-B 0 words); 2026-11-22 */
    0x009E, /* vgnfd: the Ganon's Tower door VgnFD (whole form, whole-frame form; event-only: its door demo; serial pair, gtw state, boss bit 0x3240 poked on (event byte 0x32 = 0x40, door bit 0x3904 off) before the warp to GanonA:1, B with WWHD_T60_EVENTS60=1: the demo runs (mState / mEventIdx / mStaffId change, mTimer 95 values, door bit 0x3904 set at its end), strict yes, vgnfd.admp spec PASS 383 records (compare60 pin 3 61a9be41, --admp-require-spec); control strict yes too); 2026-11-21 */
    0x0121, /* tapestry: the Earth Temple curtains Mcrtn (whole form, whole-frame form; re-pair after: dmdc1n2 M_Dai:2:2 4 curtains act (18-274 changing words); strict no only Link procs / motion / sound and the tapestry words Link's divergence moves, identical in the no-conversion control: converted B == control B byte for byte outside tevStr on every frame end (4 processes, 0 records differ), Link line equal (angcmp 0 of 3577): the own-state proof rule, the exact form); 2026-11-21 */
    0x009D, /* the Ganon's Tower warp pads Ygcwp (whole form, whole-frame form; event-only: the warp events rtn_warp / fg_warp0-1 (staff Ygcwp, GanonA / GanonM event_list); serial pair, gtw state, warp GanonA:1:5 = the PLYR spawn on the pad whose event 0 is EVNT rtn_warp, B with WWHD_T60_EVENTS60=1: the return warp runs (mStaffId / mCurBrk change, mTimer 119 values), strict yes, ygcwp.admp spec PASS 519 records (compare60 pin 3 61a9be41, --admp-require-spec)); 2026-11-21 */
    0x009B, /* vmsms: the Master Sword in its pedestal VmsMS (kenroom; whole form, whole-frame form; event-only: it deletes itself when the master_sword event starts); serial pair, state hyr1 (cryze-casual Hyrule_1 copy, before the Master Sword, saved in kenroom), Link warped next to the pedestal + A, B with WWHD_T60_EVENTS60=1: Link draws the sword, VmsMS is deleted on the same frame in both runs (203 records each), strict yes, --admp-exact vmsms.admp PASS (compare60 pin 3 61a9be41); fksw1n2 (A at 315 from the pedestal) left it idle); 2026-11-22 */
    0x0196, /* the cutscene actor (d_act0..31; whole form, whole-frame form, d_a_demo00_t60.h: the frame's demo commands on the tick30 step where the demo object holds forward(1)'s frame-end values); pair on m8cs1 (commits): warp t1.3 kenroom:0:200 (master_sword.stb in MODE_GAME, EVENTS60, the route), CONV 168,0x1DC,0x196: strict yes, --admp-exact dact.admp PASS (5193 records; one d_act acting: 75 frame changes), compare60 pin 3; control (0x196 not converted, by omission): strict yes; converted B = control B on all 5196 dact records; 2026-11-22 */
    0x0125, /* the salvaged treasure box tkrSlv (whole form, whole-frame form; event-only: created by the salvage demo); serial pair, crane state  (ship 90 from salvage point 1), origin load 60, D-right crane + A held 2-5 s, FROM 6, B with WWHD_T60_EVENTS60=1: the box is salvaged and acts (pos / mVolumeTimer / mDarkRatio / mDarkTimer / mLightTimer change, 576 records), sbox.admp spec: only +0x318 y 0.287 at frame 219 = Link's 0.287 there (the box rides the crane), identical in the control (UNCONVERT 0x125, same FROM): converted B == control B byte for byte outside tevStr on all 576 records, Link equal (angcmp 0 of 3843): the own-state proof rule (compare60 pin 3 61a9be41, --admp-require-spec); 2026-11-22 */
    0x00F6, /* / gnd_exec / gnd_move: Ganondorf (whole form, whole-frame form, d_a_gnd_t60.h: daGnd_Execute once per 30 Hz frame on the tick30 step); pair on m8cs2 (commits): warp t1.3 GTower:0:0:8 (the end-game layer), EVENTS60, CONV 0xF6 from 1 s: strict yes, --admp-exact gnd.admp PASS (1133 records; Gnd acts: 524 changing words, hair every frame; mAction 0x14 / mMode 0 the whole run: the fight states are not reached on this route), compare60 pin 3; control (0xF6 not converted, by omission, same FROM): strict yes; converted B = control B on all 1133 gnd records; 2026-11-22 */
    0x00F2, /* Phantom Ganon's dark energy balls (fgmahou; whole form, the whole-frame form rule; pair (with fganon 0xF1 marked; Bow-save state warp GanonJ:1:0: the shot2 volleys, 8 balls fly, 125-136 words change per ball in A) = its control (0xF2 unconverted) in every verdict line; converted B = control B bit for bit (fgmahou dump on 1466 frame-end records, Link trace, camera, sound, RNG): the A-vs-B differences (Link from 492, the balls from 892) are the control's - the own-state proof rule) */
    0x00FB, /* the Ss tentacle core (whole form, the whole-frame form rule addendum; pair (Forbidden Woods kindan room 1, Link warped inside the 350 range at 185 1200.6 9700 (frame 30), out at z 9300 (300), in again (570)): the Ss shield / open (mShielded, mMode) and head / hand tracking act (194-205 words per core in A); ss dump A-vs-B PASS; the A-vs-B differences (Link from the warps, camera, sound) are its control's (0xFB unconverted) in every verdict line; converted B = control B bit for bit on 901 origin-aligned frames (ss dump, Link trace, camera, RNG; sound: one 0x3831 start at 685 with another caller address 02520570 / 0280B5C0, same id and frame, as hfX1_3) - the own-state proof rule) */
    0x0065, /* sk: the swaying vine Turu (stage name "Turu" -> proc 0x65 in the rpx object name table l_objectNames 101EBFDC; whole form, the whole-frame form rule addendum; pair (Forbidden Woods kindan room 1, 7 Turu, Link idle): equivalent (strict), the 7 vines sway every frame (54-57 words each in A), sk dump exact on 6307 records, Link / camera / sound / procs / save PASS) */
    0x0082, /* homensmoke: the Homen face's fall smoke (the own-state proof rule; route tools/pairhm.sh: Wind Temple kaze room 2, Iron Boots + hookshot pull on Homen1, pin 4, spec hsmk required): pair (TEST_CONVERT 0x82 FROM 1) = control (TEST_UNCONVERT 0x82 FROM 1) bit for bit, hsmk 66 records / homen 5225 records 0 differing words; both strict no on a Link hookshot-carry split at 431 (not this subject); the smoke acts in A (emitter +0x3EC cleared, end flag +0x3F4 set at rec 65) */
    0x01C0, /* att: Puppet Ganon's 8 string attention targets (the own-state proof rule; route tools/pairat.sh,  idle 30 s, pin 4, spec att required): pair (TEST_CONVERT 0x1C0 FROM 1) = control (TEST_UNCONVERT 0x1C0 FROM 1) bit for bit, att 7216 records 0 differing words; att acts in A (~7700 records change); both strict no on a Link / camera split at frame 792 from the bgn fight (not this subject) */
    0x00E4, /* Gyorg (the own-state proof rule; route tools/pairgy.sh,  sail, dump at the profile execute daGyExecute 02169264, pin 4, spec gy required): pair (TEST_CONVERT 0xE4 FROM 1) = control (TEST_UNCONVERT 0xE4 FROM 1), gy 1890 records 0 differing words, link / camera / sound PASS in both; 4 Gyorgs act in A (modes 1/6/5/4/0) */
    0x00E9, /* Beamos (the own-state proof rule; route tools/pairbe.sh, warp Siren:6:0, stick up 3-5 s, dump at the profile execute daBemosExecute 02326020, pin 4, spec bemos required): pair (TEST_CONVERT 0xE9 FROM 1) = control (TEST_UNCONVERT 0xE9 FROM 1), bemos 836 records 0 differing words, admp strict PASS, link / camera / sound / save PASS in both; the red Hmos2 turns, charges and fires in A */
    0x00D2, /* Princess Zelda (the own-state proof rule; route tools/pairpz.sh, warp GanonK:0:0 tower-top intro, A every 0.7 s, dump at the profile execute daPzExecute 02455070, pin 4, spec pz required): pair (TEST_CONVERT 0xD2 FROM 1) = control (TEST_UNCONVERT 0xD2 FROM 1), pz 3535 records 0 differing words, every compared spec word worst 0 vs A; Zelda acts in A (3535 of 3537 records change) */
    0x0137, /* switem: the hidden-item spot ITat00 (route tools/pairsw.sh: sea room 45, sword slash on the spot at (-80000 117.8 322200), item spawned; dump at the profile execute daSwItemExecute 024A2A20, pin 4, spec switem required): pair (TEST_CONVERT 0x137 FROM 1) strict yes, control (TEST_UNCONVERT 0x137 FROM 1) strict yes, P-B = X-B on 10323 records 0 differing words */
    0x00FE, /* machine (Hkikai1, Wind Temple): whole form (the whole-frame form rule addendum: set_speed reads the windmill's mAngle[1], attack reads Link); route kaze:2:2 with the windmill's +0x1538 poked so the machines move ~470 units; bin wwhd-mach2, pin 6, machine.spec all words required, 766 records: mmP2 (TEST_CONVERT 0xFE FROM 1) strict yes, every word 0; control (TEST_UNCONVERT 0xFE FROM 1) strict yes; P-B = X-B 0 words */
    0x00E5, /* gy_ctrl (Gyorg controller): whole form; sail, dump at the profile execute 0216C1A0, pin 6, gyc spec all words: gycP2 (CONVERT 0xE5,0xE6 FROM 1) = control (UNCONVERT 0xE5,0xE6 same FROM), gyc 1802 records 0 words, gy / link / camera / sound 0 (own-state proof); 2 controllers act in A */
    0x0052, /* nest (daObjNest, Kargaroc's nest; whole-form carrier t60wc, the whole-frame form rule; v1-v3 as EM rebases): t60-rev nestP3 recipe (Link warp 235 3700 -5925 onto the nest, stick 4 / 7 s), dump at the profile execute 0237EF48, bin wwhd-s99nest, pin 6, nest-exact spec required: nestP8 (TEST_CONVERT 0x52 FROM 1) nest.admp PASS 901 records every word 0, Link frame-end pos exact to 337, from 338 the rule-6a step-1 Acch wall hit on the nest rim (poly 0x1E; pos 3.376 @341); control (TEST_UNCONVERT 0x52 same FROM, 'test unconversion profiles: 0052') the same verdict lines; P-B = X-B: nest frame ends 0 words, Link trace / camera / sound / save / RNG identical (one-frame audio-helper word mJAIZelAnime +0x64F0/+0x64F4 at 227 in P-B only, not in nestP7: run timing) */
    0x00C0, /* the Darknut's cape (the own-state proof rule; gj6 scene: warp GanonJ:6:6, the caped Darknut; dump at the profile execute 021BC3C4, pin 6, --admp-exact mant, 383 records): gjc1 (TEST_CONVERT 0xC0 FROM 6) = control (TEST_UNCONVERT 0xC0 same FROM) on every logic word (only mPacket.mMatrixBlock's GPU double-buffer index differs: picture state); Link trace / camera equal; the cape acts in A (1131 changing words) */
    0x00E6, /* gy_ctrl second profile: same pair as 0x00E5 */
    0x00C7, /* nzg: the rat hole (stage name "nezuana" -> proc 0xC7 in l_objectNames 101EBFDC; whole form, the whole-frame form rule addendum; pair (Outset night state warp Abship:5:0, the pirate ship hold: 3 holes in mode 1 create rats from the start (mRatCount, mTimers, collider words act: 17-20 words per hole in A)) = its control (0xC7 unconverted) in every verdict line; converted B = control B bit for bit on 901 origin-aligned frames (nzg dump, Link trace, camera, sound, RNG): the A-vs-B differences (Link and the rats from 280) are the control's - the own-state proof rule) */
    0x0068, /* the boss warp Warpf (whole form, the whole-frame form rule, d/t60_objw.h; the own-state proof rule); pair on m8cs3 (commits): boss bit poked (info+0x7B9 |= 08 + table copy +0x475) at 1.0, warp t1.3 SirenB:0:1, stick forward 5-9 s: Link walks into the Warpf, DEMO_BOSS_WARP (0xC9), stage change, SLOW_FALL (0x28) at the arrival; EVENTS60, CONV 0x68 from 1 s; Warpf acts in A (387 records, 25 changing words: walk-in mode words +0x3E0 / +0x3E4 / +0x3EC, mPlayerPos +0x408); control (TEST_UNCONVERT 0x68, same FROM, 'test unconversion profiles: 0068') fails A in the same verdict lines as P (link-procs: SLOW_FALL 2 frames early, 455 B / 457 A; link-motion, camera, sound, procs, admp); converted B = control B on all 387 warpf records (admpdiff 0 differing); compare60 pin 4 264e00fd; 2026-11-22 */
    0x006D, /* the Ganon's Tower warp Warpgn (whole form, the whole-frame form rule, d/t60_objw.h; the own-state proof rule); pair: warp t1.3 GanonM:2:0, stick forward 5-12 s: Link walks onto the warp; EVENTS60, CONV 0x6D from 1 s; Warpgn acts in A (413 records, 26 changing words: +3F4..+408 120x); control (TEST_UNCONVERT 0x6D, same FROM, 'test unconversion profiles: 006D') fails A on the same verdict lines as P (link-procs: procedures 53 / 55 3 frames early, link-motion, camera, sound, admp); converted B = control B on all 413 warpgn records, Link trace / camera identical, sound identical over the common window; compare60 pin 4 264e00fd; 2026-11-22 */
    0x01C3, /* the sword-hit switch SW_HIT0 (whole form, the whole-frame form rule, d/t60_objw.h; the own-state proof rule); pair: warp t1.3 GanonM:2:0 (four SW_HIT0), stick forward 5-12 s; EVENTS60, CONV 0x1C3 from 1 s; the four switches act every frame in A (1652 records; +3B4 / +440 415x, +3C8 / +3EC / +450 / +474 118-191x); control (TEST_UNCONVERT 0x1C3, same FROM, 'test unconversion profiles: 01C3') fails A on the same verdict lines as P (link-procs, link-motion, camera, sound; admp +0x664:16 +0x534:4 +0x3C:2 in both); converted B = control B on all common swh records (the one other record is the control's last record, cut by the end of the run), Link trace / camera identical, sound identical over the common window; compare60 pin 4 264e00fd; 2026-11-22 */
    0x019A, /* tag_hint: the hint tag TagHt / TagHt2 (the shared-world-state rule whole-execute form; the own-state proof rule); pair: warp t1.3 sea:1:15:1 (Outset sea room 1, TagHt on layer 1), stick forward 5-10 s: Link walks into it; EVENTS60, CONV 0x19A from 1 s; TagHt acts in A (3BC 86x); --admp-exact tht.admp PASS (481 records); control (TEST_UNCONVERT 0x19A, same FROM, 'test unconversion profiles: 019A') fails A on the same verdict line as P (camera 106.4 @396 only); converted B = control B on all 481 records, Link trace / camera identical, sound identical through frame 480 (the run end); compare60 pin 4 264e00fd; 2026-11-22 */
    0x0199, /* the event tag TagEv (the shared-world-state rule whole-execute form; the own-state proof rule); pair: warp t1.3 sea:14:0 (Flight Control Platform), Link put on the TagEv 0DFFFFFF (298463 1150 -201822) at scenario step 450 (WWHD_LINK_WARP); EVENTS60, CONV 0x199 from 1 s; the tag acts in A: ARRIVAL -> HUNT at its creation, READY / EVENT when Link is in its range, its event 0x0D runs and reloads sea:14 at the minigame spawn, the new TagEv hunts again; --admp-exact tev.admp PASS (1193 records), strict yes; control (TEST_UNCONVERT 0x199, same FROM, 'test unconversion profiles: 0199') strict yes; converted B = control B on all 1193 common records, Link trace / camera identical, sound identical through frame 4353 (P one frame longer at the run end); lint14s: CLEAN (no dt-scaled call under the shared-world-state gate); compare60 pin 4 264e00fd, re-judged with pin 6 8b888908 (same verdicts); 2026-11-22 */
    0x019B, /* tag_msg: the message tag TagMsg / TagMsg2 (the shared-world-state rule whole-execute form; the own-state proof rule); pair: warp t1.3 sea:44:3 (Outset), Link put on the TagMsg2 0FFF087F (-202700 60 317650) at scenario step 150 (WWHD_LINK_WARP), A every 1.5 s 6-12 s; EVENTS60, CONV 0x19B from 1 s; the tag acts in A: ARRIVAL -> HUNT, EVENT when Link is in range (its message: msg_mode 0..5, l_msgId, mem dump 1046E5F8), WAIT once the box closes; --admp-exact tms.admp PASS (601 records); P and control (TEST_UNCONVERT 0x19B, same FROM, 'test unconversion profiles: 019B') fail A on the same verdict lines (link-procs, link-motion, camera, sound, procs: the route's); converted B = control B on all 601 tms records and the 602 common statics records, Link trace / camera identical, sound identical through frame 4155 (X one frame longer at the run end); lint14s: CLEAN; compare60 pin 4 264e00fd and pin 6 8b888908 (same verdicts); 2026-11-22 */
    0x006A, /* warpls: the tower warp light Ywarp00 (whole form, the whole-frame form rule, d/t60_objw.h; the own-state proof rule); pair: switch 0x28 poked (testWordBit: live info+0x7A2 mask 01, Siren table copy +0x45E) at 1.0 s, warp t1.3 Siren:7:0 (Ywarp00 00010128 created active), stick forward 5-6.5 s (Link leaves the start-in-warp), Link put on the warp at scenario step 300 (WWHD_LINK_WARP); EVENTS60, CONV 0x6A from 1 s; the warp acts in A: mPlayerStartedInWarp 1 -> 0, mEvtState Warping -> the TOWER_WARP event (128 frames) -> stage change, the destination Ywarp00 then runs its start-in-warp; P and control (TEST_UNCONVERT 0x6A, same FROM, 'test unconversion profiles: 006A') fail A on the same verdict lines (link-procs, link-motion, camera, sound, procs; admp: the emitter pointer +0x3C0 and (pin 6, +0x3C0 ptr) mEvtState +0x3CC 2@300 in both: the route's event timing); converted B = control B on all 593 common records, Link trace / camera identical, sound identical through frame 4210 (P two steps longer); compare60 pin 4 264e00fd and pin 6 8b888908; 2026-11-22 */
    0x0138, /* swtact: the Wind Waker song crest SWtact / SWtactB (whole form, the whole-frame form rule, d/t60_objw.h; the own-state proof rule); its only placement (raw scan of all 887 stage archives): Cave06 room 0 SCOB SWtactB 00010012 (switch 0x12, answer 0 = Wind's Requiem); pair: noon / Wind Waker / songs pokes (mdpair route), warp t1.3 Cave06:0:0, Link put on the crest at scenario step 120, Y at 6 s, right stick Wind's Requiem 9.0-11.6 s, A at 18 s; EVENTS60, CONV 0x138 from 1 s; the crest acts in A: mPlayerStatus 0 -> 1 (conducting), mTrigger 1 on the matched song, onSwitch 0x12 and mTrigger 0 when the song event ends; P and control (TEST_UNCONVERT 0x138, same FROM, 'test unconversion profiles: 0138') fail A on the same link-motion / camera / sound lines; swt.admp: +0x3C0 (mAnswer / mTrigger / mPlayerStatus) differs from A on 2 frames in BOTH (mPlayerStatus back to 0 one frame early: Link's conduct status, the route's); P's admp line reads PASS only because its per-step records let compare60's draw-written mask (B's first-step record = A's) take those 2 frames, X's has no first-step records: the same data; converted B = control B on all 653 records, Link trace / camera / sound identical; compare60 pin 4 264e00fd and pin 6 8b888908; 2026-11-22 */
    0x00C4, /* the wind-direction demo bird WBird (whole form, the whole-frame form rule, d/t60_objw.h: the shared-world-state form kept dt 1/2 for fopAcM_posMove, pair; the own-state proof rule); pair on bin m8wb: noon / Wind Waker / songs pokes (mdpair route), warp t1.3 sea:44:0, Y at 6 s, right stick Wind's Requiem 9.0-11.6 s, A at 18 s; EVENTS60, CONV 0xC4 from 1 s; the bird acts in A (285 records, its flight +304 / +308 ... ~60x, mAction / mTimer +3B8 92x); P and control (TEST_UNCONVERT 0xC4, same FROM, 'test unconversion profiles: 00C4') fail A on the same verdict lines (link-motion, camera, sound, admp: the same wbird words in both, the route's); converted B = control B on all 286 records, Link trace / camera identical, sound identical through frame 4561 (X one start more on the last step); compare60 pin 6 8b888908; 2026-11-22 */
    0x019D, /* tag_island: the island arrival / Wind Waker lesson tag TagIsl (whole form, the whole-frame form rule, d/t60_objw.h; the own-state proof rule); its only placement: sea r11 (Windfall) layers 2/3 SCOB 1CFF5B04 (event 0x1C, switch 0x5B, type 4 ARRIVAL_TWN, area 6000); pair (= + tag_island form): ENDLESS_NIGHT 0x0A02 poked (info+0x64E = 02) at 1.0 s (arrivalTerms: arrival bit 0x1F04 clear, night stop), warp t1.3 sea:11:12:2, A every 1.5 s 5-30.5 s; EVENTS60, CONV 0x19D from 1 s; the tag acts in A: ARRIVAL -> HUNT -> READY (3621) -> EVENT (3622, the arrival event 3623-4137, demoInitProc sets 0x1F04 and switch 0x5B) -> WAIT (4085); --admp-exact tisl.admp PASS (1021 records); P and control (TEST_UNCONVERT 0x19D, same FROM, 'test unconversion profiles: 019D') fail A on the same verdict lines (link-procs, link-motion, sound, procs: the route's); converted B = control B on all 1021 records, Link trace / camera / sound identical; lint14s: no shared-world-state gate left; compare60 pin 7 cf6dfeac; 2026-11-22 */
    0x01C4, /* tag_ghostship: the ghost ship clear tag PScnChg (the shared-world-state rule whole-execute form; the own-state proof rule); pair: warp t1.3 PShip:2:0, the HIO debug switch l_HIO.mForceClear (1046E2C5) = 1 at 5 s (modeClearWait takes it as a DEFAULT_TREASURE end; no chest route); EVENTS60, CONV 0x1C4 from 1 s; the tag acts in A: mMode CLEAR_WAIT 0 -> CLEAR_EVENT 1 (3692), the PSHIP_CLEAR event (3695-3811: its cuts, WARAIGOE sound), setNextStage sea (stage change 3809); --admp-exact tgs.admp PASS (200 records); P and control (TEST_UNCONVERT 0x1C4, same FROM, 'test unconversion profiles: 01C4') both strict yes; converted B = control B on all 200 records, Link trace / camera / sound identical; lint14s: CLEAN; compare60 pin 7 cf6dfeac; 2026-11-22 */
    0x0127, /* the bomb flower's bomb daBomb2 (whole form, the whole-frame form rule carryables; picture set_mtx without the fuse shift / ignition sound the own-state proof rule); route bf.sh (bf1: warp t1.3 kindan:1:0, Hero's Sword poke, Link warped to the bomb flower at origin step 200, A pluck 8 s, A throw 10 s, keyed seq RNG, ORIGIN_LOAD 2, END 15); dump at the profile execute 020CB414 (0x9A8); bin m2c wwhd-b5 (=), A reused from pair (TEST_CONVERT 0x127 FROM 6), control (TEST_UNCONVERT 0x127, same FROM, 'test unconversion profiles: 0127'), NC bfN2 (WWHD_T60_BOMB2SND_NEGCTRL=1: fuse sound 241 starts, A 111); the bomb acts in A (209 records, fuse sound 0x6100 111 starts, explosion); P and control fail A on the same verdict lines (link-procs, link-motion, camera, sound, admp: the route's); converted B = control B on every bomb2 logic word (209 common records; only the tevStr view-space light words +0x110..118 / +0x1D0..1D8 at 303-341, picture state), Link trace identical, camera identical on every tick30 record (39 in-between records differ at 302-339: a LOCK camera reading the bomb's picture position: picture reading picture), sound identical over the common window (P one frame longer); compare60 pin 7 cf6dfeac; M2e 2026-11-22 */
    0x006C, /* the Hyrule descent warp Gdemo20 (whole form, the whole-frame form rule, d/t60_objw.h; the own-state proof rule); pair: event bit 0x2D08 poked (info+0x671 = 08) at 1.0 s, warp t1.3 sea:26:200 (spawn 200 prm 24FF201A: start mode 2 on the ship, on sea room 26's Gdemo20 at 100000 0 0); EVENTS60, CONV 0x6C from 1 s; the warp acts in A (mVisible, current.pos.y and +0x400 every frame, the +0x408.. fade from 3730); P and control (TEST_UNCONVERT 0x6C, same FROM, 'test unconversion profiles: 006C') fail A on the same verdict lines (camera 800@192 the route's warp, sound, save; admp --admp-exact: +0x304 / +0x318 / +0x400 the same counts in both); converted B = control B on all 721 records, Link trace / camera identical, sound identical over the common window (X one step longer at the run end: its last step's loop starts); compare60 pin 7 cf6dfeac; 2026-11-22 */
    0x0041, /* obj_warpt: the warp pots Warpt / Warpnt / Warpts1-3 (whole form, the whole-frame form rule, d/t60_objw.h; ride_CB gated on t60_converted(0x41) + t60objw::cbSkip; the own-state proof rule); pair: warp t1.3 kindan:0:2 (spawn on Warpts1 03020112), stick 3-4.5 s off the pot, Link put 250 above the pot centre at scenario step 330 (WWHD_LINK_WARP): he drops into the pot, ride_CB -> the DEFAULT_WARP event (0x3AC / +0xF8, its open sound 0x3019), normalWarp false (no warp switch), back to the wait mode, Link climbs out; EVENTS60, CONV 0x41 from 1 s; P and control (TEST_UNCONVERT 0x41, same FROM, 'test unconversion profiles: 0041') fail A on the same verdict lines (link-procs, link-motion, camera, sound, admp --admp-exact with the same word counts: the route's); converted B = control B on all 533 common records (P 3 more at the run end), Link trace / camera identical, sound identical over the common window (aligned by B's 2-step load offset; P two steps longer at the end); compare60 pin 7 cf6dfeac; 2026-11-22 */
    0x006E, /* warpmj: the warp to the Tower of the Gods Warpmj (whole form, the whole-frame form rule, d/t60_objw.h; the own-state proof rule); pair: event bit 0x3D02 poked (info+0x681 = 02; _create errors without it) at 1.0 s, warp t1.3 sea:1:16:3 (spawn 16 prm 34FF2001: start mode 2 on the ship, on the layer-3 Warpmj); EVENTS60, CONV 0x6E from 1 s; the warp acts in A (its state words +0x3E4.. 168x, the warp event from 3756); admp with (pos, the three emitters as ptr, mDestination / mOrder / mEventIndex / mDemoState / mStaff) + --admp-tevstr wmj:0x3E4 (mWarpTev, the warp's own light block, set in _draw) PASS in P and control (TEST_UNCONVERT 0x6E, same FROM, 'test unconversion profiles: 006E'), both fail A on the same camera / sound lines; converted B = control B on all 653 records; Link trace / camera: a run-end 3-row reorder at 4126-4128 (Link's single dt-1 step there one half-step later) in the controls only, not in the control rerun with WWHD_T60_TRACE (wmj2xt = P-B and wmj2t exactly): host step pacing, not the subject's conversion; sound: X's known pre-origin start-up loop start; compare60 pin 7 cf6dfeac; 2026-11-22 */
    0x01A3, /* tag_md_cb: the Medli / Makar companion tags TagMd1 / TagCb1 / TagMd11-16 / TagCb11-14 (the shared-world-state rule whole-execute form, d_a_tag_md_cb.cpp:635; the own-state proof rule); pair: warp t1.3 M_Dai:1:1, stick forward 4-6 s (Link ~ -7004 -150 2065, room 1); at 8 s Medli (mpPlayerPtr[1], play+0x5B38) put 150 from Link (home / old / current pos -6854 -150 2065) in room 1 and switch 3 set (info+0x79F = 08); A every 1.5 s 12-21 s; EVENTS60, CONV 0x1A3 from 1 s; TagMd11 (M_Dai r1 SCOB FFFF19F0, kind 2, mSwitchNo 3) acts in A: mTimer 150 -> 0, the event (3928-4139, message 0x19F0), mEventIdx -1 at the end; TagMd12 (kind 3) idle; --admp-exact mdcb.admp PASS (1306 records); P and control (TEST_UNCONVERT 0x1A3, same FROM, 'test unconversion profiles: 01A3') fail A on the same verdict lines (camera 2.31 @158, sound B +2: the route's); converted B = control B on all 1306 records, Link trace / camera / sound / save identical; lint14s: CLEAN; second route (Makar) cbp1: warp t1.3 kaze:1:1, at 8 s Makar (mpPlayerPtr[1]) put at -3700 1050 800 in room 1 (TagCb11 FFFF1530, kind 8, area 400): the tag acts in A (mTimer 150 -> 0, event 3935-4191, message 0x1530), --admp-exact PASS (653 records), P and control both strict yes, converted B = control B on all 653 records, Link trace / camera / sound / save identical; compare60 pin 7 cf6dfeac; succ109 2026-11-22 */
    0x006B, /* the Hyrule warp light Ghrwp (whole form, the whole-frame form rule, d/t60_objw.h; the own-state proof rule); pair: event bit 0x3201 set at 1.0 s (info+0x676 0x82 -> 0x83: without it the ship's forced ride message 0x1645 (0x2D02 set, 0x3201 unset) holds a Link <-> ship talk, event mode 1, after the start event), warp t1.3 Hyrule:0:219 (start mode 2 on the ship), A every 1.5 s 10-19 s, at 20 s the ship (mpPlayerPtr[2]) put at x z 0 1300 (349 from the Ghrwp at 0 1649); EVENTS60, CONV 0x6B from 1 s; the warp acts in A: check_warp (ship ride status 0x10000, ship within 500) -> mWarpOrder 2 (4128) -> the TO_SEA_WARP event (4129, staff 0x102) -> stage change to the sea; P and control (TEST_UNCONVERT 0x6B, same FROM, 'test unconversion profiles: 006B') fail A on the same verdict lines (link-motion, camera, sound, admp: the route's event timing (pin 7, emitter pointers ptr) both +0x3C8 mpWarpEmitter null-ness 1@111 and +0x420 mStaffIdx @247); converted B = control B on all 808 records, Link trace / camera / save identical, sound identical through 4515 (P 4 steps longer at the run end); compare60 pin 7 cf6dfeac; succ109 2026-11-22 */
    0x019C, /* tag_etc: the Medli flying camera tag TagMd (the shared-world-state rule whole-execute form; the own-state proof rule); its only placement: Adanmae r0 SCOB TagMd 00000001 (event 0 MD_FLY, type2 0: Medli flying within 800 XZ / 0..800 above -540 600 -1200; the tag never orders its event: HUNT sets / clears Medli's CamTagIn 0x20); pair: event bits 0x2E04 / 0x1608 / 0x1102 cleared at 1.0 s (Medli TYPE_ADANMAE is created), warp t1.3 Adanmae:0:4, Link put on the tag ledge at scenario step 150, Medli (47C56180) put in front of him at 5 s, A every 1 s 6.5-21.5 s: talk -> squatdown -> carry -> throw -> gliding (daMdStts_FLY) over the tag; EVENTS60, CONV 0x19C from 1 s; the tag acts in A: SEARCH -> HUNT, Medli's 0x20 set 3972-4014 and 4086-4131; --admp-exact etc.admp PASS (653 records); P and control (TEST_UNCONVERT 0x19C, same FROM, 'test unconversion profiles: 019C') fail A on the same verdict lines (link-procs, link-motion, camera: the route's); converted B = control B on all 653 tag records and all 721 common Medli records, Link trace / camera identical, sound identical over the common window; lint14s: CLEAN; compare60 pin 7 cf6dfeac; 2026-11-22 */
    /* (the end of the list: append new entries ABOVE this line; no 0 terminator, the array is sized) */
};
/* every entry reachable (2026-11-19: a 0 terminator stood before 8 appended entries and made them inert): no 0 entry,
 * no duplicate profile; checked at compile time */
constexpr bool t60_converted_list_ok() {
    constexpr int n = (int)(sizeof(kT60Converted) / sizeof(kT60Converted[0]));
    for (int i = 0; i < n; i++) {
        if (kT60Converted[i] <= 0) return false;
        for (int j = i + 1; j < n; j++)
            if (kT60Converted[i] == kT60Converted[j]) return false;
    }
    return true;
}
static_assert(t60_converted_list_ok(), "kT60Converted: a 0 / negative entry or a duplicate profile");
inline bool t60_converted(int profName) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    /* tests (port t60.h kT60TestWindowClosed -2: WWHD_T60_TEST_CONVERT_FROM before its time): nothing converted, this list
     * included, so both runs of a pair execute alike at 30 Hz until the window opens (cckA*: the default-converted
     * ChuChu drew its idle timer before the keyed test RNG's origin, from the game RNG in another cross-process order) */
    if (wwhd_t60_test_convert == -2) return false;
    /* tests (WWHD_T60_TEST_UNCONVERT): the excluded profiles run unconverted (0 = no list: one load, no call) */
    if (wwhd_t60_test_unconvert != 0 && wwhd_t60_test_unconverted(profName)) return false;
#endif
    for (short p : kT60Converted)
        if (p == profName) return true;
#if defined(GABI_GAME_BUILD) && GABI_T60
    if (wwhd_t60_test_convert != 0 && wwhd_t60_test_converted(profName)) return true;
#endif
    return false;
}
