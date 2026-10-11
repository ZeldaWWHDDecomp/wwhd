/** Graphics state and native display wrappers, using tww names and WWHD layouts.
 * HD omits the GameCube capture/renderer body. Verified against cking.rpx. */
#include "m_Do/m_Do_graphic.h"
const gc_tu_data gc_tu = {.headerStatics = {0x1048CF68, 0x101F47E4, 0x1048CF74}};
static mDoGph_StateHD l_graphics;
static mDoGph_Color l_defaultColor;
using GraphicsDisplayPtr = gptr<mDoGph_DisplayHD>;
static GraphicsDisplayPtr l_display;
static Mtx34 l_blureMtx;
static Mtx34 l_defaultBlureMtx;
static mDoGph_HeaderStaticsHD l_header;
static f32 l_minAngle = -3.1415927410125732f;
static f32 l_maxAngle = 3.1415927410125732f;
// HD: static-method bodies use free entry adapters because autoglue assumes a this parameter.

/* 025F03C4 */
static BOOL mDoGph_BeforeOfDraw() {
    mDoGph_GameInfoHD *play = mDoGph_getGameInfo();
    mDoGph_drawListBefore(play->mDrawList);
    return TRUE;
}
/* 025F03F0 */
static BOOL mDoGph_AfterOfDraw() {
    mDoGph_GameInfoHD *play = mDoGph_getGameInfo();
    mDoGph_drawListAfter(play->m2DList);
    return TRUE;
}
// inlined into mDoGph_createHeap
void mDoGph_gInf_c::createHeap() {
    // HD: heap debug labels are kept as mapped strings (Shift-JIS in the image).
    void *parent = mDoGph_getParentHeap();
    mDoGph_HeapHD *first = mDoGph_createHeapHD(0x10000, parent, 0);
    l_graphics.mHeap[0] = first;
    if (!first)
        JUT_ASSERT_fail("m_Do_graphic.cpp", 0xFB, "mHeap[0] != NULL");
    mDoGph_HeapHD *second = mDoGph_createHeapHD(0x10000, parent, 0);
    l_graphics.mHeap[1] = second;
    if (!second)
        JUT_ASSERT_fail("m_Do_graphic.cpp", 0xFD, "mHeap[1] != NULL");
    l_graphics.mHeap[0]->setName("描画用一時ヒープ[0]");
    l_graphics.mHeap[1]->setName("描画用一時ヒープ[1]");
    l_graphics.mCurrentHeap = 0;
}

/* 025F041C */
void mDoGph_createHeap() {
    mDoGph_gInf_c::createHeap();
}
// inlined into mDoGph_create
void mDoGph_gInf_c::create() {
    // HD: native display setup replaces the GC frame/depth buffer allocations.
    mDoGph_createHeap();
    mDoGph_initializeBuffers(0x400);
    l_graphics.mBlureFlag = 0;
    l_graphics.mFade = 0;
    u32 color = (u32)l_defaultColor.word;
    l_graphics.mBackColor.word = color;
    l_graphics.mFadeColor.word = color;
}

/* 025F04EC */
void mDoGph_create() {
    mDoGph_gInf_c::create();
}
/* 025F0550 */
static BOOL mDoGph_Create() {
    mDoGph_HeapHD *heap = mDoGph_createGameHeap(0, 0, 0);
    if (!heap)
        JUT_ASSERT_fail("m_Do_graphic.cpp", 0xD1C, "heap != NULL");
    heap->setName("ドルフィングラフィック(mDoGph)");
    mDoGph_create();
    mDoGph_GameInfoHD *play = mDoGph_getGameInfo();
    mDoGph_drawListInitialize(play->mDrawList);
    mDoGph_adjustGameHeap(heap);
    mDoGph_restoreCurrentHeap();
    return TRUE;
}
/* 025F05D0 */
static u32 mDoGph_displayControl(u32 first, u32 second) {
    return mDoGph_displayControlHD(l_display, first, second);
}
/* 025F05E8 */
static void mDoGph_displayControlOne(u32 first) {
    mDoGph_displayControlOneHD(l_display, first);
}
/* 025F05F8 */
static void mDoGph_setFadeColor(const mDoGph_Color *color) {
    mDoGph_DisplayHD *display = l_display;
    u32 value = color->word;
    mDoGph_FaderHD *fader = display->mFader;
    fader->mColor.word = value;
}
// inlined into mDoGph_onBlureMatrix
void mDoGph_gInf_c::onBlure(const Mtx34 *matrix) {
    l_graphics.mBlureFlag = 1;
    PSMTXCopy(matrix, &l_blureMtx);
}

