/**
 * d_a_ship.cpp (WWHD)
 * King of Red Lions
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ship.cpp) to the WWHD layout and verified against cking.rpx.
 */
#include "d/actor/d_a_ship.h"

const gc_tu_data gc_tu = {.safeString = 0x1003A1EC};
static u32 ship_drawBuffer;
static Mtx34 ship_viewMtx;
static Mtx34 ship_sightMtx;
static u32 ship_counter;
static f32 ship_ropeGravity;
static s8 ship_stayNo;
static GXColor ship_ropeColor = {200, 150, 50, 255};
static cXyz ship_ropeBase;
static cXyz ship_zeroPoint = {0.0f, 0.0f, 0.0f};
static u32 shadowPosGuard;
static ShipShadowPositions shadowPositions;
static cXyz ship_waterDropScale = {1.5f, 1.0f, 1.5f};
static cXyz ship_ropeOffset = {160.0f, 0.0f, 0.0f};
static u32 waveRightDirectionGuard;
static cXyz waveRightDirection;
static u32 waveLeftDirectionGuard;
static cXyz waveLeftDirection;
static u32 rippleScaleGuard;
static cXyz rippleScale;
static u32 leftGuard;
static cXyz left;
static u32 rightGuard;
static cXyz right;
static u32 backGuard;
static cXyz back;
static u32 frontGuard;
static cXyz front;
static u32 ship_obstacleGuard;
static cXyz ship_obstacleOffset;



static void daShip_sightPacket_draw(u8* pkt);
/* 02471298 */
BOOL daShip_c::bodyJointCallBack(int jno) {
    J3DModel* pModel = mpBodyAnm->getModel();

    if (jno == FN_BODY_JNT_J_FN_STEER1_e || jno == FN_BODY_JNT_J_FN_KAJI_e) {
        mDoMtx_ZrotS(mDoMtx_now(), m0366);
        // HD: the tiller is shrunk away while Link looks down from the boat (GameCube: draw()
// skipped the whole body model)
        fopAc_ac_c* link = dComIfGp_getLinkPlayer();
        if (link != nullptr && (ship_playerStatus0() & daPyStts0_SHIP_RIDE_e) &&
            reinterpret_cast<ShipPlayer*>(link)->mLookDownAngle <= -0x1000) {
            mDoMtx_stack_c::scaleM(0.01f, 0.01f, 0.01f);
        }
    } else if (jno == FN_BODY_JNT_J_FN_SAIL1_e) {
        mDoMtx_ZrotS(mDoMtx_now(), -mSailAngle);
    } else if (jno == FN_BODY_JNT_J_FN_MAST_e) {
        mDoMtx_ZrotS(mDoMtx_now(), -0x4000);
        stack_revConcat(model_getAnmMtx(pModel, jno));
        J3DModel_setBaseTRMtx(mpSalvageArmModel, mDoMtx_now());
        mDoMtx_YrotM(mDoMtx_now(), -0x8000);
        J3DModel_setBaseTRMtx(mpCannonModel, mDoMtx_now());
        // HD: the cannon and crane models follow the mast animation's scale
        f32 scale = 1.0f;
        if (m0392 == dRes_INDEX_SHIP_BCK_FN_MAST_ON2_e) {
            scale = (mpBodyAnm->getFrame() / mpBodyAnm->getEndFrame()) * 2.5f;
            if (scale > 1.0f)
                scale = 1.0f;
            else
                scale = scale >= 0.0f ? scale : 0.0f;
        } else if (m0392 == dRes_INDEX_SHIP_BCK_FN_MAST_OFF2_e) {
            scale = gabi::fmsubs(1.0f - mpBodyAnm->getFrame() / mpBodyAnm->getEndFrame(), 3.0f, 1.0f);
            if (scale > 1.0f)
                scale = 1.0f;
            else
                scale = scale >= 0.0f ? scale : 0.0f;
        }
        model_setBaseScale1(mpSalvageArmModel, scale);
        model_setBaseScale1(mpCannonModel, scale);
        PSMTXScale(mDoMtx_now(), m03E8, m03E8, m03E8);
    }
    stack_revConcat(model_getAnmMtx(pModel, jno));
    model_setAnmMtx(pModel, jno, mDoMtx_now());
    stack_toCurrentMtx();
    return TRUE;
}



/* 0247187C */
static BOOL daShip_bodyJointCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 model = j3dSys_getModel();
        daShip_c* i_this = gabi::at<daShip_c>(model_getUserArea(model));
        s32 jntNo = reinterpret_cast<ShipJoint*>(joint)->mJntNo;
        if (i_this != nullptr) {
            i_this->bodyJointCallBack(jntNo);
        }
    }
    return TRUE;
}



/* 024718C8 */
BOOL daShip_c::cannonJointCallBack(int jno) {
    if (jno == VFNCN_JNT_CANON1_e) {
        mDoMtx_XrotS_l(mDoMtx_now(), m0394);
    } else {
        mDoMtx_YrotS(mDoMtx_now(), -m0396);
    }
    stack_revConcat(model_getAnmMtx(mpCannonModel, jno));
    model_setAnmMtx(mpCannonModel, jno, mDoMtx_now());
    stack_toCurrentMtx();
    return TRUE;
}



/* 02471AAC */
static BOOL daShip_cannonJointCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 model = j3dSys_getModel();
        s32 jntNo = reinterpret_cast<ShipJoint*>(joint)->mJntNo;
        daShip_c* i_this = gabi::at<daShip_c>(model_getUserArea(model));
        i_this->cannonJointCallBack(jntNo);
    }
    return TRUE;
}



/* 02471AEC */
BOOL daShip_c::craneJointCallBack() {
    mDoMtx_ZrotS(mDoMtx_now(), -(m0398 + m039C));
    stack_revConcat(model_getAnmMtx(mpSalvageArmModel, VFNCR_JNT_V_CRANE_ROTATION_e));
    model_setAnmMtx(mpSalvageArmModel, VFNCR_JNT_V_CRANE_ROTATION_e, mDoMtx_now());
    stack_toCurrentMtx();
    return TRUE;
}



/* 02471BF4 */
static BOOL daShip_craneJointCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        daShip_c* i_this = gabi::at<daShip_c>(model_getUserArea(j3dSys_getModel()));
        i_this->craneJointCallBack();
    }
    return TRUE;
}



/* 02471C2C */
BOOL daShip_c::headJointCallBack0() {
    if (mAnmTransform) {
        /* mpHeadAnm->changeAnm(mAnmTransform) */
        mDoExt_McaMorf* morf = mpHeadAnm;
        J3DAnmTransform* tempAnmTransform = morf->mpAnm;
        morf->mpAnm = mAnmTransform.get();
        mAnmTransform = tempAnmTransform;
    }
    return TRUE;
}



/* 02471C50 */
static BOOL daShip_headJointCallBack0(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        daShip_c* i_this = gabi::at<daShip_c>(model_getUserArea(j3dSys_getModel()));
        i_this->headJointCallBack0();
    }
    return TRUE;
}



/* 02471C88 */
BOOL daShip_c::headJointCallBack1(int jno) {
    Mtx34* jointAnmMtx = model_getAnmMtx(mpHeadAnm->getModel(), jno);
    f32 fVar1 = jointAnmMtx->m[0][3];
    f32 fVar2 = jointAnmMtx->m[1][3];
    f32 fVar3 = jointAnmMtx->m[2][3];

    s16 sVar5 = shape_angle.y + m03A2 * (jno + -2);
    mDoMtx_YrotS(mDoMtx_now(), sVar5);
    mDoMtx_ZXYrotM(mDoMtx_now(), m03A0, m03A2, 0);
    mDoMtx_YrotM(mDoMtx_now(), -sVar5);
    stack_concat(model_getAnmMtx(mpHeadAnm->getModel(), jno));

    mDoMtx_now()->m[0][3] = fVar1;
    mDoMtx_now()->m[1][3] = fVar2;
    mDoMtx_now()->m[2][3] = fVar3;

    model_setAnmMtx(mpHeadAnm->getModel(), jno, mDoMtx_now());
    stack_toCurrentMtx();
    return TRUE;
}



/* 02471E54 */
static BOOL daShip_headJointCallBack1(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 model = j3dSys_getModel();
        s32 jntNo = reinterpret_cast<ShipJoint*>(joint)->mJntNo;
        daShip_c* i_this = gabi::at<daShip_c>(model_getUserArea(model));
        i_this->headJointCallBack1(jntNo);
    }
    return TRUE;
}



/* 0247255C */
BOOL daShip_c::draw() {
    // HD: not drawn inside the Tower of the Gods
    if (daShip_checkSirenInside()) {
        return TRUE;
    }

    J3DModel* bodyModel = mpBodyAnm->getModel();
    J3DModel* headModel = mpHeadAnm->getModel();

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), bodyModel, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), headModel, &tevStr);

    dComIfGd_setListP1();

    if (m02A0 != 0 && m02A4 != 0) {
        // HD: no j3dSys skin CPU flags
        gabi::Local<cXyz> local_5c;
        f32 x = current.pos.x;
        f32 y = current.pos.y + m03D8 + 5.0f;
        f32 z = current.pos.z;
        local_5c->x = x;
        local_5c->y = y;
        local_5c->z = z;
        f32 k = 10000.0f * cM_scos(m037C);
        gabi::Local<cXyz> local_68;
        local_68->x = gabi::fmadds(k, cM_ssin(shape_angle.y), x);
        local_68->y = gabi::fnmsubs(10000.0f, cM_ssin(m037C), y);
        local_68->z = gabi::fmadds(k, cM_scos(shape_angle.y), z);
        gabi::Local<Mtx34> MStack_50;
        mDoMtx_lookAt(MStack_50.get(), local_5c.get(), local_68.get(), m037E);
        mDoMtx_YrotS(mDoMtx_now(), -0x4000);
        mDoMtx_XrotM(mDoMtx_now(), -0x8000);
        mDoMtx_stack_c::scaleM(1.0f, 2.0f, 0.25f);
        stack_concat(MStack_50.get());
        stack_revConcat(&m02A8);
        // HD: relative to the view matrix (j3dSys.mViewMtx 0x104B45F8)
        gabi::Local<Mtx34> invView;
        PSMTXInverse(&ship_viewMtx, invView.get());
        stack_concat(invView.get());
        /* m02A0->setEffectMtx(mDoMtx_stack_c::get()) (HD inline: a 4x4 effect matrix at +0x24, then
         * the 3x4 matrix at +0x64) */
        u32 tex = m02A0;
        for (int i = 0; i < 12; i++)
            gabi::at<ShipTexMtx>(tex)->mEffectMtx[i] = mDoMtx_now()->m[i / 4][i % 4];
        gabi::at<ShipTexMtx>(tex)->mEffectMtx[15] = 1.0f;
        gabi::at<ShipTexMtx>(tex)->mEffectMtx[12] = 0.0f;
        gabi::at<ShipTexMtx>(tex)->mEffectMtx[14] = 0.0f;
        gabi::at<ShipTexMtx>(tex)->mEffectMtx[13] = 0.0f;
        gabi::Local<Mtx34> tmp;
        for (int i = 0; i < 12; i++)
            tmp->m[i / 4][i % 4] = mDoMtx_now()->m[i / 4][i % 4];
        PSMTXCopy_s(tmp.get(), &gabi::at<ShipTexMtx>(tex)->mMtx);
        /* m02A4->mSRT.mTranslationX = m03D4 */
        gabi::at<ShipTexMtx>(m02A4)->mTranslationX = m03D4;
        // HD: the same scroll for the material "m_fn_main_hashi"
        u32 tabHdr = reinterpret_cast<ShipModelDraw*>(bodyModel)->mpResHeader;
        s32 off = gabi::at<ShipResourceHeader>(tabHdr)->mMaterialNameOffset;
        u32 tab = 0;
        if (off != 0)
            tab = tabHdr + 0x18 + off;
        s32 idx = JUTNameTab_getIndex_s(tab, gabi::guest_addr("m_fn_main_hashi"));
        u32 mat = 0;
        if (idx >= 0)
            mat = reinterpret_cast<ShipModelDraw*>(bodyModel)->mMaterials + idx * 0x3C;
        u32 srt = hd_matGetTexSrt(mat, 1);
        if (srt == 0) /* JUT_ASSERT(0x4AE, srt != 0) */
            JUT_ASSERT_fail("d_a_ship.cpp", 0x4AE, "srt != (0)");
        gabi::at<ShipTextureSRT>(srt)->mTranslationX = m03D4;
        u32 slot = hd_matGetTexMtxSlot(mat, 1);
        if (slot != 0)
            *gabi::at<be<u32>>(slot) = gabi::ea(&gabi::at<ShipTexMtx>(m02A0)->mMtx);
    }

    // HD: the body is always drawn (the tiller is hidden in bodyJointCallBack)
    mDoExt_modelEntryDL(bodyModel);

    if (!(ship_playerStatus0() & daPyStts0_SHIP_RIDE_e) ||
        !ship_isLinkPlayer0() ||
        !(ship_play()->mCameraAttentionStatus & 0x22) /* camera attention status: SUBJECT | 0x20 */) {
        offStateFlg(daSFLG_HEAD_NO_DRAW_e);
        mDoExt_modelEntryDL(headModel);
    } else {
        onStateFlg(daSFLG_HEAD_NO_DRAW_e);
    }

    if (mPart == PART_CANNON_e) {
        setLightTevColorType(dKy_getEnvlight(), mpCannonModel, &tevStr);
        mDoExt_modelEntryDL(mpCannonModel);
        // HD: the cannon sight, while aiming outside events
        if (ship_play()->mEventRun == 0 &&
            ship_isLinkPlayer0() &&
            mProc.i == -1 && mProc.d == 0 &&
            mProc.f == ShipProcedure::procCannon) {
            u32 saved = ship_drawBuffer;
            ship_drawBuffer = ship_play()->mSightDrawBuffer;
            mpFallPointAnm->updateDL();
            daShip_sightPacket_draw(mHDPacket);
            ship_drawBuffer = saved;
        }
    } else if (mPart == PART_CRANE_e) {
        setLightTevColorType(dKy_getEnvlight(), mpSalvageArmModel, &tevStr);
        mDoExt_modelEntryDL(mpSalvageArmModel);
        if (mRopeCnt >= 2) {
            mDoExt_3DlineMat1_update(&mRopeLine, mRopeCnt, 5.0f, &ship_ropeColor, 0, &tevStr);
            dComIfGd_set3DlineMat(&mRopeLine);
        }
        setLightTevColorType(dKy_getEnvlight(), mpLinkModel, &tevStr);
        mDoExt_modelUpdateDL(mpLinkModel);
    }

    dComIfGd_setList();

    // HD: replaces the GameCube real shadow (mShadowId)
    drawShadow();
    return TRUE;
}



/* 02472AAC */
static BOOL daShip_Draw(daShip_c* i_this) {
    return i_this->draw();
}



/* 024731BC */
BOOL daShip_c::checkForceMessage() {
    if (dComIfGs_isGetItem(1, 0) && !dComIfGs_isEventBit(ShipEvent::UNK_0908)) {
        mNextMessageNo = 0x5E0;
    } else if (dComIfGs_isSymbol(dSymbol_DIN_e) && !dComIfGs_isEventBit(ShipEvent::UNK_0A80)) {
        mNextMessageNo = 0x5EC;
    } else if (dComIfGs_isSymbol(dSymbol_FARORE_e) && !dComIfGs_isEventBit(ShipEvent::UNK_0A08)) {
        mNextMessageNo = 0x5F6;
    } else if (dComIfGs_isEventBit(ShipEvent::ENDLESS_NIGHT) && !dComIfGs_isEventBit(ShipEvent::UNK_0A01)) {
        mNextMessageNo = 0x607;
    } else if (dComIfGs_checkGetItem(dItemNo_BOMB_BAG_e) && !dComIfGs_isEventBit(ShipEvent::UNK_1F02)) {
        mNextMessageNo = 0x624;
    } else if (dComIfGs_isSymbol(dSymbol_NAYRU_e) && !dComIfGs_isEventBit(ShipEvent::UNK_2F20)) {
        mNextMessageNo = 0xD5A;
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_2D10) && !ship_checkMasterSwordEquip()) {
        mNextMessageNo = 0xD65;
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_3E01) && !dComIfGs_isEventBit(ShipEvent::UNK_3F80)) {
        mNextMessageNo = 0x1688;
    } else if (dComIfGs_isEventBit(ShipEvent::ZELDA_AWAKENED) && !dComIfGs_isEventBit(ShipEvent::UNK_3201)) {
        mNextMessageNo = 0x1645;
    } else {
        return FALSE;
    }
    return TRUE;
}



/* 02473454 */
void daShip_c::setInitMessage() {
    if (ship_playerStatus0() & daPyStts0_SHIP_RIDE_e) {
        return;
    } else if (!dComIfGs_isGetItem(1, 0)) {
        mNextMessageNo = 0x5de;
    } else if (checkForceMessage()) {
        return;
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_2110) && !dComIfGs_checkGetItem(dItemNo_BOMB_BAG_e)) {
        mNextMessageNo = 0x623;
    } else if (checkStateFlg(daSFLG_UNK800000_e)) {
        mNextMessageNo = 0x636;
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_3D02)) {
        if (ship_isStartStage(gabi::guest_addr("GanonA")) || ship_isStartStage(gabi::guest_addr("GanonB")) ||
            ship_isStartStage(gabi::guest_addr("GanonC")) || ship_isStartStage(gabi::guest_addr("GanonD")) ||
            ship_isStartStage(gabi::guest_addr("GanonE")) || ship_isStartStage(gabi::guest_addr("GanonN")) ||
            ship_isStartStage(gabi::guest_addr("GanonM")) || ship_isStartStage(gabi::guest_addr("GanonL")) ||
            ship_isStartStage(gabi::guest_addr("GanonJ")) || ship_isStartStage(gabi::guest_addr("Xboss0")) ||
            ship_isStartStage(gabi::guest_addr("Xboss1")) || ship_isStartStage(gabi::guest_addr("Xboss2")) ||
            ship_isStartStage(gabi::guest_addr("Xboss3")) || ship_isStartStage(gabi::guest_addr("GanonK")) ||
            ship_isStartStage(gabi::guest_addr("GTower")) || ship_isStartStage(gabi::guest_addr("Hyrule")) ||
            ship_isStartStage(gabi::guest_addr("Hyroom")) || ship_isStartStage(gabi::guest_addr("kenroom"))) {
            mNextMessageNo = 0xd77;
        } else {
            mNextMessageNo = 0xd78;
        }
    } else if (dComIfGs_isEventBit(ShipEvent::BARRIER_BREAK)) {
        mNextMessageNo = 0xd76;
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_2C01)) {
        mNextMessageNo = 0xd75;
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_2D08)) {
        if (ship_isStartStage(gabi::guest_addr("Hyrule"))) {
            mNextMessageNo = 0xd74;
        } else {
            mNextMessageNo = 0xd73;
        }
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_4004)) {
        if (dComIfGs_getTriforceNum() == 8) {
            mNextMessageNo = 0xd72;
        } else {
            mNextMessageNo = 0xd71;
        }
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_2E02)) {
        mNextMessageNo = 0xd70;
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_1610)) {
        mNextMessageNo = 0xd6f;
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_3A02)) {
        if (dComIfGs_isTact(4)) {
            mNextMessageNo = 0xd6e;
        } else {
            mNextMessageNo = 0xd6d;
        }
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_2E04)) {
        mNextMessageNo = 0xd6c;
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_1620)) {
        mNextMessageNo = 0xd6b;
    } else if (dComIfGs_isTact(3) || dComIfGs_isTact(4)) {
        mNextMessageNo = 0xd6a;
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_3910)) {
        if (!dComIfGs_isEventBit(ShipEvent::UNK_3380)) {
            mNextMessageNo = 0xd7b;
        } else {
            mNextMessageNo = 0xd69;
        }
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_3040)) {
        mNextMessageNo = 0xd68;
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_3810)) {
        mNextMessageNo = 0xd67;
    } else if (ship_checkMasterSwordEquip()) {
        mNextMessageNo = 0xd66;
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_2D10)) {
        mNextMessageNo = 0xd65;
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_1E40)) {
        if (ship_getStageType() == dStageType_DUNGEON_e) {
            mNextMessageNo = 0xd63;
        } else {
            mNextMessageNo = 0xd64;
        }
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_2F20)) {
        mNextMessageNo = 0xd62;
    } else if (dComIfGs_isSymbol(dSymbol_NAYRU_e)) {
        mNextMessageNo = 0xd5a;
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_1940)) {
        mNextMessageNo = 0xd61;
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_3E10)) {
        if (!dComIfGs_isEventBit(ShipEvent::UNK_3E01)) {
            mNextMessageNo = 0x1687;
        } else if (dComIfGs_isEventBit(ShipEvent::UNK_3F80)) {
            mNextMessageNo = 0x1689;
        } else {
            mNextMessageNo = 0x1688;
        }
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_1F02)) {
        mNextMessageNo = 0xd60;
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_1F04)) {
        if (!dComIfGs_isEventBit(ShipEvent::UNK_2110)) {
            mNextMessageNo = 0x621;
        } else if (dComIfGs_checkGetItem(dItemNo_BOMB_BAG_e)) {
            mNextMessageNo = 0x624;
        } else {
            mNextMessageNo = 0x623;
        }
    } else if (dComIfGs_isEventBit(ShipEvent::ENDLESS_NIGHT)) {
        if (dComIfGs_isEventBit(ShipEvent::UNK_2A01)) {
            mNextMessageNo = 0xd5f;
        } else if (dComIfGs_isEventBit(ShipEvent::UNK_0A01)) {
            mNextMessageNo = 0x608;
        } else {
            mNextMessageNo = 0x607;
        }
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_0A08)) {
        if (dComIfGs_isEventBit(ShipEvent::UNK_2A02)) {
            mNextMessageNo = 0xd5e;
        } else if (dComIfGs_isEventBit(ShipEvent::UNK_0A04)) {
            mNextMessageNo = 0x5f9;
        } else {
            mNextMessageNo = 0x5f8;
        }
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_0A20)) {
        if (dComIfGs_isSymbol(dSymbol_FARORE_e)) {
            mNextMessageNo = 0x5f6;
        } else if (dComIfGs_isEventBit(ShipEvent::UNK_2B80)) {
            mNextMessageNo = 0x5f5;
        } else {
            mNextMessageNo = 0x5f4;
        }
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_0A80)) {
        if (dComIfGs_isEventBit(ShipEvent::UNK_1980)) {
            mNextMessageNo = 0xd5d;
        } else {
            cXyz* windVec = dKyw_get_wind_vec();
            s32 a = cM_atan2s(windVec->x, windVec->z);
            if ((a < 0 ? -a : a) < 0x1000) {
                mNextMessageNo = 0x5ee;
            } else {
                mNextMessageNo = 0x5ef;
            }
        }
    } else if (dComIfGs_isEventBit(ShipEvent::UNK_0902)) {
        if (dComIfGs_isSymbol(dSymbol_DIN_e)) {
            mNextMessageNo = 0x5ec;
        } else if (dComIfGs_isEventBit(ShipEvent::UNK_0A10)) {
            mNextMessageNo = 0x5eb;
        } else {
            mNextMessageNo = 0x5ea;
        }
    } else {
        if (dComIfGs_isEventBit(ShipEvent::RODE_KORL)) {
            mNextMessageNo = 0xd5c;
        } else if (dComIfGs_isEventBit(ShipEvent::UNK_0908)) {
            mNextMessageNo = 0x5df;
        } else {
            mNextMessageNo = 0x5e0;
        }
    }
}



