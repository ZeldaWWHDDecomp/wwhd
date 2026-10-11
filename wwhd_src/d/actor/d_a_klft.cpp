/**
 * d_a_klft.cpp (WWHD)
 * Object - Forbidden Woods - Lift (wooden platform hanging from two ropes, swung by the wind
 * and the Deku Leaf).
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_klft.cpp) has only "Nonmatching" stubs for this unit: every function here is
 * written from the WWHD code (cking.rpx) and verified against it. Names follow the GameCube
 * symbols; field names are descriptive guesses (GameCube offsets unknown).
 */
#include "d/actor/d_a_klft.h"

static const char* l_arcName = "Klft";
static constexpr u32 l_aabVtable = 0x10012FE4;
enum Resource {
    RES_LIFT_MODEL = 5,
    RES_ANCHOR_MODEL = 6,
    RES_LIFT_COLLISION = 9,
    RES_ROPE_TEXTURE = 12,
};
static GXColor l_himo_color = {150, 150, 150, 255};
static KlftSphereSource utiwa_sph_src = {0, 0, 0, 0, 0, 0, 5, 0x200000, 3,    0,    0,    0,     0,
                                         0, 0, 0, 0, 0, 0, 2, 0,        0.0f, 0.0f, 0.0f, 250.0f};
static KlftCylinderSource p_co_cyl_src = {0,   0, 0, 0,    0,    0,    0,     0xff1dfebf, 15,
                                          121, 0, 0, 0,    0,    0,    6,     0,          0,
                                          0,   1, 0, 0.0f, 0.0f, 0.0f, 40.0f, 60.0f};

const gc_tu_data gc_tu = {.safeString = 0x10012FCC};

enum { PROC_PLAYER = 0xA8 };

/* 0219F608 */
static void ride_call_back(dBgW* bgw, fopAc_ac_c* i_actor, fopAc_ac_c* rider) {
    klft_class* i_this = (klft_class*)i_actor;
    gabi::Local<cXyz> tmp;
    gabi::Local<cXyz> arg;
    gabi::Local<cXyz> now;
    gabi::Local<cXyz> old;

    mDoMtx_YrotS(calc_mtx(), (s16)-i_this->current.angle.y);
    cXyz_mi(&rider->current.pos, tmp.get(), &i_this->mCenter);
    arg->copy(*tmp.get());
    MtxPosition(arg.get(), now.get());
    cXyz_mi(&rider->old.pos, tmp.get(), &i_this->mCenter);
    arg->copy(*tmp.get());
    MtxPosition(arg.get(), old.get());

    f32 x = now->x;
    f32 z = now->z;
    if (rider != nullptr && fpcM_GetName(rider) == PROC_PLAYER) {
        i_this->mRideTimer = 10;
    }
    i_this->mSinkTarget = -50.0f;
    f32 dist = std_sqrtf(gabi::fmadds(x, x, z * z));
    s16 tilt = (s16)gabi::ftoi(dist * ((REG0F(0) + 30.0f) / i_this->scale.z));
    cLib_addCalcAngleS2(&i_this->mTilt, tilt, 10, 0x800);
    cLib_addCalcAngleS2(&i_this->mTiltDir, cM_atan2s(now->x, now->z), 2, 0x2000);

    f32 sx = CPad_GET_STICK_POS_X(0);
    f32 sy = CPad_GET_STICK_POS_Y(0);
    if (std_fabsf(sx) + std_fabsf(sy) > 0.1f || std_fabsf(i_this->mSpeed) > 0.0001f) {
        f32 dz = std_fabsf(now->z - old->z) * (REG0F(4) + 100.0f);
        if (dz > REG0F(6) + 200.0f) {
            i_this->mRotTarget.x = dz;
        }
        f32 dx = std_fabsf(now->x - old->x) * (REG0F(4) + 100.0f);
        if (dx > REG0F(6) + 200.0f) {
            i_this->mRotTarget.z = dx;
        }
        f32 k = REG0F(8) + 2.0f;
        dx = std_fabsf(now->x - old->x) * k;
        if (dx > 10.0f && i_this->mSwayAmplitude.x < dx) {
            i_this->mSwayTarget.x = dx;
        }
        dz = std_fabsf(now->z - old->z) * (REG0F(8) + 2.0f);
        if (dz > 10.0f && i_this->mSwayAmplitude.z < dz) {
            i_this->mSwayTarget.z = dz;
        }
    }
}

