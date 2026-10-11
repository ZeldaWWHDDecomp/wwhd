/**
 * d_a_obj_vfan.cpp (WWHD)
 * Object - Ganon's Tower: barrier fan broken by the Master Sword (dBgS_MoveBgActor).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_vfan.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_vfan.h"

const gc_tu_data gc_tu = {
    .safeString = 0x10032A60,
    .headerStatics = {0x1046C828, 0x101CD8E0},
};
const char Act_c::M_arcname[] = "Vfan";
static Mtx34 M_tmp_mtx;
static s16 m_evid;
// HD: the collision source retains its checked image record (shared source type is opaque).
static dCcD_SrcCyl cyl_check_src;

/* 023B1108 */
// HD: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU)
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
s32 Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }

/* 023B076C */
BOOL Act_c::CreateHeap() {
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_VFAN_BDL_V_FAN_00_e);
    JUT_ASSERT(0x8c, model_data != NULL);
    J3DModel* model = mDoExt_J3DModel__create(model_data, 0, 0x11020203);
    mpModel = model;
    return model != nullptr; /* HD: tests the returned pointer */
}

/* 023B0914 */
BOOL Act_c::Create() {
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    init_mtx();
    fopAcM_setCullSizeBox(this, -4000.0f, -500.0f, -4000.0f, 4000.0f, 500.0f, 4000.0f);
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(&cyl_check_src);
    mCyl.SetC(&current.pos);
    mCyl.SetStts(&mStts);
    mIsAlive = true;
    mBreakTimer = 0;
    mState = 0;
    m_evid = dComIfGp_evmng_getEventIdx("Vfan", 0xFF);
    return TRUE;
}

/* 023B057C */
cPhs_State Act_c::Mthd_Create() {
    fopAcM_ct(this, Act_c);

    cPhs_State phase_state;
    if (fopAcM_isSwitch(this, prm_get_swSave())) {
        return cPhs_STOP_e;
    } else {
        phase_state = dComIfG_resLoad(&mPhs, M_arcname);
        if (phase_state == cPhs_COMPLEATE_e) {
            phase_state = MoveBGCreate(M_arcname, dRes_INDEX_VFAN_DZB_V_FAN_00_e, 0, 0xA60);
            if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT(0xc5, ...) */
                JUTAssertion::showAssert("d_a_obj_vfan.cpp", 0xC5, "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)");
        }
    }
    return phase_state;
}

/* 023B0FB4 */
BOOL Act_c::Delete() {
    return TRUE;
}

/* 023B0714 */
BOOL Act_c::Mthd_Delete() {
    BOOL res = MoveBGDelete();
    if (fpcM_CreateResult(this) != cPhs_STOP_e) {
        dComIfG_resDeleteDemo(&mPhs, M_arcname);
    }
    return res;
}

/* 023B0808 */
void Act_c::set_mtx() {
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    shape_angle.x = current.angle.x;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    MTXCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
}

/* 023B08F4 */
void Act_c::init_mtx() {
    mpModel->setBaseScale(scale);
    set_mtx();
}

/* 023B0A00 */
// HD: particle IDs 0x83CD..0x83D7.
void Act_c::ParticleSet() {
    dComIfGp_particle_set(dPa_name::ID_AK_SN_GANONDOORSMOKE00, &current.pos, &current.angle);
    dComIfGp_particle_set(dPa_name::ID_AK_SN_GANONDOORFLASH00, &current.pos, &current.angle);
    dComIfGp_particle_set(dPa_name::ID_AK_SN_GANONDOORFLASH01, &current.pos, &current.angle);
    dComIfGp_particle_set(dPa_name::ID_AK_SN_GANONDOORFLASH02, &current.pos, &current.angle);
    dComIfGp_particle_set(dPa_name::ID_AK_SN_GANONDOORFLASH03, &current.pos, &current.angle);
    dComIfGp_particle_set(dPa_name::ID_AK_SN_GANONDOORFLASH04, &current.pos, &current.angle);
    dComIfGp_particle_set(dPa_name::ID_AK_SN_GANONDOORFLASH05, &current.pos, &current.angle);
    dComIfGp_particle_set(dPa_name::ID_AK_SN_GANONDOORFLASH06, &current.pos, &current.angle);
    dComIfGp_particle_set(dPa_name::ID_AK_SN_GANONDOORFLASH07, &current.pos, &current.angle);
    dComIfGp_particle_set(dPa_name::ID_AK_SN_GANONDOORFLASH08, &current.pos, &current.angle);
    dComIfGp_particle_set(dPa_name::ID_AK_SN_GANONDOORFLASH09, &current.pos, &current.angle);
}

