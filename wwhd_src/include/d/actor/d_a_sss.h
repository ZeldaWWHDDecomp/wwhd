/** Dexivine: WWHD actor layout. The phase and model members shift by +0x11C from GameCube;
 * each textured line material grows from 0x3C to 0x188, adding another +0x298 by mStts.
 * Unknown member names match tww. */
#pragma once
#include "gc_api/gc_api.h"

enum {
    dRes_INDEX_SSS_BCK_SSS_HIRAKU_e = 5,
    dRes_INDEX_SSS_BCK_SSS_TOJIRU_e = 6,
    dRes_INDEX_SSS_BMD_SSS_HAND_e = 9,
    dRes_INDEX_SSS_BTI_SSS_e = 0xC,
};
enum {
    JA_SE_OBJ_SVINE_OUT_WATER = 0x58F8,
    JA_SE_OBJ_SVINE_OUT = 0x58F7,
    JA_SE_OBJ_SVINE_GRASP = 0x58F9,
    JA_SE_OBJ_ATK_VINE_MP_SUCK = 0x7050,
    JA_SE_LK_LAST_HIT = 0x2828,
    JA_SE_OBJ_SVINE_CRASH = 0x69B0,
};

// HD: textured line material expanded from GameCube 0x3C to 0x188.
struct SssLine {
    /* 0x00 */ gptr<cXyz> mpSegments;
    /* 0x04 */ gptr<be<u8>> mpSize;
    /* 0x08 */ u8 mState[8];
};
WWHD_OFFSET(SssLine, mpSize, 0x4);
WWHD_SIZE(SssLine, 0x10);
struct SssLineMaterial {
    /* 0x000 */ u8 mPacket[0x130];
    /* 0x130 */ be<u32> __vtbl;
    /* 0x134 */ u8 mMaterial[0x50];
    /* 0x184 */ gptr<SssLine> mpLines;
    BOOL init(u16 numLines, u16 numSegments, void* texture, BOOL hasSize) {
        return gabi::call<BOOL>(0x025EBA58, this, numLines, numSegments, texture, hasSize);
    }
    // HD: textured line update takes the colour address and a 32-bit segment count.
    void update(s32 segments, const GXColor& color, dKy_tevstr_c* tev) {
        gabi::call(0x025ED1BC, this, segments, &color, tev);
    }
};
WWHD_OFFSET(SssLineMaterial, __vtbl, 0x130);
WWHD_OFFSET(SssLineMaterial, mpLines, 0x184);
WWHD_SIZE(SssLineMaterial, 0x188);
static inline SssLine* lineMat_lines(SssLineMaterial* l) { return l->mpLines; }
// HD: only the emitter fields inlined by Dexivine; translation flips Y for version >= 7.
struct SssEmitter {
    /* 0x000 */ u8 mEmitterData[0x5C];
    /* 0x05C */ be<s32> mMaxFrame;
    /* 0x060 */ u8 mParticleData[0x1F0 - 0x60];
    /* 0x1F0 */ Mtx34 mGlobalRotation;
    /* 0x220 */ u8 mScaleData[0xC];
    /* 0x22C */ cXyz mGlobalTranslation;
    /* 0x238 */ u8 mTransformData[0x254 - 0x238];
    /* 0x254 */ be<u32> mStatus;
    /* 0x258 */ u8 mStateData[0xA];
    /* 0x262 */ be<u8> mVersion;
    /* 0x263 */ u8 mPadding;

