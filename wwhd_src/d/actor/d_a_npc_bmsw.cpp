/**
 * d_a_npc_bmsw.cpp (WWHD)
 * NPC - Koboli (Rito mail sorter)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_bmsw.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_bmsw.h"
#include "gc_api/d/d_com_inf_game.h"

// HD archive IDs correspond to the BmswResourceId names in the unit header.
static const s32 l_btp_ix_tbl[] = {34, 34};
static const s32 l_bck_ix_tbl[] = {27, 32, 22, 23, 31, 20, 21};
static const s32 l_arm_bck_ix_tbl[] = {26, 30, 0, 19, 29, 17, 18};
static const s32 a_play_mode_tbl[] = {2, 2, 2, 2, 2, 2, 2};
static const f32 a_morf_frame_tbl[] = {8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f};
static const f32 a_play_speed_tbl[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static auto& l_messageManager = gabi::object_at<be<u32>>(0x101F4B5C);
static auto& l_currentModel = gabi::object_at<be<u32>>(0x104B462C);
static auto& l_attentionOffsetGuard = gabi::object_at<be<u32>>(0x104667F0);
static auto& l_eyeOffsetGuard = gabi::object_at<be<u32>>(0x104667F4);
static auto& l_effectOffsetGuard = gabi::object_at<be<u32>>(0x104667F8);
static auto& l_debugColorOneGuard = gabi::object_at<be<u32>>(0x101FDA50);
static auto& l_debugColorTwoGuard = gabi::object_at<be<u32>>(0x101FDAC0);
static auto& l_miniGameSystem = gabi::object_at<be<u32>>(0x101F8344);
static auto& l_pad = gabi::object_at<be<u32>>(0x101F5088);
static auto& l_aimPositionGuard = gabi::object_at<be<u32>>(0x104667FC);
static auto& l_cursorPositionGuard = gabi::object_at<be<u32>>(0x10466800);
static auto& l_piLow = gabi::object_at<be<f32>>(0x104666B8);
static auto& l_piHigh = gabi::object_at<be<f32>>(0x104666BC);
static auto& l_largeFloatFour = gabi::object_at<be<f32>>(0x104666CC);
static auto& l_largeFloatOne = gabi::object_at<be<f32>>(0x104666C0);
static auto& l_largeFloatTwo = gabi::object_at<be<f32>>(0x104666C4);
static auto& l_largeFloatThree = gabi::object_at<be<f32>>(0x104666C8);
static auto& l_miniGameActive = gabi::object_at<be<u8>>(0x1047B09A);

const gc_tu_data gc_tu = {
    .safeString = 0x10018004,
    .safeStringAssure = 0x02212004,
    .objAcchDt = {0x1001801C, 0x1001802C},
};
constexpr u32 l_actorVtable = 0x10018390;

// Statics shared by the actor and its mail/camera helpers.
static auto& l_attentionOffset = gabi::object_at<cXyz>(0x104666E4);
static auto& l_eyeOffset = gabi::object_at<cXyz>(0x104666F0);
static auto& l_effectOffset = gabi::object_at<cXyz>(0x104666FC);
static auto& l_zeroWords = gabi::object_at<be<u32>[4]>(0x104666D4);
static auto& l_cyl_src = gabi::object_at<dCcD_SrcCyl>(0x101BD3F4);
static auto& camera_center_data = gabi::object_at<BmswPositionTable>(0x10466804);
static auto& camera_eye = gabi::object_at<cXyz>(0x1046684C);
static auto& aim_pos_data = gabi::object_at<BmswPositionTable>(0x10466760);
static auto& cursor_pos_data = gabi::object_at<BmswPositionTable>(0x104667A8);
static auto& l_lightDefault = gabi::object_at<BmswLightDefaultBlock>(0x1016E414);
/* daNpc_Bmsw_HIO_c (HD: vtable last, at +0x54; GameCube members -4) */

static auto& l_HIO = gabi::object_at<daNpc_Bmsw_HIO_c>(0x10466708);
/* SwMail_c::m_same_count / m_no_buff, SwCam_c::camera_center_data[2][3] / camera_eye */
be<u8>& SwMail_c::m_same_count = gabi::object_at<be<u8>>(0x101BD4B1);
be<u8>& SwMail_c::m_no_buff = gabi::object_at<be<u8>>(0x101BD4B0);

/* SwMail_c / daNpc_Bmsw_c member functions (GHS pointers to member: {0, -1, function}) */
enum : u32 {
    FN_SwMail_Dummy = 0x0220F3C4,
    FN_SwMail_Appear = 0x0220F4C0,
    FN_SwMail_Wait = 0x0220F62C,
    FN_SwMail_Throw = 0x0220F7C4,
    FN_SwMail_End = 0x0220FA84,
    FN_wait_action = 0x02210AC0,
    FN_shiwake_game_action = 0x02210F60,
};

// Unit-private ABI and HD inline helpers.
// HD: Resources use a per-TU SafeString key; its vtable is stored before the text pointer.
static inline void* Bmsw_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->__vtbl = gc_tu.safeString;
    key->mStringTop = gabi::guest_addr(arc);
    return Bmsw_resourceGetID(dComIfG_resControl(), key.get(), id);
}
/* J3DAnmTexPattern::getFrameMax: virtual (vtable pointer at +4, slot +0x14) */
static inline s32 J3DAnm_getFrameMax(void* p) {
    u32 vt = gabi::object_at<BmswAnimation>(gabi::ea(p)).mVtable.get();
    return gabi::call_ptr<s32>(gabi::object_at<BmswFrameVtable>(vt).mGetFrameMax.get(), p);
}
/* cLib_calcTimer<s16> (out-of-line copy of another TU) */
static inline s16 cLib_calcTimer(be<s16>* t) { return Bmsw_calcTimer(t); }
/* mDoExt_btpAnm::init(modelData, anm, anmPlay, attr, rate, start, end, modify, entry) */
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr,
                                     f32 rate, s32 start, s32 end, u32 modify, s32 entry) {
    return Bmsw_btpInit(anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline J3DModelData* Bmsw_modelData(J3DModel* m) { return m->getModelData(); }
/* HD: mDoAud_seStart(id) without a position */
static inline void mDoAud_seStart_id(u32 id) { Bmsw_seStart(id); }
// HD: Dispatch reads the GHS index before the this delta; retain this load order.
static inline void pmf_call(void* self, BmswActionFunc* pmf, void* arg) {
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        gabi::call_ptr<BOOL>(pmf->f, thisp, arg);
    } else {
        s16 voff = Bmsw_virtualOffset(*pmf);
        u32 vt = gabi::object_at<be<u32>>(thisp + voff).get();
        gabi::call_ptr<BOOL>(gabi::object_at<BmswVirtualFunction>(vt + i * 8).mFunction.get(),
                             thisp, arg);
    }
}
/* (this->*pmf)() without an argument */
static inline void pmf_call0(void* self, BmswMailFunc* pmf) {
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        gabi::call_ptr(pmf->f, thisp);
    } else {
        s16 voff = Bmsw_virtualOffset(*pmf);
        u32 vt = gabi::object_at<be<u32>>(thisp + voff).get();
        gabi::call_ptr(gabi::object_at<BmswVirtualFunction>(vt + i * 8).mFunction.get(), thisp);
    }
}
/* pmf == {0, -1, func} (GHS: equal indices, then zero index or equal delta and function) */
static inline bool pmf_is(BmswMailFunc* pmf, u32 func) {
    s16 i = pmf->i;
    if (i != -1)
        return false;
    return pmf->d == 0 && pmf->f == func;
}
static inline void pmf_set(BmswMailFunc* pmf, u32 func) {
    pmf->i = -1;
    pmf->f = func;
    pmf->d = 0;
}

/* c_lib */
static inline s16 cLib_targetAngleX(const cXyz* a, const cXyz* b) {
    return Bmsw_targetAngleX(a, b);
}
static inline f32 cLib_addCalcPos(cXyz* pos, const cXyz* target, f32 scale, f32 maxStep,
                                  f32 minStep) {
    return Bmsw_addCalcPos(pos, target, scale, maxStep, minStep);
}
static inline void cLib_addCalcPos2(cXyz* pos, const cXyz* target, f32 scale, f32 maxStep) {
    Bmsw_addCalcPos2(pos, target, scale, maxStep);
}
static inline bool cXyz_normalizeRS(cXyz* v) { return Bmsw_normalizeRS(v); }
static inline void mDoMtx_ZXYrotS(Mtx34* m, s16 x, s16 y, s16 z) { Bmsw_ZXYrotS(m, x, y, z); }
/* HD: mDoAud_seStart(id, pos) */
static inline void mDoAud_seStart_pos(u32 id, cXyz* pos) { Bmsw_seStartPosition(id, pos); }
// HD: Saturate at zero, rereading the cached rupee field on the positive branch.
static inline void Bmsw_addMiniGameRupee() {
    auto& rupees = gabi::object_at<dComIfG_play_c>(dComIfGp_ea() + 0x12A0).mMiniGameRupee;
    if ((s32)rupees.get() + 1 > 0)
        rupees = (s16)(rupees.get() + 1);
    else
        rupees = 0;
}
/* mDoExt_btpAnm::remove(modelData) (inline): the model data's texture pattern (+0x38) is cleared */
static inline void mDoExt_btpAnm_remove(J3DModelData* d) {
    gabi::object_at<BmswModelAnimation>(gabi::ea(d)).mpTexPattern = 0;
}

/* dNpc_playerEyePos(f32): cXyz through a hidden result pointer (r3) */
static inline void Bmsw_getPlayerEyePos(cXyz* out, f32 offs) { Bmsw_playerEyePos(out, offs); }
/* dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (copy), s16 yrot, s16 vel, bool headOnly) */
static inline void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target,
                                             cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    Bmsw_jointLookAtTarget(j, outY, target, eye, yrot, vel, headOnly);
}
// HD: Compare two SafeStrings after direct and virtual termination calls.
static inline bool startStageIs(u32 name) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> b;
    a->__vtbl = gc_tu.safeString;
    b->__vtbl = gc_tu.safeString;
    a->mStringTop = name;
    b->mStringTop = 0x1047E6B8;
    Bmsw_assureTermination(a.get());
    gabi::call_ptr(gabi::object_at<BmswVirtualTable>(a->__vtbl).mAssureTermination.get(), a.get());
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::object_at<BmswVirtualTable>(b->__vtbl).mAssureTermination.get(), b.get());
    u32 s2 = b->mStringTop;
    if (s1 == s2)
        return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c1 = gabi::object_at<be<u8>>(s1 + i).get();
        u8 c2 = gabi::object_at<be<u8>>(s2 + i).get();
        if (c1 != c2)
            return false;
        if (c1 == 0)
            return true;
    }
    return false;
}

/* STControl (d_lib) */
static inline void STControl_setWaitParm(BmswSTControl* c, s16 a, s16 b, s16 d, s16 e, f32 f, f32 g,
                                         s16 h, s16 i) {
    Bmsw_stickSetWaitParm(c, a, b, d, e, f, g, h, i);
}
static inline void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z) { Bmsw_XYZrotM(m, x, y, z); }
/* fopAcM_createChild(name, parentPcId, param, pos, roomNo, angle, scale, createFunc) (HD: by name, no subtype) */
static inline u32 fopAcM_createChild(const char* name, u32 parent, u32 param, cXyz* pos, s32 roomNo,
                                     csXyz* angle, cXyz* scale, u32 cb) {
    return Bmsw_createChildByName(name, parent, param, pos, roomNo, angle, scale, cb);
}
/* fopAcM_create(name, param, pos, roomNo, angle, scale, createFunc) (HD: by name, no subtype) */
static inline u32 fopAcM_create_name(const char* name, u32 param, cXyz* pos, s32 roomNo,
                                     csXyz* angle, cXyz* scale, u32 cb) {
    return Bmsw_createActorByName(name, param, pos, roomNo, angle, scale, cb);
}
static inline void dKy_tevstr_init(dKy_tevstr_c* t, s8 roomNo, u8 p) {
    Bmsw_tevstrInit(t, roomNo, p);
}
/* dSnap_RegistFig(type, actor, const cXyz& pos, s16 angleY, f32, f32, f32) */
static inline void Bmsw_registFig(u8 type, fopAc_ac_c* a, cXyz* pos, s16 angleY, f32 x, f32 y,
                                  f32 z) {
    Bmsw_registFigure(type, a, pos, angleY, x, y, z);
}

// HD: Joint matrices live in a buffer that is marked dirty before reading its array.
static inline Mtx34* getAnmMtx(J3DModel* model, s32 jntNo) { return model->getAnmMtx(jntNo); }
static inline Mtx34* j3dSys_mCurrentMtx() { return J3DSys::mCurrentMtx; }

// The free registration preserves the static method ABI; no hidden actor argument.
u8 SwMail_c::getNextNo(u8 previous_no) { return Bmsw_getNextNo(previous_no); }

static cXyz* SwCam_camera_center_data(int row, int col) {
    return &camera_center_data.positions[row][col];
}
static cXyz* SwCam_camera_eye() { return &camera_eye; }