/* 02473080 */
u32 daShip_c::seStart(u32 i_seNum, cXyz* i_sePos) {
    return shipHD_startSound( i_seNum, i_sePos, 0u, (s32)m034A); /* mDoAud_seStart */
}



/* 024751E0 */
void daShip_c::setControllAngle(s16 angle) {
    s16 sVar1 = angle - m036C;
    s16 sVar2 = (s16)gabi::ftoi((f32)sVar1 * 0.05f);
    if (sVar2 == 0) {
        if (sVar1 > 0) {
            sVar2 = 1;
        } else if (sVar1 < 0) {
            sVar2 = -1;
        }
    }
    m036E += sVar2;
    m036C += m036E;
    cLib_addCalcAngleS(&m036E, 0, 0x14, 0x1000, 4);
}



/* 0247204C */
void daShip_c::getMaxWaterY(cXyz* shipPos) {
    if (daSea_ChkArea(shipPos->x, shipPos->z)) {
        shipPos->y = daSea_calcWave(shipPos->x, shipPos->z);
        if (m03F8 > shipPos->y) {
            shipPos->y = m03F8;
        }
    } else {
        if (m03F8 != -1000000000.0f) {
            shipPos->y = m03F8;
        } else {
            shipPos->y = m03F4;
        }
    }
}



/* 024759CC */
void daShip_c::setWaveAngle(be<s16>* param1, be<s16>* param2) {
    cXyz* local_front = ship_initVec(frontGuard, front, 0.0f, 0.0f, 180.0f);
    cXyz* local_back = ship_initVec(backGuard, back, 0.0f, 0.0f, -190.0f);
    cXyz* local_right = ship_initVec(rightGuard, right, -80.0f, 0.0f, 0.0f);
    cXyz* local_left = ship_initVec(leftGuard, left, 80.0f, 0.0f, 0.0f);

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_now(), shape_angle.x, shape_angle.y, shape_angle.z);

    gabi::Local<cXyz> frontPos;
    gabi::Local<cXyz> leftPos;
    gabi::Local<cXyz> rightPos;
    gabi::Local<cXyz> backPos;
    PSMTXMultVec(mDoMtx_now(), local_front, frontPos.get());
    PSMTXMultVec(mDoMtx_now(), local_back, backPos.get());
    PSMTXMultVec(mDoMtx_now(), local_right, rightPos.get());
    PSMTXMultVec(mDoMtx_now(), local_left, leftPos.get());

    getMaxWaterY(frontPos.get());
    getMaxWaterY(backPos.get());
    getMaxWaterY(rightPos.get());
    getMaxWaterY(leftPos.get());

    gabi::Local<cXyz> local_9c;
    cXyz_mi(frontPos.get(), local_9c.get(), backPos.get());
    f32 fbx = local_9c->x, fby = local_9c->y, fbz = local_9c->z;
    gabi::Local<cXyz> local_a8;
    cXyz_mi(rightPos.get(), local_a8.get(), leftPos.get());
    gabi::Local<cXyz> xz1;
    xz1->x = local_a8->x;
    xz1->z = local_a8->z;
    f32 rly = local_a8->y;
    xz1->y = 0.0f;
    *param2 = cM_atan2s(-rly, cXyz_abs(xz1.get()));
    gabi::Local<cXyz> xz2;
    xz2->z = fbz;
    xz2->x = fbx;
    xz2->y = 0.0f;
    s16 a1 = cM_atan2s(-fby, cXyz_abs(xz2.get()));
    *param1 = a1;

    s16 iVar2 = *param2 - m0372;
    s16 iVar3 = a1 - m0370;

    m0376 = (s16)gabi::ftoi(gabi::fmadds((f32)iVar2, 0.045f, (f32)(s16)m0376));
    m0372 += m0376;
    cLib_addCalcAngleS(&m0376, 0, 0x14, 0x1000, 4);
    m0374 = (s16)gabi::ftoi(gabi::fmadds((f32)iVar3, 0.045f, (f32)(s16)m0374));
    m0370 += m0374;
    cLib_addCalcAngleS(&m0374, 0, 0x14, 0x1000, 4);
}



/* 024753BC */
f32 daShip_c::getWaterY() {
    f32 waterY;
    BOOL waterHit = mAcch.ChkWaterHit();
    if (waterHit) {
        m03F8 = mAcch.m_wtr.m_height; /* mAcch.m_wtr.GetHeight() */
    } else {
        m03F8 = -1000000000.0f;
    }
    if (daSea_ChkArea(current.pos.x, current.pos.z)) {
        waterY = daSea_calcWave(current.pos.x, current.pos.z);
        if (!waterHit || waterY > m03F8) {
            return waterY;
        }
    }
    if (waterHit) {
        waterY = m03F8;
    } else {
        /* daPy_lk_c::checkBowMiniGame(): the current procedure (+0x430) */
        fopAc_ac_c* link = dComIfGp_getLinkPlayer();
        if (link && reinterpret_cast<ShipPlayer*>(dComIfGp_getLinkPlayer())->mProcID == 0x44) {
            waterY = 0.0f;
        } else {
            waterY = m03F4;
        }
    }
    return waterY;
}



/* 024754D8 */
void daShip_c::setYPos() {
    m03F4 = getWaterY();
    if (!checkStateFlg(daSFLG_UNK10_e)) {
        onStateFlg(daSFLG_UNK10_e);
        if (mCurMode != MODE_START_MODE_WARP_e && mCurMode != MODE_START_MODE_THROW_e && m0351 != 0xB /* DEMO_HWARP_DOWN_e */) {
            current.pos.y = m03F4;
        }
    }

    if (mCurMode == 12 || mCurMode == 15 || mCurMode == 14) {
        return;
    }

    f32 cy = current.pos.y;
    f32 fVar2 = m03F4 - cy;

    if (checkStateFlg(daSFLG_FLY_e)) {
        if (mCurMode == MODE_START_MODE_WARP_e || mCurMode == MODE_START_MODE_THROW_e) {
            return;
        }
        if (!(fVar2 < 0.0f)) {
            onStateFlg(daSFLG_LAND_e);
            if (!checkStateFlg(daSFLG_UNK2000000_e)) {
                offStateFlg(daSFLG_FLY_e);
                if (speed.y < -15.0f) {
                    speed.y = -15.0f;
                }
                m03D0 = speed.y * 0.5f;
                speedF *= 0.85f;
                m0388 = 30;
            }
        }
    } else {
        // HD: full effect from speed 100 (GameCube 55)
        f32 rate = std::fabs((f32)speedF) / 100.0f;
        if (rate > 1.0f) {
            rate = 1.0f;
        }

        current.pos.y = gabi::fmadds(fVar2 * 0.1f, rate, cy);
        f32 d = gabi::fmadds(m03F4 - current.pos.y, 0.05f, m03D0);
        if (d > 20.0f) {
            d = 20.0f;
        } else if (d < -20.0f) {
            d = -20.0f;
        }
        m03D0 = d;
        current.pos.y += d;
        f32 r = cM_rndF(1000.0f);
        m03B0 = (s16)gabi::ftoi((f32)(s16)m03B0 + gabi::fmadds(rate, 500.0f, r + 1000.0f));
        current.pos.y += gabi::fmadds(rate * 0.25f, cM_ssin(m03B0), 0.6f);

        cLib_addCalc(&m03D0, 0.0f, 0.05f, 1.0f, 0.05f);

        f32 fVar4 = current.pos.y;
        f32 fVar5 = m03F4 + 40.0f;
        if (fVar4 > fVar5) {
            current.pos.y = fVar5;
        } else {
            fVar5 = m03F4 - 60.0f;
            if (fVar4 < fVar5) {
                current.pos.y = fVar5;
            }
        }

        f32 k = gabi::fmadds(rate, 0.5f, 1.0f);
        f32 r2 = cM_rndF(800.0f);
        m03AE = (s16)gabi::ftoi(gabi::fmadds(r2 + 800.0f, k, (f32)(s16)m03AE));
        f32 old03AC = (s16)m03AC;
        f32 r3 = cM_rndF(600.0f);
        m03AC = (s16)gabi::ftoi(gabi::fmadds(r3 + 600.0f, k, old03AC));

        m0370 = (s16)gabi::ftoi((f32)(s16)m0370 + gabi::fmadds(rate * 100.0f, cM_ssin(m03AE), 30.0f));
        m0372 = (s16)gabi::ftoi((f32)(s16)m0372 + gabi::fmadds(rate * 115.0f, cM_ssin(m03AC), 35.0f));
    }
}



/* 02474A98 */
BOOL daShip_c::checkOutRange() {
    if (m034B == 0xFF) {
        return FALSE;
    }

    int pathIndex = 0;
    BOOL bVar4 = FALSE;
    int closestIndex = 0;
    u32 closestPoint = gabi::ea(&ship_zeroPoint); // HD: a zero vector until a closest point is found
    BOOL bVar5 = ship_getStageType() == dStageType_SEA_e;
    u32 path = gabi::ea(dPath_GetRoomPath(m034B, -1));
    while (path) {
        u32 num = gabi::at<ShipPath>(path)->mNum;
        /* the point iterator is the m_points read before the save checks below (GHS keeps it in a
         * register); the count and the next/previous points use a fresh read */
        u32 pnt = gabi::at<ShipPath>(path)->mPoints;
        if (num < 3) {
            path = shipHD_nextRoomPath( path, -1);
            pathIndex++;
            continue;
        }
        if (bVar5) {
            if ((pathIndex == 0 && dComIfGs_isEventBit(ShipEvent::UNK_0902)) || (pathIndex == 1 && dComIfGs_isSymbol(dSymbol_FARORE_e)) ||
                (pathIndex == 2 && ship_checkMasterSwordEquip())) {
                path = shipHD_nextRoomPath( path, -1);
                pathIndex++;
                continue;
            }
        }
        num = gabi::at<ShipPath>(path)->mNum;
        u32 points = gabi::at<ShipPath>(path)->mPoints;

        f32 minDist = 3.4028235e+38f;
        f32 cz = current.pos.z;
        f32 cx = current.pos.x;
        for (int i = 0; i < (int)num; pnt += 0x10, i++) {
            f32 dz = cz - gabi::at<ShipPoint>(pnt)->mPosition.z;
            f32 dx = cx - gabi::at<ShipPoint>(pnt)->mPosition.x;
            f32 distXZ = gabi::fmadds(dx, dx, dz * dz);
            if (minDist > distXZ) {
                closestIndex = i;
                minDist = distXZ;
                closestPoint = pnt + 4;
            }
        }

        int lastIndex = num - 1;
        u32 nextPoint, prevPoint;
        if (closestIndex == lastIndex) {
            nextPoint = points + 4;
        } else {
            nextPoint = points + closestIndex * 0x10 + 0x14;
        }
        if (closestIndex == 0) {
            prevPoint = points + lastIndex * 0x10 + 4;
        } else {
            prevPoint = points + closestIndex * 0x10 - 0xC;
        }
        cXyz* np = gabi::at<cXyz>(nextPoint);
        cXyz* pp = gabi::at<cXyz>(prevPoint);
        cXyz* cp = gabi::at<cXyz>(closestPoint);

        s16 angleNext = cM_atan2s(np->x - cp->x, np->z - cp->z);
        s16 anglePrev = cM_atan2s(cp->x - pp->x, cp->z - pp->z);
        s16 angleCurrent = cM_atan2s(current.pos.x - cp->x, current.pos.z - cp->z);

        s16 diffNext = angleCurrent - angleNext;
        s16 diffPrev = cM_atan2s(current.pos.x - pp->x, current.pos.z - pp->z) - anglePrev;

        if ((s16)(angleNext - anglePrev) >= 0) {
            if (diffNext < 0) {
                m038C = angleNext + 0x4000;
                bVar4 = TRUE;
            }
            if (diffPrev < 0) {
                m038C = anglePrev + 0x4000;
                bVar4 = TRUE;
            }
        } else if (diffNext < 0 && diffPrev < 0) {
            f32 dzNext = np->z - current.pos.z;
            f32 dxNext = np->x - current.pos.x;
            f32 distNext = std_sqrtf(gabi::fmadds(dxNext, dxNext, dzNext * dzNext));
            f32 dzPrev = pp->z - current.pos.z;
            f32 dxPrev = pp->x - current.pos.x;
            f32 distPrev = std_sqrtf(gabi::fmadds(dxPrev, dxPrev, dzPrev * dzPrev));
            if (distNext > distPrev) {
                m038C = anglePrev + 0x4000;
            } else {
                m038C = angleNext + 0x4000;
            }
            bVar4 = TRUE;
        }

        if (bVar4) {
            if (ship_play()->mMiniGameType == 1 /* dComIfGp_getMiniGameType() */) {
                mNextMessageNo = 0x60a;
            } else if (bVar5 && pathIndex != 3) {
                mNextMessageNo = 0x609;
            } else {
                mNextMessageNo = 0x5dd;
            }
            onStateFlg(daSFLG_UNK400000_e);
            return TRUE;
        }
        path = shipHD_nextRoomPath( path, -1);
        pathIndex++;
    }
    return FALSE;
}



/* 02474F4C */
void daShip_c::firstDecrementShipSpeed(f32 decrementSpeed) {
    if (cLib_addCalc(&speedF, decrementSpeed, 0.1f, 5.0f, 1.0f) < 3.0f) {
        m03E0 = 10000.0f;
    }
}



/* 024752C4 */
s16 daShip_c::getAimControllAngle(s16 referenceAngle) {
    s16 aimControlAngle = (s16)(shape_angle.y - referenceAngle) * 7;
    if (aimControlAngle > 0x600) {
        aimControlAngle = 0x600;
    } else if (aimControlAngle < -0x600) {
        aimControlAngle = -0x600;
    }
    return aimControlAngle;
}



/* 02475030 */
BOOL daShip_c::procCraneUp_init() {
    if (isProc(ShipProcedure::procCraneUp)) {
        return FALSE;
    }
    setProc(ShipProcedure::procCraneUp);
    if (mCraneBaseAngle > 0) {
        m03A6 = 0x800;
    } else {
        m03A6 = -0x800;
    }
    seStart(ShipSound::JA_SE_LK_SHIP_CRANE_SALVAGE, &current.pos);
    mCurMode = MODE_CRANE_UP_e;
    reinterpret_cast<ShipRipple*>(mRipple)->mRate = 1.0f; /* mRipple.setRate(1.0f) */
    offStateFlg(daSFLG_CRANE_UP_END_e);
    gabi::Local<cXyz> v;
    v->x = 0.0f;
    v->y = 1.0f;
    v->z = 0.0f;
    ship_StartShock(7, -0x31, v.get());
    v->x = 0.0f;
    v->y = 1.0f;
    v->z = 0.0f;
    ship_StartQuake(4, 1, v.get());
    speedF = 0.0f;
    return TRUE;
}



/* 02474FB0 */
BOOL daShip_c::procToolDemo_init() {
    setProc(ShipProcedure::procToolDemo);
    mCurMode = MODE_DEMO_e;
    offStateFlg(daSFLG_FLY_e);
    return TRUE;
}



/* 02474FEC */
BOOL daShip_c::procZevDemo_init() {
    setProc(ShipProcedure::procZevDemo);
    mCurMode = MODE_DEMO_e;
    offStateFlg(daSFLG_FLY_e);
    m0350 = 4;
    return TRUE;
}



/* 0247309C */
BOOL daShip_c::procTalkReady_init() {
    setProc(ShipProcedure::procTalkReady);
    mAnmTransform = nullptr;
    if (ship_playerStatus0() & daPyStts0_SHIP_RIDE_e) {
        if ((s16)(shape_angle.y - m038C) > 0) {
            m03B4 = dRes_INDEX_SHIP_BCK_FN_LOOK_R_e;
        } else {
            m03B4 = dRes_INDEX_SHIP_BCK_FN_LOOK_L_e;
        }
    } else if ((s16)(fopAcM_searchPlayerAngleY(this) - shape_angle.y) > 0) {
        m03B4 = dRes_INDEX_SHIP_BCK_FN_LOOK_L_e;
    } else {
        m03B4 = dRes_INDEX_SHIP_BCK_FN_LOOK_R_e;
    }
    J3DAnmTransform* pAnimRes = ship_getRes(m03B4);
    mpHeadAnm->setAnm(pAnimRes, 0, 5.0f, 1.0f, 0.0f, -1.0f, nullptr);
    seStart(ShipSound::JA_SE_SHIP_LOOK_BACK, &eyePos);
    mCurMode = MODE_TALK_e;
    return TRUE;
}



/* 024749DC */
BOOL daShip_c::procTalk_init() {
    setProc(ShipProcedure::procTalk);
    m0430 = fpcM_ERROR_PROCESS_ID_e;
    m038A = dRes_INDEX_SHIP_BCK_FN_TALK_A_e;
    mCurMode = MODE_TALK_e;
    mAnmTransform = ship_getRes(m038A);
    J3DFrameCtrl_init(&mFrameCtrl, anm_getFrameMax(mAnmTransform));
    setInitMessage();
    onStateFlg(daSFLG_UNK8000_e);
    return TRUE;
}



/* 02472AB0 */
BOOL daShip_c::procTornadoUp_init() {
    u32 camera = ship_getCameraAddress(dComIfGp_getPlayerCameraID0());
    onStateFlg(daSFLG_FLY_e | daSFLG_UNK1000_e);
    mCurMode = MODE_TORNADO_UP_e;
    setProc(ShipProcedure::procTornadoUp);
    m037A = 1;
    camera_Stop(camera);
    speed.y = 0.0f;
    gravity = 0.0f;
    fopAc_ac_c* t = mTornadoActor;
    gabi::Local<cXyz> tornadoPos;
    tornadoPos->x = t->current.pos.x;
    tornadoPos->y = t->current.pos.y + 2500.0f;
    tornadoPos->z = t->current.pos.z + 4000.0f;
    gabi::Local<cXyz> pos;
    pos->set(current.pos.x, current.pos.y, current.pos.z);
    camera_Set(camera, pos.get(), tornadoPos.get());
    m03A6 = 0;
    /* fopAcM_seStartCurrent(mTornadoActor, 0x186C, 0) (HD inline, null checks) */
    fopAc_ac_c* ta = mTornadoActor;
    if (ta != nullptr && gabi::ea(&ta->current.pos) != 0) {
        mDoAud_seStart(0x186C, &ta->current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(ta)));
    }
    dComIfGs_onEventBit(ShipEvent::UNK_3E40);
    return TRUE;
}



/* 02472E14 */
BOOL daShip_c::procWhirlDown_init() {
    setProc(ShipProcedure::procWhirlDown);
    mCurMode = MODE_WHIRL_DOWN_e;
    gravity = 0.0f;
    speed.y = 0.0f;
    onStateFlg(daSFLG_FLY_e | daSFLG_UNK1000_e);
    u32 camera = ship_getCameraAddress(dComIfGp_getPlayerCameraID0());
    camera_Stop(camera);
    fopAc_ac_c* w = mWhirlActor;
    gabi::Local<cXyz> pos;
    pos->set(w->current.pos.x, w->current.pos.y, w->current.pos.z);
    gabi::Local<cXyz> local_38;
    local_38->x = w->current.pos.x;
    local_38->y = w->current.pos.y + 2500.0f;
    local_38->z = w->current.pos.z + 4000.0f;
    camera_Set(camera, pos.get(), local_38.get());
    m037A = 0;
    return TRUE;
}



