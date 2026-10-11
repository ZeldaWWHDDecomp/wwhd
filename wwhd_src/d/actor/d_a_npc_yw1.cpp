/**
 * d_a_npc_yw1.cpp (WWHD)
 * NPC - Sue-Belle (Outset; carries a pot of water, watches the Rito partner)
 *
 * The GameCube decompilation of this TU is
 * "Nonmatching" stubs (zeldaret/tww src/d/actor/d_a_npc_yw1.cpp): every function here is written
 * from the WWHD code (cking.rpx) and verified against it, keeping the GameCube names. The hair
 * physics (setHairAngle) is in d_a_npc_yw1_hair.cpp.
 */
#include "d/actor/d_a_npc_yw1.h"

enum Yw1State : s8 {
    Yw1Wait = 1,
    Yw1Talk = 2,
    Yw1Walk = 3,
    Yw1WaitPath = 4,
    Yw1Turn = 5,
    Yw1AwaitRescue = 6
};
enum Yw1Process : s16 { Yw1RitoProcess = 0x146, Yw1PotProcess = 0x1C5 };

const gc_tu_data gc_tu = {.safeString = 0x100236F0, .objAcchDt = {0x10023708, 0x10023718}};
static BOOL nodeCB_Hair(J3DNode*, int);
static BOOL nodeCB_Head(J3DNode*, int);
static BOOL nodeCB_BackBone(J3DNode*, int);
static BOOL CheckCreateHeap(fopAc_ac_c*);
static void* searchActor_Bm1(void*, void*);
#define YW1_VTBL 0x10023920
#define HIO_VTBL 0x10023738
#define CHILDHIO_VTBL 0x10023728
static auto& l_unusedEyeGuard = gabi::object_at<be<u32>>(0x10468AB8);
static auto& l_eyeGuard = gabi::object_at<be<u32>>(0x10468ABC);
static auto& l_unusedEyeOffset = gabi::object_at<cXyz>(0x10468A00);
static auto& a_eye_pos_offst = gabi::object_at<cXyz>(0x10468A0C);
static auto& l_demoObject = gabi::object_at<be<u32>>(0x101D5FFC);
static auto& l_bm1Count = gabi::object_at<be<s32>>(0x104689E0);
static auto& l_bm1Actors = gabi::object_at<gptr<fopAc_ac_c>[20]>(0x10468A68);
static auto& l_HIO = gabi::object_at<daNpc_Yw1_HIO_c>(0x10468A24);
static auto& l_liftUp = gabi::object_at<cXyz>(0x10468A18);
static auto& l_headerZero = gabi::object_at<be<u32>[4]>(0x104689F0);
static auto& l_headerMax = gabi::object_at<be<f32>>(0x104689E8);
static auto& l_headerMin = gabi::object_at<be<f32>>(0x104689E4);
static auto& l_headerByte0 = gabi::object_at<be<u8>>(0x104689EC);
static auto& l_headerByte1 = gabi::object_at<be<u8>>(0x104689ED);
static auto& l_debugGuard0 = gabi::object_at<be<u32>>(0x101FDA50);
static auto& l_debugColour0 = gabi::object_at<be<u32>>(0x101FEBF4);
static auto& l_debugSource0 = gabi::object_at<be<u32>>(0x100236D8);
static auto& l_debugGuard1 = gabi::object_at<be<u32>>(0x101FDAC0);
static auto& l_debugColour1 = gabi::object_at<be<u32>>(0x101FEBF8);
static auto& l_debugSource1 = gabi::object_at<be<u32>>(0x100236DC);
static auto& l_debugGuard2 = gabi::object_at<be<u32>>(0x101FDA44);
static auto& l_debugColour2 = gabi::object_at<be<u32>>(0x101FEBE8);
static auto& l_debugSource2 = gabi::object_at<be<u32>>(0x100236E0);
cXyz& l_hairRoot = gabi::object_at<cXyz>(0x101C6DC0);
cXyz& l_hairTip = gabi::object_at<cXyz>(0x101C6DB4);
#define l_child(i) l_HIO.mChild[i]

static auto& dNpc_cyl_src = gabi::object_at<dCcD_SrcCyl>(0x101EA190);
#define l_cyl_src (&dNpc_cyl_src)

static s32 a_btp_resID_tbl[1] = {8};
static s32 a_head_resID[1] = {7};
static s8 a_initialTexNo[1] = {0};
static u32 a_heap_size_tbl[1] = {0};
static s32 a_bck_resID_tbl[7] = {0, 3, 4, 2, 1, 5, 9};
static daNpc_Yw1_c::anm_prm_c a_anm_num_prm[7] = {
    {0, 0, {}, 8.0f, 1.0f, 2}, {1, 0, {}, 8.0f, 1.0f, 2}, {2, 0, {}, 8.0f, 1.0f, 2},
    {3, 0, {}, 8.0f, 1.0f, 2}, {4, 0, {}, 8.0f, 1.0f, 2}, {5, 0, {}, 8.0f, 1.0f, 2},
    {6, 0, {}, 8.0f, 1.0f, 2}};
static daNpc_Yw1_c::anm_prm_c a_anm_stt_prm[7] = {
    {-1, -1, {}, 0.0f, 0.0f, -1}, {5, 0, {}, 8.0f, 1.0f, 2}, {-1, -1, {}, 0.0f, 0.0f, -1},
    {2, 0, {}, 8.0f, 1.0f, 2},    {5, 0, {}, 8.0f, 1.0f, 2}, {2, 0, {}, 8.0f, 1.0f, 2},
    {5, 0, {}, 8.0f, 1.0f, 2}};
static daNpc_Yw1_c::anm_prm_c a_anm_atr_prm[7] = {
    {0, 0, {}, 8.0f, 1.0f, 2}, {1, 0, {}, 8.0f, 1.0f, 2}, {2, 0, {}, 8.0f, 1.0f, 2},
    {3, 0, {}, 8.0f, 1.0f, 2}, {4, 0, {}, 8.0f, 1.0f, 2}, {5, 0, {}, 8.0f, 1.0f, 2},
    {6, 0, {}, 8.0f, 1.0f, 2}};
static Yw1ActionFunc l_waitAction1 = {&daNpc_Yw1_c::wait_action1};
static Yw1ActionFunc l_waitAction2 = {&daNpc_Yw1_c::wait_action2};
static Yw1HioParameters a_prm_tbl = {{8192, 7800, -8192, -7800, 0, 8400, 0, -8400, 1600},
                                     1200,
                                     170.0f,
                                     0,
                                     {},
                                     300.0f,
                                     6,
                                     1024,
                                     0.949999988079071f,
                                     1.2000000476837158f,
                                     0.5f};
static char* cut_name_tbl[1] = {"DUMMY"};

/* HD: ABI bindings retain the verified register widths. */
u32 Yw1_groundSound(dBgS* bgs, void* poly);
/* mDoExt_McaMorf::play(pos, u32 se, s8 reverb): the reverb register is passed through as loaded */
BOOL McaMorf_play(mDoExt_McaMorf* m, cXyz* pos, u32 se, s32 reverb);
static inline BOOL dNpc_PathRun_nextIdx(dNpc_PathRun_c* p) { return p->nextIdx(); }
void Yw1_setCarryNow(fopAc_ac_c* a, BOOL stageLayer);
void Yw1_constructArray(u32 p, u32 n, u32 size, u32 ctor);
/* inlined into init_YW1_0, init_YW1_1, init_YW1_2, init_YW1_3 */
static inline void set_action_tbl(daNpc_Yw1_c* i_this, u32 entry) {
    Yw1ActionFunc f;
    Yw1_view<Yw1ActionWords>(gabi::ea(&f))->mWords[0] = Yw1_view<Yw1ActionWords>(entry)->mWords[0];
    Yw1_view<Yw1ActionWords>(gabi::ea(&f))->mWords[1] = Yw1_view<Yw1ActionWords>(entry)->mWords[1];
    i_this->set_action(&f, nullptr);
}

static inline J3DAnmTexPattern* btpAnm_pattern(daNpc_Yw1_c* i) { return i->mBtpAnm.mpAnm.get(); }

/* 022FB0C8 */
static BOOL nodeCB_Hair(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        J3DModel* model = j3dSys.getModel();
        daNpc_Yw1_c* i_this = (daNpc_Yw1_c*)model->getUserArea();
        if (i_this != nullptr) {
            i_this->_nodeCB_Hair(node, model);
        }
    }
    return TRUE;
}

