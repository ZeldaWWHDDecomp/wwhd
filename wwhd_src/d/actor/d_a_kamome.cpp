/**
 * d_a_kamome.cpp (WWHD)
 * NPC - Seagull
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kamome.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_kamome.h"

/* guest string literals: GHS keeps one copy per use */
#define STR(addr) gabi::at<const char>(addr)
#define SAFESTRING_VTBL 0x10012304 /* this TU's sead::SafeString vtable */

enum {
    dRes_INDEX_KAMOME_BAS_KA_FLY1_e = 0x5,
    dRes_INDEX_KAMOME_BAS_KA_LAND1_e = 0x6,
    dRes_INDEX_KAMOME_BAS_KA_MOVE1_e = 0x7,
    dRes_INDEX_KAMOME_BAS_KA_WAIT1_e = 0x8,
    dRes_INDEX_KAMOME_BAS_KA_WAIT2_e = 0x9,
    dRes_INDEX_KAMOME_BCK_KA_EAT1_e = 0xC,
    dRes_INDEX_KAMOME_BCK_KA_FLY1_e = 0xD,
    dRes_INDEX_KAMOME_BCK_KA_LAND1_e = 0xE,
    dRes_INDEX_KAMOME_BCK_KA_MOVE1_e = 0xF,
    dRes_INDEX_KAMOME_BCK_KA_WAIT1_e = 0x12,
    dRes_INDEX_KAMOME_BCK_KA_WAIT2_e = 0x13,
    dRes_INDEX_KAMOME_BDL_KA_e = 0x17,
};
enum { fpcNm_KAMOME_e = 0xC2, fpcNm_ESA_e = 0xDD, fpcNm_NPC_LS1_e = 0x141 };
enum { J3DFrameCtrl_EMode_LOOP = 2 };
enum { DSNAP_TYPE_KAMOME = 0x55 };

/* ---- file statics (.bss) ---- */
struct kamomeHIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> m05;
    /* 0x02 */ be<s16> m06;
    /* 0x04 */ be<s16> m08;
    /* 0x06 */ u8 _06[2];
    /* 0x08 */ be<f32> m0C;
    /* 0x0C */ be<f32> m10;
    /* 0x10 */ be<u32> __vtbl; /* GHS: vtable pointer after the members */
};
static kamomeHIO_c& l_kamomeHIO() { return *gabi::at<kamomeHIO_c>(0x104648C8); }
static be<u8>& hio_set() { return *gabi::at<be<u8>>(0x101B7C00); }
static be<s32>& esa_check_count() { return *gabi::at<be<s32>>(0x104648A4); }
static be<s32>& ko_count() { return *gabi::at<be<s32>>(0x104648A8); }
static gptr<fopAc_ac_c>* esa_info() { return gabi::at<gptr<fopAc_ac_c>>(0x104648DC); } /* [100] */

/* esa_class (bait): the fields read here */
struct esa_class {
    fopAc_ac_c actor;
    /* 0x3AC */ u8 _3AC[8];
    /* 0x3B4 */ be<u8> field_0x298;
    /* 0x3B5 */ u8 _3B5[0x3BD - 0x3B5];
    /* 0x3BD */ be<s8> mState;
};
WWHD_OFFSET(esa_class, mState, 0x3BD);

/* 02185558 */
static void anm_init(kamome_class* i_this, int anmResIdx, float morf, unsigned char loopMode, float speed, int soundAnmResIdx) {
    WWHD_FUNC(0x02185558, void, i_this, anmResIdx, morf, loopMode, speed, soundAnmResIdx);
    if (REG0_S(3) == 0x23) {
        anmResIdx = dRes_INDEX_KAMOME_BCK_KA_LAND1_e;
    } else if (REG0_S(3) == 0x24) {
        anmResIdx = dRes_INDEX_KAMOME_BCK_KA_FLY1_e;
    }
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10012414), anmResIdx, SAFESTRING_VTBL);
    void* sound = dComIfG_getObjectRes(STR(0x10012414), soundAnmResIdx, SAFESTRING_VTBL);
    McaMorf_setAnm(i_this->mpMorf, anm, loopMode, morf, speed, 0.0f, -1.0f, sound);
}
VERIFY(0x02185558, anm_init);

