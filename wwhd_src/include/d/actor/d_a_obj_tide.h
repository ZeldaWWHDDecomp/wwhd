/* Tide actor, GameCube member names and verified HD layout.
 * HD: the moving-background tail ends at 0x3E0; tide fields move by 0x118.
 * The existing shared moving-background header overlaps other binding layouts, so its tail is
 * represented here without changing shared declarations. */
#pragma once
#include "bindings.h"
namespace daObjTide {
enum Type_e { TYPE_0_e, TYPE_1_e, TYPE_2_e, TYPE_3_e, TYPE_4_e, TYPE_5_e };
struct Act_c : fopAc_ac_c {
    enum Prm_e { PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 24, PRM_TYPE_W = 3, PRM_TYPE_S = 16 };
    enum Mode_e { MODE_NORM, MODE_GMTW_PRE, MODE_GMTW, MODE_GMTW_DEMO };
    void se_init_gmtw();
    s32 Mthd_Create();
    s32 Mthd_Delete();
    BOOL CreateHeap();
    void set_mtx();
    void init_mtx();
    void mode_gmtw_pre_init();
    void mode_norm_init();
    BOOL Create();
    BOOL Execute(be<u32>* matrixOut);
    BOOL Draw();
    void se_stop_gopo();
    void se_stop_out();
    void se_stop_up();
    BOOL Delete();
    void move_to_aim(BOOL sound, f32 aim);
    bool chk_demo_rescue();
    BOOL gmtw_demo_up_start();
    void mode_norm();
    void mode_gmtw_demo_init();
    void mode_gmtw_init();
    void mode_gmtw_pre();
    void se_start_gopo();
    void se_start_out();
    void se_start_up();
    void se_pos_up();
    void se_set_gmtw();
    void mode_gmtw();
    void mode_gmtw_demo();
    /* 0x3AC */ gptr<dBgW> mpBgW;
    /* 0x3B0 */ Mtx34 mBgMtx;
    /* 0x3E0 */ request_of_phase_process_class mPhase;
    /* 0x3E8 */ gptr<J3DModel> mModel1;
    /* 0x3EC */ gptr<mDoExt_bckAnm> mBck1;
    /* 0x3F0 */ gptr<mDoExt_brkAnm> mBrk1;
    /* 0x3F4 */ gptr<mDoExt_btkAnm> mBtk;
    /* 0x3F8 */ gptr<J3DModel> mModel2;
    /* 0x3FC */ gptr<mDoExt_brkAnm> mBrk2;
    /* 0x400 */ be<s32> M_type;
    /* 0x404 */ be<s32> m2EC;
    /* 0x408 */ be<f32> m2F0;
    /* 0x40C */ be<s16> m2F4;
    /* 0x40E */ be<u8> m2F6;
    /* 0x40F */ be<u8> m2F7;
    /* 0x410 */ be<u8> m2F8;
    /* 0x411 */ be<u8> m2F9;
    /* 0x412 */ be<u8> m2FA;
    /* 0x413 */ be<u8> m2FB;
    /* 0x414 */ be<s32> M_id_gopo;
    /* 0x418 */ be<s32> M_id_out;
    /* 0x41C */ be<s32> M_id_up;
    /* 0x420 */ be<u8> m308;
    /* 0x421 */ be<u8> m309;
    /* 0x422 */ be<u8> m30A;
    /* 0x423 */ be<u8> m30B;
    /* 0x424 */ be<s32> m30C;
};
} // namespace daObjTide
WWHD_SIZE(daObjTide::Act_c, 0x428);
WWHD_OFFSET(daObjTide::Act_c, mpBgW, 0x3AC);
WWHD_OFFSET(daObjTide::Act_c, mPhase, 0x3E0);
WWHD_OFFSET(daObjTide::Act_c, mModel1, 0x3E8);
WWHD_OFFSET(daObjTide::Act_c, mBck1, 0x3EC);
WWHD_OFFSET(daObjTide::Act_c, mBrk1, 0x3F0);
WWHD_OFFSET(daObjTide::Act_c, mBtk, 0x3F4);
WWHD_OFFSET(daObjTide::Act_c, mModel2, 0x3F8);
WWHD_OFFSET(daObjTide::Act_c, mBrk2, 0x3FC);
WWHD_OFFSET(daObjTide::Act_c, M_type, 0x400);
WWHD_OFFSET(daObjTide::Act_c, m2EC, 0x404);
WWHD_OFFSET(daObjTide::Act_c, m2F0, 0x408);
WWHD_OFFSET(daObjTide::Act_c, m2F4, 0x40C);
WWHD_OFFSET(daObjTide::Act_c, m2F6, 0x40E);
WWHD_OFFSET(daObjTide::Act_c, m2F7, 0x40F);
WWHD_OFFSET(daObjTide::Act_c, m2F8, 0x410);
WWHD_OFFSET(daObjTide::Act_c, M_id_gopo, 0x414);
WWHD_OFFSET(daObjTide::Act_c, m308, 0x420);
WWHD_OFFSET(daObjTide::Act_c, m30C, 0x424);

