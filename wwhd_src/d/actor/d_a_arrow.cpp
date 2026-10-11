/**
 * d_a_arrow.cpp (WWHD)
 * Item - Arrow (Link's arrows, and Zelda's light arrows in the Ganondorf fight)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_arrow.cpp) to the WWHD layout and verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_arrow.h"

const gc_tu_data gc_tu = {.safeString = 0x10007210};

const char daArrow_c::m_arc_name[] = "Link";

static const s16 l_useMpRest[4] = {0, 1, 1, 2};
static const u32 l_heapSize[4] = {4864, 4864, 4864, 2080};
static const s32 l_shootSe[8] = {0, 0, 10354, 10357, 10355, 10358, 10356, 10359};
static const s16 l_useMp[4] = {0, -1, -1, -2};
static const u8 l_arrowColor[4] = {0, 0, 1, 2};
static u8 l_keepType;
static s16 l_count;
static ArrowAttackTable l_arrowMat;
static ArrowCapsuleSource l_atCpsSrc;
static ArrowSphereSource l_coSphSrc;
static u32 l_offsetGuard;
static cXyz l_offsetArrowPos;
static u8 l_stayNo;

static inline void dCcMassS_Mng_Set(void *obj, u8 prio) {
    ArrowMassSet(gabi::at<void>(dComIfGp_ea() + PLAY_CCMASS), obj, prio);
}

// HD: save access starts at the global owner, before the embedded dSv_info_c.
static inline u32 savep() {
    return dComIfGs_ea();
}
/* daPy_py_c virtuals through the HD vtable at +0xB4 */
static inline u32 vfunc(void *obj, u32 slot) {
    return gabi::at<ArrowPlayerVtable>((u32)gabi::at<fopAc_ac_c>(gabi::ea(obj))->__vtbl)
        ->mFunctions[slot / 4];
}
/* J3DModelData joint array behind the model (+0xAC: {?, u32 count, entries of 0x1C}); an index out
 * of range yields entry 0 (bounds-checked buffer accessor) */
static inline u32 joint_entry(u32 jointBuf, u32 idx) {
    u32 base = gabi::at<ArrowJointBuffer>(jointBuf)->mpEntries;
    if (idx < gabi::at<ArrowJointBuffer>(jointBuf)->mCount)
        base += idx * 0x1C;
    return base;
}

static inline u32 self(const void *p) {
    return gabi::ea(p);
}

/* shape of a material: hide()/show() is the visible byte at +4 */
static inline void shape_show(u32 mat, u8 on) {
    gabi::at<J3DShape>((u32)gabi::at<ArrowMaterial>(mat)->mpShape)->mVisible = on;
}

/* HD: fopAcM_seStartCurrent without the null checks */
static inline void seStartCurrent(fopAc_ac_c *a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
    mDoAud_seStart(id, &a->current.pos, param, reverb);
}

static inline bool bow_has_magic_arrows() {
    u8 item = dComIfGs_getItem(12); /* dComIfGs_getItem(dInvSlot_BOW_e) */
    return item == dItemNo_MAGIC_ARROW_e || item == dItemNo_LIGHT_ARROW_e;
}

/* cLib_calcTimer<s16>: this TU's copy */
static s16 arrow_calcTimer(be<s16> *t);
/* dComIfGp_particle_setP1(id, pos, angle): dPa_control_c::set on group 1 */
static inline JPABaseEmitter *particle_setP1(u16 id, const cXyz *pos, const csXyz *angle = nullptr) {
    return dPa_control_set(dComIfGp_getParticle(), 1, id, pos, angle, nullptr, 0xFF, nullptr, -1, nullptr,
                           nullptr, nullptr);
}
static inline f32 vec_abs(const cXyz *v) {
    return std_sqrtf(PSVECSquareMag(v));
}
/* speed.absXZ(): a temporary cXyz(x, 0, z) */
static inline f32 absXZ(f32 x, f32 z) {
    gabi::Local<cXyz> t;
    t->x = x;
    t->y = 0.0f;
    t->z = z;
    return vec_abs(t);
}
static inline void set_model_mtx(daArrow_c *a) {
    mDoMtx_stack_c::transS(a->current.pos.x, a->current.pos.y, a->current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), a->shape_angle.x, a->shape_angle.y, 0);
    J3DModel_setBaseTRMtx(a->mpModel, mDoMtx_stack_c::get());
}
static inline void at_cps_set(daArrow_c *a, cXyz *end) {
    cM3dGCps_Set(a->mAtCps.mCps, &a->current.pos, end, 5.0f); /* SetStartEnd + SetR(5.0f) */
    ArrowCapsuleGeometry *capsule = gabi::at<ArrowCapsuleGeometry>(gabi::ea(a->mAtCps.mCps));
    PSVECSubtract(&capsule->mEnd, &capsule->mStart, &a->mAtCps.mGObjAt.mVec); /* CalcAtVec() */
    dComIfG_Ccsp_Set(&a->mAtCps);
    dCcMassS_Mng_Set(&a->mAtCps, 1);
}

static BOOL daArrowExecute(void *i_this);

/* HD: mCurrProcFunc == &daArrow_c::procWater (pointer-to-member compare) */
static inline bool proc_is(daArrow_c *a, u32 fn) {
    return a->mCurrProcFunc.i == -1 && a->mCurrProcFunc.d == 0 && a->mCurrProcFunc.f == fn;
}
/* bounce off: speed *= -0.1f; speed.y += speed.absXZ(); current.pos = old.pos */
static inline void rebound(daArrow_c *a) {
    a->setProc(A_procReturn);
    a->mParameters = 3; /* fopAcM_SetParam(this, 3) */
    PSVECScale(&a->speed, &a->speed, -0.1f);
    f32 xz = absXZ(a->speed.x, a->speed.z);
    f32 sy = gabi::fadds_ppc(a->speed.y, xz);
    a->current.pos.copy(a->old.pos);
    a->field_0x69c = 0x2C00;
    a->speed.y = sy;
}

// HD: the verified static magic check has no actor argument.
static void checkRestMp();

/* 02052268 */
static BOOL createHeap_CB(fopAc_ac_c *i_this) {
    return static_cast<daArrow_c *>(i_this)->_createHeap();
}

/* 020521BC */
BOOL daArrow_c::_createHeap() {
    u16 modelFileIndex;
    if (mArrowType == TYPE_LIGHT) {
        modelFileIndex = dRes_INDEX_LINK_BDL_ARROWGLITTER_e;
    } else {
        modelFileIndex = dRes_INDEX_LINK_BDL_ARROW_e;
    }
    J3DModelData *modelData =
        (J3DModelData *)dComIfG_getObjectRes(m_arc_name, modelFileIndex, gc_tu.safeString);
    if (modelData == nullptr) /* JUT_ASSERT(190, modelData != NULL) */
        JUT_ASSERT_fail("d_a_arrow.cpp", 0xBE, "modelData != (0)");

    J3DModel *model = mDoExt_J3DModel__create(modelData, 0x00080000, 0x11000022);
    mpModel = model;
    return model != nullptr;
}

/* 02052388 */
static void atHit_CB(fopAc_ac_c *i_this, dCcD_GObjInf *thisObjInf, fopAc_ac_c *hitActor,
                     dCcD_GObjInf *hitObjInf) {
    static_cast<daArrow_c *>(i_this)->_atHit(thisObjInf, hitActor, hitObjInf);
}

/* 0205226C */
void daArrow_c::_atHit(dCcD_GObjInf *thisObjInf, fopAc_ac_c *hitActor, dCcD_GObjInf *hitObjInf) {
    // Keep track of which actor this arrow hit as well as the position the hit occurred at.
    if (!hitActor) {
        return;
    }
    if (!fopAc_IsActor(hitActor)) {
        return;
    }
    if (hitActor->jntHit == 0) {
        return;
    }

    gabi::Local<cXyz> hitPos;
    hitPos->set(thisObjInf->mGObjAt.mHitPos.x, thisObjInf->mGObjAt.mHitPos.y, thisObjInf->mGObjAt.mHitPos.z);
    gabi::Local<cXyz> diff;
    cXyz_mi(hitPos, diff, &current.pos);
    f32 hitDist = std_sqrtf(PSVECSquareMag(diff));
    if (hitDist < mNearestHitDist) { // bge: unordered skips
        mNearestHitDist = hitDist;
        mHitActorProcID = fopAcM_GetID(hitActor);
        if (hitObjInf->mGObjTg.mSPrm & 1) { /* ChkTgShield */
            mbHitActor = false;
            mpAtHitActor = 0;
        } else {
            mpAtHitActor = gabi::ea(hitActor);
            mbHitActor = true;
        }
        mAtHitPos.copy(thisObjInf->mGObjAt.mHitPos);
    }
}

/* 0205238C */
void daArrow_c::checkCreater() {
    // Check if this arrow was fired by Princess Zelda (during the Ganondorf fight).
    gabi::Local<be<u32>> archer;
    if (fopAcM_SearchByID_out(parentActorID, archer)) {
        u32 a = *archer;
        if (a != 0 && gabi::at<fopAc_ac_c>(a)->mProcName == fpcNm_PZ_e) { /* HD: null check */
            mbSetByZelda = true;
        }
    }
}

