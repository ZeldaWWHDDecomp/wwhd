/* Searchlights: verified WWHD layout.
 * HD: actor base 0x3AC, GHS capsule/status arrays and smoke callback retain their exact sizes.
 * GC leaves most actor fields undecompiled; known member/function names are retained and HD fields named by use. */
#pragma once
#include "gc_api/gc_api.h"
#include "d/d_path.h"
#include "d/actor/d_a_player.h"

namespace daObj_Search {
struct Act_c;
struct Bgc_c : dBgS_LinChk {
    void wall_pos(const Act_c* a, cXyz* start, cXyz* end, bool* hit, f32* dot);
    /* 0x6C */ cXyz mWallPos;
};
WWHD_OFFSET(Bgc_c, mWallPos, 0x6C);
WWHD_SIZE(Bgc_c, 0x78);
struct Act_c : fopAc_ac_c {
    enum Proc_e { PROC_INIT_e = 0, PROC_EXEC_e = 1 };
    enum Mode_e {
        MODE_SEARCH_RND_e = 0,
        MODE_SEARCH_PATH_e = 1,
        MODE_STOP_e = 2,
        MODE_TO_SEARCH_e = 3,
        MODE_TO_STOP_e = 4,
        MODE_FIND_e = 5,
        MODE_FIND_2ND_e = 6,
        MODE_SEARCH_BDK_e = 7
    };
    s32 searchCreateHeap();
    s32 beamCreateHeap(s32 index);
    s32 _createHeap();
    void SetArgData();
    s32 set_path_info();
    bool is_path_info();
    bool isSecond(u32 mode);
    void modeStopInit();
    void modeProc(s32 operation, s32 mode);
    void modeSearchPathInit();
    s32 initialMode(u32 kind);
    void check_bk_control();
    void bg_check();
    void modeToStopInit();
    void modeFindInit();
    void modeFind2ndInit();
    void modeSearchRndInit();
    void modeToSearchInit();
    void modeSearchBdkInit();
    void modeStop();
    void set_moveBG_mtx_base();
    void set_mtx_base();
    void set_moveBG_mtx_light_A();
    void set_moveBG_mtx_light_B();
    s32 _delete();
    void modeSearchRnd();
    void modeToSearch();
    void _nodeControl(void* node, void* model);
    s32 _execute();
    void smoke_set(f32 speed, s32 duration);
    void CreateInit();
    void set_mtx_light_A();
    void set_mtx_light_B();
    void modeFind2nd();
    s32 _draw();
    void modeToStop();
    void modeFind();
    s32 player_check();
    void modeSearchPath();
    void modeSearchBdk();
    s32 _create();