/* 023B0CB8 */
BOOL Act_c::Execute(Mtx34** mtx) {
    switch ((u32)mState) {
    case 0:
        dComIfG_Ccsp_Set(&mCyl);
        if (mCyl.ChkTgHit()) {
            if (mpBgW != nullptr) {
                if (mpBgW->ChkUsed()) {
                    dBgS* bgs = dComIfG_Bgsp();
                    bgs->Release(mpBgW);
                }
            }
            fopAcM_orderOtherEventId(this, m_evid, 0xFF, 0xFFFF, 0, 1);
            mState = 1;
        }
        break;

    case 1:
        if (eventInfo.checkCommandDemoAccrpt()) {
            mDoAud_seStart(JA_SE_READ_RIDDLE_1);
            mDoAud_seStart(JA_SE_OBJ_GN_SW_DR_LIGHT, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this))); /* fopAcM_seStartCurrent */
            ParticleSet();
            mBreakTimer = 0;
            mState = 2;
        } else {
            fopAcM_orderOtherEventId(this, m_evid, 0xFF, 0xFFFF, 0, 1);
        }
        break;

    case 2:
        if (mBreakTimer == 150) {
            mIsAlive = false;
            mDoAud_seStart(JA_SE_OBJ_GN_SW_DR_BREAK, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        }
        mBreakTimer = mBreakTimer + 1;
        if (dComIfGp_evmng_endCheck(m_evid)) {
            dComIfGs_onSwitch(prm_get_swSave(), home.roomNo); /* fopAcM_onSwitch */
            dComIfGs_onEventBit(dSv_event_flag_c::UNK_3A08);
            dComIfGp_event_reset();
            fopAcM_delete(this);
        }
        break;
    }

    set_mtx();
    *reinterpret_cast<gptr<Mtx34>*>(mtx) = &M_tmp_mtx; // HD: guest pointer representation.
    return TRUE;
}

/* 023B0F10 */
BOOL Act_c::Draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    dComIfGd_setListBG();
    if (mIsAlive) { /* HD: tests != 0 */
        mDoExt_modelUpdateDL(mpModel);
    }
    dComIfGd_setList();
    return TRUE;
}

/* method table entries (HD: tail branches) */
/* 023B0FBC */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Create();
}
/* 023B0FC0 */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Delete();
}
/* 023B0FC4 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->MoveBGExecute();
}
/* 023B0FC8 */
// HD: MoveBGDraw -> virtual Draw
static BOOL Mthd_Draw(void* i_this) {
    return ((Act_c*)i_this)->Draw_v();
}
/* 023B0FD8 */
// HD: MoveBGIsDelete -> virtual IsDelete
static BOOL Mthd_IsDelete(void* i_this) {
    return ((Act_c*)i_this)->IsDelete_v();
}

/* 023B0FE8 */
static void __sinit_d_a_obj_vfan_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}

/* 023B107C */
// HD: sead::SafeString deleting destructor (this TU's copy; trivial)
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 023B1090 */
// HD: dBgS_MoveBgActor::IsDelete (virtual; this TU's copy)
static BOOL MoveBgActor_IsDelete(void*) {
    return TRUE;
}

/* 023B1098 */
// HD: sead::SafeString::assureTermination (this TU's copy; empty)
static void SafeString_assureTermination(void*) {
}

/* 023B109C */
// HD: daObjVfan::Act_c deleting destructor
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py)
str 10032B28 "Vfan"
str 10032B14 "Vfan" in 023B0914
str 10032AF0 in 023B076C
str 10032AE0 "model_data != NULL" renamed
str 10032A88 in 023B057C
str 10032A9C
data M_tmp_mtx 1046C844
data m_evid 1046C874
data cyl_check_src 101CD89C
ext 025E1988 void mDoAud_seStart(u32)
*/