/* 02052C40 */
void daArrow_c::setLightEffect() {
    u8 type = mArrowType;
    if (field_0x682 == type) {
        if (type == TYPE_NORMAL) {
            return;
        }
        if (!mbHasLightEff) {
            mLightEffPID = fopAcM_createChild(fpcNm_ARROW_LIGHTEFF_e, fopAcM_GetID(this), type, &field_0x6a8,
                                              current.roomNo, &shape_angle, nullptr, -1, 0);
            if (mLightEffPID != fpcM_ERROR_PROCESS_ID_e) {
                mbHasLightEff = true;
            }
        } else {
            field_0x682 = type;
            return;
        }
    } else {
        fopAcM_delete(fopAcM_SearchByID(mLightEffPID));
        mbHasLightEff = false;
    }
    field_0x682 = mArrowType;
}

/* 02053628 */
void daArrow_c::setBlur() {
    u32 blurEmitter = mBlurFollowCb.mpEmitter.v.get(); /* mBlurFollowCb.getEmitter() */
    if (!blurEmitter) {
        return;
    }
    s32 alpha = gabi::at<ArrowEmitter>(blurEmitter)->mGlobalAlpha; /* getGlobalAlpha() */
    alpha -= 50;
    if (alpha <= 0) {
        dPa_followEcallBack_end(gabi::at<dPa_followEcallBack>(self(&mBlurFollowCb))); /* remove() */
    } else {
        gabi::at<ArrowEmitter>(blurEmitter)->mGlobalAlpha = alpha; /* setGlobalAlpha() */
    }

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), mBlurAngle.x, mBlurAngle.y, mBlurAngle.z);
    /* blurEmitter->setGlobalRTMatrix() */
    JPASetRMtxTVecfromMtx(mDoMtx_stack_c::get(), gabi::at<ArrowEmitter>(blurEmitter)->mGlobalRotation,
                          &gabi::at<ArrowEmitter>(blurEmitter)->mGlobalTranslation);
}

/* 02053700 */
void daArrow_c::createBlur() {
    if (!mBlurFollowCb.mpEmitter.v.get()) {
        /* dComIfGp_particle_setP1(ID_IT_JN_ARW_BLUR00, &current.pos, NULL, NULL, 0xFF, &mBlurFollowCb) */
        dPa_control_set(dComIfGp_getParticle(), 1, 0x48, &current.pos, nullptr, nullptr, 0xFF,
                        gabi::at<dPa_levelEcallBack>(self(&mBlurFollowCb)), -1, nullptr, nullptr, nullptr);
    }
}

/* 02053770 */
void daArrow_c::setArrowShootSe() {
    u8 type = mArrowType;
    if (type == TYPE_NORMAL) {
        return;
    }
    seStartCurrent(this, l_shootSe[type * 2], 0);
    seStartCurrent(this, l_shootSe[mArrowType * 2 + 1], 0);
}

/* 020526A8 */
void daArrow_c::setDrawShapeMaterial() {
    /* static const ArrowAttackInfo arrow_mat[4] = {{atType, atp, tipJointIdx}} */
    u32 e = gabi::ea(&l_arrowMat.mEntries[mArrowType]);
    mAtCps.SetAtType(gabi::at<ArrowAttackInfo>(e)->mAtType);
    mAtCps.SetAtAtp(gabi::at<ArrowAttackInfo>(e)->mAtp);

    if (mbSetByZelda) {
        mAtCps.SetAtAtp(4);
        mAtCps.SetAtType(AT_TYPE_NORMAL_ARROW); /* arrow_mat[0].mAtType */
        mAtCps.OnAtSPrmBit(0xE);                /* cCcD_AtSPrm_GrpAll_e */
    }

    u16 tip = l_arrowMat.mEntries[mArrowType].mTipJointIdx;
    if (tip != 0) {
        u32 jnt = joint_entry(mpModel->mModelData.v.get(), tip);
        mpTipMat = gabi::at<ArrowJoint>(jnt)->mpMesh; /* getMesh() */
    }
}

/* 02053814 */
void daArrow_c::arrowShooting() {
    field_0x6a8.copy(current.pos);
    f32 xCos = cM_scos(current.angle.x);
    speed.x = 200.0f * cM_ssin(current.angle.y) * xCos;
    speed.y = 200.0f * cM_ssin(current.angle.x);
    speed.z = 200.0f * cM_scos(current.angle.y) * xCos;

    setArrowShootSe();

    if (mArrowType == TYPE_LIGHT && !mbSetByZelda) {
        /* HD: strcmp(dComIfGp_getStartStageName(), "GanonK") on sead::SafeStrings: cstr() is virtual
         * (slot 0x14) */
        gabi::Local<SafeString> name;
        name->mStringTop = gabi::guest_str("GanonK");
        name->__vtbl = gc_tu.safeString;
        gabi::Local<SafeString> stage;
        stage->mStringTop = dComIfGp_ea() + 0x5134;
        stage->__vtbl = gc_tu.safeString;
        gabi::call_ptr(gabi::at<ArrowSafeStringVtable>((u32)name->__vtbl)->mpAssureTermination, name.get());
        gabi::call_ptr(gabi::at<ArrowSafeStringVtable>((u32)name->__vtbl)->mpAssureTermination, name.get());
        gabi::call_ptr(gabi::at<ArrowSafeStringVtable>((u32)stage->__vtbl)->mpAssureTermination, stage.get());
        u32 a = name->mStringTop;
        u32 b = stage->mStringTop;
        bool equal = true;
        if (a != b) {
            equal = false;
            for (u32 i = 0; i < 0x40001; i++) {
                u8 ca = gabi::at<be<u8>>(a)[i];
                u8 cb = gabi::at<be<u8>>(b)[i];
                if (ca != cb)
                    break;
                if (ca == 0) {
                    equal = true;
                    break;
                }
            }
        }
        if (!equal) {
            // Not in Puppet Ganon's boss room.
            mAtCps.SetAtSpl(0xB); /* dCcG_At_Spl_UNKB */
        }
    }

    u16 bx = shape_angle.x, by = shape_angle.y, bz = shape_angle.z;
    mBlurAngle.x = bx;
    mBlurAngle.y = by;
    mBlurAngle.z = bz;

    createBlur();

    field_0x602 = l_count;
    s16 count = l_count + 1;
    if (count == 5) {
        count = 0;
    }
    l_count = count;

    gabi::Local<cXyz> step;
    cXyz_ml(&speed, step, 1.25f);
    gabi::Local<cXyz> sum;
    cXyz_pl(&current.pos, sum, step);
    gabi::Local<cXyz> end;
    end->copy(*sum);
    at_cps_set(this, end);

    mAtHitPos.copy(*end); /* clrAtHitNormal(); setAtHitPosBuff(&end) */
    mbHitActor = false;
    mNearestHitDist = 3.4028235e+38f; /* FLOAT_MAX */
}

/* 02053AD0 */
void daArrow_c::arrowUseMp() {
    s16 use = l_useMp[mArrowType];
    /* dComIfGp_setItemMagicCount(use_mp[mArrowType]) */
    dComIfGp_setItemMagicCount(use);
}

/* 02053B18 */
void daArrow_c::ShieldReflect() {
    f32 vel = vec_abs(&speed);

    fopAc_ac_c *link = dComIfGp_getLinkPlayer();
    s16 targetAngleY =
        link->shape_angle.y + gabi::at<daPy_py_c>(gabi::ea(link))->mBodyAngle.y; /* getBodyAngleY() */
    s16 targetAngleX = gabi::at<daPy_py_c>(gabi::ea(link))->mBodyAngle.x;        /* getBodyAngleX() */

    gabi::Local<be<u32>> ganondorf;
    if (fopAcM_SearchByName(0xF6 /* fpcNm_GND_e */, ganondorf) && *ganondorf != 0) { /* HD: null check */
        dAttention_c *attention = dComIfGp_getAttention();
        if (dAttention_LockonTruth(attention) && dAttention_LockonTarget(attention, 0) == *ganondorf) {
            u32 g = *ganondorf;
            gabi::Local<cXyz> chest;
            fopAc_ac_c *gnd = gabi::at<fopAc_ac_c>(g);
            chest->x = gnd->current.pos.x;
            chest->y = gnd->current.pos.y;
            chest->z = gnd->current.pos.z;
            chest->y = gabi::fadds_ppc(REG_F(8, 0), 130.0f);
            s16 ax = cLib_targetAngleX(&link->current.pos, chest);
            gabi::at<fopAc_ac_c>(*ganondorf)->mParameters = 0x23; /* fopAcM_SetParam(ganondorf, 0x23) */
            targetAngleX = -ax;
            mSparkleTimer = 15 + REG0_S(3);
            mpSparkleEmitter =
                gabi::ea(dComIfGp_particle_set(0x3EE /* ID_AK_JN_CCTHUNDER01 */, &link->current.pos));
        }
    }

    f32 sinY = cM_ssin(targetAngleY), cosX = cM_scos(targetAngleX), sinX = cM_ssin(targetAngleX),
        cosY = cM_scos(targetAngleY);
    f32 sx = (sinY * cosX) * vel;
    f32 sy = (-sinX) * vel;
    f32 sz = (cosY * cosX) * vel;
    speed.x = sx;
    speed.y = sy;
    speed.z = sz;
    f32 xz = absXZ(sx, sz); /* speed.absXZ() */

    shape_angle.x = cM_atan2s(-speed.y, -xz);
    shape_angle.y = cM_atan2s(-speed.x, -speed.z);
    shape_angle.z = 0;
}