/* daNpc_Bmsw_c::setAction(func, NULL) (inline): func is a non-virtual member {0, -1, func} */
static inline void Bmsw_setAction(daNpc_Bmsw_c* t, u32 func) {
    BmswActionFunc* cur = &t->mCurrActionFunc;
    s16 i = cur->i;
    if (i == -1) {
        if (cur->d == 0 && cur->f == func)
            return;
    }
    if (i != 0) {
        t->mActionStatus = daNpc_Bmsw_c::ACTION_ENDING;
        pmf_call(t, cur, nullptr);
    }
    t->mActionStatus = 0; /* ACTION_STARTING */
    cur->d = 0;
    cur->i = -1;
    cur->f = func;
    pmf_call(t, cur, nullptr);
}

/* J3DModelData (HD): 027F68FC returns the joint name table header (self-relative offset at +0x10
 * to the JUTNameTab), 027F3F94 (the matcher calls it __nw) the joint tree header (count u16 at +8) */
static inline s8 jointIndex(J3DModelData* d, const char* name) {
    u32 h = Bmsw_jointNameHeader(d);
    u32 off = gabi::object_at<BmswNameHeader>(h).mNameOffset.get();
    u32 tab = off != 0 ? h + 0x10 + off : 0;
    return Bmsw_jointNameIndex(tab, name);
}
static inline u16 J3DModelData_getJointNum(J3DModelData* d) {
    return gabi::object_at<BmswJointTree>(Bmsw_jointTreeHeader(d)).mCount.get();
}
/* getJointNodePointer(i)->setCallBack(cb) (HD: nodes of 0x1C bytes from +8; index checked against +4) */
static inline void setJointCallBack(J3DModelData* d, u32 i, u32 cb) {
    u32 n = d->mJointNum.get();
    u32 p = d->mJointNodes.get();
    if (i < n)
        p += i * 0x1C;
    gabi::object_at<J3DJoint>(p).mCallBack = cb;
}

/* mini game 7 (HD): play+0x5CE8 flags (u16, bit 0x40), +0x5CEA type (u8), +0x5CEE (u8); HD also
 * sets the byte at 0x1047B09A and notifies an object at *(*(0x101F8344) + 0x178) */
static inline void Bmsw_beginSortingGame() {
    u32 p = dComIfGp_ea();
    gabi::object_at<dComIfG_play_c>(p + 0x12A0).mMiniGameType = 7;
    gabi::object_at<dComIfG_play_c>(p + 0x12A0).mMinigameFlags =
        (u16)(gabi::object_at<dComIfG_play_c>(p + 0x12A0).mMinigameFlags.get() | 0x40);
    l_miniGameActive = 1;
    Bmsw_startMiniGame(gabi::object_at<BmswMiniGameSystem>(l_miniGameSystem.get()).mpGame.get());
}
static inline void Bmsw_finishSortingGame() {
    u32 p = dComIfGp_ea();
    u16 f = gabi::object_at<dComIfG_play_c>(p + 0x12A0).mMinigameFlags.get();
    gabi::object_at<dComIfG_play_c>(p + 0x12A0).mMiniGameType = 0;
    gabi::object_at<dComIfG_play_c>(p + 0x12A0).mMinigameResult = 0;
    gabi::object_at<dComIfG_play_c>(p + 0x12A0).mMinigameFlags = (u16)(f ^ 0x40);
    l_miniGameActive = 0;
    Bmsw_endMiniGame(gabi::object_at<BmswMiniGameSystem>(l_miniGameSystem.get()).mpGame.get());
}
/* Bmsw_timerAddress(): play+0x5CF0; the timer mode play+0x5CFC */
static inline u32 Bmsw_timerAddress() { return gabi::ea(dComIfG_getTimerPtr()); }
/* HD input (replaces the GameCube STControl): the controller object at *(0x101F5088); triggers
 * +0x18 | +0x20, while +0x124 has a bit of 0x00F00000 set only those bits count */
static inline u32 Bmsw_padAddress() { return l_pad.get(); }

/* daNpc_Bmsw_getGameEndMsg (inline in shiwake_game_action) */
static inline u32 daNpc_Bmsw_getGameEndMsg(s16 rupees) {
    u32 msgNo;
    switch (dComIfGs_getEventReg(0xC203)) {
    case 0:
        if (rupees == 0)
            msgNo = 0x1A3A;
        else if (rupees == 1)
            msgNo = 0x1A3B;
        else if (rupees < 10)
            msgNo = 0x1A3C;
        else
            msgNo = 0x1A37;
        break;
    case 1:
        if (rupees == 0)
            msgNo = 0x1A4F;
        else if (rupees == 1)
            msgNo = 0x1A50;
        else if (rupees < 20)
            msgNo = 0x1A52;
        else
            msgNo = 0x1A4C;
        break;
    case 2:
        if (rupees == 0)
            msgNo = 0x1A5C;
        else if (rupees == 1)
            msgNo = 0x1A5D;
        else if (rupees < 25)
            msgNo = 0x1A60;
        else
            msgNo = 0x1A57;
        break;
    case 3:
    default:
        if (dComIfGs_isEventBit(0x2701)) {
            if (rupees == 0) {
                msgNo = 0x1A69;
            } else if (rupees == 1) {
                msgNo = 0x1A6A;
            } else if (rupees <= (s32)dComIfGs_getEventReg(0x8AFF)) {
                msgNo = 0x1A6B;
            } else {
                dComIfGs_setEventReg(0x8AFF, (u8)rupees);
                msgNo = 0x1A6D;
            }
        } else {
            msgNo = 0x1A62;
        }
        break;
    }
    return msgNo;
}

/* 02211C80 */
static daNpc_Bmsw_HIO_c* daNpc_Bmsw_HIO_c_ct(daNpc_Bmsw_HIO_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Bmsw_HIO_c*)operator_new(0x58);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001803C;
    Bmsw_npcHioConstruct(gabi::ea(&i_this->mNpc)); /* dNpc_HIO_c::dNpc_HIO_c */
    i_this->mNpc.m04 = -20.0f;
    i_this->mNpc.mMaxHeadX = 0x1FFE;
    i_this->mNpc.mMaxHeadY = 3000;
    i_this->mNpc.mMaxBackboneX = 0;
    i_this->mNpc.mMaxBackboneY = 0x1C70;
    i_this->mNpc.mMinHeadX = -0x9C4;
    i_this->mNpc.mMinHeadY = -3000;
    i_this->mNpc.mMinBackboneX = 0;
    i_this->mNpc.mMinBackboneY = -0x1C70;
    i_this->mNpc.mMaxTurnStep = 0x1000;
    i_this->mNpc.mMaxHeadTurnVel = 0x800;
    i_this->mNpc.mAttnYOffset = 50.0f;
    i_this->mNpc.mMaxAttnAngleY = 0x2000;
    i_this->mNpc.m22 = 0;
    i_this->mNpc.mMaxAttnDistXZ = 300.0f;
    i_this->field_0x30 = 0x1E;
    i_this->field_0x32 = 0xF;
    i_this->field_0x34 = 0x3C;
    i_this->r_1 = 192;
    i_this->g_1 = 174;
    i_this->b_1 = 192;
    i_this->r_2 = 192;
    i_this->g_2 = 174;
    i_this->b_2 = 192;
    i_this->field_0x44 = 40.0f;
    i_this->field_0x48 = 80.0f;
    i_this->field_0x4C = 50.0f;
    i_this->field_0x50 = 0.8f;
    i_this->field_0x54 = 0.75f;
    i_this->mNo = -1;
    return i_this;
}

/* 0220CCD4 */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {                     /* J3DNodeCBCalcTiming_In */
        J3DModel* model = J3DSys().getModel(); /* j3dSys.getModel() */
        daNpc_Bmsw_c* i_this = static_cast<daNpc_Bmsw_c*>(model->getUserArea()); /* getUserArea() */
        if (i_this != nullptr) {
            /* static cXyz a_att_pos_offst(0.0f, 0.0f, 0.0f): guard 0x104667F0, object 0x104666E4 */
            cXyz* a_att_pos_offst = &l_attentionOffset;
            if (l_attentionOffsetGuard.get() == 0) {
                a_att_pos_offst->x = 0.0f;
                a_att_pos_offst->z = 0.0f;
                l_attentionOffsetGuard = 1;
                a_att_pos_offst->y = 0.0f;
            }
            /* static cXyz a_eye_pos_offst(26.0f, 26.0f, 0.0f): guard 0x104667F4, object 0x104666F0 */
            cXyz* a_eye_pos_offst = &l_eyeOffset;
            if (l_eyeOffsetGuard.get() == 0) {
                a_eye_pos_offst->z = 0.0f;
                l_eyeOffsetGuard = 1;
                a_eye_pos_offst->y = 26.0f;
                a_eye_pos_offst->x = 26.0f;
            }
            u32 jointNo =
                gabi::object_at<BmswJointData>(gabi::ea(J3DNode_toJoint(node))).mNumber.get();
            PSMTXCopy(getAnmMtx(model, jointNo), mDoMtx_stack_c::get());
            if (jointNo == (u32)(s32)i_this->m_neck_jnt_num) {
                mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->m_jnt.mAngles[0][1]);
                mDoMtx_ZrotM(mDoMtx_stack_c::get(), (s16)-i_this->m_jnt.mAngles[0][0]);
            } else if (jointNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
                PSMTXMultVec(mDoMtx_stack_c::get(), a_att_pos_offst, &i_this->mAttPos);
                PSMTXMultVec(mDoMtx_stack_c::get(), a_eye_pos_offst, &i_this->eyePos);
            } else if (jointNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
                mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->m_jnt.mAngles[1][1]);
                mDoMtx_ZrotM(mDoMtx_stack_c::get(), (s16)-i_this->m_jnt.mAngles[1][0]);
            } else if (jointNo == (u32)(s32)i_this->m_body_ArmL) {
                PSMTXCopy(mDoMtx_stack_c::get(), &i_this->field_0x794);
            } else if (jointNo == (u32)(s32)i_this->m_body_ArmR) {
                PSMTXCopy(mDoMtx_stack_c::get(), &i_this->field_0x7C4);
            }
            PSMTXCopy(mDoMtx_stack_c::get(), j3dSys_mCurrentMtx());
            mtx_copy(getAnmMtx(model, jointNo),
                     mDoMtx_stack_c::get()); /* model->setAnmMtx(jointNo, ...) */
        }
    }
    return TRUE;
}

/* 0220D134 */
static BOOL nodeCallBackArm(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        J3DModel* model = J3DSys().getModel();
        daNpc_Bmsw_c* i_this = static_cast<daNpc_Bmsw_c*>(model->getUserArea());
        if (i_this != nullptr) {
            /* static cXyz a_eff_pos_offst(0.0f, 0.0f, 0.0f) (unused): guard 0x104667F8, object 0x104666FC */
            cXyz* a_eff_pos_offst = &l_effectOffset;
            if (l_effectOffsetGuard.get() == 0) {
                l_effectOffsetGuard = 1;
                a_eff_pos_offst->x = 0.0f;
                a_eff_pos_offst->z = 0.0f;
                a_eff_pos_offst->y = 0.0f;
            }
            u32 jointNo =
                gabi::object_at<BmswJointData>(gabi::ea(J3DNode_toJoint(node))).mNumber.get();
            PSMTXCopy(getAnmMtx(model, jointNo), mDoMtx_stack_c::get());
            if (jointNo == (u32)(s32)i_this->m_ArmL) {
                PSMTXCopy(&i_this->field_0x794, j3dSys_mCurrentMtx());
                mtx_copy(getAnmMtx(model, jointNo), &i_this->field_0x794);
            } else if (jointNo == (u32)(s32)i_this->m_ArmR) {
                PSMTXCopy(&i_this->field_0x7C4, j3dSys_mCurrentMtx());
                mtx_copy(getAnmMtx(model, jointNo), &i_this->field_0x7C4);
            }
        }
    }
    return TRUE;
}

/* 0220D320 */
BOOL daNpc_Bmsw_c::initTexPatternAnm(u32 i_modify) { /* bool, passed on unnormalised */
    J3DModelData* modelData = Bmsw_modelData(field_0x6D4);
    m_head_tex_pattern = (J3DAnmTexPattern*)Bmsw_getObjectIDRes("Bmsw", l_btp_ix_tbl[field_0x9D4]);
    if (m_head_tex_pattern.get() == nullptr) /* JUT_ASSERT(392, m_head_tex_pattern != NULL) */
        JUT_ASSERT_fail("d_a_npc_bmsw.cpp", 0x188, "m_head_tex_pattern != (0)");
    if (!mDoExt_btpAnm_init(&field_0x7F8, modelData, m_head_tex_pattern, TRUE, 2 /* EMode_LOOP */,
                            1.0f, 0, -1, i_modify, FALSE)) {
        return FALSE;
    }
    field_0x80C = 0;
    field_0x964 = 0;
    return TRUE;
}

/* 0220EC00 */
void daNpc_Bmsw_c::playTexPatternAnm() {
    if (cLib_calcTimer(&field_0x964) == 0) {
        s32 max = J3DAnm_getFrameMax(m_head_tex_pattern);
        if ((s32)field_0x80C >= max) {
            s32 frameMax = J3DAnm_getFrameMax(m_head_tex_pattern);
            field_0x80C = (u8)(field_0x80C - frameMax);
            field_0x964 = (s16)gabi::ftoi(cM_rndF(100.0f) + 30.0f);
        } else {
            field_0x80C = (u8)(field_0x80C + 1);
        }
    }
}

