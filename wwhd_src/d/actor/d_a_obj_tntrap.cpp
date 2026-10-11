/**
 * d_a_obj_tntrap.cpp
 * Object - Tingle traps
 * HD implementation verified against the original; tww provides names and order.
 */
#include "d/actor/d_a_obj_tntrap.h"
#include "bindings.h"
using Trap = daObjTnTrap_c;
TnTrapTriangle* TnTrapTriangle_construct(TnTrapTriangle*);
static TnTrapMathStatics l_math;
static TnTrapDescriptors l_descriptors;
static TnTrapTriSource l_tri_src;
static TnTrapInitEntry l_actionInit[7] = {{&daObjTnTrap_c::trap_off_wait_act_init_proc},
                                          {&daObjTnTrap_c::trap_on_wait_act_init_proc},
                                          {&daObjTnTrap_c::demo_regist_wait_act_init_proc},
                                          {&daObjTnTrap_c::dummy_proc},
                                          {&daObjTnTrap_c::demo_wait2_act_init_proc},
                                          {&daObjTnTrap_c::demo_end_wait_act_init_proc},
                                          {&daObjTnTrap_c::dummy_proc}};

static TnTrapProcEntry l_actionProc[7] = {
    {&daObjTnTrap_c::trap_off_wait_act_proc},    {&daObjTnTrap_c::trap_on_wait_act_proc},
    {&daObjTnTrap_c::demo_regist_wait_act_proc}, {&daObjTnTrap_c::demo_wait_act_proc},
    {&daObjTnTrap_c::demo_wait2_act_proc},       {&daObjTnTrap_c::demo_end_wait_act_proc},
    {&daObjTnTrap_c::hide_wait_act_proc}};

static u32 l_tri_indices[12] = {0, 1, 2, 0, 2, 3, 3, 2, 5, 3, 5, 4};
static cXyz l_tri_points[6] = {{300, -70, 0}, {-300, -70, 0}, {-300, 55, 0},
                               {300, 55, 0},  {300, 180, 0},  {-300, 180, 0}};
static cXyz l_emitter_offsetA[2] = {{-300, 90, 0}, {300, 90, 0}};
static cXyz l_emitter_offsetB[3] = {{0, 25, 0}, {0, 85, 0}, {0, 145, 0}};
// HD: cut names and the checked literals are guest byte strings.
static bool sameString(const char* a, const char* b) {
    const be<u8>* left = reinterpret_cast<const be<u8>*>(a);
    const be<u8>* right = gabi::at<be<u8>>(gabi::guest_addr(b));
    while (true) {
        u8 x = *left++, y = *right++;
        if (x != y)
            return false;
        if (!x)
            return true;
    }
}

/* 023A2598 */
BOOL daObjTnTrap_c::chk_appear() {
    mSwSave = getPrm(8, 0);
    mSwSave2 = getPrm(8, 8);
    mArg0 = getPrm(1, 16);
    u32 type = getPrm(2, 17);
    mMapType = type;
    if (type == MAP_EVENT) {
        if (dComIfGs_isEventBit(dSv_event_flag_UNK_3A04) != 1)
            return 0;
        u32 sw = mSwSave;
        if (sw == 255 || dComIfGs_isSwitch(sw, home.roomNo) != 1) {
            mTrapType = TYPE_SWITCH;
            return 1;
        }
        if (dComIfGs_getTriforceNum() != 8 || mArg0 != 0)
            return 0;
        if (dComIfGs_isEventBit(dSv_event_flag_UNK_2C01) == 1) {
            sw = mSwSave2;
            if (sw == 255 || dComIfGs_isSwitch(sw, home.roomNo) != 0)
                return 0;
            mTrapType = TYPE_EVENT2;
            return 1;
        }
        mTrapType = TYPE_EVENT;
        return 1;
    }
    if (type == MAP_SHIP) {
        u32 sw = mSwSave;
        if (sw == 255 || dComIfGs_isSwitch(sw, home.roomNo) != 0)
            return 0;
        mTrapType = TYPE_SHIP;
        return 1;
    }
    if (type == MAP_VERTICAL) {
        u32 sw = mSwSave;
        if (sw != 255 && dComIfGs_isSwitch(sw, home.roomNo) != 0)
            return 0;
        mTrapType = TYPE_VERTICAL;
        return 1;
    }
    JUT_ASSERT_fail("d_a_obj_tntrap.cpp", 0x17C, "0");
    return 0;
}

