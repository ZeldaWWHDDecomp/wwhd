/**
 * d_a_warphr.cpp (WWHD)
 * Warphr - warp portal to Hyrule (Ghrwp, daWarphr_c)
 *
 * Written from the WWHD code with the GameCube decompilation (zeldaret/tww src/d/actor/d_a_warphr.cpp) as
 * reference, verified against cking.rpx.
 */
#include "d/actor/d_a_warphr.h"
namespace {
static s32 CheckCreateHeap(daWarphr_c* actor);
static void WarphrStaticInit();
static void WarphrSafeStringDtor(u8* object, s32 flags);
static bool daWarphr_IsDelete(daWarphr_c* actor);
static s32 daWarphr_Create(daWarphr_c* actor);
static bool daWarphr_Delete(daWarphr_c* actor);
static bool daWarphr_Draw(daWarphr_c* actor);
static bool daWarphr_Execute(daWarphr_c* actor);
} // namespace

u32 warphrPlay();
u32 warphrParticle(u8*, u32, u32, const cXyz*, u32, u32, u32, u32, s32, u32, u32, u32);
void* warphrResource(u8*, SafeName*, s32);
u32 warphrBtkConstruct(u32);
s32 warphrInitBtk(u8*, J3DModelData*, u8*, u32, s32, f32, s32, s32, u32, s32);
u32 warphrBrkConstruct(u32);
s32 warphrInitBrk(u8*, J3DModelData*, u8*, u32, s32, f32, s32, s32, u32, s32);
u32 warphrEventBit(u8*, u32);
s32 warphrEventIndex(u8*, const char*, u32);
void warphrActorConstruct(daWarphr_c*);
s32 warphrResLoad(request_of_phase_process_class*, const char*);
s32 warphrEntryHeap(daWarphr_c*, u32, u32);
void warphrResDelete(request_of_phase_process_class*, const char*);
u32 warphrStageBoss(u32);
f32 warphrSquareMagnitude(const cXyz*);
f32 warphrSqrt(f32);
void warphrAnimationPlay(u8*);
void warphrOnEventBit(u8*, u32);
u32 warphrStartCheck(u8*, s32);
u32 warphrEndCheck(u8*, s32);
void warphrOrderEvent(daWarphr_c*, s32, u32, u32, u32, u32);
u32 warphrDemoActor(u8*, u32);
void warphrSystemSound(u32);
void warphrSetGoal(u8*, const cXyz*);
void warphrRegisterObject(u8*);
void warphrRandomInit(u8*);
void warphrRandomInit2(u8*);
void warphrDelete(void*);
s32 warphrStaffId(u8*, const char*, u32, u32);
s32 warphrActionIndex(u8*, s32, const void*, s32, u32, u32);
void warphrCutEnd(u8*, s32);
u32 warphrAdvance(u8*, s32);
u32 warphrDemoCamera(u8*);
s16 warphrAngleY(const cXyz*, const cXyz*);
void warphrMatrixTranslate(Mtx34*, f32, f32, f32);
void warphrMatrixRotateY(Mtx34*, s16);
void warphrMatrixCopy(Mtx34*, Mtx34*);
void warphrMatrixDecompose(Mtx34*, u8*, u8*);
u32 warphrEnvironment();
void warphrTev(u8*, s32, const cXyz*, dKy_tevstr_c*);
void warphrApplyTev(u8*, J3DModel*, dKy_tevstr_c*);
void warphrEntryBtk(u8*, u8*, f32);
void warphrEntryBrk(u8*, u8*, f32);
void warphrModelUpdate(J3DModel*, u32);
u32 warphrStartCheckName(u8*, const char*);
void warphrMonotoneOn();
void warphrMonotoneOff();
s32 warphrReverb(s32);
void warphrLevelSound(u32, const cXyz*, u32, s32);
const gc_tu_data gc_tu = {.safeString = 0x10042648, .headerStatics = {0x1046EB64, 0x101D318C, 0x1046EB70}};
static warphr::InitTable event_init_tbl;
static warphr::ActionTable event_action_tbl;
static const char* action_table[5] = {"WAIT", "WARP", "WARP_ARRIVE", "WARP_ARRIVE_END", "START_WARP"};
static Mtx34 l_matrix;
static warphr::CurrentDrawLists l_currentDrawLists;
static mDoGph_StateHD l_graphics;
static cXyz arrive_target;
static u32 l_arriveTargetGuard;
static u32 l_demoObjects;
static u32 l_demoFrame;

