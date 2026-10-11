/**
 * d_a_obj_mknjd.cpp (WWHD)
 * Object - Earth God's Lyric / Wind God's Aria statues (MknjD).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_mknjd.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_mknjd.h"

const gc_tu_data gc_tu = {.safeString = 0x1002C97C,
                          .safeStringAssure = 0x02374A68,
                          .headerStatics = {0x1046A5D4, 0x101CAF68, 0x1046A608}};
static const u32 l_actorVtable = 0x1002CC10;
static const u32 l_smokeCtor = 0x023726D8;
static const u32 l_smokeDtor = 0x02374A4C;
static Mtx34 M_tmp_mtx;
static u16 joint_number_table[20] = {};
static u32 l_currentModel = 0;
static Mtx34 l_currentMatrix;
static u32 l_messageManager = 0;
static s8 l_stayRoom = 0;
static u32 l_jointName[20] = {0x1002C934, 0x1002C93C, 0x1002C944, 0x1002C94C, 0x1002C954,
                              0x1002C95C, 0x1002C964, 0x1002C96C, 0x1002C974, 0x1002C8D8,
                              0x1002C8E0, 0x1002C8E8, 0x1002C8F0, 0x1002C8F8, 0x1002C900,
                              0x1002C908, 0x1002C910, 0x1002C918, 0x1002C920, 0x1002C928};
static u32 l_eventName[8] = {0x1002C9D4, 0x1002C9E4, 0x1002C994, 0x1002C9A4,
                             0x1002C9B4, 0x1002C9C4, 0x1002C9F4, 0x1002CA04};
static u32 l_cutName[9] = {0x1002CB9C, 0x1002CBA4, 0x1002CBB0, 0x1002CBB8, 0x1002CBC0,
                           0x1002CBC8, 0x1002CBD4, 0x1002CBE0, 0x1002CBE8};

enum {
    dRes_INDEX_MKNJD_BDL_MKNJD_e = 4,
    dRes_INDEX_MKNJD_BDL_MKNJH_e = 5,
    dRes_INDEX_MKNJD_BDL_MKNJK_e = 6,
    dRes_INDEX_MKNJD_DZB_MKNJD_e = 9,
};
enum {
    ACT_SETGOAL,
    ACT_SETANGLE,
    ACT_WAIT,
    ACT_INPUT,
    ACT_BREAK,
    ACT_HIDE_LINK,
    ACT_DISP_LINK,
    ACT_LESSON,
    ACT_TACT
};

using daObjMknjD::Act_c;
enum Item_e { ITEM_WIND_WAKER = 0x22, ITEM_EARTH_GODS_LYRIC = 0x70, ITEM_WIND_GODS_ARIA = 0x71 };
enum State_e {
    STATE_WAIT,
    STATE_START_TACT,
    STATE_CHECK_PARTNER,
    STATE_ORDER_ERROR,
    STATE_ERROR,
    STATE_WAIT_TACT_END,
    STATE_ORDER_DEMO,
    STATE_DEMO,
    STATE_WAIT_LESSON,
    STATE_START_LESSON,
    STATE_LESSON,
    STATE_HIDE_PLAYER,
    STATE_TALK
};
static inline const char *EventName(int i) {
    return Mknjd_view<const char>(l_eventName[i]);
}
static inline void eventInfo_setEventName(fopAc_ac_c *a, const char *name) {
    Mknjd_setEventName(&a->eventInfo, name);
}
static inline fopAc_ac_c *Mknjd_searchByName(be<s16> *slot, s16 name) {
    *slot = name;
    return fopAcIt_Judge(0x025E121C, slot);
}
static inline void fopAcM_onDraw(fopAc_ac_c *a) {
    s16 prio = Mknjd_drawPriority(a);
    Mknjd_toDrawQueue(&a->draw_tag, prio);
}
static inline void envLight_set10A4(u8 value) {
    Mknjd_view<MknjdEnvironment>(gabi::ea(dKy_getEnvlight()))->mDemoFlag = value;
}
static inline void player_onNoDraw(fopAc_ac_c *p) {
    auto *player = Mknjd_view<MknjdPlayer>(gabi::ea(p));
    player->mNoResetFlg0 = player->mNoResetFlg0 | 0x08000000u;
}
static inline void player_offNoDraw(fopAc_ac_c *p) {
    auto *player = Mknjd_view<MknjdPlayer>(gabi::ea(p));
    player->mNoResetFlg0 = player->mNoResetFlg0 & ~0x08000000u;
}
static inline u32 J3DModelData_getJointName(u32 d) {
    u32 h = Mknjd_getNameHeader(d);
    u32 off = Mknjd_view<MknjdNameHeader>(h)->mNameOffset;
    return off != 0 ? gabi::ea(&Mknjd_view<MknjdNameHeader>(h)->mNameOffset) + off : 0;
}
static inline u16 J3DModelData_getJointNum(u32 d) {
    return Mknjd_view<MknjdJointTree>(Mknjd_getJointTree(d))->mJointCount;
}
static inline u32 J3DModelData_getJointNode(u32 d, u32 i) {
    u32 n = Mknjd_view<MknjdModelData>(d)->mJointCount;
    u32 p = Mknjd_view<MknjdModelData>(d)->mJoints;
    if (i < n)
        p += i * sizeof(MknjdJoint);
    return p;
}
static inline u32 Mknjd_modelData(u32 m) {
    return Mknjd_view<MknjdModel>(m)->mModelData;
}
static inline u32 getAnmMtx(u32 model, u32 jointNo) {
    u32 block = Mknjd_view<MknjdModel>(model)->mMatrixBlock;
    auto *matrices = Mknjd_view<MknjdMatrixBlock>(block);
    matrices->mFlags = (u16)(matrices->mFlags | 0x10); // HD: mark matrix block dirty.
    return matrices->mMatrices + jointNo * sizeof(Mtx34);
}
static inline bool SafeString_eq(SafeString *a, SafeString *b) {
    gabi::call_ptr(Mknjd_view<MknjdSafeStringVtable>(a->__vtbl)->mAssureTermination, a);
    gabi::call_ptr(Mknjd_view<MknjdSafeStringVtable>(a->__vtbl)->mAssureTermination, a);
    u32 s1 = a->mStringTop;
    gabi::call_ptr(Mknjd_view<MknjdSafeStringVtable>(b->__vtbl)->mAssureTermination, b);
    if (s1 == b->mStringTop)
        return true;
    u32 p = a->mStringTop, q = b->mStringTop;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c = *Mknjd_view<be<u8>>(p + i);
        if (c != *Mknjd_view<be<u8>>(q + i))
            return false;
        if (c == 0)
            return true;
    }
    return false;
}
// HD: this comparison retains two SafeString stack temporaries and repeated termination calls.
static inline bool Mknjd_isStartStage(u32 literal) {
    gabi::Local<SafeString> a;
    a->mStringTop = literal;
    a->__vtbl = gc_tu.safeString;
    gabi::Local<SafeString> b;
    b->mStringTop = dComIfGp_ea() + 0x5134;
    b->__vtbl = gc_tu.safeString;
    return SafeString_eq(a, b);
}
static inline void smokeCB_remove(u32 cb) {
    gabi::call_ptr(Mknjd_view<MknjdSmokeVtable>(Mknjd_view<MknjdSmokeCallback>(cb)->mVtable)->mRemove, cb);
}

/* 02374AE0 */
static u32 PrmAbstract(fopAc_ac_c *a, s32 width, s32 shift) {
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
s32 Act_c::prm_get_swSave() {
    return PrmAbstract(this, PRM_SWITCH_W, PRM_SWITCH_S);
}
u8 Act_c::prm_get_Type() {
    return (u8)PrmAbstract(this, PRM_TYPE_W, PRM_TYPE_S);
}

/* 023722E4 */
static BOOL nodeCallBackL(J3DNode *i_node, int calcTiming) {
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint *joint = J3DNode_toJoint(i_node);
        u32 mdl = l_currentModel;
        Act_c *actor = Mknjd_view<Act_c>(Mknjd_view<MknjdModel>(mdl)->mUserArea);
        u32 jntNo = Mknjd_view<MknjdJoint>(gabi::ea(joint))->mJointNo;
        if (actor != nullptr) {
            PSMTXCopy(Mknjd_view<Mtx34>(getAnmMtx(mdl, jntNo)), calc_mtx());
            MtxTrans(actor->mLeftHalfPos.x, actor->mLeftHalfPos.y, actor->mLeftHalfPos.z, 1);
            mtx_copy(Mknjd_view<Mtx34>(getAnmMtx(mdl, jntNo)), calc_mtx());
            PSMTXCopy(calc_mtx(), &l_currentMatrix);
        }
    }
    return TRUE;
}

