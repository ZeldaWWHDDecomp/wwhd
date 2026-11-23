/* c_damagereaction_carrier_t60.h (WWHD, true 60 build, 2026-11-20, rider rule): a converted
 * enemy of the damage reaction (dr_body_bg_check) standing on a 30 Hz carrier runs at 30 Hz, as Link does
 * (d_a_player_main_04.cpp lk_t60CarrierUnconverted). Decomp build only.
 * Separate file: c_damagereaction.cpp keeps a thin call.
 *
 * A carrier whose actor is unconverted, or converted in the whole form (d/t60_whole_carrier.h), moves its BG once a
 * 30 Hz frame. A per-step rider then checks its ground against the carrier's previous-frame BG on the in-between step
 * where A's rider (later in the line) checks against this frame's: pair (the ed07x, a Bokoblin on the
 * M_NewD2 room 7 rope bridge, bridge unconverted): frame 182, B's first step found the plank (ground snap, speed.y -1),
 * A's frame did not (speed.y -13); the walk then split (current.pos from frame 182). The enemy asks the hold (true60.h
 * t60_hold30_request) on each step while coupled (standing on such a carrier, or within kKeep frames of it); it runs on
 * the tick30 step at dt 1 from the next frame, the original form. Game build only (the harness never holds); at N 1 nothing is asked (dt-1 check unchanged). */
#pragma once
#include "true60.h"
#include "c/c_damagereaction.h"
#include "d/t60_whole_carrier.h"

#include <unordered_map>

namespace t60drCarrier {
/* the coupling outlasts a short hop: a rider on a swaying carrier is in the air on every other frame of A (sbh1n2, the
 * Bokoblin on the rope bridge: speed.y -1 / -13 alternating), so the hold is asked for kKeep frames after the last
 * frame it stood on such a carrier (8: a hang's end falls back onto the planks within it, sbm1n2 frames 325-328): on a swaying carrier A's rider alternates contact / airborne, and a held frame is A's
 * own form, so a longer hold never changes the logic. Host map per including unit (c_damagereaction.cpp for the ground
 * check, d_a_bk_action.cpp for b_hang; each asks the hold itself): enemy address -> the frame tag of its last coupling. */
enum { kKeep = 8 }; /* the bridge's own ride timer: ride_call_back sets dr.mBridgeRideTimer 8 each ridden frame */
#if defined(GABI_GAME_BUILD) && GABI_T60
static std::unordered_map<u32, u32> s_lastOn;
#endif
#if defined(GABI_GAME_BUILD) && GABI_T60
/* a 30 Hz carrier: the actor is unconverted (or unknown), or ran in the whole form this frame */
static inline bool thirtyHz(u32 carrier) {
    return carrier == 0 || !t60_converted((s16)gabi::at<fopAc_ac_c>(carrier)->mProfName) || t60wc::ranThisFrame(carrier);
}
/* the hold while coupled: coupled now (on), or within kKeep frames of the last coupled frame */
static inline void hold(u32 me, bool on) {
    const u32 tag = t60wc::frameTag();
    if (on)
        s_lastOn[me] = tag;
    auto it = s_lastOn.find(me);
    if (it == s_lastOn.end())
        return;
    if (tag - it->second > (u32)kKeep) { /* uncoupled (or a stale entry of a freed address) */
        s_lastOn.erase(it);
        return;
    }
    t60_hold30_request(gabi::load<u32>(me + 4) /* base.mBsPcId */);
}
#endif
/* after the body ground check (dBgS_Acch::CrrPos) of a converted enemy's step; checked false: the check was skipped
 * this step (mBodyBgCheckDelay: a hang, a knock-back): coupled while it hangs on a bridge (dr.mHangBridgeId, set by
 * the bridge's ride_call_back; the hang re-sets the delay every frame), else only an earlier coupling keeps the hold */
static inline void check(damagereaction* dr, bool checked = true) {
    fopAc_ac_c* enemy = dr->mpEnemy;
    dBgS_Acch* acch = &dr->mAcch;
#if defined(GABI_GAME_BUILD) && GABI_T60
    /* asked on frames of N steps, also while held (dt 1 on the tick30 step): the hold lasts as long as the coupling */
    if (t60::steps() <= 1)
        return;
    bool on = false;
    if (checked && acch->ChkGroundHit() && acch->m_ground_h != -1000000000.0f) {
        cBgS_PolyInfo* gnd = static_cast<cBgS_PolyInfo*>(&acch->m_gnd);
        u32 bgs = gabi::ea(dComIfG_Bgsp());
        if (gabi::call<BOOL>(0x02008254, bgs, gabi::ea(gnd)) /* cBgS::ChkPolySafe */ &&
            gabi::call<BOOL>(0x024EEABC, bgs, gabi::ea(gnd)) /* dBgS::ChkMoveBG */)
            on = thirtyHz(gabi::call<u32>(0x02008438, bgs, (u32)gnd->mBgIndex)); /* cBgS::GetActorPointer */
    }
    if (!checked && (u32)dr->mHangBridgeId != 0xFFFFFFFFu && dr->mBodyBgCheckDelay != 0) {
        fopAc_ac_c* bridge = fopAcM_SearchByID((u32)dr->mHangBridgeId);
        on = bridge != nullptr && thirtyHz(gabi::ea(bridge));
    }
    hold(gabi::ea(enemy), on);
#else
    (void)enemy, (void)acch, (void)checked; /* the verification harness: never held */
#endif
}
/* an explicit coupling to a carrier actor (the Bokoblin's b_hang on a rope bridge: it follows the bridge's side
 * position, which moves once a 30 Hz frame; pair frame 190: the per-step chase took another path) */
static inline void couple(fopAc_ac_c* enemy, fopAc_ac_c* carrier) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    if (t60::steps() <= 1)
        return;
    hold(gabi::ea(enemy), carrier != nullptr && thirtyHz(gabi::ea(carrier)));
#else
    (void)enemy, (void)carrier;
#endif
}
}  // namespace t60drCarrier