// HD: joint 1 of the lift model (the wheel) turns with mWheelRot
/* 0219F8A4 */
static BOOL nodeCallBack_main(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel* model = j3dSys.getModel();
        klft_class* i_this = static_cast<klft_class*>(model->getUserArea());
        s32 jntNo = reinterpret_cast<KlftJointView*>(joint)->mJntNo;
        if (i_this != nullptr && jntNo == 1) {
            PSMTXCopy(model->getAnmMtx(1), calc_mtx());
            mDoMtx_XrotM(calc_mtx(), i_this->mWheelRot);
            Mtx34* dst = model->getAnmMtx(1);
            mtx_copy(dst, calc_mtx());
            PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
        }
    }
    return TRUE;
}

// HD: joint 3 of the rope anchors turns with mAnchorRotY
/* 0219F9C0 */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel* model = j3dSys.getModel();
        klft_class* i_this = static_cast<klft_class*>(model->getUserArea());
        s32 jntNo = reinterpret_cast<KlftJointView*>(joint)->mJntNo;
        if (i_this != nullptr) {
            PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
            mDoMtx_YrotM(calc_mtx(), i_this->mAnchorRotY);
            Mtx34* dst = model->getAnmMtx(jntNo);
            mtx_copy(dst, calc_mtx());
            PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
        }
    }
    return TRUE;
}

// HD: himo_Draw inlined
/* 0219FAE4 */
static BOOL daKlft_Draw(klft_class* i_this) {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(i_this->mpModel);
    dComIfGd_setList();
    klftUpdateRopeMaterial(&i_this->mLineMat, 0x14, &l_himo_color, &i_this->tevStr);
    dComIfGd_set3DlineMat(&i_this->mLineMat);
    for (int i = 0; i < 2; i++) {
        mDoExt_McaMorf* morf = i_this->mpMorf[i];
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, morf->mpModel, &i_this->tevStr);
        i_this->mpMorf[i]->updateDL();
    }
    return TRUE;
}