/* 023A28C0 */
void daObjTnTrap_c::set_mtx() {
    PSMTXTrans(mDoMtx_stack_c::get(), (f32)home.pos.x, (f32)home.pos.y, (f32)home.pos.z);
    mDoMtx_XYZrotM(mDoMtx_stack_c::get(), (s32)(s16)shape_angle.x, (s32)(s16)shape_angle.y,
                   (s32)(s16)shape_angle.z);
    mDoMtx_stack_c::transM(0.f, -9000.f, -94.f);
    mDoMtx_stack_c::scaleM((f32)scale.x, 100.f, (f32)scale.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &mBgMtx);
}

/* 023A2814 */
static BOOL solidHeapCB(Trap* trap) {
    return trap->create_heap();
}

/* 023A2794 */
BOOL daObjTnTrap_c::create_heap() {
    cBgD_t* data = static_cast<cBgD_t*>(dComIfG_getObjectRes("TnTrap", TNTRAP_DZB, VT_ARCHIVE));
    dBgW* background = dBgW_NewSet(data, 1, &mBgMtx);
    mpBgW = background;
    return background != 0;
}

/* 023A2FA4 */
void daObjTnTrap_c::particle_set(s32 group, f32 height) {
    if (mParticleActive[group]) {
        if ((f32)mEmSetOffsetY[group] == height)
            return;
        particle_delete(group);
    }
    for (u32 i = 0; i < 2; ++i) {
        if (!mpEmitterA[group][i]) {
            const cXyz* offset = &l_emitter_offsetA[i];
            f32 y = f32(offset->y) + height, z = f32(offset->z), x = f32(offset->x);
            dPa_control_c* controller = dComIfGp_getParticle();
            TnTrapEmitter* emitter = reinterpret_cast<TnTrapEmitter*>(
                controller->set(0, PARTICLE_SIDE, &home.pos, &shape_angle, 0, 255, 0, -1, 0, 0, 0));
            mpEmitterA[group][i] = emitter;
            emitter->mEmitterTranslation[0] = x;
            emitter->mEmitterTranslation[2] = z;
            emitter->mEmitterTranslation[1] = y;
        }
    }
    for (u32 i = 0; i < 3; ++i) {
        if (!mpEmitterB[group][i]) {
            const cXyz* offset = &l_emitter_offsetB[i];
            f32 y = f32(offset->y) + height, z = f32(offset->z), x = f32(offset->x);
            dPa_control_c* controller = dComIfGp_getParticle();
            TnTrapEmitter* emitter = reinterpret_cast<TnTrapEmitter*>(controller->set(
                0, PARTICLE_CENTER, &home.pos, &shape_angle, 0, 255, 0, -1, 0, 0, 0));
            mpEmitterB[group][i] = emitter;
            emitter->mEmitterTranslation[0] = x;
            emitter->mEmitterTranslation[2] = z;
            emitter->mEmitterTranslation[1] = y;
        }
    }
    mEmSetOffsetY[group] = height;
    mParticleActive[group] = 1;
}

/* 023A2CEC */
void daObjTnTrap_c::particle_delete(s32 group) {
    if (mParticleActive[group]) {
        for (u32 i = 0; i < 2; ++i) {
            TnTrapEmitter* emitter = mpEmitterA[group][i];
            if (emitter) {
                u32 flags = u32(emitter->mFlags);
                emitter->mMaxFrame = -1;
                emitter->mFlags = flags | jpa_emitter_calc::JPAEmtrStts_StopEmit;
                mpEmitterA[group][i] = 0;
            }
        }
        for (u32 i = 0; i < 3; ++i) {
            TnTrapEmitter* emitter = mpEmitterB[group][i];
            if (emitter) {
                u32 flags = u32(emitter->mFlags);
                emitter->mMaxFrame = -1;
                emitter->mFlags = flags | jpa_emitter_calc::JPAEmtrStts_StopEmit;
                mpEmitterB[group][i] = 0;
            }
        }
    }
    mParticleActive[group] = 0;
}

