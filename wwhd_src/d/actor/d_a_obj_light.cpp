/* Lighthouse/ferris-wheel light: GameCube names with verified WWHD bodies.
 * HD: the GameCube body stubs leave the fire, event and model behavior specified by WWHD. */
#include "d/actor/d_a_obj_light.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor_mng.h"
using namespace daObjLight;
const gc_tu_data gc_tu = {.safeString = 0x1002C004, .headerStatics = {0x1046A48C, 0x101CA960, 0x1046A498}};
static LightOffsets l_offsets;
static LightVtable l_actorVtable;
static LightVtable l_aabVtable;
static LightStaticHalfword M_S_light_angle;
static LightStaticWord M_S_pre_set_frame_FRRS;
static LightStaticWord mCounter0;
static LightStaticByte M_S_lod_access;
static dCcD_SrcCyl M_cyl_src;
// HD: register the external callback address until autoglue supplies external HostFn bindings.
static gabi::HostFn l_crrBinding(reinterpret_cast<const void*>(dBgS_MoveBGProc_TypicalRotY), 0x024EE658);
/* 0236B524 */
static u8 solidHeapCB(Act_c* self) {
    // HD: the tail thunk preserves the callee byte without boolean normalization.
    return ObjLight_createHeapByte(self);
}

/* 0236B33C */
bool Act_c::create_heap() {
    auto* base = (J3DModelData*)dComIfG_getObjectRes("Skanran", Skanran_BDL_LIGHTHOUSE_e);
    if (!base)
        JUT_ASSERT_fail("d_a_obj_light.cpp", 0x10B, "mdl_data_lighthouse != 0");
    else
        mModels[0] = mDoExt_J3DModel__create(base, 0, 0x11020203);
    auto* lamp = (J3DModelData*)dComIfG_getObjectRes("Skanran", Skanran_BDL_LIGHT_e);
    if (!lamp)
        JUT_ASSERT_fail("d_a_obj_light.cpp", 0x112, "mdl_data_light != 0");
    else {
        mModels[1] = mDoExt_J3DModel__create(lamp, 0, 0x11020203);
        mModels[2] = mDoExt_J3DModel__create(lamp, 0, 0x11020203);
    }
    set_mtx();
    auto* bg = (cBgD_t*)dComIfG_getObjectRes("Skanran", Skanran_DZB_LIGHTHOUSE_e);
    if (!bg)
        JUT_ASSERT_fail("d_a_obj_light.cpp", 0x11C, "bgw_data != 0");
    else {
        auto* world = static_cast<LightBgW*>(new_dBgW());
        mBgW = world;
        if (world && ObjLight_BgW_Set(world, bg, dBgW::MOVE_BG_e, &mBgMatrix))
            return false;
    }
    return base && mModels[0] != nullptr && lamp && mModels[1] != nullptr && mModels[2] != nullptr && bg &&
           mBgW != nullptr;
}

/* 0236B528 */
void Act_c::init_collision() {
    mStatus.Init(255, 255, this);
    mCylinder.Set(&M_cyl_src);
    mCylinder.SetStts(&mStatus);
    u32 flags = mCylinder.mGObjTg.mSPrm;
    // Integer copies preserve the source vector's NaN payloads.
    mCylinder.mGObjTg.mVec = cXyz::Zero;
    mCylinder.mGObjTg.mSPrm = flags | 4;
}

/* 0236BD9C */
void Act_c::set_collision() {
    if (dKy_daynight_check() != 1)
        return;
    if (mCylinder.ChkTgHit()) {
        if (mLit == 0) {
            mLit = 1;
            mLightTimer = 1;
        }
    } else {
        gabi::Local<cXyz> center;
        center->set(current.pos.x, current.pos.y - 75.0f, current.pos.z);
        mCylinder.SetC(center.get());
        dComIfG_Ccsp_Set(&mCylinder);
    }
}

