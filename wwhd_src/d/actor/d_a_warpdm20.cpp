/** Warp Demo 20: HD actor behavior. */
#include "bindings.h"
#include "d/actor/d_a_warpdm20.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"

struct Warpdm20ResourceKey {
    be<u32> text;
    be<u32> vtable;
};
WWHD_SIZE(Warpdm20ResourceKey, 8);
const gc_tu_data gc_tu = {.safeString = 0x100420DC, .headerStatics = {0x1046EAF4, 0x101D2DC4, 0x1046EB00}};
static warpdm20::EventTable event_init_tbl;
static warpdm20::EventTable event_action_tbl;
static warpdm20::ActionTable action_table;
static warpdm20::PiPair l_pi;
static cXyz l_zeroVector;

static void Warpdm20CopyTranslation(daWarpdm20_c*);
static BOOL CheckCreateHeap(daWarpdm20_c*);
/* 024D5034 */
BOOL daWarpdm20_c::_delete() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    warpRippleRemove(&mRippleCallback);
    warpResDelete(&mPhase, "Gdemo20");
    return TRUE;
}

/* 024D4958 */
BOOL daWarpdm20_c::CreateHeap() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<Warpdm20ResourceKey> modelKey, bckKey, btkKey, brkKey;
    modelKey->text = gabi::guest_addr("Gdemo20");
    modelKey->vtable = gc_tu.safeString;
    J3DModelData* modelData = static_cast<J3DModelData*>(
        warpObjectResource(dComIfG_resControl(), modelKey.get(), dRes_INDEX_GDEMO20_BDL_GDEMO20_1_e));
    if (!modelData)
        JUT_ASSERT_fail("d_a_warpdm20.cpp", 0xDB, "modelData != NULL");
    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000222);
    if (!mpModel)
        return FALSE;

    bckKey->text = gabi::guest_addr("Gdemo20");
    bckKey->vtable = gc_tu.safeString;
    mpBckAnm = nullptr;
    void* bckResource =
        warpObjectResource(dComIfG_resControl(), bckKey.get(), dRes_INDEX_GDEMO20_BCK_GDEMO20_1_e);
    if (!bckResource)
        JUT_ASSERT_fail("d_a_warpdm20.cpp", 0xED, "pbck != NULL");
    warpdm20::BckConstructView* bck = static_cast<warpdm20::BckConstructView*>(operator_new(0x8C));
    if (bck) {
        warpFrameConstruct(bck, 0);
        bck->mAnmVtable = warpdm20::BaseAnmVtable;
        warpMtxCalcConstruct(&bck->mMtxCalc);
        bck->m80 = 0;
        bck->m58 = 0;
        bck->mMtxCalcVtable = warpdm20::MtxCalcVtable;
        bck->m84 = 0;
        bck->mAnmVtable = warpdm20::BckAnmVtable;
        bck->m7C = 0;
        bck->m88 = 0;
    }
    mpBckAnm = bck;
    if (!bck)
        return FALSE;
    if (!warpInitBck(bck, modelData, bckResource, 1, 0, 1.0f, 0, -1, 0))
        return FALSE;
    mpBckAnm->mFrameCtrl.mRate = 0.0f;

    mpBtkAnm = nullptr;
    btkKey->text = gabi::guest_addr("Gdemo20");
    btkKey->vtable = gc_tu.safeString;
    void* btkResource =
        warpObjectResource(dComIfG_resControl(), btkKey.get(), dRes_INDEX_GDEMO20_BTK_GDEMO20_1_e);
    if (!btkResource)
        JUT_ASSERT_fail("d_a_warpdm20.cpp", 0x117, "pbtk != NULL");
    mpBtkAnm = warpCreateBtk(0);
    if (!mpBtkAnm)
        return FALSE;
    if (!warpInitBtk(mpBtkAnm.get(), modelData, btkResource, 1, 2, 1.0f, 0, -1, 0, 0))
        return FALSE;
    mpBtkAnm->mFrameCtrl.mRate = 0.0f;

    mpBrkAnm = nullptr;
    brkKey->text = gabi::guest_addr("Gdemo20");
    brkKey->vtable = gc_tu.safeString;
    void* brkResource =
        warpObjectResource(dComIfG_resControl(), brkKey.get(), dRes_INDEX_GDEMO20_BRK_GDEMO20_1_e);
    if (!brkResource)
        JUT_ASSERT_fail("d_a_warpdm20.cpp", 0x12B, "pbrk != NULL");
    mpBrkAnm = warpCreateBrk(0);
    if (!mpBrkAnm)
        return FALSE;
    if (!warpInitBrk(mpBrkAnm.get(), modelData, brkResource, 1, 0, 1.0f, 0, -1, 0, 0))
        return FALSE;
    mpBrkAnm->mFrameCtrl.mRate = 0.0f;
    return TRUE;
}

