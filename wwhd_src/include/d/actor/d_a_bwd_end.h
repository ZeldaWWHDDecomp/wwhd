// Molgera death sequence's private bindings. Shared bwd_class keeps tww field names.
#pragma once
#include "d/actor/d_a_bwd.h"
#include "gc_api/d/d_com_inf_game.h"
#undef l_HIO
#undef eff_col

enum BwdEndResource {
    dRes_INDEX_BWD_BCK_BWD_DEAD1_e = 0xC,
    dRes_INDEX_BWD_BCK_BWD_DEAD2_e = 0xD,
    dRes_INDEX_BWD_BCK_BWD_DEAD3_e = 0xE,
    BWD_JNT_HEAD_e = 0xE
};
enum BwdEndSound {
    JA_SE_LK_B_BOSS_WARP = 0x2888,
    JA_SE_CM_BWD_LAST_HEAD_BACK = 0x5918,
    JA_SE_CM_BWD_LAST_IN_SAND = 0x5919,
    JA_SE_CV_BWD_DIE = 0x48E2,
    JA_SE_CM_BWD_FLYING = 0x5110,
    JA_SE_CM_BWD_HARDEN = 0x591A,
    JA_SE_CM_BWD_LAST_EXPLODE = 0x591B,
    JA_SE_CM_MONS_EXPLODE = 0x5801,
    JA_STRM_BWD_CLEAR = 0xC000001B
};
enum BwdEndParticle {
    ID_IT_ST_BWO_LASTHIT_SMOKE00 = 0xA267,
    ID_IT_SN_BWO_LASTHIT_SUNA00 = 0x8265,
    ID_IT_SN_BWO_LASTHIT_TAIEKI00 = 0x8266,
    ID_IT_SN_BWO_SIBOUBAKUEN00 = 0x825D,
    ID_IT_SN_BWO_SIBOUSUNA00 = 0x825E
};
enum BwdEndEventBit { MOLGERA_TRIALS_CLEAR = 0x3208, UNK_0480 = 0x480 };
enum BwdEndState {
    END_INIT = 0,
    END_ANIMATION = 1,
    END_FLY = 2,
    END_EXPLODE = 3,
    END_DISAPPEAR = 4,
    END_FINISH = 5,
    END_HARDEN = 23
};
// HD: external callbacks are registered separately because ext glue defines no HostFn entry.
using BwdEndJudge = void* (*)(void*, void*);
void* bwdEndKoDeleteSub(void*, void*);
static gabi::HostFn l_koDeleteSymbol((const void*)&bwdEndKoDeleteSub, 0x020F9AA0);
void* fpcM_Search(BwdEndJudge, void*);
void dLib_setNextStageBySclsNum(s32, s32);
void bwdEndSmoke(dPa_smokeEcallBack_bd*);
void bwdEndAnmInit(bwd_class*, s32, f32, u8, f32, s32);
void bwdEndEffOff(bwd_class*);
void bwdEndFlyPosMove(bwd_class*, s16, s16);
// HD: read all four colour bytes before writing the callback colour.
inline void bwdEndSetColor(dPa_smokeEcallBack_bd* cb, const GXColor& col) {
    u8 r = col.r;
    u8 g = col.g;
    u8 b = col.b;
    u8 a = col.a;
    cb->mColor[0] = r;
    cb->mColor[1] = g;
    cb->mColor[2] = b;
    cb->mColor[3] = a;
}
