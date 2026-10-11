/**
 * d_a_obj_swflat.cpp (WWHD)
 * Object - Tower of the Gods floor switch.
 *
 * Written in the style of zeldaret/tww and verified against the recompiled original.
 * HD-specific layout and ABI are explicit.
 */
#include "d/actor/d_a_obj_swflat.h"
#include "bindings.h"
#include "gc_api/f_op/f_op_actor_mng.h"
using daObjSwflat::Act_c;
static u32 PrmAbstract(Act_c*, s32, s32);
static daObjSwflat_HIO_c l_HIO;
static Mtx34 mtx;
static dCcD_SrcCyl l_cyl_src;
static constexpr u32 DCCD_STTS_VTBL = 0x1004AE88, DCCD_GSTTS_VTBL = 0x1004AEC0;
static constexpr u32 DCCD_HITINF_VTBL = 0x1004B108, DCCD_SHAPE_VTBL = 0x1004B160;
static constexpr u32 DCCD_CYL_VTBL = 0x1004B150, SHAPE_BASE_VTBL = 0x100015A8;
static constexpr u32 SAFE_STRING_VTBL = 0x1002F84C;
static constexpr u32 ACT_VTBL = 0x1002F930;
static constexpr u32 AAB_VTBL = 0x1002F864;
static constexpr u32 HIO_VTBL = 0x1002F874;

enum {
    ID_ACTIVE = 0x8164,
    ID_INACTIVE = 0x8163,
    JA_SE_SWITCH_PRESS = 0x6960,
    JA_SE_SWITCH_ACTIVE = 0x7027
};
enum { EVENT_WAIT, EVENT_ORDER, EVENT_RUN };
enum { PLAYER_PROFILE = 0xA8, CONTACT_SWITCH_WEIGHT = 0x400 };
// HD: these bindings retain the complete guest return widths and nullable allocation ABI.
SwflatBrk* swflatBrkConstruct(SwflatBrk*);
BOOL swflatBrkInit(SwflatBrk*, void*, void*, s32, s32, f32, s32, s32, s32, s32);
void swflatStatusBaseConstruct(dCcD_Stts*);
void swflatGStatusConstruct(be<u32>*);
void swflatGObjConstruct(dCcD_Cyl*);
void swflatCylinderConstruct(cM3dGCyl*);
void swflatSeStart(u32, cXyz*);
void swflatActorDestruct(fopAc_ac_c*, s32);
void swflatBrkEntry(SwflatBrk*, J3DModelData*, f32);
s32 swflatHioCreate(const char*, daObjSwflat_HIO_c*);
void swflatHioDelete(s32);
void* swflatHioConstruct(void*);
void swflatSetCullSphere(fopAc_ac_c*, f32, f32, f32, f32);
void swflatOrderPotentialEvent(fopAc_ac_c*, u16, u16, u16);

static const char* arc() {
    return "Hfbot";
}

static void invalidateEmitter(SwflatEmitter* emitter) {
    u32 flags = emitter->mFlags;
    emitter->mMaxFrame = -1;
    emitter->mFlags = flags | 1;
}

// HD: capture the save pointer before the room load; onSwitch uses the reverse order.
static BOOL isSwitch(Act_c* a, u8 sw) {
    dSv_info_c* save = dComIfGs_info();
    s8 room = a->home.roomNo;
    return save->isSwitch(sw, room);
}
static void onSwitch(Act_c* a) {
    s8 room = a->home.roomNo;
    dSv_info_c* save = dComIfGs_info();
    u8 sw = a->mSwitch;
    save->onSwitch(sw, room);
}
static void offSwitch(Act_c* a, u8 sw) {
    dSv_info_c* save = dComIfGs_info();
    s8 room = a->home.roomNo;
    save->offSwitch(sw, room);
}
static SwflatEmitter* spawnEmitter(Act_c* a, u16 id) {
    return reinterpret_cast<SwflatEmitter*>(
        dComIfGp_particle_set(id, &a->mEffectPos, &a->shape_angle));
}

static void setFrame(Act_c* a, s16 frame) {
    SwflatBrk* brk = a->mpBrk;
    a->mFrame = frame;
    brk->mFrameCtrl.mFrame = (f32)frame;
}
static void setEmitterAlpha(SwflatEmitter* emitter, u8 alpha) {
    emitter->mAlpha = alpha;
}

