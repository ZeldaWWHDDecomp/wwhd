/**
 * d_a_player_main_01.cpp (WWHD)
 * Player - Link (daPy_lk_c), address range #01 (023D4BB8..023E0E4F): model/heap creation, joint
 * callbacks, draw, item models, texture animations, attention list and the animation setters.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_player_main.cpp and its .inc files) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Functions of other ranges are called by address (gabi::call), not as C++ members.
 */
#include "d/actor/d_a_player_main_01.h"

static PlayerJ3DSysState j3dSysState;
static PlayerFrameCounter g_Counter;
static u16 damageEffectType;
static s16 damageEffectTimer;
static constexpr u32 Player01SafeStringVtable = 0x10034B24;
static PlayerDemoObjectState demoObject;

static SafeString sleeveMaterialName;
static SafeString ear4MaterialName;
static SafeString faceMaterialName;
static SafeString ear3MaterialName;
static SafeString ear2MaterialName;
static PlayerEyeMaterialNames eyeMaterialNames;

static f32 guard_rate;
static cXyz arm_pos;
static cXyz armA_offset;
static u32 arm_pos_initialized;
static u32 armA_offset_initialized;

static u8 m_maba_flg;
static u8 m_eye_move_flg;
static u8 m_maba_timer;
static u8 m_morf_frame;


static inline dComIfG_inf_c* Player01_game() { return gabi::at<dComIfG_inf_c>(dComIfGp_ea()); }
static inline dComIfG_inf_c* Player01_gameAt(u32 game) { return gabi::at<dComIfG_inf_c>(game); }
static inline void Player01_assureTermination(SafeString* string) {
    PlayerSafeStringVtable* vtable = gabi::at<PlayerSafeStringVtable>(string->__vtbl);
    gabi::call_ptr(vtable->mAssureTermination, string);
}

static Player01SwordAnmIndexTableStorage mSwordAnmIndexTable;
static Player01BokoAnmIndexTableStorage mBokoAnmIndexTable;
static Player01HammerAnmIndexTableStorage mHammerAnmIndexTable;
static Player01ProcInitTableStorage mProcInitTable;
static Player01SwordModelTable sword_model_tbl;
// HD: borrowed from the range-02 owner; this self-contained unit does not link that source.
static inline Player01TexAnmEntry* Player01_texAnmTable() {
    return gabi::at<Player01TexAnmIndexTableStorage>(0x100362A8)->mEntries;
}
static inline Player01AnimationResource* Player01_animation(u32 animation) { return gabi::at<Player01AnimationResource>(animation); }
static inline J3DFrameCtrl* Player01_frameCtrl(u32 control) { return gabi::at<J3DFrameCtrl>(control); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E3570 mDoExt_setCurrentHeap(JKRHeap*) -> old heap */
/* 0273AE48 operator new(size, align) (heap alloc with alignment) */
/* j3dSys.mModel (HD 0x104B462C); J3DModel user area at +0xB8 */
static inline u32 j3dSys_getModel_l() { return gabi::ea(j3dSys.getModel()); }
/* dComIfGs save data object *(0x101F84DC); dSv_event_c at +0x644 */
static inline u32 dComIfGs_base_l() { return gabi::load<u32>(0x101F84DC); }

/* daPy_lk_c members in the opaque blocks of the shared header (offsets measured in this range) */
#define LK_FIELD(T, off) (*gabi::at<be<T>>(gabi::ea(this) + (off)))
#define LK_upperAnmIdx() reinterpret_cast<daPy_anmHeap_c*>(m_anm_heap_upper)[UPPER_MOVE2_e].mIdx /* m_anm_heap_upper[UPPER_MOVE2_e].mIdx */
#define LK_underAnmIdx() reinterpret_cast<daPy_anmHeap_c*>(m_anm_heap_under)[UNDER_MOVE0_e].mIdx /* m_anm_heap_under[UNDER_MOVE0_e].mIdx */
#define mpEquipItemModel_ea (gabi::ea(this) + 0x4440)              /* J3DModel* mpEquipItemModel */
/* virtual daPy_lk_c::checkRopeReadyAnime() const (vtable slot 0xDC) */
#define LK_checkRopeReadyAnime() gabi::call_ptr<BOOL>(gabi::load<u32>(gabi::load<u32>(gabi::ea(this) + 0xB4) + 0xDC), this)

/* a float copy through an FPR that the recompiled original keeps bit-exact (no SNaN quieting) */
static inline void fcpy_l(u32 dst, u32 src) { gmem_stf32(dst, gmem_ld32(src)); }

static inline void Player_copyTransform(J3DTransformInfo*, const J3DTransformInfo*);
static inline void lk_copy16(u32 dst, u32 src);

/* ---- daPy_lk_c functions (by address) ---- */
enum : u32 {
    LK_createHeap = 0x023D5548,
    LK_jointBeforeCB = 0x023D6B30,
    LK_jointAfterCB = 0x023D7564, /* unnamed by the matcher */
    LK_draw = 0x023D9820,         /* matcher: drawShadow */
    LK_bowJointCB = 0x023D6230,
    LK_fanJointCB = 0x023D641C,
    LK_parachuteJointCB = 0x023D6650,
    LK_HDJointCB_023D689C = 0x023D689C, /* unnamed by the matcher */
    LK_jointCB0 = 0x023D7A0C,
    LK_initModel = 0x023D4BB8,
    LK_resetPriTextureAnime = 0x023DC58C,
    LK_actorKeep_clearData = 0x023DC63C,
    LK_setTextureAnimeResource = 0x023DC110, /* unnamed by the matcher */
    LK_deleteEquipItem = 0x023DC7AC,
    LK_setSwordModel = 0x023DE0B4,
    LK_makeItemType = 0x023DF600,
    LK_setTextureAnime = 0x023DD768,
    LK_actorKeep_setData = 0x023DE638,
    LK_setBowModel = 0x023DEA24,
    LK_setBottleModel = 0x023DF0FC,
    LK_setShipRideArmAngle = 0x023D7198,
};

/* operator new[](size, align) for a type without a constructor (HD, unnamed) */
/* 023D4E54 */
static u32 daPy_newAlign(u32 size, s32 align) {
    return operator_new_align(size, align);
}

/* 023D4E8C */
static u32 daPy_matAnm_ct(u32 self) {
    if (self == 0) {
        self = gabi::ea(operator_new(0x7C));
        if (self == 0) {
            return self;
        }
    }
    daPy_matAnm_c* animation = gabi::at<daPy_matAnm_c>(self);
    animation->mNowOffset.y = 0.0f;
    animation->mOldOffset.x = 0.0f;
    animation->__vtbl = daPy_matAnm_c::__vt;
    animation->mOldOffset.y = 0.0f;
    animation->mNowOffset.x = 0.0f;
    // HD: blinking and eye movement use translation-unit static members.
    m_maba_flg = 0;
    m_eye_move_flg = 0;
    m_maba_timer = 0;
    m_morf_frame = 0;
    return self;
}

/* 023D6184 */
static BOOL daPy_createHeap(fopAc_ac_c* i_this) {
    return reinterpret_cast<daPy_lk_c*>(i_this)->createHeap();
}

/* 023D6210 */
BOOL daPy_lk_c::checkBowReadyAnime() {
    u16 idx = LK_upperAnmIdx();
    return idx == dRes_INDEX_LKANM_BCK_ARROWRELORD_e || idx == dRes_INDEX_LKANM_BCK_BOWWAIT_e;
}

/* 023D63D0 */
static BOOL daPy_bowJointCB(J3DNode* node, int calcTiming) {
    if (!calcTiming) {
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 i_this = (u32)gabi::at<J3DModel>(j3dSys_getModel_l())->mUserArea;
        if (i_this != 0) { /* HD: NULL check */
            gabi::at<daPy_lk_c>(i_this)->bowJointCB((int)(u16)reinterpret_cast<PlayerJointData*>(joint)->mJntNo);
        }
    }
    return TRUE;
}

/* 023D6604 */
static BOOL daPy_fanJointCallback(J3DNode* node, int calcTiming) {
    if (!calcTiming) {
        u32 i_this = (u32)gabi::at<J3DModel>(j3dSys_getModel_l())->mUserArea;
        J3DJoint* joint = J3DNode_toJoint(node);
        gabi::at<daPy_lk_c>(i_this)->fanJointCB((int)(u16)reinterpret_cast<PlayerJointData*>(joint)->mJntNo);
    }
    return TRUE;
}

/* 023D6850 */
static BOOL daPy_parachuteJointCallback(J3DNode* node, int calcTiming) {
    if (!calcTiming) {
        u32 i_this = (u32)gabi::at<J3DModel>(j3dSys_getModel_l())->mUserArea;
        J3DJoint* joint = J3DNode_toJoint(node);
        gabi::at<daPy_lk_c>(i_this)->parachuteJointCB((int)(u16)reinterpret_cast<PlayerJointData*>(joint)->mJntNo);
    }
    return TRUE;
}

/* 023D6980 */
static BOOL daPy_HDJointCallback(J3DNode* node, int calcTiming) {
    if (!calcTiming) {
        u32 i_this = (u32)gabi::at<J3DModel>(j3dSys_getModel_l())->mUserArea;
        J3DJoint* joint = J3DNode_toJoint(node);
        gabi::at<daPy_lk_c>(i_this)->HDJointCB((int)(u16)reinterpret_cast<PlayerJointData*>(joint)->mJntNo);
    }
    return TRUE;
}

/* 023D80F0 */
static BOOL daPy_jointCallback0(J3DNode* node, int calcTiming) {
    if (!calcTiming) {
        u32 i_this = (u32)gabi::at<J3DModel>(j3dSys_getModel_l())->mUserArea;
        J3DJoint* joint = J3DNode_toJoint(node);
        gabi::at<daPy_lk_c>(i_this)->jointCB0((int)(u16)reinterpret_cast<PlayerJointData*>(joint)->mJntNo);
    }
    return TRUE;
}

/* 023D69CC */
BOOL daPy_lk_c::checkShipNotNormalMode() {
    u16 idx = LK_underAnmIdx();
    if (idx == dRes_INDEX_LKANM_BCK_SHIP_JUMP2_e /* SHIP_JUMP1 */ || idx == dRes_INDEX_LKANM_BCK_VOYAGE1_e || idx == dRes_INDEX_LKANM_BCK_SHIP_JUMP1_e) {
        return FALSE;
    }
    return TRUE;
}

/* 023D69F8 */
BOOL daPy_lk_c::checkBoomerangAnime() {
    u16 idx = LK_upperAnmIdx();
    return idx == dRes_INDEX_LKANM_BCK_BOOMWAIT_e || idx == dRes_INDEX_LKANM_BCK_BOOMTHROW_e;
}

/* 023D6A18 */
BOOL daPy_lk_c::checkBowAnime() {
    return checkBowReadyAnime() || LK_upperAnmIdx() == dRes_INDEX_LKANM_BCK_ARROWSHOOT_e /* checkBowShootAnime */;
}

/* 023D6A5C */
BOOL daPy_lk_c::checkRopeAnime() {
    return LK_checkRopeReadyAnime() || LK_upperAnmIdx() == dRes_INDEX_LKANM_BCK_ROPETHROW_e /* ROPETHROW */;
}

/* 023D6ABC */
BOOL daPy_lk_c::checkUpperReadyThrowAnime() {
    return checkBoomerangAnime() || checkBowAnime() || checkRopeAnime() || LK_upperAnmIdx() == dRes_INDEX_LKANM_BCK_HOOKSHOTWAIT_e /* checkHookshotReadyAnime */;
}

/* 023D7194 */
static BOOL daPy_jointBeforeCallback(u32 userArea, int jnt_no, u32 p2, u32 p3 /* Quaternion pointer */) {
    return gabi::at<daPy_lk_c>(userArea)->jointBeforeCB(jnt_no, p2, p3);
}

/* 023D78E0 */
static BOOL daPy_jointAfterCallback(u32 userArea, int jnt_no, u32 p2, u32 p3 /* Quaternion pointer */) {
    return gabi::at<daPy_lk_c>(userArea)->jointAfterCB(jnt_no, p2, p3);
}

/* 023D7904 */
BOOL daPy_lk_c::checkItemEquipAnime() {
    u16 idx = LK_upperAnmIdx(); /* read once, before the call */
    return idx == dRes_INDEX_LKANM_BCK_TAKE_e /* TAKE */ || checkSingleItemEquipAnime() || idx == dRes_INDEX_LKANM_BCK_TAKEBOTH_e /* TAKEBOTH */;
}

/* 023D798C */
BOOL daPy_lk_c::checkUpperReadyAnime() {
    return LK_upperAnmIdx() == dRes_INDEX_LKANM_BCK_BOOMWAIT_e /* checkBoomerangReadyAnime */ || checkBowReadyAnime() || LK_checkRopeReadyAnime() ||
           LK_upperAnmIdx() == dRes_INDEX_LKANM_BCK_HOOKSHOTWAIT_e /* checkHookshotReadyAnime */;
}

/* 023D85A4 */
BOOL daPy_lk_c::checkCaughtShapeHide() {
    if (mCurProc == daPyProc_DEMO_CAUGHT_e && mProcVar6 != 0) {
        return TRUE;
    }
    return FALSE;
}

/* 023D8E78 */
BOOL daPy_lk_c::checkDemoShieldNoDraw() {
    return dSv_event_isEventBit(gabi::at<dSv_event_c>(dComIfGs_base_l() + 0x644), 0x3F40) != 0;
}

/* HD-only, unnamed: registers a packet (r4) in a draw list kept in Link at 0x4B58 */
/* 023D9340 */
void daPy_lk_c::entryHDPacket(u32 pkt, u32 arg) {
    PlayerModelState& models = *reinterpret_cast<PlayerModelState*>(mD90);
    // HD: each packet is queued only while the model packet list is enabled.
    PlayerPacketList& list = models.mPacketList;
    if (list.mEnabled == 0) {
        return;
    }
    gabi::at<PlayerDrawPacket>(pkt)->mpModel = arg;
    s32 n = list.mCount;
    if (n >= list.mCapacity) {
        return;
    }
    list.mpPackets.get()[n] = pkt;
    list.mCount = list.mCount + 1;
}

/* 023D937C */
BOOL daPy_lk_c::checkChanceMode() {
    if (dComIfGp_getDoStatus() == 0x1A /* dActStts_PARRY_e */ || (mModeFlg & 0x80000000) /* ModeFlg_PARRY */) {
        return TRUE;
    }
    return FALSE;
}

// HD: the water-droplet effect keeps its frame controller inside the BRK object.
/* 023D93DC */
BOOL daPy_lk_c::checkHDAnmNotEnd() {
    PlayerModelState& models = *reinterpret_cast<PlayerModelState*>(mD90);
    mDoExt_brkAnm& animation = models.mYuchw00Brk;
    f32 end = (f32)animation.mFrameCtrl.mEnd - 0.5f;
    return !(animation.mFrameCtrl.mFrame > end);
}

/* 023DBBE4 */
static BOOL daPy_Draw(daPy_lk_c* i_this) {
    return i_this->draw();
}

/* 023DBBE8 */
BOOL daPy_lk_c::checkGrabSpecialHeavyState() {
    fopAc_ac_c* grab_actor = mActorKeepGrab.mActor;
    if (grab_actor != nullptr) {
        if (fpcM_GetName(grab_actor) == fpcNm_KB_e && (fopAcM_GetParam(grab_actor) & 8)) {
            return TRUE;
        }
    }
    return FALSE;
}

/* 023DC650 */
void daPy_lk_c::deleteArrow() {
    fopAc_ac_c *equip_actor = mActorKeepEquip.mActor;
    if (equip_actor != nullptr && fpcM_GetName(equip_actor) == player01::fpcNm_ARROW_e) {
        fopAcM_delete(equip_actor);
        Player01_clearActorKeep(&mActorKeepEquip);
    }
}

/* 023DDEA8 */
u32 daPy_lk_c::setAnimeHeap(u32 animeHeap) {
    /* JKRHeap::freeAll (virtual, vtable at +0xC, slot 0x54) */
    gabi::call_ptr(reinterpret_cast<Player01Heap *>(gabi::at<void>(animeHeap))->mpVtable->mFreeAll,
                   animeHeap);
    return mDoExt_setCurrentHeap(animeHeap);
}

/* 023DE71C */
u32 daPy_lk_c::getAnimeResource(u32 anmHeap, u16 index, u32 bufferSize) {
    /* HD: the animation is taken from the resident "LkAnm" archive (no read into the anime heap) */
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::guest_str("LkAnm"); /* "LkAnm" */
    key->__vtbl = Player01SafeStringVtable;
    u32 bck = Player_getArchiveResource(dComIfG_resControl(), key.get(), (u32)index);
    reinterpret_cast<daPy_anmHeap_c *>(gabi::at<void>(anmHeap))->mIdx = index;       /* mIdx */
    reinterpret_cast<daPy_anmHeap_c *>(gabi::at<void>(anmHeap))->field_0x2 = 0xFFFF; /* field_0x2 */
    return bck;
}

/* 023DE788 */
void daPy_lk_c::setFrameCtrl(J3DFrameCtrl *frameCtrl, u8 attribute, s16 start, s16 end, f32 rate,
                             f32 frame) {
    frameCtrl->mAttribute = attribute;
    if (attribute == 2 /* EMode_LOOP */) {
        frameCtrl->mEnd = (s16)(end - 1); /* HD: a looping animation ends one frame earlier */
    } else {
        frameCtrl->mEnd = end;
    }
    frameCtrl->mRate = rate;
    frameCtrl->mStart = start;
    frameCtrl->mFrame = frame;
    if (!(rate < 0.0f)) { /* rate >= 0.0f (taken on NaN) */
        frameCtrl->mLoop = start;
    } else {
        frameCtrl->mLoop = end;
    }
}

/* 023DED8C */
void daPy_lk_c::setScopeModel() {
    u32 oldHeap = gabi::ea(setItemHeap());
    gabi::call(LK_initModel, this, mpEquipItemModel_ea, player01::dRes_INDEX_LINK_BDL_TELESCOPE_e,
               0x37221222);
    mDoExt_setCurrentHeap(oldHeap);
}

/* 023DEEAC */
void daPy_lk_c::setTinkleCeiverModel() {
    u32 oldHeap = gabi::ea(setItemHeap());
    gabi::call(LK_initModel, this, mpEquipItemModel_ea, player01::dRes_INDEX_LINK_BDL_TCEIVER_e,
               0x13000022);
    mDoExt_setCurrentHeap(oldHeap);
}

/* 023DFA48 */
static void daPy_followEcallBack_end(u32 self) {
    u32 emitter =
        (u32) reinterpret_cast<daPy_mtxFollowEcallBack_c *>(gabi::at<void>(self))->mpEmitter.v;
    if (emitter != 0) {
        /* becomeInvalidEmitter: HD stops the emitter (max frame -1) and sets its delete flag */
        reinterpret_cast<Player01EmitterState *>(gabi::at<void>(emitter))->mMaxFrame = -1;
        reinterpret_cast<Player01EmitterState *>(gabi::at<void>(emitter))->mFlags =
            (u32) reinterpret_cast<Player01EmitterState *>(gabi::at<void>(emitter))->mFlags | 1;
        reinterpret_cast<Player01EmitterState *>(
            reinterpret_cast<daPy_mtxFollowEcallBack_c *>(gabi::at<void>(self))->mpEmitter.get())
            ->mpEmitterCallBack = 0; /* setEmitterCallBackPtr(NULL) */
        reinterpret_cast<daPy_mtxFollowEcallBack_c *>(gabi::at<void>(self))->mpEmitter = nullptr;
    }
}

/* 023DFA7C */
void daPy_lk_c::freeHookshotItem() {
    if (mEquipItem != dItemNo_HOOKSHOT_e) {
        return;
    }
    u32 hookshot = gabi::ea(mActorKeepEquip.mActor.get());
    if (hookshot == 0) {
        return;
    }
    if ((u32)gabi::at<fopAc_ac_c>(hookshot)->mParameters == 0) { /* checkWait() */
        return;
    }
    gabi::at<fopAc_ac_c>(hookshot)->mParameters = 2; /* setReturn() */
}

/* 023DFAAC */
void daPy_lk_c::swimOutAfter(BOOL param_1) {
    mNoResetFlg0 = mNoResetFlg0 | 0x100; /* onNoResetFlg0(daPyFlg0_UNK100) */
    m35C4 = 0.0f;
    if (param_1) {
        seStartOnlyReverb(player01::JA_SE_LK_OUTOF_WATER);
    }
    /* dComIfGp_clearItemTimeCount() */
    u32 play = dComIfGp_ea();
    Player01_gameAt(play)->play.mAirMeter = 0;
    Player01_gameAt(play)->play.mItemSwimTimerStatus = 0;
    Player01_resetPriTextureAnime(this);
}

/* ======== batch 2 ======== */
/* 025D54C4 fopAcM_SearchByID(id, fopAc_ac_c** out) (the out-of-line form) */
static inline BOOL fopAcM_SearchByID2_l(u32 id, u32 out) { return gabi::call<BOOL>(0x025D54C4, id, out); }
/* 026067F4 dRes_control_c::getIDRes(const SafeString& arc, u16 id) */
static inline u32 dRes_getIDRes_l(u32 key, u32 id) { return gabi::call<u32>(0x026067F4, dComIfG_resControl(), key, id); }
/* sead::SafeString virtual at slot 0x14 (called before its string is read; vtable at +4) */
static inline void SafeString_vf14_l(u32 s) { gabi::call_ptr(gabi::at<PlayerSafeStringVtable>(gabi::at<SafeString>(s)->__vtbl)->mAssureTermination, s); }
/* the HD 0x58 animation object at 0x6F4 (texture scroll) and J3DModelData helper */

/* 023D4BB8 */
u32 daPy_lk_c::initModel(u32 i_model, int i_fileIndex, u32 i_differedDlistFlag) {
    u32 tmp_modelData = gabi::ea(dComIfG_getObjectRes("Link" /* l_arcName "Link" */, i_fileIndex, 0x10034B24));
    if (tmp_modelData == 0) {
        JUT_ASSERT_fail("d_a_player_main.cpp", 0x5EAF, "tmp_modelData != (0)");
    }
    u32 model = gabi::ea(mDoExt_J3DModel__create(gabi::at<J3DModelData>(tmp_modelData), 0x80000, i_differedDlistFlag));
    *gabi::at<gptr<J3DModel>>(i_model) = gabi::at<J3DModel>(model);
    if (model == 0) {
        JUT_ASSERT_fail("d_a_player_main.cpp", 0x5EB3, "*i_model != (0)");
    }
    return tmp_modelData;
}

/* HD-only, unnamed: the A button status from the player's state */
/* 023D6188 */
static void daPy_setHDAStatus() {
    if (dComIfGp_getAStatus() != 0) {
        return;
    }
    fopAc_ac_c* ship = dComIfGp_getShipActor();
    PlayerShipActionState* shipState = reinterpret_cast<PlayerShipActionState*>(ship);
    if (!((f32)ship->speedF < 3.0f) && shipState->mTornadoActor == nullptr &&
        shipState->mWhirlActor == nullptr) {
        dComIfGp_setAStatus(0x13);
        return;
    }
    dComIfGp_setAStatus(8);
}

/* offBodyEffect (unnamed by the matcher) */
/* 023D83D8 */
void daPy_lk_c::offBodyEffect() {
    gptr<void>* emitters[] = {
        &reinterpret_cast<daPy_mtxFollowEcallBack_c*>(m334C)->mpEmitter,
        &reinterpret_cast<daPy_mtxFollowEcallBack_c*>(m336C)->mpEmitter,
        &m32E4.mpEmitter,
        &reinterpret_cast<daPy_mtxFollowEcallBack_c*>(m3460)[0].mpEmitter,
        &reinterpret_cast<daPy_mtxFollowEcallBack_c*>(m3460 + 0x10)->mpEmitter,
    };
    for (int i = 0; i < 5; i++) {
        auto* emitter = reinterpret_cast<jpa_emitter_calc::JPABaseEmitter*>(emitters[i]->get());
        if (emitter != nullptr) {
            emitter->mFlags = emitter->mFlags | 4u;
        }
    }
}

/* 023D8454 */
void daPy_lk_c::updateDLSetLight(J3DModel* model, u32 param_2) {
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    if (param_2 != 0) {
        dMat_control_c::iceUpdateDL(model, -1, 0);
    } else {
        mDoExt_modelUpdateDL(model, 0);
    }
}

/* 023D8DF0 */
void daPy_lk_c::entryDLSetLight(J3DModel* model, u32 param_2) {
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    if (param_2 != 0) {
        dMat_control_c::iceEntryDL(model, -1, 0);
    } else {
        mDoExt_modelEntryDL(model);
    }
}

/* 023D8EB0 */
BOOL daPy_lk_c::checkMaskDraw() {
    u8 id = demoActorID;
    /* dComIfGp_demo_getActor(id) */
    if (id != 0 && id <= 0x20) {
        u32 obj = gabi::ea(demoObject.mpObject.get());
        if (obj == 0) {
            JUT_ASSERT_fail("d_demo.h" /* "d_demo.h" */, 0x23A, "m_object != (0)");
            obj = gabi::ea(demoObject.mpObject.get());
        }
        if (Player_demoGetActor(obj, (u32)id) != 0) {
            return FALSE;
        }
    }
    /* dComIfGs_isCollect(4, 1) */
    if (dComIfGs_isCollect(4, 1) != 0) {
        return TRUE;
    }
    return FALSE;
}

/* 023D8F6C */
BOOL daPy_lk_c::checkDemoSwordNoDraw(BOOL param_0) {
    if ((mEquipItem == 0x103 /* daPyItem_SWORD_e */ || param_0) &&
        (dSv_event_isEventBit(gabi::at<dSv_event_c>(dComIfGs_base_l() + 0x644), 0x3F40) ||
         (mCurProc == daPyProc_DEMO_TOOL_e && mProcVar3 == 1))) {
        return TRUE;
    }
    return FALSE;
}

/* onBodyEffect (unnamed by the matcher) */
/* 023D8FF8 */
void daPy_lk_c::onBodyEffect() {
    gptr<void>* emitters[] = {
        &reinterpret_cast<daPy_mtxFollowEcallBack_c*>(m334C)->mpEmitter,
        &reinterpret_cast<daPy_mtxFollowEcallBack_c*>(m336C)->mpEmitter,
        &m32E4.mpEmitter,
        &reinterpret_cast<daPy_mtxFollowEcallBack_c*>(m3460)[0].mpEmitter,
        &reinterpret_cast<daPy_mtxFollowEcallBack_c*>(m3460 + 0x10)->mpEmitter,
    };
    for (int i = 0; i < 5; i++) {
        auto* emitter = reinterpret_cast<jpa_emitter_calc::JPABaseEmitter*>(emitters[i]->get());
        if (emitter != nullptr) {
            emitter->mFlags = emitter->mFlags & ~4u;
        }
    }
}

/* HD-only, unnamed: a GXColor converted to four floats (0..1) */
/* 023D9430 */
static u32 daPy_colorToF4(u32 out, u32 color) {
    GXColor* input = gabi::at<GXColor>(color);
    PlayerColorF4* output = gabi::at<PlayerColorF4>(out);
    f32 r = (f32)(u8)input->r / 255.0f;
    f32 g = (f32)(u8)input->g / 255.0f;
    f32 b = (f32)(u8)input->b / 255.0f;
    f32 a = (f32)(u8)input->a / 255.0f;
    output->r = r;
    output->g = g;
    output->b = b;
    output->a = a;
    return out;
}

/* 023DBC24 */
BOOL daPy_lk_c::checkHeavyStateOn() {
    if (Player01_game()->play.mEvtCtrl.mMode == 0 /* !dComIfGp_event_runCheck() */ &&
        (u16)mDemo.mDemoType == 0 /* !checkPlayerDemoMode() */ &&
        ((mNoResetFlg0 & 0x42000000) /* getHeavyStateAndBoots() */ ||
         checkGrabSpecialHeavyState())) {
        return TRUE;
    }
    return FALSE;
}

/* loadTextureAnimeResource (unnamed by the matcher) */
/* 023DC38C */
u32 daPy_lk_c::loadTextureAnimeResource(u32 btpIdx, BOOL isDemo) {
    /* HD: the texture animations come from the resident archives (no read into a buffer) */
    gabi::Local<SafeString> key;
    if (isDemo) {
        u32 play = dComIfGp_ea();
        Player01_assureTermination(
            &reinterpret_cast<Player01DemoArchive *>(Player01_gameAt(play))
                 ->mLkDemoAnmArchive); /* the Link demo archive name (sead::SafeString at play +
                                          0x5A50) */
        key->__vtbl = Player01SafeStringVtable;
        key->mStringTop = (u32) reinterpret_cast<Player01DemoArchive *>(Player01_gameAt(play))
                              ->mLkDemoAnmArchive.mStringTop;
        return dRes_getIDRes_l(gabi::ea(key.get()), (u16)btpIdx);
    }
    gabi::Local<SafeString> key2;
    key2->__vtbl = Player01SafeStringVtable;
    key2->mStringTop = gabi::guest_str("LkAnm"); /* "LkAnm" */
    return Player_getArchiveResource(dComIfG_resControl(), key2.get(), btpIdx);
}

/* 023DC43C */
void daPy_lk_c::setTextureScrollResource(u32 btk, int r31) {
    mpTexScrollResData = btk;
    m3532 = (u16)r31;
    /* HD: the scroll animation is an object at 0x6F4 */
    PlayerTexScroll_setAnm(gabi::ea(this) + 0x6F4,
                           (u32)gabi::at<PlayerTextureResource>(btk)->mpData);
    PlayerTexScroll_setModelData(gabi::ea(this) + 0x6F4,
                                 PlayerModelData_getResource(gabi::ea(mpCLModelData.get())));
    /* daPy_matAnm_c statics */
    m_maba_flg = 3; /* setMorfFrame(3) */
    m_morf_frame = 1;
    m_maba_timer = 0;                                      /* offMabaFlg() */
    m_morf_frame = (u8)gabi::ftoi(cM_rndF(30.0f) + 75.0f); /* setMabaTimer */
}

/* loadTextureScrollResource (unnamed by the matcher) */
/* 023DC4DC */
u32 daPy_lk_c::loadTextureScrollResource(u32 btkIdx, BOOL isDemo) {
    gabi::Local<SafeString> key;
    if (isDemo) {
        u32 play = dComIfGp_ea();
        Player01_assureTermination(
            &reinterpret_cast<Player01DemoArchive *>(Player01_gameAt(play))->mLkDemoAnmArchive);
        key->__vtbl = Player01SafeStringVtable;
        key->mStringTop = (u32) reinterpret_cast<Player01DemoArchive *>(Player01_gameAt(play))
                              ->mLkDemoAnmArchive.mStringTop;
        return dRes_getIDRes_l(gabi::ea(key.get()), (u16)btkIdx);
    }
    gabi::Local<SafeString> key2;
    key2->__vtbl = Player01SafeStringVtable;
    key2->mStringTop = gabi::guest_str("LkAnm"); /* "LkAnm" */
    return Player_getArchiveResource(dComIfG_resControl(), key2.get(), btkIdx);
}

/* 023DC58C */
void daPy_lk_c::resetPriTextureAnime() {
    daPy_anmHeap_c *h = reinterpret_cast<daPy_anmHeap_c *>(
        m_tex_anm_heap); /* m_tex_anm_heap: mIdx, field_0x2, field_0x4 */
    if ((u16)h->field_0x2 != 0xFFFF) {
        h->field_0x2 = 0xFFFF;
        if ((u16)h->field_0x4 == 0xFFFF) {
            u32 btp = loadTextureAnimeResource((u16)h->mIdx, FALSE);
            Player01_setTextureAnimeResource(this, btp, 0);
        }
    }
    h = reinterpret_cast<daPy_anmHeap_c *>(m_tex_scroll_heap); /* m_tex_scroll_heap */
    if ((u16)h->field_0x2 != 0xFFFF) {
        h->field_0x2 = 0xFFFF;
        if ((u16)h->field_0x4 == 0xFFFF) {
            u32 btk = loadTextureScrollResource((u16)h->mIdx, FALSE);
            setTextureScrollResource(btk, 0);
        }
    }
}

/* 023DCA08 */
void daPy_lk_c::endDamageEmitter() {
    for (int i = 0; i < 4; i++) {
        Player01_endMtxEmitter(gabi::ea(this) + 0x67CC + i * 0xC); /* mDmEcallBack[i] */
    }
    damageEffectTimer = 0; /* daPy_dmEcallBack_c::setTimer(0) */
    damageEffectType = 3;  /* daPy_dmEcallBack_c::setType(3) */
    reinterpret_cast<Player01LightInfluence *>(mLightInfluence)->mPower =
        0.0f; /* mLightInfluence.mPower */
}

/* 023DCA80 */
void daPy_lk_c::freeRopeItem() {
    if (mEquipItem == player01::dItemNo_GRAPPLING_HOOK_e) {
        fopAc_ac_c *rope = mActorKeepRope.mActor;
        if (mActorKeepEquip.mActor != nullptr) {
            setResetFlg0(resetFlg0() | 0x40000000); /* daPyRFlg0_ROPE_FORCE_END */
        }
        if (rope != nullptr) {
            if (fpcM_GetName(rope) == player01::fpcNm_HIMO2_e) {
                rope->mParameters = 4; /* fopAcM_SetParam */
                Player01_clearActorKeep(&mActorKeepRope);
                setResetFlg0(resetFlg0() | 0x40000000);
            } else if (fpcM_GetName(rope) == player01::fpcNm_HIMO3_e) {
                mEquipItem = player01::daPyItem_NONE_e;
                rope->mParameters = 3;
                Player01_clearActorKeep(&mActorKeepRope);
            }
        }
    }
}

/* 023DCC18 */
static void daPy_actorKeep_setActor(daPy_actorKeep_l *k) {
    if (k->mID != 0xFFFFFFFF) {
        k->mActor = fopAcM_SearchByID(k->mID);
        if (k->mActor == nullptr) {
            k->mID = 0xFFFFFFFF;
        }
    } else {
        k->mActor = nullptr;
    }
}

/* 023DCC90 */
void daPy_lk_c::setActorPointer() {
    daPy_actorKeep_setActor(&mActorKeepRope);
    daPy_actorKeep_setActor(&mActorKeepGrab);
    daPy_actorKeep_setActor(&mActorKeepThrow);
    daPy_actorKeep_setActor(&mActorKeepEquip);
    if (mEquipItem == player01::daPyItem_BOKO_e) {
        fopAc_ac_c *actor = mActorKeepEquip.mActor;
        if (actor == nullptr || !((u32)actor->actor_status & 0x2000) /* !fopAcM_checkCarryNow */) {
            Player01_clearActorKeep(&mActorKeepEquip);
            mEquipItem = 0x100;
        }
    }
    if (m3630 != 0xFFFFFFFF) {
        gabi::Local<be<u32>> sp8;
        if (!fopAcM_SearchByID2_l(m3630, gabi::ea(sp8.get()))) {
            m3630 = 0xFFFFFFFF;
        }
    }
}

/* 023DCF08 */
BOOL daPy_lk_c::checkGrabBarrelSearch(int param_0) {
    fopAc_ac_c *grab_actor;
    if (param_0 != 0) {
        if (mActorKeepGrab.mID == 0xFFFFFFFF) {
            return FALSE;
        }
        grab_actor = fopAcM_SearchByID(mActorKeepGrab.mID);
    } else {
        grab_actor = mActorKeepGrab.mActor;
    }
    if (grab_actor != nullptr && fpcM_GetName(grab_actor) == player01::fpcNm_Obj_Barrel_e) {
        return TRUE;
    }
    return FALSE;
}

/* 023DD12C */
BOOL daPy_lk_c::checkRestHPAnime() {
    /* checkPlayerGuard() is virtual (slot 0x3C) */
    if (!gabi::call_ptr<BOOL>(
            reinterpret_cast<PlayerActorDispatch *>(this)->mpVtable->mCheckPlayerGuard, this) &&
        LK_upperAnmIdx() == 0xFFFF /* checkNoUpperAnime() */ && mpAttnActorLockOn == nullptr &&
        (((u16)mDemo.mDemoType == 0 /* !checkPlayerDemoMode() */ &&
          !(mModeFlg & 0x2000) /* ModeFlg_IN_SHIP */ &&
          dComIfGs_getLife() <= 6 /* dComIfGs_getLife() <= HD constant (GameCube m_HIO) */) ||
         (s32)mDemo.mDemoMode == 0x12 /* mDemo.getDemoMode() == DEMO_UNK_018_e */)) {
        return TRUE;
    }
    return FALSE;
}

/* getItemAnimeResource (unnamed by the matcher) */
/* 023DDEEC */
u32 daPy_lk_c::getItemAnimeResource(u32 index /* HD: caller-extended u16 register */) {
    u32 oldHeap = setAnimeHeap(
        reinterpret_cast<PlayerModelState *>(mD90)->mpItemAnimeHeap /* mpItemAnimeHeap */);
    /* HD: the item animation comes from the resident "LkAnm" archive */
    gabi::Local<SafeString> key;
    key->__vtbl = Player01SafeStringVtable;
    key->mStringTop = gabi::guest_str("LkAnm"); /* "LkAnm" */
    u32 bas = Player_getArchiveResource(dComIfG_resControl(), key.get(), index);
    mDoExt_setCurrentHeap(oldHeap);
    return bas;
}

/* 023DF91C */
void daPy_lk_c::returnKeepItemData() {
    if (mKeepItem == player01::daPyItem_NONE_e) {
        return;
    }
    Player01_deleteEquipItem(this, FALSE);
    mEquipItem = mKeepItem;
    mKeepItem = 0x100;
    if (mEquipItem == player01::daPyItem_UNK10B_e) {
        mEquipItem = 0x100;
    } else if (mEquipItem == player01::daPyItem_SWORD_e) {
        Player01_setSwordModel(this, TRUE);
    } else {
        Player01_makeItemType(this);
    }
}

/* 023DF9C0 */
void daPy_lk_c::resetFootEffect() {
    Player01FootEffect *fe =
        reinterpret_cast<Player01FootEffect *>(mFootEffect); /* mFootEffect[2] (0x4C each) */
    for (int idx = 0; idx < 2; idx++, fe++) {
        /* getSmokeCallBack()->remove(), getOtherCallBack()->remove(): virtual (vtable at +0, slot
         * 0x44) */
        fe->mSmokeCallback.remove();
        fe->mOtherCallback.remove();
        fe->mID = -1; /* setID(-1) */
    }
    mFootEffectPosType = 0;
}

/* 023E048C */
u32 daPy_lk_c::getAnmData(int anm) {
    u16 item = mEquipItem;
    if (item == player01::daPyItem_SWORD_e) {
        if (anm < 0x1B) {
            return gabi::ea(&mSwordAnmIndexTable.mEntries[anm]); /* mSwordAnmIndexTable */
        }
    } else if (item == player01::daPyItem_BOKO_e) {
        if (anm < 0x1B) {
            return gabi::ea(&mBokoAnmIndexTable.mEntries[anm]); /* mBokoAnmIndexTable */
        }
    } else if (item == player01::dItemNo_SKULL_HAMMER_e) {
        if (anm < 0x1B) {
            return gabi::ea(&mHammerAnmIndexTable.mEntries[anm]); /* mHammerAnmIndexTable */
        }
    } else if (item == 0x2D /* BOOMERANG */ || item == 0x34 /* DEKU_LEAF */ ||
               item == 0x20 /* TELESCOPE */) {
        if (anm == 2 /* ANM_DASH */) {
            return gabi::ea(&mSwordAnmIndexTable.mEntries[anm]);
        }
    }
    return gabi::ea(&mAnmDataTable[anm]); /* &mAnmDataTable[anm].mAnmIdx */
}

/* ======== batch 3 ======== */
/* 025E1988 one-argument system sound (mDoAud_seStartSystem) */
static inline void mDoAud_seStartSystem_l(u32 id) { gabi::call(0x025E1988, id); }
/* 025E1918 mDoAud_subBgmStart */
static inline void mDoAud_subBgmStart_l(u32 id) { gabi::call(0x025E1918, id); }
/* play + 0x5C20: dComIfGp_setMesgBgmOn (1) / setMesgBgmOn2 (2) */
static inline void dComIfGp_setMesgBgm_l(u8 v) { gabi::store<u8>(dComIfGp_ea() + 0x5C20, v); }
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty; block at +0x2C, flags +4, matrices +0x10) */
static inline u32 lk_getAnmMtx(u32 model, s32 jnt) {
    J3DMtxBuffer_hd* buffer = gabi::at<J3DModel>(model)->mpMtxBuffer;
    buffer->mFlags |= 0x10;
    return buffer->mpAnmMtx + (u32)jnt * sizeof(Mtx34);
}
/* J3DModelData::getJointNodePointer(i) (HD: inline entries of 0x1C at +8, count at +4; out of range -> entry 0) */
static inline u32 lk_jointNode(u32 data, u32 i) {
    J3DModelData* modelData = gabi::at<J3DModelData>(data);
    u32 base = modelData->mJointNodes;
    // HD: keep the full index register and load the node table before the count.
    if (modelData->mJointNum > i) {
        base += i * 0x1C;
    }
    return base;
}
/* 025E8154 mDoExt_brkAnm::init(data, brk, bool anmPlay, int mode, f32 speed, s16 start, s16 end, bool, int (stack)) */
/* 025E7CE0 mDoExt_btkAnm::init(data, btk, bool anmPlay, int mode, f32 speed, s16 start, s16 end, bool, int (stack)) */
/* J3DAnmBase virtual at slot 0xC (vtable at +4): the animation's play mode */
static inline s32 anm_vf0C_l(u32 anm) { return gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(anm + 4) + 0xC), anm); }
/* "Link" resource (sead::SafeString key, this TU's vtable) */
static inline u32 lk_getLinkRes_l(u32 idx) {
    gabi::Local<SafeString> key;
    key->__vtbl = 0x10034B24;
    key->mStringTop = 0x101CEB48; /* l_arcName */
    return gabi::call<u32>(0x026066C4, dComIfG_resControl(), key.get(), idx);
}
/* 0273AEC4 operator new[](size, align) */