/* 020532B8 */
bool daArrow_c::check_water_in() {
    u8 prev_field_0x699 = field_0x699;
    u8 now = daPy_lk_c_setItemWaterEffect(this, prev_field_0x699, 1);
    field_0x699 = now;
    if (prev_field_0x699 != 0 || now == 0) { /* HD: any non-zero result */
        return FALSE;
    }

    gabi::Local<be<f32>> waterY;
    fopAcM_getWaterY(&current.pos, waterY);

    f32 curY = current.pos.y;
    f32 deltaY = std::fabs(old.pos.y - curY);
    f32 waterDist = std::fabs(*waterY - curY);
    gabi::Local<cXyz> waterHitPos;
    if (deltaY < 1.0f) { // bge: unordered takes the else branch
        waterHitPos->x = current.pos.x;
        waterHitPos->z = current.pos.z;
        waterHitPos->y = curY;
    } else {
        f32 weight = waterDist / deltaY;
        if (weight > 1.0f) { // ble: unordered keeps the weight
            weight = 1.0f;
        }
        gabi::Local<cXyz> a;
        cXyz_ml(&old.pos, a, weight);
        gabi::Local<cXyz> b;
        cXyz_ml(&current.pos, b, 1.0f - weight);
        gabi::Local<cXyz> sum;
        cXyz_pl(a, sum, b);
        waterHitPos->copy(*sum);
    }

    setProc(A_procWater);
    mParameters = 4; /* fopAcM_SetParam(this, 4) */

    u8 type = mArrowType;
    if (type == TYPE_FIRE) {
        mInWaterTimer = 1;
        particle_setP1(0x35A /* ID_AK_JN_EVAPORATION00 */, waterHitPos);
        if (!field_0x6e4) {
            dKy_arrowcol_chg_on(&current.pos, 0);
        }
    } else if (type == TYPE_ICE) {
        mInWaterTimer = 10 * 30;
        fopAcM_createChild(0x1D9 /* fpcNm_ARROW_ICEEFF_e */, fopAcM_GetID(this), mArrowType, waterHitPos,
                           current.roomNo, &current.angle, nullptr, -1, 0);
        if (!field_0x6e4) {
            dKy_arrowcol_chg_on(&current.pos, 1);
        }
    } else if (type == TYPE_LIGHT) {
        particle_setP1(0x2A1 /* ID_IT_JN_ARWG_HITA00 */, waterHitPos);
        seStartCurrent(this, 0x69DC /* JA_SE_OBJ_LIGHT_ARW_EFF */, 0);
        if (!field_0x6e4) {
            dKy_arrowcol_chg_on(&current.pos, 2);
        }
        mInWaterTimer = 1;
    } else {
        mInWaterTimer = 1;
    }

    field_0x698 = false;

    return TRUE;
}

// HD: keep the verified no-this ABI; autoglue currently treats qualified static methods as instance methods.
/* 02055774 */
static BOOL changeArrowMp() {
    u8 magic = dComIfGs_getMagic();
    return magic >= 1;
}

/* 0205584C */
daArrow_c *daArrow_c::changeArrowType() {
    u8 origArrowType = mArrowType;
    mBtkFrame = 0.0f;

    daArrow_c *ret = this;
    u8 type;

    if (origArrowType == TYPE_NORMAL) {
        if (dComIfGs_getMagic() == 0 || !bow_has_magic_arrows()) {
            mArrowType = type = TYPE_NORMAL;
        } else {
            mArrowType = type = TYPE_FIRE;
        }
    } else if (origArrowType == TYPE_FIRE) {
        if (dComIfGs_getMagic() == 0 || !bow_has_magic_arrows()) {
            mArrowType = type = TYPE_NORMAL;
        } else {
            mArrowType = type = TYPE_ICE;
        }
    } else if (origArrowType == TYPE_ICE) {
        u32 s = savep();
        if (gabi::at<dSv_info_c>(s + 0x20)->mSavedata.mPlayer.mPlayerStatusA.mMagic < 2 ||
            gabi::at<dSv_info_c>(s + 0x20)->mSavedata.mPlayer.mPlayerItem.mItems[12] !=
                dItemNo_LIGHT_ARROW_e) {
            mArrowType = type = TYPE_NORMAL;
        } else {
            mArrowType = type = TYPE_LIGHT;
        }
    } else if (origArrowType == TYPE_LIGHT) {
        mArrowType = type = TYPE_NORMAL;
    } else {
        type = origArrowType;
    }

    if (type != origArrowType) {
        l_keepType = type;
        daArrow_c *newNockedArrow = (daArrow_c *)fopAcM_fastCreate(
            fpcNm_ARROW_e, 0, &current.pos, fopAcM_GetRoomNo(this), nullptr, nullptr, -1, 0, 0);
        mArrowType = origArrowType;
        if (!newNockedArrow) {
            l_keepType = origArrowType;
            setDrawShapeMaterial();
            ret = this;
        } else {
            fopAcM_delete(this);
            ret = newNockedArrow;
        }
    }

    return ret;
}

/* 0205578C */
static void changeArrowTypeNotReady() {
    u8 keep = l_keepType;
    if (keep == daArrow_c::TYPE_NORMAL) {
        if (dComIfGs_getMagic() == 0) {
            return;
        }
        if (bow_has_magic_arrows()) {
            l_keepType = daArrow_c::TYPE_FIRE;
        }
    } else if (keep == daArrow_c::TYPE_FIRE) {
        if (dComIfGs_getMagic() == 0) {
            return;
        }
        if (bow_has_magic_arrows()) {
            l_keepType = daArrow_c::TYPE_ICE;
        }
    } else if (keep == daArrow_c::TYPE_ICE) {
        u32 s = savep();
        if (gabi::at<dSv_info_c>(s + 0x20)->mSavedata.mPlayer.mPlayerStatusA.mMagic >= 2 &&
            gabi::at<dSv_info_c>(s + 0x20)->mSavedata.mPlayer.mPlayerItem.mItems[12] ==
                dItemNo_LIGHT_ARROW_e) {
            l_keepType = daArrow_c::TYPE_LIGHT;
        } else {
            l_keepType = daArrow_c::TYPE_NORMAL;
        }
    } else if (keep == daArrow_c::TYPE_LIGHT) {
        l_keepType = daArrow_c::TYPE_NORMAL;
    }
}

/* 02052D04 */
void daArrow_c::setRoomInfo() {
    s32 roomNo;

    dBgS_GndChk_SetPos(&mGndChk, &current.pos);
    f32 groundY = cBgS_GroundCross(dComIfG_Bgsp(), &mGndChk);
    if (!(groundY == -1000000000.0f)) { /* -G_CM3D_F_INF */
        roomNo = dBgS_GetRoomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo *>(&mGndChk));
        s32 color = dBgS_GetPolyColor(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo *>(&mGndChk));
        current.roomNo = roomNo;
        tevStr.mRoomNo = roomNo;
        tevStr.mEnvrIdxOverride = color; /* mEnvrIdxOverride */
        mStts.mRoomId = roomNo;
    } else {
        roomNo = l_stayNo; /* dComIfGp_roomControl_getStayNo() */
        tevStr.mRoomNo = roomNo;
        mStts.mRoomId = roomNo;
        current.roomNo = roomNo;
    }
}

/* 0205244C */
void daArrow_c::setKeepMatrix() {
    // Transform the arrow onto its archer's hand.
    Mtx34 *mtx = mDoMtx_stack_c::get();
    if (mbSetByZelda) {
        gabi::Local<be<u32>> zelda;
        fopAcM_SearchByID_out(parentActorID, zelda);

        mDoMtx_stack_c::transS(0.7f, -0.07f, -0.2f);
        mDoMtx_XYZrotM(mtx, 0x238E, 0x2CDF, 0x29BE);
        u32 z = *zelda;
        if (z != 0) { /* HD: daPz_c::getRightHandMatrix() inline (null-checked) */
            u32 jnt =
                gabi::at<J3DModel>((u32)gabi::at<ArrowMorf>((u32)gabi::at<ArrowZelda>(z)->mpMorf)->mpModel)
                    ->mpMtxBuffer.v.get();
            u16 flags = gabi::at<J3DMtxBuffer_hd>(jnt)->mFlags;
            u32 m = gabi::at<J3DMtxBuffer_hd>(jnt)->mpAnmMtx;
            gabi::at<J3DMtxBuffer_hd>(jnt)->mFlags = flags | 0x10;
            PSMTXConcat(gabi::at<Mtx34>(m + 0x390), mtx, mtx);
        }
    } else {
        fopAc_ac_c *player = dComIfGp_getPlayer(0);
        mDoMtx_stack_c::transS(7.6f, -0.8f, -0.5f);
        mDoMtx_XYZrotM(mtx, -0x4F49, 0x238E, -0x6333);
        /* daPy_py_c::getLeftHandMatrix(): virtual (slot 0x14) */
        Mtx34 *hand = gabi::call_ptr<Mtx34 *>(vfunc(player, 0x14), player);
        PSMTXConcat(hand, mtx, mtx);
    }
    J3DModel_setBaseTRMtx(mpModel, mtx);
    current.pos.set(mtx->m[0][3], mtx->m[1][3], mtx->m[2][3]);
    mDoMtx_MtxToRot(mtx, &shape_angle);
    current.angle.x = -shape_angle.x;
    current.angle.y = shape_angle.y;
}

