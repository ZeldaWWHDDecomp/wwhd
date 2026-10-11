/**
 * d_a_npc_ba1.cpp (WWHD)
 * NPC - Link's Grandma
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_ba1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_ba1.h"
#include "gc_api/gc_api.h"

const gc_tu_data gc_tu = {
    .safeString = 0x10016888,
    .objAcchDt = {0x100168A0,0x100168B0},
    .headerStatics = {0x10465CCC,0x101BBB44,0x10465D08},
};

BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7);

u8 dBgS_GetPolyColor(dBgS* bgs, void* poly);

s8 dBgS_GetRoomId(dBgS* bgs, void* poly);

s8 JUTNameTab_getIndex(u32 tab, const char* name);

void dLetter_send(u16 no);

void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly);

void dComIfGp_setNextStage(const char* stage, s16 point, s8 roomNo, s8 layer, f32 lastSpeed, u32 lastMode, s32 setPoint, s8 wipe);

fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i);

fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i);

bool dAttention_LockonTruth(dAttention_c* a);

s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate,
                                     s32 start, s32 end, u32 modify, s32 entry);

static daNpc_Ba1_HIO_c l_HIO = {};
static s32 l_check_wrk = 0;
using Ba1ActorPointer = gptr<fopAc_ac_c>;
static Ba1ActorPointer l_check_inf[20] = {};
static cXyz a_eye_pos_off = {};
static u32 eyeOffsetInitialized = 0;
static u32 l_messageManager = 0;
static u32 l_demoObject = 0;
static u32 l_demoFrame = 0;
static char l_startStageName[8] = {};
u32 ba1_checkItemResult(void* self, int itemButton);
u32 ba1_setBtpResult(daNpc_Ba1_c* self, u32 modify, s32 btp);
void ba1_searchByID(fpc_ProcID id, gptr<fopAc_ac_c>* out);
void* ba1_getDemoActor(u32 object, u8 id);
J3DAnmTexPattern* ba1_getDemoBtp(void* actor, const char* arc);
void ba1_assureString(SafeString* string);
void ba1_constructEventCut(dNpc_EventCut_c* cut);

static Ba1ActionDescriptor l_wait_action1 = {&daNpc_Ba1_c::wait_action1};
static Ba1ActionDescriptor l_wait_action2 = {&daNpc_Ba1_c::wait_action2};
static Ba1ActionDescriptor l_demo_action1 = {&daNpc_Ba1_c::demo_action1};
static Ba1ActionDescriptor l_wait_action3 = {&daNpc_Ba1_c::wait_action3};
static Ba1ActionDescriptor l_wait_action4 = {&daNpc_Ba1_c::wait_action4};
static daNpc_Ba1_HIO_c::hio_prm_c a_prm_tbl = {
    5460, 8192, -5000, -8192,
    0, 4096, 0, -4096,
    1024, 1024, 115.0f,
    0, {0, 0, 0}, 500.0f, 300.0f,
};
static u32 debugRedInitialized = 0;
static u32 debugBlueInitialized = 0;
static u32 debugYellowInitialized = 0;
static u32 debugRed = 0x00000000;
static u32 debugBlue = 0x00000000;
static u32 debugYellow = 0x00000000;
static u32 a_debugRed = 0xFF000080;
static u32 a_debugBlue = 0x0000FF80;
static u32 a_debugYellow = 0xFFFF0080;
static u32 a_size_tbl[1] = {0x272E0};

#define SAFESTRING_VTBL gc_tu.safeString
#define BA1_VTBL 0x10016B18

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



u32 ba1_getMessageStatus(u32 manager);

static s32 a_btp_resID_tbl[12] = {22, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
static s32 a_bck_resID_tbl[10] = {7, 3, 4, 5, 1, 8, 6, 2, 0, 23};
static daNpc_Ba1_c::anm_prm_c a_anm_num_prm_tbl[11] = {
    {0, 0, 0, 8.0f, 1.0f, 2},
    {1, 0, 0, 8.0f, 1.0f, 2},
    {2, 0, 0, 8.0f, 1.0f, 2},
    {3, 0, 0, 8.0f, 1.0f, 2},
    {4, 0, 0, 8.0f, 1.0f, 0},
    {5, 0, 0, 8.0f, 1.0f, 2},
    {6, 0, 0, 8.0f, 1.0f, 2},
    {7, 0, 0, 8.0f, 1.0f, 2},
    {8, 0, 0, 8.0f, 1.0f, 2},
    {9, 11, 0, 8.0f, 1.0f, 0},
    {0, 9, 0, 0.0f, 1.0f, 2},
};
static daNpc_Ba1_c::anm_prm_c a_anm_status_prm_tbl[8] = {
    {-1, -1, 0, 0.0f, 0.0f, -1},
    {0, 0, 0, 8.0f, 1.0f, 2},
    {-1, -1, 0, 0.0f, 0.0f, -1},
    {0, 2, 0, 8.0f, 1.0f, 2},
    {7, 7, 0, 8.0f, 1.0f, 2},
    {-1, -1, 0, 0.0f, 0.0f, -1},
    {0, 9, 0, 8.0f, 1.0f, 2},
    {0, 0, 0, 8.0f, 1.0f, 2},
};
static daNpc_Ba1_c::anm_prm_c a_anm_atr_prm_tbl[21] = {
    {0, 0, 0, 8.0f, 1.0f, 2},
    {1, 0, 0, 8.0f, 1.0f, 2},
    {2, 0, 0, 8.0f, 1.0f, 2},
    {3, 0, 0, 8.0f, 1.0f, 2},
    {0, 1, 0, 8.0f, 1.0f, 2},
    {2, 2, 0, 8.0f, 1.0f, 2},
    {1, 3, 0, 8.0f, 1.0f, 2},
    {3, 2, 0, 8.0f, 1.0f, 2},
    {2, 3, 0, 8.0f, 1.0f, 2},
    {2, 4, 0, 8.0f, 1.0f, 2},
    {2, 6, 0, 8.0f, 1.0f, 2},
    {3, 5, 0, 8.0f, 1.0f, 2},
    {3, 6, 0, 8.0f, 1.0f, 2},
    {7, 7, 0, 8.0f, 1.0f, 2},
    {6, 8, 0, 8.0f, 1.0f, 2},
    {6, 1, 0, 8.0f, 1.0f, 2},
    {5, 10, 0, 8.0f, 1.0f, 2},
    {5, 1, 0, 8.0f, 1.0f, 2},
    {5, 9, 0, 8.0f, 1.0f, 2},
    {5, 0, 0, 8.0f, 1.0f, 2},
    {0, 8, 0, 8.0f, 1.0f, 2},
};
static const char* l_evn_tbl[6] = {"Use_Fairy", "Ba1_Get_Itm", "Ganbaru", "tale_1", "None", "tale_2"};
static const char* a_cut_tbl[2] = {"ACTION", "START_TALE1"};

static inline s32 J3DAnm_getFrameMax(J3DAnmTexPattern* p) {
    Ba1TexPatternView* pattern = reinterpret_cast<Ba1TexPatternView*>(p);
    return gabi::call_ptr<s32>(pattern->mpVtable->mGetFrameMax, p);
}

static inline s8 ba1_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return (s8)dComIfGp_evmng_getMyActIdx(staffId, gabi::at<gptr<char>>(tbl), n, force, nameType);
}

static inline void daPy_setPlayerPosAndAngle(fopAc_ac_c* player, cXyz* pos, s16 angle) {
    Ba1PlayerView* view = reinterpret_cast<Ba1PlayerView*>(player);
    gabi::call_ptr(view->mpVtable->mSetPlayerPosAndAngle, player, pos, angle);
}

static inline u32 fopMsgM_getStatus() { return ba1_getMessageStatus(l_messageManager); }

static inline void pmf_load(Ba1ProcFunc* dst, const Ba1ProcFunc* src) {
    reinterpret_cast<be<u32>*>(dst)[0] = reinterpret_cast<const be<u32>*>(src)[0];
    reinterpret_cast<be<u32>*>(dst)[1] = reinterpret_cast<const be<u32>*>(src)[1];
}

static inline void pmf_call(void* self, Ba1ProcFunc* pmf, void* arg) {
    (static_cast<daNpc_Ba1_c*>(self)->*(*pmf))(arg);
}

/* 021F2BB8 */
static daNpc_Ba1_HIO_c* daNpc_Ba1_HIO_c_ct(daNpc_Ba1_HIO_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Ba1_HIO_c*)operator_new(0x30);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x100168C0;

    memcpy_g(&i_this->mPrmTbl, &a_prm_tbl, 0x24);
    i_this->mNo = -1;
    i_this->field_0x8 = -1;
    return i_this;
}

