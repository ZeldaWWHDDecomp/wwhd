/** Object - Hyrule Castle's Link statue.
 * GameCube names and layout, verified against WWHD function by function. */
#include "d/actor/d_a_obj_YLzou.h"
using namespace ylzou;

const gc_tu_data gc_tu = {.safeString = 0x10033AE4};

// HD: mapped static aliases have TU-specific names; numeric initializers are checked against the image.
static_assert(dRes_INDEX_YLZOU_BDL_YLZOU_e == 4 && dRes_INDEX_YLZOU_BDL_YLZOU2_e == 5);
static_assert(dRes_INDEX_YLZOU_DZB_YLZOU_e == 8 && dRes_INDEX_YLZOU_DZB_YLZOU2_e == 9);
static_assert(ACTION_OPEN_WAIT == 6 && ACTION_CLOSE_WAIT_1 == 11 && ACTION_CLOSE_WAIT_2 == 12);
static s32 ylzouBdlTable[2] = {4, 5};
static s32 ylzouDzbTable[2] = {8, 9};
static u32 ylzouSolidHeapSizeTable[2] = {0x5840, 0x49E0};
static s32 ylzouNextActIdx[4] = {-1, 6, 11, 12};
static ActionTableHD ylzouActInitTable;
static ActionTableHD ylzouActTable;
static DemoNamesHD ylzouDemoNames;
static SavePointerHD ylzouSaveRoot;
static ResourcePointerHD ylzouResourceControl;
static Mtx34 ylzouMatrixStack;
static SmokePositionsHD ylzouSmokePositions;
static cXyz ylzouSmokeScale;
static WordHD ylzouPositionGuard;
static WordHD ylzouScaleGuard;
static HeaderStaticsHD ylzouHeaderStatics;
static WordHD ylzouSmokeVtable;
static WordHD ylzouActorVtable;
static u32 ylzouSmokeVtableAddress() { return gabi::ea(&ylzouSmokeVtable); }
static ZeroStaticsHD ylzouZeroStatics;
static DestructorRecordHD ylzouHeaderRecordAngle;
static DestructorRecordHD ylzouHeaderRecordDestructor;
static DestructorRecordHD ylzouHeaderRecordPi;

static dBgW *ylzouNewBgW(void *dzb, s32 flags, Mtx34 *mtx);
static s32 ylzouGetEventIdx(dEvent_manager_c *manager, const char *name, s32 mapToolId);
static void *ylzouGetEventData(dEvent_manager_c *manager, s32 index);
static s32 ylzouEventEndCheck(dEvent_manager_c *manager, s32 index);
static s32 ylzouGetStaffId(dEvent_manager_c *manager, const char *name, void *actor, s32 priority);
static const char *ylzouGetCutName(dEvent_manager_c *manager, s32 staff);
static void ylzouCancelStartDemo(void *exception);
static s32 ylzouGetTriforceNum(dSv_player_collect_c *collect);
static void ylzouOffEvent(dSv_event_c *event, s32 bit);
static void ylzouStartQuake(dVibration_c *vibration, s32 strength, s32 flags, const cXyz *axis);
static void ylzouStartShock(dVibration_c *vibration, s32 strength, s32 flags, const cXyz *axis);
static void ylzouStopQuake(dVibration_c *vibration, s32 id);
static void ylzouActorConstruct(daObjYLzou_c *actor);
static void ylzouActorDestruct(daObjYLzou_c *actor, s32 flags);
static void ylzouStartSound(u32 id, const cXyz *pos, s32 flags, s32 reverb);
static void ylzouXYZrotM(Mtx34 *mtx, s32 x, s32 y, s32 z);
static void ylzouConstructArray(void *array, s32 count, s32 size, YLzouSmokeConstructorHD constructor);
static void ylzouDestroyArray(void *array, s32 count, s32 size, YLzouSmokeDestructorHD destructor,
                              void *unused, void *unused2);
static void ylzouRegisterGlobal(ylzou::DestructorRecordHD *record);
static void ylzouConstructAngleZero(u8 *object);
static void ylzouConstructDestructorZero(u8 *object);

