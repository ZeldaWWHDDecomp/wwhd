/* Transient object smoke effects. */
#include "d/actor/d_a_obj_eff.h"
using daObjEff::Act_c;
using daObjEff::BarrelSmokeCB;
using daObjEff::LandSmokeCB;
using daObjEff::PineconeSmokeCB;
using daObjEff::SkullSmokeCB;
using daObjEff::StoolSmokeCB;
using daObjEff::WoodBoxSmokeCB;
// HD: private emitter fields missing from the shared JPABaseEmitter view.
struct SmokeEmitter {
    u8 _00[8];
    cXyz mEmitterScale;
    cXyz mEmitterTranslation;
    u8 _20[0x34 - 0x20];
    be<f32> mRate;
    u8 _38[0x5C - 0x38];
    be<s32> mMaxFrame;
    be<s16> mLifeTime;
    u8 _62[2];
    be<u16> mVolumeSize;
    u8 _66[2];
    be<f32> mInitialVelOmni;
    be<f32> mInitialVelAxis;
    u8 _70[0x247 - 0x70];
    be<u8> mGlobalAlpha;
};
WWHD_OFFSET(SmokeEmitter, mLifeTime, 0x60);
WWHD_OFFSET(SmokeEmitter, mGlobalAlpha, 0x247);
struct SmokeVtable {
    u8 _00[0x44];
    be<u32> end;
};
struct SmokeDispatch {
    gptr<SmokeVtable> vtable;
};
static void smokeEnd(dPa_smokeEcallBack* callback, JPABaseEmitter* emitter) {
    auto* view = reinterpret_cast<SmokeDispatch*>(callback);
    gabi::call_ptr(view->vtable->end, callback, emitter);
}
using CreateHeapProc = gabi::ptmf<BOOL (Act_c::*)()>;
using SetProc = gabi::ptmf<BOOL (Act_c::*)()>;
using RemoveProc = gabi::ptmf<void (Act_c::*)()>;
using DieProc = gabi::ptmf<void (Act_c::*)()>;
struct CreateHeapTable {
    CreateHeapProc entries[Act_c::Type_COUNT];
};
struct SetTable {
    SetProc entries[Act_c::Type_COUNT];
};
struct RemoveTable {
    RemoveProc entries[Act_c::Type_COUNT];
};
struct DieTable {
    DieProc entries[Act_c::Type_COUNT];
};
static CreateHeapTable create_heap_proc;
static SetTable eff_set_proc;
static RemoveTable remove_proc;
static DieTable die_proc;
struct HeapSizes {
    be<u32> entries[Act_c::Type_COUNT];
};
static HeapSizes heap_size;
struct HeaderZero {
    be<u32> words[4];
};
struct HeaderGuard {
    be<u32> value;
};
struct HeaderFloat {
    be<f32> value;
};
struct HeaderFlag {
    u8 value;
};
struct HeaderRegistration {
    u8 bytes[12];
};
static HeaderZero headerZero;
static HeaderFloat minAngle;
static HeaderFloat maxAngle;
static HeaderFloat originalMinAngle;
static HeaderFloat originalMaxAngle;
static HeaderFlag headerFlag0;
static HeaderFlag headerFlag1;
static HeaderRegistration headerRegistration0;
static HeaderRegistration headerRegistration1;
static HeaderRegistration headerRegistration2;
static void constructSmoke(dPa_smokeEcallBack*, const GXColor*, s32, s32);
extern void constructLandSmoke(dPa_smokeEcallBack*, s32);
extern void* operatorNew(u32);
extern void operatorDelete(void*);
extern BOOL Eff_create_heap(Act_c*);
extern BOOL Eff_set(Act_c*);
extern void Eff_remove(Act_c*);
extern void Eff_die(Act_c*);
extern s32 Eff_create(Act_c*);
extern s32 Eff_delete(Act_c*);
extern s32 Eff_execute(Act_c*);
extern u32 Eff_parameter(Act_c*, u32, u32);
extern u32 Eff_deleteActor(Act_c*);
extern void constructHeader0(void*);
extern void constructHeader1(void*);
extern void registerHeader(void*);
static BOOL solidHeapCB(fopAc_ac_c*);
static SmokeEmitter* particle(Act_c* effect, u16 id, cXyz* scale, cXyz* extra) {
    dPa_followEcallBack* callback = effect->mParticleCallback;
    dPa_control_c* controller = dComIfGp_getParticle();
    return reinterpret_cast<SmokeEmitter*>(controller->set(2, id, &effect->current.pos, nullptr, scale, 180,
                                                           reinterpret_cast<dPa_levelEcallBack*>(callback), -1, nullptr,
                                                           nullptr, extra));
}
static GXColor barrel_original_color;
/* 0233A258 */
BarrelSmokeCB::BarrelSmokeCB() {
    constructSmoke(this, &barrel_original_color, 0, 0);
    field_0x20 = 60;
    __vtbl = 0x10027BF4;
}
/* 0233A2C4 */
void BarrelSmokeCB::execute(JPABaseEmitter* emitter) {
    s32 life = (s32)((u32)(s32)field_0x20 - 1);
    field_0x20 = life;
    if (life <= 0) {
        smokeEnd(this, emitter);
        return;
    }
    if (life < 50)
        reinterpret_cast<SmokeEmitter*>(emitter)->mGlobalAlpha = (u8)gabi::ftoi((f32)life * 3.6f);
}
static GXColor stool_original_color;
/* 0233A35C */
StoolSmokeCB::StoolSmokeCB() {
    constructSmoke(this, &stool_original_color, 0, 0);
    field_0x20 = 60;
    __vtbl = 0x10027C3C;
}
/* 0233A3C8 */
void StoolSmokeCB::execute(JPABaseEmitter* emitter) {
    s32 life = (s32)((u32)(s32)field_0x20 - 1);
    field_0x20 = life;
    if (life <= 0) {
        smokeEnd(this, emitter);
        return;
    }
    if (life < 50)
        reinterpret_cast<SmokeEmitter*>(emitter)->mGlobalAlpha = (u8)gabi::ftoi((f32)life * 3.6f);
}
static GXColor skull_original_color;
/* 0233A460 */
SkullSmokeCB::SkullSmokeCB() {
    constructSmoke(this, &skull_original_color, 0, 0);
    field_0x20 = 60;
    __vtbl = 0x10027C84;
}
/* 0233A4CC */
void SkullSmokeCB::execute(JPABaseEmitter* emitter) {
    s32 life = (s32)((u32)(s32)field_0x20 - 1);
    field_0x20 = life;
    if (life <= 0) {
        smokeEnd(this, emitter);
        return;
    }
    if (life < 50)
        reinterpret_cast<SmokeEmitter*>(emitter)->mGlobalAlpha = (u8)gabi::ftoi((f32)life * 3.6f);
}
/* 0233A564 */
LandSmokeCB::LandSmokeCB() {
    constructLandSmoke(this, 0);
    __vtbl = 0x10027A88;
}
static GXColor pinecone_original_color;
/* 0233A5BC */
PineconeSmokeCB::PineconeSmokeCB() {
    constructSmoke(this, &pinecone_original_color, 0, 0);
    field_0x20 = 60;
    __vtbl = 0x10027CCC;
}
/* 0233A628 */
void PineconeSmokeCB::execute(JPABaseEmitter* emitter) {
    s32 life = (s32)((u32)(s32)field_0x20 - 1);
    field_0x20 = life;
    if (life <= 0) {
        smokeEnd(this, emitter);
        return;
    }
    if (life < 50)
        reinterpret_cast<SmokeEmitter*>(emitter)->mGlobalAlpha = (u8)gabi::ftoi((f32)life * 3.6f);
}
static GXColor woodbox_original_color;
/* 0233A6C0 */
WoodBoxSmokeCB::WoodBoxSmokeCB() {
    constructSmoke(this, &woodbox_original_color, 0, 0);
    field_0x20 = 60;
    __vtbl = 0x10027D14;
}
/* 0233A72C */
void WoodBoxSmokeCB::execute(JPABaseEmitter* emitter) {
    s32 life = (s32)((u32)(s32)field_0x20 - 1);
    field_0x20 = life;
    if (life <= 0) {
        smokeEnd(this, emitter);
        return;
    }
    if (life < 50)
        reinterpret_cast<SmokeEmitter*>(emitter)->mGlobalAlpha = (u8)gabi::ftoi((f32)life * 3.6f);
}
/* 0233A7C4 */
BOOL Act_c::create_heap() {
    return (this->*create_heap_proc.entries[mType])() != 0;
}
/* 0233A858 */
BOOL Act_c::eff_set() {
    return (this->*eff_set_proc.entries[mType])();
}
/* 0233A8A4 */
s32 Act_c::_create() {
    mType = Eff_parameter(this, PRM_TYPE_W, PRM_TYPE_S);
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this) {
            fopAc_ac_c_ct(this);
            __vtbl = 0x10027A78;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    u32 heap = heap_size.entries[mType];
    if (!fopAcM_entrySolidHeap(this, solidHeapCB, heap))
        return cPhs_ERROR_e;
    if (!Eff_set(this))
        return cPhs_ERROR_e;
    fopAcM_offDraw(this);
    return cPhs_COMPLEATE_e;
}
/* 0233A968 */
void Act_c::remove() {
    (this->*remove_proc.entries[mType])();
}
/* 0233A9B4 */
s32 Act_c::_delete() {
    Eff_remove(this);
    return TRUE;
}
/* 0233A9D8 */
void Act_c::die() {
    (this->*die_proc.entries[mType])();
}
/* 0233AA24 */
s32 Act_c::_execute() {
    Eff_die(this);
    return TRUE;
}
extern BarrelSmokeCB* construct_BarrelSmokeCB(BarrelSmokeCB*);
/* 0233AA48 */
BOOL Act_c::create_heap_barrel_smoke() {
    auto* callback = static_cast<BarrelSmokeCB*>(operatorNew(sizeof(BarrelSmokeCB)));
    if (callback)
        callback = construct_BarrelSmokeCB(callback);
    mParticleCallback = callback;
    return callback != nullptr;
}
static cXyz barrelScale;
static HeaderGuard barrelScaleGuard;
/* 0233ABF8 */
BOOL Act_c::eff_set_barrel_smoke() {
    if (!barrelScaleGuard.value) {
        barrelScale.x = 2.5f;
        barrelScale.z = 1.f;
        barrelScale.y = 2.5f;
        barrelScaleGuard.value = 1;
    }
    SmokeEmitter* emitter = particle(this, 0x2027, nullptr, &barrelScale);
    if (!emitter)
        return 0;
    emitter->mMaxFrame = 1;
    emitter->mInitialVelOmni = 15.f;
    emitter->mInitialVelAxis = 10.f;
    emitter->mRate = 20.f;
    emitter->mVolumeSize = 40;
    return 1;
}
/* 0233B258 */
void Act_c::remove_barrel_smoke() {
    dPa_followEcallBack* callback = mParticleCallback;
    if (callback)
        callback->remove();
}
/* 0233B300 */
u32 Act_c::die_barrel_smoke() {
    // HD: preserve r3 when the void GameCube method falls through.
    if (mParticleCallback->m10 & 1)
        return Eff_deleteActor(this);
    return gabi::ea(this);
}
extern StoolSmokeCB* construct_StoolSmokeCB(StoolSmokeCB*);
/* 0233AA90 */
BOOL Act_c::create_heap_stool_smoke() {
    auto* callback = static_cast<StoolSmokeCB*>(operatorNew(sizeof(StoolSmokeCB)));
    if (callback)
        callback = construct_StoolSmokeCB(callback);
    mParticleCallback = callback;
    return callback != nullptr;
}
static cXyz stoolScale;
static HeaderGuard stoolScaleGuard;
/* 0233AD0C */
BOOL Act_c::eff_set_stool_smoke() {
    if (!stoolScaleGuard.value) {
        stoolScale.x = 2.5f;
        stoolScale.z = 1.f;
        stoolScale.y = 2.5f;
        stoolScaleGuard.value = 1;
    }
    SmokeEmitter* emitter = particle(this, 0x2027, nullptr, &stoolScale);
    if (!emitter)
        return 0;
    emitter->mMaxFrame = 1;
    emitter->mVolumeSize = 30;
    emitter->mInitialVelAxis = 10.f;
    emitter->mInitialVelOmni = 15.f;
    emitter->mRate = 15.f;
    return 1;
}
/* 0233B274 */
void Act_c::remove_stool_smoke() {
    dPa_followEcallBack* callback = mParticleCallback;
    if (callback)
        callback->remove();
}
/* 0233B314 */
u32 Act_c::die_stool_smoke() {
    // HD: preserve r3 when the void GameCube method falls through.
    if (mParticleCallback->m10 & 1)
        return Eff_deleteActor(this);
    return gabi::ea(this);
}
extern SkullSmokeCB* construct_SkullSmokeCB(SkullSmokeCB*);
/* 0233AAD8 */
BOOL Act_c::create_heap_skull_smoke() {
    auto* callback = static_cast<SkullSmokeCB*>(operatorNew(sizeof(SkullSmokeCB)));
    if (callback)
        callback = construct_SkullSmokeCB(callback);
    mParticleCallback = callback;
    return callback != nullptr;
}
static cXyz skullScale;
static HeaderGuard skullScaleGuard;
/* 0233AE18 */
BOOL Act_c::eff_set_skull_smoke() {
    if (!skullScaleGuard.value) {
        skullScale.x = 1.6f;
        skullScale.z = 1.f;
        skullScale.y = 1.6f;
        skullScaleGuard.value = 1;
    }
    SmokeEmitter* emitter = particle(this, 0x2027, nullptr, &skullScale);
    if (!emitter)
        return 0;
    emitter->mMaxFrame = 1;
    emitter->mInitialVelOmni = 8.f;
    emitter->mRate = 10.f;
    return 1;
}
/* 0233B290 */
void Act_c::remove_skull_smoke() {
    dPa_followEcallBack* callback = mParticleCallback;
    if (callback)
        callback->remove();
}
/* 0233B328 */
u32 Act_c::die_skull_smoke() {
    // HD: preserve r3 when the void GameCube method falls through.
    if (mParticleCallback->m10 & 1)
        return Eff_deleteActor(this);
    return gabi::ea(this);
}
extern LandSmokeCB* construct_LandSmokeCB(LandSmokeCB*);
/* 0233AB20 */
BOOL Act_c::create_heap_land_smoke() {
    auto* callback = static_cast<LandSmokeCB*>(operatorNew(sizeof(LandSmokeCB)));
    if (callback)
        callback = construct_LandSmokeCB(callback);
    mParticleCallback = callback;
    return callback != nullptr;
}
static cXyz landScale;
static HeaderGuard landScaleGuard;
static cXyz landTranslation;
static HeaderGuard landTranslationGuard;
/* 0233AF18 */
BOOL Act_c::eff_set_land_smoke() {
    gabi::Local<cXyz> particleScale;
    *particleScale = scale;
    SmokeEmitter* emitter = particle(this, 0x2027, particleScale.get(), nullptr);
    if (!emitter)
        return 0;
    emitter->mVolumeSize = 30;
    emitter->mInitialVelOmni = 10.f;
    emitter->mRate = 20.f;
    emitter->mMaxFrame = 1;
    emitter->mLifeTime = 35;
    if (!landScaleGuard.value) {
        landScaleGuard.value = 1;
        landScale.y = 0.f;
        landScale.z = 1.f;
        landScale.x = 1.f;
    }
    emitter->mEmitterScale.x = (f32)landScale.x;
    emitter->mEmitterScale.z = (f32)landScale.z;
    emitter->mEmitterScale.y = (f32)landScale.y;
    if (!landTranslationGuard.value) {
        landTranslation.z = 0.f;
        landTranslation.x = 0.f;
        landTranslationGuard.value = 1;
        landTranslation.y = 5.f;
    }
    emitter->mEmitterTranslation.z = (f32)landTranslation.z;
    emitter->mEmitterTranslation.x = (f32)landTranslation.x;
    emitter->mEmitterTranslation.y = (f32)landTranslation.y;
    return 1;
}
/* 0233B2AC */
void Act_c::remove_land_smoke() {
    dPa_followEcallBack* callback = mParticleCallback;
    if (callback)
        callback->remove();
}
/* 0233B33C */
u32 Act_c::die_land_smoke() {
    // HD: preserve r3 when the void GameCube method falls through.
    if (mParticleCallback->m10 & 1)
        return Eff_deleteActor(this);
    return gabi::ea(this);
}
extern PineconeSmokeCB* construct_PineconeSmokeCB(PineconeSmokeCB*);
/* 0233AB68 */
BOOL Act_c::create_heap_pinecone_smoke() {
    auto* callback = static_cast<PineconeSmokeCB*>(operatorNew(sizeof(PineconeSmokeCB)));
    if (callback)
        callback = construct_PineconeSmokeCB(callback);
    mParticleCallback = callback;
    return callback != nullptr;
}
/* 0233B0E8 */
BOOL Act_c::eff_set_pinecone_smoke() {
    return particle(this, 0xA16B, nullptr, nullptr) != 0;
}
/* 0233B2C8 */
void Act_c::remove_pinecone_smoke() {
    dPa_followEcallBack* callback = mParticleCallback;
    if (callback)
        callback->remove();
}
/* 0233B350 */
u32 Act_c::die_pinecone_smoke() {
    // HD: preserve r3 when the void GameCube method falls through.
    if (mParticleCallback->m10 & 1)
        return Eff_deleteActor(this);
    return gabi::ea(this);
}
extern WoodBoxSmokeCB* construct_WoodBoxSmokeCB(WoodBoxSmokeCB*);
/* 0233ABB0 */
BOOL Act_c::create_heap_woodBox_smoke() {
    auto* callback = static_cast<WoodBoxSmokeCB*>(operatorNew(sizeof(WoodBoxSmokeCB)));
    if (callback)
        callback = construct_WoodBoxSmokeCB(callback);
    mParticleCallback = callback;
    return callback != nullptr;
}
static cXyz woodBoxScale;
static HeaderGuard woodBoxScaleGuard;
/* 0233B164 */
BOOL Act_c::eff_set_woodBox_smoke() {
    if (!woodBoxScaleGuard.value) {
        woodBoxScale.x = 2.5f;
        woodBoxScale.z = 1.f;
        woodBoxScale.y = 2.5f;
        woodBoxScaleGuard.value = 1;
    }
    SmokeEmitter* emitter = particle(this, 0x2027, nullptr, &woodBoxScale);
    if (!emitter)
        return 0;
    emitter->mMaxFrame = 1;
    emitter->mRate = 30.f;
    return 1;
}
/* 0233B2E4 */
void Act_c::remove_woodBox_smoke() {
    dPa_followEcallBack* callback = mParticleCallback;
    if (callback)
        callback->remove();
}
/* 0233B364 */
u32 Act_c::die_woodBox_smoke() {
    // HD: preserve r3 when the void GameCube method falls through.
    if (mParticleCallback->m10 & 1)
        return Eff_deleteActor(this);
    return gabi::ea(this);
}
/* 0233A854 */
static BOOL solidHeapCB(fopAc_ac_c* actor) {
    return Eff_create_heap(static_cast<Act_c*>(actor));
}
/* 0233B378 */
static s32 Mthd_Create(Act_c* actor) {
    return Eff_create(actor);
}
/* 0233B37C */
static s32 Mthd_Delete(Act_c* actor) {
    return Eff_delete(actor);
}
/* 0233B380 */
static s32 Mthd_Execute(Act_c* actor) {
    return Eff_execute(actor);
}
/* 0233B384 */
static BOOL Mthd_IsDelete(Act_c* actor) {
    return TRUE;
}
/* 0233B47C */
static BOOL Mthd_Draw(Act_c* actor) {
    return TRUE;
}
/* 0233B420 */
static void smokeCallback_noop(void* callback) {}
/* 0233B424 */
static void smokeCallback_draw(void* callback, JPABaseEmitter* emitter) {}
/* 0233B428 */
static void Act_destruct(Act_c* actor, s32 flags) {
    if (actor) {
        __dt(static_cast<fopAc_ac_c*>(actor));
        if (flags & 1)
            operatorDelete(actor);
    }
}
/* 0233B484 */
static u32 PrmAbstract(Act_c* actor, u32 bits, u32 shift) {
    u32 mask = (bits & 32) ? 0u : 1u << (bits & 31);
    u32 value = (shift & 32) ? 0u : (u32)actor->mParameters >> (shift & 31);
    return value & (mask - 1);
}
/* 0233B38C */
static void __sinit_d_a_obj_eff_cpp() {
    for (u32 i = 0; i < 4; i++)
        headerZero.words[i] = 0;
    registerHeader(&headerRegistration0);
    minAngle.value = (f32)originalMinAngle.value;
    maxAngle.value = (f32)originalMaxAngle.value;
    constructHeader0(&headerFlag0);
    registerHeader(&headerRegistration1);
    constructHeader1(&headerFlag1);
    registerHeader(&headerRegistration2);
}

