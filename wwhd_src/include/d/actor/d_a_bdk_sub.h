/* Helmaroc support routines: private HD views of joint data and the camera.
 * The actor layout remains in d_a_bdk.h; its tww member names are unchanged. */
#pragma once
#include "d/actor/d_a_bdk.h"
#include "gc_api/d/d_com_inf_game.h"
struct BdkSubJointData {
    u8 pad0[4];
    be<u16> mJntNo;
};
WWHD_OFFSET(BdkSubJointData, mJntNo, 4);
WWHD_SIZE(BdkSubJointData, 6);
struct BdkSubModelUserArea {
    u8 pad0[0xB8];
    gptr<bdk_class> mUserArea;
};
WWHD_OFFSET(BdkSubModelUserArea, mUserArea, 0xB8);
WWHD_SIZE(BdkSubModelUserArea, 0xBC);
struct BdkSubCamera {
    u8 pad0[0xDC];
    cXyz mEye;
};
WWHD_OFFSET(BdkSubCamera, mEye, 0xDC);
WWHD_SIZE(BdkSubCamera, 0xE8);
// HD: transfer float storage as words to preserve signalling NaN bits.
inline be<u32> &bdkSubBits(be<f32> &value) {
    return reinterpret_cast<be<u32> &>(value);
}
inline bdk_class *bdkSubActor(J3DModel_bdk *model) {
    return reinterpret_cast<BdkSubModelUserArea *>(model)->mUserArea;
}
inline s32 bdkSubJointNo(J3DJoint *joint) {
    return reinterpret_cast<BdkSubJointData *>(joint)->mJntNo;
}
inline BdkSubCamera *bdkSubCamera() {
    return reinterpret_cast<BdkSubCamera *>(dComIfGp_getCamera(0));
}
enum {
    DK_JNT_J_DK_KUBI1_e = 0x17,
    DK_JNT_J_DK_TOSAKA_A1_e = 0x1F,
    DK_JNT_J_DK_TOSAKA_B1_e = 0x23,
    DK_JNT_J_DK_TOSAKA_B2_e = 0x24,
    DK_JNT_J_DK_O_LA2_e = 0x3A
};
enum { JA_SE_CM_DK_WIND = 0x5076 };