/* 0220FC70 */
void daNpc_Bmsw_c::setAnm(s8 idx) {
    /* a_play_mode_tbl 0x101BD438, a_morf_frame_tbl 0x101BD454, a_play_speed_tbl 0x101BD470;
     * l_bck_ix_tbl 0x1001804C, l_arm_bck_ix_tbl 0x10018068 */
    if ((u32)(s32)idx != (u32)(s32)field_0x9D5) {
        field_0x9D5 = idx;
        s32 m = field_0x9DA;
        dNpc_setAnmIDRes(mpMorf, a_play_mode_tbl[m], a_morf_frame_tbl[m], a_play_speed_tbl[m],
                         l_bck_ix_tbl[(s32)idx], -1, "Bmsw");
        m = field_0x9DA;
        s32 n = field_0x9D5;
        dNpc_setAnmIDRes(mpMorfHand, a_play_mode_tbl[m], a_morf_frame_tbl[m], a_play_speed_tbl[m],
                         l_arm_bck_ix_tbl[n], -1, "Bmsw");
    }
}

/* 0220FD3C */
u8 daNpc_Bmsw_c::chkAttention(cXyz* pos, s16 angle) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 dz = player->current.pos.z - pos->z;
    f32 dx = player->current.pos.x - pos->x;
    f32 maxAttnDistXZ = l_HIO.mNpc.mMaxAttnDistXZ;
    s32 maxAttnAngleY = l_HIO.mNpc.mMaxAttnAngleY;
    f32 distXZ = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    s16 targetAngleY = cM_atan2s(dx, dz);
    if (mHasAttention) {
        maxAttnDistXZ += 40.0f;
        maxAttnAngleY += 0x71C; /* cM_deg2s(10.0f) */
    }
    targetAngleY -= angle;
    s32 a = targetAngleY < 0 ? -targetAngleY : targetAngleY;
    return maxAttnAngleY > a && maxAttnDistXZ > distXZ;
}

/* 0220EE0C */
void daNpc_Bmsw_c::eventOrder() {
    if (field_0x9D7 == 1 || field_0x9D7 == 2) {
        eventInfo_onCondition(this, 1); /* dEvtCnd_CANTALK_e */
        if (field_0x9D7 == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}

/* 0220ECC4 */
void daNpc_Bmsw_c::checkOrder() {
    u16 cmd = eventInfo.mCommand.get(); /* eventInfo.mCommand */
    if (cmd == 2)                       /* checkCommandDemoAccrpt() */
        return;
    if (cmd != 1) /* !checkCommandTalk() */
        return;
    if (field_0x9D7 != 1 && field_0x9D7 != 2)
        return;
    field_0x9D7 = 0;
    field_0x9B9 = 1;
}

/* 0220FE68 */
u16 daNpc_Bmsw_c::next_msgStatus(be<u32>* currMsgNo) {
    u16 ret = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    // HD: Read selection from the message manager rather than the actor message.
    u32 msgMgr = l_messageManager.get();
#define SELECT_NUM gabi::object_at<BmswMessageManager>(msgMgr).mSelectNum.get()
    switch (*currMsgNo) {
    case 0x1A2D:
    case 0x1A31:
    case 0x1A32:
    case 0x1A35:
    case 0x1A37:
    case 0x1A41:
    case 0x1A42:
    case 0x1A47:
    case 0x1A4A:
    case 0x1A4C:
    case 0x1A50:
    case 0x1A55:
    case 0x1A57:
    case 0x1A59:
    case 0x1A5A:
    case 0x1A5D:
    case 0x1A67:
    case 0x1A6D:
    case 0x1A6E:
        *currMsgNo = *currMsgNo + 1;
        break;
    case 0x1A63:
    case 0x1A66:
        *currMsgNo = 0x1A64;
        break;
    case 0x1A6A:
    case 0x1A6B:
        *currMsgNo = 0x1A6C;
        break;
    case 0x1A52:
        /* dComIfGp_getMiniGameRupee(): play+0x5CEC */
        if (gabi::object_at<dComIfG_play_c>(dComIfGp_ea() + 0x12A0).mMiniGameRupee.get() < 15) {
            *currMsgNo = 0x1A51;
        } else {
            *currMsgNo = 0x1A53;
        }
        break;
    case 0x1A60:
        if (gabi::object_at<dComIfG_play_c>(dComIfGp_ea() + 0x12A0).mMiniGameRupee.get() < 20) {
            *currMsgNo = 0x1A5E;
        } else {
            *currMsgNo = 0x1A61;
        }
        break;
    case 0x1A2E:
        if (SELECT_NUM == 0) {
            dComIfGs_onEventBit(0x1A01);
            *currMsgNo = 0x1A31;
        } else if (SELECT_NUM == 1) {
            if (Bmsw_isStageBossEnemy(3) /* dComIfGs_isStageBossEnemy(dSv_save_c::STAGE_DRC) */) {
                *currMsgNo = 0x1A2F;
            } else {
                *currMsgNo = 0x1A30;
            }
        }
        break;
    case 0x1A33:
        if (SELECT_NUM == 0) {
            *currMsgNo = 0x1A35;
        } else if (SELECT_NUM == 1) {
            *currMsgNo = 0x1A34;
        }
        break;
    case 0x1A34:
        if (!dComIfGs_isEventBit(0x1A01)) {
            *currMsgNo = 0x1A32;
        } else {
            *currMsgNo = 0x1A42;
        }
        break;
    case 0x1A38:
        *currMsgNo = 0x1A39; /* (*currMsgNo)++ */
        dComIfGs_setEventReg(0xC203, 1);
        break;
    case 0x1A3D:
        if (SELECT_NUM == 0) {
            dComIfGs_onEventBit(0x1A01);
            *currMsgNo = 0x1A31;
        } else {
            *currMsgNo = 0x1A3E;
        }
        break;
    case 0x1A4D:
        dComIfGs_setEventReg(0xC203, 2);
        // fallthrough
    case 0x1A51:
    case 0x1A53:
        *currMsgNo = 0x1A4E;
        break;
    case 0x1A58:
        dComIfGs_setEventReg(0xC203, 3);
        *currMsgNo = *currMsgNo + 1;
        break;
    case 0x1A5E:
    case 0x1A61:
        *currMsgNo = 0x1A5F;
        break;
    case 0x1A3F:
        if (SELECT_NUM == 0) {
            *currMsgNo = 0x1A41;
        } else {
            *currMsgNo = 0x1A40;
        }
        break;
    case 0x1A43:
        if (SELECT_NUM == 0) {
            *currMsgNo = 0x1A35;
        } else {
            *currMsgNo = 0x1A34;
        }
        break;
    case 0x1A44:
        if (SELECT_NUM == 0) {
            *currMsgNo = 0x1A46;
        } else {
            *currMsgNo = 0x1A45;
        }
        break;
    case 0x1A46:
        if (SELECT_NUM == 0) {
            *currMsgNo = 0x1A4A;
        } else {
            *currMsgNo = 0x1A47;
        }
        break;
    case 0x1A48:
        if (SELECT_NUM == 0) {
            *currMsgNo = 0x1A4A;
        } else {
            *currMsgNo = 0x1A49;
        }
        break;
    case 0x1A49:
        *currMsgNo = 0x1A47;
        break;
    case 0x1A54:
        if (SELECT_NUM == 0) {
            *currMsgNo = 0x1A55;
        } else {
            *currMsgNo = 0x1A45;
        }
        break;
    case 0x1A64:
        if (SELECT_NUM == 0) {
            *currMsgNo = 0x1A67;
        } else {
            *currMsgNo = 0x1A65;
        }
        break;
    default:
        ret = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
#undef SELECT_NUM
    return ret;
}

/* 022102CC */
u32 daNpc_Bmsw_c::getMsg() {
    u32 msg = field_0x9C0;
    if (field_0x9C0 != fpcM_ERROR_PROCESS_ID_e) {
        field_0x9C0 = fpcM_ERROR_PROCESS_ID_e;
    } else if (dComIfGs_isEventBit(0x2701)) {
        if (!dComIfGs_isTmpBit(0x0320)) {
            dComIfGs_onTmpBit(0x0320);
            msg = 0x1A63;
        } else {
            msg = 0x1A66;
        }
    } else if (!dComIfGs_isEventBit(0x1A02)) {
        dComIfGs_onEventBit(0x1A02);
        msg = 0x1A2D;
    } else if (!dComIfGs_isEventBit(0x1A01)) {
        msg = 0x1A3D;
    } else {
        switch (dComIfGs_getEventReg(0xC203)) {
        case 0:
            msg = 0x1A3F;
            break;
        case 1:
            msg = 0x1A44;
            break;
        case 2:
            msg = 0x1A54;
            break;
        case 3:
        default:
            msg = 0x1A62;
        }
    }
    return msg;
}

/* 022103D4 */
void daNpc_Bmsw_c::anmAtr(u16 i_msgStatus) {
    /* dComIfGp_getMesgAnimeAttrInfo(): play+0x5BC5 */
    switch (dComIfGp_getMesgAnimeAttrInfo()) {
    case 5:
        setAnm(1);
        break;
    case 6:
        setAnm(2);
        break;
    case 9:
        setAnm(3);
        break;
    case 14:
        setAnm(4);
        break;
    }
    dComIfGp_setMesgAnimeAttrInfo(0xFF); /* dComIfGp_setMesgAnimeAttrInfo(0xFF) */
}

/* 0220E6E8 */
BOOL daNpc_Bmsw_c::CreateInit() {
    /* field_0x97E = current.angle */
    field_0x97E.z = current.angle.z;
    field_0x97E.y = current.angle.y;
    field_0x97E.x = current.angle.x;
    attention_info.flags =
        fopAc_Attn_LOCKON_TALK_e |
        fopAc_Attn_ACTION_SPEAK_e; /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAA;  /* attention_info.distances[TALK] */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xAA; /* attention_info.distances[SPEAK] */
    gravity = -30.0f;
    Bmsw_setAction(this, FN_wait_action);
    mAttPos.copy(current.pos);
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(&l_cyl_src /* l_cyl_src */);
    mCyl.SetStts(&mStts);
    setCollision(60.0f, 150.0f);
    field_0x9C4 = 0;
    mEventCut.setActorInfo2("Bmsw", this);
    field_0x962 = 0;
    field_0x963 = 0;
    field_0x9C0 = fpcM_ERROR_PROCESS_ID_e;
    STControl_setWaitParm(&field_0x988, 5, 2, 3, 2, 1.0f, 0.9f, 0, 0x2000);
    set_mtx();
    mpMorf->calc();
    mpMorfHand->calc();
    set_mtx(); // HD: Refresh attachments again after the animation update.

    dKy_tevstr_init(&field_0x6E0, home.roomNo, 0xFF);
    if (dComIfGs_isEventBit(0x2701)) {
        field_0x9E0 =
            fopAcM_createChild("Btsw", fopAcM_GetID(this), fopAcM_GetParam(this), &current.pos,
                               fopAcM_GetRoomNo(this), &current.angle, nullptr, 0);
        /* *fopAcM_GetOldPosition_p(this) = cXyz(120.0f, 700.0f, 810.0f); current.pos = old.pos */
        BmswFloatWord t;
        t.value = 700.0f;
        u32 y = t.word().get();
        t.value = 120.0f;
        u32 x = t.word().get();
        t.value = 810.0f;
        u32 z = t.word().get();
        gabi::object_at<BmswPositionWords>(gabi::ea(&old.pos)).y = y;
        gabi::object_at<BmswPositionWords>(gabi::ea(&old.pos)).x = x;
        gabi::object_at<BmswPositionWords>(gabi::ea(&current.pos)).z = z;
        gabi::object_at<BmswPositionWords>(gabi::ea(&current.pos)).x = x;
        gabi::object_at<BmswPositionWords>(gabi::ea(&current.pos)).y = y;
        gabi::object_at<BmswPositionWords>(gabi::ea(&old.pos)).z = z;
    } else {
        field_0x9E0 = fpcM_ERROR_PROCESS_ID_e;
    }
    return TRUE;
}

/* 0220E340 */
void daNpc_Bmsw_c::set_mtx() {
    J3DModel* model = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), current.angle.y);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    J3DModel_setBaseTRMtx(field_0x6D4, getAnmMtx(model, m_jnt.mHeadJntNum));
    J3DModel_setBaseTRMtx(field_0x6D8, getAnmMtx(model, m_jnt.mBackboneJntNum));
    s8 anm = field_0x9D5;
    if (anm == 5 || anm == 6) {
        /* (anm == 5 && frame < 18.0f + REG10_F(5)) || (anm == 6 && frame < 18.0f): HD has no REG10_F(5) */
        if (mpMorfHand->mFrameCtrl.mFrame < 18.0f) {
            PSMTXCopy(getAnmMtx(mpMorfHand->getModel(), m_handR), mDoMtx_stack_c::get());
            mDoMtx_stack_c::transM(24.77f, -3.73f, 22.69f);
            /* cM_deg2s(98.02001654f), cM_deg2s(-61.737669014f), cM_deg2s(20.2807611f) */
            mDoMtx_XYZrotM(mDoMtx_stack_c::get(), 0x45B4, -0x2BE7, 0xE6C);
        } else {
            PSMTXCopy(getAnmMtx(mpMorfHand->getModel(), m_handL), mDoMtx_stack_c::get());
            mDoMtx_stack_c::transM(26.18f, 8.72f, 18.55f);
            /* cM_deg2s(113.05480612f), cM_deg2s(57.128904507f), cM_deg2s(-179.296869528f) */
            mDoMtx_XYZrotM(mDoMtx_stack_c::get(), 0x5065, 0x28A0, -0x7F80);
        }
        J3DModel_setBaseTRMtx(field_0x6DC, mDoMtx_stack_c::get());
    }
}

/* 0221047C */
void daNpc_Bmsw_c::setAttention() {
    f32 z = mAttPos.z;
    f32 y = mAttPos.y;
    f32 x = mAttPos.x;
    f32 yy = y + l_HIO.mNpc.mAttnYOffset;
    attention_info.position.z = z; /* attention_info.position */
    attention_info.position.x = x;
    attention_info.position.y = yy;
}

/* 022104A4 */
void daNpc_Bmsw_c::lookBack() {
    cXyz vec2;
    f32 vx = 0.0f, vy = 0.0f, vz = 0.0f; /* cXyz &vec(0.0f, 0.0f, 0.0f) */
    cXyz* dstPos = nullptr;
    s16 desired_y_rot = current.angle.y;
    switch (field_0x9DA) {
    case 1:
    case 2:
        if (mHasAttention) {
            cXyz eye;
            Bmsw_getPlayerEyePos(&eye, l_HIO.mNpc.m04);
            vec2.copy(eye);
            dstPos = &vec2;
            vx = current.pos.x;
            vy = eyePos.y;
            vz = current.pos.z;
        } else if (dComIfGs_isEventBit(0x2701) && field_0x9E0 != fpcM_ERROR_PROCESS_ID_e) {
            fopAc_ac_c* ac = fopAcM_SearchByID(field_0x9E0);
            if (ac != nullptr) {
                vec2.copy(ac->eyePos);
                dstPos = &vec2;
                vx = current.pos.x;
                vy = eyePos.y;
                vz = current.pos.z;
            }
        }
    }
    cXyz vec;
    if (m_jnt.mbTrn) { /* trnChk() */
        cLib_addCalcAngleS2(&field_0x97A, l_HIO.mNpc.mMaxHeadTurnVel, 4, 0x800);
        vec.set(vx, vy, vz);
        dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos, &vec, desired_y_rot,
                                  field_0x97A, true);
    } else {
        field_0x97A = 0;
        vec.set(vx, vy, vz);
        dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos, &vec, desired_y_rot, 0, true);
    }
}

