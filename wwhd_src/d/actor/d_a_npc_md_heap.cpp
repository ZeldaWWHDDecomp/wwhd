/**
 * d_a_npc_md_heap.cpp (WWHD)
 * Player - Medli: callbacks, node callbacks, heap, init, create (02283480..02286B58)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_md.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_md.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
#define MD_SAFESTRING_VTBL 0x1001D6A0 /* this TU's sead::SafeString vtable */
#define MD_M_SEATALK 0x101D5F40       /* bool daNpc_Md_c::m_seaTalk */
static inline dSv_event_c* md_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL md_isEventBit(u16 f) { return dSv_event_isEventBit(md_event(), f); }
static inline void md_onEventBit(u16 f) { dSv_event_onEventBit(md_event(), f); }
static inline u8 md_getSelectItem(int btn) { return gabi::load<u8>(dComIfGp_ea() + btn + 0x5BBB); }
/* 02606900 HD: dRes_control_c::getRes(const SafeString& arc, const SafeString& name) */
static inline void* md_getObjectRes(const char* arc, const char* name) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> n;
    a->mStringTop = gabi::ea(arc);
    a->__vtbl = MD_SAFESTRING_VTBL;
    n->mStringTop = gabi::ea(name);
    n->__vtbl = MD_SAFESTRING_VTBL;
    return gabi::call<void*>(0x02606900, dComIfG_resControl(), a.get(), n.get());
}
/* the same with the two SafeStrings at given stack addresses */
static inline void* md_getObjectRes_at(u32 a, u32 n, const char* arc, u32 name) {
    gabi::store<u32>(a, gabi::ea(arc));
    gabi::store<u32>(a + 4, MD_SAFESTRING_VTBL);
    gabi::store<u32>(n, name);
    gabi::store<u32>(n + 4, MD_SAFESTRING_VTBL);
    return gabi::call<void*>(0x02606900, dComIfG_resControl(), a, n);
}
static inline J3DModelData* md_getModelData(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
static inline J3DModel* md_morfModel(void* morf) { return gabi::at<J3DModel>(gabi::load<u32>(gabi::ea(morf) + 0x90)); }
static inline s32 J3DAnm_getFrameMax(void* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
/* HD J3D (as d_a_npc_ko1): j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at 0x104B4868; a
 * model's joint matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices) */
static inline Mtx34* md_getAnmMtx(J3DModel* model, u32 jntNo) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10)); /* HD: marks the joint matrices dirty */
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jntNo * 0x30);
}
static inline u32 md_jntNo(J3DNode* node) { return gabi::load<u16>(gabi::ea(J3DNode_toJoint(node)) + 4); }
static inline Mtx34* md_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
static inline J3DModel* md_j3dSysModel() { return gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); }
static inline void PSMTXMultVecArray(const Mtx34* m, const cXyz* src, cXyz* dst, u32 n) { gabi::call(0x028E8FB8, m, src, dst, n); }
/* c_xyz / c_m3d (HD: the cXyz results go through a hidden pointer in r4) */
static inline void cXyz_normZP(const cXyz* a, cXyz* out) { gabi::call(0x0201B12C, a, out); }
static inline void cXyz_norm(const cXyz* a, cXyz* out) { gabi::call(0x0201B084, a, out); }
static inline void cXyz_outprod(const cXyz* a, cXyz* out, const cXyz* b) { gabi::call(0x0201B080, a, out, b); }
/* 0201B3C0 cXyz::normalizeZP: normalises in place; HD passes a result slot in r4 */
static inline void cXyz_normalizeZP(cXyz* a, cXyz* out) { gabi::call(0x0201B3C0, a, out); }
static inline void dKyw_get_AllWind_vec(cXyz* pos, cXyz* dir, be<f32>* pow) { gabi::call(0x0257E1B8, pos, dir, pow); }
static inline void PSMTXRotAxisRad(Mtx34* m, const cXyz* axis, f32 rad) { gabi::call(0x028E9838, m, axis, rad); }
static inline void PSMTXConcat(const Mtx34* a, const Mtx34* b, Mtx34* ab) { gabi::call(0x028E9108, a, b, ab); }
static inline f32 PSVECDotProduct_l(const cXyz* a, const cXyz* b) { return gabi::call<f32>(0x028E8F44, a, b); }
static inline bool cM3d_UpMtx_Base(const cXyz* a, const cXyz* b, Mtx34* m) { return gabi::call<bool>(0x02017374, a, b, m); }
/* cM3dGTri (HD 0x38: the plane normal first) and cM3dGLin (0x1C) as stack objects */
struct cM3dGTri_l {
    /* 0x00 */ cXyz mNormal;
    /* 0x0C */ u8 _0C[0x38 - 0xC];
};
struct cM3dGLin_l {
    u8 _00[0x1C];
};
static inline void cM3dGTri_ct(cM3dGTri_l* t, const cXyz* a, const cXyz* b, const cXyz* c) { gabi::call(0x020190B8, t, a, b, c); }
static inline void cM3dGLin_ct(cM3dGLin_l* l, const cXyz* a, const cXyz* b) { gabi::call(0x02018780, l, a, b); }
static inline bool cM3d_Cross_LinTri(const cM3dGLin_l* l, const cM3dGTri_l* t, cXyz* out, bool a, bool b) {
    return gabi::call<bool>(0x02012AE8, l, t, out, a, b);
}
static inline void cM3dGPla_Up(cM3dGTri_l* t, f32 d) { gabi::call(0x020191B4, t, d); } /* 020191B4 (cM3dGTri::Up, unnamed) */
static inline f32 cM3d_SignedLenPlaAndPos(const cM3dGTri_l* t, const cXyz* p) { return gabi::call<f32>(0x02010C50, t, p); }
static inline f32 md_abs(const cXyz* v) { return std_sqrtf(PSVECSquareMag(v)); }
static inline void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F19F8, m, x, y, z); }
static inline void McaMorf2_calc(mDoExt_McaMorf2* m) { gabi::call(0x025E66E4, m); } /* 025E66E4 mDoExt_McaMorf2::calc */
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s32 JUTNameTab_getIndex(u32 tab, u32 name) { return gabi::call<s32>(0x027DF9B0, tab, name); }
/* HD: J3DModelData::getJointNodePointer(idx) is a bounds-checked buffer access (count +4, array +8,
 * 0x1C-byte entries; out of range: the first entry); J3DJoint::setCallBack stores at +8 */
static inline void md_setJointCallBack(J3DModelData* d, u16 idx, u32 cb) {
    u32 count = gabi::load<u32>(gabi::ea(d) + 4);
    u32 p = gabi::load<u32>(gabi::ea(d) + 8);
    if (idx < count) {
        p += idx * 0x1C;
    }
    gabi::store<u32>(p + 8, cb);
}
static inline void md_copyName(u32 dst, u32 src, u32 n) {
    for (u32 i = 0; i < n; i++) {
        gabi::store<u8>(dst + i, gabi::load<u8>(src + i));
    }
}
static inline s32 md_btpAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline s32 md_btkAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E7CE0, anm, d, p, play, attr, rate, start, end, modify, entry);
}

