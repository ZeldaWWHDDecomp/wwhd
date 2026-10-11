/** Matrix transforms and stack helpers, using GameCube layout and names.
 * HD retains s32 angles/u32 addresses and PPC fused float evaluation. Private source. */
#include "m_Do/m_Do_mtx.h"
using gabi::fmadds;
using gabi::fmsubs;
using gabi::fnmsubs;

static f32 l_zero;
static f32 l_one;
static f32 l_vectorEpsilon;
static f32 l_negativePi;
static f32 l_pi;
static mDoMtxHeaderStatics l_headerStatics;
static mDoMtxStaticRecords l_records;
static Mtx34 l_now;
static mDoMtxBuffer l_buffer;
static mDoMtxStackPointers l_stackPointers;
static mDoMtxQuatStack l_quatStack;

/* 025F181C */
void mDoMtx_ZrotS(u32 p, s32 a) {
    auto* mtx = mDoMtxAt(p);
    auto* sinCos = mDoMtxSinCosAt(a);
    f32 sn = sinCos->mSin, cs = sinCos->mCos, zero = l_zero, one = l_one;
    mtx->m[0][0] = cs;
    mtx->m[0][1] = -sn;
    mtx->m[0][2] = zero;
    mtx->m[0][3] = zero;
    mtx->m[1][0] = sn;
    mtx->m[1][1] = cs;
    mtx->m[1][2] = zero;
    mtx->m[1][3] = zero;
    mtx->m[2][0] = zero;
    mtx->m[2][1] = zero;
    mtx->m[2][2] = one;
    mtx->m[2][3] = zero;
}

/* 025F1884 */
void mDoMtx_YrotS(u32 p, s32 a) {
    auto* mtx = mDoMtxAt(p);
    auto* sinCos = mDoMtxSinCosAt(a);
    f32 sn = sinCos->mSin, cs = sinCos->mCos, zero = l_zero, one = l_one;
    mtx->m[0][0] = cs;
    mtx->m[0][1] = zero;
    mtx->m[0][2] = sn;
    mtx->m[0][3] = zero;
    mtx->m[1][0] = zero;
    mtx->m[1][1] = one;
    mtx->m[1][2] = zero;
    mtx->m[1][3] = zero;
    mtx->m[2][0] = -sn;
    mtx->m[2][1] = zero;
    mtx->m[2][2] = cs;
    mtx->m[2][3] = zero;
}

/* 025F18EC */
void mDoMtx_XrotS(u32 p, s32 a) {
    auto* mtx = mDoMtxAt(p);
    auto* sinCos = mDoMtxSinCosAt(a);
    f32 sn = sinCos->mSin, cs = sinCos->mCos, zero = l_zero, one = l_one;
    mtx->m[0][0] = one;
    mtx->m[0][1] = zero;
    mtx->m[0][2] = zero;
    mtx->m[0][3] = zero;
    mtx->m[1][0] = zero;
    mtx->m[1][1] = cs;
    mtx->m[1][2] = -sn;
    mtx->m[1][3] = zero;
    mtx->m[2][0] = zero;
    mtx->m[2][1] = sn;
    mtx->m[2][2] = cs;
    mtx->m[2][3] = zero;
}

/* 025F1954 */
void mDoMtx_XYZrotS(u32 p, s32 x, s32 y, s32 z) {
    gabi::Local<Mtx34> m;
    if (z) mDoMtx_ZrotS(p, z);
    else mDoMtx_identity(p);
    if (y) {
        mDoMtx_YrotS(gabi::ea(m.get()), y);
        mDoMtx_concat(p, gabi::ea(m.get()), p);
    }
    if (x) {
        mDoMtx_XrotS(gabi::ea(m.get()), x);
        mDoMtx_concat(p, gabi::ea(m.get()), p);
    }
}

/* 025F19F8 */
void mDoMtx_XYZrotM(u32 p, s32 x, s32 y, s32 z) {
    gabi::Local<Mtx34> m;
    if (z) {
        mDoMtx_ZrotS(gabi::ea(m.get()), z);
        mDoMtx_concat(p, gabi::ea(m.get()), p);
    }
    if (y) {
        mDoMtx_YrotS(gabi::ea(m.get()), y);
        mDoMtx_concat(p, gabi::ea(m.get()), p);
    }
    if (x) {
        mDoMtx_XrotS(gabi::ea(m.get()), x);
        mDoMtx_concat(p, gabi::ea(m.get()), p);
    }
}