/* 02372410 */
static BOOL nodeCallBackR(J3DNode *i_node, int calcTiming) {
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint *joint = J3DNode_toJoint(i_node);
        u32 mdl = l_currentModel;
        Act_c *actor = Mknjd_view<Act_c>(Mknjd_view<MknjdModel>(mdl)->mUserArea);
        u32 jntNo = Mknjd_view<MknjdJoint>(gabi::ea(joint))->mJointNo;
        if (actor != nullptr) {
            PSMTXCopy(Mknjd_view<Mtx34>(getAnmMtx(mdl, jntNo)), calc_mtx());
            MtxTrans(actor->mRightHalfPos.x, actor->mRightHalfPos.y, actor->mRightHalfPos.z, 1);
            mtx_copy(Mknjd_view<Mtx34>(getAnmMtx(mdl, jntNo)), calc_mtx());
            PSMTXCopy(calc_mtx(), &l_currentMatrix);
        }
    }
    return TRUE;
}

/* 0237253C */
static BOOL nodeCallBack_Hahen(J3DNode *i_node, int calcTiming) {
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint *joint = J3DNode_toJoint(i_node);
        u32 jntNo = Mknjd_view<MknjdJoint>(gabi::ea(joint))->mJointNo;
        if (!((u32)(jntNo - 1) < 20)) /* HD: JUT_ASSERT(0xBF, (no - 1 >= 0) && (no - 1 < 20)) */
            JUT_ASSERT_fail("d_a_obj_mknjd.cpp", 0xBF, "(no - 1 >=0) && (no - 1 < 20)");
        u32 mdl = l_currentModel;
        Act_c *actor = Mknjd_view<Act_c>(Mknjd_view<MknjdModel>(mdl)->mUserArea);
        u16 shardIdx = joint_number_table[jntNo - 1];
        if (actor != nullptr) {
            PSMTXCopy(Mknjd_view<Mtx34>(getAnmMtx(mdl, jntNo)), calc_mtx());
            cXyz &p = actor->mShardPositions[shardIdx];
            MtxTrans(p.x, p.y, p.z, 1);
            mtx_copy(Mknjd_view<Mtx34>(getAnmMtx(mdl, jntNo)), calc_mtx());
            PSMTXCopy(calc_mtx(), &l_currentMatrix);
        }
    }
    return TRUE;
}

/* 02372688 */
s16 Act_c::XyCheckCB(int i_itemBtn) {
    return dComIfGp_getSelectItem(i_itemBtn) == ITEM_WIND_WAKER;
}

/* 023726C8 */
static s16 daObjMknjD_XyCheckCB(void *i_this, int i_itemBtn) {
    return static_cast<Act_c *>(i_this)->XyCheckCB(i_itemBtn);
}

/* 023726CC */
s16 Act_c::XyEventCB(int) {
    return mLessonEventIdx;
}

/* 023726D4 */
static s16 daObjMknjD_XyEventCB(void *i_this, int i_param) {
    return static_cast<Act_c *>(i_this)->XyEventCB(i_param);
}

/* 023726D8 */
static u32 smokeEcallBack_ct(void *p) {
    return Mknjd_smokeCtor(p, 1);
}

/* 023726E0 */
static Act_c *Act_c_ct(Act_c *p) {
    if (p == nullptr) {
        p = (Act_c *)operator_new(0x620);
        if (p == nullptr)
            return p;
    }
    dBgS_MoveBgActor::ct(p);
    p->__vtbl = l_actorVtable;
    Mknjd_constructArray(p->mSmokeCBs, 4, 0x20, l_smokeCtor); /* __construct_array */
    return p;
}

