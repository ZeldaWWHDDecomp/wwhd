/* 3D collision and projection helpers, following the GameCube c_m3d layout.
 * HD arithmetic, assertion reloads and copy ordering retain the verified Cafe behavior. */
#include "SSystem/SComponent/c_m3d.h"
#include <cmath>

// HD: these BSS slots are initialized only by the compiler-generated routines below.
static u32 l_geometryHeaderWords[4] = {0, 0, 0, 0};
static u32 l_neighborHeaderWords[4] = {0, 0, 0, 0};
static f32 l_geometryNegativePi = 0.0f;
static f32 l_geometryPositivePi = 0.0f;
static f32 l_neighborNegativePi = 0.0f;
static f32 l_neighborPositivePi = 0.0f;
static f32 l_neighborPiMin = -3.1415927410125732f;
static f32 l_neighborPiMax = 3.1415927410125732f;

static f32 l_zero = 0.0f;
static f32 G_CM3D_F_ABS_MIN = 0.000003814697265625f;
static f32 l_one = 1.0f;
static f32 l_planeNormalMin = 0.02f;
static f32 l_negativeOne = -1.0f;
static f32 l_triangleMargin = 0.005f;
static f32 l_triangleUpper = 20.0f;
static f32 l_triangleLower = -20.0f;
static f32 l_triangleTolerance = 0.008f;
static f32 l_four = 4.0f;
static f32 l_half = 0.5f;
static f32 l_negativeZero = -0.0f;
static f32 l_farDistance = 1000000000.0f;
static f32 l_negativePi = -3.1415927410125732f;
static f32 l_positivePi = 3.1415927410125732f;

static f32 vectorComponent(const void* vector, u32 axis) {
    const cXyz* position = static_cast<const cXyz*>(vector);
    switch (axis) {
    case 0:
        return position->x;
    case 1:
        return position->y;
    default:
        return position->z;
    }
}

static void* ptr(u32 a) { return gabi::at<void>(a); }

// HD: GHS passes the ninth floating argument as a double in the outgoing stack area.
static s32 callInclusion(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy, f32 px, f32 py) {
    gabi::ArgPack args{gabi::cpu};
    args.put(ax);
    args.put(ay);
    args.put(bx);
    args.put(by);
    args.put(cx);
    args.put(cy);
    args.put(px);
    args.put(py);
    f64 tolerance = l_triangleMargin;
    u64 bits;
    std::memcpy(&bits, &tolerance, 8);
    args.ns = 2;
    args.stk[0] = u32(bits >> 32);
    args.stk[1] = u32(bits);
    gabi::do_call(gabi::cpu, args, 0x02011D2C, 0);
    return gabi::result<s32>(gabi::cpu);
}
// All coordinates are captured before the first guest call, as in the HD routines.
static s32 projectedTriangle(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy, f32 px, f32 py,
                             bool frontOnly = false) {
    if (!callInclusion(ax, ay, bx, by, cx, cy, px, py))
        return 0;
    if (frontOnly) {
        f32 limit = l_triangleLower;
        if (cM3d_VectorProduct2d(ax, ay, bx, by, px, py) < limit)
            return 0;
        if (cM3d_VectorProduct2d(bx, by, cx, cy, px, py) < limit)
            return 0;
        return !(cM3d_VectorProduct2d(cx, cy, ax, ay, px, py) < limit);
    }
    f32 first = cM3d_VectorProduct2d(ax, ay, bx, by, px, py), upper = l_triangleUpper;
    if (!(first > upper)) {
        f32 second = cM3d_VectorProduct2d(bx, by, cx, cy, px, py);
        if (!(second > upper)) {
            f32 third = cM3d_VectorProduct2d(cx, cy, ax, ay, px, py);
            if (!(third > upper))
                return 1;
        }
    }
    f32 lower = l_triangleLower;
    if (first < lower)
        return 0;
    if (cM3d_VectorProduct2d(bx, by, cx, cy, px, py) < lower)
        return 0;
    return !(cM3d_VectorProduct2d(cx, cy, ax, ay, px, py) < lower);
}

static void checkFloat(f32 value, const char* file, u32 line, const char* condition) {
    u32 bits;
    std::memcpy(&bits, &value, 4);
    if ((bits << 1) > 0xFF000000u)
        cM3d_assert(file, line, condition);
}
static void copyVectorWords(u32 src, u32 dst) {
    gabi::store<u32>(dst, gabi::load<u32>(src));
    gabi::store<u32>(dst + 4, gabi::load<u32>(src + 4));
    gabi::store<u32>(dst + 8, gabi::load<u32>(src + 8));
}

// HD: unlike the GC line overload, a zero-length line returns immediately.
static f32 linePointProjection(void* start, void* end, void* point, void* out, const char* file,
                               u32 firstLine, const char* conditionX, const char* conditionY,
                               const char* conditionZ) {
    gabi::Local<cXyz> direction, relative, scaled;
    cM3dSDK_PSVECSubtract(end, start, direction.get());
    f32 length = cM3dSDK_PSVECSquareMag(direction.get());
    if (std::fabs(length) < G_CM3D_F_ABS_MIN) {
        copyVectorWords(gabi::ea(point), gabi::ea(out));
        return l_zero;
    }
    cM3dSDK_PSVECSubtract(point, start, relative.get());
    f32 t = cM3dSDK_PSVECDotProduct(relative.get(), direction.get()) / length;
    cM3dSDK_PSVECScale(direction.get(), scaled.get(), t);
    cM3dSDK_PSVECAdd(scaled.get(), start, out);

    checkFloat(f32(static_cast<cXyz*>(out)->x), file, firstLine, conditionX);
    checkFloat(f32(static_cast<cXyz*>(out)->y), file, firstLine + 1, conditionY);
    checkFloat(f32(static_cast<cXyz*>(out)->z), file, firstLine + 2, conditionZ);
    return t;
}

static s32 projectedTriangleCached(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy, f32 px, f32 py,
                                   f32 tolerance, f32 upper, f32 lower) {
    gabi::ArgPack args{gabi::cpu};
    args.put(ax);
    args.put(ay);
    args.put(bx);
    args.put(by);
    args.put(cx);
    args.put(cy);
    args.put(px);
    args.put(py);
    f64 margin = tolerance;
    u64 bits;
    std::memcpy(&bits, &margin, 8);
    args.ns = 2;
    args.stk[0] = u32(bits >> 32);
    args.stk[1] = u32(bits);
    gabi::do_call(gabi::cpu, args, 0x02011D2C, 0);
    if (!gabi::result<s32>(gabi::cpu))
        return 0;
    f32 first = cM3d_VectorProduct2d(ax, ay, bx, by, px, py);
    if (!(first > upper)) {
        if (!(cM3d_VectorProduct2d(bx, by, cx, cy, px, py) > upper)) {
            if (!(cM3d_VectorProduct2d(cx, cy, ax, ay, px, py) > upper))
                return 1;
        }
    }
    if (first < lower)
        return 0;
    if (cM3d_VectorProduct2d(bx, by, cx, cy, px, py) < lower)
        return 0;
    return !(cM3d_VectorProduct2d(cx, cy, ax, ay, px, py) < lower);
}

static bool inclusionAxis(f32 a, f32 b, f32 c, f32 point, f32 margin) {
    f32 delta = a - b, low = delta >= 0 ? b : a, high = delta >= 0 ? a : b;
    if (low > c)
        low = c;
    else if (high < c)
        high = c;
    return !(low - margin > point) && !(high + margin < point);
}

/* 02010924 */
void cM3d_InDivPos1(void* a, void* b, void* out, f32 scale) {
    gabi::Local<cXyz> tmp;
    cM3dSDK_PSVECScale(b, tmp.get(), scale);
    cM3dSDK_PSVECAdd(tmp.get(), a, out);
}

/* 02010974 */
void cM3d_InDivPos2(void* a, void* b, void* out, f32 scale) {
    gabi::Local<cXyz> tmp;
    cM3dSDK_PSVECSubtract(b, a, tmp.get());
    cM3d_InDivPos1(a, tmp.get(), out, scale);
}

/* 020109E8 */
f32 cM3d_Len2dSq(f32 ax, f32 ay, f32 bx, f32 by) {
    f32 y = ay - by, x = ax - bx;
    return gabi::fmadds(x, x, y * y);
}

/* 020109FC */
s32 cM3d_Len2dSqPntAndSegLine(void* ox, void* oy, void* distance, f32 px, f32 py, f32 ax, f32 ay,
                              f32 bx, f32 by) {
    f32 dy = by - ay, dx = bx - ax, len = gabi::fmadds(dx, dx, dy * dy), zero = l_zero;
    if (std::fabs(len) < G_CM3D_F_ABS_MIN) {
        *static_cast<be<f32>*>(distance) = zero;
        return 0;
    }
    f32 t = gabi::fmadds(px - ax, dx, (py - ay) * dy) / len;
    s32 within = !(t < zero) && !(t > l_one);
    f32 x = gabi::fmadds(dx, t, ax), y = gabi::fmadds(dy, t, ay);
    *static_cast<be<f32>*>(ox) = x;
    *static_cast<be<f32>*>(oy) = y;
    f32 d = cM3d_Len2dSq(f32(*static_cast<be<f32>*>(ox)), y, px, py);
    *static_cast<be<f32>*>(distance) = d;
    return within;
}

/* 02010AE4 */
s32 cM3d_Len3dSqPntAndSegLine(void* line, void* point, void* out, void* distance) {
    gabi::Local<cXyz> direction, relative;
    cM3dSDK_PSVECSubtract(ptr(gabi::ea(line) + 12), line, direction.get());
    f32 len = cM3dSDK_PSVECDotProduct(direction.get(), direction.get()), zero = l_zero;
    if (std::fabs(len) < G_CM3D_F_ABS_MIN) {
        *static_cast<be<f32>*>(distance) = zero;
        return 0;
    }
    cM3dSDK_PSVECSubtract(point, line, relative.get());
    f32 t = cM3dSDK_PSVECDotProduct(relative.get(), direction.get()) / len;
    s32 within = !(t < zero) && !(t > l_one);
    cM3dSDK_PSVECScale(direction.get(), direction.get(), t);
    cM3dSDK_PSVECAdd(direction.get(), line, out);
    f32 d = cM3dSDK_PSVECSquareDistance(out, point);
    *static_cast<be<f32>*>(distance) = d;
    return within;
}

/* 02010C50 */
f32 cM3d_SignedLenPlaAndPos(void* plane, void* point) {
    const cM3dGPla* planeView = static_cast<const cM3dGPla*>(plane);
    f32 len = cM3dSDK_PSVECMag(plane);
    if (std::fabs(len) < G_CM3D_F_ABS_MIN)
        return l_zero;
    f32 dot = cM3dSDK_PSVECDotProduct(plane, point);
    return (dot + f32(planeView->mD)) / len;
}

/* 02010CFC */
f32 cM3d_VectorProduct2d(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy) {
    return gabi::fmsubs(bx - ax, cy - ay, (by - ay) * (cx - ax));
}

/* 02010D18 */
void cM3d_CalcPlaHD(void* a, void* b, void* c, void* normal, void* distance) {
    gabi::Local<cXyz> u, v;
    cM3dSDK_PSVECSubtract(b, a, u.get());
    cM3dSDK_PSVECSubtract(c, a, v.get());
    cM3dSDK_PSVECCrossProduct(u.get(), v.get(), normal);
    f32 len = cM3dSDK_PSVECMag(normal);
    if (std::fabs(len) < l_planeNormalMin) {
        f32 z = l_zero;

        static_cast<cXyz*>(normal)->y = z;
        *static_cast<be<f32>*>(distance) = z;
        static_cast<cXyz*>(normal)->x = z;
        static_cast<cXyz*>(normal)->z = z;
    } else {
        cM3dSDK_PSVECScale(normal, normal, l_one / len);
        f32 dot = cM3dSDK_PSVECDotProduct(normal, a);
        *static_cast<be<f32>*>(distance) = -dot;
    }
}

/* 02010E14 */
s32 cM3d_Cross_AabCyl(void* aab, void* cylinder) {
    const cM3dGCyl* cylinderView = static_cast<const cM3dGCyl*>(cylinder);
    const cM3dGAab* aabView = static_cast<const cM3dGAab*>(aab);

    f32 x = f32(cylinderView->mCenter.x), r = f32(cylinderView->mRadius);
    if (f32(aabView->mMin.x) > x + r)
        return 0;
    if (f32(aabView->mMax.x) < x - r)
        return 0;
    f32 z = f32(cylinderView->mCenter.z);
    if (f32(aabView->mMin.z) > z + r)
        return 0;
    if (f32(aabView->mMax.z) < z - r)
        return 0;
    f32 y = f32(cylinderView->mCenter.y), h = f32(cylinderView->mHeight);
    if (f32(aabView->mMin.y) > y + h)
        return 0;
    return !(f32(aabView->mMax.y) < y);
}

