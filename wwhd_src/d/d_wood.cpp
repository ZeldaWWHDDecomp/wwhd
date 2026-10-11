// Wood and bush animation, collision and HD GX2 rendering.
#include "d/d_wood.h"
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/m_Do/m_Do_audio.h"
#include <bit>
// HD: forward declaration for autoglue's namespace-qualified allocation wrapper.
namespace dWood {
static Anm_c *Anm_c__ct(Anm_c *);
static Unit_c *Unit_c__ct(Unit_c *);
static Room_c *Room_c__ct(Room_c *);
} // namespace dWood
static f32 woodZero = 0.0f;
static f32 woodCutInitVelY = 18.0f;
static f32 woodCutYAccel = -3.0f;
static f32 woodCutMinVelY = -40.0f;
static f32 woodCutZVel = 2.5f;
static f32 woodPushBackTimeScale = 0.02857142857142857f;
static f32 woodWindMedium = 0.33f;
static f32 woodWindStrong = 0.66f;
// HD: class-static guest storage is mapped by its unqualified name for autoglue.
static s32 M_init_num = 0;
static WoodModeProcEntry mode_proc[6] = {
    {&dWood::Anm_c::mode_cut}, {&dWood::Anm_c::mode_push_into}, {&dWood::Anm_c::mode_push_back},
    {&dWood::Anm_c::mode_fan}, {&dWood::Anm_c::mode_norm},      {&dWood::Anm_c::mode_to_norm},
};
static dWood::AttrSway_c L_attr_kSways[4][2] = {
    {{200, 80, 1500, 50, 0.6f}, {180, 30, 3200, 10, 0.2f}},
    {{500, 150, 1200, 150, 0.6f}, {700, 50, 2200, 30, 0.2f}},
    {{600, 200, 2200, 300, 0.6f}, {444, 30, 4000, 50, 0.2f}},
    {{600, 200, 6200, 500, 0.6f}, {444, 30, 1000, 50, 0.2f}},
};

namespace d_wood_cpp {

static f32 add(f32 a, f32 b) { return gabi::fadds_ppc(a, b); }
static void zeroPhases(dWood::Anm_c *anim) {
    for (u32 i = 0; i < 2; ++i) {
        anim->mPhaseY[i] = 0;
        anim->mPhaseX[i] = 0;
        anim->mAmpY[i] = 0;
        anim->mAmpX[i] = 0;
    }
}



static f32 cosine(u32 phase) { return cM_scos(phase); }
static void applySway(dWood::Anm_c *anim, f32 rotY, f32 rotX) {
    cMtx_YrotS(gabi::ea(anim->mModelMtx),
               (s32)(s16)((u32)gabi::ftoi(rotY) + (s32)(s16)anim->mForceDir));
    cMtx_XrotM(gabi::ea(anim->mModelMtx), (s32)(s16)gabi::ftoi(rotX));
    cMtx_YrotM(gabi::ea(anim->mModelMtx), (s32)(s16) - (s16)anim->mForceDir);
}

} // namespace d_wood_cpp
using namespace d_wood_cpp;
static s32 anm_norm_num;
/* 025CD868 */
s32 dWood::Packet_c::search_anm(u32 mode) {
    if (mode >= Anm_c::Mode_Max)
        wood_assert(gabi::guest_addr("d_wood.cpp"), 0xCB5u,
                    gabi::guest_addr("(i_mode >= 0) && (i_mode < Anm_c::Mode_Max)"));
    if (mode == Anm_c::Mode_Norm) {
        const u32 index = (u32)anm_norm_num;
        anm_norm_num = (s32)(index + 1) % AnmID_Norm_Max;
        return (s32)index;
    }
    for (u32 i = AnmID_Norm_Max; i < 72; ++i)
        if ((s32)mAnm[i].mMode == Anm_c::Mode_Max)
            return i;
    for (u32 i = AnmID_Norm_Max; i < 72; ++i)
        if ((s32)mode < (s32)mAnm[i].mMode)
            return i;
    return AnmID_Norm_Invalid;
}
static s32 hdWood_searchAnimation(u32 self, u32 mode) {
    return reinterpret_cast<dWood::Packet_c *>((uintptr_t)self)->search_anm(mode);
}

namespace d_wood_cpp {


} // namespace d_wood_cpp

/* 025CD26C */
dWood::Anm_c::Anm_c() {
    using namespace d_wood_cpp;
    PSMTXIdentity(gabi::ea(this));
    PSMTXIdentity(gabi::ea(mTrunkModelMtx));
    f32 zero = woodZero;
    mCountdown = (0);
    mVelY = (zero);
    mWindPow = (zero);
    mForceDir = (0);
    mMode = Mode_Max;
    mPosOffsetZ = (zero);
    mPosOffsetY = (zero);
    zeroPhases(this);
    mNormAnimIdx = (0);
    mAlphaScale = (255);
}

/* 025CD32C */
void dWood::Anm_c::play(dWood::Packet_c *packet) {
    using namespace d_wood_cpp;

    // HD: GHS stores compact guest descriptors, preserving virtual and adjusted-this dispatch.
    if ((s32)mMode == Mode_Max)
        return;
    (this->*mode_proc[(u32)mMode].fn)(packet);
}

/* 025CD380 */
void dWood::Anm_c::copy_angamp(const dWood::Anm_c *from) {
    using namespace d_wood_cpp;

    if (this == from)
        return;
    for (u32 i = 0; i < 2; ++i) {
        mPhaseY[i] = from->mPhaseY[i];
        mPhaseX[i] = from->mPhaseX[i];
        mAmpY[i] = from->mAmpY[i];
        mAmpX[i] = from->mAmpX[i];
    }
}

/* 025CD400 */
void dWood::Anm_c::mode_cut_init(const dWood::Anm_c *unused, s16 direction) {
    using namespace d_wood_cpp;

    zeroPhases(this);
    mForceDir = (direction);
    f32 speed = woodCutInitVelY;
    mCountdown = (20);
    f32 zero = woodZero;
    mAlphaScale = (255);
    mMode = Mode_Cut;
    mVelY = (speed);
    mPosOffsetY = (zero);
    mPosOffsetZ = (zero);
}

/* 025CD474 */
void dWood::Anm_c::mode_cut(dWood::Packet_c *unused) {
    using namespace d_wood_cpp;

    f32 speed = add((f32)mVelY, woodCutYAccel), minimum = woodCutMinVelY;
    if (speed < minimum)
        speed = minimum;
    f32 y = (f32)mPosOffsetY, z = (f32)mPosOffsetZ, zvel = woodCutZVel;
    y = add(y, speed);
    mVelY = (speed);
    z = add(z, zvel);
    s16 pitch = (s16)mPhaseX[0];
    mPosOffsetY = (y);
    mPosOffsetZ = (z);
    s16 direction = (s16)mForceDir;
    mPhaseX[0] = ((u16)(pitch - 200));
    cMtx_YrotS(gabi::ea(mDoMtx_stack_c::get()), (s32)direction);
    z = (f32)mPosOffsetZ;
    f32 zero = woodZero;
    y = (f32)mPosOffsetY;
    wood_stack_transM(zero, y, z);
    cMtx_XrotM(gabi::ea(mDoMtx_stack_c::get()), (s32)(s16)mPhaseX[0]);
    cMtx_YrotM(gabi::ea(mDoMtx_stack_c::get()), (s32)(s16) - (s16)mForceDir);
    PSMTXCopy(gabi::ea(mDoMtx_stack_c::get()), gabi::ea(this));
    s16 countdown = (s16)mCountdown;
    if (countdown < 20) {
        s32 alpha = (u8)mAlphaScale - 14;
        if (alpha < 0)
            alpha = 0;
        mAlphaScale = ((u8)alpha);
        countdown = (s16)mCountdown;
    }
    if (countdown > 0)
        mCountdown = ((u16)(countdown - 1));
}

/* 025CD584 */
void dWood::Anm_c::mode_push_into_init(const dWood::Anm_c *from, s16 direction) {
    using namespace d_wood_cpp;

    copy_angamp(from);
    // HD: preserve the full callee r3 used as the destination after the copy call.
    dWood::Anm_c *adjusted = gabi::at<dWood::Anm_c>(gabi::cpu->r[3]);
    adjusted->mForceDir = (direction);
    adjusted->mCountdown = (2);
    adjusted->mAlphaScale = (255);
    adjusted->mMode = Mode_PushInto;
}

/* 025CD5E0 */
void dWood::Anm_c::mode_push_into(dWood::Packet_c *packet) {
    using namespace d_wood_cpp;

    s16 time = (s16)((s16)mCountdown - 1);
    mCountdown = (time);
    if (time <= 0) {
        mode_push_back_init();
        return;
    }
    f32 y = woodZero, x = y;
    for (u32 i = 0; i < 2; ++i) {
        const dWood::AttrSway_c *attr = &L_attr_kSways[SWAY_PUSH][i];
        s16 delta = (s16)attr->phaseVelY, ax = (s16)attr->ampX, step = (s16)attr->phaseVelX,
            ay = (s16)attr->ampY;
        f32 bias = (f32)attr->phaseBiasX;
        mPhaseY[i] = ((u16)((s16)mPhaseY[i] + delta));
        cLib_chaseAngleS(gabi::ea(&mPhaseX[i]), 0, (s32)step);
        cLib_chaseS(gabi::ea(&mAmpY[i]), (s32)(s16)(ay / 4), 20);
        cLib_addCalcAngleS(gabi::ea(&mAmpX[i]), (s32)ax, 8, 20, 5);
        s16 newY = (s16)mAmpY[i], newX = (s16)mAmpX[i];
        u16 phaseY = (u16)mPhaseY[i], phaseX = (u16)mPhaseX[i];
        y = gabi::fmadds(cosine(phaseY), (f32)newY, y);
        x = gabi::fmadds(add(cosine(phaseX), bias), (f32)newX, x);
    }
    applySway(this, y, x);
}

/* 025CD5C4 */
void dWood::Anm_c::mode_push_back_init() {
    using namespace d_wood_cpp;
    mCountdown = (35);
    mAlphaScale = (255);
    mMode = Mode_PushBack;
}

/* 025CD958 */
void dWood::Anm_c::mode_push_back(dWood::Packet_c *packet) {
    using namespace d_wood_cpp;

    s16 time = (s16)((s16)mCountdown - 1);
    mCountdown = (time);
    if (time <= 0) {
        s32 index = hdWood_searchAnimation(gabi::ea(packet), 4);
        mode_to_norm_init((u32)index);
        return;
    }
    f32 y = woodZero, x = y, t = gabi::fmuls_ppc((f32)time, woodPushBackTimeScale);
    for (u32 i = 0; i < 2; ++i) {
        const dWood::AttrSway_c *attr = &L_attr_kSways[SWAY_PUSH][i];
        s16 targetY = (s16)gabi::ftoi(gabi::fmuls_ppc((f32)(s16)attr->ampY, t));
        s16 targetX = (s16)gabi::ftoi(gabi::fmuls_ppc((f32)(s16)attr->ampX, t));
        mPhaseY[i] = ((u16)((s16)mPhaseY[i] + (s16)attr->phaseVelY));
        mPhaseX[i] = ((u16)((s16)mPhaseX[i] + (s16)attr->phaseVelX));
        f32 bias = (f32)attr->phaseBiasX;
        cLib_chaseS(gabi::ea(&mAmpY[i]), (s32)targetY, 20);
        cLib_chaseS(gabi::ea(&mAmpX[i]), (s32)targetX, 20);
        s16 newY = (s16)mAmpY[i], newX = (s16)mAmpX[i];
        u16 phaseY = (u16)mPhaseY[i], phaseX = (u16)mPhaseX[i];
        y = gabi::fmadds(cosine(phaseY), (f32)newY, y);
        x = gabi::fmadds(add(cosine(phaseX), bias), (f32)newX, x);
    }
    applySway(this, y, x);
}

