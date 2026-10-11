/* d_bg_s: background collision system (dBgS), WWHD.
 *
 * Translation unit 024EE58C..024EFD08. Ported from the GameCube d_bg_s.cpp; the GameCube
 * JUT_ASSERTs are still present in HD (JUTAssertion 0273AA24 with per-instance strings), also
 * those of the inlined dBgW accessors (GetPolyInfId and friends). */
#include "bindings.h"

void BgAPI_CheckConstruct(void*);
void BgAPI_SphereConstruct(void*);
namespace BgSystemHD {
struct dBgS;
struct dBgW;
struct cBgS_PolyInfo;
} // namespace BgSystemHD
void BgAPI_cBgS_Ct(BgSystemHD::dBgS* s);
void BgAPI_cBgS_Dt(BgSystemHD::dBgS* s);
void BgAPI_cBgS_Move(BgSystemHD::dBgS* s);
u32 BgAPI_cBgS_Regist(BgSystemHD::dBgS* s, BgSystemHD::dBgW* w, u32 id, fopAc_ac_c* ac);
BgSystemHD::dBgW* BgAPI_cBgS_GetBgWPointer(u32 bgs, BgSystemHD::cBgS_PolyInfo* p);
s32 BgAPI_cBgS_GetTriGrp(BgSystemHD::dBgS* s, u32 bg, u32 poly);
u32 BgAPI_cBgS_GetGrpInf(BgSystemHD::dBgS* s, BgSystemHD::cBgS_PolyInfo* p, s32 grp);
u32 BgAPI_cBgS_GetGrpToRoomId(BgSystemHD::dBgS* s, u32 bg, s32 grp);
BOOL BgAPI_cBgS_ChkPolySafe(BgSystemHD::dBgS* s, BgSystemHD::cBgS_PolyInfo* p);
BOOL BgAPI_cBgS_Chk_ChkSameActorPid(u32 chk, u32 pid);
BOOL BgAPI_cBgW_LineCheckGrpRp(BgSystemHD::dBgW* w, u32 chk, s32 grp, s32 depth);
void BgAPI_dBgW_WallCorrectGrpRp(BgSystemHD::dBgW* w, u32 acch, s32 grp, s32 depth);
BOOL BgAPI_dBgW_RoofChkGrpRp(BgSystemHD::dBgW* w, u32 chk, s32 grp, s32 depth);
BOOL BgAPI_dBgW_SplGrpChkGrpRp(BgSystemHD::dBgW* w, u32 chk, s32 grp, s32 depth);
BOOL BgAPI_dBgW_SphChkGrpRp(BgSystemHD::dBgW* w, u32 chk, void* user, s32 grp, s32 depth);
void BgAPI_dBgS_SplGrpChk_Init(u32 chk);
void BgAPI_dBgS_Acch_SetWallCir(u32 acch);
void BgAPI_dBgS_Acch_CalcWallBmdCyl(u32 acch);
void BgAPI_cM3dGLin_SetStartEnd(u32 lin, u32 start, u32 end);
BOOL BgAPI_PSMTXInverse(u32 src, u32 dst);
void BgAPI_PSMTXMultVec(u32 m, u32 src, u32 dst);
void BgAPI_PSVECAdd(u32 a, u32 b, u32 dst);
void BgAPI_cBgW_GetTrans(BgSystemHD::dBgW* w, u32 out);