namespace {
static u32 play() {
    return warphrPlay();
}
using warphr::ptr;
static u32 eventmanager() {
    return gabi::ea(&warphr::game(play())->play.mEvtManager);
}
static u32 saveevent() {
    return gabi::ea(dComIfGs_getEvent());
}
// HD: invalidate in place, retaining the original maximum-frame and flag stores.
static void invalidate(u32 emitter) {
    u32 flags = u32(warphr::emitter(emitter)->mFlags);
    warphr::emitter(emitter)->mMaxFrame = -1;
    warphr::emitter(emitter)->mFlags = flags | 1;
}
/* inlined into CreateInit and _execute */
static void set_mtx(daWarphr_c* actor) {
    f32 sx = actor->scale.x, sy = actor->scale.y, sz = actor->scale.z;
    u32 model = gabi::ea((J3DModel*)actor->mpModel1);
    warphr::modelScale(model)->y = sy;
    warphr::modelScale(model)->x = sx;
    warphr::modelScale(model)->z = sz;
    f32 x = actor->current.pos.x, y = actor->current.pos.y, z = actor->current.pos.z;
    auto* stack = &l_matrix;
    warphrMatrixTranslate(stack, x, y, z);
    J3DModel_setBaseTRMtx(actor->mpModel1, stack);
    sx = actor->scale.x;
    sy = actor->scale.y;
    sz = actor->scale.z;
    model = gabi::ea((J3DModel*)actor->mpModel2);
    warphr::modelScale(model)->y = sy;
    warphr::modelScale(model)->z = sz;
    warphr::modelScale(model)->x = sx;
    J3DModel_setBaseTRMtx(actor->mpModel2, stack);
}
// HD: the full particle identifier and nullable callback registers remain explicit.
static u32 particle(daWarphr_c* actor, u32 id, u32 group = 0) {
    u32 control = gabi::ea(warphr::game(play())->play.mParticle.get());
    return warphrParticle(ptr(control), group, id, &actor->current.pos, 0, 0, 255, 0, -1, 0, 0, 0);
}

} // namespace

namespace {} // namespace

namespace {} // namespace

/* 024DA99C */
bool daWarphr_c::_delete() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 e = gabi::ea(m2AC.get());
    if (e) {
        invalidate(e);
        m2AC = 0;
    }
    e = gabi::ea(m2B0.get());
    if (e) {
        invalidate(e);
        m2B0 = 0;
    }
    warphrResDelete(&mPhase, "Ghrwp");
    return true;
}

namespace {
/* 024DA6D8 */
static s32 CheckCreateHeap(daWarphr_c* actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return actor->CreateHeap();
}
} // namespace