    /* 0x3AC */ be<s32> mMode;
    /* 0x3B0 */ Bgc_c mBgc;
    /* 0x428 */ dCcD_Cps mCps[2];
    /* 0x698 */ dCcD_Stts mStts[2];
    /* 0x710 */ gptr<J3DModel> mpSearchModel;
    /* 0x714 */ gptr<J3DModel> mpBeamModel[2];
    /* 0x71C */ cXyz mModelLightPos;
    /* 0x728 */ cXyz mInitialTarget[2];
    /* 0x740 */ cXyz mBeamStart[2];
    /* 0x758 */ cXyz mBeamEnd[2];
    /* 0x770 */ be<f32> mBeamLength[2];
    /* 0x778 */ be<f32> mBeamMaxLength[2];
    /* 0x780 */ be<u8> mBkControl;
    u8 _pad781[0x3];
    /* 0x784 */ gptr<dBgW> mpBeamBackground[2];
    /* 0x78C */ gptr<dBgW> mpBaseBackground;
    /* 0x790 */ Mtx34 mBeamMatrix[2];
    /* 0x7F0 */ Mtx34 mBaseMatrix;
    u8 _pad820[0x2];
    /* 0x822 */ csXyz mBeamAngle[2];
    u8 _pad82E[0xA];
    /* 0x838 */ Mtx34 mBeamScaleMatrix[2];
    /* 0x898 */ be<u8> mWallHit[2];
    /* 0x89A */ be<s16> mAlpha;
    /* 0x89C */ cXyz mSpotPos;
    /* 0x8A8 */ be<s16> mSpotColor[3];
    u8 _pad8AE[0x2];
    /* 0x8B0 */ be<f32> mSpotWidth;
    /* 0x8B4 */ be<f32> mSpotFalloff;
    u8 _pad8B8[0x4];
    /* 0x8BC */ be<f32> mSpotOpacity;
    /* 0x8C0 */ be<s32> mFindTimer;
    /* 0x8C4 */ be<s32> mFrame;
    /* 0x8C8 */ be<s16> mGearAngle[3];
    /* 0x8CE */ be<s16> mTopAngle;
    /* 0x8D0 */ be<s16> mPreviousYaw;
    /* 0x8D2 */ be<s16> mPreviousPitch;
    /* 0x8D4 */ be<s16> mPreviousRoll;
    /* 0x8D6 */ be<u8> mBeamCull[2];
    /* 0x8D8 */ be<u8> mSearchCull;
    u8 _pad8D9[0x3];
    /* 0x8DC */ cXyz mTrackingPos;
    /* 0x8E8 */ cXyz mTrackingTarget;
    /* 0x8F4 */ be<f32> mTrackingRadius;
    /* 0x8F8 */ be<f32> mTrackingSpeed;
    u8 _pad8FC[0x2];
    /* 0x8FE */ be<s16> mTrackingRate;
    /* 0x900 */ be<s16> mPlayerYaw;
    /* 0x902 */ be<u8> mKind;
    u8 _pad903[0x11];
    /* 0x914 */ cXyz mSmokePos;
    /* 0x920 */ csXyz mSmokeAngle;
    u8 _pad926[0x2];
    /* 0x928 */ dPa_smokeEcallBack mSmokeCallback;
    /* 0x948 */ be<s32> mSmokeTimer;
    /* 0x94C */ GXColor mLightColor;
    /* 0x950 */ be<s32> mBeamIndex;
    /* 0x954 */ be<u8> mActive;
    u8 _pad955[0x1];
    /* 0x956 */ be<u8> mType;
    /* 0x957 */ be<u8> mPathId;
    /* 0x958 */ cXyz mPathTarget;
    /* 0x964 */ be<s32> mPathPointIndex;
    /* 0x968 */ gptr<dPath> mpPath;
    /* 0x96C */ be<u32> mChildId;
    /* 0x970 */ be<u32> mChildSwitch;
    /* 0x974 */ request_of_phase_process_class mPhase;
    /* 0x97C */ be<s32> mSwitch;
    /* 0x980 */ be<u8> mDisabled;
    u8 _pad981[0x3];
    /* 0x984 */ dBgS_LinChk mPlayerLine;
    /* 0x9F0 */ be<u8> mControl;
    u8 _pad9F1[0x3];
    /* 0x9F4 */ cXyz mBirdTarget;
    /* 0xA00 */ be<s16> mBirdDirection;
    u8 _padA02[0x2];
    /* 0xA04 */ be<f32> mBirdFanRange;
    /* 0xA08 */ be<f32> mBirdCircleRange;
    /* 0xA0C */ be<s16> mBirdAngle;
    /* 0xA0E */ be<u8> mBirdInside;
    u8 _padA0F[0x1];
    /* 0xA10 */ be<s32> mBirdTimer;
    /* 0xA14 */ cXyz mBirdLastPos;
    /* 0xA20 */ be<s32> mBirdPhase;
    /* 0xA24 */ be<s32> mBirdLastTimer;
};
WWHD_OFFSET(Act_c, mMode, 0x3AC);
WWHD_OFFSET(Act_c, mBgc, 0x3B0);
WWHD_OFFSET(Act_c, mCps, 0x428);
WWHD_OFFSET(Act_c, mStts, 0x698);
WWHD_OFFSET(Act_c, mpSearchModel, 0x710);
WWHD_OFFSET(Act_c, mpBeamModel, 0x714);
WWHD_OFFSET(Act_c, mModelLightPos, 0x71C);
WWHD_OFFSET(Act_c, mInitialTarget, 0x728);
WWHD_OFFSET(Act_c, mBeamStart, 0x740);
WWHD_OFFSET(Act_c, mBeamEnd, 0x758);
WWHD_OFFSET(Act_c, mBeamLength, 0x770);
WWHD_OFFSET(Act_c, mBeamMaxLength, 0x778);
WWHD_OFFSET(Act_c, mBkControl, 0x780);
WWHD_OFFSET(Act_c, mpBeamBackground, 0x784);
WWHD_OFFSET(Act_c, mpBaseBackground, 0x78C);
WWHD_OFFSET(Act_c, mBeamMatrix, 0x790);
WWHD_OFFSET(Act_c, mBaseMatrix, 0x7F0);
WWHD_OFFSET(Act_c, mBeamAngle, 0x822);
WWHD_OFFSET(Act_c, mBeamScaleMatrix, 0x838);
WWHD_OFFSET(Act_c, mWallHit, 0x898);
WWHD_OFFSET(Act_c, mAlpha, 0x89A);
WWHD_OFFSET(Act_c, mSpotPos, 0x89C);
WWHD_OFFSET(Act_c, mSpotColor, 0x8A8);
WWHD_OFFSET(Act_c, mSpotWidth, 0x8B0);
WWHD_OFFSET(Act_c, mSpotFalloff, 0x8B4);
WWHD_OFFSET(Act_c, mSpotOpacity, 0x8BC);
WWHD_OFFSET(Act_c, mFindTimer, 0x8C0);
WWHD_OFFSET(Act_c, mFrame, 0x8C4);
WWHD_OFFSET(Act_c, mGearAngle, 0x8C8);
WWHD_OFFSET(Act_c, mTopAngle, 0x8CE);
WWHD_OFFSET(Act_c, mPreviousYaw, 0x8D0);
WWHD_OFFSET(Act_c, mPreviousPitch, 0x8D2);
WWHD_OFFSET(Act_c, mPreviousRoll, 0x8D4);
WWHD_OFFSET(Act_c, mBeamCull, 0x8D6);
WWHD_OFFSET(Act_c, mSearchCull, 0x8D8);
WWHD_OFFSET(Act_c, mTrackingPos, 0x8DC);
WWHD_OFFSET(Act_c, mTrackingTarget, 0x8E8);
WWHD_OFFSET(Act_c, mTrackingRadius, 0x8F4);
WWHD_OFFSET(Act_c, mTrackingSpeed, 0x8F8);
WWHD_OFFSET(Act_c, mTrackingRate, 0x8FE);
WWHD_OFFSET(Act_c, mPlayerYaw, 0x900);
WWHD_OFFSET(Act_c, mKind, 0x902);
WWHD_OFFSET(Act_c, mSmokePos, 0x914);
WWHD_OFFSET(Act_c, mSmokeAngle, 0x920);
WWHD_OFFSET(Act_c, mSmokeCallback, 0x928);
WWHD_OFFSET(Act_c, mSmokeTimer, 0x948);
WWHD_OFFSET(Act_c, mLightColor, 0x94C);
WWHD_OFFSET(Act_c, mBeamIndex, 0x950);
WWHD_OFFSET(Act_c, mActive, 0x954);
WWHD_OFFSET(Act_c, mType, 0x956);
WWHD_OFFSET(Act_c, mPathId, 0x957);
WWHD_OFFSET(Act_c, mPathTarget, 0x958);
WWHD_OFFSET(Act_c, mPathPointIndex, 0x964);
WWHD_OFFSET(Act_c, mpPath, 0x968);
WWHD_OFFSET(Act_c, mChildId, 0x96C);
WWHD_OFFSET(Act_c, mChildSwitch, 0x970);
WWHD_OFFSET(Act_c, mPhase, 0x974);
WWHD_OFFSET(Act_c, mSwitch, 0x97C);
WWHD_OFFSET(Act_c, mDisabled, 0x980);
WWHD_OFFSET(Act_c, mPlayerLine, 0x984);
WWHD_OFFSET(Act_c, mControl, 0x9F0);
WWHD_OFFSET(Act_c, mBirdTarget, 0x9F4);
WWHD_OFFSET(Act_c, mBirdDirection, 0xA00);
WWHD_OFFSET(Act_c, mBirdFanRange, 0xA04);
WWHD_OFFSET(Act_c, mBirdCircleRange, 0xA08);
WWHD_OFFSET(Act_c, mBirdAngle, 0xA0C);
WWHD_OFFSET(Act_c, mBirdInside, 0xA0E);
WWHD_OFFSET(Act_c, mBirdTimer, 0xA10);
WWHD_OFFSET(Act_c, mBirdLastPos, 0xA14);
WWHD_OFFSET(Act_c, mBirdPhase, 0xA20);
WWHD_OFFSET(Act_c, mBirdLastTimer, 0xA24);
WWHD_SIZE(Act_c, 0xA28);
} // namespace daObj_Search
// HD: preserve integer word copies through vector and float fields.
template <class T> inline be<u32>& search_bits(be<T>& v) { return *gabi::at<be<u32>>(gabi::ea(&v)); }