/* 02283480 */
s16 daNpc_Md_c::XyCheckCB(int i_itemBtn) {
    WWHD_FUNC(0x02283480, s16, this, i_itemBtn);
    u8 selectItemNo = md_getSelectItem(i_itemBtn);
    if (mType == 3) { /* isTypeSea() */
        if (selectItemNo == 0x22 /* dItemNo_WIND_WAKER_e */) {
            return TRUE;
        }
        if (selectItemNo == 0x47 /* dItemNo_GOLDEN_FEATHER_e */) {
            if (!md_isEventBit(0x2E40) || (md_isEventBit(0x2E40) && gabi::load<u8>(MD_M_SEATALK) != 0)) {
                return TRUE;
            }
        }
    } else if (mType == 5 || mType == 4) { /* isTypeM_Dai() || isTypeEdaichi() */
        if (selectItemNo == 0x47 && !md_isEventBit(0x3B80)) {
            return TRUE;
        }
    }
    if (selectItemNo == 0x47) {
        md_onEventBit(0x2C08);
    }
    return FALSE;
}
VERIFY(0x02283480, &daNpc_Md_c::XyCheckCB);

/* 02283584 daNpc_Md_XyCheckCB (not named by the matcher) */
static s16 daNpc_Md_XyCheckCB(void* i_this, int param_1) {
    WWHD_FUNC(0x02283584, s16, i_this, param_1);
    return static_cast<daNpc_Md_c*>(i_this)->XyCheckCB(param_1);
}
VERIFY(0x02283584, daNpc_Md_XyCheckCB);

/* 02283588 */
s16 daNpc_Md_c::XyEventCB(int i_itemBtn) {
    WWHD_FUNC(0x02283588, s16, this, i_itemBtn);
    u8 selectItemNo = md_getSelectItem(i_itemBtn);
    if (selectItemNo == 0x22) {
        clearStatus(daMdStts_DEFAULT_TALK_XY);
        return mEventIdxTable[5];
    } else if (selectItemNo == 0x47) {
        setBitStatus(daMdStts_DEFAULT_TALK_XY);
    }
    return -1;
}
VERIFY(0x02283588, &daNpc_Md_c::XyEventCB);

/* 02283610 daNpc_Md_XyEventCB (not named by the matcher) */
static s16 daNpc_Md_XyEventCB(void* i_this, int param_1) {
    WWHD_FUNC(0x02283610, s16, i_this, param_1);
    return static_cast<daNpc_Md_c*>(i_this)->XyEventCB(param_1);
}
VERIFY(0x02283610, daNpc_Md_XyEventCB);

/* 02283614 */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x02283614, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DModel* model = md_j3dSysModel();
        daNpc_Md_c* i_this = gabi::at<daNpc_Md_c>(gabi::load<u32>(gabi::ea(model) + 0xB8)); /* getUserArea() */
        if (i_this != nullptr) {
            /* static cXyz l_offsetAttPos(0, 0, 0) (guard 0x10467C5C) / l_offsetEyePos(15, 0, 0) (guard 0x10467C60) */
            cXyz* l_offsetAttPos = gabi::at<cXyz>(0x10467A6C);
            if (gabi::load<u32>(0x10467C5C) == 0) {
                l_offsetAttPos->x = 0.0f;
                l_offsetAttPos->z = 0.0f;
                gabi::store<u32>(0x10467C5C, 1);
                l_offsetAttPos->y = 0.0f;
            }
            cXyz* l_offsetEyePos = gabi::at<cXyz>(0x10467A78);
            if (gabi::load<u32>(0x10467C60) == 0) {
                l_offsetEyePos->y = 0.0f;
                gabi::store<u32>(0x10467C60, 1);
                l_offsetEyePos->x = 15.0f;
                l_offsetEyePos->z = 0.0f;
            }
            u32 jntNo = md_jntNo(node);
            Mtx34* stk = mDoMtx_stack_c::get();
            PSMTXCopy(md_getAnmMtx(model, jntNo), stk);
            PSMTXMultVec(stk, l_offsetAttPos, &i_this->m3094);
            mDoMtx_XrotM(stk, i_this->mJntCtrl.mAngles[0][1]);
            mDoMtx_ZrotM(stk, -i_this->mJntCtrl.mAngles[0][0]);
            PSMTXMultVec(stk, l_offsetEyePos, &i_this->m3088);
            if (i_this->m312B != 0xFF) { /* incAttnSetCount() */
                i_this->m312B = i_this->m312B + 1;
            }
            PSMTXCopy(stk, md_mCurrentMtx());
            mtx_copy(md_getAnmMtx(model, jntNo), stk);
        }
    }
    return TRUE;
}
VERIFY(0x02283614, nodeCallBack);

/* 022837D0 */
static BOOL waistNodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x022837D0, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DModel* model = md_j3dSysModel();
        daNpc_Md_c* i_this = gabi::at<daNpc_Md_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        if (i_this != nullptr) {
            u32 jntNo = md_jntNo(node);
            md_getAnmMtx(model, jntNo); /* HD: the dirty flag is set twice */
            Mtx34* r3 = md_getAnmMtx(model, jntNo);
            PSMTXMultVecArray(r3, gabi::at<cXyz>(0x101C02AC) /* waistVecDat */, i_this->m3234, 4);
            Mtx34* stk = mDoMtx_stack_c::get();
            PSMTXCopy(md_getAnmMtx(model, jntNo), stk);
            if (i_this->checkStatus(daNpc_Md_c::daMdStts_UNK4000)) {
                mDoMtx_XrotM(stk, i_this->m3116);
                mDoMtx_ZrotM(stk, i_this->m3114);
            } else {
                mDoMtx_XrotM(stk, i_this->mJntCtrl.mAngles[1][1]);
                mDoMtx_ZrotM(stk, -i_this->mJntCtrl.mAngles[1][0]);
            }
            PSMTXCopy(stk, md_mCurrentMtx());
            mtx_copy(md_getAnmMtx(model, jntNo), stk);
        }
    }
    return TRUE;
}
VERIFY(0x022837D0, waistNodeCallBack);

/* 022839F0 */
static BOOL hairTopNodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x022839F0, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DModel* model = md_j3dSysModel();
        daNpc_Md_c* i_this = gabi::at<daNpc_Md_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        if (i_this != nullptr) {
            u32 jntNo = md_jntNo(node);
            Mtx34* mtx = md_getAnmMtx(model, jntNo);
            cXyz* hairTopPos = &i_this->m3174[0];
            f32 x = mtx->m[0][3];
            f32 y = mtx->m[1][3];
            f32 z = mtx->m[2][3];
            hairTopPos->y = y;
            hairTopPos->x = x;
            hairTopPos->z = z;
        }
    }
    return TRUE;
}
VERIFY(0x022839F0, hairTopNodeCallBack);

