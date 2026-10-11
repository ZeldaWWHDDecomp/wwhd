#pragma once
#include "gc_api/gc_api.h"
namespace lodbg_local {
struct ResourceName {
    be<u32> text, vtable, capacity;
    be<u8> bytes[0x20];
};
struct MemberFunction {
    be<s16> adjustment, slot;
    union {
        be<u32> target;
        struct {
            be<u16> _targetHigh;
            be<s16> vtableOffset;
        };
    };
};
static_assert(sizeof(ResourceName) == 0x2C);
static_assert(sizeof(MemberFunction) == 8);
} // namespace lodbg_local
class daLodbg_c : public fopAc_ac_c {
  public:
    BOOL createHeap();
    BOOL draw();
    BOOL execute();
    BOOL createModelData();
    BOOL execCreateWait();
    BOOL execReadWait();
    BOOL execDeleteWait();
    lodbg_local::ResourceName mResourceName;  // 3AC
    lodbg_local::MemberFunction mExecuteFunc; // 3D8
    gptr<J3DModel> mModel, mModel2[2];
    gptr<J3DModelData> mModelData, mModelData2;
    be<u8> mAlpha, mDrawModel2, mResourceActive;
    u8 reserved3F7;
};
static_assert(sizeof(daLodbg_c) == 0x3F8);
static_assert(offsetof(daLodbg_c, mResourceName) == 0x3AC);
static_assert(offsetof(daLodbg_c, mExecuteFunc) == 0x3D8);
static_assert(offsetof(daLodbg_c, mModel) == 0x3E0);
static_assert(offsetof(daLodbg_c, mModel2) == 0x3E4);
static_assert(offsetof(daLodbg_c, mModelData) == 0x3EC);
static_assert(offsetof(daLodbg_c, mModelData2) == 0x3F0);
static_assert(offsetof(daLodbg_c, mAlpha) == 0x3F4);
static_assert(offsetof(daLodbg_c, mResourceActive) == 0x3F6);

