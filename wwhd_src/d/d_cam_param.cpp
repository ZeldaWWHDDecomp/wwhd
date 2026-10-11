/** Camera style parameters and math, using zeldaret/tww names and the verified HD arithmetic. */
#include "d/d_cam_param.h"

static f32 l_stickUpper;
static f32 l_stickLower;
static f32 l_farCheckAngle;
static f32 l_nearWeightL;
static f32 l_fwdBackMargin;
static f32 l_farWeightL;
static f32 l_wallCushion;
static f32 l_one;
static f32 l_cornerCushion;
static f32 l_floorMargin;
static f32 l_fwdCushion;
static f32 l_nearCheckAngle;
static f32 l_nearWeightH;
static f32 l_farCheckDistance;
static f32 l_farWeightH;
static f32 l_wallUpDistance;
static f32 l_wallBackCushion;
static f32 l_cornerAngleMax;
static f32 l_bgRange;
static f32 l_bgSecondRange;
static f32 l_bgEndDistance;
static f32 l_curveCutoff;
static f32 l_negativeOne;
static f32 l_curveScale;
static f32 l_zero;
static f32 l_radiusEpsilon;
static f32 l_degreeToSigned;
static f32 l_setupLatitudeLower;
static f32 l_setupM028;
static f32 l_setupM02C;
static f32 l_setupM054;
static f32 l_setupM05C;
static f32 l_setupSubjLinkCullDist;
static f32 l_setupM058;
static f32 l_drawFar;
static f32 l_setupJumpCushion;
static f32 l_setupM044;
static f32 l_setupM030;
static f32 l_setupChargeBRatio;
static f32 l_setupM0B4;
static f32 l_setupDMCAngle;
static f32 l_setupM0B8;
static f32 l_setupExtraParameter;
static f32 l_setupHDVector;
static f32 l_setupM084;
static f32 l_setupM078;
static f32 l_setupHDDefault;
static f64 l_doubleOne;
static f64 l_doubleNegativeOne;
static f64 l_doubleFour;
static f64 l_doubleZero;
static f64 l_positiveDenominatorEpsilon;
static f64 l_negativeDenominatorEpsilon;
static f64 l_doubleRatioEpsilon;
static f32 l_halfTurnDegrees;
static f32 l_initialNegativePi;
static f32 l_initialPi;

static CamParamHeaderStatics l_headerStatics;
static CamParamDestructorRecords l_destructors;
static CamParamStyles l_styles;
using CamParamCount = be<s32>;
static CamParamCount l_styleCount;
const dCamera__Style (&dCamParam_c::styles)[145] = l_styles.mEntries;
const be<s32>& dCamParam_c::style_num = l_styleCount;

// HD: constructors and deleting destructors retain explicit allocation/flag arguments.
// HD: vector results use word copies so NaN payloads remain bit-identical.
static void CamParam_copyVectorWords(cXyz* out, const cXyz* value) {
    auto* source = reinterpret_cast<const be<u32>*>(value);
    auto* destination = reinterpret_cast<be<u32>*>(out);
    destination[0] = source[0];
    destination[1] = source[1];
    destination[2] = source[2];
}

// HD: preserve f64 intermediates and the original fused arithmetic and compare direction.
/* 024F6F9C */
f32 dCamMath::rationalBezierRatio(f32 input, f32 weight) {

    f64 x = input, w = weight, one = l_doubleOne, sign = one;
    f32 zero = l_zero;
    if (input < zero) {
        x = -x;
        sign = l_doubleNegativeOne;
    }
    f64 twice = x + x, twicew = w + w, b = gabi::fmsub(twice, w, twice) - twicew, neg = -b,
        a = neg - one;
    f64 four = l_doubleFour, disc = gabi::fmsub(b, b, (four * a) * x), sq = l_doubleZero;
    if (disc > sq) {
        sq = std_sqrtf(f32(disc));
    }
    f64 denom = a + a, hi = l_positiveDenominatorEpsilon, numerator = neg - sq;
    if (!(denom > hi)) {
        f64 low = l_negativeDenominatorEpsilon;
        if (!(denom < low)) {
            return zero;
        }
    }
    f64 t = numerator / denom, remaining = one - t, twicerem = remaining + remaining,
        cross = (twicerem * t) * w, tsq = t * t, rsq = gabi::fmadd(remaining, remaining, cross),
        sum = rsq + tsq, eps = l_doubleRatioEpsilon;
    if (sum > eps) {
        return f32((tsq / sum) * sign);
    }
    return zero;
}

