/** Quadratic B-spline path, with tww member names. */
#include "d/d_spline_path.h"

static SplineHeaderStatics l_headerStatics;
static const SplinePiPair l_piPair;
enum { gc_tuSpline_VT = 0x100552F4 };
// HD: signed counters use the original wrapping register arithmetic.
static s32 splineAdd(s32 x, s32 n) {
    return s32(u32(x) + u32(n));
}

/* 025C0B18 */
void d2DBSplinePath::Init(s32 keys, s32 duration) {
    mFrame = 0;
    mKeyNum = keys;
    mState = STARTING;
    mKeyNo = 0;
    mEnd = duration;
    mDuration = duration;
    f32 numerator = keys < 2 ? 1.0f : f32(splineAdd(keys, -2));
    s32 end = mEnd;
    f32 denominator = end == 0 ? 1.0f : f32(splineAdd(end, -1));
    f32 step = numerator / denominator;
    mUser = nullptr;
    mStep = step;
}

/* 025C0BD0 */
static d2DBSplinePath* d2DBSplinePath__ct(d2DBSplinePath* p) {
    // HD: preserve nullable construction and the emitted zeroing calls.
    if (!p) {
        p = static_cast<d2DBSplinePath*>(operator_new(sizeof(d2DBSplinePath)));
    }
    if (p) {
        p->mKeyNum = 0;
        p->mEnd = 0;
        p->__vtbl = gc_tuSpline_VT;
        p->field_0x18 = 0;
        p->field_0x20 = 0;
        p->mState = d2DBSplinePath::STOPPED;
        p->mFrame = 0;
        p->mDuration = 0;
        p->mKeyNo = 0;
        p->mStep = 0.0f;
        SplineZeroWords(p->mCurveWeight, sizeof(p->mCurveWeight));
        SplineZeroWords(p->mCurveKey, sizeof(p->mCurveKey));
        p->mUser = nullptr;
        p->Init(0, 0);
    }
    return p;
}

/* 025C0C80 */
bool d2DBSplinePath::Step() {
    s32 duration = mDuration, frame = mFrame, state;
    if (frame >= duration) {
        state = STOPPED;
        mState = STOPPED;
    } else {
        state = mState;
        if (state == STARTING) {
            mState = RUNNING;
            state = RUNNING;
            frame = mFrame;
        }
        if (state == RUNNING) {
            s32 end = mEnd;
            if (frame > splineAdd(end, -1)) {
                state = STOPPED;
                mState = STOPPED;
            } else {
                f32 t = gabi::fmuls_ppc(mStep, (f32)frame);
                s32 key = gabi::ftoi(t);
                duration = mDuration;
                frame = mFrame;
                mKeyNo = key;
                t = gabi::fsubs_ppc(t, (f32)key);
                if (frame == splineAdd(duration, -1)) {
                    mState = LAST;
                }
                end = mEnd;
                if (frame == splineAdd(end, -1)) {
                    mState = LAST;
                    t = 1.0f;
                }
                s32 count = mKeyNum;
                state = mState;
                frame = mFrame;
                f32 tSquared = gabi::fmuls_ppc(t, t);
                key = mKeyNo;
                f32 inverse = gabi::fsubs_ppc(1.0f, t);
                s32 last = splineAdd(count, -1), next = splineAdd(key, 1), second = splineAdd(key, 2);
                f32 inverseSquared = gabi::fmuls_ppc(inverse, inverse);
                mCurveKey[0] = key < last ? key : last;
                mCurveKey[1] = last >= next ? next : last;
                f32 weight0 = gabi::fmuls_ppc(inverseSquared, 0.5f);
                mCurveWeight[0] = weight0;
                mFrame = splineAdd(frame, 1);
                mCurveKey[2] = second < last ? second : last;
                mCurveWeight[1] = gabi::fmadds(t, inverse, 0.5f);
                mCurveWeight[2] = gabi::fmuls_ppc(tSquared, 0.5f);
            }
        }
    }
    return state == RUNNING || state == LAST;
}

// HD: index arithmetic wraps in the 32-bit guest address space.
template <typename T>
static T* splineElement(T* values, s32 index) {
    return gabi::at<T>(gabi::ea(values) + u32(index) * sizeof(T));
}

/* 025C0E38 */
cXyz* d2DBSplinePath::Calc(cXyz* out, cXyz* values) {
    s32 k1 = mCurveKey[1], k0 = mCurveKey[0];
    f32 w1 = mCurveWeight[1];
    cXyz* a1 = splineElement(values, k1);
    cXyz* a0 = splineElement(values, k0);
    f32 x1 = f32(a1->x);
    s32 k2 = mCurveKey[2];
    f32 x0 = f32(a0->x), w0 = mCurveWeight[0];
    f32 x01 = gabi::fmadds(x0, w0, gabi::fmuls_ppc(x1, w1));
    f32 z1 = f32(a1->z), y1 = f32(a1->y);
    cXyz* a2 = splineElement(values, k2);
    f32 y0 = f32(a0->y);
    f32 z01 = gabi::fmuls_ppc(z1, w1), y01 = gabi::fmuls_ppc(y1, w1);
    f32 x2 = f32(a2->x), z0 = f32(a0->z);
    y01 = gabi::fmadds(y0, w0, y01);
    f32 w2 = mCurveWeight[2];
    z01 = gabi::fmadds(z0, w0, z01);
    f32 y2 = f32(a2->y);
    f32 x = gabi::fmadds(x2, w2, x01);
    f32 z2 = f32(a2->z);
    f32 y = gabi::fmadds(y2, w2, y01), z = gabi::fmadds(z2, w2, z01);
    if (!out) {
        out = static_cast<cXyz*>(operator_new(sizeof(cXyz)));
    }
    if (out) {
        out->y = y;
        out->x = x;
        out->z = z;
    }
    return out;
}

/* 025C0F30 */
f32 d2DBSplinePath::CalcScalar(be<f32>* values) {
    s32 k0 = mCurveKey[0], k1 = mCurveKey[1];
    f32 w1 = mCurveWeight[1];
    s32 k2 = mCurveKey[2];
    f32 x0 = f32(*splineElement(values, k0));
    f32 x1 = f32(*splineElement(values, k1));
    f32 w0 = mCurveWeight[0], t = gabi::fmuls_ppc(x1, w1);
    f32 x2 = f32(*splineElement(values, k2));
    t = gabi::fmadds(x0, w0, t);
    f32 w2 = mCurveWeight[2];
    return gabi::fmadds(x2, w2, t);
}

/* 025C0F70 */
static void __sinit_d_spline_path_cpp() {
    // HD: adjacent common-header initializer and registrations retain their store order.
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101EBAB0);
    f32 low = l_piPair.negativePi, high = l_piPair.positivePi;
    l_headerStatics.negativePi = low;
    l_headerStatics.positivePi = high;
    SplineFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101EBABC);
    SplineSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101EBAC8);
}

/* WWHD glue (tools/verify/autoglue.py):
data l_headerStatics 1047D978
data l_piPair 10055310
ext 028F521C void SplineZeroWords(void*, u32)
ext 028ED6F8 void SplineFirstHeaderObject_ct(u8*)
ext 028EAB2C void SplineSecondHeaderObject_ct(u8*)
*/