/* dBgS_LinChk (stack object): HD layout, sub-object vtables after the members */
struct dBgS_LinChk_l {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x58 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x64 */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ u8 _24[0x58 - 0x24];
    /* 0x58 */ be<u32> __vtbl_58;     /* dBgS_PolyPassChk */
    /* 0x5C */ be<u8> mPass[7];
    /* 0x63 */ u8 _63;
    /* 0x64 */ be<u32> __vtbl_64;     /* dBgS_GrpPassChk */
    /* 0x68 */ be<u32> mGrp;
};
WWHD_SIZE(dBgS_LinChk_l, 0x6C);

static void dBgS_LinChk_ct(dBgS_LinChk_l* c) {
    cBgS_LinChk_ct(c);
    for (int i = 0; i < 7; i++) c->mPass[i] = 0;
    c->mGrp = 1;
    c->mpGrpPassChk = gabi::ea(c) + 0x64;
    c->mpPolyPassChk = gabi::ea(c) + 0x58;
    c->__vtbl_10 = 0x100123AC;
    c->__vtbl_64 = 0x100123CC;
    c->__vtbl_58 = 0x100123DC;
    c->__vtbl_20 = 0x100123BC;
}
static void dBgS_LinChk_dt(dBgS_LinChk_l* c) {
    c->__vtbl_58 = 0x100123DC;
    c->__vtbl_64 = 0x1001232C;
    c->__vtbl_20 = 0x1001231C;
    cBgS_LinChk_dt(c, 0);
}

/* 0218566C */
static void* s_a_d_sub(void* ac1, void* ac2) {
    WWHD_FUNC(0x0218566C, void*, ac1, ac2);
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk);
    fopAc_ac_c* actor2 = (fopAc_ac_c*)ac2;

    /* HD: also requires the bait's mState == 1 */
    if (esa_check_count() < 100 && fopAc_IsActor(ac1) && ac1 != nullptr && fpcM_GetName(ac1) == fpcNm_ESA_e) {
        esa_class* esa = (esa_class*)ac1;
        if (esa->field_0x298 == 0 && esa->mState == 1) {
            gabi::Local<cXyz> sp14;
            gabi::Local<cXyz> sp08;

            Vec3f p = esa->actor.current.pos.get();
            p.y += 10.0f;
            *sp08 = p;
            sp14->copy(actor2->current.pos);

            dBgS_LinChk_Set(linChk, sp14, sp08, actor2);
            if (!cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
                s32 n = esa_check_count();
                esa_check_count() = n + 1;
                esa_info()[n] = &esa->actor;
            }
        }
    }
    dBgS_LinChk_dt(linChk);
    return nullptr;
}
VERIFY(0x0218566C, s_a_d_sub);

/* 02185808 */
static fopAc_ac_c* search_esa(kamome_class* i_this) {
    WWHD_FUNC(0x02185808, fopAc_ac_c*, i_this);
    esa_check_count() = 0;
    fpcM_Search(0x0218566C /* s_a_d_sub */, &i_this->actor);

    if (esa_check_count() != 0) {
        f32 fVar3 = 50.0f;
        s32 i = 0;
        while (i < esa_check_count()) {
            esa_class* esa = (esa_class*)(fopAc_ac_c*)esa_info()[i];
            f32 x = esa->actor.current.pos.x - i_this->actor.current.pos.x;
            f32 z = esa->actor.current.pos.z - i_this->actor.current.pos.z;
            if (std_sqrtf(gabi::fmadds(x, x, z * z)) < fVar3) {
                esa->field_0x298 = 1;
                return &esa->actor;
            }

            i++;
            if (i == esa_check_count()) {
                i = 0;
                fVar3 += 50.0f;
                if (fVar3 > 10000.0f) {
                    return nullptr;
                }
            }
        }
    }
    return nullptr;
}
VERIFY(0x02185808, search_esa);

