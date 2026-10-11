/**
 * d_a_kantera.cpp (WWHD)
 * Lantern (hanging/carried lantern with a flame, two moths and a point light).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kantera.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_kantera.h"

const gc_tu_data gc_tu = {.safeString = 0x10012644};

#define KANTERA_VTBL 0x100126DC    /* kantera_class vtable (HD virtual destructor) */
#define SAFESTRING_VTBL 0x10012644 /* this TU's sead::SafeString vtable */
static const char* l_arcName = "Kantera";
static const char* l_fileName = "d_a_kantera.cpp";
static const char* l_modelDataAssert = "modelData != (0)";
static const char* l_brkAssert = "anm_res_brk != (0)";
static KanteraSphereSource at_sph_src = {0x0,  0x200, 0x1, 0x0,  0x0,  0x0,  0x5,  0x0, 0x0,
                                         0x75, 0x0,   0x0, 0x8,  0x0,  0x0,  0x0,  0x0, 0x0,
                                         0x0,  0x0,   0x0, 0.0f, 0.0f, 0.0f, 40.0f};

#define REG10_F(i) REG_F(10, i)
#define REG17_F(i) REG_F(17, i)

/* 0218D000 */
static BOOL kantera_nodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel* model = j3dSys.getModel();
        kantera_class* i_this = static_cast<kantera_class*>(model->getUserArea());
        u16 jntNo = reinterpret_cast<KanteraJoint*>(joint)->mJntNo;
        if (i_this != nullptr &&
            (jntNo == MK_KANTERA_JNT_KANTERA_e || jntNo == MK_KANTERA_JNT_TOTTE_e)) {
            PSMTXCopy(kanteraAnmMtx(model, jntNo), calc_mtx());
            cMtx_XrotM(calc_mtx(), (s16)(i_this->mJointBaseRot.x + i_this->mJointRot[jntNo].x));
            cMtx_ZrotM(calc_mtx(), (s16)(i_this->mJointBaseRot.z + i_this->mJointRot[jntNo].z));
            /* model->setAnmMtx(jntNo, *calc_mtx) */
            J3DMtxBuffer_hd* buffer = model->mpMtxBuffer;
            u16 flags = buffer->mFlags;
            Mtx34* src = calc_mtx();
            buffer->mFlags = flags | 0x10;
            f32 t[12];
            for (int i = 0; i < 12; i++)
                t[i] = src->m[i / 4][i % 4];
            Mtx34* dst = gabi::at<Mtx34>(buffer->mpAnmMtx + jntNo * 0x30);
            for (int i = 0; i < 12; i++)
                dst->m[i / 4][i % 4] = t[i];
            PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
        }
    }
    return TRUE;
}

/* ga_draw (inlined into daKantera_Draw) */
static inline void ga_draw(kantera_class* i_this) {
    mo_ga_s* pGa = &i_this->mGa[0];
    for (s32 i = 0; i < 2; i++, pGa++) {
        if (pGa->m2E != 0) {
            MtxTrans(pGa->mPos.x, pGa->mPos.y, pGa->mPos.z, 0);
            cMtx_YrotM(calc_mtx(), pGa->mRotY);
            cMtx_XrotM(calc_mtx(), pGa->mRotX);
            f32 s = pGa->mScale;
            MtxScale(s, s * pGa->mScaleY, s, 1);
            /* HD: MtxTrans(0, REG10_F(9) - 2, 0) (GameCube: -18) */
            MtxTrans(0.0f, REG10_F(9) + -2.0f, 0.0f, 1);
            J3DModel_setBaseTRMtx(pGa->mpModel, calc_mtx());
            mDoExt_modelUpdateDL(pGa->mpModel);
        }
    }
}

