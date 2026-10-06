/**
 * d_a_npc_zl1_draw.cpp (WWHD)
 * NPC - Tetra: node callbacks, animation, effects, matrices and drawing
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_zl1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_zl1.h"

/* 023020F8 */
void daNpc_Zl1_c::_nodeCB_Head(J3DNode* i_node, J3DModel* i_pModel) {
    WWHD_FUNC(0x023020F8, void, this, i_node, i_pModel);
    /* static cXyz a_eye_pos_off(20.0f, -16.0f, 0.0f): guard 0x10468CD4, object 0x10468BB4 */
    cXyz* a_eye_pos_off = gabi::at<cXyz>(0x10468BB4);
    if (gabi::load<u32>(0x10468CD4) == 0) {
        gabi::store<u32>(0x10468CD4, 1);
        a_eye_pos_off->z = 0.0f;
        a_eye_pos_off->x = 20.0f;
        a_eye_pos_off->y = -16.0f;
    }
    u32 jointIdx = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_pModel, jointIdx), stk);
    field_0x770.x = stk->m[0][3]; /* mDoMtx_stack_c::multVecZero(&field_0x770) */
    field_0x770.y = stk->m[1][3];
    field_0x770.z = stk->m[2][3];
    mDoMtx_YrotM(stk, -field_0x83C);
    mDoMtx_ZrotM(stk, -field_0x83E);
    PSMTXMultVec(stk, a_eye_pos_off, &field_0x74C);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_pModel, jointIdx), stk);
}
VERIFY(0x023020F8, &daNpc_Zl1_c::_nodeCB_Head);

/* node callbacks: if (calcTiming == In && j3dSys.getModel()->getUserArea()) actor->_nodeCB_X(node, model) */
static inline void nodeCB_dispatch(J3DNode* node, int timing, u32 fn) {
    if (timing == 0) {
        u32 model = gabi::load<u32>(0x104B462C);
        u32 user = gabi::load<u32>(model + 0xB8);
        if (user != 0)
            gabi::call(fn, user, node, model);
    }
}

/* 02302278 */
static BOOL nodeCB_Head(J3DNode* i_node, int i_param) {
    WWHD_FUNC(0x02302278, BOOL, i_node, i_param);
    nodeCB_dispatch(i_node, i_param, 0x023020F8);
    return TRUE;
}
VERIFY(0x02302278, nodeCB_Head);

/* 023022C0 */
void daNpc_Zl1_c::_nodeCB_BackBone(J3DNode* i_node, J3DModel* i_pModel) {
    WWHD_FUNC(0x023022C0, void, this, i_node, i_pModel);
    u32 jointIdx = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_pModel, jointIdx), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stk, -m_jnt.mAngles[1][0]);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_pModel, jointIdx), stk);
}
VERIFY(0x023022C0, &daNpc_Zl1_c::_nodeCB_BackBone);

/* 023023E4 */
static BOOL nodeCB_BackBone(J3DNode* i_node, int i_param) {
    WWHD_FUNC(0x023023E4, BOOL, i_node, i_param);
    nodeCB_dispatch(i_node, i_param, 0x023022C0);
    return TRUE;
}
VERIFY(0x023023E4, nodeCB_BackBone);

/* 023041F8 setEyeCtrl / 02304230 clrEyeCtrl: in d_a_npc_zl1.cpp */

/* 02304268 */
void daNpc_Zl1_c::play_btp_anm() {
    WWHD_FUNC(0x02304268, void, this);
    u8 frame = (u8)J3DAnm_getFrameMax(gabi::at<u8>(gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10))); /* mBtpAnm.getBtpAnm() */
    if (field_0x847 != 1 || cLib_calcTimer(&mTimer) == 0) {
        u8 f = (u8)(mBtpAnmFrame + 1);
        mBtpAnmFrame = f;
        if (f >= frame) {
            if (field_0x847 != 1) {
                mBtpAnmFrame = frame;
            } else {
                mTimer = (s16)cLib_getRndValue(0x3C, 0x5A);
                mBtpAnmFrame = 0;
            }
        }
    }
}
VERIFY(0x02304268, &daNpc_Zl1_c::play_btp_anm);

static inline f32 fsel(f32 a, f32 b, f32 c) { return a >= 0.0f ? b : c; }

