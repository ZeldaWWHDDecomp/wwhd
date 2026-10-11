/**
 * d_a_tornado.cpp (WWHD)
 * Object - Ballad of Gales tornado
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tornado.cpp) to the WWHD layout and verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * HD: the joint callback writes the joint matrices to an actor-owned block (mJointMtx) and copies
 * all eleven into the model's matrix block when the last joint is reached.
 */
#include "d/actor/d_a_tornado.h"

const gc_tu_data gc_tu = {
    .safeString = 0x10040924,
    .headerStatics = {0x1046E9D0, 0x101D2A30, 0x1046E9DC},
};

static const char* l_arcName = "Trnd";
static const f32 l_joint_scale[11] = {0.1f, 0.4f, 0.7f, 1.0f, 1.0f, 1.0f,
                                      1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static const f32 joint_offset[11] = {100.0f, 200.0f, 300.0f, 400.0f, 300.0f, 300.0f,
                                     400.0f, 500.0f, 400.0f, 500.0f, 600.0f};

/* 024C8640 */
BOOL daTornado_c::jointCallBack(int jntNo) {
    int jntIdx = jntNo - 1;
    if ((u32)jntIdx >= 11) /* (jntIdx < ROOT) || (jntIdx >= JOINT11) */
        return TRUE;

    PSMTXTrans(mDoMtx_stack_c::get(), mJointX[jntIdx], 0.0f, mJointZ[jntIdx]);
    if (jntIdx != 10 && jntIdx != 0) {
        s16 rx = (s16)gabi::ftoi(-(3572.0f * mJointZ[jntIdx] * 0.001f));
        s16 rz = (s16)gabi::ftoi(3572.0f * mJointX[jntIdx] * 0.001f);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), rx, 0, rz);
    }
    if (fopAcM_GetParam(this) != 0) {
        f32 s = mJointScale[jntIdx];
        mDoMtx_stack_c::scaleM(s, s, s);
    }
    /* mDoMtx_stack_c::revConcat(mpModel->getAnmMtx(jntNo)) (HD: getAnmMtx marks the block dirty) */
    J3DMtxBuffer_hd* blk = mpModel->mpMtxBuffer;
    u32 anm = jntNo * 0x30 + blk->mpAnmMtx;
    blk->mFlags = blk->mFlags | 0x10;
    PSMTXConcat(gabi::at<Mtx34>(anm), mDoMtx_stack_c::get(), mDoMtx_stack_c::get());
    // HD: keep the matrices in the actor until the last joint is reached.
    PSMTXCopy(mDoMtx_stack_c::get(), &mJointMtx[jntIdx]);
    PSMTXTrans(mDoMtx_stack_c::get(), mJointX[jntIdx], 0.0f, mJointZ[jntIdx]);
    PSMTXConcat(J3DSys::mCurrentMtx, mDoMtx_stack_c::get(), mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), J3DSys::mCurrentMtx);

    if (jntNo == 11) {
        /* HD: copy the eleven joint matrices into the model (joints 1..11) */
        for (int i = 0; i < 11; i++) {
            mtx_copy(mpModel->getAnmMtx(i + 1), &mJointMtx[i]);
        }
    }
    return TRUE;
}

/* 024C8854 */
static BOOL daTornado_jointCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel* model = j3dSys.getModel();
        s32 jntNo = reinterpret_cast<TornadoJointView*>(joint)->mJntNo;
        daTornado_c* i_this = static_cast<daTornado_c*>(model->getUserArea());
        i_this->jointCallBack(jntNo);
    }
    return TRUE;
}

