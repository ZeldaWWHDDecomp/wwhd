/**
 * d_a_acorn_leaf.cpp (WWHD)
 * Object - Forest Haven acorn leaf (holds a tsubo "acorn" and drops a new one)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_acorn_leaf.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define m_arcname STR(0x10006D80) /* "VigaH" */
#define SAFESTRING_VTBL 0x10006CB4  /* this TU's copy of the sead::SafeString vtable */
#define FILE_NAME STR(0x10006D24)   /* "d_a_acorn_leaf.cpp" */
#define DAALEAF_VTBL 0x10006D04
#define l_cyl_src gabi::at<dCcD_SrcCyl>(0x1018F6D0)
#define acorn_offset gabi::at<cXyz>(0x1046137C)

enum {
    dRes_INDEX_VIGAH_BCK_VIGAH_e = 4,
    dRes_INDEX_VIGAH_BDL_VIGAH_e = 7,
};
enum { fpcNm_TSUBO_e = 0x1C5 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025F18EC mDoMtx_XrotS(Mtx, s16) (as in d_a_mo2.h) */
static inline void mDoMtx_XrotS_l(Mtx34* m, s16 x) { gabi::call(0x025F18EC, m, x); }
/* 025D5B20 fopAcM_createChildFromOffset(name, parentId, param, pos, roomNo, angle, scale, s8 subtype,
 * createFunc (stack)) */
static inline fpc_ProcID fopAcM_createChildFromOffset(s16 name, fpc_ProcID parent, u32 param, cXyz* pos, s32 roomNo,
                                                      csXyz* angle, cXyz* scale, s32 subtype, u32 createFunc) {
    return gabi::call<fpc_ProcID>(0x025D5B20, name, parent, param, pos, roomNo, angle, scale, subtype, createFunc);
}
/* 025D54C4 fopAcM_SearchByID(id, fopAc_ac_c** out) (out of line) */
static inline BOOL fopAcM_SearchByID_out(fpc_ProcID id, be<u32>* out) { return gabi::call<BOOL>(0x025D54C4, id, out); }
/* daPy_py_c::getGrabActorID(): virtual (vtable +0xB4, slot +0xBC) */
static inline fpc_ProcID daPy_getGrabActorID(fopAc_ac_c* player) {
    return gabi::call_ptr<fpc_ProcID>(gabi::load<u32>(player->__vtbl + 0xBC), player);
}
/* mDoExt_bckAnm::remove(J3DModelData*) (inline): getJointNodePointer(0)->setMtxCalc(NULL) (HD: the
 * root joint at model data +8, its calc pointer at +0x14) */
static inline void mDoExt_bckAnm_remove(J3DModelData* d) {
    u32 joint = gabi::load<u32>(gabi::ea(d) + 8);
    gabi::store<u32>(joint + 0x14, 0);
}
/* fabsf */
static inline f32 fabsf_g(f32 x) { return x < 0.0f || (x == 0.0f && std::signbit(x)) ? -x : x; }

struct daAleaf_c : fopAc_ac_c {
    BOOL CreateHeap();
    void CreateInit();
    void create_acorn();
    fpc_ProcID create_acorn_sub(bool);
    cPhs_State _create();
    void set_mtx();
    bool _execute();
    bool _draw();

