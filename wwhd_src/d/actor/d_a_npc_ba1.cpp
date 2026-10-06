/**
 * d_a_npc_ba1.cpp (WWHD)
 * NPC - Link's Grandma
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_ba1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_ba1.h"

#define SAFESTRING_VTBL 0x10016888 /* this TU's sead::SafeString vtable */
#define BA1_VTBL 0x10016B18        /* daNpc_Ba1_c vtable (HD: merged with fopNpc_npc_c's) */

enum { fpcNm_NPC_FA1_e = 0x168 };
enum { dItemNo_HALF_SOUP_BOTTLE_e = 0x54, dItemNo_SOUP_BOTTLE_e = 0x55, dItemNo_FAIRY_BOTTLE_e = 0x57 };
enum {
    dSv_event_flag_UNK_0001 = 0x0001,
    dSv_event_flag_UNK_0520 = 0x0520,
    dSv_event_flag_UNK_0601 = 0x0601,
    dSv_event_flag_UNK_0602 = 0x0602,
    dSv_event_flag_UNK_0608 = 0x0608,
    dSv_event_flag_UNK_0740 = 0x0740,
    dSv_event_flag_UNK_0780 = 0x0780,
    dSv_event_flag_UNK_0E20 = 0x0E20,
    dSv_event_flag_UNK_2A80 = 0x2A80,
    dSv_event_flag_GRANDMA_HEALED = 0x2A20,
    dSv_event_flag_UNK_3202 = 0x3202,
};
enum { dRes_ID_BA_BDL_BA_e = 0x1A, dRes_ID_BA_BCK_WAIT01_e = 0x0, dRes_ID_BA_BDL_BA_CLOTH_e = 0x9 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* save info: the event flags (dSv_event_c) are at *(0x101F84DC) + 0x644 (save info + 0x624).
 * The pointer is re-read at every use. */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
/* dComIfGs_checkCollect(i): HD save info byte +0xD4 + i (read through the save pointer) */
static inline u8 dComIfGs_checkCollect(int i) { return gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0xD4 + i); }
/* play object fields (dComIfGp_get() at each use) */
static inline u8 dComIfGp_getSelectItem(int btn) { return gabi::load<u8>(dComIfGp_ea() + btn + 0x5BBB); }
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
static inline u8 dComIfGp_event_getTalkXYBtn() { return gabi::load<u8>(dComIfGp_ea() + 0x52B0); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(dComIfGp_event_getTalkXYBtn() - 1) <= 3; }
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925); }
/* resources by id (HD: sead::SafeString key, per-TU vtable) */
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->__vtbl = SAFESTRING_VTBL;
    key->mStringTop = gabi::ea(arc);
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
/* J3DAnmTexPattern::getFrameMax: virtual (vtable pointer at +4, slot +0x14) */
static inline s32 J3DAnm_getFrameMax(J3DAnmTexPattern* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
/* 02055B64 cLib_calcTimer<s16> (out of line) */
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
/* 025E789C mDoExt_btpAnm::init(modelData, anm, anmPlay, attr, rate, start, end, modify, entry) */
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate,
                                     s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
/* J3DModel (HD): model data at +0xAC (the shared J3DModel_getModelData uses +0x4: wrong) */
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
/* dAttention_c (play + 0x5804). HD names: 024EC8D0 is LockonTarget, 024EE464 ActionTarget (the
 * matcher has them swapped: 024EC8D0 reads the list at +0x54 with mLockonCount) */
static inline bool dAttention_LockonTruth(dAttention_c* a) { return gabi::call<bool>(0x024EDFCC, a); }
static inline fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i); }
static inline fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EE464, a, i); }

/* event manager (play + 0x52C4) */
static inline void* dComIfGp_evmng_getMyIntegerP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 3); }
static inline void* dComIfGp_evmng_getMyFloatP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 0); }
static inline void dComIfGp_evmng_setGoal(cXyz* pos) { gabi::call(0x02543714, dComIfGp_getPEvtManager(), pos); }
static inline s8 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return gabi::call<s8>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
/* 0252012C dComIfGp_setNextStage(stage, point, roomNo, layer, lastSpeed, lastMode, setPoint, wipe) */
static inline void dComIfGp_setNextStage(const char* stage, s16 point, s8 roomNo, s8 layer, f32 lastSpeed, u32 lastMode, s32 setPoint, s8 wipe) {
    gabi::call(0x0252012C, stage, point, roomNo, layer, lastSpeed, lastMode, setPoint, wipe);
}
/* daPy_py_c::setPlayerPosAndAngle(cXyz*, s16): virtual, vtable (+0xB4) slot +0x114 */
static inline void daPy_setPlayerPosAndAngle(fopAc_ac_c* player, cXyz* pos, s16 angle) {
    u32 vt = gabi::load<u32>(gabi::ea(player) + 0xB4);
    gabi::call_ptr(gabi::load<u32>(vt + 0x114), player, pos, angle);
}

/* d_npc */
/* 0259D54C dNpc_playerEyePos(f32): returns a cXyz through a hidden result pointer (r3) (the
 * shared binding returns f32: wrong) */
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
/* 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16 desiredYrot, s16 maxVel, bool headOnly) */
static inline void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}

/* save info items: dSv_player_item_c at save + 0x5C, dSv_player_get_item_c at save + 0x71 */
static inline u32 dComIfGs_save() { return gabi::load<u32>(0x101F84DC); }
static inline BOOL dComIfGs_checkBottle(u8 item) { return gabi::call<BOOL>(0x025B5C80, dComIfGs_save() + 0x5C, item); }
static inline BOOL dComIfGs_checkEmptyBottle() { return gabi::call<BOOL>(0x025B5C54, dComIfGs_save() + 0x5C); }
static inline void dComIfGs_setBottleItemIn(u8 a, u8 b) { gabi::call(0x025B51DC, dComIfGs_save() + 0x5C, a, b); }
static inline void dComIfGs_setEmptyBottleItemIn(u8 a) { gabi::call(0x025B5448, dComIfGs_save() + 0x5C, a); }
static inline BOOL dComIfGs_isGetBottleItem(u8 item) { return gabi::call<BOOL>(0x025B5F44, dComIfGs_save() + 0x71, item); }
static inline void dComIfGs_setEventReg(u16 reg, u8 v) { dSv_event_setEventReg(dComIfGs_event(), reg, v); }
/* 02586D5C dLetter_send(u16) */
static inline void dLetter_send(u16 no) { gabi::call(0x02586D5C, no); }
/* 02544950 dEvent_manager_c::ChkPresentEnd (HD: does not read `this`) */
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
static inline u8 dComIfGp_event_getPreItemNo() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }

/* temporary event flags (dSv_event_c at save + 0x1178) */
static inline dSv_event_c* dComIfGs_tmpEvent() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x1178); }
static inline BOOL dComIfGs_isTmpBit(u16 f) { return dSv_event_isEventBit(dComIfGs_tmpEvent(), f); }
static inline void dComIfGs_onTmpBit(u16 f) { dSv_event_onEventBit(dComIfGs_tmpEvent(), f); }
static inline void dComIfGs_offTmpBit(u16 f) { gabi::call(0x025B8B7C, dComIfGs_tmpEvent(), f); } /* dSv_event_c::offEventBit */
/* HD message manager (*(0x101F4B5C)): 025F795C returns the current message's status (the
 * matcher calls it fopMsgM_SearchByID) */
static inline u32 fopMsgM_getStatus() { return gabi::call<u32>(0x025F795C, gabi::load<u32>(0x101F4B5C)); }

/* J3DModelData (HD): 027F68FC returns the joint name table header (self-relative offset at
 * +0x10 to the JUTNameTab), 027F3F94 (the matcher calls it __nw) returns the joint tree
 * header (joint count u16 at +8) */
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }

/* dBgS (play + 0x12A0) */
static inline s8 dBgS_GetRoomId(dBgS* bgs, void* poly) { return gabi::call<s8>(0x024EF130, bgs, poly); }
static inline u8 dBgS_GetPolyColor(dBgS* bgs, void* poly) { return gabi::call<u8>(0x024EEEB8, bgs, poly); }

/* 02527028 dDemo_setDemoData(actor, flags, morf, arc, n, ids, p6, p7) */
static inline BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7) {
    return gabi::call<BOOL>(0x02527028, a, flags, morf, arc, n, ids, p6, p7);
}

/* ---- file statics ---- */
/* daNpc_Ba1_HIO_c, HD: vtable at 0 */
struct daNpc_Ba1_HIO_c {
    struct hio_prm_c {
        /* 0x00 */ be<s16> mMaxHeadX;
        /* 0x02 */ be<s16> mMaxHeadY;
        /* 0x04 */ be<s16> mMinHeadX;
        /* 0x06 */ be<s16> mMinHeadY;
        /* 0x08 */ be<s16> mMaxBackboneX;
        /* 0x0A */ be<s16> mMaxBackboneY;
        /* 0x0C */ be<s16> mMinBackboneX;
        /* 0x0E */ be<s16> mMinBackboneY;
        /* 0x10 */ be<s16> mMaxTurnStep;
        /* 0x12 */ be<s16> mCalcAngleTarget;
        /* 0x14 */ be<f32> mAttPosOffsetY;
        /* 0x18 */ be<f32> m18;
        /* 0x1C */ be<f32> m1C;
        /* 0x20 */ be<f32> m20;
    };
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> field_0x8;
    /* 0x0C */ hio_prm_c mPrmTbl;
};
WWHD_SIZE(daNpc_Ba1_HIO_c, 0x30);
static daNpc_Ba1_HIO_c& l_HIO() { return *gabi::at<daNpc_Ba1_HIO_c>(0x10465CD8); }
static be<s32>& l_check_wrk() { return *gabi::at<be<s32>>(0x10465CC8); }
static gptr<fopAc_ac_c>* l_check_inf() { return gabi::at<gptr<fopAc_ac_c>>(0x10465D24); } /* [20] */

/* pointers to member functions in .data (copied to the stack before set_action) */
enum : u32 {
    PMF_wait_action1 = 0x10016830,
    PMF_wait_action2 = 0x10016838,
    PMF_demo_action1 = 0x10016840,
    PMF_wait_action3 = 0x10016848,
    PMF_wait_action4 = 0x10016850,
};
static inline void pmf_load(ProcFunc_l* dst, u32 src) {
    gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(src));
    gabi::store<u32>(gabi::ea(dst) + 4, gabi::load<u32>(src + 4));
}
/* (this->*pmf)(arg) */
static inline void pmf_call(void* self, ProcFunc_l* pmf, void* arg) {
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        gabi::call_ptr<BOOL>(pmf->f, thisp, arg);
    } else {
        s16 voff = gabi::load<s16>(gabi::ea(pmf) + 6);
        u32 vt = gabi::load<u32>(thisp + voff);
        gabi::call_ptr<BOOL>(gabi::load<u32>(vt + i * 8 + 4), thisp, arg);
    }
}

/* 021EE4AC */
static void* searchActor_Fa(void* i_param_1, void*) {
    WWHD_FUNC(0x021EE4AC, void*, i_param_1, (void*)nullptr);
    if (l_check_wrk() < 0x14 && fopAc_IsActor(i_param_1) && i_param_1 != nullptr && fpcM_GetName(i_param_1) == fpcNm_NPC_FA1_e) {
        s32 n = l_check_wrk();
        l_check_wrk() = n + 1;
        l_check_inf()[n] = (fopAc_ac_c*)i_param_1;
    }
    return nullptr;
}
VERIFY(0x021EE4AC, searchActor_Fa);

/* HD J3D: j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at 0x104B4868; a model's joint
 * matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices) */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
static Mtx34* getAnmMtx(J3DModel* model, s32 jntNo) {
    J3DMtxBlock_l* blk = gabi::at<J3DMtxBlock_l>(gabi::load<u32>(gabi::ea(model) + 0x2C));
    blk->mFlags |= 0x10; /* HD: marks the joint matrices dirty */
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30);
}

