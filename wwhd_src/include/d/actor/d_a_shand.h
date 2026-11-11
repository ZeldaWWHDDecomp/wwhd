/* Forbidden Woods ceiling tentacle.
 * HD: the enemy base shifts actor members by +0x11C; the expanded line material shifts
 * collision members by a further +0x14C. Unknown field names follow zeldaret/tww. */
#pragma once
#include "d/d_cc_uty.h"
#include "gc_api/gc_api.h"

// HD: this actor uses the textured line update overload and byte-sized segment widths.
struct ShandLineData {
    gptr<cXyz> mpPos;
    gptr<be<u8>> mpSize;
    u8 _08[0x18 - 8];
};
WWHD_SIZE(ShandLineData, 0x18);
struct ShandLineMat : mDoExt_3DlineMat1_c {
    ShandLineData* getLines() { return gabi::at<ShandLineData>(mpLines); }
    cXyz* getPos(int i) { return getLines()[i].mpPos; }
    be<u8>* getSize(int i) { return getLines()[i].mpSize; }
    void update(u16 count, GXColor* color, dKy_tevstr_c* tev) {
        gabi::call(0x025ED1BC, this, count, color, tev);
    }
};
WWHD_SIZE(ShandLineMat, 0x188);
struct ShandSortPacket {
    u8 _00[0x9C];
    void setMat(ShandLineMat* material) { gabi::call(0x025EDD04, this, material); }
};
WWHD_SIZE(ShandSortPacket, 0x9C);
struct ShandRenderContext {
    u8 _00[0x5FB4];
    ShandSortPacket mSortPackets[1];
};
WWHD_OFFSET(ShandRenderContext, mSortPackets, 0x5FB4);
// HD: the attention update loads all three words before writing either destination.
struct ShandVectorWords {
    be<u32> x, y, z;
};
WWHD_SIZE(ShandVectorWords, 12);
inline void shandSetEyePosition(fopAc_ac_c* actor, const cXyz* position) {
    auto* src = reinterpret_cast<const ShandVectorWords*>(position);
    u32 x = src->x, y = src->y, z = src->z;
    auto* eye = reinterpret_cast<ShandVectorWords*>(&actor->eyePos);
    auto* attention = reinterpret_cast<ShandVectorWords*>(&actor->attention_info.position);
    eye->x = x;
    eye->y = y;
    eye->z = z;
    attention->x = x;
    attention->y = y;
    attention->z = z;
}

// HD: private bindings retain the verified widths and array ABI.
/* 02518DB0 at_power_check(CcAtInfo*) -> attacking actor; 02518CC8 def_se_set(actor, cCcD_Obj*,
 * mtrl) */
static inline fopAc_ac_c* at_power_check(CcAtInfo* info) {
    return gabi::call<fopAc_ac_c*>(0x02518DB0, info);
}
static inline void def_se_set(fopAc_ac_c* a, dCcD_GObjInf* obj, u32 mtrl) {
    gabi::call(0x02518CC8, a, obj, mtrl);
}
static inline void shandConstructArray(void* p, u32 n, u32 size, u32 ctor) {
    gabi::call(0x028EFFD0, p, n, size, ctor);
}
static inline void shandDestroyArray(void* p, u32 n, u32 size, u32 dtor, u32 flags) {
    gabi::call(0x028F0164, p, n, size, dtor, flags);
}
/* 02008DAC cBgS_Chk::~cBgS_Chk (ground checks), 02008B4C cBgS_LinChk::~cBgS_LinChk */
static inline void cBgS_GndChk_dt(void* c, s32 f) { gabi::call(0x02008DAC, c, f); }
static inline void shandLineCheckDestroyBase(void* c, s32 f) { gabi::call(0x02008B4C, c, f); }

struct shand_s {
    /* 0x00 */ cXyz mPos;
    /* 0x0C */ u8 _pad[0x18 - 0xC];
    /* 0x18 */ be<f32> mThickness;  /* was field_18: set_thickness (in daShand_Execute 0246FB78) sets it to shand_class mThickness for links 0..11 and thinner toward the tip for 12..19; hand_move (in daShand_Execute 0246FB78) writes it (ftoi) as the link's line width into mLineMat. tww d217e2b src/d/actor/d_a_shand.cpp keeps the offset name field_18 (same uses, lines 113 / 116 / 285) */
};
WWHD_SIZE(shand_s, 0x1C);