/* 020119B0 */
s32 cM3d_Cross_AabSph(void* aab, void* sphere) {
    const cM3dGSph* sphereView = static_cast<const cM3dGSph*>(sphere);
    const cM3dGAab* aabView = static_cast<const cM3dGAab*>(aab);

    f32 r = f32(sphereView->mRadius), x = f32(sphereView->mCenter.x);
    if (f32(aabView->mMin.x) > x + r)
        return 0;
    if (f32(aabView->mMax.x) < x - r)
        return 0;
    f32 z = f32(sphereView->mCenter.z);
    if (f32(aabView->mMin.z) > z + r)
        return 0;
    if (f32(aabView->mMax.z) < z - r)
        return 0;
    f32 y = f32(sphereView->mCenter.y);
    if (f32(aabView->mMin.y) > y + r)
        return 0;
    return !(f32(aabView->mMax.y) < y - r);
}

/* 02011A30 */
s32 cM3d_Check_LinLin(void* lineA, void* lineB, void* positionA, void* positionB) {
    gabi::Local<cXyz> a, b, delta;
    cM3dSDK_PSVECSubtract(ptr(gabi::ea(lineA) + 12), lineA, a.get());
    cM3dSDK_PSVECSubtract(ptr(gabi::ea(lineB) + 12), lineB, b.get());
    f32 lengthA = cM3dSDK_PSVECMag(a.get()), lengthB = cM3dSDK_PSVECMag(b.get()),
        epsilon = G_CM3D_F_ABS_MIN;
    if (std::fabs(lengthA) < epsilon || std::fabs(lengthB) < epsilon)
        return 1;
    f32 one = l_one, invA = one / lengthA, invB = one / lengthB;
    cM3dSDK_PSVECScale(a.get(), a.get(), invA);
    cM3dSDK_PSVECScale(b.get(), b.get(), invB);
    cM3dSDK_PSVECSubtract(lineA, lineB, delta.get());
    f32 relation = -cM3dSDK_PSVECDotProduct(a.get(), b.get()),
        projectionA = cM3dSDK_PSVECDotProduct(delta.get(), a.get());
    cM3dSDK_PSVECSquareMag(delta.get());
    f32 denominator = std::fabs(gabi::fnmsubs(relation, relation, one));
    f32 ta, tb;
    s32 result;
    if (!(std::fabs(denominator) < epsilon)) {
        f32 projectionB = -cM3dSDK_PSVECDotProduct(delta.get(), b.get()),
            inverse = one / denominator;
        tb = gabi::fmsubs(relation, projectionA, projectionB) * inverse;
        ta = gabi::fmsubs(relation, projectionB, projectionA) * inverse;
        result = 3;
    } else {
        f32 zero = l_zero;
        ta = -projectionA;
        tb = zero;
        if (ta < zero || ta > lengthA) {
            ta = gabi::fmsubs(lengthB, relation, projectionA);
            tb = lengthB;
        }
        f32 projectionB = cM3dSDK_PSVECDotProduct(delta.get(), b.get());
        if (ta < zero || ta > lengthA) {
            tb = projectionB;
            ta = zero;
        }
        if (tb < zero || tb > lengthB) {
            tb = gabi::fnmsubs(lengthA, relation, projectionB);
            ta = lengthA;
        }
        result = 2;
    }
    *static_cast<be<f32>*>(positionA) = gabi::fmuls_ppc(ta, invA);
    *static_cast<be<f32>*>(positionB) = gabi::fmuls_ppc(tb, invB);
    return result;
}

/* 02010E94 */
s32 cM3d_PlaneSegmentPosition(void* start, void* end, void* out, f32 startDistance,
                              f32 endDistance) {
    f32 delta = startDistance - endDistance;
    if (std::fabs(delta) < G_CM3D_F_ABS_MIN) {
        u32 e = gabi::ea(end), o = gabi::ea(out);
        gabi::store<u32>(o, gabi::load<u32>(e));
        gabi::store<u32>(o + 4, gabi::load<u32>(e + 4));
        gabi::store<u32>(o + 8, gabi::load<u32>(e + 8));
        return 0;
    }
    cM3d_InDivPos2(start, end, out, startDistance / delta);
    return 1;
}

/* 02010F00 */
s32 cM3d_Cross_LinPla(void* line, void* plane, void* out, u32 front, u32 back) {
    const cM3dGPla* planeView = static_cast<const cM3dGPla*>(plane);
    u32 l = gabi::ea(line), o = gabi::ea(out);
    f32 first = cM3dSDK_PSVECDotProduct(plane, line);
    first += f32(planeView->mD);
    f32 last = cM3dSDK_PSVECDotProduct(plane, ptr(l + 12));
    last += f32(planeView->mD);
    f32 zero = l_zero;
    bool cross = false;
    if (!(first * last > zero)) {
        if (first < zero || last > zero)
            cross = back != 0;
        else
            cross = front != 0;
    }
    if (cross)
        return cM3d_PlaneSegmentPosition(line, ptr(l + 12), out, first, last);
    gabi::store<u32>(o, gabi::load<u32>(l + 12));
    gabi::store<u32>(o + 4, gabi::load<u32>(l + 16));
    gabi::store<u32>(o + 8, gabi::load<u32>(l + 20));
    return 0;
}

/* 02010FFC */
s32 cM3d_Cross_MinMaxBoxLine(void* minimum, void* maximum, void* start, void* end) {
    u32 startCode = 0, endCode = 0;
    f32 a[3], b[3], min[3], max[3];
    for (u32 axis : {0u, 2u, 1u}) {
        a[axis] = vectorComponent(start, axis);
        max[axis] = vectorComponent(maximum, axis);
        b[axis] = vectorComponent(end, axis);
        u32 upper = 1u << (axis * 2), lower = upper << 1;
        if (a[axis] > max[axis]) {
            if (b[axis] > max[axis])
                return 0;
            startCode |= upper;
        } else if (b[axis] > max[axis])
            endCode |= upper;
        min[axis] = vectorComponent(minimum, axis);
        if (!(startCode & upper) && a[axis] < min[axis]) {
            if (!(endCode & upper) && b[axis] < min[axis])
                return 0;
            startCode |= lower;
        } else if (!(endCode & upper) && b[axis] < min[axis])
            endCode |= lower;
    }
    if (startCode == 0 || endCode == 0)
        return 1;
    auto bevel2 = [&](const f32* p) {
        u32 bits = 0;
        f32 diff = p[1] - p[0], sum = p[0] + p[1];
        if (diff > max[1] - min[0])
            bits |= 1;
        if (diff < min[1] - max[0])
            bits |= 2;
        if (sum > max[0] + max[1])
            bits |= 4;
        if (sum < min[0] + min[1])
            bits |= 8;
        diff = p[1] - p[2];
        sum = p[2] + p[1];
        if (diff > max[1] - min[2])
            bits |= 16;
        if (diff < min[1] - max[2])
            bits |= 32;
        if (sum > max[2] + max[1])
            bits |= 64;
        if (sum < min[2] + min[1])
            bits |= 128;
        diff = p[0] - p[2];
        sum = p[2] + p[0];
        if (diff > max[0] - min[2])
            bits |= 256;
        if (diff < min[0] - max[2])
            bits |= 512;
        if (sum > max[2] + max[0])
            bits |= 1024;
        if (sum < min[2] + min[0])
            bits |= 2048;
        return bits;
    };
    startCode |= bevel2(a) << 8;
    endCode |= bevel2(b) << 8;
    if (startCode & endCode)
        return 0;
    auto bevel3 = [&](const f32* p) {
        u32 bits = 0;
        if ((p[0] + p[1]) + p[2] > (max[0] + max[1]) + max[2])
            bits |= 1;
        if ((p[1] - p[0]) + p[2] > (max[1] - min[0]) + max[2])
            bits |= 2;
        if ((p[1] - p[0]) - p[2] > (max[1] - min[0]) - min[2])
            bits |= 4;
        if ((p[0] + p[1]) - p[2] > (max[0] + max[1]) - min[2])
            bits |= 8;
        if ((p[0] - p[1]) + p[2] > (max[0] - min[1]) + max[2])
            bits |= 16;
        // HD: both bevel bits use this same expression.
        if ((-p[0] - p[1]) + p[2] > (-min[0] - min[1]) + max[2])
            bits |= 96;
        if ((-p[0] - p[1]) - p[2] > (-min[0] - min[1]) - min[2])
            bits |= 128;
        return bits;
    };
    startCode |= bevel3(a) << 24;
    endCode |= bevel3(b) << 24;
    if (startCode & endCode)
        return 0;
    gabi::Local<cXyz> first, last, cross;
    first->x = a[0];
    first->y = a[1];
    first->z = a[2];
    last->x = b[0];
    last->y = b[1];
    last->z = b[2];
    gabi::Local<cM3dGLin> line;
    cM3dGLin_constructHD(line.get(), first.get(), last.get());
    f32 zero = l_zero, one = l_one, negative = 0;
    u32 changed = startCode ^ endCode;
    // HD: each plane has separate storage; constructors may leave untouched input bytes.
    gabi::Local<cM3dGPla> planes[6];
    for (u32 face = 0; face < 6; face++) {
        if (face == 1)
            negative = l_negativeOne;
        if (!(changed & (1u << face)))
            continue;
        cM3dGPla* plane = planes[face].get();
        cM3dGPla_constructHD(plane);
        u32 axis = face / 2;
        bool high = (face & 1) == 0;
        plane->mNormal.x = axis == 0 ? (high ? one : negative) : zero;
        plane->mNormal.y = axis == 1 ? (high ? one : negative) : zero;
        plane->mNormal.z = axis == 2 ? (high ? one : negative) : zero;
        plane->mD = high ? -vectorComponent(maximum, axis) : vectorComponent(minimum, axis);
        if (!cM3d_Cross_LinPla(line.get(), plane, cross.get(), 1u, 1u))
            continue;

        bool in = true;
        for (u32 other = 0; other < 3; other++)
            if (other != axis) {
                f32 v = vectorComponent(cross.get(), other);
                if (vectorComponent(minimum, other) > v || v > vectorComponent(maximum, other)) {
                    in = false;
                    break;
                }
            }
        if (in)
            return 1;
    }
    return 0;
}

/* 02011D2C */
s32 cM3d_InclusionCheckPosIn3PosBox2d(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy, f32 px,
                                      f32 py) {
    f32 margin = f32(gabi::load<f64>(gabi::cpu->r[1] + 8));
    gabi::store<f32>(gabi::cpu->r[1] + 8, margin);
    f32 dx = ax - bx, lo = dx >= 0 ? bx : ax, hi = dx >= 0 ? ax : bx;
    if (!(lo > cx)) {
        if (hi < cx)
            hi = cx;
    } else
        lo = cx;
    if (lo - margin > px)
        return 0;
    if (hi + margin < px)
        return 0;
    f32 dy = ay - by;
    lo = dy >= 0 ? by : ay;
    hi = dy >= 0 ? ay : by;
    if (!(lo > cy)) {
        if (hi < cy)
            hi = cy;
    } else
        lo = cy;
    if (lo - margin > py)
        return 0;
    return !(hi + margin < py);
}

/* 02011DF0 */
s32 cM3d_CrossX_LinTri_proc(void* triangle, void* point) {
    const cM3dGTri* triangleView = static_cast<const cM3dGTri*>(triangle);
    const cXyz* pointView = static_cast<const cXyz*>(point);

    if (std::fabs(f32(triangleView->mNormal.x)) < G_CM3D_F_ABS_MIN)
        return 0;
    return projectedTriangle(f32(triangleView->mA.y), f32(triangleView->mA.z),
                             f32(triangleView->mB.y), f32(triangleView->mB.z),
                             f32(triangleView->mC.y), f32(triangleView->mC.z), f32(pointView->y),
                             f32(pointView->z));
}

/* 0201204C */
s32 cM3d_CrossY_Tri(void* triangle, void* point) {
    const cM3dGTri* triangleView = static_cast<const cM3dGTri*>(triangle);
    const cXyz* pointView = static_cast<const cXyz*>(point);

    if (std::fabs(f32(triangleView->mNormal.y)) < G_CM3D_F_ABS_MIN)
        return 0;
    return projectedTriangle(f32(triangleView->mA.z), f32(triangleView->mA.x),
                             f32(triangleView->mB.z), f32(triangleView->mB.x),
                             f32(triangleView->mC.z), f32(triangleView->mC.x), f32(pointView->z),
                             f32(pointView->x));
}

/* 020122A8 */
s32 cM3d_CrossY_TriVertices(void* a, void* b, void* c, void* plane, void* point) {
    const cM3dGPla* planeView = static_cast<const cM3dGPla*>(plane);
    const cXyz* pointView = static_cast<const cXyz*>(point);
    if (std::fabs(f32(planeView->mNormal.y)) < G_CM3D_F_ABS_MIN)
        return 0;

    return projectedTriangle(f32(static_cast<cXyz*>(a)->z), f32(static_cast<cXyz*>(a)->x),
                             f32(static_cast<cXyz*>(b)->z), f32(static_cast<cXyz*>(b)->x),
                             f32(static_cast<cXyz*>(c)->z), f32(static_cast<cXyz*>(c)->x),
                             f32(pointView->z), f32(pointView->x));
}

/* 02012504 */
s32 cM3d_CrossY_Tri_Front(void* a, void* b, void* c, void* point) {
    const cXyz* pointView = static_cast<const cXyz*>(point);

    return projectedTriangle(f32(static_cast<cXyz*>(a)->z), f32(static_cast<cXyz*>(a)->x),
                             f32(static_cast<cXyz*>(b)->z), f32(static_cast<cXyz*>(b)->x),
                             f32(static_cast<cXyz*>(c)->z), f32(static_cast<cXyz*>(c)->x),
                             f32(pointView->z), f32(pointView->x), true);
}