namespace BgSystemHD {

// HD: full private verified views; shared collision declarations remain unchanged.

/* cBgS_PolyInfo (0x10): HD order poly index, bg index, bgw, actor id, vtable */
struct cBgS_PolyInfo {
    /* 0x0 */ be<u16> m_poly_index;
    /* 0x2 */ be<u16> m_bg_index;
    /* 0x4 */ be<u32> m_bgw;
    /* 0x8 */ be<u32> m_actor_id;
    /* 0xC */ be<u32> __vtbl;
};
WWHD_SIZE(cBgS_PolyInfo, 0x10);

struct Triangle {
    be<u16> mVertex[3];
    be<u16> mInfoIndex;
    be<u16> mGroupIndex;
};
WWHD_SIZE(Triangle, 0xA);
struct PolygonInfo {
    be<u32> mInfo[4];
};
WWHD_SIZE(PolygonInfo, 0x10);
struct AcchActorInfo {
    u8 mOtherChecks[0x74];
    be<s32> mBgIndex;
    be<u32> mpBgW;
    be<u32> mActorId;
};
WWHD_OFFSET(AcchActorInfo, mpBgW, 0x78);

struct ActorView {
    u8 mProcessHeader[4];
    be<u32> mProcessId;
    u8 mActorData[0x31E];
    be<s8> mRoomNo;
    u8 mPadding;
    csXyz shape_angle;
};
WWHD_OFFSET(ActorView, mRoomNo, 0x326);
WWHD_OFFSET(ActorView, shape_angle, 0x328);
struct Check {
    be<u32> mpPolyPassChk, mpGrpPassChk, mActorPid;
    be<u8> mSameActorChk;
    u8 mPadding[3];
    be<u32> mVtable;
};
WWHD_SIZE(Check, 0x14);
struct PolyPass {
    be<u32> mVtable;
    be<u8> mObjThrough, mCamThrough, mLinkThrough, mArrowThrough;
    be<u8> mBombThrough, mBoomerangThrough, mRopeThrough;
    u8 mPadding;
};
WWHD_SIZE(PolyPass, 0xC);
struct GroupPass {
    be<u32> mVtable, mGroup;
};
WWHD_SIZE(GroupPass, 8);
// HD: per-TU GHS vtables for the inline check constructors.
enum CheckVtable : u32 {
    kPolyInfoVtable = 0x10043244,
    kRoofPolyVtable = 0x100432B4,
    kRoofCheckVtable = 0x100432C4,
    kRoofGroupPassVtable = 0x100432D4,
    kRoofPolyPassVtable = 0x100432E4,
    kSphereVtable = 0x100432F4,
    kSpherePolyVtable = 0x10043304,
    kSphereCheckVtable = 0x10043314,
    kSphereGroupPassVtable = 0x10043324,
    kSpherePolyPassVtable = 0x10043334,
};
struct RoofCheck {
    cBgS_PolyInfo mPoly;
    Check mCheck;
    PolyPass mPolyPass;
    GroupPass mGroupPass;
    cXyz m_pos;
    be<u32> mpUser;
    be<f32> m_now_y;
};
WWHD_SIZE(RoofCheck, 0x4C);
WWHD_OFFSET(RoofCheck, mCheck, 0x10);
WWHD_OFFSET(RoofCheck, mPolyPass, 0x24);
WWHD_OFFSET(RoofCheck, mGroupPass, 0x30);
WWHD_OFFSET(RoofCheck, m_pos, 0x38);
struct Sphere {
    cXyz mCenter;
    be<f32> mRadius;
    be<u32> mVtable;
};
WWHD_SIZE(Sphere, 0x14);
struct SphereCheck {
    Sphere mSphere;
    cBgS_PolyInfo mPoly;
    Check mCheck;
    PolyPass mPolyPass;
    GroupPass mGroupPass;
    be<u32> mpCallback;
};
WWHD_SIZE(SphereCheck, 0x50);
WWHD_OFFSET(SphereCheck, mPoly, 0x14);
WWHD_OFFSET(SphereCheck, mCheck, 0x24);
WWHD_OFFSET(SphereCheck, mPolyPass, 0x38);
WWHD_OFFSET(SphereCheck, mGroupPass, 0x44);
struct LineCheck {
    Check mCheck;
    cBgS_PolyInfo mPoly;
    cM3dGLin mLin;
    cXyz mCross;
    be<u32> mFlag;
    be<u8> mPreWallChk, mPreGroundChk, mPreRoofChk;
    u8 mRemainingFlags[5];
};
WWHD_SIZE(LineCheck, 0x58);
WWHD_OFFSET(LineCheck, mFlag, 0x4C);
WWHD_OFFSET(LineCheck, mPreWallChk, 0x50);
struct SpecialGroupCheck {
    cBgS_PolyInfo mPoly;
    Check mCheck;
    u8 mGroupAndPosition[0x24];
    be<f32> mHeight;
    be<u32> mFlag;
};
WWHD_SIZE(SpecialGroupCheck, 0x50);
WWHD_OFFSET(SpecialGroupCheck, mFlag, 0x4C);
struct StageVtable {
    u8 mOtherMethods[0x15C];
    be<u32> getStagInfo;
};
WWHD_OFFSET(StageVtable, getStagInfo, 0x15C);
struct StageData {
    be<u32> mVtable;
};
struct StageInfo {
    u8 mOtherInfo[0xC];
    be<u32> mType;
};
struct AttributeConversion {
    be<s32> mCode[0x20];
};
static AttributeConversion atr_conv;

struct BgVtable {
    u8 mOtherMethods[0x44];
    be<u32> mCrrPos;
    u8 mPadding48[4];
    be<u32> mTransPos;
    u8 mPadding50[4];
    be<u32> mMatrixCrrPos;
};
WWHD_OFFSET(BgVtable, mCrrPos, 0x44);
WWHD_OFFSET(BgVtable, mTransPos, 0x4C);
WWHD_OFFSET(BgVtable, mMatrixCrrPos, 0x54);

/* cBgD_t: the collision data of a dBgW */
struct cBgD_t {
    /* 0x00 */ u8 _00[8];
    /* 0x08 */ be<s32> m_t_num; /* triangles */
    /* 0x0C */ be<u32> m_t_tbl; /* stride 0xA, +6 u16 info index */
    /* 0x10 */ u8 _10[0x18];
    /* 0x28 */ be<s32> m_ti_num; /* infos */
    /* 0x2C */ be<u32> m_ti_tbl; /* stride 0x10: info0..info3 */
};
WWHD_OFFSET(cBgD_t, m_ti_tbl, 0x2C);

/* dBgW (cBgW base): only the members used here */
struct dBgW {
    /* 0x00 */ be<u32> m_id;    /* registration id; ChkUsed: < 0x100 */
    /* 0x04 */ be<u32> __vtbl;  /* CrrPos 0x44, TransPos 0x4C, MatrixCrrPos 0x54 */
    /* 0x08 */ be<u32> pm_base; /* base matrix pointer */
    /* 0x0C */ Mtx34 mOldMtx;
    /* 0x3C */ u8 _3C[0x30];
    /* 0x6C */ be<u8> m_flags; /* 1: move bg, 0x80: lock */
    /* 0x6D */ u8 _6D[8];
    /* 0x75 */ be<u8> m_priority;
    /* 0x76 */ u8 _76[0x1A];
    /* 0x90 */ be<u32> pm_vtx_tbl;
    /* 0x94 */ gptr<cBgD_t> pm_bgd;
    /* 0x98 */ u8 _98[0xC];
    /* 0xA4 */ be<s32> m_rootGrpIdx;
    /* 0xA8 */ u8 _A8[4];
    /* 0xAC */ be<s16> m_old_shape_angle_y;
    /* 0xAE */ be<s16> m_diff_shape_angle_y;
    /* 0xB0 */ be<u32> m_ride_callback;
    /* 0xB4 */ be<u32> m_push_pull_callback;
    /* 0xB8 */ be<u16> m_room_id;
    /* 0xBA */ be<u8> m_move_flag; /* bit 0 */
    /* 0xBB */ be<u8> m_grp_room_inf;
};
WWHD_OFFSET(dBgW, m_flags, 0x6C);
WWHD_OFFSET(dBgW, m_priority, 0x75);
WWHD_OFFSET(dBgW, pm_vtx_tbl, 0x90);
WWHD_OFFSET(dBgW, m_rootGrpIdx, 0xA4);
WWHD_OFFSET(dBgW, m_old_shape_angle_y, 0xAC);
WWHD_OFFSET(dBgW, m_grp_room_inf, 0xBB);

/* cBgS_ChkElm (0x14) */
struct cBgS_ChkElm {
    /* 0x00 */ gptr<dBgW> m_bgw_base_ptr;
    /* 0x04 */ be<u32> m_used; /* ChkUsed: bit 0 */
    /* 0x08 */ be<u32> m_actor_id;
    /* 0x0C */ be<u32> m_actor_ptr;
    /* 0x10 */ be<u32> __vtbl;
    bool ChkUsed() { return (m_used & 1) != 0; }
};
WWHD_SIZE(cBgS_ChkElm, 0x14);

struct dBgS {
    void Ct();
    void Dt();
    void ClrMoveFlag();
    void Move();
    u32 Regist(dBgW* bgw, fopAc_ac_c* ac);
    bool ChkMoveBG(cBgS_PolyInfo* polyInfo);
    bool ChkMoveBG_NoDABg(cBgS_PolyInfo* polyInfo);
    s32 GetPolyId0(s32 bg_index, s32 poly_index, s32 defv, u32 mask, u32 shift);
    s32 GetPolyCamId(s32 bg_index, s32 poly_index);
    u32 GetMtrlSndId(cBgS_PolyInfo* p);
    s32 GetExitId(cBgS_PolyInfo* p);
    s32 GetGrpRoomInfId(cBgS_PolyInfo* p);
    s32 GetGrpSoundId(cBgS_PolyInfo* p);
    u32 ChkGrpInf(cBgS_PolyInfo* p, u32 mask);
    s32 GetPolyColor(cBgS_PolyInfo* p);
    s32 GetPolyId1(s32 bg_index, s32 poly_index, s32 defv, u32 mask, u32 shift);
    s32 GetLinkNo(cBgS_PolyInfo* p);
    s32 GetWallCode(cBgS_PolyInfo* p);
    s32 GetSpecialCode(cBgS_PolyInfo* p);
    s32 GetGroundCode(cBgS_PolyInfo* p);
    s32 GetAttributeCodeDirect(cBgS_PolyInfo* p);
    s32 GetAttributeCode(cBgS_PolyInfo* p);
    s32 GetRoomId(cBgS_PolyInfo* p);
    s32 GetPolyId2(s32 bg_index, s32 poly_index, s32 defv, u32 mask, u32 shift);
    s32 GetCamMoveBG(cBgS_PolyInfo* p);
    s32 GetRoomCamId(cBgS_PolyInfo* p);
    s32 GetRoomPathId(cBgS_PolyInfo* p);
    s32 GetRoomPathPntNo(cBgS_PolyInfo* p);
    u32 ChkPolyHSStick(cBgS_PolyInfo* p);
    bool LineCrossNonMoveBG(u8* chk);
    void WallCorrect(dBgS_Acch* acch);
    f32 RoofChk(u8* chk);
    bool SplGrpChk(u8* chk);
    bool SphChk(u8* chk, void* user);
    void MoveBgCrrPos(cBgS_PolyInfo* polyInfo, u32 accept, cXyz* pos, csXyz* angle,
                      csXyz* shape_angle);
    void MoveBgTransPos(cBgS_PolyInfo* polyInfo, u32 accept, cXyz* pos, csXyz* angle,
                        csXyz* shape_angle);
    void MoveBgMatrixCrrPos(cBgS_PolyInfo* polyInfo, u32 accept, cXyz* pos, csXyz* angle,
                            csXyz* shape_angle);
    void RideCallBack(cBgS_PolyInfo* polyInfo, fopAc_ac_c* ac);
    u32 PushPullCallBack(cBgS_PolyInfo* polyInfo, fopAc_ac_c* ac, u32 inf, u32 lbl);
    /* 0x0000 */ cBgS_ChkElm m_chk_element[256];
};

// Private helpers shared by the polygon queries below.

static inline dBgW* elm_bgw(dBgS* s, s32 i) { return s->m_chk_element[i].m_bgw_base_ptr.get(); }

/* dBgW::GetPolyInfId + GetPolyInfN (inline, with their asserts; strings per instance):
 * info word `word` (0..3) of the polygon's info entry */
struct PolyInfAsserts {
    const char* file;
    const char* msg_poly;
    u32 line_id;
    const char* msg_id;
};
static inline u32 dBgW_GetPolyInf(dBgW* bgw, s32 poly_index, u32 word, const PolyInfAsserts& a) {
    if (!(0 <= poly_index && poly_index < bgw->pm_bgd->m_t_num))
        JUT_ASSERT_fail(a.file, 0x2F1, a.msg_poly);
    s32 id = gabi::at<Triangle>(bgw->pm_bgd->m_t_tbl)[poly_index].mInfoIndex.get();
    if (!(id < bgw->pm_bgd->m_ti_num))
        JUT_ASSERT_fail(a.file, a.line_id, a.msg_id);
    return gabi::at<PolygonInfo>(bgw->pm_bgd->m_ti_tbl)[id].mInfo[word].get();
}

} // namespace BgSystemHD

