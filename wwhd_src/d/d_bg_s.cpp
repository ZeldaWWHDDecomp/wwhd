/* d_bg_s: background collision system (dBgS), WWHD. 
 *
 * Translation unit 024EE58C..024EFD08. Ported from the GameCube d_bg_s.cpp; the GameCube
 * JUT_ASSERTs are still present in HD (JUTAssertion 0273AA24 with per-instance strings), also
 * those of the inlined dBgW accessors (GetPolyInfId and friends). */
#include "bindings.h"

namespace d_bg_s_cpp {

/* ---- local layouts (SHARED-CANDIDATE) ---- */

/* cBgS_PolyInfo (0x10): HD order poly index, bg index, bgw, actor id, vtable */
struct cBgS_PolyInfo_l {
    /* 0x0 */ be<u16> m_poly_index;
    /* 0x2 */ be<u16> m_bg_index;
    /* 0x4 */ be<u32> m_bgw;
    /* 0x8 */ be<u32> m_actor_id;
    /* 0xC */ be<u32> __vtbl;
};
WWHD_SIZE(cBgS_PolyInfo_l, 0x10);

/* cBgD_t: the collision data of a dBgW */
struct cBgD_t_l {
    /* 0x00 */ u8 _00[8];
    /* 0x08 */ be<s32> m_t_num;   /* triangles */
    /* 0x0C */ be<u32> m_t_tbl;   /* stride 0xA, +6 u16 info index */
    /* 0x10 */ u8 _10[0x18];
    /* 0x28 */ be<s32> m_ti_num;  /* infos */
    /* 0x2C */ be<u32> m_ti_tbl;  /* stride 0x10: info0..info3 */
};
WWHD_OFFSET(cBgD_t_l, m_ti_tbl, 0x2C);

/* dBgW (cBgW base): only the members used here */
struct dBgW_l {
    /* 0x00 */ be<u32> m_id;            /* registration id; ChkUsed: < 0x100 */
    /* 0x04 */ be<u32> __vtbl;          /* CrrPos 0x44, TransPos 0x4C, MatrixCrrPos 0x54 */
    /* 0x08 */ be<u32> pm_base;         /* base matrix pointer */
    /* 0x0C */ Mtx34 mOldMtx;
    /* 0x3C */ u8 _3C[0x30];
    /* 0x6C */ be<u8> m_flags;          /* 1: move bg, 0x80: lock */
    /* 0x6D */ u8 _6D[8];
    /* 0x75 */ be<u8> m_priority;
    /* 0x76 */ u8 _76[0x1A];
    /* 0x90 */ be<u32> pm_vtx_tbl;
    /* 0x94 */ gptr<cBgD_t_l> pm_bgd;
    /* 0x98 */ u8 _98[0xC];
    /* 0xA4 */ be<s32> m_rootGrpIdx;
    /* 0xA8 */ u8 _A8[4];
    /* 0xAC */ be<s16> m_old_shape_angle_y;
    /* 0xAE */ be<s16> m_diff_shape_angle_y;
    /* 0xB0 */ be<u32> m_ride_callback;
    /* 0xB4 */ be<u32> m_push_pull_callback;
    /* 0xB8 */ be<u16> m_room_id;
    /* 0xBA */ be<u8> m_move_flag;      /* bit 0 */
    /* 0xBB */ be<u8> m_grp_room_inf;
};
WWHD_OFFSET(dBgW_l, m_flags, 0x6C);
WWHD_OFFSET(dBgW_l, m_priority, 0x75);
WWHD_OFFSET(dBgW_l, pm_vtx_tbl, 0x90);
WWHD_OFFSET(dBgW_l, m_rootGrpIdx, 0xA4);
WWHD_OFFSET(dBgW_l, m_old_shape_angle_y, 0xAC);
WWHD_OFFSET(dBgW_l, m_grp_room_inf, 0xBB);

/* cBgS_ChkElm (0x14) */
struct cBgS_ChkElm_l {
    /* 0x00 */ gptr<dBgW_l> m_bgw_base_ptr;
    /* 0x04 */ be<u32> m_used;          /* ChkUsed: bit 0 */
    /* 0x08 */ be<u32> m_actor_id;
    /* 0x0C */ be<u32> m_actor_ptr;
    /* 0x10 */ be<u32> __vtbl;
    bool ChkUsed() { return (m_used & 1) != 0; }
};
WWHD_SIZE(cBgS_ChkElm_l, 0x14);

struct dBgS_l {
    /* 0x0000 */ cBgS_ChkElm_l m_chk_element[256];
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void cBgS_Ct(dBgS_l* s) { gabi::call(0x02008020, s); }
static inline void cBgS_Dt(dBgS_l* s) { gabi::call(0x0200805C, s); }
static inline void cBgS_Move(dBgS_l* s) { gabi::call(0x02008B48, s); }
static inline u32 cBgS_Regist(dBgS_l* s, dBgW_l* w, u32 id, fopAc_ac_c* ac) { return gabi::call<u32>(0x020086D4, s, w, id, ac); }
static inline dBgW_l* cBgS_GetBgWPointer(u32 bgs, cBgS_PolyInfo_l* p) { return gabi::call<dBgW_l*>(0x02008498, bgs, p); }
static inline s32 cBgS_GetTriGrp(dBgS_l* s, u32 bg, u32 poly) { return gabi::call<s32>(0x020082C0, s, bg, poly); }
static inline u32 cBgS_GetGrpInf(dBgS_l* s, cBgS_PolyInfo_l* p, s32 grp) { return gabi::call<u32>(0x020085F8, s, p, grp); }
static inline u32 cBgS_GetGrpToRoomId(dBgS_l* s, u32 bg, s32 grp) { return gabi::call<u32>(0x0200839C, s, bg, grp); }
static inline BOOL cBgS_ChkPolySafe(dBgS_l* s, cBgS_PolyInfo_l* p) { return gabi::call<BOOL>(0x02008254, s, p); }
static inline BOOL cBgS_Chk_ChkSameActorPid(u32 chk, u32 pid) { return gabi::call<BOOL>(0x02008BB8, chk, pid); }
static inline BOOL cBgW_LineCheckGrpRp(dBgW_l* w, u32 chk, s32 grp, s32 depth) { return gabi::call<BOOL>(0x0200AB4C, w, chk, grp, depth); }
static inline void dBgW_WallCorrectGrpRp(dBgW_l* w, u32 acch, s32 grp, s32 depth) { gabi::call(0x024F3498, w, acch, grp, depth); }
static inline BOOL dBgW_RoofChkGrpRp(dBgW_l* w, u32 chk, s32 grp, s32 depth) { return gabi::call<BOOL>(0x024F3924, w, chk, grp, depth); }
static inline BOOL dBgW_SplGrpChkGrpRp(dBgW_l* w, u32 chk, s32 grp, s32 depth) { return gabi::call<BOOL>(0x024F3DD4, w, chk, grp, depth); }
static inline BOOL dBgW_SphChkGrpRp(dBgW_l* w, u32 chk, void* user, s32 grp, s32 depth) { return gabi::call<BOOL>(0x024F42BC, w, chk, user, grp, depth); }
static inline void dBgS_SplGrpChk_Init(u32 chk) { gabi::call(0x024F220C, chk); }
static inline void dBgS_Acch_SetWallCir(u32 acch) { gabi::call(0x024F10A8, acch); }
static inline void dBgS_Acch_CalcWallBmdCyl(u32 acch) { gabi::call(0x024F1134, acch); }
static inline void cM3dGLin_SetStartEnd(u32 lin, u32 start, u32 end) { gabi::call(0x02018808, lin, start, end); }
static inline BOOL PSMTXInverse(u32 src, u32 dst) { return gabi::call<BOOL>(0x028E91EC, src, dst); }
static inline void PSMTXMultVec(u32 m, u32 src, u32 dst) { gabi::call(0x028E8F64, m, src, dst); }
static inline void PSVECAdd(u32 a, u32 b, u32 dst) { gabi::call(0x028E8D88, a, b, dst); }
static inline void cBgW_GetTrans(dBgW_l* w, u32 out) { gabi::call(0x0200A3BC, w, out); }

static inline dBgW_l* elm_bgw(dBgS_l* s, s32 i) { return s->m_chk_element[i].m_bgw_base_ptr.get(); }

/* dBgW::GetPolyInfId + GetPolyInfN (inline, with their asserts; strings per instance):
 * info word `word` (0..3) of the polygon's info entry */
struct PolyInfAsserts { u32 file, msg_poly, line_id, msg_id; };
static inline u32 dBgW_GetPolyInf(dBgW_l* bgw, s32 poly_index, u32 word, const PolyInfAsserts& a) {
    if (!(0 <= poly_index && poly_index < bgw->pm_bgd->m_t_num))
        JUT_ASSERT_fail(STR(a.file), 0x2F1, STR(a.msg_poly));
    s32 id = gabi::load<u16>(bgw->pm_bgd->m_t_tbl + poly_index * 0xA + 6);
    if (!(id < bgw->pm_bgd->m_ti_num))
        JUT_ASSERT_fail(STR(a.file), a.line_id, STR(a.msg_id));
    return gabi::load<u32>(bgw->pm_bgd->m_ti_tbl + id * 0x10 + word * 4);
}

/* 024EE58C, 024EE620, 024EE634: NOTE: these three belong to the d_attention TU (its __sinit and
 * SafeString inline copies, referenced only from d_attention's SafeString vtable 100430F8); d_bg_s proper starts at
 * 024EE638. Verified here first; kept here so each address is verified once.
 * 024EE58C: static initialisers (header statics) */
static void __sinit_d_bg_s_cpp() {
    WWHD_FUNC(0x024EE58C, void, (u32)0);
    sinit_header_statics(0x1046ECD4, 0x101D515C);
}
VERIFY(0x024EE58C, __sinit_d_bg_s_cpp);

/* 024EE620: deleting destructor of an empty class of this unit (no members to destroy) */
static void dBgS_unit_empty_dt(void* p, s32 flags) {
    WWHD_FUNC(0x024EE620, void, p, flags);
    if (p == nullptr) return;
    if (!(flags & 1)) return;
    operator_delete(p);
}
VERIFY(0x024EE620, dBgS_unit_empty_dt);

/* 024EE634: empty function */
static void dBgS_unit_empty() {
    WWHD_FUNC(0x024EE634, void, (u32)0);
}
VERIFY(0x024EE634, dBgS_unit_empty);

/* 024EE638 */
static void dBgS_ChangeAttributeCode(u32 code, be<u32>* dst) {
    WWHD_FUNC(0x024EE638, void, code, dst);
    *dst = (*dst & 0xFFE0FFFF) | (code << 16);
}
VERIFY(0x024EE638, dBgS_ChangeAttributeCode);

/* 024EE650 */
static u32 dBgS_GetRoomPathPntNo(u32 param) {
    WWHD_FUNC(0x024EE650, u32, param);
    return param >> 24;
}
VERIFY(0x024EE650, dBgS_GetRoomPathPntNo);

/* 024EE658 */
static void dBgS_MoveBGProc_Typical(dBgW_l* pbgw, void* user, cBgS_PolyInfo_l* polyInfo, u32 accept, cXyz* pos,
                                    csXyz* angle, csXyz* shape_angle) {
    WWHD_FUNC(0x024EE658, void, pbgw, user, polyInfo, accept, pos, angle, shape_angle);
    gabi::Local<Mtx34> inv;
    if (PSMTXInverse(gabi::ea(&pbgw->mOldMtx), gabi::ea(inv.get()))) {
        gabi::Local<cXyz> local, newPos;
        PSMTXMultVec(gabi::ea(inv.get()), gabi::ea(pos), gabi::ea(local.get()));
        PSMTXMultVec(pbgw->pm_base, gabi::ea(local.get()), gabi::ea(newPos.get()));
        pos->copy(*newPos);
    }
}
VERIFY(0x024EE658, dBgS_MoveBGProc_Typical);

/* 024EE6D8 */
static void dBgS_MoveBGProc_RotY(dBgW_l* pbgw, void* user, cBgS_PolyInfo_l* polyInfo, u32 accept, cXyz* pos,
                                 csXyz* angle, csXyz* shape_angle) {
    WWHD_FUNC(0x024EE6D8, void, pbgw, user, polyInfo, accept, pos, angle, shape_angle);
    if (shape_angle == nullptr) return;
    s32 rot = pbgw->m_diff_shape_angle_y;
    shape_angle->y = (s16)(shape_angle->y + rot);
    if (angle != nullptr) angle->y = (s16)(angle->y + rot);
}
VERIFY(0x024EE6D8, dBgS_MoveBGProc_RotY);

/* 024EE708 */
static void dBgS_MoveBGProc_TypicalRotY(dBgW_l* pbgw, void* user, cBgS_PolyInfo_l* polyInfo, u32 accept, cXyz* pos,
                                        csXyz* angle, csXyz* shape_angle) {
    WWHD_FUNC(0x024EE708, void, pbgw, user, polyInfo, accept, pos, angle, shape_angle);
    dBgS_MoveBGProc_Typical(pbgw, user, polyInfo, accept, pos, angle, shape_angle);
    dBgS_MoveBGProc_RotY(pbgw, user, polyInfo, accept, pos, angle, shape_angle);
}
VERIFY(0x024EE708, dBgS_MoveBGProc_TypicalRotY);

/* 024EE76C */
static void dBgS_MoveBGProc_Trans(dBgW_l* pbgw, void* user, cBgS_PolyInfo_l* polyInfo, u32 accept, cXyz* pos,
                                  csXyz* angle, csXyz* shape_angle) {
    WWHD_FUNC(0x024EE76C, void, pbgw, user, polyInfo, accept, pos, angle, shape_angle);
    gabi::Local<cXyz> trans;
    cBgW_GetTrans(pbgw, gabi::ea(trans.get()));
    PSVECAdd(gabi::ea(pos), gabi::ea(trans.get()), gabi::ea(pos));
}
VERIFY(0x024EE76C, dBgS_MoveBGProc_Trans);

/* 024EE7AC: dBgS_RoofChk::dBgS_RoofChk (HD 0x4C; GameCube d_bg_s_roof_chk.cpp, emitted here) */
static void* dBgS_RoofChk_ct(void* p) {
    WWHD_FUNC(0x024EE7AC, void*, p);
    if (p == nullptr) {
        p = operator_new(0x4C);
        if (p == nullptr) return p;
    }
    u32 b = gabi::ea(p);
    /* cBgS_PolyInfo */
    gabi::store<u16>(b + 0x2, 0x100);
    gabi::store<u32>(b + 0xC, 0x10043244);
    gabi::store<u32>(b + 0x4, 0);
    gabi::store<u32>(b + 0x8, 0xFFFFFFFF);
    gabi::store<u16>(b + 0x0, 0xFFFF);
    gabi::call(0x02008B60, b + 0x10); /* cBgS_Chk::cBgS_Chk */
    gabi::store<u8>(b + 0x2C, 0);
    gabi::store<u32>(b + 0x18, 0xFFFFFFFF);
    gabi::store<f32>(b + 0x38, 0.0f);
    gabi::store<u8>(b + 0x2B, 0);
    gabi::store<f32>(b + 0x40, 0.0f);
    gabi::store<f32>(b + 0x3C, 0.0f);
    gabi::store<u32>(b + 0x10, b + 0x24);
    gabi::store<u32>(b + 0x20, 0x100432C4);
    gabi::store<u8>(b + 0x29, 0);
    gabi::store<u8>(b + 0x2A, 0);
    gabi::store<u32>(b + 0x14, b + 0x30);
    gabi::store<u32>(b + 0x30, 0x100432D4);
    gabi::store<f32>(b + 0x48, 0.0f);
    gabi::store<u32>(b + 0x24, 0x100432E4);
    gabi::store<u32>(b + 0xC, 0x100432B4);
    gabi::store<u8>(b + 0x2D, 0);
    gabi::store<u32>(b + 0x34, 1);
    gabi::store<u8>(b + 0x28, 0);
    gabi::store<u8>(b + 0x2E, 0);
    gabi::store<u32>(b + 0x44, 0);
    return p;
}
VERIFY(0x024EE7AC, dBgS_RoofChk_ct);

/* 024EE8B8: dBgS_SphChk::dBgS_SphChk (HD 0x50: cM3dGSph base, poly info +0x14, cBgS_Chk +0x24) */
static void* dBgS_SphChk_ct(void* p) {
    WWHD_FUNC(0x024EE8B8, void*, p);
    if (p == nullptr) {
        p = operator_new(0x50);
        if (p == nullptr) return p;
    }
    u32 b = gabi::ea(p);
    gabi::call(0x02018C40, b); /* cM3dGSph::cM3dGSph */
    gabi::store<u16>(b + 0x16, 0x100);
    gabi::store<u32>(b + 0x20, 0x10043244);
    gabi::store<u32>(b + 0x1C, 0xFFFFFFFF);
    gabi::store<u16>(b + 0x14, 0xFFFF);
    gabi::store<u32>(b + 0x18, 0);
    gabi::call(0x02008B60, b + 0x24); /* cBgS_Chk::cBgS_Chk */
    gabi::store<u32>(b + 0x4C, 0);
    gabi::store<u8>(b + 0x3E, 0);
    gabi::store<u8>(b + 0x3D, 0);
    gabi::store<u32>(b + 0x44, 0x10043324);
    gabi::store<u32>(b + 0x2C, 0xFFFFFFFF);
    gabi::store<u32>(b + 0x48, 1);
    gabi::store<u32>(b + 0x10, 0x100432F4);
    gabi::store<u32>(b + 0x28, b + 0x44);
    gabi::store<u32>(b + 0x34, 0x10043314);
    gabi::store<u8>(b + 0x3F, 0);
    gabi::store<u32>(b + 0x20, 0x10043304);
    gabi::store<u32>(b + 0x38, 0x10043334);
    gabi::store<u8>(b + 0x40, 0);
    gabi::store<u8>(b + 0x42, 0);
    gabi::store<u8>(b + 0x41, 0);
    /* Init(): ClearPi */
    gabi::store<u16>(b + 0x16, 0x100);
    gabi::store<u16>(b + 0x14, 0xFFFF);
    gabi::store<u32>(b + 0x18, 0);
    gabi::store<u32>(b + 0x1C, 0xFFFFFFFF);
    gabi::store<u8>(b + 0x3C, 0);
    gabi::store<u32>(b + 0x24, b + 0x38);
    return p;
}
VERIFY(0x024EE8B8, dBgS_SphChk_ct);

/* 024EE9C0 */
static void dBgS_Ct(dBgS_l* bgs) {
    WWHD_FUNC(0x024EE9C0, void, bgs);
    cBgS_Ct(bgs);
}
VERIFY(0x024EE9C0, dBgS_Ct);

/* 024EE9C4 */
static void dBgS_Dt(dBgS_l* bgs) {
    WWHD_FUNC(0x024EE9C4, void, bgs);
    cBgS_Dt(bgs);
}
VERIFY(0x024EE9C4, dBgS_Dt);

/* 024EE9C8 */
static void dBgS_ClrMoveFlag(dBgS_l* bgs) {
    WWHD_FUNC(0x024EE9C8, void, bgs);
    for (s32 i = 0; i < 256; i++) {
        if (bgs->m_chk_element[i].ChkUsed()) {
            dBgW_l* bgwp = elm_bgw(bgs, i);
            bgwp->m_move_flag = bgwp->m_move_flag & 0xFE; /* OffMoveFlag */
        }
    }
}
VERIFY(0x024EE9C8, dBgS_ClrMoveFlag);

/* 024EEA00 */
static void dBgS_Move(dBgS_l* bgs) {
    WWHD_FUNC(0x024EEA00, void, bgs);
    cBgS_Move(bgs);
    for (s32 i = 0; i < 256; i++) {
        cBgS_ChkElm_l& e = bgs->m_chk_element[i];
        if (e.ChkUsed()) {
            dBgW_l* bgwp = e.m_bgw_base_ptr.get();
            s16 y = gabi::load<s16>(e.m_actor_ptr + 0x32A); /* shape_angle.y */
            s16 old = bgwp->m_old_shape_angle_y;           /* CalcDiffShapeAngleY */
            bgwp->m_old_shape_angle_y = y;
            bgwp->m_diff_shape_angle_y = (s16)(y - old);
        }
    }
}
VERIFY(0x024EEA00, dBgS_Move);

/* 024EEA6C */
static u32 dBgS_Regist(dBgS_l* bgs, dBgW_l* bgw, fopAc_ac_c* ac) {
    WWHD_FUNC(0x024EEA6C, u32, bgs, bgw, ac);
    if (bgw == nullptr) return 1;
    u32 a = gabi::ea(ac);
    if (ac != nullptr && (bgw->m_flags & 1)) {
        bgw->m_old_shape_angle_y = gabi::load<s16>(a + 0x32A);
        bgw->m_room_id = (u16)(s16)gabi::load<s8>(a + 0x326); /* SetRoomId(fopAcM_GetRoomNo(ac)) */
    }
    u32 id = ac != nullptr ? gabi::load<u32>(a + 4) : 0xFFFFFFFF;
    return cBgS_Regist(bgs, bgw, id, ac);
}
VERIFY(0x024EEA6C, dBgS_Regist);

/* 024EEABC: HD uses the global dBgS (play + 0x12A0), not `this` */
static bool dBgS_ChkMoveBG(dBgS_l* bgs, cBgS_PolyInfo_l* polyInfo) {
    WWHD_FUNC(0x024EEABC, bool, bgs, polyInfo);
    dBgW_l* bgwp = cBgS_GetBgWPointer(dComIfGp_ea() + 0x12A0, polyInfo);
    if (bgwp != nullptr) {
        u8 f = bgwp->m_flags;
        if (f & 0x80) return false; /* ChkLock */
        if (f & 1) return true;     /* ChkMoveBg */
    }
    return false;
}
VERIFY(0x024EEABC, dBgS_ChkMoveBG);

/* 024EEB2C */
static bool dBgS_ChkMoveBG_NoDABg(dBgS_l* bgs, cBgS_PolyInfo_l* polyInfo) {
    WWHD_FUNC(0x024EEB2C, bool, bgs, polyInfo);
    dBgW_l* bgwp = cBgS_GetBgWPointer(dComIfGp_ea() + 0x12A0, polyInfo);
    if (bgwp != nullptr) {
        if (bgwp->m_flags & 1) return true;
    }
    return false;
}
VERIFY(0x024EEB2C, dBgS_ChkMoveBG_NoDABg);

/* GetPolyId0/1/2: the same body, info word 0/1/2 */
static inline s32 GetPolyIdN(dBgS_l* bgs, s32 bg_index, s32 poly_index, s32 defv, u32 mask, u32 shift, u32 word,
                             u32 file, u32 line, u32 msg, const PolyInfAsserts& a) {
    if (!((u32)bg_index < 256)) JUT_ASSERT_fail(STR(file), line, STR(msg));
    if (!bgs->m_chk_element[bg_index].ChkUsed()) return defv;
    dBgW_l* bgwp = elm_bgw(bgs, bg_index);
    u32 v = dBgW_GetPolyInf(bgwp, poly_index, word, a) & mask;
    shift &= 63; /* srw: shifts of 32..63 give 0 */
    return shift >= 32 ? 0 : v >> shift;
}

/* 024EEB94 */
static s32 dBgS_GetPolyId0(dBgS_l* bgs, s32 bg_index, s32 poly_index, s32 defv, u32 mask, u32 shift) {
    WWHD_FUNC(0x024EEB94, s32, bgs, bg_index, poly_index, defv, mask, shift);
    return GetPolyIdN(bgs, bg_index, poly_index, defv, mask, shift, 0, 0x100433A4, 0x429, 0x10043354,
                      {0x100433B0, 0x10043374, 0x2F8, 0x100433BC});
}
VERIFY(0x024EEB94, dBgS_GetPolyId0);

/* 024EEC9C: GetPolyCamId (not named by the matcher) */
static s32 dBgS_GetPolyCamId(dBgS_l* bgs, s32 bg_index, s32 poly_index) {
    WWHD_FUNC(0x024EEC9C, s32, bgs, bg_index, poly_index);
    return dBgS_GetPolyId0(bgs, bg_index, poly_index, 0xFF, 0xFF, 0);
}
VERIFY(0x024EEC9C, dBgS_GetPolyCamId);

/* 024EECAC */
static u32 dBgS_GetMtrlSndId(dBgS_l* bgs, cBgS_PolyInfo_l* p) {
    WWHD_FUNC(0x024EECAC, u32, bgs, p);
    return dBgS_GetPolyId0(bgs, p->m_bg_index, p->m_poly_index, 0, 0x1F00, 8);
}
VERIFY(0x024EECAC, dBgS_GetMtrlSndId);

/* 024EECC8 */
static s32 dBgS_GetExitId(dBgS_l* bgs, cBgS_PolyInfo_l* p) {
    WWHD_FUNC(0x024EECC8, s32, bgs, p);
    return dBgS_GetPolyId0(bgs, p->m_bg_index, p->m_poly_index, 0x3F, 0x7E000, 13);
}
VERIFY(0x024EECC8, dBgS_GetExitId);

/* 024EECE8 */
static s32 dBgS_GetGrpRoomInfId(dBgS_l* bgs, cBgS_PolyInfo_l* p) {
    WWHD_FUNC(0x024EECE8, s32, bgs, p);
    u32 bg_index = p->m_bg_index;
    if (!(bg_index < 256)) JUT_ASSERT_fail(STR(0x10043400), 0x48F, STR(0x100433E0));
    if (!bgs->m_chk_element[bg_index].ChkUsed()) return 0xFF;
    dBgW_l* bgwp = elm_bgw(bgs, bg_index);
    s32 inf = bgwp->m_grp_room_inf;
    if (inf != 0xFF) return inf;
    s32 grp_id = cBgS_GetTriGrp(bgs, p->m_bg_index, p->m_poly_index);
    if (grp_id == -1) return 0xFF;
    return cBgS_GetGrpInf(bgs, p, grp_id) & 0xFF;
}
VERIFY(0x024EECE8, dBgS_GetGrpRoomInfId);

/* 024EEDB8 */
static s32 dBgS_GetGrpSoundId(dBgS_l* bgs, cBgS_PolyInfo_l* p) {
    WWHD_FUNC(0x024EEDB8, s32, bgs, p);
    s32 grp_id = cBgS_GetTriGrp(bgs, p->m_bg_index, p->m_poly_index);
    if (grp_id == -1) return -1;
    return (cBgS_GetGrpInf(bgs, p, grp_id) >> 11) & 0xFF;
}
VERIFY(0x024EEDB8, dBgS_GetGrpSoundId);

/* 024EEE30 */
static u32 dBgS_ChkGrpInf(dBgS_l* bgs, cBgS_PolyInfo_l* p, u32 mask) {
    WWHD_FUNC(0x024EEE30, u32, bgs, p, mask);
    s32 grp_id = cBgS_GetTriGrp(bgs, p->m_bg_index, p->m_poly_index);
    if (grp_id == -1) return 0;
    u32 inf = cBgS_GetGrpInf(bgs, p, grp_id);
    return inf & mask;
}
VERIFY(0x024EEE30, dBgS_ChkGrpInf);

/* 024EEEB8: HD: polygon colours only for stage type 1 (stage info of the stage data, virtual 0x15C) */
static s32 dBgS_GetPolyColor(dBgS_l* bgs, cBgS_PolyInfo_l* p) {
    WWHD_FUNC(0x024EEEB8, s32, bgs, p);
    u32 stageDt = dComIfGp_ea() + 0x5150;
    u32 info = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(stageDt) + 0x15C), stageDt); /* getStagInfo */
    if (((gabi::load<u32>(info + 0xC) >> 16) & 7) != 1) return 0xFF; /* dStage_stagInfo_GetSTType */
    if (p->m_poly_index == 0xFFFF || p->m_bg_index == 0x100) return 0xFF; /* !ChkSetInf */
    return dBgS_GetPolyId0(bgs, p->m_bg_index, p->m_poly_index, 0xFF, 0x07F80000, 19);
}
VERIFY(0x024EEEB8, dBgS_GetPolyColor);

