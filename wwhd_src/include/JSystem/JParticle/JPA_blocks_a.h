/** JPA1 archive key and dynamics blocks. GameCube offsets are unchanged in HD. */
#pragma once
#include "wwhd.h"

// HD: this split unit owns a private view; other archive units declare additional getters.
namespace jpa_blocks_a {
struct JPAKeyBlockData {
    /* 0x00 */ be<u8> mID;
    u8 _opaque01[3];
    /* 0x04 */ be<u8> mNumber;
    u8 _opaque05;
    /* 0x06 */ be<u8> mbLoopEnable;
};
union JPABlockCoordinate { be<f32> mValue; be<u32> mBits; };
struct JPABlockVector { JPABlockCoordinate x, y, z; };
struct JPABlockRotation { be<s16> x, y, z; };
struct JPADynamicsBlockData {
    /* 0x00 */ be<u32> mFlags;
    /* 0x04 */ be<f32> mVolumeSweep;
    /* 0x08 */ be<f32> mVolumeMinRad;
    /* 0x0C */ be<u16> mVolumeSize;
    /* 0x0E */ be<u16> mDivNumber;
    /* 0x10 */ be<f32> mRate;
    /* 0x14 */ be<f32> mRateRndm;
    /* 0x18 */ be<u8> mRateStep;
    u8 _opaque19;
    /* 0x1A */ be<s16> mMaxFrame;
    /* 0x1C */ be<s16> mStartFrame;
    /* 0x1E */ be<s16> mLifeTime;
    /* 0x20 */ be<f32> mLifeTimeRndm;
    /* 0x24 */ be<f32> mInitVelOmni;
    /* 0x28 */ be<f32> mInitVelAxis;
    /* 0x2C */ be<f32> mInitVelRndm;
    /* 0x30 */ be<f32> mInitVelDir;
    /* 0x34 */ be<f32> mInitVelRatio;
    /* 0x38 */ be<f32> mSpread;
    /* 0x3C */ be<f32> mAirResist;
    /* 0x40 */ be<f32> mAirResistRndm;
    /* 0x44 */ be<f32> mMoment;
    /* 0x48 */ be<f32> mMomentRndm;
    /* 0x4C */ be<f32> mAccel;
    /* 0x50 */ be<f32> mAccelRndm;
    /* 0x54 */ JPABlockVector mEmitterScl;
    /* 0x60 */ JPABlockVector mEmitterTrs;
    /* 0x6C */ JPABlockVector mEmitterDir;
    /* 0x78 */ JPABlockRotation mEmitterRot;
    u8 _pad7E[2];
};
class JPAKeyBlockArc {
public:
    u8 getID();
    bool isLoopEnable();
    u8 getNumber();
    const be<f32>* getKeyDataPtr();
    /* 0x00 */ gptr<void> mVtable;
    /* 0x04 */ gptr<const JPAKeyBlockData> mpData;
    /* 0x08 */ gptr<const be<f32>> mpKeyData;
};
class JPADynamicsBlockArc {
public:
    u32 getDataFlag();
    u32 getUseKeyFlag();
    void getEmitterScl(JPABlockVector* out);
    void getEmitterRot(JPABlockRotation* out);
    void getEmitterTrs(JPABlockVector* out);
    void getEmitterDir(JPABlockVector* out);
    f32 getInitVelOmni();
    f32 getInitVelAxis();
    f32 getInitVelRndm();
    f32 getInitVelDir();
    f32 getSpread();
    f32 getInitVelRatio();
    u32 getVolumeType();
    u32 getVolumeSize();
    /* 0x00 */ gptr<void> mVtable;
    /* 0x04 */ gptr<const JPADynamicsBlockData> mpData;
};
enum { JPA_VOLUME_TYPE_SHIFT = 8, JPA_VOLUME_TYPE_MASK = 7 };
WWHD_SIZE(JPAKeyBlockData,0x7);
WWHD_OFFSET(JPAKeyBlockData,mNumber,0x4);
WWHD_OFFSET(JPAKeyBlockData,mbLoopEnable,0x6);
WWHD_SIZE(JPABlockCoordinate,0x4);
WWHD_SIZE(JPABlockVector,0xC);
WWHD_SIZE(JPABlockRotation,0x6);
WWHD_SIZE(JPADynamicsBlockData,0x80);
WWHD_OFFSET(JPADynamicsBlockData,mVolumeSize,0xC);
WWHD_OFFSET(JPADynamicsBlockData,mInitVelOmni,0x24);
WWHD_OFFSET(JPADynamicsBlockData,mInitVelAxis,0x28);
WWHD_OFFSET(JPADynamicsBlockData,mInitVelRndm,0x2C);
WWHD_OFFSET(JPADynamicsBlockData,mInitVelDir,0x30);
WWHD_OFFSET(JPADynamicsBlockData,mInitVelRatio,0x34);
WWHD_OFFSET(JPADynamicsBlockData,mSpread,0x38);
WWHD_OFFSET(JPADynamicsBlockData,mEmitterScl,0x54);
WWHD_OFFSET(JPADynamicsBlockData,mEmitterTrs,0x60);
WWHD_OFFSET(JPADynamicsBlockData,mEmitterDir,0x6C);
WWHD_OFFSET(JPADynamicsBlockData,mEmitterRot,0x78);
WWHD_SIZE(JPAKeyBlockArc,0xC);
WWHD_OFFSET(JPAKeyBlockArc,mpData,0x4);
WWHD_OFFSET(JPAKeyBlockArc,mpKeyData,0x8);
WWHD_SIZE(JPADynamicsBlockArc,0x8);
WWHD_OFFSET(JPADynamicsBlockArc,mpData,0x4);

} // namespace jpa_blocks_a