/* 025CDC20 */
void dWood::Anm_c::mode_fan(dWood::Packet_c *packet) { using namespace d_wood_cpp; }

/* 025CDC24 */
void dWood::Anm_c::mode_norm_init() {
    using namespace d_wood_cpp;
    mMode = Mode_Norm;
    for (u32 i = 0; i < 2; ++i) {
        mPhaseY[i] = ((u16)((u32)M_init_num << 13));
        mPhaseX[i] = ((u16)((u32)M_init_num << 13));
        mAmpY[i] = ((u16)L_attr_kSways[SWAY_LIGHT][i].ampY);
        mAmpX[i] = ((u16)L_attr_kSways[SWAY_LIGHT][i].ampX);
    }
    mAlphaScale = (255);
    u32 next = (u32)M_init_num + 1;
    M_init_num = (s32)next % 8;
}

/* 025CDCB4 */
void dWood::Anm_c::mode_norm(dWood::Packet_c *packet) {
    using namespace d_wood_cpp;

    f32 strength = (f32)mWindPow;
    u32 level = strength < woodWindMedium ? 0 : (strength < woodWindStrong ? 1 : 2);
    f32 y = woodZero, x = y;
    for (u32 i = 0; i < 2; ++i) {
        const dWood::AttrSway_c *attr = &L_attr_kSways[level][i];
        s16 dy = (s16)attr->phaseVelY, dx = (s16)attr->phaseVelX, ay = (s16)attr->ampY,
            ax = (s16)attr->ampX;
        f32 bias = (f32)attr->phaseBiasX;
        mPhaseY[i] = ((u16)((s16)mPhaseY[i] + dy));
        mPhaseX[i] = ((u16)((s16)mPhaseX[i] + dx));
        cLib_chaseS(gabi::ea(&mAmpY[i]), (s32)ay, 2);
        cLib_chaseS(gabi::ea(&mAmpX[i]), (s32)ax, 2);
        s16 newY = (s16)mAmpY[i], newX = (s16)mAmpX[i];
        u16 py = (u16)mPhaseY[i], px = (u16)mPhaseX[i];
        y = gabi::fmadds(cosine(py), (f32)newY, y);
        x = gabi::fmadds(add(cosine(px), bias), (f32)newX, x);
    }
    applySway(this, y, x);
}

/* 025CDEDC */
void dWood::Anm_c::mode_norm_set_wind(f32 strength, s16 direction) {
    using namespace d_wood_cpp;
    mWindPow = (strength);
    mForceDir = (direction);
}

/* 025CD800 */
void dWood::Anm_c::mode_to_norm_init(u32 index) {
    using namespace d_wood_cpp;

    if (index >= 8) {
        wood_assert(gabi::guest_addr("d_wood.cpp"), 0x799u,
                    gabi::guest_addr("(anm_id_norm >= 0) && (anm_id_norm < AnmID_Norm_Max)"));
        index = 0;
    }
    mNormAnimIdx = ((u16)index);
    mAlphaScale = (255);
    mCountdown = (20);
    mMode = Mode_ToNorm;
}

/* 025CDEE8 */
void dWood::Anm_c::mode_to_norm(dWood::Packet_c *packet) {
    using namespace d_wood_cpp;

    s32 index = (s16)mNormAnimIdx;
    if ((u32)index >= 72)
        wood_assert(gabi::guest_addr("d_wood.h"), 0xE7u,
                    gabi::guest_addr("(int)id >= 0 && (int)id < AnmID_Max"));
    const auto *animationView = gabi::at<const dWood::PacketAnimationView>(gabi::ea(packet));
    const dWood::Anm_c *from = &animationView->mAnm[(u32)index];
    f32 strength = (f32)from->mWindPow;
    s16 direction = (s16)from->mForceDir;
    u32 level = strength < woodWindMedium ? 0 : (strength < woodWindStrong ? 1 : 2);
    cLib_chaseAngleS(gabi::ea(&mForceDir), (s32)direction, 3000);
    f32 y = woodZero, x = y;
    for (u32 i = 0; i < 2; ++i) {
        const dWood::AttrSway_c *attr = &L_attr_kSways[level][i];
        s16 sy = (s16)((s16)attr->phaseVelY + 3000), sx = (s16)((s16)attr->phaseVelX + 3000);
        f32 bias = (f32)attr->phaseBiasX;
        cLib_chaseS(gabi::ea(&mPhaseY[i]), (s32)(s16)from->mPhaseY[i], (s32)sy);
        cLib_chaseS(gabi::ea(&mPhaseX[i]), (s32)(s16)from->mPhaseX[i], (s32)sx);
        cLib_chaseS(gabi::ea(&mAmpY[i]), (s32)(s16)from->mAmpY[i], 15);
        cLib_chaseS(gabi::ea(&mAmpX[i]), (s32)(s16)from->mAmpX[i], 15);
        s16 newY = (s16)mAmpY[i], newX = (s16)mAmpX[i];
        u16 py = (u16)mPhaseY[i], px = (u16)mPhaseX[i];
        y = gabi::fmadds(cosine(py), (f32)newY, y);
        x = gabi::fmadds(add(cosine(px), bias), (f32)newX, x);
    }
    applySway(this, y, x);
    s16 time = (s16)mCountdown;
    if (time > 0)
        mCountdown = ((u16)(time - 1));
}

namespace d_wood_cpp {
// HD: wrappers keep the pending Packet implementation calling the typed Unit methods.

} // namespace d_wood_cpp

using namespace dWood;
void hdWood_prepareDraw(u32 self);
namespace WoodUnitConstants {
enum : u32 {
    JA_SE_OBJ_TREE_SWING = 0x6932,
    JA_SE_OBJ_CUT_L_TREE_DOWN = 0x69D1,
    AT_TYPE_BOMB = 1 << 5,
    AT_TYPE_FIRE = 1 << 9,
    AT_TYPE_NORMAL_ARROW = 1 << 14,
    AT_TYPE_HOOKSHOT = 1 << 15,
    AT_TYPE_FIRE_ARROW = 1 << 18,
    AT_TYPE_ICE_ARROW = 1 << 19,
    AT_TYPE_LIGHT_ARROW = 1 << 20,
    AT_TYPE_WIND = 1 << 21,
    ID_IT_JN_SWOODLEAF00 = 0x3E0,
    fpcNm_PLAYER_e = 168,
};
} // namespace WoodUnitConstants
using namespace WoodUnitConstants;
// HD: this translation unit owns its inline ground-check and SafeString vtables.
static constexpr u32 objGroundVtable = 0x10056794, objPolyVtable = 0x100567C4,
                     objGroupVtable = 0x100567B4, groundVtable = 0x10056764,
                     polyVtable = 0x10056784, groupVtable = 0x10056744,
                     safeStringVtable = 0x1005672C, emptyColorName = 0x10000160;
static LightObj initialLight;
static Mtx34 woodViewMatrix;
static f32 groundCheckYOffset;
static f32 groundHeightMinimum;
static f32 collisionPushDistance;
static f32 emitterColorOne;

/* 025CE1B4 */
dWood::Unit_c::Unit_c() {
    f32 values[14];
    u8 colors[4];
    u16 colorS10[4];
    for (u32 i = 0; i < 6; ++i) {
        values[i] = initialLight.mPositionAndDirection[i];
        mTevStr.mLightObj.mPositionAndDirection[i] = values[i];
    }
    for (u32 i = 0; i < 4; ++i) {
        colors[i] = initialLight.mColor[i];
        mTevStr.mLightObj.mColor[i] = colors[i];
    }
    for (u32 i = 0; i < 4; ++i) {
        colorS10[i] = initialLight.mColorS10[i];
        mTevStr.mLightObj.mColorS10[i] = colorS10[i];
    }
    for (u32 i = 0; i < 8; ++i) {
        values[i + 6] = initialLight.mFactors[i];
        mTevStr.mLightObj.mFactors[i] = values[i + 6];
    }
    for (LightObj &light : mTevStr.mHdLightParam) {
        for (u32 i = 0; i < 6; ++i)
            light.mPositionAndDirection[i] = values[i];
        for (u32 i = 0; i < 4; ++i)
            light.mColor[i] = colors[i];
        for (u32 i = 0; i < 4; ++i)
            light.mColorS10[i] = colorS10[i];
        for (u32 i = 0; i < 8; ++i)
            light.mFactors[i] = values[i + 6];
    }
    clear();
}

/* 025CE360 */
s32 dWood::Unit_c::set_ground() {
    f32 y = gabi::fadds_ppc(mPos.y, groundCheckYOffset);
    f32 x = mPos.x, z = mPos.z;
    gabi::Local<dBgS_ObjGndChk> ground;
    dBgS_ObjGndChk *check = &(*ground);
    woodGroundConstruct(check);
    check->mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk *>(check));
    check->mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk *>(check));
    check->cBgS_PolyInfo::__vtbl = objGroundVtable;
    check->dBgS_PolyPassChk::__vtbl = objPolyVtable;
    check->dBgS_GrpPassChk::__vtbl = objGroupVtable;
    check->mbObjThrough = 1;
    check->mbCamThrough = 0;
    check->mbLinkThrough = 0;
    check->mbArrowThrough = 0;
    check->mbBombThrough = 0;
    check->mbBoomerangThrough = 0;
    check->mbRopeThrough = 0;
    check->mGrp = 1;
    check->m_pos.x = x;
    check->m_pos.y = y;
    check->m_pos.z = z;
    f32 height = dComIfG_Bgsp()->GroundCross(check);
    check->cBgS_PolyInfo::__vtbl = groundVtable;
    check->dBgS_GrpPassChk::__vtbl = groupVtable;
    check->dBgS_PolyPassChk::__vtbl = polyVtable;
    bool found = height > groundHeightMinimum;
    if (found)
        mPos.y = height;
    // HD: no shadow-plane construction; only the height is retained.
    woodGroundDestruct(check, 0);
    return found;
}

/* 025CE4F0 */
void dWood::Unit_c::set_mtx(Anm_c *anim) {
    Anm_c *animation = &anim[(u32)mAnmIdx];
    Mtx34 *stack = mDoMtx_stack_c::get();
    PSMTXCopy(gabi::ea(animation->mModelMtx), gabi::ea(stack));
    for (u32 i = 0; i < 3; ++i) {
        const be<f32> *pos = gabi::at<const be<f32>>(gabi::ea(&mPos));
        stack->m[i][3] = gabi::fadds_ppc(stack->m[i][3], pos[i]);
    }
    // HD: model-view transforms are prepared by the renderer later.
    PSMTXCopy(gabi::ea(stack), gabi::ea(&mModelViewMtx));
    PSMTXCopy(gabi::ea(animation->mTrunkModelMtx), gabi::ea(stack));
    for (u32 i = 0; i < 3; ++i) {
        const be<f32> *pos = gabi::at<const be<f32>>(gabi::ea(&mPos));
        stack->m[i][3] = pos[i];
    }
    PSMTXCopy(gabi::ea(stack), gabi::ea(&mTrunkModelViewMtx));
    gabi::Local<Mtx34> product;
    woodMatrixConcat(&woodViewMatrix, stack, &(*product));
    mDepth = (*product).m[2][3];
}

/* 025CE1A8 */
void dWood::Unit_c::clear() { woodMemorySet(this, 0, sizeof(*this)); }