/* 023A2E34 */
void daObjTnTrap_c::set_se() {
    u32 action = mAction;
    if (action >= ACT_TRAP_ON_WAIT && action <= ACT_DEMO_WAIT2) {
        s32 reverb = dComIfGp_getReverb(s32(s8(current.roomNo)));
        mDoAud_seStart(0x6239, &current.pos, 0, reverb);
    }
}

/* 023A2958 */
void daObjTnTrap_c::set_tri(s32 group) {
    f32 y = (f32)home.pos.y + (f32)mEmSetOffsetY[group];
    PSMTXTrans(mDoMtx_stack_c::get(), (f32)home.pos.x, y, (f32)home.pos.z);
    mDoMtx_XYZrotM(mDoMtx_stack_c::get(), (s32)(s16)shape_angle.x, (s32)(s16)shape_angle.y,
                   (s32)(s16)shape_angle.z);
    gabi::Local<cXyz[3]> points;
    for (u32 triangle = 0; triangle < 4; ++triangle) {
        for (u32 vertex = 0; vertex < 3; ++vertex) {
            u32 index = l_tri_indices[triangle * 3 + vertex];
            const cXyz* source = &l_tri_points[index];
            cXyz* point = &(*points)[vertex];
            point->x = f32(source->x);
            point->y = f32(source->y);
            point->z = f32(source->z);
            PSMTXMultVec(mDoMtx_stack_c::get(), point, point);
        }
        mTri[group * 4 + triangle].mTri.setPos(&(*points)[0], &(*points)[1], &(*points)[2]);
    }
}

/* 023A319C */
BOOL daObjTnTrap_c::chk_event_flg() {
    u32 type = mTrapType;
    if (type == TYPE_SWITCH) {
        u32 sw = mSwSave;
        if (sw != 255 && dComIfGs_isSwitch(sw, home.roomNo) == 1) {
            s32 action = ACT_DEMO_WAIT2;
            if (mArg0 == 0) {
                action = ACT_DEMO_REGIST_WAIT;
                dComIfGs_onEventBit(dSv_event_flag_UNK_3B40);
            }
            setup_action(action);
        }
    } else if (type == TYPE_EVENT2) {
        if (mAction == ACT_TRAP_ON_WAIT)
            setup_action(ACT_DEMO_REGIST_WAIT);
    } else if (type == TYPE_SHIP) {
        u32 sw = mSwSave;
        if (sw != 255 && dComIfGs_isSwitch(sw, home.roomNo) == 1) {
            TnTrapShip* boat = reinterpret_cast<TnTrapShip*>(dComIfGp_getPlayer(2));
            if (boat) {
                boat->mStateFlag &= ~u32(TnTrapShip::daSFLG_UNK800000_e);
                fopAcM_delete(this);
                return 0;
            }
        }
    } else if (type == TYPE_VERTICAL) {
        u32 sw = mSwSave;
        if (sw != 255 && dComIfGs_isSwitch(sw, home.roomNo) == 1) {
            fopAcM_delete(this);
            return 0;
        }
    }
    return 1;
}

/* 023A2818 */
void daObjTnTrap_c::set_em_set_offsetY() {
    if (mTrapType == TYPE_VERTICAL) {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        if (player) {
            f32 difference = f32(player->eyePos.y) - (f32)home.pos.y;
            f32 rows = difference / 180.f;
            mEmSetOffsetY[0] = (f32)gabi::ftoi(rows) * 180.f;
        }
    } else
        mEmSetOffsetY[0] = 0.f;
}

/* 023A3CA8 */
u32 daObjTnTrap_c::getPrm(u32 bits, u32 shift) {
    u32 value = mParameters, one = bits & 32 ? 0u : 1u << (bits & 31);
    u32 mask = one - 1;
    return (shift & 32 ? 0u : value >> (shift & 31)) & mask;
}