/* 024D4D10 */
void daWarpdm20_c::CreateInit() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    J3DModel* model = mpModel;
    cullMtx = warpdm20::cullMatrixAddress(model);
    warpCullBox(this, -300.0f, 0.0f, -300.0f, 300.0f, 500.0f, 300.0f);
    gabi::Local<cXyz> probe;
    probe->x = current.pos.x;
    probe->y = gabi::fadds_ppc(f32(current.pos.y), 2000.0f);
    probe->z = current.pos.z;
    cullSizeFar = 1.0f;
    f32 water = getSeaY(probe.get());
    const f32 z = scale.z, y = scale.y, x = scale.x;
    J3DModel* currentModel = mpModel;
    if (water == -1000000000.0f)
        water = 0.0f;
    current.pos.y = water;
    m2E0.y = water;
    warpdm20::modelScale(currentModel)->z = z;
    warpdm20::modelScale(currentModel)->x = x;
    warpdm20::modelScale(currentModel)->y = y;
    Warpdm20CopyTranslation(this);
    m2EC.z = 1.0f;
    warpdm20::words(&m2E0)->z = warpdm20::words(&current.pos)->z;
    warpdm20::words(&m2E0)->y = warpdm20::words(&current.pos)->y;
    m2EC.x = 1.0f;
    m2C8 = 0;
    warpdm20::words(&m2E0)->x = warpdm20::words(&current.pos)->x;
    m2EC.y = 1.0f;
    dSv_event_c* save = dComIfGs_getEvent();
    if (warpIsEventBit(save, UNK_2D08)) {
        m2CC = 1;
        this->setEndAnm();
    }
    dComIfG_inf_c* game = warpdm20::game();
    mEventIdx = warpEventIdx(&game->play.mEvtManager, "TO_HYRULE_WARP", 255);
    save = dComIfGs_getEvent();
    if (warpIsEventBit(save, ZELDA_AWAKENED)) {
        save = dComIfGs_getEvent();
        warpOnEventBit(save, UNK_1820);
        // HD: awakening additionally notifies the game-data system.
        warpHdAwakened(0xB203);
    }
}

/* 024D4F58 */
s32 daWarpdm20_c::_create() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    auto* condition = &actor_condition;
    if (!(u32(*condition) & fopAcCnd_INIT_e)) {
        if (this) {
            warpActorConstruct(this);
            __vtbl = warpdm20::ActorVtable;
            warpRippleConstruct(&mRippleCallback);
        }
        *condition = u32(*condition) | fopAcCnd_INIT_e;
    }
    m2D6 = u32(mParameters) & 15;
    const s32 phase = warpResLoad(&mPhase, "Gdemo20");
    if (phase == cPhs_COMPLEATE_e) {
        if (!warpEntryHeap(this, gabi::guest_fn((const void*)&CheckCreateHeap), 0x3000))
            return cPhs_ERROR_e;
        CreateInit();
    }
    return phase;
}