// HD: klft_move and himo_move inlined
/* 0219FBFC */
static BOOL daKlft_Execute(klft_class* i_this) {
    dComIfGp_get();
    if (i_this->mHitTimer != 0) {
        i_this->mHitTimer = (s16)(i_this->mHitTimer - 1);
    }
    if (i_this->mRideTimer != 0) {
        i_this->mRideTimer = (s16)(i_this->mRideTimer - 1);
    }
    cXyz* wind_vec = dKyw_get_wind_vec();
    klftGlobals().mpWind = wind_vec;
    klftGlobals().mWindAngle = cM_atan2s(wind_vec->x, wind_vec->z);
    klftGlobals().mWindPower = dKyw_get_wind_power();

    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    i_this->mCounter = (s16)(i_this->mCounter + 1);

    /* klft_move */
    s16 ang;
    f32 pow;
    f32 spd_step = 0.01f;
    if (i_this->mRideTimer != 0) {
        pow = 1.0f;
        ang = player->shape_angle.y;
    } else {
        ang = (s16)(player->shape_angle.y + 0x8000);
        pow = 0.5f;
    }
    if (i_this->mSph.ChkTgHit() || i_this->mWindSph[0].ChkTgHit() ||
        i_this->mWindSph[1].ChkTgHit()) {
        gabi::Local<cXyz> v;
        gabi::Local<cXyz> out;
        if (i_this->mSph.ChkTgHit()) {
            mDoMtx_YrotS(calc_mtx(), (s16)(ang - i_this->current.angle.y));
            v->x = 0.0f;
            v->y = 0.0f;
            v->z = (REG0F(13) + 0.5f) * pow;
            MtxPosition(v.get(), out.get());
            i_this->mSpeedTarget = out->z;
        } else {
            s16 a = i_this->mWindSph[0].ChkTgHit() ? -0x8000 : 0;
            mDoMtx_YrotS(calc_mtx(), a);
            v->y = 0.0f;
            v->x = 0.0f;
            v->z = (REG0F(16) + 0.85f) * pow;
            spd_step = 0.1f;
            MtxPosition(v.get(), out.get());
            i_this->mSpeedTarget = out->z;
        }
        v->z = 0.0f;
        v->y = 0.0f;
        v->x = (REG0F(15) + 500.0f) * pow;
        MtxPosition(v.get(), &i_this->mRotTarget);
        i_this->mHitTimer = (s16)(REG0_S(5) + 0x3C);
        mDoMtx_YrotS(calc_mtx(), ang);
        v->x = 0.0f;
        v->y = 0.0f;
        v->z = (REG0F(16) + 20.0f) * pow;
        MtxPosition(v.get(), &i_this->mSwayTarget);
    }
    if (i_this->mHitTimer == 0x28) {
        klft_seStartCurrent(i_this, 0x3821, 0);
    }
    cLib_addCalc2(&i_this->mSpeed, i_this->mSpeedTarget, 1.0f, spd_step);
    cLib_addCalc0(&i_this->mSpeedTarget, 1.0f, REG0F(14) + 0.001f);
    f32 p = i_this->mPos + i_this->mSpeed;
    if (p > 80.0f) {
        i_this->mPos = 80.0f;
        i_this->mSpeed = 0.0f;
    } else if (p < 20.0f) {
        i_this->mPos = 20.0f;
        i_this->mSpeed = 0.0f;
    } else {
        i_this->mPos = p;
    }
    i_this->mWheelRot = (s16)(i_this->mWheelRot + (s16)gabi::ftoi(i_this->mSpeed * -2500.0f));
    if (std_fabsf(i_this->mSpeed) > 0.01f) {
        f32 v = std_fabsf(i_this->mSpeed) * (REG0F(0) + 500.0f);
        u32 vol;
        if (!(v < 2147483648.0f)) {
            vol = (u32)gabi::ftoi(v - 2147483648.0f) + 0x80000000u;
        } else {
            vol = (u32)gabi::ftoi(v);
        }
        if (vol > 100) {
            vol = 100;
        }
        for (int i = 0; i < 2; i++) {
            mDoAud_seStart(0x61B1, &i_this->mHimoTop[i], vol,
                           dComIfGp_getReverb(i_this->current.roomNo));
        }
        f32 se = i_this->mWheelSe + std_fabsf(i_this->mSpeed);
        i_this->mWheelSe = se;
        f32 lim = REG0F(1) + 3.0f;
        if (se > lim) {
            i_this->mWheelSe = se - lim;
            klft_seStartCurrent(i_this, 0x3822, vol);
        }
    }
    for (int i = 0; i < 2; i++) {
        if (i_this->mCoCyl[i].ChkTgHit()) {
            def_se_set(i_this, i_this->mCoCyl[i].GetTgHitObj(), 0xB);
        }
    }
    cLib_addCalcAngleS2(&i_this->mTilt, 0, 10, 0x200);
    cLib_addCalcAngleS2(&i_this->current.angle.x, 0, 10, 0x200);
    cLib_addCalcAngleS2(&i_this->current.angle.z, 0, 10, 0x200);

    s32 t = i_this->mCounter;
    f32 rx = i_this->mRotAmplitude.x;
    i_this->mShakeAngle.x = (s16)gabi::ftoi(cM_ssin(t * 0x5DC) * rx);
    f32 rz = i_this->mRotAmplitude.z;
    i_this->mShakeAngle.z = (s16)gabi::ftoi(cM_ssin(t * 0x514) * rz);

    if (i_this->mHitTimer != 0) {
        cLib_addCalc2(&i_this->mRotAmplitude.x, i_this->mRotTarget.x, 1.0f, 10.0f);
    } else if (std_fabsf(i_this->mRotTarget.x) > REG0F(2) + 100.0f) {
        cLib_addCalc2(&i_this->mRotAmplitude.x, i_this->mRotTarget.x, 1.0f, REG0F(4) + 10.0f);
    } else {
        cLib_addCalc2(&i_this->mRotAmplitude.x, i_this->mRotTarget.x, 1.0f, REG0F(3) + 2.0f);
    }
    if (i_this->mHitTimer != 0) {
        cLib_addCalc2(&i_this->mRotAmplitude.z, i_this->mRotTarget.z, 1.0f, 40.0f);
    } else if (std_fabsf(i_this->mRotTarget.z) > REG0F(2) + 100.0f) {
        cLib_addCalc2(&i_this->mRotAmplitude.z, i_this->mRotTarget.z, 1.0f, REG0F(4) + 10.0f);
    } else {
        cLib_addCalc2(&i_this->mRotAmplitude.z, i_this->mRotTarget.z, 1.0f, REG0F(3) + 2.0f);
    }
    f32 pos = i_this->mPos;
    if (i_this->mHitTimer == 0) {
        i_this->mRotTarget.z = 0.0f;
        i_this->mRotTarget.x = 0.0f;
    }

    s16 sa = cM_rad2s(pos * 0.01f * 3.1415927f);
    t = i_this->mCounter;
    f32 s = cM_ssin(sa);
    i_this->mSway.x = cM_ssin(t * 0x2EE) * i_this->mSwayAmplitude.x * s;
    i_this->mSway.z = cM_ssin(t * 0x384) * i_this->mSwayAmplitude.z * s;
    i_this->mSway.y = cM_ssin(t * 0x5DC) * (i_this->mSwayAmplitude.x + i_this->mSwayAmplitude.z) *
                      (REG0F(1) + 1.0f) * s;

    if (i_this->mHitTimer != 0) {
        cLib_addCalc2(&i_this->mSwayAmplitude.x, i_this->mSwayTarget.x, 1.0f, 10.0f);
    } else {
        cLib_addCalc2(&i_this->mSwayAmplitude.x, i_this->mSwayTarget.x, 1.0f,
                      (10.0f - std_fabsf(i_this->mSwayTarget.x) >= 0.0f) ? 0.25f : 1.2f);
    }
    if (i_this->mHitTimer != 0) {
        cLib_addCalc2(&i_this->mSwayAmplitude.z, i_this->mSwayTarget.z, 1.0f, 10.0f);
    } else {
        cLib_addCalc2(&i_this->mSwayAmplitude.z, i_this->mSwayTarget.z, 1.0f,
                      (10.0f - std_fabsf(i_this->mSwayTarget.z) >= 0.0f) ? 0.25f : 1.2f);
    }
    if (i_this->mHitTimer == 0) {
        i_this->mSwayTarget.x = 0.0f;
        i_this->mSwayTarget.z = 0.0f;
    }

    csXyz_pl(&i_this->current.angle, &i_this->mShakeAngle, &i_this->shape_angle);
    {
        gabi::Local<cXyz> a;
        gabi::Local<cXyz> b;
        gabi::Local<cXyz> c;
        cXyz_ml(&i_this->mPathDir, a.get(), i_this->mPos);
        cXyz_ml(a.get(), b.get(), 0.01f);
        cXyz_pl(&i_this->mPathP0, c.get(), b.get());
        i_this->home.pos.x = c->x;
        f32 y = c->y;
        i_this->home.pos.z = c->z;
        f32 k = (i_this->mSink + -100.0f) + REG0F(0);
        i_this->home.pos.y = y + gabi::fmadds(s, k, REG0F(2) + -100.0f);
    }
    cLib_addCalc2(&i_this->mSink, i_this->mSinkTarget, 0.1f, 5.0f);
    i_this->mSinkTarget = 0.0f;
    {
        gabi::Local<cXyz> a;
        cXyz_pl(&i_this->home.pos, a.get(), &i_this->mSway);
        i_this->current.pos.copy(*a.get());
    }

    /* himo_move */
    {
        KlftLine* lines = klftLine(&i_this->mLineMat);
        be<u8>* sizes = lines->mpSize;
        cXyz* positions = lines->mpPos;
        gabi::Local<cXyz> d;
        gabi::Local<cXyz> d0;
        gabi::Local<cXyz> d1;
        gabi::Local<cXyz> m;
        gabi::Local<cXyz> r;
        cXyz_mi(&i_this->current.pos, d.get(), &i_this->mPathP0);
        d0->copy(*d.get());
        cXyz_mi(&i_this->current.pos, d.get(), &i_this->mPathP1);
        d1->x = d->x;
        d0->y = d0->y - 25.0f;
        d1->y = d->y - 25.0f;
        d1->z = d->z;
        for (int k = 0; k < 20; k++) {
            f32 f;
            cXyz* base;
            cXyz* dir;
            if (k < 10) {
                f = (f32)k / 9.0f;
                base = &i_this->mPathP0;
                dir = d0.get();
            } else {
                f = (f32)(19 - k) / 9.0f;
                base = &i_this->mPathP1;
                dir = d1.get();
            }
            cXyz_ml(dir, m.get(), f);
            cXyz_pl(base, r.get(), m.get());
            cXyz* pt = &positions[k];
            pt->copy(*r.get());
            s16 sa2 = cM_rad2s(f * 3.1415927f);
            pt->y = gabi::fmadds(cM_ssin(sa2), REG0F(7) + -20.0f, pt->y);
            sizes[k] = 8;
        }
    }

    MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
    mDoMtx_YrotM(calc_mtx(), i_this->shape_angle.y);
    mDoMtx_YrotM(calc_mtx(), i_this->mTiltDir);
    mDoMtx_XrotM(calc_mtx(), i_this->mTilt);
    mDoMtx_YrotM(calc_mtx(), (s16)-i_this->mTiltDir);
    mDoMtx_XrotM(calc_mtx(), i_this->shape_angle.x);
    mDoMtx_ZrotM(calc_mtx(), i_this->shape_angle.z);
    {
        Mtx34* src = calc_mtx();
        i_this->mpModel->setBaseTRMtx(src);
    }
    PSMTXCopy(calc_mtx(), &i_this->mBgMtx);
    MtxTrans(0.0f, REG0F(7) - 400.0f, 0.0f, 1);
    {
        gabi::Local<cXyz> z;
        z->set(0.0f, 0.0f, 0.0f);
        MtxPosition(z.get(), &i_this->mCenter);
    }
    i_this->mStts.Move();
    i_this->mSph.SetC(&i_this->mCenter);
    dComIfG_Ccsp_Set(&i_this->mSph);
    for (int i = 0; i < 2; i++) {
        gabi::Local<cXyz> c;
        f32 h = REG_F(0, 11) + 35.0f;
        cXyz* p = &i_this->mHimoTop[i];
        c->x = p->x;
        c->z = p->z;
        c->y = p->y + h;
        i_this->mWindSph[i].SetC(c.get());
        dComIfG_Ccsp_Set(&i_this->mWindSph[i]);
        i_this->mCoCyl[i].SetC(c.get());
        dComIfG_Ccsp_Set(&i_this->mCoCyl[i]);
    }
    dBgW_Move(i_this->mpBgW);
    i_this->mAnchorRotY =
        (s16)(i_this->mAnchorRotY + (s16)gabi::ftoi(i_this->mSpeed * (REG0F(8) + 8000.0f)));
    for (int i = 0; i < 2; i++) {
        MtxTrans(i_this->mHimoTop[i].x, i_this->mHimoTop[i].y, i_this->mHimoTop[i].z, 0);
        mDoExt_McaMorf* morf = i_this->mpMorf[i];
        Mtx34* src = calc_mtx();
        morf->mpModel->setBaseTRMtx(src);
    }
    return TRUE;
}