/* 024F7194 */
f32 dCamMath::customRBRatio(f32 x, f32 y) {

    f32 cutoff = l_curveCutoff;
    if (x > cutoff) {
        f32 neg = l_negativeOne, pos = l_one;
        return x >= 0 ? pos : neg;
    }
    return dCamMath::rationalBezierRatio(gabi::fmuls_ppc(x, l_curveScale), y);
}

/* 024F7948 */
f32 dCamMath::zoomFovy(f32 degrees, f32 scale) {
    gabi::Local<cDegree> degree;
    CamParam_degreeConstruct(degree.get(), degrees);
    f32 cosine = CamParam_degreeCos(degree.get());
    f32 product = gabi::fmuls_ppc(scale, cosine);
    f32 sine = CamParam_degreeSin(degree.get());
    f32 angle = CamParam_atan2(sine, product);
    f32 half = l_halfTurnDegrees, pi = l_headerStatics.mPi;
    return gabi::fmuls_ppc(angle, half / pi);
}

/* 024F79BC */
cXyz* dCamMath::xyzRotateX(cXyz* out, cXyz* vector, cSAngle* angle) {
    gabi::Local<Mtx34> matrix;
    gabi::Local<cXyz> result;
    s32 value = angle->mAngle;
    CamParam_XrotS(matrix.get(), value);
    PSMTXMultVec(matrix.get(), vector, result.get());
    if (out == nullptr) {
        out = gabi::at<cXyz>(CamParam_allocate(sizeof(cXyz)));
    }
    if (out != nullptr) {
        CamParam_copyVectorWords(out, result.get());
    }
    return out;
}

/* 024F7A40 */
cXyz* dCamMath::xyzRotateY(cXyz* out, cXyz* vector, cSAngle* angle) {
    gabi::Local<Mtx34> matrix;
    gabi::Local<cXyz> result;
    s32 value = angle->mAngle;
    CamParam_YrotS(matrix.get(), value);
    PSMTXMultVec(matrix.get(), vector, result.get());
    if (out == nullptr) {
        out = gabi::at<cXyz>(CamParam_allocate(sizeof(cXyz)));
    }
    if (out != nullptr) {
        CamParam_copyVectorWords(out, result.get());
    }
    return out;
}

/* 024F7AC4 */
f32 dCamMath::xyzHorizontalDistance(cXyz* a, cXyz* b) {
    f32 bz = b->z, az = a->z, z = gabi::fsubs_ppc(az, bz);
    f32 bx = b->x, ax = a->x, dx = gabi::fsubs_ppc(ax, bx);
    f64 zz = f64(z) * z, sum = gabi::fmadd(dx, dx, zz);
    return std_sqrtf(f32(sum));
}

/* 024F7AEC */
cXyz* dCamMath::xyzProjPosOnYZ(cXyz* out, cSAngle* angle, cXyz* a, cXyz* b) {
    gabi::Local<cXyz> line, rot, back;
    gabi::Local<cSAngle> ang;
    CamParam_subtractVectors(b, line.get(), a);
    CamParam_negateAngle(angle, ang.get());
    (void)xyzRotateY(rot.get(), line.get(), ang.get());
    f32 zero = l_zero;
    rot->x = zero;
    CamParam_copyAngle(ang.get(), angle);
    // HD: the copy helper's unextended r3 is the next angle address.
    u32 ap = gabi::cpu->r[3];
    (void)xyzRotateY(back.get(), rot.get(), gabi::at<cSAngle>(ap));
    auto* source = reinterpret_cast<be<u32>*>(back.get());
    auto* destination = reinterpret_cast<be<u32>*>(line.get());
    u32 yy = source[1], zz = source[2];
    destination[1] = yy;
    destination[2] = zz;
    destination[0] = source[0];
    return CamParam_addVectors(a, out, line.get());
}

