/** JPA1 extra-texture archive. HD: indirect matrix selector is absent. */
#pragma once
#include "wwhd.h"
struct JPAExTexShapeData {
 be<u32> mFlags;
 be<f32> mIndTexMtx[2][3];
 be<s8> mExpScale;
 u8 padding[3];
 be<u8> mIndTextureID,mSubTextureID,mSecondTextureID;
 u8 tailPadding;
};
class JPAExTexShapeArc {
public:
 gptr<void> mVtable;
 gptr<const JPAExTexShapeData> mpData;
 u32 getIndTexMode();
 const be<f32>* getIndTexMtx();
 s32 getExpScale();
 u8 getIndTextureID();
 u8 getSubTextureID();
 u32 isEnableSecondTex();
 u8 getSecondTexIndex();
};
WWHD_SIZE(JPAExTexShapeData,0x24);
WWHD_OFFSET(JPAExTexShapeData,mExpScale,0x1C);
WWHD_OFFSET(JPAExTexShapeData,mSecondTextureID,0x22);
WWHD_SIZE(JPAExTexShapeArc,8);
WWHD_OFFSET(JPAExTexShapeArc,mpData,4);
struct JPAExTexShapeStaticBlock {be<u32> words[4];};
void JPAExTexShapeRegister(void*);
