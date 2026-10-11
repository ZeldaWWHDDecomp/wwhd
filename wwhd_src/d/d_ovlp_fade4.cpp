/** Fade4 overlap transition. */
#include "d/d_ovlp_fade4.h"

using RendererPtr = gptr<dOvlpFd4_renderer_c>;
using RegistryPtr = gptr<dOvlpFd4_registry_c>;
using BlurPtr = gptr<dOvlpFd4_blur_c>;
static RendererPtr l_renderer;
static RegistryPtr l_registry;
static BlurPtr l_blur;
static f32 l_zero;
static f32 l_eight;
static f32 l_one;
static f32 l_minusOne;
static f32 l_blurStrength;
static dOvlpFd4_staticWords_c l_staticWords;
static dOvlpFd4_staticFloats_c l_staticFloats;
static f32 l_staticInitialX;
static f32 l_staticInitialY;

// HD: Address-only views of this TU's vtables and static registration records.
static dOvlpFd4_data_c l_spriteVtable;
static dOvlpFd4_data_c l_effectVtable;
static dOvlpFd4_data_c l_snapshotVtable;
static dOvlpFd4_data_c l_taskVtable;
static dOvlpFd4_data_c l_outputRegistry;
static dOvlpFd4_data_c l_floatRegistration;
static dOvlpFd4_data_c l_flagRegistration;
static dOvlpFd4_data_c l_byteRegistration;
static dOvlpFd4_data_c l_staticFlag;
static dOvlpFd4_data_c l_staticByte;

static u32 execWait(u32);
static void execFirstSnap(u32);
static void execFadeOut(u32);
static void execNextSnap(u32);
static void execFadeIn(u32);
static void drawFadeOut();
static void drawFadeIn();

// HD: Keep the original u32 entry signatures used by the verification harness.
static u32 guestFunction(const void* function) { return gabi::guest_fn(function); }
static void setExecute(dOvlpFd4_c* task, void (*function)(u32)) {
    task->mExecuteFunc.d = 0;
    task->mExecuteFunc.i = -1;
    task->mExecuteFunc.f = guestFunction((const void*)function);
}

/* 025A2B88 */
u32 dDlst_2Dt_Sp_construct(u32 self) {
    if (!self) self = gabi::ea(dOvlpFd4_alloc(sizeof(dDlst_2Dt_Sp_c)));
    if (self) {
        auto* sprite = gabi::at<dDlst_2Dt_Sp_c>(self);
        dDlst_base_construct(sprite);
        sprite->__vtbl = gabi::ea(&l_spriteVtable);
        dOvlpFd4_clear(sprite->mTexObj, sizeof(sprite->mTexObj));
        f32 zero = l_zero;
        sprite->mTimg = nullptr;
        sprite->mWidth = zero;
        sprite->mImageWidth = 0;
        sprite->mHeight = zero;
        sprite->mImageHeight = 0;
        sprite->mPosX = zero;
        sprite->mPosY = zero;
        sprite->mImagePtr = nullptr;
        dOvlpFd4_clear(&sprite->mColor, sizeof(sprite->mColor));
        sprite->mPercent = zero;
    }
    return self;
}

/* 025A2C3C */
u32 dDlst_2DtEff1_construct(u32 self) {
    if (!self) self = gabi::ea(dOvlpFd4_alloc(sizeof(dDlst_2DtEff1_c)));
    if (self) {
        auto* effect = gabi::at<dDlst_2DtEff1_c>(self);
        dDlst_base_construct(effect);
        f32 zero = l_zero;
        effect->first = false;
        effect->timer = zero;
        effect->__vtbl = gabi::ea(&l_effectVtable);
        dOvlpFd4_clear(&effect->snap_dlst, sizeof(effect->snap_dlst));
        dDlst_base_construct(&effect->snap_dlst);
        effect->snap_dlst.__vtbl = gabi::ea(&l_snapshotVtable);
        dDlst_2Dt_Sp_construct(gabi::ea(&effect->redraw_dlst));
        dOvlpFd4_clear(&effect->save_dlst, sizeof(effect->save_dlst));
        dDlst_base_construct(&effect->save_dlst);
        effect->save_dlst.__vtbl = gabi::ea(&l_snapshotVtable);
        dDlst_2Dt_Sp_construct(gabi::ea(&effect->blur0_dlst));
        dOvlpFd4_clear(&effect->blur0Snap_dlst, sizeof(effect->blur0Snap_dlst));
        dDlst_base_construct(&effect->blur0Snap_dlst);
        effect->blur0Snap_dlst.__vtbl = gabi::ea(&l_snapshotVtable);
        dDlst_2Dt_Sp_construct(gabi::ea(&effect->composite_dlst));
        effect->curHeight = 0;
        effect->curWidth = 0;
    }
    return self;
}