static void *ylzouGetResource(void *, SafeString *, s32);
static s32 ylzouRegistBg(dBgS *bg, dBgW *world, daObjYLzou_c *actor);
static void ylzouReleaseBg(dBgS *bg, dBgW *world);
static void ylzouSettingTev(dScnKy_env_light_c *env, s32 type, const cXyz *pos, dKy_tevstr_c *tev);
static void ylzouSetLightTev(dScnKy_env_light_c *env, J3DModel *model, dKy_tevstr_c *tev);
// HD: forward the full event-index return register before its stored s16 copy is read.
static void ylzouOrderEvent(daObjYLzou_c *, s32, u32, u32, u32, u32);
static bool solidHeapCB(daObjYLzou_c *);
static void *smoke_cb_ctor(void *);
static void smoke_cb_dtor(void *, s32);
static u32 daObjPrmAbstractHD(daObjYLzou_c *, u32, u32);

static dComIfG_inf_c *ylzouPlay() { return reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get()); }
static SaveRootHD *ylzouSave() { return ylzouSaveRoot; }
static s32 eventBit(u32 bit) { return dComIfGs_isEventBit(bit); }
static void setEvent(u32 bit) { dComIfGs_onEventBit(bit); }
static void sound(daObjYLzou_c *i_this, u32 id) {
    s8 room = i_this->current.roomNo;
    s32 reverb = dComIfGp_getReverb(room);
    ylzouStartSound(id, &i_this->current.pos, 0, reverb);
}
/* inlined into daObjYLzou_c::move_ylzou_demo_vib_act_init_proc */
static void quake() {
    dComIfG_inf_c *game = ylzouPlay();
    cXyz axis;
    axis.x = 0;
    axis.y = 1;
    axis.z = 0;
    ylzouStartQuake(&game->play.mVibration, 6, 1, &axis);
}
static bool sameText(const char *a, const char *b) {
    const be<u8> *left = reinterpret_cast<const be<u8> *>(a);
    const be<u8> *right = reinterpret_cast<const be<u8> *>(b);
    for (;; ++left, ++right) {
        u8 x = *left;
        u8 y = *right;
        if (x != y)
            return false;
        if (x == 0)
            return true;
    }
}

/* 023BDADC */
void daObjYLzou_c::set_start_type() {
    s32 action_idx, triforce_complete = 0, demo_idx = DEMO_NONE;
    bool statue_moved = false;
    if (eventBit(EventFlagsHD::MASTER_SWORD_CUTSCENE) == 0) {
        s32 sw = field_0x2E0;
        if (sw != 255) {
            SaveRootHD *sv = ylzouSave();
            s8 room = home.roomNo;
            if (sv->info.isSwitch(sw, room) == 0) {
                action_idx = ACTION_MOVE_YLZOU_DEMO_START_WAIT;
                demo_idx = DEMO_MOVE_YLZOU;
            } else {
                action_idx = ACTION_OPEN_WAIT;
                statue_moved = true;
            }
        } else {
            action_idx = ACTION_OPEN_WAIT;
            statue_moved = true;
        }
    } else if (eventBit(EventFlagsHD::MASTER_SWORD_SWINGING_CUTSCENE) == 0) {
        action_idx = ACTION_OPEN_WAIT;
        statue_moved = true;
    } else {
        s32 courtyard = eventBit(EventFlagsHD::HYRULE_COURTYARD_CUTSCENE);
        SaveRootHD *sv = ylzouSave();
        if (courtyard == 0) {
            if (sv->info.mSavedata.mEvent.isEventBit(EventFlagsHD::MOVED_HYRULE_STATUE) == 1) {
                action_idx = ACTION_GO_UP_STAIRS_DEMO_REGIST_WAIT;
                demo_idx = DEMO_GO_UP_STAIRS;
            } else
                action_idx = ACTION_CLOSE_WAIT_1;
        } else if (sv->info.mSavedata.mEvent.isEventBit(EventFlagsHD::ZELDA_AWAKENED) == 0) {
            action_idx = ACTION_OPEN_WAIT;
            statue_moved = true;
        } else {
            s32 n = ylzouGetTriforceNum(&ylzouSave()->info.mSavedata.mPlayer.mCollect);
            sv = ylzouSave();
            if (n < 8) {
                if (sv->info.mSavedata.mEvent.isEventBit(EventFlagsHD::MOVED_HYRULE_STATUE) == 1) {
                    action_idx = ACTION_GO_UP_STAIRS_DEMO_REGIST_WAIT;
                    demo_idx = DEMO_GO_UP_STAIRS;
                } else
                    action_idx = ACTION_CLOSE_WAIT_1;
            } else {
                triforce_complete = 1;
                if (sv->info.mSavedata.mEvent.isEventBit(EventFlagsHD::UNK_2C01) == 0)
                    action_idx = ACTION_CLOSE_WAIT_2;
                else if (eventBit(EventFlagsHD::UNK_3980) == 0) {
                    action_idx = ACTION_GO_UP_STAIRS2_DEMO_REGIST_WAIT;
                    demo_idx = DEMO_GO_UP_STAIRS2;
                } else
                    action_idx = ACTION_CLOSE_WAIT_2;
            }
        }
    }
    field_0x2E8 = triforce_complete;
    mActionIdx = action_idx;
    mDemoIdx = demo_idx;
    SaveRootHD *sv = ylzouSave();
    if (statue_moved)
        sv->info.mSavedata.mEvent.onEventBit(EventFlagsHD::MOVED_HYRULE_STATUE);
    else
        ylzouOffEvent(&sv->info.mSavedata.mEvent, EventFlagsHD::MOVED_HYRULE_STATUE);
}