/* 02053DDC */
void daArrow_c::setStopActorMatrix() {
    s16 xRot = 0;
    if (arrow_calcTimer(&field_0x604) != 0) {
        s16 timer = field_0x604;
        f32 temp = (f32)timer / 40.0f;
        xRot = (s16)gabi::ftoi(((1024.0f * temp) * temp) * cM_ssin(timer * 0x52FB));
    }
    fopAc_ac_c *hitActor = fopAcM_SearchByID(mHitActorProcID);
    if (!hitActor) {
        return;
    }
    u32 jntHit = hitActor->jntHit;
    if (!jntHit) {
        return;
    }
    u32 hitModel = gabi::at<ArrowJointHit>(jntHit)->mpModel; /* jntHit->getModel() */

    // HD: lazily initialized arrow offset and its translation-unit guard.
    if (l_offsetGuard == 0) {
        l_offsetGuard = 1;
        l_offsetArrowPos.x = 0.0f;
        l_offsetArrowPos.y = 0.0f;
        l_offsetArrowPos.z = -50.0f;
    }
    cXyz *offset_arrow_pos = &l_offsetArrowPos;

    /* mDoMtx_stack_c::copy(hitModel->getAnmMtx(mHitJointIndex)) (HD: marks the joint matrices used) */
    s32 idx = mHitJointIndex;
    u32 mtxBuf = gabi::at<J3DModel>(hitModel)->mpMtxBuffer.v.get();
    u32 anm = gabi::at<J3DMtxBuffer_hd>(mtxBuf)->mpAnmMtx;
    gabi::at<J3DMtxBuffer_hd>(mtxBuf)->mFlags = gabi::at<J3DMtxBuffer_hd>(mtxBuf)->mFlags | 0x10;
    Mtx34 *stack = mDoMtx_stack_c::get();
    PSMTXCopy(gabi::at<Mtx34>(idx * 0x30 + anm), stack);
    gabi::Local<csXyz> hitJointRot;
    mDoMtx_MtxToRot(stack, hitJointRot);

    mDoMtx_stack_c::transM(field_0x618.x, field_0x618.y, field_0x618.z);
    mDoMtx_ZXYrotM(stack, field_0x6e6.x, field_0x6e6.y, field_0x6e6.z);

    f32 px = stack->m[0][3], py = stack->m[1][3], pz = stack->m[2][3]; /* multVecZero */
    current.pos.y = py;
    current.pos.x = px;
    current.pos.z = pz;

    mDoMtx_stack_c::transS(px, py, pz);
    mDoMtx_ZXYrotM(stack, hitJointRot->x, hitJointRot->y, hitJointRot->z);
    mDoMtx_ZXYrotM(stack, field_0x6e6.x, field_0x6e6.y, field_0x6e6.z);
    mDoMtx_XrotM(stack, xRot);
    mDoMtx_stack_c::transM(offset_arrow_pos->x, offset_arrow_pos->y, offset_arrow_pos->z);
    J3DModel_setBaseTRMtx(mpModel, stack);
}

/* 02053D24 */
BOOL daArrow_c::procWait() {
    speedF = 0.0f;
    setKeepMatrix();
    PSMTXCopy(mpModel ? &mpModel->mBaseTransformMtx : nullptr, &field_0x6b4);
    u16 sx = shape_angle.x, sy = shape_angle.y, sz = shape_angle.z;
    field_0x6e6.x = sx;
    field_0x6e6.y = sy;
    field_0x6e6.z = sz;

    if (fopAcM_GetParam(this) == 1) {
        if (!mbSetByZelda) {
            arrowUseMp();
            checkRestMp();
        }

        setProc(A_procMove);
        arrowShooting();
    }

    return TRUE;
}