/* inlined into dWood::Unit_c::cc_hit_before_cut */
static void checkAnimation(s32 index) {
    if ((u32)index >= 72)
        wood_assert(gabi::guest_addr("d_wood.h"), 0xE7,
                    gabi::guest_addr("(int)id >= 0 && (int)id < AnmID_Max"));
}

/* inlined into dWood::Unit_c::cc_hit_before_cut */
static void pushHit(Unit_c *unit, Packet_c *packet, gptr<fopAc_ac_c> *actor, s32 chosen) {
    mDoAud_seStart(JA_SE_OBJ_TREE_SWING, &unit->mPos);
    s32 index = unit->mAnmIdx, oldAnimIdx = index;
    if (index < AnmID_Norm_Max) {
        if (chosen != AnmID_Norm_Invalid) {
            index = chosen;
            unit->mAnmIdx = index;
        }
        if (index < AnmID_Norm_Max)
            return;
    }
    checkAnimation(index);
    if ((s32)packet->mAnm[(u32)index].mMode <= (s32)Anm_c::Mode_Cut)
        return;
    // HD: preserve the entire target-angle return register until the explicit s16 conversion.
    s32 targetAngle = woodTargetAngleY(&(*actor)->current.pos, &unit->mPos);
    s32 current = unit->mAnmIdx;
    checkAnimation(current);
    Anm_c *to = &packet->mAnm[(u32)current];
    checkAnimation(oldAnimIdx);
    to->mode_push_into_init(&packet->mAnm[(u32)oldAnimIdx], (s16)targetAngle);
}

/* 025CE5D0 */
void dWood::Unit_c::cc_hit_before_cut(Packet_c *packet) {
    gabi::Local<HitInf> inf;
    gabi::Local<gptr<fopAc_ac_c>> actor;
    woodHitConstruct(&(*inf));
    void *mass = gabi::at<void>(dComIfGp_ea() + PLAY_CCMASS);
    u32 ret = woodCheckMass(mass, &mPos, &(*actor), &(*inf));
    s16 cooldown = mAnimCooldown;
    if (cooldown > 0) {
        cooldown = (s16)(cooldown - 1);
        mAnimCooldown = cooldown;
    }
    constexpr u32 nonCutAttacks = AT_TYPE_WIND | AT_TYPE_BOMB | AT_TYPE_FIRE |
                                  AT_TYPE_NORMAL_ARROW | AT_TYPE_FIRE_ARROW | AT_TYPE_ICE_ARROW |
                                  AT_TYPE_LIGHT_ARROW | AT_TYPE_HOOKSHOT;
    if ((ret & 1) && (*inf).mpAtObj && ((*inf).mpAtObj->mAtType & nonCutAttacks)) {
        ret &= ~1u;
        if ((*actor) && cooldown == 0) {
            s32 chosen = hdWood_searchAnimation(gabi::ea(packet), Anm_c::Mode_PushInto);
            mAnimCooldown = 20;
            pushHit(this, packet, &(*actor), chosen);
        }
    }
    if ((ret & 2) && (*actor) && (*inf).mpCoObj && (*inf).mpCoObj->mStts) {
        s32 chosen = hdWood_searchAnimation(gabi::ea(packet), Anm_c::Mode_PushInto);
        fopAc_ac_c *attacker = (*actor);
        // HD: the unordered comparison also permits NaN hit lengths.
        if (attacker && (s16)attacker->mProfName == fpcNm_PLAYER_e &&
            !((*inf).mCoHitLen < collisionPushDistance) && (s16)mAnimCooldown == 0) {
            mAnimCooldown = 20;
            pushHit(this, packet, &(*actor), chosen);
        }
    }
    if (!(ret & 1))
        return;
    s32 index = mAnmIdx, oldAnimIdx = index;
    if (index < AnmID_Norm_Max) {
        index = hdWood_searchAnimation(gabi::ea(packet), Anm_c::Mode_Cut);
        if (index == AnmID_Norm_Invalid) {
            index = mAnmIdx;
            if (index < AnmID_Norm_Max)
                return;
        } else {
            mAnmIdx = index;
            if (index < AnmID_Norm_Max)
                return;
        }
    }
    checkAnimation(index);
    if ((s32)packet->mAnm[(u32)index].mMode <= (s32)Anm_c::Mode_Cut)
        return;
    s32 targetAngle = woodTargetAngleY(&(*actor)->current.pos, &mPos);
    s32 current = mAnmIdx;
    checkAnimation(current);
    Anm_c *to = &packet->mAnm[(u32)current];
    checkAnimation(oldAnimIdx);
    to->mode_cut_init(&packet->mAnm[(u32)oldAnimIdx], (s16)targetAngle);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &mPos,
                     reinterpret_cast<dKy_tevstr_c *>(&mTevStr));
    JPABaseEmitter *emitter =
        dComIfGp_getParticle()->set(0, ID_IT_JN_SWOODLEAF00, &mPos, nullptr, nullptr, 255, nullptr,
                                    -1, &mTevStr.mColorK0, nullptr, nullptr);
    if (emitter) {
        gabi::Local<EmitterColor> color;
        (*color).mName = gabi::at<char>(emptyColorName);
        (*color).__vtbl = safeStringVtable;
        f32 one = emitterColorOne;
        for (u32 i = 0; i < 4; ++i)
            gabi::at<be<f32>>(gabi::ea((*color).mColor))[i] = one;
        (*color).mEnabled = 1;
        for (u32 i = 0; i < 3; ++i)
            (*color).mPadding[i] = 0;
        auto *manager = reinterpret_cast<ParticleColorView *>(dComIfGp_getParticle());
        for (u32 i = 0; i < 4; ++i)
            (*color).mColor[i] = manager->mColor[i];
        woodEmitterSetColor(emitter, &(*color));
    }
    // HD: no GameCube shadow-scale adjustment after cutting.
    mDoAud_seStart(JA_SE_OBJ_CUT_L_TREE_DOWN, &mPos);
}

/* 025CEA84 */
void dWood::Unit_c::proc(Packet_c *packet) {
    if (!((u32)mFlags & STATE_ACTIVE))
        return;
    s32 index = mAnmIdx;
    if (index < AnmID_Norm_Max)
        return;
    if ((u32)index >= 72)
        wood_assert(gabi::guest_addr("d_wood.h"), 0xE7,
                    gabi::guest_addr("(int)id >= 0 && (int)id < AnmID_Max"));
    Anm_c *animation = &packet->mAnm[(u32)index];
    switch ((s32)animation->mMode) {
    case Anm_c::Mode_ToNorm:
        if ((s16)animation->mCountdown <= 0) {
            mAnmIdx = (s16)animation->mNormAnimIdx;
            animation->mMode = Anm_c::Mode_Max;
        }
        break;
    case Anm_c::Mode_Cut:
        if ((s16)animation->mCountdown <= 0) {
            mAnmIdx = hdWood_searchAnimation(gabi::ea(packet), Anm_c::Mode_Norm);
            animation->mMode = Anm_c::Mode_Max;
            mFlags = (u32)mFlags | STATE_CUT;
        }
        break;
    case Anm_c::Mode_Max:
        mAnmIdx = hdWood_searchAnimation(gabi::ea(packet), Anm_c::Mode_Norm);
        break;
    }
}

/* 025CEBB8 */
dWood::Room_c::Room_c() {
    mCount = 0;
    mCapacity = 0;
    mpUnits = nullptr;
    woodRoomArrayCreate(this, 200, mUnitStorage);
    mCount = 0;
}

/* 025CEC38 */
void dWood::Room_c::entry_unit(Unit_c *unit) {
    s32 count = mCount, capacity = mCapacity;
    if (count < capacity) {
        mpUnits[(u32)count] = unit;
        mCount = (s32)((u32)mCount + 1);
    }
}

/* 025CEC64 */
void dWood::Room_c::delete_all_unit() {
    u32 count = mCount;
    gptr<Unit_c> *begin = mpUnits;
    gptr<Unit_c> *end = gabi::at<gptr<Unit_c>>(gabi::ea(begin) + count * sizeof(*begin));
    // HD: guest array traversal wraps at the 32-bit address boundary.
    for (gptr<Unit_c> *ptr = begin; ptr != end;
         ptr = gabi::at<gptr<Unit_c>>(gabi::ea(ptr) + sizeof(*ptr))) {
        mDoAud_seDeleteObject(&(*ptr)->mPos);
        (*ptr)->clear();
    }
    mCount = 0;
}

/* 025D0738 */
void dWood::Packet_c::delete_room(s32 room_no) { mRoom[(u32)room_no].delete_all_unit(); }
/* 025D074C */
s32 dWood::Packet_c::search_empty_UnitID() {
    for (u32 i = 0; i < 200; ++i)
        if ((u32)mUnit[i].mFlags == 0)
            return i;
    return 200;
}

/* 025D077C */
s32 dWood::Packet_c::put_unit(const cXyz *pos, u32 room) {
    if (room >= 64)
        wood_assert(gabi::guest_addr("d_wood.cpp"), 0xA17u,
                    gabi::guest_addr("(room_no >= 0) && (room_no < L_Room_Max)"));
    const s32 index = search_empty_UnitID();
    if (index != 200) {
        Unit_c *unit = &mUnit[(u32)index];
        unit->mFlags = Unit_c::STATE_ACTIVE;
        // HD: lfs/stfs copies retain floating-store footprints and the original bit representation.
        unit->mPos.x = pos->x;
        unit->mPos.y = pos->y;
        unit->mPos.z = pos->z;
        unit->mAnmIdx = search_anm(Anm_c::Mode_Norm);
        if (unit->set_ground())
            mRoom[room].entry_unit(unit);
        else
            unit->clear();
    }
    return index;
}

