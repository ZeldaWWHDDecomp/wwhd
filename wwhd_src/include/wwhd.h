/* Common base for WWHD source (see wwhd_src/README.md). */
#pragma once
#include <cstddef>

#include "gabi.h"

using gabi::be;
using gabi::gfn;
using gabi::gptr;

/* layout checks: offsetof on layout structs (they inherit, so they are not standard layout) */
#define WWHD_OFFSET(type, field, off) static_assert(offsetof(type, field) == (off), #type "::" #field)
#define WWHD_SIZE(type, size) static_assert(sizeof(type) == (size), "sizeof " #type)

/* opaque guest objects: only ever handled through pointers */
#define WWHD_OPAQUE(name) struct name { u8 _opaque; }

/* guest string literal / .rodata object at a fixed address (GHS does not pool literals: each
 * use has its own address, so the address is part of the source) */
#define STR(addr) gabi::at<const char>(addr)

typedef s32 cPhs_State;
enum {
    cPhs_INIT_e = 0,
    cPhs_LOADING_e = 1,
    cPhs_NEXT_e = 2,
    cPhs_UNK3_e = 3,
    cPhs_COMPLEATE_e = 4,
    cPhs_ERROR_e = 5,
};

typedef u32 fpc_ProcID;
enum : u32 { fpcM_ERROR_PROCESS_ID_e = 0xFFFFFFFF };

struct Mtx34 {
    be<f32> m[3][4];
};
WWHD_SIZE(Mtx34, 0x30);

/* GXColor (rgba bytes) */
struct GXColor {
    be<u8> r, g, b, a;
};
typedef GXColor _GXColor;

/* opaque guest classes (layouts not needed so far) */
WWHD_OPAQUE(J3DModel);
WWHD_OPAQUE(J3DModelData);
WWHD_OPAQUE(J3DAnmTransform);
WWHD_OPAQUE(J3DAnmTevRegKey);
WWHD_OPAQUE(J3DAnmTextureSRTKey);
WWHD_OPAQUE(J3DAnmTexPattern);
WWHD_OPAQUE(J3DMaterialTable);
WWHD_OPAQUE(J3DNode);
WWHD_OPAQUE(J3DJoint);
WWHD_OPAQUE(cBgD_t);
WWHD_OPAQUE(cBgW);
WWHD_OPAQUE(dBgW);
WWHD_OPAQUE(dBgS);
WWHD_OPAQUE(cBgS_PolyInfo);
WWHD_OPAQUE(dSv_info_c);
WWHD_OPAQUE(dScnKy_env_light_c);
WWHD_OPAQUE(dRes_control_c);
WWHD_OPAQUE(dPath);
WWHD_OPAQUE(JPABaseEmitter);
WWHD_OPAQUE(JKRHeap);
WWHD_OPAQUE(dEvent_manager_c);
WWHD_OPAQUE(dEvt_control_c);
WWHD_OPAQUE(dAttention_c);
WWHD_OPAQUE(dVibration_c);
WWHD_OPAQUE(dPa_control_c);
WWHD_OPAQUE(cCcS);
WWHD_OPAQUE(dCcMassS_Mng);
WWHD_OPAQUE(dStage_roomControl_c);
WWHD_OPAQUE(dDetect_c);
WWHD_OPAQUE(dDlst_list_c);
WWHD_OPAQUE(dPa_levelEcallBack);
WWHD_OPAQUE(mDoExt_McaMorfCallBack1_c);
WWHD_OPAQUE(mDoExt_McaMorfCallBack2_c);
struct dCamera_c; /* HD layout: d/d_camera.h */
WWHD_OPAQUE(msg_class);
