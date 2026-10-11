/**
 * d_a_npc_photo.cpp (WWHD)
 * NPC - Lenzo
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_photo.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * This part: construction, heap, create phases, collision, small accessors and the
 * compiler-generated functions. The other parts: d_a_npc_photo_*.cpp.
 */
#include "d/actor/d_a_npc_photo.h"

// HD: the four source parts share this one translation-unit vtable configuration.
const gc_tu_data gc_tu = {
    .safeString = PHOTO_SAFESTRING_VTBL,
    .objAcchDt = {0x10020CB8, 0x10020CC8},
    .gndChk = {0x10020C78, 0x10020C88, 0x10020CA8, 0x10020C98},
    .gndChkDt = {0x10020C88, 0x10020CA8, 0x10020C68},
    .headerStatics = {0x104683A8, 0x101C52EC, 0x104683CC},
};

static int l_bck_ix_tbl[5] = {
    dRes_ID_PO_BCK_WAIT01_e,
    dRes_ID_PO_BCK_TALK01_e,
    dRes_ID_PO_BCK_TALK02_e,
    dRes_ID_PO_BCK_WALK01_e,
    dRes_ID_PO_BCK_SPIT_e,
};
static int l_btp_ix_tbl[2] = {dRes_ID_PO_BTP_MABA01_e, dRes_ID_PO_BTP_MABA02_e};
static sPhotoAnmDat l_npc_anm_wait[1] = {{0, 8, 0xFF}};
static sPhotoAnmDat l_npc_anm_talk[1] = {{1, 8, 0xFF}};
static sPhotoAnmDat l_npc_anm_walk[1] = {{3, 8, 0xFF}};
static sPhotoAnmDat l_npc_anm_spit[2] = {{4, 8, 1}, {0, 8, 0xFF}};
static sPhotoAnmDat l_npc_anm_talk2[2] = {{2, 8, 3}, {0, 8, 0xFF}};


static u32 l_msg_1st_talk[4] = {0x2a31, 0x2a32, 0x2a33, 0x0};
static u32 l_msg_2nd_talk[2] = {0x2a34, 0x0};
static u32 l_msg_1st_photo[3] = {0x2a35, 0x2a36, 0x0};
static u32 l_msg_2nd_photo[3] = {0x2a37, 0x2a38, 0x0};
static u32 l_msg_1st_order[11] = {0x2a39, 0x2a3a, 0x2a3c, 0x2a3d, 0x2a3e, 0x2a3f, 0x2a40, 0x2a41, 0x2a42, 0x2a43, 0x1};
static s16 l_msg_1st_order_c[5] = {-1, -256, -255, -1, 0x2};
static u32 l_msg_1st_order_not_end[3] = {0x2a44, 0x2a45, 0x0};
static u16 l_msg_1st_order_not_end_c[1] = {0xff02};
static u32 l_msg_2nd_order[4] = {0x2a46, 0x2a47, 0x2a48, 0x3};
static u8 l_msg_2nd_order_c[3] = {0xff, 0x0, 0xff};
static u32 l_msg_2nd_order_not_end[3] = {0x2a49, 0x2a4a, 0x0};
static u16 l_msg_2nd_order_not_end_c[1] = {0xff02};
static u32 l_msg_3rd_order[5] = {0x2a4b, 0x2a4c, 0x2a4d, 0x2a4e, 0x5};
static u32 l_msg_3rd_order_c[1] = {0xff00ff02};
static u32 l_msg_3rd_order_not_end[3] = {0x2a4f, 0x2a50, 0x0};
static u16 l_msg_3rd_order_not_end_c[1] = {0xffff};
static u32 l_msg_color[3] = {0x2a54, 0x2a55, 0x0};
static u16 l_msg_color_c[1] = {0xff01};
static u32 l_msg_week_1st[6] = {0x3781, 0x3782, 0x3783, 0x3784, 0x3785, 0x0};
static u32 l_msg_week[7] = {0x101c4fb4, 0x101c4fc4, 0x101c4fd4, 0x101c4fe4, 0x101c4ff4, 0x101c5004, 0x101c5014};
static u32 l_msg_buy_photo[2] = {0x3786, 0x0};
static u32 l_msg_get_photo[2] = {0x3785, 0x0};
static u32 l_msg_down[2] = {0x2a86, 0x0};
static u32 l_msg_ub4[2] = {0x2d62, 0x0};
static u32 l_msg_1st_order_xy[4] = {0x2a5e, 0x2a5f, 0x2a60, 0x2};
static u32 l_msg_2nd_order_xy[4] = {0x2a61, 0x2a62, 0x2a60, 0x4};
static u32 l_msg_3rd_order_xy[3] = {0x2a64, 0x2a60, 0x6};
static u32 l_msg_color_xy[4] = {0x2a58, 0x2a59, 0x2a5a, 0x9};
static u32 l_msg_xy_buy_photo[9] = {0x3788, 0x3789, 0xa, 0x378c, 0xb, 0x378d, 0xc, 0x3786, 0x0};
static cXyz l_counter_pos[2] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
static cXyz l_gallery_pos[2] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
static cXyz l_msg_camera[6] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};


