/** Signed angle, degree and spherical coordinate helpers, with tww layouts and names.
* HD keeps allocating null-this constructors and explicit result addresses. Private source. */
#include "SSystem/SComponent/c_angle.h"

static f32 l_degreeToSigned;
static f32 l_signedToDegree;
static f32 l_signedToNormal;
static f32 l_zero;
static f32 l_negativeHalfTurn;
static f32 l_halfTurn;
static f32 l_initialNegativePi;
static f32 l_initialPi;
static f32 l_interfaceNegativePi;
static f32 l_interfacePi;
static f64 l_doubleZero;
static cAngleHeaderStatics l_headerStatics;
static cAngleHeaderStatics l_interfaceHeaderStatics;
static cAngleStaticRecords l_records;
static cAngleStaticRecords l_interfaceRecords;
static cAngleConstants l_angles;

// HD: Preserve u32 entry arguments and result addresses. Typed views expose the tww fields.
/* inlined into allocating constructors */
static u32 cAngle_allocateThis(u32 address, u32 size) {
    return address ? address : cAngle_alloc(size);
}

/* 02006584 */
void cSAngle_Val(u32 i_this, s16 v) {
    auto* angle = cSAngleAt(i_this);
    angle->mAngle = v;
}

/* 0200658C */
u32 cSAngle_construct(u32 i_this, u32 v) {
    i_this = cAngle_allocateThis(i_this, 2);
    if (i_this) cSAngle_Val(i_this, v);
    return i_this;
}

/* 020065DC */
u32 cSAngle_scalarPlus(u32 out, s16 v, u32 a) {
    return cSAngle_construct(out, (s16)(v + cSAngleAt(a)->mAngle));
}

/* 020065EC */
u32 cSAngle_scalarMinus(u32 out, s16 v, u32 a) {
    return cSAngle_construct(out, (s16)(v - cSAngleAt(a)->mAngle));
}

/* 020065FC */
u32 cSAngle_constructDefault(u32 i_this) {
    i_this = cAngle_allocateThis(i_this, 2);
    auto* angle = cSAngleAt(i_this);
    if (i_this) angle->mAngle = 0;
    return i_this;
}

/* 02006638 */
void cSAngle_copyVal(u32 i_this, u32 a) {
    auto* angle = cSAngleAt(i_this);
    angle->mAngle = cSAngleAt(a)->mAngle;
}

/* 02006644 */
u32 cSAngle_constructCopy(u32 i_this, u32 a) {
    i_this = cAngle_allocateThis(i_this, 2);
    if (i_this) cSAngle_copyVal(i_this, a);
    return i_this;
}

/* 02006694 */
void cSAngle_degreeVal(u32 i_this, f32 v) {
    auto* angle = cSAngleAt(i_this);
    angle->mAngle = (s16)gabi::ftoi(gabi::fmuls_ppc(v, l_degreeToSigned));
}

/* 020066C0 */
u32 cSAngle_constructDegree(u32 i_this, f32 v) {
    i_this = cAngle_allocateThis(i_this, 2);
    if (i_this) cSAngle_degreeVal(i_this, v);
    return i_this;
}

/* 02006720 */
f32 cSAngle_Degree(u32 i_this) {
    auto* angle = cSAngleAt(i_this);
    return gabi::fmuls_ppc((f32)angle->mAngle, l_signedToDegree);
}

/* 02006760 */
f32 cSAngle_Radian(u32 i_this) {
    auto* angle = cSAngleAt(i_this);
    f32 scale = gabi::fmuls_ppc(l_headerStatics.mPi, l_signedToNormal);
    return gabi::fmuls_ppc((f32)angle->mAngle, scale);
}

/* 020067AC */
f32 cSAngle_Norm(u32 i_this) {
    auto* angle = cSAngleAt(i_this);
    return gabi::fmuls_ppc((f32)angle->mAngle, l_signedToNormal);
}

/* 020067EC */
s16 cSAngle_Abs(u32 i_this) {
    auto* angle = cSAngleAt(i_this);
    s32 v = angle->mAngle;
    return (s16)(v < 0?-v:v);
}

