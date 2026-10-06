/**
 * d_a_npc_ah.cpp (WWHD)
 * NPC - Old Man Ho Ho
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_ah.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_ah.h"

#define SAFESTRING_VTBL 0x10015EC4 /* this TU's sead::SafeString vtable */
#define AH_VTBL 0x10015FE4         /* daNpcAh_c vtable (HD virtual destructor) */

/* ---- local bindings (SHARED-CANDIDATE; as in d_a_npc_sv.cpp) ---- */
/* 025A1458 fopNpc_npc_c::fopNpc_npc_c (matcher: cDyl_LinkASync) */
static inline void fopNpc_npc_c_ct(void* p) { gabi::call(0x025A1458, p); }
/* 02525FE4 dComLbG_PhaseHandler(request_of_phase_process_class*, cPhs__Handler* table, void* user) */
static inline cPhs_State dComLbG_PhaseHandler(request_of_phase_process_class* p, u32 tbl, void* self) {
    return gabi::call<cPhs_State>(0x02525FE4, p, tbl, self);
}
/* 0259D624 dNpc_calc_DisXZ_AngY(cXyz, cXyz (pointers to copies), f32* dist, s16* angle) */
static inline void dNpc_calc_DisXZ_AngY(cXyz* a, cXyz* b, be<f32>* dist, be<s16>* ang) { gabi::call(0x0259D624, a, b, dist, ang); }
/* 025D7DEC fopAcM_createItemForPresentDemo(pos, itemNo, argFlag, itemBitNo, roomNo, angle, scale) */
static inline s32 fopAcM_createItemForPresentDemo(cXyz* pos, s32 itemNo, u8 flag, s32 bitNo, s32 roomNo, csXyz* angle, cXyz* scale) {
    return gabi::call<s32>(0x025D7DEC, pos, itemNo, flag, bitNo, roomNo, angle, scale);
}
/* 025F74D0 HD message manager setStatus(status) */
static inline void fopMsgM_setStatus(u32 mng, u32 st) { gabi::call(0x025F74D0, mng, st); }
/* dComIfGp_event_setItemPartnerId: dEvt_control_c mPtItem (play + 0x52A0) */
static inline void dComIfGp_event_setItemPartnerId(s32 id) { gabi::store<s32>(dComIfGp_ea() + 0x52A0, id); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
/* dEvt_control_c::chkPhoto(): play + 0x52B2 (GameCube mbInPhoto) */
static inline BOOL dComIfGp_event_chkPhoto() { return gabi::load<u8>(dComIfGp_ea() + 0x52B2) != 0; }
/* dComIfGp_getMesgAnimeAttrInfo / clear: play + 0x5BC5 */
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + 0x5BC5); }
static inline void dComIfGp_clearMesgAnimeAttrInfo() { gabi::store<u8>(dComIfGp_ea() + 0x5BC5, 0xFF); }
/* GHS pointer-to-member call (no argument) returning r3 */
static inline s32 ptmf_call_r(u32 entry, void* self) {
    s16 delta = gabi::load<s16>(entry);
    s16 idx = gabi::load<s16>(entry + 2);
    void* p = gabi::at<void>(gabi::ea(self) + delta);
    if (idx < 0) return gabi::call_ptr<s32>(gabi::load<u32>(entry + 4), p);
    u32 vt = gabi::load<u32>(gabi::ea(p) + gabi::load<s16>(entry + 6));
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + idx * 8 + 4), p);
}
/* virtuals of the actor vtable (+0xB4): next_msgStatus +0x14 (getMsg +0x1C, anmAtr +0x24 in d_a_npc_ob1.h) */
static inline u32 vcall_next_msgStatus(fopNpc_npc_c_l* a, be<u32>* p) { return gabi::call_ptr<u32>(gabi::load<u32>(a->__vtbl + 0x14), a, p); }
/* a float copied with lfs/stfs (bit exact in the recompiled code): a word copy */
static inline void wcopy(void* dst, const void* src) { gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(gabi::ea(src))); }
static inline void wcopy3(cXyz* dst, const cXyz* src) {
    wcopy(&dst->x, &src->x);
    wcopy(&dst->y, &src->y);
    wcopy(&dst->z, &src->z);
}

/* ---- file statics (.data / .rodata) ---- */
static inline const char* l_arcname() { return STR(gabi::load<u32>(0x101BB304)); }      /* l_arcname_tbl[0] "Ah" */
static inline const char* l_npc_staff_id() { return STR(gabi::load<u32>(0x101BB300)); } /* "Ah" */
static inline s32 l_bmd_ix() { return gabi::load<s32>(0x10015EBC); }
static inline s32 l_bck_ix(u32 i) { return gabi::load<s32>(0x10015EB0 + i * 4); }
static inline NpcDatStruct* l_npc_dat() { return gabi::at<NpcDatStruct>(0x101BB378); }
#define l_npc_anm_wait gabi::at<sAhAnmDat>(0x101BB308)
#define l_npc_anm_wait2 gabi::at<sAhAnmDat>(0x101BB30B)
#define l_msg_ah_tbl 0x101BB310u   /* u32* [10] */
#define l_execute_init 0x101BB338u /* executeWaitInit, executeTalkInit (pointers to members) */
#define moveProc 0x101BB348u       /* executeWait, executeTalk */
#define l_method 0x101BB3CCu       /* phase_1, phase_2 */
#define cut_name_tbl 0x101BB3D8u   /* "MES_SET", "GET_ITEM" */