/* 02185960 */
static void* s_a_i_sub(void* ac1, void* ac2) {
    WWHD_FUNC(0x02185960, void*, ac1, ac2);
    if (fopAc_IsActor(ac1) && ac1 != nullptr && fpcM_GetName(ac1) == fpcNm_NPC_LS1_e) {
        return ac1;
    }
    return nullptr;
}
VERIFY(0x02185960, s_a_i_sub);

/* HD J3D: j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at 0x104B4868; a model's joint
 * matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices) */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
struct J3DModel_l {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<J3DMtxBlock_l> mpMtxBlock;
    /* 0x30 */ u8 _30[0xAC - 0x30];
    /* 0xAC */ be<u32> mModelData;
    /* 0xB0 */ u8 _B0[8];
    /* 0xB8 */ be<u32> mUserArea;
};
static Mtx34* getAnmMtx(J3DModel_l* model, s32 jntNo) {
    J3DMtxBlock_l* blk = model->mpMtxBlock;
    blk->mFlags |= 0x10; /* HD: marks the joint matrices dirty */
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30);
}

/* 021859B0 */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x021859B0, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel_l* model = gabi::at<J3DModel_l>(gabi::load<u32>(0x104B462C));
        kamome_class* pKamome = gabi::at<kamome_class>(model->mUserArea);
        s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (pKamome != nullptr) {
            PSMTXCopy(getAnmMtx(model, jntNo), calc_mtx());
            cMtx_YrotM(calc_mtx(), pKamome->mJointRotY);
            cMtx_ZrotM(calc_mtx(), pKamome->mJointRotZ);
            /* model->setAnmMtx(jntNo, *calc_mtx) */
            Mtx34* dst = getAnmMtx(model, jntNo);
            mtx_copy(dst, calc_mtx());
            PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868));
        }
    }
    return TRUE;
}
VERIFY(0x021859B0, nodeCallBack);

/* 02185AE0 */
static BOOL daKamome_Draw(kamome_class* i_this) {
    WWHD_FUNC(0x02185AE0, BOOL, i_this);
    /* HD: based on the USA version (mbNoDraw is reset in Execute) */
    if ((i_this->mSwitchNo != 0) || (i_this->mbNoDraw != 0)) {
        return TRUE;
    }

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->actor.current.pos, &i_this->actor.tevStr);
    mDoExt_McaMorf_c* morf = i_this->mpMorf;
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, McaMorf_getModel(morf), &i_this->actor.tevStr);
    McaMorf_entryDL(i_this->mpMorf);

    /* HD: no blob shadow (dComIfGd_setShadow) */

    dSnap_RegistFig(DSNAP_TYPE_KAMOME, &i_this->actor, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x02185AE0, daKamome_Draw);