/* 02477278 */
void daShip_c::setEffectData(f32 param_1, s16 param_2) {
    cXyz* wave_l_direction = ship_initVec(waveLeftDirectionGuard, waveLeftDirection, 0.5f, 1.0f, -0.3f);
    cXyz* wave_r_direction = ship_initVec(waveRightDirectionGuard, waveRightDirection, -0.5f, 1.0f, -0.3f);
    u32 waveR = gabi::ea(mWaveR), waveL = gabi::ea(mWaveL), splash = gabi::ea(mSplash), track = gabi::ea(mTrack);

    if (checkStateFlg(daSFLG_FLY_e) && current.pos.y - 40.0f > param_1) {
        gabi::at<ShipWave>(waveL)->mState = 1; /* stop() */
        gabi::at<ShipTrack>(track)->mState = 1;
        gabi::at<ShipWave>(waveR)->mState = 1;
        gabi::at<ShipSplash>(splash)->mState = 1;
    } else if (!(speedF > 0.0f)) {
        gabi::at<ShipWave>(waveR)->mState = 1;
        gabi::at<ShipWave>(waveL)->mState = 1;
        gabi::at<ShipSplash>(splash)->mState = 1;
    }
    f32 fVar3 = speedF + 150.0f;
    mEffPos.x = gabi::fmadds(fVar3, cM_ssin(shape_angle.y), current.pos.x);
    mEffPos.y = param_1;
    mEffPos.z = gabi::fmadds(fVar3, cM_scos(shape_angle.y), current.pos.z);
    getMaxWaterY(&mEffPos);
    if (!checkStateFlg(daSFLG_FLY_e) || param_1 > current.pos.y) {
        if (std::fabs((f32)mFwdVel) > 3.0f && std::fabs((f32)speedF) > 3.0f && mFwdVel * speedF > 0.0f) {
            // HD: the bow waves start at speed 21 (GameCube 11)
            if (mFwdVel > 21.0f) {
                if (gabi::at<ShipWave>(waveL)->mEmitter == 0) {
                    u32 e = gabi::ea(dComIfGp_particle_set(0x37 /* ID_AK_JN_SHIPWAVE00 */, &mEffPos, &shape_angle, nullptr, 0xFF,
                                                           (dPa_levelEcallBack*)mWaveL));
                    gabi::at<ShipWave>(waveL)->mTimer = 20; /* setTimer(20) */
                    if (e) {
                        JPABaseEmitter_setDirection(gabi::at<JPABaseEmitter>(e), wave_l_direction->x, wave_l_direction->y,
                                                    wave_l_direction->z);
                    }
                }
                if (gabi::at<ShipWave>(waveR)->mEmitter == 0) {
                    u32 e = gabi::ea(dComIfGp_particle_set(0x37, &mEffPos, &shape_angle, nullptr, 0xFF, (dPa_levelEcallBack*)mWaveR));
                    gabi::at<ShipWave>(waveR)->mTimer = 20;
                    if (e) {
                        JPABaseEmitter_setDirection(gabi::at<JPABaseEmitter>(e), wave_r_direction->x, wave_r_direction->y,
                                                    wave_r_direction->z);
                    }
                }
                if (gabi::at<ShipSplash>(splash)->mEmitter == 0) {
                    dComIfGp_particle_set(0x35 /* ID_AK_JN_SHIPSPLASH00 */, &mEffPos, &shape_angle, nullptr, 0xFF,
                                          (dPa_levelEcallBack*)mSplash);
                }
            }
            if (gabi::at<ShipTrack>(track)->mEmitter == 0) {
                /* dComIfGp_particle_setShipTail(ID_AK_JN_SHIPTAIL00, ...) (group 5) */
                dPa_control_set(dComIfGp_getParticle(), 5, 0x36, &current.pos, &shape_angle, nullptr, 0, (dPa_levelEcallBack*)mTrack,
                                -1, nullptr, nullptr, nullptr);
            }
            if (checkStateFlg(daSFLG_FLY_e | daSFLG_LAND_e)) {
                gabi::Local<GXColor> amb;
                gabi::Local<GXColor> diff;
                dKy_get_seacolor(amb.get(), diff.get());
                u32 e = gabi::ea(dComIfGp_particle_set(0x34 /* ID_AK_JN_SHIPIMPACT00 */, &mEffPos, &shape_angle, nullptr, 0xFF, nullptr,
                                                       -1, amb.get()));
                if (e) {
                    // HD: over 75 (GameCube 30)
                    f32 fVar2 = gabi::fnmsubs((speed.y - -15.0f) / 75.0f, 50.0f, 10.0f);
                    if (fVar2 < 10.0f) {
                        fVar2 = 10.0f;
                    } else if (fVar2 > 60.0f) {
                        fVar2 = 60.0f;
                    }
                    gabi::at<ShipEmitter>(e)->mRate = fVar2; /* setRate */
                }
                seStart(ShipSound::JA_SE_SHIP_JUMP_ALIGHT, &current.pos);
            }
        }
    }

    gabi::at<ShipTrack>(track)->mIndirectScroll = -0.04f; /* setIndirectTexData(ef_ind_scroll, ef_ind_scale) */
    gabi::at<ShipTrack>(track)->mIndirectScale = 4.0f;
    gabi::at<ShipTrack>(track)->mSpeed = mFwdVel; /* setSpeed */
    if (!(mFwdVel < 0.0f)) {
        gabi::at<ShipWave>(waveR)->mSpeed = 0.7f;
        gabi::at<ShipWave>(waveL)->mSpeed = 0.7f;
    } else {
        gabi::at<ShipWave>(waveR)->mSpeed = -0.7f;
        gabi::at<ShipWave>(waveL)->mSpeed = -0.7f;
    }
    f32 fVar1 = (f32)(s16)(shape_angle.z - param_2) * (1.0f / 0x4000);
    if (fVar1 > 0.3f) {
        fVar1 = 0.3f;
    } else if (fVar1 < -0.3f) {
        fVar1 = -0.3f;
    }
    gabi::at<ShipWave>(waveR)->mPitch = fVar1 + 1.0f; /* setPitch */
    gabi::at<ShipWave>(waveL)->mPitch = 1.0f - fVar1;
    gabi::at<ShipSplash>(splash)->mSpeed = mFwdVel;        /* setSpeed */
    gabi::at<ShipSplash>(splash)->mMaxSpeed = 30.0f;        /* setMaxSpeed(ef_sp_max_speed) */
    /* setAnchor(front (-80, -50, -150), back (-40, -100, -350)); mirrored for the left wave */
    gabi::at<ShipWave>(waveR)->mAnchorFront.x = -80.0f;
    gabi::at<ShipWave>(waveR)->mAnchorFront.y = -50.0f;
    gabi::at<ShipWave>(waveR)->mAnchorFront.z = -150.0f;
    gabi::at<ShipWave>(waveR)->mAnchorBack.x = -40.0f;
    gabi::at<ShipWave>(waveR)->mAnchorBack.y = -100.0f;
    gabi::at<ShipWave>(waveR)->mAnchorBack.z = -350.0f;
    gabi::at<ShipWave>(waveL)->mAnchorFront.x = 80.0f;
    gabi::at<ShipWave>(waveL)->mAnchorFront.y = -50.0f;
    gabi::at<ShipWave>(waveL)->mAnchorFront.z = -150.0f;
    gabi::at<ShipWave>(waveL)->mAnchorBack.x = 40.0f;
    gabi::at<ShipWave>(waveL)->mAnchorBack.y = -100.0f;
    gabi::at<ShipWave>(waveL)->mAnchorBack.z = -350.0f;
    gabi::at<ShipWave>(waveL)->mMaxDisSpeed = 2.0f; /* setMaxDisSpeed(ef_dis_speed) */
    gabi::at<ShipWave>(waveR)->mMaxDisSpeed = 2.0f;
    gabi::at<ShipWave>(waveL)->mMaxSpeed = 40.0f; /* setMaxSpeed(40) */
    gabi::at<ShipWave>(waveR)->mMaxSpeed = 40.0f;
}



/* 024752F4 */
void daShip_c::setRoomInfo() {
    s32 roomId;
    if (mAcch.GetGroundH() != -1000000000.0f) {
        cBgS_PolyInfo* gnd = static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd); /* mAcch.m_gnd */
        roomId = shipHD_getRoomId( dComIfG_Bgsp(), gnd);
        /* tevStr.mEnvrIdxOverride (+0xBA) */
        reinterpret_cast<ShipTevStr*>(&tevStr)->mEnvrIdxOverride = (u8)shipHD_getPolyColor(dComIfG_Bgsp(), gnd);
        m03C4 = shipHD_getGroundCode( dComIfG_Bgsp(), gnd);
    } else {
        roomId = ship_stayNo; /* dComIfGp_roomControl_getStayNo() */
        m03C4 = 0;
    }
    tevStr.mRoomNo = roomId;
    m034A = dComIfGp_getReverb(roomId);
    mStts.mRoomId = roomId;
    current.roomNo = roomId;
}



/* 02476204 */
void daShip_c::incRopeCnt(int lengthChange, int minSegmentLimit) {
    cXyz* l_rope_base_vec = &ship_ropeBase;
    s32 currRopeCnt = mRopeCnt;
    u32 ropeSegments = gabi::ea(mRopeLine.getPos(0)); /* mRopeLine.getPos(0) */
    u32 currRopeSegment = gabi::ea(&mRopeLineSegments[0]) + currRopeCnt * 0xC;
    s32 targetRopeCnt = currRopeCnt + lengthChange;
    // HD: at most 125 segments (GameCube 250)
    if (targetRopeCnt >= 125) {
        targetRopeCnt = 125;
    } else if (targetRopeCnt < minSegmentLimit) {
        targetRopeCnt = minSegmentLimit;
    }
    lengthChange = targetRopeCnt - currRopeCnt;
    if (lengthChange > 0) {
        gabi::Local<cXyz> ropeDisplacement;
        cXyz_ml(l_rope_base_vec, ropeDisplacement.get(), (f32)lengthChange);
        for (int i = 0; i < mRopeCnt; i++, ropeSegments += 0xC) {
            PSVECAdd(gabi::at<cXyz>(ropeSegments), ropeDisplacement.get(), gabi::at<cXyz>(ropeSegments));
        }
    } else {
        ropeSegments += currRopeCnt * 0xC;
    }
    gabi::Local<cXyz> d;
    for (int i = mRopeCnt; i < targetRopeCnt; i++, currRopeSegment += 0xC, ropeSegments += 0xC) {
        gabi::at<cXyz>(currRopeSegment)->copy(*l_rope_base_vec);
        if (mRopeCnt) {
            cXyz_mi(gabi::at<cXyz>(ropeSegments - 0xC), d.get(), l_rope_base_vec);
            gabi::at<cXyz>(ropeSegments)->copy(*d);
        }
    }
    if (lengthChange != 0) {
        if (minSegmentLimit == 20 && targetRopeCnt == 20) {
            seStart(ShipSound::JA_SE_LK_SHIP_CRANE_STOP, &m102C);
        } else {
            seStart(ShipSound::JA_SE_LK_SHIP_CRANE_WORK, &m102C);
        }
    }
    mRopeCnt = targetRopeCnt;
}



/* 024763B0 */
void daShip_c::setRopePos() {
    /* static cXyz ripple_scale(0.6f, 0.6f, 0.6f); rope_offset {160, 0, 0} (0x101D03CC);
     * water_drop_scale {1.5, 1.0, 1.5} (0x101D03D8) */
    cXyz* ripple_scale = ship_initVec(rippleScaleGuard, rippleScale, 0.6f, 0.6f, 0.6f);


    cXyz* l_rope_base_vec = &ship_ropeBase;

    model_getAnmMtx(mpSalvageArmModel, VFNCR_JNT_V_CRANE_ROTATION_e); // HD: marked dirty once more
    u32 base = gabi::ea(mRopeLine.getPos(0)); /* mRopeLine.getPos(0) */
    s16 cnt = mRopeCnt;
    u32 cur, seg;
    if (cnt == 0) {
        cur = base;
        seg = gabi::ea(&mRopeLineSegments[0]);
    } else {
        cur = base + (cnt - 1) * 0xC;
        seg = gabi::ea(&mRopeLineSegments[0]) + (cnt - 1) * 0xC;
    }
    gabi::Local<cXyz> spF8;
    spF8->copy(*gabi::at<cXyz>(cur));
    PSMTXMultVec(model_getAnmMtx(mpSalvageArmModel, VFNCR_JNT_V_CRANE_ROTATION_e), &ship_ropeOffset, gabi::at<cXyz>(cur));

    gabi::Local<cXyz> spEC;
    if (isProc(ShipProcedure::procCraneUp)) {
        Mtx34* lm = J3DModel_getBaseTRMtx(mpLinkModel);
        f32 cx = gabi::at<cXyz>(cur)->x;
        spEC->x = lm->m[0][3] - cx;
        spEC->y = lm->m[1][3] - gabi::at<cXyz>(cur)->y;
        spEC->z = lm->m[2][3] - gabi::at<cXyz>(cur)->z;
        gabi::Local<cXyz> nrm;
        cXyz_normalize(spEC.get(), nrm.get());
        PSVECScale(spEC.get(), spEC.get(), 10.0f);
        cur -= 0xC;
        seg -= 0xC;
        gabi::Local<cXyz> t;
        for (int i = mRopeCnt - 2; i >= 0; i--, cur -= 0xC, seg -= 0xC) {
            cXyz_pl(gabi::at<cXyz>(cur + 0xC), t.get(), spEC.get());
            gabi::at<cXyz>(cur)->copy(*t);
            gabi::at<cXyz>(seg)->copy(*cXyz_Zero);
        }
    } else {
        cur -= 0xC;
        seg -= 0xC;
        gabi::Local<cXyz> d;
        gabi::Local<cXyz> t68;
        gabi::Local<cXyz> t8c;
        gabi::Local<cXyz> t80;
        gabi::Local<cXyz> t74;
        gabi::Local<cXyz> t5c;
        gabi::Local<cXyz> t38;
        for (int i = mRopeCnt - 2; i >= 0; i--, cur -= 0xC, seg -= 0xC) {
            spF8->copy(*gabi::at<cXyz>(cur));
            f32 k = gabi::at<cXyz>(cur)->y - m03F4 >= 0.0f ? 0.9f : 0.6f;
            PSVECScale(gabi::at<cXyz>(seg), gabi::at<cXyz>(seg), k);
            // HD: plus a debug register (REG18_F(9))
            gabi::at<cXyz>(seg)->y = gabi::at<cXyz>(seg)->y - (ship_ropeGravity + 3.0f);
            PSVECAdd(gabi::at<cXyz>(cur), gabi::at<cXyz>(seg), gabi::at<cXyz>(cur));
            cXyz_mi(gabi::at<cXyz>(cur), d.get(), gabi::at<cXyz>(cur + 0xC));
            spEC->copy(*d);
            f32 fVar17 = cXyz_abs(spEC.get());
            if (fVar17 < 0.01f) {
                cXyz_pl(gabi::at<cXyz>(cur + 0xC), t68.get(), l_rope_base_vec);
                gabi::at<cXyz>(cur)->copy(*t68);
            } else {
                cXyz_ml(spEC.get(), t8c.get(), 10.0f);
                shipHD_divideVector( t8c.get(), t80.get(), fVar17);
                cXyz_pl(gabi::at<cXyz>(cur + 0xC), t74.get(), t80.get());
                gabi::at<cXyz>(cur)->copy(*t74);
            }
            cXyz_mi(gabi::at<cXyz>(cur), t5c.get(), spF8.get());
            cXyz_ml(t5c.get(), t38.get(), 0.05f);
            PSVECAdd(gabi::at<cXyz>(seg), t38.get(), gabi::at<cXyz>(seg));
        }
        if (mRopeCnt == 20 && checkStateFlg(daSFLG_UNK10000000_e)) {
            ShipMtxBlock* blk = model_mtxBlock(mpSalvageArmModel);
            u32 r4 = gabi::ea(mRopeLine.getPos(0));
            blk->mFlags |= 0x10;
            Mtx34* am = gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + VFNCR_JNT_V_CRANE_ROTATION_e * 0x30);
            gabi::Local<cXyz> spE0;
            spE0->x = am->m[0][3];
            spE0->y = am->m[1][3];
            spE0->z = am->m[2][3];
            shipHD_subtractVector( spE0.get(), r4, spE0.get());
            gabi::Local<cXyz> xz;
            xz->z = spE0->z;
            xz->y = 0.0f;
            xz->x = spE0->x;
            if (PSVECSquareMag(xz.get()) < 2500.0f) {
                f32 f2 = 16.0f * cM_scos(shape_angle.x);
                u32 rs = gabi::ea(&mRopeLineSegments[0]);
                if (shape_angle.x <= 0) {
                    f2 = -f2;
                }
                gabi::Local<cXyz> spD0;
                spD0->x = f2 * cM_ssin(shape_angle.y);
                spD0->y = 0.0f;
                spD0->z = f2 * cM_scos(shape_angle.y);
                for (int i = 0; i < 20; i++, rs += 0xC) {
                    PSVECAdd(gabi::at<cXyz>(rs), spD0.get(), gabi::at<cXyz>(rs));
                    PSVECScale(spD0.get(), spD0.get(), 0.95f);
                }
                offStateFlg(daSFLG_UNK10000000_e);
            }
        }
    }

    cur = gabi::ea(mRopeLine.getPos(0));
    cXyz* curP = gabi::at<cXyz>(cur);
    s16 sVar14, sVar12;
    f32 cx_, cy_, cz_; /* spC8 */
    f32 bx, by, bz;
    if (mRopeCnt > 2) {
        gabi::Local<cXyz> spC8;
        cXyz_mi(curP, spC8.get(), gabi::at<cXyz>(cur + 0xC));
        sVar14 = 0;
        sVar12 = 0;
        cz_ = spC8->z;
        by = curP->y;
        cy_ = spC8->y;
        bx = curP->x;
        cx_ = spC8->x;
        bz = curP->z;
    } else {
        Mtx34* am = model_getAnmMtx(mpSalvageArmModel, VFNCR_JNT_V_CRANE_ROTATION_e);
        s16 craneBase = mCraneBaseAngle;
        s16 a0398 = m0398;
        by = curP->y;
        cy_ = by - am->m[1][3];
        bx = curP->x;
        f32 k = gabi::fnmsubs((f32)(0x8000 - craneBase), (f32)a0398 / (f32)craneBase, (f32)(0x8000 - a0398 * 2));
        bz = curP->z;
        sVar12 = (s16)gabi::ftoi(k);
        cx_ = bx - am->m[0][3];
        cz_ = bz - am->m[2][3];
        sVar14 = 0x7FFF;
    }
    Mtx34* lm = J3DModel_getBaseTRMtx(mpLinkModel);
    f32 sn = cM_ssin(shape_angle.y);
    f32 cs = cM_scos(shape_angle.y);
    f32 fVar17 = lm->m[1][3];
    f32 bcx = gabi::fmsubs(cs, cx_, sn * cz_);
    f32 bcz = gabi::fmadds(sn, cx_, cs * cz_);
    f32 bcy = cy_;
    mDoMtx_stack_c::transS(bx, by, bz);
    s16 ax = cM_atan2s(bcz, bcy) + sVar14;
    s16 ay = shape_angle.y;
    s16 az = cM_atan2s(-bcx, std_sqrtf(gabi::fmadds(bcy, bcy, bcz * bcz))) + sVar12;
    mDoMtx_ZXYrotM(mDoMtx_now(), ax, ay, az);
    mDoMtx_XrotM(mDoMtx_now(), -0x4000);
    if (m0392 == dRes_INDEX_SHIP_BCK_FN_MAST_ON2_e) {
        /* mpBodyAnm->getAnm()->getTransform(6, &info) (HD inline: a flag at +0x54, the table at +0x30) */
        u32 morf = gabi::ea(mpBodyAnm.get());
        u32 tbl = gabi::at<ShipMorfTransform>(morf)->mTransforms;
        gabi::at<ShipMorfTransform>(morf)->mFlags = gabi::at<ShipMorfTransform>(morf)->mFlags | 8;
        gabi::Local<be<u32>[3]> info;
        (*info)[0] = gabi::at<ShipTransformTable>(tbl)->mScale[0];
        (*info)[1] = gabi::at<ShipTransformTable>(tbl)->mScale[1];
        (*info)[2] = gabi::at<ShipTransformTable>(tbl)->mScale[2];
        mDoMtx_stack_c::scaleM(*reinterpret_cast<be<f32>*>(&(*info)[0]), *reinterpret_cast<be<f32>*>(&(*info)[1]),
                               *reinterpret_cast<be<f32>*>(&(*info)[2]));
    }
    J3DModel_setBaseTRMtx(mpLinkModel, mDoMtx_now());

    f32 fVar1 = curP->y;
    getMaxWaterY(curP);
    f32 fVar2 = curP->y;
    curP->y = fVar1;

    if (fVar2 > fVar1) {
        u32 r3 = cur;
        u32 p = cur;
        for (int i = mRopeCnt; i > 0; i--, p += 0xC) {
            if (!(gabi::at<cXyz>(p)->y > fVar2)) {
                r3 = p;
            }
        }
        mCraneRipplePos.x = gabi::at<cXyz>(r3)->x;
        mCraneRipplePos.y = fVar2;
        mCraneRipplePos.z = gabi::at<cXyz>(r3)->z;
        if (reinterpret_cast<ShipRipple*>(mRipple)->mEmitter == 0) {
            /* dComIfGp_particle_setShipTail(ID_AK_JN_HAMON00, ...) (group 5) */
            dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &mCraneRipplePos, nullptr, ripple_scale, 0xFF,
                            (dPa_levelEcallBack*)mRipple, -1, nullptr, nullptr, nullptr);
            if (reinterpret_cast<ShipRipple*>(mRipple)->mEmitter != 0) {
                reinterpret_cast<ShipRipple*>(mRipple)->mRate = 0.0f; /* mRipple.setRate(0.0f) */
                if (m034F == 0) {
                    fopKyM_createWpillar(&mCraneRipplePos, 0.7f, 0.7f, 0);
                    seStart(ShipSound::JA_SE_LK_SHIP_CRANE_DROP, &mCraneRipplePos);
                    gabi::Local<cXyz> v;
                    v->x = 0.0f;
                    v->y = 1.0f;
                    v->z = 0.0f;
                    ship_StartShock(3, 1, v.get());
                }
            }
        }
    } else {
        if (fVar17 < fVar2 && !m034F && mRopeCnt >= 20) {
            m034F = 20;
            seStart(ShipSound::JA_SE_LK_SHIP_CRANE_LIFTUP, &mCraneRipplePos);
            if (mCurMode != MODE_CRANE_UP_e) {
                if (m19AC.getEmitter() == nullptr) {
                    u32 rs = gabi::ea(mRopeLine.getPos(0));
                    /* dComIfGp_particle_setP1(ID_IT_JN_LK_NURE_POTA00, ...) (group 1) */
                    u32 emitter = gabi::ea(dPa_control_set(dComIfGp_getParticle(), 1, 0x38, gabi::at<cXyz>(rs), nullptr, nullptr, 0xFF,
                                                           (dPa_levelEcallBack*)&m19AC, -1, nullptr, nullptr, nullptr));
                    if (emitter) {
                        /* setGlobalParticleScale(1.5f, 1.5f) (HD: three values), setEmitterScale, setLifeTime(30) */
                        gabi::at<ShipEmitter>(emitter)->mGlobalParticleScale.x = 1.5f;
                        gabi::at<ShipEmitter>(emitter)->mGlobalParticleScale.y = 1.5f;
                        gabi::at<ShipEmitter>(emitter)->mGlobalParticleScale.z = 1.0f;
                        f32 sy = ship_waterDropScale.y;
                        f32 sx = ship_waterDropScale.x;
                        f32 sz = ship_waterDropScale.z;
                        gabi::at<ShipEmitter>(emitter)->mEmitterScale.x = sx;
                        gabi::at<ShipEmitter>(emitter)->mEmitterScale.y = sy;
                        gabi::at<ShipEmitter>(emitter)->mEmitterScale.z = sz;
                        gabi::at<ShipEmitter>(emitter)->mLifeTime = 30;
                    }
                }
                fopKyM_createWpillar(&mCraneRipplePos, 0.5f, 0.7f, 0);
                gabi::Local<cXyz> v;
                v->x = 0.0f;
                v->y = 1.0f;
                v->z = 0.0f;
                ship_StartShock(3, 1, v.get());
                if (reinterpret_cast<ShipRipple*>(m19C0)->mEmitter == 0) {
                    u32 e = gabi::ea(dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &m1074, nullptr, ripple_scale, 0xFF,
                                                     (dPa_levelEcallBack*)m19C0, -1, nullptr, nullptr, nullptr));
                    if (e) {
                        gabi::at<ShipEmitter>(e)->mVolumeSize = 30; /* setVolumeSize(30) */
                    }
                }
            }
        }
        // HD: mRipple.end() (GameCube remove())
        dPa_rippleEcallBack_end((dPa_rippleEcallBack*)mRipple);
    }
    if (m034F) {
        m034F--;
    }
    if (m19AC.getEmitter() == nullptr) {
        dPa_rippleEcallBack_end((dPa_rippleEcallBack*)m19C0);
    }
    u32 rb = gabi::ea(mRopeLine.getPos(0));
    f32 r1 = cM_rndFX(20.0f);
    m1074.x = gabi::at<cXyz>(rb)->x + r1;
    m1074.y = m03F4;
    u32 rb2 = gabi::ea(mRopeLine.getPos(0));
    f32 r2 = cM_rndFX(20.0f);
    m1074.z = gabi::at<cXyz>(rb2)->z + r2;
}



