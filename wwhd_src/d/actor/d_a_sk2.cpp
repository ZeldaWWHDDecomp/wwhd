/**
 * d_a_sk2.cpp (WWHD)
 * Object - swaying lily-pad lift (Forbidden Woods, "ksylf")
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_sk2.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x1003CEFC    /* this TU's sead::SafeString vtable */
#define SK2_VTBL 0x1003CF44           /* HD: sk2_class vtable */
#define BGWDEFORM_VTBL 0x1003CF54     /* this TU's dBgWDeform vtable (at +4) */
#define sk2_dzb(i) gabi::load<s32>(0x101D09DC + 4 * (i)) /* {DZB_KSYLF_00, DZB_KSYLF_01} */
#define sk2_bck(i) gabi::load<s32>(0x101D09E4 + 4 * (i)) /* {BCK_KSYLF_00, BCK_KSYLF_01} */

enum {
    dRes_INDEX_SK2_BDL_KSYLF_00_e = 9,
    dRes_INDEX_SK2_BDL_KSYLF_01_e = 0xA,
};
enum { JA_SE_OBJ_SHOKU_LIFT_MOVE = 0x3823 };
enum { TEV_TYPE_BG0_PLIGHT = 0x5B };

/* HD: CPU vertex deformation for the deforming collision ("<model>.cvtx" resource): {data, vertex
 * buffer}; init 025EDD64(this, cvtx) allocates the buffer, calc 025EDE0C(this, model) fills it */
struct mDoExt_cvtx_l {
    /* 0x0 */ be<u32> mpData;
    /* 0x4 */ be<u32> mpVtx;
};

struct sk2_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ be<u8> m2B4;
    /* 0x3D1 */ be<u8> m2B5;
    /* 0x3D2 */ be<u8> m2B6;
    /* 0x3D3 */ u8 _3D3[3];
    /* 0x3D6 */ be<s16> m2BA;
    /* 0x3D8 */ be<s16> m2BC;
    /* 0x3DA */ u8 _3DA[2];
    /* 0x3DC */ be<f32> m2C0;
    /* 0x3E0 */ be<f32> m2C4;
    /* 0x3E4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3E8 */ be<s16> m2CC;
    /* 0x3EA */ csXyz m2CE[4];
    /* 0x402 */ u8 _402[2];
    /* 0x404 */ cXyz m2E8[4];
    /* 0x434 */ u8 m318[4];
    /* 0x438 */ gptr<dBgW> mpBgW;          /* dBgWDeform */
    /* 0x43C */ mDoExt_cvtx_l mCvtx;       /* HD */
    /* 0x444 */ dBgS_AcchCir mAcchCir;
    /* 0x484 */ dBgS_Acch mAcch;           /* dBgS_ObjAcch */
};
WWHD_OFFSET(sk2_class, mpMorf, 0x3E4);
WWHD_OFFSET(sk2_class, m2CE, 0x3EA);
WWHD_OFFSET(sk2_class, m2E8, 0x404);
WWHD_OFFSET(sk2_class, mpBgW, 0x438);
WWHD_OFFSET(sk2_class, mAcch, 0x484);
WWHD_SIZE(sk2_class, 0x648);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD J3D (as in d_a_kamome): j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at 0x104B4868; a
 * model's joint matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices); user area +0xB8 */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