enum : u32 { WOOD_PACKET_VTABLE = 0x10056AE8, WOOD_FADE_PACKET_VTABLE = 0x10056AB8 };
static u32 shaderCacheContext;
static GraphicsState defaultGraphicsState;
/* inlined into dWood::Packet_c::draw_fade */
static void bindUniforms(ShaderMaterial *material, UniformShape *shape) {
    UniformSlots *slots = (u32)material->mUniformCount ? material->mpUniforms.get() : nullptr;
    const s16 vertex = slots->mVertex, pixel = slots->mPixel, geometry = slots->mGeometry;
    // HD: malformed descriptor indices retain the original 32-bit address wrap.
    const u32 descriptorAddress =
        gabi::ea(shape->mDescriptors) + (u32)shape->mDescriptorIndex * sizeof(UniformDescriptor);
    UniformDescriptor *descriptor =
        reinterpret_cast<UniformDescriptor *>((uintptr_t)descriptorAddress);
    const u32 data = gabi::ea(descriptor->mpData), size = descriptor->mSize;
    if (pixel != -1)
        woodGX2PixelUniform((s32)pixel, size, data);
    if (vertex != -1)
        woodGX2VertexUniform((s32)vertex, size, data);
    if (geometry != -1)
        woodGX2GeometryUniform((s32)geometry, size, data);
}
static void bindUniforms(u32 material, u32 shape) {
    bindUniforms(reinterpret_cast<ShaderMaterial *>((uintptr_t)material),
                 reinterpret_cast<UniformShape *>((uintptr_t)shape));
}
/* inlined into dWood::Packet_c::draw_fade */
static ShaderMaterial *selectMaterial(ShapeGroups *groups, u32 mode) {
    ShaderMaterial *material = nullptr;
    if ((s32)mode < 4) {
        const u32 capacity = groups->mCapacity;
        ShapeGroup *array = groups->mpGroups;
        material = array[mode < capacity ? mode : 0].mpMaterial;
    }
    return material;
}
/* inlined into dWood::Packet_c::draw_fade */
static void bindProgram(ShaderMaterial *material) {
    auto *cache = reinterpret_cast<ShaderCache *>(
        (uintptr_t)woodGetShaderCache(gabi::ea(&shaderCacheContext)));
    ShaderProgram *program = material->mpProgram;
    if (program != cache->mpProgram.get()) {
        const u8 flags = program->mFlags;
        const u32 old = cache->mState;
        if (flags & 2) {
            program->mFlags = flags & ~2;
            woodUpdateShaderProgram(gabi::ea(program), 0);
        }
        const u32 state = program->mpState->mState;
        if (old != state)
            woodBindShaderState(state);
        const u32 size = program->mBinarySize;
        if (size)
            woodGX2BindProgram(gabi::ea(program->mpBinary), size);
        else
            woodBindShaderProgram(gabi::ea(program));
        cache->mState = state;
        cache->mpProgram = program;
    }
}
static void applyUniform(UniformObject *object, ShaderMaterial *material) {
    gabi::call_ptr<void>(object->mpVtable->apply.get(), gabi::ea(object), gabi::ea(material));
}
static void bindDrawUniforms(ShaderMaterial *material, DrawInfo *draw) {
    DrawShapeTable *table = draw->mpShapeTable;
    if (table) {
        const u32 capacity = table->mCapacity, index = draw->mShapeIndex;
        const u32 shapeAddress =
            gabi::ea(table->mpShapes) + (index < capacity ? index * sizeof(UniformShape) : 0);
        UniformShape *shape = reinterpret_cast<UniformShape *>((uintptr_t)shapeAddress);
        bindUniforms(material, shape);
    }
}
static void drawIndices(IndexBuffer *buffer) {
    const u32 count = buffer->mCount;
    if (count)
        woodGX2DrawIndexed(gabi::ea(buffer->mpIndices), count, buffer->mPrimitive,
                           buffer->mIndexType, 0, 1);
}
/* 025CECD8 */
void dWood::Packet_c::draw_fade(DrawInfo *draw) {
    u32 mode = draw->mMode;
    ShaderMaterial *material = selectMaterial(&mFadeShapes, mode);
    bindProgram(material);
    mode = draw->mMode;
    if (mode == 0) {
        bindDrawUniforms(material, draw);
    } else if (mode == 1) {
        bindUniforms(material,
                     reinterpret_cast<UniformShape *>(mFadeTransformSet.mpDescriptor.get()));
    } else if (mode == 2) {
        bindUniforms(material,
                     reinterpret_cast<UniformShape *>(mFadeTransformSet.mpDescriptor.get()));
        UniformObject *object = draw->mpObject;
        if (object)
            applyUniform(object, material);
        woodApplyDrawObject(gabi::ea(draw), gabi::ea(material));
    }
    const u32 samplers = (u32)material->mSamplerCount ? gabi::ea(material->mpSamplers) : 0;
    woodBindSamplers(gabi::ea(&mSamplerState), samplers + 4, 0, 0);
    gabi::Local<GraphicsState> state;
    woodGraphicsStateConstruct(state.a);
    const u32 bits = state->mDepthBits;
    mode = draw->mMode;
    state->mBlendMode = 3;
    state->mDepthFunction = 4;
    state->mDepthValue = woodZero;
    state->mDepthBits = (((bits & ~15u) + 7) & 0xFFFFFF0F) + 16;
    state->mSecondaryEnabled = 0;
    state->mDepthEnabled = 1;
    state->mEnabled = 1;
    woodApplyDrawMode(mode, state.a);
    woodGraphicsStateApply(state.a);
    const u32 capacity = mFadeShapes.mCapacity;
    mode = draw->mMode;
    ShapeGroup *group = &mFadeShapes.mpGroups[mode < capacity ? mode : 0];
    const u32 selected = (u32)mMaterials.mFrameIndex == 0 ? 1 : 0;
    woodShapeBind(gabi::ea(group->mShapes[selected].mpShapes));
    const u32 count = mFadeUnits.mCount;
    const u32 begin = gabi::ea(mFadeUnits.mpUnits), end = begin + count * 4;
    u32 transformIndex = count - 1, textureIndex = count - 1;
    for (u32 item = begin; item != end; item += 4) {
        applyUniform(reinterpret_cast<UniformObject *>(&mFadeTextures[textureIndex]), material);
        --textureIndex;
        applyUniform(reinterpret_cast<UniformObject *>(&mFadeTransforms[transformIndex]), material);
        --transformIndex;
        drawIndices(&mLeafIndices);
    }
    woodGraphicsStateApply(gabi::ea(&defaultGraphicsState));
}
/* 025CF204 */
void hdWood_drawMaterialThunk(u32 self, u32 draw) {
    auto *packet = reinterpret_cast<FadePacket *>((uintptr_t)self);
    packet->mpOwner->draw_fade(reinterpret_cast<DrawInfo *>((uintptr_t)draw));
}

static u32 heapManager;
static void freeSlot(u32 slot) {
    const u32 object = *reinterpret_cast<be<u32> *>((uintptr_t)slot);
    auto *heap = reinterpret_cast<Heap *>((uintptr_t)woodFindHeap(heapManager, object));
    gabi::call_ptr<void>(heap->mpVtable->mFree.get(), gabi::ea(heap),
                         (u32) * reinterpret_cast<be<u32> *>((uintptr_t)slot));
}
static void releaseMaterialBuffers(u32 address) {
    auto *block = reinterpret_cast<MaterialBuffers *>((uintptr_t)address);
    for (u32 i = 0; i < 2; ++i) {
        for (u32 j = 0; j < 2; ++j) {
            MaterialPart *part = &block->mPart[i][j];
            woodShapeClear(gabi::ea(&part->mShape));
            part->mpVertices = nullptr;
            if (part->mpVertexStorage) {
                freeSlot(gabi::ea(&part->mpVertexStorage));
                part->mVertexCapacity = 0;
                part->mpVertexStorage = nullptr;
            }
        }
    }
    block->mState = 0;
}
static void destroyShapeArray(ShapeArray *row) {
    Shape *array = row->mpShapes;
    if (!array)
        return;
    s32 count = row->mCount;
    for (s32 i = 0; i < count; ++i) {
        Shape *object = &array[(u32)i];
        gabi::call_ptr<void>(object->mpVtable->mDestroy.get(), gabi::ea(object), 2);
        count = row->mCount;
        array = row->mpShapes;
    }
    freeSlot(gabi::ea(&row->mpShapes));
    row->mCount = 0;
    row->mpShapes = nullptr;
}
static void destroyShapeGroups(u32 address) {
    auto *header = reinterpret_cast<ShapeGroups *>((uintptr_t)address);
    ShapeGroup *array = header->mpGroups;
    if (!array)
        return;
    s32 count = header->mCapacity;
    for (s32 i = 0; i < count; ++i) {
        ShapeGroup *row = reinterpret_cast<ShapeGroup *>((uintptr_t)gabi::ea(&array[(u32)i]));
        if (row) {
            row->mpMaterial = nullptr;
            destroyShapeArray(&row->mShapes[0]);
            destroyShapeArray(&row->mShapes[1]);
            count = header->mCapacity;
            array = header->mpGroups;
        }
    }
    freeSlot(gabi::ea(&header->mpGroups));
    header->mCapacity = 0;
    header->mpGroups = nullptr;
}

/* 025D0190 */
void hdWood_packetDelete(u32 self, u32 flags) {
    if (!self)
        return;
    auto *packet = reinterpret_cast<Packet_c *>((uintptr_t)self);
    packet->mPacketBase.mVtable = WOOD_PACKET_VTABLE;
    u32 material = gabi::ea(&packet->mMaterials);
    releaseMaterialBuffers(material);
    woodArrayDestroy(gabi::ea(packet->mFadeTextures), 200u, 0x364u, 0x025D2190u, 0, 0);
    woodArrayDestroy(gabi::ea(packet->mFadeTransforms), 200u, 0xA8u, 0x025D1EB4u, 0, 0);
    woodTransformSetDestroy(gabi::ea(&packet->mFadeTransformSet), 2);
    destroyShapeGroups(gabi::ea(&packet->mFadeShapes));
    woodPacketBaseDestroy(gabi::ea(&packet->mFadePacket), 0);
    woodSamplerDestroy(gabi::ea(&packet->mSamplerState), 2);
    woodIndexBufferDestroy(gabi::ea(&packet->mTrunkIndices), 2);
    woodIndexBufferDestroy(gabi::ea(&packet->mLeafIndices), 2);
    woodArrayDestroy(gabi::ea(packet->mRoomTextures), 64u, 0x364u, 0x025D2190u, 0, 0);
    woodArrayDestroy(gabi::ea(packet->mRoomTransforms), 400u, 0xA8u, 0x025D1EB4u, 0, 0);
    woodTransformSetDestroy(gabi::ea(&packet->mRoomTransformSet), 2);
    if (material) {
        releaseMaterialBuffers(material);
        woodArrayDestroy(material, 4u, 0x254u, 0x025D2294u, 0, 0);
    }
    destroyShapeGroups(gabi::ea(&packet->mRoomShapes));
    woodPacketBaseDestroy(self, 0);
    if (flags & 1)
        woodFree(self);
}

static f32 collisionRadius;
static f32 cutCollisionRadius;
/* 025D0850 */
void dWood::Packet_c::calc_cc() {
    const s32 room = dComIfGp_roomControl_getStayNo();
    if ((u32)room >= 64)
        return;
    auto *game = reinterpret_cast<WoodMassView *>((uintptr_t)woodGetGameInfo());
    f32 radius = collisionRadius;
    woodMassSetRadius(gabi::ea(game->mShape), radius);
    woodMassSetHeight(gabi::ea(game->mShape), radius);
    game->mAttribute = 19;
    game->mEnabled = 1;
    Room_c *header = &mRoom[(u32)room];
    gptr<Unit_c> *begin = header->mpUnits;
    const u32 end = gabi::ea(begin) + (u32)header->mCount * sizeof(*begin);
    for (u32 address = gabi::ea(begin); address != end; address += sizeof(*begin)) {
        Unit_c *unit = *reinterpret_cast<gptr<Unit_c> *>((uintptr_t)address);
        if (!((u32)unit->mFlags & Unit_c::STATE_CUT))
            unit->cc_hit_before_cut(this);
    }
    game = reinterpret_cast<WoodMassView *>((uintptr_t)woodGetGameInfo());
    radius = cutCollisionRadius;
    woodMassSetRadius(gabi::ea(game->mShape), radius);
    woodMassSetHeight(gabi::ea(game->mShape), radius);
    game->mAttribute = 18;
    game->mEnabled = 1;
    // HD: the second pass has no collision body.
}

/* 025D0994 */
void dWood::Packet_c::calc() {
    calc_cc();
    const cXyz *vector = reinterpret_cast<cXyz *>((uintptr_t)woodGetWindVector());
    const f32 strength = woodGetWindPower();
    const s32 angle = woodAtan2(vector->x, vector->z);
    for (u32 i = 0; i < AnmID_Norm_Max; ++i)
        mAnm[i].mode_norm_set_wind(strength, (s16)angle);
    for (Anm_c &animation : mAnm)
        animation.play(this);
    for (Unit_c &unit : mUnit)
        unit.proc(this);
}

