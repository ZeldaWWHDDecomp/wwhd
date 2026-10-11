/**
 * d_a_npc_zl1_demo.cpp (WWHD)
 * NPC - Tetra: demo, _execute, lookBack, event cuts, darkness (part of d_a_npc_zl1)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_zl1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_zl1_demo.h"

using namespace Zl1Demo;

const gc_tu_data gc_tu = {.safeString = ZL1_SAFESTRING_VTBL, .safeStringAssure = 0x0230A308};

static daNpc_Zl1_HIO_c l_demoHIO;
static CutTable a_cut_tbl;
static TargetOffsets a_tgt_offst;
static DemoObjectPointer sDemoObject;
static u32 sDemoFrame;
static StageName sStartStage;

// HD: SafeString dispatches twice on the literal and once on the current stage name.
static inline void SafeString_assureTermination(SafeString* s) {
    auto* vtable = gabi::at<SafeStringVtable>((u32)s->__vtbl);
    gabi::call_ptr(vtable->assureTermination, s);
}
static bool isStartStage(const char* name) {
    gabi::Local<SafeString> a;
    a->mStringTop = gabi::guest_addr(name);
    a->__vtbl = ZL1_SAFESTRING_VTBL;
    u32 stage = gabi::ea(&reinterpret_cast<PlayStageView*>(dComIfGp_get())->mStartStage);
    gabi::Local<SafeString> b;
    b->__vtbl = ZL1_SAFESTRING_VTBL;
    b->mStringTop = stage;
    SafeString_assureTermination(a);
    SafeString_assureTermination(a);
    u32 pa = a->mStringTop;
    SafeString_assureTermination(b);
    u32 pb = b->mStringTop;
    if (pa == pb)
        return true;
    for (u32 i = 0; i < SAFE_STRING_MAX_LENGTH; i++) {
        u8 ca = gabi::at<be<u8>>(pa)[i];
        u8 cb = gabi::at<be<u8>>(pb)[i];
        if (ca != cb)
            return false;
        if (ca == 0)
            return true;
    }
    return false;
}

// HD: this comparison calls the TU assurance copy before virtual dispatch.
static bool SafeString_equal(u32 lit, u32 other) {
    gabi::Local<SafeString> b;
    b->__vtbl = ZL1_SAFESTRING_VTBL;
    b->mStringTop = other;
    gabi::Local<SafeString> a;
    a->mStringTop = lit;
    a->__vtbl = ZL1_SAFESTRING_VTBL;
    Zl1Demo_assureTerminationImpl(a.get()); /* sead::SafeString::assureTerminationImpl_ (empty) */
    SafeString_assureTermination(a);
    u32 pa = a->mStringTop;
    SafeString_assureTermination(b);
    u32 pb = b->mStringTop;
    if (pa == pb)
        return true;
    for (u32 i = 0; i < SAFE_STRING_MAX_LENGTH; i++) {
        u8 ca = gabi::at<be<u8>>(pa)[i];
        u8 cb = gabi::at<be<u8>>(pb)[i];
        if (ca != cb)
            return false;
        if (ca == 0)
            return true;
    }
    return false;
}

/* 0230634C searchByID(fpc_ProcID, BOOL*) (the matcher calls it cLib_getRndValue<int> of
 * d_a_npc_aj1; not inlined in HD, as in d_a_npc_ls1) */
static fopAc_ac_c* zl1_searchByID(daNpc_Zl1_c* i_this, fpc_ProcID i_PID, be<s32>* o_wasDeleted) {
    WWHD_FUNC(0x0230634C, fopAc_ac_c*, i_this, i_PID, o_wasDeleted);
    gabi::Local<gptr<fopAc_ac_c>> actor_p;
    *actor_p = nullptr;
    *o_wasDeleted = 0;
    if (!Zl1Demo_SearchByID(i_PID, actor_p.get())) { /* fopAcM_SearchByID(id, &actor) */
        *o_wasDeleted = 1;
    }
    return *actor_p;
}
// HD: retain the unregistered template copy; the original specs register its companion copy.

