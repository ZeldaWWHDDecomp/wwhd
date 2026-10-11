// Ganon's Castle warp portal. Private game-derived layout, verified by d_a_ygcwp.
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_kankyo.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/MSL_C++/new.h"

// HD: BRK controllers grow from 0x18 to 0x78; their frame controller stays first.
struct YgcwpBrkAnm : mDoExt_baseAnm {
    u8 mBindingState[0x68];
    BOOL init(J3DModelData*, J3DAnmTevRegKey*, u32, u32, f32, s32, s32, u32, u32);
    void entry(J3DModelData*, f32);
    void setPlaySpeed(f32 speed) { mFrameCtrl.setRate(speed); }
    void setFrame(f32 frame) { mFrameCtrl.setFrame(frame); }
    BOOL play() { return mDoExt_baseAnm_play(this); }
};
WWHD_SIZE(YgcwpBrkAnm, 0x78);

struct daYgcwp_c : fopAc_ac_c {
    BOOL create_heap();
    s32 _create();
    BOOL _delete();
    void init_mtx();
    void make_shine();
    void set_timer();
    BOOL _execute();
    BOOL _draw();

    static const char M_arcname[];
    /* 0x3AC */ gptr<J3DModel> mpModel;
    /* 0x3B0 */ request_of_phase_process_class mPhs;
    /* 0x3B8 */ YgcwpBrkAnm mBrkAnm[2];
    /* 0x4A8 */ be<s32> mStaffId;
    /* 0x4AC */ be<s32> mTimer;
    /* 0x4B0 */ be<s32> mCurBrk;
    /* 0x4B4 */ be<s32> m2D8; // tww's unnamed portal selection.
};
WWHD_OFFSET(daYgcwp_c, mpModel, 0x3AC);
WWHD_OFFSET(daYgcwp_c, mPhs, 0x3B0);
WWHD_OFFSET(daYgcwp_c, mBrkAnm, 0x3B8);
WWHD_OFFSET(daYgcwp_c, mStaffId, 0x4A8);
WWHD_OFFSET(daYgcwp_c, mTimer, 0x4AC);
WWHD_OFFSET(daYgcwp_c, mCurBrk, 0x4B0);
WWHD_OFFSET(daYgcwp_c, m2D8, 0x4B4);
WWHD_SIZE(daYgcwp_c, 0x4B8);

// HD: the only player member this portal accesses; no player implementation dependency.
struct YgcwpPlayer : fopAc_ac_c {
    u8 mPlayerState[0xC];
    be<u32> mNoResetFlg0;
    enum { daPyFlg0_NO_DRAW = 0x08000000 };
};
WWHD_OFFSET(YgcwpPlayer, mNoResetFlg0, 0x3B8);
WWHD_SIZE(YgcwpPlayer, 0x3BC);

enum { EVENT_WARP_START, EVENT_WARP_APPEAR, EVENT_WARP_MAKE };
enum { YGCWP_WARP_EVENT = 0x480, ID_AK_SN_GANONCASTLEWARP00 = 0x8316,
       JA_SE_LK_FG_ROOM_WARP_OUT = 0x288A, dRes_INDEX_YGCWP_BDL_YGCWP00_e = 4 };

void YgcwpBrkArray_ct(YgcwpBrkAnm*, u32, u32, u32);
void Ygcwp_offTmpBit(dSv_event_c*, u32);
void YgcwpFirstHeaderObject_ct(u8*);
void YgcwpSecondHeaderObject_ct(u8*);

s32 Ygcwp_getReverb(s32);

// HD: this translation unit's resource/action tables and common-header objects.
struct YgcwpBrkTable { be<u32> indices[2]; };
struct YgcwpActionTable { gptr<char> names[3]; };
struct YgcwpHeaderStatics {
    /* 0x00 */ be<f32> minimum, maximum;
    /* 0x08 */ u8 first, second, padding[2];
    /* 0x0C */ be<u32> zero[4];
};
struct YgcwpDestructorRecords { u8 entries[3][12]; };
// Only the address of this vtable prefix is used; its entries are untouched.
struct YgcwpActorVtable { be<u32> entries[1]; };
WWHD_SIZE(YgcwpBrkTable, 8);
WWHD_SIZE(YgcwpActionTable, 12);
WWHD_OFFSET(YgcwpHeaderStatics, minimum, 0);
WWHD_OFFSET(YgcwpHeaderStatics, maximum, 4);
WWHD_OFFSET(YgcwpHeaderStatics, first, 8);
WWHD_OFFSET(YgcwpHeaderStatics, second, 9);
WWHD_OFFSET(YgcwpHeaderStatics, zero, 0xC);
WWHD_SIZE(YgcwpHeaderStatics, 0x1C);
WWHD_SIZE(YgcwpDestructorRecords, 0x24);
WWHD_SIZE(YgcwpActorVtable, 4);