/* 021EE4AC */
static void* searchActor_Fa(void* i_param_1, void*) {
    if (l_check_wrk < 0x14 && fopAc_IsActor(i_param_1) && i_param_1 != nullptr && fpcM_GetName(i_param_1) == fpcNm_NPC_FA1_e) {
        s32 n = l_check_wrk;
        l_check_wrk = n + 1;
        l_check_inf[n] = (fopAc_ac_c*)i_param_1;
    }
    return nullptr;
}

/* 021EE6CC */
static BOOL nodeCallBack_Ba1(J3DNode* i_param_1, int i_param_2) {
    if (i_param_2 == 0) {
        J3DModel* model = j3dSys.getModel();
        daNpc_Ba1_c* user = gabi::at<daNpc_Ba1_c>(model->mUserArea);
        if (user != nullptr) {
            user->nodeBa1Control(i_param_1, model);
        }
    }
    return TRUE;
}

/* 021EE52C */
void daNpc_Ba1_c::nodeBa1Control(J3DNode* i_node, J3DModel* i_model) {
    // HD: initialize z, then the guard, then x and y.
    if (eyeOffsetInitialized == 0) {
        a_eye_pos_off.z = 0.0f;
        eyeOffsetInitialized = 1;
        a_eye_pos_off.x = 20.0f;
        a_eye_pos_off.y = -16.0f;
    }
    u32 jointIdx = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
    PSMTXCopy(i_model->getAnmMtx(jointIdx), mDoMtx_stack_c::get());
    if (jointIdx == (u32)(s32)m_hed_jnt_num) {
        mDoMtx_YrotM(mDoMtx_stack_c::get(), -m_jnt.mAngles[0][1]);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), -m_jnt.mAngles[0][0]);
        PSMTXMultVec(mDoMtx_stack_c::get(), &a_eye_pos_off, &mTransformedEyePos);
    }
    if (jointIdx == (u32)(s32)m_bbone_jnt_num) {
        mDoMtx_XrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[1][1]);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[1][0]);
    }
    PSMTXCopy(mDoMtx_stack_c::get(), J3DSys::mCurrentMtx);
    mtx_copy(i_model->getAnmMtx(jointIdx), mDoMtx_stack_c::get());
}

/* 021EE754 */
// HD: the tail call preserves the full result register.
static u32 daNpc_Ba1_XyCheck_cB(void* i_this, int i_itemBtn) {
    return ba1_checkItemResult(i_this, i_itemBtn); /* static_cast<daNpc_Ba1_c*>(i_this)->XyCheck_cB(i_itemBtn) */
}

/* 021EE714 */
bool daNpc_Ba1_c::XyCheck_cB(int i_itemBtn) {
    return dComIfGp_getSelectItem(i_itemBtn) == dItemNo_FAIRY_BOTTLE_e;
}

/* 021EE760 */
static s16 daNpc_Ba1_XyEvent_cB(void* i_this, int param_1) {
    return static_cast<daNpc_Ba1_c*>(i_this)->XyEvent_cB(param_1);
}

/* 021EE758 */
s16 daNpc_Ba1_c::XyEvent_cB(int) {
    return mEventIdTable[0];
}

/* 021EEE18 */
bool daNpc_Ba1_c::init_BA1_0() {
    if (!dComIfGs_isEventBit(dSv_event_flag_UNK_0520) && !dComIfGs_isEventBit(dSv_event_flag_UNK_0001)) {
        gabi::Local<Ba1ProcFunc> pmf;
        pmf_load(pmf, &l_wait_action1.action);
        set_action(pmf, nullptr);
        mpClothModel = nullptr;
        return true;
    } else {
        return false;
    }
}

/* 021EEEC8 */
bool daNpc_Ba1_c::init_BA1_1() {
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
        gabi::Local<Ba1ProcFunc> pmf;
        pmf_load(pmf, &l_wait_action2.action);
        set_action(pmf, nullptr);
        mpClothModel = nullptr;
        actor_status &= ~fopAcStts_NOCULLEXEC_e; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
    }
    return ret;
}

/* 021EEFB0 */
bool daNpc_Ba1_c::init_BA1_2() {
    mpClothModel = nullptr;
    gabi::Local<Ba1ProcFunc> pmf;
    pmf_load(pmf, &l_demo_action1.action);
    set_action(pmf, nullptr);
    return true;
}

/* 021EEFF4 */
bool daNpc_Ba1_c::init_BA1_3() {
    bool ret = dComIfGs_isEventBit(dSv_event_flag_UNK_0520) != 0;
    if (!ret) {
        return ret;
    }
    ret = !dComIfGs_isEventBit(dSv_event_flag_GRANDMA_HEALED);
    if (ret) {
        attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0x1C; /* attention_info.distances[SPEAK] */
        attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0x1B; /* attention_info.distances[TALK] */
        actor_status &= ~(fopAcStts_CULL_e | fopAcStts_NOCULLEXEC_e); /* fopAcM_OffStatus(this, fopAcStts_CULL_e | fopAcStts_NOCULLEXEC_e) */
        gabi::Local<Ba1ProcFunc> pmf;
        pmf_load(pmf, &l_wait_action3.action);
        set_action(pmf, nullptr);
        gravity = 0.0f;
        eventInfo.setXyEventCB((dEvt_info_c::CallbackFunc)daNpc_Ba1_XyEvent_cB); /* eventInfo.setXyEventCB(daNpc_Ba1_XyEvent_cB) */
        eventInfo.setXyCheckCB((dEvt_info_c::CallbackFunc)daNpc_Ba1_XyCheck_cB); /* eventInfo.setXyCheckCB(daNpc_Ba1_XyCheck_cB) */
    }
    return ret;
}

/* 021EF0D0 */
bool daNpc_Ba1_c::init_BA1_4() {
    bool ret = dComIfGs_isEventBit(dSv_event_flag_UNK_0520) != 0;
    if (!ret) {
        return ret;
    }
    ret = dComIfGs_isEventBit(dSv_event_flag_GRANDMA_HEALED) != 0;
    if (ret) {
        mpClothModel = nullptr;
        gabi::Local<Ba1ProcFunc> pmf;
        pmf_load(pmf, &l_wait_action4.action);
        set_action(pmf, nullptr);
    }
    return ret;
}

/* 021EF4BC */
bool daNpc_Ba1_c::createInit() {

    for (int i = 0; i < 6; i++) {
        const char* name = l_evn_tbl[i];
        mEventIdTable[i] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    }
    gravity = -4.5f;
    m79C.copy(current.pos);
    attention_info.flags = fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e; /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 171;  /* attention_info.distances[SPEAK] */
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 171;  /* attention_info.distances[TALK] */
    mEventCut.setActorInfo2("Ba1" /* "Ba1" */, (fopNpc_npc_c*)(void*)this);
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
    m7F2 = dComIfGs_getEventReg( 0xA60F /* dSv_event_flag_c::UNK_A60F */);
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mpMorf->setMorf(0.0f);
    setMtx(true);
    return true;
}

