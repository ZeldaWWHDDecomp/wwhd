/**
 * d_a_tsubo.cpp (WWHD)
 * Small, carriable objects (pots, skulls, barrels, ...): creation, specials (Bokoblin sticks),
 * mode inits, matrices, destruction.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tsubo.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Damage/position/bound functions: d_a_tsubo_dmg.cpp; modes, tensors, effects, execute/draw:
 * d_a_tsubo_mode.cpp.
 */
#include "d/actor/d_a_tsubo_main.h"

const gc_tu_data gc_tu = {
    .safeString = 0x10040AD4,
    .safeStringAssure = 0x024D0A88,
    .gndChkDt = {0x10040B1C, 0x10040B3C, 0x10040AFC},
    .headerStatics = {0x1046EA48, 0x101D2AF8, 0},
};

namespace daTsubo {
static void empty_fn(void* p);
static Act_c* Act_c_ct(Act_c* actor);
static void followEcallBack_ct_a(dPa_followEcallBack* callback);
static void followEcallBack_ct_b(dPa_followEcallBack* callback);
static u32 offsetPosGuard = 0;
static cXyz offsetPos = {0.0f, 0.0f, 0.0f};
static SpecActorSlot M_spec_act_data[3] = {{nullptr}, {nullptr}, {nullptr}};
inline gptr<fopAc_ac_c>* M_spec_act() { return reinterpret_cast<gptr<fopAc_ac_c>*>(M_spec_act_data); }
static f32 sAngleToRadian = 0.00009587380272568762f;


enum { fpcNm_BOKO_e = 0x1CF };


/* 024CA1AC */
void Act_c::prmZ_init() {
    if (m67E != 0) {
        return;
    }
    mPrmZ = home.angle.z;
    m67E = 1;
    home.angle.z = 0;
    current.angle.z = 0;
    shape_angle.z = 0;
}


/* 024CA5EC */
static u8 solidHeapCB(fopAc_ac_c* a_this) {
    return tsuboHeapResult(a_this); // HD: pass through the low result byte.
}


/* 024CA3D0 */
bool Act_c::create_heap() {
    J3DModelData* modelData =
        (J3DModelData*)dComIfG_getObjectRes(M_arcname(mType), data(this)->m6C, TSUBO_SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(1881, modelData != NULL) */
        JUT_ASSERT_fail("d_a_tsubo.cpp", 0x759, "modelData != 0");

    J3DModel* model = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    mpModel = model;
    if (model != nullptr && mType == 7) {
        /* HD: the material named 0x10040C30 gets 027F591C(model, material, 1) */
        gabi::Local<SafeString> name;
        name->mStringTop = gabi::guest_addr("bokuri2");
        name->__vtbl = TSUBO_SAFESTRING_VTBL;
        J3DModelData* md = model->mModelData;
        auto* matTbl = tsuboMaterialTable(md);
        empty_fn(name.get());               /* (empty per-TU SafeString helper) */
        u32 off = matTbl->mMaterialNameOffset;
        auto* tab = off != 0 ? reinterpret_cast<JUTNameTab*>(reinterpret_cast<u8*>(&matTbl->mMaterialNameOffset) + off) : nullptr;
        s32 idx = tsuboGetMaterialIndex(tab, tsuboStringTop(name));
        J3DMaterial* mat;
        if (idx < 0) {
            mat = nullptr;
        } else {
            u32 num = md->mMaterialNum;
            mat = tsuboMaterials(md);
            if ((u32)idx < num) {
                mat = reinterpret_cast<J3DMaterial*>(reinterpret_cast<u8*>(mat) + idx * 0x39C);
            }
        }
        tsuboSetMaterial((J3DModel*)mpModel, mat, 1);
    }

    bool ret = false;
    if (mType == 7) {
        void* brk_data = dComIfG_getObjectRes(M_arcname(mType), 7 /* dRes_INDEX_KMI00X_BRK_KMI_00X_e */, TSUBO_SAFESTRING_VTBL);
        if (brk_data == nullptr) /* JUT_ASSERT(1900, brk_data != NULL) */
            JUT_ASSERT_fail("d_a_tsubo.cpp", 0x76C, "brk_data != 0");
        void* brk = operator_new(0x78);
        if (brk != nullptr) {
            brk = tsuboBrkCtor(brk); 
        }
        mpBrk = (mDoExt_brkAnm*)brk;
        if (brk != nullptr) {
            if (tsuboBrkInit(brk, modelData, brk_data, false, J3DFrameCtrl::EMode_LOOP, 0, -1, false, 0, 1.0f)) {
                ret = true;
            }
        }
    } else {
        ret = true;
        mpBrk = nullptr;
    }
    return mpModel != nullptr && ret;
}


/* 024CA690 */
void Act_c::create_init_cull() {
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    cull_set_draw();
    /* HD: strcmp as a sead::SafeString comparison */
    if (tsuboStageEquals("sea", [] { return dComIfGp_ea() + 0x5134; })) {
        StageInfoView* info = tsuboStageInfo();
        f32 fVar1 = (f32)info->mCullPoint; /* dStage_stagInfo_GetCullPoint */
        if (fVar1 > 1.0f) {
            cullSizeFar = data(this)->mB0 / fVar1;
        }
    }
}


/* 024CA804 */
void Act_c::create_init_cc() {
    mStts.Init(data(this)->m10, 0xff, this);
    mCyl.Set(M_cyl_src);
    mCyl.SetStts(&mStts);
    mCyl.SetR((f32)data(this)->mAcchCirRad);
    mCyl.SetH((f32)data(this)->m70);
    mCyl.mGObjAt.mVec.copy(*cXyz_Zero); /* SetAtVec */
    mCyl.mGObjTg.mVec.copy(*cXyz_Zero); /* SetTgVec */
    /* OnTgShield (GObjTg SPrm bit 1) / OnTgNoHitMark (bit 4) */
    u32 bit = 4;
    if (data(this)->mFlag & Act_c::DATA_FLAG_4_e) {
        bit = 1;
    }
    mCyl.mGObjTg.mSPrm |= bit;
}


/* 024CA948 */
void Act_c::create_init_bgc() {
    mAcchCir.SetWall(30.0f, (f32)data(this)->mAcchCirRad);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle,
                      &shape_angle);
    Acch_ClrWaterNone(&mAcch);
    Acch_ClrRoofNone(&mAcch);
    mAcch.SetRoofCrrHeight((f32)data(this)->mAcchRoofHeight);
    m4F8 = -1000000000.0f; /* -G_CM3D_F_INF */
    m4FC = -1000000000.0f;
    m500 = -1000000000.0f;
    m504 = 0;
    m505 = 0;
}


/* 024CB5EC */
cPhs_State Act_c::_create() {
    prmZ_init();
    s32 type = prm_get_type(this);
    if (type >= 0x10) { /* HD: clamped */
        type = 0xF;
    }
    mType = type;

    /* fopAcM_ct(this, Act_c): HD calls the constructor out of line */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            Act_c_ct(this);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State PVar2;
    if (data(this)->mFlag & Act_c::DATA_FLAG_1_e) {
        PVar2 = cPhs_COMPLEATE_e;
    } else {
        PVar2 = dComIfG_resLoad(&mPhase, M_arcname(mType));
        if (PVar2 != cPhs_COMPLEATE_e) {
            return PVar2;
        }
    }

    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)&solidHeapCB), data(this)->mHeapSize)) {
        return cPhs_ERROR_e;
    }
    create_init_cull();
    create_init_cc();
    create_init_bgc();
    gravity = data(this)->mGravity;
    if (data(this)->mFlag & Act_c::DATA_FLAG_10_e) {
        actor_status = actor_status | fopAcStts_UNK80000_e;
    }
    if (data(this)->mFlag & Act_c::DATA_FLAG_20_e) {
        actor_status = actor_status | fopAcStts_UNK8000000_e;
    }

    fopAcM_posMoveF(this, nullptr);
    mAcch.CrrPos(dComIfG_Bgsp());
    Acch_ClrGroundLanding(&mAcch);

    bool iVar1 = prm_get_stick(this) != 0;
    bool iVar3 = prm_get_moveBg(this);
    if (iVar1 || iVar3) {
        current.pos.x = home.pos.x;
        current.pos.y = home.pos.y;
        current.pos.z = home.pos.z;
        if (iVar3 && tsuboChkMoveBg(dComIfG_Bgsp(), Acch_gnd(&mAcch))) {
            prm_off_moveBg(this);
            current.pos.y = mAcch.GetGroundH();
        }
    }

    m680 = 1;
    if (prm_get_spec(this) == 5 && !is_switch(this)) {
        m681 = 1;
        prm_off_moveBg(this);
    } else {
        m681 = 0x14;
    }

    m6FC = current.pos.y;
    m6EC = 0.0f;
    /* cXyz::Zero is loaded once here and reused for m7F4 / m814 below */
    f32 zx = cXyz_Zero->x, zy = cXyz_Zero->y, zz = cXyz_Zero->z;
    m6F0.x = zx;
    m6F0.y = zy;
    m6F0.z = zz;
    attn_onBit(this, fopAc_Attn_ACTION_CARRY_e);
    attention_info.distances[fopAc_Attn_TYPE_CARRY_e] = data(this)->mAttnDist;
    attention_info.position.x = current.pos.x;
    attention_info.position.y = current.pos.y + data(this)->mAttnY;
    attention_info.position.z = current.pos.z;
    m802 = 0;
    m800 = 0;
    if (mType == 7) {
        if (!prm_get_stick(this)) {
            m800 = 0x4B0; /* M_attrSpine.m30 */
        }
    }

    init_rot_clean();
    s32 iVar4 = prm_get_spec(this);
    if (iVar4 == 0 && !is_switch(this)) {
        mode_hide_init();
    } else if (iVar4 == 4) {
        mode_appear_init();
    } else {
        mode_wait_init();
    }

    m4F8 = -1000000000.0f;
    m67F = 0;
    m682 = 1;
    m683 = 0;
    m684 = 0;
    m685 = 0;
    m686 = 0;
    m811 = 0;

    if (mType == 2) {
        for (s32 i = 0; i < 3; i++) {
            m70C[i].m10 |= 1;
            m748[i].m10 |= 1;
        }
    }
    if (mType == 14) {
        eff_kutani_init();
    }
    spec_init();

    m7F4.set(zx, zy, zz);
    m814.set(zx, zy, zz);
    init_mtx();
    m810 = 0;
    return PVar2;
}