struct J3DModel_l {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<J3DMtxBlock_l> mpMtxBlock;
    /* 0x30 */ u8 _30[0xAC - 0x30];
    /* 0xAC */ be<u32> mModelData;
    /* 0xB0 */ u8 _B0[8];
    /* 0xB8 */ be<u32> mUserArea;
};
static Mtx34* getAnmMtx(J3DModel_l* model, s32 jntNo) {
    J3DMtxBlock_l* blk = model->mpMtxBlock;
    blk->mFlags |= 0x10; /* HD: marks the joint matrices dirty */
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30);
}
/* 02606900 HD: dRes_control_c::getRes(const SafeString& arc, const SafeString& name) */
static inline void* dComIfG_getObjectRes_name(const char* arc, const char* name) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> n;
    a->mStringTop = gabi::ea(arc);
    a->__vtbl = SAFESTRING_VTBL;
    n->mStringTop = gabi::ea(name);
    n->__vtbl = SAFESTRING_VTBL;
    return gabi::call<void*>(0x02606900, dComIfG_resControl(), a.get(), n.get());
}
/* 027F3F94 (matcher: __nw): the model data's joint tree header, joint count (u16) at +8 */
static inline u16 J3DModelData_getJointNum(u32 md) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, md) + 8); }
/* J3DModelData::getJointNodePointer(i) (HD inline, range-checked): count +4, joints (0x1C) at +8 */
static inline u32 J3DModelData_getJointNodePointer(u32 md, u32 i) {
    u32 base = gabi::load<u32>(md + 8);
    if (i < gabi::load<u32>(md + 4)) return base + i * 0x1C;
    return base;
}
static inline void fopAcM_SetMin(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D672C, a, x, y, z); }
static inline void fopAcM_SetMax(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D673C, a, x, y, z); }
static inline void dBgWSv_CopyBackVtx(dBgW* w) { gabi::call(0x024F5B08, w); }
/* 024F49E8 dBgWDeform::Set(cBgD_t*, u32 flags) (HD: no model argument) */
static inline BOOL dBgWDeform_Set(dBgW* w, cBgD_t* dzb, u32 flags) { return gabi::call<BOOL>(0x024F49E8, w, dzb, flags); }
static inline BOOL mDoExt_cvtx_init(mDoExt_cvtx_l* c, void* data) { return gabi::call<BOOL>(0x025EDD64, c, data); }
static inline void mDoExt_cvtx_calc(mDoExt_cvtx_l* c, J3DModel* m) { gabi::call(0x025EDE0C, c, m); }
/* dBgWDeform::MoveAfterAnmCalc (HD inline): vertices from the CPU deformation, at +0x90 */
static inline void dBgWDeform_MoveAfterAnmCalc(dBgW* w, u32 vtx) {
    u32 old = gabi::load<u32>(gabi::ea(w) + 0x90);
    gabi::store<u32>(gabi::ea(w) + 0x90, vtx);
    if (old == 0) dBgWSv_CopyBackVtx(w);
    dBgW_Move(w);
}

/* 02489098 */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x02489098, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel_l* model = gabi::at<J3DModel_l>(gabi::load<u32>(0x104B462C));
        sk2_class* i_this = gabi::at<sk2_class>(model->mUserArea);
        s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);

        if (i_this != nullptr && jntNo <= 3 /* KSYLF_00_JNT_B1_e */) {
            PSMTXCopy(getAnmMtx(model, jntNo), calc_mtx());
            cMtx_YrotM(calc_mtx(), i_this->m2CE[jntNo].y);
            cMtx_XrotM(calc_mtx(), i_this->m2CE[jntNo].x);
            cMtx_ZrotM(calc_mtx(), i_this->m2CE[jntNo].z);

            gabi::Local<cXyz> offset;
            offset->x = 0.0f;
            offset->y = 0.0f;
            offset->z = 0.0f;
            MtxPosition(offset.get(), &i_this->m2E8[jntNo]);
            /* model->setAnmMtx(jntNo, *calc_mtx) */
            Mtx34* dst = getAnmMtx(model, jntNo);
            mtx_copy(dst, calc_mtx());
            PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868)); /* J3DSys::mCurrentMtx */
        }
    }
    return TRUE;
}
VERIFY(0x02489098, nodeCallBack);

/* 024891F4 */
static BOOL daSk2_Draw(sk2_class* i_this) {
    WWHD_FUNC(0x024891F4, BOOL, i_this);
    J3DModel* model = i_this->mpMorf->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0_PLIGHT, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);

    dComIfGd_setListBG();
    i_this->mpMorf->entryDL();
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x024891F4, daSk2_Draw);

/* 02489290 */
void dousa_move(sk2_class* i_this) {
    WWHD_FUNC(0x02489290, void, i_this);
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
            i_this->m2CE[i].x = (s16)gabi::ftoi((REG_F(8, 1) + 500.0f) * cM_ssin(i_this->m2CC * 1000 + (i * 5000)));
            i_this->m2CE[i].z = (s16)gabi::ftoi((REG_F(8, 1) + 500.0f) * cM_ssin(i_this->m2CC * 1000 + (i * 5000)));
        } else {
            i_this->m2CE[i].y = (s16)gabi::ftoi((REG_F(8, 1) + 500.0f) * cM_ssin((i_this->m2CC + i) * 1000));
        }
    }

    i_this->mpMorf->setPlaySpeed(i_this->m2C0);
    i_this->mpMorf->play(nullptr, 0, 0);
    i_this->m2CC = i_this->m2CC + 1;
}
VERIFY(0x02489290, dousa_move);