/* 024C8894 */
BOOL daTornado_c::draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    mBck.entry(mpModel->getModelData(), mBck.getFrame());
    mBtk.entry(mpModel->getModelData(), mBtkFrame);
    mBrk.entry(mpModel->getModelData(), mBrkFrame);
    if (dComIfGs_isTmpBit(0x404)) {
        dComIfGd_setListMaskOff();
        mDoExt_modelUpdateDL(mpModel);
        dComIfGd_setList();
    } else {
        mDoExt_modelUpdateDL(mpModel);
    }
    if (fopAcM_GetParam(this) == 0) {
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG1, &current.pos, &tevStr);
        setLightTevColorType(dKy_getEnvlight(), mpModelUnder, &tevStr);
        mBckUnder.entry(mpModelUnder->getModelData(), mBck.getFrame());
        mBtkUnder.entry(mpModelUnder->getModelData(), mBtkUnderFrame);
        mBrkUnder.entry(mpModelUnder->getModelData(), mBrkFrame);
        dComIfGd_setListMaskOff();
        mDoExt_modelUpdateDL(mpModelUnder);
        dComIfGd_setList();
    }
    return TRUE;
}

/* 024C8A54 */
static BOOL daTornado_Draw(daTornado_c* i_this) { return i_this->draw(); }