/* 023A2B14 */
s32 daObjTnTrap_c::_create() {
    u32 flags = actor_condition;
    s32 phase = cPhs_ERROR_e;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (this) {
            fopAc_ac_c_ct(this);
            __vtbl = VT_ACTOR;
            tntrapStatusConstruct(&mStts);
            tntrapGlobalStatusConstruct(&mStts.__vtbl_gstts);
            mStts.__vtbl = VT_STATUS;
            mStts.__vtbl_gstts = VT_GLOBAL_STATUS;
            tntrapConstructArray(mTri, 8, sizeof(TnTrapTriangle),
                                 gabi::guest_fn((const void*)TnTrapTriangle_construct));
            flags = u32(actor_condition);
        }
        actor_condition = flags | fopAcCnd_INIT_e;
    }
    if (u8(mInitState) == 0) {
        s32 appears = chk_appear();
        mAppears = (u8)appears;
        if (!appears)
            return phase;
    } else if (!mAppears)
        return phase;
    phase = dComIfG_resLoad(&mPhase, "TnTrap");
    if (phase == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)solidHeapCB), 0x2E0))
            return cPhs_ERROR_e;
        if (mTrapType != TYPE_SHIP) {
            dComIfG_inf_c* world = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
            if (world->play.mBgS.Regist(mpBgW, this))
                return cPhs_ERROR_e;
        }
        set_em_set_offsetY();
        set_mtx();
        mStts.Init(255, 255, this);
        for (u32 group = 0; group < 2; ++group) {
            for (u32 i = 0; i < 4; ++i) {
                auto* triangle = &mTri[group * 4 + i];
                triangle->Set(reinterpret_cast<dCcD_SrcTri*>(&l_tri_src));
                triangle->SetStts(&mStts);
            }
            set_tri(group);
        }
        setup_action(mTrapType == TYPE_SHIP ? ACT_HIDE_WAIT : ACT_TRAP_OFF_WAIT);
    }
    return phase;
}

/* 023A2D88 */
BOOL daObjTnTrap_c::_delete() {
    if (mAppears) {
        dComIfG_resDelete(&mPhase, "TnTrap");
        dBgW* background = mpBgW;
        if (heap.get() && background) {
            if (background->ChkUsed()) {
                dComIfG_inf_c* world = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
                world->play.mBgS.Release(mpBgW);
            }
            mpBgW = 0;
        }
        for (u32 i = 0; i < 2; ++i)
            particle_delete(i);
    }
    return 1;
}

/* 023A3458 */
BOOL daObjTnTrap_c::trap_off_wait_act_proc() {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if (player) {
        gabi::Local<cXyz> difference;
        cXyz_mi(&player->current.pos, difference.get(), &home.pos);
        gabi::Local<cXyz> horizontal;
        horizontal->x = difference->x;
        horizontal->y = 0.f;
        horizontal->z = difference->z;
        // HD: the distance comparison consumes the full floating-point result register.
        f64 square = tntrapSquareRegister(horizontal.get());
        f64 distance = tntrapSqrtRegister(square);
        if (distance < 500.f)
            setup_action(ACT_TRAP_ON_WAIT);
    }
    return chk_event_flg();
}

/* 023A34EC */
BOOL daObjTnTrap_c::trap_on_wait_act_proc() {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if (player) {
        gabi::Local<cXyz> difference;
        cXyz_mi(&player->current.pos, difference.get(), &home.pos);
        gabi::Local<cXyz> horizontal;
        horizontal->x = difference->x;
        horizontal->y = 0.f;
        horizontal->z = difference->z;
        // HD: the distance comparison consumes the full floating-point result register.
        f64 square = tntrapSquareRegister(horizontal.get());
        f64 distance = tntrapSqrtRegister(square);
        if (distance > 500.f) {
            setup_action(ACT_TRAP_OFF_WAIT);
            return chk_event_flg();
        }
        if (mTrapType == TYPE_VERTICAL) {
            f32 distances[2];
            for (u32 i = 0; i < 2; ++i) {
                f32 center = ((f32)home.pos.y + (f32)mEmSetOffsetY[i]) + 90.f;
                f32 delta = f32(player->current.pos.y) - center;
                distances[i] = delta;
                if (std::fabs((f64)delta) > 150.f)
                    particle_delete(i);
            }
            for (u32 i = 0; i < 2; ++i) {
                f32 delta = distances[i];
                if (std::fabs((f64)delta) > 80.f) {
                    f32 homeY = home.pos.y, offset = mEmSetOffsetY[i];
                    f32 next = delta > 0.f ? offset + 180.f : offset - 180.f;
                    f32 center = (homeY + next) + 90.f,
                        further = f32(player->current.pos.y) - center;
                    if (!(std::fabs((f64)further) > 150.f))
                        particle_set(i ^ 1, next);
                }
            }
        }
    }
    return chk_event_flg();
}

