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
#include "d/actor/d_a_player_main.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E3570 mDoExt_setCurrentHeap(JKRHeap*) -> old heap */
static inline u32 mDoExt_setCurrentHeap_l(u32 heap) { return gabi::call<u32>(0x025E3570, heap); }
/* 0273AE48 operator new(size, align) (heap alloc with alignment) */
static inline u32 operator_new_align_l(u32 size, s32 align) { return gabi::call<u32>(0x0273AE48, size, align); }
/* j3dSys.mModel (HD 0x104B462C); J3DModel user area at +0xB8 */
static inline u32 j3dSys_getModel_l() { return gabi::load<u32>(0x104B462C); }
/* dComIfGs save data object *(0x101F84DC); dSv_event_c at +0x644 */
static inline u32 dComIfGs_base_l() { return gabi::load<u32>(0x101F84DC); }

/* daPy_lk_c members in the opaque blocks of the shared header (offsets measured in this range) */
#define LK_FIELD(T, off) (*gabi::at<be<T>>(gabi::ea(this) + (off)))
#define LK_upperAnmIdx() gabi::load<u16>(gabi::ea(this) + 0x5888) /* m_anm_heap_upper[UPPER_MOVE2_e].mIdx */
#define LK_underAnmIdx() gabi::load<u16>(gabi::ea(this) + 0x5848) /* m_anm_heap_under[UNDER_MOVE0_e].mIdx */
#define mpEquipItemModel_ea (gabi::ea(this) + 0x4440)              /* J3DModel* mpEquipItemModel */
/* virtual daPy_lk_c::checkRopeReadyAnime() const (vtable slot 0xDC) */
#define LK_checkRopeReadyAnime() gabi::call_ptr<BOOL>(gabi::load<u32>(gabi::load<u32>(gabi::ea(this) + 0xB4) + 0xDC), this)

/* a float copy through an FPR that the recompiled original keeps bit-exact (no SNaN quieting) */
static inline void fcpy_l(u32 dst, u32 src) { gmem_stf32(dst, gmem_ld32(src)); }

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

/* 023D4E54: operator new[](size, align) for a type without a constructor (HD, unnamed) */
static u32 daPy_newAlign(u32 size, s32 align) {
    WWHD_FUNC(0x023D4E54, u32, size, align);
    return operator_new_align_l(size, align);
}
VERIFY(0x023D4E54, daPy_newAlign);

/* 023D4E8C */
static u32 daPy_matAnm_ct(u32 self) {
    WWHD_FUNC(0x023D4E8C, u32, self);
    if (self == 0) {
        self = gabi::ea(operator_new(0x7C));
        if (self == 0) {
            return self;
        }
    }
    gabi::store<f32>(self + 0x74, 0.0f); /* mNowOffset.y */
    gabi::store<f32>(self + 0x6C, 0.0f); /* mOldOffset.x */
    gabi::store<u32>(self + 0x68, 0x10037AE0); /* HD: vtable (J3DMaterialAnm base) */
    gabi::store<f32>(self + 0x70, 0.0f); /* mOldOffset.y */
    gabi::store<f32>(self + 0x78, 0.0f); /* mNowOffset.x */
    /* HD: m_maba_flg, m_eye_move_flg, m_maba_timer, m_morf_frame are static members */
    gabi::store<u8>(0x101CEF1B, 0);
    gabi::store<u8>(0x101CEF1A, 0);
    gabi::store<u8>(0x101CEF18, 0);
    gabi::store<u8>(0x101CEF19, 0);
    return self;
}
VERIFY(0x023D4E8C, daPy_matAnm_ct);

/* 023D6184 */
static BOOL daPy_createHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x023D6184, BOOL, i_this);
    return gabi::call<BOOL>(LK_createHeap, i_this);
}
VERIFY(0x023D6184, daPy_createHeap);

/* 023D6210 */
BOOL daPy_lk_c::checkBowReadyAnime() {
    WWHD_FUNC(0x023D6210, BOOL, this);
    u16 idx = LK_upperAnmIdx();
    return idx == 0xC || idx == 0x36;
}
VERIFY(0x023D6210, &daPy_lk_c::checkBowReadyAnime);

/* 023D63D0 */
static BOOL daPy_bowJointCB(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x023D63D0, BOOL, node, calcTiming);
    if (!calcTiming) {
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 i_this = gabi::load<u32>(j3dSys_getModel_l() + 0xB8);
        if (i_this != 0) { /* HD: NULL check */
            gabi::call<BOOL>(LK_bowJointCB, i_this, (u32)gabi::load<u16>(gabi::ea(joint) + 4));
        }
    }
    return TRUE;
}
VERIFY(0x023D63D0, daPy_bowJointCB);

/* 023D6604 */
static BOOL daPy_fanJointCallback(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x023D6604, BOOL, node, calcTiming);
    if (!calcTiming) {
        u32 i_this = gabi::load<u32>(j3dSys_getModel_l() + 0xB8);
        J3DJoint* joint = J3DNode_toJoint(node);
        gabi::call<BOOL>(LK_fanJointCB, i_this, (u32)gabi::load<u16>(gabi::ea(joint) + 4));
    }
    return TRUE;
}
VERIFY(0x023D6604, daPy_fanJointCallback);

/* 023D6850 */
static BOOL daPy_parachuteJointCallback(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x023D6850, BOOL, node, calcTiming);
    if (!calcTiming) {
        u32 i_this = gabi::load<u32>(j3dSys_getModel_l() + 0xB8);
        J3DJoint* joint = J3DNode_toJoint(node);
        gabi::call<BOOL>(LK_parachuteJointCB, i_this, (u32)gabi::load<u16>(gabi::ea(joint) + 4));
    }
    return TRUE;
}
VERIFY(0x023D6850, daPy_parachuteJointCallback);

/* 023D6980 */
static BOOL daPy_HDJointCallback(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x023D6980, BOOL, node, calcTiming);
    if (!calcTiming) {
        u32 i_this = gabi::load<u32>(j3dSys_getModel_l() + 0xB8);
        J3DJoint* joint = J3DNode_toJoint(node);
        gabi::call<BOOL>(LK_HDJointCB_023D689C, i_this, (u32)gabi::load<u16>(gabi::ea(joint) + 4));
    }
    return TRUE;
}
VERIFY(0x023D6980, daPy_HDJointCallback);

/* 023D80F0 */
static BOOL daPy_jointCallback0(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x023D80F0, BOOL, node, calcTiming);
    if (!calcTiming) {
        u32 i_this = gabi::load<u32>(j3dSys_getModel_l() + 0xB8);
        J3DJoint* joint = J3DNode_toJoint(node);
        gabi::call<BOOL>(LK_jointCB0, i_this, (u32)gabi::load<u16>(gabi::ea(joint) + 4));
    }
    return TRUE;
}
VERIFY(0x023D80F0, daPy_jointCallback0);

/* 023D69CC */
BOOL daPy_lk_c::checkShipNotNormalMode() {
    WWHD_FUNC(0x023D69CC, BOOL, this);
    u16 idx = LK_underAnmIdx();
    if (idx == 0xF4 /* SHIP_JUMP1 */ || idx == 0x11B || idx == 0xF3) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x023D69CC, &daPy_lk_c::checkShipNotNormalMode);

/* 023D69F8 */
BOOL daPy_lk_c::checkBoomerangAnime() {
    WWHD_FUNC(0x023D69F8, BOOL, this);
    u16 idx = LK_upperAnmIdx();
    return idx == 0x35 || idx == 0x34;
}
VERIFY(0x023D69F8, &daPy_lk_c::checkBoomerangAnime);

/* 023D6A18 */
BOOL daPy_lk_c::checkBowAnime() {
    WWHD_FUNC(0x023D6A18, BOOL, this);
    return checkBowReadyAnime() || LK_upperAnmIdx() == 0xE /* checkBowShootAnime */;
}
VERIFY(0x023D6A18, &daPy_lk_c::checkBowAnime);

/* 023D6A5C */
BOOL daPy_lk_c::checkRopeAnime() {
    WWHD_FUNC(0x023D6A5C, BOOL, this);
    return LK_checkRopeReadyAnime() || LK_upperAnmIdx() == 0xE2 /* ROPETHROW */;
}
VERIFY(0x023D6A5C, &daPy_lk_c::checkRopeAnime);

/* 023D6ABC */
BOOL daPy_lk_c::checkUpperReadyThrowAnime() {
    WWHD_FUNC(0x023D6ABC, BOOL, this);
    return checkBoomerangAnime() || checkBowAnime() || checkRopeAnime() || LK_upperAnmIdx() == 0xA7 /* checkHookshotReadyAnime */;
}
VERIFY(0x023D6ABC, &daPy_lk_c::checkUpperReadyThrowAnime);

/* 023D7194 */
static BOOL daPy_jointBeforeCallback(u32 userArea, int jnt_no, u32 p2, u32 p3 /* J3DTransformInfo*, Quaternion* */) {
    WWHD_FUNC(0x023D7194, BOOL, userArea, jnt_no, p2, p3);
    return gabi::call<BOOL>(LK_jointBeforeCB, userArea, jnt_no, p2, p3);
}
VERIFY(0x023D7194, daPy_jointBeforeCallback);

/* 023D78E0 */
static BOOL daPy_jointAfterCallback(u32 userArea, int jnt_no, u32 p2, u32 p3 /* J3DTransformInfo*, Quaternion* */) {
    WWHD_FUNC(0x023D78E0, BOOL, userArea, jnt_no, p2, p3);
    return gabi::call<BOOL>(LK_jointAfterCB, userArea, jnt_no, p2, p3);
}
VERIFY(0x023D78E0, daPy_jointAfterCallback);

/* 023D7904 */
BOOL daPy_lk_c::checkItemEquipAnime() {
    WWHD_FUNC(0x023D7904, BOOL, this);
    u16 idx = LK_upperAnmIdx(); /* read once, before the call */
    return idx == 0x103 /* TAKE */ || checkSingleItemEquipAnime() || idx == 0x104 /* TAKEBOTH */;
}
VERIFY(0x023D7904, &daPy_lk_c::checkItemEquipAnime);

/* 023D798C */
BOOL daPy_lk_c::checkUpperReadyAnime() {
    WWHD_FUNC(0x023D798C, BOOL, this);
    return LK_upperAnmIdx() == 0x35 /* checkBoomerangReadyAnime */ || checkBowReadyAnime() || LK_checkRopeReadyAnime() ||
           LK_upperAnmIdx() == 0xA7 /* checkHookshotReadyAnime */;
}
VERIFY(0x023D798C, &daPy_lk_c::checkUpperReadyAnime);

/* 023D85A4 */
BOOL daPy_lk_c::checkCaughtShapeHide() {
    WWHD_FUNC(0x023D85A4, BOOL, this);
    if (mCurProc == 0xBD /* daPyProc_DEMO_CAUGHT_e */ && mProcVar6 != 0) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x023D85A4, &daPy_lk_c::checkCaughtShapeHide);

/* 023D8E78 */
BOOL daPy_lk_c::checkDemoShieldNoDraw() {
    WWHD_FUNC(0x023D8E78, BOOL, this);
    return dSv_event_isEventBit(gabi::at<dSv_event_c>(dComIfGs_base_l() + 0x644), 0x3F40) != 0;
}
VERIFY(0x023D8E78, &daPy_lk_c::checkDemoShieldNoDraw);

/* 023D9340: HD-only, unnamed: registers a packet (r4) in a draw list kept in Link at 0x4B58 */
void daPy_lk_c::entryHDPacket(u32 pkt, u32 arg) {
    WWHD_FUNC(0x023D9340, void, this, pkt, arg);
    if (LK_FIELD(u8, 0x4BE4) == 0) {
        return;
    }
    gabi::store<u32>(pkt + 0x98, arg);
    s32 n = LK_FIELD(s32, 0x4B58);
    if (n >= LK_FIELD(s32, 0x4B5C)) {
        return;
    }
    gabi::store<u32>(LK_FIELD(u32, 0x4B60) + n * 4, pkt);
    LK_FIELD(s32, 0x4B58) = LK_FIELD(s32, 0x4B58) + 1;
}
VERIFY(0x023D9340, &daPy_lk_c::entryHDPacket);

/* 023D937C */
BOOL daPy_lk_c::checkChanceMode() {
    WWHD_FUNC(0x023D937C, BOOL, this);
    if (gabi::load<u8>(dComIfGp_ea() + 0x5BB7) == 0x1A /* dActStts_PARRY_e */ || (mModeFlg & 0x80000000) /* ModeFlg_PARRY */) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x023D937C, &daPy_lk_c::checkChanceMode);

/* 023D93DC: HD-only, unnamed: the frame of the animation at 0x54E8 has not passed its end */
BOOL daPy_lk_c::checkHDAnmNotEnd() {
    WWHD_FUNC(0x023D93DC, BOOL, this);
    f32 end = (f32)LK_FIELD(s16, 0x54FE) - 0.5f;
    return !(LK_FIELD(f32, 0x54F8) > end);
}
VERIFY(0x023D93DC, &daPy_lk_c::checkHDAnmNotEnd);

/* 023DBBE4 */
static BOOL daPy_Draw(daPy_lk_c* i_this) {
    WWHD_FUNC(0x023DBBE4, BOOL, i_this);
    return gabi::call<BOOL>(LK_draw, i_this);
}
VERIFY(0x023DBBE4, daPy_Draw);

/* 023DBBE8 */
BOOL daPy_lk_c::checkGrabSpecialHeavyState() {
    WWHD_FUNC(0x023DBBE8, BOOL, this);
    fopAc_ac_c* grab_actor = mActorKeepGrab.mActor;
    if (grab_actor != nullptr) {
        if (fpcM_GetName(grab_actor) == 0xDC /* fpcNm_KB_e */ && (fopAcM_GetParam(grab_actor) & 8)) {
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x023DBBE8, &daPy_lk_c::checkGrabSpecialHeavyState);

/* 023DC650 */
void daPy_lk_c::deleteArrow() {
    WWHD_FUNC(0x023DC650, void, this);
    fopAc_ac_c* equip_actor = mActorKeepEquip.mActor;
    if (equip_actor != nullptr && fpcM_GetName(equip_actor) == 0x1D8 /* fpcNm_ARROW_e */) {
        fopAcM_delete(equip_actor);
        gabi::call(LK_actorKeep_clearData, &mActorKeepEquip);
    }
}
VERIFY(0x023DC650, &daPy_lk_c::deleteArrow);

/* 023DDEA8 */
u32 daPy_lk_c::setAnimeHeap(u32 animeHeap) {
    WWHD_FUNC(0x023DDEA8, u32, this, animeHeap);
    /* JKRHeap::freeAll (virtual, vtable at +0xC, slot 0x54) */
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(animeHeap + 0xC) + 0x54), animeHeap);
    return mDoExt_setCurrentHeap_l(animeHeap);
}
VERIFY(0x023DDEA8, &daPy_lk_c::setAnimeHeap);

/* 023DE71C */
u32 daPy_lk_c::getAnimeResource(u32 anmHeap, u16 index, u32 bufferSize) {
    WWHD_FUNC(0x023DE71C, u32, this, anmHeap, index, bufferSize);
    /* HD: the animation is taken from the resident "LkAnm" archive (no read into the anime heap) */
    gabi::Local<SafeString> key;
    key->mStringTop = 0x100354CC; /* "LkAnm" */
    key->__vtbl = 0x10034B24;
    u32 bck = gabi::call<u32>(0x026066C4, dComIfG_resControl(), key.get(), (u32)index);
    gabi::store<u16>(anmHeap + 0, index);  /* mIdx */
    gabi::store<u16>(anmHeap + 2, 0xFFFF); /* field_0x2 */
    return bck;
}
VERIFY(0x023DE71C, &daPy_lk_c::getAnimeResource);

/* 023DE788 */
void daPy_lk_c::setFrameCtrl(J3DFrameCtrl* frameCtrl, u8 attribute, s16 start, s16 end, f32 rate, f32 frame) {
    WWHD_FUNC(0x023DE788, void, this, frameCtrl, attribute, start, end, rate, frame);
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
VERIFY(0x023DE788, &daPy_lk_c::setFrameCtrl);

/* 023DED8C */
void daPy_lk_c::setScopeModel() {
    WWHD_FUNC(0x023DED8C, void, this);
    u32 oldHeap = gabi::ea(setItemHeap());
    gabi::call(LK_initModel, this, mpEquipItemModel_ea, 0x2F /* LINK_BDL_TELESCOPE */, 0x37221222);
    mDoExt_setCurrentHeap_l(oldHeap);
}
VERIFY(0x023DED8C, &daPy_lk_c::setScopeModel);

/* 023DEEAC */
void daPy_lk_c::setTinkleCeiverModel() {
    WWHD_FUNC(0x023DEEAC, void, this);
    u32 oldHeap = gabi::ea(setItemHeap());
    gabi::call(LK_initModel, this, mpEquipItemModel_ea, 0x27 /* LINK_BDL_TCEIVER */, 0x13000022);
    mDoExt_setCurrentHeap_l(oldHeap);
}
VERIFY(0x023DEEAC, &daPy_lk_c::setTinkleCeiverModel);

/* 023DFA48 */
static void daPy_followEcallBack_end(u32 self) {
    WWHD_FUNC(0x023DFA48, void, self);
    u32 emitter = gabi::load<u32>(self + 4);
    if (emitter != 0) {
        /* becomeInvalidEmitter: HD stops the emitter (max frame -1) and sets its delete flag */
        gabi::store<s32>(emitter + 0x5C, -1);
        gabi::store<u32>(emitter + 0x254, gabi::load<u32>(emitter + 0x254) | 1);
        gabi::store<u32>(gabi::load<u32>(self + 4) + 0x1E4, 0); /* setEmitterCallBackPtr(NULL) */
        gabi::store<u32>(self + 4, 0);
    }
}
VERIFY(0x023DFA48, daPy_followEcallBack_end);

/* 023DFA7C */
void daPy_lk_c::freeHookshotItem() {
    WWHD_FUNC(0x023DFA7C, void, this);
    if (mEquipItem != 0x2F /* dItemNo_HOOKSHOT_e */) {
        return;
    }
    u32 hookshot = gabi::ea(mActorKeepEquip.mActor.get());
    if (hookshot == 0) {
        return;
    }
    if (gabi::load<u32>(hookshot + 0xB0) == 0) { /* checkWait() */
        return;
    }
    gabi::store<u32>(hookshot + 0xB0, 2); /* setReturn() */
}
VERIFY(0x023DFA7C, &daPy_lk_c::freeHookshotItem);

/* 023DFAAC */
void daPy_lk_c::swimOutAfter(BOOL param_1) {
    WWHD_FUNC(0x023DFAAC, void, this, param_1);
    mNoResetFlg0 = mNoResetFlg0 | 0x100; /* onNoResetFlg0(daPyFlg0_UNK100) */
    m35C4 = 0.0f;
    if (param_1) {
        seStartOnlyReverb(0x3809 /* JA_SE_LK_OUTOF_WATER */);
    }
    /* dComIfGp_clearItemTimeCount() */
    u32 play = dComIfGp_ea();
    gabi::store<u32>(play + 0x5B4C, 0);
    gabi::store<u8>(play + 0x5BB0, 0);
    gabi::call(LK_resetPriTextureAnime, this);
}
VERIFY(0x023DFAAC, &daPy_lk_c::swimOutAfter);

/* ======== batch 2 ======== */
/* 025D54C4 fopAcM_SearchByID(id, fopAc_ac_c** out) (the out-of-line form) */
static inline BOOL fopAcM_SearchByID2_l(u32 id, u32 out) { return gabi::call<BOOL>(0x025D54C4, id, out); }
/* 026067F4 dRes_control_c::getIDRes(const SafeString& arc, u16 id) */
static inline u32 dRes_getIDRes_l(u32 key, u32 id) { return gabi::call<u32>(0x026067F4, dComIfG_resControl(), key, id); }
/* sead::SafeString virtual at slot 0x14 (called before its string is read; vtable at +4) */
static inline void SafeString_vf14_l(u32 s) { gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(s + 4) + 0x14), s); }
/* the HD 0x58 animation object at 0x6F4 (texture scroll) and J3DModelData helper */
static inline void HDAnm_setAnm_l(u32 obj, u32 anm) { gabi::call(0x027DF7F4, obj, anm); }
static inline void HDAnm_setModelData_l(u32 obj, u32 x) { gabi::call(0x027DF240, obj, x); }
static inline u32 J3DModelData_HD_027F3F8C_l(u32 data) { return gabi::call<u32>(0x027F3F8C, data); }

/* 023D4BB8 */
u32 daPy_lk_c::initModel(u32 i_model, int i_fileIndex, u32 i_differedDlistFlag) {
    WWHD_FUNC(0x023D4BB8, u32, this, i_model, i_fileIndex, i_differedDlistFlag);
    u32 tmp_modelData = gabi::ea(dComIfG_getObjectRes(gabi::at<const char>(0x101CEB48) /* l_arcName "Link" */, i_fileIndex, 0x10034B24));
    if (tmp_modelData == 0) {
        JUT_ASSERT_fail(gabi::at<const char>(0x100350F8), 0x5EAF, gabi::at<const char>(0x1003510C));
    }
    u32 model = gabi::ea(mDoExt_J3DModel__create(gabi::at<J3DModelData>(tmp_modelData), 0x80000, i_differedDlistFlag));
    gabi::store<u32>(i_model, model);
    if (model == 0) {
        JUT_ASSERT_fail(gabi::at<const char>(0x100350F8), 0x5EB3, gabi::at<const char>(0x100350E8));
    }
    return tmp_modelData;
}
VERIFY(0x023D4BB8, &daPy_lk_c::initModel);

/* 023D6188: HD-only, unnamed: the A button status from the player's state */
static void daPy_setHDAStatus() {
    WWHD_FUNC(0x023D6188, void);
    if (gabi::load<u8>(dComIfGp_ea() + 0x5BB6) != 0) {
        return;
    }
    u32 p = gabi::load<u32>(dComIfGp_ea() + 0x5B3C);
    if (!(gabi::load<f32>(p + 0x370) < 3.0f) && gabi::load<u32>(p + 0x704) == 0 && gabi::load<u32>(p + 0x70C) == 0) {
        gabi::store<u8>(dComIfGp_ea() + 0x5BB6, 0x13);
        return;
    }
    gabi::store<u8>(dComIfGp_ea() + 0x5BB6, 8);
}
VERIFY(0x023D6188, daPy_setHDAStatus);

/* 023D83D8: offBodyEffect (unnamed by the matcher) */
void daPy_lk_c::offBodyEffect() {
    WWHD_FUNC(0x023D83D8, void, this);
    static const u32 offs[5] = {0x6764 /* m334C */, 0x6784 /* m336C */, 0x66FC /* m32E4 */, 0x6878 /* m3460[0] */, 0x6888 /* m3460[1] */};
    for (int i = 0; i < 5; i++) {
        u32 em = gabi::load<u32>(gabi::ea(this) + offs[i]);
        if (em != 0) {
            gabi::store<u32>(em + 0x254, gabi::load<u32>(em + 0x254) | 4); /* stopDrawParticle */
        }
    }
}
VERIFY(0x023D83D8, &daPy_lk_c::offBodyEffect);

/* 023D8454 */
void daPy_lk_c::updateDLSetLight(J3DModel* model, u32 param_2) {
    WWHD_FUNC(0x023D8454, void, this, model, param_2);
    setLightTevColorType(dKy_getEnvlight(), model, gabi::at<dKy_tevstr_c>(gabi::ea(this) + 0x110));
    if (param_2 != 0) {
        gabi::call(0x02591200 /* dMat_control_c::iceUpdateDL */, model, -1, 0);
    } else {
        mDoExt_modelUpdateDL(model, 0);
    }
}
VERIFY(0x023D8454, &daPy_lk_c::updateDLSetLight);

/* 023D8DF0 */
void daPy_lk_c::entryDLSetLight(J3DModel* model, u32 param_2) {
    WWHD_FUNC(0x023D8DF0, void, this, model, param_2);
    setLightTevColorType(dKy_getEnvlight(), model, gabi::at<dKy_tevstr_c>(gabi::ea(this) + 0x110));
    if (param_2 != 0) {
        gabi::call(0x0259130C /* dMat_control_c::iceEntryDL */, model, -1, 0);
    } else {
        mDoExt_modelEntryDL(model);
    }
}
VERIFY(0x023D8DF0, &daPy_lk_c::entryDLSetLight);

/* 023D8EB0 */
BOOL daPy_lk_c::checkMaskDraw() {
    WWHD_FUNC(0x023D8EB0, BOOL, this);
    u8 id = gabi::load<u8>(gabi::ea(this) + 0x2DC); /* demoActorID */
    /* dComIfGp_demo_getActor(id) */
    if (id != 0 && id <= 0x20) {
        u32 obj = gabi::load<u32>(0x101D5FFC);
        if (obj == 0) {
            JUT_ASSERT_fail(gabi::at<const char>(0x10035038) /* "d_demo.h" */, 0x23A, gabi::at<const char>(0x10034F74));
            obj = gabi::load<u32>(0x101D5FFC);
        }
        if (gabi::call<u32>(0x02526E70 /* dDemo_object_c::getActor */, obj, (u32)id) != 0) {
            return FALSE;
        }
    }
    /* dComIfGs_isCollect(4, 1) */
    if (gabi::call<BOOL>(0x025B7A2C /* dSv_player_collect_c::isCollect */, dComIfGs_base_l() + 0xD4, 4, 1) != 0) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x023D8EB0, &daPy_lk_c::checkMaskDraw);

/* 023D8F6C */
BOOL daPy_lk_c::checkDemoSwordNoDraw(BOOL param_0) {
    WWHD_FUNC(0x023D8F6C, BOOL, this, param_0);
    if ((mEquipItem == 0x103 /* daPyItem_SWORD_e */ || param_0) &&
        (dSv_event_isEventBit(gabi::at<dSv_event_c>(dComIfGs_base_l() + 0x644), 0x3F40) ||
         (mCurProc == 0xA9 /* daPyProc_DEMO_TOOL_e */ && mProcVar3 == 1))) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x023D8F6C, &daPy_lk_c::checkDemoSwordNoDraw);

/* 023D8FF8: onBodyEffect (unnamed by the matcher) */
void daPy_lk_c::onBodyEffect() {
    WWHD_FUNC(0x023D8FF8, void, this);
    static const u32 offs[5] = {0x6764, 0x6784, 0x66FC, 0x6878, 0x6888};
    for (int i = 0; i < 5; i++) {
        u32 em = gabi::load<u32>(gabi::ea(this) + offs[i]);
        if (em != 0) {
            gabi::store<u32>(em + 0x254, gabi::load<u32>(em + 0x254) & ~4u); /* playDrawParticle */
        }
    }
}
VERIFY(0x023D8FF8, &daPy_lk_c::onBodyEffect);

/* 023D9430: HD-only, unnamed: a GXColor converted to four floats (0..1) */
static u32 daPy_colorToF4(u32 out, u32 color) {
    WWHD_FUNC(0x023D9430, u32, out, color);
    f32 r = (f32)gabi::load<u8>(color + 0) / 255.0f;
    f32 g = (f32)gabi::load<u8>(color + 1) / 255.0f;
    f32 b = (f32)gabi::load<u8>(color + 2) / 255.0f;
    f32 a = (f32)gabi::load<u8>(color + 3) / 255.0f;
    gabi::store<f32>(out + 0, r);
    gabi::store<f32>(out + 4, g);
    gabi::store<f32>(out + 8, b);
    gabi::store<f32>(out + 0xC, a);
    return out;
}
VERIFY(0x023D9430, daPy_colorToF4);

/* 023DBC24 */
BOOL daPy_lk_c::checkHeavyStateOn() {
    WWHD_FUNC(0x023DBC24, BOOL, this);
    if (gabi::load<u8>(dComIfGp_ea() + 0x5292) == 0 /* !dComIfGp_event_runCheck() */ &&
        gabi::load<u16>(gabi::ea(this) + 0x420) == 0 /* !checkPlayerDemoMode() */ &&
        ((mNoResetFlg0 & 0x42000000) /* getHeavyStateAndBoots() */ || checkGrabSpecialHeavyState())) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x023DBC24, &daPy_lk_c::checkHeavyStateOn);

/* 023DC38C: loadTextureAnimeResource (unnamed by the matcher) */
u32 daPy_lk_c::loadTextureAnimeResource(u32 btpIdx, BOOL isDemo) {
    WWHD_FUNC(0x023DC38C, u32, this, btpIdx, isDemo);
    /* HD: the texture animations come from the resident archives (no read into a buffer) */
    gabi::Local<SafeString> key;
    if (isDemo) {
        u32 play = dComIfGp_ea();
        SafeString_vf14_l(play + 0x5A50); /* the Link demo archive name (sead::SafeString at play + 0x5A50) */
        key->__vtbl = 0x10034B24;
        key->mStringTop = gabi::load<u32>(play + 0x5A50);
        return dRes_getIDRes_l(gabi::ea(key.get()), (u16)btpIdx);
    }
    gabi::Local<SafeString> key2;
    key2->__vtbl = 0x10034B24;
    key2->mStringTop = 0x100353F4; /* "LkAnm" */
    return gabi::call<u32>(0x026066C4, dComIfG_resControl(), key2.get(), btpIdx);
}
VERIFY(0x023DC38C, &daPy_lk_c::loadTextureAnimeResource);