/* 02185B78 */
static void kamome_pos_move(kamome_class* i_this) {
    WWHD_FUNC(0x02185B78, void, i_this);
    f32 fVar4 = i_this->mTargetPos.x - i_this->actor.current.pos.x;
    f32 fVar5 = i_this->mTargetPos.z - i_this->actor.current.pos.z;
    f32 fVar1 = i_this->mTargetPos.y - i_this->actor.current.pos.y;
    s16 rotTargetY = cM_atan2s(fVar4, fVar5);
    s16 rotTargetX = -cM_atan2s(fVar1, std_sqrtf(gabi::fmadds(fVar4, fVar4, fVar5 * fVar5)));
    s16 sVar3 = i_this->actor.current.angle.y;
    cLib_addCalcAngleS2(&i_this->actor.current.angle.y, rotTargetY, REG0_S(3) + 10,
                        gabi::ftoi(i_this->mRotVel * i_this->mRotVelFade));
    sVar3 = (sVar3 - i_this->actor.current.angle.y) * 0x20;

    s16 rotTargetZ = REG0_S(1) + 5500;
    if (sVar3 > rotTargetZ) {
        sVar3 = rotTargetZ;
    } else if (sVar3 < -rotTargetZ) {
        sVar3 = -rotTargetZ;
    }

    cLib_addCalcAngleS2(&i_this->actor.current.angle.z, sVar3, REG0_S(3) + 10,
                        gabi::ftoi(i_this->mRotVel * i_this->mRotVelFade * 0.5f));
    cLib_addCalcAngleS2(&i_this->actor.current.angle.x, rotTargetX, REG0_S(3) + 10,
                        gabi::ftoi(i_this->mRotVel * i_this->mRotVelFade));
    /* HD: the target is REG18_F(0) + 1.0f (GameCube: 1.0f) */
    cLib_addCalc2(&i_this->mRotVelFade, REG_F(18, 0) + 1.0f, 1.0f, 0.04f);
    cLib_addCalc2(&i_this->actor.speedF, i_this->mVelocityFwdTarget, 1.0f, i_this->mVelocityFwdTargetMaxVel);
    gabi::Local<cXyz> fwd;
    fwd->x = 0.0f;
    fwd->y = 0.0f;
    fwd->z = i_this->actor.speedF;
    cMtx_YrotS(calc_mtx(), i_this->actor.current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->actor.current.angle.x);
    MtxPosition(fwd, &i_this->actor.speed);
    i_this->actor.current.pos.x += i_this->actor.speed.x;
    i_this->actor.current.pos.y += i_this->actor.speed.y;
    i_this->actor.current.pos.z += i_this->actor.speed.z;

    if (i_this->mRiseTimer != 0) {
        i_this->mRiseTimer--;
        i_this->actor.current.pos.y += 5.0f;
    }
}
VERIFY(0x02185B78, kamome_pos_move);

/* 02185DF0 */
static void kamome_bgcheck(kamome_class* i_this) {
    WWHD_FUNC(0x02185DF0, void, i_this);
    f32 fVar1 = i_this->mScale * 40.0f;
    i_this->actor.current.pos.y = i_this->actor.current.pos.y - fVar1;
    i_this->actor.old.pos.y = i_this->actor.old.pos.y - fVar1;
    dBgS_Acch_CrrPos(&i_this->mAcch, dComIfG_Bgsp());
    i_this->actor.current.pos.y = i_this->actor.current.pos.y + fVar1;
    i_this->actor.old.pos.y = i_this->actor.old.pos.y + fVar1;
}
VERIFY(0x02185DF0, kamome_bgcheck);

/* 02185E80 */
static void kamome_ground_pos_move(kamome_class* i_this) {
    WWHD_FUNC(0x02185E80, void, i_this);
    s16 iVar2 = cM_atan2s(i_this->mTargetPos.x - i_this->actor.current.pos.x, i_this->mTargetPos.z - i_this->actor.current.pos.z);

    cLib_addCalcAngleS2(&i_this->actor.current.angle.y, iVar2, REG0_S(3) + 2, gabi::ftoi(i_this->mRotVel * i_this->mRotVelFade));
    cLib_addCalc2(&i_this->mRotVelFade, 1.0f, 1.0f, 0.1f);
    cLib_addCalc2(&i_this->actor.speedF, i_this->mVelocityFwdTarget, 1.0f, i_this->mVelocityFwdTargetMaxVel);

    gabi::Local<cXyz> sp14;
    sp14->x = 0.0f;
    sp14->y = 0.0f;
    sp14->z = i_this->actor.speedF;
    cMtx_YrotS(calc_mtx(), i_this->actor.current.angle.y);

    gabi::Local<cXyz> sp08;
    MtxPosition(sp14, sp08);
    i_this->actor.speed.x = sp08->x;
    i_this->actor.speed.z = sp08->z;

    i_this->actor.current.pos.x += i_this->actor.speed.x;
    i_this->actor.current.pos.y += i_this->actor.speed.y;
    i_this->actor.current.pos.z += i_this->actor.speed.z;

    i_this->actor.speed.y -= 3.0f;
    kamome_bgcheck(i_this);

    if (i_this->mAcch.ChkGroundHit()) {
        i_this->actor.speed.y = -0.5f;
    }
}
VERIFY(0x02185E80, kamome_ground_pos_move);