/* 023075B0 */
void daNpc_Zl1_c::lookBack() {
    gabi::Local<cXyz> temp3;
    gabi::Local<cXyz> temp; /* passed by value: a copy */
    s16 head_y = m_jnt.mAngles[0][1];
    u8 temp5 = field_0x7D8;
    temp3->x = 0.0f;
    s16 targetY = current.angle.y;
    s16 backbone_y = m_jnt.mAngles[1][1];
    temp3->z = 0.0f;
    temp3->y = 0.0f;
    field_0x794.x = targetY;
    s8 temp2 = field_0x84D;
    f32 srcZ = current.pos.z;
    field_0x794.y = head_y;
    f32 srcX = current.pos.x;
    f32 srcY = eyePos.y;
    field_0x794.z = backbone_y;
    cXyz* temp4 = nullptr;
    if (field_0x849 == 0xD || field_0x849 == 0xE) {
        temp2 = 0;
    }
    switch ((u32)(s32)temp2) {
    case 1: {
        gabi::Local<cXyz> eye;
        Zl1Demo_playerEyePos(eye, -20.0f);
        field_0x7D0 = true;
        field_0x758.copy(*eye);
        temp3->copy(*eye);
        temp4 = temp3;
        break;
    }
    case 2:
        temp3->copy(field_0x758);
        temp4 = temp3;
        break;
    case 3:
        targetY = field_0x7C0;
        field_0x7D0 = false;
        break;
    case 4: {
        gabi::Local<be<s32>> wasDeleted;
        fopAc_ac_c* actor = zl1_searchByID(this, mProcId2, wasDeleted);
        if (actor != nullptr && *wasDeleted == 0) {
            field_0x758.copy(actor->current.pos);
            f32 x = field_0x758.x;
            f32 z = field_0x758.z;
            f32 y = actor->eyePos.y;
            temp3->z = z;
            temp3->y = y;
            field_0x758.y = y;
            temp3->x = x;
            field_0x7D0 = true;
            temp4 = temp3;
        }
        break;
    }
    case 5:
        kyorokyoro();
        temp3->copy(field_0x758);
        field_0x7D0 = true;
        temp4 = temp3;
        break;
    default:
        field_0x7D0 = false;
        break;
    }
    temp->x = srcX;
    temp->z = srcZ;
    temp->y = srcY;
    dNpc_JntCtrl_lookAtTarget_2(&m_jnt, &current.angle.y, temp4, temp, targetY,
                                l_demoHIO.mPrmTbl.field_18, temp5);
    s16 hx = (s16)(m_jnt.mAngles[0][0] / 2);
    field_0x83E = hx;
    s16 hy = (s16)(m_jnt.mAngles[0][1] / 2);
    field_0x842 = hx;
    field_0x840 = hy;
    field_0x83C = hy;
}

/* 02306A40 */
// HD: returns a cXyz through the hidden result pointer (HD: NULL allocates)
void daNpc_Zl1_c::set_LightPos(cXyz* o_pos) {
    /* player's getRightHandPos() at +0x3FC (HD), each access through dComIfGp_getPlayer(0) */
    f32 x = playerHand(dComIfGp_getPlayer(0)).x;
    f32 y = playerHand(dComIfGp_getPlayer(0)).y;
    f32 z = playerHand(dComIfGp_getPlayer(0)).z;
    y = y + 5.66f;
    f32 c = cM_scos(dComIfGp_getPlayer(0)->shape_angle.y);
    f32 s = cM_ssin(dComIfGp_getPlayer(0)->shape_angle.y);
    x = x + gabi::fmadds(c, 10.25f, s * 11.13f);
    f32 s2 = cM_ssin(dComIfGp_getPlayer(0)->shape_angle.y);
    f32 c2 = cM_scos(dComIfGp_getPlayer(0)->shape_angle.y);
    z = z + gabi::fmsubs(c2, 11.13f, s2 * 10.25f);
    if (o_pos == nullptr) {
        o_pos = (cXyz*)operator_new(12);
        if (o_pos == nullptr)
            return;
    }
    o_pos->y = y;
    o_pos->x = x;
    o_pos->z = z;
}

