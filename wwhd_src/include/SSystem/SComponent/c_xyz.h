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

    /* ---- GameCube-style API (gc_api; HD out-of-line operators and PSVEC calls) ---- */
    static cXyz& Zero; /* cXyz::Zero (0x101FFBA8) */
    void set(const cXyz& o) { x = o.x; y = o.y; z = o.z; }
    /* operator-/+ return through a hidden result pointer: 0201ADE0 cXyz::operator-(this, result, b),
     * 0201AD78 cXyz::operator+ [v kamome, nz] */
    auto operator-(const cXyz& b) const {
        const cXyz* a = this;
        const cXyz* pb = &b;
        return gabi::into<cXyz>([a, pb](cXyz* r) { gabi::call(0x0201ADE0, a, r, pb); });
    }
    auto operator+(const cXyz& b) const {
        const cXyz* a = this;
        const cXyz* pb = &b;
        return gabi::into<cXyz>([a, pb](cXyz* r) { gabi::call(0x0201AD78, a, r, pb); });
    }
    template <class F> cXyz& operator=(const gabi::Into<cXyz, F>& p) { p.f(this); return *this; }
    /* struct assignment: GHS copies the three words with integer loads/stores (bit-exact; seen in
     * nz TailControl/modeEventEsa); memberwise float copies would quiet a signalling NaN */
    cXyz& operator=(const cXyz& o) { copy(o); return *this; }
    /* HD: PSVECScale(this, this, s) (028E8E64) [v nz] */
    void operator*=(f32 s) { gabi::call(0x028E8E64, this, this, s); }
    /* HD out of line, result through a hidden pointer: 0201B080 cXyz::outprod(this, result, b),
     * 0201B12C cXyz::normZP(this, result) [v lpalm] */
    auto outprod(const cXyz& b) const {
        const cXyz* a = this;
        const cXyz* pb = &b;
        return gabi::into<cXyz>([a, pb](cXyz* r) { gabi::call(0x0201B080, a, r, pb); });
    }
    auto normZP() const {
        const cXyz* a = this;
        return gabi::into<cXyz>([a](cXyz* r) { gabi::call(0x0201B12C, a, r); });
    }
    /* HD inline: sqrt(PSVECSquareMag(this)) (028E8DD0, 028F4384). The result stays in double
     * precision: callers compare it without rounding to f32 [v lpalm] */
    f64 abs() const;
    /* HD inline: sqrtf(PSVECSquareMag({x, 0, z})) (028E8DD0, 028F4384) [v nz] */
    f32 absXZ() const;
};
WWHD_SIZE(cXyz, 0xC);
inline cXyz& cXyz::Zero = *gabi::at<cXyz>(0x101FFBA8);

/* local cXyz construction (GameCube constructors; floats go through FPRs) */
inline void gabi_construct(cXyz& o, f32 x, f32 y, f32 z) { o.x = x; o.y = y; o.z = z; }
inline void gabi_construct(cXyz& o, const cXyz& s) { o.x = s.x; o.y = s.y; o.z = s.z; }
inline f64 cXyz::abs() const {
    f32 sq = gabi::call<f32>(0x028E8DD0, this);
    return gabi::call<f64>(0x028F4384, (f64)sq);
}
inline f32 cXyz::absXZ() const {
    gabi::Stack<cXyz> t(x, 0.0f, z);
    f32 sq = gabi::call<f32>(0x028E8DD0, t.get());
    return gabi::call<f32>(0x028F4384, sq);
}

struct csXyz {
    be<s16> x, y, z;
};
WWHD_SIZE(csXyz, 0x6);
