/**
 * d_a_npc_ls1.cpp (WWHD)
 * NPC - Aryll
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_ls1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_ls1.h"

const gc_tu_data gc_tu = {.safeString = 0x1001D0CC, .objAcchDt = {0x1001D0E4, 0x1001D0F4}, .headerStatics = {0x10467964, 0x101C021C, 0x10467970}};

const Ls1TuData ls1_tu = {0x1001D460, 0x1001D448, 0x1001D104};
static daNpc_Ls1_HIO_c l_HIO;
static Ls1SignedWord l_checkWork;
static Ls1ActorSearch l_checkInfo;
static cXyz l_eyePosOffset;
static Ls1Word l_eyePosGuard;
static Ls1Word l_currentModel;
static Mtx34 l_currentMatrix;
static Ls1BtpResources l_btpResources;
static Ls1BtkResources l_btkResources;
static Ls1BckResources l_bckResources;
static Ls1BtkMapping l_btkMapping;
static Ls1ProcFunc l_waitAction;
static Ls1ProcFunc l_demoAction;
static Ls1EyeMaterialNames l_eyeMaterialNames;
static Ls1EventNames l_eventNames;
static dCcD_SrcCyl l_cylinderSource;
static Ls1Animations l_animationParameters;
static Ls1StateAnimations l_stateAnimations;
static Ls1AttributeAnimations l_attributeAnimations;
static Ls1MessageBtpMapping l_messageBtpMapping;
static Ls1Word l_counter;
static Ls1Word l_messageManager;
static Ls1EventPositions l_eventPositions;
static Ls1HeapSizes l_heapSizes;
static daNpc_Ls1_HIO_c::hio_prm_c l_hioParameters;
static Ls1EventNames l_cutNames;

static Ls1BodyTextureName l_bodyTextureName;
static Ls1Word l_demoObject;
static Ls1Word l_debugColorGuard0;
static Ls1Word l_debugColorGuard1;
static Ls1Word l_debugColorGuard2;
static Ls1Word l_debugColorGuard3;
static Ls1Word l_debugColor0;
static Ls1Word l_debugColor1;
static Ls1Word l_debugColor2;
static Ls1Word l_debugColor3;
static Ls1Word l_debugColorSource0;
static Ls1Word l_debugColorSource1;
static Ls1Word l_debugColorSource2;
static Ls1Word l_debugColorSource3;

enum { fpcNm_KAMOME_e = 0xC2, fpcNm_NPC_BM1_e = 0x146 };
enum { dSv_event_flag_UNK_0001 = 0x0001, dSv_event_flag_UNK_0280 = 0x0280, dSv_event_flag_UNK_0310 = 0x0310, dSv_event_flag_UNK_2A80 = 0x2A80 };

// HD: retain the raw event-mode byte; the shared BOOL accessor normalizes it.
static inline u8 ls1EventModeByte() { return gabi::at<dEvt_control_c>(dComIfGp_ea() + PLAY_EVTCTRL)->mMode; }

static inline s32 J3DAnm_getFrameMax(void* p) {
    auto* animation = reinterpret_cast<Ls1Animation*>(p);
    return gabi::call_ptr<s32>(animation->mVtable->mFrameMax.get(), p);
}

static inline J3DModelData* getModelData(J3DModel* m) { return m->mModelData; }

static inline void* daNpc_gndPoly(fopNpc_npc_c* a) { return static_cast<cBgS_PolyInfo*>(&a->mObjAcch.m_gnd); }
static inline s8 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return ls1Guest_eventActionIndex(dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}

static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    return gabi::ea(d->getJointName());
}

static inline u32 fopMsgM_getStatus() { return ls1Guest_messageStatus((u32)l_messageManager.mValue); }

// HD: the name-table offset is relative to its own word. Keep the header load
// before the SafeString virtual call, as in the verified inline.
static Ls1Material* eyeMaterial(J3DModelData* data, int index) {
    SafeString* name = &l_eyeMaterialNames.mNames[index];
    auto* header = gabi::at<Ls1ResourceHeader>(data->mpResHeader);
    auto* vtable = gabi::at<Ls1SafeStringVtable>(name->__vtbl);
    gabi::call_ptr(vtable->mAssureTermination.get(), name);
    u32 offset = header->mMaterialNamesOffset;
    u32 table = offset ? gabi::ea(&header->mMaterialNamesOffset) + offset : 0;
    s32 materialIndex = ls1Guest_materialIndex(table, gabi::at<const char>(name->mStringTop));
    if (materialIndex < 0)
        return nullptr;
    u32 material = data->mMaterials;
    if ((u32)materialIndex < data->mMaterialNum)
        material += materialIndex * 0x39C;
    return gabi::at<Ls1Material>(material);
}

/* GHS pointer to member function call */
static inline void pmf_load(Ls1ProcFunc* dst, const Ls1ProcFunc* src) {
    auto* destination = reinterpret_cast<be<u32>*>(dst);
    auto* source = reinterpret_cast<const be<u32>*>(src);
    destination[0] = source[0];
    destination[1] = source[1];
}
static inline void pmf_call(daNpc_Ls1_c* self, Ls1ProcFunc* pmf, void* arg) {
    (self->*(*pmf))(arg);
}

// HD: joint matrices are reached through the model matrix buffer.
static Mtx34* getAnmMtx(J3DModel* model, s32 jntNo) {
    return model->getAnmMtx(jntNo);
}
static inline u32 jntNo_of(J3DNode* node) { return reinterpret_cast<Ls1JointNumber*>(J3DNode_toJoint(node))->mJointNo; }
static inline Mtx34* j3dSys_mCurrentMtx() { return &l_currentMatrix; }

/* 0227D7DC */
void daNpc_Ls1_c::_nodeCB_Head(J3DNode* i_node, J3DModel* i_model) {
    // HD: preserve the guarded initialization order of the eye offset.
    cXyz* a_eye_pos_off = &l_eyePosOffset;
    if (l_eyePosGuard.mValue == 0) {
        l_eyePosGuard.mValue = 1;
        a_eye_pos_off->z = 0.0f;
        a_eye_pos_off->x = 14.0f;
        a_eye_pos_off->y = 18.0f;
    }
    u32 jnt_no = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jnt_no), stk);
    m7E4.x = stk->m[0][3];
    m7E4.y = stk->m[1][3];
    m7E4.z = stk->m[2][3];
    mDoMtx_XrotM(stk, mHalfHeadAngleY);
    mDoMtx_ZrotM(stk, -mHalfHeadAngleX);
    PSMTXMultVec(stk, a_eye_pos_off, &mTransformedEyePos);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), stk);
}

/* node callbacks: if (calcTiming == In && j3dSys.getModel()->getUserArea()) actor->_nodeCB_X(node, model) */
static inline void nodeCB_dispatch(J3DNode* node, int timing, void (*callback)(daNpc_Ls1_c*, J3DNode*, J3DModel*)) {
    if (timing == 0) {
        J3DModel* model = gabi::at<J3DModel>(l_currentModel.mValue);
        u32 user = model->mUserArea;
        if (user != 0)
            callback(gabi::at<daNpc_Ls1_c>(user), node, model);
    }
}

/* 0227D954 */
static BOOL nodeCB_Head(J3DNode* i_node, int i_calcTiming) {
    nodeCB_dispatch(i_node, i_calcTiming, ls1Guest_node_head);
    return TRUE;
}

/* 0227D99C */
void daNpc_Ls1_c::_nodeCB_BackBone(J3DNode* i_node, J3DModel* i_model) {
    u32 jnt_no = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jnt_no), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stk, -m_jnt.mAngles[1][0]);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), stk);
}

/* 0227DAC0 */
static BOOL nodeCB_BackBone(J3DNode* i_node, int i_calcTiming) {
    nodeCB_dispatch(i_node, i_calcTiming, ls1Guest_node_backbone);
    return TRUE;
}

/* 0227DB08 */
void daNpc_Ls1_c::_nodeCB_Hand_L(J3DNode* i_node, J3DModel* i_model) {
    u32 jnt_no = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jnt_no), stk);
    PSMTXCopy(stk, &mHand_L_Mtx);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), stk);
}

/* 0227DC18 */
static BOOL nodeCB_Hand_L(J3DNode* i_node, int i_calcTiming) {
    nodeCB_dispatch(i_node, i_calcTiming, ls1Guest_node_leftHand);
    return TRUE;
}

/* 0227DC60 */
void daNpc_Ls1_c::_nodeCB_Hand_R(J3DNode* i_node, J3DModel* i_model) {
    u32 jnt_no = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jnt_no), stk);
    PSMTXCopy(stk, &mHand_R_Mtx);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), stk);
}

/* 0227DD70 */
static BOOL nodeCB_Hand_R(J3DNode* i_node, int i_calcTiming) {
    nodeCB_dispatch(i_node, i_calcTiming, ls1Guest_node_rightHand);
    return TRUE;
}

/* 0227DDB8 */
void daNpc_Ls1_c::_Ls_hand_nodeCB_Hand_L(J3DNode* i_node, J3DModel* i_model) {
    u32 jnt_no = jntNo_of(i_node);
    PSMTXCopy(&mHand_L_Mtx, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), &mHand_L_Mtx);
}

/* 0227DE8C */
static BOOL Ls_hand_nodeCB_Hand_L(J3DNode* i_node, int i_calcTiming) {
    nodeCB_dispatch(i_node, i_calcTiming, ls1Guest_node_leftHandModel);
    return TRUE;
}

/* 0227DED4 */
void daNpc_Ls1_c::_Ls_hand_nodeCB_Hand_R(J3DNode* i_node, J3DModel* i_model) {
    u32 jnt_no = jntNo_of(i_node);
    PSMTXCopy(&mHand_R_Mtx, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), &mHand_R_Mtx);
}

/* 0227DFA8 */
static BOOL Ls_hand_nodeCB_Hand_R(J3DNode* i_node, int i_calcTiming) {
    nodeCB_dispatch(i_node, i_calcTiming, ls1Guest_node_rightHandModel);
    return TRUE;
}

// daNpc_Ls1_matAnm_c::daNpc_Ls1_matAnm_c (HD: allocates when this == NULL; the
// J3DMaterialAnm base constructor is inlined: only the vtable is written)
/* 0227DFF0 */
static daNpc_Ls1_matAnm_c* daNpc_Ls1_matAnm_c_ct(daNpc_Ls1_matAnm_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Ls1_matAnm_c*)operator_new(0x80);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->mbMove = 0;
    i_this->mOffset.x = 0.0f;
    i_this->__vtbl = ls1_tu.mMaterialVtable;
    i_this->mOffset.y = 0.0f;
    return i_this;
}

/* 0227E048 */
int daNpc_Ls1_c::btpResID(int i_btpNum) {
    return l_btpResources.mResources[i_btpNum];
}

/* 0227E05C */
u32 daNpc_Ls1_c::setBtp(s8 i_btpNum, u32 i_bModify) {
    J3DModel* morf_model_p = mpMorf->getModel();
    if (i_btpNum < 0) {
        return false;
    }
    void* a_btp = dComIfG_getObjectIDRes(mArcName, btpResID(i_btpNum));
    // HD: no JUT_ASSERT
    mBtpNum = i_btpNum;
    mBtpFrame = 0;
    mTimer1 = 0;
    return ls1Guest_mDoExt_btpAnm_init(&mBtpAnm, getModelData(morf_model_p), a_btp, TRUE, 0, 1.0f, 0, -1, i_bModify, 0) != 0;
}

/* 0227E14C */
int daNpc_Ls1_c::btkResID(int i_btkNum) {
    return l_btkResources.mResources[i_btkNum];
}

/* 0227E160 */
void daNpc_Ls1_c::setMat() {
    // HD: the eye materials are found by name instead of
// the btk's update material ids
    auto* btk = reinterpret_cast<Ls1BtkAnimation*>(reinterpret_cast<Ls1BtkWork*>(&mBtkAnm)->mpAnm.get());
    u16 material_num = btk->mMaterialInfo->mMaterialNum;
    for (u16 i = 0; i < material_num; i++) {
        J3DModelData* md = getModelData(mpMorf->getModel());
        mpMatAnms[i] = eyeMaterial(md, i)->mMaterialAnimation;
    }
}