/* 0236B7C8 */
cPhs_State Act_c::_create() {
    u32 condition = actor_condition;
    if (!(condition & 8)) {
        if (this) {
            fopAc_ac_c_ct(this);
            __vtbl = gabi::ea(&l_actorVtable);
            dCcD_Stts_ct(&mStatus);
            dCcD_Cyl_ct(&mCylinder, gabi::ea(&l_aabVtable));
            dPa_followEcallBack_ct(&mFireCallback, 0, 0);
            condition = actor_condition;
        }
        actor_condition = condition | 8;
    }
    cPhs_State phase = dComIfG_resLoad(&mPhase, "Skanran");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn(reinterpret_cast<const void*>(solidHeapCB)), 0x3840))
        return cPhs_ERROR_e;
    if (dKy_daynight_check() == 1) {
        if (dComIfGs_isEventBit(LightEvent_Lit))
            mAngle = ObjLight_getLightAngle();
    } else {
        mAngle = 0x4000;
        M_S_light_angle.mValue = 0x4000;
        M_S_pre_set_frame_FRRS.mValue = mCounter0.mValue;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mModels[1]));
    fopAcM_setCullSizeBox(this, -300.0f, -300.0f, -10000.0f, 300.0f, 300.0f, 10000.0f);
    cullSizeFar = 10.0f;
    dComIfG_Bgsp()->Regist(mBgW.get(), this);
    mBgW->mCrrFunc.v = gabi::guest_fn(reinterpret_cast<const void*>(dBgS_MoveBGProc_TypicalRotY));
    init_collision();
    mLit = dComIfGs_isEventBit(LightEvent_Lit) ? 1 : 0;
    if (dKy_daynight_check() == 1 && mLit == 1)
        set_fire(0);
    mMainEventId = dComIfGp_evmng_getEventIdx("light_start", 255);
    mEventTimer = 0;
    mLightTimer = 0;
    return phase;
}

/* 0236BA5C */
bool Act_c::_delete() {
    M_S_lod_access.mValue = 0;
    if (heap != nullptr && mBgW != nullptr && dBgW_ChkUsed(mBgW)) {
        dComIfG_Bgsp()->Release(mBgW.get());
    }
    delete_fire();
    dComIfG_resDelete(&mPhase, "Skanran");
    return true;
}

/* 0236B6AC */
bool Act_c::set_fire(s32 trigger) {
    if (mFireCallback.mpEmitter.get()) {
        mFireCallback.end();
        return false;
    }
    gabi::Local<cXyz> scale;
    scale->set(1.45f, 1.45f, 1.45f);
    dComIfGp_particle_set(0x1EA, &current.pos, nullptr, scale.get(), 255,
                          reinterpret_cast<dPa_levelEcallBack*>(&mFireCallback), -1, nullptr, nullptr, nullptr);
    mFirePhase = 0;
    exe_fire();
    if (trigger == 1) {
        ObjLight_sound(LightSound_Ignite, &current.pos);
        auto* vibration = dComIfGp_getVibration();
        gabi::Local<cXyz> direction;
        direction->set(0.0f, 1.0f, 0.0f);
        vibration->StartShock(2, -17, direction.get());
    }
    return true;
}

/* 0236B5A4 */
void Act_c::exe_fire() {
    if (!mFireCallback.mpEmitter.get())
        return;
    f32 wave = cM_ssin((u16)mFirePhase);
    mFireRotation = (s16)((s16)mFireRotation + 720);
    f32 x = current.pos.x, y = current.pos.y + 24.0f, z = current.pos.z;
    mFireScale = gabi::fmadds(0.05f, wave, 0.45f);
    mFireAlpha = (u8)((u32)gabi::ftoi(10.0f * wave) + 140);
    mDoMtx_stack_c::transS(x, y, z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)((s32)shape_angle.y + (s32)mAngle));
    mDoMtx_XrotM(mDoMtx_stack_c::get(), (s16)mFireRotation);
    f32 scale = mFireScale;
    mDoMtx_stack_c::scaleM(scale, scale, scale);
    PSMTXCopy(mDoMtx_stack_c::get(), &mFireMatrix);
    mFirePhase = (s16)((s16)mFirePhase + 4500);
}

/* 0236BA40 */
void Act_c::delete_fire() {
    if (mFireCallback.mpEmitter.get()) {
        mFireCallback.remove();
    }
}

