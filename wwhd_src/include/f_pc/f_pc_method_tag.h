/* Method queue tags. */
#pragma once
#include "bindings.h"
#include "SSystem/SComponent/c_tag.h"

struct process_method_tag_class {
    /* 0x00 */ create_tag_class mCreateTag;
    /* 0x14 */ gfn mpFunc;
    /* 0x18 */ gptr<void> mpMthdData;
};
WWHD_OFFSET(process_method_tag_class, mpFunc, 0x14);
WWHD_OFFSET(process_method_tag_class, mpMthdData, 0x18);
WWHD_SIZE(process_method_tag_class, 0x1C);