/* 02304310 */
void daNpc_Zl1_c::eye_ctrl() {
    WWHD_FUNC(0x02304310, void, this);
    f32 temp, temp2;
    if (field_0x7D1) {
        f32 div = (f32)(s32)l_HIO().mPrmTbl.field_5E;
        temp = ((f32)(s32)field_0x842 / div) * 0.1f;
        temp2 = ((f32)(s32)field_0x840 / div) * 0.1f;
        /* cLib_minMaxLimit(-1, 1): the lower bound by a branch, the upper one by fsel (NaN kept) */
        temp = temp < -1.0f ? -1.0f : fsel(temp - 1.0f, 1.0f, temp);
        temp2 = temp2 < -1.0f ? -1.0f : fsel(temp2 - 1.0f, 1.0f, temp2);
    } else {
        temp = 0.0f;
        temp2 = temp;
    }
    if (field_0x6D4[0].get() != nullptr) {
        cLib_addCalc(&field_0x6D4[0]->mOffsetX, temp2, 0.5f, 0.1f, 0.03f);
        cLib_addCalc(&field_0x6D4[0]->mOffsetY, temp, 0.5f, 0.1f, 0.03f);
    }
    temp2 = -temp2;
    if (field_0x6D4[1].get() != nullptr) {
        cLib_addCalc(&field_0x6D4[1]->mOffsetX, temp2, 0.5f, 0.1f, 0.03f);
        cLib_addCalc(&field_0x6D4[1]->mOffsetY, temp, 0.5f, 0.1f, 0.03f);
    }
}
VERIFY(0x02304310, &daNpc_Zl1_c::eye_ctrl);

/* 02304548 */
void daNpc_Zl1_c::play_btk_anm() {
    WWHD_FUNC(0x02304548, void, this);
    u8 frame = (u8)J3DAnm_getFrameMax(gabi::at<u8>(gabi::load<u32>(gabi::ea(mBtkAnm) + 0x68))); /* mBtkAnm.getBtkAnm() */
    if (field_0x7D0) {
        eye_ctrl();
    } else {
        u8 f = (u8)(mBtkAnmFrame + 1);
        if (f >= frame) {
            mBtkAnmFrame = frame;
        } else {
            mBtkAnmFrame = f;
        }
    }
}
VERIFY(0x02304548, &daNpc_Zl1_c::play_btk_anm);

/* 023045E0 */
void daNpc_Zl1_c::play_animation() {
    WWHD_FUNC(0x023045E0, void, this);
    u32 mtrlSnd = 0;
    if (field_0x848 == 1 && field_0x7D0 != 0) {
        setEyeCtrl();
    } else {
        clrEyeCtrl();
    }
    play_btp_anm();
    play_btk_anm();
    if (mObjAcch.ChkGroundHit()) {
        mtrlSnd = dBgS_GetMtrlSndId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    }
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    field_0x7C3 = (s8)mpMorf->play(&eyePos, mtrlSnd, (s8)reverb);
    if (mpMorf->getFrame() < mFrame) {
        field_0x7C3 = 1;
    }
    mFrame = mpMorf->getFrame();
    switch ((u32)(s32)field_0x847) {
    case 0xD:
    case 0xE:
        mBtpAnmFrame = (u8)gabi::ftoi(mpMorf->getFrame());
        break;
    }
    switch ((u32)(s32)field_0x848) {
    case 6:
    case 7:
    case 8:
        mBtkAnmFrame = (u8)gabi::ftoi(mpMorf->getFrame());
        break;
    }
}
VERIFY(0x023045E0, &daNpc_Zl1_c::play_animation);

/* 0230473C */
bool daNpc_Zl1_c::swoon_OnShip() {
    WWHD_FUNC(0x0230473C, bool, this);
    fopAc_ac_c* pShip = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B3C)); /* dComIfGp_getShipActor() */
    if (pShip != nullptr) {
        /* pShip->getBodyMtx(): the morf at +0x3B4, its model's base matrix */
        J3DModel* body = gabi::at<mDoExt_McaMorf>(gabi::load<u32>(gabi::ea(pShip) + 0x3B4))->getModel();
        Mtx34* stk = mDoMtx_stack_c::get();
        PSMTXCopy(J3DModel_getBaseTRMtx(body), stk);
        mDoMtx_stack_c::transM(-23.88f, 58.66f, -12.44f);
        mDoMtx_XYZrotM(stk, 0x471C, 0x156, 0x4147);
        current.pos.x = stk->m[0][3];
        current.pos.z = stk->m[2][3];
        current.pos.y = stk->m[1][3];
        current.angle.y = (s16)(pShip->shape_angle.y - 0x8000);
        return true;
    }
    return false;
}
VERIFY(0x0230473C, &daNpc_Zl1_c::swoon_OnShip);