/* 02476188 */
f32 daShip_c::getAnglePartRate() {
    f32 fVar1;
    if (m0392 == dRes_INDEX_SHIP_BCK_FN_MAST_OFF2_e) {
        fVar1 = (3.0f - mpBodyAnm->getFrame()) * 0.33333334f;
    } else {
        fVar1 = (mpBodyAnm->getFrame() - 5.0f) * 0.33333334f;
    }
    if (fVar1 > 1.0f) {
        fVar1 = 1.0f;
    } else {
        fVar1 = fVar1 >= 0.0f ? fVar1 : 0.0f;
    }
    return fVar1;
}



/* 02472C04 */
void daShip_c::setTornadoActor() {
    mTornadoActor = fopAcM_SearchByID(mTornadoID);
    if (!mTornadoActor) {
        mTornadoID = fpcM_ERROR_PROCESS_ID_e;
        mTornadoActor = nullptr;
        return;
    }
    gabi::Local<cXyz> local_20;
    cXyz_mi(&current.pos, local_20.get(), &mTornadoActor->current.pos);
    gabi::Local<cXyz> xz;
    xz->y = 0.0f;
    xz->x = local_20->x;
    xz->z = local_20->z;
    m0400 = cXyz_abs(xz.get());
    m040C = cM_atan2f(local_20->x, local_20->z);
    dCamera_SetTypeForce(dCam_getBody(), gabi::guest_addr("Tornado"), mTornadoActor);
    f32 d = (6000.0f /* l_HIO.tornado_distance */ - m0400) * 0.0004f;
    fopAc_ac_c* tornado = mTornadoActor;
    if (d < 0.0f) {
        d = 0.0f;
    }
    m0404 = d;
    f32 dx = tornado_getJointPos(tornado, 0, 0) - current.pos.x;
    f32 dz = tornado_getJointPos(tornado, 0, 2) - current.pos.z;
    f32 distXZ = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    if (!checkStateFlg(daSFLG_UNK1000_e) && distXZ < 3500.0f) {
        if (daPy_shipSpecialDemoStart(dComIfGp_getLinkPlayer())) {
            procTornadoUp_init();
        }
    }
}



/* 02472F14 */
void daShip_c::setWhirlActor() {
    mWhirlActor = fopAcM_SearchByID(mWhirlID);
    if (!mWhirlActor) {
        mWhirlID = fpcM_ERROR_PROCESS_ID_e;
        mWhirlActor = nullptr;
        return;
    }
    gabi::Local<cXyz> local_20;
    cXyz_mi(&current.pos, local_20.get(), &mWhirlActor->current.pos);
    gabi::Local<cXyz> xz;
    xz->y = 0.0f;
    xz->x = local_20->x;
    xz->z = local_20->z;
    m0400 = cXyz_abs(xz.get());
    m040C = cM_atan2f(local_20->x, local_20->z);
    dCamera_SetTypeForce(dCam_getBody(), gabi::guest_addr("Tornado"), mWhirlActor);
    f32 d = (4000.0f /* l_HIO.whirl_distance */ - m0400) * (1.0f / 3500.0f);
    if (d < 0.0f) {
        d = 0.0f;
    }
    m0404 = d;
    if (m0400 < 500.0f && !checkStateFlg(daSFLG_UNK1000_e)) {
        if (daPy_shipSpecialDemoStart(dComIfGp_getLinkPlayer())) {
            procWhirlDown_init();
        }
    }
}



/* 02475DA8 */
void daShip_c::setHeadAnm() {
    s32 newFileIndex = -1;
    if (ship_play()->mEventRun != 0) { /* dComIfGp_event_runCheck() */
        if (mEvtStaffId == -1 &&
            (shipHD_startCheckOld( dComIfGp_ea() + 0x52C4, gabi::guest_addr("SV_TALK_P1_1ST")) ||
             shipHD_startCheckOld( dComIfGp_ea() + 0x52C4, gabi::guest_addr("SV_TALK_P1_2ND")) ||
             shipHD_startCheckOld( dComIfGp_ea() + 0x52C4, gabi::guest_addr("SV_TALK_P4_1ST")))) {
            newFileIndex = dRes_INDEX_SHIP_BCK_KYAKKAN1_e;
        } else if (m03B4 == dRes_INDEX_SHIP_BCK_KYAKKAN1_e || m03B4 == dRes_INDEX_SHIP_BCK_DAMAGE1_e) {
            newFileIndex = dRes_INDEX_SHIP_BCK_FN_LOOK_L_e;
        }
    } else if (ship_playerStatus0() & daPyStts0_SHIP_RIDE_e) {
        if ((ship_playerStatus0() & 0x287000 /* bow, subject, hookshot, boomerang aim, telescope */) ||
            (ship_play()->mPlayerStatus1 & 8 /* daPyStts1_PICTO_BOX_AIM_e */) || mCurMode == MODE_CANNON_e) {
            newFileIndex = dRes_INDEX_SHIP_BCK_KYAKKAN1_e;
        } else if ((!dComIfGs_isEventBit(ShipEvent::UNK_3910) || dComIfGs_isEventBit(ShipEvent::ZELDA_AWAKENED)) &&
                   (mSph.ChkTgHit() || mCyl[0].ChkTgHit() || mCyl[1].ChkTgHit() || mCyl[2].ChkTgHit())) {
            m03B4 = dRes_INDEX_SHIP_BCK_FN_LOOK_L_e;
            newFileIndex = dRes_INDEX_SHIP_BCK_DAMAGE1_e;
        } else if (m03B4 != dRes_INDEX_SHIP_BCK_DAMAGE1_e || std::fabs((f32)mpHeadAnm->getPlaySpeed()) < 0.01f) {
            newFileIndex = dRes_INDEX_SHIP_BCK_FN_LOOK_L_e;
        }
    } else {
        if ((!dComIfGs_isEventBit(ShipEvent::UNK_3910) || dComIfGs_isEventBit(ShipEvent::ZELDA_AWAKENED)) &&
            (mSph.ChkTgHit() || mCyl[0].ChkTgHit() || mCyl[1].ChkTgHit() || mCyl[2].ChkTgHit())) {
            m03B4 = dRes_INDEX_SHIP_BCK_FN_LOOK_L_e;
            newFileIndex = dRes_INDEX_SHIP_BCK_DAMAGE1_e;
        } else if (m03B4 == dRes_INDEX_SHIP_BCK_DAMAGE1_e || m03B4 == dRes_INDEX_SHIP_BCK_AKIBI1_e) {
            if (mpHeadAnm->getPlaySpeed() < 0.01f) {
                newFileIndex = dRes_INDEX_SHIP_BCK_FN_LOOK_L_e;
            }
        } else if ((m03B4 == dRes_INDEX_SHIP_BCK_FN_LOOK_L_e || m03B4 == dRes_INDEX_SHIP_BCK_FN_LOOK_R_e) &&
                   std::fabs((f32)mpHeadAnm->getPlaySpeed()) < 0.01f && cM_rnd() < 0.4f &&
                   (ship_counter & 0x1FF) == 0x1FF /* g_Counter.mTimer */ &&
                   (!dComIfGs_isEventBit(ShipEvent::UNK_3910) || dComIfGs_isEventBit(ShipEvent::ZELDA_AWAKENED)) && !checkStateFlg(daSFLG_UNK40000000_e)) {
            newFileIndex = dRes_INDEX_SHIP_BCK_AKIBI1_e;
        } else if (m03B4 != dRes_INDEX_SHIP_BCK_FN_LOOK_R_e) {
            newFileIndex = dRes_INDEX_SHIP_BCK_FN_LOOK_L_e;
        }
    }

    if (m03B4 != newFileIndex && newFileIndex != -1) {
        f32 speed, morph;
        if (newFileIndex == dRes_INDEX_SHIP_BCK_AKIBI1_e) {
            speed = 1.0f;
            morph = 5.0f;
        } else if (newFileIndex == dRes_INDEX_SHIP_BCK_DAMAGE1_e) {
            speed = 1.0f;
            morph = 0.0f;
        } else {
            speed = 0.0f;
            morph = 5.0f;
        }
        m03B4 = newFileIndex;
        J3DAnmTransform* res = ship_getRes(m03B4);
        mpHeadAnm->setAnm(res, 0, morph, speed, 0.0f, -1.0f, nullptr);
    }
}



/* 0247AE78 */
static BOOL daShip_Execute(daShip_c* i_this) {
    return i_this->execute();
}

// HD: cannon sight and water-shadow additions to this translation unit.


// HD: TRUE in the Tower of the Gods ("Siren") from room 7 on, where the boat is not * drawn (probably: the boat waits outside while Link is inside the tower)
/* 02471E94 */
BOOL daShip_checkSirenInside() {
    if (ship_isStartStage(gabi::guest_addr("Siren")) && dComIfGp_getLinkPlayer()->current.roomNo >= 7) {
        return TRUE;
    }
    return FALSE;
}



// HD: Draw of the cannon-sight packet (mHDPacket, a J3DPacket subclass): with its model * (+0x98) and both buffers (+0x9C, +0xA0) set, updates the 25 sight segments (0xA8 bytes each), * enters the packet into the opaque draw buffer and draws the model
/* 02471FB0 */
static void daShip_sightPacket_draw(u8* pkt) {
    ShipSightPacket* packet = reinterpret_cast<ShipSightPacket*>(pkt);
    if (packet->mModel == 0 || packet->mModelPacket == 0 || packet->mSegments == 0)
        return;
    for (int i = 0; i < 25; i++) {
        shipHD_updateSightSegment( packet->mSegments + i * 0xA8);
    }
    shipHD_entryImmediate( (u32)ship_drawBuffer, pkt, 0);
    shipHD_drawSightModel( (u32)packet->mModel);
}



// HD: Draws the shadow model (mpShadowModel) under the boat: placed on the highest of * five wave heights around the hull, tilted with the waves, faded out with height and speed and * tinted with the sea colour (replaces the GameCube real shadow).
/* 024720D0 */
void daShip_c::drawShadow() {
    /* static cXyz l_shadow_pos[5] = {{0,0,0}, {0,0,200}, {0,0,-200}, {-120,0,0}, {120,0,0}}; (guard 1046DDA0) */

    if (shadowPosGuard == 0) {
        const f32 init[15] = {0, 0, 0, 0, 0, 200, 0, 0, -200, -120, 0, 0, 120, 0, 0};
        for (int i = 0; i < 15; i++)
            reinterpret_cast<be<f32>*>(&shadowPositions.mPositions[i / 3])[i % 3] = init[i];
        shadowPosGuard = 1;
    }
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    gabi::Local<cXyz[5]> pos;
    for (int i = 0; i < 5; i++) {
        PSMTXMultVec(mDoMtx_now(), &shadowPositions.mPositions[i], &(*pos)[i]);
        getMaxWaterY(&(*pos)[i]);
    }
    gabi::Local<cXyz> fb;
    cXyz_mi(&(*pos)[2], fb.get(), &(*pos)[1]);
    gabi::Local<cXyz> lr;
    cXyz_mi(&(*pos)[4], lr.get(), &(*pos)[3]);
    gabi::Local<cXyz> xz1;
    xz1->x = fb->x;
    xz1->y = 0.0f;
    xz1->z = fb->z;
    s16 angX = cM_atan2s(fb->y, std_sqrtf(PSVECSquareMag(xz1.get())));
    gabi::Local<cXyz> xz2;
    xz2->x = lr->x;
    xz2->y = 0.0f;
    xz2->z = lr->z;
    s16 angZ = cM_atan2s(lr->y, std_sqrtf(PSVECSquareMag(xz2.get())));
    f32 yLR = gabi::fmadds(lr->y, 0.5f, (*pos)[3].y);
    f32 yFB = gabi::fmadds(fb->y, 0.5f, (*pos)[1].y);
    f32 y = yLR - yFB >= 0.0f ? yLR : yFB;
    if (y < (*pos)[0].y)
        y = (*pos)[0].y;
    mDoMtx_stack_c::transS(current.pos.x, y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_now(), angX, shape_angle.y, angZ);
    mDoMtx_stack_c::transM(0.0f, 3.0f, 0.0f);
    J3DModel_setBaseTRMtx(mpShadowModel, mDoMtx_now());

    f32 h = current.pos.y - y;
    f32 spd = speedF;
    spd = spd >= 0.0f ? spd : 0.0f;
    h = h >= 0.0f ? h : 0.0f;
    s32 alpha = gabi::ftoi(255.0f - gabi::fmadds(spd, 10.0f, h));
    if (alpha < 0)
        alpha = 0;
    cLib_chaseUC(&mShadowAlpha, (u8)alpha, 10);
    if (mShadowAlpha != 0) {
        J3DModelData* modelData = J3DModel_getModelData(mpShadowModel);
        u32 mat = modelData->mMaterials;
        gabi::Local<GXColor> amb;
        gabi::Local<GXColor> dif;
        dKy_get_seacolor(amb.get(), dif.get());
        gabi::Local<be<s16>[4]> color; /* GXColorS10 */
        (*color)[1] = amb->g;
        (*color)[0] = amb->r;
        (*color)[3] = mShadowAlpha;
        (*color)[2] = amb->b;
        f32 k = reinterpret_cast<ShipEnvironment*>(dKy_getEnvlight())->mSeaColorScale;
        /* material->getTevBlock()->setTevColor(1, color) (virtual +0x24) */
        u32 tev = gabi::at<ShipShadowMaterial>(mat)->mTevBlock;
        gabi::call_ptr(gabi::at<ShipTevVtable>(gabi::at<ShipTevBlock>(tev)->__vtbl)->mSetTevColor, tev, 1, color.get());
        gabi::Local<be<f32>[4]> c4;
        (*c4)[0] = (f32)(s16)(*color)[0] / 255.0f;
        (*c4)[1] = (f32)(s16)(*color)[1] / 255.0f;
        (*c4)[2] = (f32)(s16)(*color)[2] / 255.0f;
        (*c4)[3] = (f32)(s16)(*color)[3] / 255.0f;
        gabi::Local<be<f32>[4]> out;
        shipHD_convertSeaColor( out.get(), c4.get(), k);
        gabi::at<ShipShadowMaterial>(mat)->mShaderFlags = gabi::at<ShipShadowMaterial>(mat)->mShaderFlags | 0x20;
        u32 dst = shipHD_getShaderColor( mat + 0xA0, 5);
        gabi::at<ShipFloatColor>(dst)->g = (*out)[1];
        gabi::at<ShipFloatColor>(dst)->b = (*out)[2];
        gabi::at<ShipFloatColor>(dst)->r = (*out)[0];
        gabi::at<ShipFloatColor>(dst)->a = (f32)(s16)(*color)[3] / 255.0f;
        mDoExt_btkAnm_entry((mDoExt_btkAnm*)&mShadowBtk, modelData, mShadowBtk.mFrameCtrl.getFrame());
        mDoExt_modelUpdateDL(mpShadowModel);
    }
}



// HD: Copies a global matrix (0x104A03EC) into the 25 cannon-sight segments (+0x74 each)
/* 0247700C */
void daShip_sightPacket_setMtx(u8* pkt) {
    ShipSightPacket* packet = reinterpret_cast<ShipSightPacket*>(pkt);
    if (packet->mSegments == 0)
        return;
    for (int i = 0; i < 25; i++) {
        PSMTXCopy_s(&ship_sightMtx, &gabi::at<ShipSightSegment>(packet->mSegments)[i].mMtx);
    }
}



// HD: Matrix copy through single-precision registers
/* 02477188 */
static void daShip_mtxCopyF(Mtx34* dst, const Mtx34* src) {
    f32 t[12];
    for (int i = 0; i < 12; i++)
        t[i] = src->m[i / 4][i % 4];
    for (int i = 0; i < 12; i++)
        dst->m[i / 4][i % 4] = t[i];
}



// HD: Sets the matrix of cannon-sight segment idx
/* 02477228 */
void daShip_sightPacket_setSegMtx(u8* pkt, s32 idx, Mtx34* mtx) {
    gabi::Local<Mtx34> tmp;
    daShip_mtxCopyF(tmp.get(), mtx);
    PSMTXCopy_s(tmp.get(), &gabi::at<ShipSightSegment>(reinterpret_cast<ShipSightPacket*>(pkt)->mSegments)[idx].mMtx);
}



// HD: One step of the cannon-sight projectile (mSightState: angle +2, position +8, velocity +0x14, speed +0x20, gravity +0x24, delay +0x28, lowest vertical speed +0x2C)
/* 02477088 */
void daShip_c::sightStep() {
    ShipSight* sight = reinterpret_cast<ShipSight*>(mSightState);
    s16 t = sight->mDelay;
    f32 spd = sight->mSpeed;
    f32 grav = sight->mGravity;
    bool delayed = t > 0;
    u16 ang = sight->mAngle.y;
    f32 vy;
    if (!delayed) {
        vy = sight->mVelocity.y + grav;
    } else {
        sight->mGravity = 0.0f;
        vy = sight->mVelocity.y + 0.0f;
    }
    f32 vx = spd * cM_ssin(ang);
    f32 vz = spd * cM_scos(ang);
    f32 vmin = sight->mMinVelocity;
    if (vy < vmin) {
        vy = vmin;
    }
    sight->mVelocity.x = vx;
    sight->mVelocity.y = vy;
    sight->mVelocity.z = vz;
    sight->mPosition.x = sight->mPosition.x + vx;
    sight->mPosition.y = sight->mPosition.y + sight->mVelocity.y;
    sight->mPosition.z = sight->mPosition.z + sight->mVelocity.z;
    if (delayed) {
        sight->mGravity = grav;
        sight->mDelay = sight->mDelay - 1;
    }
}



// HD: The water shadow's texture scrolls faster with speed and tilt
/* 02477944 */
void daShip_c::setShadowBtkRate() {
    s16 ax = shape_angle.x, az = shape_angle.z;
    s32 tilt = (ax < 0 ? -ax : ax) + (az < 0 ? -az : az);
    f32 target = gabi::fmadds((f32)tilt, 0.002f, gabi::fmadds(speedF, 0.5f, 0.5f));
    f32 rate = mShadowBtk.mFrameCtrl.mRate;
    if (!(target < 1.0f)) {
        target = target - 3.0f >= 0.0f ? 3.0f : target;
    } else {
        target = 1.0f;
    }
    gabi::Local<be<f32>> tmp;
    *tmp = rate;
    cLib_addCalc(tmp.get(), target, 0.02f, 0.4f, 0.02f);
    mShadowBtk.mFrameCtrl.mRate = (f32)*tmp;
    mDoExt_baseAnm_play(&mShadowBtk);
}

// Remaining entries.


/* 0247D65C */
f32 daShip_c::decrementShipSpeed(f32 decrementSpeed) {
    /* HD: scale 0.1 (GameCube 0.05); the step is 0.1 while slowing down and 0.4 while speeding up */
    return cLib_addCalc(&speedF, decrementSpeed, 0.1f, speedF - decrementSpeed >= 0.0f ? 0.1f : 0.4f, 0.015f);
}



/* 02481038 */
BOOL daShip_c::setNextMessage(msg_class* msg) {
    /* HD: the selected answer comes from the message manager (*0x101F4B5C, +0x948), not from msg */
    u32 msgMng = ship_msgMng();
    u32 currMessageNo = mNextMessageNo;
    if (currMessageNo != 0x5dd && currMessageNo != 0x5de && currMessageNo != 0x5df &&
        currMessageNo != 0x5eb && currMessageNo != 0x5ee && currMessageNo != 0x5ef &&
        currMessageNo != 0x5f5 && currMessageNo != 0x5f9 &&
        currMessageNo != 0x608 && currMessageNo != 0x609 && currMessageNo != 0x60a &&
        currMessageNo != 0x621 && currMessageNo != 0x623 && currMessageNo != 0x636 &&
        (currMessageNo < 0xd5c || currMessageNo > 0xd78) &&
        currMessageNo != 0xd7b && currMessageNo != 0x164c &&
        currMessageNo != 0x1650 && currMessageNo != 0x1651 &&
        currMessageNo != 0x1684 && currMessageNo != 0x1685 &&
        currMessageNo != 0x1687 && currMessageNo != 0x1689) {
        if (currMessageNo == 0x5e4) {
            dComIfGs_onEventBit(0x0908);
        } else if (currMessageNo == 0x5ea) {
            dComIfGs_onEventBit(0x0A10);
        } else if (currMessageNo == 0x5ed) {
            dComIfGs_onEventBit(0x0A80);
        } else if (currMessageNo == 0x5f4) {
            dComIfGs_onEventBit(0x2B80);
        } else if (currMessageNo == 0x5f7) {
            dComIfGs_onEventBit(0x0A08);
        } else if (currMessageNo == 0x5f8) {
            dComIfGs_onEventBit(0x0A04);
        } else if (currMessageNo == 0x607) {
            dComIfGs_onEventBit(0x0A01);
        } else if (currMessageNo == 0x622) {
            dComIfGs_onEventBit(0x1F01);
        } else if (currMessageNo == 0x624) {
            dComIfGs_onEventBit(0x1F02);
        } else if (currMessageNo == 0xd5b) {
            dComIfGs_onEventBit(0x2F20);
        } else if (currMessageNo == 0x1682) {
            dComIfGs_onEventBit(0x3D04);
            mNextMessageNo = 0;
        } else if (currMessageNo == 0x1688) {
            dComIfGs_onEventBit(0x3F80);
        } else if (currMessageNo == 0x168c) {
            dComIfGs_onEventBit(0x3840);
        } else {
            if (currMessageNo == 0x5e0) {
                mNextMessageNo = 0x5e1;
            } else if (currMessageNo == 0x5e1) {
                mNextMessageNo = 0x5e2;
            } else if (currMessageNo == 0x5e2) {
                if (shipTail::selectedAnswer(msgMng) == 0) {
                    mNextMessageNo = 0x5e4;
                } else {
                    mNextMessageNo = 0x5e3;
                }
            } else if (currMessageNo == 0x5e3) {
                mNextMessageNo = 0x5e1;
            } else if (currMessageNo == 0x5ec) {
                mNextMessageNo = 0x5ed;
            } else if (currMessageNo == 0x5f6) {
                mNextMessageNo = 0x5f7;
            } else if (currMessageNo == 0xd5a) {
                mNextMessageNo = 0xd5b;
            } else if (currMessageNo == 0x1645) {
                mNextMessageNo = 0x1646;
            } else if (currMessageNo == 0x1646) {
                mNextMessageNo = 0x1647;
            } else if (currMessageNo == 0x1647) {
                mNextMessageNo = 0x1648;
            } else if (currMessageNo == 0x1648) {
                mNextMessageNo = 0x1649;
            } else if (currMessageNo == 0x1649) {
                mNextMessageNo = 0x164a;
            } else if (currMessageNo == 0x164a) {
                mNextMessageNo = 0x164b;
            } else if (currMessageNo == 0x164b) {
                dComIfGs_onEventBit(0x3201);
                mNextMessageNo = 0x164c;
            } else if (currMessageNo == 0x164d) {
                mNextMessageNo = 0x164e;
            } else if (currMessageNo == 0x164e) {
                mNextMessageNo = 0x164f;
            } else if (currMessageNo == 0x164f) {
                dComIfGs_onEventBit(0x3380);
                if (dComIfGs_getTriforceNum() > 0) {
                    mNextMessageNo = 0x1650;
                } else {
                    mNextMessageNo = 0x1651;
                }
            } else if (currMessageNo == 0x1683) {
                dComIfGs_onEventBit(0x3E20);
                if (dComIfGs_getItem(12) != 0xFF /* dComIfGs_getItem(12) != dItemNo_NONE_e */) {
                    mNextMessageNo = 0x1685;
                } else {
                    mNextMessageNo = 0x1684;
                }
            }
            return FALSE;
        }
    }
    return TRUE;
}



