/**
 * d_a_npc_os_a.cpp (WWHD)
 * NPC - Os, part A: event processing, action dispatch, beams, event ordering, execute, draw.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_os.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_os_a.h"

static OsSmokeLightingHD m_smoke_tevstr;
static OsWaterScaleStaticsHD l_waterScales;
static OsActionDescriptorsHD l_actionDescriptors;
static OsEventActionsHD event_init_tbl;
static OsEventActionsHD event_action_tbl;

static inline void Os_setNpcActionHD(daNpc_Os_c* t, const ProcFunc_l* pmf, void* arg) {
    gabi::Local<ProcFunc_l> fn;
    Os_copyActionHD(fn, pmf);
    t->setNpcAction(fn, arg);
}

// HD: blob shadows are removed.

// HD: lighting assignment retains each original copy width and order.
static inline void os_copy_tevstr(daNpc_Os_c* self) {
    const auto* source = reinterpret_cast<const OsSmokeLightingHD*>(&self->tevStr);
    m_smoke_tevstr.mLight.mPosition.x = (f32)source->mLight.mPosition.x;
    m_smoke_tevstr.mLight.mPosition.y = (f32)source->mLight.mPosition.y;
    m_smoke_tevstr.mLight.mPosition.z = (f32)source->mLight.mPosition.z;
    m_smoke_tevstr.mLight.mDirection.x = (f32)source->mLight.mDirection.x;
    m_smoke_tevstr.mLight.mDirection.y = (f32)source->mLight.mDirection.y;
    m_smoke_tevstr.mLight.mDirection.z = (f32)source->mLight.mDirection.z;
    m_smoke_tevstr.mLight.mColor.r = (u8)source->mLight.mColor.r;
    m_smoke_tevstr.mLight.mColor.g = (u8)source->mLight.mColor.g;
    m_smoke_tevstr.mLight.mColor.b = (u8)source->mLight.mColor.b;
    m_smoke_tevstr.mLight.mColor.a = (u8)source->mLight.mColor.a;
    m_smoke_tevstr.mLight.mColorS10.r = (u16)source->mLight.mColorS10.r;
    m_smoke_tevstr.mLight.mColorS10.g = (u16)source->mLight.mColorS10.g;
    m_smoke_tevstr.mLight.mColorS10.b = (u16)source->mLight.mColorS10.b;
    m_smoke_tevstr.mLight.mColorS10.a = (u16)source->mLight.mColorS10.a;
    m_smoke_tevstr.mLight.mCosAtten[0] = (f32)source->mLight.mCosAtten[0];
    m_smoke_tevstr.mLight.mCosAtten[1] = (f32)source->mLight.mCosAtten[1];
    m_smoke_tevstr.mLight.mCosAtten[2] = (f32)source->mLight.mCosAtten[2];
    m_smoke_tevstr.mLight.mDistAtten[0] = (f32)source->mLight.mDistAtten[0];
    m_smoke_tevstr.mLight.mDistAtten[1] = (f32)source->mLight.mDistAtten[1];
    m_smoke_tevstr.mLight.mDistAtten[2] = (f32)source->mLight.mDistAtten[2];
    m_smoke_tevstr.mLight.mCutoff = (f32)source->mLight.mCutoff;
    m_smoke_tevstr.mLight.mReferenceBrightness = (f32)source->mLight.mReferenceBrightness;
    m_smoke_tevstr.mLightPosWorld.x = (u32)source->mLightPosWorld.x;
    m_smoke_tevstr.mLightPosWorld.y = (u32)source->mLightPosWorld.y;
    m_smoke_tevstr.mLightPosWorld.z = (u32)source->mLightPosWorld.z;
    m_smoke_tevstr.mColorC0.r = (u16)source->mColorC0.r;
    m_smoke_tevstr.mColorC0.g = (u16)source->mColorC0.g;
    m_smoke_tevstr.mColorC0.b = (u16)source->mColorC0.b;
    m_smoke_tevstr.mColorC0.a = (u16)source->mColorC0.a;
    m_smoke_tevstr.mColorK0 = (u32)source->mColorK0;
    m_smoke_tevstr.mColorK1 = (u32)source->mColorK1;
    m_smoke_tevstr.mFogColor.r = (u16)source->mFogColor.r;
    m_smoke_tevstr.mFogColor.g = (u16)source->mFogColor.g;
    m_smoke_tevstr.mFogColor.b = (u16)source->mFogColor.b;
    m_smoke_tevstr.mFogColor.a = (u16)source->mFogColor.a;
    m_smoke_tevstr.mFogStartZ = (f32)source->mFogStartZ;
    m_smoke_tevstr.mFogEndZ = (f32)source->mFogEndZ;
    m_smoke_tevstr.mColpatBlend = (f32)source->mColpatBlend;
    m_smoke_tevstr.mInitTimer = (u8)source->mInitTimer;
    m_smoke_tevstr.mEnvrIdxCurr = (u8)source->mEnvrIdxCurr;
    m_smoke_tevstr.mEnvrIdxPrev = (u8)source->mEnvrIdxPrev;
    m_smoke_tevstr.mColpatCurr = (u8)source->mColpatCurr;
    m_smoke_tevstr.mColpatPrev = (u8)source->mColpatPrev;
    m_smoke_tevstr.mRoomNo = (u8)source->mRoomNo;
    m_smoke_tevstr.mEnvrIdxOverride = (u8)source->mEnvrIdxOverride;
    m_smoke_tevstr.mLightMode = (u8)source->mLightMode;
    m_smoke_tevstr.mInitType = (u8)source->mInitType;
    m_smoke_tevstr.mCachedLight.mPosition.x = (f32)source->mCachedLight.mPosition.x;
    m_smoke_tevstr.mCachedLight.mPosition.y = (f32)source->mCachedLight.mPosition.y;
    m_smoke_tevstr.mCachedLight.mPosition.z = (f32)source->mCachedLight.mPosition.z;
    m_smoke_tevstr.mCachedLight.mDirection.x = (f32)source->mCachedLight.mDirection.x;
    m_smoke_tevstr.mCachedLight.mDirection.y = (f32)source->mCachedLight.mDirection.y;
    m_smoke_tevstr.mCachedLight.mDirection.z = (f32)source->mCachedLight.mDirection.z;
    m_smoke_tevstr.mCachedLight.mColor.r = (u8)source->mCachedLight.mColor.r;
    m_smoke_tevstr.mCachedLight.mColor.g = (u8)source->mCachedLight.mColor.g;
    m_smoke_tevstr.mCachedLight.mColor.b = (u8)source->mCachedLight.mColor.b;
    m_smoke_tevstr.mCachedLight.mColor.a = (u8)source->mCachedLight.mColor.a;
    m_smoke_tevstr.mCachedLight.mColorS10.r = (u16)source->mCachedLight.mColorS10.r;
    m_smoke_tevstr.mCachedLight.mColorS10.g = (u16)source->mCachedLight.mColorS10.g;
    m_smoke_tevstr.mCachedLight.mColorS10.b = (u16)source->mCachedLight.mColorS10.b;
    m_smoke_tevstr.mCachedLight.mColorS10.a = (u16)source->mCachedLight.mColorS10.a;
    m_smoke_tevstr.mCachedLight.mCosAtten[0] = (f32)source->mCachedLight.mCosAtten[0];
    m_smoke_tevstr.mCachedLight.mCosAtten[1] = (f32)source->mCachedLight.mCosAtten[1];
    m_smoke_tevstr.mCachedLight.mCosAtten[2] = (f32)source->mCachedLight.mCosAtten[2];
    m_smoke_tevstr.mCachedLight.mDistAtten[0] = (f32)source->mCachedLight.mDistAtten[0];
    m_smoke_tevstr.mCachedLight.mDistAtten[1] = (f32)source->mCachedLight.mDistAtten[1];
    m_smoke_tevstr.mCachedLight.mDistAtten[2] = (f32)source->mCachedLight.mDistAtten[2];
    m_smoke_tevstr.mCachedLight.mCutoff = (f32)source->mCachedLight.mCutoff;
    m_smoke_tevstr.mCachedLight.mReferenceBrightness = (f32)source->mCachedLight.mReferenceBrightness;
    m_smoke_tevstr.mCachedLight2.mPosition.x = (f32)source->mCachedLight2.mPosition.x;
    m_smoke_tevstr.mCachedLight2.mPosition.y = (f32)source->mCachedLight2.mPosition.y;
    m_smoke_tevstr.mCachedLight2.mPosition.z = (f32)source->mCachedLight2.mPosition.z;
    m_smoke_tevstr.mCachedLight2.mDirection.x = (f32)source->mCachedLight2.mDirection.x;
    m_smoke_tevstr.mCachedLight2.mDirection.y = (f32)source->mCachedLight2.mDirection.y;
    m_smoke_tevstr.mCachedLight2.mDirection.z = (f32)source->mCachedLight2.mDirection.z;
    m_smoke_tevstr.mCachedLight2.mColor.r = (u8)source->mCachedLight2.mColor.r;
    m_smoke_tevstr.mCachedLight2.mColor.g = (u8)source->mCachedLight2.mColor.g;
    m_smoke_tevstr.mCachedLight2.mColor.b = (u8)source->mCachedLight2.mColor.b;
    m_smoke_tevstr.mCachedLight2.mColor.a = (u8)source->mCachedLight2.mColor.a;
    m_smoke_tevstr.mCachedLight2.mColorS10.r = (u16)source->mCachedLight2.mColorS10.r;
    m_smoke_tevstr.mCachedLight2.mColorS10.g = (u16)source->mCachedLight2.mColorS10.g;
    m_smoke_tevstr.mCachedLight2.mColorS10.b = (u16)source->mCachedLight2.mColorS10.b;
    m_smoke_tevstr.mCachedLight2.mColorS10.a = (u16)source->mCachedLight2.mColorS10.a;
    m_smoke_tevstr.mCachedLight2.mCosAtten[0] = (f32)source->mCachedLight2.mCosAtten[0];
    m_smoke_tevstr.mCachedLight2.mCosAtten[1] = (f32)source->mCachedLight2.mCosAtten[1];
    m_smoke_tevstr.mCachedLight2.mCosAtten[2] = (f32)source->mCachedLight2.mCosAtten[2];
    m_smoke_tevstr.mCachedLight2.mDistAtten[0] = (f32)source->mCachedLight2.mDistAtten[0];
    m_smoke_tevstr.mCachedLight2.mDistAtten[1] = (f32)source->mCachedLight2.mDistAtten[1];
    m_smoke_tevstr.mCachedLight2.mDistAtten[2] = (f32)source->mCachedLight2.mDistAtten[2];
    m_smoke_tevstr.mCachedLight2.mCutoff = (f32)source->mCachedLight2.mCutoff;
    m_smoke_tevstr.mCachedLight2.mReferenceBrightness = (f32)source->mCachedLight2.mReferenceBrightness;
}

static inline bool os_isPmf(ProcFunc_l* p, u32 function) {
    return p->i == -1 && p->d == 0 && p->f == function;
}
static inline void os_setEmitterScale(u32 address, u32 scaleAddress) {
    auto* emitter = gabi::at<OsEmitterScaleHD>(address);
    const auto* scale = gabi::at<const cXyz>(scaleAddress);
    f32 x = scale->x;
    emitter->mGlobalScale.x = x;
    f32 y = scale->y;
    emitter->mGlobalScale.y = y;
    f32 z = scale->z;
    emitter->mGlobalParticleScale.x = x;
    emitter->mGlobalScale.z = z;
    emitter->mGlobalParticleScale.y = y;
    emitter->mGlobalParticleScale.z = z;
}
static inline cBgS_PolyInfo_l* os_gnd(daNpc_Os_c* t) {
    return reinterpret_cast<cBgS_PolyInfo_l*>(static_cast<cBgS_PolyInfo*>(&t->mAcch.m_gnd));
} /* mAcch.m_gnd */
static inline f32 os_groundH(daNpc_Os_c* t) { return t->mAcch.GetGroundH(); }