// HD: allocating constructors and deleting destructors keep their explicit ABI parameters.
/* 024F6DEC */
dCstick_c* dCstick_c__ct(dCstick_c* self) {

    u32 a = gabi::ea(self);
    if (!a) {
        a = CamParam_allocate(0x14);
    }
    if (a) {
        self = gabi::at<dCstick_c>(a);
        f32 upper = l_stickUpper, lower = l_stickLower;
        self->__vtbl = CAM_PARAM_STICK_VTABLE;
        self->m08 = 6;
        self->mHDFlags = 0;
        self->m04 = upper;
        self->m00 = lower;
    }
    return gabi::at<dCstick_c>(a);
}

/* 024F6E54 */
s32 dCstick_c::Shift(u32 flags) { return 0; }

/* 024F6E5C */
dCamBGChk_c* dCamBGChk_c__ct(dCamBGChk_c* self) {

    u32 a = gabi::ea(self);
    if (!a) {
        a = CamParam_allocate(0x64);
    }
    if (a) {
        self = gabi::at<dCamBGChk_c>(a);
        f32 v1 = l_farCheckAngle;
        f32 v2 = l_nearWeightL;
        f32 v3 = l_fwdBackMargin;
        self->mChkInfo[0].mWeightL = v2;
        f32 v4 = l_farWeightL;
        self->mChkInfo[1].mChkAngle = v1;
        f32 v5 = l_wallCushion;
        self->mGazeBackMargin = v3;
        f32 v6 = l_stickLower;
        self->mChkInfo[1].mWeightL = v4;
        f32 v7 = l_one;
        self->mFwdBackMargin = v3;
        f32 v8 = l_cornerCushion;
        self->m2C = v6;
        f32 v9 = l_floorMargin;
        self->mCornerCushion = v8;
        f32 v10 = l_fwdCushion;
        self->mFloorMargin = v9;
        f32 v11 = l_nearCheckAngle;
        self->mFwdCushion = v10;
        f32 v12 = l_nearWeightH;
        self->mChkInfo[0].mChkAngle = v11;
        self->mChkInfo[0].mWeightH = v12;
        f32 v13 = l_farCheckDistance;
        self->mChkInfo[0].mDistance = v7;
        f32 v14 = l_farWeightH;
        self->mChkInfo[1].mDistance = v13;
        f32 v15 = l_wallUpDistance;
        self->mChkInfo[1].mWeightH = v14;
        f32 v16 = l_wallBackCushion;
        f32 v17 = l_cornerAngleMax;
        self->m4C = v15;
        self->mCornerAngleMax = v17;
        f32 v18 = l_bgRange;
        self->mWallBackCushion = v16;
        f32 v19 = l_bgSecondRange;
        self->mWallUpDistance = v15;
        self->m5C = v19;
        f32 v20 = l_bgEndDistance;
        self->m50 = v5;
        self->m58 = v20;
        self->__vtbl = CAM_PARAM_BG_VTABLE;
        self->m48 = v18;
        self->m54 = v5;
        self->mWallCushion = v5;
    }
    return gabi::at<dCamBGChk_c>(a);
}

/* 024F72BC */
dCamParam_c* dCamParam_c__ct(dCamParam_c* self, s32 index) {

    u32 a = gabi::ea(self);
    if (!a) {
        a = CamParam_allocate(0xC);
    }
    if (a) {
        self = gabi::at<dCamParam_c>(a);
        self->__vtbl = CAM_PARAM_STYLE_VTABLE;
        (void)self->Change(index);
    }
    return gabi::at<dCamParam_c>(a);
}

/* 024F7318 */
void dCamParam_c__dt(dCamParam_c* self, s32 flag) {

    if (self && (u32(flag) & CAM_PARAM_DELETE_STORAGE)) {
        CamParam_deallocate(self);
    }
}

/* 024F727C */
BOOL dCamParam_c::Change(s32 index) {
    mStyleIdx = index;
    s32 count = style_num;
    if (index < count) {
        mpStyle = &styles[index];
        return TRUE;
    }
    mpStyle = &styles[dCamStyle_NN00_e];
    return FALSE;
}