static NpcDatStruct l_npc_dat = {
    0.0f,
    2500,
    2000,
    5000,
    8000,
    -150,
    -512,
    -1000,
    -8000,
    500,
    800,
    1800,
    {0,
    0},
    0.0f,
    215.0f,
    30.0f,
    32767,
    {0,
    0},
    200.0f,
    300.0f,
    -50.0f,
    0.30000001192092896f,
    5.0f,
    1.2999999523162842f,
    1000,
    6,
    1024,
    30,
    30,
    9999,
    9999,
    1,
    1,
};
static const char* l_arcname_tbl[1] = {"Po"};
static const char* l_npc_staff_id = {"Po"};

static u32 l_msg_1st_talk_photo[5] = {0x2a69, 0x2a6a, 0x2a6b, 0x2a6c, 0x0};
static u32 l_msg_talk_photo[7] = {0x101c5164, 0x101c5170, 0x101c517c, 0x101c5188, 0x101c5194, 0x101c51a0, 0x101c51ac};
static u32 l_msg_2F[3] = {0x2a82, 0x2a83, 0x2a84};

static PhotoCylinderSource l_cyl_src2 = {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x279, 0x0, 0x0, 0x0, 0x4, 0x0, 0x0, 0x0, 0x0, 0x42200000, 0x43200000}};
static PsoData l_pso_photo = {0.0f, 100.0f, -30.0f, 40.0f, 90.0f, 0, 255, 4};
static cPhs_State phase_1(daNpcPhoto_c*);
static cPhs_State phase_2(daNpcPhoto_c*);
static dCcD_Cyl* dCcD_Cyl_ct_tu(dCcD_Cyl*);
static PhotoPhaseHandler l_method[3] = {{(const void*)&phase_1}, {(const void*)&phase_2}, {0}};
// HD: the common NPC cylinder source is owned by d_npc, not this translation unit.
static const dCcD_SrcCyl& photo_npcCylinderSource = gabi::object_at<dCcD_SrcCyl>(0x101EA190);

// HD bindings and helpers.
static u32 daObj_PrmAbstract(fopAc_ac_c* i_actor, u32 i_width, u32 i_shift);
/* dComLbG_PhaseHandler(phase, table, this) */

/* __construct_array(array, n, size, ctor), 028F0164 __destroy_arr(array, n, size, dtor, flags) */


