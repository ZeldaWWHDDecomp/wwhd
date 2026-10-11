/* HD-only swipe/stick cursor inertia and four-way direction detector. */
#include "d/hd_input_cursor.h"
using gabi::fmuls_ppc;
using gabi::fadds_ppc;
using gabi::fsubs_ppc;
using gabi::fmadds;
using gabi::fnmsubs;
using gabi::frsqrte;
static u32 l_cursorVtable;
static f32 l_zero;
static f32 l_half;
static f32 l_three;
static f32 l_one;
static f32 l_unusedRate;
static f32 l_returnRate;
static f32 l_inputScaleX;
static f32 l_inputScaleY;
static u32 l_iconVtable;
static HdCursorIconName l_controllerIcon;
static HdCursorIconName l_controllerIconAlt;
static HdCursorGlobals l_globals1;
static HdCursorRange l_range1;
static f32 l_lower1;
static f32 l_upper1;
static HdCursorRegistration l_registration1_1;
static HdCursorRegistration l_registration1_2;
static HdCursorRegistration l_registration1_3;
static HdCursorGlobals l_globals2;
static HdCursorRange l_range2;
static f32 l_lower2;
static f32 l_upper2;
static HdCursorRegistration l_registration2_1;
static HdCursorRegistration l_registration2_2;
static HdCursorRegistration l_registration2_3;

enum HdCursorDirection : u32 { HdCursor_None = 0, HdCursor_Up = 1, HdCursor_Right = 2,
    HdCursor_Down = 3, HdCursor_Left = 4 };
enum HdCursorAngle : u32 { HdCursor_QuarterTurn = 0x40000000, HdCursor_HalfTurn = 0x80000000,
    HdCursor_RightLimit = 0x2AAAAAAA, HdCursor_UpLimit = 0x55555554,
    HdCursor_LeftLimit = 0xAAAAAAA8, HdCursor_DownLimit = 0xD5555552 };

/* 0261A198 */
static HdInputCursor* HdInputCursor__ct(HdInputCursor* self) {
    if (self == NULL) {
        self = static_cast<HdInputCursor*>(hdCursorAllocate(sizeof(HdInputCursor)));
        if (self == NULL) return NULL;
    }
    self->mDirection = HdCursor_None;
    self->mVtable = &l_cursorVtable;
    // HD: retain embedded-constructor null checks after guest address wrapping.
    if (!gabi::ea(self->mHistory)) hdCursorAllocate(sizeof(self->mHistory));
    self->mActive = 0;
    f32 zero = l_zero;
    HdCursorPoint* cursor = &self->mCursor;
    if (!gabi::ea(cursor)) cursor = static_cast<HdCursorPoint*>(hdCursorAllocate(sizeof(HdCursorPoint)));
    if (cursor != NULL) { cursor->y = zero; cursor->x = zero; }
    f32 one = l_one;
    f32 half = l_half;
    self->mInertiaFactor = one;
    self->mUnusedRate = l_unusedRate;
    self->mScale.x = one;
    self->mInertiaFrames = 10;
    self->mDirectionThreshold = half;
    self->mFramesLeft = 0;
    self->mReturnRate = l_returnRate;
    self->mScale.y = one;
    for (u32 i = 0; i < 5; i++) {
        self->mHistory[i].x = zero;
        self->mHistory[i].y = zero;
    }
    return self;
}

// HD: retain the double estimate and PowerPC 25-bit operand rounding.
static f32 approxLen(f32 l2, f32 z, f32 half, f32 three) {
    if (!(l2 > z)) return fmuls_ppc(z, l2);
    const f64 e = frsqrte(l2);
    const f32 e2 = f32(e * round25(e));
    const f32 eh = f32(e * round25(half));
    const f32 t = fnmsubs(e2, l2, three);
    return fmuls_ppc(fmuls_ppc(t, eh), l2);
}

