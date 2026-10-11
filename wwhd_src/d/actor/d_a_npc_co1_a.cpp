/**
 * d_a_npc_co1_a.cpp (WWHD)
 * NPC - Prince Komali (before Dragon Roost Cavern): part A (heap, animation, create, matrices)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_co1.cpp) has only "Nonmatching" stubs for this actor, so the functions are
 * written from the WWHD code, verified against cking.rpx.
 */
#include "d/actor/d_a_npc_co1.h"

#define SAFESTRING_VTBL 0x10019380 /* this TU's sead::SafeString vtable */
#define CO1_VTBL 0x10019628        /* daNpc_Co1_c vtable (HD: merged with fopNpc_npc_c's) */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* resources by id (HD: sead::SafeString key, per-TU vtable) */
static inline void* co1_dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->__vtbl = SAFESTRING_VTBL;
    key->mStringTop = gabi::guest_addr(arc);
    return co1_getIDResource(dComIfG_resControl(), key.get(), id);
}
/* J3DModel (HD): model data at +0xAC */
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return m->getModelData(); }
/* J3DModelData (HD): 027F68FC returns the joint name table header (self-relative offset at
 * +0x10 to the JUTNameTab), 027F3F94 (the matcher calls it __nw) the joint tree header (joint
 * count u16 at +8) */
static inline u32 co1_J3DModelData_getJointName(J3DModelData* data) {
    return gabi::ea(data->getJointName());
}
static inline s8 co1_JUTNameTab_getIndex(u32 tab, const char* name) { return co1_findJointName(tab, name); }
static inline u16 co1_J3DModelData_getJointNum(J3DModelData* data) { return data->getJointNum(); }
/* 025E789C mDoExt_btpAnm::init(modelData, anm, anmPlay, attr, rate, start, end, modify, entry) */
static inline s32 co1_mDoExt_btpAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start,
                                     s32 end, u32 modify, s32 entry) {
    return co1_initBtp(anm, d, p, play, attr, rate, start, end, modify, entry);
}
/* 025E7CE0 mDoExt_btkAnm::init(modelData, key, anmPlay, attr, rate, start, end, modify, entry) */
static inline s32 co1_mDoExt_btkAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start,
                                     s32 end, u32 modify, s32 entry) {
    return co1_initBtk(anm, d, p, play, attr, rate, start, end, modify, entry);
}
/* delete a McaMorf (HD: virtual deleting destructor, vtable at +0, slot +0xC) */
static inline void co1_McaMorf_delete(mDoExt_McaMorf* morf) {
    if (morf != nullptr) {
        u32 vt = gabi::load<u32>(gabi::ea(morf));
        gabi::call_ptr(gabi::load<u32>(vt + 0xC), morf, 3);
    }
}

/* HD J3D: j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at 0x104B4868; a model's joint
 * matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices) */
static Mtx34* getAnmMtx(J3DModel* model, s32 jntNo) {
    return model->getAnmMtx(jntNo);
}

/* J3DAnmTexPattern / J3DAnmTextureSRTKey::getFrameMax: virtual (vtable pointer at +4, slot +0x14) */
static inline s32 co1_J3DAnm_getFrameMax(u32 p) {
    u32 vt = gabi::load<u32>(p + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), gabi::at<u8>(p));
}
/* 02055B64 co1_cLib_calcTimer<s16> (out of line) */
static inline s16 co1_cLib_calcTimer(be<s16>* t) { return co1_decrementTimer(t); }
/* 025B7D90 dSv_player_collect_c::isSymbol (collect at save + 0xD4) */

/* ---- file statics ---- */
/* l_HIO (daNpc_Co1_HIO_c, 0x38 bytes, HD vtable at 0) at 0x10466CB0; fields read here: */
static f32 l_HIO_attOffsetY() { return (f32)l_HIO.mAttentionOffsetY; }

/* pointers to member functions in .data (copied to the stack before set_action) */
static inline void pmf_load(Co1ProcFunc* dst, Co1ActionWords* src) {
    Co1ActionWords* words = reinterpret_cast<Co1ActionWords*>(dst);
    words->mThisAndVirtualIndex = src->mThisAndVirtualIndex;
    words->mFunction = src->mFunction;
}
/* (this->*pmf)(arg) */
static inline void pmf_call(daNpc_Co1_c* self, Co1ProcFunc* pmf, void* arg) {
    (self->*(*pmf))(arg);
}