/* 022ABE08 */
s8 daNpc_Os_c::getFinishOrderEventNum() {
    if (actor_status & fopAcStts_CARRY_e) {
        if (argument == 0)
            return 0xB;
        if (argument == 1)
            return 0xD;
        if (argument == 2)
            return 0xF;
    } else {
        if (argument == 0)
            return 0xA;
        if (argument == 1)
            return 0xC;
        if (argument == 2)
            return 0xE;
    }
    return -1;
}

/* 022ABE84 */
BOOL daNpc_Os_c::checkGoalRoom() {
    if (current.roomNo == 7 && wakeupCheck() && !finishCheck()) {
        field_0x7A5 = getFinishOrderEventNum();
        return TRUE;
    }
    return FALSE;
}

/* 022AC030 */
void daNpc_Os_c::eventOrderCheck() {
    if (field_0x7A5 == -1) {
        s8 roomNo = current.roomNo;
        if (argument == 0) {
            if (roomNo == 7 && dComIfGs_isSwitch(field_0x794, roomNo) && !dComIfGs_isEventBit(0x2510)) {
                field_0x7A5 = 7;
            }
        } else if (argument == 1) {
            if (roomNo == 7 && dComIfGs_isSwitch(field_0x794, roomNo) && !dComIfGs_isEventBit(0x2608)) {
                field_0x7A5 = 8;
            }
        } else if (argument == 2) {
            if (roomNo == 7 && dComIfGs_isSwitch(field_0x794, roomNo) && !dComIfGs_isEventBit(0x2604)) {
                field_0x7A5 = 9;
            }
        }
    }
}

