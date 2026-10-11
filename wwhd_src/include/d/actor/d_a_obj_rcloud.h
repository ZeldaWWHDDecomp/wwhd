// Dragon Roost clouds, private WWHD layout.
#pragma once
#include "gc_api/gc_api.h"

struct RcloudTexMtx {
    u8 mPadding[0x10];
    be<f32> mTranslationX, mTranslationY;
};
struct RcloudMatPacket {
    u8 mStorage[0x3C];
    RcloudTexMtx* getTexMtx(u32 index);
};
struct RcloudModelLayout {
    u8 mPadding[0x34];
    gptr<RcloudMatPacket> mMatPackets;
};
struct RcloudMaterialData {
    u8 mPadding[0xC];
    be<u16> mMaterialIndex;
};
struct RcloudMaterial { gptr<RcloudMaterialData> mpData; };
struct RcloudMaterialTable {
    u8 mPadding[0x24];
    be<u16> mMaterialNum;
};
WWHD_OFFSET(RcloudModelLayout, mMatPackets, 0x34);
WWHD_SIZE(RcloudMatPacket, 0x3C);
WWHD_OFFSET(RcloudTexMtx, mTranslationX, 0x10);
RcloudMaterialTable* Rcloud_getMaterialTable(J3DModelData*);
struct RcloudBtkAnm : mDoExt_btkAnm {
    // HD: this variant enters animation on the model rather than model data.
    void entry(J3DModel*, f32 frame);
};
BOOL Rcloud_isSymbol(dSv_player_collect_c*, u8);
BOOL Rcloud_startCheck(dEvent_manager_c*, const char*);

class daObjRcloud_c : public fopAc_ac_c {
public:
    typedef void (daObjRcloud_c::*ProcFunc)();
    struct RcloudProcEntry { gabi::ptmf<ProcFunc> proc; };
    enum Param_e { PRM_ARG_W = 1, PRM_ARG_S = 0 };
    daObjRcloud_c();
    int param_get_arg();
    void init_mtx();
    BOOL create_heap();
    s32 _create();
    BOOL _delete();
    void wait_act_proc();
    void clouds_lift_start_wait_act_proc();
    void clouds_lift_act_proc();
    void setup_action(s32);
    BOOL _execute();
    void setTexMtx();
    BOOL _draw();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ RcloudBtkAnm mBtkAnm;
    /* 0x42C */ gabi::ptmf<ProcFunc> mCurrentActProc;
    /* 0x434 */ be<s32> m2BC;
    /* 0x438 */ be<s32> mDemoNameIndex;
    /* 0x43C */ be<u8> mResourceLoadedFlag;
    /* 0x43D */ u8 m2C5[3];
    /* 0x440 */ be<f32> mCloudAnimProgress;
};
WWHD_OFFSET(daObjRcloud_c, mCurrentActProc, 0x42C);
WWHD_OFFSET(daObjRcloud_c, mCloudAnimProgress, 0x440);
WWHD_SIZE(daObjRcloud_c, 0x444);

// HD: header statics have the HIO float pair interleaved before their SDK objects.
struct RcloudStatics {
    be<f32> mMinAngle, mMaxAngle, mUnused, mRate;
    u8 mSDKObjectA, mSDKObjectB;
    u8 mPadding[2];
    be<u32> mZero[4];
};
WWHD_SIZE(RcloudStatics, 0x24);
void Rcloud_initSDKObjectA(u8*);
void Rcloud_initSDKObjectB(u8*);
