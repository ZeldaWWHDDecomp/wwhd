/**
 * d_a_bdk.cpp (WWHD)
 * Boss - Helmaroc King (battle): the search callbacks, small helpers, anm_init, nodeCallBack,
 * the compiler-generated functions and the HIO. The larger functions are in d_a_bdk_*.cpp.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bdk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bdk.h"

/* HD process names (the HD enumeration differs from GameCube's) */
enum {
    HD_fpcNm_KAMOME = 0xC2,
    HD_fpcNm_KUI = 0xFA,
    HD_fpcNm_EP = 0xB9,
    HD_fpcNm_DK = 0xA7,
    HD_fpcNm_BDKOBJ = 0xEF,
    HD_fpcNm_Obj_Tide = 0x27,
    HD_fpcNm_BK = 0xBD,
    HD_fpcNm_BOKO = 0x1CF,
    HD_fpcNm_1BE = 0x1BE, /* searched by demo_camera (HD) */
};

/* 02064CF4 (not named by the matcher) */
static void* kamome_delete_sub(void* param_1, void*) {
    WWHD_FUNC(0x02064CF4, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == HD_fpcNm_KAMOME) {
        fopAcM_delete((fopAc_ac_c*)param_1);
    }
    return nullptr;
}
VERIFY(0x02064CF4, kamome_delete_sub);

/* 02064D48 (not named by the matcher) */
static void* kui_delete_sub(void* param_1, void*) {
    WWHD_FUNC(0x02064D48, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == HD_fpcNm_KUI) {
        fopAcM_delete((fopAc_ac_c*)param_1);
    }
    return nullptr;
}
VERIFY(0x02064D48, kui_delete_sub);

/* 02064D9C */
static void* ep_delete_sub(void* param_1, void*) {
    WWHD_FUNC(0x02064D9C, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == HD_fpcNm_EP) {
        fopAcM_delete((fopAc_ac_c*)param_1);
    }
    return nullptr;
}
VERIFY(0x02064D9C, ep_delete_sub);

/* 02064DF0 (not named by the matcher) */
static void* dk_delete_sub(void* param_1, void*) {
    WWHD_FUNC(0x02064DF0, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == HD_fpcNm_DK) {
        fopAcM_delete((fopAc_ac_c*)param_1);
    }
    return nullptr;
}
VERIFY(0x02064DF0, dk_delete_sub);

/* 02064E44 (not named by the matcher) */
static void* obj_delete_sub(void* param_1, void*) {
    WWHD_FUNC(0x02064E44, void*, param_1, (u32)0);
    fopAc_ac_c* actor = (fopAc_ac_c*)param_1;
    if (fopAc_IsActor(actor) && actor != nullptr && fpcM_GetName(actor) == HD_fpcNm_BDKOBJ && (fopAcM_GetParam(actor) & 0xF) >= 2) {
        fopAcM_delete(actor);
    }
    return nullptr;
}
VERIFY(0x02064E44, obj_delete_sub);

/* bdkobj_class::mEffs (HD bdo_eff_s, 0x1B8 bytes, from +0x3B8): the fields used here */
struct bdo_eff_s_l {
    /* 0x000 */ be<s8> m000;
    /* 0x001 */ u8 _001[7];
    /* 0x008 */ cXyz m008;
    /* 0x014 */ u8 _014[0x10];
    /* 0x024 */ be<f32> m024x; /* HD: horizontal speed */
    /* 0x028 */ be<f32> m024z;
    /* 0x02C */ u8 _02C[0x12];
    /* 0x03E */ csXyz m036;
    /* 0x044 */ u8 _044[0x1B8 - 0x44];
};
WWHD_SIZE(bdo_eff_s_l, 0x1B8);

/* 02064EA8 (HD only; t_down): the bdkobj's falling fragments (state 2) get a random spin and a
 * horizontal push of 10 away from the boss */
