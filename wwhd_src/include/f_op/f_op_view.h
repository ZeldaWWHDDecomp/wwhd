/* View processes, GameCube names and WWHD layout. Local only. */
#pragma once
#include "gc_api/gc_api.h"
#include "f_pc/f_pc_leaf.h"
struct view_process_profile_definition : leaf_process_profile_definition {
    /* 0x24 */ gptr<leafdraw_method_class> sub_method;
    /* 0x28 */ be<u8> mViewMode;
};
struct view_class : leafdraw_class {
    /* 0xC4 */ gptr<leafdraw_method_class> sub_method;
    /* 0xC8 */ be<u8> mViewMode;
};
// HD: the virtual destructor slot shifts fields four bytes after the process base.
WWHD_OFFSET(view_class, sub_method, 0xC4);
WWHD_OFFSET(view_class, mViewMode, 0xC8);
WWHD_OFFSET(view_process_profile_definition, sub_method, 0x24);
WWHD_OFFSET(view_process_profile_definition, mViewMode, 0x28);
s32 fpcLf_DrawMethod(leafdraw_method_class*, view_class*);
struct ViewHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    u8 firstObject;
    u8 secondObject;
    u8 padding[2];
    be<u32> zero[4];
};
struct ViewPiPair { be<f32> negativePi; be<f32> positivePi; };
WWHD_OFFSET(ViewHeaderStatics, zero, 0xC);
WWHD_SIZE(ViewHeaderStatics, 0x1C);
void ViewFirstHeaderObject_ct(u8*);
void ViewSecondHeaderObject_ct(u8*);