/* 024F732C */
s32 dCamParam_c::SearchStyle(u32 name) {
    s32 count = style_num;
    if (count <= 0) {
        return -1;
    }
    for (s32 i = 0; i < count; ++i) {
        if ((u32)styles[i].m00 == name) {
            return i;
        }
    }
    return -1;
}

/* 024F71CC */
f32 dCamParam_c::ratiof(f32 t, f32 upper, f32 lower, f32 base) {

    f32 zero = l_zero;
    if (t == zero) {
        return base;
    }
    f32 quarter = l_wallCushion;
    f32 span = t < zero ? gabi::fsubs_ppc(base, lower) : gabi::fsubs_ppc(upper, base);
    f32 curve = dCamMath::customRBRatio(t, quarter);
    return gabi::fmadds(span, curve, base);
}

/* 024F7400 */
BOOL dCamParam_c::DefaultRadius(f32* radius) {
    const dCamera__Style* style = mpStyle.get();
    f32 lo = style->styleParam[dCamStyleParam_UNK13];
    f32 hi = style->styleParam[dCamStyleParam_UNK14];
    if (!(lo < hi)) {
        f32 copy = lo;
        lo = hi;
        hi = copy;
    }
    auto* value = reinterpret_cast<be<f32>*>(radius);
    f32 val = *value;
    if (val > hi) {
        *value = hi;
        return FALSE;
    }
    if (val < lo) {
        *value = lo;
        return FALSE;
    }
    return TRUE;
}

/* 024F7370 */
f32 dCamParam_c::RadiusRatio(f32 radius) {

    f32 eps = l_radiusEpsilon;
    const dCamera__Style* style = mpStyle.get();
    f32 center = (f32)style->styleParam[dCamStyleParam_UNK10];
    if (radius < center) {
        f32 low = (f32)style->styleParam[dCamStyleParam_UNK11],
            dist = gabi::fsubs_ppc(center, radius), span = gabi::fsubs_ppc(center, low);
        if (!(dist < span)) {
            return l_negativeOne;
        }
        if (span > eps) {
            return -(dist / span);
        }
    } else if (radius > center) {
        f32 high = (f32)style->styleParam[dCamStyleParam_UNK12],
            dist = gabi::fsubs_ppc(radius, center), span = gabi::fsubs_ppc(high, center);
        if (!(dist < span)) {
            return l_one;
        }
        if (span > eps) {
            return dist / span;
        }
    }
    return l_zero;
}

/* 024F7464 */
f32 dCamParam_c::CenterHeight(f32 t) {

    const dCamera__Style* style = mpStyle.get();
    f32 upper = (f32)style->styleParam[dCamStyleParam_CENTER_HEIGHT_UPPER],
        base = (f32)style->styleParam[dCamStyleParam_CENTER_HEIGHT_BASE],
        lower = (f32)style->styleParam[dCamStyleParam_CENTER_HEIGHT_LOWER];
    return ratiof(t, upper, lower, base);
}

/* 024F7478 */
f32 dCamParam_c::Fovy(f32 t) {

    const dCamera__Style* style = mpStyle.get();
    f32 upper = (f32)style->styleParam[dCamStyleParam_FOVY_UPPER],
        base = (f32)style->styleParam[dCamStyleParam_FOVY_BASE],
        lower = (f32)style->styleParam[dCamStyleParam_FOVY_LOWER];
    return ratiof(t, upper, lower, base);
}

// HD: preserve the full sign-extended angle result register.
/* 024F748C */
s32 dCamParam_c::LockonLongitude(f32 t) {
    gabi::Local<CamParamAngles> local;
    const dCamera__Style* style = mpStyle.get();
    f32 low = style->styleParam[dCamStyleParam_LOCKON_LONGITUDE_MIN];
    CamParam_angleDegree(&local->base, low);
    style = mpStyle.get();
    f32 high = style->styleParam[dCamStyleParam_LOCKON_LONGITUDE_MAX];
    CamParam_angleDegree(&local->maximum, high);
    CamParam_subtractAngles(&local->maximum, &local->difference, &local->base);
    CamParam_multiplyAngle(&local->difference, &local->multiplied, t);
    CamParam_addAngle(&local->base, &local->multiplied);
    return local->base.mAngle;
}