/* WWHD glue
data create_heap_proc 10027ADC
data eff_set_proc 10027B0C
data heap_size 10027B3C
data remove_proc 10027B54
data die_proc 10027B84
data headerZero 10469734
data minAngle 10469728
data maxAngle 1046972C
data originalMinAngle 10027BD8
data originalMaxAngle 10027BDC
data headerFlag0 10469730
data headerFlag1 10469731
data headerRegistration0 101C8F78
data headerRegistration1 101C8F84
data headerRegistration2 101C8F90
data barrel_original_color 10027BE0
data stool_original_color 10027BE4
data skull_original_color 10027BE8
data pinecone_original_color 10027BEC
data woodbox_original_color 10027BF0
ext 0233A258 BarrelSmokeCB* construct_BarrelSmokeCB(BarrelSmokeCB*)
data barrelScale 10469744
data barrelScaleGuard 10469774
ext 0233A35C StoolSmokeCB* construct_StoolSmokeCB(StoolSmokeCB*)
data stoolScale 10469750
data stoolScaleGuard 10469778
ext 0233A460 SkullSmokeCB* construct_SkullSmokeCB(SkullSmokeCB*)
data skullScale 1046975C
data skullScaleGuard 1046977C
ext 0233A564 LandSmokeCB* construct_LandSmokeCB(LandSmokeCB*)
data landScale 10469788
data landScaleGuard 10469780
data landTranslation 10469794
data landTranslationGuard 10469784
ext 0233A5BC PineconeSmokeCB* construct_PineconeSmokeCB(PineconeSmokeCB*)
ext 0233A6C0 WoodBoxSmokeCB* construct_WoodBoxSmokeCB(WoodBoxSmokeCB*)
data woodBoxScale 10469768
data woodBoxScaleGuard 104697A0
ext 025A5CEC void constructSmoke(dPa_smokeEcallBack*, const GXColor*, s32, s32)
ext 025A5B18 void constructLandSmoke(dPa_smokeEcallBack*, s32)
ext 0273AD10 void* operatorNew(u32)
ext 0273AF40 void operatorDelete(void*)
ext 0233A7C4 BOOL Eff_create_heap(Act_c*)
ext 0233A858 BOOL Eff_set(Act_c*)
ext 0233A968 void Eff_remove(Act_c*)
ext 0233A9D8 void Eff_die(Act_c*)
ext 0233A8A4 s32 Eff_create(Act_c*)
ext 0233A9B4 s32 Eff_delete(Act_c*)
ext 0233AA24 s32 Eff_execute(Act_c*)
ext 0233B484 u32 Eff_parameter(Act_c*, u32, u32)
ext 025D57E0 u32 Eff_deleteActor(Act_c*)
ext 028ED6F8 void constructHeader0(void*)
ext 028EAB2C void constructHeader1(void*)
ext 028F026C void registerHeader(void*)
*/
