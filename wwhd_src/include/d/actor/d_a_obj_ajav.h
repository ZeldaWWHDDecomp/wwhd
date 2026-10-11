/** daObjAjav::Part_c and Act_c: Jabun cave's six-part barrier.
 * HD: actor members start at 0x3AC; GHS member pointers are eight bytes, so Part_c is 0x90.
 * Pointer parameters retain the HD ABI for GameCube's by-value cXyz/csXyz arguments.
 */
#pragma once
#include "gc_api/gc_api.h"
#include "d/d_cc_d.h"
enum {
    dRes_INDEX_AJAV_BDL_AJAVA_e = 4,
    dRes_INDEX_AJAV_BDL_AJAVB_e,
    dRes_INDEX_AJAV_BDL_AJAVC_e,
    dRes_INDEX_AJAV_BDL_AJAVD_e,
    dRes_INDEX_AJAV_BDL_AJAVE_e,
    dRes_INDEX_AJAV_BDL_AJAVF_e,
    dRes_INDEX_AJAV_DZB_AJAV_e = 12,
};
namespace daObjAjav {
class Act_c;
class Part_c {
  public:
    // HD: GHS stores direct callbacks as delta/index/function words; keep the eight-byte ABI.
    using daObjAjavPartCallback = gabi::ptmf<void (Part_c::*)(Act_c*)>;
    enum : u32 {
        PROC_NONE = 0x02313AA8,
        PROC_DRAW_NORMAL = 0x02313AAC,
        PROC_FALL_0 = 0x02313F24,
        PROC_FALL_1 = 0x023141CC,
        PROC_FLAW = 0x02314460,
        PROC_DRAW_FLASHING_NORMAL = 0x02313E0C,
    };
    void setExeProc(u32 target) {
        mExeProc.d = 0;
        mExeProc.i = -1;
        mExeProc.f = target;
    }
    void setDrawProc(u32 target) {
        mDrawProc.d = 0;
        mDrawProc.i = -1;
        mDrawProc.f = target;
    }
    void no_proc(Act_c* actor);
    void init_data(const cXyz* actorPosition, const cXyz* offset, dKy_tevstr_c* lighting,
                   const cXyz* particleOffset);
    BOOL set_mdl_area(const char* archive, s32 index, u32 flags);
    void init_mtx(const cXyz* position, const csXyz* angle, const cXyz* scale);
    void draw_normal(Act_c* actor);
    void fall_init(const cXyz* rate, const csXyz* rotationRate, s16 limit, u16 trigger);
    void set_fall_mtx(const cXyz* position, const csXyz* angle);
    void set_flaw_mtx(const cXyz* position, const csXyz* angle);
    void flaw(Act_c* actor);
    void make_hamon(cXyz* position, f32 size);
    void make_fall_rock(BOOL increased);
    void fall_0(Act_c* actor);
    void fall_1(Act_c* actor);
    void draw_flashing(Act_c* actor);
    void draw_flashing_normal(Act_c* actor);
    /* 0x00 */ cXyz mCenterPos;
    /* 0x0C */ cXyz mCenterToOriginTrans;
    /* 0x18 */ cXyz mRockDisplacement;
    /* 0x24 */ cXyz mRockDisplacementRate;
    /* 0x30 */ cXyz mFlawPos;
    /* 0x3C */ csXyz mAccumulatedRotation;
    /* 0x42 */ csXyz mRotationSpeed;
    /* 0x48 */ cXyz mSePos;
    /* 0x54 */ be<u16> mTimerTrigger;
    /* 0x56 */ be<u16> mTimer;
    /* 0x58 */ be<s16> mAngleLimit;
    /* 0x5A */ be<u8> mbHasSplashed;
    /* 0x5B */ u8 mB8[0x11];
    /* 0x6C */ cXyz mRockParticlePos;
    /* 0x78 */ gptr<J3DModel> mpModel;
    /* 0x7C */ gptr<dKy_tevstr_c> mpTevStr;
    /* 0x80 */ daObjAjavPartCallback mExeProc;
    /* 0x88 */ daObjAjavPartCallback mDrawProc;
};
class Act_c : public fopAc_ac_c {
  public:
    enum { STATUS_MAX = 4, PART_COUNT = 6, ENDLESS_NIGHT = 0xA02 };
    enum Action { ACTION_WAIT, ACTION_EVENT, ACTION_EVENT_END, ACTION_LAST_FALL, ACTION_DONE };
    BOOL check_all_wait();
    BOOL check_end();
    BOOL _draw();
    BOOL create_heap();
    void init_mtx();
    void set_tex();
    BOOL _delete();
    void to_broken();
    BOOL damage_part();
    void set_co_offset();
    void make_hamon2(cXyz* position, f32 size);
    void make_shot_rock();
    void set_hamon(f32 size);
    s32 _create();
    BOOL _execute();
    /* 0x3AC */ be<u32> mSwNo;
    /* 0x3B0 */ request_of_phase_process_class mPhs;
    /* 0x3B8 */ dCcD_Stts mSphStts;
    /* 0x3F4 */ dCcD_Sph mSph;
    /* 0x520 */ cXyz mSphCoOffset;
    /* 0x52C */ dCcD_Stts mCylStts;
    /* 0x568 */ dCcD_Cyl mCyl;
    /* 0x698 */ dCcD_Stts mHintCylStts[2];
    /* 0x710 */ dCcD_Cyl mHintCyls[2];
    /* 0x970 */ Mtx34 mMtx;
    /* 0x9A0 */ u8 m884[12];
    /* 0x9AC */ Part_c mRockParts[6];
    /* 0xD0C */ be<s16> mEventIdx;
    /* 0xD0E */ be<u8> M_status;
    /* 0xD0F */ be<u8> mbResLoaded;
    /* 0xD10 */ be<u8> mActionIdx;
    /* 0xD11 */ u8 mC25;
    /* 0xD12 */ be<u16> mCutsceneEndDelay;
    /* 0xD14 */ be<u8> mHintCylHits;
    /* 0xD15 */ u8 mC29[3];
    /* 0xD18 */ gptr<dBgW> mpBgW;
};
} // namespace daObjAjav
WWHD_OFFSET(daObjAjav::Part_c, mCenterPos, 0x00);
WWHD_OFFSET(daObjAjav::Part_c, mCenterToOriginTrans, 0x0C);
WWHD_OFFSET(daObjAjav::Part_c, mRockDisplacement, 0x18);
WWHD_OFFSET(daObjAjav::Part_c, mRockDisplacementRate, 0x24);
WWHD_OFFSET(daObjAjav::Part_c, mFlawPos, 0x30);
WWHD_OFFSET(daObjAjav::Part_c, mAccumulatedRotation, 0x3C);
WWHD_OFFSET(daObjAjav::Part_c, mRotationSpeed, 0x42);
WWHD_OFFSET(daObjAjav::Part_c, mSePos, 0x48);
WWHD_OFFSET(daObjAjav::Part_c, mTimerTrigger, 0x54);
WWHD_OFFSET(daObjAjav::Part_c, mTimer, 0x56);
WWHD_OFFSET(daObjAjav::Part_c, mAngleLimit, 0x58);
WWHD_OFFSET(daObjAjav::Part_c, mbHasSplashed, 0x5A);
WWHD_OFFSET(daObjAjav::Part_c, mB8, 0x5B);
WWHD_OFFSET(daObjAjav::Part_c, mRockParticlePos, 0x6C);
WWHD_OFFSET(daObjAjav::Part_c, mpModel, 0x78);
WWHD_OFFSET(daObjAjav::Part_c, mpTevStr, 0x7C);
WWHD_OFFSET(daObjAjav::Part_c, mExeProc, 0x80);
WWHD_OFFSET(daObjAjav::Part_c, mDrawProc, 0x88);
WWHD_OFFSET(daObjAjav::Act_c, mSwNo, 0x3AC);
WWHD_OFFSET(daObjAjav::Act_c, mPhs, 0x3B0);
WWHD_OFFSET(daObjAjav::Act_c, mSphStts, 0x3B8);
WWHD_OFFSET(daObjAjav::Act_c, mSph, 0x3F4);
WWHD_OFFSET(daObjAjav::Act_c, mSphCoOffset, 0x520);
WWHD_OFFSET(daObjAjav::Act_c, mCylStts, 0x52C);
WWHD_OFFSET(daObjAjav::Act_c, mCyl, 0x568);
WWHD_OFFSET(daObjAjav::Act_c, mHintCylStts, 0x698);
WWHD_OFFSET(daObjAjav::Act_c, mHintCyls, 0x710);
WWHD_OFFSET(daObjAjav::Act_c, mMtx, 0x970);
WWHD_OFFSET(daObjAjav::Act_c, m884, 0x9A0);
WWHD_OFFSET(daObjAjav::Act_c, mRockParts, 0x9AC);
WWHD_OFFSET(daObjAjav::Act_c, mEventIdx, 0xD0C);
WWHD_OFFSET(daObjAjav::Act_c, M_status, 0xD0E);
WWHD_OFFSET(daObjAjav::Act_c, mbResLoaded, 0xD0F);
WWHD_OFFSET(daObjAjav::Act_c, mActionIdx, 0xD10);
WWHD_OFFSET(daObjAjav::Act_c, mC25, 0xD11);
WWHD_OFFSET(daObjAjav::Act_c, mCutsceneEndDelay, 0xD12);
WWHD_OFFSET(daObjAjav::Act_c, mHintCylHits, 0xD14);
WWHD_OFFSET(daObjAjav::Act_c, mC29, 0xD15);
WWHD_OFFSET(daObjAjav::Act_c, mpBgW, 0xD18);
WWHD_SIZE(daObjAjav::Part_c, 0x90);
WWHD_SIZE(daObjAjav::Act_c, 0xD1C);

