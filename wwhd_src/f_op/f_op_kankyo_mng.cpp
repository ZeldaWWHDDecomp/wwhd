/** Environment Process Manager. WWHD. */
#include "f_op/f_op_kankyo_mng.h"

static const f32 l_unitScale;
static KankyoMngHeaderStatics l_headerStatics;
static const KankyoMngPiPair l_piPair;

/* 025DAC50 */
kankyo_class* fopKyM_SearchByID(u32 id) {
    return fpcEx_SearchByID(id);
}

/* 025DAC54 */
BOOL fopKyM_IsKankyo(kankyo_class* i_process) {
    // HD: adjacent tail wrapper retained with the complete unit interval.
    return fopKy_IsKankyo(i_process);
}

/* 025DAC58 */
static fopKyM_prm_class* fopKyM_CreateAppend() {
    fopKyM_prm_class* params = static_cast<fopKyM_prm_class*>(cMl::memalignB(-4, sizeof(fopKyM_prm_class)));
    if (params) {
        cLib_memSet(params, 0, sizeof(fopKyM_prm_class));
        f32 one = l_unitScale;
        // HD: preserve the emitted x, z, y store order.
        params->mScale.x = one;
        params->mScale.z = one;
        params->mScale.y = one;
    }
    return params;
}

/* 025DACB8 */
static fopKyM_prm_class* createAppend(s32 i_param, cXyz* i_pos, cXyz* i_scale) {
    fopKyM_prm_class* params = fopKyM_CreateAppend();
    if (params) {
        if (i_pos) {
            params->mPos = *i_pos;
        }
        if (i_scale) {
            params->mScale = *i_scale;
        }
        params->mParam = i_param;
    }
    return params;
}

/* 025DAD48 */
u32 fopKyM_Delete(void* i_process) {
    // HD: retain the tail call's return register.
    return fpcM_Delete(i_process);
}

/* 025DAD4C */
u32 fopKyM_Create(s32 i_procName, fopKyM_CreateFunc i_createFunc, void* i_process) {
    // HD: full register-width process name and explicit current-layer lookup.
    layer_class* layer = fpcLy_CurrentLayer();
    return fpcSCtRq_Request(layer, i_procName, i_createFunc, 0, i_process);
}

/* 025DADA4 */
u32 fopKyM_create(s32 i_procName, s32 i_param, cXyz* i_pos, cXyz* i_scale, fopKyM_CreateFunc i_createFunc) {
    fopKyM_prm_class* params = createAppend(i_param, i_pos, i_scale);
    if (!params) {
        return -1;
    }
    return fopKyM_Create(i_procName, i_createFunc, params);
}

/* 025DAE00 */
base_process_class* fopKyM_fastCreate(s32 i_procName, s32 i_param, cXyz* i_pos, cXyz* i_scale, fopKyM_CreateFunc i_createFunc) {
    fopKyM_prm_class* params = createAppend(i_param, i_pos, i_scale);
    if (!params) {
        return nullptr;
    }
    return fpcM_FastCreate(i_procName, i_createFunc, 0, params);
}

// HD: distinct adapter name retains the returned ID; the legacy binding has a void signature.
/* 025DAE64 */
u32 fopKyM_createWpillarHD(const cXyz* i_pos, f32 scaleXZ, f32 scaleY, s32 i_param) {
    fopKyM_prm_class* params = fopKyM_CreateAppend();
    if (!params) {
        return -1;
    }
    params->mPos = *i_pos;
    // HD: retain x, z, parameter, y store order after the position copy.
    params->mScale.x = scaleXZ;
    params->mScale.z = scaleXZ;
    params->mParam = i_param;
    params->mScale.y = scaleY;
    return fopKyM_Create(fpcNm_WPILLAR_e, 0, params);
}

/* 025DAF3C */
s32 fopKyM_createMpillar(const cXyz* i_pos, f32 i_size) {
    cXyz scale;
    scale.x = i_size;
    scale.y = i_size;
    scale.z = i_size;
    dComIfGp_particle_set(dPa_name::ID_IT_SN_YOGAN_HANE00, i_pos, nullptr, &scale);
    return -1;
}

/* 025DAFBC */
static void __sinit_f_op_kankyo_mng_cpp() {
    // HD: common-header initialization, original source name attributed by adjacency.
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101F3420);
    f32 lo = l_piPair.negativePi;
    f32 hi = l_piPair.positivePi;
    l_headerStatics.negativePi = lo;
    l_headerStatics.positivePi = hi;
    KankyoMngFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F342C);
    KankyoMngSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F3438);
}

/* WWHD glue: data and other-unit calls
 data l_unitScale 100579B8
 data l_headerStatics 1048A4A8
 data l_piPair 100579C4
 ext 025DE50C kankyo_class* fpcEx_SearchByID(u32)
 ext 025DA958 BOOL fopKy_IsKankyo(kankyo_class*)
 ext 0200ECCC void cLib_memSet(void*, s32, u32)
 ext 025DF944 u32 fpcM_Delete(void*)
 ext 025E14A8 u32 fpcSCtRq_Request(layer_class*, s32, fopKyM_CreateFunc, u32, void*)
 ext 025DFAB8 base_process_class* fpcM_FastCreate(s32, fopKyM_CreateFunc, u32, fopKyM_prm_class*)
 ext 028ED6F8 void KankyoMngFirstHeaderObject_ct(u8*)
 ext 028EAB2C void KankyoMngSecondHeaderObject_ct(u8*)
*/