/* 022FAEEC */
void daNpc_Yw1_c::_nodeCB_Hair(J3DNode* node, J3DModel* model) {
    /* static cXyz (18.0f, 20.0f, 0.0f): initialised, not used */
    if (l_unusedEyeGuard == 0) {
        l_unusedEyeGuard = 1;
        cXyz* v = &l_unusedEyeOffset;
        v->z = 0.0f;
        v->x = 18.0f;
        v->y = 20.0f;
    }
    J3DJoint* joint = (J3DJoint*)node;
    u16 jntNo = joint->getJntNo();
    Mtx34* stack = mDoMtx_stack_c::get();
    PSMTXCopy(model->getAnmMtx(jntNo), stack);
    if (m_hair1_jnt_num == jntNo) {
        mDoMtx_YrotM(stack, mHair1Yaw);
        mDoMtx_ZrotM(stack, (s16)(mHair1Pitch + mHeadPitch + mHair1Wave));
    } else if (m_hair2_jnt_num == jntNo) {
        mDoMtx_YrotM(stack, mHair2Yaw);
        mDoMtx_ZrotM(stack, (s16)(mHair2Pitch + mHair2Wave));
    } else if (m_hair3_jnt_num == jntNo) {
        mDoMtx_YrotM(stack, mHair3Yaw);
        mDoMtx_ZrotM(stack, (s16)(mHair3Pitch + mHair3Wave));
    }
    PSMTXCopy(stack, j3dSys.mCurrentMtx);
    mtx_copy(model->getAnmMtx(jntNo), stack); /* model->setAnmMtx(jntNo, stack) */
}

/* 022FB288 */
static BOOL nodeCB_Head(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        J3DModel* model = j3dSys.getModel();
        daNpc_Yw1_c* i_this = (daNpc_Yw1_c*)model->getUserArea();
        if (i_this != nullptr) {
            i_this->_nodeCB_Head(node, model);
        }
    }
    return TRUE;
}

/* 022FB110 */
void daNpc_Yw1_c::_nodeCB_Head(J3DNode* node, J3DModel* model) {
    /* static cXyz a_eye_pos_offst(18.0f, 20.0f, 0.0f) */
    cXyz* eyeOffset = &a_eye_pos_offst;
    if (l_eyeGuard == 0) {
        l_eyeGuard = 1;
        eyeOffset->z = 0.0f;
        eyeOffset->x = 18.0f;
        eyeOffset->y = 20.0f;
    }
    J3DJoint* joint = (J3DJoint*)node;
    u16 jntNo = joint->getJntNo();
    Mtx34* stack = mDoMtx_stack_c::get();
    PSMTXCopy(model->getAnmMtx(jntNo), stack);
    mAttPos.x = (f32)stack->m[0][3];
    mAttPos.y = (f32)stack->m[1][3];
    mAttPos.z = (f32)stack->m[2][3];
    mDoMtx_XrotM(stack, m_jnt.mAngles[0][1]);
    mDoMtx_ZrotM(stack, (s16)-m_jnt.mAngles[0][0]);
    PSMTXMultVec(stack, eyeOffset, &mEyePos);
    PSMTXCopy(stack, j3dSys.mCurrentMtx);
    mtx_copy(model->getAnmMtx(jntNo), stack);
}

/* 022FB3F4 */
static BOOL nodeCB_BackBone(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        J3DModel* model = j3dSys.getModel();
        daNpc_Yw1_c* i_this = (daNpc_Yw1_c*)model->getUserArea();
        if (i_this != nullptr) {
            i_this->_nodeCB_BackBone(node, model);
        }
    }
    return TRUE;
}

/* 022FB2D0 */
void daNpc_Yw1_c::_nodeCB_BackBone(J3DNode* node, J3DModel* model) {
    J3DJoint* joint = (J3DJoint*)node;
    u16 jntNo = joint->getJntNo();
    Mtx34* stack = mDoMtx_stack_c::get();
    PSMTXCopy(model->getAnmMtx(jntNo), stack);
    mDoMtx_XrotM(stack, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stack, (s16)-m_jnt.mAngles[1][0]);
    PSMTXCopy(stack, j3dSys.mCurrentMtx);
    mtx_copy(model->getAnmMtx(jntNo), stack);
}

/* 022FBB00 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) { return ((daNpc_Yw1_c*)i_this)->CreateHeap(); }

/* 022FBB04 */
static void* searchActor_Bm1(void* p, void*) {
    if (l_bm1Count < 20 && fopAc_IsActor(p) && p != nullptr && fpcM_GetName(p) == Yw1RitoProcess) {
        s32 n = l_bm1Count;
        l_bm1Count = n + 1;
        l_bm1Actors[n] = (fopAc_ac_c*)p;
    }
    return nullptr;
}

/* 022FBDAC */
BOOL daNpc_Yw1_c::init_YW1_0() {
    if (dComIfGs_isEventBit(0x520) || dComIfGs_isEventBit(0x1) || mPathRun.mPath.get() == nullptr) {
        return FALSE;
    }
    mbTsubo = 1;
    mTsuboId = fopAcM_create(Yw1PotProcess, 0x7F063F, &current.pos, current.roomNo, nullptr,
                             nullptr, -1, 0);
    set_action_tbl(this, gabi::ea(&l_waitAction1)); /* &daNpc_Yw1_c::wait_action1 */
    set_pthPoint(0);
    return mTsuboId != fpcM_ERROR_PROCESS_ID_e;
}

/* 022FBEB4 */
BOOL daNpc_Yw1_c::init_YW1_1() {
    if (dComIfGs_isEventBit(0x520)) {
        return FALSE;
    }
    if (!dComIfGs_isEventBit(0x1)) {
        actor_status &= ~0x3Fu;
    }
    set_action_tbl(this, gabi::ea(&l_waitAction2)); /* &daNpc_Yw1_c::wait_action2 */
    mbAwaitRescue = 1;
    return TRUE;
}

/* 022FBF70 */
BOOL daNpc_Yw1_c::init_YW1_2() {
    if (!dComIfGs_isEventBit(0x520)) {
        return FALSE;
    }
    if (dKy_daynight_check() != 1 && dComIfGs_isEventBit(0x2A20)) {
        return FALSE;
    }
    set_action_tbl(this, gabi::ea(&l_waitAction2)); /* &daNpc_Yw1_c::wait_action2 */
    return TRUE;
}

/* 022FC024 */
BOOL daNpc_Yw1_c::init_YW1_3() {
    if (!dComIfGs_isEventBit(0x520) || dKy_daynight_check() != 0 ||
        mPathRun.mPath.get() == nullptr || !dComIfGs_isEventBit(0x2A20)) {
        return FALSE;
    }
    mbTsubo = 1;
    mTsuboId = fopAcM_create(Yw1PotProcess, 0x7F063F, &current.pos, current.roomNo, nullptr,
                             nullptr, -1, 0);
    set_action_tbl(this, gabi::ea(&l_waitAction1)); /* &daNpc_Yw1_c::wait_action1 */
    set_pthPoint(0);
    return mTsuboId != fpcM_ERROR_PROCESS_ID_e;
}