/* 02054040 */
BOOL daArrow_c::procMove() {
    speedF = 100.0f;
    PSVECAdd(&current.pos, &speed, &current.pos);
    gabi::Local<cXyz> step;
    cXyz_ml(&speed, step, 0.25f);
    gabi::Local<cXyz> sum;
    cXyz_pl(&current.pos, sum, step);
    gabi::Local<cXyz> quarterStepPos;
    quarterStepPos->copy(*sum);
    dBgS_LinChk_Set(&mLinChk, &old.pos, quarterStepPos, this);
    u16 sy = shape_angle.y, sx = shape_angle.x, sz = shape_angle.z;
    field_0x6e6.z = sz;
    field_0x6e6.x = sx;
    field_0x6e6.y = sy;

    PSMTXCopy(mpModel ? &mpModel->mBaseTransformMtx : nullptr, &field_0x6b4);
    mBlurAngle.z = mBlurAngle.z + 0x889;

    void *poly = static_cast<cBgS_PolyInfo *>(&mLinChk);
    cXyz *crossP = &mLinChk.mLin.mEnd; /* mLinChk.GetCrossP() */
    dCcD_GObjAt *at = &mAtCps.mGObjAt;
    gabi::Local<cXyz> hitPos;
    s32 hitType = 0;      // No hit
    bool bg_check = true; /* HD: the paths join at the background line check */

    if (mAtCps.ChkAtHit()) {
        fopAc_ac_c *ac;
        if (mArrowType == TYPE_LIGHT && !mbLinkReflect && (at->mRPrm & 1) /* ChkAtShieldHit */ &&
            (ac = ArrowAtHitActor(at)) != nullptr && fpcM_GetName(ac) == 0xA8 /* fpcNm_PLAYER_e */) {
            ArrowAtHitActor(at); /* mAtCps.GetAtHitAc() */
            hitPos->copy(at->mHitPos);
            mbLinkReflect = true;
            ShieldReflect(); // Reflected hit
        } else {
            fopAc_ac_c *hitActor;
            BOOL hitWasBlocked;
            if (mbHitActor) {
                hitPos->copy(mAtHitPos);
                hitActor = gabi::at<fopAc_ac_c>(mpAtHitActor);
                hitWasBlocked = FALSE;
            } else {
                hitActor = ArrowAtHitActor(at);
                hitPos->copy(at->mHitPos);
                hitWasBlocked = at->mRPrm & 1;
            }

            if (hitActor) {
                u32 jntHit = hitActor->jntHit;
                if (mArrowType == TYPE_LIGHT) {
                    fopAc_ac_c *a;
                    bool ganon = ((a = ArrowAtHitActor(at)) != nullptr && fpcM_GetName(a) == 0xF3) ||
                                 ((a = ArrowAtHitActor(at)) != nullptr && fpcM_GetName(a) == 0xF4) ||
                                 ((a = ArrowAtHitActor(at)) != nullptr && fpcM_GetName(a) == 0xF5);
                    if (ganon && hitWasBlocked) {
                        // Hit Puppet Ganon (fpcNm_BGN_e / BGN2 / BGN3).
                        field_0x6a8.copy(*hitPos);
                        gabi::Local<cXyz> back;
                        cXyz_ml(&speed, back, 0.25f);
                        gabi::Local<cXyz> pos;
                        cXyz_mi(hitPos, pos, back);
                        current.pos.copy(*pos);

                        if (!field_0x6e4) {
                            dKy_arrowcol_chg_on(&current.pos, 2);
                        }

                        f32 vx = speed.x, vz = speed.z;
                        actor_status = actor_status | 0x4000; /* fopAcM_OnStatus(this, fopAcStts_UNK4000_e) */
                        setProc(A_procStop_BG);
                        mParameters = 2;
                        field_0x604 = 0x28;
                        /* HD: no GetTriPla call */

                        gabi::Local<csXyz> temp10;
                        f32 xz = absXZ(vx, vz);
                        temp10->x = cM_atan2s(speed.y, xz);
                        temp10->y = cM_atan2s(speed.x, speed.z);
                        temp10->z = 0;
                        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
                        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), temp10->x, temp10->y, 0);
                        PSMTXCopy(mDoMtx_stack_c::get(), &field_0x6b4);

                        particle_setP1(0x2A1 /* ID_IT_JN_ARWG_HITA00 */, &field_0x6a8, temp10);
                        seStartCurrent(this, 0x69DC /* JA_SE_OBJ_LIGHT_ARW_EFF */, 0);
                        fopAcM_delete(this);
                    }
                    // otherwise: no hit (pass through)
                } else if (hitWasBlocked) {
                    hitType = 1; // Blocked hit
                    field_0x604 = 0x28;
                    actor_status = actor_status | 0x4000;
                } else if (jntHit) {
                    gabi::Local<cXyz> temp12;
                    gabi::Local<csXyz> temp11;
                    s32 jnt =
                        ArrowSearchJointHit(jntHit, hitPos.get(), &shape_angle, temp12.get(), temp11.get());
                    mHitJointIndex = jnt;
                    if (jnt >= 0) {
                        u16 rx = temp11->x, ry = temp11->y;
                        field_0x6e6.x = rx;
                        field_0x6e6.y = ry;
                        u32 a = gabi::at<ArrowWords>(self(temp12.get()))->y,
                            b = gabi::at<ArrowWords>(self(temp12.get()))->z;
                        gabi::at<ArrowWords>(self(&field_0x618))->y = a;
                        gabi::at<ArrowWords>(self(&field_0x618))->z = b;
                        field_0x6e6.z = temp11->z;
                        actor_status = actor_status | 0x4000;
                        field_0x604 = 0x28;
                        hitType = 2; // Hit a joint
                        gabi::at<ArrowWords>(self(&field_0x618))->x =
                            (u32)gabi::at<ArrowWords>(self(temp12.get()))->x;
                    } else if (jnt == -3 /* JntHitIdx_DELETE_e */) {
                        fopAcM_delete(this);
                        return TRUE;
                    }
                }
            }
        }
    }

    if (hitType > 0) {
        bg_check = false;
        if (mBlurFollowCb.mpEmitter.v.get()) {
            dPa_followEcallBack_end(gabi::at<dPa_followEcallBack>(self(&mBlurFollowCb))); /* remove() */
        }

        if (hitType == 1) { // Blocked hit
            rebound(this);
            set_model_mtx(this);
            seStartCurrent(this, 0x287A /* JA_SE_LK_ARROW_REBOUND */, 0x20);
        } else if (hitType == 2) { // Hit a joint
            setProc(A_procStop_Actor);
            mParameters = 2;

            u8 type = mArrowType;
            if (type == TYPE_FIRE) {
                seStartCurrent(this, 0x69DA /* JA_SE_OBJ_FIRE_ARW_EFF */, 0);
                field_0x698 = false;
            } else if (type == TYPE_ICE) {
                seStartCurrent(this, 0x69DB /* JA_SE_OBJ_ICE_ARW_EFF */, 0);
            } else if (type == TYPE_LIGHT) {
                seStartCurrent(this, 0x69DC /* JA_SE_OBJ_LIGHT_ARW_EFF */, 0);
                field_0x698 = false;
            }

            setStopActorMatrix();
        }
    }

    if (bg_check) {
        if (cBgS_LineCross(dComIfG_Bgsp(), &mLinChk)) {
            field_0x6a8.copy(*crossP);
            gabi::Local<cXyz> back;
            cXyz_ml(&speed, back, 0.25f);
            gabi::Local<cXyz> pos;
            cXyz_mi(crossP, pos, back);
            current.pos.copy(*pos);

            if (!check_water_in()) {
                u8 type = mArrowType;
                if (type >= 1 && type <= 3 && !field_0x6e4) {
                    /* switch (mArrowType): FIRE 0, ICE 1, LIGHT 2, default -1 */
                    dKy_arrowcol_chg_on(&current.pos, l_arrowColor[type]);
                }

                setProc(A_procStop_BG);
                field_0x604 = 0x28;
                mParameters = 2;
                actor_status = actor_status | 0x4000;
                void *triPla = dBgS_GetTriPla(dComIfG_Bgsp(), poly);

                gabi::Local<csXyz> temp10;
                if (triPla) { /* HD: null check */
                    u32 n = gabi::ea(triPla);
                    f32 xz = absXZ(gabi::at<cM3dGPla>(n)->mNormal.x, gabi::at<cM3dGPla>(n)->mNormal.z);
                    temp10->x = cM_atan2s(-gabi::at<cM3dGPla>(n)->mNormal.y, -xz);
                    temp10->y =
                        cM_atan2s(-gabi::at<cM3dGPla>(n)->mNormal.x, -gabi::at<cM3dGPla>(n)->mNormal.z);
                    temp10->z = 0;
                } else {
                    temp10->y = 0;
                    temp10->z = 0;
                    temp10->x = 0;
                }
                mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
                mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), temp10->x, temp10->y, 0);
                PSMTXCopy(mDoMtx_stack_c::get(), &field_0x6b4);

                type = mArrowType;
                if (type == TYPE_FIRE) {
                    particle_setP1(0x29A /* ID_IT_JN_ARWF_HITA00 */, &field_0x6a8, temp10);
                    particle_setP1(0x29B /* ID_IT_JN_ARWF_HITB00 */, &field_0x6a8, temp10);
                    seStartCurrent(this, 0x69DA /* JA_SE_OBJ_FIRE_ARW_EFF */, 0);
                    field_0x698 = false;
                } else if (type == TYPE_ICE) {
                    if (dBgS_ChkGrpInf(dComIfG_Bgsp(), poly, 0x200)) {
                        fopAcM_create(0x2A /* fpcNm_Obj_Magmarock_e */, 0, &field_0x6a8, current.roomNo,
                                      nullptr, nullptr, -1, 0);
                    } else {
                        particle_setP1(0x29E /* ID_IT_JN_ARWI_HITA00 */, &field_0x6a8, temp10);
                        fopAcM_createChild(0x1D9 /* fpcNm_ARROW_ICEEFF_e */, fopAcM_GetID(this), mArrowType,
                                           &field_0x6a8, current.roomNo, &field_0x6e6, nullptr, -1, 0);
                        seStartCurrent(this, 0x69DB /* JA_SE_OBJ_ICE_ARW_EFF */, 0);
                    }
                } else if (type == TYPE_LIGHT) {
                    particle_setP1(0x2A1 /* ID_IT_JN_ARWG_HITA00 */, &field_0x6a8, temp10);
                    seStartCurrent(this, 0x69DC /* JA_SE_OBJ_LIGHT_ARW_EFF */, 0);
                    field_0x698 = false;
                }

                u32 attribCode = dBgS_GetAttributeCode(dComIfG_Bgsp(), poly);
                u32 mtrlSndId = dBgS_GetMtrlSndId(dComIfG_Bgsp(), (cBgS_PolyInfo *)poly);

                if (mArrowType == TYPE_NORMAL &&
                    (attribCode == 3 /* dBgS_Attr_STONE_e */ || attribCode == 9 /* METAL */ ||
                     attribCode == 0xF /* ICE */ ||
                     (attribCode >= 0x14 && attribCode <= 0x15) /* DAMAGE, FREEZE */)) {
                    rebound(this);
                    gabi::Local<csXyz> temp9;
                    cM3d_CalcVecZAngle(triPla, temp9); /* *triPla->GetNP() */

                    particle_setP1(0xC /* ID_AK_JN_NG */, &field_0x6a8, temp9);
                    seStartCurrent(this, 0x287A /* JA_SE_LK_ARROW_REBOUND */, mtrlSndId);
                } else {
                    seStartCurrent(this, 0x2879 /* JA_SE_LK_ARROW_HIT */, mtrlSndId);
                }
            }
        } else {
            check_water_in();
        }
    }

    if (proc_is(this, A_procWater)) {
        set_model_mtx(this);
    } else {
        gabi::Local<cXyz> d;
        cXyz_mi(&current.pos, d, &field_0x6a8);
        if (vec_abs(d) > 25000.0f) { // ble: unordered skips
            f32 vy = speed.y - 2.0f;
            if (vy < -100.0f) { // bge
                vy = -100.0f;
            }
            f32 posY = current.pos.y;
            f32 startY = field_0x6a8.y;
            speed.y = vy;
            if (startY > posY) { // ble
                fopAcM_delete(this);
                return TRUE;
            }
        } else {
            gabi::Local<cXyz> d2;
            cXyz_mi(&current.pos, d2, &field_0x6a8);
            if (vec_abs(d2) > 20000.0f) { // ble
                field_0x6e4 = true;
            }
        }

        createBlur();

        set_model_mtx(this);

        gabi::Local<cXyz> step2;
        cXyz_ml(&speed, step2, 1.25f);
        cXyz_pl(&current.pos, d, step2);
        gabi::Local<cXyz> end;
        end->copy(*d);
        at_cps_set(this, end);
    }

    return TRUE;
}