/* 024EEF58 */
static s32 dBgS_GetPolyId1(dBgS_l* bgs, s32 bg_index, s32 poly_index, s32 defv, u32 mask, u32 shift) {
    WWHD_FUNC(0x024EEF58, s32, bgs, bg_index, poly_index, defv, mask, shift);
    return GetPolyIdN(bgs, bg_index, poly_index, defv, mask, shift, 1, 0x1004345C, 0x4FA, 0x1004340C,
                      {0x10043468, 0x1004342C, 0x2FD, 0x10043474});
}
VERIFY(0x024EEF58, dBgS_GetPolyId1);

/* 024EF064 */
static s32 dBgS_GetLinkNo(dBgS_l* bgs, cBgS_PolyInfo_l* p) {
    WWHD_FUNC(0x024EF064, s32, bgs, p);
    return dBgS_GetPolyId1(bgs, p->m_bg_index, p->m_poly_index, 0xFF, 0xFF, 0);
}
VERIFY(0x024EF064, dBgS_GetLinkNo);

/* 024EF080 */
static s32 dBgS_GetWallCode(dBgS_l* bgs, cBgS_PolyInfo_l* p) {
    WWHD_FUNC(0x024EF080, s32, bgs, p);
    return dBgS_GetPolyId1(bgs, p->m_bg_index, p->m_poly_index, 0, 0xF00, 8);
}
VERIFY(0x024EF080, dBgS_GetWallCode);