/* 025F0634 */
void mDoGph_onBlureMatrix(const Mtx34 *matrix) {
    mDoGph_gInf_c::onBlure(matrix);
}
// inlined into mDoGph_onBlure
void mDoGph_gInf_c::onBlure() {
    mDoGph_onBlureMatrix(&l_defaultBlureMtx);
}

/* 025F064C */
void mDoGph_onBlure() {
    mDoGph_gInf_c::onBlure();
}
// inlined into mDoGph_fadeOut
void mDoGph_gInf_c::fadeOut(f32 speed, const mDoGph_Color &color) {
    l_graphics.mFade = 1;
    l_graphics.mFadeSpeed = speed;
    u32 value = color.word;
    l_graphics.mFadeColor.word = value;
    l_graphics.mFadeRate = speed >= 0.0f ? 0.0f : 1.0f;
}

/* 025F0658 */
void mDoGph_fadeOut(f32 speed, const mDoGph_Color *color) {
    mDoGph_gInf_c::fadeOut(speed, *color);
}
// inlined into mDoGph_fadeOutDefault
void mDoGph_gInf_c::fadeOut(f32 speed) {
    mDoGph_fadeOut(speed, &l_defaultColor);
}

/* 025F069C */
void mDoGph_fadeOutDefault(f32 speed) {
    mDoGph_gInf_c::fadeOut(speed);
}
// inlined into mDoGph_calcFade
void mDoGph_gInf_c::calcFade() {
    // HD: the native display applies the color instead of drawing a GX quad.
    if (l_graphics.mFade) {
        f32 speed = l_graphics.mFadeSpeed;
        f32 rate = gabi::fadds_ppc(l_graphics.mFadeRate, speed);
        if (rate < 0.0f) {
            rate = 0.0f;
            l_graphics.mFadeRate = rate;
            l_graphics.mFade = 0;
        } else if (rate > 1.0f) {
            rate = 1.0f;
            l_graphics.mFadeRate = rate;
        } else
            l_graphics.mFadeRate = rate;
        u8 alpha = (u8)gabi::ftoi(gabi::fmuls_ppc(255.0f, rate));
        l_graphics.mFadeColor.rgba.a = alpha;
        if (!alpha)
            return;
    } else {
        mDoGph_GameInfoHD *play = mDoGph_getGameInfo();
        if ((u8)play->mFadeBrightness == 255) {
            l_graphics.mFadeColor.rgba.a = 0;
            return;
        }
        l_graphics.mFadeColor.rgba.r = 0;
        l_graphics.mFadeColor.rgba.g = 0;
        l_graphics.mFadeColor.rgba.b = 0;
        play = mDoGph_getGameInfo();
        u8 alpha = 255 - (u8)play->mFadeBrightness;
        l_graphics.mFadeColor.rgba.a = alpha;
        if (!alpha)
            return;
    }
    mDoGph_applyFade(l_display);
}

/* 025F06A8 */
void mDoGph_calcFade() {
    mDoGph_gInf_c::calcFade();
}
// inlined into mDoGph_onMonotone
void mDoGph_gInf_c::onMonotone() {
    // HD: there is no GC frame-buffer texture swap.
    l_graphics.mMonotone = 1;
}

/* 025F0820 */
void mDoGph_onMonotone() {
    mDoGph_gInf_c::onMonotone();
}
// inlined into mDoGph_offMonotone
void mDoGph_gInf_c::offMonotone() {
    // HD: there is no GC frame-buffer texture swap.
    l_graphics.mMonotone = 0;
}

/* 025F0830 */
void mDoGph_offMonotone() {
    mDoGph_gInf_c::offMonotone();
}
// inlined into mDoGph_calcMonotone
void mDoGph_gInf_c::calcMonotone() {
    s16 speed = l_graphics.mMonotoneRateSpeed;
    s32 target = speed < 0 ? 400 : -600;
    s32 step = (s16)(speed < 0 ? -speed : speed);
    s32 complete = mDoGph_chaseMonotone(&l_graphics.mMonotoneRate, target, step);
    if (complete && l_graphics.mMonotoneRateSpeed > 0)
        mDoGph_offMonotone();
}