/* 02307A58 */
void daNpc_Zl1_c::darkProc() {
    /* HD: nothing in the stage "Hyrule" */
    if (isStartStage("Hyrule")) {
        return;
    }
    switch ((u32)(s32)field_0x7C2) {
    case 1:
        incEnvironment();
        dKy_set_vrboxcol_ratio(mRatio);
        break;
    case (u32)-1:
        decEnvironment();
        dKy_set_vrboxcol_ratio(mRatio);
        break;
    default:
        mRatio = 1.0f;
        dKy_set_vrboxcol_ratio(1.0f);
        break;
    }
    dKy_set_fogcol_ratio(mRatio);
    f32 temp = (mRatio - 0.15f) * 1.1764705f;
    if (temp > 0.5f) {
        dKy_fog_startendz_set(300.0f, 2000.0f, 2.0f - (temp + temp));
    } else {
        f32 t = temp * 0.85f;
        dKy_set_bgcol_ratio((t + t) + 0.15f);
        dKy_fog_startendz_set(300.0f, gabi::fmadds(3000.0f, temp, 500.0f), 1.0f);
    }
}

/* 02306D48 */
u32 daNpc_Zl1_c::cut_move_OKIRU_2() {
    if (field_0x7C3) {
        setAnm_NUM(0x10, 1);
        mpMorf->setMorf(16.0f);
        return TRUE;
    }
    return FALSE;
}

/* 0230684C */
void daNpc_Zl1_c::cut_init_JMP_OFF(int i_staffIdx) {
    be<f32>* pSpeed = (be<f32>*)dComIfGp_evmng_getMyFloatP(i_staffIdx, "Speed");
    be<f32>* pSpeedY = (be<f32>*)dComIfGp_evmng_getMyFloatP(i_staffIdx, "Spd_y");
    be<f32>* pGrvty = (be<f32>*)dComIfGp_evmng_getMyFloatP(i_staffIdx, "Grvty");
    f32 speedF1 = get_prmFloat(pSpeed, 0.0f);
    f32 speedY = get_prmFloat(pSpeedY, 0.0f);
    f32 grvty = get_prmFloat(pGrvty, 0.0f);
    setAnm_NUM(10, 1);
    field_0x7C8 = false;
    field_0x7CA = false;
    field_0x7CB = true;
    gravity = grvty;
    field_0x7C7 = true;
    speed.y = speedY;
    speedF = speedF1;
    current.angle.y = current.angle.y + 0x4000;
    /* daPy_getPlayerActorClass()->onPlayerNoDraw() */
    static_cast<daPy_py_c*>(dComIfGp_getPlayer(0))->onNoResetFlg0(daPy_py_c::daPyFlg0_NO_DRAW);
    m_jnt.mAngles[1][0] = 0; /* setBackBone_x(0) */
    field_0x7D3 = false;
    m_jnt.mAngles[1][1] = 0;  /* setBackBone_y(0) */
    m_jnt.mbBackBoneLock = 0; /* offBackBoneLock() */
    field_0x84D = 0;
    m_jnt.mAngles[0][1] = 0; /* setHead_y(0) */
    m_jnt.mAngles[0][0] = 0; /* setHead_x(0) */
    field_0x7D8 = false;
}

/* 02306ED4 */
u32 daNpc_Zl1_c::cut_move_JMP_OFF() {
    if (!(speedF < 0.1f)) {
        cLib_chaseF(&speedF, 0.1f, 0.05f);
    }
    return (u8)(move_jmp() ^ 1);
}

/* 02306BD8 */
void daNpc_Zl1_c::cut_init_OMAMORI_ONOFF(int i_staffIdx) {
    be<s32>* pOnOff = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, "OnOff");
    field_0x7C2 = 0;
    if (pOnOff != nullptr) {
        if (*pOnOff == 0) {
            Zl1Demo_seStart(JA_SE_ITM_OMAMORI_TETLA);
            field_0x7C2 = -1;
            mRatio = 1.0f;
            init_Light();
            gabi::Local<cXyz> pos;
            set_LightPos(pos);
            // GHS interleaves the three word copies; retain that order for aliased destinations.
            auto* words = reinterpret_cast<be<u32>*>(pos.get());
            auto* saved = reinterpret_cast<be<u32>*>(&field_0x830);
            auto* light1 = reinterpret_cast<be<u32>*>(&mLightInfluence1.mPos);
            auto* light2 = reinterpret_cast<be<u32>*>(&mLightInfluence2.mPos);
            for (u32 i = 0; i < 3; i++) {
                u32 word = words[i];
                saved[i] = word;
                light1[i] = word;
                light2[i] = word;
            }
        } else {
            field_0x7C2 = 1;
            mLightInfluence1.mPower = 0.0f;
            mRatio = 0.15f;
            mLightInfluence2.mPower = 0.0f;
        }
    }
}