using namespace BgSystemHD;

/* 024EE58C, 024EE620, 024EE634: NOTE: these three belong to the d_attention TU (its __sinit and
 * SafeString inline copies, referenced only from d_attention's SafeString vtable 100430F8); d_bg_s proper starts at
 * 024EE638. Verified here first; kept here so each address is verified once.
 * 024EE58C: static initialisers (header statics) */
/* 024EE58C */
void __sinit_d_bg_s_cpp() { sinit_header_statics(0x1046ECD4, 0x101D515C); }

/* 024EE620: deleting destructor of an empty class of this unit (no members to destroy) */
/* 024EE620 */
void dBgS_unit_empty_dt(void* p, s32 flags) {
    if (p == nullptr)
        return;
    if (!(flags & 1))
        return;
    operator_delete(p);
}

/* 024EE634: empty function */
/* 024EE634 */
void dBgS_unit_empty() {}

/* 024EE638 */
void dBgS_ChangeAttributeCode(u32 code, be<u32>* dst) { *dst = (*dst & 0xFFE0FFFF) | (code << 16); }

/* 024EE650 */
u32 dBgS_GetRoomPathPntNo(u32 param) { return param >> 24; }

/* 024EE658 */
void dBgS_MoveBGProc_Typical(BgSystemHD::dBgW* pbgw, void* user,
                             BgSystemHD::cBgS_PolyInfo* polyInfo, u32 accept, cXyz* pos,
                             csXyz* angle, csXyz* shape_angle) {
    gabi::Local<Mtx34> inv;
    if (BgAPI_PSMTXInverse(gabi::ea(&pbgw->mOldMtx), gabi::ea(inv.get()))) {
        gabi::Local<cXyz> local, newPos;
        BgAPI_PSMTXMultVec(gabi::ea(inv.get()), gabi::ea(pos), gabi::ea(local.get()));
        BgAPI_PSMTXMultVec(pbgw->pm_base, gabi::ea(local.get()), gabi::ea(newPos.get()));
        pos->copy(*newPos);
    }
}

/* 024EE6D8 */
void dBgS_MoveBGProc_RotY(BgSystemHD::dBgW* pbgw, void* user, BgSystemHD::cBgS_PolyInfo* polyInfo,
                          u32 accept, cXyz* pos, csXyz* angle, csXyz* shape_angle) {
    if (shape_angle == nullptr)
        return;
    s32 rot = pbgw->m_diff_shape_angle_y;
    shape_angle->y = (s16)(shape_angle->y + rot);
    if (angle != nullptr)
        angle->y = (s16)(angle->y + rot);
}

/* 024EE708 */
void dBgS_MoveBGProc_TypicalRotY(BgSystemHD::dBgW* pbgw, void* user,
                                 BgSystemHD::cBgS_PolyInfo* polyInfo, u32 accept, cXyz* pos,
                                 csXyz* angle, csXyz* shape_angle) {
    dBgS_MoveBGProc_Typical(pbgw, user, polyInfo, accept, pos, angle, shape_angle);
    dBgS_MoveBGProc_RotY(pbgw, user, polyInfo, accept, pos, angle, shape_angle);
}

/* 024EE76C */
void dBgS_MoveBGProc_Trans(BgSystemHD::dBgW* pbgw, void* user, BgSystemHD::cBgS_PolyInfo* polyInfo,
                           u32 accept, cXyz* pos, csXyz* angle, csXyz* shape_angle) {
    gabi::Local<cXyz> trans;
    BgAPI_cBgW_GetTrans(pbgw, gabi::ea(trans.get()));
    BgAPI_PSVECAdd(gabi::ea(pos), gabi::ea(trans.get()), gabi::ea(pos));
}

/* 024EE7AC: dBgS_RoofChk::dBgS_RoofChk (HD 0x4C; GameCube d_bg_s_roof_chk.cpp, emitted here) */
/* 024EE7AC */
void* dBgS_RoofChk_ct(void* p) {
    if (p == nullptr) {
        p = operator_new(0x4C);
        if (p == nullptr)
            return p;
    }
    u32 b = gabi::ea(p);
    auto* view = gabi::at<RoofCheck>(b);
    /* BgSystemHD::cBgS_PolyInfo */
    view->mPoly.m_bg_index = 0x100;
    view->mPoly.__vtbl = kPolyInfoVtable;
    view->mPoly.m_bgw = 0;
    view->mPoly.m_actor_id = 0xFFFFFFFF;
    view->mPoly.m_poly_index = 0xFFFF;
    BgAPI_CheckConstruct(&view->mCheck); /* cBgS_Chk::cBgS_Chk */
    view->mPolyPass.mBombThrough = 0;
    view->mCheck.mActorPid = 0xFFFFFFFF;
    view->m_pos.x = 0.0f;
    view->mPolyPass.mArrowThrough = 0;
    view->m_pos.z = 0.0f;
    view->m_pos.y = 0.0f;
    view->mCheck.mpPolyPassChk = gabi::ea(&view->mPolyPass);
    view->mCheck.mVtable = kRoofCheckVtable;
    view->mPolyPass.mCamThrough = 0;
    view->mPolyPass.mLinkThrough = 0;
    view->mCheck.mpGrpPassChk = gabi::ea(&view->mGroupPass);
    view->mGroupPass.mVtable = kRoofGroupPassVtable;
    view->m_now_y = 0.0f;
    view->mPolyPass.mVtable = kRoofPolyPassVtable;
    view->mPoly.__vtbl = kRoofPolyVtable;
    view->mPolyPass.mBoomerangThrough = 0;
    view->mGroupPass.mGroup = 1;
    view->mPolyPass.mObjThrough = 0;
    view->mPolyPass.mRopeThrough = 0;
    view->mpUser = 0;
    return p;
}