/* 022FCF9C */
BOOL daNpc_Yw1_c::createInit() {
    setActorInfo2(&mEventCut, "Yw1", this);
    u8 pathIdx = (fopAcM_GetParam(this) >> 16) & 0xFF;
    attention_info.flags =
        fopAc_Attn_LOCKON_TALK_e |
        fopAc_Attn_ACTION_SPEAK_e; /* attention_info.flags: LOCKON_TALK | ACTION_SPEAK */
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAB;  /* attention_info.distances[1] */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xAB; /* attention_info.distances[3] */
    s32 weight = 0xFF;
    if (pathIdx != 0xFF) {
        dNpc_PathRun_setInf(&mPathRun, pathIdx, current.roomNo, 1);
        if (mPathRun.mPath.get() == nullptr) {
            return FALSE;
        }
        actor_status &= ~0x80u;
        weight = 0xF0;
    }
    mAnmNo = 7;
    BOOL ok;
    switch (mType) {
    case 0:
        ok = init_YW1_0();
        break;
    case 1:
        ok = init_YW1_1();
        break;
    case 2:
        ok = init_YW1_2();
        break;
    case 3:
        ok = init_YW1_3();
        break;
    default:
        return FALSE;
    }
    if (!ok) {
        return FALSE;
    }
    mAngle.x = current.angle.x;
    mAngle.y = current.angle.y;
    mAngle.z = current.angle.z;
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    gravity = -4.5f;
    mStts.Init(weight, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(l_cyl_src);
    play_animation();
    mObjAcch.CrrPos(dComIfG_Bgsp());
    tevStr.mRoomNo = (s8)dBgS_GetRoomId(dComIfG_Bgsp(), gnd_poly(mObjAcch)); /* tevStr.mRoomNo */
    tevStr.mEnvrIdxOverride =
        (u8)dBgS_GetPolyColor(dComIfG_Bgsp(), gnd_poly(mObjAcch)); /* tevStr.mEnvrIdxOverride */
    mpMorf->setMorf(0.0f);
    setMtx(1);
    return TRUE;
}

/* 022FC1F0 */
void daNpc_Yw1_c::play_animation() {
    u32 se = 0;
    play_texPttrnAnm();
    if (mObjAcch.ChkGroundHit()) {
        se = Yw1_groundSound(dComIfG_Bgsp(), gnd_poly(mObjAcch));
    }
    s32 reverb = dComIfGp_getReverb(current.roomNo);
    mAnmEnd = (s8)McaMorf_play(mpMorf, &eyePos, se, reverb);
    if (mpMorf->getFrame() < mPrevFrame) {
        mAnmEnd = 1;
    }
    mPrevFrame = mpMorf->getFrame();
}

/* 022FCC54 */
// HD: the pot follows the head
u8 daNpc_Yw1_c::upLift() {
    Yw1SearchError error;
    fopAc_ac_c* tsubo = searchByID(mTsuboId, &error.mValue);
    u8 lost = error.mValue == 1;
    mbTsuboLost = lost;
    if (tsubo == nullptr) {
        return lost;
    }
    if (!(tsubo->actor_status & fopAcStts_CARRY_e)) {
        Yw1_setCarryNow(tsubo, FALSE);
    }
    cXyz ofs;
    ofs.x = 34.0f;
    ofs.y = -4.0f;
    ofs.z = 0.0f;
    cXyz dir;
    PSMTXMultVecSR(mpMorf->getModel()->getAnmMtx(m_head_jnt_num), &l_liftUp /* (0, 1, 0) */, &dir);
    tsubo->shape_angle.y = cM_atan2s(dir.x, dir.z);
    PSMTXCopy(mpMorf->getModel()->getAnmMtx(m_head_jnt_num), mDoMtx_stack_c::get());
    PSMTXMultVec(mDoMtx_stack_c::get(), &ofs, &tsubo->current.pos);
    return mbTsuboLost;
}

/* 022FCDEC */
void daNpc_Yw1_c::setMtx(s32 force) {
    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), mAngle.x, mAngle.y, mAngle.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    setHairAngle();
    Mtx34* src = mpMorf->getModel()->getAnmMtx(m_head_jnt_num);
    J3DModel_setBaseTRMtx(mpHedModel, src);
    J3DModel_calc(mpHedModel);
    upLift();
    setAttention(force);
}

/* 022FDF08 */
s32 daNpc_Yw1_c::bckResID(int num) { return a_bck_resID_tbl[num]; /* a_bck_resID_tbl */ }

/* 022FB6C0 */
// HD: HD: one texture pattern; the number is not used
s32 daNpc_Yw1_c::btpResID(int) { return a_btp_resID_tbl[0]; }

/* 022FB6CC */
BOOL daNpc_Yw1_c::init_texPttrnAnm(s8 num, s32 modify) {
    J3DModel* hedModel = mpHedModel;
    if (num < 0) {
        return FALSE;
    }
    s32 resID = btpResID(num);
    J3DAnmTexPattern* btp = (J3DAnmTexPattern*)dComIfG_getObjectIDRes("Yw", resID);
    mTexNo = num;
    mBtpFrame = 0;
    mBlinkTimer = 0;
    return mDoExt_btpAnm_init(&mBtpAnm, Yw1_modelData(hedModel), btp, 1, 0 /* HD: EMode_NONE */,
                              1.0f, 0, -1, modify, 0) != 0;
}

/* 022FC138 */
void daNpc_Yw1_c::play_texPttrnAnm() {
    if (mTexNo == 0 && cLib_calcTimer(&mBlinkTimer) != 0) {
        return;
    }
    u8 frame = (u8)(mBtpFrame + 1);
    mBtpFrame = frame;
    if ((s32)frame < Yw1_patternFrameMax(btpAnm_pattern(this))) {
        return;
    }
    if (mTexNo != 0) {
        mBtpFrame = (u8)Yw1_patternFrameMax(btpAnm_pattern(this));
    } else {
        mBlinkTimer = (s16)cLib_getRndValue(60, 90);
        mBtpFrame = 0;
    }
}

/* 022FDF1C */
void daNpc_Yw1_c::setAnm_anm(anm_prm_c* prm) {
    s8 anmNo = prm->mAnmNo;
    if (anmNo < 0 || mAnmNo == anmNo) {
        return;
    }
    s32 resID = bckResID(anmNo);
    s32 loop = prm->mLoopMode;
    f32 speed = prm->mSpeed;
    f32 morf = prm->mMorf;
    dNpc_setAnmIDRes(mpMorf, loop, morf, speed, resID, -1, "Yw");
    mAnmNo = prm->mAnmNo;
    mPrevFrame = 0.0f;
    mAnmEnd = 0;
    mLoopCnt = 0;
}

/* 022FDFB8 */
void daNpc_Yw1_c::setAnm_NUM(int num, int withTex) {
    if (withTex) {
        init_texPttrnAnm(a_anm_num_prm[num].mTexNo, 1);
    }
    setAnm_anm(&a_anm_num_prm[num]);
}

/* 022FE028 */
void daNpc_Yw1_c::setAnm() {
    init_texPttrnAnm(a_anm_stt_prm[mStt].mTexNo, 1);
    setAnm_anm(&a_anm_stt_prm[mStt]);
}

/* 022FE100 */
void daNpc_Yw1_c::chngAnmAtr(u8 atr) {
    if (atr == mAnmAtr || atr >= 7) {
        return;
    }
    mAnmAtr = atr;
    setAnm_ATR();
}

/* 022FE098 */
void daNpc_Yw1_c::setAnm_ATR() {
    init_texPttrnAnm(a_anm_atr_prm[mAnmAtr].mTexNo, 1);
    setAnm_anm(&a_anm_atr_prm[mAnmAtr]);
}

/* 022FE11C */
void daNpc_Yw1_c::anmAtr(u16 msgStatus) {
    if (msgStatus == 6 /* fopMsgStts_MSG_TYPING_e */) {
        if (mAtrCnt == 0) {
            chngAnmAtr(dComIfGp_getMesgAnimeAttrInfo()); /* dComIfGp_getMesgAnimeAttrInfo */
            mAtrCnt = (s8)(mAtrCnt + 1);
        }
        u8 tag = dComIfGp_getMesgAnimeTagInfo(); /* dComIfGp_getMesgAnimeTagInfo */
        if (tag != 0xFF && tag != mAnmTag) {
            dComIfGp_clearMesgAnimeTagInfo(); /* dComIfGp_clearMesgAnimeTagInfo */
            mAnmTag = tag;
            /* chngAnmTag(): empty */
        }
    } else if (msgStatus == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
        mAtrCnt = 0;
    }
    /* ctrlAnmAtr(), ctrlAnmTag(): empty */
}

/* 022FE1EC */
u16 daNpc_Yw1_c::next_msgStatus(u32* pMsgNo) {
    be<u32>* msgNo = (be<u32>*)pMsgNo;
    if (*msgNo != 0x8A3) {
        return 0x10; /* fopMsgStts_MSG_ENDS_e */
    }
    if (!dComIfGs_isEventBit(0x2A20)) {
        *msgNo = 0x8A4;
        return 0xF;
    }
    *msgNo = dKy_daynight_check() ? 0x8A5 : 0x8A6;
    return 0xF; /* fopMsgStts_MSG_CONTINUES_e */
}

