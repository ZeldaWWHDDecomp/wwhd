/* cM2dGBox / cM2dGCir, WWHD layouts. */
#pragma once
#include "bindings.h"

struct cXy {
    /* 0x00 */ be<f32> x;
    /* 0x04 */ be<f32> y;
    // HD: integer word copies preserve signalling NaNs and overlapping source objects.
    cXy& operator=(const cXy& other) {
        gabi::store<u32>(gabi::ea(this), gabi::load<u32>(gabi::ea(&other)));
        gabi::store<u32>(gabi::ea(this) + 4, gabi::load<u32>(gabi::ea(&other) + 4));
        return *this;
    }
};
WWHD_SIZE(cXy, 8);

class cM2dGBox {
public:
    /* 0x00 */ cXy mP0;
    /* 0x08 */ cXy mP1;
    /* 0x10 */ be<u32> vtable; // HD: GHS places the virtual table after the coordinates.
    // HD/glue: guest references are represented as typed pointers for autoglue forwarding.
    void Set(cXy* p0, cXy* p1);
    // HD/glue: omit the host const qualifier because VERIFY registers non-const member pointers.
    f32 GetLen(const cXy* p);
};
WWHD_OFFSET(cM2dGBox, mP1, 8);
WWHD_OFFSET(cM2dGBox, vtable, 0x10);
WWHD_SIZE(cM2dGBox, 0x14);

class cM2dGCir {
public:
    /* 0x00 */ be<f32> mPosX;
    /* 0x04 */ be<f32> mPosY;
    /* 0x08 */ be<f32> mRadius;
    /* 0x0C */ be<u32> vtable;
};
WWHD_SIZE(cM2dGCir, 0x10);