/* 02185FE4 */
static void* ko_s_sub(void* ac1, void* ac2) {
    WWHD_FUNC(0x02185FE4, void*, ac1, ac2);
    kamome_class* i_this = (kamome_class*)ac1;
    if (fopAc_IsActor(ac1) && i_this != nullptr && fpcM_GetName(ac1) == fpcNm_KAMOME_e &&
        (fopAcM_GetParam(&i_this->actor) & 0xf) == 7) {
        ko_count()++;
    }
    return nullptr;
}
VERIFY(0x02185FE4, ko_s_sub);

/* 02186050 */
static void* h_s_sub(void* ac1, void* ac2) {
    WWHD_FUNC(0x02186050, void*, ac1, ac2);
    kamome_class* i_this = (kamome_class*)ac1;
    if (fopAc_IsActor(ac1) && i_this != nullptr && fpcM_GetName(ac1) == fpcNm_KAMOME_e &&
        (fopAcM_GetParam(&i_this->actor) & 0xf) == 6) {
        return &i_this->actor;
    }
    return nullptr;
}
VERIFY(0x02186050, h_s_sub);

/* HD J3DModelData: joint nodes; getJointNodePointer(1)->setCallBack(cb) */
static void setHeadCallBack(J3DModel_l* model, u32 cb) {
    u32 data = model->mModelData;
    u32 n = gabi::load<u32>(data + 4);
    u32 p = gabi::load<u32>(data + 8);
    if (n > 8) p += 0xE0;
    gabi::store<u32>(p + 8, cb);
}

/* 021860B0 */
static void daKamome_setMtx(kamome_class* i_this) {
    WWHD_FUNC(0x021860B0, void, i_this);
    MtxTrans(i_this->actor.current.pos.x, i_this->actor.current.pos.y, i_this->actor.current.pos.z, 0);

    /* HD: no mRot offsets (GameCube: angle.y + mRot.y, angle.x + mRot.x) */
    cMtx_YrotM(calc_mtx(), i_this->actor.current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->actor.current.angle.x);
    cMtx_ZrotM(calc_mtx(), i_this->actor.current.angle.z);

    J3DModel* pJVar1 = McaMorf_getModel(i_this->mpMorf);
    J3DModel_setBaseTRMtx(pJVar1, calc_mtx());
    setHeadCallBack((J3DModel_l*)pJVar1, 0x021859B0 /* nodeCallBack */);
    McaMorf_calc(i_this->mpMorf);
    setHeadCallBack((J3DModel_l*)pJVar1, 0);
}
VERIFY(0x021860B0, daKamome_setMtx);

/* 02188C04 */
static BOOL daKamome_IsDelete(kamome_class*) {
    WWHD_FUNC(0x02188C04, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02188C04, daKamome_IsDelete);

/* 02188C0C */
static BOOL daKamome_Delete(kamome_class* i_this) {
    WWHD_FUNC(0x02188C0C, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, STR(0x100124C4));
    if (i_this->m688 != 0) {
        s8 no = l_kamomeHIO().mNo;
        hio_set() = false;
        mDoHIO_deleteChild(no);
    }
    return TRUE;
}
VERIFY(0x02188C0C, daKamome_Delete);

/* GHS: `new mDoExt_McaMorf(...)` calls the constructor with this == NULL */
static mDoExt_McaMorf_c* new_mDoExt_McaMorf(J3DModelData* modelData, void* cb1, void* cb2, J3DAnmTransform* anm, s32 loopMode,
                                             f32 rate, s32 startFrame, s32 endFrame, s32 param, void* soundAnm, u32 modelFlag,
                                             u32 differedDlistFlag) {
    return gabi::call<mDoExt_McaMorf_c*>(0x025E4F64, (u32)0, modelData, cb1, cb2, anm, loopMode, rate, startFrame, endFrame, param,
                                         soundAnm, modelFlag, differedDlistFlag);
}

/* 02188C70 */
static BOOL createHeap(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02188C70, BOOL, a_this);
    kamome_class* i_this = (kamome_class*)a_this;

    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x100124CC), dRes_INDEX_KAMOME_BDL_KA_e, SAFESTRING_VTBL);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x100124CC), dRes_INDEX_KAMOME_BCK_KA_WAIT1_e, SAFESTRING_VTBL);
    void* sound = dComIfG_getObjectRes(STR(0x100124CC), dRes_INDEX_KAMOME_BAS_KA_WAIT1_e, SAFESTRING_VTBL);
    mDoExt_McaMorf_c* morf = new_mDoExt_McaMorf(modelData, nullptr, nullptr, anm, J3DFrameCtrl_EMode_LOOP, 1.0f, 0, -1, 1, sound,
                                                 0x80000, 0x11000022);
    i_this->mpMorf = morf;
    if (morf == nullptr || McaMorf_getModel(morf) == nullptr) {
        return FALSE;
    }

    gabi::store<u32>(gabi::ea(McaMorf_getModel(morf)) + 0xB8, gabi::ea(&i_this->actor)); /* setUserArea */
    return TRUE;
}
VERIFY(0x02188C70, createHeap);

