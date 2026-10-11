/** Joint hit geometry. HD keeps the GameCube 0x20-byte JntHit layout, but its
 * HIO vtable moves to the tail and vector-by-value arguments use pointers. */
#pragma once
#include "bindings.h"
enum {
    JntHitType_CYL_e, JntHitType_SPH_e, JntHitType_CYL2_e,
    JntHitType_CYL_THROW_e, JntHitType_SPH_THROW_e,
    JntHitType_CYL_UNK_e, JntHitType_SPH_UNK_e,
    JntHitType_CYL_DELETE_e, JntHitType_SPH_DELETE_e,
    JntHitIdx_DELETE_e = -3, JntHitIdx_NONE_e = -1,
};
struct __jnt_hit_data_c {
    be<s16> mShapeType, mJointIndex;
    be<f32> mRadius;
    gptr<cXyz> mpOffsets;
};
class JntHit_c {
public:
    u32 CreateInit();
    u32 SphHitPosAngleOffset(cXyz*, csXyz*, cXyz*, csXyz*, cXyz*, f32);
    u32 CylHitPosAngleOffset(cXyz*, csXyz*, cXyz*, csXyz*, cXyz*, cXyz*, f32);
    u32 Cyl2HitPosAngleOffset(cXyz*, csXyz*, cXyz*, csXyz*, cXyz*, cXyz*, f32);
    u32 HitBufferUpdate(be<s32>*, cXyz*, u32, csXyz*, cXyz*);
    s32 searchJntHitPosAngleOffset(cXyz*, csXyz*, cXyz*, csXyz*);
    /* 0x00 */ gptr<__jnt_hit_data_c> mpSearchData;
    /* 0x04 */ gptr<J3DModel> mpModel;
    /* 0x08 */ gptr<cXyz> mpOffsets;
    /* 0x0C */ gptr<be<f32>> mpRadiuses;
    /* 0x10 */ gptr<be<s16>> mpShapeTypes;
    /* 0x14 */ gptr<be<s16>> mpJointIndexes;
    /* 0x18 */ be<s16> mMaxNum;
    u8 padding[2];
    /* 0x1C */ gptr<void> field_0x1C;
};
class JntHit_HIO_c {
public:
    /* 0x00 */ be<u8> mNo;
    u8 padding_1;
    /* 0x02 */ be<s16> m06;
    /* 0x04 */ be<s16> m08;
    u8 padding_6[2];
    /* 0x08 */ be<f32> m0C;
    /* 0x0C */ cXyz m10;
    /* 0x18 */ be<f32> m1C, m20, m24;
    /* 0x24 */ be<u8> field_0x24;
    u8 padding_25[3];
    /* 0x28 */ gptr<void> __vtbl;
};
namespace JntHitHD {
struct PiPair { be<f32> negative, positive; };
struct HeaderStatics {
    be<f32> mNegativePi, mPositivePi;
    be<u8> mFirstObject, mSecondObject;
    u8 padding[2];
    be<u32> mZero[4];
};
}
void* JntHit_newArray(u32);
cXyz* JntHit_constructVectorArray(u32, u32, u32, u32);
void JntHitFirstHeaderObject_ct(be<u8>*);
void JntHitSecondHeaderObject_ct(be<u8>*);
WWHD_SIZE(__jnt_hit_data_c, 0xC);
WWHD_SIZE(JntHit_c, 0x20);
WWHD_OFFSET(JntHit_c, mMaxNum, 0x18);
WWHD_OFFSET(JntHit_c, field_0x1C, 0x1C);
WWHD_SIZE(JntHit_HIO_c, 0x2C);
WWHD_OFFSET(JntHit_HIO_c, m10, 0xC);
WWHD_OFFSET(JntHit_HIO_c, __vtbl, 0x28);
WWHD_SIZE(JntHitHD::HeaderStatics, 0x1C);

f32 PSVECDotProduct(const cXyz*, const cXyz*);
f32 PSVECSquareMag(const cXyz*);
void PSVECAdd(const cXyz*, const cXyz*, cXyz*);
void PSVECSubtract(const cXyz*, const cXyz*, cXyz*);
void PSVECScale(const cXyz*, cXyz*, f32);
void JntHit_normZP(const cXyz*, cXyz*);
void JntHit_normalizeZP(cXyz*, cXyz*);
u32 JntHit_normalizeRS(cXyz*);
void JntHit_cross(const cXyz*, cXyz*, const cXyz*);
void JntHit_inverse(Mtx34*, Mtx34*);
void JntHit_MtxToRot(Mtx34*, csXyz*);

namespace JntHitHD {
struct SinCos { be<f32> sin, cos; };
struct SinCosTable { SinCos m[8192]; };
}
WWHD_SIZE(JntHitHD::SinCos, 8);