/* 020126EC */
s32 cM3d_CrossY_TriPosition(void* triangle, void* point, void* height) {
    const cM3dGTri* triangleView = static_cast<const cM3dGTri*>(triangle);
    const cXyz* pointView = static_cast<const cXyz*>(point);
    if (!cM3d_CrossY_Tri(triangle, point))
        return 0;

    f32 product = f32(triangleView->mNormal.x) * f32(pointView->x);
    f32 value = gabi::fnmsubs(f32(triangleView->mNormal.z), f32(pointView->z), -product);
    value = (value - f32(triangleView->mD)) / f32(triangleView->mNormal.y);
    *static_cast<be<f32>*>(height) = value;
    return 1;
}

/* 02012760 */
s32 cM3d_CrossY_TriRange(void* triangle, void* point, void* range, void* height) {
    const cM3dGTri* triangleView = static_cast<const cM3dGTri*>(triangle);
    const cXyz* pointView = static_cast<const cXyz*>(point);

    if (std::fabs(f32(triangleView->mNormal.y)) < G_CM3D_F_ABS_MIN)
        return 0;
    gabi::Local<cXyz> work;
    work->x = f32(pointView->x);
    work->y = static_cast<be<f32>*>(range)[0];
    work->z = f32(pointView->z);
    f32 first = cM3dSDK_PSVECDotProduct(triangle, work.get());
    work->y = static_cast<be<f32>*>(range)[1];
    first += f32(triangleView->mD);
    f32 last = cM3dSDK_PSVECDotProduct(triangle, work.get()), zero = l_zero;
    last += f32(triangleView->mD);
    if (first > zero && last > zero)
        return 0;
    if (first < zero && last < zero)
        return 0;
    return cM3d_CrossY_TriPosition(triangle, point, height);
}

/* 0201288C */
s32 cM3d_CrossZ_LinTri_proc(void* triangle, void* point) {
    const cM3dGTri* triangleView = static_cast<const cM3dGTri*>(triangle);
    const cXyz* pointView = static_cast<const cXyz*>(point);

    if (std::fabs(f32(triangleView->mNormal.z)) < G_CM3D_F_ABS_MIN)
        return 0;
    return projectedTriangle(f32(triangleView->mA.x), f32(triangleView->mA.y),
                             f32(triangleView->mB.x), f32(triangleView->mB.y),
                             f32(triangleView->mC.x), f32(triangleView->mC.y), f32(pointView->x),
                             f32(pointView->y));
}

/* 02012AE8 */
s32 cM3d_Cross_LinTri(void* line, void* triangle, void* out, u32 front, u32 back) {
    const cM3dGTri* triangleView = static_cast<const cM3dGTri*>(triangle);
    if (!cM3d_Cross_LinPla(line, triangle, out, front, back))
        return 0;

    f32 nx = f32(triangleView->mNormal.x), threshold = l_triangleTolerance,
        margin = l_triangleMargin, lower = l_triangleLower, upper = l_triangleUpper;
    if (!(std::fabs(nx) < threshold) &&
        !projectedTriangleCached(
            f32(triangleView->mA.y), f32(triangleView->mA.z), f32(triangleView->mB.y),
            f32(triangleView->mB.z), f32(triangleView->mC.y), f32(triangleView->mC.z),
            f32(static_cast<cXyz*>(out)->y), f32(static_cast<cXyz*>(out)->z), margin, upper, lower))
        return 0;
    if (!(std::fabs(f32(triangleView->mNormal.y)) < threshold) &&
        !projectedTriangleCached(
            f32(triangleView->mA.z), f32(triangleView->mA.x), f32(triangleView->mB.z),
            f32(triangleView->mB.x), f32(triangleView->mC.z), f32(triangleView->mC.x),
            f32(static_cast<cXyz*>(out)->z), f32(static_cast<cXyz*>(out)->x), margin, upper, lower))
        return 0;
    if (!(std::fabs(f32(triangleView->mNormal.z)) < threshold) &&
        !projectedTriangleCached(
            f32(triangleView->mA.x), f32(triangleView->mA.y), f32(triangleView->mB.x),
            f32(triangleView->mB.y), f32(triangleView->mC.x), f32(triangleView->mC.y),
            f32(static_cast<cXyz*>(out)->x), f32(static_cast<cXyz*>(out)->y), margin, upper, lower))
        return 0;
    return 1;
}

/* 02013090 */
s32 cM3d_Cross_SphPnt(void* sphere, void* point) {
    const cM3dGSph* sphereView = static_cast<const cM3dGSph*>(sphere);
    f32 d = cM3dSDK_PSVECSquareDistance(sphere, point), r = f32(sphereView->mRadius);
    return !(d > r * r);
}

/* 020130D4 */
s32 cM3d_Cross_LinSph(void* line, void* sphere, void* out) {
    const cM3dGSph* sphereView = static_cast<const cM3dGSph*>(sphere);
    u32 l = gabi::ea(line);
    f32 radius = f32(sphereView->mRadius);
    for (u32 offset : {0u, 4u, 8u}) {
        f32 center = vectorComponent(sphere, offset / 4), start = vectorComponent(line, offset / 4),
            high = center + radius;
        if (high < start && high < vectorComponent(&static_cast<cM3dGLin*>(line)->mEnd, offset / 4))
            return 0;
        f32 low = center - radius;
        if (low > start && low > vectorComponent(&static_cast<cM3dGLin*>(line)->mEnd, offset / 4))
            return 0;
    }
    gabi::Local<cXyz> direction, relative, scaled;
    cM3dSDK_PSVECSubtract(ptr(l + 12), line, direction.get());
    f32 length = cM3dSDK_PSVECSquareMag(direction.get());
    if (std::fabs(length) < G_CM3D_F_ABS_MIN)
        return 0;
    cM3dSDK_PSVECSubtract(sphere, line, relative.get());
    f32 t = cM3dSDK_PSVECDotProduct(relative.get(), direction.get()) / length;
    f32 zero = l_zero;
    if (t < zero || t > l_one) {
        f32 a = cM3dSDK_PSVECSquareDistance(line, sphere),
            b = cM3dSDK_PSVECSquareDistance(ptr(l + 12), sphere);
        u32 chosen = a < b ? l : l + 12;
        if (!cM3d_Cross_SphPnt(sphere, ptr(chosen)))
            return 0;
        copyVectorWords(chosen, gabi::ea(out));
        return 1;
    }
    cM3dSDK_PSVECScale(direction.get(), scaled.get(), t);
    cM3dSDK_PSVECAdd(scaled.get(), line, out);
    f32 d = cM3dSDK_PSVECSquareDistance(out, sphere), r = f32(sphereView->mRadius);
    return !(d > r * r);
}

/* 02013380 */
s32 cM3d_Cross_LinSph_CrossPos(void* sphere, void* line, void* first, void* second) {
    const cM3dGSph* sphereView = static_cast<const cM3dGSph*>(sphere);
    gabi::Local<cXyz> direction, relative, scaled;
    u32 l = gabi::ea(line);
    cM3dSDK_PSVECSubtract(ptr(l + 12), line, direction.get());
    cM3dSDK_PSVECSubtract(line, sphere, relative.get());
    f32 a = cM3dSDK_PSVECDotProduct(direction.get(), direction.get());
    f32 b = cM3dSDK_PSVECDotProduct(direction.get(), relative.get());
    b = b + b;
    f32 c = cM3dSDK_PSVECDotProduct(relative.get(), relative.get()), epsilon = G_CM3D_F_ABS_MIN,
        r = f32(sphereView->mRadius);
    c = gabi::fnmsubs(r, r, c);
    auto position = [&](f32 t, void* out) {
        cM3dSDK_PSVECScale(direction.get(), scaled.get(), t);
        cM3dSDK_PSVECAdd(scaled.get(), line, out);
    };
    if (std::fabs(a) < epsilon) {
        if (std::fabs(b) < epsilon)
            return 0;
        position(-(c / b), first);
        return 1;
    }
    f32 four = l_four, discriminant = gabi::fmsubs(b, b, (four * a) * c);
    if (std::fabs(discriminant) < epsilon) {
        position(-(b / (a + a)), first);
        return 1;
    }
    if (discriminant < l_zero)
        return 0;
    f32 inverse = l_one / (a + a), root1 = cM3dSDK_sqrtfHD(discriminant),
        t1 = (root1 - b) * inverse;
    f32 negativeB = -b, root2 = cM3dSDK_sqrtfHD(discriminant), t2 = (negativeB - root2) * inverse;
    position(t1, first);
    position(t2, second);
    return 2;
}

/* 020135C8 */
s32 cM3d_Cross_CylSphDepthHD(void* cylinder, void* sphere, void* penetration) {
    const cM3dGSph* sphereView = static_cast<const cM3dGSph*>(sphere);
    const cM3dGCyl* cylinderView = static_cast<const cM3dGCyl*>(cylinder);

    checkFloat(
        f32(sphereView->mCenter.x), "c_m3d.cpp", 0x9A2,
        "!__generic(pnow_sph_center->x, , ,__isnand,__isnanf,__isnanl,,,)(pnow_sph_center->x)");
    checkFloat(
        f32(sphereView->mCenter.y), "c_m3d.cpp", 0x9A3,
        "!__generic(pnow_sph_center->y, , ,__isnand,__isnanf,__isnanl,,,)(pnow_sph_center->y)");
    checkFloat(
        f32(sphereView->mCenter.z), "c_m3d.cpp", 0x9A4,
        "!__generic(pnow_sph_center->z, , ,__isnand,__isnanf,__isnanl,,,)(pnow_sph_center->z)");
    checkFloat(f32(sphereView->mRadius), "c_m3d.cpp", 0x9A5,
               "!__generic(psph->GetR(), , ,__isnand,__isnanf,__isnanl,,,)(psph->GetR())");
    checkFloat(
        f32(cylinderView->mCenter.x), "c_m3d.cpp", 0x9A7,
        "!__generic(pnow_cyl_center->x, , ,__isnand,__isnanf,__isnanl,,,)(pnow_cyl_center->x)");
    checkFloat(
        f32(cylinderView->mCenter.y), "c_m3d.cpp", 0x9A8,
        "!__generic(pnow_cyl_center->y, , ,__isnand,__isnanf,__isnanl,,,)(pnow_cyl_center->y)");
    checkFloat(
        f32(cylinderView->mCenter.z), "c_m3d.cpp", 0x9A9,
        "!__generic(pnow_cyl_center->z, , ,__isnand,__isnanf,__isnanl,,,)(pnow_cyl_center->z)");
    checkFloat(f32(cylinderView->mHeight), "c_m3d.cpp", 0x9AA,
               "!__generic(pcyl->GetH(), , ,__isnand,__isnanf,__isnanl,,,)(pcyl->GetH())");
    f32 radius = f32(cylinderView->mRadius);
    checkFloat(radius, "c_m3d.cpp", 0x9AB,
               "!__generic(pcyl->GetR(), , ,__isnand,__isnanf,__isnanl,,,)(pcyl->GetR())");
    // HD: the failed radius check is the only path that reloads it before computing the sum.
    u32 bits;
    std::memcpy(&bits, &radius, 4);
    if ((bits << 1) > 0xFF000000u)
        radius = f32(cylinderView->mRadius);
    f32 sum = radius + f32(sphereView->mRadius);
    f32 d = cM3d_Len2dSq(f32(sphereView->mCenter.x), f32(sphereView->mCenter.z),
                         f32(cylinderView->mCenter.x), f32(cylinderView->mCenter.z));
    d = cM3dSDK_sqrtfHD(d);
    if (sum < d)
        return 0;
    f32 sy = f32(sphereView->mCenter.y), sr = f32(sphereView->mRadius),
        cy = f32(cylinderView->mCenter.y);
    if (sy + sr < cy)
        return 0;
    f32 height = f32(cylinderView->mHeight);
    if (sy - sr > cy + height)
        return 0;
    f32 overlap = sum - d;
    *static_cast<be<f32>*>(penetration) = overlap;
    checkFloat(overlap, "c_m3d.cpp", 0x9BC,
               "!__generic(*pcross_len, , ,__isnand,__isnanf,__isnanl,,,)(*pcross_len)");
    return 1;
}