/* 02055450 */
BOOL daArrow_c::procReturn() {
    f32 sy = speed.y - 2.0f;
    speedF = 0.0f;
    speed.y = sy;
    PSVECAdd(&current.pos, &speed, &current.pos);
    shape_angle.x = shape_angle.x + field_0x69c;
    gabi::Local<cXyz> step;
    cXyz_ml(&speed, step, 0.25f);
    gabi::Local<cXyz> quarterStepPos;
    cXyz_pl(&current.pos, quarterStepPos, step);
    field_0x699 = daPy_lk_c_setItemWaterEffect(this, field_0x699, 1);
    dBgS_LinChk_Set(&mLinChk, &old.pos, quarterStepPos, this);
    setBlur();

    if (cBgS_LineCross(dComIfG_Bgsp(), &mLinChk)) {
        void *triPla = dBgS_GetTriPla(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo *>(&mLinChk));
        if (triPla) { /* HD: null check */
            f32 temp2 = vec_abs(&speed);
            gabi::Local<cXyz> temp1;
            C_VECReflect(&speed, triPla, temp1);
            speed.y = gabi::fmuls_ppc(gabi::fmuls_ppc(temp1->y, temp2), 0.5f);
            speed.x = gabi::fmuls_ppc(gabi::fmuls_ppc(temp1->x, temp2), 0.5f);
            speed.z = gabi::fmuls_ppc(gabi::fmuls_ppc(temp1->z, temp2), 0.5f);

            s32 temp3 = -field_0x69c;
            field_0x69c = (s16)(temp3 / 2);
            if (!(gabi::at<cM3dGPla>(gabi::ea(triPla))->mNormal.y <
                  0.5f)) { /* cBgW_CheckBGround(triPla->GetNP()->y) */
                field_0x69a = true;
            }
        }
    } else if (field_0x69a && speed.y < 0.0f) { // bge: unordered skips
        fopAcM_delete(this);
    }

    set_model_mtx(this);

    return TRUE;
}

/* 02054FE0 */
BOOL daArrow_c::procStop_BG() {
    speedF = 0.0f;
    void *poly = static_cast<cBgS_PolyInfo *>(&mLinChk); /* mLinChk's cBgS_PolyInfo */

    if (!cBgS_ChkPolySafe(dComIfG_Bgsp(), poly)) {
        fopAcM_delete(this);
        return TRUE;
    }

    BOOL temp2 = FALSE;

    s16 t = field_0x604;
    if (t > 0) {
        t = t - 1;
        f32 a = (f32)t * (1 / 40.0f);
        f32 base = (f32)(s16)field_0x6e6.x;
        f32 amp = (1024.0f * a) * a;
        field_0x604 = t;
        s16 sx = (s16)gabi::ftoi(gabi::fmadds(amp, cM_ssin(t * 0x52FB), base));
        s16 sy = field_0x6e6.y;
        shape_angle.x = sx;
        shape_angle.y = sy;
        temp2 = TRUE;
    } else if (field_0x600) {
        fopAcM_delete(this);
        return TRUE;
    }

    setBlur();

    if (dBgS_ChkMoveBG(dComIfG_Bgsp(), poly)) {
        dBgS_MoveBgTransPos(dComIfG_Bgsp(), poly, true, &field_0x6a8, &current.angle, &shape_angle);
        temp2 = TRUE;
    }

    if (temp2) {
        f32 xCos = cM_scos(shape_angle.x);
        f32 ax = 50.0f * cM_ssin(shape_angle.y);
        current.pos.x = gabi::fnmsubs(ax, xCos, field_0x6a8.x);
        current.pos.y = gabi::fmadds(50.0f, cM_ssin(shape_angle.x), field_0x6a8.y);
        f32 az = 50.0f * cM_scos(shape_angle.y);
        current.pos.z = gabi::fnmsubs(az, xCos, field_0x6a8.z);

        set_model_mtx(this);
    }

    if (mArrowType == TYPE_NORMAL) {
        cM3dGSph_SetC(&mCoSph.mSph, &current.pos);
        dComIfG_Ccsp_Set(&mCoSph);

        s32 n = field_0x6a0;
        if (n == 0) {
            field_0x600 = true;
            field_0x698 = false;
        } else {
            n--;
            field_0x6a0 = n;

            if (n < 60) {
                if ((n & 1) == 0) {
                    field_0x698 = false;
                } else {
                    field_0x698 = true;
                }
            } else {
                field_0x698 = true;
            }
        }

        if (mCoSph.ChkCoHit()) {
            /* dComIfGp_setItemArrowNumCount(1) */
            dComIfGp_setItemArrowNumCount(1);
            fopAcM_createItemForSimpleDemo(&current.pos, 0x10 /* dItemNo_ARROW_10_e */, -1, nullptr, nullptr,
                                           0.0f, 0.0f);
            mDoAud_seStart_noPos(0x827 /* JA_SE_CONSUMP_ITEM_GET */);
            fopAcM_delete(this);
            return TRUE;
        }
    }

    if (mbSetByZelda) {
        field_0x600 = true;
    }

    return TRUE;
}

/* 020553C0 */
BOOL daArrow_c::procStop_Actor() {
    speedF = 0.0f;
    if (field_0x600) {
        fopAcM_delete(this);
        return TRUE;
    }

    setBlur();

    if (fopAcM_SearchByID(mHitActorProcID)) {
        setStopActorMatrix();
    } else {
        field_0x600 = true;
    }

    return TRUE;
}

/* 02055734 */
BOOL daArrow_c::procWater() {
    if (mInWaterTimer <= 0) {
        fopAcM_delete(this);
        return TRUE;
    }

    mInWaterTimer = mInWaterTimer - 1;
    return TRUE;
}

/* 020523E8 */
static void checkRestMp() {
    u8 keep = l_keepType;
    if ((s32)dComIfGs_getMagic() < l_useMpRest[keep]) {
        l_keepType = daArrow_c::TYPE_NORMAL;
    }
}

/* 02052420 */
void daArrow_c::setTypeByPlayer() {
    checkRestMp();
    mArrowType = l_keepType;
}

/* 02052738 */
BOOL daArrow_c::createInit() {
    u8 type = mArrowType;
    fopAc_ac_c *link = dComIfGp_getLinkPlayer();
    if (type == TYPE_LIGHT) {
        mpBtk = gabi::ea(link) + 0x4720; /* daPy_lk_c::getLightArrowBtk() */
    } else {
        mpBtk = gabi::ea(link) + 0x46AC; /* daPy_lk_c::getIceArrowBtk() */
    }

    setProc(A_procWait);

    setKeepMatrix();
    cullMtx = mpModel ? gabi::ea(mpModel) + 0xC8 : 0; /* fopAcM_SetMtx(this, mpModel->getBaseTRMtx()) */
    fopAcM_SetMin(this, -6.0f, -6.0f, 0.0f);
    fopAcM_SetMax(this, 6.0f, 6.0f, 65.0f);

    mStts.Init(10, 0xFF, this);
    dCcD_Cps_Set(&mAtCps, gabi::ea(&l_atCpsSrc));
    mAtCps.SetStts(&mStts);
    mAtCps.mGObjAt.mHitCallback = A_atHit_CB;
    dCcD_Sph_Set(&mCoSph, gabi::ea(&l_coSphSrc));
    mCoSph.SetStts(&mStts);

    field_0x602 = -1;

    if (mArrowType != TYPE_LIGHT) {
        u32 joints = mpModel->mModelData.v.get();
        shape_show(gabi::at<ArrowJoint>(joint_entry(joints, 4))->mpMesh, 0); /* ARROW_JNT_TIPNOMAL_e */
        shape_show(gabi::at<ArrowJoint>(joint_entry(joints, 2))->mpMesh, 0); /* ARROW_JNT_TIPFIRE_e */
        shape_show(gabi::at<ArrowJoint>(joint_entry(joints, 3))->mpMesh, 0); /* ARROW_JNT_TIPICE_e */
        shape_show(gabi::at<ArrowMaterial>((u32)gabi::at<ArrowJoint>(joint_entry(joints, 3))->mpMesh)->mpNext,
                   0); /* its next material */
    }
    setDrawShapeMaterial();

    field_0x698 = true;
    field_0x699 = 0;
    field_0x69a = false;
    field_0x69c = 0;
    field_0x6a0 = 300;
    mInWaterTimer = 0;
    field_0x6e4 = false;
    mbHasLightEff = false;
    mbLinkReflect = false;
    field_0x604 = 0;

    return TRUE;
}