/* hairCross (inlined in hairNodeCallBack): the part after the first cross test */
static BOOL hairCross_hit(cM3dGTri_l* tri, cM3dGLin_l* lin, cXyz* cross, cXyz* i_r4, cXyz* i_r5) {
    cM3dGPla_Up(tri, 2.0f);
    if (!cM3d_Cross_LinTri(lin, tri, cross, true, false)) {
        return FALSE;
    }
    f32 f31 = cM3d_SignedLenPlaAndPos(tri, i_r4);
    gabi::Local<cXyz> r1_12c;
    f32 nz = tri->mNormal.z;
    f32 nx = tri->mNormal.x;
    f32 ny = tri->mNormal.y;
    r1_12c->z = nz;
    r1_12c->x = nx;
    r1_12c->y = ny;
    gabi::Local<cXyz> t38;
    cXyz_ml(r1_12c, t38, -f31);
    gabi::Local<cXyz> r1_120;
    cXyz_pl(i_r4, r1_120, t38);
    gabi::Local<cXyz> r1_e4;
    cXyz_mi(i_r4, r1_e4, i_r5);
    f32 f30 = md_abs(r1_e4);
    gabi::Local<cXyz> r1_108;
    cXyz_mi(cross, r1_108, r1_120);
    cXyz_normalizeZP(r1_108, t38);
    cXyz_ml(r1_108, t38, std_sqrtf(gabi::fmsubs(f30, f30, f31 * f31)));
    r1_108->copy(*t38);
    cXyz_pl(r1_120, t38, r1_108);
    i_r5->copy(*t38);
    return TRUE;
}
static BOOL hairCross(cXyz* i_r3, cXyz* i_r4, cXyz* i_r5) {
    gabi::Local<cM3dGTri_l> r1_198;
    cM3dGTri_ct(r1_198, &i_r3[0], &i_r3[1], &i_r3[2]);
    gabi::Local<cM3dGTri_l> r1_160;
    cM3dGTri_ct(r1_160, &i_r3[1], &i_r3[3], &i_r3[2]);
    gabi::Local<cM3dGLin_l> r1_144;
    cM3dGLin_ct(r1_144, i_r4, i_r5);
    gabi::Local<cXyz> r1_138;
    if (cM3d_Cross_LinTri(r1_144, r1_198, r1_138, true, false)) {
        return hairCross_hit(r1_198, r1_144, r1_138, i_r4, i_r5);
    } else if (cM3d_Cross_LinTri(r1_144, r1_160, r1_138, true, false)) {
        return hairCross_hit(r1_160, r1_144, r1_138, i_r4, i_r5);
    }
    return FALSE;
}

/* vecChange (inlined in hairNodeCallBack) */
static void vecChange(cXyz* i_r3, cXyz* i_r4, s16 i_r5) {
    if (!(std::fabs(md_abs(i_r3)) < 3.814697265625e-06f)) { /* !cM3d_IsZero */
        if (!(std::fabs(md_abs(i_r4)) < 3.814697265625e-06f)) {
            gabi::Local<cXyz> r1_4c;
            cXyz_norm(i_r3, r1_4c);
            gabi::Local<cXyz> r1_40;
            cXyz_norm(i_r4, r1_40);
            f32 dot = PSVECDotProduct_l(r1_4c, r1_40);
            if (dot < cM_scos(i_r5)) {
                gabi::Local<cXyz> r1_34;
                cXyz_outprod(r1_4c, r1_34, r1_40);
                PSMTXRotAxisRad(mDoMtx_stack_c::get(), r1_34, 0.39269909262657166f /* cM_s2rad(0x1000) */);
                PSMTXMultVec(mDoMtx_stack_c::get(), r1_4c, i_r4);
            } else {
                i_r4->copy(*r1_40);
            }
        }
    }
}

/* 02283A78 */
static BOOL hairNodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x02283A78, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DModel* model = md_j3dSysModel();
        daNpc_Md_c* i_this = gabi::at<daNpc_Md_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        if (i_this != nullptr) {
            u32 jntNo = md_jntNo(node);
            for (int i = 1; i < 8; i++) {
                if ((u32)(s32)i_this->m_hair_jnt_nums[i] != jntNo) {
                    continue;
                }

                Mtx34* mtx = md_getAnmMtx(model, jntNo);
                u8 mask = gabi::load<u8>(0x101C0270 + i); /* HairModeMaskData[i] */
                if (!(u8)(i_this->m3134 & mask)) {
                    i_this->m3134 = i_this->m3134 | mask;
                    cXyz* hairPos = &i_this->m3174[i];
                    f32 y = mtx->m[1][3];
                    f32 x = mtx->m[0][3];
                    f32 z = mtx->m[2][3];
                    hairPos->y = y;
                    hairPos->x = x;
                    hairPos->z = z;
                    gabi::Local<cXyz> tmp;
                    cXyz_mi(hairPos, tmp, &i_this->m3174[i - 1]);
                    cXyz* r29 = &i_this->m31D4[i];
                    r29->copy(*tmp);
                    i_this->m3264[i] = md_abs(r29);
                    break;
                }

                cXyz* r25 = &i_this->m3174[i - 1];
                cXyz* r28 = &i_this->m3174[i];
                cXyz* r24 = &i_this->m31D4[i];
                if (i <= 1) {
                    r28->x = mtx->m[0][3];
                    r28->y = mtx->m[1][3];
                    r28->z = mtx->m[2][3];
                    gabi::Local<cXyz> tmp;
                    cXyz_mi(r28, tmp, r25);
                    r24->copy(*tmp);
                    break;
                }

                be<f32>* hairDist = &i_this->m3264[i];
                cXyz* r22 = &i_this->m31D4[i - 1];
                gabi::Local<cXyz> r1_d0;
                r1_d0->x = r28->x;
                r1_d0->y = r28->y;
                r1_d0->z = r28->z;
                gabi::Local<cXyz> r1_c4;
                cXyz_mi(r1_d0, r1_c4, r25);
                gabi::Local<cXyz> r1_e8;
                r1_e8->x = 0.0f;
                r1_e8->z = 0.0f;
                r1_e8->y = -1.0f;
                PSVECScale(r1_e8, r1_e8, l_HIO().m14C);
                gabi::Local<cXyz> r1_dc;
                gabi::Local<be<f32>> power;
                dKyw_get_AllWind_vec(&i_this->current.pos, r1_dc, power);
                gabi::Local<cXyz> t20;
                cXyz_ml(r1_dc, t20, *power);
                gabi::Local<cXyz> t84;
                cXyz_ml(t20, t84, 3.0f);
                PSVECAdd(r1_e8, t84, r1_e8);
                PSVECAdd(r1_c4, r1_e8, r1_c4);
                cXyz_normZP(r1_c4, t84);
                cXyz_ml(t84, t20, *hairDist);
                r1_c4->copy(*t20);
                vecChange(r22, r1_c4, 0x1000);
                cXyz_ml(r1_c4, t20, *hairDist);
                r24->copy(*t20);
                cXyz_pl(r24, t20, r25);
                r28->copy(*t20);
                if (hairCross(i_this->m3234, &i_this->m3174[1], r28)) {
                    gabi::Local<cXyz> t78;
                    cXyz_mi(r28, t78, r25);
                    r24->copy(*t78);
                    cXyz_normalizeZP(r24, t78);
                    r1_c4->copy(*r24);
                    gabi::Local<cXyz> t148;
                    cXyz_ml(r1_c4, t148, *hairDist);
                    cXyz_pl(t148, t78, r25);
                    r28->copy(*t78);
                }

                s32 prevHairJntNo = i_this->m_hair_jnt_nums[i - 1];
                gabi::Local<cXyz> r1_b8;
                cXyz_normZP(r22, r1_b8);
                gabi::Local<Mtx34> r1_124;
                cM3d_UpMtx_Base(r1_b8, r1_c4, r1_124);
                gabi::Local<Mtx34> r1_f4;
                PSMTXCopy(md_getAnmMtx(model, prevHairJntNo), r1_f4);
                r1_f4->m[1][3] = 0.0f;
                r1_f4->m[0][3] = 0.0f;
                r1_f4->m[2][3] = 0.0f;
                PSMTXCopy(r1_f4, mDoMtx_stack_c::get());
                PSMTXConcat(r1_124, mDoMtx_stack_c::get(), mDoMtx_stack_c::get()); /* revConcat */
                PSMTXCopy(mDoMtx_stack_c::get(), mtx);
                mtx->m[0][3] = r28->x;
                mtx->m[1][3] = r28->y;
                mtx->m[2][3] = r28->z;
                PSMTXCopy(mtx, md_mCurrentMtx());
                break;
            }
        }
    }
    return TRUE;
}
VERIFY(0x02283A78, hairNodeCallBack);

