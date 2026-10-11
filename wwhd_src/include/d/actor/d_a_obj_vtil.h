#pragma once
#include "f_op/f_op_actor.h"
#include "m_Do/m_Do_ext.h"
#include "d/d_bg_s.h"
#include "d/d_cc_d.h"
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"

// HD: measured statue layout; tww leaves the member block opaque.
struct daObjVtil_c : fopAc_ac_c {
    BOOL create_heap();
    void init_mtx();
    void init_co();
    void init_bgc();
    void renew_attention_pos();
    void to_wait_mode();
    u32 getPrm(u32 width, u32 shift);
    s32 _create();
    BOOL _delete();
    void set_sound(s32 sound, s32 mode);
    void hit_co();
    BOOL check_sink();
    void make_splash();
    void se_splash();
    void to_sink_mode();
    void make_smoke();
    void se_smoke();
    BOOL check_circle();
    void make_vib();
    BOOL check_sink_end();
    void hit_bg();
    BOOL _execute();
    BOOL _draw();
    void calc_throw(be<f32>* gravityOut, be<f32>* linearOut, be<f32>* angularOut);
    void to_carry_mode();
    void mode_wait();
    void to_throw_mode();
    void mode_carry();
    void mode_throw();
    void mode_sink();
    gptr<J3DModel> mModel;
    request_of_phase_process_class mPhase;
    dBgS_ObjAcch mAcch;
    dBgS_AcchCir mAcchCircle;
    dCcD_Stts mCollisionStatus;
    dCcD_Cyl mCylinder;
    be<u32> mStatueType;
    be<f32> mPreviousVerticalSpeed;
    be<u8> mMode;
    be<u8> mDeleteState;
    be<u8> mFirstGroundHit;
    u8 _733[5];
    LIGHT_INFLUENCE mLight;
};
WWHD_OFFSET(daObjVtil_c, mModel, 0x3AC);
WWHD_OFFSET(daObjVtil_c, mPhase, 0x3B0);
WWHD_OFFSET(daObjVtil_c, mAcch, 0x3B8);
WWHD_OFFSET(daObjVtil_c, mAcchCircle, 0x57C);
WWHD_OFFSET(daObjVtil_c, mCollisionStatus, 0x5BC);
WWHD_OFFSET(daObjVtil_c, mCylinder, 0x5F8);
WWHD_OFFSET(daObjVtil_c, mStatueType, 0x728);
WWHD_OFFSET(daObjVtil_c, mPreviousVerticalSpeed, 0x72C);
WWHD_OFFSET(daObjVtil_c, mMode, 0x730);
WWHD_OFFSET(daObjVtil_c, mDeleteState, 0x731);
WWHD_OFFSET(daObjVtil_c, mFirstGroundHit, 0x732);
WWHD_OFFSET(daObjVtil_c, mLight, 0x738);
WWHD_SIZE(daObjVtil_c, 0x75C);

#include "gc_api/f_op/f_op_actor.h"
struct VtilShapeAttr { u8 mBounds[0x18]; be<u32> mBoundsVtable; be<u32> mShapeVtable; };
WWHD_OFFSET(VtilShapeAttr, mBoundsVtable, 0x18);
WWHD_SIZE(VtilShapeAttr, 0x20);
inline VtilShapeAttr* vtilShape(dCcD_Cyl* cyl) { return reinterpret_cast<VtilShapeAttr*>(cyl->mShapeAttr); }
inline be<u32>& vtilWord(be<f32>& f) { return *reinterpret_cast<be<u32>*>(&f); }
struct VtilZeroWords { be<u32> x, y, z; };
WWHD_SIZE(VtilZeroWords, 12);
struct VtilCameraController { u8 mOpaque; };
struct VtilCamera { u8 mPrefix[0x248]; VtilCameraController mCamera; };
WWHD_OFFSET(VtilCamera, mCamera, 0x248);
struct VtilMathStatics {
    be<f32> mMinAngle, mMaxAngle;
    u8 mSinTable, mAtanTable, mPadding[2];
    be<u32> mZero[4];
};
WWHD_OFFSET(VtilMathStatics, mZero, 0xC);
WWHD_SIZE(VtilMathStatics, 0x1C);
struct VtilGlobalDescriptor { u8 mOpaque[12]; };
WWHD_SIZE(VtilGlobalDescriptor, 12);
struct VtilGlobalDescriptors { VtilGlobalDescriptor mEntries[3]; };
WWHD_SIZE(VtilGlobalDescriptors, 36);
struct VtilCylSource { u8 mOpaque[0x44]; };
// HD: GHS member pointers use an eight-byte descriptor, including the virtual form.
struct VtilAction {
    be<s16> mThisDelta, mSelector;
    be<u32> mFunction;
    void invoke(daObjVtil_c* actor) const {
        u32 adjusted = gabi::ea(actor) + s32(mThisDelta);
        s32 selector = mSelector;
        u32 fn;
        if (selector < 0) fn = mFunction;
        else {
            u32 vtable = gabi::load<u32>(adjusted + s32(s16(u32(mFunction))));
            fn = gabi::load<u32>(vtable + selector * 8 + 4);
        }
        gabi::call_ptr(fn, gabi::at<daObjVtil_c>(adjusted));
    }
};
WWHD_SIZE(VtilAction, 8);
enum VtilVtables : u32 {
    VT_ACTOR=0x100331BC, VT_ACCH_CHECK=0x1003318C, VT_ACCH_GROUP=0x1003319C,
    VT_ACCH_POLYGON=0x100331AC, VT_STATUS=0x1004AE88, VT_GLOBAL_STATUS=0x1004AEC0,
    VT_SHAPE=0x100015A8, VT_BOUNDS=0x1003317C, VT_CYLINDER=0x1004B150,
    VT_CYL_HIT=0x1004B108, VT_CYL_SHAPE=0x1004B160, VT_ARCHIVE=0x10033164,
};
enum VtilMode { MODE_WAIT, MODE_CARRY, MODE_THROW, MODE_SINK, MODE_MAX };
void vtilForceLockOff(VtilCameraController*, u32);
J3DModelData* vtilGetResource(void*, SafeString*, u32);
BOOL vtilIsStageTbox(u32, s32);
void dKy_Sound_set(cXyz*, s32, u32, s32);
void vtilHitSeStart(cXyz*, s32, dCcD_GObjInf*, u32);
void vtilHitEffHibana(fopAc_ac_c*, dCcD_Cyl*);
void vtilMakeLandEffect(fopAc_ac_c*, dBgS_GndChk*, f32);
// HD: full-FPR result adapters for this verified call chain.
f64 vtilSqrtRegister(f32);
f64 vtilSquareMagRegister(const cXyz*);
void vtilInitSinTable(u8*);
void vtilInitAtanTable(u8*);
void vtilAcchConstruct(dBgS_Acch*);
void vtilStatusConstruct(dCcD_Stts*);
void vtilGlobalStatusConstruct(be<u32>*);
void vtilColliderConstruct(dCcD_GObjInf*);
void vtilCylinderConstruct(cM3dGCyl*);
void vtilCircleDestruct(cM3dGCir_l*, s32);
void vtilAcchDestruct(dBgS_Acch*, s32);
void vtilActorDestruct(fopAc_ac_c*, s32);

void vtilMoveCAtTg(dCcD_Cyl*, cXyz*);