// HD: unit-local guest ABI adapters retain the original argument and result widths.
void* SearchHD_dRes_control_c_getRes(void*, void*, u32);
void SearchHD_JUTAssertion_showAssert(const char*, u32, const char*);
void* SearchHD_mDoExt_J3DModel__create(void*, u32, u32);
void* SearchHD_dBgW_dBgW(void*);
u32 SearchHD_cBgW_Set(void*, void*, u32, void*);
u32 SearchHD_dLib_pathInfo(void*, u32);
u32 SearchHD_dSv_info_c_isSwitch(void*, u32, u32);
void* SearchHD_fopAcIt_Judge(void*, void*);
void* SearchHD_dComIfGp_getPlay();
f32 SearchHD_fopAcM_searchActorDistance(void*, void*);
void SearchHD_daObj_Search_Act_c_set_moveBG_mtx_light_A(void*);
void SearchHD_daObj_Search_Act_c_set_moveBG_mtx_light_B(void*);
u32 SearchHD_dSv_event_c_isEventBit(void*, u32);
u32 SearchHD_dComIfGp_getReverb(u32);
void SearchHD_mDoAud_seStart(u32, void*, u32, u32);
void SearchHD_dSv_event_c_onEventBit(void*, u32);
void SearchHD_mDoAud_bgmStart(u32);
void SearchHD_dSv_info_c_onSwitch(void*, u32, u32);
u32 SearchHD_daObj_Search_Act_c__create(void*);
u32 SearchHD_daObj_Search_Act_c__delete(void*);
u32 SearchHD_daObj_Search_Act_c__execute(void*);
u32 SearchHD_daObj_Search_Act_c__draw(void*);
void SearchHD_dl(void*);
void* SearchHD_operator_new(u32);
void SearchHD_cCcD_Stts_construct(void*);
void SearchHD_dCcD_GStts_dCcD_GStts(void*);
void SearchHD_dCcD_GObjInf_dCcD_GObjInf(void*);
void SearchHD_cM3dGCps_construct(void*);
void SearchHD_cBgS_LinChk_ct(void*);
void SearchHD_fopAc_ac_c_fopAc_ac_c(void*);
void SearchHD_construct_array(void*, u32, u32, void*);
void SearchHD_dPa_smokeEcallBack_dPa_smokeEcallBack(void*, u32);
void SearchHD_register_global_object(void*);
void SearchHD_initializeHeaderByte(void*);
void SearchHD_initializeHeaderFlag(void*);
void SearchHD_PSMTXTrans(void*, f32, f32, f32);
void SearchHD_mDoMtx_YrotM(void*, u32);
void SearchHD_PSMTXCopy(void*, void*);
void SearchHD_dBgW_Move(void*);
void SearchHD_mDoMtx_XYZrotM(void*, u32, u32, u32);
void SearchHD_mDoMtx_stack_c_transM(f32, f32, f32);
void SearchHD_dBgS_LinChk_Set(void*, void*, void*, const void*);
u32 SearchHD_cBgS_LineCross(void*, void*);
void* SearchHD_cBgS_GetTriPla(void*, u32, u32);
void SearchHD_cXyz___mi(void*, void*, void*);
void SearchHD_cXyz_normZP(void*, void*);
f32 SearchHD_PSVECDotProduct(void*, void*);
void SearchHD_cBgS_LinChk_destruct(void*, u32);
void SearchHD_destroy_arr(void*, u32, u32, void*, u32, u32);
void SearchHD_fopAc_ac_c_fopAc_ac_c(void*, u32);
void SearchHD_daObj_Search_Act_c__nodeControl(void*, void*, void*);
void SearchHD_dKy_plight_cut(void*);
void SearchHD_cBgS_Release(void*, void*);
void SearchHD_dComIfG_resDelete(void*, const char*);
u32 SearchHD_mDoAud_bgmCheck(u32);
void SearchHD_mDoAud_bgmStop(u32, u32);
u32 SearchHD_daObj_Search_Act_c_player_check(void*);
u32 SearchHD_cLib_addCalcAngleS(void*, u32, u32, u32, u32);
void* SearchHD_J3DJoint_getJointData(void*);
void SearchHD_mDoMtx_XrotM(void*, u32);
u32 SearchHD_cLib_calcTimer_i(void*);
void SearchHD_dPa_smokeEcallBack_end(void*);
void SearchHD_J3DModel_calc(void*);
void SearchHD_PSMTXMultVec(void*, void*, void*);
void SearchHD_daObj_Search_Act_c_set_mtx_light_A(void*);
void SearchHD_daObj_Search_Act_c_set_mtx_light_B(void*);
void SearchHD_cLib_chaseS(void*, u32, u32);
void SearchHD_dPa_control_c_set(void*, u32, u32, void*, void*, u32, u32, void*, u32, u32, u32, u32);
void SearchHD_fopAcM_setCullSizeBox(void*, f32, f32, f32, f32, f32, f32);
void SearchHD_dBgS_Regist(void*, void*, void*);
void SearchHD_dCcD_Stts_Init(void*, u32, u32, void*);
void SearchHD_dCcD_Cps_Set(void*, void*);
void SearchHD_cM3dGCps_set(void*, void*, void*);
void* SearchHD_J3DModelData_getJointTree(void*);
void* SearchHD_dPath_GetRoomPath(u32, u32);
void SearchHD_dLib_pathMove(void*, void*, void*, u32, u32, f32);
u32 SearchHD_cM_atan2s(f32, f32);
f32 SearchHD_sqrtf(f32);
f32 SearchHD_PSVECSquareDistance(void*, void*);
void SearchHD_cXyz___ml(void*, void*, f32);
void SearchHD_cXyz___pl(void*, void*, void*);
void SearchHD_mDoMtx_stack_c_push();
void SearchHD_mDoMtx_stack_c_scaleM(f32, f32, f32);
void SearchHD_mDoMtx_stack_c_pop();
void SearchHD_cLib_addCalcAngleS2(void*, u32, u32, u32);
void SearchHD_J3DUClipper_calcViewFrustum(void*);
u32 SearchHD_fopAcM_checkCullingBox(void*, f32, f32, f32, f32, f32, f32);
void* SearchHD_dScnKy_env_light_c_setLightTevColorType();
void SearchHD_dScnKy_env_light_c_settingTevStruct(void*, u32, void*, void*);
void SearchHD_dKy_setLightTevColorType_MAJI(void*, void*, void*);
void SearchHD_mDoExt_modelEntryDL(void*);
void SearchHD_cXyz_norm(void*, void*);
f32 SearchHD_PSVECSquareMag(void*);
void SearchHD_PSVECScale(void*, void*, f32);
void SearchHD_dDlst_2D_LightSpot_draw(void*, void*, void*, void*, u32, void*, f32, f32, f32, f32);
void SearchHD_mDoExt_modelUpdateDL(void*, u32);
void SearchHD_fopAcM_orderOtherEvent2(void*, const char*, u32, u32);
u32 SearchHD_dEvent_manager_c_getMyStaffId(void*, const char*, u32, u32);
u32 SearchHD_dEvent_manager_c_endCheckOld(void*, const char*);
void* SearchHD_dEvent_manager_c_getMyNowCutName(void*, u32);
void SearchHD_dEvent_manager_c_cutEnd(void*, u32);
u32 SearchHD_dEvent_manager_c_getEventIdx(void*, const char*, u32);
void SearchHD_dComIfGp_setNextStage(const char*, u32, u32, u32, u32, u32, u32, f32);
u32 SearchHD_cLib_distanceAngleS(u32, u32);
void SearchHD_cCcS_Set(void*, void*);
u32 SearchHD_dCcD_GObjInf_ChkCoHit(void*);
void* SearchHD_dCcD_GAtTgCoCommonBase_GetAc(void*);
u32 SearchHD_fopAcM_SearchByName(u32, void*);
u32 SearchHD_dLib_checkActorInFan(void*, void*, u32, u32, f32, f32);
u32 SearchHD_dLib_checkActorInCircle(void*, void*, f32, f32);
f32 SearchHD_cM_rndF(f32);
void SearchHD_cLib_addCalc2(void*, f32, f32, f32);
void SearchHD_dLib_setCirclePath(void*);
u32 SearchHD_dComIfG_resLoad(void*, const char*);
u32 SearchHD_fopAcM_entrySolidHeap(void*, void*, u32);