/* 021EE52C */
void daNpc_Ba1_c::nodeBa1Control(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x021EE52C, void, this, i_node, i_model);
    /* static cXyz a_eye_pos_off(20.0f, -16.0f, 0.0f): guard 0x10465D74, object 0x10465D18 */
    cXyz* a_eye_pos_off = gabi::at<cXyz>(0x10465D18);
    if (gabi::load<u32>(0x10465D74) == 0) {
        a_eye_pos_off->z = 0.0f;
        gabi::store<u32>(0x10465D74, 1);
        a_eye_pos_off->x = 20.0f;
        a_eye_pos_off->y = -16.0f;
    }
    J3DJoint* joint = J3DNode_toJoint(i_node);
    u32 jointIdx = gabi::load<u16>(gabi::ea(joint) + 4);
    PSMTXCopy(getAnmMtx(i_model, jointIdx), mDoMtx_stack_c::get());
    if (jointIdx == (u32)(s32)m_hed_jnt_num) {
        mDoMtx_YrotM(mDoMtx_stack_c::get(), -m_jnt.mAngles[0][1]);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), -m_jnt.mAngles[0][0]);
        PSMTXMultVec(mDoMtx_stack_c::get(), a_eye_pos_off, &mTransformedEyePos);
    }
    if (jointIdx == (u32)(s32)m_bbone_jnt_num) {
        mDoMtx_XrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[1][1]);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[1][0]);
    }
    PSMTXCopy(mDoMtx_stack_c::get(), gabi::at<Mtx34>(0x104B4868));
    mtx_copy(getAnmMtx(i_model, jointIdx), mDoMtx_stack_c::get());
}
VERIFY(0x021EE52C, &daNpc_Ba1_c::nodeBa1Control);

/* 021EE6CC */
static BOOL nodeCallBack_Ba1(J3DNode* i_param_1, int i_param_2) {
    WWHD_FUNC(0x021EE6CC, BOOL, i_param_1, i_param_2);
    if (i_param_2 == 0) {
        u32 model = gabi::load<u32>(0x104B462C);
        daNpc_Ba1_c* user = gabi::at<daNpc_Ba1_c>(gabi::load<u32>(model + 0xB8));
        if (user != nullptr) {
            user->nodeBa1Control(i_param_1, gabi::at<J3DModel>(model));
        }
    }
    return TRUE;
}
VERIFY(0x021EE6CC, nodeCallBack_Ba1);

/* 021EE714 */
bool daNpc_Ba1_c::XyCheck_cB(int i_itemBtn) {
    WWHD_FUNC(0x021EE714, bool, this, i_itemBtn);
    return dComIfGp_getSelectItem(i_itemBtn) == dItemNo_FAIRY_BOTTLE_e;
}
VERIFY(0x021EE714, &daNpc_Ba1_c::XyCheck_cB);

/* 021EE754 */
/* tail call: the result register is passed through as is (typed u32) */
static u32 daNpc_Ba1_XyCheck_cB(void* i_this, int i_itemBtn) {
    WWHD_FUNC(0x021EE754, u32, i_this, i_itemBtn);
    return gabi::call<u32>(0x021EE714, i_this, i_itemBtn); /* static_cast<daNpc_Ba1_c*>(i_this)->XyCheck_cB(i_itemBtn) */
}
VERIFY(0x021EE754, daNpc_Ba1_XyCheck_cB);

/* 021EE758 */
s16 daNpc_Ba1_c::XyEvent_cB(int) {
    WWHD_FUNC(0x021EE758, s16, this, 0);
    return mEventIdTable[0];
}
VERIFY(0x021EE758, &daNpc_Ba1_c::XyEvent_cB);

/* 021EE760 */
static s16 daNpc_Ba1_XyEvent_cB(void* i_this, int param_1) {
    WWHD_FUNC(0x021EE760, s16, i_this, param_1);
    return static_cast<daNpc_Ba1_c*>(i_this)->XyEvent_cB(param_1);
}
VERIFY(0x021EE760, daNpc_Ba1_XyEvent_cB);

/* 021EE99C */
int daNpc_Ba1_c::btpNum_toResID(int param_1) {
    WWHD_FUNC(0x021EE99C, int, this, param_1);
    /* a_btp_resID_tbl (.data 0x1001699C) */
    return gabi::load<s32>(0x1001699C + param_1 * 4);
}
VERIFY(0x021EE99C, &daNpc_Ba1_c::btpNum_toResID);

/* 021EE9B0 */
/* i_param_1 (GameCube bool) is passed on unnormalised: typed u32 */
bool daNpc_Ba1_c::setBtp(u32 i_param_1, int i_btp_num) {
    WWHD_FUNC(0x021EE9B0, bool, this, i_param_1, i_btp_num);
    J3DModelData* model_data = J3DModel_getModelData_l(mpMorf->getModel());
    int res_id = btpNum_toResID(i_btp_num);
    m_hed_tex_pttrn = (J3DAnmTexPattern*)dComIfG_getObjectIDRes(STR(0x100169CC), res_id);
    if (m_hed_tex_pttrn.get() == nullptr) /* JUT_ASSERT(572, m_hed_tex_pttrn != NULL) */
        JUT_ASSERT_fail(STR(0x100169D0), 0x23C, STR(0x100169E0));

    int iVar1 = mDoExt_btpAnm_init(mHeadBtpAnm, model_data, m_hed_tex_pttrn, 1, 2, 1.0f, 0, -1, i_param_1, 0);
    bool o_retval = iVar1 == 1;
    if (o_retval) {
        mBlinkTimer = 0;
        mBlinkFrame = 0;
    }
    return o_retval;
}
VERIFY(0x021EE9B0, &daNpc_Ba1_c::setBtp);

/* 021EEA9C */
/* tail call: setBtp's result register is passed through (typed u32) */
u32 daNpc_Ba1_c::iniTexPttrnAnm(u32 i_param_1) {
    WWHD_FUNC(0x021EEA9C, u32, this, i_param_1);
    return gabi::call<u32>(0x021EE9B0, this, i_param_1, (s32)mBtpNum); /* setBtp(i_param_1, mBtpNum) */
}
VERIFY(0x021EEA9C, &daNpc_Ba1_c::iniTexPttrnAnm);

/* 021EEAA8 */
bool daNpc_Ba1_c::create_itm_Mdl() {
    WWHD_FUNC(0x021EEAA8, bool, this);
    mpClothModel = nullptr;
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x100169F8), dRes_ID_BA_BDL_BA_CLOTH_e);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2926, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x100169FC), 0xB6E, STR(0x10016A0C));
    mpClothModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000002);
    return true;
}
VERIFY(0x021EEAA8, &daNpc_Ba1_c::create_itm_Mdl);

/* 021EECB8 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021EECB8, BOOL, i_this);
    return static_cast<daNpc_Ba1_c*>(i_this)->CreateHeap();
}
VERIFY(0x021EECB8, CheckCreateHeap);

/* 021EECBC */
bool daNpc_Ba1_c::charDecide(int i_param_1) {
    WWHD_FUNC(0x021EECBC, bool, this, i_param_1);
    mType = 0;
    /* the GameCube switch (0..4 -> mSpecificType = i_param_1) folded */
    s8 specificType = -1;
    bool ret;
    if ((u32)i_param_1 <= 4) {
        specificType = (s8)i_param_1;
        ret = true;
    } else {
        ret = false;
    }
    mSpecificType = specificType;
    return ret;
}
VERIFY(0x021EECBC, &daNpc_Ba1_c::charDecide);

/* 021EECEC */
BOOL daNpc_Ba1_c::set_action(ProcFunc_l* i_newProcFunc, void* i_argsP) {
    WWHD_FUNC(0x021EECEC, BOOL, this, i_newProcFunc, i_argsP);
    ProcFunc_l* cur = &mCurrProcFunc;
    s16 newI = i_newProcFunc->i;
    s16 newD;
    u32 newF;
    bool differ;
    if ((u16)cur->i == (u16)newI) {
        if (cur->i == 0)
            return TRUE;
        newD = i_newProcFunc->d;
        newF = i_newProcFunc->f;
        differ = (u16)cur->d != (u16)newD || cur->f != newF;
        if (!differ)
            return TRUE;
    } else {
        newF = i_newProcFunc->f;
        newD = i_newProcFunc->d;
    }
    if (cur->i != 0) {
        m818 = 9;
        pmf_call(this, cur, i_argsP);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    m818 = 0;
    pmf_call(this, cur, i_argsP);
    return TRUE;
}
VERIFY(0x021EECEC, &daNpc_Ba1_c::set_action);

/* 021EEE18 */
bool daNpc_Ba1_c::init_BA1_0() {
    WWHD_FUNC(0x021EEE18, bool, this);
    if (!dComIfGs_isEventBit(dSv_event_flag_UNK_0520) && !dComIfGs_isEventBit(dSv_event_flag_UNK_0001)) {
        gabi::Local<ProcFunc_l> pmf;
        pmf_load(pmf, PMF_wait_action1);
        set_action(pmf, nullptr);
        mpClothModel = nullptr;
        return true;
    } else {
        return false;
    }
}
VERIFY(0x021EEE18, &daNpc_Ba1_c::init_BA1_0);

/* 021EEEC8 */
bool daNpc_Ba1_c::init_BA1_1() {
    WWHD_FUNC(0x021EEEC8, bool, this);
    bool ret = !dComIfGs_isEventBit(dSv_event_flag_UNK_0520);
    if (!ret) {
        return ret;
    }
    ret = dComIfGs_isEventBit(dSv_event_flag_UNK_0001) != 0;
    if (ret) {
        if (dComIfGs_checkCollect(1)) {
            current.pos.y = 0.0f;
            current.pos.x = -290.0f;
            current.angle.y = 0;
            current.pos.z = 110.0f;
        }
        gabi::Local<ProcFunc_l> pmf;
        pmf_load(pmf, PMF_wait_action2);
        set_action(pmf, nullptr);
        mpClothModel = nullptr;
        actor_status &= ~0x80u; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
    }
    return ret;
}
VERIFY(0x021EEEC8, &daNpc_Ba1_c::init_BA1_1);

/* 021EEFB0 */
bool daNpc_Ba1_c::init_BA1_2() {
    WWHD_FUNC(0x021EEFB0, bool, this);
    mpClothModel = nullptr;
    gabi::Local<ProcFunc_l> pmf;
    pmf_load(pmf, PMF_demo_action1);
    set_action(pmf, nullptr);
    return true;
}
VERIFY(0x021EEFB0, &daNpc_Ba1_c::init_BA1_2);

/* 021EEFF4 */
bool daNpc_Ba1_c::init_BA1_3() {
    WWHD_FUNC(0x021EEFF4, bool, this);
    bool ret = dComIfGs_isEventBit(dSv_event_flag_UNK_0520) != 0;
    if (!ret) {
        return ret;
    }
    ret = !dComIfGs_isEventBit(dSv_event_flag_GRANDMA_HEALED);
    if (ret) {
        gabi::store<u8>(gabi::ea(this) + 0x38B, 0x1C); /* attention_info.distances[SPEAK] */
        gabi::store<u8>(gabi::ea(this) + 0x389, 0x1B); /* attention_info.distances[TALK] */
        actor_status &= ~0x180u; /* fopAcM_OffStatus(this, fopAcStts_CULL_e | fopAcStts_NOCULLEXEC_e) */
        gabi::Local<ProcFunc_l> pmf;
        pmf_load(pmf, PMF_wait_action3);
        set_action(pmf, nullptr);
        gravity = 0.0f;
        gabi::store<u32>(gabi::ea(this) + 0x100, 0x021EE760); /* eventInfo.setXyEventCB(daNpc_Ba1_XyEvent_cB) */
        gabi::store<u32>(gabi::ea(this) + 0x104, 0x021EE754); /* eventInfo.setXyCheckCB(daNpc_Ba1_XyCheck_cB) */
    }
    return ret;
}
VERIFY(0x021EEFF4, &daNpc_Ba1_c::init_BA1_3);

/* 021EF0D0 */
bool daNpc_Ba1_c::init_BA1_4() {
    WWHD_FUNC(0x021EF0D0, bool, this);
    bool ret = dComIfGs_isEventBit(dSv_event_flag_UNK_0520) != 0;
    if (!ret) {
        return ret;
    }
    ret = dComIfGs_isEventBit(dSv_event_flag_GRANDMA_HEALED) != 0;
    if (ret) {
        mpClothModel = nullptr;
        gabi::Local<ProcFunc_l> pmf;
        pmf_load(pmf, PMF_wait_action4);
        set_action(pmf, nullptr);
    }
    return ret;
}
VERIFY(0x021EF0D0, &daNpc_Ba1_c::init_BA1_4);

/* 021EF170 */
void daNpc_Ba1_c::plyTexPttrnAnm() {
    WWHD_FUNC(0x021EF170, void, this);
    if (mBtpNum != 0 || !cLib_calcTimer(&mBlinkTimer)) {
        u8 frame = (u8)(mBlinkFrame + 1);
        mBlinkFrame = frame;
        if ((s32)frame >= J3DAnm_getFrameMax(m_hed_tex_pttrn)) {
            if (mBtpNum != 0) {
                mBlinkFrame = (u8)J3DAnm_getFrameMax(m_hed_tex_pttrn);
            } else {
                s16 t = (s16)gabi::ftoi(cM_rndF(60.0f) + 30.0f);
                mBlinkFrame = 0;
                mBlinkTimer = t;
            }
        }
    }
}
VERIFY(0x021EF170, &daNpc_Ba1_c::plyTexPttrnAnm);

/* 021EF244 */
void daNpc_Ba1_c::setAttention(u32 i_setEyePos) {
    WWHD_FUNC(0x021EF244, void, this, i_setEyePos);
    f32 offset = l_HIO().mPrmTbl.mAttPosOffsetY;
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    attPos->z = current.pos.z;
    attPos->y = current.pos.y + offset;
    attPos->x = current.pos.x;
    if (!mbSetEyePos && !i_setEyePos) {
        return;
    }
    f32 y = mTransformedEyePos.y + mEyeOffset;
    eyePos.x = mTransformedEyePos.x;
    eyePos.z = mTransformedEyePos.z;
    eyePos.y = y;
}
VERIFY(0x021EF244, &daNpc_Ba1_c::setAttention);

/* 021EF7E8 */
static cPhs_State daNpc_Ba1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021EF7E8, cPhs_State, i_this);
    return ((daNpc_Ba1_c*)i_this)->_create();
}
VERIFY(0x021EF7E8, daNpc_Ba1_Create);