/* 02006804 */
s16 cSAngle_Inv(u32 i_this) {
    auto* angle = cSAngleAt(i_this);
    return (s16)(angle->mAngle - 0x8000);
}

/* 02006814 */
f32 cSAngle_Sin(u32 i_this) {
    f32 r = cSAngle_Radian(i_this);
    return cAngle_sin(r);
}

/* 02006838 */
f32 cSAngle_Cos(u32 i_this) {
    f32 r = cSAngle_Radian(i_this);
    return cAngle_cos(r);
}

/* 0200685C */
f32 cSAngle_Tan(u32 i_this) {
    f32 r = cSAngle_Radian(i_this);
    return cAngle_tan(r);
}

/* 02006880 */
u32 cSAngle_negate(u32 i_this, u32 out) {
    auto* angle = cSAngleAt(i_this);
    return cSAngle_construct(out, (s16) - angle->mAngle);
}

/* 02006894 */
u32 cSAngle_add(u32 i_this, u32 out, u32 q) {
    auto* angle = cSAngleAt(i_this);
    return cSAngle_construct(out, (s16)(angle->mAngle + cSAngleAt(q)->mAngle));
}

/* 020068B0 */
u32 cSAngle_subtract(u32 i_this, u32 out, u32 q) {
    auto* angle = cSAngleAt(i_this);
    return cSAngle_construct(out, (s16)(angle->mAngle - cSAngleAt(q)->mAngle));
}

/* 020068CC */
void cSAngle_addAssign(u32 i_this, u32 q) {
    auto* angle = cSAngleAt(i_this);
    angle->mAngle = (s16)(angle->mAngle + cSAngleAt(q)->mAngle);
}

/* 020068E0 */
void cSAngle_subtractAssign(u32 i_this, u32 q) {
    auto* angle = cSAngleAt(i_this);
    angle->mAngle = (s16)(angle->mAngle - cSAngleAt(q)->mAngle);
}

/* 020068F4 */
u32 cSAngle_addScalar(u32 i_this, u32 out, s16 v) {
    auto* angle = cSAngleAt(i_this);
    return cSAngle_construct(out, (s16)(angle->mAngle + v));
}

/* 02006908 */
u32 cSAngle_subtractScalar(u32 i_this, u32 out, s16 v) {
    auto* angle = cSAngleAt(i_this);
    return cSAngle_construct(out, (s16)(angle->mAngle - v));
}

/* 0200691C */
void cSAngle_addScalarAssign(u32 i_this, s16 v) {
    auto* angle = cSAngleAt(i_this);
    angle->mAngle = (s16)(angle->mAngle + v);
}

/* 0200692C */
void cSAngle_subtractScalarAssign(u32 i_this, s16 v) {
    auto* angle = cSAngleAt(i_this);
    angle->mAngle = (s16)(angle->mAngle - v);
}

/* 0200693C */
u32 cSAngle_multiply(u32 i_this, u32 out, f32 v) {
    auto* angle = cSAngleAt(i_this);
    return cSAngle_construct(out, (s16)gabi::ftoi(gabi::fmuls_ppc((f32)angle->mAngle, v)));
}

/* 020069A0 */
void cSAngle_multiplyAssign(u32 i_this, f32 v) {
    auto* angle = cSAngleAt(i_this);
    angle->mAngle = (s16)gabi::ftoi(gabi::fmuls_ppc((f32)angle->mAngle, v));
}

/* 020069EC */
u32 cDegree_Formal(u32 i_this) {
    auto* degree = cDegreeAt(i_this);
    f32 v = cAngle_Adjust(degree->mDegree, l_negativeHalfTurn, l_halfTurn);
    degree->mDegree = v;
    return i_this;
}

/* 02006A34 */
u32 cDegree_Val(u32 i_this, f32 v) {
    auto* degree = cDegreeAt(i_this);
    degree->mDegree = v;
    return cDegree_Formal(i_this);
}

/* 02006A3C */
u32 cDegree_construct(u32 i_this, f32 v) {
    i_this = cAngle_allocateThis(i_this, 4);
    if (i_this) cDegree_Val(i_this, v);
    return i_this;
}