static void* obj_hahen_sub(void* param_1, void* param_2) {
    WWHD_FUNC(0x02064EA8, void*, param_1, param_2);
    fopAc_ac_c* actor = (fopAc_ac_c*)param_1;
    fopAc_ac_c* bdk = (fopAc_ac_c*)param_2;
    if (fopAc_IsActor(actor) && actor != nullptr && fpcM_GetName(actor) == HD_fpcNm_BDKOBJ && (u8)gabi::load<u8>(gabi::ea(actor) + 0xB3) == 0xF) {
        gabi::Local<cXyz> local_1;
        gabi::Local<cXyz> local_2;
        bdo_eff_s_l* eff = gabi::at<bdo_eff_s_l>(gabi::ea(actor) + 0x3B8);
        for (int i = 0; i < 3; i++, eff++) {
            if (eff->m000 == 2) {
                eff->m036.y = (s16)gabi::ftoi(cM_rndF(500.0f) + 500.0f);
                f32 r = cM_rndF(1.0f);
                f32 x = eff->m008.x;
                if (r < 0.5f) {
                    eff->m036.y = -eff->m036.y;
                }
                cMtx_YrotS(calc_mtx(), cM_atan2s(x - bdk->current.pos.x, eff->m008.z - bdk->current.pos.z));
                local_1->x = 0.0f;
                local_1->y = 0.0f;
                local_1->z = 10.0f;
                MtxPosition(local_1, local_2);
                bdk_fcopy(eff->m024x, local_2->x);
                bdk_fcopy(eff->m024z, local_2->z);
            }
        }
    }
    return nullptr;
}
VERIFY(0x02064EA8, obj_hahen_sub);

/* 0206505C (not named by the matcher) */
static void* sea_delete_sub(void* param_1, void*) {
    WWHD_FUNC(0x0206505C, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == HD_fpcNm_Obj_Tide) {
        fopAcM_delete((fopAc_ac_c*)param_1);
    }
    return nullptr;
}
VERIFY(0x0206505C, sea_delete_sub);

/* 020650B0 (not named by the matcher) */
static void* bk_delete_sub(void* param_1, void*) {
    WWHD_FUNC(0x020650B0, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == HD_fpcNm_BK) {
        fopAcM_delete((fopAc_ac_c*)param_1);
    }
    return nullptr;
}
VERIFY(0x020650B0, bk_delete_sub);

/* 02065104 (not named by the matcher) */
static void* boko_delete_sub(void* param_1, void*) {
    WWHD_FUNC(0x02065104, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == HD_fpcNm_BOKO) {
        fopAcM_delete((fopAc_ac_c*)param_1);
    }
    return nullptr;
}
VERIFY(0x02065104, boko_delete_sub);

/* 02065158 */
static void* obj2_delete_sub(void* param_1, void*) {
    WWHD_FUNC(0x02065158, void*, param_1, (u32)0);
    fopAc_ac_c* actor = (fopAc_ac_c*)param_1;
    if (fopAc_IsActor(actor) && actor != nullptr && fpcM_GetName(actor) == HD_fpcNm_BDKOBJ && actor->model == 0) {
        fopAcM_delete(actor);
    }
    return nullptr;
}
VERIFY(0x02065158, obj2_delete_sub);

/* 020651B8 (HD only; demo_camera): finds the actor with process name 0x1BE */
static void* s_1BE_sub(void* param_1, void*) {
    WWHD_FUNC(0x020651B8, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == HD_fpcNm_1BE) {
        return param_1;
    }
    return nullptr;
}
VERIFY(0x020651B8, s_1BE_sub);

/* 02065208 */
static BOOL land_area_check(cXyz* param_1, f32 param_2) {
    WWHD_FUNC(0x02065208, BOOL, param_1, param_2);
    f32 val2 = param_1->z - -3800.0f;
    f32 val1 = param_1->x - 3600.0f;
    if (std_sqrtf(gabi::fmadds(val1, val1, val2 * val2)) < param_2) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x02065208, land_area_check);

/* 020669B8 */
static void* obj_s_sub(void* param_1, void*) {
    WWHD_FUNC(0x020669B8, void*, param_1, (u32)0);
    fopAc_ac_c* actor = (fopAc_ac_c*)param_1;
    if (fopAc_IsActor(actor) && actor != nullptr && fpcM_GetName(actor) == HD_fpcNm_BDKOBJ && (fopAcM_GetParam(actor) & 0xF) != 0xF &&
        actor->health != 0) {
        return param_1;
    }
    return nullptr;
}
VERIFY(0x020669B8, obj_s_sub);

/* 0206D638 (not named by the matcher) */
static void after_fight(bdk_class* i_this) {
    WWHD_FUNC(0x0206D638, void, i_this);
    i_this->m2F8 = 10;
}
VERIFY(0x0206D638, after_fight);