/* 025F1AA4 */
void mDoMtx_ZXYrotS(u32 p, s32 x, s32 y, s32 z) {
    gabi::Local<Mtx34> m;
    if (y) mDoMtx_YrotS(p, y);
    else mDoMtx_identity(p);
    if (x) {
        mDoMtx_XrotS(gabi::ea(m.get()), x);
        mDoMtx_concat(p, gabi::ea(m.get()), p);
    }
    if (z) {
        mDoMtx_ZrotS(gabi::ea(m.get()), z);
        mDoMtx_concat(p, gabi::ea(m.get()), p);
    }
}

/* 025F1B48 */
void mDoMtx_ZXYrotM(u32 p, s32 x, s32 y, s32 z) {
    gabi::Local<Mtx34> m;
    if (y) {
        mDoMtx_YrotS(gabi::ea(m.get()), y);
        mDoMtx_concat(p, gabi::ea(m.get()), p);
    }
    if (x) {
        mDoMtx_XrotS(gabi::ea(m.get()), x);
        mDoMtx_concat(p, gabi::ea(m.get()), p);
    }
    if (z) {
        mDoMtx_ZrotS(gabi::ea(m.get()), z);
        mDoMtx_concat(p, gabi::ea(m.get()), p);
    }
}

/* 025F1BF4 */
void mDoMtx_XrotM(u32 p, s32 a) {
    gabi::Local<Mtx34> m;
    mDoMtx_XrotS(gabi::ea(m.get()), a);
    mDoMtx_concat(p, gabi::ea(m.get()), p);
}

/* 025F1C28 */
void mDoMtx_YrotM(u32 p, s32 a) {
    gabi::Local<Mtx34> m;
    mDoMtx_YrotS(gabi::ea(m.get()), a);
    mDoMtx_concat(p, gabi::ea(m.get()), p);
}

/* 025F1C5C */
void mDoMtx_ZrotM(u32 p, s32 a) {
    gabi::Local<Mtx34> m;
    mDoMtx_ZrotS(gabi::ea(m.get()), a);
    mDoMtx_concat(p, gabi::ea(m.get()), p);
}

/* 025F1C90 */
void mDoMtx_lookAt(u32 p, u32 eye, u32 target, s32 angle) {
    auto* mtx = mDoMtxAt(p);
    gabi::Local<cXyz> e, t, n, u, b, tmp, alt;
    gabi::Local<Mtx34> rot;
    mDoMtx_copyVectorFloats(gabi::ea(e.get()), eye);
    mDoMtx_copyVectorFloats(gabi::ea(t.get()), target);
    mDoMtx_vectorSubtract(gabi::ea(e.get()), gabi::ea(tmp.get()), gabi::ea(t.get()));
    mDoMtx_copyVectorWords(gabi::ea(n.get()), gabi::ea(tmp.get()));
    mDoMtx_vectorNormalize(gabi::ea(n.get()), gabi::ea(t.get()));
    f32 zero = l_zero, one = l_one;
    u->x = zero;
    u->y = one;
    u->z = zero;
    mDoMtx_vectorCross(gabi::ea(u.get()), gabi::ea(t.get()), gabi::ea(n.get()));
    mDoMtx_copyVectorWords(gabi::ea(u.get()), gabi::ea(t.get()));
    if (mDoMtx_vectorNormalizeRS(gabi::ea(u.get()))==0) {
        u->y = zero;
        u->z = zero;
        u->x = -n->y;
        mDoMtx_vectorCross(gabi::ea(u.get()), gabi::ea(alt.get()), gabi::ea(n.get()));
        mDoMtx_copyVectorWords(gabi::ea(u.get()), gabi::ea(alt.get()));
        mDoMtx_vectorNormalize(gabi::ea(u.get()), gabi::ea(alt.get()));
    }
    mDoMtx_vectorCross(gabi::ea(n.get()), gabi::ea(t.get()), gabi::ea(u.get()));
    mDoMtx_copyVectorWords(gabi::ea(b.get()), gabi::ea(t.get()));
    mDoMtx_vectorNormalize(gabi::ea(b.get()), gabi::ea(t.get()));
    f32 ux = u->x, uy = u->y, uz = u->z;
    mtx->m[0][0] = ux;
    mtx->m[0][2] = uz;
    mtx->m[0][1] = uy;
    f32 dot = mDoMtx_vectorDot(gabi::ea(u.get()), gabi::ea(e.get()));
    f32 bx = b->x, by = b->y, bz = b->z;
    mtx->m[1][0] = bx;
    mtx->m[1][2] = bz;
    mtx->m[1][1] = by;
    mtx->m[0][3] = -dot;
    dot = mDoMtx_vectorDot(gabi::ea(b.get()), gabi::ea(e.get()));
    f32 nx = n->x, ny = n->y, nz = n->z;
    mtx->m[2][2] = nz;
    mtx->m[2][0] = nx;
    mtx->m[2][1] = ny;
    mtx->m[1][3] = -dot;
    dot = mDoMtx_vectorDot(gabi::ea(n.get()), gabi::ea(e.get()));
    mtx->m[2][3] = -dot;
    mDoMtx_ZrotS(gabi::ea(rot.get()), angle);
    mDoMtx_concat(gabi::ea(rot.get()), p, p);
}