/* 023A3710 */
BOOL daObjTnTrap_c::demo_regist_wait_act_proc() {
    s32 event = mEventIdx;
    if (event != -1) {
        if (eventInfo.checkCommandDemoAccrpt())
            setup_action(ACT_DEMO_WAIT);
        else
            tntrapOrderEventRegister(this, event, 255, 65535, 0, 1);
    } else {
        s32 type = mTrapType;
        dComIfG_inf_c* world = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        mEventIdx = (s16)tntrapEventIndexRegister(
            &world->play.mEvtManager, type == TYPE_EVENT2 ? "break_tntrap2" : "break_tntrap", 255);
    }
    return 1;
}

/* 023A37D0 */
BOOL daObjTnTrap_c::demo_wait_act_proc() {
    s32 event = mEventIdx;
    dComIfG_inf_c* world = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    if (tntrapEventData(&world->play.mEvtManager, event)) {
        dComIfG_inf_c* manager = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        s32 staff = tntrapStaffId(&manager->play.mEvtManager, "TnTrap", 0, 0);
        if (staff != -1) {
            manager = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
            const char* cut = tntrapCutName(&manager->play.mEvtManager, staff);
            if (sameString(cut, "Delete"))
                setup_action(ACT_DEMO_END_WAIT);
        }
    }
    return 1;
}

/* 023A3894 */
BOOL daObjTnTrap_c::demo_wait2_act_proc() {
    dComIfG_inf_c* first = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    dComIfG_inf_c* manager = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    // HD: preserve the full index register before the event-data lookup.
    s32 event = tntrapEventIndexRegister(&manager->play.mEvtManager, "break_tntrap", 255);
    if (tntrapEventData(&first->play.mEvtManager, event)) {
        manager = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        s32 staff = tntrapStaffId(&manager->play.mEvtManager, "TnTrap", 0, 0);
        if (staff != -1) {
            manager = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
            const char* cut = tntrapCutName(&manager->play.mEvtManager, staff);
            if (sameString(cut, "Delete2")) {
                fopAcM_delete(this);
                return 0;
            }
        }
    }
    return 1;
}

/* 023A3978 */
BOOL daObjTnTrap_c::demo_end_wait_act_proc() {
    s32 event = mEventIdx;
    dComIfG_inf_c* world = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    if (!tntrapEventEnd(&world->play.mEvtManager, event))
        return 1;
    world = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    world->play.mEvtCtrl.mEventFlag |= 8;
    u32 type = mTrapType;
    if (type == TYPE_SWITCH)
        mDoAud_seStartCom(0x806);
    else if (type == TYPE_EVENT2) {
        u32 sw = mSwSave2;
        if (sw != 255)
            dComIfGs_onSwitch(sw, home.roomNo);
    }
    fopAcM_delete(this);
    return 0;
}

/* 023A3A50 */
BOOL daObjTnTrap_c::hide_wait_act_proc() {
    u32 sw = mSwSave2;
    if (sw != 255 && dComIfGs_isSwitch(sw, home.roomNo) == 1) {
        TnTrapShip* boat = reinterpret_cast<TnTrapShip*>(dComIfGp_getPlayer(2));
        if (boat) {
            boat->mStateFlag |= TnTrapShip::daSFLG_UNK800000_e;
            dComIfG_inf_c* world = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
            if (!world->play.mBgS.Regist(mpBgW, this))
                setup_action(ACT_TRAP_OFF_WAIT);
        }
    }
    return 0;
}

