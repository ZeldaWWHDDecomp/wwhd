/* Warp light shafts: audited WWHD translation unit. */
#include "d/actor/d_a_warpls.h"

enum daWarpls_WarpType_e { Type_Tower, Type_Dungeon };
enum daWarpls_EvtType_e { EvtType_TowerUp, EvtType_TowerDown, EvtType_Dungeon };
enum daWarpls_EvtState_e { EvtState_None, EvtState_Activation, EvtState_Warping };
enum WarplsSoundHD { WarpAppear = 0x6967, WarpSustain = 0x7025, WarpTravel = 0x285D };

const gc_tu_data gc_tu = {.safeString = 0x100427BC};
static const char* m_arcname[] = {"Ywarp00", "Ysdls00"};
static WarplsSafeNameVtableHD l_safeNameVtable;
static WarplsHeaderHD l_header;
static WarplsRegistrationTableHD l_registration;
static s16 m_bdlidx[] = {8, 3};
static s16 m_brkidx[] = {11, -1};
static s16 m_bckidx[] = {5, -1};
static u32 m_heapsize[] = {8192, 8192};
s32 Warpls_initBrk(mDoExt_brkAnm *, J3DModelData *, J3DAnmTevRegKey *, s32, s32, f32, s32, s32, s32, s32);
s32 Warpls_initBck(mDoExt_bckAnm *, J3DModelData *, J3DAnmTransform *, s32, s32, f32, s32, s32, s32);
void Warpls_subtract(cXyz *, cXyz *, cXyz *);
f32 Warpls_squareMagnitude(cXyz *);
f32 Warpls_sqrt(f32);
void Warpls_setCullBox(fopAc_ac_c *, f32, f32, f32, f32, f32, f32);
void Warpls_constructActor(fopAc_ac_c *);
s32 Warpls_resLoad(request_of_phase_process_class *, const char *);
void Warpls_resDelete(request_of_phase_process_class *, const char *);
s32 Warpls_startCheck(dEvent_manager_c *, s32);
s32 Warpls_endCheck(dEvent_manager_c *, s32);
void Warpls_setGoal(dEvent_manager_c *, cXyz *);
void dLib_setNextStageBySclsNum(u32, s32);
void Warpls_orderOtherEventId(fopAc_ac_c *, s32, u32, u32, u32, u32);
void Warpls_playAnimation(mDoExt_baseAnm *);
dScnKy_env_light_c *Warpls_getEnvLight();
void Warpls_settingTev(dScnKy_env_light_c *, s32, cXyz *, dKy_tevstr_c *);
void Warpls_setLight(dScnKy_env_light_c *, J3DModel *, dKy_tevstr_c *);
void Warpls_entryBrk(mDoExt_brkAnm *, J3DModelData *, f32);
void Warpls_entryBck(mDoExt_bckAnm *, J3DModelData *, f32);
void Warpls_updateModel(J3DModel *, u32);
// HD: sound forwards the complete reverb return register.
s32 Warpls_getReverb(s32);
void Warpls_startSound(u32, cXyz *, u32, s32);
u32 Warpls_isSwitch(dSv_info_c *, s32, s32);
mDoExt_brkAnm *Warpls_newBrk(mDoExt_brkAnm *);
void WarplsFrameCtrl_init(J3DFrameCtrl *, s32);
void WarplsTransform_construct(void *);
s32 Warpls_getEventIdx(dEvent_manager_c *, const char *, u32);
WarplsEmitterHD *Warpls_spawnParticle(dPa_control_c *, u32, u32, cXyz *, csXyz *, u32, u32, u32, s32, u32,
                                      u32, u32);