/* 021E4E8C */
static BOOL da_Npc_Ah_nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x021E4E8C, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model = gabi::at<J3DModel>(j3dSys_getModel());
        daNpcAh_c* i_this = gabi::at<daNpcAh_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        u32 jointNo = gabi::load<u16>(gabi::ea(J3DNode_toJoint(node)) + 4);
        PSMTXCopy(J3DModel_getAnmMtx(model, jointNo), calc_mtx());
        if (jointNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
            cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[0][1]);
            cMtx_ZrotM(calc_mtx(), (s16)-i_this->m_jnt.mAngles[0][0]);
        } else if (jointNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
            cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]);
            cMtx_ZrotM(calc_mtx(), (s16)-i_this->m_jnt.mAngles[1][0]);
        }
        mtx_copy(J3DModel_getAnmMtx(model, jointNo), calc_mtx()); /* MTXCopy(*calc_mtx, model->getAnmMtx(jointNo)) */
        PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx);
    }
    return TRUE;
}
VERIFY(0x021E4E8C, da_Npc_Ah_nodeCallBack);

/* 021E4FE8 */
BOOL daNpcAh_c::initTexPatternAnm(u32 modify) {
    WWHD_FUNC(0x021E4FE8, BOOL, this, modify);
    u32 modelData = J3DModel_modelData(mpMorf->getModel());
    /* l_btp_ix_tbl[0] (4) as an immediate */
    m_head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectIDRes(l_arcname(), 4, SAFESTRING_VTBL);
    if (m_head_tex_pattern == nullptr) /* JUT_ASSERT(0x67D, m_head_tex_pattern != NULL) */
        JUT_ASSERT_fail(STR(0x10015F1C), 0x67D, STR(0x10015F00));
    BOOL ret = mDoExt_btpAnm_init(mBtpAnm, modelData, m_head_tex_pattern, 1, 2, 1.0f, 0, -1, modify, FALSE);
    if (ret == FALSE) return FALSE;
    mBtpFrame = 0;
    mTimer = 0;
    return TRUE;
}
VERIFY(0x021E4FE8, &daNpcAh_c::initTexPatternAnm);

/* 021E5394 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021E5394, BOOL, i_this);
    return ((daNpcAh_c*)i_this)->createHeap();
}
VERIFY(0x021E5394, CheckCreateHeap);

/* 021E6F54 */
static u32 daObj_PrmAbstract(fopAc_ac_c* ac, s32 width, s32 shift) {
    WWHD_FUNC(0x021E6F54, u32, ac, width, shift);
    u32 prm = ac->mParameters;
    /* PowerPC slw/srw: shift amounts of 32..63 give 0 */
    u32 bit = (width & 0x20) ? 0 : (1u << (width & 0x1F));
    u32 v = (shift & 0x20) ? 0 : (prm >> (shift & 0x1F));
    return v & (bit - 1);
}
VERIFY(0x021E6F54, daObj_PrmAbstract);

/* 021E5398 */
daNpcAh_c* daNpcAh_c::ct(daNpcAh_c* p) {
    WWHD_FUNC(0x021E5398, daNpcAh_c*, p);
    if (p == nullptr) {
        p = (daNpcAh_c*)operator_new(0x8C8);
        if (p == nullptr) return nullptr;
    }
    fopNpc_npc_c_ct(p);
    p->__vtbl = AH_VTBL;
    gabi::call(0x025E7820, p->mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
    p->mBckIdx = 0;
    p->field_0x747 = 0; /* setResFlag(0) */
    p->field_0x71C = -1.0f;
    p->field_0x736 = 0;
    p->field_0x732 = p->home.angle.y;
    p->field_0x718 = 0.0f;
    p->mHeadOnlyFollow = 1;
    p->mMoveState = 0;
    p->field_0x74E = 0;
    p->field_0x72C = 0;
    return p;
}
VERIFY(0x021E5398, &daNpcAh_c::ct);

/* 021E5438 */
u8 daNpcAh_c::getPrmArg0() {
    WWHD_FUNC(0x021E5438, u8, this);
    u8 ret = (u8)daObj_PrmAbstract(this, 8 /* PRM_SWSAVE_S */, 0 /* PRM_SWSAVE_W */);
    if (ret >= 10) ret = 0;
    return ret;
}
VERIFY(0x021E5438, &daNpcAh_c::getPrmArg0);

/* 021E5470 */
u8 daNpcAh_c::getSwBit() {
    WWHD_FUNC(0x021E5470, u8, this);
    return (u8)daObj_PrmAbstract(this, 8 /* PRM_SWSAVE2_S */, 8 /* PRM_SWSAVE2_W */);
}
VERIFY(0x021E5470, &daNpcAh_c::getSwBit);

/* 021E549C */
static cPhs_State phase_1(daNpcAh_c* i_this) {
    WWHD_FUNC(0x021E549C, cPhs_State, i_this);
    /* fopAcM_ct(i_this, daNpcAh_c) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) daNpcAh_c::ct(i_this);
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    switch (i_this->getPrmArg0()) {
    case 2:
        if (dComIfGs_isSwitch(0x6C, i_this->home.roomNo) /* fopAcM_isSwitch: home room */) {
            return 3; /* cPhs_STOP_e */
        }
        if (!dComIfGs_isEventBit(0x0520 /* UNK_0520 */)) {
            return 3;
        }
        break;
    case 5:
        if (dComIfGs_isSwitch(0x10, i_this->home.roomNo) /* fopAcM_isSwitch: home room */) {
            return 3;
        }
        break;
    case 9:
        if (dComIfGs_isSwitch(i_this->getSwBit(), i_this->home.roomNo) /* fopAcM_isSwitch: home room */) {
            return 3;
        }
        break;
    }
    i_this->field_0x747 = 1; /* setResFlag(1) */
    return 2; /* cPhs_NEXT_e */
}
VERIFY(0x021E549C, phase_1);