/* 022ABF7C */
void daNpc_Os_c::makeBeam(int param_1) {
    if (field_0x738.mpBaseEmitter == nullptr) {
        daNpc_Os_infiniteEcallBack_makeEmitter(&field_0x738, os_a::ID_AK_SN_OTOMOBEAM00, &current.pos,
                                               &shape_angle, nullptr);
        if (param_1) {
            s32 reverb = dComIfGp_getReverb(current.roomNo);
            mDoAud_seStart(os_a::JA_SE_OBJ_OSTATUE_LIGHT_ST, &current.pos, 0, reverb);
        }
    }
    if (field_0x740.mpBaseEmitter == nullptr) {
        daNpc_Os_infiniteEcallBack_makeEmitter(&field_0x740, os_a::ID_AK_SN_OTOMOBEAM01, &current.pos,
                                               &shape_angle, nullptr);
    }
}

/* 022ABD30 */
void daNpc_Os_c::npcAction(void* param_1) {
    if (mNpcAction.i == 0) { /* !mNpcAction */
        s8 brk = field_0x7A2;
        speedF = 0.0f;
        if (brk == 8) {
            Os_initBrkHD(this, 6, 1);
        }
        Os_setNpcActionHD(this, &l_actionDescriptors.mWaitNpc, nullptr);
    }
    Os_callActionHD(this, &mNpcAction, param_1);
}

/* 022ABB18 */
void daNpc_Os_c::playerAction(void* param_1) {
    if (mPlayerAction.i == 0) { /* !mPlayerAction */
        speedF = 0.0f;
        gabi::Local<ProcFunc_l> fn;
        Os_copyActionHD(fn, &l_actionDescriptors.mWaitPlayer);
        setPlayerAction(fn, nullptr);
    }
    dComIfGp_setRStatusForce(os_a::dActStts_RETURN_e);
    dComIfGp_setDoStatus(os_a::dActStts_HIDDEN_e);
    dComIfGp_setAStatus(os_a::dActStts_HIDDEN_e);
    Os_callActionHD(this, &mPlayerAction, param_1);
}

