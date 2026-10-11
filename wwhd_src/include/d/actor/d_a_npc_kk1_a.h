// Mila creation and animation part. Private projections add no actor storage.
// HD: checked offsets for fields absent from the nonmatching GameCube header.
#pragma once
#include "d/actor/d_a_npc_kk1.h"
struct Kk1CreationFields {
    u8 _0[0x86C];
    /* 0x86C */ be<u32> mPartnerId;
    u8 _870[0x1E];
    /* 0x88E */ csXyz mModelAngle;
    u8 _894[0x44];
    /* 0x8D8 */ be<f32> mPreviousFrame;
    u8 _8DC[0xC];
    /* 0x8E8 */ be<u32> mActionResult;
    u8 _8EC[0x10];
    /* 0x8FC */ be<s16> mEventAction;
    u8 _8FE[0x14];
    /* 0x912 */ be<s16> mTalkLookTimer;
    /* 0x914 */ be<s16> mActionTimer;
    u8 _916[0xC];
    /* 0x922 */ be<u8> mAnimationEnded;
    /* 0x923 */ be<u8> mAnimationStarted;
    u8 _924[0x1];
    /* 0x925 */ be<u8> mSwitchNo;
    /* 0x926 */ be<u8> mOnPath;
    u8 _927[0x5];
    /* 0x92C */ be<u8> mResourceLoaded;
    u8 _92D[0x1];
    /* 0x92E */ be<u8> mEffectHidden;
    /* 0x92F */ be<u8> mDemoAnimation;
    u8 _930[0x3];
    /* 0x933 */ be<u8> mTalkRequested;
    u8 _934[0x2];
    /* 0x936 */ be<u8> mInDemo;
    u8 _937[0x175];
    /* 0xAAC */ csXyz mEffectFrame;
    u8 _AB2[0x2];
    /* 0xAB4 */ be<u32> mEffectModel;
    u8 _AB8[0x4];
    /* 0xABC */ be<u32> mSweatEmitter;
    u8 _AC0[0x1];
    /* 0xAC1 */ be<u8> mLookState;
    /* 0xAC2 */ be<u8> mCommentAction;
    /* 0xAC3 */ be<u8> mTalkAnimation;
    /* 0xAC4 */ be<u8> mMessageTag;
    /* 0xAC5 */ be<u8> mTextureAnimation;
    /* 0xAC6 */ be<u8> mAnimationNo;
    /* 0xAC7 */ be<u8> mEventState;
    /* 0xAC8 */ be<u8> mActionState;
    /* 0xAC9 */ be<u8> mPreviousState;
    u8 _ACA[0x2];
    /* 0xACC */ be<s8> mMessageType;
    /* 0xACD */ be<u8> mActionStep;
    /* 0xACE */ be<u8> mMessageAnimationStarted;
    u8 _ACF[0x1];
};
WWHD_SIZE(Kk1CreationFields, 0xAD0);
WWHD_OFFSET(Kk1CreationFields, mPartnerId, 0x86C);
WWHD_OFFSET(Kk1CreationFields, mModelAngle, 0x88E);
WWHD_OFFSET(Kk1CreationFields, mPreviousFrame, 0x8D8);
WWHD_OFFSET(Kk1CreationFields, mActionResult, 0x8E8);
WWHD_OFFSET(Kk1CreationFields, mEventAction, 0x8FC);
WWHD_OFFSET(Kk1CreationFields, mTalkLookTimer, 0x912);
WWHD_OFFSET(Kk1CreationFields, mActionTimer, 0x914);
WWHD_OFFSET(Kk1CreationFields, mAnimationEnded, 0x922);
WWHD_OFFSET(Kk1CreationFields, mAnimationStarted, 0x923);
WWHD_OFFSET(Kk1CreationFields, mSwitchNo, 0x925);
WWHD_OFFSET(Kk1CreationFields, mOnPath, 0x926);
WWHD_OFFSET(Kk1CreationFields, mResourceLoaded, 0x92C);
WWHD_OFFSET(Kk1CreationFields, mEffectHidden, 0x92E);
WWHD_OFFSET(Kk1CreationFields, mDemoAnimation, 0x92F);
WWHD_OFFSET(Kk1CreationFields, mTalkRequested, 0x933);
WWHD_OFFSET(Kk1CreationFields, mInDemo, 0x936);
WWHD_OFFSET(Kk1CreationFields, mEffectFrame, 0xAAC);
WWHD_OFFSET(Kk1CreationFields, mEffectModel, 0xAB4);
WWHD_OFFSET(Kk1CreationFields, mSweatEmitter, 0xABC);
WWHD_OFFSET(Kk1CreationFields, mLookState, 0xAC1);
WWHD_OFFSET(Kk1CreationFields, mCommentAction, 0xAC2);
WWHD_OFFSET(Kk1CreationFields, mTalkAnimation, 0xAC3);
WWHD_OFFSET(Kk1CreationFields, mMessageTag, 0xAC4);
WWHD_OFFSET(Kk1CreationFields, mTextureAnimation, 0xAC5);
WWHD_OFFSET(Kk1CreationFields, mAnimationNo, 0xAC6);
WWHD_OFFSET(Kk1CreationFields, mEventState, 0xAC7);
WWHD_OFFSET(Kk1CreationFields, mActionState, 0xAC8);
WWHD_OFFSET(Kk1CreationFields, mPreviousState, 0xAC9);
WWHD_OFFSET(Kk1CreationFields, mMessageType, 0xACC);
WWHD_OFFSET(Kk1CreationFields, mActionStep, 0xACD);
WWHD_OFFSET(Kk1CreationFields, mMessageAnimationStarted, 0xACE);
template<class T, class U> inline T* Kk1A_view(U* p) { return gabi::at<T>(gabi::ea(p)); }
inline Kk1CreationFields& Kk1A_fields(daNpc_Kk1_c* p) { return *Kk1A_view<Kk1CreationFields>(p); }
struct Kk1A_Btp { u8 _0[0x10]; gptr<J3DAnmTexPattern> mpAnm; };
struct Kk1A_Btk { u8 _0[0x68]; gptr<void> mpAnm; };
struct Kk1A_Bck {
    u8 mFrameCtrl[0x10]; be<u32> mAnmVtable; u8 mAnmTransform[0x34];
    be<u32> mBlendVtable; u8 _4C[0xC]; be<u32> mBlendFlags;
    u8 _5C[0x20]; be<u32> mFlags; be<u32> mFrameFlags, mAnimationFlags; gptr<void> mpAnm;
};
WWHD_SIZE(Kk1A_Bck, 0x8C);
WWHD_OFFSET(Kk1A_Bck, mBlendVtable, 0x48);
WWHD_OFFSET(Kk1A_Bck, mpAnm, 0x88);
struct Kk1A_Emitter { u8 _0[0x5C]; be<s32> mMaxFrame; u8 _60[0x1CC]; cXyz mGlobalTranslation; u8 _238[0x1C]; be<u32> mFlags; u8 _258[0xA]; be<u8> mCoordinateMode; };
WWHD_OFFSET(Kk1A_Emitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(Kk1A_Emitter, mCoordinateMode, 0x262);
struct Kk1A_JointTree { u8 _0[0x14]; be<u32> mpAnm; };
struct Kk1A_Ground { u8 _0[0x20]; be<u32> mPolyVtable; cXyz mPos; u8 _30[0x10]; be<u32> mPassVtable; u8 _44[8]; be<u32> mGroupVtable; };
WWHD_OFFSET(Kk1A_Ground, mPos, 0x24);
struct Kk1A_AcchVtables { u8 _0[0x14]; be<u32> mCheckVtable; u8 _18[8]; be<u32> mPolyVtable; };
struct Kk1A_MorfVirtual { be<u32> mVtable; };
struct Kk1A_MorfVtable { u8 _0[0xC]; be<u32> mDelete; };
inline void Kk1A_deleteMorf(mDoExt_McaMorf* p) { Kk1A_MorfVirtual* v=Kk1A_view<Kk1A_MorfVirtual>(p); auto* t=gabi::at<Kk1A_MorfVtable>(v->mVtable); gabi::call_ptr(t->mDelete,p,3); }
inline Kk1A_Emitter* Kk1A_emitter(u32 a) { return gabi::at<Kk1A_Emitter>(a); }
inline Kk1A_JointTree* Kk1A_jointTree(u32 a) { return gabi::at<Kk1A_JointTree>(a); }
inline fopAc_ac_c* Kk1A_actor(u32 a) { return gabi::at<fopAc_ac_c>(a); }
inline be<s16>* Kk1A_eventIds(daNpc_Kk1_c* p) { return &p->m8EC; }
inline Kk1A_Bck* Kk1A_bck(daNpc_Kk1_c* p) { return Kk1A_view<Kk1A_Bck>(&p->mBckAnm); }
inline be<u32>* Kk1A_checkList(u32 a) { return gabi::at<be<u32>>(a); }

void Kk1A_pathSetInf(dNpc_PathRun_l* p, u8 path, s8 room, u8 fwd);
void Kk1A_pathGetPoint(dNpc_PathRun_l* p, cXyz* out, u8 idx);
BOOL Kk1A_pathNextIdx(dNpc_PathRun_l* p);
void Kk1A_soundStart(u32 id, cXyz* pos);
s32 Kk1A_brkInit(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry);
s32 Kk1A_bckInit(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify);
void Kk1A_bckEntry(void* anm, J3DModelData* d, f32 frame);
BOOL Kk1A_setAnmIDRes(mDoExt_McaMorf* m, s32 loop, f32 morf, f32 speed, s32 bck, s32 bas, const char* arc);
void* Kk1A_demoGetActor(u32 obj, u8 id);
J3DAnmTexPattern* Kk1A_demoBtp(void* ac, const char* arc);
BOOL Kk1A_demoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* m, const char* arc, s32 n, u32 p, u32 q, s8 r);
void Kk1A_zxyRot(Mtx34* m, s16 x, s16 y, s16 z);
BOOL Kk1A_play(mDoExt_McaMorf* m, cXyz* pos, u32 snd, s32 reverb);
void Kk1A_groundDestroy(dBgS_GndChk* p, s32 flags);
void Kk1A_npcCtor(daNpc_Kk1_c* p);
void Kk1A_btpCtor(void* p);
void Kk1A_brkCtor(void* p);
void Kk1A_btkCtor(void* p);
void Kk1A_frameInit(void* p, s32 n);
void Kk1A_transformCtor(void* p);
void* Kk1A_processSearch(u32 fn, void* p);
BOOL Kk1A_searchById(fpc_ProcID id, be<u32>* out);
void Kk1A_anmDestroy(void* p, s32 flags);
void Kk1A_circleDestroy(void* p, s32 flags);
void Kk1A_acchDestroy(dBgS_Acch* p, s32 flags);
void Kk1A_actorDestroy(daNpc_Kk1_c* p, s32 flags);