/* 02013854 */
s32 cM3d_CylinderSpherePosition(void* cylinder, void* sphere, void* out, void* penetration) {
    const cM3dGSph* sphereView = static_cast<const cM3dGSph*>(sphere);
    const cM3dGCyl* cylinderView = static_cast<const cM3dGCyl*>(cylinder);
    u32 s = gabi::ea(sphere);
    checkFloat(
        f32(sphereView->mCenter.x), "c_m3d.cpp", 0x9D2,
        "!__generic(pnow_sph_center->x, , ,__isnand,__isnanf,__isnanl,,,)(pnow_sph_center->x)");
    checkFloat(
        f32(sphereView->mCenter.y), "c_m3d.cpp", 0x9D3,
        "!__generic(pnow_sph_center->y, , ,__isnand,__isnanf,__isnanl,,,)(pnow_sph_center->y)");
    checkFloat(
        f32(sphereView->mCenter.z), "c_m3d.cpp", 0x9D4,
        "!__generic(pnow_sph_center->z, , ,__isnand,__isnanf,__isnanl,,,)(pnow_sph_center->z)");
    checkFloat(f32(sphereView->mRadius), "c_m3d.cpp", 0x9D5,
               "!__generic(psph->GetR(), , ,__isnand,__isnanf,__isnanl,,,)(psph->GetR())");
    checkFloat(
        f32(cylinderView->mCenter.x), "c_m3d.cpp", 0x9D7,
        "!__generic(pnow_cyl_center->x, , ,__isnand,__isnanf,__isnanl,,,)(pnow_cyl_center->x)");
    checkFloat(
        f32(cylinderView->mCenter.y), "c_m3d.cpp", 0x9D8,
        "!__generic(pnow_cyl_center->y, , ,__isnand,__isnanf,__isnanl,,,)(pnow_cyl_center->y)");
    checkFloat(
        f32(cylinderView->mCenter.z), "c_m3d.cpp", 0x9D9,
        "!__generic(pnow_cyl_center->z, , ,__isnand,__isnanf,__isnanl,,,)(pnow_cyl_center->z)");
    checkFloat(f32(cylinderView->mHeight), "c_m3d.cpp", 0x9DA,
               "!__generic(pcyl->GetH(), , ,__isnand,__isnanf,__isnanl,,,)(pcyl->GetH())");
    f32 radius = f32(cylinderView->mRadius);
    checkFloat(radius, "c_m3d.cpp", 0x9DB,
               "!__generic(pcyl->GetR(), , ,__isnand,__isnanf,__isnanl,,,)(pcyl->GetR())");
    u32 bits;
    std::memcpy(&bits, &radius, 4);
    if ((bits << 1) > 0xFF000000u)
        radius = f32(cylinderView->mRadius);
    f32 sum = radius + f32(sphereView->mRadius),
        d = cM3d_Len2dSq(f32(sphereView->mCenter.x), f32(sphereView->mCenter.z),
                         f32(cylinderView->mCenter.x), f32(cylinderView->mCenter.z));
    d = cM3dSDK_sqrtfHD(d);
    if (sum < d)
        return 0;
    f32 sy = f32(sphereView->mCenter.y), sr = f32(sphereView->mRadius),
        cy = f32(cylinderView->mCenter.y);
    if (sy + sr < cy)
        return 0;
    if (sy - sr > cy + f32(cylinderView->mHeight))
        return 0;
    f32 epsilon = G_CM3D_F_ABS_MIN;
    *static_cast<be<f32>*>(penetration) = sum - d;
    bool copy = std::fabs(d) < epsilon;
    f32 ratio = 0;
    if (!copy) {
        ratio = f32(cylinderView->mRadius) / d;
        copy = ratio > l_one;
    }
    if (copy)
        copyVectorWords(s, gabi::ea(out));
    else {
        gabi::Local<cXyz> delta;
        cM3dSDK_PSVECSubtract(sphere, cylinder, delta.get());
        cM3dSDK_PSVECScale(delta.get(), delta.get(), ratio);
        cM3dSDK_PSVECAdd(delta.get(), cylinder, out);
    }
    checkFloat(f32(*static_cast<be<f32>*>(penetration)), "c_m3d.cpp", 0x9FA,
               "!__generic(*pcross_len, , ,__isnand,__isnanf,__isnanl,,,)(*pcross_len)");
    return 1;
}

/* 02013B78 */
s32 cM3d_Cross_SphSphDepth(void* a, void* b, void* penetration) {
    const cM3dGSph* aView = static_cast<const cM3dGSph*>(a);
    const cM3dGSph* bView = static_cast<const cM3dGSph*>(b);

    checkFloat(f32(aView->mCenter.x), "c_m3d.cpp", 0xA0D,
               "!__generic(i_a->GetCP()->x, , ,__isnand,__isnanf,__isnanl,,,)(i_a->GetCP()->x)");
    checkFloat(f32(aView->mCenter.y), "c_m3d.cpp", 0xA0E,
               "!__generic(i_a->GetCP()->y, , ,__isnand,__isnanf,__isnanl,,,)(i_a->GetCP()->y)");
    checkFloat(f32(aView->mCenter.z), "c_m3d.cpp", 0xA0F,
               "!__generic(i_a->GetCP()->z, , ,__isnand,__isnanf,__isnanl,,,)(i_a->GetCP()->z)");
    checkFloat(f32(aView->mRadius), "c_m3d.cpp", 0xA10,
               "!__generic(i_a->GetR(), , ,__isnand,__isnanf,__isnanl,,,)(i_a->GetR())");
    checkFloat(f32(bView->mCenter.x), "c_m3d.cpp", 0xA11,
               "!__generic(i_b->GetCP()->x, , ,__isnand,__isnanf,__isnanl,,,)(i_b->GetCP()->x)");
    checkFloat(f32(bView->mCenter.y), "c_m3d.cpp", 0xA12,
               "!__generic(i_b->GetCP()->y, , ,__isnand,__isnanf,__isnanl,,,)(i_b->GetCP()->y)");
    checkFloat(f32(bView->mCenter.z), "c_m3d.cpp", 0xA13,
               "!__generic(i_b->GetCP()->z, , ,__isnand,__isnanf,__isnanl,,,)(i_b->GetCP()->z)");
    checkFloat(f32(bView->mRadius), "c_m3d.cpp", 0xA14,
               "!__generic(i_b->GetR(), , ,__isnand,__isnanf,__isnanl,,,)(i_b->GetR())");
    gabi::Local<cXyz> delta;
    cM3dSDK_PSVECSubtract(a, b, delta.get());
    f32 d = cM3dSDK_PSVECMag(delta.get()), r2 = f32(bView->mRadius), r1 = f32(aView->mRadius),
        overlap = (r1 + r2) - d;
    if (overlap > G_CM3D_F_ABS_MIN) {
        *static_cast<be<f32>*>(penetration) = overlap;
        checkFloat(
            overlap, "c_m3d.cpp", 0xA1D,
            "!__generic(*i_pcc_crosslen, , ,__isnand,__isnanf,__isnanl,,,)(*i_pcc_crosslen)");
        return 1;
    }
    f32 zero = l_zero;
    *static_cast<be<f32>*>(penetration) = zero;
    checkFloat(zero, "c_m3d.cpp", 0xA22,
               "!__generic(*i_pcc_crosslen, , ,__isnand,__isnanf,__isnanl,,,)(*i_pcc_crosslen)");
    return 0;
}

/* 02013DC0 */
s32 cM3d_SphereSphereMetrics(void* a, void* b, void* distance, void* penetration) {
    const cM3dGSph* aView = static_cast<const cM3dGSph*>(a);
    const cM3dGSph* bView = static_cast<const cM3dGSph*>(b);

    checkFloat(f32(aView->mCenter.x), "c_m3d.cpp", 0xA35,
               "!__generic(i_a->GetCP()->x, , ,__isnand,__isnanf,__isnanl,,,)(i_a->GetCP()->x)");
    checkFloat(f32(aView->mCenter.y), "c_m3d.cpp", 0xA36,
               "!__generic(i_a->GetCP()->y, , ,__isnand,__isnanf,__isnanl,,,)(i_a->GetCP()->y)");
    checkFloat(f32(aView->mCenter.z), "c_m3d.cpp", 0xA37,
               "!__generic(i_a->GetCP()->z, , ,__isnand,__isnanf,__isnanl,,,)(i_a->GetCP()->z)");
    checkFloat(f32(aView->mRadius), "c_m3d.cpp", 0xA38,
               "!__generic(i_a->GetR(), , ,__isnand,__isnanf,__isnanl,,,)(i_a->GetR())");
    checkFloat(f32(bView->mCenter.x), "c_m3d.cpp", 0xA39,
               "!__generic(i_b->GetCP()->x, , ,__isnand,__isnanf,__isnanl,,,)(i_b->GetCP()->x)");
    checkFloat(f32(bView->mCenter.y), "c_m3d.cpp", 0xA3A,
               "!__generic(i_b->GetCP()->y, , ,__isnand,__isnanf,__isnanl,,,)(i_b->GetCP()->y)");
    checkFloat(f32(bView->mCenter.z), "c_m3d.cpp", 0xA3B,
               "!__generic(i_b->GetCP()->z, , ,__isnand,__isnanf,__isnanl,,,)(i_b->GetCP()->z)");
    checkFloat(f32(bView->mRadius), "c_m3d.cpp", 0xA3C,
               "!__generic(i_b->GetR(), , ,__isnand,__isnanf,__isnanl,,,)(i_b->GetR())");
    gabi::Local<cXyz> delta;
    cM3dSDK_PSVECSubtract(a, b, delta.get());
    f32 d = cM3dSDK_PSVECMag(delta.get());
    *static_cast<be<f32>*>(distance) = d;
    f32 overlap = (f32(aView->mRadius) + f32(bView->mRadius)) - d;
    if (overlap > G_CM3D_F_ABS_MIN) {
        *static_cast<be<f32>*>(penetration) = overlap;
        checkFloat(
            overlap, "c_m3d.cpp", 0xA46,
            "!__generic(*i_pcc_crosslen, , ,__isnand,__isnanf,__isnanl,,,)(*i_pcc_crosslen)");
        return 1;
    }
    f32 zero = l_zero;
    *static_cast<be<f32>*>(penetration) = zero;
    checkFloat(zero, "c_m3d.cpp", 0xA4B,
               "!__generic(*i_pcc_crosslen, , ,__isnand,__isnanf,__isnanl,,,)(*i_pcc_crosslen)");
    return 0;
}

/* 02014010 */
s32 cM3d_Cross_SphSphPosition(void* a, void* b, void* out) {
    const cM3dGSph* bView = static_cast<const cM3dGSph*>(b);
    gabi::Local<be<f32>> distance, penetration;
    gabi::Local<cXyz> delta;
    if (!cM3d_SphereSphereMetrics(a, b, distance.get(), penetration.get()))
        return 0;
    f32 d = distance->get();
    if (std::fabs(d) < G_CM3D_F_ABS_MIN)
        copyVectorWords(gabi::ea(a), gabi::ea(out));
    else {
        f32 ratio = f32(bView->mRadius) / d;
        cM3dSDK_PSVECSubtract(a, b, delta.get());
        cM3dSDK_PSVECScale(delta.get(), delta.get(), ratio);
        cM3dSDK_PSVECAdd(delta.get(), b, out);
    }
    return 1;
}

/* 02014140 */
void cM3d_CalcSphVsTriCrossPoint(void* sphere, void* triangle, void* out) {
    const cM3dGSph* sphereView = static_cast<const cM3dGSph*>(sphere);
    gabi::Local<cXyz> sum, midpoint;
    u32 t = gabi::ea(triangle);
    cM3dSDK_PSVECAdd(ptr(t + 20), ptr(t + 32), sum.get());
    cM3dSDK_PSVECScale(sum.get(), midpoint.get(), l_half);
    f32 d = cM3dSDK_PSVECSquareDistance(midpoint.get(), sphere);
    if (std::fabs(d) < G_CM3D_F_ABS_MIN)
        copyVectorWords(gabi::ea(sphere), gabi::ea(out));
    else
        cM3d_InDivPos2(sphere, midpoint.get(), out, f32(sphereView->mRadius) / d);
}

