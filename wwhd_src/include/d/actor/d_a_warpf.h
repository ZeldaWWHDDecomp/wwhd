/* Boss warp flower. Private HD layout. */
#pragma once
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_item.h"
#include "d/d_level_se.h"
#include "gc_api/JSystem/JPAEmitter.h"
template<class T> inline T* Warpf_view(u32 address) {return gabi::at<T>(address);}
struct WarpfStageInfo {
    u8 mPadding0[9];
    be<u8> mSaveTable;
};
WWHD_OFFSET(WarpfStageInfo, mSaveTable, 9);
struct WarpfEmitter : JPABaseEmitter {
    u8 mPadding70[0x1D7];
    be<u8> mGlobalAlpha;
    u8 mPadding248[12];
    be<u32> mFlags;
};
WWHD_OFFSET(WarpfEmitter, mGlobalAlpha, 0x247);
WWHD_OFFSET(WarpfEmitter, mFlags, 0x254);
WWHD_SIZE(WarpfEmitter, 0x258);
struct WarpfBckAnimation {
    u8 mFrameCtrl[0x10];
    be<u32> mVtable;
    u8 mMatrix[0x34];
    be<u32> mMatrixVtable;
    u8 mPadding4C[12];
    be<u32> mMatrixAnm;
    u8 mPadding5C[0x20];
    be<u32> mFrameData, mBlend, mJoint, mModel;
};
WWHD_OFFSET(WarpfBckAnimation, mVtable, 0x10);
WWHD_OFFSET(WarpfBckAnimation, mMatrix, 0x14);
WWHD_OFFSET(WarpfBckAnimation, mMatrixVtable, 0x48);
WWHD_OFFSET(WarpfBckAnimation, mMatrixAnm, 0x58);
WWHD_OFFSET(WarpfBckAnimation, mFrameData, 0x7C);
WWHD_OFFSET(WarpfBckAnimation, mBlend, 0x80);
WWHD_OFFSET(WarpfBckAnimation, mJoint, 0x84);
WWHD_OFFSET(WarpfBckAnimation, mModel, 0x88);
WWHD_SIZE(WarpfBckAnimation, 0x8C);
struct WarpfPlayerVtable {
    u8 mPadding0[0x4C];
    be<u32> mCheckFly;
};
WWHD_OFFSET(WarpfPlayerVtable, mCheckFly, 0x4C);
struct daWarpf_c : fopAc_ac_c {
    using InitFunc = void(daWarpf_c::*)(int);
    using ActionFunc = BOOL(daWarpf_c::*)(int);
    BOOL CreateHeap();
    bool _delete();
    BOOL checkEndDemo();
    void onEndDemo() {}
    cPhs_State CreateInit();
    cPhs_State _create();
    bool _execute();
    void eventOrder();
    void checkOrder();
    void demo_proc();
    void initWait(int);
    BOOL actWait(int);
    void initWarpStart(int);
    BOOL actWarpStart(int);
    void initWarpMode_1(int);
    BOOL actWarpMode_1(int);
    void initWarpMode_2(int);
    BOOL actWarpMode_2(int);
    void initWarpMode_3(int);
    BOOL actWarpMode_3(int);
    void initEndWait(int);
    BOOL actEndWait(int);
    BOOL check_warp_event();
    f32 get_distance();
    f32 get_earth_pos();
    void set_effect();
    void set_effect_wind00();
    u32 get_angle_wind01();
    void anim_play();
    void setEndAnim();
    void set_se();
    void set_mtx();
    bool _draw();

    const char* arcname();

