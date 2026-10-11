/* Rotating grate. HD expands the move-bg actor prefix from 0x2C8 to 0x3E0.
 * Numbered fields retain the GameCube decompilation's names. */
#pragma once
#include "gc_api/gc_api.h"
namespace daObjHami3 {
struct Act_c : dBgS_MoveBgActor {
    enum Prm_e {
        PRM_SWSAVE_W = 8,
        PRM_SWSAVE_S = 0,
        PRM_SWSAVE2_W = 8,
        PRM_SWSAVE2_S = 8,
        PRM_SOUND_W = 1,
        PRM_SOUND_S = 16
    };
    enum Mode { CLOSE_STOP, OPEN_DEMO_WAIT, OPEN_DEMO, OPEN_STOP, CLOSE_DEMO_WAIT, CLOSE_DEMO };
    /* 0x3E0 */ be<s16> field_0x2C8, field_0x2CA;
    /* 0x3E4 */ request_of_phase_process_class field_0x2CC;
    /* 0x3EC */ gptr<J3DModel> field_0x2D4;
    /* 0x3F0 */ be<s32> field_0x2D8;
    /* 0x3F4 */ be<s16> field_0x2DC, field_0x2DE;
    s32 Mthd_Create();
    BOOL Mthd_Delete();
    BOOL CreateHeap();
    BOOL Create();
    BOOL Delete();
    BOOL Execute(be<u32> *);
    BOOL Draw();
    void set_mtx();
    void init_mtx();
    void daObjHami3_close_stop();
    void daObjHami3_open_demo_wait();
    void daObjHami3_open_demo();
    void daObjHami3_open_stop();
    void daObjHami3_close_demo_wait();
    void daObjHami3_close_demo();
};
WWHD_OFFSET(Act_c, field_0x2C8, 0x3E0);
WWHD_OFFSET(Act_c, field_0x2CA, 0x3E2);
WWHD_OFFSET(Act_c, field_0x2CC, 0x3E4);
WWHD_OFFSET(Act_c, field_0x2D4, 0x3EC);
WWHD_OFFSET(Act_c, field_0x2D8, 0x3F0);
WWHD_OFFSET(Act_c, field_0x2DC, 0x3F4);
WWHD_OFFSET(Act_c, field_0x2DE, 0x3F6);
WWHD_SIZE(Act_c, 0x3F8);
struct DrawLists {
    u8 _00[0x5D70];
    be<u32> mOpaList, mXluList, mDefaultOpaList, mDefaultXluList;
};
WWHD_OFFSET(DrawLists, mOpaList, 0x5D70);
WWHD_OFFSET(DrawLists, mXluList, 0x5D74);
WWHD_OFFSET(DrawLists, mDefaultOpaList, 0x5D78);
WWHD_OFFSET(DrawLists, mDefaultXluList, 0x5D7C);
WWHD_SIZE(DrawLists, 0x5D80);
struct PlayView {
    u8 _00[0x52B8];
    be<u16> mEventFlags;
    u8 _52BA[10];
    u8 mEventManager[1];
    u8 _52C5[0x599C - 0x52C5];
    u8 mVibration[1];
};
WWHD_OFFSET(PlayView, mEventFlags, 0x52B8);
WWHD_OFFSET(PlayView, mEventManager, 0x52C4);
WWHD_OFFSET(PlayView, mVibration, 0x599C);
WWHD_SIZE(PlayView, 0x599D);
struct SaveView {
    u8 _00[0x20];
    u8 mMemory[1];
};
WWHD_OFFSET(SaveView, mMemory, 0x20);
WWHD_SIZE(SaveView, 0x21);
struct NameTableView {
    u8 _00[0x10];
    be<u32> mNamesOffset;
};
WWHD_OFFSET(NameTableView, mNamesOffset, 0x10);
WWHD_SIZE(NameTableView, 0x14);
struct JointRecord {
    u8 _00[8];
    be<u32> mCallback;
    u8 _0C[0x10];
};
WWHD_OFFSET(JointRecord, mCallback, 8);
WWHD_SIZE(JointRecord, 0x1C);
struct ActorVtable {
    u8 _00[0x2C];
    gfn mDraw;
    u8 _30[12];
    gfn mIsDelete;
};
WWHD_OFFSET(ActorVtable, mDraw, 0x2C);
WWHD_OFFSET(ActorVtable, mIsDelete, 0x3C);
WWHD_SIZE(ActorVtable, 0x40);
struct JointIndex {
    u8 _00[4];
    be<u16> mJointNo;
};
WWHD_OFFSET(JointIndex, mJointNo, 4);
WWHD_SIZE(JointIndex, 6);
struct GuestText {
    char bytes[1];
};
enum Resource { dRes_INDEX_HAMI3_BDL_HAMI3 = 4, dRes_INDEX_HAMI3_DZB_HAMI3 = 7 };
enum {
    OPEN_ANGLE = 0x4000,
    ROTATION_STEP = 0x100,
    JA_SE_OBJ_AMI_MOVE = 0x61CB,
    JA_SE_READ_RIDDLE_1 = 0x806,
    MATRIX_DIRTY = 0x10,
    ACTOR_INITIALIZED = 8,
    EVENT_RESET = 8
};
} // namespace daObjHami3