/* 024895C8 */
static BOOL daSk2_Execute(sk2_class* i_this) {
    WWHD_FUNC(0x024895C8, BOOL, i_this);
    dousa_move(i_this);

    J3DModel* model = i_this->mpMorf->getModel();
    J3DModel_setBaseScale(model, &i_this->scale);
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->current.angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->current.angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->current.angle.z);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());

    dBgWSv_CopyBackVtx(i_this->mpBgW);
    i_this->mpMorf->calc();

    /* i_this->mpBgW->MoveAfterAnmCalc(i_this->mpMorf->getModel()) (HD: CPU vertex deformation) */
    mDoExt_cvtx_calc(&i_this->mCvtx, i_this->mpMorf->getModel());
    dBgWDeform_MoveAfterAnmCalc(i_this->mpBgW, i_this->mCvtx.mpVtx);
    return TRUE;
}
VERIFY(0x024895C8, daSk2_Execute);

/* 02489710 */
static BOOL daSk2_IsDelete(sk2_class*) {
    WWHD_FUNC(0x02489710, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02489710, daSk2_IsDelete);

/* 02489718 */
static BOOL daSk2_Delete(sk2_class* i_this) {
    WWHD_FUNC(0x02489718, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, STR(0x1003CFD0) /* "Sk2" */);
    if (i_this->heap != nullptr) {
        cBgS_Release(dComIfG_Bgsp(), i_this->mpBgW);
    }
    return TRUE;
}
VERIFY(0x02489718, daSk2_Delete);

/* 02489770 */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02489770, BOOL, a_this);
    sk2_class* i_this = (sk2_class*)a_this;
    const char* arc = STR(0x1003CFD4); /* "Sk2" */

    J3DModelData* pModelData;
    void* cvtx; /* HD */
    if (i_this->m2B4 == 0) {
        pModelData = (J3DModelData*)dComIfG_getObjectRes(arc, dRes_INDEX_SK2_BDL_KSYLF_00_e, SAFESTRING_VTBL);
        cvtx = dComIfG_getObjectRes_name(arc, STR(0x1003CFD8) /* "ksylf_00.cvtx" */);
    } else {
        pModelData = (J3DModelData*)dComIfG_getObjectRes(arc, dRes_INDEX_SK2_BDL_KSYLF_01_e, SAFESTRING_VTBL);
        cvtx = dComIfG_getObjectRes_name(arc, STR(0x1003CFE8) /* "ksylf_01.cvtx" */);
    }

    s32 bck = sk2_bck(i_this->m2B4);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(arc, bck, SAFESTRING_VTBL);
    i_this->mpMorf = mDoExt_McaMorf::create(nullptr, pModelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 0.5f,
                                            0, -1, 1, nullptr, 0x80000, 0x11000022);

    if (i_this->mpMorf == nullptr || i_this->mpMorf->getModel() == nullptr) {
        return FALSE;
    }

    gabi::at<J3DModel_l>(gabi::ea(i_this->mpMorf->getModel()))->mUserArea = gabi::ea(i_this);

    for (u16 i = 0; i < J3DModelData_getJointNum(gabi::ea(J3DModel_getModelData(i_this->mpMorf->getModel()))); i++) {
        u32 jnt = J3DModelData_getJointNodePointer(gabi::ea(J3DModel_getModelData(i_this->mpMorf->getModel())), i);
        gabi::store<u32>(jnt + 8, 0x02489098 /* nodeCallBack */); /* setCallBack */
    }

    /* new dBgWDeform() */
    dBgW* bgw = (dBgW*)operator_new(0xC4);
    if (bgw != nullptr) {
        gabi::call(0x024F5A18, bgw); /* dBgW::dBgW */
        gabi::store<u32>(gabi::ea(bgw) + 4, BGWDEFORM_VTBL);
    }
    i_this->mpBgW = bgw;

    if (i_this->mpBgW == nullptr) {
        return FALSE;
    }

    s32 dzb = sk2_dzb(i_this->m2B4);
    cBgD_t* dzbData = (cBgD_t*)dComIfG_getObjectRes(arc, dzb, SAFESTRING_VTBL);
    if (dBgWDeform_Set(i_this->mpBgW, dzbData, 0)) {
        return FALSE;
    }
    if (!mDoExt_cvtx_init(&i_this->mCvtx, cvtx)) { /* HD */
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x02489770, useHeapInit);