/* 021E5AD8 */
static cPhs_State phase_2(daNpcAh_c* i_this) {
    WWHD_FUNC(0x021E5AD8, cPhs_State, i_this);
    cPhs_State state = dComIfG_resLoad(&i_this->mPhs, l_arcname());
    if (state == 4 /* cPhs_COMPLEATE_e */) {
        if (fopAcM_entrySolidHeap(i_this, 0x021E5394 /* CheckCreateHeap */, 0x2F00)) {
            state = i_this->createInit();
        } else {
            i_this->mpMorf = nullptr;
            return 5; /* cPhs_ERROR_e */
        }
    }
    return state;
}
VERIFY(0x021E5AD8, phase_2);

/* 021E5B60 */
cPhs_State daNpcAh_c::_create() {
    WWHD_FUNC(0x021E5B60, cPhs_State, this);
    return dComLbG_PhaseHandler(&mPhsMethod, l_method, this);
}
VERIFY(0x021E5B60, &daNpcAh_c::_create);

/* 021E50E8 */
BOOL daNpcAh_c::createHeap() {
    WWHD_FUNC(0x021E50E8, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectIDRes(l_arcname(), l_bmd_ix(), SAFESTRING_VTBL);
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectIDRes(l_arcname(), l_bck_ix(mBckIdx), SAFESTRING_VTBL);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2 /* J3DFrameCtrl::EMode_LOOP */, 1.0f, 0, -1, 1,
                                    nullptr, 0x80000, 0x11020022);
    if (mpMorf == nullptr || mpMorf->mpModel == nullptr) return FALSE;
    m_jnt.mHeadJntNum = (s8)J3DModelData_getJointIndex(modelData, STR(0x10015F30) /* "head" */);
    if (m_jnt.mHeadJntNum < 0) /* JUT_ASSERT(0x2BC, m_jnt.getHeadJntNum() >= 0) */
        JUT_ASSERT_fail(STR(0x10015F38), 0x2BC, STR(0x10015F48));
    m_jnt.mBackboneJntNum = (s8)J3DModelData_getJointIndex(modelData, STR(0x10015F64) /* "backbone" */);
    if (m_jnt.mBackboneJntNum < 0) /* JUT_ASSERT(0x2C0, m_jnt.getBackboneJntNum() >= 0) */
        JUT_ASSERT_fail(STR(0x10015F38), 0x2C0, STR(0x10015F70));
    if (initTexPatternAnm(false) == FALSE) return FALSE;
    for (u16 jntIdx = 0; jntIdx < J3DModelData_getJointNum(modelData); jntIdx++) {
        if (jntIdx == m_jnt.mHeadJntNum || jntIdx == m_jnt.mBackboneJntNum) {
            J3DModelData_setJointCallBack(gabi::ea(modelData), jntIdx, 0x021E4E8C /* da_Npc_Ah_nodeCallBack */);
        }
    }
    gabi::store<u32>(gabi::ea(mpMorf->mpModel.get()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    mAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    return TRUE;
}
VERIFY(0x021E50E8, &daNpcAh_c::createHeap);

/* 021E55D8 */
void daNpcAh_c::setAnm(u8 bck_ix, int loopMode, f32 morf) {
    WWHD_FUNC(0x021E55D8, void, this, bck_ix, loopMode, morf);
    f32 tempMorf = field_0x71C;
    if (!(tempMorf < 0.0f)) {
        morf = tempMorf;
        field_0x71C = -1.0f;
    }
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(l_arcname(), l_bck_ix(bck_ix), SAFESTRING_VTBL);
    mpMorf->setAnm(anm, loopMode, morf, 1.0f, 0.0f, -1.0f, nullptr);
    mBckIdx = bck_ix;
}
VERIFY(0x021E55D8, &daNpcAh_c::setAnm);

/* 021E56E4 */
BOOL daNpcAh_c::setAnmTbl(sAhAnmDat* i_anmDat) {
    WWHD_FUNC(0x021E56E4, BOOL, this, i_anmDat);
    if (i_anmDat->mBckIdx == 0xFF) {
        mpAnmDat = nullptr;
        return TRUE;
    }
    mpAnmDat = i_anmDat;
    s8 n = i_anmDat->field_0x02;
    field_0x74B = n;
    u8 bck = i_anmDat->mBckIdx;
    if (n > 0) {
        setAnm(bck, 0 /* J3DFrameCtrl::EMode_NONE */, (f32)i_anmDat->mMorf);
    } else if (mBckIdx != bck) {
        setAnm(bck, 2 /* J3DFrameCtrl::EMode_LOOP */, (f32)i_anmDat->mMorf);
    }
    return FALSE;
}
VERIFY(0x021E56E4, &daNpcAh_c::setAnmTbl);

/* 021E57B0 */
void daNpcAh_c::setMtx() {
    WWHD_FUNC(0x021E57B0, void, this);
    J3DModel* model = mpMorf->getModel();
    J3DModel_setBaseScale(model, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
}
VERIFY(0x021E57B0, &daNpcAh_c::setMtx);

/* 021E5890 */
void daNpcAh_c::setCollision(dCcD_Cyl* cyl, cXyz* center, f32 radius, f32 height) {
    WWHD_FUNC(0x021E5890, void, this, cyl, center, radius, height);
    cyl->SetC(center);
    cyl->SetR(radius);
    cyl->SetH(height);
    dComIfG_Ccsp_Set(cyl);
}
VERIFY(0x021E5890, &daNpcAh_c::setCollision);

/* 021E5920 */
cPhs_State daNpcAh_c::createInit() {
    WWHD_FUNC(0x021E5920, cPhs_State, this);
    gravity = -9.0f;
    setAnmTbl(l_npc_anm_wait);
    setActorInfo2(&mEventCut, l_npc_staff_id(), this);
    mLookAtMaxVel = 0;
    field_0x743 = 0;
    field_0x742 = 0;
    field_0x751 = 0;
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -70.0f, 0.0f, -70.0f, 70.0f, 200.0f, 70.0f);
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xA7); /* attention_info.distances[fopAc_Attn_TYPE_TALK_e] */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xA9); /* attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] */
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0x0100000A); /* attention_info.flags: LOCKON_TALK | ACTION_SPEAK | UNK1000000 */
    NpcDatStruct* dat = l_npc_dat();
    m_jnt.setParam(dat->mMax_backbone_x, dat->mMax_backbone_y, dat->mMin_backbone_x, dat->mMin_backbone_y, dat->mMax_head_x,
                   dat->mMax_head_y, dat->mMin_head_x, dat->mMin_head_y, dat->mMax_turn_step);
    field_0x74F = (u8)dat->field_0x52;
    field_0x750 = (u8)dat->field_0x53;
    wcopy(&field_0x720, &dat->field_0x28);
    field_0x730 = dat->field_0x30;
    mObjAcch.CrrPos(dComIfG_Bgsp());
    /* HD (as GameCube after the demo version): matrix, collision */
    setMtx();
    J3DModel_calc(mpMorf->getModel());
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mCyl.SetStts(&mStts);
    gabi::Local<cXyz> center;
    wcopy3(center, &current.pos);
    setCollision(&mCyl, center, dat->field_0x38, 150.0f);
    return 4; /* cPhs_COMPLEATE_e */
}
VERIFY(0x021E5920, &daNpcAh_c::createInit);

