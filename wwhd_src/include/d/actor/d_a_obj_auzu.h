/* WWHD whirlpool. */
#pragma once
#include "gc_api/gc_api.h"
namespace daObjAuzu {
struct Attr_c {
    be<f32> mRadiusMult, mOuterActivationMult, mInnerActivationMult, mAnmSpeed;
};
struct Act_c : fopAc_ac_c {
    Act_c();
    const Attr_c &attr() const;
    enum Prm_e {
        PRM_APPEAR_W = 1,
        PRM_APPEAR_S = 16,
        PRM_LINK_ID_W = 8,
        PRM_LINK_ID_S = 8,
        PRM_SWSAVE_W = 8,
        PRM_SWSAVE_S = 0,
        PRM_TYPE_W = 4,
        PRM_TYPE_S = 20
    };
    request_of_phase_process_class mPhs;
    gptr<J3DModel> mpModel;
    mDoExt_btkAnm mBtkAnm;
    be<s32> mType;
    be<f32> mScaleAnimFactor;
    be<u8> mbToAppear, mbIsExist, mbBgmStarted;
    u8 _437;
    be<u32> mKytagPcId;
    u32 is_exist();
    bool create_heap();
    void set_mtx();
    void init_mtx();
    void set_state_map();
    s32 _create();
    bool _delete();
    void bgm_start();
    void ship_whirl();
    bool _execute();
    bool _draw();
};
WWHD_OFFSET(Act_c, mType, 0x42C);
WWHD_OFFSET(Act_c, mKytagPcId, 0x438);
WWHD_SIZE(Act_c, 0x43C);
struct FloatColor {
    be<f32> r, g, b, a;
};
} // namespace daObjAuzu

using daObjAuzu::FloatColor;
struct AuzuColor {
    be<u8> r, g, b, a;
};
struct AuzuModel {
    u8 pad[0x74];
    be<u32> mFlags;
};
struct AuzuShip {
    u8 pad0[8];
    be<s16> mProcName;
    u8 padA[0x63B - 10];
    be<u8> m0352;
    u8 pad63C[0x69A - 0x63C];
    be<s16> m03B2;
    u8 pad69C[0x708 - 0x69C];
    be<u32> mWhirlID, mWhirlActor;
};
struct AuzuPlayer {
    u8 pad[0x6A8C];
    be<u32> mWhirlId;
};
struct AuzuPlay {
    u8 pad[0x5B34];
    gptr<AuzuPlayer> mPlayer;
    u8 gap[4];
    gptr<AuzuShip> mShip;
    u8 gap2[0x5CD8 - 0x5B40];
    be<u32> mPlayerStatus;
};
inline AuzuPlay *auzuPlay() {
    return gabi::at<AuzuPlay>(dComIfGp_ea());
}
struct AuzuWaveTag {
    u8 pad[0x3B8];
    be<f32> mOuterRadius, mInnerRadius;
};
struct AuzuTevBlock {
    u8 pad[4];
    be<u32> mVtable;
    AuzuColor *getTevKColor(u32 slot) {
        return gabi::call<AuzuColor *>(gabi::load<u32>(mVtable + 0x4C), this, slot);
    }
    void setTevKColor(u32 slot, AuzuColor *color) {
        gabi::call(gabi::load<u32>(mVtable + 0x3C), this, slot, color);
    }
};
struct AuzuUniform {
    be<u32> mFlags;
};
struct AuzuMaterial {
    u8 pad0[4];
    gptr<AuzuMaterial> mNext;
    gptr<J3DShape> mShape;
    u8 padC[12];
    gptr<AuzuTevBlock> mTevBlock;
    u8 pad1C[0xA0 - 0x1C];
    AuzuUniform mUniform;
};
struct AuzuJoint {
    u8 pad[0x10];
    gptr<AuzuMaterial> mMesh;
};
struct AuzuAttributes {
    daObjAuzu::Attr_c values[2];
};
struct AuzuModelData {
    u8 pad[8];
    gptr<AuzuJoint> mJointNodes;
};
// HD: private views cover only the fields touched by this unit.
struct AuzuHeaderZeros {
    be<u32> words[4];
};
void auzuSetModelFlags(J3DModel *, u32);
void auzuSubBgmStart(u32);
void auzuConvertColor(FloatColor *, FloatColor *, f32);
FloatColor *auzuGetUniform(AuzuUniform *, u32);
void auzuInitA(void *);
void auzuInitB(void *);
void auzuActorDt(void *, s32);
