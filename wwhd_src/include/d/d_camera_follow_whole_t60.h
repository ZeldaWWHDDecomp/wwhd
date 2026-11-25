/* d_camera_follow_whole_t60.h (WWHD, true 60 build, 2026-11-20, rule (A) on the
 * irol yaw pairs). Decomp build only. Separate file:
 * d_camera_run.cpp and f_pc_executor.cpp keep thin calls.
 *
 * The follow camera's frame ends exact: a frame that STARTS in a follow-engine style (dCamAlg_FOLLOW_CAMERA_e) runs the
 * per-step camera on its earlier steps for the picture only, and on its tick30 step the camera's frame-start state (Run's
 * rollback copy, d_camera_run_t60.h: the dCamera_c object and its com status block, taken at the top of the first
 * step's Run) is put back and Run runs ONCE for the whole frame at dt 1: the original's arithmetic on the frame-end world.
 *
 * Why: / (Outset irol, Link standing still while the camera settles): the centre's per-frame chase increments
 * are one or two float32 ulps (1/32 at z ~ 319365). A's one whole-frame add rounds to 0.0625; B's two half-size adds
 * round away or one ulp short, so B's centre stalls (from 3580 z .438 vs A .875) and the yaw read-back (an integer
 * direction about the centre) sits two 9.5-unit quanta off; 241 frames differ, the 3668 split follows. No per-step ratio
 * form reproduces float rounding of the frame's add; the frame-end recompute does, bit-exact, for centre, eye, direction
 * and every other camera field together (the yaw / latitude read-backs included).
 *
 * Readers of the in-between camera (check): the first step's per-step values exist from the camera's first-step
 * Run to the start of the tick30 step. (1) Processes before the camera in the line read it on the first step before it
 * ran (the frame-start = A's previous frame end). (2) On the tick30 step every process executes after the restore below
 * (the tick30 step's first process execute puts the frame-start state back, before any reader; the camera then runs the
 * whole frame from it): Link (dCam_getControledAngleY, already frame-start by d_a_player_camyaw_t60.h, and his other
 * dComIfGp_getCamera reads), the unconverted actors (NPC look-at, the ship, warp, vibration ...) all see A's value (the
 * previous frame end), as A's frame f reads it before the camera's frame-f Run. (3) The first step's picture (drawn on the
 * first step's pass) shows the per-step values; the tick30 picture the whole frame's end. (4) Writes into the camera made
 * on the first step AFTER the camera's Run (processes later in the line than the camera) are dropped by the restore (none
 * known on the routes; the camera runs late in the line). The camera's sound requests gathered on the earlier steps
 * (seFrameEdges) are dropped: the whole run makes the frame's requests, as A's one Run.
 * dt = 1 (N = 1): never (the original). Host memory, per camera body. */
#pragma once
#include "true60.h"

#if defined(WWHD_T60_CAM_WHOLE_IMPL)
#include <cstdlib>
#include "d/d_camera_key_t60.h"
#include "d/d_camera_run_t60.h"
#include "d/d_camera_subjbase_t60.h"