/* 024D58FC */
BOOL daWarpdm20_c::_execute() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!u8(demoActorID)) {
        set_effect(0x83D8);
        this->checkOrder();
        this->demo_proc();
        this->eventOrder();
    } else
        this->demo_execute();
    if (u8(m2CC)) {
        const s32 reverb = warpReverb(s32(current.roomNo));
        warpLevelSound(JA_SE_OBJ_HL_WAPR_EFF, &eyePos, 0, reverb);
    }
    gabi::Local<cXyz> probe;
    probe->x = current.pos.x;
    probe->y = gabi::fadds_ppc(f32(current.pos.y), 2000.0f);
    probe->z = current.pos.z;
    f32 water = getSeaY(probe.get());
    const f32 scaleY = scale.y;
    if (water == -1000000000.0f)
        water = 0.0f;
    m2E0.y = water;
    const f32 scaleX = scale.x, scaleZ = scale.z;
    J3DModel* model = mpModel;
    current.pos.y = water;
    warpdm20::modelScale(model)->x = scaleX;
    warpdm20::modelScale(model)->y = scaleY;
    warpdm20::modelScale(model)->z = scaleZ;
    Warpdm20CopyTranslation(this);
    return TRUE;
}

/* 024D5354 */
void daWarpdm20_c::normal_execute() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    warpdm20::Emitter* emitter = mRippleCallback.mpEmitter;
    dComIfG_inf_c* game = warpdm20::game();
    fopAc_ac_c* player = game->play.mPlayerInfo[0].mpPlayer;
    gabi::Local<cXyz> delta, horizontal;
    cXyz_mi(warpdm20::actorPosition(player), delta.get(), &current.pos);
    horizontal->x = delta->x;
    horizontal->y = 0.0f;
    horizontal->z = delta->z;
    PSVECSquareMag(horizontal.get());
    // HD: sqrt consumes the preceding square-magnitude result left in f1.
    const f32 distance = warpSqrt();
    dSv_event_c* save = dComIfGs_getEvent();
    if (!warpIsEventBit(save, UNK_2D08))
        return;
    m2CC = 1;
    this->animPlay();
    if (emitter) {
        auto* flags = &emitter->mStatus;
        if (distance < 2000.0f)
            *flags = u32(*flags) & ~1u;
        else
            *flags = u32(*flags) | 1;
    }
    if (this->check_warp())
        m2C8 = 1;
}

/* 024D57E4 */
void daWarpdm20_c::demo_execute() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    const u32 demoId = demoActorID;
    m2CC = 1;
    if (!demoId || demoId > 32)
        return;
    warpdm20::DemoObjects* objects = warpdm20::demoObjects();
    if (!objects) {
        JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != NULL");
        objects = warpdm20::demoObjects();
    }
    dDemo_actor_c* demo = warpDemoActor(objects, demoId);
    if (!demo)
        return;
    const u32 action = demo->mShapeId;
    m2D8 = action;
    if (action == 0) {
        warpOffDraw(warpdm20::drawTag(this));
    } else if (action == 1) {
        set_effect(0x82BD);
        warpdm20::Emitter* emitter = mRippleCallback.mpEmitter;
        if (emitter) {
            auto* flags = &emitter->mStatus;
            *flags = u32(*flags) & ~1u;
        }
        this->set_wpsenko();
        const u32 priority = warpDrawPriority(this);
        warpOnDraw(warpdm20::drawTag(this), priority);
        this->animPlay();
    }
}

// HD: translation entries are copied as words, preserving SNaN bits.
/* inlined into daWarpdm20_c::_execute, daWarpdm20_c::CreateInit */
static void Warpdm20CopyTranslation(daWarpdm20_c* actor) {
    PSMTXTrans(mDoMtx_stack_c::get(), f32(actor->current.pos.x), f32(actor->current.pos.y),
               f32(actor->current.pos.z));
    u32 values[12];
    for (u32 i = 0; i < 12; i++)
        values[i] = warpdm20::matrixWords(mDoMtx_stack_c::get())[i];
    J3DModel* model = actor->mpModel;
    for (u32 i = 0; i < 12; i++)
        warpdm20::matrixWords(warpdm20::modelMatrix(model))[i] = values[i];
}