void Warpls_registerGlobal(WarplsRegistrationHD *);
void Warpls_initializeHeaderByte(be<u8> *);
void Warpls_initializeHeaderFlag(be<u8> *);
static void Warpls_sound(daWarpls_c *actor, u32 id) {
    s32 room = (s8)actor->current.roomNo;
    s32 reverb = Warpls_getReverb(room);
    Warpls_startSound(id, &actor->eyePos, 0, reverb);
}
static u32 Warpls_switchOn(daWarpls_c *actor) {
    dSv_info_c *save = dComIfGs_info();
    s32 room = (s8)actor->home.roomNo;
    s32 sw = actor->mSwitchNo;
    return Warpls_isSwitch(save, sw, room);
}
static void Warpls_emitterFlag(WarplsEmitterHD *emitter, u32 bit, bool on) {
    u32 flags = emitter->mFlags;
    emitter->mFlags = on ? (flags | bit) : (flags & ~bit);
}
// HD: scale stores have different orders in CreateInit and execute.
static void Warpls_matrix(daWarpls_c *actor, bool initialization) {
    f32 y = actor->scale.y, z = actor->scale.z, x = actor->scale.x;
    J3DModel *model = actor->mpModel;
    if (initialization) {
        model->mBaseScale.y = y;
        model->mBaseScale.z = z;
        model->mBaseScale.x = x;
    } else {
        model->mBaseScale.x = x;
        model->mBaseScale.z = z;
        model->mBaseScale.y = y;
    }
    f32 px = actor->current.pos.x, py = actor->current.pos.y, pz = actor->current.pos.z;
    mDoMtx_stack_c::transS(px, py, pz);
    J3DModel_setBaseTRMtx(actor->mpModel, mDoMtx_stack_c::get());
}