/* dBgS (play + 0x12A0) */
static inline s8 dBgS_GetRoomId_l(dBgS* bgs, void* poly) { return co1_getRoomId(bgs, poly); }
static inline u8 dBgS_GetPolyColor_l(dBgS* bgs, void* poly) { return co1_getPolyColor(bgs, poly); }

static mDoExt_McaMorf* prlMorf(daNpc_Co1_c* i_this) { return i_this->mPearlMorf.get(); } /* pearl McaMorf */

/* 022271C0 */
void daNpc_Co1_c::nodeCo1Control(J3DNode* i_node, J3DModel* i_model) {
    /* static cXyz a_eye_pos_off(14.0f, 18.0f, 0.0f): guard 0x10466D04, object 0x10466CF8 */
    cXyz* a_eye_pos_off = &l_eyeOffset;
    if (l_eyeOffsetGuard == 0) {
        a_eye_pos_off->z = 0.0f;
        l_eyeOffsetGuard = 1;
        a_eye_pos_off->x = 14.0f;
        a_eye_pos_off->y = 18.0f;
    }
    J3DJoint* joint = J3DNode_toJoint(i_node);
    u32 jointIdx = (u16)reinterpret_cast<Co1JointResource*>(joint)->mJointNo;
    PSMTXCopy(getAnmMtx(i_model, jointIdx), mDoMtx_stack_c::get());
    if (jointIdx == (u32)(s32)mHeadJoint) { /* head */
        mDoMtx_XrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[0][1]);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), -m_jnt.mAngles[0][0]);
        PSMTXMultVec(mDoMtx_stack_c::get(), a_eye_pos_off, &mEyePosition);
    }
    if (jointIdx == (u32)(s32)mBackboneJoint) { /* backbone */
        mDoMtx_XrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[1][1]);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), -m_jnt.mAngles[1][0]);
    }
    PSMTXCopy(mDoMtx_stack_c::get(), J3DSys::mCurrentMtx);
    mtx_copy(getAnmMtx(i_model, jointIdx), mDoMtx_stack_c::get());
}

/* 02227360 */
static BOOL nodeCallBack_Co1(J3DNode* i_node, int i_calcTiming) {
    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys.getModel();
        daNpc_Co1_c* user = gabi::at<daNpc_Co1_c>(model->mUserArea);
        if (user != nullptr) {
            user->nodeCo1Control(i_node, model);
        }
    }
    return TRUE;
}

/* 022273A8 */
J3DModelData* daNpc_Co1_c::create_Anm() {
    J3DModelData* a_mdl_dat = (J3DModelData*)co1_dComIfG_getObjectIDRes("Co", dRes_ID_CO_BDL_CO_e);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2098, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_co1.cpp", 0x832, "a_mdl_dat != (0)");
    J3DAnmTransform* anm = (J3DAnmTransform*)co1_dComIfG_getObjectIDRes("Co", dRes_ID_CO_BCK_CO_WAIT01_e);
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, anm, 2 /* J3DFrameCtrl::EMode_LOOP */, 1.0f, 0, -1, 1,
                                    nullptr, 0x80000, 0x11020022);
    if (mpMorf.get() == nullptr) {
        return nullptr;
    }
    if (mpMorf->getModel() == nullptr) {
        co1_McaMorf_delete(mpMorf);
        mpMorf = nullptr;
        return nullptr;
    }
    mHeadJoint = co1_JUTNameTab_getIndex(co1_J3DModelData_getJointName(a_mdl_dat), "head");
    if (mHeadJoint < 0) /* JUT_ASSERT(2120, m_hed_jnt_num >= 0) */
        JUT_ASSERT_fail("d_a_npc_co1.cpp", 0x848, "m_hed_jnt_num >= 0");
    mBackboneJoint = co1_JUTNameTab_getIndex(co1_J3DModelData_getJointName(a_mdl_dat), "backbone");
    if (mBackboneJoint < 0)
        JUT_ASSERT_fail("d_a_npc_co1.cpp", 0x84B, "m_bbone_jnt_num >= 0");
    mRightHandJoint = co1_JUTNameTab_getIndex(co1_J3DModelData_getJointName(a_mdl_dat), "handR");
    if (mRightHandJoint < 0)
        JUT_ASSERT_fail("d_a_npc_co1.cpp", 0x84E, "m_hnd_R_jnt_num >= 0");
    return a_mdl_dat;
}