/* 024CBC58 */
bool Act_c::_is_delete() {
    spec_set_actor();
    spec_remove();
    spec_clr_actor();
    return true;
}


/* 024CBC98 */
bool Act_c::_delete() {
    for (s32 i = 0; i < 3; i++) {
        m70C[i].remove();
        m748[i].remove();
    }
    m784.remove();
    if (!(data(this)->mFlag & Act_c::DATA_FLAG_1_e)) {
        dComIfG_resDelete(&mPhase, M_arcname(mType)); /* dComIfG_resDeleteDemo */
    }
    return true;
}


/* 024CAD38 */
void Act_c::spec_make_boko(int arg1) {
    Act_c::M7A0* ptr = &m7A0[0];
    for (s32 i = 0; i < arg1; i++, ptr++) {
        ptr->m00 = fopAcM_create(fpcNm_BOKO_e, 0 /* daBoko_c::Type_BOKO_STICK_e */, &current.pos,
                                 home.roomNo, nullptr, nullptr, -1, 0);
        s16 a = M_data_spec_boko(i)->m00;
        ptr->m06 = a;
        ptr->m08 = a;
        ptr->m10 = M_data_spec_boko(i)->m04;
    }
}


/* 024CAF78 */
void Act_c::spec_init() {
    Act_c::M7A0* ptr = &m7A0[0];
    for (s32 i = 0; i < 3; i++, ptr++) {
        ptr->m00 = fpcM_ERROR_PROCESS_ID_e;
        ptr->m04 = 0;
        ptr->m06 = 0;
        ptr->m08 = 0;
        ptr->m0C = 0.0f;
        ptr->m10 = 0.0f;
        ptr->m14 = 0.0f;
        ptr->m18 = 0;
        M_spec_act()[i] = nullptr;
    }

    s32 iVar2 = prm_get_spec(this);
    if (spec_is_boko(iVar2)) {
        if (iVar2 == 1) {
            spec_make_boko(1);
        } else if (iVar2 == 2) {
            spec_make_boko(2);
        } else if (iVar2 == 3) {
            spec_make_boko(3);
        }
        /* (GameCube bug kept: ptr is one past the array, i.e. m7F4.x) */
        if (ptr->m00 != fpcM_ERROR_PROCESS_ID_e) {
            prm_set_cull_non(this);
        }
    }
    spec_set_actor();
    spec_carry_spec();
    spec_clr_actor();
}