/* 023DC43C */
void daPy_lk_c::setTextureScrollResource(u32 btk, int r31) {
    WWHD_FUNC(0x023DC43C, void, this, btk, r31);
    mpTexScrollResData = btk;
    m3532 = (u16)r31;
    /* HD: the scroll animation is an object at 0x6F4 */
    HDAnm_setAnm_l(gabi::ea(this) + 0x6F4, gabi::load<u32>(btk + 0xC));
    HDAnm_setModelData_l(gabi::ea(this) + 0x6F4, J3DModelData_HD_027F3F8C_l(gabi::ea(mpCLModelData.get())));
    /* daPy_matAnm_c statics */
    gabi::store<u8>(0x101CEF1B, 3); /* setMorfFrame(3) */
    gabi::store<u8>(0x101CEF19, 1);
    gabi::store<u8>(0x101CEF18, 0); /* offMabaFlg() */
    gabi::store<u8>(0x101CEF19, (u8)gabi::ftoi(cM_rndF(30.0f) + 75.0f)); /* setMabaTimer */
}
VERIFY(0x023DC43C, &daPy_lk_c::setTextureScrollResource);

/* 023DC4DC: loadTextureScrollResource (unnamed by the matcher) */
u32 daPy_lk_c::loadTextureScrollResource(u32 btkIdx, BOOL isDemo) {
    WWHD_FUNC(0x023DC4DC, u32, this, btkIdx, isDemo);
    gabi::Local<SafeString> key;
    if (isDemo) {
        u32 play = dComIfGp_ea();
        SafeString_vf14_l(play + 0x5A50);
        key->__vtbl = 0x10034B24;
        key->mStringTop = gabi::load<u32>(play + 0x5A50);
        return dRes_getIDRes_l(gabi::ea(key.get()), (u16)btkIdx);
    }
    gabi::Local<SafeString> key2;
    key2->__vtbl = 0x10034B24;
    key2->mStringTop = 0x10035400; /* "LkAnm" */
    return gabi::call<u32>(0x026066C4, dComIfG_resControl(), key2.get(), btkIdx);
}
VERIFY(0x023DC4DC, &daPy_lk_c::loadTextureScrollResource);

/* 023DC58C */
void daPy_lk_c::resetPriTextureAnime() {
    WWHD_FUNC(0x023DC58C, void, this);
    u32 h = gabi::ea(this) + 0x65D0; /* m_tex_anm_heap: mIdx, field_0x2, field_0x4 */
    if (gabi::load<u16>(h + 2) != 0xFFFF) {
        gabi::store<u16>(h + 2, 0xFFFF);
        if (gabi::load<u16>(h + 4) == 0xFFFF) {
            u32 btp = loadTextureAnimeResource(gabi::load<u16>(h + 0), FALSE);
            gabi::call(LK_setTextureAnimeResource, this, btp, 0);
        }
    }
    h = gabi::ea(this) + 0x65E0; /* m_tex_scroll_heap */
    if (gabi::load<u16>(h + 2) != 0xFFFF) {
        gabi::store<u16>(h + 2, 0xFFFF);
        if (gabi::load<u16>(h + 4) == 0xFFFF) {
            u32 btk = loadTextureScrollResource(gabi::load<u16>(h + 0), FALSE);
            setTextureScrollResource(btk, 0);
        }
    }
}
VERIFY(0x023DC58C, &daPy_lk_c::resetPriTextureAnime);

/* 023DCA08 */
void daPy_lk_c::endDamageEmitter() {
    WWHD_FUNC(0x023DCA08, void, this);
    for (int i = 0; i < 4; i++) {
        gabi::call(0x023D4538 /* daPy_mtxFollowEcallBack_c::end */, gabi::ea(this) + 0x67CC + i * 0xC); /* mDmEcallBack[i] */
    }
    gabi::store<s16>(0x101CEF14, 0); /* daPy_dmEcallBack_c::setTimer(0) */
    gabi::store<s16>(0x101CEF16, 3); /* daPy_dmEcallBack_c::setType(3) */
    LK_FIELD(f32, 0x68C4) = 0.0f;    /* mLightInfluence.mPower */
}
VERIFY(0x023DCA08, &daPy_lk_c::endDamageEmitter);

/* 023DCA80 */
void daPy_lk_c::freeRopeItem() {
    WWHD_FUNC(0x023DCA80, void, this);
    if (mEquipItem == 0x25 /* dItemNo_GRAPPLING_HOOK_e */) {
        fopAc_ac_c* rope = mActorKeepRope.mActor;
        if (mActorKeepEquip.mActor != nullptr) {
            setResetFlg0(resetFlg0() | 0x40000000); /* daPyRFlg0_ROPE_FORCE_END */
        }
        if (rope != nullptr) {
            if (fpcM_GetName(rope) == 0x1BE /* fpcNm_HIMO2_e */) {
                gabi::store<u32>(gabi::ea(rope) + 0xB0, 4); /* fopAcM_SetParam */
                gabi::call(LK_actorKeep_clearData, &mActorKeepRope);
                setResetFlg0(resetFlg0() | 0x40000000);
            } else if (fpcM_GetName(rope) == 0x1BF /* fpcNm_HIMO3_e */) {
                mEquipItem = 0x100; /* daPyItem_NONE_e */
                gabi::store<u32>(gabi::ea(rope) + 0xB0, 3);
                gabi::call(LK_actorKeep_clearData, &mActorKeepRope);
            }
        }
    }
}
VERIFY(0x023DCA80, &daPy_lk_c::freeRopeItem);

/* 023DCC18 */
static void daPy_actorKeep_setActor(daPy_actorKeep_l* k) {
    WWHD_FUNC(0x023DCC18, void, k);
    if (k->mID != 0xFFFFFFFF) {
        k->mActor = fopAcM_SearchByID(k->mID);
        if (k->mActor == nullptr) {
            k->mID = 0xFFFFFFFF;
        }
    } else {
        k->mActor = nullptr;
    }
}
VERIFY(0x023DCC18, daPy_actorKeep_setActor);

/* 023DCC90 */
void daPy_lk_c::setActorPointer() {
    WWHD_FUNC(0x023DCC90, void, this);
    daPy_actorKeep_setActor(&mActorKeepRope);
    daPy_actorKeep_setActor(&mActorKeepGrab);
    daPy_actorKeep_setActor(&mActorKeepThrow);
    daPy_actorKeep_setActor(&mActorKeepEquip);
    if (mEquipItem == 0x101 /* daPyItem_BOKO_e */) {
        fopAc_ac_c* actor = mActorKeepEquip.mActor;
        if (actor == nullptr || !(gabi::load<u32>(gabi::ea(actor) + 0x2E0) & 0x2000) /* !fopAcM_checkCarryNow */) {
            gabi::call(LK_actorKeep_clearData, &mActorKeepEquip);
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
VERIFY(0x023DCC90, &daPy_lk_c::setActorPointer);

/* 023DCF08 */
BOOL daPy_lk_c::checkGrabBarrelSearch(int param_0) {
    WWHD_FUNC(0x023DCF08, BOOL, this, param_0);
    fopAc_ac_c* grab_actor;
    if (param_0 != 0) {
        if (mActorKeepGrab.mID == 0xFFFFFFFF) {
            return FALSE;
        }
        grab_actor = fopAcM_SearchByID(mActorKeepGrab.mID);
    } else {
        grab_actor = mActorKeepGrab.mActor;
    }
    if (grab_actor != nullptr && fpcM_GetName(grab_actor) == 0x1C8 /* fpcNm_Obj_Barrel_e */) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x023DCF08, &daPy_lk_c::checkGrabBarrelSearch);

/* 023DD12C */
BOOL daPy_lk_c::checkRestHPAnime() {
    WWHD_FUNC(0x023DD12C, BOOL, this);
    /* checkPlayerGuard() is virtual (slot 0x3C) */
    if (!gabi::call_ptr<BOOL>(gabi::load<u32>(gabi::load<u32>(gabi::ea(this) + 0xB4) + 0x3C), this) &&
        LK_upperAnmIdx() == 0xFFFF /* checkNoUpperAnime() */ && mpAttnActorLockOn == nullptr &&
        ((gabi::load<u16>(gabi::ea(this) + 0x420) == 0 /* !checkPlayerDemoMode() */ && !(mModeFlg & 0x2000) /* ModeFlg_IN_SHIP */ &&
          gabi::load<u16>(dComIfGs_base_l() + 0x22) <= 6 /* dComIfGs_getLife() <= HD constant (GameCube m_HIO) */) ||
         gabi::load<s32>(gabi::ea(this) + 0x430) == 0x12 /* mDemo.getDemoMode() == DEMO_UNK_018_e */)) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x023DD12C, &daPy_lk_c::checkRestHPAnime);

/* 023DDEEC: getItemAnimeResource (unnamed by the matcher) */
u32 daPy_lk_c::getItemAnimeResource(u32 index /* u16, passed on as the caller extended it */) {
    WWHD_FUNC(0x023DDEEC, u32, this, index);
    u32 oldHeap = setAnimeHeap(LK_FIELD(u32, 0x4808) /* mpItemAnimeHeap */);
    /* HD: the item animation comes from the resident "LkAnm" archive */
    gabi::Local<SafeString> key;
    key->__vtbl = 0x10034B24;
    key->mStringTop = 0x10035450; /* "LkAnm" */
    u32 bas = gabi::call<u32>(0x026066C4, dComIfG_resControl(), key.get(), index);
    mDoExt_setCurrentHeap_l(oldHeap);
    return bas;
}
VERIFY(0x023DDEEC, &daPy_lk_c::getItemAnimeResource);

/* 023DF91C */
void daPy_lk_c::returnKeepItemData() {
    WWHD_FUNC(0x023DF91C, void, this);
    if (mKeepItem == 0x100 /* daPyItem_NONE_e */) {
        return;
    }
    gabi::call(LK_deleteEquipItem, this, FALSE);
    mEquipItem = mKeepItem;
    mKeepItem = 0x100;
    if (mEquipItem == 0x10B /* daPyItem_UNK10B_e */) {
        mEquipItem = 0x100;
    } else if (mEquipItem == 0x103 /* daPyItem_SWORD_e */) {
        gabi::call(LK_setSwordModel, this, TRUE);
    } else {
        gabi::call(LK_makeItemType, this);
    }
}
VERIFY(0x023DF91C, &daPy_lk_c::returnKeepItemData);

/* 023DF9C0 */
void daPy_lk_c::resetFootEffect() {
    WWHD_FUNC(0x023DF9C0, void, this);
    u32 fe = gabi::ea(this) + 0x65FC; /* mFootEffect[2] (0x4C each) */
    for (int idx = 0; idx < 2; idx++, fe += 0x4C) {
        /* getSmokeCallBack()->remove(), getOtherCallBack()->remove(): virtual (vtable at +0, slot 0x44) */
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(fe) + 0x44), fe);
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(fe + 0x20) + 0x44), fe + 0x20);
        gabi::store<s32>(fe + 0x48, -1); /* setID(-1) */
    }
    mFootEffectPosType = 0;
}
VERIFY(0x023DF9C0, &daPy_lk_c::resetFootEffect);

/* 023E048C */
u32 daPy_lk_c::getAnmData(int anm) {
    WWHD_FUNC(0x023E048C, u32, this, anm);
    u16 item = mEquipItem;
    if (item == 0x103 /* daPyItem_SWORD_e */) {
        if (anm < 0x1B) {
            return 0x1003655C + anm * 4; /* mSwordAnmIndexTable */
        }
    } else if (item == 0x101 /* daPyItem_BOKO_e */) {
        if (anm < 0x1B) {
            return 0x100365C8 + anm * 4; /* mBokoAnmIndexTable */
        }
    } else if (item == 0x33 /* dItemNo_SKULL_HAMMER_e */) {
        if (anm < 0x1B) {
            return 0x10036634 + anm * 4; /* mHammerAnmIndexTable */
        }
    } else if (item == 0x2D /* BOOMERANG */ || item == 0x34 /* DEKU_LEAF */ || item == 0x20 /* TELESCOPE */) {
        if (anm == 2 /* ANM_DASH */) {
            return 0x1003655C + anm * 4;
        }
    }
    return 0x100366A0 + anm * 8; /* &mAnmDataTable[anm].mAnmIdx */
}
VERIFY(0x023E048C, &daPy_lk_c::getAnmData);

/* ======== batch 3 ======== */
/* 025E1988 one-argument system sound (mDoAud_seStartSystem) */
static inline void mDoAud_seStartSystem_l(u32 id) { gabi::call(0x025E1988, id); }
/* 025E1918 mDoAud_subBgmStart */
static inline void mDoAud_subBgmStart_l(u32 id) { gabi::call(0x025E1918, id); }
/* play + 0x5C20: dComIfGp_setMesgBgmOn (1) / setMesgBgmOn2 (2) */
static inline void dComIfGp_setMesgBgm_l(u8 v) { gabi::store<u8>(dComIfGp_ea() + 0x5C20, v); }
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty; block at +0x2C, flags +4, matrices +0x10) */
static inline u32 lk_getAnmMtx(u32 model, s32 jnt) {
    u32 blk = gabi::load<u32>(model + 0x2C);
    gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
    return gabi::load<u32>(blk + 0x10) + jnt * 0x30;
}
/* J3DModelData::getJointNodePointer(i) (HD: inline entries of 0x1C at +8, count at +4; out of range -> entry 0) */
static inline u32 lk_jointNode(u32 data, u32 i) {
    u32 base = gabi::load<u32>(data + 8);
    if (gabi::load<u32>(data + 4) > i) {
        base += i * 0x1C;
    }
    return base;
}
/* 025E8154 mDoExt_brkAnm::init(data, brk, bool anmPlay, int mode, f32 speed, s16 start, s16 end, bool, int (stack)) */
static inline BOOL mDoExt_brkAnm_init_l(u32 anm, u32 data, u32 brk, s32 play, s32 mode, f32 speed, s32 start, s32 end, s32 b, s32 st) {
    return gabi::call<BOOL>(0x025E8154, anm, data, brk, play, mode, speed, start, end, b, st);
}
/* 025E7CE0 mDoExt_btkAnm::init(data, btk, bool anmPlay, int mode, f32 speed, s16 start, s16 end, bool, int (stack)) */
static inline BOOL mDoExt_btkAnm_init_l(u32 anm, u32 data, u32 btk, s32 play, s32 mode, f32 speed, s32 start, s32 end, s32 b, s32 st) {
    return gabi::call<BOOL>(0x025E7CE0, anm, data, btk, play, mode, speed, start, end, b, st);
}
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
static inline u32 operator_new_arr_align_l(u32 size, s32 align) { return gabi::call<u32>(0x0273AEC4, size, align); }

/* 023D4C78: HD-only, unnamed: loads a brk of "Link" and binds it to a brkAnm object */
u32 daPy_lk_c::initBrkAnm(u32 data, u32 brkAnm, u32 idx) {
    WWHD_FUNC(0x023D4C78, u32, this, data, brkAnm, idx);
    u32 res = lk_getLinkRes_l(idx);
    if (res == 0) {
        JUT_ASSERT_fail(gabi::at<const char>(0x1003512C), 0x5F0C, gabi::at<const char>(0x10035140));
    }
    s32 mode = anm_vf0C_l(res);
    mDoExt_brkAnm_init_l(brkAnm, data, res, 1, mode, 1.0f, 0, -1, gabi::load<u32>(brkAnm + 0x10) != 0, 0);
    gabi::call(0x025E83FC /* mDoExt_brkAnm::entry */, brkAnm, data, 0.0f);
    return res;
}
VERIFY(0x023D4C78, &daPy_lk_c::initBrkAnm);

/* 023D4D64: HD-only, unnamed: loads a btk of "Link" and binds it to a btkAnm object */
u32 daPy_lk_c::initBtkAnm(u32 data, u32 btkAnm, u32 idx) {
    WWHD_FUNC(0x023D4D64, u32, this, data, btkAnm, idx);
    u32 res = lk_getLinkRes_l(idx);
    if (res == 0) {
        JUT_ASSERT_fail(gabi::at<const char>(0x10035150), 0x5ED1, gabi::at<const char>(0x10035164));
        return res;
    }
    s32 mode = anm_vf0C_l(res);
    mDoExt_btkAnm_init_l(btkAnm, data, res, 1, mode, 1.0f, 0, -1, gabi::load<u32>(btkAnm + 0x68) != 0, 0);
    gabi::call(0x025E7FC4 /* mDoExt_btkAnm::entry */, btkAnm, data, 0.0f);
    return res;
}
VERIFY(0x023D4D64, &daPy_lk_c::initBtkAnm);

/* 023D689C: HD-only, unnamed joint callback: copies Link's joint matrix to the model at 0x5700 */
BOOL daPy_lk_c::HDJointCB(int jnt) {
    WWHD_FUNC(0x023D689C, BOOL, this, jnt);
    u32 src = lk_getAnmMtx(gabi::ea(mpCLModel.get()), jnt);
    u32 dst = lk_getAnmMtx(LK_FIELD(u32, 0x5700), jnt);
    mtx_copy(gabi::at<Mtx34>(dst), gabi::at<Mtx34>(src));
    PSMTXCopy(gabi::at<Mtx34>(lk_getAnmMtx(gabi::ea(mpCLModel.get()), jnt)), gabi::at<Mtx34>(0x104B4868) /* J3DSys::mCurrentMtx */);
    return TRUE;
}
VERIFY(0x023D689C, &daPy_lk_c::HDJointCB);

/* 023D84E0 */
void daPy_lk_c::drawMirrorLightModel() {
    WWHD_FUNC(0x023D84E0, void, this);
    if (resetFlg0() & 0x200000 /* daPyRFlg0_LIGHT_REFLECT */) {
        mDoMtx_stack_c::transS(5.0f, 7.0f, 0.0f); /* l_ms_light_local_start */
        mDoMtx_stack_c::YrotM(-0x8000);
        u32 shield = LK_FIELD(u32, 0xE80); /* mpEquippedShieldModel */
        gabi::call(0x028E9108 /* PSMTXConcat */, shield ? shield + 0xC8 : 0, mDoMtx_stack_c::get(), mDoMtx_stack_c::get());
        gabi::call(0x0252DA64 /* dDlst_mirrorPacket::update */, gabi::ea(this) + 0xF8C, mDoMtx_stack_c::get(), 0xFF, 60.0f);
        gabi::call(0x027F0E04 /* J3DDrawBuffer::entryImm */, gabi::load<u32>(dComIfGp_ea() + 0x5D7C) /* dComIfGd_getXluList() */,
                   gabi::ea(this) + 0xF8C, 0xFF);
        updateDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x43B4)) /* mpYmsls00Model */, 0);
    }
}
VERIFY(0x023D84E0, &daPy_lk_c::drawMirrorLightModel);

/* 023DBCA0: setGetItemSound (unnamed by the matcher) */
void daPy_lk_c::setGetItemSound(u32 itemNo, BOOL param_2) {
    WWHD_FUNC(0x023DBCA0, void, this, itemNo, param_2);
    if ((itemNo >= 1 && itemNo <= 5) /* GREEN..PURPLE_RUPEE */ || itemNo == 0xB3 /* TINGLE_RUPEE_1 */) {
        mDoAud_seStartSystem_l(0x8F5 /* JA_SE_ME_ITEM_GET_S */);
        dComIfGp_setMesgBgm_l(2);
    } else if ((s32)itemNo == 8 /* HEART_CONTAINER */ ||
               ((s32)itemNo == 7 /* HEART_PIECE */ && (gabi::load<u16>(dComIfGs_base_l() + 0x20) & 3) == 3 /* getMaxLife() % 4 */)) {
        mDoAud_subBgmStart_l(0x80000024 /* JA_BGM_GET_HEART */);
        dComIfGp_setMesgBgm_l(1);
    } else if (itemNo == 7 || itemNo == 6 || itemNo == 0xF || itemNo == 0x15 || itemNo == 0x1F) {
        mDoAud_subBgmStart_l(0x80000025 /* JA_BGM_ITEM_GET_S */);
        dComIfGp_setMesgBgm_l(1);
    } else if (param_2) {
        mDoAud_seStartSystem_l(0x8F5);
        dComIfGp_setMesgBgm_l(2);
    } else {
        mDoAud_subBgmStart_l(0x80000002 /* JA_BGM_ITEM_GET */);
        dComIfGp_setMesgBgm_l(1);
    }
}
VERIFY(0x023DBCA0, &daPy_lk_c::setGetItemSound);

/* 023DC6A4 */
BOOL daPy_lk_c::resetActAnimeUpper(int upperIdx, f32 i_morf) {
    WWHD_FUNC(0x023DC6A4, BOOL, this, upperIdx, i_morf);
    if (LK_upperAnmIdx() == 0x64 /* checkDashDamageAnime() */) {
        mDamageWaitTimer = 0x1E; /* HD: constant (GameCube m_HIO) */
    }
    u32 ratio = gabi::ea(this) + 0x5818 + upperIdx * 0x10; /* mAnmRatioUpper[upperIdx] */
    gabi::store<u32>(ratio + 4, 0);                        /* setAnmTransform(NULL) */
    for (s32 j = 0; j < gabi::load<s32>(ratio + 8); j++) { /* setRatio(0.0f): HD keeps one ratio per joint */
        gabi::store<f32>(gabi::load<u32>(ratio + 0xC) + j * 4, 0.0f);
    }
    gabi::store<u16>(gabi::ea(this) + 0x5868 + upperIdx * 0x10, 0xFFFF); /* m_anm_heap_upper[upperIdx].mIdx */
    gabi::call(0x027F2BC0 /* J3DFrameCtrl::init */, gabi::ea(this) + 0x58B8 + upperIdx * 0x10, 0);
    if (!(i_morf < 0.0f)) { /* i_morf >= 0.0f (taken on NaN) */
        gabi::call(0x025E3F3C /* mDoExt_MtxCalcOldFrame::initOldFrameMorf */, (u32)m_old_fdata, i_morf, 2, 0x1D);
    }
    resetPriTextureAnime();
    deleteArrow();
    return TRUE;
}
VERIFY(0x023DC6A4, &daPy_lk_c::resetActAnimeUpper);

/* 023DCB54 */
void daPy_lk_c::setDamageCurseEmitter() {
    WWHD_FUNC(0x023DCB54, void, this);
    if (gabi::load<u16>(0x101CEF16) != 1) { /* !daPy_dmEcallBack_c::checkCurse() */
        endDamageEmitter();
        u32 mtx = lk_getAnmMtx(gabi::ea(mpCLModel.get()), 0x1E /* CL_JNT_WAIST_JNT_e */);
        gabi::call(0x023D457C /* daPy_mtxFollowEcallBack_c::makeEmitter */, gabi::ea(this) + 0x67CC, 0x815B /* ID_AK_SN_BUBBLECURSE00 */, mtx,
                   &current.pos, 0);
        if (checkUpperReadyThrowAnime()) {
            resetActAnimeUpper(2 /* UPPER_MOVE2_e */, -1.0f);
            freeRopeItem();
        }
    }
    cancelNoDamageMode();
    gabi::store<u16>(0x101CEF16, 1);   /* daPy_dmEcallBack_c::setCurse(200) */
    gabi::store<s16>(0x101CEF14, 200);
}
VERIFY(0x023DCB54, &daPy_lk_c::setDamageCurseEmitter);

/* 023DCF8C */
void daPy_lk_c::freeGrabItem() {
    WWHD_FUNC(0x023DCF8C, void, this);
    u32 barrel = gabi::ea(mActorKeepGrab.mActor.get());
    if (barrel != 0) {
        fopAcM_cancelCarryNow(gabi::at<fopAc_ac_c>(barrel));
        gabi::store<s16>(barrel + 0x32C, 0); /* shape_angle.z */
        if (checkGrabBarrelSearch(0)) {
            gabi::store<s16>(barrel + 0x738, 0); /* daObjBarrel::Act_c::m61C */
        }
    }
    gabi::call(LK_actorKeep_clearData, &mActorKeepGrab);
    u16 upper = LK_upperAnmIdx();
    if (upper == 0x95 || upper == 0x96) { /* checkGrabAnime() */
        resetActAnimeUpper(2 /* UPPER_MOVE2_e */, 2.4f /* HD: constant (GameCube m_HIO) */);
    }
    LK_FIELD(f32, 0x3CC) = 0.0f; /* field_0x2b0 */
    m35CC = 0.0f;
    mNoResetFlg0 = mNoResetFlg0 & ~0x400u; /* offNoResetFlg0(daPyFlg0_UNK400) */
    u32 a = dComIfGp_ea() + 0x5CDC;     /* dComIfGp_clearPlayerStatus1(0, daPyStts1_UNK40000_e) */
    gabi::store<u32>(a, gabi::load<u32>(a) & ~0x40000u);
}
VERIFY(0x023DCF8C, &daPy_lk_c::freeGrabItem);

/* 023DD054 */
void daPy_lk_c::animeUpdate() {
    WWHD_FUNC(0x023DD054, void, this);
    u32 upperPack0 = gabi::load<u32>(gabi::ea(this) + 0x581C); /* getNowAnmPackUpper(UPPER_MOVE0_e) */
    u32 underPack0 = gabi::load<u32>(gabi::ea(this) + 0x57FC); /* getNowAnmPackUnder(UNDER_MOVE0_e) */
    u32 upperPack1 = gabi::load<u32>(gabi::ea(this) + 0x582C);
    u32 underPack1 = gabi::load<u32>(gabi::ea(this) + 0x580C);
    gabi::call(0x027F2FC4 /* J3DFrameCtrl::update */, &mFrameCtrlUnder[0]);
    fcpy_l(underPack0, gabi::ea(&mFrameCtrlUnder[0].mFrame)); /* setFrame (HD: frame at +0) */
    if (underPack1 != 0) {
        gabi::call(0x027F2FC4, &mFrameCtrlUnder[1]);
        fcpy_l(underPack1, gabi::ea(&mFrameCtrlUnder[1].mFrame));
    }
    if (upperPack0 != 0 && upperPack0 != underPack0) {
        gabi::call(0x027F2FC4, &mFrameCtrlUpper[0]);
        fcpy_l(upperPack0, gabi::ea(&mFrameCtrlUpper[0].mFrame));
    }
    if (upperPack1 != 0 && upperPack1 != underPack1) {
        gabi::call(0x027F2FC4, &mFrameCtrlUpper[1]);
        fcpy_l(upperPack1, gabi::ea(&mFrameCtrlUpper[1].mFrame));
    }
    u32 upperPack2 = gabi::load<u32>(gabi::ea(this) + 0x583C);
    if (upperPack2 != 0) {
        gabi::call(0x027F2FC4, &mFrameCtrlUpper[2]);
        fcpy_l(upperPack2, gabi::ea(&mFrameCtrlUpper[2].mFrame));
    }
    u32 morf = LK_FIELD(u32, 0x44D0); /* mpParachuteFanMorf */
    if (morf != 0) {
        gabi::call(0x025E535C /* mDoExt_McaMorf::play */, morf, 0, 0, 0);
    }
}
VERIFY(0x023DD054, &daPy_lk_c::animeUpdate);