/* 021EF7EC */
BOOL daNpc_Ba1_c::_delete() {
    WWHD_FUNC(0x021EF7EC, BOOL, this);
    dComIfG_resDelete(&mPhs, STR(0x10016A4B));
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x021EF7EC, &daNpc_Ba1_c::_delete);

/* 021EF844 */
static BOOL daNpc_Ba1_Delete(daNpc_Ba1_c* i_this) {
    WWHD_FUNC(0x021EF844, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x021EF844, daNpc_Ba1_Delete);

/* 021EF848 */
void daNpc_Ba1_c::partner_srch() {
    WWHD_FUNC(0x021EF848, void, this);
    if (m818 == 1) {
        m818 = m818 + 1;
    }
}
VERIFY(0x021EF848, &daNpc_Ba1_c::partner_srch);

/* 021EF864 */
bool daNpc_Ba1_c::checkCommandTalk() {
    WWHD_FUNC(0x021EF864, bool, this);
    bool ret = false;
    if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 1 /* eventInfo.checkCommandTalk() */) {
        ret = dComIfGp_event_chkTalkXY() == 0;
    }
    return ret;
}
VERIFY(0x021EF864, &daNpc_Ba1_c::checkCommandTalk);

/* 021EF8B4 */
void daNpc_Ba1_c::checkOrder() {
    WWHD_FUNC(0x021EF8B4, void, this);
    if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 2 /* eventInfo.checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mEventIdTable[mEventIdx])) {
            m812 = 0;
        }
    } else if (checkCommandTalk() && (m812 == 1 || m812 == 2)) {
        m812 = 0;
        m809 = true;
    }
}
VERIFY(0x021EF8B4, &daNpc_Ba1_c::checkOrder);

/* 021EFC5C */
s32 daNpc_Ba1_c::isEventEntry() {
    WWHD_FUNC(0x021EFC5C, s32, this);
    const char* name = gabi::at<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}
VERIFY(0x021EFC5C, &daNpc_Ba1_c::isEventEntry);

/* 021EFC9C */
void daNpc_Ba1_c::endEvent() {
    WWHD_FUNC(0x021EFC9C, void, this);
    dComIfGp_event_reset();
    m80E = 0xFF;
}
VERIFY(0x021EFC9C, &daNpc_Ba1_c::endEvent);

/* 021EFCDC */
u32 daNpc_Ba1_c::setAnm_tex(s8 i_btp_num) {
    WWHD_FUNC(0x021EFCDC, u32, this, i_btp_num);
    if (mBtpNum != i_btp_num) {
        mBtpNum = i_btp_num;
        return iniTexPttrnAnm(true);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x021EFCDC, &daNpc_Ba1_c::setAnm_tex);

/* 021EFCFC */
int daNpc_Ba1_c::anmNum_toResID(int param_1) {
    WWHD_FUNC(0x021EFCFC, int, this, param_1);
    /* a_bck_resID_tbl (.data 0x10016A5C) */
    return gabi::load<s32>(0x10016A5C + param_1 * 4);
}
VERIFY(0x021EFCFC, &daNpc_Ba1_c::anmNum_toResID);

/* 021EFD10 */
BOOL daNpc_Ba1_c::setAnm_anm(daNpc_Ba1_c::anm_prm_c* i_anmPrmP) {
    WWHD_FUNC(0x021EFD10, BOOL, this, i_anmPrmP);
    if (mAnmNum == i_anmPrmP->mAnmNum) {
        return TRUE;
    }
    mAnmNum = i_anmPrmP->mAnmNum;
    int resID = anmNum_toResID(mAnmNum);
    s32 loopMode = i_anmPrmP->mLoopMode;
    f32 speed = i_anmPrmP->mSpeed;
    f32 morf = i_anmPrmP->mMorf;
    dNpc_setAnmIDRes(mpMorf, loopMode, morf, speed, resID, -1, STR(0x10016A84));
    mbMorfAnimStopped = 0;
    mPrevMorfFrame = 0.0f;
    m7EF = 0;
    return TRUE;
}
VERIFY(0x021EFD10, &daNpc_Ba1_c::setAnm_anm);

/* 021EFDA4 */
void daNpc_Ba1_c::setAnm_NUM(int i_param_1, int i_param_2) {
    WWHD_FUNC(0x021EFDA4, void, this, i_param_1, i_param_2);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BB898); /* [11] */
    if (i_param_2 != 0) {
        setAnm_tex(a_anm_prm_tbl[i_param_1].mBtpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[i_param_1]);
}
VERIFY(0x021EFDA4, &daNpc_Ba1_c::setAnm_NUM);

/* 021EFF5C */
void daNpc_Ba1_c::eInit_USE_FAIRY_END_() {
    WWHD_FUNC(0x021EFF5C, void, this);
    gabi::store<u8>(gabi::ea(this) + 0x389, 0x5B); /* attention_info.distances[TALK] */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0x59); /* attention_info.distances[SPEAK] */
    mFairyUsed = true;
}
VERIFY(0x021EFF5C, &daNpc_Ba1_c::eInit_USE_FAIRY_END_);

/* 021EFF78 */
void daNpc_Ba1_c::eInit_MOV_POS_() {
    WWHD_FUNC(0x021EFF78, void, this);
    m7FE = true;
    mpClothModel = nullptr;
    m7F5 = true;
    m7F7 = true;
}
VERIFY(0x021EFF78, &daNpc_Ba1_c::eInit_MOV_POS_);

/* 021EFF94 */
void daNpc_Ba1_c::eInit_SET_PLYER_TRN_ANG_() {
    WWHD_FUNC(0x021EFF94, void, this);
    /* HD: daPy_getPlayerActorClass() and dComIfGp_getPlayer(0) are one load */
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    /* player->changeDemoMoveAngle(angle): daPy_py_c +0x422 */
    gabi::store<s16>(gabi::ea(player) + 0x422, cLib_targetAngleY(&player->current.pos, &current.pos));
}
VERIFY(0x021EFF94, &daNpc_Ba1_c::eInit_SET_PLYER_TRN_ANG_);

/* 021F0158 */
void daNpc_Ba1_c::eInit_setEvTimer(be<s32>* i_param_1) {
    WWHD_FUNC(0x021F0158, void, this, i_param_1);
    mEvTimer = 0;
    if (i_param_1 != nullptr) {
        mEvTimer = (s16)*i_param_1;
    }
}
VERIFY(0x021F0158, &daNpc_Ba1_c::eInit_setEvTimer);

/* 021F0174 */
void daNpc_Ba1_c::eInit_CHK_FAIRY_(be<s32>* i_param_1) {
    WWHD_FUNC(0x021F0174, void, this, i_param_1);
    eInit_setEvTimer(i_param_1);
}
VERIFY(0x021F0174, &daNpc_Ba1_c::eInit_CHK_FAIRY_);

/* 021F0178 */
f32 daNpc_Ba1_c::eInit_prmFloat(be<f32>* i_param_1, f32 i_param_2) {
    WWHD_FUNC(0x021F0178, f32, this, i_param_1, i_param_2);
    if (i_param_1 != nullptr) {
        return *i_param_1;
    }
    return i_param_2;
}
VERIFY(0x021F0178, &daNpc_Ba1_c::eInit_prmFloat);

/* 021F0188 */
void daNpc_Ba1_c::eInit_SET_EYE_OFF_(be<f32>* i_param_1) {
    WWHD_FUNC(0x021F0188, void, this, i_param_1);
    mEyeOffset = eInit_prmFloat(i_param_1, 0.0f);
}
VERIFY(0x021F0188, &daNpc_Ba1_c::eInit_SET_EYE_OFF_);

/* 021F01B8 */
void daNpc_Ba1_c::eInit_EYE_OFF_ZRO_(be<f32>* i_param_1) {
    WWHD_FUNC(0x021F01B8, void, this, i_param_1);
    mEyeOffsetZero = eInit_prmFloat(i_param_1, 0.0f);
}
VERIFY(0x021F01B8, &daNpc_Ba1_c::eInit_EYE_OFF_ZRO_);

/* 021F01E8 */
void daNpc_Ba1_c::eInit_CHK_FAIRY_MOV_1(be<s32>* i_param_1) {
    WWHD_FUNC(0x021F01E8, void, this, i_param_1);
    eInit_setEvTimer(i_param_1);
}
VERIFY(0x021F01E8, &daNpc_Ba1_c::eInit_CHK_FAIRY_MOV_1);

/* 021F0838: the matcher calls it cLib_calcTimer<s16> (d_a_npc_ac1); it is searchByID */
fopAc_ac_c* daNpc_Ba1_c::searchByID(fpc_ProcID i_procID) {
    WWHD_FUNC(0x021F0838, fopAc_ac_c*, this, i_procID);
    gabi::Local<gptr<fopAc_ac_c>> o_actor;
    *o_actor = nullptr;
    gabi::call(0x025D54C4, i_procID, o_actor.get()); /* fopAcM_SearchByID(id, &actor) */
    return *o_actor;
}
VERIFY(0x021F0838, &daNpc_Ba1_c::searchByID);

/* 021F1288 */
static BOOL daNpc_Ba1_Execute(daNpc_Ba1_c* i_this) {
    WWHD_FUNC(0x021F1288, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x021F1288, daNpc_Ba1_Execute);

/* 021F1460 */
static BOOL daNpc_Ba1_Draw(daNpc_Ba1_c* i_this) {
    WWHD_FUNC(0x021F1460, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x021F1460, daNpc_Ba1_Draw);

/* 021F1464 */
static BOOL daNpc_Ba1_IsDelete(daNpc_Ba1_c*) {
    WWHD_FUNC(0x021F1464, BOOL, (daNpc_Ba1_c*)nullptr);
    return TRUE;
}
VERIFY(0x021F1464, daNpc_Ba1_IsDelete);

/* 021F146C */
bool daNpc_Ba1_c::setAnm() {
    WWHD_FUNC(0x021F146C, bool, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BB950); /* [8] */
    if (a_anm_prm_tbl[mStatus].mBtpNum >= 0) {
        setAnm_tex(a_anm_prm_tbl[mStatus].mBtpNum);
    }
    if (a_anm_prm_tbl[mStatus].mAnmNum >= 0) {
        setAnm_anm(&a_anm_prm_tbl[mStatus]);
    }
    return true;
}
VERIFY(0x021F146C, &daNpc_Ba1_c::setAnm);

/* 021F14F0 */
void daNpc_Ba1_c::setAnm_ATR(int i_param_1) {
    WWHD_FUNC(0x021F14F0, void, this, i_param_1);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BB9D0); /* [21] */
    if (i_param_1 != 0) {
        setAnm_tex(a_anm_prm_tbl[m80E].mBtpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[m80E]);
}
VERIFY(0x021F14F0, &daNpc_Ba1_c::setAnm_ATR);

/* 021F1560 */
void daNpc_Ba1_c::chg_anmAtr(u8 i_param_1) {
    WWHD_FUNC(0x021F1560, void, this, i_param_1);
    if ((i_param_1 >= 0x15) || (i_param_1 == m80E)) {
        return;
    }
    m80E = i_param_1;
    setAnm_ATR(1);
}
VERIFY(0x021F1560, &daNpc_Ba1_c::chg_anmAtr);

/* 021F1584 */
void daNpc_Ba1_c::control_anmAtr() {
    WWHD_FUNC(0x021F1584, void, this);
    switch (m80E) {
    case 5:
    case 6:
        switch (mLookBackState) {
        case 1:
            break;
        default:
            m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
            mLookBackState = 1;
            break;
        }
        break;
    default:
        return;
    }
}
VERIFY(0x021F1584, &daNpc_Ba1_c::control_anmAtr);

/* 021F15B8 */
void daNpc_Ba1_c::anmAtr(u16 i_param_1) {
    WWHD_FUNC(0x021F15B8, void, this, i_param_1);
    switch (i_param_1) {
    case 6: {
        if (m819 == 0) {
            m80E = 0xFF;
            chg_anmAtr(dComIfGp_getMesgAnimeAttrInfo());
            m819 = m819 + 1;
        }
        u8 mesgAnimeTagInfo = gabi::load<u8>(dComIfGp_ea() + 0x5BC6); /* dComIfGp_getMesgAnimeTagInfo() */
        gabi::store<u8>(dComIfGp_ea() + 0x5BC6, 0xFF);                /* dComIfGp_clearMesgAnimeTagInfo() */
        if (mesgAnimeTagInfo != 0xFF && mMesgAnimeTagInfo != mesgAnimeTagInfo) {
            mMesgAnimeTagInfo = mesgAnimeTagInfo;
            /* chg_anmTag(): empty */
        }
        break;
    }
    case 0xE:
        m819 = 0;
        break;
    default:
        break;
    }
    /* control_anmTag(): empty */
    control_anmAtr();
}
VERIFY(0x021F15B8, &daNpc_Ba1_c::anmAtr);

/* 021F1CB4 */
u32 daNpc_Ba1_c::getMsg_BA1_4() {
    WWHD_FUNC(0x021F1CB4, u32, this);
    return getMsg_BA1_3();
}
VERIFY(0x021F1CB4, &daNpc_Ba1_c::getMsg_BA1_4);

/* 021F1CB8 */
u32 daNpc_Ba1_c::getMsg() {
    WWHD_FUNC(0x021F1CB8, u32, this);
    u32 ret = 0;
    switch (mSpecificType) {
    case 0:
        ret = getMsg_BA1_0();
        break;
    case 1:
        ret = getMsg_BA1_1();
        break;
    case 3:
        ret = getMsg_BA1_3();
        break;
    case 4:
        ret = getMsg_BA1_4();
        break;
    }
    return ret;
}
VERIFY(0x021F1CB8, &daNpc_Ba1_c::getMsg);

/* 021F1D20 */
u8 daNpc_Ba1_c::chkAttention() {
    WWHD_FUNC(0x021F1D20, u8, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}
VERIFY(0x021F1D20, &daNpc_Ba1_c::chkAttention);

/* 021F17AC */
u8 daNpc_Ba1_c::chk_partsNotMove() {
    WWHD_FUNC(0x021F17AC, u8, this);
    return mJointHeadY == m_jnt.mAngles[0][1] && mJointBackboneY == m_jnt.mAngles[1][1] && mActorAngleY == current.angle.y;
}
VERIFY(0x021F17AC, &daNpc_Ba1_c::chk_partsNotMove);

/* 021F1E78 */
void daNpc_Ba1_c::setStt(s8 i_status) {
    WWHD_FUNC(0x021F1E78, void, this, i_status);
    s8 temp = mStatus;
    mStatus = i_status;
    mEvTimer2 = 0;
    switch ((u32)(s32)i_status) {
    case 2:
        m80E = 0xFF;
        if (mAnmNum != 4) {
            m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
            mLookBackState = 1;
        }
        mPrevStatus = temp;
        return;
    case 0:
        break;
    case 5:
        mPrevStatus = temp;
        m80E = 0xFF;
        return;
    case 1:
    case 3:
    case 4:
    case 6:
    case 7:
        mHeadOnlyFollow = true;
        break;
    }
    setAnm();
}
VERIFY(0x021F1E78, &daNpc_Ba1_c::setStt);

/* 021F24B4 */
BOOL daNpc_Ba1_c::talk_2() {
    WWHD_FUNC(0x021F24B4, BOOL, this);
    return talk_1();
}
VERIFY(0x021F24B4, &daNpc_Ba1_c::talk_2);

/* 021F26D0 */
BOOL daNpc_Ba1_c::wait_3() {
    WWHD_FUNC(0x021F26D0, BOOL, this);
    return wait_2();
}
VERIFY(0x021F26D0, &daNpc_Ba1_c::wait_3);

/* 021EFE10 */
void daNpc_Ba1_c::eInit_SET_PLYER_GOL_() {
    WWHD_FUNC(0x021EFE10, void, this);
    dComIfGp_get(); /* HD: an unused accessor call */
    gabi::Local<cXyz> tmp;
    gabi::Local<cXyz> target;
    f32 x = current.pos.x, y = current.pos.y;
    tmp->x = 0.0f;
    tmp->y = 0.0f;
    f32 z = current.pos.z;
    tmp->z = 50.0f;
    PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), tmp, target);
    dComIfGp_evmng_setGoal(target);
}
VERIFY(0x021EFE10, &daNpc_Ba1_c::eInit_SET_PLYER_GOL_);

/* 021EFEA4 */
void daNpc_Ba1_c::eInit_PLYER_INI_POS_() {
    WWHD_FUNC(0x021EFEA4, void, this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0); /* daPy_getPlayerActorClass() */
    gabi::Local<cXyz> tmp;
    gabi::Local<cXyz> target;
    f32 x = current.pos.x, y = current.pos.y;
    tmp->x = 0.0f;
    tmp->y = 0.0f;
    tmp->z = 180.0f;
    PSMTXTrans(mDoMtx_stack_c::get(), x, y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), tmp, target);
    s16 angle = cLib_targetAngleY(target, &current.pos);
    daPy_setPlayerPosAndAngle(player, target, angle);
}
VERIFY(0x021EFEA4, &daNpc_Ba1_c::eInit_PLYER_INI_POS_);

/* 021EFFDC */
void daNpc_Ba1_c::eInit_ACTOR_DRW_CONTROL_(be<s32>* i_param_1, be<s32>* i_param_2) {
    WWHD_FUNC(0x021EFFDC, void, this, i_param_1, i_param_2);
    if (i_param_1 != nullptr) {
        fopAc_ac_c* player = dComIfGp_getPlayer(0); /* daPy_getPlayerActorClass() */
        be<u32>* noResetFlg0 = gabi::at<be<u32>>(gabi::ea(player) + 0x3B8);
        switch ((u32)(s32)*i_param_1) {
        case 0:
            *noResetFlg0 |= 0x08000000u; /* onNoResetFlg0(daPyFlg0_NO_DRAW) */
            break;
        case 1:
            *noResetFlg0 &= ~0x08000000u; /* offNoResetFlg0(daPyFlg0_NO_DRAW) */
            break;
        }
    }
    if (i_param_2 != nullptr) {
        switch ((u32)(s32)*i_param_2) {
        case 0:
            m7F7 = 1;
            break;
        case 1:
            m7F7 = 0;
            break;
        case 2:
            setAnm_NUM(10, 1);
            m7F7 = 1;
            return;
        case 3: {
            current.angle.y = cLib_targetAngleY(&current.pos, &dComIfGp_getPlayer(0)->current.pos);
            gabi::Local<cXyz> tmp;
            f32 x = current.pos.x, y = current.pos.y;
            gabi::store<u8>(gabi::ea(this) + 0x38B, 0xAB); /* attention_info.distances[SPEAK] */
            tmp->x = 0.0f;
            gabi::store<u8>(gabi::ea(this) + 0x389, 0xAB); /* attention_info.distances[TALK] */
            tmp->y = 0.0f;
            tmp->z = 90.0f;
            PSMTXTrans(mDoMtx_stack_c::get(), x, y, current.pos.z);
            mDoMtx_stack_c::YrotM(current.angle.y);
            PSMTXMultVec(mDoMtx_stack_c::get(), tmp, &current.pos);
            m7F7 = 0;
            break;
        }
        }
    }
}
VERIFY(0x021EFFDC, &daNpc_Ba1_c::eInit_ACTOR_DRW_CONTROL_);

/* 021F01EC */
void daNpc_Ba1_c::event_actionInit(int i_staff_idx) {
    WWHD_FUNC(0x021F01EC, void, this, i_staff_idx);
    be<s32>* act_no_p = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staff_idx, STR(0x10016A90)); /* "ActNo" */
    be<s32>* prm_0_p = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staff_idx, STR(0x10016A98)); /* "prm_0" */
    be<s32>* prm_1_p = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staff_idx, STR(0x10016AA0)); /* "prm_1" */
    be<s32>* timer_p = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staff_idx, STR(0x10016AA8)); /* "Timer" */
    be<f32>* atten_p = (be<f32>*)dComIfGp_evmng_getMyFloatP(i_staff_idx, STR(0x10016AB0));   /* "Atten" */
    be<f32>* speed_p = (be<f32>*)dComIfGp_evmng_getMyFloatP(i_staff_idx, STR(0x10016AB8));   /* "Speed" */
    if (act_no_p != nullptr) {
        mActNo = (s8)*act_no_p;
        switch (mActNo) {
        case 0:
            setAnm_NUM(9, 1);
            break;
        case 1:
            eInit_SET_PLYER_GOL_();
            break;
        case 2:
            eInit_PLYER_INI_POS_();
            break;
        case 3:
            eInit_USE_FAIRY_END_();
            break;
        case 4:
            eInit_MOV_POS_();
            break;
        case 5:
            eInit_SET_PLYER_TRN_ANG_();
            break;
        case 6:
            eInit_ACTOR_DRW_CONTROL_(prm_0_p, prm_1_p);
            break;
        case 7:
            eInit_CHK_FAIRY_(timer_p);
            break;
        case 8:
            eInit_SET_EYE_OFF_(atten_p);
            break;
        case 9:
            eInit_EYE_OFF_ZRO_(speed_p);
            break;
        case 10:
            eInit_CHK_FAIRY_MOV_1(timer_p);
            break;
        }
    }
}
VERIFY(0x021F01EC, &daNpc_Ba1_c::event_actionInit);

