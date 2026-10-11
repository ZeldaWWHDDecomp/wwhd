/**
 * d_a_sk2.cpp (WWHD)
 * Object - swaying lily-pad lift (Forbidden Woods, "ksylf")
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_sk2.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_sk2.h"

// HD: constructor/destructor vtables and header statics belong to this translation unit.
const gc_tu_data gc_tu = {
    .safeString = 0x1003CEFC,
    .objAcch = {0x1003CF14, 0x1003CF34, 0x1003CF24},
    .objAcchDt = {0x1003CF24, 0x1003CF34},
    .headerStatics = {0x1046DF00, 0x101D0A0C, 0x1046DF0C},
};
static constexpr u32 SK2_VTBL = 0x1003CF44;
static constexpr u32 BGWDEFORM_VTBL = 0x1003CF54;

static Sk2::ResourceIndices sk2_dzb;
static Sk2::ResourceIndices sk2_bck;

// HD: preserve the two SafeString stack objects and the original construction order.
static void* getNamedResource(const char* arc, const char* name) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> n;
    a->mStringTop = gabi::guest_addr(arc);
    a->__vtbl = gc_tu.safeString;
    n->mStringTop = gabi::guest_addr(name);
    n->__vtbl = gc_tu.safeString;
    return sk2GetNamedResource(dComIfG_resControl(), a.get(), n.get());
}
void Sk2::dBgWDeform::MoveAfterAnmCalc(cXyz* vertices) {
    cXyz* old = mpVtx;
    mpVtx = vertices;
    if (old == nullptr)
        sk2CopyBackVertices(this);
    Move();
}

/* 02489098 */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel* model = j3dSys.getModel();
        sk2_class* i_this = (sk2_class*)model->getUserArea();
        s32 jntNo = ((Sk2::JointData*)joint)->mJntNo;

        if (i_this != nullptr && jntNo <= KSYLF_00_JNT_B1_e) {
            PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
            cMtx_YrotM(calc_mtx(), i_this->m2CE[jntNo].y);
            cMtx_XrotM(calc_mtx(), i_this->m2CE[jntNo].x);
            cMtx_ZrotM(calc_mtx(), i_this->m2CE[jntNo].z);

            gabi::Local<cXyz> offset;
            offset->x = 0.0f;
            offset->y = 0.0f;
            offset->z = 0.0f;
            MtxPosition(offset.get(), &i_this->m2E8[jntNo]);
            /* model->setAnmMtx(jntNo, *calc_mtx) */
            Mtx34* dst = model->getAnmMtx(jntNo);
            mtx_copy(dst, calc_mtx());
            PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx); /* J3DSys::mCurrentMtx */
        }
    }
    return TRUE;
}

/* 024891F4 */
static BOOL daSk2_Draw(sk2_class* i_this) {
    J3DModel* model = i_this->mpMorf->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0_PLIGHT, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);

    dComIfGd_setListBG();
    i_this->mpMorf->entryDL();
    dComIfGd_setList();
    return TRUE;
}

/* 02489290 */
void dousa_move(sk2_class* i_this) {
    switch ((u16)i_this->m2BA) {
    case 0:
        i_this->mpMorf->setFrame(i_this->m2C4);
        i_this->m2C0 = 0.5f;
        i_this->m2BA = i_this->m2BA + 1;
        /* fallthrough */
    case 1:
        if (!i_this->mpMorf->checkFrame(0.0f) && !i_this->mpMorf->checkFrame(18.0f)) {
            break;
        }
        i_this->m2BA = i_this->m2BA + 1;
        /* fallthrough */
    case 2:
        cLib_addCalc0(&i_this->m2C0, 0.3f, 0.3f);
        if (i_this->m2C0 < 0.1f) {
            i_this->m2C0 = 0.0f;
            i_this->m2BC = (s16)(REG_S(8, 1) + 100);
            i_this->m2BA = i_this->m2BA + 1;
        }
        break;
    case 3:
        i_this->m2BC = i_this->m2BC - 1;
        if (i_this->m2BC == 0) {
            i_this->m2C0 = 0.5f;
            fopAcM_seStart(i_this, JA_SE_OBJ_SHOKU_LIFT_MOVE, 0);
            i_this->m2BA = 1;
        }
        break;
    }

    for (s32 i = 1; i < 4; i++) {
        if (i_this->m2B4 == 0) {
            i_this->m2CE[i].x =
                (s16)gabi::ftoi((REG_F(8, 1) + 500.0f) * cM_ssin(i_this->m2CC * 1000 + (i * 5000)));
            i_this->m2CE[i].z =
                (s16)gabi::ftoi((REG_F(8, 1) + 500.0f) * cM_ssin(i_this->m2CC * 1000 + (i * 5000)));
        } else {
            i_this->m2CE[i].y =
                (s16)gabi::ftoi((REG_F(8, 1) + 500.0f) * cM_ssin((i_this->m2CC + i) * 1000));
        }
    }

    i_this->mpMorf->setPlaySpeed(i_this->m2C0);
    i_this->mpMorf->play(nullptr, 0, 0);
    i_this->m2CC = i_this->m2CC + 1;
}