/* 024CADD8 */
void Act_c::spec_set_actor() {
    s32 spec = prm_get_spec(this);
    if (spec != 0x3f && spec_is_boko(spec)) {
        Act_c::M7A0* ptr = &m7A0[0];
        for (s32 i = 0; i < 3; i++, ptr++) {
            if (ptr->m00 == fpcM_ERROR_PROCESS_ID_e) {
                M_spec_act()[i] = nullptr;
            } else if (!tsuboSearchByID(ptr->m00, &M_spec_act()[i])) {
                ptr->m00 = fpcM_ERROR_PROCESS_ID_e;
            }
        }
    }
}


/* 024CAF20 */
void Act_c::spec_clr_actor() {
    s32 spec = prm_get_spec(this);
    if (spec != 0x3f && spec_is_boko(spec)) {
        for (s32 i = 0; i < 3; i++) {
            M_spec_act()[i] = nullptr;
        }
    }
}


/* 024CAE78 */
void Act_c::spec_carry_spec() {
    s32 spec = prm_get_spec(this);
    if (spec != 0x3f && spec_is_boko(spec)) {
        Act_c::M7A0* ptr = &m7A0[0];
        for (s32 i = 0; i < 3; i++, ptr++) {
            if (ptr->m04 == 0 && M_spec_act()[i] != nullptr) {
                tsuboSetCarryNow(M_spec_act()[i], 0);
                ptr->m04 = 1;
            }
        }
    }
}