/* 02391724 */
static void* daObjSwflat_HIO_c__ct(void* a) {
    if (!a) {
        a = operator_new(sizeof(daObjSwflat_HIO_c));
        if (!a)
            return a;
    }
    auto hio = static_cast<daObjSwflat_HIO_c*>(a);
    hio->mRadius = 10.0f;
    hio->mCount = 0;
    hio->mEventTime = 30;
    hio->__vtbl = HIO_VTBL;
    hio->mLockTime = 30;
    hio->mNo = -1;
    hio->mbResetSwitch = 0;
    hio->mbDebug = 0;
    hio->mHeight = 20.0f;
    return a;
}

/* 023919B8 */
BOOL daObjSwflat::Act_c::CreateHeap() {
    auto data = dComIfG_getObjectRes(arc(), dRes_INDEX_HFBOT_BDL_HFBOT_e, SAFE_STRING_VTBL);
    if (!data)
        return FALSE;
    mpModel = mDoExt_J3DModel__create((J3DModelData*)data, 0, 0x11020203);
    auto animation = dComIfG_getObjectRes(arc(), dRes_INDEX_HFBOT_BRK_HFBOT_e, SAFE_STRING_VTBL);
    if (!animation)
        return FALSE;
    SwflatBrk* brk = swflatBrkConstruct(nullptr);
    mpBrk = brk;
    if (!brk)
        return FALSE;
    BOOL result = swflatBrkInit(brk, data, animation, 1, 0, 1.0f, 0, -1, 0, 0);
    return mpModel != nullptr && result != 0;
}

/* 02391BB8 */
BOOL daObjSwflat::Act_c::Create() {
    auto model = (J3DModel*)mpModel;
    cullMtx = model ? gabi::ea(&model->mBaseTransformMtx) : 0;
    init_mtx();
    swflatSetCullSphere(this, 0.0f, 0.0f, 0.0f, 90.0f);
    mStts.Init(255, 255, this);
    mCyl.Set(&l_cyl_src);
    mCyl.SetStts(&mStts);
    mType = PrmAbstract(this, 2, 0);
    mSwitch = PrmAbstract(this, 8, 8);
    mSwitch2 = PrmAbstract(this, 8, 16);
    SwflatBrk* brk = mpBrk;
    mFrame = 0;
    s16 end = brk->mFrameCtrl.mEnd;
    // HD: copy all position words before stores; retain the low signed half of fctiwz.
    auto from = reinterpret_cast<SwflatPositionWords*>(&current.pos);
    auto to = reinterpret_cast<SwflatPositionWords*>(&mEffectPos);
    u32 x = from->x, y = from->y, z = from->z;
    to->x = x;
    to->y = y;
    to->z = z;
    mLocked = 0;
    mPressEndFrame = (s16)gabi::ftoi((f32)end - 5.0f);
    if (l_HIO.mbResetSwitch == 1) {
        u8 sw = mSwitch;
        if (sw != 255)
            offSwitch(this, sw);
        sw = mSwitch2;
        if (sw != 255)
            offSwitch(this, sw);
    }
    u8 type = mType, cachedSw = mSwitch;
    BOOL active;
    if (type == 2 && isSwitch(this, mSwitch2)) {
        mType = 0;
        active = isSwitch(this, mSwitch2);
    } else {
        if (type == 2)
            mType = 1;
        active = isSwitch(this, cachedSw);
    }
    if (active) {
        mWasPressed = 1;
        mPressed = 1;
        if (mType != 0) {
            mpActiveEmitter = spawnEmitter(this, ID_ACTIVE);
            mInactiveAlpha = 0;
            mActiveAlpha = 250;
            mEventTimer = 0;
            mEventState = EVENT_WAIT;
            mLockTimer = 0;
            mpInactiveEmitter = 0;
        } else {
            mActiveAlpha = 0;
            mLocked = 1;
            mpActiveEmitter = 0;
            brk = mpBrk;
            brk->mFrameCtrl.mFrame = (f32)brk->mFrameCtrl.mEnd;
            mpInactiveEmitter = 0;
            mInactiveAlpha = 0;
            mEventState = EVENT_WAIT;
            mEventTimer = 0;
            mLockTimer = 0;
        }
    } else {
        mWasPressed = 0;
        mPressed = 0;
        mpInactiveEmitter = spawnEmitter(this, ID_INACTIVE);
        mActiveAlpha = 0;
        mInactiveAlpha = 250;
        mEventTimer = 0;
        mEventState = EVENT_WAIT;
        mLockTimer = 0;
        mpActiveEmitter = 0;
    }
    // HD: HIO uses the Shift-JIS label フラットスイッチ; glue maps the readable spelling.
    if (l_HIO.mNo < 0) {
        s32 child = swflatHioCreate("Flat switch", &l_HIO);
        l_HIO.mNo = child;
    }
    s32 count = l_HIO.mCount;
    l_HIO.mCount = count + 1;
    return TRUE;
}

