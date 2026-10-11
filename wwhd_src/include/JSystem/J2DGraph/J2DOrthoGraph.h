/* Orthographic 2D context.
 * HD: bounds/matrices precede a tail vtable; ortho/near/far retain GC offsets. */
#pragma once
#include "wwhd.h"
namespace Ortho {
struct Matrix44 {
    be<f32> m[4][4];
};
WWHD_SIZE(Matrix44, 0x40);
struct Point {
    be<f32> x, y;
};
struct Box {
    Point i, f;
};
WWHD_SIZE(Box, 0x10);
} // namespace Ortho
struct J2DOrthoGraph {
    /* 0x00 */ Ortho::Box mBounds;
    u8 _10[0x2C];
    /* 0x3C */ Ortho::Matrix44 mMtx44;
    /* 0x7C */ Mtx34 mPosMtx;
    u8 _AC[0xC];
    /* 0xB8 */ be<u32> __vtbl;
    /* 0xBC */ Ortho::Box mOrtho;
    /* 0xCC */ be<f32> mNear;
    /* 0xD0 */ be<f32> mFar;
    void setLookat();
    void setPort();
    void scissorBounds(Ortho::Box*, const Ortho::Box*);
    u32 getGrafType();
};
WWHD_OFFSET(J2DOrthoGraph, mMtx44, 0x3C);
WWHD_OFFSET(J2DOrthoGraph, mPosMtx, 0x7C);
WWHD_OFFSET(J2DOrthoGraph, __vtbl, 0xB8);
WWHD_OFFSET(J2DOrthoGraph, mOrtho, 0xBC);
WWHD_OFFSET(J2DOrthoGraph, mNear, 0xCC);
WWHD_OFFSET(J2DOrthoGraph, mFar, 0xD0);
WWHD_SIZE(J2DOrthoGraph, 0xD4);
void* orthoAllocate(u32);
void orthoBaseConstruct(J2DOrthoGraph*, f32, f32, f32, f32);
void orthoBaseSetPort(J2DOrthoGraph*);
void orthoMatrixIdentity(Mtx34*);
void orthoMatrixProjection(Ortho::Matrix44*, f32, f32, f32, f32, f32, f32);
void orthoRegisterStatic(void*);