/* 024EE8B8: dBgS_SphChk::dBgS_SphChk (HD 0x50: cM3dGSph base, poly info +0x14, cBgS_Chk +0x24) */
/* 024EE8B8 */
void* dBgS_SphChk_ct(void* p) {
    if (p == nullptr) {
        p = operator_new(0x50);
        if (p == nullptr)
            return p;
    }
    u32 b = gabi::ea(p);
    auto* view = gabi::at<SphereCheck>(b);
    BgAPI_SphereConstruct(&view->mSphere); /* cM3dGSph::cM3dGSph */
    view->mPoly.m_bg_index = 0x100;
    view->mPoly.__vtbl = kPolyInfoVtable;
    view->mPoly.m_actor_id = 0xFFFFFFFF;
    view->mPoly.m_poly_index = 0xFFFF;
    view->mPoly.m_bgw = 0;
    BgAPI_CheckConstruct(&view->mCheck); /* cBgS_Chk::cBgS_Chk */
    view->mpCallback = 0;
    view->mPolyPass.mLinkThrough = 0;
    view->mPolyPass.mCamThrough = 0;
    view->mGroupPass.mVtable = kSphereGroupPassVtable;
    view->mCheck.mActorPid = 0xFFFFFFFF;
    view->mGroupPass.mGroup = 1;
    view->mSphere.mVtable = kSphereVtable;
    view->mCheck.mpGrpPassChk = gabi::ea(&view->mGroupPass);
    view->mCheck.mVtable = kSphereCheckVtable;
    view->mPolyPass.mArrowThrough = 0;
    view->mPoly.__vtbl = kSpherePolyVtable;
    view->mPolyPass.mVtable = kSpherePolyPassVtable;
    view->mPolyPass.mBombThrough = 0;
    view->mPolyPass.mRopeThrough = 0;
    view->mPolyPass.mBoomerangThrough = 0;
    /* Init(): ClearPi */
    view->mPoly.m_bg_index = 0x100;
    view->mPoly.m_poly_index = 0xFFFF;
    view->mPoly.m_bgw = 0;
    view->mPoly.m_actor_id = 0xFFFFFFFF;
    view->mPolyPass.mObjThrough = 0;
    view->mCheck.mpPolyPassChk = gabi::ea(&view->mPolyPass);
    return p;
}

/* 024EE9C0 */
void BgSystemHD::dBgS::Ct() { BgAPI_cBgS_Ct(this); }

/* 024EE9C4 */
void BgSystemHD::dBgS::Dt() { BgAPI_cBgS_Dt(this); }

/* 024EE9C8 */
void BgSystemHD::dBgS::ClrMoveFlag() {
    for (s32 i = 0; i < 256; i++) {
        if (m_chk_element[i].ChkUsed()) {
            BgSystemHD::dBgW* bgwp = elm_bgw(this, i);
            bgwp->m_move_flag = bgwp->m_move_flag & 0xFE; /* OffMoveFlag */
        }
    }
}

/* 024EEA00 */
void BgSystemHD::dBgS::Move() {
    BgAPI_cBgS_Move(this);
    for (s32 i = 0; i < 256; i++) {
        BgSystemHD::cBgS_ChkElm& e = m_chk_element[i];
        if (e.ChkUsed()) {
            BgSystemHD::dBgW* bgwp = e.m_bgw_base_ptr.get();
            s16 y = gabi::at<ActorView>(e.m_actor_ptr)->shape_angle.y.get(); /* shape_angle.y */
            s16 old = bgwp->m_old_shape_angle_y; /* CalcDiffShapeAngleY */
            bgwp->m_old_shape_angle_y = y;
            bgwp->m_diff_shape_angle_y = (s16)(y - old);
        }
    }
}

/* 024EEA6C */
u32 BgSystemHD::dBgS::Regist(BgSystemHD::dBgW* bgw, fopAc_ac_c* ac) {
    if (bgw == nullptr)
        return 1;
    u32 a = gabi::ea(ac);
    if (ac != nullptr && (bgw->m_flags & 1)) {
        bgw->m_old_shape_angle_y = gabi::at<ActorView>(a)->shape_angle.y.get();
        bgw->m_room_id =
            (u16)(s16)gabi::at<ActorView>(a)->mRoomNo.get(); /* SetRoomId(fopAcM_GetRoomNo(ac)) */
    }
    u32 id = ac != nullptr ? gabi::at<ActorView>(a)->mProcessId.get() : 0xFFFFFFFF;
    return BgAPI_cBgS_Regist(this, bgw, id, ac);
}

// HD: uses the global BgSystemHD::dBgS (play + 0x12A0), not `this`.
/* 024EEABC */
bool BgSystemHD::dBgS::ChkMoveBG(BgSystemHD::cBgS_PolyInfo* polyInfo) {
    BgSystemHD::dBgW* bgwp = BgAPI_cBgS_GetBgWPointer(dComIfGp_ea() + 0x12A0, polyInfo);
    if (bgwp != nullptr) {
        u8 f = bgwp->m_flags;
        if (f & 0x80)
            return false; /* ChkLock */
        if (f & 1)
            return true; /* ChkMoveBg */
    }
    return false;
}

/* 024EEB2C */
bool BgSystemHD::dBgS::ChkMoveBG_NoDABg(BgSystemHD::cBgS_PolyInfo* polyInfo) {
    BgSystemHD::dBgW* bgwp = BgAPI_cBgS_GetBgWPointer(dComIfGp_ea() + 0x12A0, polyInfo);
    if (bgwp != nullptr) {
        if (bgwp->m_flags & 1)
            return true;
    }
    return false;
}

/* GetPolyId0/1/2: the same body, info word 0/1/2 */
static inline s32 GetPolyIdN(BgSystemHD::dBgS* bgs, s32 bg_index, s32 poly_index, s32 defv,
                             u32 mask, u32 shift, u32 word, const char* file, u32 line,
                             const char* msg, const PolyInfAsserts& a) {
    if (!((u32)bg_index < 256))
        JUT_ASSERT_fail(file, line, msg);
    if (!bgs->m_chk_element[bg_index].ChkUsed())
        return defv;
    BgSystemHD::dBgW* bgwp = elm_bgw(bgs, bg_index);
    u32 v = dBgW_GetPolyInf(bgwp, poly_index, word, a) & mask;
    shift &= 63; /* srw: shifts of 32..63 give 0 */
    return shift >= 32 ? 0 : v >> shift;
}