/* 023A3C20 */
void daObjTnTrap_c::dummy_proc() {}

/* 023A3310 */
void daObjTnTrap_c::trap_off_wait_act_init_proc() {
    for (u32 i = 0; i < 2; ++i)
        particle_delete(i);
}

/* 023A334C */
void daObjTnTrap_c::trap_on_wait_act_init_proc() {
    set_em_set_offsetY();
    particle_set(0, (f32)mEmSetOffsetY[0]);
}

/* 023A3388 */
void daObjTnTrap_c::demo_regist_wait_act_init_proc() {
    mEmSetOffsetY[0] = 0.f;
    particle_set(0, 0.f);
    s32 type = mTrapType;
    dComIfG_inf_c* world = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    mEventIdx = (s16)tntrapEventIndexRegister(
        &world->play.mEvtManager, type == TYPE_EVENT2 ? "break_tntrap2" : "break_tntrap", 255);
}

/* 023A3408 */
void daObjTnTrap_c::demo_wait2_act_init_proc() {
    mEmSetOffsetY[0] = 0.f;
    particle_set(0, 0.f);
}

/* 023A341C */
void daObjTnTrap_c::demo_end_wait_act_init_proc() {
    for (u32 i = 0; i < 2; ++i)
        particle_delete(i);
}

/* 023A2A5C */
void daObjTnTrap_c::setup_action(s32 action) {
    (this->*l_actionInit[action].fn)();
    tntrapCopyPtmfHead(mActionProc, l_actionProc[action].fn);
    mAction = action;
    mActionProc.f = l_actionProc[action].fn.f;
}

/* 023A2E90 */
BOOL daObjTnTrap_c::_execute() {
    dBgW* background = mpBgW;
    if (background && background->ChkUsed())
        background->Move();
    for (u32 i = 0; i < 2; ++i)
        set_tri(i);
    mStts.Move();
    if ((this->*mActionProc)()) {
        for (u32 group = 0; group < 2; ++group)
            for (u32 i = 0; i < 4; ++i) {
                dComIfG_inf_c* world = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
                cCcS_Set(&world->play.mCcS, &mTri[group * 4 + i]);
            }
        set_se();
    }
    return 1;
}

/* 023A2F94 */
BOOL daObjTnTrap_c::_draw() {
    return 1;
}

/* 023A2CE8 */
static s32 daObjTnTrap_Create(Trap* trap) {
    return trap->_create();
}

/* 023A2E30 */
static BOOL daObjTnTrap_Delete(Trap* trap) {
    return trap->_delete();
}

/* 023A2F90 */
static BOOL daObjTnTrap_Execute(Trap* trap) {
    return trap->_execute();
}

/* 023A2F9C */
static BOOL daObjTnTrap_IsDelete(Trap* trap) {
    return 1;
}

// HD: compiler-generated and template code of this translation unit
/* 023A3AEC */
void __sinit_d_a_obj_tntrap_cpp() {
    for (u32 i = 0; i < 4; ++i)
        l_math.mZero[i] = 0;
    __register_global_object(gabi::ea(l_descriptors.mEntries[0]));
    l_math.mMinAngle = -3.1415927410125732f;
    l_math.mMaxAngle = 3.1415927410125732f;
    tntrapInitSinTable(&l_math.mSinTable);
    __register_global_object(gabi::ea(l_descriptors.mEntries[1]));
    tntrapInitAtanTable(&l_math.mAtanTable);
    __register_global_object(gabi::ea(l_descriptors.mEntries[2]));
}

// HD: allocating GHS triangle constructor and TU-specific vtable installation.
/* 023A3B80 */
TnTrapTriangle* TnTrapTriangle_construct(TnTrapTriangle* self) {
    if (!self)
        self = static_cast<TnTrapTriangle*>(operator_new(sizeof(TnTrapTriangle)));
    if (self) {
        tntrapTriangleConstruct(self);
        self->mShape.mShapeVtable = VT_SHAPE;
        self->mShape.mBoundsVtable = VT_BOUNDS;
        tntrapGeometryConstruct(&self->mTri);
        self->__vtbl_hitinf = VT_TRI_HIT;
        self->mTri.__vtbl = VT_TRI_PLANE;
        self->mShape.mShapeVtable = VT_TRI_SHAPE;
    }
    return self;
}