/* 024EF09C */
static s32 dBgS_GetSpecialCode(dBgS_l* bgs, cBgS_PolyInfo_l* p) {
    WWHD_FUNC(0x024EF09C, s32, bgs, p);
    return dBgS_GetPolyId1(bgs, p->m_bg_index, p->m_poly_index, 0, 0xF000, 12);
}
VERIFY(0x024EF09C, dBgS_GetSpecialCode);

/* 024EF0BC */
static s32 dBgS_GetGroundCode(dBgS_l* bgs, cBgS_PolyInfo_l* p) {
    WWHD_FUNC(0x024EF0BC, s32, bgs, p);
    return dBgS_GetPolyId1(bgs, p->m_bg_index, p->m_poly_index, 0, 0x03E00000, 21);
}
VERIFY(0x024EF0BC, dBgS_GetGroundCode);

/* 024EF0D8 */
static s32 dBgS_GetAttributeCodeDirect(dBgS_l* bgs, cBgS_PolyInfo_l* p) {
    WWHD_FUNC(0x024EF0D8, s32, bgs, p);
    return dBgS_GetPolyId1(bgs, p->m_bg_index, p->m_poly_index, 0, 0x001F0000, 16);
}
VERIFY(0x024EF0D8, dBgS_GetAttributeCodeDirect);