/* 023BDEE4 */
void daObjYLzou_c::set_mtx() {
    u16 angle = field_0x358;
    f32 y = current.pos.y;
    f32 amplitude = field_0x35C;
    f32 sine = cM_ssin(angle);
    y = gabi::fmadds(sine, amplitude, y);
    f32 z = current.pos.z, x = current.pos.x;
    mDoMtx_stack_c::transS(x, y, z);
    s16 ry = shape_angle.y, rz = shape_angle.z, rx = shape_angle.x;
    ylzouXYZrotM(&ylzouMatrixStack, rx, ry, rz);
    J3DModel *model = field_0x298;
    // HD: snapshot all twelve floats before the first model-matrix store.
    model->setBaseTRMtx(&ylzouMatrixStack);
    x = scale.x;
    z = scale.z;
    y = scale.y;
    mDoMtx_stack_c::scaleM(x, y, z);
    PSMTXCopy(&ylzouMatrixStack, &field_0x2A0);
}

/* 023BDFE4 */
void daObjYLzou_c::init_mtx() {
    J3DModel *model = field_0x298;
    f32 y = scale.y, x = scale.x, z = scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    set_mtx();
}

/* 023BDE28 */
static bool solidHeapCB(daObjYLzou_c *a) { return a->create_heap(); }

/* 023BDD20 */
bool daObjYLzou_c::create_heap() {
    u32 which = field_0x2E8;
    s32 idx = ylzouBdlTable[which];
    SafeString request;
    request.mStringTop = gabi::guest_addr("YLzou");
    request.__vtbl = gc_tu.safeString;
    J3DModelData *model_data =
        static_cast<J3DModelData *>(ylzouGetResource(ylzouResourceControl, &request, idx));
    if (!model_data) {
        // HD: the failed condition is spelled "0" in the image.
        JUT_ASSERT_fail("d_a_obj_YLzou.cpp", 0x194, "0");
        return false;
    }
    J3DModel *model = mDoExt_J3DModel__create(model_data, 0x80000, 0x11000022);
    field_0x298 = model;
    // HD: failed model creation returns before requesting collision data.
    if (!model)
        return false;
    which = field_0x2E8;
    SafeString collisionName;
    collisionName.mStringTop = gabi::guest_addr("YLzou");
    collisionName.__vtbl = gc_tu.safeString;
    idx = ylzouDzbTable[which];
    void *dzb = ylzouGetResource(ylzouResourceControl, &collisionName, idx);
    model = field_0x298;
    dBgW *bgw = ylzouNewBgW(dzb, dBgW::MOVE_BG_e, model ? &model->mBaseTransformMtx : nullptr);
    field_0x29C = bgw;
    return field_0x298.get() != nullptr && bgw != nullptr;
}

/* 023BE21C */
void daObjYLzou_c::eff_set_slip_smoke_pos() {
    // HD: the guarded vector array is initialized in GHS store order.
    if (ylzouPositionGuard == 0) {
        ylzouPositionGuard = 1;
        ylzouSmokePositions.entries[1].y = 0;
        ylzouSmokePositions.entries[1].x = 240;
        ylzouSmokePositions.entries[0].z = -240;
        ylzouSmokePositions.entries[0].x = -240;
        ylzouSmokePositions.entries[1].z = -240;
        ylzouSmokePositions.entries[0].y = 0;
    }
    f32 y = current.pos.y - 1550.0f, z = current.pos.z - 1200.0f, x = current.pos.x;
    mDoMtx_stack_c::transS(x, y, z);
    s16 ry = current.angle.y;
    mDoMtx_stack_c::YrotM(ry);
    for (int i = 0; i < 2; ++i) {
        mDoMtx_stack_c::push();
        x = ylzouSmokePositions.entries[i].x;
        y = ylzouSmokePositions.entries[i].y;
        z = ylzouSmokePositions.entries[i].z;
        mDoMtx_stack_c::transM(x, y, z);
        EffSmokeCB *cb = &mSmokeCbs[i];
        cb->field_0x20.x = ylzouMatrixStack.m[0][3];
        cb->field_0x20.y = ylzouMatrixStack.m[1][3];
        cb->field_0x20.z = ylzouMatrixStack.m[2][3];
        ry = current.angle.y;
        cb->field_0x2C.x = 0;
        cb->field_0x2C.y = ry;
        cb->field_0x2C.z = 0;
        mDoMtx_stack_c::pop();
    }
}