/* HD-only, unnamed: loads a brk of "Link" and binds it to a brkAnm object */
/* 023D4C78 */
u32 daPy_lk_c::initBrkAnm(u32 data, u32 brkAnm, u32 idx) {
    u32 res = lk_getLinkRes_l(idx);
    if (res == 0) {
        JUT_ASSERT_fail("d_a_player_main.cpp", 0x5F0C, "brk_anm != (0)");
    }
    s32 mode = anm_vf0C_l(res);
    Player_initBrkAnimation(brkAnm, data, res, 1, mode, 1.0f, 0, -1, gabi::at<mDoExt_brkAnm>(brkAnm)->mpAnm != nullptr, 0);
    gabi::at<mDoExt_brkAnm>(brkAnm)->entry(gabi::at<J3DModelData>(data), 0.0f);
    return res;
}

/* HD-only, unnamed: loads a btk of "Link" and binds it to a btkAnm object */
/* 023D4D64 */
u32 daPy_lk_c::initBtkAnm(u32 data, u32 btkAnm, u32 idx) {
    u32 res = lk_getLinkRes_l(idx);
    if (res == 0) {
        JUT_ASSERT_fail("d_a_player_main.cpp", 0x5ED1, "btk_anm != (0)");
        return res;
    }
    s32 mode = anm_vf0C_l(res);
    Player_initBtkAnimation(btkAnm, data, res, 1, mode, 1.0f, 0, -1, (u32)gabi::at<PlayerBtkRuntime>(btkAnm)->mAnimationResource != 0, 0);
    gabi::at<mDoExt_btkAnm>(btkAnm)->entry(gabi::at<J3DModelData>(data), 0.0f);
    return res;
}

/* HD-only, unnamed joint callback: copies Link's joint matrix to the model at 0x5700 */
/* 023D689C */
BOOL daPy_lk_c::HDJointCB(int jnt) {
    PlayerModelState& models = *reinterpret_cast<PlayerModelState*>(mD90);
    u32 src = lk_getAnmMtx(gabi::ea(mpCLModel.get()), jnt);
    u32 dst = lk_getAnmMtx(gabi::ea(models.mpYmgcs00Model.get()), jnt);
    mtx_copy(gabi::at<Mtx34>(dst), gabi::at<Mtx34>(src));
    PSMTXCopy(gabi::at<Mtx34>(lk_getAnmMtx(gabi::ea(mpCLModel.get()), jnt)), gabi::at<Mtx34>(0x104B4868) /* J3DSys::mCurrentMtx */);
    return TRUE;
}

/* 023D84E0 */
void daPy_lk_c::drawMirrorLightModel() {
    PlayerModelState& models = *reinterpret_cast<PlayerModelState*>(mD90);
    if (resetFlg0() & daPyRFlg0_LIGHT_REFLECT) {
        mDoMtx_stack_c::transS(5.0f, 7.0f, 0.0f); /* l_ms_light_local_start */
        mDoMtx_stack_c::YrotM(-0x8000);
        u32 shield = gabi::ea(models.mpEquippedShieldModel.get()); /* mpEquippedShieldModel */
        MTXConcat(gabi::at<J3DModel>(shield)->getBaseTRMtx(), mDoMtx_stack_c::get(), mDoMtx_stack_c::get());
        Player_updateMirrorPacket( gabi::ea(&models.mMirrorPacket), mDoMtx_stack_c::get(), 0xFF, 60.0f);
        Player_drawBufferEntryImmediate( (u32)reinterpret_cast<dDlstHD::dDlst_list_c*>(&gabi::at<dComIfG_inf_c>(dComIfGp_ea())->drawlist)->mpXluList /* dComIfGd_getXluList() */,
                   gabi::ea(&models.mMirrorPacket), 0xFF);
        updateDLSetLight(models.mpYmsls00Model.get() /* mpYmsls00Model */, 0);
    }
}

/* setGetItemSound (unnamed by the matcher) */
/* 023DBCA0 */
void daPy_lk_c::setGetItemSound(u32 itemNo, BOOL param_2) {
    if ((itemNo >= 1 && itemNo <= 5) /* GREEN..PURPLE_RUPEE */ ||
        itemNo == 0xB3 /* TINGLE_RUPEE_1 */) {
        mDoAud_seStartSystem_l(player01::JA_SE_ME_ITEM_GET_S);
        dComIfGp_setMesgBgm_l(2);
    } else if ((s32)itemNo == 8 /* HEART_CONTAINER */ ||
               ((s32)itemNo == 7 /* HEART_PIECE */ &&
                (dComIfGs_getMaxLife() & 3) == 3 /* getMaxLife() % 4 */)) {
        mDoAud_subBgmStart_l(player01::JA_BGM_GET_HEART);
        dComIfGp_setMesgBgm_l(1);
    } else if (itemNo == 7 || itemNo == 6 || itemNo == 0xF || itemNo == 0x15 || itemNo == 0x1F) {
        mDoAud_subBgmStart_l(player01::JA_BGM_ITEM_GET_S);
        dComIfGp_setMesgBgm_l(1);
    } else if (param_2) {
        mDoAud_seStartSystem_l(0x8F5);
        dComIfGp_setMesgBgm_l(2);
    } else {
        mDoAud_subBgmStart_l(player01::JA_BGM_ITEM_GET);
        dComIfGp_setMesgBgm_l(1);
    }
}

/* 023DC6A4 */
BOOL daPy_lk_c::resetActAnimeUpper(int upperIdx, f32 i_morf) {
    if (LK_upperAnmIdx() == 0x64 /* checkDashDamageAnime() */) {
        mDamageWaitTimer = 0x1E; /* HD: constant (GameCube m_HIO) */
    }
    Player01AnmRatioPack *ratio = &reinterpret_cast<Player01AnmRatioPack *>(
        mAnmRatioUpper)[upperIdx];   /* mAnmRatioUpper[upperIdx] */
    ratio->mpAnmTransform = nullptr; /* setAnmTransform(NULL) */
    for (s32 j = 0; j < (s32)ratio->mJointCount;
         j++) { /* setRatio(0.0f): HD keeps one ratio per joint */
        ratio->mpJointWeights[j] = 0.0f;
    }
    reinterpret_cast<daPy_anmHeap_c *>(m_anm_heap_upper)[upperIdx].mIdx =
        0xFFFF; /* m_anm_heap_upper[upperIdx].mIdx */
    Player01_initFrameCtrl(gabi::ea(&mFrameCtrlUpper[0]) + upperIdx * 0x10, 0);
    if (!(i_morf < 0.0f)) { /* i_morf >= 0.0f (taken on NaN) */
        Player01_initOldFrameMorf((u32)m_old_fdata, i_morf, 2, 0x1D);
    }
    resetPriTextureAnime();
    deleteArrow();
    return TRUE;
}

/* 023DCB54 */
void daPy_lk_c::setDamageCurseEmitter() {
    if (damageEffectType != 1) { /* !daPy_dmEcallBack_c::checkCurse() */
        endDamageEmitter();
        u32 mtx = lk_getAnmMtx(gabi::ea(mpCLModel.get()), CL_JNT_WAIST_JNT_e);
        Player01_makeMtxEmitter(gabi::ea(this) + 0x67CC, 0x815B /* ID_AK_SN_BUBBLECURSE00 */, mtx,
                                &current.pos, 0);
        if (checkUpperReadyThrowAnime()) {
            resetActAnimeUpper(2 /* UPPER_MOVE2_e */, -1.0f);
            freeRopeItem();
        }
    }
    cancelNoDamageMode();
    damageEffectType = 1; /* daPy_dmEcallBack_c::setCurse(200) */
    damageEffectTimer = 200;
}

/* 023DCF8C */
void daPy_lk_c::freeGrabItem() {
    u32 barrel = gabi::ea(mActorKeepGrab.mActor.get());
    if (barrel != 0) {
        fopAcM_cancelCarryNow(gabi::at<fopAc_ac_c>(barrel));
        gabi::at<fopAc_ac_c>(barrel)->shape_angle.z = 0; /* shape_angle.z */
        if (checkGrabBarrelSearch(0)) {
            reinterpret_cast<Player01BarrelState *>(gabi::at<void>(barrel))->m61C =
                0; /* daObjBarrel::Act_c::m61C */
        }
    }
    Player01_clearActorKeep(&mActorKeepGrab);
    u16 upper = LK_upperAnmIdx();
    if (upper == 0x95 || upper == 0x96) { /* checkGrabAnime() */
        resetActAnimeUpper(2 /* UPPER_MOVE2_e */, 2.4f /* HD: constant (GameCube m_HIO) */);
    }
    field_0x2b0 = 0.0f; /* field_0x2b0 */
    m35CC = 0.0f;
    mNoResetFlg0 = mNoResetFlg0 & ~0x400u; /* offNoResetFlg0(daPyFlg0_UNK400) */
    be<u32> *a =
        &Player01_game()
             ->play.mPlayerStatus[0][1]; /* dComIfGp_clearPlayerStatus1(0, daPyStts1_UNK40000_e) */
    *a = (u32)*a & ~0x40000u;
}

/* 023DD054 */
void daPy_lk_c::animeUpdate() {
    u32 upperPack0 = reinterpret_cast<Player01AnmRatioPack *>(mAnmRatioUpper)[0]
                         .mpAnmTransform.v; /* getNowAnmPackUpper(UPPER_MOVE0_e) */
    u32 underPack0 = reinterpret_cast<Player01AnmRatioPack *>(mAnmRatioUnder)[0]
                         .mpAnmTransform.v; /* getNowAnmPackUnder(UNDER_MOVE0_e) */
    u32 upperPack1 = reinterpret_cast<Player01AnmRatioPack *>(mAnmRatioUpper)[1].mpAnmTransform.v;
    u32 underPack1 = reinterpret_cast<Player01AnmRatioPack *>(mAnmRatioUnder)[1].mpAnmTransform.v;
    Player01_updateFrameCtrl(&mFrameCtrlUnder[0]);
    fcpy_l(underPack0, gabi::ea(&mFrameCtrlUnder[0].mFrame)); /* setFrame (HD: frame at +0) */
    if (underPack1 != 0) {
        Player01_updateFrameCtrl(&mFrameCtrlUnder[1]);
        fcpy_l(underPack1, gabi::ea(&mFrameCtrlUnder[1].mFrame));
    }
    if (upperPack0 != 0 && upperPack0 != underPack0) {
        Player01_updateFrameCtrl(&mFrameCtrlUpper[0]);
        fcpy_l(upperPack0, gabi::ea(&mFrameCtrlUpper[0].mFrame));
    }
    if (upperPack1 != 0 && upperPack1 != underPack1) {
        Player01_updateFrameCtrl(&mFrameCtrlUpper[1]);
        fcpy_l(upperPack1, gabi::ea(&mFrameCtrlUpper[1].mFrame));
    }
    u32 upperPack2 = reinterpret_cast<Player01AnmRatioPack *>(mAnmRatioUpper)[2].mpAnmTransform.v;
    if (upperPack2 != 0) {
        Player01_updateFrameCtrl(&mFrameCtrlUpper[2]);
        fcpy_l(upperPack2, gabi::ea(&mFrameCtrlUpper[2].mFrame));
    }
    u32 morf =
        reinterpret_cast<PlayerModelState *>(mD90)->mpParachuteFanMorf.v; /* mpParachuteFanMorf */
    if (morf != 0) {
        Player01_playMorf(morf, 0, 0, 0);
    }
}

/* 023DDF9C */
void daPy_lk_c::setSmallFanModel() {
    u32 bck = getItemAnimeResource(player01::dRes_INDEX_LKANM_BCK_FANWAIT_e);
    u32 oldHeap = gabi::ea(setItemHeap());
    u32 tmp_modelData =
        initModel(mpEquipItemModel_ea, player01::dRes_INDEX_LINK_BDL_FANSMALL_e, 0x37221222);
    BOOL ret = Player_initBckAnimation(
        gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mSwordAnim) /* mSwordAnim */,
        tmp_modelData, bck, 0, 2, 1.0f, 0, -1, 0);
    if (!ret) {
        JUT_ASSERT_fail("d_a_player_fan.inc", 0x126, "0");
    }
    mDoExt_setCurrentHeap(oldHeap);
    reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemModel->mUserArea =
        gabi::ea(this); /* setUserArea(this) */
    gabi::at<J3DJoint>(lk_jointNode(tmp_modelData, 2))
        ->setCallBack(daPy_fanJointCallback); /* FANSMA_JNT: daPy_fanJointCallback */
    gabi::at<J3DJoint>(lk_jointNode(tmp_modelData, 3))
        ->setCallBack(daPy_fanJointCallback); /* FANSMB_JNT */
    m355C = 0;
}

/* 023DE654 */
void daPy_lk_c::setHookshotModel() {
    u32 bck = getItemAnimeResource(player01::dRes_INDEX_LKANM_BCK_HOOKSHOTA_e);
    u32 oldHeap = gabi::ea(setItemHeap());
    u32 tmp_modelData =
        initModel(mpEquipItemModel_ea, player01::dRes_INDEX_LINK_BDL_HOOKSHOT_e, 0x37221222);
    BOOL ret =
        Player_initBckAnimation(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mSwordAnim),
                                tmp_modelData, bck, 0, 2, 1.0f, 0, -1, 0);
    if (!ret) {
        JUT_ASSERT_fail("d_a_player_hook.inc" /* "d_a_player_hook.inc" */, 100, "0");
    }
    mDoExt_setCurrentHeap(oldHeap);
    m35EC = 0.0f;
}

/* 023DEDE4 */
void daPy_lk_c::setPhotoBoxModel() {
    u32 oldHeap = gabi::ea(setItemHeap());
    initModel(mpEquipItemModel_ea, player01::dRes_INDEX_LINK_BDL_CAMERA_e, 0x37221222);
    mDoExt_setCurrentHeap(oldHeap);
    u32 data = gabi::ea(reinterpret_cast<PlayerModelState *>(mD90)
                            ->mpEquipItemModel->mModelData.get()); /* getModelData() */
    u32 shape = gabi::ea(gabi::at<PlayerJointNodeView>(lk_jointNode(data, 2))
                             ->mMesh->mShape.get()); /* getMesh()->getShape() */
    if (mEquipItem == dItemNo_PICTO_BOX_e) {
        gabi::at<J3DShape>(shape)->hide(); /* hide() */
    } else {
        gabi::at<J3DShape>(shape)->show(); /* show() */
    }
}

/* 023DEF04 */
void daPy_lk_c::setTactModel() {
    if (mEquipItem == player01::dItemNo_WIND_WAKER_e) {
        return;
    }
    Player01_deleteEquipItem(this, FALSE);
    mEquipItem = 0x22;
    u32 oldHeap = gabi::ea(setItemHeap());
    u32 modelData =
        initModel(mpEquipItemModel_ea, player01::dRes_INDEX_LINK_BDL_TAKT_e, 0x37221222);
    /* HD: the brk comes from the resident "LkAnm" archive into the brkAnm object at 0x48F8 */
    gabi::Local<SafeString> key;
    key->__vtbl = Player01SafeStringVtable;
    key->mStringTop = gabi::guest_str("LkAnm");
    u32 brk =
        Player_getArchiveResource(dComIfG_resControl(), key.get(), dRes_INDEX_LKANM_BRK_TTAKT_e);
    s32 mode = anm_vf0C_l(brk);
    Player_initBrkAnimation(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBrk),
                            modelData, brk, 1, mode, 1.0f, 0, -1,
                            reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBrk.mpAnm.v != 0,
                            0);
    Player01_entryBrk(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBrk),
                      modelData, 0.0f);
    mDoExt_setCurrentHeap(oldHeap);
}

/* 023DF020 */
void daPy_lk_c::setHammerModel() {
    u32 bck = getItemAnimeResource(player01::dRes_INDEX_LKANM_BCK_HAMMERDAM_e);
    u32 oldHeap = gabi::ea(setItemHeap());
    u32 tmp_modelData =
        initModel(mpEquipItemModel_ea, player01::dRes_INDEX_LINK_BDL_HAMMER_e, 0x37221222);
    BOOL ret =
        Player_initBckAnimation(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mSwordAnim),
                                tmp_modelData, bck, 0, 2, 1.0f, 0, -1, 0);
    if (!ret) {
        JUT_ASSERT_fail("d_a_player_hammer.inc", 0x20, "0");
    }
    u32 blur = mpSwBlur; /* HD: daPy_swBlur_c is allocated */
    reinterpret_cast<PlayerSwordBlurView *>(gabi::at<void>(blur))->mpPosBuffer =
        operator_new_array_align(0x4800, 0x20); /* mpPosBuffer = new (0x20) Vec[2 * 0x300] */
    mDoExt_setCurrentHeap(oldHeap);
    m35EC = 0.0f;
}

/* 023E0530 */
void daPy_lk_c::getUnderUpperAnime(u32 anmIndex, u32 pUnderBck, u32 pUpperBck, int r7,
                                   u32 bufferSize) {
    daPy_anmHeap_c *under =
        &reinterpret_cast<daPy_anmHeap_c *>(m_anm_heap_under)[r7]; /* m_anm_heap_under[r7] */
    u16 underIdx =
        (u16) reinterpret_cast<Player01AnmIndex *>(gabi::at<void>(anmIndex))->mUnderBckIdx;
    if ((u16)under->mIdx != underIdx) {
        *reinterpret_cast<be<u32> *>(gabi::at<void>(pUnderBck)) =
            getAnimeResource(gabi::ea(under), underIdx, bufferSize);
    } else {
        *reinterpret_cast<be<u32> *>(gabi::at<void>(pUnderBck)) =
            (u32) reinterpret_cast<Player01AnmRatioPack *>(mAnmRatioUnder)[r7]
                .mpAnmTransform.v; /* getNowAnmPackUnder */
    }
    u16 upperIdx =
        (u16) reinterpret_cast<Player01AnmIndex *>(gabi::at<void>(anmIndex))->mUpperBckIdx;
    if ((u16) reinterpret_cast<Player01AnmIndex *>(gabi::at<void>(anmIndex))->mUnderBckIdx !=
        upperIdx) {
        if (bufferSize == 0xB400) {
            bufferSize = 0x4800;
        }
        daPy_anmHeap_c *upper = &reinterpret_cast<daPy_anmHeap_c *>(m_anm_heap_upper)[r7];
        if ((u16)upper->mIdx != upperIdx) {
            *reinterpret_cast<be<u32> *>(gabi::at<void>(pUpperBck)) =
                getAnimeResource(gabi::ea(upper), upperIdx, bufferSize);
        } else {
            *reinterpret_cast<be<u32> *>(gabi::at<void>(pUpperBck)) =
                (u32) reinterpret_cast<Player01AnmRatioPack *>(mAnmRatioUpper)[r7]
                    .mpAnmTransform.v; /* getNowAnmPackUpper */
        }
    } else {
        *reinterpret_cast<be<u32> *>(gabi::at<void>(pUpperBck)) = 0;
        reinterpret_cast<daPy_anmHeap_c *>(m_anm_heap_upper)[r7].mIdx = 0xFFFF;
    }
}

/* ======== batch 4 ======== */
/* checkBossBgm (unnamed by the matcher) */
/* 023DD1D8 */
BOOL daPy_lk_c::checkBossBgm() {
    u32 mainBgm = Player01_checkMainBgm();
    u32 subBgm = Player01_checkSubBgm();
    /* HD BGM ids (the GameCube list: GOMA .. BGN_HAYAMUSHI, MBOSS .. DIOCTA_2) */
    if (mainBgm == 0x80000003 || mainBgm == 0x80000005 || mainBgm == 0x80000010 ||
        mainBgm == 0x80000014 || mainBgm == 0x80000015 || mainBgm == 0x80000023 ||
        mainBgm == 0x80000029 || (mainBgm >= 0x80000048 && mainBgm <= 0x8000004D) ||
        mainBgm == 0x80000054 || (mainBgm >= 0x80000059 && mainBgm <= 0x8000005B) ||
        subBgm == 0x80000019 || subBgm == 0x8000001A || subBgm == 0x8000002B ||
        subBgm == 0x80000041 || subBgm == 0x80000046 || subBgm == 0x80000047) {
        return TRUE;
    }
    return FALSE;
}

/* 023DDD48 */
void daPy_lk_c::resetDemoTextureAnime() {
    daPy_anmHeap_c *h = reinterpret_cast<daPy_anmHeap_c *>(
        m_tex_anm_heap); /* m_tex_anm_heap: mIdx, field_0x2, field_0x4, field_0x6 */
    if ((u16)h->field_0x4 != 0xFFFF) {
        u16 idx2 = (u16)h->field_0x2;
        h->field_0x6 = 0xFFFF;
        h->field_0x4 = 0xFFFF;
        if (idx2 != 0xFFFF) {
            Player01_setTextureAnimeResource(this, loadTextureAnimeResource(idx2, FALSE), 0);
        } else if ((u16)h->mIdx != 0xFFFF) {
            Player01_setTextureAnimeResource(this, loadTextureAnimeResource((u16)h->mIdx, FALSE),
                                             0);
        } else {
            Player01_setTextureAnime(this, 0, 0);
        }
    }
    h = reinterpret_cast<daPy_anmHeap_c *>(m_tex_scroll_heap); /* m_tex_scroll_heap */
    if ((u16)h->field_0x4 != 0xFFFF) {
        u16 idx2 = (u16)h->field_0x2;
        h->field_0x6 = 0xFFFF;
        h->field_0x4 = 0xFFFF;
        if (idx2 != 0xFFFF) {
            setTextureScrollResource(loadTextureScrollResource(idx2, FALSE), 0);
        } else if ((u16)h->mIdx != 0xFFFF) {
            setTextureScrollResource(loadTextureScrollResource((u16)h->mIdx, FALSE), 0);
        } else {
            Player01_setTextureAnime(this, 0, 0);
        }
    }
}

