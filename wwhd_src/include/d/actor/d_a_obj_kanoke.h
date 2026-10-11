/* Earth Temple coffin. Tww provides method names/order but leaves its fields opaque.
 * HD: descriptive fields below retain the verified layout, including four capsules and smoke data. */
#pragma once
#include "f_op/f_op_actor.h"
#include "JSystem/J3DGraph.h"
#include "d/d_cc_d.h"
#include "d/d_particle.h"
struct KanokeShapeAttr {
    u8 bounds[0x18];
    be<u32> boundsVtable, __vtbl;
};
struct KanokeCapsuleGeometry {
    cXyz mStart, mEnd;
    be<u32> __vtbl;
    be<f32> mRadius;
};
struct KanokeCapsule : dCcD_GObjInf {
    KanokeShapeAttr mShape;
    KanokeCapsuleGeometry mCps;
};
WWHD_SIZE(KanokeCapsule, 0x138);
struct daObjKanoke_c : fopAc_ac_c {
    BOOL createHeap();
    u8 getPrmType();
    u8 getPrmSearch();
    u8 getPrmSwNo();
    u8 getPrmSwNo2();
    void setMtxHontai();
    void setMtxHuta(cXyz* position);
    void setMtx();
    s32 createInit();
    s32 _create();
    BOOL _delete();
    BOOL _execute();
    BOOL _draw();
    u8 getPrmYure();
    void executeNormal();
    void executeYureYoko();
    void executeOpenYoko();
    void executeEffectYoko();
    void executeYureTate();
    void executeOpenTate();
    void executeEffectTate();
    void executeWait();
    u32 getPrm(u32 width, u32 shift);

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mBodyModel, mLidModel;
    /* 0x3BC */ gptr<dBgW> mpBodyBgW, mpLidBgW;
    /* 0x3C4 */ Mtx34 mBodyMtx, mLidMtx;
    /* 0x424 */ dCcD_Stts mStts;
    /* 0x460 */ KanokeCapsule mBodyCps;
    /* 0x598 */ KanokeCapsule mLidCps[3];
    /* 0x940 */ be<u32> mEffectHandles[2];
    /* 0x948 */ dPa_smokeEcallBack mSmoke;
    /* 0x968 */ cXyz mSmokePos;
    /* 0x974 */ csXyz mSmokeAngle;
    u8 padding97A[2];
    /* 0x97C */ cXyz mLidOffset, mPivot;
    /* 0x994 */ be<f32> mSmokeAlpha;
    /* 0x998 */ csXyz mLidAngle;
    /* 0x99E */ be<s16> mAngularSpeed, mTimer, mShakeSpeed, mLightTimer;
    /* 0x9A6 */ be<u8> mType, mState, mSearchRange, mSwNo, mSwNo2, mHidden;
};
WWHD_OFFSET(daObjKanoke_c, mBodyModel, 0x3B4);
WWHD_OFFSET(daObjKanoke_c, mBodyMtx, 0x3C4);
WWHD_OFFSET(daObjKanoke_c, mStts, 0x424);
WWHD_OFFSET(daObjKanoke_c, mBodyCps, 0x460);
WWHD_OFFSET(daObjKanoke_c, mLidCps, 0x598);
WWHD_OFFSET(daObjKanoke_c, mEffectHandles, 0x940);
WWHD_OFFSET(daObjKanoke_c, mSmoke, 0x948);
WWHD_OFFSET(daObjKanoke_c, mLidOffset, 0x97C);
WWHD_OFFSET(daObjKanoke_c, mLidAngle, 0x998);
WWHD_OFFSET(daObjKanoke_c, mType, 0x9A6);
WWHD_SIZE(daObjKanoke_c, 0x9AC);