    /* 0x3AC */ be<u32> mpBstWarpEmitter;  /* was m290: set_effect 024D6940: on the TOTG stage the emitter of ID_AK_SN_BSTWARP00 + 3, created once; _delete 024D717C invalidates it (tww the same); JPABaseEmitter* */
    /* 0x3B0 */ be<u32> mpKirakiraEmitter;  /* was m294: set_effect 024D6940: on the boss stages the emitter of ID_AK_SN_BOSSWARPKIRAKIRA00, created once; initWait 024D7BCC / _delete 024D717C invalidate it (tww the same) */
    /* 0x3B4 */ be<u32> mpLightEmitter;  /* was m298: set_effect 024D6940: on ET / WT before the end demo the emitter of ID_IT_SN_SHINDENTF_LIGHT00; initWait 024D7BCC / _delete 024D717C invalidate it (tww the same) */
    /* 0x3B8 */ be<u32> mpStarEmitter;  /* was m29C: set_effect 024D6940: on ET / WT the emitter of ID_IT_SN_SHINDENTF_STAR00 (or ID_IT_SN_DEMO29_STAR00 after the end demo); initWait 024D7BCC / _delete 024D717C invalidate it (tww the same) */
    /* 0x3BC */ request_of_phase_process_class mPhase;
    /* 0x3C4 */ gptr<J3DModel> mpModel;  /* was m2A8: CreateHeap 024D6114 creates the main model (YSBWP00 / GTFGLOW00 / GDEMO29_A00); set_mtx 024D7258 places it, _draw 024D73B4 draws it with the brk / bck / btk (tww the same) */
    /* 0x3C8 */ be<u32> mpBrkAnm;  /* was m2AC: CreateHeap 024D6114: the main model's brk (YSBWP00 / GTFGLOW00 / GDEMO29_A01); anim_play 024D7A54 plays it, setEndAnim 024D6D94 sets its end frame, _draw 024D73B4 enters it (tww the same); mDoExt_brkAnm* */
    /* 0x3CC */ be<u32> mpBckAnm;  /* was m2B0: CreateHeap 024D6114: the TOTG model's bck (YSBWP00); anim_play 024D7A54 plays it, setEndAnim 024D6D94 sets its end frame, _draw 024D73B4 enters it (tww the same); mDoExt_bckAnm* */
    /* 0x3D0 */ be<u32> mpBtkAnm;  /* was m2B4: CreateHeap 024D6114: the TOTG model's btk (YSBWP00); anim_play 024D7A54 plays it, setEndAnim 024D6D94 sets its end frame, _draw 024D73B4 enters it (tww the same); mDoExt_btkAnm* */
    /* 0x3D4 */ be<u32> mpBrkAnm1;  /* was m2B8: CreateHeap 024D6114: the second brk (GTFGLOW01, loop, rate 0); set_effect 024D6940 / anim_play 024D7A54 (from frame 120 of mFrameCount) start it, _draw 024D73B4 enters it while its rate > 0 (tww the same); mDoExt_brkAnm* */
    /* 0x3D8 */ be<u32> mpBrkAnm2;  /* was m2BC: CreateHeap 024D6114: the third brk (GTFGLOW02, rate 0), NULL after the end demo; never played or entered in WWHD or tww; mDoExt_brkAnm* */
    /* 0x3DC */ gptr<J3DModel> mpEndModel;  /* was m2C0: CreateHeap 024D6114: on ET / WT after the end demo the second model GDEMO29_B00, else NULL; set_mtx 024D7258 places it, _draw 024D73B4 draws it (tww the same) */
    /* 0x3E0 */ be<s32> mStaffID;
    /* 0x3E4 */ be<s32> mWarpTimer;
    /* 0x3E8 */ u8 m2CC[4];
    /* 0x3EC */ be<s32> mOrderRequest;  /* was m2D0: checkOrder 024D7628 sets 1 when check_warp_event finds the player in warp range and clears it once the event starts; eventOrder 024D79F4 orders the event mEventIdx while it is 1 (tww the same) */
    /* 0x3F0 */ u8 m2D4[2];
    /* 0x3F2 */ be<s16> mEventIdx;  /* was m2D6: CreateInit 024D6EEC: event index of WARP_WIND (WARP_WIND_AFTER after the end demo); eventOrder 024D79F4 orders it, checkOrder 024D7628 startCheck / endCheck it (tww the same) */
    /* 0x3F4 */ be<s32> mSeState;  /* was m2D8: set_se 024D78D0: 0 plays the appear SE (JA_SE_CM_BST_WARP_BEAM / JA_SE_OBJ_BOSS_WARP_APPEAR) once and sets 1 (tww the same) */
    /* 0x3F8 */ be<s32> mStageNo;
    /* 0x3FC */ be<s32> mSeTimer;  /* was m2E0: set_se 024D78D0 counts it up to 20000; on TOTG the level SE mpLevelSe starts only from 45 (tww the same) */
    /* 0x400 */ be<s32> mFrameCount;  /* was m2E4: _execute 024D7B0C adds 1 each frame; actWait 024D7C98 / set_effect 024D6940 spawn wind every N frames of it, anim_play 024D7A54 starts mpBrkAnm1 from 120 (tww the same) */
    /* 0x404 */ be<u32> mpLevelSe;  /* was m2E8: _create 024D6FB8: the dLevelSe_c made by fopKyM_fastCreate(fpcNm_LEVEL_SE_e, ..), reverb set on it; set_se 024D78D0 dLevelSe_seStart it (tww the same); dLevelSe_c* */
    /* 0x408 */ cXyz mPlayerPos;  /* was m2EC: initWarpStart 024D7CE4 copies the player's current.pos; actWarpMode_1 024D7D38 / _2 024D7E64 / _3 024D7F68 put the wind effect at its y + 100 / 120 / 140 (tww the same) */
};
WWHD_OFFSET(daWarpf_c, mPhase, 0x3BC);
WWHD_OFFSET(daWarpf_c, mStaffID, 0x3E0);
WWHD_OFFSET(daWarpf_c, mEventIdx, 0x3F2);
WWHD_OFFSET(daWarpf_c, mStageNo, 0x3F8);
WWHD_OFFSET(daWarpf_c, mPlayerPos, 0x408);
WWHD_SIZE(daWarpf_c, 0x414);