/* 024EF0F4: atr_conv at 101D51CC */
static s32 dBgS_GetAttributeCode(dBgS_l* bgs, cBgS_PolyInfo_l* p) {
    WWHD_FUNC(0x024EF0F4, s32, bgs, p);
    u32 attr = dBgS_GetAttributeCodeDirect(bgs, p);
    if (attr >= 0x20) return 0;
    return gabi::load<s32>(0x101D51CC + attr * 4);
}
VERIFY(0x024EF0F4, dBgS_GetAttributeCode);

/* 024EF130 */
static s32 dBgS_GetRoomId(dBgS_l* bgs, cBgS_PolyInfo_l* p) {
    WWHD_FUNC(0x024EF130, s32, bgs, p);
    if (p->m_poly_index == 0xFFFF) return -1; /* ChkSetInf */
    u32 id = p->m_bg_index;
    if (id == 0x100) return -1;
    if (!(id < 256)) JUT_ASSERT_fail(STR(0x100434AC), 0x601, STR(0x10043498));
    if (!cBgS_ChkPolySafe(bgs, p)) return -1;
    dBgW_l* bgwp = elm_bgw(bgs, id);
    u32 roomNo = bgwp->m_room_id;
    if (roomNo == 0xFFFF) {
        s32 grp = cBgS_GetTriGrp(bgs, p->m_bg_index, p->m_poly_index);
        roomNo = cBgS_GetGrpToRoomId(bgs, p->m_bg_index, grp);
        if (roomNo == 0xFFFF) return -1;
    }
    return roomNo;
}
VERIFY(0x024EF130, dBgS_GetRoomId);

