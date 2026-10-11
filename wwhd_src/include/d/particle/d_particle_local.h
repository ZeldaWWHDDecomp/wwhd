#pragma once
#include "gabi.h"
#include "wwhd.h"
#include <cmath>
void pa_registerGlobalObject(void* descriptor);
void pa_dKy_get_seacolor(void* sea, void* foam);
void pa_JPAGetXYZRotateMtx(s32 x, s32 y, s32 z, void* matrix);
f64 pa_sqrtHD(f32 value);
void pa_constructArray(void* array, u32 count, u32 stride, void* constructor);
void pa_PSMTXIdentity(void* matrix);
void pa_PSMTXConcat(void* left, void* right, void* out);
void pa_PSMTXMultVec(void* matrix, void* vector, void* out);
s32 pa_PSMTXInverse(void* matrix, void* out);
void pa_mDoExt_destroySolidHeap(void* heap);
void pa_mDoExt_adjustSolidHeap(void* heap);
void pa_mDoExt_restoreCurrentHeap();
s32 pa_fopAcM_getWaterY(void* position, void* height);
s32 pa_cLib_chaseS(void* value, s32 target, s32 step);
void pa_operatorDelete(void* object);
void* pa_operatorNew(u32 size);
void* pa_operatorNewHeap(u32 size, void* heap, u32 alignment);
void pa_heapFree(void* object);
void pa_setEmitterDrawFlag(void* emitter, u32 flag);
void pa_calcEmitterGroup(void* manager, u32 group);
void pa_clearResourceManager(void* manager, u32 resource);
void pa_forceDeleteAllEmitter(void* manager);
void pa_bindVertexBuffer(void* buffer);
void pa_flushVertexBuffer(void* buffer, u32 offset, u32 size);
void pa_drawIndexed(void* indices, u32 count, void* vertices, u32 vertexCount, u32 first,
                    u32 instances);
void pa_setMaterialColor(void* material, void* color, u32 slot);
void pa_setMaterialKonstColor(void* material, void* color, u32 slot);
void pa_setMaterialAmbientColor(void* material, void* color, u32 slot);
void pa_loadMaterial(void* material);
void pa_loadShape(void* shape, void* resource);
void* pa_getEnvlight();
void* pa_getGameInfo();
void pa_dKyw_get_AllWind_vec(void* position, void* wind, void* power);
void pa_PSVECScale(void* source, void* out, f32 scale);
s32 pa_daSea_ChkArea(f32 x, f32 z);
f32 pa_daSea_calcWave(f32 x, f32 z);

// Unit-local checked views preserve HD guest layouts without extending shared APIs.
namespace pa {
struct ParticleMatrix {
    be<f32> m[3][4];
};
WWHD_SIZE(ParticleMatrix, 48);
WWHD_OFFSET(ParticleMatrix, m, 0);
inline void* p(u32 a) { return gabi::at<void>(a); }
inline u32 w(u32 a) { return gabi::load<u32>(a); }
inline u8 b(u32 a) { return gabi::load<u8>(a); }
inline u16 h(u32 a) { return gabi::load<u16>(a); }
inline s16 sh(u32 a) { return gabi::load<s16>(a); }
inline f32 f(u32 a) { return gabi::load<f32>(a); }
inline void W(u32 a, u32 v) { gabi::store<u32>(a, v); }
inline void B(u32 a, u8 v) { gabi::store<u8>(a, v); }
inline void H(u32 a, u16 v) { gabi::store<u16>(a, v); }
inline void F(u32 a, f32 v) { gabi::store<f32>(a, v); }
inline void copy(u32 s, u32 d, u32 n) {
    u8 bytes[64];
    for (u32 i = 0; i < n; i++)
        bytes[i] = b(s + i);
    for (u32 i = 0; i < n; i++)
        B(d + i, bytes[i]);
}
struct Vec {
    be<f32> x;
    be<f32> y;
    be<f32> z;
};
WWHD_SIZE(Vec, 12);
WWHD_OFFSET(Vec, x, 0);
WWHD_OFFSET(Vec, y, 4);
WWHD_OFFSET(Vec, z, 8);
struct Color {
    u8 bytes[4];
};
struct Color16 {
    u8 bytes[8];
};
struct ColorF {
    u8 bytes[16];
};
inline void* allocate(void* obj, u32 size) { return obj ? obj : pa_operatorNew(size); }
inline void destroy(void* obj, u32 flag) {
    if (obj && (flag & 1))
        pa_operatorDelete(obj);
}
} // namespace pa

// GameCube callback names; HD keeps the callback vtable at zero.
namespace pa {
struct dPa_followEcallBack {
    void execute(void* emitter);
    void setup(void* emitter, void* pos, void* angle, s8 info);
    void end();
    be<u32> __vtbl;
    be<u32> mpEmitter;
    be<u32> mPos;
    be<u32> mAngle;
    be<u8> mFlag;
    be<u8> mRateOff;
    be<u8> field_0x12;
    be<u8> field_0x13;
};
WWHD_SIZE(dPa_followEcallBack, 0x14);
WWHD_OFFSET(dPa_followEcallBack, mpEmitter, 4);
WWHD_OFFSET(dPa_followEcallBack, mPos, 8);
WWHD_OFFSET(dPa_followEcallBack, mAngle, 0xC);
WWHD_OFFSET(dPa_followEcallBack, mFlag, 0x10);
WWHD_OFFSET(dPa_followEcallBack, mRateOff, 0x11);
WWHD_OFFSET(dPa_followEcallBack, field_0x12, 0x12);
WWHD_OFFSET(dPa_followEcallBack, field_0x13, 0x13);
struct dPa_smokeEcallBack : dPa_followEcallBack {
    void execute(void* emitter);
    void end();
    void setup(void* emitter, void* pos, void* angle, u32 info);
    be<s8> field_0x14;
    be<u8> mWindOff;
    u8 mColor[4];
    u8 padding[2];
    be<u32> mTevstr;
};
WWHD_SIZE(dPa_smokeEcallBack, 0x20);
WWHD_OFFSET(dPa_smokeEcallBack, field_0x14, 0x14);
WWHD_OFFSET(dPa_smokeEcallBack, mWindOff, 0x15);
WWHD_OFFSET(dPa_smokeEcallBack, mColor, 0x16);
WWHD_OFFSET(dPa_smokeEcallBack, mTevstr, 0x1C);
} // namespace pa