/* 0236BC48 */
bool Act_c::now_event(s16 id) {
    return mEventState != 0 && (s16)mEventId == id;
}

/* 0236BE5C */
bool Act_c::set_event(s16 id) {
    if (mEventState != 0)
        return false;
    mEventId = id;
    mEventState = 1;
    return true;
}

/* 0236BAE0 */
void Act_c::exe_event() {
    if (mEventState == 1) {
        if (eventInfo.checkCommandDemoAccrpt())
            mEventState = 2;
        else {
            fopAcM_orderOtherEventId(this, mEventId, 255, 65535, 0, 1);
            eventInfo.onCondition(2);
        }
    } else if (mEventState == 2) {
        s16 timer = mEventTimer;
        if (timer != 0) {
            timer = (s16)(timer - 1);
            mEventTimer = timer;
            if (timer == 1) {
                dComIfGs_onSwitch(LightEvent_Switch, (s8)home.roomNo);
                mEventTimer = 0;
                dComIfGp_getEvent()->reset();
                mEventId = -1;
                mEventState = 0;
            }
        } else {
            s16 id = mEventId;
            if (dComIfGp_evmng_endCheck(id))
                mEventTimer = 20;
        }
    }
}

/* 0236AF68 */
void Act_c::set_mtx() {
    for (u32 i = 0; i < 3; ++i) {
        J3DModel* model = mModels[i];
        J3DModel_setBaseScale(model, &scale);
        if (l_offsets.mInitialized == 0) {
            // HD: lazily initialize the shared model offsets.
            for (u32 j = 0; j < 3; ++j)
                l_offsets.mPosition[j].set(0.0f, 0.0f, j == 1 ? 377.74f : j == 2 ? -377.74f : 0.0f);
            l_offsets.mInitialized = 1;
        }
        if (i == 0) {
            gabi::Local<cXyz> position;
            position->set(current.pos.x, current.pos.y, current.pos.z);
            PSVECAdd(position.get(), &l_offsets.mPosition[0], position.get());
            mDoMtx_stack_c::transS(position->x, position->y, position->z);
        } else {
            f32 x = l_offsets.mPosition[i].x, y = l_offsets.mPosition[i].y, z = l_offsets.mPosition[i].z;
            if (i == 2)
                z += 700.0f;
            mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
            s16 angle = (s16)((s32)shape_angle.y + (s32)mAngle + (i == 2 ? 0x8000 : 0));
            mDoMtx_YrotM(mDoMtx_stack_c::get(), angle);
            mDoMtx_stack_c::transM(x, y, z);
        }
        if (i == 0) {
            s16 angle = (s16)((s32)shape_angle.y + (s32)mAngle);
            mDoMtx_YrotM(mDoMtx_stack_c::get(), angle);
        }
        J3DModel_setBaseTRMtx(mModels[i], mDoMtx_stack_c::get());
        if (i == 0)
            PSMTXCopy(mDoMtx_stack_c::get(), &mBgMatrix);
        mModels[i]->calc();
    }
}

/* 0236BC70 */
void Act_c::renew_angle() {
    if (now_event(mMainEventId)) {
        if (mLightTimer == 0) {
            mAngle = (s16)((s16)mAngle + 64);
            ObjLight_sound(LightSound_Rotate, &current.pos);
        } else
            mAngle = 0;
    } else if (dKy_daynight_check() == 1) {
        if (dComIfGs_isEventBit(LightEvent_Lit)) {
            s16 angle = (s16)((s16)mAngle + 64);
            mAngle = angle;
            if (!ObjLight_setLightAngle(angle))
                mAngle = ObjLight_getLightAngle();
        } else
            mAngle = (s16)((s16)mAngle + 128);
        ObjLight_sound(LightSound_Rotate, &current.pos);
    } else
        mAngle = 0x4000;
}

