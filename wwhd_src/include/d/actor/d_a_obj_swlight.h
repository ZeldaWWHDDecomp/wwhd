/* Moon/sun light switch. HD actor base grows by0x11C and the animation wrappers
 * move the GameCube collision/state fields by0x1F8. */
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor_mng.h"
namespace daObjSwlight {
struct Act_c : fopAc_ac_c {
    enum Prm_e {
        PRM_SWSAVE_W = 8,
        PRM_SWSAVE_S = 0,
        PRM_SWSAVE2_W = 8,
        PRM_SWSAVE2_S = 8,
        PRM_TYPE_W = 1,
        PRM_TYPE_S = 16
    };
    void set_mtx();
    void init_mtx();
    void init_cc();
    void init_eye_pos();
    void mode_norm_sun_init();
    void mode_norm_moon_init();
    bool is_switch2();
    void mode_active_sun_init();
    void mode_active_moon_init();
    void set_cc();
    bool chk_light();
    bool power_up();
    bool power_down();
    void mode_norm_moon();
    void mode_norm_sun();
    void mode_active_moon();
    void mode_active_sun();
    void set_cc_pos();
    bool create_heap();
    bool _delete();
    bool _execute();
    s32 _create();
    bool _draw();
    static const char M_arcname[];
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> m298;
    /* 0x3B8 */ mDoExt_btkAnm m29C;
    /* 0x42C */ mDoExt_bckAnm m2B0;
    /* 0x4B8 */ gptr<dBgW> m2C0;
    /* 0x4BC */ dCcD_Tri m2C4[8];
    /* 0xF3C */ dCcD_Stts mD44[8];
    /* 0x111C */ be<s32> mF24;
    /* 0x1120 */ be<s32> mF28;
    /* 0x1124 */ be<s16> mF2C;
    u8 _1126[2];
    /* 0x1128 */ be<f32> mF30;
    u8 _112c[4];
    /* 0x1130 */ Mtx34 mF38;
};
using ModeFunc = gabi::ptmf<void (Act_c::*)()>;
} // namespace daObjSwlight
WWHD_OFFSET(daObjSwlight::Act_c, m298, 0x3B4);
WWHD_OFFSET(daObjSwlight::Act_c, m2C0, 0x4B8);
WWHD_OFFSET(daObjSwlight::Act_c, m2C4, 0x4BC);
WWHD_OFFSET(daObjSwlight::Act_c, mF28, 0x1120);
WWHD_OFFSET(daObjSwlight::Act_c, mF38, 0x1130);
WWHD_SIZE(daObjSwlight::Act_c, 0x1160);

struct SwlightBckCache {
    /* 0x00 */ J3DFrameCtrl mFrameCtrl;
    /* 0x10 */ be<u32> mVtable;
    /* 0x14 */ u8 mTransformCache[0x34];
    /* 0x48 */ be<u32> mCacheVtable;
    /* 0x4C */ u8 _4C[0xC];
    /* 0x58 */ be<u32> mCacheData;
    /* 0x5C */ u8 _5C[0x20];
    /* 0x7C */ be<u32> mCacheCount;
    /* 0x80 */ be<u32> mData;
    /* 0x84 */ be<u32> mResource;
    /* 0x88 */ be<u32> mFlags;
};
WWHD_OFFSET(SwlightBckCache, mTransformCache, 0x14);
WWHD_OFFSET(SwlightBckCache, mCacheVtable, 0x48);
WWHD_OFFSET(SwlightBckCache, mCacheData, 0x58);
WWHD_OFFSET(SwlightBckCache, mCacheCount, 0x7C);
WWHD_OFFSET(SwlightBckCache, mData, 0x80);
WWHD_OFFSET(SwlightBckCache, mResource, 0x84);
WWHD_OFFSET(SwlightBckCache, mFlags, 0x88);
WWHD_SIZE(SwlightBckCache, 0x8C);

