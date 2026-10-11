/* WWHD flame pillar, reconstructed from HD disassembly. */
#include "d/actor/d_a_obj_flame.h"
using daObjFlame::Act_c;
enum { JA_SE_FIREBLAST_OMEN = 0x704E, JA_SE_FIREBLAST_BLOW = 0x380D, JA_SE_FIREBLAST_NOISE = 0x7009 };
enum { MODE_WAIT, MODE_WAIT2, MODE_L_BEFORE, MODE_L_U, MODE_U, MODE_U_L, MODE_L_AFTER };
// HD: attribute records are 0x58 bytes. Names describe verified uses where tww has no table layout.
struct FlameAttr {
    be<f32> mRadiusScale, mVerticalScale, mModelScaleXZ, mModelScaleY;
    be<s32> mBdlIndex, mBtkIndex, mBrkIndex;
    be<u32> mHeapSize;
    be<f32> mAnimSpeed, mTimerStep;
    u8 _28[2];
    be<u16> mFootEffect;
    be<u8> mSimpleEmitter, mBlockEmitter;
    u8 _2E[2];
    be<f32> mEmitter0ScaleXZ, mEmitter0ScaleY, mEmitter1ScaleXZ, mEmitter1ScaleY, mEmitter2ScaleXZ, mEmitter2ScaleY;
    be<s16> mCullMinX, mCullMinY, mCullMinZ, mCullMaxX, mCullMaxY, mCullMaxZ;
    be<f32> mCollisionHeight;
};
WWHD_SIZE(FlameAttr, 0x58);
struct FlameAttributes {
    FlameAttr mType[4];
};
static FlameAttributes l_attr;
struct FlameEmitter {
    u8 _00[0x5C];
    be<s32> mMaxFrame;
    u8 _60[0x1F0 - 0x60];
    Mtx34 mGlobalRotation;
    u8 _220[0xC];
    cXyz mGlobalTranslation;
    u8 _238[0x1C];
    be<u32> mStatus;
};
struct FlameModeTable {
    gabi::ptmf<void (Act_c::*)()> mProc[7];
};
static FlameModeTable l_modeProc;
struct FlameVtable {
    be<u32> mEntry;
};
static FlameVtable l_actorVtable;
static FlameVtable l_aabVtable;
static FlameVtable l_shapeVtable;
static FlameVtable l_capsuleVtable;
static FlameVtable l_hitVtable;
static FlameVtable l_cpsShapeVtable;
struct FlameVecWords {
    be<u32> mX, mY, mZ;
};
static dCcD_SrcCps M_cps_src;
static cXyz l_origin;
struct FlameRaiseFlag {
    be<f32> mValue;
};
static FlameRaiseFlag l_raiseFlag;
struct FlameGameDrawView {
    u8 _00[0x5D80];
    be<u32> mEffectList;
};
struct FlameDrawBuffers {
    be<u32> mOpa, mXlu;
};
static FlameDrawBuffers l_drawBuffers;
struct FlameStageInfo {
    u8 _00[15];
    be<u8> mTimeStep;
};
struct FlameCapsule {
    cXyz mStart, mEnd;
    be<u32> __vtbl;
    be<f32> mRadius;
};
struct FlameCpsView : dCcD_GObjInf {
    u8 _F8[0x18];
    be<u32> mAabVtable, mShapeVtable;
    FlameCapsule mCapsule;
};
WWHD_SIZE(FlameCpsView, 0x138);
struct FlameLiftVtable {
    u8 _00[0x14];
    be<u32> mLift;
    u8 _18[4];
    be<u32> mWait;
};
struct FlamePlatformView : fopAc_ac_c {
    u8 _3AC[10];
    be<s16> mType;
};
const gc_tu_data gc_tu = {.safeString = 0x10028BCC, .headerStatics = {0x10469BA0, 0x101C9474, 0x10469BAC}};
u32 Flame_schedule();
s32 Flame_scheduleTimer();
s32 Flame_prm(const Act_c*, s32, s32);
u32 Flame_audio(u32, cXyz*, u32, s32);
void Flame_gObjCtor(dCcD_GObjInf*);
void Flame_capsuleCtor(FlameCapsule*);
void Flame_capsuleSet(FlameCapsule*, cXyz*);
void Flame_cpsSet(dCcD_Cps*, const dCcD_SrcCps*);
void Flame_gObjDtor(dCcD_GObjInf*, s32);
mDoExt_btkAnm* Flame_btkCtor(mDoExt_btkAnm*);
mDoExt_brkAnm* Flame_brkCtor(mDoExt_brkAnm*);
f32 Flame_sqrt(f32);
f32 Flame_addCalc(be<f32>*, f32, f32, f32, f32);
BOOL Flame_isActor(fopAc_ac_c*);
BOOL Flame_isCarried(u32);
using FlameJudge = s32 (*)(fopAc_ac_c*, Act_c*);
void Flame_search(FlameJudge, Act_c*);
static void removeEmitter(gptr<FlameEmitter>& slot) {
    FlameEmitter* emitter = slot;
    if (emitter) {
        u32 flags = emitter->mStatus;
        emitter->mMaxFrame = -1;
        emitter->mStatus = flags | 1;
        slot = nullptr;
    }
}

// HD: actor process numbers differ from the GC enum.
enum { fpcNm_KI_e = 0xD7, fpcNm_Obj_Magmarock_e = 0x2A, fpcNm_MFLFT_e = 0x5B };
namespace dPa_name {
enum { ID_AK_SP_O_FIREHOLEKAGEROU = 0xC06B, ID_AK_SN_O_FIRESHAFTHEAD = 0x805A, ID_AK_SN_O_FIRESHAFTBODY = 0x805B };
}
s32 liftup_magmarock(fopAc_ac_c*, Act_c*);
s32 liftup_mflft(fopAc_ac_c*, Act_c*);
void Flame_modeProc(Act_c*);
void Flame_setMtx(Act_c*);