/* 024D5544 */
void daWarpdm20_c::demo_proc() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfG_inf_c* game = warpdm20::game();
    mStaffIdx = warpStaffId(&game->play.mEvtManager, "Gdemo20", 0, 0);
    game = warpdm20::game();
    if ((u8)game->play.mEvtCtrl.mMode == 0)
        return;
    if ((u16)eventInfo.mCommand == dEvtCmd_INTALK_e)
        return;
    s32 staff = mStaffIdx;
    if (staff == -1)
        return;
    game = warpdm20::game();
    const s32 action = warpActionIdx(&game->play.mEvtManager, staff, &action_table, 6, 0, 0);
    staff = mStaffIdx;
    if (action == -1) {
        game = warpdm20::game();
        warpCutEnd(&game->play.mEvtManager, staff);
        return;
    }
    game = warpdm20::game();
    if (warpAdvance(&game->play.mEvtManager, staff))
        (this->*warpdm20::eventAt(&event_init_tbl, u32(action)))(s32(mStaffIdx));
    if ((this->*warpdm20::eventAt(&event_action_tbl, u32(action)))(s32(mStaffIdx))) {
        staff = mStaffIdx;
        game = warpdm20::game();
        warpCutEnd(&game->play.mEvtManager, staff);
    }
}

/* 024D5A98 */
void daWarpdm20_c::initWait(s32 action) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mRippleCallback.mpEmitter) {
        auto* flags = &mRippleCallback.mpEmitter->mStatus;
        *flags = u32(*flags) & ~1u;
    }
}

/* 024D5AB4 */
BOOL daWarpdm20_c::actWait(s32 action) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    animPlay();
    return TRUE;
}

/* 024D5E84 */
void daWarpdm20_c::initWait2(s32 action) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfG_inf_c* game = warpdm20::game();
    warpSetGoal(&game->play.mEvtManager, &current.pos);
}

/* 024D5EBC */
BOOL daWarpdm20_c::actWait2(s32 action) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    animPlay();
    return TRUE;
}

/* 024D5AD8 */
void daWarpdm20_c::initWarp(s32 action) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    warpSystemSound(JA_SE_LK_HL_WAPR_D_IN);
    dComIfG_inf_c* game = warpdm20::game();
    warpSetGoal(&game->play.mEvtManager, &current.pos);
}

/* 024D5B18 */
BOOL daWarpdm20_c::actWarp(s32 action) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    animPlay();
    return TRUE;
}

/* 024D5D28 */
void daWarpdm20_c::initWaitDead(s32 action) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    warpdm20::Emitter* emitter = mRippleCallback.mpEmitter;
    if (emitter) {
        auto* flags = &emitter->mStatus;
        *flags = u32(*flags) & ~1u;
    }
    gabi::Local<cXyz> goal;
    goal->x = current.pos.x;
    goal->y = current.pos.y;
    goal->z = f32(current.pos.z) + 1000.0f;
    dComIfG_inf_c* game = warpdm20::game();
    warpSetGoal(&game->play.mEvtManager, goal.get());
    m2CC = 1;
    init_bck_anm(dRes_INDEX_GDEMO20_BCK_GDEMO20_1A_e);
    if (mpBrkAnm) {
        auto* animation = mpBrkAnm.get();
        animation->mFrameCtrl.mFrame = f32(animation->mFrameCtrl.mEnd);
    }
    if (mpBckAnm)
        mpBckAnm->mFrameCtrl.mRate = 0.0f;
    warpSystemSound(JA_SE_LK_HL_WAPR_U_OUT);
}