/* 023917A0 */
s32 daObjSwflat::Act_c::Mthd_Create() {
    u32 condition = actor_condition;
    if (!(condition & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = ACT_VTBL;
            swflatStatusBaseConstruct(&mStts);
            swflatGStatusConstruct(&mStts.__vtbl_gstts);
            mStts.__vtbl = DCCD_STTS_VTBL;
            mStts.__vtbl_gstts = DCCD_GSTTS_VTBL;
            swflatGObjConstruct(&mCyl);
            auto shape = reinterpret_cast<SwflatShapeView*>(&mCyl);
            shape->__vtbl = SHAPE_BASE_VTBL;
            shape->mAabVtable = AAB_VTBL;
            swflatCylinderConstruct(&mCyl.mCyl);
            mCyl.__vtbl_hitinf = DCCD_HITINF_VTBL;
            shape->__vtbl = DCCD_SHAPE_VTBL;
            condition = actor_condition;
            mCyl.mCyl.__vtbl = DCCD_CYL_VTBL;
        }
        actor_condition = condition | fopAcCnd_INIT_e;
    }
    s32 phase = dComIfG_resLoad(&mPhase, arc());
    if (phase == cPhs_COMPLEATE_e) {
        phase = MoveBGCreate(arc(), dRes_INDEX_HFBOT_DZB_HFBOT_e, 0, -1);
        if (phase != cPhs_COMPLEATE_e && phase != cPhs_ERROR_e)
            JUT_ASSERT_fail("d_a_obj_swflat.cpp", 0x182,
                            "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)");
    }
    return phase;
}

/* 023927E8 */
BOOL daObjSwflat::Act_c::Delete() {
    return TRUE;
}

/* 023918E4 */
BOOL daObjSwflat::Act_c::Mthd_Delete() {
    if (heap) {
        SwflatEmitter* inactive = mpInactiveEmitter;
        if (inactive) {
            invalidateEmitter(inactive);
            mpInactiveEmitter = 0;
        }
        SwflatEmitter* active = mpActiveEmitter;
        if (active) {
            invalidateEmitter(active);
            mpActiveEmitter = 0;
        }
    }
    s8 child = l_HIO.mNo;
    if (child >= 0) {
        s32 count = l_HIO.mCount - 1;
        l_HIO.mCount = count;
        if (!count) {
            swflatHioDelete(child);
            l_HIO.mNo = -1;
        }
    }
    BOOL result = MoveBGDelete();
    dComIfG_resDelete(&mPhase, arc());
    return result;
}

/* 02391AC4 */
void daObjSwflat::Act_c::set_mtx() {
    auto stackMtx = mDoMtx_stack_c::get();
    f32 x = current.pos.x, z = current.pos.z, y = current.pos.y;
    PSMTXTrans(stackMtx, x, y, z);
    s16 rx = shape_angle.x, rz = shape_angle.z, ry = shape_angle.y;
    mDoMtx_ZXYrotM(stackMtx, rx, ry, rz);
    // HD: retain float loads of all twelve entries before the model stores.
    f32 values[12];
    for (u32 i = 0; i < 12; i++)
        values[i] = stackMtx->m[i / 4][i % 4];
    J3DModel* model = mpModel;
    for (u32 i = 0; i < 12; i++)
        model->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
    PSMTXCopy(stackMtx, &mtx);
}

/* 02391B98 */
void daObjSwflat::Act_c::init_mtx() {
    // HD: the matcher labels this function PrmAbstract; it initializes model scale.
    J3DModel* model = mpModel;
    f32 y = scale.y, x = scale.x, z = scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    set_mtx();
}