// HD: inherits the enemy base; tww names the actor base directly.
struct shand_class : fopEn_enemy_c {
    enum : u32 { __vt = 0x1003A0F4 }; // HD: virtual destructor.
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ be<s16> mExecuteCount;
    /* 0x3D2 */ be<s16> unused_2B6;
    /* 0x3D4 */ be<s16> mState;
    /* 0x3D6 */ be<s16> mTipMode;  /* was field_2BA: normal (in daShand_Execute 0246FB78): 0 while the tip (mLinks[19]) moves to mTipTargetPos, 1 once it is within 10 (then it tells the parent *mpParentState = 2) and keeps the target at mTipGroundPos; daShand_Create 02470E24 sets 1, cut_control (regrow) and hand_move (parent dead / cut) set 0 */
    /* 0x3D8 */ be<s16> mTimers[4];  /* was field_2BC: daShand_Execute 0246FB78 counts all 4 down; hand_move (in daShand_Execute) sets [0] (regrow wait, l_HIO) and [1] (no new cut) after a cut and checks them */
    /* 0x3E0 */ be<s16> mNoHitTimer;  /* was field_2C4: hand_move (in daShand_Execute 0246FB78) checks the mCylArr / mSph TG hits only while it is 0 and sets 10 after a hit; daShand_Execute counts it down; daShand_Create 02470E24 sets 30 */
    /* 0x3E2 */ u8 _3E2[2];
    /* 0x3E4 */ cXyz mTipGroundPos;  /* was field_2C8: normal (in daShand_Execute 0246FB78): the ground point under the tip found by the line check while mGroundSetTimer runs; the tip goes back there (mTipTargetPos = it) and its height below home sets mLinkLen */
    /* 0x3F0 */ cXyz mTipTargetPos;  /* was field_2D4: control1 (in daShand_Execute 0246FB78) moves the tip link (mLinks[19]) toward it with step mTipSpeedRate * 50; normal sets it to mTipGroundPos */
    /* 0x3FC */ u8 unused_2E0[0x40C - 0x3FC];
    /* 0x40C */ be<f32> mTipSpeedRate;  /* was field_2F0: control1 (in daShand_Execute 0246FB78): rate of the tip's move to mTipTargetPos (step * 50), eased up to 1; daShand_Create 02470E24 sets 1, cut_control resets it to 0 when the hand regrows */
    /* 0x410 */ be<f32> mLinkLen;  /* was field_2F4: control1 / control2 (in daShand_Execute 0246FB78): z of the per-link offset (the link length); normal eases it to the tip's depth below home * 0.05, cut_control to 20 */
    /* 0x414 */ be<f32> mBendY;  /* was field_2F8: control1 / control2 (in daShand_Execute 0246FB78): y of the per-link bend offset, eased to a REG value (0 / -5); a cut sets 3 */
    /* 0x418 */ be<f32> mBendZ;  /* was field_2FC: control1 / control2 (in daShand_Execute 0246FB78): z of the per-link bend offset, eased to a REG value (0 / 20); a cut sets 40 */
    /* 0x41C */ be<f32> mWobbleAmp;  /* was field_300: control1 / control2 (in daShand_Execute 0246FB78): amplitude of the sine / cosine wobble of the links (phase mExecuteCount); normal eases it to 10, control2 to 0, a cut sets 30..50 */
    /* 0x420 */ be<f32> mThickness;  /* was field_304: set_thickness (in daShand_Execute 0246FB78): base thickness of each link (shand_s mThickness), thinner toward the end; daShand_Create 02470E24 sets 15.75 / 10.5 by type */
    /* 0x424 */ be<u32> mParentId;  /* was field_308: the parent Kokiie's process id: himo_create (in Kokiie 021A6418) sets it; hand_move (in daShand_Execute 0246FB78) looks the parent up by it (mpParent) */
    /* 0x428 */ gptr<fopAc_ac_c> mpParent;  /* was field_30C: hand_move (in daShand_Execute 0246FB78): the parent actor found by mParentId; the hand follows its position / shape angle and is cut when its health is 0 */
    /* 0x42C */ gptr<cXyz> mpRootPos;  /* was field_310: himo_create (in Kokiie 021A6418) points it at the parent's anchor position (kokiie mRopeEndPos[i]); hand_move (in daShand_Execute 0246FB78) puts the hand's root there */
    /* 0x430 */ gptr<be<u8>> mpParentState;  /* was field_314: himo_create (in Kokiie 021A6418) points it at the parent's per-hand state byte (kokiie mRopeState[i]); the hand writes 1 when it is cut and 2 when its tip is back in place */
    /* 0x434 */ be<s16> mGroundSetTimer;  /* was field_318: daShand_Create 02470E24 sets 10; normal (in daShand_Execute 0246FB78) line-checks the ground under the tip and sets mTipGroundPos / mTipTargetPos / the tip position each frame while it counts down */
    /* 0x436 */ u8 _436[2];
    /* 0x438 */ shand_s mLinks[20];  /* was field_31C: the 20 shand_s links of the hand (position, thickness mThickness): control1 / control2 / set_thickness (in daShand_Execute 0246FB78) move them, [0] is put at the root, [19] is the tip */
    /* 0x668 */ ShandLineMat mLineMat;
    /* 0x7F0 */ dCcD_Stts mStts;
    /* 0x82C */ dCcD_Sph mSph;
    /* 0x958 */ dCcD_Cyl mCylArr[5];
    /* 0xF48 */ be<f32> ground_y;
    /* 0xF4C */ be<u8> mHasHIO;
};
WWHD_OFFSET(shand_class, mTipSpeedRate, 0x40C);
WWHD_OFFSET(shand_class, mLinks, 0x438);
WWHD_OFFSET(shand_class, mStts, 0x7F0);
WWHD_OFFSET(shand_class, mCylArr, 0x958);
WWHD_OFFSET(shand_class, mHasHIO, 0xF4C);