/* 023BE33C */
void daObjYLzou_c::eff_smoke_slip_start() {
    if (ylzouScaleGuard == 0) {
        ylzouSmokeScale.x = 2;
        ylzouSmokeScale.z = 2;
        ylzouScaleGuard = 1;
        ylzouSmokeScale.y = 2;
    }
    eff_set_slip_smoke_pos();
    for (int i = 0; i < 2; ++i) {
        s8 room = current.roomNo;
        EffSmokeCB *cb = &mSmokeCbs[i];
        dComIfG_inf_c *game = ylzouPlay();
        dPa_control_c *control = game->play.mParticle;
        EmitterHD *emitter = reinterpret_cast<EmitterHD *>(control->set(
            2, dPa_name::ID_AK_JT_ELEMENTSMOKE00, &cb->field_0x20, &cb->field_0x2C, &ylzouSmokeScale, 0xB9,
            reinterpret_cast<dPa_levelEcallBack *>(cb), room, nullptr, nullptr, nullptr));
        if (emitter) {
            emitter->mDirectionalSpeed = 15;
            emitter->mSpread = 0.15f;
            emitter->mRate = 2;
            emitter->mLifeTime = 30;
        }
    }
}

/* 023BE46C */
void daObjYLzou_c::eff_smoke_slip_end() {
    for (int i = 0; i < 2; ++i)
        mSmokeCbs[i].end();
}

/* 023BE148 */
void daObjYLzou_c::eff_smoke_slip_remove() {
    for (int i = 0; i < 2; ++i)
        mSmokeCbs[i].remove();
}

/* 023BE4B4 */
void daObjYLzou_c::eff_smoke_proc() {
    // HD: both smoke flags are tested for nonzero, rather than GameCube's equality to one.
    u8 requested = field_0x361, previous = field_0x360;
    if (previous != requested) {
        if (requested)
            eff_smoke_slip_start();
        else
            eff_smoke_slip_end();
        field_0x360 = field_0x361;
    } else if (previous)
        eff_set_slip_smoke_pos();
}

/* 023BE548 */
void daObjYLzou_c::vib_proc() {
    u32 previous = field_0x364, requested = field_0x368;
    s16 angle = field_0x358;
    if (previous != requested) {
        previous = requested;
        field_0x364 = previous;
    }
    field_0x358 = s16(u16(angle) + 0x4000);
    f32 height = field_0x35C;
    if (previous == 1) {
        height += 0.05f;
        if (height > 0.8f)
            height = 0.8f;
    } else if (previous == 2) {
        height -= 0.05f;
        if (height < 0.4f)
            height = 0.4f;
    } else {
        height -= 0.1f;
        if (height < 0)
            height = 0;
    }
    field_0x35C = height;
}

/* 023BE004 */
s32 daObjYLzou_c::_create() {
    u32 flags = actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            ylzouActorConstruct(this);
            __vtbl = gabi::ea(&ylzouActorVtable);
            ylzouConstructArray(mSmokeCbs, 2, 0x34, smoke_cb_ctor);
            flags = actor_condition;
        }
        actor_condition = flags | fopAcCnd_INIT_e;
    }
    if (mInitState == 0) {
        field_0x2E0 = daObjPrmAbstractHD(this, PARAM_SWSAVE_W, PARAM_SWSAVE_S);
        set_start_type();
    }
    s32 phase = dComIfG_resLoad(&field_0x290, "YLzou");
    if (phase == cPhs_COMPLEATE_e) {
        u32 idx = field_0x2E8;
        u32 heapSize = ylzouSolidHeapSizeTable[idx];
        if (fopAcM_entrySolidHeap(this, reinterpret_cast<BOOL (*)(fopAc_ac_c *)>(solidHeapCB), heapSize)) {
            dComIfG_inf_c *p = ylzouPlay();
            dBgW *bg = field_0x29C;
            if (ylzouRegistBg(&p->play.mBgS, bg, this))
                phase = cPhs_ERROR_e;
            else {
                J3DModel *model = field_0x298;
                s32 idx = mActionIdx;
                cullMtx = model ? gabi::ea(&model->mBaseTransformMtx) : 0;
                setup_action(idx);
                init_mtx();
            }
        } else
            phase = cPhs_ERROR_e;
    }
    return phase;
}