/* 0237274C */
cPhs_State Act_c::Mthd_Create() {
    /* fopAcM_ct(this, daObjMknjD::Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0)
            Act_c_ct(this);
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    m043E = prm_get_Type();
    if (fopAcM_isSwitch(this, prm_get_swSave())) {
        mEmitters[2] = nullptr;
        mEmitters[3] = nullptr;
        return cPhs_STOP_e;
    }
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, "MknjD");
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate("MknjD", dRes_INDEX_MKNJD_DZB_MKNJD_e, 0, 0x65A0);
        if (!((phase_state == cPhs_COMPLEATE_e) ||
              (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT (HD line 0x1EF) */
            JUT_ASSERT_fail("d_a_obj_mknjd.cpp", 0x1EF,
                            "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)");
    }
    return phase_state;
}

/* 02372884 */
BOOL Act_c::Mthd_Delete() {
    BOOL result = MoveBGDelete();
    if (mCreateResult != cPhs_STOP_e)      /* fpcM_CreateResult */
        dComIfG_resDelete(&mPhs, "MknjD"); /* dComIfG_resDeleteDemo */
    return result;
}

// HD: preserve the unsigned byte loop when matching relative joint names.
static inline s32 Mknjd_compareJointName(u32 a, u32 b) {
    for (;;) {
        u8 c = *Mknjd_view<be<u8>>(a++);
        u8 d = *Mknjd_view<be<u8>>(b++);
        if (c != d)
            return (s32)c - (s32)d;
        if (c == 0)
            return 0;
    }
}

// HD: half-joint lookup and bounds-checked shard table differ from GC.
/* 023728DC */
BOOL Act_c::CreateHeap() {
    J3DModelData *model_data_d;
    if (m043E == TYPE_WIND) {
        model_data_d =
            (J3DModelData *)dComIfG_getObjectRes("MknjD", dRes_INDEX_MKNJD_BDL_MKNJK_e, gc_tu.safeString);
    } else {
        model_data_d =
            (J3DModelData *)dComIfG_getObjectRes("MknjD", dRes_INDEX_MKNJD_BDL_MKNJD_e, gc_tu.safeString);
    }
    J3DModelData *model_data_h =
        (J3DModelData *)dComIfG_getObjectRes("MknjD", dRes_INDEX_MKNJD_BDL_MKNJH_e, gc_tu.safeString);
    if (model_data_d == nullptr) /* JUT_ASSERT(0x128, model_data_d != NULL) */
        JUT_ASSERT_fail("d_a_obj_mknjd.cpp", 0x128, "model_data_d != 0");
    if (model_data_h == nullptr) /* JUT_ASSERT(0x129, model_data_h != NULL) */
        JUT_ASSERT_fail("d_a_obj_mknjd.cpp", 0x129, "model_data_h != 0");

    mMainMdl = mDoExt_J3DModel__create(model_data_d, 0x80000, 0x31000002);
    mBreakMdl = mDoExt_J3DModel__create(model_data_h, 0x80000, 0x11000002);
    if (mMainMdl == nullptr || mBreakMdl == nullptr)
        return FALSE;

    s32 idx =
        JUTNameTab_getIndex(J3DModelData_getJointName(Mknjd_modelData(gabi::ea(mMainMdl.get()))), "MknjL");
    if (idx >= 0) {
        u32 node = J3DModelData_getJointNode(Mknjd_modelData(gabi::ea(mMainMdl.get())), (u16)idx);
        Mknjd_view<MknjdJoint>(node)->mCallback = gabi::guest_fn((const void *)nodeCallBackL);
    }
    idx = JUTNameTab_getIndex(J3DModelData_getJointName(Mknjd_modelData(gabi::ea(mMainMdl.get()))), "MknjR");
    if (idx >= 0) {
        u32 node = J3DModelData_getJointNode(Mknjd_modelData(gabi::ea(mMainMdl.get())), (u16)idx);
        Mknjd_view<MknjdJoint>(node)->mCallback = gabi::guest_fn((const void *)nodeCallBackR);
    }
    Mknjd_view<MknjdModel>(gabi::ea(mMainMdl.get()))->mUserArea = gabi::ea(this); /* setUserArea */

    s32 curTblIdx = 0;
    for (u16 i = 0; i < J3DModelData_getJointNum(Mknjd_modelData(gabi::ea(mBreakMdl.get()))); i++) {
        u32 node = J3DModelData_getJointNode(Mknjd_modelData(gabi::ea(mBreakMdl.get())), i);
        for (u16 j = 0; j < 20; j++) {
            u32 joint = gabi::ea(J3DNode_toJoint(Mknjd_view<J3DNode>(node)));
            u32 off = Mknjd_view<MknjdJoint>(joint)->mNameOffset;
            u32 jntName = off != 0 ? joint + off : 0; /* HD: the joint's own name */
            if (Mknjd_compareJointName(l_jointName[j], jntName) == 0) {
                u32 n2 = J3DModelData_getJointNode(Mknjd_modelData(gabi::ea(mBreakMdl.get())), i);
                Mknjd_view<MknjdJoint>(n2)->mCallback = gabi::guest_fn((const void *)nodeCallBack_Hahen);
                if (curTblIdx < 20)
                    joint_number_table[curTblIdx] = j;
                curTblIdx++;
                break;
            }
        }
    }
    Mknjd_view<MknjdModel>(gabi::ea(mBreakMdl.get()))->mUserArea = gabi::ea(this); /* setUserArea */
    mMainMdlAlpha = 0xFF;
    envLight_set10A4(0);
    return TRUE;
}

/* 02372C44 */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mMainMdl, mDoMtx_stack_c::get());
    J3DModel_setBaseTRMtx(mBreakMdl, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
}

/* 02372D7C */
void Act_c::init_mtx() {
    u32 m = gabi::ea(mMainMdl.get());
    Mknjd_view<MknjdModel>(m)->mBaseScale.x = 1.0f;
    Mknjd_view<MknjdModel>(m)->mBaseScale.y = 1.0f;
    Mknjd_view<MknjdModel>(m)->mBaseScale.z = 1.0f;
    m = gabi::ea(mBreakMdl.get());
    Mknjd_view<MknjdModel>(m)->mBaseScale.x = 1.0f;
    Mknjd_view<MknjdModel>(m)->mBaseScale.y = 1.0f;
    Mknjd_view<MknjdModel>(m)->mBaseScale.z = 1.0f;
    set_mtx();
}