// HD: scalar deleting destructor for the header object.
/* 023A3C0C */
void TnTrap_headerStatics_destruct(void* self, s32 flags) {
    if (self && (flags & 1))
        operator_delete(self);
}

// HD: array destruction precedes the actor base destructor.
/* 023A3C24 */
void daObjTnTrap_destruct(Trap* trap, s32 flags) {
    if (trap) {
        tntrapDestroyArray(trap->mTri, 8, sizeof(TnTrapTriangle), FN_TRI_DESTRUCTOR, 0, 0);
        dCcD_Stts_dt(&trap->mStts, 2);
        tntrapActorDestruct(trap, 0);
        if (flags & 1)
            operator_delete(trap);
    }
}

// HD: empty virtual base entry retained.
/* 023A3CA4 */
void daObjTnTrap_baseNoop(Trap* trap) {}

/* WWHD glue
data l_math 1046C508
data l_descriptors 101CD3E4
data l_tri_src 100310E0
data l_tri_indices 101CD344
data l_tri_points 10031134
data l_emitter_offsetA 100310A4
data l_emitter_offsetB 100310BC
data l_actionProc 101CD374
data l_actionInit 101CD3AC
ext 025F19F8 void mDoMtx_XYZrotM(Mtx34*, s32, s32, s32)
ext 025E1988 void mDoAud_seStartCom(u32)
ext 028E8DD0 f64 tntrapSquareRegister(const cXyz*)
ext 028F4384 f64 tntrapSqrtRegister(f64)
ext 02543F10 s32 tntrapEventIndexRegister(dEvent_manager_c*, const char*, u32)
ext 02544044 void* tntrapEventData(dEvent_manager_c*, s32)
ext 02544830 const char* tntrapCutName(dEvent_manager_c*, s32)
ext 02542D88 s32 tntrapStaffId(dEvent_manager_c*, const char*, fopAc_ac_c*, s32)
ext 025440C8 BOOL tntrapEventEnd(dEvent_manager_c*, s32)
ext 025D7A58 void tntrapOrderEventRegister(fopAc_ac_c*, s32, u32, u32, u32, u32)
ext 028EFFD0 void tntrapConstructArray(void*, s32, u32, u32)
ext 028F0164 void tntrapDestroyArray(void*, s32, u32, u32, u32, u32)
ext 02515DA0 void tntrapGlobalStatusConstruct(be<u32>*)
ext 0200BD2C void tntrapStatusConstruct(dCcD_Stts*)
ext 02515FB8 void tntrapTriangleConstruct(dCcD_GObjInf*)
ext 02019040 void tntrapGeometryConstruct(cM3dGTri*)
ext 024F2478 dBgW* dBgW_NewSet(cBgD_t*, u32, Mtx34*)
ext 028ED6F8 void tntrapInitSinTable(u8*)
ext 028EAB2C void tntrapInitAtanTable(u8*)
ext 025D50BC void tntrapActorDestruct(fopAc_ac_c*, s32)
ext 0251650C void TnTrapTriangle::Set(const dCcD_SrcTri*)
ext 0201924C void cM3dGTri::setPos(const cXyz*, const cXyz*, const cXyz*)
str 10031064 "TnTrap" in 023A2794
str 10031064 "TnTrap" in 023A2B14
str 10031064 "TnTrap" in 023A2D88
str 10031180 "d_a_obj_tntrap.cpp" in 023A2598
str 1003117C "0" in 023A2598
str 100311B0 "break_tntrap2" in 023A3388
str 100311C0 "break_tntrap" in 023A3388
str 100311E0 "break_tntrap2" in 023A3710
str 100311F0 "break_tntrap" in 023A3710
str 10031200 "TnTrap" in 023A37D0
str 10031208 "Delete" in 023A37D0
str 10031210 "Delete2" in 023A3894
str 10031218 "TnTrap" in 023A3894
str 10031220 "break_tntrap" in 023A3894
*/