/* 02014200 */
s32 cM3d_Cross_SphTri(void* sphere, void* triangle, void* out) {
    const cM3dGSph* sphereView = static_cast<const cM3dGSph*>(sphere);
    const cM3dGTri* triangleView = static_cast<const cM3dGTri*>(triangle);

    f32 radius = f32(sphereView->mRadius);
    if (!inclusionAxis(f32(triangleView->mA.x), f32(triangleView->mB.x), f32(triangleView->mC.x),
                       f32(sphereView->mCenter.x), radius))
        return 0;
    if (!inclusionAxis(f32(triangleView->mA.z), f32(triangleView->mB.z), f32(triangleView->mC.z),
                       f32(sphereView->mCenter.z), radius))
        return 0;
    if (!inclusionAxis(f32(triangleView->mA.y), f32(triangleView->mB.y), f32(triangleView->mC.y),
                       f32(sphereView->mCenter.y), radius))
        return 0;
    f32 distance = cM3d_SignedLenPlaAndPos(triangle, sphere);
    if (std::fabs(distance) > f32(sphereView->mRadius))
        return 0;
    gabi::Local<cXyz> normalOffset, projected, a, b, intersection;
    cM3dSDK_PSVECScale(triangle, normalOffset.get(), distance);
    cM3dSDK_PSVECSubtract(sphere, normalOffset.get(), projected.get());
    f32 ny = f32(triangleView->mNormal.y), negativeZero = l_negativeZero, half = l_half,
        zero = l_zero, epsilon = G_CM3D_F_ABS_MIN, margin = l_triangleMargin;

    bool inside = false, tested = false;
    f32 ax = 0, ay = 0, az = 0, bx = 0, by = 0, bz = 0;
    if (std::fabs(ny) > half) {
        if (!(std::fabs(ny) < epsilon)) {
            az = f32(triangleView->mA.z);
            ax = f32(triangleView->mA.x);
            bz = f32(triangleView->mB.z);
            bx = f32(triangleView->mB.x);
            f32 cz = f32(triangleView->mC.z), cx = f32(triangleView->mC.x);
            inside = projectedTriangleCached(az, ax, bz, bx, cz, cx, f32(projected->z),
                                             f32(projected->x), margin, zero, negativeZero);
            tested = true;
            if (!inside) {
                by = f32(triangleView->mB.y);
                ay = f32(triangleView->mA.y);
            }
        }
    } else {
        f32 nx = f32(triangleView->mNormal.x);
        if (std::fabs(nx) > l_half) {
            if (!(std::fabs(nx) < epsilon)) {
                ay = f32(triangleView->mA.y);
                az = f32(triangleView->mA.z);
                by = f32(triangleView->mB.y);
                bz = f32(triangleView->mB.z);
                f32 cy = f32(triangleView->mC.y), cz = f32(triangleView->mC.z);
                inside = projectedTriangleCached(ay, az, by, bz, cy, cz, f32(projected->y),
                                                 f32(projected->z), margin, zero, negativeZero);
                tested = true;
                if (!inside) {
                    bx = f32(triangleView->mB.x);
                    ax = f32(triangleView->mA.x);
                }
            }
        } else if (!(std::fabs(f32(triangleView->mNormal.z)) < epsilon)) {
            ax = f32(triangleView->mA.x);
            ay = f32(triangleView->mA.y);
            bx = f32(triangleView->mB.x);
            by = f32(triangleView->mB.y);
            f32 cx = f32(triangleView->mC.x), cy = f32(triangleView->mC.y);
            inside = projectedTriangleCached(ax, ay, bx, by, cx, cy, f32(projected->x),
                                             f32(projected->y), margin, zero, negativeZero);
            tested = true;
            if (!inside) {
                bz = f32(triangleView->mB.z);
                az = f32(triangleView->mA.z);
            }
        }
    }
    if (inside) {
        if (out)
            cM3d_CalcSphVsTriCrossPoint(sphere, triangle, out);
        return 1;
    }
    if (!tested) {
        bx = f32(triangleView->mB.x);
        by = f32(triangleView->mB.y);
        ax = f32(triangleView->mA.x);
        az = f32(triangleView->mA.z);
        bz = f32(triangleView->mB.z);
        ay = f32(triangleView->mA.y);
    }
    a->x = ax;
    a->y = ay;
    a->z = az;
    b->x = bx;
    b->y = by;
    b->z = bz;
    gabi::Local<cM3dGLin> firstLine, secondLine, thirdLine;
    cM3dGLin_constructHD(firstLine.get(), a.get(), b.get());
    if (cM3d_Cross_LinSph(firstLine.get(), sphere, intersection.get())) {
        if (out)
            cM3d_CalcSphVsTriCrossPoint(sphere, triangle, out);
        return 1;
    }
    a->x = f32(triangleView->mC.x);
    a->y = f32(triangleView->mC.y);
    a->z = f32(triangleView->mC.z);
    b->x = f32(triangleView->mB.x);
    b->y = f32(triangleView->mB.y);
    b->z = f32(triangleView->mB.z);
    cM3dGLin_constructHD(secondLine.get(), b.get(), a.get());
    if (cM3d_Cross_LinSph(secondLine.get(), sphere, intersection.get())) {
        if (out)
            cM3d_CalcSphVsTriCrossPoint(sphere, triangle, out);
        return 1;
    }
    b->x = f32(triangleView->mC.x);
    b->y = f32(triangleView->mC.y);
    b->z = f32(triangleView->mC.z);
    a->x = f32(triangleView->mA.x);
    a->y = f32(triangleView->mA.y);
    a->z = f32(triangleView->mA.z);
    cM3dGLin_constructHD(thirdLine.get(), b.get(), a.get());
    if (!cM3d_Cross_LinSph(thirdLine.get(), sphere, intersection.get()))
        return 0;
    if (out)
        cM3d_CalcSphVsTriCrossPoint(sphere, triangle, out);
    return 1;
}

/* 02014AF8 */
s32 cM3d_Cross_CylCylDepth(void* a, void* b, void* penetration) {
    const cM3dGCyl* aView = static_cast<const cM3dGCyl*>(a);
    const cM3dGCyl* bView = static_cast<const cM3dGCyl*>(b);

    checkFloat(f32(aView->mCenter.x), "c_m3d.cpp", 0xB3C,
               "!__generic(c1.x, , ,__isnand,__isnanf,__isnanl,,,)(c1.x)");
    checkFloat(f32(aView->mCenter.y), "c_m3d.cpp", 0xB3D,
               "!__generic(c1.y, , ,__isnand,__isnanf,__isnanl,,,)(c1.y)");
    checkFloat(f32(aView->mCenter.z), "c_m3d.cpp", 0xB3E,
               "!__generic(c1.z, , ,__isnand,__isnanf,__isnanl,,,)(c1.z)");
    checkFloat(f32(aView->mRadius), "c_m3d.cpp", 0xB3F,
               "!__generic(i_cyl1->GetR(), , ,__isnand,__isnanf,__isnanl,,,)(i_cyl1->GetR())");
    checkFloat(f32(aView->mHeight), "c_m3d.cpp", 0xB40,
               "!__generic(i_cyl1->GetH(), , ,__isnand,__isnanf,__isnanl,,,)(i_cyl1->GetH())");
    checkFloat(f32(bView->mCenter.x), "c_m3d.cpp", 0xB42,
               "!__generic(c2.x, , ,__isnand,__isnanf,__isnanl,,,)(c2.x)");
    checkFloat(f32(bView->mCenter.y), "c_m3d.cpp", 0xB43,
               "!__generic(c2.y, , ,__isnand,__isnanf,__isnanl,,,)(c2.y)");
    checkFloat(f32(bView->mCenter.z), "c_m3d.cpp", 0xB44,
               "!__generic(c2.z, , ,__isnand,__isnanf,__isnanl,,,)(c2.z)");
    checkFloat(f32(bView->mRadius), "c_m3d.cpp", 0xB45,
               "!__generic(i_cyl2->GetR(), , ,__isnand,__isnanf,__isnanl,,,)(i_cyl2->GetR())");
    checkFloat(f32(bView->mHeight), "c_m3d.cpp", 0xB46,
               "!__generic(i_cyl2->GetH(), , ,__isnand,__isnanf,__isnanl,,,)(i_cyl2->GetH())");
    f32 dz = f32(aView->mCenter.z) - f32(bView->mCenter.z),
        r = f32(aView->mRadius) + f32(bView->mRadius),
        dx = f32(aView->mCenter.x) - f32(bView->mCenter.x), d = gabi::fmadds(dx, dx, dz * dz),
        zero = l_zero;
    if (d > r * r) {
        *static_cast<be<f32>*>(penetration) = zero;
        checkFloat(zero, "c_m3d.cpp", 0xB53,
                   "!__generic(*i_pcross_len, , ,__isnand,__isnanf,__isnanl,,,)(*i_pcross_len)");
        return 0;
    }
    f32 ay = f32(aView->mCenter.y), height = f32(aView->mHeight), by = f32(bView->mCenter.y);
    if (ay + height < by || ay > by + f32(bView->mHeight)) {
        *static_cast<be<f32>*>(penetration) = zero;
        checkFloat(zero, "c_m3d.cpp", 0xB5B,
                   "!__generic(*i_pcross_len, , ,__isnand,__isnanf,__isnanl,,,)(*i_pcross_len)");
        return 0;
    }
    d = cM3dSDK_sqrtfHD(d);
    f32 overlap = r - d;
    *static_cast<be<f32>*>(penetration) = overlap;
    checkFloat(overlap, "c_m3d.cpp", 0xB62,
               "!__generic(*i_pcross_len, , ,__isnand,__isnanf,__isnanl,,,)(*i_pcross_len)");
    return 1;
}

/* 02014E18 */
s32 cM3d_Cross_CylCylPosition(void* a, void* b, void* out) {
    const cM3dGCyl* aView = static_cast<const cM3dGCyl*>(a);
    const cM3dGCyl* bView = static_cast<const cM3dGCyl*>(b);
    u32 y = gabi::ea(b), o = gabi::ea(out);
    f32 dz = f32(aView->mCenter.z) - f32(bView->mCenter.z),
        dx = f32(aView->mCenter.x) - f32(bView->mCenter.x),
        r = f32(aView->mRadius) + f32(bView->mRadius), d = gabi::fmadds(dx, dx, dz * dz);
    if (d > r * r)
        return 0;
    f32 ay = f32(aView->mCenter.y), ah = f32(aView->mHeight), by = f32(bView->mCenter.y);
    if (ay + ah < by)
        return 0;
    if (ay > by + f32(bView->mHeight))
        return 0;
    d = cM3dSDK_sqrtfHD(d);
    if (std::fabs(d) < G_CM3D_F_ABS_MIN)
        copyVectorWords(y, o);
    else {
        f32 bh = f32(bView->mHeight), base = f32(bView->mCenter.y), half = l_half,
            radius = f32(bView->mRadius), height = gabi::fmadds(bh, half, base);
        static_cast<cXyz*>(out)->y = height;
        f32 low = f32(aView->mCenter.y), ratio = radius / d;
        if (height < low)
            static_cast<cXyz*>(out)->y = low;
        else {
            f32 top = low + f32(aView->mHeight);
            if (height > top)
                static_cast<cXyz*>(out)->y = top;
        }
        f32 ax = f32(aView->mCenter.x), bx = f32(bView->mCenter.x);
        static_cast<cXyz*>(out)->x = gabi::fmadds(ax - bx, ratio, bx);
        f32 bz = f32(bView->mCenter.z), az = f32(aView->mCenter.z);
        static_cast<cXyz*>(out)->z = gabi::fmadds(az - bz, ratio, bz);
    }
    return 1;
}

/* 020156D0 */
s32 cM3d_Cross_CylTriHD(void* cylinder, void* triangle, void* out) {
    const cM3dGTri* triangleView = static_cast<const cM3dGTri*>(triangle);
    const cM3dGCyl* cylinderView = static_cast<const cM3dGCyl*>(cylinder);
    u32 c = gabi::ea(cylinder), t = gabi::ea(triangle), o = gabi::ea(out);
    f32 firstY = f32(triangleView->mA.y), base = f32(cylinderView->mCenter.y),
        top = base + f32(cylinderView->mHeight);
    if (base > firstY && base > f32(triangleView->mB.y) && base > f32(triangleView->mC.y))
        return 0;
    if (top < firstY && top < f32(triangleView->mB.y) && top < f32(triangleView->mC.y))
        return 0;
    f32 infinity = l_farDistance, best = infinity;
    gabi::Local<cXyz> first, second;
    for (u32 edge = 0; edge < 3; edge++) {
        u32 a = t + (edge == 1 ? 44 : 20), b = t + (edge == 2 ? 44 : 32);
        if (cM3d_Cross_CylPntPnt(cylinder, ptr(a), ptr(b), first.get(), second.get())) {
            f32 d = cM3dSDK_PSVECSquareDistance(first.get(), ptr(a));
            if (edge == 0 || best > d) {
                u32 p = gabi::ea(first.get()), y = gabi::load<u32>(p + 4), x = gabi::load<u32>(p),
                    z = gabi::load<u32>(p + 8);
                gabi::store<u32>(o, x);
                gabi::store<u32>(o + 8, z);
                gabi::store<u32>(o + 4, y);
                best = d;
            }
        }
    }
    if (best != infinity)
        return 1;
    struct Range {
        be<f32> min, max;
    };
    gabi::Local<Range> range;
    gabi::Local<be<f32>> height;
    range->min = f32(cylinderView->mCenter.y);
    range->max = top;
    if (!cM3d_CrossY_TriRange(triangle, cylinder, range.get(), height.get()))
        return 0;
    gabi::Local<cXyz> center, sum, midpoint, delta;
    u32 a = gabi::ea(center.get());
    gabi::store<u32>(a, gabi::load<u32>(c));
    gabi::store<u32>(a + 4, gabi::load<u32>(c + 4));
    f32 y = height->get();
    u32 z = gabi::load<u32>(c + 8);
    center->y = y;
    gabi::store<u32>(a + 8, z);
    cM3dSDK_PSVECAdd(&static_cast<cM3dGTri*>(triangle)->mA, &static_cast<cM3dGTri*>(triangle)->mB,
                     sum.get());
    cM3dSDK_PSVECScale(sum.get(), midpoint.get(), l_half);
    cM3dSDK_PSVECSubtract(midpoint.get(), center.get(), delta.get());

    f32 dz = f32(delta->z), dx = f32(delta->x),
        distance = cM3dSDK_sqrtfHD(gabi::fmadds(dx, dx, dz * dz));
    if (std::fabs(distance) < G_CM3D_F_ABS_MIN) {

        f32 yy = f32(midpoint->y), xx = f32(midpoint->x);
        static_cast<cXyz*>(out)->y = yy;
        f32 zz = f32(midpoint->z);
        static_cast<cXyz*>(out)->x = xx;
        static_cast<cXyz*>(out)->z = zz;
    } else
        cM3d_InDivPos1(center.get(), delta.get(), out, f32(cylinderView->mRadius) / distance);
    return 1;
}

