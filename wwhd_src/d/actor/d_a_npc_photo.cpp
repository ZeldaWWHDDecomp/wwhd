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

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static u32 daObj_PrmAbstract(fopAc_ac_c* i_actor, u32 i_width, u32 i_shift);
/* 02525FE4 dComLbG_PhaseHandler(phase, table, this) */
static inline cPhs_State dComLbG_PhaseHandler(request_of_phase_process_class* p, u32 tbl, void* self) {
    return gabi::call<cPhs_State>(0x02525FE4, p, tbl, self);
}
/* 028EFFD0 __construct_array(array, n, size, ctor), 028F0164 __destroy_arr(array, n, size, dtor, flags) */
static inline void __construct_array(void* p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void __destroy_arr(void* p, u32 n, u32 size, u32 dtor, u32 flags) { gabi::call(0x028F0164, p, n, size, dtor, flags); }

/* 022CBEDC (unnamed by the matcher) */
static BOOL daNpc_Photo_nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x022CBEDC, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        u32 model = gabi::load<u32>(0x104B462C); /* j3dSys.getModel() */
        daNpcPhoto_c* i_this = gabi::at<daNpcPhoto_c>(gabi::load<u32>(model + 0xB8)); /* getUserArea() */
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        J3DMtxBlock_l* blk = gabi::at<J3DMtxBlock_l>(gabi::load<u32>(model + 0x2C));
        blk->mFlags |= 0x10;
        PSMTXCopy(gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30), calc_mtx());
        if (jntNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
            mDoMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[0][1]);
            mDoMtx_ZrotM(calc_mtx(), -i_this->m_jnt.mAngles[0][0]);
        }
        if (jntNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
            mDoMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]);
            mDoMtx_ZrotM(calc_mtx(), -i_this->m_jnt.mAngles[1][0]);
        }
        /* model->setAnmMtx(jntNo, *calc_mtx) */
        blk = gabi::at<J3DMtxBlock_l>(gabi::load<u32>(model + 0x2C));
        blk->mFlags |= 0x10;
        mtx_copy(gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30), calc_mtx());
        PSMTXCopy(calc_mtx(), j3dSys_mCurrentMtx());
    }
    return TRUE;
}
VERIFY(0x022CBEDC, daNpc_Photo_nodeCallBack);

/* 022CC034 */
BOOL daNpcPhoto_c::initTexPatternAnm(u32 i_modify, int i_param2) {
    WWHD_FUNC(0x022CC034, BOOL, this, i_modify, i_param2);
    J3DModelData* modelData = J3DModel_getModelData_l(mpMorf->getModel());
    if (i_param2 == -1) {
        i_param2 = dComIfGs_isEventBit(0x1701 /* l_save_dat.field_0x02 */) ? 1 : 0;
    }
    m_head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectIDRes(photo_arcname(), gabi::load<s32>(PHOTO_l_btp_ix_tbl + i_param2 * 4));
    if (m_head_tex_pattern.get() == nullptr) { /* JUT_ASSERT(0xC30, m_head_tex_pattern != NULL) */
        JUT_ASSERT_fail(STR(0x10020CF0), 0xC30, STR(0x10020D04));
    }
    if (!mDoExt_btpAnm_init(mBtpAnm, modelData, m_head_tex_pattern, TRUE, 2 /* EMode_LOOP */, 1.0f, 0, -1, i_modify, FALSE)) {
        return FALSE;
    }
    mFrame = 0;
    mTimer = 0;
    return TRUE;
}
VERIFY(0x022CC034, &daNpcPhoto_c::initTexPatternAnm);

/* 022CC160 */
BOOL daNpcPhoto_c::createHeap() {
    WWHD_FUNC(0x022CC160, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectIDRes(photo_arcname(), 1 /* dRes_ID_PO_BDL_PO_e */);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(photo_arcname(), gabi::load<s32>(PHOTO_l_bck_ix_tbl + field_0x9C8 * 4));
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1, nullptr,
                                    0x80000, 0x11020022);
    if (mpMorf.get() == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    m_jnt.mHeadJntNum = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x10020D24) /* "head" */);
    if (!(m_jnt.mHeadJntNum >= 0)) {
        JUT_ASSERT_fail(STR(0x10020D2C), 0x4C6, STR(0x10020D40));
    }
    m_jnt.mBackboneJntNum = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x10020D5C) /* "backbone" */);
    if (!(m_jnt.mBackboneJntNum >= 0)) {
        JUT_ASSERT_fail(STR(0x10020D2C), 0x4CB, STR(0x10020D68));
    }
    if (!initTexPatternAnm(false, -1)) {
        return FALSE;
    }
    for (u16 i = 0; i < J3DModelData_getJointNum_l(modelData); i++) {
        if (i == (u32)(s32)m_jnt.mHeadJntNum || i == (u32)(s32)m_jnt.mBackboneJntNum) {
            J3DModelData_setJointCallBack_l(modelData, i, 0x022CBEDC /* daNpc_Photo_nodeCallBack */);
        }
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    mAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    return TRUE;
}
VERIFY(0x022CC160, &daNpcPhoto_c::createHeap);