/* 024DA3FC */
s32 daWarphr_c::CreateHeap() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<SafeName> modelA, textureA, modelB, textureB, colors;
    modelA->text = gabi::guest_addr("Ghrwp");
    modelA->vt = gc_tu.safeString;
    u32 resource = gabi::ea(dComIfG_resControl());
    auto* data = static_cast<J3DModelData*>(
        warphrResource(ptr(resource), modelA.get(), dRes_INDEX_GHRWP_BDL_GHRWPA00_e));
    if (!data)
        JUT_ASSERT_fail("d_a_warphr.cpp", 0xDD, "modelData != NULL");
    mpModel1 = mDoExt_J3DModel__create(data, 0x80000, 0x11000222);
    if (!(J3DModel*)mpModel1)
        return 0;
    textureA->vt = gc_tu.safeString;
    mpBtkAnm1 = 0;
    resource = gabi::ea(dComIfG_resControl());
    textureA->text = gabi::guest_addr("Ghrwp");
    auto* tex =
        static_cast<u8*>(warphrResource(ptr(resource), textureA.get(), dRes_INDEX_GHRWP_BTK_GHRWPA00_e));
    if (!tex)
        JUT_ASSERT_fail("d_a_warphr.cpp", 0xEF, "pbtk != NULL");
    u32 obj = warphrBtkConstruct(0);
    mpBtkAnm1 = warphr::btkAt(obj);
    if (!obj)
        return 0;
    if (!warphrInitBtk(ptr(obj), data, tex, 1, 2, 1.0f, 0, -1, 0, 0))
        return 0;
    obj = gabi::ea(mpBtkAnm1.get());
    warphr::animation(obj)->mFrameCtrl.mRate = 1.0f;
    resource = gabi::ea(dComIfG_resControl());
    modelB->vt = gc_tu.safeString;
    modelB->text = gabi::guest_addr("Ghrwp");
    data = static_cast<J3DModelData*>(
        warphrResource(ptr(resource), modelB.get(), dRes_INDEX_GHRWP_BDL_GHRWPB00_e));
    if (!data)
        JUT_ASSERT_fail("d_a_warphr.cpp", 0x100, "modelData != NULL");
    mpModel2 = mDoExt_J3DModel__create(data, 0x80000, 0x11000222);
    if (!(J3DModel*)mpModel2)
        return 0;
    textureB->vt = gc_tu.safeString;
    mpBtkAnm2 = 0;
    resource = gabi::ea(dComIfG_resControl());
    textureB->text = gabi::guest_addr("Ghrwp");
    tex = static_cast<u8*>(warphrResource(ptr(resource), textureB.get(), dRes_INDEX_GHRWP_BTK_GHRWPB00_e));
    if (!tex)
        JUT_ASSERT_fail("d_a_warphr.cpp", 0x112, "pbtk != NULL");
    obj = warphrBtkConstruct(0);
    mpBtkAnm2 = warphr::btkAt(obj);
    if (!obj)
        return 0;
    if (!warphrInitBtk(ptr(obj), data, tex, 1, 0, 1.0f, 0, -1, 0, 0))
        return 0;
    obj = gabi::ea(mpBtkAnm2.get());
    warphr::animation(obj)->mFrameCtrl.mRate = 0.0f;
    mpBrkAnm = 0;
    resource = gabi::ea(dComIfG_resControl());
    colors->vt = gc_tu.safeString;
    colors->text = gabi::guest_addr("Ghrwp");
    tex = static_cast<u8*>(warphrResource(ptr(resource), colors.get(), dRes_INDEX_GHRWP_BRK_GHRWPB00_e));
    if (!tex)
        JUT_ASSERT_fail("d_a_warphr.cpp", 0x127, "pbrk != NULL");
    obj = warphrBrkConstruct(0);
    mpBrkAnm = warphr::brkAt(obj);
    if (!obj)
        return 0;
    if (!warphrInitBrk(ptr(obj), data, tex, 1, 0, 1.0f, 0, -1, 0, 0))
        return 0;
    obj = gabi::ea(mpBrkAnm.get());
    warphr::animation(obj)->mFrameCtrl.mRate = 0.0f;
    return 1;
}

/* 024DA6DC */
void daWarphr_c::CreateInit() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    auto* model = mpModel1.get();
    cullMtx = warphr::cullMatrixAddress(model);
    fopAcM_setCullSizeBox(this, -300, 0, -300, 300, 5000, 300);
    cullSizeFar = 1.0f;
    set_mtx(this);
    u32 event = warphrEventBit(ptr(saveevent()), 0x2D08);
    u32 manager = eventmanager();
    m2F8 = warphrEventIndex(ptr(manager), event ? "TO_SEA_WARP_2" : "TO_SEA_WARP_1", 255);
}

/* 024DA8C8 */
s32 daWarphr_c::_create() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!(actor_condition & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            warphrActorConstruct(this);
            __vtbl = warphr::ActorVtable;
        }
        actor_condition = (u32)actor_condition | fopAcCnd_INIT_e;
    }
    m2FA = (u32)mParameters >> 28;
    s32 phase = warphrResLoad(&mPhase, "Ghrwp");
    if (phase == cPhs_COMPLEATE_e) {
        if (!warphrEntryHeap(this, gabi::guest_fn((const void*)&CheckCreateHeap), 0x3000))
            return cPhs_ERROR_e;
        CreateInit();
    }
    return phase;
}