namespace camera_follow_whole_t60 {
constexpr u32 kFollowAlg = 1; /* dCamAlg_FOLLOW_CAMERA_e (d_camera_mode.h) */
/* (accepted): the lock-on camera (dCamAlg_LOCKON_CAMERA_e) in the same whole form. The own-state proof control
 * pr88n of the parry pair (Link unconverted, bin71) first differs in the camera at frame 30 in a lock-on
 * style (type 25 mode 2 style 71, work "LOCK"; eye x +0.17, growing), 16 frames before Link's shape yaw (frame 46): the
 * per-step lock-on run does not end on the original's frame, and Link's camera-relative move target follows it. */
constexpr u32 kLockonAlg = 2;
/* (CANDIDATE, open): the ride camera (dCamAlg_RIDE_CAMERA_e: type 37 / style 36, Link on the ship) in the same
 * whole form. sailing frame form (pair sqPg / sqPi on the pship route) moves the carrier (the ship
 * 0xA5) inside the frame with exact frame ends; ship and Link then match A at every frame end, but the per-step ride run
 * chases a carrier that moves within the frame: its centre lags A's by the speed-ramp position-gap moving-target gap (a ratio r chase run as two
 * steps of 1 - sqrt(1 - r) against a target that moves half a frame per step: (1/2)(1 - s)/s - (1 - r)/r frames of
 * travel, 0.21 at r 0.5), from the launch at frame 56: B's centre z 6 -> 8 units behind at ~42 units a frame, eye 11.04
 * @70 (tol 2). The control (0xA5 unconverted: the ship moves once a frame) has camera 0. The whole run on the tick30
 * step recomputes the frame from the frame-start camera on the frame-end ship / Link: A's arithmetic. The ride form's
 * own per-step pieces (d_camera_ride_t60.h, the frame-end radius ease) are no-ops at dt 1; the ship rock block runs
 * once in the whole run (the restore takes back the first step's). */
constexpr u32 kRideAlg = 8;
/* M2f (s95b candidate rejected then for 12 @116, the coil demo eye fixed since): the hung
 * camera (dCamAlg_HUNG_CAMERA_e: the rope hang / swing, type 55 mode 18 style 35, work "HUNG") in the same whole form.
 * Rope pair (r95c recipe, CONV 168,0x1DC,0x1BE FROM 1): Link exact (0.001) over the swing, the per-step hung run's
 * chases (e2R centre follow / radius / latitude / fovy) chase a Link who swings within the frame: the speed-ramp position-gap moving-target gap
 * from the hang start at 174 (0.09) grows to eye 10.95 @235 (tol 2); the control (himo2 unconverted) has the same
 * drift. The whole run on the tick30 step recomputes the frame from the frame-start camera on the frame-end Link: A's
 * arithmetic. NC WWHD_T60_HUNGWHOLE_NEGCTRL=1 (the per-step hung run). */
constexpr u32 kHungAlg = 9;
static inline bool hungWholeOn() {
    static int s = -1;
    if (s < 0) {
        const char* e = getenv("WWHD_T60_HUNGWHOLE_NEGCTRL");
        s = (e && e[0] == '1') ? 0 : 1;
    }
    return s == 1;
}
/* (2026-11-25; the boomerang aim route at N 2: Link
 * draws the boomerang (BOOMERANG_SUBJECT) at frame 65 and the camera goes FLLW -> SUBJ (type 12 mode 11 style 45); the
 * switch frame started in the follow style (whole, exact), but the subject style's frames after it ran per step: its
 * entry glide eases centre / radius / latitude / yaw / fovy toward a goal made from Link (subjectCentre, the globe
 * default) by t = 1 / (end - mCurModeTimer) a frame, a moving goal, so the per-step share (sjR) undershot (radius frame
 * 66: B 62.620 vs A 62.714 at the frame end, 0.09 at 66, 17.3 by 96 with the right-stick aim on top), Link exact
 * throughout) the subject camera (dCamAlg_SUBJECT_CAMERA_e) in the same whole form: the earlier steps keep the per-step
 * run (the picture), the tick30 step runs the whole frame once at dt 1 from the frame start: A's arithmetic on the
 * frame-end Link, the stick read in A's place (the run's step words). NC WWHD_T60_SUBJWHOLE_NEGCTRL=1 (per step, as before). */
constexpr u32 kSubjectAlg = 4;
static inline bool subjWholeOn() {
    static int s = -1;
    if (s < 0) {
        const char* e = getenv("WWHD_T60_SUBJWHOLE_NEGCTRL");
        s = (e && e[0] == '1') ? 0 : 1;
    }
    return s == 1;
}
/* (2026-11-25; the Bokoblin tower route bdk at N 2: the
 * camera runs the tower style (towerCamera, work "TOWR", type 35 style 128) while Link stands and then runs; towerCamera
 * builds its yaw target from the eye's direction about the chased centre through a radius blend t = Bezier((radius - min)
 * / range) that is steep at the minimum radius, so the frame-end radius's last bits decide it: A's frame 92 (Link's
 * first running frame was 91) has t 0.333 (radius exactly on the 500.0 minimum), yaw target 18242, mLongitude 11674 ->
 * 13841 and the view yaw +1819; B's per-step run left the radius a few ulps above: t 0.019 / 0, the view yaw +947 or +59
 * by the origin's step parity, and Link, whose move target is the camera yaw, turned 174 short at frame 93 and the run
 * divided. The forward tilt (Run's forward check before the style) took one unit less a frame on B's tick30 step in a
 * mode-local whole run, so the frame-start restore has to be Run's, not towerCamera's.) the tower camera
 * (dCamAlg_TOWER_CAMERA_e) in the same whole form. NC WWHD_T60_TOWERWHOLE_NEGCTRL=1 (per step, as before). */
constexpr u32 kTowerAlg = 7;
static inline bool towerWholeOn() {
    static int s = -1;
    if (s < 0) {
        const char* e = getenv("WWHD_T60_TOWERWHOLE_NEGCTRL");
        s = (e && e[0] == '1') ? 0 : 1;
    }
    return s == 1;
}
/* (2026-11-25; a long route at N 2, Link
 * and the enemies exact: the camera leaves the lock-on for style 142 (dCamAlg_MANUAL_CAMERA_e, type 25 mode 12) at
 * frame 801; the manual style ran per step: its eye eases toward a goal made from Link each frame (a moving goal), the
 * per-step shares fell short: V 1710 / 1706 / 1700 for A's 1709 / 1704 / 1698, the eye 1.5 off at 802, 7.36 at 810, the
 * centre exact) the manual camera in the same whole form. NC WWHD_T60_MANUALWHOLE_NEGCTRL=1 (per step, as before). */
constexpr u32 kManualAlg = 10;
static inline bool manualWholeOn() {
    static int s = -1;
    if (s < 0) {
        const char* e = getenv("WWHD_T60_MANUALWHOLE_NEGCTRL");
        s = (e && e[0] == '1') ? 0 : 1;
    }
    return s == 1;
}
struct FollowWhole {
    uint32_t stamp;        /* t60_step_counter of the frame's first step */
    uint32_t objEa, comEa; /* the camera body and its com status block */
    uint32_t restoredStep; /* the tick30 step whose first execute put the frame start back */
    bool follow;           /* the frame started in a follow-engine style (or switched into one: switchIn) */
    bool valid;
    bool switchIn;         /* an earlier step's style decision entered a whole-form style */
};
inline camera_key_t60::PerCam<FollowWhole> g_followWhole;
/* top of Run on the frame's first step (after noteFrameStart): the frame's starting style family */
static inline void noteFrame(uint32_t objEa, uint32_t comEa, u32 alg) {
    if (t60::dt() == 1.0f || !t60::first_step())
        return;
    FollowWhole& f = g_followWhole();
    f.stamp = t60_step_counter();
    f.objEa = objEa;
    f.comEa = comEa;
    f.restoredStep = 0xFFFFFFFFu;
    f.switchIn = false;
    f.follow = alg == kFollowAlg || alg == kLockonAlg /* lock-on (see kLockonAlg) */ ||
               alg == kRideAlg /*: ride candidate (see kRideAlg) */ ||
               (alg == kHungAlg && hungWholeOn()) /* M2f: hung (see kHungAlg) */ ||
               (alg == kSubjectAlg && subjWholeOn()) /* (see kSubjectAlg) */ ||
               (alg == kTowerAlg && towerWholeOn()) /* (see kTowerAlg) */ ||
               (alg == kManualAlg && manualWholeOn()); /* (see kManualAlg) */
    f.valid = true;
}
/* Switch-in (2026-11-23; the follow form's counterpart of eventWholeSwitchIn). Boomerang
 * catch pair (frame 115 = step 3635): Link leaves BOOMERANG_SUBJECT on B's first step and the camera's
 * style decision on that step goes SUBJ (type 12 mode 11 style 45) -> LOCK (mode 2 style 70). The frame STARTED in the
 * subject style, so noteFrame did not mark it whole and the lock-on camera's first frame ran per step at dt 0.5: eye z
 * +1.36 at 115 (every frame before exact to 0.03), about 1 unit / 0.9 deg of yaw kept through the follow camera after
 * it (view V -163 vs A -173); Link's camera-relative MOVE heading took the 0.9 deg (MOVE entry angle 12219 vs 12187) and
 * his xz drifted ~11 units in 65 frames (link-tslack pos 11.75 @180 on bcF1). Form: a style decision on an earlier
 * (non-tick30) step that enters a whole-form style (follow / lock-on / ride / hung) in a frame that started outside one
 * marks the frame whole: the earlier steps keep their per-step run (the picture; the shown values stay per step on
 * this frame, no extrapolation from the previous style's motion), the tick30 step's first execute puts the frame start
 * back and the tick30 Run runs the whole frame once at dt 1 from it, taking the switch again on the frame-end world
 * (A's switch and A's first frame of the new style). Switch-out (a frame that starts whole and leaves mid-frame) is
 * already whole: the tick30 run from the frame start takes the switch itself. NC WWHD_T60_FWSWITCHIN_NEGCTRL=1 (the
 * per-step switch frame, as before). dt = 1: never. */
static inline bool fwSwitchInNegctrl() {
    static int s = -1;
    if (s < 0) {
        const char* e = getenv("WWHD_T60_FWSWITCHIN_NEGCTRL");
        s = (e && e[0] == '1') ? 1 : 0;
    }
    return s == 1;
}
/* after Run's style decision (the caller tests dt != 1 first) */
static inline void switchIn(u32 alg) {
    if (t60::dt() == 1.0f || t60::tick30() || fwSwitchInNegctrl())
        return;
    FollowWhole& f = g_followWhole();
    if (!f.valid || f.follow || !camera_run_t60::g_runFrameStart().valid ||
        camera_run_t60::g_runFrameStart().stamp != f.stamp)
        return;
    if (!(alg == kFollowAlg || alg == kLockonAlg || alg == kRideAlg || (alg == kHungAlg && hungWholeOn()) ||
          (alg == kSubjectAlg && subjWholeOn()) || (alg == kTowerAlg && towerWholeOn()) ||
          (alg == kManualAlg && manualWholeOn())))
        return;
    f.follow = true;
    f.switchIn = true;
}
/* the record is of the running frame (its first step recorded it, this is its last step) */
static inline bool ofThisFrame(const FollowWhole& f, uint32_t now) {
    return f.valid && f.follow && now - f.stamp == (uint32_t)(t60::steps() - 1) && camera_run_t60::g_runFrameStart().valid &&
           camera_run_t60::g_runFrameStart().stamp == f.stamp;
}
/* top of Run on the tick30 step: true = run the whole frame now at dt 1 from the frame start (put back here when the
 * tick30 step's first execute did not) */
static inline bool wholeOnTick30(void* self, uint32_t comEa) {
    if (t60::dt() == 1.0f || !t60::tick30() || t60::first_step())
        return false;
    FollowWhole& f = g_followWhole();
    const uint32_t now = t60_step_counter();
    if (!ofThisFrame(f, now))
        return false;
    if (f.restoredStep != now) {
        camera_run_t60::restoreFrameStart(self, comEa);
        subject_t60::applyCarry(gabi::ea(self)); /* (d_camera_subjbase_t60.h) */
    }
    f.valid = false;
    camera_run_t60::g_seFrameReq() = 0; /* the earlier steps' requests: the whole run makes the frame's */
    return true;
}
} // namespace camera_follow_whole_t60