/* 02391F58 */
BOOL daObjSwflat::Act_c::Execute(Mtx34** output) {
    BOOL statue = FALSE, contact = FALSE, spawned = FALSE;
    mCyl.mCyl.SetC(&current.pos);
    if (l_HIO.mbDebug) {
        mCyl.mCyl.SetR(l_HIO.mRadius);
        mCyl.mCyl.SetH(l_HIO.mHeight);
    }
    dComIfG_Ccsp_Set(&mCyl);
    if (mCyl.ChkCoHit()) {
        auto actor = reinterpret_cast<SwflatContactView*>(mCyl.GetCoHitAc());
        if (actor && (actor->actor_status & CONTACT_SWITCH_WEIGHT)) {
            contact = TRUE;
            if (actor->mProfName == PLAYER_PROFILE)
                statue = TRUE;
            else if (actor->mProfName == fpcNm_TSUBO_e)
                actor->mPressed = 1;
        }
    }
    if (mEventTimer == 0) {
        if (contact) {
            mPressed = 1;
            mCyl.mCyl.SetR(15.0f);
        } else if (mType == 1) {
            mPressed = 0;
            mCyl.mCyl.SetR(10.0f);
        }
    }
    if (PrmAbstract(this, 2, 0) == 2 && isSwitch(this, mSwitch2)) {
        u8 locked = mLocked;
        mType = 0;
        if (!locked) {
            mLocked = 1;
            mLockTimer = l_HIO.mLockTime;
        }
    }
    if (mPressed == 1) {
        if (!mWasPressed)
            swflatSeStart(JA_SE_SWITCH_PRESS, &current.pos);
        if (!mLocked)
            swflatSeStart(JA_SE_SWITCH_ACTIVE, &current.pos);
        s16 frame = mFrame, end = mPressEndFrame;
        if (frame < end)
            setFrame(this, (s16)(frame + 1));
        if (!statue)
            onSwitch(this);
        if (!mLocked) {
            if (!mpActiveEmitter) {
                mpActiveEmitter = spawnEmitter(this, ID_ACTIVE);
                mActiveAlpha = 250;
                spawned = TRUE;
                if (PrmAbstract(this, 2, 0) == 0) {
                    mLocked = 1;
                    mLockTimer = l_HIO.mLockTime;
                }
            } else if (mActiveAlpha < 250) {
                u8 alpha = (u8)mActiveAlpha + 25;
                mActiveAlpha = alpha;
                setEmitterAlpha(mpActiveEmitter, alpha);
                onSwitch(this);
            }
        }
        if (mpInactiveEmitter) {
            u8 alpha = (u8)mInactiveAlpha - 25;
            mInactiveAlpha = alpha;
            setEmitterAlpha(mpInactiveEmitter, alpha);
            if (mInactiveAlpha == 0) {
                invalidateEmitter(mpInactiveEmitter);
                mpInactiveEmitter = 0;
            }
        }
    } else {
        s16 frame = mFrame;
        if (frame > 0)
            setFrame(this, (s16)(frame - 1));
        offSwitch(this, mSwitch);
        if (!mpInactiveEmitter) {
            mpInactiveEmitter = spawnEmitter(this, ID_INACTIVE);
            mInactiveAlpha = 250;
        } else if (mInactiveAlpha < 250) {
            u8 alpha = (u8)mInactiveAlpha + 25;
            mInactiveAlpha = alpha;
            setEmitterAlpha(mpInactiveEmitter, alpha);
        }
        if (mpActiveEmitter) {
            u8 alpha = (u8)mActiveAlpha - 25;
            mActiveAlpha = alpha;
            setEmitterAlpha(mpActiveEmitter, alpha);
            if (mActiveAlpha == 0) {
                invalidateEmitter(mpActiveEmitter);
                mpActiveEmitter = 0;
            }
        }
    }
    switch ((u8)mEventState) {
    case EVENT_WAIT:
        mEventTimer = 0;
        if (spawned && statue) {
            mEventState = EVENT_ORDER;
            mEventTimer = 30;
        }
        break;
    case EVENT_ORDER:
        if (eventInfo.mCommand == dEvtCmd_INDEMO_e) {
            mEventState = EVENT_RUN;
            if (l_HIO.mbDebug)
                mEventTimer = l_HIO.mEventTime;
        } else {
            swflatOrderPotentialEvent(this, 1, 0, 0);
            eventInfo.onCondition(2);
        }
        break;
    case EVENT_RUN: {
        s16 timer = (s16)mEventTimer - 1;
        mEventTimer = timer;
        if (timer <= 1) {
            dComIfGp_event_reset();
            mEventState = EVENT_WAIT;
            onSwitch(this);
        }
        break;
    }
    }
    if (mLocked) {
        mPressed = 1;
        s16 lockTimer = mLockTimer;
        if (lockTimer != 0) {
            mLockTimer = lockTimer - 1;
            set_mtx();
            // HD: the output slot holds a four-byte guest pointer.
            *reinterpret_cast<gptr<Mtx34>*>(output) = &mtx;
            mWasPressed = mPressed;
            return TRUE;
        }
        if (mpActiveEmitter) {
            u8 alpha = (u8)mActiveAlpha - 25;
            mActiveAlpha = alpha;
            setEmitterAlpha(mpActiveEmitter, alpha);
            if (mActiveAlpha == 0) {
                invalidateEmitter(mpActiveEmitter);
                mpActiveEmitter = 0;
            }
        }
        if (mpInactiveEmitter) {
            u8 alpha = (u8)mInactiveAlpha - 25;
            mInactiveAlpha = alpha;
            setEmitterAlpha(mpInactiveEmitter, alpha);
            if (mInactiveAlpha == 0) {
                invalidateEmitter(mpInactiveEmitter);
                mpInactiveEmitter = 0;
            }
        }
        s16 end = mPressEndFrame, frame = mFrame;
        if ((s32)frame < (s32)end + 5)
            setFrame(this, (s16)(frame + 1));
    }
    set_mtx();
    *reinterpret_cast<gptr<Mtx34>*>(output) = &mtx;
    mWasPressed = mPressed;
    return TRUE;
}