/* 022275E0 */
u32 daNpc_Co1_c::btpNum_toResID(int i_btpNum) {
    /* a_btp_resID_tbl (.data 0x1001947C) */
    return l_btpResID[i_btpNum];
}

/* 022275F4 */
/* i_modify (GameCube bool) is passed on unnormalised: typed u32 */
BOOL daNpc_Co1_c::setBtp(u32 i_modify, int i_btpNum) {
    J3DModelData* model_data = J3DModel_getModelData_l(mpMorf->getModel());
    u32 res_id = btpNum_toResID(i_btpNum);
    mHeadTextureAnm = (J3DAnmTexPattern*)co1_dComIfG_getObjectIDRes("Co", res_id);
    if (mHeadTextureAnm.get() == nullptr) /* JUT_ASSERT(465, m_hed_tex_pttrn != NULL) */
        JUT_ASSERT_fail("d_a_npc_co1.cpp", 0x1D1, "m_hed_tex_pttrn != (0)");
    s32 r = co1_mDoExt_btpAnm_init(&mBtpAnm, model_data, mHeadTextureAnm.get(), 1, 2, 1.0f, 0, -1, i_modify, 0);
    bool ok = r == 1;
    if (ok) {
        mBlinkTimer = 0;
        mBlinkFrame = 0;
    }
    return ok;
}

/* 022276E0 */
/* tail call: setBtp's result register is passed through (typed u32) */
u32 daNpc_Co1_c::iniTexPttrnAnm(u32 i_modify) {
    return co1_setBtpResult(this, i_modify, (s32)mBtpNumber); /* setBtp(i_modify, mBtpNum) */
}

/* 022276EC */
J3DModelData* daNpc_Co1_c::create_prl_Anm() {
    J3DModelData* a_mdl_dat = (J3DModelData*)co1_dComIfG_getObjectIDRes("Co", dRes_ID_CO_BDL_CO_PEAL_e);
    if (a_mdl_dat == nullptr)
        JUT_ASSERT_fail("d_a_npc_co1.cpp", 0x868, "a_mdl_dat != (0)");
    J3DAnmTransform* anm = (J3DAnmTransform*)co1_dComIfG_getObjectIDRes("Co", 7);
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, anm, 2, 1.0f, 0, -1, 1, nullptr,
                                                  0x80000, 0x15021222);
    mPearlMorf = morf;
    if (morf == nullptr) {
        return nullptr;
    }
    if (morf->getModel() == nullptr) {
        co1_McaMorf_delete(morf);
        mPearlMorf = nullptr;
        return nullptr;
    }
    mPearlJoint = co1_JUTNameTab_getIndex(co1_J3DModelData_getJointName(a_mdl_dat), "co_pearl");
    if (mPearlJoint < 0)
        JUT_ASSERT_fail("d_a_npc_co1.cpp", 0x882, "m_prl_jnt_num >= 0");
    return a_mdl_dat;
}

/* 02227884 */
BOOL daNpc_Co1_c::setBtk(u32 i_modify) {
    J3DModelData* model_data = J3DModel_getModelData_l(prlMorf(this)->getModel());
    mPearlTextureAnm = (J3DAnmTextureSRTKey*)co1_dComIfG_getObjectIDRes("Co", dRes_ID_CO_BTK_CO_PEAL_e);
    if (mPearlTextureAnm.get() == nullptr)
        JUT_ASSERT_fail("d_a_npc_co1.cpp", 0x1ED, "m_prl_btk != (0)");
    s32 r = co1_mDoExt_btkAnm_init(&mBtkAnm, model_data, mPearlTextureAnm.get(), 1, 2, 1.0f, 0, -1, i_modify, 0);
    bool ok = r == 1;
    if (ok) {
        mPearlBtkFrame = 0;
    }
    return ok;
}