/* the tick30 step's first process execute (f_pc_executor.cpp): each camera whose frame started in a follow style gets
 * its frame-start state back before any reader executes */
extern "C" __attribute__((visibility("default"))) void wwhd_t60_cam_tick30_start() {
    if (t60::steps() == 1 || !t60::tick30() || t60::first_step())
        return;
    const uint32_t now = t60_step_counter();
    static uint32_t s_last = 0xFFFFFFFFu;
    if (s_last == now)
        return;
    s_last = now;
    const int prev = camera_key_t60::g_cur;
    for (int s = 0; s < camera_key_t60::kCams; s++) {
        camera_key_t60::g_cur = s;
        camera_follow_whole_t60::FollowWhole& f = camera_follow_whole_t60::g_followWhole();
        if (camera_follow_whole_t60::ofThisFrame(f, now) && f.restoredStep != now) {
            camera_run_t60::copyToGuest(f.objEa, camera_run_t60::g_runFrameStart().obj, camera_run_t60::kCamObjSize);
            camera_run_t60::copyToGuest(f.comEa, camera_run_t60::g_runFrameStart().com, camera_run_t60::kCamComSize);
            camera_run_t60::restoreZoom(); /* the env light's HD zoom and the pad's subject base (Run's stores outside obj / com) */
            subject_t60::applyCarry(f.objEa); /* (d_camera_subjbase_t60.h) */
            f.restoredStep = now;
        }
    }
    camera_key_t60::g_cur = prev;
}

