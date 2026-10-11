/** Snapshot fade overlap. */
#include "d/d_ovlp_fade3.h"

// HD: This TU's derived process and draw-packet vtables.
enum : u32 {
    overlapVtable = 0x1005191C,
    snapshotVtable = 0x10051940,
    texturePacketVtable = 0x1004CBC0,
};

static dOvlpFadeRenderer_c* l_renderer;
static dOvlpDrawRegistry_c* l_drawRegistry;
static f32 mFadeRate;
static u8 mFade;
static f32 l_fadeReset = 1.0f;
static f32 l_fadeBias = -0.04f;
static f32 l_fadeComplete = 0.0f;
static f32 l_fadeSpeed;

static BOOL dOvlpFd3_execFirstSnap(u32);
static BOOL dOvlpFd3_execFadeOut(u32);
static BOOL dOvlpFd3_execNextSnap(u32);
static BOOL dOvlpFd3_execFadeIn(u32);

/* 025A2514 */
static BOOL dOvlpFd3_Execute(u32 self) {
    dOvlpFd3_c* i_this = gabi::at<dOvlpFd3_c>(self);
    return (i_this->*i_this->mExecuteFn)();
}

/* 025A2558 */
static BOOL dOvlpFd3_Delete(u32 self) {
    dOvlpFd3_c* i_this = gabi::at<dOvlpFd3_c>(self);
    if (self) {
        dDlst_base_destroy(&i_this->field_0x120, 0);
        dDlst_base_destroy(&i_this->field_0xdc, 0);
        dDlst_base_destroy(&i_this->mSnapshot_c, 0);
        fpcBs_destruct(&i_this->base, 0);
    }
    f32 value = l_fadeReset;
    dOvlpFadeRenderer_c* renderer = l_renderer;
    mFade = 0;
    mFadeRate = value;
    renderer->release(4);
    l_drawRegistry->clear();
    return TRUE;
}

/* 025A25F0 */
static u32 dOvlpFd3_construct(u32 self) {
    if (!self) self = gabi::ea(operator_new(sizeof(dOvlpFd3_c)));
    if (self) {
        dOvlpFd3_c* i_this = gabi::at<dOvlpFd3_c>(self);
        fpcBs_construct(&i_this->base);
        i_this->base.__vtbl = overlapVtable;
        dDlst_base_construct(&i_this->mSnapshot_c);
        i_this->mSnapshot_c.__vtbl = snapshotVtable;
        dDlst_base_construct(&i_this->field_0xdc);
        i_this->field_0xdc.__vtbl = texturePacketVtable;
        dDlst_base_construct(&i_this->field_0x120);
        i_this->setExecute(dOvlpFd3_execFirstSnap);
        i_this->field_0x120.__vtbl = texturePacketVtable;
        l_drawRegistry->add(i_this);
    }
    return self;
}

/* 025A26AC */
static u32 dOvlpFd3_Create(u32 self) {
    if (self) dOvlpFd3_construct(self);
    return cPhs_COMPLEATE_e;
}

/* 025A26D8 */
static BOOL dOvlpFd3_execFirstSnap(u32 self) {
    dOvlpFd3_c* i_this = gabi::at<dOvlpFd3_c>(self);
    if (i_this->field_0x164) {
        i_this->setExecute(dOvlpFd3_execFadeOut);
        fopOvlpM_Done(i_this);
        dComIfGp_setWindowNum(0);
        f32 speed = l_fadeSpeed;
        f32 bias = l_fadeBias;
        // HD: Snapshot fades use a configurable, single-precision speed.
        mDoGph_gInf_c::fadeOut(-gabi::fadds_ppc(speed, bias));
    }
    return TRUE;
}