/* 02480FD4 */
BOOL daShip_c::procTalkReady() {
    firstDecrementShipSpeed(0.0f);
    setMoveAngle(0);
    if (!(mpHeadAnm->getPlaySpeed() > 0.1f)) {
        procTalk_init();
    }
    return TRUE;
}



/* 02481630 */
BOOL daShip_c::procTalk() {
    /* HD: one message manager instead of message processes */
    u32 mng = ship_msgMng();
    firstDecrementShipSpeed(0.0f);
    setMoveAngle(0);
    if (m0430 == fpcM_ERROR_PROCESS_ID_e) {
        if (dComIfGp_checkCameraAttentionStatus(dComIfGp_getPlayerCameraID0(), 4)) {
            m0430 = fopMsgM_messageSet(mng, mNextMessageNo, &eyePos);
        }
    } else if (msgMng_getStatus(mng) == fopMsgStts_MSG_DISPLAYED_e) {
        /* HD: msg is not passed (r4 holds whatever the status query left there) */
        if (shipTailNextMessage(this)) {
            msgMng_setStatus(mng, fopMsgStts_MSG_ENDS_e);
        } else {
            msgMng_setStatus(mng, fopMsgStts_MSG_CONTINUES_e);
            fopMsgM_messageSet(mng, mNextMessageNo, nullptr);
        }
    } else if (msgMng_getStatus(mng) == fopMsgStts_BOX_CLOSED_e) {
        msgMng_setStatus(mng, fopMsgStts_MSG_DESTROYED_e);
        if ((ship_playerStatus0() & daPyStts0_SHIP_RIDE_e) && checkStateFlg(daSFLG_UNK400000_e)) {
            return procTurn_init();
        }
        mpHeadAnm->setPlaySpeed(-1.0f);
        dComIfGp_event_reset();
        changeDemoEndProc();
        return TRUE;
    }
    if (mFrameCtrl.checkState(2)) {
        u16 fileIndex;
        if (cM_rndF(2.0f) < 1.0f) {
            fileIndex = dRes_INDEX_SHIP_BCK_FN_TALK_A_e;
        } else {
            fileIndex = dRes_INDEX_SHIP_BCK_FN_TALK_B_e;
        }
        if (fileIndex != m038A) {
            m038A = fileIndex;
            mAnmTransform = ship_getRes(fileIndex);
            J3DFrameCtrl_init(&mFrameCtrl, anm_getFrameMax(mAnmTransform));
        }
    }
    return TRUE;
}



/* 024814F8 */
BOOL daShip_c::procTurn_init() {
    m0430 = fpcM_ERROR_PROCESS_ID_e;
    mpHeadAnm->setPlaySpeed(-1.0f);
    seStart(shipTail::JA_SE_SHIP_LOOK_FORWARD, &eyePos);
    setProc(shipTail::procTurn);
    m038E = 0;
    u32 camera = ship_getCameraAddress(dComIfGp_getPlayerCameraID0());
    camera_Stop(camera);
    gabi::Local<cXyz> eye;
    camera_Eye(camera, eye.get());
    gabi::Local<cXyz> center;
    camera_Center(camera, center.get());
    gabi::Local<cXyz> cameraPos;
    cXyz_mi(eye.get(), cameraPos.get(), center.get());
    gabi::Local<cXyz> nrm;
    cXyz_normalize(cameraPos.get(), nrm.get());
    gabi::Local<cXyz> center2;
    camera_Center(camera, center2.get());
    gabi::Local<cXyz> ofs;
    cXyz_ml(cameraPos.get(), ofs.get(), 1600.0f);
    gabi::Local<cXyz> res;
    cXyz_pl(center2.get(), res.get(), ofs.get());
    m037A = 10;
    m0450.copy(*res);
    offStateFlg(daSFLG_UNK8000_e);
    return TRUE;
}



/* 0247D4B0 */
void daShip_c::setMoveAngle(s16 moveAngle) {
    if (!mTornadoActor && !mWhirlActor) { /* !checkForceMove() */
        /* HD: the turn rate falls off up to speed 100 (GameCube 55) */
        f32 turnRate = 4.0f - (std::fabs((f32)speedF) / 100.0f) * 3.0f;
        s16 initialAngle = shape_angle.y;
        if (turnRate > 3.6f) {
            turnRate = 3.6f;
        } else if (turnRate < 0.1f) {
            turnRate = 0.1f;
        }
        if (m0388 >= 0xF) {
            turnRate = 3.6f;
        } else {
            turnRate += ((3.6f - turnRate) * m0388) / 15.0f;
        }
        s16 temp = (s16)gabi::ftoi(-(turnRate * (f32)(moveAngle >> 6)));
        shape_angle.y += temp;
        setControllAngle(getAimControllAngle(initialAngle));
        current.angle.y = shape_angle.y;
    }
}



/* 0247D760 */
void daShip_c::changeDemoEndProc() {
    offStateFlg(daSFLG_UNK8000_e);
    if (checkStateFlg(daSFLG_UNK80000_e)) {
        mpHeadAnm->setPlaySpeed(-1.0f);
        offStateFlg(daSFLG_UNK80000_e);
    }
    if (m0351 == shipTail::DEMO_OPEN_e || checkStateFlg(daSFLG_UNK4000000_e)) {
        m0366 = 0;
        offStateFlg(daSFLG_UNK4000000_e);
    }
    gravity = -2.5f;

    if (ship_playerStatus0() & daPyStts0_SHIP_RIDE_e) {
        if (mPart == PART_WAIT_e) {
            procPaddleMove_init();
        } else if (mPart == PART_CANNON_e) {
            procCannonReady_init();
        } else if (mPart == PART_CRANE_e) {
            procCraneReady_init();
        } else {
            procSteerMove_init();
        }
    } else {
        procWait_init();
    }
}



/* 02481874 */
BOOL daShip_c::procTurn() {
    u32 camera = ship_getCameraAddress(dComIfGp_getPlayerCameraID0());
    gabi::Local<cXyz> center;
    camera_Center(camera, center.get());
    gabi::Local<cXyz> local_c4;
    cXyz_mi(&current.pos, local_c4.get(), center.get());
    cXyz_abs(local_c4.get()); /* fVar5, unused */
    gabi::Local<cXyz> center2;
    camera_Center(camera, center2.get());
    gabi::Local<cXyz> d;
    cXyz_mi(&current.pos, d.get(), &m045C);
    gabi::Local<cXyz> local_ac;
    cXyz_pl(center2.get(), local_ac.get(), d.get());
    f32 acx = local_ac->x, acy = local_ac->y, acz = local_ac->z;
    gabi::Local<cXyz> eye;
    camera_Eye(camera, eye.get());
    gabi::Local<cXyz> d2;
    cXyz_mi(&m0450, d2.get(), eye.get());
    local_c4->copy(*d2);
    f32 eyeDistance = cXyz_abs(local_c4.get());
    gabi::Local<cXyz> setCenter;
    gabi::Local<cXyz> setEye;
    if (eyeDistance > 1.0f) {
        f32 eyeStep = eyeDistance * 0.5f;
        if (eyeStep > 60.0f) {
            eyeStep = 60.0f;
        } else if (eyeStep < 15.0f) {
            eyeStep = 15.0f - eyeDistance >= 0.0f ? eyeDistance : 15.0f;
        }
        gabi::Local<cXyz> eye2;
        camera_Eye(camera, eye2.get());
        gabi::Local<cXyz> nrm;
        cXyz_normalize(local_c4.get(), nrm.get());
        gabi::Local<cXyz> step;
        cXyz_ml(nrm.get(), step.get(), eyeStep);
        gabi::Local<cXyz> local_b8;
        cXyz_pl(eye2.get(), local_b8.get(), step.get());
        setCenter->set(acx, acy, acz);
        setEye->set(local_b8->x, local_b8->y, local_b8->z);
    } else {
        setCenter->set(acx, acy, acz);
        setEye->set(m0450.x, m0450.y, m0450.z);
    }
    camera_Set(camera, setCenter.get(), setEye.get());
    if (!(eyeDistance > 1.0f)) {
        s16 previousAngle = shape_angle.y;
        cLib_addCalcAngleS2(&shape_angle.y, m038C, 8, m038E);
        cLib_addCalcAngleS2(&m038E, 0x800, 8, 0x80);
        current.angle.y = shape_angle.y;
        setControllAngle(getAimControllAngle(previousAngle));
        s16 diff = shape_angle.y - m038C;
        if ((diff < 0 ? -diff : diff) < 0x200) {
            /* HD: towards speed 100 (GameCube 55) */
            cLib_addCalc(&speedF, 100.0f, 0.1f, 5.0f, 1.0f);
        }
        s16 diff2 = m038C - shape_angle.y;
        if ((diff2 < 0 ? -diff2 : diff2) < 0x100 && !checkOutRange()) {
            if (m037A <= 0) {
                camera_Start(camera);
                camera_Reset(camera);
                dComIfGp_event_reset();
                changeDemoEndProc();
                m038E = 0;
                onStateFlg(daSFLG_UNK100000_e);
            } else {
                m037A--;
            }
        }
    }
    return TRUE;
}



/* 0247D398 */
void daShip_c::setSailAngle() {
    s16 sVar1;
    cXyz* windVec = dKyw_get_wind_vec();
    if (mTornadoActor) {
        sVar1 = (s16)(m03AA + 0x4000) - shape_angle.y;
    } else {
        sVar1 = cM_atan2s(windVec->x, windVec->z) - shape_angle.y;
    }
    if (sVar1 > 0x800) {
        m0380 = -0x1555;
    } else if (sVar1 < -0x800) {
        m0380 = 0x1555;
    } else if (m0380 == 0) {
        if (sVar1 >= 0) {
            m0380 = -0x1555;
        } else {
            m0380 = 0x1555;
        }
    }
    cLib_addCalcAngleS(&mSailAngle, m0380, 0x10, 0x800, 0x20);
    m0378 = mSailAngle >> 1;
    if (mSailAngle * m0380 < 0) {
        if (!checkStateFlg(daSFLG_UNK2_e)) {
            seStart(shipFirst::JA_SE_SHIP_SAIL_MOVE, &m0444);
            onStateFlg(daSFLG_UNK2_e);
        }
    } else {
        offStateFlg(daSFLG_UNK2_e);
    }
}



/* 0247D880 */
BOOL daShip_c::setCrashData(s16 param1) {
    /* HD: no crash while the timer runs (GameCube: counted down here), and the result says whether
     * Link fell off the boat */
    if (m03B6 != 0 || dComIfGp_event_runCheck() != 0 /* dComIfGp_event_runCheck() */) {
        return FALSE;
    }
    BOOL droppedLink = FALSE;
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    if ((shipFirst::linkNoResetFlags(gabi::ea(link)) & 1) || shipFirst::linkDamageWait(gabi::ea(link)) != 0) {
        /* HD: Link takes the collider's damage (probably while he cannot be thrown off) */
        u32 l = gabi::ea(dComIfGp_getLinkPlayer());
        shipFirstSetDamageRupee(l, (f32)-(s32)mStts.m_dmg);
    } else if (gabi::ea(dComIfGp_getPlayer(0)) == gabi::ea(link) && !mTornadoActor && !mWhirlActor) {
        u32 crash = checkStateFlg(daSFLG_UNK4_e);
        u32 l = gabi::ea(dComIfGp_getLinkPlayer());
        if (!crash) {
            /* HD: a plain hit damages Link */
            shipFirstSetDamagePointWait(l, (f32)-(s32)mStts.m_dmg);
        } else {
            /* daPy_getPlayerLinkActorClass()->onShipDrop(param1) */
            shipFirst::setLinkDropAngle(l, param1);
            shipFirst::setLinkResetFlags(l, shipFirst::linkResetFlags(l) | shipFirst::LINK_SHIP_DROP);
            procWait_init();
            droppedLink = TRUE;
        }
    }
    onStateFlg(daSFLG_UNK8000000_e);
    /* HD: speed limit 100 (GameCube wind_inc_speed 55) */
    f32 spd = speedF;
    if (spd > 100.0f) {
        spd = 100.0f;
    }
    speedF = spd * 0.7f;
    s16 iVar5 = param1 - shape_angle.y;
    f32 fVar1 = cM_ssin(iVar5);
    f32 fVar2 = cM_scos(iVar5);
    m0370 = (s16)gabi::ftoi(gabi::fmadds(3072.0f, fVar2, (f32)(s16)m0370));
    m0374 = (s16)gabi::ftoi(gabi::fmadds(500.0f, fVar2, (f32)(s16)m0374));
    m0372 = (s16)gabi::ftoi(gabi::fnmsubs(3072.0f, fVar1, (f32)(s16)m0372));
    m0376 = (s16)gabi::ftoi(gabi::fnmsubs(500.0f, fVar1, (f32)(s16)m0376));
    m03B6 = 30;
    return droppedLink;
}



/* 0247DD90 */
BOOL daShip_c::checkNextMode(int i_curMode) {
    if (checkStateFlg(daSFLG_UNK4_e) != 0) {
        setCrashData(shape_angle.y + 0x8000);
        offStateFlg(daSFLG_UNK4_e);
        return TRUE;
    }

    if (!(ship_playerStatus0() & daPyStts0_SHIP_RIDE_e)) {
        if (mNextMode == MODE_GET_OFF_FIRST_e) {
            procGetOff_init();
        } else {
            procWait_init();
        }
        return TRUE;
    }

    if (dComIfGp_event_runCheck() == 0) { /* !dComIfGp_event_runCheck() */
        cXyz* tgRVecP = nullptr;
        cXyz* tgHitPos = nullptr;
        u32 tgHitObj = 0;

        if (mSph.ChkTgHit()) {
            tgRVecP = &mSph.mGObjTg.mRVec;
            tgHitPos = &mSph.mGObjTg.mHitPos;
            tgHitObj = shipFirstGetTgHitGObj(&mSph);
        } else {
            dCcD_Cyl* cyl = mCyl;
            for (int i = 0; i < 3; i++, cyl++) {
                if (cyl->ChkTgHit()) {
                    tgRVecP = &cyl->mGObjTg.mRVec;
                    tgHitPos = &cyl->mGObjTg.mHitPos;
                    tgHitObj = shipFirstGetTgHitGObj(cyl);
                }
            }
        }

        if (tgHitObj && shipFirst::attackType(tgHitObj) == shipFirst::AT_TYPE_WIND) {
            if (ship_playerStatus0() & daPyStts0_SHIP_RIDE_e) {
                onStateFlg(daSFLG_FLY_e | daSFLG_UNK2000000_e);
            }
        } else if (tgRVecP) {
            s16 sVar2;
            gabi::Local<cXyz> xz;
            xz->x = tgRVecP->x;
            xz->y = 0.0f;
            xz->z = tgRVecP->z;
            if (PSVECSquareMag(xz.get()) < 0.1f) {
                gabi::Local<cXyz> local_20;
                cXyz_mi(&current.pos, local_20.get(), tgHitPos);
                gabi::Local<cXyz> xz2;
                xz2->x = local_20->x;
                xz2->y = 0.0f;
                xz2->z = local_20->z;
                if (PSVECSquareMag(xz2.get()) < 0.1f) {
                    sVar2 = shape_angle.y + 0x8000;
                } else {
                    sVar2 = cM_atan2s(local_20->x, local_20->z);
                }
            } else {
                sVar2 = cM_atan2s(tgRVecP->x, tgRVecP->z);
            }
            /* HD: only a crash that threw Link off ends the check */
            if (setCrashData(sVar2)) {
                return TRUE;
            }
        }
    }

    if (i_curMode == mNextMode) {
        return FALSE;
    } else if (mNextMode == MODE_PADDLE_MOVE_e) {
        procPaddleMove_init();
    } else if (mNextMode == MODE_STEER_MOVE_e) {
        procSteerMove_init();
    } else if (mNextMode == MODE_CANNON_e) {
        procCannonReady_init();
    } else if (mNextMode == MODE_CRANE_e) {
        procCraneReady_init();
    } else if (mNextMode == MODE_TACT_WARP_e) {
        procTactWarp_init();
    } else {
        return FALSE;
    }
    return TRUE;
}



/* 0247BA38 */
void daShip_c::setPartOffAnime() {
    J3DAnmTransform* pAnimRes = ship_getRes(dRes_INDEX_SHIP_BCK_FN_MAST_OFF2_e);
    mpBodyAnm->setAnm(pAnimRes, 0, 3.0f, 1.0f, 0.0f, -1.0f, nullptr);
    if (mPart == PART_CRANE_e) {
        seStart(shipFirst::JA_SE_LK_SHIP_CRANE_IN, &m0444);
    } else if (mPart == PART_CANNON_e) {
        seStart(shipFirst::JA_SE_LK_SHIP_CANNON_IN, &m0444);
    } else {
        seStart(shipFirst::JA_SE_SHIP_SAIL_IN, &m0444);
    }
    m0392 = dRes_INDEX_SHIP_BCK_FN_MAST_OFF2_e;
    offStateFlg(daSFLG_SAIL_ON_e);
}



/* 0247BB24 */
void daShip_c::setPartOnAnime(u8 i_part) {
    J3DAnmTransform* pAnimRes = ship_getRes(dRes_INDEX_SHIP_BCK_FN_MAST_ON2_e);
    mpBodyAnm->setAnm(pAnimRes, 0, 3.0f, 1.0f, 0.0f, -1.0f, nullptr);
    m0392 = dRes_INDEX_SHIP_BCK_FN_MAST_ON2_e;
    mPart = i_part;
    if (mPart == PART_STEER_e) {
        seStart(shipFirst::JA_SE_SHIP_SAIL_OUT, &m0444);
        m03E8 = 1.0f;
    } else if (mPart == PART_CANNON_e) {
        seStart(shipFirst::JA_SE_LK_SHIP_CANNON_OUT, &m0444);
        m03E8 = 0.001f;
    } else if (mPart == PART_CRANE_e) {
        seStart(shipFirst::JA_SE_LK_SHIP_CRANE_OUT, &m0444);
        m03E8 = 0.001f;
    }
}



/* 0247BC44 */
void daShip_c::setPartAnimeInit(u8 i_part) {
    f32 remainingFrameRatio = 1.0f - mpBodyAnm->getFrame() / mpBodyAnm->getEndFrame();
    if (i_part == PART_WAIT_e) {
        if (m0392 == dRes_INDEX_SHIP_BCK_FN_MAST_ON2_e) {
            setPartOffAnime();
            mpBodyAnm->setFrame(remainingFrameRatio * mpBodyAnm->getEndFrame());
        }
    } else if (mPart != i_part) {
        if (m0392 == dRes_INDEX_SHIP_BCK_FN_MAST_ON2_e) {
            setPartOffAnime();
            mpBodyAnm->setFrame(remainingFrameRatio * mpBodyAnm->getEndFrame());
        } else if (mpBodyAnm->getPlaySpeed() < 0.01f) {
            setPartOnAnime(i_part);
        }
    } else if (m0392 == dRes_INDEX_SHIP_BCK_FN_MAST_OFF2_e) {
        setPartOnAnime(i_part);
        mpBodyAnm->setFrame(remainingFrameRatio * mpBodyAnm->getEndFrame());
    }
}



/* 0247E0D0: HD-only. Three line checks ahead of the boat's motion (spread around the speed direction,
 * 5000 ahead, 100 up); TRUE when one hits something. Not in the mini-game. Used to slow the boat
 * down (probably: so it does not ram walls at full speed) */
/* 0247E0D0 */
BOOL daShip_c::checkFrontObstacle() {
    if (dComIfGp_getMiniGameType() == 1 /* dComIfGp_getMiniGameType() */) {
        return FALSE;
    }
    if (!(std_sqrtf(gabi::fmadds(speed.x, speed.x, speed.z * speed.z)) > 30.0f)) {
        return FALSE;
    }
    s16 angle = cM_atan2s(speed.x, speed.z) - 0xC00 + ((ship_counter & 3) << 9);
    cXyz* ofs = ship_initVec(ship_obstacleGuard, ship_obstacleOffset, 0.0f, 100.0f, 5000.0f);
    gabi::Local<dBgS_LinChk> linChk;
    dBgS_LinChk_ct(linChk.get(), SHIP_LINCHK_VT, false);
    gabi::Local<cXyz> end;
    for (int i = 0; i < 3; i++) {
        cLib_offsetPos(end.get(), &current.pos, angle, ofs);
        dBgS_LinChk_Set(linChk.get(), &current.pos, end.get(), this);
        if (cBgS_LineCross(dComIfG_Bgsp(), linChk.get())) {
            ship_LinChk_dt(linChk.get());
            return TRUE;
        }
        angle += 0x800;
    }
    ship_LinChk_dt(linChk.get());
    return FALSE;
}



/* 0247E2B4 */
void daShip_c::setSelfMove(int param_1) {
    s16 tillerAngle;
    if (param_1) {
        tillerAngle = (s16)gabi::ftoi(-((8192.0f * mStickMVal) * cM_ssin(mStickMAng)));
    } else {
        tillerAngle = 0;
    }
    cLib_addCalcAngleS(&m0366, tillerAngle, 4, 700 /* l_HIO.tiller_speed */, 0x100);
    setMoveAngle(m0366);
    if (checkStateFlg(daSFLG_FLY_e)) {
        return;
    }
    /* HD: "no control" became "the link player is not the current player" */
    if ((ship_playerStatus0() & 0x2000 /* daPyStts0_SUBJECT_e */) || dComIfGp_event_runCheck() != 0 ||
        !ship_isLinkPlayer0()) {
        firstDecrementShipSpeed(0.0f);
        return;
    }
    if (!(m03E0 > speedF)) {
        firstDecrementShipSpeed(m03E0);
        return;
    }
    /* HD: top speed 55, 30 with an obstacle ahead; slowing down above it */
    f32 maxSpeed = 55.0f;
    if (checkFrontObstacle()) {
        maxSpeed = 30.0f;
    }
    if (speedF > maxSpeed) {
        cLib_addCalc(&speedF, maxSpeed, 0.1f, 2.0f, 0.1f);
        return;
    }
    if (CPad_CHECK_HOLD_A(0)) {
        /* HD: holding A without the stop action keeps the speed */
        if (dComIfGp_getAStatus() == shipFirst::ACT_STATUS_STOP /* HD: A status, rather than the GameCube Do status. */) {
            cLib_addCalc(&speedF, 0.0f, 0.1f, 1.0f, 0.1f);
        }
        return;
    }
    /* HD: 20 (GameCube 10) per unit of the R trigger */
    f32 triggerSpeed = (f32)(20.0 * CPad_GET_ANALOG_R(0));
    BOOL wallAhead = FALSE;
    if (mAcch.ChkWallHit()) {
        for (int i = 0; i < m03CC; i++) {
            if (mAcchCir[i].ChkWallHit() && cLib_distanceAngleS(mAcchCir[i].GetWallAngleY(), shape_angle.y) > 0x5000) {
                wallAhead = TRUE;
            }
        }
    }
    if (wallAhead && speedF > triggerSpeed) {
        firstDecrementShipSpeed(triggerSpeed);
    } else {
        decrementShipSpeed(triggerSpeed);
    }
}