    void becomeInvalidEmitter() {
        u32 flags = mStatus;
        mMaxFrame = -1;
        mStatus = flags | 1;
    }
    void setGlobalTranslation(f32 x, f32 y, f32 z) {
        if (mVersion >= 7)
            y = -y;
        mGlobalTranslation.x = x;
        mGlobalTranslation.y = y;
        mGlobalTranslation.z = z;
    }
};
WWHD_OFFSET(SssEmitter, mMaxFrame, 0x5C);
WWHD_OFFSET(SssEmitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(SssEmitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(SssEmitter, mStatus, 0x254);
WWHD_OFFSET(SssEmitter, mVersion, 0x262);
WWHD_SIZE(SssEmitter, 0x264);

// HD: preserve the raw signed result width of the original local binding.
static inline s32 sss_at_power_check(void* info) { return gabi::call<s32>(0x02518DB0, info); }
// HD: this inlined sound path tests the eye-position address only.
static inline void sss_seStart(fopAc_ac_c* actor, u32 id) {
    if (gabi::ea(&actor->eyePos) != 0) {
        s32 reverb = dComIfGp_getReverb(actor->current.roomNo);
        mDoAud_seStart(id, &actor->eyePos, 0, reverb);
    }
}

struct sss_s {
    /* 0x00 */ cXyz field_0x00;
    /* 0x0C */ u8 _0C[0xC];
    /* 0x18 */ be<f32> field_0x18;
};
WWHD_OFFSET(sss_s, field_0x18, 0x18);
WWHD_SIZE(sss_s, 0x1C);

struct sss_class : fopAc_ac_c {
    /* 0x3AC */ u8 _3AC[0x3C8 - 0x3AC];
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D4 */ be<u8> mType;  /* was field_0x2B8: daSss_Create 0248F140: SSS_WATER_e (param byte 0 == 1) or SSS_LAND_e; daSss_Draw 0248EC3C draws and daSss_Execute 0248EEEC moves only while it is set; hand_move 0248F484 picks SVINE_OUT_WATER / SVINE_OUT by it */
    /* 0x3D5 */ be<u8> mOpenRangePrm;  /* was field_0x2B9: daSss_Create 0248F140: param bits 8-15; hand_move 0248F484 opens the hand (SSS_WAIT_e -> SSS_OPEN_e) when the player is within it * 10 (0xFF: 1000) and closes again 100 beyond */
    /* 0x3D6 */ be<u8> mSwitchNo;  /* was field_0x2BA: daSss_Create 0248F140: param switch (0xFF -> 0); hand_move 0248F484 (SSS_WAIT_e) opens the hand when that switch is on instead of by distance */
    /* 0x3D7 */ u8 _3D7;
    /* 0x3D8 */ be<s16> mFrameCounter;  /* was field_0x2BC: daSss_Create 0248F140: rnd(10000); daSss_Execute 0248EEEC adds 1 per frame; hand_move 0248F484 / control1 / cut_control1 (in hand_move) use it as the sway phase and its low bits for the magic drain and the sphere points */
    /* 0x3DA */ u8 _3DA[2];
    /* 0x3DC */ be<s16> mAction;  /* was field_0x2C0: hand_move 0248F484: switch over SSS_WAIT_e / SSS_OPEN_e / SSS_REACH_e / SSS_CATCH_e / SSS_CUT_e / SSS_SINK_e */
    /* 0x3DE */ be<s16> mTimers[3];  /* was field_0x2C2: daSss_Execute 0248EEEC counts the 3 down; indexed by SSS_ACTION_TIMER_e / SSS_EMITTER_TIMER_e / SSS_HIT_TIMER_e in hand_move 0248F484 and cut_control1 (in hand_move) */
    /* 0x3E4 */ cXyz mHandPos;  /* was field_0x2C8: hand_move 0248F484: the hand position, eased toward the player / the open point and falling after the cut; control2 (in hand_move) makes it the last vine point */
    /* 0x3F0 */ cXyz mHandMtxPos;  /* was field_0x2D4: control2 / cut_control2 (in hand_move 0248F484) copy the last vine point; hand_mtx_set 0248EDD0 places the hand model there */
    /* 0x3FC */ be<s16> mHandAngleX;  /* was field_0x2E0: control2 / cut_control2 (in hand_move 0248F484): -atan2 of the last vine segment; hand_mtx_set 0248EDD0 rotates the hand model x by it */
    /* 0x3FE */ be<s16> mHandAngleY;  /* was field_0x2E2: control2 / cut_control2 (in hand_move 0248F484): atan2 of the last vine segment; hand_mtx_set 0248EDD0 rotates the hand model y by it */
    /* 0x400 */ u8 _400[4];
    /* 0x404 */ be<f32> mSegmentLen;  /* was field_0x2E8: hand_move 0248F484 eases it to 0 / 25 / 30 by action; control1 / control2 / cut_control2 (in hand_move) use it as the length of each vine segment */
    /* 0x408 */ u8 _408[4];
    /* 0x40C */ be<f32> mReachPull;  /* was field_0x2F0: hand_move 0248F484 eases it to 10 in SSS_REACH_e, else 0; control1 (in hand_move) adds it along the vine direction to every segment */
    /* 0x410 */ be<f32> mSwayAmp;  /* was field_0x2F4: hand_move 0248F484: 10 (catch) / 50 (cut), eased to 5 / 50 / 0; control1 / cut_control2 (in hand_move) scale the segment sway by it */
    /* 0x414 */ be<f32> mCutGroundY;  /* was field_0x2F8: hand_move 0248F484 (SSS_CUT_e): ground height under the falling hand; cut_control2 (in hand_move) keeps the vine points above it + 5 */
    /* 0x418 */ be<f32> mCutSegmentLen;  /* was field_0x2FC: hand_move 0248F484 sets 1.5 * the 5th segment length at the cut and eases it to 0; cut_control1 (in hand_move) uses it as the segment length of the cut-off piece; daSss_Draw 0248EC3C draws that piece while it is > 0.1 */
    /* 0x41C */ SssLineMaterial mVineLine;  /* was field_0x300: useHeapInit 0248EFEC: init(1, 10, BTI); hand_move 0248F484 copies the 10 vine points into it; daSss_Draw 0248EC3C updates and draws it (l_color) */
    /* 0x5A4 */ sss_s mVine[10];  /* was field_0x33C: control1 / control2 (in hand_move 0248F484): the 10 vine points from the root (current.pos) to the hand; size_d[] widths; copied into mVineLine */
    /* 0x6BC */ SssLineMaterial mCutLine;  /* was field_0x454: useHeapInit 0248EFEC: init(1, 5, BTI); hand_move 0248F484 copies the 5 cut-piece points into it; daSss_Draw 0248EC3C draws it (l_cutColor) while mCutSegmentLen > 0.1 */
    /* 0x844 */ sss_s mCutVine[5];  /* was field_0x490: hand_move 0248F484 copies the first 5 vine points at the cut; cut_control1 (in hand_move) moves them and sets the TSURU_TAIEKI00 particle at the end */
    /* 0x8D0 */ dCcD_Stts mStts;
    /* 0x90C */ dCcD_Sph mVineSph[3];  /* was field_0x558: daSss_Create 0248F140: Set(tg_sph_src) x3; hand_move 0248F484 places them on vine points (by mFrameCounter) and cuts the vine on a Tg hit */
    /* 0xC90 */ dCcD_Sph mHandSph;  /* was field_0x8DC: daSss_Create 0248F140: Set(bm_sph_src); hand_move 0248F484 centres it on eyePos; a Tg hit cuts the vine, attack type 8 pushes the hand away instead */
    /* 0xDBC */ be<f32> mPushDist;  /* was field_0xA08: hand_move 0248F484 sets 300 when mHandSph is hit by attack type 8 and eases it to 0; the hand moves to 100 up / it away from the root while it is > 1 */
    /* 0xDC0 */ be<s16> mPushAngleY;  /* was field_0xA0C: hand_move 0248F484: player angle + 0x8000 at that hit; direction of the push (mPushDist) */
    /* 0xDC2 */ u8 _DC2[2];
    /* 0xDC4 */ gptr<SssEmitter> mpEmitter1;
    /* 0xDC8 */ gptr<SssEmitter> mpEmitter2;
    /* 0xDCC */ be<u8> mCutEffectOn;  /* was field_0xA18: hand_move 0248F484 sets 1 at the cut; cut_control1 / cut_control2 (in hand_move) start the TSURU_TAIEKI00 emitters only while it is set; cut_control2 clears it when its emitter ends */
    /* 0xDCD */ u8 _DCD[3];
};
WWHD_OFFSET(sss_class, mVine, 0x5A4);
WWHD_OFFSET(sss_class, mCutVine, 0x844);
WWHD_OFFSET(sss_class, mVineSph, 0x90C);
WWHD_OFFSET(sss_class, mpEmitter1, 0xDC4);
WWHD_OFFSET(sss_class, mPhs, 0x3C8);
WWHD_OFFSET(sss_class, mpMorf, 0x3D0);
WWHD_OFFSET(sss_class, mType, 0x3D4);
WWHD_OFFSET(sss_class, mOpenRangePrm, 0x3D5);
WWHD_OFFSET(sss_class, mSwitchNo, 0x3D6);
WWHD_OFFSET(sss_class, mFrameCounter, 0x3D8);
WWHD_OFFSET(sss_class, mAction, 0x3DC);
WWHD_OFFSET(sss_class, mTimers, 0x3DE);
WWHD_OFFSET(sss_class, mHandPos, 0x3E4);
WWHD_OFFSET(sss_class, mHandMtxPos, 0x3F0);
WWHD_OFFSET(sss_class, mHandAngleX, 0x3FC);
WWHD_OFFSET(sss_class, mHandAngleY, 0x3FE);
WWHD_OFFSET(sss_class, mSegmentLen, 0x404);
WWHD_OFFSET(sss_class, mReachPull, 0x40C);
WWHD_OFFSET(sss_class, mSwayAmp, 0x410);
WWHD_OFFSET(sss_class, mCutGroundY, 0x414);
WWHD_OFFSET(sss_class, mCutSegmentLen, 0x418);
WWHD_OFFSET(sss_class, mVineLine, 0x41C);
WWHD_OFFSET(sss_class, mCutLine, 0x6BC);
WWHD_OFFSET(sss_class, mStts, 0x8D0);
WWHD_OFFSET(sss_class, mHandSph, 0xC90);
WWHD_OFFSET(sss_class, mPushDist, 0xDBC);
WWHD_OFFSET(sss_class, mPushAngleY, 0xDC0);
WWHD_OFFSET(sss_class, mpEmitter2, 0xDC8);
WWHD_OFFSET(sss_class, mCutEffectOn, 0xDCC);
WWHD_SIZE(sss_class, 0xDD0);

// HD: attack work area written by at_power_check; exact guest stack size.
struct SssAtInfo {
    /* 0x00 */ gptr<void> mpObj;
    /* 0x04 */ u8 mAttackData[6];
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 mHitData[9];
    /* 0x14 */ gptr<cXyz> pParticlePos;
    /* 0x18 */ u8 mTail[4];
};
WWHD_OFFSET(SssAtInfo, mResultingAttackType, 0xA);
WWHD_OFFSET(SssAtInfo, pParticlePos, 0x14);
WWHD_SIZE(SssAtInfo, 0x1C);

struct SssSphereSource {
    be<u32> mFlags;
    be<u32> mAtType;
    u8 mAtp;
    u8 mAtPadding0;
    u8 mAtPadding1;
    u8 mAtPadding2;
    be<u32> mAtSPrm;
    be<u32> mTgType;
    be<u32> mTgSPrm;
    be<u32> mCoSPrm;
    u8 mGAtSe;
    u8 mGAtHitMark;
    u8 mGAtSpl;
    u8 mGAtMtrl;
    be<u32> mGAtSPrm;
    u8 mGTgSe;
    u8 mGTgHitMark;
    u8 mGTgSpl;
    u8 mGTgMtrl;
    be<u32> mGTgSPrm;
    be<u32> mGCoSPrm;
    be<f32> mCenterX;
    be<f32> mCenterY;
    be<f32> mCenterZ;
    be<f32> mRadius;
};
WWHD_SIZE(SssSphereSource, 0x40);

struct SssPlayerVTable {
    /* 0x00 */ u8 mEntries[0xEC];
    /* 0xEC */ be<u32> setOutPower;
};
WWHD_OFFSET(SssPlayerVTable, setOutPower, 0xEC);
WWHD_SIZE(SssPlayerVTable, 0xF0);
static inline void sss_setOutPower(daPy_py_c* player, SssPlayerVTable* vtable, f32 power, s16 angle,
                                   s32 mode) {
    gabi::call_ptr(vtable->setOutPower, player, power, angle, mode);
}

// HD: this byte lies in the unexpanded tail of the shared play layout.
struct SssGameMagicFlags {
    /* 0x0000 */ u8 mGameInfo[0x5BEF];
    /* 0x5BEF */ be<u8> mMagicDrainFlags;
};
WWHD_OFFSET(SssGameMagicFlags, mMagicDrainFlags, 0x5BEF);
WWHD_SIZE(SssGameMagicFlags, 0x5BF0);

namespace sss {
enum { daPyFace_TIYAYA = 0xA3 };
}

constexpr u32 SSS_SPHERE_CTOR = 0x025166F0;
constexpr u32 SSS_SPHERE_DTOR = 0x02515AE8;
static inline void sss_sphere_ct(dCcD_Sph* sphere) { gabi::call(SSS_SPHERE_CTOR, sphere); }
static inline void sss_sphere_dt(dCcD_Sph* sphere, s32 flags) {
    gabi::call(SSS_SPHERE_DTOR, sphere, flags);
}
static inline void sss_constructSpheres(dCcD_Sph* spheres, u32 count) {
    gabi::call(0x028EFFD0, spheres, count, u32(sizeof(dCcD_Sph)), SSS_SPHERE_CTOR);
}
static inline void sss_destroySpheres(dCcD_Sph* spheres, u32 count) {
    gabi::call(0x028F0164, spheres, count, u32(sizeof(dCcD_Sph)), SSS_SPHERE_DTOR, 0, 0);
}
static inline void sss_line_ct(SssLineMaterial* line) { gabi::call(0x025EB82C, line); }
static inline void sss_line_dt(SssLineMaterial* line, s32 flags) {
    gabi::call(0x025EB8B8, line, flags);
}
static inline void sss_actor_dt(fopAc_ac_c* actor, s32 flags) {
    gabi::call(0x025D50BC, actor, flags);
}
static inline void sss_bgCheck_dt(cBgS_Chk* check, s32 flags) {
    gabi::call(0x02008DAC, check, flags);
}

// Dexivine's hand state and timer roles; the unknown member names remain those in tww.
enum SssAction {
    SSS_WAIT_e = 0,
    SSS_OPEN_e = 1,
    SSS_REACH_e = 2,
    SSS_CATCH_e = 3,
    SSS_CUT_e = 5,
    SSS_SINK_e = 6,
};
enum SssTimer {
    SSS_ACTION_TIMER_e = 0,
    SSS_EMITTER_TIMER_e = 1,
    SSS_HIT_TIMER_e = 2,
};
enum SssKind {
    SSS_WATER_e = 1,
    SSS_LAND_e = 0x23,
};
namespace sss {
enum { ID_IT_SN_TSURU_TAIEKI00 = 0x8184 };
}