/* 024CBB58 */
void Act_c::spec_remove() {
    s32 spec = prm_get_spec(this);
    if (spec != 0x3f && spec_is_boko(spec)) {
        Act_c::M7A0* ptr = &m7A0[0];
        for (s32 i = 0; i < 3; i++, ptr++) {
            if (ptr->m04 != 0) {
                fopAc_ac_c* boko = M_spec_act()[i];
                if (boko != nullptr) {
                    boko->current.roomNo = fopAcM_GetRoomNo(this); /* fopAcM_SetRoomNo */
                    fopAcM_cancelCarryNow(boko);
                    boko->current.angle.y = (s16)(shape_angle.y - ptr->m06 - 0x8000);
                    fopAcM_SetSpeedF(boko, cM_rndF(9.0f));
                    boko->speed.y = cM_rndF(13.0f);
                    ptr->m00 = fpcM_ERROR_PROCESS_ID_e;
                }
            }
        }
    }
}


/* 024CB118 */
void Act_c::spec_mtx() {
    s32 spec = prm_get_spec(this);
    if (spec != 0x3f && spec_is_boko(spec)) {
        Act_c::M7A0* ptr = &m7A0[0];
        for (s32 i = 0; i < 3; i++, ptr++) {
            fopAc_ac_c* boko = M_spec_act()[i];
            if (boko != nullptr) {
                gabi::Local<Mtx34> sp08;
                PSMTXCopy(mDoMtx_stack_c::get(), sp08);
                mDoMtx_stack_push();
                mDoMtx_YrotS(mDoMtx_stack_c::get(), (s16)-ptr->m06);
                mDoMtx_stack_c::transM(0.0f, ptr->m10, M_data_spec_boko(i)->m08);
                mDoMtx_XrotM(mDoMtx_stack_c::get(), M_data_spec_boko(i)->m02);
                gabi::Local<Mtx34> sp38;
                tsuboMtxConcat(sp08, mDoMtx_stack_c::get(), sp38);
                /* daBoko_c::setMatrix: the model (HD +0x3B4) gets the matrix */
                J3DModel* model = reinterpret_cast<BokoModelView*>(boko)->mpModel.get();
                if (model != nullptr) {
                    J3DModel_setBaseTRMtx(model, sp38);
                }
                mDoMtx_stack_pop();
            }
        }
    }
}