/* 022ABAC8 */
void daNpc_Os_c::setPlayerAction(ProcFunc_l* actionFunc, void* arg) {
    mNpcAction.d = 0;
    mNpcAction.i = 0;
    mNpcAction.f = 0;
    gabi::Local<ProcFunc_l> fn; /* by-value copy */
    Os_copyActionHD(fn, actionFunc);
    setAction(&mPlayerAction, fn, arg);
}

/* 022ABBF8 */
BOOL daNpc_Os_c::returnLinkCheck() {
    if (!dComIfGp_event_runCheck()) {
        // HD: a single combined pad check replaces the GC R/Start checks.
        if (Os_triggerReturnHD(0) && (mAcch.m_flags & 0x20)) {
            return TRUE;
        }
    }
    return FALSE;
}

/* 022AB6DC */
BOOL daNpc_Os_c::eventProc() {
    if (eventInfo.checkCommandDemoAccrpt() && field_0x7A5 != -1) {
        if (field_0x7A5 == 0) {
            if (Os_startCheckHD(dComIfGp_getPEvtManager(), "OPTION_CHAR_END") ||
                Os_endCheckNameHD(dComIfGp_getPEvtManager(), "OPTION_CHAR_END")) {
                fopAc_ac_c* link = dComIfGp_getLinkPlayer();
                dComIfGp_event_setTalkPartner(link);
                Os_controlSoundHD(os_a::JA_SE_CTRL_NPC_TO_LINK);
            } else {
                m4E4 = m4E4 & ~1u;
                field_0x7A5 = -1;
            }
        } else if (field_0x7A5 != 2 && field_0x7A5 != 4 && field_0x7A5 != 6) {
            if ((u32)(field_0x7A5 - 0xA) <= 5) {
                Os_setNpcActionHD(this, &l_actionDescriptors.mWaitNpc, nullptr);
                mNoResetFlg1 = (mNoResetFlg1 & ~2u) | 0x40;
                Os_clearCbPlayerHD();
                setFinish();
            } else if (field_0x7A5 == 8) {
                dComIfGs_onEventBit(0x2608);
            } else if (field_0x7A5 == 9) {
                dComIfGs_onEventBit(0x2604);
            }
        }
        if (field_0x7A5 != -1) {
            u32 f = m4E4;
            field_0x7A5 = -1;
            m4E4 = f | 2;
        }
    }
    int staffIdx = getMyStaffId();
    if (dComIfGp_event_runCheck() && !checkCommandTalk()) {
        if (staffIdx != -1) {
            s32 actIdx = Os_getActIndexHD(dComIfGp_getPEvtManager(), staffIdx,
                                          gabi::at<OsCutNamesHD>(0x101C2854)->mNames, 0xC, 1, 0);
            dEvent_manager_c* ev = dComIfGp_getPEvtManager();
            if (actIdx == -1) {
                Os_cutEndHD(ev, staffIdx);
            } else {
                if (Os_getAdvanceHD(ev, staffIdx)) {
                    (this->*event_init_tbl.mActions[actIdx])(staffIdx);
                }
                if ((this->*event_action_tbl.mActions[actIdx])(staffIdx)) {
                    Os_cutEndHD(dComIfGp_getPEvtManager(), staffIdx);
                }
            }
        }
        if (m4E4 & 2) {
            s16 id = field_0x7C4[field_0x7AA];
            if (Os_endCheckHD(dComIfGp_getPEvtManager(), (s32)id)) {
                dComIfGp_event_reset();
                m4E4 = m4E4 & ~2u;
                if (field_0x7AA == 0) {
                    returnLinkPlayer();
                    m4E4 = m4E4 & ~1u;
                }
                field_0x7AA = -1;
            }
            return TRUE;
        }
        if (dComIfGp_getLinkPlayer()->eventInfo.mCommand != dEvtCmd_INDOOR_e) {
            return TRUE;
        }
    }
    return FALSE;
}

/* 022ABC68 */
void daNpc_Os_c::carryCheck() {
    if (actor_status & fopAcStts_CARRY_e) {
        Os_setNpcActionHD(this, &l_actionDescriptors.mCarry, nullptr);
    }
}