/* 023DDF9C */
void daPy_lk_c::setSmallFanModel() {
    WWHD_FUNC(0x023DDF9C, void, this);
    u32 bck = getItemAnimeResource(0x85 /* LKANM_BCK_FANWAIT */);
    u32 oldHeap = gabi::ea(setItemHeap());
    u32 tmp_modelData = initModel(mpEquipItemModel_ea, 0x1B /* LINK_BDL_FANSMALL */, 0x37221222);
    BOOL ret = gabi::call<BOOL>(0x025E8508 /* mDoExt_bckAnm::init */, gabi::ea(this) + 0x4444 /* mSwordAnim */, tmp_modelData, bck, 0, 2,
                                1.0f, 0, -1, 0);
    if (!ret) {
        JUT_ASSERT_fail(gabi::at<const char>(0x1003545C), 0x126, gabi::at<const char>(0x10035458));
    }
    mDoExt_setCurrentHeap_l(oldHeap);
    gabi::store<u32>(gabi::load<u32>(mpEquipItemModel_ea) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    gabi::store<u32>(lk_jointNode(tmp_modelData, 2) + 8, 0x023D6604); /* FANSMA_JNT: daPy_fanJointCallback */
    gabi::store<u32>(lk_jointNode(tmp_modelData, 3) + 8, 0x023D6604); /* FANSMB_JNT */
    m355C = 0;
}
VERIFY(0x023DDF9C, &daPy_lk_c::setSmallFanModel);

/* 023DE654 */
void daPy_lk_c::setHookshotModel() {
    WWHD_FUNC(0x023DE654, void, this);
    u32 bck = getItemAnimeResource(0xA5 /* LKANM_BCK_HOOKSHOTA */);
    u32 oldHeap = gabi::ea(setItemHeap());
    u32 tmp_modelData = initModel(mpEquipItemModel_ea, 0x1E /* LINK_BDL_HOOKSHOT */, 0x37221222);
    BOOL ret = gabi::call<BOOL>(0x025E8508, gabi::ea(this) + 0x4444, tmp_modelData, bck, 0, 2, 1.0f, 0, -1, 0);
    if (!ret) {
        JUT_ASSERT_fail(gabi::at<const char>(0x100354B8) /* "d_a_player_hook.inc" */, 100, gabi::at<const char>(0x100354B4));
    }
    mDoExt_setCurrentHeap_l(oldHeap);
    m35EC = 0.0f;
}
VERIFY(0x023DE654, &daPy_lk_c::setHookshotModel);

/* 023DEDE4 */
void daPy_lk_c::setPhotoBoxModel() {
    WWHD_FUNC(0x023DEDE4, void, this);
    u32 oldHeap = gabi::ea(setItemHeap());
    initModel(mpEquipItemModel_ea, 0x17 /* LINK_BDL_CAMERA */, 0x37221222);
    mDoExt_setCurrentHeap_l(oldHeap);
    u32 data = gabi::load<u32>(gabi::load<u32>(mpEquipItemModel_ea) + 0xAC); /* getModelData() */
    u32 shape = gabi::load<u32>(gabi::load<u32>(lk_jointNode(data, 2 /* CAMERA_JNT_FRASH_e */) + 0x10) + 8); /* getMesh()->getShape() */
    if (mEquipItem == 0x23 /* dItemNo_PICTO_BOX_e */) {
        gabi::store<u8>(shape + 4, 0); /* hide() */
    } else {
        gabi::store<u8>(shape + 4, 1); /* show() */
    }
}
VERIFY(0x023DEDE4, &daPy_lk_c::setPhotoBoxModel);

/* 023DEF04 */
void daPy_lk_c::setTactModel() {
    WWHD_FUNC(0x023DEF04, void, this);
    if (mEquipItem == 0x22 /* dItemNo_WIND_WAKER_e */) {
        return;
    }
    gabi::call(LK_deleteEquipItem, this, FALSE);
    mEquipItem = 0x22;
    u32 oldHeap = gabi::ea(setItemHeap());
    u32 modelData = initModel(mpEquipItemModel_ea, 0x47 /* LINK_BDL_TAKT */, 0x37221222);
    /* HD: the brk comes from the resident "LkAnm" archive into the brkAnm object at 0x48F8 */
    gabi::Local<SafeString> key;
    key->__vtbl = 0x10034B24;
    key->mStringTop = 0x1003550C;
    u32 brk = gabi::call<u32>(0x026066C4, dComIfG_resControl(), key.get(), 0x154 /* LKANM_BRK_TTAKT */);
    s32 mode = anm_vf0C_l(brk);
    mDoExt_brkAnm_init_l(gabi::ea(this) + 0x48F8, modelData, brk, 1, mode, 1.0f, 0, -1, LK_FIELD(u32, 0x4908) != 0, 0);
    gabi::call(0x025E83FC /* mDoExt_brkAnm::entry */, gabi::ea(this) + 0x48F8, modelData, 0.0f);
    mDoExt_setCurrentHeap_l(oldHeap);
}
VERIFY(0x023DEF04, &daPy_lk_c::setTactModel);

/* 023DF020 */
void daPy_lk_c::setHammerModel() {
    WWHD_FUNC(0x023DF020, void, this);
    u32 bck = getItemAnimeResource(0x97 /* LKANM_BCK_HAMMERDAM */);
    u32 oldHeap = gabi::ea(setItemHeap());
    u32 tmp_modelData = initModel(mpEquipItemModel_ea, 0x1C /* LINK_BDL_HAMMER */, 0x37221222);
    BOOL ret = gabi::call<BOOL>(0x025E8508, gabi::ea(this) + 0x4444, tmp_modelData, bck, 0, 2, 1.0f, 0, -1, 0);
    if (!ret) {
        JUT_ASSERT_fail(gabi::at<const char>(0x10035518), 0x20, gabi::at<const char>(0x10035514));
    }
    u32 blur = mpSwBlur; /* HD: daPy_swBlur_c is allocated */
    gabi::store<u32>(blur + 0xAC, operator_new_arr_align_l(0x4800, 0x20)); /* mpPosBuffer = new (0x20) Vec[2 * 0x300] */
    mDoExt_setCurrentHeap_l(oldHeap);
    m35EC = 0.0f;
}
VERIFY(0x023DF020, &daPy_lk_c::setHammerModel);

/* 023E0530 */
void daPy_lk_c::getUnderUpperAnime(u32 anmIndex, u32 pUnderBck, u32 pUpperBck, int r7, u32 bufferSize) {
    WWHD_FUNC(0x023E0530, void, this, anmIndex, pUnderBck, pUpperBck, r7, bufferSize);
    u32 under = gabi::ea(this) + 0x5848 + r7 * 0x10; /* m_anm_heap_under[r7] */
    u16 underIdx = gabi::load<u16>(anmIndex + 0);
    if (gabi::load<u16>(under) != underIdx) {
        gabi::store<u32>(pUnderBck, getAnimeResource(under, underIdx, bufferSize));
    } else {
        gabi::store<u32>(pUnderBck, gabi::load<u32>(gabi::ea(this) + 0x57FC + r7 * 0x10)); /* getNowAnmPackUnder */
    }
    u16 upperIdx = gabi::load<u16>(anmIndex + 2);
    if (gabi::load<u16>(anmIndex + 0) != upperIdx) {
        if (bufferSize == 0xB400) {
            bufferSize = 0x4800;
        }
        u32 upper = gabi::ea(this) + 0x5868 + r7 * 0x10;
        if (gabi::load<u16>(upper) != upperIdx) {
            gabi::store<u32>(pUpperBck, getAnimeResource(upper, upperIdx, bufferSize));
        } else {
            gabi::store<u32>(pUpperBck, gabi::load<u32>(gabi::ea(this) + 0x581C + r7 * 0x10)); /* getNowAnmPackUpper */
        }
    } else {
        gabi::store<u32>(pUpperBck, 0);
        gabi::store<u16>(gabi::ea(this) + 0x5868 + r7 * 0x10, 0xFFFF);
    }
}
VERIFY(0x023E0530, &daPy_lk_c::getUnderUpperAnime);

/* ======== batch 4 ======== */
/* 023DD1D8: checkBossBgm (unnamed by the matcher) */
BOOL daPy_lk_c::checkBossBgm() {
    WWHD_FUNC(0x023DD1D8, BOOL, this);
    u32 mainBgm = gabi::call<u32>(0x025E1E54 /* mDoAud_checkPlayingMainBgmFlag */);
    u32 subBgm = gabi::call<u32>(0x025E1E60 /* mDoAud_checkPlayingSubBgmFlag */);
    /* HD BGM ids (the GameCube list: GOMA .. BGN_HAYAMUSHI, MBOSS .. DIOCTA_2) */
    if (mainBgm == 0x80000003 || mainBgm == 0x80000005 || mainBgm == 0x80000010 || mainBgm == 0x80000014 || mainBgm == 0x80000015 ||
        mainBgm == 0x80000023 || mainBgm == 0x80000029 || (mainBgm >= 0x80000048 && mainBgm <= 0x8000004D) || mainBgm == 0x80000054 ||
        (mainBgm >= 0x80000059 && mainBgm <= 0x8000005B) || subBgm == 0x80000019 || subBgm == 0x8000001A || subBgm == 0x8000002B ||
        subBgm == 0x80000041 || subBgm == 0x80000046 || subBgm == 0x80000047) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x023DD1D8, &daPy_lk_c::checkBossBgm);

/* 023DDD48 */
void daPy_lk_c::resetDemoTextureAnime() {
    WWHD_FUNC(0x023DDD48, void, this);
    u32 h = gabi::ea(this) + 0x65D0; /* m_tex_anm_heap: mIdx, field_0x2, field_0x4, field_0x6 */
    if (gabi::load<u16>(h + 4) != 0xFFFF) {
        u16 idx2 = gabi::load<u16>(h + 2);
        gabi::store<u16>(h + 6, 0xFFFF);
        gabi::store<u16>(h + 4, 0xFFFF);
        if (idx2 != 0xFFFF) {
            gabi::call(LK_setTextureAnimeResource, this, loadTextureAnimeResource(idx2, FALSE), 0);
        } else if (gabi::load<u16>(h + 0) != 0xFFFF) {
            gabi::call(LK_setTextureAnimeResource, this, loadTextureAnimeResource(gabi::load<u16>(h + 0), FALSE), 0);
        } else {
            gabi::call(LK_setTextureAnime, this, 0, 0);
        }
    }
    h = gabi::ea(this) + 0x65E0; /* m_tex_scroll_heap */
    if (gabi::load<u16>(h + 4) != 0xFFFF) {
        u16 idx2 = gabi::load<u16>(h + 2);
        gabi::store<u16>(h + 6, 0xFFFF);
        gabi::store<u16>(h + 4, 0xFFFF);
        if (idx2 != 0xFFFF) {
            setTextureScrollResource(loadTextureScrollResource(idx2, FALSE), 0);
        } else if (gabi::load<u16>(h + 0) != 0xFFFF) {
            setTextureScrollResource(loadTextureScrollResource(gabi::load<u16>(h + 0), FALSE), 0);
        } else {
            gabi::call(LK_setTextureAnime, this, 0, 0);
        }
    }
}
VERIFY(0x023DDD48, &daPy_lk_c::resetDemoTextureAnime);

/* 023E0660 */
void daPy_lk_c::initSeAnime() {
    WWHD_FUNC(0x023E0660, void, this);
    u32 fc = mpSeAnmFrameCtrl;
    if (fc != 0) { /* HD: the frame control is only used inside the NULL check */
        s32 dir = gabi::load<f32>(fc + 0) < 0.0f ? -1 : 1;
        f32 loop = (f32)gabi::load<s16>(fc + 0xC);
        /* HD: JAIZelAnime::initActorAnimSound(buffer (0x658C), dir, f32 loop) */
        gabi::call(0x0201C020, gabi::ea(this) + 0x64F0, (u32)LK_FIELD(u32, 0x658C), dir, loop);
        fc = mpSeAnmFrameCtrl;
        if (gabi::load<u8>(fc + 0xE) == 2 /* J3DFrameCtrl::EMode_LOOP */) {
            f32 position = gabi::fsubs_ppc(gabi::load<f32>(fc + 4), gabi::load<f32>(fc + 0));
            if (!(position > (f32)gabi::load<s16>(fc + 8))) {
                position = gabi::fsubs_ppc((f32)gabi::load<s16>(fc + 0xA), position);
            } else if (!(position < (f32)gabi::load<s16>(fc + 0xA))) {
                position = gabi::fsubs_ppc(position, (f32)gabi::load<s16>(fc + 0xA));
            }
            gabi::call(0x0201BFD8 /* JAIZelAnime::setPlayPosition */, gabi::ea(this) + 0x64F0, position);
        }
    }
}
VERIFY(0x023E0660, &daPy_lk_c::initSeAnime);

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
    WWHD_FUNC(0x023D6230, BOOL, this, param_0);
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
VERIFY(0x023D6230, &daPy_lk_c::bowJointCB);

/* 023D641C */
BOOL daPy_lk_c::fanJointCB(int param_0) {
    WWHD_FUNC(0x023D641C, BOOL, this, param_0);
    if (param_0 == 2 /* FANSMALL_JNT_FANSMA_JNT_e */) {
        gabi::call(0x025F181C /* mDoMtx_ZrotS */, LK_MTX_STACK, (s32)m3558);
    } else {
        gabi::call(0x025F181C, LK_MTX_STACK, (s32)m355A);
    }
    PSMTXConcat_l(LK_equipAnmMtx(param_0), LK_MTX_STACK, LK_MTX_STACK);
    mtx_copy_q(LK_equipAnmMtx(param_0) /* FANSMALL_JNT_FANSMROOT_JNT_e * param_0 */, LK_MTX_STACK);
    PSMTXCopy_l(LK_MTX_STACK, LK_J3DSYS_CURMTX);
    return TRUE;
}
VERIFY(0x023D641C, &daPy_lk_c::fanJointCB);

/* 023D6650 */
BOOL daPy_lk_c::parachuteJointCB(int param_0) {
    WWHD_FUNC(0x023D6650, BOOL, this, param_0);
    if (param_0 == 3 /* FANB_JNT_LARMB_e */ || param_0 == 9 /* FANB_JNT_RARMB_e */) {
        mDoMtx_stack_c::transS(m3600, 0.0f, 0.0f);
    } else if (param_0 == 1 /* FANB_JNT_LROOT_e */ || param_0 == 7 /* FANB_JNT_RROOT_e */) {
        gabi::call(0x025F1884 /* mDoMtx_YrotS */, LK_MTX_STACK, (s32)m355E);
    } else {
        return TRUE;
    }
    PSMTXConcat_l(LK_equipAnmMtx(param_0), LK_MTX_STACK, LK_MTX_STACK);
    mtx_copy_q(LK_equipAnmMtx(param_0) /* FANB_JNT_LROOT_e * param_0 */, LK_MTX_STACK);
    PSMTXCopy_l(LK_MTX_STACK, LK_J3DSYS_CURMTX);
    return TRUE;
}
VERIFY(0x023D6650, &daPy_lk_c::parachuteJointCB);

/* 023DCD30 */
void daPy_lk_c::setAtnList() {
    WWHD_FUNC(0x023DCD30, void, this);
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
    if (gabi::call<BOOL>(0x024EDFCC, att) || (gabi::load<u32>(att + 0x20) & 0x20000000)) {
        u32 a = dComIfGp_ea() + 0x5CD8; /* dComIfGp_clearPlayerStatus0(0, daPyStts0_BOOMERANG_WAIT_e) */
        gabi::store<u32>(a, gabi::load<u32>(a) & ~0x400000u);
    }
    if (mActorKeepThrow.mActor != nullptr && (gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x400000)) {
        mpAttnActorA = mActorKeepThrow.mActor;
        mpAttnActorLockOn = mpAttnActorA;
    } else {
        att = mpAttention;
        if (gabi::call<BOOL>(0x024EDFCC, att) || (gabi::load<u32>(att + 0x20) & 0x20000000)) {
            mpAttnEntryA = gabi::call<u32>(0x024EE058 /* dAttention_c::GetLockonList */, (u32)mpAttention, 0);
            if (mpAttnEntryA != 0 && gabi::call<BOOL>(0x024EDFCC, (u32)mpAttention)) {
                fopAc_ac_c* actor = gabi::call<fopAc_ac_c*>(0x024EBA14 /* dAttList_c::getActor */, (u32)mpAttnEntryA);
                mpAttnActorLockOn = actor;
                mpAttnActorA = actor;
                setResetFlg0(resetFlg0() | 0x10000); /* onResetFlg0(daPyRFlg0_ATTENTION_LOCK) */
            }
            m34E6 = shape_angle.y;
        }
    }
    if (mpAttnActorLockOn == nullptr) {
        u32 e = gabi::call<u32>(0x024EE090 /* dAttention_c::getActionBtnB */, (u32)mpAttention);
        mpAttnEntryA = e;
        if (e != 0) {
            fopAc_ac_c* actor = gabi::call<fopAc_ac_c*>(0x024EBA14, e);
            mpAttnActorA = actor;
            mpAttnActorAction = actor;
        }
    }
    u32 e = gabi::call<u32>(0x024EE2F4 /* dAttention_c::getActionBtnX */, (u32)mpAttention);
    mpAttnEntryX = e;
    if (e != 0) {
        mpAttnActorX = gabi::call<fopAc_ac_c*>(0x024EBA14, e);
    }
    e = gabi::call<u32>(0x024EE350 /* dAttention_c::getActionBtnY */, (u32)mpAttention);
    mpAttnEntryY = e;
    if (e != 0) {
        mpAttnActorY = gabi::call<fopAc_ac_c*>(0x024EBA14, e);
    }
    e = gabi::call<u32>(0x024EE3AC /* dAttention_c::getActionBtnZ */, (u32)mpAttention);
    mpAttnEntryZ = e;
    if (e != 0) {
        mpAttnActorZ = gabi::call<fopAc_ac_c*>(0x024EBA14, e);
    }
    e = gabi::call<u32>(0x024EE408 /* HD: the fourth item button's entry */, (u32)mpAttention);
    mpAttnEntryHD = e;
    if (e != 0) {
        mpAttnActorHD = gabi::call<fopAc_ac_c*>(0x024EBA14, e);
    }
}
VERIFY(0x023DCD30, &daPy_lk_c::setAtnList);

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
        u32 x = J3DModelData_HD_027F3F8C_l(md);
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

/* 023DC110: setTextureAnimeResource (unnamed by the matcher) */
void daPy_lk_c::setTextureAnimeResource(u32 btp, int r31) {
    WWHD_FUNC(0x023DC110, void, this, btp, r31);
    mpAnmTexPatternData = btp;
    m3530 = (u16)r31;
    /* HD: the texture pattern drives two animation objects (0x644 and 0x69C) */
    gabi::call(0x027E12B4, gabi::ea(this) + 0x644, gabi::load<u32>(btp + 0xC));
    gabi::call(0x027E0C88, gabi::ea(this) + 0x644, J3DModelData_HD_027F3F8C_l(gabi::ea(mpCLModelData.get())));
    gabi::call(0x027E12B4, gabi::ea(this) + 0x69C, gabi::load<u32>((u32)mpAnmTexPatternData + 0xC));
    gabi::call(0x027E0C88, gabi::ea(this) + 0x69C, J3DModelData_HD_027F3F8C_l(gabi::ea(mpCLModelData.get())));
    lk_linkTexMaterials(this, 0x100353EC);
}
VERIFY(0x023DC110, &daPy_lk_c::setTextureAnimeResource);

/* 023DC7AC */
void daPy_lk_c::deleteEquipItem(BOOL param_1) {
    WWHD_FUNC(0x023DC7AC, void, this, param_1);
    u32 equip_actor = gabi::ea(mActorKeepEquip.mActor.get());
    u16 item;
    if (param_1) {
        item = mEquipItem;
        if (item == 0x100 /* daPyItem_NONE_e */ || item == 0x101 /* daPyItem_BOKO_e */) {
            goto not_sword;
        }
        seStartOnlyReverb(item == 0x103 ? 0x280A /* JA_SE_LK_SW_PUTIN_S */ : 0x2810 /* JA_SE_LK_ITEM_TAKEOUT */);
    }
    item = mEquipItem;
    if (item == 0x103 /* daPyItem_SWORD_e */) {
        gabi::call(0x025E1D08 /* mDoAud_bgmSetSwordUsing */, 0, (u32)item); /* r4 still holds the item (the callee reads it) */
        item = mEquipItem;
        goto anime;
    }
not_sword:
    if (equip_actor != 0) {
        if (item == 0x101) {
            fopAcM_cancelCarryNow(gabi::at<fopAc_ac_c>(equip_actor));
        } else {
            fopAcM_delete(gabi::at<fopAc_ac_c>(equip_actor));
        }
        item = mEquipItem;
    }
anime: {
    bool reset = false;
    if (item == 0x25 /* dItemNo_GRAPPLING_HOOK_e */) {
        if (checkRopeAnime()) {
            reset = true;
        } else {
            item = mEquipItem;
        }
    }
    if (!reset) {
        if (item == 0x2D /* dItemNo_BOOMERANG_e */) {
            reset = checkBoomerangAnime();
        } else if (item == 0x2F /* dItemNo_HOOKSHOT_e */) {
            reset = LK_upperAnmIdx() == 0xA7; /* checkHookshotReadyAnime() */
        }
        if (!reset && checkBowItem(item) && checkBowAnime()) {
            reset = true;
        }
    }
    if (reset) {
        resetActAnimeUpper(2 /* UPPER_MOVE2_e */, -1.0f);
    }
}
    gabi::call(LK_actorKeep_clearData, &mActorKeepEquip);
    u32 bckAnm = LK_FIELD(u32, 0x44CC); /* mSwordAnim.getBckAnm() */
    mEquipItem = 0x100;
    gabi::store<u32>(mpEquipItemModel_ea, 0);
    if (bckAnm != 0) {
        gabi::call(0x025E871C /* mDoExt_bckAnm::changeBckOnly */, gabi::ea(this) + 0x4444, 0);
    }
    LK_FIELD(u32, 0x44D0) = 0; /* mpParachuteFanMorf */
    /* HD: the item btk/brk animations are objects, re-initialised without data */
    mDoExt_btkAnm_init_l(gabi::ea(this) + 0x4810, 0, 0, 1, 2, 1.0f, 0, -1, 0, 0); /* mpEquipItemBtk */
    LK_FIELD(u32, 0x4970) = 0; /* mpBottleContentsModel */
    LK_FIELD(u32, 0x4974) = 0; /* mpBottleCapModel */
    mDoExt_brkAnm_init_l(gabi::ea(this) + 0x48F8, 0, 0, 1, 2, 1.0f, 0, -1, 0, 0); /* mpEquipItemBrk */
    mDoExt_btkAnm_init_l(gabi::ea(this) + 0x4884, 0, 0, 1, 2, 1.0f, 0, -1, 0, 0); /* mpSwordBtk */
    LK_FIELD(u32, 0x4978) = 0; /* mpSwordModel1 */
    gabi::call(0x023D4538 /* daPy_mtxFollowEcallBack_c::end */, &m3454);
    if (LK_FIELD(u32, 0x497C) != 0) { /* HD: clears three fields of the sword tip model's data */
        gabi::store<u32>(gabi::load<u32>(LK_FIELD(u32, 0x497C) + 0xAC) + 0x44, 0);
        gabi::store<u32>(gabi::load<u32>(LK_FIELD(u32, 0x497C) + 0xAC) + 0x48, 0);
        gabi::store<u32>(gabi::load<u32>(LK_FIELD(u32, 0x497C) + 0xAC) + 0x40, 0);
    }
    u32 f1 = noResetFlg1();
    LK_FIELD(u32, 0x497C) = 0; /* mpSwordTipStabModel */
    setNoResetFlg1(f1 & ~0x200000u); /* offNoResetFlg1(daPyFlg1_UNK200000) */
}
VERIFY(0x023DC7AC, &daPy_lk_c::deleteEquipItem);

/* 023DE7E8 */
BOOL daPy_lk_c::setActAnimeUpper(u32 bckIdx, int upperIdx, f32 rate, f32 start, int end, f32 i_morf) {
    WWHD_FUNC(0x023DE7E8, BOOL, this, bckIdx, upperIdx, rate, start, end, i_morf);
    u32 bck = gabi::call<u32>(0x023DE71C /* getAnimeResource */, this, gabi::ea(this) + 0x5868 + upperIdx * 0x10, bckIdx, 0x2400);
    resetPriTextureAnime();
    u32 ratio = gabi::ea(this) + 0x5818 + upperIdx * 0x10;
    gabi::store<u32>(ratio + 4, bck); /* setAnmTransform(bck) */
    for (s32 j = 0; j < gabi::load<s32>(ratio + 8); j++) { /* setRatio(1.0f) */
        gabi::store<f32>(gabi::load<u32>(ratio + 0xC) + j * 4, 1.0f);
    }
    if (end < 0) {
        end = gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(bck + 4) + 0x14), bck); /* getFrameMax() */
    }
    u32 fc = gabi::ea(this) + 0x58B8 + upperIdx * 0x10;
    if (rate < 0.0f) {
        f32 frame = (f32)end - 0.001f;
        u32 attr = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(bck + 4) + 0xC), bck); /* getAttribute() */
        gabi::call(0x023DE788 /* setFrameCtrl */, this, fc, attr, (s32)(s16)gabi::ftoi(start), end, rate, frame);
        gabi::store<f32>(bck, frame); /* setFrame (HD: frame at +0) */
    } else {
        u32 attr = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(bck + 4) + 0xC), bck);
        gabi::call(0x023DE788, this, fc, attr, (s32)(s16)gabi::ftoi(start), end, rate, start);
        gabi::store<f32>(bck, start);
    }
    if (!(i_morf < 0.0f)) {
        gabi::call(0x025E3F3C /* mDoExt_MtxCalcOldFrame::initOldFrameMorf */, (u32)m_old_fdata, i_morf, 2, 0x1D);
    }
    return TRUE;
}
VERIFY(0x023DE7E8, &daPy_lk_c::setActAnimeUpper);

/* 023E07E4 */
void daPy_lk_c::setSeAnime(u32 anmRes, u32 anmHeap, u32 frameCtrl) {
    WWHD_FUNC(0x023E07E4, void, this, anmRes, anmHeap, frameCtrl);
    /* HD: mpSeAnm (0x6938) is a sead::SafeString naming the SE animation; the name is stored behind the bck data */
    u32 buf = gabi::load<u32>(anmRes + 8);
    gabi::Local<SafeString> name;
    name->mStringTop = lk_relPtr(buf + 4);
    name->__vtbl = 0x10034B24;
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
    if (same && m34F0 == gabi::load<u16>(anmHeap + 6) &&
        !(gabi::fmuls_ppc(mSeAnmRate, gabi::load<f32>(frameCtrl + 0)) < 0.0f)) {
        mpSeAnmFrameCtrl = frameCtrl;
        return;
    }
    gabi::Local<SafeString> key;
    key->__vtbl = 0x10034B24;
    key->mStringTop = lk_relPtr(gabi::load<u32>(anmRes + 8) + 4);
    u32 sanm = gabi::call<u32>(0x0260702C /* the SE animation data of the resource */, dComIfG_resControl(), key.get(), anmRes + 0xC);
    LK_FIELD(u32, 0x658C) = sanm;
    if (sanm == 0) {
        resetSeAnime();
        gabi::call(0x0201C020 /* JAIZelAnime::initActorAnimSound */, gabi::ea(this) + 0x64F0, 0, 1, 0.0f);
        return;
    }
    mpSeAnmFrameCtrl = frameCtrl;
    mpSeAnm = lk_relPtr(gabi::load<u32>(anmRes + 8) + 4);
    m34F0 = gabi::load<u16>(anmHeap + 6);
    fcpy_l(gabi::ea(&mSeAnmRate), frameCtrl + 0); /* mSeAnmRate = frameCtrl->getRate() */
    initSeAnime();
}
VERIFY(0x023E07E4, &daPy_lk_c::setSeAnime);

/* ======== batch 7 ======== */
/* 023D813C */
static void daPy_sightPacket_setSight(u32 self) {
    WWHD_FUNC(0x023D813C, void, self);
    /* HD: mDoLib_project takes the viewport, whose height scales the sight (480 lines = 1) */
    gabi::Local<u8[0x28]> vp; /* 025F1084 copies a 0x28-byte view/viewport record (5 x 2 words); was 0x18 (game test: frame guard) */
    gabi::call(0x025F1084 /* the current viewport */, vp.get());
    f32 scr = gabi::load<f32>(gabi::ea(vp.get()) + 0xC) / 480.0f;
    gabi::Local<cXyz> proj;
    gabi::call(0x025F1018 /* mDoLib_project */, self + 8 /* mPos */, proj.get(), vp.get());
    mDoMtx_stack_c::transS(proj->x, proj->y, proj->z);
    if (gabi::load<u8>(self + 5) /* mLockFlag */) {
        u8 frame = gabi::load<u8>(self + 6); /* mFrame */
        f32 var_f2;
        f32 var_f31;
        if (frame < 13) {
            var_f2 = 1.0f - (f32)(s32)frame / 13.0f;
            gabi::store<u8>(self + 7, (u8)gabi::ftoi(gabi::fmadds(30.0f, var_f2, 150.0f))); /* mLockAlpha */
            var_f31 = -var_f2;
        } else {
            var_f2 = (f32)(s32)(frame - 13) / 13.0f;
            gabi::store<u8>(self + 7, (u8)gabi::ftoi(gabi::fmadds(30.0f, var_f2, 150.0f)));
            var_f31 = var_f2;
        }
        f32 div = 4.0f / scr;
        u32 img = gabi::load<u32>(self + 0x44); /* mpImg */
        f32 w = (f32)gabi::load<u16>(img + 2);
        f32 h = (f32)gabi::load<u16>(img + 4);
        f32 temp_f3 = gabi::fmadds(0.35f, var_f2, 0.65f);
        mDoMtx_stack_c::scaleM((temp_f3 * w) / div, (temp_f3 * h) / div, temp_f3);
        /* cM_ssin(0x4000 * var_f31): the sin/cos table at 0x104A44F8 (8 bytes per entry) */
        u16 a = (u16)gabi::ftoi(16384.0f * var_f31);
        f32 s = gabi::load<f32>(0x104A44F8 + (a >> 3) * 8);
        gabi::call(0x025F1C5C /* mDoMtx_ZrotM */, LK_MTX_STACK, (s32)(s16)gabi::ftoi(32768.0f * s));
    } else {
        f32 sc = 10.0f * scr;
        mDoMtx_stack_c::scaleM(sc, sc, sc);
    }
    PSMTXCopy_l(LK_MTX_STACK, self + 0x14 /* mMtx */);
    u32 list = dComIfGp_ea() + 0x5D30; /* dComIfGd_set2DXlu(this) */
    gabi::call(0x0252CDC0 /* dDlst_list_c::set */, list, list + 0x264, list + 0x268, self);
}
VERIFY(0x023D813C, daPy_sightPacket_setSight);