/* 02006AA4 */
f32 cDegree_Radian(u32 i_this) {
    auto* degree = cDegreeAt(i_this);
    f32 scale = l_headerStatics.mPi / l_halfTurn;
    return gabi::fmuls_ppc(degree->mDegree, scale);
}

/* 02006AC4 */
f32 cDegree_Sin(u32 i_this) {
    f32 r = cDegree_Radian(i_this);
    return cAngle_sin(r);
}

/* 02006AE8 */
f32 cDegree_Cos(u32 i_this) {
    f32 r = cDegree_Radian(i_this);
    return cAngle_cos(r);
}

/* 02006B0C */
u32 cSPolar_constructDefault(u32 i_this) {
    i_this = cAngle_allocateThis(i_this, 8);
    auto* polar = cSPolarAt(i_this);
    if (i_this) {
        polar->mRadial = l_zero;
        cSAngle_constructDefault(gabi::ea(&polar->mAngle1));
        cSAngle_constructDefault(gabi::ea(&polar->mAngle2));
    }
    return i_this;
}

/* 02006B68 */
u32 cSPolar_Formal(u32 i_this) {
    auto* polar = cSPolarAt(i_this);
    if (polar->mRadial < l_zero) {
        polar->mRadial = -polar->mRadial;
        gabi::Local<cSAngle> a, b;
        u32 q = cSAngle_construct(gabi::ea(a.get()), (s16) - 32768);
        cSAngle_subtract(q, gabi::ea(b.get()), gabi::ea(&polar->mAngle1));
        cSAngle_copyVal(gabi::ea(&polar->mAngle1), gabi::ea(b.get()));
        s16 v = cSAngle_Inv(gabi::ea(&polar->mAngle2));
        cSAngle_Val(gabi::ea(&polar->mAngle2), v);
    }
    s16 v = polar->mAngle1.mAngle;
    if (v < 0 && v != -32768) {
        gabi::Local<cSAngle> a;
        cSAngle_negate(gabi::ea(&polar->mAngle1), gabi::ea(a.get()));
        cSAngle_copyVal(gabi::ea(&polar->mAngle1), gabi::ea(a.get()));
        s16 w = cSAngle_Inv(gabi::ea(&polar->mAngle2));
        cSAngle_Val(gabi::ea(&polar->mAngle2), w);
    }
    return i_this;
}

/* 02006C28 */
void cSPolar_Val(u32 i_this, u32 a, u32 b, f32 r) {
    auto* polar = cSPolarAt(i_this);
    polar->mRadial = r;
    gabi::Local<cSAngle> t;
    u32 q = cSAngle_construct(gabi::ea(t.get()), a);
    polar->mAngle1.mAngle = cSAngleAt(q)->mAngle;
    q = cSAngle_construct(gabi::ea(t.get()), b);
    polar->mAngle2.mAngle = cSAngleAt(q)->mAngle;
    cSPolar_Formal(i_this);
}

/* 02006C8C */
void cSPolar_vectorVal(u32 i_this, u32 v) {
    auto* polar = cSPolarAt(i_this);
    f32 x = gabi::at<cAngleXyz>(v)->x, z = gabi::at<cAngleXyz>(v)->z, y = gabi::at<cAngleXyz>(v)->y;
    f64 h = (f64)gabi::fmuls_ppc(z, z) + (f64)gabi::fmuls_ppc(x, x), full = h + (f64)gabi::fmuls_ppc(y, y);
    f32 zero = l_zero, hr = zero, r = zero;
    f64 dz = l_doubleZero;
    if (h > dz) hr = cAngle_sqrt((f32)h);
    if (full > dz) r = cAngle_sqrt((f32)full);
    polar->mRadial = r;
    s16 a = cAngle_atan2s(hr, y);
    cSAngle_Val(gabi::ea(&polar->mAngle1), a);
    s16 b = cAngle_atan2s(x, z);
    cSAngle_Val(gabi::ea(&polar->mAngle2), b);
    cSPolar_Formal(i_this);
}

