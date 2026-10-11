/**
 * d_a_npc_people.cpp (WWHD)
 * NPC - Windfall townspeople (Uo, Ub, Uw, Um, Sa, Ug)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_people.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * This part: construction, heap, create phases, draw-side helpers and the small accessors.
 * The other parts: d_a_npc_people_*.cpp.
 */
#include "d/actor/d_a_npc_people_main.h"

// HD: per-TU SafeString vtable used by resource requests.
const gc_tu_data gc_tu = { .safeString = PEOPLE_SAFESTRING_VTBL, .safeStringAssure = 0x022C885C };

static u8 l_photo_no[19] = { 104, 105, 106, 107, 107, 108, 109, 102, 103, 100, 99, 101, 130, 110, 111, 113, 112, 98, 98 };
static s32 l_bmd_ix_tbl[19] = { 6, 10, 11, 1, 6, 7, 8, 1, 10, 11, 1, 12, 2, 18, 19, 21, 20, 2, 5 };
static s32 l_head_bmd_ix_tbl[19] = { 7, 8, 9, 2, 3, 4, 5, 2, 8, 7, 2, 8, 3, 11, 12, 14, 13, 3, 4 };
static s32 l_btp_ix_tbl[19] = { 12, 13, 14, 9, 10, 11, 12, 3, 9, 9, 3, 10, 4, 15, -1, 17, 16, 6, 7 };
static u32 l_head_diff_flag_tbl[19] = { 285343778, 285343778, 352452642, 285343778, 352452642, 352452642, 352452642, 352452642, 285343778, 352452642, 285343778, 352452642, 285343778, 352452642, 285343778, 352452642, 352452642, 285343778, 352452642 };
static s32 l_etc_bmd_ix_tbl[19][2] = { {-1, -1}, {-1, -1}, {21, 21}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, 22}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, 38}, {-1, 37}, {-1, -1}, {-1, -1} };
static cPhs_State phase_1(daNpcPeople_c*);
static cPhs_State phase_2(daNpcPeople_c*);
static PeopleMainPhase l_method[] = { { (const void*)&phase_1 }, { (const void*)&phase_2 }, {} };
static PeopleMainHeaderStatics l_headerStatics = {};
static PeopleMainRegistration l_registrations[3] = {};



static u32 daObj_PrmAbstract(fopAc_ac_c*, u32, u32);


static BOOL daNpc_People_nodeCallBack(J3DNode* node, int calcTiming);
static BOOL CheckCreateHeap(fopAc_ac_c* i_this);
static daNpcPeople_c* daNpcPeople_c_ct(daNpcPeople_c* i_this);
static s16 daNpcPeople_XyCheckCB(void* i_this, int i_itemBtn);
static s16 daNpcPeople_XyEventCB(void* i_this, int i_itemBtn);
static s16 daNpcPeople_photoCB(void* i_this, int param_1);
static cPhs_State phase_2(daNpcPeople_c* i_this);
static cPhs_State daNpc_PeopleCreate(void* i_this);
static BOOL daNpc_PeopleDelete(void* i_this);
static BOOL daNpc_PeopleExecute(void* i_this);
static BOOL daNpc_PeopleDraw(void* i_this);
static BOOL daNpc_PeopleIsDelete(void* i_this);
static u32 daObj_PrmAbstract(fopAc_ac_c* i_actor, u32 i_width, u32 i_shift);
static cPhs_State phase_1(daNpcPeople_c* i_this);
static void __sinit_d_a_npc_people_cpp();
static void SafeString_dt(void* p, s32 flags);
static void daNpcPeople_dt(daNpcPeople_c* p, s32 flags);
static void SafeString_assureTerminationImpl(void*);

static void people_setBaseTRMtx_anm(J3DModel* dst, J3DModel* src, s32 jnt) {
    mtx_copy(&dst->mBaseTransformMtx, src->getAnmMtx(jnt));
}

static J3DModelData* people_getRes(u32 npcNo, s32 id) {
    return (J3DModelData*)dComIfG_getObjectIDRes(people_arcname(npcNo), id);
}

static s16 people_getEventIdx(const char* name) { return dComIfGp_evmng_getEventIdx(name, 0xFF); }

/* 022BE034 */
static BOOL daNpc_People_nodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        J3DModel* model = peopleCurrentModel();
        daNpcPeople_c* i_this = static_cast<daNpcPeople_c*>(model->getUserArea());
        u32 jntNo = reinterpret_cast<J3DJoint*>(node)->getJntNo();
        PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
        if (jntNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
            mDoMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[0][1]);
            mDoMtx_ZrotM(calc_mtx(), -i_this->m_jnt.mAngles[0][0]);
        }
        if (jntNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
            mDoMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]);
            mDoMtx_ZrotM(calc_mtx(), -i_this->m_jnt.mAngles[1][0]);
        }
        mtx_copy(model->getAnmMtx(jntNo), calc_mtx());
        PSMTXCopy(calc_mtx(), peopleCurrentMtx());
    }
    return TRUE;
}