bool Flame_solidHeapCB(Act_c*);
using FlameHeapCallback = bool (*)(Act_c*);
s32 Flame_entryHeap(Act_c*, FlameHeapCallback, u32);
f32 Flame_squareDistance(const cXyz*, const cXyz*);
static FlameLiftVtable* Flame_liftVtable(fopAc_ac_c* actor) {
    // HD: the guest vtable uses raw function entries at 0x14 and 0x1C.
    gptr<FlameLiftVtable>& table = reinterpret_cast<gptr<FlameLiftVtable>&>(actor->__vtbl);
    return table;
}

s32 daObjFlame::Act_c::prm(s32 width, s32 shift) const {
    return Flame_prm(this, width, shift);
}

/* 02344974 */
void daObjFlame::Act_c::set_switch() {
    if (mType != 1)
        return;
    s32 mode = mModeProc;
    bool active = mode == MODE_L_U || mode == MODE_U;
    s32 sw = prm(8, 16);
    s32 room = home.roomNo;
    if (active)
        dComIfGs_onSwitch(sw, room);
    else
        dComIfGs_offSwitch(sw, room);
}
/* 023432B0 */
// HD: free binding for the tww static heap callback.
bool Flame_solidHeapCB(Act_c* actor) {
    return actor->create_heap();
}

/* 02342FC4 */
bool daObjFlame::Act_c::create_heap() {
    auto attr = [&]() {
        u32 type = mType;
        if (type >= 4) {
            JUT_ASSERT_fail("d_a_obj_flame.cpp", 0x1C0, "(M_scl >= 0) && (M_scl < Scl_Max)");
            type = mType;
        }
        return &l_attr.mType[type];
    };
    FlameAttr* a = attr();
    auto* data = static_cast<J3DModelData*>(dComIfG_getObjectRes("Yfire_00", a->mBdlIndex));
    if (!data)
        JUT_ASSERT_fail("d_a_obj_flame.cpp", 0x1F5, "mdl_data != 0");
    mpModel = mDoExt_J3DModel__create(data, 0, 0x11020203);
    a = attr();
    auto* animation = static_cast<J3DAnmTextureSRTKey*>(dComIfG_getObjectRes("Yfire_00", a->mBtkIndex));
    auto* btk = static_cast<mDoExt_btkAnm*>(operator_new(0x74));
    if (btk)
        btk = Flame_btkCtor(btk);
    mpBtkAnm = btk;
    if (!animation) {
        JUT_ASSERT_fail("d_a_obj_flame.cpp", 0x1FE, "btk != 0");
        btk = mpBtkAnm;
    }
    s32 btkOK = 0;
    if (btk) {
        u32 type = mType;
        if (type >= 4) {
            JUT_ASSERT_fail("d_a_obj_flame.cpp", 0x1C0, "(M_scl >= 0) && (M_scl < Scl_Max)");
            btk = mpBtkAnm;
            type = mType;
        }
        f32 speed = l_attr.mType[type].mAnimSpeed;
        btkOK = btk->init(data, animation, true, J3DFrameCtrl::EMode_LOOP, speed, 0, -1, false, 0);
    }
    a = attr();
    s32 brkOK = 0;
    if (a->mBrkIndex < 0)
        brkOK = 1;
    else {
        u32 type = mType;
        if (type >= 4) {
            JUT_ASSERT_fail("d_a_obj_flame.cpp", 0x1C0, "(M_scl >= 0) && (M_scl < Scl_Max)");
            type = mType;
            a = &l_attr.mType[type];
        }
        auto* brkData = static_cast<J3DAnmTevRegKey*>(dComIfG_getObjectRes("Yfire_00", a->mBrkIndex));
        auto* brk = static_cast<mDoExt_brkAnm*>(operator_new(0x78));
        if (brk)
            brk = Flame_brkCtor(brk);
        mpBrkAnm = brk;
        if (!brkData) {
            JUT_ASSERT_fail("d_a_obj_flame.cpp", 0x214, "brk != 0");
            brk = mpBrkAnm;
        }
        if (brk) {
            type = mType;
            if (type >= 4) {
                JUT_ASSERT_fail("d_a_obj_flame.cpp", 0x1C0, "(M_scl >= 0) && (M_scl < Scl_Max)");
                brk = mpBrkAnm;
                type = mType;
            }
            f32 speed = l_attr.mType[type].mAnimSpeed;
            brkOK = brk->init(data, brkData, true, J3DFrameCtrl::EMode_LOOP, speed, 0, -1, false, 0);
        }
    }
    return mpModel != nullptr && btkOK != 0 && brkOK != 0;
}
/* 0234365C */
void daObjFlame::Act_c::create_mode_init() {
    u32 schedule = (u8)prm(8, 0);
    u32 current = static_cast<u8>(Flame_schedule());
    if (!schedule || !current) {
        mModeProc = MODE_WAIT;
        mTimer = schedule ? 0.0f : 120.0f;
        mHeight = 0.0f;
        return;
    }
    s32 timer = Flame_scheduleTimer();
    auto* config = static_cast<FlameStageInfo*>(dComIfGp_getStageStagInfo());
    u32 step = u32(config->mTimeStep) * 30;
    if (schedule < current)
        schedule <<= 8;
    u32 shifts = 0;
    while (!(schedule & current)) {
        current <<= 1;
        ++shifts;
    }
    timer = (s32)(step * shifts + (u32)timer);
    u32 type = mType;
    auto attr = [&]() {
        if (type >= 4) {
            JUT_ASSERT_fail("d_a_obj_flame.cpp", 0x1C0, "(M_scl >= 0) && (M_scl < Scl_Max)");
            type = mType;
        }
        return &l_attr.mType[type];
    };
    FlameAttr* a = attr();
    f32 phase = (f32)timer * a->mTimerStep;
    if (type != 1)
        phase -= 127.0f;
    s32 mode;
    f32 remain, height;
    if (phase < 0.0f) {
        mode = MODE_WAIT2;
        remain = phase + 127.0f;
        height = 0.0f;
    } else if (phase < 23.0f) {
        mode = MODE_L_BEFORE;
        remain = 23.0f - phase;
        height = 0.0f;
    } else if (phase < 45.0f) {
        mode = MODE_L_U;
        remain = 45.0f - phase;
        height = 0.04545454680919647f * (22.0f - remain);
    } else if (phase < 135.0f) {
        mode = MODE_U;
        remain = 135.0f - phase;
        height = 1.0f;
    } else if (phase < 160.0f) {
        mode = MODE_U_L;
        remain = 160.0f - phase;
        height = 0.03999999910593033f * remain;
    } else if (phase < 180.0f) {
        mode = MODE_L_AFTER;
        remain = 180.0f - phase;
        height = 0.0f;
    } else {
        mTimer = 0.0f;
        mModeProc = MODE_WAIT;
        mHeight = 0.0f;
        return;
    }
    mModeProc = mode;
    mTimer = remain;
    mHeight = height;
    if (mode != MODE_WAIT2) {
        f32 fraction = phase * 0.0055555556900799274f;
        mDoExt_btkAnm* btk = mpBtkAnm;
        if (btk)
            btk->setFrame(gabi::fmadds(fraction, btk->getEndFrame(), 1.f));
        mDoExt_brkAnm* brk = mpBrkAnm;
        if (brk)
            brk->setFrame(gabi::fmadds(fraction, brk->getEndFrame(), 1.f));
        mode = mModeProc;
    }
    if (mode == MODE_WAIT)
        return;
    u32 index = (u32)(mode - 1);
    if (index <= 3) {
        mode = mModeProc;
        index = (u32)(mode - 1);
        mEm0State = 1;
    }
    type = mType;
    if (index <= 4) {
        mEm1State = 1;
        mEm2State = 1;
    }
    a = attr();
    if (!a->mSimpleEmitter)
        em_manual_set();
    m457 = 1;
}

