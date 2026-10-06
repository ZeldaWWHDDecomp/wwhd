/* cXyz / csXyz: guest layouts (unchanged from GameCube) and host value types. */
#pragma once
#include "wwhd.h"

/* host-side value (temporaries); single-precision arithmetic as on the console */
struct Vec3f {
    f32 x, y, z;
};

struct cXyz {
    be<f32> x, y, z;
    Vec3f get() const { return {x, y, z}; }
    cXyz& operator=(const Vec3f& v) { x = v.x; y = v.y; z = v.z; return *this; }
    void set(f32 ax, f32 ay, f32 az) { x = ax; y = ay; z = az; }
    /* struct assignment: GHS copies the words with integer loads/stores (bit-exact, a float
     * load/store pair would quiet a signalling NaN) */
    void copy(const cXyz& o) {
        for (u32 i = 0; i < 12; i += 4) gabi::store<u32>(gabi::ea(this) + i, gabi::load<u32>(gabi::ea(&o) + i));
    }
};
WWHD_SIZE(cXyz, 0xC);

struct csXyz {
    be<s16> x, y, z;
};
WWHD_SIZE(csXyz, 0x6);