namespace daObjTide {
struct Attr_c {
    /* 0x00 */ gptr<const char> arcName;
    /* 0x04 */ be<s16> bdl_res_index;
    /* 0x06 */ be<s16> dzb_res_index;
    /* 0x08 */ be<s16> bck_res_index;
    /* 0x0A */ be<s16> brk_res_index;
    /* 0x0C */ be<s16> btk_res_index;
    /* 0x0E */ u8 padding0E[2];
    /* 0x10 */ be<u32> bdl_dlist_flags;
    /* 0x14 */ be<s16> nure_bdl_res_index;
    /* 0x16 */ be<s16> nure_brk_res_index;
    /* 0x18 */ be<u32> nure_bdl_dlist_flags;
    /* 0x1C */ be<u32> res_size;
    /* 0x20 */ be<s16> m20;
    /* 0x22 */ u8 padding22[2];
    /* 0x24 */ be<f32> m24;
    /* 0x28 */ be<f32> m28;
    /* 0x2C */ be<f32> m2C;
    /* 0x30 */ be<f32> m30;
    /* 0x34 */ be<s16> min_x;
    /* 0x36 */ be<s16> min_y;
    /* 0x38 */ be<s16> min_z;
    /* 0x3A */ be<s16> max_x;
    /* 0x3C */ be<s16> max_y;
    /* 0x3E */ be<s16> max_z;
    /* 0x40 */ be<u8> sch_flag;
    /* 0x41 */ u8 padding41;
    /* 0x42 */ be<s16> m42;
    /* 0x44 */ be<f32> m44;
};
WWHD_SIZE(Attr_c, 0x48);
WWHD_OFFSET(Attr_c, bdl_dlist_flags, 0x10);
WWHD_OFFSET(Attr_c, m24, 0x24);
WWHD_OFFSET(Attr_c, sch_flag, 0x40);
} // namespace daObjTide
namespace tideHD {
enum {
    dRes_INDEX_HUMI_BDL_HNURE0_e = 0x6,
    dRes_INDEX_HUMI_BDL_HNURE2_e = 0x7,
    dRes_INDEX_HUMI_BDL_HNURE3_e = 0x8,
    dRes_INDEX_HUMI_BDL_HNURE4_e = 0x9,
    dRes_INDEX_HUMI_BDL_HNURE5_e = 0xA,
    dRes_INDEX_HUMI_BDL_HUMI0_e = 0xB,
    dRes_INDEX_HUMI_BDL_HUMI2_e = 0xC,
    dRes_INDEX_HUMI_BDL_HUMI3_e = 0xD,
    dRes_INDEX_HUMI_BDL_HUMI4_e = 0xE,
    dRes_INDEX_HUMI_BDL_HUMI5_e = 0xF,
    dRes_INDEX_HUMI_BRK_HNURE0_e = 0x12,
    dRes_INDEX_HUMI_BRK_HNURE2_e = 0x13,
    dRes_INDEX_HUMI_BRK_HNURE3_e = 0x14,
    dRes_INDEX_HUMI_BRK_HNURE4_e = 0x15,
    dRes_INDEX_HUMI_BRK_HNURE5_e = 0x16,
    dRes_INDEX_HUMI_BTK_HUMI0_e = 0x19,
    dRes_INDEX_HUMI_BTK_HUMI2_e = 0x1A,
    dRes_INDEX_HUMI_BTK_HUMI3_e = 0x1B,
    dRes_INDEX_HUMI_BTK_HUMI4_e = 0x1C,
    dRes_INDEX_HUMI_BTK_HUMI5_e = 0x1D,
    dRes_INDEX_HUMI_DZB_HUMI0_e = 0x20,
    dRes_INDEX_HUMI_DZB_HUMI2_e = 0x21,
    dRes_INDEX_HUMI_DZB_HUMI3_e = 0x22,
    dRes_INDEX_HUMI_DZB_HUMI4_e = 0x23,
    dRes_INDEX_HUMI_DZB_HUMI5_e = 0x24,
    dRes_INDEX_GMTW_BCK_GMTW00_e = 0x6,
    dRes_INDEX_GMTW_BDL_GMTW00_e = 0x9,
    dRes_INDEX_GMTW_BTK_GMTW00_e = 0xC,
    dRes_INDEX_GMTW_DZB_GUMI0_e = 0xF,
    JA_SE_ATM_SHIREN_SEA_UP = 0x701B,
    JA_SE_ATM_SHIREN_SEA_DOWN = 0x701C,
    JA_SE_ATM_MJ_WATER_GOPO = 0x702A,
    JA_SE_ATM_MJ_WATER_OUT = 0x702B,
    JA_SE_ATM_MJ_WATER_UP = 0x702C,
    JA_SE_ATM_MJ_WATER_PACHA = 0x702D,
    ID_IT_SN_MT_WATER00 = 0x814D,
    LEVEL_SE_PROCESS = 0x18,
    STATE_1 = 1,
    STATE_2 = 2,
    STATE_4 = 4,
    RESCUE_COMPLETE = 0x401,
};

struct SaveBlock {
    u8 padding[0x20];
    dSv_info_c mSaveInfo;
};
struct DrawLists {
    u8 padding[0x5D70];
    be<u32> mListBG[2];
    be<u32> mList[2];
};
struct DrawBufferPair {
    be<u32> mDrawBuffer[2];
};
struct DemoActor {
    u8 padding[4];
    be<u16> mFlags;
    u8 padding2[0x46];
    be<u32> mPrm;
};
struct EnvironmentActor {
    u8 padding[0xE0];
    cXyz mPos;
};
struct MoveBgVtable {
    u8 methods[0x2C];
    be<u32> Draw;
    u8 methods2[0xC];
    be<u32> IsDelete;
};
struct ActorVtableView {
    /* 0x00 */ u8 padding[0xB4];
    /* 0xB4 */ gptr<MoveBgVtable> mVtable;
};
WWHD_OFFSET(ActorVtableView, mVtable, 0xB4);
// HD: constructor-only view; the four trailing words are cleared individually.
struct BckConstruction {
    /* 0x00 */ J3DFrameCtrl mFrameCtrl;
    /* 0x10 */ be<u32> __vtbl;
    /* 0x14 */ u8 mMaya[0x34];
    /* 0x48 */ be<u32> __vtblMaya;
    /* 0x4C */ u8 padding4C[0xC];
    /* 0x58 */ be<u32> mpAnm;
    /* 0x5C */ u8 padding5C[0x20];
    /* 0x7C */ be<u32> mpMtxCalc;
    /* 0x80 */ be<u32> mpMtxCalc2;
    /* 0x84 */ be<u32> mpTransform;
    /* 0x88 */ be<u32> mAnm;
};
struct HeaderZeros {
    be<u32> words[4];
};
struct DestructorRecord {
    u8 data[0xC];
};
WWHD_OFFSET(SaveBlock, mSaveInfo, 0x20);
WWHD_OFFSET(DrawLists, mListBG, 0x5D70);
WWHD_OFFSET(DrawLists, mList, 0x5D78);
WWHD_SIZE(DrawBufferPair, 8);
WWHD_OFFSET(DemoActor, mFlags, 4);
WWHD_OFFSET(DemoActor, mPrm, 0x4C);
WWHD_OFFSET(EnvironmentActor, mPos, 0xE0);
WWHD_OFFSET(MoveBgVtable, Draw, 0x2C);
WWHD_OFFSET(MoveBgVtable, IsDelete, 0x3C);
WWHD_SIZE(BckConstruction, 0x8C);
WWHD_OFFSET(BckConstruction, mpAnm, 0x58);
WWHD_OFFSET(BckConstruction, mAnm, 0x88);
} // namespace tideHD