/* 02014FDC */
s32 cM3d_Cross_CylLin(void* cylinder, void* line, void* firstOut, void* secondOut) {
    const cM3dGCyl* cylinderView = static_cast<const cM3dGCyl*>(cylinder);
    u32 l = gabi::ea(line);
    f32 zero = l_zero, secondT = zero;
    if (cM3d_Cross_CylPnt(cylinder, line) && cM3d_Cross_CylPnt(cylinder, ptr(l + 12))) {
        copyVectorWords(l, gabi::ea(firstOut));
        copyVectorWords(l + 12, gabi::ea(secondOut));
        return 2;
    }
    gabi::Local<cXyz> relativeStart, relativeEnd, direction;
    gabi::Local<cXyz[4]> candidates;
    cM3dSDK_PSVECSubtract(line, cylinder, relativeStart.get());
    cM3dSDK_PSVECSubtract(ptr(l + 12), cylinder, relativeEnd.get());
    cM3dSDK_PSVECSubtract(relativeEnd.get(), relativeStart.get(), direction.get());
    u32 points = gabi::ea(candidates.get());
    f32 dy = f32(direction->y), epsilon = G_CM3D_F_ABS_MIN, radius = f32(cylinderView->mRadius),
        radiusSq = radius * radius, one = l_one;
    f32 dz, az, dx, ax, startY = 0;
    u32 mask = 0;
    if (!(std::fabs(dy) < epsilon)) {
        startY = f32(relativeStart->y);
        f32 t = -(startY / dy);
        if (!(t < zero) && !(t > one)) {
            dz = f32(direction->z);
            az = f32(relativeStart->z);
            dx = f32(direction->x);
            ax = f32(relativeStart->x);
            f32 z = gabi::fmadds(dz, t, az), x = gabi::fmadds(dx, t, ax);
            if (gabi::fmadds(x, x, z * z) < radiusSq) {
                (*candidates.get())[0].x = x + f32(cylinderView->mCenter.x);
                (*candidates.get())[0].y = f32(cylinderView->mCenter.y);
                (*candidates.get())[0].z = z + f32(cylinderView->mCenter.z);
                mask = 1;
            }
        }
        f32 height = f32(cylinderView->mHeight);
        t = (height - startY) / dy;
        dz = f32(direction->z);
        ax = f32(relativeStart->x);
        az = f32(relativeStart->z);
        dx = f32(direction->x);
        if (!(t < zero) && !(t > one)) {
            f32 z = gabi::fmadds(dz, t, az), x = gabi::fmadds(dx, t, ax);
            if (gabi::fmadds(x, x, z * z) < radiusSq) {
                (*candidates.get())[1].x = x + f32(cylinderView->mCenter.x);
                (*candidates.get())[1].y = f32(cylinderView->mCenter.y) + height;
                (*candidates.get())[1].z = z + f32(cylinderView->mCenter.z);
                mask |= 2;
            }
        }
    } else {
        az = f32(relativeStart->z);
        dz = f32(direction->z);
        dx = f32(direction->x);
        ax = f32(relativeStart->x);
    }
    f32 qa = gabi::fmadds(dx, dx, dz * dz), twiceA = qa + qa, qb0 = gabi::fmadds(dx, ax, dz * az),
        qb = qb0 + qb0, qc = gabi::fmadds(ax, ax, az * az) - radiusSq;
    f32 firstT;
    bool firstValid = true, secondValid = false;
    if (!(std::fabs(twiceA) < epsilon)) {
        f32 discriminant = gabi::fmsubs(qb, qb, (l_four * qa) * qc);
        if (discriminant < zero)
            return 0;
        secondValid = discriminant > zero;
        f32 root = cM3dSDK_sqrtfHD(discriminant);
        firstT = (root - qb) / twiceA;
        if (secondValid)
            secondT = (-qb - root) / twiceA;
    } else {
        if (std::fabs(qb) < epsilon)
            return 0;
        firstT = -(qc / qb);
    }
    if (!secondValid) {
        if (zero > firstT || firstT > one)
            return 0;
    } else {
        bool outsideFirst = zero > firstT || firstT > one,
             outsideSecond = zero > secondT || secondT > one;
        if (outsideFirst && outsideSecond)
            return 0;
        if (outsideFirst)
            firstValid = false;
        if (outsideSecond)
            secondValid = false;
    }
    if (firstValid) {
        f32 y = gabi::fmadds(firstT, f32(direction->y), f32(relativeStart->y));
        if (y < zero || y > f32(cylinderView->mHeight))
            firstValid = false;
    }
    if (secondValid) {
        f32 y = gabi::fmadds(secondT, f32(direction->y), f32(relativeStart->y));
        if (y < zero || y > f32(cylinderView->mHeight))
            secondValid = false;
    }
    if (!firstValid && !secondValid)
        return 0;
    gabi::Local<cXyz> base, scaled, relative;
    if (firstValid && secondValid) {
        cM3dSDK_PSVECAdd(relativeStart.get(), cylinder, base.get());
        mask |= 12;
        cM3dSDK_PSVECScale(direction.get(), scaled.get(), firstT);
        cM3dSDK_PSVECAdd(scaled.get(), base.get(), ptr(points + 24));
        cM3dSDK_PSVECScale(direction.get(), scaled.get(), secondT);
        cM3dSDK_PSVECAdd(scaled.get(), base.get(), ptr(points + 36));
    } else {
        mask |= 4;
        cM3dSDK_PSVECScale(direction.get(), scaled.get(), firstValid ? firstT : secondT);
        cM3dSDK_PSVECAdd(scaled.get(), relativeStart.get(), relative.get());
        cM3dSDK_PSVECAdd(relative.get(), cylinder, ptr(points + 24));
    }
    s32 count = 0;
    for (u32 i = 0; i < 4; i++)
        if (mask & (1u << i)) {
            u32 p = points + i * 12;
            if (count == 0) {
                u32 z = gabi::load<u32>(p + 8), x = gabi::load<u32>(p), y = gabi::load<u32>(p + 4),
                    o = gabi::ea(firstOut);
                gabi::store<u32>(o, x);
                gabi::store<u32>(o + 4, y);
                gabi::store<u32>(o + 8, z);
                count++;
            } else if (count == 1) {
                f32 previous = cM3dSDK_PSVECSquareDistance(line, firstOut),
                    next = cM3dSDK_PSVECSquareDistance(line, ptr(p));
                if (previous < next)
                    copyVectorWords(p, gabi::ea(secondOut));
                else {
                    copyVectorWords(gabi::ea(firstOut), gabi::ea(secondOut));
                    copyVectorWords(p, gabi::ea(firstOut));
                }
                break;
            } else
                count++;
        }
    return count;
}

/* 02015668 */
s32 cM3d_Cross_CylPntPnt(void* cylinder, void* start, void* end, void* count, void* out) {
    gabi::Local<cM3dGLin> line;
    line->__vtbl = 0x10002EE8;
    cM3dGLin_SetStartEndHD(line.get(), start, end);
    return cM3d_Cross_CylLin(cylinder, line.get(), count, out);
}

/* 02014F7C */
s32 cM3d_Cross_CylPnt(void* cylinder, void* point) {
    const cM3dGCyl* cylinderView = static_cast<const cM3dGCyl*>(cylinder);
    const cXyz* pointView = static_cast<const cXyz*>(point);

    f32 dz = f32(cylinderView->mCenter.z) - f32(pointView->z),
        dx = f32(cylinderView->mCenter.x) - f32(pointView->x), r = f32(cylinderView->mRadius),
        h = f32(cylinderView->mHeight), y = f32(cylinderView->mCenter.y);
    f32 sq = gabi::fmadds(dx, dx, dz * dz), top = y + h;
    if (!(sq < r * r))
        return 0;
    f32 py = f32(pointView->y);
    if (!(y < py))
        return 0;
    if (!(top > py))
        return 0;
    return 1;
}

/* 020159EC */
s32 cM3d_Cross_CpsCps(void* a, void* b, void* out) {
    const cM3dCapsuleView* aView = static_cast<const cM3dCapsuleView*>(a);
    const cM3dCapsuleView* bView = static_cast<const cM3dCapsuleView*>(b);
    gabi::Local<be<f32>> ta, tb;
    gabi::Local<cXyz> pa, pb;
    s32 relation = cM3d_Check_LinLin(a, b, ta.get(), tb.get());
    if (relation == 1)
        return 0;
    f32 half = l_half, zero = l_zero, one = l_one;
    u32 x = gabi::ea(a), y = gabi::ea(b);
    if (relation == 2) {
        f32 t = ta->get();
        if (!(t > zero) || !(t < one))
            return 0;
        f32 u = tb->get();
        if (!(u > zero) || !(u < one))
            return 0;
        cM3dGLin_CalcPosHD(a, pa.get(), t);
        cM3dGLin_CalcPosHD(b, pb.get(), tb->get());
    } else if (relation == 3) {
        f32 t = ta->get();
        if (t < zero)
            copyVectorWords(x, gabi::ea(pa.get()));
        else if (t > one)
            copyVectorWords(x + 12, gabi::ea(pa.get()));
        else
            cM3dGLin_CalcPosHD(a, pa.get(), t);
        f32 u = tb->get();
        if (u < zero)
            copyVectorWords(y, gabi::ea(pb.get()));
        else if (u > one)
            copyVectorWords(y + 12, gabi::ea(pb.get()));
        else
            cM3dGLin_CalcPosHD(b, pb.get(), u);
    } else
        return 0;
    f32 distance = cM3dSDK_PSVECDistance(pa.get(), pb.get());
    if (!(distance < f32(aView->mRadius) + f32(bView->mRadius)))
        return 0;
    cM3dSDK_PSVECAdd(pa.get(), pb.get(), out);
    cM3dSDK_PSVECScale(out, out, half);
    return 1;
}

/* 02015CF0 */
s32 cM3d_Cross_CpsCyl(void* capsule, void* cylinder, void* out) {
    const cM3dGCyl* cylinderView = static_cast<const cM3dGCyl*>(cylinder);
    const cM3dCapsuleView* capsuleView = static_cast<const cM3dCapsuleView*>(capsule);
    u32 c = gabi::ea(capsule);
    gabi::Local<cM3dGLin> line;
    line->__vtbl = 0x10002EE8;
    if (cM3d_Cross_CylPnt(cylinder, &static_cast<cM3dCapsuleView*>(capsule)->mEnd)) {
        copyVectorWords(c + 12, gabi::ea(out));
        return 1;
    }
    if (cM3d_Cross_CylPnt(cylinder, capsule)) {
        copyVectorWords(c, gabi::ea(out));
        return 1;
    }
    u32 l = gabi::ea(line.get());
    f32 base = f32(cylinderView->mCenter.y), height = f32(cylinderView->mHeight),
        x = f32(cylinderView->mCenter.x), z = f32(cylinderView->mCenter.z), top = base + height;
    line->mStart.y = base;
    line->mEnd.x = x;
    line->mEnd.z = z;
    line->mEnd.y = top;
    line->mStart.z = z;
    line->mStart.x = x;
    gabi::Local<cXyz> nearest, pa, pb;
    gabi::Local<be<f32>> distance, ta, tb;
    s32 onSegment = cM3d_Len3dSqPntAndSegLine(capsule, ptr(l + 12), nearest.get(), distance.get());
    f32 half = l_half;
    if (onSegment) {
        f32 d = cM3dSDK_PSVECDistance(ptr(l + 12), nearest.get());
        if (d < f32(capsuleView->mRadius)) {
            cM3dSDK_PSVECAdd(ptr(l + 12), nearest.get(), out);
            cM3dSDK_PSVECScale(out, out, half);
            copyVectorWords(l + 12, gabi::ea(out));
            return 1;
        }
    }
    if (cM3d_Len3dSqPntAndSegLine(capsule, ptr(l), nearest.get(), distance.get())) {
        f32 d = cM3dSDK_PSVECDistance(ptr(l), nearest.get());
        if (d < f32(capsuleView->mRadius)) {
            cM3dSDK_PSVECAdd(ptr(l), nearest.get(), out);
            cM3dSDK_PSVECScale(out, out, half);
            return 1;
        }
    }
    s32 relation = cM3d_Check_LinLin(capsule, line.get(), ta.get(), tb.get());
    if (relation == 1) {
        gabi::Local<cM3dGSph> sphere;
        gabi::Local<be<f32>> overlap;
        cM3dGSph_constructHD(sphere.get());
        cM3dGSph_SetRHD(sphere.get(), f32(capsuleView->mRadius));
        cM3dGSph_SetCHD(sphere.get(), capsule);
        return cM3d_CylinderSpherePosition(cylinder, sphere.get(), out, overlap.get());
    }
    f32 one = l_one, zero = l_zero;
    if (relation == 2) {
        f32 a = ta->get(), b = tb->get();
        if (a < zero || a > one || b < zero || b > one)
            return 0;
        cM3dGLin_CalcPosHD(capsule, pa.get(), a);
        cM3dGLin_CalcPosHD(line.get(), pb.get(), tb->get());
    } else if (relation == 3) {
        f32 a = ta->get();
        if (a < zero)
            copyVectorWords(c, gabi::ea(pa.get()));
        else if (a > one)
            copyVectorWords(c + 12, gabi::ea(pa.get()));
        else
            cM3dGLin_CalcPosHD(capsule, pa.get(), a);
        f32 b = tb->get();
        if (b < zero)
            copyVectorWords(l, gabi::ea(pb.get()));
        else if (b > one)
            copyVectorWords(l + 12, gabi::ea(pb.get()));
        else
            cM3dGLin_CalcPosHD(line.get(), pb.get(), b);
    } else
        return 0;
    f32 d = cM3dSDK_PSVECDistance(pa.get(), pb.get());
    if (!(d < f32(capsuleView->mRadius) + f32(cylinderView->mRadius)))
        return 0;
    cM3dSDK_PSVECAdd(pa.get(), pb.get(), out);
    cM3dSDK_PSVECScale(out, out, half);
    return 1;
}