/* 024EF218: GetPolyId2 (matcher: ChkPolyHSStick) */
static s32 dBgS_GetPolyId2(dBgS_l* bgs, s32 bg_index, s32 poly_index, s32 defv, u32 mask, u32 shift) {
    WWHD_FUNC(0x024EF218, s32, bgs, bg_index, poly_index, defv, mask, shift);
    return GetPolyIdN(bgs, bg_index, poly_index, defv, mask, shift, 2, 0x1004350C, 0x5A7, 0x100434BC,
                      {0x10043518, 0x100434DC, 0x302, 0x10043524});
}
VERIFY(0x024EF218, dBgS_GetPolyId2);

/* 024EF324: GetCamMoveBG (not named by the matcher) */
static s32 dBgS_GetCamMoveBG(dBgS_l* bgs, cBgS_PolyInfo_l* p) {
    WWHD_FUNC(0x024EF324, s32, bgs, p);
    return dBgS_GetPolyId2(bgs, p->m_bg_index, p->m_poly_index, 0xFF, 0xFF, 0);
}
VERIFY(0x024EF324, dBgS_GetCamMoveBG);

/* 024EF340: GetRoomCamId (not named by the matcher) */
static s32 dBgS_GetRoomCamId(dBgS_l* bgs, cBgS_PolyInfo_l* p) {
    WWHD_FUNC(0x024EF340, s32, bgs, p);
    return dBgS_GetPolyId2(bgs, p->m_bg_index, p->m_poly_index, 0xFF, 0xFF00, 8);
}
VERIFY(0x024EF340, dBgS_GetRoomCamId);