/* 021EF2A0 */
void daNpc_Ba1_c::setMtx(u32 param_1) {
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
    void* gndPoly = static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd);
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), gndPoly);
    tevStr.mEnvrIdxOverride = dBgS_GetPolyColor(dComIfG_Bgsp(), gndPoly); /* tevStr.mEnvrIdxOverride */

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(m78A.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();

    if (mpClothModel.get() != nullptr) {
        J3DModel_setBaseTRMtx(mpClothModel, mpMorf->getModel()->getAnmMtx(m_footL_jnt_num));
        J3DModel_calc(mpClothModel);
    }
    setAttention(param_1);
}

/* 021EFCFC */
int daNpc_Ba1_c::anmNum_toResID(int param_1) {

    return a_bck_resID_tbl[param_1];
}

/* 021EE99C */
int daNpc_Ba1_c::btpNum_toResID(int param_1) {

    return a_btp_resID_tbl[param_1];
}

/* 021EE9B0 */
/* i_param_1 (GameCube bool) is passed on unnormalised: typed u32 */
bool daNpc_Ba1_c::setBtp(u32 i_param_1, int i_btp_num) {
    J3DModelData* model_data = mpMorf->getModel()->getModelData();
    int res_id = btpNum_toResID(i_btp_num);
    m_hed_tex_pttrn = (J3DAnmTexPattern*)dComIfG_getObjectIDRes("Ba", res_id);
    if (m_hed_tex_pttrn.get() == nullptr) /* JUT_ASSERT(572, m_hed_tex_pttrn != NULL) */
        JUT_ASSERT_fail("d_a_npc_ba1.cpp", 0x23C, "m_hed_tex_pttrn != (0)");

    int iVar1 = mDoExt_btpAnm_init(&mHeadBtpAnm, model_data, m_hed_tex_pttrn, 1, 2, 1.0f, 0, -1, i_param_1, 0);
    bool o_retval = iVar1 == 1;
    if (o_retval) {
        mBlinkTimer = 0;
        mBlinkFrame = 0;
    }
    return o_retval;
}

/* 021EEA9C */
// HD: the tail call preserves setBtp's full result register.
u32 daNpc_Ba1_c::iniTexPttrnAnm(u32 i_param_1) {
    return ba1_setBtpResult(this, i_param_1, (s32)mBtpNum); /* setBtp(i_param_1, mBtpNum) */
}