/* 022BE780 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return static_cast<daNpcPeople_c*>(i_this)->createHeap();
}

/* 022BE8C0 */
static cPhs_State phase_1(daNpcPeople_c* i_this) {
    /* fopAcM_ct(i_this, daNpcPeople_c) */
    if (!(i_this->actor_condition & fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            daNpcPeople_c_ct(i_this);
        }
        i_this->actor_condition = i_this->actor_condition | fopAcCnd_INIT_e;
    }
    u32 arg0 = i_this->getPrmArg0();
    switch (i_this->getNpcNo()) {
    case NPC_UB4: {
        gabi::Local<SafeString> name;
        name->mStringTop = gabi::guest_addr("Ocmera"); /* "Ocmera" */
        name->__vtbl = PEOPLE_SAFESTRING_VTBL;
        dComIfG_inf_c* play = peopleMainGame();
        gabi::Local<SafeString> start; /* dComIfGp_getStartStageName() */
        start->__vtbl = PEOPLE_SAFESTRING_VTBL;
        start->mStringTop = gabi::ea(play->play.mCurStage.mName);
        if (people_SafeString_eq(name.get(), start.get())) {
            if (dComIfGs_checkGetItem(dItemNo_COLLECT_MAP_20_e) ||
                arg0 != (u32)(s32)dComIfGp_getStartStagePoint()) {
                return cPhs_UNK3_e; /* cPhs_STOP_e */
            }
            i_this->setResFlag(1);
            i_this->setEtcFlag(0x40);
            return cPhs_NEXT_e;
        }
        break;
    }
    case NPC_UW2:
    case NPC_UM2:
        if (arg0 != 0xFF && (u32)((arg0 << 1) & 0xFE) != (u32)(dComIfGs_getEventReg(0xB907) & 6)) {
            return cPhs_UNK3_e;
        }
        break;
    case NPC_UG1:
    case NPC_UG2: {
        int day = dKy_get_dayofweek();
        switch (arg0) {
        case 0:
            if (day == 5 || day == 6) {
                return cPhs_UNK3_e;
            }
            break;
        case 1:
            if (day != 5 && day != 6) {
                return cPhs_UNK3_e;
            }
            break;
        }
        break;
    }
    default:
        break;
    }
    i_this->setResFlag(1);
    return cPhs_NEXT_e;
}



/* 022BFE54 */
static cPhs_State phase_2(daNpcPeople_c* i_this) {
    cPhs_State rt = dComIfG_resLoad(i_this->getPhaseP(), people_arcname(i_this->getNpcNo()));
    if (rt == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, gabi::guest_fn((const void*)&CheckCreateHeap), 0x3300)) {
            return i_this->createInit();
        }
        i_this->mpMorf = nullptr;
        return cPhs_ERROR_e;
    }
    return rt;
}

/* 022BFEE8 */
cPhs_State daNpcPeople_c::_create() {
    /* static cPhs__Handler l_method[] = {phase_1, phase_2, NULL} (.data 0x101C4C5C) */
    return dComLbG_PhaseHandler(&mPhase2, gabi::ea(l_method), this);
}

/* 022BE334 */
BOOL daNpcPeople_c::createHeap() {
    /* r6/r7: what GHS leaves there (compared through getIDRes's GameCube signature) */
    u32 npc = mNpcNo;
    people_set_r(7, npc * 4);
    J3DModelData* bodyModelData = people_getRes(npc, l_bmd_ix_tbl[npc]);
    u32 npc2 = mNpcNo;
    s32 bck = getBck(m793);
    people_set_r(7, npc2 * 4);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(people_arcname(npc2), bck);
    /* HD: l_diff_flag_tbl is 0x11020203 for every NPC (a constant) */
    mpMorf = mDoExt_McaMorf::create(nullptr, bodyModelData, nullptr, nullptr, anm, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1,
                                    nullptr, 0x00080000, 0x11020203);
    if (mpMorf.get() == nullptr || mpMorf->getModel() == nullptr) {
        return false;
    }
    u32 npc3 = mNpcNo;
    people_set_r(7, npc3 * 4);
    J3DModelData* headModelData = people_getRes(npc3, l_head_bmd_ix_tbl[npc3]);
    int headBck = getHeadBck(0);
    if (headBck < 0) {
        mpHeadModel = mDoExt_J3DModel__create(headModelData, 0x80000, l_head_diff_flag_tbl[mNpcNo]);
        if (mpHeadModel.get() == nullptr) {
            return false;
        }
    } else {
        u32 npc4 = mNpcNo;
        s32 hbck = getHeadBck(m793);
        const char* arc = people_arcname(npc4);
        people_set_r(6, gabi::ea(arc));
        J3DAnmTransform* hanm = (J3DAnmTransform*)dComIfG_getObjectIDRes(arc, hbck);
        mpHeadMorf = mDoExt_McaMorf::create(nullptr, headModelData, nullptr, nullptr, hanm, 2, 1.0f, 0, -1, 1, nullptr,
                                            0x00080000, l_head_diff_flag_tbl[mNpcNo]);
        if (mpHeadMorf.get() == nullptr || mpHeadMorf->getModel() == nullptr) {
            return false;
        }
    }
    m_jnt.mHeadJntNum = peopleJointNameIndex(bodyModelData->getJointName(), "head" /* "head" */);
    if (!(m_jnt.mHeadJntNum >= 0)) {
        JUT_ASSERT_fail("d_a_npc_people.cpp", 0x128C, "m_jnt.getHeadJntNum() >= 0");
    }
    m_jnt.mBackboneJntNum = peopleJointNameIndex(bodyModelData->getJointName(), "backbone" /* "backbone" */);
    if (!(m_jnt.mBackboneJntNum >= 0)) {
        JUT_ASSERT_fail("d_a_npc_people.cpp", 0x1290, "m_jnt.getBackboneJntNum() >= 0");
    }
    if (!initTexPatternAnm(false)) {
        return false;
    }
    for (u16 i = 0; i < bodyModelData->getJointNum(); i++) {
        if (i == (u32)(s32)m_jnt.mHeadJntNum || i == (u32)(s32)m_jnt.mBackboneJntNum) {
            bodyModelData->getJointNodePointer(i)->setCallBack(&daNpc_People_nodeCallBack);
        }
    }
    mpMorf->getModel()->mUserArea = gabi::ea(this);
    mAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    u32 npc5 = mNpcNo;
    s32 etc = l_etc_bmd_ix_tbl[npc5][mbIsNight];
    if (etc >= 0) {
        people_set_r(6, npc5 * 4);
        J3DModelData* etcModelData = people_getRes(npc5, etc);
        mpEtcModel = mDoExt_J3DModel__create(etcModelData, 0x80000, 0x11000002);
        if (mpEtcModel.get() == nullptr) {
            return false;
        }
        m7A3 = peopleJointNameIndex(bodyModelData->getJointName(), "handR" /* "handR" */);
    }
    return true;
}

