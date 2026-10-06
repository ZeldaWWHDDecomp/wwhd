/* Lighthouse/ferris-wheel light: written from WWHD disassembly (GC bodies are stubs).
 */
#include "d/actor/d_a_obj_light.h"
using daObjLight::Act_c;
static u8* ptr(u32 ea) { return gabi::at<u8>(ea); }
static u32 play() { return gabi::call<u32>(0x025200D4); }
static u32 save() { return gabi::load<u32>(0x101F84DC); }
static s32 daynight() { return gabi::call<s32>(0x02556D14); }
static bool eventBit() { return gabi::call<BOOL>(0x025B8B94, ptr(save() + 0x644), 0x1C02); }
static void sound(u32 id, cXyz* position) { gabi::call(0x025E19CC, id, position); }

void Act_c::set_mtx() {
    WWHD_FUNC(0x0236AF68, void, this);
    for (u32 i = 0; i < 3; ++i) {
        J3DModel* model = mModels[i];
        J3DModel_setBaseScale(model, &scale);
        if (gabi::load<u32>(0x1046A4CC) == 0) {
            for (u32 j = 0; j < 9; j++)
                gabi::store<f32>(0x1046A4A8 + j * 4, j == 5 ? 377.74f : j == 8 ? -377.74f : 0.0f);
            gabi::store<u32>(0x1046A4CC, 1);
        }
        if (i == 0) {
            gabi::Local<cXyz> position;
            position->set(current.pos.x, current.pos.y, current.pos.z);
            gabi::call(0x028E8D88, position.get(), ptr(0x1046A4A8), position.get());
            mDoMtx_stack_c::transS(position->x, position->y, position->z);
        } else {
            u32 offset = 0x1046A4A8 + i * 12;
            f32 x = gabi::load<f32>(offset), y = gabi::load<f32>(offset + 4);
            f32 z = gabi::load<f32>(offset + 8);
            if (i == 2) z += 700.0f;
            mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
            s16 angle = (s16)((s32)shape_angle.y + (s32)mAngle + (i == 2 ? 0x8000 : 0));
            gabi::call(0x025F1C28, mDoMtx_stack_c::get(), angle);
            gabi::call(0x025F24E0, x, y, z);
        }
        if (i == 0) {
            s16 angle = (s16)((s32)shape_angle.y + (s32)mAngle);
            gabi::call(0x025F1C28, mDoMtx_stack_c::get(), angle);
        }
        J3DModel_setBaseTRMtx(mModels[i], mDoMtx_stack_c::get());
        if (i == 0) PSMTXCopy(mDoMtx_stack_c::get(), &mBgMatrix);
        gabi::call(0x027F4D5C, (J3DModel*)mModels[i]);
    }
}
VERIFY(0x0236AF68, &Act_c::set_mtx);
bool Act_c::create_heap() {
    WWHD_FUNC(0x0236B33C, bool, this);
    auto* base = (J3DModelData*)dComIfG_getObjectRes(STR(0x1002C0D8), 7, 0x1002C004);
    if (!base) JUT_ASSERT_fail(STR(0x1002C05C), 0x10B, STR(0x1002C080));
    else mModels[0] = mDoExt_J3DModel__create(base, 0, 0x11020203);
    auto* lamp = (J3DModelData*)dComIfG_getObjectRes(STR(0x1002C0D8), 5, 0x1002C004);
    if (!lamp) JUT_ASSERT_fail(STR(0x1002C05C), 0x112, STR(0x1002C048));
    else {
        mModels[1] = mDoExt_J3DModel__create(lamp, 0, 0x11020203);
        mModels[2] = mDoExt_J3DModel__create(lamp, 0, 0x11020203);
    }
    set_mtx();
    auto* bg = (u8*)dComIfG_getObjectRes(STR(0x1002C0D8), 12, 0x1002C004);
    if (!bg) JUT_ASSERT_fail(STR(0x1002C05C), 0x11C, STR(0x1002C070));
    else {
        dBgW* world = gabi::call<dBgW*>(0x024F23F4, (dBgW*)nullptr);
        mBgW = world;
        if (world && gabi::call<BOOL>(0x0200A030, world, bg, 1, &mBgMatrix)) return false;
    }
    return base && mModels[0] != nullptr && lamp && mModels[1] != nullptr && mModels[2] != nullptr && bg && mBgW != nullptr;
}
VERIFY(0x0236B33C, &Act_c::create_heap);
static u8 solidHeapCB(Act_c* self) {
    WWHD_FUNC(0x0236B524, u8, self);
    // The tail thunk preserves the callee byte without boolean normalization.
    return gabi::call<u8>(0x0236B33C, self);
}
VERIFY(0x0236B524, solidHeapCB);
void Act_c::init_collision() {
    WWHD_FUNC(0x0236B528, void, this);
    gabi::call(0x02515F14, &mStatus, 255, 255, this);
    gabi::call(0x02516518, &mCylinder, ptr(0x1002C0E0));
    u32 a = gabi::ea(this);
    gabi::store<u32>(a + 0x47C, a + 0x3FC);
    u32 flags = gabi::load<u32>(a + 0x4CC);
    // Integer copies preserve the source vector's NaN payloads.
    for (u32 i = 0; i < 3; i++) gabi::store<u32>(a + 0x4EC + i * 4, gabi::load<u32>(0x101FFBA8 + i * 4));
    gabi::store<u32>(a + 0x4CC, flags | 4);
}
VERIFY(0x0236B528, &Act_c::init_collision);
void Act_c::exe_fire() {
    WWHD_FUNC(0x0236B5A4, void, this);
    if (!gabi::load<u32>(gabi::ea(this) + 0x56C)) return;
    f32 wave = cM_ssin((u16)mFirePhase);
    mFireRotation = (s16)((s16)mFireRotation + 720);
    f32 x = current.pos.x, y = current.pos.y + 24.0f, z = current.pos.z;
    mFireScale = gabi::fmadds(0.05f, wave, 0.45f);
    mFireAlpha = (u8)((u32)gabi::ftoi(10.0f * wave) + 140);
    mDoMtx_stack_c::transS(x, y, z);
    gabi::call(0x025F1C28, mDoMtx_stack_c::get(), (s16)((s32)shape_angle.y + (s32)mAngle));
    gabi::call(0x025F1BF4, mDoMtx_stack_c::get(), (s16)mFireRotation);
    f32 scale = mFireScale;
    gabi::call(0x025F2518, scale, scale, scale);
    PSMTXCopy(mDoMtx_stack_c::get(), &mFireMatrix);
    mFirePhase = (s16)((s16)mFirePhase + 4500);
}
VERIFY(0x0236B5A4, &Act_c::exe_fire);
bool Act_c::set_fire(s32 trigger) {
    WWHD_FUNC(0x0236B6AC, bool, this, trigger);
    u32 a = gabi::ea(this);
    if (gabi::load<u32>(a + 0x56C)) {
        gabi::call(0x025A5AC8, ptr(a + 0x568));
        return false;
    }
    gabi::Local<cXyz> scale;
    scale->set(1.45f, 1.45f, 1.45f);
    u32 controller = gabi::load<u32>(play() + 0x5AB0);
    gabi::call(0x025A847C, ptr(controller), 0, 0x1EA, &current.pos, (u8*)nullptr, scale.get(), 255, ptr(a + 0x568), -1, 0, 0, 0);
    mFirePhase = 0;
    exe_fire();
    if (trigger == 1) {
        sound(0x6A03, &current.pos);
        u32 p = play();
        gabi::Local<cXyz> direction;
        direction->set(0.0f, 1.0f, 0.0f);
        gabi::call(0x025CB374, ptr(p + 0x599C), 2, -17, direction.get());
    }
    return true;
}
VERIFY(0x0236B6AC, &Act_c::set_fire);
cPhs_State Act_c::_create() {
    WWHD_FUNC(0x0236B7C8, cPhs_State, this);
    u32 a = gabi::ea(this), condition = actor_condition;
    if (!(condition & 8)) {
        if (this) {
            fopAc_ac_c_ct(this);
            __vtbl = 0x1002C02C;
            gabi::call(0x0200BD2C, &mStatus);
            gabi::call(0x02515DA0, ptr(a + 0x418));
            gabi::store<u32>(a + 0x414, 0x1004AE88);
            gabi::store<u32>(a + 0x418, 0x1004AEC0);
            gabi::call(0x02515FB8, &mCylinder);
            gabi::store<u32>(a + 0x54C, 0x100015A8);
            gabi::store<u32>(a + 0x548, 0x1002C01C);
            gabi::call(0x02018590, ptr(a + 0x550));
            gabi::store<u32>(a + 0x474, 0x1004B108);
            gabi::store<u32>(a + 0x564, 0x1004B150);
            gabi::store<u32>(a + 0x54C, 0x1004B160);
            gabi::call(0x025A5894, mFireCallback, 0, 0);
            condition = actor_condition;
        }
        actor_condition = condition | 8;
    }
    cPhs_State phase = dComIfG_resLoad(&mPhase, STR(0x1002C0D8));
    if (phase != cPhs_COMPLEATE_e) return phase;
    if (!gabi::call<BOOL>(0x025D63E8, this, 0x0236B524, 0x3840)) return cPhs_ERROR_e;
    if (daynight() == 1) {
        if (eventBit()) mAngle = gabi::call<s16>(0x02526CA8);
    } else {
        mAngle = 0x4000;
        gabi::store<u16>(0x101D5F3C, 0x4000);
        gabi::store<u32>(0x101D5F34, gabi::load<u32>(0x101FF558));
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mModels[1]));
    fopAcM_setCullSizeBox(this, -300.0f, -300.0f, -10000.0f, 300.0f, 300.0f, 10000.0f);
    cullSizeFar = 10.0f;
    u32 p = play();
    gabi::call(0x024EEA6C, ptr(p + 0x12A0), (dBgW*)mBgW, this);
    gabi::store<u32>(gabi::ea((dBgW*)mBgW) + 0xA8, 0x024EE658);
    init_collision();
    mLit = eventBit() ? 1 : 0;
    if (daynight() == 1 && mLit == 1) set_fire(0);
    p = play();
    mMainEventId = gabi::call<s16>(0x02543F10, ptr(p + 0x52C4), STR(0x1002C0C0), 255);
    mEventTimer = 0;
    mLightTimer = 0;
    return phase;
}
VERIFY(0x0236B7C8, &Act_c::_create);
void Act_c::delete_fire() {
    WWHD_FUNC(0x0236BA40, void, this);
    u32 a = gabi::ea(this);
    if (gabi::load<u32>(a + 0x56C)) {
        u32 vtable = gabi::load<u32>(a + 0x568);
        gabi::call_ptr<void>(gabi::load<u32>(vtable + 0x44), ptr(a + 0x568));
    }
}
VERIFY(0x0236BA40, &Act_c::delete_fire);
bool Act_c::_delete() {
    WWHD_FUNC(0x0236BA5C, bool, this);
    gabi::store<u8>(0x101D5F4A, 0);
    if (heap != nullptr && mBgW != nullptr && dBgW_ChkUsed(mBgW)) {
        u32 p = play();
        gabi::call(0x020087EC, ptr(p + 0x12A0), (dBgW*)mBgW);
    }
    delete_fire();
    dComIfG_resDelete(&mPhase, STR(0x1002C0D8));
    return true;
}
VERIFY(0x0236BA5C, &Act_c::_delete);
void Act_c::exe_event() {
    WWHD_FUNC(0x0236BAE0, void, this);
    u32 a = gabi::ea(this);
    if (mEventState == 1) {
        if (gabi::load<u16>(a + 0xF8) == 2) mEventState = 2;
        else {
            gabi::call(0x025D7A58, this, (s16)mEventId, 255, 65535, 0, 1);
            gabi::store<u16>(a + 0xFA, gabi::load<u16>(a + 0xFA) | 2);
        }
    } else if (mEventState == 2) {
        s16 timer = mEventTimer;
        if (timer != 0) {
            timer = (s16)(timer - 1);
            mEventTimer = timer;
            if (timer == 1) {
                gabi::call(0x025B9E38, ptr(save() + 0x20), 0x5D, (s8)home.roomNo);
                mEventTimer = 0;
                u32 p = play();
                gabi::store<u16>(p + 0x52B8, gabi::load<u16>(p + 0x52B8) | 8);
                mEventId = -1;
                mEventState = 0;
            }
        } else {
            s16 id = mEventId;
            u32 p = play();
            if (gabi::call<BOOL>(0x025440C8, ptr(p + 0x52C4), id)) mEventTimer = 20;
        }
    }
}
VERIFY(0x0236BAE0, &Act_c::exe_event);
bool Act_c::now_event(s16 id) {
    WWHD_FUNC(0x0236BC48, bool, this, id);
    return mEventState != 0 && (s16)mEventId == id;
}
VERIFY(0x0236BC48, &Act_c::now_event);
void Act_c::renew_angle() {
    WWHD_FUNC(0x0236BC70, void, this);
    if (now_event(mMainEventId)) {
        if (mLightTimer == 0) {
            mAngle = (s16)((s16)mAngle + 64);
            sound(0x303B, &current.pos);
        } else mAngle = 0;
    } else if (daynight() == 1) {
        if (eventBit()) {
            s16 angle = (s16)((s16)mAngle + 64);
            mAngle = angle;
            if (!gabi::call<BOOL>(0x0252695C, angle)) mAngle = gabi::call<s16>(0x02526CA8);
        } else mAngle = (s16)((s16)mAngle + 128);
        sound(0x303B, &current.pos);
    } else mAngle = 0x4000;
}
VERIFY(0x0236BC70, &Act_c::renew_angle);
void Act_c::set_collision() {
    WWHD_FUNC(0x0236BD9C, void, this);
    if (daynight() != 1) return;
    if (gabi::call<BOOL>(0x025162A4, &mCylinder)) {
        if (mLit == 0) { mLit = 1; mLightTimer = 1; }
    } else {
        gabi::Local<cXyz> center;
        center->set(current.pos.x, current.pos.y - 75.0f, current.pos.z);
        gabi::call(0x020182E0, ptr(gabi::ea(this) + 0x550), center.get());
        u32 p = play();
        gabi::call(0x0200E240, ptr(p + 0x26A4), &mCylinder);
    }
}
VERIFY(0x0236BD9C, &Act_c::set_collision);
bool Act_c::set_event(s16 id) {
    WWHD_FUNC(0x0236BE5C, bool, this, id);
    if (mEventState != 0) return false;
    mEventId = id;
    mEventState = 1;
    return true;
}
VERIFY(0x0236BE5C, &Act_c::set_event);
void Act_c::control_light() {
    WWHD_FUNC(0x0236BE84, void, this);
    s16 timer = mLightTimer;
    if ((u32)(s32)timer < 1) return;
    if (timer == 1) {
        if (set_event(mMainEventId) == true) mLightTimer = (s16)((s16)mLightTimer + 1);
        return;
    }
    timer = (s16)(timer + 1);
    mLightTimer = timer;
    if (timer == 10) set_fire(1);
    else if (timer == 75 || timer == 77 || timer == 80) {
        gabi::call(0x025B8B68, ptr(save() + 0x644), 0x1C02);
        if (timer == 75) sound(0x6A04, &current.pos);
        if (timer == 80) mLightTimer = 0;
    } else if (timer == 76 || timer == 79) gabi::call(0x025B8B7C, ptr(save() + 0x644), 0x1C02);
}
VERIFY(0x0236BE84, &Act_c::control_light);
bool Act_c::_execute() {
    WWHD_FUNC(0x0236BFE0, bool, this);
    exe_fire(); exe_event(); renew_angle(); set_mtx(); set_collision();
    gabi::call(0x024F43DC, (dBgW*)mBgW);
    control_light();
    return true;
}
VERIFY(0x0236BFE0, &Act_c::_execute);
bool Act_c::_draw() {
    WWHD_FUNC(0x0236C040, bool, this);
    auto* env = dKy_getEnvlight();
    settingTevStruct(env, 1, &current.pos, &tevStr);
    dComIfGd_setListBG();
    env = dKy_getEnvlight();
    J3DModel* model = mModels[0];
    setLightTevColorType(env, model, &tevStr);
    mDoExt_modelUpdateDL(mModels[0]);
    dComIfGd_setList();
    for (u32 i = 1; i < 3; ++i) {
        if (daynight() == 1 && eventBit() && gabi::load<u8>(0x101D5F4A) == 0) {
            env = dKy_getEnvlight();
            model = mModels[i];
            setLightTevColorType(env, model, &tevStr);
            mDoExt_modelUpdateDL(mModels[i]);
        }
    }
    gabi::store<u8>(0x101D5F4A, 0);
    return true;
}
VERIFY(0x0236C040, &Act_c::_draw);
static cPhs_State Mthd_Create(Act_c* self) {
    WWHD_FUNC(0x0236C160, cPhs_State, self); return self->_create();
}
VERIFY(0x0236C160, Mthd_Create);
static bool Mthd_Delete(Act_c* self) {
    WWHD_FUNC(0x0236C164, bool, self); return self->_delete();
}
VERIFY(0x0236C164, Mthd_Delete);
static bool Mthd_Execute(Act_c* self) {
    WWHD_FUNC(0x0236C168, bool, self); return self->_execute();
}
VERIFY(0x0236C168, Mthd_Execute);
static bool Mthd_Draw(Act_c* self) {
    WWHD_FUNC(0x0236C16C, bool, self); return self->_draw();
}
VERIFY(0x0236C16C, Mthd_Draw);
static void initStatics() {
    WWHD_FUNC(0x0236C170, void);
    sinit_header_statics(0x1046A48C, 0x101CA960);
}
VERIFY(0x0236C170, initStatics);
static void trivialDestructor(void* self, s32 flags) {
    WWHD_FUNC(0x0236C204, void, self, flags);
    if (self && (flags & 1)) operator_delete(self);
}
VERIFY(0x0236C204, trivialDestructor);
static void emptyVirtual(Act_c* self) { WWHD_FUNC(0x0236C218, void, self); }
VERIFY(0x0236C218, emptyVirtual);
static void actorDestructor(Act_c* self, s32 flags) {
    WWHD_FUNC(0x0236C21C, void, self, flags);
    if (self) {
        gabi::call(0x02515A70, &self->mCylinder, 2);
        gabi::call(0x02515860, &self->mStatus, 2);
        gabi::call(0x025D50BC, self, 0);
        if (flags & 1) operator_delete(self);
    }
}
VERIFY(0x0236C21C, actorDestructor);
static BOOL IsDelete(Act_c* self) { WWHD_FUNC(0x0236C288, BOOL, self); return TRUE; }
VERIFY(0x0236C288, IsDelete);