/* 024F7520 */
s32 dCamParam_c::LockonLatitude(f32 t) {
    gabi::Local<CamParamAngles> local;
    const dCamera__Style* style = mpStyle.get();
    f32 low = style->styleParam[dCamStyleParam_LOCKON_LATITUDE_MIN];
    CamParam_angleDegree(&local->base, low);
    style = mpStyle.get();
    f32 high = style->styleParam[dCamStyleParam_LOCKON_LATITUDE_MAX];
    CamParam_angleDegree(&local->maximum, high);
    CamParam_subtractAngles(&local->maximum, &local->difference, &local->base);
    CamParam_multiplyAngle(&local->difference, &local->multiplied, t);
    CamParam_addAngle(&local->base, &local->multiplied);
    return local->base.mAngle;
}

/* 024F75B4 */
f32 dCamParam_c::LockonFovy(f32 t) {

    const dCamera__Style* style = mpStyle.get();
    f32 low = (f32)style->styleParam[dCamStyleParam_LOCKON_FOVY_MIN],
        high = (f32)style->styleParam[dCamStyleParam_LOCKON_FOVY_MAX];
    return gabi::fmadds(gabi::fsubs_ppc(high, low), t, low);
}

/* 024F75CC */
f32 dCamParam_c::LockonCenterHeight(f32 t) {

    const dCamera__Style* style = mpStyle.get();
    f32 low = (f32)style->styleParam[dCamStyleParam_LOCKON_CENTER_HEIGHT_MIN],
        high = (f32)style->styleParam[dCamStyleParam_LOCKON_CENTER_HEIGHT_MAX];
    return gabi::fmadds(gabi::fsubs_ppc(high, low), t, low);
}

/* 024F75E4 */
dCamSetup_c* dCamSetup_c__ct(dCamSetup_c* self) {

    u32 a = gabi::ea(self);
    if (!a) {
        a = CamParam_allocate(0x168);
    }
    if (a) {
        self = gabi::at<dCamSetup_c>(a);
        self->__vtbl = CAM_PARAM_SETUP_VTABLE;
        if (gabi::ea(&self->mHDVector) == 0) {
            (void)CamParam_allocate(sizeof(cXyz));
        }
        (void)dCstick_c__ct(&self->mCstick);
        (void)dCamBGChk_c__ct(&self->mBGChk);
        f32 q1 = l_setupLatitudeLower;
        f32 q2 = l_setupM028;
        self->m05C = q1;
        f32 q3 = l_one;
        f32 q4 = l_setupM02C;
        self->mDrawNear = q3;
        self->m028 = q4;
        f32 q5 = l_setupM054;
        f32 q6 = l_setupM05C;
        f32 q7 = l_setupSubjLinkCullDist;
        self->mForceType = 0xffffffff;
        f32 q8 = l_setupM058;
        f32 q9 = l_cornerCushion;
        f32 q10 = l_bgSecondRange;
        self->m024 = q2;
        f32 q11 = l_drawFar;
        self->mSubjLinkCullDist = q10;
        self->mJumpCushion = q3;
        f32 q12 = l_nearCheckAngle;
        f32 q13 = l_wallUpDistance;
        self->mDrawFar = q11;
        f32 q14 = l_stickLower;
        self->m030 = q9;
        self->mCurveWeight = q3;
        f32 q15 = l_setupJumpCushion;
        f32 q16 = l_setupM044;
        self->mBaseCushion = q15;
        self->m098 = q16;
        self->m054 = q8;
        f32 q17 = l_bgEndDistance;
        self->m044 = q7;
        self->m034 = q12;
        self->m050 = q5;
        self->m060 = q13;
        f32 q18 = l_setupM030;
        f32 q19 = l_farCheckAngle;
        self->m02C = q18;
        f32 q20 = l_setupChargeBRatio;
        f32 q21 = l_setupM0B4;
        self->mModeSwitchType = 0xffffffff;
        self->mChargeBRatio = q21;
        self->m06C = q19;
        self->mTrimVistaHeight = q17;
        self->mCusCus = q14;
        self->mTrimCineScopeHeight = q17;
        self->mParallelDist = q16;
        self->m00C = 0x1;
        f32 q22 = l_zero;
        f32 q23 = l_setupDMCAngle;
        self->m0BC = q22;
        f32 q24 = l_farWeightL;
        f32 q25 = l_setupM0B8;
        self->mManualStartCThreshold = q24;
        self->m0B8 = q25;
        self->mManualEndVal = q14;
        f32 q26 = l_setupExtraParameter;
        self->m058 = q6;
        self->mHDVector.y = q22;
        f32 q27 = l_fwdCushion;
        self->mHDExtraParameter = q26;
        f32 q28 = l_fwdBackMargin;
        self->mDMCValue = q27;
        f32 q29 = l_setupHDVector;
        f32 q30 = l_setupM084;
        self->mHDVector.x = q29;
        self->m0C8 = 0xffffffff;
        self->mDebugFlags = 0x1;
        self->mHDVector.z = q22;
        self->m084 = q30;
        self->mChargeLatitude = q20;
        f32 q31 = l_setupM078;
        self->m0C0 = q16;
        self->mForceLockOffDist = q31;
        self->m094 = 0x96;
        self->m080 = q28;
        self->mDMCAngle = q23;
        self->m0B0 = 0x3c;
        self->mChargeTimer = 0x14;
        self->mHDDefaults[1] = q23;
        f32 q32 = l_setupHDDefault;
        f32 q33 = l_farWeightH;
        f32 q34 = l_nearWeightH;
        self->mHDDefaults[3] = q32;
        self->m07C = q34;
        self->mHDDefaults[0] = q5;
        self->mHDDefaults[2] = q13;
        self->m0B4 = q6;
        self->m0C4 = q5;
        self->m078 = q33;
        self->mForceLockOffTimer = 0x78;
    }
    return gabi::at<dCamSetup_c>(a);
}

