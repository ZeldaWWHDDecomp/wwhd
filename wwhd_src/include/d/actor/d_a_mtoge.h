/** Fortress spikes, WWHD layout.
 * HD: the actor base shifts tww members by 0x11C. */
#pragma once
#include "gc_api/gc_api.h"
enum {
    dRes_INDEX_MTOGE_BMD_S_MTOGE_e = 4,
    dRes_INDEX_MTOGE_DZB_S_MTOGE_e = 7,
};

enum Action {
    ACT_WAIT,
    ACT_HIND,
    ACT_UP,
    ACT_ARRIVAL,
    ACT_DOWN,
};

enum {
    JA_SE_OBJ_TOGE_OUT = 0x6976,
    JA_SE_OBJ_TOGE_IN = 0x6977,
};

struct daMtoge_c : fopAc_ac_c {
    void setAction(u8 action) { mAction = action; }

    u8 getSwbit();
    BOOL CreateHeap();
    void calcMtx();
    BOOL CreateInit();
    cPhs_State create();

    /* 0x3AC */ request_of_phase_process_class mPhaseProcReq; /* GameCube 0x290 */
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<dBgW> mpBgW;
    /* 0x3BC */ be<u8> mAction;
    /* 0x3BD */ be<s8> m2A1;
    /* 0x3BE */ be<s8> m2A2;
    /* 0x3BF */ be<s8> m2A3;
    /* 0x3C0 */ be<f32> mHeightOffset;
};
WWHD_OFFSET(daMtoge_c, mpModel, 0x3B4);
WWHD_OFFSET(daMtoge_c, mHeightOffset, 0x3C0);

WWHD_SIZE(daMtoge_c, 0x3C4);
struct MtogeActionTable { be<u32> entries[5]; };
WWHD_SIZE(MtogeActionTable, 0x14);