/* 02392658 */
BOOL daObjSwflat::Act_c::Draw() {
    settingTevStruct(dKy_getEnvlight(), 1, &current.pos, &tevStr);
    auto env = dKy_getEnvlight();
    auto model = (J3DModel*)mpModel;
    setLightTevColorType(env, model, &tevStr);
    model = mpModel;
    SwflatBrk* brk = mpBrk;
    J3DModelData* data = model->mModelData;
    f32 frame = brk->mFrameCtrl.mFrame;
    swflatBrkEntry(brk, data, frame);
    dComIfGd_setListBG();
    model = mpModel;
    mDoExt_modelUpdateDL(model);
    dComIfGd_setList();
    return TRUE;
}

/* 023927F0 */
BOOL daObjSwflat::Act_c::IsDelete() {
    return TRUE;
}

/* 02392704 */
static s32 Mthd_Create(Act_c* a) {
    return a->Mthd_Create();
}

/* 02392708 */
static BOOL Mthd_Delete(Act_c* a) {
    return a->Mthd_Delete();
}

/* 0239270C */
static BOOL Mthd_Execute(Act_c* a) {
    return a->MoveBGExecute();
}

/* 02392710 */
static BOOL Mthd_Draw(Act_c* a) {
    return a->Draw_v();
}

/* 02392720 */
static BOOL Mthd_IsDelete(Act_c* a) {
    return a->IsDelete_v();
}

// HD: compiler-generated and template code of this translation unit.

/* 02392730 */
static void __sinit_d_a_obj_swflat_cpp() {
    sinit_header_statics(0x1046BF30, 0x101CCDB0);
    swflatHioConstruct(&l_HIO);
}

/* 023927D0 */
static void trivialDestructor(void* a, s32 flags) {
    if (a && (flags & 1))
        operator_delete(a);
}

/* 023927E4 */
static void emptyVirtual(void* a) {
}

/* 023927F8 */
static void destructor(Act_c* a, s32 flags) {
    if (a) {
        dCcD_Cyl_dt(&a->mCyl, 2);
        dCcD_Stts_dt(&a->mStts, 2);
        swflatActorDestruct(a, 0);
        if (flags & 1)
            operator_delete(a);
    }
}

/* 02392864 */
static u32 PrmAbstract(Act_c* a, s32 width, s32 shift) {
    u32 value = a->mParameters, sh = (u32)shift & 63, w = (u32)width & 63;
    u32 bits = sh < 32 ? value >> sh : 0;
    return bits & ((w < 32 ? 1u << w : 0) - 1);
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
data l_HIO 1046BF4C
data mtx 1046BF68
data l_cyl_src 101CCD6C
str 1002F928
str 1002F894
str 1002F8A8
str 1002F904 "Flat switch" renamed
ext 025E80D0 SwflatBrk *swflatBrkConstruct(SwflatBrk *)
ext 025E8154 BOOL swflatBrkInit(SwflatBrk *, void *, void *, s32, s32, f32, s32, s32, s32, s32)
ext 0200BD2C void swflatStatusBaseConstruct(dCcD_Stts *)
ext 02515DA0 void swflatGStatusConstruct(be<u32> *)
ext 02515FB8 void swflatGObjConstruct(dCcD_Cyl *)
ext 02018590 void swflatCylinderConstruct(cM3dGCyl *)

ext 025E19CC void swflatSeStart(u32, cXyz *)
ext 025D50BC void swflatActorDestruct(fopAc_ac_c *, s32)
ext 025E83FC void swflatBrkEntry(SwflatBrk *, J3DModelData *, f32)
ext 025F0A10 s32 swflatHioCreate(const char *, daObjSwflat_HIO_c *)
ext 025F0A18 void swflatHioDelete(s32)
ext 02391724 void *swflatHioConstruct(void *)
ext 025D6768 void swflatSetCullSphere(fopAc_ac_c*,f32,f32,f32,f32)
ext 025D7B24 void swflatOrderPotentialEvent(fopAc_ac_c*,u16,u16,u16)
*/
// clang-format on