/* 022BF510 */
cPhs_State daNpcPeople_c::createInit() {
    int temp = 0xFF;
    u8 pathIndex = getPrmRailID();
    if (pathIndex != 0xFF) {
        dNpc_PathRun_setInf(&mPathRun, pathIndex, fopAcM_GetRoomNo(this), true);
        if (mPathRun.mPath.get() == nullptr) {
            return cPhs_ERROR_e;
        }
        actor_status = actor_status & ~fopAcStts_NOCULLEXEC_e; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
        gabi::Local<cXyz> point;
        dNpc_PathRun_getPoint(&mPathRun, point.get(), mPathRun.mIdx);
        old.pos.copy(*point);
        current.pos.copy(*point);
        dNpc_PathRun_incIdxLoop(&mPathRun);
        m76E = 1;
        temp = 0xFE;
        m7A1 = 1;
    }
    gravity = -9.0f;
    setWaitAnm();
    for (int i = 0; i < 4; i++) {
        m766[i] = -1;
    }
    switch (mNpcNo) {
    case NPC_UO1:
        m766[0] = people_getEventIdx("UO1_GET_ITEM"); /* "UO1_GET_ITEM" */
        break;
    case NPC_UB1:
    case NPC_UB2:
        m766[0] = people_getEventIdx("UB1_TALK"); /* "UB1_TALK" */
        m766[1] = people_getEventIdx("UB1_TALK_XY"); /* "UB1_TALK_XY" */
        m766[1] = people_getEventIdx("UB1_TALK_PHOTO"); /* "UB1_TALK_PHOTO" */
        {
            s16 idx = people_getEventIdx("UB1_TALK_PHOTO_GET_ITEM"); /* "UB1_TALK_PHOTO_GET_ITEM" */
            m79C = 0;
            m766[3] = idx;
        }
        eventInfo.setEventId(m766[0]);
        break;
    case NPC_UB4:
        m766[0] = people_getEventIdx("UB4_GET_ITEM"); /* "UB4_GET_ITEM" */
        break;
    case NPC_UW2:
        m766[0] = people_getEventIdx("UW2_GET_ITEM"); /* "UW2_GET_ITEM" */
        break;
    case NPC_UM1:
        m766[0] = people_getEventIdx("UM1_GET_ITEM"); /* "UM1_GET_ITEM" */
        m766[1] = people_getEventIdx("UM1_TALK"); /* "UM1_TALK" */
        break;
    case NPC_UM3:
        m766[0] = people_getEventIdx("UM3_TELESCOPE_TALK"); /* "UM3_TELESCOPE_TALK" */
        m766[1] = people_getEventIdx("UM3_GET_ITEM"); /* "UM3_GET_ITEM" */
        if (mbIsNight) {
            mEtcFlag = mEtcFlag | 0x40000000;
        }
        break;
    case NPC_SA3:
        m766[0] = people_getEventIdx("SA3_GET_ITEM"); /* "SA3_GET_ITEM" */
        break;
    case NPC_SA5: {
        m766[0] = people_getEventIdx("SA5_TALK_XY"); /* "SA5_TALK_XY" */
        m766[1] = people_getEventIdx("SA5_GET_ITEM"); /* "SA5_GET_ITEM" */
        u8 reg = dComIfGs_getTmpReg(0xFD07 /* UNK_FD07 */);
        for (int i = 0; i < 3; i++) {
            if (reg & (1 << i)) {
                PeopleMainPig* pig = fopAcM_searchFromName_l("Pig", 0xF00, l_pig_para()[i]);
                if (pig != 0) {
                    /* kb_class::taura_pos_set(current.pos) */
                    f32 y = current.pos.y, x = current.pos.x, z = current.pos.z;
                    pig->m450.x = x;
                    pig->m450.y = y;
                    pig->m408 = 1;
                    pig->m450.z = z;
                    pig->current.pos.copy(current.pos);
                }
            }
        }
    }
        /* fall through */
    case NPC_SA4:
        if (mbIsNight) {
            mEtcFlag = mEtcFlag | 0x40000000;
        }
        break;
    case NPC_UG1:
        /* HD: an UG1 placed at x = -1984 is moved to (-2002, 284, -200328) */
        if (gabi::ftoi(home.pos.x) == -0x7C0) {
            current.pos.y = 284.0f;
            current.pos.z = -200328.0f;
            old.pos.y = 284.0f;
            old.pos.z = -200328.0f;
            current.pos.x = -2002.0f;
            old.pos.x = -2002.0f;
            home.pos.x = -2002.0f;
            home.pos.y = 284.0f;
            home.pos.z = -200328.0f;
        }
        /* fall through */
    case NPC_UG2:
        if (getPrmArg0() == 0) {
            temp = 0x50;
            m76E = 1;
            mEtcFlag = 0x1000;
        }
        m766[0] = people_getEventIdx("UG1_TALK"); /* "UG1_TALK" */
        m7A1 = 1;
        break;
    default:
        break;
    }
    eventInfo.setXyCheckCB(&daNpcPeople_XyCheckCB);
    eventInfo.setXyEventCB(&daNpcPeople_XyEventCB);
    eventInfo.mpPhotoCB = gabi::guest_fn((const void*)&daNpcPeople_photoCB);
    mEventCut.setActorInfo2(l_npc_staff_id()[mNpcNo], this);
    m784 = 0;
    mTalk = 0;
    m789 = 0;
    m78A = 0;
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -70.0f, 0.0f, -70.0f, 70.0f, 200.0f, 70.0f);
    cullSizeFar = mpNpcDat->field_0x30 / peopleClipperFar();
    be<u8>* dist = &l_npc_dist_tbl()[mNpcNo][mbIsNight];
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = *dist; /* attention_info.distances[fopAc_Attn_TYPE_TALK_e] */
    attention_info.flags = 0x0100000A; /* attention_info.flags */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = *dist; /* attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] */
    daNpcPeople_c__l_npc_dat* dat = mpNpcDat;
    m_jnt.setParam(dat->field_0x04, dat->field_0x06, dat->field_0x0C, dat->field_0x0E, dat->field_0x00, dat->field_0x02,
                   dat->field_0x08, dat->field_0x0A, dat->field_0x10);
    if (mNpcNo == NPC_UM3 && !mbIsNight) {
        dComIfGs_checkGetItem(dItemNo_COLLECT_MAP_15_e);
    }
    m79D = mpNpcDat->field_0x5A;
    m79E = mpNpcDat->field_0x5C;
    m79F = mpNpcDat->field_0x5D;
    m748 = mpNpcDat->field_0x28;
    m776 = mpNpcDat->field_0x34;
    m778 = mpNpcDat->field_0x36;
    m74C = mpNpcDat->field_0x3C;
    if (mNpcNo != NPC_UM3) {
        mObjAcch.CrrPos(dComIfG_Bgsp());
        f32 gnd = mObjAcch.GetGroundH();
        if (-1000000000.0f /* -G_CM3D_F_INF */ != gnd) {
            home.pos.y = gnd;
            current.pos.y = gnd;
        }
    }
    setMtx();
    J3DModel_calc(mpMorf->getModel());
    mStts.Init(temp, 0xFF, this);
    mCyl.Set(peopleCylinderSource() /* dNpc_cyl_src */);
    mCyl.SetStts(&mStts);
    gabi::Local<cXyz> center;
    center->x = current.pos.x;
    center->y = current.pos.y;
    center->z = current.pos.z;
    setCollision(&mCyl, center.get(), m74C, mpNpcDat->field_0x40);
    return cPhs_COMPLEATE_e;
}

