/* Forbidden Woods ceiling tentacle.
 * HD: the enemy base shifts actor members by +0x11C; the expanded line material shifts
 * collision members by a further +0x14C. Unknown field names follow zeldaret/tww. */
#pragma once
#include "d/d_cc_uty.h"
#include "gc_api/gc_api.h"

// HD: this actor uses the textured line update overload and byte-sized segment widths.
struct ShandLineData {
    gptr<cXyz> mpPos;
    gptr<be<u8>> mpSize;
    u8 _08[0x18 - 8];
};
WWHD_SIZE(ShandLineData, 0x18);
struct ShandLineMat : mDoExt_3DlineMat1_c {
    ShandLineData* getLines() { return gabi::at<ShandLineData>(mpLines); }
    cXyz* getPos(int i) { return getLines()[i].mpPos; }
    be<u8>* getSize(int i) { return getLines()[i].mpSize; }
    void update(u16 count, GXColor* color, dKy_tevstr_c* tev) {
        gabi::call(0x025ED1BC, this, count, color, tev);
    }
};
WWHD_SIZE(ShandLineMat, 0x188);
struct ShandSortPacket {
    u8 _00[0x9C];
    void setMat(ShandLineMat* material) { gabi::call(0x025EDD04, this, material); }
};
WWHD_SIZE(ShandSortPacket, 0x9C);
struct ShandRenderContext {
    u8 _00[0x5FB4];
    ShandSortPacket mSortPackets[1];
};
WWHD_OFFSET(ShandRenderContext, mSortPackets, 0x5FB4);
// HD: the attention update loads all three words before writing either destination.
struct ShandVectorWords {
    be<u32> x, y, z;
};
WWHD_SIZE(ShandVectorWords, 12);
inline void shandSetEyePosition(fopAc_ac_c* actor, const cXyz* position) {
    auto* src = reinterpret_cast<const ShandVectorWords*>(position);
    u32 x = src->x, y = src->y, z = src->z;
    auto* eye = reinterpret_cast<ShandVectorWords*>(&actor->eyePos);
    auto* attention = reinterpret_cast<ShandVectorWords*>(&actor->attention_info.position);
    eye->x = x;
    eye->y = y;
    eye->z = z;
    attention->x = x;
    attention->y = y;
    attention->z = z;
}

// HD: private bindings retain the verified widths and array ABI.
/* 02518DB0 at_power_check(CcAtInfo*) -> attacking actor; 02518CC8 def_se_set(actor, cCcD_Obj*,
 * mtrl) */
static inline fopAc_ac_c* at_power_check(CcAtInfo* info) {
    return gabi::call<fopAc_ac_c*>(0x02518DB0, info);
}
static inline void def_se_set(fopAc_ac_c* a, dCcD_GObjInf* obj, u32 mtrl) {
    gabi::call(0x02518CC8, a, obj, mtrl);
}
static inline void shandConstructArray(void* p, u32 n, u32 size, u32 ctor) {
    gabi::call(0x028EFFD0, p, n, size, ctor);
}
static inline void shandDestroyArray(void* p, u32 n, u32 size, u32 dtor, u32 flags) {
    gabi::call(0x028F0164, p, n, size, dtor, flags);
}
/* 02008DAC cBgS_Chk::~cBgS_Chk (ground checks), 02008B4C cBgS_LinChk::~cBgS_LinChk */
static inline void cBgS_GndChk_dt(void* c, s32 f) { gabi::call(0x02008DAC, c, f); }
static inline void shandLineCheckDestroyBase(void* c, s32 f) { gabi::call(0x02008B4C, c, f); }

struct shand_s {
    /* 0x00 */ cXyz mPos;
    /* 0x0C */ u8 _pad[0x18 - 0xC];
    /* 0x18 */ be<f32> field_18;
};
WWHD_SIZE(shand_s, 0x1C);