/* 02006DE8 */
u32 cSPolar_constructVector(u32 i_this, u32 v) {
    i_this = cAngle_allocateThis(i_this, 8);
    auto* polar = cSPolarAt(i_this);
    if (i_this) {
        cSAngle_constructDefault(gabi::ea(&polar->mAngle1));
        cSAngle_constructDefault(gabi::ea(&polar->mAngle2));
        cSPolar_vectorVal(i_this, v);
    }
    return i_this;
}

/* 02006E50 */
void cSPolar_Xyz(u32 i_this, u32 out) {
    auto* polar = cSPolarAt(i_this);
    f32 a = cSAngle_Radian(gabi::ea(&polar->mAngle1)), sa = cAngle_sin(a);
    f32 radialSin = gabi::fmuls_ppc(polar->mRadial, sa);
    f32 b = cSAngle_Radian(gabi::ea(&polar->mAngle2)), sb = cAngle_sin(b);
    f32 x = gabi::fmuls_ppc(radialSin, sb);
    a = cSAngle_Radian(gabi::ea(&polar->mAngle1));
    f32 ca = cAngle_cos(a), y = gabi::fmuls_ppc(polar->mRadial, ca);
    b = cSAngle_Radian(gabi::ea(&polar->mAngle2));
    f32 cb = cAngle_cos(b), z = gabi::fmuls_ppc(radialSin, cb);
    out = cAngle_allocateThis(out, 12);
    if (out) {
        gabi::at<cAngleXyz>(out)->y = y;
        gabi::at<cAngleXyz>(out)->x = x;
        gabi::at<cAngleXyz>(out)->z = z;
    }
}

/* 02006F30 */
u32 cSGlobe_Formal(u32 i_this) {
    auto* globe = cSGlobeAt(i_this);
    if (globe->mRadius < l_zero) {
        globe->mRadius = -globe->mRadius;
        gabi::Local<cSAngle> t;
        cSAngle_negate(gabi::ea(&globe->mAzimuth), gabi::ea(t.get()));
        globe->mAzimuth.mAngle = t->mAngle;
        s16 v = cSAngle_Inv(gabi::ea(&globe->mInclination));
        cSAngle_Val(gabi::ea(&globe->mInclination), v);
    }
    s16 a = globe->mAzimuth.mAngle;
    if (a < -16384 || a > 16384) {
        gabi::Local<cSAngle> t, u;
        u32 q = cSAngle_construct(gabi::ea(t.get()), (s16) - 32768);
        cSAngle_subtract(q, gabi::ea(u.get()), gabi::ea(&globe->mAzimuth));
        globe->mAzimuth.mAngle = u->mAngle;
        s16 v = cSAngle_Inv(gabi::ea(&globe->mInclination));
        cSAngle_Val(gabi::ea(&globe->mInclination), v);
    }
    return i_this;
}

/* 02006FE4 */
void cSGlobe_Val(u32 i_this, u32 a, u32 b, f32 r) {
    auto* globe = cSGlobeAt(i_this);
    globe->mRadius = r;
    gabi::Local<cSAngle> t;
    u32 q = cSAngle_construct(gabi::ea(t.get()), a);
    globe->mAzimuth.mAngle = cSAngleAt(q)->mAngle;
    q = cSAngle_construct(gabi::ea(t.get()), b);
    globe->mInclination.mAngle = cSAngleAt(q)->mAngle;
    cSGlobe_Formal(i_this);
}

/* 02007048 */
void cSPolar_Globe(u32 i_this, u32 out) {
    auto* polar = cSPolarAt(i_this);
    s16 a = (s16)(16384 - polar->mAngle1.mAngle), b = polar->mAngle2.mAngle;
    f32 r = polar->mRadial;
    cSGlobe_Val(out, a, b, r);
}

/* 02007068 */
u32 cSGlobe_construct(u32 i_this, u32 a, u32 b, f32 r) {
    i_this = cAngle_allocateThis(i_this, 8);
    auto* globe = cSGlobeAt(i_this);
    if (i_this) {
        cSAngle_constructDefault(gabi::ea(&globe->mAzimuth));
        cSAngle_constructDefault(gabi::ea(&globe->mInclination));
        cSGlobe_Val(i_this, a, b, r);
    }
    return i_this;
}