/* 021A0AEC */
static BOOL daKlft_IsDelete(klft_class*) { return TRUE; }

/* 021A0AF4 */
static BOOL daKlft_Delete(klft_class* i_this) {
    dComIfG_resDelete(&i_this->mPhs, l_arcName);
    if (i_this->heap) {
        cBgS_Release(dComIfG_Bgsp(), i_this->mpBgW);
    }
    u8 sw = i_this->mSwitchNo;
    if (sw != 0) {
        if (i_this->mPos < 50.0f) {
            dComIfGs_offSwitch(sw, i_this->current.roomNo);
        } else {
            dComIfGs_onSwitch(sw, i_this->current.roomNo);
        }
        mDoAud_seDeleteObject(&i_this->mHimoTop[0]);
    } else {
        /* harness workaround: the matcher's GameCube signature (a JAIZelBasic member) makes the
         * harness compare r4, which 025E1B34 does not read; here r4 still holds sw (0) */
        klftDeleteSoundObject(&i_this->mHimoTop[0], sw);
    }
    mDoAud_seDeleteObject(&i_this->mHimoTop[1]);
    return TRUE;
}

/* 021A0BB0 */
static BOOL CallbackCreateHeap(fopAc_ac_c* i_actor) {
    klft_class* i_this = (klft_class*)i_actor;
    J3DModelData* modelData =
        (J3DModelData*)dComIfG_getObjectRes(l_arcName, RES_LIFT_MODEL, gc_tu.safeString);
    J3DModel* model = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    i_this->mpModel = model;
    if (model == nullptr) {
        return FALSE;
    }
    model->setUserArea(reinterpret_cast<uintptr_t>(i_this));
    for (u16 i = 0; i < modelData->getJointNum(); i++) {
        if (i == 1) {
            modelData->getJointNodePointer(i)->setCallBack(nodeCallBack_main);
        }
    }
    i_this->mpBgW = new_dBgW();
    if (i_this->mpBgW == nullptr) /* JUT_ASSERT(0x344, actor->pm_bgw != NULL) */
        JUT_ASSERT_fail("d_a_klft.cpp", 0x344, "actor->pm_bgw != (0)");
    cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes(l_arcName, RES_LIFT_COLLISION, gc_tu.safeString);
    cBgW_Set(i_this->mpBgW, dzb, cBgW_MOVE_BG_e, &i_this->mBgMtx);
    klftBackground(i_this->mpBgW)->mCrrFunc = 0x024EE658; // dBgS_MoveBGProc_Trans
    klftBackground(i_this->mpBgW)->mRideCallback.v = gabi::guest_fn((const void*)ride_call_back);
    void* tex = dComIfG_getObjectRes(l_arcName, RES_ROPE_TEXTURE, gc_tu.safeString);
    if (!mDoExt_3DlineMat1_init(&i_this->mLineMat, 1, 0x14, tex, 1)) {
        return FALSE;
    }
    for (int k = 0; k < 2; k++) {
        J3DModelData* data =
            (J3DModelData*)dComIfG_getObjectRes(l_arcName, RES_ANCHOR_MODEL, gc_tu.safeString);
        mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, nullptr, 2,
                                                      1.0f, 0, -1, 0, nullptr, 0, 0x11020203);
        i_this->mpMorf[k] = morf;
        J3DModel* anchorModel = morf->mpModel;
        anchorModel->setUserArea(reinterpret_cast<uintptr_t>(i_this));
        for (u16 i = 0; i < anchorModel->getModelData()->getJointNum(); i++) {
            if (i == 3) {
                anchorModel->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack);
            }
        }
    }
    return TRUE;
}

