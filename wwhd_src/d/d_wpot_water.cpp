/* Spilled water effect, verified WWHD behavior. */
#include "d/d_wpot_water.h"

static f32 surfaceOffset;
static f32 waterScale;
static f32 waterHeight;
static f32 invalidGround;
static f32 probeStep;
static f32 lavaRaise;
static f32 splashOffset;
static f32 alphaReference;
static f32 initialLow;
static f32 initialHigh;
static Mtx34 viewMatrix;
static WpotSinCosTable sincosTable;
static dWpotWater_EcallBack mEcallback;
static WpotInitWords initializedWords;
static WpotInitFloats initializedFloats;
static WpotRegistration wordsRegistration;
static WpotRegistration byteRegistration;
static WpotRegistration wideRegistration;
static WpotRegistration callbackRegistration;
static u8 byteStringState;
static u8 wideStringState;
static u32 actorVtable;
static u32 packetVtable;
static u32 callbackVtable;
static u32 groundChkVtable;
static u32 groundPolyVtable;
static u32 groundPassVtable;
static u32 groundGroupVtable;
static u32 lavaChkVtable;
static u32 lavaPolyVtable;
static u32 lavaPassVtable;
static u32 lavaGroupVtable;
static u32 groundGroupDtorVtable;

/* 025D3950 */
// HD: distinct deleting-destructor entry preserves the forwarded flags register.
static void wpotCallbackDeletingDtor(void* object, u32 flags) {
    if (object != NULL && (flags & WpotDestructor_Delete)) wpotDeleteStorage(object, flags);
}

/* 025D3964 */
static void wpotPacketNoop(void* packet) {}

/* 025D3968 */
BOOL dWpotWater_c::draw() {
    WpotDrawListView* game = reinterpret_cast<WpotDrawListView*>(dComIfGp_get());
    wpotDrawEntry(game->mpXluList, &mPacket, 0);
    return TRUE;
}

/* 025D39A8 */
BOOL dWpotWater_c::execute() {
    mDoMtx_stack_c::transS(mPos.x, gabi::fadds_ppc(mPos.y, surfaceOffset), mPos.z);
    f32 scale = waterScale;
    mDoMtx_stack_c::scaleM(scale, waterHeight, scale);
    wpotMatrixConcat(&viewMatrix, mDoMtx_stack_c::get(), &mPacket.mtx);
    WpotEmitter* emitter = emtr;
    // HD: unsigned count addition and its wrap are part of the delete test.
    if ((emitter->mStatus & WpotEmitter_Delete) &&
        u32(u32(emitter->mAliveParticleCount) + u32(emitter->mAliveChildCount)) == 0) {
        emitter->mStatus = u32(emitter->mStatus) & ~WpotEmitter_Immortal;
        wpotKankyoDelete(this);
    }
    return TRUE;
}

/* 025D3A60 */
static BOOL dWpotWater_IsDelete(dWpotWater_c* i_this) { return TRUE; }

/* 025D3A68 */
static BOOL dWpotWater_Delete(dWpotWater_c* i_this) {
    // HD: packet and environment base destruction are explicit.
    if (i_this != NULL) {
        wpotPacketDestruct(&i_this->mPacket, 0);
        wpotBaseDestruct(i_this, 0);
    }
    return TRUE;
}

static void wpotGroundSetup(dBgS_GndChk* check, bool lava) {
    wpotGroundConstruct(check);
    check->mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(check));
    check->mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(check));
    static_cast<cBgS_Chk*>(check)->__vtbl = gabi::ea(lava ? &lavaChkVtable : &groundChkVtable);
    static_cast<cBgS_PolyInfo*>(check)->__vtbl = gabi::ea(lava ? &lavaPolyVtable : &groundPolyVtable);
    check->mbObjThrough = 0;
    check->mbCamThrough = 0;
    check->mbLinkThrough = 0;
    check->mbArrowThrough = 0;
    check->mbBombThrough = 0;
    check->mbBoomerangThrough = 0;
    check->mbRopeThrough = 0;
    if (lava) check->mbObjThrough = 1;
    static_cast<dBgS_PolyPassChk*>(check)->__vtbl = gabi::ea(lava ? &lavaPassVtable : &groundPassVtable);
    static_cast<dBgS_GrpPassChk*>(check)->__vtbl = gabi::ea(lava ? &lavaGroupVtable : &groundGroupVtable);
    check->mGrp = lava ? WpotGround_Lava : WpotGround_Normal;
}

static void wpotGroundDestroy(dBgS_GndChk* lava, dBgS_GndChk* ground) {
    for (dBgS_GndChk* check : {lava, ground}) {
        static_cast<cBgS_PolyInfo*>(check)->__vtbl = gabi::ea(&groundPolyVtable);
        static_cast<dBgS_PolyPassChk*>(check)->__vtbl = gabi::ea(&groundPassVtable);
        static_cast<dBgS_GrpPassChk*>(check)->__vtbl = gabi::ea(&groundGroupDtorVtable);
        wpotGroundDestruct(check, 0);
    }
}