/* rippleEcallBack (HD 0x14): emitter at +4, rate at +0x10 */
static inline u32 ripple_ea(daNpc_Zl1_c* a) { return gabi::ea(a->mRippleCallBack); }

/* 02304824 */
void daNpc_Zl1_c::setWaterRipple() {
    WWHD_FUNC(0x02304824, void, this);
    if (!field_0x7CA) {
        /* HD: tests field_0x7C8 (the water flag _execute stored) instead of mObjAcch.ChkWaterIn() */
        if (field_0x7C8) {
            if (gabi::load<u32>(ripple_ea(this) + 4) == 0) { /* mRippleCallBack.getEmitter() == NULL */
                /* dComIfGp_particle_setShipTail(dPa_name::ID_AK_JN_HAMON00, &current.pos, NULL, NULL, 0xFF, &mRippleCallBack) */
                dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &current.pos, nullptr, nullptr, 0xFF,
                                (dPa_levelEcallBack*)(void*)mRippleCallBack, -1, nullptr, nullptr, nullptr);
            }
            f32 temp = std::fabs((f32)speedF) * 0.1f;
            f32 temp2 = temp * temp;
            temp2 = (temp2 - 1.0f >= 0.0f) ? 1.0f : temp2; /* cLib_maxLimit (fsel) */
            gabi::store<f32>(ripple_ea(this) + 0x10, temp2); /* mRippleCallBack.setRate() */
        } else {
            dPa_rippleEcallBack_end((dPa_rippleEcallBack*)(void*)mRippleCallBack);
        }
    }
}
VERIFY(0x02304824, &daNpc_Zl1_c::setWaterRipple);

/* 025A8BFC dPa_control_c::setSimpleLand(polyInfo, pos, angle, size, scale, rate, tevStr, attr*, type) */
static inline u32 dComIfGp_particle_setSimpleLand(void* poly, cXyz* pos, csXyz* angle, f32 size, f32 scale, f32 rate,
                                                 dKy_tevstr_c* tev, be<s32>* attr, s32 type) {
    return gabi::call<u32>(0x025A8BFC, dComIfGp_getParticle(), poly, pos, angle, size, scale, rate, tev, attr, type);
}

/* 023048F4 */
void daNpc_Zl1_c::setWaterSplash() {
    WWHD_FUNC(0x023048F4, void, this);
    gabi::Local<be<s32>> temp;
    u32 pEmitter = dComIfGp_particle_setSimpleLand(daNpc_gndPoly(this), &field_0x77C, &field_0x744, 1.25f, 0.65f, 1.0f,
                                                   &tevStr, temp, 6);
    if (pEmitter != 0) {
        gabi::store<u16>(pEmitter + 0x60, 0xF);   /* setLifeTime(0xF) */
        gabi::store<f32>(pEmitter + 0x70, 11.0f); /* setDirectionalSpeed(11.0f) */
        gabi::store<f32>(pEmitter + 0x34, 4.0f);  /* setRate(4.0f) */
        gabi::store<f32>(pEmitter + 0x58, 0.6f);  /* setSpread(0.6f) */
    }
}
VERIFY(0x023048F4, &daNpc_Zl1_c::setWaterSplash);

