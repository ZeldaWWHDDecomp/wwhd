/* Wind-column tag. HD adds material uniforms, expands animation and lighting,
 * and duplicates emitter scale data. Guest offsets are */
#pragma once
#include "d/d_cc_d.h"
#include "d/d_level_se.h"
#include "d/d_path.h"
#include "d/d_point_wind.h"
#include "f_op/f_op_kankyo.h"
#include "gc_api/gc_api.h"
namespace WindTagHD {
// HD: this translation unit's emitted constructor vtables.
enum : u32 {
    ActorVtable = 0x10042B0C,
    CollisionStatusVtable = 0x1004AE88,
    GameCollisionStatusVtable = 0x1004AEC0,
    ShapeBaseVtable = 0x100015A8,
    AabVtable = 0x10042AD4,
    HitInfoVtable = 0x1004AF18,
    CapsuleShapeVtable = 0x1004AF70,
    CapsuleVtable = 0x1004AF60,
    BaseAnimationVtable = 0x1016E54C,
    TransformCacheVtable = 0x1016D820,
    BckAnimationVtable = 0x10042AE4
};
struct CapsuleSource {
    be<u32> mFlags, mAtType, mAtPowerWord, mAtSPrm, mTgType, mTgSPrm, mCoSPrm;
    be<u32> mGObjAtEffects, mGObjAtSPrm, mGObjTgEffects, mGObjTgSPrm, mGObjCoSPrm;
    be<f32> mStart[3], mEnd[3], mRadius;
};
WWHD_OFFSET(CapsuleSource, mAtType, 4);
WWHD_OFFSET(CapsuleSource, mGObjAtEffects, 0x1C);
WWHD_OFFSET(CapsuleSource, mStart, 0x30);
WWHD_OFFSET(CapsuleSource, mRadius, 0x48);
struct PlayMass {
    u8 prefix[PLAY_CCMASS];
    u8 mMass[1];
};
WWHD_OFFSET(PlayMass, mMass, PLAY_CCMASS);
WWHD_SIZE(CapsuleSource, 0x4C);
// HD: animation embeds transform cache and bookkeeping after its frame
// controller.
struct BckConstruction {
    J3DFrameCtrl mFrameCtrl;
    be<u32> mVtable;
    u8 mTransformCache[0x34];
    be<u32> mCacheVtable;
    u8 cache[0xC];
    be<u32> mCacheData;
    u8 state[0x20];
    be<u32> mCacheCount, mData, mResource, mFlags;
};
struct Camera {
    u8 prefix[0xDC];
    cXyz mEye;
};
WWHD_OFFSET(BckConstruction, mTransformCache, 0x14);
WWHD_OFFSET(BckConstruction, mCacheVtable, 0x48);
WWHD_OFFSET(BckConstruction, mCacheData, 0x58);
WWHD_OFFSET(BckConstruction, mCacheCount, 0x7C);
WWHD_SIZE(BckConstruction, 0x8C);
WWHD_OFFSET(Camera, mEye, 0xDC);
WWHD_SIZE(Camera, 0xE8);

struct LightTemplate {
    be<f32> mPositionDirection[6];
    be<u8> mColor[4];
    be<s16> mSpot[4];
    be<f32> mAttenuation[8];
};
WWHD_SIZE(LightTemplate, 0x44);
WWHD_OFFSET(LightTemplate, mColor, 0x18);
WWHD_OFFSET(LightTemplate, mSpot, 0x1C);
WWHD_OFFSET(LightTemplate, mAttenuation, 0x24);

struct TevVtable {
    gabi::gfn slots[20];
};
struct TevBlock {
    be<u32> prefix;
    gptr<TevVtable> __vtbl;
};
struct Material {
    u8 prefix[0x18];
    gptr<TevBlock> mpTevBlock;
    u8 parameters[0xA0 - 0x1C];
    be<u32> mUniformFlags;
    u8 tail[0x39C - 0xA4];
};
struct MaterialData {
    u8 prefix[0xC];
    be<u32> mMaterialNum;
    gptr<Material> mpMaterials;
};
struct Model {
    u8 prefix[0x2A];
    be<u16> mMaterialNum;
    u8 matrices[8];
    be<u32> mMatPackets;
    u8 state[0xAC - 0x38];
    gptr<MaterialData> mpModelData;
    u8 user[0xBC - 0xB0];
    cXyz mBaseScale;
};
struct ColorF {
    be<f32> r, g, b, a;
};
struct ColorWords {
    be<u32> rgba[4];
};
WWHD_OFFSET(TevBlock, __vtbl, 4);
WWHD_SIZE(TevVtable, 0x50);
WWHD_SIZE(TevBlock, 8);
WWHD_OFFSET(Material, mpTevBlock, 0x18);
WWHD_OFFSET(Material, mUniformFlags, 0xA0);
WWHD_SIZE(Material, 0x39C);
WWHD_OFFSET(MaterialData, mMaterialNum, 0xC);
WWHD_OFFSET(MaterialData, mpMaterials, 0x10);
WWHD_SIZE(MaterialData, 0x14);
WWHD_OFFSET(Model, mMaterialNum, 0x2A);
WWHD_OFFSET(Model, mMatPackets, 0x34);
WWHD_OFFSET(Model, mpModelData, 0xAC);
WWHD_OFFSET(Model, mBaseScale, 0xBC);
WWHD_SIZE(Model, 0xC8);
WWHD_SIZE(ColorF, 0x10);
WWHD_SIZE(ColorWords, 0x10);

struct CapsuleGeometry {
    cXyz mStart, mEnd;
    be<u32> __vtbl;
    be<f32> mRadius;
};
WWHD_SIZE(CapsuleGeometry, 0x20);
// HD: this actor's capsule exposes its attack vector at the verified offset.
struct Capsule {
    u8 prefix[0x3C];
    be<u32> mHitInfoVtable;
    be<u32> mFlags;
    gptr<dCcD_Stts> mpStts;
    u8 attributes[0x7C - 0x48];
    cXyz mAtVec;
    u8 collision[0x110 - 0x88];
    be<u32> mAabVtable, mShapeVtable;
    CapsuleGeometry mCps;
};
WWHD_OFFSET(Capsule, mpStts, 0x44);
WWHD_OFFSET(Capsule, mAtVec, 0x7C);
WWHD_OFFSET(Capsule, mAabVtable, 0x110);
WWHD_OFFSET(Capsule, mShapeVtable, 0x114);
WWHD_OFFSET(Capsule, mCps, 0x118);
WWHD_SIZE(Capsule, 0x138);

// HD: fields touched by this actor in the expanded particle emitter.
struct Emitter {
    u8 prefix[0x5C];
    be<s32> mMaxFrame;
    u8 state[0x1F0 - 0x60];
    Mtx34 mGlobalRotation;
    cXyz mGlobalScale;
    cXyz mGlobalTranslation;
    cXyz mGlobalParticleScale;
    be<u8> mPrmColor[4];
    u8 colors[0x254 - 0x248];
    be<u32> mFlags;
    u8 counters[0x262 - 0x258];
    be<u8> mDrawTimes;
    u8 padding;
};
WWHD_OFFSET(Emitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(Emitter, mGlobalScale, 0x220);
WWHD_OFFSET(Emitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(Emitter, mGlobalParticleScale, 0x238);
WWHD_OFFSET(Emitter, mPrmColor, 0x244);
WWHD_OFFSET(Emitter, mFlags, 0x254);
WWHD_OFFSET(Emitter, mDrawTimes, 0x262);
WWHD_SIZE(Emitter, 0x264);
} // namespace WindTagHD

enum WindTagSound { JA_SE_OBJ_WIND_TAG = 0x701D, JA_SE_OBJ_WIND_TAG_L = 0x701E };
enum WindTagParticle { ID_AK_SN_AIRFLOWSPLASH00 = 0x8290 };
namespace WindTagHD {
enum TevSlot { SetTevColor = 0x24, GetTevColor = 0x34, SetTevKColor = 0x3C, GetTevKColor = 0x4C };
enum MaterialFlags { TevColorUniform = 0x40, KonstColorUniform = 0x100 };
enum EmitterFlags { StopCreateParticle = 1 };
} // namespace WindTagHD
namespace daWindTag {
struct daWindTag_c : fopAc_ac_c {
    bool _delete();
    BOOL CreateHeap();
    void CreateInit();
    void set_wind_angle();
    cPhs_State _create();
    void set_mtx();
    bool checkSizeSpecialBig();
    void set_wind_se_sub(u32, cXyz*);
    void set_wind_se();
    bool _execute();
    void path_move();
    void set_next_pnt();
    bool _draw();
    void MoveEmitter();

    /* 0x3AC */ dCcD_Stts mStts;
    /* 0x3E8 */ WindTagHD::Capsule mCps;
    /* 0x520 */ PointWind::cM3dGCpsS mCpsS;
    /* 0x53C */ request_of_phase_process_class mPhs;
    /* 0x544 */ gptr<J3DModel> mpModel;
    /* 0x548 */ mDoExt_btkAnm mBtkAnm0;
    /* 0x5BC */ mDoExt_btkAnm mBtkAnm1;
    /* 0x630 */ mDoExt_bckAnm mBckAnm;
    /* 0x6BC */ dPointWind_c mPointWind; /* dPointWind_c (mWind.mStrength at +0x20) */
    /* 0x6EC */ be<u8> mType;
    /* 0x6ED */ u8 _6ED[3];
    /* 0x6F0 */ be<f32> mOffsY;
    /* 0x6F4 */ be<f32> field_0x49c;
    /* 0x6F8 */ gptr<WindTagHD::Emitter> mpEmitter;
    /* 0x6FC */ be<u8> mEfColor[4];
    /* 0x700 */ dKy_tevstr_c mEfTevStr;
    /* 0x8C8 */ cXyz mSePos;
    /* 0x8D4 */ cXyz mTargetPos;
    /* 0x8E0 */ gptr<path_layout::dPath> mpPath;
    /* 0x8E4 */ be<u8> mPathId;
    /* 0x8E5 */ be<s8> mCurPathPoint;
    /* 0x8E6 */ be<s8> mPathPointDir;
    /* 0x8E7 */ be<u8> field_0x577;
    /* 0x8E8 */ be<s32> mSwNo;
    /* 0x8EC */ be<u8> field_0x57c;
    /* 0x8ED */ be<u8> field_0x57d;
    /* 0x8EE */ be<u8> mbDraw;
    /* 0x8EF */ be<u8> field_0x57f;
    /* 0x8F0 */ be<u32> mLevelSeID;
};
WWHD_OFFSET(daWindTag_c, mCpsS, 0x520);
WWHD_OFFSET(daWindTag_c, mBckAnm, 0x630);
WWHD_OFFSET(daWindTag_c, mType, 0x6EC);
WWHD_OFFSET(daWindTag_c, mEfTevStr, 0x700);
WWHD_OFFSET(daWindTag_c, mLevelSeID, 0x8F0);
WWHD_SIZE(daWindTag_c, 0x8F4);

} // namespace daWindTag

void J3DFrameCtrl_init(void* p, s16 end);
void mDoExt_btkAnm_entryModel(void* a, J3DModel* m, f32 frame);
void cM3dGCps_Set(void* cps, void* src);
void PSVECSubtract(void* a, void* b, void* out);
void dCcD_Cps_Set(void* cps, u32 src);
u32 fopKyM_create(s16 name, u32 prm, cXyz* pos, cXyz* scale, u32 fn);
void* fopKyM_SearchByID(u32 id);
BOOL cLib_chasePos(cXyz* pos, cXyz* target, f32 speed);
f32 cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep);
u32 dKy_get_schbit();
f32 dBgS_ObjGndChk_Wtr_Func(cXyz* pos);
void cM3d_CalcVecAngle(void* v, be<s16>* x, be<s16>* z);
void JPAGetXYZRotateMtx(s16 x, s16 y, s16 z, u32 mtx);
s32 fopAcM_cullingCheck_call(fopAc_ac_c* a);
s16 fpcLf_GetPriority(void* a);
void fopDwTg_ToDrawQ(u32 tag, s16 prio);
void dKy_tevstr_init_l(void* t, s8 roomNo, u8 p);
void color_convert(void* out, void* in, f32 f);
WindTagHD::ColorF* material_uniform(void* p, s32 n);
void matpacket_getTevColor(void* out, u32 packet, s32 idx);

void cCcD_Stts_ct(void* p);
void dCcD_GStts_ct(void* p);
void dCcD_GObjInf_ct(void* p);
void cM3dGCps_ct(void* p);
void J3DAnmTransform_ct(void* p);
void cM3dGSph_ct(void* p);
void cM3dGCps_SetStartEnd(void* p, cXyz* start, cXyz* end, f32 radius);
void cM3dGSph_SetCenterRadius(void* p, cXyz* center, f32 radius);
BOOL cM3d_Cross_CpsSph(void* cps, void* sph, cXyz* cross);
void cM3dGCps_NearPos(void* p, cXyz* eye, cXyz* nearest);
void cM3dGCps_dt(void* p, s32 flags);
void dCcS_SetMass(void* p, void* shape, s32 mass);
void cM3dGTri_ct(void* p, cXyz* a, cXyz* b, cXyz* c);
void J3DAnmTransform_dt(void* p, s32 flags);
void dCcD_GObjInf_dt(void* p, s32 flags);
void dCcD_Stts_dt(void* p, s32 flags);
void fopAc_ac_c_dt(void* p, s32 flags);
