#pragma once
#include "d/actor/d_a_bst.h"
#include "d/d_camera.h"
#include "d/d_event.h"
// Gohdan camera demo (d_a_bst_camera.cpp): tables, tuning and play views; the actor is bst_class (d_a_bst.h).

struct BstCameraPlayer : fopAc_ac_c {
    u8 mBeforeDemo[0x420 - 0x3AC];
    be<s16> mDemoOriginal;
    u8 mBeforeParam[6];
    be<u32> mDemoParam;
    u8 mBeforeMode[4];
    be<u32> mDemoMode;
};
struct BstCameraPlayerVtable {
    u8 mPrefix[0x114];
    be<u32> mSetPlayerPosAndAngle;
};
struct BstCameraProcess {
    u8 mPrefix[0x248];
    dCamera_c mCamera;
};
struct BstCameraInfo {
    gptr<BstCameraProcess> mpCamera;
    u8 mTail[0x30];
};
struct BstCameraPlay {
    u8 mPrefix[0x51D0];
    dEvt_control_c mEvent;
    u8 mBeforeVibration[0x599C - 0x52C4];
    u8 mVibration[1];
    u8 mBeforeParticle[0x5AB0 - 0x599D];
    gptr<void> mpParticle;
    u8 mBeforeCamera[0x5AF8 - 0x5AB4];
    BstCameraInfo mCameraInfo;
    BstCameraInfo& cameraInfo(s32 id) { return (&mCameraInfo)[id]; }
    gptr<BstCameraPlayer> mpPlayer;
    be<s8> mPlayerCameraId;
};
struct BstCameraAudio {
    u8 mPrefix[0x1E44];
    be<u8> mBstHori;
};
struct BstCameraSave {
    u8 mPrefix[0x798];
    u8 mStage[1];
};
struct BstCameraTune {
    u8 mPrefix[8];
    be<f32> mFloat[30];
    be<s16> mShort[16];
    u8 mBeforeIntro[0x5B0 - 0xA0];
    be<f32> mIntroCenterX, mIntroCenterY;
    u8 mBeforeDefeat[0x10];
    be<f32> mDefeatCenterStep, mDefeatEyeStep;
};
struct BstCameraHands {
    gptr<bst_class> mHand[2];
};
struct BstCameraSmoke {
    be<u16> mId[3];
};
struct BstCameraArchive {
    gptr<char> mpName;
    be<u32> mVtable;
};
struct BstCameraTrig {
    struct Entry {
        be<f32> mSin, mCos;
    };
    Entry mEntries[8192];
};
WWHD_OFFSET(BstCameraPlayer, mDemoOriginal, 0x420);
WWHD_OFFSET(BstCameraPlayer, mDemoParam, 0x428);
WWHD_OFFSET(BstCameraPlayer, mDemoMode, 0x430);
WWHD_OFFSET(BstCameraPlay, mEvent, 0x51D0);
WWHD_OFFSET(BstCameraPlay, mVibration, 0x599C);
WWHD_OFFSET(BstCameraPlay, mpParticle, 0x5AB0);
WWHD_OFFSET(BstCameraPlay, mCameraInfo, 0x5AF8);
WWHD_OFFSET(BstCameraPlay, mpPlayer, 0x5B2C);
WWHD_OFFSET(BstCameraTune, mFloat, 8);
WWHD_OFFSET(BstCameraTune, mShort, 0x80);
WWHD_OFFSET(BstCameraTune, mIntroCenterX, 0x5B0);
WWHD_OFFSET(BstCameraTune, mDefeatCenterStep, 0x5C8);

enum BstCameraState_e {
    CAMERA_IDLE = 0,
    CAMERA_NOSE_START = 1,
    CAMERA_NOSE = 2,
    CAMERA_INTRO_START = 10,
    CAMERA_INTRO = 11,
    CAMERA_LEFT_HAND = 12,
    CAMERA_RIGHT_HAND = 13,
    CAMERA_HEAD = 14,
    CAMERA_BATTLE = 15,
    CAMERA_DEFEAT_START = 50,
    CAMERA_DEFEAT_INTRO = 51,
    CAMERA_LOOK_UP = 52,
    CAMERA_PLAYER = 53,
    CAMERA_RETRACT = 54,
    CAMERA_RETURN_HOME = 55,
    CAMERA_DROP_ITEM = 56,
    CAMERA_FOLLOW_ITEM = 57
};
enum BstCameraResource_e {
    dRes_INDEX_BST_BRK_UDEL_e = 0x4A,
    dRes_INDEX_BST_BTK_UDEL_e = 0x64,
    dRes_INDEX_BST_BRK_UDER_e = 0x4D,
    dRes_INDEX_BST_BTK_UDER_e = 0x67,
    dRes_INDEX_BST_BRK_MUNE_e = 0x42,
    dRes_INDEX_BST_BTK_MUNE_e = 0x5C
};
enum BstCameraJoint_e { BST_JNT_HEAD_e = 0, BST_JNT_HANAR_e = 7 };
WWHD_SIZE(BstCameraPlayer, 0x434);
WWHD_SIZE(BstCameraArchive, 8);
WWHD_SIZE(BstCameraHands, 8);
WWHD_SIZE(BstCameraTune, 0x5D0);
WWHD_SIZE(BstCameraInfo, 0x34);
WWHD_OFFSET(BstCameraProcess, mCamera, 0x248);
WWHD_OFFSET(BstCameraAudio, mBstHori, 0x1E44);
WWHD_OFFSET(BstCameraSave, mStage, 0x798);

// HD: standalone camera specs omit the owner source; this is the same TU vtable.
constexpr u32 BstCameraSafeStringVtable = 0x1000B714;
