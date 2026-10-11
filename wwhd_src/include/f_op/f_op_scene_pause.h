/* Scene pause support, WWHD layout. */
#pragma once
#include "bindings.h"
#include "f_pc/f_pc_base.h"

struct layer_class {
    u8 _00[0x18];
    /* 0x18 */ gptr<base_process_class> mpPcNode;
};
WWHD_OFFSET(layer_class, mpPcNode, 0x18);
struct scene_class : base_process_class {};

int fopScnPause_Enable(scene_class*);
int fopScnPause_Disable(scene_class*);
void fpcM_PauseEnable(base_process_class*, u32);
void fpcM_PauseDisable(base_process_class*, u32);
s32 fpcM_IsPause(base_process_class*, u32);
s32 fpcEx_IsExist(s32);

/* HD: this translation unit's header objects and ordered zero-initialization. */
struct ScenePauseHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    u8 firstObject;
    u8 secondObject;
    u8 padding[2];
    be<u32> zero[4];
};
struct ScenePausePiPair {
    be<f32> negativePi;
    be<f32> positivePi;
};
WWHD_OFFSET(ScenePauseHeaderStatics, zero, 0xC);
WWHD_SIZE(ScenePauseHeaderStatics, 0x1C);
void ScenePauseFirstHeaderObject_ct(u8*);
void ScenePauseSecondHeaderObject_ct(u8*);