// HD: ga_draw is inlined; no alpha-model halo, flame model or second brk.
/* 0218D14C */
static BOOL daKantera_Draw(kantera_class* i_this) {
    if (i_this->mState >= 10) {
        MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
        f32 fVar2 = i_this->mBonScale * (REG17_F(0) + 0.8f);
        cMtx_YrotM(calc_mtx(), i_this->mBonRot);
        MtxScale(fVar2, fVar2, fVar2, 1);
        PSMTXCopy(calc_mtx(), &i_this->mAlphaModelMtx);
        return TRUE;
    }
    if (i_this->mSwitchNo != 0) {
        return TRUE;
    }

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    J3DModel* pModel = i_this->mpModel1;
    setLightTevColorType(dKy_getEnvlight(), pModel, &i_this->tevStr);
    mDoExt_brkAnm* brk = i_this->mpBrkAnm1;
    mDoExt_brkAnm_entry(brk, J3DModel_getModelData(pModel), anm_frameCtrl(brk)->getFrame());
    mDoExt_modelEntryDL(pModel);

    MtxTrans(i_this->mBonPos.x, i_this->mBonPos.y, i_this->mBonPos.z, 0);
    f32 fVar2 = 4.06f * i_this->mBonScale * 0.25f;
    MtxScale(fVar2, fVar2, fVar2, 1);
    cMtx_YrotM(calc_mtx(), i_this->mBonRot);
    cMtx_XrotM(calc_mtx(), (s16)(i_this->mBonRot / 2));
    PSMTXCopy(calc_mtx(), &i_this->mAlphaModelMtx);
    ga_draw(i_this);
    return TRUE;
}