/* 024895C8 */
static BOOL daSk2_Execute(sk2_class* i_this) {
    dousa_move(i_this);

    J3DModel* model = i_this->mpMorf->getModel();
    J3DModel_setBaseScale(model, &i_this->scale);
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->current.angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->current.angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->current.angle.z);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());

    sk2CopyBackVertices(i_this->mpBgW);
    i_this->mpMorf->calc();

    /* i_this->mpBgW->MoveAfterAnmCalc(i_this->mpMorf->getModel()) (HD: CPU vertex deformation) */
    sk2CalcVertices(&i_this->mCvtx, i_this->mpMorf->getModel());
    i_this->mpBgW->MoveAfterAnmCalc(i_this->mCvtx.mpVtx);
    return TRUE;
}

/* 02489710 */
static BOOL daSk2_IsDelete(sk2_class*) { return TRUE; }

/* 02489718 */
static BOOL daSk2_Delete(sk2_class* i_this) {
    dComIfG_resDelete(&i_this->mPhase, "Sk2");
    if (i_this->heap != nullptr) {
        cBgS_Release(dComIfG_Bgsp(), i_this->mpBgW);
    }
    return TRUE;
}

/* 02489770 */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    gabi::AutoFrame frame{gabi::current_function()};
    sk2_class* i_this = (sk2_class*)a_this;
    const char* arc = "Sk2";

    J3DModelData* pModelData;
    void* cvtx; /* HD */
    if (i_this->m2B4 == 0) {
        pModelData = (J3DModelData*)dComIfG_getObjectRes(arc, dRes_INDEX_SK2_BDL_KSYLF_00_e);
        cvtx = getNamedResource(arc, "ksylf_00.cvtx");
    } else {
        pModelData = (J3DModelData*)dComIfG_getObjectRes(arc, dRes_INDEX_SK2_BDL_KSYLF_01_e);
        cvtx = getNamedResource(arc, "ksylf_01.cvtx");
    }

    s32 bck = sk2_bck.indices[i_this->m2B4];
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(arc, bck);
    i_this->mpMorf =
        mDoExt_McaMorf::create(nullptr, pModelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP,
                               0.5f, 0, -1, 1, nullptr, 0x80000, 0x11000022);

    if (i_this->mpMorf == nullptr || i_this->mpMorf->getModel() == nullptr) {
        return FALSE;
    }

    i_this->mpMorf->getModel()->setUserArea((uintptr_t)i_this);

    for (u16 i = 0; i < i_this->mpMorf->getModel()->getModelData()->getJointNum(); i++) {
        J3DJoint* joint = i_this->mpMorf->getModel()->getModelData()->getJointNodePointer(i);
        joint->setCallBack(nodeCallBack);
    }

    /* new dBgWDeform() */
    Sk2::dBgWDeform* bgw = (Sk2::dBgWDeform*)operator_new(sizeof(Sk2::dBgWDeform));
    if (bgw != nullptr) {
        sk2BgW_ct(bgw);
        bgw->__vtbl = BGWDEFORM_VTBL;
    }
    i_this->mpBgW = bgw;

    if (i_this->mpBgW == nullptr) {
        return FALSE;
    }

    s32 dzb = sk2_dzb.indices[i_this->m2B4];
    cBgD_t* dzbData = (cBgD_t*)dComIfG_getObjectRes(arc, dzb);
    if (sk2BgWSet(i_this->mpBgW, dzbData, 0)) {
        return FALSE;
    }
    if (!sk2InitVertices(&i_this->mCvtx, cvtx)) { /* HD */
        return FALSE;
    }
    return TRUE;
}

