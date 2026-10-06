/* d_a_sea: the sea surface (WWHD layout). 
 *
 * GameCube -> WWHD: daSea_packet_c grew from 0x148 to 0x3A8. Its J3DPacket base is 0x98 bytes
 * (GameCube 0x10), so the CPU fields move by +0x88; the three GXTexObj became 0x40 bytes each
 * (GameCube 0x20), so everything after them moves by +0xE8. HD-only GPU objects (vertex buffers,
 * shader, material) follow at 0x22E..0x3A8. l_cloth is a function-local static (object
 * 0x1046D8D0, guard 0x1046DC78) reached through the pointer at 0x1046D8B0. */
#pragma once
#include "bindings.h"

struct daSea_WaterHeightInfo_Mng {
    /* 0x00 */ be<u8> mHeight[9][9];
    /* 0x51 */ u8 m51[3];
    /* 0x54 */ be<u32> __vtbl;

    int Pos2Index(f32, be<f32>*);
    int GetHeight(f32, f32);
    int GetHeight(int, int);
    void GetArea(int, int, be<f32>*, be<f32>*, be<f32>*, be<f32>*);
    void SetInf();
};
WWHD_SIZE(daSea_WaterHeightInfo_Mng, 0x58);

struct daSea_WaveInfoDat {
    /* 0x00 */ be<f32> mHeight;
    /* 0x04 */ be<f32> mKm;
    /* 0x08 */ be<s16> mPhase;
    /* 0x0A */ u8 _0A[2];
    /* 0x0C */ be<f32> mScaleX;
    /* 0x10 */ be<f32> mScaleZ;
    /* 0x14 */ be<s32> mCounterMax;
};
WWHD_SIZE(daSea_WaveInfoDat, 0x18);

struct daSea_WaveInfo {
    /* 0x00 */ gptr<daSea_WaveInfoDat> mWaveInfoTable;
    /* 0x04 */ be<f32> m04[4];
    /* 0x14 */ be<s32> mCounters[4];
    /* 0x24 */ be<f32> mCurScale;
    /* 0x28 */ be<u32> __vtbl;

    void AddCounter();
    f32 GetRatio(int);
    f32 GetKm(int);
    f32 GetScale(f32);
};
WWHD_SIZE(daSea_WaveInfo, 0x2C);

struct daSea_packet_c {
    /* 0x000 */ u8 _000[0xC];          /* J3DPacket (HD 0x98) */
    /* 0x00C */ be<u32> __vtbl;
    /* 0x010 */ u8 _010[0x98 - 0x10];
    /* 0x098 */ daSea_WaterHeightInfo_Mng mWaterHeightMgr; /* GameCube 0x010 */
    /* 0x0F0 */ daSea_WaveInfo mWaveInfo;                  /* GameCube 0x068 */
    /* 0x11C */ cXyz mPlayerPos;
    /* 0x128 */ be<s32> mIdxX;
    /* 0x12C */ be<s32> mIdxZ;
    /* 0x130 */ be<f32> mFlatInter;
    /* 0x134 */ be<f32> mFlatTarget;
    /* 0x138 */ be<f32> mFlatInterCounter;
    /* 0x13C */ u8 mTexObj[3][0x40];   /* mTexSea0/mTexSea1/mTexYura (HD GX2 texture objects) */
    /* 0x1FC */ be<f32> mDrawMinX;     /* GameCube 0x114 */
    /* 0x200 */ be<f32> mDrawMinZ;
    /* 0x204 */ be<f32> mDrawMaxX;
    /* 0x208 */ be<f32> mDrawMaxZ;
    /* 0x20C */ be<u32> mpHeightTable; /* f32[65 * 65] */
    /* 0x210 */ cXyz mCurPos;
    /* 0x21C */ be<u32> m_draw_vtx;
    /* 0x220 */ be<u8> mInitFlag;
    /* 0x221 */ be<u8> mCullStopFlag;
    /* 0x222 */ be<u8> m13A;
    /* 0x223 */ be<u8> m13B;
    /* 0x224 */ be<s32> mRoomNo;
    /* 0x228 */ be<s32> mFlags;
    /* 0x22C */ be<s16> mAnimCounter;
    /* 0x22E */ u8 _22E[2];
    /* 0x230 */ be<u32> mpShader;      /* HD: "wave_draw" shader */
    /* 0x234 */ u8 _234[0x24C - 0x234];
    /* 0x24C */ be<u32> m24C;          /* HD: 0x8200-byte buffer */
    /* 0x250 */ be<u32> mpVtxBuf;      /* HD: double-buffered vertex buffer set */
    /* 0x254 */ be<u32> m254;
    /* 0x258 */ u8 _258[0x274 - 0x258];
    /* 0x274 */ be<u32> mpMaterial;    /* HD: material/uniform block */
    /* 0x278 */ u8 _278[0x3A8 - 0x278];

    bool create(cXyz*);    /* GameCube: cXyz& */
    void CleanUp();
    void SetFlat();
    void ClrFlat();
    f32 CalcFlatInterTarget(cXyz*); /* GameCube: cXyz& (harness entry takes pointers) */
    void CalcFlatInter();
    void SetCullStopFlag();
    void CheckRoomChange();
    void execute(cXyz*);    /* GameCube: cXyz& */
};
WWHD_SIZE(daSea_packet_c, 0x3A8);
WWHD_OFFSET(daSea_packet_c, mWaveInfo, 0xF0);
WWHD_OFFSET(daSea_packet_c, mDrawMinX, 0x1FC);
WWHD_OFFSET(daSea_packet_c, mInitFlag, 0x220);
WWHD_OFFSET(daSea_packet_c, mAnimCounter, 0x22C);
WWHD_OFFSET(daSea_packet_c, mpMaterial, 0x274);

struct sea_class : fopAc_ac_c {};