/* 023E0660 */
void daPy_lk_c::initSeAnime() {
    u32 fc = mpSeAnmFrameCtrl;
    if (fc != 0) { /* HD: the frame control is only used inside the NULL check */
        s32 dir = (f32)Player01_frameCtrl(fc)->mRate < 0.0f ? -1 : 1;
        f32 loop = (f32)(s16)Player01_frameCtrl(fc)->mLoop;
        /* HD: JAIZelAnime::initActorAnimSound(buffer (0x658C), dir, f32 loop) */
        Player01_initActorAnimSound(gabi::ea(this) + 0x64F0,
                                    (u32) * reinterpret_cast<be<u32> *>(_658C), dir, loop);
        fc = mpSeAnmFrameCtrl;
        if ((u8)Player01_frameCtrl(fc)->mAttribute == 2 /* J3DFrameCtrl::EMode_LOOP */) {
            f32 position = gabi::fsubs_ppc((f32)Player01_frameCtrl(fc)->mFrame,
                                           (f32)Player01_frameCtrl(fc)->mRate);
            if (!(position > (f32)(s16)Player01_frameCtrl(fc)->mStart)) {
                position = gabi::fsubs_ppc((f32)(s16)Player01_frameCtrl(fc)->mEnd, position);
            } else if (!(position < (f32)(s16)Player01_frameCtrl(fc)->mEnd)) {
                position = gabi::fsubs_ppc(position, (f32)(s16)Player01_frameCtrl(fc)->mEnd);
            }
            Player01_setPlayPosition(gabi::ea(this) + 0x64F0, position);
        }
    }
}

/* ======== batch 5: joint callbacks, attention ======== */
/* a matrix copy through FPRs (twelve lfs, then twelve stfs: SNaNs are quieted) */
static inline void mtx_copy_q(u32 dst, u32 src) { mtx_copy(gabi::at<Mtx34>(dst), gabi::at<Mtx34>(src)); }
static inline void PSMTXConcat_l(u32 a, u32 b, u32 ab) { gabi::call(0x028E9108, a, b, ab); }
static inline void PSMTXCopy_l(u32 src, u32 dst) { gabi::call(0x028E90D4, src, dst); }
#define LK_MTX_STACK 0x1048D0CCu  /* mDoMtx_stack_c::now */
#define LK_J3DSYS_CURMTX 0x104B4868u /* J3DSys::mCurrentMtx */
/* mpEquipItemModel->getAnmMtx(jnt) / setAnmMtx(jnt, src) (HD: both mark the joint matrices dirty) */
#define LK_equipAnmMtx(jnt) lk_getAnmMtx(gabi::load<u32>(mpEquipItemModel_ea), (jnt))

/* 023D6230 */
BOOL daPy_lk_c::bowJointCB(int param_0) {
    if (checkBowReadyAnime()) {
        return TRUE;
    }
    if (param_0 == 6) {
        mDoMtx_stack_c::transS(0.0f, 4.5f, 4.5f);
        PSMTXConcat_l(LK_equipAnmMtx(6 /* LINK_BOW_JNT_LINEAB_JNT_e */), LK_MTX_STACK, LK_MTX_STACK);
        mtx_copy_q(LK_equipAnmMtx(param_0), LK_MTX_STACK); /* setAnmMtx(param_0, mDoMtx_stack_c::get()) */
    } else {
        u32 src = LK_equipAnmMtx(6);
        mtx_copy_q(LK_equipAnmMtx(0xB /* LINK_BOW_JNT_LINEBB_JNT_e */), src);
    }
    return TRUE;
}

/* 023D641C */
BOOL daPy_lk_c::fanJointCB(int param_0) {
    if (param_0 == 2 /* FANSMALL_JNT_FANSMA_JNT_e */) {
        Player_ZrotS(LK_MTX_STACK, (s32)m3558);
    } else {
        Player_ZrotS(LK_MTX_STACK, (s32)m355A);
    }
    PSMTXConcat_l(LK_equipAnmMtx(param_0), LK_MTX_STACK, LK_MTX_STACK);
    mtx_copy_q(LK_equipAnmMtx(param_0) /* FANSMALL_JNT_FANSMROOT_JNT_e * param_0 */, LK_MTX_STACK);
    PSMTXCopy_l(LK_MTX_STACK, LK_J3DSYS_CURMTX);
    return TRUE;
}

/* 023D6650 */
BOOL daPy_lk_c::parachuteJointCB(int param_0) {
    if (param_0 == 3 /* FANB_JNT_LARMB_e */ || param_0 == 9 /* FANB_JNT_RARMB_e */) {
        mDoMtx_stack_c::transS(m3600, 0.0f, 0.0f);
    } else if (param_0 == 1 /* FANB_JNT_LROOT_e */ || param_0 == 7 /* FANB_JNT_RROOT_e */) {
        Player_YrotS(LK_MTX_STACK, (s32)m355E);
    } else {
        return TRUE;
    }
    PSMTXConcat_l(LK_equipAnmMtx(param_0), LK_MTX_STACK, LK_MTX_STACK);
    mtx_copy_q(LK_equipAnmMtx(param_0) /* FANB_JNT_LROOT_e * param_0 */, LK_MTX_STACK);
    PSMTXCopy_l(LK_MTX_STACK, LK_J3DSYS_CURMTX);
    return TRUE;
}

/* 023DCD30 */
void daPy_lk_c::setAtnList() {
    u32 att = dComIfGp_ea() + 0x5804; /* &dComIfGp_getAttention() */
    mpAttention = att;
    mpAttnActorHD = nullptr; /* HD: the fourth item button */
    mpAttnEntryY = 0;
    mpAttnEntryX = 0;
    mpAttnEntryA = 0;
    mpAttnActorY = nullptr;
    mpAttnActorAction = nullptr;
    mpAttnActorZ = nullptr;
    mpAttnActorX = nullptr;
    mpAttnActorLockOn = nullptr;
    mpAttnEntryHD = 0;
    mpAttnEntryZ = 0;
    mpAttnActorA = nullptr;
    /* checkAttentionLock(): dAttention_c::LockonTruth() || the lock flag (+0x20 & 0x20000000) */
    if (Player01_lockonTruth(att) ||
        ((u32) reinterpret_cast<Player01Attention *>(gabi::at<void>(att))->mLockFlags &
         0x20000000)) {
        be<u32> *a = &Player01_game()->play.mPlayerStatus[0][0]; /* dComIfGp_clearPlayerStatus0(0,
                                                                    daPyStts0_BOOMERANG_WAIT_e) */
        *a = (u32)*a & ~0x400000u;
    }
    if (mActorKeepThrow.mActor != nullptr &&
        ((u32)Player01_game()->play.mPlayerStatus[0][0] & 0x400000)) {
        mpAttnActorA = mActorKeepThrow.mActor;
        mpAttnActorLockOn = mpAttnActorA;
    } else {
        att = mpAttention;
        if (Player01_lockonTruth(att) ||
            ((u32) reinterpret_cast<Player01Attention *>(gabi::at<void>(att))->mLockFlags &
             0x20000000)) {
            mpAttnEntryA = Player01_getLockonList((u32)mpAttention, 0);
            if (mpAttnEntryA != 0 && Player01_lockonTruth((u32)mpAttention)) {
                fopAc_ac_c *actor = Player01_getAttentionActor((u32)mpAttnEntryA);
                mpAttnActorLockOn = actor;
                mpAttnActorA = actor;
                setResetFlg0(resetFlg0() | 0x10000); /* onResetFlg0(daPyRFlg0_ATTENTION_LOCK) */
            }
            m34E6 = shape_angle.y;
        }
    }
    if (mpAttnActorLockOn == nullptr) {
        u32 e = Player01_getActionBtnB((u32)mpAttention);
        mpAttnEntryA = e;
        if (e != 0) {
            fopAc_ac_c *actor = Player01_getAttentionActor(e);
            mpAttnActorA = actor;
            mpAttnActorAction = actor;
        }
    }
    u32 e = Player01_getActionBtnX((u32)mpAttention);
    mpAttnEntryX = e;
    if (e != 0) {
        mpAttnActorX = Player01_getAttentionActor(e);
    }
    e = Player01_getActionBtnY((u32)mpAttention);
    mpAttnEntryY = e;
    if (e != 0) {
        mpAttnActorY = Player01_getAttentionActor(e);
    }
    e = Player01_getActionBtnZ((u32)mpAttention);
    mpAttnEntryZ = e;
    if (e != 0) {
        mpAttnActorZ = Player01_getAttentionActor(e);
    }
    e = Player01_getActionBtnHD((u32)mpAttention);
    mpAttnEntryHD = e;
    if (e != 0) {
        mpAttnActorHD = Player01_getAttentionActor(e);
    }
}

/* ======== batch 6: animation resources ======== */
/* sead::SafeString compare as GHS inlined it: equal pointers, else a byte compare of at most 0x40001 characters */
static inline bool lk_safeStrEq(u32 a, u32 b) {
    if (a == b) {
        return true;
    }
    for (u32 n = 0; n < 0x40001; n++) {
        u8 x = gabi::load<u8>(a + n);
        u8 y = gabi::load<u8>(b + n);
        if (x != y) {
            return false;
        }
        if (x == 0) {
            return true;
        }
    }
    return false;
}
/* a self-relative offset field: ptr + *ptr, or NULL when the offset is 0 */
static inline u32 lk_relPtr(u32 p) {
    s32 off = gabi::load<s32>(p);
    return off != 0 ? p + off : 0;
}

/* the texture pattern's materials (HD): each material's animation index entry (array at 0x6D4) links to the model
 * material of the same name; "mouth" gets its own (unlinked) entry */
static void lk_linkTexMaterials(daPy_lk_c* lk, u32 key) {
    u32 self = gabi::ea(lk);
    u32 data = gabi::load<u32>(self + 0x6E0);
    u16 num = gabi::load<u16>(data + 0x16); /* material count */
    gabi::Local<SafeString> name;
    gabi::Local<SafeString> mouth;
    for (u32 i = 0; i < num; i++) {
        u32 entry = lk_relPtr(data + 0x2C) + i * 0x1C;
        u32 str = lk_relPtr(entry + 0xC);
        u32 md = gabi::load<u32>(self + 0x444);
        name->__vtbl = 0x10034B24;
        name->mStringTop = str;
        u32 x = PlayerModelData_getResource(md);
        SafeString_vf14_l(gabi::ea(name.get()));
        u32 matIdx = gabi::call<u32>(0x027DF9B0 /* material name -> index */, lk_relPtr(x + 0x18), (u32)name->mStringTop);
        mouth->mStringTop = key; /* "mouth" */
        mouth->__vtbl = 0x10034B24;
        SafeString_vf14_l(gabi::ea(name.get()));
        SafeString_vf14_l(gabi::ea(name.get()));
        u32 a = name->mStringTop;
        SafeString_vf14_l(gabi::ea(mouth.get()));
        u32 arr = gabi::load<u32>(self + 0x6D4);
        if (lk_safeStrEq(a, mouth->mStringTop)) {
            gabi::store<u32>(arr + i * 4, gabi::load<u32>(arr + i * 4) | 0xC0007FFF);
            arr = gabi::load<u32>(self + 0x6D4);
            gabi::store<u32>(arr + matIdx * 4, gabi::load<u32>(arr + matIdx * 4) | 0x3FFF8000);
        } else {
            gabi::store<u32>(arr + i * 4, gabi::load<u32>(arr + i * 4) & 0x3FFF8000);
            arr = gabi::load<u32>(self + 0x6D4);
            gabi::store<u32>(arr + i * 4, gabi::load<u32>(arr + i * 4) | ((matIdx + 1) & 0x7FFF));
            arr = gabi::load<u32>(self + 0x6D4);
            u32 k = (matIdx + 1) * 4;
            gabi::store<u32>(arr + k, gabi::load<u32>(arr + k) & 0xC0007FFF);
            arr = gabi::load<u32>(self + 0x6D4);
            gabi::store<u32>(arr + k, gabi::load<u32>(arr + k) | ((i << 15) & 0x3FFF8000));
        }
    }
}

/* setTextureAnimeResource (unnamed by the matcher) */
/* 023DC110 */
void daPy_lk_c::setTextureAnimeResource(u32 btp, int r31) {
    mpAnmTexPatternData = btp;
    m3530 = (u16)r31;
    /* HD: the texture pattern drives two animation objects (0x644 and 0x69C) */
    PlayerTexPattern_setAnm(gabi::ea(this) + 0x644,
                            (u32)gabi::at<PlayerTextureResource>(btp)->mpData);
    PlayerTexPattern_setModelData(gabi::ea(this) + 0x644,
                                  PlayerModelData_getResource(gabi::ea(mpCLModelData.get())));
    PlayerTexPattern_setAnm(gabi::ea(this) + 0x69C,
                            (u32)gabi::at<PlayerTextureResource>((u32)mpAnmTexPatternData)->mpData);
    PlayerTexPattern_setModelData(gabi::ea(this) + 0x69C,
                                  PlayerModelData_getResource(gabi::ea(mpCLModelData.get())));
    lk_linkTexMaterials(this, gabi::guest_str("mouth"));
}

/* 023DC7AC */
void daPy_lk_c::deleteEquipItem(BOOL param_1) {
    u32 equip_actor = gabi::ea(mActorKeepEquip.mActor.get());
    u16 item;
    bool checkSword = true;
    bool sword = false;
    if (param_1) {
        item = mEquipItem;
        if (item == 0x100 || item == 0x101) {
            checkSword = false;
        } else {
            seStartOnlyReverb(item == 0x103 ? 0x280A : 0x2810);
        }
    }
    if (checkSword) {
        item = mEquipItem;
        if (item == 0x103) {
            // HD: the callee also reads the item register.
            Player01_stopSwordBgm(0, (u32)item);
            item = mEquipItem;
            sword = true;
        }
    }
    if (!sword && equip_actor != 0) {
        if (item == 0x101) {
            fopAcM_cancelCarryNow(gabi::at<fopAc_ac_c>(equip_actor));
        } else {
            fopAcM_delete(gabi::at<fopAc_ac_c>(equip_actor));
        }
        item = mEquipItem;
    }
    bool reset = false;
    if (item == player01::dItemNo_GRAPPLING_HOOK_e) {
        if (checkRopeAnime()) {
            reset = true;
        } else {
            item = mEquipItem;
        }
    }
    if (!reset) {
        if (item == player01::dItemNo_BOOMERANG_e) {
            reset = checkBoomerangAnime();
        } else if (item == dItemNo_HOOKSHOT_e) {
            reset = LK_upperAnmIdx() == 0xA7; /* checkHookshotReadyAnime() */
        }
        if (!reset && checkBowItem(item) && checkBowAnime()) {
            reset = true;
        }
    }
    if (reset) {
        resetActAnimeUpper(2 /* UPPER_MOVE2_e */, -1.0f);
    }
    Player01_clearActorKeep(&mActorKeepEquip);
    u32 bckAnm = reinterpret_cast<Player01BckRuntime *>(
                     &reinterpret_cast<PlayerModelState *>(mD90)->mSwordAnim)
                     ->mpBck; /* mSwordAnim.getBckAnm() */
    mEquipItem = 0x100;
    reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemModel = nullptr;
    if (bckAnm != 0) {
        Player01_changeBckOnly(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mSwordAnim),
                               0);
    }
    reinterpret_cast<PlayerModelState *>(mD90)->mpParachuteFanMorf.v = 0; /* mpParachuteFanMorf */
    /* HD: the item btk/brk animations are objects, re-initialised without data */
    Player_initBtkAnimation(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBtk),
                            0, 0, 1, 2, 1.0f, 0, -1, 0, 0); /* mpEquipItemBtk */
    reinterpret_cast<PlayerModelState *>(mD90)->mpBottleContentsModel.v =
        0;                                                              /* mpBottleContentsModel */
    reinterpret_cast<PlayerModelState *>(mD90)->mpBottleCapModel.v = 0; /* mpBottleCapModel */
    Player_initBrkAnimation(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBrk),
                            0, 0, 1, 2, 1.0f, 0, -1, 0, 0); /* mpEquipItemBrk */
    Player_initBtkAnimation(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpSwordBtk), 0, 0,
                            1, 2, 1.0f, 0, -1, 0, 0);                /* mpSwordBtk */
    reinterpret_cast<PlayerModelState *>(mD90)->mpSwordModel1.v = 0; /* mpSwordModel1 */
    Player01_endMtxEmitter(gabi::ea(&m3454));
    if (reinterpret_cast<PlayerModelState *>(mD90)->mpSwordTipStabModel.v !=
        0) { /* HD: clears three fields of the sword tip model's data */
        reinterpret_cast<Player01SwordTipModelData *>(
            reinterpret_cast<PlayerModelState *>(mD90)->mpSwordTipStabModel->mModelData.get())
            ->mTextureScroll = 0;
        reinterpret_cast<Player01SwordTipModelData *>(
            reinterpret_cast<PlayerModelState *>(mD90)->mpSwordTipStabModel->mModelData.get())
            ->mMaterialAnimation = 0;
        reinterpret_cast<Player01SwordTipModelData *>(
            reinterpret_cast<PlayerModelState *>(mD90)->mpSwordTipStabModel->mModelData.get())
            ->mTexturePattern = 0;
    }
    u32 f1 = noResetFlg1();
    reinterpret_cast<PlayerModelState *>(mD90)->mpSwordTipStabModel.v = 0; /* mpSwordTipStabModel */
    setNoResetFlg1(f1 & ~0x200000u); /* offNoResetFlg1(daPyFlg1_UNK200000) */
}

/* 023DE7E8 */
BOOL daPy_lk_c::setActAnimeUpper(u32 bckIdx, int upperIdx, f32 rate, f32 start, int end,
                                 f32 i_morf) {
    u32 bck = Player01_getAnimeResource(
        this, gabi::ea(&reinterpret_cast<daPy_anmHeap_c *>(m_anm_heap_upper)[upperIdx]), bckIdx,
        0x2400);
    resetPriTextureAnime();
    Player01AnmRatioPack *ratio =
        &reinterpret_cast<Player01AnmRatioPack *>(mAnmRatioUpper)[upperIdx];
    ratio->mpAnmTransform.v = bck;                      /* setAnmTransform(bck) */
    for (s32 j = 0; j < (s32)ratio->mJointCount; j++) { /* setRatio(1.0f) */
        ratio->mpJointWeights[j] = 1.0f;
    }
    if (end < 0) {
        end = Player01_animation(bck)->getFrameMax(); /* getFrameMax() */
    }
    u32 fc = gabi::ea(&mFrameCtrlUpper[0]) + upperIdx * 0x10;
    if (rate < 0.0f) {
        f32 frame = (f32)end - 0.001f;
        u32 attr = Player01_animation(bck)->getAttribute(); /* getAttribute() */
        Player01_setFrameCtrl(this, fc, attr, (s32)(s16)gabi::ftoi(start), end, rate, frame);
        Player01_animation(bck)->mFrame = frame; /* setFrame (HD: frame at +0) */
    } else {
        u32 attr = Player01_animation(bck)->getAttribute();
        Player01_setFrameCtrl(this, fc, attr, (s32)(s16)gabi::ftoi(start), end, rate, start);
        Player01_animation(bck)->mFrame = start;
    }
    if (!(i_morf < 0.0f)) {
        Player01_initOldFrameMorf((u32)m_old_fdata, i_morf, 2, 0x1D);
    }
    return TRUE;
}

/* 023E07E4 */
void daPy_lk_c::setSeAnime(u32 anmRes, u32 anmHeap, u32 frameCtrl) {
    /* HD: mpSeAnm (0x6938) is a sead::SafeString naming the SE animation; the name is stored behind
     * the bck data */
    u32 buf = (u32)Player01_animation(anmRes)->mpMetadata;
    gabi::Local<SafeString> name;
    name->mStringTop = lk_relPtr(buf + 4);
    name->__vtbl = Player01SafeStringVtable;
    SafeString_vf14_l(gabi::ea(this) + 0x6938);
    SafeString_vf14_l(gabi::ea(this) + 0x6938);
    u32 cur = mpSeAnm;
    SafeString_vf14_l(gabi::ea(name.get()));
    bool same = false;
    if (cur == name->mStringTop) {
        same = true;
    } else {
        same = lk_safeStrEq(mpSeAnm, name->mStringTop);
    }
    if (same &&
        m34F0 == (u16) reinterpret_cast<daPy_anmHeap_c *>(gabi::at<void>(anmHeap))->field_0x6 &&
        !(gabi::fmuls_ppc(mSeAnmRate, (f32)Player01_frameCtrl(frameCtrl)->mRate) < 0.0f)) {
        mpSeAnmFrameCtrl = frameCtrl;
        return;
    }
    gabi::Local<SafeString> key;
    key->__vtbl = Player01SafeStringVtable;
    key->mStringTop = lk_relPtr((u32)Player01_animation(anmRes)->mpMetadata + 4);
    u32 sanm = Player01_getSoundAnimeResource(dComIfG_resControl(), key.get(), anmRes + 0xC);
    *reinterpret_cast<be<u32> *>(_658C) = sanm;
    if (sanm == 0) {
        resetSeAnime();
        Player01_initActorAnimSound(gabi::ea(this) + 0x64F0, 0, 1, 0.0f);
        return;
    }
    mpSeAnmFrameCtrl = frameCtrl;
    mpSeAnm = lk_relPtr((u32)Player01_animation(anmRes)->mpMetadata + 4);
    m34F0 = (u16) reinterpret_cast<daPy_anmHeap_c *>(gabi::at<void>(anmHeap))->field_0x6;
    fcpy_l(gabi::ea(&mSeAnmRate), frameCtrl + 0); /* mSeAnmRate = frameCtrl->getRate() */
    initSeAnime();
}

/* ======== batch 7 ======== */
/* 023D813C */
static void daPy_sightPacket_setSight(u32 self) {
    auto* sight = gabi::at<daPy_sightPacket_c>(self);
    /* HD: mDoLib_project takes the viewport, whose height scales the sight (480 lines = 1) */
    gabi::Local<view_port_class> vp; /* 025F1084 copies a 0x28-byte view/viewport record (5 x 2 words); was 0x18 (game test: frame guard) */
    Player_getViewport(vp.get());
    f32 scr = (f32)vp->mHeight / 480.0f;
    gabi::Local<cXyz> proj;
    Player_project(&sight->mPos, proj.get(), vp.get());
    mDoMtx_stack_c::transS(proj->x, proj->y, proj->z);
    if ((u8)sight->mLockFlag /* mLockFlag */) {
        u8 frame = (u8)sight->mFrame; /* mFrame */
        f32 var_f2;
        f32 var_f31;
        if (frame < 13) {
            var_f2 = 1.0f - (f32)(s32)frame / 13.0f;
            sight->mLockAlpha = (u8)gabi::ftoi(gabi::fmadds(30.0f, var_f2, 150.0f)); /* mLockAlpha */
            var_f31 = -var_f2;
        } else {
            var_f2 = (f32)(s32)(frame - 13) / 13.0f;
            sight->mLockAlpha = (u8)gabi::ftoi(gabi::fmadds(30.0f, var_f2, 150.0f));
            var_f31 = var_f2;
        }
        f32 div = 4.0f / scr;
        PlayerSightImage* img = sight->mpImg.get(); /* mpImg */
        f32 w = (f32)(u16)img->width;
        f32 h = (f32)(u16)img->height;
        f32 temp_f3 = gabi::fmadds(0.35f, var_f2, 0.65f);
        mDoMtx_stack_c::scaleM((temp_f3 * w) / div, (temp_f3 * h) / div, temp_f3);
        // HD: the angle is converted to the fixed-angle sine/cosine table index.
        u16 a = (u16)gabi::ftoi(16384.0f * var_f31);
        f32 s = cM_ssin(a);
        Player_ZrotNative(LK_MTX_STACK, (s32)(s16)gabi::ftoi(32768.0f * s));
    } else {
        f32 sc = 10.0f * scr;
        mDoMtx_stack_c::scaleM(sc, sc, sc);
    }
    PSMTXCopy_l(LK_MTX_STACK, gabi::ea(&sight->mMtx));
    auto* list = gabi::at<PlayerDrawList2D>(dComIfGp_ea() + 0x5D30);
    Player_listSet(gabi::ea(list), gabi::ea(&list->mpFirst), gabi::ea(&list->mpLast), self);
}

/* 023DFB18 */
void daPy_lk_c::setBgCheckParam() {
    /* mAcchCir[i] (0x40 each from 0x74C): SetWallH stores +0x30, GetWallR reads +0x34 */
    dBgS_AcchCir *cir0 = &mAcchCir[0];
    dBgS_AcchCir *cir1 = &mAcchCir[1];
    dBgS_AcchCir *cir2 = &mAcchCir[2];
    s32 proc = mCurProc;
    cir2->m_wall_h = 125.0f;
    cir1->m_wall_h = 89.9f;
    cir0->m_wall_h = 30.1f;
    if (proc == player01::daPyProc_ROPE_SWING_START_e) {
        cir2->m_wall_h = 0.0f;
        cir0->m_wall_h = -125.0f;
        cir1->m_wall_h = -89.9f;
        /* HD: mAcchCir[0]'s radius is left as it is */
        Player01_setWallR(cir1, (f32)cir0->m_wall_r);
        Player01_setWallR(cir2, (f32)cir0->m_wall_r);
        return;
    }
    u32 mode = 0;
    if (proc == player01::daPyProc_LARGE_DAMAGE_WALL_e ||
        ((mode = mModeFlg) & 0x410000) /* ModeFlg_CLIMB | ModeFlg_LADDER */) {
        Player01_setWallR(cir0, 5.0f);
    } else if (mode & player01::ModeFlg_PUSHPULL) {
        Player01_setWallR(cir0, 40.0f);
    } else if (mode & player01::ModeFlg_WHIDE) {
        Player01_setWallR(cir0, 8.5f); /* HD: constant (GameCube m_HIO) */
        if (mNoResetFlg0 & daPyFlg0_UNK10000) {
            cir2->m_wall_h = 89.9f;
        }
    } else if (mode & player01::ModeFlg_SWIM) {
        Player01_setWallR(cir0, (f32) reinterpret_cast<Player01SwimDefaults *>(0x1046CCE8)
                                    ->mSwimWallRadius); /* HD: a global (GameCube 67.5f) */
        cir1->m_wall_h = 0.0f;
        cir0->m_wall_h = -5.0f;
        cir2->m_wall_h = 20.0f;
    } else if ((mode & player01::ModeFlg_CRAWL) && proc != player01::daPyProc_CRAWL_END_e) {
        Player01_setWallR(cir0, 30.0f);
        cir1->m_wall_h = 50.0f;
        cir2->m_wall_h = 50.0f;
        cir0->m_wall_h = 10.0f;
    } else if (mode & player01::ModeFlg_HANG) {
        Player01_setWallR(cir0, 12.5f);
        cir0->m_wall_h = 25.0f;
        cir2->m_wall_h = 25.0f;
        cir1->m_wall_h = 25.0f;
    } else if (field_0x2b0 < 0.0f /* checkGrabWear() */) {
        Player01_setWallR(cir0, 50.0f);
    } else {
        Player01_setWallR(cir0, 35.0f);
    }
    Player01_setWallR(cir1, (f32)cir0->m_wall_r);
    Player01_setWallR(cir2, (f32)cir0->m_wall_r);
}

/* hideHatAndBackle (unnamed by the matcher) */
/* 023D9074 */
void daPy_lk_c::hideHatAndBackle(u32 mtl) {
    /* HD: the materials are found by name (three sead::SafeString globals) instead of by index */
    gabi::Local<SafeString> s1;
    gabi::Local<SafeString> s2;
    gabi::Local<SafeString> s3;
    for (; mtl != 0; mtl = gabi::ea(gabi::at<PlayerMaterialAnimationView>(mtl)->mNext.get()) /* getNext() */) {
        bool hide = false;
        if (!(noResetFlg1() & 0x800)) { /* !checkFreezeState() */
            s1->__vtbl = 0x10034B24;
            s1->mStringTop = lk_relPtr((u32)gabi::at<PlayerMaterialAnimationView>(mtl)->mpData + 4); /* the material's name */
            SafeString_vf14_l(gabi::ea(&faceMaterialName));
            SafeString_vf14_l(gabi::ea(&faceMaterialName));
            u32 g = (u32)faceMaterialName.mStringTop;
            SafeString_vf14_l(gabi::ea(s1.get()));
            if (g == s1->mStringTop || lk_safeStrEq((u32)faceMaterialName.mStringTop, s1->mStringTop)) {
                hide = true;
            } else {
                s2->__vtbl = 0x10034B24;
                s2->mStringTop = lk_relPtr((u32)gabi::at<PlayerMaterialAnimationView>(mtl)->mpData + 4);
                SafeString_vf14_l(gabi::ea(&ear2MaterialName));
                SafeString_vf14_l(gabi::ea(&ear2MaterialName));
                g = (u32)ear2MaterialName.mStringTop;
                SafeString_vf14_l(gabi::ea(s2.get()));
                if (g == s2->mStringTop || lk_safeStrEq((u32)ear2MaterialName.mStringTop, s2->mStringTop)) {
                    hide = true;
                }
            }
        }
        if (!hide) {
            s3->__vtbl = 0x10034B24;
            s3->mStringTop = lk_relPtr((u32)gabi::at<PlayerMaterialAnimationView>(mtl)->mpData + 4);
            SafeString_vf14_l(gabi::ea(&ear3MaterialName));
            SafeString_vf14_l(gabi::ea(&ear3MaterialName));
            u32 g = (u32)ear3MaterialName.mStringTop;
            SafeString_vf14_l(gabi::ea(s3.get()));
            if (g == s3->mStringTop || lk_safeStrEq((u32)ear3MaterialName.mStringTop, s3->mStringTop)) {
                if (checkCaughtShapeHide() || (noResetFlg1() & 8) /* daPyFlg1_CASUAL_CLOTHES */) {
                    hide = true;
                }
            }
        }
        gabi::at<PlayerMaterialAnimationView>(mtl)->mShape->mVisible = hide ? 0 : 1; /* getShape()->hide() / show() */
    }
    if (noResetFlg1() & 8) {
        /* mpCLModelData->getJointNodePointer(CL_JNT_CL_BACK_e)->getMesh()->getShape()->hide() */
        u32 node = lk_jointNode(gabi::ea(mpCLModelData.get()), 0x29);
        gabi::at<PlayerJointNodeView>(node)->mMesh->mShape->hide();
    }
}

/* 023DF600 */
void daPy_lk_c::makeItemType() {
#define LK_fastCreate(name, prm)                                                                   \
    Player01_fastCreate((s32)(name), (u32)(prm), &current.pos, -1, 0, 0, -1, 0, 0)
    u16 item = mEquipItem;
    if (item == player01::dItemNo_GRAPPLING_HOOK_e) {
        Player01_setActorKeep(&mActorKeepEquip, LK_fastCreate(player01::fpcNm_HIMO2_e, 0));
    } else if (item == dItemNo_HOOKSHOT_e) {
        Player01_setActorKeep(&mActorKeepEquip, LK_fastCreate(player01::fpcNm_HOOKSHOT_e, 0));
        setHookshotModel();
    } else if (item == player01::dItemNo_BOOMERANG_e) {
        Player01_setActorKeep(&mActorKeepEquip, LK_fastCreate(player01::fpcNm_BOOMERANG_e, 0));
    } else if (item == player01::dItemNo_BOMB_BAG_e) {
        u32 prm = Player01_makeBombParameters(3, 0, 0);
        Player01_setActorKeep(&mActorKeepGrab, LK_fastCreate(player01::fpcNm_BOMB_e, prm));
        u32 grab = gabi::ea(mActorKeepGrab.mActor.get());
        mEquipItem = 0x100;
        if (grab != 0) {
            setActAnimeUpper(player01::dRes_INDEX_LKANM_BCK_GRABWAIT_e, 2 /* UPPER_MOVE2_e */, 0.0f,
                             0.0f, -1, 5.0f);
            mActivePlayerBombs = mActivePlayerBombs + 1;
            Player01_setCarryNow(mActorKeepGrab.mActor.get(), 0);
            u32 play = dComIfGp_ea(); /* dComIfGp_setItemBombNumCount(-1) */
            Player01_gameAt(play)->play.mItemBombNumCount =
                (s16)(Player01_gameAt(play)->play.mItemBombNumCount - 1);
            m35C8 = 17.0f;
        }
        return;
    } else if (checkBowItem(item)) {
        Player01_setBowModel(this);
        return;
    } else if (item == player01::dItemNo_TELESCOPE_e) {
        setScopeModel();
        return;
    } else if (checkPhotoBoxItem(item)) {
        setPhotoBoxModel();
        return;
    } else if (item == player01::dItemNo_TINGLE_TUNER_e) {
        setTinkleCeiverModel();
        return;
    } else if (item == player01::dItemNo_DEKU_LEAF_e) {
        return; /* HD: no small fan model here */
    } else if (item == player01::dItemNo_WIND_WAKER_e) {
        mEquipItem = player01::dItemNo_NONE_e;
        setTactModel();
        return;
    } else if (item == player01::dItemNo_SKULL_HAMMER_e) {
        setHammerModel();
        return;
    } else if (item == dItemNo_EMPTY_BOTTLE_e) {
        Player01_setBottleModel(this, 0x50);
        return;
    } else {
        return;
    }
#undef LK_fastCreate
    if (mActorKeepEquip.mActor == nullptr) {
        u32 f = resetFlg0();
        mEquipItem = 0x100;
        setResetFlg0(f & ~0x80u); /* offResetFlg0(daPyRFlg0_UNK80) */
    }
}