/* 02284418 armNodeCallBack (not named by the matcher): copies the arm/wing joint matrices of the
 * body model to the arm model */
static BOOL armNodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x02284418, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DModel* armModel = md_j3dSysModel();
        daNpc_Md_c* i_this = gabi::at<daNpc_Md_c>(gabi::load<u32>(gabi::ea(armModel) + 0xB8));
        if (i_this != nullptr) {
            u32 armJntNo = md_jntNo(node);
            s32 bodyJntNo;
            J3DModel* bodyModel = md_morfModel(i_this->mpMorf.get());
            /* HD: the GameCube "else if (armJntNo == ArmLlocJntNum)" is a plain else */
            if (armJntNo == (u32)(s32)i_this->m_armRloc_jnt_num) {
                bodyJntNo = i_this->m_armR_jnt_num;
            } else {
                bodyJntNo = i_this->m_armL_jnt_num;
            }
            PSMTXCopy(md_getAnmMtx(bodyModel, bodyJntNo), md_mCurrentMtx());
            Mtx34* src = md_getAnmMtx(bodyModel, bodyJntNo);
            mtx_copy(md_getAnmMtx(armModel, armJntNo), src);
        }
    }
    return TRUE;
}
VERIFY(0x02284418, armNodeCallBack);

/* 0228461C */
BOOL daNpc_Md_c::initLightBtkAnm(u32 param_1) {
    WWHD_FUNC(0x0228461C, BOOL, this, param_1);
    J3DModelData* pModelData = md_getModelData(mpHarpLightModel.get());
    bool ret = false;
    void* a_lightBtk = md_getObjectRes(mModelArcName, STR(0x1001D91C) /* "ymdhp00.btk" */);
    if (a_lightBtk == nullptr) {
        JUT_ASSERT_fail(STR(0x1001D93C), 0x1EA9, STR(0x1001D928));
    }
    if (md_btkAnm_init(&mLightBtkAnm, pModelData, a_lightBtk, 1, 2 /* EMode_LOOP */, 1.0f, 0, -1, param_1, 0)) {
        ret = true;
    }
    return ret;
}
VERIFY(0x0228461C, &daNpc_Md_c::initLightBtkAnm);

/* 022846EC */
BOOL daNpc_Md_c::initTexPatternAnm(u8 btpAnmTblIdx, u32 param_2) {
    WWHD_FUNC(0x022846EC, BOOL, this, btpAnmTblIdx, param_2);
    u32 tbl = 0x101C05EC + btpAnmTblIdx * 0x21; /* static btpAnmTbl[] {char name[0x20]; u8 m20;} */
    J3DModelData* modelData = md_getModelData(md_morfModel(mpMorf.get()));
    bool ret = false;
    void* eyeTexPtrn = md_getObjectRes(mModelArcName, gabi::at<const char>(tbl));
    if (eyeTexPtrn == nullptr) {
        JUT_ASSERT_fail(STR(0x1001D960), 0x1E70, STR(0x1001D94C));
    }
    if (md_btpAnm_init(m0520, modelData, eyeTexPtrn, 1, 1 /* EMode_RESET */, 1.0f, 0, -1, param_2, 0)) {
        m3112 = (s16)J3DAnm_getFrameMax(eyeTexPtrn);
        m3133 = 0;
        m3136 = gabi::load<u8>(tbl + 0x20);
        m3137 = btpAnmTblIdx;
        ret = true;
    }
    return ret;
}
VERIFY(0x022846EC, &daNpc_Md_c::initTexPatternAnm);

#define MD_HEAP_FILE 0x1001DB40 /* JUT_ASSERT file name */
/* joint index of a name, with the GameCube assertion (index >= 0) */
static inline s8 md_jntIndex(J3DModelData* md, u32 name, u32 line, u32 msg, be<s8>* dst) {
    s8 idx = (s8)JUTNameTab_getIndex(J3DModelData_getJointName(md), name);
    *dst = idx;
    if (idx < 0) {
        JUT_ASSERT_fail(STR(MD_HEAP_FILE), line, gabi::at<const char>(msg));
    }
    return idx;
}