/* 025D3AB0 */
cPhs_State dWpotWater_c::create() {
    // HD: the ground-probe wrapper and actor creation are one entry point.
    gabi::Local<dBgS_GndChk> ground;
    wpotGroundSetup(ground.get(), false);
    gabi::Local<dBgS_GndChk> lava;
    wpotGroundSetup(lava.get(), true);
    f32 sentinel = invalidGround;
    f32 step = probeStep;
    f32 up = waterHeight;
    f32 raise = lavaRaise;
    for (u32 i = 0; i < 50; ++i) {
        u32 index = u16(i * 10000) >> 3;
        f32 radius = gabi::fmuls_ppc(f32(i), step);
        f32 x = gabi::fmadds(radius, sincosTable.entries[index].cosine, mPos.x);
        f32 y = gabi::fadds_ppc(mPos.y, up);
        f32 z = gabi::fmadds(radius, sincosTable.entries[index].sine, mPos.z);
        lava->m_pos.x = x;
        lava->m_pos.y = y;
        lava->m_pos.z = z;
        // HD: ground-query returns stay double precision until their explicit stores.
        f64 lavaY = wpotGroundCross(dComIfG_Bgsp(), lava.get());
        if (lavaY == sentinel) continue;
        ground->m_pos.x = x;
        ground->m_pos.y = y;
        ground->m_pos.z = z;
        f64 groundY = wpotGroundCross(dComIfG_Bgsp(), ground.get());
        if (lava->mPolyIndex == WpotGround_InvalidPoly || lava->mBgIndex == WpotGround_InvalidBg) continue;
        if (wpotGetAttribute(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(lava.get())) != WpotGround_LavaAttribute
            || !(lavaY > groundY)) continue;
        gabi::Local<cXyz> spawnPos;
        spawnPos->x = x;
        spawnPos->y = f32(lavaY + raise);
        spawnPos->z = z;
        fopAcM_create(fpcNm_Obj_Magmarock_e, 0, spawnPos.get(), mParam, NULL, NULL, -1, 0);
        break;
    }
    ground->m_pos.x = mPos.x;
    ground->m_pos.y = gabi::fadds_ppc(mPos.y, up);
    ground->m_pos.z = mPos.z;
    f64 groundY = wpotGroundCross(dComIfG_Bgsp(), ground.get());
    mPos.y = f32(groundY);
    if (groundY == sentinel) {
        wpotGroundDestroy(lava.get(), ground.get());
        return cPhs_ERROR_e;
    }
    gabi::Local<cXyz> pos;
    pos->x = mPos.x;
    pos->y = f32(groundY);
    pos->z = mPos.z;
    fopAcM_create(fpcNm_HITOBJ_e, 0, pos.get(), mParam, NULL, NULL, -1, 0);
    if (gabi::ea(this) != 0) {
        wpotBaseConstruct(this);
        base.__vtbl = gabi::ea(&actorVtable);
        wpotPacketConstruct(&mPacket);
        mPacket.__vtbl = gabi::ea(&packetVtable);
    }
    // HD: the first splash is raised independently of the actor's grounded position.
    gabi::Local<cXyz> splash;
    splash->x = mPos.x;
    splash->y = gabi::fadds_ppc(mPos.y, splashOffset);
    splash->z = mPos.z;
    dComIfGp_particle_set(ID_IT_SN_WPOT_BITYA, splash.get());
    dComIfGp_particle_set(ID_IT_SN_WPOT_SHIBUKI, &mPos);
    JPABaseEmitter* emitter = dComIfGp_particle_set(ID_IT_SN_WPOT_YUKA, &mPos, NULL, NULL,
        0xAA, reinterpret_cast<dPa_levelEcallBack*>(&mEcallback));
    emtr = reinterpret_cast<WpotEmitter*>(emitter);
    if (emitter != NULL) emtr->mStatus = u32(emtr->mStatus) | WpotEmitter_Immortal;
    wpotGroundDestroy(lava.get(), ground.get());
    return emitter != NULL ? cPhs_COMPLEATE_e : cPhs_ERROR_e;
}

/* 025D4124 */
void dWpotWater_EcallBack::draw(WpotEmitter* emtr) {
    if (!(emtr->mDrawFlags & WpotEmitter_ZDraw)) return;
    // HD: the GX setup is expressed through an expanded draw-state object.
    gabi::Local<WpotDrawState> state;
    wpotDrawStateConstruct(state.get());
    u32 flags = state->mFlags;
    u32 mode = state->mBlendMode;
    state->mAlphaCompare = WpotRender_Always;
    state->mAlphaReference = alphaReference;
    state->mDepthCompare = WpotRender_LessEqual;
    state->mFlags = flags | WpotRender_StateMask;
    state->mColorUpdate = 0;
    state->mDepthUpdate = 0;
    state->mBlendFactors[0] = WpotRender_DestinationAlpha;
    state->mAlphaUpdate = 1;
    state->mBlendFactors[1] = WpotRender_DestinationAlpha;
    state->mBlendFactors[3] = WpotRender_InverseDestinationAlpha;
    state->mBlendFactors[7] = WpotRender_DestinationAlpha;
    state->mBlendFactors[2] = WpotRender_InverseDestinationAlpha;
    state->mBlendFactors[6] = WpotRender_DestinationAlpha;
    state->mBlendFactors[8] = WpotRender_InverseDestinationAlpha;
    state->mBlendMode = (((mode & ~WpotRender_ModeLowMask) + WpotRender_ModeLowValue) & ~WpotRender_ModeHighMask) + WpotRender_ModeHighValue;
    state->mBlendFactors[9] = WpotRender_InverseDestinationAlpha;
    wpotApplyBlend(state.get());
    wpotApplyDepth(state.get());
    wpotApplyCompare(state.get());
    wpotApplyColor(state.get());
}