/* 023DBDD0 */
BOOL daPy_lk_c::setGetDemo() {
    s32 staffIdx = dComIfGp_evmng_getMyStaffId("Link" /* "Link" */, this, 0);
    if (Player01_game()->play.mEvtCtrl.mMode != 0 /* dComIfGp_event_runCheck() */ && (m34CE & 4)) {
        dComIfGp_evmng_cutEnd(staffIdx);
        return TRUE;
    }
    /* HD: dEvent_manager_c::startCheckOld */
    if (Player01_startCheckOld(dComIfGp_getPEvtManager(),
                               "DEFAULT_GETITEM" /* "DEFAULT_GETITEM" */)) {
        u8 b = m34CE;
        bool go = (b & 1) != 0;
        if (!go) {
            if ((mNoResetFlg0 & 0xA0000000) /* daPyFlg0_UNK80000000 | daPyFlg0_UNK20000000 */ ||
                !((u32)mAcch.m_flags & 0x20) /* !mAcch.ChkGroundHit() */ ||
                (mModeFlg & 0x10452822) /* checkPlayerFly() */) {
                if (!((u32)Player01_game()->play.mPlayerStatus[0][0] &
                      0x10000) /* !daPyStts0_SHIP_RIDE_e */) {
                    go = true;
                    b = m34CE;
                    if (!(b & 1)) {
                        b = m34CE;
                        mMsgId = 0xFFFFFFFF;
                    }
                }
            }
        }
        if (go) {
            m34CE = b | 1;
            u32 cutName = Player01_getMyNowCutName(dComIfGp_getPEvtManager(), staffIdx);
            if (cutName != 0) {
                s32 cut = gabi::load<u8>(cutName) * 100 + gabi::load<u8>(cutName + 1) * 10 +
                          gabi::load<u8>(cutName + 2) - 0x14D0;
                if (cut == 0xB) {
                    fopAc_ac_c *item = Player01_getItemEventPartner(this);
                    if (item != nullptr && (fpcM_GetName(item) == player01::fpcNm_ITEM_e ||
                                            fpcM_GetName(item) == player01::fpcNm_Demo_Item_e)) {
                        Player01_hideItem(item);
                        u32 itemNo = Player01_getItemNo(item);
                        if (!(m34CE & 2)) {
                            setGetItemSound(itemNo, 0);
                            m34CE = m34CE | 2;
                        }
                        /* HD: the messages go through a message manager (*0x101F4B5C) and the play
                         * message status */
                        u32 mgr = gabi::load<u32>(0x101F4B5C);
                        u32 msgNo;
                        u16 maxLife;
                        if (itemNo == player01::dItemNo_HEART_PIECE_e &&
                            ((maxLife = dComIfGs_getMaxLife()) & 3)) {
                            msgNo = (maxLife & 3) + 0x7B;
                        } else {
                            msgNo = itemNo + 0x65;
                        }
                        if (mMsgId == 0xFFFFFFFF) {
                            if (Player01_messageSet(mgr, msgNo, 0) != -1) {
                                mMsgId = 0; /* HD: stores 0, not the message id */
                            }
                            return TRUE;
                        }
                        /* 025F795C (matcher: fopMsgM_SearchByID) returns the message status (play +
                         * 0x5BB2) */
                        if (Player01_getMessageStatus() == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
                            Player01_setMessageStatus(mgr, 0x10 /* fopMsgStts_MSG_ENDS_e */);
                            return TRUE;
                        }
                        if (Player01_getMessageStatus() == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
                            Player01_setMessageStatus(mgr, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
                            dComIfGp_evmng_cutEnd(staffIdx);
                            Player01_deadItem(item);
                            m34CE = m34CE | 4;
                            return TRUE;
                        }
                        if (Player01_getMessageStatus() == 1 /* fopMsgStts_MSG_PREPARING_e */) {
                            Player01_demoMsgFlagOn();
                        }
                    }
                } else {
                    dComIfGp_evmng_cutEnd(staffIdx);
                }
            }
            return TRUE;
        }
    }
    m34CE = 0;
    return FALSE;
}

/* 023D94E4: drawShadow (unnamed by the matcher; the matcher put the name on draw 023D9820).
 * HD: no shadow id / dComIfGd_setShadow; a shadow model (0x438) is placed on the highest of four ground
 * points around Link and faded with the height above it. */
/* 023D94E4 */
void daPy_lk_c::drawShadow() {
    f64 gnd = mAcch.GetGroundH(); /* mAcch ground height */
    if (gnd == -1000000000.0) {
        return;
    }
    gabi::Local<cXyz> ofs;
    ofs->x = 0.0f;
    ofs->z = 50.0f;
    ofs->y = 0.0f;
    /* dBgS_GndChk (0x54) on the stack */
    gabi::Local<dBgS_GndChk> chk;
    u32 c = gabi::ea(chk.get());
    Player_constructGroundCheck(chk.get());
    chk->cBgS_Chk::__vtbl = 0x10034C34;
    chk->cBgS_PolyInfo::__vtbl = 0x10034C44;
    chk->mbObjThrough = 0;
    chk->mbCamThrough = 0;
    chk->mbLinkThrough = 0;
    chk->dBgS_GrpPassChk::__vtbl = 0x10034C54;
    chk->mbArrowThrough = 0;
    chk->mPolyPassChk = c + 0x40;
    chk->mbBombThrough = 0;
    chk->mbBoomerangThrough = 0;
    chk->mGrp = 1;
    chk->mbRopeThrough = 0;
    chk->dBgS_PolyPassChk::__vtbl = 0x10034C64;
    chk->mGrpPassChk = c + 0x4C;
    gabi::Local<cXyz> pos;
    for (s32 i = 0, angle = 0; i < 4; i++, angle += 0x4000) {
        Player_offsetPosition( pos.get(), &current.pos, (s32)(s16)angle, ofs.get());
        fcpy_l(gabi::ea(&chk->m_pos.x), gabi::ea(pos.get()) + 0);
        fcpy_l(gabi::ea(&chk->m_pos.y), gabi::ea(pos.get()) + 4);
        fcpy_l(gabi::ea(&chk->m_pos.z), gabi::ea(pos.get()) + 8);
        f64 cross = Player_groundCrossNative(dComIfG_Bgsp(), chk.get());
        if (cross > gnd) {
            gnd = cross;
        }
    }
    f32 h = (f32)((f64)current.pos.y - gnd);
    if (h > 10.0f && Player_checkPolySafe(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd))) {
        f32 t = (h - 10.0f) * 0.2f;
        u8 alpha = (u8)gabi::ftoi(t - 36.0f >= 0.0f ? 36.0f : t);
        mDoMtx_stack_c::transS(current.pos.x, (f32)(gnd + 10.0), current.pos.z);
        s32 ang = (s16)(shape_angle.y + 0x8000);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)ang);
        mtx_copy_q(gabi::ea(Player_getShadowModel(this)) + 0xC8, LK_MTX_STACK); /* the shadow model's base matrix */
        u32 mat = gabi::at<J3DModel>(gabi::ea(Player_getShadowModel(this)))->getModelData()->mMaterials;
        gabi::Local<be<u32>> col; /* GXColor {0, 0, 0, alpha} */
        *col = alpha;
        auto* material = gabi::at<PlayerMaterialAnimationView>(mat);
        u32 tev = material->mpTevBlock;
        gabi::at<PlayerTevBlockNative>(tev)->setTevColor(1, col.get());
        gabi::Local<PlayerColorF4> f4;
        daPy_colorToF4(gabi::ea(f4.get()), gabi::ea(col.get()));
        gabi::Local<cXyz> rgb;
        Player_convertColorToRGB(rgb.get(), f4.get(), 1.0f);
        material->mParameterFlags = material->mParameterFlags | 0x100;
        auto* destination = gabi::at<PlayerColorF4>(Player_materialParameter(gabi::ea(&material->mParameterFlags), 8));
        f32 a = (f32)(u8)gabi::at<GXColor>(gabi::ea(col.get()))->a / 255.0f;
        destination->g = rgb->y;
        destination->b = rgb->z;
        destination->r = rgb->x;
        destination->a = a;
        mDoExt_modelUpdateDL(gabi::at<J3DModel>(gabi::ea(Player_getShadowModel(this))), 0);
    }
    /* ~dBgS_GndChk */
    chk->cBgS_PolyInfo::__vtbl = 0x10034C44;
    chk->dBgS_PolyPassChk::__vtbl = 0x10034C64;
    chk->dBgS_GrpPassChk::__vtbl = 0x10034C24;
    Player_destroyGroundCheck(chk.get(), 0);
}

/* 023DEA24 */
void daPy_lk_c::setBowModel() {
    u32 bck = getItemAnimeResource(player01::dRes_INDEX_LKANM_BCK_ARROWRELORDA_e);
    u32 oldHeap = gabi::ea(setItemHeap());
    u32 tmp_modelData =
        initModel(mpEquipItemModel_ea, player01::dRes_INDEX_LINK_BDL_BOW_e, 0x37221222);
    BOOL ret =
        Player_initBckAnimation(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mSwordAnim),
                                tmp_modelData, bck, 0, 2, 1.0f, 0, -1, 0);
    if (!ret) {
        JUT_ASSERT_fail("d_a_player_bow.inc", 0x18B, "0");
    }
    mDoExt_setCurrentHeap(oldHeap);
    m35EC = 0.0f;
    /* HD: the bow string materials are found by name among the materials of joint 0: "lineballMat"
     * and "lineMat" shown, "lineDamMAT" hidden */
    u32 data =
        gabi::ea(reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemModel->mModelData.get());
    u32 mtl = gabi::ea(
        gabi::at<PlayerJointNodeView>(gabi::at<J3DModelData>(data)->mJointNodes)->mMesh.get());
    gabi::Local<SafeString> k1, k2, k3, n1, n2, n3;
    const char *keys[3] = {"lineballMat", "lineMat", "lineDamMAT"};
    SafeString *ks[3] = {k1.get(), k2.get(), k3.get()};
    SafeString *ns[3] = {n1.get(), n2.get(), n3.get()};
    while (mtl != 0) {
        for (int i = 0; i < 3; i++) {
            SafeString *k = ks[i];
            SafeString *n = ns[i];
            if (i == 0) {
                k->mStringTop = gabi::guest_str(keys[i]);
                k->__vtbl = Player01SafeStringVtable;
            } else {
                k->__vtbl = Player01SafeStringVtable;
                k->mStringTop = gabi::guest_str(keys[i]);
            }
            u32 name = lk_relPtr((u32)gabi::at<PlayerMaterialAnimationView>(mtl)->mpData + 4);
            n->__vtbl = Player01SafeStringVtable;
            n->mStringTop = name;
            Player01_assureMaterialName(gabi::ea(k));
            Player01_assureTermination(k);
            u32 a = (u32)k->mStringTop;
            Player01_assureTermination(n);
            if (a == (u32)n->mStringTop || lk_safeStrEq((u32)k->mStringTop, (u32)n->mStringTop)) {
                gabi::at<PlayerMaterialAnimationView>(mtl)->mShape->mVisible =
                    i < 2 ? 1 : 0; /* show() / hide() */
                break;
            }
        }
        mtl = gabi::ea(gabi::at<PlayerMaterialAnimationView>(mtl)->mNext.get()); /* getNext() */
    }
    gabi::at<J3DJoint>(lk_jointNode(tmp_modelData, 6))->setCallBack(daPy_bowJointCB);
    gabi::at<J3DJoint>(lk_jointNode(tmp_modelData, 0xB /* LINK_BOW_JNT_LINEBB_JNT_e */))
        ->setCallBack(daPy_bowJointCB);
    reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemModel->mUserArea =
        gabi::ea(this); /* setUserArea(this) */
}

/* jointAfterCB (unnamed by the matcher) */
/* 023D7564 */
BOOL daPy_lk_c::jointAfterCB(int jnt_no, u32 param_2 /* J3DTransformInfo* */, u32 param_3 /* Quaternion* */) {
    auto* transform = gabi::at<J3DTransformInfo>(param_2);
    auto* feet = reinterpret_cast<daPy_footData_c*>(mFootData);
    // HD: restore the saved state for this joint before the foot/ship callbacks.
    auto* flags = reinterpret_cast<be<u8>*>(m68E2);
    auto* state = reinterpret_cast<PlayerJointState*>(m3658);
    u8 f = flags[jnt_no];
    if (f != 0) {
        if (f & 1) {
            lk_copy16(param_3, gabi::ea(&state->mSavedQuaternion[jnt_no]));
            f = flags[jnt_no];
        }
        if (f & 2) {
            Player_copyTransform(gabi::at<J3DTransformInfo>(param_2), &state->mSavedTransform[jnt_no]);
        }
        flags[jnt_no] = 0;
    }
    u32 cl = gabi::ea(mpCLModel.get());
    if (jnt_no == CL_JNT_LFOOT_JNT_e) {
        PSMTXCopy_l(lk_getAnmMtx(cl, 0x22), gabi::ea(&feet[1].field_0x088[2])); /* mFootData[1].field_0x088[2] */
    } else if (jnt_no == CL_JNT_RFOOT_JNT_e) {
        PSMTXCopy_l(lk_getAnmMtx(cl, 0x27), gabi::ea(&feet[0].field_0x088[2])); /* mFootData[0].field_0x088[2] */
    } else if (jnt_no == CL_JNT_LLEGA_JNT_e) {
        PSMTXCopy_l(lk_getAnmMtx(cl, 0x20), gabi::ea(&feet[1].field_0x088[0]));
    } else if (jnt_no == CL_JNT_LLEGB_JNT_e) {
        PSMTXCopy_l(lk_getAnmMtx(cl, 0x21), gabi::ea(&feet[1].field_0x088[1]));
    } else if (jnt_no == CL_JNT_RLEGA_JNT_e) {
        PSMTXCopy_l(lk_getAnmMtx(cl, 0x25), gabi::ea(&feet[0].field_0x088[0]));
    } else if (jnt_no == CL_JNT_RLEGB_JNT_e) {
        PSMTXCopy_l(lk_getAnmMtx(cl, 0x26), gabi::ea(&feet[0].field_0x088[1]));
    } else if (jnt_no == 0xA || jnt_no == 0xB) {
        /* HD: the arm angles while riding the ship are applied here (and blended with the old frame) */
        if (dComIfGp_checkPlayerStatus0(0, 0x10000) /* daPyStts0_SHIP_RIDE_e */ &&
            (u8)gabi::at<mDoExtAnimation::mDoExt_MtxCalcOldFrame>(m_old_fdata)->mOldFrameFlg != 0 && gabi::ea(dComIfGp_getShipActor()) != 0) {
            u32 ship = gabi::ea(dComIfGp_getShipActor());
            if (!Player_isCreating(ship != 0 ? (u32)gabi::at<base_process_class>(ship)->mBsPcId : 0xFFFFFFFF) &&
                !checkShipNotNormalMode()) {
                setShipRideArmAngle(jnt_no, param_2);
                mtx_copy_q(lk_getAnmMtx(gabi::ea(mpCLModel.get()), jnt_no), LK_J3DSYS_CURMTX);
                transform->mTranslate.z = 0.0f;
                transform->mTranslate.y = 0.0f;
                transform->mTranslate.x = 0.0f;
                auto* oldFrame = gabi::at<mDoExtAnimation::mDoExt_MtxCalcOldFrame>(m_old_fdata);
                Player_blendQuaternion((u32)oldFrame->mOldFrameQuaternion + jnt_no * 0x10, 0x101CEE50, param_3,
                           gabi::fsubs_ppc(1.0f, (f32)oldFrame->mOldFrameRate));
            }
        }
    }
    return TRUE;
}

/* 023D7198 */
void daPy_lk_c::setShipRideArmAngle(int jnt_no, u32 param_2 /* J3DTransformInfo* */) {
    auto* transform = gabi::at<J3DTransformInfo>(param_2);
    // HD: retain explicit initialization guards and their original store order.
    if (arm_pos_initialized == 0) {
        arm_pos.x = 0.0f;
        arm_pos.z = 0.0f;
        arm_pos_initialized = 1;
        arm_pos.y = 0.0f;
    }
    if (armA_offset_initialized == 0) {
        armA_offset.x = 17.0f;
        armA_offset.z = 0.0f;
        armA_offset_initialized = 1;
        armA_offset.y = 0.0f;
    }
    BOOL uVar3 = jnt_no == 0xA;
    if (uVar3) {
        auto* oldFrame = gabi::at<mDoExtAnimation::mDoExt_MtxCalcOldFrame>(m_old_fdata);
        f32 rate = (f32)oldFrame->mOldFrameRate; /* getOldFrameRate() */
        f32 fVar2 = 1.0f / (1.0f - rate);
        auto* oldTransform = gabi::at<J3DTransformInfo>((u32)oldFrame->mOldFrameTransInfo + jnt_no * sizeof(J3DTransformInfo)); /* getOldFrameTransInfo(jnt_no) */
        gabi::Local<cXyz> local_74;
        f32 x = gabi::fnmsubs((f32)oldTransform->mTranslate.x, rate, (f32)transform->mTranslate.x) * fVar2;
        transform->mTranslate.x = x;
        f32 y = gabi::fnmsubs((f32)oldTransform->mTranslate.y, rate, (f32)transform->mTranslate.y) * fVar2;
        transform->mTranslate.y = y;
        f32 z = gabi::fnmsubs((f32)oldTransform->mTranslate.z, rate, (f32)transform->mTranslate.z) * fVar2;
        local_74->x = x;
        local_74->y = y;
        transform->mTranslate.z = z;
        local_74->z = z;
        /* HD: the arm joint's matrix (mpCLModel->getAnmMtx(9)) instead of J3DSys::mCurrentMtx */
        PSMTXMultVec(gabi::at<Mtx34>(lk_getAnmMtx(gabi::ea(mpCLModel.get()), 9)), local_74.get(), &arm_pos);
    }
    gabi::Local<cXyz> local_68;
    Player_subtractPosition(gabi::ea(dComIfGp_getShipActor()) + 0x720 /* ship->getTillerTopPosP() */, local_68.get(),
               gabi::ea(&arm_pos));
    f32 lx = local_68->x, ly = local_68->y, lz = local_68->z;
    gabi::Local<cXyz> xz1;
    xz1->x = lx;
    xz1->y = 0.0f;
    xz1->z = lz;
    f64 m1 = Player_squareMagnitudeNative(xz1.get());
    f64 absXZ = Player_sqrtNative(m1);
    u32 uVar5 = Player_atan2Native((f64)ly, absXZ);
    mDoMtx_stack_c::transS((f32)arm_pos.x, (f32)arm_pos.y, (f32)arm_pos.z);
    gabi::Local<cXyz> xz2;
    xz2->x = lx;
    f32 f30 = 26.3f * cM_scos((s32)uVar5); /* 26.3f * cM_scos(uVar5) */
    xz2->y = 0.0f;
    xz2->z = lz;
    f64 f1 = Player_squareMagnitudeNative(xz2.get()); /* abs2XZ */
    f64 f31 = Player_sqrtNative(f1);
    if (!uVar3) {
        u32 a = Player_atan2Native((f64)lx, (f64)lz);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)(a - 0x4000));
        if (!(f31 < f30)) {
            mDoMtx_stack_c::scaleM((f32)(f31 / f30), 1.0f, 1.0f);
        }
    } else {
        f32 f0 = 17.0f * cM_scos((s32)uVar5);
        f32 f29 = f30 + f0;
        f32 f2 = f30 * f30;
        f32 f3 = f0 * f0;
        if (!(f31 < f29)) {
            u32 a = Player_atan2Native((f64)lx, (f64)lz);
            mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)(a - 0x4000));
            mDoMtx_stack_c::scaleM((f32)(f31 / f29), 1.0f, 1.0f);
        } else {
            f32 t = (f32)((f64)gabi::fsubs_ppc(f3, f2) + f1);
            f32 r = (f32)(t / (f64)(f32)(f1 + f1));
            u32 a = Player_atan2Native((f64)lx, (f64)lz);
            f32 q = (f32)(f1 * r);
            f64 s = Player_sqrtNative((f64)gabi::fnmsubs(q, r, f3));
            u32 b = Player_atan2Native(s, (f64)(f32)(f31 * r));
            mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)((a - 0x4000) - b));
        }
    }
    Player_ZrotNative(LK_MTX_STACK, uVar5);
    Player_XrotNative(LK_MTX_STACK, 0x4000);
    PSMTXCopy_l(LK_MTX_STACK, LK_J3DSYS_CURMTX);
    if (uVar3) {
        PSMTXMultVec(gabi::at<Mtx34>(LK_MTX_STACK), &armA_offset, &arm_pos);
    }
}

/* initTextureAnime (unnamed by the matcher) */
/* 023D5164 */
void daPy_lk_c::initTextureAnime() {
    u32 buf = operator_new_array_align(0x1000, 0x20);
    reinterpret_cast<daPy_anmHeap_c*>(m_tex_anm_heap)->m_buffer = gabi::at<void>(buf);
    if (buf == 0) {
        JUT_ASSERT_fail("d_a_player_main.cpp", 0x5C2C, "m_tex_anm_heap.m_buffer != (0)");
    }
    /* HD: the eye/mouth pattern animation comes from the resident "LkAnm" archive and drives the two HD
     * animation objects at 0x644 and 0x69C (instead of J3DTexNoAnm / J3DMaterialAnm objects) */
    gabi::Local<SafeString> key;
    key->__vtbl = 0x10034B24;
    key->mStringTop = 0x100351B4; /* "LkAnm" */
    u32 btp = Player_getArchiveResource( dComIfG_resControl(), key.get(), dRes_INDEX_LKANM_BTP_TMABAA_e);
    gabi::Local<PlayerTextureAnimationSetup> info; /* the animation object's setup (sp+0x24..0x64; HD: 0x40 bytes, field +0x3C) */
    info->mParameters[3] = -1;
    info->mParameters[2] = -1;
    info->mParameters[1] = -1;
    info->mParameters[0] = -1;
    info->field_0x14 = 0;
    info->mEnabled = 1;
    info->field_0x18 = 0;
    info->field_0x11 = 0;
    auto* animationData = gabi::at<PlayerTexPatternData>(gabi::at<PlayerTextureResource>(btp)->mpData);
    info->mParameters[1] = animationData->field_0x16;
    s32 v = animationData->field_0x18;
    if (v >= 0) {
        info->mParameters[2] = v;
    }
    v = animationData->field_0x1C;
    if (v >= 0) {
        info->mParameters[3] = v;
    }
    u16 flags = animationData->mFlags;
    info->field_0x14 = 0;
    info->field_0x18 = 0;
    info->field_0x11 = (flags & 1) ^ 1;
    /* HD: +0x3C = the model data's u16 at +0x24 (game test: the field was never set, and the 0x1C-byte Local was
     * smaller than the object the HD animation-object init reads and writes) */
    info->mModelResourceValue = (u16)gabi::at<PlayerModelResource>(PlayerModelData_getResource(gabi::ea(mpCLModelData.get())))->mMaterialNum;
    info->mParameters[2] = 5;
    info->field_0x18 = 0;
    info->mParameters[3] = 0x17C;
    info->mParameters[1] = 5;
    info->field_0x14 = 0;
    info->field_0x11 = 1;
    info->mParameters[0] = 0x18;
    for (u32 obj = gabi::ea(this) + 0x644; obj <= gabi::ea(this) + 0x69C; obj += 0x58) {
        u32 b = daPy_newAlign(0x2000, 4);
        PlayerTexPattern_init(obj, info.get(), b, 0x2000);
        PlayerTexPattern_setAnm(obj, (u32)gabi::at<PlayerTextureResource>(btp)->mpData);
        PlayerTexPattern_setModelData(obj, PlayerModelData_getResource(gabi::ea(mpCLModelData.get())));
    }
    lk_linkTexMaterials(this, 0x100351BC /* "mouth" */);
}

/* 023DD36C */
u16 daPy_lk_c::checkNormalFace() {
    if (mCurProc == player01::daPyProc_SHIP_CRANE_e) {
        return player01::daPyFace_TMABAA;
    }
    s32 face = mFace;
    if (face != 0xAD /* !checkFaceTypeNot() */) {
        return (u16)face;
    }
    if (resetFlg0() & daPyRFlg0_UNK40000) {
        return player01::daPyFace_TMABAG;
    }
    u16 upper = LK_upperAnmIdx();
    if (upper == 0x96 /* GRABWAITB */) {
        return player01::daPyFace_TMABAJ_TEYORIME;
    }
    if (upper == 0x95 /* GRABWAIT */) {
        return player01::daPyFace_TMABAE;
    }
    if (damageEffectType == 1 /* daPy_dmEcallBack_c::checkCurse() */ || checkRestHPAnime()) {
        return player01::daPyFace_TMABAF;
    }
    u32 f1 = noResetFlg1();
    if ((f1 & 1) /* checkNoDamageMode(): EQUIP_DRAGON_SHIELD */ || mTinkleShieldTimer != 0) {
        return player01::daPyFace_TMABAC;
    }
    if (damageEffectType == 0 /* daPy_dmEcallBack_c::checkFlame() */) {
        return player01::daPyFace_TDAMDASH;
    }
    if (f1 & daPyFlg1_CONFUSE) {
        return player01::daPyFace_TMABAH_TABEKOBE;
    }
    if (mNoResetFlg0 & 0x40000000 /* getHeavyState() */) {
        return player01::daPyFace_TMABAH;
    }
    if ((f1 & 0x1000000) /* daPyFlg1_UNK1000000 */ && !(mModeFlg & 1)) {
        return player01::daPyFace_TDASHKAZE;
    }
    if (checkUpperReadyAnime() || (noResetFlg1() & 0x400) ||
        (mpAttnActorLockOn != nullptr && (u8)mpAttnActorLockOn->group == 2 /* fopAc_ENEMY_e */) ||
        (mNoResetFlg0 & 0x2000000) /* checkEquipHeavyBoots() */ ||
        mEquipItem == player01::dItemNo_SKULL_HAMMER_e) {
        return 3;
    }
    if (checkBossBgm()) {
        return 3;
    }
    u32 att = mpAttention;
    if (Player01_lockonTruth(att) ||
        ((u32) reinterpret_cast<Player01Attention *>(gabi::at<void>(att))->mLockFlags &
         0x20000000)) {
        return player01::daPyFace_TMABACB;
    }
    if (mEquipItem != player01::daPyItem_NONE_e) {
        return player01::daPyFace_TMABAB;
    }
    if ((u16) reinterpret_cast<daPy_anmHeap_c *>(m_anm_heap_under)[1].mIdx ==
        0x136 /* m_anm_heap_under[UNDER_MOVE1_e].mIdx == WALKSLOPE */) {
        return 3;
    }
    s32 proc = mCurProc;
    if (proc == player01::daPyProc_MOVE_e) {
        f32 spd = mNormalSpeed;
        f32 x;
        if (m3580 == 8) {
            x = (1.0f * spd) / (f32)mMaxNormalSpeed /* mMaxNormalSpeed */;
        } else {
            f32 c = cM_scos(m34E2); /* cM_scos(m34E2) */
            x = (c * spd) / (f32)mMaxNormalSpeed;
        }
        if (!(x < 0.9f)) {
            return player01::daPyFace_TMABACC;
        }
    }
    if (proc == player01::daPyProc_WAIT_e &&
        Player01_game()->play.mEvtCtrl.mMode == 0 /* !dComIfGp_event_runCheck() */) {
        f64 rnd = Player01_random();
        u16 idx = (u16) reinterpret_cast<daPy_anmHeap_c *>(m_tex_anm_heap)
                      ->mIdx; /* m_tex_anm_heap.mIdx */
        if (idx == 0x22D /* TMABAA */ || idx == 0x22E /* TMABAB */) {
            if (rnd < 0.01f) {
                return 6;
            }
        } else if (idx == 0x231 /* TMABACC */) {
            if (!(rnd < 0.01f)) {
                return 6;
            }
        }
        /* dStage_stagInfo_GetSTType(dComIfGp_getStageStagInfo()): the stage data's virtual (slot
         * 0x15C) */
        auto *info = static_cast<PlayerStageInfo *>(dComIfGp_getStageStagInfo());
        if ((((u32)info->mStageFlags >> 16) & 7) == 1 /* dStageType_DUNGEON_e */) {
            return 2;
        }
        info = static_cast<PlayerStageInfo *>(dComIfGp_getStageStagInfo());
        if ((((u32)info->mStageFlags >> 16) & 7) == 4 /* dStageType_SUBDUNGEON_e */) {
            return 2;
        }
    }
    return 0;
}

/* mDoExt_AnmRatioPack::setRatio (HD: one ratio per joint: count at +8, array at +0xC) */
static inline void lk_setRatio(u32 pack, f32 r) {
    for (s32 j = 0; j < gabi::load<s32>(pack + 8); j++) {
        gabi::store<f32>(gabi::load<u32>(pack + 0xC) + j * 4, r);
    }
}

/* 023E0A04 */
BOOL daPy_lk_c::setSingleMoveAnime(int anm, f32 rate, f32 start, int end, f32 i_morf) {
    u32 anmData = getAnmData(anm);
    gabi::Local<be<u32>> under_bck;
    gabi::Local<be<u32>> upper_bck;
    getUnderUpperAnime(anmData, gabi::ea(under_bck.get()), gabi::ea(upper_bck.get()), 0, 0xB400);
    u32 self = gabi::ea(this);
    reinterpret_cast<daPy_anmHeap_c *>(m_anm_heap_under)[1].mIdx =
        0xFFFF; /* m_anm_heap_under[UNDER_MOVE1_e].mIdx */
    reinterpret_cast<daPy_anmHeap_c *>(m_anm_heap_upper)[1].mIdx =
        0xFFFF; /* m_anm_heap_upper[UPPER_MOVE1_e].mIdx */
    reinterpret_cast<Player01AnmRatioPack *>(mAnmRatioUpper)[0].setRatio(
        1.0f); /* mAnmRatioUpper[UPPER_MOVE0_e] */
    reinterpret_cast<Player01AnmRatioPack *>(mAnmRatioUpper)[1].setRatio(0.0f);
    reinterpret_cast<Player01AnmRatioPack *>(mAnmRatioUnder)[0].setRatio(
        1.0f); /* mAnmRatioUnder[UNDER_MOVE0_e] */
    reinterpret_cast<Player01AnmRatioPack *>(mAnmRatioUnder)[1].setRatio(0.0f);
    u32 under = *under_bck;
    reinterpret_cast<Player01AnmRatioPack *>(mAnmRatioUnder)[0].mpAnmTransform.v =
        under; /* mAnmRatioUnder[UNDER_MOVE0_e].setAnmTransform(under_bck) */
    reinterpret_cast<Player01AnmRatioPack *>(mAnmRatioUnder)[1].mpAnmTransform.v = 0;
    s32 endUnder = end < 0 ? Player01_animation(under)->getFrameMax() : end;
    s32 start16;
    if (rate < 0.0f) {
        f32 frame = (f32)endUnder - 0.001f;
        u32 attr = Player01_animation((u32)*under_bck)->getAttribute();
        start16 = (s16)gabi::ftoi(start);
        Player01_setFrameCtrl(this, gabi::ea(&mFrameCtrlUnder[0]), attr, start16, endUnder, rate,
                              frame);
        Player01_animation(*under_bck)->mFrame = frame;
    } else {
        u32 attr = Player01_animation((u32)*under_bck)->getAttribute();
        start16 = (s16)gabi::ftoi(start);
        Player01_setFrameCtrl(this, gabi::ea(&mFrameCtrlUnder[0]), attr, start16, endUnder, rate,
                              start);
        Player01_animation(*under_bck)->mFrame = start;
    }
    u32 upper = *upper_bck;
    if (upper != 0) {
        reinterpret_cast<Player01AnmRatioPack *>(mAnmRatioUpper)[0].mpAnmTransform.v = upper;
        s32 endUpper = end < 0 ? Player01_animation(upper)->getFrameMax() : end;
        if (rate < 0.0f) {
            f32 frame = (f32)endUpper - 0.001f;
            u32 attr = Player01_animation((u32)*under_bck)
                           ->getAttribute(); /* under_bck's attribute, as in the GameCube code */
            Player01_setFrameCtrl(this, gabi::ea(&mFrameCtrlUpper[0]), attr, start16, endUpper,
                                  rate, frame);
            Player01_animation(*upper_bck)->mFrame = frame;
        } else {
            u32 attr = Player01_animation((u32)*under_bck)->getAttribute();
            Player01_setFrameCtrl(this, gabi::ea(&mFrameCtrlUpper[0]), attr, start16, endUpper,
                                  rate, start);
            Player01_animation(*upper_bck)->mFrame = start;
        }
    } else {
        reinterpret_cast<Player01AnmRatioPack *>(mAnmRatioUpper)[0].mpAnmTransform.v = *under_bck;
    }
    reinterpret_cast<Player01AnmRatioPack *>(mAnmRatioUpper)[1].mpAnmTransform.v = 0;
    if (!(i_morf < 0.0f)) {
        Player01_initOldFrameMorf((u32)m_old_fdata, i_morf, 0, 0x2A);
    }
    Player01_setTextureAnime(
        this, (u32)(u16)mAnmDataTable[anm].mTexAnmIdx /* mAnmDataTable[anm].mTexAnmIdx */, 0);
    setHandModel(anm);
    setSeAnime((u32) reinterpret_cast<Player01AnmRatioPack *>(mAnmRatioUnder)[0].mpAnmTransform.v,
               gabi::ea(&m_anm_heap_under[0]), gabi::ea(&mFrameCtrlUnder[0]));
    m34C3 = 0;
    return TRUE;
}