/* 024EEB94 */
s32 BgSystemHD::dBgS::GetPolyId0(s32 bg_index, s32 poly_index, s32 defv, u32 mask, u32 shift) {
    return GetPolyIdN(this, bg_index, poly_index, defv, mask, shift, 0, "d_bg_s.cpp", 0x429,
                      "0 <= bg_index && bg_index < 256",
                      {"c_bg_w.h", "0 <= poly_index && poly_index < pm_bgd->m_t_num", 0x2F8,
                       "0 <= id && id < pm_bgd->m_ti_num"});
}

/* 024EEC9C: GetPolyCamId (not named by the matcher) */
/* 024EEC9C */
s32 BgSystemHD::dBgS::GetPolyCamId(s32 bg_index, s32 poly_index) {
    return GetPolyId0(bg_index, poly_index, 0xFF, 0xFF, 0);
}

/* 024EECAC */
u32 BgSystemHD::dBgS::GetMtrlSndId(BgSystemHD::cBgS_PolyInfo* p) {
    return GetPolyId0(p->m_bg_index, p->m_poly_index, 0, 0x1F00, 8);
}

/* 024EECC8 */
s32 BgSystemHD::dBgS::GetExitId(BgSystemHD::cBgS_PolyInfo* p) {
    return GetPolyId0(p->m_bg_index, p->m_poly_index, 0x3F, 0x7E000, 13);
}

/* 024EECE8 */
s32 BgSystemHD::dBgS::GetGrpRoomInfId(BgSystemHD::cBgS_PolyInfo* p) {
    u32 bg_index = p->m_bg_index;
    if (!(bg_index < 256))
        JUT_ASSERT_fail("d_bg_s.cpp", 0x48F, "0 <= bg_index && bg_index < 256");
    if (!m_chk_element[bg_index].ChkUsed())
        return 0xFF;
    BgSystemHD::dBgW* bgwp = elm_bgw(this, bg_index);
    s32 inf = bgwp->m_grp_room_inf;
    if (inf != 0xFF)
        return inf;
    s32 grp_id = BgAPI_cBgS_GetTriGrp(this, p->m_bg_index, p->m_poly_index);
    if (grp_id == -1)
        return 0xFF;
    return BgAPI_cBgS_GetGrpInf(this, p, grp_id) & 0xFF;
}

/* 024EEDB8 */
s32 BgSystemHD::dBgS::GetGrpSoundId(BgSystemHD::cBgS_PolyInfo* p) {
    s32 grp_id = BgAPI_cBgS_GetTriGrp(this, p->m_bg_index, p->m_poly_index);
    if (grp_id == -1)
        return -1;
    return (BgAPI_cBgS_GetGrpInf(this, p, grp_id) >> 11) & 0xFF;
}

/* 024EEE30 */
u32 BgSystemHD::dBgS::ChkGrpInf(BgSystemHD::cBgS_PolyInfo* p, u32 mask) {
    s32 grp_id = BgAPI_cBgS_GetTriGrp(this, p->m_bg_index, p->m_poly_index);
    if (grp_id == -1)
        return 0;
    u32 inf = BgAPI_cBgS_GetGrpInf(this, p, grp_id);
    return inf & mask;
}

// HD: polygon colours only for stage type 1 (native stage data getStagInfo).
/* 024EEEB8 */
s32 BgSystemHD::dBgS::GetPolyColor(BgSystemHD::cBgS_PolyInfo* p) {
    auto* stageDt = gabi::at<StageData>(dComIfGp_ea() + 0x5150);
    u32 info = gabi::call_ptr<u32>(gabi::at<StageVtable>(stageDt->mVtable)->getStagInfo.get(),
                                   stageDt); /* getStagInfo */
    if (((gabi::at<StageInfo>(info)->mType.get() >> 16) & 7) != 1)
        return 0xFF; /* dStage_stagInfo_GetSTType */
    if (p->m_poly_index == 0xFFFF || p->m_bg_index == 0x100)
        return 0xFF; /* !ChkSetInf */
    return GetPolyId0(p->m_bg_index, p->m_poly_index, 0xFF, 0x07F80000, 19);
}

/* 024EEF58 */
s32 BgSystemHD::dBgS::GetPolyId1(s32 bg_index, s32 poly_index, s32 defv, u32 mask, u32 shift) {
    return GetPolyIdN(this, bg_index, poly_index, defv, mask, shift, 1, "d_bg_s.cpp", 0x4FA,
                      "0 <= bg_index && bg_index < 256",
                      {"c_bg_w.h", "0 <= poly_index && poly_index < pm_bgd->m_t_num", 0x2FD,
                       "0 <= id && id < pm_bgd->m_ti_num"});
}

/* 024EF064 */
s32 BgSystemHD::dBgS::GetLinkNo(BgSystemHD::cBgS_PolyInfo* p) {
    return GetPolyId1(p->m_bg_index, p->m_poly_index, 0xFF, 0xFF, 0);
}

/* 024EF080 */
s32 BgSystemHD::dBgS::GetWallCode(BgSystemHD::cBgS_PolyInfo* p) {
    return GetPolyId1(p->m_bg_index, p->m_poly_index, 0, 0xF00, 8);
}

/* 024EF09C */
s32 BgSystemHD::dBgS::GetSpecialCode(BgSystemHD::cBgS_PolyInfo* p) {
    return GetPolyId1(p->m_bg_index, p->m_poly_index, 0, 0xF000, 12);
}

/* 024EF0BC */
s32 BgSystemHD::dBgS::GetGroundCode(BgSystemHD::cBgS_PolyInfo* p) {
    return GetPolyId1(p->m_bg_index, p->m_poly_index, 0, 0x03E00000, 21);
}

/* 024EF0D8 */
s32 BgSystemHD::dBgS::GetAttributeCodeDirect(BgSystemHD::cBgS_PolyInfo* p) {
    return GetPolyId1(p->m_bg_index, p->m_poly_index, 0, 0x001F0000, 16);
}

/* 024EF0F4: atr_conv at 101D51CC */
/* 024EF0F4 */
s32 BgSystemHD::dBgS::GetAttributeCode(BgSystemHD::cBgS_PolyInfo* p) {
    u32 attr = GetAttributeCodeDirect(p);
    if (attr >= 0x20)
        return 0;
    return atr_conv.mCode[attr];
}

/* 024EF130 */
s32 BgSystemHD::dBgS::GetRoomId(BgSystemHD::cBgS_PolyInfo* p) {
    if (p->m_poly_index == 0xFFFF)
        return -1; /* ChkSetInf */
    u32 id = p->m_bg_index;
    if (id == 0x100)
        return -1;
    if (!(id < 256))
        JUT_ASSERT_fail("d_bg_s.cpp", 0x601, "0 <= id && id < 256");
    if (!BgAPI_cBgS_ChkPolySafe(this, p))
        return -1;
    BgSystemHD::dBgW* bgwp = elm_bgw(this, id);
    u32 roomNo = bgwp->m_room_id;
    if (roomNo == 0xFFFF) {
        s32 grp = BgAPI_cBgS_GetTriGrp(this, p->m_bg_index, p->m_poly_index);
        roomNo = BgAPI_cBgS_GetGrpToRoomId(this, p->m_bg_index, grp);
        if (roomNo == 0xFFFF)
            return -1;
    }
    return roomNo;
}

