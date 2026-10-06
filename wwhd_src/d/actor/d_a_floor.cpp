/**
 * d_a_floor.cpp (WWHD)
 * Object - Wind Temple - Breakable floor (Iron Boots, Bombs, etc.)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_floor.cpp) to the WWHD layout and verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define m_arcname STR(0x1000F1E0)  /* "Hhyu1" */
#define SAFESTRING_VTBL 0x1000F174 /* this TU's sead::SafeString vtable */
#define FLOOR_VTBL 0x1000F1E8      /* daFloor_c vtable (HD virtual destructor) */

enum {
    dRes_INDEX_HHYU1_BDL_HHYU1_e = 4,
    dRes_INDEX_HHYU1_DZB_HHYU1_e = 7,
};
enum { fpcNm_PLAYER_e = 0xA8 };
enum { JA_SE_OBJ_HEAVY_FLOOR_BRK = 0x69C3 };
enum { dPa_name_ID_AK_SN_KAZEBREAKFLOOR00 = 0x81A6, dPa_name_ID_AK_JT_ELEMENTSMOKE01 = 0x2027 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* daPy_py_c::checkEquipHeavyBoots(): mNoResetFlg0 (+0x3B8 HD) & 0x02000000 */
static inline bool checkEquipHeavyBoots(fopAc_ac_c* pl) { return (gabi::load<u32>(gabi::ea(pl) + 0x3B8) & 0x02000000) != 0; }
/* fopAcM_GetName (inline, HD: NULL-checked): base.mProcName (s16 at +8) */
static inline s16 proc_name(fopAc_ac_c* a) { return gabi::load<s16>(gabi::ea(a) + 8); }
/* dPa_smokeEcallBack constructor (025A5B18, as in d_a_kb.h) and remove() (virtual, vtable slot 0x44) */
static inline void dPa_smokeEcallBack_ct(void* p, u8 a) { gabi::call(0x025A5B18, p, a); }
static inline void dPa_smokeEcallBack_remove(void* p) { gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(p)) + 0x44), p); }
/* (a - b).absXZ() (as in d_a_npc_tc.cpp) */
static inline f32 diffAbsXZ(cXyz* a, cXyz* b) {
    gabi::Local<cXyz> d;
    cXyz_mi(a, d, b);
    gabi::Local<cXyz> xz;
    xz->x = d->x;
    xz->y = 0.0f;
    xz->z = d->z;
    return std_sqrtf(PSVECSquareMag(xz));
}
/* dBgW::SetRideCallback: +0xB0 */
static inline void dBgW_SetRideCallback(dBgW* w, u32 fn) { gabi::store<u32>(gabi::ea(w) + 0xB0, fn); }

struct daFloor_c : dBgS_MoveBgActor {
    BOOL Delete();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State _create();
    void set_mtx();
    void set_effect();
    BOOL Draw();
    BOOL Execute(Mtx34** mtx);

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ be<s32> mSwitchNo;
    /* 0x3F0 */ be<u8> field_0x2d8;
    /* 0x3F1 */ be<u8> field_0x2d9;
    /* 0x3F2 */ be<u8> field_0x2da;
    /* 0x3F3 */ be<u8> field_0x2db;
    /* 0x3F4 */ u8 mSmokeCallBack[0x20]; /* dPa_smokeEcallBack */
};
WWHD_OFFSET(daFloor_c, mPhs, 0x3E0);
WWHD_OFFSET(daFloor_c, mSwitchNo, 0x3EC);
WWHD_OFFSET(daFloor_c, mSmokeCallBack, 0x3F4);