namespace pa {
// HD: expanded draw and callback state shifts the emitter tail after the dynamics fields.
struct EmitterVector {
    be<f32> x, y, z;
};
WWHD_SIZE(EmitterVector, 12);
struct EmitterColor {
    be<u8> r, g, b, a;
};
WWHD_SIZE(EmitterColor, 4);
struct EmitterParticleList {
    be<u32> mHead, mTail, mLength;
};
WWHD_SIZE(EmitterParticleList, 12);
struct JPABaseEmitter {
    u8 padding0[52];
    /* 0x034 */ be<f32> mRate;
    u8 padding38[32];
    /* 0x058 */ be<f32> mSpread;
    /* 0x05C */ be<s32> mMaxFrame;
    u8 padding60[16];
    /* 0x070 */ be<f32> mInitialVelDir;
    u8 padding74[312];
    /* 0x1AC */ EmitterParticleList mActiveParticles;
    /* 0x1B8 */ EmitterParticleList mChildParticles;
    u8 padding1C4[32];
    /* 0x1E4 */ be<u32> mpEmitterCallBack;
    /* 0x1E8 */ be<u32> mpParticleCallBack;
    u8 padding1EC[52];
    /* 0x220 */ EmitterVector mGlobalDynamicsScale;
    /* 0x22C */ EmitterVector mGlobalTranslation;
    /* 0x238 */ EmitterVector mGlobalParticleScale;
    /* 0x244 */ EmitterColor mGlobalPrmColor;
    /* 0x248 */ EmitterColor mGlobalEnvColor;
    u8 padding24C[8];
    /* 0x254 */ be<u32> mFlags;
    /* 0x258 */ be<u32> mUserData;
    u8 padding25C[6];
    /* 0x262 */ be<u8> mGroupID;
    u8 tail;
};
WWHD_SIZE(JPABaseEmitter, 0x264);
WWHD_OFFSET(JPABaseEmitter, mRate, 0x34);
WWHD_OFFSET(JPABaseEmitter, mSpread, 0x58);
WWHD_OFFSET(JPABaseEmitter, mMaxFrame, 0x5C);
WWHD_OFFSET(JPABaseEmitter, mInitialVelDir, 0x70);
WWHD_OFFSET(JPABaseEmitter, mActiveParticles.mHead, 0x1AC);
WWHD_OFFSET(JPABaseEmitter, mActiveParticles.mTail, 0x1B0);
WWHD_OFFSET(JPABaseEmitter, mActiveParticles.mLength, 0x1B4);
WWHD_OFFSET(JPABaseEmitter, mChildParticles.mHead, 0x1B8);
WWHD_OFFSET(JPABaseEmitter, mChildParticles.mTail, 0x1BC);
WWHD_OFFSET(JPABaseEmitter, mChildParticles.mLength, 0x1C0);
WWHD_OFFSET(JPABaseEmitter, mpEmitterCallBack, 0x1E4);
WWHD_OFFSET(JPABaseEmitter, mpParticleCallBack, 0x1E8);
WWHD_OFFSET(JPABaseEmitter, mGlobalDynamicsScale.x, 0x220);
WWHD_OFFSET(JPABaseEmitter, mGlobalDynamicsScale.y, 0x224);
WWHD_OFFSET(JPABaseEmitter, mGlobalDynamicsScale.z, 0x228);
WWHD_OFFSET(JPABaseEmitter, mGlobalTranslation.x, 0x22C);
WWHD_OFFSET(JPABaseEmitter, mGlobalTranslation.y, 0x230);
WWHD_OFFSET(JPABaseEmitter, mGlobalTranslation.z, 0x234);
WWHD_OFFSET(JPABaseEmitter, mGlobalParticleScale.x, 0x238);
WWHD_OFFSET(JPABaseEmitter, mGlobalParticleScale.y, 0x23C);
WWHD_OFFSET(JPABaseEmitter, mGlobalParticleScale.z, 0x240);
WWHD_OFFSET(JPABaseEmitter, mGlobalPrmColor.r, 0x244);
WWHD_OFFSET(JPABaseEmitter, mGlobalPrmColor.g, 0x245);
WWHD_OFFSET(JPABaseEmitter, mGlobalPrmColor.b, 0x246);
WWHD_OFFSET(JPABaseEmitter, mGlobalPrmColor.a, 0x247);
WWHD_OFFSET(JPABaseEmitter, mGlobalEnvColor.r, 0x248);
WWHD_OFFSET(JPABaseEmitter, mGlobalEnvColor.g, 0x249);
WWHD_OFFSET(JPABaseEmitter, mGlobalEnvColor.b, 0x24A);
WWHD_OFFSET(JPABaseEmitter, mGlobalEnvColor.a, 0x24B);
WWHD_OFFSET(JPABaseEmitter, mFlags, 0x254);
WWHD_OFFSET(JPABaseEmitter, mUserData, 0x258);
WWHD_OFFSET(JPABaseEmitter, mGroupID, 0x262);
} // namespace pa