/* 024DB310 */
bool daWarphr_c::_execute() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (demoActorID == 0) {
        checkOrder();
        demo_proc();
        eventOrder();
    } else {
        if (warphr::eventRunning(play())) {
            u32 manager = eventmanager();
            u32 started = warphrStartCheckName(ptr(manager), "warp_out");
            if (started) {
                u32 emitter = gabi::ea(m2B0.get());
                if (emitter) {
                    if (l_demoFrame >= 0x225 && !m2E4) {
                        u32 flags = u32(warphr::emitter(emitter)->mFlags);
                        warphr::emitter(emitter)->mFlags = flags | 1;
                        warphrMonotoneOn();
                        gabi::Local<SafeName> hyrule, stage;
                        hyrule->text = gabi::guest_addr("Hyrule");
                        hyrule->vt = gc_tu.safeString;
                        u32 p = play();
                        stage->text = gabi::ea(warphr::game(p)->play.mCurStage.mName);
                        stage->vt = gc_tu.safeString;
                        warphr::assure(hyrule.get());
                        warphr::assure(hyrule.get());
                        u32 left = hyrule->text;
                        warphr::assure(stage.get());
                        u32 right = stage->text;
                        // HD: SafeString checks termination twice for the literal and once for the stage.
                        bool equal = warphr::equalStrings(left, right);
                        s16 rate = 400;
                        if (equal) {
                            p = play();
                            if (s8(warphr::game(p)->play.mCurStage.mLayer) == 8)
                                rate = -600;
                        }
                        l_graphics.mMonotoneRate = rate;
                        l_graphics.mMonotoneRateSpeed = 0;
                        m2E4 = 1;
                    }
                } else {
                    m2B0 = warphr::emitterAt(particle(this, warphr::ID_IT_SP_DEMO20_INDWP00, 4));
                    warphrMonotoneOff();
                }
            }
        }
        m2F0 = 0;
        demo_execute();
    }
    s32 room = current.roomNo;
    s32 reverb = warphrReverb(room);
    warphrLevelSound(warphr::JA_SE_OBJ_HL_WAPR_EFF, &eyePos, 0, reverb);
    set_mtx(this);
    return true;
}

/* 024DAEE4 */
bool daWarphr_c::normal_execute() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (check_warp()) {
        if (!get_return_count())
            warphrOnEventBit(ptr(saveevent()), 0x3810);
        m2F0 = 2;
    }
    anim_play(2);
    return true;
}

/* 024DB24C */
bool daWarphr_c::demo_execute() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 id = demoActorID;
    if (id == 0 || id > 32)
        return true;
    u32 obj = l_demoObjects;
    if (!obj) {
        JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != NULL");
        obj = l_demoObjects;
    }
    u32 actor = warphrDemoActor(ptr(obj), id);
    if (actor) {
        u32 shape = warphr::demoShape(actor);
        m300 = shape;
        if (shape == 0) {
            anim_play(0);
            return true;
        }
        if (shape == 1)
            anim_play(1);
    }
    return true;
}

/* 024DB028 */
void daWarphr_c::demo_proc() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 manager = eventmanager();
    m304 = warphrStaffId(ptr(manager), "Ghrwp", 0, 0);
    if (!warphr::eventRunning(play()))
        return;
    if (eventInfo.mCommand == dEvtCmd_INTALK_e)
        return;
    s32 staff = m304;
    if (staff == -1)
        return;
    manager = eventmanager();
    s32 action = warphrActionIndex(ptr(manager), staff, action_table, 5, 0, 0);
    staff = m304;
    if (action == -1) {
        manager = eventmanager();
        warphrCutEnd(ptr(manager), staff);
        return;
    }
    manager = eventmanager();
    u32 advance = warphrAdvance(ptr(manager), staff);
    if (advance)
        warphr::invokeAction(this, warphr::eventAt(event_init_tbl.entries, u32(action)));
    if (warphr::invokeAction(this, warphr::eventAt(event_action_tbl.entries, u32(action)))) {
        staff = m304;
        manager = eventmanager();
        warphrCutEnd(ptr(manager), staff);
    }
}

