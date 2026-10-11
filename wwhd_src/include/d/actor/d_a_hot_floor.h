/* Heat-floor actor, private WWHD layout. */
#pragma once
#include "gc_api/gc_api.h"
struct hot_floor_rotation_c { be<f32> matrix[3][3]; };
WWHD_SIZE(hot_floor_rotation_c, 0x24);
void hot_floor_brk_ct(void*);
void hot_floor_actor_dt(void*, s32);
void hot_floor_setRTMatrix(Mtx34*, hot_floor_rotation_c*, cXyz*);
struct hot_floor_brk_c { u8 storage[0x78]; };
WWHD_SIZE(hot_floor_brk_c, 0x78);
// HD: emitter lacks the GameCube vtable; private view of the touched members.
struct hot_floor_emitter_c {
    u8 pad0[0x5C];
    be<s32> mMaxFrame;
    u8 pad60[0x1F0 - 0x60];
    hot_floor_rotation_c mGlobalRot;
    u8 pad214[0x22C - 0x214];
    cXyz mGlobalTrs;
    u8 pad238[0x254 - 0x238];
    be<u32> mFlags;
    void becomeInvalidEmitter() {
        u32 flags = mFlags;
        mMaxFrame = -1;
        mFlags = flags | 1;
    }
    void setGlobalRTMatrix(Mtx34* matrix) {
        hot_floor_setRTMatrix(matrix, &mGlobalRot, &mGlobalTrs);
    }
};
WWHD_OFFSET(hot_floor_emitter_c, mGlobalRot, 0x1F0);
WWHD_OFFSET(hot_floor_emitter_c, mGlobalTrs, 0x22C);
WWHD_OFFSET(hot_floor_emitter_c, mFlags, 0x254);
WWHD_SIZE(hot_floor_emitter_c, 0x258);
struct daHot_Floor_c : fopAc_ac_c {
    daHot_Floor_c();
    enum : u32 { __vt = 0x10011794 };
    void set_mtx_init();
    void set_mtx();
    cPhs_State CreateInit();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ be<u32> field_0x298;
    /* 0x3B8 */ be<u32> field_0x29c;
    /* 0x3BC */ hot_floor_brk_c mBrkAnm; /* mDoExt_brkAnm (HD 0x78) */
    /* 0x434 */ be<f32> mSpawnTimer;
    /* 0x438 */ be<u32> field_0x2bc;
    /* 0x43C */ mDoExt_btkAnm mBtkAnm; /* HD 0x74 */
    /* 0x4B0 */ gptr<hot_floor_emitter_c> mEmitter1;
    /* 0x4B4 */ gptr<hot_floor_emitter_c> mEmitter2;
    /* 0x4B8 */ be<u8> mbSpawnParticle;
    /* 0x4B9 */ u8 _4B9[3];
    /* 0x4BC */ Mtx34 mtx[5];
    /* 0x5AC */ gptr<Mtx34> mtx_p;
    /* 0x5B0 */ be<u32> field_0x3d4;
};
WWHD_OFFSET(daHot_Floor_c, mSpawnTimer, 0x434);
WWHD_OFFSET(daHot_Floor_c, mBtkAnm, 0x43C);
WWHD_OFFSET(daHot_Floor_c, mbSpawnParticle, 0x4B8);
WWHD_OFFSET(daHot_Floor_c, mtx_p, 0x5AC);
WWHD_SIZE(daHot_Floor_c, 0x5B4);

