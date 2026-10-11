/**
 * d_a_npc_photo_exec.cpp (WWHD)
 * NPC - Lenzo: attention, animation, execute and the move procedures.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_photo.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_photo.h"

/* 022CCD4C */
void daNpcPhoto_c::chkAttention() {
    if (mEventCut.mbAttention) { /* getAttnFlag() */
        mLookAtPos.z = mEventCut.mPos.z;
        field_0x9D6 = 1;
        mLookAtPos.x = mEventCut.mPos.x;
        mLookAtPos.y = mEventCut.mPos.y;
        if (field_0x9D7) {
            field_0x994 = false;
            m_jnt.mbTrn = 1; /* setTrn() */
        } else {
            field_0x994 = true;
        }
        if (!field_0x9BD) {
            field_0x9BD = true;
        }
    } else {
        fopAc_ac_c* link = dComIfGp_getLinkPlayer();
        gabi::Local<cXyz> pos;
        pos->z = current.pos.z;
        pos->y = current.pos.y;
        pos->x = current.pos.x;
        gabi::Local<cXyz> lpos;
        lpos->x = link->current.pos.x;
        lpos->y = link->current.pos.y;
        f32 temp = l_npc_dat.field_0x2C;
        lpos->z = link->current.pos.z;
        s32 temp2 = field_0x9B2;
        gabi::Local<be<f32>> temp3;
        gabi::Local<be<s16>> angle;
        dNpc_calc_DisXZ_AngY(pos.get(), lpos.get(), temp3.get(), angle.get());
        if (field_0x9BD) {
            temp += 40.0f;
            temp2 += 0x71C;
        }
        s16 ang = (s16)(*angle - shape_angle.y);
        *angle = ang;
        s32 abs_ang = ang < 0 ? -ang : ang;
        if ((temp > *temp3 && temp2 > abs_ang) || field_0x9CD) {
            gabi::Local<cXyz> eye;
            dNpc_playerEyePos(eye.get(), l_npc_dat.field_0x00);
            field_0x994 = (field_0x9D7 == 0);
            mLookAtPos.copy(*eye);
            field_0x9D6 = 1;
            if (!field_0x9D8) {
                field_0x9BA = field_0x9AE;
                field_0x994 = false;
                field_0x9D6 = 2;
                m_jnt.mbTrn = 1;
            }
            if (!field_0x9BD) {
                field_0x9BD = true;
            }
        } else {
            u8 bd = field_0x9BD;
            u8 c0 = field_0x9C0;
            if (bd == 1) {
                field_0x9BD = false;
                field_0x9AC = 30;
            }
            field_0x9D6 = 0;
            if (c0 == 0 && field_0x9C1 != 3) {
                if (field_0x9AC != 0) {
                    field_0x9AC = field_0x9AC - 1;
                } else {
                    field_0x9BA = field_0x9AE;
                    field_0x994 = false;
                    field_0x9D6 = 2;
                    m_jnt.mbTrn = 1;
                }
            }
        }
    }
    field_0x9B6 = l_npc_dat.field_0x16;
}
/* 022CCFAC */
void daNpcPhoto_c::setAnm(u32 param_1, int param_2, f32 param_3) {
    if (!(field_0x98C < 0.0f)) {
        param_3 = field_0x98C;
        field_0x98C = -1.0f;
    }
    J3DAnmTransform* pAnmRes = (J3DAnmTransform*)dComIfG_getObjectIDRes(l_arcname_tbl[0], l_bck_ix_tbl[param_1]);
    mpMorf->setAnm(pAnmRes, param_2, param_3, 1.0f, 0.0f, -1.0f, nullptr);
    field_0x9C8 = param_1;
}
/* 022CD0B8 */
bool daNpcPhoto_c::setAnmTbl(sPhotoAnmDat* i_anmDat) {
    if (field_0x9C9 & 4) {
        return false;
    }
    if (i_anmDat->field_0x00 == 0xFF) {
        mpAnmDat = nullptr;
        return true;
    }
    mpAnmDat = i_anmDat;
    field_0x9CA = (s8)i_anmDat->field_0x02;
    u8 no = i_anmDat->field_0x00;
    if (field_0x9CA > 0) {
        setAnm(no, 0, (f32)i_anmDat->field_0x01);
    } else if (field_0x9C8 != no) {
        setAnm(no, 2, (f32)i_anmDat->field_0x01);
    }
    return false;
}
/* 022CD190 */
void daNpcPhoto_c::executeSetMode(u32 param_1) {
    field_0x984 = 0.0f;
    switch (param_1) {
    case 0: {
        setAnmTbl(l_npc_anm_wait);
        f32 r = cM_rndF((f32)(s32)(l_npc_dat.field_0x4C - l_npc_dat.field_0x4A));
        field_0x9A8 = (s16)gabi::ftoi(r + (f32)(s32)l_npc_dat.field_0x4A);
        break;
    }
    case 2: {
        setAnmTbl(l_npc_anm_walk);
        f32 r = cM_rndF((f32)(s32)(l_npc_dat.field_0x50 - l_npc_dat.field_0x4E));
        field_0x9A8 = (s16)gabi::ftoi(r + (f32)(s32)l_npc_dat.field_0x4E);
        break;
    }
    case 3: {
        gabi::Local<cXyz> point;
        mPathRun.getPoint(point.get(), mPathRun.mIdx);
        gabi::Local<cXyz> pos;
        pos->y = current.pos.y;
        pos->x = current.pos.x;
        gabi::Local<cXyz> pt;
        pt->y = point->y;
        pos->z = current.pos.z;
        pt->x = point->x;
        pt->z = point->z;
        gabi::Local<be<s16>> angle;
        dNpc_calc_DisXZ_AngY(pos.get(), pt.get(), nullptr, angle.get());
        if (*angle == current.angle.y) {
            param_1 = 2;
            setAnmTbl(l_npc_anm_walk);
            f32 r = cM_rndF((f32)(s32)(l_npc_dat.field_0x50 - l_npc_dat.field_0x4E));
            field_0x9A8 = (s16)gabi::ftoi(r + (f32)(s32)l_npc_dat.field_0x4E);
        }
        break;
    }
    }
    field_0x9C0 = param_1;
}
/* 022CED84 */
void daNpcPhoto_c::playTexPatternAnm() {
    if (!(field_0x9C9 & 0x80) && cLib_calcTimer(&mTimer) == 0) {
        s32 frame_max = J3DAnm_getFrameMax(m_head_tex_pattern);
        if ((s32)mFrame >= frame_max) {
            s32 max = J3DAnm_getFrameMax(m_head_tex_pattern);
            mTimer = 0x78;
            mFrame = mFrame - max;
        } else {
            mFrame = mFrame + 1;
        }
    }
}
/* 022CEE30 */
void daNpcPhoto_c::playAnm() {
    field_0x9C9 = field_0x9C9 & 0xFE;
    u32 mtrlSndId;
    if (mObjAcch.ChkGroundHit()) {
        mtrlSndId = dBgS_GetMtrlSndId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd) /* m_gnd */);
    } else {
        mtrlSndId = 0;
    }
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    if (mpMorf->play(&eyePos, mtrlSndId, reverb)) {
        field_0x9C9 = field_0x9C9 & ~0x4;
        if (mpAnmDat.get() != nullptr) {
            if (field_0x9CA > 0) {
                field_0x9CA = field_0x9CA - 1;
                if (field_0x9CA == 0) {
                    mpAnmDat = mpAnmDat.get() + 1;
                    if (setAnmTbl(mpAnmDat)) {
                        field_0x9C9 = field_0x9C9 | 1;
                    }
                } else {
                    setAnm(mpAnmDat->field_0x00, 0, 0.0f);
                }
            }
        }
    }
}
/* 022CEF70 */
void daNpcPhoto_c::lookBack() {
    s16 target = field_0x9B6;
    s16 desiredYRot = current.angle.y;
    cXyz* dstTemp = nullptr;
    gabi::Local<cXyz> temp2;
    /* cXyz dstPos = eyePos (through FPRs) */
    f32 eye_x = eyePos.x;
    f32 eye_y = eyePos.y;
    f32 eye_z = eyePos.z;
    u8 temp3 = field_0x994;
    switch ((u32)(s32)field_0x9D6) {
    case 1:
        temp2->copy(mLookAtPos);
        dstTemp = temp2.get();
        break;
    case 2:
        desiredYRot = field_0x9BA;
        break;
    }
    if (field_0x9BC && field_0x9D7) {
        temp3 = false;
        m_jnt.mbTrn = 1; /* setTrn() */
    }
    if (m_jnt.mbTrn != 0) { /* trnChk() */
        if (mEventCut.mTurnSpeed != 0) {
            target = mEventCut.mTurnSpeed;
        }
        cLib_addCalcAngleS2(&field_0x9B8, target, 4, 0x800);
    } else {
        field_0x9B8 = 0;
    }
    gabi::Local<cXyz> dstPos;
    dstPos->x = eye_x;
    dstPos->y = eye_y;
    dstPos->z = eye_z;
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstTemp, dstPos.get(), desiredYRot, field_0x9B8, temp3);
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
}
/* 022CF17C */
bool daNpcPhoto_c::_execute() {
    NpcDatStruct& dat = l_npc_dat;
    m_jnt.setParam(dat.field_0x08, dat.field_0x0A, dat.field_0x10, dat.field_0x12, dat.field_0x04, dat.field_0x06,
                   dat.field_0x0C, dat.field_0x0E, dat.field_0x14);
    chkAttention();
    f32 y = current.pos.y + dat.field_0x30;
    mEyePos.z = current.pos.z;
    mEyePos.x = current.pos.x;
    mEyePos.y = y;
    checkOrder();
    if (!dComIfGp_event_runCheck() || (eventInfo_getCommand(this) == dEvtCmd_INTALK_e && field_0x9C7)) {
        ptmf_call(0x101C506C + field_0x9C0 * 8, this); /* (this->*moveProc[field_0x9C0])() */
    } else {
        eventMove();
    }
    eventOrder();
    playTexPatternAnm();
    playAnm();
    if (field_0x9C8 == 3) {
        cLib_chaseF(&speedF, field_0x984, 0.1f);
        f32 temp = speedF * dat.field_0x38;
        mDoExt_McaMorf* morf = mpMorf;
        if (temp < 0.5f) {
            temp = 0.5f;
        }
        morf->setPlaySpeed(temp);
    } else {
        cLib_chaseF(&speedF, field_0x984, 0.5f);
    }
    fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
    mObjAcch.CrrPos(dComIfG_Bgsp());
    gabi::Local<cXyz> center;
    cXyz_pl(&current.pos, center.get(), &field_0x958);
    setCollision(&mCyl, center.get(), field_0x988, 150.0f);
    if (field_0x9C1 == 2) {
        for (int i = 0; i < 2; i++) {
            cXyz* counterPos = &l_counter_pos[i];
            gabi::Local<cXyz> pos;
            pos->x = counterPos->x;
            pos->y = counterPos->y;
            pos->z = counterPos->z;
            setCollision(&field_0x6F8[i], pos.get(), 110.0f, 150.0f);
        }
    }
    gabi::Local<cXyz> temp;
    temp->x = dat.field_0x1C;
    temp->y = dat.field_0x20;
    temp->z = dat.field_0x24;
    mDoMtx_YrotS(mDoMtx_stack_c::get(), current.angle.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), temp.get(), temp.get());
    PSVECAdd(temp.get(), &current.pos, temp.get());
    eyePos.copy(mEyePos);
    attention_info.position.copy(*temp); /* attention_info.position */
    lookBack();
    setMtx();
    return false;
}
/* 022CF530 */
static BOOL daNpc_PhotoExecute(void* i_this) {
    return static_cast<daNpcPhoto_c*>(i_this)->_execute();
}
/* dSnap_Obj (0x34, out-of-line constructor) */
struct dSnap_Obj {
    u8 _00[0x34];
};