/* 0227E244 */
u32 daNpc_Ls1_c::setBtk(s8 i_btkNum, u32 i_bModify) {
    J3DModel* morf_model_p = mpMorf->getModel();
    if (i_btkNum < 0) {
        return false;
    }
    void* a_btk = dComIfG_getObjectIDRes(mArcName, btkResID(i_btkNum));
    mBtkNum = i_btkNum;
    mBtkFrame = 0;
    if (ls1Guest_mDoExt_btkAnm_init(&mBtkAnm, getModelData(morf_model_p), a_btk, TRUE, 0, 1.0f, 0, -1, i_bModify, 0) != 0) {
        if (!i_bModify) {
            setMat();
        }
        return true;
    }
    return false;
}

/* 0227E344 */
u32 daNpc_Ls1_c::init_texPttrnAnm(s8 i_btpNum, u32 i_bModify) {
    if (setBtp(i_btpNum, i_bModify) == false) {
        return false;
    }
    /* a_btk_num_tbl  */
    return setBtk(l_btkMapping.mAnimations[i_btpNum], i_bModify);
}

/* 0227ED64 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return static_cast<daNpc_Ls1_c*>(i_this)->CreateHeap();
}

/* 0227ED68 */
static void* searchActor_Bm1(void* i_actorP, void*) {
    if (l_checkWork.mValue < 20 && fopAc_IsActor(i_actorP) && i_actorP != nullptr && fpcM_GetName(i_actorP) == fpcNm_NPC_BM1_e) {
        s32 n = l_checkWork.mValue;
        l_checkWork.mValue = n + 1;
        l_checkInfo.mActors[n] = (fopAc_ac_c*)i_actorP;
    }
    return nullptr;
}

/* 0227EDE8 */
static void* searchActor_kamome_Set_NOSTOP_DEMO(void* i_actorP, void*) {
    if (fopAc_IsActor(i_actorP) && i_actorP != nullptr && fpcM_GetName(i_actorP) == fpcNm_KAMOME_e) {
        ((fopAc_ac_c*)i_actorP)->actor_status |= 0x4000u; /* fopAcM_OnStatus(fopAcStts_UNK4000_e) */
    }
    return nullptr;
}

/* 0227EE40 */
static void* searchActor_kamome_Clr_NOSTOP_DEMO(void* i_actorP, void*) {
    if (fopAc_IsActor(i_actorP) && i_actorP != nullptr && fpcM_GetName(i_actorP) == fpcNm_KAMOME_e) {
        ((fopAc_ac_c*)i_actorP)->actor_status &= ~0x4000u;
    }
    return nullptr;
}

/* 0227EE98 */
u8 daNpc_Ls1_c::decideType(int i_type) {
    if (m854 > 0) {
        return true;
    }
    m854 = 1;
    switch ((u32)i_type) {
    case 0:
        mType = -1;
        if (dComIfGs_isEventBit(dSv_event_flag_UNK_2A80)) {
            mType = 0;
        }
        break;
    case 1:
    case 2:
    case 4:
        mType = (s8)i_type;
        break;
    case 3:
        mType = -1;
        if (!dComIfGs_isEventBit(dSv_event_flag_UNK_2A80)) {
            mType = 3;
        }
        break;
    default:
        mType = -1;
        return false;
    }
    if (mType < 0) {
        return false;
    }
    // Copy the archive name through byte channels in guest memory.
    auto* archive = reinterpret_cast<be<u8>*>(mArcName);
    archive[0] = 'L';
    archive[1] = 's';
    archive[2] = 0;
    return m854 != -1 && mType != -1;
}

/* 0227EFF4 */
BOOL daNpc_Ls1_c::set_action(Ls1ProcFunc* i_newProcFunc, void* i_argsP) {
    Ls1ProcFunc* cur = &mCurrProcFunc;
    s16 newI = i_newProcFunc->i;
    s16 newD;
    u32 newF;
    if ((u16)cur->i == (u16)newI) {
        if (cur->i == 0)
            return TRUE;
        newD = i_newProcFunc->d;
        newF = i_newProcFunc->f;
        if (!((u16)cur->d != (u16)newD || cur->f != newF))
            return TRUE;
    } else {
        newF = i_newProcFunc->f;
        newD = i_newProcFunc->d;
    }
    if (cur->i != 0) {
        m856 = 9;
        pmf_call(this, cur, i_argsP);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    m856 = 0;
    pmf_call(this, cur, i_argsP);
    return TRUE;
}

/* 0227F120 */
bool daNpc_Ls1_c::init_LS1_0() {
    gabi::Local<Ls1ProcFunc> pmf;
    pmf_load(pmf, &l_waitAction);
    set_action(pmf, nullptr);
    actor_status &= ~0x80u; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
    return true;
}

/* 0227F178 */
bool daNpc_Ls1_c::init_LS1_1() {
    gabi::Local<Ls1ProcFunc> pmf;
    pmf_load(pmf, &l_demoAction);
    set_action(pmf, nullptr);
    return true;
}

// HD: these tail calls pass the entire result register through.
/* 0227F1B8 */
static u32 init_LS1_2(daNpc_Ls1_c* i_this) {
    return ls1Guest_initDemoRegister(i_this); /* init_LS1_1() */
}
/* 0227F1BC */
static u32 init_LS1_3(daNpc_Ls1_c* i_this) {
    return ls1Guest_initWaitRegister(i_this); /* init_LS1_0() */
}
/* 0227F1C0 */
static u32 init_LS1_4(daNpc_Ls1_c* i_this) {
    return ls1Guest_initDemoRegister(i_this); /* init_LS1_1() */
}
bool daNpc_Ls1_c::init_LS1_2() { return ls1Guest_initType2(this); }
bool daNpc_Ls1_c::init_LS1_3() { return ls1Guest_initType3(this); }
bool daNpc_Ls1_c::init_LS1_4() { return ls1Guest_initType4(this); }

/* 0227F1C4 */
void daNpc_Ls1_c::setEyeCtrl() {
    for (int i = 0; i < 2; i++) {
        daNpc_Ls1_matAnm_c* mat_anm_p = mpMatAnms[i];
        if (mat_anm_p) {
            mat_anm_p->mbMove = 1;
        }
    }
    mbEyeCtrlSet = true;
}

/* 0227F1FC */
void daNpc_Ls1_c::clrEyeCtrl() {
    for (int i = 0; i < 2; i++) {
        daNpc_Ls1_matAnm_c* mat_anm_p = mpMatAnms[i];
        if (mat_anm_p) {
            mat_anm_p->mbMove = 0;
        }
    }
    mbEyeCtrlSet = false;
}

/* 0227F234 */
void daNpc_Ls1_c::play_btp_anm() {
    u8 frame_max = (u8)J3DAnm_getFrameMax(mBtpAnm.mpAnm.get()); /* mBtpAnm.getBtpAnm() */
    if (mBtpNum != BTPNUM_MABA_e || ls1Guest_cLib_calcTimer(&mTimer1) == 0) {
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame;
        if (frame >= frame_max) {
            if (mBtpNum != BTPNUM_MABA_e) {
                mBtpFrame = frame_max;
            } else {
                mTimer1 = (s16)ls1Guest_cLib_getRndValue(60, 90);
                mBtpFrame = 0;
            }
        }
    }
}

/* fsel-based clamp to 1.0 (cLib_minMaxLimit's upper bound, NaN kept) */
static inline f32 clamp_m1_1(f32 v) {
    if (v < -1.0f)
        return -1.0f;
    return (v - 1.0f >= 0.0f) ? 1.0f : v;
}

/* 0227F2DC */
void daNpc_Ls1_c::eye_ctrl() {
    f32 fVar1, fVar2;
    if (mbEyeCtrlSet) {
        f32 div = (f32)(s32)l_HIO.mPrm.m30;
        fVar1 = ((f32)(s32)m848 / div) * 0.1f;
        fVar2 = ((f32)(s32)m846 / div) * 0.1f;
        fVar1 = clamp_m1_1(fVar1);
        fVar2 = clamp_m1_1(fVar2);
    } else {
        fVar1 = 0.0f;
        fVar2 = fVar1;
    }
    if (mpMatAnms[0].get()) {
        ls1Guest_cLib_addCalc(&mpMatAnms[0]->mOffset.x, fVar2, 0.5f, 0.1f, 0.03f);
        ls1Guest_cLib_addCalc(&mpMatAnms[0]->mOffset.y, fVar1, 0.5f, 0.1f, 0.03f);
    }
    fVar2 = -fVar2;
    if (mpMatAnms[1].get()) {
        ls1Guest_cLib_addCalc(&mpMatAnms[1]->mOffset.x, fVar2, 0.5f, 0.1f, 0.03f);
        ls1Guest_cLib_addCalc(&mpMatAnms[1]->mOffset.y, fVar1, 0.5f, 0.1f, 0.03f);
    }
}

/* 0227F514 */
void daNpc_Ls1_c::play_btk_anm() {
    u8 frame_max = (u8)J3DAnm_getFrameMax(reinterpret_cast<Ls1BtkWork*>(&mBtkAnm)->mpAnm.get()); /* mBtkAnm.getBtkAnm() */
    if (m838) {
        eye_ctrl();
    } else {
        u8 frame = (u8)(mBtkFrame + 1);
        if (frame >= frame_max) {
            mBtkFrame = frame_max;
        } else {
            mBtkFrame = frame;
        }
    }
}

/* 0227F5AC */
void daNpc_Ls1_c::play_animation() {
    u32 snd_id = 0;
    if (mBtkNum == 0 && m838 != 0) {
        setEyeCtrl();
    } else {
        clrEyeCtrl();
    }
    play_btp_anm();
    play_btk_anm();
    if (mObjAcch.m_flags & 0x20) { /* mObjAcch.ChkGroundHit() */
        snd_id = ls1Guest_dBgS_GetMtrlSndId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    }
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    mbMorfAnimStopped = (s8)mpMorf->play(&eyePos, snd_id, (s8)reverb);
    if (mpMorf->getFrame() < mPrevMorfFrame) {
        mbMorfAnimStopped = true;
    }
    mPrevMorfFrame = mpMorf->getFrame();
}

/* 0227F69C */
void daNpc_Ls1_c::setAttention(u32 i_setEyePos) {
    cXyz* attPos = &attention_info.position; /* attention_info.position */
    f32 y = current.pos.y + l_HIO.mPrm.mAttPosOffsetY;
    attPos->x = current.pos.x;
    attPos->y = y;
    attPos->z = current.pos.z;
    if (!mbSetEyePos && !i_setEyePos) {
        return;
    }
    eyePos.z = mTransformedEyePos.z;
    eyePos.y = mTransformedEyePos.y;
    eyePos.x = mTransformedEyePos.x;
}

/* 0227F6F0 */
void daNpc_Ls1_c::setMtx(u32 i_setEyePos) {
    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), mAngle.x, mAngle.y, mAngle.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    J3DModel_calc(mpLsHandModel);
    if (mpTelescopeModel.get()) {
        PSMTXCopy(getAnmMtx(mpMorf->getModel(), m_hnd_R_jnt_num), mDoMtx_stack_c::get());
        if (m841) {
            mDoMtx_stack_c::transM(5.7f, -17.5f, -1.0f);
        } else {
            mDoMtx_stack_c::transM(5.5f, -3.0f, -2.0f);
        }
        f32 s = mTelescopeScale;
        mDoMtx_stack_c::scaleM(s, s, s);
        ls1Guest_mDoMtx_XYZrotM(mDoMtx_stack_c::get(), -0x1D27, 0x3B05, -0x5C71);
        J3DModel_setBaseTRMtx(mpTelescopeModel, mDoMtx_stack_c::get());
        J3DModel_calc(mpTelescopeModel);
    }
    setAttention(i_setEyePos);
}

