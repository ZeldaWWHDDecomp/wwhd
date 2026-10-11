#pragma once
#include "gc_api/gc_api.h"

enum {
    dRes_INDEX_KANTERA_BMD_GA_e = 9,
    dRes_INDEX_KANTERA_BMD_LF_e = 0xC,
    dRes_INDEX_KANTERA_BMD_MK_KANTERA_e = 0xD,
    dRes_INDEX_KANTERA_BRK_MK_KANTERA_e = 0x11,
};
enum { MK_KANTERA_JNT_TOTTE_e = 0, MK_KANTERA_JNT_KANTERA_e = 1 };



// HD: the constructor returns the allocation selected by the callee.
inline mDoExt_brkAnm* kanteraBrkConstruct(void* p) {
    return gabi::call<mDoExt_brkAnm*>(0x025E80D0, p);
}
inline BOOL kanteraBrkInit(mDoExt_brkAnm* a, J3DModelData* d, J3DAnmTevRegKey* k, bool play,
                           s32 mode, f32 rate, s16 start, s16 end, bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, k, play, mode, rate, start, end, modify, entry);
}
/* J3DModel::getAnmMtx / setAnmMtx (HD): joint matrix block at +0x2C (flags +4 |= 0x10, matrices at
 * +0x10) */
// HD: this inline loads the matrix pointer before marking the buffer dirty.
static inline Mtx34* kanteraAnmMtx(J3DModel* model, s32 jnt) {
    J3DMtxBuffer_hd* buffer = model->mpMtxBuffer;
    u16 flags = buffer->mFlags;
    u32 matrices = buffer->mpAnmMtx;
    buffer->mFlags = flags | 0x10;
    return gabi::at<Mtx34>(matrices + jnt * 0x30);
}

struct KanteraJoint {
    be<u32> mpData;
    be<u16> mJntNo;
};
WWHD_OFFSET(KanteraJoint, mJntNo, 4);

struct mo_ga_s {
    /* 0x00 */ gptr<J3DModel> mpModel;
    /* 0x04 */ cXyz mPos;
    /* 0x10 */ cXyz m10;
    /* 0x1C */ be<s16> mRotX;
    /* 0x1E */ be<s16> mRotY;
    /* 0x20 */ u8 m20[0x24 - 0x20];
    /* 0x24 */ be<f32> mScale;
    /* 0x28 */ be<f32> mScaleY;
    /* 0x2C */ be<s16> m2C;
    /* 0x2E */ be<u8> m2E;
    /* 0x2F */ be<u8> m2F;
};
WWHD_SIZE(mo_ga_s, 0x30);

/* GameCube layout +0x11C up to mPlight; LIGHT_INFLUENCE is 4 bytes larger in HD, so +0x120 after it
 */
struct kantera_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ be<u8> mParam0;
    /* 0x3B5 */ be<u8> mParam1;
    /* 0x3B6 */ be<u8> mParam2;
    /* 0x3B7 */ be<u8> mSwitchNo;
    /* 0x3B8 */ gptr<J3DModel> mpModel1;
    /* 0x3BC */ gptr<J3DModel> mpModel2;
    /* 0x3C0 */ be<u32> mTargetActorID;
    /* 0x3C4 */ gptr<mDoExt_brkAnm> mpBrkAnm1;
    /* 0x3C8 */ gptr<mDoExt_brkAnm> mpBrkAnm2; /* HD: not created */
    /* 0x3CC */ be<s32> m2B0;
    /* 0x3D0 */ be<s32> m2B4;
    /* 0x3D4 */ csXyz mJointRot[2];
    /* 0x3E0 */ csXyz mJointBaseRot;
    /* 0x3E6 */ csXyz m2CA;
    /* 0x3EC */ csXyz mJointBaseRotTarget;
    /* 0x3F2 */ u8 m2D6[2];
    /* 0x3F4 */ cXyz mBonPos;
    /* 0x400 */ cXyz m2E4;
    /* 0x40C */ LIGHT_INFLUENCE mPlight;
    /* 0x430 */ Mtx34 mAlphaModelMtx;
    /* 0x460 */ be<f32> mBonAlpha;
    /* 0x464 */ be<f32> mBonAlphaTarget;
    /* 0x468 */ be<f32> mBonScale;
    /* 0x46C */ be<f32> mBonScaleTarget;
    /* 0x470 */ be<s16> mBonRot;
    /* 0x472 */ be<s16> mAnimCounters[2];
    /* 0x476 */ u8 m356[2];
    /* 0x478 */ be<s32> mAnimCounter;
    /* 0x47C */ be<s16> m35C;
    /* 0x47E */ u8 m35E[2];
    /* 0x480 */ be<f32> mModel2Scale;
    /* 0x484 */ be<s8> mState;
    /* 0x485 */ u8 m365[3];
    /* 0x488 */ be<f32> mOffsY;
    /* 0x48C */ be<u32> mPeekZResult;
    /* 0x490 */ dBgS_AcchCir mAcchCir;
    /* 0x4D0 */ dBgS_ObjAcch mAcch;
    /* 0x694 */ dCcD_Stts mStts;
    /* 0x6D0 */ dCcD_Sph mSph;
    /* 0x7FC */ dPa_followEcallBack mPtclCallBack0;
    /* 0x810 */ dPa_followEcallBack mPtclCallBack1;
    /* 0x824 */ cXyz mParticleScale;
    /* 0x830 */ mo_ga_s mGa[2];
};
WWHD_OFFSET(kantera_class, mPlight, 0x40C);
WWHD_OFFSET(kantera_class, mAlphaModelMtx, 0x430);
WWHD_OFFSET(kantera_class, mState, 0x484);
WWHD_OFFSET(kantera_class, mAcch, 0x4D0);
WWHD_OFFSET(kantera_class, mSph, 0x6D0);
WWHD_OFFSET(kantera_class, mGa, 0x830);
WWHD_SIZE(kantera_class, 0x890);