/* 024F7858 */
void dCamSetup_c__dt(dCamSetup_c* self, s32 flag) {

    if (self) {
        self->mBGChk.__vtbl = CAM_PARAM_BG_VTABLE;
        if (u32(flag) & CAM_PARAM_DELETE_STORAGE) {
            CamParam_deallocate(self);
        }
    }
}

/* 024F7878 */
BOOL dCamSetup_c::CheckLatitudeRange(s16* angle) {
    f32 high = m060, factor = l_degreeToSigned;
    s16 max = s16(gabi::ftoi(gabi::fmuls_ppc(high, factor)));
    f32 low = m05C;
    s16 min = s16(gabi::ftoi(gabi::fmuls_ppc(low, factor)));
    auto* value = reinterpret_cast<be<s16>*>(angle);
    s16 val = *value;
    if (val > max) {
        *value = max;
        return FALSE;
    }
    if (val < min) {
        *value = min;
        return FALSE;
    }
    return TRUE;
}

/* 024F78F0 */
f32 dCamSetup_c::FanBank() {

    f32 width = (f32)this->m084, random = cM_rndFX(width), wind = dKyw_get_wind_pow(),
        base = (f32)this->m080;
    return gabi::fmadds(random, wind, base);
}

// HD: compiler-generated and template code of this translation unit.
/* 024F7BA4 */
static void __sinit_d_cam_param_cpp() {
    l_headerStatics.mZeroWords[2] = 0;
    l_headerStatics.mZeroWords[0] = 0;
    l_headerStatics.mZeroWords[3] = 0;
    l_headerStatics.mZeroWords[1] = 0;
    CamParam_registerStatic(&l_destructors.mEntries[0]);
    f32 lo = l_initialNegativePi, hi = l_initialPi;
    l_headerStatics.mNegativePi = lo;
    l_headerStatics.mPi = hi;
    CamParam_initFirstObject(&l_headerStatics.mFirstObject);
    CamParam_registerStatic(&l_destructors.mEntries[1]);
    CamParam_initSecondObject(&l_headerStatics.mSecondObject);
    CamParam_registerStatic(&l_destructors.mEntries[2]);
}

