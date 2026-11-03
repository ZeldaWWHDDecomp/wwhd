#pragma once
#include "JSystem/J2DGraph/J2DGrafContext.h"
#include "JSystem/JSupport/JSUList.h"
#include "JSystem/JSupport/JSUInputStream.h"
struct J2DPaneVtable {
    u8 prefix[0xC];be<u32> destroy;u8 pad10[0x2C];be<u32> calcMtx;
    u8 pad40[0x14];be<u32> drawSelf;u8 pad58[4];be<u32> search;u8 pad60[4];be<u32> makeMatrix;
};
struct J2DPaneTree { JSUPtrList children; JSUPtrLink link; };
struct J2DPane {
    struct J2DScrnBlockHeader { be<u32> magic,size; };
    be<u32> mMagic,mTag;
    J2DBox mBounds,mGlobalBounds,mDrawBounds;
    Mtx34 mMtx,mDrawMtx;
    J2DPoint mBasePosition;
    be<f32> mRotation;
    be<u8> mRotationAxis,m2DBasePosition,mVisible,mCullMode,mAlpha,mDrawAlpha,mInfluencedAlpha,mIsConnectParent;
    J2DPaneTree mPaneTree;
    gptr<J2DPaneVtable> __vtbl;
    void draw(f32,f32,const J2DGrafContext*,u32);
    void makePaneStream(J2DPane*,JSURandomInputStream*);
    void initiate();void setBasePosition(s32);void calcMtx();void drawSelf(f32,f32);
    J2DPane* search(u32);
    void makeMatrix(f32,f32);
};
WWHD_SIZE(J2DPaneTree,0x1C);WWHD_SIZE(J2DPaneVtable,0x68);
WWHD_OFFSET(J2DPane,mBounds,8);WWHD_OFFSET(J2DPane,mMtx,0x38);
WWHD_OFFSET(J2DPane,mBasePosition,0x98);WWHD_OFFSET(J2DPane,mRotation,0xA0);
WWHD_OFFSET(J2DPane,mAlpha,0xA8);WWHD_OFFSET(J2DPane,mPaneTree,0xAC);
WWHD_OFFSET(J2DPane,__vtbl,0xC8);WWHD_SIZE(J2DPane,0xCC);
void* paneAllocate(u32);
void paneDelete(void*);
void paneListInit(JSUPtrList*);
void paneLinkConstruct(JSUPtrLink*,void*);
void paneLinkDestroy(JSUPtrLink*,u32);
void paneListDestroy(JSUPtrList*,u32);
u32 paneListAppend(JSUPtrList*,JSUPtrLink*);
void paneTranslate(Mtx34*,f32,f32,f32);
void paneRotate(Mtx34*,u32,f32);
void paneConcat(const Mtx34*,const Mtx34*,Mtx34*);
void paneRegisterStatic(void*);

s32 paneRead(JSUInputStream*,void*,s32);
s32 paneAlign(JSURandomInputStream*,s32);

void* paneCopyBytes(void*,const void*,u32);
void paneCopyMatrix(const Mtx34*,Mtx34*);
void paneScissorBounds(const J2DGrafContext*,J2DBox*,const J2DBox*);
void panePlaceContext(J2DGrafContext*,const J2DBox*);
void paneScissorContext(J2DGrafContext*,const J2DBox*);
void paneSetScissor(J2DGrafContext*);