/* 022FE28C */
u32 daNpc_Yw1_c::getMsg_YW1_0() {
    if (mbPotPenaltyPaid != 0) {
        return 0x8A0;
    }
    if (mbTsuboLost != 0) {
        return 0x89F;
    }
    if (dComIfGs_isEventBit(0x1)) {
        return dComIfGs_isEventBit(0x140) ? 0x89C : 0x89B;
    }
    if (mPathRun.mPath.get() != nullptr && mbPathEnd != 0) {
        return mPathRun.mbDir == 1 ? 0x8A2 : 0x8A1;
    }
    if (dComIfGs_isEventBit(0x180)) {
        return 0x89A;
    }
    return dComIfGs_isEventBit(0x2A80) ? 0x8AA : 0x899;
}

/* 022FE3FC */
u32 daNpc_Yw1_c::getMsg_YW1_1() {
    if (dComIfGs_isEventBit(0xE20)) {
        return dComIfGs_isEventBit(0x120) ? 0x89E : 0x89D;
    }
    return dComIfGs_isEventBit(0x140) ? 0x89C : 0x89B;
}

/* 022FE48C */
u32 daNpc_Yw1_c::getMsg_YW1_2() {
    if (!dComIfGs_isEventBit(0x3A40)) {
        return 0x8A3;
    }
    if (!dComIfGs_isEventBit(0x2A20)) {
        return 0x8A7;
    }
    return dKy_daynight_check() ? 0x8A8 : 0x8A9;
}

/* 022FE52C */
u32 daNpc_Yw1_c::getMsg_YW1_3() {
    if (mbPotPenaltyPaid != 0) {
        return 0x8A0;
    }
    if (mbTsuboLost != 0) {
        return 0x89F;
    }
    if (mPathRun.mPath.get() != nullptr && mbPathEnd != 0) {
        return mPathRun.mbDir == 1 ? 0x8A2 : 0x8A1;
    }
    return getMsg_YW1_2();
}

/* 022FE588 */
u32 daNpc_Yw1_c::getMsg() {
    switch (mType) {
    case 0:
        return getMsg_YW1_0();
    case 1:
        return getMsg_YW1_1();
    case 2:
        return getMsg_YW1_2();
    case 3:
        return getMsg_YW1_3();
    default:
        return 0;
    }
}

/* 022FDAA4 */
void daNpc_Yw1_c::eventOrder() {
    if (mOrderType == 1 || mOrderType == 2) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        if (mOrderType == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}

/* 022FD448 */
void daNpc_Yw1_c::checkOrder() {
    u16 cmd = eventInfo.mCommand; /* eventInfo.mCommand */
    if (cmd == 2 /* dEvtCmd_INDEMO_e */) {
        return;
    }
    if (cmd != 1 /* dEvtCmd_INTALK_e */) {
        return;
    }
    if (mOrderType == 1 || mOrderType == 2) {
        mOrderType = 0;
        mbTalk = 1;
    }
}

/* 022FE5EC */
BOOL daNpc_Yw1_c::chk_talk() {
    u8 talkXY = dComIfGp_event_chkTalkXY();
    if ((u32)(talkXY - 1) <= 3) { /* dComIfGp_event_chkTalkXY() */
        if (dComIfGp_evmng_ChkPresentEnd()) {
            mPreItemNo = dComIfGp_event_getPreItemNo(); /* dComIfGp_event_getPreItemNo() */
            return TRUE;
        }
        return FALSE;
    }
    mPreItemNo = 0xFF;
    return TRUE;
}

/* 022FE684 */
u8 daNpc_Yw1_c::chk_parts_notMov() {
    return mSaveHeadY == m_jnt.mAngles[0][1] && mSaveBboneY == m_jnt.mAngles[1][1] &&
           mSaveAngleY == current.angle.y;
}

/* 022FCBF4 */
fopAc_ac_c* daNpc_Yw1_c::searchByID(fpc_ProcID id, be<s32>* pErr) {
    Yw1SearchResult result;
    result.mActor = 0;
    if (pErr != nullptr) {
        *pErr = 0;
    }
    BOOL found = fopAcM_SearchByID2(id, &result.mActor);
    fopAc_ac_c* a = Yw1_view<fopAc_ac_c>(result.mActor);
    if (!found && pErr != nullptr) {
        *pErr = 1;
    }
    return a;
}

/* 022FD330 */
BOOL daNpc_Yw1_c::partner_search_sub(u32 searchFn) {
    BOOL ret = FALSE;
    mBm1Id = fpcM_ERROR_PROCESS_ID_e;
    l_bm1Count = 0;
    for (int i = 0; i < 20; i++) {
        l_bm1Actors[i] = nullptr;
    }
    fpcM_Search(searchFn, this);
    if (l_bm1Count != 0) {
        mBm1Id = fopAcM_GetID(l_bm1Actors[0].get());
        ret = TRUE;
    }
    return ret;
}

/* 022FD3DC */
void daNpc_Yw1_c::partner_search() {
    if (mActStep == 1) {
        if (mType != 0 || partner_search_sub(gabi::guest_fn((const void*)searchActor_Bm1))) {
            mActStep = (s8)(mActStep + 1);
        }
    }
}

/* 022FD768 */
void daNpc_Yw1_c::lookBack() {
    cXyz dst;
    u8 headOnly = mbHeadOnly;
    dst.x = 0.0f;
    s16 targetY = current.angle.y;
    mSaveAngleY = targetY;
    mSaveHeadY = m_jnt.mAngles[0][1];
    dst.y = 0.0f;
    f32 eyeX = current.pos.x;
    f32 eyeY = eyePos.y;
    dst.z = 0.0f;
    mSaveBboneY = m_jnt.mAngles[1][1];
    f32 eyeZ = current.pos.z;
    cXyz* dstPos = nullptr;
    switch (mLookMode) {
    case 0:
        break;
    case 1: {
        cXyz tmp;
        dNpc_playerEyePos(&tmp, -20.0f);
        dst.copy(tmp);
        dstPos = &dst;
        break;
    }
    case 2:
        dst.copy(mLookPos);
        dstPos = &dst;
        break;
    case 3:
        targetY = mTargetAngY;
        break;
    case 4: {
        fopAc_ac_c* actor = searchByID(mLookActorId, nullptr);
        if (actor != nullptr) {
            mLookPos.copy(actor->current.pos);
            dst.x = (f32)mLookPos.x;
            f32 y = actor->eyePos.y;
            dst.y = y;
            dst.z = (f32)mLookPos.z;
            mLookPos.y = y;
            dstPos = &dst;
        }
        break;
    }
    }
    cLib_addCalcAngleS2(&mHeadTurnSpd, l_child(mHioNo).mHeadTurnSpd, 4, 0x800);
    s16 maxVel;
    if (!m_jnt.mbTrn) {
        maxVel = 0;
        mHeadTurnSpd = 0;
    } else {
        maxVel = mHeadTurnSpd;
    }
    cXyz eye;
    eye.x = eyeX;
    eye.y = eyeY;
    eye.z = eyeZ;
    lookAtTarget(&m_jnt, &current.angle.y, dstPos, &eye, targetY, maxVel, headOnly);
}

/* 022FE6C4 */
BOOL daNpc_Yw1_c::chkAttention() {
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == dAttention_LockonTarget(attention, 0);
    } else {
        return this == dAttention_ActionTarget(attention, 0);
    }
}

/* 022FCD88 */
void daNpc_Yw1_c::setAttention(s32 force) {
    cXyz* attnPos = &attention_info.position; /* attention_info.position */
    attnPos->set(current.pos.x, current.pos.y + l_child(mHioNo).mAttnYOffset, current.pos.z);
    if (mActRet == 0 && !force) {
        return;
    }
    eyePos.set(mEyePos.x, mEyePos.y, mEyePos.z);
}

/* 022FBB84 */
u8 daNpc_Yw1_c::decideType(int type) {
    mHioNo = 0;
    if ((u32)type > 3) {
        mType = -1;
        return FALSE;
    }
    mType = (s8)type;
    return mHioNo != -1 && (s8)type != -1;
}

/* 022FD6D0 */
void daNpc_Yw1_c::privateCut(int staffIdx) {
    if (staffIdx == -1) {
        return;
    }
    /* static char* cut_name_tbl[] = {"DUMMY"} (101C6DD8) */
    s8 actIdx = (s8)dComIfGp_evmng_getMyActIdx(staffIdx, cut_name_tbl, 1, TRUE, 0);
    mActIdx = actIdx;
    if (actIdx != -1) {
        dComIfGp_evmng_getIsAddvance(staffIdx);
    }
    dComIfGp_evmng_cutEnd(staffIdx);
}

