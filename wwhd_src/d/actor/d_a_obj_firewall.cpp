/* Ring-shaped fire wall. */
#include "d/actor/d_a_obj_firewall.h"
const gc_tu_data gc_tu = {.safeString = 0x10028A4C};
// HD: typed views for fields absent from the shared emitter and shape API.
struct FirewallEmitter {
    u8 _00[0x5C];
    be<s32> mMaxFrame;
    u8 _60[0x254 - 0x60];
    be<u32> mStatus;
};
struct FirewallShape {
    u8 _00[0x18];
    be<u32> __vtbl_base, __vtbl_shape;
};
struct FirewallPlayerVtable {
    u8 _00[0xE4];
    be<u32> seStart;
};
struct FirewallPlayer {
    u8 _00[0xB4];
    gptr<FirewallPlayerVtable> vtable;
};
// HD: tww has no action body yet; the state names follow its declared methods.
enum ActionTarget : u32 {
    Wait = 0x02342878,
    Wait2 = 0x02342990,
    Wait3 = 0x02342A8C,
    Appear = 0x02342B6C,
    DemoEndWait = 0x02342C04,
    BurnWait = 0x02342C88,
    Retire = 0x02342DD8
};
struct ArcName {
    be<u8> bytes[8];
};
static ArcName archive;
struct GuestText {
    be<u8> bytes[64];
};
static GuestText fileText;
static GuestText heapAssertText;
static GuestText playerStaffText;
static GuestText waitStaffText;
static GuestText wait3StaffText;
static GuestText burnCutText;
static GuestText burn3CutText;
static GuestText burnAssertFileText;
static GuestText burnAssertText;
static const char *text(const GuestText &t) { return reinterpret_cast<const char *>(&t); }
static const char *arcName() { return reinterpret_cast<const char *>(&archive); }
static const char *assertFile() { return text(fileText); }
static const char *heapAssertion() { return text(heapAssertText); }
static const char *playerStaffName() { return text(playerStaffText); }
static const char *waitStaffName() { return text(waitStaffText); }
static const char *wait3StaffName() { return text(wait3StaffText); }
static const char *burnCutName() { return text(burnCutText); }
static const char *burn3CutName() { return text(burn3CutText); }
static const char *burnAssertFile() { return text(burnAssertFileText); }
static const char *burnAssertion() { return text(burnAssertText); }
struct Names2 {
    gptr<char> names[2];
};
static Names2 eventNames;
struct Names3 {
    gptr<char> names[3];
};
static Names3 playerCuts;
struct Sounds3 {
    be<u32> ids[3];
};
static Sounds3 playerSounds;
struct Angles5 {
    be<s16> values[5];
};
static Angles5 collisionAngles;
struct TrigEntry {
    be<f32> sine, cosine;
};
struct TrigTable {
    TrigEntry entries[8192];
};
static TrigTable trig;
struct CylinderSource {
    be<u32> words[17];
};
static CylinderSource l_cyl_src;
struct FloatValue {
    be<f32> value;
};
struct WordValue {
    be<u32> value;
};
struct ByteValue {
    be<u8> value;
};
struct Record {
    be<u32> words[3];
};
static FloatValue radiusBias;
static FloatValue minimum;
static FloatValue maximum;
static WordValue zero0;
static WordValue zero1;
static WordValue zero2;
static WordValue zero3;
static WordValue angleLimit;
static ByteValue flag0;
static ByteValue flag1;
static Record record0;
static Record record1;
static Record record2;
// HD: preserve bit copies of the floating-point vectors, including NaN payloads.
static be<u32> &word(be<f32> &v) { return *reinterpret_cast<be<u32> *>(&v); }
static bool sameGuestString(const char *left, const char *right) {
    const auto *a = reinterpret_cast<const be<u8> *>(left);
    const auto *b = reinterpret_cast<const be<u8> *>(right);
    for (;;) {
        u8 x = *a++, y = *b++;
        if (x != y)
            return false;
        if (!x)
            return true;
    }
}
static void setAction(daObjFirewall_c *actor, ActionTarget action) {
    actor->mAction.d = 0;
    actor->mAction.i = -1;
    actor->mAction.f = action;
}
static void releaseBackground(daObjFirewall_c *actor) {
    if (actor->heap) {
        dBgW *bg = actor->mpBgW;
        if (bg) {
            if (bg->ChkUsed()) {
                dBgS *world = dComIfG_Bgsp();
                bg = actor->mpBgW;
                world->Release(bg);
            }
            actor->mpBgW = nullptr;
        }
    }
}
extern void *getResource(dRes_control_c *, SafeString *, s32);
static void *resource(s32 index) {
    gabi::Local<SafeString> name;
    name->mStringTop = gabi::ea(&archive);
    name->__vtbl = 0x10028A4C;
    return getResource(dComIfG_resControl(), name.get(), index);
}
extern void startSound(u32, const cXyz *);
extern void deleteSound(const cXyz *);
extern void modelUpdateDL(J3DModel *, u32);
extern void playAnimation(void *);
extern void destroyCylinder(dCcD_Cyl *, u32);
extern void destroyStatus(dCcD_Stts *, u32);
extern void destroyActor(fopAc_ac_c *, u32);
extern void constructStatus(dCcD_Stts *);
extern void constructGStatus(void *);
extern void constructObjectInfo(dCcD_GObjInf *);
extern void constructCylinder(cM3dGCyl *);
extern void constructArray(void *, u32, u32, u32);
extern void registerHeader(void *);
extern void constructHeader0(void *);
extern void constructHeader1(void *);
extern void addCalc(be<f32> *, f32, f32, f32);
extern void setLightType(u32, u32, f32);
extern dBgW *createBackground(cBgD_t *, u32, Mtx34 *);
extern u32 initBtk(mDoExt_btkAnm *, J3DModelData *, J3DAnmTextureSRTKey *, u32, u32, f32, s32, s32, u32, u32);
extern u32 initBrk(mDoExt_brkAnm *, J3DModelData *, J3DAnmTevRegKey *, u32, u32, f32, s32, s32, u32, u32);
extern void translateMatrix(f32, f32, f32);
extern const char *getNowCutName(void *, s32);
extern u32 getEventData(void *, s32);
// HD: these paths forward the full return word to getEventData, rather than a narrowed event ID.
extern s32 getEventIdxFullWord(void *, const char *, u32);
extern void subtractVector(const cXyz *, cXyz *, const cXyz *);
extern f32 vectorSquare(const cXyz *);
extern f32 sqrtf_guest(f32);
extern void orderEvent(fopAc_ac_c *, s16, u32, u32, u32, u32);
extern void registCollisionGuest(daObjFirewall_c *);
extern s32 atan2s(f32, f32);
static daObjFirewall_c *firewall_ctor(daObjFirewall_c *);
static u32 param_extract(daObjFirewall_c *, u32, u32);
static BOOL solidHeapCB(fopAc_ac_c *);
/* 02341BBC */
void daObjFirewall_c::particle_delete() {
    for (u32 i = 0; i < 12; i++) {
        FirewallEmitter *e = mpEmitters[i];
        if (e) {
            u32 flags = e->mStatus;
            e->mMaxFrame = -1;
            e->mStatus = flags | 1;
            mpEmitters[i] = nullptr;
        }
    }
}
/* 02341C68 */
void daObjFirewall_c::seStart(u32 sound) {
    for (u32 i = 0; i < 8; i++)
        startSound(sound, &mSoundPositions[i]);
}
/* 02341CC0 */
void daObjFirewall_c::set_se(u32 enabled) {
    // HD: the entry accepts a full word, compares it before narrowing to the stored byte.
    u8 old = mSoundEnabled;
    if (old != enabled)
        seStart(old ? 0x6950 : 0x694E);
    else if (old)
        seStart(0x614F);
    mSoundEnabled = enabled;
}
/* 02341D34 */
void daObjFirewall_c::setup_put_the_fire_out() {
    if (mBurning) {
        particle_delete();
        releaseBackground(this);
        set_se(0);
        mBurning = 0;
    }
}
/* 02341DC4 */
void daObjFirewall_c::seDelete() {
    if (mSoundInitialized)
        for (u32 i = 0; i < 8; i++)
            deleteSound(&mSoundPositions[i]);
}
/* 02341E18 */
BOOL daObjFirewall_c::_delete() {
    dComIfG_resDelete(&mPhase, arcName());
    releaseBackground(this);
    setup_put_the_fire_out();
    seDelete();
    return TRUE;
}
/* 0234218C */
BOOL daObjFirewall_c::_draw() {
    J3DModel *model = mpModel;
    f32 frame = mBtk.getFrame();
    J3DModelData *data = model->getModelData();
    mBtk.entry(data, frame);
    model = mpModel;
    frame = mBrk.getFrame();
    data = model->getModelData();
    mBrk.entry(data, frame);
    modelUpdateDL(mpModel, 0);
    return TRUE;
}
/* 02342DD8 */
void daObjFirewall_c::retire_act_proc() {
    playAnimation(&mBrk);
    if (mBrk.mFrameCtrl.checkState(J3DFrameCtrl::STATE_STOP_E) || mBrk.getPlaySpeed() == 0) {
        dComIfGs_onEventBit(0x2C01);
        fopAcM_delete(this);
    }
}
/* 02342B6C */
void daObjFirewall_c::appear_act_proc() {
    playAnimation(&mBrk);
    if (mBrk.mFrameCtrl.checkState(J3DFrameCtrl::STATE_STOP_E) || mBrk.getPlaySpeed() == 0) {
        s16 event = mEventIndex;
        mAction.d = 0;
        mLightStrength = 1;
        mAction.i = -1;
        mAction.f = event == -1 ? BurnWait : DemoEndWait;
    }
    set_se(1);
}
/* 02342C04 */
void daObjFirewall_c::demo_end_wait_act_proc() {
    s16 event = mEventIndex;
    if (dComIfGp_evmng_endCheck(event)) {
        dComIfGp_event_onEventFlag(8);
        setAction(this, BurnWait);
    }
    set_se(1);
}
/* 02342EE4 */
static LIGHT_INFLUENCE *particle_ctor(LIGHT_INFLUENCE *particle) {
    if (!particle)
        particle = static_cast<LIGHT_INFLUENCE *>(operator_new(sizeof(LIGHT_INFLUENCE)));
    if (particle)
        particle->mHD20 = 1;
    return particle;
}
/* 02342F24 */
static void particle_dtor(LIGHT_INFLUENCE *particle, u32 flags) {
    if (particle && (flags & 1))
        operator_delete(particle);
}
/* 02342F38 */
static void firewall_dtor(daObjFirewall_c *actor, u32 flags) {
    if (actor) {
        destroyCylinder(&actor->mCollisionCylinder, 2);
        destroyStatus(&actor->mCollisionStatus, 2);
        destroyActor(actor, 0);
        if (flags & 1)
            operator_delete(actor);
    }
}
/* 02342FA4 */
static void empty_virtual() {}
/* 02342FA8 */
static u32 param_extract(daObjFirewall_c *actor, u32 width, u32 shift) {
    u32 params = actor->mParameters;
    u32 mask = (width & 32) ? 0u : 1u << (width & 31);
    return ((shift & 32) ? 0u : params >> (shift & 31)) & (mask - 1);
}
/* 023418D0 */
void daObjFirewall_c::init_mtx() {
    f32 y = scale.y, x = scale.x;
    J3DModel *model = mpModel;
    f32 z = scale.z;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    model->mBaseScale.x = x;
    mDoMtx_stack_c::transS(0, 0, 0);
    f32 matrix[12];
    Mtx34 *now = mDoMtx_stack_c::get();
    for (u32 i = 0; i < 12; i++)
        matrix[i] = now->m[i / 4][i % 4];
    model = mpModel;
    for (u32 i = 0; i < 12; i++)
        model->mBaseTransformMtx.m[i / 4][i % 4] = matrix[i];
}
/* 02341614 */
static daObjFirewall_c *firewall_ctor(daObjFirewall_c *actor) {
    if (!actor)
        actor = static_cast<daObjFirewall_c *>(operator_new(sizeof(daObjFirewall_c)));
    if (actor) {
        fopAc_ac_c_ct(actor);
        actor->__vtbl = 0x10028A74;
        constructStatus(&actor->mCollisionStatus);
        constructGStatus(&actor->mCollisionStatus.__vtbl_gstts);
        actor->mCollisionStatus.__vtbl = 0x1004AE88;
        actor->mCollisionStatus.__vtbl_gstts = 0x1004AEC0;
        constructObjectInfo(&actor->mCollisionCylinder);
        auto *shape = reinterpret_cast<FirewallShape *>(actor->mCollisionCylinder.mShapeAttr);
        shape->__vtbl_shape = 0x100015A8;
        shape->__vtbl_base = 0x10028A64;
        constructCylinder(&actor->mCollisionCylinder.mCyl);
        actor->mCollisionCylinder.__vtbl_hitinf = 0x1004B108;
        shape->__vtbl_shape = 0x1004B160;
        actor->mCollisionCylinder.mCyl.__vtbl = 0x1004B150;
        mDoExt_btkAnm::ct(&actor->mBtk);
        mDoExt_brkAnm::ct(&actor->mBrk);
        // HD: the array helper takes the separately verified light-constructor guest entry.
        constructArray(&actor->mLight, 1, sizeof(LIGHT_INFLUENCE), 0x02342EE4);
    }
    return actor;
}
/* 02342E44 */
static void firewall_static_init() {
    zero2.value = 0;
    zero0.value = 0;
    zero3.value = 0;
    zero1.value = 0;
    registerHeader(&record0);
    minimum.value = -3.1415927410125732f;
    maximum.value = 3.1415927410125732f;
    constructHeader0(&flag0);
    registerHeader(&record1);
    constructHeader1(&flag1);
    registerHeader(&record2);
    angleLimit.value = 0xE00;
}
/* 023419A0 */
s32 daObjFirewall_c::_create() {
    u32 condition = actor_condition;
    if (!(condition & fopAcCnd_INIT_e)) {
        if (this) {
            firewall_ctor(this);
            condition = actor_condition;
        }
        actor_condition = condition | fopAcCnd_INIT_e;
    }
    s32 phase = dComIfG_resLoad(&mPhase, arcName());
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (!fopAcM_entrySolidHeap(this, solidHeapCB, 0x1420))
        return cPhs_ERROR_e;
    J3DModel *model = mpModel;
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(model));
    init_mtx();
    mCollisionStatus.Init(255, 255, this);
    mCollisionCylinder.Set(reinterpret_cast<dCcD_SrcCyl *>(&l_cyl_src));
    u32 flags = mCollisionCylinder.mFlags;
    mCollisionCylinder.mStts = &mCollisionStatus;
    mCollisionCylinder.mFlags = flags | 2;
    for (u32 i = 0; i < 8; i++) {
        f32 scaleX = scale.x, sine = trig.entries[0x400 * i].sine, x = current.pos.x;
        f32 sx = sine * 1000.f, cosine = trig.entries[0x400 * i].cosine;
        f32 px = gabi::fmadds(sx, scaleX, x);
        f32 y = current.pos.y, cz = cosine * 1000.f, z = current.pos.z;
        mSoundPositions[i].x = px;
        mSoundPositions[i].y = y;
        mSoundPositions[i].z = gabi::fmadds(cz, scaleX, z);
    }
    mSoundInitialized = 1;
    u32 sw = param_extract(this, 8, 0);
    mSaveSwitch = sw;
    // HD: this branch tests exactly one, rather than any nonzero event-bit result.
    if (dComIfGs_isEventBit(0x3520) == 1) {
        mEventStarted = 1;
        const char *name = eventNames.names[1];
        s32 event = getEventIdxFullWord(dComIfGp_getPEvtManager(), name, 255);
        mEventIndex = event;
        setAction(this, Wait2);
    } else {
        mAction.d = 0;
        mEventIndex = -1;
        mAction.i = -1;
        mEventStarted = 0;
        mAction.f = Wait;
    }
    return phase;
}
/* 02341BB8 */
static BOOL daObjFirewall_Create(daObjFirewall_c *actor) { return actor->_create(); }
/* 02341EA0 */
static BOOL daObjFirewall_Delete(daObjFirewall_c *actor) { return actor->_delete(); }
/* 023421EC */
static BOOL daObjFirewall_Draw(daObjFirewall_c *actor) { return actor->_draw(); }
/* 023421F0 */
static BOOL daObjFirewall_IsDelete(daObjFirewall_c *actor) { return TRUE; }
/* 02341EA4 */
void daObjFirewall_c::setPointLight() {
    s16 index = mAction.i;
    bool lit = false;
    if (index == -1 && mAction.d == 0) {
        u32 action = mAction.f;
        lit = action == BurnWait || action == DemoEndWait || action == Appear;
    }
    if (lit) {
        f32 random = cM_rndF(0.5f);
        addCalc(&mLightStrength, random + 1, 0.5f, 0.04f);
    }
    if (lit) {
        u32 y = word(mLightPosition.y), x = word(mLightPosition.x);
        mLight.mColorR = 500;
        word(mLight.mPos.x) = x;
        word(mLight.mPos.y) = y;
        u32 z = word(mLightPosition.z);
        mLight.mColorG = 200;
        mLight.mColorB = 120;
        word(mLight.mPos.z) = z;
    } else {
        u32 z = word(mLightPosition.z), y = word(mLightPosition.y), x = word(mLightPosition.x);
        mLightStrength = 0;
        word(mLight.mPos.x) = x;
        word(mLight.mPos.y) = y;
        mLight.mColorR = 500;
        mLight.mColorG = 200;
        mLight.mColorB = 120;
        word(mLight.mPos.z) = z;
    }
    f32 radius = (radiusBias.value + 400) * mLightStrength;
    mLight.mPower = (f32)(s16)gabi::ftoi(radius);
    mLight.mFluctuation = 250;
    f32 strength = mLightStrength;
    if (strength > 1)
        setLightType(0, 3, 1);
    else if (strength > 0.5f)
        setLightType(0, 3, strength);
    else
        setLightType(3, 0, 1 - strength);
}
/* 023420EC */
BOOL daObjFirewall_c::_execute() {
    dBgW *bg = mpBgW;
    if (bg && bg->ChkUsed())
        bg->Move();
    playAnimation(&mBtk);
    setPointLight();
    (this->*mAction)();
    return TRUE;
}
/* 02342188 */
static BOOL daObjFirewall_Execute(daObjFirewall_c *actor) { return actor->_execute(); }
/* 02341704 */
BOOL daObjFirewall_c::create_heap() {
    auto *data = static_cast<J3DModelData *>(resource(6));
    auto *btk = static_cast<J3DAnmTextureSRTKey *>(resource(12));
    auto *brk = static_cast<J3DAnmTevRegKey *>(resource(9));
    if (!data || !btk || !brk) {
        JUT_ASSERT_fail(assertFile(), 0x171, heapAssertion());
        return FALSE;
    }
    mpModel = mDoExt_J3DModel__create(data, 0x80000, 0x11000222);
    auto *mesh = static_cast<cBgD_t *>(resource(15));
    J3DModel *model = mpModel;
    mpBgW = createBackground(mesh, 1, J3DModel_getBaseTRMtx(model));
    u32 btkOk = initBtk(&mBtk, data, btk, 1, 2, 1, 0, -1, 0, 0);
    u32 brkOk = initBrk(&mBrk, data, brk, 1, 0, 1, 0, -1, 0, 0);
    return mpModel && mpBgW && btkOk && brkOk;
}
/* 023418CC */
static BOOL solidHeapCB(fopAc_ac_c *actor) { return static_cast<daObjFirewall_c *>(actor)->create_heap(); }
/* 023424AC */
void daObjFirewall_c::particle_set() {
    gabi::Local<csXyz> rotation;
    csXyz_ct(rotation.get(), 0, 0, 0);
    const s16 angles[11] = {0x1555, s16(-0x1555), 0x4000, s16(-0x4000), 0x6AAA,      s16(-0x6AAA),
                            0,      s16(-0x2AAA), 0x2AAA, 0x5555,       s16(-0x5555)};
    for (u32 i = 0; i < 11; i++) {
        if (!mpEmitters[i]) {
            rotation->y = angles[i];
            mpEmitters[i] = reinterpret_cast<FirewallEmitter *>(
                dComIfGp_particle_set(i < 6 ? 0x82B6 : 0x82B7, &current.pos, rotation.get(), nullptr, 255, nullptr, -1,
                                      nullptr, nullptr, nullptr));
        }
    }
    if (!mpEmitters[11])
        mpEmitters[11] = reinterpret_cast<FirewallEmitter *>(
            dComIfGp_particle_set(0x82B8, &current.pos, nullptr, nullptr, 255, nullptr, -1, nullptr, nullptr, nullptr));
}
/* 0234276C */
void daObjFirewall_c::setup_burn_up() {
    particle_set();
    dBgW *bg = mpBgW;
    if (bg && !bg->ChkUsed()) {
        dBgS *world = dComIfG_Bgsp();
        bg = mpBgW;
        world->Regist(bg, this);
    }
    set_se(1);
    f32 scaleX = scale.x, x = current.pos.x, z = current.pos.z, y = current.pos.y;
    f32 distance = 1000 * scaleX;
    mDoMtx_stack_c::transS(x, y, z);
    mDoMtx_stack_c::YrotM(0);
    translateMatrix(distance, 0, 0);
    Mtx34 *matrix = mDoMtx_stack_c::get();
    mLightPosition.x = matrix->m[0][3];
    mLightPosition.y = matrix->m[1][3];
    mLightPosition.z = matrix->m[2][3];
    mAction.f = Appear;
    mAction.i = -1;
    mAction.d = 0;
    mBurning = 1;
}
/* 0234268C */
void daObjFirewall_c::set_pl_se() {
    s32 staff = dComIfGp_evmng_getMyStaffId(playerStaffName(), 0, 0);
    if (staff == -1)
        return;
    const char *cut = getNowCutName(dComIfGp_getPEvtManager(), staff);
    u32 index = mPlayerSoundIndex;
    const char *name = playerCuts.names[index];
    if (!sameGuestString(cut, name))
        return;
    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    if (!player)
        return;
    auto *view = reinterpret_cast<FirewallPlayer *>(player);
    index = mPlayerSoundIndex;
    u32 sound = playerSounds.ids[index];
    gabi::call_ptr<void>(view->vtable->seStart, player, sound);
    mPlayerSoundIndex = (u32)mPlayerSoundIndex + 1;
}
/* 02342878 */
void daObjFirewall_c::wait_act_proc() {
    const char *name = eventNames.names[0];
    void *manager = dComIfGp_getPEvtManager();
    s32 event = getEventIdxFullWord(dComIfGp_getPEvtManager(), name, 255);
    if (!getEventData(manager, event))
        return;
    s32 staff = dComIfGp_evmng_getMyStaffId(waitStaffName(), 0, 0);
    if (staff == -1)
        return;
    const char *cut = getNowCutName(dComIfGp_getPEvtManager(), staff);
    if (sameGuestString(cut, burnCutName())) {
        dComIfGs_onEventBit(0x3520);
        setup_burn_up();
    } else
        set_pl_se();
}
/* 02342A8C */
void daObjFirewall_c::wait3_act_proc() {
    const char *name = eventNames.names[1];
    void *manager = dComIfGp_getPEvtManager();
    s32 event = getEventIdxFullWord(dComIfGp_getPEvtManager(), name, 255);
    if (!getEventData(manager, event))
        return;
    s32 staff = dComIfGp_evmng_getMyStaffId(wait3StaffName(), 0, 0);
    if (staff == -1)
        return;
    const char *cut = getNowCutName(dComIfGp_getPEvtManager(), staff);
    if (sameGuestString(cut, burn3CutName())) {
        setup_burn_up();
    }
}
/* 02342990 */
void daObjFirewall_c::wait2_act_proc() {
    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    if (!player)
        return;
    gabi::Local<cXyz> delta, flat;
    subtractVector(&player->current.pos, delta.get(), &current.pos);
    flat->x = (f32)delta->x;
    flat->y = 0;
    flat->z = (f32)delta->z;
    f32 square = vectorSquare(flat.get());
    f32 distance = sqrtf_guest(square);
    if (distance < 950 && (f32)player->current.pos.z < -7000) {
        if (eventInfo.mCommand == 2) {
            mAction.i = -1;
            mAction.f = Wait3;
            mAction.d = 0;
        } else {
            s16 event = mEventIndex;
            orderEvent(this, event, 255, 65535, 0, 1);
        }
    }
}
/* 02342C88 */
void daObjFirewall_c::burn_wait_act_proc() {
    mCollisionStatus.Move();
    registCollisionGuest(this);
    u32 sw = mSaveSwitch;
    if (sw != 255) {
        s8 room = home.roomNo;
        if (dComIfGs_isSwitch(sw, room) == 1) {
            dComIfGs_onEventBit(0x2C01);
            auto *brk = static_cast<J3DAnmTevRegKey *>(resource(9));
            if (!brk)
                JUT_ASSERT_fail(burnAssertFile(), 0x4E9, burnAssertion());
            J3DModel *model = mpModel;
            J3DModelData *data = model->getModelData();
            initBrk(&mBrk, data, brk, 1, 0, -1, 0, -1, 1, 0);
            setup_put_the_fire_out();
            setAction(this, Retire);
            return;
        }
    }
    set_se(1);
}
/* 023421F8 */
void daObjFirewall_c::registCollisionTable() {
    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    f32 ax = current.pos.x, px = player->current.pos.x, az = current.pos.z, pz = player->current.pos.z;
    s32 angle = atan2s(px - ax, pz - az);
    s32 absolute = std::abs((s32)(s16)angle), limit = angleLimit.value;
    f32 inset = 100;
    if (absolute < limit) {
        u16 theta = gabi::ftoi(((f32)absolute / (f32)limit) * 16384);
        f32 cosine = trig.entries[theta >> 3].cosine;
        inset = gabi::fmadds(30, cosine, 100);
    } else {
        for (u32 i = 0; i < 5; i++) {
            s16 base = collisionAngles.values[i];
            s32 diff = std::abs((s32)(s16)(angle - base));
            if (diff < 0x1A00) {
                u16 theta = gabi::ftoi(((f32)diff / 6656) * 16384);
                f32 cosine = trig.entries[theta >> 3].cosine;
                inset = gabi::fmadds(115, cosine, 100);
                if (inset == -1)
                    inset = 100;
                break;
            }
        }
    }
    gabi::Local<cXyz> center, delta;
    center->x = (f32)current.pos.x;
    f32 z = current.pos.z, y = current.pos.y, scaleX = scale.x, scaleY = scale.y;
    center->y = y - 300;
    center->z = z;
    f32 height = gabi::fmadds(10000, scaleY, 300);
    mCollisionCylinder.SetC(center.get());
    mCollisionCylinder.SetR(gabi::fmsubs(1000, scaleX, inset));
    mCollisionCylinder.SetH(height);
    subtractVector(&current.pos, delta.get(), &player->current.pos);
    u32 dx = word(delta->x), dy = word(delta->y);
    word(mCollisionCylinder.mGObjAt.mVec.x) = dx;
    u32 dz = word(delta->z);
    word(mCollisionCylinder.mGObjAt.mVec.y) = dy;
    word(mCollisionCylinder.mGObjAt.mVec.z) = dz;
    dComIfG_Ccsp_Set(&mCollisionCylinder);
}
/* WWHD glue
data archive 10028A20
data fileText 10028AF0
data heapAssertText 10028AEC
data playerStaffText 10028B4C
data waitStaffText 10028B54
data wait3StaffText 10028B6C
data burnCutText 10028B5C
data burn3CutText 10028B74
data burnAssertFileText 10028B7C
data burnAssertText 10028B94
data eventNames 101C943C
data playerCuts 101C93E0
data playerSounds 101C93EC
data collisionAngles 101C93D4
data trig 104A44F8
data l_cyl_src 10028AA4
data radiusBias 1047BCD0
data minimum 10469B80
data maximum 10469B84
data zero0 10469B90
data zero1 10469B94
data zero2 10469B98
data zero3 10469B9C
data angleLimit 10469B88
data flag0 10469B8C
data flag1 10469B8D
data record0 101C9418
data record1 101C9424
data record2 101C9430
ext 026066C4 void* getResource(dRes_control_c*,SafeString*,s32)
ext 025E19CC void startSound(u32,const cXyz*)
ext 025E1B34 void deleteSound(const cXyz*)
ext 025E2DE0 void modelUpdateDL(J3DModel*,u32)
ext 025E742C void playAnimation(void*)
ext 02515A70 void destroyCylinder(dCcD_Cyl*,u32)
ext 02515860 void destroyStatus(dCcD_Stts*,u32)
ext 025D50BC void destroyActor(fopAc_ac_c*,u32)
ext 0200BD2C void constructStatus(dCcD_Stts*)
ext 02515DA0 void constructGStatus(void*)
ext 02515FB8 void constructObjectInfo(dCcD_GObjInf*)
ext 02018590 void constructCylinder(cM3dGCyl*)
ext 028EFFD0 void constructArray(void*,u32,u32,u32)
ext 028F026C void registerHeader(void*)
ext 028ED6F8 void constructHeader0(void*)
ext 028EAB2C void constructHeader1(void*)
ext 0200ED84 void addCalc(be<f32>*,f32,f32,f32)
ext 0255FDA4 void setLightType(u32,u32,f32)
ext 024F2478 dBgW* createBackground(cBgD_t*,u32,Mtx34*)
ext 025E7CE0 u32 initBtk(mDoExt_btkAnm*,J3DModelData*,J3DAnmTextureSRTKey*,u32,u32,f32,s32,s32,u32,u32)
ext 025E8154 u32 initBrk(mDoExt_brkAnm*,J3DModelData*,J3DAnmTevRegKey*,u32,u32,f32,s32,s32,u32,u32)
ext 025F24E0 void translateMatrix(f32,f32,f32)
ext 02544830 const char* getNowCutName(void*,s32)
ext 02543F10 s32 getEventIdxFullWord(void*,const char*,u32)
ext 02544044 u32 getEventData(void*,s32)
ext 0201ADE0 void subtractVector(const cXyz*,cXyz*,const cXyz*)
ext 028E8DD0 f32 vectorSquare(const cXyz*)
ext 028F4384 f32 sqrtf_guest(f32)
ext 025D7A58 void orderEvent(fopAc_ac_c*,s16,u32,u32,u32,u32)
ext 023421F8 void registCollisionGuest(daObjFirewall_c*)
ext 020195B0 s32 atan2s(f32,f32)
*/
