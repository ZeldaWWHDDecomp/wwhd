/* Sliding double shutter, WWHD layout.
 * HD: fopAc_ac_c grows by 0x11C; the actor fields retain their GameCube names. */
#pragma once
#include "gc_api/gc_api.h"

enum { JA_SE_OBJ_B_SHUTTER_OPEN = 0x69B4, JA_SE_OBJ_B_SHUTTER_STOP = 0x69B5 };
enum { STAGE_TOTG = 5 };
enum { dRes_INDEX_HTOBI1_BDL_HTOBI1_e = 4, dRes_INDEX_HTOBI2_BDL_HTOBI2_e = 4,
       dRes_INDEX_HTOBI1_DZB_HTOBI1_e = 7, dRes_INDEX_HTOBI2_DZB_HTOBI2_e = 7 };

// Private prefix: the save-table number occupies bits 1..7 of this stage-info byte.
struct ShutterStageInfo {
    u8 _00[9];
    be<u8> mSaveTableAndFlags;
};
WWHD_OFFSET(ShutterStageInfo, mSaveTableAndFlags, 9);
WWHD_SIZE(ShutterStageInfo, 10);
inline s32 shutter_getSaveTable(void* info) {
    return (((ShutterStageInfo*)info)->mSaveTableAndFlags >> 1) & 0x7F;
}

// GHS captures the complete scale before writing the model, and copies matrices by words.
struct ShutterModel {
    u8 _00[0xBC];
    cXyz mBaseScale;
    Mtx34 mBaseTransformMtx;
    void setBaseScale(const cXyz& scale) {
        f32 x = scale.x, y = scale.y, z = scale.z;
        mBaseScale.x = x; mBaseScale.y = y; mBaseScale.z = z;
    }
    void setBaseTRMtx(const Mtx34* matrix) { mtx_copy(&mBaseTransformMtx, matrix); }
};
WWHD_OFFSET(ShutterModel, mBaseScale, 0xBC);
WWHD_OFFSET(ShutterModel, mBaseTransformMtx, 0xC8);
WWHD_SIZE(ShutterModel, 0xF8);

f32 cLib_addCalc(be<f32>*, f32, f32, f32, f32);
u8 shutter_calcTimer(be<u8>*);

class daShutter_c : public fopAc_ac_c {
public:
    enum : u32 { __vt = 0x1003CA3C }; // HD: this actor's vtable
    daShutter_c();
    bool _delete();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State _create();
    void set_mtx();
    bool _execute();
    void shutter_move();
    void demo();
    bool _draw();

    /* 0x3AC */ request_of_phase_process_class mPhs; /* GameCube 0x290 */
    /* 0x3B4 */ gptr<J3DModel> mpModel[2];
    /* 0x3BC */ gptr<dBgW> mdBgW[2];
    /* 0x3C4 */ Mtx34 mMtx[2];
    /* 0x424 */ cXyz mcXyz[2];
    /* 0x43C */ be<s32> mEventOrder;  /* was field_0x320: demo 02484B10: 1 (open) / 2 (close) when the switch changes after mFrameTimer; orders mOpenEventIdx / mCloseEventIdx while set and clears it once that event starts (tww the same) */
    /* 0x440 */ be<s32> mSwitchNo;
    /* 0x444 */ be<s32> field_0x328;
    /* 0x448 */ be<s32> mStaffId;
    /* 0x44C */ be<s16> mOpenEventIdx;
    /* 0x44E */ be<s16> mCloseEventIdx;
    /* 0x450 */ be<u8> field_0x334;
    /* 0x451 */ be<u8> mType;
    /* 0x452 */ be<u8> mbIsSwitch;
    /* 0x453 */ be<s8> mFrameTimer;
    /* 0x454 */ be<u8> mTimer;
    /* 0x455 */ be<u8> mActionStarted;  /* was field_0x339: shutter_move 0248478C: set on the first frame of ACT_OPEN (with the TOTG open sound) and ACT_CLOSE, cleared by ACT_WAIT02 when its timer ends (tww the same) */
    /* 0x456 */ be<u8> mOpenFrame;  /* was field_0x33A: shutter_move 0248478C: ACT_WAIT clears it, ACT_OPEN counts it up; at 75 the TOTG shutter plays the stop sound and a shock (tww the same) */
    /* 0x457 */ be<u8> field_0x33B;
};
WWHD_OFFSET(daShutter_c, mpModel, 0x3B4);
WWHD_OFFSET(daShutter_c, mMtx, 0x3C4);
WWHD_OFFSET(daShutter_c, mcXyz, 0x424);
WWHD_OFFSET(daShutter_c, mEventOrder, 0x43C);
WWHD_OFFSET(daShutter_c, mType, 0x451);
WWHD_SIZE(daShutter_c, 0x458);

namespace daShutter_prm {
inline u8 getSwitchNo(daShutter_c* i_this) { return (fopAcM_GetParam(i_this) >> 0) & 0xFF; }
inline u8 getType(daShutter_c* i_this) { return (fopAcM_GetParam(i_this) >> 8) & 0x0F; }
}  // namespace daShutter_prm