/* 0247C300 */
BOOL daShip_c::procWait_init() {
    mCurMode = MODE_WAIT_e;
    setProc(shipFirst::procWait);
    m037A = 0;
    if (m0392 != dRes_INDEX_SHIP_BCK_FN_MAST_OFF2_e) {
        setPartOffAnime();
    }
    return TRUE;
}



/* 0247E670 */
BOOL daShip_c::procWait() {
    cLib_addCalc(&speedF, 0.0f, 0.1f, 1.0f, 0.05f);
    /* HD: frame 9 (GameCube 7) */
    if (!(mpBodyAnm->getFrame() < 9.0f)) {
        mPart = PART_WAIT_e;
        m03E8 = 1.0f;
    }
    if (mNextMode == MODE_READY_FIRST_e) {
        procReady_init();
    } else if (mNextMode == MODE_PADDLE_MOVE_e) {
        procPaddleMove_init();
    } else if (mNextMode == MODE_START_MODE_WARP_e) {
        procStartModeWarp_init();
    } else if (mNextMode == MODE_START_MODE_THROW_e) {
        procStartModeThrow_init();
    } else {
        setControllAngle(0);
        cLib_addCalcAngleS(&m0366, 0, 2, 0x1000, 0x400);
    }
    return TRUE;
}



/* 0247E640 */
BOOL daShip_c::procReady_init() {
    setProc(shipFirst::procReady);
    mCurMode = MODE_READY_FIRST_e;
    return TRUE;
}



/* 0247E77C */
BOOL daShip_c::procReady() {
    if (!(ship_playerStatus0() & (daPyStts0_SHIP_RIDE_e | shipTail::daPyStts0_UNK1000000_e))) {
        procWait_init();
    } else if (mNextMode == MODE_PADDLE_MOVE_e) {
        procPaddleMove_init();
    } else {
        if (mNextMode == 4) {
            mCurMode = 4;
        }
        s16 playerAngle;
        if (mCurMode == MODE_READY_FIRST_e) {
            playerAngle = fopAcM_searchPlayerAngleY(this);
            if ((s16)(playerAngle - shape_angle.y) > 0) {
                playerAngle = -0x800;
            } else {
                playerAngle = 0x800;
            }
        } else {
            playerAngle = 0;
        }
        setControllAngle(playerAngle);
    }
    return TRUE;
}



/* 0247DB84 */
BOOL daShip_c::procGetOff_init() {
    mCurMode = MODE_GET_OFF_FIRST_e;
    setProc(shipFirst::procGetOff);
    if (m0392 != dRes_INDEX_SHIP_BCK_FN_MAST_OFF2_e) {
        setPartOffAnime();
    }
    speedF = 0.0f;
    return TRUE;
}



/* 0247FE54 */
BOOL daShip_c::procGetOff() {
    /* HD: frame 9 (GameCube 7) */
    if (!(mpBodyAnm->getFrame() < 9.0f)) {
        mPart = PART_WAIT_e;
        m03E8 = 1.0f;
    }
    if (mNextMode == MODE_GET_OFF_SECOND_e) {
        mCurMode = MODE_GET_OFF_SECOND_e;
    }
    if (mCurMode == MODE_GET_OFF_SECOND_e) {
        if (m036C > 0x800) {
            procWait_init();
        } else {
            setControllAngle(0x1000);
            cLib_addCalcAngleS(&m0366, 0, 2, 0x1000, 0x400);
        }
    } else {
        setControllAngle(0);
        cLib_addCalcAngleS(&m0366, 0, 2, 0x1000, 0x400);
        /* daPy_getPlayerLinkActorClass()->checkShipGetOff(): the current procedure (+0x65F0) */
        if (shipTail::linkProcedure(gabi::ea(dComIfGp_getLinkPlayer())) != 0x90) {
            procWait_init();
        }
    }
    return TRUE;
}



/* 0247BEFC */
BOOL daShip_c::procPaddleMove_init() {
    offStateFlg(daSFLG_UNK4_e);
    mCurMode = MODE_PADDLE_MOVE_e;
    setProc(shipFirst::procPaddleMove);
    if (m0392 != dRes_INDEX_SHIP_BCK_FN_MAST_OFF2_e) {
        setPartOffAnime();
    }
    if (checkStateFlg(daSFLG_JUMP_RIDE_e) != 0) {
        offStateFlg(daSFLG_JUMP_RIDE_e);
        current.pos.y -= 50.0f;
    }
    dComIfGs_onEventBit(ShipEvent::RODE_KORL);
    if (dComIfGs_isEventBit(ShipEvent::UNK_0A80)) {
        dComIfGs_onEventBit(ShipEvent::UNK_1980);
    }
    if (dComIfGs_isEventBit(ShipEvent::UNK_0A08)) {
        dComIfGs_onEventBit(ShipEvent::UNK_2A02);
    }
    if (dComIfGs_isEventBit(ShipEvent::ENDLESS_NIGHT)) {
        dComIfGs_onEventBit(ShipEvent::UNK_2A01);
    }
    return TRUE;
}



/* 0247EEE8 */
BOOL daShip_c::procPaddleMove() {
    if (checkNextMode(MODE_PADDLE_MOVE_e)) {
        return TRUE;
    }
    /* HD: frame 9 (GameCube 7) */
    if (!(mpBodyAnm->getFrame() < 9.0f)) {
        mPart = PART_WAIT_e;
        m03E8 = 1.0f;
    }
    if ((ship_playerStatus0() & 0x685000 /* bow, hookshot, boomerang aim, telescope, boomerang wait */) ||
        (shipTail::playerStatus1(dComIfGp_ea()) & shipTail::daPyStts1_PICTO_BOX_AIM_e)) {
        cLib_addCalcAngleS(&m0366, 0, 4, 700, 0x100);
        if (shipTail::eventRun(dComIfGp_ea()) == 0) {
            setMoveAngle(m0366);
        }
        firstDecrementShipSpeed(0.0f);
    } else if (shipTail::eventRun(dComIfGp_ea()) == 0) {
        setSelfMove(1);
    }
    return TRUE;
}



/* 0247BE9C */
BOOL daShip_c::procSteerMove_init() {
    offStateFlg(daSFLG_UNK4_e);
    mCurMode = MODE_STEER_MOVE_e;
    setProc(shipFirst::procSteerMove);
    setPartAnimeInit(PART_STEER_e);
    return TRUE;
}



/* 0247E854 */
BOOL daShip_c::procSteerMove() {
    if (checkNextMode(MODE_STEER_MOVE_e)) {
        return TRUE;
    }
    cLib_addCalcAngleS(&m0366, (s16)gabi::ftoi(-((8192.0f * mStickMVal) * cM_ssin(mStickMAng))), 4, 700, 0x100);
    if (!checkStateFlg(daSFLG_FLY_e)) {
        setMoveAngle(m0366);
    }
    setSailAngle();
    /* HD: frame 9 (GameCube 7) */
    if (m0392 == dRes_INDEX_SHIP_BCK_FN_MAST_ON2_e && !(mpBodyAnm->getFrame() < 9.0f) && mPart == PART_STEER_e) {
        onStateFlg(daSFLG_SAIL_ON_e);
    }
    if (!checkStateFlg(daSFLG_FLY_e)) {
        BOOL justRaised = FALSE;
        f32 maxSpeed = 100.0f;
        cXyz* windVec;
        f32 windPower;
        /* HD: the Swift Sail (play+0x5D2C): the wind always follows the boat, top speed 100 */
        if (shipTail::swiftSailEnabled(dComIfGp_ea()) != 0) {
            dKyw_tact_wind_set(0, (s16)((0x5000 - shape_angle.y) & ~0x1FFF));
            if (mSwiftSailActive == 0) {
                mSwiftSailActive = 1;
                seStart(shipTail::SAIL_POWER_UP, &m0444);
                hd_seStartByName(gabi::guest_addr("SE_SAIL_POWER_UP"));
                if (mpGrid != 0) {
                    shipTail::setSailGridTimer(mpGrid, 0xF);
                }
                onStateFlg(daSFLG_UNK80000000_e);
                justRaised = TRUE;
            }
            windVec = dKyw_get_wind_vec();
            f32 windStrength = *dKyw_get_wind_power();
            windPower = windStrength + windStrength;
        } else {
            maxSpeed = 55.0f;
            if (mSwiftSailActive == 1) {
                mSwiftSailActive = 0;
                seStart(shipTail::SAIL_POWER_DOWN, &m0444);
                if (mpGrid != 0) {
                    shipTail::setSailGridTimer(mpGrid, 0xF);
                }
                onStateFlg(daSFLG_UNK80000000_e);
            }
            windVec = dKyw_get_wind_vec();
            f32 windStrength = *dKyw_get_wind_power();
            windPower = windStrength + windStrength;
        }
        s16 windAngle = cM_atan2s(windVec->x, windVec->z) - shape_angle.y;
        int angleDiff = windAngle < 0 ? -windAngle : windAngle;
        f32 windFactor;
        if (angleDiff < 0x4000) {
            windFactor = gabi::fnmsubs((f32)angleDiff, 1.0f / 0x10000, 1.0f);
        } else if (angleDiff <= 0x6000) {
            windFactor = gabi::fnmsubs((f32)(angleDiff - 0x4000), 5.493164e-05f, 0.75f);
        } else {
            windFactor = gabi::fnmsubs((f32)(angleDiff - 0x6000), 0.00032967035f, 0.3f);
            windFactor = windFactor >= 0.0f ? windFactor : 0.0f;
        }
        if (windPower > 1.0f) {
            windPower = 1.0f;
        }
        windPower = (windFactor * maxSpeed) * windPower;

        f32 stickRate = 20.0f;
        if (checkFrontObstacle()) {
            /* HD: an obstacle ahead caps the wind speed */
            windPower = 30.0f;
        } else if (justRaised || mpBodyAnm->checkFrame(9.0f)) {
            /* HD: 1.2 times the wind speed, no cap (GameCube: twice, at most 80) */
            f32 targetSpeed = windPower * 1.2f;
            if (speedF < targetSpeed) {
                speedF = targetSpeed;
                if (mTornadoActor || mWhirlActor) {
                    m03E0 = 10000.0f;
                } else {
                    m03E0 = windPower;
                }
            }
        }
        if (m03E0 < speedF) {
            firstDecrementShipSpeed(m03E0);
        } else if (!checkStateFlg(daSFLG_SAIL_ON_e) || !(windPower > 0.0f)) {
            /* HD: 20 (GameCube 10) */
            decrementShipSpeed((stickRate * mStickMVal) * cM_scos(mStickMAng));
        } else {
            /* HD: always towards the wind speed, step 2 */
            cLib_addCalc(&speedF, windPower, 0.1f, 2.0f, 0.1f);
        }
        if (shipTail::eventRun(dComIfGp_ea()) == 0 && ship_isLinkPlayer0() &&
            m0392 == dRes_INDEX_SHIP_BCK_FN_MAST_ON2_e && !(mpBodyAnm->getFrame() < 9.0f) &&
            !checkStateFlg(daSFLG_FLY_e) && mFwdVel > 30.000002f) {
            /* HD: jumps need wind speed 20 and forward speed 30 (GameCube 11 / 16.5) */
            if (!mTornadoActor && !mWhirlActor && !(windPower < stickRate)) {
                onStateFlg(daSFLG_JUMP_OK_e);
                if (CPad_R_LOCK_TRIGGER(0)) {
                    f32 vy = windPower;
                    onStateFlg(daSFLG_FLY_e | daSFLG_JUMP_e);
                    if (vy < 15.0f) {
                        vy = 15.0f;
                    } else if (vy > 40.0f) {
                        vy = 40.0f;
                    }
                    current.angle.y = shape_angle.y;
                    speed.y = vy;
                    m0386 = 0;
                    m036C = m036C >> 1;
                    mJumpRate = (vy - stickRate) * 0.008f;
                }
            }
        }
    } else {
        setControllAngle(0);
        m036E = 0;
    }
    if (mPart != PART_STEER_e && mpBodyAnm->getPlaySpeed() < 0.01f) {
        setPartOnAnime(PART_STEER_e);
    }
    return TRUE;
}



/* 0247D684 */
BOOL daShip_c::procCannonReady_init() {
    mCurMode = MODE_CANNON_e;
    setProc(shipFirst::procCannonReady);
    setPartAnimeInit(PART_CANNON_e);
    m0394 = 0;
    return TRUE;
}



/* 0247F024 */
BOOL daShip_c::procCannonReady() {
    if (checkNextMode(MODE_CANNON_e)) {
        return TRUE;
    }
    cLib_addCalcAngleS(&m0366, 0, 2, 0x1000, 0x400);
    setMoveAngle(m0366);
    firstDecrementShipSpeed(0.0f);
    if (mpBodyAnm->getPlaySpeed() < 0.01f) {
        if (mPart != PART_CANNON_e) {
            setPartOnAnime(PART_CANNON_e);
        } else {
            procCannon_init();
        }
    }
    return TRUE;
}



/* 0247EFE0 */
BOOL daShip_c::procCannon_init() {
    setProc(ShipProcedure::procCannon);
    m03A8 = 0;
    m0366 = 0;
    speedF = 0.0f;
    m037A = 0;
    m03A6 = 0;
    return TRUE;
}



/* 0247F0D0 */
BOOL daShip_c::procCannon() {
    if (checkNextMode(MODE_CANNON_e)) {
        return TRUE;
    }
    setControllAngle(0);
    if (shipTail::eventRun(dComIfGp_ea()) != 0 || !ship_isLinkPlayer0()) {
        return TRUE;
    }
    u32 attention = dComIfGp_ea() + 0x5804;
    fopAc_ac_c* target = nullptr;
    /* attention.Lockon() && attention.GetLockonList(0) && attention.LockonTruth() */
    if (dAttention_LockonTruth(attention) || (shipTail::attentionFlags(attention) & 0x20000000)) {
        u32 list = dAttention_GetLockonList(attention, 0);
        if (list != 0 && dAttention_LockonTruth(attention)) {
            target = dAttList_getActor(list);
        }
    }
    s16 prev0396 = m0396;
    s16 prev0394 = m0394;
    if (target) {
        /* mDoMtx_multVecZero(mpCannonModel->getAnmMtx(VFNCN_JNT_CANON2_e), &cannonPos) */
        Mtx34* m = model_getAnmMtx(mpCannonModel, VFNCN_JNT_CANON2_e);
        gabi::Local<cXyz> cannonPos;
        cannonPos->x = m->m[0][3];
        cannonPos->y = m->m[1][3];
        cannonPos->z = m->m[2][3];
        gabi::Local<cXyz> d;
        cXyz_mi(&target->eyePos, d.get(), cannonPos.get());
        f32 dx = d->x, dy = d->y, dz = d->z;
        gabi::Local<cXyz> xz;
        xz->x = dx;
        cannonPos->y = dy;
        xz->z = dz;
        cannonPos->z = dz;
        xz->y = 0.0f;
        cannonPos->x = dx;
        cLib_addCalcAngleS(&m0396, cM_atan2s(-cannonPos->y, cXyz_abs(xz.get())) + 0x4000, 5, 0x180, 0x40);
        cLib_addCalcAngleS(&m0394, cM_atan2s(cannonPos->x, cannonPos->z) - shape_angle.y, 5, 0x180, 0x40);
    } else {
        BOOL rlock = CPad_R_LOCK_BUTTON(0);
        f32 adjust = mStickMVal * gabi::fmadds(m0404, 4.0f, 1.0f);
        if (!rlock) {
            /* HD: the vertical aim follows the camera inversion option */
            f32 v = adjust;
            if (ship_isCameraInverted()) {
                v = -v;
            }
            m0396 = (s16)gabi::ftoi(gabi::fmadds(384.0f * v, cM_scos(mStickMAng), (f32)(s16)m0396));
            m0394 += (s16)gabi::ftoi((384.0f * adjust) * cM_ssin(mStickMAng));
        } else {
            /* HD: with R held the stick still aims up and down */
            if (ship_isCameraInverted()) {
                adjust = -adjust;
            }
            m0396 = (s16)gabi::ftoi(gabi::fmadds(384.0f * adjust, cM_scos(mStickMAng), (f32)(s16)m0396));
        }
    }
    if (m0396 > 0x4000) {
        m0396 = 0x4000;
    } else if (m0396 < 0x1556) {
        m0396 = 0x1556;
    }
    if (cLib_distanceAngleS(prev0396, m0396) >= 0x80 || cLib_distanceAngleS(prev0394, m0394) >= 0x80) {
        seStart(shipTail::JA_SE_LK_SHIP_CANNON_MOVE, &m1038);
    }
    if (m037A == 0 && dComIfGp_getDoStatus() == 0x3F /* HD */ &&
        ((CPad_CHECK_TRIG_X(0) && dComIfGp_getSelectItem(0) == dItemNo_BOMB_BAG_e) ||
         (CPad_CHECK_TRIG_Y(0) && dComIfGp_getSelectItem(1) == dItemNo_BOMB_BAG_e) ||
         (CPad_CHECK_TRIG_Z(0) && dComIfGp_getSelectItem(2) == dItemNo_BOMB_BAG_e) ||
         CPad_CHECK_TRIG_HD1(0))) {
        m037A = 30;
        if (dComIfGs_getBombNum() == 0 /* dComIfGs_getBombNum() */) {
            mDoAud_seStart_simple(shipTail::JA_SE_ITEM_TARGET_OUT);
            m037A--;
        }
    } else if (m037A > 0) {
        m037A--;
    }
    setSelfMove(CPad_R_LOCK_BUTTON(0) ? 1 : 0);
    return TRUE;
}



/* 0247D6EC */
BOOL daShip_c::procCraneReady_init() {
    mCurMode = MODE_CRANE_e;
    setProc(shipFirst::procCraneReady);
    mCraneBaseAngle = 0x3000;
    m039C = 0;
    setPartAnimeInit(PART_CRANE_e);
    mRopeCnt = 0;
    return TRUE;
}



/* 0247F618 */
BOOL daShip_c::procCraneReady() {
    if (checkNextMode(MODE_CRANE_e)) {
        return TRUE;
    }
    cLib_addCalcAngleS(&m0366, 0, 2, 0x1000, 0x400);
    setMoveAngle(m0366);
    firstDecrementShipSpeed(0.0f);
    if (mpBodyAnm->getPlaySpeed() < 0.01f) {
        if (mPart != PART_CRANE_e) {
            setPartOnAnime(PART_CRANE_e);
        } else {
            incRopeCnt(2, 0);
            if (mRopeCnt == 20) {
                procCrane_init();
            }
        }
    }
    if (mPart == PART_CRANE_e) {
        f32 frameRate = (mpBodyAnm->getFrame() - 5.0f) * 0.33333334f;
        if (frameRate > 1.0f) {
            frameRate = 1.0f;
        } else {
            frameRate = frameRate >= 0.0f ? frameRate : 0.0f;
        }
        m0398 = (s16)gabi::ftoi(frameRate * (f32)(s16)mCraneBaseAngle);
    }
    return TRUE;
}



/* 0247F57C */
BOOL daShip_c::procCrane_init() {
    setProc(shipTail::procCrane);
    m0366 = 0;
    speedF = 0.0f;
    shipTail::setRippleRate(gabi::ea(mRipple), 0.0f); /* mRipple.setRate(0.0f) */
    mCurMode = MODE_CRANE_e;
    offStateFlg(daSFLG_UNK800_e);
    m0353 = cM_rnd() < 0.5f;
    return TRUE;
}



/* 0247F7CC */
BOOL daShip_c::procCrane() {
    if (checkNextMode(MODE_CRANE_e)) {
        offStateFlg(daSFLG_UNK800_e);
        return TRUE;
    }
    setControllAngle(0);
    if (shipTail::eventRun(dComIfGp_ea()) != 0 || !ship_isLinkPlayer0()) {
        return TRUE;
    }
    if (m0398 == mCraneBaseAngle) {
        s16 craneTurn;
        if (std::fabs((f32)speedF) < 3.0f &&
            ((CPad_CHECK_HOLD_X(0) && dComIfGp_getSelectItem(0) == dItemNo_GRAPPLING_HOOK_e) ||
             (CPad_CHECK_HOLD_Y(0) && dComIfGp_getSelectItem(1) == dItemNo_GRAPPLING_HOOK_e) ||
             (CPad_CHECK_HOLD_Z(0) && dComIfGp_getSelectItem(2) == dItemNo_GRAPPLING_HOOK_e) ||
             CPad_CHECK_HOLD_HD1(0) /* HD */)) {
            s16 previousRopeCount = mRopeCnt;
            incRopeCnt(2, 0);
            /* HD: the rope is at most 125 segments long (GameCube 250) */
            if (mRopeCnt == 125) {
                craneTurn = 0;
                if (mCraneBaseAngle > 0) {
                    setControllAngle(0xA00);
                } else {
                    setControllAngle(-0xA00);
                }
                if (m0353 && previousRopeCount != mRopeCnt) {
                    gabi::Local<cXyz> v;
                    v->x = 0.0f;
                    v->y = 1.0f;
                    v->z = 0.0f;
                    ship_StartShock(5, -0x31, v.get());
                }
            } else if (mCraneBaseAngle > 0) {
                craneTurn = 0x800;
            } else {
                craneTurn = -0x800;
            }
        } else {
            /* HD: winds in twice as fast */
            incRopeCnt(-4, 20);
            if (mRopeCnt == 20) {
                craneTurn = 0;
            } else if (mCraneBaseAngle > 0) {
                craneTurn = -0x800;
            } else {
                craneTurn = 0x800;
            }
        }
        cLib_chaseAngleS(&m039C, craneTurn, 0x100);
        if (mRopeCnt == 20 && !CPad_R_LOCK_BUTTON(0)) {
            if (mStickMVal > 0.1f) {
                s16 previousBaseAngle = mCraneBaseAngle;
                if (mStickMAng < -0x2000 && mStickMAng > -0x6000) {
                    mCraneBaseAngle = 0x3000;
                    m03A6 = 0;
                    onStateFlg(daSFLG_UNK10000000_e);
                } else if (mStickMAng > 0x2000 && mStickMAng < 0x6000) {
                    mCraneBaseAngle = -0x3000;
                    m03A6 = 0;
                    onStateFlg(daSFLG_UNK10000000_e);
                }
                if (previousBaseAngle * mCraneBaseAngle < 0) {
                    seStart(shipTail::JA_SE_LK_SHIP_CRANE_ARM, &m102C);
                }
            }
            /* HD: the stick also steers while the crane is idle */
            cLib_addCalcAngleS(&m0366, (s16)gabi::ftoi(-((8192.0f * mStickMVal) * cM_ssin(mStickMAng))), 4, 700, 0x100);
            setMoveAngle(m0366);
        }
    } else {
        cLib_chaseAngleS(&m03A6, 0x1800, 0x100);
        cLib_addCalcAngleS(&m0398, mCraneBaseAngle, 5, m03A6, 0x100);
    }
    if (CPad_R_LOCK_BUTTON(0) && mRopeCnt == 20) {
        setSelfMove(1);
        m0353 = cM_rnd() < 0.5f;
    } else {
        setSelfMove(0);
    }
    return TRUE;
}