inline be<u8>* Kk1A_arcBytes(daNpc_Kk1_c* p) { return gabi::at<be<u8>>(gabi::ea(p->mArcName)); }

WWHD_OFFSET(Kk1A_Btp, mpAnm, 0x10);
WWHD_OFFSET(Kk1A_Btk, mpAnm, 0x68);
WWHD_OFFSET(Kk1A_Bck, mAnmVtable, 0x10);
WWHD_OFFSET(Kk1A_Bck, mBlendFlags, 0x58);
WWHD_OFFSET(Kk1A_Bck, mFlags, 0x7C);
WWHD_OFFSET(Kk1A_Bck, mFrameFlags, 0x80);
WWHD_OFFSET(Kk1A_Bck, mAnimationFlags, 0x84);
WWHD_OFFSET(Kk1A_Emitter, mMaxFrame, 0x5C);
WWHD_OFFSET(Kk1A_Emitter, mFlags, 0x254);
WWHD_OFFSET(Kk1A_JointTree, mpAnm, 0x14);
WWHD_OFFSET(Kk1A_Ground, mPolyVtable, 0x20);
WWHD_OFFSET(Kk1A_Ground, mPassVtable, 0x40);
WWHD_OFFSET(Kk1A_Ground, mGroupVtable, 0x4C);
WWHD_OFFSET(Kk1A_AcchVtables, mCheckVtable, 0x14);
WWHD_OFFSET(Kk1A_AcchVtables, mPolyVtable, 0x20);
inline J3DModel* Kk1A_actorEffectModel(daNpc_Kk1_c* p) { return gabi::at<J3DModel>(Kk1A_fields(p).mEffectModel); }
inline const char* Kk1A_string(u32 a) { return gabi::at<const char>(a); }
inline J3DModelData* Kk1A_modelData(u32 a) { return gabi::at<J3DModelData>(a); }
enum Kk1A_ActionState { KK1A_ACTION_WAIT = 1, KK1A_ACTION_TALK = 2, KK1A_ACTION_WALK = 3, KK1A_ACTION_WAIT_PARTNER = 4, KK1A_ACTION_COMMENT = 5, KK1A_ACTION_WAIT_NEAR = 6, KK1A_ACTION_WAIT_FAR = 7 };