/* 022BFF00 */
bool daNpcPeople_c::_delete() {
    dComIfG_resDelete(&mPhase, people_arcname(mNpcNo)); /* HD: dComIfG_resDeleteDemo is resDelete */
    if (heap.get() != nullptr) {
        if (mpMorf.get() != nullptr) {
            mpMorf->stopZelAnime();
        }
        if (mpHeadMorf.get() != nullptr) {
            mpHeadMorf->stopZelAnime();
        }
    }
    return true;
}

/* 022C36D4 */
bool daNpcPeople_c::_draw() {
    J3DModel* bodyModel = mpMorf->getModel();
    J3DModel* headModel;
    if (mpHeadModel.get() != nullptr) {
        headModel = mpHeadModel;
    } else {
        headModel = mpHeadMorf->getModel();
    }
    J3DModelData* headModelData = headModel->getModelData();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    mDoExt_McaMorf* morf = mpMorf;
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, morf->getModel(), &tevStr);
    setLightTevColorType(dKy_getEnvlight(), headModel, &tevStr);
    if (l_btp_ix_tbl[mNpcNo] >= 0) {
        mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, headModelData, m78E);
    }
    /* HD: no l_bmt_ix_tbl material table */
    mpMorf->updateDL();
    people_setBaseTRMtx_anm(headModel, bodyModel, m_jnt.mHeadJntNum);
    if (mpHeadModel.get() != nullptr) {
        mDoExt_modelUpdateDL(mpHeadModel);
    } else {
        mpHeadMorf->updateDL();
    }
    if (l_btp_ix_tbl[mNpcNo] >= 0) {
        peopleModelDataTail(headModelData)->mTexNoAnimator = 0; /* mBtpAnm.remove(headModelData) */
    }
    if (mpEtcModel.get() != nullptr && (mEtcFlag & 0x40000001)) {
        setLightTevColorType(dKy_getEnvlight(), mpEtcModel, &tevStr);
        people_setBaseTRMtx_anm(mpEtcModel, bodyModel, m7A3);
        mDoExt_modelUpdateDL(mpEtcModel);
    }
    /* HD: no dComIfGd_setShadow / addRealShadow */
    if (m7A6 != 0xFF) {
        gabi::Local<dSnap_Obj_l> obj;
        dSnap_Obj_ct(obj.get());
        PsoData_l* pso = l_pso_tbl()[m7A6];
        gabi::Local<cXyz> temp;
        temp->x = pso->field_0x00;
        temp->y = pso->field_0x04;
        temp->z = pso->field_0x08;
        PSVECAdd(temp.get(), &current.pos, temp.get());
        dSnap_Obj_SetInf(obj.get(), pso->photoNo, this, pso->field_0x16, pso->field_0x17, 0x7FFF);
        dSnap_Obj_SetGeo(obj.get(), temp.get(), pso->field_0x0C, pso->field_0x10, pso->field_0x14 + current.angle.y);
        dSnap_RegistSnapObj(obj.get());
        /* HD: m7A6 is not reset to 0xFF */
    } else {
        dSnap_RegistFig(l_photo_no[mNpcNo], this, 1.0f, 1.0f, 1.0f);
    }
    return true;
}