/* 02227964 */
bool daNpc_Co1_c::create_itm_Mdl() {
    mLetterModel = nullptr;
    J3DModelData* a_mdl_dat = (J3DModelData*)co1_dComIfG_getObjectIDRes("Co", dRes_ID_CO_BDL_CO_LETTER_e);
    if (a_mdl_dat == nullptr)
        JUT_ASSERT_fail("d_a_npc_co1.cpp", 0x894, "a_mdl_dat != (0)");
    mLetterModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000002);
    return true;
}

/* 02227A04 */
BOOL daNpc_Co1_c::CreateHeap() {
    J3DModelData* anm_model = create_Anm();
    if (!anm_model) {
        return FALSE;
    }
    mBtpNumber = 6;
    if (!iniTexPttrnAnm(false) || !create_prl_Anm()) {
        mpMorf = nullptr;
        return FALSE;
    }
    if (setBtk(false) && create_itm_Mdl()) {
        for (u16 i = 0; i < co1_J3DModelData_getJointNum(anm_model); i++) {
            if ((i == (u32)(s32)mHeadJoint) || (i == (u32)(s32)mBackboneJoint)) {
                /* mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack_Co1) */
                J3DModelData* md = J3DModel_getModelData_l(mpMorf->getModel());
                u32 n = (u32)md->mJointNum;
                u32 joint = (u32)md->mJointNodes;
                if (i < n)
                    joint += i * 0x1C;
                gabi::at<J3DJoint>(joint)->setCallBack(nodeCallBack_Co1);
            }
        }
        mpMorf->getModel()->mUserArea = gabi::ea(this); /* setUserArea(this) */
        mAcchCir.SetWall(30.0f, 40.0f);
        mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
        return TRUE;
    }
    mpMorf = nullptr;
    mPearlMorf = nullptr;
    return FALSE;
}

/* 02227BB8 */
/* tail call: CreateHeap's result register is passed through */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return static_cast<daNpc_Co1_c*>(i_this)->CreateHeap();
}

/* 02227BBC daNpc_Co1_c::daNpc_Co1_c (fopAcM_ct's constructor, out of line in HD; allocates when
 * this == NULL) */
/* 02227BBC */
static daNpc_Co1_c* daNpc_Co1_c_ct(daNpc_Co1_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Co1_c*)operator_new(0xA38);
        if (i_this == nullptr)
            return i_this;
    }
    co1_constructNpc(i_this); /* fopNpc_npc_c::fopNpc_npc_c (the matcher calls it cDyl_LinkASync) */
    i_this->__vtbl = CO1_VTBL;
    co1_constructBtk(&i_this->mBtkAnm); /* mDoExt_btkAnm::mDoExt_btkAnm */
    co1_constructBtp(&i_this->mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
    co1_constructEventCut(&i_this->mEventCut); /* dNpc_EventCut_c::dNpc_EventCut_c */
    i_this->mPearlLight.mHD20 = 1.0f;
    return i_this;
}

/* 02227C34 */
bool daNpc_Co1_c::charDecide(int) {
    mCharacterType = 0;
    mCharacterSubtype = 0;
    return true;
}