/* 024DBAD0 */
void daWarphr_c::initWait(s32 staff) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

/* 024DB6F0 */
bool daWarphr_c::actWait(s32 staff) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    anim_play(0);
    return true;
}

/* 024DB9C0 */
void daWarphr_c::initStartWarp(s32 staff) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 manager = eventmanager();
    warphrSetGoal(ptr(manager), &current.pos);
}

/* 024DB9F8 */
bool daWarphr_c::actStartWarp(s32 staff) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    anim_play(-1);
    return true;
}

/* 024DB718 */
void daWarphr_c::initWarp(s32 staff) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    particle(this, warphr::ID_IT_SN_HRWARPA00);
    m2AC = warphr::emitterAt(particle(this, warphr::ID_IT_SN_HRWARPB00));
    warphrSystemSound(warphr::JA_SE_LK_HL_WAPR_U_IN);
}

/* 024DB7E0 */
bool daWarphr_c::actWarp(s32 staff) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    anim_play(0);
    return true;
}

/* 024DB808 */
void daWarphr_c::initWarpArrive(s32 staff) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    particle(this, warphr::ID_IT_SN_HRWARPA00);
    m2AC = warphr::emitterAt(particle(this, warphr::ID_IT_SN_HRWARPB00));
    set_end_anim();
    if (!l_arriveTargetGuard) {
        l_arriveTargetGuard = 1;
        arrive_target.z = 650;
        arrive_target.x = -500;
        arrive_target.y = 0;
    }
    u32 manager = eventmanager();
    warphrSetGoal(ptr(manager), &arrive_target);
    warphrSystemSound(warphr::JA_SE_LK_HL_WAPR_D_OUT);
}

/* 024DBAD4 */
bool daWarphr_c::actWarpArrive(s32 staff) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return true;
}

/* 024DB930 */
void daWarphr_c::initWarpArriveEnd(s32 staff) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 e = gabi::ea(m2AC.get());
    if (e) {
        invalidate(e);
        m2AC = 0;
    }
}

/* 024DB95C */
bool daWarphr_c::actWarpArriveEnd(s32 staff) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    anim_play(1);
    u32 anm = gabi::ea(mpBrkAnm.get());
    return anm && f32(warphr::animation(anm)->mFrameCtrl.mFrame) < 0.5f;
}

/* 024DB1EC */
void daWarphr_c::eventOrder() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (m2F0 == 2) {
        s32 idx = m2F8;
        warphrOrderEvent(this, idx, 255, 65535, 0, 1);
        eventInfo.mCondition = u16(eventInfo.mCondition) | dEvtCnd_UNK2_e;
    }
}

/* 024DAF54 */
void daWarphr_c::checkOrder() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (eventInfo.mCommand == dEvtCmd_INDEMO_e) {
        s32 idx = m2F8;
        u32 manager = eventmanager();
        u32 started = warphrStartCheck(ptr(manager), idx);
        idx = m2F8;
        if (started && m2F0 != 0)
            m2F0 = 0;
        manager = eventmanager();
        if (warphrEndCheck(ptr(manager), idx)) {
            u32 p = play();
            u16 flags = u16(warphr::game(p)->play.mEvtCtrl.mEventFlag);
            warphr::game(p)->play.mEvtCtrl.mEventFlag = flags | 8;
        }
    } else if (m2F0 == 0) {
        if (!warphr::eventRunning(play()))
            normal_execute();
    }
}

/* 024DADD0 */
void daWarphr_c::anim_play(s32 mode) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mode == 0 || mode == 1) {
        u32 anm = gabi::ea(mpBtkAnm2.get());
        if (anm) {
            warphr::animation(anm)->mFrameCtrl.mRate = 1.0f;
            warphrAnimationPlay(reinterpret_cast<u8*>(mpBtkAnm2.get()));
        }
        anm = gabi::ea(mpBrkAnm.get());
        if (anm) {
            warphr::animation(anm)->mFrameCtrl.mRate = mode == 0 ? 1.0f : -2.0f;
            warphrAnimationPlay(reinterpret_cast<u8*>(mpBrkAnm.get()));
        }
    } else if (mode == 2) {
        u32 anm = gabi::ea(mpBrkAnm.get());
        if (anm) {
            warphr::animation(anm)->mFrameCtrl.mRate = -2.0f;
            warphrAnimationPlay(reinterpret_cast<u8*>(mpBrkAnm.get()));
        }
    }
    u32 anm = gabi::ea(mpBtkAnm1.get());
    if (anm) {
        warphr::animation(anm)->mFrameCtrl.mRate = 1.0f;
        warphrAnimationPlay(reinterpret_cast<u8*>(mpBtkAnm1.get()));
    }
}