/* 023BE198 */
bool daObjYLzou_c::_delete() {
    dComIfG_resDelete(&field_0x290, "YLzou");
    eff_smoke_slip_remove();
    if (heap) {
        dBgW *bg = field_0x29C;
        if (bg) {
            if (bg->ChkUsed()) {
                dComIfG_inf_c *p = ylzouPlay();
                bg = field_0x29C;
                ylzouReleaseBg(&p->play.mBgS, bg);
            }
            field_0x29C = nullptr;
        }
    }
    return true;
}

/* 023BEA30 */
void daObjYLzou_c::move_ylzou_demo_start_wait_act_proc() {
    s32 sw = field_0x2E0;
    if (sw != 255) {
        s8 room = home.roomNo;
        SaveRootHD *sv = ylzouSave();
        if (sv->info.isSwitch(sw, room) == 1)
            setup_action(ACTION_MOVE_YLZOU_DEMO_REGIST_WAIT);
    }
}

/* 023BEA90 */
void daObjYLzou_c::demo_regist_wait_act_proc() {
    s16 idx = field_0x2E4;
    if (idx != -1) {
        if (eventInfo.mCommand == dEvtCmd_INDEMO_e)
            setup_action(u32(mActionIdx) + 1);
        else
            ylzouOrderEvent(this, idx, 255, 65535, 0, 1);
    } else {
        const char *name = ylzouDemoNames.entries[u32(mDemoIdx)];
        dComIfG_inf_c *p = ylzouPlay();
        field_0x2E4 = ylzouGetEventIdx(&p->play.mEvtManager, name, 255);
    }
}

/* 023BEB5C */
void daObjYLzou_c::demo_vib_start_wait_act_proc() {
    s16 event = field_0x2E4;
    dComIfG_inf_c *p = ylzouPlay();
    void *data = ylzouGetEventData(&p->play.mEvtManager, event);
    if (data) {
        p = ylzouPlay();
        s32 staff = ylzouGetStaffId(&p->play.mEvtManager, "YLzou", nullptr, 0);
        if (staff != -1) {
            p = ylzouPlay();
            const char *text = ylzouGetCutName(&p->play.mEvtManager, staff);
            if (sameText(text, gabi::at<const char>(gabi::guest_addr("Vibrate"))))
                setup_action(u32(mActionIdx) + 1);
        }
    }
}

/* 023BEC20 */
void daObjYLzou_c::demo_vib_act_proc() {
    s16 event = field_0x2E4;
    dComIfG_inf_c *p = ylzouPlay();
    void *data = ylzouGetEventData(&p->play.mEvtManager, event);
    if (data) {
        p = ylzouPlay();
        s32 staff = ylzouGetStaffId(&p->play.mEvtManager, "YLzou", nullptr, 0);
        if (staff != -1) {
            p = ylzouPlay();
            const char *text = ylzouGetCutName(&p->play.mEvtManager, staff);
            if (sameText(text, gabi::at<const char>(gabi::guest_addr("Move"))))
                setup_action(u32(mActionIdx) + 1);
        }
    }
    sound(this, JA_SE_OBJ_H_STATUE_VIB);
}

/* 023BED04 */
void daObjYLzou_c::move_ylzou_demo_move_act_proc() {
    f32 homeZ = home.pos.z, z = current.pos.z;
    if (z < homeZ - 680.0f) {
        sound(this, JA_SE_OBJ_H_STATUE_STOP);
        setup_action(ACTION_MOVE_YLZOU_DEMO_END_WAIT);
    } else {
        f32 speed = speedF + 0.1f;
        s8 room = current.roomNo;
        if (speed > 6.0f)
            speed = 6.0f;
        speedF = speed;
        s32 reverb = dComIfGp_getReverb(room);
        ylzouStartSound(JA_SE_OBJ_H_STATUE_MOVE, &current.pos, 0, reverb);
    }
}