/* 024EF218: GetPolyId2 (matcher: ChkPolyHSStick) */
/* 024EF218 */
s32 BgSystemHD::dBgS::GetPolyId2(s32 bg_index, s32 poly_index, s32 defv, u32 mask, u32 shift) {
    return GetPolyIdN(this, bg_index, poly_index, defv, mask, shift, 2, "d_bg_s.cpp", 0x5A7,
                      "0 <= bg_index && bg_index < 256",
                      {"c_bg_w.h", "0 <= poly_index && poly_index < pm_bgd->m_t_num", 0x302,
                       "0 <= id && id < pm_bgd->m_ti_num"});
}

/* 024EF324: GetCamMoveBG (not named by the matcher) */
/* 024EF324 */
s32 BgSystemHD::dBgS::GetCamMoveBG(BgSystemHD::cBgS_PolyInfo* p) {
    return GetPolyId2(p->m_bg_index, p->m_poly_index, 0xFF, 0xFF, 0);
}

/* 024EF340: GetRoomCamId (not named by the matcher) */
/* 024EF340 */
s32 BgSystemHD::dBgS::GetRoomCamId(BgSystemHD::cBgS_PolyInfo* p) {
    return GetPolyId2(p->m_bg_index, p->m_poly_index, 0xFF, 0xFF00, 8);
}

/* 024EF360 */
s32 BgSystemHD::dBgS::GetRoomPathId(BgSystemHD::cBgS_PolyInfo* p) {
    return GetPolyId2(p->m_bg_index, p->m_poly_index, 0xFF, 0x00FF0000, 16);
}

/* 024EF37C */
s32 BgSystemHD::dBgS::GetRoomPathPntNo(BgSystemHD::cBgS_PolyInfo* p) {
    return GetPolyId2(p->m_bg_index, p->m_poly_index, 0xFF, 0xFF000000, 24);
}

/* 024EF398: ChkPolyHSStick (matcher: GetPolyId2); BgSystemHD::dBgW::GetPolyHSStick inlined (info3 bit 4) */
/* 024EF398 */
u32 BgSystemHD::dBgS::ChkPolyHSStick(BgSystemHD::cBgS_PolyInfo* p) {
    u32 bg_index = p->m_bg_index;
    if (!(bg_index < 256))
        JUT_ASSERT_fail("d_bg_s.cpp", 0x669, "0 <= bg_index && bg_index < 256");
    if (!m_chk_element[bg_index].ChkUsed())
        return FALSE;
    BgSystemHD::dBgW* bgwp = elm_bgw(this, bg_index);
    s32 poly = p->m_poly_index;
    if (!(poly < bgwp->pm_bgd->m_t_num))
        JUT_ASSERT_fail("c_bg_w.h", 0x2F1, "0 <= poly_index && poly_index < pm_bgd->m_t_num");
    s32 id = gabi::at<Triangle>(bgwp->pm_bgd->m_t_tbl)[poly].mInfoIndex.get();
    if (!(id < bgwp->pm_bgd->m_ti_num))
        JUT_ASSERT_fail("c_bg_w.h", 0x307, "0 <= id && id < pm_bgd->m_ti_num");
    return gabi::at<PolygonInfo>(bgwp->pm_bgd->m_ti_tbl)[id].mInfo[3].get() & 0x10;
}

/* 024EF4A8 */
bool BgSystemHD::dBgS::LineCrossNonMoveBG(u8* chk) {
    u32 c = gabi::ea(chk);
    auto* view = gabi::at<LineCheck>(c);
    bool ret = false;
    /* ClearPi */
    view->mPoly.m_actor_id = 0xFFFFFFFF;
    view->mPoly.m_bgw = 0;
    view->mPoly.m_bg_index = 0x100;
    view->mPoly.m_poly_index = 0xFFFF;
    view->mFlag = view->mFlag.get() & ~0x10u; /* ClrHit */
    for (s32 bg_index = 0; bg_index < 256; bg_index++) {
        BgSystemHD::cBgS_ChkElm* elm = &m_chk_element[bg_index];
        if (elm->ChkUsed() && elm->m_bgw_base_ptr->pm_vtx_tbl != 0 &&
            !BgAPI_cBgS_Chk_ChkSameActorPid(c, elm->m_actor_id) &&
            !(elm->m_bgw_base_ptr->m_flags & 1)) {
            /* PreCalc */
            u32 f = view->mFlag.get();
            view->mPreWallChk = ((f >> 30) & 1) ^ 1;
            view->mPreGroundChk = (f >> 31) ^ 1;
            view->mPreRoofChk = ((f >> 29) & 1) ^ 1;
            BgSystemHD::dBgW* w = elm->m_bgw_base_ptr.get();
            if (BgAPI_cBgW_LineCheckGrpRp(w, c, w->m_rootGrpIdx, 1)) {
                /* SetActorInfo */
                view->mPoly.m_actor_id = elm->m_actor_id;
                view->mPoly.m_bgw = elm->m_bgw_base_ptr.v;
                ret = true;
                view->mPoly.m_bg_index = (u16)bg_index;
            }
        }
    }
    if (ret)
        view->mFlag = view->mFlag.get() | 0x10; /* SetHit */
    return ret;
}

/* 024EF5CC */
void BgSystemHD::dBgS::WallCorrect(dBgS_Acch* acch) {
    u32 a = gabi::ea(acch);
    /* CalcWallRR */
    for (s32 i = 0; i < acch->m_tbl_size; i++) {
        dBgS_AcchCir& cir = acch->pm_acch_cir.get()[i];
        f32 r = cir.m_wall_r;
        cir.m_wall_rr = r * r;
    }
    BgAPI_dBgS_Acch_SetWallCir(a);
    BgAPI_cM3dGLin_SetStartEnd(gabi::ea(&acch->m_lin), acch->pm_old_pos.v,
                               acch->pm_pos.v); /* SetLin */
    BgAPI_dBgS_Acch_CalcWallBmdCyl(a);
    for (s32 prio = 0; prio < 3; prio++) {
        for (s32 bg_index = 0; bg_index < 256; bg_index++) {
            BgSystemHD::cBgS_ChkElm* elm = &m_chk_element[bg_index];
            if (elm->ChkUsed() && elm->m_bgw_base_ptr->pm_vtx_tbl != 0) {
                /* SetNowActorInfo */
                u32 w = elm->m_bgw_base_ptr.v;
                gabi::at<AcchActorInfo>(a)->mpBgW = w;
                acch->m_ap_id = elm->m_actor_id;
                acch->m_bg_index = bg_index;
                if (!BgAPI_cBgS_Chk_ChkSameActorPid(a, elm->m_actor_id)) {
                    BgSystemHD::dBgW* bgwp = elm->m_bgw_base_ptr.get();
                    if ((u32)bgwp->m_priority == (u32)prio) /* ChkPriority */
                        BgAPI_dBgW_WallCorrectGrpRp(bgwp, a, bgwp->m_rootGrpIdx, 1);
                }
            }
        }
    }
}