/* 02343AE4 */
void daObjFlame::Act_c::set_mtx() {
    f32 x = scale.x, z = scale.z, y = scale.y;
    J3DModel* model = mpModel;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    x = current.pos.x;
    y = current.pos.y;
    z = current.pos.z;
    Mtx34* mtx = mDoMtx_stack_c::get();
    PSMTXTrans(mtx, x, y, z);
    s16 rz = shape_angle.z, rx = shape_angle.x, ry = shape_angle.y;
    mDoMtx_ZXYrotM(mtx, rx, ry, rz);
    ry = mRotY;
    mDoMtx_YrotM(mtx, ry);
    f32 matrix[12];
    for (u32 i = 0; i < 12; ++i)
        matrix[i] = mtx->m[i / 4][i % 4];
    model = mpModel; // HD: this assignment does not preserve a null model pointer.
    Mtx34* destination = &model->mBaseTransformMtx;
    for (u32 i = 0; i < 12; ++i)
        destination->m[i / 4][i % 4] = matrix[i];
    u32 type = mType;
    auto attr = [&]() {
        if (type >= 4) {
            JUT_ASSERT_fail("d_a_obj_flame.cpp", 0x1C0, "(M_scl >= 0) && (M_scl < Scl_Max)");
            type = mType;
        }
        return &l_attr.mType[type];
    };
    FlameAttr* attrs[6];
    for (u32 i = 0; i < 6; i++)
        attrs[i] = attr();
    f32 minx = attrs[0]->mCullMinX, miny = attrs[1]->mCullMinY, minz = attrs[2]->mCullMinZ;
    f32 maxx = attrs[3]->mCullMaxX, maxy = attrs[4]->mCullMaxY, maxz = attrs[5]->mCullMaxZ;
    f32 height = mHeight;
    fopAcM_setCullSizeBox(this, minx, miny, minz, maxx, maxy * height, maxz);
}
/* 02343D78 */
void daObjFlame::Act_c::init_mtx() {
    set_mtx();
}

/* 02343D7C */
void daObjFlame::Act_c::em_position() {
    if (!(s8)m457)
        return;
    Mtx34* mtx = mDoMtx_stack_c::get();
    f32 z = current.pos.z, y = current.pos.y, x = current.pos.x;
    PSMTXTrans(mtx, x, y, z);
    s16 rz = shape_angle.z, rx = shape_angle.x, ry = shape_angle.y;
    mDoMtx_ZXYrotM(mtx, rx, ry, rz);
    u32 type = mType;
    auto attr = [&]() {
        if (type >= 4) {
            JUT_ASSERT_fail("d_a_obj_flame.cpp", 0x1C0, "(M_scl >= 0) && (M_scl < Scl_Max)");
            type = mType;
        }
        return &l_attr.mType[type];
    };
    FlameAttr* a = attr();
    if (!a->mSimpleEmitter) {
        if (mpEmitter0) {
            mDoMtx_stack_c::push();
            type = mType;
            FlameAttr* a0 = attr();
            FlameAttr* a1 = attr();
            f32 height = mHeight, sy1 = a1->mModelScaleY, sy0 = a0->mModelScaleY, extra = mExtraScaleY;
            f32 offset = gabi::fmadds((height * 1500.0f) * sy0, extra, (-300.0f * sy1) * extra);
            mDoMtx_stack_c::transM(0.0f, offset, 0.0f);
            FlameEmitter* em = mpEmitter0;
            JPASetRMtxTVecfromMtx(mtx, &em->mGlobalRotation, &em->mGlobalTranslation);
            mDoMtx_stack_c::pop();
        }
        FlameEmitter* em = mpEmitter1;
        if (em)
            JPASetRMtxTVecfromMtx(mtx, &em->mGlobalRotation, &em->mGlobalTranslation);
        type = mType;
    }
    a = attr();
    f32 height = mHeight, sy = a->mVerticalScale, ratio = m46C;
    mDoMtx_stack_c::transM(0.0f, ((height * 1500.0f) * sy) * ratio, 0.0f);
    PSMTXMultVec(mtx, &l_origin, &eyePos);
    height = mHeight;
    m434 = gabi::fmsubs(1500.0f, height, 300.0f) > 0.0f;
    if (!m434)
        return;
    type = mType;
    a = attr();
    sy = a->mVerticalScale;
    ratio = m46C;
    mDoMtx_stack_c::transM(0.0f, (-300.0f * sy) * ratio, 0.0f);
    PSMTXMultVec(mtx, &l_origin, &mCpsP1);
}