/* 022AC160 */
void daNpc_Os_c::eventOrder() {
    if (m4E4 & 2) {
        return;
    }
    if (field_0x7A5 == 0x12 || field_0x7A5 == 0x11) {
        eventInfo.onCondition(dEvtCnd_CANTALK_e);
        if (field_0x7A5 == 0x12) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (field_0x7A5 != -1 && field_0x7A5 < 0x10) {
        field_0x7AA = field_0x7A5;
        Os_orderEventHD(this, (s32)field_0x7C4[field_0x7AA], 0xFF, 0xFFFF, 0, 1);
    }
}

/* 022ABCB0 */
void daNpc_Os_c::checkOrder() {
    if (eventInfo.checkCommandTalk() && (field_0x7A5 == 0x12 || field_0x7A5 == 0x11)) {
        field_0x7A5 = -1;
        Os_setNpcActionHD(this, &l_actionDescriptors.mTalk, nullptr);
        fopAcM_cancelCarryNow(this);
    }
}

/* 022AD138 */
BOOL daNpc_Os_c::draw() {
    if (home.roomNo < 0) {
        return TRUE;
    }
    if (finishCheck() && mpPedestal == nullptr) {
        return TRUE;
    }
    s8 roomNo = current.roomNo;
    dComIfGp_get(); // HD: retain the room-control accessor before reading the static status table.
    if (!(gabi::at<OsRoomStatusHD>(0x1047E8E8)[(s8)roomNo].mStatus & 0x10)) {
        return TRUE;
    }
    J3DModel* pModel = mpMorf->getModel();
    J3DModelData* pModelData = pModel->mModelData;
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), pModel, &tevStr);
    Os_enterBrkHD(&mBrkAnm, pModelData, (f32)mBrkAnm.mFrameCtrl.mFrame); /* mBrkAnm.entry(modelData) */
    mpMorf->entryDL();                                                   /* mpMorf->entryDL() */
    reinterpret_cast<OsAnimatedModelDataHD*>(pModelData)->mpBrk = 0;     /* mBrkAnm.remove(modelData) */
    return TRUE;
}