// clang-format off
/* WWHD glue
data l_headerStatics 1046EE5C
data l_destructors 101D540C
ext 0273AD10 u32 CamParam_allocate(u32 size)
ext 0273AF40 void CamParam_deallocate(void* memory)
ext 020066C0 void CamParam_angleDegree(cSAngle* angle,f32 degrees)
ext 020068B0 void CamParam_subtractAngles(cSAngle* left,cSAngle* result,cSAngle* right)
ext 0200693C void CamParam_multiplyAngle(cSAngle* angle,cSAngle* result,f32 scale)
ext 020068CC void CamParam_addAngle(cSAngle* angle,cSAngle* right)
ext 02006A3C void CamParam_degreeConstruct(cDegree* degree,f32 value)
ext 02006AE8 f32 CamParam_degreeCos(cDegree* degree)
ext 02006AC4 f32 CamParam_degreeSin(cDegree* degree)
ext 0201971C f32 CamParam_atan2(f32 y,f32 x)
ext 025F18EC void CamParam_XrotS(Mtx34* matrix,s32 angle)
ext 025F1884 void CamParam_YrotS(Mtx34* matrix,s32 angle)
ext 0201ADE0 void CamParam_subtractVectors(cXyz* left,cXyz* result,cXyz* right)
ext 02006880 void CamParam_negateAngle(cSAngle* angle,cSAngle* result)
ext 02006644 void CamParam_copyAngle(cSAngle* angle,cSAngle* source)
ext 0201AD78 cXyz* CamParam_addVectors(cXyz* left,cXyz* result,cXyz* right)
ext 028F026C void CamParam_registerStatic(void* descriptor)
ext 028ED6F8 void CamParam_initFirstObject(u8* object)
ext 028EAB2C void CamParam_initSecondObject(u8* object)

data l_stickUpper 10044734
data l_stickLower 10044730
data l_farCheckAngle 10044750
data l_nearWeightL 10044748
data l_fwdBackMargin 1004475C
data l_farWeightL 10044758
data l_wallCushion 10044768
data l_one 1004473C
data l_cornerCushion 10044764
data l_floorMargin 10044738
data l_fwdCushion 10044760
data l_nearCheckAngle 10044740
data l_nearWeightH 10044744
data l_farCheckDistance 1004474C
data l_farWeightH 10044754
data l_wallUpDistance 1004476C
data l_wallBackCushion 10044770
data l_cornerAngleMax 10044774
data l_bgRange 10044778
data l_bgSecondRange 1004477C
data l_bgEndDistance 10044780
data l_curveCutoff 100447C8
data l_negativeOne 100447CC
data l_curveScale 100447D0
data l_zero 10044790
data l_radiusEpsilon 100447D4
data l_degreeToSigned 100447D8
data l_setupLatitudeLower 100447E4
data l_setupM028 100447E0
data l_setupM02C 100447E8
data l_setupM054 100447F0
data l_setupM05C 100447F8
data l_setupSubjLinkCullDist 100447FC
data l_setupM058 10044800
data l_drawFar 100447DC
data l_setupJumpCushion 100447F4
data l_setupM044 10044804
data l_setupM030 100447EC
data l_setupChargeBRatio 1004480C
data l_setupM0B4 10044810
data l_setupDMCAngle 10044808
data l_setupM0B8 10044814
data l_setupExtraParameter 10044818
data l_setupHDVector 10044824
data l_setupM084 10044820
data l_setupM078 1004481C
data l_setupHDDefault 10044828
data l_doubleOne 10044788
data l_doubleNegativeOne 10044798
data l_doubleFour 100447A0
data l_doubleZero 100447A8
data l_positiveDenominatorEpsilon 100447B0
data l_negativeDenominatorEpsilon 100447B8
data l_doubleRatioEpsilon 100447C0
data l_halfTurnDegrees 10044830
data l_initialNegativePi 10044838
data l_initialPi 1004483C
data l_styles 1004487C
data l_styleCount 10044878
*/

// clang-format on
