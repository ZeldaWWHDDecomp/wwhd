/* Scene process lifecycle, GameCube names and WWHD layout. Local only. */
#pragma once
#include "gc_api/gc_api.h"
#include "f_pc/f_pc_base.h"

struct scene_method_class;
struct scene_tag_class { create_tag_class base; };
struct scene_process_profile_definition : process_profile_definition {
    gptr<void> nodeSubMethod;
    /* 0x20 */ gptr<scene_method_class> sub_method;
    u8 padding[4];
};
WWHD_OFFSET(scene_process_profile_definition, sub_method, 0x20);
struct scene_class : base_process_class {
    // HD: process-node data follows the base's four-byte virtual destructor slot.
    u8 mProcessNodeData[0x1B0 - 0xB8];
    /* 0x1B0 */ gptr<scene_method_class> mpMtd;
    /* 0x1B4 */ scene_tag_class mScnTg;
};
WWHD_OFFSET(scene_class, mpMtd, 0x1B0);
WWHD_OFFSET(scene_class, mScnTg, 0x1B4);
WWHD_SIZE(scene_class, 0x1C8);

s32 fpcNd_DrawMethod(scene_method_class*, scene_class*);
s32 fpcMtd_Execute(scene_method_class*, scene_class*);
s32 fpcMtd_IsDelete(scene_method_class*, scene_class*);
s32 fpcMtd_Delete(scene_method_class*, scene_class*);
s32 fpcMtd_Create(scene_method_class*, scene_class*);
void fopScnTg_QueueTo(scene_tag_class*);
void fopScnTg_ToQueue(scene_tag_class*);
void fopScnTg_Init(scene_tag_class*, scene_class*);
// HD: the root update entry point needs no explicit object argument.
void mDoHIO_root_update();