/* 02372DA8 */
BOOL Act_c::Create() {
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mMainMdl)); /* fopAcM_SetMtx */
    init_mtx();
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mBreakMdl));
    init_mtx();
    fopAcM_setCullSizeBox(this, -400.0f, -1.0f, -400.0f, 400.0f, 405.0f, 400.0f);
    mLeftHalfPos.x = 0.0f;
    mLeftHalfPos.y = 0.0f;
    mLeftHalfPos.z = 0.0f;
    mRightHalfPos.x = 0.0f;
    mRightHalfPos.y = 0.0f;
    mRightHalfPos.z = 0.0f;
    for (int i = 0; i < 20; i++) {
        mShardPositions[i].x = 0.0f;
        mShardPositions[i].y = 0.0f;
        mShardPositions[i].z = 0.0f;
        mShardHeights[i] = 0.0f;
    }
    m043D = false;
    mBreakTimer = 0;
    for (int i = 0; i < 4; i++)
        mEmitters[i] = nullptr;

    if (m043E == TYPE_WIND) {
        mCheckEventIdx = dComIfGp_evmng_getEventIdx(EventName(3), 0xFF);
        mDemoEventIdx = dComIfGp_evmng_getEventIdx(EventName(1), 0xFF);
        mErrorEventIdx = dComIfGp_evmng_getEventIdx(EventName(5), 0xFF);
        mLessonEventIdx = dComIfGp_evmng_getEventIdx(EventName(7), 0xFF);
        mMelodyNum = 4;
        mGiveItemNo = ITEM_WIND_GODS_ARIA;
        eventInfo_setEventName(this, "MKNJD_K_TALK");
        m0430 = 0x2910;
    } else {
        mCheckEventIdx = dComIfGp_evmng_getEventIdx(EventName(2), 0xFF);
        mDemoEventIdx = dComIfGp_evmng_getEventIdx(EventName(0), 0xFF);
        mErrorEventIdx = dComIfGp_evmng_getEventIdx(EventName(4), 0xFF);
        mLessonEventIdx = dComIfGp_evmng_getEventIdx(EventName(6), 0xFF);
        mMelodyNum = 3;
        mGiveItemNo = ITEM_EARTH_GODS_LYRIC;
        eventInfo_setEventName(this, "MKNJD_D_TALK");
        m0430 = 0x2920;
    }
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0x3D;  /* attention_info.distances[TALK] */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0x3D; /* attention_info.distances[SPEAK] */
    attention_info.flags = attention_info.flags | 0x20000008;

    if (!checkItemGet(mGiveItemNo, TRUE)) {
        m043F = STATE_WAIT_LESSON;
        eventInfo.mpEventCB =
            gabi::guest_fn((const void *)daObjMknjD_XyEventCB); /* HD eventInfo: event callback at +8 */
        eventInfo.mpCheckCB = gabi::guest_fn((const void *)daObjMknjD_XyCheckCB); /* check callback at +0xC */
    } else {
        m043F = STATE_WAIT;
    }
    mMsgFound = 0;
    mMsgPID = -1;
    m0504 = false;
    return TRUE;
}

/* 023730B4 */
static void manage_friend_draw(int i_param1) {
    gabi::Local<be<s16>[2]> names; /* two adjacent stack temporaries */
    fopAc_ac_c *judgeResult = Mknjd_searchByName(&(*names)[0], fpcNm_NPC_MD_e);
    if (judgeResult != nullptr) {
        if (i_param1 == 1) {
            fopAcM_onDraw(judgeResult);
            u32 p = Mknjd_view<MknjdMedli>(gabi::ea(judgeResult))->mEmitter;
            if (p != 0)
                Mknjd_view<MknjdEmitter>(p)->mFlags = Mknjd_view<MknjdEmitter>(p)->mFlags & ~4u;
        } else {
            fopAcM_offDraw(judgeResult);
            u32 p = Mknjd_view<MknjdMedli>(gabi::ea(judgeResult))->mEmitter;
            if (p != 0)
                Mknjd_view<MknjdEmitter>(p)->mFlags = Mknjd_view<MknjdEmitter>(p)->mFlags | 4u;
        }
    }
    judgeResult = Mknjd_searchByName(&(*names)[1], fpcNm_NPC_CB1_e);
    if (judgeResult != nullptr) {
        if (i_param1 == 1)
            fopAcM_onDraw(judgeResult);
        else
            fopAcM_offDraw(judgeResult);
    }
}

/* 023731D8 */
void Act_c::setGoal(int i_staffIdx) {
    cXyz *p = (cXyz *)dComIfGp_evmng_getMySubstanceP(i_staffIdx, "Posion", 1);
    f32 px = p->x, py = p->y, pz = p->z;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), current.angle.y);
    mDoMtx_stack_c::transM(px, py, pz);
    u32 m = gabi::ea(mDoMtx_stack_c::get());
    mGoalPos.x = Mknjd_view<Mtx34>(m)->m[0][3]; /* multVecZero */
    mGoalPos.y = Mknjd_view<Mtx34>(m)->m[1][3];
    mGoalPos.z = Mknjd_view<Mtx34>(m)->m[2][3];
    dComIfGp_evmng_setGoal(&mGoalPos);
}

/* 023732D8 */
void Act_c::setPlayerAngle(int i_staffIdx) {
    void *p = dComIfGp_evmng_getMySubstanceP(i_staffIdx, "angle", 3);
    s16 angle = 0;
    if (p != nullptr)
        angle = Mknjd_view<MknjdInteger>(gabi::ea(p))->mLowHalf;
    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    s16 a = (s16)(current.angle.y + angle);
    /* daPy_py_c::setPlayerPosAndAngle(cXyz*, s16) (virtual +0x114) */
    gabi::call_ptr(Mknjd_view<MknjdPlayerVtable>(player->__vtbl)->mSetPlayerPosAndAngle, player,
                   &player->current.pos, a);
}