// HD: GHS member pointers are eight bytes; each mode also stores its diagnostic name.
using SearchModeProc = void (daObj_Search::Act_c::*)();
struct SearchModeEntry {
    gabi::ptmf<SearchModeProc> init;
    gabi::ptmf<SearchModeProc> run;
    gptr<char> name;
};
WWHD_OFFSET(SearchModeEntry, init, 0x0);
WWHD_OFFSET(SearchModeEntry, run, 0x8);
WWHD_OFFSET(SearchModeEntry, name, 0x10);
WWHD_SIZE(SearchModeEntry, 0x14);

// HD: the shared collider stores its capsule geometry as bytes.
struct SearchCapsuleGeometry {
    cXyz mStart, mEnd;
    be<u32> __vtbl;
    be<f32> mRadius;
};
WWHD_OFFSET(SearchCapsuleGeometry, mStart, 0x0);
WWHD_OFFSET(SearchCapsuleGeometry, mEnd, 0xC);
WWHD_OFFSET(SearchCapsuleGeometry, __vtbl, 0x18);
WWHD_OFFSET(SearchCapsuleGeometry, mRadius, 0x1C);
WWHD_SIZE(SearchCapsuleGeometry, 0x20);
inline SearchCapsuleGeometry* search_capsule(dCcD_Cps& collider) {
    return gabi::at<SearchCapsuleGeometry>(gabi::ea(collider.mCps));
}

