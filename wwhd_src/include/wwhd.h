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
    cPhs_UNK3_e = 3, /* cPhs_STOP_e: gc_api/SSystem/c_phase.h (several raw units define it locally) */
    cPhs_COMPLEATE_e = 4,
    cPhs_ERROR_e = 5,
};

typedef u32 fpc_ProcID;
enum : u32 { fpcM_ERROR_PROCESS_ID_e = 0xFFFFFFFF };

struct Mtx34 {
    be<f32> m[3][4];
};
WWHD_SIZE(Mtx34, 0x30);

/* Quaternion (dolphin mtx). Assignment is a bit copy of the four words, loads before stores (GHS
 * copies with lfs/stfs pairs, which keep signalling NaNs in the recompiled original) [v lpalm] */
struct Quaternion {
    be<f32> x, y, z, w;
    Quaternion& operator=(const Quaternion& o) {
        u32 s = gabi::ea(&o), d = gabi::ea(this);
        u32 a = gabi::load<u32>(s), b = gabi::load<u32>(s + 4), c = gabi::load<u32>(s + 8), e = gabi::load<u32>(s + 12);
        gabi::store<u32>(d, a);
        gabi::store<u32>(d + 4, b);
        gabi::store<u32>(d + 8, c);
        gabi::store<u32>(d + 12, e);
        return *this;
    }
};
WWHD_SIZE(Quaternion, 0x10);
/* local Quaternion q(x, y, z, w) (autoglue: guest stack slot) */
inline void gabi_construct(Quaternion& q, f32 x, f32 y, f32 z, f32 w) { q.x = x; q.y = y; q.z = z; q.w = w; }

/* GXColor (rgba bytes) */
struct GXColor {
    be<u8> r, g, b, a;
};
typedef GXColor _GXColor;
/* GXColorS10 (signed 16-bit rgba: TEV colour registers) */
struct GXColorS10 {
    be<s16> r, g, b, a;
};

/* opaque guest classes (layouts not needed so far) */
/* J3DModel, J3DModelData, J3DNode, J3DJoint: HD layouts in JSystem/J3DGraph.h */
struct J3DModel;
struct J3DModelData;
struct J3DNode;
struct J3DJoint;
WWHD_OPAQUE(J3DAnmTransform);
WWHD_OPAQUE(J3DAnmTevRegKey);
WWHD_OPAQUE(J3DAnmTextureSRTKey);
WWHD_OPAQUE(J3DAnmTexPattern);
WWHD_OPAQUE(J3DMaterialTable);
WWHD_OPAQUE(cBgD_t);
WWHD_OPAQUE(cBgW);
struct dBgW; /* d/d_bg_s.h */
struct dBgS; /* d/d_bg_s.h */
struct cBgS_PolyInfo; /* d/d_bg_s.h */
struct dSv_info_c; /* d/d_save.h */
WWHD_OPAQUE(dScnKy_env_light_c);
WWHD_OPAQUE(dRes_control_c);
WWHD_OPAQUE(dPath);
struct JPABaseEmitter; /* layout: gc_api/JSystem/JPAEmitter.h */
WWHD_OPAQUE(JKRHeap);
WWHD_OPAQUE(dEvent_manager_c);
struct dEvt_control_c; /* d/d_event.h */
WWHD_OPAQUE(dAttention_c);
struct dVibration_c;  /* d/d_vibration.h */
struct dPa_control_c; /* d/d_particle.h */
WWHD_OPAQUE(cCcS);
WWHD_OPAQUE(dCcMassS_Mng);
WWHD_OPAQUE(dStage_roomControl_c);
WWHD_OPAQUE(dDetect_c);
WWHD_OPAQUE(dDlst_list_c);
WWHD_OPAQUE(dPa_levelEcallBack);
WWHD_OPAQUE(mDoExt_McaMorfCallBack1_c);
WWHD_OPAQUE(mDoExt_McaMorfCallBack2_c);
struct dCamera_c; /* HD layout: d/d_camera.h */
struct msg_class; /* HD layout: gc_api/f_op/f_op_msg.h */