/* 025D0A6C */
void hdWood_copyMatrix(Mtx34 *out, const Mtx34 *in) {
    f32 matrix[12];
    const be<f32> *source = &in->m[0][0];
    be<f32> *target = &out->m[0][0];
    for (u32 i = 0; i < 12; ++i)
        matrix[i] = source[i];
    for (u32 i = 0; i < 12; ++i)
        target[i] = matrix[i];
}
static f32 colorDivisor;
/* 025D0B0C */
void hdWood_unpackColor(WoodColorF *out, const be<s16> *in) {
    const f32 scale = colorDivisor;
    f32 values[4];
    for (u32 i = 0; i < 4; ++i)
        values[i] = (f32)((f64)(f32)in[i] / (f64)scale);
    for (u32 i = 0; i < 4; ++i)
        out->mComponents[i] = values[i];
}
/* 025D0BD0 */
s32 hdWood_compareDepth(const Unit_c *a, const Unit_c *b) { return a->mDepth < b->mDepth ? 1 : -1; }
static f32 cullRadius;
static u32 woodFrustum;
static u32 woodDrawList;
static u32 woodCurrentDrawList;
/* 025D15B0 */
void dWood::Packet_c::update() {
    const f32 radius = cullRadius;
    gabi::Local<cXyz> point;
    for (Unit_c &unit : mUnit) {
        const f32 x = unit.mPos.x, y = unit.mPos.y, z = unit.mPos.z;
        if (!((u32)unit.mFlags & Unit_c::STATE_ACTIVE))
            continue;
        point->x = x;
        point->y = y;
        point->z = z;
        const s32 outside =
            woodFrustumCheck(gabi::ea(&woodFrustum), gabi::ea(&woodViewMatrix), point.a, radius);
        const u32 flags = unit.mFlags;
        if (outside) {
            unit.mFlags = flags | Unit_c::STATE_FRUSTUM_CULLED;
        } else {
            unit.mFlags = flags & ~Unit_c::STATE_FRUSTUM_CULLED;
            unit.set_mtx(mAnm);
        }
    }
    auto *game = reinterpret_cast<WoodDrawListsView *>((uintptr_t)woodGetGameInfo());
    const u32 list = game->mRoomList;
    woodCurrentDrawList = list;
    woodDrawListEntry(woodDrawList, gabi::ea(this), 0);
    woodDrawListEntry(woodCurrentDrawList, gabi::ea(&mFadePacket), 0);
    game = reinterpret_cast<WoodDrawListsView *>((uintptr_t)woodGetGameInfo());
    woodCurrentDrawList = game->mAfterDrawList;
    hdWood_prepareDraw(gabi::ea(this));
}

static InitState woodInitState;
static DestructorRecords woodDestructorRecords;
static f32 woodInitFirstValue;
static f32 woodInitSecondValue;
static f32 textureZero;
static f32 textureOne;
enum : u32 { TRANSFORM_VTABLE = 0x1016EF84, TEXTURE_TRANSFORM_VTABLE = 0x1016EFB4 };
/* 025D1D6C */
void __sinit_d_wood_cpp() {
    woodInitState.mZero[2] = 0;
    woodInitState.mZero[0] = 0;
    woodInitState.mZero[3] = 0;
    woodInitState.mZero[1] = 0;
    woodRegisterStatic(gabi::ea(woodDestructorRecords.mRecord[0]));
    const f32 a = woodInitFirstValue, b = woodInitSecondValue;
    woodInitState.mFirstValue = a;
    woodInitState.mSecondValue = b;
    woodFirstStaticFlagConstruct(gabi::ea(&woodInitState.mFirstFlag));
    woodRegisterStatic(gabi::ea(woodDestructorRecords.mRecord[1]));
    woodSecondStaticFlagConstruct(gabi::ea(&woodInitState.mSecondFlag));
    woodRegisterStatic(gabi::ea(woodDestructorRecords.mRecord[2]));
}
// HD: compiler-generated and template code of this translation unit.
/* 025D1E00 */
void hdWood_deleteEmpty(u32 self, u32 flags) {
    if (self && (flags & 1))
        woodFree(self);
}
/* 025D1E14 */
u32 hdWood_createVector4(u32 self) { return self ? self : woodAllocate(sizeof(Vector4)); }
/* 025D1E40 */
void hdWood_emptyVector4(u32 self) {}
/* 025D1E44 */
u32 hdWood_createTransform(u32 self) {
    if (!self)
        self = woodAllocate(sizeof(Transform));
    if (self) {
        auto *object = reinterpret_cast<Transform *>((uintptr_t)self);
        woodUniformConstruct(self);
        object->mpVtable = reinterpret_cast<UniformVtable *>((uintptr_t)TRANSFORM_VTABLE);
        woodClearUniformData(gabi::ea(&object->mMatrix), sizeof(Mtx34) + 4);
        // HD: retain placement-new allocation only when the embedded address wraps to zero.
        if (gabi::ea(&object->mMatrix) == 0)
            woodAllocate(sizeof(Mtx34));
    }
    return self;
}
/* 025D1EB4 */
void hdWood_deleteTransform(u32 self, u32 flags) {
    if (self) {
        woodUniformDestroy(self, 0);
        if (flags & 1)
            woodFree(self);
    }
}
/* 025D1F08 */
u32 hdWood_createTextureTransform(u32 self) {
    if (!self)
        self = woodAllocate(sizeof(TextureTransform));
    if (self) {
        auto *object = reinterpret_cast<TextureTransform *>((uintptr_t)self);
        woodUniformConstruct(self);
        object->mpVtable = reinterpret_cast<UniformVtable *>((uintptr_t)TEXTURE_TRANSFORM_VTABLE);
        woodClearUniformData(gabi::ea(&object->mData), sizeof(TextureUniformData));
        const f32 zero = textureZero, one = textureOne;
        for (Vector4 &color : object->mData.mColors) {
            for (u32 j = 0; j < 3; ++j)
                color.mValues[j] = zero;
            color.mValues[3] = one;
        }
        for (u32 i = 0; i < 3; ++i)
            woodArrayConstruct(gabi::ea(object->mData.mVectors[i]), 2, sizeof(Vector4), 0x025D1E14);
        for (Mtx34 &matrix : object->mData.mMatrices)
            if (gabi::ea(&matrix) == 0)
                woodAllocate(sizeof(Mtx34));
        for (Vector4 &vector : object->mData.mTailVectors)
            if (gabi::ea(&vector) == 0)
                woodAllocate(sizeof(Vector4));
    }
    return self;
}
/* 025D2190 */
void hdWood_deleteTextureTransform(u32 self, u32 flags) {
    if (self) {
        woodUniformDestroy(self, 0);
        if (flags & 1)
            woodFree(self);
    }
}
/* 025D21E4 */
void hdWood_deletePacketBase(u32 self, u32 flags) {
    if (self) {
        woodPacketBaseDestroy(self, 0);
        if (flags & 1)
            woodFree(self);
    }
}
/* 025D2238 */
u32 hdWood_createMaterial(u32 self) {
    if (!self)
        self = woodAllocate(sizeof(MaterialPart));
    if (self) {
        auto *part = reinterpret_cast<MaterialPart *>((uintptr_t)self);
        woodVertexBufferConstruct(gabi::ea(&part->mVertexBuffer));
        woodShapeConstruct(gabi::ea(&part->mShape));
        part->mpVertexStorage = nullptr;
        part->mVertexCapacity = 0;
    }
    return self;
}
/* 025D2294 */
void hdWood_deleteMaterial(u32 self, u32 flags) {
    if (self) {
        auto *part = reinterpret_cast<MaterialPart *>((uintptr_t)self);
        woodShapeDestroy(gabi::ea(&part->mShape), 2);
        woodVertexBufferDestroy(gabi::ea(&part->mVertexBuffer), 2);
        if (flags & 1)
            woodFree(self);
    }
}
/* 025D22F4 */
void hdWood_emptyMaterial(u32 self) {}

