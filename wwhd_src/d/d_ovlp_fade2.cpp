/* Second scene fade and HD snapshot rendering. Verified against WWHD. */
#include "d/d_ovlp_fade2.h"
// HD: singleton storage and live debug adjustments are BSS.
static be<u8>& l_blur = *gabi::at<be<u8>>(0x101f4825);
static be<u8>& l_monotone = *gabi::at<be<u8>>(0x101f4829);
static gptr<void>& l_textureManager = *gabi::at<gptr<void>>(0x101f9968);
static gptr<void>& l_fadeManager = *gabi::at<gptr<void>>(0x101f86b4);
static gptr<void>& l_renderer = *gabi::at<gptr<void>>(0x101f8710);
static be<f32>& l_zOffset = *gabi::at<be<f32>>(0x1047b610);
static be<f32>& l_scaleTarget = *gabi::at<be<f32>>(0x1047b614);
static be<f32>& l_scaleStep = *gabi::at<be<f32>>(0x1047b618);
static be<f32>& l_scaleInStep = *gabi::at<be<f32>>(0x1047b61c);
static be<s16>& l_rotationStep = *gabi::at<be<s16>>(0x1047b688);
static be<s16>& l_fadeDelay = *gabi::at<be<s16>>(0x1047b68a);
static void* const l_handleKey = gabi::at<void>(0x104B45C0);
// HD: preserve the GHS member-pointer store order.
static void phase(dOvlpFd2_c* s, fd2::Phase target) {
    s->mProc.i = -1;
    s->mProc.d = 0;
    s->mProc.f = target;
}
/* 025A1BD0 */
void dOvlpFd2_c::draw() {

    if (mHasSnap) {
        fd2GameInfoView* play = fd2_getGameInfo();
        if (!play->mWindowNum) {
            f32 z = l_zOffset + (-420.0f);
            fd2_mtxTrans(mDoMtx_stack_c::get(), 0.0f, 0.0f, z);
            mDoMtx_stack_c::transM(0.5f, 0.5f, 0.0f);
            mDoMtx_ZrotM(mDoMtx_stack_c::get(), rotZ);
            f32 y = -gabi::fmsubs(scale, 0.5f, 1.0f);
            mDoMtx_stack_c::scaleM(1.0f, y, 1.0f);
            mDoMtx_ZrotM(mDoMtx_stack_c::get(), s16(-s32(rotZ)));
            mDoMtx_stack_c::transM(-0.5f, -0.5f, 0.0f);
            PSMTXCopy(mDoMtx_stack_c::get(), &dOvlpFd2_dlst.mtx);
            play = fd2_getGameInfo();
            fd2_setDrawList(play->mDrawList, &play->mDrawList[0x264], &play->mDrawList[0x268],
                            &dOvlpFd2_dlst);
        }
    }
    l_blur = 0;
}
/* 025A1D10 */
u32 dOvlpFd2_Draw(dOvlpFd2_c* s) {
    s->draw();
    return 1;
}
/* 025A1D34 */
u32 dOvlpFd2_Execute(dOvlpFd2_c* s) {
    (s->*s->mProc)();
    return 1;
}
/* 025A1DA0 */
u32 dOvlpFd2_Delete(dOvlpFd2_c* s) {
    if (s) {
        fd2_snapshotStateDelete(&s->mHdSnapshotState, 2);
        fd2_drawListDelete(&s->dOvlpFd2_dlst, 0);
        fd2_drawListDelete(&s->dSnap_dlst, 0);
        fd2_baseDelete(s, 0);
    }
    fd2_releaseTexture(l_textureManager, 4);
    fd2_unlinkFade(l_fadeManager);
    return 1;
}
// HD: allocating constructor entry returns this in r3, including a null allocation result.
/* 025A1E1C */
dOvlpFd2_c* dOvlpFd2_constructor(dOvlpFd2_c* s) {
    if (!s)
        s = (dOvlpFd2_c*)operator_new(sizeof(dOvlpFd2_c));
    if (s) {
        fd2_baseConstruct(s);
        s->base.__vtbl = 0x100518C4;
        fd2_drawListConstruct(&s->dSnap_dlst);
        s->dSnap_dlst.mVtable = 0x10051940;
        fd2_drawListConstruct(&s->dOvlpFd2_dlst);
        s->dOvlpFd2_dlst.mVtable = 0x10051900;
        fd2_snapshotStateConstruct(&s->mHdSnapshotState);
        fd2_rendererStateConstruct(&s->mHdRendererState);
        phase(s, fd2::FIRST);
        fd2GameInfoView* p = fd2_getGameInfo();
        p->m2dShow = 0;
        s->mTimer = 2;
        fd2_linkFade(l_fadeManager, s);
        fd2_mtxIdentity(&s->dOvlpFd2_dlst.mtx);
    }
    return s;
}
/* 025A1EF4 */
u32 dOvlpFd2_Create(dOvlpFd2_c* s) {
    if (s)
        dOvlpFd2_constructor(s);
    return cPhs_COMPLEATE_e;
}
/* 025A1F20 */
void dOvlpFd2_c::execFirstSnap() {

    if (mHasSnap) {
        if (!fd2_calcTimer(&mTimer)) {
            phase(this, fd2::OUT);
            fopOvlpM_Done(this);
            mTimer = -12;
        }
        fd2GameInfoView* p = fd2_getGameInfo();
        p->mWindowNum = 0;
        if (l_monotone)
            fd2_offMonotone();
    }
}
/* 025A1FB4 */
void dOvlpFd2_c::execFadeOut() {

    fd2GameInfoView* p = fd2_getGameInfo();
    p->mWindowNum = 0;
    fd2_chaseAngleS(&field_0x112, 2000, 100);
    s32 a = field_0x110, b = field_0x112;
    u32 cross = (u32(a + 0x4000) & 0x8000) | 0x4000;
    s32 sum = a + b;
    s32 diff = s16(s32(cross) - b - sum);
    field_0x110 = s16(sum);
    s32 timer = mTimer;
    bool transitioned = false;
    if (diff * b < 0 && timer == 0) {
        if (fopOvlpM_IsOutReq(this)) {
            fopOvlpM_SceneIsStart();
            phase(this, fd2::NEXT);
            mTimer = 15;
            field_0x110 = -0x4000;
            fd2_calcTimer(&mTimer);
            transitioned = true;
        } else
            timer = mTimer;
    }
    if (!transitioned) {
        if (timer < 0) {
            s8 next = s8(timer + 1);
            mTimer = next;
            if (!next) {
                fd2_startFadeOut(16);
                mTimer = u8(s32(l_fadeDelay) + 20);
            }
        } else
            fd2_calcTimer(&mTimer);
    }
    s32 step = s32(l_rotationStep) + 0x800;
    rotZ = s16(s32(rotZ) + step);
    f32 f1 = l_scaleTarget + 1.0f, f3 = l_scaleStep + 0.05f;
    fd2_addCalc2(&scale, f1, 1.0f, f3);
}
/* 025A2150 */
void dOvlpFd2_c::execNextSnap() {

    if (!fd2_calcTimer(&mTimer) && !fd2_startFadeIn(l_fadeManager, 16, 1)) {
        mHasSnap = 0;
        field_0x110 = s16(s32(field_0x110) + s32(field_0x112));
        fd2GameInfoView* p = fd2_getGameInfo();
        p->mWindowNum = 1;
        p = fd2_getGameInfo();
        p->m2dShow = 0;
        phase(this, fd2::IN);
    }
}
/* 025A21F0 */
void dOvlpFd2_c::execFadeIn() {

    s32 step = s32(l_rotationStep) + 0x800;
    rotZ = s16(s32(rotZ) - step);
    f32 f2 = l_scaleInStep + 0.03f;
    fd2_addCalc0(&scale, 1.0f, f2);
    if (scale < 0.001f) {
        if (!field_0x11e) {
            fopOvlpM_SceneIsStart();
            field_0x11e = 1;
        } else {
            fopOvlpM_Done(this);
            fd2GameInfoView* p = fd2_getGameInfo();
            p->mWindowNum = 1;
            p = fd2_getGameInfo();
            p->m2dShow = 1;
        }
    } else {
        fd2GameInfoView* p = fd2_getGameInfo();
        p->mWindowNum = 0;
        fopOvlpM_SceneIsStop();
    }
}
/* 025A22D4 */
void dOvlpFd2_copyMtx(Mtx34* dst, const Mtx34* src) {
    f32 a[12];
    for (u32 i = 0; i < 12; ++i)
        a[i] = ((const be<f32>*)src)[i];
    for (u32 i = 0; i < 12; ++i)
        ((be<f32>*)dst)[i] = a[i];
}
// HD: captures and renders a snapshot through the HD texture manager.
/* 025A2374 */
void dOvlpFd2_c::captureSnapshot(fd2Snapshot* snap) {

    void* manager = l_textureManager;
    if (!mHasSnap) {
        fd2_releaseTexture(manager, 4);
        fd2_storeSnapshot(manager, 4, snap);
        mHasSnap = 1;
    }
    void* renderer = l_renderer;
    fd2_setSurface(snap->mSurface);
    void* r3 = snap->mTarget;
    void* r4 = snap->mSurface;
    fd2_setTargetSurface(r3, r4);
    void* texture = fd2_getTexture(manager, 4);
    gabi::Local<fd2TextureView> view;
    fd2_textureViewConstruct(view.get(), texture);
    gabi::Local<Mtx34> mtx;
    dOvlpFd2_copyMtx(mtx.get(), &dOvlpFd2_dlst.mtx);
    fd2Handle* ref = fd2_getHandle(l_handleKey);
    u32 old = ref->mValue;
    u32 handle = fd2_renderSnapshot(renderer, view.get(), mtx.get(), old);
    ref = fd2_getHandle(l_handleKey);
    ref->mValue = handle;
    ref->mFlags = 0;
    fd2_textureViewDelete(view.get(), 2);
}
// HD: compiler-generated header statics.
/* 025A2474 */
void __sinit_d_ovlp_fade2_cpp() {
    sinit_header_statics_z(0x1047B100, 0x101EA2EC, 0x1047B10C);
}
/* 025A2508 */
u32 dOvlpFd2_IsDelete(dOvlpFd2_c* s) {
    return 1;
}
/* 025A2510 */
void dOvlpFd2_empty(dOvlpFd2_c* s) {}
/* WWHD glue
ext 025200D4 fd2GameInfoView* fd2_getGameInfo()
ext 028E93CC void fd2_mtxTrans(Mtx34* mtx, f32 x, f32 y, f32 z)
ext 0252CDC0 void fd2_setDrawList(void* list, void* head, void* tail, dOvlpFd2_dlst_c* entry)
ext 027B9694 void fd2_snapshotStateDelete(void* state, s32 flags)
ext 0252CCFC void fd2_drawListDelete(void* entry, s32 flags)
ext 025DD630 void fd2_baseDelete(dOvlpFd2_c* self, s32 flags)
ext 027F88C8 void fd2_releaseTexture(void* manager, u32 slot)
ext 02728EE0 void fd2_unlinkFade(void* manager)
ext 025DD5F0 void fd2_baseConstruct(dOvlpFd2_c* self)
ext 0252CCBC void fd2_drawListConstruct(void* entry)
ext 027B95B4 void fd2_snapshotStateConstruct(void* state)
ext 027B6CB8 void fd2_rendererStateConstruct(void* state)
ext 02728E64 void fd2_linkFade(void* manager, dOvlpFd2_c* self)
ext 028E9098 void fd2_mtxIdentity(Mtx34* mtx)
ext 0220CCB4 u32 fd2_calcTimer(be<s8>* timer)
ext 025DBDDC void fopOvlpM_Done(dOvlpFd2_c* self)
ext 025F0830 void fd2_offMonotone()
ext 0200F8D0 void fd2_chaseAngleS(be<s16>* angle, s32 target, s32 step)
ext 025DBDC4 u32 fopOvlpM_IsOutReq(dOvlpFd2_c* self)
ext 025DBD74 void fopOvlpM_SceneIsStart()
ext 025F05E8 void fd2_startFadeOut(u32 time)
ext 0200ED84 void fd2_addCalc2(be<f32>* value, f32 target, f32 rate, f32 step)
ext 02728CB8 u32 fd2_startFadeIn(void* manager, u32 time, u32 flag)
ext 0200EDC8 void fd2_addCalc0(be<f32>* value, f32 rate, f32 step)
ext 025DBD24 void fopOvlpM_SceneIsStop()
ext 027F84A4 void fd2_storeSnapshot(void* manager, u32 slot, fd2Snapshot* snapshot)
ext 0274D690 void fd2_setSurface(void* surface)
ext 0274F964 void fd2_setTargetSurface(void* target, void* surface)
ext 027F81A4 void* fd2_getTexture(void* manager, u32 slot)
ext 027BE114 void fd2_textureViewConstruct(fd2TextureView* view, void* texture)
ext 027F29D4 fd2Handle* fd2_getHandle(void* key)
ext 0272CAE0 u32 fd2_renderSnapshot(void* renderer, fd2TextureView* view, Mtx34* mtx, u32 old)
ext 027BE2B0 void fd2_textureViewDelete(fd2TextureView* view, s32 flags)
*/