/* 025A2D24 */
u32 dOvlpFd4_construct(u32 self) {
    if (!self) self = gabi::ea(dOvlpFd4_alloc(sizeof(dOvlpFd4_c)));
    if (self) {
        auto* task = gabi::at<dOvlpFd4_c>(self);
        fpcBs_construct(&task->base);
        task->base.__vtbl = gabi::ea(&l_taskVtable);
        dOvlpFd4_clear(&task->mExecuteFunc, sizeof(task->mExecuteFunc));
        dOvlpFd4_clear(&task->mDrawFunc, sizeof(task->mDrawFunc));
        dOvlpFd4_clear(&task->fadeOutBlur0_dlst, sizeof(task->fadeOutBlur0_dlst));
        dDlst_base_construct(&task->fadeOutBlur0_dlst);
        task->fadeOutBlur0_dlst.__vtbl = gabi::ea(&l_snapshotVtable);
        dOvlpFd4_clear(&task->fadeOutSnap_dlst, sizeof(task->fadeOutSnap_dlst));
        dDlst_base_construct(&task->fadeOutSnap_dlst);
        task->fadeOutSnap_dlst.__vtbl = gabi::ea(&l_snapshotVtable);
        dDlst_2Dt_Sp_construct(gabi::ea(&task->fadeOutComposite_dlst));
        dDlst_2Dt_Sp_construct(gabi::ea(&task->fadeOutBlur1_dlst));
        dDlst_2DtEff1_construct(gabi::ea(&task->fadeIn_dlst));
        task->first = true;
        f32 zero = l_zero;
        task->mBlurCounter = 0;
        task->field_0x2b8 = zero;
        task->delayPercent = zero;
        task->timerStep = zero;
        task->mExecuteFunc.f = guestFunction((const void*)execWait);
        task->mDrawFunc.i = -1;
        task->mDrawFunc.f = guestFunction((const void*)drawFadeOut);
        task->mExecuteFunc.d = 0;
        task->sizeStep = 0;
        task->field_0x2bc = zero;
        task->field_0x2c4 = 0;
        task->totalTime = 0;
        task->field_0x2cc = 0;
        task->fadeOutStatus = 0;
        task->field_0x2c2 = 0;
        task->startTime = 0;
        task->timer = 0;
        task->mExecuteFunc.i = -1;
        task->mDrawFunc.d = 0;
        dOvlpFd4_getPlay()->m2DShow = false;
        task->totalTime = 39;
        // HD: The initial wait is ten frames instead of GameCube's two.
        task->timer = 10;
        task->delayPercent = zero;
        task->fadeOutStatus = 0;
    }
    return self;
}

/* 025A32E8 */
static u32 execWait(u32 self) {
    auto* task = gabi::at<dOvlpFd4_c>(self);
    s16 time = task->timer;
    if (time > 0) {
        task->timer = time - 1;
        return self; // HD: The early return retains the incoming r3 value.
    }
    setExecute(task, execFirstSnap);
    return dOvlpFd4_register(l_registry, task);
}

