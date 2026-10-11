/* BG source-only HD views. Shared d_a_bg.h stays unchanged for createHeap.
 * HD: four model slots include extra render-list flags; animation curves use indirect evaluators.
 */
#pragma once
#include "d/actor/d_a_bg.h"
#include "gc_api/gc_api.h"
struct BgRoomStatus {
    u8 _00[0x54];
    dKy_tevstr_c mTevStr;
    be<u8> mFlags;
    u8 _21D[0x228 - 0x21D];
    gptr<dBgW> mpBgW;
};
WWHD_OFFSET(BgRoomStatus, mTevStr, 0x54);
WWHD_OFFSET(BgRoomStatus, mFlags, 0x21C);
WWHD_OFFSET(BgRoomStatus, mpBgW, 0x228);
WWHD_SIZE(BgRoomStatus, 0x22C);
struct BgCurve {
    be<f32> mResult, mStart, mEnd;
    u8 _0C[4];
    gfn mEvaluate;
    be<u32> mContext;
};
WWHD_OFFSET(BgCurve, mEvaluate, 0x10);
WWHD_OFFSET(BgCurve, mContext, 0x14);
WWHD_SIZE(BgCurve, 0x18);
struct BgAnimation {
    J3DFrameCtrl mFrameCtrl;
    be<u32> mTextureCurve;
    u8 _14[12];
    be<u32> mColorCurve;
    u8 _24[0x68 - 0x24];
    gptr<be<f32>> mpTextureFrame;
};
WWHD_OFFSET(BgAnimation, mTextureCurve, 0x10);
WWHD_OFFSET(BgAnimation, mColorCurve, 0x20);
WWHD_OFFSET(BgAnimation, mpTextureFrame, 0x68);
WWHD_SIZE(BgAnimation, 0x6C);
struct BgMaterialKey {
    u8 _00[0xC];
    be<u32> mNameData;
};
WWHD_OFFSET(BgMaterialKey, mNameData, 0xC);
WWHD_SIZE(BgMaterialKey, 0x10);
struct BgMaterialNames {
    u8 _00[0x2C];
    be<u32> mTableOffset;
};
WWHD_OFFSET(BgMaterialNames, mTableOffset, 0x2C);
WWHD_SIZE(BgMaterialNames, 0x30);
struct BgNameTable {
    u8 _00[0x10];
    be<u32> mFirstNameOffset;
};
WWHD_OFFSET(BgNameTable, mFirstNameOffset, 0x10);
WWHD_SIZE(BgNameTable, 0x14);
struct BgClipper {
    u8 _00[0x54];
    be<f32> mFar;
};
WWHD_OFFSET(BgClipper, mFar, 0x54);
WWHD_SIZE(BgClipper, 0x58);
struct BgSave {
    u8 _00[0x1688];
    be<u8> mSpecialDraw;
};
WWHD_OFFSET(BgSave, mSpecialDraw, 0x1688);
WWHD_SIZE(BgSave, 0x1689);
struct BgPlay {
    u8 _00[0x5AB4];
    be<u32> mMagma, mGrass, mTree, mWood, mFlower;
    u8 _5AC8[0x5D00 - 0x5AC8];
    be<u16> mWaveFrame;
    u8 _5D02[0x5D4C - 0x5D02];
    be<u32> mSpecialOpa, mSpecialXlu;
    u8 _5D54[0x5DA0 - 0x5D54];
    be<u32> mInvisibleOpa;
};
WWHD_OFFSET(BgPlay, mMagma, 0x5AB4);
WWHD_OFFSET(BgPlay, mGrass, 0x5AB8);
WWHD_OFFSET(BgPlay, mTree, 0x5ABC);
WWHD_OFFSET(BgPlay, mWood, 0x5AC0);
WWHD_OFFSET(BgPlay, mFlower, 0x5AC4);
WWHD_OFFSET(BgPlay, mWaveFrame, 0x5D00);
WWHD_OFFSET(BgPlay, mSpecialOpa, 0x5D4C);
WWHD_OFFSET(BgPlay, mSpecialXlu, 0x5D50);
WWHD_OFFSET(BgPlay, mInvisibleOpa, 0x5DA0);
WWHD_SIZE(BgPlay, 0x5DA4);
struct BgText {
    char bytes[32];
};
WWHD_SIZE(BgText, 32);
struct BgGrassRoom {
    u8 _00[0x1A5CC];
    be<u32> mRooms[1];
};
WWHD_OFFSET(BgGrassRoom, mRooms, 0x1A5CC);
WWHD_SIZE(BgGrassRoom, 0x1A5D0);
struct BgTreeRoom {
    u8 mData[0x10C];
};
WWHD_SIZE(BgTreeRoom, 0x10C);
struct BgTree {
    u8 _00[0x4FBC];
    BgTreeRoom mRooms[1];
};
WWHD_OFFSET(BgTree, mRooms, 0x4FBC);
WWHD_SIZE(BgTree, 0x50C8);
struct BgFlower {
    u8 _00[0x457C];
    be<u32> mRooms[1];
};
WWHD_OFFSET(BgFlower, mRooms, 0x457C);
WWHD_SIZE(BgFlower, 0x4580);
// HD: room identifiers participate in wrapping 32-bit table arithmetic.
inline BgRoomStatus *Bg_roomStatus(s32 room) {
    return gabi::at<BgRoomStatus>(daBg_roomStatus(room));
}
inline BgPlay *Bg_play() { return gabi::at<BgPlay>(dComIfGp_ea()); }
// HD: private bindings keep the original ABI, including full-width animation results.
/* 028F186C sprintf (varargs) */
static inline s32 Bg_sprintf_g(u32 buf, u32 fmt, u32 v) {
    constexpr u32 target = 0x028F186C;
    return gabi::call<s32>(target, buf, fmt, v);
}
static inline void *Bg_mDoExt_btkAnm_new() {
    constexpr u32 target = 0x025E7C6C;
    return gabi::call<void *>(target, (u32)0);
} /* new mDoExt_btkAnm() */
static inline void *Bg_mDoExt_brkAnm_new() {
    constexpr u32 target = 0x025E80D0;
    return gabi::call<void *>(target, (u32)0);
} /* new mDoExt_brkAnm() */
static inline BOOL Bg_mDoExt_brkAnm_init(void *a, J3DModelData *d, J3DAnmTevRegKey *k, bool play,
                                         s32 mode, f32 speed, s16 start, s16 end, bool modify,
                                         s32 entry) {
    constexpr u32 target = 0x025E8154;
    return gabi::call<BOOL>(target, a, d, k, play, mode, speed, start, end, modify, entry);
}
static inline void Bg_brkEntry(void *a, J3DModelData *d, f32 frame) {
    constexpr u32 target = 0x025E83FC;
    gabi::call(target, a, d, frame);
}
static inline void Bg_dKy_tevstr_init(dKy_tevstr_c *t, s8 roomNo, u8 p) {
    constexpr u32 target = 0x0255FFF4;
    gabi::call(target, t, roomNo, p);
}
/* 02520630 Bg_dComIfGp_getMapTrans(roomNo, f32* x, f32* z, s16* angle) */
static inline BOOL Bg_dComIfGp_getMapTrans(s32 roomNo, be<f32> *x, be<f32> *z, be<s16> *a) {
    constexpr u32 target = 0x02520630;
    return gabi::call<BOOL>(target, roomNo, x, z, a);
}
static inline void Bg_dStage_escapeRestart() {
    constexpr u32 target = 0x025C3ED0;
    gabi::call(target);
}
static inline void Bg_J3DUClipper_calcViewFrustum(void *c) {
    constexpr u32 target = 0x0283801C;
    gabi::call(target, c);
}
static inline void Bg_J3DUClipper_clip(void *c, J3DModel *m) {
    constexpr u32 target = 0x02838524;
    gabi::call(target, c, m);
}