/* 022CBEDC */
/* (unnamed by the matcher) */
static BOOL daNpc_Photo_nodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        J3DModel* model = j3dSys.getModel(); /* j3dSys.getModel() */
        daNpcPhoto_c* i_this = static_cast<daNpcPhoto_c*>(model->getUserArea()); /* getUserArea() */
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 jntNo = gabi::object_at<PhotoJointNumber>(gabi::ea(joint)).mNumber;
        PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
        if (jntNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
            mDoMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[0][1]);
            mDoMtx_ZrotM(calc_mtx(), -i_this->m_jnt.mAngles[0][0]);
        }
        if (jntNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
            mDoMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]);
            mDoMtx_ZrotM(calc_mtx(), -i_this->m_jnt.mAngles[1][0]);
        }
        /* model->setAnmMtx(jntNo, *calc_mtx) */
        mtx_copy(model->getAnmMtx(jntNo), calc_mtx());
        PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
    }
    return TRUE;
}
/* 022CC034 */
BOOL daNpcPhoto_c::initTexPatternAnm(u32 i_modify, int i_param2) {
    J3DModelData* modelData = mpMorf->getModel()->getModelData();
    if (i_param2 == -1) {
        i_param2 = dComIfGs_isEventBit(0x1701 /* l_save_dat.field_0x02 */) ? 1 : 0;
    }
    m_head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectIDRes(l_arcname_tbl[0], l_btp_ix_tbl[i_param2]);
    if (m_head_tex_pattern.get() == nullptr) { /* JUT_ASSERT(0xC30, m_head_tex_pattern != NULL) */
        JUT_ASSERT_fail("d_a_npc_photo.cpp", 0xC30, "m_head_tex_pattern != (0)");
    }
    if (!mDoExt_btpAnm_init(&mBtpAnm, modelData, m_head_tex_pattern, TRUE, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, i_modify, FALSE)) {
        return FALSE;
    }
    mFrame = 0;
    mTimer = 0;
    return TRUE;
}
/* 022CC160 */
BOOL daNpcPhoto_c::createHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectIDRes(l_arcname_tbl[0], dRes_ID_PO_BDL_PO_e);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(l_arcname_tbl[0], l_bck_ix_tbl[field_0x9C8]);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1, nullptr,
                                    0x80000, 0x11020022);
    if (mpMorf.get() == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    m_jnt.mHeadJntNum = JUTNameTab_getIndex(gabi::ea(modelData->getJointName()), "head" /* "head" */);
    if (!(m_jnt.mHeadJntNum >= 0)) {
        JUT_ASSERT_fail("d_a_npc_photo.cpp", 0x4C6, "m_jnt.getHeadJntNum() >= 0");
    }
    m_jnt.mBackboneJntNum = JUTNameTab_getIndex(gabi::ea(modelData->getJointName()), "backbone" /* "backbone" */);
    if (!(m_jnt.mBackboneJntNum >= 0)) {
        JUT_ASSERT_fail("d_a_npc_photo.cpp", 0x4CB, "m_jnt.getBackboneJntNum() >= 0");
    }
    if (!initTexPatternAnm(false, -1)) {
        return FALSE;
    }
    for (u16 i = 0; i < modelData->getJointNum(); i++) {
        if (i == (u32)(s32)m_jnt.mHeadJntNum || i == (u32)(s32)m_jnt.mBackboneJntNum) {
            modelData->getJointNodePointer(i)->setCallBack(daNpc_Photo_nodeCallBack);
        }
    }
    mpMorf->getModel()->mUserArea = gabi::ea(this); /* setUserArea(this) */
    mAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    return TRUE;
}
/* 022CC3E8 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return static_cast<daNpcPhoto_c*>(i_this)->createHeap();
}
/* 022CC3EC */
u8 daNpcPhoto_c::getPrmArg0() {
    return daObj_PrmAbstract(this, PRM_ARG0_W, PRM_ARG0_S);
}
/* 022CC418 */
/* daNpcPhoto_c::daNpcPhoto_c (HD: allocates when this == NULL) */
static daNpcPhoto_c* daNpcPhoto_c_ct(daNpcPhoto_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpcPhoto_c*)operator_new(0xB50);
        if (i_this == nullptr) {
            return i_this;
        }
    }
    photo_actorConstruct(i_this); /* fopNpc_npc_c::fopNpc_npc_c */
    i_this->__vtbl = PHOTO_VTBL;
    mDoExt_btpAnm::ct(&i_this->mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
    __construct_array(i_this->field_0x6F8, 2, 0x130, gabi::guest_fn((const void*)&dCcD_Cyl_ct_tu));
    i_this->field_0x9C3 = 0;
    i_this->field_0x9C2 = false;
    i_this->field_0x9C0 = 0;
    i_this->field_0x9C1 = 0;
    i_this->field_0x984 = 0.0f;
    i_this->field_0x9A8 = 0;
    i_this->field_0x988 = 60.0f;
    i_this->field_0x958.x = 0.0f;
    i_this->field_0x958.y = 0.0f;
    i_this->field_0x958.z = 0.0f;
    i_this->field_0x9CD = false;
    i_this->field_0x9D6 = 0;
    i_this->field_0x994 = true;
    i_this->field_0x9AE = i_this->home.angle.y;
    i_this->field_0x9C8 = false;
    i_this->field_0x9C7 = true;
    i_this->field_0x98C = -1.0f;
    i_this->mReplaceDateMessages = 0; /* HD */
    return i_this;
}
/* 022CC4F8 */
static cPhs_State phase_1(daNpcPhoto_c* i_this) {
    /* fopAcM_ct(i_this, daNpcPhoto_c) */
    if (!(i_this->actor_condition & fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            daNpcPhoto_c_ct(i_this);
        }
        i_this->actor_condition = i_this->actor_condition | fopAcCnd_INIT_e;
    }
    u32 arg0 = i_this->getPrmArg0();
    if (arg0 != 0xFF) {
        if (arg0 == 0) {
            if (!dComIfGs_checkGetItem(dItemNo_COLLECT_MAP_20_e) && arg0 != (u32)(s32)dComIfGp_getStartStagePoint()) {
                return cPhs_UNK3_e; /* cPhs_STOP_e */
            }
        } else {
            if (dComIfGs_checkGetItem(dItemNo_COLLECT_MAP_20_e) || arg0 != (u32)(s32)dComIfGp_getStartStagePoint()) {
                return cPhs_UNK3_e;
            }
            i_this->field_0x9C1 = 4;
        }
    }
    i_this->field_0x9C2 = true;
    return cPhs_NEXT_e;
}
/* 022CC5DC */
u8 daNpcPhoto_c::getPrmRailID() {
    return daObj_PrmAbstract(this, PRM_RAIL_ID_W, PRM_RAIL_ID_S);
}
/* 022CC608 */
s16 daNpcPhoto_c::XyCheckCB(int i_itemBtn) {
    if (dComIfGs_isTmpBit(0x0302 /* UNK_0302 */) && !dComIfGs_isTmpBit(0x0301 /* UNK_0301 */)) {
        attention_info.flags = fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e; /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    } else {
        attention_info.flags = PHOTO_HD_ATTENTION | fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e;
    }
    return 1;
}
/* 022CC68C */
static s16 daNpcPhoto_XyCheckCB(void* i_this, int i_itemBtn) {
    return ((daNpcPhoto_c*)i_this)->XyCheckCB(i_itemBtn);
}
/* 022CC690 */
s16 daNpcPhoto_c::XyEventCB(int i_itemBtn) {
    u8 itemNo = dComIfGp_getSelectItem(i_itemBtn);
    if (itemNo == dItemNo_DELUXE_PICTO_BOX_e && dComIfGs_isTmpBit(0x0302)) {
        // HD: up to 12 pictures (GameCube 3)
        if (photo_getPictureNum() < 0xC) {
            s16 eventIdx = mPhotoGetPhotoEventIdx;
            field_0x9C7 = false;
            return eventIdx;
        }
        return dComIfGp_evmng_getEventIdx("DEFAULT_TALK_XY" /* "DEFAULT_TALK_XY" */, 0xFF);
    }
    // HD: no firefly bottle branch (the Deluxe Picto Box is not traded for the bottle)
    return dComIfGp_evmng_getEventIdx("DEFAULT_TALK_XY", 0xFF);
}
/* 022CC75C */
static s16 daNpcPhoto_XyEventCB(void* i_this, int i_itemBtn) {
    return ((daNpcPhoto_c*)i_this)->XyEventCB(i_itemBtn);
}
/* 022CC760 */
void daNpcPhoto_c::setMtx() {
    J3DModel* model = mpMorf->getModel();
    cXyz* base_scale = &model->mBaseScale; /* setBaseScale(scale) */
    f32 sx = scale.x, sy = scale.y, sz = scale.z;
    base_scale->x = sx;
    base_scale->y = sy;
    base_scale->z = sz;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
}
/* 022CC840 */
void daNpcPhoto_c::setCollision(dCcD_Cyl* cyl, cXyz* center, f32 radius, f32 height) {
    cyl->SetC(center);
    cyl->SetR(radius);
    cyl->SetH(height);
    dComIfG_Ccsp_Set(cyl);
}
/* 022CC8D0 */
cPhs_State daNpcPhoto_c::createInit() {
    int temp = 0xFF;
    u8 pathIndex = getPrmRailID();
    if (pathIndex != 0xFF) {
        mPathRun.setInf(pathIndex, fopAcM_GetRoomNo(this), true);
        if (mPathRun.mPath.get() == nullptr) {
            return cPhs_ERROR_e;
        }
        // HD: no dPath_GetNextRoomPath(mPathRun.getPath(), -1)
        if (dComIfGs_isEventBit(0x1701 /* l_save_dat.field_0x02 */)) {
            gabi::Local<cXyz> point;
            mPathRun.getPoint(point.get(), mPathRun.mIdx);
            old.pos.copy(*point);
            current.pos.copy(old.pos);
            mPathRun.incIdxLoop();
            field_0x9A8 = 1;
            field_0x9C1 = 2;
        }
        temp = 0xFE;
    }
    gravity = -9.0f;
    mPhotoLinkBackEventIdx = dComIfGp_evmng_getEventIdx("PHOTO_LINK_BACK" /* "PHOTO_LINK_BACK" */, 0xFF);
    mPhotoGetItemEventIdx = dComIfGp_evmng_getEventIdx("PHOTO_GET_ITEM" /* "PHOTO_GET_ITEM" */, 0xFF);
    mPhotoGetItem2EventIdx = dComIfGp_evmng_getEventIdx("PHOTO_GET_ITEM2" /* "PHOTO_GET_ITEM2" */, 0xFF);
    mPhotoGetPhotoEventIdx = dComIfGp_evmng_getEventIdx("PHOTO_GET_PHOTO" /* "PHOTO_GET_PHOTO" */, 0xFF);
    mPhotoGalleryEventIdx = dComIfGp_evmng_getEventIdx("PHOTO_GALLERY" /* "PHOTO_GALLERY" */, 0xFF);
    mPhotoCounterTalk0EventIdx = dComIfGp_evmng_getEventIdx("PHOTO_COUNTER_TALK0" /* "PHOTO_COUNTER_TALK0" */, 0xFF);
    mPhotoCounterTalk1EventIdx = dComIfGp_evmng_getEventIdx("PHOTO_COUNTER_TALK1" /* "PHOTO_COUNTER_TALK1" */, 0xFF);
    mPhotoDateUB4EventIdx = dComIfGp_evmng_getEventIdx("PHOTO_DATE_UB4" /* "PHOTO_DATE_UB4" */, 0xFF);
    field_0x9A6 = mPhotoCounterTalk0EventIdx;
    mEventCut.setActorInfo2(l_npc_staff_id /* "Po" */, this);
    field_0x9B6 = 0;
    field_0x9BC = false;
    field_0x9BD = false;
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 173; /* attention_info.distances[fopAc_Attn_TYPE_TALK_e] */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 173; /* attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] */
    attention_info.flags = PHOTO_HD_ATTENTION | fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e; /* attention_info.flags */
    eventInfo.setXyCheckCB(daNpcPhoto_XyCheckCB); /* eventInfo.setXyCheckCB(daNpcPhoto_XyCheckCB) */
    eventInfo.setXyEventCB(daNpcPhoto_XyEventCB); /* eventInfo.setXyEventCB(daNpcPhoto_XyEventCB) */
    if (field_0x9C1 == 4) {
        field_0x9D7 = false;
        field_0x9D8 = false;
    } else {
        field_0x9D7 = l_npc_dat.field_0x52;
        field_0x9D8 = l_npc_dat.field_0x53;
    }
    field_0x9B2 = l_npc_dat.field_0x28;
    mObjAcch.CrrPos(dComIfG_Bgsp());
    f32 gnd = mObjAcch.GetGroundH();
    if (gnd != -1000000000.0f /* -G_CM3D_F_INF */) {
        current.pos.y = gnd;
        home.pos.y = gnd;
    }
    setMtx();
    J3DModel_calc(mpMorf->getModel());
    mStts.Init(temp, 0xFF, this);
    mCyl.Set(&photo_npcCylinderSource /* dNpc_cyl_src */);
    mCyl.SetStts(&mStts);
    gabi::Local<cXyz> center;
    cXyz_pl(&current.pos, center.get(), &field_0x958);
    setCollision(&mCyl, center.get(), field_0x988, 150.0f);
    field_0x6F8[0].Set(reinterpret_cast<dCcD_SrcCyl*>(&l_cyl_src2));
    field_0x6F8[0].SetStts(&mStts);
    field_0x6F8[1].Set(reinterpret_cast<dCcD_SrcCyl*>(&l_cyl_src2));
    field_0x6F8[1].SetStts(&mStts);
    return cPhs_COMPLEATE_e;
}
/* 022CCC50 */
static cPhs_State phase_2(daNpcPhoto_c* i_this) {
    cPhs_State phase_state = dComIfG_resLoad(i_this->getPhaseP(), l_arcname_tbl[0]);
    if (phase_state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, CheckCreateHeap, 0)) {
            return i_this->createInit();
        }
        i_this->mpMorf = nullptr;
        return cPhs_ERROR_e;
    }
    return phase_state;
}
/* 022CCCD8 */
cPhs_State daNpcPhoto_c::_create() {
    /* static cPhs__Handler l_method[] = {phase_1, phase_2, NULL} (.data 0x101C52B4) */
    return dComLbG_PhaseHandler(&mPhs2, gabi::ea(l_method), this);
}
/* 022CCCEC */
static cPhs_State daNpc_PhotoCreate(void* i_this) {
    return static_cast<daNpcPhoto_c*>(i_this)->_create();
}
/* 022CCCF0 */
bool daNpcPhoto_c::_delete() {
    dComIfG_resDelete(getPhaseP(), l_arcname_tbl[0]);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return true;
}
/* 022CCD48 */
static BOOL daNpc_PhotoDelete(void* i_this) {
    return static_cast<daNpcPhoto_c*>(i_this)->_delete();
}
/* 022D0C60 */
static u32 daObj_PrmAbstract(fopAc_ac_c* i_actor, u32 i_width, u32 i_shift) {
    /* PowerPC slw/srw: shift counts 32..63 give 0 */
    u32 mask = ((i_width & 0x20) ? 0u : (1u << (i_width & 0x1F))) - 1;
    u32 prm = i_actor->mParameters;
    return ((i_shift & 0x20) ? 0u : (prm >> (i_shift & 0x1F))) & mask;
}
/* 022D0904 */
/* __sinit_d_a_npc_photo_cpp: header statics (P = 0x104683A8, zeroed object at
 * 0x104683CC) and the cXyz statics */