/* 024DB674 */
void daWarphr_c::set_end_anim() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 anm = gabi::ea(mpBtkAnm2.get());
    if (anm) {
        s16 end = s16(warphr::animation(anm)->mFrameCtrl.mEnd);
        warphr::animation(anm)->mFrameCtrl.mFrame = f32(end);
    }
    anm = gabi::ea(mpBrkAnm.get());
    if (anm) {
        s16 end = s16(warphr::animation(anm)->mFrameCtrl.mEnd);
        warphr::animation(anm)->mFrameCtrl.mFrame = f32(end);
    }
}

/* 024DAC5C */
s32 daWarphr_c::get_return_count() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!warphrStageBoss(2))
        return 0;
    return warphrEventBit(ptr(saveevent()), 0x1001) ? 2 : 1;
}

/* 024DACC0 */
s32 daWarphr_c::check_warp() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<cXyz> delta, flat, shipdelta, shipflat;
    u32 player = warphr::player(play());
    cXyz_mi(warphr::actorPosition(player), delta.get(), &current.pos);
    flat->y = 0.0f;
    flat->x = delta->x;
    flat->z = delta->z;
    f32 sq = warphrSquareMagnitude(flat.get());
    // HD: the first distance is computed even though only the ship distance determines the result.
    warphrSqrt(sq);
    if (!(warphr::playerStatus(play()) & 0x10000))
        return 0;
    if (!warphr::ship(play()))
        return 0;
    u32 ship = warphr::ship(play());
    cXyz_mi(warphr::actorPosition(ship), shipdelta.get(), &current.pos);
    shipflat->x = shipdelta->x;
    shipflat->y = 0.0f;
    shipflat->z = shipdelta->z;
    sq = warphrSquareMagnitude(shipflat.get());
    f32 distance = warphrSqrt(sq);
    return distance < 500.0f;
}