/* 021E5B74 */
static cPhs_State daNpc_AhCreate(void* i_this) {
    WWHD_FUNC(0x021E5B74, cPhs_State, i_this);
    return ((daNpcAh_c*)i_this)->_create();
}
VERIFY(0x021E5B74, daNpc_AhCreate);

/* 021E5B78 */
BOOL daNpcAh_c::_delete() {
    WWHD_FUNC(0x021E5B78, BOOL, this);
    if (field_0x747 != 0) {
        dComIfG_resDelete(&mPhs, l_arcname()); /* HD: dComIfG_resDelete (GameCube dComIfG_resDeleteDemo) */
    }
    if (heap != nullptr && mpMorf != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x021E5B78, &daNpcAh_c::_delete);

/* 021E5BDC */
static BOOL daNpc_AhDelete(void* i_this) {
    WWHD_FUNC(0x021E5BDC, BOOL, i_this);
    return ((daNpcAh_c*)i_this)->_delete();
}
VERIFY(0x021E5BDC, daNpc_AhDelete);

/* 021E5BE0 */
void daNpcAh_c::chkAttention() {
    WWHD_FUNC(0x021E5BE0, void, this);
    NpcDatStruct* dat = l_npc_dat();
    field_0x751 = 0;
    if (mEventCut.mbAttention != 0) { /* mEventCut.getAttnFlag() */
        wcopy3(&mEyePos, &mEventCut.mPos);
        field_0x74E = 1;
        if (field_0x74F != 0) {
            mHeadOnlyFollow = 0;
            m_jnt.mbTrn = 1;
        } else {
            mHeadOnlyFollow = 1;
        }
        if (field_0x743 == 0) {
            field_0x743 = 1;
        }
    } else {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        gabi::Local<cXyz> pos;
        wcopy3(pos, &current.pos);
        gabi::Local<cXyz> plPos;
        wcopy3(plPos, &player->current.pos);
        f32 temp720 = field_0x720;
        s32 temp730 = field_0x730;
        gabi::Local<be<f32>> distXZ;
        gabi::Local<be<s16>> angle;
        dNpc_calc_DisXZ_AngY(pos, plPos, distXZ, angle);
        u8 attn = field_0x743;
        if (attn != 0) {
            temp720 = temp720 + 40.0f;
            temp730 += 0x71C;
        }
        s16 ang = (s16)(*angle - shape_angle.y);
        *angle = ang;
        f32 dist = *distXZ;
        s32 absAng = ang < 0 ? -ang : ang;
        if (temp720 > dist && temp730 > absAng) {
            gabi::Local<cXyz> eye;
            dNpc_playerEyePos_l(eye, dat->field_0x14);
            wcopy3(&mEyePos, eye);
            mHeadOnlyFollow = field_0x74F == 0;
            field_0x74E = 1;
            if (field_0x750 == 0) {
                mTargetYRot = field_0x732;
                mHeadOnlyFollow = 0;
                field_0x74E = 2;
                m_jnt.mbTrn = 1;
            }
            if (field_0x743 == 0) {
                field_0x743 = 1;
            }
        } else {
            if (attn == 1) {
                field_0x743 = 0;
                field_0x72E = dat->field_0x50;
            }
            if (dat->field_0x2C > dist) {
                gabi::Local<cXyz> eye;
                dNpc_playerEyePos_l(eye, dat->field_0x14);
                wcopy3(&mEyePos, eye);
                mHeadOnlyFollow = field_0x74F == 0;
                field_0x74E = 1;
                if (field_0x750 == 0) {
                    mTargetYRot = field_0x732;
                    mHeadOnlyFollow = 0;
                    field_0x74E = 2;
                    m_jnt.mbTrn = 1;
                }
                field_0x751 = 1;
            } else {
                u32 path = gabi::ea(mPathRun.mPath.get());
                field_0x74E = 0;
                if (path == 0) { /* !mPathRun.isPath() */
                    if (field_0x72E != 0) {
                        field_0x72E = field_0x72E - 1;
                    } else {
                        mTargetYRot = field_0x732;
                        mHeadOnlyFollow = 0;
                        field_0x74E = 2;
                        m_jnt.mbTrn = 1;
                    }
                }
            }
        }
    }
    mTargetAngle = dat->field_0x32;
}
VERIFY(0x021E5BE0, &daNpcAh_c::chkAttention);

/* 021E5EB0 */
void daNpcAh_c::executeSetMode(u8 proc) {
    WWHD_FUNC(0x021E5EB0, void, this, proc);
    field_0x718 = 0.0f;
    mMoveState = (u8)ptmf_call_r(l_execute_init + proc * 8, this);
}
VERIFY(0x021E5EB0, &daNpcAh_c::executeSetMode);

/* 021E5F38 */
void daNpcAh_c::checkOrder() {
    WWHD_FUNC(0x021E5F38, void, this);
    u16 cmd = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (cmd == 2 /* checkCommandDemoAccrpt */) {
        return;
    }
    if (cmd == 1 /* checkCommandTalk */ && (field_0x744 == 2 || field_0x744 == 1)) {
        field_0x742 = 1;
        executeSetMode(1);
    }
}
VERIFY(0x021E5F38, &daNpcAh_c::checkOrder);

/* 021E5F6C */
void daNpcAh_c::setMessage(u32 msgNo) {
    WWHD_FUNC(0x021E5F6C, void, this, msgNo);
    mCurrMsgNo = msgNo;
}
VERIFY(0x021E5F6C, &daNpcAh_c::setMessage);

/* 021E5F74 */
void daNpcAh_c::eventMesSetInit(int staffIdx) {
    WWHD_FUNC(0x021E5F74, void, this, staffIdx);
    be<u32>* pData = (be<u32>*)dComIfGp_evmng_getMySubstanceP(staffIdx, STR(0x10015FB8) /* "MsgNo" */, 3 /* integer */);
    if (pData != nullptr) {
        mpMsgNo = 0;
        u32 msg = *pData;
        switch (msg) {
        case 1:
            return;
        case 0:
            msg = vcall_getMsg(this);
            setMessage(msg);
            break;
        default:
            setMessage(msg);
            break;
        }
        if (mpMsgNo != 0) {
            setMessage(gabi::load<u32>(mpMsgNo));
        }
    } else {
        mpMsgNo = mpMsgNo + 4;
        setMessage(gabi::load<u32>(mpMsgNo));
    }
}
VERIFY(0x021E5F74, &daNpcAh_c::eventMesSetInit);

/* 021E6030 */
void daNpcAh_c::eventGetItemInit() {
    WWHD_FUNC(0x021E6030, void, this);
    s32 procItem = fopAcM_createItemForPresentDemo(&current.pos, mItemNo, 0, -1, -1, nullptr, nullptr);
    if (procItem != -1 /* fpcM_ERROR_PROCESS_ID_e */) {
        dComIfGp_event_setItemPartnerId(procItem);
    }
}
VERIFY(0x021E6030, &daNpcAh_c::eventGetItemInit);

/* 021E6088 */
u16 daNpcAh_c::talk2(int i_param) {
    WWHD_FUNC(0x021E6088, u16, this, i_param);
    u32 mng = l_msgMng();
    u16 msg_status = 0xFF;
    if (mCurrMsgBsPcId == 0xFFFFFFFF) {
        if (i_param == 1) mCurrMsgNo = vcall_getMsg(this);
        mCurrMsgBsPcId = fopMsgM_messageSet(mng, mCurrMsgNo, &eyePos);
        /* HD: the message is reached through the manager; the flag replaces mpCurrMsg */
        if (mCurrMsgBsPcId != 0xFFFFFFFF) {
            mbCurrMsg = 0;
            field_0x734 = 0xFFFF;
        }
    } else if (mbCurrMsg != 0) {
        msg_status = (u16)fopMsgM_SearchByID(mng);
        switch (msg_status) {
        case 0xE: /* fopMsgStts_MSG_DISPLAYED_e */
            fopMsgM_setStatus(mng, vcall_next_msgStatus(this, &mCurrMsgNo));
            if (fopMsgM_SearchByID(mng) == 0xF /* fopMsgStts_MSG_CONTINUES_e */) {
                fopMsgM_messageSet(mng, mCurrMsgNo, nullptr);
            }
            break;
        /* HD: no fopMsgStts_MSG_TYPING_e case (GameCube chkMsg, empty) */
        case 0x12: /* fopMsgStts_BOX_CLOSED_e */
            fopMsgM_setStatus(mng, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
            mCurrMsgBsPcId = 0xFFFFFFFF;
            break;
        }
        field_0x734 = msg_status;
        vcall_anmAtr(this, msg_status);
    } else {
        mbCurrMsg = 1; /* mpCurrMsg = fopMsgM_SearchByID(mCurrMsgBsPcId) */
    }
    return msg_status;
}
VERIFY(0x021E6088, &daNpcAh_c::talk2);

/* 021E623C */
BOOL daNpcAh_c::eventMesSet() {
    WWHD_FUNC(0x021E623C, BOOL, this);
    u16 talkVal = talk2(0);
    if (talkVal == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        u8 f = field_0x748;
        if ((f & 1) != 0) {
            field_0x748 = f & ~1;
            mItemNo = 7;
            field_0x744 = 3;
        } else if ((f & 2) != 0) {
            field_0x748 = f & ~2;
            mItemNo = 5;
            field_0x744 = 3;
        }
    }
    return talkVal == 0x12;
}
VERIFY(0x021E623C, &daNpcAh_c::eventMesSet);

/* 021E62C8 */
void daNpcAh_c::privateCut() {
    WWHD_FUNC(0x021E62C8, void, this);
    const char* name = l_npc_staff_id();
    s32 staff_idx = dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
    if (staff_idx != -1) {
        s8 act = (s8)dComIfGp_evmng_getMyActIdx(staff_idx, cut_name_tbl, 2, 1, 0);
        mActIdx = act;
        dEvent_manager_c* evtMng = dComIfGp_getPEvtManager();
        if (act == -1) {
            gabi::call(0x02543280, evtMng, staff_idx); /* cutEnd */
        } else {
            if (gabi::call<BOOL>(0x025447C8, evtMng, staff_idx) /* getIsAddvance */) {
                switch ((u32)(s32)mActIdx) {
                case 0:
                    eventMesSetInit(staff_idx);
                    break;
                case 1:
                    eventGetItemInit();
                    break;
                }
            }
            bool end;
            switch (mActIdx) {
            case 0:
                end = eventMesSet();
                break;
            default:
                end = true;
                break;
            }
            if (end) {
                dComIfGp_evmng_cutEnd(staff_idx);
            }
        }
    }
}
VERIFY(0x021E62C8, &daNpcAh_c::privateCut);

/* 021E63E8 */
void daNpcAh_c::setAnmFromMsgTag() {
    WWHD_FUNC(0x021E63E8, void, this);
    switch (dComIfGp_getMesgAnimeAttrInfo()) {
    case 0:
        setAnmTbl(l_npc_anm_wait);
        break;
    case 1:
        setAnmTbl(l_npc_anm_wait2);
        break;
    }
    dComIfGp_clearMesgAnimeAttrInfo();
}
VERIFY(0x021E63E8, &daNpcAh_c::setAnmFromMsgTag);

/* 021E6464 */
void daNpcAh_c::eventMove() {
    WWHD_FUNC(0x021E6464, void, this);
    /* chkEndEvent() is FALSE (inlined) */
    u8 attnFlag = mEventCut.mbAttention;
    if (mEventCut.cutProc()) {
        if (mEventCut.mbAttention == 0) {
            mEventCut.mbAttention = attnFlag;
        }
    } else {
        privateCut();
        setAnmFromMsgTag();
    }
}
VERIFY(0x021E6464, &daNpcAh_c::eventMove);

/* 021E64E0 */
void daNpcAh_c::eventOrder() {
    WWHD_FUNC(0x021E64E0, void, this);
    if (field_0x744 == 2 || field_0x744 == 1) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        if (field_0x744 == 2) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}
VERIFY(0x021E64E0, &daNpcAh_c::eventOrder);

/* 021E6510 */
void daNpcAh_c::playTexPatternAnm() {
    WWHD_FUNC(0x021E6510, void, this);
    if (cLib_calcTimer(&mTimer) == 0) {
        s32 frameMax = J3DAnmTexPattern_getFrameMax(m_head_tex_pattern);
        if ((s32)mBtpFrame >= frameMax) {
            s32 frameMax2 = J3DAnmTexPattern_getFrameMax(m_head_tex_pattern);
            mTimer = 0x78;
            mBtpFrame = (u8)(mBtpFrame - frameMax2);
        } else {
            mBtpFrame = mBtpFrame + 1;
        }
    }
}
VERIFY(0x021E6510, &daNpcAh_c::playTexPatternAnm);

/* 021E65B0 */
void daNpcAh_c::playAnm() {
    WWHD_FUNC(0x021E65B0, void, this);
    mDoExt_McaMorf* morf = mpMorf;
    field_0x74A = field_0x74A & ~1;
    if (morf->play(nullptr, 0, 0)) {
        if (mpAnmDat != nullptr) {
            s8 n = field_0x74B;
            if (n > 0) {
                n = (s8)(n - 1);
                field_0x74B = n;
                if (n == 0) {
                    sAhAnmDat* next = gabi::at<sAhAnmDat>(gabi::ea(mpAnmDat.get()) + 3);
                    mpAnmDat = next;
                    if (setAnmTbl(next)) {
                        field_0x74A = field_0x74A | 1;
                    }
                } else {
                    setAnm(mpAnmDat->mBckIdx, 0 /* J3DFrameCtrl::EMode_NONE */, 0.0f);
                }
            }
        }
    }
}
VERIFY(0x021E65B0, &daNpcAh_c::playAnm);

/* 021E6680 */
void daNpcAh_c::lookBack() {
    WWHD_FUNC(0x021E6680, void, this);
    s8 mode = field_0x74E;
    u32 eyeX = gabi::load<u32>(gabi::ea(&eyePos.x)); /* cXyz eyePosFollow = eyePos */
    s16 desiredYRot = current.angle.y;
    u8 headOnlyFollow = mHeadOnlyFollow;
    s16 target = mTargetAngle;
    u32 eyeZ = gabi::load<u32>(gabi::ea(&eyePos.z));
    cXyz* dstPos = nullptr;
    u32 eyeY = gabi::load<u32>(gabi::ea(&eyePos.y));
    gabi::Local<cXyz> newDes;
    switch ((u32)(s32)mode) {
    case 1:
        wcopy3(newDes, &mEyePos);
        dstPos = newDes;
        break;
    case 2:
        desiredYRot = mTargetYRot;
        break;
    }
    bool turn;
    if (field_0x742 != 0 && field_0x74F != 0) {
        headOnlyFollow = 0;
        m_jnt.mbTrn = 1;
        turn = true;
    } else {
        turn = m_jnt.mbTrn != 0; /* m_jnt.trnChk() */
    }
    gabi::Local<cXyz> eye;
    if (turn) {
        s16 turnSpeed = mEventCut.mTurnSpeed;
        if (turnSpeed != 0) target = turnSpeed;
        cLib_addCalcAngleS2(&mLookAtMaxVel, target, 4, 0x800);
        s16 vel = mLookAtMaxVel;
        gabi::store<u32>(gabi::ea(&eye->x), eyeX);
        gabi::store<u32>(gabi::ea(&eye->y), eyeY);
        gabi::store<u32>(gabi::ea(&eye->z), eyeZ);
        lookAtTarget(&m_jnt, &current.angle.y, dstPos, eye, desiredYRot, vel, headOnlyFollow);
    } else {
        mLookAtMaxVel = 0;
        gabi::store<u32>(gabi::ea(&eye->x), eyeX);
        gabi::store<u32>(gabi::ea(&eye->y), eyeY);
        gabi::store<u32>(gabi::ea(&eye->z), eyeZ);
        lookAtTarget(&m_jnt, &current.angle.y, dstPos, eye, desiredYRot, 0, headOnlyFollow);
    }
    shape_angle.x = current.angle.x;
    shape_angle.z = current.angle.z;
    shape_angle.y = current.angle.y;
}
VERIFY(0x021E6680, &daNpcAh_c::lookBack);

/* 021E688C */
BOOL daNpcAh_c::_execute() {
    WWHD_FUNC(0x021E688C, BOOL, this);
    switch (getPrmArg0()) {
    case 2:
        if (dComIfGs_isSwitch(0x6C, home.roomNo) /* fopAcM_isSwitch: home room */) fopAcM_delete(this);
        break;
    case 5:
        if (dComIfGs_isSwitch(0x10, home.roomNo) /* fopAcM_isSwitch: home room */) fopAcM_delete(this);
        break;
    case 9:
        if (dComIfGs_isSwitch(getSwBit() & 0xFF, home.roomNo) /* fopAcM_isSwitch: home room */) fopAcM_delete(this);
        break;
    }
    chkAttention();
    checkOrder();
    if (!dComIfGp_event_runCheck() || gabi::load<u16>(gabi::ea(this) + 0xF8) == 1 /* eventInfo.checkCommandTalk() */) {
        ptmf_call_r(moveProc + mMoveState * 8, this);
    } else {
        eventMove();
    }
    eventOrder();
    playTexPatternAnm();
    playAnm();
    fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
    mObjAcch.CrrPos(dComIfG_Bgsp());
    NpcDatStruct* dat = l_npc_dat();
    gabi::Local<cXyz> center;
    wcopy3(center, &current.pos);
    setCollision(&mCyl, center, dat->field_0x38, 150.0f);

    gabi::Local<cXyz> attnInfoPos;
    wcopy(&attnInfoPos->z, &dat->field_0x20);
    wcopy(&attnInfoPos->x, &dat->field_0x18);
    wcopy(&attnInfoPos->y, &dat->field_0x1C);
    cMtx_YrotS(mDoMtx_stack_c::get(), current.angle.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), attnInfoPos, attnInfoPos);
    PSVECAdd(attnInfoPos, &current.pos, attnInfoPos); /* attnInfoPos += current.pos */
    cXyz* attnPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    wcopy3(attnPos, attnInfoPos);
    f32 eyeOffs = dat->field_0x24;
    f32 y = current.pos.y;
    wcopy(&eyePos.z, &current.pos.z);
    wcopy(&eyePos.x, &current.pos.x);
    eyePos.y = gabi::fadds_ppc(y, eyeOffs);
    lookBack();
    setMtx();
    return FALSE;
}
VERIFY(0x021E688C, &daNpcAh_c::_execute);

/* 021E6B10 */
static BOOL daNpc_AhExecute(void* i_this) {
    WWHD_FUNC(0x021E6B10, BOOL, i_this);
    return ((daNpcAh_c*)i_this)->_execute();
}
VERIFY(0x021E6B10, daNpc_AhExecute);

/* 021E6B14 */
BOOL daNpcAh_c::_draw() {
    WWHD_FUNC(0x021E6B14, BOOL, this);
    J3DModel* morfModel = mpMorf->getModel();
    J3DModelData* morfModelData = gabi::at<J3DModelData>(J3DModel_modelData(morfModel));
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morfModel, &tevStr);
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, morfModelData, mBtpFrame);
    mpMorf->updateDL();
    gabi::store<u32>(gabi::ea(morfModelData) + 0x38, 0); /* mBtpAnm.remove(morfModelData) */
    /* HD: no dComIfGd_setShadow (HD shadows) */
    dSnap_RegistFig(0x7F /* DSNAP_TYPE_NPC_AH */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x021E6B14, &daNpcAh_c::_draw);