/* 02007100 */
u32 cSGlobe_constructDefault(u32 i_this) {
    i_this = cAngle_allocateThis(i_this, 8);
    auto* globe = cSGlobeAt(i_this);
    if (i_this) {
        globe->mRadius = l_zero;
        cSAngle_constructDefault(gabi::ea(&globe->mAzimuth));
        cSAngle_constructDefault(gabi::ea(&globe->mInclination));
    }
    return i_this;
}

/* 0200715C */
u32 cSGlobe_copyVal(u32 i_this, u32 q) {
    auto* globe = cSGlobeAt(i_this);
    // HD: The lfs/stfs copy preserves the radius word, including NaN payloads.
    cSGlobe_copyRadiusWord(globe, cSGlobeAt(q));
    globe->mAzimuth.mAngle = cSGlobeAt(q)->mAzimuth.mAngle;
    globe->mInclination.mAngle = cSGlobeAt(q)->mInclination.mAngle;
    return cSGlobe_Formal(i_this);
}

/* 02007178 */
u32 cSGlobe_constructCopy(u32 i_this, u32 q) {
    i_this = cAngle_allocateThis(i_this, 8);
    auto* globe = cSGlobeAt(i_this);
    if (i_this) {
        cSAngle_constructDefault(gabi::ea(&globe->mAzimuth));
        cSAngle_constructDefault(gabi::ea(&globe->mInclination));
        cSGlobe_copyVal(i_this, q);
    }
    return i_this;
}

/* 020071E0 */
void cSGlobe_angleVal(u32 i_this, u32 a, u32 b, f32 r) {
    auto* globe = cSGlobeAt(i_this);
    globe->mRadius = r;
    gabi::Local<cSAngle> t;
    u32 q = cSAngle_construct(gabi::ea(t.get()), cSAngleAt(a)->mAngle);
    globe->mAzimuth.mAngle = cSAngleAt(q)->mAngle;
    q = cSAngle_construct(gabi::ea(t.get()), cSAngleAt(b)->mAngle);
    globe->mInclination.mAngle = cSAngleAt(q)->mAngle;
    cSGlobe_Formal(i_this);
}

/* 02007248 */
u32 cSGlobe_constructAngles(u32 i_this, u32 a, u32 b, f32 r) {
    i_this = cAngle_allocateThis(i_this, 8);
    auto* globe = cSGlobeAt(i_this);
    if (i_this) {
        cSAngle_constructDefault(gabi::ea(&globe->mAzimuth));
        cSAngle_constructDefault(gabi::ea(&globe->mInclination));
        cSGlobe_angleVal(i_this, a, b, r);
    }
    return i_this;
}

/* 020072E0 */
void cSGlobe_vectorVal(u32 i_this, u32 v) {
    gabi::Local<cSPolar> t;
    cSPolar_constructVector(gabi::ea(t.get()), v);
    cSPolar_Globe(gabi::ea(t.get()), i_this);
    cSGlobe_Formal(i_this);
}

/* 02007324 */
u32 cSGlobe_constructVector(u32 i_this, u32 v) {
    i_this = cAngle_allocateThis(i_this, 8);
    auto* globe = cSGlobeAt(i_this);
    if (i_this) {
        cSAngle_constructDefault(gabi::ea(&globe->mAzimuth));
        cSAngle_constructDefault(gabi::ea(&globe->mInclination));
        cSGlobe_vectorVal(i_this, v);
    }
    return i_this;
}

/* 0200738C */
void cSGlobe_Polar(u32 i_this, u32 out) {
    auto* globe = cSGlobeAt(i_this);
    s16 a = (s16)(16384 - globe->mAzimuth.mAngle), b = globe->mInclination.mAngle;
    f32 r = globe->mRadius;
    cSPolar_Val(out, a, b, r);
}