/* 024D5E08 */
BOOL daWarpdm20_c::actWaitDead(s32 action) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mpBtkAnm) {
        mpBtkAnm->mFrameCtrl.mRate = 1.0f;
        mpBtkAnm.get()->play();
    }
    if (mpBrkAnm) {
        mpBrkAnm->mFrameCtrl.mRate = 1.0f;
        mpBrkAnm.get()->play();
    }
    return TRUE;
}

/* 024D5B3C */
void daWarpdm20_c::initDead(s32 action) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    const s32 reverb = warpReverb(s32(current.roomNo));
    warpLevelSound(JA_SE_OBJ_HL_WARP_OFF, &eyePos, 0, reverb);
}

/* 024D5B84 */
BOOL daWarpdm20_c::actDead(s32 action) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    BOOL done = FALSE;
    if (mpBtkAnm) {
        mpBtkAnm->mFrameCtrl.mRate = 1.0f;
        mpBtkAnm.get()->play();
    }
    if (mpBrkAnm) {
        mpBrkAnm->mFrameCtrl.mRate = 1.0f;
        mpBrkAnm.get()->play();
    }
    if (mpBckAnm) {
        mpBckAnm->mFrameCtrl.mRate = 1.0f;
        if (mpBckAnm.get()->play()) {
            m2CC = 0;
            done = TRUE;
        }
    }
    warpdm20::Emitter* emitter = mRippleCallback.mpEmitter;
    if (emitter) {
        warpChasePosition(&m2EC, &l_zeroVector, 0.2f);
        const f32 x = m2EC.x, y = m2EC.y, z = m2EC.z;
        emitter->mGlobalParticleScale.z = z;
        emitter->mGlobalParticleScale.x = x;
        emitter->mGlobalDynamicsScale.y = y;
        emitter->mGlobalDynamicsScale.x = x;
        emitter->mGlobalDynamicsScale.z = z;
        emitter->mGlobalParticleScale.y = y;
    }
    return done;
}

/* 024D5EE0 */
u32 daWarpdm20_c::initReturnWait(s32 action) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mRippleCallback.mpEmitter) {
        auto* flags = &mRippleCallback.mpEmitter->mStatus;
        *flags = u32(*flags) & ~1u;
    }
    return warpSystemSound(JA_SE_LK_HL_WAPR_U_OUT);
}

/* 024D5F00 */
BOOL daWarpdm20_c::actReturnWait(s32 action) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    animPlay();
    return TRUE;
}

/* 024D5708 */
void daWarpdm20_c::eventOrder() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (s32(m2C8) != 1)
        return;
    warpOrderEvent(this, s32(mEventIdx), 255, 65535, 0, 1);
    auto* condition = &eventInfo.mCondition;
    *condition = u16(*condition) | dEvtCnd_UNK2_e;
}

/* 024D5468 */
void daWarpdm20_c::checkOrder() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if ((u16)eventInfo.mCommand == dEvtCmd_INDEMO_e) {
        s32 event = mEventIdx;
        dComIfG_inf_c* game = warpdm20::game();
        const s32 started = warpEventStart(&game->play.mEvtManager, event);
        event = mEventIdx;
        if (started && s32(m2C8))
            m2C8 = 0;
        game = warpdm20::game();
        if (warpEventEnd(&game->play.mEvtManager, event)) {
            game = warpdm20::game();
            auto* flags = &game->play.mEvtCtrl.mEventFlag;
            *flags = u16(*flags) | 8;
            warpDeleteActor(this);
        }
    } else if (s32(m2C8) == 0) {
        dComIfG_inf_c* game = warpdm20::game();
        if ((u8)game->play.mEvtCtrl.mMode == 0)
            normal_execute();
    }
}