/* 025F1EAC */
void mDoMtx_lookAtUp(u32 p, u32 eye, u32 target, u32 up, s32 angle) {
    gabi::Local<cXyz> e, t, u, n;
    gabi::Local<Mtx34> rot;
    mDoMtx_copyVectorFloats(gabi::ea(e.get()), eye);
    mDoMtx_copyVectorFloats(gabi::ea(t.get()), target);
    mDoMtx_copyVectorFloats(gabi::ea(u.get()), up);
    mDoMtx_vectorSubtract(gabi::ea(e.get()), gabi::ea(n.get()), gabi::ea(t.get()));
    s32 ok = mDoMtx_vectorNormalizeRS(gabi::ea(n.get()));
    f32 one = l_one;
    if (!ok) t->z = t->z+one;
    // Ordered comparisons preserve the reference's unordered-NaN fallback.
    f32 eps = l_vectorEpsilon;
    if (std::fabs(u->x)<eps && std::fabs(u->y)<eps && std::fabs(u->z)<eps) u->y = one;
    mDoMtx_sdkLookAt(p, gabi::ea(e.get()), gabi::ea(u.get()), gabi::ea(t.get()));
    mDoMtx_ZrotS(gabi::ea(rot.get()), angle);
    mDoMtx_concat(gabi::ea(rot.get()), p, p);
}

/* 025F1FC4 */
void mDoMtx_concatProjView(u32 p, u32 v, u32 d) {
    auto* projection = mDoMtx44At(p);
    auto* view = mDoMtxAt(v);
    auto* dest = mDoMtx44At(d);
    mDoMtx_concat(p, v, d);
    for (u32 i = 0;i<4;i++) {
        f32 b = projection->m[3][1], c = view->m[1][i], a = projection->m[3][0], x = view->m[0][i], z = projection->m[3][2], y = view->m[2][i];
        f32 r = fmadds(z, y, fmadds(a, x, b*c));
        if (i==3) r = r+projection->m[3][3];
        dest->m[3][i] = r;
    }
}

/* 025F20B0 */
s32 mDoMtx_inverseTranspose(u32 p, u32 d) {
    // HD: Preserve fused evaluation and source reloads between stores when matrices alias.
    auto* mtx = mDoMtxAt(p);
    auto* dest = mDoMtxAt(d);
    f32 h = mtx->m[2][1], a = mtx->m[0][0], g = mtx->m[2][0], e = mtx->m[1][1], b = mtx->m[0][1], f = mtx->m[1][2], c = mtx->m[0][2], i = mtx->m[2][2], dd = mtx->m[1][0];
    f32 ah = a*h, ae = a*e, bf = b*f;
    f32 determinant = fmadds(ae, i, bf*g);
    determinant = fmadds(c*dd, h, determinant);
    f32 ge = g*e;
    determinant = fnmsubs(ge, c, determinant);
    determinant = fnmsubs(dd*b, i, determinant);
    determinant = fnmsubs(ah, f, determinant);
    f32 zero = l_zero;
    if (determinant==zero) return 0;
    f32 inv = l_one/determinant;
    f32 r02 = fmsubs(dd, h, ge)*inv;
    f32 r01 = -fmsubs(dd, i, g*f)*inv;
    f32 r00 = fmsubs(e, i, h*f)*inv;
    f32 r10 = -fmsubs(b, i, h*c)*inv;
    f32 r11 = fmsubs(a, i, g*c)*inv;
    f32 r20 = fnmsubs(e, c, bf)*inv;
    dest->m[2][0] = r20;
    f32 d2 = mtx->m[1][0], c2 = mtx->m[0][2], a2 = mtx->m[0][0], f2 = mtx->m[1][2];
    f32 r21 = -fmsubs(a2, f2, d2*c2)*inv;
    f32 r12 = -fnmsubs(g, b, ah)*inv;
    dest->m[2][1] = r21;
    f32 b2 = mtx->m[0][1], d3 = mtx->m[1][0], a3 = mtx->m[0][0], e2 = mtx->m[1][1];
    dest->m[0][2] = r02;
    dest->m[0][3] = zero;
    dest->m[1][1] = r11;
    dest->m[1][2] = r12;
    dest->m[0][1] = r01;
    f32 r22 = fmsubs(a3, e2, d3*b2)*inv;
    dest->m[0][0] = r00;
    dest->m[1][0] = r10;
    dest->m[2][2] = r22;
    dest->m[2][3] = zero;
    dest->m[1][3] = zero;
    return 1;
}