/* 022FD68C */
void daNpc_Yw1_c::endEvent() {
    dComIfGp_event_reset();
    mAnmAtr = 0xFF;
    mAnmTag = 0xFF;
}

/* 022FD64C */
s32 daNpc_Yw1_c::isEventEntry() {
    const char* name = Yw1_view<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}

/* 022FDA4C */
void daNpc_Yw1_c::event_proc(int staffIdx) {
    if (!mEventCut.cutProc()) {
        privateCut(staffIdx);
    }
    lookBack();
}

/* 022FBBD8 */
BOOL daNpc_Yw1_c::set_action(Yw1ActionFunc* action, void* arg) {
    s16 idx = action->mFunction.i;
    s16 delta = action->mFunction.d;
    u32 fn = action->mFunction.f;
    if (mAction.mFunction.i == idx) {
        if (idx == 0 || (mAction.mFunction.d == delta && mAction.mFunction.f == fn)) {
            return TRUE;
        }
    }
    if (mAction.mFunction.i != 0) {
        // HD: exiting actions use step 9, rather than the GameCube convention of -1.
        mActStep = 9;
        Yw1_callAction(&mAction, this, arg);
    }
    mAction.mFunction.f = fn;
    mAction.mFunction.d = delta;
    mAction.mFunction.i = idx;
    mActStep = 0;
    Yw1_callAction(&mAction, this, arg);
    return TRUE;
}

/* 022FE850 */
void daNpc_Yw1_c::setStt(s8 stt) {
    s8 prev = mStt;
    mStt = stt;
    switch (stt) {
    case 2:
        mPrevStt = prev;
        mAnmAtr = 0xFF;
        mAnmTag = 0xFF;
        mAtrCnt = 0;
        break;
    case 3:
    case 5:
        mbHeadOnly = 1;
        break;
    }
    setAnm();
    chngTsuboAnm();
}

/* 022FE74C */
void daNpc_Yw1_c::chngTsuboAnm() {
    if (mbTsubo == 0) {
        return;
    }
    if (mbTsuboLost == 0) {
        switch (mAnmNo) {
        case 4:
            setAnm_NUM(3, 1);
            break;
        case 5:
            setAnm_NUM(1, 1);
            break;
        }
        return;
    }
    switch (mAnmNo) {
    case 1:
        setAnm_NUM(5, 1);
        break;
    case 3:
        setAnm_NUM(4, 1);
        break;
    }
    mbTsubo = 0;
}

/* 022FE8F8 */
BOOL daNpc_Yw1_c::chk_areaIN(f32 r, f32 h, s16 ang, cXyz* pos) {
    cXyz d;
    cXyz_mi(&dComIfGp_getLinkPlayer()->current.pos, &d, pos);
    cXyz xz;
    xz.y = 0.0f;
    xz.x = (f32)d.x;
    xz.z = (f32)d.z;
    f32 dist = std_sqrtf(PSVECSquareMag(&xz));
    f32 dy = dComIfGp_getLinkPlayer()->current.pos.y - pos->y;
    s16 a = (s16)(cLib_targetAngleY(&current.pos, &dComIfGp_getLinkPlayer()->current.pos) -
                  current.angle.y);
    if (dist < r && std::fabs(dy) < h && std::abs((s32)a) < ang) {
        return TRUE;
    }
    return FALSE;
}

/* 022FBD04 */
void daNpc_Yw1_c::set_pthPoint(u8 idx) {
    if (mPathRun.mPath.get() != nullptr) {
        mPathRun.mIdx = idx;
        cXyz pnt;
        dNpc_PathRun_getPoint(&mPathRun, &pnt, idx);
        current.pos.copy(pnt);
        if (dNpc_PathRun_nextIdx(&mPathRun)) {
            cXyz pnt2;
            dNpc_PathRun_getPoint(&mPathRun, &pnt2, mPathRun.mIdx);
            cXyz next;
            next.copy(pnt2);
            current.angle.y = cLib_targetAngleY(&current.pos, &next);
        }
    }
}

/* 022FEA7C */
u8 daNpc_Yw1_c::chk_brkTsubo() {
    if (mbTsuboLost == 0) {
        return 0;
    }
    setStt(Yw1WaitPath);
    u8 ret = mbTsuboLost;
    mbHeadOnly = 0;
    mLookMode = 0;
    speedF = 0.0f;
    return ret;
}

/* 022FEAD8 */
u8 daNpc_Yw1_c::chk_bm1Odoroki() {
    fopAc_ac_c* bm1 = searchByID(mBm1Id, nullptr);
    if (bm1 == nullptr) {
        return 0;
    }
    return Yw1_view<Yw1RitoView>(gabi::ea(bm1))
        ->mbStartled; /* daNpc_Bm1_c: the Rito partner is startled */
}

/* 022FEB10 */
BOOL daNpc_Yw1_c::wait_1() {
    if (chk_brkTsubo()) {
        return TRUE;
    }
    if (mbTalk != 0) {
        if (chk_talk()) {
            setStt(Yw1Talk);
            mbHeadOnly = 0;
            m_jnt.mbTrn = 1;
            mLookMode = 1;
        }
        return TRUE;
    }
    if (chk_bm1Odoroki()) {
        mLookMode = 4;
        mLookActorId = mBm1Id;
        return TRUE;
    }
    mOrderType = 2;
    chngTsuboAnm();
    cXyz pos;
    pos.x = (f32)current.pos.x;
    pos.y = (f32)current.pos.y;
    pos.z = (f32)current.pos.z;
    if (chk_areaIN(l_child(mHioNo).mAreaRadius + 100.0f, 100.0f, 0x4400, &pos)) {
        mWaitTimer = (s16)cLib_getRndValue(10, 20);
        mLookMode = 1;
        return TRUE;
    }
    if (cLib_calcTimer(&mWaitTimer) != 0) {
        mLookMode = 1;
        return TRUE;
    }
    setStt(Yw1Walk);
    mLookMode = 0;
    mbHeadOnly = 1;
    return TRUE;
}

/* 022FEC58 */
BOOL daNpc_Yw1_c::wait_2() {
    if (mbTalk != 0) {
        if (chk_talk()) {
            setStt(Yw1Talk);
            mbHeadOnly = 0;
            m_jnt.mbTrn = 1;
            mLookMode = 1;
        }
        return TRUE;
    }
    if (chk_bm1Odoroki()) {
        mbHeadOnly = 0;
        mLookActorId = mBm1Id;
        mLookMode = 4;
        m_jnt.mbTrn = 1;
        return TRUE;
    }
    mOrderType = 2;
    mbHeadOnly = 1;
    cLib_addCalcAngleS(&current.angle.y, mHomeAngle.y, 4, 0x800, 0x80);
    if (mbAttention != 0) {
        mWaitTimer = (s16)cLib_getRndValue(10, 20);
    }
    mLookMode = cLib_calcTimer(&mWaitTimer) != 0 ? 1 : 0;
    return TRUE;
}

/* 022FED54 */
BOOL daNpc_Yw1_c::wait_3() {
    if (mbTalk != 0) {
        if (chk_talk()) {
            setStt(Yw1Talk);
            mLookMode = 1;
            mbHeadOnly = 0;
            m_jnt.mbTrn = 1;
        }
        return TRUE;
    }
    mOrderType = 2;
    mbHeadOnly = 1;
    cLib_addCalcAngleS(&current.angle.y, mHomeAngle.y, 4, 0x800, 0x80);
    if (mbAttention != 0) {
        mWaitTimer = (s16)cLib_getRndValue(10, 20);
    }
    mLookMode = cLib_calcTimer(&mWaitTimer) != 0 ? 1 : 0;
    return TRUE;
}