/* 024EF360 */
static s32 dBgS_GetRoomPathId(dBgS_l* bgs, cBgS_PolyInfo_l* p) {
    WWHD_FUNC(0x024EF360, s32, bgs, p);
    return dBgS_GetPolyId2(bgs, p->m_bg_index, p->m_poly_index, 0xFF, 0x00FF0000, 16);
}
VERIFY(0x024EF360, dBgS_GetRoomPathId);

/* 024EF37C */
static s32 dBgS_GetRoomPathPntNo_p(dBgS_l* bgs, cBgS_PolyInfo_l* p) {
    WWHD_FUNC(0x024EF37C, s32, bgs, p);
    return dBgS_GetPolyId2(bgs, p->m_bg_index, p->m_poly_index, 0xFF, 0xFF000000, 24);
}
VERIFY(0x024EF37C, dBgS_GetRoomPathPntNo_p);

/* 024EF398: ChkPolyHSStick (matcher: GetPolyId2); dBgW::GetPolyHSStick inlined (info3 bit 4) */
static u32 dBgS_ChkPolyHSStick(dBgS_l* bgs, cBgS_PolyInfo_l* p) {
    WWHD_FUNC(0x024EF398, u32, bgs, p);
    u32 bg_index = p->m_bg_index;
    if (!(bg_index < 256)) JUT_ASSERT_fail(STR(0x100435B0), 0x669, STR(0x10043560));
    if (!bgs->m_chk_element[bg_index].ChkUsed()) return FALSE;
    dBgW_l* bgwp = elm_bgw(bgs, bg_index);
    s32 poly = p->m_poly_index;
    if (!(poly < bgwp->pm_bgd->m_t_num)) JUT_ASSERT_fail(STR(0x100435BC), 0x2F1, STR(0x10043580));
    s32 id = gabi::load<u16>(bgwp->pm_bgd->m_t_tbl + poly * 0xA + 6);
    if (!(id < bgwp->pm_bgd->m_ti_num)) JUT_ASSERT_fail(STR(0x100435BC), 0x307, STR(0x100435C8));
    return gabi::load<u32>(bgwp->pm_bgd->m_ti_tbl + id * 0x10 + 0xC) & 0x10;
}
VERIFY(0x024EF398, dBgS_ChkPolyHSStick);

