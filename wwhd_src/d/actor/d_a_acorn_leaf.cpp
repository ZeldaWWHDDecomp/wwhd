/**
 * d_a_acorn_leaf.cpp (WWHD)
 * Object - Forest Haven acorn leaf (holds a tsubo "acorn" and drops a new one)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_acorn_leaf.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_acorn_leaf.h"

const gc_tu_data gc_tu = {
    .safeString = 0x10006CB4,
    .headerStatics = {0x10461360, 0x1018F714, 0x1046136C},
};
const char daAleaf_c::m_arcname[] = "VigaH";
static cXyz acorn_offset;
// HD: the unchanged collision source uses the opaque shared dCcD_SrcCyl layout.
static dCcD_SrcCyl l_cyl_src;

// GHS fabsf preserves a NaN payload and flips negative zero.
static inline f32 fabsf_g(f32 x) { return x < 0.0f || (x == 0.0f && std::signbit(x)) ? -x : x; }

/* daObj::PrmAbstract(actor, width, shift) (this TU's out-of-line copy):
 * (mParameters >> shift) & ((1 << width) - 1), with PowerPC shifts (amounts 32..63 give 0) */
static inline u32 ppc_slw(u32 v, u32 n) { return (n & 0x20) ? 0 : v << (n & 0x1F); }
static inline u32 ppc_srw(u32 v, u32 n) { return (n & 0x20) ? 0 : v >> (n & 0x1F); }
/* 02048038 */
static u32 daObj_PrmAbstract_acorn(fopAc_ac_c* a, s32 width, s32 shift) {
    return ppc_srw(fopAcM_GetParam(a), shift) & (ppc_slw(1, width) - 1);
}

namespace daAleaf_prm {
inline u32 getItemBitNo(daAleaf_c* actor) { return (fopAcM_GetParam(actor) >> 6) & 0x7F; }
inline u32 getItemNo(daAleaf_c* actor) { return fopAcM_GetParam(actor) & 0x3F; }
}; // namespace daAleaf_prm

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
static BOOL CheckCreateHeap(fopAc_ac_c* i_actor) { return ((daAleaf_c*)i_actor)->CreateHeap(); }

/* 020475B4 */
BOOL daAleaf_c::CreateHeap() {
    J3DModelData* modelData =
        (J3DModelData*)dComIfG_getObjectRes(m_arcname, dRes_INDEX_VIGAH_BDL_VIGAH_e);
    JUT_ASSERT(262, modelData != NULL); // HD: assertion line

    unk_298 = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (unk_298 == nullptr) {
        return FALSE;
    }

    J3DAnmTransform* pbck =
        (J3DAnmTransform*)dComIfG_getObjectRes(m_arcname, dRes_INDEX_VIGAH_BCK_VIGAH_e);
    JUT_ASSERT(277, pbck != NULL); // HD: assertion line

    if (!unk_408.init(modelData, pbck, true, J3DFrameCtrl::EMode_RESET, 1.0f, 0, -1, false)) {
        return FALSE;
    }

    return TRUE;
}

/* 02047878 */
void daAleaf_c::CreateInit() {
    fopAcM_SetMtx(this, unk_298->getBaseTRMtx());
    fopAcM_setCullSizeBox(this, -50.0f, 0.0f, -50.0f, 50.0f, 100.0f, 50.0f);
    unk_29C.Init(255, 255, this);
    unk_2D8.Set(&l_cyl_src);
    unk_2D8.SetStts(&unk_29C);
    current.angle.x = home.angle.x;
    current.angle.y = home.angle.y;
    current.angle.z = home.angle.z;
    unk_428 = fpcM_ERROR_PROCESS_ID_e;
    unk_424 = fpcM_ERROR_PROCESS_ID_e;
    set_mtx();
    create_acorn_sub(false);
}

/* 02047B8C */
void daAleaf_c::create_acorn() {
    gabi::Local<gptr<fopAc_ac_c>> sp8;

    if ((unk_420 == 0) && !Aleaf_SearchByID(unk_424, sp8.get())) {
        create_acorn_sub(true);
        unk_420 = 60;
    }

    if (unk_420 > 0) {
        unk_420 = unk_420 - 1;
    }
}

/* 020477B0 */
fpc_ProcID daAleaf_c::create_acorn_sub(bool arg1) {
    u32 params =
        prm_make_acorn(arg1, daAleaf_prm::getItemNo(this), daAleaf_prm::getItemBitNo(this));

    Aleaf_XrotS(mDoMtx_stack_c::get(), home.angle.x);
    gabi::Local<cXyz> sp10;
    PSMTXMultVec(mDoMtx_stack_c::get(), &acorn_offset, sp10.get()); /* mDoMtx_stack_c::multVec */

    unk_424 = Aleaf_createChildFromOffset(fpcNm_TSUBO_e, fopAcM_GetID(this), params, sp10.get(),
                                          fopAcM_GetRoomNo(this), nullptr, nullptr, -1, 0);
    unk_41B = false;
    unk_41C = false;

    return unk_424;
}