/* 024CAAD0 */
void Act_c::mode_hide_init() {
    mCyl.OffAtSPrmBit(1);
    mCyl.OffTgSPrmBit(1);
    mCyl.OffCoSPrmBit(1);
    mAcch.SetRoofNone();
    mAcch.SetWallNone();
    mAcch.SetGrndNone();
    mAcch.SetWaterNone();
    mAcch.OffLineCheck();
    m67F = 1;
    fopAcM_SetSpeedF(this, 0.0f);
    attn_offBit(this, fopAc_Attn_ACTION_CARRY_e);
    m678 = 0;
}


/* 024CAB30 */
void Act_c::mode_appear_init() {
    mCyl.OffAtSPrmBit(1);
    mCyl.OffTgSPrmBit(1);
    mCyl.OnCoSPrmBit(1);
    mAcch.SetRoofNone();
    mAcch.SetWallNone();
    mAcch.ClrGrndNone();
    mAcch.SetWaterNone();
    mAcch.OffLineCheck();
    m67F = 1;
    fopAcM_SetSpeedF(this, 0.0f);
    scale.set(0.01f, 0.01f, 0.01f);
    attn_offBit(this, fopAc_Attn_ACTION_CARRY_e);

    if (prm_get_spec(this) == 4) {
        m808 = 0.2f; /* M_attrSpine.m10 */
        m80C = 0.2f; /* M_attrSpine.m14 */
        m802 = 0x50; /* M_attrSpine.m18 */
    } else {
        speed.y = cM_rndF(5.0f) + 29.0f;
        gravity = data(this)->mGravity;
        m802 = 0xb;
    }
    m678 = 1;
}


/* 024CAC44 */
void Act_c::mode_wait_init() {
    mCyl.OffAtSPrmBit(1);
    mCyl.OnTgSPrmBit(1);
    mCyl.OnCoSPrmBit(1);
    Acch_ClrRoofNone(&mAcch);
    Acch_ClrWallNone(&mAcch);
    mAcch.ClrGrndNone();
    Acch_ClrWaterNone(&mAcch);
    mAcch.OffLineCheck();
    m67F = 0;
    fopAcM_SetSpeedF(this, 0.0f);
    mStts.Init(data(this)->m10, 0xff, this);
    m802 = 0;
    m678 = 2;
}


/* 024CA5F0 */
void Act_c::cull_set_draw() {
    Act_c::Data_c* d = data(this);
    tsuboSetCullSphere(this, d->mCullSphX_Draw, d->mCullSphY_Draw, d->mCullSphZ_Draw, d->mCullSphR_Draw);
}


/* 024CBD48 */
void Act_c::cull_set_move() {
    Act_c::Data_c* d = data(this);
    tsuboSetCullSphere(this, d->mCullSphX_Move, d->mCullSphY_Move, d->mCullSphZ_Move, d->mCullSphR_Move);
}


/* 024CB564 */
void Act_c::init_mtx() {
    tsuboMtxIdentity(&mPoseMtx);
    for (int k = 0; k < 4; k++) /* m694 = ZeroQuat (0x101E9C38), a word copy */
        reinterpret_cast<be<u32>*>(m694)[k] = tsuboZeroQuatWords()[k];
    m6B0.x = cXyz_Zero->x;
    m6B0.y = cXyz_Zero->y;
    m6B0.z = cXyz_Zero->z;
    set_tensor(tsuboBaseX());
    set_mtx();
}