static void __sinit_d_a_npc_photo_cpp() {
    __sinit_header_statics();
    const f32 init[] = {
        /* 0x104683B4 l_counter_pos[2] */
        -490.0f, 0.0f, -10.0f, -260.0f, 0.0f, -250.0f,
    };
    for (int i = 0; i < 6; i++) {
        photo_positionComponent(l_counter_pos[i / 3], i % 3) = init[i];
    }
    /* 0x104683DC l_gallery_pos; 0x104683E8 HD: the second gallery position */
    const f32 gallery[] = {-260.0f, 500.0f, 400.0f, -384.8f, 450.0f, 603.2f};
    for (int i = 0; i < 6; i++) {
        photo_positionComponent(l_gallery_pos[i / 3], i % 3) = gallery[i];
    }
    /* 0x104683F4 l_msg_camera[3][2] */
    const f32 camera[] = {
        -1336.0f, -497.0f, 928.0f, 185.0f, -26.0f, 0.0f,
        -1213.0f, -902.0f, -244.0f, 284.0f, 36.0f, 280.0f,
        -29.0f, 252.0f, -1634.0f, 5.0f, -91.0f, 178.0f,
    };
    for (int i = 0; i < 18; i++) {
        photo_positionComponent(l_msg_camera[i / 3], i % 3) = camera[i];
    }
}
/* 022D0AF8 */
/*: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
/* 022D0B0C */
/*: dCcD_Cyl::dCcD_Cyl (this TU's copy; HD: allocates when this == NULL) */
static dCcD_Cyl* dCcD_Cyl_ct_tu(dCcD_Cyl* p) {
    if (p == nullptr) {
        p = (dCcD_Cyl*)operator_new(0x130);
        if (p == nullptr) {
            return p;
        }
    }
    dCcD_Cyl_ct(p, PHOTO_AAB_VTBL);
    return p;
}
/* 022D0B98 */
static BOOL daNpc_PhotoIsDelete(void*) {
    return TRUE;
}
/* 022D0BA0 */
/*: daNpcPhoto_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpcPhoto_dt(daNpcPhoto_c* p, s32 flags) {
    if (p != nullptr) {
        photo_destroyArray(p->field_0x6F8, 2, 0x130, gabi::guest_fn((const void*)&photo_cylElementDestroy), 0, 0); /* __destroy_arr */
        __dt(&p->mCyl);                                    /* ~dCcD_Cyl */
        __dt(&p->mStts);                                   /* ~dCcD_Stts */
        __dt(&p->mAcchCir); /* ~cM3dGCir */
        __dt(&p->mObjAcch);
        photo_actorDestroy(p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) {
            operator_delete(p);
        }
    }
}
/* 022D0C5C */
/*: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
}

/* WWHD glue (tools/verify/autoglue.py): shared TU tables
data l_msg_1st_talk 101C4F7C
data l_msg_2nd_talk 101C4F24
data l_msg_1st_photo 101C50AC
data l_msg_2nd_photo 101C50B8
data l_msg_1st_order 101C50C4
data l_msg_1st_order_c 101C5210
data l_msg_1st_order_not_end 101C50F0
data l_msg_1st_order_not_end_c 101C4F68
data l_msg_2nd_order 101C4F8C
data l_msg_2nd_order_c 101C4F79
data l_msg_2nd_order_not_end 101C50FC
data l_msg_2nd_order_not_end_c 101C4F6A
data l_msg_3rd_order 101C5108
data l_msg_3rd_order_c 101C4F50
data l_msg_3rd_order_not_end 101C511C
data l_msg_3rd_order_not_end_c 101C4F6C
data l_msg_color 101C5128
data l_msg_color_c 101C4F6E
data l_msg_week_1st 101C4F9C
data l_msg_week 101C5134
data l_msg_buy_photo 101C4F2C
data l_msg_get_photo 101C4F34
data l_msg_down 101C4F3C
data l_msg_ub4 101C4F44
data l_msg_1st_order_xy 101C5024
data l_msg_2nd_order_xy 101C5034
data l_msg_3rd_order_xy 101C51E0
data l_msg_color_xy 101C5044
data l_msg_xy_buy_photo 101C51EC
data l_counter_pos 104683B4 nocheck # BSS: initialized by verified __sinit, absent from image
data l_gallery_pos 104683DC nocheck # BSS: initialized by verified __sinit, absent from image
data l_msg_camera 104683F4 nocheck # BSS: initialized by verified __sinit, absent from image
data l_npc_dat 101C521C
data l_arcname_tbl 101C4F4C
data l_npc_staff_id 101C4F54
str 10020C3C "Po"
data l_bck_ix_tbl 10020CD8
data l_btp_ix_tbl 10020C30
data l_npc_anm_wait 101C4F70
data l_npc_anm_talk 101C4F73
data l_npc_anm_walk 101C4F76
data l_npc_anm_spit 101C4F58
data l_npc_anm_talk2 101C4F60
ext 025E789C s32 mDoExt_btpAnm_init(void*, J3DModelData*, void*, s32, s32, f32, s32, s32, u32, s32)
ext 0259DED0 void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8)
ext 025F795C u32 msgMng_getStatus(u32)
ext 025F74D0 void msgMng_setStatus(u32, u32)
ext 025F7DB0 u32 msgMng_messageSet(u32, u32, cXyz*)
ext 027DF9B0 s8 JUTNameTab_getIndex(u32, const char*)
ext 025D9F38 fopAc_ac_c* fopAcM_searchFromName(const char*, u32, u32)
ext 02554288 s32 dKy_get_dayofweek()
ext 025BD71C void dSnap_Obj_ct(dSnap_Obj*)
ext 025BEB90 void dSnap_Obj_SetInf(dSnap_Obj*, u8, fopAc_ac_c*, u8, u8, s16)
ext 025BEB5C void dSnap_Obj_SetGeo(dSnap_Obj*, cXyz*, f32, f32, s16)
ext 025BEB4C void dSnap_RegistSnapObj(dSnap_Obj*)
ext 02525FE4 cPhs_State dComLbG_PhaseHandler(request_of_phase_process_class*, u32, void*)
ext 028EFFD0 void __construct_array(void*, u32, u32, u32)
ext 028F0164 void __destroy_arr(void*, u32, u32, u32, u32)
ext 025E1988 void mDoAud_seStart_1(u32)
ext 025AB070 dPath* dPath_GetNextRoomPath(dPath*, s32)
ext 025AAEB8 u32 dPath_GetPnt(dPath*, s32)
ext 025D7DEC u32 fopAcM_createItemForPresentDemo(cXyz*, s32, u8, s32, s32, csXyz*, cXyz*)
ext 0200EF78 void cLib_addCalcPosXZ(cXyz*, const cXyz*, f32, f32, f32)
ext 024F8044 u32 dCam_getBody()
ext 02514F2C void dCamera_Stop(u32)
ext 02514F38 void dCamera_Start(u32)
ext 02514F88 void dCamera_Set(u32, cXyz*, cXyz*, f32, s16)

str 10020CF0 "d_a_npc_photo.cpp" in 022CC034
str 10020D04 "m_head_tex_pattern != (0)" in 022CC034
str 10020D24 "head" in 022CC160
str 10020D2C "d_a_npc_photo.cpp" in 022CC160
str 10020D40 "m_jnt.getHeadJntNum() >= 0" in 022CC160
str 10020D5C "backbone" in 022CC160
str 10020D68 "m_jnt.getBackboneJntNum() >= 0" in 022CC160
str 10020D94 "DEFAULT_TALK_XY" in 022CC690
str 10020DB0 "PHOTO_LINK_BACK" in 022CC8D0
str 10020E18 "PHOTO_GET_ITEM" in 022CC8D0
str 10020DC0 "PHOTO_GET_ITEM2" in 022CC8D0
str 10020DD0 "PHOTO_GET_PHOTO" in 022CC8D0
str 10020E08 "PHOTO_GALLERY" in 022CC8D0
str 10020DE0 "PHOTO_COUNTER_TALK0" in 022CC8D0
str 10020DF4 "PHOTO_COUNTER_TALK1" in 022CC8D0
str 10020E28 "PHOTO_DATE_UB4" in 022CC8D0
ext 02720144 u32 photo_getPictureSave(u32)
ext 02726218 u32 photo_getFreePictureSlot(u32)
ext 027261D0 void photo_markPictureSlot(u32, u32)
ext 0272609C u32 photo_getPicture(u32, u32)
ext 02725A2C void photo_setPictureDay(u32, s32)
ext 02725E0C void photo_erasePicture(u32, u8)
ext 022CE1E0 u32 photo_eventMesSetRegister(daNpcPhoto_c*)
ext 025A1458 void photo_actorConstruct(fopNpc_npc_c*)
ext 028F0164 void photo_destroyArray(void*, u32, u32, u32, u32, u32)
ext 025D50BC void photo_actorDestroy(fopAc_ac_c*, s32)
ext 02542EDC s8 photo_eventGetActIdx(dEvent_manager_c*, s32, u32, s32, BOOL, s32)
ext 02542E94 u32 photo_runEventName(dEvent_manager_c*)
data l_msg_1st_talk_photo 101C5150
data l_msg_talk_photo 101C51B8
data l_msg_2F 101C51D4
data l_cyl_src2 101C5270
data l_pso_photo 101C5054
data l_method 101C52B4 nocheck # Named callbacks registered at their preserved ADDR markers
ext 02515A70 void photo_cylElementDestroy(dCcD_Cyl*, s32) callback
*/