/* 021E6BBC */
static BOOL daNpc_AhDraw(void* i_this) {
    WWHD_FUNC(0x021E6BBC, BOOL, i_this);
    return ((daNpcAh_c*)i_this)->_draw();
}
VERIFY(0x021E6BBC, daNpc_AhDraw);

/* 021E6BC0 */
u8 daNpcAh_c::executeCommon() {
    WWHD_FUNC(0x021E6BC0, u8, this);
    field_0x744 = field_0x743 != 0 ? 1 : 0;
    if (field_0x742 == 1 && mMoveState != 1) executeSetMode(1);
    return field_0x742;
}
VERIFY(0x021E6BC0, &daNpcAh_c::executeCommon);

/* 021E6C20 */
BOOL daNpcAh_c::executeWaitInit() {
    WWHD_FUNC(0x021E6C20, BOOL, this);
    speedF = 0.0f;
    setAnmTbl(l_npc_anm_wait);
    NpcDatStruct* dat = l_npc_dat();
    m_jnt.setParam(dat->mMax_backbone_x, dat->mMax_backbone_y, dat->mMin_backbone_x, dat->mMin_backbone_y, dat->mMax_head_x,
                   dat->mMax_head_y, dat->mMin_head_x, dat->mMin_head_y, dat->mMax_turn_step);
    return FALSE;
}
VERIFY(0x021E6C20, &daNpcAh_c::executeWaitInit);