/* inlined into _create */
daAleaf_c::daAleaf_c() {
    // HD: GHS constructs the base, collision and expanded animation calculator in this order.
    fopAc_ac_c_ct(this);
    __vtbl = 0x10006D04;
    dCcD_Stts_ct(&unk_29C);
    dCcD_Cyl_ct(&unk_2D8, 0x10006CCC);
    AleafFrameCtrl_init(&unk_408.mFrameCtrl, 0);
    unk_408.mMtxCalcVtable = 0x1016E54C;
    AleafMtxCalc_construct(unk_408.mMtxCalcBase);
    unk_408.mBlendFlags = 0;
    unk_408.mBlendVtable = 0x1016D820;
    unk_408.mAnmData = 0;
    unk_408.mAnmIndex = 0;
    unk_408.mMtxCalcVtable = 0x10006CDC;
    unk_408.mAnmTarget = 0;
    unk_408.mAnmFlags = 0;
}

/* 02047948 */
cPhs_State daAleaf_c::_create() {
    fopAcM_ct(this, daAleaf_c);

    cPhs_State ret = dComIfG_resLoad(&unk_290, m_arcname);
    if (ret == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0x620)) {
            return cPhs_ERROR_e;
        }
        CreateInit();
    }
    return ret;
}

/* 020476D0 */
void daAleaf_c::set_mtx() {
    J3DModel_setBaseScale(unk_298, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    J3DModel_setBaseTRMtx(unk_298, mDoMtx_stack_c::get());
}

/* inlined into _execute: (cXyz)(a - b).absXZ(), with both GHS stack temporaries */
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
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    fopAc_ac_c* actor = fopAcM_SearchByID(unk_424);

    bool var_r29 = false;

    if (actor != nullptr) {
        var_r29 = daObj_PrmAbstract_acorn(actor, 1, 0x1F) !=
                  0; /* ((daTsubo::Act_c*)actor)->prm_get_stick() */
    }

    create_acorn();

    if (actor != nullptr) {
        if (unk_41C < 69) {
            unk_41C = unk_41C + 1;
        }

        if (unk_41C == 69) {
            unk_418 = true;
            unk_408.mFrameCtrl.mRate = 1.0f;  /* setPlaySpeed */
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
        fpc_ProcID grab = Aleaf_getGrabActorID(player);
        near = unk_428 != grab && Aleaf_getGrabActorID(player) == fpcM_ERROR_PROCESS_ID_e;
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
    unk_428 = Aleaf_getGrabActorID(player);
    unk_42C = var_f31;

    return true;
}

/* 02047B04 */
bool daAleaf_c::_draw() {
    dScnKy_env_light_c* env = dKy_getEnvlight();
    settingTevStruct(env, TEV_TYPE_ACTOR, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, unk_298, &tevStr);

    unk_408.entry(J3DModel_getModelData(unk_298), unk_408.getFrame());
    mDoExt_modelUpdateDL(unk_298);
    Aleaf_removeAnimation(J3DModel_getModelData(unk_298));

    return true;
}

/* 02047AD0 */
static cPhs_State daAleaf_Create(void* i_this) { return ((daAleaf_c*)i_this)->_create(); }

/* 02047AD4 */
static BOOL daAleaf_Delete(void* i_this) {
    dComIfG_resDelete(&((daAleaf_c*)i_this)->unk_290, daAleaf_c::m_arcname);
    return TRUE;
}

/* 02047B88 */
static BOOL daAleaf_Draw(void* i_this) { return Aleaf_drawResult(i_this); }

/* 02047EE8 */
static BOOL daAleaf_Execute(void* i_this) { return Aleaf_executeResult(i_this); }

/* 02047FB4 */
static BOOL daAleaf_IsDelete(void*) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.
/* 02047EEC */
static void acorn_leaf_sinit() {
    __sinit_header_statics();
    acorn_offset.x = 0.0f;
    acorn_offset.y = 15.0f;
    acorn_offset.z = 0.0f;
}

/* 02047FA0 */
static void acorn_leaf_SafeString_dtor(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02048034 */
static void acorn_leaf_SafeString_v14(void*) {}

/* 02047FBC */
static void daAleaf_dtor(daAleaf_c* p, s32 flags) {
    if (p != nullptr) {
        AleafMtxCalc_destroy(&p->unk_408.mMtxCalcVtable, 0); // HD: calculator base destructor
        __dt(&p->unk_2D8);
        __dt(&p->unk_29C);
        __dt(static_cast<fopAc_ac_c*>(p));
        if (flags & 1)
            operator_delete(p);
    }
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py): per-TU data and private declarations
str 10006D80
str 10006D24
str 10006D38 "modelData != NULL"
str 10006D18 "pbck != NULL"
data acorn_offset 1046137C
data l_cyl_src 1018F6D0
ext 025F18EC void Aleaf_XrotS(Mtx34*, s16)
ext 025D5B20 fpc_ProcID Aleaf_createChildFromOffset(s16, fpc_ProcID, u32, cXyz*, s32, csXyz*, cXyz*, s32, u32)
ext 025D54C4 BOOL Aleaf_SearchByID(fpc_ProcID, gptr<fopAc_ac_c>*)
ext 027F2BC0 void AleafFrameCtrl_init(J3DFrameCtrl*, s32)
ext 027DA984 void AleafMtxCalc_construct(void*)
ext 027F3628 void AleafMtxCalc_destroy(be<u32>*, s32)
ext 025E8508 BOOL AleafBckAnm::init(J3DModelData*, J3DAnmTransform*, bool, s32, f32, s16, s16, bool)
ext 025E86B8 void AleafBckAnm::entry(J3DModelData*, f32)
ext 02047B04 BOOL Aleaf_drawResult(void*)
ext 02047C00 BOOL Aleaf_executeResult(void*)
*/
// clang-format on
