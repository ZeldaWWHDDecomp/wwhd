/* Creation queue tags. GameCube layout unchanged in HD. */
#pragma once
#include "bindings.h"
#include "SSystem/SComponent/c_list.h"
struct create_tag { create_tag_class base; };
WWHD_OFFSET(create_tag, base, 0);
WWHD_SIZE(create_tag, 0x14);
// HD: the queue wrapper forwards the complete addition result.
u32 cTg_Addition(node_list_class*, create_tag_class*);
void cTg_SingleCut(create_tag_class*);
void cTg_Create(create_tag_class*, void*);
struct CreateTagHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    u8 firstObject;
    u8 secondObject;
    u8 mPadding[2];
    be<u32> zero[4];
};
struct CreateTagPiPair { be<f32> negativePi; be<f32> positivePi; };
WWHD_OFFSET(CreateTagHeaderStatics, zero, 0xC);
WWHD_SIZE(CreateTagHeaderStatics, 0x1C);
WWHD_SIZE(CreateTagPiPair, 8);
void CreateTagFirstHeaderObject_ct(u8*);
void CreateTagSecondHeaderObject_ct(u8*);
