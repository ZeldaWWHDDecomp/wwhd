#pragma once
#include "d/actor/d_a_bdk.h"
#include "d/actor/d_a_player.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
struct BdkDemoCameraBody {};
// HD: camera_process_class view prefix; mCamera begins at 0x248.
struct BdkDemoCamera {
    u8 prefix[0xDC];
    cXyz mEye;
    cXyz mCenter;
};
inline BdkDemoCameraBody *BdkDemo_cameraBody(BdkDemoCamera *camera) {
    // Add in guest space first: the original also accepts a zero camera address.
    return gabi::at<BdkDemoCameraBody>(gabi::ea(camera) + 0x248);
}
struct BdkDemoPlayerVtable {
    u8 prefix[0x114];
    be<u32> setPlayerPosAndAngle;
};
inline void bdk_changeOriginalDemo(daPy_py_c *player) {
    player->mDemo.mParam0 = 0;
    player->mDemo.mDemoType = daPy_demo_c::TYPE_ORIGINAL_e;
}
inline void bdk_changeDemoMode(daPy_py_c *player, u32 mode) { player->mDemo.mDemoMode = mode; }
inline void bdk_changeDemoParam0(daPy_py_c *player, s32 value) { player->mDemo.mParam0 = value; }
inline void bdk_setPlayerPosAndAngle(daPy_py_c *player, cXyz *pos, s16 angle) {
    auto vtable = gabi::at<BdkDemoPlayerVtable>(player->__vtbl);
    gabi::call_ptr(vtable->setPlayerPosAndAngle, player, pos, angle);
}
// HD: preserve integer-copy bit patterns and each interleaved load/store.
inline void BdkDemo_copyFloat(be<f32> &dst, const be<f32> &src) {
    reinterpret_cast<be<u32> &>(dst) = reinterpret_cast<const be<u32> &>(src);
}
inline void BdkDemo_copyVector(cXyz *dst, const cXyz *src) {
    BdkDemo_copyFloat(dst->x, src->x);
    BdkDemo_copyFloat(dst->y, src->y);
    BdkDemo_copyFloat(dst->z, src->z);
}
namespace BdkDemoSearch {
enum : fpcM_SearchFunc { obj_s_sub = 0x020651B8, ep_delete_sub = 0x02064D9C };
}
// HD: separately emitted debug strings form one contiguous, read-only view.
struct BdkDemoReports {
    char mainCount[0x14];
    char subCount[0x14];
    char demoMode[0x14];
};
inline BdkDemoReports &BdkDemo_reports() { return *gabi::at<BdkDemoReports>(0x10007F88); }
inline be<u8> &BdkDemo_globalFlag() { return *gabi::at<be<u8>>(0x101F4825); }
void dCamera_Stop_bdk(BdkDemoCameraBody *);
void dCamera_Start_bdk(BdkDemoCameraBody *);
void dCamera_SetTrimSize_bdk(BdkDemoCameraBody *, s32);
void dCamera_Set_bdk(BdkDemoCameraBody *, cXyz *, cXyz *, s16, f32);
void dCamera_Reset_bdk(BdkDemoCameraBody *, cXyz *, cXyz *);
BOOL fopAcM_orderPotentialEvent_bdk(fopAc_ac_c *, u16, u16, u16);
void mDoAud_bgmStreamPrepare_bdk(u32);
void mDoAud_bgmStreamPlay_bdk();
void mDoAud_seStart_simple_bdk(u32);
void BdkDemo_report(s32, s32, const char *, s32);
void BdkDemo_smokeEnd(dPa_smokeEcallBack_bdk *);
enum BdkDemoConstants {
    ACTION_FLY = 0,
    JA_STRM_DK_START = 0xC0000008,
    ID_IT_ST_DK_FUTATOJI_SMOKE_A00 = 0xA14F,
    ID_IT_ST_DK_FUTATOJI_SMOKE_B00 = 0xA150,
    ID_IT_SN_DK_FUTATOJI_ROCK00 = 0x814E,
    JA_SE_ATM_MJT_JINARI = 0x7020,
    dRes_INDEX_BDK_BCK_FLY3_e = 0x2C,
    dRes_INDEX_BDK_BAS_FLY3_e = 0xD,
    JA_SE_ATM_MJT_SHUTTER_END = 0x7822,
    ID_IT_ST_DK_FUTATOJI_SMOKE_C00 = 0xA151,
    AT_TYPE_WIND = 0x200000,
    JA_BGM_DK_BATTLE = 0x80000014,
    ACTION_START = 0xF,
    JA_STRM_DK_CLEAR = 0xC000001A,
    JA_SE_OBJ_TOGE_IN = 0x6977,
    JA_SE_ATM_MJT_SHUTTER = 0x7021,
};

WWHD_OFFSET(BdkDemoCamera, mEye, 0xDC);
WWHD_OFFSET(BdkDemoCamera, mCenter, 0xE8);
WWHD_OFFSET(BdkDemoPlayerVtable, setPlayerPosAndAngle, 0x114);
// HD: canonical onStageBossDemo/onStageBossEnemy setters are absent from the shared d_com_inf_game accessors.
void BdkDemo_onDungeonItem(dSv_memBit_c *, s32);