/* 0201671C */
void cM3d_Cross_CpsSph_CrossPos(void* capsule, void* sphere, void* nearest, void* out) {
    const cM3dCapsuleView* capsuleView = static_cast<const cM3dCapsuleView*>(capsule);
    const cM3dGSph* sphereView = static_cast<const cM3dGSph*>(sphere);
    gabi::Local<cXyz> first, second, delta;
    s32 count = cM3d_Cross_LinSph_CrossPos(sphere, capsule, first.get(), second.get());
    if (count == 1 || count == 2) {
        u32 selected = gabi::ea(first.get());
        if (count == 2) {
            f32 a = cM3dSDK_PSVECSquareDistance(first.get(), capsule),
                b = cM3dSDK_PSVECSquareDistance(second.get(), capsule);
            if (!(a < b))
                selected = gabi::ea(second.get());
        }
        u32 y = gabi::load<u32>(selected + 4), x = gabi::load<u32>(selected),
            z = gabi::load<u32>(selected + 8), o = gabi::ea(out);
        gabi::store<u32>(o, x);
        gabi::store<u32>(o + 8, z);
        gabi::store<u32>(o + 4, y);
        return;
    }
    u32 n = gabi::ea(nearest), o = gabi::ea(out);
    u32 x = gabi::load<u32>(n);
    f32 sum = f32(capsuleView->mRadius) + f32(sphereView->mRadius);
    gabi::store<u32>(o, x);
    f32 epsilon = G_CM3D_F_ABS_MIN;
    gabi::store<u32>(o + 4, gabi::load<u32>(n + 4));
    gabi::store<u32>(o + 8, gabi::load<u32>(n + 8));
    if (std::fabs(sum) < epsilon)
        copyVectorWords(n, o);
    cM3dSDK_PSVECSubtract(nearest, sphere, delta.get());
    // HD: use fixed half scale instead of capsule radius divided by the radius sum.
    cM3dSDK_PSVECScale(delta.get(), delta.get(), l_half);
    cM3dSDK_PSVECAdd(out, delta.get(), out);
}

/* 020168D4 */
s32 cM3d_Cross_CpsSph(void* capsule, void* sphere, void* out) {
    const cM3dCapsuleView* capsuleView = static_cast<const cM3dCapsuleView*>(capsule);
    const cM3dGSph* sphereView = static_cast<const cM3dGSph*>(sphere);

    f32 d = cM3dSDK_PSVECDistance(capsule, sphere);
    if (d < f32(capsuleView->mRadius) + f32(sphereView->mRadius)) {
        cM3d_Cross_CpsSph_CrossPos(capsule, sphere, capsule, out);
        return 1;
    }
    d = cM3dSDK_PSVECDistance(&static_cast<cM3dCapsuleView*>(capsule)->mEnd, sphere);
    if (d < f32(capsuleView->mRadius) + f32(sphereView->mRadius)) {
        cM3d_Cross_CpsSph_CrossPos(capsule, sphere, &static_cast<cM3dCapsuleView*>(capsule)->mEnd,
                                   out);
        return 1;
    }
    gabi::Local<be<f32>> distance;
    gabi::Local<cXyz> nearest;
    *distance = d;
    if (!cM3d_Len3dSqPntAndSegLine(capsule, sphere, nearest.get(), distance.get()))
        return 0;
    d = cM3dSDK_sqrtfHD(distance->get());
    if (!(d < f32(capsuleView->mRadius) + f32(sphereView->mRadius)))
        return 0;
    cM3d_Cross_CpsSph_CrossPos(capsule, sphere, nearest.get(), out);
    return 1;
}

/* 02016A20 */
s32 cM3d_Cross_TriTri(void* a, void* b, void* out) {
    const cM3dGTri* aView = static_cast<const cM3dGTri*>(a);
    const cM3dGTri* bView = static_cast<const cM3dGTri*>(b);
    u32 x = gabi::ea(a), y = gabi::ea(b);
    f32 d1 = cM3dSDK_PSVECDotProduct(a, &static_cast<cM3dGTri*>(b)->mA);
    d1 += f32(aView->mD);
    f32 d2 = cM3dSDK_PSVECDotProduct(a, &static_cast<cM3dGTri*>(b)->mB);
    d2 += f32(aView->mD);
    f32 d3 = cM3dSDK_PSVECDotProduct(a, &static_cast<cM3dGTri*>(b)->mC), zero = l_zero;
    d3 += f32(aView->mD);
    if ((d1 > zero && d2 > zero && d3 > zero) || (d1 < zero && d2 < zero && d3 < zero))
        return 0;
    d1 = cM3dSDK_PSVECDotProduct(b, &static_cast<cM3dGTri*>(a)->mA);
    d1 += f32(bView->mD);
    d2 = cM3dSDK_PSVECDotProduct(b, &static_cast<cM3dGTri*>(a)->mB);
    d2 += f32(bView->mD);
    d3 = cM3dSDK_PSVECDotProduct(b, &static_cast<cM3dGTri*>(a)->mC);
    d3 += f32(bView->mD);
    if ((d1 > zero && d2 > zero && d3 > zero) || (d1 < zero && d2 < zero && d3 < zero))
        return 0;
    gabi::Local<cM3dGLin> line;
    line->__vtbl = 0x10002EE8;
    // HD repeats the first edge for the third test, in both directions.
    for (u32 side = 0; side < 2; side++) {
        u32 triangle = side ? x : y;
        void* other = side ? b : a;
        for (u32 edge = 0; edge < 3; edge++) {
            u32 start = triangle + (edge == 1 ? 32 : 20), end = triangle + (edge == 1 ? 44 : 32);
            cM3dGLin_SetStartEndHD(line.get(), ptr(start), ptr(end));
            if (cM3d_Cross_LinTri(line.get(), other, out, 0u, 0u))
                return 1;
        }
    }
    return 0;
}

/* 02016CE8 */
s32 cM3d_Cross_CpsTri(void* capsule, void* triangle, void* out) {
    const cM3dCapsuleView* capsuleView = static_cast<const cM3dCapsuleView*>(capsule);
    const cM3dGTri* triangleView = static_cast<const cM3dGTri*>(triangle);
    u32 t = gabi::ea(triangle);
    gabi::Local<cM3dGSph> sphere;
    cM3dGSph_constructHD(sphere.get());
    cM3dGSph_SetCHD(sphere.get(), capsule);
    cM3dGSph_SetRHD(sphere.get(), f32(capsuleView->mRadius));
    if (cM3d_Cross_SphTri(sphere.get(), triangle, out))
        return 1;
    cM3dGSph_SetCHD(sphere.get(), &static_cast<cM3dCapsuleView*>(capsule)->mEnd);
    cM3dGSph_SetRHD(sphere.get(), f32(capsuleView->mRadius));
    if (cM3d_Cross_SphTri(sphere.get(), triangle, out))
        return 1;
    if (!cM3d_Cross_LinPla(capsule, triangle, out, 1u, 1u))
        return 0;
    f32 epsilon = G_CM3D_F_ABS_MIN;
    bool projected = true;
    if (!(std::fabs(f32(triangleView->mNormal.x)) < epsilon) &&
        !cM3d_CrossX_LinTri_proc(triangle, out))
        projected = false;
    if (projected && !(std::fabs(f32(triangleView->mNormal.y)) < epsilon) &&
        !cM3d_CrossY_Tri(triangle, out))
        projected = false;
    if (projected && !(std::fabs(f32(triangleView->mNormal.z)) < epsilon) &&
        !cM3d_CrossZ_LinTri_proc(triangle, out))
        projected = false;
    if (projected)
        return 1;
    gabi::Local<cM3dGLin> line;
    line->__vtbl = 0x10002EE8;
    gabi::Local<be<f32>> capsuleT, edgeT;
    gabi::Local<cXyz> onCapsule, onEdge;
    f32 zero = 0, one = 0, half = 0;
    for (u32 edge = 0; edge < 3; edge++) {
        u32 start = t + (edge == 0   ? 20
                         : edge == 1 ? 32
                                     : 44),
            end = t + (edge == 0   ? 32
                       : edge == 1 ? 44
                                   : 20);
        cM3dGLin_SetStartEndHD(line.get(), ptr(start), ptr(end));
        s32 relation = cM3d_Check_LinLin(capsule, line.get(), capsuleT.get(), edgeT.get());
        if (edge == 0) {
            zero = l_zero;
            one = l_one;
            half = l_half;
        }
        if (relation < 2)
            continue;
        f32 a = capsuleT->get();
        if (!(a > zero) || !(a < one))
            continue;
        f32 b = edgeT->get();
        if (!(b > zero) || !(b < one))
            continue;
        cM3dGLin_CalcPosHD(capsule, onCapsule.get(), a);
        cM3dGLin_CalcPosHD(line.get(), onEdge.get(), b);
        cM3dSDK_PSVECAdd(onCapsule.get(), onEdge.get(), out);
        cM3dSDK_PSVECScale(out, out, half);
        f32 distance = cM3dSDK_PSVECDistance(onCapsule.get(), onEdge.get());
        if (distance < f32(capsuleView->mRadius))
            return 1;
    }
    return 0;
}

/* 020170F4 */
s32 cM3d_NearPos_Cps(void* capsule, void* point, void* out) {
    const cM3dCapsuleView* capsuleView = static_cast<const cM3dCapsuleView*>(capsule);
    gabi::Local<be<f32>> distance;
    gabi::Local<cXyz> nearest, delta;
    u32 c = gabi::ea(capsule);
    if (cM3d_Len3dSqPntAndSegLine(capsule, point, nearest.get(), distance.get())) {
        f32 d = cM3dSDK_sqrtfHD(distance->get());
        *distance = d;
        cM3dSDK_PSVECSubtract(point, nearest.get(), delta.get());
        f32 scale = f32(capsuleView->mRadius) / distance->get();
        cM3dSDK_PSVECScale(delta.get(), delta.get(), scale);
        cM3dSDK_PSVECAdd(nearest.get(), delta.get(), out);
    } else {
        f32 first = cM3dSDK_PSVECSquareDistance(capsule, point),
            last =
                cM3dSDK_PSVECSquareDistance(&static_cast<cM3dCapsuleView*>(capsule)->mEnd, point);
        u32 chosen = first < last ? c : c + 12;
        f32 d = cM3dSDK_sqrtfHD(first < last ? first : last);
        cM3dSDK_PSVECSubtract(ptr(chosen), point, delta.get());
        cM3dSDK_PSVECScale(delta.get(), delta.get(), f32(capsuleView->mRadius) / d);
        cM3dSDK_PSVECAdd(ptr(chosen), delta.get(), out);
    }
    return 1;
}

/* 02017264 */
void cM3d_CalcVecAngleHD(void* vector, void* xAngle, void* zAngle) {
    const cXyz* vectorView = static_cast<const cXyz*>(vector);

    f32 y = f32(vectorView->y), z = f32(vectorView->z), one = l_one;
    s32 x = cM_atan2sHD(-(z * y), one);
    gabi::store<u16>(gabi::ea(xAngle), u16(0u - u32(x)));
    f32 a = f32(vectorView->x), b = f32(vectorView->y);
    s32 angle = cM_atan2sHD(-(a * b), one);
    gabi::store<u16>(gabi::ea(zAngle), u16(angle));
}

/* 02017300 */
void cM3d_CalcVecZAngle(void* vector, void* angles) {
    const cXyz* vectorView = static_cast<const cXyz*>(vector);
    u32 a = gabi::ea(angles);
    f32 z = f32(vectorView->z), x = f32(vectorView->x);
    f32 horizontal = cM3dSDK_sqrtfHD(gabi::fmadds(x, x, z * z));
    s32 pitch = cM_atan2sHD(f32(vectorView->y), horizontal);
    gabi::store<u16>(a, u16(0u - u32(pitch)));
    s32 yaw = cM_atan2sHD(f32(vectorView->x), f32(vectorView->z));
    gabi::store<u16>(a + 2, u16(yaw));
    gabi::store<u16>(a + 4, 0);
}

/* 02017374 */
s32 cM3d_UpMtx_Base(void* up, void* vector, void* matrix) {
    f32 length = cM3dSDK_PSVECMag(vector), epsilon = G_CM3D_F_ABS_MIN;
    if (std::fabs(length) < epsilon) {
        cM3dSDK_PSMTXIdentity(matrix);
        return 0;
    }
    gabi::Local<cXyz> normalized, axis;
    cM3dSDK_PSVECNormalize(vector, normalized.get());
    cM3dSDK_PSVECCrossProduct(up, normalized.get(), axis.get());
    f32 axisLength = cM3dSDK_PSVECMag(axis.get()), one = l_one;
    if (std::fabs(axisLength) < epsilon) {
        f32 zero = l_zero;
        axis->x = one;
        axis->y = zero;
        axis->z = zero;
    }
    f32 dot = cM3dSDK_PSVECDotProduct(up, normalized.get());
    if (dot > one)
        dot = one;
    else {
        f32 negative = l_negativeOne;
        if (dot < negative)
            dot = negative;
    }
    f64 angle = cM3dSDK_acosHD(f64(dot));
    cM3dSDK_PSMTXRotAxisRad(matrix, axis.get(), angle);
    return 1;
}