/* 024D5208 */
void daWarpdm20_c::animPlay() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mpBtkAnm) {
        mpBtkAnm.get()->mFrameCtrl.mRate = 1.0f;
        mpBtkAnm.get()->play();
    }
    if (mpBrkAnm) {
        mpBrkAnm.get()->mFrameCtrl.mRate = 1.0f;
        mpBrkAnm.get()->play();
    }
    if (mpBckAnm) {
        mpBckAnm.get()->mFrameCtrl.mRate = 1.0f;
        mpBckAnm.get()->play();
    }
}

/* 024D4C94 */
void daWarpdm20_c::setEndAnm() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mpBckAnm) {
        auto* animation = mpBckAnm.get();
        animation->mFrameCtrl.mFrame = f32(animation->mFrameCtrl.mEnd);
    }
    if (mpBrkAnm) {
        auto* animation = mpBrkAnm.get();
        animation->mFrameCtrl.mFrame = f32(animation->mFrameCtrl.mEnd);
    }
}

/* 024D4C40 */
// HD: the verifier sees a pointer to the caller-owned position slot.
f32 daWarpdm20_c::getSeaY(cXyz* position) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (warpSeaArea(f32(position->x), f32(position->z)))
        return warpSeaWave(f32(position->x), f32(position->z));
    return warpWaterHeight(position);
}

/* 024D513C */
// HD: the particle ID remains a full register at this entry.
BOOL daWarpdm20_c::set_effect(u32 effect) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mRippleCallback.mpEmitter)
        return FALSE;
    dComIfG_inf_c* game = warpdm20::game();
    dPa_control_c* particles = game->play.mParticle;
    warpParticle(particles, 5, effect, &m2E0, 0, &m2EC, 255, &mRippleCallback, -1, 0, 0, 0);
    warpdm20::Emitter* emitter = mRippleCallback.mpEmitter;
    auto* flags = &mRippleCallback.mStatus;
    mRippleCallback.mRate = 7.0f;
    *flags = u32(*flags) & ~1u;
    if (emitter) {
        auto* emitterFlags = &emitter->mStatus;
        *emitterFlags = u32(*emitterFlags) | 1;
    }
    return TRUE;
}

/* 024D5768 */
void daWarpdm20_c::set_wpsenko() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (u8(m2F8))
        return;
    dComIfG_inf_c* game = warpdm20::game();
    dPa_control_c* particles = game->play.mParticle;
    warpParticle(particles, 0, 0x82BC, &current.pos, 0, 0, 255, 0, -1, 0, 0, 0);
    m2F8 = 1;
}

/* 024D5C88 */
void daWarpdm20_c::init_bck_anm(s32 index) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<Warpdm20ResourceKey> modelKey, animationKey;
    modelKey->text = gabi::guest_addr("Gdemo20");
    modelKey->vtable = gc_tu.safeString;
    dRes_control_c* control = dComIfG_resControl();
    J3DModelData* model = static_cast<J3DModelData*>(
        warpObjectResource(control, modelKey.get(), dRes_INDEX_GDEMO20_BDL_GDEMO20_1_e));
    animationKey->text = gabi::guest_addr("Gdemo20");
    animationKey->vtable = gc_tu.safeString;
    dRes_control_c* currentControl = dComIfG_resControl();
    void* animation = warpObjectResource(currentControl, animationKey.get(), index);
    warpInitBck(mpBckAnm.get(), model, animation, 1, 0, 1.0f, 0, -1, 1);
}

/* 024D5298 */
BOOL daWarpdm20_c::check_warp() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    warpdm20::game(); // HD: retain the extra play-object getter call.
    dComIfG_inf_c* game = warpdm20::game();
    fopAc_ac_c* ship = game->play.mpPlayerPtr[2];
    game = warpdm20::game();
    if (!((u32)game->play.mPlayerStatus[0][0] & 0x10000) || !ship)
        return FALSE;
    gabi::Local<cXyz> delta, horizontal;
    cXyz_mi(warpdm20::actorPosition(ship), delta.get(), &current.pos);
    horizontal->x = delta->x;
    horizontal->y = 0.0f;
    horizontal->z = delta->z;
    PSVECSquareMag(horizontal.get());
    // HD: sqrt consumes the preceding square-magnitude result left in f1.
    const f32 distance = warpSqrt();
    return distance < 500.0f;
}