/* 022CC3E8 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022CC3E8, BOOL, i_this);
    return static_cast<daNpcPhoto_c*>(i_this)->createHeap();
}
VERIFY(0x022CC3E8, CheckCreateHeap);

/* 022CC3EC */
u8 daNpcPhoto_c::getPrmArg0() {
    WWHD_FUNC(0x022CC3EC, u8, this);
    return daObj_PrmAbstract(this, PRM_ARG0_W, PRM_ARG0_S);
}
VERIFY(0x022CC3EC, &daNpcPhoto_c::getPrmArg0);

/* 022CC418 daNpcPhoto_c::daNpcPhoto_c (HD: allocates when this == NULL) */
static daNpcPhoto_c* daNpcPhoto_c_ct(daNpcPhoto_c* i_this) {
    WWHD_FUNC(0x022CC418, daNpcPhoto_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpcPhoto_c*)operator_new(0xB50);
        if (i_this == nullptr) {
            return i_this;
        }
    }
    gabi::call(0x025A1458, i_this); /* fopNpc_npc_c::fopNpc_npc_c */
    i_this->__vtbl = PHOTO_VTBL;
    gabi::call(0x025E7820, i_this->mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
    __construct_array(i_this->field_0x6F8, 2, 0x130, 0x022D0B0C /* dCcD_Cyl::dCcD_Cyl */);
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
    i_this->mHD_B4E = 0; /* HD */
    return i_this;
}
VERIFY(0x022CC418, daNpcPhoto_c_ct);

/* 022CC4F8 */
static cPhs_State phase_1(daNpcPhoto_c* i_this) {
    WWHD_FUNC(0x022CC4F8, cPhs_State, i_this);
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
            if (!dComIfGs_checkGetItem(0xEB /* dItemNo_COLLECT_MAP_20_e */) && arg0 != (u32)(s32)dComIfGp_getStartStagePoint()) {
                return cPhs_UNK3_e; /* cPhs_STOP_e */
            }
        } else {
            if (dComIfGs_checkGetItem(0xEB) || arg0 != (u32)(s32)dComIfGp_getStartStagePoint()) {
                return cPhs_UNK3_e;
            }
            i_this->field_0x9C1 = 4;
        }
    }
    i_this->field_0x9C2 = true;
    return cPhs_NEXT_e;
}
VERIFY(0x022CC4F8, phase_1);

/* 022CC5DC */
u8 daNpcPhoto_c::getPrmRailID() {
    WWHD_FUNC(0x022CC5DC, u8, this);
    return daObj_PrmAbstract(this, PRM_RAIL_ID_W, PRM_RAIL_ID_S);
}
VERIFY(0x022CC5DC, &daNpcPhoto_c::getPrmRailID);

/* 022CC608 */
s16 daNpcPhoto_c::XyCheckCB(int i_itemBtn) {
    WWHD_FUNC(0x022CC608, s16, this, i_itemBtn);
    if (dComIfGs_isTmpBit(0x0302 /* UNK_0302 */) && !dComIfGs_isTmpBit(0x0301 /* UNK_0301 */)) {
        gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    } else {
        gabi::store<u32>(gabi::ea(this) + 0x39C, 0x0100000A);
    }
    return 1;
}
VERIFY(0x022CC608, &daNpcPhoto_c::XyCheckCB);

/* 022CC68C */
static s16 daNpcPhoto_XyCheckCB(void* i_this, int i_itemBtn) {
    WWHD_FUNC(0x022CC68C, s16, i_this, i_itemBtn);
    return ((daNpcPhoto_c*)i_this)->XyCheckCB(i_itemBtn);
}
VERIFY(0x022CC68C, daNpcPhoto_XyCheckCB);