struct AjavColorUniform;
using AjavDestructor = void (*)(void*, s32);
using AjavConstructor = void* (*)(void*);
struct ColorFloat {
    be<f32> value[3];
};
void ajavCollisionSet(cCcS*, void*);
void ajavCylinderGeometryCtor(void*);
void ajavAddRotation(csXyz*, const csXyz*);
u32 ajavScaleRotation(csXyz*, f32);
void ajavAddPosition(const cXyz*, cXyz*, const cXyz*);
void ajavSubtractPosition(const cXyz*, cXyz*, const cXyz*);
void ajavScalePosition(const cXyz*, cXyz*, f32);
void ajavNormalizeColor(ColorFloat*, GXColor*);
dBgW* ajavNewBackground(void*, u32, Mtx34*);
void ajavStatusDt(void*, s32);
void ajavCylinderDt(void*, s32);
void ajavSphereDt(void*, s32);
void ajavObjectInfoCtor(void*);
void ajavSphereCtor(void*);
s32 ajavGetStaff(dEvent_manager_c*, const char*, s32, s32);
void ajavCutEnd(dEvent_manager_c*, s32);
s32 ajavGetEvent(dEvent_manager_c*, const char*, s32);
BOOL ajavEventEndCheck(dEvent_manager_c*, s16);
void ajavActorCtor(fopAc_ac_c*);
void ajavActorDt(fopAc_ac_c*, s32);
s32 ajavWaterHeight(cXyz*, be<f32>*);
void ajavSubBgmStop();
void ajavSystemSeStart(u32);
void ajavYrotS(Mtx34*, s16);
void* ajavObjectResource(dRes_control_c*, SafeString*, s32);
void* ajavAllocate(u32);
void ajavConvertColor(ColorFloat*, ColorFloat*, f32);
AjavColorUniform* ajavUniformBuffer(void*, u32);
void ajavConstructArray(void*, u32, u32, AjavConstructor);
void ajavDestroyArray(void*, u32, u32, AjavDestructor, u32, u32);