/* 0230498C */
void daNpc_Zl1_c::set_simpleLand(u32 param_1) {
    WWHD_FUNC(0x0230498C, void, this, param_1);
    gabi::Local<be<s32>> temp;
    s32 temp2 = 6;
    if ((u8)param_1) {
        temp2 = 7;
    }
    u32 pEmitter = dComIfGp_particle_setSimpleLand(daNpc_gndPoly(this), &field_0x77C, &field_0x744, 1.25f, 0.65f, 0.6f,
                                                   &tevStr, temp, temp2);
    if (pEmitter != 0) {
        switch ((u32)*temp) {
        case 0x24:
            gabi::store<f32>(pEmitter + 0x58, 1.0f);  /* setSpread(1.0f) */
            gabi::store<u16>(pEmitter + 0x60, 0xF);   /* setLifeTime(0xF) */
            gabi::store<f32>(pEmitter + 0x34, 10.0f); /* setRate(10.0f) */
            break;
        case 0x23:
            gabi::store<f32>(pEmitter + 0x58, 1.0f);  /* setSpread(1.0f) */
            gabi::store<f32>(pEmitter + 0x34, 18.0f); /* setRate(18.0f) */
            break;
        }
    }
}
VERIFY(0x0230498C, &daNpc_Zl1_c::set_simpleLand);

static inline s32 dBgS_GetAttributeCode(dBgS* bgs, void* poly) { return gabi::call<s32>(0x024EF0F4, bgs, poly); }

/* 02304A6C */
void daNpc_Zl1_c::setEff() {
    WWHD_FUNC(0x02304A6C, void, this);
    if (field_0x849 == 0xB || field_0x7C6) {
        /* HD: tests field_0x7C8 (the water flag _execute stored) instead of mObjAcch.ChkWaterIn() */
        if (field_0x7C8) {
            if (!field_0x7C6) {
                s16 ang = shape_angle.y;
                field_0x77C.x = gabi::fmadds(40.0f, cM_ssin(ang), current.pos.x);
                field_0x77C.z = gabi::fmadds(40.0f, cM_scos(ang), current.pos.z);
                field_0x744.z = 0;
                field_0x744.x = 0;
                field_0x744.y = (s16)(ang - 0x8000);
            }
            field_0x77C.y = gabi::load<f32>(gabi::ea(&mObjAcch) + 0x1BC); /* mObjAcch.m_wtr.GetHeight() */
            if (dBgS_GetAttributeCode(dComIfG_Bgsp(), daNpc_gndPoly(this)) == 0x13) {
                f32 y = current.pos.y + 40.0f;
                if (gabi::load<f32>(gabi::ea(&mObjAcch) + 0x1BC) > y) {
                    setWaterSplash();
                }
            }
        } else {
            if (!field_0x7C6) {
                gabi::Local<csXyz> temp;
                csXyz* t = gabi::call<csXyz*>(0x0201A478, temp.get(), (s16)current.angle.y, (s16)0, (s16)0); /* csXyz(angle.y, 0, 0) */
                field_0x744.x = t->x;
                field_0x744.y = t->y;
                field_0x744.z = t->z;
                field_0x77C.copy(current.pos);
            }
        }
        if (mObjAcch.ChkGroundHit() &&
            (mpMorf->mFrameCtrl.checkPass(13.0f) || mpMorf->mFrameCtrl.checkPass(27.0f) || field_0x7C6)) {
            set_simpleLand(field_0x7C6);
            field_0x7C6 = false;
        }
    }
}
VERIFY(0x02304A6C, &daNpc_Zl1_c::setEff);

/* HD: sead::SafeString equality of two C strings (both through the per-TU SafeString vtable;
 * at most 0x40001 characters compared) */