void kanokeActorConstruct(fopAc_ac_c*);
void kanokeStatusConstruct(dCcD_Stts*);
void kanokeGlobalStatusConstruct(void*);
void kanokeHitInfoConstruct(void*);
void kanokeCapsuleGeometryConstruct(void*);
void kanokeConstructArray(void*, s32, s32, u32);
void kanokeSmokeConstruct(dPa_smokeEcallBack*, u8);
void kanokeSetCapsule(void*, const void*);
void kanokeSetCapsuleEnds(void*, const cXyz*, const cXyz*);
s32 kanokeCheckLight(void*, const cXyz*);
f64 kanokeSquareMagnitudeFPR(const cXyz*);
f64 kanokeSqrtFPR(f64);
void kanokeRegisterGlobal(void*);
void kanokeConstructSinTable(void*);
void kanokeConstructAtanTable(void*);
void kanokeDestructArray(void*, s32, s32, u32, s32, s32);
void kanokeCapsuleDestruct(void*, s32);
void kanokeStatusDestruct(dCcD_Stts*, s32);
void kanokeActorDestruct(fopAc_ac_c*, s32);
void* kanokeGetResource(void*, const void*, s32);
// HD: private declarations above retain compiler support and full FPR entry signatures.

// HD: packed GHS member pointers here are eight bytes, with overlapping virtual-table displacement.
struct KanokeAction {
    be<s16> mThisDelta, mSelector;
    be<u32> mFunction;
    void invoke(daObjKanoke_c* actor) const {
        u32 adjusted = gabi::ea(actor) + s32(mThisDelta);
        s32 selector = mSelector;
        u32 fn;
        if (selector < 0)
            fn = mFunction;
        else {
            u32 vtable = gabi::load<u32>(adjusted + s32(s16(u32(mFunction))));
            fn = gabi::load<u32>(vtable + selector * 8 + 4);
        }
        gabi::call_ptr(fn, gabi::at<daObjKanoke_c>(adjusted));
    }
};
WWHD_SIZE(KanokeAction, 8);
struct KanokeActions {
    KanokeAction entries[8];
};
struct KanokeMathStatics {
    be<f32> mMinAngle, mMaxAngle;
    u8 mSinTable, mAtanTable, padding[2];
    be<u32> mZero[4];
};
WWHD_SIZE(KanokeMathStatics, 0x1C);
struct KanokeGlobalDescriptors {
    u8 entries[3][12];
};
struct KanokeLidPoints {
    cXyz points[6];
};
struct KanokeCapsuleSource {
    u8 data[0x4C];
};
// HD: emitter tail omitted from the shared minimal JPA layout.
struct KanokeEmitter {
    u8 padding[0x247];
    be<u8> mGlobalAlpha;
    u8 padding248[0xC];
    be<u32> mFlags;
};
WWHD_OFFSET(KanokeEmitter, mFlags, 0x254);
WWHD_OFFSET(KanokeEmitter, mGlobalAlpha, 0x247);
enum KanokeState {
    STATE_NORMAL,
    STATE_YURE_YOKO,
    STATE_OPEN_YOKO,
    STATE_EFFECT_YOKO,
    STATE_YURE_TATE,
    STATE_OPEN_TATE,
    STATE_EFFECT_TATE,
    STATE_WAIT
};
enum KanokeType { TYPE_YOKO, TYPE_TATE };

enum KanokeResource {
    RES_BODY_MODEL = 4,
    RES_LID_MODEL = 5,
    RES_BODY_COLLISION = 8,
    RES_LID_COLLISION = 9
};
enum KanokeVtable : u32 {
    VT_ARCHIVE = 0x1002BC44,
    VT_ACTOR = 0x1002BC6C,
    VT_STATUS = 0x1004AE88,
    VT_GLOBAL_STATUS = 0x1004AEC0,
    VT_SHAPE = 0x100015A8,
    VT_BOUNDS = 0x1002BC5C,
    VT_CAPSULE_HIT = 0x1004AF18,
    VT_CAPSULE_SHAPE = 0x1004AF70,
    VT_CAPSULE = 0x1004AF60,
};
// HD: the array destructor takes an external guest callback token, not a host function pointer.
constexpr u32 KANOKE_CAPSULE_DESTRUCTOR = 0x02515980;