/* 0206A43C (HD: allocates when this == NULL; the vtable is the last member) */
static daBdk_HIO_c* daBdk_HIO_c_ct(daBdk_HIO_c* self) {
    WWHD_FUNC(0x0206A43C, daBdk_HIO_c*, self);
    if (self == nullptr) {
        self = (daBdk_HIO_c*)operator_new(sizeof(daBdk_HIO_c));
        if (self == nullptr) return self;
    }
    self->m014 = 0;
    self->m005 = 0;
    self->m020 = 60.0f;
    self->m006 = 0;
    self->m008 = 1.0f;
    self->m018 = 20.0f;
    self->__vtbl = DABDK_HIO_VTBL;
    self->m028 = 0x2D0;
    self->m010 = 1.5f;
    self->m00C = 4.0f;
    self->m01C = 2.0f;
    self->m024 = 100.0f;
    self->mNo = -1;
    return self;
}
VERIFY(0x0206A43C, daBdk_HIO_c_ct);

/* 0206A4F0: static initialisation (header statics, then l_HIO) */
static void __sinit_d_a_bdk_cpp() {
    WWHD_FUNC(0x0206A4F0, void, (u32)0);
    /* the header statics (this unit places the two initialised objects at +0x10/+0x11) */
    for (int i = 0; i < 4; i++) gabi::store<u32>(0x104618E8 + 4 * i, 0);
    __register_global_object(0x10190A70);
    gabi::store<f32>(0x104618BC, -3.1415927f);
    gabi::store<f32>(0x104618C0, 3.1415927f);
    gabi::call(0x028ED6F8, 0x104618CC);
    __register_global_object(0x10190A7C);
    gabi::call(0x028EAB2C, 0x104618CD);
    __register_global_object(0x10190A88);
    daBdk_HIO_c_ct(&l_HIO);
}
VERIFY(0x0206A4F0, __sinit_d_a_bdk_cpp);

/* 0206A590 (not named by the matcher): fopAcM_seStart on current.pos, out of line (HD: null checks) */
static void fopAcM_seStartCurrent(fopAc_ac_c* actor, u32 sfx, u32 param) {
    WWHD_FUNC(0x0206A590, void, actor, sfx, param);
    if (actor != nullptr && gabi::ea(&actor->current.pos) != 0) {
        mDoAud_seStart(sfx, &actor->current.pos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
    }
}
VERIFY(0x0206A590, fopAcM_seStartCurrent);

/* 0206A5FC: sead::SafeString deleting destructor (this TU's vtable 0x10007E60 +0xC) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0206A5FC, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x0206A5FC, SafeString_dt);

/* 02072884: empty virtual (sead::SafeString::assureTerminationImpl_, vtable 0x10007E60 +0x14) */
static void SafeString_assureTermination(void*) {
    WWHD_FUNC(0x02072884, void, (u32)0);
}
VERIFY(0x02072884, SafeString_assureTermination);

/* 0206BA98 (out of line): JPABaseEmitter status flags (HD +0x254) |= flag */
static void JPABaseEmitter_setStatus(JPABaseEmitter* e, u32 flag) {
    WWHD_FUNC(0x0206BA98, void, e, flag);
    u32 a = gabi::ea(e) + 0x254;
    gabi::store<u32>(a, gabi::load<u32>(a) | flag);
}
VERIFY(0x0206BA98, JPABaseEmitter_setStatus);

/* 020725FC (out of line): JGeometry::TVec3<f32>::set(x, y, z) */
static void TVec3_set(cXyz* v, f32 x, f32 y, f32 z) {
    WWHD_FUNC(0x020725FC, void, v, x, y, z);
    v->y = y;
    v->z = z;
    v->x = x;
}
VERIFY(0x020725FC, TVec3_set);

/* 02069948 / 02069950: dPa_smokeEcallBack array element constructors (__construct_array) */
static u32 smokeEcallBack_ct_a(void* p) {
    WWHD_FUNC(0x02069948, u32, p);
    return gabi::call<u32>(0x025A5B18, p, (u8)1);
}
VERIFY(0x02069948, smokeEcallBack_ct_a);

static u32 smokeEcallBack_ct_b(void* p) {
    WWHD_FUNC(0x02069950, u32, p);
    return gabi::call<u32>(0x025A5B18, p, (u8)1);
}
VERIFY(0x02069950, smokeEcallBack_ct_b);

/* 02069958: dPa_followEcallBack array element constructor (__construct_array) */
static u32 followEcallBack_ct(void* p) {
    WWHD_FUNC(0x02069958, u32, p);
    return gabi::call<u32>(0x025A5894, p, (u8)0, (u8)0);
}
VERIFY(0x02069958, followEcallBack_ct);

/* 0207260C: bdk_eff_s::bdk_eff_s (HD: allocates when this == NULL) */
static bdk_eff_s* bdk_eff_s_ct(bdk_eff_s* self) {
    WWHD_FUNC(0x0207260C, bdk_eff_s*, self);
    if (self == nullptr) {
        self = (bdk_eff_s*)operator_new(sizeof(bdk_eff_s));
        if (self == nullptr) return self;
    }
    gabi::call(0x025166F0, &self->m048); /* dCcD_Sph::dCcD_Sph */
    return self;
}
VERIFY(0x0207260C, bdk_eff_s_ct);

/* 02072654: dPa_followEcallBack array element deleting destructor (__destroy_arr) */
static void followEcallBack_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02072654, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x02072654, followEcallBack_dt);

