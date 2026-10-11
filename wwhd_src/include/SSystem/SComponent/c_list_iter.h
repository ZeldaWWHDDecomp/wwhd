/* List iteration. */
#pragma once
#include "SSystem/SComponent/c_list.h"

// HD: callbacks remain guest code addresses.
s32 cLsIt_Method(node_list_class*, u32, void*);
void* cLsIt_Judge(node_list_class*, u32, void*);
s32 cNdIt_Method(node_class*, u32, void*);
void* cNdIt_Judge(node_class*, u32, void*);

// HD: unidentified adjacent translation unit, three-word constructor.
struct hd_word3 {
    be<u32> first;
    be<u32> second;
    be<u32> third;
};
WWHD_OFFSET(hd_word3, first, 0);
WWHD_OFFSET(hd_word3, second, 4);
WWHD_OFFSET(hd_word3, third, 8);
WWHD_SIZE(hd_word3, 0xC);
hd_word3* hd_word3_ctor(hd_word3*, u32, u32, u32);
void* hd_allocate(u32);
