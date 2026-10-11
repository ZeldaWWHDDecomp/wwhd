/** Tower of the Gods floor switch: GameCube class names with the verified WWHD layout.
 * HD: actor base is 0x3E0, status/cylinder retain their GC sizes, and HIO/BRK/emitter
 * views expose only the fields this unit exercises.
 */
#pragma once
#include "d/d_bg_s.h"
#include "d/d_cc_d.h"
#include "m_Do/m_Do_ext.h"

// HD: private views of the fields exercised by this actor. Particle offsets are four bytes
// earlier than GameCube; BRK begins with the reordered, vtable-free HD frame controller.
struct SwflatBrk {
    /* 0x00 */ J3DFrameCtrl mFrameCtrl;
    u8 _10[0x68];
};
WWHD_OFFSET(SwflatBrk, mFrameCtrl, 0);
WWHD_SIZE(SwflatBrk, 0x78);
struct SwflatEmitter {
    u8 _00[0x5C];
    /* 0x5C */ be<s32> mMaxFrame;
    u8 _60[0x247 - 0x60];
    /* 0x247 */ be<u8> mAlpha;
    u8 _248[0xC];
    /* 0x254 */ be<u32> mFlags;
};
WWHD_OFFSET(SwflatEmitter, mMaxFrame, 0x5C);
WWHD_OFFSET(SwflatEmitter, mAlpha, 0x247);
WWHD_OFFSET(SwflatEmitter, mFlags, 0x254);
WWHD_SIZE(SwflatEmitter, 0x258);
struct SwflatPositionWords {
    be<u32> x, y, z;
};
WWHD_SIZE(SwflatPositionWords, 0xC);
struct SwflatShapeView {
    u8 _00[0x110];
    be<u32> mAabVtable;
    be<u32> __vtbl;
};
WWHD_OFFSET(SwflatShapeView, mAabVtable, 0x110);
WWHD_OFFSET(SwflatShapeView, __vtbl, 0x114);
WWHD_SIZE(SwflatShapeView, 0x118);
// HD: contact tests use the profile name, rather than fopAcM_GetName's process name.
struct SwflatContactView : fopAc_ac_c {
    /* 0x3AC */ u8 _3AC[0x79E - 0x3AC];
    /* 0x79E */ be<u8> mPressed;
};
WWHD_OFFSET(SwflatContactView, mPressed, 0x79E);
WWHD_SIZE(SwflatContactView, 0x79F);
struct daObjSwflat_HIO_c {
    /* 0x00 */ be<s8> mNo;
    u8 _01[3];
    /* 0x04 */ be<s32> mCount;
    /* 0x08 */ be<f32> mRadius;
    /* 0x0C */ be<f32> mHeight;
    /* 0x10 */ be<s16> mEventTime;
    /* 0x12 */ be<s16> mLockTime;
    /* 0x14 */ be<u8> mbDebug;
    /* 0x15 */ be<u8> mbResetSwitch;
    u8 _16[2];
    /* 0x18 */ be<u32> __vtbl;
};
WWHD_OFFSET(daObjSwflat_HIO_c, mCount, 4);
WWHD_OFFSET(daObjSwflat_HIO_c, mEventTime, 0x10);
WWHD_OFFSET(daObjSwflat_HIO_c, __vtbl, 0x18);
WWHD_SIZE(daObjSwflat_HIO_c, 0x1C);

enum {
    dRes_INDEX_HFBOT_BDL_HFBOT_e = 5,
    dRes_INDEX_HFBOT_BRK_HFBOT_e = 8,
    dRes_INDEX_HFBOT_DZB_HFBOT_e = 11
};

namespace daObjSwflat {
struct Act_c : dBgS_MoveBgActor {
    /* 0x3E0 */ request_of_phase_process_class mPhase;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ gptr<SwflatBrk> mpBrk;
    /* 0x3F0 */ dCcD_Stts mStts;
    /* 0x42C */ dCcD_Cyl mCyl;
    /* 0x55C */ gptr<SwflatEmitter> mpInactiveEmitter;
    /* 0x560 */ gptr<SwflatEmitter> mpActiveEmitter;
    /* 0x564 */ cXyz mEffectPos;
    /* 0x570 */ be<s16> mFrame;
    /* 0x572 */ be<s16> mPressEndFrame;
    /* 0x574 */ be<s16> mEventTimer;
    /* 0x576 */ be<s16> mLockTimer;
    /* 0x578 */ be<u8> mType;
    /* 0x579 */ be<u8> mPressed;
    /* 0x57A */ be<u8> mWasPressed;
    /* 0x57B */ be<u8> mSwitch;
    /* 0x57C */ be<u8> mSwitch2;
    /* 0x57D */ be<u8> mInactiveAlpha;
    /* 0x57E */ be<u8> mActiveAlpha;
    /* 0x57F */ be<u8> mLocked;
    /* 0x580 */ be<u8> mEventState;
    u8 _581[3];
    BOOL CreateHeap();
    BOOL Create();
    s32 Mthd_Create();
    BOOL Delete();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    BOOL Execute(Mtx34**);
    BOOL Draw();
    BOOL IsDelete();
};
WWHD_OFFSET(Act_c, mPhase, 0x3E0);
WWHD_OFFSET(Act_c, mpModel, 0x3E8);
WWHD_OFFSET(Act_c, mpBrk, 0x3EC);
WWHD_OFFSET(Act_c, mStts, 0x3F0);
WWHD_OFFSET(Act_c, mCyl, 0x42C);
WWHD_OFFSET(Act_c, mpInactiveEmitter, 0x55C);
WWHD_OFFSET(Act_c, mpActiveEmitter, 0x560);
WWHD_OFFSET(Act_c, mEffectPos, 0x564);
WWHD_OFFSET(Act_c, mFrame, 0x570);
WWHD_OFFSET(Act_c, mPressEndFrame, 0x572);
WWHD_OFFSET(Act_c, mEventTimer, 0x574);
WWHD_OFFSET(Act_c, mLockTimer, 0x576);
WWHD_OFFSET(Act_c, mType, 0x578);
WWHD_OFFSET(Act_c, mPressed, 0x579);
WWHD_OFFSET(Act_c, mWasPressed, 0x57A);
WWHD_OFFSET(Act_c, mSwitch, 0x57B);
WWHD_OFFSET(Act_c, mSwitch2, 0x57C);
WWHD_OFFSET(Act_c, mInactiveAlpha, 0x57D);
WWHD_OFFSET(Act_c, mActiveAlpha, 0x57E);
WWHD_OFFSET(Act_c, mLocked, 0x57F);
WWHD_OFFSET(Act_c, mEventState, 0x580);
WWHD_SIZE(Act_c, 0x584);
} // namespace daObjSwflat