/* 022CC690 */
s16 daNpcPhoto_c::XyEventCB(int i_itemBtn) {
    WWHD_FUNC(0x022CC690, s16, this, i_itemBtn);
    u8 itemNo = dComIfGp_getSelectItem(i_itemBtn);
    if (itemNo == 0x26 /* dItemNo_DELUXE_PICTO_BOX_e */ && dComIfGs_isTmpBit(0x0302)) {
        /* HD: up to 12 pictures (GameCube 3) */
        if (dComIfGs_getPictureNum() < 0xC) {
            s16 eventIdx = mPhotoGetPhotoEventIdx;
            field_0x9C7 = false;
            return eventIdx;
        }
        return dComIfGp_evmng_getEventIdx(STR(0x10020D94) /* "DEFAULT_TALK_XY" */, 0xFF);
    }
    /* HD: no firefly bottle branch (the Deluxe Picto Box is not traded for the bottle) */
    return dComIfGp_evmng_getEventIdx(STR(0x10020D94), 0xFF);
}
VERIFY(0x022CC690, &daNpcPhoto_c::XyEventCB);

/* 022CC75C */
static s16 daNpcPhoto_XyEventCB(void* i_this, int i_itemBtn) {
    WWHD_FUNC(0x022CC75C, s16, i_this, i_itemBtn);
    return ((daNpcPhoto_c*)i_this)->XyEventCB(i_itemBtn);
}
VERIFY(0x022CC75C, daNpcPhoto_XyEventCB);

/* 022CC760 */
void daNpcPhoto_c::setMtx() {
    WWHD_FUNC(0x022CC760, void, this);
    J3DModel* model = mpMorf->getModel();
    cXyz* base_scale = gabi::at<cXyz>(gabi::ea(model) + 0xBC); /* setBaseScale(scale) */
    f32 sx = scale.x, sy = scale.y, sz = scale.z;
    base_scale->x = sx;
    base_scale->y = sy;
    base_scale->z = sz;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
}
VERIFY(0x022CC760, &daNpcPhoto_c::setMtx);

/* 022CC840 */
void daNpcPhoto_c::setCollision(dCcD_Cyl* cyl, cXyz* center, f32 radius, f32 height) {
    WWHD_FUNC(0x022CC840, void, this, cyl, center, radius, height);
    cyl->SetC(center);
    cyl->SetR(radius);
    cyl->SetH(height);
    dComIfG_Ccsp_Set(cyl);
}
VERIFY(0x022CC840, &daNpcPhoto_c::setCollision);

