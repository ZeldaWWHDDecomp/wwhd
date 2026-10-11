/**
 * d_a_floor.cpp (WWHD)
 * Object - Wind Temple - Breakable floor (Iron Boots, Bombs, etc.)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_floor.cpp) to the WWHD layout and verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_floor.h"

const gc_tu_data gc_tu = {
    .safeString = 0x1000F174,
    .headerStatics = {0x10463DD4, 0x101B50B8},
};
const char daFloor_c::m_arcname[6] = "Hhyu1";
namespace daFloor_prm {
inline u32 getSwitchNo(daFloor_c* ac) { return fopAcM_GetParam(ac) & 0xFF; }
}
/* 0213FD18 */
// HD: rideCallBack (unnamed by the matcher)
static void rideCallBack(dBgW* bgw, fopAc_ac_c* ac, fopAc_ac_c* other) {
    daFloor_c* i_this = (daFloor_c*)ac;
    if (other != nullptr && fopAcM_GetName(other) == fpcNm_PLAYER_e && !((daPy_floor_view*)other)->checkEquipHeavyBoots()) {
        i_this->field_0x2d9 = 1;
    } else if (other != nullptr && fopAcM_GetName(other) == fpcNm_PLAYER_e && i_this->field_0x2d9 && ((daPy_floor_view*)other)->checkEquipHeavyBoots() &&
               !i_this->field_0x2d8) {
        i_this->field_0x2d8 = 1;
        i_this->field_0x2da = 6;
    }
}

/* 0213FEB4 */
BOOL daFloor_c::Delete() {
    mSmokeCallBack.remove();
    dComIfG_resDelete(&mPhs, m_arcname);
    return TRUE;
}

/* 021401A0 */
BOOL daFloor_c::CreateHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arcname, dRes_INDEX_HHYU1_BDL_HHYU1_e);
    JUT_ASSERT(0xc1, modelData != NULL);
    J3DModel* m = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    mpModel = m;
    if (m == nullptr)
        return FALSE;
    return TRUE;
}

/* 02140314 */
BOOL daFloor_c::Create() {
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    fopAcM_setCullSizeBox(this, -100.0f, -50.0f, -100.0f, 100.0f, 50.0f, 100.0f);
    set_mtx();
    ((dBgW_floor_view*)mpBgW.get())->SetRideCallback(rideCallBack);
    return TRUE;
}

/* 0213FD90 */
cPhs_State daFloor_c::_create() {
    fopAcM_ct(this, daFloor_c);

    s32 sw = daFloor_prm::getSwitchNo(this);
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

/* 0214023C */
void daFloor_c::set_mtx() {
    mpModel->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 0214000C */
BOOL daFloor_c::Execute(Mtx34** mtx) {
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
    // HD: one cached player pointer supplies both positions.
    cXyz difference;
    difference = pl->current.pos - pl->old.pos;
    if (difference.absXZ() != 0.0f)
        field_0x2d9 = 0;
    return TRUE;
}

/* 0213FF04 */
void daFloor_c::set_effect() {
    dComIfGp_particle_set(dPa_name::ID_AK_SN_KAZEBREAKFLOOR00, &current.pos, &current.angle);
    JPABaseEmitter* emtr = dComIfGp_particle_set(dPa_name::ID_AK_JT_ELEMENTSMOKE01, &current.pos, &current.angle, nullptr, 0xFF,
                                                 (dPa_levelEcallBack*)&mSmokeCallBack, fopAcM_GetRoomNo(this));
    if (emtr != nullptr) {
        // HD: the max-frame store precedes the rate store.
        emtr->setMaxFrame(1);
        emtr->setRate(30.0f);
        ((JPAEmitter_floor_view*)emtr)->setEmitterScale(1.0f, 0.5f, 1.0f);
        ((JPAEmitter_floor_view*)emtr)->setGlobalParticleScale(3.0f, 3.0f, 3.0f);
    }
}

/* 02140108 */
BOOL daFloor_c::Draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}

/* 0213FE98 */
static cPhs_State daFloor_Create(void* i_this) {
    return ((daFloor_c*)i_this)->_create();
}

/* 0213FE9C */
static BOOL daFloor_Delete(void* i_this) {
    return ((daFloor_c*)i_this)->MoveBGDelete();
}

/* 0213FEA0 */
// HD: MoveBGDraw inline: the virtual Draw
static BOOL daFloor_Draw(void* i_this) {
    return ((daFloor_c*)i_this)->Draw_v();
}

/* 0213FEB0 */
static BOOL daFloor_Execute(void* i_this) {
    return ((daFloor_c*)i_this)->MoveBGExecute();
}

/* 02140430 */
static BOOL daFloor_IsDelete(void* i_this) {
    return TRUE;
}

/* 0214039C */
static void __sinit_d_a_floor_cpp() {
    // HD: this unit initializes only the two header statics.
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}

/* 02140438 */
// HD: deleting destructor of a class with a trivial destructor (sead::SafeString, per TU)
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 0214044C */
// HD: dBgS_MoveBgActor::IsDelete (per-TU copy)
static BOOL MoveBgActor_IsDelete(void* p) {
    return TRUE;
}

/* 02140454 */
// HD: daFloor_c deleting destructor
static void daFloor_c_dt(daFloor_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 021404A8 */
// HD: an empty virtual (per-TU copy)
static void empty_virtual(void* p) {
}

/* WWHD glue (tools/verify/autoglue.py)
str 1000F1E0
str 1000F1A0
str 1000F1B0 "modelData != NULL" renamed
*/