/* 021F0474 */
void daNpc_Ba1_c::cut_init_START_TALE1(int i_staff_idx) {
    WWHD_FUNC(0x021F0474, void, this, i_staff_idx);
    be<s32>* timer_p = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staff_idx, STR(0x10016AC0)); /* "Timer" */
    mEvTimer = 0;
    if (timer_p != nullptr) {
        mEvTimer = (s16)*timer_p;
    }
}
VERIFY(0x021F0474, &daNpc_Ba1_c::cut_init_START_TALE1);

/* 021F0694 */
bool daNpc_Ba1_c::partner_srch_sub(u32 i_searchFunc) {
    WWHD_FUNC(0x021F0694, bool, this, i_searchFunc);
    bool o_retval = false;
    mPartnerProcID = 0xFFFFFFFF;
    l_check_wrk() = 0;
    for (int i = 0; i < 20; i++) {
        l_check_inf()[i] = nullptr;
    }
    fpcM_Search(i_searchFunc, this);
    if (l_check_wrk() != 0) {
        mPartnerProcID = fopAcM_GetID(l_check_inf()[0].get());
        o_retval = true;
    }
    return o_retval;
}
VERIFY(0x021F0694, &daNpc_Ba1_c::partner_srch_sub);

/* 021F0740 */
u32 daNpc_Ba1_c::eMove_CHK_FAIRY_() {
    WWHD_FUNC(0x021F0740, u32, this);
    if (cLib_calcTimer(&mEvTimer) == 0) {
        return !partner_srch_sub(0x021EE4AC /* searchActor_Fa */);
    }
    return false;
}
VERIFY(0x021F0740, &daNpc_Ba1_c::eMove_CHK_FAIRY_);

