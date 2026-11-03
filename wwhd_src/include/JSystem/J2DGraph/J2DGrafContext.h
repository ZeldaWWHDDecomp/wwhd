#pragma once
#include "wwhd.h"
struct J2DPoint { union { be<f32> x; be<u32> xBits; }; union { be<f32> y; be<u32> yBits; }; };
struct J2DBox { J2DPoint i,f; };
struct J2DColor { be<u32> rgba; };
struct J2DBlend { be<u8> mType,mSrcFactor,mDstFactor; };
struct J2DGrafVtable { u8 header[0x2C]; be<u32> setup2D; u8 pad30[4]; be<u32> setScissor; u8 pad38[4]; be<u32> getGrafType; };
struct J2DGrafContext {
    J2DBox mBounds,mScissorBounds;
    J2DColor mColorTL,mColorTR,mColorBR,mColorBL;
    be<u8> mLineWidth; u8 pad31[3];
    J2DPoint mPrevPos;
    u8 mMatrices[0x70];
    J2DBlend mBlend,mLinePart,mBoxPart; u8 padB5[3];
    gptr<J2DGrafVtable> __vtbl;
    void setColor(const J2DColor*,const J2DColor*,const J2DColor*,const J2DColor*);
    void setLineWidth(u8);
    void place(const J2DBox*);
    void scissor(const J2DBox*);
    void setPort();
    void setScissor();
    void setup2D();
    u32 getGrafType();
};
WWHD_SIZE(J2DPoint,8); WWHD_SIZE(J2DBox,0x10); WWHD_SIZE(J2DColor,4);
WWHD_SIZE(J2DBlend,3); WWHD_SIZE(J2DGrafVtable,0x40);
WWHD_OFFSET(J2DGrafContext,mBounds,0); WWHD_OFFSET(J2DGrafContext,mScissorBounds,0x10);
WWHD_OFFSET(J2DGrafContext,mColorTL,0x20); WWHD_OFFSET(J2DGrafContext,mLineWidth,0x30);
WWHD_OFFSET(J2DGrafContext,mPrevPos,0x34); WWHD_OFFSET(J2DGrafContext,mBlend,0xAC);
WWHD_OFFSET(J2DGrafContext,mLinePart,0xAF); WWHD_OFFSET(J2DGrafContext,mBoxPart,0xB2);
WWHD_OFFSET(J2DGrafContext,__vtbl,0xB8); WWHD_SIZE(J2DGrafContext,0xBC);
void* grafAllocate(u32);
void grafRegisterStatic(void*);

J2DGrafVtable* J2DGetBaseVtable();