/* 022847F8 */
BOOL daNpc_Md_c::createHeap() {
    WWHD_FUNC(0x022847F8, BOOL, this);
    /* The name SafeStrings of the two wait animations point into the stack name buffers; the
     * harness compares such pointers relative to the SafeString, so the buffers and their
     * SafeStrings share one stack object laid out as GHS's frame (buffers 0x68 above the name) */
    gabi::Local<u8[0xD0]> frm;
    u32 sp = gabi::ea(frm.get());
    u32 wait_anim_name = sp + 0x9C;
    u32 arm_wait_anim_name = sp + 0xBC;
    if (isTypeShipRide()) {
        md_copyName(wait_anim_name, 0x1001D9DC, 0x10);     /* "md_shipwait.bck" */
        md_copyName(arm_wait_anim_name, 0x1001DB18, 0x13); /* "mdarm_shipwait.bck" */
    } else {
        md_copyName(wait_anim_name, 0x1001DA70, 0xE);      /* "md_wait01.bck" */
        md_copyName(arm_wait_anim_name, 0x1001DB2C, 0x11); /* "mdarm_wait01.bck" */
    }

    J3DModelData* modelData = (J3DModelData*)md_getObjectRes(mModelArcName, STR(0x1001D994) /* "md.bdl" */);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR(MD_HEAP_FILE), 0x843, STR(0x1001DB50));
    }
    void* anm = md_getObjectRes_at(sp + 0x2C, sp + 0x34, mModelArcName, wait_anim_name);
    mpMorf = gabi::call<mDoExt_McaMorf2*>(0x025E6220 /* mDoExt_McaMorf2::mDoExt_McaMorf2 (new) */, 0u, modelData, 0u, 0u, anm, 0u,
                                          2 /* EMode_LOOP */, 1.0f, 0, -1, 1, 0u, 0x00080000u, 0x11020022u);
    if (mpMorf.get() == nullptr || md_morfModel(mpMorf.get()) == nullptr) {
        return FALSE;
    }

    md_jntIndex(modelData, 0x1001DA80 /* "backbone1" */, 0x856, 0x1001DB64, &m_backbone1_jnt_num);
    md_jntIndex(modelData, 0x1001DA8C /* "backbone2" */, 0x859, 0x1001DB80, &m_backbone2_jnt_num);
    md_jntIndex(modelData, 0x1001D99C /* "armR" */, 0x85C, 0x1001DA1C, &m_armR_jnt_num);
    md_jntIndex(modelData, 0x1001D9A4 /* "armL" */, 0x85F, 0x1001DA30, &m_armL_jnt_num);
    md_jntIndex(modelData, 0x1001D9AC /* "neck" */, 0x862, 0x1001DA44, &m_neck_jnt_num);

    s16 head_jnt_num = (s16)JUTNameTab_getIndex(J3DModelData_getJointName(modelData), 0x1001D9B4 /* "head" */);
    if (head_jnt_num >= 0) {
        md_setJointCallBack(modelData, head_jnt_num, 0x02283614 /* nodeCallBack */);
    }
    md_setJointCallBack(modelData, (s16)(s8)m_backbone1_jnt_num, 0x022837D0 /* waistNodeCallBack */);
    int i = 0;
    {
        u32 name = gabi::load<u32>(0x101C02DC); /* hairName[0] */
        s8 idx = (s8)JUTNameTab_getIndex(J3DModelData_getJointName(modelData), name);
        m_hair_jnt_nums[i] = idx;
        if (idx >= 0) {
            md_setJointCallBack(modelData, idx, 0x022839F0 /* hairTopNodeCallBack */);
        }
    }
    for (i = 1; i < 8; i++) {
        u32 name = gabi::load<u32>(0x101C02DC + i * 4);
        s8 idx = (s8)JUTNameTab_getIndex(J3DModelData_getJointName(modelData), name);
        m_hair_jnt_nums[i] = idx;
        if (idx >= 0) {
            md_setJointCallBack(modelData, idx, 0x02283A78 /* hairNodeCallBack */);
        }
    }

    gabi::store<u32>(gabi::ea(md_morfModel(mpMorf.get())) + 0xB8, gabi::ea(this)); /* setUserArea(this) */

    modelData = (J3DModelData*)md_getObjectRes(mModelArcName, STR(0x1001DA98) /* "mdarm.bdl" */);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR(MD_HEAP_FILE), 0x8A0, STR(0x1001DB50));
    }
    anm = md_getObjectRes_at(sp + 0x4C, sp + 0x54, mModelArcName, arm_wait_anim_name);
    mpArmMorf = gabi::call<mDoExt_McaMorf2*>(0x025E6220, 0u, modelData, 0u, 0u, anm, 0u, 2, 1.0f, 0, -1, 0, 0u, 0u, 0x11020203u);
    if (mpArmMorf.get() == nullptr || md_morfModel(mpArmMorf.get()) == nullptr) {
        return FALSE;
    }

    md_jntIndex(modelData, 0x1001D97C /* "armRloc" */, 0x8AF, 0x1001DB9C, &m_armRloc_jnt_num);
    md_jntIndex(modelData, 0x1001D984 /* "armLloc" */, 0x8B2, 0x1001DBB4, &m_armLloc_jnt_num);
    md_jntIndex(modelData, 0x1001D98C /* "handL" */, 0x8B5, 0x1001DBCC, &m_handL_jnt_num);

    md_setJointCallBack(modelData, (s16)(s8)m_armRloc_jnt_num, 0x02284418 /* armNodeCallBack */);
    md_setJointCallBack(modelData, (s16)(s8)m_armLloc_jnt_num, 0x02284418);

    gabi::store<u32>(gabi::ea(md_morfModel(mpArmMorf.get())) + 0xB8, gabi::ea(this));

    if (!isTypeShipRide()) {
        modelData = (J3DModelData*)md_getObjectRes(mModelArcName, STR(0x1001DBE4) /* "mdwing.bdl" */);
        if (modelData == nullptr) {
            JUT_ASSERT_fail(STR(MD_HEAP_FILE), 0x8CC, STR(0x1001DB50));
        }
        anm = md_getObjectRes(mModelArcName, STR(0x1001DAA4) /* "mdwing_wait01.bck" */);
        mpWingMorf = gabi::call<mDoExt_McaMorf*>(0x025E4F64 /* mDoExt_McaMorf::mDoExt_McaMorf (new) */, 0u, modelData, 0u, 0u, anm,
                                                 2 /* EMode_LOOP */, 1.0f, 0, -1, 0, 0u, 0u, 0x11020203u);
        if (mpWingMorf.get() == nullptr || md_morfModel(mpWingMorf.get()) == nullptr) {
            return FALSE;
        }

        md_jntIndex(modelData, 0x1001DBF0 /* "wingRloc" */, 0x8DB, 0x1001D9EC, &m_wingRloc_jnt_num);
        md_jntIndex(modelData, 0x1001DBFC /* "wingLloc" */, 0x8DE, 0x1001DA04, &m_wingLloc_jnt_num);
        md_jntIndex(modelData, 0x1001D9BC /* "wingR2" */, 0x8E1, 0x1001DAB8, &m_wingR2_jnt_num);
        md_jntIndex(modelData, 0x1001D9C4 /* "wingL2" */, 0x8E4, 0x1001DAD0, &m_wingL2_jnt_num);
        md_jntIndex(modelData, 0x1001D9CC /* "wingR3" */, 0x8E7, 0x1001DAE8, &m_wingR3_jnt_num);
        md_jntIndex(modelData, 0x1001D9D4 /* "wingL3" */, 0x8EA, 0x1001DB00, &m_wingL3_jnt_num);

        md_setJointCallBack(modelData, (s16)(s8)m_wingRloc_jnt_num, 0x02284418 /* armNodeCallBack */);
        md_setJointCallBack(modelData, (s16)(s8)m_wingLloc_jnt_num, 0x02284418);

        gabi::store<u32>(gabi::ea(md_morfModel(mpWingMorf.get())) + 0xB8, gabi::ea(this));
    }

    modelData = (J3DModelData*)md_getObjectRes(mModelArcName, STR(0x1001DA58) /* "md_harp.bdl" */);
    mpHarpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (mpHarpModel.get() == nullptr) {
        return FALSE;
    }

    modelData = (J3DModelData*)md_getObjectRes(mModelArcName, STR(0x1001DA64) /* "ymdhp00.bdl" */);
    mpHarpLightModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (mpHarpLightModel.get() == nullptr) {
        return FALSE;
    }

    if (!initLightBtkAnm(false)) {
        return FALSE;
    }
    if (!initTexPatternAnm(0, 0)) {
        return FALSE;
    }

    mAcchCir[0].SetWall(20.0f, 20.0f);
    mAcchCir[1].SetWall(60.0f, 20.0f);
    mAcch.Set(&current.pos, &old.pos, this, 2, mAcchCir, &speed);
    u32 flags = mAcch.m_flags;
    mAcch.m_roof_crr_height = 120.0f;                    /* SetRoofCrrHeight(120.0f) */
    mAcch.m_flags = ((flags & ~8u) | 0x2000) & ~0x400u; /* ClrRoofNone, OnLineCheck, ClrWaterNone */
    return TRUE;
}
VERIFY(0x022847F8, &daNpc_Md_c::createHeap);

/* 02285380 CheckCreateHeap (not named by the matcher) */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02285380, BOOL, i_this);
    return static_cast<daNpc_Md_c*>(i_this)->createHeap();
}
VERIFY(0x02285380, CheckCreateHeap);

/* 02285384 */
BOOL daNpc_Md_c::setAction(ProcFunc_l* cur, ProcFunc_l* newFunc, void* arg) {
    WWHD_FUNC(0x02285384, BOOL, this, cur, newFunc, arg);
    s16 newI = newFunc->i;
    s16 newD;
    u32 newF;
    if ((u16)cur->i == (u16)newI) {
        if (cur->i == 0) {
            return TRUE;
        }
        newD = newFunc->d;
        newF = newFunc->f;
        if (!((u16)cur->d != (u16)newD || cur->f != newF)) {
            return TRUE;
        }
    } else {
        newF = newFunc->f;
        newD = newFunc->d;
        if (cur->i == 0) {
            goto set;
        }
    }
    mActionStatus = ACTION_ENDING;
    md_pmf_call<BOOL>(this, cur, arg);
set:
    cur->i = newI;
    cur->f = newF;
    cur->d = newD;
    m3150 = 0.0f;
    m314A = 0;
    m3146 = 0;
    m3148 = 0;
    m3144 = 0;
    mActionStatus = ACTION_STARTING;
    md_pmf_call<BOOL>(this, cur, arg);
    return TRUE;
}
VERIFY(0x02285384, &daNpc_Md_c::setAction);