/* 024C8A58 */
BOOL daTornado_c::execute() {
    // HD: the guard is stored before the final component.
    auto& scales = tornadoScales();
    if (scales.windGuard == 0) {
        scales.wind.y = 20.0f;
        scales.wind.z = 20.0f;
        scales.windGuard = 1;
        scales.wind.x = 20.0f;
    }

    mBck.play();
    f32 f = mBtkFrame + 1.0f;
    mBtkFrame = f;
    if (!(f < (f32)btkAnm()->getFrameMax())) {
        f32 max = (f32)btkAnm()->getFrameMax();
        mBtkFrame = mBtkFrame - max;
    }
    f = mBtkUnderFrame + 1.0f;
    mBtkUnderFrame = f;
    if (!(f < (f32)btkUnderAnm()->getFrameMax())) {
        f32 max = (f32)btkUnderAnm()->getFrameMax();
        mBtkUnderFrame = mBtkUnderFrame - max;
    }

    // HD: update the movement radius between the two angle stores.
    mAngle2 = (s16)(mAngle2 + 250);
    m32c = 10000.0f;
    mAngle1 = (s16)(mAngle1 + 500);
    u32 param = fopAcM_GetParam(this);
    f32 fVar8;
    if (param == 1) {
        if (dComIfGp_getShipActor() != 0) {
            TornadoShipView* ship = tornadoShip();
            fVar8 = (ship->current.pos.y - home.pos.y) / 500.0f;
            if (fVar8 < 0.0f) {
                fVar8 = 0.0f;
            } else if (fVar8 > 1.0f) {
                fVar8 = 1.0f;
            }
        } else {
            fVar8 = 1.0f;
        }
    } else if (param == 0 && dComIfGp_getShipActor() != 0 && tornadoShip()->mTornadoActor != 0) {
        fVar8 = 0.25f;
    } else {
        fVar8 = 1.0f;
    }

    for (int i = 0; i < 11; i++) {
        f32 fVar1 = (joint_offset[i] * fVar8) * (cM_ssin(mAngle1 - 0x1000 * i) + 1.0f);
        s32 angle2 = mAngle2 - 0x1800 * i;
        mJointX[i] = fVar1 * cM_ssin(angle2) * scale.x;
        mJointZ[i] = fVar1 * cM_scos(angle2) * scale.x;
    }

    param = fopAcM_GetParam(this);
    if (param == 0) {
        if (!dComIfGp_event_runCheck()) {
            s16 target = fopAcM_searchActorAngleY(this, dComIfGp_getLinkPlayer());
            cLib_addCalcAngleS(&current.angle.y, target, 10, 0x1000, 0x100);
            cLib_chaseF(&speedF, 20.0f, 0.2f);
        }
        if (dComIfGp_checkPlayerStatus0(0, daPyStts0_SHIP_RIDE_e) && dComIfGp_getShipActor() != 0) {
            TornadoShipView* ship = tornadoShip();
            gabi::Local<cXyz> diff;
            cXyz_mi(&ship->current.pos, diff, &current.pos);
            gabi::Local<cXyz> xz;
            xz->x = diff->x;
            xz->y = 0.0f;
            xz->z = diff->z;
            if (PSVECSquareMag(xz) < 10000.0f * 10000.0f) {
                ship->mTornadoID = fopAcM_GetID(this);
                speedF = 0.0f;
            }
        }
        if (!dComIfGp_event_runCheck()) {
            fopAcM_posMoveF(this, nullptr);
            gabi::Local<cXyz> diff;
            cXyz_mi(&current.pos, diff, &home.pos);
            f32 dx = diff->x;
            diff->y = 0.0f;
            gabi::Local<cXyz> xz;
            xz->x = dx;
            xz->y = 0.0f;
            xz->z = diff->z;
            if (PSVECSquareMag(xz) > 7500.0f * 7500.0f) {
                gabi::Local<cXyz> tmp;
                cXyz_normalize(diff, tmp);
                gabi::Local<cXyz> ofs;
                cXyz_ml(diff, ofs, 7500.0f);
                cXyz_pl(&home.pos, tmp, ofs);
                current.pos = *tmp;
            }
        }
        if (mPtclTimer != 0) {
            mPtclTimer = (s16)(mPtclTimer - 1);
        } else {
            mPtclTimer = 10;
            dComIfGp_particle_set(dPa_name_ID_AK_SN_TORNADOWIND00, &current.pos, nullptr,
                                  &scales.wind);
        }
        /* fopAcM_seStartCurrent (HD: no NULL checks here) */
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        mDoAud_seStart(JA_SE_OBJ_TORNADE_SUS, &current.pos, 100, reverb);
        if (dComIfGs_isEventBit(0x2710)) {
            mPtclCb.end();
            TornadoShipView* ship = tornadoShip();
            if (ship != 0)
                ship->offTornadoFlg();
            f32 b = mBrkFrame + 0.3f;
            mBrkFrame = b;
            if (!(b < (f32)brkAnm()->getFrameMax()))
                fopAcM_delete(this);
        } else {
            dKyw_tornado_Notice(&current.pos);
        }
    } else if (m31c != 0) {
        if (param == 1) {
            for (int i = 0; i < 11; i++) {
                cLib_chaseF(&mJointScale[i], l_joint_scale[i], 0.007f * (f32)(11 - i));
                mJointX[i] = mJointX[i] * mJointScale[i];
                mJointZ[i] = mJointZ[i] * mJointScale[i];
            }
        } else {
            for (int i = 0; i < 11; i++) {
                cLib_chaseF(&mJointScale[i], 0.0f, 0.001f * (f32)(i + 1));
                mJointX[i] = mJointX[i] * mJointScale[i];
                mJointZ[i] = mJointZ[i] * mJointScale[i];
            }
            if (mJointScale[10] < 1e-06f) {
                fopAcM_delete(this);
                return TRUE;
            }
        }
    }

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    tornadoSetAttention(this);
    mCenter.x = current.pos.x + mJointX[0];
    mCenter.y = current.pos.y;
    mCenter.z = current.pos.z + mJointZ[0];
    if (mPtclCb.getEmitter() != nullptr) {
        gabi::Local<GXColor> colorAmb;
        gabi::Local<GXColor> colorDif;
        dKy_get_seacolor(colorAmb, colorDif);
        auto* emtr = reinterpret_cast<TornadoEmitterView*>(mPtclCb.getEmitter());
        auto* color = reinterpret_cast<TornadoColorView*>(colorAmb.get());
        emtr->mGlobalPrmColor.r = color->r;
        emtr->mGlobalPrmColor.g = color->g;
        emtr->mGlobalPrmColor.b = color->b;
    }
    mDoMtx_stack_c::transS(mCenter.x, mCenter.y, mCenter.z);
    mpModelUnder->setBaseTRMtx(mDoMtx_stack_c::get());
    return TRUE;
}

/* 024C9368 */
static BOOL daTornado_Execute(daTornado_c* i_this) { return i_this->execute(); }

/* 024C936C */
static BOOL daTornado_IsDelete(daTornado_c* i_this) { return TRUE; }