/* The shown camera on a follow-whole frame's earlier steps (on the walkcam judder,
 * the whole-frame form rule whole-form picture). The earlier steps' per-step Run chased a half-moved Link at the step's dt share
 * from the frame start, and the tick30 step's whole run carried the rest:
 * the drawn centre moved ~5.6 units on the first step and ~7.8 on the tick30 step while Link moved ~6.7 per step.
 * Form: camera_execute, after store (the lookat = the camera's shown values) and before view_setup: on the tick30 step
 * the shown eye / centre (camera_class + 0xDC / + 0xE8: the frame end) are noted with the change from the previous
 * frame's end (consecutive frames only); on an earlier step k of a frame that started in a follow style, the shown
 * eye / centre are the previous frame's end moved on by k/N of the previous frame's change, so every drawn step moves
 * its share. Nothing when the previous frame's change is unknown (the first frame, a frame at N 1 or a gap) or a cut
 * (> kPicCut units per frame): the per-step values stay then. Logic: the camera body (dCamera_c) is untouched, its
 * tick30 whole run and its frame end are as before; the lookat is rewritten by store on every step (the tick30 store
 * writes the whole frame's end). Readers of the lookat on an earlier step after the camera (view / culling) read the
 * shown values, as they read the per-step values before. Host memory, per camera body. N 1: never. */