    /* 0x3AC */ request_of_phase_process_class unk_290;
    /* 0x3B4 */ gptr<J3DModel> unk_298;
    /* 0x3B8 */ dCcD_Stts unk_29C;
    /* 0x3F4 */ dCcD_Cyl unk_2D8;
    /* 0x524 */ mDoExt_bckAnm unk_408;
    /* 0x5B0 */ be<u8> unk_418;
    /* 0x5B1 */ be<u8> unk_419;
    /* 0x5B2 */ be<u8> unk_41A;
    /* 0x5B3 */ be<u8> unk_41B;
    /* 0x5B4 */ be<s32> unk_41C;
    /* 0x5B8 */ be<s32> unk_420;
    /* 0x5BC */ be<u32> unk_424;
    /* 0x5C0 */ be<u32> unk_428;
    /* 0x5C4 */ be<f32> unk_42C;
};
WWHD_OFFSET(daAleaf_c, unk_29C, 0x3B8);
WWHD_OFFSET(daAleaf_c, unk_2D8, 0x3F4);
WWHD_OFFSET(daAleaf_c, unk_408, 0x524);
WWHD_OFFSET(daAleaf_c, unk_418, 0x5B0);
WWHD_OFFSET(daAleaf_c, unk_42C, 0x5C4);
WWHD_SIZE(daAleaf_c, 0x5C8);

/* 02048038 daObj::PrmAbstract(actor, width, shift) (this TU's out-of-line copy):
 * (mParameters >> shift) & ((1 << width) - 1), with PowerPC shifts (amounts 32..63 give 0) */
static inline u32 ppc_slw(u32 v, u32 n) { return (n & 0x20) ? 0 : v << (n & 0x1F); }
static inline u32 ppc_srw(u32 v, u32 n) { return (n & 0x20) ? 0 : v >> (n & 0x1F); }
static u32 daObj_PrmAbstract_acorn(fopAc_ac_c* a, s32 width, s32 shift) {
    WWHD_FUNC(0x02048038, u32, a, width, shift);
    return ppc_srw(fopAcM_GetParam(a), shift) & (ppc_slw(1, width) - 1);
}
VERIFY(0x02048038, daObj_PrmAbstract_acorn);

namespace daAleaf_prm {
inline u32 getItemBitNo(daAleaf_c* actor) { return (fopAcM_GetParam(actor) >> 6) & 0x7F; }
inline u32 getItemNo(daAleaf_c* actor) { return fopAcM_GetParam(actor) & 0x3F; }
};  // namespace daAleaf_prm

/* daTsubo::Act_c::prm_make_acorn (inline) */
static inline u32 prm_make_acorn(bool arg1, int item_no, int bit_no) {
    u32 a1 = (arg1 ? 0x4 : 0x3F);
    u32 param = 0;
    param |= ((bit_no & 0x7F) << 16);
    param |= ((item_no & 0x3F) << 0) | (a1 << 8);
    param |= ((7 << 24) | (1u << 31));
    return param;
}

/* 020476CC */
static BOOL CheckCreateHeap(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x020476CC, BOOL, i_actor);
    return gabi::call<BOOL>(0x020475B4, i_actor); /* ((daAleaf_c*)i_actor)->CreateHeap() (tail call) */
}
VERIFY(0x020476CC, CheckCreateHeap);

/* 020475B4 */
BOOL daAleaf_c::CreateHeap() {
    WWHD_FUNC(0x020475B4, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arcname, dRes_INDEX_VIGAH_BDL_VIGAH_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(262, modelData != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x106, STR(0x10006D38));

    unk_298 = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (unk_298 == nullptr) {
        return FALSE;
    }

    J3DAnmTransform* pbck = (J3DAnmTransform*)dComIfG_getObjectRes(m_arcname, dRes_INDEX_VIGAH_BCK_VIGAH_e, SAFESTRING_VTBL);
    if (pbck == nullptr) /* JUT_ASSERT(277, pbck != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x115, STR(0x10006D18));

    if (!unk_408.init(modelData, pbck, true, 1 /* J3DFrameCtrl::EMode_RESET */, 1.0f, 0, -1, false)) {
        return FALSE;
    }

    return TRUE;
}
VERIFY(0x020475B4, &daAleaf_c::CreateHeap);

/* 02047878 */
void daAleaf_c::CreateInit() {
    WWHD_FUNC(0x02047878, void, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(unk_298)); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -50.0f, 0.0f, -50.0f, 50.0f, 100.0f, 50.0f);
    unk_29C.Init(255, 255, this);
    unk_2D8.Set(l_cyl_src);
    unk_2D8.SetStts(&unk_29C);
    current.angle.x = home.angle.x;
    current.angle.y = home.angle.y;
    current.angle.z = home.angle.z;
    unk_428 = fpcM_ERROR_PROCESS_ID_e;
    unk_424 = fpcM_ERROR_PROCESS_ID_e;
    set_mtx();
    create_acorn_sub(false);
}
VERIFY(0x02047878, &daAleaf_c::CreateInit);

/* 02047B8C */
void daAleaf_c::create_acorn() {
    WWHD_FUNC(0x02047B8C, void, this);
    gabi::Local<be<u32>> sp8; /* fopAc_ac_c* */

    if ((unk_420 == 0) && !fopAcM_SearchByID_out(unk_424, sp8.get())) {
        create_acorn_sub(true);
        unk_420 = 60;
    }

    if (unk_420 > 0) {
        unk_420 = unk_420 - 1;
    }
}
VERIFY(0x02047B8C, &daAleaf_c::create_acorn);