namespace pa {
struct dPa_simpleData_c {
    EmitterVector mPos;
    EmitterColor mPrmColor;
    be<u8> mEnvColor[3];
    be<u8> mbAffectedByWind;
};
WWHD_SIZE(dPa_simpleData_c, 20);
struct dPa_rippleEcallBack {
    void execute(void* emitter);
    void draw(void* emitter);
    void setup(void* emitter, void* pos, void* angle, u32 info);
    void end();
    /* 0x0 */ be<u32> __vtbl;
    /* 0x4 */ be<u32> mpBaseEmitter;
    /* 0x8 */ be<u32> mPos;
    /* 0xC */ be<u32> mFlags;
    /* 0x10 */ be<f32> mRate;
};
WWHD_SIZE(dPa_rippleEcallBack, 0x14);
WWHD_OFFSET(dPa_rippleEcallBack, __vtbl, 0x0);
WWHD_OFFSET(dPa_rippleEcallBack, mpBaseEmitter, 0x4);
WWHD_OFFSET(dPa_rippleEcallBack, mPos, 0x8);
WWHD_OFFSET(dPa_rippleEcallBack, mFlags, 0xC);
WWHD_OFFSET(dPa_rippleEcallBack, mRate, 0x10);
struct dPa_waveEcallBack {
    void remove();
    void setup(void* emitter, void* pos, void* angle, u32 info);
    void drawBefore(void* emitter);
    void executeAfter(void* emitter);
    void draw(void* emitter);
    /* 0x0 */ be<u32> __vtbl;
    /* 0x4 */ be<s16> mState;
    /* 0x6 */ be<s16> mFadeTimer;
    /* 0x8 */ be<f32> mVelFade1;
    /* 0xC */ be<f32> mVel;
    /* 0x10 */ be<f32> mVelSpeed;
    /* 0x14 */ be<f32> mVelFade2;
    /* 0x18 */ be<f32> mMaxParticleVelocity;
    /* 0x1C */ EmitterVector mCollapsePos[2];
    /* 0x34 */ be<u32> mpPos;
    /* 0x38 */ be<u32> mpRot;
    /* 0x3C */ EmitterVector mRotMtx[3];
    /* 0x60 */ be<u32> mpBaseEmitter;
};
WWHD_SIZE(dPa_waveEcallBack, 0x64);
WWHD_OFFSET(dPa_waveEcallBack, __vtbl, 0x0);
WWHD_OFFSET(dPa_waveEcallBack, mState, 0x4);
WWHD_OFFSET(dPa_waveEcallBack, mFadeTimer, 0x6);
WWHD_OFFSET(dPa_waveEcallBack, mVelFade1, 0x8);
WWHD_OFFSET(dPa_waveEcallBack, mVel, 0xC);
WWHD_OFFSET(dPa_waveEcallBack, mVelSpeed, 0x10);
WWHD_OFFSET(dPa_waveEcallBack, mVelFade2, 0x14);
WWHD_OFFSET(dPa_waveEcallBack, mMaxParticleVelocity, 0x18);
WWHD_OFFSET(dPa_waveEcallBack, mCollapsePos, 0x1C);
WWHD_OFFSET(dPa_waveEcallBack, mpPos, 0x34);
WWHD_OFFSET(dPa_waveEcallBack, mpRot, 0x38);
WWHD_OFFSET(dPa_waveEcallBack, mRotMtx, 0x3C);
WWHD_OFFSET(dPa_waveEcallBack, mpBaseEmitter, 0x60);
struct dPa_splashEcallBack {
    void remove();
    void setup(void* emitter, void* pos, void* angle, u32 info);
    void execute(void* emitter);
    /* 0x0 */ be<u32> __vtbl;
    /* 0x4 */ be<s16> mState;
    u8 padding6[2];
    /* 0x8 */ be<f32> mScaleTimer;
    /* 0xC */ be<f32> mMaxScaleTimer;
    /* 0x10 */ be<u32> mpPos;
    /* 0x14 */ be<u32> mpRot;
    /* 0x18 */ be<u32> mpBaseEmitter;
};
WWHD_SIZE(dPa_splashEcallBack, 0x1C);
WWHD_OFFSET(dPa_splashEcallBack, __vtbl, 0x0);
WWHD_OFFSET(dPa_splashEcallBack, mState, 0x4);
WWHD_OFFSET(dPa_splashEcallBack, mScaleTimer, 0x8);
WWHD_OFFSET(dPa_splashEcallBack, mMaxScaleTimer, 0xC);
WWHD_OFFSET(dPa_splashEcallBack, mpPos, 0x10);
WWHD_OFFSET(dPa_splashEcallBack, mpRot, 0x14);
WWHD_OFFSET(dPa_splashEcallBack, mpBaseEmitter, 0x18);
struct dPa_trackEcallBack {
    void getMaxWaterY(void* pos);
    void remove();
    void setup(void* emitter, void* pos, void* angle, u32 info);
    void execute(void* emitter);
    void drawBefore(void* emitter);
    void draw(void* emitter);
    /* 0x0 */ be<u32> __vtbl;
    /* 0x4 */ be<s16> mState;
    u8 padding6[2];
    /* 0x8 */ be<f32> mBaseY;
    /* 0xC */ be<f32> mMinY;
    /* 0x10 */ EmitterVector mPos[3];
    /* 0x34 */ be<u32> mpPos;
    /* 0x38 */ be<u32> mpRot;
    /* 0x3C */ be<f32> mExTransY;
    /* 0x40 */ be<f32> mExScaleY;
    /* 0x44 */ be<f32> mVel;
    /* 0x48 */ be<f32> mMinVel;
    /* 0x4C */ be<u32> mpBaseEmitter;
};
WWHD_SIZE(dPa_trackEcallBack, 0x50);
WWHD_OFFSET(dPa_trackEcallBack, __vtbl, 0x0);
WWHD_OFFSET(dPa_trackEcallBack, mState, 0x4);
WWHD_OFFSET(dPa_trackEcallBack, mBaseY, 0x8);
WWHD_OFFSET(dPa_trackEcallBack, mMinY, 0xC);
WWHD_OFFSET(dPa_trackEcallBack, mPos, 0x10);
WWHD_OFFSET(dPa_trackEcallBack, mpPos, 0x34);
WWHD_OFFSET(dPa_trackEcallBack, mpRot, 0x38);
WWHD_OFFSET(dPa_trackEcallBack, mExTransY, 0x3C);
WWHD_OFFSET(dPa_trackEcallBack, mExScaleY, 0x40);
WWHD_OFFSET(dPa_trackEcallBack, mVel, 0x44);
WWHD_OFFSET(dPa_trackEcallBack, mMinVel, 0x48);
WWHD_OFFSET(dPa_trackEcallBack, mpBaseEmitter, 0x4C);
struct dPa_cutTurnEcallBack_c {
    void executeAfter(void* emitter);
    void setup(void* emitter, void* pos, void* angle, u32 info);
    void end();
    /* 0x0 */ be<u32> __vtbl;
    /* 0x4 */ be<u8> mAlpha;
    /* 0x5 */ be<u8> field_0x5;
    /* 0x6 */ be<s16> field_0x6;
    /* 0x8 */ be<u32> mPosArray;
    /* 0xC */ be<u32> mpBaseEmitter;
};
WWHD_SIZE(dPa_cutTurnEcallBack_c, 0x10);
WWHD_OFFSET(dPa_cutTurnEcallBack_c, __vtbl, 0x0);
WWHD_OFFSET(dPa_cutTurnEcallBack_c, mAlpha, 0x4);
WWHD_OFFSET(dPa_cutTurnEcallBack_c, field_0x5, 0x5);
WWHD_OFFSET(dPa_cutTurnEcallBack_c, field_0x6, 0x6);
WWHD_OFFSET(dPa_cutTurnEcallBack_c, mPosArray, 0x8);
WWHD_OFFSET(dPa_cutTurnEcallBack_c, mpBaseEmitter, 0xC);
struct dPa_simpleEcallBack {
    s32 set(void* pos, u8 alpha, void* primary, void* environment, s32 flags);
    void draw(void* emitter);
    void* createEmitter(void* manager);
    void* create(void* manager, u32 id, u32 group);
    void executeAfter(void* emitter);
    /* 0x0 */ be<u32> __vtbl;
    /* 0x4 */ be<u32> mpBaseEmitter;
    /* 0x8 */ be<u16> mResID;
    /* 0xA */ be<u8> mGrpID;
    /* 0xB */ be<u8> mbIsSmoke;
    /* 0xC */ be<s16> mCount;
    u8 paddingE[2];
    /* 0x10 */ dPa_simpleData_c mSimpleData[32];
};
WWHD_SIZE(dPa_simpleEcallBack, 0x290);
WWHD_OFFSET(dPa_simpleEcallBack, __vtbl, 0x0);
WWHD_OFFSET(dPa_simpleEcallBack, mpBaseEmitter, 0x4);
WWHD_OFFSET(dPa_simpleEcallBack, mResID, 0x8);
WWHD_OFFSET(dPa_simpleEcallBack, mGrpID, 0xA);
WWHD_OFFSET(dPa_simpleEcallBack, mbIsSmoke, 0xB);
WWHD_OFFSET(dPa_simpleEcallBack, mCount, 0xC);
WWHD_OFFSET(dPa_simpleEcallBack, mSimpleData, 0x10);
} // namespace pa