/* 024DBDAC */
static s32 CheckCreateHeap(daWarpls_c *actor) {
    return actor->CreateHeap();
}
/* 024DBB34 */
s32 daWarpls_c::CreateHeap() {
    u32 type = mWarpType;
    s32 idx = m_bdlidx[type];
    J3DModelData *data = (J3DModelData *)dComIfG_getObjectRes(m_arcname[type], idx);
    if (!data)
        JUT_ASSERT_fail("d_a_warpls.cpp", 0xE9, "modelData != (0)");
    mpModel = mDoExt_J3DModel__create(data, 0, 0x11020203);
    if (!(J3DModel *)mpModel)
        return 0;
    type = mWarpType;
    mpBrkAnm = nullptr;
    idx = m_brkidx[type];
    if (idx != -1) {
        J3DAnmTevRegKey *anim = (J3DAnmTevRegKey *)dComIfG_getObjectRes(m_arcname[type], idx);
        if (!anim)
            JUT_ASSERT_fail("d_a_warpls.cpp", 0xF8, "pbrk != (0)");
        mDoExt_brkAnm *object = Warpls_newBrk(nullptr);
        mpBrkAnm = object;
        if (!object)
            return 0;
        if (!Warpls_initBrk(object, data, anim, 1, 0, 1.0f, 0, -1, 0, 0))
            return 0;
    }
    type = mWarpType;
    mpBckAnm = nullptr;
    idx = m_bckidx[type];
    if (idx != -1) {
        J3DAnmTransform *anim = (J3DAnmTransform *)dComIfG_getObjectRes(m_arcname[type], idx);
        if (!anim)
            JUT_ASSERT_fail("d_a_warpls.cpp", 0x10B, "pbck != (0)");
        mDoExt_bckAnm *object = (mDoExt_bckAnm *)operator_new(0x8C);
        // HD: preserve the inherited constructor stores and vtable transitions.
        if (object) {
            WarplsBckLayoutHD *bck = (WarplsBckLayoutHD *)object;
            WarplsFrameCtrl_init(&bck->mFrameCtrl, 0);
            bck->mVtable = 0x1016E54C;
            WarplsTransform_construct(bck->mTransformMtx);
            bck->mAnm = 0;
            bck->mJoint = 0;
            bck->mMtxCalcVtable = 0x1016D820;
            bck->mFlags = 0;
            bck->mVtable = 0x100427D4;
            bck->mParent = 0;
            bck->mExtra = 0;
        }
        mpBckAnm = object;
        if (!object)
            return 0;
        if (!Warpls_initBck(object, data, anim, 1, 0, 1.0f, 0, -1, 0))
            return 0;
    }
    return 1;
}
/* 024DBDB0 */
s32 daWarpls_c::check_warp_distance() {
    fopAc_ac_c *player = dComIfGp_getLinkPlayer();
    fopAc_ac_c *player0 = dComIfGp_getPlayer(0);
    if (player != player0)
        return 0;
    gabi::Local<cXyz> delta, flat;
    Warpls_subtract(&player->current.pos, delta.get(), &current.pos);
    flat->x = delta->x;
    flat->y = 0.0f;
    flat->z = delta->z;
    f32 mag = Warpls_squareMagnitude(flat.get());
    f32 length = Warpls_sqrt(mag);
    f32 limit = 112.5f * (f32)scale.x;
    return length < limit;
}
/* 024DC660 */
s32 daWarpls_c::check_warp_link() {
    fopAc_ac_c *player = dComIfGp_getLinkPlayer();
    fopAc_ac_c *player0 = dComIfGp_getPlayer(0);
    if (player != player0 || !mWarpActive || mPlayerStartedInWarp)
        return 0;
    gabi::Local<cXyz> delta, flat;
    Warpls_subtract(&player->current.pos, delta.get(), &current.pos);
    flat->x = delta->x;
    flat->y = 0.0f;
    flat->z = delta->z;
    f32 mag = Warpls_squareMagnitude(flat.get());
    f32 length = Warpls_sqrt(mag);
    f32 limit = 112.5f * (f32)scale.x;
    return length < limit;
}
/* 024DBE68 */
void daWarpls_c::CreateInit() {
    gabi::Local<gptr<const char>[3]> events;
    (*events)[0] = Warpls_guestString("TOWER_WARP_U");
    (*events)[1] = Warpls_guestString("TOWER_WARP_D");
    (*events)[2] = Warpls_guestString("DUNGEON_WARP");
    gabi::Local<SafeName> siren, stage;
    siren->mText = Warpls_guestString("Siren");
    siren->mVtable = &l_safeNameVtable;
    stage->mText = dComIfGp_getStartStageName();
    stage->mVtable = &l_safeNameVtable;
    // HD: retain SafeString virtual calls and compare bytes through guest memory.
    Warpls_assureName(siren.get());
    Warpls_assureName(siren.get());
    const be<u8> *left = reinterpret_cast<const be<u8> *>(siren->mText.get());
    Warpls_assureName(stage.get());
    const be<u8> *right = reinterpret_cast<const be<u8> *>(stage->mText.get());
    bool equal = left == right;
    if (!equal) {
        equal = true;
        for (u32 n = 0; n < 0x40001; n++) {
            u8 a = (u8)left[n], b = (u8)right[n];
            if (a != b) {
                equal = false;
                break;
            }
            if (!a)
                break;
        }
    }
    if (equal) {
        s32 room = (s8)current.roomNo;
        if (room == 7)
            mEvtType = EvtType_TowerUp;
        else if (room == 17)
            mEvtType = EvtType_TowerDown;
    } else
        mEvtType = EvtType_Dungeon;
    u32 prm = mParameters;
    mSwitchNo = prm & 255;
    mSceneNo = (prm >> 8) & 255;
    u32 type = mWarpType;
    if (type == Type_Tower || type == Type_Dungeon) {
        if (type == Type_Tower) {
            scale.z = 2.0f;
            scale.x = 2.0f;
        }
        dPa_control_c *particles = dComIfGp_getParticle();
        WarplsEmitterHD *emitter = Warpls_spawnParticle(particles, 0, type == Type_Tower ? 0x810B : 0x8225,
                                                        &current.pos, &current.angle, 0, 255, 0, -1, 0, 0, 0);
        mpEmitter = emitter;
        if (type == Type_Tower && emitter) {
            Warpls_emitterFlag(emitter, 0x40, true);
            emitter = mpEmitter.get();
            f32 y = scale.y, x = scale.x, z = scale.z;
            emitter->mGlobalDynamicsScale.x = x;
            emitter->mGlobalDynamicsScale.y = y;
            emitter->mGlobalDynamicsScale.z = z;
        }
    }
    u32 sw = Warpls_switchOn(this);
    if (sw || mSwitchNo == 255) {
        WarplsEmitterHD *emitter = mpEmitter.get();
        if (emitter)
            Warpls_emitterFlag(emitter, 1, false);
        mDoExt_brkAnm *brk = mpBrkAnm.get();
        if (brk)
            brk->setFrame(brk->getEndFrame());
        mDoExt_bckAnm *bck = mpBckAnm.get();
        if (bck)
            bck->setFrame(bck->getEndFrame());
        mPrevSwitchState = 1;
        mWarpActive = 1;
        mSkipActivationEvt = 1;
        mStartupDelayTimer = 0;
    } else {
        WarplsEmitterHD *emitter = mpEmitter.get();
        if (emitter)
            Warpls_emitterFlag(emitter, 1, true);
        mWarpActive = 0;
        mStartupDelayTimer = 10;
    }
    prm = mParameters;
    u32 event = (prm >> 16) & 255;
    mEvtState = EvtState_None;
    mActivationEvtIdx = Warpls_getEventIdx(dComIfGp_getPEvtManager(), nullptr, event);
    const char *name = (*events)[(u32)mEvtType];
    mWarpingEvtIdx = Warpls_getEventIdx(dComIfGp_getPEvtManager(), name, 255);
    J3DModel *model = mpModel;
    cullMtx = gabi::ea(model->getBaseTRMtx());
    Warpls_setCullBox(this, -250.0f, 0.0f, -250.0f, 250.0f, 2600.0f, 250.0f);
    cullSizeFar = 1.0f;
    Warpls_matrix(this, true);
    if (check_warp_distance())
        mPlayerStartedInWarp = 1;
}
/* 024DC3B8 */
s32 daWarpls_c::_create() {
    u32 condition = actor_condition;
    if (!(condition & 8)) {
        if (this) {
            Warpls_constructActor(this);
            condition = actor_condition;
            __vtbl = 0x100427FC;
        }
        actor_condition = condition | 8;
    }
    u32 type = (u32)mParameters >> 28;
    mWarpType = type;
    if (type >= 2) {
        JUT_ASSERT_fail("d_a_warpls.cpp", 0x18B, "mType >= 0 && mType < 2");
        type = mWarpType;
    }
    const char *name = m_arcname[type];
    s32 phase = Warpls_resLoad(&mPhs, name);
    if (phase == 4) {
        type = mWarpType;
        u32 heap = m_heapsize[type];
        if (!fopAcM_entrySolidHeap(this, (BOOL (*)(fopAc_ac_c *))CheckCreateHeap, heap))
            return 5;
        CreateInit();
    }
    return phase;
}
/* 024DC4C0 */
bool daWarpls_c::_delete() {
    WarplsEmitterHD *emitter = mpEmitter.get();
    if (emitter) {
        u32 flags = emitter->mFlags;
        emitter->mMaxFrame = -1;
        emitter->mFlags = flags | 1;
        mpEmitter = nullptr;
    }
    const char *name = m_arcname[mWarpType];
    Warpls_resDelete(&mPhs, name);
    return true;
}
/* 024DC5C4 */
void daWarpls_c::warp_eff_start() {
    if (mWarpActive)
        return;
    mDoExt_brkAnm *brk = mpBrkAnm.get();
    if (brk)
        brk->setPlaySpeed(1.0f);
    mDoExt_bckAnm *bck = mpBckAnm.get();
    if (bck)
        bck->setPlaySpeed(1.0f);
    WarplsEmitterHD *emitter = mpEmitter.get();
    if (emitter)
        Warpls_emitterFlag(emitter, 1, false);
    Warpls_sound(this, WarpAppear);
    mWarpActive = 1;
}
/* 024DC730 */
bool daWarpls_c::demo() {
    u8 sw = Warpls_switchOn(this);
    if (mPlayerStartedInWarp) {
        if (!check_warp_distance())
            mPlayerStartedInWarp = 0;
        return true;
    }
    if (sw) {
        if (mEvtState == EvtState_None && !mSkipActivationEvt && !mWarpActive && mActivationEvtIdx != -1)
            mEvtState = EvtState_Activation;
    } else if (mSwitchNo != 255) {
        mDoExt_brkAnm *brk = mpBrkAnm.get();
        if (brk)
            brk->setPlaySpeed(-1.0f);
        mDoExt_bckAnm *bck = mpBckAnm.get();
        if (bck)
            bck->setPlaySpeed(-1.0f);
        WarplsEmitterHD *emitter = mpEmitter.get();
        if (emitter)
            Warpls_emitterFlag(emitter, 1, true);
        mWarpActive = 0;
        return true;
    }
    if (check_warp_link() && mEvtState == EvtState_None)
        mEvtState = EvtState_Warping;
    return true;
}
/* 024DC880 */
void daWarpls_c::checkOrder() {
    if (!eventInfo.checkCommandDemoAccrpt()) {
        demo();
        return;
    }
    s32 event = mActivationEvtIdx;
    if (Warpls_startCheck(dComIfGp_getPEvtManager(), event) && mEvtState == EvtState_Activation) {
        mEvtState = EvtState_None;
        warp_eff_start();
    }
    event = mActivationEvtIdx;
    if (Warpls_endCheck(dComIfGp_getPEvtManager(), event)) {
        dComIfGp_event_reset();
    }
    event = mWarpingEvtIdx;
    if (Warpls_startCheck(dComIfGp_getPEvtManager(), event) && mEvtState == EvtState_Warping) {
        mEvtState = EvtState_None;
        Warpls_setGoal(dComIfGp_getPEvtManager(), &current.pos);
    }
    event = mWarpingEvtIdx;
    if (Warpls_endCheck(
            dComIfGp_getPEvtManager(),
            event)) { // HD: stage selection consumes only the low byte of the stored scene number.
        s32 room = (s8)current.roomNo;
        u32 scene = (u8)mSceneNo;
        dLib_setNextStageBySclsNum(scene, room);
        Warpls_sound(this, WarpTravel);
    }
}
/* 024DC9D4 */
void daWarpls_c::eventOrder() {
    u8 sw = Warpls_switchOn(this);
    s32 state = mEvtState;
    if (state == EvtState_Activation) {
        u32 ev = ((u32)mParameters >> 16) & 255;
        s32 idx = mActivationEvtIdx;
        Warpls_orderOtherEventId(this, idx, ev, 65535, 0, 1);
    } else if (state == EvtState_Warping) {
        u32 type = mEvtType;
        if (type == EvtType_TowerUp) {
            if (!check_warp_distance())
                mEvtState = EvtState_None;
            else {
                s32 idx = mWarpingEvtIdx;
                Warpls_orderOtherEventId(this, idx, 255, 65535, 0, 5);
            }
        } else {
            s32 idx = mWarpingEvtIdx;
            Warpls_orderOtherEventId(this, idx, 255, 65535, 0, 1);
        }
    } else {
        if (mActivationEvtIdx == -1 && (mSwitchNo == 255 || sw))
            warp_eff_start();
        return;
    }
    eventInfo.onCondition(dEvtCnd_UNK2_e);
}
/* 024DCB70 */
bool daWarpls_c::setStatus() {
    if (mWarpActive)
        Warpls_sound(this, WarpSustain);
    return true;
}
/* 024DCBC8 */
bool daWarpls_c::_execute() {
    u8 sw = Warpls_switchOn(this);
    s32 timer = mStartupDelayTimer;
    if (timer > 0)
        mStartupDelayTimer = timer - 1;
    checkOrder();
    eventOrder();
    setStatus();
    mDoExt_brkAnm *brk = mpBrkAnm.get();
    if (brk)
        Warpls_playAnimation(brk);
    mDoExt_bckAnm *bck = mpBckAnm.get();
    if (bck)
        Warpls_playAnimation(bck);
    Warpls_matrix(this, false);
    mPrevSwitchState = sw;
    return true;
}
/* 024DC524 */
static bool daWarpls_Draw(daWarpls_c *actor) {
    if (!actor->mWarpActive)
        return true;
    dScnKy_env_light_c *env = Warpls_getEnvLight();
    Warpls_settingTev(env, 0, &actor->current.pos, &actor->tevStr);
    env = Warpls_getEnvLight();
    J3DModel *model = actor->mpModel;
    Warpls_setLight(env, model, &actor->tevStr);
    mDoExt_brkAnm *brk = actor->mpBrkAnm.get();
    if (brk) {
        model = actor->mpModel;
        f32 frame = brk->getFrame();
        J3DModelData *data = model->getModelData();
        Warpls_entryBrk(brk, data, frame);
    }
    mDoExt_bckAnm *bck = actor->mpBckAnm.get();
    if (bck) {
        model = actor->mpModel;
        f32 frame = bck->getFrame();
        J3DModelData *data = model->getModelData();
        Warpls_entryBck(bck, data, frame);
    }
    model = actor->mpModel;
    Warpls_updateModel(model, 0);
    return true;
}
/* 024DC4BC */
static s32 daWarpls_Create(daWarpls_c *actor) {
    return actor->_create();
}
/* 024DC520 */
static bool daWarpls_Delete(daWarpls_c *actor) {
    return actor->_delete();
}
/* 024DCD10 */
static bool daWarpls_Execute(daWarpls_c *actor) {
    return actor->_execute();
}
/* 024DCDBC */
static bool daWarpls_IsDelete(void *actor) {
    return true;
}
/* 024DCD14 */
static void __sinit_d_a_warpls_cpp() {
    // HD: global initialization writes these words in the native order.
    l_header.mZero[2] = 0;
    l_header.mZero[0] = 0;
    l_header.mZero[3] = 0;
    l_header.mZero[1] = 0;
    Warpls_registerGlobal(&l_registration.mRecords[0]);
    l_header.mPiMin = -3.1415927410125732f;
    l_header.mPiMax = 3.1415927410125732f;
    Warpls_initializeHeaderByte(&l_header.mAngleByte);
    Warpls_registerGlobal(&l_registration.mRecords[1]);
    Warpls_initializeHeaderFlag(&l_header.mAngleFlag);
    Warpls_registerGlobal(&l_registration.mRecords[2]);
}
/* 024DCDA8 */
static void Warpls_SafeStringDestructor(void *object, u32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}
/* 024DCDC4 */
static void daWarpls_deletingDestructor(daWarpls_c *actor, u32 flags) {
    if (actor) {
        __dt(actor);
        if (flags & 1)
            operator_delete(actor);
    }
}
/* 024DCE18 */
static void Warpls_SafeStringAssure(SafeName *object) {}