/* inlined into hdWood_packetCreate */
static u32 materialInstance(u32 name) {
    gabi::Local<StringView> desc;
    desc->mString = name;
    desc->mVtable = safeStringVtable;
    auto *manager = reinterpret_cast<MaterialManager *>((uintptr_t)woodGetMaterialManager());
    const s32 index = woodFindMaterialIndex(gabi::ea(manager->mpResource), desc.a);
    if (index < 0)
        return 0;
    u32 result = gabi::ea(manager->mpInstances) +
                 ((u32)index < (u32)manager->mCount ? (u32)index * sizeof(MaterialInstance) : 0);
    if (!(u8) reinterpret_cast<MaterialInstance *>((uintptr_t)result)->mReady) {
        MaterialResource *resource = manager->mpResource;
        const u32 binding =
            (u32)index < (u32)resource->mCount
                ? gabi::ea(resource->mpBindings) + (u32)index * sizeof(MaterialBinding)
                : 0;
        woodCreateMaterialInstance(result, binding, 0);
        result = gabi::ea(manager->mpInstances) +
                 ((u32)index < (u32)manager->mCount ? (u32)index * sizeof(MaterialInstance) : 0);
    }
    return result;
}
static u32 heapAllocate(u32 bytes, u32 align) {
    auto *heap = reinterpret_cast<Heap *>((uintptr_t)woodGetHeap(heapManager));
    return gabi::call_ptr<u32>(heap->mpVtable->mAllocate.get(), gabi::ea(heap), bytes, align);
}
static void createShapeGroups(u32 address, u32 material) {
    auto *header = reinterpret_cast<ShapeGroups *>((uintptr_t)address);
    auto *buffers = reinterpret_cast<MaterialBuffers *>((uintptr_t)material);
    for (u32 i = 0; i < (u32)header->mCount; ++i) {
        ShapeGroup *row = &header->mpGroups[i < (u32)header->mCapacity ? i : 0];
        ShaderMaterial *original = row->mpMaterial;
        row->mpMaterial = nullptr;
        destroyShapeArray(&row->mShapes[0]);
        destroyShapeArray(&row->mShapes[1]);
        row->mpMaterial = original;
        for (u32 group = 0; group < 2; ++group) {
            const u32 array = heapAllocate(2 * sizeof(Shape), 4);
            for (u32 j = 0; j < 2; ++j) {
                const u32 shape = array + j * sizeof(Shape);
                if (shape)
                    woodShapeConstruct(shape);
            }
            if (array) {
                row->mShapes[group].mpShapes = reinterpret_cast<Shape *>((uintptr_t)array);
                row->mShapes[group].mCount = 2;
            }
        }
        for (u32 group = 0; group < 2; ++group)
            for (u32 j = 0; j < 2; ++j) {
                const u32 shape = gabi::ea(row->mShapes[group].mpShapes) +
                                  (j < (u32)row->mShapes[group].mCount ? j * sizeof(Shape) : 0);
                woodConfigureShape(gabi::ea(original), shape,
                                   gabi::ea(&buffers->mPart[j][group].mVertexBuffer),
                                   gabi::ea(&buffers->mVertexFormat), 0);
            }
    }
}
static void clearVertexBuffer(u32 buffer) {
    const u32 end = buffer + 98 * sizeof(Vertex);
    for (u32 address = buffer; address < end; address += sizeof(CacheLine)) {
        auto *line = reinterpret_cast<CacheLine *>((uintptr_t)(address & ~31u));
        for (be<u32> &word : line->mWords)
            word = 0;
    }
}
static dWood::GeometrySource woodGeometry;
static void fillVertices(u32 material, u32 group) {
    auto *buffers = reinterpret_cast<MaterialBuffers *>((uintptr_t)material);
    MaterialPart *part = &buffers->mPart[group][(u32)buffers->mFrameIndex];
    clearVertexBuffer(gabi::ea(part->mpVertices));
    const be<u32> *count = group ? &woodGeometry.mTrunkVertexCount : &woodGeometry.mLeafVertexCount;
    const Vector3 *position = group ? woodGeometry.mTrunkPosition : woodGeometry.mLeafPosition;
    const Vector3 *normal = group ? woodGeometry.mTrunkNormal : woodGeometry.mLeafNormal;
    const Vector4 *color = group ? woodGeometry.mTrunkColor : woodGeometry.mLeafColor;
    const Vector2 *uv = group ? woodGeometry.mTrunkUV : woodGeometry.mLeafUV;
    part = &buffers->mPart[group][(u32)buffers->mFrameIndex];
    Vertex *out = part->mpVertices;
    for (u32 i = 0; i < (u32)*count; ++i) {
        for (u32 j = 0; j < 3; ++j)
            out[i].mPosition[j] = position[i].mValues[j];
        for (u32 j = 0; j < 3; ++j)
            out[i].mNormal[j] = normal[i].mValues[j];
        for (u32 j = 0; j < 4; ++j)
            out[i].mColor[j] = color[i].mValues[j];
        for (u32 j = 0; j < 2; ++j)
            out[i].mTexCoord[j] = uv[i].mValues[j];
    }
}
static u32 woodGraphicsLock;
static u32 woodResourceManager;
static u32 woodVertexOffsets[4] = {0, 12, 24, 32};
/* 025CF20C */
u32 hdWood_packetCreate(u32 self) {
    if (!self)
        self = woodAllocate(0x7D3D0u);
    if (!self)
        return self;
    auto *packet = reinterpret_cast<Packet_c *>((uintptr_t)self);
    woodPacketBaseConstruct(self);
    packet->mPacketBase.mVtable = WOOD_PACKET_VTABLE;
    woodArrayConstruct(gabi::ea(packet->mUnit), 200u, 0x248u, 0x025CE1B4u);
    woodArrayConstruct(gabi::ea(packet->mAnm), 72u, 0x8Cu, 0x025CD26Cu);
    woodArrayConstruct(gabi::ea(packet->mRoom), 64u, 0x32Cu, 0x025CEBB8u);
    u32 base = gabi::ea(&packet->mRoomShapes), material = gabi::ea(&packet->mMaterials),
        r = gabi::ea(&packet->mLeafIndices);
    packet->mRoomShapes.mCount = 0;
    u32 header = gabi::ea(&packet->mRoomShapes.mCapacity);
    if (!header)
        header = woodAllocate(8u);
    if (header) {
        reinterpret_cast<WoodWordPair *>((uintptr_t)header)->mSecond = 0;
        reinterpret_cast<WoodWordPair *>((uintptr_t)header)->mFirst = 0;
    }
    u32 m = material;
    if (!m)
        m = woodAllocate(0x968u);
    if (m) {
        woodArrayConstruct(m, 4u, 0x254u, 0x025D2238u);
        reinterpret_cast<MaterialBuffers *>((uintptr_t)m)->mFrameIndex = 0;
        reinterpret_cast<MaterialBuffers *>((uintptr_t)m)->mState = 0;
        reinterpret_cast<MaterialBuffers *>((uintptr_t)m)->mVertexSize = 0x30;
        reinterpret_cast<MaterialBuffers *>((uintptr_t)m)->mReady = 0;
        reinterpret_cast<MaterialBuffers *>((uintptr_t)m)->mVertexFormat = 0;
        for (u32 i = 0; i < 2; ++i)
            for (u32 j = 0; j < 2; ++j)
                reinterpret_cast<MaterialBuffers *>((uintptr_t)m)->mPart[j][i].mpVertices = nullptr;
    }
    woodTransformSetConstruct(gabi::ea(&packet->mRoomTransformSet));
    woodArrayConstruct(gabi::ea(packet->mRoomTransforms), 400u, 0xA8u, 0x025D1E44u);
    woodArrayConstruct(gabi::ea(packet->mRoomTextures), 64u, 0x364u, 0x025D1F08u);
    woodIndexBufferConstruct(r);
    woodIndexBufferConstruct(gabi::ea(&packet->mTrunkIndices));
    woodSamplerConstruct(gabi::ea(&packet->mSamplerState.mState));
    woodSamplerResourceConstruct(gabi::ea(packet->mSamplerState.mResource));
    woodPacketBaseConstruct(gabi::ea(&packet->mFadePacket));
    packet->mFadePacket.mVtable = WOOD_FADE_PACKET_VTABLE;
    header = gabi::ea(&packet->mFadeUnits);
    if (!header)
        header = woodAllocate(0x32Cu);
    if (header) {
        reinterpret_cast<WoodWordPair *>((uintptr_t)header)->mFirst = 0;
        reinterpret_cast<WoodWordPair *>((uintptr_t)header)->mSecond = 0;
        reinterpret_cast<Room_c *>((uintptr_t)header)->mpUnits = nullptr;
        woodCreatePointerArray(header, 200u, header + 12);
    }
    packet->mFadeShapes.mCount = 0;
    header = gabi::ea(&packet->mFadeShapes.mCapacity);
    if (!header)
        header = woodAllocate(8u);
    if (header) {
        reinterpret_cast<WoodWordPair *>((uintptr_t)header)->mFirst = 0;
        reinterpret_cast<WoodWordPair *>((uintptr_t)header)->mSecond = 0;
    }
    woodTransformSetConstruct(gabi::ea(&packet->mFadeTransformSet));
    woodArrayConstruct(gabi::ea(packet->mFadeTransforms), 200u, 0xA8u, 0x025D1E44u);
    woodArrayConstruct(gabi::ea(packet->mFadeTextures), 200u, 0x364u, 0x025D1F08u);
    for (u32 i = 0; i < 8; ++i)
        packet->mAnm[i].mode_norm_init();
    woodBindMaterialGroups(base, materialInstance(gabi::guest_addr("wood_draw")), 0);
    woodBindMaterialGroups(gabi::ea(&packet->mFadeShapes),
                           materialInstance(gabi::guest_addr("wood_draw_xlu")), 0);
    packet->mMaterials.mVertexFormat = 0x1013;
    packet->mMaterials.mpVertexDeclaration = woodVertexOffsets;
    for (u32 i = 0; i < 2; ++i)
        for (u32 j = 0; j < 2; ++j) {
            u32 part = gabi::ea(&packet->mMaterials.mPart[j][i]);
            if (!(u32) reinterpret_cast<MaterialPart *>((uintptr_t)part)->mpVertices.v) {
                u32 buffer = heapAllocate(0x1260, 0x40);
                if (buffer) {
                    reinterpret_cast<MaterialPart *>((uintptr_t)part)->mpVertexStorage.v = buffer;
                    reinterpret_cast<MaterialPart *>((uintptr_t)part)->mVertexCapacity = 98;
                }
                reinterpret_cast<MaterialPart *>((uintptr_t)part)->mpVertices.v =
                    (u32) reinterpret_cast<MaterialPart *>((uintptr_t)part)->mpVertexStorage.v;
            }
            woodConfigureVertexBuffer(
                part + 4, (u32) reinterpret_cast<MaterialPart *>((uintptr_t)part)->mpVertices.v,
                98u, material + 0x954);
        }
    packet->mMaterials.mState = 0;
    packet->mMaterials.mReady = 1;
    createShapeGroups(base, material);
    createShapeGroups(gabi::ea(&packet->mFadeShapes), material);
    woodTransformSetConfigure(gabi::ea(&packet->mRoomTransformSet), 1, 0);
    woodTransformSetConfigure(gabi::ea(&packet->mFadeTransformSet), 1, 0);
    for (u32 i = 0; i < 64; ++i)
        woodUniformSetState(gabi::ea(packet->mRoomTextures) + i * 0x364, 0);
    for (u32 i = 0; i < 200; ++i)
        woodUniformSetState(gabi::ea(packet->mFadeTextures) + i * 0x364, 0);
    for (u32 i = 0; i < 400; ++i)
        woodUniformSetState(gabi::ea(packet->mRoomTransforms) + i * 0xA8, 0);
    for (u32 i = 0; i < 200; ++i)
        woodUniformSetState(gabi::ea(packet->mFadeTransforms) + i * 0xA8, 0);
    woodIndexBufferSet(r, gabi::ea(woodGeometry.mLeafIndices), 4,
                       (u32)woodGeometry.mLeafIndexCount);
    packet->mLeafIndices.mpIndices.v = 4;
    woodIndexBufferSet(gabi::ea(&packet->mTrunkIndices), gabi::ea(woodGeometry.mTrunkIndices), 4,
                       (u32)woodGeometry.mTrunkIndexCount);
    packet->mTrunkIndices.mpIndices.v = 4;
    fillVertices(material, 0);
    fillVertices(material, 1);
    // Cache both buffer pointers once per vertex, then interleave scalar float transfers as native.
    for (u32 group = 0; group < 2; ++group) {
        u32 index = (u32)packet->mMaterials.mFrameIndex,
            src = material + (index + group * 2) * 0x254,
            dst = material + ((index ? 0 : 1) + group * 2) * 0x254;
        for (u32 i = 0; i < 98; ++i) {
            const be<u32> *from = reinterpret_cast<const be<u32> *>(
                reinterpret_cast<MaterialPart *>((uintptr_t)src)->mpVertices.get());
            be<f32> *to = reinterpret_cast<be<f32> *>(
                reinterpret_cast<MaterialPart *>((uintptr_t)dst)->mpVertices.get());
            for (u32 j = 0; j < 12; ++j)
                to[i * 12 + j] = std::bit_cast<f32>((u32)from[i * 12 + j]);
        }
    }
    u32 part = material + (u32)packet->mMaterials.mFrameIndex * 0x254 + 4;
    for (u32 i = 0; i < 2; ++i) {
        woodVertexBufferFlush(part, 0,
                              (u32) reinterpret_cast<VertexBuffer *>((uintptr_t)part)->mFlushSize);
        part += 0x4A8;
    }
    packet->mMaterials.mFrameIndex = (u32)packet->mMaterials.mFrameIndex ? 0 : 1;
    woodLockGraphics(woodGraphicsLock);
    gabi::Local<StringView> a, b;
    a->mString = gabi::guest_addr("ProgramTexture");
    a->mVtable = safeStringVtable;
    b->mString = gabi::guest_addr("ProgramTexture.bfres");
    b->mVtable = safeStringVtable;
    u32 resource = woodGetResource(woodResourceManager, a.a, b.a, 0);
    woodLoadSamplerResource(gabi::ea(packet->mSamplerState.mResource), resource,
                            gabi::guest_addr("Txa_swood_b"));
    bool equal = true;
    for (u32 word : {1u, 2u, 3u, 4u, 5u, 6u, 14u, 13u, 7u})
        if ((u32)packet->mSamplerState.mState.mWords[word] !=
            (u32) reinterpret_cast<be<u32> *>(packet->mSamplerState.mResource)[word]) {
            equal = false;
            break;
        }
    if (!equal) {
        woodCopySamplerState(gabi::ea(&packet->mSamplerState.mState),
                             gabi::ea(packet->mSamplerState.mResource));
        packet->mSamplerState.mState.mWords[(0x18C - 0x30) / 4] = 1;
        packet->mSamplerState.mState.mWords[(0x190 - 0x30) / 4] = 1;
        packet->mSamplerState.mState.mWords[(0x194 - 0x30) / 4] = 1;
        packet->mSamplerState.mState.mFlags = (u8)packet->mSamplerState.mState.mFlags | 2;
    } else {
        u32 x =
            (u32) reinterpret_cast<be<u32> *>(packet->mSamplerState.mResource)[(0x1F0 - 0x1C8) / 4];
        packet->mSamplerState.mState.mWords[(0x18C - 0x30) / 4] = 1;
        u32 y =
            (u32) reinterpret_cast<be<u32> *>(packet->mSamplerState.mResource)[(0x1F8 - 0x1C8) / 4];
        u8 flags = (u8)packet->mSamplerState.mState.mFlags;
        packet->mSamplerState.mState.mWords[(0x60 - 0x30) / 4] = y;
        packet->mSamplerState.mState.mWords[(0x58 - 0x30) / 4] = x;
        packet->mSamplerState.mState.mWords[(0x190 - 0x30) / 4] = 1;
        packet->mSamplerState.mState.mWords[(0x10C - 0x30) / 4] = y;
        packet->mSamplerState.mState.mFlags = flags | 2;
        packet->mSamplerState.mState.mWords[(0x104 - 0x30) / 4] = x;
        packet->mSamplerState.mState.mWords[(0x194 - 0x30) / 4] = 1;
    }
    woodUnlockGraphics(woodGraphicsLock);
    packet->mFadePacket.mpOwner.v = self;
    return self;
}