/* 02285520 */
BOOL daNpc_Md_c::shipRideCheck() {
    WWHD_FUNC(0x02285520, BOOL, this);
    if (isTypeShipRide()) {
        gabi::Local<ProcFunc_l> fn;
        md_pmf_load(fn, PMF_shipNpcAction);
        setNpcAction(fn, nullptr);
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x02285520, &daNpc_Md_c::shipRideCheck);

/* 02285984 */
void daNpc_Md_c::setAttention(bool param_1) {
    WWHD_FUNC(0x02285984, void, this, param_1);
    if (!param_1 && m312B >= 2) {
        return;
    }
    eyePos.y = m3088.y;
    eyePos.z = m3088.z;
    eyePos.x = m3088.x;
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    f32 fVar1;
    if (gabi::load<u8>(MD_M_FLYING) != 0) { /* isFlying() */
        fVar1 = 70.0f;
    } else if (dComIfGp_getPlayer(0) == this) {
        fVar1 = 110.0f;
    } else {
        fVar1 = l_HIO().mNpc.mAttnYOffset;
    }
    f32 z = current.pos.z;
    f32 y = current.pos.y + fVar1;
    f32 x = current.pos.x;
    attPos->z = z;
    attPos->x = x;
    attPos->y = y;
}
VERIFY(0x02285984, &daNpc_Md_c::setAttention);

/* 02285580 */
BOOL daNpc_Md_c::init() {
    WWHD_FUNC(0x02285580, BOOL, this);
    u8 uVar1 = fopAcM_GetParam(this) & 0xFF;
    m3100 = (fopAcM_GetParam(this) >> 0x10) & 0xFF;
    m313E = 0;
    m30F4 = 0x5940; /* HD: JA_SE_CM_MD_HARP_CN4 */
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA);  /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    gabi::store<u8>(gabi::ea(this) + 0x389, 0x21);  /* distances[TALK] */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0x21);  /* distances[SPEAK] */
    gabi::store<u8>(gabi::ea(this) + 0x38C, 0x27);  /* distances[CARRY] */
    mCurEventMode = 0;
    mCurEvent = 0;
    for (int i = 0; i < 6; i++) {
        m0508[i] = 0;
    }
    m30AC.x = 0.0f;
    m30AC.y = 0.0f;
    m30AC.z = 50.0f;
    m313C = 0; /* setPiyo2TalkCNT(0) */
    m30F0 = 0; /* clearStatus() */
    gravity = l_HIO().m0F4;
    gabi::Local<ProcFunc_l> fn;
    if (mType == 6) { /* isTypeM_DaiB() */
        md_pmf_load(fn, PMF_harpWaitNpcAction);
    } else {
        md_pmf_load(fn, PMF_waitNpcAction);
    }
    setNpcAction(fn, nullptr);
    m3094.copy(current.pos);
    m3088.copy(current.pos);
    for (s32 i = 0; i < 4; i++) {
        m3234[i].set(0.0f, 0.0f, 0.0f);
    }
    if ((uVar1 & 1) != 0) {
        m30F0 = daMdStts_UNK8; /* setStatus */
    }
    m3135 = 0; /* setEffectStatus(0) */
    mCurEvent = -1;
    gabi::store<u8>(MD_M_FLYING, 0); /* offFlying() */
    gabi::store<u8>(0x101D5F3F, 0);  /* offMirror() (m_mirror) */
    mRunRate = 0.0f;
    m30F8 = 120.0f;
    gabi::store<f32>(gabi::ea(mpMorf.get()) + 0xC0, 0.0f); /* mpMorf->setAnmRate(mRunRate) */
    gabi::store<f32>(gabi::ea(mpArmMorf.get()) + 0xC0, mRunRate);
    if (!md_isEventBit(0x1620)) {
        mNoResetFlg1 = mNoResetFlg1 | 0x40; /* onNpcNotChange() */
    } else if (shipRideCheck()) {
        gabi::call(0x025E5D1C, mpMorf.get(), 0.0f); /* mDoExt_McaMorf2::setMorf */
        gabi::call(0x025E5D1C, mpArmMorf.get(), 0.0f);
    }
    if (mType == 5 || mType == 4) { /* isTypeM_Dai() || isTypeEdaichi() */
        mStts.Init(0xFE, 0xFF, this);
    } else {
        mStts.Init(0xFF, 0xFF, this);
    }
    mCyl1.Set(gabi::at<dCcD_SrcCyl>(0x101C031C)); /* l_cyl_src */
    mCyl1.SetStts(&mStts);
    mCyl2.Set(gabi::at<dCcD_SrcCyl>(0x101C03F0)); /* l_wind_cyl_src */
    mCyl2.SetStts(&mStts);
    mCyl3.Set(gabi::at<dCcD_SrcCyl>(0x101C0360)); /* l_light_cyl_src */
    mCyl3.SetStts(&mStts);
    gabi::call(0x025164C0, &mCps, 0x101C03A4u); /* mCps.Set(l_fan_light_cps_src) */
    mCps.SetAtType(0x800000); /* AT_TYPE_LIGHT */
    mCps.SetStts(&mStts);
    gabi::store<f32>(gabi::ea(&mCps) + 0x134, 20.0f); /* mCps.SetR(20.0f) */
    void* pBti = md_getObjectRes(mModelArcName, STR(0x1001DC14) /* "md_spot.bti" */);
    gabi::call(0x0252D6E8, m0B70, pBti); /* dDlst_mirrorPacket::init */
    if (mType == 4) { /* isTypeEdaichi() */
        mNoResetFlg1 = mNoResetFlg1 | 2; /* onNpcCallCommand() */
    }
    for (s32 i = 0; i < 10; i++) {
        mEventIdxTable[i] = dComIfGp_evmng_getEventIdx(gabi::at<const char>(gabi::load<u32>(0x101C0284 + i * 4)) /* event_name_tbl */, 0xFF);
    }
    gabi::store<u32>(gabi::ea(this) + 0x104, 0x02283584); /* eventInfo.setXyCheckCB(daNpc_Md_XyCheckCB) */
    gabi::store<u32>(gabi::ea(this) + 0x100, 0x02283610); /* eventInfo.setXyEventCB(daNpc_Md_XyEventCB) */
    return TRUE;
}
VERIFY(0x02285580, &daNpc_Md_c::init);