/* 020174C0 */
void cM3d_PlaneCrossLineProcWork(void* first, void* second, f32 a, f32 b, f32 c, f32 d,
                                 f32 denominator, f32 f, f32 g) {
    f32 x = gabi::fmsubs(b, g, d * f) / denominator, y = gabi::fmsubs(c, f, a * g) / denominator;
    *static_cast<be<f32>*>(first) = x;
    *static_cast<be<f32>*>(second) = y;
}

/* 020174E4 */
s32 cM3d_2PlaneCrossLine(void* a, void* b, void* line) {
    const cM3dGPla* aView = static_cast<const cM3dGPla*>(a);
    const cM3dGPla* bView = static_cast<const cM3dGPla*>(b);
    gabi::Local<cXyz> direction;
    cM3dSDK_PSVECCrossProduct(a, b, direction.get());
    u32 o = gabi::ea(line);
    f32 dx = f32(direction->x), dy = f32(direction->y), dz = f32(direction->z), ax = std::fabs(dx),
        ay = std::fabs(dy), az = std::fabs(dz), epsilon = G_CM3D_F_ABS_MIN;
    if (ax < epsilon && ay < epsilon && az < epsilon)
        return 0;
    if (!(ax < ay) && !(ax < az)) {
        cM3d_PlaneCrossLineProcWork(ptr(o + 4), ptr(o + 8), f32(aView->mNormal.y),
                                    f32(aView->mNormal.z), f32(bView->mNormal.y),
                                    f32(bView->mNormal.z), dx, f32(aView->mD), f32(bView->mD));
        static_cast<cM3dGLin*>(line)->mStart.x = l_zero;
    } else if (!(ay < ax) && !(ay < az)) {
        cM3d_PlaneCrossLineProcWork(ptr(o + 8), ptr(o), f32(aView->mNormal.z),
                                    f32(aView->mNormal.x), f32(bView->mNormal.z),
                                    f32(bView->mNormal.x), dy, f32(aView->mD), f32(bView->mD));
        static_cast<cM3dGLin*>(line)->mStart.y = l_zero;
    } else {
        cM3d_PlaneCrossLineProcWork(ptr(o), ptr(o + 4), f32(aView->mNormal.x),
                                    f32(aView->mNormal.y), f32(bView->mNormal.x),
                                    f32(bView->mNormal.y), dz, f32(aView->mD), f32(bView->mD));
        static_cast<cM3dGLin*>(line)->mStart.z = l_zero;
    }
    f32 scale = cM3dSDK_PSVECMag(line);
    // HD: a zero-length start uses unit scale.
    if (std::fabs(scale) < epsilon)
        scale = l_one;
    cM3dSDK_PSVECScale(direction.get(), direction.get(), scale);
    cM3dSDK_PSVECAdd(line, direction.get(), ptr(o + 12));
    return 1;
}

/* 020176D4 */
s32 cM3d_3PlaneCrossPos(void* a, void* b, void* c, void* out) {
    const cM3dGPla* cView = static_cast<const cM3dGPla*>(c);
    gabi::Local<cM3dGLin> line;
    line->__vtbl = 0x10002EE8;
    if (!cM3d_2PlaneCrossLine(a, b, line.get()))
        return 0;
    u32 l = gabi::ea(line.get());
    f32 start = cM3dSDK_PSVECDotProduct(c, ptr(l));
    start += f32(cView->mD);
    f32 end = cM3dSDK_PSVECDotProduct(c, ptr(l + 12));
    end += f32(cView->mD);
    return cM3d_PlaneSegmentPosition(ptr(l), ptr(l + 12), out, start, end) != 0;
}

/* 020177AC */
f32 cM3d_lineVsPosSuisenCross(void* line, void* point, void* out) {
    return linePointProjection(line, ptr(gabi::ea(line) + 12), point, out, "c_m3d.cpp", 0x1098,
                               "!__generic(cross->x, , ,__isnand,__isnanf,__isnanl,,,)(cross->x)",
                               "!__generic(cross->y, , ,__isnand,__isnanf,__isnanl,,,)(cross->y)",
                               "!__generic(cross->z, , ,__isnand,__isnanf,__isnanl,,,)(cross->z)");
}

/* 0201794C */
f32 cM3d_lineVsPosSuisenCrossVertices(void* start, void* end, void* point, void* out) {
    return linePointProjection(
        start, end, point, out, "c_m3d.cpp", 0x10CB,
        "!__generic(pcross->x, , ,__isnand,__isnanf,__isnanl,,,)(pcross->x)",
        "!__generic(pcross->y, , ,__isnand,__isnanf,__isnanl,,,)(pcross->y)",
        "!__generic(pcross->z, , ,__isnand,__isnanf,__isnanl,,,)(pcross->z)");
}

/* 02017AEC */
s32 cM3d_2PlaneLinePosNearPos(void* a, void* b, void* point, void* out) {
    gabi::Local<cM3dGLin> line;
    line->__vtbl = 0x10002EE8;
    if (!cM3d_2PlaneCrossLine(a, b, line.get()))
        return 0;
    cM3d_lineVsPosSuisenCross(line.get(), point, out);
    return 1;
}

/* 02017B6C */
void cM3d_CrawVec(void* normal, void* vector, void* out) {
    const cXyz* normalView = static_cast<const cXyz*>(normal);
    const cXyz* vectorView = static_cast<const cXyz*>(vector);

    f32 ny = f32(normalView->y), vy = f32(vectorView->y), nx = f32(normalView->x),
        vx = f32(vectorView->x), vz = f32(vectorView->z), nz = f32(normalView->z);
    f32 dot = gabi::fmadds(vz, nz, gabi::fmadds(vx, nx, vy * ny));
    gabi::Local<cXyz> scaled;
    cM3dSDK_PSVECScale(normal, scaled.get(), std::fabs(dot));
    cM3dSDK_PSVECAdd(scaled.get(), vector, out);
}

// HD: compiler-generated static initialization.

/* 02017BE0 */
void c_m3d_static_init() {
    l_geometryHeaderWords[2] = 0;
    l_geometryHeaderWords[0] = 0;
    l_geometryHeaderWords[3] = 0;
    l_geometryHeaderWords[1] = 0;
    registerGeometryDestructor(ptr(0x1018C88C));
    f32 negativePi = l_negativePi, positivePi = l_positivePi;
    l_geometryNegativePi = negativePi;
    l_geometryPositivePi = positivePi;
    initializeGeometryByte(ptr(0x101FF88C));
    registerGeometryDestructor(ptr(0x1018C898));
    initializeGeometryFlag(ptr(0x101FF88D));
    registerGeometryDestructor(ptr(0x1018C8A4));
}

// HD: a neighboring header-only translation unit initializes its own copies here.
/* 02017C74 */
void initializeGeometryHeaderStatics() {
    l_neighborHeaderWords[2] = 0;
    l_neighborHeaderWords[0] = 0;
    l_neighborHeaderWords[3] = 0;
    l_neighborHeaderWords[1] = 0;
    registerGeometryDestructor(gabi::at<void>(0x1018C8B0));
    f32 negativePi = l_neighborPiMin, positivePi = l_neighborPiMax;
    l_neighborNegativePi = negativePi;
    l_neighborPositivePi = positivePi;
    initializeGeometryByte(gabi::at<void>(0x101FF8A8));
    registerGeometryDestructor(gabi::at<void>(0x1018C8BC));
    initializeGeometryFlag(gabi::at<void>(0x101FF8A9));
    registerGeometryDestructor(gabi::at<void>(0x1018C8C8));
}

/* WWHD glue
str 10001FB4 in 020135C8
str 10001FC0 in 020135C8
str 10002018 in 020135C8
str 10002070 in 020135C8
str 100020C8 in 020135C8
str 10002114 in 020135C8
str 1000216C in 020135C8
str 100021C4 in 020135C8
str 1000221C in 020135C8
str 10002268 in 020135C8
str 100022B4 in 020135C8
str 10002644 in 02013B78
str 10002650 in 02013B78
str 100026A0 in 02013B78
str 100026F0 in 02013B78
str 10002740 in 02013B78
str 10002788 in 02013B78
str 100027D8 in 02013B78
str 10002828 in 02013B78
str 10002878 in 02013B78
str 100028C0 in 02013B78
str 10002910 in 02013DC0
str 1000291C in 02013DC0
str 1000296C in 02013DC0
str 100029BC in 02013DC0
str 10002A0C in 02013DC0
str 10002A54 in 02013DC0
str 10002AA4 in 02013DC0
str 10002AF4 in 02013DC0
str 10002B44 in 02013DC0
str 10002B8C in 02013DC0
str 100022FC in 02013854
str 10002308 in 02013854
str 10002360 in 02013854
str 100023B8 in 02013854
str 10002410 in 02013854
str 1000245C in 02013854
str 100024B4 in 02013854
str 1000250C in 02013854
str 10002564 in 02013854
str 100025B0 in 02013854
str 100025FC in 02013854
str 10002BE4 in 02014AF8
str 10002BF0 in 02014AF8
str 10002C2C in 02014AF8
str 10002C68 in 02014AF8
str 10002D58 in 02014AF8
str 10002DA8 in 02014AF8
str 10002CA4 in 02014AF8
str 10002CE0 in 02014AF8
str 10002D1C in 02014AF8
str 10002DF8 in 02014AF8
str 10002E48 in 02014AF8
str 10002E98 in 02014AF8
str 10002EFC in 020177AC
str 10002F08 in 020177AC
str 10002F4C in 020177AC
str 10002F90 in 020177AC
str 10002FD4 in 0201794C
str 10002FE0 in 0201794C
str 10003024 in 0201794C
str 10003068 in 0201794C
data l_geometryHeaderWords 101FF890 nocheck
data l_neighborHeaderWords 101FF8AC nocheck
data l_geometryNegativePi 101FF884 nocheck
data l_geometryPositivePi 101FF888 nocheck
data l_neighborNegativePi 101FF8A0 nocheck
data l_neighborPositivePi 101FF8A4 nocheck
data l_neighborPiMin 100030C8
data l_neighborPiMax 100030CC
ext 028EAB2C void initializeGeometryFlag(void* value)
ext 028ED6F8 void initializeGeometryByte(void* value)
ext 028F026C void registerGeometryDestructor(void* descriptor)
ext 0273AA24 void cM3d_assert(const char* file, u32 line, const char* condition)
ext 020195B0 s32 cM_atan2sHD(f32 y, f32 x)
ext 020189A0 void* cM3dGPla_constructHD(void* plane)
ext 02018C8C void cM3dGSph_SetRHD(void* sphere, f32 radius)
ext 02018D40 void cM3dGSph_SetCHD(void* sphere, const void* center)
ext 02018C40 void* cM3dGSph_constructHD(void* sphere)
ext 02018780 void* cM3dGLin_constructHD(void* line, const void* start, const void* end)
ext 0201888C void cM3dGLin_CalcPosHD(void* line, void* out, f32 scale)
ext 0201883C void cM3dGLin_SetStartEndHD(void* line, const void* start, const void* end)
data l_zero 10001F88
data G_CM3D_F_ABS_MIN 10001F8C
data l_one 10001F90
data l_planeNormalMin 10001F94
data l_negativeOne 10001F98
data l_triangleMargin 10001F9C
data l_triangleUpper 10001FA0
data l_triangleLower 10001FA4
data l_triangleTolerance 10001FA8
data l_four 10001FB0
data l_half 10002BDC
data l_negativeZero 10002BE0
data l_farDistance 10002EF8
data l_negativePi 100030B0
data l_positivePi 100030B4
ext 028E8E64 void cM3dSDK_PSVECScale(const void* in, void* out, f32 scale)
ext 028E8D88 void cM3dSDK_PSVECAdd(const void* a, const void* b, void* out)
ext 028E8DAC void cM3dSDK_PSVECSubtract(const void* a, const void* b, void* out)
ext 028E8D4C void cM3dSDK_PSVECCrossProduct(const void* a, const void* b, void* out)
ext 028E8F44 f32 cM3dSDK_PSVECDotProduct(const void* a, const void* b)
ext 028E8E10 f32 cM3dSDK_PSVECMag(const void* v)
ext 028E8DD0 f32 cM3dSDK_PSVECSquareMag(const void* v)
ext 028E8DE8 f32 cM3dSDK_PSVECSquareDistance(const void* a, const void* b)
ext 028E8E80 f32 cM3dSDK_PSVECDistance(const void* a, const void* b)
ext 028E8EF0 void cM3dSDK_PSVECNormalize(const void* in, void* out)
ext 028E9098 void cM3dSDK_PSMTXIdentity(void* matrix)
ext 028E954C void cM3dSDK_PSMTXRotAxisRad(void* matrix, const void* axis, f64 angle)
ext 028F4384 f32 cM3dSDK_sqrtfHD(f32 value)
ext 028F4FAC f64 cM3dSDK_acosHD(f64 value)
*/