/* 02373360 */
u16 Act_c::talk(int i_param1) {
    u16 msgStatus = 0xFF;
    u32 mgr = l_messageManager;
    if (mMsgPID == -1) {
        if (i_param1 == 1) {
            /* getMsg() */
            if (m0500 == 0) {
                u8 melody = mMelodyNum;
                dComIfGp_setMelodyNum(melody); /* dComIfGp_setMelodyNum */
                mMsgNo = 0x5B3;
            } else {
                mMsgNo = 0x1901;
            }
        }
        mMsgPID = msgMng_messageSet(mgr, mMsgNo, &eyePos);
        if (mMsgPID != -1)
            mMsgFound = 0;
    } else if (mMsgFound != 0) {
        msgStatus = (u16)msgMng_getStatus(mgr);
        if (msgStatus == fopMsgStts_MSG_DISPLAYED_e) {
            if (dComIfGp_checkMesgCancelButton() != 0) { /* dComIfGp_checkMesgCancelButton */
                m0504 = true;
                Mknjd_view<MknjdMessage>(mgr)->mSend = 1; /* fopMsgM_messageSendOn */
            }
            msgMng_setStatus(mgr, fopMsgStts_MSG_ENDS_e);
        } else if (msgStatus == fopMsgStts_BOX_CLOSED_e) {
            msgMng_setStatus(mgr, fopMsgStts_MSG_DESTROYED_e);
            mMsgPID = -1;
        }
    } else {
        mMsgFound = 1;
    }
    return msgStatus;
}

/* 023734B4 */
bool Act_c::daObjMknjD_break() {
    bool ret = false;
    mBreakTimer = mBreakTimer + 1;
    if (mBreakTimer < 60) {
        mLeftHalfPos.x = -(f32)((mBreakTimer * 20.0) / 60.0);
        mRightHalfPos.x = (f32)((mBreakTimer * 20.0) / 60.0);
    } else {
        mLeftHalfPos.x = -20.0f;
        mRightHalfPos.x = 20.0f;
    }

    if (mBreakTimer == 1) {
        mEmitters[0] = dComIfGp_particle_set(0x8185 /* ID_AK_SN_WGFLASH00 */, &current.pos, &current.angle);
        gabi::Local<GXColor> color;
        color->r = (u8)tevStr.mColorC0.r;
        color->g = (u8)tevStr.mColorC0.g;
        color->b = (u8)tevStr.mColorC0.b;
        color->a = (u8)tevStr.mColorC0.a;
        s8 roomNo = current.roomNo;
        dPa_control_c *pa = dComIfGp_getParticle();
        mEmitters[1] =
            dPa_control_set(pa, 4, 0x8186 /* ID_AK_SN_WGROCK00 */, &current.pos, &current.angle, nullptr,
                            0xFF, nullptr, roomNo, &tevStr.mColorK0 /* tevStr.mColorK0 */, color, nullptr);
        pa = dComIfGp_getParticle();
        mEmitters[2] =
            dPa_control_set(pa, 2, 0xA187 /* ID_AK_ST_WGSMOKE00 */, &current.pos, &current.angle, nullptr,
                            0xFF, Mknjd_view<dPa_levelEcallBack>(smokeCB(2)), -1, nullptr, nullptr, nullptr);
        mSmokeCBs[2].mRateOff = 0; /* setRateOff(0) */
        mDoAud_seStart(0x69A8 /* JA_SE_OBJ_SAGE_GATE_CREAK */, &current.pos, 0,
                       dComIfGp_getReverb(current.roomNo));
    } else if (mBreakTimer == 30) {
        mDoAud_seStart(0x69A9 /* JA_SE_OBJ_SAGE_GATE_LIGHT */, &current.pos, 0,
                       dComIfGp_getReverb(current.roomNo));
    } else if (mBreakTimer == 60) {
        m043D = true;
        for (u32 i = 0; i < 20; i++) {
            mShardPositions[i].y = 0.0f;
            mShardPositions[i].x = 0.0f;
            mShardPositions[i].z = 0.0f;
            if (i == 0x02 || i == 0x03 || i == 0x06 || i == 0x07 || i == 0x0A || i == 0x0D || i == 0x0E ||
                i == 0x12 || i == 0x13) {
                mShardPositions[i].x = mShardPositions[i].x + 20.0f;
            } else {
                mShardPositions[i].x = mShardPositions[i].x - 20.0f;
            }
        }
    } else if (mBreakTimer == 160) {
        gabi::Local<cXyz> v;
        v->x = 0.0f;
        v->y = 1.0f;
        v->z = 0.0f;
        dComIfGp_getVibration_StartShock(6, -0x21, v);
        mDoAud_seStart(0x69AA /* JA_SE_OBJ_SAGE_GATE_BREAK */, &current.pos, 0,
                       dComIfGp_getReverb(current.roomNo));
        f32 y = current.pos.y;
        mBrokenPos.y = y + 350.0f;
        mBrokenPos.x = current.pos.x;
        mBrokenPos.z = current.pos.z;
        dPa_control_c *pa = dComIfGp_getParticle();
        mEmitters[3] =
            dPa_control_set(pa, 2, 0x2027 /* ID_AK_JT_ELEMENTSMOKE01 */, &mBrokenPos, &current.angle, nullptr,
                            0xFF, Mknjd_view<dPa_levelEcallBack>(smokeCB(3)), -1, nullptr, nullptr, nullptr);
        if (mEmitters[3] != nullptr) {
            Mknjd_view<MknjdEmitter>(gabi::ea(mEmitters[3].get()))->mVolumeSweep = 0.5f; /* setVolumeSweep */
            Mknjd_view<MknjdEmitter>(gabi::ea(mEmitters[3].get()))->mLifeTime = 0x2D;    /* setLifeTime */
            Mknjd_view<MknjdEmitter>(gabi::ea(mEmitters[3].get()))->mRate = 50.0f;       /* setRate */
            Mknjd_view<MknjdEmitter>(gabi::ea(mEmitters[3].get()))->mMaxFrame = 1;       /* setMaxFrame */
            u32 e = gabi::ea(mEmitters[3].get());
            Mknjd_view<MknjdEmitter>(e)->mDynamicsScale.x = 3.0f; /* setGlobalDynamicsScale */
            Mknjd_view<MknjdEmitter>(e)->mDynamicsScale.y = 3.0f;
            Mknjd_view<MknjdEmitter>(e)->mDynamicsScale.z = 3.0f;
            e = gabi::ea(mEmitters[3].get());
            Mknjd_view<MknjdEmitter>(e)->mParticleScale.x = 6.0f; /* setGlobalParticleScale */
            Mknjd_view<MknjdEmitter>(e)->mParticleScale.y = 6.0f;
            Mknjd_view<MknjdEmitter>(e)->mParticleScale.z = 6.0f;
        }
    } else if (mBreakTimer == 255) {
        ret = true;
    }

    if (mBreakTimer < 60) {
        mMainMdlAlpha = 0xFF;
    } else if (mBreakTimer >= 60 && mBreakTimer < 160) {
        f64 mdlAlpha = ((0xA0 - mBreakTimer) * 255.0) / 100.0;
        if (mdlAlpha < 255.0) {
            mMainMdlAlpha = (u8)gabi::ftoi(mdlAlpha);
        } else {
            mMainMdlAlpha = 0xFF;
        }
    } else {
        mMainMdlAlpha = 0;
    }

    if (mBreakTimer >= 160) {
        int i = 19 - (mBreakTimer - 160);
        if (i < 0)
            i = 0;
        for (; i < 20; i++) {
            mShardHeights[i] = mShardHeights[i] - 2.0f;
            mShardPositions[i].y = mShardPositions[i].y + mShardHeights[i];
        }
    }
    return ret;
}

