/* Scene process manager, WWHD. */
#pragma once
#include "gc_api/gc_api.h"

struct scene_class;
struct SceneSearchKey {
    union {
        be<u32> id;
        be<u16> name;
    };
};
WWHD_SIZE(SceneSearchKey, 4);

// HD: callbacks are guest entry points, never invoked by this unit directly.
enum : u32 {
    fpcSch_JudgeByID = 0x025E1234,
    fpcSch_JudgeForPName = 0x025E121C,
};
enum { SceneRequest_CREATE = 0, SceneRequest_DELETE = 1, SceneRequest_CHANGE = 2 };
enum { fpcNm_INVALID_e = 0x7FFF };

/* HD: common-header objects and copied pi pair, preserving constructor order. */
struct SceneMngHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    u8 firstObject;
    u8 secondObject;
    u8 padding[2];
    be<u32> zero[4];
};
struct SceneMngPiPair {
    be<f32> negativePi;
    be<f32> positivePi;
};
WWHD_OFFSET(SceneMngHeaderStatics, zero, 0xC);
WWHD_SIZE(SceneMngHeaderStatics, 0x1C);

scene_class* fopScnIt_Judge(u32 callback, SceneSearchKey* key);
u32 fopScnRq_Request(u32 kind, scene_class* scene, u32 procName, u32 user, u32 fadeProcName, u32 fadePeekTime, u32 flags);
u32 fopScnRq_ReRequest(u32 id, u32 procName, u32 user);
s32 fopScnRq_Handler();
void SceneMngFirstHeaderObject_ct(u8*);
void SceneMngSecondHeaderObject_ct(u8*);