/* 022FEE28 */
BOOL daNpc_Yw1_c::walk_1() {
    if (chk_brkTsubo()) {
        return TRUE;
    }
    f32 target;
    bool walk = false;
    if (mbPathEnd == 0) {
        cXyz pos;
        pos.x = (f32)current.pos.x;
        pos.y = (f32)current.pos.y;
        pos.z = (f32)current.pos.z;
        if (dNpc_PathRun_chkPointPass(&mPathRun, &pos, mPathRun.mbDir != 0)) {
            mbPathEnd = (u8)(dNpc_PathRun_nextIdx(&mPathRun) ^ 1);
            walk = mbPathEnd == 0;
        } else {
            walk = mbPathEnd == 0;
        }
    }
    if (walk) {
        cXyz pos;
        pos.x = (f32)current.pos.x;
        pos.y = (f32)current.pos.y;
        pos.z = (f32)current.pos.z;
        if (chk_areaIN(l_child(mHioNo).mAreaRadius, 100.0f, 0x4000, &pos) || mbTalk != 0 ||
            chk_bm1Odoroki()) {
            walk = false;
        }
    }
    if (walk) {
        cXyz pnt;
        dNpc_PathRun_getPoint(&mPathRun, &pnt, mPathRun.mIdx);
        cXyz next;
        next.copy(pnt);
        s16 ang = cLib_targetAngleY(&current.pos, &next);
        cLib_addCalcAngleS(&current.angle.y, ang, l_child(mHioNo).mTurnScale,
                           l_child(mHioNo).mTurnStep, 0);
        target = l_child(mHioNo).mWalkSpd;
    } else {
        target = 0.0f;
    }
    cLib_chaseF(&speedF, target, l_child(mHioNo).mWalkAccel);
    f32 rate = speedF * l_child(mHioNo).mWalkAnmRate;
    mpMorf->setPlaySpeed(rate - 0.5f >= 0.0f ? rate : 0.5f); /* fsel */
    if (gabi::ftoi(target) != 0 || gabi::ftoi(speedF) != 0) {
        mOrderType = 2;
        mLookMode = 0;
        return TRUE;
    }
    speedF = 0.0f;
    if (mbTalk != 0) {
        if (chk_talk()) {
            setStt(Yw1Talk);
            mbHeadOnly = 0;
            m_jnt.mbTrn = 1;
            mLookMode = 1;
        }
        return TRUE;
    }
    if (mbPathEnd != 0) {
        setStt(Yw1Turn);
        mLookMode = 0;
        return TRUE;
    }
    setStt(Yw1Wait);
    mbHeadOnly = 0;
    m_jnt.mbTrn = 1;
    mLookMode = 1;
    return TRUE;
}

/* 022FF190 */
BOOL daNpc_Yw1_c::turn_1() {
    if (chk_brkTsubo()) {
        return TRUE;
    }
    if (mbTalk != 0) {
        if (chk_talk()) {
            setStt(Yw1Talk);
        }
        return TRUE;
    }
    mOrderType = 2;
    cXyz pnt;
    dNpc_PathRun_getPoint(&mPathRun, &pnt, mPathRun.mIdx);
    cXyz next;
    next.copy(pnt);
    s16 ang = cLib_targetAngleY(&current.pos, &next);
    cLib_chaseAngleS(&current.angle.y, ang, l_child(mHioNo).mTurnStep);
    if (current.angle.y == ang) {
        setStt(Yw1Walk);
        mbPathEnd = 0;
        mLookMode = 0;
        mPathRun.mbDir = (u8)(mPathRun.mbDir ^ 1);
    }
    return TRUE;
}

/* 022FF290 */
BOOL daNpc_Yw1_c::talk_1() {
    u8 notMove = chk_parts_notMov();
    talk(1);
    if (mbHasMsg != 0 && fopMsgM_SearchByID(l_msgMng()) == 0x13 /* fopMsgStts_BOX_CLOSED_e */) {
        u32 msgNo = mCurrMsgNo;
        switch (msgNo) {
        case 0x899:
        case 0x8AA:
            dComIfGs_onEventBit(0x180);
            break;
        case 0x89B:
            dComIfGs_onEventBit(0x140);
            break;
        case 0x89D:
            dComIfGs_onEventBit(0x120);
            break;
        case 0x89F:
            dComIfGp_setItemRupeeCount(-10);
            mbTalk = 0;
            mbPotPenaltyPaid = 1;
            break;
        case 0x8A4:
        case 0x8A5:
        case 0x8A6:
            dComIfGs_onEventBit(0x3A40);
            break;
        }
        s8 prev = mPrevStt;
        mPreItemNo = 0xFF;
        mbTalk = 0;
        setStt(prev);
        mWaitTimer = (s16)cLib_getRndValue(10, 20);
        endEvent();
    }
    return notMove;
}

/* 022FF4FC */
BOOL daNpc_Yw1_c::wait_action1(void*) {
    s8 step = mActStep;
    if (step == 0) {
        setStt(Yw1Walk);
        mActStep = (s8)(mActStep + 1);
    } else if ((u32)(s32)step <= 3) {
        mbAttention = chkAttention();
        switch (mStt) {
        case Yw1Wait:
            mActRet = wait_1();
            break;
        case Yw1Talk:
            mActRet = talk_1();
            break;
        case Yw1Walk:
            mActRet = walk_1();
            break;
        case Yw1WaitPath:
            mActRet = wait_2();
            break;
        case Yw1Turn:
            mActRet = turn_1();
            break;
        }
        lookBack();
    }
    if (dComIfGs_isEventBit(0x1) && mType == 0) {
        Yw1SearchError error;
        fopAc_ac_c* tsubo = searchByID(mTsuboId, &error.mValue);
        if (error.mValue == 0 && tsubo != nullptr) {
            fopAcM_delete(tsubo);
        }
        fopAcM_delete(this);
    }
    return TRUE;
}

/* 022FF698 */
BOOL daNpc_Yw1_c::wait_action2(void*) {
    if (mActStep == 0) {
        setStt(Yw1AwaitRescue);
        mActStep = (s8)(mActStep + 1);
        return TRUE;
    }
    if ((u32)(s32)mActStep > 3) {
        return TRUE;
    }
    mbAttention = chkAttention();
    switch (mStt) {
    case Yw1Talk:
        mActRet = talk_1();
        break;
    case Yw1AwaitRescue:
        mActRet = wait_3();
        break;
    }
    lookBack();
    return TRUE;
}

/* 022FD488 */
u8 daNpc_Yw1_c::demo() {
    u8 id = demoActorID;
    if (id == 0) {
        if (mbDemo != 0) {
            mbDemo = 0;
        }
        return 0;
    }
    if (mbDemo == 0) {
        m_jnt.mAngles[0][1] = 0;
        m_jnt.mAngles[1][0] = 0;
        mbDemo = 1;
        mbKeepShapeAngle = 0;
        m_jnt.mAngles[0][0] = 0;
        id = demoActorID;
        m_jnt.mAngles[1][1] = 0;
    }
    void* demoAc = nullptr;
    if (id != 0 && id <= 0x20) { /* dComIfGp_demo_getActor(id) */
        if (l_demoObject == 0)   /* JUT_ASSERT(0x23A, m_object != NULL) */
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
        demoAc = dDemo_object_getActor(l_demoObject, id);
    }
    if (btpAnm_pattern(this) != nullptr) {
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame;
        if ((s32)frame >= Yw1_patternFrameMax(btpAnm_pattern(this))) {
            mBtpFrame = (u8)Yw1_patternFrameMax(btpAnm_pattern(this));
        }
    }
    if (demoAc != nullptr) {
        J3DAnmTexPattern* btp = dDemo_actor_getP_BtpData(demoAc, "Yw");
        if (btp != nullptr) {
            mDoExt_btpAnm_init(&mBtpAnm, Yw1_modelData(mpHedModel), btp, 1, 0, 1.0f, 0, -1, 1, 0);
            mBtpFrame = 0;
            mTexNo = 1;
        }
    }
    dDemo_setDemoData(this, 0x6A, mpMorf, "Yw", 0, 0, 0, 0);
    return mbDemo;
}