struct ColorF {
    be<f32> r, g, b, a;
};
struct LodText {
    char bytes[1];
};
struct LodCamera {
    u8 _00[0xDC];
    cXyz mEye;
};
struct LodPlayView {
    u8 _00[0x513E];
    be<s8> mRoomNo;
    u8 _513F[0x5D08 - 0x513F];
    be<u32> mCurrentView;
};
WWHD_OFFSET(LodPlayView, mRoomNo, 0x513E);
WWHD_OFFSET(LodPlayView, mCurrentView, 0x5D08);
struct LodRoomState {
    u8 _00[0x21C];
    be<u8> mStatus;
    u8 _21D[0xF];
};
WWHD_SIZE(LodRoomState, 0x22C);
struct LodClipper {
    u8 _00[0x54];
    be<f32> mFar;
};
enum : u32 {
    LodActorVtable = 0x10014004,
    LodSafeStringVtable = 0x10013FBC,
    LodBufferVtable = 0x10013FD4,
    LodResourceNameVtable = 0x10013FEC
};
template <class R = void, class... A> inline R lodbgAssert(A... args) {
    constexpr u32 callTarget = 0x0273AA24;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgCreateModel(A... args) {
    constexpr u32 callTarget = 0x025E38E0;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgCreateHeapTail(A... args) {
    constexpr u32 callTarget = 0x021B43BC;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgResourceState(A... args) {
    constexpr u32 callTarget = 0x02523D08;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgReleaseResource(A... args) {
    constexpr u32 callTarget = 0x02524180;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgDeleteHeap(A... args) {
    constexpr u32 callTarget = 0x025D6134;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgDeleteModelDataTail(A... args) {
    constexpr u32 callTarget = 0x021B4CB0;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgDeleteTail(A... args) {
    constexpr u32 callTarget = 0x021B4D78;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgAllocate(A... args) {
    constexpr u32 callTarget = 0x0273AD10;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgConstructActor(A... args) {
    constexpr u32 callTarget = 0x025D4ED0;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgInitTev(A... args) {
    constexpr u32 callTarget = 0x0255FFF4;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgConstructTail(A... args) {
    constexpr u32 callTarget = 0x021B4D80;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgCreateStepTail(A... args) {
    constexpr u32 callTarget = 0x021B4EB8;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgPlayerDistance(A... args) {
    constexpr u32 callTarget = 0x025D6958;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgLoadResource(A... args) {
    constexpr u32 callTarget = 0x02523A04;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgResourceModelData(A... args) {
    constexpr u32 callTarget = 0x0252447C;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgCreateModelDataTail(A... args) {
    constexpr u32 callTarget = 0x021B50C4;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgEnterHeap(A... args) {
    constexpr u32 callTarget = 0x025D63E8;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgRegisterStatic(A... args) {
    constexpr u32 callTarget = 0x028F026C;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgInitStaticChar(A... args) {
    constexpr u32 callTarget = 0x028ED6F8;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgInitStaticByte(A... args) {
    constexpr u32 callTarget = 0x028EAB2C;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgFree(A... args) {
    constexpr u32 callTarget = 0x0273AF40;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgDestroyActor(A... args) {
    constexpr u32 callTarget = 0x025D50BC;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgMaterialTable(A... args) {
    constexpr u32 callTarget = 0x027F3F8C;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgNormalizeColor(A... args) {
    constexpr u32 callTarget = 0x021B4528;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgTintColor(A... args) {
    constexpr u32 callTarget = 0x0274D458;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgMaterialChannel(A... args) {
    constexpr u32 callTarget = 0x027F9F0C;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgEnvLight(A... args) {
    constexpr u32 callTarget = 0x02555D0C;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgSetTev(A... args) {
    constexpr u32 callTarget = 0x025626A4;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgSetLight(A... args) {
    constexpr u32 callTarget = 0x02562F5C;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgUpdateClipper(A... args) {
    constexpr u32 callTarget = 0x0283801C;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgDrawTail(A... args) {
    constexpr u32 callTarget = 0x021B45DC;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgSubtract(A... args) {
    constexpr u32 callTarget = 0x0201ADE0;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgSquare(A... args) {
    constexpr u32 callTarget = 0x028E8DD0;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgSqrt(A... args) {
    constexpr u32 callTarget = 0x028F4384;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgTranslate(A... args) {
    constexpr u32 callTarget = 0x028E93CC;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgRotateY(A... args) {
    constexpr u32 callTarget = 0x025F1C28;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgRenewLight(A... args) {
    constexpr u32 callTarget = 0x025268C8;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgGetLightAngle(A... args) {
    constexpr u32 callTarget = 0x02526944;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgCalcModel(A... args) {
    constexpr u32 callTarget = 0x027F4D5C;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgClipModel(A... args) {
    constexpr u32 callTarget = 0x02838524;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgEntryModel(A... args) {
    constexpr u32 callTarget = 0x025E2E5C;
    return gabi::call<R>(callTarget, args...);
}
template <class R = void, class... A> inline R lodbgPlayAddress(A... args) {
    constexpr u32 callTarget = 0x025200D4;
    return gabi::call<R>(callTarget, args...);
}
template <class... A> inline void lodbgFormatName(A... args) {
    constexpr u32 callTarget = 0x02759C28;
    gabi::call(callTarget, args...);
}

struct LodTevVtable {
    u8 _00[0x3C];
    be<u32> mSetKColor;
    u8 _40[0xC];
    be<u32> mGetKColor;
};
struct LodTevBlock {
    u8 _00[4];
    gptr<LodTevVtable> mVtable;
    u32 getKColor(u32 index) { return gabi::call<u32>((u32)mVtable->mGetKColor, this, index); }
    void setKColor(u32 index, void *color) {
        gabi::call<void>((u32)mVtable->mSetKColor, this, index, color);
    }
};
struct LodMaterialView {
    u8 _00[0x18];
    gptr<LodTevBlock> mTevBlock;
    u8 _1C[0x84];
    be<u32> mChannelFlags;
};
WWHD_OFFSET(LodMaterialView, mTevBlock, 0x18);
WWHD_OFFSET(LodMaterialView, mChannelFlags, 0xA0);
struct LodMaterialInfo {
    u8 _00[0x24];
    be<u16> mMaterialNum;
};
struct LodRegistration {
    u8 bytes[12];
};
struct LodStaticState {
    be<u32> words[4];
};
inline LodMaterialView *lodbgMaterial(J3DModelData *data, u16 index) {
    u32 material = data->mMaterials;
    if ((u32)index < (u32)data->mMaterialNum)
        material += (u32)index * 0x39C;
    return gabi::at<LodMaterialView>(material);
}

using Lod = daLodbg_c;
namespace {
template <class T> T read(u32 a, u32 o = 0) { return *gabi::at<be<T>>(a + o); }

void *ptr(u32 address) { return gabi::at<void>(address); }
u32 play() { return lodbgPlayAddress<u32>(); }
s32 eventOn(u32 id) { return dComIfGs_isEventBit((u16)id); }
u32 actionTarget(Lod *l, u32 &self) {
    self = gabi::ea(l) + (s32)(s16)l->mExecuteFunc.adjustment;
    s16 slot = l->mExecuteFunc.slot;
    return slot < 0 ? (u32)l->mExecuteFunc.target
                    : read<u32>(read<u32>(self, (s32)(s16)l->mExecuteFunc.vtableOffset),
                                (u32)slot * 8 + 4);
}
void setAction(Lod *l, u32 target) {
    l->mExecuteFunc.adjustment = 0;
    l->mExecuteFunc.slot = -1;
    l->mExecuteFunc.target = target;
}
void nameAccess(Lod *l) {
    auto *resource = &l->mResourceName;
    gabi::call<void>(read<u32>(resource->vtable, 0x14), resource);
}
void copyMatrix(J3DModel *model) {
    f32 values[12];
    const Mtx34 &matrix = gabi::object_at<Mtx34>(0x1048D0CC);
    const be<f32> *source = reinterpret_cast<const be<f32> *>(&matrix);
    be<f32> *destination = reinterpret_cast<be<f32> *>(&model->mBaseTransformMtx);
    for (u32 i = 0; i < 12; i++)
        values[i] = source[i];
    for (u32 i = 0; i < 12; i++)
        destination[i] = values[i];
}
void drawModel(gptr<J3DModel> &modelSlot) {
    lodbgCalcModel<void>(modelSlot.get());
    lodbgClipModel<void>(ptr(0x1048CFF0), modelSlot.get());
    lodbgEntryModel<void>(modelSlot.get());
}
} // namespace

inline LodPlayView *lodbgPlay() { return gabi::at<LodPlayView>(lodbgPlayAddress<u32>()); }

enum : u32 { LodHeapCallback = 0x021B4524 };

namespace lodbg_state {
enum : u32 { CreateWait = 0x021B5010, ReadWait = 0x021B5224, DeleteWait = 0x021B5340 };
}
namespace lodbg_room {
enum : u32 {
    ForsakenFortress = 1,
    WindfallIsland = 11,
    DragonRoostIsland = 13,
    TowerOfTheGods = 26
};
}

WWHD_OFFSET(lodbg_local::ResourceName, text, 0);
WWHD_OFFSET(lodbg_local::ResourceName, vtable, 4);
WWHD_OFFSET(lodbg_local::ResourceName, capacity, 8);
WWHD_OFFSET(lodbg_local::ResourceName, bytes, 0xC);
WWHD_OFFSET(lodbg_local::MemberFunction, adjustment, 0);
WWHD_OFFSET(lodbg_local::MemberFunction, slot, 2);
WWHD_OFFSET(lodbg_local::MemberFunction, target, 4);
WWHD_OFFSET(lodbg_local::MemberFunction, vtableOffset, 6);
WWHD_OFFSET(LodCamera, mEye, 0xDC);
WWHD_OFFSET(LodClipper, mFar, 0x54);
WWHD_OFFSET(LodTevVtable, mSetKColor, 0x3C);
WWHD_OFFSET(LodTevVtable, mGetKColor, 0x4C);
WWHD_OFFSET(LodTevBlock, mVtable, 4);
WWHD_OFFSET(LodMaterialInfo, mMaterialNum, 0x24);
// HD: mResourceName and mResourceActive replace the GameCube archive/heap bookkeeping.
// The action pair follows the HD member-function ABI; model pointers retain their retail names.