/* 0221068C */
void daNpc_Bmsw_c::wait01() {
    if (field_0x9B9) {
        m_jnt.mbHeadLock = 0;
        m_jnt.mbBackBoneLock = 0;
        field_0x9DA = 2;
    } else {
        if (mHasAttention) {
            field_0x9D7 = 2;
        }
        if (!dComIfGs_isEventBit(0x2701)) {
            if (mpMorf->checkFrame(mpMorf->getEndFrame() - 1.0f)) {
                field_0x9D6 = (s8)(field_0x9D6 - 1);
                if (field_0x9D6 <= 0) {
                    if (field_0x9D5 == 5) {
                        field_0x9D6 = 1;
                        setAnm(6);
                    } else {
                        field_0x9D6 = (s8)gabi::ftoi(cM_rndF(4.0f) + 2.0f);
                        setAnm(5);
                    }
                }
            }
            if (field_0x9D5 == 5 || field_0x9D5 == 6) {
                m_jnt.mbHeadLock = 1;
                m_jnt.mbBackBoneLock = 1;
            } else {
                m_jnt.mbHeadLock = 0;
                m_jnt.mbBackBoneLock = 0;
            }
        }
    }
}

/* 02210804 */
void daNpc_Bmsw_c::talk01() {
    u16 talk_res = talk(1);
    /* dComIfGp_checkMesgSendButton(): play+0x5BD2 */
    if (dComIfGp_checkMesgSendButton() == 1) {
        u32 no = mCurrMsgNo;
        // HD: The final game-start message also activates the sorting camera.
        if (no == 0x1A32 || no == 0x1A42 || no == 0x1A47 || no == 0x1A4A || no == 0x1A55 ||
            no == 0x1A67 || no == 0x1A68) {
            /* mSwCam.ActiveOn() */
            mSwCam.mActive = 1;
            mSwCam.mCenter.copy(*SwCam_camera_center_data(0, 1));
            mSwCam.mEye.copy(*SwCam_camera_eye());
            mSwCam.mFovY = 58.0f;
            mSwCam.field_0x1C = 0;
            mSwCam.field_0x1D = 0;
        } else if (no == 0x1A39 || no == 0x1A4E || no == 0x1A5F || no == 0x1A59 || no == 0x1A6F) {
            /* dComIfGp_setItemRupeeCount(field_0x960): play+0x5B48 += n */
            s32 n = field_0x960;
            u32 a = dComIfGp_ea() + 0x5B48;
            gabi::object_at<be<s32>>(a) = gabi::object_at<be<s32>>(a).get() + n;
        }
    }
    if (talk_res == 0x12) { /* fopMsgStts_BOX_CLOSED_e */
        field_0x9DA = 1;
        dComIfGp_event_reset();
        if (!dComIfGs_isEventBit(0x2701)) {
            setAnm(0);
        } else {
            setAnm(1);
        }
        field_0x9D6 = 1;
        field_0x9B9 = 0;
        u32 no = mCurrMsgNo;
        if (no == 0x1A36 || no == 0x1A4B || no == 0x1A56 || no == 0x1A68) {
            Bmsw_setAction(this, FN_shiwake_game_action);
        }
    }
}

/* 02210AC0 */
BOOL daNpc_Bmsw_c::wait_action(void* arg) {
    if (mActionStatus == ACTION_STARTING) {
        field_0x9DA = 1;
        field_0x9D6 = 1;
        if (!dComIfGs_isEventBit(0x2701)) {
            // HD: In "Demo10", Koboli starts with the sorting animation.
            if (startStageIs(gabi::guest_addr("Demo10"))) {
                setAnm(5);
            } else {
                setAnm(0);
            }
        } else {
            setAnm(1);
        }
        mActionStatus = (s8)(mActionStatus + 1);
    } else if (mActionStatus != ACTION_ENDING) {
        s16 angle = (s16)(current.angle.y + m_jnt.mAngles[0][1] + m_jnt.mAngles[1][1]);
        cXyz pos;
        pos.set(current.pos.x, current.pos.y, current.pos.z);
        mHasAttention = chkAttention(&pos, angle);
        field_0x9D7 = 0;
        switch (field_0x9DA) {
        case 1:
            wait01();
            break;
        case 2:
            talk01();
            break;
        }
        lookBack();
        setAttention();
    }
    return TRUE;
}

/* 02210DCC */
BOOL daNpc_Bmsw_c::checkNextMailThrowOK() {
    u8 mailIdx = field_0x93C;
    if (mailIdx < 2) {
        mailIdx++;
    } else {
        mailIdx = 0;
    }
    u8 ret = 1;
    SwMail_c* mail = field_0x930[mailIdx];
    if (!pmf_is(&mail->mFunc, FN_SwMail_Dummy)) {
        if (!pmf_is(&mail->mFunc, FN_SwMail_End)) {
            ret = 0;
        }
    }
    return ret;
}

/* 02210E74 */
void daNpc_Bmsw_c::setGameGetRupee(s16 rupees) {
    s16 v;
    switch (dComIfGs_getEventReg(0xC203)) {
    case 0:
        if (rupees < 10) {
            v = 0;
        } else {
            v = rupees >> 1;
        }
        break;
    case 1:
        if (rupees < 15) {
            v = 0;
        } else if (rupees < 20) {
            v = rupees >> 1;
        } else {
            v = rupees;
        }
        break;
    case 2:
        if (rupees < 20) {
            v = 0;
        } else if (rupees < 25) {
            v = rupees;
        } else {
            v = (s16)(rupees * 3);
        }
        break;
    case 3:
    default:
        if (dComIfGs_isEventBit(0x2701)) {
            v = rupees;
        } else {
            v = (s16)(rupees * 3);
        }
        break;
    }
    field_0x960 = v;
    gabi::object_at<dComIfG_play_c>(dComIfGp_ea() + 0x12A0).mMsgCountNumber =
        v; /* dComIfGp_setMessageCountNumber */
}

/* 02210D20 */
void daNpc_Bmsw_c::TimerCountDown() {
    s32 rest = dComIfG_getTimerRestTimeMs();
    if (field_0x9B4 > rest) {
        if (field_0x9B4 <= 10000) {
            mDoAud_seStart_id(JA_SE_MINIGAME_TIMER_30); /* JA_SE_MINIGAME_TIMER_30 */
            field_0x9B4 = field_0x9B4 - 1000;
        } else {
            mDoAud_seStart_id(JA_SE_MINIGAME_TIMER_10); /* JA_SE_MINIGAME_TIMER_10 */
            field_0x9B4 = field_0x9B4 - 10000;
        }
    }
}