/* member constructors run by fopAcM_ct (HD: inline parts write the sub-object vtables) */
static void kamome_class_ct(kamome_class* i_this) {
    fopAc_ac_c_ct(&i_this->actor);
    i_this->actor.__vtbl = 0x100123FC;
    gabi::call(0x024EFE94, &i_this->mAcchCir); /* dBgS_AcchCir::dBgS_AcchCir */
    gabi::call(0x024F0474, &i_this->mAcch);    /* dBgS_Acch::dBgS_Acch */
    u32 acch = gabi::ea(&i_this->mAcch);      /* dBgS_ObjAcch */
    gabi::store<u8>(acch + 0x18, 1);
    gabi::store<u32>(acch + 0x10, 0x1001237C);
    gabi::store<u32>(acch + 0x20, 0x1001238C);
    gabi::store<u32>(acch + 0x14, 0x1001239C);
    gabi::call(0x0200BD2C, &i_this->mStts);    /* cCcD_Stts */
    u32 stts = gabi::ea(&i_this->mStts);
    gabi::call(0x02515DA0, stts + 0x1C);       /* dCcD_GStts::dCcD_GStts */
    gabi::store<u32>(stts + 0x18, 0x1004AE88);
    gabi::store<u32>(stts + 0x1C, 0x1004AEC0);
    gabi::call(0x025166F0, &i_this->mSph);     /* dCcD_Sph */
}