/* 025F0840 */
void mDoGph_calcMonotone() {
    mDoGph_gInf_c::calcMonotone();
}
// HD: compiler-generated header objects and graphics interface companions.
/* 025F08B0 */
static void __sinit_m_Do_graphic_cpp() {
    for (int i = 0; i < 4; ++i)
        l_header.mZero[i] = 0;
    __register_global_object(gc_tu.headerStatics[1]);
    l_header.mMinAngle = l_minAngle;
    l_header.mMaxAngle = l_maxAngle;
    // HD: the one-byte header objects are two bytes later than in most TUs.
    mDoGph_initializeAngle(&l_header.mAngleTable);
    __register_global_object(gc_tu.headerStatics[1] + 0xc);
    mDoGph_initializeMath(&l_header.mMathTable);
    __register_global_object(gc_tu.headerStatics[1] + 0x18);
}

/* 025F0944 */
static void mDoGph_BlankingOnStub() {}

/* 025F0948 */
static void mDoGph_BlankingOffStub() {}

/* 025F094C */
static u32 mDoGph_interfaceCreate() {
    return 1;
}

/* 025F0954 */
static void SafeString_deletingDtor_m_Do_graphic(void *p, u32 flags) {
    if (p == 0)
        return;
    if (flags & 1)
        operator_delete(p);
}

/* 025F0968 */
static void mDoGph_logoHook() {}

/* 025F096C */
static void mDoGph_sceneHook() {}

/* 025F0970 */
static void SafeString_assureTerminationImpl_m_Do_graphic(void *p) {}

/* WWHD glue
data l_graphics 101F4808
data l_defaultColor 101D5E90
data l_display 101F86B4
data l_blureMtx 1048CF84
data l_defaultBlureMtx 101F48F0
data l_header 1048CF68
data l_minAngle 10058F58
data l_maxAngle 10058F5C
str 10058EF4 in 025F041C # "m_Do_graphic.cpp"
str 10058F14 in 025F0550 # "m_Do_graphic.cpp"
str 10058EAC "mHeap[0] != NULL"
str 10058EBC "mHeap[1] != NULL"
str 10058F08 "heap != NULL"
str 10058ECC "\u63cf\u753b\u7528\u4e00\u6642\u30d2\u30fc\u30d7[0]" renamed
str 10058EE0 "\u63cf\u753b\u7528\u4e00\u6642\u30d2\u30fc\u30d7[1]" renamed
str 10058F28 "\u30c9\u30eb\u30d5\u30a3\u30f3\u30b0\u30e9\u30d5\u30a3\u30c3\u30af(mDoGph)" renamed
ext 027EC230 void* mDoGph_getParentHeap()
ext 027EC464 mDoGph_HeapHD* mDoGph_createHeapHD(u32,void*,u32)
ext 027F0148 void mDoGph_initializeBuffers(u32)
ext 025E35BC mDoGph_HeapHD* mDoGph_createGameHeap(u32,u32,u32)
ext 025E3678 void mDoGph_adjustGameHeap(mDoGph_HeapHD*)
ext 025E37D8 void mDoGph_restoreCurrentHeap()
ext 025200D4 mDoGph_GameInfoHD* mDoGph_getGameInfo()
ext 0252F264 void mDoGph_drawListBefore(void*)
ext 0252E388 void mDoGph_drawListAfter(void*)
ext 0252ED28 void mDoGph_drawListInitialize(void*)
ext 02728CB8 u32 mDoGph_displayControlHD(mDoGph_DisplayHD*,u32,u32)
ext 02728CD4 void mDoGph_displayControlOneHD(mDoGph_DisplayHD*,u32)
ext 027291FC void mDoGph_applyFade(mDoGph_DisplayHD*)
ext 028ED6F8 void mDoGph_initializeAngle(be<u8>*)
ext 028EAB2C void mDoGph_initializeMath(be<u8>*)
ext 0200F564 s32 mDoGph_chaseMonotone(be<s16>*,s32,s32)
*/