/* 02210F60 */
BOOL daNpc_Bmsw_c::shiwake_game_action(void* arg) {
    /* static cXyz aim_pos_data[2][3]: guard 0x104667FC, object 0x10466760 */
    if (l_aimPositionGuard.get() == 0) {
        l_aimPositionGuard = 1;
        aim_pos_data.positions[0][0].set(-40.0f, 772.0f, 1035.0f);
        aim_pos_data.positions[0][1].set(-139.0f, 772.0f, 1035.0f);
        aim_pos_data.positions[0][2].set(-240.0f, 772.0f, 1035.0f);
        aim_pos_data.positions[1][0].set(-40.0f, 855.0f, 1035.0f);
        aim_pos_data.positions[1][1].set(-139.0f, 855.0f, 1035.0f);
        aim_pos_data.positions[1][2].set(-240.0f, 855.0f, 1035.0f);
    }
    /* static cXyz cursor_pos_data[2][3]: guard 0x10466800, object 0x104667A8 */
    if (l_cursorPositionGuard.get() == 0) {
        cursor_pos_data.positions[0][0].set(-52.0f, 804.0f, 996.0f);
        cursor_pos_data.positions[0][1].set(-140.0f, 804.0f, 996.0f);
        cursor_pos_data.positions[0][2].set(-227.0f, 804.0f, 996.0f);
        cursor_pos_data.positions[1][0].set(-52.0f, 889.0f, 996.0f);
        cursor_pos_data.positions[1][1].set(-140.0f, 889.0f, 996.0f);
        cursor_pos_data.positions[1][2].set(-227.0f, 889.0f, 996.0f);

        l_cursorPositionGuard = 1;
    }
#define AIM_POS(row, col) (&aim_pos_data.positions[row][col])
#define CURSOR_POS(row, col) (&cursor_pos_data.positions[row][col])

    if (mActionStatus == ACTION_STARTING) {
        field_0x9B0 = 0;
        field_0x9DA = 3;
        field_0x9B1 = 0;
        if (!eventInfo_checkCommandDemoAccrpt(this)) {
            BOOL two = dComIfGs_isEventBit(0x2701);
            /* getEventIdx's result register is passed on as is (typed u32) */
            u32 idx;
            if (!two) {
                idx = Bmsw_getEventIdx(dComIfGp_getPEvtManager(), "SHIWAKEGAME", 0xFF);
            } else {
                idx = Bmsw_getEventIdx(dComIfGp_getPEvtManager(), "SHIWAKEGAME2", 0xFF);
            }
            field_0x9D8 = (s16)idx;
            Bmsw_orderOtherEventId(this, idx, 0xFF, (u16)0xFFFF, 0,
                                   1);      /* fopAcM_orderOtherEventId */
            eventInfo_onCondition(this, 2); /* dEvtCnd_UNK2_e */
            return FALSE;
        }
        if (!field_0x962) {
            /* dTimer_createTimer(6, limit, 3, 4, 221.0f, field_0x48, 32.0f, field_0x4C) */
            Bmsw_timerCreate(6, (u16)l_HIO.field_0x30, 3, 4, 221.0f, (f32)l_HIO.field_0x48, 32.0f,
                             (f32)l_HIO.field_0x4C);
            field_0x962 = 1;
            gabi::object_at<dComIfG_play_c>(dComIfGp_ea() + 0x12A0).mMiniGameRupee =
                0; /* dComIfGp_setMiniGameRupee(0) */
            Bmsw_beginSortingGame();
        }
        if (Bmsw_timerAddress() == 0) {
            return FALSE;
        }
        /* dComIfG_TimerStart(6, field_0x32) (HD inline) */
        s16 limit = l_HIO.field_0x32;
        if (gabi::object_at<dComIfG_play_c>(dComIfGp_ea() + 0x12A0).mTimerInfo.mTimerMode.get() ==
            6) {
            u32 timer = Bmsw_timerAddress();
            if (timer != 0) {
                if (limit == 0) {
                    Bmsw_timerStart(timer); /* dTimer_c::start() */
                } else {
                    Bmsw_timerStartWithLimit(timer, limit); /* dTimer_c::start(s16) */
                }
            }
        }
        field_0x963 = 1;
        field_0x930[0]->init();
        field_0x930[0]->AppearInit();
        field_0x930[1]->init();
        field_0x930[1]->DummyInit();
        field_0x930[2]->init();
        field_0x930[2]->DummyInit();
        s32 row = field_0x9B1;
        s32 col = field_0x9B0;
        field_0x93C = 0;
        mActionStatus = (s8)(mActionStatus + 1);
        Bmsw_cursorSetPos(mpShopCursor.get(),
                          CURSOR_POS(row, col + 1)); /* mpShopCursor->setPos() */
        mpShopCursor->m54 = 1;                       /* mpShopCursor->show() */
        field_0x9B4 = 20000;
        Bmsw_padRepeatOn(Bmsw_padAddress(), 5, 5); /* HD: cursor repeat on */
        return TRUE;
    } else if (mActionStatus == ACTION_ENDING) {
        field_0x962 = 0;
        field_0x963 = 0;
        Bmsw_padRepeatOff(Bmsw_padAddress()); /* HD: cursor repeat off */
        return TRUE;
    }

    if (dComIfG_getTimerRestTimeMs() > 0) {
        TimerCountDown();
        // HD: Controller trigger/repeat bits replace STControl input.
        u32 in = Bmsw_padAddress();
        s8 x = field_0x9B0;
        u32 trig = gabi::object_at<BmswPad>(in).mTrigger.get() |
                   gabi::object_at<BmswPad>(in).mRepeatTrigger.get();
        if (gabi::object_at<BmswPad>(in).mDirectionalHold.get() & 0x00F00000)
            trig &= 0x00F00000;
        if (x > -1 && (trig & 0x00440000)) {             /* left */
            mDoAud_seStart_id(JA_SE_LETTER_GAME_CURSOR); /* JA_SE_LETTER_GAME_CURSOR */
            field_0x9B0 = (s8)(field_0x9B0 - 1);
        } else if (x < 1 && (trig & 0x00880000)) { /* right */
            mDoAud_seStart_id(JA_SE_LETTER_GAME_CURSOR);
            field_0x9B0 = (s8)(field_0x9B0 + 1);
        }
        if (field_0x9B1 < 1 && (trig & 0x00110000)) { /* up */
            mDoAud_seStart_id(JA_SE_LETTER_GAME_CURSOR);
            field_0x9B1 = (s8)(field_0x9B1 + 1);
        } else if (field_0x9B1 > 0 && (trig & 0x00220000)) { /* down */
            mDoAud_seStart_id(JA_SE_LETTER_GAME_CURSOR);
            field_0x9B1 = (s8)(field_0x9B1 - 1);
        }
        if (pmf_is(&field_0x930[field_0x93C]->mFunc, FN_SwMail_Wait) &&
            gabi::object_at<BmswTimer>(Bmsw_timerAddress()).mStatus.get() !=
                1 /* Bmsw_timerAddress()->getStatus() */
            && Bmsw_padCheckTriggerA(0) /* CPad_CHECK_TRIG_A(0) */ && checkNextMailThrowOK()) {
            s32 col = field_0x9B0;
            s32 row = field_0x9B1;
            u8 idx = (u8)(col - row * 3 + 4); /* (col + 1) + (1 - row) * 3 */
            SwMail_c* mail = field_0x930[field_0x93C];
            cXyz* src = AIM_POS(row, col + 1);
            cXyz aim_pos;
            aim_pos.set(src->x, src->y, src->z);
            mail->ThrowInit(&aim_pos, idx);
            if (field_0x93C < 2) {
                field_0x93C = (u8)(field_0x93C + 1);
            } else {
                field_0x93C = 0;
            }
            mDoAud_seStart_id(JA_SE_LETTER_GAME_OK); /* JA_SE_LETTER_GAME_OK */
            field_0x930[field_0x93C]->AppearInit();
        }
        s8 row = field_0x9B1;
        s8 col = field_0x9B0;
        mSwCam.field_0x1D = row; /* mSwCam.setAimIdx(field_0x9B0, field_0x9B1) */
        mSwCam.field_0x1C = col;
        Bmsw_cursorSetPos(mpShopCursor.get(), CURSOR_POS(row, col + 1));
        /* mpShopCursor->setScale(0.65f, 0.9f, 0.5f, 27.0f, 20.0f) */
        mpShopCursor->m38 = 27.0f;
        mpShopCursor->m48 = 0.65f;
        mpShopCursor->m3C = 20.0f;
        mpShopCursor->m50 = 0.5f;
        mpShopCursor->m4C = 0.9f;
        Bmsw_cursorAnimationPlay(mpShopCursor.get()); /* anm_play() */
    } else if (mActionStatus == ACTION_ONGOING) {
        mDoAud_seStart_id(JA_SE_LETTER_GAME_TIMER_0);              /* JA_SE_LETTER_GAME_TIMER_0 */
        Bmsw_timerEnd(Bmsw_timerAddress(), (s32)l_HIO.field_0x34); /* dTimer_c::end */
        mpShopCursor->m54 = 0;                                     /* hide() */
        mActionStatus = (s8)(mActionStatus + 1);
    } else if (mActionStatus == ACTION_UNK_2) {
        if (Bmsw_timerDeleteCheck(Bmsw_timerAddress()) /* deleteCheck() */) {
            Bmsw_timerDeleteRequest(Bmsw_timerAddress()); /* deleteRequest() */
            mActionStatus = (s8)(mActionStatus + 1);
        }
    } else if (Bmsw_timerAddress() == 0) {
        s32 staff_id = dComIfGp_evmng_getMyStaffId("Bmsw", nullptr, 0);
        dComIfGp_evmng_cutEnd(staff_id);
        if (dComIfGp_evmng_endCheck(field_0x9D8)) {
            dComIfGp_event_reset();
            s16 rupees = gabi::object_at<dComIfG_play_c>(dComIfGp_ea() + 0x12A0)
                             .mMiniGameRupee.get(); /* dComIfGp_getMiniGameRupee() */
            field_0x9C0 = daNpc_Bmsw_getGameEndMsg(rupees);
            setGameGetRupee(rupees);
            field_0x9D7 = 1;
            Bmsw_setAction(this, FN_wait_action);
            mSwCam.mActive = 0; /* ActiveOff() */
            if (gabi::object_at<dComIfG_play_c>(dComIfGp_ea() + 0x12A0).mMiniGameType.get() ==
                7) { /* dComIfGp_getMiniGameType() */
                Bmsw_finishSortingGame();
            }
        }
    }
    field_0x930[0]->move();
    field_0x930[1]->move();
    field_0x930[2]->move();
#undef AIM_POS
#undef CURSOR_POS
    return TRUE;
}

/* 0220F06C */
BOOL daNpc_Bmsw_c::_draw() {
    J3DModel* model = mpMorf->getModel();
    J3DModelData* headModelData = Bmsw_modelData(field_0x6D4);
    J3DModel* handModel = mpMorfHand->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), field_0x6D4, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), field_0x6D8, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), handModel, &tevStr);
    mDoExt_btpAnm_entry(&field_0x7F8, headModelData, field_0x80C);
    mpMorf->entryDL();
    mpMorfHand->entryDL();
    mDoExt_modelUpdateDL(field_0x6D4);
    mDoExt_modelUpdateDL(field_0x6D8);
    if (field_0x9D5 == 5 || field_0x9D5 == 6) {
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos,
                         &field_0x6E0); // HD: The debug lighting override is absent.

        u32 t = gabi::ea(&field_0x6E0);
        gabi::object_at<BmswTevWork>(t).mColorK0.r = (u8)l_HIO.r_1; /* mColorK0 (HD +0x98) */
        gabi::object_at<BmswTevWork>(t).mColorK0.g = (u8)l_HIO.g_1;
        gabi::object_at<BmswTevWork>(t).mColorK0.b = (u8)l_HIO.b_1;
        gabi::object_at<BmswTevWork>(t).mColorC0.r = l_HIO.r_2; /* mColorC0 (HD +0x90) */
        gabi::object_at<BmswTevWork>(t).mColorC0.g = l_HIO.g_2;
        gabi::object_at<BmswTevWork>(t).mColorC0.b = l_HIO.b_2;
        setLightTevColorType(dKy_getEnvlight(), field_0x6DC, &field_0x6E0);
        mDoExt_modelUpdateDL(field_0x6DC);
    }
    mDoExt_btpAnm_remove(headModelData);
    if (field_0x963 != 0) {
        for (s32 i = 0; i < 3; i++) {
            field_0x930[i]->draw(&tevStr);
        }
    }
    Bmsw_cursorDraw(mpShopCursor.get()); /* mpShopCursor->draw() */
    Bmsw_registFig(0x96 /* DSNAP_TYPE_NPC_BMSW */, this, &current.pos, current.angle.y, 1.0f, 1.0f,
                   1.0f);
    // HD: The debug display only initializes its two guarded color objects.
    if (l_HIO.mNpc.m22 != 0) {
        if (l_debugColorOneGuard.get() == 0) {
            l_debugColorOneGuard = 1;
            memcpy_g(&gabi::object_at<u8>(0x101FEBF4), &gabi::object_at<u8>(0x10017FE8), 4);
        }
        if (l_debugColorTwoGuard.get() == 0) {
            l_debugColorTwoGuard = 1;
            memcpy_g(&gabi::object_at<u8>(0x101FEBF8), &gabi::object_at<u8>(0x10017FEC), 4);
        }
    }
    return TRUE;
}

/* 0220EE44 */
BOOL daNpc_Bmsw_c::_execute() {
    dNpc_HIO_c& n = l_HIO.mNpc;
    m_jnt.setParam(n.mMaxBackboneX, n.mMaxBackboneY, n.mMinBackboneX, n.mMinBackboneY, n.mMaxHeadX,
                   n.mMaxHeadY, n.mMinHeadX, n.mMinHeadY, n.mMaxTurnStep);
    STControl_setWaitParm(&field_0x988, 5, 2, 3, 2, l_HIO.field_0x50, l_HIO.field_0x54, 0, 0x2000);
    playTexPatternAnm();
    mpMorf->play(&eyePos, 0, 0);
    mpMorf->calc();
    mpMorfHand->play(nullptr, 0, 0);
    mpMorfHand->calc();
    checkOrder();
    if (!fopNpc_npc_c::mEventCut.cutProc()) {
        pmf_call(this, &mCurrActionFunc, nullptr); /* (this->*mCurrActionFunc)(NULL) */
    }
    mSwCam.Move();
    eventOrder();
    fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
    dBgS_Acch_CrrPos(&mObjAcch, dComIfG_Bgsp());
    u32 gnd = gabi::ea(static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd)); /* mObjAcch.m_gnd */
    gabi::object_at<BmswTevWork>(gabi::ea(&tevStr)).mRoomNo =
        Bmsw_getRoomId(dComIfG_Bgsp(), gnd); /* mRoomNo = GetRoomId */
    gabi::object_at<BmswTevWork>(gabi::ea(&tevStr)).mEnvrIdxOverride =
        Bmsw_getPolyColor(dComIfG_Bgsp(), gnd); /* mEnvrIdxOverride = GetPolyColor */
    set_mtx();
    setCollision(60.0f, 150.0f);
    return TRUE;
}

/* 0220EB70 */
BOOL daNpc_Bmsw_c::_delete() {
    dComIfG_resDelete(&mPhs, "Bmsw");
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    mSwMail0.SeDelete();
    mSwMail1.SeDelete();
    mSwMail2.SeDelete();
    if (l_HIO.mNo >= 0) {
        mDoHIO_deleteChild(l_HIO.mNo);
        l_HIO.mNo = -1;
    }
    return TRUE;
}

/* 0220E0D0 */
static BOOL CallbackCreateHeap(fopAc_ac_c* i_this) { return ((daNpc_Bmsw_c*)i_this)->CreateHeap(); }

/* 0220E9D8 */
cPhs_State daNpc_Bmsw_c::_create() {
    /* fopAcM_ct(this, daNpc_Bmsw_c) */
    u32 cond = actor_condition;
    if (!(cond & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            Bmsw_actorConstruct(this);
            cond = actor_condition;
        }
        actor_condition = cond | fopAcCnd_INIT_e;
    }
    if (dComIfGs_getEventReg(0xC203) >= 3 && !dComIfGs_isEventBit(0x2701)) {
        fopAcM_create_name("Btsw", fopAcM_GetParam(this), &current.pos, fopAcM_GetRoomNo(this),
                           &current.angle, nullptr, 0);
        return cPhs_ERROR_e;
    }
    cPhs_State res = dComIfG_resLoad(&mPhs, "Bmsw");
    if (res == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)CallbackCreateHeap), 0xCDA0)) {
            return cPhs_ERROR_e;
        }
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
        if (l_HIO.mNo < 0) {
            l_HIO.mNo = mDoHIO_createChild(
                "g°idª¯Q[j" /* "リト族（仕分けゲーム）" */, &l_HIO);
        }
        if (!CreateInit()) {
            return cPhs_ERROR_e;
        }
    }
    return res;
}