static bool safestring_equal(u32 str1, u32 str2) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> b;
    a->__vtbl = ZL1_SAFESTRING_VTBL;
    b->__vtbl = ZL1_SAFESTRING_VTBL;
    a->mStringTop = str1;
    b->mStringTop = str2;
    gabi::call(0x0230A308, a.get()); /* sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b.get());
    u32 s2 = b->mStringTop;
    if (s1 == s2)
        return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c1 = gabi::load<u8>(s1 + i);
        u8 c2 = gabi::load<u8>(s2 + i);
        if (c1 != c2)
            return false;
        if (c1 == 0)
            return true;
    }
    return false;
}

/* 02304C98 */
void daNpc_Zl1_c::setMtx(u32 param_1) {
    WWHD_FUNC(0x02304C98, void, this, param_1);
    bool temp = false;
    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    Mtx34* stk = mDoMtx_stack_c::get();
    if (field_0x7CE) {
        if (!gabi::call<BOOL>(0x025DD868, (u32)mProcId1) /* fpcM_IsCreating */ && mProcId1 != 0xFFFFFFFF) {
            gabi::Local<gptr<fopAc_ac_c>> actor;
            if (gabi::call<s32>(0x025D54C4, (u32)mProcId1, actor.get()) == 1 /* fopAcM_SearchByID */ && actor->get() != nullptr) {
                temp = true;
                /* daBranch_c::getJointMtx("wood2") */
                PSMTXCopy(gabi::call<Mtx34*>(0x020DF7A8, actor->get(), STR(0x10024008)), stk);
                mDoMtx_stack_c::transM(120.28999f, -88.36f, -19.130001f);
            }
        }
    } else if (field_0x7CA) {
        temp = swoon_OnShip();
    }
    if (!temp) {
        PSMTXTrans(stk, current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_ZXYrotM(stk, field_0x73E.x, field_0x73E.y, field_0x73E.z);
    }
    J3DModel_setBaseTRMtx(mpMorf->getModel(), stk);
    mpMorf->calc();
    if (mpModel.get() != nullptr) {
        J3DModel_setBaseTRMtx(mpModel, getAnmMtx(mpMorf->getModel(), m_bbone_jnt_num));
        J3DModel_calc(mpModel);
    }
    if (!field_0x7D9) {
        setWaterRipple();
        setEff();
    } else if (safestring_equal(0x10024010 /* "Demo37" */, 0x1047E6B8 /* start stage name */)) {
        /* HD: in the demo stage "Demo37" the ripple runs with the water flag set */
        field_0x7C8 = 1;
        setWaterRipple();
    }
    setAttention(param_1);
}
VERIFY(0x02304C98, &daNpc_Zl1_c::setMtx);

/* dBgS_LinChk (stack object, HD layout; this TU's vtables) */
struct dBgS_LinChk_l {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x58 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x64 */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 mPolyInfo[4];       /* cBgS_PolyInfo: poly index, bg index */
    /* 0x18 */ u8 _18[0x20 - 0x18];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ u8 _24[0x58 - 0x24];
    /* 0x58 */ be<u32> __vtbl_58;     /* dBgS_PolyPassChk */
    /* 0x5C */ be<u8> mPass[7];
    /* 0x63 */ u8 _63;
    /* 0x64 */ be<u32> __vtbl_64;     /* dBgS_GrpPassChk */
    /* 0x68 */ be<u32> mGrp;
};
WWHD_SIZE(dBgS_LinChk_l, 0x6C);

/* dBgS_ObjRoofChk (stack object, 0x4C): dBgS_RoofChk base constructor 024EE7AC, then the
 * derived vtables; the dBgS_Chk part (destructor 02008B4C) at +0x10 */
struct dBgS_ObjRoofChk_l {
    /* 0x00 */ u8 _00[0xC];
    /* 0x0C */ be<u32> __vtbl_0C;
    /* 0x10 */ u8 _10[0x20 - 0x10];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ be<u32> __vtbl_24;
    /* 0x28 */ be<u8> m28;
    /* 0x29 */ u8 _29[0x30 - 0x29];
    /* 0x30 */ be<u32> __vtbl_30;
    /* 0x34 */ u8 _34[4];
    /* 0x38 */ cXyz mPos;
    /* 0x44 */ u8 _44[8];
};
WWHD_SIZE(dBgS_ObjRoofChk_l, 0x4C);

/* 02308EC4 (the matcher's name; GameCube source) */
BOOL daNpc_Zl1_c::setFrontWallType() {
    WWHD_FUNC(0x02308EC4, BOOL, this);
    s16 ang = shape_angle.y;
    f32 sin = cM_ssin(ang);
    f32 cos = cM_scos(ang);

    gabi::Local<dBgS_LinChk_l> linChk;
    cBgS_LinChk_ct(linChk);
    for (int i = 0; i < 7; i++) linChk->mPass[i] = 0;
    linChk->__vtbl_58 = 0x10023C08;
    linChk->__vtbl_10 = 0x10023BD8;
    linChk->mpPolyPassChk = gabi::ea(linChk.get()) + 0x58;
    linChk->mpGrpPassChk = gabi::ea(linChk.get()) + 0x64;
    linChk->__vtbl_20 = 0x10023BE8;
    linChk->__vtbl_64 = 0x10023BF8;
    linChk->mGrp = 1;

    gabi::Local<dBgS_ObjRoofChk_l> roofChk;
    gabi::call(0x024EE7AC, roofChk.get()); /* dBgS_RoofChk::dBgS_RoofChk */
    roofChk->__vtbl_0C = 0x10023B78;
    roofChk->m28 = 1;
    roofChk->__vtbl_30 = 0x10023B98;
    roofChk->__vtbl_24 = 0x10023BA8;
    roofChk->__vtbl_20 = 0x10023B88;

    u32 cir = gabi::ea(&mAcchCir);
    f32 wallH = gabi::load<f32>(cir + 0x30); /* mAcchCir.GetWallH() */
    f32 r = gabi::load<f32>(cir + 0x34) + 25.0f; /* mAcchCir.GetWallR() + 25.0f */
    gabi::Local<cXyz> sp14;
    gabi::Local<cXyz> sp08;
    f32 x = current.pos.x, z = current.pos.z;
    f32 y = current.pos.y + wallH;
    sp14->x = x;
    sp08->x = gabi::fmadds(sin, r, x);
    sp14->z = z;
    sp08->z = gabi::fmadds(cos, r, z);
    sp08->y = y;
    sp14->y = y;
    dBgS_LinChk_Set(linChk, sp14, sp08, this);

    BOOL ret;
    if (!cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
        ret = TRUE; /* return FALSE */
    } else {
        /* HD: the plane is checked for NULL */
        u8* plane = (u8*)dBgS_GetTriPla(dComIfG_Bgsp(), linChk->mPolyInfo);
        if (plane == nullptr || std::fabs(gabi::load<f32>(gabi::ea(plane) + 4)) > 0.05f) {
            ret = TRUE; /* return FALSE */
        } else {
            f32 r2 = gabi::load<f32>(cir + 0x34) + 25.0f;
            f32 x2 = current.pos.x, z2 = current.pos.z;
            f32 y2 = current.pos.y + 70.1f; /* 70.0f + 0.1f */
            sp14->x = x2;
            sp08->y = y2;
            sp14->z = z2;
            sp14->y = y2;
            sp08->z = gabi::fmadds(cos, r2, z2);
            sp08->x = gabi::fmadds(sin, r2, x2);
            dBgS_LinChk_Set(linChk, sp14, sp08, this);
            ret = cBgS_LineCross(dComIfG_Bgsp(), linChk);
            if (!ret) {
                roofChk->mPos.copy(current.pos); /* roofChk.SetPos(current.pos) */
                f32 h = gabi::call<f32>(0x024EF6E8, dComIfG_Bgsp(), roofChk.get()); /* dBgS::RoofChk */
                ret = (h - current.pos.y > 80.0f) ? FALSE : TRUE;
            }
        }
    }
    /* ~dBgS_ObjRoofChk, ~dBgS_LinChk (inline: this TU's vtables, then the dBgS_Chk destructor) */
    roofChk->__vtbl_20 = 0x10023B48;
    roofChk->__vtbl_24 = 0x10023B68;
    roofChk->__vtbl_30 = 0x10023B38;
    gabi::call(0x02008B4C, gabi::ea(roofChk.get()) + 0x10, 0);
    linChk->__vtbl_20 = 0x10023B28;
    linChk->__vtbl_64 = 0x10023B38;
    linChk->__vtbl_58 = 0x10023C08;
    cBgS_LinChk_dt(linChk, 0);
    return ret == FALSE;
}
VERIFY(0x02308EC4, &daNpc_Zl1_c::setFrontWallType);

/* ---- _draw helpers (HD) ---- */
/* j3dSys opa/xlu draw buffers (0x104B4634/0x104B4638) from two play draw lists */
static inline void j3dSys_setDrawBuffers(u32 opaOff, u32 xluOff) {
    gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + opaOff));
    gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + xluOff));
}
/* packet->entryOpa(): J3DDrawBuffer::entryImm(j3dSys opa buffer, packet, 0) */
static inline void packet_entryOpa(void* packet) {
    gabi::call(0x027F0E04, gabi::load<u32>(0x104B4634), packet, 0);
}
/* 027F583C HD: J3DJoint::entryIn as (J3DModel*, J3DJoint*) */
static inline void J3DJoint_entryIn(J3DModel* model, J3DJoint* joint) { gabi::call(0x027F583C, model, joint); }
/* J3DShape show/hide (HD: a visibility byte at +4) */
static inline void shape_vis(u32 shape, u8 v) { gabi::store<u8>(shape + 4, v); }
/* 025E7BE4 HD: mDoExt_btpAnm::entry(J3DModel*, f32 frame) */
static inline void mDoExt_btpAnm_entry_l(void* anm, J3DModel* model, f32 frame) { gabi::call(0x025E7BE4, anm, model, frame); }
/* 025BEBB8 HD: dSnap_RegistFig(type, actor, const cXyz& pos, s16 angleY, f32, f32, f32) */
static inline void dSnap_RegistFig_l(s32 type, fopAc_ac_c* a, cXyz* pos, s16 angY, f32 p1, f32 p2, f32 p3) {
    gabi::call(0x025BEBB8, type, a, pos, angY, p1, p2, p3);
}
/* static initialisation of a 4-byte function-local constant on first use */
static inline void local_static_init(u32 guard, u32 dst, u32 src) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        memcpy_g(gabi::at<u8>(dst), gabi::at<u8>(src), 4); /* 028FEAC0 memcpy */
    }
}
/* the materials of a joint's mesh: show (v) the shapes whose material index is mB0C/mB10 (HD: by
 * material index; GameCube: the first two materials), the others !v */