namespace pa {
// HD: controller state precedes the GameCube heap/scene members and adds render resources.
// HD: controller parameters store a shader name and four scale components.
struct ParticleControllerParameter {
    be<u32> mName;
    be<u32> mNameVtable;
    be<f32> mScale[4];
};
WWHD_SIZE(ParticleControllerParameter, 24);
WWHD_OFFSET(ParticleControllerParameter, mName, 0);
WWHD_OFFSET(ParticleControllerParameter, mNameVtable, 4);
WWHD_OFFSET(ParticleControllerParameter, mScale, 8);
struct dPa_control_c {
    enum {
        dPtclGroup_Normal_e = 0,
        dPtclGroup_NormalP1_e,
        dPtclGroup_Toon_e,
        dPtclGroup_ToonP1_e,
        dPtclGroup_Projection_e,
        dPtclGroup_ShipTail_e,
        dPtclGroup_Wind_e,
        dPtclGroup_2Dfore_e,
        dPtclGroup_2Dback_e,
        dPtclGroup_2DmenuFore_e,
        dPtclGroup_2DmenuBack_e,
        // HD: two additional menu groups follow the GameCube groups.
        dPtclGroup_Count_e = 13,
    };
    s32 newSimple(u32 id, u32 group);
    s32 readScene(u32 room, void* request);
    void createScene(void* data);
    void calc2D();
    void calc3D();
    void calcMenu();
    void runSubstep(void* info);
    void runSubstepAfter(void* info);
    void runDrawSubstep(void* info);
    void draw(void* info, u32 group);
    s32 checkAtrCodeEffect(s32 code);
    void* getSimple(u32 id);
    s32 setSimple(u32 id, void* pos, u32 alpha, void* primary, void* environment, s32 flags);
    void* setBombSmoke(u32 id, void* pos, void* rot, void* scale, u32 alpha);
    void* setNormalStripes(u32 id, void* pos, void* rot, void* scale, u32 alpha, u32 work);
    void* set(u32 group, u32 id, void* pos, void* angle, void* scale, u32 alpha, void* callback);
    void* setSimpleLandPoly(void* poly, void* pos, void* angle, void* tev, void* returnId,
                            u32 flags, f32 sx, f32 sy, f32 sz);
    void* setSimpleLand(s32 code, void* pos, void* angle, void* tev, void* out, u32 flags,
                        f32 landScale, f32 waterScale, f32 foamScale);
    void modelControlDraw();
    s32 removeRoomScene(s32 force, s32 room);
    void removeScene();
    void createRoomScene(void* data);
    void applyDefaultParameters();
    void createCommon(void* data);
    ParticleControllerParameter mParameters[10];
    EmitterColor mPrimaryColor;
    EmitterColor mEnvironmentColor;
    be<f32> mColorBlend;
    be<u32> mParameterReferences[3];
    EmitterColor mDrawPrimaryColor;
    EmitterColor mDrawEnvironmentColor;
    be<f32> mDrawColorBlend;
    be<u32> mRenderState[4];
    be<u8> mMenuDrawEnabled;
    u8 padding125[3];
    be<u32> mHeap;
    be<u32> mCommonResMng;
    be<u32> mModelControl;
    be<u32> mSceneHeap;
    be<u32> mpData;
    be<u32> mSceneResMng;
    be<s32> mSceneReleaseTimer;
    be<u8> mSceneNo;
    be<u8> mCount;
    be<u8> mNumSimple;
    be<u8> mNumSimpleCommon;
    dPa_simpleEcallBack mSimpleCallbacks[25];
    be<u32> mRenderControl;
    be<u32> mTextureBank;
    u8 hdCallbacks[0x44];
};
WWHD_SIZE(dPa_control_c, 0x41A4);
WWHD_OFFSET(dPa_control_c, mSimpleCallbacks, 0x148);
WWHD_OFFSET(dPa_control_c, mRenderControl, 0x4158);
WWHD_OFFSET(dPa_control_c, mTextureBank, 0x415C);
} // namespace pa
namespace pa {
WWHD_OFFSET(dPa_control_c, mHeap, 0x128);
WWHD_OFFSET(dPa_control_c, mCommonResMng, 0x12C);
WWHD_OFFSET(dPa_control_c, mModelControl, 0x130);
WWHD_OFFSET(dPa_control_c, mSceneHeap, 0x134);
WWHD_OFFSET(dPa_control_c, mpData, 0x138);
WWHD_OFFSET(dPa_control_c, mSceneResMng, 0x13C);
WWHD_OFFSET(dPa_control_c, mSceneReleaseTimer, 0x140);
WWHD_OFFSET(dPa_control_c, mSceneNo, 0x144);
WWHD_OFFSET(dPa_control_c, mCount, 0x145);
WWHD_OFFSET(dPa_control_c, mNumSimple, 0x146);
WWHD_OFFSET(dPa_control_c, mNumSimpleCommon, 0x147);
WWHD_OFFSET(dPa_control_c, mRenderControl, 0x4158);
WWHD_OFFSET(dPa_control_c, mTextureBank, 0x415C);
} // namespace pa