/* 023BEDD0 */
void daObjYLzou_c::go_up_stairs_demo_move_act_proc() {
    f32 homeZ = home.pos.z, z = current.pos.z;
    if (z > homeZ) {
        sound(this, JA_SE_OBJ_H_STATUE_STOP);
        setup_action(ACTION_GO_UP_STAIRS_DEMO_END_WAIT);
    } else {
        f32 speed = speedF + 0.1f;
        s8 room = current.roomNo;
        if (speed > 6.0f)
            speed = 6.0f;
        speedF = speed;
        s32 reverb = dComIfGp_getReverb(room);
        ylzouStartSound(JA_SE_OBJ_H_STATUE_MOVE, &current.pos, 0, reverb);
    }
}

/* 023BEE90 */
void daObjYLzou_c::demo_end_wait_act_proc() {
    s16 event = field_0x2E4;
    dComIfG_inf_c *p = ylzouPlay();
    if (ylzouEventEndCheck(&p->play.mEvtManager, event)) {
        p = ylzouPlay();
        u16 status = p->play.mEvtCtrl.mEventFlag;
        p->play.mEvtCtrl.mEventFlag = status | 8;
        if (s32(mActionIdx) == ACTION_GO_UP_STAIRS2_DEMO_END_WAIT)
            setEvent(EventFlagsHD::UNK_3980);
        u32 demo = mDemoIdx;
        u32 flags = actor_status;
        actor_status = flags & ~u32(fopAcStts_UNK4000_e);
        s32 next = ylzouNextActIdx[demo];
        setup_action(next);
    }
}

/* 023BF0C4 */
void daObjYLzou_c::wait_act_proc() {}

/* 023BE720 */
void daObjYLzou_c::move_ylzou_demo_start_wait_act_init_proc() {
    field_0x2E4 = -1;
    current.pos.z = 0;
    home.pos.y = 0;
    current.pos.y = 0;
    current.pos.x = 0;
    home.pos.z = 0;
    home.pos.x = 0;
}

/* 023BE74C */
void daObjYLzou_c::demo_regist_wait_act_init_proc() {
    s32 demo = mDemoIdx;
    if (demo != DEMO_MOVE_YLZOU) {
        dComIfG_inf_c *p = ylzouPlay();
        ylzouCancelStartDemo(reinterpret_cast<EventManagerPrefixHD *>(&p->play.mEvtManager)->mException);
        u32 status = actor_status;
        demo = mDemoIdx;
        actor_status = status | fopAcStts_UNK4000_e;
    }
    const char *name = ylzouDemoNames.entries[u32(demo)];
    dComIfG_inf_c *p = ylzouPlay();
    s32 idx = ylzouGetEventIdx(&p->play.mEvtManager, name, 255);
    field_0x2E4 = idx;
    ylzouOrderEvent(this, idx, 255, 65535, 0, 1);
}

/* 023BE7EC */
void daObjYLzou_c::move_ylzou_demo_vib_start_wait_act_init_proc() {
    setEvent(EventFlagsHD::MOVED_HYRULE_STATUE);
}

/* 023BE800 */
void daObjYLzou_c::move_ylzou_demo_vib_act_init_proc() {
    quake();
    field_0x368 = 1;
}

/* 023BE864 */
void daObjYLzou_c::move_ylzou_demo_move_act_init_proc() {
    field_0x361 = 1;
    field_0x368 = 2;
    current.angle.y = -32768;
}

/* 023BE880 */
void daObjYLzou_c::demo_end_wait_act_init_proc() {
    s32 demo = mDemoIdx;
    field_0x361 = 0;
    speedF = 0;
    if (demo != DEMO_GO_UP_STAIRS2) {
        dComIfG_inf_c *p = ylzouPlay();
        ylzouStopQuake(&p->play.mVibration, -1);
        p = ylzouPlay();
        cXyz axis;
        axis.z = 0;
        axis.x = 0;
        axis.y = 1;
        ylzouStartShock(&p->play.mVibration, 8, 1, &axis);
    }
    field_0x368 = 0;
}

/* 023BE928 */
void daObjYLzou_c::open_wait_act_init_proc() {
    home.pos.x = 0;
    current.pos.z = -680;
    home.pos.y = 0;
    home.pos.z = -680;
    current.pos.x = 0;
    current.pos.y = 0;
}

/* 023BEA0C */
void daObjYLzou_c::close_wait_act_init_proc() {
    current.pos.z = 0;
    home.pos.y = 0;
    current.pos.y = 0;
    current.pos.x = 0;
    home.pos.z = 0;
    home.pos.x = 0;
}

/* 023BE954 */
void daObjYLzou_c::go_up_stairs_demo_move_start_wait_act_init_proc() {
    current.angle.y = 0;
    home.pos.x = 0;
    current.pos.x = 0;
    home.pos.y = 0;
    current.pos.y = 0;
    home.pos.z = 0;
    current.pos.z = -680;
    quake();
    field_0x368 = 1;
}