/* 021EF170 */
void daNpc_Ba1_c::plyTexPttrnAnm() {
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

/* 021EFCDC */
u32 daNpc_Ba1_c::setAnm_tex(s8 i_btp_num) {
    if (mBtpNum != i_btp_num) {
        mBtpNum = i_btp_num;
        return iniTexPttrnAnm(true);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}

/* 021EFD10 */
BOOL daNpc_Ba1_c::setAnm_anm(daNpc_Ba1_c::anm_prm_c* i_anmPrmP) {
    if (mAnmNum == i_anmPrmP->mAnmNum) {
        return TRUE;
    }
    mAnmNum = i_anmPrmP->mAnmNum;
    int resID = anmNum_toResID(mAnmNum);
    s32 loopMode = i_anmPrmP->mLoopMode;
    f32 speed = i_anmPrmP->mSpeed;
    f32 morf = i_anmPrmP->mMorf;
    dNpc_setAnmIDRes(mpMorf, loopMode, morf, speed, resID, -1, "Ba");
    mbMorfAnimStopped = 0;
    mPrevMorfFrame = 0.0f;
    m7EF = 0;
    return TRUE;
}

/* 021EFDA4 */
void daNpc_Ba1_c::setAnm_NUM(int i_param_1, int i_param_2) {
    anm_prm_c* a_anm_prm_tbl = a_anm_num_prm_tbl;
    if (i_param_2 != 0) {
        setAnm_tex(a_anm_prm_tbl[i_param_1].mBtpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[i_param_1]);
}

/* 021F146C */
bool daNpc_Ba1_c::setAnm() {
    anm_prm_c* a_anm_prm_tbl = a_anm_status_prm_tbl;
    if (a_anm_prm_tbl[mStatus].mBtpNum >= 0) {
        setAnm_tex(a_anm_prm_tbl[mStatus].mBtpNum);
    }
    if (a_anm_prm_tbl[mStatus].mAnmNum >= 0) {
        setAnm_anm(&a_anm_prm_tbl[mStatus]);
    }
    return true;
}

/* 021F1560 */
void daNpc_Ba1_c::chg_anmAtr(u8 i_param_1) {
    if ((i_param_1 >= 0x15) || (i_param_1 == m80E)) {
        return;
    }
    m80E = i_param_1;
    setAnm_ATR(1);
}

/* 021F1584 */
void daNpc_Ba1_c::control_anmAtr() {
    switch (m80E) {
    case 5:
    case 6:
        switch (mLookBackState) {
        case LOOK_PLAYER:
            break;
        default:
            m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
            mLookBackState = LOOK_PLAYER;
            break;
        }
        break;
    default:
        return;
    }
}

/* 021F14F0 */
void daNpc_Ba1_c::setAnm_ATR(int i_param_1) {
    anm_prm_c* a_anm_prm_tbl = a_anm_atr_prm_tbl;
    if (i_param_1 != 0) {
        setAnm_tex(a_anm_prm_tbl[m80E].mBtpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[m80E]);
}

/* 021F15B8 */
void daNpc_Ba1_c::anmAtr(u16 i_param_1) {
    switch (i_param_1) {
    case 6: {
        if (m819 == 0) {
            m80E = 0xFF;
            chg_anmAtr(dComIfGp_getMesgAnimeAttrInfo());
            m819 = m819 + 1;
        }
        u8 mesgAnimeTagInfo = dComIfGp_getMesgAnimeTagInfo(); /* dComIfGp_getMesgAnimeTagInfo() */
        dComIfGp_clearMesgAnimeTagInfo();                /* dComIfGp_clearMesgAnimeTagInfo() */
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

/* 021F0F78 */
void daNpc_Ba1_c::eventOrder() {
    s8 condition = m812;
    if (condition == 1 || condition == 2) {
        eventInfo.onCondition(dEvtCnd_CANTALK_e); /* onCondition(dEvtCnd_CANTALK_e) */
        if (mSpecificType == 3 && !dComIfGs_isEventBit(dSv_event_flag_GRANDMA_HEALED)) {
            eventInfo.onCondition(dEvtCnd_CANTALKITEM_e); /* onCondition(dEvtCnd_CANTALKITEM_e) */
        }
        if (m812 == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (condition >= 3) {
        mEventIdx = condition - 3;
        fopAcM_orderOtherEventId(this, mEventIdTable[mEventIdx], 0xFF, 0xFFFF, 0, 1);
    }
}

/* 021EF8B4 */
void daNpc_Ba1_c::checkOrder() {
    if (eventInfo.mCommand == dEvtCmd_INDEMO_e /* eventInfo.checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mEventIdTable[mEventIdx])) {
            m812 = 0;
        }
    } else if (checkCommandTalk() && (m812 == 1 || m812 == 2)) {
        m812 = 0;
        m809 = true;
    }
}

/* 021F1680 */
bool daNpc_Ba1_c::chk_talk() {
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

/* 021F1714 */
bool daNpc_Ba1_c::chk_drct(f32 i_param_1) {
    s16 target = cLib_targetAngleY(&current.pos, &dComIfGp_getPlayer(0)->current.pos) - current.angle.y;
    int cmp = abs((int)target);
    return cmp < (s16)gabi::ftoi(i_param_1 * 182.04445f); /* cM_deg2s */
}

/* 021F17AC */
u8 daNpc_Ba1_c::chk_partsNotMove() {
    return mJointHeadY == m_jnt.mAngles[0][1] && mJointBackboneY == m_jnt.mAngles[1][1] && mActorAngleY == current.angle.y;
}

/* 021F0B18 */
void daNpc_Ba1_c::lookBack() {
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
    case LOOK_PLAYER: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos(eye, -20.0f);
        dstPos->copy(*eye);
        dstPos_p = dstPos;
        srcZ = current.pos.z;
        srcX = current.pos.x;
        srcY = eyePos.y;
        break;
    }
    case LOOK_POSITION:
        dstPos->copy(m79C);
        dstPos_p = dstPos;
        srcZ = current.pos.z;
        srcX = current.pos.x;
        srcY = eyePos.y;
        break;
    case LOOK_ANGLE:
        desiredYrot = mTargetYRot;
        break;
    }
    cLib_addCalcAngleS2(&mLookAtMaxVel, l_HIO.mPrmTbl.mCalcAngleTarget, 4, 0x800);
    if (!m_jnt.mbTrn) { /* !m_jnt.trnChk() */
        mLookAtMaxVel = 0;
    }
    gabi::Local<cXyz> src_pos; /* passed by value: a copy on the stack */
    src_pos->x = srcX;
    src_pos->y = srcY;
    src_pos->z = srcZ;
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos_p, src_pos, desiredYrot, mLookAtMaxVel, headOnlyFollow);
}

/* 021F17EC */
u16 daNpc_Ba1_c::next_msgStatus(be<u32>* i_msg_no) {
    u16 ret = 0xF;
    // HD: the message is reached through the message manager.
    u32 msg = l_messageManager;
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
        // HD: no checkEmptyBottle test (GameCube: 0x7F5 with an empty bottle, else 0x7F9)
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
                // HD: also with a half-full soup bottle
                *i_msg_no = 0x7FE;
            } else {
                *i_msg_no = 0x7FF;
            }
            break;
        }
        *i_msg_no = 0x7F5; // HD: no empty-bottle test on this path.
        break;
    case 0x7FF:
        switch (gabi::at<msg_class>(msg)->mSelectNum /* mSelectNum */) {
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

/* 021F19AC */
u32 daNpc_Ba1_c::getMsg_BA1_0() {
    if (m7FA) {
        return 0x7d8;
    }
    if (mbHoldEvent) {
        return dComIfGs_isEventBit(dSv_event_flag_UNK_0608) ? 0x7e7 : 0x7e6;
    }
    return m7FB ? 0x7e5 : 0x7e4;
}

/* 021F1A50 */
u32 daNpc_Ba1_c::getMsg_BA1_1() {
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

/* 021F1B70 */
u32 daNpc_Ba1_c::getMsg_BA1_3() {
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

/* 021F1CB4 */
u32 daNpc_Ba1_c::getMsg_BA1_4() {
    return getMsg_BA1_3();
}

/* 021F1CB8 */
u32 daNpc_Ba1_c::getMsg() {
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

/* 021F1D20 */
u8 daNpc_Ba1_c::chkAttention() {
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}

/* 021EF244 */
void daNpc_Ba1_c::setAttention(u32 i_setEyePos) {
    f32 offset = l_HIO.mPrmTbl.mAttPosOffsetY;
    cXyz* attPos = &attention_info.position; /* attention_info.position */
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

/* 021F0838 */
fopAc_ac_c* daNpc_Ba1_c::searchByID(fpc_ProcID i_procID) {
    gabi::Local<gptr<fopAc_ac_c>> o_actor;
    *o_actor = nullptr;
    ba1_searchByID(i_procID, o_actor.get()); /* fopAcM_SearchByID(id, &actor) */
    return *o_actor;
}

/* 021F0694 */
bool daNpc_Ba1_c::partner_srch_sub(u32 i_searchFunc) {
    bool o_retval = false;
    mPartnerProcID = 0xFFFFFFFF;
    l_check_wrk = 0;
    for (int i = 0; i < 20; i++) {
        l_check_inf[i] = nullptr;
    }
    fpcM_Search(i_searchFunc, this);
    if (l_check_wrk != 0) {
        mPartnerProcID = fopAcM_GetID(l_check_inf[0].get());
        o_retval = true;
    }
    return o_retval;
}

/* 021EF848 */
void daNpc_Ba1_c::partner_srch() {
    if (m818 == PROC_SEARCH) {
        m818 = m818 + 1;
    }
}

/* 021F1DA8 */
bool daNpc_Ba1_c::check_useFairyArea() {
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
    if (std::fabs(player->current.pos.y - current.pos.y) < 100.0f && mag < l_HIO.mPrmTbl.m1C) {
        ret = true;
    }
    return ret;
}

/* 021EF864 */
bool daNpc_Ba1_c::checkCommandTalk() {
    bool ret = false;
    if (eventInfo.mCommand == dEvtCmd_INTALK_e /* eventInfo.checkCommandTalk() */) {
        ret = dComIfGp_event_chkTalkXY() == 0;
    }
    return ret;
}

/* 021EECBC */
bool daNpc_Ba1_c::charDecide(int i_param_1) {
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

/* 021EFE10 */
void daNpc_Ba1_c::eInit_SET_PLYER_GOL_() {
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

/* 021EFEA4 */
void daNpc_Ba1_c::eInit_PLYER_INI_POS_() {
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

/* 021EFF5C */
void daNpc_Ba1_c::eInit_USE_FAIRY_END_() {
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0x5B; /* attention_info.distances[TALK] */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0x59; /* attention_info.distances[SPEAK] */
    mFairyUsed = true;
}

/* 021EFF78 */
void daNpc_Ba1_c::eInit_MOV_POS_() {
    m7FE = true;
    mpClothModel = nullptr;
    m7F5 = true;
    m7F7 = true;
}

/* 021EFF94 */
void daNpc_Ba1_c::eInit_SET_PLYER_TRN_ANG_() {
    // HD: daPy_getPlayerActorClass() and dComIfGp_getPlayer(0) are one load
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    /* player->changeDemoMoveAngle(angle): daPy_py_c +0x422 */
    reinterpret_cast<Ba1PlayerView*>(player)->mDemoMoveAngle = cLib_targetAngleY(&player->current.pos, &current.pos);
}

/* 021EFFDC */
void daNpc_Ba1_c::eInit_ACTOR_DRW_CONTROL_(be<s32>* i_param_1, be<s32>* i_param_2) {
    if (i_param_1 != nullptr) {
        fopAc_ac_c* player = dComIfGp_getPlayer(0); /* daPy_getPlayerActorClass() */
        be<u32>* noResetFlg0 = &reinterpret_cast<Ba1PlayerView*>(player)->mNoResetFlg0;
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
            attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xAB; /* attention_info.distances[SPEAK] */
            tmp->x = 0.0f;
            attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAB; /* attention_info.distances[TALK] */
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

/* 021F0158 */
void daNpc_Ba1_c::eInit_setEvTimer(be<s32>* i_param_1) {
    mEvTimer = 0;
    if (i_param_1 != nullptr) {
        mEvTimer = (s16)*i_param_1;
    }
}

/* 021F0174 */
void daNpc_Ba1_c::eInit_CHK_FAIRY_(be<s32>* i_param_1) {
    eInit_setEvTimer(i_param_1);
}

/* 021F0178 */
f32 daNpc_Ba1_c::eInit_prmFloat(be<f32>* i_param_1, f32 i_param_2) {
    if (i_param_1 != nullptr) {
        return *i_param_1;
    }
    return i_param_2;
}

/* 021F0188 */
void daNpc_Ba1_c::eInit_SET_EYE_OFF_(be<f32>* i_param_1) {
    mEyeOffset = eInit_prmFloat(i_param_1, 0.0f);
}

/* 021F01B8 */
void daNpc_Ba1_c::eInit_EYE_OFF_ZRO_(be<f32>* i_param_1) {
    mEyeOffsetZero = eInit_prmFloat(i_param_1, 0.0f);
}

/* 021F01E8 */
void daNpc_Ba1_c::eInit_CHK_FAIRY_MOV_1(be<s32>* i_param_1) {
    eInit_setEvTimer(i_param_1);
}

/* 021F01EC */
void daNpc_Ba1_c::event_actionInit(int i_staff_idx) {
    be<s32>* act_no_p = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staff_idx, "ActNo"); /* "ActNo" */
    be<s32>* prm_0_p = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staff_idx, "prm_0"); /* "prm_0" */
    be<s32>* prm_1_p = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staff_idx, "prm_1"); /* "prm_1" */
    be<s32>* timer_p = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staff_idx, "Timer"); /* "Timer" */
    be<f32>* atten_p = (be<f32>*)dComIfGp_evmng_getMyFloatP(i_staff_idx, "Atten");   /* "Atten" */
    be<f32>* speed_p = (be<f32>*)dComIfGp_evmng_getMyFloatP(i_staff_idx, "Speed");   /* "Speed" */
    if (act_no_p != nullptr) {
        mActNo = (s8)*act_no_p;
        switch (mActNo) {
        case ACT_WAKE:
            setAnm_NUM(9, 1);
            break;
        case ACT_SET_PLAYER_GOAL:
            eInit_SET_PLYER_GOL_();
            break;
        case ACT_SET_PLAYER_POS:
            eInit_PLYER_INI_POS_();
            break;
        case ACT_FAIRY_END:
            eInit_USE_FAIRY_END_();
            break;
        case ACT_MOVE_POS:
            eInit_MOV_POS_();
            break;
        case ACT_TURN_PLAYER:
            eInit_SET_PLYER_TRN_ANG_();
            break;
        case ACT_DRAW_CONTROL:
            eInit_ACTOR_DRW_CONTROL_(prm_0_p, prm_1_p);
            break;
        case ACT_CHECK_FAIRY:
            eInit_CHK_FAIRY_(timer_p);
            break;
        case ACT_EYE_OFFSET:
            eInit_SET_EYE_OFF_(atten_p);
            break;
        case ACT_EYE_RESET:
            eInit_EYE_OFF_ZRO_(speed_p);
            break;
        case ACT_FAIRY_MOVEMENT:
            eInit_CHK_FAIRY_MOV_1(timer_p);
            break;
        }
    }
}

/* 021F04DC */
u32 daNpc_Ba1_c::eMove_MOV_POS_() {
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

/* 021F0740 */
u32 daNpc_Ba1_c::eMove_CHK_FAIRY_() {
    if (cLib_calcTimer(&mEvTimer) == 0) {
        return !partner_srch_sub(gabi::guest_fn((const void*)searchActor_Fa));
    }
    return false;
}

/* 021F07A0 */
u32 daNpc_Ba1_c::eMove_EYE_OFF_ZRO_() {
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

/* 021F086C */
u32 daNpc_Ba1_c::eMove_CHK_FAIRY_MOV_1() {
    bool ret = false;
    if (!cLib_calcTimer(&mEvTimer)) {
        if (partner_srch_sub(gabi::guest_fn((const void*)searchActor_Fa))) {
            fopAc_ac_c* actor_fa = searchByID(mPartnerProcID);
            if (actor_fa != nullptr) {
                ret = reinterpret_cast<Ba1FairyView*>(actor_fa)->mMode == 5; /* daNpc_Fa1_c::isBabaMode() */
                if (ret) {
                    eInit_PLYER_INI_POS_();
                }
            }
        }
    }
    return ret;
}

/* 021F08FC */
// HD: eMove results preserve the full result register.
u32 daNpc_Ba1_c::event_action() {
    u32 ret;
    switch (mActNo) {
    case ACT_MOVE_POS:
        ret = eMove_MOV_POS_();
        break;
    case ACT_CHECK_FAIRY:
        ret = eMove_CHK_FAIRY_();
        break;
    case ACT_EYE_RESET:
        ret = eMove_EYE_OFF_ZRO_();
        break;
    case ACT_FAIRY_MOVEMENT:
        ret = eMove_CHK_FAIRY_MOV_1();
        break;
    default:
        ret = true;
        break;
    }
    return ret;
}

/* 021F0474 */
void daNpc_Ba1_c::cut_init_START_TALE1(int i_staff_idx) {
    be<s32>* timer_p = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staff_idx, "Timer"); /* "Timer" */
    mEvTimer = 0;
    if (timer_p != nullptr) {
        mEvTimer = (s16)*timer_p;
    }
}

/* 021F0964 */
bool daNpc_Ba1_c::cut_move_START_TALE1() {
    if (cLib_calcTimer(&mEvTimer) == 0) {
        /* dComIfGs_getClearCount(): save info byte +0x1C0 */
        s16 point = dComIfGs_getClearCount() != 0 ? 202 : 200;
        dComIfGp_setNextStage("LinkRM" /* "LinkRM" */, point, 0, 8, 0.0f, 0, 1, 0);
    }
    return mEvTimer == 0;
}

/* 021F09EC */
void daNpc_Ba1_c::privateCut(int i_staff_idx) {
    /* static char* a_cut_tbl[] = {"ACTION", "START_TALE1"} (.data 0x101BB948) */
    enum { ACT_ACTION, ACT_START_TALE1 };
    if (i_staff_idx == -1) {
        return;
    }
    mActionIndex = ba1_getMyActIdx(i_staff_idx, gabi::ea(a_cut_tbl), 2, TRUE, 0);
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

/* 021EFC9C */
void daNpc_Ba1_c::endEvent() {
    dComIfGp_event_reset();
    m80E = 0xFF;
}

/* 021EFC5C */
s32 daNpc_Ba1_c::isEventEntry() {
    const char* name = gabi::at<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}

/* 021F0D58 */
void daNpc_Ba1_c::event_proc(int i_staff_idx) {
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
            gabi::Local<Ba1ProcFunc> pmf;
            pmf_load(pmf, &l_wait_action4.action);
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

/* 021EECEC */
BOOL daNpc_Ba1_c::set_action(Ba1ProcFunc* i_newProcFunc, void* i_argsP) {
    Ba1ProcFunc* cur = &mCurrProcFunc;
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
        m818 = PROC_END;
        pmf_call(this, cur, i_argsP);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    m818 = PROC_START;
    pmf_call(this, cur, i_argsP);
    return TRUE;
}

/* 021F1E78 */
void daNpc_Ba1_c::setStt(s8 i_status) {
    s8 temp = mStatus;
    mStatus = i_status;
    mEvTimer2 = 0;
    switch ((u32)(s32)i_status) {
    case 2:
        m80E = 0xFF;
        if (mAnmNum != 4) {
            m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
            mLookBackState = LOOK_PLAYER;
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

/* 021F1F0C */
BOOL daNpc_Ba1_c::wait_0() {
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
        if (std::fabs(dComIfGp_getPlayer(0)->current.pos.y - current.pos.y) < 100.0f && dist < l_HIO.mPrmTbl.m20) {
            dComIfGs_onTmpBit(0x0310);
            m812 = 6;
        }
    }
    return TRUE;
}

/* 021F2040 */
BOOL daNpc_Ba1_c::wait_1() {
    cLib_addCalcAngleS(&current.angle.y, mInitialAngle.y, 4, 0x800, 0);
    if (m812 == 1 || m812 >= 3) {
        return TRUE;
    }
    if (m809) {
        if (chk_talk()) {
            setStt(STATUS_TALK);
            mHeadOnlyFollow = false;
        }
        return TRUE;
    } else if (mbHoldEvent) {
        setAnm_NUM(4, 1);
        m812 = 1;
        mLookBackState = LOOK_NONE;
        mTargetYRot = current.angle.y + -0x638e;
    } else {
        m812 = 2;
        if (mbAttention != 0) {
            mEvTimer2 = 60;
        }
        if (cLib_calcTimer(&mEvTimer2) && chk_drct(61.0f)) {
            mLookBackState = LOOK_PLAYER;
        } else {
            mLookBackState = LOOK_ANGLE;
            mTargetYRot = mInitialAngle.y;
            m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        }
    }
    return TRUE;
}

/* 021F217C */
BOOL daNpc_Ba1_c::talk_1() {
    u8 ret = chk_partsNotMove();
    if (mAnmNum == 4) {
        cLib_addCalcAngleS(&current.angle.y, mTargetYRot, 2, 0x1000, 1);
        if (!mbMorfAnimStopped) {
            return TRUE;
        }
    }
    talk(1);
    // HD: the message status comes from the message manager (GameCube: mpCurrMsg->mStatus)
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

/* 021F24B4 */
BOOL daNpc_Ba1_c::talk_2() {
    return talk_1();
}

/* 021F24B8 */
BOOL daNpc_Ba1_c::wait_2() {
    cLib_addCalcAngleS(&current.angle.y, mInitialAngle.y, 4, 0x800, 0);
    if (m812 == 1 || m812 >= 3) {
        return TRUE;
    }
    if (m809) {
        if (m7FF) {
            dComIfGp_setNextStage("LinkRM" /* "LinkRM" */, 201, 0, 9, 0.0f, 0, 1, 0);
            return TRUE;
        }
        if (chk_talk()) {
            setStt(STATUS_TALK);
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
            mLookBackState = LOOK_ANGLE;
            m_jnt.mbTrn = 1;
            return TRUE;
        }
        mLookBackState = LOOK_ANGLE;
        mTargetYRot = mInitialAngle.y;
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        return TRUE;
    }
    m812 = 2;
    if (mbAttention) {
        mEvTimer2 = 60;
    }
    if (cLib_calcTimer(&mEvTimer2) && chk_drct(61.0f)) {
        mLookBackState = LOOK_PLAYER;
    } else {
        mLookBackState = LOOK_ANGLE;
        mTargetYRot = mInitialAngle.y;
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
    }
    return TRUE;
}

/* 021F26D0 */
BOOL daNpc_Ba1_c::wait_3() {
    return wait_2();
}

/* 021F26D4 */
BOOL daNpc_Ba1_c::ZZZwai() {
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
            setStt(STATUS_TALK_SLEEP);
        }
        return TRUE;
    }
    if (dComIfGs_isEventBit(dSv_event_flag_GRANDMA_HEALED)) {
        m812 = 2;
        if (mbAttention) {
            mEvTimer2 = 60;
        }
        if (cLib_calcTimer(&mEvTimer2) && chk_drct(61.0f)) {
            mLookBackState = LOOK_PLAYER;
        } else {
            mLookBackState = LOOK_ANGLE;
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

/* 021F2818 */
BOOL daNpc_Ba1_c::wait_action1(void*) {
    switch ((u32)(s32)m818) {
    case PROC_START:
        if (dComIfGs_isEventBit(dSv_event_flag_UNK_2A80)) {
            m7FA = dComIfGs_isTmpBit(0x0310) != 0;
            if (m7FA) {
                dComIfGs_offTmpBit(0x0310);
            }
            setStt(STATUS_WAIT_HOME);
            m818 = m818 + 1;
        } else {
            if (dComIfGs_isTmpBit(0x0310)) {
                setStt(STATUS_WAIT_HOME);
                m818 = m818 + 1;
            } else {
                setStt(STATUS_WAIT_PLAYER);
                m818 = m818 + 1;
            }
        }
        break;
    case PROC_SEARCH:
    case PROC_PARTNER_FOUND:
    case PROC_ACTIVE:
        mbAttention = chkAttention();
        switch (mStatus) {
        case STATUS_WAIT_PLAYER:
            mbSetEyePos = wait_0();
            break;
        case STATUS_WAIT_HOME:
            mbSetEyePos = wait_1();
            break;
        case STATUS_TALK:
            mbSetEyePos = talk_1();
            break;
        }
        lookBack();
    default:
        break;
    }
    return TRUE;
}

/* 021F2980 */
BOOL daNpc_Ba1_c::wait_action2(void*) {
    switch ((u32)(s32)m818) {
    case PROC_START:
        setStt(STATUS_WAIT_OUTSIDE);
        m818 = m818 + 1;
        break;
    case PROC_SEARCH:
    case PROC_PARTNER_FOUND:
    case PROC_ACTIVE:
        mbAttention = chkAttention();
        switch (mStatus) {
        case STATUS_WAIT_OUTSIDE:
            mbSetEyePos = wait_2();
            break;
        case STATUS_TALK:
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

/* 021F2A30 */
BOOL daNpc_Ba1_c::demo_action1(void*) {
    switch (m818) {
    case PROC_END:
        break;
    case PROC_START:
        m818 = m818 + 1;
    case PROC_SEARCH:
    case PROC_PARTNER_FOUND:
    case PROC_ACTIVE:
        break;
    }
    return TRUE;
}

/* 021F2A4C */
BOOL daNpc_Ba1_c::wait_action3(void*) {
    switch ((u32)(s32)m818) {
    case PROC_START:
        setStt(STATUS_SLEEP);
        m818 = m818 + 1;
        break;
    case PROC_SEARCH:
    case PROC_PARTNER_FOUND:
    case PROC_ACTIVE:
        mbAttention = chkAttention();
        switch (mStatus) {
        case STATUS_SLEEP:
            mbSetEyePos = ZZZwai();
            break;
        case STATUS_TALK_SLEEP:
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

/* 021F2B08 */
BOOL daNpc_Ba1_c::wait_action4(void*) {
    switch ((u32)(s32)m818) {
    case PROC_START:
        setStt(STATUS_WAIT_HEALED);
        m818 = m818 + 1;
        break;
    case PROC_SEARCH:
    case PROC_PARTNER_FOUND:
    case PROC_ACTIVE:
        mbAttention = chkAttention();
        switch (mStatus) {
        case STATUS_WAIT_HEALED:
            mbSetEyePos = wait_3();
            break;
        case STATUS_TALK:
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

/* 021EF978 */
/* returns the mbInDemo byte as is (typed u8) */
u8 daNpc_Ba1_c::demo() {
    if (demoActorID == 0) {
        if (mbInDemo != 0) {
            mbInDemo = 0;
        }
        return mbInDemo;
    }
    u8 id = demoActorID;
    mbInDemo = 1;
    // HD: assert the demo object and check the actor ID range.
    void* demo_actor = nullptr;
    if (id != 0 && id <= 0x20) {
        u32 obj = l_demoObject;
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
            obj = l_demoObject;
        }
        demo_actor = ba1_getDemoActor(obj, id); /* dDemo_object_c::getActor */
    }
    if (m_hed_tex_pttrn.get() != nullptr) {
        u8 frame = (u8)(mBlinkFrame + 1);
        mBlinkFrame = frame;
        if ((s32)frame >= J3DAnm_getFrameMax(m_hed_tex_pttrn)) {
            mBlinkFrame = (u8)J3DAnm_getFrameMax(m_hed_tex_pttrn);
        }
    }
    // HD: demo_actor is checked for NULL
    if (demo_actor != nullptr) {
        J3DAnmTexPattern* demopattern = ba1_getDemoBtp(demo_actor, "Ba");
        if (demopattern != nullptr) {
            m_hed_tex_pttrn = demopattern;
            J3DModelData* md = mpMorf->getModel()->getModelData();
            if (mDoExt_btpAnm_init(&mHeadBtpAnm, md, demopattern, 1, 2, 1.0f, 0, -1, 1, 0)) {
                mBlinkFrame = 0;
                mBtpNum = dRes_ID_BA_BTP_BA_T02_e;
            }
        }
    }
    // HD: Demo01 uses flags 0x28 during the specified frame interval.
    u8 flags = 0x6A;
    {
        gabi::Local<SafeString> a;
        gabi::Local<SafeString> b;
        a->__vtbl = SAFESTRING_VTBL;
        b->__vtbl = SAFESTRING_VTBL;
        b->mStringTop = gabi::ea(l_startStageName);
        a->mStringTop = gabi::guest_addr("Demo01"); /* "Demo01" */
        ba1_assureString(a.get()); /* sead::SafeString::assureTerminationImpl_ (empty) */
        gabi::call_ptr(gabi::at<Ba1SafeStringVtable>(a->__vtbl)->mAssureTermination, a.get());
        u32 s1 = a->mStringTop;
        gabi::call_ptr(gabi::at<Ba1SafeStringVtable>(b->__vtbl)->mAssureTermination, b.get());
        u32 s2 = b->mStringTop;
        bool equal = true;
        if (s1 != s2) {
            equal = false;
            for (u32 i = 0; i < 0x40001; i++) {
                u8 c1 = gabi::at<be<u8>>(s1)[i];
                u8 c2 = gabi::at<be<u8>>(s2)[i];
                if (c1 != c2)
                    break;
                if (c1 == 0) {
                    equal = true;
                    break;
                }
            }
        }
        if (equal && (u32)(l_demoFrame - 0x14A) < 0x37) {
            flags = 0x28;
        }
    }
    dDemo_setDemoData(this, flags, mpMorf, "Ba" /* "Ba" */, 0, nullptr, 0, 0);
    m78A.x = current.angle.x;
    m78A.y = current.angle.y;
    m78A.z = current.angle.z;
    shape_angle.x = m78A.x;
    shape_angle.y = m78A.y;
    shape_angle.z = m78A.z;
    return mbInDemo;
}

/* 021F128C */
BOOL daNpc_Ba1_c::_draw() {
    J3DModel* morf_model = mpMorf->getModel();
    J3DModelData* model_data = morf_model->getModelData();
    if (mbInitGrandma0 || m7F7) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model, &tevStr);
    dComIfGp_get(); /* HD: an unused accessor call */
    mDoExt_btpAnm_entry(&mHeadBtpAnm, model_data, mBlinkFrame);
    mpMorf->entryDL();
    reinterpret_cast<Ba1ModelDataView*>(model_data)->mpTexNoAnimator = 0; /* mHeadBtpAnm.remove(model_data) */
    if (mpClothModel.get() != nullptr) {
        setLightTevColorType(dKy_getEnvlight(), mpClothModel, &tevStr);
        mDoExt_modelEntryDL(mpClothModel);
    }
    // HD: no shadowDraw()
    dSnap_RegistFig(0x4A /* DSNAP_TYPE_NPC_BA1 */, this, 1.0f, 1.0f, 1.0f);

    /* debug leftovers: function-local static colors initialised on first use */
    if (l_HIO.mPrmTbl.m18 /* l_HIO.mPrmTbl.m18 */ != 0) {
        if (debugRedInitialized == 0) {
            debugRedInitialized = 1;
            memcpy_g(&debugRed, &a_debugRed, 4);
        }
        if (debugBlueInitialized == 0) {
            debugBlueInitialized = 1;
            memcpy_g(&debugBlue, &a_debugBlue, 4);
        }
        if (mSpecificType == 0) {
            if (debugYellowInitialized != 0)
                return TRUE;
            debugYellowInitialized = 1;
            memcpy_g(&debugYellow, &a_debugYellow, 4);
        }
        if (mSpecificType == 3) {
            if (debugYellowInitialized == 0) {
                debugYellowInitialized = 1;
                memcpy_g(&debugYellow, &a_debugYellow, 4);
            }
        }
    }
    return TRUE;
}

/* 021F1064 */
BOOL daNpc_Ba1_c::_execute() {
    if (!mbRanExecute) {
        mInitialAngle.y = current.angle.y;
        mInitialAngle.z = current.angle.z;
        mInitialPos.copy(current.pos);
        mInitialAngle.x = current.angle.x;
        mbRanExecute = true;
    }
    daNpc_Ba1_HIO_c::hio_prm_c& prm = l_HIO.mPrmTbl;
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
            fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
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

/* 021EF7EC */
BOOL daNpc_Ba1_c::_delete() {
    dComIfG_resDelete(&mPhs, "Ba");
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}

/* 021EECB8 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return static_cast<daNpc_Ba1_c*>(i_this)->CreateHeap();
}

/* 021EF6AC */
cPhs_State daNpc_Ba1_c::_create() {
    /* fopAcM_ct(this, daNpc_Ba1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            __ct(static_cast<fopNpc_npc_c*>(this)); /* fopNpc_npc_c::fopNpc_npc_c (the matcher calls it cDyl_LinkASync) */
            __vtbl = BA1_VTBL;
            mDoExt_btpAnm::ct(&mHeadBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            ba1_constructEventCut(&mEventCut);  /* dNpc_EventCut_c::dNpc_EventCut_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, "Ba" /* "Ba" */);
    if (state != cPhs_COMPLEATE_e) {
        return state;
    }
    if (!charDecide(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }

    if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, a_size_tbl[0] /* a_size_tbl[mType]: HD reads [0] */)) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -50.0f, -20.0f, -50.0f, 50.0f, 120.0f, 50.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}

/* 021EE764 */
J3DModelData* daNpc_Ba1_c::create_Anm() {
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes("Ba", dRes_ID_BA_BDL_BA_e);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2869, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_ba1.cpp", 0xB35, "a_mdl_dat != (0)");
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes("Ba", dRes_ID_BA_BCK_WAIT01_e);
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, anm, 2 /* J3DFrameCtrl::EMode_LOOP */, 1.0f, 0, -1, 1,
                                    nullptr, 0x80000, 0x11020022);
    if (mpMorf.get() == nullptr) {
        return nullptr;
    }
    if (mpMorf->getModel() == nullptr) {
        /* delete mpMorf (HD: virtual deleting destructor, vtable at +0, slot +0xC) */
        mDoExt_McaMorf* morf = mpMorf;
        if (morf != nullptr) {
            Ba1MorfView* view = reinterpret_cast<Ba1MorfView*>(morf);
            gabi::call_ptr(view->mpVtable->mDeletingDestructor, morf, 3);
        }
        mpMorf = nullptr;
        return nullptr;
    }
    m_hed_jnt_num = JUTNameTab_getIndex(gabi::ea(a_mdl_dat->getJointName()), "head" /* "head" */);
    if (m_hed_jnt_num < 0) /* JUT_ASSERT(2891, m_hed_jnt_num >= 0) */
        JUT_ASSERT_fail("d_a_npc_ba1.cpp", 0xB4B, "m_hed_jnt_num >= 0");
    m_bbone_jnt_num = JUTNameTab_getIndex(gabi::ea(a_mdl_dat->getJointName()), "backbone" /* "backbone" */);
    if (m_bbone_jnt_num < 0) /* JUT_ASSERT(2894, m_bbone_jnt_num >= 0) */
        JUT_ASSERT_fail("d_a_npc_ba1.cpp", 0xB4E, "m_bbone_jnt_num >= 0");
    m_footL_jnt_num = JUTNameTab_getIndex(gabi::ea(a_mdl_dat->getJointName()), "footL" /* "footL" */);
    if (m_footL_jnt_num < 0) /* JUT_ASSERT(2897, m_footL_jnt_num >= 0) */
        JUT_ASSERT_fail("d_a_npc_ba1.cpp", 0xB51, "m_footL_jnt_num >= 0");
    return a_mdl_dat;
}

/* 021EEAA8 */
bool daNpc_Ba1_c::create_itm_Mdl() {
    mpClothModel = nullptr;
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes("Ba", dRes_ID_BA_BDL_BA_CLOTH_e);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2926, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_ba1.cpp", 0xB6E, "a_mdl_dat != (0)");
    mpClothModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000002);
    return true;
}

/* 021EEB48 */
BOOL daNpc_Ba1_c::CreateHeap() {
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
        for (u16 i = 0; i < anm_model->getJointNum(); i++) {
            if ((i == (u32)(s32)m_hed_jnt_num) || (i == (u32)(s32)m_bbone_jnt_num)) {
                /* mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack_Ba1) */
                J3DModelData* md = mpMorf->getModel()->getModelData();
                md->getJointNodePointer(i)->setCallBack(nodeCallBack_Ba1);
            }
        }
        mpMorf->getModel()->mUserArea = gabi::ea(this); /* setUserArea(this) */
        mAcchCir.SetWall(30.0f, 50.0f);
        mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
        return TRUE;
    }
    mpMorf = nullptr;
    return FALSE;
}

/* 021EF7E8 */
static cPhs_State daNpc_Ba1_Create(fopAc_ac_c* i_this) {
    return ((daNpc_Ba1_c*)i_this)->_create();
}

/* 021EF844 */
static BOOL daNpc_Ba1_Delete(daNpc_Ba1_c* i_this) {
    return i_this->_delete();
}

/* 021F1288 */
static BOOL daNpc_Ba1_Execute(daNpc_Ba1_c* i_this) {
    return i_this->_execute();
}

/* 021F1460 */
static BOOL daNpc_Ba1_Draw(daNpc_Ba1_c* i_this) {
    return i_this->_draw();
}

/* 021F1464 */
static BOOL daNpc_Ba1_IsDelete(daNpc_Ba1_c*) {
    return TRUE;
}

// HD: compiler-generated initialization and deleting destructors.

/* 021F2C24 */
static void __sinit_d_a_npc_ba1_cpp() {

    __sinit_header_statics();
    daNpc_Ba1_HIO_c_ct(&l_HIO); /* static daNpc_Ba1_HIO_c l_HIO */
}

/* 021F2CC4 */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 021F2CD8 */
static void daNpc_Ba1_c_dt(daNpc_Ba1_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(&i_this->mCyl);
        __dt(&i_this->mStts);
        __dt(&i_this->mAcchCir);
        __dt(&i_this->mObjAcch);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 021F2D74 */
static void SafeString_assureTerminationImpl(SafeString*) {
}

/* WWHD glue
data l_wait_action1 10016830
data l_wait_action2 10016838
data l_demo_action1 10016840
data l_wait_action3 10016848
data l_wait_action4 10016850
data a_prm_tbl 101BBB20
data debugRedInitialized 101FDA50 nocheck
data debugBlueInitialized 101FDAC0 nocheck
data debugYellowInitialized 101FDA44 nocheck
data debugRed 101FEBF4 nocheck
data debugBlue 101FEBF8 nocheck
data debugYellow 101FEBE8 nocheck
data a_debugRed 10016860
data a_debugBlue 10016864
data a_debugYellow 10016868
data a_size_tbl 101BB894
data l_HIO 10465CD8 nocheck
data l_check_wrk 10465CC8 nocheck
data l_check_inf 10465D24 nocheck
data a_eye_pos_off 10465D18 nocheck
data eyeOffsetInitialized 10465D74 nocheck
data l_messageManager 101F4B5C nocheck
data l_demoObject 101D5FFC nocheck
data l_demoFrame 101D600C nocheck
data l_startStageName 1047E6B8 nocheck
str 10016A54 in 021EF978
ext 025F795C u32 ba1_getMessageStatus(u32 manager)
ext 021EE714 u32 ba1_checkItemResult(void* self, int itemButton)
ext 021EE9B0 u32 ba1_setBtpResult(daNpc_Ba1_c* self, u32 modify, s32 btp)
ext 025D54C4 void ba1_searchByID(fpc_ProcID id, gptr<fopAc_ac_c>* out)
ext 02526E70 void* ba1_getDemoActor(u32 object, u8 id)
ext 02527828 J3DAnmTexPattern* ba1_getDemoBtp(void* actor, const char* arc)
ext 021F2D74 void ba1_assureString(SafeString* string)
ext 0259F740 void ba1_constructEventCut(dNpc_EventCut_c* cut)
str 10016A48 in 021EF6AC
str 10016A4E in 021EF978
str 100168D0 in 021EF978
str 100168F8 in 021EF978
str 10016A38 in 021EF4BC
str 10016984 in 021EE764
str 10016928 in 021EE764
str 10016914 in 021EE764
str 1001696C in 021EE764
str 10016960 in 021EE764
str 1001694C in 021EE764
str 10016920 in 021EE764
str 1001691C in 021EE764
str 10016938 in 021EE764
str 10016B04 in 021F24B8
str 10016ADC in 021F0964
str 10016AC0 in 021F0474
str 10016AB8 in 021F01EC
str 10016AB0 in 021F01EC
str 10016AA8 in 021F01EC
str 10016AA0 in 021F01EC
str 10016A98 in 021F01EC
str 10016A90 in 021F01EC
str 10016A84 in 021EFD10
str 10016A4B in 021EF7EC
str 10016A0C in 021EEAA8
str 100169FC in 021EEAA8
str 100169F8 in 021EEAA8
str 100169E0 in 021EE9B0
str 100169D0 in 021EE9B0
str 100169CC in 021EE9B0
data a_btp_resID_tbl 1001699C
data a_bck_resID_tbl 10016A5C
data a_anm_num_prm_tbl 101BB898
data a_anm_status_prm_tbl 101BB950
data a_anm_atr_prm_tbl 101BB9D0
data l_evn_tbl 101BB85C
str 100168EC
str 100168E0
str 10016858
str 10016870
str 10016878
str 10016880
data a_cut_tbl 101BB948
str 10016AE4
str 10016AEC
ext 02527028 BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7)
ext 024EEEB8 u8 dBgS_GetPolyColor(dBgS* bgs, void* poly)
ext 024EF130 s8 dBgS_GetRoomId(dBgS* bgs, void* poly)
ext 027DF9B0 s8 JUTNameTab_getIndex(u32 tab, const char* name)
ext 02586D5C void dLetter_send(u16 no)
ext 0259DED0 void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly)
ext 0252012C void dComIfGp_setNextStage(const char* stage, s16 point, s8 roomNo, s8 layer, f32 lastSpeed, u32 lastMode, s32 setPoint, s8 wipe)
ext 024EE464 fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i)
ext 024EC8D0 fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i)
ext 024EDFCC bool dAttention_LockonTruth(dAttention_c* a)
ext 025E789C s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry)
*/