namespace hami3_call {
template <class... A> inline u32 getPlay(A... args) {
    constexpr u32 target = 0x025200D4;
    return gabi::call<u32>(target, args...);
}
template <class... A> inline void orderOtherEvent(A... args) {
    constexpr u32 target = 0x025D7A58;
    return gabi::call<void>(target, args...);
}
template <class... A> inline u32 getJointData(A... args) {
    constexpr u32 target = 0x027F7878;
    return gabi::call<u32>(target, args...);
}
template <class... A> inline void matrixCopy(A... args) {
    constexpr u32 target = 0x028E90D4;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void matrixRotateY(A... args) {
    constexpr u32 target = 0x025F1C28;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void constructMoveBgActor(A... args) {
    constexpr u32 target = 0x024F1D40;
    return gabi::call<void>(target, args...);
}
template <class... A> inline s32 resLoad(A... args) {
    constexpr u32 target = 0x02520460;
    return gabi::call<s32>(target, args...);
}
template <class... A> inline s32 moveBGCreate(A... args) {
    constexpr u32 target = 0x024F1D9C;
    return gabi::call<s32>(target, args...);
}
template <class... A> inline void assertFail(A... args) {
    constexpr u32 target = 0x0273AA24;
    return gabi::call<void>(target, args...);
}
template <class... A> inline BOOL moveBGDelete(A... args) {
    constexpr u32 target = 0x024F1F64;
    return gabi::call<BOOL>(target, args...);
}
template <class... A> inline void resDelete(A... args) {
    constexpr u32 target = 0x025204C8;
    return gabi::call<void>(target, args...);
}
template <class... A> inline u32 getObjectResource(A... args) {
    constexpr u32 target = 0x026066C4;
    return gabi::call<u32>(target, args...);
}
template <class... A> inline u32 createModel(A... args) {
    constexpr u32 target = 0x025E38E0;
    return gabi::call<u32>(target, args...);
}
template <class... A> inline u32 getJointNameTable(A... args) {
    constexpr u32 target = 0x027F68FC;
    return gabi::call<u32>(target, args...);
}
template <class... A> inline s32 getNameIndex(A... args) {
    constexpr u32 target = 0x027DF9B0;
    return gabi::call<s32>(target, args...);
}
template <class... A> inline void matrixTranslate(A... args) {
    constexpr u32 target = 0x028E93CC;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void matrixRotateZXY(A... args) {
    constexpr u32 target = 0x025F1B48;
    return gabi::call<void>(target, args...);
}
template <class... A> inline BOOL isSwitch(A... args) {
    constexpr u32 target = 0x025BA0C0;
    return gabi::call<BOOL>(target, args...);
}
template <class... A> inline void setCullBox(A... args) {
    constexpr u32 target = 0x025D674C;
    return gabi::call<void>(target, args...);
}
template <class... A> inline s32 getEventIndex(A... args) {
    constexpr u32 target = 0x02543F10;
    return gabi::call<s32>(target, args...);
}
template <class... A> inline void startSystemSound(A... args) {
    constexpr u32 target = 0x025E1988;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void startShock(A... args) {
    constexpr u32 target = 0x025CB374;
    return gabi::call<void>(target, args...);
}
template <class... A> inline s32 getReverb(A... args) {
    constexpr u32 target = 0x02520540;
    return gabi::call<s32>(target, args...);
}
template <class... A> inline void startPositionSound(A... args) {
    constexpr u32 target = 0x025E1A40;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void onSwitch(A... args) {
    constexpr u32 target = 0x025B9E38;
    return gabi::call<void>(target, args...);
}
template <class... A> inline u32 getEnvironmentLight(A... args) {
    constexpr u32 target = 0x02555D0C;
    return gabi::call<u32>(target, args...);
}
template <class... A> inline void settingTevStruct(A... args) {
    constexpr u32 target = 0x025626A4;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void setLightTevColor(A... args) {
    constexpr u32 target = 0x02562F5C;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void modelUpdate(A... args) {
    constexpr u32 target = 0x025E2DE0;
    return gabi::call<void>(target, args...);
}
template <class... A> inline BOOL moveBGExecute(A... args) {
    constexpr u32 target = 0x024F1E9C;
    return gabi::call<BOOL>(target, args...);
}
template <class... A> inline void registerDestructor(A... args) {
    constexpr u32 target = 0x028F026C;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void constructVector(A... args) {
    constexpr u32 target = 0x028ED6F8;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void constructAngles(A... args) {
    constexpr u32 target = 0x028EAB2C;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void deleteObject(A... args) {
    constexpr u32 target = 0x0273AF40;
    return gabi::call<void>(target, args...);
}
template <class... A> inline void destroyActor(A... args) {
    constexpr u32 target = 0x025D50BC;
    return gabi::call<void>(target, args...);
}
} // namespace hami3_call