/* 023BE9F8 */
void daObjYLzou_c::go_up_stairs_demo_move_act_init_proc() {
    field_0x361 = 1;
    field_0x368 = 2;
}

/* 023BDE2C */
void daObjYLzou_c::setup_action(s32 idx) {
    u32 offset = u32(idx);
    (this->*ylzouActInitTable.entries[offset].fn)();
    u32 first = ylzouActTable.entries[offset].words[0];
    mCurrentActProc.words[0] = first;
    u32 second = ylzouActTable.entries[offset].words[1];
    mActionIdx = idx;
    mCurrentActProc.words[1] = second;
}

/* 023BE5F8 */
bool daObjYLzou_c::_execute() {
    fopAcM_posMoveF(this, nullptr);
    set_mtx();
    dBgW *bg = field_0x29C;
    if (bg && bg->ChkUsed())
        bg->Move();
    (this->*mCurrentActProc.fn)();
    eff_smoke_proc();
    vib_proc();
    return true;
}

/* 023BE6B8 */
bool daObjYLzou_c::_draw() {
    dScnKy_env_light_c *env = dKy_getEnvlight();
    ylzouSettingTev(env, 1, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    J3DModel *model = field_0x298;
    ylzouSetLightTev(env, model, &tevStr);
    model = field_0x298;
    mDoExt_modelUpdateDL(model, 0);
    return true;
}

/* 023BE144 */
static s32 daObjYLzou_Create(daObjYLzou_c *a) { return a->_create(); }

/* 023BE218 */
static bool daObjYLzou_Delete(daObjYLzou_c *a) { return a->_delete(); }

/* 023BE6B4 */
static bool daObjYLzou_Execute(daObjYLzou_c *a) { return a->_execute(); }

/* 023BE714 */
static bool daObjYLzou_Draw(daObjYLzou_c *a) { return a->_draw(); }

/* 023BE718 */
static s32 daObjYLzou_IsDelete(daObjYLzou_c *a) { return 1; }

// HD: compiler-generated and template code of this translation unit.
/* 023BEF30 */
static void __sinit_d_a_obj_YLzou_cpp() {
    ylzouZeroStatics.words[2] = 0;
    ylzouZeroStatics.words[0] = 0;
    ylzouZeroStatics.words[3] = 0;
    ylzouZeroStatics.words[1] = 0;
    ylzouRegisterGlobal(&ylzouHeaderRecordAngle);
    ylzouHeaderStatics.negativePi = -3.1415927410125732f;
    ylzouHeaderStatics.positivePi = 3.1415927410125732f;
    ylzouConstructAngleZero(&ylzouHeaderStatics.angleZero);
    ylzouRegisterGlobal(&ylzouHeaderRecordDestructor);
    ylzouConstructDestructorZero(&ylzouHeaderStatics.destructorZero);
    ylzouRegisterGlobal(&ylzouHeaderRecordPi);
}

/* 023BEFC4 */
static void *smoke_cb_ctor(void *object) {
    EffSmokeCB *cb = static_cast<EffSmokeCB *>(object);
    if (!cb)
        cb = static_cast<EffSmokeCB *>(operator_new(sizeof(EffSmokeCB)));
    if (cb) {
        __ct(static_cast<dPa_smokeEcallBack *>(cb), 1);
        cb->__vtbl = ylzouSmokeVtableAddress();
    }
    return cb;
}

/* 023BF01C */
static void callback_base_dtor(void *object, s32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}

/* 023BF038 */
static void smoke_cb_dtor(void *object, s32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}

/* 023BF030 */
static void callback_execute(void *object) {}

/* 023BF034 */
static void callback_draw(void *object) {}

/* 023BF04C */
static void callback_setup(void *object) {}

/* 023BF050 */
static void actor_dtor(daObjYLzou_c *a, s32 flags) {
    if (a) {
        ylzouDestroyArray(a->mSmokeCbs, 2, sizeof(EffSmokeCB), smoke_cb_dtor, nullptr, nullptr);
        ylzouActorDestruct(a, 0);
        if (flags & 1)
            operator_delete(a);
    }
}

/* 023BF0C8 */
static u32 daObjPrmAbstractHD(daObjYLzou_c *a, u32 width, u32 shift) {
    u32 bits = a->mParameters;
    // HD: PPC shifts use six count bits and yield zero for counts at least 32.
    width &= 63;
    shift &= 63;
    u32 mask = (width < 32 ? (1u << width) : 0u) - 1;
    return (shift < 32 ? bits >> shift : 0u) & mask;
}
/* WWHD glue
str 10033ADC "YLzou"
str 10033B84 "d_a_obj_YLzou.cpp"
str 10033B80 "0"
str 10033BD0 "Vibrate"
str 10033BE8 "Move"
str 10033BD8 "YLzou" in 023BEB5C
str 10033BE0 "YLzou" in 023BEC20
data ylzouBdlTable 101CE094
data ylzouDzbTable 101CE09C
data ylzouSolidHeapSizeTable 101CE194
data ylzouNextActIdx 101CE19C
data ylzouActInitTable 101CE0A4
data ylzouActTable 101CE11C
data ylzouDemoNames 101CE200
data ylzouSaveRoot 101F84DC
data ylzouResourceControl 101F4F28
data ylzouMatrixStack 1048D0CC
data ylzouSmokePositions 1046CA94
data ylzouSmokeScale 1046CABC
data ylzouPositionGuard 1046CAC8
data ylzouScaleGuard 1046CACC
data ylzouHeaderStatics 1046CA88
data ylzouZeroStatics 1046CAAC
data ylzouHeaderRecordAngle 101CE1AC
data ylzouHeaderRecordDestructor 101CE1B8
data ylzouHeaderRecordPi 101CE1C4
data ylzouSmokeVtable 10033B38
data ylzouActorVtable 10033AFC
ext 024F2478 dBgW* ylzouNewBgW(void* dzb, s32 flags, Mtx34* mtx)
ext 02543F10 s32 ylzouGetEventIdx(dEvent_manager_c* manager, const char* name, s32 mapToolId)
ext 02544044 void* ylzouGetEventData(dEvent_manager_c* manager, s32 index)
ext 025440C8 s32 ylzouEventEndCheck(dEvent_manager_c* manager, s32 index)
ext 02542D88 s32 ylzouGetStaffId(dEvent_manager_c* manager, const char* name, void* actor, s32 priority)
ext 02544830 const char* ylzouGetCutName(dEvent_manager_c* manager, s32 staff)
ext 0254351C void ylzouCancelStartDemo(void* exception)
ext 025B7E00 s32 ylzouGetTriforceNum(dSv_player_collect_c* collect)
ext 025B8B7C void ylzouOffEvent(dSv_event_c* event, s32 bit)
ext 025CB408 void ylzouStartQuake(dVibration_c* vibration, s32 strength, s32 flags, const cXyz* axis)
ext 025CB374 void ylzouStartShock(dVibration_c* vibration, s32 strength, s32 flags, const cXyz* axis)
ext 025CB610 void ylzouStopQuake(dVibration_c* vibration, s32 id)
ext 025D4ED0 void ylzouActorConstruct(daObjYLzou_c* actor)
ext 025D50BC void ylzouActorDestruct(daObjYLzou_c* actor, s32 flags)
ext 025E1A40 void ylzouStartSound(u32 id, const cXyz* pos, s32 flags, s32 reverb)
ext 025F19F8 void ylzouXYZrotM(Mtx34* mtx, s32 x, s32 y, s32 z)
ext 028EFFD0 void ylzouConstructArray(void* array, s32 count, s32 size, YLzouSmokeConstructorHD constructor)
ext 028F0164 void ylzouDestroyArray(void* array, s32 count, s32 size, YLzouSmokeDestructorHD destructor, void* unused, void* unused2)
ext 028F026C void ylzouRegisterGlobal(ylzou::DestructorRecordHD* record)
ext 028ED6F8 void ylzouConstructAngleZero(u8* object)
ext 028EAB2C void ylzouConstructDestructorZero(u8* object)
ext 025D7A58 void ylzouOrderEvent(daObjYLzou_c* actor, s32 event, u32 mapToolId, u32 p3, u32 priority, u32 flags)
ext 026066C4 void* ylzouGetResource(void* control, SafeString* name, s32 index)
ext 024EEA6C s32 ylzouRegistBg(dBgS* bg, dBgW* world, daObjYLzou_c* actor)
ext 020087EC void ylzouReleaseBg(dBgS* bg, dBgW* world)
ext 025626A4 void ylzouSettingTev(dScnKy_env_light_c* env, s32 type, const cXyz* pos, dKy_tevstr_c* tev)
ext 02562F5C void ylzouSetLightTev(dScnKy_env_light_c* env, J3DModel* model, dKy_tevstr_c* tev)
*/
