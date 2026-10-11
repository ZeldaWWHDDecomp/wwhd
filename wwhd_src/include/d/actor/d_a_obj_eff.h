#pragma once
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_particle.h"
namespace daObjEff {
struct Act_c : fopAc_ac_c {
    enum Prm_e { PRM_TYPE_W = 8, PRM_TYPE_S = 0 };
    enum Type_e {
        Type_BARREL_SMOKE_e,
        Type_STOOL_SMOKE_e,
        Type_SKULL_SMOKE_e,
        Type_LAND_SMOKE_e,
        Type_PINECONE_SMOKE_e,
        Type_WOODBOX_SMOKE_e,
        Type_COUNT
    };
    // HD: preserve full-word returns where the GameCube declaration uses bool/void.
    be<s32> mType;
    gptr<dPa_followEcallBack> mParticleCallback;
    BOOL create_heap();
    BOOL eff_set();
    s32 _create();
    void remove();
    s32 _delete();
    void die();
    s32 _execute();
    BOOL create_heap_barrel_smoke();
    BOOL eff_set_barrel_smoke();
    void remove_barrel_smoke();
    u32 die_barrel_smoke();
    BOOL create_heap_stool_smoke();
    BOOL eff_set_stool_smoke();
    void remove_stool_smoke();
    u32 die_stool_smoke();
    BOOL create_heap_skull_smoke();
    BOOL eff_set_skull_smoke();
    void remove_skull_smoke();
    u32 die_skull_smoke();
    BOOL create_heap_land_smoke();
    BOOL eff_set_land_smoke();
    void remove_land_smoke();
    u32 die_land_smoke();
    BOOL create_heap_pinecone_smoke();
    BOOL eff_set_pinecone_smoke();
    void remove_pinecone_smoke();
    u32 die_pinecone_smoke();
    BOOL create_heap_woodBox_smoke();
    BOOL eff_set_woodBox_smoke();
    void remove_woodBox_smoke();
    u32 die_woodBox_smoke();
};
struct BarrelSmokeCB : dPa_smokeEcallBack {
    BarrelSmokeCB();
    be<s32> field_0x20;
    void execute(JPABaseEmitter*);
};
struct StoolSmokeCB : dPa_smokeEcallBack {
    StoolSmokeCB();
    be<s32> field_0x20;
    void execute(JPABaseEmitter*);
};
struct SkullSmokeCB : dPa_smokeEcallBack {
    SkullSmokeCB();
    be<s32> field_0x20;
    void execute(JPABaseEmitter*);
};
struct LandSmokeCB : dPa_smokeEcallBack {
    LandSmokeCB();
};
struct PineconeSmokeCB : dPa_smokeEcallBack {
    PineconeSmokeCB();
    be<s32> field_0x20;
    void execute(JPABaseEmitter*);
};
struct WoodBoxSmokeCB : dPa_smokeEcallBack {
    WoodBoxSmokeCB();
    be<s32> field_0x20;
    void execute(JPABaseEmitter*);
};
} // namespace daObjEff
WWHD_SIZE(daObjEff::Act_c, 0x3B4);
WWHD_OFFSET(daObjEff::Act_c, mType, 0x3AC);
WWHD_OFFSET(daObjEff::Act_c, mParticleCallback, 0x3B0);
WWHD_SIZE(daObjEff::BarrelSmokeCB, 0x24);
WWHD_OFFSET(daObjEff::BarrelSmokeCB, field_0x20, 0x20);