/* 024CB280 */
void Act_c::set_mtx() {
    J3DModel* model = mpModel;
    {
        gabi::Local<cXyz> s;
        cXyz_ml(&scale, s, data(this)->mModelScale);
        J3DModel_setBaseScale(model, s);
    }

    s32 type = mType;
    f32 fVar1;
    if (type == 13 || type == 7) {
        fVar1 = (f32)data(this)->mAcchRoofHeight * 0.5f * scale.y;
    } else {
        fVar1 = 0.0f;
    }

    Mtx34* now = mDoMtx_stack_c::get();
    if (m678 == 4 && m685) {
        PSMTXTrans(now, current.pos.x, current.pos.y + m6EC + fVar1, current.pos.z);
        tsuboMtxConcat(now, &mPoseMtx, now);
    } else {
        PSMTXTrans(now, current.pos.x, current.pos.y + data(this)->m04 + m6EC + fVar1, current.pos.z);
        tsuboMtxConcat(now, &mPoseMtx, now);
        mDoMtx_stack_c::transM(0.0f, -data(this)->m04, 0.0f);
    }
    J3DModel_setBaseTRMtx(mpModel, now);

    if (mType == 2) {
        if (offsetPosGuard == 0) {
            offsetPosGuard = 1;
            offsetPos.x = 0.0f;
            offsetPos.y = 85.0f;
            offsetPos.z = 0.0f;
        }
        PSMTXMultVec(now, &offsetPos, &m700);
    }
    spec_mtx();
}


/* 024CB088 */
void Act_c::set_tensor(const cXyz* pos) {
    tsuboQuatRotAxisRad(tsuboQuatStack(), pos, (f32)(s16)m68E * sAngleToRadian);
    tsuboMtxQuat(&mPoseMtx, tsuboQuatStack());
    mDoMtx_ZXYrotM(&mPoseMtx, shape_angle.x, shape_angle.y, shape_angle.z);
}


/* 024CAA54 */
void Act_c::init_rot_clean() {
    cSAngle_Val(&m688, cSAngle__0);
    cSAngle_Val(&m68A, cSAngle__0);
    cSAngle_Val(&m68C, cSAngle__0);
    cSAngle_Val(&m68E, cSAngle__0);
    cSAngle_Val(&m690, cSAngle__0);
    cSAngle_Val(&m692, (s16)current.angle.y);
}


/* 024CACEC */
void Act_c::eff_kutani_init() {
    m798 = (u8)(gabi::ftoi(cM_rndF(30.0f)) + 1);
}


/* 024CBDE8 */
void Act_c::set_senv(int arg1, int arg2) {
    gabi::Local<cXyz> pos; /* cXyz by value */
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    pos->z = current.pos.z;
    tsuboSoundSet(pos, arg1, mBsPcId, arg2);
}


namespace Method {
/* 024D09B8 */
cPhs_State Create(void* p) { return ((Act_c*)p)->_create(); }
}


namespace Method {
/* 024D09BC */
bool Delete(void* p) { return ((Act_c*)p)->_delete(); }
}


namespace Method {
/* 024D09C0 */
bool Execute(void* p) { return tsuboExecute(p); }
}


namespace Method {
/* 024D09C4 */
bool Draw(void* p) { return tsuboDraw(p); }
}


namespace Method {
/* 024D09C8 */
bool IsDelete(void* p) { return ((Act_c*)p)->_is_delete(); }
}

// HD: compiler-generated and template code of this translation unit.


/* 024CA194 */
static void followEcallBack_ct_a(dPa_followEcallBack* p) {
    dPa_followEcallBack_ct(p, 0, 0);
}


/* 024CA1A0 */
static void followEcallBack_ct_b(dPa_followEcallBack* p) {
    dPa_followEcallBack_ct(p, 0, 0);
}