/* 024C9374 */
BOOL daTornado_c::tornado_delete() {
    /* mPtclCb.remove(): virtual (slot 0x44) */
    // HD: deletion dispatches remove virtually; execute calls end directly.
    mPtclCb.remove();
    if (fopAcM_GetParam(this) == 0) {
        TornadoShipView* ship = tornadoShip();
        if (ship != 0)
            ship->offTornadoFlg();
    }
    dComIfG_resDelete(&mPhs, l_arcName);
    return TRUE;
}

/* 024C93F0 */
static BOOL daTornado_Delete(daTornado_c* i_this) {
    i_this->tornado_delete();
    return TRUE;
}

/* 024C9414 */
BOOL daTornado_c::createHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(
        l_arcName, dRes_INDEX_TRND_BDL_YTRND00_e, gc_tu.safeString);
    if (modelData == nullptr) /* JUT_ASSERT(0x20b, modelData != NULL) */
        JUT_ASSERT_fail("d_a_tornado.cpp", 0x20b, "modelData != (0)");
    J3DModel* m = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000202);
    mpModel = m;
    if (m == nullptr)
        return FALSE;
    // HD: enable each model entry after creating the main model.
    tornadoSetModelEntryFlags(m, 1);
    if (!mBck.init(modelData,
                   (J3DAnmTransform*)dComIfG_getObjectRes(l_arcName, dRes_INDEX_TRND_BCK_YTRND00_e,
                                                          gc_tu.safeString),
                   true, 2 /* EMode_LOOP */, 1.0f, 0, -1, false))
        return FALSE;
    if (!mBtk.init(modelData,
                   (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(
                       l_arcName, dRes_INDEX_TRND_BTK_YTRND00_e, gc_tu.safeString),
                   false, 2, 1.0f, 0, -1, false, 0))
        return FALSE;
    if (!mBrk.init(modelData,
                   (J3DAnmTevRegKey*)dComIfG_getObjectRes(l_arcName, dRes_INDEX_TRND_BRK_YTRND00_e,
                                                          gc_tu.safeString),
                   false, 2, 1.0f, 0, -1, false, 0))
        return FALSE;
    modelData = (J3DModelData*)dComIfG_getObjectRes(l_arcName, dRes_INDEX_TRND_BDL_YWUWT00_e,
                                                    gc_tu.safeString);
    if (modelData == nullptr) /* JUT_ASSERT(0x236, modelData != NULL) */
        JUT_ASSERT_fail("d_a_tornado.cpp", 0x236, "modelData != (0)");
    m = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000202);
    mpModelUnder = m;
    if (m == nullptr)
        return FALSE;
    if (!mBckUnder.init(modelData,
                        (J3DAnmTransform*)dComIfG_getObjectRes(
                            l_arcName, dRes_INDEX_TRND_BCK_YWUWT00_e, gc_tu.safeString),
                        false, 2, 1.0f, 0, -1, false))
        return FALSE;
    if (!mBtkUnder.init(modelData,
                        (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(
                            l_arcName, dRes_INDEX_TRND_BTK_YWUWT00_e, gc_tu.safeString),
                        false, 2, 1.0f, 0, -1, false, 0))
        return FALSE;
    if (!mBrkUnder.init(modelData,
                        (J3DAnmTevRegKey*)dComIfG_getObjectRes(
                            l_arcName, dRes_INDEX_TRND_BRK_YWUWT00_e, gc_tu.safeString),
                        false, 2, 1.0f, 0, -1, false, 0))
        return FALSE;
    return TRUE;
}

/* 024C9708 */
static BOOL daTornado_createHeap(fopAc_ac_c* i_this) {
    return ((daTornado_c*)i_this)->createHeap();
}