/* 0213FD18: rideCallBack (unnamed by the matcher) */
static void rideCallBack(dBgW* bgw, fopAc_ac_c* ac, fopAc_ac_c* other) {
    WWHD_FUNC(0x0213FD18, void, bgw, ac, other);
    daFloor_c* i_this = (daFloor_c*)ac;
    if (other != nullptr && proc_name(other) == fpcNm_PLAYER_e && !checkEquipHeavyBoots(other)) {
        i_this->field_0x2d9 = 1;
    } else if (other != nullptr && proc_name(other) == fpcNm_PLAYER_e && i_this->field_0x2d9 && checkEquipHeavyBoots(other) &&
               !i_this->field_0x2d8) {
        i_this->field_0x2d8 = 1;
        i_this->field_0x2da = 6;
    }
}
VERIFY(0x0213FD18, rideCallBack);

/* 0213FEB4 */
BOOL daFloor_c::Delete() {
    WWHD_FUNC(0x0213FEB4, BOOL, this);
    dPa_smokeEcallBack_remove(mSmokeCallBack);
    dComIfG_resDelete(&mPhs, m_arcname);
    return TRUE;
}
VERIFY(0x0213FEB4, &daFloor_c::Delete);

/* 021401A0 */
BOOL daFloor_c::CreateHeap() {
    WWHD_FUNC(0x021401A0, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arcname, dRes_INDEX_HHYU1_BDL_HHYU1_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0xc1, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x1000F1A0), 0xc1, STR(0x1000F1B0));
    J3DModel* m = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    mpModel = m;
    if (m == nullptr)
        return FALSE;
    return TRUE;
}
VERIFY(0x021401A0, &daFloor_c::CreateHeap);

/* 02140314 */
BOOL daFloor_c::Create() {
    WWHD_FUNC(0x02140314, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -100.0f, -50.0f, -100.0f, 100.0f, 50.0f, 100.0f);
    set_mtx();
    dBgW_SetRideCallback(mpBgW, 0x0213FD18 /* rideCallBack */);
    return TRUE;
}
VERIFY(0x02140314, &daFloor_c::Create);

/* 0213FD90 */
cPhs_State daFloor_c::_create() {
    WWHD_FUNC(0x0213FD90, cPhs_State, this);
    /* fopAcM_ct(this, daFloor_c): HD vtable, inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = FLOOR_VTBL;
            dPa_smokeEcallBack_ct(mSmokeCallBack, 1);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    s32 sw = fopAcM_GetParam(this) & 0xFF; /* daFloor_prm::getSwitchNo */
    mSwitchNo = sw;
    if (sw != 0xFF && dComIfGs_isSwitch(sw, home.roomNo))
        return cPhs_ERROR_e;

    cPhs_State rt = dComIfG_resLoad(&mPhs, m_arcname);
    if (rt == cPhs_COMPLEATE_e) {
        if (!MoveBGCreate(m_arcname, dRes_INDEX_HHYU1_DZB_HHYU1_e, 0, 0x8A0))
            return cPhs_ERROR_e;
    }
    return rt;
}
VERIFY(0x0213FD90, &daFloor_c::_create);

