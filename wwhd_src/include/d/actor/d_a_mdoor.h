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
    /* 0x3AC */ dPa_smokeEcallBack mSmokeCallback;  /* was m290: smokeInit 021C10C4: the dPa_smokeEcallBack of the ID_AK_JT_ELEMENTSMOKE00 landing smoke; create 021C1E8C constructs it, smokeEnd 021C19FC ends it (tww dPa_smokeEcallBack) */
    /* 0x3CC */ request_of_phase_process_class mPhase;
    /* 0x3D4 */ gptr<J3DModel> mpModel;
    /* 0x3D8 */ gptr<dBgW> mpBgW;
    /* 0x3DC */ be<s32> mStaffId;  /* was m2C0: daMdoor_actionDemoWait 021C1418 ("Mori1") / daMdoor_actionReady 021C17A4 ("SHUTTER_DOOR"): dComIfGp_evmng_getMyStaffId; demoProc 021C1174 reads the action and advance of that staff */
    /* 0x3E0 */ be<s16> mDelayTimer;  /* was m2C4: CreateInit 021C1AFC sets 1 (type 1); daMdoor_actionGenocide 021C1514 counts it down after the room's enemies are gone before ordering the event; daMdoor_actionReadyOpen 021C188C sets 20 and daMdoor_actionOpen 021C15B0 counts it down before the door rises */
    /* 0x3E2 */ be<s16> mEventIdx;  /* was m2C6: CreateInit 021C1AFC: event index of "MORI1_EVENT" (type 0) or "DEFAULT_STOP_OPEN" (types 1 / 2); order and the demo / open actions start and end-check it */
    /* 0x3E4 */ be<u8> mToolId;  /* was m2C8: CreateInit 021C1AFC: 0xFF, or getToolId() for type 2; order passes it to fopAcM_orderOtherEventId; it selects ACT_SWITCH8_e / ACT_SWITCH12_e */
    /* 0x3E5 */ be<u8> mAction;
    /* 0x3E6 */ be<u16> mDemoFlags;  /* was m2CA: demoProc 021C1174: bit 0 set when the close action starts, cleared when the door lands (then the debris and smokeInit) */
    /* 0x3E8 */ be<f32> mHeight;  /* was m2CC: calcMtx 021C0EAC: the model's height above current.pos; CreateInit 021C1AFC: 0 (closed) or 280 (open); demoProc 021C1174 / daMdoor_actionOpen 021C15B0 chase it to 0 / 280 by speedF */
};
WWHD_OFFSET(daMdoor_c, mSmokeCallback, 0x3AC);
WWHD_OFFSET(daMdoor_c, mPhase, 0x3CC);
WWHD_OFFSET(daMdoor_c, mpModel, 0x3D4);
WWHD_OFFSET(daMdoor_c, mpBgW, 0x3D8);
WWHD_OFFSET(daMdoor_c, mStaffId, 0x3DC);
WWHD_OFFSET(daMdoor_c, mDelayTimer, 0x3E0);
WWHD_OFFSET(daMdoor_c, mEventIdx, 0x3E2);
WWHD_OFFSET(daMdoor_c, mToolId, 0x3E4);
WWHD_OFFSET(daMdoor_c, mAction, 0x3E5);
WWHD_OFFSET(daMdoor_c, mDemoFlags, 0x3E6);
WWHD_OFFSET(daMdoor_c, mHeight, 0x3E8);
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