// HD: kantera_move, ga_move and the model update are inlined into Execute.
#define REG6_F(i) REG_F(6, i)
/* HD fopAcM_seStart inline where the actor is known non-null (only &eyePos is checked) */
static inline void fopAcM_seStart_nn(fopAc_ac_c* a, u32 id, u32 param) {
    if (gabi::ea(&a->eyePos) != 0)
        mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* JPABaseEmitter global scale (HD +8) */
static inline void JPABaseEmitter_setGlobalScale(u32 em, const cXyz* s) {
    KanteraEmitter* emitter = gabi::at<KanteraEmitter>(em);
    emitter->mGlobalScale.x = s->x;
    emitter->mGlobalScale.y = s->y;
    emitter->mGlobalScale.z = s->z;
}
/* player state byte read by the lantern (daPy_lk_c +0x2E0D: 1 = lantern dropped, 2 = thrown) */
static inline s8 holder_state(fopAc_ac_c* a) {
    return reinterpret_cast<KanteraHolder*>(a)->mLanternState;
}

/* flame light at the flame position */
static inline void bon_light(kantera_class* i_this) {
    i_this->mPlight.mPos.copy(i_this->mBonPos);
    i_this->mPlight.mColorR = 600;
    i_this->mPlight.mColorG = 400;
    i_this->mPlight.mColorB = 120;
    i_this->mPlight.mPower = 225.00001525878906f;
    i_this->mPlight.mFluctuation = 250.0f;
}
/* relax the hanging joints */
static inline void joint_relax(kantera_class* i_this) {
    cLib_addCalcAngleS2(&i_this->mJointBaseRot.x, i_this->mJointBaseRotTarget.x, 2, 0x320);
    cLib_addCalcAngleS2(&i_this->mJointBaseRot.z, i_this->mJointBaseRotTarget.z, 2, 0x320);
    cLib_addCalcAngleS2(&i_this->mJointBaseRotTarget.x, 0, 1, 0x5DC);
    cLib_addCalcAngleS2(&i_this->mJointBaseRotTarget.z, 0, 1, 0x5DC);
}
/* swing target: grows towards a (clamped to +-0x4000) */
static inline void swing_target(be<s16>* t, s16 a) {
    if (a >= 0) {
        if (*t <= a)
            *t = a;
    } else {
        if (*t >= a)
            *t = a;
    }
}

/* ga_move (inlined) */
static inline void ga_move(kantera_class* i_this) {
    f32 dx = (i_this->mBonPos.x - i_this->m2E4.x) * 0.3f;
    f32 dy = (i_this->mBonPos.y - i_this->m2E4.y) * 0.3f;
    f32 dz = (i_this->mBonPos.z - i_this->m2E4.z) * 0.3f;
    gabi::Local<cXyz> sp3C;
    sp3C->x = 0.0f;
    sp3C->y = 0.0f;
    sp3C->z = 10.0f;
    mo_ga_s* ga = &i_this->mGa[0];
    for (s32 i = 0; i < 2; i++, ga++) {
        if (ga->m2E == 0)
            continue;
        if (ga->m2F != 0) {
            ga->m2F = (u8)(ga->m2F - 1);
        } else {
            ga->m2F = (u8)gabi::ftoi(cM_rndF(5.0f));
            f32 r = cM_rndFX(150.0f);
            ga->m10.x = i_this->mBonPos.x + r;
            r = cM_rndFX(30.0f);
            ga->m10.y = i_this->mBonPos.y + r;
            r = cM_rndFX(150.0f);
            ga->m10.z = i_this->mBonPos.z + r;
        }
        gabi::Local<cXyz> sp18;
        cXyz_mi(&ga->m10, sp18, &ga->mPos);
        f32 x = sp18->x, z = sp18->z, y = sp18->y;
        cLib_addCalcAngleS2(&ga->mRotY, cM_atan2s(x, z), 2, 0x1000);
        s16 ax = cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
        cLib_addCalcAngleS2(&ga->mRotX, (s16)-ax, 2, 0x1000);
        cMtx_YrotS(calc_mtx(), ga->mRotY);
        cMtx_XrotM(calc_mtx(), ga->mRotX);
        gabi::Local<cXyz> sp30;
        MtxPosition(sp3C, sp30);
        ga->mPos.x = ga->mPos.x + (sp30->x + dx);
        ga->mPos.y = ga->mPos.y + (sp30->y + dy);
        ga->mPos.z = ga->mPos.z + (sp30->z + dz);
        u16 c = ga->m2C;
        ga->m2C = (s16)(c + 0x3E00);
        ga->mScaleY = cM_ssin(c);
    }
}

/* the falling lantern (state 5) */
static inline void fall_move(kantera_class* i_this) {
    cLib_addCalc2(&i_this->mOffsY, 55.0f, 1.0f, 4.0f);
    f32 sx = i_this->speed.x;
    f32 px = i_this->current.pos.x;
    f32 sz = i_this->speed.z;
    s16 ay = i_this->current.angle.y;
    f32 sy = i_this->speed.y;
    i_this->current.pos.x = px + sx;
    i_this->current.pos.y = i_this->current.pos.y + sy;
    i_this->current.pos.z = i_this->current.pos.z + sz;
    i_this->current.angle.y = (s16)(ay + 0x1F4);
    i_this->current.angle.x = (s16)(i_this->current.angle.x + 0x514);
    i_this->speed.y = sy - (REG6_F(10) + 2.0f);
    for (int i = 0; i < 2; i++) {
        cLib_addCalcAngleS2(&i_this->mJointRot[i].x, 0, 2, 0x320);
        cLib_addCalcAngleS2(&i_this->mJointRot[i].z, 0, 2, 0x320);
    }
}

/* 0218D3F8 */
static BOOL daKantera_Execute(kantera_class* i_this) {
    dComIfGp_get(); /* HD: leftover accessor call (result unused) */
    if (i_this->mSwitchNo != 0) {
        if (!dComIfGs_isSwitch(i_this->mSwitchNo - 1, fopAcM_GetRoomNo(i_this))) {
            return TRUE;
        }
        i_this->mSwitchNo = 0;
    }

    if (i_this->m35C != 0) {
        s16 c = (s16)(i_this->m35C - 1);
        i_this->m35C = c;
        if (c == 30) {
            dPa_followEcallBack_end(&i_this->mPtclCallBack0);
            dPa_followEcallBack_end(&i_this->mPtclCallBack1);
            c = i_this->m35C;
        }
        if (c == 0) {
            fopAcM_delete(i_this);
        }
    }

    /* ---- kantera_move ---- */
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    fopAc_ac_c* target = fopAcM_SearchByID(i_this->mTargetActorID);
    i_this->mBonRot = (s16)(i_this->mBonRot + 0x100);
    i_this->mAnimCounter = i_this->mAnimCounter + 1;

    s8 state = i_this->mState;
    switch (state) {
    case 0:
        if (i_this->mParam0 == 0x23) {
            i_this->mState = 1;
            break;
        } else if (i_this->mParam0 == 1) {
            state = 3;
        } else {
            state = 100;
        }
        i_this->mState = state;
        if (state >= 10)
            return TRUE;
        break;
    case 1: {
        if (target == nullptr)
            break;
        /* carried: follow the holder's hand */
        cXyz* hand = &reinterpret_cast<KanteraHolder*>(target)->mHandPos;
        i_this->current.pos.x = hand->x;
        i_this->current.pos.y = hand->y;
        i_this->current.pos.z = hand->z;
        s16 ty = target->current.angle.y;
        i_this->current.angle.y = ty;
        cMtx_YrotS(calc_mtx(), (s16)-ty);
        {
            gabi::Local<cXyz> sp38;
            gabi::Local<cXyz> sp6c;
            f32 z = i_this->mBonPos.z - i_this->m2E4.z;
            sp38->y = 0.0f;
            sp38->x = i_this->mBonPos.x - i_this->m2E4.x;
            sp38->z = z;
            MtxPosition(sp38, sp6c);
            s16 ax = (s16)gabi::ftoi(sp6c->z * 1.2f * 100.0f);
            s16 az = (s16)gabi::ftoi(sp6c->x * -1.2f * 100.0f);
            if (ax > 0x4000)
                ax = 0x4000;
            else if (ax < -0x4000)
                ax = -0x4000;
            if (az > 0x4000)
                az = 0x4000;
            else if (az < -0x4000)
                az = -0x4000;
            swing_target(&i_this->mJointBaseRotTarget.x, ax);
            swing_target(&i_this->mJointBaseRotTarget.z, az);
        }
        s32 n = i_this->mAnimCounter;
        i_this->mJointRot[1].x = (s16)gabi::ftoi(cM_ssin(n * 3000) * 3000.0f);
        i_this->mJointRot[1].z = (s16)gabi::ftoi(cM_scos(n * 2500) * 3000.0f);
        s8 hs = holder_state(target);
        if (hs != 1 && hs != 2) {
            if (i_this->mState >= 10)
                return TRUE;
            break;
        }
        i_this->mState = 5;
        if (holder_state(target) == 1) {
            i_this->speed.x = cM_rndFX(15.0f);
            i_this->speed.z = cM_rndFX(15.0f);
            i_this->speed.y = REG6_F(8) + 30.0f;
        } else {
            s16 a = cM_atan2s(player->current.pos.x - i_this->current.pos.x,
                              player->current.pos.z - i_this->current.pos.z);
            cMtx_YrotS(calc_mtx(), a);
            gabi::Local<cXyz> sp38;
            sp38->x = 0.0f;
            sp38->y = 0.0f;
            sp38->z = REG6_F(9) + 20.0f;
            MtxPosition(sp38, &i_this->speed);
            i_this->speed.y = REG6_F(8) + 10.0f;
        }
    }
        /* fallthrough */
    case 5: {
        fall_move(i_this);
        i_this->mAcch.CrrPos(dComIfG_Bgsp());
        i_this->mSph.SetC(&i_this->current.pos);
        dComIfG_Ccsp_Set(&i_this->mSph);
        if ((i_this->mAcch.m_flags & (dBgS_Acch::WALL_HIT | dBgS_Acch::GROUND_HIT)) == 0 &&
            !i_this->mSph.ChkAtHit()) {
            if (i_this->mState >= 10)
                return TRUE;
            break;
        }
        /* landed / hit: break, flame burst */
        gabi::Local<dBgS_GndChk> gndChk;
        dBgS_GndChk_ct(gndChk, {0x1001266C, 0x1001267C, 0x1001269C, 0x1001268C}, false);
        {
            gabi::Local<cXyz> pos;
            (void)pos;
        }
        cXyz* gpos = &gndChk->m_pos;
        f32 y = i_this->current.pos.y;
        f32 z = i_this->current.pos.z;
        f32 x = i_this->current.pos.x;
        gpos->z = z;
        gpos->x = x;
        gpos->y = y + 50.0f;
        f32 h = cBgS_GroundCross(dComIfG_Bgsp(), gndChk) + 2.5f;
        if (h != -1000000000.0f) {
            i_this->current.pos.y = h + 2.0f;
        }
        /* static cXyz l_scale(1, 1, 1) (function-local) */
        be<u32>& guard = kanteraStatics()->mParticleScaleGuard;
        cXyz* l_scale = &kanteraStatics()->mParticleScale;
        if (guard == 0) {
            guard = 1;
            l_scale->set(1.0f, 1.0f, 1.0f);
        }
        dComIfGp_particle_set(0x243A, &i_this->current.pos, nullptr, l_scale, 0xC8,
                              (dPa_levelEcallBack*)(void*)&i_this->mPtclCallBack0);
        dComIfGp_particle_set(0x043B, &i_this->current.pos, nullptr, l_scale, 0xC8,
                              (dPa_levelEcallBack*)(void*)&i_this->mPtclCallBack1);
        fopAcM_seStart_nn(i_this, 0x6905, 0);
        {
            gabi::Local<cXyz> sp50;
            sp50->x = i_this->current.pos.x;
            sp50->y = i_this->current.pos.y;
            sp50->z = i_this->current.pos.z;
            kanteraSoundSet(sp50, 0x96, fopAcM_GetID(i_this), 10);
        }
        /* ~dBgS_GndChk (inline): this TU's vtables, then cBgS_GndChk::~cBgS_GndChk */
        gndChk->cBgS_PolyInfo::__vtbl = 0x1001267C;
        gndChk->dBgS_GrpPassChk::__vtbl = 0x1001265C;
        gndChk->dBgS_PolyPassChk::__vtbl = 0x1001269C;
        i_this->mState = 10;
        i_this->m35C = 0x82;
        kanteraGroundDestroy(gndChk);
    }
        /* fallthrough */
    case 10: {
        if (i_this->mPtclCallBack0.mpEmitter != nullptr &&
            i_this->mPtclCallBack1.mpEmitter != nullptr) {
            s16 c = i_this->m35C;
            if (c > 0x28) {
                i_this->mSph.SetR(30.0f * i_this->mParticleScale.x);
                i_this->mSph.SetC(&i_this->current.pos);
                dComIfG_Ccsp_Set(&i_this->mSph);
                cLib_addCalc2(&i_this->mParticleScale.x, 2.0f, 0.2f, REG0_F(0) + 0.1f);
                cLib_addCalc2(&i_this->mParticleScale.z, 2.0f, 0.2f, REG0_F(0) + 0.1f);
                f32 s = i_this->mParticleScale.x;
                u32 em0 = gabi::ea(i_this->mPtclCallBack0.mpEmitter.get());
                i_this->mBonScale = s;
                JPABaseEmitter_setGlobalScale(em0, &i_this->mParticleScale);
                JPABaseEmitter_setGlobalScale(gabi::ea(i_this->mPtclCallBack1.mpEmitter.get()),
                                              &i_this->mParticleScale);
                f32 py = i_this->current.pos.y;
                i_this->mPlight.mPos.x = i_this->current.pos.x;
                i_this->mPlight.mColorR = 600;
                i_this->mPlight.mColorG = 400;
                i_this->mPlight.mColorB = 120;
                i_this->mPlight.mPos.y = py + 50.0f;
                i_this->mPlight.mPos.z = i_this->current.pos.z;
                i_this->mPlight.mPower = 262.5f;
                i_this->mPlight.mFluctuation = 250.0f;
            } else if (c < 0x1E) {
                cLib_addCalc0(&i_this->mBonScale, 1.0f, 0.1f);
            }
        } else {
            s16 c = i_this->m35C;
            if (c < 0x1E) {
                i_this->mPlight.mPower = 262.5f * ((f32)c / 30.0f);
            }
        }
        if (target != nullptr && holder_state(target) == 2) {
            if (i_this->m35C > 0x6E) {
                i_this->mSph.OnAtSPrmBit(2);
            } else {
                i_this->mSph.OffAtSPrmBit(2);
            }
        }
        {
            if (i_this->mState >= 10)
                return TRUE;
            break;
        }
    }
    default:
        if (state >= 10)
            return TRUE;
        break;
    }

    joint_relax(i_this);
    bon_light(i_this);
    ga_move(i_this);

    /* ---- model update ---- */
    if (i_this->mState >= 10)
        return TRUE;
    MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
    cMtx_YrotM(calc_mtx(), i_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    MtxTrans(0.0f, i_this->mOffsY, 0.0f, 1);
    {
        J3DModel* model = i_this->mpModel1;
        J3DModel_setBaseScale(model, &i_this->scale);
        J3DModel_setBaseTRMtx(model, calc_mtx());
        J3DModel_calc(model); /* HD: after the base matrix (GameCube: model->calc() first) */
        i_this->eyePos.copy(i_this->current.pos);
        PSMTXCopy(kanteraAnmMtx(model, MK_KANTERA_JNT_KANTERA_e), calc_mtx());
    }
    i_this->m2E4.copy(i_this->mBonPos);
    {
        gabi::Local<cXyz> sp08;
        sp08->x = 0.0f;
        sp08->y = -40.0f;
        sp08->z = 0.0f;
        MtxPosition(sp08, &i_this->mBonPos);
    }
    mDoExt_baseAnm_play(i_this->mpBrkAnm1); /* HD: no second brk */
    return TRUE;
}

/* 0218E328 */
static BOOL daKantera_IsDelete(kantera_class*) { return TRUE; }

/* 0218E330 */
static BOOL daKantera_Delete(kantera_class* i_this) {
    i_this->mPtclCallBack0.remove();
    i_this->mPtclCallBack1.remove();
    dComIfG_resDelete(&i_this->mPhase, l_arcName);
    dKy_plight_cut(&i_this->mPlight);
    return TRUE;
}

/* 0218E39C */
static BOOL daKantera_CreateHeap(fopAc_ac_c* a_this) {
    kantera_class* i_this = (kantera_class*)a_this;

    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(
        l_arcName, dRes_INDEX_KANTERA_BMD_MK_KANTERA_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(1054, modelData != NULL) */
        JUT_ASSERT_fail(l_fileName, 0x41E, l_modelDataAssert);

    i_this->mpModel1 = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->mpModel1 == nullptr) {
        return FALSE;
    }

    J3DAnmTevRegKey* anm_res_brk = (J3DAnmTevRegKey*)dComIfG_getObjectRes(
        l_arcName, dRes_INDEX_KANTERA_BRK_MK_KANTERA_e, SAFESTRING_VTBL);
    if (anm_res_brk == nullptr) /* JUT_ASSERT(1077, anm_res_brk != NULL) */
        JUT_ASSERT_fail(l_fileName, 0x435, l_brkAssert);

    void* p = operator_new(0x78);
    if (p != nullptr) {
        p = kanteraBrkConstruct(p);
    }
    mDoExt_brkAnm* brk = (mDoExt_brkAnm*)p;
    i_this->mpBrkAnm1 = brk;
    if (brk == nullptr) {
        return FALSE;
    }
    if (!kanteraBrkInit(brk, modelData, anm_res_brk, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1,
                        false, 0)) {
        return FALSE;
    }

    modelData = (J3DModelData*)dComIfG_getObjectRes(l_arcName, dRes_INDEX_KANTERA_BMD_LF_e,
                                                    SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(1089, modelData != NULL) */
        JUT_ASSERT_fail(l_fileName, 0x441, l_modelDataAssert);

    i_this->mpModel2 = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->mpModel2 == nullptr) {
        return FALSE;
    }

    modelData = (J3DModelData*)dComIfG_getObjectRes(l_arcName, dRes_INDEX_KANTERA_BMD_GA_e,
                                                    SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(1168, modelData != NULL) */
        JUT_ASSERT_fail(l_fileName, 0x490, l_modelDataAssert);

    for (s32 i = 0; i < 2; i++) {
        J3DModel* m = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        i_this->mGa[i].mpModel = m;
        if (m == nullptr) {
            return FALSE;
        }
        if (cM_rndF(1.0f) < 0.5f) {
            i_this->mGa[i].m2E = 1;
            i_this->mGa[i].mPos.copy(i_this->current.pos);
            i_this->mGa[i].mScale = cM_rndF(0.3f) + 0.3f;
            i_this->mGa[i].m2C = (s16)gabi::ftoi(cM_rndF(30000.0f));
        }
    }
    return TRUE;
}

/* 0218E684 */
static kantera_class* kantera_class_ct(kantera_class* i_this) {
    if (i_this == nullptr) {
        i_this = (kantera_class*)operator_new(0x890);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = KANTERA_VTBL;
    i_this->mPlight.mHD20 = 1.0f; /* HD: LIGHT_INFLUENCE constructor */
    dBgS_AcchCir_ct(&i_this->mAcchCir);
    dBgS_ObjAcch_ct(&i_this->mAcch, {0x100126AC, 0x100126CC, 0x100126BC});
    dCcD_Stts_ct(&i_this->mStts);
    kanteraSphereConstruct(&i_this->mSph);
    dPa_followEcallBack_ct(&i_this->mPtclCallBack0, 0, 0);
    dPa_followEcallBack_ct(&i_this->mPtclCallBack1, 0, 0);
    /* cXyz mParticleScale: GHS's null-checked member constructor */
    if (gabi::ea(&i_this->mParticleScale) == 0)
        operator_new(0xC);
    return i_this;
}

/* 0218E780 */
static cPhs_State daKantera_Create(fopAc_ac_c* a_this) {
    kantera_class* i_this = (kantera_class*)a_this;

    /* fopAcM_ct(a_this, kantera_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr)
            kantera_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    u32 prm = fopAcM_GetParam(a_this);
    i_this->mParam0 = (u8)prm;
    i_this->mParam1 = (u8)(prm >> 8);
    i_this->mParam2 = (u8)(prm >> 0x18);

    cPhs_State PVar1 = dComIfG_resLoad(&i_this->mPhase, l_arcName);
    if (PVar1 == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(a_this, daKantera_CreateHeap, 0x10000)) {
            return cPhs_ERROR_e;
        }

        J3DModel* model = i_this->mpModel1;
        for (u16 i = 0; i < model->getModelData()->getJointNum(); i++) {
            model->getModelData()->getJointNodePointer(i)->setCallBack(kantera_nodeCallBack);
        }

        /* fopAcM_OffStatus(a_this, 0): no effect */
        J3DModel_setBaseScale(i_this->mpModel2, &a_this->scale);
        if (i_this->mParam2 != 0xFF) {
            i_this->mSwitchNo = i_this->mParam2 + 1;
        }

        a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpModel1)); /* fopAcM_SetMtx */
        fopAcM_setCullSizeBox(a_this, -60.0f, -100.0f, -60.0f, 60.0f, 50.0f, 60.0f);
        model->mUserArea = gabi::ea(a_this);
        i_this->m2B0 = (s16)gabi::ftoi(cM_rndF(100.0f));
        i_this->m2B4 = (s16)gabi::ftoi(cM_rndF(100.0f));
        dKy_plight_set(&i_this->mPlight);
        i_this->mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir,
                          &a_this->speed);
        i_this->mAcchCir.SetWall(0.0f, 30.0f);
        i_this->mStts.Init(0xE6, 0xFF, a_this);

        if (i_this->mParam0 == 7) {
            a_this->scale.x = 0.0f;
            a_this->scale.y = 0.0f;
            a_this->scale.z = 0.0f;
            i_this->mParam0 = 1;
        }
        i_this->mSph.Set(reinterpret_cast<dCcD_SrcSph*>(&at_sph_src));
        i_this->mSph.SetStts(&i_this->mStts);
        kanteraExecuteCall(i_this);
    }
    return PVar1;
}

