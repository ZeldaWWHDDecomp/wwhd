#pragma once
#include "d/actor/d_a_bgn.h"
namespace bgn_draw {
// HD: private partial views for the two other bodies and reflection renderer.
// Only fields read by this inlined draw are exposed; the shared actor header stays unchanged.
struct bgn2_class : fopEn_enemy_c {
    u8 mUnused3C8[0x8];
    gptr<mDoExt_McaMorf> mpHeadMorf;
    mDoExt_J3DModelPacketS_l m02B8;
    gptr<mDoExt_McaMorf> mpBodyMorf;
    mDoExt_J3DModelPacketS_l m02D0;
    mDoExt_J3DModelPacketS_l m02E4;
    gptr<J3DModel> mpJyakutenModel[3];
    gptr<mDoExt_brkAnm> mJyakutenCBrkAnm;
    gptr<mDoExt_brkAnm> mJyakutenBBrkAnm;
    u8 mUnused5FC[0x2A54];
    be<s16> m2D60;
    u8 mUnused3052[0x2];
    be<f32> m2D64;
    be<s16> mArrowHitFlashTimer;
    u8 mUnused305A[0x112];
    be<f32> m2E7C;
    u8 mUnused3170[0x8];
    mDoExt_3DlineMat1_bgn mRedRopeMat;
    u8 mUnused3300[0x4];
    mDoExt_J3DModelPacketS_l mHdPacket;
};
WWHD_OFFSET(bgn2_class, mpHeadMorf, 0x3D0);
WWHD_OFFSET(bgn2_class, m02B8, 0x3D4);
WWHD_OFFSET(bgn2_class, mpBodyMorf, 0x484);
WWHD_OFFSET(bgn2_class, m02D0, 0x488);
WWHD_OFFSET(bgn2_class, m02E4, 0x538);
WWHD_OFFSET(bgn2_class, mpJyakutenModel, 0x5E8);
WWHD_OFFSET(bgn2_class, mJyakutenCBrkAnm, 0x5F4);
WWHD_OFFSET(bgn2_class, mJyakutenBBrkAnm, 0x5F8);
WWHD_OFFSET(bgn2_class, m2D60, 0x3050);
WWHD_OFFSET(bgn2_class, m2D64, 0x3054);
WWHD_OFFSET(bgn2_class, mArrowHitFlashTimer, 0x3058);
WWHD_OFFSET(bgn2_class, m2E7C, 0x316C);
WWHD_OFFSET(bgn2_class, mRedRopeMat, 0x3178);
WWHD_OFFSET(bgn2_class, mHdPacket, 0x3304);
WWHD_SIZE(bgn2_class, 0x33B4);
struct part_s3 {
    gptr<J3DModel> mpPart3Model;
    mDoExt_J3DModelPacketS_l m0004;
    dKy_tevstr_l mPart3TevStr;
    be<s16> m00C8;
    u8 mUnused27E[0x2];
    be<f32> m00CC;
    be<s16> mPart3ArrowHitFlashTimer;
    u8 mUnused286[0x2];
    cXyz m00D4;
    u8 mUnused294[0x1744];
};
WWHD_OFFSET(part_s3, mpPart3Model, 0x0);
WWHD_OFFSET(part_s3, m0004, 0x4);
WWHD_OFFSET(part_s3, mPart3TevStr, 0xB4);
WWHD_OFFSET(part_s3, m00C8, 0x27C);
WWHD_OFFSET(part_s3, m00CC, 0x280);
WWHD_OFFSET(part_s3, mPart3ArrowHitFlashTimer, 0x284);
WWHD_OFFSET(part_s3, m00D4, 0x288);
WWHD_SIZE(part_s3, 0x19D8);
struct bgn3_class : fopEn_enemy_c {
    u8 mUnused3C8[0x8];
    gptr<mDoExt_McaMorf> mpMorf;
    mDoExt_J3DModelPacketS_l m002B8;
    gptr<J3DModel> m002CC;
    mDoExt_J3DModelPacketS_l m002D0;
    u8 mUnused538[0x16A4];
    gptr<mDoExt_brkAnm> mJyakutenCBrkAnm;
    gptr<mDoExt_brkAnm> mJyakutenBBrkAnm;
    u8 mUnused1BE4[0x4];
    part_s3 mParts[10];
    u8 mUnused11E58[0x1A];
    be<s16> m0FD96;
    be<f32> m0FD98;
    be<s16> mArrowHitFlashTimer;
    u8 mUnused11E7A[0x2C2];
    be<f32> m10060;
    mDoExt_3DlineMat1_bgn mRedRopeMat;
    u8 mUnused122C8[0x4];
    mDoExt_J3DModelPacketS_l mHdPacket;
};
WWHD_OFFSET(bgn3_class, mpMorf, 0x3D0);
WWHD_OFFSET(bgn3_class, m002B8, 0x3D4);
WWHD_OFFSET(bgn3_class, m002CC, 0x484);
WWHD_OFFSET(bgn3_class, m002D0, 0x488);
WWHD_OFFSET(bgn3_class, mJyakutenCBrkAnm, 0x1BDC);
WWHD_OFFSET(bgn3_class, mJyakutenBBrkAnm, 0x1BE0);
WWHD_OFFSET(bgn3_class, mParts, 0x1BE8);
WWHD_OFFSET(bgn3_class, m0FD96, 0x11E72);
WWHD_OFFSET(bgn3_class, m0FD98, 0x11E74);
WWHD_OFFSET(bgn3_class, mArrowHitFlashTimer, 0x11E78);
WWHD_OFFSET(bgn3_class, m10060, 0x1213C);
WWHD_OFFSET(bgn3_class, mRedRopeMat, 0x12140);
WWHD_OFFSET(bgn3_class, mHdPacket, 0x122CC);
WWHD_SIZE(bgn3_class, 0x1237C);
struct LinkReflection {
    u8 mUnused0[0x318];
    be<f32> mVerticalSpeed;
    u8 mUnused31C[0xB4];
    be<s16> mViewAngle;
    u8 mUnused3D2[0xA];
    be<f32> mCameraDistance;
    u8 mUnused3E0[0x4778];
    be<u32> mPacketCount;
    u8 mUnused4B5C[0x4];
    gptr<gptr<mDoExt_J3DModelPacketS_l>> mpPackets;
    u8 mUnused4B64[0x84];
    mDoExt_J3DModelPacketS_l mNearPacket;
    u8 mUnused4C98[0x630];
    mDoExt_J3DModelPacketS_l mFarPacket;
};
WWHD_OFFSET(LinkReflection, mVerticalSpeed, 0x318);
WWHD_OFFSET(LinkReflection, mViewAngle, 0x3D0);
WWHD_OFFSET(LinkReflection, mCameraDistance, 0x3DC);
WWHD_OFFSET(LinkReflection, mPacketCount, 0x4B58);
WWHD_OFFSET(LinkReflection, mpPackets, 0x4B60);
WWHD_OFFSET(LinkReflection, mNearPacket, 0x4BE8);
WWHD_OFFSET(LinkReflection, mFarPacket, 0x52C8);
WWHD_SIZE(LinkReflection, 0x5378);
struct PlayDraw {
    u8 mUnused0[0x5B34];
    gptr<LinkReflection> mpLink;
    u8 mUnused5B38[0x240];
    be<u32> mpOpaList;
    be<u32> mpXluList;
    u8 mUnused5D80[0x20];
    be<u32> mpOpaListSky;
    u8 mUnused5DA4[0x210];
    u8 mLineSortPackets[1];
};
WWHD_OFFSET(PlayDraw, mpLink, 0x5B34);
WWHD_OFFSET(PlayDraw, mpOpaList, 0x5D78);
WWHD_OFFSET(PlayDraw, mpXluList, 0x5D7C);
WWHD_OFFSET(PlayDraw, mpOpaListSky, 0x5DA0);
WWHD_OFFSET(PlayDraw, mLineSortPackets, 0x5FB4);
WWHD_SIZE(PlayDraw, 0x5FB5);
struct DrawLists {
    be<u32> mpOpa;
    be<u32> mpXlu;
};
WWHD_OFFSET(DrawLists, mpOpa, 0x0);
WWHD_OFFSET(DrawLists, mpXlu, 0x4);
WWHD_SIZE(DrawLists, 0x8);
struct Blur {
    be<u8> mEnabled;
    be<u8> mRate;
};
WWHD_OFFSET(Blur, mEnabled, 0x0);
WWHD_OFFSET(Blur, mRate, 0x1);
WWHD_SIZE(Blur, 0x2);
struct MaterialNode {
    be<u32> mpMaterial;
    u8 mUnused4[0x14];
    be<u32> mpTevBlock;
    u8 mUnused1C[0x84];
    be<u32> mUniformFlags;
};
WWHD_OFFSET(MaterialNode, mpMaterial, 0x0);
WWHD_OFFSET(MaterialNode, mpTevBlock, 0x18);
WWHD_OFFSET(MaterialNode, mUniformFlags, 0xA0);
WWHD_SIZE(MaterialNode, 0xA4);
struct Material {
    u8 mUnused0[0x20];
    be<s32> mPEBlockOffset;
};
WWHD_OFFSET(Material, mPEBlockOffset, 0x20);
WWHD_SIZE(Material, 0x24);
struct TevBlock {
    u8 mUnused0[0x4];
    be<u32> mpVtable;
};
WWHD_OFFSET(TevBlock, mpVtable, 0x4);
WWHD_SIZE(TevBlock, 0x8);
struct Vtable {
    u8 mUnused0[0x14];
    be<u32> mGetMaterialID;
    u8 mUnused18[0xC];
    be<u32> mSetTevColor;
    u8 mUnused28[0xC];
    be<u32> mGetTevColor;
    u8 mUnused38[0x4];
    be<u32> mSetTevKColor;
    u8 mUnused40[0xC];
    be<u32> mGetTevKColor;
};
WWHD_OFFSET(Vtable, mGetMaterialID, 0x14);
WWHD_OFFSET(Vtable, mSetTevColor, 0x24);
WWHD_OFFSET(Vtable, mGetTevColor, 0x34);
WWHD_OFFSET(Vtable, mSetTevKColor, 0x3C);
WWHD_OFFSET(Vtable, mGetTevKColor, 0x4C);
WWHD_SIZE(Vtable, 0x50);
struct Resource {
    u8 mUnused0[0x4];
    be<s32> mNameOffset;
};
WWHD_OFFSET(Resource, mNameOffset, 0x4);
WWHD_SIZE(Resource, 0x8);
struct ModelResource {
    u8 mUnused0[0x14];
    be<u32> mpResource;
};
WWHD_OFFSET(ModelResource, mpResource, 0x14);
WWHD_SIZE(ModelResource, 0x18);
struct Uniform {
    be<f32> r;
    be<f32> g;
    be<f32> b;
    be<f32> a;
};
WWHD_OFFSET(Uniform, r, 0x0);
WWHD_OFFSET(Uniform, g, 0x4);
WWHD_OFFSET(Uniform, b, 0x8);
WWHD_OFFSET(Uniform, a, 0xC);
WWHD_SIZE(Uniform, 0x10);
template <class T> inline T *view(u32 address) { return gabi::at<T>(address); }
} // namespace bgn_draw
extern void bgnDraw_onBlure();
extern void bgnDraw_scaleColor(u32 dst, u32 src, f32 intensity);
extern u32 bgnDraw_uniformSlot(u32 flags, s32 index);
extern void bgnDraw_updateLine(u32 material, s32 count, u32 color, u32 tev);
extern void bgnDraw_setLineMaterial(u32 packet, u32 material);
extern void bgnDraw_entryDL(u32 morf);
extern void bgnDraw_setBlendType(u32 blend, s32 value);
extern void bgnDraw_setColorSrc(u32 blend, s32 value);
extern void bgnDraw_setAlphaSrc(u32 blend, s32 value);
extern void bgnDraw_setColorDst(u32 blend, s32 value);
extern void bgnDraw_setAlphaDst(u32 blend, s32 value);
extern void bgnDraw_setDepthEnable(u32 mode, s32 value);
extern void bgnDraw_setDepthFunction(u32 mode, s32 value);
extern void bgnDraw_setDepthUpdate(u32 mode, s32 value);
namespace bgn_draw {
struct StringVtable {
    u8 mUnused[0x14];
    be<u32> mAssureTermination;
};
WWHD_SIZE(StringVtable, 0x18);
} // namespace bgn_draw
// HD: callback identities remain guest addresses; native host function pointers do not have
// the GHS layout.
enum BgnDrawCallbacks : u32 {
    BGN_DRAW_ten_a_d_sub = 0x0207B040,
    BGN_DRAW_ki_a_d_sub = 0x0207B0D8,
    BGN_DRAW_ks_a_d_sub = 0x0207B13C,
    BGN_DRAW_himo3_a_d_sub = 0x0207B1C8,
    BGN_DRAW_bgn2_s_sub = 0x0207B22C,
    BGN_DRAW_bgn3_s_sub = 0x0207B27C,
};
