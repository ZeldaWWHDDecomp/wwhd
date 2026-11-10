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
    /* 0x3B4 */ gptr<J3DModel> mpModel;  /* was m298: create_heap 02395298 creates the J3DModel; _create 023958E8 takes its base matrix as the actor matrix; set_mtx 02395564 / _draw 02395ED4 use it */
    /* 0x3B8 */ mDoExt_btkAnm mBtkAnm;  /* was m29C: create_heap 02395298 inits the btk animation; _execute 02395CA4 plays it; _draw 02395ED4 enters it */
    /* 0x42C */ mDoExt_bckAnm mBckAnm;  /* was m2B0: create_heap 02395298 inits the bck animation; _execute 02395CA4 plays it; _draw 02395ED4 enters it */
    /* 0x4B8 */ gptr<dBgW> mpBgW;  /* was m2C0: create_heap 02395298 creates the dBgW; _create 023958E8 registers it (dComIfG_Bgsp()->Regist); _delete 02395C40 releases it */
    /* 0x4BC */ dCcD_Tri mTri[8];  /* was m2C4: init_cc 02395758 sets the 8 dCcD_Tri from M_tri_src; set_cc_pos 02395648 places them; set_cc 0239620C registers them; chk_light 0239625C tests ChkTgHit */
    /* 0xF3C */ dCcD_Stts mStts[8];  /* was mD44: init_cc 02395758: Init(0xFF, 0xFF, this) and m2C4[i].SetStts(&mD44[i]) */
    /* 0x111C */ be<s32> mType;  /* was mF24: _create 023958E8: prm PRM_TYPE; 0 starts the norm (sun/moon) modes, else the active modes that also need switch 2 */
    /* 0x1120 */ be<s32> mMode;  /* was mF28: mode_norm_sun_init 02395830 / mode_norm_moon_init 0239583C / mode_active_sun_init 023958C8 / mode_active_moon_init 023958D4 set MODE_*; _execute 02395CA4 calls mode_proc[mF28] */
    /* 0x1124 */ be<s16> mLightTimer;  /* was mF2C: mode_norm_moon_init 0239583C / mode_active_moon_init 023958D4 set 20; mode_norm_moon 02396370 / mode_active_moon 0239657C count it down while chk_light, reset 20 otherwise; 0 turns the switch on */
    u8 _1126[2];
    /* 0x1128 */ be<f32> mPower;  /* was mF30: power_up 023962F8 / power_down 02396334 chase it to 1 / 0 by 1/30; _create 023958E8 sets 1 or 0 by the switch; _draw 02395ED4 alpha = 255 * it; mode_active_sun 0239666C sound while > 0.5 */
    u8 _112c[4];
    /* 0x1130 */ Mtx34 mBgMtx;  /* was mF38: _create 023958E8 copies the position/angle/scale matrix here; create_heap 02395298 passes it to the dBgW set (swlightSetBackground) */
};
using ModeFunc = gabi::ptmf<void (Act_c::*)()>;
} // namespace daObjSwlight
WWHD_OFFSET(daObjSwlight::Act_c, mpModel, 0x3B4);
WWHD_OFFSET(daObjSwlight::Act_c, mpBgW, 0x4B8);
WWHD_OFFSET(daObjSwlight::Act_c, mTri, 0x4BC);
WWHD_OFFSET(daObjSwlight::Act_c, mMode, 0x1120);
WWHD_OFFSET(daObjSwlight::Act_c, mBgMtx, 0x1130);
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
WWHD_OFFSET(daObjSwlight::Act_c, mpModel, 0x3B4);
WWHD_OFFSET(daObjSwlight::Act_c, mBtkAnm, 0x3B8);
WWHD_OFFSET(daObjSwlight::Act_c, mBckAnm, 0x42C);
WWHD_OFFSET(daObjSwlight::Act_c, mpBgW, 0x4B8);
WWHD_OFFSET(daObjSwlight::Act_c, mTri, 0x4BC);
WWHD_OFFSET(daObjSwlight::Act_c, mStts, 0xF3C);
WWHD_OFFSET(daObjSwlight::Act_c, mType, 0x111C);
WWHD_OFFSET(daObjSwlight::Act_c, mMode, 0x1120);
WWHD_OFFSET(daObjSwlight::Act_c, mLightTimer, 0x1124);
WWHD_OFFSET(daObjSwlight::Act_c, mPower, 0x1128);
WWHD_OFFSET(daObjSwlight::Act_c, mBgMtx, 0x1130);
enum SwlightMode { MODE_NORM_MOON, MODE_NORM_SUN, MODE_ACTIVE_MOON, MODE_ACTIVE_SUN };
enum { JA_SE_OBJ_LIGHT_SW_ON = 0x6962, JA_SE_OBJ_LIGHT_SW_SUNSHINE = 0x7029 };
enum {
    dRes_INDEX_MMIRROR_BDL_MSUSW_e = 10,
    dRes_INDEX_MMIRROR_BTK_MSUSW_e = 15,
    dRes_INDEX_MMIRROR_BCK_MSUSW_e = 6,
    dRes_INDEX_MMIRROR_DZB_MSUSW_e = 20
};