/* 022FDD68 */
BOOL daNpc_Yw1_c::_draw() {
    J3DModel* model = mpMorf->getModel();
    J3DModel* hedModel = mpHedModel;
    u32 hedData = Yw1_modelData(hedModel);
    if (mbAwaitRescue != 0 || mbHidden != 0) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), hedModel, &tevStr);
    dComIfGp_get(); /* HD: unused */
    mpMorf->entryDL();
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)&mBtpAnm, Yw1_view<J3DModelData>(hedData), mBtpFrame);
    mDoExt_modelEntryDL(hedModel);
    Yw1_view<Yw1ModelEffects>(hedData)->mTexPattern = 0; /* mBtpAnm.remove(hedData) */
    dSnap_RegistFig(0x4B /* DSNAP_TYPE_NPC_YW1 */, this, 1.0f, 1.0f, 1.0f);
    if (l_child(mHioNo).mDebugDraw != 0) {
        /* debug colours (GXColor function-local statics, unused) */
        if (l_debugGuard0 == 0) {
            l_debugGuard0 = 1;
            Yw1_copyBytes(gabi::ea(&l_debugColour0), gabi::ea(&l_debugSource0), 4);
        }
        if (l_debugGuard1 == 0) {
            l_debugGuard1 = 1;
            Yw1_copyBytes(gabi::ea(&l_debugColour1), gabi::ea(&l_debugSource1), 4);
        }
        if (l_debugGuard2 == 0) {
            l_debugGuard2 = 1;
            Yw1_copyBytes(gabi::ea(&l_debugColour2), gabi::ea(&l_debugSource2), 4);
        }
    }
    return TRUE;
}

/* 022FDADC */
BOOL daNpc_Yw1_c::_execute() {
    if (mbInit == 0) {
        mHomeAngle.y = current.angle.y;
        *Yw1_view<be<u32>>(gabi::ea(&mHomePos.y)) = *Yw1_view<be<u32>>(gabi::ea(&current.pos.y));
        mHomeAngle.z = current.angle.z;
        mHomeAngle.x = current.angle.x;
        *Yw1_view<be<u32>>(gabi::ea(&mHomePos.z)) = *Yw1_view<be<u32>>(gabi::ea(&current.pos.z));
        *Yw1_view<be<u32>>(gabi::ea(&mHomePos.x)) = *Yw1_view<be<u32>>(gabi::ea(&current.pos.x));
        mbInit = 1;
    }
    daNpc_Yw1_childHIO_c& hio = l_child(mHioNo);
    m_jnt.setParam(hio.mPrm[4], hio.mPrm[5], hio.mPrm[6], hio.mPrm[7], hio.mPrm[0], hio.mPrm[1],
                   hio.mPrm[2], hio.mPrm[3], hio.mPrm[8]);
    if (mbAwaitRescue != 0 && demoActorID == 0) {
        if (!dComIfGs_isEventBit(0x1)) {
            return TRUE;
        }
        if (mType == 1) {
            actor_status = (actor_status & ~0x3Fu) | 0x28;
        }
        mbAwaitRescue = 0;
    }
    partner_search();
    checkOrder();
    if (demo() == 0) {
        s32 staffIdx;
        if (dComIfGp_event_runCheck() && eventInfo.mCommand != 1 /* !checkCommandTalk */ &&
            (staffIdx = isEventEntry()) >= 0) {
            event_proc(staffIdx);
        } else {
            Yw1_callAction(&mAction, this, nullptr);
        }
        fopAcM_posMoveF(this, &mStts.m_cc_move);
        play_animation();
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    eventOrder();
    mAngle.x = current.angle.x;
    mAngle.y = current.angle.y;
    mAngle.z = current.angle.z;
    if (mbKeepShapeAngle == 0) {
        shape_angle.z = current.angle.z;
        shape_angle.y = current.angle.y;
        shape_angle.x = current.angle.x;
    }
    tevStr.mRoomNo = (s8)dBgS_GetRoomId(dComIfG_Bgsp(), gnd_poly(mObjAcch)); /* tevStr.mRoomNo */
    tevStr.mEnvrIdxOverride =
        (u8)dBgS_GetPolyColor(dComIfG_Bgsp(), gnd_poly(mObjAcch)); /* tevStr.mEnvrIdxOverride */
    setMtx(0);
    setCollision(30.0f, 150.0f);
    return TRUE;
}

/* 022FD2D4 */
BOOL daNpc_Yw1_c::_delete() {
    dComIfG_resDelete(&mPhs, "Yw");
    if (heap.get() != nullptr && mpMorf.get() != nullptr) { /* HD: only when the heap was created */
        mpMorf->stopZelAnime();
    }
    return TRUE;
}

/* 022FD19C */
cPhs_State daNpc_Yw1_c::_create() {
    /* fopAcM_SetupActor(this, daNpc_Yw1_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            Yw1_npcCtor(this); /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = YW1_VTBL;
            mDoExt_btpAnm::ct(&mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State phase = dComIfG_resLoad(&mPhs, "Yw");
    if (phase != cPhs_COMPLEATE_e) {
        return phase;
    }
    if (!decideType(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)CheckCreateHeap),
                               a_heap_size_tbl[0] /* a_heap_size_tbl[0] */)) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -40.0f, -20.0f, -40.0f, 40.0f, 210.0f, 40.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return phase;
}

/* 022FB43C */
BOOL daNpc_Yw1_c::bodyCreateHeap() {
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes("Yw", Yw1BodyBdl);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(0x985, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_yw1.cpp", 0x985, "a_mdl_dat != (0)");
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1,
                                    1, nullptr, 0x80000, 0x11020022);
    mDoExt_McaMorf* morf = mpMorf;
    if (morf == nullptr) {
        return FALSE;
    }
    if (morf->getModel() == nullptr) {
        if (morf != nullptr) {
            gabi::call_ptr(Yw1_view<Yw1VirtualObject>(gabi::ea(morf))->mVtable->mWords[3], morf,
                           3); /* delete mpMorf */
        }
        mpMorf = nullptr;
        return FALSE;
    }
    m_head_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("head");
    if (m_head_jnt_num < 0) /* JUT_ASSERT(0x995, m_head_jnt_num >= 0) */
        JUT_ASSERT_fail("d_a_npc_yw1.cpp", 0x995, "m_hed_jnt_num >= 0");
    m_bbone_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("backbone");
    if (m_bbone_jnt_num < 0) /* JUT_ASSERT(0x997, m_bbone_jnt_num >= 0) */
        JUT_ASSERT_fail("d_a_npc_yw1.cpp", 0x997, "m_bbone_jnt_num >= 0");
    Yw1_setJointCallback(Yw1_modelData(mpMorf->getModel()), (u16)(s32)m_head_jnt_num,
                         gabi::guest_fn((const void*)nodeCB_Head));
    Yw1_setJointCallback(Yw1_modelData(mpMorf->getModel()), (u16)(s32)m_bbone_jnt_num,
                         gabi::guest_fn((const void*)nodeCB_BackBone));
    mpMorf->getModel()->setUserArea(gabi::ea(this)); /* setUserArea */
    return TRUE;
}