/* 021F07A0 */
u32 daNpc_Ba1_c::eMove_EYE_OFF_ZRO_() {
    WWHD_FUNC(0x021F07A0, u32, this);
    f32 step = mEyeOffsetZero;
    bool ret = gabi::ftoi(step) == 0;
    if (ret) {
        mEyeOffset = 0.0f;
    } else {
        cLib_chaseF(&mEyeOffset, 0.0f, step);
        ret = gabi::ftoi(mEyeOffset) == 0;
        if (ret) {
            mEyeOffset = 0.0f;
        }
    }
    return ret;
}
VERIFY(0x021F07A0, &daNpc_Ba1_c::eMove_EYE_OFF_ZRO_);

/* 021F086C */
u32 daNpc_Ba1_c::eMove_CHK_FAIRY_MOV_1() {
    WWHD_FUNC(0x021F086C, u32, this);
    bool ret = false;
    if (!cLib_calcTimer(&mEvTimer)) {
        if (partner_srch_sub(0x021EE4AC /* searchActor_Fa */)) {
            fopAc_ac_c* actor_fa = searchByID(mPartnerProcID);
            if (actor_fa != nullptr) {
                ret = gabi::load<u8>(gabi::ea(actor_fa) + 0x8AC) == 5; /* daNpc_Fa1_c::isBabaMode() */
                if (ret) {
                    eInit_PLYER_INI_POS_();
                }
            }
        }
    }
    return ret;
}
VERIFY(0x021F086C, &daNpc_Ba1_c::eMove_CHK_FAIRY_MOV_1);

/* 021F08FC */
/* the eMove_* results (bool) are returned as is: typed u32 */
u32 daNpc_Ba1_c::event_action() {
    WWHD_FUNC(0x021F08FC, u32, this);
    u32 ret;
    switch (mActNo) {
    case 4:
        ret = eMove_MOV_POS_();
        break;
    case 7:
        ret = eMove_CHK_FAIRY_();
        break;
    case 9:
        ret = eMove_EYE_OFF_ZRO_();
        break;
    case 10:
        ret = eMove_CHK_FAIRY_MOV_1();
        break;
    default:
        ret = true;
        break;
    }
    return ret;
}
VERIFY(0x021F08FC, &daNpc_Ba1_c::event_action);

/* 021F0964 */
bool daNpc_Ba1_c::cut_move_START_TALE1() {
    WWHD_FUNC(0x021F0964, bool, this);
    if (cLib_calcTimer(&mEvTimer) == 0) {
        /* dComIfGs_getClearCount(): save info byte +0x1C0 */
        s16 point = gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x1C0) != 0 ? 202 : 200;
        dComIfGp_setNextStage(STR(0x10016ADC) /* "LinkRM" */, point, 0, 8, 0.0f, 0, 1, 0);
    }
    return mEvTimer == 0;
}
VERIFY(0x021F0964, &daNpc_Ba1_c::cut_move_START_TALE1);

/* 021F09EC */
void daNpc_Ba1_c::privateCut(int i_staff_idx) {
    WWHD_FUNC(0x021F09EC, void, this, i_staff_idx);
    /* static char* a_cut_tbl[] = {"ACTION", "START_TALE1"} (.data 0x101BB948) */
    enum { ACT_ACTION, ACT_START_TALE1 };
    if (i_staff_idx == -1) {
        return;
    }
    mActionIndex = dComIfGp_evmng_getMyActIdx(i_staff_idx, 0x101BB948, 2, TRUE, 0);
    if (mActionIndex == -1) {
        dComIfGp_evmng_cutEnd(i_staff_idx);
    } else {
        if (dComIfGp_evmng_getIsAddvance(i_staff_idx)) {
            switch (mActionIndex) {
            case ACT_ACTION:
                event_actionInit(i_staff_idx);
                break;
            case ACT_START_TALE1:
                cut_init_START_TALE1(i_staff_idx);
                break;
            }
        }
        bool endCut;
        switch (mActionIndex) {
        case ACT_ACTION:
            endCut = event_action();
            break;
        case ACT_START_TALE1:
            endCut = cut_move_START_TALE1();
            break;
        default:
            endCut = true;
            break;
        }
        if (endCut) {
            dComIfGp_evmng_cutEnd(i_staff_idx);
        }
    }
}
VERIFY(0x021F09EC, &daNpc_Ba1_c::privateCut);

/* 021F04DC */
u32 daNpc_Ba1_c::eMove_MOV_POS_() {
    WWHD_FUNC(0x021F04DC, u32, this);
    bool ret = false;
    gabi::Local<cXyz> temp;
    temp->y = 0.0f;
    temp->x = 503.0f;
    temp->z = -91.0f;
    if (cLib_calcTimer(&mEvTimer)) {
        return FALSE;
    }
    if (gabi::ftoi(mEyeOffset) != 0) {
        cLib_chaseF(&mEyeOffset, 0.0f, 2.0f);
        return FALSE;
    }
    /* (temp - current.pos).absXZ() */
    gabi::Local<cXyz> diff;
    cXyz_mi(temp, diff, &current.pos);
    gabi::Local<cXyz> xz;
    f32 dx = diff->x, dz = diff->z;
    xz->x = dx;
    xz->y = 0.0f;
    xz->z = dz;
    f32 mag = std_sqrtf(PSVECSquareMag(xz));
    ret = mag < 4.0f;
    m78A.y = cLib_targetAngleY(&current.pos, &dComIfGp_getPlayer(0)->current.pos);
    m7F5 = true;
    if (ret) {
        mEyeOffset = 0.0f;
        m7F5 = false;
        current.angle.y = m78A.y;
        speedF = 0.0f;
    } else {
        cLib_addCalcAngleS(&current.angle.y, cLib_targetAngleY(&current.pos, temp), 4, 0x400, 0);
        cLib_chaseF(&speedF, 4.0f, 0.4f);
    }
    return ret;
}
VERIFY(0x021F04DC, &daNpc_Ba1_c::eMove_MOV_POS_);

/* 021F0B18 */
void daNpc_Ba1_c::lookBack() {
    WWHD_FUNC(0x021F0B18, void, this);
    gabi::Local<cXyz> dstPos;
    cXyz* dstPos_p;
    mJointBackboneY = m_jnt.mAngles[1][1];
    s16 desiredYrot = current.angle.y;
    mActorAngleY = desiredYrot;
    mJointHeadY = m_jnt.mAngles[0][1];
    dstPos->set(0.0f, 0.0f, 0.0f);
    /* src_pos = current.pos with y = eyePos.y */
    f32 srcX = current.pos.x;
    f32 srcY = eyePos.y;
    f32 srcZ = current.pos.z;
    dstPos_p = nullptr;
    u8 headOnlyFollow = mHeadOnlyFollow;

    switch ((u32)(s32)mLookBackState) {
    case 1: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, -20.0f);
        dstPos->copy(*eye);
        dstPos_p = dstPos;
        srcZ = current.pos.z;
        srcX = current.pos.x;
        srcY = eyePos.y;
        break;
    }
    case 2:
        dstPos->copy(m79C);
        dstPos_p = dstPos;
        srcZ = current.pos.z;
        srcX = current.pos.x;
        srcY = eyePos.y;
        break;
    case 3:
        desiredYrot = mTargetYRot;
        break;
    }
    cLib_addCalcAngleS2(&mLookAtMaxVel, l_HIO().mPrmTbl.mCalcAngleTarget, 4, 0x800);
    if (!m_jnt.mbTrn) { /* !m_jnt.trnChk() */
        mLookAtMaxVel = 0;
    }
    gabi::Local<cXyz> src_pos; /* passed by value: a copy on the stack */
    src_pos->x = srcX;
    src_pos->y = srcY;
    src_pos->z = srcZ;
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos_p, src_pos, desiredYrot, mLookAtMaxVel, headOnlyFollow);
}
VERIFY(0x021F0B18, &daNpc_Ba1_c::lookBack);

/* 021F0D58 */
void daNpc_Ba1_c::event_proc(int i_staff_idx) {
    WWHD_FUNC(0x021F0D58, void, this, i_staff_idx);
    if (dComIfGp_evmng_endCheck(mEventIdTable[mEventIdx])) {
        switch ((u32)(s32)mEventIdx) {
        case 0:
            break;
        case 1:
            if (dComIfGs_checkBottle(dItemNo_HALF_SOUP_BOTTLE_e)) {
                dComIfGs_setBottleItemIn(dItemNo_HALF_SOUP_BOTTLE_e, dItemNo_SOUP_BOTTLE_e);
            } else {
                dComIfGs_setEmptyBottleItemIn(dItemNo_SOUP_BOTTLE_e);
            }
            if (dComIfGs_isEventBit(dSv_event_flag_GRANDMA_HEALED)) {
                m812 = 1;
                m7F0 = 1;
            } else {
                m812 = 5;
            }
            break;
        case 2: {
            dLetter_send(0x9D03);
            dComIfGs_onEventBit(dSv_event_flag_GRANDMA_HEALED);
            gabi::Local<ProcFunc_l> pmf;
            pmf_load(pmf, PMF_wait_action4);
            set_action(pmf, nullptr);
            mInitialPos.copy(current.pos);
            mInitialAngle.x = current.angle.x;
            mInitialAngle.y = current.angle.y;
            mInitialAngle.z = current.angle.z;
            m7FE = 0;
            break;
        }
        case 3:
            break;
        }
        endEvent();
        return;
    } else {
        if (!mEventCut.cutProc()) {
            privateCut(i_staff_idx);
        }
        lookBack();
    }
}
VERIFY(0x021F0D58, &daNpc_Ba1_c::event_proc);

/* 021F1680 */
bool daNpc_Ba1_c::chk_talk() {
    WWHD_FUNC(0x021F1680, bool, this);
    bool ret = true;
    mItemNo = 0xFF;
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd()) {
            mItemNo = dComIfGp_event_getPreItemNo();
        } else {
            ret = false;
        }
        mItemNo = dComIfGp_event_getPreItemNo();
    }
    return ret;
}
VERIFY(0x021F1680, &daNpc_Ba1_c::chk_talk);