/* 024D507C */
BOOL daWarpdm20_c::_draw() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!u8(m2CC))
        return TRUE;
    dScnKy_env_light_c* environment = dKy_getEnvlight();
    settingTevStruct(environment, 0, &current.pos, &tevStr);
    environment = dKy_getEnvlight();
    setLightTevColorType(environment, mpModel.get(), &tevStr);
    if (mpBtkAnm) {
        auto* animation = mpBtkAnm.get();
        J3DModelData* data = warpdm20::modelData(mpModel.get());
        warpEntryBtk(animation, data, f32(animation->mFrameCtrl.mFrame));
    }
    if (mpBrkAnm) {
        auto* animation = mpBrkAnm.get();
        J3DModelData* data = warpdm20::modelData(mpModel.get());
        warpEntryBrk(animation, data, f32(animation->mFrameCtrl.mFrame));
    }
    if (mpBckAnm) {
        auto* animation = mpBckAnm.get();
        J3DModelData* data = warpdm20::modelData(mpModel.get());
        warpEntryBck(animation, data, f32(animation->mFrameCtrl.mFrame));
    }
    mDoExt_modelUpdateDL(mpModel.get(), 0);
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.
/* 024D4C3C */
static BOOL CheckCreateHeap(daWarpdm20_c* actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return actor->CreateHeap();
}

/* 024D5030 */
static s32 daWarpdm20_Create(daWarpdm20_c* actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return actor->_create();
}

/* 024D5078 */
static BOOL daWarpdm20_Delete(daWarpdm20_c* actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return actor->_delete();
}

/* 024D5138 */
static BOOL daWarpdm20_Draw(daWarpdm20_c* actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return actor->_draw();
}

/* 024D5A94 */
static BOOL daWarpdm20_Execute(daWarpdm20_c* actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return actor->_execute();
}

/* 024D5F24 */
static void Warpdm20StaticInit() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    sinit_header_statics_rodata(gc_tu.headerStatics[0], gc_tu.headerStatics[1], gc_tu.headerStatics[2],
                                gabi::ea(&l_pi));
}

/* 024D5FB8 */
static void Warpdm20SafeStringDtor(void* key, u32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (key && (flags & 1))
        operator_delete(key);
}

/* 024D5FCC */
static BOOL daWarpdm20_IsDelete(daWarpdm20_c* actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return TRUE;
}

/* 024D5FD4 */
static void Warpdm20Dtor(daWarpdm20_c* actor, u32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (actor) {
        warpActorDestroy(actor, 0);
        if (flags & 1)
            operator_delete(actor);
    }
}