/* 025F2258 */
void mDoMtx_QuatConcat(u32 a, u32 b, u32 d) {
    // HD: Reload each component after the preceding store; result may alias either input.
    auto* left = mDoMtxQuatAt(a);
    auto* right = mDoMtxQuatAt(b);
    auto* result = mDoMtxQuatAt(d);
    f32 w = fmsubs(left->w, right->w, left->x*right->x);
    w = fnmsubs(left->y, right->y, w);
    w = fnmsubs(left->z, right->z, w);
    result->w = w;
    f32 x = left->x*right->w;
    x = fmadds(left->w, right->x, x);
    x = fmadds(left->y, right->z, x);
    x = fnmsubs(left->z, right->y, x);
    result->x = x;
    f32 y = left->y*right->w;
    y = fmadds(left->w, right->y, y);
    y = fmadds(left->z, right->x, y);
    y = fnmsubs(left->x, right->z, y);
    result->y = y;
    f32 z = left->z*right->w;
    z = fmadds(left->w, right->z, z);
    z = fmadds(left->x, right->y, z);
    z = fnmsubs(left->y, right->x, z);
    result->z = z;
}

/* 025F232C */
void mDoMtx_MtxToRot(u32 p, u32 d) {
    auto* mtx = mDoMtxAt(p);
    auto* rotation = mDoMtxAnglesAt(d);
    f32 z = mtx->m[2][2], x = mtx->m[0][2];
    f32 len = mDoMtx_sqrt(fmadds(x, x, z*z));
    s32 rx = mDoMtx_atan2s(-mtx->m[1][2], len);
    rotation->x = (s16)rx;
    if (rx==0x4000 || rx==-0x4000) {
        rotation->z = 0;
        f32 a = -mtx->m[2][0], b = mtx->m[0][0];
        rotation->y = (s16)mDoMtx_atan2s(a, b);
    }
    else {
        f32 a = mtx->m[0][2], b = mtx->m[2][2];
        rotation->y = (s16)mDoMtx_atan2s(a, b);
        a = mtx->m[1][0];
        b = mtx->m[1][1];
        rotation->z = (s16)mDoMtx_atan2s(a, b);
    }
}

/* 025F23EC */
s32 mDoMtx_push() {
    u32 end = gabi::ea(l_stackPointers.end.get()), next = gabi::ea(l_stackPointers.next.get());
    if (next>=end) {
        mDoMtx_assert("m_Do_mtx.cpp", 0x2DBu, "next < end");
        return 0;
    }
    next = gabi::ea(l_stackPointers.next.get());
    l_stackPointers.next = mDoMtxAt(next + sizeof(Mtx34));
    mDoMtx_copy(gabi::ea(&l_now), next);
    return 1;
}

/* 025F2468 */
s32 mDoMtx_pop() {
    u32 next = gabi::ea(l_stackPointers.next.get());
    if (next<=gabi::ea(&l_buffer)) {
        mDoMtx_assert("m_Do_mtx.cpp", 0x2F1u, "next > buffer");
        return 0;
    }
    next-=48;
    l_stackPointers.next = mDoMtxAt(next);
    mDoMtx_copy(next, gabi::ea(&l_now));
    return 1;
}

/* 025F24E0 */
void mDoMtx_transM(f32 x, f32 y, f32 z) {
    gabi::Local<Mtx34> m;
    mDoMtx_trans(gabi::ea(m.get()), x, y, z);
    mDoMtx_concat(gabi::ea(&l_now), gabi::ea(m.get()), gabi::ea(&l_now));
}

/* 025F2518 */
void mDoMtx_scaleM(f32 x, f32 y, f32 z) {
    gabi::Local<Mtx34> m;
    mDoMtx_scale(gabi::ea(m.get()), x, y, z);
    mDoMtx_concat(gabi::ea(&l_now), gabi::ea(m.get()), gabi::ea(&l_now));
}