/* 021E6CA0 */
void daNpcAh_c::executeWait() {
    WWHD_FUNC(0x021E6CA0, void, this);
    executeCommon();
}
VERIFY(0x021E6CA0, &daNpcAh_c::executeWait);

/* 021E6CA4 */
BOOL daNpcAh_c::executeTalkInit() {
    WWHD_FUNC(0x021E6CA4, BOOL, this);
    return TRUE;
}
VERIFY(0x021E6CA4, &daNpcAh_c::executeTalkInit);

/* 021E6CAC */
void daNpcAh_c::executeTalk() {
    WWHD_FUNC(0x021E6CAC, void, this);
    executeCommon();
    if (talk2(1) == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        field_0x742 = 0;
        executeSetMode(0);
        dComIfGp_event_reset();
    } else {
        setAnmFromMsgTag();
    }
}
VERIFY(0x021E6CAC, &daNpcAh_c::executeTalk);

/* 021E6D24 */
u16 daNpcAh_c::next_msgStatus(be<u32>* pMsgNo) {
    WWHD_FUNC(0x021E6D24, u16, this, pMsgNo);
    u16 ret = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    if (mpMsgNo != 0) {
        mpMsgNo = mpMsgNo + 4;
        u32 next = gabi::load<u32>(mpMsgNo);
        switch (next) {
        case 0:
            mpMsgNo = 0;
            ret = 0x10; /* fopNpc_npc_c::next_msgStatus(pMsgNo) inlined: fopMsgStts_MSG_ENDS_e */
            break;
        default:
            *pMsgNo = next;
            break;
        }
    } else {
        ret = 0x10; /* fopNpc_npc_c::next_msgStatus(pMsgNo) inlined */
    }
    return ret;
}
VERIFY(0x021E6D24, &daNpcAh_c::next_msgStatus);