/* 022C1890 */
void daNpcPeople_c::eventMesSetPoInit(int param_1) {
    eventMesSetInit(param_1);
}

/* 022BE7B0 */
u8 daNpcPeople_c::getPrmNpcNo() {
    if (0 <= argument && argument < NPC_NO_MAX) {
        return argument;
    }
    return 0;
}

/* 022BEB50 */
u8 daNpcPeople_c::getPrmRailID() {
    return daObj_PrmAbstract(this, PRM_RAIL_ID_W, PRM_RAIL_ID_S);
}

/* 022BE784 */
u8 daNpcPeople_c::getPrmArg0() {
    return daObj_PrmAbstract(this, PRM_ARG0_W, PRM_ARG0_S);
}

/* 022BF3A0 */
void daNpcPeople_c::setMtx() {
    J3DModel* model = mpMorf->getModel();
    cXyz* base_scale = &model->mBaseScale;
    f32 sx = scale.x, sy = scale.y, sz = scale.z;
    base_scale->x = sx;
    base_scale->y = sy;
    base_scale->z = sz;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
}

/* 022BE1E8 */
BOOL daNpcPeople_c::initTexPatternAnm(u32 param_1) {
    m_head_tex_pattern = nullptr;
    s32 btp = l_btp_ix_tbl[mNpcNo];
    if (btp >= 0) {
        J3DModelData* modelData;
        if (mpHeadModel.get() != nullptr) {
            modelData = mpHeadModel->getModelData();
        } else {
            modelData = mpHeadMorf->getModel()->getModelData();
        }
        const char* arc = people_arcname(mNpcNo);
        m_head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectIDRes_r67(arc, btp, gabi::ea(arc), gabi::ea(l_arcname_tbl()));
        if (m_head_tex_pattern.get() == nullptr) {
            JUT_ASSERT_fail("d_a_npc_people.cpp", 0x240D, "m_head_tex_pattern != (0)");
        }
        if (!mDoExt_btpAnm_init(mBtpAnm, modelData, m_head_tex_pattern, TRUE, 2 /* EMode_LOOP */, 1.0f, 0, -1, param_1, FALSE)) {
            return false;
        }
    }
    m78E = 0;
    m780 = 0;
    return true;
}

/* 022BE18C */
int daNpcPeople_c::getBck(int param_1) {
    be<s32>* tbl = l_bck_ix_tbl()[mNpcNo][mbIsNight];
    return tbl[param_1];
}

/* 022BE1B4 */
int daNpcPeople_c::getHeadBck(int param_1) {
    be<s32>* tbl = l_head_bck_ix_tbl()[mNpcNo][mbIsNight];
    if (tbl == 0) {
        return -1;
    }
    return tbl[param_1];
}

/* 022BEB7C */
void daNpcPeople_c::setAnm(u32 param_1, int param_2, f32 morf, f32 param_4) {
    if (!(mNpcNo < NPC_NO_MAX)) {
        JUT_ASSERT_fail("d_a_npc_people.cpp", 0x246A, "mNpcNo >= 0 && mNpcNo < daNpcPeople_c::Npc_No_Max"); /* HD: l_arcname_tbl bound check */
    }
    if (mNpcNo == NPC_UW1) {
        if (m793 == 4 && param_1 == 0) {
            morf = 12.0f;
        } else if (m793 == 3 && param_1 == 1) {
            morf = 12.0f;
        }
    }
    if (!(m744 < 0.0f)) {
        morf = m744;
        m744 = -1.0f;
    }
    const char* arc = people_arcname(mNpcNo);
    s32 bck = getBck(param_1);
    /* r6/r7: the high halves GHS left from loading the -1.0f/0.0f constants */
    J3DAnmTransform* pAnm = (J3DAnmTransform*)dComIfG_getObjectIDRes_r67(arc, bck, PEOPLE_RESOURCE_HIGH_HALF, PEOPLE_RESOURCE_HIGH_HALF);
    mpMorf->setAnm(pAnm, param_2, morf, param_4, 0.0f, -1.0f, nullptr);
    if (mpHeadMorf.get() != nullptr && getHeadBck(0) >= 0) {
        const char* arc2 = people_arcname(mNpcNo);
        s32 head_bck = getHeadBck(param_1);
        J3DAnmTransform* pHeadAnm = (J3DAnmTransform*)dComIfG_getObjectIDRes(arc2, head_bck);
        mpHeadMorf->setAnm(pHeadAnm, param_2, morf, param_4, 0.0f, -1.0f, nullptr);
    }
    m793 = param_1;
    m760 = param_2;
}