/* 02489A00 */
static cPhs_State daSk2_Create(fopAc_ac_c* a_this) {
    sk2_class* i_this = (sk2_class*)a_this;
    /* fopAcM_ct(a_this, sk2_class) (HD/USA: before the resource load) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            i_this->mCvtx.mpData = nullptr;
            i_this->mCvtx.mpVtx = nullptr;
            a_this->__vtbl = SK2_VTBL;
            __ct(&i_this->mAcchCir);
            __ct(&i_this->mAcch);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    cPhs_State PVar1 = dComIfG_resLoad(&i_this->mPhase, "Sk2");
    if (PVar1 == cPhs_COMPLEATE_e) {
        i_this->m2B4 = fopAcM_GetParam(a_this);
        i_this->m2B5 = fopAcM_GetParam(a_this) >> 8;

        if (i_this->m2B4 == 0xff) {
            i_this->m2B4 = 0;
        }

        if (i_this->m2B5 != 0xff) {
            i_this->m2C4 = (f32)(u8)i_this->m2B5;
            if (i_this->m2C4 > 35.0f) {
                i_this->m2C4 = 0.0f;
            }
        }

        i_this->m2B6 = fopAcM_GetParam(a_this) >> 0x18;
        if (i_this->m2B6 != 0xff && fopAcM_isSwitch(a_this, i_this->m2B6)) {
            return cPhs_ERROR_e;
        }

        if (!fopAcM_entrySolidHeap(a_this, useHeapInit, 0x3b20)) {
            return cPhs_ERROR_e;
        }

        a_this->cullMtx =
            gabi::ea(J3DModel_getBaseTRMtx(i_this->mpMorf->getModel())); /* fopAcM_SetMtx */
        J3DModel* model = i_this->mpMorf->getModel();
        J3DModel_setBaseScale(model, &a_this->scale);
        mDoMtx_stack_c::transS(a_this->current.pos.x, a_this->current.pos.y, a_this->current.pos.z);
        mDoMtx_stack_c::YrotM(a_this->current.angle.y);
        mDoMtx_XrotM(mDoMtx_stack_c::get(), a_this->current.angle.x);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), a_this->current.angle.z);
        J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());

        dousa_move(i_this); /* HD */
        i_this->mpMorf->calc();

        sk2SetMin(a_this, -800.0f, -200.0f, -400.0f);
        sk2SetMax(a_this, 500.0f, 1000.0f, 1000.0f);
        a_this->attention_info.flags = 0;

        if (dBgS_Regist(dComIfG_Bgsp(), i_this->mpBgW, a_this)) {
            return cPhs_ERROR_e;
        }
    }
    return PVar1;
}

/* 02489CF4 */
static void __sinit_d_a_sk2_cpp() { __sinit_header_statics(); }

/* 02489D88 */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 02489D9C */
static void dBgWDeform_empty_v(void*) {}

/* 02489DA0 */
static void sk2_class_dt(sk2_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(&i_this->mAcch);
        __dt(&i_this->mAcchCir);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02489E24 */
static void SafeString_assureTerminationImpl(void*) {}

// clang-format off
/* WWHD glue: resources, tables and HD deformation calls.
data sk2_dzb 101D09DC
data sk2_bck 101D09E4
str 1003CFD0 "Sk2" in 02489718
str 1003CFD4 "Sk2" in 02489770
str 1003D014 "Sk2" in 02489A00
str 1003CFD8 "ksylf_00.cvtx"
str 1003CFE8 "ksylf_01.cvtx"
ext 025D672C void sk2SetMin(fopAc_ac_c*, f32, f32, f32)
ext 025D673C void sk2SetMax(fopAc_ac_c*, f32, f32, f32)
ext 024F5B08 void sk2CopyBackVertices(dBgW*)
ext 024F5A18 void sk2BgW_ct(dBgW*)
ext 024F49E8 BOOL sk2BgWSet(Sk2::dBgWDeform*, cBgD_t*, u32)
ext 025EDD64 BOOL sk2InitVertices(Sk2::CpuVertices*, void*)
ext 025EDE0C void sk2CalcVertices(Sk2::CpuVertices*, J3DModel*)
ext 02606900 void* sk2GetNamedResource(dRes_control_c*, SafeString*, SafeString*)
*/