/* 024C970C */
cPhs_State daTornado_c::create() {
    auto& scales = tornadoScales();
    tornadoInitScale(scales.smallGuard, scales.small, 0.25f, 0.175f, 0.25f);
    tornadoInitScale(scales.underSmallGuard, scales.underSmall, 0.251f, 0.25f, 0.251f);
    tornadoInitScale(scales.underGuard, scales.under, 1.01f, 1.0f, 1.01f);

    cPhs_State rt = dComIfG_resLoad(&mPhs, l_arcName);
    /* fopAcM_ct(this, daTornado_c): HD vtable, inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = TORNADO_VTBL;
            tornadoConstructBck(&mBck);
            mDoExt_btkAnm::ct(&mBtk);
            mDoExt_brkAnm::ct(&mBrk);
            tornadoConstructBck(&mBckUnder);
            mDoExt_btkAnm::ct(&mBtkUnder);
            mDoExt_brkAnm::ct(&mBrkUnder);
            dPa_followEcallBack_ct(&mPtclCb, 0, 0);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    if (rt == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, daTornado_createHeap, 0x8000)) {
            return cPhs_ERROR_e;
        }
        u32 param = fopAcM_GetParam(this);
        if (param == 1) {
            current.pos.y = current.pos.y - 2000.0f;
            tornadoSetScale(mpModel, scales.small);
            tornadoSetScale(mpModelUnder, scales.underSmall);
        } else if (param == 2) {
            tornadoSetScale(mpModel, scales.small);
            tornadoSetScale(mpModelUnder, scales.underSmall);
            for (int i = 0; i < 11; i++) {
                mJointScale[i] = l_joint_scale[i];
            }
        } else {
            mParameters = 0; /* fopAcM_SetParam(this, 0) */
            if (dComIfGs_isEventBit(0x2710)) {
                return cPhs_ERROR_e;
            }
            dKyw_tornado_Notice(&current.pos);
            tornadoSetScale(mpModelUnder, scales.under);
            mCenter = current.pos;
            dComIfGp_particle_set(dPa_name_ID_AK_SN_WINDUPWATER00, &mCenter, nullptr, nullptr, 0xFF,
                                  (dPa_levelEcallBack*)&mPtclCb);
            actor_status = actor_status | fopAcStts_SHOWMAP_e;
        }
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
        mpModelUnder->setBaseTRMtx(mDoMtx_stack_c::get());
        J3DModelData* modelData = mpModel->getModelData();
        for (u16 i = 1; i < modelData->getJointNum(); i++) {
            /* modelData->getJointNodePointer(i)->setCallBack(daTornado_jointCallBack) */
            u32 n = modelData->mJointNodes;
            if (i < modelData->mJointNum)
                n += i * 0x1C;
            gabi::at<J3DJoint>(n)->setCallBack(daTornado_jointCallBack);
        }
        mpModel->setUserArea(reinterpret_cast<uintptr_t>(this));
        /* HD: the joint matrix block starts as zero matrices */
        PSMTXScale(mDoMtx_stack_c::get(), 0.0f, 0.0f, 0.0f);
        for (int i = 0; i < 11; i++)
            PSMTXCopy(mDoMtx_stack_c::get(), &mJointMtx[i]);
    }
    return rt;
}

/* 024C9C28 */
static cPhs_State daTornado_Create(fopAc_ac_c* i_this) { return ((daTornado_c*)i_this)->create(); }

// HD: compiler-generated and template code of this translation unit.

/* 024C9C2C */
static void __sinit_d_a_tornado_cpp() { __sinit_header_statics(); }

/* 024C9CC0 */
// HD: deleting destructor of a class with a trivial destructor (sead::SafeString, per TU)
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 024C9CD4 */
// HD: daTornado_c deleting destructor (the two bck sub-objects' destructors)
static void daTornado_c_dt(daTornado_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        tornadoDestroyBck(&i_this->mBckUnder);
        tornadoDestroyBck(&i_this->mBck);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 024C9D40 */
// HD: an empty virtual (per-TU copy)
static void empty_virtual(void* p) {}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
 str 101D2A08 "Trnd"
 data l_joint_scale 10040974
 data joint_offset 100409A0
 str 10040A10 "d_a_tornado.cpp" in 024C9414
 str 10040A20 "modelData != (0)" in 024C9414
*/
// clang-format on