/* 0220D98C */
BOOL daNpc_Bmsw_c::CreateHeap() {
    J3DModelData* modelData = (J3DModelData*)Bmsw_getObjectIDRes("Bmsw", dRes_ID_BMSW_BDL_BM_e);
    if (modelData == nullptr)
        JUT_ASSERT_fail("d_a_npc_bmsw.cpp", 0x612, "modelData != (0)");
    J3DAnmTransform* anm =
        (J3DAnmTransform*)Bmsw_getObjectIDRes("Bmsw", dRes_ID_BMSW_BCK_BM_TALK01_e);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, 2 /* EMode_LOOP */,
                                    1.0f, 0, -1, 1, nullptr, 0, 0x11020203);
    if (mpMorf.get() == nullptr || mpMorf->getModel() == nullptr)
        return FALSE;
    m_jnt.mHeadJntNum = jointIndex(modelData, "head");
    if (m_jnt.mHeadJntNum < 0)
        JUT_ASSERT_fail("d_a_npc_bmsw.cpp", 0x621, "m_jnt.getHeadJntNum() >= 0");
    m_neck_jnt_num = jointIndex(modelData, "neck");
    if (m_neck_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_bmsw.cpp", 0x623, "m_neck_jnt_num >= 0");
    m_jnt.mBackboneJntNum = jointIndex(modelData, "backbone");
    m_body_ArmL = jointIndex(modelData, "armL");
    m_body_ArmR = jointIndex(modelData, "armR");
    if (!(m_body_ArmL >= 0 || m_body_ArmR >= 0))
        JUT_ASSERT_fail("d_a_npc_bmsw.cpp", 0x627, "m_body_ArmL >= 0 || m_body_ArmR >= 0");

    J3DModelData* headModelData =
        (J3DModelData*)Bmsw_getObjectIDRes("Bmsw", dRes_ID_BMSW_BDL_BMHEAD11_e);
    field_0x6D4 = mDoExt_J3DModel__create(headModelData, 0x80000, 0x11020022);
    if (field_0x6D4.get() == nullptr)
        return FALSE;

    J3DModelData* armModelData =
        (J3DModelData*)Bmsw_getObjectIDRes("Bmsw", dRes_ID_BMSW_BDL_BMARM_e);
    J3DAnmTransform* armAnm =
        (J3DAnmTransform*)Bmsw_getObjectIDRes("Bmsw", dRes_ID_BMSW_BCK_BMARM_TALK01_e);
    mpMorfHand = mDoExt_McaMorf::create(nullptr, armModelData, nullptr, nullptr, armAnm, 2, 1.0f, 0,
                                        -1, 1, nullptr, 0, 0x11020203);
    if (mpMorfHand.get() == nullptr || mpMorfHand->getModel() == nullptr)
        return FALSE;
    m_ArmL = jointIndex(armModelData, "armLloc");
    m_ArmR = jointIndex(armModelData, "armRloc");
    m_handL = jointIndex(armModelData, "handL");
    m_handR = jointIndex(armModelData, "handR");
    if (!(m_ArmL >= 0 || m_ArmR >= 0))
        JUT_ASSERT_fail("d_a_npc_bmsw.cpp", 0x65C, "m_ArmL >= 0 || m_ArmR >= 0");
    if (!(m_handL >= 0 || m_handR >= 0))
        JUT_ASSERT_fail("d_a_npc_bmsw.cpp", 0x65D, "m_handL >= 0 || m_handR >= 0");

    J3DModelData* bagModelData =
        (J3DModelData*)Bmsw_getObjectIDRes("Bmsw", dRes_ID_BMSW_BDL_BM_BAG_e);
    field_0x6D8 = mDoExt_J3DModel__create(bagModelData, 0, 0x11020203);
    if (field_0x6D8.get() == nullptr)
        return FALSE;
    J3DModelData* letterModelData =
        (J3DModelData*)Bmsw_getObjectIDRes("Bmsw", dRes_ID_BMSW_BDL_BM_LETTER_e);
    field_0x6DC = mDoExt_J3DModel__create(letterModelData, 0, 0x11020203);
    if (field_0x6DC.get() == nullptr)
        return FALSE;

    field_0x9D4 = 0;
    if (!initTexPatternAnm(false))
        return FALSE;

    for (u16 i = 0; i < J3DModelData_getJointNum(modelData); i++) {
        if (i == (u32)(s32)m_jnt.mHeadJntNum || i == (u32)(s32)m_jnt.mBackboneJntNum ||
            i == (u32)(s32)m_neck_jnt_num || i == (u32)(s32)m_body_ArmL ||
            i == (u32)(s32)m_body_ArmR) {
            setJointCallBack(Bmsw_modelData(mpMorf->getModel()), i,
                             gabi::guest_fn((const void*)nodeCallBack));
        }
    }
    for (u16 i = 0; i < J3DModelData_getJointNum(armModelData); i++) {
        if (i == (u32)(s32)m_ArmR || i == (u32)(s32)m_ArmL) {
            setJointCallBack(armModelData, i, gabi::guest_fn((const void*)nodeCallBackArm));
        }
    }
    mpMorf->getModel()->mUserArea = gabi::ea(this); /* setUserArea */
    mpMorfHand->getModel()->mUserArea = gabi::ea(this);

    field_0x930[0] = &mSwMail0;
    field_0x930[1] = &mSwMail1;
    field_0x930[2] = &mSwMail2;
    field_0x93C = 0;
    cXyz* cameraCenter = &mSwCam.mCenter;
    cXyz* eyeP = &mSwCam.mEye;
    if (!field_0x930[0]->MailCreateInit(cameraCenter, eyeP))
        return FALSE;
    if (!field_0x930[1]->MailCreateInit(cameraCenter, eyeP))
        return FALSE;
    if (!field_0x930[2]->MailCreateInit(cameraCenter, eyeP))
        return FALSE;

    void* tevRegKey = Bmsw_getObjectIDRes("Bmsw", dRes_ID_BMSW_BRK_SHOP_CURSOR01_e);
    void* cursorModelData = Bmsw_getObjectIDRes("Bmsw", dRes_ID_BMSW_BMD_SHOP_CURSOR01_e);
    mpShopCursor = Bmsw_cursorCreate(cursorModelData, tevRegKey, 0.65f); /* ShopCursor_create */
    if (mpShopCursor.get() == nullptr)
        return FALSE;
    mpShopCursor->m54 = 0; /* mpShopCursor->m54 (HD +0xB4) */

    mAcchCir.SetWall(30.0f, 0.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}

/* 0220D884 */
BOOL SwMail_c::MailCreateInit(cXyz* param_1, cXyz* param_2) {
    J3DModelData* modelData = (J3DModelData*)Bmsw_getObjectIDRes("Bmsw", dRes_ID_BMSW_BDL_QMAIL_e);
    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11020002);
    if (mpModel.get() == nullptr) {
        return FALSE;
    }
    void* texPattern = Bmsw_getObjectIDRes("Bmsw", dRes_ID_BMSW_BTP_QMAIL_e);
    if (!mDoExt_btpAnm_init(&field_0x10, modelData, texPattern, TRUE, 2 /* EMode_LOOP */, 0.0f, 0,
                            -1, 0, FALSE)) {
        return FALSE;
    }
    field_0x5C = param_2;
    field_0x54 = 0;
    field_0x58 = param_1;
    init();
    DummyInit();
    return TRUE;
}

/* 0220D57C */
u8 Bmsw_getNextNo(u8 previous_no) {
    f32 random = cM_rndF(10.0f);
    u8 cnt = SwMail_c::m_same_count;
    f32 fVar4 = gabi::fnmsubs(1.25f, (f32)cnt, 2.5f); /* 2.5f - m_same_count * 1.25f (fused) */
    s32 box_x = previous_no / 3;
    s32 box_y = previous_no - box_x * 3;
    if (random < fVar4) {
        SwMail_c::m_same_count = (u8)(cnt + 1);
    } else {
        f32 new_threshold = 2.5f + fVar4;
        if (random < new_threshold) {
            if (cM_rndF(1.0f) > 0.5f) {
                if (box_y == 0 || box_y == 2) {
                    box_y = 1;
                } else if (cM_rndF(1.0f) > 0.5f) {
                    box_y = 0;
                } else {
                    box_y = 2;
                }
            } else {
                box_x = box_x == 0;
            }
        } else { // HD: also for NaN

            fVar4 = cM_rndF(1.0f);
            if (fVar4 < 0.3333f) {
                if (box_y == 0 || box_y == 2) {
                    box_y = 1;
                } else if (cM_rndF(1.0f) > 0.5f) {
                    box_y = 0;
                } else {
                    box_y = 2;
                }
                box_x = box_x == 0;
            } else if (fVar4 < 0.6666f) {
                if (box_y == 0) {
                    box_y = 2;
                } else if (box_y == 2) {
                    box_y = 0;
                } else if (cM_rndF(1.0f) > 0.5f) {
                    box_y = 0;
                } else {
                    box_y = 2;
                }
            } else {
                if (box_y == 0) {
                    box_y = 2;
                } else if (box_y == 2) {
                    box_y = 0;
                } else if (cM_rndF(1.0f) > 0.5f) {
                    box_y = 0;
                } else {
                    box_y = 2;
                }
                box_x = box_x == 0;
            }
        }
        SwMail_c::m_same_count = 0;
    }
    u8 res = (u8)(box_y + box_x * 3);
    SwMail_c::m_no_buff = res;
    return res;
}

/* 0220D50C */
void SwMail_c::init() {
    field_0x54 = 0;
    field_0x56 = 0;
    field_0x30.copy(*cXyz_Zero);
    field_0x3C.copy(*cXyz_Zero);
    field_0x55 = 0;
    cXyz* p = field_0x5C;
    field_0x24.set(p->x, p->y - 200.0f, p->z);
    set_mtx();
}

/* 0220D430 */
void SwMail_c::set_mtx() {
    mDoMtx_stack_c::transS(field_0x24.x + field_0x3C.x, field_0x24.y + field_0x3C.y,
                           field_0x24.z + field_0x3C.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), field_0x48.x, field_0x48.y, field_0x48.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}

/* 0220F2A0 */
void SwMail_c::set_mtx_throw() {
    mDoMtx_stack_c::transS(field_0x24.x + field_0x3C.x, field_0x24.y + field_0x3C.y,
                           field_0x24.z + field_0x3C.z);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), field_0x48.x);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), field_0x48.y);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}

/* 0220D7AC */
void SwMail_c::DummyInit() {
    cLib_targetAngleY(field_0x58, field_0x5C);
    field_0x48.y = -0x4000;
    field_0x48.z = 0;
    field_0x48.x = 0;
    cXyz* p = field_0x5C;
    field_0x24.set(p->x - 150.0f, p->y - 200.0f, p->z);
    field_0x54 = Bmsw_getNextNo(field_0x54);
    field_0x4E.z = -0x4000;
    field_0x4E.x = 0;
    field_0x4E.y = -0x4000;
    field_0x3C.copy(*cXyz_Zero);
    pmf_set(&mFunc, FN_SwMail_Dummy);
}

/* 0220F3C4 */
void SwMail_c::Dummy() { set_mtx(); }

/* 0220F3C8 */
void SwMail_c::AppearInit() {
    cLib_targetAngleY(field_0x58, field_0x5C);
    cXyz* p = field_0x5C;
    u8 no = field_0x54;
    field_0x48.y = -0x4000;
    field_0x48.x = 0;
    field_0x48.z = 0;
    field_0x24.set(p->x, p->y - 200.0f, p->z);
    u8 res = Bmsw_getNextNo(no);
    field_0x4E.z = -0x4000;
    field_0x4E.x = 0;
    field_0x4E.y = -0x4000;
    field_0x54 = res;
    field_0x3C.copy(*cXyz_Zero);
    field_0x56 = 0;
    mDoAud_seStart_id(JA_SE_LETTER_GAME_NEW); /* JA_SE_LETTER_GAME_NEW */
    pmf_set(&mFunc, FN_SwMail_Appear);
}

/* 0220F4C0 */
void SwMail_c::Appear() {
    s16 new_y = cLib_targetAngleY(field_0x58, field_0x5C);
    cXyz diff;
    cXyz_mi(field_0x58, &diff, field_0x5C);
    if (!cXyz_normalizeRS(&diff)) {
        diff.set(0.0f, 0.0f, 1.0f);
    }
    cXyz vec;
    vec.copy(*field_0x5C.get());
    cXyz tmp;
    cXyz_ml(&diff, &tmp, 120.0f);
    diff.copy(tmp);
    PSVECAdd(&vec, &diff, &vec);
    f32 y = vec.y - 40.0f;
    field_0x48.x = field_0x4E.x;
    field_0x48.y = (s16)(field_0x4E.y + new_y);
    vec.y = y;
    cLib_addCalcAngleS2(&field_0x48.z, field_0x4E.z, 4, 0x1000);
    if (std::fabs(cLib_addCalcPos(&field_0x24, &vec, 0.25f, 30.0f, 2.5f)) < 2.5f) {
        WaitInit();
    }
    set_mtx();
}

/* 0220F4A0 */
void SwMail_c::WaitInit() { pmf_set(&mFunc, FN_SwMail_Wait); }

/* 0220F62C */
void SwMail_c::Wait() {
    s16 new_y = cLib_targetAngleY(field_0x58, field_0x5C);
    cXyz diff;
    cXyz_mi(field_0x58, &diff, field_0x5C);
    if (!cXyz_normalizeRS(&diff)) {
        diff.set(0.0f, 0.0f, 1.0f);
    }
    field_0x24.copy(*field_0x5C.get());
    cXyz tmp;
    cXyz_ml(&diff, &tmp, 120.0f);
    diff.copy(tmp);
    PSVECAdd(&field_0x24, &diff, &field_0x24);
    f32 y = field_0x24.y - 40.0f;
    field_0x48.y = (s16)(field_0x4E.y + new_y);
    field_0x48.z = field_0x4E.z;
    field_0x48.x = field_0x4E.x;
    field_0x24.y = y;
    set_mtx();
}

