/* Scene tags, WWHD. */
#pragma once
#include "gc_api/gc_api.h"
#include "SSystem/SComponent/c_list.h"
struct scene_tag_class { create_tag_class base; };
WWHD_SIZE(scene_tag_class, 0x14);
s32 cTg_SingleCut(create_tag_class*);
s32 cTg_Addition(node_list_class*, create_tag_class*);
void cTg_Create(create_tag_class*, void*);
struct SceneTagHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    u8 firstObject;
    u8 secondObject;
    u8 padding[2];
    be<u32> zero[4];
};
struct SceneTagPiPair { be<f32> negativePi; be<f32> positivePi; };
WWHD_OFFSET(SceneTagHeaderStatics, zero, 0xC);
WWHD_SIZE(SceneTagHeaderStatics, 0x1C);
void SceneTagFirstHeaderObject_ct(u8*);
void SceneTagSecondHeaderObject_ct(u8*);