/* 0214023C */
void daFloor_c::set_mtx() {
    WWHD_FUNC(0x0214023C, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x0214023C, &daFloor_c::set_mtx);

/* 0214000C */
BOOL daFloor_c::Execute(Mtx34** mtx) {
    WWHD_FUNC(0x0214000C, BOOL, this, mtx);
    if (field_0x2da != 0) {
        u8 t = field_0x2da - 1;
        field_0x2da = t;
        if (t == 0) {
            set_effect();
            fopAcM_seStart(this, JA_SE_OBJ_HEAVY_FLOOR_BRK, 0);
            dComIfGs_onSwitch(mSwitchNo, home.roomNo); /* fopAcM_onSwitch */
            fopAcM_delete(this);
        }
    }
    fopAc_ac_c* pl = dComIfGp_getPlayer(0);
    if (diffAbsXZ(&pl->current.pos, &pl->old.pos) != 0.0f)
        field_0x2d9 = 0;
    return TRUE;
}
VERIFY(0x0214000C, &daFloor_c::Execute);

/* 0213FF04 */
void daFloor_c::set_effect() {
    WWHD_FUNC(0x0213FF04, void, this);
    dComIfGp_particle_set(dPa_name_ID_AK_SN_KAZEBREAKFLOOR00, &current.pos, &current.angle);
    JPABaseEmitter* emtr = dComIfGp_particle_set(dPa_name_ID_AK_JT_ELEMENTSMOKE01, &current.pos, &current.angle, nullptr, 0xFF,
                                                 (dPa_levelEcallBack*)mSmokeCallBack, fopAcM_GetRoomNo(this));
    if (emtr != nullptr) {
        u32 e = gabi::ea(emtr);
        gabi::store<s32>(e + 0x5C, 1);       /* setMaxFrame(1) */
        gabi::store<f32>(e + 0x34, 30.0f);   /* setRate(30) */
        gabi::store<f32>(e + 0x10, 1.0f);    /* setEmitterScale(1, 0.5, 1) */
        gabi::store<f32>(e + 0x08, 1.0f);
        gabi::store<f32>(e + 0x0C, 0.5f);
        gabi::store<f32>(e + 0x238, 3.0f);   /* setGlobalParticleScale(3, 3, 3) */
        gabi::store<f32>(e + 0x240, 3.0f);
        gabi::store<f32>(e + 0x23C, 3.0f);
    }
}
VERIFY(0x0213FF04, &daFloor_c::set_effect);

/* 02140108 */
BOOL daFloor_c::Draw() {
    WWHD_FUNC(0x02140108, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x02140108, &daFloor_c::Draw);

/* 0213FE98 */
static cPhs_State daFloor_Create(void* i_this) {
    WWHD_FUNC(0x0213FE98, cPhs_State, i_this);
    return ((daFloor_c*)i_this)->_create();
}
VERIFY(0x0213FE98, daFloor_Create);

/* 0213FE9C */
static BOOL daFloor_Delete(void* i_this) {
    WWHD_FUNC(0x0213FE9C, BOOL, i_this);
    return ((daFloor_c*)i_this)->MoveBGDelete();
}
VERIFY(0x0213FE9C, daFloor_Delete);

/* 0213FEA0: MoveBGDraw inline: the virtual Draw */
static BOOL daFloor_Draw(void* i_this) {
    WWHD_FUNC(0x0213FEA0, BOOL, i_this);
    return ((daFloor_c*)i_this)->Draw_v();
}
VERIFY(0x0213FEA0, daFloor_Draw);

/* 0213FEB0 */
static BOOL daFloor_Execute(void* i_this) {
    WWHD_FUNC(0x0213FEB0, BOOL, i_this);
    return ((daFloor_c*)i_this)->MoveBGExecute();
}
VERIFY(0x0213FEB0, daFloor_Execute);

/* 02140430 */
static BOOL daFloor_IsDelete(void* i_this) {
    WWHD_FUNC(0x02140430, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02140430, daFloor_IsDelete);

/* 0214039C */
static void __sinit_d_a_floor_cpp() {
    WWHD_FUNC(0x0214039C, void, (u32)0);
    sinit_header_statics(0x10463DD4, 0x101B50B8);
}
VERIFY(0x0214039C, __sinit_d_a_floor_cpp);

/* 02140438: deleting destructor of a class with a trivial destructor (sead::SafeString, per TU) */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02140438, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02140438, trivial_dt);

/* 0214044C: dBgS_MoveBgActor::IsDelete (per-TU copy) */
static BOOL MoveBgActor_IsDelete(void* p) {
    WWHD_FUNC(0x0214044C, BOOL, p);
    return TRUE;
}
VERIFY(0x0214044C, MoveBgActor_IsDelete);

/* 02140454: daFloor_c deleting destructor */
static void daFloor_c_dt(daFloor_c* i_this, s32 flags) {
    WWHD_FUNC(0x02140454, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02140454, daFloor_c_dt);

/* 021404A8: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x021404A8, void, p);
}
VERIFY(0x021404A8, empty_virtual);