/* 024EF6E8 */
f32 BgSystemHD::dBgS::RoofChk(u8* chk) {
    u32 c = gabi::ea(chk);
    auto* view = gabi::at<RoofCheck>(c);
    view->mPoly.m_bgw = 0;
    view->m_now_y = 1000000000.0f; /* SetNowY(G_CM3D_F_INF) */
    view->mPoly.m_actor_id = 0xFFFFFFFF;
    view->mPoly.m_poly_index = 0xFFFF;
    view->mPoly.m_bg_index = 0x100;
    for (s32 bg_index = 0; bg_index < 256; bg_index++) {
        BgSystemHD::cBgS_ChkElm* elm = &m_chk_element[bg_index];
        if (elm->ChkUsed() && elm->m_bgw_base_ptr->pm_vtx_tbl != 0 &&
            !BgAPI_cBgS_Chk_ChkSameActorPid(gabi::ea(&view->mCheck), elm->m_actor_id)) {
            BgSystemHD::dBgW* bgwp = elm->m_bgw_base_ptr.get();
            if (BgAPI_dBgW_RoofChkGrpRp(bgwp, c, bgwp->m_rootGrpIdx, 1)) {
                view->mPoly.m_actor_id = elm->m_actor_id;
                view->mPoly.m_bgw = elm->m_bgw_base_ptr.v;
                view->mPoly.m_bg_index = (u16)bg_index;
            }
        }
    }
    return view->m_now_y.get();
}

/* 024EF7C0 */
bool BgSystemHD::dBgS::SplGrpChk(u8* chk) {
    u32 c = gabi::ea(chk);
    auto* view = gabi::at<SpecialGroupCheck>(c);
    bool ret = false;
    BgAPI_dBgS_SplGrpChk_Init(c);
    for (s32 bg_index = 0; bg_index < 256; bg_index++) {
        BgSystemHD::cBgS_ChkElm* elm = &m_chk_element[bg_index];
        if (elm->ChkUsed() && elm->m_bgw_base_ptr->pm_vtx_tbl != 0 &&
            !BgAPI_cBgS_Chk_ChkSameActorPid(gabi::ea(&view->mCheck), elm->m_actor_id)) {
            BgSystemHD::dBgW* bgwp = elm->m_bgw_base_ptr.get();
            if (BgAPI_dBgW_SplGrpChkGrpRp(bgwp, c, bgwp->m_rootGrpIdx, 1)) {
                ret = true;
                view->mPoly.m_actor_id = elm->m_actor_id;
                view->mPoly.m_bgw = elm->m_bgw_base_ptr.v;
                view->mPoly.m_bg_index = (u16)bg_index;
                view->mFlag = view->mFlag.get() | 1; /* OnFind */
            }
        }
    }
    return ret;
}

/* 024EF88C */
bool BgSystemHD::dBgS::SphChk(u8* chk, void* user) {
    u32 c = gabi::ea(chk);
    auto* view = gabi::at<SphereCheck>(c);
    bool ret = false;
    /* Init: ClearPi */
    view->mPoly.m_poly_index = 0xFFFF;
    view->mPoly.m_bg_index = 0x100;
    view->mPoly.m_bgw = 0;
    view->mPoly.m_actor_id = 0xFFFFFFFF;
    for (s32 bg_index = 0; bg_index < 256; bg_index++) {
        BgSystemHD::cBgS_ChkElm* elm = &m_chk_element[bg_index];
        if (elm->ChkUsed() && elm->m_bgw_base_ptr->pm_vtx_tbl != 0 &&
            !BgAPI_cBgS_Chk_ChkSameActorPid(gabi::ea(&view->mCheck), elm->m_actor_id)) {
            BgSystemHD::dBgW* bgwp = elm->m_bgw_base_ptr.get();
            if (BgAPI_dBgW_SphChkGrpRp(bgwp, c, user, bgwp->m_rootGrpIdx, 1)) {
                view->mPoly.m_actor_id = elm->m_actor_id;
                view->mPoly.m_bgw = elm->m_bgw_base_ptr.v;
                ret = true;
                view->mPoly.m_bg_index = (u16)bg_index;
            }
        }
    }
    return ret;
}

/* MoveBgCrrPos / MoveBgTransPos / MoveBgMatrixCrrPos: virtual BgSystemHD::dBgW::CrrPos 0x44 / TransPos 0x4C /
 * MatrixCrrPos 0x54 */
static inline void MoveBgProc(BgSystemHD::dBgS* bgs, BgSystemHD::cBgS_PolyInfo* polyInfo,
                              u32 accept, cXyz* pos, csXyz* angle, csXyz* shape_angle,
                              const char* file, u32 line, const char* msg, be<u32> BgVtable::* slot,
                              bool polySafe) {
    if (accept == 0)
        return;
    u32 bg_index = polyInfo->m_bg_index;
    if (bg_index == 0x100)
        return; /* !ChkBgIndex */
    if (!(bg_index < 256))
        JUT_ASSERT_fail(file, line, msg);
    BgSystemHD::cBgS_ChkElm* elm = &bgs->m_chk_element[bg_index];
    if (elm->ChkUsed()) {
        BgSystemHD::dBgW* bgwp = elm->m_bgw_base_ptr.get();
        if ((bgwp->m_move_flag & 1) && (!polySafe || BgAPI_cBgS_ChkPolySafe(bgs, polyInfo))) {
            u32 fn = (gabi::at<BgVtable>(bgwp->__vtbl)->*slot).get();
            gabi::call_ptr(fn, bgwp, polyInfo, (u32)elm->m_actor_ptr, accept, pos, angle,
                           shape_angle);
        }
    }
}

/* 024EF968 */
void BgSystemHD::dBgS::MoveBgCrrPos(BgSystemHD::cBgS_PolyInfo* polyInfo, u32 accept, cXyz* pos,
                                    csXyz* angle, csXyz* shape_angle) {
    MoveBgProc(this, polyInfo, accept, pos, angle, shape_angle, "d_bg_s.cpp", 0x85D,
               "0 <= bg_index && bg_index < 256", &BgVtable::mCrrPos, true);
}

/* 024EFA38 */
void BgSystemHD::dBgS::MoveBgTransPos(BgSystemHD::cBgS_PolyInfo* polyInfo, u32 accept, cXyz* pos,
                                      csXyz* angle, csXyz* shape_angle) {
    MoveBgProc(this, polyInfo, accept, pos, angle, shape_angle, "d_bg_s.cpp", 0x88A,
               "0 <= bg_index && bg_index < 256", &BgVtable::mTransPos, true);
}

/* 024EFB08 */
void BgSystemHD::dBgS::MoveBgMatrixCrrPos(BgSystemHD::cBgS_PolyInfo* polyInfo, u32 accept,
                                          cXyz* pos, csXyz* angle, csXyz* shape_angle) {
    MoveBgProc(this, polyInfo, accept, pos, angle, shape_angle, "d_bg_s.cpp", 0x8B5,
               "0 <= bg_index && bg_index < 256", &BgVtable::mMatrixCrrPos, false);
}