/* 02227C48 */
BOOL daNpc_Co1_c::set_action(Co1ProcFunc* i_newProcFunc, void* i_argsP) {
    Co1ProcFunc* cur = &mCurrProcFunc;
    s16 newI = i_newProcFunc->i;
    s16 newD;
    u32 newF;
    bool finishCurrent = true;
    if ((u16)cur->i == (u16)newI) {
        if (cur->i == 0)
            return TRUE;
        newD = i_newProcFunc->d;
        newF = i_newProcFunc->f;
        if ((u16)cur->d == (u16)newD && cur->f == newF)
            return TRUE;
    } else {
        newF = i_newProcFunc->f;
        newD = i_newProcFunc->d;
        finishCurrent = cur->i != 0;
    }
    if (finishCurrent) {
        mActionPhase = 9;
        pmf_call(this, cur, i_argsP);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    mActionPhase = 0;
    pmf_call(this, cur, i_argsP);
    return TRUE;
}

/* 02227D74 */
bool daNpc_Co1_c::init_CO1_0() {
    bool ret = !dComIfGs_isSymbol(1); /* Din's Pearl not yet obtained */
    if (ret) {
        actor_status = (actor_status & ~0x80u) | 0x4000; /* OffStatus(NOCULLEXEC), OnStatus(0x4000) */
        gabi::Local<Co1ProcFunc> pmf;
        pmf_load(pmf, &l_waitAction);
        set_action(pmf, nullptr);
    }
    return ret;
}

/* 02227DFC */
void daNpc_Co1_c::plyTexPttrnAnm() {
    /* the pearl's btk loops */
    u8 btkFrame = (u8)(mPearlBtkFrame + 1);
    mPearlBtkFrame = btkFrame;
    s32 btkMax = co1_J3DAnm_getFrameMax(gabi::ea(mPearlTextureAnm.get()));
    s8 btpNum = mBtpNumber;
    if ((s32)btkFrame >= btkMax) {
        mPearlBtkFrame = 0;
    }
    if (btpNum == 6 || btpNum == 5 || !co1_cLib_calcTimer(&mBlinkTimer)) {
        u8 frame = (u8)(mBlinkFrame + 1);
        mBlinkFrame = frame;
        if ((s32)frame >= co1_J3DAnm_getFrameMax(gabi::ea(mHeadTextureAnm.get()))) {
            if (mBtpNumber == 6 || mBtpNumber == 5) {
                mBlinkFrame = (u8)co1_J3DAnm_getFrameMax(gabi::ea(mHeadTextureAnm.get()));
            } else {
                s16 t = (s16)gabi::ftoi(cM_rndF(60.0f) + 30.0f);
                mBlinkFrame = 0;
                mBlinkTimer = t;
            }
        }
    }
}

/* 02227F1C */
void daNpc_Co1_c::setAttention(u32 i_setEyePos) {
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    cXyz* attPos = &attention_info.position; /* attention_info.position */
    if (mBckNumber == 1) {
        mDoMtx_stack_c::transS(x, y, z);
        mDoMtx_stack_c::YrotM(current.angle.y);
        gabi::Local<cXyz> off;
        off->x = 0.0f;
        off->y = 60.0f;
        off->z = -50.0f;
        PSMTXMultVec(mDoMtx_stack_c::get(), off, attPos);
    } else {
        attPos->z = z;
        attPos->x = x;
        attPos->y = y + l_HIO_attOffsetY();
    }
    if (mEyePositionResult == 0 && !i_setEyePos) {
        return;
    }
    eyePos.z = mEyePosition.z;
    eyePos.y = mEyePosition.y;
    eyePos.x = mEyePosition.x;
}

/* 02228014 */
void daNpc_Co1_c::setMtx(u32 param_1) {
    if (mInDemo == 0) {
        plyTexPttrnAnm();
        mAnimationStopped = (s8)mpMorf->play(&eyePos, 0, 0);
        prlMorf(this)->mFrameCtrl.mFrame = (f32)(s16)gabi::ftoi(mpMorf->getFrame()); /* the pearl follows the body */
        if (mpMorf->getFrame() < mPreviousAnmFrame) {
            mAnimationStopped = 1;
        }
        mPreviousAnmFrame = mpMorf->getFrame();
        mObjAcch.CrrPos(dComIfG_Bgsp());
        switch ((u32)(s32)mBckNumber) {
        case 4:
            mShowLetterModel = 1;
            break;
        case 8:
            mShowLetterModel = !(mpMorf->getFrame() < 15.0f);
            break;
        default:
            mShowLetterModel = 0;
            break;
        }
    }
    /* the ground polygon: cBgS_PolyInfo at m_gnd + 0x14 */
    void* gndPoly = static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd);
    tevStr.mRoomNo = dBgS_GetRoomId_l(dComIfG_Bgsp(), gndPoly);
    u8 color = dBgS_GetPolyColor_l(dComIfG_Bgsp(), gndPoly);
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    tevStr.mEnvrIdxOverride = color; /* tevStr.mEnvrIdxOverride */
    mDoMtx_stack_c::transS(x, y, z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();

    /* the pearl hangs from the backbone */
    PSMTXCopy(getAnmMtx(mpMorf->getModel(), mBackboneJoint), mDoMtx_stack_c::get());
    J3DModel_setBaseTRMtx(prlMorf(this)->getModel(), mDoMtx_stack_c::get());
    prlMorf(this)->calc();
    if (mShowLetterModel != 0) { /* item in the right hand */
        PSMTXCopy(getAnmMtx(mpMorf->getModel(), mRightHandJoint), mDoMtx_stack_c::get());
        J3DModel* itm = mLetterModel.get();
        J3DModel_setBaseTRMtx(itm, mDoMtx_stack_c::get());
        J3DModel_calc(mLetterModel.get());
    }
    PSMTXCopy(getAnmMtx(prlMorf(this)->getModel(), mPearlJoint), mDoMtx_stack_c::get());
    Mtx34* m = mDoMtx_stack_c::get();
    f32 px = m->m[0][3];
    mPearlPositionX = px;
    f32 py = m->m[1][3];
    mPearlPositionY = py;
    f32 pz = m->m[2][3];
    mPearlLight.mPos.x = px;
    mPearlPositionZ = pz;
    mPearlLight.mPos.y = py;
    mPearlLight.mPos.z = pz;
    // HD: the pearl point light pulses between the HIO power limits.
    mPearlLight.mColorR = l_HIO.mGlowColorR;
    mPearlLight.mColorG = l_HIO.mGlowColorG;
    mPearlLight.mColorB = l_HIO.mGlowColorB;
    s16 ang = (s16)(mGlowPulseAngle + l_HIO.mGlowAngleStep);
    mGlowPulseAngle = ang;
    f32 s = std::fabs(cM_ssin(ang));
    mGlowPulseSin = s;
    f32 lo = l_HIO.mGlowMinPower;
    f32 d = l_HIO.mGlowMaxPower - lo;
    f32 range = d >= 0.0f ? d : 0.0f;
    f32 v = gabi::fmadds(range, s, lo);
    mPearlLight.mPower = (v - 0.001f) >= 0.0f ? v : 0.001f;
    setAttention(param_1);
}

/* 0222843C */
bool daNpc_Co1_c::createInit() {
    /* l_evn_tbl (.data 0x101BDC50), 3 names */
    for (int i = 0; i < 3; i++) {
        const char* name = l_eventNames[i].get();
        mEventIdx[i] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    }
    mLookTarget.copy(current.pos);
    mHeadOnlyFollow = 1;
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0x45; /* attention_info.distances[SPEAK] */
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0x46; /* attention_info.distances[TALK] */
    attention_info.flags = fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e; /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    gravity = -4.5f;
    mEventCut.setActorInfo2("Co1", (fopNpc_npc_c*)(void*)this);
    mBckNumber = 0xB;
    bool init_success;
    switch (mCharacterType) {
    case 0:
        init_success = init_CO1_0();
        break;
    default:
        init_success = false;
        break;
    }
    if (!init_success) {
        return false;
    }
    shape_angle.z = current.angle.z;
    shape_angle.y = current.angle.y;
    shape_angle.x = current.angle.x;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(&co1NpcCylinderSource());
    mpMorf->setMorf(0.0f);
    setMtx(true);
    return true;
}

/* 02228598 */
cPhs_State daNpc_Co1_c::_create() {
    /* fopAcM_ct(this, daNpc_Co1_c): HD out-of-line constructor */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            daNpc_Co1_c_ct(this);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, "Co");
    if (state != cPhs_COMPLEATE_e) {
        return state;
    }
    if (!charDecide(gabi::load<u8>(gabi::ea(this) + 0xB3) /* fopAcM_GetParam(this) & 0xFF */)) {
        return cPhs_ERROR_e;
    }
    /* static int a_size_tbl[] (.data 0x101BDC5C): HD reads [0] */
    if (!fopAcM_entrySolidHeap(this, 0x02227BB8 /* CheckCreateHeap */, l_heapSize[0])) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -40.0f, -20.0f, -100.0f, 40.0f, 100.0f, 60.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}