/* 021F1714 */
bool daNpc_Ba1_c::chk_drct(f32 i_param_1) {
    WWHD_FUNC(0x021F1714, bool, this, i_param_1);
    s16 target = cLib_targetAngleY(&current.pos, &dComIfGp_getPlayer(0)->current.pos) - current.angle.y;
    int cmp = abs((int)target);
    return cmp < (s16)gabi::ftoi(i_param_1 * 182.04445f); /* cM_deg2s */
}
VERIFY(0x021F1714, &daNpc_Ba1_c::chk_drct);

/* 021F17EC */
u16 daNpc_Ba1_c::next_msgStatus(be<u32>* i_msg_no) {
    WWHD_FUNC(0x021F17EC, u16, this, i_msg_no);
    u16 ret = 0xF;
    /* HD: the message (GameCube mpCurrMsg) is the message manager at *(0x101F4B5C) */
    u32 msg = gabi::load<u32>(0x101F4B5C);
    switch (*i_msg_no) {
    case 0x7EB:
        *i_msg_no = 0x7EC;
        break;
    case 0x7EF:
        *i_msg_no = 0x7F0;
        break;
    case 0x7F3:
        *i_msg_no = 0x7F4;
        break;
    case 0x7F4:
    neverGotSoup:
        /* HD: no checkEmptyBottle test (GameCube: 0x7F5 with an empty bottle, else 0x7F9) */
        *i_msg_no = 0x7F5;
        break;
    case 0x7F6:
        *i_msg_no = 0x7F7;
        break;
    case 0x7FA:
    case 0x7FB:
    case 0x7FC:
    case 0x7FD:
        if (dComIfGs_isGetBottleItem(dItemNo_SOUP_BOTTLE_e)) {
            if (!dComIfGs_checkEmptyBottle()) {
                *i_msg_no = 0x803;
            } else if (dComIfGs_checkBottle(dItemNo_SOUP_BOTTLE_e) || dComIfGs_checkBottle(dItemNo_HALF_SOUP_BOTTLE_e)) {
                /* HD: also with a half-full soup bottle */
                *i_msg_no = 0x7FE;
            } else {
                *i_msg_no = 0x7FF;
            }
            break;
        }
        goto neverGotSoup;
    case 0x7FF:
        switch (gabi::load<u32>(msg + 0x948) /* mSelectNum */) {
        case 0:
            *i_msg_no = 0x800;
            break;
        case 1:
            *i_msg_no = 0x802;
            break;
        }
        break;
    default:
        ret = 0x10;
        break;
    }
    return ret;
}
VERIFY(0x021F17EC, &daNpc_Ba1_c::next_msgStatus);

/* 021F19AC */
u32 daNpc_Ba1_c::getMsg_BA1_0() {
    WWHD_FUNC(0x021F19AC, u32, this);
    if (m7FA) {
        return 0x7d8;
    }
    if (mbHoldEvent) {
        return dComIfGs_isEventBit(dSv_event_flag_UNK_0608) ? 0x7e7 : 0x7e6;
    }
    return m7FB ? 0x7e5 : 0x7e4;
}
VERIFY(0x021F19AC, &daNpc_Ba1_c::getMsg_BA1_0);

/* 021F1A50 */
u32 daNpc_Ba1_c::getMsg_BA1_1() {
    WWHD_FUNC(0x021F1A50, u32, this);
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0E20)) {
        if (dComIfGs_checkCollect(1)) {
            return 0x80b;
        }
        return dComIfGs_isEventBit(dSv_event_flag_UNK_0740) ? 0x7ee : 0x7ef;
    }
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0780)) {
        return 0x7ed;
    }
    if (dComIfGs_checkCollect(0)) {
        return dComIfGs_isEventBit(dSv_event_flag_UNK_0602) ? 0x7ea : 0x7eb;
    }
    return dComIfGs_isEventBit(dSv_event_flag_UNK_0601) ? 0x7e9 : 0x7e8;
}
VERIFY(0x021F1A50, &daNpc_Ba1_c::getMsg_BA1_1);

/* 021F1B70 */
u32 daNpc_Ba1_c::getMsg_BA1_3() {
    WWHD_FUNC(0x021F1B70, u32, this);
    if (m7F0) {
        if (dComIfGs_isEventBit(dSv_event_flag_GRANDMA_HEALED)) {
            return 0x801;
        }
        return 0x7f6;
    }
    if (dComIfGs_isEventBit(dSv_event_flag_GRANDMA_HEALED)) {
        dComIfGs_setEventReg(0xA60F /* dSv_event_flag_c::UNK_A60F */, 0);
        if (m7F2 >= 3) {
            if (m7FA) {
                return 0x7fd;
            }
            m7FA = true;
            return 0x7fc;
        }
        if (m7FA) {
            return 0x7fa;
        }
        m7FA = true;
        return 0x7fb;
    }
    return 0x7f1;
}
VERIFY(0x021F1B70, &daNpc_Ba1_c::getMsg_BA1_3);

/* 021F1DA8 */
bool daNpc_Ba1_c::check_useFairyArea() {
    WWHD_FUNC(0x021F1DA8, bool, this);
    gabi::Local<cXyz> diff;
    cXyz_mi(&dComIfGp_getPlayer(0)->current.pos, diff, &current.pos);
    gabi::Local<cXyz> xz;
    f32 dx = diff->x, dz = diff->z;
    xz->y = 0.0f;
    xz->x = dx;
    xz->z = dz;
    f32 mag = std_sqrtf(PSVECSquareMag(xz));
    bool ret = false;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if (std::fabs(player->current.pos.y - current.pos.y) < 100.0f && mag < l_HIO().mPrmTbl.m1C) {
        ret = true;
    }
    return ret;
}
VERIFY(0x021F1DA8, &daNpc_Ba1_c::check_useFairyArea);

/* 021F1F0C */
BOOL daNpc_Ba1_c::wait_0() {
    WWHD_FUNC(0x021F1F0C, BOOL, this);
    cLib_addCalcAngleS(&current.angle.y, mInitialAngle.y, 4, 0x800, 0);
    if (m812 == 1 || m812 >= 3) {
        return TRUE;
    }
    m812 = 0;
    if (!dComIfGs_isTmpBit(0x0310 /* dSv_event_tmp_flag_c::UNK_0310 */)) {
        gabi::Local<cXyz> diff;
        cXyz_mi(&dComIfGp_getPlayer(0)->current.pos, diff, &current.pos);
        gabi::Local<cXyz> xz;
        f32 dx = diff->x, dz = diff->z;
        xz->y = 0.0f;
        xz->x = dx;
        xz->z = dz;
        f32 dist = std_sqrtf(PSVECSquareMag(xz));
        if (std::fabs(dComIfGp_getPlayer(0)->current.pos.y - current.pos.y) < 100.0f && dist < l_HIO().mPrmTbl.m20) {
            dComIfGs_onTmpBit(0x0310);
            m812 = 6;
        }
    }
    return TRUE;
}
VERIFY(0x021F1F0C, &daNpc_Ba1_c::wait_0);

/* 021F2040 */
BOOL daNpc_Ba1_c::wait_1() {
    WWHD_FUNC(0x021F2040, BOOL, this);
    cLib_addCalcAngleS(&current.angle.y, mInitialAngle.y, 4, 0x800, 0);
    if (m812 == 1 || m812 >= 3) {
        return TRUE;
    }
    if (m809) {
        if (chk_talk()) {
            setStt(2);
            mHeadOnlyFollow = false;
        }
        return TRUE;
    } else if (mbHoldEvent) {
        setAnm_NUM(4, 1);
        m812 = 1;
        mLookBackState = 0;
        mTargetYRot = current.angle.y + -0x638e;
    } else {
        m812 = 2;
        if (mbAttention != 0) {
            mEvTimer2 = 60;
        }
        if (cLib_calcTimer(&mEvTimer2) && chk_drct(61.0f)) {
            mLookBackState = 1;
        } else {
            mLookBackState = 3;
            mTargetYRot = mInitialAngle.y;
            m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        }
    }
    return TRUE;
}
VERIFY(0x021F2040, &daNpc_Ba1_c::wait_1);

/* 021F217C */
BOOL daNpc_Ba1_c::talk_1() {
    WWHD_FUNC(0x021F217C, BOOL, this);
    u8 ret = chk_partsNotMove();
    if (mAnmNum == 4) {
        cLib_addCalcAngleS(&current.angle.y, mTargetYRot, 2, 0x1000, 1);
        if (!mbMorfAnimStopped) {
            return TRUE;
        }
    }
    talk(1);
    /* HD: the message status comes from the message manager (GameCube: mpCurrMsg->mStatus) */
    if (mbHasMsg) {
        if (fopMsgM_getStatus() == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
            switch (mCurrMsgNo) {
            case 0x7e4:
                m7FB = true;
                break;
            case 0x7e6:
                dComIfGs_onEventBit(dSv_event_flag_UNK_0608);
                break;
            case 0x7e8:
                dComIfGs_onEventBit(dSv_event_flag_UNK_0601);
                break;
            case 0x7ec:
                dComIfGs_onEventBit(dSv_event_flag_UNK_0602);
                break;
            case 0x7f0:
                dComIfGs_onEventBit(dSv_event_flag_UNK_0740);
                break;
            case 0x800:
                /* GameCube: if (mCurrMsgNo != 0x7f9), always true here */
                m812 = 4;
                mFairyUsed = false;
                break;
            case 0x801:
                m7F0 = 0;
                break;
            }
            mItemNo = 0xff;
            m809 = false;
            setStt(mPrevStatus);
            mEvTimer2 = 60;
            endEvent();
            mbHoldEvent = false;
        }
    }
    return ret;
}
VERIFY(0x021F217C, &daNpc_Ba1_c::talk_1);

/* 021F24B8 */
BOOL daNpc_Ba1_c::wait_2() {
    WWHD_FUNC(0x021F24B8, BOOL, this);
    cLib_addCalcAngleS(&current.angle.y, mInitialAngle.y, 4, 0x800, 0);
    if (m812 == 1 || m812 >= 3) {
        return TRUE;
    }
    if (m809) {
        if (m7FF) {
            dComIfGp_setNextStage(STR(0x10016B04) /* "LinkRM" */, 201, 0, 9, 0.0f, 0, 1, 0);
            return TRUE;
        }
        if (chk_talk()) {
            setStt(2);
            mHeadOnlyFollow = false;
        }
        return TRUE;
    } else if (!dComIfGs_checkCollect(1) && dComIfGs_isEventBit(dSv_event_flag_UNK_3202)) {
        mInitialAngle.x = current.angle.x;
        current.pos.x = -290.0f;
        current.pos.y = 0.0f;
        current.pos.z = 110.0f;
        mInitialAngle.z = current.angle.z;
        mInitialPos.copy(current.pos);
        mInitialAngle.y = -0x8000;
        current.angle.y = -0x8000; /* current.angle.y = -0x8000; mInitialAngle = current.angle */
        if ((dComIfGp_getPlayer(0)->current.pos.y - current.pos.y) < 1.0f) {
            m7FF = 1;
            mTargetYRot = mInitialAngle.y;
            m812 = 1;
            mLookBackState = 3;
            m_jnt.mbTrn = 1;
            return TRUE;
        }
        mLookBackState = 3;
        mTargetYRot = mInitialAngle.y;
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        return TRUE;
    }
    m812 = 2;
    if (mbAttention) {
        mEvTimer2 = 60;
    }
    if (cLib_calcTimer(&mEvTimer2) && chk_drct(61.0f)) {
        mLookBackState = 1;
    } else {
        mLookBackState = 3;
        mTargetYRot = mInitialAngle.y;
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
    }
    return TRUE;
}
VERIFY(0x021F24B8, &daNpc_Ba1_c::wait_2);