/* 0220F744 */
void SwMail_c::ThrowInit(cXyz* param_1, u8 param_2) {
    field_0x30.copy(*param_1);
    field_0x56 = 0;
    field_0x55 = param_2;
    pmf_set(&mFunc, FN_SwMail_Throw);
}

/* 0220F7C4 */
void SwMail_c::Throw() {
    u8 step = field_0x56;
    s16 x_angle = cLib_targetAngleX(
        &field_0x30, field_0x5C); // HD: The throw angle is calculated before either phase.

    if (step == 0) {
        s16 y_angle = cLib_targetAngleY(field_0x58, field_0x5C);
        cXyz vec;
        vec.set(-40.0f, -50.0f, 0.0f);
        cXyz multVec;
        mDoMtx_ZXYrotS(mDoMtx_stack_c::get(), x_angle, y_angle, 0);
        PSMTXMultVec(mDoMtx_stack_c::get(), &vec, &multVec);
        PSVECAdd(&multVec, field_0x5C, &multVec);
        if (cLib_addCalcPos(&field_0x24, &multVec, 0.5f, 30.0f, 2.0f) < 1.0f) {
            field_0x48.y = 0;
            field_0x4E.x = 0;
            mDoAud_seStart_id(JA_SE_LK_SW_KAZEKIRI_S); /* JA_SE_LK_SW_KAZEKIRI_S */
            field_0x56 = (u8)(field_0x56 + 1);
        }
        set_mtx();
    } else {
        field_0x48.x = (s16)-x_angle;
        s16 add = (s16)(field_0x54 * 0x80 + 0x1000);
        field_0x48.z = 0;
        field_0x48.y = (s16)(field_0x48.y + add);
        f32 pos_step = cLib_addCalcPos(&field_0x24, &field_0x30, 0.5f, l_HIO.field_0x44, 1.0f);
        field_0x4E.x = (s16)(field_0x4E.x + 4000);
        field_0x3C.y = 20.0f * (1.0f - cM_scos(field_0x4E.x));
        if (pos_step < 1.0f) {
            mDoAud_seStart_pos(JA_SE_LETTER_IN_BOX, &field_0x24); /* JA_SE_LETTER_IN_BOX */
            u8 aim = field_0x55;
            u8 no = field_0x54;
            if (aim == no) {
                Bmsw_addMiniGameRupee();
                mDoAud_seStart_id(JA_SE_MINIGAME_RIGHT); /* JA_SE_MINIGAME_RIGHT */
            } else {
                cXyz dir;
                dir.set(0.0f, 1.0f, 0.0f);
                dComIfGp_getVibration_StartShock(3, 9, &dir);
                mDoAud_seStart_id(JA_SE_MINIGAME_WRONG); /* JA_SE_MINIGAME_WRONG */
            }
            EndInit();
        }
        set_mtx_throw();
    }
}

/* 0220F784 */
void SwMail_c::EndInit() {
    field_0x30.copy(*cXyz_Zero);
    field_0x30.z = -10.0f;
    pmf_set(&mFunc, FN_SwMail_End);
}

/* 0220FA84 */
void SwMail_c::End() {
    if (field_0x55 == field_0x54) {
        cLib_addCalcAngleS2(&field_0x48.y, 0, 4, 0x1000);
        cXyz target;
        target.set(cXyz_Zero->x, cXyz_Zero->y, cXyz_Zero->z);
        cLib_addCalcPos2(&field_0x3C, &target, 0.25f, 5.0f);
        set_mtx_throw();
    } else {
        cLib_addCalcAngleS2(&field_0x48.y, -0x8000, 4, 0x1000);
        cXyz target;
        target.set(cXyz_Zero->x, cXyz_Zero->y, cXyz_Zero->z);
        cLib_addCalcPos2(&field_0x3C, &target, 0.25f, 5.0f);
        if (field_0x24.z < 995.0f) {
            s16 tgt;
            if (field_0x24.y > 720.0f) {
                tgt = -0x4000;
                field_0x30.y = field_0x30.y - 3.0f;
            } else {
                tgt = 0;
                field_0x30.y = field_0x30.y * 0.6f;
                field_0x30.x = field_0x30.x * 0.9f;
                field_0x30.z = field_0x30.z * 0.9f;
            }
            if (!(field_0x24.y > 700.0f)) {
                field_0x24.y = 700.0f;
                field_0x30.y = 0.0f;
            }
            cLib_addCalcAngleS2(&field_0x48.x, tgt, 2, 0x800);
        }
        PSVECScale(&field_0x30, &field_0x30, 0.9f);
        PSVECAdd(&field_0x24, &field_0x30, &field_0x24);
        set_mtx_throw();
    }
}

/* 0220EB68 */
void SwMail_c::SeDelete() { mDoAud_seDeleteObject(&field_0x24); }

/* 0220F380 */
void SwMail_c::move() { pmf_call0(this, &mFunc); /* (this->*mFunc)() */ }

/* 0220EFF4 */
void SwMail_c::draw(dKy_tevstr_c* tevStr) {
    J3DModelData* modelData = Bmsw_modelData(mpModel);
    mDoExt_btpAnm_entry(&field_0x10, modelData, field_0x54);
    dScnKy_env_light_c* l = dKy_getEnvlight();
    setLightTevColorType(l, mpModel, tevStr);
    mDoExt_modelUpdateDL(mpModel);
    mDoExt_btpAnm_remove(modelData);
}

/* 0220ED04 */
void SwCam_c::Move() {
    if (mActive) {
        /* dComIfGp_getCamera(dComIfGp_getPlayerCameraID(0)): play+0x5AF8 + id * 0x34, id at play+0x5B30 */
        s32 camId =
            gabi::object_at<dComIfG_play_c>(dComIfGp_ea() + 0x12A0).mPlayerInfo[0].mCameraID.get();
        u32 camera = gabi::ea(dComIfGp_getCamera(camId));
        cXyz* src = SwCam_camera_center_data(field_0x1D, field_0x1C + 1);
        cXyz center;
        center.set(src->x, src->y, src->z);
        cLib_addCalcPos(&mCenter, &center, 0.25f, 10.0f, 1.0f);
        Bmsw_cameraStay(camera + 0x248); /* camera->mCamera.Stay() */
        cXyz c;
        cXyz e;
        c.set(mCenter.x, mCenter.y, mCenter.z);
        e.set(mEye.x, mEye.y, mEye.z);
        Bmsw_cameraSet(camera + 0x248, &c, &e, (f32)mFovY,
                       0); /* mCamera.Set(&center, eye, fovy, 0) */
    }
}

/* 0220EB64 */
static cPhs_State daNpc_Bmsw_Create(fopAc_ac_c* i_this) {
    return ((daNpc_Bmsw_c*)i_this)->_create();
}

/* 0220EBFC */
static BOOL daNpc_Bmsw_Delete(daNpc_Bmsw_c* i_this) { return i_this->_delete(); }

/* 0220EFF0 */
static BOOL daNpc_Bmsw_Execute(daNpc_Bmsw_c* i_this) { return i_this->_execute(); }

/* 0220F29C */
static BOOL daNpc_Bmsw_Draw(daNpc_Bmsw_c* i_this) { return i_this->_draw(); }

/* 02211F54 */
static BOOL daNpc_Bmsw_IsDelete(daNpc_Bmsw_c* i_this) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.

/* 02211F40 */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 02212004 */
static void SafeString_assureTerminationImpl_(SafeString* i_this) {}

/* 02211DC4 */
static void __sinit_d_a_npc_bmsw_cpp() {
    /* the per-TU header statics (see bindings.h sinit_header_statics), in this unit's layout */
    for (int i = 0; i < 4; i++)
        l_zeroWords[i] = 0;
    __register_global_object(0x101BD48C);
    l_piLow = -3.1415927f;
    l_piHigh = 3.1415927f;
    Bmsw_xyzInit(0x104666D0);
    __register_global_object(0x101BD498);
    Bmsw_angleInit(0x104666D1);
    __register_global_object(0x101BD4A4);
    l_largeFloatFour = 10000.0f;
    l_largeFloatOne = 50000.0f;
    l_largeFloatTwo = 50000.0f;
    l_largeFloatThree = 10000.0f;
    daNpc_Bmsw_HIO_c_ct(&l_HIO);
    SwCam_camera_center_data(0, 0)->set(-104.0f, 805.0f, 875.0f);
    SwCam_camera_center_data(0, 1)->set(-139.0f, 805.0f, 882.0f);
    SwCam_camera_center_data(0, 2)->set(-173.0f, 805.0f, 875.0f);
    SwCam_camera_center_data(1, 0)->set(-104.0f, 825.0f, 868.0f);
    SwCam_camera_center_data(1, 1)->set(-139.0f, 825.0f, 875.0f);
    SwCam_camera_center_data(1, 2)->set(-173.0f, 825.0f, 868.0f);
    SwCam_camera_eye()->set(-139.0f, 815.0f, 662.0f);
}

/* 0220E0D4 */
static daNpc_Bmsw_c* daNpc_Bmsw_c_ct(daNpc_Bmsw_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Bmsw_c*)operator_new(0xD84);
        if (i_this == nullptr)
            return i_this;
    }
    Bmsw_npcConstruct(i_this); /* fopNpc_npc_c::fopNpc_npc_c */
    i_this->__vtbl = l_actorVtable;
    /* dKy_tevstr_c field_0x6E0 (HD, inline constructor): three 0x44-byte light blocks at +0, +0xC0
     * and +0x144 initialised from the template at 0x1016E414 (floats, then bytes and shorts) */
    for (int b = 0; b < 3; b++) {
        u32 offset = b == 0 ? 0 : b == 1 ? 0xC0 : 0x144;
        auto& block =
            gabi::object_at<BmswLightDefaultBlock>(gabi::ea(&i_this->field_0x6E0) + offset);
        for (int i = 0; i < 6; i++)
            block.mVectors[i] = l_lightDefault.mVectors[i];
        for (int i = 0; i < 4; i++)
            block.mColor[i] = l_lightDefault.mColor[i];
        for (int i = 0; i < 4; i++)
            block.mSignedColor[i] = l_lightDefault.mSignedColor[i];
        for (int i = 0; i < 8; i++)
            block.mFactors[i] = l_lightDefault.mFactors[i];
    }
    Bmsw_btpConstruct(&i_this->field_0x7F8); /* mDoExt_btpAnm::mDoExt_btpAnm */
    Bmsw_btpConstruct(&i_this->mSwMail0.field_0x10);
    Bmsw_btpConstruct(&i_this->mSwMail1.field_0x10);
    Bmsw_btpConstruct(&i_this->mSwMail2.field_0x10);
    /* SwCam_c::SwCam_c() */
    i_this->mSwCam.mActive = 0;
    i_this->mSwCam.field_0x1C = 0;
    i_this->mSwCam.field_0x1D = 0;
    i_this->mSwCam.mCenter.copy(*SwCam_camera_center_data(0, 1));
    i_this->mSwCam.mEye.copy(*SwCam_camera_eye());
    i_this->mSwCam.mFovY = 58.0f;
    /* STControl::STControl(15, 15, 0, 0, 0.9f, 0.5f, 0, 0x2000) */
    i_this->field_0x988.__vtbl = 0x10050788;
    STControl_setWaitParm(&i_this->field_0x988, 15, 15, 0, 0, 0.9f, 0.5f, 0, 0x2000);
    Bmsw_stickInit(&i_this->field_0x988); /* STControl::init */
    return i_this;
}