/* 0218EA1C */
static void __sinit_d_a_kantera_cpp() {
    /* header statics; in this unit the {-pi, pi} pair sits 4 bytes below the usual place */
    for (int i = 0; i < 4; i++)
        kanteraStatics()->mZero[i] = 0;
    __register_global_object(0x101B7F4C);
    kanteraStatics()->mNegativePi = -3.1415927f;
    kanteraStatics()->mPi = 3.1415927f;
    kanteraMathInit(&kanteraStatics()->mMathInit);
    __register_global_object(0x101B7F58);
    kanteraRandomInit(&kanteraStatics()->mRandomInit);
    __register_global_object(0x101B7F64);
}

/* 0218EAB0 */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 0218EAC4 */
static void kantera_class_dt(kantera_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        kanteraSphereDestroy(&i_this->mSph);
        dCcD_Stts_dt(&i_this->mStts, 2);
        /* dBgS_ObjAcch::~dBgS_ObjAcch (inline): this TU's vtables, then dBgS_Acch::~dBgS_Acch */
        i_this->mAcch.dBgS_GrpPassChk::__vtbl = 0x100126BC;
        i_this->mAcch.dBgS_PolyPassChk::__vtbl = 0x100126CC;
        kanteraAcchDestroy(&i_this->mAcch);
        kanteraCircleDestroy(&i_this->mAcchCir.m_cir);
        kanteraActorDestroy(i_this);
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 0218EB60 */
static void SafeString_assureTerminationImpl(void*) {}

// clang-format off
/* WWHD glue
str 1001277C "Kantera" in 0218E39C
str 1001276C "Kantera" in 0218E330
str 100127C8 "Kantera" in 0218E780
str 10012784 "d_a_kantera.cpp"
str 10012794 "modelData != (0)"
str 100127A8 "anm_res_brk != (0)"
data at_sph_src 101B7F0C
*/
// clang-format on