/* 025A2EC0 */
void dDlst_2DtEff1_draw(u32 self) {
    auto* effect = gabi::at<dDlst_2DtEff1_c>(self);
    s32 time = gabi::ftoi(gabi::fmuls_ppc(l_eight, effect->timer));
    // HD: Conversion uses the full signed word rather than GameCube's s16.
    effect->curWidth = (u32)time * -16u + 320;
    effect->curHeight = (u32)time * -12u + 240;
    if (effect->first) {
        effect->first = false;
        return;
    }
    if (dOvlpFd4_getPlay()->mWindowNum) return;
    s32 width = 320, height = 240;
    // HD: Native rendering submits these packet dimensions elsewhere.
    while (width - 16 >= effect->curWidth && height - 12 >= effect->curHeight) {
        s32 dstWidth = width - 16, dstHeight = height - 12;
        effect->blur0_dlst.mImageWidth = width / 2;
        effect->blur0_dlst.mImageHeight = height / 2;
        effect->blur0Snap_dlst.mWidth = dstWidth;
        effect->blur0Snap_dlst.mHeight = dstHeight;
        effect->blur0_dlst.mWidth = (f32)dstWidth;
        effect->blur0_dlst.mHeight = (f32)dstHeight;
        width = dstWidth;
        height = dstHeight;
    }
    f32 zero = l_zero;
    effect->composite_dlst.mImageWidth = width / 2;
    effect->composite_dlst.mPosX = zero;
    effect->composite_dlst.mPosY = zero;
    effect->composite_dlst.mImageHeight = height / 2;
}

/* 025A332C */
void dOvlpFd4_drawNative(u32 self, u32 context) {
    auto* task = gabi::at<dOvlpFd4_c>(self);
    auto* drawContext = gabi::at<dOvlpFd4_context_c>(context);
    f32 strength = l_blurStrength, one = l_one;
    auto* renderer = l_renderer.get();
    bool initial = task->mDrawFunc.i == -1 && task->mDrawFunc.d == 0 &&
        task->mDrawFunc.f == guestFunction((const void*)drawFadeOut);
    if (task->first) {
        dOvlpFd4_capture(renderer, 4, context);
        task->first = false;
        if (initial) return;
    }
    // HD: Snapshot textures and a native blur replace the GameCube GX draw lists.
    dOvlpFd4_setSize(drawContext->mSize);
    dOvlpFd4_setTarget(drawContext->mTarget, drawContext->mSize);
    u32 image = dOvlpFd4_getImage(renderer, 4);
    gabi::Local<dOvlpFd4_texture_c> texture;
    dOvlpFd4_initTexture(texture.get(), image);
    texture->mDirty |= 4;
    auto* size = drawContext->mSize.get();
    f32 denominator = gabi::fadds_ppc((f32)task->mBlurCounter, one);
    texture->mFilter[0] = 0;
    texture->mFilter[1] = 0;
    texture->mFilter[2] = 0;
    gabi::Local<dOvlpFd4_scale_c> scale;
    scale->x = (f32)(size->mWidth / denominator);
    scale->y = (f32)(size->mHeight / denominator);
    auto* output = dOvlpFd4_getOutput(gabi::ea(&l_outputRegistry));
    u32 result = dOvlpFd4_blur(l_blur, texture.get(), scale.get(), output->mTexture,
                             (f32)task->mBlurCounter, strength);
    output = dOvlpFd4_getOutput(gabi::ea(&l_outputRegistry));
    output->mTexture = result;
    output->mFlags = 0;
    dOvlpFd4_destroyTexture(texture.get(), 2);
}

/* 025A3014 */
static void execFirstSnap(u32 self) {
    auto* task = gabi::at<dOvlpFd4_c>(self);
    if (!task->first && !dOvlpFd4_calcTimer(&task->timer)) {
        setExecute(task, execFadeOut);
        fopOvlpM_Done(task);
        dOvlpFd4_getPlay()->mWindowNum = 0;
        task->mBlurCounter = 0;
        task->timer = 15;
    }
}

/* 025A3098 */
static void execFadeOut(u32 self) {
    auto* task = gabi::at<dOvlpFd4_c>(self);
    dOvlpFd4_getPlay()->mWindowNum = 0;
    // HD: Forty-five native blur steps replace the GameCube dimension test.
    if (!dOvlpFd4_calcTimer(&task->timer) && task->mBlurCounter >= 45 && fopOvlpM_IsOutReq(task)) {
        fopOvlpM_SceneIsStart();
        task->fadeOutStatus = 0;
        setExecute(task, execNextSnap);
        dOvlpFd4_getPlay()->m2DShow = false;
        task->mBlurCounter = 45;
        return;
    }
    if (!task->timer && task->mBlurCounter < 45) task->mBlurCounter = task->mBlurCounter + 1;
}