/* 02344014 */
void daObjFlame::Act_c::em_simple_set() {
    u32 type = mType;
    auto attr = [&]() {
        if (type >= 4) {
            JUT_ASSERT_fail("d_a_obj_flame.cpp", 0x1C0, "(M_scl >= 0) && (M_scl < Scl_Max)");
            type = mType;
        }
        return &l_attr.mType[type];
    };
    FlameAttr* a = attr();
    s8 state = mEm0State;
    bool blocked = a->mBlockEmitter != 0 && m459 != 0;
    auto particle = [](u32 id, cXyz* pos) { dComIfGp_particle_setSimple(id, pos); };
    if (state == 1 && !blocked) {
        f32 x = eyePos.x;
        a = attr();
        f32 sy = a->mVerticalScale, extra = mExtraScaleY, y = eyePos.y, z = eyePos.z;
        gabi::Local<cXyz> pos;
        pos->x = x;
        pos->y = gabi::fmadds(-300.0f * sy, extra, y);
        pos->z = z;
        particle(dPa_name::ID_AK_SN_O_FIRESHAFTHEAD, pos.get());
    }
    if ((s8)mEm1State == 1)
        particle(dPa_name::ID_AK_SN_O_FIRESHAFTBODY, &eyePos);
    if ((s8)mEm2State == 1 && !blocked) {
        type = mType;
        a = attr();
        u32 id = a->mFootEffect;
        particle(id, &home.pos);
    }
}

/* 023441EC */
void daObjFlame::Act_c::em_simple_inv() {
    s8 a = mEm0State, b = mEm1State;
    if (a == 3)
        mpEmitter0 = 0;
    s8 c = mEm2State;
    if (b == 3)
        mpEmitter1 = 0;
    if (c == 3)
        mpEmitter2 = 0;
}
/* 023432B4 */
void daObjFlame::Act_c::em_manual_set() {
    u32 type = mType;
    auto attr = [&]() {
        if (type >= 4) {
            JUT_ASSERT_fail("d_a_obj_flame.cpp", 0x1C0, "(M_scl >= 0) && (M_scl < Scl_Max)");
            type = mType;
        }
        return &l_attr.mType[type];
    };
    FlameAttr* a = attr();
    bool blocked = a->mBlockEmitter != 0 && m459 != 0;
    auto particle = [&](u32 id, cXyz* scale) {
        return reinterpret_cast<FlameEmitter*>(dPa_control_set(dComIfGp_getParticle(), 0, id, &home.pos, &home.angle,
                                                               scale, 255, nullptr, -1, nullptr, nullptr, nullptr));
    };
    if ((s8)mEm0State == 1 && !blocked && type != 1) {
        a = attr();
        f32 x = a->mEmitter0ScaleXZ;
        a = attr();
        f32 extra = mExtraScaleY, y = a->mEmitter0ScaleY * extra;
        a = attr();
        f32 z = a->mEmitter0ScaleXZ;
        gabi::Local<cXyz> scale;
        scale->x = x;
        scale->y = y;
        scale->z = z;
        mpEmitter0 = particle(dPa_name::ID_AK_SN_O_FIRESHAFTHEAD, scale.get());
        mEm0State = 2;
    }
    if ((s8)mEm1State == 1) {
        type = mType;
        a = attr();
        f32 x = a->mEmitter1ScaleXZ;
        a = attr();
        f32 extra = mExtraScaleY, y = a->mEmitter1ScaleY * extra;
        a = attr();
        f32 z = a->mEmitter1ScaleXZ;
        gabi::Local<cXyz> scale;
        scale->x = x;
        scale->y = y;
        scale->z = z;
        mpEmitter1 = particle(dPa_name::ID_AK_SN_O_FIRESHAFTBODY, scale.get());
        mEm1State = 2;
    }
    if ((s8)mEm2State == 1 && !blocked) {
        type = mType;
        a = attr();
        f32 x = a->mEmitter2ScaleXZ;
        a = attr();
        f32 y = a->mEmitter2ScaleXZ;
        a = attr();
        f32 z = a->mEmitter2ScaleXZ;
        gabi::Local<cXyz> scale;
        scale->x = x;
        scale->y = y;
        scale->z = z;
        a = attr();
        u32 id = a->mFootEffect;
        mpEmitter2 = particle(id, scale.get());
        mEm2State = 2;
    }
}

/* 02344230 */
void daObjFlame::Act_c::em_manual_inv() {
    u32 type = mType;
    if (type >= 4) {
        JUT_ASSERT_fail("d_a_obj_flame.cpp", 0x1C0, "(M_scl >= 0) && (M_scl < Scl_Max)");
        type = mType;
    }
    bool enabled = l_attr.mType[type].mBlockEmitter != 0;
    s8 first = mEm0State;
    bool stop = enabled && m459 != 0;
    s8 last = mEm2State;
    if (first == 2 && stop) {
        first = 3;
        mEm0State = 3;
    }
    if (last == 2 && stop) {
        first = mEm0State;
        mEm2State = 3;
    }
    if (first == 3)
        removeEmitter(mpEmitter0);
    if ((s8)mEm1State == 3)
        removeEmitter(mpEmitter1);
    if ((s8)mEm2State == 3)
        removeEmitter(mpEmitter2);
}