/* 0247FBC4 */
BOOL daShip_c::procCraneUp() {
    s16 craneTurn;
    if (mCraneBaseAngle > 0) {
        craneTurn = 0x800;
    } else {
        craneTurn = -0x800;
    }
    cLib_addCalcAngleS(&m039C, craneTurn, 5, 0x1800, 0x100);
    if (shipTail::rippleEmitter(gabi::ea(mRipple)) == 0) { /* mRipple.getEmitter() == NULL */
        setControllAngle(0);
        onStateFlg(daSFLG_CRANE_UP_END_e);
        ship_StopQuake(-1);
    } else {
        setControllAngle(m03A6);
        if (cLib_distanceAngleS(m036C, m03A6) < 0x400) {
            if (mCraneBaseAngle > 0) {
                if (m03A6 > 0x400) {
                    m03A6 = (s16)gabi::ftoi(256.0f * cM_rnd());
                } else {
                    m03A6 = (s16)gabi::ftoi(gabi::fmadds(1024.0f, cM_rnd(), 2048.0f));
                    seStart(0x381E, &current.pos);
                }
            } else {
                if (m03A6 < -0x400) {
                    m03A6 = (s16)gabi::ftoi(-256.0f * cM_rnd());
                } else {
                    m03A6 = (s16)gabi::ftoi(gabi::fnmsubs(1024.0f, cM_rnd(), -2048.0f));
                    seStart(0x381E, &current.pos);
                }
            }
        }
    }
    incRopeCnt(-1, 20);
    if (mRopeCnt == 20) {
        if (shipTail::eventRun(dComIfGp_ea()) != 0) {
            /* daPy_getPlayerLinkActorClass()->getBaseAnimeFrameRate() (virtual +0x9C) */
            fopAc_ac_c* link = dComIfGp_getLinkPlayer();
            if (shipTail::baseAnimationRate(link) < 0.01f) {
                dComIfGp_evmng_cutEnd(mEvtStaffId);
            }
        } else {
            procCrane_init();
        }
    }
    return TRUE;
}



/* 0247FF54 */
BOOL daShip_c::procToolDemo() {
    u32 demoActor = ship_demo_getActor(demoActorID);
    if (demoActor) {
        if (shipTail::demoFlags(demoActor) & shipTail::ENABLE_TRANS_e) {
            current.pos.x = shipTail::demoTranslationX(demoActor);
            current.pos.z = shipTail::demoTranslationZ(demoActor);
        }
        if (shipTail::demoFlags(demoActor) & shipTail::ENABLE_ROTATE_e) {
            s16 prevShapeAngleY = shape_angle.y;
            shape_angle.y = shipTail::demoRotationY(demoActor);
            current.angle.y = shape_angle.y;
            s16 angleDiff = shape_angle.y - prevShapeAngleY;
            if (angleDiff > 0) {
                angleDiff = 0x2000;
            } else if (angleDiff < 0) {
                angleDiff = -0x2000;
            } else {
                angleDiff = 0;
            }
            cLib_addCalcAngleS(&m0366, angleDiff, 4, 700, 0x100);
            setControllAngle(getAimControllAngle(prevShapeAngleY));
        }
    } else {
        changeDemoEndProc();
    }
    return TRUE;
}



/* 024800A8 */
BOOL daShip_c::procZevDemo() {
    if (!(ship_playerStatus0() & daPyStts0_SHIP_RIDE_e) && m0392 != dRes_INDEX_SHIP_BCK_FN_MAST_OFF2_e) {
        setPartOffAnime();
    }
    if (mEvtStaffId == -1) {
        changeDemoEndProc();
        return TRUE;
    }
    u32 posP = ship_getSubst(mEvtStaffId, gabi::guest_addr("pos"), 1);
    u32 angleP = ship_getSubst(mEvtStaffId, gabi::guest_addr("angle"), 3);
    u32 speedP = ship_getSubst(mEvtStaffId, gabi::guest_addr("speed"), 0);
    u32 partP = ship_getSubst(mEvtStaffId, gabi::guest_addr("part"), 3);
    u32 talkP = ship_getSubst(mEvtStaffId, gabi::guest_addr("talk"), 3);
    u32 atn_actorP = ship_getSubst(mEvtStaffId, gabi::guest_addr("atn_actor"), 3);

    if (atn_actorP && dComIfGp_event_getPt1()) {
        m0428 = shipTail::actorEyePosition(gabi::ea(dComIfGp_event_getPt1())); /* &pt1->eyePos */
    } else {
        m0428 = gabi::at<cXyz>(ship_getSubst(mEvtStaffId, gabi::guest_addr("atn_pos"), 1));
    }

    if (m0351 != shipTail::DEMO_INIT_e && partP) {
        u32 part = shipTail::word(partP);
        if (m0350 == 4 || m0350 != part) {
            m0350 = (u8)part;
            setPartAnimeInit(shipTail::part(partP));
        }
    }

    if (talkP && shipTail::integer(talkP) == 1) {
        if (!checkStateFlg(daSFLG_UNK8000_e)) {
            m038A = dRes_INDEX_SHIP_BCK_FN_TALK_A_e;
            mAnmTransform = ship_getRes(dRes_INDEX_SHIP_BCK_FN_TALK_A_e);
            J3DFrameCtrl_init(&mFrameCtrl, anm_getFrameMax(mAnmTransform));
            onStateFlg(daSFLG_UNK8000_e);
        }
    } else {
        offStateFlg(daSFLG_UNK8000_e);
    }

    /* daPy_getPlayerLinkActorClass()->onShipTact() / offShipTact() */
    u32 tact = ship_getSubst(mEvtStaffId, gabi::guest_addr("tact"), 3);
    u32 link = gabi::ea(dComIfGp_getLinkPlayer());
    if (tact) {
        shipTail::setLinkNoResetFlags(link, shipTail::linkNoResetFlags(link) | shipTail::LINK_SHIP_TACT);
    } else {
        shipTail::setLinkNoResetFlags(link, shipTail::linkNoResetFlags(link) & ~shipTail::LINK_SHIP_TACT);
    }

    if (m0351 == shipTail::DEMO_INIT_e || m0351 == shipTail::DEMO_OPEN_e) {
        s16 targetAngle;
        if (angleP) {
            targetAngle = shipTail::angle(angleP);
        } else {
            targetAngle = shape_angle.y;
        }
        if (posP) {
            initStartPos(gabi::at<cXyz>(posP), targetAngle);
        } else {
            initStartPos(&current.pos, targetAngle);
        }
        if (speedP) {
            speedF = shipTail::scalar(speedP);
        }
        if (partP || m0351 == 8) {
            if (m0351 == 8) {
                m0350 = 0;
            } else {
                m0350 = shipTail::part(partP);
            }
            mRopeCnt = 0;
            mPart = m0350;
            u16 fileIndex;
            if (mPart != PART_WAIT_e) {
                fileIndex = dRes_INDEX_SHIP_BCK_FN_MAST_ON2_e;
            } else {
                fileIndex = dRes_INDEX_SHIP_BCK_FN_MAST_OFF2_e;
                m03E8 = 0.001f;
            }
            if (m0392 != fileIndex) {
                J3DAnmTransform* res1 = ship_getRes(fileIndex);
                mpBodyAnm->setAnm(res1, 0, 0.0f, 1.0f, 0.0f, -1.0f, nullptr);
                m0392 = fileIndex;
                mpBodyAnm->setFrame(mpBodyAnm->getEndFrame() - 0.001f);
                /* HD: no mpBodyAnm->getAnm()->setFrame() */
                mpBodyAnm->play(nullptr, 0, 0);
            }
        }
        setControllAngle(0);
        dComIfGp_evmng_cutEnd(mEvtStaffId);
    } else if (m0351 == shipTail::DEMO_MOVE_e || m0351 == shipTail::DEMO_UNK03_e) {
        cXyz* pos = gabi::at<cXyz>(posP);
        if (pos == nullptr) {
            pos = dComIfGp_evmng_getGoal();
        }
        f32 lx, lz;
        f32 targetDistance;
        if (m0351 == 1) {
            gabi::Local<cXyz> local_7c;
            cXyz_mi(pos, local_7c.get(), &current.pos);
            lx = local_7c->x;
            lz = local_7c->z;
        } else {
            gabi::Local<cXyz> local_70;
            cXyz_mi(&current.pos, local_70.get(), pos);
            gabi::Local<cXyz> xz;
            xz->x = local_70->x;
            xz->y = 0.0f;
            xz->z = local_70->z;
            f32 dist = cXyz_abs(xz.get());
            if (!ship_getSubst(mEvtStaffId, gabi::guest_addr("rad"), 0)) {
                JUT_ASSERT_fail(STR(0x1003A7B0), 0x134A, STR(0x1003A780));
            }
            /* HD: a missing "rad" counts as 0 */
            u32 radP = ship_getSubst(mEvtStaffId, gabi::guest_addr("rad"), 0);
            f32 radiusOrMaxSpeed = radP ? shipTail::scalar(radP) : 0.0f;
            gabi::Local<cXyz> local_64;
            if (dist < 0.1f) {
                f32 z = pos->z + radiusOrMaxSpeed;
                local_64->y = pos->y;
                local_64->x = gabi::fmadds(radiusOrMaxSpeed, cM_ssin(0x4000), pos->x);
                local_64->z = gabi::fmadds(radiusOrMaxSpeed, cM_scos(0x4000), z);
            } else {
                s16 targetAngle = cM_atan2s(local_70->x, local_70->z) + 0x4000;
                gabi::Local<cXyz> scaled;
                cXyz_ml(local_70.get(), scaled.get(), radiusOrMaxSpeed / dist);
                gabi::Local<cXyz> p64;
                cXyz_pl(pos, p64.get(), scaled.get());
                f32 py = p64->y;
                local_64->x = gabi::fmadds(radiusOrMaxSpeed, cM_ssin(targetAngle), p64->x);
                local_64->y = py;
                local_64->z = gabi::fmadds(radiusOrMaxSpeed, cM_scos(targetAngle), p64->z);
            }
            gabi::Local<cXyz> local_7c;
            cXyz_mi(local_64.get(), local_7c.get(), &current.pos);
            lx = local_7c->x;
            lz = local_7c->z;
        }
        gabi::Local<cXyz> xz2;
        xz2->y = 0.0f;
        xz2->z = lz;
        xz2->x = lx;
        targetDistance = cXyz_abs(xz2.get());

        if (targetDistance < 10.0f) {
            speedF = 0.0f;
            dComIfGp_evmng_cutEnd(mEvtStaffId);
        } else if (speedP && !(shipTail::scalar(speedP) > -0.5f)) {
            f32 sp = shipTail::scalar(speedP);
            if (!(m03E0 > speedF)) {
                firstDecrementShipSpeed(m03E0);
            } else if (sp < -1.5f || speedF > 55.0f /* HD */) {
                cLib_addCalc(&speedF, 0.0f, 0.1f, 1.0f, 0.1f);
            } else {
                decrementShipSpeed(0.0f);
            }
        } else {
            f32 v = targetDistance * 0.2f;
            speedF = v;
            /* HD: at most 100 without a "speed" (GameCube 55) */
            f32 radiusOrMaxSpeed = speedP ? shipTail::scalar(speedP) : 100.0f;
            if (v > radiusOrMaxSpeed) {
                speedF = radiusOrMaxSpeed;
            }
        }
        if (targetDistance > 1.0f) {
            s16 targetAngle = shape_angle.y;
            cLib_addCalcAngleS(&shape_angle.y, cM_atan2s(lx, lz), 3, 0x2000, 0x80);
            current.angle.y = shape_angle.y;
            s16 sVar5 = shape_angle.y - targetAngle;
            if (sVar5 > 0) {
                sVar5 = 0x2000;
            } else if (sVar5 < 0) {
                sVar5 = -0x2000;
            } else {
                sVar5 = 0;
            }
            cLib_addCalcAngleS(&m0366, sVar5, 4, 700, 0x100);
            setControllAngle(getAimControllAngle(targetAngle));
        } else {
            setControllAngle(0);
        }
    } else if (m0351 == shipTail::DEMO_RACE_FAIL_e) {
        if (!checkStateFlg(daSFLG_UNK10000_e)) {
            J3DAnmTransform* res2 = ship_getRes(dRes_INDEX_SHIP_BCK_FN_LOSE1_e);
            mpHeadAnm->setAnm(res2, 0, 5.0f, 1.0f, 0.0f, -1.0f, nullptr);
            m03B4 = dRes_INDEX_SHIP_BCK_FN_LOSE1_e;
            onStateFlg(daSFLG_UNK10000_e);
        }
        speedF = 0.0f;
        setControllAngle(0);
        if (mpHeadAnm->getPlaySpeed() < 0.01f) {
            dComIfGp_evmng_cutEnd(mEvtStaffId);
        }
    } else if (m0351 == shipTail::DEMO_KEEP_e) {
        if (speedP) {
            firstDecrementShipSpeed(shipTail::scalar(speedP));
        }
        setControllAngle(0);
        dComIfGp_evmng_cutEnd(mEvtStaffId);
    } else if (m0351 == shipTail::DEMO_NECK_e) {
        u32 prm = ship_getSubst(mEvtStaffId, gabi::guest_addr("prm0"), 3);
        u16 fileIndex;
        if (prm == 0 || !(shipTail::word(prm) & 1)) {
            fileIndex = dRes_INDEX_SHIP_BCK_FN_LOOK_R_e;
        } else {
            fileIndex = dRes_INDEX_SHIP_BCK_FN_LOOK_L_e;
        }
        f32 rate;
        if (prm && shipTail::integer(prm) == 2) {
            rate = -1.0f;
        } else {
            rate = 1.0f;
        }
        if (!checkStateFlg(daSFLG_UNK80000_e) && rate > 0.0f) {
            J3DAnmTransform* res3 = ship_getRes(fileIndex);
            mpHeadAnm->setAnm(res3, 0, 5.0f, rate, 0.0f, -1.0f, nullptr);
            m03B4 = fileIndex;
            onStateFlg(daSFLG_UNK80000_e);
        } else if (checkStateFlg(daSFLG_UNK80000_e) && rate < 0.0f) {
            offStateFlg(daSFLG_UNK80000_e);
            mpHeadAnm->setPlaySpeed(-1.0f);
        }
        setControllAngle(0);
        if (!(std::fabs(mpHeadAnm->getPlaySpeed()) > 0.1f)) {
            dComIfGp_evmng_cutEnd(mEvtStaffId);
        }
    } else if (m0351 == shipTail::DEMO_THROW_e) {
        speedF = 150.0f; /* l_HIO.throw_start_speedF */
        u32 gravP = ship_getSubst(mEvtStaffId, gabi::guest_addr("gravity"), 0);
        if (gravP) {
            gravity = shipTail::scalar(gravP);
        }
        speed.y = 50.0f; /* l_HIO.throw_start_speed_y */
        onStateFlg(daSFLG_FLY_e);
        if (angleP) {
            current.angle.y = shipTail::angle(angleP);
        }
        shape_angle.y += 4500; /* l_HIO.throw_start_angle_speed */
        dComIfGp_evmng_cutEnd(mEvtStaffId);
    } else if (m0351 == shipTail::DEMO_HWARP_UP_e) {
        cLib_addCalcAngleS(&m0370, 0, 10, 0x1000, 0x200);
        cLib_addCalcAngleS(&m0372, 0, 10, 0x1000, 0x200);
        cLib_addCalcAngleS(&m0384, 0, 10, 0x1000, 0x200);
        cLib_addCalcAngleS(&m036C, 0, 10, 0x1000, 0x200);
        onStateFlg(daSFLG_FLY_e);
        if (speed.y < 15.0f) {
            speed.y += 1.0f;
        }
        m0366 = 0;
        m036E = 0;
        m0374 = 0;
        m0376 = 0;
        m0386 = 0;
        gravity = 0.0f;
        speedF = 0.0f;
        current.pos.y += speed.y;
        dComIfGp_evmng_cutEnd(mEvtStaffId);
    } else if (m0351 == shipTail::DEMO_HWARP_DOWN_e) {
        if (m03F4 > current.pos.y && !(gravity < 0.0f)) {
            offStateFlg(daSFLG_FLY_e);
            m03F4 = current.pos.y;
            dComIfGp_evmng_cutEnd(mEvtStaffId);
        } else {
            m0370 = 0;
            m0372 = 0;
            m0384 = 0;
            m036C = 0;
            m0366 = 0;
            m036E = 0;
            m0374 = 0;
            m0376 = 0;
            m0386 = 0;
            onStateFlg(daSFLG_FLY_e);
            if (gravity < 0.0f) {
                gravity = 0.0f;
                current.pos.y += 500.0f;
                speed.y = 0.0f;
            } else {
                f32 vy = speed.y - 1.0f;
                if (vy < -15.0f) {
                    vy = -15.0f;
                }
                speed.y = vy;
            }
        }
    } else {
        return TRUE;
    }
    if (mpBodyAnm->getPlaySpeed() < 0.01f) {
        if (m0350 != 4 && m0350 != 0 && mPart != m0350) {
            setPartOnAnime(m0350);
        }
    }
    return TRUE;
}



/* 02481C94 */
BOOL daShip_c::procTornadoUp() {
    fopAc_ac_c* tornado = mTornadoActor;
    cLib_addCalc(&current.pos.x, tornado_getJointPos(tornado, m037A, 0), 0.5f, 80.0f, 20.0f);
    cLib_addCalc(&current.pos.z, tornado_getJointPos(tornado, m037A, 2), 0.5f, 80.0f, 20.0f);
    f32 vy = speed.y;
    current.pos.y += vy;
    vy = vy + 2.0f;
    if (vy > 50.0f) {
        vy = 50.0f;
    }
    speed.y = vy;
    if (tornado_getJointPos(tornado, m037A, 1) < current.pos.y && m037A < 11) {
        m037A++;
    }
    shape_angle.y += 0x1C25;

    u32 camera = ship_getCameraAddress(dComIfGp_getPlayerCameraID0());
    gabi::Local<cXyz> local_3c;
    local_3c->x = tornado->current.pos.x;
    local_3c->y = current.pos.y;
    local_3c->z = tornado->current.pos.z;
    gabi::Local<cXyz> eye;
    camera_Eye(camera, eye.get());
    camera_Set(camera, local_3c.get(), eye.get());

    if (m03A6 == 0 && current.pos.y > tornado->current.pos.y + 5000.0f) {
        m03A6 = 1;
        s32 exitId = gabi::ftoi(cM_rndF(8.0f)) + 0xC6;
        if (exitId >= 0xCE) {
            exitId = 0xCD;
        }
        shipTailChangeScene(exitId, 0.0f, 0, -1);
    }
    return TRUE;
}



/* 024827D4 */
BOOL daShip_c::procWhirlDown() {
    shape_angle.y = (s16)gabi::ftoi(gabi::fmadds(m0408 / 6.2831855f, 65536.0f, (f32)(s16)shape_angle.y));
    if (mWhirlActor) {
        speedF = 40.0f;
        if (cLib_addCalcPosXZ(&current.pos, &mWhirlActor->current.pos, 1.0f, 40.0f, 10.0f) < 10.0f) {
            f32 vy = speed.y - 0.5f;
            if (vy < -10.0f) {
                vy = -10.0f;
            }
            speed.y = vy;
            current.pos.y += vy;
            if (m037A == 0 && current.pos.y < m03F4 - 1000.0f) {
                /* HD: the event flag is set only with the bomb bag */
                if (dComIfGs_checkGetItem(dItemNo_BOMB_BAG_e)) {
                    dComIfGs_onEventBit(0x1940);
                }
                u32 play = dComIfGp_ea();
                shipTailSetNextStage(shipTail::startStageName(play), (s16)m03B2, (s8)fopAcM_GetRoomNo(this), -1,
                           0.0f, 0, 1, 0);
                m037A = 1;
            }
        }
    }
    return TRUE;
}



