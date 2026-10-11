/* Deletion queue tags. GameCube layout unchanged in HD. */
#pragma once
#include "bindings.h"
#include "SSystem/SComponent/c_list.h"
struct delete_tag_class {
    /* 0x00 */ create_tag_class base;
    /* 0x14 */ gptr<layer_class> mpLayer;
    /* 0x18 */ be<s16> mTimer;
    u8 mPadding[2];
};
WWHD_OFFSET(delete_tag_class, base, 0);
WWHD_OFFSET(delete_tag_class, mpLayer, 0x14);
WWHD_OFFSET(delete_tag_class, mTimer, 0x18);
WWHD_SIZE(delete_tag_class, 0x1C);
// The callback is a guest function register, not a host function pointer.
using delete_tag_func = u32;
u32 cTg_Addition(node_list_class*, create_tag_class*);
void cTg_SingleCut(create_tag_class*);
void cTg_Create(create_tag_class*, void*);
struct DeleteTagHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    u8 firstObject;
    u8 secondObject;
    u8 mPadding[2];
    be<u32> zero[4];
};
struct DeleteTagPiPair { be<f32> negativePi; be<f32> positivePi; };
WWHD_OFFSET(DeleteTagHeaderStatics, zero, 0xC);
WWHD_SIZE(DeleteTagHeaderStatics, 0x1C);
WWHD_SIZE(DeleteTagPiPair, 8);
void DeleteTagFirstHeaderObject_ct(u8*);
void DeleteTagSecondHeaderObject_ct(u8*);