/* 020477B0 */
fpc_ProcID daAleaf_c::create_acorn_sub(bool arg1) {
    WWHD_FUNC(0x020477B0, fpc_ProcID, this, arg1);
    u32 params = prm_make_acorn(arg1, daAleaf_prm::getItemNo(this), daAleaf_prm::getItemBitNo(this));

    mDoMtx_XrotS_l(mDoMtx_stack_c::get(), home.angle.x);
    gabi::Local<cXyz> sp10;
    PSMTXMultVec(mDoMtx_stack_c::get(), acorn_offset, sp10.get()); /* mDoMtx_stack_c::multVec */

    unk_424 = fopAcM_createChildFromOffset(fpcNm_TSUBO_e, fopAcM_GetID(this), params, sp10.get(), fopAcM_GetRoomNo(this),
                                           nullptr, nullptr, -1, 0);
    unk_41B = false;
    unk_41C = false;

    return unk_424;
}
VERIFY(0x020477B0, &daAleaf_c::create_acorn_sub);

/* 02047948 */
cPhs_State daAleaf_c::_create() {
    WWHD_FUNC(0x02047948, cPhs_State, this);
    /* fopAcM_ct(this, daAleaf_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = DAALEAF_VTBL;
            dCcD_Stts_ct(&unk_29C);
            dCcD_Cyl_ct(&unk_2D8, 0x10006CCC);
            /* mDoExt_bckAnm::mDoExt_bckAnm() (inline, HD) */
            const u32 b = gabi::ea(&unk_408);
            gabi::call(0x027F2BC0, &unk_408, 0); /* J3DFrameCtrl::init */
            gabi::store<u32>(b + 0x10, 0x1016E54C);
            gabi::call(0x027DA984, gabi::at<void>(b + 0x14));
            gabi::store<u32>(b + 0x58, 0);
            gabi::store<u32>(b + 0x48, 0x1016D820);
            gabi::store<u32>(b + 0x84, 0);
            gabi::store<u32>(b + 0x80, 0);
            gabi::store<u32>(b + 0x10, 0x10006CDC);
            gabi::store<u32>(b + 0x88, 0);
            gabi::store<u32>(b + 0x7C, 0);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State ret = dComIfG_resLoad(&unk_290, m_arcname);
    if (ret == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, 0x020476CC /* CheckCreateHeap */, 0x620)) {
            return cPhs_ERROR_e;
        }
        CreateInit();
    }
    return ret;
}
VERIFY(0x02047948, &daAleaf_c::_create);