/* 021A0E60 */
static cPhs_State daKlft_Create(fopAc_ac_c* i_actor) {
    klft_class* i_this = (klft_class*)i_actor;
    dComIfGp_get();
    /* fopAcM_ct(i_this, klft_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = klft_class::__vt;
            __ct(&i_this->mLineMat);
            dCcD_Stts_ct(&i_this->mStts);
            klftConstructSphere(&i_this->mSph);
            klftConstructArray(i_this->mCoCyl, 2, 0x130, 0x021A1448);
            klftConstructArray(i_this->mWindSph, 2, 0x12C, 0x025166F0);
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&i_this->mPhs, l_arcName);
    if (phase_state != cPhs_COMPLEATE_e) {
        return phase_state;
    }
    i_this->mType = (u8)fopAcM_GetParam(i_this);
    i_this->mSwitchNo = (u8)i_this->current.angle.z;
    u32 path_id = (fopAcM_GetParam(i_this) >> 16) & 0xFF;
    i_this->current.angle.z = 0;
    if (i_this->mSwitchNo == 0xFF) {
        i_this->mSwitchNo = 0;
    } else if (i_this->mSwitchNo != 0) {
        if (dComIfGs_isSwitch(i_this->mSwitchNo, i_this->current.roomNo)) {
            i_this->mPos = 80.0f;
        } else {
            i_this->mPos = 20.0f;
        }
    }
    if (i_this->mType == 0xFF) {
        i_this->mType = 0;
    }

    if (!fopAcM_entrySolidHeap(i_this, CallbackCreateHeap, 0x10000)) {
        return cPhs_ERROR_e;
    }
    if (i_this->mpBgW != nullptr && dBgS_Regist(dComIfG_Bgsp(), i_this->mpBgW, i_this)) {
        return cPhs_ERROR_e;
    }
    if (path_id == 0xFF) {
        return cPhs_ERROR_e;
    }
    dPath* path = dPath_GetRoomPath(path_id, i_this->current.roomNo);
    if (path == nullptr) {
        return cPhs_ERROR_e;
    }
    KlftPathPoint* points = reinterpret_cast<KlftPath*>(path)->m_points;
    i_this->mPathP0.set(points[0].m_position.x, points[0].m_position.y, points[0].m_position.z);
    i_this->mPathP1.set(points[1].m_position.x, points[1].m_position.y, points[1].m_position.z);
    {
        gabi::Local<cXyz> d;
        cXyz_mi(&i_this->mPathP1, d.get(), &i_this->mPathP0);
        i_this->home.angle.y = (s16)(cM_atan2s(d->x, d->z) + 0x8000);
        i_this->current.angle.y = (s16)(cM_atan2s(d->x, d->z) + 0x8000);
        i_this->mPathDir.copy(*d.get());
    }
    i_this->mHimoTop[0].set(points[2].m_position.x, points[2].m_position.y, points[2].m_position.z);
    i_this->mHimoTop[1].set(points[3].m_position.x, points[3].m_position.y, points[3].m_position.z);

    f32 s;
    switch (i_this->mType) {
    case 1:
        s = 0.9f;
        break;
    case 2:
        s = 0.8f;
        break;
    case 3:
        s = 0.7f;
        break;
    default:
        s = 1.0f;
        break;
    }
    i_this->scale.x = s;
    i_this->scale.y = 1.0f;
    i_this->scale.z = s;
    {
        J3DModel* model = i_this->mpModel;
        model->mBaseScale.y = 1.0f;
        model->mBaseScale.z = s;
        model->mBaseScale.x = s;
    }
    i_this->mStts.Init(0xFF, 0xFF, i_this);
    i_this->mSph.Set(reinterpret_cast<const dCcD_SrcSph*>(&utiwa_sph_src));
    i_this->mSph.SetStts(&i_this->mStts);
    for (int i = 0; i < 2; i++) {
        i_this->mWindSph[i].Set(reinterpret_cast<const dCcD_SrcSph*>(&utiwa_sph_src));
        i_this->mWindSph[i].SetStts(&i_this->mStts);
        i_this->mWindSph[i].SetR(REG_F(0, 11) + 130.0f);
        i_this->mCoCyl[i].Set(reinterpret_cast<const dCcD_SrcCyl*>(&p_co_cyl_src));
        i_this->mCoCyl[i].SetStts(&i_this->mStts);
    }
    daKlft_Execute(i_this);
    return phase_state;
}

/* 021A13A0 */
static void __sinit_d_a_klft_cpp() {
    /* the header statics in a different order: floats at 0x10464BCC, objects at +0xA/+0xB */
    for (auto& value : klftGlobals().mZeroState)
        value = 0;
    __register_global_object(0x101B84CC);
    klftGlobals().mNegativePi = -3.1415927f;
    klftGlobals().mPi = 3.1415927f;
    klftMathInit(&klftGlobals().mMathInit);
    __register_global_object(0x101B84D8);
    klftRandomInit(&klftGlobals().mRandomInit);
    __register_global_object(0x101B84E4);
}

// HD: sead::SafeString deleting destructor (this TU's vtable slot)
/* 021A1434 */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// HD: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty)
/* 021A158C */
static void SafeString_assureTerminationImpl(void*) {}

// HD: dCcD_Cyl::dCcD_Cyl (this TU's copy for klftConstructArray; allocates when NULL)
/* 021A1448 */
static dCcD_Cyl* dCcD_Cyl_ct_tu(dCcD_Cyl* p) {
    if (p == nullptr) {
        p = (dCcD_Cyl*)operator_new(0x130);
        if (p == nullptr)
            return nullptr;
    }
    dCcD_Cyl_ct(p, l_aabVtable);
    return p;
}

// HD: klft_class deleting destructor (HD virtual)
/* 021A14D4 */
static void klft_class_dt(klft_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        klftDestroyArray(i_this->mWindSph, 2, 0x12C, 0x02515AE8); /* dCcD_Sph::~dCcD_Sph */
        klftDestroyArray(i_this->mCoCyl, 2, 0x130, 0x02515A70);   /* dCcD_Cyl::~dCcD_Cyl */
        klftSphereDestruct(&i_this->mSph);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(&i_this->mLineMat);
        __dt(i_this);
        if (flags & 1)
            operator_delete(i_this);
    }
}

// clang-format off
/* WWHD glue
data l_himo_color 101B8424
data utiwa_sph_src 101B8448
data p_co_cyl_src 101B8488
str 10013094 "Klft" in 021A0AF4
str 1001309C "Klft" in 021A0BB0
str 100130DC "Klft" in 021A0E60
str 100130A4 "d_a_klft.cpp" in 021A0BB0
str 100130B4 "actor->pm_bgw != (0)" in 021A0BB0
*/
// clang-format on