namespace pa {
// HD: the emitter pointer precedes the secondary vtable, and a second model shifts animation
// fields.
struct dPa_J3DmodelEmitter_c {
    u8 node[12];
    be<u32> mpBaseEmitter;
    be<u32> __vtbl;
    be<u32> mpHeap;
    be<u32> modelData;
    be<u32> hdModelData;
    be<u32> field_0x1c;
    be<u32> field_0x20;
    be<u16> field_0x24;
    be<u8> field_0x26;
    u8 padding2B;
    u8 tevStr[0x1C8];
};
WWHD_SIZE(dPa_J3DmodelEmitter_c, 0x1F4);
WWHD_OFFSET(dPa_J3DmodelEmitter_c, mpBaseEmitter, 12);
WWHD_OFFSET(dPa_J3DmodelEmitter_c, __vtbl, 16);
WWHD_OFFSET(dPa_J3DmodelEmitter_c, mpHeap, 20);
WWHD_OFFSET(dPa_J3DmodelEmitter_c, modelData, 24);
WWHD_OFFSET(dPa_J3DmodelEmitter_c, hdModelData, 28);
WWHD_OFFSET(dPa_J3DmodelEmitter_c, field_0x1c, 32);
WWHD_OFFSET(dPa_J3DmodelEmitter_c, field_0x20, 36);
WWHD_OFFSET(dPa_J3DmodelEmitter_c, field_0x24, 40);
WWHD_OFFSET(dPa_J3DmodelEmitter_c, field_0x26, 42);
WWHD_OFFSET(dPa_J3DmodelEmitter_c, tevStr, 0x2C);
} // namespace pa