/* 02211F5C */
static void daNpc_Bmsw_c_dt(daNpc_Bmsw_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        Bmsw_stickDestroy(&i_this->field_0x988, 2); /* STControl::~STControl */
        Bmsw_cylinderDestroy(&i_this->mCyl,
                             2); /* dCcD_Cyl::~dCcD_Cyl (matcher: dBgS_Acch::~dBgS_Acch) */
        Bmsw_statusDestroy(&i_this->mStts, 2);                    /* dCcD_Stts::~dCcD_Stts */
        Bmsw_circleDestroy(gabi::ea(&i_this->mAcchCir.m_cir), 2); /* mAcchCir's cM3dGCir */
        /* dBgS_ObjAcch::~dBgS_ObjAcch (inline): vtables, then dBgS_Acch::~dBgS_Acch */
        i_this->mObjAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcchDt[0];
        i_this->mObjAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcchDt[1];
        Bmsw_acchDestroy(&i_this->mObjAcch, 0);
        Bmsw_actorDestroy(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}

// Declarations consumed by the verifier; these bind named calls to the HD ABI.
/* WWHD glue
ext 025A5AC8 void Bmsw_particleCallbackConstruct(dPa_followEcallBack * arg0)
ext 026067F4 void * Bmsw_resourceGetID(dRes_control_c * arg0, SafeString * arg1, s32 arg2)
ext 02055B64 s16 Bmsw_calcTimer(be<s16> * arg0)
ext 025E789C int Bmsw_btpInit(void * arg0, J3DModelData * arg1, void * arg2, s32 arg3, s32 arg4, f32 arg5, s32 arg6, s32 arg7, u32 arg8, s32 arg9)
ext 025E1988 void Bmsw_seStart(u32 arg0)
ext 0200F974 s16 Bmsw_targetAngleX(const cXyz * arg0, const cXyz * arg1)
ext 0200EE00 f32 Bmsw_addCalcPos(cXyz * arg0, const cXyz * arg1, f32 arg2, f32 arg3, f32 arg4)
ext 0200F164 void Bmsw_addCalcPos2(cXyz * arg0, const cXyz * arg1, f32 arg2, f32 arg3)
ext 0201B47C bool Bmsw_normalizeRS(cXyz * arg0)
ext 025F1AA4 void Bmsw_ZXYrotS(Mtx34 * arg0, s16 arg1, s16 arg2, s16 arg3)
ext 025E19CC void Bmsw_seStartPosition(u32 arg0, cXyz * arg1)
ext 0259D54C void Bmsw_playerEyePos(cXyz * arg0, f32 arg1)
ext 0259DED0 void Bmsw_jointLookAtTarget(dNpc_JntCtrl_c * arg0, be<s16> * arg1, cXyz * arg2, cXyz * arg3, s16 arg4, s16 arg5, u8 arg6)
ext 02212004 void Bmsw_assureTermination(SafeString * arg0)
ext 025885C4 void Bmsw_stickSetWaitParm(BmswSTControl * arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4, f32 arg5, f32 arg6, s16 arg7, s16 arg8)
ext 025F19F8 void Bmsw_XYZrotM(Mtx34 * arg0, s16 arg1, s16 arg2, s16 arg3)
ext 025D5AA4 u32 Bmsw_createChildByName(const char * arg0, u32 arg1, u32 arg2, cXyz * arg3, s32 arg4, csXyz * arg5, cXyz * arg6, u32 arg7)
ext 025D58B4 u32 Bmsw_createActorByName(const char * arg0, u32 arg1, cXyz * arg2, s32 arg3, csXyz * arg4, cXyz * arg5, u32 arg6)
ext 0255FFF4 void Bmsw_tevstrInit(dKy_tevstr_c * arg0, s8 arg1, u8 arg2)
ext 025BEBB8 void Bmsw_registFigure(u8 arg0, fopAc_ac_c * arg1, cXyz * arg2, s16 arg3, f32 arg4, f32 arg5, f32 arg6)
ext 0259DA18 void Bmsw_npcHioConstruct(u32 arg0)
ext 028ED6F8 void Bmsw_xyzInit(int arg0)
ext 028EAB2C void Bmsw_angleInit(int arg0)
ext 02514F44 void Bmsw_cameraStay(u32 arg0)
ext 02514F88 void Bmsw_cameraSet(u32 arg0, cXyz * arg1, cXyz * arg2, f32 arg3, int arg4)
ext 02520A84 int Bmsw_isStageBossEnemy(int arg0)
ext 0220E0D4 void Bmsw_actorConstruct(daNpc_Bmsw_c * arg0)
ext 024EF130 s8 Bmsw_getRoomId(dBgS * arg0, u32 arg1)
ext 024EEEB8 u8 Bmsw_getPolyColor(dBgS * arg0, u32 arg1)
ext 025BD338 void Bmsw_cursorDraw(ShopCursor_c * arg0)
ext 027F68FC u32 Bmsw_jointNameHeader(J3DModelData * arg0)
ext 027DF9B0 s8 Bmsw_jointNameIndex(u32 arg0, const char * arg1)
ext 027F3F94 u32 Bmsw_jointTreeHeader(J3DModelData * arg0)
ext 025BBD7C ShopCursor_c * Bmsw_cursorCreate(void * arg0, void * arg1, f32 arg2)
ext 025A1458 void Bmsw_npcConstruct(daNpc_Bmsw_c * arg0)
ext 025E7820 void Bmsw_btpConstruct(void * arg0)
ext 025885E8 void Bmsw_stickInit(BmswSTControl * arg0)
ext 025886D8 void Bmsw_stickDestroy(BmswSTControl * arg0, int arg1)
ext 02515A70 void Bmsw_cylinderDestroy(dCcD_Cyl * arg0, int arg1)
ext 02515860 void Bmsw_statusDestroy(dCcD_Stts * arg0, int arg1)
ext 02018034 void Bmsw_circleDestroy(u32 arg0, int arg1)
ext 024EFD9C void Bmsw_acchDestroy(dBgS_ObjAcch * arg0, int arg1)
ext 025D50BC void Bmsw_actorDestroy(daNpc_Bmsw_c * arg0, int arg1)
ext 02676900 void Bmsw_startMiniGame(u32 arg0)
ext 026768E4 void Bmsw_endMiniGame(u32 arg0)
ext 02543F10 u32 Bmsw_getEventIdx(dEvent_manager_c * arg0, const char * arg1, int arg2)
ext 025D7A58 int Bmsw_orderOtherEventId(daNpc_Bmsw_c * arg0, u32 arg1, int arg2, u16 arg3, int arg4, int arg5)
ext 025C60F4 void Bmsw_timerCreate(int arg0, u16 arg1, int arg2, int arg3, f32 arg4, f32 arg5, f32 arg6, f32 arg7)
ext 025C5864 void Bmsw_timerStart(u32 arg0)
ext 025C6250 void Bmsw_timerStartWithLimit(u32 arg0, s16 arg1)
ext 025BD31C void Bmsw_cursorSetPos(ShopCursor_c * arg0, cXyz * arg1)
ext 02618760 void Bmsw_padRepeatOn(u32 arg0, int arg1, int arg2)
ext 02618774 void Bmsw_padRepeatOff(u32 arg0)
ext 02007898 int Bmsw_padCheckTriggerA(int arg0)
ext 025BD290 void Bmsw_cursorAnimationPlay(ShopCursor_c * arg0)
ext 025C58A8 void Bmsw_timerEnd(u32 arg0, s32 arg1)
ext 025C6278 int Bmsw_timerDeleteCheck(u32 arg0)
ext 025C58D8 void Bmsw_timerDeleteRequest(u32 arg0)
data l_btp_ix_tbl 10017FE0
data l_bck_ix_tbl 1001804C
data l_arm_bck_ix_tbl 10018068
data a_play_mode_tbl 101BD438
data a_morf_frame_tbl 101BD454
data a_play_speed_tbl 101BD470
str 1001831C "Bmsw" in 02210F60
str 10018330 "SHIWAKEGAME2" in 02210F60
str 10018324 "SHIWAKEGAME" in 02210F60
str 10018118 "handR" in 0220D98C
str 10018110 "handL" in 0220D98C
str 10018108 "armRloc" in 0220D98C
str 10018100 "armLloc" in 0220D98C
str 10018140 "armR" in 0220D98C
str 10018138 "armL" in 0220D98C
str 100181A0 "backbone" in 0220D98C
str 10018130 "neck" in 0220D98C
str 10018128 "head" in 0220D98C
str 1001815C "d_a_npc_bmsw.cpp" in 0220F06C
str 10018120 "Bmsw" in 0220F06C
str 10018268 "g°idª¯Q[j" in 0220E9D8
str 10018260 "Bmsw" in 0220E9D8
str 10018258 "Btsw" in 0220E9D8
str 10018250 "Btsw" in 0220E6E8
str 10018248 "Bmsw" in 0220E6E8
str 10018280 "Bmsw" in 0220EB70
str 100180F0 "Bmsw" in 0220D884
str 100180F0 "Bmsw" in 0220D884
str 100182BC "Bmsw" in 0220FC70
str 100182BC "Bmsw" in 0220FC70
str 10018098 "m_head_tex_pattern != (0)" in 0220D320
str 100180B4 "d_a_npc_bmsw.cpp" in 0220D320
str 10018090 "Bmsw" in 0220D320

str 10018120 "Bmsw" in 0220D98C
str 1001815C "d_a_npc_bmsw.cpp" in 0220D98C
str 100181F0 "m_handL >= 0 || m_handR >= 0" in 0220D98C
str 100181D4 "m_ArmL >= 0 || m_ArmR >= 0" in 0220D98C
str 100181AC "m_body_ArmL >= 0 || m_body_ArmR >= 0" in 0220D98C
str 10018148 "m_neck_jnt_num >= 0" in 0220D98C
str 10018184 "m_jnt.getHeadJntNum() >= 0" in 0220D98C
str 10018170 "modelData != (0)" in 0220D98C

ext 0220D884 BOOL Bmsw_guest_SwMail_c_MailCreateInit(SwMail_c* self, cXyz* a0, cXyz* a1)
ext 0220D50C void Bmsw_guest_SwMail_c_init(SwMail_c* self)
ext 0220D430 void Bmsw_guest_SwMail_c_set_mtx(SwMail_c* self)
ext 0220F2A0 void Bmsw_guest_SwMail_c_set_mtx_throw(SwMail_c* self)
ext 0220D7AC void Bmsw_guest_SwMail_c_DummyInit(SwMail_c* self)
ext 0220F3C4 void Bmsw_guest_SwMail_c_Dummy(SwMail_c* self)
ext 0220F3C8 void Bmsw_guest_SwMail_c_AppearInit(SwMail_c* self)
ext 0220F4C0 void Bmsw_guest_SwMail_c_Appear(SwMail_c* self)
ext 0220F4A0 void Bmsw_guest_SwMail_c_WaitInit(SwMail_c* self)
ext 0220F62C void Bmsw_guest_SwMail_c_Wait(SwMail_c* self)
ext 0220F744 void Bmsw_guest_SwMail_c_ThrowInit(SwMail_c* self, cXyz* a0, u8 a1)
ext 0220F7C4 void Bmsw_guest_SwMail_c_Throw(SwMail_c* self)
ext 0220F784 void Bmsw_guest_SwMail_c_EndInit(SwMail_c* self)
ext 0220FA84 void Bmsw_guest_SwMail_c_End(SwMail_c* self)
ext 0220EB68 void Bmsw_guest_SwMail_c_SeDelete(SwMail_c* self)
ext 0220F380 void Bmsw_guest_SwMail_c_move(SwMail_c* self)
ext 0220EFF4 void Bmsw_guest_SwMail_c_draw(SwMail_c* self, dKy_tevstr_c* a0)
ext 0220ED04 void Bmsw_guest_SwCam_c_Move(SwCam_c* self)
ext 0220D320 BOOL Bmsw_guest_daNpc_Bmsw_c_initTexPatternAnm(daNpc_Bmsw_c* self, u32 a0)
ext 0220EC00 void Bmsw_guest_daNpc_Bmsw_c_playTexPatternAnm(daNpc_Bmsw_c* self)
ext 0220FC70 void Bmsw_guest_daNpc_Bmsw_c_setAnm(daNpc_Bmsw_c* self, s8 a0)
ext 0220FD3C u8 Bmsw_guest_daNpc_Bmsw_c_chkAttention(daNpc_Bmsw_c* self, cXyz* a0, s16 a1)
ext 0220EE0C void Bmsw_guest_daNpc_Bmsw_c_eventOrder(daNpc_Bmsw_c* self)
ext 0220ECC4 void Bmsw_guest_daNpc_Bmsw_c_checkOrder(daNpc_Bmsw_c* self)
ext 0220FE68 u16 Bmsw_guest_daNpc_Bmsw_c_next_msgStatus(daNpc_Bmsw_c* self, be<u32>* a0)
ext 022102CC u32 Bmsw_guest_daNpc_Bmsw_c_getMsg(daNpc_Bmsw_c* self)
ext 022103D4 void Bmsw_guest_daNpc_Bmsw_c_anmAtr(daNpc_Bmsw_c* self, u16 a0)
ext 0220E6E8 BOOL Bmsw_guest_daNpc_Bmsw_c_CreateInit(daNpc_Bmsw_c* self)
ext 0220E340 void Bmsw_guest_daNpc_Bmsw_c_set_mtx(daNpc_Bmsw_c* self)
ext 0221047C void Bmsw_guest_daNpc_Bmsw_c_setAttention(daNpc_Bmsw_c* self)
ext 022104A4 void Bmsw_guest_daNpc_Bmsw_c_lookBack(daNpc_Bmsw_c* self)
ext 0221068C void Bmsw_guest_daNpc_Bmsw_c_wait01(daNpc_Bmsw_c* self)
ext 02210804 void Bmsw_guest_daNpc_Bmsw_c_talk01(daNpc_Bmsw_c* self)
ext 02210AC0 BOOL Bmsw_guest_daNpc_Bmsw_c_wait_action(daNpc_Bmsw_c* self, void* a0)
ext 02210DCC BOOL Bmsw_guest_daNpc_Bmsw_c_checkNextMailThrowOK(daNpc_Bmsw_c* self)
ext 02210E74 void Bmsw_guest_daNpc_Bmsw_c_setGameGetRupee(daNpc_Bmsw_c* self, s16 a0)
ext 02210D20 void Bmsw_guest_daNpc_Bmsw_c_TimerCountDown(daNpc_Bmsw_c* self)
ext 02210F60 BOOL Bmsw_guest_daNpc_Bmsw_c_shiwake_game_action(daNpc_Bmsw_c* self, void* a0)
ext 0220F06C BOOL Bmsw_guest_daNpc_Bmsw_c__draw(daNpc_Bmsw_c* self)
ext 0220EE44 BOOL Bmsw_guest_daNpc_Bmsw_c__execute(daNpc_Bmsw_c* self)
ext 0220EB70 BOOL Bmsw_guest_daNpc_Bmsw_c__delete(daNpc_Bmsw_c* self)
ext 0220E9D8 cPhs_State Bmsw_guest_daNpc_Bmsw_c__create(daNpc_Bmsw_c* self)
ext 0220D98C BOOL Bmsw_guest_daNpc_Bmsw_c_CreateHeap(daNpc_Bmsw_c* self)

str 100182DC "Demo10" in 02210AC0
*/