/* HD mDoExt_btkAnm / brkAnm setFrame: frame at +4, the animation's frame (*(+0x80 / +0x10)), and the frame
 * control object at +0x10 (+0x28) whose callback (+0x10, argument +0x14) maps the frame */
static inline void lk_anmObjSetFrame(u32 anm, u32 anmPtrOff, u32 ctrlOff, f32 v) {
    u32 a = gabi::load<u32>(anm + anmPtrOff);
    gabi::store<f32>(anm + 4, v);
    gabi::store<f32>(a, v);
    u32 c = gabi::load<u32>(anm + ctrlOff);
    f64 r = gabi::call_ptr<f64>(gabi::load<u32>(c + 0x10), gabi::load<u32>(c + 0x14), v, gabi::load<f32>(c + 4), gabi::load<f32>(c + 8));
    gabi::store<f32>(c, (f32)r);
    gabi::call(0x027DF40C, anm + ctrlOff);
}

/* 023DF0FC */
void daPy_lk_c::setBottleModel(u32 param_0 /* u16 */) {
    mEquipItem = (u16)param_0;
    u32 oldHeap = gabi::ea(setItemHeap());
    u32 data =
        initModel(mpEquipItemModel_ea, player01::dRes_INDEX_LINK_BDL_BOTTLEEMP_e, 0x37221222);
    /* HD: the animations come from the resident "LkAnm" archive into the btk/brk objects at 0x4810
     * / 0x48F8 */
    gabi::Local<SafeString> k1;
    k1->__vtbl = Player01SafeStringVtable;
    k1->mStringTop = gabi::guest_str("LkAnm");
    u32 btk =
        Player_getArchiveResource(dComIfG_resControl(), k1.get(), dRes_INDEX_LKANM_BTK_TBOTTLE_e);
    s32 mode = anm_vf0C_l(btk);
    Player_initBtkAnimation(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBtk),
                            data, btk, 1, mode, 1.0f, 0, -1,
                            reinterpret_cast<PlayerBtkRuntime *>(
                                &reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBtk)
                                    ->mAnimationResource != 0,
                            0);
    Player01_entryBtk(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBtk), data,
                      0.0f);
    lk_anmObjSetFrame(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBtk), 0x68,
                      0x10, 0.0f);
    gabi::Local<SafeString> k2;
    bool liquid = false;
    bool checkCap = true;
    bool makeCap = false;
    u32 bdl = player01::dRes_INDEX_LINK_BDL_BINLIQUID_e;
    if (param_0 >= 0x51 && (param_0 <= 0x56 || param_0 == 0x59)) {
        liquid = true; /* RED/GREEN/BLUE_POTION, HALF_SOUP, SOUP, WATER, FOREST_WATER */
        if (param_0 == player01::dItemNo_HALF_SOUP_BOTTLE_e) {
            bdl = player01::dRes_INDEX_LINK_BDL_BINLIQUIDH_e;
        }
    }
    if (liquid) {
        data = initModel(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)
                                       ->mpBottleContentsModel) /* mpBottleContentsModel */,
                         bdl, 0x13000022);
        k2->__vtbl = Player01SafeStringVtable;
        k2->mStringTop = gabi::guest_str("LkAnm");
        u32 brk = Player_getArchiveResource(dComIfG_resControl(), k2.get(),
                                            dRes_INDEX_LKANM_BRK_TBINLIQUID_e);
        mode = anm_vf0C_l(brk);
        Player_initBrkAnimation(
            gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBrk), data, brk, 1,
            mode, 1.0f, 0, -1,
            reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBrk.mpAnm.v != 0, 0);
        Player01_entryBrk(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBrk),
                          data, 0.0f);
        f32 frame;
        if (param_0 == 0x51 /* RED_POTION */) {
            frame = 0.0f;
        } else if (param_0 == 0x52 /* GREEN_POTION */) {
            frame = 1.0f;
        } else if (param_0 == 0x53 /* BLUE_POTION */) {
            frame = 2.0f;
        } else if (param_0 == 0x55 /* SOUP */ || param_0 == 0x54 /* HALF_SOUP */) {
            frame = 4.0f;
        } else {
            frame = 3.0f;
        }
        lk_anmObjSetFrame(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBrk),
                          0x10, 0x20, frame);
    } else if ((s32)param_0 == player01::dItemNo_FAIRY_BOTTLE_e) {
        initModel(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpBottleContentsModel),
                  player01::dRes_INDEX_LINK_BDL_BINFAIRY_e, 0x13000022);
    } else if ((s32)param_0 == player01::dItemNo_FIREFLY_BOTTLE_e) {
        data =
            initModel(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpBottleContentsModel),
                      player01::dRes_INDEX_LINK_BDL_BINHO_e, 0x13000022);
        Player01_setBottleModelFlag(
            (u32) reinterpret_cast<PlayerModelState *>(mD90)->mpBottleContentsModel.v, 1); /* HD */
        k2->__vtbl = Player01SafeStringVtable;
        k2->mStringTop = gabi::guest_str("LkAnm");
        u32 brk = Player_getArchiveResource(dComIfG_resControl(), k2.get(),
                                            dRes_INDEX_LKANM_BRK_TBINHO_e);
        mode = anm_vf0C_l(brk);
        Player_initBrkAnimation(
            gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBrk), data, brk, 1,
            mode, 1.0f, 0, -1,
            reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBrk.mpAnm.v != 0, 0);
        Player01_entryBrk(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBrk),
                          data, 0.0f);
        s32 proc = mCurProc;
        m3600 = 0.0f;
        makeCap = proc != 0xA3;
        checkCap = false;
    }
    if (checkCap) {
        makeCap = (s32)param_0 != dItemNo_EMPTY_BOTTLE_e && mCurProc != 0xA3;
    }
    if (makeCap) {
        initModel(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)
                                ->mpBottleCapModel) /* mpBottleCapModel */,
                  player01::dRes_INDEX_LINK_BDL_BOTTLECAP_e, 0x13000022);
        m355E = 1;
    }
    mDoExt_setCurrentHeap(oldHeap);
    if ((s32)param_0 == player01::dItemNo_FOREST_WATER_e) {
        u32 m = (u32) reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemModel.v;
        Player01_makeMtxEmitter(gabi::ea(&m32F0), 0x20D /* ID_AK_JN_FORESTWATER00 */,
                                m ? m + 0xC8 : 0, &current.pos, 0);
    }
}

/* checkNormalSwordEquip(): HD: the save's sword (+0x2E) is the normal sword (0x38), or play + 0x5CEA == 2 */
static inline bool lk_checkNormalSwordEquip() {
    if (gabi::load<u8>(dComIfGs_base_l() + 0x2E) == 0x38) {
        return true;
    }
    return gabi::load<u8>(dComIfGp_ea() + 0x5CEA) == 2;
}
/* a "LkAnm" resource (sead::SafeString key with the string at 0x10035474) */
static inline u32 Player01_getLkAnm(u32 idx) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::guest_str("LkAnm");
    key->__vtbl = Player01SafeStringVtable;
    return Player_getArchiveResource(dComIfG_resControl(), key.get(), idx);
}

/* 023DE0B4 */
void daPy_lk_c::setSwordModel(BOOL r28) {
    u32 bck =
        getItemAnimeResource(lk_checkNormalSwordEquip() ? player01::dRes_INDEX_LKANM_BCK_CUTAA_e
                                                        : player01::dRes_INDEX_LKANM_BCK_CUTAMS_e);
    mEquipItem = player01::daPyItem_SWORD_e;
    u32 oldHeap = gabi::ea(setItemHeap());
    /* sword_model_tbl[2] (.rodata 0x1003547C / 0x1003548C): blade, glow, glow btk, glow brk, tip
     * stab, its bpk, btk, brk */
    Player01SwordModelResources *resources =
        &sword_model_tbl.mEntries[lk_checkNormalSwordEquip() ? 0 : 1];
    u32 data = initModel(mpEquipItemModel_ea, resources->mBladeModel, 0x37221222);
    BOOL ret =
        Player_initBckAnimation(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mSwordAnim),
                                data, bck, 0, 2, 1.0f, 0, -1, 0);
    if (!ret) {
        JUT_ASSERT_fail("d_a_player_sword.inc", 0x52, "0");
    }
    if (!lk_checkNormalSwordEquip()) {
        u32 btk = Player01_getLkAnm(player01::dRes_INDEX_LKANM_BTK_TSWMS_e);
        s32 mode = anm_vf0C_l(btk);
        Player_initBtkAnimation(
            gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBtk), data, btk, 1,
            mode, 1.0f, 0, -1,
            reinterpret_cast<PlayerBtkRuntime *>(
                &reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBtk)
                    ->mAnimationResource != 0,
            0); /* mpEquipItemBtk */
        Player01_entryBtk(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBtk),
                          data, 0.0f);
    }
    u32 blur = mpSwBlur;
    reinterpret_cast<PlayerSwordBlurView *>(gabi::at<void>(blur))->mpPosBuffer =
        operator_new_array_align(0x4800,
                                 0x20); /* mSwBlur.mpPosBuffer = new (0x20) Vec[2 * 0x300] */
    data = initModel(
        gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpSwordModel1) /* mpSwordModel1 */,
        resources->mGlowModel, 0x13000222);
    u32 r = Player01_getLkAnm(resources->mGlowBtk);
    s32 mode = anm_vf0C_l(r);
    Player_initBtkAnimation(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpSwordBtk), data,
                            r, 1, mode, 1.0f, 0, -1,
                            reinterpret_cast<PlayerBtkRuntime *>(
                                &reinterpret_cast<PlayerModelState *>(mD90)->mpSwordBtk)
                                    ->mAnimationResource != 0,
                            0); /* mpSwordBtk */
    Player01_entryBtk(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpSwordBtk), data,
                      0.0f);
    r = Player01_getLkAnm(resources->mGlowBrk);
    mode = anm_vf0C_l(r);
    Player_initBrkAnimation(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBrk),
                            data, r, 1, mode, 1.0f, 0, -1,
                            reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBrk.mpAnm.v != 0,
                            0); /* mpEquipItemBrk */
    Player01_entryBrk(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemBrk), data,
                      0.0f);
    data = initModel(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)
                                   ->mpSwordTipStabModel) /* mpSwordTipStabModel */,
                     resources->mTipStabModel, 0x13000223);
    PlayerModelData_getResource(data);
    r = Player01_getLkAnm(resources->mTipStabBtk);
    mode = anm_vf0C_l(r);
    Player_initBtkAnimation(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpCutfBtk), data,
                            r, 1, mode, 1.0f, 0, -1, 0, 0); /* mpCutfBtk */
    Player01_entryBtk(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpCutfBtk), data, 0.0f);
    r = Player01_getLkAnm(resources->mTipStabBrk);
    mode = anm_vf0C_l(r);
    Player_initBrkAnimation(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpCutfBrk), data,
                            r, 1, mode, 1.0f, 0, -1, 0, 0); /* mpCutfBrk */
    Player01_entryBrk(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpCutfBrk), data, 0.0f);
    r = Player01_getLkAnm(resources->mTipStabBpk);
    mode = anm_vf0C_l(r);
    Player01_initBpk(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpCutfBpk), data, r, 1,
                     mode, 1.0f, 0, -1, 0, 0); /* mpCutfBpk */
    Player01_entryBpk(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpCutfBpk), data, 0.0f);
    mDoExt_setCurrentHeap(oldHeap);
    Player01_startSwordBgm(1); /* r4 (not read: the GameCube signature is a member) is left as the
                                  previous call left it */
    m355C = 0;
    if (r28) {
        f32 frame =
            (f32)(s16) reinterpret_cast<PlayerModelState *>(mD90)->mpSwordBtk.mFrameCtrl.mEnd -
            0.001f;                               /* mpSwordBtk->getFrameMax() - 0.001f */
        setNoResetFlg1(noResetFlg1() | 0x200000); /* onNoResetFlg1(daPyFlg1_UNK200000) */
        lk_anmObjSetFrame(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mpSwordBtk), 0x68,
                          0x10, frame);
    }
}

/* 023DD768 */
void daPy_lk_c::setTextureAnime(u32 param_1 /* u16 */, int param_2) {
    u32 face = param_1;
    if (face == 0) {
        face = checkNormalFace();
    }
    Player01TexAnmEntry *e = &Player01_texAnmTable()[face]; /* Player01_texAnmTable()[face] */
    u16 anmIdx = (u16) reinterpret_cast<daPy_anmHeap_c *>(m_tex_anm_heap)
                     ->mIdx; /* m_tex_anm_heap.mIdx, read before the call */
    u16 btp_idx = (u16)e->mBtpIdx;
    u16 btk_idx = (u16)e->mBtkIdx;
    BOOL bVar3 = FALSE;
    u32 p2 = (u32)param_2;
    if (face < 0x13 && checkMabaAnimeBtp(anmIdx) && mFace == 0xAD /* checkFaceTypeNot() */) {
        bVar3 = TRUE;
        p2 = m3530;
    }
    daPy_anmHeap_c *h = reinterpret_cast<daPy_anmHeap_c *>(m_tex_anm_heap);
    if (anmIdx == btp_idx) {
        if (mFace == 0xAD && !(mModeFlg & 0x500) /* ModeFlg_00000100 | ModeFlg_00000400 */) {
            m3530 = (u16)p2;
        }
    } else {
        u16 f2 = (u16)h->field_0x2;
        h->mIdx = btp_idx;
        if (f2 == 0xFFFF && (u16)h->field_0x4 == 0xFFFF) {
            Player01_setTextureAnimeResource(this, loadTextureAnimeResource(btp_idx, 0), p2);
        }
    }
    f64 dVar9 = Player01_random();
    /* daPy_matAnm_c statics (HD): m_maba_flg 0x101CEF18, m_maba_timer 0x101CEF19 */

    h = reinterpret_cast<daPy_anmHeap_c *>(m_tex_scroll_heap); /* m_tex_scroll_heap */
    if ((u16)h->mIdx != btk_idx) {
        if (bVar3 && (mModeFlg & 1) && (u8)m_maba_timer != 0 &&
            Player01_game()->play.mEvtCtrl.mMode == 0 && mpAttnActorLockOn == nullptr &&
            btk_idx == dRes_INDEX_LKANM_BTK_TMABA_e) {
            if ((u8)m_morf_frame != 0) {
                return;
            }
            if (dVar9 < 0.44f) {
                btk_idx = 0x1A7;
            } else {
                p2 = 0;
                if (dVar9 < 0.51f) {
                    btk_idx = 0x187; /* TEUP */
                } else if (dVar9 < 0.58f) {
                    btk_idx = 0x183; /* TEDW */
                } else if (dVar9 < 0.65f) {
                    btk_idx = 0x184; /* TEL */
                } else if (dVar9 < 0.72f) {
                    btk_idx = 0x185; /* TER */
                } else if (dVar9 < 0.79f) {
                    btk_idx = 0x181; /* TEDL */
                } else if (dVar9 < 0.86f) {
                    btk_idx = 0x182; /* TEDR */
                } else if (dVar9 < 0.93f) {
                    btk_idx = 0x188; /* TEUR */
                } else {
                    btk_idx = 0x186; /* TEUL */
                }
            }
            u16 f2 = (u16)h->field_0x2;
            h->mIdx = btk_idx;
            if (f2 == 0xFFFF && (u16)h->field_0x4 == 0xFFFF) {
                setTextureScrollResource(loadTextureScrollResource(btk_idx, 0), p2);
            }
            if (btk_idx != 0x1A7) {
                m_maba_timer = 1; /* daPy_matAnm_c::onMabaFlg() */
            }
        } else {
            u16 f2 = (u16)h->field_0x2;
            h->mIdx = btk_idx;
            if (f2 == 0xFFFF && (u16)h->field_0x4 == 0xFFFF) {
                setTextureScrollResource(loadTextureScrollResource(btk_idx, 0), p2);
            }
        }
    } else if (bVar3 && (mModeFlg & 1) && Player01_game()->play.mEvtCtrl.mMode == 0 &&
               (u8)m_morf_frame == 0 && mpAttnActorLockOn == nullptr && btk_idx == 0x1A7 &&
               (u16)h->field_0x2 == 0xFFFF && (u16)h->field_0x4 == 0xFFFF &&
               Player01_random() < 0.025f) {
        u16 idx;
        if (dVar9 < 0.125f) {
            idx = 0x187;
        } else if (dVar9 < 0.25f) {
            idx = 0x183;
        } else if (dVar9 < 0.375f) {
            idx = 0x184;
        } else if (dVar9 < 0.5f) {
            idx = 0x185;
        } else if (dVar9 < 0.625f) {
            idx = 0x181;
        } else if (dVar9 < 0.75f) {
            idx = 0x182;
        } else if (dVar9 < 0.875f) {
            idx = 0x186;
        } else {
            idx = 0x188;
        }
        h->mIdx = idx;
        setTextureScrollResource(loadTextureScrollResource(idx, 0), 0);
        m_maba_timer = 1;
    }
}

/* 023D4F08 */
void daPy_lk_c::initTextureScroll() {
    u32 buf = operator_new_array_align(0x800, 0x20);
    reinterpret_cast<daPy_anmHeap_c*>(m_tex_scroll_heap)->m_buffer = gabi::at<void>(buf);
    if (buf == 0) {
        JUT_ASSERT_fail("d_a_player_main.cpp", 0x5CA5, "m_tex_scroll_heap.m_buffer != (0)");
    }
    /* HD: the eye scroll animation comes from the resident "LkAnm" archive and drives the HD animation object at
     * 0x6F4 (instead of J3DTexMtxAnm objects); the daPy_matAnm_c objects are attached to the materials named by
     * the sead::SafeString table at 0x1046CD2C */
    gabi::Local<SafeString> key;
    key->__vtbl = 0x10034B24;
    key->mStringTop = 0x10035174; /* "LkAnm" */
    u32 btk = Player_getArchiveResource( dComIfG_resControl(), key.get(), dRes_INDEX_LKANM_BTK_TMABA_e);
    gabi::Local<PlayerTextureAnimationSetup> info; /* the animation object's setup (sp+0x14..0x54; HD: 0x40 bytes, field +0x3C) */
    info->mParameters[3] = -1;
    info->field_0x14 = 0;
    info->mParameters[1] = -1;
    info->field_0x18 = 0;
    info->mEnabled = 1;
    info->mParameters[0] = -1;
    info->field_0x11 = 0;
    info->mParameters[2] = -1;
    auto* animationData = gabi::at<PlayerTexScrollData>(gabi::at<PlayerTextureResource>(btk)->mpData);
    info->mParameters[1] = animationData->mMaterialNum;
    s32 v = animationData->field_0x18;
    if (v >= 0) {
        info->mParameters[2] = v;
    }
    v = animationData->field_0x1C;
    if (v >= 0) {
        info->mParameters[3] = v;
    }
    u32 flags = animationData->mFlags;
    info->field_0x14 = 0;
    info->field_0x18 = 0;
    info->field_0x11 = (flags & 1) ^ 1;
    /* HD: +0x3C = the model data's u16 at +0x24 (game test: the field was never set, and the 0x1C-byte Local was
     * smaller than the object the HD animation-object init reads and writes) */
    info->mModelResourceValue = (u16)gabi::at<PlayerModelResource>(PlayerModelData_getResource(gabi::ea(mpCLModelData.get())))->mMaterialNum;
    info->field_0x18 = 0;
    info->mParameters[1] = 2;
    info->field_0x11 = 1;
    info->mParameters[0] = 0x18;
    info->mParameters[2] = 0x10;
    info->mParameters[3] = 0x17C;
    info->field_0x14 = 0;
    u32 b = daPy_newAlign(0x2000, 4);
    PlayerTexScroll_init(gabi::ea(this) + 0x6F4, info.get(), b, 0x2000);
    PlayerTexScroll_setAnm(gabi::ea(this) + 0x6F4, (u32)gabi::at<PlayerTextureResource>(btk)->mpData);
    PlayerTexScroll_setModelData(gabi::ea(this) + 0x6F4, PlayerModelData_getResource(gabi::ea(mpCLModelData.get())));
    u16 material_num = (u16)gabi::at<PlayerTexScrollData>(gabi::at<PlayerTextureResource>(btk)->mpData)->mMaterialNum;
    for (u32 no = 0; no < material_num; no++) {
        u32 mat = gabi::ea(operator_new(0x7C));
        if (mat != 0) {
            mat = daPy_matAnm_ct(mat);
        }
        auto* slot = &m_tex_eye_scroll[no];
        *slot = mat;
        u32 x = PlayerModelData_getResource(gabi::ea(mpCLModelData.get()));
        u32 name = gabi::ea(&eyeMaterialNames.mNames[no]);
        SafeString_vf14_l(name);
        u16 matID = (u16)Player_materialNameIndex(lk_relPtr(x + 0x18), (u32)gabi::at<SafeString>(name)->mStringTop);
        J3DModelData* modelData = mpCLModelData.get();
        u32 cnt = modelData->mMaterialNum;
        u32 anm = *slot;
        u32 m = modelData->mMaterials; /* getMaterialNodePointer(matID) (HD: 0x39C each, entry 0 when out of range) */
        if (matID < cnt) {
            m += matID * 0x39C;
        }
        gabi::at<PlayerMaterialAnimationView>(m)->mMaterialAnm = gabi::at<daPy_matAnm_c>(anm);
    }
}

// HD: these FPR copies preserve the original bits and interleaved access order.
static inline void Player_copyTransform(J3DTransformInfo* destination, const J3DTransformInfo* source) {
    fcpy_l(gabi::ea(&destination->mScale.x), gabi::ea(&source->mScale.x));
    fcpy_l(gabi::ea(&destination->mScale.y), gabi::ea(&source->mScale.y));
    fcpy_l(gabi::ea(&destination->mScale.z), gabi::ea(&source->mScale.z));
    destination->mRotation.x = source->mRotation.x;
    destination->mRotation.y = source->mRotation.y;
    destination->mRotation.z = source->mRotation.z;
    fcpy_l(gabi::ea(&destination->mTranslate.x), gabi::ea(&source->mTranslate.x));
    fcpy_l(gabi::ea(&destination->mTranslate.y), gabi::ea(&source->mTranslate.y));
    fcpy_l(gabi::ea(&destination->mTranslate.z), gabi::ea(&source->mTranslate.z));
}
static inline void lk_saveTransInfo(u32 self, int jnt, u32 ti) {
    daPy_lk_c* player = gabi::at<daPy_lk_c>(self);
    reinterpret_cast<be<u8>*>(player->m68E2)[jnt] = 2;
    PlayerJointState* state = reinterpret_cast<PlayerJointState*>(player->m3658);
    Player_copyTransform(&state->mSavedTransform[jnt], gabi::at<J3DTransformInfo>(ti));
}
static inline void lk_copy16(u32 dst, u32 src) {
    auto* destination = gabi::at<PlayerSavedQuaternion>(dst);
    auto* source = gabi::at<PlayerSavedQuaternion>(src);
    for (int i = 0; i < 4; i++) destination->mWords[i] = source->mWords[i];
}

/* 023D6B30 */
BOOL daPy_lk_c::jointBeforeCB(int jnt_no, u32 param_2 /* J3DTransformInfo* */, u32 param_3 /* Quaternion* */) {
    auto* feet = reinterpret_cast<daPy_footData_c*>(mFootData);
    u32 self = gabi::ea(this);
    J3DTransformInfo* transform = gabi::at<J3DTransformInfo>(param_2);
    gabi::Local<csXyz> L;
    csXyz_ct(L.get(), 0, 0, 0);
    csXyz* angles = L.get();
    bool hatSection = true;
    if (jnt_no == CL_JNT_HEAD_JNT_e) {
        angles->y = m3564.z;
        angles->x = m3564.y;
        angles->z = m3564.x;
    } else if (jnt_no == CL_JNT_LCLOTCH_JNT_e) {
        lk_saveTransInfo(self, jnt_no, param_2);
        transform->mTranslate.x = gabi::fsubs_ppc((f32)transform->mTranslate.x, feet[1].field_0x030 /* mFootData[1].field_0x030 */);
    } else if (jnt_no == CL_JNT_RCLOTCH_JNT_e) {
        lk_saveTransInfo(self, jnt_no, param_2);
        transform->mTranslate.x = gabi::fsubs_ppc((f32)transform->mTranslate.x, feet[0].field_0x030 /* mFootData[0].field_0x030 */);
    } else if (jnt_no == CL_JNT_LMOMI_JNT_e) {
        angles->y = m3516;
        angles->z = m351A;
    } else if (jnt_no == CL_JNT_RMOMI_JNT_e) {
        angles->z = m351A;
        angles->y = m3518;
    } else if (jnt_no == CL_JNT_BODY_CHN_e) {
        /* local_38.set(-mBodyAngle.z, mBodyAngle.y, mBodyAngle.x) */
        s16 bz = mBodyAngle.z;
        s16 by = mBodyAngle.y;
        angles->x = (s16)-bz;
        angles->y = by;
        angles->z = mBodyAngle.x;
        lk_saveTransInfo(self, jnt_no, param_2);
        transform->mTranslate.y = gabi::fadds_ppc((f32)transform->mTranslate.y, m35D8);
    } else if (jnt_no == 0) {
        angles->x = m34F2;
        angles->z = m34F4;
        if (m34C2 == 0xC) {
            // HD: root and waist translation are suppressed in this demo state.
            transform->mTranslate.y = 0.0f;
            transform->mTranslate.x = 0.0f;
            transform->mTranslate.z = 0.0f;
        }
    } else if (jnt_no == 1 || jnt_no == 0x29) {
        if (m34C2 == 0xC) { /* HD */
            transform->mTranslate.y = 0.0f;
            transform->mTranslate.x = 0.0f;
            transform->mTranslate.z = 0.0f;
        }
    } else if (jnt_no == 0xA || jnt_no == 0xB) {
        /* HD: the ship ride arm angles moved to jointAfterCB; only the (unused) creating check is left */
        if ((dComIfGp_getPlayerStatus0(0) & 0x10000) && (u8)gabi::at<mDoExtAnimation::mDoExt_MtxCalcOldFrame>(m_old_fdata)->mOldFrameFlg != 0 &&
            gabi::ea(dComIfGp_getShipActor()) != 0) {
            u32 ship = gabi::ea(dComIfGp_getShipActor());
            Player_isCreating(ship != 0 ? (u32)gabi::at<base_process_class>(ship)->mBsPcId : 0xFFFFFFFF);
        }
    }
    if (resetFlg0() & daPyRFlg0_ORIGINAL_HAT_ANIM) {
        hatSection = false;
    }
    if (hatSection && jnt_no == CL_JNT_HATA_JNT_e) {
        PlayerEulerToQuat(0, (s32)m34F8, (s32)(s16)(m34F6 - 0x4000 + m3528 + m350E - 0x4000), param_3);
    } else if (hatSection && jnt_no == CL_JNT_HATB_JNT_e) {
        PlayerEulerToQuat(0, (s32)m34FC, (s32)(s16)(m34FA + m3510), param_3);
    } else if (hatSection && jnt_no == CL_JNT_HATC_JNT_e) {
        PlayerEulerToQuat(0, (s32)m3500, (s32)(s16)(m34FE + m3512), param_3);
    } else if (jnt_no == CL_JNT_LINK_ROOT_e) {
        if (m34C2 == 1) {
            transform->mTranslate.x = 0.0f;
            transform->mTranslate.z = 0.0f;
        } else if (m34C2 == 5) {
            transform->mTranslate.x = 0.0f;
            fcpy_l(gabi::ea(&transform->mTranslate.y), gabi::ea(&m35E0));
            transform->mTranslate.z = 0.0f;
        }
    } else if (jnt_no == CL_JNT_WAIST_JNT_e) {
        s16 e0 = m34E0;
        s16 e4 = m34E4;
        if (e0 != 0 || e4 != 0) {
            angles->y = e4;
            angles->z = e0;
        }
    }
    s16 lx = (s16)angles->x, ly = (s16)angles->y;
    if ((lx | ly) != 0 || (s16)angles->z != 0) {
        auto* flags = reinterpret_cast<be<u8>*>(m68E2);
        flags[jnt_no] = flags[jnt_no] | 1;
        auto* state = reinterpret_cast<PlayerJointState*>(m3658);
        // HD: preserve the original quaternion before applying joint angles.
        lk_copy16(gabi::ea(&state->mSavedQuaternion[jnt_no]), param_3);
        gabi::Local<PlayerSavedQuaternion> local_20;
        gabi::Local<PlayerSavedQuaternion> afStack_30;
        if ((lx | ly) != 0) {
            auto* quaternion = gabi::at<PlayerSavedQuaternion>(param_3);
            local_20.get()->mWords[3] = quaternion->mWords[3];
            local_20.get()->mWords[2] = quaternion->mWords[2];
            local_20.get()->mWords[0] = quaternion->mWords[0];
            local_20.get()->mWords[1] = quaternion->mWords[1];
            PlayerEulerToQuat((s32)lx, (s32)ly, 0, gabi::ea(afStack_30.get()));
            PlayerQuatConcat(gabi::ea(local_20.get()), gabi::ea(afStack_30.get()), param_3);
        }
        s16 lz = (s16)angles->z;
        if (lz != 0) {
            lk_copy16(gabi::ea(local_20.get()), param_3);
            PlayerEulerToQuat(0, 0, (s32)lz, gabi::ea(afStack_30.get()));
            PlayerQuatConcat(gabi::ea(local_20.get()), gabi::ea(afStack_30.get()), param_3);
        }
    }
    u16 upper = LK_upperAnmIdx();
    bool hit = false;
    bool checkOtherAnimation = false;
    if (upper == 0x16 || upper == 0x1B) { /* checkUpperGuardAnime() */
        hit = mCurProc != daPyProc_BACK_JUMP_e;
        checkOtherAnimation = !hit;
    } else if (upper >= 0x95 && upper <= 0x96) { /* checkGrabAnime() */
        hit = true;
    } else {
        checkOtherAnimation = true;
    }
    if (checkOtherAnimation) {
        u16 up0 = reinterpret_cast<daPy_anmHeap_c*>(m_anm_heap_upper)[0].mIdx;
        hit = up0 == 0x10 /* ATNBOKO */ || up0 == 0x1C /* ATNHAM */ || checkUpperReadyThrowAnime();
    }
    if (hit) {
        if (jnt_no == 0) {
            lk_copy16(gabi::ea(m3648), param_3); /* m3648 = *param_3 */
            lk_copy16(param_3, 0x101CEE40);     /* HD: *param_3 = norm_quat */
        } else if (jnt_no == 0x1D) {
            /* HD: the waist chain gets m3648 concatenated (no root matrix save/restore) */
            PlayerQuatConcat(gabi::ea(m3648), param_3, param_3);
        }
    }
    return TRUE;
}