/* 02344A30 */
void daObjFlame::Act_c::ki_init() {
    s32 count = (s32)((u32)prm(5, 8) + 1);
    if (count != 32 && count > 0) {
        m464 = 0;
        m45A = 1;
        m460 = 0;
    }
}
/* 02345068 */
void daObjFlame::Act_c::ki_make() {
    if (!m45A)
        return;
    if (m45C > 0) {
        m45A = 0;
        m45C = (s32)((u32)(s32)m45C - 1);
        return;
    }
    s32 count = (s32)((u32)prm(5, 8) + 1);
    if (count == 32)
        count = 0;
    if (m464 >= count) {
        m45A = 0;
        m45C = prm(2, 13);
        return;
    }
    s32 timer = (s32)((u32)(s32)m460 - 1);
    if (timer > 0) {
        m460 = timer;
        return;
    }
    m460 = 5;
    m464 = (s32)((u32)(s32)m464 + 1);
    f32 angle = cM_rndFX(32768.0f);
    gabi::Local<csXyz> rotation;
    csXyz_ct(rotation.get(), 0, (s16)gabi::ftoi(angle), 0);
    s32 room = current.roomNo;
    fopAcM_create(fpcNm_KI_e, -32766, &current.pos, room, rotation.get(), nullptr, -1, 0);
}
/* 023451C8 */
void daObjFlame::Act_c::eff_hase() {
    dComIfGp_particle_setSimple(dPa_name::ID_AK_SP_O_FIREHOLEKAGEROU, &home.pos);
}
/* 02344B94 */
u32 daObjFlame::Act_c::se_fireblast_omen() {
    s32 rev = dComIfGp_getReverb(current.roomNo);
    return Flame_audio(JA_SE_FIREBLAST_OMEN, &current.pos, 0u,
                       (s32)(s8)rev); /* mDoAud_seStart: its result stays in r3 (mode_l_before tail-calls this) */
}
/* 023443A0 */
// HD: typed free callback keeps the actor-search calling convention.
s32 liftup_magmarock(fopAc_ac_c* rock, Act_c* actor) {
    if (!Flame_isActor(rock) || !rock || s16(rock->mProcName) != fpcNm_Obj_Magmarock_e)
        return 0;
    u32 id = rock ? u32(rock->mBsPcId) : 0xFFFFFFFF;
    if (Flame_isCarried(id))
        return 0;
    u32 type = actor->mType;
    auto attr = [&]() {
        if (type >= 4) {
            JUT_ASSERT_fail("d_a_obj_flame.cpp", 0x1C0, "(M_scl >= 0) && (M_scl < Scl_Max)");
            type = actor->mType;
        }
        return &l_attr.mType[type];
    };
    FlameAttr* a = attr();
    f32 ey = actor->eyePos.y, base = actor->current.pos.y, radius = gabi::fmadds(a->mRadiusScale, 145.0f, 200.0f);
    bool upper = ey < base;
    f32 low = (upper ? ey : base) - 100.0f, high = (upper ? base : ey) + 1000.0f;
    gabi::Local<cXyz> pos, target;
    pos->x = rock->current.pos.x;
    pos->y = 0.0f;
    pos->z = rock->current.pos.z;
    target->x = actor->eyePos.x;
    target->y = 0.0f;
    target->z = actor->eyePos.z;
    f32 dist = Flame_squareDistance(pos.get(), target.get());
    dist = Flame_sqrt(dist);
    if (!(dist < radius))
        return 0;
    f32 ry = rock->current.pos.y;
    if (!(ry > low) || !(ry < high))
        return 0;
    type = actor->mType;
    if (type == 1)
        return 0;
    f32 height = actor->mHeight, boost, offset = 100.0f;
    if (height < 0.10000000149011612f)
        boost = height * 2700.0f;
    else if (height > 0.8999999761581421f) {
        f32 fall = 1.0f - height;
        boost = fall * 2700.0f;
        offset = fall * 1000.0f;
    } else
        boost = 270.0f;
    a = attr();
    f32 ratio = actor->m46C, scale = a->mVerticalScale;
    offset *= scale * ratio;
    a = attr();
    scale = a->mVerticalScale;
    ratio = actor->m46C;
    f32 x = actor->eyePos.x, y = actor->eyePos.y, z = actor->eyePos.z;
    s32 mode = actor->mModeProc;
    gabi::Local<cXyz> destination;
    destination->x = x;
    destination->y = gabi::fmadds(boost, scale * ratio, y + offset);
    destination->z = z;
    auto* vt = Flame_liftVtable(rock);
    u32 call = (mode == MODE_WAIT2 || mode == MODE_L_BEFORE) ? vt->mWait : vt->mLift;
    gabi::call_ptr(call, rock, destination.get());
    actor->m459 = 1;
    return 0;
}
/* 023446B0 */
s32 liftup_mflft(fopAc_ac_c* platform, Act_c* actor) {
    if (!Flame_isActor(platform) || !platform || s16(platform->mProcName) != fpcNm_MFLFT_e)
        return 0;
    f32 desired = s16(reinterpret_cast<FlamePlatformView*>(platform)->mType) == 0 ? 1.0004417896270752f : 1.0f;
    f32 height = actor->mHeight, boost, offset;
    if (height < 0.10000000149011612f) {
        boost = height * 2700.0f;
        offset = 100.0f;
    } else if (height > 0.8999999761581421f) {
        f32 fall = 1.0f - height;
        offset = fall * 1000.0f;
        boost = fall * 2700.0f;
    } else {
        offset = 100.0f;
        boost = 270.0f;
    }
    u32 type = actor->mType;
    auto attr = [&]() {
        if (type >= 4) {
            JUT_ASSERT_fail("d_a_obj_flame.cpp", 0x1C0, "(M_scl >= 0) && (M_scl < Scl_Max)");
            type = actor->mType;
        }
        return &l_attr.mType[type];
    };
    FlameAttr* a = attr();
    f32 ratio = actor->m46C, scale = a->mVerticalScale;
    offset *= scale * ratio;
    a = attr();
    ratio = actor->m46C;
    scale = a->mVerticalScale;
    f32 y = actor->eyePos.y, base = actor->current.pos.y, x = actor->eyePos.x, z = actor->eyePos.z;
    y = gabi::fmadds(boost, scale * ratio, y + offset);
    if (y > base + 5000.0f)
        y = base + 5000.0f;
    s32 mode = actor->mModeProc;
    if (mode != MODE_WAIT2 && mode != MODE_L_BEFORE) {
        gabi::Local<cXyz> destination;
        destination->x = x;
        destination->y = y;
        destination->z = z;
        auto* vt = Flame_liftVtable(platform);
        u32 call = vt->mLift;
        gabi::call_ptr(call, platform, destination.get());
    }
    actor->m459 = 1;
    Flame_addCalc(&actor->m46C, 1.0f, 0.30000001192092896f, 0.10000000149011612f, 0.009999999776482582f);
    Flame_addCalc(&actor->mExtraScaleY, desired, 0.30000001192092896f, 0.10000000149011612f, 0.009999999776482582f);
    return 0;
}