namespace pa {
// HD: the GameCube particle prefix is followed by GPU buffers and draw state.
struct JPABaseParticle {
    u8 padding0[16];
    EmitterVector mOffsetPosition;
    EmitterVector mLocalPosition;
    EmitterVector mGlobalPosition;
    EmitterVector mVelocity;
    u8 padding40[76];
    EmitterVector mAxis;
    be<f32> mScaleOut;
    be<f32> mScaleX;
    be<f32> mScaleY;
    u8 paddingA4[8];
    be<f32> mAlphaOut;
    be<f32> mAlphaWaveRandom;
    be<s32> mLoopOffset;
    EmitterColor mPrmColor;
    EmitterColor mEnvColor;
    be<u16> mRotateAngle;
    be<s16> mRotateSpeed;
    u8 paddingC4[2];
    be<u16> mTexIdx;
    be<u32> mpCallBack2;
    be<u32> mStatus;
    u8 paddingD0[16];
    be<u32> hdRippleBuffer;
    u8 paddingE4[4];
    be<u32> hdMaterial;
    be<u32> hdModel;
    be<u32> hdModelData;
    u8 paddingF4[12];
    be<u32> hdVertexBuffer;
    u8 padding104[8];
    be<u8> hdHidden;
    u8 padding10D[3];
    be<f32> hdDrawWidth;
    be<f32> hdDrawHeight;
    u8 padding118[8];
    be<u8> hdRenderMode;
    u8 padding121[1];
    be<u8> hdSmokeMode;
    u8 padding123[1];
    be<u32> hdDrawIndex;
};
WWHD_SIZE(JPABaseParticle, 0x128);
WWHD_OFFSET(JPABaseParticle, mOffsetPosition, 0x10);
WWHD_OFFSET(JPABaseParticle, mLocalPosition, 0x1C);
WWHD_OFFSET(JPABaseParticle, mGlobalPosition, 0x28);
WWHD_OFFSET(JPABaseParticle, mVelocity, 0x34);
WWHD_OFFSET(JPABaseParticle, mAxis, 0x8C);
WWHD_OFFSET(JPABaseParticle, mScaleOut, 0x98);
WWHD_OFFSET(JPABaseParticle, mScaleX, 0x9C);
WWHD_OFFSET(JPABaseParticle, mScaleY, 0xA0);
WWHD_OFFSET(JPABaseParticle, mAlphaOut, 0xAC);
WWHD_OFFSET(JPABaseParticle, mAlphaWaveRandom, 0xB0);
WWHD_OFFSET(JPABaseParticle, mLoopOffset, 0xB4);
WWHD_OFFSET(JPABaseParticle, mPrmColor, 0xB8);
WWHD_OFFSET(JPABaseParticle, mEnvColor, 0xBC);
WWHD_OFFSET(JPABaseParticle, mRotateAngle, 0xC0);
WWHD_OFFSET(JPABaseParticle, mRotateSpeed, 0xC2);
WWHD_OFFSET(JPABaseParticle, mTexIdx, 0xC6);
WWHD_OFFSET(JPABaseParticle, mpCallBack2, 0xC8);
WWHD_OFFSET(JPABaseParticle, mStatus, 0xCC);
WWHD_OFFSET(JPABaseParticle, hdRippleBuffer, 0xE0);
WWHD_OFFSET(JPABaseParticle, hdMaterial, 0xE8);
WWHD_OFFSET(JPABaseParticle, hdModel, 0xEC);
WWHD_OFFSET(JPABaseParticle, hdModelData, 0xF0);
WWHD_OFFSET(JPABaseParticle, hdVertexBuffer, 0x100);
WWHD_OFFSET(JPABaseParticle, hdHidden, 0x10C);
WWHD_OFFSET(JPABaseParticle, hdDrawWidth, 0x110);
WWHD_OFFSET(JPABaseParticle, hdDrawHeight, 0x114);
WWHD_OFFSET(JPABaseParticle, hdRenderMode, 0x120);
WWHD_OFFSET(JPABaseParticle, hdSmokeMode, 0x122);
WWHD_OFFSET(JPABaseParticle, hdDrawIndex, 0x124);
} // namespace pa

namespace pa {
// HD: each pooled model keeps a second model alongside the GameCube model.
struct dPa_J3Dmodel_c {
    be<u8> mInit;
    u8 padding1[3];
    be<u32> mModel;
    be<u32> hdModel;
};
WWHD_SIZE(dPa_J3Dmodel_c, 12);
WWHD_OFFSET(dPa_J3Dmodel_c, mInit, 0);
WWHD_OFFSET(dPa_J3Dmodel_c, mModel, 4);
WWHD_OFFSET(dPa_J3Dmodel_c, hdModel, 8);
} // namespace pa

void pa_PSMTXRotRad(void* matrix, u32 axis, f32 angle);
void pa_PSMTXRotAxisRad(void* matrix, void* axis, f32 angle);
void pa_mDoExt_modelUpdateDL(void* model, u32 flag);
s32 pa_cLib_chaseF(void* value, f32 target, f32 step);
void* pa_createParticle(void* emitter);
void pa_setWindPower(void* particle);

namespace pa {
// HD: particle shader setup stores a SafeString followed by scale and enable components.
struct ParticleShaderName {
    be<u32> mString;
    be<u32> __vtbl;
};
struct ParticleShaderConfig {
    ParticleShaderName mName;
    be<f32> mScaleX;
    be<f32> mScaleY;
    be<f32> mScaleZ;
    be<f32> mScaleW;
    be<u8> mEnableX;
    be<u8> mEnableY;
    be<u8> mEnableZ;
    be<u8> mEnableW;
};
WWHD_SIZE(ParticleShaderName, 8);
WWHD_OFFSET(ParticleShaderName, mString, 0);
WWHD_OFFSET(ParticleShaderName, __vtbl, 4);
WWHD_SIZE(ParticleShaderConfig, 28);
WWHD_OFFSET(ParticleShaderConfig, mName, 0);
WWHD_OFFSET(ParticleShaderConfig, mScaleX, 8);
WWHD_OFFSET(ParticleShaderConfig, mScaleY, 12);
WWHD_OFFSET(ParticleShaderConfig, mScaleZ, 16);
WWHD_OFFSET(ParticleShaderConfig, mScaleW, 20);
WWHD_OFFSET(ParticleShaderConfig, mEnableX, 24);
WWHD_OFFSET(ParticleShaderConfig, mEnableY, 25);
WWHD_OFFSET(ParticleShaderConfig, mEnableZ, 26);
WWHD_OFFSET(ParticleShaderConfig, mEnableW, 27);
} // namespace pa

namespace pa {
enum : u32 {
    JPAEmtrStts_StopEmit = 0x01,
    JPAEmtrStts_StopCalc = 0x02,
    JPAEmtrStts_StopDraw = 0x04,
    JPAEmtrStts_EnableDeleteEmitter = 0x08,
    JPAEmtrStts_FirstEmit = 0x10,
    JPAEmtrStts_RateStepEmit = 0x20,
    JPAEmtrStts_Immortal = 0x40,
};
}