/* 024EFBC0 */
void BgSystemHD::dBgS::RideCallBack(BgSystemHD::cBgS_PolyInfo* polyInfo, fopAc_ac_c* ac) {
    u32 bg_index = polyInfo->m_bg_index;
    if (!(bg_index < 256))
        JUT_ASSERT_fail("d_bg_s.cpp", 0x967, "0 <= bg_index && bg_index < 256");
    BgSystemHD::dBgW* bgwp = elm_bgw(this, bg_index);
    if (bgwp->m_id < 0x100 && bgwp->m_ride_callback != 0)
        gabi::call_ptr(bgwp->m_ride_callback, bgwp, (u32)m_chk_element[bg_index].m_actor_ptr, ac);
}

/* 024EFC54 */
u32 BgSystemHD::dBgS::PushPullCallBack(BgSystemHD::cBgS_PolyInfo* polyInfo, fopAc_ac_c* ac, u32 inf,
                                       u32 lbl) {
    u32 bg_index = polyInfo->m_bg_index;
    if (!(bg_index < 256))
        JUT_ASSERT_fail("d_bg_s.cpp", 0x988, "0 <= bg_index && bg_index < 256");
    BgSystemHD::dBgW* bgwp = elm_bgw(this, bg_index);
    if (!(bgwp->m_id < 0x100))
        return 0;
    u32 actor = m_chk_element[bg_index].m_actor_ptr;
    if (actor == 0)
        return 0;
    u32 cb = bgwp->m_push_pull_callback;
    if (cb == 0)
        return 0;
    return gabi::call_ptr<u32>(cb, actor, ac, inf, lbl);
}

/* WWHD glue (tools/verify/autoglue.py)
str 10043354 "0 <= bg_index && bg_index < 256" in 024EEB94
str 10043374 "0 <= poly_index && poly_index < pm_bgd->m_t_num" in 024EEB94
str 100433A4 "d_bg_s.cpp" in 024EEB94
str 100433B0 "c_bg_w.h" in 024EEB94
str 100433BC "0 <= id && id < pm_bgd->m_ti_num" in 024EEB94
str 100433E0 "0 <= bg_index && bg_index < 256" in 024EECE8
str 10043400 "d_bg_s.cpp" in 024EECE8
str 1004340C "0 <= bg_index && bg_index < 256" in 024EEF58
str 1004342C "0 <= poly_index && poly_index < pm_bgd->m_t_num" in 024EEF58
str 1004345C "d_bg_s.cpp" in 024EEF58
str 10043468 "c_bg_w.h" in 024EEF58
str 10043474 "0 <= id && id < pm_bgd->m_ti_num" in 024EEF58
str 10043498 "0 <= id && id < 256" in 024EF130
str 100434AC "d_bg_s.cpp" in 024EF130
str 100434BC "0 <= bg_index && bg_index < 256" in 024EF218
str 100434DC "0 <= poly_index && poly_index < pm_bgd->m_t_num" in 024EF218
str 1004350C "d_bg_s.cpp" in 024EF218
str 10043518 "c_bg_w.h" in 024EF218
str 10043524 "0 <= id && id < pm_bgd->m_ti_num" in 024EF218
str 10043560 "0 <= bg_index && bg_index < 256" in 024EF398
str 10043580 "0 <= poly_index && poly_index < pm_bgd->m_t_num" in 024EF398
str 100435B0 "d_bg_s.cpp" in 024EF398
str 100435BC "c_bg_w.h" in 024EF398
str 100435C8 "0 <= id && id < pm_bgd->m_ti_num" in 024EF398
str 100435EC "0 <= bg_index && bg_index < 256" in 024EF968
str 1004360C "d_bg_s.cpp" in 024EF968
str 10043618 "0 <= bg_index && bg_index < 256" in 024EFA38
str 10043638 "d_bg_s.cpp" in 024EFA38
str 10043644 "0 <= bg_index && bg_index < 256" in 024EFB08
str 10043664 "d_bg_s.cpp" in 024EFB08
str 10043670 "0 <= bg_index && bg_index < 256" in 024EFBC0
str 10043690 "d_bg_s.cpp" in 024EFBC0
str 1004369C "0 <= bg_index && bg_index < 256" in 024EFC54
str 100436BC "d_bg_s.cpp" in 024EFC54
ext 02008B60 void BgAPI_CheckConstruct(void*)
ext 02018C40 void BgAPI_SphereConstruct(void*)
data atr_conv 101D51CC
ext 02008020 void BgAPI_cBgS_Ct(BgSystemHD::dBgS*)
ext 0200805C void BgAPI_cBgS_Dt(BgSystemHD::dBgS*)
ext 02008B48 void BgAPI_cBgS_Move(BgSystemHD::dBgS*)
ext 020086D4 u32 BgAPI_cBgS_Regist(BgSystemHD::dBgS*, BgSystemHD::dBgW*, u32, fopAc_ac_c*)
ext 02008498 BgSystemHD::dBgW* BgAPI_cBgS_GetBgWPointer(u32, BgSystemHD::cBgS_PolyInfo*)
ext 020082C0 s32 BgAPI_cBgS_GetTriGrp(BgSystemHD::dBgS*, u32, u32)
ext 020085F8 u32 BgAPI_cBgS_GetGrpInf(BgSystemHD::dBgS*, BgSystemHD::cBgS_PolyInfo*, s32)
ext 0200839C u32 BgAPI_cBgS_GetGrpToRoomId(BgSystemHD::dBgS*, u32, s32)
ext 02008254 BOOL BgAPI_cBgS_ChkPolySafe(BgSystemHD::dBgS*, BgSystemHD::cBgS_PolyInfo*)
ext 02008BB8 BOOL BgAPI_cBgS_Chk_ChkSameActorPid(u32, u32)
ext 0200AB4C BOOL BgAPI_cBgW_LineCheckGrpRp(BgSystemHD::dBgW*, u32, s32, s32)
ext 024F3498 void BgAPI_dBgW_WallCorrectGrpRp(BgSystemHD::dBgW*, u32, s32, s32)
ext 024F3924 BOOL BgAPI_dBgW_RoofChkGrpRp(BgSystemHD::dBgW*, u32, s32, s32)
ext 024F3DD4 BOOL BgAPI_dBgW_SplGrpChkGrpRp(BgSystemHD::dBgW*, u32, s32, s32)
ext 024F42BC BOOL BgAPI_dBgW_SphChkGrpRp(BgSystemHD::dBgW*, u32, void*, s32, s32)
ext 024F220C void BgAPI_dBgS_SplGrpChk_Init(u32)
ext 024F10A8 void BgAPI_dBgS_Acch_SetWallCir(u32)
ext 024F1134 void BgAPI_dBgS_Acch_CalcWallBmdCyl(u32)
ext 02018808 void BgAPI_cM3dGLin_SetStartEnd(u32, u32, u32)
ext 028E91EC BOOL BgAPI_PSMTXInverse(u32, u32)
ext 028E8F64 void BgAPI_PSMTXMultVec(u32, u32, u32)
ext 028E8D88 void BgAPI_PSVECAdd(u32, u32, u32)
ext 0200A3BC void BgAPI_cBgW_GetTrans(BgSystemHD::dBgW*, u32)
*/