/* 0227F9A8 */
bool daNpc_Ls1_c::createInit() {
    /* l_evn_tbl : "zelda_fly", "omedeto", "get_telescope", "eTalk" */
    for (int i = 0; i < 4; i++) {
        const char* name = l_eventNames.mNames[i].get();
        mEventIDTbl[i] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    }
    mEventCut.setActorInfo2("Ls1" /* "Ls1" */, (fopNpc_npc_c*)(void*)this);
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xA9; /* attention_info.distances[SPEAK] */
    attention_info.flags = fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e; /* attention_info.flags */
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xA9; /* attention_info.distances[TALK] */
    mBckNum = BCKNUM_NULL_e;
    gravity = 0.0f;
    bool init_result;
    switch (mType) {
    case 0:
        init_result = init_LS1_0();
        break;
    case 1:
        init_result = init_LS1_1();
        break;
    case 2:
        init_result = init_LS1_2();
        break;
    case 3:
        init_result = init_LS1_3();
        break;
    case 4:
        init_result = init_LS1_4();
        break;
    default:
        init_result = false;
        break;
    }
    if (!init_result) {
        return false;
    }
    mAngle.x = current.angle.x;
    mAngle.y = current.angle.y;
    mAngle.z = current.angle.z;
    shape_angle.x = mAngle.x;
    shape_angle.y = mAngle.y;
    shape_angle.z = mAngle.z;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(&l_cylinderSource /* dNpc_cyl_src */);
    play_animation();
    if (mType) {
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    tevStr.mRoomNo = ls1Guest_dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    tevStr.mEnvrIdxOverride = ls1Guest_dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this));
    mpMorf->setMorf(0.0f);
    setMtx(true);
    return true;
}

/* 0227FD40 */
static cPhs_State daNpc_Ls1_Create(fopAc_ac_c* i_this) {
    return ((daNpc_Ls1_c*)i_this)->_create();
}

/* 0227FD44 */
BOOL daNpc_Ls1_c::_delete() {
    dComIfG_resDelete(&mPhs, mArcName);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}

/* 0227FD98 */
static BOOL daNpc_Ls1_Delete(daNpc_Ls1_c* i_this) {
    return i_this->_delete();
}

/* 0227FD9C */
u8 daNpc_Ls1_c::partner_search_sub(u32 i_judgeFunc) {
    bool result = false;
    mBm1ProcID = 0xFFFFFFFF;
    l_checkWork.mValue = 0;
    for (int i = 0; i < 20; i++) {
        l_checkInfo.mActors[i] = nullptr;
    }
    fpcM_Search(i_judgeFunc, this);
    if (l_checkWork.mValue != 0) {
        mBm1ProcID = fopAcM_GetID(l_checkInfo.mActors[0].get());
        result = true;
    }
    return result;
}

/* 0227FE48 */
void daNpc_Ls1_c::partner_search() {
    bool temp = false;
    if (m856 == 1) {
        switch ((u32)(s32)mType) {
        case 0:
            temp = partner_search_sub(gabi::guest_fn((const void*)searchActor_Bm1));
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            temp = true;
            break;
        }
        if (temp) {
            m856 = m856 + 1;
        }
    }
}

/* 0227FED0 */
int daNpc_Ls1_c::bckResID(int i_bckNum) {
    return l_bckResources.mResources[i_bckNum];
}

/* 0227FEE4 */
void daNpc_Ls1_c::setAnm_anm(anm_prm_c* i_anmPrmP) {
    s8 bck = i_anmPrmP->bckNum;
    if (bck < 0 || mBckNum == bck) {
        return;
    }
    int resID = bckResID(bck);
    s32 loopMode = i_anmPrmP->loopMode;
    f32 morf = i_anmPrmP->morf;
    f32 speed = i_anmPrmP->speed;
    dNpc_setAnmIDRes(mpMorf, loopMode, morf, speed, resID, -1, mArcName);
    mbMorfAnimStopped = false;
    mBckNum = i_anmPrmP->bckNum;
    mPrevMorfFrame = 0.0f;
    m831 = 0;
}

/* 0227FF7C */
void daNpc_Ls1_c::setAnm_NUM(int i_anmNum, BOOL param_2) {
    anm_prm_c* a_anm_prm_tbl = l_animationParameters.mAnimations; /* [14] */
    if (param_2) {
        init_texPttrnAnm(a_anm_prm_tbl[i_anmNum].btpNum, true);
    }
    setAnm_anm(&a_anm_prm_tbl[i_anmNum]);
}

/* 02280548 */
void daNpc_Ls1_c::setAnm() {
    anm_prm_c* a_anm_prm_tbl = l_stateAnimations.mAnimations; /* [6] */
    init_texPttrnAnm(a_anm_prm_tbl[m851].btpNum, true);
    setAnm_anm(&a_anm_prm_tbl[m851]);
}

/* 022805B8 */
void daNpc_Ls1_c::setStt(s8 param_1) {
    s8 prev_m851 = m851;
    m851 = param_1;
    switch ((u32)(s32)param_1) {
    case 1:
        m850 = 0;
        mTimer5 = (s16)ls1Guest_cLib_getRndValue(60, 90);
        break;
    case 2:
        m833 = 0;
        mTimer3 = 0;
        m850 = 0;
        break;
    case 3:
        m850 = 0;
        break;
    case 4:
        m850 = 0;
        mTelescopeScale = 1.0f;
        break;
    case 5:
        fpcM_Search(gabi::guest_fn((const void*)searchActor_kamome_Set_NOSTOP_DEMO), this);
        m850 = 0;
        m857 = 0;
        mMesgAnimeTag = 0xFF;
        m84B = 0xFF;
        m852 = prev_m851;
        break;
    }
    setAnm();
}

/* 02280508 */
s32 daNpc_Ls1_c::isEventEntry() {
    const char* name = gabi::at<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}

/* 022806DC */
void daNpc_Ls1_c::endEvent() {
    dComIfGp_event_reset();
    m84B = 0xFF;
    mMesgAnimeTag = 0xFF;
}

/* 02280720 */
void daNpc_Ls1_c::cut_init_LOK_PLYER(int param_1) {
    be<s32>* prm_p = (be<s32>*)dComIfGp_evmng_getMyIntegerP(param_1, "prm_0" /* "prm_0" */);
    m853 = 1;
    m_jnt.mbBackBoneLock = 0; /* offBackBoneLock() */
    m840 = 0;
    m_jnt.mbHeadLock = 0;     /* offHeadLock() */
    if (prm_p) {
        s32 prm = *prm_p;
        if (prm & (1 << 3)) {
            m_jnt.mbTrn = 1; /* setTrn() */
        } else {
            if (prm & 1) {
                m840 = 1;
            }
            if (prm & (1 << 1)) {
                m_jnt.mbBackBoneLock = 1;
            }
            if (prm & (1 << 2)) {
                m_jnt.mbHeadLock = 1;
            }
        }
    }
}

/* 022807DC */
void daNpc_Ls1_c::cut_init_PLYER_MOV(int i_staffIdx) {
    dComIfGp_evmng_setGoal(&m7CC[1]);
}

/* 02280814 */
void daNpc_Ls1_c::cut_init_ANM_CHG(int param_1) {
    be<s32>* anm_no_p = (be<s32>*)dComIfGp_evmng_getMyIntegerP(param_1, "AnmNo" /* "AnmNo" */);
    int anm_no = mBckNum;
    if (anm_no_p) {
        anm_no = *anm_no_p;
    }
    setAnm_NUM(anm_no, TRUE);
}

/* 02280884 */
u32 daNpc_Ls1_c::cut_move_WAI() {
    if (mBckNum != BCKNUM_WAIT02_e) {
        return TRUE;
    }
    if (mbMorfAnimStopped) {
        setAnm_NUM(ANMNUM_WAIT01_e, TRUE);
        return TRUE;
    }
    return FALSE;
}

/* 022808F4 */
void daNpc_Ls1_c::privateCut(int i_staffIdx) {
    /* a_cut_tbl : "LOK_PLYER", "PLYER_MOV", "WAI", "ANM_CHG" */
    if (i_staffIdx == -1) {
        return;
    }
    mActionIndex = dComIfGp_evmng_getMyActIdx(i_staffIdx, gabi::ea(&l_cutNames), 4, TRUE, 0);
    if (mActionIndex == -1) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
        return;
    }
    if (dComIfGp_evmng_getIsAddvance(i_staffIdx)) {
        switch (mActionIndex) {
        case 0:
            cut_init_LOK_PLYER(i_staffIdx);
            break;
        case 1:
            cut_init_PLYER_MOV(i_staffIdx);
            break;
        case 2: /* cut_init_WAI: empty */
            break;
        case 3:
            cut_init_ANM_CHG(i_staffIdx);
            break;
        }
    }
    /* cut_move_LOK_PLYER / PLYER_MOV / ANM_CHG return TRUE (inlined) */
    bool cut_end = true;
    if (mActionIndex == 2) {
        cut_end = cut_move_WAI();
    }
    if (cut_end) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
    }
}

/* 02280A1C */
void daNpc_Ls1_c::event_proc(int i_staffIdx) {
    if (dComIfGp_evmng_endCheck(mEventIDTbl[mEventIndex])) {
        switch ((u32)(s32)mEventIndex) {
        case 0:
            // HD: no scope message status check, no camera restart and no player SCOPE_CANCEL
// flag; after releasing the scope mode the event ends in the same frame
            if (!m835) {
                if (!ls1Guest_fopMsgM_releaseScopeMode( (u32)l_messageManager.mValue)) { /* ls1Guest_fopMsgM_releaseScopeMode() */
                    return;
                }
                ls1Guest_bgmStop(4);           /* mDoAud_bgmStop(4) */
                ls1Guest_bgmStart(0x8000000Eu); /* mDoAud_bgmStart(0x8000000E) */
                m835 = true;
            }
            dComIfGs_onEventBit(1);
            setStt(3);
            m840 = 0;
            eventInfo.setEventId(mEventIDTbl[3]);
            m850 = 1;
            mEventIndex = 3;
            m83B = false;
            m_jnt.mbBackBoneLock = 0; /* offBackBoneLock() */
            m834 = false;
            break;
        case 1:
            m850 = 5;
            break;
        case 2:
            setStt(2);
            m850 = 1;
            m836 = true;
            break;
        }
        endEvent();
    } else {
        if (mEventCut.cutProc()) {
            return;
        }
        privateCut(i_staffIdx);
    }
}