u32 Warpf_fastCreate(s16, u32, cXyz*, cXyz*, void*);
u32 Warpf_brkConstruct(u32);
BOOL Warpf_brkInit(u32, J3DModelData*, void*, bool, s32, f32, s16, s16, bool, s32);
void Warpf_brkEntryModel(u32, J3DModel*, f32);
u32 Warpf_btkConstruct(u32);
void Warpf_frameConstruct(u32, s32);
void Warpf_matrixConstruct(void*);
s32 Warpf_calcTimer(be<s32>*);
f32 Warpf_groundCheck(cXyz*);
s16 Warpf_cameraAngleX(u32);
s16 Warpf_cameraAngleY(u32);
void Warpf_actorDestruct(daWarpf_c*, s32);

struct WarpfInitEntry {
    gabi::ptmf<daWarpf_c::InitFunc> mFunction;
};
struct WarpfActionEntry {
    gabi::ptmf<daWarpf_c::ActionFunc> mFunction;
};
WWHD_SIZE(WarpfInitEntry, 8);
WWHD_SIZE(WarpfActionEntry, 8);
struct WarpfTU {
    u32 mBckVtable;
    u32 mActorVtable;
    u32 mBaseAnmVtable;
    u32 mMatrixVtable;
    u32 mHeapCallback;
    u32 mHeaderStorage;
    u32 mHeaderRecords;
};

namespace daWarpf_prm {
inline u32 getSetType(daWarpf_c* actor) {return fopAcM_GetParam(actor) >> 0x1C;}
}

WWHD_OFFSET(daWarpf_c, mpBstWarpEmitter, 0x3AC);
WWHD_OFFSET(daWarpf_c, mpKirakiraEmitter, 0x3B0);
WWHD_OFFSET(daWarpf_c, mpLightEmitter, 0x3B4);
WWHD_OFFSET(daWarpf_c, mpStarEmitter, 0x3B8);
WWHD_OFFSET(daWarpf_c, mpModel, 0x3C4);
WWHD_OFFSET(daWarpf_c, mpBrkAnm, 0x3C8);
WWHD_OFFSET(daWarpf_c, mpBckAnm, 0x3CC);
WWHD_OFFSET(daWarpf_c, mpBtkAnm, 0x3D0);
WWHD_OFFSET(daWarpf_c, mpBrkAnm1, 0x3D4);
WWHD_OFFSET(daWarpf_c, mpBrkAnm2, 0x3D8);
WWHD_OFFSET(daWarpf_c, mpEndModel, 0x3DC);
WWHD_OFFSET(daWarpf_c, mWarpTimer, 0x3E4);
WWHD_OFFSET(daWarpf_c, mOrderRequest, 0x3EC);
WWHD_OFFSET(daWarpf_c, mSeState, 0x3F4);
WWHD_OFFSET(daWarpf_c, mSeTimer, 0x3FC);
WWHD_OFFSET(daWarpf_c, mFrameCount, 0x400);
WWHD_OFFSET(daWarpf_c, mpLevelSe, 0x404);
