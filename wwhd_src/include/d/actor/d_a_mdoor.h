/* Wooden/metal bars, GameCube names and WWHD layout.
 * HD: actor members move by +0x11C; the smoke callback retains its 0x20-byte size. */
#pragma once
#include "gc_api/gc_api.h"

enum {
    dRes_INDEX_MDOOR_BDL_MORI1_e = 4,
    dRes_INDEX_MDOOR_BDL_MORI2_e = 5,
    dRes_INDEX_MDOOR_DZB_MORI1_e = 8,
    dRes_INDEX_MDOOR_DZB_MORI2_e = 9,
    JA_SE_OBJ_CAGE_CLOSE = 0x697C,
    JA_SE_OBJ_CAGE_OPEN = 0x697D,
};
class daMdoor_c : public fopAc_ac_c {
  public:
    enum : u32 { __vt = 0x1001490C };
    enum Action_e {
        ACT_WAIT0_e,
        ACT_DEMO_WAIT_e,
        ACT_DEMO_e,
        ACT_WAIT3_e,
        ACT_GENOCIDE_e,
        ACT_READY5_e,
        ACT_EVENT6_e,
        ACT_WAIT7_e,
        ACT_SWITCH8_e,
        ACT_READY9_e,
        ACT_EVENT10_e,
        ACT_WAIT11_e,
        ACT_SWITCH12_e,
        ACT_READY_OPEN_e,
        ACT_OPEN_e,
        ACT_WAIT15_e
    };
    u8 getSwbit();
    u8 getType();
    u8 getToolId();
    u8 getShapeType();
    BOOL CreateHeap();
    void calcMtx();
    void smokeInit();
    void smokeEnd();
    BOOL CreateInit();
    s32 create(); // HD: preserve the full phase return register.
    void demoProc();
    static const char M_arcname[];
    /* 0x3AC */ dPa_smokeEcallBack m290;
    /* 0x3CC */ request_of_phase_process_class mPhase;
    /* 0x3D4 */ gptr<J3DModel> mpModel;
    /* 0x3D8 */ gptr<dBgW> mpBgW;
    /* 0x3DC */ be<s32> m2C0;
    /* 0x3E0 */ be<s16> m2C4;
    /* 0x3E2 */ be<s16> m2C6;
    /* 0x3E4 */ be<u8> m2C8;
    /* 0x3E5 */ be<u8> mAction;
    /* 0x3E6 */ be<u16> m2CA;
    /* 0x3E8 */ be<f32> m2CC;
};
WWHD_OFFSET(daMdoor_c, m290, 0x3AC);
WWHD_OFFSET(daMdoor_c, mPhase, 0x3CC);
WWHD_OFFSET(daMdoor_c, mpModel, 0x3D4);
WWHD_OFFSET(daMdoor_c, mpBgW, 0x3D8);
WWHD_OFFSET(daMdoor_c, m2C0, 0x3DC);
WWHD_OFFSET(daMdoor_c, m2C4, 0x3E0);
WWHD_OFFSET(daMdoor_c, m2C6, 0x3E2);
WWHD_OFFSET(daMdoor_c, m2C8, 0x3E4);
WWHD_OFFSET(daMdoor_c, mAction, 0x3E5);
WWHD_OFFSET(daMdoor_c, m2CA, 0x3E6);
WWHD_OFFSET(daMdoor_c, m2CC, 0x3E8);
WWHD_SIZE(daMdoor_c, 0x3EC);

// HD: this unit writes both particle scale vectors, alternating their components.
struct MdoorEmitter : JPABaseEmitter {
    u8 _70[0x220 - 0x70];
    be<f32> mGlobalScale[3];
    u8 _22C[0x238 - 0x22C];
    be<f32> mGlobalScaleOut[3];
};
WWHD_OFFSET(MdoorEmitter, mGlobalScale, 0x220);
WWHD_OFFSET(MdoorEmitter, mGlobalScaleOut, 0x238);
WWHD_SIZE(MdoorEmitter, 0x244);
struct MdoorActionNames {
    gptr<char> entries[3];
};
struct MdoorActionTable {
    be<u32> entries[16];
};
WWHD_SIZE(MdoorActionNames, 12);
WWHD_SIZE(MdoorActionTable, 64);

// HD: full return widths and explicit SafeString stack placement used by this unit.
void mdoorOffEventBit(dSv_event_c*, u16);
void mdoorActorConstruct(fopAc_ac_c*);
void* mdoorGetResource(dRes_control_c*, SafeString*, s32);
BOOL mdoorSetBackground(dBgW*, cBgD_t*, u32, Mtx34*);
u32 mdoorSearchEnemy(s8);
void mDoAud_subBgmStop();

void mdoorCopyActions(void*, const void*, u32);