/* 02281004 */
void daNpc_Ls1_c::eventOrder() {
    if (m850 == 1 || m850 == 2) {
        eventInfo.onCondition(dEvtCnd_CANTALK_e);
        if (m850 == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (m850 >= 3) {
        mEventIndex = m850 - 3;
        fopAcM_orderOtherEventId(this, mEventIDTbl[mEventIndex], 0xFF, 0xFFFF, 0, 1);
    }
}

/* 02281314 */
static BOOL daNpc_Ls1_Execute(daNpc_Ls1_c* i_this) {
    return i_this->_execute();
}

/* 02281558 */
static BOOL daNpc_Ls1_Draw(daNpc_Ls1_c* i_this) {
    return i_this->_draw();
}

/* 0228155C */
static BOOL daNpc_Ls1_IsDelete(daNpc_Ls1_c*) {
    return TRUE;
}

/* daNpc_Ls1_matAnm_c::calc(J3DMaterial*) (HD: the texture matrix is found through the
 * material's resource tables by the name "texmtx1"; the GameCube loops over 8 texture matrices) */
/* 02281564 */
static void daNpc_Ls1_matAnm_c_calc(daNpc_Ls1_matAnm_c* i_this, void* i_material) {
    auto* material = reinterpret_cast<Ls1MaterialTexture*>(i_material);
    Ls1TextureResource* header = material->mResource;
    s32 index = ls1Guest_materialIndex(header->mNames.address(), "texmtx1");
    header = material->mResource;
    auto* entry = gabi::at<Ls1TextureEntry>(header->mEntries.address() + index * 0x14);
    if (entry->mAnimationIndex >= 0) {
        material->mFlags |= 4;
        be<u32>* changed = material->mChangedEntries;
        changed[index >> 5] |= 1u << (index & 31);
        header = material->mResource;
    }
    u32 linkedIndex = entry->mLinkedEntry;
    auto* linkedEntry = gabi::at<Ls1TextureEntry>(header->mEntries.address() + linkedIndex * 0x14);
    if (linkedEntry->mAnimationIndex >= 0) {
        material->mFlags |= 4;
        be<u32>* changed = material->mChangedEntries;
        changed[(s32)linkedIndex >> 5] |= 1u << (linkedIndex & 31);
    }
    auto* srt = reinterpret_cast<Ls1TextureSrt*>(material->mSrtData.get() + entry->mSrtOffset);
    if (i_this->mbMove != 0) {
        srt->mTranslationX = i_this->mOffset.x;
        srt->mTranslationY = i_this->mOffset.y;
    }
}

/* 022816A4 */
u32 daNpc_Ls1_c::chngAnmTag() {
    u8 tag = mMesgAnimeTag;
    if (tag == 0) {
        return init_texPttrnAnm(BTPNUM_WARAI_e, true);
    } else if (tag >= 11 && tag <= 13) {
        /* 11: KIZUKU, 12: NGWARAI, 13: FUAN  */
        return init_texPttrnAnm(l_messageBtpMapping.mAnimations[tag - 11], true);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}

/* 022816E4 */
void daNpc_Ls1_c::setAnm_ATR(BOOL param_1) {
    anm_prm_c* a_anm_prm_tbl = l_attributeAnimations.mAnimations; /* [17] */
    if (param_1) {
        init_texPttrnAnm(a_anm_prm_tbl[m84B].btpNum, true);
    }
    setAnm_anm(&a_anm_prm_tbl[m84B]);
}

static inline u32 g_Counter_mCounter0() { return l_counter.mValue; }

/* 02281758 */
void daNpc_Ls1_c::chngAnmAtr(u8 param_1) {
    if ((m84B == 3 || m84B == 5) && (param_1 == 3 || param_1 == 5)) {
        init_texPttrnAnm(BTPNUM_WARAI_e, true);
        m84B = param_1;
        return;
    }
    // HD: >= 17 (GameCube: > 17)
    if (param_1 == m84B || param_1 >= 17) {
        return;
    }
    m84B = param_1;
    setAnm_ATR(TRUE);
    switch (param_1) {
    case 1:
        mTimer2 = (s16)ls1Guest_cLib_getRndValue(60, 90);
        break;
    case 3:
    case 5:
        mTimer2 = (g_Counter_mCounter0() & 3) + 3;
        break;
    case 7:
        mTelescopeScale = 0.0f;
        break;
    case 12:
    case 14:
    case 15:
    case 16:
        mTimer2 = 2;
        break;
    }
}

/* 022818EC */
void daNpc_Ls1_c::ctrlAnmAtr() {
    switch (m84B) {
    case 1:
        if (ls1Guest_cLib_calcTimer(&mTimer2) == 0) {
            m84B = 2;
            setAnm_ATR(TRUE);
        }
        break;
    case 2:
        if (mbMorfAnimStopped) {
            m84B = 1;
            setAnm_ATR(TRUE);
        }
        break;
    case 3:
    case 5:
        if (mbMorfAnimStopped && ls1Guest_cLib_calcTimer(&mTimer2) == 0) {
            mTimer2 = (g_Counter_mCounter0() & 3) + 3;
            switch (m84B) {
            case 3:
                m84B = 5;
                break;
            case 5:
                m84B = 3;
                break;
            }
            setAnm_ATR(FALSE);
        }
        break;
    case 7:
        if (mpMorf->checkFrame(23.0f) != 0) {
            mTelescopeScale = 1.0f;
        }
        break;
    case 12:
    case 14:
    case 15:
    case 16:
        if (mbMorfAnimStopped && ls1Guest_cLib_calcTimer(&mTimer2) == 0) {
            switch (m84B) {
            case 12:
                if (mBckNum != BCKNUM_WAIT07_e) {
                    setAnm_NUM(ANMNUM_WAIT07_e, TRUE);
                    init_texPttrnAnm(BTPNUM_FUAN02_e, true);
                }
                break;
            case 14:
                if (mBckNum != BCKNUM_WAIT06_e) {
                    setAnm_NUM(ANMNUM_WAIT06_e, TRUE);
                    init_texPttrnAnm(BTPNUM_WARAI_e, true);
                }
                break;
            case 15:
                if (mBckNum != BCKNUM_TALK01_e) {
                    setAnm_NUM(BCKNUM_TALK01_e, TRUE);
                    init_texPttrnAnm(BTPNUM_WARAI_e, true);
                }
                break;
            case 16:
                if (mBckNum != BCKNUM_WAIT05_e) {
                    setAnm_NUM(BCKNUM_WAIT05_e, TRUE);
                    init_texPttrnAnm(BTPNUM_WARAI_e, true);
                }
                break;
            }
        }
        break;
    }
}

/* 02281C14 */
void daNpc_Ls1_c::anmAtr(u16 param_1) {
    switch (param_1) {
    case 6: {
        if (m857 == 0) {
            chngAnmAtr(dComIfGp_getMesgAnimeAttrInfo());
            m857 = m857 + 1;
        }
        u8 tag = dComIfGp_getMesgAnimeTagInfo(); /* dComIfGp_getMesgAnimeTagInfo() */
        if (tag != 0xFF && tag != mMesgAnimeTag) {
            dComIfGp_clearMesgAnimeTagInfo();
            mMesgAnimeTag = tag;
            chngAnmTag();
        }
        break;
    }
    case 14:
        m857 = 0;
        break;
    }
    ctrlAnmAtr();
    /* ctrlAnmTag(): empty */
}

/* : the matcher calls it ls1Guest_cLib_getRndValue<int>; it is searchByID */
/* 02280BEC */
fopAc_ac_c* daNpc_Ls1_c::searchByID(fpc_ProcID i_PID, be<s32>* param_2) {
    gabi::Local<gptr<fopAc_ac_c>> actor_p;
    *actor_p = nullptr;
    *param_2 = 0;
    if (!ls1Guest_searchActorByID(i_PID, actor_p.get())) { /* fopAcM_SearchByID(id, &actor) */
        *param_2 = 1;
    }
    return *actor_p;
}

/* 02281CDC */
u16 daNpc_Ls1_c::next_msgStatus(be<u32>* o_msgNoP) {
    u16 msg_status = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    switch (*o_msgNoP) {
    case 0xBB9:
        if (dComIfGs_getClearCount() /* dComIfGs_getClearCount() */) {
            *o_msgNoP = 0xBDA;
            break;
        }
        *o_msgNoP = 0xBBA;
        break;
    case 0xBBA:
    case 0xBDA:
        *o_msgNoP = 0xBBB;
        break;
    case 0xBBB:
        *o_msgNoP = 0xBBC;
        break;
    case 0xBBC:
        *o_msgNoP = 0xBBD;
        break;
    case 0xBBD:
        *o_msgNoP = 0xBDC;
        break;
    case 0xBBE:
        *o_msgNoP = 0xBBF;
        break;
    case 0xBC5:
        *o_msgNoP = 0xBC6;
        break;
    default:
        msg_status = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return msg_status;
}

/* 02281DA4 */
u32 daNpc_Ls1_c::getMsg_LS1_0() {
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0001)) {
        if (m835) {
            // HD: m835 clears only while the scope message status is zero (GameCube: always).
            if (dComIfGp_getScopeMesgStatus() == 0) {
                m835 = false;
            }
            return dComIfGs_checkCollect(0) != 0 ? 0xBC4 : 0xBC5;
        }
        if (dComIfGs_isEventBit(dSv_event_flag_UNK_0280)) {
            return 0xBC9;
        }
        return dComIfGs_checkCollect(0) != 0 ? 0xBC8 : 0xBC7;
    }
    if (m836) {
        m836 = false;
        return 0xBBE;
    }
    if (dComIfGp_getSelectItem(0) == 0x20 /* dItemNo_TELESCOPE_e */) {
        return 0xBCB;
    }
    if (dComIfGp_getSelectItem(1) == 0x20) {
        return 0xBCC;
    }
    if (dComIfGp_getSelectItem(2) == 0x20) {
        return 0xBCD;
    }
    return 0xBC0;
}

/* 02281F64 */
u32 daNpc_Ls1_c::getMsg() {
    u32 result = 0;
    switch (mType) {
    case 0:
        result = getMsg_LS1_0();
        break;
    case 3:
        result = 0xBD9; /* getMsg_LS1_3() */
        break;
    }
    return result;
}

/* 02281FAC */
u8 daNpc_Ls1_c::chk_talk() {
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd()) {
            mItemNo = dComIfGp_event_getPreItemNo();
            return true;
        }
        return false;
    }
    mItemNo = 0xFF;
    return true;
}

/* 02282044 */
u8 daNpc_Ls1_c::chk_parts_notMov() {
    return mJointHeadY != m_jnt.mAngles[0][1] || mJointBackboneY != m_jnt.mAngles[1][1] || mActorAngleY != current.angle.y;
}

/* 02282084 */
u8 daNpc_Ls1_c::chkAttention() {
    dAttention_c* attention = dComIfGp_getAttention();
    if (ls1Guest_dAttention_LockonTruth(attention)) {
        return this == (void*)ls1Guest_dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)ls1Guest_dAttention_ActionTarget(attention, 0);
}

/* 0228210C */
u8 daNpc_Ls1_c::chk_areaIN(f32 param_1, f32 param_2, s16 param_3, cXyz* param_4) {
    gabi::Local<cXyz> diff;
    cXyz_mi(&dComIfGp_getLinkPlayer()->current.pos, diff, param_4);
    gabi::Local<cXyz> xz;
    f32 dx = diff->x, dz = diff->z;
    xz->y = 0.0f;
    xz->x = dx;
    xz->z = dz;
    f32 abs_xz = std_sqrtf(PSVECSquareMag(xz));
    f32 fVar1 = dComIfGp_getLinkPlayer()->current.pos.y - param_4->y;
    s16 sVar = cLib_targetAngleY(param_4, &dComIfGp_getLinkPlayer()->current.pos) - current.angle.y;
    if (abs_xz < param_1 && std::fabs(fVar1) < param_2 && abs((int)sVar) < (int)param_3) {
        return true;
    }
    return false;
}

// : returns a cXyz through the hidden result pointer (r4; HD: a NULL result pointer
// allocates the object)
/* 02282290 */
void daNpc_Ls1_c::get_playerEvnPos(cXyz* o_pos, int param_1) {
    /* const Vec pos[] = {{200, 0, -200}, {100, 0, -80}}  */
    /* the original frame: sp08 at +8, pos[2] at +0x14 (an out-of-range index points elsewhere in it) */
    gabi::Local<Ls1EventFrame> frame;
    cXyz* sp08 = &frame->mResult;
    cXyz* pos = frame->mPositions;
    auto* words = reinterpret_cast<be<u32>*>(pos);
    auto* source = reinterpret_cast<be<u32>*>(&l_eventPositions);
    for (u32 i = 0; i < 6; i++) words[i] = source[i];
    PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), gabi::at<cXyz>(gabi::ea(pos) + param_1 * 12), sp08);
    if (o_pos == nullptr) {
        o_pos = (cXyz*)operator_new(12);
        if (o_pos == nullptr)
            return;
    }
    o_pos->x = sp08->x;
    o_pos->y = sp08->y;
    o_pos->z = sp08->z;
}

/* ls1Guest_mDoLib_project(cXyz* pos, cXyz* out) */

/* (the matcher calls it chkTelescope; it is chkTelescope_sph) */
/* 02282380 */
u8 daNpc_Ls1_c::chkTelescope_sph(cXyz* param_1, f32 param_2, f32 param_3) {
    dComIfGp_get(); // HD: an unused accessor call
    gabi::Local<cXyz> sp18;
    ls1Guest_mDoLib_project(param_1, sp18);
    f32 x = sp18->x, y = sp18->y;
    gabi::Local<cXyz> sp0C;
    sp18->z = y;
    sp0C->x = x - 319.5f;
    sp18->y = 0.0f;
    sp0C->z = y - 186.5f;
    bool chk = false;
    sp0C->y = 0.0f;
    if (ls1Guest_normalize(sp0C.get())) { /* sp0C.normalizeRS() */
        PSVECScale(sp0C, sp0C, param_2);        /* sp0C *= param_2 */
        PSVECAdd(sp18, sp0C, sp18);             /* sp18 += sp0C */
        f32 dz = sp18->z - 186.5f;
        f32 dx = sp18->x - 319.5f;
        chk = param_3 > std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    }
    return chk;
}