/* 024D6028 */
static void Warpdm20SafeStringEmpty(void* key, void* context) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
data event_init_tbl 101D2D2C
data event_action_tbl 101D2D5C
data action_table 101D2DAC
data l_pi 10042218
data l_zeroVector 101FFBA8
str 10042220 "Gdemo20"
str 100421BC "TO_HYRULE_WARP"
str 100421D0 "Gdemo20" in 024D5544
str 1004213C "d_demo.h"
str 1004212C "m_object != NULL"
str 10042174 "d_a_warpdm20.cpp"
str 10042188 "modelData != NULL"
str 10042150 "pbck != NULL"
str 1004215C "pbtk != NULL"
str 10042168 "pbrk != NULL"
ext 0246B6A4 s32 warpSeaArea(f32, f32)
ext 0246BA0C f32 warpSeaWave(f32, f32)
ext 024F1478 f32 warpWaterHeight(const cXyz*)
ext 025A9270 void warpRippleRemove(warpdm20::RippleCallback*)
ext 025204C8 void warpResDelete(request_of_phase_process_class*, const char*)
ext 02520460 s32 warpResLoad(request_of_phase_process_class*, const char*)
ext 025D4ED0 void warpActorConstruct(fopAc_ac_c*)
ext 025A9084 void warpRippleConstruct(warpdm20::RippleCallback*)
ext 025D63E8 s32 warpEntryHeap(fopAc_ac_c*, u32, u32)
ext 025D7A58 void warpOrderEvent(fopAc_ac_c*, s32, u32, u32, u32, u32)
ext 028F4384 f32 warpSqrt()
ext 0254407C s32 warpEventStart(dEvent_manager_c*, s32)
ext 025440C8 s32 warpEventEnd(dEvent_manager_c*, s32)
ext 025D57E0 void warpDeleteActor(fopAc_ac_c*)
ext 025A847C void warpParticle(dPa_control_c*, u32, u32, const cXyz*, const csXyz*, const cXyz*, u32, warpdm20::RippleCallback*, s32, const GXColor*, const GXColor*, const cXyz*)
ext 026066C4 void* warpObjectResource(dRes_control_c*, Warpdm20ResourceKey*, s32)
ext 025E8508 s32 warpInitBck(mDoExt_baseAnm*, J3DModelData*, void*, u32, s32, f32, s32, s32, u32)
ext 02543714 void warpSetGoal(dEvent_manager_c*, const cXyz*)
ext 025E1988 u32 warpSystemSound(u32)
ext 025B8B94 s32 warpIsEventBit(dSv_event_c*, u32)
ext 025B8B68 void warpOnEventBit(dSv_event_c*, u32)
ext 02586E94 void warpHdAwakened(u32)
ext 02526E70 dDemo_actor_c* warpDemoActor(warpdm20::DemoObjects*, u32)
ext 025DA884 void warpOffDraw(void*)
ext 025DF2B8 u32 warpDrawPriority(fopAc_ac_c*)
ext 025DA874 void warpOnDraw(void*, u32)
ext 02520540 s32 warpReverb(s32)
ext 025E1A40 void warpLevelSound(u32, const cXyz*, u32, s32)
ext 025D674C void warpCullBox(fopAc_ac_c*, f32, f32, f32, f32, f32, f32)
ext 02543F10 s32 warpEventIdx(dEvent_manager_c*, const char*, u32)
ext 02542D88 s32 warpStaffId(dEvent_manager_c*, const char*, u32, u32)
ext 02542EDC s32 warpActionIdx(dEvent_manager_c*, s32, warpdm20::ActionTable*, s32, u32, u32)
ext 02543280 void warpCutEnd(dEvent_manager_c*, s32)
ext 025447C8 s32 warpAdvance(dEvent_manager_c*, s32)
ext 025E7C6C mDoExt_baseAnm* warpCreateBtk(void*)
ext 025E80D0 mDoExt_baseAnm* warpCreateBrk(void*)
ext 025E7CE0 s32 warpInitBtk(mDoExt_baseAnm*, J3DModelData*, void*, u32, s32, f32, s32, s32, u32, s32)
ext 025E8154 s32 warpInitBrk(mDoExt_baseAnm*, J3DModelData*, void*, u32, s32, f32, s32, s32, u32, s32)
ext 027F2BC0 void warpFrameConstruct(mDoExt_baseAnm*, u32)
ext 027DA984 void warpMtxCalcConstruct(warpdm20::MtxCalc*)
ext 025D50BC void warpActorDestroy(fopAc_ac_c*, u32)
ext 025E7FC4 void warpEntryBtk(mDoExt_baseAnm*, J3DModelData*, f32)
ext 025E83FC void warpEntryBrk(mDoExt_baseAnm*, J3DModelData*, f32)
ext 025E86B8 void warpEntryBck(mDoExt_baseAnm*, J3DModelData*, f32)
ext 0200F62C void warpChasePosition(cXyz*, const cXyz*, f32)
*/
// clang-format on