/* 02489A00 */
static cPhs_State daSk2_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02489A00, cPhs_State, a_this);
    sk2_class* i_this = (sk2_class*)a_this;
    /* fopAcM_ct(a_this, sk2_class) (HD/USA: before the resource load) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            i_this->mCvtx.mpData = 0;
            i_this->mCvtx.mpVtx = 0;
            a_this->__vtbl = SK2_VTBL;
            dBgS_AcchCir_ct(&i_this->mAcchCir);
            dBgS_ObjAcch_ct(&i_this->mAcch, dBgS_ObjAcch_vt{0x1003CF14, 0x1003CF34, 0x1003CF24});
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    cPhs_State PVar1 = dComIfG_resLoad(&i_this->mPhase, STR(0x1003D014) /* "Sk2" */);
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

        if (!fopAcM_entrySolidHeap(a_this, 0x02489770 /* useHeapInit */, 0x3b20)) {
            return cPhs_ERROR_e;
        }

        a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpMorf->getModel())); /* fopAcM_SetMtx */
        J3DModel* model = i_this->mpMorf->getModel();
        J3DModel_setBaseScale(model, &a_this->scale);
        mDoMtx_stack_c::transS(a_this->current.pos.x, a_this->current.pos.y, a_this->current.pos.z);
        mDoMtx_stack_c::YrotM(a_this->current.angle.y);
        mDoMtx_XrotM(mDoMtx_stack_c::get(), a_this->current.angle.x);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), a_this->current.angle.z);
        J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());

        dousa_move(i_this); /* HD */
        i_this->mpMorf->calc();

        fopAcM_SetMin(a_this, -800.0f, -200.0f, -400.0f);
        fopAcM_SetMax(a_this, 500.0f, 1000.0f, 1000.0f);
        gabi::store<u32>(gabi::ea(a_this) + 0x39C, 0); /* attention_info.flags */

        if (dBgS_Regist(dComIfG_Bgsp(), i_this->mpBgW, a_this)) {
            return cPhs_ERROR_e;
        }
    }
    return PVar1;
}
VERIFY(0x02489A00, daSk2_Create);

/* 02489CF4 */
static void __sinit_d_a_sk2_cpp() {
    WWHD_FUNC(0x02489CF4, void, (u32)0);
    sinit_header_statics(0x1046DF00, 0x101D0A0C);
}
VERIFY(0x02489CF4, __sinit_d_a_sk2_cpp);

/* 02489D88: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02489D88, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x02489D88, SafeString_dt);

/* 02489D9C: empty virtual in this TU's dBgWDeform vtable copy (slot 0x1003CFA8) */
static void dBgWDeform_empty_v(void*) {
    WWHD_FUNC(0x02489D9C, void, (u32)0);
}
VERIFY(0x02489D9C, dBgWDeform_empty_v);

/* 02489DA0: sk2_class deleting destructor (compiler-generated) */
static void sk2_class_dt(sk2_class* i_this, s32 flags) {
    WWHD_FUNC(0x02489DA0, void, i_this, flags);
    if (i_this != nullptr) {
        /* ~dBgS_ObjAcch: this TU's vtables, then dBgS_Acch::~dBgS_Acch */
        gabi::store<u32>(gabi::ea(&i_this->mAcch) + 0x20, 0x1003CF24);
        gabi::store<u32>(gabi::ea(&i_this->mAcch) + 0x14, 0x1003CF34);
        gabi::call(0x024EFD9C, &i_this->mAcch, 0);
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&i_this->mAcchCir) + 0x14), 2); /* ~cM3dGCir */
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02489DA0, sk2_class_dt);

/* 02489E24: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x02489E24, void, (u32)0);
}
VERIFY(0x02489E24, SafeString_assureTerminationImpl);