/* 02373A30 */
void Act_c::privateCut() {
    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    s32 staffIdx = dComIfGp_evmng_getMyStaffId("MknjD", nullptr, 0);
    if (staffIdx == -1)
        return;
    mActionIdx = dComIfGp_evmng_getMyActIdx(staffIdx, reinterpret_cast<gptr<char> *>(l_cutName), 9, TRUE, 0);
    if (mActionIdx == -1) {
        dComIfGp_evmng_cutEnd(staffIdx);
        return;
    }
    bool doCutEnd = false;
    if (dComIfGp_evmng_getIsAddvance(staffIdx)) {
        switch (mActionIdx) {
        case ACT_SETGOAL:
            setGoal(staffIdx);
            break;
        case ACT_SETANGLE:
            setPlayerAngle(staffIdx);
            break;
        case ACT_BREAK: {
            Mknjd_seStart(0x806 /* JA_SE_READ_RIDDLE_1 */);
            s32 sw = prm_get_swSave();
            dComIfGs_onSwitch(sw, home.roomNo); /* fopAcM_onSwitch */
            dBgW *bgw = mpBgW;
            mBreakTimer = 0;
            if (bgw != nullptr && dBgW_ChkUsed(bgw)) {
                dBgS *bgs = dComIfG_Bgsp();
                cBgS_Release(bgs, mpBgW);
            }
            break;
        }
        case ACT_HIDE_LINK:
            player_onNoDraw(player);
            break;
        case ACT_DISP_LINK:
            player_offNoDraw(player);
            break;
        case ACT_LESSON:
            m0504 = false;
            m0500 = 0;
            mMsgPID = -1;
            break;
        }
    }
    switch (mActionIdx) {
    case ACT_INPUT:
        if (talk(1) == fopMsgStts_BOX_CLOSED_e)
            doCutEnd = true;
        break;
    case ACT_BREAK:
        if (daObjMknjD_break() == true) {
            if (Mknjd_isStartStage(gabi::guest_addr("Ekaze")) ||
                Mknjd_isStartStage(gabi::guest_addr("Edaichi"))) {
                mDoAud_bgmStart(0x80000057 /* JA_BGM_JABOO_CAVE */);
            } else if (m043E == TYPE_WIND) {
                mDoAud_bgmStart(0x8000002F /* JA_BGM_D_WIND */);
            } else {
                mDoAud_bgmStart(0x8000002D /* JA_BGM_D_EARTH */);
            }
            if (mSmokeCBs[2].mEmitter != 0) /* getEmitter() */
                smokeCB_remove(smokeCB(2));
            if (mSmokeCBs[3].mEmitter != 0)
                smokeCB_remove(smokeCB(3));
            doCutEnd = true;
        }
        break;
    case ACT_LESSON: {
        u16 msgStatus = talk(1);
        if (msgStatus == fopMsgStts_BOX_CLOSED_e || msgStatus == fopMsgStts_INPUT_e)
            doCutEnd = true;
        break;
    }
    case ACT_TACT:
        if (m0504 == false)
            doCutEnd = true;
        else
            dComIfGp_event_reset();
        break;
    default:
        doCutEnd = true;
        break;
    }
    if (doCutEnd)
        dComIfGp_evmng_cutEnd(staffIdx);
}

