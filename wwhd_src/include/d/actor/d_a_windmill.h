#pragma once
#include "gc_api/gc_api.h"
struct WindmillCapsuleData {
    cXyz mStart, mEnd;
    be<f32> mRadius;
};
struct WindCapsuleGeometry {
    cXyz mStart, mEnd;
    be<u32> __vtbl;
    be<f32> mRadius;
};
WWHD_SIZE(WindmillCapsuleData, 0x1C);
WWHD_SIZE(WindCapsuleGeometry, 0x20);
WWHD_OFFSET(WindCapsuleGeometry, __vtbl, 0x18);
struct daWindMill_c : fopAc_ac_c {
    enum Type { LARGE = 0, SMALL = 1, NUM_TYPES = 2 };
    BOOL CreateHeap();
    void set_mtx();
    void search_wind();
    void CreateInit();
    s32 _create();
    BOOL _delete();
    BOOL _draw();
    void hane_move();
    void set_at();
    void set_co();
    BOOL _execute();

    request_of_phase_process_class mPhs;
    gptr<J3DModel> mpModel;
    dCcD_Stts mStts;
    dCcD_Sph mSph[9];
    dCcD_Cps mCps[4];
    WindmillCapsuleData m1244[4];
    gptr<dBgW> mpBgW;
    Mtx34 mMtx;
    dCcD_Cyl mCyl;
    be<u8> mType;
    u8 pad;
    be<s16> mAngle[3];
    be<u32> mWindTagId;
};
WWHD_OFFSET(daWindMill_c, mpModel, 0x3B4);
WWHD_OFFSET(daWindMill_c, mSph, 0x3F4);
WWHD_OFFSET(daWindMill_c, mCps, 0xE80);
WWHD_OFFSET(daWindMill_c, m1244, 0x1360);
WWHD_OFFSET(daWindMill_c, mpBgW, 0x13D0);
WWHD_OFFSET(daWindMill_c, mCyl, 0x1404);
WWHD_OFFSET(daWindMill_c, mType, 0x1534);
WWHD_OFFSET(daWindMill_c, mWindTagId, 0x153C);
static_assert(sizeof(daWindMill_c) == 0x1540);

// HD: keep the verified caller linkage words below every guest temporary payload.
namespace wind {
template <class T> struct Local {
    struct Frame {
        be<u32> linkage[2];
        T payload;
    };
    static_assert(offsetof(Frame, payload) == 8);
    gabi::Local<Frame> frame;
    T *get() const { return gabi::at<T>(frame.a + 8); }
    T *operator->() const { return get(); }
    T &operator*() const { return *get(); }
    operator T *() const { return get(); }
};
} // namespace wind