static Ls1ScopeState* ls1ScopeState() {
    return gabi::at<Ls1ScopeState>(dComIfGp_ea() + PLAY_OFF(mScopeType));
}
static f32 ls1CameraZoom() {
    return reinterpret_cast<Ls1CameraInfo*>(gabi::at<u8>(dComIfGp_ea() + PLAY_OFF(mCameraInfo)))->mZoomScale;
}
static Ls1Camera* ls1Camera() { return reinterpret_cast<Ls1Camera*>(dComIfGp_getCamera(0)); }

/* chkTelescope */
/* 022824B0 */
u8 daNpc_Ls1_c::chkTelescope(cXyz* param_1, f32 param_2, f32 param_3) {
    /* dComIfGd_getView()->mFovy (view at play+0x5FA4, +0xD4) */
    f32 fov_y = reinterpret_cast<Ls1GameInfo*>(dComIfGp_get())->mpDrawlistView->mFovy / 40.0f;
    if (fov_y > 1.0f) {
        fov_y = 1.0f;
    }
    gabi::Local<cXyz> sp14;
    ls1Guest_mDoLib_project(param_1, sp14);
    f32 x = (sp14->x - 319.5f) * fov_y;
    f32 z = (sp14->y - 186.5f) * fov_y;
    return std::fabs(x) < param_2 && z > param_3;
}

// HD: the scope interface uses the message-manager singleton.
static inline u32 msgMgr() { return (u32)l_messageManager.mValue; }

static inline void fopMsgM_forceSendOn(u32 m) { gabi::at<Ls1MessageManager>(m)->mForceSend = 1; }
/* camera 0 (play+0x5AF8): dCamera_c at +0x248, flags at +0x758 */

/* 022825A0 */
u8 daNpc_Ls1_c::telescope_proc() {
    gabi::Local<be<s32>> param;
    fopAc_ac_c* bm1_npc_p = searchByID(mBm1ProcID, param);
    if (!bm1_npc_p || *param == 1) {
        return false;
    }
    u8 scope_mesg_status = dComIfGp_getScopeMesgStatus(); /* dComIfGp_getScopeMesgStatus() */
    if (scope_mesg_status == 0) {
        if (dComIfGp_getPlayerStatus0() & 0x00200000) { /* daPyStts0_TELESCOPE_LOOK_e */
            if (ls1ScopeState()->mScopeWipeFlag /* mScopeWipeFlag */) {
                auto* link = reinterpret_cast<daPy_py_c*>(dComIfGp_getLinkPlayer());
                link->setPlayerPosAndAngle(&m7CC[1], (s16)-0x3390);
            }
            ls1ScopeState()->mScopeType = 1; /* dComIfGp_setScopeType(1) */
            return true;
        }
        return false;
    }
    u32 msg = msgMgr();
    if (m833 == 0) {
        ls1Guest_bgmStreamPrepare(0xC0000001u); /* mDoAud_bgmStreamPrepare(JA_STRM_DEMO_TETRA_FLY) */
        reinterpret_cast<Ls1Bm1*>(bm1_npc_p)->mTelescopeDemo = 1; /* bm1->setTelescopeDemo() */
        m833 = 1;
        mTimer5 = 150;
        m810 = 0xFFFFFFFF;
        ls1Guest_fopMsgM_getScopeMode(msg);
        return true;
    }
    u8 m833v = m833;
    if (scope_mesg_status == 0x12 /* BOX_CLOSED */) {
        dComIfGp_setScopeMesgStatus(0x11); /* setScopeMesgStatus(BOX_CLOSING) */
        if (m833 == 3 && m810 != 0) {
            Ls1Camera* cam = ls1Camera();
            cam->mCamera.mEventFlags &= ~0x01000000u; /* clrFlag(0x800000) */
            ls1Guest_subjectLockOff(gabi::ea(&ls1Camera()->mCamera));                   /* SubjectLockOff() */
            dComIfGs_onEventBit(dSv_event_flag_UNK_0310);
            mTimer5 = 0;
            m833 = 4;
            ls1Guest_fopMsgM_getScopeMode(msg);
            return true;
        }
        m833v = m833;
    } else if (scope_mesg_status != 0xA /* CLOSE_WAIT */ && scope_mesg_status != 0xD /* SCOPE_WAIT */) {
        ls1Guest_fopMsgM_getScopeMode(msg);
        return true;
    }
    u32 temp_msg_no = 0;
    switch (m833v) {
    case 1: {
        temp_msg_no = m810;
        m810 = 0xBC1;
        gabi::Local<cXyz> att;
        att->x = bm1_npc_p->attention_info.position.x;
        att->y = bm1_npc_p->attention_info.position.y;
        att->z = bm1_npc_p->attention_info.position.z;
        if (chkTelescope_sph(att, 0.0f, l_HIO.mPrm.m2C)) {
            // HD: choose the message from the camera zoom scale and m24.
            m810 = ls1CameraZoom() < l_HIO.mPrm.m24 ? 0xBCA : 0xBCB;
        }
        m833v = m833;
        break;
    }
    case 2:
        m810 = 0xBCA;
        if (!(ls1CameraZoom() < l_HIO.mPrm.m28)) {
            if (scope_mesg_status == 0xA) {
                fopMsgM_forceSendOn(msgMgr());
            }
            Ls1Camera* cam = ls1Camera();
            cam->mCamera.mEventFlags |= 0x01000000u; /* setFlag(0x800000) */
            reinterpret_cast<Ls1Bm1*>(bm1_npc_p)->mFocus = 1;                          /* bm1->setFocus() */
            mTimer5 = 0;
            m810 = 0;
            m833 = 3;
            ls1Guest_fopMsgM_getScopeMode(msg);
            return true;
        }
        m833v = m833;
        break;
    case 3:
        if (m810 == 0 && reinterpret_cast<Ls1Bm1*>(bm1_npc_p)->mOdoroki /* bm1->getOdoroki() */) {
            if (scope_mesg_status == 0xA) {
                fopMsgM_forceSendOn(msg);
            } else if (ls1Guest_fopMsgM_releaseScopeMode(msg)) {
                m810 = 0xBC2;
                ls1Guest_fopMsgM_scopeMessageSet(msg, 0xBC2);
            }
        }
        ls1Guest_fopMsgM_getScopeMode(msg);
        return true;
    case 4: {
        gabi::Local<cXyz> pos;
        f32 y = bm1_npc_p->attention_info.position.y;
        f32 z = bm1_npc_p->attention_info.position.z;
        f32 x = bm1_npc_p->attention_info.position.x;
        pos->z = z;
        pos->y = y + 2000.0f;
        pos->x = x;
        // HD: 90/30 (GameCube: 60/-30)
        if (chkTelescope(pos, 90.0f, 30.0f)) {
            if (scope_mesg_status == 0xA) {
                fopMsgM_forceSendOn(msg);
                m833v = m833;
                break;
            }
            if (ls1Guest_fopMsgM_releaseScopeMode(msg)) {
                m850 = 3;
            }
        }
        m810 = 0xBC3;
        m833v = m833;
        break;
    }
    }
    if ((m833v == 1 && temp_msg_no != m810) || ls1Guest_cLib_calcTimer(&mTimer5) == 0) {
        if (scope_mesg_status == 0xA) {
            fopMsgM_forceSendOn(msg);
            mTimer5 = 0;
        } else if (ls1Guest_fopMsgM_releaseScopeMode(msg)) {
            mTimer5 = 150;
            if (m833 == 1 && m810 == 0xBCB) {
                ls1Guest_subjectLockOn(gabi::ea(&ls1Camera()->mCamera), bm1_npc_p); /* SubjectLockOn(bm1) */
                m810 = 0xBCA;
                mTimer5 = 150;
                m833 = 2;
            }
            if (m810 == 0xBC3) {
                mTimer3 = (s16)ls1Guest_cLib_getRndValue(30, 30);
            }
            ls1Guest_fopMsgM_scopeMessageSet(msg, m810);
        }
    }
    ls1Guest_fopMsgM_getScopeMode(msg);
    return true;
}

/* 02282A60 */
BOOL daNpc_Ls1_c::wait_1() {
    if (m850 < 3) {
        gabi::Local<cXyz> pos;
        f32 x = current.pos.x, z = current.pos.z, y = current.pos.y;
        pos->x = x;
        pos->z = z;
        pos->y = y;
        m850 = 0;
        if (chk_areaIN(l_HIO.mPrm.m29, 100.0f, 0x7FFF, pos)) {
            m850 = 4;
        }
    }
    m853 = 0;
    if (mBckNum == BCKNUM_WAIT01_e) {
        if (ls1Guest_cLib_calcTimer(&mTimer5) == 0) {
            setAnm_NUM(ANMNUM_WAIT02_e, TRUE);
        }
        return true;
    }
    if (mbMorfAnimStopped) {
        mTimer5 = (s16)ls1Guest_cLib_getRndValue(60, 90);
        setAnm_NUM(ANMNUM_WAIT01_e, TRUE);
    }
    return true;
}

/* 02282B54 */
BOOL daNpc_Ls1_c::wait_2() {
    cLib_addCalcAngleS(&current.angle.y, m7A0.y, 4, 0x800, 0x80);
    if (current.angle.y == m7A0.y && mBckNum != BCKNUM_WAIT01_e) {
        setAnm_NUM(ANMNUM_WAIT01_e, TRUE);
    }
    if (m83F) {
        if (chk_talk()) {
            setStt(5);
            setAnm_NUM(ANMNUM_WAIT06_e, TRUE);
            m_jnt.mbBackBoneLock = 0; /* offBackBoneLock() */
            m840 = 0;
            m853 = 1;
            m_jnt.mbTrn = 1;          /* setTrn() */
        }
        return TRUE;
    }
    if (m850 != 1 && m850 < 3) {
        bool telescope_proc_result = false;
        gabi::Local<cXyz> pos;
        f32 x = current.pos.x, y = current.pos.y;
        pos->x = x;
        pos->y = y;
        pos->z = current.pos.z;
        m850 = 0;
        if (chk_areaIN(l_HIO.mPrm.m1C, 100.0f, 0x7FFF, pos) &&
            (dComIfGp_getPlayerStatus0() & 0x00200000) /* daPyStts0_TELESCOPE_LOOK_e */) {
            telescope_proc_result = telescope_proc();
        }
        if (!telescope_proc_result) {
            m850 = 2;
        }
    }
    m_jnt.mbBackBoneLock = 1; /* onBackBoneLock() */
    m840 = 1;
    if (m833 != 0) {
        gabi::Local<cXyz> base_pos;
        base_pos->x = 20.0f;
        base_pos->y = 0.0f;
        base_pos->z = 50.0f;
        if (m833 >= 4) {
            if (mBtpNum != BTPNUM_FUAN02_e) {
                init_texPttrnAnm(BTPNUM_FUAN02_e, true);
            }
            if (ls1Guest_cLib_calcTimer(&mTimer3) != 0) {
                m853 = 1;
                return TRUE;
            }
            base_pos->y = 120.0f;
        }
        PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::YrotM(current.angle.y);
        PSMTXMultVec(mDoMtx_stack_c::get(), base_pos, &mPlayerEyePos);
        m853 = 2;
        m838 = true;
        return TRUE;
    }
    if (mbAttention && mBckNum == BCKNUM_WAIT01_e) {
        m853 = 1;
        return TRUE;
    }
    m853 = 0;
    return TRUE;
}

/* 02282D94 */
BOOL daNpc_Ls1_c::wait_3() {
    gabi::Local<cXyz> local_18;
    local_18->x = 40.0f;
    local_18->y = 140.0f;
    local_18->z = 100.0f;
    PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(m7A0.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), local_18, &mPlayerEyePos);
    cLib_addCalcAngleS(&current.angle.y, cLib_targetAngleY(&current.pos, &mPlayerEyePos), 4, 0x800, 0x80);
    if (m83F) {
        if (chk_talk()) {
            setStt(5);
            m_jnt.mbTrn = 1; /* setTrn() */
            m840 = 0;
            m853 = 1;
        }
        return true;
    }
    if (m850 != 1) {
        m850 = 2;
    }
    m840 = 1;
    m853 = 2;
    m838 = true;
    return true;
}