/* 0261A2C0 */
void HdInputCursor::update(HdCursorPoint* input) {
    const f32 z = l_zero, three = l_three, half = l_half;
    const f32 ix = input->x;
    if (!(ix == z) || !(input->y == z)) {
        const f32 lx = mHistory[4].x;
        mActive = 1;
        if (lx == z && mHistory[4].y == z) mFramesLeft = mInertiaFrames;
        for (u32 i = 0; i < 4; i++) {
            mHistory[i].x = (f32)mHistory[i + 1].x;
            mHistory[i].y = (f32)mHistory[i + 1].y;
        }
        gabi::Local<HdCursorSample> sample;
        sample->mWords[0] = *reinterpret_cast<be<u32>*>(&input->x);
        const f32 nx = sample->mPoint.x / l_inputScaleX;
        sample->mWords[1] = *reinterpret_cast<be<u32>*>(&input->y);
        const f32 ny = sample->mPoint.y / l_inputScaleY;
        mHistory[4].x = nx;
        mHistory[4].y = ny;
        f32 ax = z, ay = z;
        for (u32 i = 0; i < 4; i++) {
            HdCursorPoint* current = &mHistory[i];
            HdCursorPoint* next = &mHistory[i + 1];
            if (current->x == z && current->y == z) continue;
            const f32 dx = fsubs_ppc(next->x, current->x);
            const f32 dy = fsubs_ppc(next->y, current->y);
            ax = fadds_ppc(ax, dx);
            ay = fadds_ppc(ay, dy);
        }
        f32 vx = fmuls_ppc(ax, mScale.x);
        const s32 left = s32(mFramesLeft);
        const f32 cy0 = mCursor.y, cx0 = mCursor.x;
        f32 vy = fmuls_ppc(ay, mScale.y);
        if (left > 0) {
            const f32 d = mInertiaFactor;
            vx = fmuls_ppc(vx, d);
            mFramesLeft = u32(left - 1);
            vy = fmuls_ppc(vy, d);
        }
        f32 cx = fadds_ppc(cx0, vx);
        const f32 one = l_one;
        f32 cy = fadds_ppc(cy0, vy);
        mCursor.x = cx;
        mCursor.y = cy;
        if (cx > one) cx = one;
        mCursor.x = cx;
        if (cy > one) cy = one;
        const f32 l2 = fmadds(cx, cx, fmuls_ppc(cy, cy));
        mCursor.y = cy;
        if (approxLen(l2, z, half, three) > one) hdCursorNormalize(&mCursor);
    } else {
        mActive = 0;
        for (u32 i = 0; i < 5; i++) {
            mHistory[i].x = z;
            mHistory[i].y = z;
        }
        const f32 rate = mReturnRate;
        gabi::Local<HdCursorPoint> target;
        target->x = z;
        target->y = z;
        mDirection = HdCursor_None;
        hdCursorChase(&mCursor, target.get(), rate);
    }
    const f32 y2 = mCursor.y, x2 = mCursor.x;
    const f32 l2 = fmadds(x2, x2, fmuls_ppc(y2, y2));
    if (!(approxLen(l2, z, half, three) > mDirectionThreshold)) {
        mDirection = HdCursor_None;
        return;
    }
    const f32 x = mCursor.x, y = mCursor.y;
    if (x == z && y == z) return;
    u32 a;
    if (!(x < z)) {
        if (!(y < z)) {
            if (!(x < y)) a = hdCursorAtan(y / x);
            else a = HdCursor_QuarterTurn - hdCursorAtan(x / y);
        } else {
            if (!(x < -y)) a = 0u - hdCursorAtan(-(y / x));
            else a = hdCursorAtan(-(x / y)) - HdCursor_QuarterTurn;
        }
    } else {
        if (!(y < z)) {
            if (!(-x < y)) a = HdCursor_HalfTurn - hdCursorAtan(-(y / x));
            else a = hdCursorAtan(-(x / y)) + HdCursor_QuarterTurn;
        } else {
            if (x > y) a = 0u - hdCursorAtan(x / y) - HdCursor_QuarterTurn;
            else a = hdCursorAtan(y / x) - HdCursor_HalfTurn;
        }
    }
    u32 dir;
    if (a <= HdCursor_RightLimit) {
        if (!a) return;
        dir = HdCursor_Right;
    } else if (a <= HdCursor_UpLimit) dir = HdCursor_Up;
    else if (a <= HdCursor_LeftLimit) dir = HdCursor_Left;
    else if (a <= HdCursor_DownLimit) dir = HdCursor_Down;
    else dir = HdCursor_Right;
    mDirection = dir;
}

