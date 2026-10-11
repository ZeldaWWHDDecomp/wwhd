/* Camera creation/parameters; tww framework names and HD register widths. */
#include "f_op/f_op_camera.h"

// HD: a guest object view keeps the four BSS IDs uninitialized until creation.
static fopCamM_IDTableHD l_fopCamM_id;

/* 025DA64C */
u32 fopCamM_GetParam(camera_class* i_this) { return i_this->view.mParameters; }

/* 025DA654 */
fpc_ProcID fopCamM_Create(u32 i_cameraIdx, u32 i_procName, void* i_append) {
    // HD: preserve the camera-index register passed through the layer accessor.
    layer_class* layer = fopCamM_CurrentLayerHD(i_cameraIdx);
    fpc_ProcID id = fopCamM_RequestHD(layer, i_procName, 0, 0, i_append);
    l_fopCamM_id.mID[i_cameraIdx] = id;
    return id;
}

/* 025DA750 */
void fopCamM_Management() {}

/* 025DA754 */
void fopCamM_Init() {}

// HD: compiler-generated common-header initialization for this TU.
/* 025DA6BC */
static void __sinit_f_op_camera_mng_cpp() {
    sinit_header_statics_rodata(0x10487548, 0x101F336C, 0x10487564, 0x10057964);
}

/* WWHD glue (tools/verify/autoglue.py)
data l_fopCamM_id 10487554
ext 025DED64 layer_class* fopCamM_CurrentLayerHD(u32)
ext 025E14A8 fpc_ProcID fopCamM_RequestHD(layer_class*, u32, u32, u32, void*)
*/