/* 024CA1DC */
static Act_c* Act_c_ct(Act_c* i_this) {
    if (i_this == nullptr) {
        i_this = (Act_c*)operator_new(0x93C);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = TSUBO_ACT_VTBL;
    dBgS_ObjAcch_ct(&i_this->mAcch, TSUBO_OBJACCH_VT);
    dBgS_AcchCir_ct(&i_this->mAcchCir);
    {
        auto* ground = &i_this->mGndChkYogan;
        tsuboGroundCheckCtor(ground);
        ground->mbObjThrough = 1;
        ground->mbCamThrough = 0;
        ground->mbLinkThrough = 0;
        ground->mbArrowThrough = 0;
        ground->mbBombThrough = 0;
        ground->mbBoomerangThrough = 0;
        ground->mbRopeThrough = 0;
        ground->mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(ground));
        ground->cBgS_PolyInfo::__vtbl = TSUBO_YOGAN_VT.v20;
        ground->cBgS_Chk::__vtbl = TSUBO_YOGAN_VT.v10;
        ground->dBgS_GrpPassChk::__vtbl = TSUBO_YOGAN_VT.v4C;
        ground->dBgS_PolyPassChk::__vtbl = TSUBO_YOGAN_VT.v40;
        ground->mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(ground));
        ground->mGrp = 4; // Lava.

    }
    dCcD_Cyl_ct(&i_this->mCyl, TSUBO_CYL_AAB_VTBL);
    dCcD_Stts_ct(&i_this->mStts);
    cSAngle_ct(&i_this->m688);
    cSAngle_ct(&i_this->m68A);
    cSAngle_ct(&i_this->m68C);
    cSAngle_ct(&i_this->m68E);
    cSAngle_ct(&i_this->m690);
    cSAngle_ct(&i_this->m692);
    tsuboConstructArray(i_this->m70C, 3, sizeof(dPa_followEcallBack), gabi::guest_fn((const void*)&followEcallBack_ct_a)); /* __construct_array */
    tsuboConstructArray(i_this->m748, 3, sizeof(dPa_followEcallBack), gabi::guest_fn((const void*)&followEcallBack_ct_b));
    dPa_followEcallBack_ct(&i_this->m784, 0, 0);
    return i_this;
}


/* 024D09A8 */
static void thunk_eff_break_pinecone(Act_c* p) { tsuboBreakPinecone(p); }


/* 024D09AC */
static void thunk_eff_break_stool_a(Act_c* p) { tsuboBreakStool(p); }


/* 024D09B0 */
static void thunk_eff_break_skull(Act_c* p) { tsuboBreakSkull(p); }


/* 024D09B4 */
static void thunk_eff_break_stool_b(Act_c* p) { tsuboBreakStool(p); }


/* 024D09CC */
static void __sinit_d_a_tsubo_cpp() {
    sinit_header_statics(0x1046EA48, 0x101D2AF8);
}


/* 024D0A60 */
static void trivial_dt_a(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}


/* 024D0A74 */
static void trivial_dt_b(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}


/* 024D0A88 */
static void empty_fn(void* p) {
}


/* 024D0A8C */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        tsuboDestroyArray(i_this->m748, 3, sizeof(dPa_followEcallBack), gabi::guest_fn((const void*)&trivial_dt_b), 0, 0); /* __destroy_arr */
        tsuboDestroyArray(i_this->m70C, 3, sizeof(dPa_followEcallBack), gabi::guest_fn((const void*)&trivial_dt_b), 0, 0);
        dCcD_Stts_dt(&i_this->mStts, 2);
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        i_this->mGndChkYogan.cBgS_PolyInfo::__vtbl = gc_tu.gndChkDt[0]; /* ~dBgS_ObjGndChk_Yogan: base vtables */
        i_this->mGndChkYogan.dBgS_PolyPassChk::__vtbl = gc_tu.gndChkDt[1];
        i_this->mGndChkYogan.dBgS_GrpPassChk::__vtbl = gc_tu.gndChkDt[2];
        tsuboGroundCheckDtor(&i_this->mGndChkYogan, 0); /* cBgS_GndChk::~cBgS_GndChk */
        tsuboCircleDtor(&i_this->mAcchCir.m_cir, 2); /* mAcchCir's cM3dGCir */
        i_this->mAcch.dBgS_GrpPassChk::__vtbl = TSUBO_OBJACCH_VT.v20;
        i_this->mAcch.dBgS_PolyPassChk::__vtbl = TSUBO_OBJACCH_VT.v14;
        tsuboAcchDtor(&i_this->mAcch, 0); /* dBgS_Acch::~dBgS_Acch */
        tsuboActorDtor(i_this, 0);         /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

} // namespace daTsubo