/* 024DAA1C */
bool daWarphr_c::_draw() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<cXyz> eye, target;
    gabi::Local<csXyz> angle;
    play();
    u32 object = l_demoObjects;
    if (!object) {
        JUT_ASSERT_fail("d_demo.h", 0x23E, "m_object != NULL");
        object = l_demoObjects;
    }
    u32 camera = warphrDemoCamera(ptr(object));
    s16 yaw;
    if (camera) {
        warphr::cameraPosition(camera, eye.get());
        warphr::cameraTarget(camera, target.get());
        yaw = warphrAngleY(eye.get(), target.get());
    } else
        yaw = angle->y;
    /* HD: NOTE (game test 2026-10-04): on the camera == NULL path the ORIGINAL reads uninitialised stack
     * (eye at SP+8..0x10, angle.y at SP+0x22 are never written; undefined behaviour, already in the
     * GameCube source: sp1C/sp08 stay uninitialised when dComIfGp_demo_getCamera() is NULL). The
     * candidate's gabi::Locals start zeroed, so it yields zeros (identity translation, yaw 0) where the
     * game builds m2B4 (+0x3D0) from stale stack bytes. Visual only (projection emitter). */
    f32 x = eye->x, y = eye->y, z = eye->z;
    auto* stack = &l_matrix;
    warphrMatrixTranslate(stack, x, y, z);
    warphrMatrixRotateY(stack, yaw);
    warphrMatrixCopy(stack, &m2B4);
    u32 emitter = gabi::ea(m2B0.get());
    if (emitter)
        warphrMatrixDecompose(&m2B4, warphr::emitter(emitter)->mGlobalRotation,
                              reinterpret_cast<u8*>(&warphr::emitter(emitter)->mGlobalTranslation));
    u32 env = warphrEnvironment();
    warphrTev(ptr(env), 0, &current.pos, &tevStr);
    env = warphrEnvironment();
    warphrApplyTev(ptr(env), (J3DModel*)mpModel1, &tevStr);
    env = warphrEnvironment();
    warphrApplyTev(ptr(env), (J3DModel*)mpModel2, &tevStr);
    u32 anm = gabi::ea(mpBtkAnm1.get());
    if (anm) {
        u32 model = gabi::ea((J3DModel*)mpModel1);
        f32 frame = f32(warphr::animation(anm)->mFrameCtrl.mFrame);
        u32 data = warphr::modelData(model);
        warphrEntryBtk(ptr(anm), ptr(data), frame);
    }
    anm = gabi::ea(mpBtkAnm2.get());
    if (anm) {
        u32 model = gabi::ea((J3DModel*)mpModel2);
        f32 frame = f32(warphr::animation(anm)->mFrameCtrl.mFrame);
        u32 data = warphr::modelData(model);
        warphrEntryBtk(ptr(anm), ptr(data), frame);
    }
    anm = gabi::ea(mpBrkAnm.get());
    if (anm) {
        u32 model = gabi::ea((J3DModel*)mpModel2);
        f32 frame = f32(warphr::animation(anm)->mFrameCtrl.mFrame);
        u32 data = warphr::modelData(model);
        warphrEntryBrk(ptr(anm), ptr(data), frame);
    }
    if (u8(l_graphics.mMonotone)) {
        u32 p = play();
        u32 list = u32(warphr::drawLists(p)->projectionOpaque);
        l_currentDrawLists.opaque = list;
        p = play();
        list = u32(warphr::drawLists(p)->projectionTranslucent);
        l_currentDrawLists.translucent = list;
    }
    warphrModelUpdate((J3DModel*)mpModel1, 0);
    warphrModelUpdate((J3DModel*)mpModel2, 0);
    if (u8(l_graphics.mMonotone)) {
        u32 p = play();
        u32 list = u32(warphr::drawLists(p)->normalOpaque);
        l_currentDrawLists.opaque = list;
        p = play();
        list = u32(warphr::drawLists(p)->normalTranslucent);
        l_currentDrawLists.translucent = list;
    }
    return true;
}

namespace {
/* 024DA998 */
static s32 daWarphr_Create(daWarphr_c* actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return actor->_create();
}
} // namespace

namespace {
/* 024DAA18 */
static bool daWarphr_Delete(daWarphr_c* actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return actor->_delete();
}
} // namespace

namespace {
/* 024DAC58 */
static bool daWarphr_Draw(daWarphr_c* actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return actor->_draw();
}
} // namespace

namespace {
/* 024DB670 */
static bool daWarphr_Execute(daWarphr_c* actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return actor->_execute();
}
} // namespace

namespace {
/* 024DBAC8 */
static bool daWarphr_IsDelete(daWarphr_c* actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return true;
}
} // namespace

// HD: compiler-generated SafeString destructor and header statics.
namespace {
/* 024DBA20 */
static void WarphrStaticInit() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    sinit_header_statics_z(gc_tu.headerStatics[0], gc_tu.headerStatics[1], gc_tu.headerStatics[2]);
}
} // namespace