void* pa_simpleCreateEmitter(void* callback, void* manager);
u32 pa_getRM_ID(u32 id);
s32 pa_effectNeedsSpecialWind(u32 id);
void pa_waveRemove(void* callback);
void pa_splashRemove(void* callback);
void pa_trackRemove(void* callback);
void pa_trackGetMaxWaterY(void* callback, void* position);
void* pa_newModel(void* modelKind);

namespace pa {
// HD: particle rendering alternates native vertex and draw buffer records.
struct ParticleNativeVertexBuffer {
    u8 state[0x14C];
    be<u32> mSize;
    u8 padding150[4];
};
struct ParticleRenderBuffer {
    be<u32> mData;
    ParticleNativeVertexBuffer mVertexBuffer;
    u8 mDrawBuffer[0xFC];
};
struct ParticleSmokeBuffers {
    ParticleRenderBuffer mBuffers[2];
    be<u32> mBank;
};
struct ParticleRippleBuffers {
    ParticleRenderBuffer mBuffers[4];
    be<u32> mBank;
};
struct ParticleStripesBuffers {
    ParticleRenderBuffer mBuffers[10];
    be<u32> mBank;
    u8 padding174C[12];
    be<u32> mDrawReady;
};
WWHD_SIZE(ParticleNativeVertexBuffer, 0x154);
WWHD_OFFSET(ParticleNativeVertexBuffer, mSize, 0x14C);
WWHD_SIZE(ParticleRenderBuffer, 0x254);
WWHD_OFFSET(ParticleRenderBuffer, mData, 0);
WWHD_OFFSET(ParticleRenderBuffer, mVertexBuffer, 4);
WWHD_OFFSET(ParticleRenderBuffer, mDrawBuffer, 0x158);
WWHD_SIZE(ParticleSmokeBuffers, 0x4AC);
WWHD_OFFSET(ParticleSmokeBuffers, mBank, 0x4A8);
WWHD_SIZE(ParticleRippleBuffers, 0x954);
WWHD_OFFSET(ParticleRippleBuffers, mBank, 0x950);
WWHD_SIZE(ParticleStripesBuffers, 0x175C);
WWHD_OFFSET(ParticleStripesBuffers, mBank, 0x1748);
WWHD_OFFSET(ParticleStripesBuffers, mDrawReady, 0x1758);
} // namespace pa

s32 pa_cBgS_ChkPolySafe(void* background, void* polygon);
f32 pa_cBgS_GroundCross(void* background, void* check);
void pa_cBgS_GndChk_ctor(void* check);
s32 pa_dBgS_GetAttributeCode(void* background, void* polygon);
s32 pa_dBgS_SplGrpChk(void* background, void* check);
void pa_dBgS_WtrChk_ctor(void* check);
void* pa_mDoExt_createSolidHeapFromGame(u32 size, void* parent);
void* pa_mDoExt_createSolidHeapToCurrent(u32 size, void* parent, u32 alignment);
void* pa_mDoExt_J3DModel_create(void* data, u32 flags, u32 modelFlags);
void pa_mDoExt_btpAnm_entry(void* animation, void* modelData, s32 frame);
void* pa_dRes_control_getRes(void* resources, void* name, u32 index);
s32 pa_J3DUClipper_clip(void* clipper, void* matrix, void* position, f32 radius);

s32 pa_removeRoomScene(void* control, u32 force, u32 room);
void pa_modelControlDtor(void* control);
s32 pa_newSimple(void* control, u32 id, u32 group);
void* pa_smokeCtorFlags(void* callback, u32 rate, u32 fixed, u32 flag, u32 temporary);
void* pa_modelControlCtor(void* control, void* first, void* second, void* heap);
void pa_applyDefaultParameters(void* control);
void pa_colorToFloat(void* output, void* color);
void pa_copyMatrix(void* output, void* input);
void pa_followSetup(void* callback, void* emitter, void* position, void* angle, u32 info);
void* pa_modelEmitterCtor(void* emitter);
void* pa_simpleCreate(void* callback, void* manager, u32 id, u32 group);
void pa_createRoomScene(void* control, void* data);
void* pa_getSimple(void* control, u32 id);
s32 pa_simpleSet(void* callback, void* position, u32 alpha, void* primary, void* environment,
                 s32 flags);
void* pa_setSimpleLand(void* control, s32 code, void* position, void* angle, void* tev,
                       void* output, u32 flags, f32 landScale, f32 waterScale, f32 foamScale);
s32 pa_checkAtrCodeEffect(void* control, s32 code);
void* pa_followCtor(void* callback, u8 rate, u8 fixed);
void pa_applyLighting(void* emitter, void* tev, s32 info, void* color);
u32 pa_referenceCountDecrement(void* counter);
void pa_colorS10ToFloat(void* output, void* color);
void* pa_setEmitter(void* control, u32 group, u32 id, void* position, void* angle, void* scale,
                    u32 alpha, void* callback, s32 info, void* primary, void* environment,
                    void* scale2D);

void pa_JUTAssertion_showAssert(void* file, u32 line, void* message);
void pa_bzero(void* destination, u32 size);

void pa_cLs_Create(void* list);
void pa_cLs_SingleCut(void* node);
void pa_cNd_ForcedClear(void* node);
void pa_MTXCopy(void* source, void* destination);
void pa_setLightTevColorType(void* environment, void* model, void* tev);
void pa_dKy_setLight_again();
void pa_JPAEmitterManager_draw(void* manager, void* drawInfo, u32 group);
void* pa_JPAEmitterManager_createSimpleEmitterID(void* manager, void* position, u32 id, u32 group,
                                                 u32 resource, u32 emitterCallback,
                                                 u32 particleCallback);
void* pa_JPAEmitterManager_ctor(void* manager, void* resource, u32 maxParticles, u32 maxEmitters,
                                u32 maxGroups, void* heap, void* renderControl);