static inline void mesh_vis(daNpc_Zl1_c* t, u8 v) {
    u32 mat = gabi::load<u32>(gabi::ea(t->mJoint1.get()) + 0x10); /* mJoint1->getMesh() */
    while (mat != 0) {
        u32 idx = gabi::load<u16>(gabi::load<u32>(mat) + 0xC);
        if (idx == (u32)(s32)t->mB0C || idx == (u32)(s32)t->mB10) {
            shape_vis(gabi::load<u32>(mat + 8), v);
        } else {
            shape_vis(gabi::load<u32>(mat + 8), !v);
        }
        mat = gabi::load<u32>(mat + 4); /* getNext() */
    }
}

/* 02308090 */
BOOL daNpc_Zl1_c::_draw() {
    WWHD_FUNC(0x02308090, BOOL, this);
    J3DModel* pModel = mpMorf->getModel();
    J3DModelData* modelData = J3DModel_getModelData_l(pModel);
    if (field_0x7D2 || field_0x7D4) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), pModel, &tevStr);
    dComIfGp_get(); /* HD: an unused accessor call */
    mpMorf->calc();
    /* HD: no pModel->unlock() and no material animation sharing (field_0x860/878) */
    j3dSys_setDrawBuffers(0x5D64, 0x5D64); /* dComIfGd_setListP0() */
    packet_entryOpa(mOnCupOffAupPacket2);

    shape_vis(field_0x890[2], 0);
    shape_vis(field_0x890[5], 0);
    shape_vis(field_0x890[1], 0);
    shape_vis(field_0x890[4], 0);
    shape_vis(field_0x890[0], 1);
    shape_vis(field_0x890[3], 1);
    J3DJoint_entryIn(pModel, mJoint2);
    shape_vis(field_0x8A8[2], 0);
    shape_vis(field_0x8A8[5], 0);
    shape_vis(field_0x8A8[1], 0);
    shape_vis(field_0x8A8[4], 0);
    shape_vis(field_0x8A8[0], 1);
    shape_vis(field_0x8A8[3], 1);
    J3DJoint_entryIn(pModel, mJoint3);
    packet_entryOpa(mOffCupOnAupPacket2);

    mDoExt_btkAnm_entry((mDoExt_btkAnm*)(void*)mBtkAnm, modelData, (f32)(u32)mBtkAnmFrame);
    shape_vis(field_0x890[0], 0);
    shape_vis(field_0x890[3], 0);
    shape_vis(field_0x890[2], 1);
    shape_vis(field_0x890[5], 1);
    J3DJoint_entryIn(pModel, mJoint2);
    shape_vis(field_0x8A8[0], 0);
    shape_vis(field_0x8A8[3], 0);
    shape_vis(field_0x8A8[2], 1);
    shape_vis(field_0x8A8[5], 1);
    J3DJoint_entryIn(pModel, mJoint3);
    /* HD: mBtkAnm.remove() moved to the end */

    mesh_vis(this, 1);
    J3DJoint_entryIn(pModel, mJoint1);
    mesh_vis(this, 0);

    j3dSys_setDrawBuffers(0x5D6C, 0x5D6C); /* dComIfGd_setListP1() */
    packet_entryOpa(mOnCupOffAupPacket1);
    shape_vis(field_0x890[1], 1);
    shape_vis(field_0x890[4], 1);
    shape_vis(field_0x890[2], 0);
    shape_vis(field_0x890[5], 0);
    J3DJoint_entryIn(pModel, mJoint2);
    shape_vis(field_0x8A8[1], 1);
    shape_vis(field_0x8A8[4], 1);
    shape_vis(field_0x8A8[2], 0);
    shape_vis(field_0x8A8[5], 0);
    J3DJoint_entryIn(pModel, mJoint3);
    packet_entryOpa(mOffCupOnAupPacket1);
    shape_vis(field_0x890[1], 0);
    shape_vis(field_0x890[4], 0);
    shape_vis(field_0x8A8[1], 0);
    shape_vis(field_0x8A8[4], 0);

    j3dSys_setDrawBuffers(0x5D68, 0x5D60); /* HD: another draw list pair */
    {
        u32 play = dComIfGp_ea();
        gabi::store<u32>(play + 0x5D3C, gabi::load<u32>(gabi::ea(&current.pos) + 0));
        gabi::store<u32>(play + 0x5D40, gabi::load<u32>(gabi::ea(&current.pos) + 4));
        gabi::store<u32>(play + 0x5D44, gabi::load<u32>(gabi::ea(&current.pos) + 8));
        gabi::call(0x0252F4E0, play + 0x5D30); /* HD: draw list (play+0x5D30) call with the position */
    }
    gabi::call(0x025E5580, mpMorf.get()); /* mpMorf->entry() */
    dComIfGd_setList();
    /* HD: the btp animations (two) are entered last, with the model */
    mDoExt_btpAnm_entry_l(mBtpAnm, pModel, (f32)(u32)mBtpAnmFrame);
    mDoExt_btpAnm_entry_l(mBtpAnm2, pModel, (f32)(u32)mBtpAnmFrame);
    gabi::store<u32>(gabi::ea(modelData) + 0x44, 0); /* mBtkAnm.remove(modelData) */
    gabi::store<u32>(gabi::ea(modelData) + 0x38, 0); /* mBtpAnm.remove(modelData) */
    if (mpModel.get() != nullptr) {
        setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
        mDoExt_modelEntryDL(mpModel);
    }
    /* HD: no shadowDraw(); the snap figure is registered at the branch joint while hanging */
    gabi::Local<cXyz> pos;
    if (field_0x7CE) {
        gabi::Local<Mtx34> mtx;
        PSMTXCopy(J3DModel_getBaseTRMtx(mpMorf->getModel()), mtx);
        pos->x = mtx->m[0][3];
        pos->y = mtx->m[1][3];
        pos->z = mtx->m[2][3];
        dSnap_RegistFig_l(0x72 /* DSNAP_TYPE_NPC_ZL1 */, this, pos, current.angle.y, 0.5f, 1.2f, 1.0f);
    } else {
        pos->copy(current.pos);
        dSnap_RegistFig_l(0x72 /* DSNAP_TYPE_NPC_ZL1 */, this, pos, current.angle.y, 1.0f, 1.0f, 1.0f);
    }
    /* debug leftovers: function-local static colours initialised on first use */
    if (l_HIO().mPrmTbl.field_20 != 0) {
        if (field_0x84F == 3) {
            local_static_init(0x101FDA48, 0x101FEBEC, 0x10023AF8);
            dComIfGp_get();
            local_static_init(0x101FDA44, 0x101FEBE8, 0x10023AFC);
            local_static_init(0x101FDA48, 0x101FEBEC, 0x10023AF8);
        }
        local_static_init(0x101FDA50, 0x101FEBF4, 0x10023B00);
        local_static_init(0x101FDAC0, 0x101FEBF8, 0x10023B04);
        local_static_init(0x101FDA44, 0x101FEBE8, 0x10023AFC);
        local_static_init(0x101FDA50, 0x101FEBF4, 0x10023B00);
    }
    return TRUE;
}
VERIFY(0x02308090, &daNpc_Zl1_c::_draw);