/* 02072668: dPa_smokeEcallBack array element deleting destructor (__destroy_arr) */
static void smokeEcallBack_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02072668, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x02072668, smokeEcallBack_dt);

/* 0207267C: bdk_eff_s deleting destructor */
static void bdk_eff_s_dt(bdk_eff_s* self, s32 flags) {
    WWHD_FUNC(0x0207267C, void, self, flags);
    if (self == nullptr) return;
    gabi::call(0x02515AE8, &self->m048, 2); /* dCcD_Sph::~dCcD_Sph */
    if (flags & 1) operator_delete(self);
}
VERIFY(0x0207267C, bdk_eff_s_dt);

/* GHS array helpers */
static inline void __construct_array(void* p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void __destroy_arr(void* p, u32 n, u32 size, u32 dtor, u32 f7, u32 f8) { gabi::call(0x028F0164, p, n, size, dtor, f7, f8); }

/* 020726D0: bdk_class deleting destructor (HD virtual destructor) */
static void bdk_class_dt(bdk_class* self, s32 flags) {
    WWHD_FUNC(0x020726D0, void, self, flags);
    if (self == nullptr) return;
    __destroy_arr(self->m61C4, 4, 0x14, 0x02072654, 0, 0);
    __destroy_arr(self->m6130, 4, 0x20, 0x02072668, 0, 0);
    __destroy_arr(self->m6080, 4, 0x20, 0x02072668, 0, 0);
    gabi::call(0x02515860, &self->m603C, 2); /* dCcD_Stts::~dCcD_Stts */
    __destroy_arr(self->m261C, 40, 0x174, 0x0207267C, 0, 0);
    __destroy_arr(self->mWindAtSph, 10, 0x12C, 0x02515AE8, 0, 0);
    __destroy_arr(self->mFootCCSph, 2, 0x12C, 0x02515AE8, 0, 0);
    gabi::call(0x02515AE8, &self->mBodyCCSph, 2);
    gabi::call(0x02515AE8, &self->mTosakaTgSph, 2);
    gabi::call(0x02515AE8, &self->mHeadTgSph, 2);
    gabi::call(0x02515AE8, &self->mHeadAtSph, 2);
    gabi::call(0x02515860, &self->mStts, 2);
    /* ~dBgS_ObjAcch (inline) -> ~dBgS_Acch */
    u32 acch = gabi::ea(&self->mAcch);
    gabi::store<u32>(acch + 0x20, 0x10007EE8);
    gabi::store<u32>(acch + 0x14, 0x10007EF8);
    gabi::call(0x024EFD9C, acch, 0);                              /* dBgS_Acch::~dBgS_Acch */
    gabi::call(0x02018034, gabi::ea(&self->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir */
    __destroy_arr(self->mA50, 4, 0x12C, 0x02515AE8, 0, 0);
    gabi::call(0x02515860, &self->mA14, 2);
    gabi::call(0x025D50BC, self, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) operator_delete(self);
}
VERIFY(0x020726D0, bdk_class_dt);

/* 020692D8 */
static BOOL daBdk_IsDelete(bdk_class*) {
    WWHD_FUNC(0x020692D8, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x020692D8, daBdk_IsDelete);

/* 020692E0 */
static BOOL daBdk_Delete(bdk_class* i_this) {
    WWHD_FUNC(0x020692E0, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, STR(0x100080B4) /* "Bdk" */);
    mDoHIO_deleteChild(l_HIO.mNo);
    fopAc_ac_c* actor = i_this;
    if (actor->heap.get() != nullptr) {
        cBgS_Release(dComIfG_Bgsp(), i_this->pm_bgw);
        cBgS_Release(dComIfG_Bgsp(), i_this->mp63BC[0]);
        cBgS_Release(dComIfG_Bgsp(), i_this->mp63BC[1]);
        cBgS_Release(dComIfG_Bgsp(), i_this->mp63BC[2]);
        gabi::call(0x025E563C, i_this->mpMorf.get()); /* mDoExt_McaMorf::stopZelAnime */
    }
    for (s32 i = 0; i < 4; i++) {
        mDoAud_seDeleteObject(&i_this->m910[i]);
    }
    mDoAud_seDeleteObject(&center_pos);
    mDoAud_seDeleteObject(&wind_se_pos);
    mDoAud_seDeleteObject(&i_this->m2CC);
    for (s32 i = 0; i < 4; i++) {
        bdk_vremove(&i_this->m6080[i]);
        bdk_vremove(&i_this->m6130[i]);
    }
    bdk_vremove(&i_this->m6110);
    bdk_vremove(&i_this->m61B0);
    return TRUE;
}
VERIFY(0x020692E0, daBdk_Delete);

/* dKy_tevstr_c::dKy_tevstr_c (HD, inline): three light blocks from the default at 0x1016E414 */
static inline void bdk_tevstr_ct(dKy_tevstr_c* t) {
    u32 d = gabi::ea(t);
    const u32 s = 0x1016E414;
    static const u32 blocks[3] = {0x00, 0xC0, 0x144};
    for (u32 o : blocks) {
        for (int i = 0; i < 6; i++) gabi::store<f32>(d + o + 4 * i, gabi::load<f32>(s + 4 * i));
        for (int i = 0; i < 4; i++) gabi::store<u8>(d + o + 0x18 + i, gabi::load<u8>(s + 0x18 + i));
        for (int i = 0; i < 4; i++) gabi::store<s16>(d + o + 0x1C + 2 * i, gabi::load<s16>(s + 0x1C + 2 * i));
        for (int i = 0; i < 8; i++) gabi::store<f32>(d + o + 0x24 + 4 * i, gabi::load<f32>(s + 0x24 + 4 * i));
    }
}
static const dBgS_ObjAcch_vt BDK_OBJACCH_VT = {0x10007ED8, 0x10007EF8, 0x10007EE8};

/* 02069964: bdk_class::bdk_class (HD: out of line; allocates when this == NULL) */
static bdk_class* bdk_class_ct(bdk_class* self) {
    WWHD_FUNC(0x02069964, bdk_class*, self);
    if (self == nullptr) {
        self = (bdk_class*)operator_new(sizeof(bdk_class));
        if (self == nullptr) return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = BDK_VTBL;
    dCcD_Stts_ct(&self->mA14);
    __construct_array(self->mA50, 4, 0x12C, 0x025166F0);
    dBgS_AcchCir_ct(&self->mAcchCir);
    dBgS_ObjAcch_ct(&self->mAcch, BDK_OBJACCH_VT);
    dCcD_Stts_ct(&self->mStts);
    gabi::call(0x025166F0, &self->mHeadAtSph); /* dCcD_Sph::dCcD_Sph */
    gabi::call(0x025166F0, &self->mHeadTgSph);
    gabi::call(0x025166F0, &self->mTosakaTgSph);
    gabi::call(0x025166F0, &self->mBodyCCSph);
    __construct_array(self->mFootCCSph, 2, 0x12C, 0x025166F0);
    __construct_array(self->mWindAtSph, 10, 0x12C, 0x025166F0);
    __construct_array(self->m261C, 40, 0x174, 0x0207260C);
    dCcD_Stts_ct(&self->m603C);
    __construct_array(self->m6080, 4, 0x20, 0x02069948);
    gabi::call(0x025A5B18, &self->m6110, (u8)1); /* dPa_smokeEcallBack::dPa_smokeEcallBack */
    __construct_array(self->m6130, 4, 0x20, 0x02069950);
    dPa_followEcallBack_ct(&self->m61B0, 0, 0);
    __construct_array(self->m61C4, 4, 0x14, 0x02069958);
    bdk_tevstr_ct(&self->m6224);
    return self;
}
VERIFY(0x02069964, bdk_class_ct);

#define STR_BDK_HEAP STR(0x100080B8) /* "Bdk" (useHeapInit) */
#define STR_FILE STR(0x100080BC)     /* "d_a_bdk.cpp" */
#define STR_MODELDATA STR(0x100080E0) /* "modelData != (0)" */
#define STR_PM_BGW STR(0x100080C8)   /* "i_this->pm_bgw != (0)" */
static inline void* bdk_res(s32 idx) { return dComIfG_getObjectRes(STR_BDK_HEAP, idx, BDK_SAFESTRING_VTBL); }
/* 027F3F94 (the matcher names it __nw): returns the model data's joint tree; joint count (u16) at +8 */
static inline u16 bdk_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::ea(gabi::call<void*>(0x027F3F94, d)) + 8); }
/* J3DModelData::getJointNodePointer(i)->setCallBack(cb) (HD: nodes of 0x1C bytes from +8; index checked against +4) */
static inline void bdk_setJointCallBack(J3DModelData* d, u32 i, u32 cb) {
    u32 data = gabi::ea(d);
    u32 n = gabi::load<u32>(data + 4);
    u32 p = gabi::load<u32>(data + 8);
    if (i < n) p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
static inline J3DModelData* bdk_modelData(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
static inline void bdk_dBgW_SetCrrFunc(dBgW* w, u32 fn) { gabi::store<u32>(gabi::ea(w) + 0xA8, fn); }
#define dBgS_MoveBGProc_Typical 0x024EE658u

/* 02069428 (HD: no shadow model, no mask visibility animation) */
static BOOL useHeapInit(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x02069428, BOOL, i_actor);
    bdk_class* i_this = (bdk_class*)i_actor;
    J3DModelData* bdl = (J3DModelData*)bdk_res(0x4A /* BDL_DK */);
    J3DAnmTransform* bck = (J3DAnmTransform*)bdk_res(0x2A /* BCK_FLY1 */);
    void* bas = bdk_res(0xB /* BAS_FLY1 */);
    i_this->mpMorf = mDoExt_McaMorf::create(nullptr, bdl, nullptr, nullptr, bck, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1, bas, 0x80000, 0x11000022);
    if (i_this->mpMorf.get() == nullptr || i_this->mpMorf->getModel() == nullptr) {
        return FALSE;
    }

    for (u16 i = 0; i < bdk_getJointNum(bdk_modelData(i_this->mpMorf->getModel())); i++) {
        if (i == 0x17 || (u32)(i - 0x1F) < 6 || i == 0x3A || i == 0x3C || i == 0x3E || i == 0x40) {
            bdk_setJointCallBack(bdk_modelData(i_this->mpMorf->getModel()), i, 0x02065794 /* nodeCallBack */);
        }
    }
    gabi::store<u32>(gabi::ea(i_this->mpMorf->getModel()) + 0xB8, gabi::ea(i_this)); /* setUserArea */

    J3DModelData* modelData = (J3DModelData*)bdk_res(0x4B /* BDL_DK_KAMEN4 */);
    i_this->mp8F0 = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (i_this->mp8F0.get() == nullptr) {
        return FALSE;
    }

    for (s32 i = 0; i < 4; i++) {
        u16 idx = gabi::load<u16>(0x1019070C + 2 * i); /* kamen_break_bdl */
        i_this->m8FC[i] = mDoExt_J3DModel__create((J3DModelData*)bdk_res(idx), 0x80000, 0x11000022);
        if (i_this->m8FC[i].get() == nullptr) {
            return FALSE;
        }
    }

    modelData = (J3DModelData*)bdk_res(0x4C /* BDL_DK_TAIL */);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR_FILE, 0x1B0A, STR_MODELDATA);
    }
    for (int i = 0; i < 4; i++) {
        for (s32 j = 0; j < 9; j++) {
            i_this->m300[i].m000[j] = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
            if (i_this->m300[i].m000[j].get() == nullptr) {
                return FALSE;
            }
        }
    }

    for (s32 i = 0; i < 0x28; i++) {
        if (i == 0x27) {
            modelData = (J3DModelData*)bdk_res(0x4E /* BDL_GROCK00 */);
        } else if (i < 0x27) {
            modelData = (J3DModelData*)bdk_res(0x4D /* BDL_GHANE00 */);
        }
        if (modelData == nullptr) {
            JUT_ASSERT_fail(STR_FILE, 0x1B2E, STR_MODELDATA);
        }
        i_this->m261C[i].m044 = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
        if (i_this->m261C[i].m044.get() == nullptr) {
            return FALSE;
        }
    }

    modelData = (J3DModelData*)bdk_res(0x51 /* BDL_S_TSHUTTER */);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR_FILE, 0x1B3B, STR_MODELDATA);
    }
    for (s32 i = 0; i < 3; i++) {
        i_this->mp6310[i] = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
        if (i_this->mp6310[i].get() == nullptr) {
            return FALSE;
        }
        i_this->mp63BC[i] = new_dBgW();
        if (i_this->mp63BC[i].get() == nullptr) {
            return FALSE;
        }
        cBgD_t* dzb = (cBgD_t*)bdk_res(0x58 /* DZB_S_TSHUTTER */);
        if (cBgW_Set(i_this->mp63BC[i], dzb, cBgW_MOVE_BG_e, &i_this->m632C[i])) {
            return FALSE;
        }
        bdk_dBgW_SetCrrFunc(i_this->mp63BC[i], dBgS_MoveBGProc_Typical);
    }

    modelData = (J3DModelData*)bdk_res(0x52 /* BDL_S_TTOGE */);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR_FILE, 0x1B56, STR_MODELDATA);
    }
    i_this->mp62D8 = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (i_this->mp62D8.get() == nullptr) {
        return FALSE;
    }

    i_this->pm_bgw = new_dBgW();
    if (i_this->pm_bgw.get() == nullptr) {
        JUT_ASSERT_fail(STR_FILE, 0x1B60, STR_PM_BGW);
    }
    cBgD_t* dzb = (cBgD_t*)bdk_res(0x59 /* DZB_S_TTOGE */);
    cBgW_Set(i_this->pm_bgw, dzb, cBgW_MOVE_BG_e, &i_this->m62DC);
    bdk_dBgW_SetCrrFunc(i_this->pm_bgw, dBgS_MoveBGProc_Typical);

    i_this->mp63C8 = gabi::call<u32>(0x02552B60, i_this->mpMorf->getModel(), 0x10190714 /* search_data */, 0xC); /* JntHit_create */
    if (i_this->mp63C8 != 0) {
        i_actor->jntHit = i_this->mp63C8;
    }
    return TRUE;
}
VERIFY(0x02069428, useHeapInit);

