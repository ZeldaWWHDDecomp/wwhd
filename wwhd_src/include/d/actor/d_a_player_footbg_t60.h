/* d_a_player_footbg_t60.h (WWHD, true 60 build, 2026-11-20, the camera's move start,
 * irol pair). Decomp build only. Separate file:
 * d_a_player_main_03.cpp keeps thin calls.
 *
 * footBgCheck 0240005C (execute, right after setWorldMatrix) probes the ground under both feet from the foot records'
 * toe / heel positions (mFootData, written by posMoveFromFootPos from the joint matrices of the last model calc), eases
 * mFootBgOffsetY to the lower foot's ground below pos.y and adds it to the model base matrix y (setAttentionPos reads
 * that y + 92.5: the follow camera's centre chases it; the base matrix places the legs' IK and so the next toe joints),
 * eases the leg angles mLegAAngle / mLegBAngle / field_0x004 (cLib_addCalcAngleS, half the rest per call; the leg IK of
 * the next calc, which moves the toe joints the foot travel and the next probe read) and counts the foot's stop timer.
 * At 30 Hz frame f reads the pose T(f-1) and eases once. At N steps the tick30 step read the pose after the frame's
 * first step's calc and every value eased N times (ym83c entry frame A 3611 / B 3609: attention y 1450.357 / 1450.589,
 * pos and m35C4 exact; the camera centre y split from it and the yaw quantum at 3613).
 * Form (the process-order read rule frame-start reads, whole-frame form whole form): on the frame's later steps the call reads
 * the frame's first-step foot records (toe, heel and foot position of both feet: the pose the first step saw = A's
 * T(f-1)) and starts from the frame's values of mFootBgOffsetY and the leg angles, so each step eases once from the frame
 * start (the tick30 step = the original frame's); the ground probes stay at the step's own position (the per-step ground-check rule). The
 * stop timer counts on the tick30 step only (the 30 Hz timer rule; earlier steps' counts are put back). The step's own toe / heel
 * records go back after the call (posMoveFromFootPos owns them). A frame whose first step did not come here: the step's
 * own values. dt 1 (N = 1): never (the original). Host memory, one player. */
#pragma once
#include "true60.h"

struct LkT60FootBg {
    uint32_t stamp;           /* step counter of the frame's first step */
    uint8_t fs[2][0x30];      /* both foot records 0x00..0x2F as the frame's first step's call began */
    uint8_t fsOff[4];         /* mFootBgOffsetY then */
    uint8_t own[2][0x18];     /* this step's heel / toe (0x0C..0x23), put back after the call */
    uint8_t timer[2];         /* the stop timers as this step's call began */
    bool later;               /* this call substituted the frame's records */
    bool active;              /* this call is under the form (dt != 1) */
    bool valid;
    float prevDt;             /* the step's dt, put back at the end (the call's eases run at dt 1:) */
};
static LkT60FootBg s_lkT60FootBg;

/* guest memory byte copies through gabi load / store (no raw host access: the verify harness does not map guest memory) */
static inline void lkT60_fbRead(uint8_t *dst, uint32_t ea, uint32_t n) {
    for (uint32_t i = 0; i < n; i++)
        dst[i] = gabi::load<uint8_t>(ea + i);
}
static inline void lkT60_fbWrite(uint32_t ea, const uint8_t *src, uint32_t n) {
    for (uint32_t i = 0; i < n; i++)
        gabi::store<uint8_t>(ea + i, src[i]);
}

/* top of footBgCheck: fd = the two foot records (0x118 bytes each), off = mFootBgOffsetY (guest addresses) */
static inline void lkT60_footBgBegin(uint32_t fd, uint32_t off) {
    LkT60FootBg &s = s_lkT60FootBg;
    s.active = false;
    s.later = false;
    if (t60::dt() == 1.0f || t60::steps() <= 1)
        return;
    s.active = true;
    const uint32_t now = t60_step_counter();
    if (t60::first_step()) {
        s.stamp = now;
        for (int i = 0; i < 2; i++)
            lkT60_fbRead(s.fs[i], fd + i * 0x118, 0x30);
        lkT60_fbRead(s.fsOff, off, 4);
        s.valid = true;
    } else if (s.valid && now - s.stamp < t60::steps()) {
        s.later = true;
        for (int i = 0; i < 2; i++) {
            const uint32_t f = fd + i * 0x118;
            lkT60_fbRead(s.own[i], f + 0x0C, 0x18);
            lkT60_fbWrite(f + 0x01, s.fs[i] + 0x01, 1);    /* mStopTimer */
            lkT60_fbWrite(f + 0x04, s.fs[i] + 0x04, 6);    /* field_0x004, mLegBAngle, mLegAAngle */
            lkT60_fbWrite(f + 0x0C, s.fs[i] + 0x0C, 0x24); /* mHeelPos, mToePos, mFootPos */
        }
        lkT60_fbWrite(off, s.fsOff, 4);
    }
    /* (c_lib run converted in N-step spec): each step eases ONCE FROM THE FRAME START (the form above), i.e. the original's
     * one ease at dt 1: at the step's dt the eases took a dt share, and the int eases' frame entries restarted as partial
     * frames on the frame-start write-back (N-step spec footBgCheck r3+0x73F2 mLegBAngle 909/2000). The call runs at dt 1. */
    s.prevDt = t60_set_dt(1.0f);
    for (int i = 0; i < 2; i++)
        s.timer[i] = gabi::load<uint8_t>(fd + i * 0x118 + 0x01);
}

/* end of footBgCheck */
static inline void lkT60_footBgEnd(uint32_t fd) {
    LkT60FootBg &s = s_lkT60FootBg;
    if (!s.active)
        return;
    for (int i = 0; i < 2; i++) {
        const uint32_t f = fd + i * 0x118;
        if (s.later)
            lkT60_fbWrite(f + 0x0C, s.own[i], 0x18);
        if (!t60::tick30())
            gabi::store<uint8_t>(f + 0x01, s.timer[i]);
    }
    t60_set_dt(s.prevDt);
    s.active = false;
}