namespace {
/* 024DBAB4 */
static void WarphrSafeStringDtor(u8* object, s32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (object && (flags & 1))
        warphrDelete(object);
}
} // namespace

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
str 100426AC "d_a_warphr.cpp"
str 100426BC "modelData != NULL" renamed
str 10042694 "pbtk != NULL" renamed
str 100426A0 "pbrk != NULL" renamed
str 100426DC "TO_SEA_WARP_2"
str 100426EC "TO_SEA_WARP_1"
str 1004279C "Ghrwp"
str 10042680 "d_demo.h"
str 10042670 "m_object != NULL" renamed
str 10042720 "Ghrwp" in 024DB028
str 1004270C "d_demo.h" in 024DAA1C
str 100426FC "m_object != NULL" renamed in 024DAA1C
str 10042768 "warp_out"
str 10042760 "Hyrule"
data event_init_tbl 101D3108
data event_action_tbl 101D3130
data action_table 101D3178
data l_matrix 1048D0CC
data l_currentDrawLists 104B4634
data l_graphics 101F4808
data arrive_target 1046EB80
data l_arriveTargetGuard 1046EB8C
data l_demoObjects 101D5FFC
data l_demoFrame 101D6008
ext 025200D4 u32 warphrPlay()
ext 025A847C u32 warphrParticle(u8*, u32, u32, const cXyz*, u32, u32, u32, u32, s32, u32, u32, u32)
ext 026066C4 void* warphrResource(u8*, SafeName*, s32)
ext 025E7C6C u32 warphrBtkConstruct(u32)
ext 025E7CE0 s32 warphrInitBtk(u8*, J3DModelData*, u8*, u32, s32, f32, s32, s32, u32, s32)
ext 025E80D0 u32 warphrBrkConstruct(u32)
ext 025E8154 s32 warphrInitBrk(u8*, J3DModelData*, u8*, u32, s32, f32, s32, s32, u32, s32)
ext 025B8B94 u32 warphrEventBit(u8*, u32)
ext 02543F10 s32 warphrEventIndex(u8*, const char*, u32)
ext 025D4ED0 void warphrActorConstruct(daWarphr_c*)
ext 02520460 s32 warphrResLoad(request_of_phase_process_class*, const char*)
ext 025D63E8 s32 warphrEntryHeap(daWarphr_c*, u32, u32)
ext 025204C8 void warphrResDelete(request_of_phase_process_class*, const char*)
ext 02520A84 u32 warphrStageBoss(u32)
ext 028E8DD0 f32 warphrSquareMagnitude(const cXyz*)
ext 028F4384 f32 warphrSqrt(f32)
ext 025E742C void warphrAnimationPlay(u8*)
ext 025B8B68 void warphrOnEventBit(u8*, u32)
ext 0254407C u32 warphrStartCheck(u8*, s32)
ext 025440C8 u32 warphrEndCheck(u8*, s32)
ext 025D7A58 void warphrOrderEvent(daWarphr_c*, s32, u32, u32, u32, u32)
ext 02526E70 u32 warphrDemoActor(u8*, u32)
ext 025E1988 void warphrSystemSound(u32)
ext 02543714 void warphrSetGoal(u8*, const cXyz*)
ext 028F026C void warphrRegisterObject(u8*)
ext 028ED6F8 void warphrRandomInit(u8*)
ext 028EAB2C void warphrRandomInit2(u8*)
ext 0273AF40 void warphrDelete(void*)
ext 02542D88 s32 warphrStaffId(u8*, const char*, u32, u32)
ext 02542EDC s32 warphrActionIndex(u8*, s32, const void*, s32, u32, u32)
ext 02543280 void warphrCutEnd(u8*, s32)
ext 025447C8 u32 warphrAdvance(u8*, s32)
ext 025283F8 u32 warphrDemoCamera(u8*)
ext 0200F93C s16 warphrAngleY(const cXyz*, const cXyz*)
ext 028E93CC void warphrMatrixTranslate(Mtx34*, f32, f32, f32)
ext 025F1C28 void warphrMatrixRotateY(Mtx34*, s16)
ext 028E90D4 void warphrMatrixCopy(Mtx34*, Mtx34*)
ext 028249B0 void warphrMatrixDecompose(Mtx34*, u8*, u8*)
ext 02555D0C u32 warphrEnvironment()
ext 025626A4 void warphrTev(u8*, s32, const cXyz*, dKy_tevstr_c*)
ext 02562F5C void warphrApplyTev(u8*, J3DModel*, dKy_tevstr_c*)
ext 025E7FC4 void warphrEntryBtk(u8*, u8*, f32)
ext 025E83FC void warphrEntryBrk(u8*, u8*, f32)
ext 025E2DE0 void warphrModelUpdate(J3DModel*, u32)
ext 025445B8 u32 warphrStartCheckName(u8*, const char*)
ext 025F0820 void warphrMonotoneOn()
ext 025F0830 void warphrMonotoneOff()
ext 02520540 s32 warphrReverb(s32)
ext 025E1A40 void warphrLevelSound(u32, const cXyz*, u32, s32)
*/
// clang-format on