/* PsoData (l_pso_photo, .data 0x101C5054) */


/* 022CF534 */
bool daNpcPhoto_c::_draw() {
    J3DModel* model = mpMorf->getModel();
    J3DModelData* model_data = model->getModelData();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    mDoExt_btpAnm_entry(&mBtpAnm, model_data, mFrame);
    mpMorf->updateDL();
    gabi::object_at<PhotoModelMaterialState>(gabi::ea(model_data)).mTexturePattern = 0; /* mBtpAnm.remove(model_data) */
    // HD: no dComIfGd_setShadow
    if (field_0x9C1 == 4) {
        gabi::Local<dSnap_Obj> obj;
        dSnap_Obj_ct(obj.get());
        PsoData* pso = &l_pso_photo; /* l_pso_photo */
        gabi::Local<cXyz> temp;
        temp->x = pso->field_0x00;
        temp->y = pso->field_0x04;
        temp->z = pso->field_0x08;
        PSVECAdd(temp.get(), &current.pos, temp.get());
        dSnap_Obj_SetInf(obj.get(), DSNAP_TYPE_UNK05, this, pso->field_0x16, pso->field_0x17, 0x7FFF);
        dSnap_Obj_SetGeo(obj.get(), temp.get(), pso->field_0x0C, pso->field_0x10, pso->field_0x14 + current.angle.y);
        dSnap_RegistSnapObj(obj.get());
    } else {
        dSnap_RegistFig(DSNAP_TYPE_NPC_PHOTO, this, 1.0f, 1.0f, 1.0f);
    }
    return true;
}
/* 022CF66C */
static BOOL daNpc_PhotoDraw(void* i_this) {
    return static_cast<daNpcPhoto_c*>(i_this)->_draw();
}
/* 022CF670 */
u8 daNpcPhoto_c::executeCommon() {
    field_0x9BE = field_0x9BD ? 1 : 0;
    if (field_0x9BC == 1) {
        executeSetMode(1);
    }
    return field_0x9BC;
}
static inline bool photo_isPlayer(fopAc_ac_c* ac) { return ac != nullptr && ac->mProfName == fpcNm_PLAYER_e; }