// HD: only the controller and bird fields used by the searchlights are exposed here.
struct SearchControllerView : fopAc_ac_c {
    u8 _padSwitch[0x3D3 - 0x3AC];
    be<u8> mSwitch;
    u8 _padTimer[0x41A - 0x3D4];
    be<s16> mTimer;
    u8 _padYaw[0x1384 - 0x41C];
    be<s16> mBeamYaw;
};
WWHD_OFFSET(SearchControllerView, mSwitch, 0x3D3);
WWHD_OFFSET(SearchControllerView, mTimer, 0x41A);
WWHD_OFFSET(SearchControllerView, mBeamYaw, 0x1384);
WWHD_SIZE(SearchControllerView, 0x1386);
struct SearchBirdView : fopAc_ac_c {
    u8 _padState[0x3DA - 0x3AC];
    be<s16> mState, mPhase;
};
WWHD_OFFSET(SearchBirdView, mState, 0x3DA);
WWHD_OFFSET(SearchBirdView, mPhase, 0x3DC);
WWHD_SIZE(SearchBirdView, 0x3DE);
struct SearchEmitterView {
    u8 _padParticleScale[0x220];
    cXyz mParticleScale;
    u8 _padGlobalScale[0x238 - 0x22C];
    cXyz mGlobalScale;
};
WWHD_OFFSET(SearchEmitterView, mParticleScale, 0x220);
WWHD_OFFSET(SearchEmitterView, mGlobalScale, 0x238);
WWHD_SIZE(SearchEmitterView, 0x244);
struct SearchColliderShape {
    u8 _padVtables[0x18];
    be<u32> mShapeVtable, mGeometryVtable;
};
WWHD_OFFSET(SearchColliderShape, mShapeVtable, 0x18);
WWHD_OFFSET(SearchColliderShape, mGeometryVtable, 0x1C);
WWHD_SIZE(SearchColliderShape, 0x20);
inline SearchColliderShape* search_shape(dCcD_Cps& collider) {
    return gabi::at<SearchColliderShape>(gabi::ea(collider.mShapeAttr));
}