/* 023DFB18 */
void daPy_lk_c::setBgCheckParam() {
    WWHD_FUNC(0x023DFB18, void, this);
    /* mAcchCir[i] (0x40 each from 0x74C): SetWallH stores +0x30, GetWallR reads +0x34 */
    u32 cir0 = gabi::ea(this) + 0x74C, cir1 = cir0 + 0x40, cir2 = cir0 + 0x80;
#define LK_setWallR(c, r) gabi::call(0x024EFF3C /* dBgS_AcchCir::SetWallR */, (c), (f32)(r))
#define LK_setWallH(c, h) gabi::store<f32>((c) + 0x30, (h))
    s32 proc = mCurProc;
    LK_setWallH(cir2, 125.0f);
    LK_setWallH(cir1, 89.9f);
    LK_setWallH(cir0, 30.1f);
    if (proc == 0x7C /* daPyProc_ROPE_SWING_START_e */) {
        LK_setWallH(cir2, 0.0f);
        LK_setWallH(cir0, -125.0f);
        LK_setWallH(cir1, -89.9f);
        /* HD: mAcchCir[0]'s radius is left as it is */
        LK_setWallR(cir1, gabi::load<f32>(cir0 + 0x34));
        LK_setWallR(cir2, gabi::load<f32>(cir0 + 0x34));
        return;
    }
    u32 mode = 0;
    if (proc == 0x6A /* daPyProc_LARGE_DAMAGE_WALL_e */ || ((mode = mModeFlg) & 0x410000) /* ModeFlg_CLIMB | ModeFlg_LADDER */) {
        LK_setWallR(cir0, 5.0f);
    } else if (mode & 0x200000 /* ModeFlg_PUSHPULL */) {
        LK_setWallR(cir0, 40.0f);
    } else if (mode & 0x10 /* ModeFlg_WHIDE */) {
        LK_setWallR(cir0, 8.5f); /* HD: constant (GameCube m_HIO) */
        if (mNoResetFlg0 & 0x10000 /* daPyFlg0_UNK10000 */) {
            LK_setWallH(cir2, 89.9f);
        }
    } else if (mode & 0x40000 /* ModeFlg_SWIM */) {
        LK_setWallR(cir0, gabi::load<f32>(0x1046CCE8)); /* HD: a global (GameCube 67.5f) */
        LK_setWallH(cir1, 0.0f);
        LK_setWallH(cir0, -5.0f);
        LK_setWallH(cir2, 20.0f);
    } else if ((mode & 0x1000000 /* ModeFlg_CRAWL */) && proc != 0x12 /* daPyProc_CRAWL_END_e */) {
        LK_setWallR(cir0, 30.0f);
        LK_setWallH(cir1, 50.0f);
        LK_setWallH(cir2, 50.0f);
        LK_setWallH(cir0, 10.0f);
    } else if (mode & 0x20 /* ModeFlg_HANG */) {
        LK_setWallR(cir0, 12.5f);
        LK_setWallH(cir0, 25.0f);
        LK_setWallH(cir2, 25.0f);
        LK_setWallH(cir1, 25.0f);
    } else if (LK_FIELD(f32, 0x3CC) < 0.0f /* checkGrabWear() */) {
        LK_setWallR(cir0, 50.0f);
    } else {
        LK_setWallR(cir0, 35.0f);
    }
    LK_setWallR(cir1, gabi::load<f32>(cir0 + 0x34));
    LK_setWallR(cir2, gabi::load<f32>(cir0 + 0x34));
#undef LK_setWallR
#undef LK_setWallH
}
VERIFY(0x023DFB18, &daPy_lk_c::setBgCheckParam);

/* 023D9074: hideHatAndBackle (unnamed by the matcher) */
void daPy_lk_c::hideHatAndBackle(u32 mtl) {
    WWHD_FUNC(0x023D9074, void, this, mtl);
    /* HD: the materials are found by name (three sead::SafeString globals) instead of by index */
    gabi::Local<SafeString> s1;
    gabi::Local<SafeString> s2;
    gabi::Local<SafeString> s3;
    for (; mtl != 0; mtl = gabi::load<u32>(mtl + 4) /* getNext() */) {
        bool hide = false;
        if (!(noResetFlg1() & 0x800)) { /* !checkFreezeState() */
            s1->__vtbl = 0x10034B24;
            s1->mStringTop = lk_relPtr(gabi::load<u32>(mtl) + 4); /* the material's name */
            SafeString_vf14_l(0x1046CCB4);
            SafeString_vf14_l(0x1046CCB4);
            u32 g = gabi::load<u32>(0x1046CCB4);
            SafeString_vf14_l(gabi::ea(s1.get()));
            if (g == s1->mStringTop || lk_safeStrEq(gabi::load<u32>(0x1046CCB4), s1->mStringTop)) {
                hide = true;
            } else {
                s2->__vtbl = 0x10034B24;
                s2->mStringTop = lk_relPtr(gabi::load<u32>(mtl) + 4);
                SafeString_vf14_l(0x1046CCCC);
                SafeString_vf14_l(0x1046CCCC);
                g = gabi::load<u32>(0x1046CCCC);
                SafeString_vf14_l(gabi::ea(s2.get()));
                if (g == s2->mStringTop || lk_safeStrEq(gabi::load<u32>(0x1046CCCC), s2->mStringTop)) {
                    hide = true;
                }
            }
        }
        if (!hide) {
            s3->__vtbl = 0x10034B24;
            s3->mStringTop = lk_relPtr(gabi::load<u32>(mtl) + 4);
            SafeString_vf14_l(0x1046CCC4);
            SafeString_vf14_l(0x1046CCC4);
            u32 g = gabi::load<u32>(0x1046CCC4);
            SafeString_vf14_l(gabi::ea(s3.get()));
            if (g == s3->mStringTop || lk_safeStrEq(gabi::load<u32>(0x1046CCC4), s3->mStringTop)) {
                if (checkCaughtShapeHide() || (noResetFlg1() & 8) /* daPyFlg1_CASUAL_CLOTHES */) {
                    hide = true;
                }
            }
        }
        gabi::store<u8>(gabi::load<u32>(mtl + 8) + 4, hide ? 0 : 1); /* getShape()->hide() / show() */
    }
    if (noResetFlg1() & 8) {
        /* mpCLModelData->getJointNodePointer(CL_JNT_CL_BACK_e)->getMesh()->getShape()->hide() */
        u32 node = lk_jointNode(gabi::ea(mpCLModelData.get()), 0x29);
        gabi::store<u8>(gabi::load<u32>(gabi::load<u32>(node + 0x10) + 8) + 4, 0);
    }
}
VERIFY(0x023D9074, &daPy_lk_c::hideHatAndBackle);

/* 023DF600 */
void daPy_lk_c::makeItemType() {
    WWHD_FUNC(0x023DF600, void, this);
#define LK_fastCreate(name, prm) gabi::call<fopAc_ac_c*>(0x025D5928 /* fopAcM_fastCreate */, (s32)(name), (u32)(prm), &current.pos, -1, 0, 0, -1, 0, 0)
    u16 item = mEquipItem;
    if (item == 0x25 /* dItemNo_GRAPPLING_HOOK_e */) {
        gabi::call(LK_actorKeep_setData, &mActorKeepEquip, LK_fastCreate(0x1BE /* fpcNm_HIMO2_e */, 0));
    } else if (item == 0x2F /* dItemNo_HOOKSHOT_e */) {
        gabi::call(LK_actorKeep_setData, &mActorKeepEquip, LK_fastCreate(0xA9 /* fpcNm_HOOKSHOT_e */, 0));
        setHookshotModel();
    } else if (item == 0x2D /* dItemNo_BOOMERANG_e */) {
        gabi::call(LK_actorKeep_setData, &mActorKeepEquip, LK_fastCreate(0x1B0 /* fpcNm_BOOMERANG_e */, 0));
    } else if (item == 0x31 /* dItemNo_BOMB_BAG_e */) {
        u32 prm = gabi::call<u32>(0x020CB8D8 /* daBomb_c::prm_make */, 3, 0, 0);
        gabi::call(LK_actorKeep_setData, &mActorKeepGrab, LK_fastCreate(0x126 /* fpcNm_BOMB_e */, prm));
        u32 grab = gabi::ea(mActorKeepGrab.mActor.get());
        mEquipItem = 0x100;
        if (grab != 0) {
            setActAnimeUpper(0x95 /* LKANM_BCK_GRABWAIT */, 2 /* UPPER_MOVE2_e */, 0.0f, 0.0f, -1, 5.0f);
            mActivePlayerBombs = mActivePlayerBombs + 1;
            gabi::call(0x025D9D0C /* fopAcM_setCarryNow */, mActorKeepGrab.mActor.get(), 0);
            u32 play = dComIfGp_ea(); /* dComIfGp_setItemBombNumCount(-1) */
            gabi::store<s16>(play + 0x5B6C, (s16)(gabi::load<s16>(play + 0x5B6C) - 1));
            m35C8 = 17.0f;
        }
        return;
    } else if (checkBowItem(item)) {
        gabi::call(LK_setBowModel, this);
        return;
    } else if (item == 0x20 /* dItemNo_TELESCOPE_e */) {
        setScopeModel();
        return;
    } else if (checkPhotoBoxItem(item)) {
        setPhotoBoxModel();
        return;
    } else if (item == 0x21 /* dItemNo_TINGLE_TUNER_e */) {
        setTinkleCeiverModel();
        return;
    } else if (item == 0x34 /* dItemNo_DEKU_LEAF_e */) {
        return; /* HD: no small fan model here */
    } else if (item == 0x22 /* dItemNo_WIND_WAKER_e */) {
        mEquipItem = 0xFF; /* dItemNo_NONE_e */
        setTactModel();
        return;
    } else if (item == 0x33 /* dItemNo_SKULL_HAMMER_e */) {
        setHammerModel();
        return;
    } else if (item == 0x50 /* dItemNo_EMPTY_BOTTLE_e */) {
        gabi::call(LK_setBottleModel, this, 0x50);
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
VERIFY(0x023DF600, &daPy_lk_c::makeItemType);

/* 023DBDD0 */
BOOL daPy_lk_c::setGetDemo() {
    WWHD_FUNC(0x023DBDD0, BOOL, this);
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(gabi::at<const char>(0x100353D4) /* "Link" */, this, 0);
    if (gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0 /* dComIfGp_event_runCheck() */ && (m34CE & 4)) {
        dComIfGp_evmng_cutEnd(staffIdx);
        return TRUE;
    }
    /* HD: dEvent_manager_c::startCheckOld */
    if (gabi::call<BOOL>(0x025445B8, dComIfGp_getPEvtManager(), gabi::at<const char>(0x100353DC) /* "DEFAULT_GETITEM" */)) {
        u8 b = m34CE;
        bool go = (b & 1) != 0;
        if (!go) {
            if ((mNoResetFlg0 & 0xA0000000) /* daPyFlg0_UNK80000000 | daPyFlg0_UNK20000000 */ ||
                !(gabi::load<u32>(gabi::ea(this) + 0x834) & 0x20) /* !mAcch.ChkGroundHit() */ ||
                (mModeFlg & 0x10452822) /* checkPlayerFly() */) {
                if (!(gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x10000) /* !daPyStts0_SHIP_RIDE_e */) {
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
            u32 cutName = gabi::call<u32>(0x02544830 /* dEvent_manager_c::getMyNowCutName */, dComIfGp_getPEvtManager(), staffIdx);
            if (cutName != 0) {
                s32 cut = gabi::load<u8>(cutName) * 100 + gabi::load<u8>(cutName + 1) * 10 + gabi::load<u8>(cutName + 2) - 0x14D0;
                if (cut == 0xB) {
                    fopAc_ac_c* item = gabi::call<fopAc_ac_c*>(0x025D7C98 /* fopAcM_getItemEventPartner */, this);
                    if (item != nullptr && (fpcM_GetName(item) == 0xFF /* fpcNm_ITEM_e */ || fpcM_GetName(item) == 0x101 /* fpcNm_Demo_Item_e */)) {
                        gabi::call(0x021842B8 /* daItemBase_c::hide */, item);
                        u32 itemNo = gabi::call<u32>(0x021841C8 /* daItemBase_c::getItemNo */, item);
                        if (!(m34CE & 2)) {
                            setGetItemSound(itemNo, 0);
                            m34CE = m34CE | 2;
                        }
                        /* HD: the messages go through a message manager (*0x101F4B5C) and the play message status */
                        u32 mgr = gabi::load<u32>(0x101F4B5C);
                        u32 msgNo;
                        u16 maxLife;
                        if (itemNo == 7 /* dItemNo_HEART_PIECE_e */ && ((maxLife = gabi::load<u16>(dComIfGs_base_l() + 0x20)) & 3)) {
                            msgNo = (maxLife & 3) + 0x7B;
                        } else {
                            msgNo = itemNo + 0x65;
                        }
                        if (mMsgId == 0xFFFFFFFF) {
                            if (gabi::call<s32>(0x025F7DB0 /* fopMsgM_messageSet */, mgr, msgNo, 0) != -1) {
                                mMsgId = 0; /* HD: stores 0, not the message id */
                            }
                            return TRUE;
                        }
                        /* 025F795C (matcher: fopMsgM_SearchByID) returns the message status (play + 0x5BB2) */
                        if (gabi::call<u8>(0x025F795C) == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
                            gabi::call(0x025F74D0 /* set the message status */, mgr, 0x10 /* fopMsgStts_MSG_ENDS_e */);
                            return TRUE;
                        }
                        if (gabi::call<u8>(0x025F795C) == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
                            gabi::call(0x025F74D0, mgr, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
                            dComIfGp_evmng_cutEnd(staffIdx);
                            gabi::call(0x0218432C /* daItemBase_c::dead */, item);
                            m34CE = m34CE | 4;
                            return TRUE;
                        }
                        if (gabi::call<u8>(0x025F795C) == 1 /* fopMsgStts_MSG_PREPARING_e */) {
                            gabi::call(0x025DB58C /* fopMsgM_demoMsgFlagOn */);
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
VERIFY(0x023DBDD0, &daPy_lk_c::setGetDemo);

/* 023D94E4: drawShadow (unnamed by the matcher; the matcher put the name on draw 023D9820).
 * HD: no shadow id / dComIfGd_setShadow; a shadow model (0x438) is placed on the highest of four ground
 * points around Link and faded with the height above it. */
void daPy_lk_c::drawShadow() {
    WWHD_FUNC(0x023D94E4, void, this);
    f64 gnd = LK_FIELD(f32, 0x8A0); /* mAcch ground height */
    if (gnd == -1000000000.0) {
        return;
    }
    gabi::Local<cXyz> ofs;
    ofs->x = 0.0f;
    ofs->z = 50.0f;
    ofs->y = 0.0f;
    /* dBgS_GndChk (0x54) on the stack */
    gabi::Local<u8[0x54]> chk;
    u32 c = gabi::ea(chk.get());
    gabi::call(0x02008E0C /* cBgS_GndChk::cBgS_GndChk */, c);
    gabi::store<u32>(c + 0x10, 0x10034C34);
    gabi::store<u32>(c + 0x20, 0x10034C44);
    gabi::store<u8>(c + 0x44, 0);
    gabi::store<u8>(c + 0x45, 0);
    gabi::store<u8>(c + 0x46, 0);
    gabi::store<u32>(c + 0x4C, 0x10034C54);
    gabi::store<u8>(c + 0x47, 0);
    gabi::store<u32>(c + 0x00, c + 0x40);
    gabi::store<u8>(c + 0x48, 0);
    gabi::store<u8>(c + 0x49, 0);
    gabi::store<u32>(c + 0x50, 1);
    gabi::store<u8>(c + 0x4A, 0);
    gabi::store<u32>(c + 0x40, 0x10034C64);
    gabi::store<u32>(c + 0x04, c + 0x4C);
    gabi::Local<cXyz> pos;
    for (s32 i = 0, angle = 0; i < 4; i++, angle += 0x4000) {
        gabi::call(0x0200FA40 /* pos + offset rotated by angle */, pos.get(), &current.pos, (s32)(s16)angle, ofs.get());
        gabi::store<u32>(c + 0x24, gabi::load<u32>(gabi::ea(pos.get()) + 0));
        gabi::store<u32>(c + 0x28, gabi::load<u32>(gabi::ea(pos.get()) + 4));
        gabi::store<u32>(c + 0x2C, gabi::load<u32>(gabi::ea(pos.get()) + 8));
        f64 cross = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), c);
        if (cross > gnd) {
            gnd = cross;
        }
    }
    f32 h = (f32)((f64)current.pos.y - gnd);
    if (h > 10.0f && gabi::call<BOOL>(0x02008254 /* cBgS::ChkPolySafe */, dComIfG_Bgsp(), gabi::ea(this) + 0x8F4)) {
        f32 t = (h - 10.0f) * 0.2f;
        u8 alpha = (u8)gabi::ftoi(t - 36.0f >= 0.0f ? 36.0f : t);
        mDoMtx_stack_c::transS(current.pos.x, (f32)(gnd + 10.0), current.pos.z);
        s32 ang = (s16)(shape_angle.y + 0x8000);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)ang);
        mtx_copy_q(LK_FIELD(u32, 0x438) + 0xC8, LK_MTX_STACK); /* the shadow model's base matrix */
        u32 mat = gabi::load<u32>(gabi::load<u32>(LK_FIELD(u32, 0x438) + 0xAC) + 0x10);
        gabi::Local<be<u32>> col; /* GXColor {0, 0, 0, alpha} */
        *col = alpha;
        u32 tev = gabi::load<u32>(mat + 0x18);
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(tev + 4) + 0x3C), tev, 1, col.get()); /* setTevColor(1, color) */
        gabi::Local<u8[0x10]> f4;
        daPy_colorToF4(gabi::ea(f4.get()), gabi::ea(col.get()));
        gabi::Local<u8[0xC]> rgb;
        gabi::call(0x0274D458, rgb.get(), f4.get(), 1.0f);
        gabi::store<u32>(mat + 0xA0, gabi::load<u32>(mat + 0xA0) | 0x100);
        u32 dst = gabi::call<u32>(0x027F9F0C, mat + 0xA0, 8);
        f32 a = (f32)gabi::load<u8>(gabi::ea(col.get()) + 3) / 255.0f;
        gabi::store<f32>(dst + 4, gabi::load<f32>(gabi::ea(rgb.get()) + 4));
        gabi::store<f32>(dst + 8, gabi::load<f32>(gabi::ea(rgb.get()) + 8));
        gabi::store<f32>(dst + 0, gabi::load<f32>(gabi::ea(rgb.get()) + 0));
        gabi::store<f32>(dst + 0xC, a);
        mDoExt_modelUpdateDL(gabi::at<J3DModel>(LK_FIELD(u32, 0x438)), 0);
    }
    /* ~dBgS_GndChk */
    gabi::store<u32>(c + 0x20, 0x10034C44);
    gabi::store<u32>(c + 0x40, 0x10034C64);
    gabi::store<u32>(c + 0x4C, 0x10034C24);
    gabi::call(0x02008DAC /* cBgS_Chk::~cBgS_Chk */, c, 0);
}
VERIFY(0x023D94E4, &daPy_lk_c::drawShadow);

/* 023DEA24 */
void daPy_lk_c::setBowModel() {
    WWHD_FUNC(0x023DEA24, void, this);
    u32 bck = getItemAnimeResource(0xD /* LKANM_BCK_ARROWRELORDA */);
    u32 oldHeap = gabi::ea(setItemHeap());
    u32 tmp_modelData = initModel(mpEquipItemModel_ea, 0x16 /* LINK_BDL_BOW */, 0x37221222);
    BOOL ret = gabi::call<BOOL>(0x025E8508 /* mDoExt_bckAnm::init */, gabi::ea(this) + 0x4444, tmp_modelData, bck, 0, 2, 1.0f, 0, -1, 0);
    if (!ret) {
        JUT_ASSERT_fail(gabi::at<const char>(0x100354EC), 0x18B, gabi::at<const char>(0x100354DC));
    }
    mDoExt_setCurrentHeap_l(oldHeap);
    m35EC = 0.0f;
    /* HD: the bow string materials are found by name among the materials of joint 0: "lineballMat" and "lineMat" shown, "lineDamMAT" hidden */
    u32 data = gabi::load<u32>(gabi::load<u32>(mpEquipItemModel_ea) + 0xAC);
    u32 mtl = gabi::load<u32>(gabi::load<u32>(data + 8) + 0x10);
    gabi::Local<SafeString> k1, k2, k3, n1, n2, n3;
    static const u32 keys[3] = {0x100354E0, 0x100354D4, 0x10035500};
    SafeString* ks[3] = {k1.get(), k2.get(), k3.get()};
    SafeString* ns[3] = {n1.get(), n2.get(), n3.get()};
    while (mtl != 0) {
        for (int i = 0; i < 3; i++) {
            u32 k = gabi::ea(ks[i]), n = gabi::ea(ns[i]);
            if (i == 0) {
                gabi::store<u32>(k + 0, keys[i]);
                gabi::store<u32>(k + 4, 0x10034B24);
            } else {
                gabi::store<u32>(k + 4, 0x10034B24);
                gabi::store<u32>(k + 0, keys[i]);
            }
            u32 name = lk_relPtr(gabi::load<u32>(mtl) + 4);
            gabi::store<u32>(n + 4, 0x10034B24);
            gabi::store<u32>(n + 0, name);
            gabi::call(0x02444F48, k);
            SafeString_vf14_l(k);
            u32 a = gabi::load<u32>(k);
            SafeString_vf14_l(n);
            if (a == gabi::load<u32>(n) || lk_safeStrEq(gabi::load<u32>(k), gabi::load<u32>(n))) {
                gabi::store<u8>(gabi::load<u32>(mtl + 8) + 4, i < 2 ? 1 : 0); /* show() / hide() */
                break;
            }
        }
        mtl = gabi::load<u32>(mtl + 4); /* getNext() */
    }
    gabi::store<u32>(lk_jointNode(tmp_modelData, 6 /* LINK_BOW_JNT_LINEAB_JNT_e */) + 8, 0x023D63D0 /* daPy_bowJointCB */);
    gabi::store<u32>(lk_jointNode(tmp_modelData, 0xB /* LINK_BOW_JNT_LINEBB_JNT_e */) + 8, 0x023D63D0);
    gabi::store<u32>(gabi::load<u32>(mpEquipItemModel_ea) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
}
VERIFY(0x023DEA24, &daPy_lk_c::setBowModel);

/* 023D7564: jointAfterCB (unnamed by the matcher) */
BOOL daPy_lk_c::jointAfterCB(int jnt_no, u32 param_2 /* J3DTransformInfo* */, u32 param_3 /* Quaternion* */) {
    WWHD_FUNC(0x023D7564, BOOL, this, jnt_no, param_2, param_3);
    /* HD: m34C6 became a per-joint flag array (0x68E2) with per-joint saved quaternions (0x6AB0, 0x10 each) and
     * transform infos (0x6D50, 0x20 each) */
    u32 flg = gabi::ea(this) + 0x68E2 + jnt_no;
    u8 f = gabi::load<u8>(flg);
    if (f != 0) {
        if (f & 1) {
            u32 q = gabi::ea(this) + 0x6AB0 + jnt_no * 0x10;
            for (int i = 0; i < 4; i++) gabi::store<u32>(param_3 + i * 4, gabi::load<u32>(q + i * 4)); /* *param_3 = quaternion */
            f = gabi::load<u8>(flg);
        }
        if (f & 2) {
            u32 t = gabi::ea(this) + 0x6D50 + jnt_no * 0x20; /* *param_2 = transform info */
            fcpy_l(param_2 + 0, t + 0);
            fcpy_l(param_2 + 4, t + 4);
            fcpy_l(param_2 + 8, t + 8);
            gabi::store<s16>(param_2 + 0xC, gabi::load<s16>(t + 0xC));
            gabi::store<s16>(param_2 + 0xE, gabi::load<s16>(t + 0xE));
            gabi::store<s16>(param_2 + 0x10, gabi::load<s16>(t + 0x10));
            fcpy_l(param_2 + 0x14, t + 0x14);
            fcpy_l(param_2 + 0x18, t + 0x18);
            fcpy_l(param_2 + 0x1C, t + 0x1C);
        }
        gabi::store<u8>(flg, 0);
    }
    u32 cl = gabi::ea(mpCLModel.get());
    if (jnt_no == 0x22 /* CL_JNT_LFOOT_JNT_e */) {
        PSMTXCopy_l(lk_getAnmMtx(cl, 0x22), gabi::ea(this) + 0x75F0); /* mFootData[1].field_0x088[2] */
    } else if (jnt_no == 0x27 /* CL_JNT_RFOOT_JNT_e */) {
        PSMTXCopy_l(lk_getAnmMtx(cl, 0x27), gabi::ea(this) + 0x74D8); /* mFootData[0].field_0x088[2] */
    } else if (jnt_no == 0x20 /* CL_JNT_LLEGA_JNT_e */) {
        PSMTXCopy_l(lk_getAnmMtx(cl, 0x20), gabi::ea(this) + 0x7590);
    } else if (jnt_no == 0x21 /* CL_JNT_LLEGB_JNT_e */) {
        PSMTXCopy_l(lk_getAnmMtx(cl, 0x21), gabi::ea(this) + 0x75C0);
    } else if (jnt_no == 0x25 /* CL_JNT_RLEGA_JNT_e */) {
        PSMTXCopy_l(lk_getAnmMtx(cl, 0x25), gabi::ea(this) + 0x7478);
    } else if (jnt_no == 0x26 /* CL_JNT_RLEGB_JNT_e */) {
        PSMTXCopy_l(lk_getAnmMtx(cl, 0x26), gabi::ea(this) + 0x74A8);
    } else if (jnt_no == 0xA || jnt_no == 0xB) {
        /* HD: the arm angles while riding the ship are applied here (and blended with the old frame) */
        if ((gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x10000) /* daPyStts0_SHIP_RIDE_e */ &&
            gabi::load<u8>((u32)m_old_fdata) != 0 && gabi::load<u32>(dComIfGp_ea() + 0x5B3C) != 0) {
            u32 ship = gabi::load<u32>(dComIfGp_ea() + 0x5B3C);
            if (!gabi::call<BOOL>(0x025DD868 /* fpcM_IsCreating */, ship != 0 ? gabi::load<u32>(ship + 4) : 0xFFFFFFFF) &&
                !checkShipNotNormalMode()) {
                gabi::call(LK_setShipRideArmAngle, this, jnt_no, param_2);
                mtx_copy_q(lk_getAnmMtx(gabi::ea(mpCLModel.get()), jnt_no), LK_J3DSYS_CURMTX);
                gabi::store<f32>(param_2 + 0x1C, 0.0f);
                gabi::store<f32>(param_2 + 0x18, 0.0f);
                gabi::store<f32>(param_2 + 0x14, 0.0f);
                u32 od = m_old_fdata;
                gabi::call(0x027ED3AC /* quaternion blend */, gabi::load<u32>(od + 0x20) + jnt_no * 0x10, 0x101CEE50, param_3,
                           gabi::fsubs_ppc(1.0f, gabi::load<f32>(od + 0xC)));
            }
        }
    }
    return TRUE;
}
VERIFY(0x023D7564, &daPy_lk_c::jointAfterCB);

/* 023D7198 */
void daPy_lk_c::setShipRideArmAngle(int jnt_no, u32 param_2 /* J3DTransformInfo* */) {
    WWHD_FUNC(0x023D7198, void, this, jnt_no, param_2);
    /* function-local statics: arm_pos (0x1046CDC0, guard 0x1046D080), armA_offset (0x1046CDCC, guard 0x1046D084) */
    const u32 arm_pos = 0x1046CDC0, armA_offset = 0x1046CDCC;
    if (gabi::load<u32>(0x1046D080) == 0) {
        gabi::store<f32>(arm_pos + 0, 0.0f);
        gabi::store<f32>(arm_pos + 8, 0.0f);
        gabi::store<u32>(0x1046D080, 1);
        gabi::store<f32>(arm_pos + 4, 0.0f);
    }
    if (gabi::load<u32>(0x1046D084) == 0) {
        gabi::store<f32>(armA_offset + 0, 17.0f);
        gabi::store<f32>(armA_offset + 8, 0.0f);
        gabi::store<u32>(0x1046D084, 1);
        gabi::store<f32>(armA_offset + 4, 0.0f);
    }
    BOOL uVar3 = jnt_no == 0xA;
    if (uVar3) {
        u32 od = m_old_fdata;
        f32 rate = gabi::load<f32>(od + 0xC); /* getOldFrameRate() */
        f32 fVar2 = 1.0f / (1.0f - rate);
        u32 ti = gabi::load<u32>(od + 0x1C) + jnt_no * 0x20; /* getOldFrameTransInfo(jnt_no) */
        gabi::Local<cXyz> local_74;
        f32 x = gabi::fnmsubs(gabi::load<f32>(ti + 0x14), rate, gabi::load<f32>(param_2 + 0x14)) * fVar2;
        gabi::store<f32>(param_2 + 0x14, x);
        f32 y = gabi::fnmsubs(gabi::load<f32>(ti + 0x18), rate, gabi::load<f32>(param_2 + 0x18)) * fVar2;
        gabi::store<f32>(param_2 + 0x18, y);
        f32 z = gabi::fnmsubs(gabi::load<f32>(ti + 0x1C), rate, gabi::load<f32>(param_2 + 0x1C)) * fVar2;
        local_74->x = x;
        local_74->y = y;
        gabi::store<f32>(param_2 + 0x1C, z);
        local_74->z = z;
        /* HD: the arm joint's matrix (mpCLModel->getAnmMtx(9)) instead of J3DSys::mCurrentMtx */
        gabi::call(0x028E8F64 /* PSMTXMultVec */, lk_getAnmMtx(gabi::ea(mpCLModel.get()), 9), local_74.get(), arm_pos);
    }
    gabi::Local<cXyz> local_68;
    gabi::call(0x0201ADE0 /* cXyz::operator- */, gabi::load<u32>(dComIfGp_ea() + 0x5B3C) + 0x720 /* ship->getTillerTopPosP() */, local_68.get(),
               arm_pos);
    f32 lx = local_68->x, ly = local_68->y, lz = local_68->z;
    gabi::Local<cXyz> xz1;
    xz1->x = lx;
    xz1->y = 0.0f;
    xz1->z = lz;
    f64 m1 = gabi::call<f64>(0x028E8DD0 /* PSVECSquareMag */, xz1.get());
    f64 absXZ = gabi::call<f64>(0x028F4384 /* sqrtf */, m1);
    u32 uVar5 = gabi::call<u32>(0x020195B0 /* cM_atan2s */, (f64)ly, absXZ);
    mDoMtx_stack_c::transS(gabi::load<f32>(arm_pos + 0), gabi::load<f32>(arm_pos + 4), gabi::load<f32>(arm_pos + 8));
    u32 cs = 0x104A44F8 + (((uVar5 & 0xFFFF) >> 3) << 3); /* the sin/cos table entry of uVar5 */
    gabi::Local<cXyz> xz2;
    xz2->x = lx;
    f32 f30 = 26.3f * gabi::load<f32>(cs + 4); /* 26.3f * cM_scos(uVar5) */
    xz2->y = 0.0f;
    xz2->z = lz;
    f64 f1 = gabi::call<f64>(0x028E8DD0, xz2.get()); /* abs2XZ */
    f64 f31 = gabi::call<f64>(0x028F4384, f1);
    if (!uVar3) {
        u32 a = gabi::call<u32>(0x020195B0, (f64)lx, (f64)lz);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)(a - 0x4000));
        if (!(f31 < f30)) {
            mDoMtx_stack_c::scaleM((f32)(f31 / f30), 1.0f, 1.0f);
        }
    } else {
        f32 f0 = 17.0f * gabi::load<f32>(cs + 4);
        f32 f29 = f30 + f0;
        f32 f2 = f30 * f30;
        f32 f3 = f0 * f0;
        if (!(f31 < f29)) {
            u32 a = gabi::call<u32>(0x020195B0, (f64)lx, (f64)lz);
            mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)(a - 0x4000));
            mDoMtx_stack_c::scaleM((f32)(f31 / f29), 1.0f, 1.0f);
        } else {
            f32 t = (f32)((f64)gabi::fsubs_ppc(f3, f2) + f1);
            f32 r = (f32)(t / (f64)(f32)(f1 + f1));
            u32 a = gabi::call<u32>(0x020195B0, (f64)lx, (f64)lz);
            f32 q = (f32)(f1 * r);
            f64 s = gabi::call<f64>(0x028F4384, (f64)gabi::fnmsubs(q, r, f3));
            u32 b = gabi::call<u32>(0x020195B0, s, (f64)(f32)(f31 * r));
            mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)((a - 0x4000) - b));
        }
    }
    gabi::call(0x025F1C5C /* mDoMtx_ZrotM */, LK_MTX_STACK, uVar5);
    gabi::call(0x025F1BF4 /* mDoMtx_XrotM */, LK_MTX_STACK, 0x4000);
    PSMTXCopy_l(LK_MTX_STACK, LK_J3DSYS_CURMTX);
    if (uVar3) {
        gabi::call(0x028E8F64 /* PSMTXMultVec */, LK_MTX_STACK, armA_offset, arm_pos);
    }
}
VERIFY(0x023D7198, &daPy_lk_c::setShipRideArmAngle);

/* 023D5164: initTextureAnime (unnamed by the matcher) */
void daPy_lk_c::initTextureAnime() {
    WWHD_FUNC(0x023D5164, void, this);
    u32 buf = operator_new_arr_align_l(0x1000, 0x20);
    LK_FIELD(u32, 0x65D8) = buf; /* m_tex_anm_heap.m_buffer */
    if (buf == 0) {
        JUT_ASSERT_fail(gabi::at<const char>(0x100351C4), 0x5C2C, gabi::at<const char>(0x100351D8));
    }
    /* HD: the eye/mouth pattern animation comes from the resident "LkAnm" archive and drives the two HD
     * animation objects at 0x644 and 0x69C (instead of J3DTexNoAnm / J3DMaterialAnm objects) */
    gabi::Local<SafeString> key;
    key->__vtbl = 0x10034B24;
    key->mStringTop = 0x100351B4; /* "LkAnm" */
    u32 btp = gabi::call<u32>(0x026066C4, dComIfG_resControl(), key.get(), 0x22D /* LKANM_BTP_TMABAA */);
    gabi::Local<u8[0x40]> info; /* the animation object's setup (sp+0x24..0x64; HD: 0x40 bytes, field +0x3C) */
    u32 o = gabi::ea(info.get());
    gabi::store<s32>(o + 0xC, -1);
    gabi::store<s32>(o + 0x8, -1);
    gabi::store<s32>(o + 0x4, -1);
    gabi::store<s32>(o + 0x0, -1);
    gabi::store<u32>(o + 0x14, 0);
    gabi::store<u8>(o + 0x10, 1);
    gabi::store<u32>(o + 0x18, 0);
    gabi::store<u8>(o + 0x11, 0);
    u32 d = gabi::load<u32>(btp + 0xC);
    gabi::store<u32>(o + 0x4, gabi::load<u16>(d + 0x16));
    s32 v = gabi::load<s32>(d + 0x18);
    if (v >= 0) {
        gabi::store<s32>(o + 0x8, v);
    }
    v = gabi::load<s32>(d + 0x1C);
    if (v >= 0) {
        gabi::store<s32>(o + 0xC, v);
    }
    u16 flags = gabi::load<u16>(d + 0xC);
    gabi::store<u32>(o + 0x14, 0);
    gabi::store<u32>(o + 0x18, 0);
    gabi::store<u8>(o + 0x11, (flags & 1) ^ 1);
    /* HD: +0x3C = the model data's u16 at +0x24 (game test: the field was never set, and the 0x1C-byte Local was
     * smaller than the object the HD animation-object init reads and writes) */
    gabi::store<u32>(o + 0x3C, gabi::load<u16>(J3DModelData_HD_027F3F8C_l(gabi::ea(mpCLModelData.get())) + 0x24));
    gabi::store<s32>(o + 0x8, 5);
    gabi::store<u32>(o + 0x18, 0);
    gabi::store<s32>(o + 0xC, 0x17C);
    gabi::store<s32>(o + 0x4, 5);
    gabi::store<u32>(o + 0x14, 0);
    gabi::store<u8>(o + 0x11, 1);
    gabi::store<s32>(o + 0x0, 0x18);
    for (u32 obj = gabi::ea(this) + 0x644; obj <= gabi::ea(this) + 0x69C; obj += 0x58) {
        u32 b = daPy_newAlign(0x2000, 4);
        gabi::call(0x027E09B8 /* HD animation object init */, obj, o, b, 0x2000);
        gabi::call(0x027E12B4, obj, gabi::load<u32>(btp + 0xC));
        gabi::call(0x027E0C88, obj, J3DModelData_HD_027F3F8C_l(gabi::ea(mpCLModelData.get())));
    }
    lk_linkTexMaterials(this, 0x100351BC /* "mouth" */);
}
VERIFY(0x023D5164, &daPy_lk_c::initTextureAnime);

/* 023DD36C */
u16 daPy_lk_c::checkNormalFace() {
    WWHD_FUNC(0x023DD36C, u16, this);
    if (mCurProc == 0x8F /* daPyProc_SHIP_CRANE_e */) {
        return 0; /* daPyFace_TMABAA */
    }
    s32 face = mFace;
    if (face != 0xAD /* !checkFaceTypeNot() */) {
        return (u16)face;
    }
    if (resetFlg0() & 0x40000 /* daPyRFlg0_UNK40000 */) {
        return 0xF; /* daPyFace_TMABAG */
    }
    u16 upper = LK_upperAnmIdx();
    if (upper == 0x96 /* GRABWAITB */) {
        return 0x12; /* daPyFace_TMABAJ_TEYORIME */
    }
    if (upper == 0x95 /* GRABWAIT */) {
        return 9; /* daPyFace_TMABAE */
    }
    if (gabi::load<u16>(0x101CEF16) == 1 /* daPy_dmEcallBack_c::checkCurse() */ || checkRestHPAnime()) {
        return 0xE; /* daPyFace_TMABAF */
    }
    u32 f1 = noResetFlg1();
    if ((f1 & 1) /* checkNoDamageMode(): EQUIP_DRAGON_SHIELD */ || mTinkleShieldTimer != 0) {
        return 3; /* daPyFace_TMABAC */
    }
    if (gabi::load<u16>(0x101CEF16) == 0 /* daPy_dmEcallBack_c::checkFlame() */) {
        return 0x40; /* daPyFace_TDAMDASH */
    }
    if (f1 & 0x100 /* daPyFlg1_CONFUSE */) {
        return 0x92; /* daPyFace_TMABAH_TABEKOBE */
    }
    if (mNoResetFlg0 & 0x40000000 /* getHeavyState() */) {
        return 0x10; /* daPyFace_TMABAH */
    }
    if ((f1 & 0x1000000) /* daPyFlg1_UNK1000000 */ && !(mModeFlg & 1)) {
        return 0xA2; /* daPyFace_TDASHKAZE */
    }
    if (checkUpperReadyAnime() || (noResetFlg1() & 0x400) ||
        (mpAttnActorLockOn != nullptr && gabi::load<u8>(gabi::ea(mpAttnActorLockOn.get()) + 0x2DA) == 2 /* fopAc_ENEMY_e */) ||
        (mNoResetFlg0 & 0x2000000) /* checkEquipHeavyBoots() */ || mEquipItem == 0x33 /* dItemNo_SKULL_HAMMER_e */) {
        return 3;
    }
    if (checkBossBgm()) {
        return 3;
    }
    u32 att = mpAttention;
    if (gabi::call<BOOL>(0x024EDFCC /* dAttention_c::LockonTruth */, att) || (gabi::load<u32>(att + 0x20) & 0x20000000)) {
        return 4; /* daPyFace_TMABACB */
    }
    if (mEquipItem != 0x100 /* daPyItem_NONE_e */) {
        return 2; /* daPyFace_TMABAB */
    }
    if (gabi::load<u16>(gabi::ea(this) + 0x5858) == 0x136 /* m_anm_heap_under[UNDER_MOVE1_e].mIdx == WALKSLOPE */) {
        return 3;
    }
    s32 proc = mCurProc;
    if (proc == 6 /* daPyProc_MOVE_e */) {
        f32 spd = mNormalSpeed;
        f32 x;
        if (m3580 == 8) {
            x = (1.0f * spd) / gabi::load<f32>(gabi::ea(this) + 0x3C4) /* mMaxNormalSpeed */;
        } else {
            f32 c = gabi::load<f32>(0x104A44FC + ((u32)(u16)m34E2 >> 3) * 8); /* cM_scos(m34E2) */
            x = (c * spd) / gabi::load<f32>(gabi::ea(this) + 0x3C4);
        }
        if (!(x < 0.9f)) {
            return 6; /* daPyFace_TMABACC */
        }
    }
    if (proc == 4 /* daPyProc_WAIT_e */ && gabi::load<u8>(dComIfGp_ea() + 0x5292) == 0 /* !dComIfGp_event_runCheck() */) {
        f64 rnd = gabi::call<f64>(0x02019788 /* cM_rnd */);
        u16 idx = gabi::load<u16>(gabi::ea(this) + 0x65D0); /* m_tex_anm_heap.mIdx */
        if (idx == 0x22D /* TMABAA */ || idx == 0x22E /* TMABAB */) {
            if (rnd < 0.01f) {
                return 6;
            }
        } else if (idx == 0x231 /* TMABACC */) {
            if (!(rnd < 0.01f)) {
                return 6;
            }
        }
        /* dStage_stagInfo_GetSTType(dComIfGp_getStageStagInfo()): the stage data's virtual (slot 0x15C) */
        u32 sd = dComIfGp_ea() + 0x5150;
        u32 info = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(sd) + 0x15C), sd);
        if (((gabi::load<u32>(info + 0xC) >> 16) & 7) == 1 /* dStageType_DUNGEON_e */) {
            return 2;
        }
        sd = dComIfGp_ea() + 0x5150;
        info = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(sd) + 0x15C), sd);
        if (((gabi::load<u32>(info + 0xC) >> 16) & 7) == 4 /* dStageType_SUBDUNGEON_e */) {
            return 2;
        }
    }
    return 0;
}
VERIFY(0x023DD36C, &daPy_lk_c::checkNormalFace);