/* 02285A6C */
void daNpc_Md_c::setBaseMtx() {
    WWHD_FUNC(0x02285A6C, void, this);
    J3DModel* model = md_morfModel(mpMorf.get());
    Mtx34* now = mDoMtx_stack_c::get();
    u32 status = m30F0; /* HD: read before the matrix calls */
    if (actor_status & 0x2000) { /* fopAcM_checkCarryNow(this) */
        PSMTXTrans(now, current.pos.x, current.pos.y, current.pos.z); /* transS */
        if (status & daMdStts_CARRY_ACTION) { /* isNoCarryAction() */
            mDoMtx_ZXYrotM(now, shape_angle.x, shape_angle.y, shape_angle.z);
            mDoMtx_stack_transM(0.0f, -l_HIO().m05C.m4, 0.0f);
        } else {
            mDoMtx_YrotM(now, shape_angle.y);
            mDoMtx_stack_transM(m3298.x, m3298.y, m3298.z);
            mDoMtx_XrotM(now, shape_angle.x);
            mDoMtx_ZrotM(now, shape_angle.z);
        }
    } else if (status & daMdStts_SHIP_RIDE) { /* isShipRide() */
        actor_status = actor_status | 0x4000; /* fopAcM_OnStatus(this, fopAcStts_UNK4000_e) */
        fopAc_ac_c* ship = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B3C)); /* dComIfGp_getShipActor() */
        /* ship->getBodyMtx(): the body morf's model base matrix */
        J3DModel* shipModel = gabi::at<J3DModel>(gabi::load<u32>(gabi::load<u32>(gabi::ea(ship) + 0x3B4) + 0x90));
        PSMTXCopy(J3DModel_getBaseTRMtx(shipModel), now);
        mDoMtx_stack_transM(43.0f, 14.65f, 13.99f);
        mDoMtx_XYZrotM(now, 0, -0x45B0, 0);
        f32 z = now->m[2][3];
        f32 x = now->m[0][3];
        f32 y = now->m[1][3];
        current.pos.x = x;
        current.pos.y = y;
        current.pos.z = z;
        s16 sVar4 = ship->shape_angle.y + -0x4000;
        current.angle.y = sVar4;
        shape_angle.y = sVar4;
        setAttention(true);
        gabi::store<u8>(gabi::ea(this) + 0x1C9, gabi::load<u8>(gabi::ea(ship) + 0x1C9)); /* tevStr.mRoomNo */
        gabi::store<u8>(gabi::ea(this) + 0x1CA, gabi::load<u8>(gabi::ea(ship) + 0x1CA)); /* tevStr.mEnvrIdxOverride */
    } else {
        PSMTXTrans(now, current.pos.x, current.pos.y + 95.0f, current.pos.z);
        mDoMtx_YrotM(now, shape_angle.y);
        mDoMtx_stack_transM(m3298.x, m3298.y, m3298.z);
        mDoMtx_XrotM(now, shape_angle.x);
        mDoMtx_ZrotM(now, shape_angle.z);
        mDoMtx_stack_transM(0.0f, -95.0f, 0.0f);
    }
    J3DModel_setBaseTRMtx(model, now);
    McaMorf2_calc(mpMorf.get());
    if (!isTypeShipRide()) {
        McaMorf_calc((mDoExt_McaMorf_c*)(void*)mpWingMorf.get());
    }
    McaMorf2_calc(mpArmMorf.get());
    if (checkStatus(daMdStts_UNK80)) {
        J3DModel* armModel = md_morfModel(mpArmMorf.get());
        PSMTXCopy(md_getAnmMtx(armModel, (s32)m_handL_jnt_num), now);
        mDoMtx_stack_transM(0.37f, 3.81f, -11.13f);
        mDoMtx_XYZrotM(now, 0x1B24, -0x2913, -0x1982);
    } else {
        J3DModel* bodyModel = md_morfModel(mpMorf.get());
        PSMTXCopy(md_getAnmMtx(bodyModel, (s32)m_backbone2_jnt_num), now);
        mDoMtx_stack_transM(2.2f, -11.681f, 1.3f);
        mDoMtx_XYZrotM(now, 0x3F11, 0x2894, 0x7F7E);
    }
    J3DModel_setBaseTRMtx(mpHarpModel.get(), now);
    J3DModel* lightModel = mpHarpLightModel.get();
    J3DModel_setBaseTRMtx(lightModel, J3DModel_getBaseTRMtx(mpHarpModel.get()));
}
VERIFY(0x02285A6C, &daNpc_Md_c::setBaseMtx);

/* sead::SafeString operator== (HD inline, as d_a_mo2): cstr() of both operands through the vtable
 * (slot 0x14; the left one twice), pointer compare, then a bounded strcmp (0x40001 characters) */
static inline bool md_isStartStage(u32 lit) {
    gabi::Local<SafeString> sa;
    sa->mStringTop = lit;
    sa->__vtbl = MD_SAFESTRING_VTBL;
    u32 stage = dComIfGp_ea() + 0x5134; /* dComIfGp_getStartStageName() */
    gabi::Local<SafeString> sb;
    sb->mStringTop = stage;
    sb->__vtbl = MD_SAFESTRING_VTBL;
    gabi::call_ptr(gabi::load<u32>(sa->__vtbl + 0x14), sa.get());
    gabi::call_ptr(gabi::load<u32>(sa->__vtbl + 0x14), sa.get());
    u32 pa = sa->mStringTop;
    gabi::call_ptr(gabi::load<u32>(sb->__vtbl + 0x14), sb.get());
    u32 pb = sb->mStringTop;
    if (pa == pb) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = gabi::load<u8>(pa + i);
        if (ca != gabi::load<u8>(pb + i)) return false;
        if (ca == 0) return true;
    }
    return false;
}