/* 02306CDC */
void daNpc_Zl1_c::cut_init_SURPRISED(int i_staffIdx) {
    Zl1Demo_seStart(JA_SE_ITM_OMAMORI_BLINK);
    dVibration_c* vib = dComIfGp_getVibration();
    gabi::Local<cXyz> pos;
    pos->z = 0.0f;
    pos->y = 1.0f;
    pos->x = 0.0f;
    Zl1Demo_StartShock(vib, 4, 1, pos.get());
}

/* 02306F30 */
void daNpc_Zl1_c::privateCut(int i_staffIdx) {
    if (i_staffIdx == -1) {
        return;
    }
    s8 idx = dComIfGp_evmng_getMyActIdx(i_staffIdx, gabi::ea(&a_cut_tbl), ACT_COUNT, TRUE, 0);
    mActIdx = idx;
    dEvent_manager_c* evmng = dComIfGp_getPEvtManager();
    if (idx == -1) {
        Zl1Demo_cutEnd(evmng, i_staffIdx);
        return;
    }
    if (Zl1Demo_getIsAddvance(evmng, i_staffIdx)) { /* getIsAddvance */
        switch ((u32)(s32)mActIdx) {
        case ACT_LOK_PLYER:
            cut_init_LOK_PLYER(i_staffIdx);
            break;
        case ACT_LOK_PARTNER:
            cut_init_LOK_PARTNER(i_staffIdx);
            break;
        case ACT_CHG_ANM_ATR:
            cut_init_CHG_ANM_ATR(i_staffIdx);
            break;
        case ACT_PLYER_TRN_PARTNER:
            cut_init_PLYER_TRN_PARTNER(i_staffIdx);
            break;
        case ACT_PLYER_TRN_TETRA:
            cut_init_PLYER_TRN_TETRA(i_staffIdx);
            break;
        case ACT_MAJYU_START:
            cut_init_MAJYU_START(i_staffIdx);
            break;
        case ACT_OKIRU:
            cut_init_OKIRU(i_staffIdx);
            break;
        case ACT_OKIRU_2:
            cut_init_OKIRU_2(i_staffIdx);
            break;
        case ACT_DRW_ONOFF:
            cut_init_DRW_ONOFF(i_staffIdx);
            break;
        case ACT_PLYER_DRW_ONOFF:
            cut_init_PLYER_DRW_ONOFF(i_staffIdx);
            break;
        case ACT_JMP_OFF:
            cut_init_JMP_OFF(i_staffIdx);
            break;
        case ACT_OMAMORI_ONOFF:
            cut_init_OMAMORI_ONOFF(i_staffIdx);
            break;
        case ACT_SURPRISED:
            cut_init_SURPRISED(i_staffIdx);
            break;
        }
    }
    /* the other cut_move_* return TRUE (inlined) */
    u32 end;
    switch ((u32)(s32)mActIdx) {
    case ACT_OKIRU:
        end = cut_move_OKIRU();
        break;
    case ACT_OKIRU_2:
        end = cut_move_OKIRU_2();
        break;
    case ACT_JMP_OFF:
        end = cut_move_JMP_OFF();
        break;
    default:
        end = TRUE;
        break;
    }
    if (end) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
    }
}