/* 022286C0 */
static cPhs_State daNpc_Co1_Create(fopAc_ac_c* i_this) {
    return ((daNpc_Co1_c*)i_this)->_create();
}

/* 022286C4 */
BOOL daNpc_Co1_c::_delete() {
    dComIfG_resDelete(&mPhs, "Co");
    dKy_plight_cut(&mPearlLight); /* HD: the pearl's point light */
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}

/* 02228724 */
static BOOL daNpc_Co1_Delete(daNpc_Co1_c* i_this) {
    return i_this->_delete();
}

/* 02228728 */
void daNpc_Co1_c::checkOrder() {
    u16 command = eventInfo.mCommand; /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt */) {
        if (dComIfGp_evmng_startCheck(mEventIdx[mActiveEventIdx])) {
            switch ((u32)(s32)mActiveEventIdx) {
            case 1:
                mEventOrderKind = 0;
                actor_status &= ~0x4000u;
                break;
            case 2:
                mEventOrderKind = 0;
                mLookMode = 0;
                break;
            default:
                mEventOrderKind = 0;
                break;
            }
        }
    } else if (command == 1 /* checkCommandTalk */) {
        if (mEventOrderKind == 1 || mEventOrderKind == 2) {
            mEventOrderKind = 0;
            mTalkRequested = 1;
        }
    }
}