/* 02052DB8 */
BOOL daArrow_c::_execute() {
    if (mbSetByZelda) {
        if (!mbLinkReflect) {
            fopAc_ac_c *link = dComIfGp_getLinkPlayer();
            /* daPy_py_c::checkPlayerGuard(): virtual (slot 0x3C) */
            if (gabi::call_ptr<BOOL>(vfunc(link, 0x3C), link)) {
                mAtCps.SetAtSpl(0); /* dCcG_At_Spl_UNK0 */
                mAtCps.SetAtType(AT_TYPE_NORMAL_ARROW);
            } else {
                mAtCps.SetAtSpl(0xB); /* dCcG_At_Spl_UNKB */
                mAtCps.SetAtType(AT_TYPE_LIGHT_ARROW);
            }
        } else {
            mAtCps.SetAtSpl(0);
            mAtCps.SetAtType(AT_TYPE_NORMAL_ARROW);
        }
    }

    s16 sparkle = mSparkleTimer;
    if (sparkle != 0) {
        mSparkleTimer = sparkle - 1;
        fopAc_ac_c *player = dComIfGp_getPlayer(0);

        gabi::Local<cXyz> offset;
        offset->x = 0.0f;
        offset->y = gabi::fadds_ppc(REG0_F(8), 45.0f);
        offset->z = gabi::fadds_ppc(REG0_F(9), 30.0f);
        mDoMtx_YrotS(calc_mtx(), player->shape_angle.y);
        gabi::Local<cXyz> offsetOut;
        MtxPosition(offset, offsetOut);
        gabi::Local<cXyz> pos;
        cXyz_pl(&player->current.pos, pos, offsetOut);
        f32 px = pos->x, py = pos->y, pz = pos->z;
        current.pos.y = py;
        current.pos.z = pz;
        current.pos.x = px;

        mDoMtx_stack_c::transS(px, py, pz);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, 0);
        J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());

        u32 e = mpSparkleEmitter;
        if (!e) {
            return TRUE;
        }
        s16 t = mSparkleTimer;
        if (t != 0) {
            f32 scaleMag = (f32)t;
            scaleMag = scaleMag + scaleMag; /* mSparkleTimer * 2.0f */
            if (scaleMag > 7.0f) {
                scaleMag = 7.0f;
            }
            /* mpSparkleEmitter->setGlobalTranslation(current.pos) (HD: y negated from version 7 on) */
            f32 tz = current.pos.z, ty = current.pos.y, tx = current.pos.x;
            if (gabi::at<ArrowEmitter>(e)->mVersion >= 7) {
                ty = -ty;
            }
            gabi::at<ArrowEmitter>(e)->mGlobalTranslation.x = tx;
            gabi::at<ArrowEmitter>(e)->mGlobalTranslation.y = ty;
            gabi::at<ArrowEmitter>(e)->mGlobalTranslation.z = tz;
            /* setGlobalScale(scaleVec) */
            e = mpSparkleEmitter;
            gabi::at<ArrowEmitter>(e)->mGlobalScale.x = scaleMag;
            gabi::at<ArrowEmitter>(e)->mGlobalScale.y = scaleMag;
            gabi::at<ArrowEmitter>(e)->mGlobalScale.z = scaleMag;
            gabi::at<ArrowEmitter>(e)->mGlobalParticleScale.x = scaleMag;
            gabi::at<ArrowEmitter>(e)->mGlobalParticleScale.y = scaleMag;
            gabi::at<ArrowEmitter>(e)->mGlobalParticleScale.z = scaleMag;
            return TRUE;
        }
        /* becomeInvalidEmitter() */
        u32 flags = gabi::at<ArrowEmitter>(e)->mFlags;
        gabi::at<ArrowEmitter>(e)->mMaxFrame = -1;
        gabi::at<ArrowEmitter>(e)->mFlags = flags | 1;
        mpSparkleEmitter = 0;
    }

    if ((u32)(s32)field_0x602 == (u32)(s32)l_count) {
        field_0x600 = true;
    }

    f32 frame = mBtkFrame;
    if (frame > 0.0f) { // ble: unordered takes the else branch
        frame = frame + 1.0f;
        mBtkFrame = frame;
        if (!(frame < (f32)gabi::at<ArrowAnimation>(mpBtk)->mFrameMax)) { /* mpBtk->getFrameMax() */
            mBtkFrame = 0.0f;
        }
    } else {
        if (cM_rnd() < 0.02f) { // bge: unordered skips
            mBtkFrame = mBtkFrame + 1.0f;
        }
    }

    setLightEffect();

    /* if (mCurrProcFunc) (this->*mCurrProcFunc)() */
    s16 vidx = mCurrProcFunc.i;
    if (vidx != 0) {
        u32 p = self(this) + mCurrProcFunc.d;
        if (vidx < 0) {
            gabi::call_ptr(mCurrProcFunc.f, gabi::at<void>(p));
        } else {
            u32 vt = *gabi::at<be<u32>>(p + gabi::at<ArrowPtmfView>(gabi::ea(&mCurrProcFunc))->mVtableOffset);
            gabi::call_ptr(gabi::at<ArrowDispatchVtable>(vt)->mEntries[(u32)vidx].mpFunction,
                           gabi::at<void>(p));
        }
    }

    u32 cx = gabi::at<ArrowWords>(self(&current.pos))->x, cy = gabi::at<ArrowWords>(self(&current.pos))->y,
        cz = gabi::at<ArrowWords>(self(&current.pos))->z;
    u32 ap = gabi::ea(&attention_info.position); /* attention_info.position */
    gabi::at<ArrowWords>(ap)->x = cx;
    gabi::at<ArrowWords>(ap)->y = cy;
    gabi::at<ArrowWords>(ap)->z = cz;
    gabi::at<ArrowWords>(self(&eyePos))->x = cx;
    gabi::at<ArrowWords>(self(&eyePos))->y = cy;
    gabi::at<ArrowWords>(self(&eyePos))->z = cz;
    setRoomInfo();

    return TRUE;
}

/* 02053188 */
BOOL daArrow_c::_draw() {
    if (!field_0x698) {
        return TRUE;
    }

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);

    if (mArrowType != TYPE_LIGHT) {
        shape_show(mpTipMat, 1);
        if (gabi::at<ArrowMaterial>(mpTipMat)->mpNext != 0) { /* getNext() */
            shape_show(gabi::at<ArrowMaterial>(mpTipMat)->mpNext, 1);
        }
    }

    gabi::at<ArrowAnimation>(mpBtk)->mFrame = mBtkFrame; /* mpBtk->setFrame(mBtkFrame) */
    /* dComIfGd_setListP1() */
    dComIfGd_setListP1();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();

    if (mArrowType != TYPE_LIGHT) {
        shape_show(mpTipMat, 0);
        if (gabi::at<ArrowMaterial>(mpTipMat)->mpNext != 0) {
            shape_show(gabi::at<ArrowMaterial>(mpTipMat)->mpNext, 0);
        }
    }

    return TRUE;
}

/* 0205295C */
cPhs_State daArrow_c::_create() {
    /* fopAcM_ct(this, daArrow_c): the HD constructor is inlined */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            fopAc_ac_c_ct(this);
            __vtbl = arrow_tu::actorVtable;
            /* dBgS_ArrowLinChk mLinChk */
            ArrowLineConstruct(&mLinChk); /* cBgS_LinChk::cBgS_LinChk */
            mLinChk.mbLinkThrough = 0;
            mLinChk.mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk *>(&mLinChk));
            mLinChk.mbBombThrough = 0;
            mLinChk.mbRopeThrough = 0;
            mLinChk.cBgS_PolyInfo::__vtbl = arrow_tu::linePolyInfoVtable;
            mLinChk.mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk *>(&mLinChk));
            mLinChk.mbCamThrough = 0;
            mLinChk.dBgS_PolyPassChk::__vtbl = arrow_tu::linePolyPassVtable;
            mLinChk.mbBoomerangThrough = 0;
            mLinChk.dBgS_GrpPassChk::__vtbl = arrow_tu::lineGroupVtable;
            mLinChk.mGrp = 5;
            mLinChk.mbArrowThrough = 1; /* dBgS_ArrowLinChk: the arrow pass flag */
            mLinChk.cBgS_Chk::__vtbl = arrow_tu::lineCheckVtable;
            mLinChk.mbObjThrough = 0;
            /* dBgS_ObjGndChk mGndChk */
            dBgS_GndChk_ct(&mGndChk,
                           dBgS_GndChk_vt{arrow_tu::groundCheckVtable, arrow_tu::groundPolyInfoVtable,
                                          arrow_tu::groundPolyPassVtable, arrow_tu::groundGroupVtable},
                           true);
            dCcD_Stts_ct(&mStts);
            /* dCcD_Cps mAtCps */
            ArrowCollisionConstruct(&mAtCps); /* dCcD_GObjInf::dCcD_GObjInf */
            gabi::at<ArrowShapeAttr>(gabi::ea(mAtCps.mShapeAttr))->mShapeVtable = arrow_tu::shapeVtable;
            gabi::at<ArrowShapeAttr>(gabi::ea(mAtCps.mShapeAttr))->mAabVtable = arrow_tu::aabVtable;
            ArrowCapsuleConstruct(mAtCps.mCps); /* cM3dGCps::cM3dGCps */
            mAtCps.__vtbl_hitinf = arrow_tu::capsuleHitInfoVtable;
            gabi::at<ArrowShapeAttr>(gabi::ea(mAtCps.mShapeAttr))->mShapeVtable = arrow_tu::capsuleAttrVtable;
            gabi::at<ArrowCapsuleGeometry>(gabi::ea(mAtCps.mCps))->__vtbl = arrow_tu::capsuleVtable;
            ArrowSphereConstruct(&mCoSph);              /* dCcD_Sph::dCcD_Sph */
            ArrowFollowConstruct(&mBlurFollowCb, 0, 0); /* dPa_followEcallBack(0, 0) */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    checkCreater();

    if (mbSetByZelda) {
        mArrowType = TYPE_LIGHT;
    } else {
        setTypeByPlayer();
    }

    if (dComIfGp_getMiniGameType() == 8) { /* dComIfGp_getMiniGameType() */
        mArrowType = TYPE_NORMAL;
    }

    if (!fopAcM_entrySolidHeap(this, A_createHeap_CB, l_heapSize[mArrowType])) {
        return cPhs_ERROR_e;
    }

    if (createInit()) {
        return cPhs_COMPLEATE_e;
    } else {
        return cPhs_ERROR_e;
    }
}