/* 02344A8C */
void daObjFlame::Act_c::mode_wait() {
    u8 schedule = prm(8, 0);
    if (schedule) {
        if (!(Flame_schedule() & schedule))
            return;
        if (Flame_scheduleTimer())
            return;
    } else if (mTimer > 0.0f)
        return;
    if (mType == 1) {
        mpBtkAnm->setFrame(0.f);
        mDoExt_brkAnm* brk = mpBrkAnm;
        if (brk)
            brk->setFrame(0.f);
        mModeProc = MODE_L_BEFORE;
        mTimer = 23.0f;
        ki_init();
        mEm0State = 1;
        m457 = 1;
        mEm1State = 1;
        mEm2State = 1;
    } else {
        mModeProc = MODE_WAIT2;
        mTimer = 127.0f;
        m457 = 1;
        mEm0State = 1;
        mEm2State = 1;
        mEm1State = 1;
    }
}
/* 02344BDC */
void daObjFlame::Act_c::mode_wait2() {
    se_fireblast_omen();
    if (mTimer > 0.0f)
        return;
    mpBtkAnm->setFrame(0.f);
    mDoExt_brkAnm* brk = mpBrkAnm;
    if (brk)
        brk->setFrame(0.f);
    mModeProc = MODE_L_BEFORE;
    mTimer = 23.0f;
    ki_init();
}
/* 02344C50 */
u32 daObjFlame::Act_c::mode_l_before() {
    f32 timer = mTimer;
    m457 = 0;
    if (timer > 0.0f)
        return se_fireblast_omen();
    s32 rev = m458;
    mModeProc = MODE_L_U;
    mTimer = 22.0f;
    return Flame_audio(JA_SE_FIREBLAST_BLOW, &eyePos, 0u, rev); /* mDoAud_seStart */
}
/* 02344CA4 */
void daObjFlame::Act_c::mode_l_u() {
    f32 timer = mTimer;
    m457 = 1;
    mHeight = 0.04545454680919647f * (22.0f - timer);
    if (timer > 0.0f)
        return;
    mModeProc = MODE_U;
    mTimer = 90.0f;
}
/* 02344CF4 */
void daObjFlame::Act_c::mode_u() {
    if (mTimer > 0.0f)
        return;
    mModeProc = MODE_U_L;
    mEm0State = 3;
    mTimer = 25.0f;
}
/* 02344D28 */
void daObjFlame::Act_c::mode_u_l() {
    f32 timer = mTimer;
    m457 = 1;
    mHeight = 0.03999999910593033f * timer;
    if (timer > 0.0f)
        return;
    mModeProc = MODE_L_AFTER;
    mTimer = 20.0f;
    mEm2State = 3;
    mEm1State = 3;
}
/* 02344D78 */
void daObjFlame::Act_c::mode_l_after() {
    if (mTimer > 0.0f)
        return;
    mModeProc = MODE_WAIT;
    mTimer = (u8)prm(8, 0) ? 0.0f : 120.0f;
}
/* 02344DF4 */
void daObjFlame::Act_c::mode_proc_call() {
    f32 timer = mTimer;
    if (!(timer < -0.10000000149011612f)) {
        s32 mode = mModeProc;
        if (mode == MODE_WAIT || mode == MODE_WAIT2)
            mTimer = timer - 1.0f;
        else {
            u32 type = mType;
            if (type >= 4) {
                JUT_ASSERT_fail("d_a_obj_flame.cpp", 0x1C0, "(M_scl >= 0) && (M_scl < Scl_Max)");
                timer = mTimer;
                type = mType;
            }
            mTimer = timer - l_attr.mType[type].mTimerStep;
        }
    }
    (this->*l_modeProc.mProc[s32(mModeProc)])();
    auto attr = [&]() {
        u32 type = mType;
        if (type >= 4) {
            JUT_ASSERT_fail("d_a_obj_flame.cpp", 0x1C0, "(M_scl >= 0) && (M_scl < Scl_Max)");
            type = mType;
        }
        return &l_attr.mType[type];
    };
    FlameAttr* a = attr();
    if (a->mSimpleEmitter) {
        em_position();
        em_simple_set();
        em_simple_inv();
    } else {
        em_manual_set();
        em_manual_inv();
        em_position();
    }
    s32 mode = mModeProc;
    if (mode != MODE_WAIT && mode != MODE_WAIT2) {
        mpBtkAnm->play();
        mDoExt_brkAnm* brk = mpBrkAnm;
        if (brk)
            brk->play();
        s32 rev = m458;
        mDoAud_seStart(JA_SE_FIREBLAST_NOISE, &eyePos, 0, rev);
    }
    if (!m434)
        return;
    if (mModeProc == MODE_U_L) {
        a = attr();
        f32 height = mHeight, limit = a->mCollisionHeight;
        if (!(height > limit))
            return;
    }
    Flame_capsuleSet(&reinterpret_cast<FlameCpsView*>(&mCps)->mCapsule, &mCpsP0);
    cCcS_Set(dComIfG_Ccsp(), &mCps);
}