/* 0236BE84 */
void Act_c::control_light() {
    s16 timer = mLightTimer;
    if ((u32)(s32)timer < 1)
        return;
    if (timer == 1) {
        if (set_event(mMainEventId) == true)
            mLightTimer = (s16)((s16)mLightTimer + 1);
        return;
    }
    timer = (s16)(timer + 1);
    mLightTimer = timer;
    if (timer == 10)
        set_fire(1);
    else if (timer == 75 || timer == 77 || timer == 80) {
        dComIfGs_onEventBit(LightEvent_Lit);
        if (timer == 75)
            ObjLight_sound(LightSound_Complete, &current.pos);
        if (timer == 80)
            mLightTimer = 0;
    } else if (timer == 76 || timer == 79)
        dComIfGs_offEventBit(LightEvent_Lit);
}

/* 0236BFE0 */
bool Act_c::_execute() {
    exe_fire();
    exe_event();
    renew_angle();
    set_mtx();
    set_collision();
    mBgW->Move();
    control_light();
    return true;
}

/* 0236C040 */
bool Act_c::_draw() {
    auto* env = dKy_getEnvlight();
    settingTevStruct(env, 1, &current.pos, &tevStr);
    dComIfGd_setListBG();
    env = dKy_getEnvlight();
    J3DModel* model = mModels[0];
    setLightTevColorType(env, model, &tevStr);
    mDoExt_modelUpdateDL(mModels[0]);
    dComIfGd_setList();
    for (u32 i = 1; i < 3; ++i) {
        if (dKy_daynight_check() == 1 && dComIfGs_isEventBit(LightEvent_Lit) && M_S_lod_access.mValue == 0) {
            env = dKy_getEnvlight();
            model = mModels[i];
            setLightTevColorType(env, model, &tevStr);
            mDoExt_modelUpdateDL(mModels[i]);
        }
    }
    M_S_lod_access.mValue = 0;
    return true;
}

/* 0236C160 */
static cPhs_State Mthd_Create(Act_c* self) {
    return self->_create();
}

/* 0236C164 */
static bool Mthd_Delete(Act_c* self) {
    return self->_delete();
}

/* 0236C168 */
static bool Mthd_Execute(Act_c* self) {
    return self->_execute();
}

/* 0236C16C */
static bool Mthd_Draw(Act_c* self) {
    return self->_draw();
}

/* 0236C288 */
static BOOL Mthd_IsDelete(Act_c* self) {
    return TRUE;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 0236C170 */
static void __sinit_d_a_obj_light_cpp() {
    __sinit_header_statics();
}

/* 0236C204 */
static void SafeString_dtor(void* self, s32 flags) {
    if (self && (flags & 1))
        operator_delete(self);
}

/* 0236C218 */
static void SafeString_assureTerminationImpl(SafeString* self) {}

/* 0236C21C */
static void Act_c_dtor(Act_c* self, s32 flags) {
    if (self) {
        dCcD_Cyl_dt(&self->mCylinder, 2);
        dCcD_Stts_dt(&self->mStatus, 2);
        __dt(static_cast<fopAc_ac_c*>(self));
        if (flags & 1)
            operator_delete(self);
    }
}

/* WWHD glue
data l_offsets 1046A4A8
data l_actorVtable 1002C02C
data l_aabVtable 1002C01C
data M_S_light_angle 101D5F3C
data M_S_pre_set_frame_FRRS 101D5F34
data mCounter0 101FF558
data M_S_lod_access 101D5F4A
data M_cyl_src 1002C0E0
str 1002C0D8 "Skanran"
str 1002C05C "d_a_obj_light.cpp"
str 1002C080 "mdl_data_lighthouse != 0"
str 1002C048 "mdl_data_light != 0"
str 1002C070 "bgw_data != 0"
str 1002C0C0 "light_start"
ext 0236B33C u8 ObjLight_createHeapByte(daObjLight::Act_c*)
ext 0200A030 BOOL ObjLight_BgW_Set(dBgW*,cBgD_t*,u32,Mtx34*)
ext 025E19CC void ObjLight_sound(u32,cXyz*)
ext 0252695C BOOL ObjLight_setLightAngle(s16)
ext 02526CA8 s16 ObjLight_getLightAngle()
ext 024EE658 void dBgS_MoveBGProc_TypicalRotY(dBgW*,void*,cBgS_PolyInfo*,bool,cXyz*,csXyz*,csXyz*)
*/