/* 024EF4A8 */
static bool dBgS_LineCrossNonMoveBG(dBgS_l* bgs, u8* chk) {
    WWHD_FUNC(0x024EF4A8, bool, bgs, chk);
    u32 c = gabi::ea(chk);
    bool ret = false;
    /* ClearPi */
    gabi::store<u32>(c + 0x1C, 0xFFFFFFFF);
    gabi::store<u32>(c + 0x18, 0);
    gabi::store<u16>(c + 0x16, 0x100);
    gabi::store<u16>(c + 0x14, 0xFFFF);
    gabi::store<u32>(c + 0x4C, gabi::load<u32>(c + 0x4C) & ~0x10u); /* ClrHit */
    for (s32 bg_index = 0; bg_index < 256; bg_index++) {
        cBgS_ChkElm_l* elm = &bgs->m_chk_element[bg_index];
        if (elm->ChkUsed() && elm->m_bgw_base_ptr->pm_vtx_tbl != 0 && !cBgS_Chk_ChkSameActorPid(c, elm->m_actor_id) &&
            !(elm->m_bgw_base_ptr->m_flags & 1)) {
            /* PreCalc */
            u32 f = gabi::load<u32>(c + 0x4C);
            gabi::store<u8>(c + 0x50, ((f >> 30) & 1) ^ 1);
            gabi::store<u8>(c + 0x51, (f >> 31) ^ 1);
            gabi::store<u8>(c + 0x52, ((f >> 29) & 1) ^ 1);
            dBgW_l* w = elm->m_bgw_base_ptr.get();
            if (cBgW_LineCheckGrpRp(w, c, w->m_rootGrpIdx, 1)) {
                /* SetActorInfo */
                gabi::store<u32>(c + 0x1C, elm->m_actor_id);
                gabi::store<u32>(c + 0x18, elm->m_bgw_base_ptr.v);
                ret = true;
                gabi::store<u16>(c + 0x16, (u16)bg_index);
            }
        }
    }
    if (ret) gabi::store<u32>(c + 0x4C, gabi::load<u32>(c + 0x4C) | 0x10); /* SetHit */
    return ret;
}
VERIFY(0x024EF4A8, dBgS_LineCrossNonMoveBG);

/* 024EF5CC */
static void dBgS_WallCorrect(dBgS_l* bgs, dBgS_Acch* acch) {
    WWHD_FUNC(0x024EF5CC, void, bgs, acch);
    u32 a = gabi::ea(acch);
    /* CalcWallRR */
    for (s32 i = 0; i < acch->m_tbl_size; i++) {
        dBgS_AcchCir& cir = acch->pm_acch_cir.get()[i];
        f32 r = cir.m_wall_r;
        cir.m_wall_rr = r * r;
    }
    dBgS_Acch_SetWallCir(a);
    cM3dGLin_SetStartEnd(a + 0x40, acch->pm_old_pos.v, acch->pm_pos.v); /* SetLin */
    dBgS_Acch_CalcWallBmdCyl(a);
    for (s32 prio = 0; prio < 3; prio++) {
        for (s32 bg_index = 0; bg_index < 256; bg_index++) {
            cBgS_ChkElm_l* elm = &bgs->m_chk_element[bg_index];
            if (elm->ChkUsed() && elm->m_bgw_base_ptr->pm_vtx_tbl != 0) {
                /* SetNowActorInfo */
                u32 w = elm->m_bgw_base_ptr.v;
                acch->field_0x78 = w;
                acch->m_ap_id = elm->m_actor_id;
                acch->m_bg_index = bg_index;
                if (!cBgS_Chk_ChkSameActorPid(a, elm->m_actor_id)) {
                    dBgW_l* bgwp = elm->m_bgw_base_ptr.get();
                    if ((u32)bgwp->m_priority == (u32)prio) /* ChkPriority */
                        dBgW_WallCorrectGrpRp(bgwp, a, bgwp->m_rootGrpIdx, 1);
                }
            }
        }
    }
}
VERIFY(0x024EF5CC, dBgS_WallCorrect);

/* 024EF6E8 */
static f32 dBgS_RoofChk(dBgS_l* bgs, u8* chk) {
    WWHD_FUNC(0x024EF6E8, f32, bgs, chk);
    u32 c = gabi::ea(chk);
    gabi::store<u32>(c + 0x4, 0);
    gabi::store<f32>(c + 0x48, 1000000000.0f); /* SetNowY(G_CM3D_F_INF) */
    gabi::store<u32>(c + 0x8, 0xFFFFFFFF);
    gabi::store<u16>(c + 0x0, 0xFFFF);
    gabi::store<u16>(c + 0x2, 0x100);
    for (s32 bg_index = 0; bg_index < 256; bg_index++) {
        cBgS_ChkElm_l* elm = &bgs->m_chk_element[bg_index];
        if (elm->ChkUsed() && elm->m_bgw_base_ptr->pm_vtx_tbl != 0 && !cBgS_Chk_ChkSameActorPid(c + 0x10, elm->m_actor_id)) {
            dBgW_l* bgwp = elm->m_bgw_base_ptr.get();
            if (dBgW_RoofChkGrpRp(bgwp, c, bgwp->m_rootGrpIdx, 1)) {
                gabi::store<u32>(c + 0x8, elm->m_actor_id);
                gabi::store<u32>(c + 0x4, elm->m_bgw_base_ptr.v);
                gabi::store<u16>(c + 0x2, (u16)bg_index);
            }
        }
    }
    return gabi::load<f32>(c + 0x48);
}
VERIFY(0x024EF6E8, dBgS_RoofChk);

/* 024EF7C0 */
static bool dBgS_SplGrpChk(dBgS_l* bgs, u8* chk) {
    WWHD_FUNC(0x024EF7C0, bool, bgs, chk);
    u32 c = gabi::ea(chk);
    bool ret = false;
    dBgS_SplGrpChk_Init(c);
    for (s32 bg_index = 0; bg_index < 256; bg_index++) {
        cBgS_ChkElm_l* elm = &bgs->m_chk_element[bg_index];
        if (elm->ChkUsed() && elm->m_bgw_base_ptr->pm_vtx_tbl != 0 && !cBgS_Chk_ChkSameActorPid(c + 0x10, elm->m_actor_id)) {
            dBgW_l* bgwp = elm->m_bgw_base_ptr.get();
            if (dBgW_SplGrpChkGrpRp(bgwp, c, bgwp->m_rootGrpIdx, 1)) {
                ret = true;
                gabi::store<u32>(c + 0x8, elm->m_actor_id);
                gabi::store<u32>(c + 0x4, elm->m_bgw_base_ptr.v);
                gabi::store<u16>(c + 0x2, (u16)bg_index);
                gabi::store<u32>(c + 0x4C, gabi::load<u32>(c + 0x4C) | 1); /* OnFind */
            }
        }
    }
    return ret;
}
VERIFY(0x024EF7C0, dBgS_SplGrpChk);