/* 02282EB0 */
BOOL daNpc_Ls1_c::wait_4() {
    if (m83F) {
        if (chk_talk()) {
            setStt(5);
            m853 = 1;
            m840 = 0;
            m_jnt.mbTrn = 1; /* setTrn() */
        }
        return true;
    }
    m850 = 2;
    m840 = 1;
    if (mbAttention) {
        mTimer4 = (s16)ls1Guest_cLib_getRndValue(15, 30);
    }
    if (ls1Guest_cLib_calcTimer(&mTimer4) != 0) {
        m853 = 1;
        return true;
    }
    m853 = 0;
    return true;
}

/* 02282F6C */
BOOL daNpc_Ls1_c::talk_1() {
    BOOL res = chk_parts_notMov();
    talk(1);
    // HD: the message status comes from the message manager (GameCube: mpCurrMsg->mStatus)
    if (!mbHasMsg) {
        return res;
    }
    if (fopMsgM_getStatus() == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
        switch (mCurrMsgNo) {
        case 0xBC4:
        case 0xBC6:
            eventInfo.setEventId(-1);
            break;
        }
        fpcM_Search(gabi::guest_fn((const void*)searchActor_kamome_Clr_NOSTOP_DEMO), this);
        mItemNo = 0xFF;
        m83F = false;
        setStt(m852);
        mTimer4 = (s16)ls1Guest_cLib_getRndValue(15, 30);
        endEvent();
    }
    return res;
}

/* 02283038 */
BOOL daNpc_Ls1_c::wait_action1(void*) {
    switch ((u32)(s32)m856) {
    case 0: {
        gabi::Local<cXyz> tmp;
        get_playerEvnPos(tmp, 0);
        m7CC[0].copy(*tmp);
        get_playerEvnPos(tmp, 1);
        m7CC[1].copy(*tmp);
        m856 = m856 + 1;
        if (dComIfGs_isEventBit(dSv_event_flag_UNK_2A80)) {
            if (dComIfGs_isEventBit(dSv_event_flag_UNK_0001)) {
                setStt(3);
            } else if (ls1Guest_checkGetItem(0x20) /* dComIfGs_checkGetItem(dItemNo_TELESCOPE_e) */) {
                m7A0.y = current.angle.y;
                setStt(2);
            } else {
                setStt(1);
            }
        } else {
            setStt(4);
        }
        break;
    }
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch ((u32)(s32)m851) {
        case 1:
            mbSetEyePos = wait_1();
            break;
        case 2:
            mbSetEyePos = wait_2();
            break;
        case 3:
            mbSetEyePos = wait_3();
            break;
        case 4:
            mbSetEyePos = wait_4();
            break;
        case 5:
            mbSetEyePos = talk_1();
            break;
        }
        break;
    }
    return TRUE;
}

/* 02283228 */
BOOL daNpc_Ls1_c::demo_action1(void*) {
    if (m856 == 0) {
        setAnm_NUM(mType == 1 ? ANMNUM_DEMOWAIT_e : ANMNUM_WAIT06_e, TRUE);
        m856 = m856 + 1;
    }
    return TRUE;
}

/* 0227FFEC */
void daNpc_Ls1_c::checkOrder() {
    u16 command = eventInfo.getCommand(); /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mEventIDTbl[mEventIndex]) && m850 >= 3) {
            switch ((u32)(s32)mEventIndex) {
            case 0:
                ls1Guest_cameraStop(gabi::ea(&ls1Camera()->mCamera)); /* dComIfGp_getCamera(0)->mCamera.Stop() */
                ls1Guest_bgmStreamPlay();                                /* mDoAud_bgmStreamPlay() */
                m834 = true;
                break;
            case 1:
                cLib_targetAngleY(&m7CC[0], &current.pos);
                m83B = true;
                break;
            case 2:
                mTelescopeScale = 0.0f;
                setAnm_NUM(ANMNUM_GET_e, TRUE);
                break;
            }
            m850 = 0;
            mMesgAnimeTag = 0xFF;
            m84B = 0xFF;
        }
    } else if (command == 1 /* checkCommandTalk() */) {
        if (m850 == 1 || m850 == 2) {
            m850 = 0;
            m83F = true;
            // HD: the camera is restarted here
            ls1Guest_cameraStart(gabi::ea(&ls1Camera()->mCamera)); /* dComIfGp_getCamera(0)->mCamera.Start() */
        }
    }
}

/* dNpc_JntCtrl_c::lookAtTarget_2(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16, s16, bool) */

/* 02280C40 */
void daNpc_Ls1_c::lookBack() {
    mJointHeadY = m_jnt.mAngles[0][1];
    gabi::Local<cXyz> player_eye_pos;
    player_eye_pos->set(0.0f, 0.0f, 0.0f);
    s16 target_y = current.angle.y;
    mActorAngleY = target_y;
    f32 srcY = eyePos.y;
    u8 temp_m840 = m840;
    f32 srcZ = current.pos.z;
    f32 srcX = current.pos.x;
    mJointBackboneY = m_jnt.mAngles[1][1];
    cXyz* player_eye_pos_p = nullptr;

    switch ((u32)(s32)m853) {
    case 1: {
        gabi::Local<cXyz> eye;
        ls1Guest_playerEyePosition(eye, l_HIO.mPrm.mPlayerEyePosOffsetY);
        mPlayerEyePos.copy(*eye);
        player_eye_pos->copy(*eye);
        player_eye_pos_p = player_eye_pos;
        m838 = true;
        break;
    }
    case 2:
        player_eye_pos->copy(mPlayerEyePos);
        player_eye_pos_p = player_eye_pos;
        break;
    case 3:
        target_y = m82E;
        m838 = false;
        break;
    case 4: {
        gabi::Local<be<s32>> param;
        fopAc_ac_c* actor_p = searchByID(m790, param);
        if (actor_p && *param == 0) {
            mPlayerEyePos.copy(actor_p->current.pos);
            mPlayerEyePos.y = actor_p->eyePos.y;
            player_eye_pos->x = mPlayerEyePos.x;
            player_eye_pos->y = mPlayerEyePos.y;
            player_eye_pos->z = mPlayerEyePos.z;
            player_eye_pos_p = player_eye_pos;
            m838 = true;
        }
        break;
    }
    default:
        m838 = false;
        break;
    }
    gabi::Local<cXyz> current_pos; /* passed by value: a copy */
    current_pos->x = srcX;
    current_pos->y = srcY;
    current_pos->z = srcZ;
    ls1Guest_dNpc_JntCtrl_lookAtTarget_2(&m_jnt, &current.angle.y, player_eye_pos_p, current_pos, target_y, l_HIO.mPrm.m12, temp_m840);
    s16 halfX = (s16)(m_jnt.mAngles[0][0] / 2);
    mHalfHeadAngleX = halfX;
    s16 halfY = (s16)(m_jnt.mAngles[0][1] / 2);
    m848 = halfX;
    mHalfHeadAngleY = halfY;
    m846 = -halfY;
}

/* 022801BC */
u8 daNpc_Ls1_c::demo() {
    u8 id = demoActorID;
    if (id == 0) {
        if (m841) {
            if (mType == 3) {
                mBckNum = BCKNUM_NULL_e;
                setAnm_NUM(ANMNUM_WAIT06_e, TRUE);
            }
            m841 = false;
        }
        return m841;
    }
    if (!m841) {
        for (int i = 0; i < 2; i++) {
            if (mpMatAnms[i].get()) {
                mpMatAnms[i]->mbMove = 0; /* clrMoveFlag() */
            }
        }
        m_jnt.mAngles[0][1] = 0; /* setHead_y(0) */
        m_jnt.mAngles[1][0] = 0; /* setBackBone_x(0) */
        m841 = true;
        m83B = false;
        m_jnt.mAngles[0][0] = 0; /* setHead_x(0) */
        id = demoActorID;
        m_jnt.mAngles[1][1] = 0; /* setBackBone_y(0) */
    }
    /* dComIfGp_demo_getActor(demoActorID) (HD inline with a range check) */
    void* demo_actor_p = nullptr;
    if (id != 0 && id <= 0x20) {
        u32 obj = (u32)l_demoObject.mValue;
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
            obj = (u32)l_demoObject.mValue;
        }
        demo_actor_p = ls1Guest_demoActor(obj, id);
    }
    void* btp_anm = mBtpAnm.mpAnm.get();
    if (btp_anm) {
        u8 frame_max = (u8)J3DAnm_getFrameMax(btp_anm);
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame < frame_max ? frame : frame_max;
    }
    if (demo_actor_p) {
        void* demo_btp_p = ls1Guest_demoBtpData(demo_actor_p, mArcName); /* getP_BtpData */
        if (demo_btp_p) {
            ls1Guest_mDoExt_btpAnm_init(&mBtpAnm, getModelData(mpMorf->getModel()), demo_btp_p, TRUE, 0, 1.0f, 0, -1, 1, 0);
            mBtpFrame = 0;
            mBtpNum = BTPNUM_NULL_e;
        }
    }
    void* btk_anm = reinterpret_cast<Ls1BtkWork*>(&mBtkAnm)->mpAnm.get();
    if (btk_anm) {
        u8 frame_max = (u8)J3DAnm_getFrameMax(btk_anm);
        u8 frame = (u8)(mBtkFrame + 1);
        mBtkFrame = frame < frame_max ? frame : frame_max;
    }
    if (demo_actor_p) {
        void* demo_btk_p = ls1Guest_demoBtkData(demo_actor_p, mArcName); /* getP_BtkData */
        if (demo_btk_p) {
            ls1Guest_mDoExt_btkAnm_init(&mBtkAnm, getModelData(mpMorf->getModel()), demo_btk_p, TRUE, 0, 1.0f, 0, -1, 1, 0);
            mBtkFrame = 0;
            mBtkNum = 3; /* BTKNUM_NULL_e */
        }
        auto* demoActor = reinterpret_cast<Ls1DemoActor*>(demo_actor_p);
        if (demoActor->mFlags & 0x10) { /* checkEnable(ENABLE_SHAPE_e) */
            mTelescopeScale = demoActor->mShapeId == 1 ? 1.0f : 0.0f; /* getShapeId() */
        }
        // HD: for type 3 the demo actor's flag 0x40 is cleared
        if (mType == 3) {
            demoActor->mFlags &= 0xFFBF;
        }
    }
    gabi::Local<cXyz> pos; /* passed by value */
    f32 z = current.pos.z, y = current.pos.y;
    pos->z = z;
    pos->y = y;
    pos->x = current.pos.x;
    u32 snd = ls1Guest_groundMaterialSound(pos.get(), 10.0f); /* dBgS_GetGndMtrlSndId_Func(pos, 10.0f) */
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    ls1Guest_setDemoData(this, 0x6A, mpMorf.get(), mArcName, 0, 0, snd, reverb); /* dDemo_setDemoData */
    return m841;
}

/* 02281074 */
BOOL daNpc_Ls1_c::_execute() {
    if (!m83D) {
        m7A0.y = current.angle.y;
        m794.copy(current.pos);
        m7A0.x = current.angle.x;
        m83D = true;
        m7A0.z = current.angle.z;
    }
    daNpc_Ls1_HIO_c::hio_prm_c& prm = l_HIO.mPrm;
    m_jnt.setParam(prm.mMaxBackBoneX, prm.mMaxBackBoneY, prm.mMinBackBoneX, prm.mMinBackBoneY, prm.mMaxHeadX,
                   prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, prm.mMaxTurnStep);
    if (m83A && demoActorID == 0) {
        return TRUE;
    }
    partner_search();
    checkOrder();
    if (!demo()) {
        s32 staff_id = -1;
        if (ls1EventModeByte() && eventInfo.getCommand() != 1 /* !eventInfo.checkCommandTalk() */) {
            staff_id = isEventEntry();
        }
        if (staff_id >= 0 || m834) {
            event_proc(staff_id);
        } else {
            pmf_call(this, &mCurrProcFunc, nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        lookBack();
        if (mType != 0) {
            fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
            mObjAcch.CrrPos(dComIfG_Bgsp());
        }
        play_animation();
    } else {
        m83A = false;
    }
    eventOrder();
    mAngle.x = current.angle.x;
    mAngle.y = current.angle.y;
    mAngle.z = current.angle.z;
    if (!m83B) {
        shape_angle.x = current.angle.x;
        shape_angle.y = current.angle.y;
        shape_angle.z = current.angle.z;
    }
    tevStr.mRoomNo = ls1Guest_dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    tevStr.mEnvrIdxOverride = ls1Guest_dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this));
    setMtx(false);
    if (!m841) {
        setCollision(40.0f, 100.0f);
    }
    return TRUE;
}