/* 02228848 */
/* returns the in-demo byte as is (typed u8) */
u8 daNpc_Co1_c::demo() {
    if (demoActorID == 0) {
        if (mInDemo != 0) {
            mInDemo = 0;
            return 0;
        }
        return mInDemo;
    }
    u8 id = demoActorID;
    mInDemo = 1;
    /* dComIfGp_demo_getActor(demoActorID): HD inline with a range check and the demo object
     * (0x101D5FFC) asserted */
    void* demo_actor = nullptr;
    if (id != 0 && id <= 0x20) {
        u32 obj = gabi::load<u32>(0x101D5FFC);
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
            obj = gabi::load<u32>(0x101D5FFC);
        }
        demo_actor = co1_getDemoActor(obj, id); /* dDemo_object_c::getActor */
    }
    if (mHeadTextureAnm.get() != nullptr) {
        u8 frame = (u8)(mBlinkFrame + 1);
        mBlinkFrame = frame;
        if ((s32)frame >= co1_J3DAnm_getFrameMax(gabi::ea(mHeadTextureAnm.get()))) {
            mBlinkFrame = (u8)co1_J3DAnm_getFrameMax(gabi::ea(mHeadTextureAnm.get()));
        }
    }
    /* HD: demo_actor is checked for NULL */
    if (demo_actor != nullptr) {
        u32 demopattern = co1_getDemoBtp(demo_actor, "Co"); /* getP_BtpData */
        if (demopattern != 0) {
            mHeadTextureAnm = gabi::at<J3DAnmTexPattern>(demopattern);
            J3DModelData* md = J3DModel_getModelData_l(mpMorf->getModel());
            if (co1_mDoExt_btpAnm_init(&mBtpAnm, md, gabi::at<u8>(demopattern), 1, 2, 1.0f, 0, -1, 1, 0)) {
                mBlinkFrame = 0;
                mBtpNumber = 7;
            }
        }
    }
    co1_setDemoData(this, 0x6A, mpMorf.get(), "Co", 0, 0, 0, 0); /* dDemo_setDemoData */
    return mInDemo;
}

/* 022289FC */
s32 daNpc_Co1_c::isEventEntry() {
    const char* name = gabi::at<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}