/* 022BED70 */
bool daNpcPeople_c::setAnmTbl(sPeopleAnmDat* param_1, int param_2) {
    if (mNpcNo == NPC_SA5) {
        if (m793 == 1) {
            m744 = 16.0f;
        }
    } else if (mNpcNo == NPC_SA4) {
        if (m793 == 1) {
            m744 = 13.0f;
        }
    } else if (mNpcNo == NPC_UM3 && m793 == 5) {
        m744 = 13.0f;
        if (param_1->field_0x00 == 1) {
            m744 = 6.0f;
        }
    }
    mAnmFlag = mAnmFlag & ~0x1;
    if (param_1->field_0x00 == 0xFF && param_2 != 0) {
        m728 = nullptr;
        return true;
    }
    m728 = param_1;
    m796 = m728->field_0x02;
    m750 = 1.0f;
    int temp = 2;
    if (m796 != 0) {
        temp = 0;
    }
    if (m796 < 0) {
        m796 = -m796;
        m750 = -1.0f;
    }
    if (m793 != m728->field_0x00 || temp == 0 || m760 == 0) {
        setAnm(m728->field_0x00, temp, (f32)m728->field_0x01, m750);
    }
    return false;
}

/* 022BEEDC */
void daNpcPeople_c::setWaitAnm() {
    if (mNpcNo == NPC_UB3 && m789 != 0) {
        setAnmTbl(l_npc_anm_wait(), 0);
    } else if (mNpcNo == NPC_UM3 && !mbIsNight && dComIfGs_checkGetItem(dItemNo_COLLECT_MAP_15_e)) {
        setAnmTbl(l_npc_anm_um3_wait3(), 1);
    } else if (mNpcNo == NPC_UW2 && m793 == 1) {
        setAnmTbl(l_npc_anm_talkH2(), 1);
    } else {
        if (!(mNpcNo < NPC_NO_MAX)) {
            JUT_ASSERT_fail("d_a_npc_people.cpp", 0x24CB, "mNpcNo >= 0 && mNpcNo < daNpcPeople_c::Npc_No_Max");
        }

        setAnmTbl(l_npc_anm_wait_tbl()[mNpcNo][mbIsNight], 1);
    }
}

/* 022BF05C */
s16 daNpcPeople_c::XyCheckCB(int i_itemBtn) {
    u8 itemNo = dComIfGp_getSelectItem(i_itemBtn);
    switch (mNpcNo) {
    case NPC_UO3:
    case NPC_UM2:
        if (isPhoto(itemNo)) {
            return true;
        }
        break;
    case NPC_UB1:
    case NPC_UB2:
        if (dComIfGs_isEventBit(0x2102) && isPhoto(itemNo)) {
            return true;
        }
        if (itemNo == dItemNo_JOY_PENDANT_e) {
            return true;
        }
        break;
    case NPC_UB3:
        if (itemNo == dItemNo_JOY_PENDANT_e) {
            return true;
        }
        break;
    case NPC_UB4:
        if (!(mEtcFlag & 0x40) && isPhoto(itemNo)) {
            return true;
        }
        if (itemNo == dItemNo_JOY_PENDANT_e) {
            return true;
        }
        break;
    case NPC_UM3:
        if (!mbIsNight && isPhoto(itemNo) && dComIfGs_isEventBit(0x2310) && !dComIfGs_checkGetItem(dItemNo_COLLECT_MAP_15_e)) {
            return true;
        }
        break;
    case NPC_SA5:
        if (isPhoto(itemNo)) {
            return false;
        }
        if (!mbIsNight && dComIfGs_isEventBit(0x2440) && !dComIfGs_isTmpBit(0x0280)) {
            return true;
        }
        break;
    }
    return false;
}

/* 022BF220 */
s16 daNpcPeople_c::XyEventCB(int i_itemBtn) {
    s16 ret = -1;
    u8 itemNo = dComIfGp_getSelectItem(i_itemBtn);
    switch (mNpcNo) {
    case NPC_UB1:
    case NPC_UB2:
        if (itemNo != dItemNo_JOY_PENDANT_e) {
            ret = m766[1];
            m79C = 0;
            break;
        }
        m79C = 1;
        break;
    case NPC_SA5:
        if (itemNo == dItemNo_SKULL_NECKLACE_e) {
            ret = m766[0];
            m79C = 0;
            break;
        }
    default:
        ret = dComIfGp_evmng_getEventIdx("DEFAULT_TALK_XY" /* "DEFAULT_TALK_XY" */, 0xFF);
    }
    return ret;
}

// HD: matrix copies retain the original interleaved loads and stores.


/* 022BF330 */
s16 daNpcPeople_c::photoCB(int param_1) {
    s16 ret = -1;
    switch (mNpcNo) {
    case NPC_UB1:
    case NPC_UB2:
        if (dComIfGp_getPictureResult() == 5 && !is1GetMap20()) {
            ret = m766[3];
        } else {
            ret = m766[1];
        }
    }
    return ret;
}

/* 022BFF78 */
int daNpcPeople_c::getRand(int max) {
    int rnd = gabi::ftoi(cM_rndF((f32)max));
    if (rnd == max) {
        rnd = 0;
    }
    return rnd;
}

/* 022BEFF4 */
BOOL daNpcPeople_c::isPhoto(u8 param_1) {
    /* HD: dComIfGs_getPictureNum() is a byte of the picture save data (02720144 returns it) */
    if ((param_1 == dItemNo_PICTO_BOX_e || param_1 == dItemNo_DELUXE_PICTO_BOX_e) &&
        peoplePictureData(&peopleMainSave()->mPictureSave)->mPictureNum != 0) {
        return true;
    }
    return false;
}

/* 022BF480 */
void daNpcPeople_c::setCollision(dCcD_Cyl* cyl, cXyz* center, f32 radius, f32 height) {
    cyl->SetC(center);
    cyl->SetR(radius);
    cyl->SetH(height);
    dComIfG_Ccsp_Set(cyl);
}