/* mDoExt_AnmRatioPack::setRatio (HD: one ratio per joint: count at +8, array at +0xC) */
static inline void lk_setRatio(u32 pack, f32 r) {
    for (s32 j = 0; j < gabi::load<s32>(pack + 8); j++) {
        gabi::store<f32>(gabi::load<u32>(pack + 0xC) + j * 4, r);
    }
}

/* 023E0A04 */
BOOL daPy_lk_c::setSingleMoveAnime(int anm, f32 rate, f32 start, int end, f32 i_morf) {
    WWHD_FUNC(0x023E0A04, BOOL, this, anm, rate, start, end, i_morf);
    u32 anmData = getAnmData(anm);
    gabi::Local<be<u32>> under_bck;
    gabi::Local<be<u32>> upper_bck;
    getUnderUpperAnime(anmData, gabi::ea(under_bck.get()), gabi::ea(upper_bck.get()), 0, 0xB400);
    u32 self = gabi::ea(this);
    gabi::store<u16>(self + 0x5858, 0xFFFF); /* m_anm_heap_under[UNDER_MOVE1_e].mIdx */
    gabi::store<u16>(self + 0x5878, 0xFFFF); /* m_anm_heap_upper[UPPER_MOVE1_e].mIdx */
    lk_setRatio(self + 0x5818, 1.0f); /* mAnmRatioUpper[UPPER_MOVE0_e] */
    lk_setRatio(self + 0x5828, 0.0f);
    lk_setRatio(self + 0x57F8, 1.0f); /* mAnmRatioUnder[UNDER_MOVE0_e] */
    lk_setRatio(self + 0x5808, 0.0f);
    u32 under = *under_bck;
    gabi::store<u32>(self + 0x57FC, under); /* mAnmRatioUnder[UNDER_MOVE0_e].setAnmTransform(under_bck) */
    gabi::store<u32>(self + 0x580C, 0);
#define LK_frameMax(a) gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>((a) + 4) + 0x14), (a))
#define LK_attribute(a) gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>((a) + 4) + 0xC), (a))
    s32 endUnder = end < 0 ? LK_frameMax(under) : end;
    s32 start16;
    if (rate < 0.0f) {
        f32 frame = (f32)endUnder - 0.001f;
        u32 attr = LK_attribute((u32)*under_bck);
        start16 = (s16)gabi::ftoi(start);
        gabi::call(0x023DE788 /* setFrameCtrl */, this, self + 0x5898, attr, start16, endUnder, rate, frame);
        gabi::store<f32>(*under_bck, frame);
    } else {
        u32 attr = LK_attribute((u32)*under_bck);
        start16 = (s16)gabi::ftoi(start);
        gabi::call(0x023DE788, this, self + 0x5898, attr, start16, endUnder, rate, start);
        gabi::store<f32>(*under_bck, start);
    }
    u32 upper = *upper_bck;
    if (upper != 0) {
        gabi::store<u32>(self + 0x581C, upper);
        s32 endUpper = end < 0 ? LK_frameMax(upper) : end;
        if (rate < 0.0f) {
            f32 frame = (f32)endUpper - 0.001f;
            u32 attr = LK_attribute((u32)*under_bck); /* under_bck's attribute, as in the GameCube code */
            gabi::call(0x023DE788, this, self + 0x58B8, attr, start16, endUpper, rate, frame);
            gabi::store<f32>(*upper_bck, frame);
        } else {
            u32 attr = LK_attribute((u32)*under_bck);
            gabi::call(0x023DE788, this, self + 0x58B8, attr, start16, endUpper, rate, start);
            gabi::store<f32>(*upper_bck, start);
        }
    } else {
        gabi::store<u32>(self + 0x581C, *under_bck);
    }
#undef LK_frameMax
#undef LK_attribute
    gabi::store<u32>(self + 0x582C, 0);
    if (!(i_morf < 0.0f)) {
        gabi::call(0x025E3F3C /* mDoExt_MtxCalcOldFrame::initOldFrameMorf */, (u32)m_old_fdata, i_morf, 0, 0x2A);
    }
    gabi::call(LK_setTextureAnime, this, (u32)gabi::load<u16>(0x100366A6 + anm * 8) /* mAnmDataTable[anm].mTexAnmIdx */, 0);
    setHandModel(anm);
    setSeAnime(gabi::load<u32>(self + 0x57FC), self + 0x5848, self + 0x5898);
    m34C3 = 0;
    return TRUE;
}
VERIFY(0x023E0A04, &daPy_lk_c::setSingleMoveAnime);

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
    WWHD_FUNC(0x023DF0FC, void, this, param_0);
    mEquipItem = (u16)param_0;
    u32 oldHeap = gabi::ea(setItemHeap());
    u32 data = initModel(mpEquipItemModel_ea, 0x3D /* LINK_BDL_BOTTLEEMP */, 0x37221222);
    /* HD: the animations come from the resident "LkAnm" archive into the btk/brk objects at 0x4810 / 0x48F8 */
    gabi::Local<SafeString> k1;
    k1->__vtbl = 0x10034B24;
    k1->mStringTop = 0x10035534;
    u32 btk = gabi::call<u32>(0x026066C4, dComIfG_resControl(), k1.get(), 0x16B /* LKANM_BTK_TBOTTLE */);
    s32 mode = anm_vf0C_l(btk);
    mDoExt_btkAnm_init_l(gabi::ea(this) + 0x4810, data, btk, 1, mode, 1.0f, 0, -1, LK_FIELD(u32, 0x4878) != 0, 0);
    gabi::call(0x025E7FC4 /* mDoExt_btkAnm::entry */, gabi::ea(this) + 0x4810, data, 0.0f);
    lk_anmObjSetFrame(gabi::ea(this) + 0x4810, 0x68, 0x10, 0.0f);
    gabi::Local<SafeString> k2;
    bool liquid = false;
    u32 bdl = 0x3A; /* LINK_BDL_BINLIQUID */
    if (param_0 >= 0x51 && (param_0 <= 0x56 || param_0 == 0x59)) {
        liquid = true; /* RED/GREEN/BLUE_POTION, HALF_SOUP, SOUP, WATER, FOREST_WATER */
        if (param_0 == 0x54 /* dItemNo_HALF_SOUP_BOTTLE_e */) {
            bdl = 0x3B; /* LINK_BDL_BINLIQUIDH */
        }
    }
    if (liquid) {
        data = initModel(gabi::ea(this) + 0x4970 /* mpBottleContentsModel */, bdl, 0x13000022);
        k2->__vtbl = 0x10034B24;
        k2->mStringTop = 0x10035534;
        u32 brk = gabi::call<u32>(0x026066C4, dComIfG_resControl(), k2.get(), 0x153 /* LKANM_BRK_TBINLIQUID */);
        mode = anm_vf0C_l(brk);
        mDoExt_brkAnm_init_l(gabi::ea(this) + 0x48F8, data, brk, 1, mode, 1.0f, 0, -1, LK_FIELD(u32, 0x4908) != 0, 0);
        gabi::call(0x025E83FC /* mDoExt_brkAnm::entry */, gabi::ea(this) + 0x48F8, data, 0.0f);
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
        lk_anmObjSetFrame(gabi::ea(this) + 0x48F8, 0x10, 0x20, frame);
    } else if ((s32)param_0 == 0x57 /* dItemNo_FAIRY_BOTTLE_e */) {
        initModel(gabi::ea(this) + 0x4970, 0x13 /* LINK_BDL_BINFAIRY */, 0x13000022);
    } else if ((s32)param_0 == 0x58 /* dItemNo_FIREFLY_BOTTLE_e */) {
        data = initModel(gabi::ea(this) + 0x4970, 0x39 /* LINK_BDL_BINHO */, 0x13000022);
        gabi::call(0x027F58E0, (u32)LK_FIELD(u32, 0x4970), 1); /* HD */
        k2->__vtbl = 0x10034B24;
        k2->mStringTop = 0x10035534;
        u32 brk = gabi::call<u32>(0x026066C4, dComIfG_resControl(), k2.get(), 0x152 /* LKANM_BRK_TBINHO */);
        mode = anm_vf0C_l(brk);
        mDoExt_brkAnm_init_l(gabi::ea(this) + 0x48F8, data, brk, 1, mode, 1.0f, 0, -1, LK_FIELD(u32, 0x4908) != 0, 0);
        gabi::call(0x025E83FC, gabi::ea(this) + 0x48F8, data, 0.0f);
        s32 proc = mCurProc;
        m3600 = 0.0f;
        if (proc != 0xA3) {
            goto cap;
        }
        goto done;
    }
    if ((s32)param_0 != 0x50 /* dItemNo_EMPTY_BOTTLE_e */ && mCurProc != 0xA3 /* daPyProc_BOTTLE_DRINK_e */) {
    cap:
        initModel(gabi::ea(this) + 0x4974 /* mpBottleCapModel */, 0x15 /* LINK_BDL_BOTTLECAP */, 0x13000022);
        m355E = 1;
    }
done:
    mDoExt_setCurrentHeap_l(oldHeap);
    if ((s32)param_0 == 0x59 /* dItemNo_FOREST_WATER_e */) {
        u32 m = gabi::load<u32>(mpEquipItemModel_ea);
        gabi::call(0x023D457C /* daPy_mtxFollowEcallBack_c::makeEmitter */, &m32F0, 0x20D /* ID_AK_JN_FORESTWATER00 */, m ? m + 0xC8 : 0,
                   &current.pos, 0);
    }
}
VERIFY(0x023DF0FC, &daPy_lk_c::setBottleModel);

/* checkNormalSwordEquip(): HD: the save's sword (+0x2E) is the normal sword (0x38), or play + 0x5CEA == 2 */
static inline bool lk_checkNormalSwordEquip() {
    if (gabi::load<u8>(dComIfGs_base_l() + 0x2E) == 0x38) {
        return true;
    }
    return gabi::load<u8>(dComIfGp_ea() + 0x5CEA) == 2;
}
/* a "LkAnm" resource (sead::SafeString key with the string at 0x10035474) */
static inline u32 lk_getLkAnm_l(u32 idx) {
    gabi::Local<SafeString> key;
    key->mStringTop = 0x10035474;
    key->__vtbl = 0x10034B24;
    return gabi::call<u32>(0x026066C4, dComIfG_resControl(), key.get(), idx);
}

/* 023DE0B4 */
void daPy_lk_c::setSwordModel(BOOL r28) {
    WWHD_FUNC(0x023DE0B4, void, this, r28);
    u32 bck = getItemAnimeResource(lk_checkNormalSwordEquip() ? 0x40 /* LKANM_BCK_CUTAA */ : 0x41 /* LKANM_BCK_CUTAMS */);
    mEquipItem = 0x103; /* daPyItem_SWORD_e */
    u32 oldHeap = gabi::ea(setItemHeap());
    /* sword_model_tbl[2] (.rodata 0x1003547C / 0x1003548C): blade, glow, glow btk, glow brk, tip stab, its bpk, btk, brk */
    u32 tbl = lk_checkNormalSwordEquip() ? 0x1003547C : 0x1003548C;
    u32 data = initModel(mpEquipItemModel_ea, gabi::load<u16>(tbl + 0), 0x37221222);
    BOOL ret = gabi::call<BOOL>(0x025E8508 /* mDoExt_bckAnm::init */, gabi::ea(this) + 0x4444, data, bck, 0, 2, 1.0f, 0, -1, 0);
    if (!ret) {
        JUT_ASSERT_fail(gabi::at<const char>(0x1003549C), 0x52, gabi::at<const char>(0x10035470));
    }
    if (!lk_checkNormalSwordEquip()) {
        u32 btk = lk_getLkAnm_l(0x1C3 /* LKANM_BTK_TSWMS */);
        s32 mode = anm_vf0C_l(btk);
        mDoExt_btkAnm_init_l(gabi::ea(this) + 0x4810, data, btk, 1, mode, 1.0f, 0, -1, LK_FIELD(u32, 0x4878) != 0, 0); /* mpEquipItemBtk */
        gabi::call(0x025E7FC4 /* mDoExt_btkAnm::entry */, gabi::ea(this) + 0x4810, data, 0.0f);
    }
    u32 blur = mpSwBlur;
    gabi::store<u32>(blur + 0xAC, operator_new_arr_align_l(0x4800, 0x20)); /* mSwBlur.mpPosBuffer = new (0x20) Vec[2 * 0x300] */
    data = initModel(gabi::ea(this) + 0x4978 /* mpSwordModel1 */, gabi::load<u16>(tbl + 2), 0x13000222);
    u32 r = lk_getLkAnm_l(gabi::load<u16>(tbl + 4));
    s32 mode = anm_vf0C_l(r);
    mDoExt_btkAnm_init_l(gabi::ea(this) + 0x4884, data, r, 1, mode, 1.0f, 0, -1, LK_FIELD(u32, 0x48EC) != 0, 0); /* mpSwordBtk */
    gabi::call(0x025E7FC4, gabi::ea(this) + 0x4884, data, 0.0f);
    r = lk_getLkAnm_l(gabi::load<u16>(tbl + 6));
    mode = anm_vf0C_l(r);
    mDoExt_brkAnm_init_l(gabi::ea(this) + 0x48F8, data, r, 1, mode, 1.0f, 0, -1, LK_FIELD(u32, 0x4908) != 0, 0); /* mpEquipItemBrk */
    gabi::call(0x025E83FC /* mDoExt_brkAnm::entry */, gabi::ea(this) + 0x48F8, data, 0.0f);
    data = initModel(gabi::ea(this) + 0x497C /* mpSwordTipStabModel */, gabi::load<u16>(tbl + 8), 0x13000223);
    J3DModelData_HD_027F3F8C_l(data);
    r = lk_getLkAnm_l(gabi::load<u16>(tbl + 0xC));
    mode = anm_vf0C_l(r);
    mDoExt_btkAnm_init_l(gabi::ea(this) + 0x49F4, data, r, 1, mode, 1.0f, 0, -1, 0, 0); /* mpCutfBtk */
    gabi::call(0x025E7FC4, gabi::ea(this) + 0x49F4, data, 0.0f);
    r = lk_getLkAnm_l(gabi::load<u16>(tbl + 0xE));
    mode = anm_vf0C_l(r);
    mDoExt_brkAnm_init_l(gabi::ea(this) + 0x4A68, data, r, 1, mode, 1.0f, 0, -1, 0, 0); /* mpCutfBrk */
    gabi::call(0x025E83FC, gabi::ea(this) + 0x4A68, data, 0.0f);
    r = lk_getLkAnm_l(gabi::load<u16>(tbl + 0xA));
    mode = anm_vf0C_l(r);
    gabi::call(0x025E74FC /* mDoExt_bpkAnm::init */, gabi::ea(this) + 0x4980, data, r, 1, mode, 1.0f, 0, -1, 0, 0); /* mpCutfBpk */
    gabi::call(0x025E779C /* mDoExt_bpkAnm::entry */, gabi::ea(this) + 0x4980, data, 0.0f);
    mDoExt_setCurrentHeap_l(oldHeap);
    gabi::call(0x025E1D08 /* mDoAud_bgmSetSwordUsing */, 1); /* r4 (not read: the GameCube signature is a member) is left as the previous call left it */
    m355C = 0;
    if (r28) {
        f32 frame = (f32)gabi::load<s16>(gabi::ea(this) + 0x488E) - 0.001f; /* mpSwordBtk->getFrameMax() - 0.001f */
        setNoResetFlg1(noResetFlg1() | 0x200000); /* onNoResetFlg1(daPyFlg1_UNK200000) */
        lk_anmObjSetFrame(gabi::ea(this) + 0x4884, 0x68, 0x10, frame);
    }
}
VERIFY(0x023DE0B4, &daPy_lk_c::setSwordModel);