/* 021E6D6C */
u32 daNpcAh_c::getMsg() {
    WWHD_FUNC(0x021E6D6C, u32, this);
    u32 ret = 0;
    mpMsgNo = 0;
    if (!dComIfGp_event_chkPhoto()) {
        if (!dComIfGp_event_chkTalkXY()) {
            u8 index = getPrmArg0();
            mpMsgNo = gabi::load<u32>(l_msg_ah_tbl + index * 4);
        }
    }
    if (mpMsgNo != 0) ret = gabi::load<u32>(mpMsgNo);
    return ret;
}
VERIFY(0x021E6D6C, &daNpcAh_c::getMsg);

/* 021E6E04 */
static void __sinit_d_a_npc_ah_cpp() {
    WWHD_FUNC(0x021E6E04, void, (u32)0);
    sinit_header_statics(0x104659B8, 0x101BB3E0);
}
VERIFY(0x021E6E04, __sinit_d_a_npc_ah_cpp);

/* 021E6E98: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021E6E98, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x021E6E98, SafeString_dt);

/* 021E6EAC */
static BOOL daNpc_AhIsDelete(void*) {
    WWHD_FUNC(0x021E6EAC, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021E6EAC, daNpc_AhIsDelete);

/* 021E6EB4: daNpcAh_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpcAh_dt(daNpcAh_c* p, s32 flags) {
    WWHD_FUNC(0x021E6EB4, void, p, flags);
    if (p != nullptr) {
        dCcD_Cyl_dt(&p->mCyl, 2);
        dCcD_Stts_dt(&p->mStts, 2);
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&p->mAcchCir) + 0x14), 2); /* ~cM3dGCir */
        gabi::store<u32>(gabi::ea(&p->mObjAcch) + 0x20, 0x10015EDC);            /* ~dBgS_ObjAcch */
        gabi::store<u32>(gabi::ea(&p->mObjAcch) + 0x14, 0x10015EEC);
        gabi::call(0x024EFD9C, &p->mObjAcch, 0);
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) operator_delete(p);
    }
}
VERIFY(0x021E6EB4, daNpcAh_dt);

/* 021E6F50: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x021E6F50, void, (u32)0);
}
VERIFY(0x021E6F50, SafeString_assureTerminationImpl);