// HD: bitwise vector transfers retain NaN payloads and signed zero.
struct AjavVectorWords {
    be<u32> x, y, z;
};
WWHD_SIZE(AjavVectorWords, 12);

// HD: partial graphics views expose only members used by this actor.
struct AjavTevBlockVtable {
    u8 reserved[0x3C];
    be<u32> setTevKColor;
    u8 reserved40[0xC];
    be<u32> getTevKColor;
};
struct AjavTevBlock {
    be<u32> reserved;
    gptr<AjavTevBlockVtable> __vtbl;
    GXColor* getTevKColor() { return gabi::call_ptr<GXColor*>(__vtbl->getTevKColor, this, 1); }
    void setTevKColor(GXColor* color) { gabi::call_ptr(__vtbl->setTevKColor, this, 1, color); }
};
struct AjavMaterialUniform {
    be<u32> mFlags;
};
struct AjavMaterial {
    u8 reserved[0x18];
    gptr<AjavTevBlock> mTevBlock;
    u8 reserved1C[0xA0 - 0x1C];
    AjavMaterialUniform mUniform;
};
struct AjavJoint {
    u8 reserved[0x10];
    gptr<AjavMaterial> mMesh;
};
struct AjavModelData {
    u8 reserved[8];
    gptr<AjavJoint> mJointNodePointer;
    u8 reserved0C[0x30 - 0x0C];
    be<u32> mTexture;
    be<u32> mTextureName;
};
struct AjavEmitter {
    u8 reserved[0x244];
    GXColor mGlobalPrmColor;
};
struct AjavColorUniform {
    be<f32> r, g, b, a;
};
struct AjavCylinderShape {
    u8 reserved[0x110];
    be<u32> mAabVtable;
    be<u32> mShapeVtable;
    cM3dGCyl mCyl;
};
struct AjavSinCos {
    be<f32> sine, cosine;
};
struct AjavTrigTable {
    AjavSinCos entries[8192];
};
WWHD_OFFSET(AjavTevBlockVtable, setTevKColor, 0x3C);
WWHD_OFFSET(AjavTevBlockVtable, getTevKColor, 0x4C);
WWHD_OFFSET(AjavMaterial, mUniform, 0xA0);
WWHD_OFFSET(AjavModelData, mTexture, 0x30);
WWHD_OFFSET(AjavEmitter, mGlobalPrmColor, 0x244);
WWHD_OFFSET(AjavCylinderShape, mCyl, 0x118);
WWHD_SIZE(AjavColorUniform, 0x10);

WWHD_OFFSET(AjavMaterial, mTevBlock, 0x18);
WWHD_OFFSET(AjavJoint, mMesh, 0x10);
WWHD_OFFSET(AjavModelData, mJointNodePointer, 8);
WWHD_OFFSET(AjavModelData, mTextureName, 0x34);