/* 022FB7BC */
BOOL daNpc_Yw1_c::headCreateHeap() {
    J3DModelData* a_mdl_dat =
        (J3DModelData*)dComIfG_getObjectIDRes("Yw", a_head_resID[0] /* head BDL */);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(0x9BA, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_yw1.cpp", 0x9BA, "a_mdl_dat != (0)");
    mpHedModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x15020022);
    if (mpHedModel.get() == nullptr) {
        return FALSE;
    }
    if (!init_texPttrnAnm(a_initialTexNo[0], 0)) {
        return FALSE;
    }
    m_hair1_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("hair1");
    if (m_hair1_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_yw1.cpp", 0x9CC, "m_hair1 >= 0");
    m_hair2_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("hair2");
    if (m_hair2_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_yw1.cpp", 0x9CE, "m_hair2 >= 0");
    m_hair3_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("hair3");
    if (m_hair3_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_yw1.cpp", 0x9D0, "m_hair3 >= 0");
    Yw1_setJointCallback(Yw1_modelData(mpHedModel), (u16)(s32)m_hair1_jnt_num,
                         gabi::guest_fn((const void*)nodeCB_Hair));
    Yw1_setJointCallback(Yw1_modelData(mpHedModel), (u16)(s32)m_hair2_jnt_num,
                         gabi::guest_fn((const void*)nodeCB_Hair));
    Yw1_setJointCallback(Yw1_modelData(mpHedModel), (u16)(s32)m_hair3_jnt_num,
                         gabi::guest_fn((const void*)nodeCB_Hair));
    mpHedModel->setUserArea(gabi::ea(this)); /* setUserArea */
    return TRUE;
}

/* 022FBA48 */
BOOL daNpc_Yw1_c::CreateHeap() {
    if (!bodyCreateHeap()) {
        return FALSE;
    }
    if (!headCreateHeap()) {
        mpMorf = nullptr;
        return FALSE;
    }
    mAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}

/* 022FD2D0 */
static cPhs_State daNpc_Yw1_Create(fopAc_ac_c* i_this) { return ((daNpc_Yw1_c*)i_this)->_create(); }

/* 022FD32C */
static BOOL daNpc_Yw1_Delete(daNpc_Yw1_c* i_this) { return i_this->_delete(); }

/* 022FDD64 */
static BOOL daNpc_Yw1_Execute(daNpc_Yw1_c* i_this) { return i_this->_execute(); }

/* 022FDEFC */
static BOOL daNpc_Yw1_Draw(daNpc_Yw1_c* i_this) { return i_this->_draw(); }

/* 022FDF00 */
static BOOL daNpc_Yw1_IsDelete(daNpc_Yw1_c*) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.

/* 022FF748 */
// HD: daNpc_Yw1_childHIO_c::daNpc_Yw1_childHIO_c (HD: allocates when this == NULL)
static daNpc_Yw1_childHIO_c* daNpc_Yw1_childHIO_ct(daNpc_Yw1_childHIO_c* p) {
    if (p == nullptr) {
        p = (daNpc_Yw1_childHIO_c*)operator_new(0x38);
        if (p == nullptr)
            return nullptr;
    }
    p->__vtbl = CHILDHIO_VTBL;
    return p;
}

/* 022FF788 */
// HD: daNpc_Yw1_HIO_c::daNpc_Yw1_HIO_c
static daNpc_Yw1_HIO_c* daNpc_Yw1_HIO_ct(daNpc_Yw1_HIO_c* p) {
    if (p == nullptr) {
        p = (daNpc_Yw1_HIO_c*)operator_new(0x44);
        if (p == nullptr)
            return nullptr;
    }
    p->__vtbl = HIO_VTBL;
    Yw1_constructArray(gabi::ea(p->mChild), 1, sizeof(daNpc_Yw1_childHIO_c),
                       gabi::guest_fn((const void*)daNpc_Yw1_childHIO_ct));
    p->mChild[0].mChildIndex = 0;
    Yw1_copyBytes(gabi::ea(&p->mChild[0].mPrm), gabi::ea(&a_prm_tbl), sizeof(a_prm_tbl));
    p->mSelectedChild = -1;
    p->mNo = -1;
    return p;
}

/* 022FF814 */
static void __sinit_d_a_npc_yw1_cpp() {
    for (int i = 0; i < 4; i++)
        l_headerZero[i] = 0;
    __register_global_object(0x101C6F5C);
    // HD: initialize the positive limit before the negative one.
    l_headerMax = 3.1415927f;
    l_headerMin = -3.1415927f;
    Yw1_headerByteCtor0(&l_headerByte0);
    __register_global_object(0x101C6F68);
    Yw1_headerByteCtor1(&l_headerByte1);
    __register_global_object(0x101C6F74);
    daNpc_Yw1_HIO_ct(&l_HIO); /* static daNpc_Yw1_HIO_c l_HIO */
    /* dynamically initialised statics: cXyz (0, 1, 0) (upLift), hair joint offsets (setHairAngle) */
    l_hairTip.x = 15.0f;
    l_liftUp.x = 0.0f;
    l_liftUp.y = 1.0f;
    l_liftUp.z = 0.0f;
    l_hairRoot.x = 15.0f;
    l_hairTip.y = 10.0f;
}

/* 022FF8F8 */
// HD: sead::SafeString deleting destructor (this TU's copy)
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 022FF90C */
// HD: daNpc_Yw1_c deleting destructor (compiler-generated, HD virtual destructor)
static void daNpc_Yw1_dt(daNpc_Yw1_c* p, s32 flags) {
    if (p != nullptr) {
        __dt(static_cast<fopNpc_npc_c*>(p));
        if (flags & 1)
            operator_delete(p);
    }
}

/* 022FF9A8 */
// HD: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty)
static void SafeString_assureTerminationImpl(void* p) {}

/* WWHD glue
data a_btp_resID_tbl 100237F4
data a_head_resID 101C6DCC
data a_initialTexNo 101C6DD0
data a_heap_size_tbl 101C6DD4
data a_bck_resID_tbl 100238E4
data a_anm_num_prm 101C6DDC
data a_anm_stt_prm 101C6E4C
data a_anm_atr_prm 101C6EBC
data l_waitAction1 100236C8
data l_waitAction2 100236D0
data a_prm_tbl 101C6F2C
data cut_name_tbl 101C6DD8
str 1002378C "Yw" in 022FB43C
str 10023798 "d_a_npc_yw1.cpp" in 022FB43C
str 100237A8 "a_mdl_dat != (0)" in 022FB43C
str 10023790 "head" in 022FB43C
str 100237BC "m_hed_jnt_num >= 0" in 022FB43C
str 100237D0 "backbone" in 022FB43C
str 100237DC "m_bbone_jnt_num >= 0" in 022FB43C
str 100237F8 "Yw" in 022FB6CC
str 10023814 "Yw" in 022FB7BC
str 10023818 "d_a_npc_yw1.cpp" in 022FB7BC
str 10023828 "a_mdl_dat != (0)" in 022FB7BC
str 100237FC "hair1" in 022FB7BC
str 1002383C "m_hair1 >= 0" in 022FB7BC
str 10023804 "hair2" in 022FB7BC
str 1002384C "m_hair2 >= 0" in 022FB7BC
str 1002380C "hair3" in 022FB7BC
str 1002385C "m_hair3 >= 0" in 022FB7BC
str 100238B8 "Yw1" in 022FCF9C
str 100238CC "Yw" in 022FD19C
str 100238CF "Yw" in 022FD2D4
str 10023770 "d_demo.h" in 022FD488
str 10023748 "m_object != (0)" in 022FD488
str 100238D2 "Yw" in 022FD488
str 10023900 "Yw" in 022FDF1C
ext 028EAB2C void Yw1_headerByteCtor1(be<u8>* p)
ext 028ED6F8 void Yw1_headerByteCtor0(be<u8>* p)
ext 025A1458 void Yw1_npcCtor(fopNpc_npc_c* p)
ext 028EFFD0 void Yw1_constructArray(u32 p, u32 n, u32 size, u32 ctor)
ext 025D9D0C void Yw1_setCarryNow(fopAc_ac_c* a, BOOL stageLayer)
ext 025E535C BOOL McaMorf_play(mDoExt_McaMorf* m, cXyz* pos, u32 se, s32 reverb)
ext 024EECAC u32 Yw1_groundSound(dBgS* bgs, void* poly)
ext 025E789C BOOL mDoExt_btpAnm_init(void* a, u32 d, J3DAnmTexPattern* btp, s32 anmPlay, s32 mode, f32 rate, s16 start, s16 end, s32 modify, s32 entry)
ext 021E1E78 s32 cLib_getRndValue(s32 min, s32 range)
ext 024EF130 s32 dBgS_GetRoomId(dBgS* bgs, void* poly)
ext 024EEEB8 s32 dBgS_GetPolyColor(dBgS* bgs, void* poly)
ext 0259DED0 void lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* dst, cXyz* eye, s16 defY, s16 maxVel, u8 headOnly)
ext 0259F814 void setActorInfo2(dNpc_EventCut_c* c, const char* name, fopAc_ac_c* a)
ext 02526E70 void* dDemo_object_getActor(u32 obj, u8 id)
ext 02527828 J3DAnmTexPattern* dDemo_actor_getP_BtpData(void* ac, const char* arc)
ext 02527028 BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* m, const char* arc, s32 n, u32 p, u32 q, s8 r)
ext 024EC8D0 fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i)
ext 024EE464 fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i)
ext 025D54C4 BOOL fopAcM_SearchByID2(fpc_ProcID id, be<u32>* out)
ext 025F7DB0 u32 fopMsgM_messageSet(u32 mng, u32 msgNo, cXyz* pos)
ext C000A848 void Yw1_copyBytes(u32 dst, u32 src, u32 n)
ext 02055B64 s16 cLib_calcTimer(be<s16>* t)
ext 024EDFCC BOOL dAttention_LockonTruth(dAttention_c* a)
ext 025F795C u32 fopMsgM_SearchByID(u32 mng)
*/
