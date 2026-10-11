// Icy clouds around Ice Ring Isle. GC actor logic adapted to the WWHD layout.
#include "d/actor/d_a_obj_iceisland.h"

static IceResourceManager l_resourceManager;
static IceLightRecord l_lightDefaults;
static f32 l_srgbPower;
static f32 l_linearPower;
static IcePiPair l_piPair;
static IceHeaderZeros l_headerZeros;
static u8 l_registrationZ;
static u8 l_registrationA;
static u8 l_registrationB;
static f32 l_minAngle;
static f32 l_maxAngle;
static u8 l_initA;
static u8 l_initB;

using Actor = daObjIceisland_c;
static BOOL isSwitch(Actor *a) {
    return fopAcM_isSwitch(a, (u32(a->mParameters) >> 8) & 255);
}
static void order(Actor *a, s16 event) {
    iceOrder(a, event, 255, 65535, 0, 1);
}
static J3DModelData *resource(s32 index) {
    gabi::Local<ResourceName> name;
    name->data = 0x1002B220;
    name->vtable = 0x1002B1F4;
    return iceResource(l_resourceManager.mpManager, (ResourceName *)name, index);
}
/* 0235EB00 */
BOOL daObjIceisland_c::CreateHeap() {
    auto *data = resource(5);
    if (!data)
        JUT_ASSERT_fail(STR(0x1002B238), 0x68, STR(0x1002B228));
    mpModel = mDoExt_J3DModel__create(data, 0, 0x11020203);
    auto *animation = resource(11);
    if (!animation)
        JUT_ASSERT_fail(STR(0x1002B238), 0x6F, STR(0x1002B250));
    s32 first = iceBtkInit(&mBtkAnm1, data, animation, 1, 2, 0, 1.0f, -1, 0, 0);
    animation = resource(12);
    if (!animation)
        JUT_ASSERT_fail(STR(0x1002B238), 0x75, STR(0x1002B25C));
    s32 second = iceBtkInit(&mBtkAnm2, data, animation, 1, 2, 0, 1.0f, -1, 0, 0);
    animation = resource(8);
    if (!animation)
        JUT_ASSERT_fail(STR(0x1002B238), 0x7C, STR(0x1002B268));
    s32 third = iceBrkInit(&mBrkAnm, data, animation, 1, 0, 0, 1.0f, -1, 0, 0);
    J3DModel *model = mpModel;
    if (model) {
        u32 flags = reinterpret_cast<IceModel *>(model)->mFlags & ~1u;
        reinterpret_cast<IceModel *>(model)->mFlags = flags;
        iceModelFlags(model, flags);
        model = mpModel;
    }
    return model && first && second && third;
}
/* 0235ED24 */
static BOOL checkCreateHeap(fopAc_ac_c *base) {
    return static_cast<Actor *>(base)->CreateHeap();
}
/* 0235ED28 */
static Actor *construct(Actor *a) {
    if (!a)
        a = static_cast<Actor *>(operator_new(0x6F4));
    if (a) {
        fopAc_ac_c_ct(a);
        a->__vtbl = 0x1002B20C;
        mDoExt_btkAnm::ct(&a->mBtkAnm1);
        mDoExt_btkAnm::ct(&a->mBtkAnm2);
        iceBrkCtor(&a->mBrkAnm);
        // HD: copy each lighting constructor record in its original mixed float/word order.
        for (IceLightRecord *record : {&a->mTevStr.first, &a->mTevStr.second, &a->mTevStr.third}) {
            for (u32 i = 0; i < 6; i++)
                record->head[i] = (f32)l_lightDefaults.head[i];
            for (u32 i = 0; i < 3; i++)
                record->colors[i] = (u32)l_lightDefaults.colors[i];
            for (u32 i = 0; i < 8; i++)
                record->tail[i] = (f32)l_lightDefaults.tail[i];
        }
    }
    return a;
}
/* 0235EEF8 */
void daObjIceisland_c::set_mtx() {
    f32 x = scale.x, y = scale.y, z = scale.z;
    J3DModel *model = mpModel;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    x = current.pos.x;
    y = current.pos.y;
    z = current.pos.z;
    PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
    mDoMtx_stack_c::YrotM((s16)current.angle.y);
    model = mpModel;
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
}
static IceEmitter *spawn(Actor *a, u32 id) {
    IceParticleController *controller = icePlay()->mParticle;
    return iceParticleSet(controller, 0, id, &a->current.pos, &a->current.angle, 0, 255, 0, -1, 0,
                          0, 0);
}
/* 0235EFD0 */
void daObjIceisland_c::daObjIceisland_particle_set() {
    if (!mEmitter1)
        mEmitter1 = spawn(this, 0x81AA);
    if (!mEmitter2)
        mEmitter2 = spawn(this, 0x81AB);
}
/* 0235F0A0 */
void daObjIceisland_c::CreateInit() {
    J3DModel *model = mpModel;
    fopAcM_SetMtx(this, model->getBaseTRMtx());
    cullSizeFar = 1.0f;
    set_mtx();
    iceTevInit(&mTevStr, (s8)home.roomNo, 255);
    auto *light = dKy_getEnvlight();
    iceSettingTev(light, 1, &current.pos, &mTevStr);
    mEmitter1 = 0;
    mEmitter2 = 0;
    if (isSwitch(this)) {
        mBrkAnm.mFrameCtrl.mFrame = (f32)(s16)mBrkAnm.mFrameCtrl.mEnd;
        IceSave *save = iceSave();
        if ((s16)save->mStartPoint == 2 && (s8)current.roomNo == (s8)save->mRestartRoomNo)
            field_0x39C = WaitFailure;
        else
            field_0x39C = Melted;
        field_0x396 = 0;
    } else {
        daObjIceisland_particle_set();
        field_0x39C = Frozen;
        field_0x396 = 100;
    }
    auto *p = icePlay();
    mMeltIceEventIdx = iceEventIdx(&p->mEventManager, STR(0x1002B280), 255);
    p = icePlay();
    mFreezeIceEventIdx = iceEventIdx(&p->mEventManager, STR(0x1002B28C), 255);
}
/* 0235F294 */
s32 daObjIceisland_c::_create() {
    u32 condition = actor_condition;
    if (!(condition & 8)) {
        if (gabi::ea(this)) {
            construct(this);
            condition = actor_condition;
        }
        actor_condition = condition | 8;
    }
    s32 phase = dComIfG_resLoad(&mPhs, STR(0x1002B1EC));
    if (phase == 4) {
        if (!fopAcM_entrySolidHeap(this, checkCreateHeap, 0x13D0))
            return 5;
        CreateInit();
    }
    return phase;
}
/* 0235F34C */
BOOL daObjIceisland_c::_delete() {
    dComIfG_resDelete(&mPhs, STR(0x1002B298));
    return TRUE;
}
/* 0235F37C */
BOOL daObjIceisland_c::_draw() {
    auto *light = dKy_getEnvlight();
    iceSettingTev(light, 1, &current.pos, &mTevStr);
    light = dKy_getEnvlight();
    iceSettingTev(light, 3, &current.pos, &tevStr);
    light = dKy_getEnvlight();
    setLightTevColorType(light, mpModel, &tevStr);
    f32 frame = (f32)mBtkAnm1.mFrameCtrl.mFrame;
    J3DModel *model = mpModel;
    iceBtkEntry(&mBtkAnm1, model, frame);
    frame = (f32)mBtkAnm2.mFrameCtrl.mFrame;
    model = mpModel;
    iceBtkEntry(&mBtkAnm2, model, frame);
    model = mpModel;
    frame = (f32)mBrkAnm.mFrameCtrl.mFrame;
    iceBrkEntry(&mBrkAnm, model->mModelData, frame);
    mDoExt_modelUpdateDL(mpModel, 0);
    return TRUE;
}
/* 0235F420 */
void daObjIceisland_c::daObjIceisland_freeze_main() {
    if (isSwitch(this)) {
        order(this, mMeltIceEventIdx);
        field_0x39C = WaitMelt;
    }
}
static void retire(IceEmitter *emitter) {
    u32 flags = emitter->mFlags;
    emitter->mDeleteID = 0xFFFFFFFF;
    emitter->mFlags = flags | 1;
}
/* 0235F498 */
void daObjIceisland_c::daObjIceisland_melt_demo_wait() {
    if (eventInfo.checkCommandDemoAccrpt()) {
        iceRiddleSound(0x806);
        IceEmitter *emitter = mEmitter1;
        if (emitter) {
            retire(emitter);
            mEmitter1 = 0;
        }
        emitter = mEmitter2;
        if (emitter) {
            retire(emitter);
            mEmitter2 = 0;
        }
        field_0x39C = MeltDemo;
        mBrkAnm.mFrameCtrl.mRate = 1.0f;
    } else
        order(this, mMeltIceEventIdx);
}
static void finishDemo(Actor *a, be<s16> *eventIndex, u32 state) {
    mDoExt_baseAnm_play(&a->mBrkAnm);
    s16 event = *eventIndex;
    auto *p = icePlay();
    if (iceEventEnd(&p->mEventManager, event)) {
        dComIfGp_event_reset();
        a->field_0x39C = state;
    }
}
/* 0235F574 */
void daObjIceisland_c::daObjIceisland_melt_demo() {
    finishDemo(this, &mMeltIceEventIdx, Melted);
}
/* 0235F5E0 */
void daObjIceisland_c::daObjIceisland_melt_main() {
    if (!isSwitch(this)) {
        order(this, mFreezeIceEventIdx);
        field_0x39C = WaitFreeze;
    }
}
/* 0235F658 */
void daObjIceisland_c::daObjIceisland_freeze_demo_wait() {
    if (eventInfo.checkCommandDemoAccrpt()) {
        daObjIceisland_particle_set();
        field_0x39C = FreezeDemo;
        mBrkAnm.mFrameCtrl.mRate = -1.0f;
    } else
        order(this, mFreezeIceEventIdx);
}
/* 0235F6D4 */
void daObjIceisland_c::daObjIceisland_freeze_demo() {
    finishDemo(this, &mFreezeIceEventIdx, Frozen);
}
/* 0235F740 */
void daObjIceisland_c::daObjIceisland_fail_demo_wait() {
    if (!isSwitch(this)) {
        daObjIceisland_particle_set();
        field_0x39C = FailureDemo;
        mBrkAnm.mFrameCtrl.mRate = 1.0f;
    }
}
/* 0235F7AC */
u32 daObjIceisland_c::daObjIceisland_fail_demo_main() {
    return iceAnmPlay(&mBrkAnm);
}
/* 0235F7B4 */
static u8 toSrgb(u32 channel) {
    // HD: convert lighting to sRGB before applying the Ice Ring color bias.
    f32 value = icePow((f32)channel / 255.0f, (f32)l_srgbPower);
    if (value < 0)
        value = 0;
    else if (value > 1)
        value = 1;
    return (u8)gabi::ftoi(value * 255.0f);
}
/* 0235F870 */
static f32 toLinear(u32 channel) {
    // HD: convert the lit emitter color back to linear space.
    f32 value = icePow((f32)channel / 255.0f, (f32)l_linearPower);
    if (value < 0)
        value = 0;
    else if (value > 1)
        value = 1;
    return value;
}
/* 0235F8F0 */
BOOL daObjIceisland_c::_execute() {
    switch ((u32)field_0x39C) {
    case Frozen:
        daObjIceisland_freeze_main();
        break;
    case WaitMelt:
        daObjIceisland_melt_demo_wait();
        break;
    case MeltDemo:
        daObjIceisland_melt_demo();
        break;
    case Melted:
        daObjIceisland_melt_main();
        break;
    case WaitFreeze:
        daObjIceisland_freeze_demo_wait();
        break;
    case FreezeDemo:
        daObjIceisland_freeze_demo();
        break;
    case WaitFailure:
        daObjIceisland_fail_demo_wait();
        break;
    case FailureDemo:
        daObjIceisland_fail_demo_main();
        break;
    }
    mDoExt_baseAnm_play(&mBtkAnm1);
    mDoExt_baseAnm_play(&mBtkAnm2);
    set_mtx();
    BOOL active = isSwitch(this);
    s8 room = current.roomNo;
    s16 volume = field_0x396;
    if (active) {
        if (volume > 0) {
            volume = (s16)(volume - 1);
            field_0x396 = volume;
        }
    } else if (volume < 100) {
        volume = (s16)(volume + 1);
        field_0x396 = volume;
    }
    s32 reverb = dComIfGp_getReverb(room);
    mDoAud_seStart(0x1072, &current.pos, volume, reverb);
    if (mEmitter1) {
        u8 channels[3];
        for (u32 i = 0; i < 3; i++)
            channels[i] = (toSrgb(mTevStr.color(i)) >> 1) + 0x80;
        for (u32 i = 0; i < 3; i++)
            channels[i] = (u8)gabi::ftoi(toLinear((u32)channels[i]) * 255.0f);
        IceEmitter *emitter = mEmitter1;
        for (u32 i = 0; i < 3; i++)
            emitter->mPrmColor[i] = channels[i];
        emitter = mEmitter1;
        for (u32 i = 0; i < 3; i++)
            emitter->mEnvColor[i] = channels[i];
    }
    return TRUE;
}
/* 0235FD7C */
static void staticInit() {
    l_headerZeros.words[2] = 0;
    l_headerZeros.words[0] = 0;
    l_headerZeros.words[3] = 0;
    l_headerZeros.words[1] = 0;
    __register_global_object(gabi::ea(&l_registrationZ));
    l_minAngle = l_piPair.values[0];
    l_maxAngle = l_piPair.values[1];
    iceInitA(&l_initA);
    __register_global_object(gabi::ea(&l_registrationA));
    iceInitB(&l_initB);
    __register_global_object(gabi::ea(&l_registrationB));
}
/* 0235FE10 */
static void trivialDestructor(void *a, s32 flags) {
    if (a && (flags & 1))
        operator_delete(a);
}
/* 0235FE24 */
static BOOL isDelete(Actor *a) {
    return TRUE;
}

