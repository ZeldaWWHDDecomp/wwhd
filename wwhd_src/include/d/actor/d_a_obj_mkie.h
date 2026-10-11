/* Earth Temple elephant statue. GameCube names with HD actor prefix/BRK animation expansion.
 * HD layout: phase0x3E0, models0x3E8, animation0x3F0, triangles0x468, status0xEE8. */
#pragma once
#include "gc_api/gc_api.h"
namespace daObjMkie {
struct Attr_c {
    gptr<const char> name;
    be<f32> mScale;
    cXyz table[8][3];
};
WWHD_OFFSET(Attr_c, mScale, 4);
WWHD_OFFSET(Attr_c, table, 8);
WWHD_SIZE(Attr_c, 0x128);
struct Act_c : dBgS_MoveBgActor {
    enum Prm_e {
        PRM_EVID_W = 8,
        PRM_EVID_S = 0,
        PRM_TYPE_W = 1,
        PRM_TYPE_S = 8,
        PRM_CORRECT_W = 1,
        PRM_CORRECT_S = 12,
        PRM_FLAG_W = 1,
        PRM_FLAG_S = 13,
        PRM_SWSAVE_W = 8,
        PRM_SWSAVE_S = 16
    };
    enum Mode { WAIT, DEMO_WAIT, DEMO };

    request_of_phase_process_class mPhs;
    gptr<J3DModel> mModelBase, mModelVan;
    mDoExt_brkAnm mBrkVan;
    dCcD_Tri mTri[8];
    dCcD_Stts mStts[8];
    be<s32> mModeProc, mType;
    be<u8> mCcInit, mStatueDelete, mEvtPlaying, mBaseAnmPlaying;
    be<s16> mTimer, mLightCnt, mEvtIdx;
    be<u8> mSwitch, mBroken;
    s32 Mthd_Create();
    BOOL Mthd_Delete();
    BOOL CreateHeap();
    BOOL Create();
    BOOL Execute(be<u32> *);
    BOOL Draw();
    void set_mtx();
    void init_mtx();
    void set_cc_pos();
    void init_cc();
    BOOL chk_light();
    void eff_break();
    void sound_melt();
    void sound_break();
    void vib_break();
    void mode_wait();
    void mode_demoWait_init();
    void mode_demo_init();
    void mode_demoWait();
    void mode_demo();
    void mode_proc_call();
};
WWHD_OFFSET(Act_c, mModelBase, 0x3E8);
WWHD_OFFSET(Act_c, mBrkVan, 0x3F0);
WWHD_OFFSET(Act_c, mTri, 0x468);
WWHD_OFFSET(Act_c, mStts, 0xEE8);
WWHD_OFFSET(Act_c, mModeProc, 0x10C8);
WWHD_OFFSET(Act_c, mPhs, 0x3E0);
WWHD_OFFSET(Act_c, mModelVan, 0x3EC);
WWHD_OFFSET(Act_c, mType, 0x10CC);
WWHD_OFFSET(Act_c, mCcInit, 0x10D0);
WWHD_OFFSET(Act_c, mStatueDelete, 0x10D1);
WWHD_OFFSET(Act_c, mEvtPlaying, 0x10D2);
WWHD_OFFSET(Act_c, mBaseAnmPlaying, 0x10D3);
WWHD_OFFSET(Act_c, mTimer, 0x10D4);
WWHD_OFFSET(Act_c, mLightCnt, 0x10D6);
WWHD_OFFSET(Act_c, mEvtIdx, 0x10D8);
WWHD_OFFSET(Act_c, mSwitch, 0x10DA);
WWHD_OFFSET(Act_c, mBroken, 0x10DB);
WWHD_SIZE(Act_c, 0x10DC);
} // namespace daObjMkie
struct MkieTriView {
    u8 _00[0x110];
    be<u32> mBoundsVtable, mShapeVtable;
    u8 mGeometry[0x10];
    be<u32> mPlaneVtable;
    u8 _12C[0x24];
};
WWHD_OFFSET(MkieTriView, mGeometry, 0x118);
WWHD_OFFSET(MkieTriView, mPlaneVtable, 0x128);
WWHD_SIZE(MkieTriView, 0x150);

namespace mkie_call {
template <class... A> inline void brkConstruct(A... args) {
    constexpr u32 target = 0x025E80D0;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void constructArray(A... args) {
    constexpr u32 target = 0x028EFFD0;
    return gabi::call<void>(target, args...);
}
template <class... A> inline s32 brkInitHD(A... args) {
    constexpr u32 target = 0x025E8154;
    return gabi::call<s32>(target, args...);
}
template <class... A> inline void scaleM(A... args) {
    constexpr u32 target = 0x025F2518;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void matrixCopy(A... args) {
    constexpr u32 target = 0x028E90D4;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void matrixMultVecArray(A... args) {
    constexpr u32 target = 0x028E8FB8;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void triangleSetPos(A... args) {
    constexpr u32 target = 0x0201924C;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void triangleSet(A... args) {
    constexpr u32 target = 0x0251650C;
    return gabi::call<void>(target, args...);
}
template <class... A> inline s32 eventGetIndex(A... args) {
    constexpr u32 target = 0x02543F10;
    return gabi::call<s32>(target, args...);
}
template <class... A> inline s32 detectLight(A... args) {
    constexpr u32 target = 0x0252A038;
    return gabi::call<s32>(target, args...);
}
template <class... A> inline s32 triangleCheckHit(A... args) {
    constexpr u32 target = 0x025162A4;
    return gabi::call<s32>(target, args...);
}
template <class... A> inline void triangleClearHit(A... args) {
    constexpr u32 target = 0x0251621C;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void startShock(A... args) {
    constexpr u32 target = 0x025CB374;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void collisionSet(A... args) {
    constexpr u32 target = 0x0200E240;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void playRiddle(A... args) {
    constexpr u32 target = 0x025E1988;
    return gabi::call<void>(target, args...);
}
template <class... A> inline u32 eventExists(A... args) {
    constexpr u32 target = 0x02544044;
    return gabi::call<u32>(target, args...);
}
template <class... A> inline u32 brkPlayHD(A... args) {
    constexpr u32 target = 0x025E742C;
    return gabi::call<u32>(target, args...);
}
template <class... A> inline s32 eventEndCheck(A... args) {
    constexpr u32 target = 0x025440C8;
    return gabi::call<s32>(target, args...);
}
template <class R, class... A> inline R allocate(A... args) {
    constexpr u32 target = 0x0273AD10;
    return gabi::call<R>(target, args...);
}
template <class... A> inline void statusConstruct(A... args) {
    constexpr u32 target = 0x0200BD2C;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void globalStatusConstruct(A... args) {
    constexpr u32 target = 0x02515DA0;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void objectInfoConstruct(A... args) {
    constexpr u32 target = 0x02515FB8;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void triangleGeometryConstruct(A... args) {
    constexpr u32 target = 0x02019040;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void destroyArray(A... args) {
    constexpr u32 target = 0x028F0164;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void actorDestroy(A... args) {
    constexpr u32 target = 0x025D50BC;
    return gabi::call<void>(target, args...);
}
} // namespace mkie_call

enum MkieResource {
    dRes_INDEX_MKIEB_BDL_MKIEB_e = 6,
    dRes_INDEX_MKIEB_BDL_YLSMF00_e = 9,
    dRes_INDEX_MKIEB_BRK_YLSMF00_e = 12,
    dRes_INDEX_MKIEB_DZB_MKIEB_e = 15
};

struct MkieGuestText { char bytes[1]; };
WWHD_SIZE(MkieGuestText, 1);