/* 021F26D4 */
BOOL daNpc_Ba1_c::ZZZwai() {
    WWHD_FUNC(0x021F26D4, BOOL, this);
    if (m812 == 1 || m812 >= 3) {
        return TRUE;
    }
    if (mFairyUsed) {
        mFairyUsed = false;
        m812 = 4;
        return TRUE;
    }
    if (m809) {
        if (chk_talk()) {
            setStt(5);
        }
        return TRUE;
    }
    if (dComIfGs_isEventBit(dSv_event_flag_GRANDMA_HEALED)) {
        m812 = 2;
        if (mbAttention) {
            mEvTimer2 = 60;
        }
        if (cLib_calcTimer(&mEvTimer2) && chk_drct(61.0f)) {
            mLookBackState = 1;
        } else {
            mLookBackState = 3;
            mTargetYRot = mInitialAngle.y;
            m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        }
    } else {
        m812 = 0;
        if (check_useFairyArea()) {
            m812 = 2;
        }
    }
    return TRUE;
}
VERIFY(0x021F26D4, &daNpc_Ba1_c::ZZZwai);

/* 021F2818 */
BOOL daNpc_Ba1_c::wait_action1(void*) {
    WWHD_FUNC(0x021F2818, BOOL, this, (void*)nullptr);
    switch ((u32)(s32)m818) {
    case 0:
        if (dComIfGs_isEventBit(dSv_event_flag_UNK_2A80)) {
            m7FA = dComIfGs_isTmpBit(0x0310) != 0;
            if (m7FA) {
                dComIfGs_offTmpBit(0x0310);
            }
            setStt(1);
            m818 = m818 + 1;
        } else {
            if (dComIfGs_isTmpBit(0x0310)) {
                setStt(1);
                m818 = m818 + 1;
            } else {
                setStt(7);
                m818 = m818 + 1;
            }
        }
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 7:
            mbSetEyePos = wait_0();
            break;
        case 1:
            mbSetEyePos = wait_1();
            break;
        case 2:
            mbSetEyePos = talk_1();
            break;
        }
        lookBack();
    default:
        break;
    }
    return TRUE;
}
VERIFY(0x021F2818, &daNpc_Ba1_c::wait_action1);

/* 021F2980 */
BOOL daNpc_Ba1_c::wait_action2(void*) {
    WWHD_FUNC(0x021F2980, BOOL, this, (void*)nullptr);
    switch ((u32)(s32)m818) {
    case 0:
        setStt(3);
        m818 = m818 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 3:
            mbSetEyePos = wait_2();
            break;
        case 2:
            mbSetEyePos = talk_1();
            break;
        }
        lookBack();
        break;
    default:
        break;
    }
    return TRUE;
}
VERIFY(0x021F2980, &daNpc_Ba1_c::wait_action2);

/* 021F2A30 */
BOOL daNpc_Ba1_c::demo_action1(void*) {
    WWHD_FUNC(0x021F2A30, BOOL, this, (void*)nullptr);
    switch (m818) {
    case 9:
        break;
    case 0:
        m818 = m818 + 1;
    case 1:
    case 2:
    case 3:
        break;
    }
    return TRUE;
}
VERIFY(0x021F2A30, &daNpc_Ba1_c::demo_action1);

/* 021F2A4C */
BOOL daNpc_Ba1_c::wait_action3(void*) {
    WWHD_FUNC(0x021F2A4C, BOOL, this, (void*)nullptr);
    switch ((u32)(s32)m818) {
    case 0:
        setStt(4);
        m818 = m818 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 4:
            mbSetEyePos = ZZZwai();
            break;
        case 5:
            mbSetEyePos = talk_2();
            break;
        }
        lookBack();
        break;
    default:
        break;
    }
    return TRUE;
}
VERIFY(0x021F2A4C, &daNpc_Ba1_c::wait_action3);

/* 021F2B08 */
BOOL daNpc_Ba1_c::wait_action4(void*) {
    WWHD_FUNC(0x021F2B08, BOOL, this, (void*)nullptr);
    switch ((u32)(s32)m818) {
    case 0:
        setStt(6);
        m818 = m818 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 6:
            mbSetEyePos = wait_3();
            break;
        case 2:
            mbSetEyePos = talk_1();
            break;
        }
        lookBack();
        break;
    default:
        break;
    }
    return TRUE;
}
VERIFY(0x021F2B08, &daNpc_Ba1_c::wait_action4);

/* 021EE764 */
J3DModelData* daNpc_Ba1_c::create_Anm() {
    WWHD_FUNC(0x021EE764, J3DModelData*, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x1001691C), 0xA /* dRes_ID_BA_BDL_BA_e */);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2869, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x10016928), 0xB35, STR(0x10016938));
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(STR(0x1001691C), 7 /* dRes_ID_BA_BCK_WAIT01_e */);
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, anm, 2 /* J3DFrameCtrl::EMode_LOOP */, 1.0f, 0, -1, 1,
                                    nullptr, 0x80000, 0x11020022);
    if (mpMorf.get() == nullptr) {
        return nullptr;
    }
    if (mpMorf->getModel() == nullptr) {
        /* delete mpMorf (HD: virtual deleting destructor, vtable at +0, slot +0xC) */
        mDoExt_McaMorf* morf = mpMorf;
        if (morf != nullptr) {
            u32 vt = gabi::load<u32>(gabi::ea(morf));
            gabi::call_ptr(gabi::load<u32>(vt + 0xC), morf, 3);
        }
        mpMorf = nullptr;
        return nullptr;
    }
    m_hed_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10016920) /* "head" */);
    if (m_hed_jnt_num < 0) /* JUT_ASSERT(2891, m_hed_jnt_num >= 0) */
        JUT_ASSERT_fail(STR(0x10016928), 0xB4B, STR(0x1001694C));
    m_bbone_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10016960) /* "backbone" */);
    if (m_bbone_jnt_num < 0) /* JUT_ASSERT(2894, m_bbone_jnt_num >= 0) */
        JUT_ASSERT_fail(STR(0x10016928), 0xB4E, STR(0x1001696C));
    m_footL_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10016914) /* "footL" */);
    if (m_footL_jnt_num < 0) /* JUT_ASSERT(2897, m_footL_jnt_num >= 0) */
        JUT_ASSERT_fail(STR(0x10016928), 0xB51, STR(0x10016984));
    return a_mdl_dat;
}
VERIFY(0x021EE764, &daNpc_Ba1_c::create_Anm);

/* 021EEB48 */
BOOL daNpc_Ba1_c::CreateHeap() {
    WWHD_FUNC(0x021EEB48, BOOL, this);
    J3DModelData* anm_model = create_Anm();
    if (!anm_model) {
        return FALSE;
    }
    mBtpNum = 1;
    if (!iniTexPttrnAnm(false)) {
        mpMorf = nullptr;
        return FALSE;
    }
    if (create_itm_Mdl()) {
        for (u16 i = 0; i < J3DModelData_getJointNum(anm_model); i++) {
            if ((i == (u32)(s32)m_hed_jnt_num) || (i == (u32)(s32)m_bbone_jnt_num)) {
                /* mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack_Ba1) */
                J3DModelData* md = J3DModel_getModelData_l(mpMorf->getModel());
                u32 n = gabi::load<u32>(gabi::ea(md) + 4);
                u32 joint = gabi::load<u32>(gabi::ea(md) + 8);
                if (i < n)
                    joint += i * 0x1C;
                gabi::store<u32>(joint + 8, 0x021EE6CC /* nodeCallBack_Ba1 */);
            }
        }
        gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
        mAcchCir.SetWall(30.0f, 50.0f);
        mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
        return TRUE;
    }
    mpMorf = nullptr;
    return FALSE;
}
VERIFY(0x021EEB48, &daNpc_Ba1_c::CreateHeap);

/* 021EF2A0 */
void daNpc_Ba1_c::setMtx(u32 param_1) {
    WWHD_FUNC(0x021EF2A0, void, this, param_1);
    if (!mbInDemo) {
        plyTexPttrnAnm();
        mbMorfAnimStopped = (s8)mpMorf->play(&eyePos, 0, 0);
        if (mpMorf->getFrame() < mPrevMorfFrame) {
            mbMorfAnimStopped = true;
        }
        mPrevMorfFrame = mpMorf->getFrame();
        if (mSpecificType != 3) {
            mObjAcch.CrrPos(dComIfG_Bgsp());
        }
    }
    /* the ground polygon: cBgS_PolyInfo at m_gnd + 0x14 */
    void* gndPoly = gabi::at<u8>(gabi::ea(&mObjAcch) + 0xD4 + 0x14);
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), gndPoly);
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), gndPoly)); /* tevStr.mEnvrIdxOverride */

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(m78A.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();

    if (mpClothModel.get() != nullptr) {
        J3DModel_setBaseTRMtx(mpClothModel, getAnmMtx(mpMorf->getModel(), m_footL_jnt_num));
        J3DModel_calc(mpClothModel);
    }
    setAttention(param_1);
}
VERIFY(0x021EF2A0, &daNpc_Ba1_c::setMtx);

/* 021EF4BC */
bool daNpc_Ba1_c::createInit() {
    WWHD_FUNC(0x021EF4BC, bool, this);
    /* l_evn_tbl (.data 0x101BB85C): "Use_Fairy", "Ba1_Get_Itm", "Ganbaru", "tale_1", "None", "tale_2" */
    for (int i = 0; i < 6; i++) {
        const char* name = gabi::at<const char>(gabi::load<u32>(0x101BB85C + i * 4));
        mEventIdTable[i] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    }
    gravity = -4.5f;
    m79C.copy(current.pos);
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 171);  /* attention_info.distances[SPEAK] */
    gabi::store<u8>(gabi::ea(this) + 0x389, 171);  /* attention_info.distances[TALK] */
    mEventCut.setActorInfo2(STR(0x10016A38) /* "Ba1" */, (fopNpc_npc_c*)(void*)this);
    mAnmNum = 10;
    bool init_success;
    switch (mSpecificType) {
    case 0:
        init_success = init_BA1_0();
        break;
    case 1:
        init_success = init_BA1_1();
        break;
    case 2:
        init_success = init_BA1_2();
        break;
    case 3:
        init_success = init_BA1_3();
        break;
    case 4:
        init_success = init_BA1_4();
        break;
    default:
        init_success = false;
        break;
    }
    if (init_success) {
        m78A.x = current.angle.x;
        m78A.y = current.angle.y;
        m78A.z = current.angle.z;
        shape_angle.x = m78A.x;
        shape_angle.y = m78A.y;
        shape_angle.z = m78A.z;
    } else {
        return false;
    }
    m7F2 = dSv_event_getEventReg(dComIfGs_event(), 0xA60F /* dSv_event_flag_c::UNK_A60F */);
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mpMorf->setMorf(0.0f);
    setMtx(true);
    return true;
}
VERIFY(0x021EF4BC, &daNpc_Ba1_c::createInit);

