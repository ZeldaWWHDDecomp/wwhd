// Link water-drop callbacks. Private views retain the measured WWHD layout.
#pragma once
#include "d/actor/d_a_player_main_04_item.h"

struct LinkWaterCallback : LinkItemFollowCallback {
    /* 0x1C */ be<u32> field_0x1C;
};
WWHD_OFFSET(LinkWaterCallback, field_0x1C, 0x1C);
WWHD_SIZE(LinkWaterCallback, 0x20);
struct LinkWaterParticleCallback {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<u32> mbWaterMark;
};
WWHD_OFFSET(LinkWaterParticleCallback, mbWaterMark, 4);
WWHD_SIZE(LinkWaterParticleCallback, 8);
struct LinkWaterEmitter {
    u8 _000[0x1E8];
    /* 0x1E8 */ gptr<LinkWaterParticleCallback> mpParticleCallBack;
    u8 _1EC[0x254 - 0x1EC];
    /* 0x254 */ be<u32> mStatus;
};
WWHD_OFFSET(LinkWaterEmitter, mpParticleCallBack, 0x1E8);
WWHD_OFFSET(LinkWaterEmitter, mStatus, 0x254);
WWHD_SIZE(LinkWaterEmitter, 0x258);
struct LinkWaterApi {
    static void deleteAllParticle(JPABaseEmitter*, u32);
    static BOOL playerOverheadBgCheck();
};
namespace LinkWaterConstants {
enum : u32 {
    daPyFlg1_WATER_DROP = 0x80000,
    ModeFlg_00000001 = 1,
    daPyProc_DEMO_LAST_COMBO_e = 0xD8,
    CL_JNT_NECK_JNT_e = 14,
    CL_JNT_BODY_CHN_e = 2,
    ID_IT_JN_LK_NURE_POTA00 = 0x38,
    ID_AK_JN_LK_NURE_POTA01 = 0x432,
    dStageType_MISC_e = 2,
    IMMORTAL_EMITTER = 0x40,
};
}
inline LinkWaterCallback& LinkWaterDrop(void* callback) {
    return *reinterpret_cast<LinkWaterCallback*>(callback);
}
inline LinkWaterEmitter& LinkWaterEmitterView(JPABaseEmitter* emitter) {
    return *reinterpret_cast<LinkWaterEmitter*>(emitter);
}
inline LinkItemMtxCallback& LinkWaterRain(daPy_lk_c* player, u32 index) {
    return *reinterpret_cast<LinkItemMtxCallback*>(player->m3460 + index * 0x10);
}