/* 02052C0C */
BOOL daArrow_c::_delete() {
    /* mBlurFollowCb.remove(): virtual (slot 0x44) */
    mBlurFollowCb.remove();
    return TRUE;
}

/* 02052C08 */
static cPhs_State daArrowCreate(void *i_this) {
    return static_cast<daArrow_c *>(i_this)->_create();
}

/* 02052C3C */
static BOOL daArrowDelete(void *i_this) {
    return static_cast<daArrow_c *>(i_this)->_delete();
}

/* 02053184 */
static BOOL daArrowExecute(void *i_this) {
    return static_cast<daArrow_c *>(i_this)->_execute();
}

/* 020532B4 */
static BOOL daArrowDraw(void *i_this) {
    return static_cast<daArrow_c *>(i_this)->_draw();
}

/* 02055A80 */
static BOOL daArrowIsDelete(void *) {
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.
/* 020559B8 */
static void sinit_d_a_arrow() {
    sinit_header_statics(arrow_tu::headerVectors, arrow_tu::headerVectorGuard);
    /* the radii of m_at_cps_src (5.0f) and m_co_sph_src (25.0f) */
    l_atCpsSrc.mRadius = 5.0f;
    l_coSphSrc.mRadius = 25.0f;
}

/* 02055A6C */
static void emptyClass_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 02055A88 */
static void daArrow_dt(daArrow_c *p, s32 flags) {
    if (p == nullptr)
        return;
    ArrowSphereDestroy(&p->mCoSph, 2);    /* dCcD_Sph::~dCcD_Sph */
    ArrowCollisionDestroy(&p->mAtCps, 2); /* dCcD_Cps (dCcD_GObjInf::~dCcD_GObjInf) */
    ArrowStatusDestroy(&p->mStts, 2);     /* dCcD_Stts::~dCcD_Stts */
    /* ~dBgS_ObjGndChk: this TU's vtables, then cBgS_Chk::~cBgS_Chk */
    p->mGndChk.cBgS_PolyInfo::__vtbl = arrow_tu::groundPolyInfoDestroyVtable;
    p->mGndChk.dBgS_GrpPassChk::__vtbl = arrow_tu::groupDestroyVtable;
    p->mGndChk.dBgS_PolyPassChk::__vtbl = arrow_tu::groundPolyPassDestroyVtable;
    ArrowGroundDestroy(&p->mGndChk, 0);
    /* ~dBgS_ArrowLinChk */
    p->mLinChk.dBgS_GrpPassChk::__vtbl = arrow_tu::groupDestroyVtable;
    p->mLinChk.dBgS_PolyPassChk::__vtbl = arrow_tu::linePolyPassDestroyVtable;
    p->mLinChk.cBgS_PolyInfo::__vtbl = arrow_tu::linePolyInfoDestroyVtable;
    ArrowLineDestroy(&p->mLinChk, 0); /* cBgS_LinChk::~cBgS_LinChk */
    ArrowActorDestroy(p, 0);          /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) {
        operator_delete(p);
    }
}

/* 02055B60 */
static void emptyFunc() {}

/* 02055B64 */
static s16 arrow_calcTimer(be<s16> *t) {
    s16 v = *t;
    if (v != 0) {
        v = v - 1;
        *t = v;
    }
    return v;
}

/* WWHD glue (tools/verify/autoglue.py): Arrow translation unit
str 1000749C "Link"
str 10007368 "d_a_arrow.cpp"
str 10007378 "modelData != (0)"
str 1000743C "GanonK"
data l_useMpRest 1000738C
data l_heapSize 100073DC
data l_shootSe 1000740C
data l_useMp 10007444
data l_arrowColor 10007477
data l_keepType 101D5F43
data l_count 1018FDE8
data l_arrowMat 100073AC
data l_atCpsSrc 1018FE5C
data l_coSphSrc 1018FE1C
data l_offsetGuard 10461494
data l_offsetArrowPos 10461488
data l_stayNo 1047E6C8
ext 028E9108 void PSMTXConcat(const Mtx34* a, const Mtx34* b, Mtx34* ab)
ext 025F19F8 void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z)
ext 025F232C void mDoMtx_MtxToRot(Mtx34* m, csXyz* rot)
ext 025D672C void fopAcM_SetMin(fopAc_ac_c* a, f32 x, f32 y, f32 z)
ext 025D673C void fopAcM_SetMax(fopAc_ac_c* a, f32 x, f32 y, f32 z)
ext 025D54C4 BOOL fopAcM_SearchByID_out(u32 id, be<u32>* out)
ext 025D5A20 u32 fopAcM_createChild(s16 name, u32 parent, u32 prm, cXyz* pos, s32 room, csXyz* angle, cXyz* scale, s8 subtype, u32 createFunc)
ext 025D5928 fopAc_ac_c* fopAcM_fastCreate(s16 name, u32 prm, cXyz* pos, s32 room, csXyz* angle, cXyz* scale, s8 subtype, u32 createFunc, u32 data)
ext 02008974 f32 cBgS_GroundCross(dBgS* bgs, void* chk)
ext 024EF130 s32 dBgS_GetRoomId(dBgS* bgs, void* poly)
ext 024EEEB8 s32 dBgS_GetPolyColor(dBgS* bgs, void* poly)
ext 025164C0 void dCcD_Cps_Set(void* cps, u32 src)
ext 0251677C void dCcD_Sph_Set(void* sph, u32 src)
ext 020181B0 void cM3dGCps_Set(void* cps, cXyz* start, cXyz* end, f32 r)
ext 028E8DAC void PSVECSubtract(const cXyz* a, const cXyz* b, cXyz* ab)
ext 028249B0 void JPASetRMtxTVecfromMtx(const Mtx34* m, void* r, void* t)
ext 02019788 f32 cM_rnd()
ext 02443208 u8 daPy_lk_c_setItemWaterEffect(fopAc_ac_c* a, s32 prev, s32 p)
ext 0255F3E8 void dKy_arrowcol_chg_on(cXyz* pos, s32 type)
ext 025D9F70 BOOL fopAcM_getWaterY(const cXyz* pos, be<f32>* y)
ext 025D5578 BOOL fopAcM_SearchByName(s16 name, be<u32>* out)
ext 024EDFCC BOOL dAttention_LockonTruth(void* att)
ext 024EC8D0 u32 dAttention_LockonTarget(void* att, s32 i)
ext 0200F974 s16 cLib_targetAngleX(cXyz* a, cXyz* b)
ext 025F1BF4 void mDoMtx_XrotM(Mtx34* m, s16 x)
ext 02008254 BOOL cBgS_ChkPolySafe(dBgS* bgs, void* poly)
ext 024EEABC BOOL dBgS_ChkMoveBG(dBgS* bgs, void* poly)
ext 024EFA38 void dBgS_MoveBgTransPos(dBgS* bgs, void* poly, bool b, cXyz* pos, csXyz* angle, csXyz* shape)
ext 024EEE30 BOOL dBgS_ChkGrpInf(dBgS* bgs, void* poly, u32 grp)
ext 024EF0F4 s32 dBgS_GetAttributeCode(dBgS* bgs, void* poly)
ext 028E9F74 void C_VECReflect(const cXyz* in, const void* normal, cXyz* out)
ext 02017300 void cM3d_CalcVecZAngle(const void* v, csXyz* out)
ext 025D8F90 s32 fopAcM_createItemForSimpleDemo(cXyz* pos, s32 item, s32 room, csXyz* angle, cXyz* scale, f32 sp, f32 spy)
ext 025E1988 void mDoAud_seStart_noPos(u32 id)
ext 02018D40 void cM3dGSph_SetC(void* sph, cXyz* c)
ext 02516C14 void ArrowMassSet(void*, void*, u8)
ext 02008FEC void ArrowLineConstruct(void*)
ext 02515FB8 void ArrowCollisionConstruct(void*)
ext 02018150 void ArrowCapsuleConstruct(void*)
ext 025166F0 void ArrowSphereConstruct(void*)
ext 025A5894 void ArrowFollowConstruct(void*, s32, s32)
ext 02515BBC fopAc_ac_c* ArrowAtHitActor(dCcD_GObjAt*)
ext 025538E4 s32 ArrowSearchJointHit(u32, cXyz*, csXyz*, cXyz*, csXyz*)
ext 02515AE8 void ArrowSphereDestroy(void*, s32)
ext 02515980 void ArrowCollisionDestroy(void*, s32)
ext 02515860 void ArrowStatusDestroy(void*, s32)
ext 02008DAC void ArrowGroundDestroy(void*, s32)
ext 02008B4C void ArrowLineDestroy(void*, s32)
ext 025D50BC void ArrowActorDestroy(daArrow_c*, s32)
*/