static f32 roomDrawDepth;
/* 025D16E0 */
void dWood::Packet_c::draw_rooms(DrawInfo *draw) {
    u32 mode = draw->mMode;
    ShaderMaterial *material = selectMaterial(&mRoomShapes, mode);
    bindProgram(material);
    mode = draw->mMode;
    if (mode == 0) {
        bindDrawUniforms(material, draw);
    } else if (mode == 1) {
        bindUniforms(material,
                     reinterpret_cast<UniformShape *>(mRoomTransformSet.mpDescriptor.get()));
    } else if (mode == 2) {
        bindUniforms(material,
                     reinterpret_cast<UniformShape *>(mRoomTransformSet.mpDescriptor.get()));
        UniformObject *object = draw->mpObject;
        if (object)
            applyUniform(object, material);
    }
    if (mode <= 2) {
        const u32 samplers = (u32)material->mSamplerCount ? gabi::ea(material->mpSamplers) : 0;
        woodBindSamplers(gabi::ea(&mSamplerState), samplers + 4, 0, 0);
        if (mode == 2)
            woodApplyDrawObject(gabi::ea(draw), gabi::ea(material));
    }
    gabi::Local<GraphicsState> state;
    woodGraphicsStateConstruct(state.a);
    mode = draw->mMode;
    state->mBlendMode = 2;
    state->mDepthEnabled = 1;
    state->mDepthFunction = 4;
    state->mDepthValue = roomDrawDepth;
    state->mDepthBits = ((((u32)state->mDepthBits & ~15u) + 7) & 0xFFFFFF0F) + 16;
    woodApplyDrawMode(mode, state.a);
    woodGraphicsStateApply(state.a);
    u32 transformIndex = 0, textureIndex = 0;
    for (u32 room = 0; room < 64; ++room) {
        Room_c *header = &mRoom[room];
        const u32 begin = gabi::ea(header->mpUnits), end = begin + (u32)header->mCount * 4;
        if (begin != end) {
            applyUniform(reinterpret_cast<UniformObject *>(&mRoomTextures[textureIndex]), material);
            ++textureIndex;
        }
        for (u32 address = begin; address != end; address += 4) {
            auto *entry = reinterpret_cast<gptr<Unit_c> *>((uintptr_t)address);
            Unit_c *unit = *entry;
            if ((u32)unit->mFlags & Unit_c::STATE_FRUSTUM_CULLED)
                continue;
            const u32 animationIndex = unit->mAnmIdx;
            if (animationIndex >= 72)
                wood_assert(gabi::guest_addr("d_wood.cpp"), 0xB89,
                            gabi::guest_addr("anmID > AnmID_Err && anmID < AnmID_Max"));
            const bool simple = (u8)mAnm[animationIndex].mAlphaScale == 255 &&
                                !((u32)(*entry)->mFlags & Unit_c::STATE_CUT);
            applyUniform(reinterpret_cast<UniformObject *>(&mRoomTransforms[transformIndex]),
                         material);
            ++transformIndex;
            if (simple) {
                mode = draw->mMode;
                ShapeGroup *group =
                    &mRoomShapes.mpGroups[mode < (u32)mRoomShapes.mCapacity ? mode : 0];
                const u32 select = (u32)mMaterials.mFrameIndex ? 0 : 1;
                woodShapeBind(gabi::ea(group->mShapes[select].mpShapes));
                drawIndices(&mLeafIndices);
                applyUniform(reinterpret_cast<UniformObject *>(&mRoomTransforms[transformIndex]),
                             material);
                ++transformIndex;
            }
            mode = draw->mMode;
            ShapeGroup *group = &mRoomShapes.mpGroups[mode < (u32)mRoomShapes.mCapacity ? mode : 0];
            const u32 select = (u32)mMaterials.mFrameIndex ? 0 : 1;
            Shape *array = group->mShapes[select].mpShapes;
            const u32 count = group->mShapes[select].mCount;
            woodShapeBind(gabi::ea(&array[count > 1 ? 1 : 0]));
            drawIndices(&mTrunkIndices);
        }
    }
    woodGraphicsStateApply(gabi::ea(&defaultGraphicsState));
}

static Mtx34 woodProjection;
static u32 woodCamera;
/* inlined into hdWood_prepareDraw */
static void copyTransform(Mtx34 *out, const Mtx34 *in, Transform *matrix) {
    const be<f32> *source = &in->m[0][0];
    be<f32> *target = &out->m[0][0];
    for (u32 j = 0; j < 12; ++j)
        target[j] = source[j];
    PSMTXCopy(gabi::ea(out), gabi::ea(&matrix->mMatrix));
    woodUniformUpdate(gabi::ea(matrix));
}
/* inlined into hdWood_prepareDraw */
static void textureColors(TextureTransform *object, TevStr *tev, f32 alpha, f32 divisor,
                          TransformSet *matrixSet) {
    for (u32 j = 0; j < 4; ++j)
        object->mData.mColors[4].mValues[j] = (f32)((f64)(f32)tev->mColorC0[j] / (f64)divisor);
    f32 color[4];
    const be<u8> *rgba = reinterpret_cast<const be<u8> *>(&tev->mColorK0);
    for (u32 j = 0; j < 4; ++j)
        color[j] = (f32)((f64)(f32)rgba[j] / (f64)divisor);
    object->mData.mColors[5].mValues[0] = color[0];
    object->mData.mColors[5].mValues[2] = color[2];
    object->mData.mColors[5].mValues[1] = color[1];
    object->mData.mColors[5].mValues[3] = color[3];
    object->mData.mColors[6].mValues[3] = alpha;
    woodScaleColor(gabi::ea(&object->mData.mColors[5]), tev->mLightObj.mFactors[0]);
    auto *descriptor = reinterpret_cast<FogDescriptor *>(matrixSet->mpDescriptor.get());
    be<u32> *target = reinterpret_cast<be<u32> *>(&object->mData.mColors[2]);
    for (u32 j = 0; j < 4; ++j)
        target[j] = descriptor->mColors[0][j];
    woodUniformUpdate(gabi::ea(object));
}
/* 025D0BEC */
void hdWood_prepareDraw(u32 self) {
    auto *packet = reinterpret_cast<Packet_c *>((uintptr_t)self);
    // HD: one exact-sized frame keeps recorded library outputs in their original stack objects.
    gabi::Local<DrawFrame> frame;
    hdWood_copyMatrix(&frame->mView, &woodViewMatrix);
    TevStr *tev = &frame->mLight;
    LightObj *light[3] = {&tev->mLightObj, &tev->mHdLightParam[0], &tev->mHdLightParam[1]};
    for (u32 i = 0; i < 3; ++i) {
        for (u32 j = 0; j < 6; ++j)
            light[i]->mPositionAndDirection[j] = initialLight.mPositionAndDirection[j];
        for (u32 j = 0; j < 4; ++j)
            light[i]->mColor[j] = initialLight.mColor[j];
        for (u32 j = 0; j < 4; ++j)
            light[i]->mColorS10[j] = initialLight.mColorS10[j];
        for (u32 j = 0; j < 8; ++j)
            light[i]->mFactors[j] = initialLight.mFactors[j];
    }
    woodInitLight(gabi::ea(tev), dComIfGp_roomControl_getStayNo(), 255);
    const u32 env = woodGetEnvironment();
    woodSetupEnvironment(env, 1, 0, gabi::ea(tev));
    woodSetLightColor(gabi::ea(tev));
    TransformSet *textureSet = &packet->mRoomTransformSet;
    TransformSet *transformSet = &packet->mFadeTransformSet;
    woodTransformSetView(gabi::ea(textureSet), 0, gabi::ea(&frame->mView),
                         gabi::ea(&woodProjection), woodCamera + 0x240);
    woodTransformSetView(gabi::ea(transformSet), 0, gabi::ea(&frame->mView),
                         gabi::ea(&woodProjection), woodCamera + 0x240);
    for (u32 i = 0; i < 2; ++i) {
        TransformSet *set = i ? transformSet : textureSet;
        for (u32 j = 0; j < 2; ++j) {
            auto *descriptor = reinterpret_cast<FogDescriptor *>(set->mpDescriptor.get());
            hdWood_unpackColor(&frame->mConverted,
                               j ? reinterpret_cast<const be<s16> *>(light[2]->mColorS10)
                                 : tev->mColorC0);
            woodConvertFogColor(gabi::ea(&frame->mFog[i * 2 + j]), gabi::ea(&frame->mConverted),
                                (j ? light[2] : light[0])->mFactors[1]);
            const be<u32> *source = reinterpret_cast<const be<u32> *>(&frame->mFog[i * 2 + j]);
            for (u32 k = 0; k < 4; ++k)
                descriptor->mColors[j][k] = source[k];
        }
        woodTransformSetUpdate(gabi::ea(set));
    }
    const f32 fixedAlpha = emitterColorOne, divisor = colorDivisor;
    u32 normalIndex = 0, textureIndex = 0, fadeIndex = 0;
    Room_c *instanceHeader = &packet->mFadeUnits;
    instanceHeader->mCount = 0;
    for (u32 room = 0; room < 64; ++room) {
        Room_c *header = &packet->mRoom[room];
        const u32 begin = gabi::ea(header->mpUnits), end = begin + (u32)header->mCount * 4;
        if (begin != end) {
            woodSortPointerArray(gabi::ea(header), 0x025D0BD0);
            textureColors(&packet->mRoomTextures[textureIndex], tev, fixedAlpha, divisor,
                          textureSet);
            ++textureIndex;
        }
        for (u32 address = begin; address != end; address += 4) {
            auto *entry = reinterpret_cast<gptr<Unit_c> *>((uintptr_t)address);
            Unit_c *unit = *entry;
            if ((u32)unit->mFlags & Unit_c::STATE_FRUSTUM_CULLED)
                continue;
            const u32 index = unit->mAnmIdx;
            if (index >= 72) {
                wood_assert(gabi::guest_addr("d_wood.cpp"), 0xC61,
                            gabi::guest_addr("anmID > AnmID_Err && anmID < AnmID_Max"));
                unit = *entry;
            }
            const u8 alpha = packet->mAnm[index].mAlphaScale;
            Transform *normalBase = packet->mRoomTransforms;
            if (alpha && alpha == 255 && !((u32)unit->mFlags & Unit_c::STATE_CUT)) {
                copyTransform(&frame->mNormalMatrix, &unit->mModelViewMtx,
                              &normalBase[normalIndex]);
                ++normalIndex;
                unit = *entry;
            } else if (!alpha || ((u32)unit->mFlags & Unit_c::STATE_CUT)) {
                copyTransform(&frame->mTrunkMatrix, &unit->mTrunkModelViewMtx,
                              &normalBase[normalIndex]);
                ++normalIndex;
                continue;
            } else {
                TextureTransform *texture = &packet->mFadeTextures[fadeIndex];
                textureColors(texture, tev, (f32)((f64)(f32)alpha / (f64)divisor), divisor,
                              transformSet);
                unit = *entry;
                copyTransform(&frame->mFadeMatrix, &unit->mModelViewMtx,
                              &packet->mFadeTransforms[fadeIndex]);
                ++fadeIndex;
                if ((s32)instanceHeader->mCount < (s32)instanceHeader->mCapacity) {
                    instanceHeader->mpUnits[(u32)instanceHeader->mCount] = *entry;
                    instanceHeader->mCount = (u32)instanceHeader->mCount + 1;
                }
                unit = *entry;
            }
            copyTransform(&frame->mTrunkMatrix, &unit->mTrunkModelViewMtx,
                          &normalBase[normalIndex]);
            ++normalIndex;
        }
    }
}