/* static initialisation of a 4-byte function-local constant on first use */
static inline void local_static_init(Ls1Word* guard, Ls1Word* dst, Ls1Word* src) {
    if (guard->mValue == 0) {
        guard->mValue = 1;
        memcpy_g(dst, src, 4);
    }
}

/* 02281318 */
BOOL daNpc_Ls1_c::_draw() {
    J3DModel* morf_model_p = mpMorf->getModel();
    J3DModel* model_p = mpLsHandModel;
    J3DModelData* morf_model_info_p = getModelData(morf_model_p);
    if (m83A || m83C != 0) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model_p, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model_p, &tevStr);
    dComIfGp_get(); // HD: an unused accessor call
    mDoExt_btpAnm_entry(&mBtpAnm, morf_model_info_p, mBtpFrame);
    mDoExt_btkAnm_entry(&mBtkAnm, morf_model_info_p, (f32)(u32)mBtkFrame);
    mpMorf->entryDL();
    reinterpret_cast<Ls1ModelDataTail*>(morf_model_info_p)->mBtkAnimation = 0;
    reinterpret_cast<Ls1ModelDataTail*>(morf_model_info_p)->mBtpAnimation = 0;
    mDoExt_modelEntryDL(model_p);
    if (mpTelescopeModel.get()) {
        setLightTevColorType(dKy_getEnvlight(), mpTelescopeModel, &tevStr);
        mDoExt_modelEntryDL(mpTelescopeModel);
    }
    // HD: no shadowDraw()
    dSnap_RegistFig(0x49 /* DSNAP_TYPE_NPC_LS1 */, this, 1.0f, 1.0f, 1.0f);
    /* debug leftovers: function-local static colors initialised on first use */
    if (l_HIO.mPrm.m18 /* l_HIO.mPrm.m18 */) {
        local_static_init(&l_debugColorGuard0, &l_debugColor0, &l_debugColorSource0);
        local_static_init(&l_debugColorGuard1, &l_debugColor1, &l_debugColorSource1);
        local_static_init(&l_debugColorGuard2, &l_debugColor2, &l_debugColorSource2);
        dComIfGp_get();
        local_static_init(&l_debugColorGuard2, &l_debugColor2, &l_debugColorSource2);
        local_static_init(&l_debugColorGuard3, &l_debugColor3, &l_debugColorSource3);
    }
    return TRUE;
}

/* PowerPC 32-bit shifts (shift amount: low 6 bits, >= 32 gives 0) */
static inline u32 ppc_slw(u32 x, u32 n) { n &= 0x3F; return n & 0x20 ? 0 : x << n; }
static inline u32 ppc_srw(u32 x, u32 n) { n &= 0x3F; return n & 0x20 ? 0 : x >> n; }
/* modelData->getJointNodePointer(jnt)->setCallBack(cb) (HD inline: joints are 0x1C-byte records) */
static inline void setJointCallBack(J3DModel* model, s8 jnt, u32 cb) {
    J3DModelData* md = getModelData(model);
    md->getJointNodePointer((u16)(s16)jnt)->mCallBack = cb;
}

/* 0227E3BC */
BOOL daNpc_Ls1_c::bodyCreateHeap() {
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, dRes_ID_LS_BDL_LS_e);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(3132, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_ls1.cpp", 0xC3C, "a_mdl_dat != (0)");
    // HD: only the two eye materials get a
// daNpc_Ls1_matAnm_c (GameCube: every material)
    for (int i = 0; i < 2; i++) {
        Ls1Material* material = eyeMaterial(a_mdl_dat, i);
        daNpc_Ls1_matAnm_c* anm = (daNpc_Ls1_matAnm_c*)operator_new(0x80);
        if (anm != nullptr)
            anm = ls1Guest_constructMaterialAnimation(anm); /* new daNpc_Ls1_matAnm_c() */
        material->mMaterialAnimation = anm;
    }
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, nullptr, -1 /* EMode_NULL */, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020222);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() == nullptr) {
        mDoExt_McaMorf* morf = mpMorf;
        auto* object = reinterpret_cast<Ls1DeletingObject*>(morf);
        gabi::call_ptr(object->mVtable->mDestructor.get(), morf, 3); /* delete (virtual deleting destructor) */
        mpMorf = nullptr;
        return FALSE;
    }
    if (mType == 4 || dComIfGs_getClearCount() /* dComIfGs_getClearCount() */ != 0) {
        u32 tex_info_p = gabi::ea(dComIfG_getObjectIDRes(mArcName, dRes_ID_LS_BTI_LSBODY02_e));
        auto* a_texture = gabi::at<Ls1Texture>(reinterpret_cast<Ls1ModelDataTail*>(a_mdl_dat)->mTexture);
        if (a_texture == 0) /* JUT_ASSERT(3176, a_texture != NULL) */
            JUT_ASSERT_fail("d_a_npc_ls1.cpp", 0xC68, "a_texture != (0)");
        u32 a_textureName = reinterpret_cast<Ls1ModelDataTail*>(a_mdl_dat)->mTextureNames;
        if (a_textureName == 0) /* JUT_ASSERT(3178, a_textureName != NULL) */
            JUT_ASSERT_fail("d_a_npc_ls1.cpp", 0xC6A, "a_textureName != (0)");
        for (u16 i = 0; i < a_texture->mCount; i++) {
            u32 name = ls1Guest_textureName(a_textureName, i); /* JUTNameTab::getName */
            if (name == 0) // HD: JUT_ASSERT(3182, a_nme != NULL)
                JUT_ASSERT_fail("d_a_npc_ls1.cpp", 0xC6E, "a_nme != (0)");
            /* strcmp(name, "lsbody01") */
            auto* a = gabi::at<be<u8>>(name);
            auto* b = l_bodyTextureName.mName;
            u8 c1, c2;
            do {
                c1 = *a++;
                c2 = *b++;
            } while (c1 == c2 && c1 != 0);
            if (c1 != c2)
                continue;
            /* a_texture->setResTIMG(i, *tex_info_p) (HD inline) */
            auto* entry = &a_texture->mImages[i];
            auto* words = reinterpret_cast<be<u32>*>(entry);
            auto* source = gabi::at<be<u32>>(tex_info_p);
            for (u32 k = 0; k < 9; k++) words[k] = source[k];
            entry = &a_texture->mImages[i];
            entry->mImageOffset = entry->mImageOffset + tex_info_p - gabi::ea(entry);
            entry = &a_texture->mImages[i];
            entry->mPaletteOffset = entry->mPaletteOffset + tex_info_p - gabi::ea(entry);
            entry = &a_texture->mImages[i];
            entry->mHdWord = gabi::at<Ls1TextureImage>(tex_info_p)->mHdWord;
            /* mark texture i as changed: a 128-bit mask at +8 gets (base << i), base at +0x18 */
            u32 hi = a_texture->mBaseMaskHigh;
            u32 lo = a_texture->mBaseMaskLow;
            if (i < 0x40) {
                u32 n = i;
                u32 nh = ppc_slw(lo, n + 0x20) | (ppc_slw(hi, n) | ppc_srw(lo, 0x20 - n));
                a_texture->mChangedMask[0] |= nh;
                a_texture->mChangedMask[1] |= ppc_slw(lo, n);
            } else {
                u32 n = i - 0x40;
                a_texture->mChangedMask[3] |= ppc_slw(lo, n);
                u32 nh = ppc_slw(lo, n + 0x20) | (ppc_slw(hi, n) | ppc_srw(lo, 0x20 - n));
                a_texture->mChangedMask[2] |= nh;
            }
        }
    }
    if (!init_texPttrnAnm(BTPNUM_FUAN_e, false)) {
        mpMorf = nullptr;
        return FALSE;
    }
    m_hed_jnt_num = ls1Guest_JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "head" /* "head" */);
    if (m_hed_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_ls1.cpp", 0xC7D, "m_hed_jnt_num >= 0");
    m_bbone_jnt_num = ls1Guest_JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "backbone" /* "backbone" */);
    if (m_bbone_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_ls1.cpp", 0xC7F, "m_bbone_jnt_num >= 0");
    m_hnd_L_jnt_num = ls1Guest_JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "handL" /* "handL" */);
    if (m_hnd_L_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_ls1.cpp", 0xC81, "m_hnd_L_jnt_num >= 0");
    m_hnd_R_jnt_num = ls1Guest_JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "handR" /* "handR" */);
    if (m_hnd_R_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_ls1.cpp", 0xC83, "m_hnd_R_jnt_num >= 0");
    setJointCallBack(mpMorf->getModel(), m_hed_jnt_num, gabi::guest_fn((const void*)nodeCB_Head));
    setJointCallBack(mpMorf->getModel(), m_bbone_jnt_num, gabi::guest_fn((const void*)nodeCB_BackBone));
    setJointCallBack(mpMorf->getModel(), m_hnd_L_jnt_num, gabi::guest_fn((const void*)nodeCB_Hand_L));
    setJointCallBack(mpMorf->getModel(), m_hnd_R_jnt_num, gabi::guest_fn((const void*)nodeCB_Hand_R));
    mpMorf->getModel()->mUserArea = gabi::ea(this);
    return TRUE;
}

/* 0227EA10 */
BOOL daNpc_Ls1_c::handCreateHeap() {
    mpLsHandModel = nullptr;
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, dRes_ID_LS_BDL_LSHAND_e);
    if (a_mdl_dat == nullptr)
        JUT_ASSERT_fail("d_a_npc_ls1.cpp", 0xCAC, "a_mdl_dat != (0)");
    mpLsHandModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000022);
    if (!mpLsHandModel.get()) {
        return FALSE;
    }
    m_lsHnd_L_jnt_num = ls1Guest_JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "ls_handL" /* "ls_handL" */);
    if (m_lsHnd_L_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_ls1.cpp", 0xCB5, "m_lsHnd_L_jnt_num >= 0");
    m_lsHnd_R_jnt_num = ls1Guest_JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), "ls_handR" /* "ls_handR" */);
    if (m_lsHnd_R_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_ls1.cpp", 0xCB7, "m_lsHnd_R_jnt_num >= 0");
    setJointCallBack(mpLsHandModel, m_lsHnd_L_jnt_num, gabi::guest_fn((const void*)Ls_hand_nodeCB_Hand_L));
    setJointCallBack(mpLsHandModel, m_lsHnd_R_jnt_num, gabi::guest_fn((const void*)Ls_hand_nodeCB_Hand_R));
    mpLsHandModel->mUserArea = gabi::ea(this);
    return TRUE;
}

/* 0227EBF4 */
BOOL daNpc_Ls1_c::itemCreateHeap() {
    mpTelescopeModel = nullptr;
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectRes("Link" /* "Link" */, dRes_INDEX_LINK_BDL_TELESCOPE_e, gc_tu.safeString);
    if (a_mdl_dat == nullptr)
        JUT_ASSERT_fail("d_a_npc_ls1.cpp", 0xCD7, "a_mdl_dat != (0)");
    mpTelescopeModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000022);
    return mpTelescopeModel.get() != nullptr;
}