/* 025D41F0 */
static void __sinit_d_wpot_water_cpp() {
    // HD: common header statics and callback registration.
    for (u32 i = 0; i < 4; ++i) initializedWords.values[i] = 0;
    wpotRegisterGlobal(&wordsRegistration);
    initializedFloats.low = initialLow;
    initializedFloats.high = initialHigh;
    wpotInitializeByteState(&byteStringState);
    wpotRegisterGlobal(&byteRegistration);
    wpotInitializeWideState(&wideStringState);
    wpotRegisterGlobal(&wideRegistration);
    mEcallback.__vtbl = gabi::ea(&callbackVtable);
    wpotRegisterGlobal(&callbackRegistration);
}

/* 025D42A0 */
static void wpotCallbackExecuteNoop(void* callback) {}
/* 025D42A4 */
static void wpotCallbackSetupNoop(void* callback) {}
/* 025D42A8 */
// HD: the second callback deleting-destructor entry retains the forwarded flags.
static void wpotCallbackDelete(void* object, u32 flags) {
    if (object != NULL && (flags & WpotDestructor_Delete)) wpotDeleteStorage(object, flags);
}
/* 025D42BC */
static void wpotCallbackDrawNoop(void* callback) {}
/* 025D42C0 */
static void wpotCallbackCleanupNoop(void* callback) {}

/* WWHD glue
data surfaceOffset 10056D84
data waterScale 10056D88
data waterHeight 10056D8C
data invalidGround 10056D90
data probeStep 10056D94
data lavaRaise 10056D98
data splashOffset 10056DA8
data alphaReference 10056DAC
data initialLow 10056DB4
data initialHigh 10056DB8
data viewMatrix 104B45F8
data sincosTable 104A44F8
data mEcallback 104873AC
data initializedWords 1048739C
data initializedFloats 10487390
data wordsRegistration 101F2FE0
data byteRegistration 101F2FEC
data wideRegistration 101F2FF8
data callbackRegistration 101F3004
data byteStringState 10487398
data wideStringState 10487399
data actorVtable 10056CA4
data packetVtable 1004CC38
data callbackVtable 10056DC0
data groundChkVtable 10056CC4
data groundPolyVtable 10056CD4
data groundPassVtable 10056CF4
data groundGroupVtable 10056CE4
data lavaChkVtable 10056D44
data lavaPolyVtable 10056D54
data lavaPassVtable 10056D74
data lavaGroupVtable 10056D64
data groundGroupDtorVtable 10056CB4
ext 0273AF40 void wpotDeleteStorage(void*, u32)
ext 027F0E04 void wpotDrawEntry(void*, dDlst_alphaVolPacket*, u32)
ext 025DAD48 void wpotKankyoDelete(dWpotWater_c*)
ext 027F13DC void wpotPacketDestruct(dDlst_alphaVolPacket*, u32)
ext 025DD630 void wpotBaseDestruct(dWpotWater_c*, u32)
ext 02008E0C void wpotGroundConstruct(dBgS_GndChk*)
ext 02008DAC void wpotGroundDestruct(dBgS_GndChk*, u32)
ext 02008974 f64 wpotGroundCross(dBgS*, dBgS_GndChk*)
ext 024EF0F4 s32 wpotGetAttribute(dBgS*, cBgS_PolyInfo*)
ext 025DD5F0 void wpotBaseConstruct(dWpotWater_c*)
ext 027F1278 void wpotPacketConstruct(dDlst_alphaVolPacket*)
ext 028E9108 void wpotMatrixConcat(Mtx34*, Mtx34*, Mtx34*)
ext 02750250 void wpotDrawStateConstruct(WpotDrawState*)
ext 027505B4 void wpotApplyBlend(WpotDrawState*)
ext 02750520 void wpotApplyDepth(WpotDrawState*)
ext 02750534 void wpotApplyCompare(WpotDrawState*)
ext 027505DC void wpotApplyColor(WpotDrawState*)
ext 028F026C void wpotRegisterGlobal(WpotRegistration*)
ext 028ED6F8 void wpotInitializeByteState(void*)
ext 028EAB2C void wpotInitializeWideState(void*)
*/
