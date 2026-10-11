/* Door checks: tww dDoor_info_c fields move by +0x11C in HD.
 */
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/JSystem/JUTAssert.h"

struct dDoor_info_c : fopAc_ac_c {
    u32 getFRoomNo();
    u32 getBRoomNo();
    BOOL adjoinPlayer();
    u32 getViewRoomNo();
    BOOL frontCheckOld();
    u32 frontCheck();
    s32 checkExecute();
    void startDemoProc();
    BOOL checkArea(f32 lateral, f32 forward, f32 distance);
    s32 drawCheck_local();
    BOOL drawCheck(s32 mode);

    /* 0x3AC */ be<s8> mRoomNo2;
    /* 0x3AD */ be<u8> mFromRoomNo;
    /* 0x3AE */ be<u8> mToRoomNo;
    /* 0x3AF */ u8 _3AF;
    /* 0x3B0 */ cXyz mAngleVec;
    /* 0x3BC */ be<u8> mFrontCheck;
    /* 0x3BD */ be<u8> m2A1;
    /* 0x3BE */ be<s16> mEventIdx[12];
    /* 0x3D6 */ be<u8> mToolId[12];
    /* 0x3E2 */ be<u8> m2C6;
    /* 0x3E3 */ be<u8> m2C7;
    /* 0x3E4 */ be<s32> mStaffId;
    /* 0x3E8 */ be<s8> mRoomNo;
    /* 0x3E9 */ u8 _3E9[3];
};
WWHD_OFFSET(dDoor_info_c, mRoomNo2, 0x3AC);
WWHD_OFFSET(dDoor_info_c, mAngleVec, 0x3B0);
WWHD_OFFSET(dDoor_info_c, mFrontCheck, 0x3BC);
WWHD_OFFSET(dDoor_info_c, mStaffId, 0x3E4);
WWHD_SIZE(dDoor_info_c, 0x3EC);
namespace door_checks {
struct cSAngle {
    be<s16> mAngle;
    u32 Abs(); // HD: retain the verified full-width return register.
};
struct cSGlobe {
    be<f32> mRadius;
    cSAngle mAzimuth, mInclination;
};
WWHD_SIZE(cSAngle, 2);
WWHD_SIZE(cSGlobe, 8);
struct camera_view {
    u8 _000[0xDC];
    cXyz mEye;
};
struct play_view {
    u8 _0000[0x5FA4];
    gptr<camera_view> mView;
};
struct roomControl_view { u32 checkRoomDisp(u32 room); };
}
inline door_checks::camera_view* dDoor_getView() {
    return reinterpret_cast<door_checks::play_view*>(dComIfGp_get())->mView;
}
inline door_checks::roomControl_view* dDoor_getRoomControl() {
    return reinterpret_cast<door_checks::roomControl_view*>(
        &reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mRoomCtrl);
}
void cSGlobe_fromVector(door_checks::cSGlobe* globe, cXyz* vector);
void cSAngle_init(door_checks::cSAngle* angle);
void cSAngle_sub(door_checks::cSAngle* angle, door_checks::cSAngle* result, s32 value);
void cXyz_normalize(cXyz* vector, cXyz* result);