/* 023071C4 */
void daNpc_Zl1_c::event_proc(int i_staffIdx) {
    if (dComIfGp_evmng_endCheck(mEventIdx[field_0x7A8])) {
        switch ((u32)(s32)field_0x7A8) {
        case 0:
            field_0x7D3 = false;
            dComIfGs_onEventBit(EVENT_0802);
            field_0x738.z = current.angle.z;
            field_0x738.x = current.angle.x;
            field_0x72C.copy(current.pos);
            field_0x738.y = current.angle.y;
            setStt(1);
            break;
        case 1:
            dComIfGs_onEventBit(EVENT_0804);
            field_0x738.z = current.angle.z;
            current.pos.x = -37692.0f;
            current.angle.y = -0x5C72;
            current.pos.z = 8016.0f;
            current.pos.y = 2200.0f;
            field_0x764.y = 2200.0f;
            field_0x738.y = current.angle.y;
            field_0x738.x = current.angle.x;
            field_0x84D = 0;
            field_0x72C.copy(current.pos);
            setStt(8);
            break;
        case 2:
            dComIfGs_onEventBit(EVENT_0801);
            field_0x7C9 = false;
            break;
        case 3:
            dComIfGs_onEventBit(HYRULE_COURTYARD_CUTSCENE); /* HYRULE_COURTYARD_CUTSCENE */
            setStt(3);
            field_0x7CB = false;
            break;
        }
        endEvent();
    } else {
        if (!mEventCut.cutProc()) {
            privateCut(i_staffIdx);
        }
    }
}

/* 02306DB8 */
u8 daNpc_Zl1_c::move_jmp() {
    mDoExt_McaMorf* morf = mpMorf;
    f32 end = (f32)(s32)morf->mFrameCtrl.mEnd - 1.0f;
    if (!(morf->getFrame() < end)) {
        setAnm_NUM(8, 1);
        return false;
    }
    if (!field_0x7C7 && mObjAcch.ChkGroundHit()) {
        morf->setPlaySpeed(1.0f);
        gravity = -4.5f;
        speedF = 0.0f;
        speed.y = 0.0f;
        if (field_0x7C8) {
            return true;
        }
    } else if (field_0x7C8 || !mObjAcch.ChkWaterIn()) {
        return true;
    }
    field_0x744.x = current.angle.x;
    field_0x744.y = current.angle.y;
    field_0x744.z = current.angle.z;
    field_0x77C.copy(current.pos);
    field_0x7C6 = true;
    return true;
}

/* 0230741C */
// HD: returns a cXyz through the hidden result pointer (HD: a NULL result pointer
// allocates the object
void daNpc_Zl1_c::kyoroPos(cXyz* o_pos, int param_1) {
    const cXyz& offset = a_tgt_offst.values[param_1];
    gabi::Local<cXyz> temp1;
    gabi::Local<cXyz> temp2;
    f32 x = eyePos.x, y = eyePos.y, z = eyePos.z;
    temp1->z = offset.z;
    temp1->y = offset.y;
    temp1->x = offset.x;
    PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), temp1, temp2);
    if (o_pos == nullptr) {
        o_pos = (cXyz*)operator_new(12);
        if (o_pos == nullptr)
            return;
    }
    o_pos->x = temp2->x;
    o_pos->y = temp2->y;
    o_pos->z = temp2->z;
}

/* 023074E4 */
BOOL daNpc_Zl1_c::kyorokyoro() {
    if (cLib_calcTimer(&field_0x7B0) != 0) {
        gabi::Local<cXyz> pos;
        kyoroPos(pos, field_0x7B2);
        field_0x758.copy(*pos);
        return TRUE;
    }
    field_0x7B2 = (s16)cLib_getRndValue(1, 10);
    if (field_0x849 == 0xB) {
        field_0x7B0 = l_demoHIO.mPrmTbl.field_2E;
        field_0x7AE = l_demoHIO.mPrmTbl.field_32;
    } else {
        field_0x7B0 = l_demoHIO.mPrmTbl.field_2C;
        field_0x7AE = l_demoHIO.mPrmTbl.field_30;
    }
    return FALSE;
}