/* 025F2550 */
void mDoMtx_lYrotM(s32 a) {
    gabi::Local<Mtx34> m;
    mDoMtx_YrotS(gabi::ea(m.get()), a>>16);
    mDoMtx_concat(gabi::ea(&l_now), gabi::ea(m.get()), gabi::ea(&l_now));
}

/* 025F2590 */
void mDoMtx_rYrotM(f32 a) {
    gabi::Local<Mtx34> m;
    mDoMtx_rotRad(gabi::ea(m.get()), u32('Y'), a);
    mDoMtx_concat(gabi::ea(&l_now), gabi::ea(m.get()), gabi::ea(&l_now));
}

/* 025F25CC */
void mDoMtx_quatM(u32 q) {
    gabi::Local<Mtx34> m;
    mDoMtx_quat(gabi::ea(m.get()), q);
    mDoMtx_concat(gabi::ea(&l_now), gabi::ea(m.get()), gabi::ea(&l_now));
}

// HD: Compiler-generated statics and stack pointers for this translation unit.
/* 025F2608 */
void __sinit_m_Do_mtx_cpp() {
    l_headerStatics.mZeroWords[3] = 0;
    l_headerStatics.mZeroWords[2] = 0;
    l_headerStatics.mZeroWords[1] = 0;
    l_headerStatics.mZeroWords[0] = 0;
    mDoMtx_registerStatic(gabi::ea(&l_records.mFirst));
    f32 lo = l_negativePi, hi = l_pi;
    l_headerStatics.mNegativePi = lo;
    l_headerStatics.mPi = hi;
    mDoMtx_initFlag(gabi::ea(&l_headerStatics.mFlag));
    mDoMtx_registerStatic(gabi::ea(&l_records.mSecond));
    mDoMtx_initByte(gabi::ea(&l_headerStatics.mByte));
    mDoMtx_registerStatic(gabi::ea(&l_records.mThird));
    l_stackPointers.next = &l_buffer.buffer[0];
    f32 zero = l_zero;
    l_stackPointers.end = &l_buffer.buffer[16];
    l_quatStack.field_0x4.y = zero;
    l_quatStack.field_0x4.w = zero;
    l_quatStack.field_0x118 = &l_quatStack.field_0x114;
    l_quatStack.field_0x114 = &l_quatStack.field_0x14[0];
    l_quatStack.field_0x4.z = zero;
    l_quatStack.field_0x0 = &l_quatStack.field_0x4;
    l_quatStack.field_0x4.x = zero;
}

/* WWHD glue
str 1005904C "m_Do_mtx.cpp" in 025F23EC
str 1005905C "next < end" in 025F23EC
str 10059078 "m_Do_mtx.cpp" in 025F2468
str 10059068 "next > buffer" in 025F2468
ext 028E9098 void mDoMtx_identity(u32)
ext 028E9108 void mDoMtx_concat(u32, u32, u32)
ext 028E90D4 void mDoMtx_copy(u32, u32)
ext 0201ADE0 void mDoMtx_vectorSubtract(u32, u32, u32)
ext 0201B31C void mDoMtx_vectorNormalize(u32, u32)
ext 0201B080 void mDoMtx_vectorCross(u32, u32, u32)
ext 0201B47C s32 mDoMtx_vectorNormalizeRS(u32)
ext 028E8F44 f32 mDoMtx_vectorDot(u32, u32)
ext 028E9684 void mDoMtx_sdkLookAt(u32, u32, u32, u32)
ext 028F4384 f32 mDoMtx_sqrt(f32)
ext 020195B0 s32 mDoMtx_atan2s(f32, f32)
ext 0273AA24 void mDoMtx_assert(const char*, u32, const char*)
ext 028E93CC void mDoMtx_trans(u32, f32, f32, f32)
ext 028E945C void mDoMtx_scale(u32, f32, f32, f32)
ext 028E98C0 void mDoMtx_rotRad(u32, u32, f32)
ext 028E8C78 void mDoMtx_quat(u32, u32)
ext 028F026C void mDoMtx_registerStatic(u32)
ext 028ED6F8 void mDoMtx_initFlag(u32)
ext 028EAB2C void mDoMtx_initByte(u32)
data l_zero 10059044
data l_one 10059048
data l_vectorEpsilon 100030B8
data l_negativePi 10059088
data l_pi 1005908C
data l_headerStatics 1048D0B0
data l_records 101F48C4
data l_now 1048D0CC
data l_buffer 1048D0FC
data l_stackPointers 101F48E8
data l_quatStack 1048D3FC
*/