/* 023DFDD8 */
BOOL daPy_lk_c::commonProcInit(int proc) {
    u32 self = gabi::ea(this);
    Player01ProcInitEntry *procInit =
        &mProcInitTable
             .mEntries[proc]; /* mProcInitTable.mEntries[proc]: ProcFunc (8 bytes), mProcFlags */
    BOOL resetDemoAnime = FALSE;
    s32 cur = mCurProc;
    if (cur == player01::daPyProc_SLIP_e) {
        Player01_stopSound(player01::JA_SE_LK_RUN_SLIP);
    } else if (cur == player01::daPyProc_GRAB_MISS_e ||
               (cur == player01::daPyProc_GRAB_READY_e && proc != 0x70)) {
        Player01_clearActorKeep(&mActorKeepRope);
    } else if (cur == player01::daPyProc_DEMO_TALISMAN_WAIT_e) {
        seStartOnlyReverb(player01::JA_SE_LK_ITEM_TAKEOUT);
        reinterpret_cast<PlayerModelState *>(mD90)->mpEquipItemModel = nullptr;
    } else if (cur == player01::daPyProc_FAN_SWING_e) {
        setSmallFanModel();
    } else if (cur == daPyProc_FAN_GLIDE_e) {
        deleteEquipItem(FALSE);
        maxFallSpeed = -175.0f; /* maxFallSpeed (HD: constant) */
        setSmallFanModel();
        mEquipItem = player01::dItemNo_DEKU_LEAF_e;
        fcpy_l(gabi::ea(&m35F0), gabi::ea(&m3688.y));
        reinterpret_cast<be<u32> *>(&m3730)[0] =
            reinterpret_cast<be<u32> *>(&cXyz::Zero)[0]; /* m3730 = cXyz::Zero */
        reinterpret_cast<be<u32> *>(&m3730)[1] = reinterpret_cast<be<u32> *>(&cXyz::Zero)[1];
        m34E4 = 0;
        reinterpret_cast<be<u32> *>(&m3730)[2] = reinterpret_cast<be<u32> *>(&cXyz::Zero)[2];
        m34E0 = 0;
    } else if (cur == player01::daPyProc_SLOW_FALL_e) {
        maxFallSpeed = -175.0f;
    } else if (cur == daPyProc_DEMO_TOOL_e) {
        resetDemoAnime = TRUE;
        speed.y = 0.0f;
    } else if (cur == player01::daPyProc_DEMO_GET_ITEM_e ||
               cur == player01::daPyProc_DEMO_GET_DANCE_e) {
        Player01_itemGetColorOff();
        if (mCurProc == 0xAE && mProcVar4 != 0) {
            m34C2 = 0xB;
        }
    } else if (cur == player01::daPyProc_DEMO_PRESENT_e || cur == player01::daPyProc_NOT_USE_e) {
        fopAc_ac_c *item = Player01_getItemEventPartner(this);
        if (item != nullptr && (fpcM_GetName(item) == player01::fpcNm_ITEM_e ||
                                fpcM_GetName(item) == player01::fpcNm_Demo_Item_e)) {
            Player01_deadItem(item);
        }
    } else if (cur == player01::daPyProc_DEMO_LETTER_READ_e) {
        deleteEquipItem(FALSE);
    } else if (cur == player01::daPyProc_CUT_ROLL_END_e) {
        u32 em = (u32) reinterpret_cast<daPy_mtxFollowEcallBack_c *>(m33A8)
                     ->mpEmitter.v; /* m33A8.getEmitter() */
        if (em != 0) {
            reinterpret_cast<Player01EmitterAlpha *>(gabi::at<void>(em))->mGlobalAlpha =
                0; /* setGlobalAlpha(0) */
        }
    } else if (cur == player01::daPyProc_SHIP_BOW_e) {
        deleteArrow();
    }
    returnKeepItemData();
    u32 mode = mModeFlg;
    BOOL temp_r28 = (mode >> 18) & 1; /* checkModeFlg(ModeFlg_SWIM) */
    if (mode & 0x10040820 /* ModeFlg_HANG | ModeFlg_ROPE | ModeFlg_SWIM | ModeFlg_CAUGHT */) {
        mode = mModeFlg;
        m34C2 = 0xA;
    }
    mCurProc = proc;
    u8 c2 = m34C2;
    reinterpret_cast<Player01ProcFuncWords *>(&mCurProcFunc)->mWords[0] =
        procInit->mProcFunc.mWords[0]; /* mCurProcFunc = procInit.mProcFunc */
    m3598 = 0.0f;
    reinterpret_cast<Player01ProcFuncWords *>(&mCurProcFunc)->mWords[1] =
        procInit->mProcFunc.mWords[1];
    BOOL temp_r29 = ((mode >> 1) & 1) ^ 1; /* !checkModeFlg(ModeFlg_MIDAIR) */
    mode = (u32)procInit->mProcFlags;
    mModeFlg = mode; /* procInit.mProcFlags */
    if (c2 == 1) {
        mode = mModeFlg;
        m34C2 = 2;
    }
    if (mode & 0x10040820) {
        mode = mModeFlg;
        m34C2 = 0xB;
    }
    u32 acch = (u32)mAcch.m_flags; /* mAcch flags */
    if (mode & player01::ModeFlg_HANG) {
        mAcch.m_flags = acch & ~0x2000u; /* mAcch.OffLineCheck() */
    } else {
        mAcch.m_flags = acch | 0x2000; /* mAcch.OnLineCheck() */
    }
    /* HD: a timer in the tail (0x8260) */
    if (((u32)Player01_game()->play.mPlayerStatus[0][0] & 0x200000) ||
        ((u32)Player01_game()->play.mPlayerStatus[0][1] & 8)) {
        reinterpret_cast<PlayerTailRuntime *>(m8260)->mItemButtonLockTimer = 0x1E;
    }
    be<u32> *a = &Player01_game()->play.mPlayerStatus[0][0]; /* dComIfGp_clearPlayerStatus0(0,
                                                                ~(BOOMERANG_WAIT | UNK10)) */
    *a = (u32)*a & 0x00400010;
    a = &Player01_game()->play.mPlayerStatus[0][1]; /* dComIfGp_clearPlayerStatus1 */
    *a = (u32)*a & 0xFFF48400;
    u32 rf0 = resetFlg0() | 0x08000000; /* onResetFlg0(daPyRFlg0_NOT_ATTACKING) */
    u32 nf1 = noResetFlg1();
    reinterpret_cast<Player01FanSwingCallback *>(mFanSwingCb)->mAlphaOutFlag =
        1; /* mFanSwingCb.onAlphaOutFlg() */
    reinterpret_cast<daPy_sightPacket_c *>(mSightPacket)->mDrawFlag =
        0;                              /* mSightPacket.offDrawFlg() */
    setNoResetFlg1(nf1 & ~0x08000000u); /* offNoResetFlg1(daPyFlg1_UNK8000000) */
    shape_angle.x = 0;
    setResetFlg0(rf0 & ~2u); /* offResetFlg0(daPyRFlg0_UNK2) */
    u32 nf0 = mNoResetFlg0;
    m3544 = 0;
    m34EC = 0;
    m35C4 = 0.0f;
    m34C5 = 0;
    reinterpret_cast<daPy_sightPacket_c *>(mSightPacket)->mLockFlag =
        0; /* mSightPacket.offLockFlg() */
    shape_angle.z = 0;
    m34F4 = 0;
    gravity = -2.5f;          /* gravity (HD: constant) */
    mNoResetFlg0 = nf0 & ~4u; /* offNoResetFlg0(daPyFlg0_UNK4) */
    m34F2 = 0;
    BOOL equip = checkEquipAnime();
    mode = mModeFlg;
    if (!(equip && (mode & 4)) && !(mode & 0x1000)) {
        u16 upper = LK_upperAnmIdx();
        if (!(upper == 0x95 || upper == 0x96 ||
              upper == 0x34 /* checkGrabAnime() || checkBoomerangThrowAnime() */) ||
            !(mode & 0x4000)) {
            resetActAnimeUpper(2 /* UPPER_MOVE2_e */, -1.0f);
            mode = mModeFlg;
            if (!(mode & player01::ModeFlg_GRAB)) {
                freeGrabItem();
                mode = mModeFlg;
            }
        }
    }
    if (!(mode & 0x100000)) {
        u16 upper = LK_upperAnmIdx();
        if (upper != 0x95 && upper != 0x96 && mActorKeepGrab.mActor != nullptr) {
            freeGrabItem();
        }
    }
    if (temp_r29 && (mModeFlg & player01::ModeFlg_MIDAIR)) {
        f32 x = current.pos.x, y = current.pos.y;
        m3688.x = x;
        m35F4 = y;
        f32 z = current.pos.z;
        m3688.y = y;
        m3688.z = z;
        m35F0 = y;
    }
    s32 cp = mCurProc;
    mode = mModeFlg;
    if (cp != player01::daPyProc_SWIM_MOVE_e) {
        /* mSwimTailEcallBack[0/1].onEnd() */
        reinterpret_cast<Player01SwimTailCallback *>(mSwimTailEcallBack)[0].mTimer = 0;
        reinterpret_cast<Player01SwimTailCallback *>(mSwimTailEcallBack)[1].mTimer = 0;
        reinterpret_cast<Player01SwimTailCallback *>(mSwimTailEcallBack)[0].mEnd = 1;
        reinterpret_cast<Player01SwimTailCallback *>(mSwimTailEcallBack)[1].mEnd = 1;
    }
    if (!(mode & 0x40000)) {
        m3608 = 0.0f;
    }
    resetFootEffect();
    if (resetDemoAnime) {
        resetDemoTextureAnime();
    }
    daPy_followEcallBack_end(self + 0x67A0); /* m338C.end() */
    Player01_endMtxEmitter(self + 0x67BC);   /* m33A8 */
    Player01_endMtxEmitter(self + 0x66F8);   /* m32E4 */
    Player01_endMtxEmitter(self + 0x6704);   /* m32F0 */
    Player01_endSmoke(self + 0x6710);
    f32 wear = field_0x2b0;
    u16 item = mEquipItem;
    u32 nf1b = noResetFlg1();
    if (!(wear < 0.0f) /* !checkGrabWear() */) {
        m35D8 = 0.0f;
    }
    m35EC = 0.0f;
    setNoResetFlg1(nf1b & 0xFE7FFF7F); /* offNoResetFlg1(UNK80 | UNK800000 | UNK1000000) */
    if (item == player01::dItemNo_SKULL_HAMMER_e) {
        Player01_changeBckOnly(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mSwordAnim),
                               getItemAnimeResource(0x97 /* HAMMERDAM */));
    } else if (checkBowItem(item) && !checkBowAnime()) {
        Player01_changeBckOnly(gabi::ea(&reinterpret_cast<PlayerModelState *>(mD90)->mSwordAnim),
                               getItemAnimeResource(0xD /* ARROWRELORDA */));
    }
    mode = mModeFlg;
    m35E8 = 0.0f;
    if (!(mode & player01::ModeFlg_ROPE) && mCurProc != player01::daPyProc_ROPE_SUBJECT_e &&
        mCurProc != player01::daPyProc_ROPE_MOVE_e) {
        freeRopeItem();
        mode = mModeFlg;
    }
    if (!(mode & player01::ModeFlg_HOOKSHOT)) {
        freeHookshotItem();
    }
    if (temp_r28 && !(mModeFlg & 0x40000)) {
        swimOutAfter(FALSE);
    }
    Player01_game()->play.mDirection = 0; /* dComIfGp_setAdvanceDirection(0) */
    setBgCheckParam();
    return TRUE;
}

/* HD mDoExt_AnmRatioPack::setRatio(pack, v) on a joint subtree: the joints jnt..end-1 of the pb calc's model
 * (end from the joint tree, 027E0174) get the ratio v in the pack's per-joint array (packs at pb + 0x7C, array +0xC) */
static inline s32 lk_jntEnd(u32 pb, s32 jnt) {
    auto* calc = gabi::at<mDoExtAnimation::BlendCalc>(pb);
    auto* model = gabi::at<J3DModel>(calc->mpModel);
    u32 tree = Player_getJointTree(gabi::ea(model->getModelData()));
    return Player_jointSubtreeEnd(tree, jnt);
}
static inline be<f32>* Player_jointRatios(u32 pb, u32 arrOff) {
    auto* calc = gabi::at<mDoExtAnimation::BlendCalc>(pb);
    auto* pack = gabi::at<mDoExtAnimation::AnmRatio>(calc->mAnmRatio + arrOff - 0xC);
    return gabi::at<be<f32>>(pack->mpValues);
}
static inline void lk_ratioFill(u32 pb, u32 arrOff, s32 from, s32 end, f32 v) {
    for (s32 i = from; i < end; i++) {
        // HD: each pack owns a per-joint ratio array. Reload it for each store.
        Player_jointRatios(pb, arrOff)[i] = v;
    }
}

/* 023D7A0C */
BOOL daPy_lk_c::jointCB0(int jnt_no) {
    u32 self = gabi::ea(this);
    if (checkEquipAnime()) {
        if (!(mModeFlg & 1)) {
            if (jnt_no == 2) {
                u32 pb = m_pbCalc[1]; /* m_pbCalc[PART_UPPER_e] */
                lk_ratioFill(pb, 0x2C, 2, lk_jntEnd(pb, 2), 0.0f);
            } else if (jnt_no == 5 || jnt_no == 9 || jnt_no == 0xD || jnt_no == 0x1D) {
                u32 pb = m_pbCalc[1];
                lk_ratioFill(pb, 0x2C, jnt_no, lk_jntEnd(pb, jnt_no), 1.0f);
            }
        }
        return TRUE;
    }
    if (checkUpperReadyAnime() || LK_upperAnmIdx() == dRes_INDEX_LKANM_BCK_ROPETHROW_e /* ROPETHROW */ ||
        (mCurProc != daPyProc_GRAB_WAIT_e && (LK_upperAnmIdx() == 0x95 || LK_upperAnmIdx() == 0x96))) {
        if (jnt_no == 0xF) {
            u32 pb = m_pbCalc[1];
            lk_ratioFill(pb, 0x2C, 0xF, lk_jntEnd(pb, 0xF), 0.0f);
            Player_jointRatios(m_pbCalc[1], 0x2C)[15] = 1.0f;
        } else if (jnt_no == 0x1D) {
            u32 pb = m_pbCalc[1];
            lk_ratioFill(pb, 0x2C, 0x1D, lk_jntEnd(pb, 0x1D), 1.0f);
        }
        return TRUE;
    }
    if ((dComIfGp_getPlayerStatus0(0) & 0x10000) /* daPyStts0_SHIP_RIDE_e */ && !checkShipNotNormalMode()) {
        if (jnt_no == 0xF) {
            u32 pb = m_pbCalc[1];
            lk_ratioFill(pb, 0x2C, 0xF, lk_jntEnd(pb, 0xF), 1.0f);
            Player_jointRatios(m_pbCalc[1], 0x2C)[15] = 0.0f;
        } else if (jnt_no == 0x1D) {
            u32 pb = m_pbCalc[1];
            lk_ratioFill(pb, 0x2C, 0x1D, lk_jntEnd(pb, 0x1D), 0.0f);
        }
        return TRUE;
    }
    u16 upper = LK_upperAnmIdx();
    if (upper == 0x16 || upper == 0x1B) { /* checkUpperGuardAnime() */
        if (jnt_no == 1) {
            u32 pb = m_pbCalc[1];
            fcpy_l(gabi::ea(&guard_rate), gabi::ea(&Player_jointRatios(pb, 0x2C)[1])); /* m_pbCalc[PART_UPPER_e]->getRatio(2) */
            lk_ratioFill(pb, 0x2C, 1, lk_jntEnd(pb, 1), 1.0f);
        } else if (jnt_no == 0xE) {
            u32 pb = m_pbCalc[1];
            lk_ratioFill(pb, 0x2C, 0xE, lk_jntEnd(pb, 0xE), 1.0f);
        } else if (jnt_no == 5 || jnt_no == 9 || jnt_no == 0xD || jnt_no == 0x1D) {
            u32 pb = m_pbCalc[1];
            f32 r = guard_rate;
            lk_ratioFill(pb, 0x2C, jnt_no, lk_jntEnd(pb, jnt_no), r);
        }
        return TRUE;
    }
    if ((dComIfGp_getPlayerStatus1(0) & 1) /* daPyStts1_WIND_WAKER_CONDUCT_e */ && (mCurProc != daPyProc_TACT_PLAY_e || mProcVar6 != 0)) {
        if (jnt_no == 5) {
            u32 pb = m_pbCalc[1];
            lk_ratioFill(pb, 0x2C, 5, lk_jntEnd(pb, 5), 1.0f);
        } else if (jnt_no == 9) {
            u32 pb = m_pbCalc[1];
            lk_ratioFill(pb, 0x1C, 9, lk_jntEnd(pb, 9), 1.0f); /* setRatio(1, 1.0f) */
            pb = m_pbCalc[1];
            lk_ratioFill(pb, 0x2C, 9, lk_jntEnd(pb, 9), 0.0f);
        } else if (jnt_no == 0xD || jnt_no == 0xE || jnt_no == 0x1D) {
            u32 pb = m_pbCalc[1];
            lk_ratioFill(pb, 0x1C, jnt_no, lk_jntEnd(pb, jnt_no), 0.0f);
        }
    }
    (void)self;
    return TRUE;
}

/* 023D8624 */
BOOL daPy_lk_c::setDrawHandModel() {
    u32 self = gabi::ea(this);
#define LK_checkPlayerGuard() gabi::call_ptr<BOOL>(gabi::at<PlayerActorDispatch>(self)->mpVtable->mCheckPlayerGuard, this) /* virtual */
    gabi::at<J3DShape>(mpLhandShape)->hide(); /* mpLhandShape->hide() */
    gabi::at<J3DShape>(mpRhandShape)->hide(); /* mpRhandShape->hide() */
    u32 l, r;
    if (mCurProc == daPyProc_DEMO_TOOL_e) {
        l = mLeftHandIdx;
        r = mRightHandIdx;
    } else {
        u16 upper = LK_upperAnmIdx(); /* read once; also used after the calls below */
        if ((upper >= 0x33 && upper <= 0x34) || (upper >= 0x95 && upper <= 0x96)) {
            /* checkGrabAnime() || checkBoomerangThrowAnime() || checkBoomerangCatchAnime() */
            l = 0;
            r = 0;
        } else if (upper == 0xA7 /* checkHookshotReadyAnime() */) {
            l = 3;
            r = 0;
        } else if (checkBowReadyAnime()) {
            l = 3;
            r = 0xA;
        } else if (upper == 0xE /* checkBowShootAnime() */) {
            l = 6;
            r = 0xA;
        } else if (upper == 0x35 /* checkBoomerangReadyAnime() */) {
            l = 0;
            r = 7;
        } else {
            u16 item = mEquipItem;
            if (checkBottleItem(item) || checkPhotoBoxItem(item)) {
                l = 6;
                r = mRightHandIdx;
            } else if (checkBowItem(item)) {
                l = mLeftHandIdx;
                r = 8;
            } else if (item == 0x101 /* daPyItem_BOKO_e */) {
                l = 3;
                r = 9;
            } else if (item == 0x103 /* daPyItem_SWORD_e */) {
                l = 3;
                if (dComIfGs_getSelectEquip(1) == 0xFF /* !checkShieldEquip() */) {
                    r = mRightHandIdx;
                } else {
                    r = 8;
                }
            } else if ((item == 0x25 && !(mModeFlg & 0x800)) || (item == 0x33 && (u8)*reinterpret_cast<be<u8>*>(&mCutType) == 0 /* mCutType == CUT_TYPE_NONE */)) {
                l = 3;
                r = 8;
            } else if (item == 0x2D /* BOOMERANG */) {
                l = 0;
                r = mRightHandIdx;
            } else if (item == 0x22 /* WIND_WAKER */) {
                l = 5;
                r = mRightHandIdx;
            } else if (item == 0x2F /* HOOKSHOT */) {
                l = 2;
                r = mRightHandIdx;
            } else if (item == 0x34 /* DEKU_LEAF */) {
                l = 3;
                r = mRightHandIdx;
            } else if (item == 0x20 /* TELESCOPE */) {
                l = 1;
                r = mRightHandIdx;
            } else {
                if (item == 0x100 && upper == 0x104 /* TAKEBOTH */) {
                    l = 1;
                } else if (item == 0x100 && checkItemEquipAnime()) {
                    l = 0;
                } else {
                    l = mLeftHandIdx;
                }
                if (LK_checkPlayerGuard() || mCurProc == daPyProc_GUARD_CRASH_e) {
                    r = 8;
                } else if (mEquipItem == 0x100 && LK_upperAnmIdx() == dRes_INDEX_LKANM_BCK_TAKEBOTH_e) {
                    r = 7;
                } else if (mEquipItem == 0x100 && checkItemEquipAnime()) {
                    r = 0;
                } else {
                    r = mRightHandIdx;
                }
            }
        }
    }
    if (LK_checkPlayerGuard()) {
        r = 8; /* HANDS_JNT_CL_RHANDC_e */
    }
#undef LK_checkPlayerGuard
    if (l == HANDS_JNT_WORLD_ROOT_e) {
        u32 shape = gabi::ea(gabi::at<PlayerJointNodeView>(lk_jointNode(gabi::ea(mpCLModelData.get()), CL_JNT_CL_LHANDA_e))->mMesh->mShape.get());
        mpLhandShape = shape;
        gabi::at<J3DShape>(shape)->show();
    } else {
        u32 hdata = gabi::ea(gabi::at<J3DModel>(gabi::ea(mpHandsModel.get()))->getModelData()); /* mpHandsModel->getModelData() */
        mpLhandShape = gabi::ea(gabi::at<PlayerJointNodeView>(lk_jointNode(hdata, l))->mMesh->mShape.get());
        u32 src = lk_getAnmMtx(gabi::ea(mpCLModel.get()), 8);
        mtx_copy_q(lk_getAnmMtx(gabi::ea(mpHandsModel.get()), l), src); /* mpHandsModel->setAnmMtx(l, mpCLModel->getAnmMtx(CL_LHANDA)) */
        gabi::at<J3DShape>(mpLhandShape)->show();
    }
    if (LK_upperAnmIdx() == dRes_INDEX_LKANM_BCK_ARROWSHOOT_e) {
        /* HD: while shooting the bow from the ship (or the HD proc 0x94) in the first-person camera, a hand material flag is cleared */
        u32 body = Player_cameraBody();
        if ((u8)gabi::at<PlayerCameraHandState>(body)->mFirstPersonFlags[0] != 0 && (u8)gabi::at<PlayerCameraHandState>(body)->mFirstPersonFlags[1] != 0 && (u8)gabi::at<PlayerCameraHandState>(body)->mFirstPersonFlags[2] != 0 &&
            (mCurProc == 0x94 || mCurProc == 0x8D)) {
            u32 hands = gabi::ea(mpHandsModel.get());
            u32 mesh = gabi::ea(gabi::at<PlayerJointNodeView>(lk_jointNode(gabi::ea(gabi::at<J3DModel>(hands)->getModelData()), l))->mMesh.get());
            u16 idx = (u16)gabi::at<PlayerMaterialResource>(gabi::at<PlayerMaterialAnimationView>(mesh)->mpData)->mIndex;
            u32 m = 0;
            if (idx < (u32)gabi::at<PlayerModelDrawState>(hands)->mMaterialNum) {
                m = gabi::ea(gabi::at<PlayerModelDrawState>(hands)->mpMaterials.get()) + idx * sizeof(PlayerRuntimeMaterial);
            }
            gabi::at<PlayerRuntimeMaterial>(m)->mFlags = (u32)gabi::at<PlayerRuntimeMaterial>(m)->mFlags & ~6u;
        }
    }
    if (r == 0) {
        u32 shape = gabi::ea(gabi::at<PlayerJointNodeView>(lk_jointNode(gabi::ea(mpCLModelData.get()), CL_JNT_CL_RHANDA_e))->mMesh->mShape.get());
        mpRhandShape = shape;
        gabi::at<J3DShape>(shape)->show();
    } else {
        u32 hdata = gabi::ea(gabi::at<J3DModel>(gabi::ea(mpHandsModel.get()))->getModelData());
        mpRhandShape = gabi::ea(gabi::at<PlayerJointNodeView>(lk_jointNode(hdata, r))->mMesh->mShape.get());
        u32 src = lk_getAnmMtx(gabi::ea(mpCLModel.get()), 0xC);
        mtx_copy_q(lk_getAnmMtx(gabi::ea(mpHandsModel.get()), r), src);
        gabi::at<J3DShape>(mpRhandShape)->show();
    }
    return TRUE;
}

/* 023D5548 */
BOOL daPy_lk_c::createHeap() {
    PlayerModelState& models = *reinterpret_cast<PlayerModelState*>(mD90);
    auto* packets = reinterpret_cast<PlayerDrawPacket*>(models._effects);
    u32 self = gabi::ea(this);
#define LK_ASSERT(line, msg) JUT_ASSERT_fail("d_a_player_main.cpp", (line), (msg))
    reinterpret_cast<PlayerTailRuntime*>(m8260)->mItemButtonLockTimer = 0;
    mpCLModelData = gabi::at<J3DModelData>(initModel(gabi::ea(&mpCLModel), dRes_INDEX_LINK_BDL_CL_e, 0x37221222));
    initModel(gabi::ea(&mpHandsModel), dRes_INDEX_LINK_BDL_HANDS_e, 0x37221222);
    initModel(gabi::ea(&mpKatsuraModel), dRes_INDEX_LINK_BDL_KATSURA_e, 0x37221222);
    initModel(gabi::ea(&mpYamuModel), dRes_INDEX_LINK_BDL_YAMU_e, 0x37221222);
    initModel(gabi::ea(&models.mpPringModel), dRes_INDEX_LINK_BDL_PRING_e, 0x37221222);
    initModel(gabi::ea(&mpSwgripaModel), dRes_INDEX_LINK_BDL_SWGRIPA_e, 0x37221222);
    u32 data = initModel(gabi::ea(&mpSwgripmsModel), dRes_INDEX_LINK_BDL_SWGRIPMS_e, 0x37221222);
    u32 model = gabi::ea(mpSwgripmsModel.get());
    u32 res = lk_getLinkRes_l(0xF);
    if (!Player_initBckAnimation(gabi::ea(&mSwgripmsabBckAnim), gabi::ea(gabi::at<J3DModel>(model)->getModelData()), res, 0, 2,
                          1.0f, 0, -1, 0)) {
        LK_ASSERT(0x5D32, "0");
    }
    /* HD: the brk/btk animations of the models are objects (initBrkAnm / initBtkAnm) */
    initBrkAnm(data, gabi::ea(&models.mTswgripmsabBrk), 0x56);
    initBtkAnm(data, gabi::ea(&models.mTswgripmsBtk), 0x67);
    initModel(gabi::ea(&models.mpPodmsModel), dRes_INDEX_LINK_BDL_PODMS_e, 0x37221222);
    initModel(gabi::ea(&models.mpShaModel), dRes_INDEX_LINK_BDL_SHA_e, 0x37221222);
    model = gabi::ea(models.mpShaModel.get());
    res = lk_getLinkRes_l(0xA);
    if (!Player_initBckAnimation(gabi::ea(&models.mAtngshaBck), gabi::ea(gabi::at<J3DModel>(model)->getModelData()), res, 0, 0, 1.0f, 0, -1, 0)) {
        LK_ASSERT(0x5D44, "0");
    }
    data = initModel(gabi::ea(&models.mpShmsModel), dRes_INDEX_LINK_BDL_SHMS_e, 0x37221222);
    initBtkAnm(data, gabi::ea(&models.mTshmsBtk), 0x66);
    data = initModel(gabi::ea(&models.mpYmsls00Model) /* mpYmsls00Model */, dRes_INDEX_LINK_BDL_YMSLS00_e, 0x13000222);
    initBtkAnm(data, gabi::ea(&models.mYmsls00Btk), 0x6C);
    initModel(gabi::ea(&models.mpHbootsModels[0]), dRes_INDEX_LINK_BDL_HBOOTS_e, 0x37221222);
    initModel(gabi::ea(&models.mpHbootsModels[1]), dRes_INDEX_LINK_BDL_HBOOTS_e, 0x37221222);
    data = initModel(gabi::ea(&models.mpSuimenMunyaModel), dRes_INDEX_LINK_BDL_SUIMEN_MUNYA_e, 0x11001222);
    res = lk_getLinkRes_l(0x64);
    s32 mode = anm_vf0C_l(res);
    Player_initBtkAnimation(gabi::ea(&models.mSuimenMunyaBtk), data, res, 1, mode, 1.0f, 0, -1, 0, 0);
    gabi::at<mDoExt_btkAnm>(gabi::ea(&models.mSuimenMunyaBtk))->entry(gabi::at<J3DModelData>(data), 0.0f);
    data = initModel(gabi::ea(&models.mpYuchw00Model), dRes_INDEX_LINK_BDL_YUCHW00_e, 0x13000222);
    res = lk_getLinkRes_l(0x10);
    if (!Player_initBckAnimation(gabi::ea(&models.mYuchw00Bck), data, res, 1, 2, 1.0f, 0, -1, 0)) {
        LK_ASSERT(0x5D85, "0");
    }
    initBtkAnm(data, gabi::ea(&models.mYuchw00Btk), 0x6D);
    initBrkAnm(data, gabi::ea(&models.mYuchw00Brk), 0x5B);
    lk_anmObjSetFrame(gabi::ea(&models.mYuchw00Brk), 0x10, 0x20, (f32)models.mYuchw00Brk.mFrameCtrl.getEnd() - 0.001f); /* frame max - 0.001 */
    data = initModel(gabi::ea(&models.mpYbafo00Model), dRes_INDEX_LINK_BDL_YBAFO00_e, 0x13000222);
    initBtkAnm(data, gabi::ea(&models.mYbafo00Btk), 0x6A);
    lk_anmObjSetFrame(gabi::ea(&models.mYbafo00Btk), 0x68, 0x10, (f32)models.mYbafo00Btk.mFrameCtrl.getEnd() - 0.001f);
    u32 mdata = lk_getLinkRes_l(0x48);
    if (mdata == 0) {
        LK_ASSERT(0x5D96, "tmp_modelData != (0)");
    }
    for (u32 i = 0; i < 6; i++) {
        u32 m = gabi::ea(mDoExt_J3DModel__create(gabi::at<J3DModelData>(mdata), 0x80000, 0x11001222));
        models.mMagicArmorAuraEntries[i].mpModel = gabi::at<J3DModel>(m);
        if (m == 0) {
            LK_ASSERT(0x5D9F, "aura_p->getModel() != (0)");
        }
    }
    initBtkAnm(mdata, gabi::ea(&models.mYaura00Btk), 0x69);
    res = lk_getLinkRes_l(0x58);
    if (!Player_initBrkAnimation(gabi::ea(&models.mYaura00rBrk), mdata, res, 0, 2, 1.0f, 0, -1, 0, 0)) {
        LK_ASSERT(0x5DA7, "0");
    }
    lk_anmObjSetFrame(gabi::ea(&models.mYaura00rBrk), 0x10, 0x20, 0.0f);
    data = initModel(gabi::ea(&models.mpYmgcs00Model), dRes_INDEX_LINK_BDL_YMGCS00_e, 0x11001222);
    initBtkAnm(data, gabi::ea(&models.mYmgcs00Btk), 0x6B);
    res = lk_getLinkRes_l(0x59);
    if (!Player_initBrkAnimation(gabi::ea(&models.mYmgcs00Brk), data, res, 0, 2, 1.0f, 0, -1, 0, 0)) {
        LK_ASSERT(0x5DB5, "0");
    }
    lk_anmObjSetFrame(gabi::ea(&models.mYmgcs00Brk), 0x10, 0x20, 0.0f);
    constexpr u32 anmLines[][2] = {{0x3C, 0x5DBD}, {0x42, 0x5DC4}, {0x37, 0x5DCC}, {0x38, 0x5DD3}, {0x40, 0x5DDA}, {0x41, 0x5DE1}};
    for (int k = 0; k < 6; k++) {
        mdata = lk_getLinkRes_l(anmLines[k][0]);
        if (mdata == 0) {
            LK_ASSERT(anmLines[k][1], "tmp_modelData != (0)");
        }
        switch (k) {
        case 0: initBrkAnm(mdata, self + 0x44D4, 0x51); break;
        case 1:
            initBrkAnm(mdata, self + 0x454C, 0x55);
            initBtkAnm(mdata, self + 0x45C4, 0x63);
            break;
        case 2: initBtkAnm(mdata, self + 0x46AC, 0x68); break;
        case 3: initBtkAnm(mdata, self + 0x4720, 0x65); break;
        case 4: initBtkAnm(mdata, self + 0x4638, 0x61); break;
        case 5: initBtkAnm(mdata, self + 0x4794, 0x62); break;
        }
    }
    /* m_old_fdata = new mDoExt_MtxCalcOldFrame(new J3DTransformInfo[0x2A], new Quaternion[0x2A]) (inline constructor) */
    u32 ti = Player_newArray(0x540);
    u32 q = Player_newArray(0x2A0);
    u32 od = gabi::ea(operator_new(0x24));
    if (od != 0) {
        auto* oldFrame = gabi::at<mDoExtAnimation::mDoExt_MtxCalcOldFrame>(od);
        oldFrame->mOldFrameStartJoint = 0;
        oldFrame->mOldFrameEndJoint = 0;
        oldFrame->field_0x1 = 1;
        oldFrame->mOldFrameFlg = 0;
        oldFrame->mOldFrameRate = 0.0f;
        oldFrame->mOldFrameMorfCounter = 0.0f;
        oldFrame->field_0x8 = 0.0f;
        oldFrame->field_0x10 = 0.0f;
        oldFrame->field_0x14 = 0.0f;
        oldFrame->mOldFrameTransInfo = ti;
        oldFrame->mOldFrameQuaternion = q;
    }
    m_old_fdata = od;
    if (od == 0) {
        LK_ASSERT(0x5DFE, "m_old_fdata != (0)");
    }
    /* HD: the anime ratio packs hold one ratio per joint (0x2A) */
    for (u32 k = 0; k < 2; k++) {
        reinterpret_cast<mDoExtAnimation::AnmRatio*>(mAnmRatioUnder)[k].mValueCount = 0x2A;
        reinterpret_cast<mDoExtAnimation::AnmRatio*>(mAnmRatioUnder)[k].mpValues = Player_newArray(0xA8);
    }
    for (u32 k = 0; k < 3; k++) {
        reinterpret_cast<mDoExtAnimation::AnmRatio*>(mAnmRatioUpper)[k].mValueCount = 0x2A;
        reinterpret_cast<mDoExtAnimation::AnmRatio*>(mAnmRatioUpper)[k].mpValues = Player_newArray(0xA8);
    }
    u32 pb = mDoExt_MtxCalcAnmBlendTblOld_constructCall(nullptr, gabi::at<void>(m_old_fdata), 2, mAnmRatioUnder, mpCLModel.get());
    m_pbCalc[0] = pb;
    if (pb == 0) {
        LK_ASSERT(0x5E0D, "m_pbCalc[PART_UNDER_e] != (0)");
    }
    pb = mDoExt_MtxCalcAnmBlendTblOld_constructCall(nullptr, gabi::at<void>(m_old_fdata), 3, mAnmRatioUpper, mpCLModel.get());
    m_pbCalc[1] = pb;
    if (pb == 0) {
        LK_ASSERT(0x5E14, "m_pbCalc[PART_UPPER_e] != (0)");
    }
    mDoExt_MtxCalcAnmBlendTblOld_bindModelCall(gabi::at<void>(m_pbCalc[0]), 0);
    mDoExt_MtxCalcAnmBlendTblOld_bindModelCall(gabi::at<void>(m_pbCalc[1]), 0);
    initTextureScroll();
    initTextureAnime();
    u32 hio = gabi::ea(operator_new(0x40)); /* HD: m_HIO is only allocated */
    m_HIO = hio;
    if (hio == 0) {
        LK_ASSERT(0x5E26, "m_HIO != (0)");
    }
    u32 b = operator_new_array_align(0xB400, 0x20);
    reinterpret_cast<daPy_anmHeap_c*>(m_anm_heap_under)[0].m_buffer = gabi::at<void>(b); /* m_anm_heap_under[UNDER_MOVE0_e].m_buffer */
    if (b == 0) {
        LK_ASSERT(0x5E2B, "m_anm_heap_under[UNDER_MOVE0_e].m_buffer != (0)");
    }
    b = operator_new_array_align(0x200, 0x20);
    *reinterpret_cast<gptr<void>*>(_658C) = gabi::at<void>(b); // HD: SE animation buffer follows the original sound buffer slot.
    if (b == 0) {
        LK_ASSERT(0x5E32, "m_sanm_buffer != (0)");
    }
    b = operator_new_array_align(0x1000, 0x20);
    models.m_item_bck_buffer = gabi::at<void>(b);
    if (b == 0) {
        LK_ASSERT(0x5E35, "m_item_bck_buffer != (0)");
    }
    gabi::Local<SafeString> agb;
    agb->__vtbl = 0x10034B24;
    agb->mStringTop = 0x10035208; /* "Agb" */
    u32 shadowData = Player_getArchiveResource( dComIfG_resControl(), agb.get(), 4);
    if (shadowData == 0) {
        LK_ASSERT(0x5E3C, "p_landing_model_data");
    }
    u32 shadow = gabi::ea(mDoExt_J3DModel__create(gabi::at<J3DModelData>(shadowData), 0, 0x11020203));
    *reinterpret_cast<gptr<J3DModel>*>(_438) = gabi::at<J3DModel>(shadow); /* HD: the shadow model (see drawShadow) */
    if (shadow == 0) {
        LK_ASSERT(0x5E3E, "m_landing_model");
    }
    models.mPacketList.mEnabled = 0;
    models.mPacketList.mCount = 0;
    /* HD: in the stage "GanonK" eleven HD objects (0xB0 each, 0x4BE8..) are set up and the packet list enabled */
    gabi::Local<SafeString> a;
    a->mStringTop = 0x10035210; /* "GanonK" */
    a->__vtbl = 0x10034B24;
    gabi::Local<SafeString> stage;
    u32 play = dComIfGp_ea();
    stage->__vtbl = 0x10034B24;
    stage->mStringTop = play + 0x5134; /* the stage name */
    SafeString_vf14_l(gabi::ea(a.get()));
    SafeString_vf14_l(gabi::ea(a.get()));
    u32 s = a->mStringTop;
    SafeString_vf14_l(gabi::ea(stage.get()));
    if (s == stage->mStringTop || lk_safeStrEq(a->mStringTop, stage->mStringTop)) {
        Player_initializeDrawPacket(&packets[0], 0);
        packets[0].field_0xad = 1;
        packets[0].field_0xaf = 1;
        Player_initializeDrawPacket(&packets[10], 0);
        packets[10].field_0xad = 1;
        packets[10].field_0xae = 1;
        for (u32 i = 1; i <= 9; i++) {
            Player_initializeDrawPacket(&packets[i], 0);
        }
        models.mPacketList.mEnabled = 1;
    }
#undef LK_ASSERT
    return TRUE;
}

