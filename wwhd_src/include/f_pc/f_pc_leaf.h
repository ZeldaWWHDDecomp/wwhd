/* Leaf-process support. */
#pragma once
#include "bindings.h"
#include "gc_api/f_pc/f_pc.h"

struct process_method_class {
    gfn mpCreateFunc;
    gfn mpDeleteFunc;
    gfn mpExecuteFunc;
    gfn mpIsDeleteFunc;
};
struct leafdraw_method_class {
    process_method_class base;
    /* 0x10 */ gfn mpDrawFunc;
};
WWHD_SIZE(leafdraw_method_class, 0x14);
WWHD_OFFSET(leafdraw_method_class, mpDrawFunc, 0x10);
/* HD: the shared leaf layout stores mpDrawMtd as a guest address. */
inline leafdraw_method_class* fpcLf_getMethods(leafdraw_class* leaf) {
    return gabi::at<leafdraw_method_class>(leaf->mpDrawMtd);
}
s32 fpcDwPi_Get(be<s16>*);
void fpcDwPi_Init(be<s16>*, s32);
BOOL fpcMtd_Method(u32, void*);
s32 fpcBs_MakeOfType(be<s32>*);

/* HD: this translation unit's two header objects and pi constants. */
struct LeafHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    u8 firstObject;
    u8 secondObject;
    u8 padding[2];
    be<u32> zero[4];
};
struct LeafPiPair { be<f32> negativePi; be<f32> positivePi; };
WWHD_SIZE(LeafHeaderStatics, 0x1C);
WWHD_OFFSET(LeafHeaderStatics, zero, 0xC);
void LeafFirstHeaderObject_ct(u8*);
void LeafSecondHeaderObject_ct(u8*);