/* 022CC8D0 */
cPhs_State daNpcPhoto_c::createInit() {
    WWHD_FUNC(0x022CC8D0, cPhs_State, this);
    int temp = 0xFF;
    u8 pathIndex = getPrmRailID();
    if (pathIndex != 0xFF) {
        dNpc_PathRun_setInf(&mPathRun, pathIndex, fopAcM_GetRoomNo(this), true);
        if (mPathRun.mPath.get() == nullptr) {
            return cPhs_ERROR_e;
        }
        /* HD: no dPath_GetNextRoomPath(mPathRun.getPath(), -1) */
        if (dComIfGs_isEventBit(0x1701 /* l_save_dat.field_0x02 */)) {
            gabi::Local<cXyz> point;
            dNpc_PathRun_getPoint(&mPathRun, point.get(), mPathRun.mIdx);
            old.pos.copy(*point);
            current.pos.copy(old.pos);
            dNpc_PathRun_incIdxLoop(&mPathRun);
            field_0x9A8 = 1;
            field_0x9C1 = 2;
        }
        temp = 0xFE;
    }
    gravity = -9.0f;
    mPhotoLinkBackEventIdx = dComIfGp_evmng_getEventIdx(STR(0x10020DB0) /* "PHOTO_LINK_BACK" */, 0xFF);
    mPhotoGetItemEventIdx = dComIfGp_evmng_getEventIdx(STR(0x10020E18) /* "PHOTO_GET_ITEM" */, 0xFF);
    mPhotoGetItem2EventIdx = dComIfGp_evmng_getEventIdx(STR(0x10020DC0) /* "PHOTO_GET_ITEM2" */, 0xFF);
    mPhotoGetPhotoEventIdx = dComIfGp_evmng_getEventIdx(STR(0x10020DD0) /* "PHOTO_GET_PHOTO" */, 0xFF);
    mPhotoGalleryEventIdx = dComIfGp_evmng_getEventIdx(STR(0x10020E08) /* "PHOTO_GALLERY" */, 0xFF);
    mPhotoCounterTalk0EventIdx = dComIfGp_evmng_getEventIdx(STR(0x10020DE0) /* "PHOTO_COUNTER_TALK0" */, 0xFF);
    mPhotoCounterTalk1EventIdx = dComIfGp_evmng_getEventIdx(STR(0x10020DF4) /* "PHOTO_COUNTER_TALK1" */, 0xFF);
    mPhotoDateUB4EventIdx = dComIfGp_evmng_getEventIdx(STR(0x10020E28) /* "PHOTO_DATE_UB4" */, 0xFF);
    field_0x9A6 = mPhotoCounterTalk0EventIdx;
    mEventCut.setActorInfo2(gabi::at<const char>(gabi::load<u32>(PHOTO_l_npc_staff_id)) /* "Po" */, this);
    field_0x9B6 = 0;
    field_0x9BC = false;
    field_0x9BD = false;
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    gabi::store<u8>(gabi::ea(this) + 0x389, 173); /* attention_info.distances[fopAc_Attn_TYPE_TALK_e] */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 173); /* attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] */
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0x0100000A); /* attention_info.flags */
    gabi::store<u32>(gabi::ea(this) + 0x104, 0x022CC68C); /* eventInfo.setXyCheckCB(daNpcPhoto_XyCheckCB) */
    gabi::store<u32>(gabi::ea(this) + 0x100, 0x022CC75C); /* eventInfo.setXyEventCB(daNpcPhoto_XyEventCB) */
    if (field_0x9C1 == 4) {
        field_0x9D7 = false;
        field_0x9D8 = false;
    } else {
        field_0x9D7 = l_npc_dat().field_0x52;
        field_0x9D8 = l_npc_dat().field_0x53;
    }
    field_0x9B2 = l_npc_dat().field_0x28;
    mObjAcch.CrrPos(dComIfG_Bgsp());
    f32 gnd = mObjAcch.GetGroundH();
    if (gnd != -1000000000.0f /* -G_CM3D_F_INF */) {
        current.pos.y = gnd;
        home.pos.y = gnd;
    }
    setMtx();
    J3DModel_calc(mpMorf->getModel());
    mStts.Init(temp, 0xFF, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mCyl.SetStts(&mStts);
    gabi::Local<cXyz> center;
    cXyz_pl(&current.pos, center.get(), &field_0x958);
    setCollision(&mCyl, center.get(), field_0x988, 150.0f);
    field_0x6F8[0].Set(gabi::at<dCcD_SrcCyl>(PHOTO_l_cyl_src2));
    field_0x6F8[0].SetStts(&mStts);
    field_0x6F8[1].Set(gabi::at<dCcD_SrcCyl>(PHOTO_l_cyl_src2));
    field_0x6F8[1].SetStts(&mStts);
    return cPhs_COMPLEATE_e;
}
VERIFY(0x022CC8D0, &daNpcPhoto_c::createInit);

/* 022CCC50 */
static cPhs_State phase_2(daNpcPhoto_c* i_this) {
    WWHD_FUNC(0x022CCC50, cPhs_State, i_this);
    cPhs_State phase_state = dComIfG_resLoad(i_this->getPhaseP(), photo_arcname());
    if (phase_state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, 0x022CC3E8 /* CheckCreateHeap */, 0)) {
            return i_this->createInit();
        }
        i_this->mpMorf = nullptr;
        return cPhs_ERROR_e;
    }
    return phase_state;
}
VERIFY(0x022CCC50, phase_2);

/* 022CCCD8 */
cPhs_State daNpcPhoto_c::_create() {
    WWHD_FUNC(0x022CCCD8, cPhs_State, this);
    /* static cPhs__Handler l_method[] = {phase_1, phase_2, NULL} (.data 0x101C52B4) */
    return dComLbG_PhaseHandler(&mPhs2, PHOTO_l_method, this);
}
VERIFY(0x022CCCD8, &daNpcPhoto_c::_create);

/* 022CCCEC */
static cPhs_State daNpc_PhotoCreate(void* i_this) {
    WWHD_FUNC(0x022CCCEC, cPhs_State, i_this);
    return static_cast<daNpcPhoto_c*>(i_this)->_create();
}
VERIFY(0x022CCCEC, daNpc_PhotoCreate);

/* 022CCCF0 */
bool daNpcPhoto_c::_delete() {
    WWHD_FUNC(0x022CCCF0, bool, this);
    dComIfG_resDelete(getPhaseP(), photo_arcname());
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return true;
}
VERIFY(0x022CCCF0, &daNpcPhoto_c::_delete);

/* 022CCD48 */
static BOOL daNpc_PhotoDelete(void* i_this) {
    WWHD_FUNC(0x022CCD48, BOOL, i_this);
    return static_cast<daNpcPhoto_c*>(i_this)->_delete();
}
VERIFY(0x022CCD48, daNpc_PhotoDelete);