/* 0261A9B8 */
void HdInputCursor::updateThunk(HdCursorPoint* input) { update(input); }

static void initializeGlobals(HdCursorGlobals* globals, HdCursorRegistration* first,
                              be<f32>* lower, be<f32>* upper, HdCursorRange* range,
                              HdCursorRegistration* second, HdCursorRegistration* third) {
    globals->mThird = 0;
    globals->mFirst = 0;
    globals->mFourth = 0;
    globals->mSecond = 0;
    hdCursorRegister(first);
    f32 lowerValue = *lower;
    f32 upperValue = *upper;
    range->mLower = lowerValue;
    range->mUpper = upperValue;
    hdCursorInitSigned(&range->mSignedInitialized);
    hdCursorRegister(second);
    hdCursorInitUnsigned(&range->mUnsignedInitialized);
    hdCursorRegister(third);
}

/* 0261A9BC */
static void __sinit_hd_input_cursor_cpp() {
    initializeGlobals(&l_globals1, &l_registration1_1, &l_lower1, &l_upper1, &l_range1,
                      &l_registration1_2, &l_registration1_3);
    l_controllerIconAlt.mVtable = &l_iconVtable;
    l_controllerIconAlt.mpName = gabi::at<const char>(gabi::guest_addr("controller alternate"));
    l_controllerIcon.mVtable = &l_iconVtable;
    l_controllerIcon.mpName = gabi::at<const char>(gabi::guest_addr("controller"));
}

/* 0261ABF0 */
static void __sinit_hd_input_cursor_second_cpp() {
    initializeGlobals(&l_globals2, &l_registration2_1, &l_lower2, &l_upper2, &l_range2,
                      &l_registration2_2, &l_registration2_3);
}

/* WWHD glue (tools/verify/autoglue.py): HD cursor tuning and icon labels.
data l_cursorVtable 100E2B54
data l_zero 100E2B08
data l_half 100E2B0C
data l_three 100E2B10
data l_one 100E2B14
data l_unusedRate 100E2B68
data l_returnRate 100E2B64
data l_inputScaleX 100E2B6C
data l_inputScaleY 100E2B70
data l_iconVtable 100E2B1C
data l_controllerIcon 1048DEBC
data l_controllerIconAlt 1048DEC4
str 100E2B80 "controller alternate" renamed
str 100E2B90 "controller" renamed
data l_globals1 1048DED8
data l_range1 1048DECC
data l_lower1 100E2B78
data l_upper1 100E2B7C
data l_registration1_1 101F50E0
data l_registration1_2 101F50EC
data l_registration1_3 101F50F8
data l_globals2 1048DEF4
data l_range2 1048DEE8
data l_lower2 100E2BA8
data l_upper2 100E2BAC
data l_registration2_1 101F5104
data l_registration2_2 101F5110
data l_registration2_3 101F511C
ext 0273AD10 void* hdCursorAllocate(u32)
ext 0261AAA0 void hdCursorNormalize(HdCursorPoint*)
ext 0261AB28 u32 hdCursorChase(HdCursorPoint*, HdCursorPoint*, f32)
ext 02756D88 u32 hdCursorAtan(f32)
ext 028F026C void hdCursorRegister(void*)
ext 028ED6F8 void hdCursorInitSigned(be<u8>*)
ext 028EAB2C void hdCursorInitUnsigned(be<u8>*)
*/