/* 02373EA8 */
BOOL Act_c::Execute(Mtx34 **i_mtx) {
    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    fopAc_ac_c *partner = dComIfGp_getCb1Player();

    switch ((u8)m043F) {
    case STATE_WAIT:
        eventInfo.onCondition(dEvtCnd_CANTALK_e);
        if (eventInfo.checkCommandTalk()) {
            m0500 = 1;
            m043F = STATE_HIDE_PLAYER;
        } else if (player != nullptr) {
            gabi::Local<cXyz> partnerDiff;
            cXyz_mi(&current.pos, partnerDiff, &player->current.pos);
            s16 rotDiff = (s16)(cM_atan2s(partnerDiff->x, partnerDiff->z) - current.angle.y);
            gabi::Local<cXyz> xz;
            xz->y = 0.0f;
            xz->x = partnerDiff->x;
            xz->z = partnerDiff->z;
            f32 absXZ = std_sqrtf(PSVECSquareMag(xz));
            if (absXZ < 800.0f && (rotDiff < -0x4000 || rotDiff > 0x4000)) {
                if (dComIfGp_checkPlayerStatus1(0, 1 /* daPyStts1_WIND_WAKER_CONDUCT_e */)) {
                    m043F = STATE_START_TACT;
                    manage_friend_draw(0);
                }
            }
        }
        break;
    case STATE_START_TACT: {
        /* daPy_py_c::setTactZev(procId, melody, eventName) (virtual +0x84) */
        u32 fn = Mknjd_view<MknjdPlayerVtable>(player->__vtbl)->mSetTactZev;
        u32 id = mBsPcId;
        if (m043E == TYPE_WIND)
            gabi::call_ptr(fn, player, id, 4, EventName(3));
        else
            gabi::call_ptr(fn, player, id, 3, EventName(2));
        m043F = STATE_CHECK_PARTNER;
        break;
    }
    case STATE_CHECK_PARTNER:
        if (dComIfGp_evmng_startCheck(mCheckEventIdx) != 0) {
            if (partner != nullptr && player != nullptr) {
                s16 rotDiff = (s16)(cM_atan2s(current.pos.x - partner->current.pos.x,
                                              current.pos.z - partner->current.pos.z) -
                                    current.angle.y);
                gabi::Local<cXyz> diff;
                cXyz_mi(&player->current.pos, diff, &partner->current.pos);
                gabi::Local<cXyz> xz;
                xz->x = diff->x;
                xz->y = 0.0f;
                xz->z = diff->z;
                f32 absXZ = std_sqrtf(PSVECSquareMag(xz));
                if (absXZ < 800.0f && (rotDiff < -0x4000 || rotDiff > 0x4000)) {
                    Mknjd_orderChangeEventId(this, mDemoEventIdx, 0, 0xFFFF);
                    dComIfGs_onEventBit(m0430);
                    gabi::Local<be<s16>> name;
                    fopAc_ac_c *judgeResult = Mknjd_searchByName(name, fpcNm_NPC_MD_e);
                    if (judgeResult != nullptr)
                        dComIfGp_event_setTalkPartner(judgeResult);
                    m043F = STATE_ORDER_DEMO;
                } else {
                    Mknjd_orderChangeEventId(this, mErrorEventIdx, 0, 0xFFFF);
                    m043F = STATE_ORDER_ERROR;
                    manage_friend_draw(1);
                }
            } else {
                Mknjd_orderChangeEventId(this, mErrorEventIdx, 0, 0xFFFF);
                m043F = STATE_ORDER_ERROR;
                manage_friend_draw(1);
            }
        } else if (!dComIfGp_checkPlayerStatus1(0, 1)) {
            m043F = STATE_WAIT;
            manage_friend_draw(1);
        }
        break;
    case STATE_ORDER_ERROR:
        if (eventInfo_checkCommandDemoAccrpt(this))
            m043F = STATE_ERROR;
        break;
    case STATE_ERROR:
        if (dComIfGp_evmng_endCheck(mErrorEventIdx)) {
            dComIfGp_event_reset();
            m043F = STATE_WAIT_TACT_END;
        }
        break;
    case STATE_WAIT_TACT_END:
        if (!dComIfGp_checkPlayerStatus1(0, 1))
            m043F = STATE_WAIT;
        break;
    case STATE_ORDER_DEMO:
        if (eventInfo_checkCommandDemoAccrpt(this)) {
            m043F = STATE_DEMO;
            /* HD: the Wind statue's demo in "kaze" room 12 sets an env-light flag */
            bool kaze = false;
            if (Mknjd_isStartStage(gabi::guest_addr("kaze")) && l_stayRoom == 0xC)
                kaze = true;
            if (kaze)
                envLight_set10A4(1);
            mDoAud_bgmStop(30);
            mDoAud_taktModeMuteOff();
            dComIfGp_att_offAleart();
            if (m043E == TYPE_WIND) {
                m0432 = 0x2B;
                m0434 = 5;
            } else {
                m0432 = 0x0E;
                m0434 = 5;
            }
        }
        break;
    case STATE_DEMO:
        privateCut();
        if (dComIfGp_evmng_endCheck(mDemoEventIdx)) {
            dComIfGp_att_revivalAleart();
            dComIfGp_event_reset();
            envLight_set10A4(0);
            fopAcM_delete(this);
        }
        break;
    case STATE_WAIT_LESSON:
        eventInfo.onCondition(dEvtCnd_CANTALK_e | dEvtCnd_CANTALKITEM_e);
        if (eventInfo.checkCommandTalk()) {
            if (dComIfGp_event_chkTalkXY()) {
                m0500 = 0;
                m043F = STATE_START_LESSON;
                mDoAud_seStart(0x8A7 /* JA_SE_PRE_TAKT */, &eyePos, 0,
                               dComIfGp_getReverb(current.roomNo)); /* fopAcM_seStart */
            } else {
                m0500 = 1;
                m043F = STATE_HIDE_PLAYER;
            }
        }
        break;
    case STATE_START_LESSON:
        m043F = STATE_LESSON;
        break;
    case STATE_LESSON:
        privateCut();
        if (!dComIfGp_event_runCheck()) {
            if (checkItemGet(mGiveItemNo, TRUE))
                m043F = STATE_WAIT;
            else
                m043F = STATE_WAIT_LESSON;
        }
        break;
    case STATE_HIDE_PLAYER:
        player_onNoDraw(player);
        m043F = STATE_TALK;
        break;
    case STATE_TALK:
        if (talk(1) == fopMsgStts_BOX_CLOSED_e) {
            player_offNoDraw(player);
            dComIfGp_event_reset();
            if (checkItemGet(mGiveItemNo, TRUE))
                m043F = STATE_WAIT;
            else
                m043F = STATE_WAIT_LESSON;
        }
        break;
    }

    set_mtx();
    *Mknjd_view<be<u32>>(gabi::ea(i_mtx)) = gabi::ea(&M_tmp_mtx);

    if (m0432 > 1)
        m0432 = (u16)(m0432 - 1);
    if (m0432 == 1) {
        if (m043E == TYPE_WIND)
            mDoAud_bgmStart(0x80000021 /* JA_BGM_TAKT_MAKORE */);
        else
            mDoAud_bgmStart(0x80000022 /* JA_BGM_TAKT_MEDRI */);
        m0432 = 0;
    }
    if (m0434 > 1)
        m0434 = (u16)(m0434 - 1);
    if (m0434 == 1) {
        manage_friend_draw(1);
        m0434 = 0;
    }
    return TRUE;
}