/* 02228A3C */
u32 daNpc_Co1_c::setAnm_tex(s8 i_btpNum) {
    if (mBtpNumber != i_btpNum) {
        mBtpNumber = i_btpNum;
        return iniTexPttrnAnm(true);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}

/* 02228A5C */
u32 daNpc_Co1_c::anmNum_toResID(int i_anmNum) {
    /* a_bck_resID_tbl (.data 0x100195A4) */
    return l_bckResID[i_anmNum];
}

/* 02228A70 */
u32 daNpc_Co1_c::anmNum_toResID_prl(int i_anmNum) {
    /* the pearl's bck table (.data 0x100195D0) */
    return l_pearlBckResID[i_anmNum];
}

/* 02228A84 */
BOOL daNpc_Co1_c::setAnm_anm(anm_prm_c* i_anmPrmP) {
    s8 anmNum = i_anmPrmP->mAnmNum;
    if (mBckNumber == anmNum) {
        return TRUE;
    }
    mBckNumber = anmNum;
    u32 resID = anmNum_toResID(anmNum);
    {
        mDoExt_McaMorf* morf = mpMorf;
        f32 morfF = i_anmPrmP->mMorf;
        f32 speed = i_anmPrmP->mSpeed;
        s32 loopMode = i_anmPrmP->mLoopMode;
        dNpc_setAnmIDRes(morf, loopMode, morfF, speed, resID, -1, "Co");
    }
    u32 prlID = anmNum_toResID_prl(mBckNumber);
    {
        mDoExt_McaMorf* morf = prlMorf(this);
        s32 loopMode = i_anmPrmP->mLoopMode;
        dNpc_setAnmIDRes(morf, loopMode, 0.0f, 0.0f, prlID, -1, "Co");
    }
    mPreviousAnmFrame = 0.0f;
    mAnimationStopped = 0;
    mAnimationFlag = 0;
    return TRUE;
}

/* 02228B70 */
bool daNpc_Co1_c::setAnm() {
    anm_prm_c* a_anm_prm_tbl = l_stateAnmPrm;
    if (a_anm_prm_tbl[mState].mBtpNum >= 0) {
        setAnm_tex(a_anm_prm_tbl[mState].mBtpNum);
    }
    if (a_anm_prm_tbl[mState].mAnmNum >= 0) {
        setAnm_anm(&a_anm_prm_tbl[mState]);
    }
    return true;
}

/* 02228BF4 */
void daNpc_Co1_c::setStt(s8 i_status) {
    mLookTimer = 0;
    mState = i_status;
    switch ((u32)(s32)i_status) {
    case 2:
        mAnimationAttribute = 0xFF;
        mLookMode = 1;
        break;
    case 7:
        mLookMode = 0;
        break;
    }
    setAnm();
}

/* 02228C38 */
void daNpc_Co1_c::endEvent() {
    dComIfGp_event_reset();
    mAnimationAttribute = 0xFF;
}

/* 02228C78 */
void daNpc_Co1_c::setAnm_NUM(int i_anmNum, int i_setTex) {
    anm_prm_c* a_anm_prm_tbl = l_numberAnmPrm;
    if (i_setTex != 0) {
        setAnm_tex(a_anm_prm_tbl[i_anmNum].mBtpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[i_anmNum]);
}

/* WWHD glue (tools/verify/autoglue.py)
str 100193C8 "m_object != (0)" in 02228848
str 100193D8 "d_demo.h" in 02228848
str 100193F4 "handR" in 022273A8
str 100193FC "Co" in 022273A8
str 10019400 "head" in 022273A8
str 10019408 "d_a_npc_co1.cpp" in 022273A8
str 10019418 "a_mdl_dat != (0)" in 022273A8
str 1001942C "m_hed_jnt_num >= 0" in 022273A8
str 10019440 "backbone" in 022273A8
str 1001944C "m_bbone_jnt_num >= 0" in 022273A8
str 10019464 "m_hnd_R_jnt_num >= 0" in 022273A8
str 10019498 "Co" in 022275F4
str 1001949C "d_a_npc_co1.cpp" in 022275F4
str 100194AC "m_hed_tex_pttrn != (0)" in 022275F4
str 100194C4 "Co" in 022276EC
str 100194C8 "d_a_npc_co1.cpp" in 022276EC
str 100194D8 "a_mdl_dat != (0)" in 022276EC
str 100194EC "co_pearl" in 022276EC
str 100194F8 "m_prl_jnt_num >= 0" in 022276EC
str 1001950C "Co" in 02227884
str 10019510 "d_a_npc_co1.cpp" in 02227884
str 10019520 "m_prl_btk != (0)" in 02227884
str 10019534 "Co" in 02227964
str 10019538 "d_a_npc_co1.cpp" in 02227964
str 10019548 "a_mdl_dat != (0)" in 02227964
str 10019584 "Co1" in 0222843C
str 10019598 "Co" in 02228598
str 1001959B "Co" in 022286C4
str 1001959E "Co" in 02228848
str 100195FC "Co" in 02228A84
*/