/* 024EF88C */
static bool dBgS_SphChk(dBgS_l* bgs, u8* chk, void* user) {
    WWHD_FUNC(0x024EF88C, bool, bgs, chk, user);
    u32 c = gabi::ea(chk);
    bool ret = false;
    /* Init: ClearPi */
    gabi::store<u16>(c + 0x14, 0xFFFF);
    gabi::store<u16>(c + 0x16, 0x100);
    gabi::store<u32>(c + 0x18, 0);
    gabi::store<u32>(c + 0x1C, 0xFFFFFFFF);
    for (s32 bg_index = 0; bg_index < 256; bg_index++) {
        cBgS_ChkElm_l* elm = &bgs->m_chk_element[bg_index];
        if (elm->ChkUsed() && elm->m_bgw_base_ptr->pm_vtx_tbl != 0 && !cBgS_Chk_ChkSameActorPid(c + 0x24, elm->m_actor_id)) {
            dBgW_l* bgwp = elm->m_bgw_base_ptr.get();
            if (dBgW_SphChkGrpRp(bgwp, c, user, bgwp->m_rootGrpIdx, 1)) {
                gabi::store<u32>(c + 0x1C, elm->m_actor_id);
                gabi::store<u32>(c + 0x18, elm->m_bgw_base_ptr.v);
                ret = true;
                gabi::store<u16>(c + 0x16, (u16)bg_index);
            }
        }
    }
    return ret;
}
VERIFY(0x024EF88C, dBgS_SphChk);

/* MoveBgCrrPos / MoveBgTransPos / MoveBgMatrixCrrPos: virtual dBgW::CrrPos 0x44 / TransPos 0x4C /
 * MatrixCrrPos 0x54 */
static inline void MoveBgProc(dBgS_l* bgs, cBgS_PolyInfo_l* polyInfo, u32 accept, cXyz* pos, csXyz* angle,
                              csXyz* shape_angle, u32 file, u32 line, u32 msg, u32 slot, bool polySafe) {
    if (accept == 0) return;
    u32 bg_index = polyInfo->m_bg_index;
    if (bg_index == 0x100) return; /* !ChkBgIndex */
    if (!(bg_index < 256)) JUT_ASSERT_fail(STR(file), line, STR(msg));
    cBgS_ChkElm_l* elm = &bgs->m_chk_element[bg_index];
    if (elm->ChkUsed()) {
        dBgW_l* bgwp = elm->m_bgw_base_ptr.get();
        if ((bgwp->m_move_flag & 1) && (!polySafe || cBgS_ChkPolySafe(bgs, polyInfo))) {
            u32 fn = gabi::load<u32>(bgwp->__vtbl + slot);
            gabi::call_ptr(fn, bgwp, polyInfo, (u32)elm->m_actor_ptr, accept, pos, angle, shape_angle);
        }
    }
}

/* 024EF968 */
static void dBgS_MoveBgCrrPos(dBgS_l* bgs, cBgS_PolyInfo_l* polyInfo, u32 accept, cXyz* pos, csXyz* angle,
                              csXyz* shape_angle) {
    WWHD_FUNC(0x024EF968, void, bgs, polyInfo, accept, pos, angle, shape_angle);
    MoveBgProc(bgs, polyInfo, accept, pos, angle, shape_angle, 0x1004360C, 0x85D, 0x100435EC, 0x44, true);
}
VERIFY(0x024EF968, dBgS_MoveBgCrrPos);

/* 024EFA38 */
static void dBgS_MoveBgTransPos(dBgS_l* bgs, cBgS_PolyInfo_l* polyInfo, u32 accept, cXyz* pos, csXyz* angle,
                                csXyz* shape_angle) {
    WWHD_FUNC(0x024EFA38, void, bgs, polyInfo, accept, pos, angle, shape_angle);
    MoveBgProc(bgs, polyInfo, accept, pos, angle, shape_angle, 0x10043638, 0x88A, 0x10043618, 0x4C, true);
}
VERIFY(0x024EFA38, dBgS_MoveBgTransPos);

/* 024EFB08 */
static void dBgS_MoveBgMatrixCrrPos(dBgS_l* bgs, cBgS_PolyInfo_l* polyInfo, u32 accept, cXyz* pos, csXyz* angle,
                                    csXyz* shape_angle) {
    WWHD_FUNC(0x024EFB08, void, bgs, polyInfo, accept, pos, angle, shape_angle);
    MoveBgProc(bgs, polyInfo, accept, pos, angle, shape_angle, 0x10043664, 0x8B5, 0x10043644, 0x54, false);
}
VERIFY(0x024EFB08, dBgS_MoveBgMatrixCrrPos);

/* 024EFBC0 */
static void dBgS_RideCallBack(dBgS_l* bgs, cBgS_PolyInfo_l* polyInfo, fopAc_ac_c* ac) {
    WWHD_FUNC(0x024EFBC0, void, bgs, polyInfo, ac);
    u32 bg_index = polyInfo->m_bg_index;
    if (!(bg_index < 256)) JUT_ASSERT_fail(STR(0x10043690), 0x967, STR(0x10043670));
    dBgW_l* bgwp = elm_bgw(bgs, bg_index);
    if (bgwp->m_id < 0x100 && bgwp->m_ride_callback != 0)
        gabi::call_ptr(bgwp->m_ride_callback, bgwp, (u32)bgs->m_chk_element[bg_index].m_actor_ptr, ac);
}
VERIFY(0x024EFBC0, dBgS_RideCallBack);

/* 024EFC54 */
static u32 dBgS_PushPullCallBack(dBgS_l* bgs, cBgS_PolyInfo_l* polyInfo, fopAc_ac_c* ac, u32 inf, u32 lbl) {
    WWHD_FUNC(0x024EFC54, u32, bgs, polyInfo, ac, inf, lbl);
    u32 bg_index = polyInfo->m_bg_index;
    if (!(bg_index < 256)) JUT_ASSERT_fail(STR(0x100436BC), 0x988, STR(0x1004369C));
    dBgW_l* bgwp = elm_bgw(bgs, bg_index);
    if (!(bgwp->m_id < 0x100)) return 0;
    u32 actor = bgs->m_chk_element[bg_index].m_actor_ptr;
    if (actor == 0) return 0;
    u32 cb = bgwp->m_push_pull_callback;
    if (cb == 0) return 0;
    return gabi::call_ptr<u32>(cb, actor, ac, inf, lbl);
}
VERIFY(0x024EFC54, dBgS_PushPullCallBack);

}  // namespace d_bg_s_cpp