/* 025A2750 */
static BOOL dOvlpFd3_execFadeOut(u32 self) {
    dOvlpFd3_c* i_this = gabi::at<dOvlpFd3_c>(self);
    dComIfGp_setWindowNum(0);
    if (mFade && mFadeRate == l_fadeComplete && fopOvlpM_IsOutReq(i_this)) {
        fopOvlpM_SceneIsStart();
        i_this->setExecute(dOvlpFd3_execNextSnap);
    }
    return TRUE;
}

/* 025A27E4 */
static BOOL dOvlpFd3_execNextSnap(u32 self) {
    dOvlpFd3_c* i_this = gabi::at<dOvlpFd3_c>(self);
    i_this->mExecuteFn.i = -1;
    i_this->mExecuteFn.f = gabi::guest_fn((const void*)dOvlpFd3_execFadeIn);
    i_this->field_0x164 = 0;
    i_this->mExecuteFn.d = 0;
    dOvlp_play_c* play = dOvlp_getPlay();
    f32 bias = l_fadeBias;
    play->mWindowNum = 1;
    f32 speed = l_fadeSpeed;
    mDoGph_gInf_c::fadeOut(gabi::fadds_ppc(bias, speed));
    return TRUE;
}

/* 025A2850 */
static BOOL dOvlpFd3_execFadeIn(u32 self) {
    dOvlpFd3_c* i_this = gabi::at<dOvlpFd3_c>(self);
    fopOvlpM_SceneIsStop();
    dComIfGp_setWindowNum(0);
    if (!mFade) {
        fopOvlpM_Done(i_this);
        dComIfGp_setWindowNum(1);
    }
    return TRUE;
}

/* 025A28B0 */
static void dOvlpFd3_nativeDraw(u32 self, u32 context) {
    dOvlpFd3_c* i_this = gabi::at<dOvlpFd3_c>(self);
    u8 first = i_this->field_0x164;
    dOvlpFadeRenderer_c* renderer = l_renderer;
    if (!first) {
        renderer->capture(4, context);
        i_this->field_0x164 = 1;
        return;
    }
    renderer->draw(4, context, 1);
}

/* 025A291C */
static void __sinit_d_ovlp_fade3_cpp() {
    sinit_header_statics_rodata(0x1047B11C, 0x101EA34C, 0x1047B128, 0x10051938);
}

/* 025A29B0 */
static BOOL dOvlpFd3_Draw() { return TRUE; }

/* 025A29B8 */
static BOOL dOvlpFd3_IsDelete() { return TRUE; }

/* 025A29C0 */
// HD: Snapshot work is handled by the native renderer callback.
static void dDlst_snapShot_draw() {}

/* WWHD glue
data l_renderer 101F9968
data l_drawRegistry 101F86B4
data mFadeRate 101D6160
data mFade 101D616C
data l_fadeReset 1005192C
data l_fadeBias 10051930
data l_fadeComplete 10051934
data l_fadeSpeed 1047BA0C
ext 0252CCBC void dDlst_base_construct(dDlst_base_c*)
ext 0252CCFC void dDlst_base_destroy(dDlst_base_c*, s32)
ext 025DD5F0 void fpcBs_construct(base_process_class*)
ext 025DD630 void fpcBs_destruct(base_process_class*, s32)
ext 025DBDDC void fopOvlpM_Done(dOvlpFd3_c*)
ext 025DBDC4 u32 fopOvlpM_IsOutReq(dOvlpFd3_c*)
ext 025DBD74 void fopOvlpM_SceneIsStart()
ext 025DBD24 void fopOvlpM_SceneIsStop()
ext 0252F590 void mDoGph_fadeOut(f32)
ext 027F84A4 void dOvlpFadeRenderer_c::capture(s32, u32)
ext 027F8974 void dOvlpFadeRenderer_c::draw(s32, u32, s32)
ext 027F88C8 void dOvlpFadeRenderer_c::release(s32)
ext 02728DE8 void dOvlpDrawRegistry_c::add(dOvlpFd3_c*)
ext 02728EE0 void dOvlpDrawRegistry_c::clear()
*/