// HD: only the holder fields used by the lantern are mapped here.
struct KanteraHolder : fopAc_ac_c {
    u8 mBeforeHand[0x2CF4 - 0x3AC];
    cXyz mHandPos;
    u8 mBeforeLanternState[0x2E0D - 0x2D00];
    be<s8> mLanternState;
};
WWHD_OFFSET(KanteraHolder, mHandPos, 0x2CF4);
WWHD_OFFSET(KanteraHolder, mLanternState, 0x2E0D);

// HD: this emitter view exposes the scale at +8 used by the lantern.
struct KanteraEmitter {
    u8 mBeforeScale[8];
    cXyz mGlobalScale;
};
WWHD_OFFSET(KanteraEmitter, mGlobalScale, 8);

// HD: this translation unit places the angle constants before the local-static guard.
struct KanteraStatics {
    be<f32> mNegativePi;
    be<f32> mPi;
    be<u32> mParticleScaleGuard;
    u8 mMathInit;
    u8 mRandomInit;
    u8 mPadding[2];
    be<u32> mZero[4];
    cXyz mParticleScale;
};
WWHD_OFFSET(KanteraStatics, mParticleScaleGuard, 8);
WWHD_OFFSET(KanteraStatics, mZero, 0x10);
WWHD_OFFSET(KanteraStatics, mParticleScale, 0x20);
WWHD_SIZE(KanteraStatics, 0x2C);
inline KanteraStatics* kanteraStatics() { return gabi::at<KanteraStatics>(0x10464AC4); }

inline void kanteraSoundSet(cXyz* pos, s32 p, u32 pid, s32 t) {
    gabi::call(0x0255F458, pos, p, pid, t);
}
inline void kanteraSphereConstruct(dCcD_Sph* sphere) { gabi::call(0x025166F0, sphere); }
inline void kanteraSphereDestroy(dCcD_Sph* sphere) { gabi::call(0x02515AE8, sphere, 2); }
inline void kanteraGroundDestroy(dBgS_GndChk* ground) { gabi::call(0x02008DAC, ground, 0); }
inline void kanteraAcchDestroy(dBgS_ObjAcch* acch) { gabi::call(0x024EFD9C, acch, 0); }
inline void kanteraCircleDestroy(cM3dGCir_l* circle) { gabi::call(0x02018034, circle, 2); }
inline void kanteraActorDestroy(kantera_class* actor) { gabi::call(0x025D50BC, actor, 0); }
// HD: Create calls the engine entry rather than inlining Execute.
inline void kanteraExecuteCall(kantera_class* actor) { gabi::call(0x0218D3F8, actor); }
inline void kanteraMathInit(u8* state) { gabi::call(0x028ED6F8, state); }
inline void kanteraRandomInit(u8* state) { gabi::call(0x028EAB2C, state); }

struct KanteraSphereSource {
    be<u32> mFlags;
    be<u32> mAtType;
    u8 mAtp;
    u8 mAtPadding0;
    u8 mAtPadding1;
    u8 mAtPadding2;
    be<u32> mAtSPrm;
    be<u32> mTgType;
    be<u32> mTgSPrm;
    be<u32> mCoSPrm;
    u8 mGAtSe;
    u8 mGAtHitMark;
    u8 mGAtSpl;
    u8 mGAtMtrl;
    be<u32> mGAtSPrm;
    u8 mGTgSe;
    u8 mGTgHitMark;
    u8 mGTgSpl;
    u8 mGTgMtrl;
    be<u32> mGTgSPrm;
    be<u32> mGCoSPrm;
    be<f32> mCenterX;
    be<f32> mCenterY;
    be<f32> mCenterZ;
    be<f32> mRadius;
};
WWHD_SIZE(KanteraSphereSource, 0x40);