/* 021EF978 */
/* returns the mbInDemo byte as is (typed u8) */
u8 daNpc_Ba1_c::demo() {
    WWHD_FUNC(0x021EF978, u8, this);
    if (demoActorID == 0) {
        if (mbInDemo != 0) {
            mbInDemo = 0;
        }
        return mbInDemo;
    }
    u8 id = demoActorID;
    mbInDemo = 1;
    /* dComIfGp_demo_getActor(demoActorID): HD inline with a range check and the demo object
     * (0x101D5FFC) asserted */
    void* demo_actor = nullptr;
    if (id != 0 && id <= 0x20) {
        u32 obj = gabi::load<u32>(0x101D5FFC);
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail(STR(0x100168F8), 0x23A, STR(0x100168D0));
            obj = gabi::load<u32>(0x101D5FFC);
        }
        demo_actor = gabi::call<void*>(0x02526E70, obj, id); /* dDemo_object_c::getActor */
    }
    if (m_hed_tex_pttrn.get() != nullptr) {
        u8 frame = (u8)(mBlinkFrame + 1);
        mBlinkFrame = frame;
        if ((s32)frame >= J3DAnm_getFrameMax(m_hed_tex_pttrn)) {
            mBlinkFrame = (u8)J3DAnm_getFrameMax(m_hed_tex_pttrn);
        }
    }
    /* HD: demo_actor is checked for NULL */
    if (demo_actor != nullptr) {
        J3DAnmTexPattern* demopattern = gabi::call<J3DAnmTexPattern*>(0x02527828, demo_actor, STR(0x10016A4E) /* "Ba" */);
        if (demopattern != nullptr) {
            m_hed_tex_pttrn = demopattern;
            J3DModelData* md = J3DModel_getModelData_l(mpMorf->getModel());
            if (mDoExt_btpAnm_init(mHeadBtpAnm, md, demopattern, 1, 2, 1.0f, 0, -1, 1, 0)) {
                mBlinkFrame = 0;
                mBtpNum = 0xC; /* dRes_ID_BA_BTP_BA_T02_e */
            }
        }
    }
    /* HD: in the stage "Demo01" (start stage name at 0x1047E6B8) with 0x101D600C in
     * [0x14A, 0x181), the demo data flags are 0x28 instead of 0x6A */
    u8 flags = 0x6A;
    {
        gabi::Local<SafeString> a;
        gabi::Local<SafeString> b;
        a->__vtbl = SAFESTRING_VTBL;
        b->__vtbl = SAFESTRING_VTBL;
        b->mStringTop = 0x1047E6B8;
        a->mStringTop = 0x10016A54; /* "Demo01" */
        gabi::call(0x021F2D74, a.get()); /* sead::SafeString::assureTerminationImpl_ (empty) */
        gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
        u32 s1 = a->mStringTop;
        gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b.get());
        u32 s2 = b->mStringTop;
        bool equal = true;
        if (s1 != s2) {
            equal = false;
            for (u32 i = 0; i < 0x40001; i++) {
                u8 c1 = gabi::load<u8>(s1 + i);
                u8 c2 = gabi::load<u8>(s2 + i);
                if (c1 != c2)
                    break;
                if (c1 == 0) {
                    equal = true;
                    break;
                }
            }
        }
        if (equal && (u32)(gabi::load<u32>(0x101D600C) - 0x14A) < 0x37) {
            flags = 0x28;
        }
    }
    dDemo_setDemoData(this, flags, mpMorf, STR(0x10016A4E) /* "Ba" */, 0, nullptr, 0, 0);
    m78A.x = current.angle.x;
    m78A.y = current.angle.y;
    m78A.z = current.angle.z;
    shape_angle.x = m78A.x;
    shape_angle.y = m78A.y;
    shape_angle.z = m78A.z;
    return mbInDemo;
}
VERIFY(0x021EF978, &daNpc_Ba1_c::demo);

/* 021F128C */
BOOL daNpc_Ba1_c::_draw() {
    WWHD_FUNC(0x021F128C, BOOL, this);
    J3DModel* morf_model = mpMorf->getModel();
    J3DModelData* model_data = J3DModel_getModelData_l(morf_model);
    if (mbInitGrandma0 || m7F7) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model, &tevStr);
    dComIfGp_get(); /* HD: an unused accessor call */
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mHeadBtpAnm, model_data, mBlinkFrame);
    mpMorf->entryDL();
    gabi::store<u32>(gabi::ea(model_data) + 0x38, 0); /* mHeadBtpAnm.remove(model_data) */
    if (mpClothModel.get() != nullptr) {
        setLightTevColorType(dKy_getEnvlight(), mpClothModel, &tevStr);
        mDoExt_modelEntryDL(mpClothModel);
    }
    /* HD: no shadowDraw() */
    dSnap_RegistFig(0x4A /* DSNAP_TYPE_NPC_BA1 */, this, 1.0f, 1.0f, 1.0f);

    /* debug leftovers: function-local static colors initialised on first use */
    if (gabi::load<u8>(0x10465CFC) /* l_HIO.mPrmTbl.m18 */ != 0) {
        if (gabi::load<u32>(0x101FDA50) == 0) {
            gabi::store<u32>(0x101FDA50, 1);
            memcpy_g(gabi::at<u8>(0x101FEBF4), gabi::at<u8>(0x10016860), 4); /* 028FEAC0 memcpy */
        }
        if (gabi::load<u32>(0x101FDAC0) == 0) {
            gabi::store<u32>(0x101FDAC0, 1);
            memcpy_g(gabi::at<u8>(0x101FEBF8), gabi::at<u8>(0x10016864), 4); /* 028FEAC0 memcpy */
        }
        if (mSpecificType == 0) {
            if (gabi::load<u32>(0x101FDA44) != 0)
                return TRUE;
            gabi::store<u32>(0x101FDA44, 1);
            memcpy_g(gabi::at<u8>(0x101FEBE8), gabi::at<u8>(0x10016868), 4); /* 028FEAC0 memcpy */
        }
        if (mSpecificType == 3) {
            if (gabi::load<u32>(0x101FDA44) == 0) {
                gabi::store<u32>(0x101FDA44, 1);
                memcpy_g(gabi::at<u8>(0x101FEBE8), gabi::at<u8>(0x10016868), 4); /* 028FEAC0 memcpy */
            }
        }
    }
    return TRUE;
}
VERIFY(0x021F128C, &daNpc_Ba1_c::_draw);

/* 021F1064 */
BOOL daNpc_Ba1_c::_execute() {
    WWHD_FUNC(0x021F1064, BOOL, this);
    if (!mbRanExecute) {
        mInitialAngle.y = current.angle.y;
        mInitialAngle.z = current.angle.z;
        mInitialPos.copy(current.pos);
        mInitialAngle.x = current.angle.x;
        mbRanExecute = true;
    }
    daNpc_Ba1_HIO_c::hio_prm_c& prm = l_HIO().mPrmTbl;
    m_jnt.setParam(prm.mMaxBackboneX, prm.mMaxBackboneY, prm.mMinBackboneX, prm.mMinBackboneY, prm.mMaxHeadX,
                   prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, prm.mMaxTurnStep);
    if (mbInitGrandma0 && demoActorID == 0) {
        return TRUE;
    }
    m7F6 = false;
    mbInitGrandma0 = false;
    partner_srch();
    checkOrder();
    if (!demo()) {
        s32 cond = -1;
        if (dComIfGp_event_runCheck() && checkCommandTalk() == false) {
            cond = isEventEntry();
        }
        if (cond >= 0) {
            mbSetEyePos = 1;
            event_proc(cond);
        } else {
            pmf_call(this, &mCurrProcFunc, nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        if (!m7F6) {
            fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
        }
        if (!m7F5) {
            m78A.x = current.angle.x;
            m78A.y = current.angle.y;
            m78A.z = current.angle.z;
            shape_angle.x = m78A.x;
            shape_angle.y = m78A.y;
            shape_angle.z = m78A.z;
        }
    }
    eventOrder();
    setMtx(false);
    if (!mbInDemo && !m7FE) {
        f32 radius = 50.0f;
        f32 height = 110.0f;
        if (m7FD) {
            radius = 30.0f;
        }
        setCollision(radius, height);
    }
    return TRUE;
}
VERIFY(0x021F1064, &daNpc_Ba1_c::_execute);

/* 021F0F78 */
void daNpc_Ba1_c::eventOrder() {
    WWHD_FUNC(0x021F0F78, void, this);
    s8 condition = m812;
    if (condition == 1 || condition == 2) {
        u32 a = gabi::ea(this) + 0xFA; /* eventInfo.mCondition */
        gabi::store<u16>(a, gabi::load<u16>(a) | 1); /* onCondition(dEvtCnd_CANTALK_e) */
        if (mSpecificType == 3 && !dComIfGs_isEventBit(dSv_event_flag_GRANDMA_HEALED)) {
            gabi::store<u16>(a, gabi::load<u16>(a) | 0x20); /* onCondition(dEvtCnd_CANTALKITEM_e) */
        }
        if (m812 == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (condition >= 3) {
        mEventIdx = condition - 3;
        fopAcM_orderOtherEventId(this, mEventIdTable[mEventIdx], 0xFF, 0xFFFF, 0, 1);
    }
}
VERIFY(0x021F0F78, &daNpc_Ba1_c::eventOrder);

/* 021EF6AC */
cPhs_State daNpc_Ba1_c::_create() {
    WWHD_FUNC(0x021EF6AC, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Ba1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025A1458, this); /* fopNpc_npc_c::fopNpc_npc_c (the matcher calls it cDyl_LinkASync) */
            __vtbl = BA1_VTBL;
            gabi::call(0x025E7820, mHeadBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            gabi::call(0x0259F740, &mEventCut);  /* dNpc_EventCut_c::dNpc_EventCut_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, STR(0x10016A48) /* "Ba" */);
    if (state != cPhs_COMPLEATE_e) {
        return state;
    }
    if (!charDecide(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    /* static int a_size_tbl[] = {0x272E0} (.data 0x101BB894) */
    if (!fopAcM_entrySolidHeap(this, 0x021EECB8 /* CheckCreateHeap */, gabi::load<u32>(0x101BB894) /* a_size_tbl[mType]: HD reads [0] */)) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -50.0f, -20.0f, -50.0f, 50.0f, 120.0f, 50.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}
VERIFY(0x021EF6AC, &daNpc_Ba1_c::_create);

/* 021F2BB8 daNpc_Ba1_HIO_c::daNpc_Ba1_HIO_c (HD: allocates when this == NULL) */
static daNpc_Ba1_HIO_c* daNpc_Ba1_HIO_c_ct(daNpc_Ba1_HIO_c* i_this) {
    WWHD_FUNC(0x021F2BB8, daNpc_Ba1_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Ba1_HIO_c*)operator_new(0x30);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x100168C0;
    /* memcpy(&mPrmTbl, &a_prm_tbl (.data 0x101BBB20), sizeof(hio_prm_c)) */
    memcpy_g(&i_this->mPrmTbl, gabi::at<u8>(0x101BBB20), 0x24); /* 028FEAC0 memcpy */
    i_this->mNo = -1;
    i_this->field_0x8 = -1;
    return i_this;
}
VERIFY(0x021F2BB8, daNpc_Ba1_HIO_c_ct);

/* 021F2C24: static initialisation of the translation unit */
static void __sinit_d_a_npc_ba1_cpp() {
    WWHD_FUNC(0x021F2C24, void, (u32)0);
    /* header statics (as sinit_header_statics, but the zeroed object is at 0x10465D08) */
    const u32 P = 0x10465CCC, D = 0x101BBB44;
    for (int i = 0; i < 4; i++) gabi::store<u32>(0x10465D08 + 4 * i, 0);
    __register_global_object(D);
    gabi::store<f32>(P, -3.1415927f);
    gabi::store<f32>(P + 4, 3.1415927f);
    gabi::call(0x028ED6F8, P + 8);
    __register_global_object(D + 0xC);
    gabi::call(0x028EAB2C, P + 9);
    __register_global_object(D + 0x18);
    daNpc_Ba1_HIO_c_ct(&l_HIO()); /* static daNpc_Ba1_HIO_c l_HIO */
}
VERIFY(0x021F2C24, __sinit_d_a_npc_ba1_cpp);

/* 021F2CC4: sead::SafeString deleting destructor (this TU's copy; vtable 0x10016888 slot +0xC) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x021F2CC4, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x021F2CC4, SafeString_dt);

/* 021F2CD8: daNpc_Ba1_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Ba1_c_dt(daNpc_Ba1_c* i_this, s32 flags) {
    WWHD_FUNC(0x021F2CD8, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        /* ~dBgS_ObjAcch: this TU's vtables of its sub-objects, then dBgS_Acch::~dBgS_Acch */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x100168A0);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x100168B0);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021F2CD8, daNpc_Ba1_c_dt);

/* 021F2D74: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x021F2D74, void, (SafeString*)nullptr);
}
VERIFY(0x021F2D74, SafeString_assureTerminationImpl);