using TideModeProc = void (daObjTide::Act_c::*)();

struct TideModeEntry {
    gabi::ptmf<TideModeProc> run;
};
WWHD_SIZE(TideModeEntry, 8);

WWHD_OFFSET(daObjTide::Act_c, mBgMtx, 0x3B0);
WWHD_SIZE(tideHD::HeaderZeros, 0x10);
WWHD_SIZE(tideHD::DestructorRecord, 0xC);
WWHD_SIZE(tideHD::ActorVtableView, 0xB8);

WWHD_OFFSET(daObjTide::Act_c, m2F9, 0x411);
WWHD_OFFSET(daObjTide::Act_c, m2FA, 0x412);
WWHD_OFFSET(daObjTide::Act_c, m2FB, 0x413);
WWHD_OFFSET(daObjTide::Act_c, M_id_out, 0x418);
WWHD_OFFSET(daObjTide::Act_c, M_id_up, 0x41C);
WWHD_OFFSET(daObjTide::Act_c, m309, 0x421);
WWHD_OFFSET(daObjTide::Act_c, m30A, 0x422);
WWHD_OFFSET(daObjTide::Act_c, m30B, 0x423);
WWHD_SIZE(tideHD::SaveBlock, sizeof(dSv_info_c) + 0x20);
WWHD_SIZE(tideHD::DrawLists, 0x5D80);
WWHD_SIZE(tideHD::DemoActor, 0x50);
WWHD_SIZE(tideHD::EnvironmentActor, 0xEC);
WWHD_SIZE(tideHD::MoveBgVtable, 0x40);