/* 023747BC */
BOOL Act_c::Draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    dScnKy_env_light_c *env = dKy_getEnvlight();
    setLightTevColorType(env, mBreakMdl, &tevStr);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, mMainMdl, &tevStr);
    dComIfGd_setListBG();

    u32 mdlData = Mknjd_modelData(gabi::ea(mMainMdl.get()));
    u16 jointCount = J3DModelData_getJointNum(mdlData);
    for (u16 i = 0; i < jointCount; i++) {
        u32 node = J3DModelData_getJointNode(mdlData, i);
        u32 mat = Mknjd_view<MknjdJoint>(node)->mMesh; /* getMesh() */
        u8 alpha = mMainMdlAlpha;
        for (; mat != 0; mat = Mknjd_view<MknjdMaterial>(mat)->mNext) /* getNext() */
            Mknjd_view<MknjdShape>(Mknjd_view<MknjdMaterial>(mat)->mShape)->mVisible =
                alpha != 0; /* getShape()->show()/hide() */
    }
    mDoExt_modelUpdateDL(mMainMdl);
    if (m043D)
        mDoExt_modelUpdateDL(mBreakMdl);
    dComIfGd_setList();
    return TRUE;
}

/* 02374910 */
BOOL Act_c::Delete() {
    dComIfGp_att_revivalAleart();
    for (int i = 0; i < 4; i++)
        smokeCB_remove(smokeCB(i));
    return TRUE;
}

// HD: method table entries are tail branches.
/* 02374978 */
static cPhs_State Mthd_Create(void *i_this) {
    return ((Act_c *)i_this)->Mthd_Create();
}
/* 0237497C */
static BOOL Mthd_Delete(void *i_this) {
    return ((Act_c *)i_this)->Mthd_Delete();
}
/* 02374980 */
static BOOL Mthd_Execute(void *i_this) {
    return ((Act_c *)i_this)->MoveBGExecute();
}
/* 02374984 */
static BOOL Mthd_Draw(void *i_this) {
    return ((Act_c *)i_this)->Draw_v();
}
/* 02374994 */
static BOOL Mthd_IsDelete(void *i_this) {
    return ((Act_c *)i_this)->IsDelete_v();
}

// HD: compiler-generated and template code of this translation unit.
/* 023749A4 */
static void __sinit_d_a_obj_mknjd_cpp() {
    sinit_header_statics_z(gc_tu.headerStatics[0], gc_tu.headerStatics[1], gc_tu.headerStatics[2]);
}

/* 02374A38 */
static void SafeString_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02374A4C */
static void smokeEcallBack_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02374A60 */
static BOOL MoveBgActor_IsDelete(Act_c *i_this) {
    return TRUE;
}

/* 02374A68 */
static void SafeString_assureTermination(void *p) {}

/* 02374A6C */
static void Act_c_dt(Act_c *i_this, s32 flags) {
    if (i_this != nullptr) {
        Mknjd_destroyArray(i_this->mSmokeCBs, 4, 0x20, l_smokeDtor, 0, 0); /* __destroy_arr */
        Mknjd_actorDtor(i_this, 0);                                        /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue
str 1002CA14 "d_a_obj_mknjd.cpp" in 0237253C
str 1002CA28 "(no - 1 >=0) && (no - 1 < 20)"
str 1002CA4C "d_a_obj_mknjd.cpp" in 0237274C
str 1002CA60 "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)"
str 1002CAB4 "d_a_obj_mknjd.cpp" in 023728DC
str 1002CAC8 "model_data_d != 0"
str 1002CADC "model_data_h != 0"
str 1002CAA4 "MknjL"
str 1002CAAC "MknjR"
str 1002CB04 "MKNJD_K_TALK"
str 1002CB14 "MKNJD_D_TALK"
str 1002CB24 "Posion"
str 1002CB2C "angle"
str 1002CB8C "MknjD" in 02373A30
str 1002CB94 "Ekaze"
str 1002CB84 "Edaichi"
str 1002CBF4 "kaze"
str 1002CC08 "MknjD" in 0237274C
str 1002CC08 "MknjD" in 02372884
str 1002CC08 "MknjD" in 023728DC
data M_tmp_mtx 1046A618 nocheck
data joint_number_table 1046A5E0 nocheck
data l_currentModel 104B462C nocheck
data l_currentMatrix 104B4868 nocheck
data l_messageManager 101F4B5C nocheck
data l_stayRoom 1047E6C8 nocheck
data l_jointName 101CAEF4
data l_eventName 101CAF8C
data l_cutName 101CAF44
ext 025D7874 void Mknjd_orderChangeEventId(fopAc_ac_c*, s16, u16, u16)
ext 0253E9B0 void Mknjd_setEventName(void*, const char*)
ext 025E18EC void mDoAud_bgmStart(u32)
ext 025E1904 void mDoAud_bgmStop(u32)
ext 025E1E88 void mDoAud_taktModeMuteOff()
ext 025E1988 void Mknjd_seStart(u32)
ext 025DF2B8 s16 Mknjd_drawPriority(fopAc_ac_c*)
ext 025DA874 void Mknjd_toDrawQueue(void*, s16)
ext 025F795C u32 msgMng_getStatus(u32)
ext 025F74D0 void msgMng_setStatus(u32, u32)
ext 025F7DB0 u32 msgMng_messageSet(u32, u32, cXyz*)
ext 027F68FC u32 Mknjd_getNameHeader(u32)
ext 027DF9B0 s32 JUTNameTab_getIndex(u32, const char*)
ext 027F3F94 u32 Mknjd_getJointTree(u32)
ext 025A5B18 u32 Mknjd_smokeCtor(void*, s32)
ext 028EFFD0 void Mknjd_constructArray(void*, s32, s32, u32)
ext 028F0164 void Mknjd_destroyArray(void*, s32, s32, u32, s32, s32)
ext 025D50BC void Mknjd_actorDtor(void*, s32)
*/