/* 025A3168 */
static void execNextSnap(u32 self) {
    auto* task = gabi::at<dOvlpFd4_c>(self);
    setExecute(task, execFadeIn);
    task->mDrawFunc.d = 0;
    task->mDrawFunc.i = -1;
    task->first = true;
    task->mDrawFunc.f = guestFunction((const void*)drawFadeIn);
    dOvlpFd4_getPlay()->mWindowNum = 1;
    s8 time = task->totalTime;
    f32 one = l_one, minusOne = l_minusOne;
    task->timer = time;
    task->fadeIn_dlst.timer = one;
    task->timerStep = (f32)(minusOne / (f32)time);
}

/* 025A3224 */
static void execFadeIn(u32 self) {
    auto* task = gabi::at<dOvlpFd4_c>(self);
    if (!dOvlpFd4_calcTimer(&task->timer) && !task->mBlurCounter) {
        if (!task->field_0x2cc) {
            fopOvlpM_SceneIsStart();
            task->field_0x2cc = true;
            return;
        }
        fopOvlpM_Done(task);
        dOvlpFd4_getPlay()->mWindowNum = 1;
        dOvlpFd4_getPlay()->m2DShow = true;
        return;
    }
    dOvlpFd4_getPlay()->mWindowNum = 0;
    fopOvlpM_SceneIsStop();
    u32 count = task->mBlurCounter;
    if (count) task->mBlurCounter = count - 1;
}

/* 025A29C4 */
BOOL dOvlpFd4_Draw(u32 self) {
    auto* task = gabi::at<dOvlpFd4_c>(self);
    if (task->mDrawFunc.i) (task->*task->mDrawFunc)();
    return TRUE;
}

/* 025A2A34 */
BOOL dOvlpFd4_Execute(u32 self) {
    auto* task = gabi::at<dOvlpFd4_c>(self);
    (task->*task->mExecuteFunc)();
    return TRUE;
}

/* 025A2AA0 */
BOOL dOvlpFd4_Delete(u32 self) {
    if (self) {
        auto* task = gabi::at<dOvlpFd4_c>(self);
        // HD: Keep the original reverse destruction order of the embedded draw packets.
        dDlst_base_destroy(&task->fadeIn_dlst.composite_dlst, 0);
        dDlst_base_destroy(&task->fadeIn_dlst.blur0Snap_dlst, 0);
        dDlst_base_destroy(&task->fadeIn_dlst.blur0_dlst, 0);
        dDlst_base_destroy(&task->fadeIn_dlst.save_dlst, 0);
        dDlst_base_destroy(&task->fadeIn_dlst.redraw_dlst, 0);
        dDlst_base_destroy(&task->fadeIn_dlst.snap_dlst, 0);
        dDlst_base_destroy(&task->fadeIn_dlst, 0);
        dDlst_base_destroy(&task->fadeOutBlur1_dlst, 0);
        dDlst_base_destroy(&task->fadeOutComposite_dlst, 0);
        dDlst_base_destroy(&task->fadeOutSnap_dlst, 0);
        dDlst_base_destroy(&task->fadeOutBlur0_dlst, 0);
        fpcBs_destruct(&task->base, 0);
    }
    dOvlpFd4_release(l_renderer, 4);
    dOvlpFd4_unregister(l_registry);
    return TRUE;
}

/* 025A2E94 */
u32 dOvlpFd4_Create(u32 self) {
    if (self) dOvlpFd4_construct(self);
    return cPhs_COMPLEATE_e;
}