/* 022AC1DC */
BOOL daNpc_Os_c::execute() {
    /* static JGeometry::TVec3<f32> splash_scale(0.6f, ...), ripple_scale(1.0f, ...) */
    if ((u32)l_waterScales.mSplashGuard == 0) {
        l_waterScales.mSplashGuard = 1;
        l_waterScales.mSplashScale.x = 0.6f;
        l_waterScales.mSplashScale.z = 0.6f;
        l_waterScales.mSplashScale.y = 0.6f;
    }
    if ((u32)l_waterScales.mRippleGuard == 0) {
        l_waterScales.mRippleGuard = 1;
        l_waterScales.mRippleScale.x = 1.0f;
        l_waterScales.mRippleScale.z = 1.0f;
        l_waterScales.mRippleScale.y = 1.0f;
    }
    field_0x784 = field_0x784 & ~0x10u;
    actor_status = actor_status & ~fopAcStts_SHOWMAP_e;
    checkPlayerRoom();
    if (!finishCheck()) {
        if (!Os_initialRoomCheckHD(this)) {
            current.roomNo = (s8)Os_groundRoomHD(dComIfG_Bgsp(), os_gnd(this)); /* HD: GetRoomId(m_gnd) */
            if (dComIfGp_getCb1Player() == this) {
                Os_clearCbPlayerHD();
            }
            endBeam();
            return TRUE;
        }
        Os_initialRestartHD(this, (s32)getRestartNumber(), 1);
        if (wakeupCheck()) {
            mNoResetFlg1 = mNoResetFlg1 & ~0x40u;
        } else {
            mNoResetFlg1 = mNoResetFlg1 | 0x40u;
        }
    }
    fopAcM_setStageLayer(this);

    f32 r15 = 15.0f;
    if (os_isPmf(&mNpcAction, OS_CARRY_ACTION_HD)) {
        Os_setWallRadiusHD(&mAcchCir[0], r15);
        Os_setWallRadiusHD(&mAcchCir[1], r15);
    } else {
        Os_setWallRadiusHD(&mAcchCir[0], 40.0f);
        Os_setWallRadiusHD(&mAcchCir[1], 40.0f);
    }

    fopAc_ac_c* ped = Os_searchPedestalHD(gabi::at<OsNamesHD>(0x101C28F0)->mNames[(s8)argument], 0xFF, 1);
    if (ped != nullptr) {
        u32 id = ped != nullptr ? (u32)ped->mBsPcId : 0xFFFFFFFFu;
        if (Os_isCreatingHD(id)) {
            ped = nullptr;
        }
    }
    mpPedestal = ped;

    if (ped != nullptr) {
        if (!(field_0x784 & 1) /* !isFinish() */) {
            if (finishCheck()) {
                fopAc_ac_c* p = mpPedestal;
                home.pos.x = p->current.pos.x;
                home.pos.y = p->current.pos.y + 240.0f;
                home.pos.z = p->current.pos.z;
            }
        } else if (!(field_0x784 & 2) /* !isSetHomePos() */) {
            fopAc_ac_c* p = mpPedestal;
            home.pos.x = ped->current.pos.x;
            home.pos.y = p->current.pos.y + 240.0f;
            home.pos.z = p->current.pos.z;
            s8 pedRoom = p->home.roomNo;
            if (home.roomNo != pedRoom) {
                home.roomNo = pedRoom;
            }
            if (current.roomNo != pedRoom) {
                current.roomNo = pedRoom;
            }
            mAcch.CrrPos(dComIfG_Bgsp());
            field_0x784 = field_0x784 | 0x10;
            if (os_groundH(this) != -1000000000.0f) {
                tevStr.mRoomNo = (u8)Os_groundRoomHD(dComIfG_Bgsp(), os_gnd(this)); /* tevStr.mRoomNo */
                u8 color = (u8)Os_groundColorHD(dComIfG_Bgsp(), os_gnd(this));
                u32 f = field_0x784;
                tevStr.mEnvrIdxOverride = color; /* tevStr.mEnvrIdxOverride */
                field_0x784 = f | 2;
            }
        }
    }

    if (finishCheck()) {
        u32 f = field_0x784;
        s8 roomNo = current.roomNo;
        if (f & 1) {
            current.pos.copy(home.pos);
        }
        dComIfGp_get();
        u8 st = gabi::at<OsRoomStatusHD>(0x1047E8E8)[(s8)roomNo].mStatus;
        if (roomNo < 0 || !(st & 0x10) || mpPedestal == nullptr) {
            endBeam();
            return TRUE;
        }
    } else {
        if (Os_moveStopCheckHD(this)) {
            gabi::Local<ProcFunc_l> fn;
            Os_copyActionHD(fn, &l_actionDescriptors.mWaitNpc);
            setNpcAction(fn, nullptr);
            field_0x7A3 = 0;
            endBeam();
            return TRUE;
        }
        if (field_0x7A8 && Os_polygonSafeHD(dComIfG_Bgsp(), &field_0x7FC) &&
            Os_isMovingBackgroundHD(dComIfG_Bgsp(), &field_0x7FC)) {
            Os_correctMovingBackgroundHD(dComIfG_Bgsp(), &field_0x7FC, 1, &old.pos, 0, 0);
        }
    }

    actor_status = (actor_status & ~0x3Fu) | 0x2E;
    {
        const auto* h = gabi::at<const OsJointHioHD>(0x10467E44); /* mJntCtrl.setParam(l_HIO.mNpc...) */
        Os_setJointParamsHD(&mJntCtrl, (s32)h->mNpc.mMaxBackboneX, (s32)h->mNpc.mMaxBackboneY,
                            (s32)h->mNpc.mMinBackboneX, (s32)h->mNpc.mMinBackboneY, (s32)h->mNpc.mMaxHeadX,
                            (s32)h->mNpc.mMaxHeadY, (s32)h->mNpc.mMinHeadX, (s32)h->mNpc.mMinHeadY,
                            (s32)h->mNpc.mMaxTurnStep);
    }

    if (!(field_0x784 & 1) /* !isFinish() */) {
        if (!(actor_status & fopAcStts_CARRY_e) && (field_0x784 & 8) &&
            Os_positionMoveCheckHD(this, gabi::at<OsNamesHD>(0x101C28E4)->mNames[(s8)argument])) {
            u16 ang = current.angle.y;
            f32 sy = speed.y;
            if (maxFallSpeed < sy) {
                sy = sy - gravity;
                speed.y = sy;
                if (sy < maxFallSpeed) {
                    speed.y = maxFallSpeed;
                }
            } else if (maxFallSpeed > sy) {
                sy = sy + gravity;
                speed.y = sy;
                if (sy > maxFallSpeed) {
                    speed.y = maxFallSpeed;
                }
            }
            f32 sf = speedF;
            speed.x = sf * cM_ssin(ang);
            speed.z = sf * cM_scos(ang);
            Os_movePositionHD(this, &mStts);
        }
        mAcch.CrrPos(dComIfG_Bgsp());
        field_0x784 = field_0x784 | 0x10;

        f32 gndY = os_groundH(this);
        if ((os_isPmf(&mPlayerAction, OS_WALK_PLAYER_ACTION_HD) ||
             os_isPmf(&mNpcAction, OS_SEARCH_ACTION_HD)) &&
            !(mAcch.m_flags & 0x20) /* !ChkGroundHit() */) {
            f32 delta = gndY - current.pos.y;
            if (delta < 0.0f && !(delta < -30.1f)) {
                speed.y = 0.0f;
                current.pos.y = gndY;
                mAcch.m_flags = mAcch.m_flags | 0x20;
            }
            gndY = os_groundH(this);
        }

        if (gndY != -1000000000.0f) {
            cBgS_PolyInfo_l* gnd = os_gnd(this);
            u32 pla = Os_trianglePlaneHD(dComIfG_Bgsp(), (u32)gnd->mBgIndex, (u32)gnd->mPolyIndex);
            if (pla) {
                field_0x7F0.copy(*gabi::at<cXyz>(pla)); /* plane->mNormal */
            }
            s8 roomNo = (s8)Os_groundRoomHD(dComIfG_Bgsp(), gnd);
            current.roomNo = roomNo;
            tevStr.mRoomNo = roomNo; /* tevStr.mRoomNo */
            u8 color = (u8)Os_groundColorHD(dComIfG_Bgsp(), gnd);
            field_0x7FC.mpBgW = (u32)gnd->mpBgW; /* field_0x7FC.SetPolyInfo(m_gnd) */
            field_0x7FC.mPolyIndex = gnd->mPolyIndex;
            field_0x7FC.mBgIndex = gnd->mBgIndex;
            tevStr.mEnvrIdxOverride = color;
            mStts.mRoomId = roomNo; /* mStts.SetRoomId(roomNo) */
            field_0x7FC.mProcId = (u32)gnd->mProcId;
            if (roomNo != 7) {
                mpPedestal = nullptr;
            }
        }

        field_0x7A8 = (mAcch.m_flags >> 5) & 1;
        if (!(actor_status & fopAcStts_CARRY_e)) {
            if (os_groundH(this) == -1000000000.0f || Os_groundCodeHD(dComIfG_Bgsp(), os_gnd(this)) == 4) {
                u8 c = m4E8;
                if (c < 30) {
                    m4E8 = c + 1;
                } else {
                    s8 room = home.roomNo;
                    dComIfGp_get();
                    if (!(gabi::at<OsRoomStatusHD>(0x1047E8E8)[(s8)room].mStatus & 0x10)) {
                        // HD: preserve the GHS word-copy order, including packed angle/room words.
                        auto* dst = reinterpret_cast<OsPlacementWordsHD*>(&current);
                        const auto* src = reinterpret_cast<const OsPlacementWordsHD*>(&home);
                        u16 ax = home.angle.x;
                        u16 ay = home.angle.y;
                        shape_angle.x = ax;
                        dst->mPosition.z = (u32)src->mPosition.z;
                        dst->mAnglesXY = (u32)src->mAnglesXY;
                        dst->mAngleZAndRoom = (u32)src->mAngleZAndRoom;
                        speedF = 0.0f;
                        m4E8 = 0;
                        shape_angle.y = ay;
                        dst->mPosition.y = (u32)src->mPosition.y;
                        dst->mPosition.x = (u32)src->mPosition.x;
                        shape_angle.z = (u16)home.angle.z;
                    } else {

                        Os_restartLinkHD(dComIfGp_getLinkPlayer(), 5, 0xC9, -1.0f, 0);
                    }
                }
            } else {
                m4E8 = 0;
            }
            if (mAcch.m_flags & 0x1000) {
                Os_restartLinkHD(dComIfGp_getLinkPlayer(), 5, 0xC9, -1.0f, 0);
                if (!(field_0x784 & 4) /* !isWaterHit() */) {
                    field_0x784 = field_0x784 | 4;
                    u32 splash = gabi::ea(
                        dPa_control_set(dComIfGp_getParticle(), 0, os_a::ID_IT_JN_WP_SHIBUKI, &current.pos,
                                        nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr));
                    if (splash) {
                        gabi::at<JPABaseEmitter>(splash)->setRate(r15);
                        os_setEmitterScale(splash, gabi::ea(&l_waterScales.mSplashScale));
                    }

                    u32 ripple = gabi::ea(dPa_control_set(
                        dComIfGp_getParticle(), 5, os_a::ID_IT_JN_WP_HAMON01, &current.pos, nullptr, nullptr,
                        0xFF, gabi::at<dPa_levelEcallBack>(0x1047B2E4), -1, nullptr, nullptr, nullptr));
                    if (ripple) {
                        os_setEmitterScale(ripple, gabi::ea(&l_waterScales.mRippleScale));
                    }
                }
            }
        }
    } else {
        mAcch.CrrPos(dComIfG_Bgsp());
        f32 gh = os_groundH(this);
        field_0x784 = field_0x784 | 0x10;
        if (gh != -1000000000.0f) {
            tevStr.mRoomNo = (u8)Os_groundRoomHD(dComIfG_Bgsp(), os_gnd(this));
            tevStr.mEnvrIdxOverride = (u8)Os_groundColorHD(dComIfG_Bgsp(), os_gnd(this));
        }
    }

    if (!(actor_status & fopAcStts_CARRY_e)) {
        setCollision();
    }
    if (Os_positionMoveCheckHD(this, gabi::at<OsNamesHD>(0x101C28E4)->mNames[(s8)argument])) {
        animationPlay();
    }

    if (!eventProc()) {
        if (!finishCheck()) {
            if (dComIfGp_getPlayer(0) == this) {
                u32 st = actor_status;
                u32 flg = m4E4;
                actor_status = (st & ~0x3Fu) | 0x33;
                if (flg & 1) {
                    field_0x7A5 = 0;
                } else {
                    playerAction(nullptr);
                    if (returnLinkCheck()) {
                        m4E4 = m4E4 | 1;
                    }
                }
            } else {
                if (wakeupCheck()) {
                    actor_status = (actor_status & ~0x3Fu) | 0x2E;
                }
                carryCheck();
                checkOrder();
                npcAction(nullptr);
                if (!os_isPmf(&mNpcAction, OS_THROW_ACTION_HD)) {
                    current.angle.y = shape_angle.y;
                }
            }
            checkGoalRoom();
        }
        if (field_0x78C == 2 || field_0x78C == 4) {
            makeBeam(0);
        } else {
            endBeam();
        }
        eventOrderCheck();
    }

    eventOrder();
    setBaseMtx();
    if (field_0x738.mpBaseEmitter != nullptr || field_0x740.mpBaseEmitter != nullptr) {
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_seStart(os_a::JA_SE_OBJ_OSTATUE_LIGHT_SUS, &current.pos, 0, reverb);
    }
    os_copy_tevstr(this);
    return TRUE;
}