/* fopAcM_seStartCurrent(this, se, 0) (HD inline) */
static inline void ship_seStartCurrent(fopAc_ac_c* a, u32 se) {
    if (a != nullptr && gabi::ea(&a->current.pos) != 0) {
        mDoAud_seStart(se, &a->current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
    }
}

/* 0247C024 */
BOOL daShip_c::procStartModeWarp_init() {
    current.pos.y = m03F4 + 5000.0f;
    onStateFlg(daSFLG_FLY_e);
    setProc(shipFirst::procStartModeWarp);
    mCurMode = MODE_START_MODE_WARP_e;
    if (m0392 != dRes_INDEX_SHIP_BCK_FN_MAST_OFF2_e) {
        setPartOffAnime();
    }
    m03A6 = 0x1C25;
    u32 camera = ship_getCameraAddress(dComIfGp_getPlayerCameraID0());
    camera_Stop(camera);
    gabi::Local<cXyz> local_38;
    local_38->y = m03F4 + 1500.0f;
    local_38->x = current.pos.x;
    local_38->z = current.pos.z + 2000.0f;
    gabi::Local<cXyz> pos;
    pos->set(current.pos.x, current.pos.y, current.pos.z);
    camera_Set(camera, pos.get(), local_38.get());
    m037A = 0;
    ship_seStartCurrent(this, 0x186D);
    return TRUE;
}



/* 02481FAC */
BOOL daShip_c::procStartModeWarp() {
    fopAc_ac_c* tornado = fopAcM_SearchByID(mTactWarpID);
    u32 camera = ship_getCameraAddress(dComIfGp_getPlayerCameraID0());
    shape_angle.y += m03A6;
    current.angle.y = shape_angle.y;
    gabi::Local<cXyz> local_30;
    if (tornado) {
        if (!checkStateFlg(daSFLG_UNK8_e)) {
            onStateFlg(daSFLG_UNK8_e);
            shipTail::copyWord(&tornado->current.pos.x, &current.pos.x);
            shipTail::copyWord(&tornado->current.pos.z, &current.pos.z);
        }
        s32 jointIndex = tornado_findJoint(tornado, &current.pos.y);
        cLib_chaseF(&current.pos.x, tornado_getJointPos(tornado, jointIndex, 0), 50.0f);
        cLib_chaseF(&current.pos.z, tornado_getJointPos(tornado, jointIndex, 2), 50.0f);
        shipTail::copyWord(&local_30->x, &tornado->current.pos.x);
        shipTail::copyWord(&local_30->z, &tornado->current.pos.z);
    } else {
        shipTail::copyWord(&local_30->x, &current.pos.x);
        shipTail::copyWord(&local_30->z, &current.pos.z);
    }
    shipTail::copyWord(&local_30->y, &current.pos.y);
    gabi::Local<cXyz> eye;
    camera_Eye(camera, eye.get());
    camera_Set(camera, local_30.get(), eye.get());
    if (m037A == 0) {
        if (current.pos.y < m03F4) {
            current.pos.y = m03F4 - 50.0f;
            m037A = 1;
            offStateFlg(daSFLG_FLY_e);
            speed.y = 0.0f;
            if (tornado != nullptr) {
                shipTail::setTornadoScaleOn(gabi::ea(tornado), 1); /* setScaleOn() */
            }
            gabi::Local<GXColor> diff;
            gabi::Local<GXColor> amb;
            dKy_get_seacolor(diff.get(), amb.get());
            m03BC.z = 0;
            m03BC.x = 0;
            m03BC.y = shape_angle.y + 0x8000;
            dComIfGp_particle_set(shipTail::ID_AK_JN_SHIPWARPSPLASH00, &current.pos, &shape_angle, nullptr, 0xFF,
                                  (dPa_levelEcallBack*)&m1984, -1, diff.get(), nullptr, nullptr);
            dComIfGp_particle_set(shipTail::ID_AK_JN_SHIPWARPSPLASH00, &current.pos, &m03BC, nullptr, 0xFF, (dPa_levelEcallBack*)&m1998, -1, diff.get(),
                                  nullptr, nullptr);
        }
        if (tornado != nullptr) {
            tornado->current.pos.y = current.pos.y - 700.0f;
        }
    } else if (tornado == nullptr || shipTail::tornadoScale(gabi::ea(tornado)) < 0.8f /* getSmallScaleEnd() */) {
        if (tornado != nullptr) {
            tornado->current.pos.y -= 12.0f;
        }
        m03BC.y = shape_angle.y + 0x8000;
        cLib_chaseS(&m03A6, 0, 0x40);
        if (m03A6 < 0x400) {
            camera_Start(camera);
            camera_Reset(camera);
            mTactWarpID = fpcM_ERROR_PROCESS_ID_e;
            /* HD: the effects are ended, not removed */
            dPa_followEcallBack_end_l(&m1984);
            dPa_followEcallBack_end_l(&m1998);
            dComIfGp_evmng_cutEnd(mEvtStaffId);
            procPaddleMove_init();
        }
    }
    return TRUE;
}



/* 0247DBF8 */
BOOL daShip_c::procTactWarp_init() {
    mCurMode = MODE_TACT_WARP_e;
    setProc(shipFirst::procTactWarp);
    if (m0392 != dRes_INDEX_SHIP_BCK_FN_MAST_OFF2_e) {
        setPartOffAnime();
    }
    m03A6 = 0;
    gravity = 0.0f;
    speed.y = 0.0f;
    onStateFlg(daSFLG_FLY_e);
    m037A = 0;
    ship_seStartCurrent(this, 0x186E);
    shipFirstStartEventCamera(dCam_getBody(), shipFirst::CAMERA_EVENT_TACT_WARP, fopAcM_GetID(this), 0);
    gabi::Local<GXColor> amb;
    gabi::Local<GXColor> diff;
    dKy_get_seacolor(amb.get(), diff.get());
    m03BC.z = 0;
    m03BC.x = 0;
    m03BC.y = shape_angle.y + 0x8000;
    dComIfGp_particle_set(shipTail::ID_AK_JN_SHIPWARPSPLASH00, &current.pos, &shape_angle, nullptr, 0xFF,
                          (dPa_levelEcallBack*)&m1984, -1, amb.get(), nullptr, nullptr);
    dComIfGp_particle_set(shipTail::ID_AK_JN_SHIPWARPSPLASH00, &current.pos, &m03BC, nullptr, 0xFF, (dPa_levelEcallBack*)&m1998, -1, amb.get(), nullptr,
                          nullptr);
    return TRUE;
}



/* 02482438 */
BOOL daShip_c::procTactWarp() {
    shape_angle.y += m03A6;
    current.angle.y = shape_angle.y;
    fopAc_ac_c* tornado = fopAcM_SearchByID(mTactWarpID);
    if (tornado == nullptr || fpcM_IsCreating(mTactWarpID)) {
        if (mTactWarpID == fpcM_ERROR_PROCESS_ID_e) {
            dComIfGp_event_reset();
            dPa_followEcallBack_end_l(&m1984);
            dPa_followEcallBack_end_l(&m1998);
            procPaddleMove_init();
            shipTailEndEventCamera(dCam_getBody(), fopAcM_GetID(this));
        }
        return FALSE;
    }
    if (cLib_chaseS(&m03A6, 0x1C25, 0x40) && shipTail::tornadoScale(gabi::ea(tornado)) > 0.8f /* getScaleEnd() */) {
        f32 vy = speed.y + 1.0f;
        if (vy > 50.0f) {
            vy = 50.0f;
        }
        speed.y = vy;
        current.pos.y += vy;
        tornado->current.pos.y = current.pos.y - 700.0f;
        dPa_followEcallBack_end_l(&m1984);
        dPa_followEcallBack_end_l(&m1998);
    } else if (!m037A && m03A6 > 0x1000) {
        m037A = 1;
        shipTail::setTornadoScaleOn(gabi::ea(tornado), 1); /* setScaleOn() */
    }
    if (m037A == 1) {
        cLib_chaseF(&tornado->current.pos.y, current.pos.y - 700.0f, 20.0f);
        s32 jointIndex = tornado_findJoint(tornado, &current.pos.y);
        cLib_chaseF(&current.pos.x, tornado_getJointPos(tornado, jointIndex, 0), 50.0f);
        cLib_chaseF(&current.pos.z, tornado_getJointPos(tornado, jointIndex, 2), 50.0f);
    }
    if (m037A != 2 && current.pos.y > m03F4 + 5000.0f) {
        m037A = 2;
        dStage_changeScene(mTactWarpPosNum + 0xC5, 0.0f, 0, -1);
        shipTailTactMuteOff();
    }
    m03BC.y = shape_angle.y + 0x8000;
    return TRUE;
}



/* 0247C178 */
BOOL daShip_c::procStartModeThrow_init() {
    current.pos.y = m03F4 + 2500.0f;
    speedF = 100.0f;
    onStateFlg(daSFLG_FLY_e);
    setProc(shipFirst::procStartModeThrow);
    mCurMode = MODE_START_MODE_THROW_e;
    if (m0392 != dRes_INDEX_SHIP_BCK_FN_MAST_OFF2_e) {
        setPartOffAnime();
    }
    m03A6 = 3600; /* l_HIO.throw_return_angle_speed */
    u32 camera = ship_getCameraAddress(dComIfGp_getPlayerCameraID0());
    camera_Stop(camera);
    gabi::Local<cXyz> pos;
    pos->set(current.pos.x, current.pos.y, current.pos.z);
    gabi::Local<cXyz> local_38;
    local_38->x = gabi::fmadds(300.0f, cM_scos(current.angle.y), current.pos.x);
    local_38->y = m03F4 + 150.0f;
    local_38->z = gabi::fnmsubs(300.0f, cM_ssin(current.angle.y), current.pos.z);
    camera_Set(camera, pos.get(), local_38.get());
    current.pos.x = gabi::fnmsubs(4500.0f, cM_ssin(current.angle.y), current.pos.x);
    current.pos.z = gabi::fnmsubs(4500.0f, cM_scos(current.angle.y), current.pos.z);
    m037A = 0;
    return TRUE;
}



/* 02482970 */
BOOL daShip_c::procStartModeThrow() {
    u32 camera = ship_getCameraAddress(dComIfGp_getPlayerCameraID0());
    shape_angle.y += m03A6;
    gabi::Local<cXyz> pos;
    pos->set(current.pos.x, current.pos.y, current.pos.z);
    gabi::Local<cXyz> eye;
    camera_Eye(camera, eye.get());
    camera_Set(camera, pos.get(), eye.get());
    if (m037A == 0) {
        if (current.pos.y < m03F4) {
            m037A++;
            speed.y = 0.0f;
            speedF = 0.0f;
            offStateFlg(daSFLG_FLY_e);
            gabi::Local<GXColor> amb;
            gabi::Local<GXColor> diff;
            dKy_get_seacolor(amb.get(), diff.get());
            u8 r = (u8)gabi::ftoi(gabi::fmadds(0.23529412f, (f32)(u8)amb->r, 195.0f));
            u8 g = (u8)gabi::ftoi(gabi::fmadds(0.23529412f, (f32)(u8)amb->g, 195.0f));
            current.pos.y = m03F4;
            u8 b = (u8)gabi::ftoi(gabi::fmadds(0.23529412f, (f32)(u8)amb->b, 195.0f));
            amb->r = r;
            amb->g = g;
            amb->b = b;
            dComIfGp_particle_set(shipTail::ID_IT_SN_FN_SHIBUKI00, &current.pos, &shape_angle, nullptr, 0xFF, nullptr, -1,
                                  amb.get(), nullptr, nullptr);
            dComIfGp_particle_set(shipTail::ID_IT_SN_FN_SHIBUKI01, &current.pos, &shape_angle, nullptr, 0xFF, nullptr, -1,
                                  amb.get(), nullptr, nullptr);
            current.pos.y = m03F4 - 50.0f;
        }
    } else if (cLib_chaseS(&m03A6, 0, 0x100)) {
        m037A++;
        if (m037A == 45) {
            camera_Start(camera);
            camera_Reset(camera);
            current.angle.y = shape_angle.y;
            dComIfGp_evmng_cutEnd(mEvtStaffId);
            procPaddleMove_init();
        }
    }
    return TRUE;
}



/* 0247AE7C */
static BOOL daShip_IsDelete(daShip_c*) {
    return TRUE;
}



/* 0247AE84 */
BOOL daShip_c::shipDelete() {
    shipFirstWaveRemove((ShipWave*)mWaveL);
    shipFirstWaveRemove((ShipWave*)mWaveR);
    shipFirstSplashRemove((ShipSplash*)mSplash);
    shipFirstTrackRemove((ShipTrack*)mTrack);
    /* HD: the ripples are ended (GameCube remove()) */
    dPa_rippleEcallBack_end((dPa_rippleEcallBack*)mRipple);
    m1984.remove();
    m1998.remove();
    m19AC.remove();
    dPa_rippleEcallBack_end((dPa_rippleEcallBack*)m19C0);
    mDoAud_seDeleteObject(&mTillerTopPos);
    mDoAud_seDeleteObject(&m0444);
    mDoAud_seDeleteObject(&m102C);
    mDoAud_seDeleteObject(&mCraneRipplePos);
    mDoAud_seDeleteObject(&m1038);
    /* dComIfGp_clearPlayerStatus1(0, daPyStts1_SAIL_e) */
    u32 play = dComIfGp_ea();
    shipFirst::clearSailStatus(play);
    dComIfG_resDelete(&mPhs, STR(l_arcName));
    return TRUE;
}



/* 0247AF74 */
static BOOL daShip_Delete(daShip_c* i_this) {
    i_this->shipDelete();
    return TRUE;
}

/* WWHD glue
ext 02481038 BOOL shipTailNextMessage(daShip_c* ship)
ext 025C3748 void shipTailChangeScene(s32 exit, f32 speed, s32 mode, s32 room)
ext 0253E860 void shipTailEndEventCamera(u32 camera, u32 actorID)
ext 025E1E88 void shipTailTactMuteOff()
ext 0252012C void shipTailSetNextStage(u32 stage, s16 point, s8 room, s32 layer, f32 speed, s32 wipe, s32 reset, s32 flags)
str 1003A724 "SE_SAIL_POWER_UP" in 0247E854
str 1003A768 "pos" in 024800A8
str 1003A770 "angle" in 024800A8
str 1003A778 "speed" in 024800A8
str 1003A784 "part" in 024800A8
str 1003A78C "talk" in 024800A8
str 1003A7A4 "atn_actor" in 024800A8
str 1003A758 "atn_pos" in 024800A8
str 1003A794 "tact" in 024800A8
str 1003A76C "rad" in 024800A8
str 1003A79C "prm0" in 024800A8
str 1003A760 "gravity" in 024800A8
ext 025A92C0 void shipFirstWaveRemove(ShipWave* callback)
ext 025A99B8 void shipFirstSplashRemove(ShipSplash* callback)
ext 025A9E38 void shipFirstTrackRemove(ShipTrack* callback)
ext 023F4CB0 void shipFirstSetDamageRupee(u32 link, f32 damage)
ext 023F9D1C void shipFirstSetDamagePointWait(u32 link, f32 damage)
ext 02516360 u32 shipFirstGetTgHitGObj(dCcD_GObjInf* object)
ext 0253E70C void shipFirstStartEventCamera(u32 camera, u32 mode, u32 actorID, u32 flags)
data ship_obstacleGuard 1046DDE8
data ship_obstacleOffset 1046DCEC
ext 025CB374 void shipNative_startShock(u32 vibration, s32 a, s32 b, cXyz* v)
ext 025CB408 void shipNative_startQuake(u32 vibration, s32 a, s32 b, cXyz* v)
ext 025CB610 void shipNative_stopQuake(u32 vibration, s32 a)
ext 027200D0 u32 shipNative_getCameraOptions(u32 options)
ext 02514F50 void shipNative_cameraSet(u32 body, cXyz* center, cXyz* eye)
ext 02514F2C void shipNative_cameraStop(u32 body)
ext 02514F38 void shipNative_cameraStart(u32 body)
ext 02515048 void shipNative_cameraReset(u32 body)
ext 02008B4C void shipNative_destroyLineCheck(void* check, s32 deleteFlag)
ext 02526E70 u32 shipNative_getDemoActor(u32 demo, u8 id)
ext 024832E0 void daShip_c::initStartPos(cXyz* pos, s16 angle)
ext 0201AEAC void shipHD_divideVector(cXyz* result, cXyz* source, f32 divisor)
ext 024EEEB8 s32 shipHD_getPolyColor(dBgS* bg, cBgS_PolyInfo* poly)
ext 024EF0BC s32 shipHD_getGroundCode(dBgS* bg, cBgS_PolyInfo* poly)
ext 024EF130 s32 shipHD_getRoomId(dBgS* bg, cBgS_PolyInfo* poly)
ext 025445B8 BOOL shipHD_startCheckOld(u32 events, u32 name)
ext 025AB070 u32 shipHD_nextRoomPath(u32 path, s32 room)
ext 025E1A40 u32 shipHD_startSound(u32 sound, cXyz* position, u32 flags, s32 reverb)
ext 0274D458 void shipHD_convertSeaColor(void* result, void* source, f32 rate)
ext 027F0E04 void shipHD_entryImmediate(u32 buffer, u8* packet, s32 sort)
ext 027F55FC void shipHD_drawSightModel(u32 model)
ext 027F9F0C u32 shipHD_getShaderColor(u32 material, s32 index)
ext 027FB678 void shipHD_updateSightSegment(u32 segment)
ext 028E8DAC void shipHD_subtractVector(cXyz* result, u32 source, cXyz* subtract)
ext 0200ECD4 f32 cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep)
ext 0200F4FC BOOL cLib_chaseUC(be<u8>* v, u8 target, u8 step)
ext 025602F0 void dKy_get_seacolor(GXColor* amb, GXColor* dif)
ext 025F181C void mDoMtx_ZrotS(Mtx34* m, s16 z)
ext 025F18EC void mDoMtx_XrotS_l(Mtx34* m, s16 x)
ext 028E9108 void PSMTXConcat_s(const Mtx34* a, const Mtx34* b, Mtx34* ab)
ext 028E945C void PSMTXScale(Mtx34* m, f32 x, f32 y, f32 z)
ext 028E91EC void PSMTXInverse(const Mtx34* src, Mtx34* dst)
ext 025F1C90 void mDoMtx_lookAt(Mtx34* m, const cXyz* eye, const cXyz* center, s16 bank)
ext 027DF9B0 s32 JUTNameTab_getIndex_s(u32 tab, u32 name)
ext 027FA7F8 u32 hd_matGetTexSrt(u32 mat, s32 i)
ext 027FA678 u32 hd_matGetTexMtxSlot(u32 mat, s32 i)
ext 025EC62C void mDoExt_3DlineMat1_update(mDoExt_3DlineMat1_c* l, u16 segs, f32 size, const GXColor* color, u16 space, dKy_tevstr_c* tev)
ext 0257DAA8 cXyz* dKyw_get_wind_vec()
ext 0257DB04 be<f32>* dKyw_get_wind_power()
ext 0257E490 void dKyw_tact_wind_set(s16 x, s16 y)
ext 02030B38 void hd_seStartByName(u32 name)
ext 02007CDC BOOL CPad_R_LOCK_BUTTON(s32 pad)
ext 020078E8 BOOL CPad_CHECK_TRIG_X(s32 pad)
ext 02007914 BOOL CPad_CHECK_TRIG_Y(s32 pad)
ext 02007814 BOOL CPad_CHECK_TRIG_Z(s32 pad)
ext 02007898 BOOL CPad_CHECK_TRIG_HD1(s32 pad)
ext 0200770C BOOL CPad_CHECK_HOLD_X(s32 pad)
ext 02007738 BOOL CPad_CHECK_HOLD_Y(s32 pad)
ext 02007638 BOOL CPad_CHECK_HOLD_Z(s32 pad)
ext 020076BC BOOL CPad_CHECK_HOLD_HD1(s32 pad)
ext 024EDFCC BOOL dAttention_LockonTruth(u32 att)
ext 024EE058 u32 dAttention_GetLockonList(u32 att, s32 i)
ext 024EBA14 fopAc_ac_c* dAttList_getActor(u32 l)
ext 025E1988 void mDoAud_seStart_simple(u32 id)
ext 02007D4C BOOL CPad_R_LOCK_TRIGGER(s32 pad)
ext 025F7DB0 u32 fopMsgM_messageSet(u32 mng, u32 msgNo, cXyz* pos)
ext 025F795C s32 msgMng_getStatus(u32 mng)
ext 025F74D0 void msgMng_setStatus(u32 mng, s32 st)
ext 0201B31C void cXyz_normalize(cXyz* v, cXyz* res)
ext 027F2BC0 void J3DFrameCtrl_init(J3DFrameCtrl* f, s32 end)
ext 020076E0 BOOL CPad_CHECK_HOLD_A(s32 pad)
ext 02007C50 f64 CPad_GET_ANALOG_R(s32 pad)
ext 0200FA40 void cLib_offsetPos(cXyz* out, const cXyz* base, s16 angle, const cXyz* ofs)
ext 0200F564 BOOL cLib_chaseS(be<s16>* v, s16 target, s16 step)
ext 025DD868 BOOL fpcM_IsCreating(u32 id)
ext 025A5AC8 void dPa_followEcallBack_end_l(dPa_followEcallBack* cb)
ext 025C3748 void dStage_changeScene(s32 exit, f32 speed, u32 mode, s8 roomNo)
ext 0201971C f32 cM_atan2f(f32 y, f32 x)
ext 024F8044 u32 dCam_getBody()
ext 02514EE4 void dCamera_SetTypeForce(u32 cam, u32 name, fopAc_ac_c* a)
ext 0244300C BOOL daPy_shipSpecialDemoStart(fopAc_ac_c* link)
ext 0200EF78 f32 cLib_addCalcPosXZ(cXyz* p, const cXyz* t, f32 scale, f32 maxStep, f32 minStep)
 data ship_drawBuffer 104B4634
 data ship_viewMtx 104B45F8
 data ship_sightMtx 104A03EC
 data ship_counter 101FF560
 data ship_ropeGravity 1047C054
 data ship_stayNo 1047E6C8
 data ship_ropeColor 101D03F0
 data ship_ropeBase 1046DCE0
 data ship_zeroPoint 101D03F4
 str 1003A34C "Siren" in 02471E54
 str 1003A34C "Siren" in 02471E94
 str 1003A384 "m_fn_main_hashi" in 0247255C
 str 1003A394 "d_a_ship.cpp" in 0247255C
 str 1003A3A4 "srt != (0)" in 0247255C
 str 1003A3E8 "GanonA" in 02473454
 str 1003A3F0 "GanonB" in 02473454
 str 1003A3F8 "GanonC" in 02473454
 str 1003A400 "GanonD" in 02473454
 str 1003A408 "GanonE" in 02473454
 str 1003A410 "GanonN" in 02473454
 str 1003A418 "GanonM" in 02473454
 str 1003A420 "GanonL" in 02473454
 str 1003A428 "GanonJ" in 02473454
 str 1003A430 "Xboss0" in 02473454
 str 1003A438 "Xboss1" in 02473454
 str 1003A440 "Xboss2" in 02473454
 str 1003A448 "Xboss3" in 02473454
 str 1003A450 "GanonK" in 02473454
 str 1003A458 "GTower" in 02473454
 str 1003A460 "Hyrule" in 02473454
 str 1003A468 "Hyroom" in 02473454
 str 1003A3E0 "kenroom" in 02473454
 str 1003A3C4 "Tornado" in 02472C04
 str 1003A3D4 "Tornado" in 02472F14
 str 1003A4CC "SV_TALK_P1_1ST" in 02475DA8
 str 1003A4DC "SV_TALK_P1_2ND" in 02475DA8
 str 1003A4EC "SV_TALK_P4_1ST" in 02475DA8
 data front 1046DD34
 data frontGuard 1046DDA4
 data back 1046DD40
 data backGuard 1046DDA8
 data right 1046DD4C
 data rightGuard 1046DDAC
 data left 1046DD58
 data leftGuard 1046DDB0
 data rippleScale 1046DD64
 data rippleScaleGuard 1046DDB4
 data waveLeftDirection 1046DDC0
 data waveLeftDirectionGuard 1046DDB8
 data waveRightDirection 1046DDCC
 data waveRightDirectionGuard 1046DDBC
 data ship_ropeOffset 101D03CC
 data ship_waterDropScale 101D03D8
 data shadowPositions 1046DCF8
 data shadowPosGuard 1046DDA0
*/