/* 022BF304 */
BOOL daNpcPeople_c::is1GetMap20() {
    return dComIfGs_checkGetItem(dItemNo_COLLECT_MAP_20_e) ? TRUE : FALSE;
}

/* 022C1108 */
BOOL daNpcPeople_c::is1DayGetMap20() {
    return dComIfGs_getEventReg(0xC103 /* UNK_C103 */) == 2 ? TRUE : FALSE;
}

/* 022C6C00 */
BOOL daNpcPeople_c::isPigOk() {
    u8 a = dComIfGs_getTmpReg(0xFC03 /* UNK_FC03 */);
    return a == dComIfGs_getTmpReg(0xFF03 /* UNK_FF03 */) ? TRUE : FALSE;
}

/* 022C6BDC */
s16 daNpcPeople_c::getPigTimer() {
    return peopleMainGame()->play.mItemTimer;
}

/* 022BFFE4 */
void daNpcPeople_c::warp() {
    if (fopAcM_GetParam(this) & 0x80000000) {
        mParameters = fopAcM_GetParam(this) & ~0x80000000;
        if (mPathRun.mPath.get() != nullptr) {
            mPathRun.mbDir = true;
            mPathRun.mIdx = 0;
            gabi::Local<cXyz> pt;
            dNpc_PathRun_getPoint(&mPathRun, pt.get(), 0);
            old.pos.copy(*pt);
            current.pos.copy(old.pos);
            dNpc_PathRun_incIdxLoop(&mPathRun);
        }
        speedF = 0.0f;
        m740 = 0.0f;
    }
}

/* 022BFEFC */
static cPhs_State daNpc_PeopleCreate(void* i_this) {
    return static_cast<daNpcPeople_c*>(i_this)->_create();
}

/* 022BFF74 */
static BOOL daNpc_PeopleDelete(void* i_this) {
    return static_cast<daNpcPeople_c*>(i_this)->_delete();
}

/* 022C36D0 */
static BOOL daNpc_PeopleExecute(void* i_this) {
    return static_cast<daNpcPeople_c*>(i_this)->_execute();
}

/* 022C39EC */
static BOOL daNpc_PeopleDraw(void* i_this) {
    return static_cast<daNpcPeople_c*>(i_this)->_draw();
}

/* 022C87B8 */
static BOOL daNpc_PeopleIsDelete(void* i_this) {
    return true;
}

/* 022BF21C */
static s16 daNpcPeople_XyCheckCB(void* i_this, int i_itemBtn) {
    return static_cast<daNpcPeople_c*>(i_this)->XyCheckCB(i_itemBtn);
}

/* 022BF300 */
static s16 daNpcPeople_XyEventCB(void* i_this, int i_itemBtn) {
    return static_cast<daNpcPeople_c*>(i_this)->XyEventCB(i_itemBtn);
}

/* 022BF39C */
static s16 daNpcPeople_photoCB(void* i_this, int param_1) {
    return static_cast<daNpcPeople_c*>(i_this)->photoCB(param_1);
}


// HD: compiler-generated and template code of this translation unit.
/* 022BE7CC */
static daNpcPeople_c* daNpcPeople_c_ct(daNpcPeople_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpcPeople_c*)operator_new(0x920);
        if (i_this == nullptr) {
            return i_this;
        }
    }
    fopNpc_npc_c_ct(i_this);
    i_this->__vtbl = PEOPLE_VTBL;
    mDoExt_btpAnm_ct(i_this->mBtpAnm);
    i_this->mNpcNo = i_this->getPrmNpcNo();
    i_this->mResFlag = 0;
    i_this->m78F = 0;
    i_this->m740 = 0.0f;
    i_this->m76E = 0;
    i_this->m744 = -1.0f;
    i_this->m799 = 0;
    i_this->m764 = true;
    i_this->m79C = 1;
    i_this->m738 = nullptr;
    i_this->m77A = i_this->home.angle.y;
    i_this->m793 = 0;
    i_this->m760 = 0;
    i_this->m794 = 0;
    i_this->mEtcFlag = 0;
    i_this->m7A1 = 0;
    i_this->mbIsNight = dKy_daynight_check() & 1;
    i_this->mpNpcDat = l_npc_dat()[i_this->mNpcNo][i_this->mbIsNight];
    i_this->m730 = nullptr;
    return i_this;
}

/* 022C8860 */
static u32 daObj_PrmAbstract(fopAc_ac_c* i_actor, u32 i_width, u32 i_shift) {
    /* PowerPC slw/srw: shift counts 32..63 give 0 */
    u32 mask = ((i_width & 0x20) ? 0u : (1u << (i_width & 0x1F))) - 1;
    u32 prm = i_actor->mParameters;
    return ((i_shift & 0x20) ? 0u : (prm >> (i_shift & 0x1F))) & mask;
}

/* 022C8710 */
static void __sinit_d_a_npc_people_cpp() {
    for (int i = 0; i < 4; i++) {
        l_headerStatics.mZeros[i] = 0;
    }
    __register_global_object(gabi::ea(&l_registrations[0]));
    l_headerStatics.mMin = -3.1415927f;
    l_headerStatics.mMax = 3.1415927f;
    peopleHeaderObjectOneCtor(&l_headerStatics.mObjectOne);
    __register_global_object(gabi::ea(&l_registrations[1]));
    peopleHeaderObjectTwoCtor(&l_headerStatics.mObjectTwo);
    __register_global_object(gabi::ea(&l_registrations[2]));
}