/* 02188D84 */
static cPhs_State daKamome_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02188D84, cPhs_State, a_this);
    kamome_class* i_this = (kamome_class*)a_this;
    /* fopAcM_ct(a_this, kamome_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) kamome_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State PVar1 = dComIfG_resLoad(&i_this->mPhase, STR(0x100124D8));
    if (PVar1 == cPhs_COMPLEATE_e) {
        i_this->mType = fopAcM_GetParam(a_this);
        if (i_this->mType == 0xff) {
            i_this->mType = 0;
        }

        i_this->mKoMaxCount = fopAcM_GetParam(a_this) >> 8;
        i_this->mPathIdx = fopAcM_GetParam(a_this) >> 0x10;
        i_this->mSwitchNoPrm = fopAcM_GetParam(a_this) >> 0x18;

        if (!fopAcM_entrySolidHeap(a_this, 0x02188C70 /* createHeap */, 0x1360)) {
            return cPhs_ERROR_e;
        }

        if (i_this->mPathIdx != 0xff) {
            i_this->mpPath = dPath_GetRoomPath(i_this->mPathIdx, fopAcM_GetRoomNo(a_this));
            if (i_this->mpPath == nullptr) {
                return cPhs_ERROR_e;
            }
            i_this->mbUsePathMovement = i_this->mPathIdx + 1;
            i_this->mPathIdxIncr = 1;
        }

        if (i_this->mSwitchNoPrm != 0xff) {
            i_this->mSwitchNo = i_this->mSwitchNoPrm + 1;
        }

        /* fopAcM_SetMtx(a_this, model->getBaseTRMtx()) */
        a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(McaMorf_getModel(i_this->mpMorf)));
        f32 tmp = REG0_F(2) + 1.0f;
        f32 fVar6 = REG0_F(3) + 1.0f;
        i_this->mScale = fVar6 + cM_rndF(tmp - fVar6);
        if ((i_this->mType == 4) || (i_this->mType == 5)) {
            i_this->mScale = i_this->mScale * (REG0_F(9) + 0.75f);
        }

        /* model->setBaseScale(mScale, mScale, mScale) */
        u32 model = gabi::ea(McaMorf_getModel(i_this->mpMorf));
        f32 s = i_this->mScale;
        for (int k = 0; k < 3; k++) gabi::store<f32>(model + 0xBC + 4 * k, s);
        daKamome_setMtx(i_this);
        gabi::call(0x024F06B4, &i_this->mAcch, &a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir, &a_this->speed,
                   (u32)0, (u32)0); /* dBgS_Acch::Set */

        if ((i_this->mType == 4) || (i_this->mType == 5)) {
            gabi::call(0x024EFF44, &i_this->mAcchCir, 50.0f, 80.0f); /* dBgS_AcchCir::SetWall */
        } else {
            gabi::call(0x024EFF44, &i_this->mAcchCir, 30.0f, 20.0f);
        }

        if (i_this->mType == 4) {
            gabi::call(0x02515F14, &i_this->mStts, 0x32, 0xff, a_this); /* dCcD_Stts::Init */
            gabi::call(0x0251677C, &i_this->mSph, (u32)0x101B7C24);       /* dCcD_Sph::Set(co_sph_src) */
            i_this->mSph.mpStts = &i_this->mStts;
        }

        i_this->mGlobalTimer = (s16)gabi::ftoi(cM_rndF(10000.0f));

        if (!hio_set()) {
            l_kamomeHIO().mNo = mDoHIO_createChild(STR(0x100124E0) /* "カモメ" */, &l_kamomeHIO());
            i_this->m688 = 1;
            hio_set() = true;
        }

        if ((i_this->mType != 6) && (i_this->mType == 7)) {
            i_this->m2B4 = (s16)gabi::ftoi(cM_rndF(10000.0f));
            fopAcM_setStageLayer(a_this);
            a_this->current.roomNo = (s8)0xff; /* fopAcM_SetRoomNo */
        }
    }
    return PVar1;
}
VERIFY(0x02188D84, daKamome_Create);

/* 02189180: static initialisers */
static void __sinit_d_a_kamome_cpp() {
    WWHD_FUNC(0x02189180, void, (u32)0);
    /* l_kamomeHIO's base JORReflexible / mDoHIO_entry registration objects (HD) */
    for (int i = 3; i >= 0; i--) gabi::store<u32>(0x104648B8 + 4 * i, 0);
    gabi::call(0x028F026C, (u32)0x101B7C64); /* __register_global_object */
    gabi::store<f32>(0x104648AC, -3.1415927f);
    gabi::store<f32>(0x104648B0, 3.1415927f);
    gabi::call(0x028ED6F8, (u32)0x104648B4);
    gabi::call(0x028F026C, (u32)0x101B7C70);
    gabi::call(0x028EAB2C, (u32)0x104648B5);
    gabi::call(0x028F026C, (u32)0x101B7C7C);
    /* kamomeHIO_c::kamomeHIO_c() */
    kamomeHIO_c& h = l_kamomeHIO();
    h.m06 = 1;
    h.m05 = 0;
    h.m0C = 400.0f;
    h.__vtbl = 0x100123EC;
    h.m10 = 300.0f;
    h.m08 = 8;
}
VERIFY(0x02189180, __sinit_d_a_kamome_cpp);

/* 0218925C: kamomeHIO_c deleting destructor (HD: classes with a virtual destructor) */
static void kamomeHIO_c_dt(kamomeHIO_c* p, s32 flags) {
    WWHD_FUNC(0x0218925C, void, p, flags);
    if (p != nullptr && (flags & 1)) gabi::call(0x0273AF40, p); /* operator delete */
}
VERIFY(0x0218925C, kamomeHIO_c_dt);