/* HD: the vtable pointer follows the members (+8); size 0xC */
struct daShand_HIO_c {
    enum : u32 { __vt = 0x1003A0E4 };
    /* 0x0 */ be<s8> mNo;
    /* 0x1 */ u8 _1;
    /* 0x2 */ be<s16> mEyeSegment;  /* was field_6: daShand_HIO_c_ct sets -3; hand_move 0246FAEC: eyePos is the hand line segment it + 10 (tww the same) */
    /* 0x4 */ be<s16> mHitHoldTime;  /* was field_8: daShand_HIO_c_ct sets 50; hand_move 0246FAEC: mTimers[0] after the hand hits while it has a parent, mTimers[1] it + 90 (tww field_2BC[0] the same) */
    /* 0x6 */ u8 _6[2];
    /* 0x8 */ be<u32> __vtbl;
};
WWHD_SIZE(daShand_HIO_c, 0xC);
#define l_HIO (*gabi::at<daShand_HIO_c>(0x1046DCB8)) // HD: BSS, initialised by the TU constructor.

// HD: out-of-line construction/destruction and external array destructor callback.
inline void shandLineConstruct(ShandLineMat* material) { gabi::call(0x025EB82C, material); }
inline void shandLineDestroy(ShandLineMat* material) { gabi::call(0x025EB8B8, material, 2); }
inline void shandSphereConstruct(dCcD_Sph* sphere) { gabi::call(0x025166F0, sphere); }
inline void shandSphereDestroy(dCcD_Sph* sphere) { gabi::call(0x02515AE8, sphere, 2); }
static constexpr u32 shandCylinderAabVtable = 0x10039FB4;
static constexpr u32 shandCylinderDestructor = 0x02515A70;
WWHD_SIZE(shand_class, 0xF4D);

static constexpr u32 shandGroundGroupVtable = 0x10039FD4;
static constexpr u32 shandLinePolyVtable = 0x10039FC4;
static dCcD_Cyl* dCcD_Cyl_ct_tu(dCcD_Cyl*);

// HD: flat source views spell out tww collision fields for checked static initialisers.
struct ShandSphereSource {
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
WWHD_OFFSET(ShandSphereSource, mCenterX, 0x30);
WWHD_SIZE(ShandSphereSource, 0x40);
struct ShandCylinderSource {
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
    be<f32> mHeight;
};
WWHD_OFFSET(ShandCylinderSource, mCenterX, 0x30);
WWHD_SIZE(ShandCylinderSource, 0x44);