/* ---- leftover functions of the translation unit ---- */

/* 0235FE2C daObjIceisland_c::~daObjIceisland_c (deleting; vtable slot 1002B218): only the
 * fopAc_ac_c base */
/* 0235FE2C */
static void daObjIceisland_c_dt(void *p, s32 flags) {
    if (p != nullptr) {
        iceActorDt(p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}

/* 0235FE80 sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 1002B208, after
 * the destructor 0235FE10 */
/* 0235FE80 */
static void iceisland_SafeString_assureTermination(void *p) {
}

// clang-format off
/* WWHD glue
data l_resourceManager 101F4F28
data l_lightDefaults 1016E414
data l_srgbPower 1002B2B0
data l_linearPower 1002B2B8
data l_piPair 1002B2C0
data l_headerZeros 1046A08C
data l_registrationZ 101CA428
data l_minAngle 1046A080
data l_maxAngle 1046A084
data l_initA 1046A088
data l_initB 1046A089
data l_registrationA 101CA434
data l_registrationB 101CA440
ext 026066C4 J3DModelData* iceResource(void*, ResourceName*, s32)
ext 025D7A58 void iceOrder(daObjIceisland_c*,s16,s32,s32,s32,s32)
ext 025E7CE0 s32 iceBtkInit(void*,void*,void*,s32,s32,s32,f32,s32,s32,s32)
ext 025E8154 s32 iceBrkInit(void*,void*,void*,s32,s32,s32,f32,s32,s32,s32)
ext 027F596C void iceModelFlags(J3DModel*,u32)
ext 025E80D0 void iceBrkCtor(IceBrkAnm*)
ext 025A847C IceEmitter* iceParticleSet(IceParticleController*,s32,u32,cXyz*,csXyz*,s32,s32,s32,s32,s32,s32,s32)
ext 0255FFF4 void iceTevInit(IceTevStr*,s8,s32)
ext 025626A4 void iceSettingTev(dScnKy_env_light_c*,s32,cXyz*,void*)
ext 02543F10 s16 iceEventIdx(u8*,const char*,s32)
ext 025440C8 BOOL iceEventEnd(u8*,s16)
ext 025E8048 void iceBtkEntry(mDoExt_btkAnm*,J3DModel*,f32)
ext 025E83FC void iceBrkEntry(IceBrkAnm*,J3DModelData*,f32)
ext 025E742C u32 iceAnmPlay(void*)
ext 028F4560 f32 icePow(f32,f32)
ext 028ED6F8 void iceInitA(void*)
ext 028EAB2C void iceInitB(void*)
ext 025D50BC void iceActorDt(void*,s32)
ext 025E1988 void iceRiddleSound(u32)
*/
// clang-format on