/* WWHD glue
data m_arcname 101D3224
data m_bdlidx 100428E0
data m_brkidx 100428E4
data m_bckidx 100428E8
data m_heapsize 100428D8
data l_safeNameVtable 100427BC
data l_header 1046EB90
data l_registration 101D3200
str 10042828 in 024DBB34 # "d_a_warpls.cpp"
str 10042838 "modelData != (0)"
str 10042810 "pbrk != (0)"
str 1004281C "pbck != (0)"
str 100428BC in 024DC3B8 # "d_a_warpls.cpp"
str 100428A4 "mType >= 0 && mType < 2"
str 1004286C "Siren"
str 10042874 "TOWER_WARP_U"
str 10042884 "TOWER_WARP_D"
str 10042894 "DUNGEON_WARP"
ext 025E8154 s32 Warpls_initBrk(mDoExt_brkAnm*, J3DModelData*, J3DAnmTevRegKey*, s32, s32, f32, s32, s32, s32, s32)
ext 025E8508 s32 Warpls_initBck(mDoExt_bckAnm*, J3DModelData*, J3DAnmTransform*, s32, s32, f32, s32, s32, s32)
ext 0201ADE0 void Warpls_subtract(cXyz*, cXyz*, cXyz*)
ext 028E8DD0 f32 Warpls_squareMagnitude(cXyz*)
ext 028F4384 f32 Warpls_sqrt(f32)
ext 025D674C void Warpls_setCullBox(fopAc_ac_c*, f32, f32, f32, f32, f32, f32)
ext 025D4ED0 void Warpls_constructActor(fopAc_ac_c*)
ext 02520460 s32 Warpls_resLoad(request_of_phase_process_class*, const char*)
ext 025204C8 void Warpls_resDelete(request_of_phase_process_class*, const char*)
ext 0254407C s32 Warpls_startCheck(dEvent_manager_c*, s32)
ext 025440C8 s32 Warpls_endCheck(dEvent_manager_c*, s32)
ext 02543714 void Warpls_setGoal(dEvent_manager_c*, cXyz*)
ext 02587EFC void dLib_setNextStageBySclsNum(u32, s32)
ext 025D7A58 void Warpls_orderOtherEventId(fopAc_ac_c*, s32, u32, u32, u32, u32)
ext 025E742C void Warpls_playAnimation(mDoExt_baseAnm*)
ext 02555D0C dScnKy_env_light_c* Warpls_getEnvLight()
ext 025626A4 void Warpls_settingTev(dScnKy_env_light_c*, s32, cXyz*, dKy_tevstr_c*)
ext 02562F5C void Warpls_setLight(dScnKy_env_light_c*, J3DModel*, dKy_tevstr_c*)
ext 025E83FC void Warpls_entryBrk(mDoExt_brkAnm*, J3DModelData*, f32)
ext 025E86B8 void Warpls_entryBck(mDoExt_bckAnm*, J3DModelData*, f32)
ext 025E2DE0 void Warpls_updateModel(J3DModel*, u32)
ext 02520540 s32 Warpls_getReverb(s32)
ext 025E1A40 void Warpls_startSound(u32, cXyz*, u32, s32)
ext 025BA0C0 u32 Warpls_isSwitch(dSv_info_c*, s32, s32)
ext 025E80D0 mDoExt_brkAnm* Warpls_newBrk(mDoExt_brkAnm*)
ext 027F2BC0 void WarplsFrameCtrl_init(J3DFrameCtrl*, s32)
ext 027DA984 void WarplsTransform_construct(void*)
ext 02543F10 s32 Warpls_getEventIdx(dEvent_manager_c*, const char*, u32)
ext 025A847C WarplsEmitterHD* Warpls_spawnParticle(dPa_control_c*, u32, u32, cXyz*, csXyz*, u32, u32, u32, s32, u32, u32, u32)
ext 028F026C void Warpls_registerGlobal(WarplsRegistrationHD*)
ext 028ED6F8 void Warpls_initializeHeaderByte(be<u8>*)
ext 028EAB2C void Warpls_initializeHeaderFlag(be<u8>*)
*/