/* 022AD0F0 */
static BOOL daNpc_Os_Execute(daNpc_Os_c* i_this) {
    BOOL ret = i_this->execute();
    if (i_this->argument == 2) {
        gabi::at<OsCattleRoomHD>(0x101D5F46)->mRoomNo = (s8)i_this->current.roomNo;
    }
    return ret;
}

/* 022AD210 */
static BOOL daNpc_Os_IsDelete(daNpc_Os_c* i_this) { return TRUE; }

/* 022ABF04 */
static void daNpc_Os_infiniteEcallBack_makeEmitter(daNpc_Os_infiniteEcallBack_l* cb, u16 id, const cXyz* pos,
                                                   const csXyz* angle, const cXyz* scale) {
    Os_endEmitterHD(cb);

    dPa_control_set(dComIfGp_getParticle(), 0, id, pos, angle, scale, 0xFF, (dPa_levelEcallBack*)cb, -1,
                    nullptr, nullptr, nullptr);
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py): this translation unit
data m_smoke_tevstr 10475460
data l_waterScales 10467F00
data l_actionDescriptors 1001F260
data event_init_tbl 101C2964
data event_action_tbl 101C29C4
str 1001F620 "OPTION_CHAR_END"
ext 02007840 BOOL Os_triggerReturnHD(s32 port)
ext 022A9F4C void Os_initBrkHD(daNpc_Os_c* actor, u32 animation, u32 loop)
ext 022AB258 void Os_endEmitterHD(daNpc_Os_infiniteEcallBack_l* callback)
ext 025D7A58 void Os_orderEventHD(daNpc_Os_c* actor, s32 event, u32 mapTool, u32 flags, s32 priority, s32 mode)
ext 025445B8 BOOL Os_startCheckHD(dEvent_manager_c* manager, const char* name)
ext 0254457C BOOL Os_endCheckNameHD(dEvent_manager_c* manager, const char* name)
ext 025E1988 void Os_controlSoundHD(u32 sound)
ext 02542EDC s32 Os_getActIndexHD(dEvent_manager_c* manager, s32 staff, gptr<char>* names, s32 count, s32 flag, s32 extra)
ext 02543280 void Os_cutEndHD(dEvent_manager_c* manager, s32 staff)
ext 025447C8 BOOL Os_getAdvanceHD(dEvent_manager_c* manager, s32 staff)
ext 025440C8 BOOL Os_endCheckHD(dEvent_manager_c* manager, s32 event)
ext 025E83FC void Os_enterBrkHD(mDoExt_brkAnm_os* animation, J3DModelData* data, f32 frame)
ext 024451B4 BOOL Os_initialRoomCheckHD(daNpc_Os_c* actor)
ext 0244586C void Os_initialRestartHD(daNpc_Os_c* actor, s32 restart, s32 flag)
ext 024EFF3C void Os_setWallRadiusHD(dBgS_AcchCir* circle, f32 radius)
ext 025D9F38 fopAc_ac_c* Os_searchPedestalHD(const char* name, s32 room, s32 flag)
ext 025DD868 BOOL Os_isCreatingHD(u32 processId)
ext 024452A0 BOOL Os_moveStopCheckHD(daNpc_Os_c* actor)
ext 02008254 BOOL Os_polygonSafeHD(dBgS* background, cBgS_PolyInfo_l* polygon)
ext 024EEABC BOOL Os_isMovingBackgroundHD(dBgS* background, cBgS_PolyInfo_l* polygon)
ext 024EF968 void Os_correctMovingBackgroundHD(dBgS* background, cBgS_PolyInfo_l* polygon, s32 flag, cXyz* pos, s32 angle, s32 extra)
ext 0259E08C void Os_setJointParamsHD(dNpc_JntCtrl_c* joint, s32 backX, s32 backY, s32 minBackX, s32 minBackY, s32 headX, s32 headY, s32 minHeadX, s32 minHeadY, s32 step)
ext 02445950 BOOL Os_positionMoveCheckHD(daNpc_Os_c* actor, const char* name)
ext 025D6800 void Os_movePositionHD(daNpc_Os_c* actor, dCcD_Stts* status)
ext 024EF130 s32 Os_groundRoomHD(dBgS* background, cBgS_PolyInfo_l* polygon)
ext 024EEEB8 u32 Os_groundColorHD(dBgS* background, cBgS_PolyInfo_l* polygon)
ext 024EF0BC s32 Os_groundCodeHD(dBgS* background, cBgS_PolyInfo_l* polygon)
ext 020084C8 u32 Os_trianglePlaneHD(dBgS* background, u32 backgroundIndex, u32 polygonIndex)
ext 023FD4E4 void Os_restartLinkHD(fopAc_ac_c* player, s32 mode, s32 room, f32 speed, s32 flag)
ext 022AA630 BOOL daNpc_Os_c::setAction(ProcFunc_l* cur, ProcFunc_l* action, void* arg)
ext 022AA778 void daNpc_Os_c::setNpcAction(ProcFunc_l* action, void* arg)
ext 022A9E0C BOOL daNpc_Os_c::wakeupCheck()
ext 022A9EAC BOOL daNpc_Os_c::finishCheck()
ext 022AB28C void daNpc_Os_c::endBeam()
ext 022AB618 void daNpc_Os_c::setFinish()
ext 022AB5A4 int daNpc_Os_c::getMyStaffId()
ext 022AB670 BOOL daNpc_Os_c::checkCommandTalk()
ext 022AB684 void daNpc_Os_c::returnLinkPlayer()
ext 022AB1C4 void daNpc_Os_c::checkPlayerRoom()
ext 022AA42C s8 daNpc_Os_c::getRestartNumber()
ext 022AB2BC void daNpc_Os_c::setCollision()
ext 022AB488 void daNpc_Os_c::animationPlay()
ext 022AA464 void daNpc_Os_c::setBaseMtx()
*/
// clang-format on