/* 0234521C */
cPhs_State daObjFlame::Act_c::_create() {
    u32 flags = actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (this) {
            fopAc_ac_c_ct(this);
            __vtbl = gabi::ea(&l_actorVtable);
            dCcD_Stts_ct(&mStts);
            auto* cps = reinterpret_cast<FlameCpsView*>(&mCps);
            Flame_gObjCtor(&mCps);
            cps->mShapeVtable = gabi::ea(&l_shapeVtable);
            cps->mAabVtable = gabi::ea(&l_aabVtable);
            Flame_capsuleCtor(&cps->mCapsule);
            flags = actor_condition;
            cps->__vtbl_hitinf = gabi::ea(&l_hitVtable);
            cps->mCapsule.__vtbl = gabi::ea(&l_capsuleVtable);
            cps->mShapeVtable = gabi::ea(&l_cpsShapeVtable);
        }
        actor_condition = flags | fopAcCnd_INIT_e;
    }
    cPhs_State result = dComIfG_resLoad(&mPhs, "Yfire_00");
    if (result != cPhs_COMPLEATE_e)
        return result;
    mType = prm(2, 28);
    auto attr = [&]() {
        u32 type = mType;
        if (type >= 4) {
            JUT_ASSERT_fail("d_a_obj_flame.cpp", 0x1C0, "(M_scl >= 0) && (M_scl < Scl_Max)");
            type = mType;
        }
        return &l_attr.mType[type];
    };
    FlameAttr* a = attr();
    u32 heap = a->mHeapSize;
    if (!Flame_entryHeap(this, Flame_solidHeapCB, heap))
        return cPhs_ERROR_e;
    // HD: these scale/position copies are integer loads and stores, preserving NaN payloads.
    auto* scaleWords = reinterpret_cast<FlameVecWords*>(&scale);
    auto* origWords = reinterpret_cast<FlameVecWords*>(&mOrigScale);
    u32 x = scaleWords->mX, y = scaleWords->mY, z;
    m46C = 1.f;
    origWords->mX = x;
    s32 type = mType;
    z = scaleWords->mZ;
    origWords->mY = y;
    origWords->mZ = z;
    mExtraScaleY = type == 1 ? 1.0004417896270752f : 1.f;
    a = attr();
    scale.x = f32(scale.x) * a->mModelScaleXZ;
    a = attr();
    f32 extra = mExtraScaleY, sy = a->mModelScaleY, oldy = scale.y;
    scale.y = oldy * (sy * extra);
    a = attr();
    f32 oldz = scale.z, sz = a->mModelScaleXZ;
    mEm2State = 0;
    mEm1State = 0;
    mEm0State = 0;
    scale.z = oldz * sz;
    create_mode_init();
    set_switch();
    J3DModel* model = mpModel;
    cullMtx = model ? gabi::ea(model->getBaseTRMtx()) : 0;
    init_mtx();
    mStts.Init(100, 255, this);
    Flame_cpsSet(&mCps, &M_cps_src);
    auto* pos = reinterpret_cast<FlameVecWords*>(&current.pos);
    auto* p0 = reinterpret_cast<FlameVecWords*>(&mCpsP0);
    auto* p1 = reinterpret_cast<FlameVecWords*>(&mCpsP1);
    z = pos->mZ;
    type = mType;
    x = pos->mX;
    y = pos->mY;
    p0->mX = x;
    p0->mY = y;
    p0->mZ = z;
    p1->mX = x;
    p1->mY = y;
    mCps.SetStts(&mStts);
    p1->mZ = z;
    a = attr();
    f32 radius = a->mRadiusScale * 145.f;
    m434 = 0;
    mCpsRad = radius;
    em_position();
    s32 rev = dComIfGp_getReverb(home.roomNo);
    m458 = rev;
    m459 = 0;
    s32 bound = static_cast<s32>(u32(prm(2, 13)) + 1);
    f32 random = cM_rndF(f32(bound));
    s32 delay = gabi::ftoi(random), mode = mModeProc;
    m45C = delay;
    if (mode != MODE_WAIT && delay == 0)
        m45C = 1;
    return result;
}

/* 023455B0 */
s32 Flame_Method_Delete(Act_c* actor) {
    dComIfG_resDelete(&actor->mPhs, "Yfire_00");
    return 1;
}
/* 023455E0 */
s32 Flame_Method_Execute(Act_c* actor) {
    auto attr = [&]() {
        u32 type = actor->mType;
        if (type >= 4) {
            JUT_ASSERT_fail("d_a_obj_flame.cpp", 0x1C0, "(M_scl >= 0) && (M_scl < Scl_Max)");
            type = actor->mType;
        }
        return &l_attr.mType[type];
    };
    FlameAttr* a = attr();
    actor->scale.x = (f32)actor->mOrigScale.x * a->mModelScaleXZ;
    a = attr();
    actor->scale.y = ((f32)actor->mOrigScale.y * a->mModelScaleY) * (f32)actor->mExtraScaleY;
    a = attr();
    actor->scale.z = (f32)actor->mOrigScale.z * a->mModelScaleXZ;
    Flame_modeProc(actor);
    actor->ki_make();
    if (!actor->prm(1, 31))
        actor->eff_hase();
    s32 type = actor->mType;
    s16 rot = actor->mRotY;
    if (type == 1) {
        f32 height = actor->mHeight;
        actor->mRotY = (s16)(rot + 400);
        if (height > 0.009999999776482582f)
            l_raiseFlag.mValue = 1.f;
    } else
        actor->mRotY = (s16)(rot + 400);
    s32 mode = actor->mModeProc;
    actor->m459 = 0;
    if ((u32)(mode - 1) <= 3) {
        Flame_search(liftup_magmarock, actor);
        if (actor->mType == 1)
            Flame_search(liftup_mflft, actor);
    }
    actor->set_switch();
    Flame_setMtx(actor);
    return 1;
}