/* ======== draw ======== */
/* HD J3DModel draw flags (+0x74) and their update (027F596C, the new flags passed in r4) */
static inline void lk_mdlFlags(u32 m, u32 v) {
    gabi::at<PlayerModelDrawState>(m)->mDrawFlags = v;
    gabi::call(0x027F596C, m, v);
}
/* the HD two-step flag update of a model field: clear the low three flags, then set flag 1 */
static inline void lk_mdlFlags2(gptr<J3DModel>& slot) {
    u32 m = gabi::ea(slot.get());
    lk_mdlFlags(m, (u32)gabi::at<PlayerModelDrawState>(m)->mDrawFlags & ~7u);
    m = gabi::ea(slot.get());
    lk_mdlFlags(m, (u32)gabi::at<PlayerModelDrawState>(m)->mDrawFlags | 1);
}
/* name (sead::SafeString built from the self-relative name at *(p) + 4) equals the global SafeString g (0x40001 compare) */
static inline bool lk_nameIsGlobal(u32 nameOwner, u32 g, u32 local) {
    gabi::store<u32>(local + 0, lk_relPtr(gabi::load<u32>(nameOwner) + 4));
    gabi::store<u32>(local + 4, 0x10034B24);
    SafeString_vf14_l(g);
    SafeString_vf14_l(g);
    u32 a = gabi::load<u32>(g);
    SafeString_vf14_l(local);
    return a == gabi::load<u32>(local) || lk_safeStrEq(gabi::load<u32>(g), gabi::load<u32>(local));
}
/* key (a literal) vs name, with 02444F48 called on the first of the two (as in setBowModel) */
static inline bool lk_keyEq(u32 first, u32 second) {
    gabi::call(0x02444F48, first);
    SafeString_vf14_l(first);
    u32 a = gabi::load<u32>(first);
    SafeString_vf14_l(second);
    return a == gabi::load<u32>(second) || lk_safeStrEq(gabi::load<u32>(first), gabi::load<u32>(second));
}
static inline void lk_shapeShow(u32 node, u8 v) { gabi::at<PlayerJointNodeView>(node)->mMesh->mShape->mVisible = v; }
static inline u32 lk_camAttnStatus(daPy_lk_c* lk, u32 idxAddr) {
    s32 idx = gabi::load<s32>(idxAddr);
    return gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5B00);
}
#define LK_J3DSYS 0x104B45C0u /* j3dSys */
/* a function-local static J3D packet (HD: constructed on first use, then entered into the opa list) */
static inline void lk_staticPacketEntry(u32 pkt, u32 guard, u32 vtbl, u32 desc) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        gabi::call(0x027F1278 /* J3DPacket constructor */, pkt);
        gabi::store<u32>(pkt + 0xC, vtbl);
        __register_global_object(desc);
    }
    Player_drawBufferEntryImmediate(gabi::ea(j3dSysState.mpOpaDrawBuffer.get()), pkt, 0);
}
static inline void lk_setList(u32 opa, u32 xlu) {
    j3dSysState.mpOpaDrawBuffer = gabi::at<J3DDrawBuffer>(gabi::load<u32>(dComIfGp_ea() + opa));
    j3dSysState.mpXluDrawBuffer = gabi::at<J3DDrawBuffer>(gabi::load<u32>(dComIfGp_ea() + xlu));
}
/* mpEquipItemModel->setAnmMtx(HOOKSHOT_JNT_HSHOTTIP_e, hookshot->getMtxTop()) */
static inline void lk_setHookshotTipMtx(u32 self) {
    u32 hs = gabi::load<u32>(self + 0x6594);
    if (hs != 0) {
        u32 blk = gabi::load<u32>(gabi::load<u32>(self + 0x4440) + 0x2C);
        gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
        mtx_copy_q(gabi::load<u32>(blk + 0x10) + 0xC0, hs + 0xD778);
    }
}

/* draw (the matcher names it drawShadow) */
/* 023D9820 */
BOOL daPy_lk_c::draw() {
    PlayerModelState& models = *reinterpret_cast<PlayerModelState*>(mD90);
    auto* packets = reinterpret_cast<PlayerDrawPacket*>(models._effects);
    u32 self = gabi::ea(this);
    u32 tev = self + 0x110;
#define LK_freeze() (noResetFlg1() & 0x800) /* checkFreezeState() */
    packets[10].mEnabled = 0;
    u8 drawFlg = (u8)reinterpret_cast<daPy_sightPacket_c*>(mSightPacket)->mDrawFlag;
    packets[0].mEnabled = 0;
    if (drawFlg) {
        daPy_sightPacket_setSight(gabi::ea(mSightPacket));
    }
    settingTevStruct(dKy_getEnvlight(), 9 /* TEV_TYPE_PLAYER */, &current.pos, gabi::at<dKy_tevstr_c>(tev));
    if (gabi::call_ptr<BOOL>(gabi::at<PlayerActorDispatch>(self)->mpVtable->mCheckPlayerNoDraw, this) /* checkPlayerNoDraw() */) {
        offBodyEffect();
        drawMirrorLightModel();
        /* HD: Link is still drawn (without effects) in some states */
        BOOL st1 = (dComIfGp_getPlayerStatus1(0) & 0x100000) != 0;
        u8 st0 = 0;
        if ((dComIfGp_getPlayerStatus0(0) & 0x08000000) && (dComIfGp_getPlayerStatus0(0) & 0x2000)) {
            st0 = 1;
        }
        if ((mCurProc == 0 || st0) && !st1) {
            return TRUE;
        }
        gabi::Local<SafeString> nm;
        for (u32 mtl = gabi::ea(gabi::at<PlayerJointNodeView>(mpCLModelData->mJointNodes)->mMesh.get()); mtl != 0; mtl = gabi::ea(gabi::at<PlayerMaterialAnimationView>(mtl)->mNext.get())) {
            u8 v = 1;
            if (lk_nameIsGlobal(mtl, gabi::ea(&ear3MaterialName), gabi::ea(nm.get())) && (checkCaughtShapeHide() || (noResetFlg1() & 8))) {
                v = 0;
            }
            gabi::at<PlayerMaterialAnimationView>(mtl)->mShape->mVisible = v;
        }
        if (noResetFlg1() & 8) {
            lk_shapeShow(lk_jointNode(gabi::ea(mpCLModelData.get()), 0x29), 0);
        }
        setDrawHandModel();
        u32 d = gabi::ea(mpCLModelData.get());
        u32 lshape = gabi::ea(gabi::at<PlayerJointNodeView>(lk_jointNode(d, 8))->mMesh->mShape.get());
        u32 rshape = gabi::ea(gabi::at<PlayerJointNodeView>(lk_jointNode(d, 0xC))->mMesh->mShape.get());
        if (lshape != mpLhandShape) {
            lk_shapeShow(lk_jointNode(d, 8), 0);
        }
        if (rshape != mpRhandShape) {
            lk_shapeShow(lk_jointNode(gabi::ea(mpCLModelData.get()), 0xC), 0);
        }
        lk_mdlFlags2(mpCLModel);
        reinterpret_cast<PlayerModelTextureControllers*>(mpCLModelData.get())->mpTexturePatterns[0] = nullptr;
        reinterpret_cast<PlayerModelTextureControllers*>(mpCLModelData.get())->mpTexturePatterns[1] = nullptr;
        reinterpret_cast<PlayerModelTextureControllers*>(mpCLModelData.get())->mpTextureScroll = nullptr;
        mDoExt_modelEntryDL(mpCLModel);
        lk_mdlFlags2(mpHandsModel);
        entryDLSetLight(gabi::at<J3DModel>(gabi::ea(mpHandsModel.get())), noResetFlg1() & 0x800);
        if ((noResetFlg1() & 8) && !checkCaughtShapeHide() && !(lk_camAttnStatus(this, self + 0x69BC) & 0x20)) {
            lk_mdlFlags2(mpKatsuraModel);
            entryDLSetLight(gabi::at<J3DModel>(gabi::ea(mpKatsuraModel.get())), noResetFlg1() & 0x800);
        }
        u8 sw = dComIfGs_getSelectEquip(0);
        if ((sw == 0x39 || sw == 0x3A || sw == 0x3E) /* checkMasterSwordEquip() */ && !checkCaughtShapeHide() && !checkDemoShieldNoDraw()) {
            lk_mdlFlags2(models.mpPodmsModel);
            updateDLSetLight(models.mpPodmsModel.get(), noResetFlg1() & 0x800);
        }
        if ((noResetFlg1() & 0x800) && checkMaskDraw()) {
            lk_mdlFlags2(mpYamuModel);
            entryDLSetLight(gabi::at<J3DModel>(gabi::ea(mpYamuModel.get())), noResetFlg1() & 0x800);
        }
        if (dComIfGs_getSelectEquip(2) == 0x28 /* checkPowerGloveEquip() */) {
            lk_mdlFlags2(models.mpPringModel);
            entryDLSetLight(models.mpPringModel.get(), noResetFlg1() & 0x800);
        }
        if (mNoResetFlg0 & 0x2000000 /* checkEquipHeavyBoots() */) {
            lk_mdlFlags2(models.mpHbootsModels[0]);
            lk_mdlFlags2(models.mpHbootsModels[1]);
            entryDLSetLight(models.mpHbootsModels[0].get(), noResetFlg1() & 0x800);
            entryDLSetLight(models.mpHbootsModels[1].get(), noResetFlg1() & 0x800);
        }
        if (!(lk_camAttnStatus(this, self + 0x69BC) & 0x20)) {
            if ((dComIfGs_getSelectEquip(0) != 0xFF || dComIfGp_getMiniGameType() == 2) /* checkSwordEquip() */ &&
                !checkDemoSwordNoDraw(1)) {
                lk_mdlFlags2(mpEquippedSwordModel);
                entryDLSetLight(gabi::at<J3DModel>(gabi::ea(mpEquippedSwordModel.get())), noResetFlg1() & 0x800);
            }
        }
        if (dComIfGs_getSelectEquip(1) != 0xFF /* checkShieldEquip() */ && !checkCaughtShapeHide() && !checkDemoShieldNoDraw()) {
            lk_mdlFlags2(models.mpEquippedShieldModel);
            entryDLSetLight(models.mpEquippedShieldModel.get(), noResetFlg1() & 0x800);
        }
        u32 bc = gabi::ea(models.mpBottleContentsModel.get());
        if (bc != 0) {
            gabi::Local<SafeString> n1;
            gabi::Local<SafeString> k1;
            n1->__vtbl = 0x10034B24;
            n1->mStringTop = lk_relPtr((u32)gabi::at<PlayerModelNameView>(bc)->mpNameData + 4); /* the contents model's name */
            k1->__vtbl = 0x10034B24;
            k1->mStringTop = 0x100353C4; /* "binho" */
            if (!lk_keyEq(gabi::ea(n1.get()), gabi::ea(k1.get()))) {
                lk_mdlFlags2(models.mpBottleContentsModel);
                updateDLSetLight(gabi::at<J3DModel>(gabi::ea(models.mpBottleContentsModel.get())), 0);
            }
        }
        if (gabi::ea(models.mpEquipItemModel.get()) != 0 && !checkCaughtShapeHide() && !checkDemoSwordNoDraw(0)) {
            u16 item = mEquipItem;
            bool skip = false;
            if (checkBowItem(item)) {
                skip = gabi::call_ptr<BOOL>(gabi::at<PlayerActorDispatch>(self)->mpVtable->mCheckPlayerGuard, this) != 0; /* checkPlayerGuard() */
                item = mEquipItem;
            }
            if (!skip) {
                if (item == dItemNo_HOOKSHOT_e) {
                    lk_setHookshotTipMtx(self);
                }
                lk_mdlFlags2(models.mpEquipItemModel);
                entryDLSetLight(models.mpEquipItemModel.get(), noResetFlg1() & 0x800);
            }
        }
        if (gabi::ea(models.mpBottleCapModel.get()) != 0 && m355E != 0) {
            lk_mdlFlags2(models.mpBottleCapModel);
            updateDLSetLight(gabi::at<J3DModel>(gabi::ea(models.mpBottleCapModel.get())), 0);
        }
        return TRUE;
    }

    gptr<J3DModel>* mdls[] = {&mpCLModel, &mpHandsModel, &mpKatsuraModel, &models.mpPodmsModel, &mpYamuModel, &models.mpPringModel};
    for (auto* slot : mdls) {
        u32 m = gabi::ea(slot->get());
        if (m != 0) {
            lk_mdlFlags(m, (u32)gabi::at<PlayerModelDrawState>(m)->mDrawFlags | 7);
        }
    }
    for (u32 i = 0; i < 2; i++) { /* mpHbootsModels[2] (no NULL check) */
        u32 m = gabi::ea(models.mpHbootsModels[i].get());
        lk_mdlFlags(m, (u32)gabi::at<PlayerModelDrawState>(m)->mDrawFlags | 7);
    }
    gptr<J3DModel>* mdls2[] = {&mpEquippedSwordModel, &models.mpEquippedShieldModel, &models.mpBottleContentsModel, &models.mpEquipItemModel, &models.mpBottleCapModel};
    for (auto* slot : mdls2) {
        u32 m = gabi::ea(slot->get());
        if (m != 0) {
            lk_mdlFlags(m, (u32)gabi::at<PlayerModelDrawState>(m)->mDrawFlags | 7);
        }
    }
    if (lk_camAttnStatus(this, self + 0x69BC) & 0x20 /* dCamAttnStts_00000020_e */) {
        offBodyEffect();
    } else {
        onBodyEffect();
    }
    /* GXColorS10 origFog, origFogStartZ, origFogEndZ */
    s16 fogG = (s16)tevStr.mFogColor.g;
    f32 fogEnd = (f32)tevStr.mFogEndZ;
    s16 fogB = (s16)tevStr.mFogColor.b;
    u32 nf1 = noResetFlg1();
    s16 fogR = (s16)tevStr.mFogColor.r;
    f32 fogStart = (f32)tevStr.mFogStartZ;
    if (!(nf1 & 0x800) && mCurProc != daPyProc_ELEC_DAMAGE_e && !(dComIfGp_getPlayerStatus1(0) & 0x100000) &&
        (damageEffectType == 1 /* checkCurse() */ || (noResetFlg1() & 0x100) /* checkConfuse() */ || mDamageWaitTimer > 0)) {
        gabi::Local<cXyz> sp18;
        Player_posToCamera(&current.pos, sp18.get());
        u32 cnt = (u32)g_Counter.mTimer; /* g_Counter.mTimer */
        f32 f2 = std::fabs(cM_ssin((s32)(cnt << 11))); /* std::abs(cM_ssin(g_Counter.mTimer * 0x800)) */
        if (damageEffectType == 1 || (noResetFlg1() & 0x100)) {
            tevStr.mFogColor.r = 0x80;
            f32 start = gabi::fmadds(200.0f, f2, -sp18->z - 200.0f);
            tevStr.mFogColor.g = 0;
            tevStr.mFogColor.b = 0xFF;
            tevStr.mFogStartZ = start;
            tevStr.mFogEndZ = start + 300.0f;
        } else {
            tevStr.mFogColor.r = 0xFF;
            tevStr.mFogColor.g = 0x3C;
            f32 start = gabi::fmadds(200.0f, f2, -sp18->z - 200.0f);
            tevStr.mFogColor.b = 0x3C;
            tevStr.mFogStartZ = start;
            tevStr.mFogEndZ = start + 300.0f;
        }
    }
    /* HD: the eye/mouth animations are the HD objects at 0x644 / 0x69C / 0x6F4, attached to the model data */
    auto* textureControllers = reinterpret_cast<PlayerTextureController*>(m644);
    Player_prepareTexturePattern(&textureControllers[0]);
    reinterpret_cast<PlayerModelTextureControllers*>(mpCLModelData.get())->mpTexturePatterns[0] = &textureControllers[0];
    Player_prepareTexturePattern(&textureControllers[1]);
    reinterpret_cast<PlayerModelTextureControllers*>(mpCLModelData.get())->mpTexturePatterns[1] = &textureControllers[1];
    Player_prepareTextureScroll(&textureControllers[2]);
    reinterpret_cast<PlayerModelTextureControllers*>(mpCLModelData.get())->mpTextureScroll = &textureControllers[2];
    gabi::at<PlayerAnimationFrame>(mpAnmTexPatternData)->mFrame = (f32)(u16)m3530; /* mpAnmTexPatternData->setFrame(m3530) */
    gabi::at<PlayerAnimationFrame>(mpTexScrollResData)->mFrame = (f32)(u16)m3532; /* mpTexScrollResData->setFrame(m3532) */
    for (int k = 0; k < 3; k++) {
        PlayerTextureFrameControl* control = textureControllers[k].mpFrameControl;
        f32 fr = (f32)(u16)(k < 2 ? m3530 : m3532);
        f64 r = gabi::call_ptr<f64>(control->mCalculateFrame, (u32)control->mContext, fr, (f32)control->mStartFrame, (f32)control->mEndFrame);
        control->mFrame = (f32)r;
    }
    setLightTevColorType(dKy_getEnvlight(), mpCLModel, gabi::at<dKy_tevstr_c>(tev));
    u32 d = gabi::ea(mpCLModelData.get());
    u32 rootNode = gabi::at<J3DModelData>(d)->mJointNodes; /* mpCLModelData->getJointNodePointer(CL_JNT_LINK_ROOT_e) */
    lk_shapeShow(lk_jointNode(d, CL_JNT_CL_LHANDA_e), 0);
    lk_shapeShow(lk_jointNode(gabi::ea(mpCLModelData.get()), CL_JNT_CL_RHANDA_e), 0);
    {
        // HD: the ear_3_ material is found by name.
        d = gabi::ea(mpCLModelData.get());
        u32 x = gabi::at<J3DModelData>(d)->mpResHeader;
        SafeString_vf14_l(gabi::ea(&ear3MaterialName));
        s32 idx = (s32)Player_materialNameIndex( lk_relPtr(x + 0x18), (u32)ear3MaterialName.mStringTop);
        u32 mat = 0;
        if (idx >= 0) {
            mat = gabi::at<J3DModelData>(d)->mMaterials;
            if ((u32)idx < (u32)gabi::at<J3DModelData>(d)->mMaterialNum) {
                mat += idx * 0x39C;
            }
        }
        gabi::at<PlayerMaterialAnimationView>(mat)->mShape->mVisible = 1; /* show */
    }
    u32 eye = lk_jointNode(gabi::ea(mpCLModelData.get()), CL_JNT_CL_EYE_e);
    u32 mayu = lk_jointNode(gabi::ea(mpCLModelData.get()), CL_JNT_CL_MAYU_e);
    setDrawHandModel();
    j3dSysState.mpModel = mpCLModel.get(); /* j3dSys.setModel(mpCLModel) */
    BOOL r24 = !(field_0x2b0 > -85.0f); /* field_0x2b0 <= -85.0f */
    gabi::Local<SafeString> s1, s2;
    if (r24) {
        for (u32 i = 0; i < 4; i++) {
            gabi::at<J3DShape>(mpZOffBlendShape[i])->hide(); /* mpZOffBlendShape[i]->hide() */
            gabi::at<J3DShape>(mpZOffNoneShape[i])->hide(); /* mpZOffNoneShape[i]->hide() */
            gabi::at<J3DShape>(mpZOnShape[i])->hide(); /* mpZOnShape[i]->hide() */
        }
        lk_shapeShow(lk_jointNode(gabi::ea(mpCLModelData.get()), 8), 0);
        lk_shapeShow(lk_jointNode(gabi::ea(mpCLModelData.get()), 0xC), 0);
        // HD: only the ear_4_ material stays visible.
        for (u32 mtl = gabi::ea(gabi::at<PlayerJointNodeView>(rootNode)->mMesh.get()); mtl != 0; mtl = gabi::ea(gabi::at<PlayerMaterialAnimationView>(mtl)->mNext.get())) {
            u8 v = lk_nameIsGlobal(mtl, gabi::ea(&ear4MaterialName), gabi::ea(s1.get())) ? 1 : 0;
            gabi::at<PlayerMaterialAnimationView>(mtl)->mShape->mVisible = v;
        }
    } else if (lk_camAttnStatus(this, self + 0x69BC) & 0x20) {
        for (u32 i = 0; i < 4; i++) {
            gabi::at<J3DShape>(mpZOffBlendShape[i])->hide();
            gabi::at<J3DShape>(mpZOffNoneShape[i])->hide();
            gabi::at<J3DShape>(mpZOnShape[i])->hide();
        }
        /* HD: the sleeve and ear_4_ materials stay visible; the others are drawn without
         * colour/alpha updates (material flags +0x30) */
        for (u32 mtl = gabi::ea(gabi::at<PlayerJointNodeView>(rootNode)->mMesh.get()); mtl != 0; mtl = gabi::ea(gabi::at<PlayerMaterialAnimationView>(mtl)->mNext.get())) {
            bool show = lk_nameIsGlobal(mtl, gabi::ea(&sleeveMaterialName), gabi::ea(s1.get())) || lk_nameIsGlobal(mtl, gabi::ea(&ear4MaterialName), gabi::ea(s2.get()));
            u32 cl = gabi::ea(mpCLModel.get());
            if (show) {
                gabi::at<PlayerMaterialAnimationView>(mtl)->mShape->mVisible = 1;
                cl = gabi::ea(mpCLModel.get());
            }
            u16 mi = (u16)gabi::at<PlayerMaterialResource>(gabi::at<PlayerMaterialAnimationView>(mtl)->mpData)->mIndex;
            u32 m = 0;
            if (mi < (u32)gabi::at<PlayerModelDrawState>(cl)->mMaterialNum) {
                m = gabi::ea(gabi::at<PlayerModelDrawState>(cl)->mpMaterials.get()) + mi * 0x38;
            }
            if (show) {
                gabi::at<PlayerRuntimeMaterial>(m)->mFlags = (u32)gabi::at<PlayerRuntimeMaterial>(m)->mFlags | 7;
            } else {
                gabi::at<PlayerRuntimeMaterial>(m)->mFlags = (u32)gabi::at<PlayerRuntimeMaterial>(m)->mFlags & ~7u;
                cl = gabi::ea(mpCLModel.get());
                mi = (u16)gabi::at<PlayerMaterialResource>(gabi::at<PlayerMaterialAnimationView>(mtl)->mpData)->mIndex;
                m = 0;
                if (mi < (u32)gabi::at<PlayerModelDrawState>(cl)->mMaterialNum) {
                    m = gabi::ea(gabi::at<PlayerModelDrawState>(cl)->mpMaterials.get()) + mi * 0x38;
                }
                gabi::at<PlayerRuntimeMaterial>(m)->mFlags = (u32)gabi::at<PlayerRuntimeMaterial>(m)->mFlags | 1;
            }
        }
        lk_shapeShow(lk_jointNode(gabi::ea(mpCLModelData.get()), CL_JNT_CL_HANA_e), 0);
    } else {
        u32 cl = gabi::ea(mpCLModel.get());
        lk_mdlFlags(cl, (u32)gabi::at<PlayerModelDrawState>(cl)->mDrawFlags | 7);
        if (!(noResetFlg1() & 0x800)) {
            lk_setList(0x5D54, 0x5D54); /* dComIfGd_setListP0() */
            lk_staticPacketEntry(0x1046CE20, 0x1046D088, 0x10058D40, 0x101CEE64); /* l_onCupOffAupPacket2.entryOpa() */
            for (u32 i = 0; i < 4; i++) {
                gabi::at<J3DShape>(mpZOffBlendShape[i])->hide();
                gabi::at<J3DShape>(mpZOnShape[i])->hide();
                gabi::at<J3DShape>(mpZOffNoneShape[i])->show();
            }
            gabi::at<J3DJoint>(eye)->entryIn(mpCLModel.get());
            gabi::at<J3DJoint>(mayu)->entryIn(mpCLModel.get());
            lk_staticPacketEntry(0x1046CEB8, 0x1046D08C, 0x10058D10, 0x101CEE70); /* l_offCupOnAupPacket2.entryOpa() */
            for (u32 i = 0; i < 4; i++) {
                gabi::at<J3DShape>(mpZOffBlendShape[i])->show();
                gabi::at<J3DShape>(mpZOffNoneShape[i])->hide();
            }
            gabi::at<J3DJoint>(eye)->entryIn(mpCLModel.get());
            gabi::at<J3DJoint>(mayu)->entryIn(mpCLModel.get());
            /* HD: the face (gabi::ea(&faceMaterialName)) and hair (gabi::ea(&ear2MaterialName)) materials stay visible */
            for (u32 mtl = gabi::ea(gabi::at<PlayerJointNodeView>(rootNode)->mMesh.get()); mtl != 0; mtl = gabi::ea(gabi::at<PlayerMaterialAnimationView>(mtl)->mNext.get())) {
                if (!lk_nameIsGlobal(mtl, gabi::ea(&faceMaterialName), gabi::ea(s1.get())) && !lk_nameIsGlobal(mtl, gabi::ea(&ear2MaterialName), gabi::ea(s2.get()))) {
                    gabi::at<PlayerMaterialAnimationView>(mtl)->mShape->mVisible = 0;
                }
            }
            gabi::at<J3DJoint>(rootNode)->entryIn(mpCLModel.get()); /* link_root_joint->entryIn() */
            if (checkMaskDraw()) {
                entryDLSetLight(gabi::at<J3DModel>(gabi::ea(mpYamuModel.get())), noResetFlg1() & 0x800);
            }
            j3dSysState.mpModel = mpCLModel.get();
            hideHatAndBackle(gabi::ea(gabi::at<PlayerJointNodeView>(rootNode)->mMesh.get()));
            lk_setList(0x5D5C, 0x5D5C);
            lk_staticPacketEntry(0x1046CF50, 0x1046D090, 0x10058D40, 0x101CEE7C); /* l_onCupOffAupPacket1.entryOpa() */
            for (u32 i = 0; i < 4; i++) {
                gabi::at<J3DShape>(mpZOffBlendShape[i])->hide();
                gabi::at<J3DShape>(mpZOnShape[i])->show();
                gabi::at<J3DShape>(mpZOffNoneShape[i])->hide();
            }
            gabi::at<J3DJoint>(eye)->entryIn(mpCLModel.get());
            gabi::at<J3DJoint>(mayu)->entryIn(mpCLModel.get());
            lk_staticPacketEntry(0x1046CFE8, 0x1046D094, 0x10058D10, 0x101CEE88); /* l_offCupOnAupPacket1.entryOpa() */
            for (u32 i = 0; i < 4; i++) {
                gabi::at<J3DShape>(mpZOnShape[i])->hide();
            }
            u32 clm = gabi::ea(mpCLModel.get());
            packets[0].mEnabled = 1; /* HD packets */
            packets[10].mpModel = clm;
            packets[10].mEnabled = 1;
            packets[0].mpModel = clm;
        } else {
            hideHatAndBackle(gabi::ea(gabi::at<PlayerJointNodeView>(rootNode)->mMesh.get()));
        }
        bool hide;
        if (dComIfGs_getSelectEquip(0) == 0x38 || dComIfGp_getMiniGameType() == 2) { /* checkNormalSwordEquip() */
            hide = false;
        } else {
            auto* info = static_cast<PlayerStageInfo*>(dComIfGp_getStageStagInfo());
            hide = (((u32)info->mStageFlags >> 16) & 7) != dStageType_FF1_e;
        }
        if (!hide) {
            hide = checkCaughtShapeHide() || checkDemoShieldNoDraw();
        }
        if (hide && (u16)mDemo.mDemoType != 0) {
            /* HD: during the demo "Demo50" the back scabbard stays visible */
            gabi::Local<SafeString> k1;
            gabi::Local<SafeString> k2;
            k1->__vtbl = 0x10034B24;
            k2->__vtbl = 0x10034B24;
            k2->mStringTop = 0x1047E6B8; /* the current demo name */
            k1->mStringTop = 0x100353CC; /* "Demo50" */
            if (lk_keyEq(gabi::ea(k1.get()), gabi::ea(k2.get()))) {
                hide = false;
            }
        }
        lk_shapeShow(lk_jointNode(gabi::ea(mpCLModelData.get()), CL_JNT_CL_PODA_e), hide ? 0 : 1);
    }
    lk_setList(0x5D58, 0x5D60); /* dComIfGd_setListP1() */
    {
        auto* drawList = reinterpret_cast<dDlstHD::dDlst_list_c*>(&gabi::at<dComIfG_inf_c>(dComIfGp_ea())->drawlist);
        // HD: draw-list setup receives Link's position in its three leading words.
        fcpy_l(gabi::ea(&drawList->mXOrig), gabi::ea(&current.pos.x));
        fcpy_l(gabi::ea(&drawList->mYOrig), gabi::ea(&current.pos.y));
        fcpy_l(gabi::ea(&drawList->mWidth), gabi::ea(&current.pos.z));
        Player_setDrawListPosition(gabi::ea(drawList)); /* HD: the draw list gets Link's position */
    }
    if (noResetFlg1() & 0x800) {
        dMat_control_c::iceEntryDL(mpCLModel.get(), -1, 0);
    } else {
        mDoExt_modelEntryDL(mpCLModel);
    }
    for (u32 mtl = gabi::ea(gabi::at<PlayerJointNodeView>(rootNode)->mMesh.get()); mtl != 0; mtl = gabi::ea(gabi::at<PlayerMaterialAnimationView>(mtl)->mNext.get())) {
        gabi::at<PlayerMaterialAnimationView>(mtl)->mShape->mVisible = 1;
    }
    lk_shapeShow(lk_jointNode(gabi::ea(mpCLModelData.get()), CL_JNT_CL_HANA_e), 1);
    lk_shapeShow(lk_jointNode(gabi::ea(mpCLModelData.get()), CL_JNT_CL_BACK_e), 1);
    if (!r24) {
        entryDLSetLight(gabi::at<J3DModel>(gabi::ea(mpHandsModel.get())), noResetFlg1() & 0x800);
        entryHDPacket(self + 0x4D48, gabi::ea(mpHandsModel.get()));
        u32 f = noResetFlg1();
        if ((f & 8) && !checkCaughtShapeHide()) {
            if (!(lk_camAttnStatus(this, self + 0x69BC) & 0x20)) {
                entryDLSetLight(gabi::at<J3DModel>(gabi::ea(mpKatsuraModel.get())), noResetFlg1() & 0x800);
                entryHDPacket(self + 0x4C98, gabi::ea(mpKatsuraModel.get()));
            }
            f = noResetFlg1();
        }
        if ((f & 0x800) && checkMaskDraw()) {
            entryDLSetLight(gabi::at<J3DModel>(gabi::ea(mpYamuModel.get())), noResetFlg1() & 0x800);
        }
        if (dComIfGs_getSelectEquip(2) == 0x28 /* checkPowerGloveEquip() */) {
            entryDLSetLight(models.mpPringModel.get(), noResetFlg1() & 0x800);
            entryHDPacket(self + 0x5168, gabi::ea(models.mpPringModel.get()));
        }
        u8 sw = dComIfGs_getSelectEquip(0);
        if ((sw == 0x39 || sw == 0x3A || sw == 0x3E) && !checkCaughtShapeHide() && !checkDemoShieldNoDraw()) {
            bool draw = true;
            if ((u16)mDemo.mDemoType != 0) {
                gabi::Local<SafeString> k1;
                gabi::Local<SafeString> k2;
                k1->__vtbl = 0x10034B24;
                k2->__vtbl = 0x10034B24;
                k2->mStringTop = 0x1047E6B8;
                k1->mStringTop = 0x100353CC; /* "Demo50" */
                draw = !lk_keyEq(gabi::ea(k1.get()), gabi::ea(k2.get()));
            }
            if (draw) {
                updateDLSetLight(models.mpPodmsModel.get(), noResetFlg1() & 0x800); /* mpPodmsModel */
                entryHDPacket(self + 0x4EA8, gabi::ea(models.mpPodmsModel.get()));
            }
        }
    }
    if (mNoResetFlg0 & 0x2000000 /* checkEquipHeavyBoots() */) {
        entryDLSetLight(models.mpHbootsModels[0].get(), noResetFlg1() & 0x800);
        entryDLSetLight(models.mpHbootsModels[1].get(), noResetFlg1() & 0x800);
        entryHDPacket(self + 0x5008, gabi::ea(models.mpHbootsModels[0].get()));
        entryHDPacket(self + 0x50B8, gabi::ea(models.mpHbootsModels[1].get()));
    }
    tevStr.mFogStartZ = fogStart;
    tevStr.mFogColor.r = fogR;
    tevStr.mFogEndZ = fogEnd;
    tevStr.mFogColor.g = fogG;
    tevStr.mFogColor.b = fogB;
    if (!r24) {
        s32 proc = mCurProc;
        if (proc == daPyProc_CUT_F_e || proc == daPyProc_BT_VERTICAL_JUMP_CUT_e) {
            updateDLSetLight(gabi::at<J3DModel>(gabi::ea(models.mpSwordTipStabModel.get())) /* mpSwordTipStabModel */, 0);
        } else if ((mModeFlg & 0x40000) && (mNoResetFlg0 & 0x100) && !(proc == daPyProc_DEMO_DEAD_e && mProcVar3 == 0)) {
            gabi::Local<GXColor> seaColor;
            gabi::Local<GXColor> seaColorSecondary;
            Player_getSeaColor(seaColor.get(), seaColorSecondary.get());
            gabi::Local<GXColorS10> tevColor;
            u32 c10 = gabi::ea(tevColor.get());
            tevColor->b = (u8)seaColor->b;
            tevColor->g = (u8)seaColor->g;
            tevColor->r = (u8)seaColor->r;
            u32 x = PlayerModelData_getResource(gabi::ea(gabi::at<J3DModel>(gabi::ea(models.mpSuimenMunyaModel.get()))->getModelData())); /* mpSuimenMunyaModel */
            u16 num = (u16)gabi::at<PlayerModelResource>(x)->mMaterialNum;
            gabi::Local<PlayerColorF4> f4;
            gabi::Local<cXyz> rgb;
            u32 matID = num - 1;
            for (u32 n = num; n != 0; n--, matID--) {
                u32 md = gabi::ea(gabi::at<J3DModel>(gabi::ea(models.mpSuimenMunyaModel.get()))->getModelData());
                u32 mat = gabi::at<J3DModelData>(md)->mMaterials;
                if ((u16)matID < (u32)gabi::at<J3DModelData>(md)->mMaterialNum) {
                    mat += (u16)matID * 0x39C;
                }
                u32 tevb = gabi::at<PlayerMaterialAnimationView>(mat)->mpTevBlock;
                gabi::call_ptr(gabi::at<PlayerSurfaceTevBlock>(tevb)->mpVtable->mSetTevColor, tevb, 0, c10); /* setTevColor(0, &sp10) */
                /* HD: the colour also goes to the material's HD colour block (index 4) */
                f32 r = (f32)(s16)tevColor->r / 255.0f;
                f32 g = (f32)(s16)tevColor->g / 255.0f;
                f32 b = (f32)(s16)tevColor->b / 255.0f;
                f32 a = (f32)(s16)tevColor->a / 255.0f;
                f4->r = r;
                f4->g = g;
                f4->b = b;
                f4->a = a;
                Player_convertColorToRGB(rgb.get(), f4.get(), 1.0f);
                gabi::at<PlayerMaterialAnimationView>(mat)->mParameterFlags = (u32)gabi::at<PlayerMaterialAnimationView>(mat)->mParameterFlags | 0x10;
                u32 dst = Player_materialParameter(mat + 0xA0, 4);
                f32 a2 = (f32)(s16)tevColor->a / 255.0f;
                gabi::at<PlayerColorF4>(dst)->r = rgb->x;
                gabi::at<PlayerColorF4>(dst)->g = rgb->y;
                gabi::at<PlayerColorF4>(dst)->b = rgb->z;
                gabi::at<PlayerColorF4>(dst)->a = a2;
            }
            mDoExt_modelUpdateDL(gabi::at<J3DModel>(gabi::ea(models.mpSuimenMunyaModel.get())), 0);
        }
        if (!(lk_camAttnStatus(this, self + 0x69BC) & 0x20)) {
            if ((dComIfGs_getSelectEquip(0) != 0xFF || dComIfGp_getMiniGameType() == 2) && !checkDemoSwordNoDraw(1)) {
                entryDLSetLight(gabi::at<J3DModel>(gabi::ea(mpEquippedSwordModel.get())), noResetFlg1() & 0x800); /* mpEquippedSwordModel */
                entryHDPacket(self + 0x4DF8, gabi::ea(mpEquippedSwordModel.get()));
            }
        }
        if (dComIfGs_getSelectEquip(1) != 0xFF && !checkCaughtShapeHide() && !checkDemoShieldNoDraw()) {
            entryDLSetLight(models.mpEquippedShieldModel.get(), noResetFlg1() & 0x800); /* mpEquippedShieldModel */
            entryHDPacket(self + 0x4F58, gabi::ea(models.mpEquippedShieldModel.get()));
        }
        lk_setList(0x5D78, 0x5D7C); /* dComIfGd_setList() */
        drawMirrorLightModel();
        lk_setList(0x5D58, 0x5D60);
        BOOL binho = FALSE;
        u32 bc = gabi::ea(models.mpBottleContentsModel.get());
        if (bc != 0) {
            gabi::Local<SafeString> n1;
            gabi::Local<SafeString> k1;
            n1->__vtbl = 0x10034B24;
            n1->mStringTop = lk_relPtr((u32)gabi::at<PlayerModelNameView>(bc)->mpNameData + 4);
            k1->__vtbl = 0x10034B24;
            k1->mStringTop = 0x100353C4; /* "binho": the firefly is drawn after the bottle cap */
            if (lk_keyEq(gabi::ea(n1.get()), gabi::ea(k1.get()))) {
                binho = TRUE;
            } else {
                updateDLSetLight(gabi::at<J3DModel>(gabi::ea(models.mpBottleContentsModel.get())), 0);
            }
        }
        if (gabi::ea(models.mpEquipItemModel.get()) != 0 && !checkCaughtShapeHide() && !checkDemoSwordNoDraw(0)) {
            u16 item = mEquipItem;
            bool skip = false;
            if (checkBowItem(item)) {
                skip = gabi::call_ptr<BOOL>(gabi::at<PlayerActorDispatch>(self)->mpVtable->mCheckPlayerGuard, this) != 0; /* checkPlayerGuard() */
                item = mEquipItem;
            }
            if (!skip) {
                if (item == 0x2F) {
                    lk_setHookshotTipMtx(self);
                }
                entryDLSetLight(models.mpEquipItemModel.get(), noResetFlg1() & 0x800);
                entryHDPacket(self + 0x5218, gabi::ea(models.mpEquipItemModel.get()));
                if (gabi::ea(models.mpSwordModel1.get()) != 0 &&
                    (checkChanceMode() || (noResetFlg1() & 0x8000) /* SOUP_POWER_UP */ || dComIfGs_getSelectEquip(0) == 0x3E /* checkFinalMasterSwordEquip() */)) {
                    updateDLSetLight(gabi::at<J3DModel>(gabi::ea(models.mpSwordModel1.get())), 0); /* mpSwordModel1 */
                }
            }
        }
        if (gabi::ea(models.mpBottleCapModel.get()) != 0 && m355E != 0) {
            updateDLSetLight(gabi::at<J3DModel>(gabi::ea(models.mpBottleCapModel.get())), 0); /* mpBottleCapModel */
        }
        if (gabi::ea(models.mpBottleContentsModel.get()) != 0 && binho) {
            updateDLSetLight(gabi::at<J3DModel>(gabi::ea(models.mpBottleContentsModel.get())), 0);
        }
        if (!(lk_camAttnStatus(this, self + 0x69BC) & 0x20)) {
            u32 a = gabi::ea(models.mYaura00rBrk.mpAnm.get()); /* mYaura00rBrk's animation */
            if ((f32)gabi::at<PlayerAnimationFrame>(a)->mFrame > 0.0f) {
                gabi::at<mDoExt_brkAnm>(gabi::ea(&models.mYaura00rBrk))->entry(gabi::at<J3DModelData>(gabi::ea(gabi::at<J3DModel>(gabi::ea(models.mMagicArmorAuraEntries[0].mpModel.get()))->getModelData())), (f32)gabi::at<PlayerAnimationFrame>(a)->mFrame);
                for (int i = 0; i < 6; i++) { /* mMagicArmorAuraEntries: HD sets the btk frame only */
                    lk_anmObjSetFrame(gabi::ea(&models.mYaura00Btk), 0x68, 0x10, (f32)models.mMagicArmorAuraEntries[i].mFrame);
                }
            }
            a = gabi::ea(models.mYmgcs00Brk.mpAnm.get()); /* mYmgcs00Brk */
            if ((f32)gabi::at<PlayerAnimationFrame>(a)->mFrame > 0.0f) {
                gabi::at<mDoExt_brkAnm>(gabi::ea(&models.mYmgcs00Brk))->entry(gabi::at<J3DModelData>(gabi::ea(gabi::at<J3DModel>(gabi::ea(models.mpYmgcs00Model.get()))->getModelData())), (f32)gabi::at<PlayerAnimationFrame>(a)->mFrame);
                mDoExt_modelEntryDL(gabi::at<J3DModel>(gabi::ea(models.mpYmgcs00Model.get())));
            }
        }
    }
    if (checkHDAnmNotEnd()) { /* HD: fanWindEffectDraw() */
        updateDLSetLight(gabi::at<J3DModel>(gabi::ea(models.mpYuchw00Model.get())), 0);
    }
    lk_setList(0x5D78, 0x5D7C); /* dComIfGd_setList() */
    {
        J3DMtxBuffer_hd* buffer = mpCLModel->mpMtxBuffer;
        buffer->mFlags = (u16)buffer->mFlags | 0x10;
    }
    if (mCurProc == daPyProc_FAN_GLIDE_e) { /* HD: the shadow model only while gliding */
        drawShadow();
    }
    u32 blur = mpSwBlur;
    if ((s32)gabi::at<PlayerSwordBlurView>(blur)->mCount > 0) {
        u32 play = dComIfGp_ea();
        Player_entryZSortXlu(play + 0x5D30, (u32)reinterpret_cast<dDlstHD::dDlst_list_c*>(&gabi::at<dComIfG_inf_c>(play)->drawlist)->mpXluList, blur, gabi::ea(&gabi::at<PlayerSwordBlurView>(blur)->mPosition));
    }
#undef LK_freeze
    return TRUE;
}