using namespace daTsubo;
using namespace daTsubo::Method;

/* WWHD glue
ext 025D54C4 BOOL tsuboSearchByID(u32 id, gptr<fopAc_ac_c>* out)
ext 025D9D0C void tsuboSetCarryNow(fopAc_ac_c* actor, s32 stageLayer)
ext 025D6768 void tsuboSetCullSphere(fopAc_ac_c* actor, f32 x, f32 y, f32 z, f32 radius)
ext 028E9B14 void tsuboQuatRotAxisRad(Quaternion* quat, const cXyz* axis, f32 rad)
ext 028E8C78 void tsuboMtxQuat(Mtx34* matrix, Quaternion* quat)
ext 028E9098 void tsuboMtxIdentity(Mtx34* matrix)
ext 028E9108 void tsuboMtxConcat(const Mtx34* a, const Mtx34* b, Mtx34* result)
ext 024EEB2C BOOL tsuboChkMoveBg(dBgS* bg, cBgS_PolyInfo* poly)
ext 0255F458 void tsuboSoundSet(cXyz* pos, s32 sound, u32 id, s32 flags)
ext 027DF9B0 s32 tsuboGetMaterialIndex(JUTNameTab* table, const char* name)
ext 025E8154 BOOL tsuboBrkInit(void* animation, J3DModelData* model, void* key, s32 play, s32 attr, s16 start, s16 end, bool modify, s32 entry, f32 rate)
ext 02008E0C void tsuboGroundCheckCtor(cBgS_GndChk* check)
ext 028EFFD0 void tsuboConstructArray(void* array, s32 count, s32 stride, u32 constructor)
ext 027F591C void tsuboSetMaterial(J3DModel* model, J3DMaterial* material, s32 enabled)
ext 025E80D0 void* tsuboBrkCtor(void* animation)
ext 024CA3D0 u8 tsuboHeapResult(fopAc_ac_c* actor)
ext 024D08B8 void tsuboBreakPinecone(daTsubo::Act_c* actor)
ext 024D06F4 void tsuboBreakStool(daTsubo::Act_c* actor)
ext 024D07FC void tsuboBreakSkull(daTsubo::Act_c* actor)
ext 024CE71C bool tsuboExecute(void* actor)
ext 024CE994 bool tsuboDraw(void* actor)
ext 028F0164 void tsuboDestroyArray(void* array, s32 count, s32 stride, u32 destructor, s32 flags, s32 reserved)
ext 02008DAC void tsuboGroundCheckDtor(cBgS_GndChk* check, s32 flags)
ext 02018034 void tsuboCircleDtor(cM3dGCir_l* circle, s32 flags)
ext 024EFD9C void tsuboAcchDtor(dBgS_Acch* acch, s32 flags)
ext 025D50BC void tsuboActorDtor(fopAc_ac_c* actor, s32 flags)
ext 02048038 u32 tsuboPrmAbstract(const fopAc_ac_c* actor, s32 width, s32 shift)
ext 020065FC void tsuboAngleCtor(be<s16>* angle)
ext 02006638 void tsuboAngleCopy(be<s16>* angle, const be<s16>* value)
ext 02006584 void tsuboAngleSet(be<s16>* angle, s16 value)
data offsetPosGuard 1046EA7C nocheck
data offsetPos 1046EA64 nocheck
data M_spec_act_data 1046EA94 nocheck
data sAngleToRadian 10040C98
str 10040C38 "d_a_tsubo.cpp"
str 10040C58 "modelData != 0"
str 10040C48 "brk_data != 0"
str 10040C30 "bokuri2"
str 10040C78 "sea"
*/