/* 020476D0 */
void daAleaf_c::set_mtx() {
    WWHD_FUNC(0x020476D0, void, this);
    J3DModel_setBaseScale(unk_298, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    J3DModel_setBaseTRMtx(unk_298, mDoMtx_stack_c::get());
}
VERIFY(0x020476D0, &daAleaf_c::set_mtx);

/* (cXyz)(a - b).absXZ() */
static inline f32 absXZ_diff(cXyz* a, cXyz* b) {
    gabi::Local<cXyz> d;
    cXyz_mi(a, d.get(), b);
    gabi::Local<cXyz> xz;
    f32 x = d->x, z = d->z;
    xz->x = x;
    xz->y = 0.0f;
    xz->z = z;
    return std_sqrtf(PSVECSquareMag(xz.get()));
}

/* 02047C00 */
bool daAleaf_c::_execute() {
    WWHD_FUNC(0x02047C00, bool, this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    fopAc_ac_c* actor = fopAcM_SearchByID(unk_424);

    bool var_r29 = false;

    if (actor != nullptr) {
        var_r29 = daObj_PrmAbstract_acorn(actor, 1, 0x1F) != 0; /* ((daTsubo::Act_c*)actor)->prm_get_stick() */
    }

    create_acorn();

    if (actor != nullptr) {
        if (unk_41C < 69) {
            unk_41C = unk_41C + 1;
        }

        if (unk_41C == 69) {
            unk_418 = true;
            unk_408.mFrameCtrl.mRate = 1.0f; /* setPlaySpeed */
            unk_408.mFrameCtrl.mFrame = 0.0f; /* setFrame */
            unk_41C = 70;
        }
    }

    if (!var_r29 && unk_419) {
        unk_418 = true;
        unk_408.mFrameCtrl.mRate = 1.0f;
    }

    f32 var_f31 = absXZ_diff(&player->current.pos, &current.pos);

    if (var_f31 < 50.0f && (unk_42C - var_f31) > 3.0f) {
        f32 y = current.pos.y;
        if (fabsf_g(player->current.pos.y - y) < 5.0f) {
            unk_418 = true;
            unk_408.mFrameCtrl.mRate = 1.0f;
        }
    }

    bool near = false;
    if (var_f31 < 200.0f) {
        fpc_ProcID grab = daPy_getGrabActorID(player);
        near = unk_428 != grab && daPy_getGrabActorID(player) == fpcM_ERROR_PROCESS_ID_e;
    }
    if (near) {
        fopAc_ac_c* actor2 = fopAcM_SearchByID(unk_428);

        if (actor2 != nullptr) {
            f32 var_f1 = absXZ_diff(&actor2->current.pos, &current.pos);

            if (var_f1 < 70.0f) {
                unk_418 = true;
                unk_408.mFrameCtrl.mRate = 1.0f;
            }
        }
    }

    if (unk_418 && unk_408.play()) {
        unk_418 = false;
    }

    set_mtx();

    unk_419 = var_r29;
    unk_428 = daPy_getGrabActorID(player);
    unk_42C = var_f31;

    return true;
}
VERIFY(0x02047C00, &daAleaf_c::_execute);

/* 02047B04 */
bool daAleaf_c::_draw() {
    WWHD_FUNC(0x02047B04, bool, this);
    dScnKy_env_light_c* env = dKy_getEnvlight();
    settingTevStruct(env, TEV_TYPE_ACTOR, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, unk_298, &tevStr);

    unk_408.entry(J3DModel_getModelData(unk_298), unk_408.getFrame());
    mDoExt_modelUpdateDL(unk_298);
    mDoExt_bckAnm_remove(J3DModel_getModelData(unk_298));

    return true;
}
VERIFY(0x02047B04, &daAleaf_c::_draw);

/* 02047AD0 */
static cPhs_State daAleaf_Create(void* i_this) {
    WWHD_FUNC(0x02047AD0, cPhs_State, i_this);
    return gabi::call<cPhs_State>(0x02047948, i_this); /* ((daAleaf_c*)i_this)->_create() (tail call) */
}
VERIFY(0x02047AD0, daAleaf_Create);

/* 02047AD4: _delete() inlined. HD: dComIfG_resDelete (GameCube: resDeleteDemo) */
static BOOL daAleaf_Delete(void* i_this) {
    WWHD_FUNC(0x02047AD4, BOOL, i_this);
    dComIfG_resDelete(&((daAleaf_c*)i_this)->unk_290, m_arcname);
    return TRUE;
}
VERIFY(0x02047AD4, daAleaf_Delete);

/* 02047B88 */
static BOOL daAleaf_Draw(void* i_this) {
    WWHD_FUNC(0x02047B88, BOOL, i_this);
    return gabi::call<BOOL>(0x02047B04, i_this); /* ((daAleaf_c*)i_this)->_draw() (tail call) */
}
VERIFY(0x02047B88, daAleaf_Draw);

/* 02047EE8 */
static BOOL daAleaf_Execute(void* i_this) {
    WWHD_FUNC(0x02047EE8, BOOL, i_this);
    return gabi::call<BOOL>(0x02047C00, i_this); /* ((daAleaf_c*)i_this)->_execute() (tail call) */
}
VERIFY(0x02047EE8, daAleaf_Execute);

/* 02047FB4 */
static BOOL daAleaf_IsDelete(void*) {
    WWHD_FUNC(0x02047FB4, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02047FB4, daAleaf_IsDelete);

/* 02047EEC __sinit_d_a_acorn_leaf_cpp: header statics, then acorn_offset(0, 15, 0) */
static void acorn_leaf_sinit() {
    WWHD_FUNC(0x02047EEC, void, (u32)0);
    sinit_header_statics(0x10461360, 0x1018F714);
    acorn_offset->x = 0.0f;
    acorn_offset->y = 15.0f;
    acorn_offset->z = 0.0f;
}
VERIFY(0x02047EEC, acorn_leaf_sinit);

/* 02047FA0 sead::SafeString deleting destructor (this TU's vtable 0x10006CB4, slot 0xC) */
static void acorn_leaf_SafeString_dtor(void* p, s32 flags) {
    WWHD_FUNC(0x02047FA0, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02047FA0, acorn_leaf_SafeString_dtor);

/* 02048034 sead::SafeString virtual (empty; vtable 0x10006CB4 slot 0x14) */
static void acorn_leaf_SafeString_v14(void*) {
    WWHD_FUNC(0x02048034, void, (u32)0);
}
VERIFY(0x02048034, acorn_leaf_SafeString_v14);

/* 02047FBC daAleaf_c deleting destructor (vtable 0x10006D04 slot 0xC) */
static void daAleaf_dtor(daAleaf_c* p, s32 flags) {
    WWHD_FUNC(0x02047FBC, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x027F3628, gabi::at<void>(gabi::ea(&p->unk_408) + 0x10), 0); /* ~mDoExt_bckAnm (base at +0x10) */
        dCcD_Cyl_dt(&p->unk_2D8, 2);
        dCcD_Stts_dt(&p->unk_29C, 2);
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x02047FBC, daAleaf_dtor);
