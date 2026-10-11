/* Scene iteration, WWHD. */
#pragma once
#include "gc_api/gc_api.h"
#include "SSystem/SComponent/c_list_iter.h"

struct SceneIterParams {
    be<u32> mFunc;
    gptr<void> mpData;
};
WWHD_SIZE(SceneIterParams, 8);
// HD: this callback is a guest entry point consumed by cLsIt_Judge.
enum : u32 { cTgIt_JudgeFilter = 0x0201AA08 };
struct SceneIterHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    u8 firstObject;
    u8 secondObject;
    u8 padding[2];
    be<u32> zero[4];
};
struct SceneIterPiPair { be<f32> negativePi; be<f32> positivePi; };
WWHD_OFFSET(SceneIterHeaderStatics, zero, 0xC);
WWHD_SIZE(SceneIterHeaderStatics, 0x1C);
void SceneIterFirstHeaderObject_ct(u8*);
void SceneIterSecondHeaderObject_ct(u8*);