/* 02286084 */
cPhs_State daNpc_Md_c::create() {
    WWHD_FUNC(0x02286084, cPhs_State, this);
    m313D = 0;
    gabi::call(0x028F040C /* strcpy */, mModelArcName, gabi::load<u32>(0x101C0278) /* l_arc_name "Md" */);
    int heapSizeIdx = 0;

    /* fopAcM_ct(this, daNpc_Md_c): the constructor, inlined */
    if (!(actor_condition & 8)) {
        if (this != nullptr) {
            u32 t = gabi::ea(this);
            gabi::call(0x024450B4, this); /* daPy_npc_c::daPy_npc_c */
            __vtbl = MD_VTBL;
            gabi::call(0x025E7820, t + 0x63C); /* mDoExt_btpAnm::mDoExt_btpAnm */
            mDoExt_btkAnm::ct(&mLightBtkAnm);
            gabi::call(0x028EFFD0 /* __construct_array */, mAcchCir, 2, 0x40, 0x024EFE94u /* dBgS_AcchCir::dBgS_AcchCir */);
            /* dBgS_MirLightLinChk mLinChk */
            gabi::call(0x02008FEC, t + 0x7A4); /* cBgS_LinChk::cBgS_LinChk */
            gabi::store<u8>(t + 0x804, 0);
            gabi::store<u8>(t + 0x801, 0);
            gabi::store<u8>(t + 0x806, 0);
            gabi::store<u32>(t + 0x7A4, t + 0x7FC);
            gabi::store<u32>(t + 0x7B4, 0x1001D798);
            gabi::store<u32>(t + 0x7A8, t + 0x808);
            gabi::store<u32>(t + 0x7C4, 0x1001D7A8);
            gabi::store<u8>(t + 0x805, 0);
            gabi::store<u8>(t + 0x803, 1);
            gabi::store<u32>(t + 0x808, 0x1001D7B8);
            gabi::store<u8>(t + 0x802, 0);
            gabi::store<u32>(t + 0x80C, 0x1F);
            gabi::store<u32>(t + 0x7FC, 0x1001D7C8);
            gabi::store<u8>(t + 0x800, 0);
            /* dCcD_Stts mStts */
            gabi::call(0x0200BD2C, &mStts); /* cCcD_Stts::cCcD_Stts */
            gabi::call(0x02515DA0, t + 0x82C); /* dCcD_GStts::dCcD_GStts */
            gabi::store<u32>(t + 0x828, 0x1004AE88);
            gabi::store<u32>(t + 0x82C, 0x1004AEC0);
            /* dCcD_Cyl mCyl1..3 */
            for (u32 c = 0x84C; c < 0xBDC; c += 0x130) {
                gabi::call(0x02515FB8, t + c); /* dCcD_GObjInf::dCcD_GObjInf */
                gabi::store<u32>(t + c + 0x114, 0x100015A8);
                gabi::store<u32>(t + c + 0x110, 0x1001D6D8);
                gabi::call(0x02018590, t + c + 0x118); /* cM3dGCyl::cM3dGCyl */
                gabi::store<u32>(t + c + 0x3C, 0x1004B108);
                gabi::store<u32>(t + c + 0x114, 0x1004B160);
                gabi::store<u32>(t + c + 0x12C, 0x1004B150);
            }
            /* dCcD_Cps mCps */
            gabi::call(0x02515FB8, &mCps);
            gabi::store<u32>(t + 0xCEC, 0x1001D6D8);
            gabi::store<u32>(t + 0xCF0, 0x100015A8);
            gabi::call(0x02018150, t + 0xCF4); /* cM3dGCps::cM3dGCps */
            gabi::store<u32>(t + 0xC18, 0x1004AF18);
            gabi::store<u32>(t + 0xD0C, 0x1004AF60);
            gabi::store<u32>(t + 0xCF0, 0x1004AF70);
            gabi::call(0x0259DAA0, &mJntCtrl); /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
            gabi::call(0x0252CE74, m0B70);    /* dDlst_mirrorPacket::dDlst_mirrorPacket */
            m304C.__vtbl = 0x100347F0;         /* daPy_mtxFollowEcallBack_c */
            m3058.__vtbl = 0x1001E048;         /* daNpc_Md_followEcallBack_c */
            gabi::call(0x025A9084, &m3074);    /* dPa_rippleEcallBack::dPa_rippleEcallBack */
            /* cBgS_PolyInfo mPolyInfo */
            mPolyInfo.mpBgW = 0;
            mPolyInfo.__vtbl = 0x1001D6E8;
            mPolyInfo.mPolyIndex = 0xFFFF;
            mPolyInfo.mBgIndex = 0x100;
            mPolyInfo.mProcId = -1;
        }
        actor_condition = actor_condition | 8;
    }

    mType = (u8)(fopAcM_GetParam(this) >> 0x08); /* setTalkType; HD: the (always false) == -2 test is gone */
    u32 save = gabi::load<u32>(0x101F84DC);
    if (gabi::call<BOOL>(0x025B7A2C /* dSv_player_collect_c::isCollect */, save + 0xD4, 0, 2)) {
        if (!md_isStartStage(0x1001DC80 /* "M_DaiB" */)) {
            return cPhs_ERROR_e;
        }
        mType = 6; /* setTypeM_DaiB() */
    } else if (md_isStartStage(0x1001DC74 /* "sea" */)) {
        if (md_isEventBit(0x2E04) || !md_isEventBit(0x1820) || !gabi::call<BOOL>(0x02520A84 /* dComIfGs_isStageBossEnemy */, 3)) {
            return cPhs_ERROR_e;
        }
    } else if (md_isStartStage(0x1001DC54 /* "Atorizk" */)) {
        if (md_isEventBit(0x2E04) || gabi::call<BOOL>(0x0259D734 /* dNpc_chkLetterPassed */)) {
            return cPhs_ERROR_e;
        }
    } else if (md_isStartStage(0x1001DC5C /* "Adanmae" */)) {
        if (md_isEventBit(0x2E04) || !gabi::call<BOOL>(0x0259D734)) {
            return cPhs_ERROR_e;
        }
    } else if (md_isStartStage(0x1001DC64 /* "M_Dra09" */)) {
        if (md_isEventBit(0x2E04) || md_isEventBit(0x1101)) {
            return cPhs_ERROR_e;
        }
    } else if (md_isStartStage(0x1001DC6C /* "Edaichi" */)) {
        if (!md_isEventBit(0x2E04) || md_isEventBit(0x2920)) {
            return cPhs_ERROR_e;
        }
        mType = 4; /* setTypeEdaichi() */
    } else if (md_isStartStage(0x1001DC78 /* "M_Dai" */)) {
        if (!md_isEventBit(0x2E04) || !md_isEventBit(0x2920)) {
            return cPhs_ERROR_e;
        }
        mType = 5; /* setTypeM_Dai() */
    } else if (md_isStartStage(0x1001DC80 /* "M_DaiB" */)) {
        return cPhs_ERROR_e;
    }

    if (!md_isEventBit(0x2E04) && md_isEventBit(0x1608)) {
        mType = 7; /* setTypeShipRide() */
        gabi::call(0x028F040C /* strcpy */, mModelArcName, gabi::load<u32>(0x101C027C) /* l_arc_name_ship "Md_ship" */);
        heapSizeIdx = 1;
    }

    cPhs_State phase_state = dComIfG_resLoad(&mPhase, mModelArcName);
    m313D = 1;
    if (phase_state == cPhs_COMPLEATE_e) {
        if (gabi::load<u32>(dComIfGp_ea() + 0x5B38) != 0) { /* dComIfGp_getCb1Player() */
            return cPhs_ERROR_e;
        }
        /* static int l_heep_size[] = {0x7660, 0x61C0} */
        if (!gabi::call<BOOL>(0x025D63E8 /* fopAcM_entrySolidHeap */, this, 0x02285380u /* CheckCreateHeap */,
                              gabi::load<u32>(0x101C07FC + heapSizeIdx * 4))) {
            mpArmMorf = nullptr;
            mpMorf = nullptr;
            mpWingMorf = nullptr;
            return cPhs_ERROR_e;
        }

        if (mType == 5) { /* isTypeM_Dai() */
            u32 sv = gabi::load<u32>(0x101F84DC);
            u32 priest = sv + 0x1CC; /* dSv_player_priest_c: pos, rotate +0xC, roomNo +0xE, flag +0xF */
            if (gabi::load<u8>(sv + 0x1DB) == 2) {
                s8 roomNo = gabi::load<s8>(priest + 0xE);
                s16 rot = gabi::load<s16>(priest + 0xC);
                gabi::call(0x025B9834 /* dSv_restart_c::setRestartOption */, sv + 0x1148, 2, priest, rot, roomNo);
                /* HD: the priest position is set again */
                gabi::call(0x025B8880 /* dSv_player_priest_c::set */, gabi::load<u32>(0x101F84DC) + 0x1CC, 2, priest, rot, roomNo);
            }
            gabi::call(0x02445784 /* daPy_npc_c::checkRestart */, this, 2);
        }

        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(md_morfModel(mpMorf.get()))); /* fopAcM_SetMtx */
        /* HD: no l_HIO child (mDoHIO_createChild) */
        if (!init()) {
            return cPhs_ERROR_e;
        }

        setBaseMtx();
        fopAcM_setStageLayer(this);
        gabi::store<u32>(0x101CEF74, gabi::ea(this)); /* HD: a global pointer to Medli */
    }

    return phase_state;
}
VERIFY(0x02286084, &daNpc_Md_c::create);

/* 02286B58 */
static cPhs_State daNpc_Md_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02286B58, cPhs_State, i_this);
    return static_cast<daNpc_Md_c*>(i_this)->create();
}
VERIFY(0x02286B58, daNpc_Md_Create);
