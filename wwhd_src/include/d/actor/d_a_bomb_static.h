/* Bomb static utility methods. */
#pragma once
#include "gc_api/gc_api.h"
#include "d/actor/d_a_bomb.h"
#include "d/actor/d_a_bomb2.h"
/* Private views reuse the existing actor layouts without extending their shared APIs.
 * HD: const qualifiers are omitted for the current member-function verifier. */
namespace BombStatic {
struct daBomb_c : ::daBomb_c {
    enum Prm_e { PRM_STATE_W = 8, PRM_STATE_S = 0, PRM_CHEAP_EFF_W = 1,
        PRM_CHEAP_EFF_S = 16, PRM_ANGXZERO_W = 1, PRM_ANGXZERO_S = 17,
        PRM_VERSION_W = 1, PRM_VERSION_S = 31 };
    u32 prm_get_version();
    void _prm_chk_version();
    s16 getBombRestTime();
    u8 getBombCheck_Flag();
    void setBombCheck_Flag();
    void setBombFire_ON();
    void setBombNoHit();
    void setBombOffCoSet();
    void setBombOnCoSet();
    void setBombNoEff();
    void setBombRestTime(s16);
    void setNoGravityTime(s16);
    u32 prm_get_state();
    bool chk_state(u32);
    void change_state(u32);
    bool get_explode_instant();
    bool prm_get_cheapEff();
    bool prm_get_angXZero();
};
namespace daBomb2 {
struct Act_c : ::daBomb2::Act_c {
    void remove_fuse_effect();
    void set_time(s32);
    s32 get_time();
    u8 chk_eat();
    void set_eat();
    void set_no_hit();
    bool chk_explode();
};
}
}
/* HD: emitter callbacks and invalidation fields at GameCube offset minus four. */
struct BombEmitter_view {
    u8 _00[0x5C];
    be<u32> mMaxFrame;
    u8 _60[0x1E4 - 0x60];
    gptr<void> mpEmitterCallBack;
    u8 _1E8[0x254 - 0x1E8];
    be<u32> mFlags;
    void setEmitterCallBackPtr(void* cb) { mpEmitterCallBack = cb; }
    void becomeInvalidEmitter() {
        u32 flags = mFlags;
        mMaxFrame = ~0u;
        mFlags = flags | 1u;
    }
};
WWHD_OFFSET(BombEmitter_view, mpEmitterCallBack, 0x1E4);
WWHD_OFFSET(BombEmitter_view, mFlags, 0x254);