// HD: background and default draw-list selectors used by the projected light.
struct SearchDrawListView {
    u8 _padLists[0x40];
    be<u32> mpOpaListBG, mpXluListBG, mpOpaList, mpXluList;
};
WWHD_OFFSET(SearchDrawListView, mpOpaListBG, 0x40);
WWHD_OFFSET(SearchDrawListView, mpXluListBG, 0x44);
WWHD_OFFSET(SearchDrawListView, mpOpaList, 0x48);
WWHD_OFFSET(SearchDrawListView, mpXluList, 0x4C);
WWHD_SIZE(SearchDrawListView, 0x50);

// HD: scalar output pointers still refer to guest memory and require big-endian stores.
inline void search_output_float(f32* out, f32 value) { *gabi::at<be<f32>>(gabi::ea(out)) = value; }
inline void search_output_hit(bool* out, bool value) { *gabi::at<be<u8>>(gabi::ea(out)) = value; }

// HD: explicit word views preserve signalling-NaN payloads in component copies.
inline be<u32>& search_component_bits(cXyz& vector, u32 index) {
    return gabi::at<be<u32>>(gabi::ea(&vector))[index];
}
inline u32 search_virtual(u32 table, u32 offset) { return *gabi::at<be<u32>>(table + offset); }
struct SearchJointDataView {
    u8 _padJointNo[4];
    be<u16> mJointNo;
};
WWHD_OFFSET(SearchJointDataView, mJointNo, 0x4);
WWHD_SIZE(SearchJointDataView, 0x6);
struct SearchJointTreeView {
    u8 _padJointNum[8];
    be<u16> mJointNum;
};
WWHD_OFFSET(SearchJointTreeView, mJointNum, 0x8);
WWHD_SIZE(SearchJointTreeView, 0xA);