/* WWHD glue
data woodGraphicsLock 101F8B18
data woodResourceManager 101F4F7C
data woodVertexOffsets 10056AA8
str 100568E0 "d_wood.cpp" in 025CD868
str 100568B4 "(i_mode >= 0) && (i_mode < Anm_c::Mode_Max)" in 025CD868
str 100569E4 "d_wood.cpp" in 025D077C
str 100569F0 "(room_no >= 0) && (room_no < L_Room_Max)" in 025D077C
str 10056A6C "d_wood.cpp" in 025D16E0
str 10056A78 "anmID > AnmID_Err && anmID < AnmID_Max" in 025D16E0
str 10056A30 "d_wood.cpp" in 025D0BEC
str 10056A3C "anmID > AnmID_Err && anmID < AnmID_Max" in 025D0BEC
str 100569A0 "wood_draw"
str 100569AC "wood_draw_xlu"
str 100569BC "ProgramTexture"
str 100569CC "ProgramTexture.bfres"
str 10056994 "Txa_swood_b"
data woodGeometry 101F13C8
data woodProjection 104B470C
data woodCamera 104B4708
data woodInitState 10487358
data woodDestructorRecords 101F2F44
data woodInitFirstValue 10056AA0
data woodInitSecondValue 10056AA4
data textureZero 10145180
data textureOne 1014517C
data colorDivisor 10056A24
data cullRadius 10056A64
data woodFrustum 1048CFF0
data woodDrawList 104B4634
data woodCurrentDrawList 104B4638
data heapManager 101F8B4C
data roomDrawDepth 10056A68
data shaderCacheContext 104B45C0
data defaultGraphicsState 104B474C
data collisionRadius 10056A1C
data cutCollisionRadius 10056A20
ext 02018428 void woodMassSetHeight(u32 a0, f32 a1)
ext 020184DC void woodMassSetRadius(u32 a0, f32 a1)
ext 020195B0 s32 woodAtan2(f32 a0, f32 a1)
ext 025200D4 u32 woodGetGameInfo()
ext 02555D0C u32 woodGetEnvironment()
ext 0255FAF0 void woodSetLightColor(u32 a0)
ext 0255FFF4 void woodInitLight(u32 a0, u32 a1, u32 a2)
ext 025626A4 void woodSetupEnvironment(u32 a0, u32 a1, u32 a2, u32 a3)
ext 02578348 f32 woodGetWindPower()
ext 0257DAA8 u32 woodGetWindVector()
ext 026124B0 u32 woodGetResource(u32 a0, u32 a1, u32 a2, u32 a3)
ext 0273AD10 u32 woodAllocate(u32 a0)
ext 0273AF40 void woodFree(u32 a0)
ext 0273B560 void woodCreatePointerArray(u32 a0, u32 a1, u32 a2)
ext 0273B7B4 void woodSortPointerArray(u32 a0, u32 a1)
ext 0274D2AC void woodScaleColor(u32 a0, f32 a1)
ext 0274D458 void woodConvertFogColor(u32 a0, u32 a1, f32 a2)
ext 0274FBF8 void woodLockGraphics(u32 a0)
ext 0274FCCC void woodUnlockGraphics(u32 a0)
ext 02750250 void woodGraphicsStateConstruct(u32 a0)
ext 02750370 void woodGraphicsStateApply(u32 a0)
ext 02755FEC u32 woodFindHeap(u32 a0, u32 a1)
ext 02756140 u32 woodGetHeap(u32 a0)
ext 02773870 void woodLoadSamplerResource(u32 a0, u32 a1, u32 a2)
ext 027B5430 void woodIndexBufferConstruct(u32 a0)
ext 027B54A0 void woodIndexBufferDestroy(u32 a0, u32 a1)
ext 027B54E0 void woodIndexBufferSet(u32 a0, u32 a1, u32 a2, u32 a3)
ext 027B5BD8 void woodVertexBufferConstruct(u32 a0)
ext 027B5CBC void woodVertexBufferDestroy(u32 a0, u32 a1)
ext 027B5E94 void woodVertexBufferFlush(u32 a0, u32 a1, u32 a2)
ext 027B90AC s32 woodFindMaterialIndex(u32 a0, u32 a1)
ext 027B9F68 void woodBindShaderState(u32 a0)
ext 027BB7CC void woodBindShaderProgram(u32 a0)
ext 027BB9E0 void woodUpdateShaderProgram(u32 a0, u32 a1)
ext 027BDEB4 void woodCopySamplerState(u32 a0, u32 a1)
ext 027BDF7C void woodSamplerConstruct(u32 a0)
ext 027BE2B0 void woodSamplerDestroy(u32 a0, u32 a1)
ext 027BE53C void woodBindSamplers(u32 a0, u32 a1, u32 a2, u32 a3)
ext 027BE6B8 void woodSamplerResourceConstruct(u32 a0)
ext 027BF734 void woodShapeConstruct(u32 a0)
ext 027BF7E8 void woodShapeClear(u32 a0)
ext 027BF880 void woodShapeDestroy(u32 a0, u32 a1)
ext 027BFE5C void woodShapeBind(u32 a0)
ext 027F0E04 void woodDrawListEntry(u32 a0, u32 a1, u32 a2)
ext 027F1278 void woodPacketBaseConstruct(u32 a0)
ext 027F13DC void woodPacketBaseDestroy(u32 a0, u32 a1)
ext 027F29D4 u32 woodGetShaderCache(u32 a0)
ext 027FB40C void woodUniformConstruct(u32 a0)
ext 027FB528 void woodUniformDestroy(u32 a0, u32 a1)
ext 027FB5D4 void woodUniformSetState(u32 a0, u32 a1)
ext 027FB678 void woodUniformUpdate(u32 a0)
ext 027FD6F4 void woodTransformSetConstruct(u32 a0)
ext 027FD764 void woodTransformSetDestroy(u32 a0, u32 a1)
ext 027FD838 void woodTransformSetConfigure(u32 a0, u32 a1, u32 a2)
ext 027FDA54 void woodTransformSetView(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4)
ext 027FE010 void woodTransformSetUpdate(u32 a0)
ext 027FF478 void woodConfigureVertexBuffer(u32 a0, u32 a1, u32 a2, u32 a3)
ext 027FF530 void woodConfigureShape(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4)
ext 027FFCBC u32 woodGetMaterialManager()
ext 027FFE54 void woodApplyDrawObject(u32 a0, u32 a1)
ext 0280037C void woodApplyDrawMode(u32 a0, u32 a1)
ext 0280068C void woodBindMaterialGroups(u32 a0, u32 a1, u32 a2)
ext 02800B0C void woodCreateMaterialInstance(u32 a0, u32 a1, u32 a2)
ext 02838148 s32 woodFrustumCheck(u32 a0, u32 a1, u32 a2, f32 a3)
ext 028EAB2C void woodSecondStaticFlagConstruct(u32 a0)
ext 028ED6F8 void woodFirstStaticFlagConstruct(u32 a0)
ext 028EFFD0 void woodArrayConstruct(u32 a0, u32 a1, u32 a2, u32 a3)
ext 028F0164 void woodArrayDestroy(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5)
ext 028F026C void woodRegisterStatic(u32 a0)
ext 028F521C void woodClearUniformData(u32 a0, u32 a1)
ext C00060E0 void woodGX2BindProgram(u32 a0, u32 a1)
ext C0006178 void woodGX2DrawIndexed(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5)
ext C00068A8 void woodGX2GeometryUniform(u32 a0, u32 a1, u32 a2)
ext C0006900 void woodGX2PixelUniform(u32 a0, u32 a1, u32 a2)
ext C0006A38 void woodGX2VertexUniform(u32 a0, u32 a1, u32 a2)
data anm_norm_num 101F2F40
data initialLight 1016E414
data woodViewMatrix 104B45F8
data emitterColorOne 100566E0
data collisionPushDistance 10056930
data groundHeightMinimum 1005692C
data groundCheckYOffset 10056928
str 10056870 "d_wood.cpp" in 025CD800
str 1005687C "(anm_id_norm >= 0) && (anm_id_norm < AnmID_Norm_Max)"
str 1005691C "d_wood.h" in 025CDEE8
str 10056958 "d_wood.h" in 025CE5D0
str 10056934 "(int)id >= 0 && (int)id < AnmID_Max" in 025CE5D0
str 10056988 "d_wood.h" in 025CEA84
str 10056964 "(int)id >= 0 && (int)id < AnmID_Max" in 025CEA84
str 100568F8 "(int)id >= 0 && (int)id < AnmID_Max"
data mode_proc 100566E8
data woodZero 100566E4
data woodCutInitVelY 10056718
data woodCutYAccel 1005671C
data woodCutMinVelY 10056720
data woodCutZVel 10056724
data woodPushBackTimeScale 100568EC
data woodWindMedium 100568F0
data woodWindStrong 100568F4
data M_init_num 101F2F68
data L_attr_kSways 100567D4
ext 028E9098 void PSMTXIdentity(u32 matrix)
ext 025F1884 void cMtx_YrotS(u32 matrix, s32 angle)
ext 025F24E0 void wood_stack_transM(f32 x, f32 y, f32 z)
ext 025F1BF4 void cMtx_XrotM(u32 matrix, s32 angle)
ext 025F1C28 void cMtx_YrotM(u32 matrix, s32 angle)
ext 028E90D4 void PSMTXCopy(u32 source, u32 target)
ext 0200F8D0 void cLib_chaseAngleS(u32 value, s32 target, s32 step)
ext 0200F564 void cLib_chaseS(u32 value, s32 target, s32 step)
ext 0200F378 void cLib_addCalcAngleS(u32 value, s32 target, s32 scale, s32 step, s32 minimum)
ext 0200ECCC void woodMemorySet(void* object, u32 value, u32 size)
ext 02008E0C void woodGroundConstruct(cBgS_GndChk* check)
ext 02008DAC void woodGroundDestruct(cBgS_GndChk* check, s32 flags)
ext 0251694C void woodHitConstruct(dWood::HitInf* hit)
ext 025170D8 u32 woodCheckMass(void* cc, cXyz* pos, gptr<fopAc_ac_c>* actor, dWood::HitInf* hit)
ext 028E9108 void woodMatrixConcat(Mtx34* first, Mtx34* second, Mtx34* output)
ext 0273B560 void woodRoomArrayCreate(dWood::Room_c* room, s32 capacity, gptr<dWood::Unit_c>* storage)
ext 0281E5A8 void woodEmitterSetColor(JPABaseEmitter* emitter, dWood::EmitterColor* color)
ext 0200F93C s32 woodTargetAngleY(const cXyz* from, const cXyz* to)
ext 0273AA24 void wood_assert(u32 file, u32 line, u32 text)
*/