/* 022D0C60 */
static u32 daObj_PrmAbstract(fopAc_ac_c* i_actor, u32 i_width, u32 i_shift) {
    WWHD_FUNC(0x022D0C60, u32, i_actor, i_width, i_shift);
    /* PowerPC slw/srw: shift counts 32..63 give 0 */
    u32 mask = ((i_width & 0x20) ? 0u : (1u << (i_width & 0x1F))) - 1;
    u32 prm = i_actor->mParameters;
    return ((i_shift & 0x20) ? 0u : (prm >> (i_shift & 0x1F))) & mask;
}
VERIFY(0x022D0C60, daObj_PrmAbstract);

/* 022D0904 __sinit_d_a_npc_photo_cpp: header statics (P = 0x104683A8, zeroed object at
 * 0x104683CC) and the cXyz statics */
static void __sinit_d_a_npc_photo_cpp() {
    WWHD_FUNC(0x022D0904, void);
    sinit_header_statics_z(0x104683A8, 0x101C52EC, 0x104683CC);
    static const f32 init[] = {
        /* 0x104683B4 l_counter_pos[2] */
        -490.0f, 0.0f, -10.0f, -260.0f, 0.0f, -250.0f,
    };
    for (int i = 0; i < 6; i++) {
        gabi::store<f32>(0x104683B4 + i * 4, init[i]);
    }
    /* 0x104683DC l_gallery_pos; 0x104683E8 HD: the second gallery position */
    static const f32 gallery[] = {-260.0f, 500.0f, 400.0f, -384.8f, 450.0f, 603.2f};
    for (int i = 0; i < 6; i++) {
        gabi::store<f32>(0x104683DC + i * 4, gallery[i]);
    }
    /* 0x104683F4 l_msg_camera[3][2] */
    static const f32 camera[] = {
        -1336.0f, -497.0f, 928.0f, 185.0f, -26.0f, 0.0f,
        -1213.0f, -902.0f, -244.0f, 284.0f, 36.0f, 280.0f,
        -29.0f, 252.0f, -1634.0f, 5.0f, -91.0f, 178.0f,
    };
    for (int i = 0; i < 18; i++) {
        gabi::store<f32>(0x104683F4 + i * 4, camera[i]);
    }
}
VERIFY(0x022D0904, __sinit_d_a_npc_photo_cpp);

/* 022D0AF8: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x022D0AF8, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x022D0AF8, SafeString_dt);

/* 022D0B0C: dCcD_Cyl::dCcD_Cyl (this TU's copy; HD: allocates when this == NULL) */
static dCcD_Cyl* dCcD_Cyl_ct_tu(dCcD_Cyl* p) {
    WWHD_FUNC(0x022D0B0C, dCcD_Cyl*, p);
    if (p == nullptr) {
        p = (dCcD_Cyl*)operator_new(0x130);
        if (p == nullptr) {
            return p;
        }
    }
    dCcD_Cyl_ct(p, PHOTO_AAB_VTBL);
    return p;
}
VERIFY(0x022D0B0C, dCcD_Cyl_ct_tu);

/* 022D0B98 */
static BOOL daNpc_PhotoIsDelete(void*) {
    WWHD_FUNC(0x022D0B98, BOOL, (void*)nullptr);
    return TRUE;
}
VERIFY(0x022D0B98, daNpc_PhotoIsDelete);

/* 022D0BA0: daNpcPhoto_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpcPhoto_dt(daNpcPhoto_c* p, s32 flags) {
    WWHD_FUNC(0x022D0BA0, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x028F0164, p->field_0x6F8, 2, 0x130, 0x02515A70 /* ~dCcD_Cyl */, 0, 0); /* __destroy_arr */
        gabi::call(0x02515A70, &p->mCyl, 2);                                    /* ~dCcD_Cyl */
        gabi::call(0x02515860, &p->mStts, 2);                                   /* ~dCcD_Stts */
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&p->mAcchCir) + 0x14), 2); /* ~cM3dGCir */
        gabi::store<u32>(gabi::ea(&p->mObjAcch) + 0x20, 0x10020CB8);            /* ~dBgS_ObjAcch */
        gabi::store<u32>(gabi::ea(&p->mObjAcch) + 0x14, 0x10020CC8);
        gabi::call(0x024EFD9C, &p->mObjAcch, 0);
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) {
            operator_delete(p);
        }
    }
}
VERIFY(0x022D0BA0, daNpcPhoto_dt);

/* 022D0C5C: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x022D0C5C, void);
}
VERIFY(0x022D0C5C, SafeString_assureTerminationImpl);