/* 020073AC */
void cSGlobe_Xyz(u32 i_this, u32 out) {
    gabi::Local<cSPolar> t;
    cSPolar_constructDefault(gabi::ea(t.get()));
    cSGlobe_Polar(i_this, gabi::ea(t.get()));
    cSPolar_Xyz(gabi::ea(t.get()), out);
}

/* 02007400 */
u32 cSGlobe_Invert(u32 i_this) {
    auto* globe = cSGlobeAt(i_this);
    globe->mRadius = -globe->mRadius;
    return cSGlobe_Formal(i_this);
}

/* 020074FC */
f32 cAngle_Adjust(f32 v, f32 lo, f32 hi) {
    f32 period = hi - lo;
    while (!(v < hi)) v = v - period;
    while (v < lo) v = v + period;
    return v;
}

// HD: Compiler - generated header statics and allocating constructors are verified entries.
/* 02007410 */
void cAngle_staticInit() {
    for (auto& word : l_headerStatics.mZeroWords) word = 0;
    cAngle_registerStatic(&l_records.mFirst);
    l_headerStatics.mNegativePi = l_initialNegativePi;
    l_headerStatics.mPi = l_initialPi;
    cAngle_initFlag(&l_headerStatics.mFlag);
    cAngle_registerStatic(&l_records.mSecond);
    cAngle_initByte(&l_headerStatics.mByte);
    cAngle_registerStatic(&l_records.mThird);
    cSAngle_construct(gabi::ea(&l_angles.mZero), (s16)0);
    cSAngle_construct(gabi::ea(&l_angles.mOne), (s16)182);
    cSAngle_construct(gabi::ea(&l_angles.mRightAngle), (s16)16384);
    cSAngle_construct(gabi::ea(&l_angles.mHalfTurn), (s16) - 32768);
    cSAngle_construct(gabi::ea(&l_angles.mThreeQuarterTurn), (s16) - 16384);
}

/* 0200752C */
void cAngle_interfaceStaticInit() {
    // HD: The header - only interface TU initializes its words in a different order.
    l_interfaceHeaderStatics.mZeroWords[2] = 0;
    l_interfaceHeaderStatics.mZeroWords[0] = 0;
    l_interfaceHeaderStatics.mZeroWords[3] = 0;
    l_interfaceHeaderStatics.mZeroWords[1] = 0;
    cAngle_registerStatic(&l_interfaceRecords.mFirst);
    f32 negativePi = l_interfaceNegativePi, positivePi = l_interfacePi;
    l_interfaceHeaderStatics.mNegativePi = negativePi;
    l_interfaceHeaderStatics.mPi = positivePi;
    cAngle_initFlag(&l_interfaceHeaderStatics.mFlag);
    cAngle_registerStatic(&l_interfaceRecords.mSecond);
    cAngle_initByte(&l_interfaceHeaderStatics.mByte);
    cAngle_registerStatic(&l_interfaceRecords.mThird);
}

/* WWHD glue
ext 0273AD10 u32 cAngle_alloc(u32)
ext 028F43F8 f32 cAngle_sin(f32)
ext 028F4BE0 f32 cAngle_cos(f32)
ext 028F423C f32 cAngle_tan(f32)
ext 028F4384 f32 cAngle_sqrt(f32)
ext 020195B0 s16 cAngle_atan2s(f32, f32)
ext 028F026C void cAngle_registerStatic(void*)
ext 028ED6F8 void cAngle_initFlag(void*)
ext 028EAB2C void cAngle_initByte(void*)
data l_headerStatics 101FF338
data l_interfaceHeaderStatics 101FF360
data l_angles 101FF354
data l_records 1018C454
data l_interfaceRecords 1018C478
data l_doubleZero 10000B08
data l_degreeToSigned 10000AD8
data l_signedToDegree 10000AE8
data l_signedToNormal 10000AF8
data l_zero 10000AFC
data l_negativeHalfTurn 10000B00
data l_halfTurn 10000B04
data l_initialNegativePi 10000B10
data l_initialPi 10000B14
data l_interfaceNegativePi 10000B18
data l_interfacePi 10000B1C
*/
