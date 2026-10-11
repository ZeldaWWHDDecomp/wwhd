/* Cutscene seagull, WWHD layout. */
#pragma once
#include "gc_api/gc_api.h"
enum { dRes_ID_DEMO_KMM_BMD_KA_e = 0, dRes_ID_DEMO_KMM_BCK_KA_WAIT1_e = 2 };
enum { fpcNm_KAMOME_e = 0xC2 }; // HD: GameCube process ID is 0xC3.
struct dDemo_actor_c {
    enum { ENABLE_TRANS_e = 2, ENABLE_ROTATE_e = 8, ENABLE_ANM_e = 0x20, ENABLE_ANM_FRAME_e = 0x40 };
};
BOOL dDemo_setDemoData(fopAc_ac_c*, u32, mDoExt_McaMorf*, const char*, s32, void*, u32, s8);
fopAc_ac_c* fopAcM_fastCreate(s16, u32, cXyz*, s32, csXyz*, cXyz*, s8, u32, void*);
/* HD: strcmp(dComIfGp_getStartStageName() (0x1047E6B8), name) == 0 through two sead::SafeString
 * temporaries (this TU's assureTerminationImpl_ 02124510, called directly and through the vtable)
 * (as d_a_npc_bmsw's startStageIs) */
static inline bool startStageIs(const char* name) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> b;
    a->__vtbl = gc_tu.safeString;
    b->__vtbl = gc_tu.safeString;
    a->mStringTop = gabi::guest_addr(name);
    b->mStringTop = 0x1047E6B8;
    gabi::call(0x02124510, a.get());
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b.get());
    u32 s2 = b->mStringTop;
    if (s1 == s2)
        return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c1 = gabi::load<u8>(s1 + i);
        u8 c2 = gabi::load<u8>(s2 + i);
        if (c1 != c2)
            return false;
        if (c1 == 0)
            return true;
    }
    return false;
}

struct daDemo_Kmm_c : fopAc_ac_c {
    daDemo_Kmm_c() {
        fopAc_ac_c_ct(this);
        __vtbl = 0x1000DC4C; // HD: actor vtable.
    }
    static const char M_arcname[];
    BOOL draw();
    BOOL execute();
    void setAction(u8 idx) { unk_29C = idx; }

    BOOL CreateHeap();
    void calcMtx();
    void setAnime(s32, s32, f32, f32);
    BOOL CreateInit();
    cPhs_State create();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3B8 */ be<u8> unk_29C;
    /* 0x3B9 */ u8 _3B9;
    /* 0x3BA */ be<s16> unk_29E;
    /* 0x3BC */ u8 unk2A0[4];
};
WWHD_OFFSET(daDemo_Kmm_c, unk_29E, 0x3BA);
WWHD_SIZE(daDemo_Kmm_c, 0x3C0);

/* HD: contiguous first-use guard/value storage; preserve writes before initialization. */
struct DemoKmmEnv_view {
    void setLightTevColorType(J3DModel* model, dKy_tevstr_c* tev) {
        ::setLightTevColorType((dScnKy_env_light_c*)this, model, tev);
    }
};
struct DemoKmmStatics {
    be<u32> guardUnused1;
    be<s32> unused1;
    be<u32> guardUnused2;
    be<s32> unused2;
    be<u32> guardAction;
    gfn l_action;
    be<u32> guardSpawnPos;
    cXyz spawnPos[3];
    void runAction(daDemo_Kmm_c* actor) { gabi::call_ptr(l_action.get(), actor); }
};
WWHD_OFFSET(DemoKmmStatics, l_action, 0x14);
WWHD_OFFSET(DemoKmmStatics, spawnPos, 0x1C);