// HD: triangle shape layout within the collision object.
struct SwlightTriangle {
    u8 mPlane[0x10];
    be<u32> mVtable;
    u8 vertices[0x24];
};
WWHD_SIZE(SwlightTriangle, 0x38);
struct SwlightTriShape {
    u8 object[0x110];
    be<u32> mAabVtable, mShapeVtable;
    SwlightTriangle mTriangle;
};
WWHD_OFFSET(SwlightTriShape, mTriangle, 0x118);
WWHD_SIZE(SwlightTriShape, 0x150);
s32 swlightGetReverb(s32);
s32 swlightIsSwitch(dSv_info_c *, s32, s32);
void swlightOnSwitch(dSv_info_c *, s32, s32);
void swlightOffSwitch(dSv_info_c *, s32, s32);
void swlightSetTriangle(dCcD_Tri *, const dCcD_SrcTri *);
s32 swlightDetectLight(dDetect_c *, const cXyz *);
s32 swlightChaseF(be<f32> *, f32, f32);
void swlightSetTrianglePos(SwlightTriangle *, cXyz *, cXyz *, cXyz *);

struct SwlightBackground {
    u8 state[0xA8];
    gfn mCorrect;
};
WWHD_OFFSET(SwlightBackground, mCorrect, 0xA8);
enum : u32 {
    SWLIGHT_ACTOR_VTABLE = 0x1002FE68,
    SWLIGHT_ANM_BASE_VTABLE = 0x1016E54C,
    SWLIGHT_BCK_VTABLE = 0x1002FE40,
    SWLIGHT_CACHE_VTABLE = 0x1016D820
};
void swlightRegisterTriangle(cCcS *, dCcD_Tri *);
dBgW *swlightNewBackground(dBgW *);
s32 swlightSetBackground(dBgW *, cBgD_t *, u32, Mtx34 *);
void swlightConstructFrame(mDoExt_bckAnm *, s32);
void swlightConstructCache(void *);
using SwlightConstructor = void *(*)(void *);
void swlightConstructArray(void *, u32, u32, SwlightConstructor);

struct SwlightResourceHeader {
    u8 header[0x20];
    be<u32> mMaterialMode;
};
WWHD_OFFSET(SwlightResourceHeader, mMaterialMode, 0x20);
struct SwlightFloatColor {
    be<f32> r, g, b, a;
};
WWHD_SIZE(SwlightFloatColor, 16);
struct SwlightRgba {
    be<u8> r, g, b, a;
};
WWHD_SIZE(SwlightRgba, 4);
struct SwlightTevVtable {
    u8 _00[0x3C];
    gfn setKonstColor;
    u8 _40[12];
    gfn getKonstColor;
};
WWHD_OFFSET(SwlightTevVtable, setKonstColor, 0x3C);
WWHD_OFFSET(SwlightTevVtable, getKonstColor, 0x4C);
WWHD_SIZE(SwlightTevVtable, 0x50);
struct SwlightTevBlock {
    u8 _00[4];
    gptr<SwlightTevVtable> mVtable;
};
WWHD_OFFSET(SwlightTevBlock, mVtable, 4);
WWHD_SIZE(SwlightTevBlock, 8);
struct SwlightMaterial {
    gptr<SwlightResourceHeader> mData;
    gptr<SwlightMaterial> mNext;
    gptr<J3DShape> mShape;
    u8 _0C[12];
    gptr<SwlightTevBlock> mTevBlock;
    u8 _1C[0x84];
    be<u32> mFlags;
};
WWHD_OFFSET(SwlightMaterial, mNext, 4);
WWHD_OFFSET(SwlightMaterial, mShape, 8);
WWHD_OFFSET(SwlightMaterial, mTevBlock, 0x18);
WWHD_OFFSET(SwlightMaterial, mFlags, 0xA0);
WWHD_SIZE(SwlightMaterial, 0xA4);