// HD: Compiler-generated static initialization and native draw stubs.
/* 025A3634 */
void dOvlpFd4_staticInit() {
    l_staticWords.values[2] = 0;
    l_staticWords.values[0] = 0;
    l_staticWords.values[3] = 0;
    l_staticWords.values[1] = 0;
    dOvlpFd4_registerStatic(gabi::ea(&l_floatRegistration));
    f32 x = l_staticInitialX, y = l_staticInitialY;
    l_staticFloats.values[0] = x;
    l_staticFloats.values[1] = y;
    dOvlpFd4_initFlag(gabi::ea(&l_staticFlag));
    dOvlpFd4_registerStatic(gabi::ea(&l_flagRegistration));
    dOvlpFd4_initByte(gabi::ea(&l_staticByte));
    dOvlpFd4_registerStatic(gabi::ea(&l_byteRegistration));
}

/* 025A36C8 */
BOOL dOvlpFd4_IsDelete() { return TRUE; }
/* 025A36D0 */
void dDlst_snapShot2_draw() {}
/* 025A36D4 */
void dDlst_2Dt_Sp_draw() {}
/* 025A36D8 */
static void drawFadeOut() {}
/* 025A36DC */
static void drawFadeIn() {}

/* WWHD glue
ext 0273AD10 void* dOvlpFd4_alloc(u32)
ext 0252CCBC void dDlst_base_construct(dDlst_base_c*)
ext 0252CCFC void dDlst_base_destroy(dDlst_base_c*, s32)
ext 028F521C void dOvlpFd4_clear(void*, u32)
ext 025DD5F0 void fpcBs_construct(base_process_class*)
ext 025DD630 void fpcBs_destruct(base_process_class*, s32)
ext 025200D4 dOvlpFd4_play_c* dOvlpFd4_getPlay()
ext 02055B64 u32 dOvlpFd4_calcTimer(be<s16>*)
ext 025DBDDC void fopOvlpM_Done(dOvlpFd4_c*)
ext 025DBDC4 u32 fopOvlpM_IsOutReq(dOvlpFd4_c*)
ext 025DBD74 void fopOvlpM_SceneIsStart()
ext 025DBD24 void fopOvlpM_SceneIsStop()
ext 02728D6C u32 dOvlpFd4_register(dOvlpFd4_registry_c*, dOvlpFd4_c*)
ext 02728EE0 void dOvlpFd4_unregister(dOvlpFd4_registry_c*)
ext 027F84A4 void dOvlpFd4_capture(dOvlpFd4_renderer_c*, s32, u32)
ext 027F88C8 void dOvlpFd4_release(dOvlpFd4_renderer_c*, s32)
ext 0274D690 void dOvlpFd4_setSize(dOvlpFd4_size_c*)
ext 0274F964 void dOvlpFd4_setTarget(void*, dOvlpFd4_size_c*)
ext 027F81A4 u32 dOvlpFd4_getImage(dOvlpFd4_renderer_c*, s32)
ext 027BE114 void dOvlpFd4_initTexture(dOvlpFd4_texture_c*, u32)
ext 027F29D4 dOvlpFd4_output_c* dOvlpFd4_getOutput(u32)
ext 0272CF9C u32 dOvlpFd4_blur(dOvlpFd4_blur_c*, dOvlpFd4_texture_c*, dOvlpFd4_scale_c*, u32, f32, f32)
ext 027BE2B0 void dOvlpFd4_destroyTexture(dOvlpFd4_texture_c*, s32)
ext 028F026C void dOvlpFd4_registerStatic(u32)
ext 028ED6F8 void dOvlpFd4_initFlag(u32)
ext 028EAB2C void dOvlpFd4_initByte(u32)
data l_spriteVtable 100519C0
data l_effectVtable 100519D8
data l_snapshotVtable 100519A8
data l_taskVtable 10051960
data l_outputRegistry 104B45C0
data l_floatRegistration 101EA3AC
data l_flagRegistration 101EA3B8
data l_byteRegistration 101EA3C4
data l_staticFlag 1047B140
data l_staticByte 1047B141
data l_renderer 101F9968
data l_registry 101F86B4
data l_blur 101F8710
data l_zero 10051974
data l_eight 10051978
data l_one 10051988
data l_minusOne 1005198C
data l_blurStrength 10051990
data l_staticWords 1047B144
data l_staticFloats 1047B138
data l_staticInitialX 100519A0
data l_staticInitialY 100519A4
*/