/* 022C87A4 */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

// HD: compiler-generated deleting destructor.
/* 022C87C0 */
static void daNpcPeople_dt(daNpcPeople_c* p, s32 flags) {
    if (p != nullptr) {
        peopleCylinderDestructor(&p->mCyl, 2);                                    /* ~dCcD_Cyl */
        peopleSttsDestructor(&p->mStts, 2);                                   /* ~dCcD_Stts */
        peopleCircleDestructor(&p->mAcchCir.m_cir, 2); /* ~cM3dGCir */
        peopleAcchVtables(&p->mObjAcch)->mGrpPassVtable = PEOPLE_GRP_PASS_VTBL;            /* ~dBgS_ObjAcch */
        peopleAcchVtables(&p->mObjAcch)->mPolyPassVtable = PEOPLE_POLY_PASS_VTBL;
        peopleAcchDestructor(&p->mObjAcch, 0);
        peopleActorDestructor(p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) {
            operator_delete(p);
        }
    }
}

/* 022C885C */
static void SafeString_assureTerminationImpl(void*) {
}


/* WWHD glue
ext 02525FE4 cPhs_State dComLbG_PhaseHandler(request_of_phase_process_class* p, u32 tbl, void* self)
ext 025A1458 void fopNpc_npc_c_ct(void* p)
ext 025E7820 void mDoExt_btpAnm_ct(void* p)
ext 0259E778 void dNpc_PathRun_getPoint(dNpc_PathRun_l* r, cXyz* out, u8 idx)
ext 0259EB60 BOOL dNpc_PathRun_incIdxLoop(dNpc_PathRun_l* r)
ext 025BD71C void dSnap_Obj_ct(dSnap_Obj_l* o)
ext 025BEB90 void dSnap_Obj_SetInf(dSnap_Obj_l* o, u8 photoNo, fopAc_ac_c* a, u8 b, u8 c, s16 cull)
ext 025BEB5C void dSnap_Obj_SetGeo(dSnap_Obj_l* o, cXyz* c, f32 r, f32 h, s16 ang)
ext 025BEB4C void dSnap_RegistSnapObj(dSnap_Obj_l* o)
ext 02554288 s32 dKy_get_dayofweek()
ext 0259E6D0 bool dNpc_PathRun_setInf(dNpc_PathRun_l* r, u8 idx, s8 room, u8 fwd)
ext 025D9F38 PeopleMainPig* fopAcM_searchFromName_l(const char* name, u32 mask, u32 prm)
ext 02720144 PeopleMainPicture* peoplePictureData(void* save)
ext 027DF9B0 s8 peopleJointNameIndex(JUTNameTab* tab, const char* name)
ext 02515A70 void peopleCylinderDestructor(dCcD_Cyl* p, s32 flags)
ext 02515860 void peopleSttsDestructor(dCcD_Stts* p, s32 flags)
ext 02018034 void peopleCircleDestructor(cM3dGCir_l* p, s32 flags)
ext 024EFD9C void peopleAcchDestructor(dBgS_ObjAcch* p, s32 flags)
ext 025D50BC void peopleActorDestructor(fopAc_ac_c* p, s32 flags)
ext 028ED6F8 void peopleHeaderObjectOneCtor(u8* p)
ext 028EAB2C void peopleHeaderObjectTwoCtor(u8* p)
data l_photo_no 10020158
data l_bmd_ix_tbl 1002016C
data l_head_bmd_ix_tbl 100201B8
data l_btp_ix_tbl 10020204
data l_head_diff_flag_tbl 10020250
data l_etc_bmd_ix_tbl 1002029C
data l_method 101C4C5C nocheck
data l_headerStatics 10468314 nocheck
data l_registrations 101C4CAC nocheck
str 10020354 "d_a_npc_people.cpp" in 022BE1E8
str 10020338 "m_head_tex_pattern != (0)"
str 10020420 "d_a_npc_people.cpp" in 022BEB7C
str 100203EC "mNpcNo >= 0 && mNpcNo < daNpcPeople_c::Npc_No_Max" in 022BEB7C
str 1002047C "d_a_npc_people.cpp" in 022BEEDC
str 10020448 "mNpcNo >= 0 && mNpcNo < daNpcPeople_c::Npc_No_Max" in 022BEEDC
str 10020490 "DEFAULT_TALK_XY"
str 10020374 "head"
str 1002037C "d_a_npc_people.cpp" in 022BE334
str 10020390 "m_jnt.getHeadJntNum() >= 0"
str 100203AC "backbone"
str 100203B8 "m_jnt.getBackboneJntNum() >= 0"
str 1002036C "handR"
str 100204F4 "UO1_GET_ITEM"
str 10020504 "UB1_TALK"
str 100204DC "UB1_TALK_XY"
str 10020510 "UB1_TALK_PHOTO"
str 100204C4 "UB1_TALK_PHOTO_GET_ITEM"
str 10020520 "UB4_GET_ITEM"
str 10020530 "UW2_GET_ITEM"
str 10020540 "UM1_GET_ITEM"
str 10020550 "UM1_TALK"
str 1002055C "UM3_TELESCOPE_TALK"
str 10020570 "UM3_GET_ITEM"
str 10020580 "SA3_GET_ITEM"
str 100204E8 "SA5_TALK_XY"
str 10020590 "SA5_GET_ITEM"
str 100205A0 "UG1_TALK"
str 100203E0 "Ocmera"
str 100204C0 "Pig"
*/