struct SwlightJoint {
    u8 header[0x10];
    gptr<SwlightMaterial> mMesh;
};
WWHD_OFFSET(SwlightJoint, mMesh, 0x10);
void swlightSetMaterialMode(void *, u32);
void swlightNormalizeColor(SwlightFloatColor *, SwlightRgba *);
void swlightConvertColor(SwlightFloatColor *, SwlightFloatColor *, f32);
SwlightFloatColor *swlightGetUniform(be<u32> *, s32);
void swlightMaterialEntry(J3DModel *, SwlightMaterial *, u32);

// HD: this TU's inline collision vtables.
enum : u32 {
    SWLIGHT_SHAPE_BASE_VTABLE = 0x100015A8,
    SWLIGHT_AAB_VTABLE = 0x1002FE30,
    SWLIGHT_TRI_VTABLE = 0x1004B010,
    SWLIGHT_TRI_GEOMETRY_VTABLE = 0x1004B058,
    SWLIGHT_TRI_SHAPE_VTABLE = 0x1004B068
};
void swlightRegister(be<u32> *);
void swlightInitChar(be<u8> *);
void swlightInitByte(be<u8> *);
void swlightFree(void *);
void *swlightAllocate(u32);
void swlightConstructGObj(dCcD_Tri *);
void swlightConstructTriangle(SwlightTriangle *);
void swlightDestroyArray(void *, u32, u32, u32, u32, u32);
void swlightDestroyBckCache(be<u32> *, s32);
void swlightDestroyActor(fopAc_ac_c *, s32);

BOOL swlightCreateHeapResult(daObjSwlight::Act_c *);
BOOL swlightDeleteResult(daObjSwlight::Act_c *);
BOOL swlightExecuteResult(daObjSwlight::Act_c *);
BOOL swlightDrawResult(daObjSwlight::Act_c *);

void swlightSetCullSphere(fopAc_ac_c *, f32, f32, f32, f32);

// HD: array teardown receives guest-code addresses of shared destructors.
enum : u32 { SWLIGHT_STTS_DESTRUCTOR = 0x02515860, SWLIGHT_TRI_DESTRUCTOR = 0x025159F8 };

WWHD_OFFSET(daObjSwlight::Act_c, mPhase, 0x3AC);
WWHD_OFFSET(daObjSwlight::Act_c, m298, 0x3B4);
WWHD_OFFSET(daObjSwlight::Act_c, m29C, 0x3B8);
WWHD_OFFSET(daObjSwlight::Act_c, m2B0, 0x42C);
WWHD_OFFSET(daObjSwlight::Act_c, m2C0, 0x4B8);
WWHD_OFFSET(daObjSwlight::Act_c, m2C4, 0x4BC);
WWHD_OFFSET(daObjSwlight::Act_c, mD44, 0xF3C);
WWHD_OFFSET(daObjSwlight::Act_c, mF24, 0x111C);
WWHD_OFFSET(daObjSwlight::Act_c, mF28, 0x1120);
WWHD_OFFSET(daObjSwlight::Act_c, mF2C, 0x1124);
WWHD_OFFSET(daObjSwlight::Act_c, mF30, 0x1128);
WWHD_OFFSET(daObjSwlight::Act_c, mF38, 0x1130);
enum SwlightMode { MODE_NORM_MOON, MODE_NORM_SUN, MODE_ACTIVE_MOON, MODE_ACTIVE_SUN };
enum { JA_SE_OBJ_LIGHT_SW_ON = 0x6962, JA_SE_OBJ_LIGHT_SW_SUNSHINE = 0x7029 };
enum {
    dRes_INDEX_MMIRROR_BDL_MSUSW_e = 10,
    dRes_INDEX_MMIRROR_BTK_MSUSW_e = 15,
    dRes_INDEX_MMIRROR_BCK_MSUSW_e = 6,
    dRes_INDEX_MMIRROR_DZB_MSUSW_e = 20
};