void* SearchJudge_actorId(void*, void*);

void* SearchJudge_actorName(void*, void*);

void SearchDestroy_status(void*, u32);

void SearchDestroy_capsule(void*, u32);

// HD: archive indices differ from the GameCube archive; these are the verified HD entries.
enum SearchResourceIndex {
    dRes_SEARCH_BDL_BEAM_e = 4,
    dRes_SEARCH_BDL_SEARCH_e = 5,
    dRes_SEARCH_DZB_BASE_e = 8,
    dRes_SEARCH_DZB_BEAM_A_e = 9,
    dRes_SEARCH_DZB_BEAM_B_e = 10,
};
template <class F> inline void* search_callback(F callback) {
    return gabi::at<void>(gabi::guest_fn((const void*)callback));
}

void SearchBackground_correctPosition(void*);

inline be<u16>& search_color_component(GXColorS10& color, u32 index) {
    return gabi::at<be<u16>>(gabi::ea(&color))[index];
}
inline be<u8>& search_color_component(GXColor& color, u32 index) {
    return gabi::at<be<u8>>(gabi::ea(&color))[index];
}
// HD: the three environmental colour bytes are copied in separate loops.
inline u8 search_environment_color(u32 environment, u32 index) {
    return gabi::at<be<u8>>(environment + 0xB64)[index];
}

// HD: collision-position callback extends the shared background prefix.
struct SearchBackgroundView {
    u8 _padCorrection[0xA8];
    be<u32> mCorrection;
};
WWHD_OFFSET(SearchBackgroundView, mCorrection, 0xA8);
WWHD_SIZE(SearchBackgroundView, 0xAC);

// HD: GHS allocation/deleting wrappers retain the original public entry signatures.
s32 createHeap_CB(daObj_Search::Act_c* a);
void search_SafeString_assureTermination(void* p);
void search_static_dtor(void* p, u32 flags);
void* search_status_ctor(void* p);
void* search_capsule_ctor(void* p);
daObj_Search::Bgc_c* Bgc_ctor(daObj_Search::Bgc_c* p);
daObj_Search::Act_c* Act_ctor(daObj_Search::Act_c* p);
void search_sinit();
void Act_dtor(daObj_Search::Act_c* a, u32 flags);
s32 nodeControl_CB(void* node, s32 timing);

namespace daObj_Search {
// HD: static wrappers retain GameCube Mthd names and let autoglue distinguish them from instance methods.
struct Mthd {
    static s32 Create(Act_c* actor);
    static s32 Delete(Act_c* actor);
    static s32 Execute(Act_c* actor);
    static s32 Draw(Act_c* actor);
    static s32 IsDelete(Act_c* actor);
};
} // namespace daObj_Search