struct WindText {
    char bytes[1];
};
struct WindBgView {
    u8 _00[0xA8];
    be<u32> mCrrFunc;
};
WWHD_OFFSET(WindBgView, mCrrFunc, 0xA8);
struct WindJointView {
    u8 _00[4];
    be<u16> mJointNo;
};
WWHD_OFFSET(WindJointView, mJointNo, 4);
struct WindJointTree {
    u8 _00[8];
    be<u16> mJointNum;
};
WWHD_OFFSET(WindJointTree, mJointNum, 8);
struct WindTagView {
    u8 _00[0x6EC];
    be<u8> mType;
    u8 _6ED[3];
    be<f32> mOffsY;
    be<f32> field_0x49c;
};
WWHD_OFFSET(WindTagView, mType, 0x6EC);
WWHD_OFFSET(WindTagView, mOffsY, 0x6F0);
WWHD_OFFSET(WindTagView, field_0x49c, 0x6F4);
struct WindPlayView {
    u8 _00[0x5D70];
    be<u32> mBgOpaque, mBgTranslucent, mOpaque, mTranslucent;
};
WWHD_OFFSET(WindPlayView, mBgOpaque, 0x5D70);
WWHD_OFFSET(WindPlayView, mTranslucent, 0x5D7C);
struct WindCapsuleView : dCcD_GObjInf {
    u8 _F8[0x18];
    be<u32> mAabVtable, mShapeVtable;
    WindCapsuleGeometry mCps;
};
WWHD_OFFSET(WindCapsuleView, mAabVtable, 0x110);
WWHD_OFFSET(WindCapsuleView, mShapeVtable, 0x114);
WWHD_OFFSET(WindCapsuleView, mCps, 0x118);
WWHD_SIZE(WindCapsuleView, 0x138);
struct WindCylView : dCcD_GObjInf {
    u8 _F8[0x18];
    be<u32> mAabVtable, mShapeVtable;
    cM3dGCyl mCyl;
};
WWHD_OFFSET(WindCylView, mAabVtable, 0x110);
WWHD_OFFSET(WindCylView, mShapeVtable, 0x114);
WWHD_OFFSET(WindCylView, mCyl, 0x118);
WWHD_SIZE(WindCylView, 0x130);
inline void *windmillResourceController() {
    return gabi::at<void>(gabi::object_at<be<u32>>(0x101F4F28));
}
namespace windAbi {
enum : u32 {
    TypicalRotY = 0x024EE708,
    NodeCallback = 0x024E0CF4,
    ActorVtable = 0x10042C1C,
    SttsVtable = 0x1004AE88,
    GSttsVtable = 0x1004AEC0,
    V100015A8 = 0x100015A8,
    V10042C0C = 0x10042C0C,
    V1004B160 = 0x1004B160,
    V1004B108 = 0x1004B108,
    V1004B150 = 0x1004B150,
    V1004AF18 = 0x1004AF18,
    V1004AF60 = 0x1004AF60,
    V1004AF70 = 0x1004AF70
};
}
template <class R = void, class... A> inline R windCall_releaseBg(A... args) {
    constexpr u32 callTarget = 0x020087EC;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_setBg(A... args) {
    constexpr u32 callTarget = 0x0200A030;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_constructStts(A... args) {
    constexpr u32 callTarget = 0x0200BD2C;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_registerCollision(A... args) {
    constexpr u32 callTarget = 0x0200E240;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_addCalcAngle(A... args) {
    constexpr u32 callTarget = 0x0200F378;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_constructCapsule(A... args) {
    constexpr u32 callTarget = 0x02018150;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_setCapsuleShape(A... args) {
    constexpr u32 callTarget = 0x020181FC;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_setCylinderCenter(A... args) {
    constexpr u32 callTarget = 0x020182E0;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_constructCylinder(A... args) {
    constexpr u32 callTarget = 0x02018590;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_setSphereRadius(A... args) {
    constexpr u32 callTarget = 0x02018C8C;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_setSphereCenter(A... args) {
    constexpr u32 callTarget = 0x02018D40;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_registerBg(A... args) {
    constexpr u32 callTarget = 0x024EEA6C;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_constructBg(A... args) {
    constexpr u32 callTarget = 0x024F23F4;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_moveBg(A... args) {
    constexpr u32 callTarget = 0x024F43DC;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_destroyStts(A... args) {
    constexpr u32 callTarget = 0x02515860;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_destroyCylinder(A... args) {
    constexpr u32 callTarget = 0x02515A70;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_constructGStts(A... args) {
    constexpr u32 callTarget = 0x02515DA0;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_initStts(A... args) {
    constexpr u32 callTarget = 0x02515F14;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_constructCollision(A... args) {
    constexpr u32 callTarget = 0x02515FB8;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_setCapsule(A... args) {
    constexpr u32 callTarget = 0x025164C0;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_setCylinder(A... args) {
    constexpr u32 callTarget = 0x02516518;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_setSphere(A... args) {
    constexpr u32 callTarget = 0x0251677C;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_getPlay(A... args) {
    constexpr u32 callTarget = 0x025200D4;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_loadResource(A... args) {
    constexpr u32 callTarget = 0x02520460;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_deleteResource(A... args) {
    constexpr u32 callTarget = 0x025204C8;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_getReverb(A... args) {
    constexpr u32 callTarget = 0x02520540;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_envLight(A... args) {
    constexpr u32 callTarget = 0x02555D0C;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_setTev(A... args) {
    constexpr u32 callTarget = 0x025626A4;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_setLight(A... args) {
    constexpr u32 callTarget = 0x02562F5C;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_constructActor(A... args) {
    constexpr u32 callTarget = 0x025D4ED0;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_destroyActor(A... args) {
    constexpr u32 callTarget = 0x025D50BC;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_search(A... args) {
    constexpr u32 callTarget = 0x025D5218;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_enterHeap(A... args) {
    constexpr u32 callTarget = 0x025D63E8;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_setCullBox(A... args) {
    constexpr u32 callTarget = 0x025D674C;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_startSound(A... args) {
    constexpr u32 callTarget = 0x025E1A40;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_updateModel(A... args) {
    constexpr u32 callTarget = 0x025E2DE0;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_entryModel(A... args) {
    constexpr u32 callTarget = 0x025E2E5C;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_createModel(A... args) {
    constexpr u32 callTarget = 0x025E38E0;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_rotateZXY(A... args) {
    constexpr u32 callTarget = 0x025F1B48;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_rotateY(A... args) {
    constexpr u32 callTarget = 0x025F1C28;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_rotateZ(A... args) {
    constexpr u32 callTarget = 0x025F1C5C;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_getResource(A... args) {
    constexpr u32 callTarget = 0x026066C4;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_assertFail(A... args) {
    constexpr u32 callTarget = 0x0273AA24;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_allocate(A... args) {
    constexpr u32 callTarget = 0x0273AD10;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_free(A... args) {
    constexpr u32 callTarget = 0x0273AF40;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_jointTree(A... args) {
    constexpr u32 callTarget = 0x027F3F94;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_calcModel(A... args) {
    constexpr u32 callTarget = 0x027F4D5C;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_jointData(A... args) {
    constexpr u32 callTarget = 0x027F7878;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_multiply(A... args) {
    constexpr u32 callTarget = 0x028E8F64;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_copyMatrix(A... args) {
    constexpr u32 callTarget = 0x028E90D4;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_translate(A... args) {
    constexpr u32 callTarget = 0x028E93CC;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_initStaticByte(A... args) {
    constexpr u32 callTarget = 0x028EAB2C;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_initStaticChar(A... args) {
    constexpr u32 callTarget = 0x028ED6F8;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_constructArray(A... args) {
    constexpr u32 callTarget = 0x028EFFD0;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_destroyArray(A... args) {
    constexpr u32 callTarget = 0x028F0164;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R windCall_registerStatic(A... args) {
    constexpr u32 callTarget = 0x028F026C;
    return gabi::call<R>(callTarget, args...);
}

struct WindStaticState {
    be<u32> words[4];
};
struct WindRegistration {
    u8 bytes[12];
};
inline void *windCollisionSpace(void *play) { return gabi::at<void>(gabi::ea(play) + 0x26A4); }
inline void *windBackgroundSpace(void *play) { return gabi::at<void>(gabi::ea(play) + 0x12A0); }
inline void *windGuestFunction(u32 target) { return gabi::at<void>(target); }
namespace windAbi {
enum : u32 {
    SearchByName = 0x025E121C,
    SphereConstructor = 0x025166F0,
    CapsuleConstructor = 0x024E1DA4,
    HeapCallback = 0x024E0CF0,
    SearchById = 0x025E1234,
    CapsuleDestructor = 0x02515980,
    SphereDestructor = 0x02515AE8
};
}

namespace windAbi {
enum : u32 {
    SafeStringVtable = 0x10042BF4,
    WindTagProfile = 0x187,
    LargeWindSound = 0x7043,
    SmallWindSound = 0x7044
};
}
