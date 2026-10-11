/** Scene Process Pause. */
#include "f_op/f_op_scene_pause.h"

static ScenePauseHeaderStatics l_headerStatics;
static const ScenePausePiPair l_piPair;

/* 025DCA64 */
int fopScnPause_Enable(scene_class* i_scene) {
    if (i_scene) {
        fpcM_PauseEnable(i_scene, 1);
        fpcM_PauseEnable(i_scene, 2);
        return 1;
    }
    return 0;
}

/* 025DCAC4 */
int fopScnPause_Disable(scene_class* i_scene) {
    if (i_scene) {
        base_process_class* parent = i_scene->mLyTg.mpLayer->mpPcNode;
        if (!parent) {
            fpcM_PauseDisable(i_scene, 1);
            fpcM_PauseDisable(i_scene, 2);
        } else if (fpcEx_IsExist((s32)parent->mBsPcId) == 1) {
            if (!fpcM_IsPause(parent, 1)) {
                fpcM_PauseDisable(i_scene, 1);
            }
            if (!fpcM_IsPause(parent, 2)) {
                fpcM_PauseDisable(i_scene, 2);
            }
        }
        return 1;
    }
    return 0;
}

/* 025DCB94 */
static void __sinit_f_op_scene_pause_cpp() {
    // HD: preserve header construction, including zero-store and registration order.
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101F37BC);
    l_headerStatics.negativePi = l_piPair.negativePi;
    l_headerStatics.positivePi = l_piPair.positivePi;
    ScenePauseFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F37C8);
    ScenePauseSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F37D4);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data l_headerStatics 1048A5F8
data l_piPair 10057D90
ext 025DFB20 void fpcM_PauseEnable(base_process_class*, u32)
ext 025DFB24 void fpcM_PauseDisable(base_process_class*, u32)
ext 025DFB1C s32 fpcM_IsPause(base_process_class*, u32)
ext 025DE564 s32 fpcEx_IsExist(s32)
ext 028ED6F8 void ScenePauseFirstHeaderObject_ct(u8*)
ext 028EAB2C void ScenePauseSecondHeaderObject_ct(u8*)
*/