/* 022CF6C4 */
void daNpcPhoto_c::executeWait() {
    if (executeCommon()) {
        return;
    }
    field_0x9CD = false;
    field_0x958.x = 0.0f;
    field_0x958.z = 0.0f;
    field_0x958.y = 0.0f;
    field_0x988 = 60.0f;
    if (!dComIfGs_isEventBit(0x1701 /* l_save_dat.field_0x02 */)) {
        field_0x988 = 150.0f;
        field_0x958.x = 0.0f;
        field_0x958.z = l_npc_dat.field_0x34;
        field_0x958.y = 0.0f;
        if (mCyl.ChkCoHit()) {
            if (photo_isPlayer(mCyl.GetCoHitAc())) {
                field_0x9BE = 2;
            }
        }
    } else if (field_0x9C1 == 4 && !(field_0x9C6 & 0x10)) {
        fopAc_ac_c* link = dComIfGp_getLinkPlayer();
        if (link->current.pos.y < 10.0f && link->current.pos.x > -300.0f && link->current.pos.z > -500.0f) {
            field_0x9BE = 10;
        }
    } else {
        if (dComIfGp_getStartStagePoint() == 0 && !dComIfGs_isEventBit(0x1601 /* l_save_dat.field_0x04 */) && field_0x9BE != 7) {
            fopAc_ac_c* link = dComIfGp_getLinkPlayer(); // HD: read once
            if (link->current.pos.y > 400.0f && link->current.pos.x > -600.0f) {
                field_0x9BE = 7;
            }
        }
        if (field_0x9C1 == 2) {
            eventInfo_setEventId(this, -1);
            field_0x9C7 = true;
            for (int i = 0; i < 2; i++) {
                if (field_0x6F8[i].ChkCoHit()) {
                    if (photo_isPlayer(field_0x6F8[i].GetCoHitAc())) {
                        field_0x9CD = true;
                        break;
                    }
                }
            }
        }
        if (field_0x9C1 == 1 && !field_0x9BD) {
            fopAc_ac_c* link = dComIfGp_getLinkPlayer();
            if (link->current.pos.y < current.pos.y + -50.0f) {
                executeSetMode(3);
                field_0x9C1 = 3;
            }
        }
        if (field_0x9C1 == 3 && !field_0x9BD) {
            if (field_0x9A8 == 0) {
                executeSetMode(3);
            } else {
                field_0x9A8 = field_0x9A8 - 1;
            }
        }
    }
}
/* 022CFA00 */
void daNpcPhoto_c::executeTalk() {
    executeCommon();
    if (!dComIfGp_event_chkTalkXY() || dComIfGp_evmng_ChkPresentEnd()) {
        if (talk2(1) == fopMsgStts_BOX_CLOSED_e) {
            field_0x9BC = false;
            executeSetMode(0);
            u8 temp = field_0x9C6;
            if (temp & 1) {
                field_0x9C6 = temp & 0xFE;
                mItemNo = 0x26; // HD: dItemNo_DELUXE_PICTO_BOX_e (GameCube 31)
                field_0x9BE = 4;
            } else if (temp & 2) {
                field_0x9C6 = temp & ~0x02;
                mItemNo = 0x1F; /* HD (GameCube 38, event 5) */
                field_0x9BE = 4;
            } else if (temp & 4) {
                field_0x9C6 = temp & ~0x04;
                field_0x9BE = 3;
                setAnmTbl(l_npc_anm_spit /* l_npc_anm_spit */);
                field_0x9C9 = field_0x9C9 | 4;
            } else {
                field_0x9C6 = temp | 0x40;
            }
        } else {
            setAnmFromMsgTag();
        }
    }
}
/* 022CFB4C */
void daNpcPhoto_c::executeWalk() {
    if (executeCommon()) {
        return;
    }
    gabi::Local<cXyz> cur;
    cur->x = current.pos.x;
    cur->y = current.pos.y;
    cur->z = current.pos.z;
    bool temp = false;
    if (mPathRun.chkPointPass(cur.get(), mPathRun.mbDir != 0) && !mPathRun.nextIdxAuto()) {
        temp = true;
    }
    if (field_0x9BD) {
        executeSetMode(0);
    } else if (!temp) {
        gabi::Local<cXyz> point;
        mPathRun.getPoint(point.get(), mPathRun.mIdx);
        f32 px = point->x, py = point->y, pz = point->z;
        gabi::Local<cXyz> pos;
        pos->x = current.pos.x;
        pos->y = current.pos.y;
        pos->z = current.pos.z;
        gabi::Local<cXyz> pt;
        pt->x = px;
        pt->y = py;
        pt->z = pz;
        gabi::Local<be<s16>> angle;
        dNpc_calc_DisXZ_AngY(pos.get(), pt.get(), nullptr, angle.get());
        field_0x994 = false;
        field_0x9BA = *angle;
        field_0x9D6 = 2;
        field_0x9B6 = l_npc_dat.field_0x18;
        m_jnt.mbTrn = 1; /* setTrn() */
        field_0x984 = l_npc_dat.field_0x3C;
        u8 pointIndex = mPathRun.mIdx;
        if (mPathRun.mbDir) {
            pointIndex--;
        } else {
            pointIndex++;
        }
        gabi::Local<cXyz> point2;
        mPathRun.getPoint(point2.get(), pointIndex);
        if (point2->y - py > 400.0f) {
            field_0x984 = l_npc_dat.field_0x3C * l_npc_dat.field_0x40;
        }
    } else {
        executeSetMode(0);
        field_0x9C1 = 2;
        field_0x9AE = home.angle.y;
    }
}
/* 022CFD7C */
void daNpcPhoto_c::executeTurn() {
    if (executeCommon()) {
        return;
    }
    gabi::Local<cXyz> point;
    mPathRun.getPoint(point.get(), mPathRun.mIdx);
    gabi::Local<cXyz> pos;
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    pos->z = current.pos.z;
    gabi::Local<cXyz> pt;
    pt->x = point->x;
    pt->y = point->y;
    pt->z = point->z;
    gabi::Local<be<s16>> angle;
    dNpc_calc_DisXZ_AngY(pos.get(), pt.get(), nullptr, angle.get());
    u8 trn = m_jnt.mbTrn;
    field_0x994 = false;
    field_0x9D6 = 2;
    field_0x9BA = *angle;
    if (!trn) { /* !m_jnt.trnChk() */
        executeSetMode(2);
    }
}

/* WWHD glue (tools/verify/autoglue.py): data owned by d_a_npc_photo.cpp

*/