/* 023DD768 */
void daPy_lk_c::setTextureAnime(u32 param_1 /* u16 */, int param_2) {
    WWHD_FUNC(0x023DD768, void, this, param_1, param_2);
    u32 face = param_1;
    if (face == 0) {
        face = checkNormalFace();
    }
    u32 e = 0x100362A8 + face * 4; /* mTexAnmIndexTable[face] */
    u16 anmIdx = gabi::load<u16>(gabi::ea(this) + 0x65D0); /* m_tex_anm_heap.mIdx, read before the call */
    u16 btp_idx = gabi::load<u16>(e + 0);
    u16 btk_idx = gabi::load<u16>(e + 2);
    BOOL bVar3 = FALSE;
    u32 p2 = (u32)param_2;
    if (face < 0x13 && checkMabaAnimeBtp(anmIdx) && mFace == 0xAD /* checkFaceTypeNot() */) {
        bVar3 = TRUE;
        p2 = m3530;
    }
    u32 h = gabi::ea(this) + 0x65D0;
    if (anmIdx == btp_idx) {
        if (mFace == 0xAD && !(mModeFlg & 0x500) /* ModeFlg_00000100 | ModeFlg_00000400 */) {
            m3530 = (u16)p2;
        }
    } else {
        u16 f2 = gabi::load<u16>(h + 2);
        gabi::store<u16>(h + 0, btp_idx);
        if (f2 == 0xFFFF && gabi::load<u16>(h + 4) == 0xFFFF) {
            gabi::call(LK_setTextureAnimeResource, this, loadTextureAnimeResource(btp_idx, 0), p2);
        }
    }
    f64 dVar9 = gabi::call<f64>(0x02019788 /* cM_rnd */);
    /* daPy_matAnm_c statics (HD): m_maba_flg 0x101CEF18, m_maba_timer 0x101CEF19 */
    const u32 mabaFlg = 0x101CEF18, mabaTimer = 0x101CEF19;
    h = gabi::ea(this) + 0x65E0; /* m_tex_scroll_heap */
    if (gabi::load<u16>(h + 0) != btk_idx) {
        if (bVar3 && (mModeFlg & 1) && gabi::load<u8>(mabaFlg) != 0 && gabi::load<u8>(dComIfGp_ea() + 0x5292) == 0 &&
            mpAttnActorLockOn == nullptr && btk_idx == 0x1A7 /* LKANM_BTK_TMABA */) {
            if (gabi::load<u8>(mabaTimer) != 0) {
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
            u16 f2 = gabi::load<u16>(h + 2);
            gabi::store<u16>(h + 0, btk_idx);
            if (f2 == 0xFFFF && gabi::load<u16>(h + 4) == 0xFFFF) {
                setTextureScrollResource(loadTextureScrollResource(btk_idx, 0), p2);
            }
            if (btk_idx != 0x1A7) {
                gabi::store<u8>(mabaFlg, 1); /* daPy_matAnm_c::onMabaFlg() */
            }
        } else {
            u16 f2 = gabi::load<u16>(h + 2);
            gabi::store<u16>(h + 0, btk_idx);
            if (f2 == 0xFFFF && gabi::load<u16>(h + 4) == 0xFFFF) {
                setTextureScrollResource(loadTextureScrollResource(btk_idx, 0), p2);
            }
        }
    } else if (bVar3 && (mModeFlg & 1) && gabi::load<u8>(dComIfGp_ea() + 0x5292) == 0 && gabi::load<u8>(mabaTimer) == 0 &&
               mpAttnActorLockOn == nullptr && btk_idx == 0x1A7 && gabi::load<u16>(h + 2) == 0xFFFF && gabi::load<u16>(h + 4) == 0xFFFF &&
               gabi::call<f64>(0x02019788) < 0.025f) {
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
        gabi::store<u16>(h + 0, idx);
        setTextureScrollResource(loadTextureScrollResource(idx, 0), 0);
        gabi::store<u8>(mabaFlg, 1);
    }
}
VERIFY(0x023DD768, &daPy_lk_c::setTextureAnime);

/* 023D4F08 */
void daPy_lk_c::initTextureScroll() {
    WWHD_FUNC(0x023D4F08, void, this);
    u32 buf = operator_new_arr_align_l(0x800, 0x20);
    LK_FIELD(u32, 0x65E8) = buf; /* m_tex_scroll_heap.m_buffer */
    if (buf == 0) {
        JUT_ASSERT_fail(gabi::at<const char>(0x1003517C), 0x5CA5, gabi::at<const char>(0x10035190));
    }
    /* HD: the eye scroll animation comes from the resident "LkAnm" archive and drives the HD animation object at
     * 0x6F4 (instead of J3DTexMtxAnm objects); the daPy_matAnm_c objects are attached to the materials named by
     * the sead::SafeString table at 0x1046CD2C */
    gabi::Local<SafeString> key;
    key->__vtbl = 0x10034B24;
    key->mStringTop = 0x10035174; /* "LkAnm" */
    u32 btk = gabi::call<u32>(0x026066C4, dComIfG_resControl(), key.get(), 0x1A7 /* LKANM_BTK_TMABA */);
    gabi::Local<u8[0x40]> info; /* the animation object's setup (sp+0x14..0x54; HD: 0x40 bytes, field +0x3C) */
    u32 o = gabi::ea(info.get());
    gabi::store<s32>(o + 0xC, -1);
    gabi::store<u32>(o + 0x14, 0);
    gabi::store<s32>(o + 0x4, -1);
    gabi::store<u32>(o + 0x18, 0);
    gabi::store<u8>(o + 0x10, 1);
    gabi::store<s32>(o + 0x0, -1);
    gabi::store<u8>(o + 0x11, 0);
    gabi::store<s32>(o + 0x8, -1);
    u32 d = gabi::load<u32>(btk + 0xC);
    gabi::store<u32>(o + 0x4, gabi::load<u16>(d + 0x14));
    s32 v = gabi::load<s32>(d + 0x18);
    if (v >= 0) {
        gabi::store<s32>(o + 0x8, v);
    }
    v = gabi::load<s32>(d + 0x1C);
    if (v >= 0) {
        gabi::store<s32>(o + 0xC, v);
    }
    u32 flags = gabi::load<u32>(d + 0xC);
    gabi::store<u32>(o + 0x14, 0);
    gabi::store<u32>(o + 0x18, 0);
    gabi::store<u8>(o + 0x11, (flags & 1) ^ 1);
    /* HD: +0x3C = the model data's u16 at +0x24 (game test: the field was never set, and the 0x1C-byte Local was
     * smaller than the object the HD animation-object init reads and writes) */
    gabi::store<u32>(o + 0x3C, gabi::load<u16>(J3DModelData_HD_027F3F8C_l(gabi::ea(mpCLModelData.get())) + 0x24));
    gabi::store<u32>(o + 0x18, 0);
    gabi::store<s32>(o + 0x4, 2);
    gabi::store<u8>(o + 0x11, 1);
    gabi::store<s32>(o + 0x0, 0x18);
    gabi::store<s32>(o + 0x8, 0x10);
    gabi::store<s32>(o + 0xC, 0x17C);
    gabi::store<u32>(o + 0x14, 0);
    u32 b = daPy_newAlign(0x2000, 4);
    gabi::call(0x027DEE5C /* HD animation object init */, gabi::ea(this) + 0x6F4, o, b, 0x2000);
    HDAnm_setAnm_l(gabi::ea(this) + 0x6F4, gabi::load<u32>(btk + 0xC));
    HDAnm_setModelData_l(gabi::ea(this) + 0x6F4, J3DModelData_HD_027F3F8C_l(gabi::ea(mpCLModelData.get())));
    u16 material_num = gabi::load<u16>(gabi::load<u32>(btk + 0xC) + 0x14);
    for (u32 no = 0; no < material_num; no++) {
        u32 mat = gabi::ea(operator_new(0x7C));
        if (mat != 0) {
            mat = daPy_matAnm_ct(mat);
        }
        u32 slot = gabi::ea(this) + 0x604 + no * 4; /* m_tex_eye_scroll[no] */
        gabi::store<u32>(slot, mat);
        u32 x = J3DModelData_HD_027F3F8C_l(gabi::ea(mpCLModelData.get()));
        u32 name = 0x1046CD2C + no * 8;
        SafeString_vf14_l(name);
        u16 matID = (u16)gabi::call<u32>(0x027DF9B0 /* material name -> index */, lk_relPtr(x + 0x18), gabi::load<u32>(name));
        u32 md = gabi::ea(mpCLModelData.get());
        u32 cnt = gabi::load<u32>(md + 0xC);
        u32 anm = gabi::load<u32>(slot);
        u32 m = gabi::load<u32>(md + 0x10); /* getMaterialNodePointer(matID) (HD: 0x39C each, entry 0 when out of range) */
        if (matID < cnt) {
            m += matID * 0x39C;
        }
        gabi::store<u32>(m + 0x24, anm); /* setMaterialAnm */
    }
}
VERIFY(0x023D4F08, &daPy_lk_c::initTextureScroll);

/* the HD per-joint save of a J3DTransformInfo (0x6D50 + jnt * 0x20) before it is modified (flag 2 at 0x68E2 + jnt) */
static inline void lk_saveTransInfo(u32 self, int jnt, u32 ti) {
    gabi::store<u8>(self + 0x68E2 + jnt, 2);
    u32 d = self + 0x6D50 + jnt * 0x20;
    fcpy_l(d + 0, ti + 0);
    fcpy_l(d + 4, ti + 4);
    fcpy_l(d + 8, ti + 8);
    gabi::store<s16>(d + 0xC, gabi::load<s16>(ti + 0xC));
    gabi::store<s16>(d + 0xE, gabi::load<s16>(ti + 0xE));
    gabi::store<s16>(d + 0x10, gabi::load<s16>(ti + 0x10));
    fcpy_l(d + 0x14, ti + 0x14);
    fcpy_l(d + 0x18, ti + 0x18);
    fcpy_l(d + 0x1C, ti + 0x1C);
}
static inline void lk_copy16(u32 dst, u32 src) {
    for (int i = 0; i < 4; i++) gabi::store<u32>(dst + i * 4, gabi::load<u32>(src + i * 4));
}

/* 023D6B30 */
BOOL daPy_lk_c::jointBeforeCB(int jnt_no, u32 param_2 /* J3DTransformInfo* */, u32 param_3 /* Quaternion* */) {
    WWHD_FUNC(0x023D6B30, BOOL, this, jnt_no, param_2, param_3);
    u32 self = gabi::ea(this);
    gabi::Local<csXyz> L;
    gabi::call(0x0201A478 /* csXyz::csXyz */, L.get(), 0, 0, 0);
    u32 l = gabi::ea(L.get());
    bool hatSection = true;
    if (jnt_no == 0xF /* CL_JNT_HEAD_JNT_e */) {
        gabi::store<s16>(l + 2, m3564.z);
        gabi::store<s16>(l + 0, m3564.y);
        gabi::store<s16>(l + 4, m3564.x);
    } else if (jnt_no == 0x1F /* CL_JNT_LCLOTCH_JNT_e */) {
        lk_saveTransInfo(self, jnt_no, param_2);
        gabi::store<f32>(param_2 + 0x14, gabi::fsubs_ppc(gabi::load<f32>(param_2 + 0x14), LK_FIELD(f32, 0x7538) /* mFootData[1].field_0x030 */));
    } else if (jnt_no == 0x24 /* CL_JNT_RCLOTCH_JNT_e */) {
        lk_saveTransInfo(self, jnt_no, param_2);
        gabi::store<f32>(param_2 + 0x14, gabi::fsubs_ppc(gabi::load<f32>(param_2 + 0x14), LK_FIELD(f32, 0x7420) /* mFootData[0].field_0x030 */));
    } else if (jnt_no == 0x10 /* CL_JNT_LMOMI_JNT_e */) {
        gabi::store<s16>(l + 2, m3516);
        gabi::store<s16>(l + 4, m351A);
    } else if (jnt_no == 0x11 /* CL_JNT_RMOMI_JNT_e */) {
        gabi::store<s16>(l + 4, m351A);
        gabi::store<s16>(l + 2, m3518);
    } else if (jnt_no == 2 /* CL_JNT_BODY_CHN_e */) {
        /* local_38.set(-mBodyAngle.z, mBodyAngle.y, mBodyAngle.x) */
        s16 bz = LK_FIELD(s16, 0x3D4);
        s16 by = LK_FIELD(s16, 0x3D2);
        gabi::store<s16>(l + 0, (s16)-bz);
        gabi::store<s16>(l + 2, by);
        gabi::store<s16>(l + 4, LK_FIELD(s16, 0x3D0));
        lk_saveTransInfo(self, jnt_no, param_2);
        gabi::store<f32>(param_2 + 0x18, gabi::fadds_ppc(gabi::load<f32>(param_2 + 0x18), m35D8));
    } else if (jnt_no == 0) {
        gabi::store<s16>(l + 0, m34F2);
        gabi::store<s16>(l + 4, m34F4);
        if (m34C2 == 0xC) {
            goto zero_translate;
        }
    } else if (jnt_no == 1 || jnt_no == 0x29) {
        if (m34C2 == 0xC) { /* HD */
        zero_translate:
            gabi::store<f32>(param_2 + 0x18, 0.0f);
            gabi::store<f32>(param_2 + 0x14, 0.0f);
            gabi::store<f32>(param_2 + 0x1C, 0.0f);
        }
    } else if (jnt_no == 0xA || jnt_no == 0xB) {
        /* HD: the ship ride arm angles moved to jointAfterCB; only the (unused) creating check is left */
        if ((gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x10000) && gabi::load<u8>((u32)m_old_fdata) != 0 &&
            gabi::load<u32>(dComIfGp_ea() + 0x5B3C) != 0) {
            u32 ship = gabi::load<u32>(dComIfGp_ea() + 0x5B3C);
            gabi::call<BOOL>(0x025DD868 /* fpcM_IsCreating */, ship != 0 ? gabi::load<u32>(ship + 4) : 0xFFFFFFFF);
        }
    }
    if (resetFlg0() & 0x800000 /* daPyRFlg0_ORIGINAL_HAT_ANIM */) {
        hatSection = false;
    }
    if (hatSection && jnt_no == 0x1A /* CL_JNT_HATA_JNT_e */) {
        gabi::call(0x027ED2D0 /* JMAEulerToQuat */, 0, (s32)m34F8, (s32)(s16)(m34F6 - 0x4000 + m3528 + m350E - 0x4000), param_3);
    } else if (hatSection && jnt_no == 0x1B /* CL_JNT_HATB_JNT_e */) {
        gabi::call(0x027ED2D0, 0, (s32)m34FC, (s32)(s16)(m34FA + m3510), param_3);
    } else if (hatSection && jnt_no == 0x1C /* CL_JNT_HATC_JNT_e */) {
        gabi::call(0x027ED2D0, 0, (s32)m3500, (s32)(s16)(m34FE + m3512), param_3);
    } else if (jnt_no == 0 /* CL_JNT_LINK_ROOT_e */) {
        if (m34C2 == 1) {
            gabi::store<f32>(param_2 + 0x14, 0.0f);
            gabi::store<f32>(param_2 + 0x1C, 0.0f);
        } else if (m34C2 == 5) {
            gabi::store<f32>(param_2 + 0x14, 0.0f);
            fcpy_l(param_2 + 0x18, gabi::ea(&m35E0));
            gabi::store<f32>(param_2 + 0x1C, 0.0f);
        }
    } else if (jnt_no == 0x1E /* CL_JNT_WAIST_JNT_e */) {
        s16 e0 = m34E0;
        s16 e4 = m34E4;
        if (e0 != 0 || e4 != 0) {
            gabi::store<s16>(l + 2, e4);
            gabi::store<s16>(l + 4, e0);
        }
    }
    s16 lx = gabi::load<s16>(l + 0), ly = gabi::load<s16>(l + 2);
    if ((lx | ly) != 0 || gabi::load<s16>(l + 4) != 0) {
        u32 f = self + 0x68E2 + jnt_no;
        gabi::store<u8>(f, gabi::load<u8>(f) | 1);
        lk_copy16(self + 0x6AB0 + jnt_no * 0x10, param_3); /* the HD per-joint saved quaternion */
        gabi::Local<u8[0x10]> local_20;
        gabi::Local<u8[0x10]> afStack_30;
        if ((lx | ly) != 0) {
            u32 d = gabi::ea(local_20.get());
            gabi::store<u32>(d + 0xC, gabi::load<u32>(param_3 + 0xC));
            gabi::store<u32>(d + 8, gabi::load<u32>(param_3 + 8));
            gabi::store<u32>(d + 0, gabi::load<u32>(param_3 + 0));
            gabi::store<u32>(d + 4, gabi::load<u32>(param_3 + 4));
            gabi::call(0x027ED2D0 /* JMAEulerToQuat */, (s32)lx, (s32)ly, 0, afStack_30.get());
            gabi::call(0x025F2258 /* mDoMtx_QuatConcat */, local_20.get(), afStack_30.get(), param_3);
        }
        s16 lz = gabi::load<s16>(l + 4);
        if (lz != 0) {
            lk_copy16(gabi::ea(local_20.get()), param_3);
            gabi::call(0x027ED2D0, 0, 0, (s32)lz, afStack_30.get());
            gabi::call(0x025F2258, local_20.get(), afStack_30.get(), param_3);
        }
    }
    u16 upper = LK_upperAnmIdx();
    bool hit;
    if (upper == 0x16 || upper == 0x1B) { /* checkUpperGuardAnime() */
        hit = mCurProc != 0x22 /* daPyProc_BACK_JUMP_e */;
        if (!hit) {
            goto other;
        }
    } else if (upper >= 0x95 && upper <= 0x96) { /* checkGrabAnime() */
        hit = true;
    } else {
    other:
        u16 up0 = gabi::load<u16>(self + 0x5868); /* m_anm_heap_upper[UPPER_MOVE0_e].mIdx */
        hit = up0 == 0x10 /* ATNBOKO */ || up0 == 0x1C /* ATNHAM */ || checkUpperReadyThrowAnime();
    }
    if (hit) {
        if (jnt_no == 0) {
            lk_copy16(self + 0x6AA0, param_3); /* m3648 = *param_3 */
            lk_copy16(param_3, 0x101CEE40);     /* HD: *param_3 = norm_quat */
        } else if (jnt_no == 0x1D) {
            /* HD: the waist chain gets m3648 concatenated (no root matrix save/restore) */
            gabi::call(0x025F2258 /* mDoMtx_QuatConcat */, self + 0x6AA0, param_3, param_3);
        }
    }
    return TRUE;
}
VERIFY(0x023D6B30, &daPy_lk_c::jointBeforeCB);

/* 023DFDD8 */
BOOL daPy_lk_c::commonProcInit(int proc) {
    WWHD_FUNC(0x023DFDD8, BOOL, this, proc);
    u32 self = gabi::ea(this);
    u32 procInit = 0x10036DF0 + proc * 0xC; /* mProcInitTable[proc]: ProcFunc (8 bytes), mProcFlags */
    BOOL resetDemoAnime = FALSE;
    s32 cur = mCurProc;
    if (cur == 0x19 /* daPyProc_SLIP_e */) {
        gabi::call(0x025E1ACC /* mDoAud_seStop */, 0x280D /* JA_SE_LK_RUN_SLIP */);
    } else if (cur == 0x70 /* daPyProc_GRAB_MISS_e */ || (cur == 0x6E /* daPyProc_GRAB_READY_e */ && proc != 0x70)) {
        gabi::call(LK_actorKeep_clearData, &mActorKeepRope);
    } else if (cur == 0xB7 /* daPyProc_DEMO_TALISMAN_WAIT_e */) {
        seStartOnlyReverb(0x2810 /* JA_SE_LK_ITEM_TAKEOUT */);
        gabi::store<u32>(mpEquipItemModel_ea, 0);
    } else if (cur == 0x92 /* daPyProc_FAN_SWING_e */) {
        setSmallFanModel();
    } else if (cur == 0x93 /* daPyProc_FAN_GLIDE_e */) {
        deleteEquipItem(FALSE);
        LK_FIELD(f32, 0x378) = -175.0f; /* maxFallSpeed (HD: constant) */
        setSmallFanModel();
        mEquipItem = 0x34; /* dItemNo_DEKU_LEAF_e */
        fcpy_l(gabi::ea(&m35F0), gabi::ea(&m3688.y));
        gabi::store<u32>(self + 0x7338, gabi::load<u32>(0x101FFBA8 + 0)); /* m3730 = cXyz::Zero */
        gabi::store<u32>(self + 0x733C, gabi::load<u32>(0x101FFBA8 + 4));
        m34E4 = 0;
        gabi::store<u32>(self + 0x7340, gabi::load<u32>(0x101FFBA8 + 8));
        m34E0 = 0;
    } else if (cur == 0x28 /* daPyProc_SLOW_FALL_e */) {
        LK_FIELD(f32, 0x378) = -175.0f;
    } else if (cur == 0xA9 /* daPyProc_DEMO_TOOL_e */) {
        resetDemoAnime = TRUE;
        speed.y = 0.0f;
    } else if (cur == 0xAE /* daPyProc_DEMO_GET_ITEM_e */ || cur == 0xD0 /* daPyProc_DEMO_GET_DANCE_e */) {
        gabi::call(0x0255F3B4 /* dKy_Itemgetcol_chg_off */);
        if (mCurProc == 0xAE && mProcVar4 != 0) {
            m34C2 = 0xB;
        }
    } else if (cur == 0xC3 /* daPyProc_DEMO_PRESENT_e */ || cur == 0xA2 /* daPyProc_NOT_USE_e */) {
        fopAc_ac_c* item = gabi::call<fopAc_ac_c*>(0x025D7C98 /* fopAcM_getItemEventPartner */, this);
        if (item != nullptr && (fpcM_GetName(item) == 0xFF /* fpcNm_ITEM_e */ || fpcM_GetName(item) == 0x101 /* fpcNm_Demo_Item_e */)) {
            gabi::call(0x0218432C /* daItemBase_c::dead */, item);
        }
    } else if (cur == 0xCD /* daPyProc_DEMO_LETTER_READ_e */) {
        deleteEquipItem(FALSE);
    } else if (cur == 0x57 /* daPyProc_CUT_ROLL_END_e */) {
        u32 em = gabi::load<u32>(self + 0x67C0); /* m33A8.getEmitter() */
        if (em != 0) {
            gabi::store<u8>(em + 0x247, 0); /* setGlobalAlpha(0) */
        }
    } else if (cur == 0x8D /* daPyProc_SHIP_BOW_e */) {
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
    gabi::store<u32>(self + 0x65F4, gabi::load<u32>(procInit + 0)); /* mCurProcFunc = procInit.mProcFunc */
    m3598 = 0.0f;
    gabi::store<u32>(self + 0x65F8, gabi::load<u32>(procInit + 4));
    BOOL temp_r29 = ((mode >> 1) & 1) ^ 1; /* !checkModeFlg(ModeFlg_MIDAIR) */
    mode = gabi::load<u32>(procInit + 8);
    mModeFlg = mode; /* procInit.mProcFlags */
    if (c2 == 1) {
        mode = mModeFlg;
        m34C2 = 2;
    }
    if (mode & 0x10040820) {
        mode = mModeFlg;
        m34C2 = 0xB;
    }
    u32 acch = gabi::load<u32>(self + 0x834); /* mAcch flags */
    if (mode & 0x20 /* ModeFlg_HANG */) {
        gabi::store<u32>(self + 0x834, acch & ~0x2000u); /* mAcch.OffLineCheck() */
    } else {
        gabi::store<u32>(self + 0x834, acch | 0x2000); /* mAcch.OnLineCheck() */
    }
    /* HD: a timer in the tail (0x8260) */
    if ((gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x200000) || (gabi::load<u32>(dComIfGp_ea() + 0x5CDC) & 8)) {
        LK_FIELD(s32, 0x8260) = 0x1E;
    }
    u32 a = dComIfGp_ea() + 0x5CD8; /* dComIfGp_clearPlayerStatus0(0, ~(BOOMERANG_WAIT | UNK10)) */
    gabi::store<u32>(a, gabi::load<u32>(a) & 0x00400010);
    a = dComIfGp_ea() + 0x5CDC; /* dComIfGp_clearPlayerStatus1 */
    gabi::store<u32>(a, gabi::load<u32>(a) & 0xFFF48400);
    u32 rf0 = resetFlg0() | 0x08000000; /* onResetFlg0(daPyRFlg0_NOT_ATTACKING) */
    u32 nf1 = noResetFlg1();
    gabi::store<u32>(self + 0x680C, 1); /* mFanSwingCb.onAlphaOutFlg() */
    gabi::store<u8>(self + 0x58EC, 0);  /* mSightPacket.offDrawFlg() */
    setNoResetFlg1(nf1 & ~0x08000000u); /* offNoResetFlg1(daPyFlg1_UNK8000000) */
    shape_angle.x = 0;
    setResetFlg0(rf0 & ~2u); /* offResetFlg0(daPyRFlg0_UNK2) */
    u32 nf0 = mNoResetFlg0;
    m3544 = 0;
    m34EC = 0;
    m35C4 = 0.0f;
    m34C5 = 0;
    gabi::store<u8>(self + 0x58ED, 0); /* mSightPacket.offLockFlg() */
    shape_angle.z = 0;
    m34F4 = 0;
    LK_FIELD(f32, 0x374) = -2.5f; /* gravity (HD: constant) */
    mNoResetFlg0 = nf0 & ~4u; /* offNoResetFlg0(daPyFlg0_UNK4) */
    m34F2 = 0;
    BOOL equip = checkEquipAnime();
    mode = mModeFlg;
    if (!(equip && (mode & 4)) && !(mode & 0x1000)) {
        u16 upper = LK_upperAnmIdx();
        if (!(upper == 0x95 || upper == 0x96 || upper == 0x34 /* checkGrabAnime() || checkBoomerangThrowAnime() */) || !(mode & 0x4000)) {
            resetActAnimeUpper(2 /* UPPER_MOVE2_e */, -1.0f);
            mode = mModeFlg;
            if (mode & 0x100000 /* ModeFlg_GRAB */) {
                goto grab_done;
            }
            freeGrabItem();
            mode = mModeFlg;
        }
    }
    if (!(mode & 0x100000)) {
        u16 upper = LK_upperAnmIdx();
        if (upper != 0x95 && upper != 0x96 && mActorKeepGrab.mActor != nullptr) {
            freeGrabItem();
        }
    }
grab_done:
    if (temp_r29 && (mModeFlg & 2 /* ModeFlg_MIDAIR */)) {
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
    if (cp != 0x37 /* daPyProc_SWIM_MOVE_e */) {
        /* mSwimTailEcallBack[0/1].onEnd() */
        gabi::store<u32>(self + 0x66C8, 0);
        gabi::store<u32>(self + 0x66F0, 0);
        gabi::store<u8>(self + 0x66AC, 1);
        gabi::store<u8>(self + 0x66D4, 1);
    }
    if (!(mode & 0x40000)) {
        m3608 = 0.0f;
    }
    resetFootEffect();
    if (resetDemoAnime) {
        resetDemoTextureAnime();
    }
    daPy_followEcallBack_end(self + 0x67A0);              /* m338C.end() */
    gabi::call(0x023D4538 /* daPy_mtxFollowEcallBack_c::end */, self + 0x67BC); /* m33A8 */
    gabi::call(0x023D4538, self + 0x66F8);                                   /* m32E4 */
    gabi::call(0x023D4538, self + 0x6704);                                   /* m32F0 */
    gabi::call(0x025A5F88 /* dPa_smokeEcallBack::end */, self + 0x6710);
    f32 wear = LK_FIELD(f32, 0x3CC);
    u16 item = mEquipItem;
    u32 nf1b = noResetFlg1();
    if (!(wear < 0.0f) /* !checkGrabWear() */) {
        m35D8 = 0.0f;
    }
    m35EC = 0.0f;
    setNoResetFlg1(nf1b & 0xFE7FFF7F); /* offNoResetFlg1(UNK80 | UNK800000 | UNK1000000) */
    if (item == 0x33 /* dItemNo_SKULL_HAMMER_e */) {
        gabi::call(0x025E871C /* mDoExt_bckAnm::changeBckOnly */, self + 0x4444, getItemAnimeResource(0x97 /* HAMMERDAM */));
    } else if (checkBowItem(item) && !checkBowAnime()) {
        gabi::call(0x025E871C, self + 0x4444, getItemAnimeResource(0xD /* ARROWRELORDA */));
    }
    mode = mModeFlg;
    m35E8 = 0.0f;
    if (!(mode & 0x800 /* ModeFlg_ROPE */) && mCurProc != 0x76 /* daPyProc_ROPE_SUBJECT_e */ && mCurProc != 0x7D /* daPyProc_ROPE_MOVE_e */) {
        freeRopeItem();
        mode = mModeFlg;
    }
    if (!(mode & 0x200 /* ModeFlg_HOOKSHOT */)) {
        freeHookshotItem();
    }
    if (temp_r28 && !(mModeFlg & 0x40000)) {
        swimOutAfter(FALSE);
    }
    gabi::store<u8>(dComIfGp_ea() + 0x5BCB, 0); /* dComIfGp_setAdvanceDirection(0) */
    setBgCheckParam();
    return TRUE;
}
VERIFY(0x023DFDD8, &daPy_lk_c::commonProcInit);

/* HD mDoExt_AnmRatioPack::setRatio(pack, v) on a joint subtree: the joints jnt..end-1 of the pb calc's model
 * (end from the joint tree, 027E0174) get the ratio v in the pack's per-joint array (packs at pb + 0x7C, array +0xC) */
static inline s32 lk_jntEnd(u32 pb, s32 jnt) {
    u32 data = gabi::load<u32>(gabi::load<u32>(pb + 0x80) + 0xAC);
    u32 tree = gabi::call<u32>(0x027F3F94, data);
    return gabi::call<s32>(0x027E0174, tree, jnt);
}
static inline void lk_ratioFill(u32 pb, u32 arrOff, s32 from, s32 end, f32 v) {
    for (s32 i = from; i < end; i++) {
        gabi::store<f32>(gabi::load<u32>(gabi::load<u32>(pb + 0x7C) + arrOff) + i * 4, v);
    }
}

/* 023D7A0C */
BOOL daPy_lk_c::jointCB0(int jnt_no) {
    WWHD_FUNC(0x023D7A0C, BOOL, this, jnt_no);
    u32 self = gabi::ea(this);
    if (checkEquipAnime()) {
        if (!(mModeFlg & 1)) {
            if (jnt_no == 2) {
                u32 pb = LK_FIELD(u32, 0x57F4); /* m_pbCalc[PART_UPPER_e] */
                lk_ratioFill(pb, 0x2C, 2, lk_jntEnd(pb, 2), 0.0f);
            } else if (jnt_no == 5 || jnt_no == 9 || jnt_no == 0xD || jnt_no == 0x1D) {
                u32 pb = LK_FIELD(u32, 0x57F4);
                lk_ratioFill(pb, 0x2C, jnt_no, lk_jntEnd(pb, jnt_no), 1.0f);
            }
        }
        return TRUE;
    }
    if (checkUpperReadyAnime() || LK_upperAnmIdx() == 0xE2 /* ROPETHROW */ ||
        (mCurProc != 0x73 /* daPyProc_GRAB_WAIT_e */ && (LK_upperAnmIdx() == 0x95 || LK_upperAnmIdx() == 0x96))) {
        if (jnt_no == 0xF) {
            u32 pb = LK_FIELD(u32, 0x57F4);
            lk_ratioFill(pb, 0x2C, 0xF, lk_jntEnd(pb, 0xF), 0.0f);
            gabi::store<f32>(gabi::load<u32>(gabi::load<u32>(LK_FIELD(u32, 0x57F4) + 0x7C) + 0x2C) + 0x3C, 1.0f);
        } else if (jnt_no == 0x1D) {
            u32 pb = LK_FIELD(u32, 0x57F4);
            lk_ratioFill(pb, 0x2C, 0x1D, lk_jntEnd(pb, 0x1D), 1.0f);
        }
        return TRUE;
    }
    if ((gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x10000) /* daPyStts0_SHIP_RIDE_e */ && !checkShipNotNormalMode()) {
        if (jnt_no == 0xF) {
            u32 pb = LK_FIELD(u32, 0x57F4);
            lk_ratioFill(pb, 0x2C, 0xF, lk_jntEnd(pb, 0xF), 1.0f);
            gabi::store<f32>(gabi::load<u32>(gabi::load<u32>(LK_FIELD(u32, 0x57F4) + 0x7C) + 0x2C) + 0x3C, 0.0f);
        } else if (jnt_no == 0x1D) {
            u32 pb = LK_FIELD(u32, 0x57F4);
            lk_ratioFill(pb, 0x2C, 0x1D, lk_jntEnd(pb, 0x1D), 0.0f);
        }
        return TRUE;
    }
    u16 upper = LK_upperAnmIdx();
    const u32 guard_rate = 0x101CEE60; /* function-local static */
    if (upper == 0x16 || upper == 0x1B) { /* checkUpperGuardAnime() */
        if (jnt_no == 1) {
            u32 pb = LK_FIELD(u32, 0x57F4);
            fcpy_l(guard_rate, gabi::load<u32>(gabi::load<u32>(pb + 0x7C) + 0x2C) + 4); /* m_pbCalc[PART_UPPER_e]->getRatio(2) */
            lk_ratioFill(pb, 0x2C, 1, lk_jntEnd(pb, 1), 1.0f);
        } else if (jnt_no == 0xE) {
            u32 pb = LK_FIELD(u32, 0x57F4);
            lk_ratioFill(pb, 0x2C, 0xE, lk_jntEnd(pb, 0xE), 1.0f);
        } else if (jnt_no == 5 || jnt_no == 9 || jnt_no == 0xD || jnt_no == 0x1D) {
            u32 pb = LK_FIELD(u32, 0x57F4);
            f32 r = gabi::load<f32>(guard_rate);
            lk_ratioFill(pb, 0x2C, jnt_no, lk_jntEnd(pb, jnt_no), r);
        }
        return TRUE;
    }
    if ((gabi::load<u32>(dComIfGp_ea() + 0x5CDC) & 1) /* daPyStts1_WIND_WAKER_CONDUCT_e */ && (mCurProc != 0x9B /* daPyProc_TACT_PLAY_e */ || mProcVar6 != 0)) {
        if (jnt_no == 5) {
            u32 pb = LK_FIELD(u32, 0x57F4);
            lk_ratioFill(pb, 0x2C, 5, lk_jntEnd(pb, 5), 1.0f);
        } else if (jnt_no == 9) {
            u32 pb = LK_FIELD(u32, 0x57F4);
            lk_ratioFill(pb, 0x1C, 9, lk_jntEnd(pb, 9), 1.0f); /* setRatio(1, 1.0f) */
            pb = LK_FIELD(u32, 0x57F4);
            lk_ratioFill(pb, 0x2C, 9, lk_jntEnd(pb, 9), 0.0f);
        } else if (jnt_no == 0xD || jnt_no == 0xE || jnt_no == 0x1D) {
            u32 pb = LK_FIELD(u32, 0x57F4);
            lk_ratioFill(pb, 0x1C, jnt_no, lk_jntEnd(pb, jnt_no), 0.0f);
        }
    }
    (void)self;
    return TRUE;
}
VERIFY(0x023D7A0C, &daPy_lk_c::jointCB0);

/* 023D8624 */
BOOL daPy_lk_c::setDrawHandModel() {
    WWHD_FUNC(0x023D8624, BOOL, this);
    u32 self = gabi::ea(this);
#define LK_checkPlayerGuard() gabi::call_ptr<BOOL>(gabi::load<u32>(gabi::load<u32>(self + 0xB4) + 0x3C), this) /* virtual */
    gabi::store<u8>(LK_FIELD(u32, 0x63C) + 4, 0); /* mpLhandShape->hide() */
    gabi::store<u8>(LK_FIELD(u32, 0x640) + 4, 0); /* mpRhandShape->hide() */
    u32 l, r;
    if (mCurProc == 0xA9 /* daPyProc_DEMO_TOOL_e */) {
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
                if (gabi::load<u8>(dComIfGs_base_l() + 0x2F) == 0xFF /* !checkShieldEquip() */) {
                    r = mRightHandIdx;
                } else {
                    r = 8;
                }
            } else if ((item == 0x25 && !(mModeFlg & 0x800)) || (item == 0x33 && gabi::load<u8>(self + 0x3AC) == 0 /* mCutType == CUT_TYPE_NONE */)) {
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
                if (LK_checkPlayerGuard() || mCurProc == 0x65 /* daPyProc_GUARD_CRASH_e */) {
                    r = 8;
                } else if (mEquipItem == 0x100 && LK_upperAnmIdx() == 0x104) {
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
    if (l == 0 /* HANDS_JNT_WORLD_ROOT_e */) {
        u32 shape = gabi::load<u32>(gabi::load<u32>(lk_jointNode(gabi::ea(mpCLModelData.get()), 8 /* CL_JNT_CL_LHANDA_e */) + 0x10) + 8);
        LK_FIELD(u32, 0x63C) = shape;
        gabi::store<u8>(shape + 4, 1);
    } else {
        u32 hdata = gabi::load<u32>(LK_FIELD(u32, 0xCF4) + 0xAC); /* mpHandsModel->getModelData() */
        LK_FIELD(u32, 0x63C) = gabi::load<u32>(gabi::load<u32>(lk_jointNode(hdata, l) + 0x10) + 8);
        u32 src = lk_getAnmMtx(gabi::ea(mpCLModel.get()), 8);
        mtx_copy_q(lk_getAnmMtx(LK_FIELD(u32, 0xCF4), l), src); /* mpHandsModel->setAnmMtx(l, mpCLModel->getAnmMtx(CL_LHANDA)) */
        gabi::store<u8>(LK_FIELD(u32, 0x63C) + 4, 1);
    }
    if (LK_upperAnmIdx() == 0xE) {
        /* HD: while shooting the bow from the ship (or the HD proc 0x94) in the first-person camera, a hand material flag is cleared */
        u32 body = gabi::call<u32>(0x024F8044 /* dCam_getBody */);
        if (gabi::load<u8>(body + 0x100) != 0 && gabi::load<u8>(body + 0x101) != 0 && gabi::load<u8>(body + 0x102) != 0 &&
            (mCurProc == 0x94 || mCurProc == 0x8D)) {
            u32 hands = LK_FIELD(u32, 0xCF4);
            u32 mesh = gabi::load<u32>(lk_jointNode(gabi::load<u32>(hands + 0xAC), l) + 0x10);
            u16 idx = gabi::load<u16>(gabi::load<u32>(mesh) + 0xC);
            u32 m = 0;
            if (idx < gabi::load<u32>(hands + 0x138)) {
                m = gabi::load<u32>(hands + 0x13C) + idx * 0x38;
            }
            gabi::store<u32>(m + 0x30, gabi::load<u32>(m + 0x30) & ~6u);
        }
    }
    if (r == 0) {
        u32 shape = gabi::load<u32>(gabi::load<u32>(lk_jointNode(gabi::ea(mpCLModelData.get()), 0xC /* CL_JNT_CL_RHANDA_e */) + 0x10) + 8);
        LK_FIELD(u32, 0x640) = shape;
        gabi::store<u8>(shape + 4, 1);
    } else {
        u32 hdata = gabi::load<u32>(LK_FIELD(u32, 0xCF4) + 0xAC);
        LK_FIELD(u32, 0x640) = gabi::load<u32>(gabi::load<u32>(lk_jointNode(hdata, r) + 0x10) + 8);
        u32 src = lk_getAnmMtx(gabi::ea(mpCLModel.get()), 0xC);
        mtx_copy_q(lk_getAnmMtx(LK_FIELD(u32, 0xCF4), r), src);
        gabi::store<u8>(LK_FIELD(u32, 0x640) + 4, 1);
    }
    return TRUE;
}
VERIFY(0x023D8624, &daPy_lk_c::setDrawHandModel);

/* 023D5548 */
BOOL daPy_lk_c::createHeap() {
    WWHD_FUNC(0x023D5548, BOOL, this);
    u32 self = gabi::ea(this);
#define LK_ASSERT(line, msg) JUT_ASSERT_fail(gabi::at<const char>(0x10035258) /* "d_a_player_main.cpp" */, (line), gabi::at<const char>(msg))
    LK_FIELD(u32, 0x8260) = 0; /* HD tail */
    mpCLModelData = gabi::at<J3DModelData>(initModel(self + 0x448 /* mpCLModel */, 0x18 /* LINK_BDL_CL */, 0x37221222));
    initModel(self + 0xCF4 /* mpHandsModel */, 0x1D, 0x37221222);
    initModel(self + 0x44C /* mpKatsuraModel */, 0x20, 0x37221222);
    initModel(self + 0x450 /* mpYamuModel */, 0x29, 0x37221222);
    initModel(self + 0x4434, 0x23, 0x37221222);
    initModel(self + 0xCFC /* mpSwgripaModel */, 0x26, 0x37221222);
    u32 data = initModel(self + 0xD00 /* mpSwgripmsModel */, 0x45, 0x37221222);
    u32 model = LK_FIELD(u32, 0xD00);
    u32 res = lk_getLinkRes_l(0xF);
    if (!gabi::call<BOOL>(0x025E8508 /* mDoExt_bckAnm::init */, self + 0xD04 /* mSwgripmsabBckAnim */, gabi::load<u32>(model + 0xAC), res, 0, 2,
                          1.0f, 0, -1, 0)) {
        LK_ASSERT(0x5D32, 0x1003520C);
    }
    /* HD: the brk/btk animations of the models are objects (initBrkAnm / initBtkAnm) */
    initBrkAnm(data, self + 0xD90, 0x56);
    initBtkAnm(data, self + 0xE08, 0x67);
    initModel(self + 0xE7C, 0x22, 0x37221222);
    initModel(self + 0xE84, 0x24, 0x37221222);
    model = LK_FIELD(u32, 0xE84);
    res = lk_getLinkRes_l(0xA);
    if (!gabi::call<BOOL>(0x025E8508, self + 0xE8C, gabi::load<u32>(model + 0xAC), res, 0, 0, 1.0f, 0, -1, 0)) {
        LK_ASSERT(0x5D44, 0x1003520C);
    }
    data = initModel(self + 0xE88, 0x43, 0x37221222);
    initBtkAnm(data, self + 0xF18, 0x66);
    data = initModel(self + 0x43B4 /* mpYmsls00Model */, 0x4D, 0x13000222);
    initBtkAnm(data, self + 0x43B8, 0x6C);
    initModel(self + 0x442C, 0x2D, 0x37221222);
    initModel(self + 0x4430, 0x2D, 0x37221222);
    data = initModel(self + 0x5378, 0x44, 0x11001222);
    res = lk_getLinkRes_l(0x64);
    s32 mode = anm_vf0C_l(res);
    mDoExt_btkAnm_init_l(self + 0x537C, data, res, 1, mode, 1.0f, 0, -1, 0, 0);
    gabi::call(0x025E7FC4 /* mDoExt_btkAnm::entry */, self + 0x537C, data, 0.0f);
    data = initModel(self + 0x53F0, 0x4E, 0x13000222);
    res = lk_getLinkRes_l(0x10);
    if (!gabi::call<BOOL>(0x025E8508, self + 0x53F4, data, res, 1, 2, 1.0f, 0, -1, 0)) {
        LK_ASSERT(0x5D85, 0x1003520C);
    }
    initBtkAnm(data, self + 0x5480, 0x6D);
    initBrkAnm(data, self + 0x54F4, 0x5B);
    lk_anmObjSetFrame(self + 0x54F4, 0x10, 0x20, (f32)gabi::load<s16>(self + 0x54FE) - 0.001f); /* frame max - 0.001 */
    data = initModel(self + 0x556C, 0x49, 0x13000222);
    initBtkAnm(data, self + 0x5570, 0x6A);
    lk_anmObjSetFrame(self + 0x5570, 0x68, 0x10, (f32)gabi::load<s16>(self + 0x557A) - 0.001f);
    u32 mdata = lk_getLinkRes_l(0x48);
    if (mdata == 0) {
        LK_ASSERT(0x5D96, 0x100352C8);
    }
    for (u32 i = 0; i < 6; i++) {
        u32 m = gabi::ea(mDoExt_J3DModel__create(gabi::at<J3DModelData>(mdata), 0x80000, 0x11001222));
        gabi::store<u32>(self + 0x55E4 + i * 8, m);
        if (m == 0) {
            LK_ASSERT(0x5D9F, 0x1003526C);
        }
    }
    initBtkAnm(mdata, self + 0x568C, 0x69);
    res = lk_getLinkRes_l(0x58);
    if (!mDoExt_brkAnm_init_l(self + 0x5614, mdata, res, 0, 2, 1.0f, 0, -1, 0, 0)) {
        LK_ASSERT(0x5DA7, 0x1003520C);
    }
    lk_anmObjSetFrame(self + 0x5614, 0x10, 0x20, 0.0f);
    data = initModel(self + 0x5700, 0x4B, 0x11001222);
    initBtkAnm(data, self + 0x577C, 0x6B);
    res = lk_getLinkRes_l(0x59);
    if (!mDoExt_brkAnm_init_l(self + 0x5704, data, res, 0, 2, 1.0f, 0, -1, 0, 0)) {
        LK_ASSERT(0x5DB5, 0x1003520C);
    }
    lk_anmObjSetFrame(self + 0x5704, 0x10, 0x20, 0.0f);
    static const u32 anmLines[][2] = {{0x3C, 0x5DBD}, {0x42, 0x5DC4}, {0x37, 0x5DCC}, {0x38, 0x5DD3}, {0x40, 0x5DDA}, {0x41, 0x5DE1}};
    for (int k = 0; k < 6; k++) {
        mdata = lk_getLinkRes_l(anmLines[k][0]);
        if (mdata == 0) {
            LK_ASSERT(anmLines[k][1], 0x100352C8);
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
    u32 ti = gabi::call<u32>(0x0273ADAC /* operator new[] */, 0x540);
    u32 q = gabi::call<u32>(0x0273ADAC, 0x2A0);
    u32 od = gabi::ea(operator_new(0x24));
    if (od != 0) {
        gabi::store<u16>(od + 0x18, 0);
        gabi::store<u16>(od + 0x1A, 0);
        gabi::store<u8>(od + 1, 1);
        gabi::store<u8>(od + 0, 0);
        gabi::store<f32>(od + 0xC, 0.0f);
        gabi::store<f32>(od + 4, 0.0f);
        gabi::store<f32>(od + 8, 0.0f);
        gabi::store<f32>(od + 0x10, 0.0f);
        gabi::store<f32>(od + 0x14, 0.0f);
        gabi::store<u32>(od + 0x1C, ti);
        gabi::store<u32>(od + 0x20, q);
    }
    m_old_fdata = od;
    if (od == 0) {
        LK_ASSERT(0x5DFE, 0x100352E0);
    }
    /* HD: the anime ratio packs hold one ratio per joint (0x2A) */
    for (u32 k = 0; k < 2; k++) {
        gabi::store<u32>(self + 0x57F8 + k * 0x10 + 8, 0x2A);
        gabi::store<u32>(self + 0x57F8 + k * 0x10 + 0xC, gabi::call<u32>(0x0273ADAC, 0xA8));
    }
    for (u32 k = 0; k < 3; k++) {
        gabi::store<u32>(self + 0x5818 + k * 0x10 + 8, 0x2A);
        gabi::store<u32>(self + 0x5818 + k * 0x10 + 0xC, gabi::call<u32>(0x0273ADAC, 0xA8));
    }
    u32 pb = gabi::call<u32>(0x025E3FB4 /* new mDoExt_MtxCalcAnmBlendTblOld */, 0, (u32)m_old_fdata, 2, self + 0x57F8, gabi::ea(mpCLModel.get()));
    m_pbCalc[0] = pb;
    if (pb == 0) {
        LK_ASSERT(0x5E0D, 0x10035288);
    }
    pb = gabi::call<u32>(0x025E3FB4, 0, (u32)m_old_fdata, 3, self + 0x5818, gabi::ea(mpCLModel.get()));
    m_pbCalc[1] = pb;
    if (pb == 0) {
        LK_ASSERT(0x5E14, 0x100352A8);
    }
    gabi::call(0x025E4050, (u32)m_pbCalc[0], 0);
    gabi::call(0x025E4050, (u32)m_pbCalc[1], 0);
    initTextureScroll();
    initTextureAnime();
    u32 hio = gabi::ea(operator_new(0x40)); /* HD: m_HIO is only allocated */
    m_HIO = hio;
    if (hio == 0) {
        LK_ASSERT(0x5E26, 0x100352F4);
    }
    u32 b = operator_new_arr_align_l(0xB400, 0x20);
    LK_FIELD(u32, 0x5850) = b; /* m_anm_heap_under[UNDER_MOVE0_e].m_buffer */
    if (b == 0) {
        LK_ASSERT(0x5E2B, 0x10035218);
    }
    b = operator_new_arr_align_l(0x200, 0x20);
    LK_FIELD(u32, 0x658C) = b; /* the SE animation buffer */
    if (b == 0) {
        LK_ASSERT(0x5E32, 0x10035304);
    }
    b = operator_new_arr_align_l(0x1000, 0x20);
    LK_FIELD(u32, 0x480C) = b; /* m_item_bck_buffer */
    if (b == 0) {
        LK_ASSERT(0x5E35, 0x1003531C);
    }
    gabi::Local<SafeString> agb;
    agb->__vtbl = 0x10034B24;
    agb->mStringTop = 0x10035208; /* "Agb" */
    u32 shadowData = gabi::call<u32>(0x026066C4, dComIfG_resControl(), agb.get(), 4);
    if (shadowData == 0) {
        LK_ASSERT(0x5E3C, 0x10035338);
    }
    u32 shadow = gabi::ea(mDoExt_J3DModel__create(gabi::at<J3DModelData>(shadowData), 0, 0x11020203));
    LK_FIELD(u32, 0x438) = shadow; /* HD: the shadow model (see drawShadow) */
    if (shadow == 0) {
        LK_ASSERT(0x5E3E, 0x10035248);
    }
    LK_FIELD(u8, 0x4BE4) = 0;
    LK_FIELD(s32, 0x4B58) = 0;
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
        gabi::call(0x0207FD38, self + 0x4BE8, 0);
        LK_FIELD(u8, 0x4C95) = 1;
        LK_FIELD(u8, 0x4C97) = 1;
        gabi::call(0x0207FD38, self + 0x52C8, 0);
        LK_FIELD(u8, 0x5375) = 1;
        LK_FIELD(u8, 0x5376) = 1;
        for (u32 o = 0x4C98; o <= 0x5218; o += 0xB0) {
            gabi::call(0x0207FD38, self + o, 0);
        }
        LK_FIELD(u8, 0x4BE4) = 1;
    }
#undef LK_ASSERT
    return TRUE;
}
VERIFY(0x023D5548, &daPy_lk_c::createHeap);

/* ======== draw ======== */
/* HD J3DModel draw flags (+0x74) and their update (027F596C, the new flags passed in r4) */
static inline void lk_mdlFlags(u32 m, u32 v) {
    gabi::store<u32>(m + 0x74, v);
    gabi::call(0x027F596C, m, v);
}
/* the HD two-step flag update of a model field: clear the low three flags, then set flag 1 */
static inline void lk_mdlFlags2(u32 self, u32 off) {
    u32 m = gabi::load<u32>(self + off);
    lk_mdlFlags(m, gabi::load<u32>(m + 0x74) & ~7u);
    m = gabi::load<u32>(self + off);
    lk_mdlFlags(m, gabi::load<u32>(m + 0x74) | 1);
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
static inline void lk_shapeShow(u32 node, u8 v) { gabi::store<u8>(gabi::load<u32>(gabi::load<u32>(node + 0x10) + 8) + 4, v); }
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
    gabi::call(0x027F0E04 /* J3DDrawBuffer::entryImm */, gabi::load<u32>(LK_J3DSYS + 0x74), pkt, 0);
}
static inline void lk_setList(u32 opa, u32 xlu) {
    gabi::store<u32>(LK_J3DSYS + 0x74, gabi::load<u32>(dComIfGp_ea() + opa));
    gabi::store<u32>(LK_J3DSYS + 0x78, gabi::load<u32>(dComIfGp_ea() + xlu));
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

/* 023D9820: draw (the matcher names it drawShadow) */
BOOL daPy_lk_c::draw() {
    WWHD_FUNC(0x023D9820, BOOL, this);
    u32 self = gabi::ea(this);
    u32 tev = self + 0x110;
#define LK_freeze() (noResetFlg1() & 0x800) /* checkFreezeState() */
    LK_FIELD(u8, 0x5374) = 0;
    u8 drawFlg = gabi::load<u8>(self + 0x58EC);
    LK_FIELD(u8, 0x4C94) = 0;
    if (drawFlg) {
        daPy_sightPacket_setSight(self + 0x58E8);
    }
    settingTevStruct(dKy_getEnvlight(), 9 /* TEV_TYPE_PLAYER */, &current.pos, gabi::at<dKy_tevstr_c>(tev));
    if (gabi::call_ptr<BOOL>(gabi::load<u32>(gabi::load<u32>(self + 0xB4) + 0xCC), this) /* checkPlayerNoDraw() */) {
        offBodyEffect();
        drawMirrorLightModel();
        /* HD: Link is still drawn (without effects) in some states */
        BOOL st1 = (gabi::load<u32>(dComIfGp_ea() + 0x5CDC) & 0x100000) != 0;
        u8 st0 = 0;
        if ((gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x08000000) && (gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x2000)) {
            st0 = 1;
        }
        if ((mCurProc == 0 || st0) && !st1) {
            return TRUE;
        }
        gabi::Local<SafeString> nm;
        for (u32 mtl = gabi::load<u32>(gabi::load<u32>(gabi::load<u32>(self + 0x444) + 8) + 0x10); mtl != 0; mtl = gabi::load<u32>(mtl + 4)) {
            u8 v = 1;
            if (lk_nameIsGlobal(mtl, 0x1046CCC4, gabi::ea(nm.get())) && (checkCaughtShapeHide() || (noResetFlg1() & 8))) {
                v = 0;
            }
            gabi::store<u8>(gabi::load<u32>(mtl + 8) + 4, v);
        }
        if (noResetFlg1() & 8) {
            lk_shapeShow(lk_jointNode(gabi::load<u32>(self + 0x444), 0x29), 0);
        }
        setDrawHandModel();
        u32 d = gabi::load<u32>(self + 0x444);
        u32 lshape = gabi::load<u32>(gabi::load<u32>(lk_jointNode(d, 8) + 0x10) + 8);
        u32 rshape = gabi::load<u32>(gabi::load<u32>(lk_jointNode(d, 0xC) + 0x10) + 8);
        if (lshape != LK_FIELD(u32, 0x63C)) {
            lk_shapeShow(lk_jointNode(d, 8), 0);
        }
        if (rshape != LK_FIELD(u32, 0x640)) {
            lk_shapeShow(lk_jointNode(gabi::load<u32>(self + 0x444), 0xC), 0);
        }
        lk_mdlFlags2(self, 0x448);
        gabi::store<u32>(gabi::load<u32>(self + 0x444) + 0x38, 0);
        gabi::store<u32>(gabi::load<u32>(self + 0x444) + 0x3C, 0);
        gabi::store<u32>(gabi::load<u32>(self + 0x444) + 0x44, 0);
        mDoExt_modelEntryDL(mpCLModel);
        lk_mdlFlags2(self, 0xCF4);
        entryDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0xCF4)), noResetFlg1() & 0x800);
        if ((noResetFlg1() & 8) && !checkCaughtShapeHide() && !(lk_camAttnStatus(this, self + 0x69BC) & 0x20)) {
            lk_mdlFlags2(self, 0x44C);
            entryDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x44C)), noResetFlg1() & 0x800);
        }
        u8 sw = gabi::load<u8>(dComIfGs_base_l() + 0x2E);
        if ((sw == 0x39 || sw == 0x3A || sw == 0x3E) /* checkMasterSwordEquip() */ && !checkCaughtShapeHide() && !checkDemoShieldNoDraw()) {
            lk_mdlFlags2(self, 0xE7C);
            updateDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0xE7C)), noResetFlg1() & 0x800);
        }
        if ((noResetFlg1() & 0x800) && checkMaskDraw()) {
            lk_mdlFlags2(self, 0x450);
            entryDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x450)), noResetFlg1() & 0x800);
        }
        if (gabi::load<u8>(dComIfGs_base_l() + 0x30) == 0x28 /* checkPowerGloveEquip() */) {
            lk_mdlFlags2(self, 0x4434);
            entryDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x4434)), noResetFlg1() & 0x800);
        }
        if (mNoResetFlg0 & 0x2000000 /* checkEquipHeavyBoots() */) {
            lk_mdlFlags2(self, 0x442C);
            lk_mdlFlags2(self, 0x4430);
            entryDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x442C)), noResetFlg1() & 0x800);
            entryDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x4430)), noResetFlg1() & 0x800);
        }
        if (!(lk_camAttnStatus(this, self + 0x69BC) & 0x20)) {
            if ((gabi::load<u8>(dComIfGs_base_l() + 0x2E) != 0xFF || gabi::load<u8>(dComIfGp_ea() + 0x5CEA) == 2) /* checkSwordEquip() */ &&
                !checkDemoSwordNoDraw(1)) {
                lk_mdlFlags2(self, 0xCF8);
                entryDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0xCF8)), noResetFlg1() & 0x800);
            }
        }
        if (gabi::load<u8>(dComIfGs_base_l() + 0x2F) != 0xFF /* checkShieldEquip() */ && !checkCaughtShapeHide() && !checkDemoShieldNoDraw()) {
            lk_mdlFlags2(self, 0xE80);
            entryDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0xE80)), noResetFlg1() & 0x800);
        }
        u32 bc = LK_FIELD(u32, 0x4970);
        if (bc != 0) {
            gabi::Local<SafeString> n1;
            gabi::Local<SafeString> k1;
            gabi::store<u32>(gabi::ea(n1.get()) + 4, 0x10034B24);
            gabi::store<u32>(gabi::ea(n1.get()) + 0, lk_relPtr(gabi::load<u32>(bc + 0x14) + 4)); /* the contents model's name */
            k1->__vtbl = 0x10034B24;
            k1->mStringTop = 0x100353C4; /* "binho" */
            if (!lk_keyEq(gabi::ea(n1.get()), gabi::ea(k1.get()))) {
                lk_mdlFlags2(self, 0x4970);
                updateDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x4970)), 0);
            }
        }
        if (LK_FIELD(u32, 0x4440) != 0 && !checkCaughtShapeHide() && !checkDemoSwordNoDraw(0)) {
            u16 item = mEquipItem;
            bool skip = false;
            if (checkBowItem(item)) {
                skip = gabi::call_ptr<BOOL>(gabi::load<u32>(gabi::load<u32>(self + 0xB4) + 0x3C), this) != 0; /* checkPlayerGuard() */
                item = mEquipItem;
            }
            if (!skip) {
                if (item == 0x2F /* dItemNo_HOOKSHOT_e */) {
                    lk_setHookshotTipMtx(self);
                }
                lk_mdlFlags2(self, 0x4440);
                entryDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x4440)), noResetFlg1() & 0x800);
            }
        }
        if (LK_FIELD(u32, 0x4974) != 0 && m355E != 0) {
            lk_mdlFlags2(self, 0x4974);
            updateDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x4974)), 0);
        }
        return TRUE;
    }

    static const u32 mdls[] = {0x448, 0xCF4, 0x44C, 0xE7C, 0x450, 0x4434};
    for (u32 off : mdls) {
        u32 m = gabi::load<u32>(self + off);
        if (m != 0) {
            lk_mdlFlags(m, gabi::load<u32>(m + 0x74) | 7);
        }
    }
    for (u32 off = 0x442C; off <= 0x4430; off += 4) { /* mpHbootsModels[2] (no NULL check) */
        u32 m = gabi::load<u32>(self + off);
        lk_mdlFlags(m, gabi::load<u32>(m + 0x74) | 7);
    }
    static const u32 mdls2[] = {0xCF8, 0xE80, 0x4970, 0x4440, 0x4974};
    for (u32 off : mdls2) {
        u32 m = gabi::load<u32>(self + off);
        if (m != 0) {
            lk_mdlFlags(m, gabi::load<u32>(m + 0x74) | 7);
        }
    }
    if (lk_camAttnStatus(this, self + 0x69BC) & 0x20 /* dCamAttnStts_00000020_e */) {
        offBodyEffect();
    } else {
        onBodyEffect();
    }
    /* GXColorS10 origFog, origFogStartZ, origFogEndZ */
    s16 fogG = gabi::load<s16>(tev + 0xA2);
    f32 fogEnd = gabi::load<f32>(tev + 0xAC);
    s16 fogB = gabi::load<s16>(tev + 0xA4);
    u32 nf1 = noResetFlg1();
    s16 fogR = gabi::load<s16>(tev + 0xA0);
    f32 fogStart = gabi::load<f32>(tev + 0xA8);
    if (!(nf1 & 0x800) && mCurProc != 0x6C /* daPyProc_ELEC_DAMAGE_e */ && !(gabi::load<u32>(dComIfGp_ea() + 0x5CDC) & 0x100000) &&
        (gabi::load<u16>(0x101CEF16) == 1 /* checkCurse() */ || (noResetFlg1() & 0x100) /* checkConfuse() */ || mDamageWaitTimer > 0)) {
        gabi::Local<cXyz> sp18;
        gabi::call(0x025F1108 /* mDoLib_pos2camera */, &current.pos, sp18.get());
        u32 cnt = gabi::load<u32>(0x101FF560); /* g_Counter.mTimer */
        f32 f2 = std::fabs(gabi::load<f32>(0x104A44F8 + ((cnt << 11) & 0xF800))); /* std::abs(cM_ssin(g_Counter.mTimer * 0x800)) */
        if (gabi::load<u16>(0x101CEF16) == 1 || (noResetFlg1() & 0x100)) {
            gabi::store<s16>(tev + 0xA0, 0x80);
            f32 start = gabi::fmadds(200.0f, f2, -sp18->z - 200.0f);
            gabi::store<s16>(tev + 0xA2, 0);
            gabi::store<s16>(tev + 0xA4, 0xFF);
            gabi::store<f32>(tev + 0xA8, start);
            gabi::store<f32>(tev + 0xAC, start + 300.0f);
        } else {
            gabi::store<s16>(tev + 0xA0, 0xFF);
            gabi::store<s16>(tev + 0xA2, 0x3C);
            f32 start = gabi::fmadds(200.0f, f2, -sp18->z - 200.0f);
            gabi::store<s16>(tev + 0xA4, 0x3C);
            gabi::store<f32>(tev + 0xA8, start);
            gabi::store<f32>(tev + 0xAC, start + 300.0f);
        }
    }
    /* HD: the eye/mouth animations are the HD objects at 0x644 / 0x69C / 0x6F4, attached to the model data */
    gabi::call(0x027E1038, self + 0x644);
    gabi::store<u32>(gabi::load<u32>(self + 0x444) + 0x38, self + 0x644);
    gabi::call(0x027E1038, self + 0x69C);
    gabi::store<u32>(gabi::load<u32>(self + 0x444) + 0x3C, self + 0x69C);
    gabi::call(0x027DF40C, self + 0x6F4);
    gabi::store<u32>(gabi::load<u32>(self + 0x444) + 0x44, self + 0x6F4);
    gabi::store<f32>(LK_FIELD(u32, 0x5F4), (f32)(u16)m3530); /* mpAnmTexPatternData->setFrame(m3530) */
    gabi::store<f32>(LK_FIELD(u32, 0x5FC), (f32)(u16)m3532); /* mpTexScrollResData->setFrame(m3532) */
    static const u32 ctrls[3] = {0x644, 0x69C, 0x6F4};
    for (int k = 0; k < 3; k++) {
        u32 c = gabi::load<u32>(self + ctrls[k]);
        f32 fr = (f32)(u16)(k < 2 ? m3530 : m3532);
        f64 r = gabi::call_ptr<f64>(gabi::load<u32>(c + 0x10), gabi::load<u32>(c + 0x14), fr, gabi::load<f32>(c + 4), gabi::load<f32>(c + 8));
        gabi::store<f32>(c, (f32)r);
    }
    setLightTevColorType(dKy_getEnvlight(), mpCLModel, gabi::at<dKy_tevstr_c>(tev));
    u32 d = gabi::load<u32>(self + 0x444);
    u32 rootNode = gabi::load<u32>(d + 8); /* mpCLModelData->getJointNodePointer(CL_JNT_LINK_ROOT_e) */
    lk_shapeShow(lk_jointNode(d, 8 /* CL_JNT_CL_LHANDA_e */), 0);
    lk_shapeShow(lk_jointNode(gabi::load<u32>(self + 0x444), 0xC /* CL_JNT_CL_RHANDA_e */), 0);
    {
        /* HD: the hat material is found by name (0x1046CCC4) */
        d = gabi::load<u32>(self + 0x444);
        u32 x = gabi::load<u32>(d);
        SafeString_vf14_l(0x1046CCC4);
        s32 idx = gabi::call<s32>(0x027DF9B0 /* material name -> index */, lk_relPtr(x + 0x18), gabi::load<u32>(0x1046CCC4));
        u32 mat = 0;
        if (idx >= 0) {
            mat = gabi::load<u32>(d + 0x10);
            if ((u32)idx < gabi::load<u32>(d + 0xC)) {
                mat += idx * 0x39C;
            }
        }
        gabi::store<u8>(gabi::load<u32>(mat + 8) + 4, 1); /* show */
    }
    u32 eye = lk_jointNode(gabi::load<u32>(self + 0x444), 0x13 /* CL_JNT_CL_EYE_e */);
    u32 mayu = lk_jointNode(gabi::load<u32>(self + 0x444), 0x15 /* CL_JNT_CL_MAYU_e */);
    setDrawHandModel();
    gabi::store<u32>(LK_J3DSYS + 0x6C, LK_FIELD(u32, 0x448)); /* j3dSys.setModel(mpCLModel) */
    BOOL r24 = !(LK_FIELD(f32, 0x3CC) > -85.0f); /* field_0x2b0 <= -85.0f */
    gabi::Local<SafeString> s1, s2;
    if (r24) {
        for (u32 i = 0; i < 4; i++) {
            gabi::store<u8>(LK_FIELD(u32, 0x60C + i * 4) + 4, 0); /* mpZOffBlendShape[i]->hide() */
            gabi::store<u8>(LK_FIELD(u32, 0x61C + i * 4) + 4, 0); /* mpZOffNoneShape[i]->hide() */
            gabi::store<u8>(LK_FIELD(u32, 0x62C + i * 4) + 4, 0); /* mpZOnShape[i]->hide() */
        }
        lk_shapeShow(lk_jointNode(gabi::load<u32>(self + 0x444), 8), 0);
        lk_shapeShow(lk_jointNode(gabi::load<u32>(self + 0x444), 0xC), 0);
        /* HD: only the material named 0x1046CCBC (the legs) stays visible */
        for (u32 mtl = gabi::load<u32>(rootNode + 0x10); mtl != 0; mtl = gabi::load<u32>(mtl + 4)) {
            u8 v = lk_nameIsGlobal(mtl, 0x1046CCBC, gabi::ea(s1.get())) ? 1 : 0;
            gabi::store<u8>(gabi::load<u32>(mtl + 8) + 4, v);
        }
    } else if (lk_camAttnStatus(this, self + 0x69BC) & 0x20) {
        for (u32 i = 0; i < 4; i++) {
            gabi::store<u8>(LK_FIELD(u32, 0x60C + i * 4) + 4, 0);
            gabi::store<u8>(LK_FIELD(u32, 0x61C + i * 4) + 4, 0);
            gabi::store<u8>(LK_FIELD(u32, 0x62C + i * 4) + 4, 0);
        }
        /* HD: the sleeve (0x1046CCA4) and leg (0x1046CCBC) materials stay visible; the others are drawn without
         * colour/alpha updates (material flags +0x30) */
        for (u32 mtl = gabi::load<u32>(rootNode + 0x10); mtl != 0; mtl = gabi::load<u32>(mtl + 4)) {
            bool show = lk_nameIsGlobal(mtl, 0x1046CCA4, gabi::ea(s1.get())) || lk_nameIsGlobal(mtl, 0x1046CCBC, gabi::ea(s2.get()));
            u32 cl = LK_FIELD(u32, 0x448);
            if (show) {
                gabi::store<u8>(gabi::load<u32>(mtl + 8) + 4, 1);
                cl = LK_FIELD(u32, 0x448);
            }
            u16 mi = gabi::load<u16>(gabi::load<u32>(mtl) + 0xC);
            u32 m = 0;
            if (mi < gabi::load<u32>(cl + 0x138)) {
                m = gabi::load<u32>(cl + 0x13C) + mi * 0x38;
            }
            if (show) {
                gabi::store<u32>(m + 0x30, gabi::load<u32>(m + 0x30) | 7);
            } else {
                gabi::store<u32>(m + 0x30, gabi::load<u32>(m + 0x30) & ~7u);
                cl = LK_FIELD(u32, 0x448);
                mi = gabi::load<u16>(gabi::load<u32>(mtl) + 0xC);
                m = 0;
                if (mi < gabi::load<u32>(cl + 0x138)) {
                    m = gabi::load<u32>(cl + 0x13C) + mi * 0x38;
                }
                gabi::store<u32>(m + 0x30, gabi::load<u32>(m + 0x30) | 1);
            }
        }
        lk_shapeShow(lk_jointNode(gabi::load<u32>(self + 0x444), 0x14 /* CL_JNT_CL_HANA_e */), 0);
    } else {
        u32 cl = LK_FIELD(u32, 0x448);
        lk_mdlFlags(cl, gabi::load<u32>(cl + 0x74) | 7);
        if (!(noResetFlg1() & 0x800)) {
            lk_setList(0x5D54, 0x5D54); /* dComIfGd_setListP0() */
            lk_staticPacketEntry(0x1046CE20, 0x1046D088, 0x10058D40, 0x101CEE64); /* l_onCupOffAupPacket2.entryOpa() */
            for (u32 i = 0; i < 4; i++) {
                gabi::store<u8>(LK_FIELD(u32, 0x60C + i * 4) + 4, 0);
                gabi::store<u8>(LK_FIELD(u32, 0x62C + i * 4) + 4, 0);
                gabi::store<u8>(LK_FIELD(u32, 0x61C + i * 4) + 4, 1);
            }
            gabi::call(0x027F583C /* J3DJoint::entryIn (HD: model, joint) */, (u32)LK_FIELD(u32, 0x448), eye);
            gabi::call(0x027F583C, (u32)LK_FIELD(u32, 0x448), mayu);
            lk_staticPacketEntry(0x1046CEB8, 0x1046D08C, 0x10058D10, 0x101CEE70); /* l_offCupOnAupPacket2.entryOpa() */
            for (u32 i = 0; i < 4; i++) {
                gabi::store<u8>(LK_FIELD(u32, 0x60C + i * 4) + 4, 1);
                gabi::store<u8>(LK_FIELD(u32, 0x61C + i * 4) + 4, 0);
            }
            gabi::call(0x027F583C, (u32)LK_FIELD(u32, 0x448), eye);
            gabi::call(0x027F583C, (u32)LK_FIELD(u32, 0x448), mayu);
            /* HD: the face (0x1046CCB4) and hair (0x1046CCCC) materials stay visible */
            for (u32 mtl = gabi::load<u32>(rootNode + 0x10); mtl != 0; mtl = gabi::load<u32>(mtl + 4)) {
                if (!lk_nameIsGlobal(mtl, 0x1046CCB4, gabi::ea(s1.get())) && !lk_nameIsGlobal(mtl, 0x1046CCCC, gabi::ea(s2.get()))) {
                    gabi::store<u8>(gabi::load<u32>(mtl + 8) + 4, 0);
                }
            }
            gabi::call(0x027F583C, (u32)LK_FIELD(u32, 0x448), rootNode); /* link_root_joint->entryIn() */
            if (checkMaskDraw()) {
                entryDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x450)), noResetFlg1() & 0x800);
            }
            gabi::store<u32>(LK_J3DSYS + 0x6C, LK_FIELD(u32, 0x448));
            hideHatAndBackle(gabi::load<u32>(rootNode + 0x10));
            lk_setList(0x5D5C, 0x5D5C);
            lk_staticPacketEntry(0x1046CF50, 0x1046D090, 0x10058D40, 0x101CEE7C); /* l_onCupOffAupPacket1.entryOpa() */
            for (u32 i = 0; i < 4; i++) {
                gabi::store<u8>(LK_FIELD(u32, 0x60C + i * 4) + 4, 0);
                gabi::store<u8>(LK_FIELD(u32, 0x62C + i * 4) + 4, 1);
                gabi::store<u8>(LK_FIELD(u32, 0x61C + i * 4) + 4, 0);
            }
            gabi::call(0x027F583C, (u32)LK_FIELD(u32, 0x448), eye);
            gabi::call(0x027F583C, (u32)LK_FIELD(u32, 0x448), mayu);
            lk_staticPacketEntry(0x1046CFE8, 0x1046D094, 0x10058D10, 0x101CEE88); /* l_offCupOnAupPacket1.entryOpa() */
            for (u32 i = 0; i < 4; i++) {
                gabi::store<u8>(LK_FIELD(u32, 0x62C + i * 4) + 4, 0);
            }
            u32 clm = LK_FIELD(u32, 0x448);
            LK_FIELD(u8, 0x4C94) = 1; /* HD packets */
            LK_FIELD(u32, 0x5360) = clm;
            LK_FIELD(u8, 0x5374) = 1;
            LK_FIELD(u32, 0x4C80) = clm;
        } else {
            hideHatAndBackle(gabi::load<u32>(rootNode + 0x10));
        }
        bool hide;
        if (gabi::load<u8>(dComIfGs_base_l() + 0x2E) == 0x38 || gabi::load<u8>(dComIfGp_ea() + 0x5CEA) == 2) { /* checkNormalSwordEquip() */
            hide = false;
        } else {
            u32 sd = dComIfGp_ea() + 0x5150;
            u32 info = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(sd) + 0x15C), sd);
            hide = ((gabi::load<u32>(info + 0xC) >> 16) & 7) != 5 /* dStageType_FF1_e */;
        }
        if (!hide) {
            hide = checkCaughtShapeHide() || checkDemoShieldNoDraw();
        }
        if (hide && gabi::load<u16>(self + 0x420) != 0) {
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
        lk_shapeShow(lk_jointNode(gabi::load<u32>(self + 0x444), 0xD /* CL_JNT_CL_PODA_e */), hide ? 0 : 1);
    }
    lk_setList(0x5D58, 0x5D60); /* dComIfGd_setListP1() */
    {
        u32 dl = dComIfGp_ea() + 0x5D30;
        gabi::store<u32>(dl + 0, gabi::load<u32>(self + 0x314));
        gabi::store<u32>(dl + 4, gabi::load<u32>(self + 0x318));
        gabi::store<u32>(dl + 8, gabi::load<u32>(self + 0x31C));
        gabi::call(0x0252F4E0, dl); /* HD: the draw list gets Link's position */
    }
    if (noResetFlg1() & 0x800) {
        gabi::call(0x0259130C /* dMat_control_c::iceEntryDL */, (u32)LK_FIELD(u32, 0x448), -1, 0);
    } else {
        mDoExt_modelEntryDL(mpCLModel);
    }
    for (u32 mtl = gabi::load<u32>(rootNode + 0x10); mtl != 0; mtl = gabi::load<u32>(mtl + 4)) {
        gabi::store<u8>(gabi::load<u32>(mtl + 8) + 4, 1);
    }
    lk_shapeShow(lk_jointNode(gabi::load<u32>(self + 0x444), 0x14 /* CL_JNT_CL_HANA_e */), 1);
    lk_shapeShow(lk_jointNode(gabi::load<u32>(self + 0x444), 0x29 /* CL_JNT_CL_BACK_e */), 1);
    if (!r24) {
        entryDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0xCF4)), noResetFlg1() & 0x800);
        entryHDPacket(self + 0x4D48, LK_FIELD(u32, 0xCF4));
        u32 f = noResetFlg1();
        if ((f & 8) && !checkCaughtShapeHide()) {
            if (!(lk_camAttnStatus(this, self + 0x69BC) & 0x20)) {
                entryDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x44C)), noResetFlg1() & 0x800);
                entryHDPacket(self + 0x4C98, LK_FIELD(u32, 0x44C));
            }
            f = noResetFlg1();
        }
        if ((f & 0x800) && checkMaskDraw()) {
            entryDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x450)), noResetFlg1() & 0x800);
        }
        if (gabi::load<u8>(dComIfGs_base_l() + 0x30) == 0x28 /* checkPowerGloveEquip() */) {
            entryDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x4434)), noResetFlg1() & 0x800);
            entryHDPacket(self + 0x5168, LK_FIELD(u32, 0x4434));
        }
        u8 sw = gabi::load<u8>(dComIfGs_base_l() + 0x2E);
        if ((sw == 0x39 || sw == 0x3A || sw == 0x3E) && !checkCaughtShapeHide() && !checkDemoShieldNoDraw()) {
            bool draw = true;
            if (gabi::load<u16>(self + 0x420) != 0) {
                gabi::Local<SafeString> k1;
                gabi::Local<SafeString> k2;
                k1->__vtbl = 0x10034B24;
                k2->__vtbl = 0x10034B24;
                k2->mStringTop = 0x1047E6B8;
                k1->mStringTop = 0x100353CC; /* "Demo50" */
                draw = !lk_keyEq(gabi::ea(k1.get()), gabi::ea(k2.get()));
            }
            if (draw) {
                updateDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0xE7C)), noResetFlg1() & 0x800); /* mpPodmsModel */
                entryHDPacket(self + 0x4EA8, LK_FIELD(u32, 0xE7C));
            }
        }
    }
    if (mNoResetFlg0 & 0x2000000 /* checkEquipHeavyBoots() */) {
        entryDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x442C)), noResetFlg1() & 0x800);
        entryDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x4430)), noResetFlg1() & 0x800);
        entryHDPacket(self + 0x5008, LK_FIELD(u32, 0x442C));
        entryHDPacket(self + 0x50B8, LK_FIELD(u32, 0x4430));
    }
    gabi::store<f32>(tev + 0xA8, fogStart);
    gabi::store<s16>(tev + 0xA0, fogR);
    gabi::store<f32>(tev + 0xAC, fogEnd);
    gabi::store<s16>(tev + 0xA2, fogG);
    gabi::store<s16>(tev + 0xA4, fogB);
    if (!r24) {
        s32 proc = mCurProc;
        if (proc == 0x42 /* daPyProc_CUT_F_e */ || proc == 0x63 /* daPyProc_BT_VERTICAL_JUMP_CUT_e */) {
            updateDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x497C)) /* mpSwordTipStabModel */, 0);
        } else if ((mModeFlg & 0x40000) && (mNoResetFlg0 & 0x100) && !(proc == 0xB2 /* daPyProc_DEMO_DEAD_e */ && mProcVar3 == 0)) {
            gabi::Local<u8[4]> spc;
            gabi::Local<u8[4]> sp8;
            gabi::call(0x025602F0 /* dKy_get_seacolor */, spc.get(), sp8.get());
            gabi::Local<u8[8]> sp10; /* J3DGXColorS10 */
            u32 c10 = gabi::ea(sp10.get());
            gabi::store<s16>(c10 + 4, gabi::load<u8>(gabi::ea(spc.get()) + 2));
            gabi::store<s16>(c10 + 2, gabi::load<u8>(gabi::ea(spc.get()) + 1));
            gabi::store<s16>(c10 + 0, gabi::load<u8>(gabi::ea(spc.get()) + 0));
            u32 x = J3DModelData_HD_027F3F8C_l(gabi::load<u32>(LK_FIELD(u32, 0x5378) + 0xAC)); /* mpSuimenMunyaModel */
            u16 num = gabi::load<u16>(x + 0x24);
            gabi::Local<u8[0x10]> f4;
            gabi::Local<u8[0xC]> rgb;
            u32 matID = num - 1;
            for (u32 n = num; n != 0; n--, matID--) {
                u32 md = gabi::load<u32>(LK_FIELD(u32, 0x5378) + 0xAC);
                u32 mat = gabi::load<u32>(md + 0x10);
                if ((u16)matID < gabi::load<u32>(md + 0xC)) {
                    mat += (u16)matID * 0x39C;
                }
                u32 tevb = gabi::load<u32>(mat + 0x18);
                gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(tevb + 4) + 0x24), tevb, 0, c10); /* setTevColor(0, &sp10) */
                /* HD: the colour also goes to the material's HD colour block (index 4) */
                u32 fo = gabi::ea(f4.get());
                f32 r = (f32)gabi::load<s16>(c10 + 0) / 255.0f;
                f32 g = (f32)gabi::load<s16>(c10 + 2) / 255.0f;
                f32 b = (f32)gabi::load<s16>(c10 + 4) / 255.0f;
                f32 a = (f32)gabi::load<s16>(c10 + 6) / 255.0f;
                gabi::store<f32>(fo + 0, r);
                gabi::store<f32>(fo + 4, g);
                gabi::store<f32>(fo + 8, b);
                gabi::store<f32>(fo + 0xC, a);
                gabi::call(0x0274D458, rgb.get(), f4.get(), 1.0f);
                gabi::store<u32>(mat + 0xA0, gabi::load<u32>(mat + 0xA0) | 0x10);
                u32 dst = gabi::call<u32>(0x027F9F0C, mat + 0xA0, 4);
                f32 a2 = (f32)gabi::load<s16>(c10 + 6) / 255.0f;
                gabi::store<f32>(dst + 0, gabi::load<f32>(gabi::ea(rgb.get()) + 0));
                gabi::store<f32>(dst + 4, gabi::load<f32>(gabi::ea(rgb.get()) + 4));
                gabi::store<f32>(dst + 8, gabi::load<f32>(gabi::ea(rgb.get()) + 8));
                gabi::store<f32>(dst + 0xC, a2);
            }
            mDoExt_modelUpdateDL(gabi::at<J3DModel>(LK_FIELD(u32, 0x5378)), 0);
        }
        if (!(lk_camAttnStatus(this, self + 0x69BC) & 0x20)) {
            if ((gabi::load<u8>(dComIfGs_base_l() + 0x2E) != 0xFF || gabi::load<u8>(dComIfGp_ea() + 0x5CEA) == 2) && !checkDemoSwordNoDraw(1)) {
                entryDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0xCF8)), noResetFlg1() & 0x800); /* mpEquippedSwordModel */
                entryHDPacket(self + 0x4DF8, LK_FIELD(u32, 0xCF8));
            }
        }
        if (gabi::load<u8>(dComIfGs_base_l() + 0x2F) != 0xFF && !checkCaughtShapeHide() && !checkDemoShieldNoDraw()) {
            entryDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0xE80)), noResetFlg1() & 0x800); /* mpEquippedShieldModel */
            entryHDPacket(self + 0x4F58, LK_FIELD(u32, 0xE80));
        }
        lk_setList(0x5D78, 0x5D7C); /* dComIfGd_setList() */
        drawMirrorLightModel();
        lk_setList(0x5D58, 0x5D60);
        BOOL binho = FALSE;
        u32 bc = LK_FIELD(u32, 0x4970);
        if (bc != 0) {
            gabi::Local<SafeString> n1;
            gabi::Local<SafeString> k1;
            gabi::store<u32>(gabi::ea(n1.get()) + 4, 0x10034B24);
            gabi::store<u32>(gabi::ea(n1.get()) + 0, lk_relPtr(gabi::load<u32>(bc + 0x14) + 4));
            k1->__vtbl = 0x10034B24;
            k1->mStringTop = 0x100353C4; /* "binho": the firefly is drawn after the bottle cap */
            if (lk_keyEq(gabi::ea(n1.get()), gabi::ea(k1.get()))) {
                binho = TRUE;
            } else {
                updateDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x4970)), 0);
            }
        }
        if (LK_FIELD(u32, 0x4440) != 0 && !checkCaughtShapeHide() && !checkDemoSwordNoDraw(0)) {
            u16 item = mEquipItem;
            bool skip = false;
            if (checkBowItem(item)) {
                skip = gabi::call_ptr<BOOL>(gabi::load<u32>(gabi::load<u32>(self + 0xB4) + 0x3C), this) != 0; /* checkPlayerGuard() */
                item = mEquipItem;
            }
            if (!skip) {
                if (item == 0x2F) {
                    lk_setHookshotTipMtx(self);
                }
                entryDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x4440)), noResetFlg1() & 0x800);
                entryHDPacket(self + 0x5218, LK_FIELD(u32, 0x4440));
                if (LK_FIELD(u32, 0x4978) != 0 &&
                    (checkChanceMode() || (noResetFlg1() & 0x8000) /* SOUP_POWER_UP */ || gabi::load<u8>(dComIfGs_base_l() + 0x2E) == 0x3E /* checkFinalMasterSwordEquip() */)) {
                    updateDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x4978)), 0); /* mpSwordModel1 */
                }
            }
        }
        if (LK_FIELD(u32, 0x4974) != 0 && m355E != 0) {
            updateDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x4974)), 0); /* mpBottleCapModel */
        }
        if (LK_FIELD(u32, 0x4970) != 0 && binho) {
            updateDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x4970)), 0);
        }
        if (!(lk_camAttnStatus(this, self + 0x69BC) & 0x20)) {
            u32 a = LK_FIELD(u32, 0x5624); /* mYaura00rBrk's animation */
            if (gabi::load<f32>(a) > 0.0f) {
                gabi::call(0x025E83FC /* mDoExt_brkAnm::entry */, self + 0x5614, gabi::load<u32>(LK_FIELD(u32, 0x55E4) + 0xAC), gabi::load<f32>(a));
                for (int i = 0; i < 6; i++) { /* mMagicArmorAuraEntries: HD sets the btk frame only */
                    lk_anmObjSetFrame(self + 0x568C, 0x68, 0x10, gabi::load<f32>(self + 0x55E4 + i * 8 + 4));
                }
            }
            a = LK_FIELD(u32, 0x5714); /* mYmgcs00Brk */
            if (gabi::load<f32>(a) > 0.0f) {
                gabi::call(0x025E83FC, self + 0x5704, gabi::load<u32>(LK_FIELD(u32, 0x5700) + 0xAC), gabi::load<f32>(a));
                mDoExt_modelEntryDL(gabi::at<J3DModel>(LK_FIELD(u32, 0x5700)));
            }
        }
    }
    if (checkHDAnmNotEnd()) { /* HD: fanWindEffectDraw() */
        updateDLSetLight(gabi::at<J3DModel>(LK_FIELD(u32, 0x53F0)), 0);
    }
    lk_setList(0x5D78, 0x5D7C); /* dComIfGd_setList() */
    {
        u32 blk = gabi::load<u32>(LK_FIELD(u32, 0x448) + 0x2C);
        gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
    }
    if (mCurProc == 0x93 /* daPyProc_FAN_GLIDE_e */) { /* HD: the shadow model only while gliding */
        drawShadow();
    }
    u32 blur = mpSwBlur;
    if (gabi::load<s32>(blur + 0x9C) > 0) {
        u32 play = dComIfGp_ea();
        gabi::call(0x0252F3B0 /* dDlst_list_c::entryZSortXluDrawList */, play + 0x5D30, gabi::load<u32>(play + 0x5D7C), blur, blur + 0x38C);
    }
#undef LK_freeze
    return TRUE;
}
VERIFY(0x023D9820, &daPy_lk_c::draw);