void pa_JPAEmitterManager_substep(void* manager, void* info, u8 group);
void pa_JPAEmitterManager_substepAfter(void* manager, void* info, u8 group);
void pa_JPAEmitterManager_drawSubstep(void* manager, void* info, u8 group);
void pa_JPAEmitterManager_prepareMenuDraw(void* manager);
void* pa_JPAResourceManager_ctor(void* manager, void* data, void* heap, void* textureBank);
void* pa_JPAResourceManager_getResource(void* manager, u32 id);
void pa_destroyArray(void* array, u32 count, u32 stride, void* destructor, u32 flags, u32 reserved);
void* pa_constructModelArray(void* array, u32 count, u32 stride, void* constructor, u32 flags);
void pa_cBgS_Chk_dtor(void* check, u32 flags);
void pa_cBgS_GndChk_baseDtor(void* check, u32 flags);
void pa_blendParticleColors(void* output, void* input, f32 blend);
void pa_setParticleShaderConfig(void* emitter, void* config);
void* pa_getParticleMaterial(void* modelData, u32 index);
void pa_setParticleMaterialMatrix(void* material, void* matrix, u32 index);
void* pa_mDoExt_btpAnm_ctor(void* animation);
void pa_mDoExt_btpAnm_init(void* animation, void* modelData, void* pattern, u32 play, u32 attribute,
                           u32 start, s32 end, u32 modify, u32 reserved, f32 speed);

// HD: SDK bindings named for their verified particle call-site roles.
void pa_notifyParticleRoomRelease(s32 room);
void pa_initializeMathHeaderPrimary(void* state);
void pa_initializeMathHeaderSecondary(void* state);
void* pa_getParticleSystemHeap();
void* pa_createParticleHeap(u32 size, void* name, void* parent, u32 direction, u32 flags);
u32 pa_getParticleHeapSize(void* heap);
void pa_OSReport(void* format);
void pa_OSReport(void* format, u32 size);
u32 pa_exchangeParticleContext(void* manager, void* context);
void pa_restoreParticleContext(void* manager, u32 context);
void* pa_loadParticleTextureBank(void* resources, void* archive, void* resource, u32 flags);
void* pa_getParticleTextureBank();
void pa_initializeParticleRenderControl(void* control, void* texture, void* heap);
void* pa_findParticleTexture(void* table, void* name);
void pa_registerParticleTexture(void* manager, void* texture, u16 id);
u32 pa_allocateParticleModelPool(void* allocator, void* heap, u32 alignment);
void pa_prepareParticleModelLighting();
void* pa_getParticleCallbackContext(void* callback);
void pa_setParticleTexture(void* resource, u32 index, u32 mode, void* model);
void pa_setParticleTexture(void* resource, u32 index, u32 mode, u32 model);
void pa_initializeSmokeParticleModel(void* particle);
void pa_captureParticleShaderState(void* state);
void pa_applyParticleShaderState(void* state);
void pa_releaseTrackEmitterRendering(void* emitter);
void pa_prepareWaveEmitterRendering(void* emitter);
void pa_drawParticleModel(void* particle);
void pa_drawParticleModel(void* particle, void* emitter);
void* pa_getParticleShaderBank();
s32 pa_findParticleShader(void* bank, void* name);
void pa_initializeParticleShader(void* entry, void* data, u32 flags);
void pa_prepareParticleMenuEffect(void* effect);
void pa_prepareParticleWorldEffect(void* effect);
void pa_applyParticleWorldEffect(void* effect);
void* pa_getParticleCameraMatrix(void* camera);
u32 pa_getParticleWidthCount(void* emitter);
u32 pa_getParticleHeightCount(void* emitter);
void pa_prepareStripesEmitterRendering(void* emitter);
void pa_copyParticleBytes(void* destination, void* source, u32 count);
void pa_selectParticleDrawMode(void* particle, u32 mode);
void pa_prepareRippleParticleDraw(void* particle, u32 first, u32 size, u32 flags);

#ifndef WWHD_PARTICLE_DATA_OWNER
extern const be<f32>& l_particleColorRange;
extern const be<f32>& l_particleOne;
extern const be<f32>& l_particleZero;
extern const be<f32>& l_particleNormalizeEpsilon;
extern const be<f32>& l_particleDegreesToRadians;
extern const be<f32>& l_particleAngleToDegrees;
extern const be<f32>& l_particleHalf;
#endif

#ifndef WWHD_PARTICLE_DATA_OWNER
extern be<u32>& l_particleEmitterManager;
extern be<u32>& l_particleModelPool;
extern be<u8>& l_particleStatus;
#endif

namespace pa {
WWHD_OFFSET(dPa_control_c, mParameters, 0x0);
WWHD_OFFSET(dPa_control_c, mPrimaryColor, 0xF0);
WWHD_OFFSET(dPa_control_c, mEnvironmentColor, 0xF4);
WWHD_OFFSET(dPa_control_c, mColorBlend, 0xF8);
WWHD_OFFSET(dPa_control_c, mParameterReferences, 0xFC);
WWHD_OFFSET(dPa_control_c, mDrawPrimaryColor, 0x108);
WWHD_OFFSET(dPa_control_c, mDrawEnvironmentColor, 0x10C);
WWHD_OFFSET(dPa_control_c, mDrawColorBlend, 0x110);
WWHD_OFFSET(dPa_control_c, mRenderState, 0x114);
WWHD_OFFSET(dPa_control_c, mMenuDrawEnabled, 0x124);
} // namespace pa

#ifndef WWHD_PARTICLE_DATA_OWNER
extern const be<u32> (&l_particleSimpleVtable)[14];
extern const be<u32> (&l_particleFollowVtable)[18];
extern const be<u32> (&l_particleSmokeVtable)[18];
extern const be<u32> (&l_particleRippleVtable)[18];
extern const be<u32> (&l_particleModelEmitterVtable)[6];
#endif