namespace camera_follow_whole_t60 {
constexpr f32 kPicBlend = 0.5f; /* (b 0.5; walkcam: latency [4,4] -> [3,4], eye jerk 9.85 -> 6.21; 0 = the s89 form) */
constexpr f32 kPicBlendC = 0.0f; /* (part 2): the centre's blend (words 3-5); the eye (0-2) keeps kPicBlend */
constexpr f32 kPicCut = 150.0f; /* a camera cut: game units per 30 Hz frame (the metric's camera cut) */
struct PicLatch {
    uint32_t endStep; /* the tick30 step the frame end was noted on */
    f32 end[6];       /* shown eye xyz, centre xyz at that frame end */
    f32 dv[6];        /* the change from the frame end before it */
    bool valid, dvValid;
};
inline camera_key_t60::PerCam<PicLatch> g_pic;
} // namespace camera_follow_whole_t60
extern "C" __attribute__((visibility("default"))) void wwhd_t60_cam_follow_picture(uint32_t camEa) {
    using namespace camera_follow_whole_t60;
    if (t60::steps() <= 1)
        return;
    PicLatch& L = g_pic();
    const uint32_t now = t60_step_counter();
    const uint32_t n = (uint32_t)t60::steps();
    auto word = [camEa](int i) { return gabi::at<be<f32>>(camEa + (i < 3 ? 0xDC + 4 * i : 0xE8 + 4 * (i - 3))); };
    if (t60::tick30()) {
        f32 cur[6];
        for (int i = 0; i < 6; i++)
            cur[i] = *word(i);
        bool ok = L.valid && now - L.endStep == n;
        f32 de = 0.0f, dc = 0.0f;
        for (int i = 0; i < 6 && ok; i++) {
            L.dv[i] = cur[i] - L.end[i];
            (i < 3 ? de : dc) += L.dv[i] * L.dv[i];
        }
        L.dvValid = ok && de <= kPicCut * kPicCut && dc <= kPicCut * kPicCut;
        for (int i = 0; i < 6; i++)
            L.end[i] = cur[i];
        L.endStep = now;
        L.valid = true;
        return;
    }
    const FollowWhole& f = g_followWhole();
    const uint32_t k = now - f.stamp + 1; /* this step's place in the frame: 1 .. N - 1 */
    if (!f.valid || !f.follow || f.switchIn /* a switch-in frame keeps its per-step picture */ ||
        k < 1 || k >= n || !L.valid || !L.dvValid || L.endStep + 1 != f.stamp)
        return;
    const f32 share = (f32)k / (f32)n;
    /* (walkcam: an R-stick onset shows only on the tick30 refresh,
     * cam latency [4, 4] refreshes vs interp's [3, 2]; DC (per-step picture) [3, 4]): the shown value moves from the
     * extrapolation toward this step's own per-step value (the step's Run read this frame's input) by kPicBlend.
     * 0 = the s89 extrapolation, 1 = the per-step values (the DC picture). WWHD_T60_CAM_PIC_BLEND (0..1) overrides it
     * for measurement (read once). */
    static f32 s_blend = -1.0f;
    if (s_blend < 0.0f) {
        const char* e = getenv("WWHD_T60_CAM_PIC_BLEND");
        s_blend = e ? (f32)atof(e) : kPicBlend;
        if (!(s_blend >= 0.0f && s_blend <= 1.0f))
            s_blend = kPicBlend;
    }
    /* (part 2, walkcam: the follow centre's alt30 0.101 vs interp 0.064): the
     * per-step Run's centre chases a half-moved Link and trails the frame's linear share (the s89 note: ~5.6 of ~6.7
     * units on the first step), so a blended centre moves short / long on alternate refreshes (centre alt30 0.075 at
     * blend 0, 0.102 at 0.5, 0.161 at 1). The latency gain of the blend is in the eye (the R-stick orbit), so the
     * centre takes kPicBlendC (0: the s89 extrapolation) and the eye keeps its per-word kPicBlend (s98 q98g, ROT_NEWEST
     * 1: centre alt30 0.0748, eye alt30 0.0757 / jerk 6.28, latency 41.7 ms = interp). WWHD_T60_CAM_PIC_BLEND_C (0..1)
     * overrides the centre's blend for measurement (read once). */
    static f32 s_blendC = -1.0f;
    if (s_blendC < 0.0f) {
        const char* e = getenv("WWHD_T60_CAM_PIC_BLEND_C");
        s_blendC = e ? (f32)atof(e) : kPicBlendC;
        if (!(s_blendC >= 0.0f && s_blendC <= 1.0f))
            s_blendC = kPicBlendC;
    }
    for (int i = 0; i < 6; i++) {
        const f32 ext = L.end[i] + share * L.dv[i];
        *word(i) = ext + (i < 3 ? s_blend : s_blendC) * ((f32)*word(i) - ext);
    }
}
#elif defined(GABI_GAME_BUILD) && GABI_T60
extern "C" __attribute__((weak)) void wwhd_t60_cam_tick30_start();
static inline void t60_cam_tick30_start() {
    if (wwhd_t60_cam_tick30_start)
        wwhd_t60_cam_tick30_start();
}
extern "C" __attribute__((weak)) void wwhd_t60_cam_follow_picture(uint32_t camEa);
/* camera_execute after store: the shown eye / centre of a follow-whole frame's earlier step (see the impl above) */
static inline void t60_cam_follow_picture(uint32_t camEa) {
    if (wwhd_t60_cam_follow_picture)
        wwhd_t60_cam_follow_picture(camEa);
}
#else
static inline void t60_cam_tick30_start() {}
static inline void t60_cam_follow_picture(uint32_t) {}
#endif