/* 0227EC98 */
BOOL daNpc_Ls1_c::CreateHeap() {
    if (!bodyCreateHeap()) {
        return FALSE;
    }
    if (!handCreateHeap() || !itemCreateHeap()) {
        mpMorf = nullptr;
        return FALSE;
    }
    mAcchCir.SetWall(30.0f, 40.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}

/* 0227FBF0 */
cPhs_State daNpc_Ls1_c::_create() {
    /* fopAcM_ct(this, daNpc_Ls1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            ls1Guest_constructNpc(this);           /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = ls1_tu.mActorVtable;
            ls1Guest_constructBtk(&mBtkAnm);        /* mDoExt_btkAnm::mDoExt_btkAnm (the matcher calls it init) */
            ls1Guest_constructBtp(&mBtpAnm);        /* mDoExt_btpAnm::mDoExt_btpAnm */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (!decideType(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, mArcName);
    mbResLoadIsComplete = state == cPhs_COMPLEATE_e;
    if (!mbResLoadIsComplete) {
        return state;
    }
    /* a_siz_tbl  */
    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)CheckCreateHeap), l_heapSizes.mSizes[m854])) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -50.0f, -20.0f, -50.0f, 50.0f, 140.0f, 50.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}

// daNpc_Ls1_HIO_c::daNpc_Ls1_HIO_c (HD: allocates when this == NULL)
/* 02283290 */
static daNpc_Ls1_HIO_c* daNpc_Ls1_HIO_c_ct(daNpc_Ls1_HIO_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Ls1_HIO_c*)operator_new(0x44);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = ls1_tu.mHioVtable;
    memcpy_g(&i_this->mPrm, &l_hioParameters, 0x38); /* a_prm_tbl; 028FEAC0 memcpy */
    i_this->m04 = -1;
    i_this->m08 = -1;
    return i_this;
}

// HD: compiler-generated and template code of this translation unit.
/* 022832FC */
static void __sinit_d_a_npc_ls1_cpp() {
    /* header statics (as sinit_header_statics, but the zeroed object is at the TU data) */
    sinit_header_statics_z(gc_tu.headerStatics[0], gc_tu.headerStatics[1], gc_tu.headerStatics[2]);
    // HD: two static sead::SafeStrings naming the eye materials (setMat, bodyCreateHeap)
    l_eyeMaterialNames.mNames[0].__vtbl = gc_tu.safeString;
    l_eyeMaterialNames.mNames[0].mStringTop = 0x1001D438;
    l_eyeMaterialNames.mNames[1].__vtbl = gc_tu.safeString;
    l_eyeMaterialNames.mNames[1].mStringTop = 0x1001D440;
    daNpc_Ls1_HIO_c_ct(&l_HIO); /* static daNpc_Ls1_HIO_c l_HIO */
}

/* : sead::SafeString deleting destructor (this TU's copy; vtable the TU data slot +0xC) */
/* 022833CC */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* : daNpc_Ls1_c deleting destructor (compiler-generated, HD virtual destructor) */
/* 022833E0 */
static void daNpc_Ls1_c_dt(daNpc_Ls1_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        ls1Guest_destroyCircle(&i_this->mAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        i_this->mObjAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcchDt[0];
        i_this->mObjAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcchDt[1];
        ls1Guest_destroyAcch(&i_this->mObjAcch, 0);
        ls1Guest_destroyActor(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* : sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
/* 0228347C */
static void SafeString_assureTerminationImpl(SafeString*) {
}

/* WWHD glue
ext 027ED1F0 u32 ls1Guest_textureName(u32, u32)
ext 027DF9B0 s32 ls1Guest_materialIndex(u32, const char*)
ext 025F795C u32 ls1Guest_messageStatus(u32)
ext 025E7C6C void ls1Guest_constructBtk(mDoExt_btkAnm*)
ext 025E7820 void ls1Guest_constructBtp(mDoExt_btpAnm*)
ext 025E1944 void ls1Guest_bgmStreamPlay()
ext 025E1934 void ls1Guest_bgmStreamPrepare(u32)
ext 025E1904 void ls1Guest_bgmStop(s32)
ext 025E18EC void ls1Guest_bgmStart(u32)
ext 025D54C4 BOOL ls1Guest_searchActorByID(fpc_ProcID, gptr<fopAc_ac_c>*)
ext 025D50BC void ls1Guest_destroyActor(daNpc_Ls1_c*, s32)
ext 025A1458 void ls1Guest_constructNpc(daNpc_Ls1_c*)
ext 02542EDC s8 ls1Guest_eventActionIndex(void*, s32, u32, s32, BOOL, s32)
ext 025279C8 void* ls1Guest_demoBtkData(void*, const char*)
ext 02527828 void* ls1Guest_demoBtpData(void*, const char*)
ext 02527028 void ls1Guest_setDemoData(daNpc_Ls1_c*, u32, mDoExt_McaMorf*, const char*, s32, u32, u32, s32)
ext 02526E70 void* ls1Guest_demoActor(u32, u32)
ext 02520C0C BOOL ls1Guest_checkGetItem(u32)
ext 0251544C void ls1Guest_subjectLockOff(u32)
ext 02515434 void ls1Guest_subjectLockOn(u32, fopAc_ac_c*)
ext 02514F38 void ls1Guest_cameraStart(u32)
ext 02514F2C void ls1Guest_cameraStop(u32)
ext 024F1914 u32 ls1Guest_groundMaterialSound(cXyz*, f32)
ext 024EFD9C void ls1Guest_destroyAcch(dBgS_ObjAcch*, s32)
ext 0227F1C0 bool ls1Guest_initType4(daNpc_Ls1_c*)
ext 0227F1BC bool ls1Guest_initType3(daNpc_Ls1_c*)
ext 0227F1B8 bool ls1Guest_initType2(daNpc_Ls1_c*)
ext 0227F178 u32 ls1Guest_initDemoRegister(daNpc_Ls1_c*)
ext 0227F120 u32 ls1Guest_initWaitRegister(daNpc_Ls1_c*)
ext 0227DFF0 daNpc_Ls1_matAnm_c* ls1Guest_constructMaterialAnimation(daNpc_Ls1_matAnm_c*)
ext 0201B47C BOOL ls1Guest_normalize(cXyz*)
ext 02018034 void ls1Guest_destroyCircle(void*, s32)
ext 0227DED4 void ls1Guest_node_rightHandModel(daNpc_Ls1_c*, J3DNode*, J3DModel*)
ext 0227DDB8 void ls1Guest_node_leftHandModel(daNpc_Ls1_c*, J3DNode*, J3DModel*)
ext 0227DC60 void ls1Guest_node_rightHand(daNpc_Ls1_c*, J3DNode*, J3DModel*)
ext 0227DB08 void ls1Guest_node_leftHand(daNpc_Ls1_c*, J3DNode*, J3DModel*)
ext 0227D99C void ls1Guest_node_backbone(daNpc_Ls1_c*, J3DNode*, J3DModel*)
ext 0227D7DC void ls1Guest_node_head(daNpc_Ls1_c*, J3DNode*, J3DModel*)
data l_HIO 1046799C
data l_checkWork 10467960
data l_checkInfo 104679E0
data l_eyePosOffset 10467990
data l_eyePosGuard 10467A30
data l_currentModel 104B462C
data l_currentMatrix 104B4868
data l_btpResources 1001D158
data l_btkResources 1001D188
data l_bckResources 1001D374
data l_btkMapping 1001D194
data l_waitAction 1001D098
data l_demoAction 1001D0A0
data l_eyeMaterialNames 10467980
data l_eventNames 101BFF4C
data l_cylinderSource 101EA190
data l_animationParameters 101BFF84
data l_stateAnimations 101C0064
data l_attributeAnimations 101C00D4
data l_messageBtpMapping 1001D3F8
data l_counter 101FF558
data l_messageManager 101F4B5C
data l_eventPositions 1001D400
data l_heapSizes 101BFF7C
data l_hioParameters 101C01E4
data l_cutNames 101C00C4
ext 02055B64 s16 ls1Guest_cLib_calcTimer(be<s16>*)
ext 021E1E78 s32 ls1Guest_cLib_getRndValue(s32, s32)
ext 025E789C s32 ls1Guest_mDoExt_btpAnm_init(void*, J3DModelData*, void*, s32, s32, f32, s32, s32, u32, s32)
ext 025E7CE0 s32 ls1Guest_mDoExt_btkAnm_init(void*, J3DModelData*, void*, s32, s32, f32, s32, s32, u32, s32)
ext 024EDFCC bool ls1Guest_dAttention_LockonTruth(dAttention_c*)
ext 024EC8D0 fopAc_ac_c* ls1Guest_dAttention_LockonTarget(dAttention_c*, s32)
ext 024EE464 fopAc_ac_c* ls1Guest_dAttention_ActionTarget(dAttention_c*, s32)
ext 024EF130 s8 ls1Guest_dBgS_GetRoomId(dBgS*, void*)
ext 024EEEB8 u8 ls1Guest_dBgS_GetPolyColor(dBgS*, void*)
ext 024EECAC u32 ls1Guest_dBgS_GetMtrlSndId(dBgS*, void*)
ext 0259D54C void ls1Guest_playerEyePosition(cXyz*, f32)
ext 027DF9B0 s8 ls1Guest_JUTNameTab_getIndex(u32, const char*)
ext 0200ECD4 f32 ls1Guest_cLib_addCalc(be<f32>*, f32, f32, f32, f32)
ext 025F19F8 void ls1Guest_mDoMtx_XYZrotM(Mtx34*, s16, s16, s16)
ext 025F0C48 void ls1Guest_mDoLib_project(cXyz*, cXyz*)
ext 025F8B5C void ls1Guest_fopMsgM_getScopeMode(u32)
ext 025F8BEC BOOL ls1Guest_fopMsgM_releaseScopeMode(u32)
ext 025F7E68 void ls1Guest_fopMsgM_scopeMessageSet(u32, u32)
ext 0259E2D4 void ls1Guest_dNpc_JntCtrl_lookAtTarget_2(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8)

str 1001D360 "Ls1" in 0227F9A8
str 1001D3B0 "prm_0" in 02280720
str 1001D3B8 "AnmNo" in 02280814
str 1001D3F0 "texmtx1" in 02281564
str 1001D140 "d_demo.h" in 022801BC
str 1001D114 "m_object != (0)" in 022801BC
str 1001D1B8 "d_a_npc_ls1.cpp" in 0227E3BC
str 1001D1C8 "a_mdl_dat != (0)" in 0227E3BC
str 1001D1DC "a_texture != (0)" in 0227E3BC
str 1001D1F0 "a_textureName != (0)" in 0227E3BC
str 1001D208 "a_nme != (0)" in 0227E3BC
str 1001D1B0 "head" in 0227E3BC
str 1001D224 "m_hed_jnt_num >= 0" in 0227E3BC
str 1001D238 "backbone" in 0227E3BC
str 1001D244 "m_bbone_jnt_num >= 0" in 0227E3BC
str 1001D1A0 "handL" in 0227E3BC
str 1001D25C "m_hnd_L_jnt_num >= 0" in 0227E3BC
str 1001D1A8 "handR" in 0227E3BC
str 1001D274 "m_hnd_R_jnt_num >= 0" in 0227E3BC
str 1001D28C "d_a_npc_ls1.cpp" in 0227EA10
str 1001D29C "a_mdl_dat != (0)" in 0227EA10
str 1001D2B0 "ls_handL" in 0227EA10
str 1001D2BC "m_lsHnd_L_jnt_num >= 0" in 0227EA10
str 1001D2D4 "ls_handR" in 0227EA10
str 1001D2E0 "m_lsHnd_R_jnt_num >= 0" in 0227EA10
str 1001D2F8 "Link" in 0227EBF4
str 1001D300 "d_a_npc_ls1.cpp" in 0227EBF4
str 1001D310 "a_mdl_dat != (0)" in 0227EBF4
data l_demoObject 101D5FFC
data l_debugColorGuard0 101FDA50
data l_debugColorGuard1 101FDAC0
data l_debugColorGuard2 101FDA44
data l_debugColorGuard3 101FDA48
data l_debugColor0 101FEBF4
data l_debugColor1 101FEBF8
data l_debugColor2 101FEBE8
data l_debugColor3 101FEBEC
data l_debugColorSource0 1001D0B0
data l_debugColorSource1 1001D0B4
data l_debugColorSource2 1001D0B8
data l_debugColorSource3 1001D0BC
data l_bodyTextureName 1001D218
*/