/* 02305778 */
u8 daNpc_Zl1_c::demo() {
    u8 id = demoActorID;
    if (id == 0) {
        if (field_0x7D9) {
            field_0x7D9 = FALSE;
        }
        return field_0x7D9;
    }
    if (!field_0x7D9) {
        for (int i = 0; i < 2; i++) {
            if (field_0x6D4[i].get() != nullptr) {
                field_0x6D4[i]->field_0x7C = 0; /* clrMoveFlag() */
            }
        }
        m_jnt.mAngles[0][1] = 0; /* setHead_y(0) */
        m_jnt.mAngles[0][0] = 0; /* setHead_x(0) */
        m_jnt.mAngles[1][1] = 0; /* setBackBone_y(0) */
        field_0x7D9 = TRUE;
        field_0x7D3 = false;
        id = demoActorID;
        m_jnt.mAngles[1][0] = 0; /* setBackBone_x(0) */
    }
    /* dComIfGp_demo_getActor(demoActorID) (HD inline with a range check) */
    DemoActor* demo_actor = nullptr;
    if (id != 0 && id <= DEMO_ACTOR_COUNT) {
        DemoObject* obj = sDemoObject.pointer;
        if (obj == nullptr) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
            obj = sDemoObject.pointer;
        }
        demo_actor = Zl1Demo_getActor(obj, id);
    }
    void* btp = reinterpret_cast<PatternAnimation*>(mBtpAnm)->mpAnm.get(); /* mBtpAnm.getBtpAnm() */
    if (btp != nullptr) {
        u8 frame_max = (u8)J3DAnm_getFrameMax(btp);
        u8 frame = (u8)(mBtpAnmFrame + 1);
        mBtpAnmFrame = frame < frame_max ? frame : frame_max;
    }
    if (demo_actor != nullptr) {
        void* demo_btp = Zl1Demo_getP_BtpData(demo_actor, mArcName); /* getP_BtpData */
        if (demo_btp != nullptr) {
            J3DModelData* md = mpMorf->getModel()->getModelData();
            mDoExt_btpAnm_init(mBtpAnm, md, demo_btp, 1, 0, 1.0f, 0, -1, 1, 0);
            /* HD: the second btp animation, and the eye materials ("zelda_mouth") linked
             * through the animation's material table */
            mDoExt_btpAnm_init(mBtpAnm2, md, demo_btp, 1, 0, 1.0f, 0, -1, 1, 0);
            auto* pattern = reinterpret_cast<PatternAnimation*>(mBtpAnm2);
            PatternResource* anm2 = pattern->mpResource;
            u16 n = anm2->mMaterialCount;
            for (u32 i = 0; i < n; i++) {
                u32 name = gabi::ea(anm2->mMaterials.get()[i].mName.get());
                gabi::Local<SafeString> s1;
                s1->__vtbl = ZL1_SAFESTRING_VTBL;
                s1->mStringTop = name;
                MaterialNameHeader* h =
                    Zl1Demo_getMaterialName(md); /* the material name table header */
                SafeString_assureTermination(s1);
                s32 idx = Zl1Demo_getMaterialIndex(h->mNames.get(),
                                                   (u32)s1->mStringTop); /* JUTNameTab::getIndex */
                gabi::Local<SafeString> s2;
                s2->mStringTop = gabi::guest_addr("zelda_mouth"); /* "zelda_mouth" */
                s2->__vtbl = ZL1_SAFESTRING_VTBL;
                SafeString_assureTermination(s1);
                SafeString_assureTermination(s1);
                u32 pa = s1->mStringTop;
                SafeString_assureTermination(s2);
                u32 pb = s2->mStringTop;
                bool equal = true;
                if (pa != pb) {
                    equal = false;
                    for (u32 k = 0; k < SAFE_STRING_MAX_LENGTH; k++) {
                        u8 ca = gabi::at<be<u8>>(pa)[k];
                        u8 cb = gabi::at<be<u8>>(pb)[k];
                        if (ca != cb)
                            break;
                        if (ca == 0) {
                            equal = true;
                            break;
                        }
                    }
                }
                be<u32>* arr = pattern->mpMaterialLinks;
                u32 w = (u32)arr[i];
                if (!equal) {
                    arr[i] = w & MATERIAL_ANIMATION_MASK;
                    arr = pattern->mpMaterialLinks;
                    u32 j = (u32)idx + 1;
                    arr[i] = (u32)arr[i] | (j & MATERIAL_INDEX_MASK);
                    arr = pattern->mpMaterialLinks;
                    arr[j] = (u32)arr[j] & ~MATERIAL_ANIMATION_MASK;
                    arr = pattern->mpMaterialLinks;
                    arr[j] =
                        (u32)arr[j] | ((i << MATERIAL_ANIMATION_SHIFT) & MATERIAL_ANIMATION_MASK);
                } else {
                    arr[i] = w | ~MATERIAL_ANIMATION_MASK;
                    arr = pattern->mpMaterialLinks;
                    u32 j = (u32)idx;
                    arr[j] = (u32)arr[j] | MATERIAL_ANIMATION_MASK;
                }
            }
            mBtpAnmFrame = 0;
            field_0x847 = 0x11;
        }
    }
    void* btk = reinterpret_cast<TextureAnimation*>(mBtkAnm)->mpAnm.get(); /* mBtkAnm.getBtkAnm() */
    if (btk != nullptr) {
        u8 frame_max = (u8)J3DAnm_getFrameMax(btk);
        u8 frame = (u8)(mBtkAnmFrame + 1);
        mBtkAnmFrame = frame < frame_max ? frame : frame_max;
    }
    if (demo_actor != nullptr) {
        void* demo_btk = Zl1Demo_getP_BtkData(demo_actor, mArcName); /* getP_BtkData */
        if (demo_btk != nullptr) {
            mDoExt_btkAnm_init(mBtkAnm, mpMorf->getModel()->getModelData(), demo_btk, 1, 0, 1.0f, 0,
                               -1, 1, 0);
            mBtkAnmFrame = 0;
            field_0x848 = 10;
        }
        if (field_0x7CE) {
            f32 x = demo_actor->mTrans.x; /* *demo_actor->getTrans() */
            f32 y = demo_actor->mTrans.y;
            f32 z = demo_actor->mTrans.z;
            if (gabi::ftoi(x) != 0 || gabi::ftoi(y) != 0 || gabi::ftoi(z) != 0) {
                field_0x7CE = false;
                mpModel = nullptr;
            }
        }
    }
    /* HD: in the stage "Demo17" after frame 0x606 the demo actor's flag 0x40 is cleared and the
     * animation runs at 1.5x */
    if (SafeString_equal(gabi::guest_addr("Demo17"), gabi::ea(&sStartStage)) &&
        sDemoFrame >= 0x606) {
        if (demo_actor != nullptr) {
            demo_actor->mFlags &= ~ENABLE_ANM_FRAME_e;
        }
        mpMorf->setPlaySpeed(1.5f);
    }
    gabi::Local<cXyz> pos; /* passed by value */
    f32 z = current.pos.z;
    pos->z = z;
    f32 y = current.pos.y;
    pos->y = y;
    pos->x = current.pos.x;
    u32 snd = Zl1Demo_GetGndMtrlSndId(pos.get(), 10.0f); /* dBgS_GetGndMtrlSndId_Func(pos, 10.0f) */
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    dDemo_setDemoData(this, ENABLE_TRANS_e | ENABLE_ROTATE_e | ENABLE_ANM_e | ENABLE_ANM_FRAME_e,
                      mpMorf, mArcName, 0, nullptr, snd, (s8)reverb);
    /* HD: type 6 hides Tetra in parts of the demos "Demo05" and "Demo23" */
    if (field_0x84F == 6) {
        if (SafeString_equal(gabi::guest_addr("Demo05"), gabi::ea(&sStartStage))) {
            u32 f = sDemoFrame;
            field_0x7D4 = (u32)(f - 0x492) < 0x182;
        } else if (SafeString_equal(gabi::guest_addr("Demo23"), gabi::ea(&sStartStage))) {
            u32 f = sDemoFrame;
            field_0x7D4 = (u32)(f - 0xD66) < 0xB;
        }
    }
    return field_0x7D9;
}
/* 02307D18 */
BOOL daNpc_Zl1_c::_execute() {
    if (!field_0x7D5) {
        field_0x72C.copy(current.pos);
        field_0x738.x = current.angle.x;
        field_0x738.y = current.angle.y;
        field_0x738.z = current.angle.z;
        field_0x7D5 = true;
    }
    daNpc_Zl1_HIO_c::hio_prm_c& prm = l_demoHIO.mPrmTbl;
    s16 temp = field_0x7BC < 0 ? (s16)prm.mMaxTurnStep : (s16)prm.field_5C;
    m_jnt.setParam(prm.mMaxBackboneX, prm.mMaxBackboneY, prm.mMinBackboneX, prm.mMinBackboneY,
                   prm.mMaxHeadX, prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, temp);
    if (field_0x7D2 && demoActorID == 0) {
        return TRUE;
    }
    partner_search();
    checkOrder();
    if (!demo()) {
        s32 staff_id = -1;
        if (dComIfGp_event_runCheck() && !eventInfo.checkCommandTalk()) {
            staff_id = isEventEntry();
        }
        if (staff_id >= 0 || field_0x7C9) {
            event_proc(staff_id);
        } else {
            pmf_call(this, &mCurrActionFunc, nullptr); /* (this->*mCurrActionFunc)(NULL) */
        }
        lookBack();
        fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
        u32 flags = mObjAcch.m_flags;
        field_0x7C7 = (flags >> 5) & 1;  /* ChkGroundHit() */
        field_0x7C8 = (flags >> 12) & 1; /* ChkWaterIn() */
        mObjAcch.CrrPos(dComIfG_Bgsp());
        play_animation();
    } else {
        // HD: as GameCube USA/PAL: CrrPos and the ground/water flags while in a demo
        mObjAcch.CrrPos(dComIfG_Bgsp());
        u32 flags = mObjAcch.m_flags;
        field_0x7D2 = false;
        field_0x7C7 = (flags >> 5) & 1;
        field_0x7C8 = (flags >> 12) & 1;
    }
    eventOrder();
    field_0x73E.x = current.angle.x;
    field_0x73E.y = current.angle.y;
    field_0x73E.z = current.angle.z;
    if (!field_0x7D3) {
        shape_angle.z = current.angle.z;
        shape_angle.y = current.angle.y;
        shape_angle.x = current.angle.x;
    }
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    tevStr.mEnvrIdxOverride = dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this));
    darkProc();
    setMtx(false);
    if (!field_0x7D9 && !field_0x7CA && !field_0x7CB) {
        setCollision(50.0f, 140.0f);
    }
    return TRUE;
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py): private HD demo bindings and TU data
str 1002415C
str 10024128
str 10024130
str 10024138
str 10024240
str 10023C60
str 10023C38
data l_demoHIO 10468C20
data a_cut_tbl 101C74AC
data a_tgt_offst 101C74E4
data sDemoObject 101D5FFC
data sDemoFrame 101D600C
data sStartStage 1047E6B8
str 10024038
str 10024040
str 10024048
str 10024050
ext 025E1988 void Zl1Demo_seStart(u32)
ext 0255A51C void dKy_set_vrboxcol_ratio(f32)
ext 0255A458 void dKy_set_fogcol_ratio(f32)
ext 0255A418 void dKy_set_bgcol_ratio(f32)
ext 0255671C void dKy_fog_startendz_set(f32, f32, f32)
ext 025CB374 void Zl1Demo_StartShock(dVibration_c*, s32, s32, cXyz*)
ext 02543280 void Zl1Demo_cutEnd(dEvent_manager_c*, s32)
ext 025447C8 BOOL Zl1Demo_getIsAddvance(dEvent_manager_c*, s32)
ext 025D54C4 BOOL Zl1Demo_SearchByID(fpc_ProcID, gptr<fopAc_ac_c>*)
ext 0259D54C void Zl1Demo_playerEyePos(cXyz*, f32)
ext 0230A308 void Zl1Demo_assureTerminationImpl(SafeString*)
ext 02526E70 Zl1Demo::DemoActor* Zl1Demo_getActor(Zl1Demo::DemoObject*, u8)
ext 02527828 void* Zl1Demo_getP_BtpData(Zl1Demo::DemoActor*, const char*)
ext 025279C8 void* Zl1Demo_getP_BtkData(Zl1Demo::DemoActor*, const char*)
ext 027F3F8C Zl1Demo::MaterialNameHeader* Zl1Demo_getMaterialName(J3DModelData*)
ext 027DF9B0 s32 Zl1Demo_getMaterialIndex(void*, u32)
ext 024F1914 u32 Zl1Demo_GetGndMtrlSndId(cXyz*, f32)
*/