template <class... A> inline u32 Bg_createHeap(A... args) {
    constexpr u32 target = 0x02077FCC;
    return gabi::call<u32>(target, args...);
}

template <class... A> inline void Bg_setInvisibleLighting(A... args) {
    constexpr u32 target = 0x0255F8A0;
    return gabi::call<void>(target, args...);
}

template <class... A> inline void Bg_resetInvisibleLighting(A... args) {
    constexpr u32 target = 0x0255F84C;
    return gabi::call<void>(target, args...);
}

template <class... A> inline void Bg_deleteMagmaRoom(A... args) {
    constexpr u32 target = 0x0258CE1C;
    return gabi::call<void>(target, args...);
}

template <class... A> inline void Bg_deleteGrassRoom(A... args) {
    constexpr u32 target = 0x0254AC50;
    return gabi::call<void>(target, args...);
}

template <class... A> inline void Bg_deleteTreeRoom(A... args) {
    constexpr u32 target = 0x025C73B8;
    return gabi::call<void>(target, args...);
}

template <class... A> inline void Bg_deleteWoodRoom(A... args) {
    constexpr u32 target = 0x025D0738;
    return gabi::call<void>(target, args...);
}

template <class... A> inline void Bg_deleteFlowerRoom(A... args) {
    constexpr u32 target = 0x025459A8;
    return gabi::call<void>(target, args...);
}

template <class... A> inline void Bg_destroyActor(A... args) {
    constexpr u32 target = 0x025D50BC;
    return gabi::call<void>(target, args...);
}

template <class... A> inline void Bg_evaluateCurveSlot(A... args) {
    constexpr u32 target = 0x027DF40C;
    return gabi::call<void>(target, args...);
}