/* 023457C8 */
s32 Flame_Method_Draw(Act_c* actor) {
    auto* env = dKy_getEnvlight();
    settingTevStruct(env, 0, &actor->current.pos, &actor->tevStr);
    env = dKy_getEnvlight();
    J3DModel* model = actor->mpModel;
    setLightTevColorType(env, model, &actor->tevStr);
    model = actor->mpModel;
    mDoExt_btkAnm* btk = actor->mpBtkAnm;
    J3DModelData* data = model->getModelData();
    f32 frame = btk->getFrame();
    btk->entry(data, frame);
    mDoExt_brkAnm* brk = actor->mpBrkAnm;
    if (brk) {
        model = actor->mpModel;
        frame = brk->getFrame();
        data = model->getModelData();
        brk->entry(data, frame);
    }
    // HD: the two draw-list fetches and restores are individually observable.
    l_drawBuffers.mOpa = reinterpret_cast<FlameGameDrawView*>(dComIfGp_get())->mEffectList;
    l_drawBuffers.mXlu = reinterpret_cast<FlameGameDrawView*>(dComIfGp_get())->mEffectList;
    model = actor->mpModel;
    mDoExt_modelUpdateDL(model);
    dComIfGd_setList();
    return 1;
}

/* 02345890 */
s32 Flame_Method_IsDelete(Act_c* actor) {
    u32 type = actor->mType;
    if (type >= 4) {
        JUT_ASSERT_fail("d_a_obj_flame.cpp", 0x1C0, "(M_scl >= 0) && (M_scl < Scl_Max)");
        type = actor->mType;
    }
    if (!l_attr.mType[type].mSimpleEmitter) {
        removeEmitter(actor->mpEmitter0);
        removeEmitter(actor->mpEmitter1);
        removeEmitter(actor->mpEmitter2);
    }
    return 1;
}
/* 0234596C */
void staticInit() {
    __sinit_header_statics();
    l_attr.mType[1].mVerticalScale = 3.3333334922790527f;
    l_attr.mType[1].mModelScaleY = 0.8149999976158142f;
    l_attr.mType[1].mEmitter0ScaleY = 2.7166666984558105f;
    l_attr.mType[1].mEmitter1ScaleY = 2.7166666984558105f;
    l_attr.mType[1].mEmitter2ScaleY = 0.5433333516120911f;
}

/* 02345A3C */
void deleteHelper(void* self, u32 flags) {
    if (self && (flags & 1))
        operator_delete(self);
}
/* 02345A50 */
void emptyVirtual(void* self) {}
/* 02345A54 */
void destroyActor(Act_c* self, u32 flags) {
    if (!self)
        return;
    Flame_gObjDtor(&self->mCps, 2);
    __dt(&self->mStts);
    __dt(static_cast<fopAc_ac_c*>(self));
    if (flags & 1)
        operator_delete(self);
}
/* 02345AC0 */
u32 PrmAbstract(const Act_c* actor, u32 width, u32 shift) {
    u32 value = u32(actor->mParameters);
    u32 mask = (width & 32) ? 0u : 1u << (width & 31);
    return ((shift & 32) ? 0u : value >> (shift & 31)) & (mask - 1);
}

/* WWHD glue
data l_attr 101C94F4
data l_modeProc 10028EAC
data l_actorVtable 10028BF4
data l_aabVtable 10028BE4
data l_shapeVtable 100015A8
data l_capsuleVtable 1004AF60
data l_hitVtable 1004AF18
data l_cpsShapeVtable 1004AF70
data M_cps_src 10028FE4
data l_origin 101FFBA8
data l_raiseFlag 1047562C
data l_drawBuffers 104B4634
str 101C94E8
str 10028F90 in 02345890
str 10028FA4 in 02345890
str 10028DC0 in 02344230
str 10028DD4 in 02344230
str 10028F58 in 023455E0
str 10028F6C in 023455E0
str 10028D0C in 02343AE4
str 10028D20 in 02343AE4
str 10028D50 in 02343D7C
str 10028D64 in 02343D7C
str 10028D88 in 02344014
str 10028D9C in 02344014
str 10028C64 in 023432B4
str 10028C78 in 023432B4
str 10028CD4 in 0234365C
str 10028CE8 in 0234365C
str 10028EE4 in 02344DF4
str 10028EF8 in 02344DF4
str 10028C04 in 02342FC4
str 10028C18 in 02342FC4
str 10028C3C in 02342FC4
str 10028C4C in 02342FC4
str 10028C58 in 02342FC4
str 10028F20 in 0234521C
str 10028F34 in 0234521C
str 10028E1C in 023443A0
str 10028E30 in 023443A0
str 10028E64 in 023446B0
str 10028E78 in 023446B0
ext 025602A8 u32 Flame_schedule()
ext 025602CC s32 Flame_scheduleTimer()
ext 02345AC0 s32 Flame_prm(const Act_c*,s32,s32)
ext 025E1A40 u32 Flame_audio(u32,cXyz*,u32,s32)
ext 02515FB8 void Flame_gObjCtor(dCcD_GObjInf*)
ext 02018150 void Flame_capsuleCtor(FlameCapsule*)
ext 020181FC void Flame_capsuleSet(FlameCapsule*,cXyz*)
ext 025164C0 void Flame_cpsSet(dCcD_Cps*,const dCcD_SrcCps*)
ext 02515980 void Flame_gObjDtor(dCcD_GObjInf*,s32)
ext 025E7C6C mDoExt_btkAnm* Flame_btkCtor(mDoExt_btkAnm*)
ext 025E80D0 mDoExt_brkAnm* Flame_brkCtor(mDoExt_brkAnm*)
ext 0200ECD4 f32 Flame_addCalc(be<f32>*,f32,f32,f32,f32)
ext 028F4384 f32 Flame_sqrt(f32)
ext 025D4604 BOOL Flame_isActor(fopAc_ac_c*)
ext 025DD868 BOOL Flame_isCarried(u32)
ext 025D5218 void Flame_search(FlameJudge,Act_c*)
ext 02344DF4 void Flame_modeProc(Act_c*)
ext 02343AE4 void Flame_setMtx(Act_c*)
ext 025D63E8 s32 Flame_entryHeap(Act_c*,FlameHeapCallback,u32)
ext 028E8DE8 f32 Flame_squareDistance(const cXyz*,const cXyz*)
*/