/* WWHD glue (tools/verify/autoglue.py)
data j3dSysState 104B45C0
data g_Counter 101FF560
data damageEffectType 101CEF16
data demoObject 101D5FFC
data faceMaterialName 1046CCB4
data ear3MaterialName 1046CCC4
data ear2MaterialName 1046CCCC
data eyeMaterialNames 1046CD2C
ext 02526E70 u32 Player_demoGetActor(u32 object, u32 id)
ext 025F181C void Player_ZrotS(u32 matrix, s32 angle)
ext 025F1884 void Player_YrotS(u32 matrix, s32 angle)
ext 025DD868 BOOL Player_isCreating(u32 processId)
ext 027ED3AC void Player_blendQuaternion(u32 first, u32 second, u32 out, f32 ratio)
ext 0201ADE0 void Player_subtractPosition(u32 first, cXyz* out, u32 second)
ext 027DF9B0 u32 Player_materialNameIndex(u32 table, u32 name)
ext 024F8044 u32 Player_cameraBody()
ext 025F1108 void Player_posToCamera(cXyz* position, cXyz* out)
ext 0252F4E0 void Player_setDrawListPosition(u32 position)
ext 0252F3B0 void Player_entryZSortXlu(u32 list, u32 buffer, u32 packet, u32 position)
str 101CEB48 "Link" in 023D4BB8
str 100350F8 "d_a_player_main.cpp" in 023D4BB8
str 1003510C "tmp_modelData != (0)" in 023D4BB8
str 100350E8 "*i_model != (0)" in 023D4BB8
str 10035038 "d_demo.h" in 023D8EB0
str 10034F74 "m_object != (0)" in 023D8EB0
str 1003512C "d_a_player_main.cpp" in 023D4C78
str 10035140 "brk_anm != (0)" in 023D4C78
str 10035150 "d_a_player_main.cpp" in 023D4D64
str 10035164 "btk_anm != (0)" in 023D4D64
str 100351C4 "d_a_player_main.cpp" in 023D5164
str 100351D8 "m_tex_anm_heap.m_buffer != (0)" in 023D5164
str 1003517C "d_a_player_main.cpp" in 023D4F08
str 10035190 "m_tex_scroll_heap.m_buffer != (0)" in 023D4F08
str 10035258 "d_a_player_main.cpp" in 023D5548
str 1003520C "0" in 023D5548
str 100352C8 "tmp_modelData != (0)" in 023D5548
str 1003526C "aura_p->getModel() != (0)" in 023D5548
str 100352E0 "m_old_fdata != (0)" in 023D5548
str 10035288 "m_pbCalc[PART_UNDER_e] != (0)" in 023D5548
str 100352A8 "m_pbCalc[PART_UPPER_e] != (0)" in 023D5548
str 100352F4 "m_HIO != (0)" in 023D5548
str 10035218 "m_anm_heap_under[UNDER_MOVE0_e].m_buffer != (0)" in 023D5548
str 10035304 "m_sanm_buffer != (0)" in 023D5548
str 1003531C "m_item_bck_buffer != (0)" in 023D5548
str 10035338 "p_landing_model_data" in 023D5548
str 10035248 "m_landing_model" in 023D5548
ext 027E1038 void Player_prepareTexturePattern(PlayerTextureController* controller)
ext 027DF40C void Player_prepareTextureScroll(PlayerTextureController* controller)
ext 028E9108 void MTXConcat(void*, void*, void*)
ext 0252DA64 void Player_updateMirrorPacket(u32 packet, Mtx34* matrix, u32 alpha, f32 size)
ext 027F0E04 void Player_drawBufferEntryImmediate(u32 buffer, u32 packet, u32 index)
ext 0207FD38 void Player_initializeDrawPacket(PlayerDrawPacket* packet, u32 mode)
ext 025E3FB4 u32 mDoExt_MtxCalcAnmBlendTblOld_constructCall(void*, void*, u32, void*, void*)
ext 025E4050 void mDoExt_MtxCalcAnmBlendTblOld_bindModelCall(void*, u32)
ext 025602F0 void Player_getSeaColor(GXColor* primary, GXColor* secondary)
ext 0273ADAC u32 Player_newArray(u32 size)
ext 025F1084 void Player_getViewport(view_port_class* out)
ext 025F1018 void Player_project(cXyz* position, cXyz* out, view_port_class* viewport)
ext 0252CDC0 void Player_listSet(u32 list, u32 first, u32 last, u32 packet)
ext 02008E0C void Player_constructGroundCheck(cBgS_GndChk* check)
ext 02008DAC void Player_destroyGroundCheck(cBgS_Chk* check, s32 flag)
ext 0200FA40 void Player_offsetPosition(cXyz* out, cXyz* position, s32 angle, cXyz* offset)
ext 02008974 f64 Player_groundCrossNative(dBgS* background, dBgS_GndChk* check)
ext 02008254 BOOL Player_checkPolySafe(dBgS* background, cBgS_PolyInfo* poly)
ext 0274D458 void Player_convertColorToRGB(cXyz* out, PlayerColorF4* color, f32 scale)
ext 027F9F0C u32 Player_materialParameter(u32 block, u32 index)
data guard_rate 101CEE60
ext 027F3F94 u32 Player_getJointTree(u32 modelData)
ext 027E0174 s32 Player_jointSubtreeEnd(u32 tree, s32 joint)
ext 026066C4 u32 Player_getArchiveResource(dRes_control_c* control, SafeString* archive, u32 index)
ext 027E09B8 void PlayerTexPattern_init(u32 obj, PlayerTextureAnimationSetup* setup, u32 buffer, u32 size)
ext 027DEE5C void PlayerTexScroll_init(u32 obj, PlayerTextureAnimationSetup* setup, u32 buffer, u32 size)
ext 027E12B4 void PlayerTexPattern_setAnm(u32 obj, u32 animation)
ext 027E0C88 void PlayerTexPattern_setModelData(u32 obj, u32 modelData)
ext 028E8DD0 f64 Player_squareMagnitudeNative(const cXyz* vector)
ext 028F4384 f64 Player_sqrtNative(f64 value)
ext 020195B0 u32 Player_atan2Native(f64 y, f64 x)
ext 025F1C5C void Player_ZrotNative(u32 matrix, u32 angle)
ext 025F1BF4 void Player_XrotNative(u32 matrix, u32 angle)
data arm_pos 1046CDC0
data armA_offset 1046CDCC
data arm_pos_initialized 1046D080
data armA_offset_initialized 1046D084
ext 027ED2D0 void PlayerEulerToQuat(s32 x, s32 y, s32 z, u32 out)
ext 025F2258 void PlayerQuatConcat(u32 first, u32 second, u32 out)
ext 025E8508 BOOL Player_initBckAnimation(u32 anm, u32 data, u32 animation, s32 play, s32 mode, f32 speed, s32 start, s32 end, s32 modify)
ext 025E8154 BOOL Player_initBrkAnimation(u32 anm, u32 data, u32 animation, s32 play, s32 mode, f32 speed, s32 start, s32 end, s32 modify, s32 entry)
ext 025E7CE0 BOOL Player_initBtkAnimation(u32 anm, u32 data, u32 animation, s32 play, s32 mode, f32 speed, s32 start, s32 end, s32 modify, s32 entry)
ext 025E3570 u32 mDoExt_setCurrentHeap(u32 heap)
ext 0273AE48 u32 operator_new_align(u32 size, s32 align)
ext 0273AEC4 u32 operator_new_array_align(u32 size, s32 align)
ext 027F3F8C u32 PlayerModelData_getResource(u32 data)
ext 027DF7F4 void PlayerTexScroll_setAnm(u32 obj, u32 anm)
ext 027DF240 void PlayerTexScroll_setModelData(u32 obj, u32 resource)
ext 02591200 void dMat_control_c::iceUpdateDL(J3DModel*, s32, s32)
ext 0259130C void dMat_control_c::iceEntryDL(J3DModel*, s32, s32)
data m_maba_flg 101CEF1B
data m_eye_move_flg 101CEF1A
data m_maba_timer 101CEF18
data m_morf_frame 101CEF19
data sleeveMaterialName 1046CCA4
data ear4MaterialName 1046CCBC
ext 023DC63C void Player01_clearActorKeep(daPy_actorKeep_l* keep)
ext 023DE638 void Player01_setActorKeep(daPy_actorKeep_l* keep, fopAc_ac_c* actor)
ext 023DC7AC void Player01_deleteEquipItem(daPy_lk_c* player, BOOL sound)
ext 023DE0B4 void Player01_setSwordModel(daPy_lk_c* player, BOOL drawn)
ext 023DF600 void Player01_makeItemType(daPy_lk_c* player)
ext 023DC58C void Player01_resetPriTextureAnime(daPy_lk_c* player)
ext 023DEA24 void Player01_setBowModel(daPy_lk_c* player)
ext 023DF0FC void Player01_setBottleModel(daPy_lk_c* player, u32 item)
ext 023DD768 void Player01_setTextureAnime(daPy_lk_c* player, u32 face, u32 mode)
ext 023DC110 void Player01_setTextureAnimeResource(daPy_lk_c* player, u32 resource, u32 mode)
data damageEffectTimer 101CEF14
ext 024EFF3C void Player01_setWallR(dBgS_AcchCir* circle, f32 radius)
ext 023D4538 void Player01_endMtxEmitter(u32 callback)
ext 027F2BC0 void Player01_initFrameCtrl(u32 frameCtrl, s32 end)
ext 027F2FC4 void Player01_updateFrameCtrl(J3DFrameCtrl* frameCtrl)
ext 025E3F3C void Player01_initOldFrameMorf(u32 oldFrame, f32 duration, u32 start, u32 end)
ext 023D457C void Player01_makeMtxEmitter(u32 callback, u32 effect, u32 matrix, cXyz* position, u32 flags)
ext 025E535C void Player01_playMorf(u32 morf, u32 flags, u32 reverb, u32 sound)
ext 025E83FC void Player01_entryBrk(u32 animation, u32 modelData, f32 frame)
ext 025E7FC4 void Player01_entryBtk(u32 animation, u32 modelData, f32 frame)
ext 025E779C void Player01_entryBpk(u32 animation, u32 modelData, f32 frame)
ext 025E1E54 u32 Player01_checkMainBgm()
ext 025E1E60 u32 Player01_checkSubBgm()
ext 0201C020 void Player01_initActorAnimSound(u32 sound, u32 buffer, s32 direction, f32 loop)
ext 0201BFD8 void Player01_setPlayPosition(u32 sound, f32 position)
ext 024EDFCC BOOL Player01_lockonTruth(u32 attention)
ext 024EE058 u32 Player01_getLockonList(u32 attention, u32 index)
ext 024EBA14 fopAc_ac_c* Player01_getAttentionActor(u32 entry)
ext 024EE090 u32 Player01_getActionBtnB(u32 attention)
ext 024EE2F4 u32 Player01_getActionBtnX(u32 attention)
ext 024EE350 u32 Player01_getActionBtnY(u32 attention)
ext 024EE3AC u32 Player01_getActionBtnZ(u32 attention)
ext 024EE408 u32 Player01_getActionBtnHD(u32 attention)
ext 025E871C void Player01_changeBckOnly(u32 animation, u32 resource)
ext 023DE71C u32 Player01_getAnimeResource(daPy_lk_c* player, u32 heap, u32 index, u32 size)
ext 023DE788 void Player01_setFrameCtrl(daPy_lk_c* player, u32 control, u32 attribute, s32 start, s32 end, f32 rate, f32 frame)
ext 0260702C u32 Player01_getSoundAnimeResource(dRes_control_c* control, SafeString* archive, u32 metadata)
ext 025D5928 fopAc_ac_c* Player01_fastCreate(s32 name, u32 parameters, cXyz* position, s32 room, u32 angle, u32 scale, s32 subtype, u32 callback, u32 user)
ext 020CB8D8 u32 Player01_makeBombParameters(u32 type, u32 flag, u32 fuse)
ext 025D9D0C void Player01_setCarryNow(fopAc_ac_c* actor, u32 flags)
ext 025445B8 BOOL Player01_startCheckOld(dEvent_manager_c* manager, const char* name)
ext 02544830 u32 Player01_getMyNowCutName(dEvent_manager_c* manager, s32 staff)
ext 025D7C98 fopAc_ac_c* Player01_getItemEventPartner(daPy_lk_c* player)
ext 021842B8 void Player01_hideItem(fopAc_ac_c* item)
ext 021841C8 u32 Player01_getItemNo(fopAc_ac_c* item)
ext 0218432C void Player01_deadItem(fopAc_ac_c* item)
ext 025F7DB0 s32 Player01_messageSet(u32 manager, u32 message, u32 flags)
ext 025F795C u8 Player01_getMessageStatus()
ext 025F74D0 void Player01_setMessageStatus(u32 manager, u32 status)
ext 025DB58C void Player01_demoMsgFlagOn()
ext 02444F48 void Player01_assureMaterialName(u32 string)
ext 02019788 f64 Player01_random()
ext 027F58E0 void Player01_setBottleModelFlag(u32 model, u32 flag)
ext 025E74FC void Player01_initBpk(u32 animation, u32 data, u32 resource, s32 play, s32 mode, f32 speed, s32 start, s32 end, s32 modify, s32 entry)
ext 025E1ACC void Player01_stopSound(u32 sound)
ext 0255F3B4 void Player01_itemGetColorOff()
ext 025A5F88 void Player01_endSmoke(u32 callback)
ext 025E1D08 void Player01_stopSwordBgm(u32 usingSword, u32 item)
ext 025E1D08 void Player01_startSwordBgm(u32 usingSword)
data mSwordAnmIndexTable 1003655C
data mBokoAnmIndexTable 100365C8
data mHammerAnmIndexTable 10036634
data mProcInitTable 10036DF0
data sword_model_tbl 1003547C
str 1003549C "d_a_player_sword.inc" in 023DE0B4
str 10035470 "0" in 023DE0B4
str 10035534 "LkAnm" in 023DF0FC
str 100354EC "d_a_player_bow.inc" in 023DEA24
str 100354DC "0" in 023DEA24
str 100353D4 "Link" in 023DBDD0
str 100353DC "DEFAULT_GETITEM" in 023DBDD0
str 100353EC "mouth" in 023DC110
str 10035518 "d_a_player_hammer.inc" in 023DF020
str 10035514 "0" in 023DF020
str 1003550C "LkAnm" in 023DEF04
str 100354B8 "d_a_player_hook.inc" in 023DE654
str 100354B4 "0" in 023DE654
str 1003545C "d_a_player_fan.inc" in 023DDF9C
str 10035458 "0" in 023DDF9C
str 10035450 "LkAnm" in 023DDEEC
str 10035400 "LkAnm" in 023DC4DC
str 100353F4 "LkAnm" in 023DC38C
str 100354CC "LkAnm" in 023DE71C
str 10035474 "LkAnm"
 str 100354E0 "lineballMat" in 023DEA24
str 100354D4 "lineMat" in 023DEA24
str 10035500 "lineDamMAT" in 023DEA24
*/