// HD: inherits the enemy base; tww names the actor base directly.
struct shand_class : fopEn_enemy_c {
    enum : u32 { __vt = 0x1003A0F4 }; // HD: virtual destructor.
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ be<s16> mExecuteCount;
    /* 0x3D2 */ be<s16> unused_2B6;
    /* 0x3D4 */ be<s16> mState;
    /* 0x3D6 */ be<s16> field_2BA;
    /* 0x3D8 */ be<s16> field_2BC[4];
    /* 0x3E0 */ be<s16> field_2C4;
    /* 0x3E2 */ u8 _3E2[2];
    /* 0x3E4 */ cXyz field_2C8;
    /* 0x3F0 */ cXyz field_2D4;
    /* 0x3FC */ u8 unused_2E0[0x40C - 0x3FC];
    /* 0x40C */ be<f32> field_2F0;
    /* 0x410 */ be<f32> field_2F4;
    /* 0x414 */ be<f32> field_2F8;
    /* 0x418 */ be<f32> field_2FC;
    /* 0x41C */ be<f32> field_300;
    /* 0x420 */ be<f32> field_304;
    /* 0x424 */ be<u32> field_308;
    /* 0x428 */ gptr<fopAc_ac_c> field_30C;
    /* 0x42C */ gptr<cXyz> field_310;
    /* 0x430 */ gptr<be<u8>> field_314;
    /* 0x434 */ be<s16> field_318;
    /* 0x436 */ u8 _436[2];
    /* 0x438 */ shand_s field_31C[20];
    /* 0x668 */ ShandLineMat mLineMat;
    /* 0x7F0 */ dCcD_Stts mStts;
    /* 0x82C */ dCcD_Sph mSph;
    /* 0x958 */ dCcD_Cyl mCylArr[5];
    /* 0xF48 */ be<f32> ground_y;
    /* 0xF4C */ be<u8> mHasHIO;
};
WWHD_OFFSET(shand_class, field_2F0, 0x40C);
WWHD_OFFSET(shand_class, field_31C, 0x438);
WWHD_OFFSET(shand_class, mStts, 0x7F0);
WWHD_OFFSET(shand_class, mCylArr, 0x958);
WWHD_OFFSET(shand_class, mHasHIO, 0xF4C);

/* HD: the vtable pointer follows the members (+8); size 0xC */
struct daShand_HIO_c {
    enum : u32 { __vt = 0x1003A0E4 };
    /* 0x0 */ be<s8> mNo;
    /* 0x1 */ u8 _1;
    /* 0x2 */ be<s16> field_6;
    /* 0x4 */ be<s16> field_8;
    /* 0x6 */ u8 _6[2];
    /* 0x8 */ be<u32> __vtbl;
};
WWHD_SIZE(daShand_HIO_c, 0xC);
#define l_HIO (*gabi::at<daShand_HIO_c>(0x1046DCB8)) // HD: BSS, initialised by the TU constructor.

// HD: out-of-line construction/destruction and external array destructor callback.
inline void shandLineConstruct(ShandLineMat* material) { gabi::call(0x025EB82C, material); }
inline void shandLineDestroy(ShandLineMat* material) { gabi::call(0x025EB8B8, material, 2); }
inline void shandSphereConstruct(dCcD_Sph* sphere) { gabi::call(0x025166F0, sphere); }
inline void shandSphereDestroy(dCcD_Sph* sphere) { gabi::call(0x02515AE8, sphere, 2); }
static constexpr u32 shandCylinderAabVtable = 0x10039FB4;
static constexpr u32 shandCylinderDestructor = 0x02515A70;
WWHD_SIZE(shand_class, 0xF4D);

static constexpr u32 shandGroundGroupVtable = 0x10039FD4;
static constexpr u32 shandLinePolyVtable = 0x10039FC4;
static dCcD_Cyl* dCcD_Cyl_ct_tu(dCcD_Cyl*);

// HD: flat source views spell out tww collision fields for checked static initialisers.
struct ShandSphereSource {
    be<u32> mFlags;
    be<u32> mAtType;
    u8 mAtp;
    u8 mAtPadding0;
    u8 mAtPadding1;
    u8 mAtPadding2;
    be<u32> mAtSPrm;
    be<u32> mTgType;
    be<u32> mTgSPrm;
    be<u32> mCoSPrm;
    u8 mGAtSe;
    u8 mGAtHitMark;
    u8 mGAtSpl;
    u8 mGAtMtrl;
    be<u32> mGAtSPrm;
    u8 mGTgSe;
    u8 mGTgHitMark;
    u8 mGTgSpl;
    u8 mGTgMtrl;
    be<u32> mGTgSPrm;
    be<u32> mGCoSPrm;
    be<f32> mCenterX;
    be<f32> mCenterY;
    be<f32> mCenterZ;
    be<f32> mRadius;
};
WWHD_OFFSET(ShandSphereSource, mCenterX, 0x30);
WWHD_SIZE(ShandSphereSource, 0x40);
struct ShandCylinderSource {
    be<u32> mFlags;
    be<u32> mAtType;
    u8 mAtp;
    u8 mAtPadding0;
    u8 mAtPadding1;
    u8 mAtPadding2;
    be<u32> mAtSPrm;
    be<u32> mTgType;
    be<u32> mTgSPrm;
    be<u32> mCoSPrm;
    u8 mGAtSe;
    u8 mGAtHitMark;
    u8 mGAtSpl;
    u8 mGAtMtrl;
    be<u32> mGAtSPrm;
    u8 mGTgSe;
    u8 mGTgHitMark;
    u8 mGTgSpl;
    u8 mGTgMtrl;
    be<u32> mGTgSPrm;
    be<u32> mGCoSPrm;
    be<f32> mCenterX;
    be<f32> mCenterY;
    be<f32> mCenterZ;
    be<f32> mRadius;
    be<f32> mHeight;
};
WWHD_OFFSET(ShandCylinderSource, mCenterX, 0x30);
WWHD_SIZE(ShandCylinderSource, 0x44);