#define head_at_sph_src gabi::at<dCcD_SrcSph>(0x10190870)
#define head_tg_sph_src gabi::at<dCcD_SrcSph>(0x101908B0)
#define tosaka_tg_sph_src gabi::at<dCcD_SrcSph>(0x101908F0)
#define body_cc_sph_src gabi::at<dCcD_SrcSph>(0x10190930)
#define foot_cc_sph_src gabi::at<dCcD_SrcSph>(0x10190970)
#define wind_at_sph_src gabi::at<dCcD_SrcSph>(0x101909B0)
#define kamen_sph_src gabi::at<dCcD_SrcSph>(0x101909F0)
#define eff_sph_src gabi::at<dCcD_SrcSph>(0x10190A30)

/* 02069C8C */
static cPhs_State daBdk_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02069C8C, cPhs_State, a_this);
    bdk_class* i_this = (bdk_class*)a_this;
    fopAc_ac_c* actor = a_this;
    /* fopAcM_ct(a_this, bdk_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) bdk_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&i_this->mPhase, STR(0x1000810C) /* "Bdk" */);
    if (phase_state == cPhs_COMPLEATE_e) {
        for (s32 i = 0; i < 4; i++) {
            smoke_setFollowOff(&i_this->m6080[i]);
        }
        for (s32 i = 0; i < 3; i++) {
            smoke_setFollowOff(&i_this->m6130[i]);
        }
        i_this->m2B4 = 1;
        smoke_setFollowOff(&i_this->m6110);

        if (!fopAcM_entrySolidHeap(actor, 0x02069428 /* useHeapInit */, 0x96000)) {
            return cPhs_ERROR_e;
        }
        if (i_this->pm_bgw.get() != nullptr) {
            dBgS* bgs = dComIfG_Bgsp();
            if (dBgS_Regist(bgs, i_this->pm_bgw, actor)) {
                return cPhs_ERROR_e;
            }
        }
        for (s32 i = 0; i < 3; i++) {
            if (i_this->mp63BC[i].get() != nullptr) {
                dBgS* bgs = dComIfG_Bgsp();
                if (dBgS_Regist(bgs, i_this->mp63BC[i], actor)) {
                    return cPhs_ERROR_e;
                }
            }
        }

        gabi::store<u32>(gabi::ea(actor) + 0x39C, 4);    /* attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e */
        gabi::store<u8>(gabi::ea(actor) + 0x38A, 0x2A); /* attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] */
        l_HIO.mNo = mDoHIO_createChild(STR(0x10008110) /* "戦闘用大怪鳥" */, &l_HIO);
        i_this->mAcch.Set(&actor->current.pos, &actor->old.pos, actor, 1, &i_this->mAcchCir, &actor->speed);
        gabi::store<u8>(gabi::ea(&i_this->mAcch) + 0xC, 0); /* OffSameActorChk */
        i_this->mAcchCir.SetWall(400.0f, 500.0f);
        i_this->mStts.Init(0xFF, 0xFF, actor);

        i_this->mHeadAtSph.Set(head_at_sph_src);
        i_this->mHeadTgSph.Set(head_tg_sph_src);
        i_this->mBodyCCSph.Set(body_cc_sph_src);
        i_this->mFootCCSph[0].Set(foot_cc_sph_src);
        i_this->mFootCCSph[1].Set(foot_cc_sph_src);
        i_this->mHeadAtSph.SetStts(&i_this->mStts);
        i_this->mHeadTgSph.SetStts(&i_this->mStts);
        i_this->mBodyCCSph.SetStts(&i_this->mStts);
        i_this->mBodyCCSph.mGObjTg.mSPrm |= 4; /* OnTgNoHitMark */
        i_this->mFootCCSph[0].SetStts(&i_this->mStts);
        i_this->mFootCCSph[1].SetStts(&i_this->mStts);

        i_this->mTosakaTgSph.Set(tosaka_tg_sph_src);
        i_this->mTosakaTgSph.SetStts(&i_this->mStts);

        for (s32 i = 0; i < 10; i++) {
            i_this->mWindAtSph[i].Set(wind_at_sph_src);
            i_this->mWindAtSph[i].SetStts(&i_this->mStts);
        }
        i_this->mA14.Init(0xFF, 0xFF, nullptr);
        for (s32 i = 0; i < 4; i++) {
            i_this->mA50[i].Set(kamen_sph_src);
            i_this->mA50[i].SetStts(&i_this->mA14);
        }
        i_this->m603C.Init(0xFF, 0xFF, nullptr);
        for (s32 i = 0; i < 0x28; i++) {
            i_this->m261C[i].m048.Set(eff_sph_src);
            i_this->m261C[i].m048.SetStts(&i_this->m603C);
        }
        center_pos.x = 3600.0f;
        center_pos.y = 9800.0f;
        center_pos.z = -3800.0f;
        center_pos2.x = 3600.0f;
        center_pos2.y = 9720.0f;
        center_pos2.z = -3800.0f;

        if (dSv_memBit_isDungeonItem(bdk_memBit(), 3) || REG0_S(0) != 0) {
            dSv_memBit_onDungeonItem(bdk_memBit(), 3); /* dComIfGs_onStageBossEnemy */
            i_this->mAction = 0x6E;                    /* ACTION_AFTER_FIGHT */
            actor->current.pos.x = 300000.0f;
            actor->current.pos.y = 300000.0f;
            actor->current.pos.z = 300000.0f;
            i_this->m6320 = -630.0f;
            i_this->m6324 = -70.0f;
            i_this->m62D4 = -500.0f;
        } else if (i_this->m2B4 == 1) {
            i_this->mAction = 0x64; /* ACTION_T_FLY */
            actor->current.pos.copy(center_pos);
            actor->current.pos.y = 5000.0f;
            i_this->m6320 = -630.0f;
            i_this->m6324 = -70.0f;
            mDoAud_bgmStart(0x80000015 /* JA_BGM_MJ_TOWER_BATTLE */);
            for (s32 i = 0; i < 10; i++) {
                i_this->mWindAtSph[i].SetAtType(0x400000 /* AT_TYPE_UNK400000 */);
            }
            i_this->m2EC[0] = 0x168;
        } else {
            actor->gravity = REG0_F(4) + -5.0f;
            actor->speedF = (f32)l_HIO.m020;
            i_this->m2CA = -1;
            i_this->m1120 = -2000;
            actor->health = 20;
            actor->max_health = 20;
            actor->current.pos.y = 11000.0f;
        }
    }
    bdk_tevstr_copy(&i_this->m6224, &actor->tevStr);
    return phase_state;
}
VERIFY(0x02069C8C, daBdk_Create);
