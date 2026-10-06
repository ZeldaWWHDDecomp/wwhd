/**
 * d_camera_proc.cpp (WWHD)
 * Camera process (camera_process_class): phases, execute, delete, view setup; dCamera_c
 * construction/destruction helpers.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_camera.cpp) to the WWHD layout and code, verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * camera_process_class (HD): view_class fields used here: near 0xCC, far 0xD0, fovy 0xD4,
 * aspect 0xD8, lookat eye 0xDC / center 0xE8 / up 0xF4, bank 0x100, view matrix 0x144,
 * no-trans view matrix 0x1E4; prm1..3 bytes 0x230..0x232; dCamera_c body at 0x248.
 */
#include "d/d_camera.h"
#include <bit>

static inline s32 fopCamM_GetParam(camera_class* cam) { return gabi::call<s32>(0x025DA64C, cam); }
static inline u32 cam_ea(camera_class* cam) { return gabi::ea(cam); }
/* sead render layers (HD): an array of layer pointers at +0x1024 (count at +0x1020) of the object
 * at *0x101F95D0; layer +0x48 is the camera, +0x4C the projection */
static void setRenderLayerCamera(u32 cameraPtr, u32 projectionPtr) {
    u32 mgr = gabi::load<u32>(0x101F95D0);
    u32 arr = gabi::load<u32>(mgr + 0x1024);
    if (gabi::load<u32>(mgr + 0x1020) > 1) arr += 4;
    gabi::store<u32>(gabi::load<u32>(arr) + 0x48, cameraPtr);
    arr = gabi::load<u32>(mgr + 0x1024);
    if (gabi::load<u32>(mgr + 0x1020) > 1) arr += 4;
    gabi::store<u32>(gabi::load<u32>(arr) + 0x4C, projectionPtr);
    gabi::store<u32>(gabi::load<u32>(gabi::load<u32>(mgr + 0x1024)) + 0x48, cameraPtr);
    gabi::store<u32>(gabi::load<u32>(gabi::load<u32>(mgr + 0x1024)) + 0x4C, projectionPtr);
}

/* 024F8068. HD: the window/viewport/view are stored in the play object (0x5F9C..0x5FA4) */
static void view_setup(camera_class* i_this) {
    WWHD_FUNC(0x024F8068, void, i_this);
    u32 cam = cam_ea(i_this);
    s32 camId = fopCamM_GetParam(i_this);
    s32 winId = gabi::load<s8>(dComIfGp_ea() + camId * 0x34 + 0x5AFC);
    u32 window = dComIfGp_ea() + 0x5ACC + winId * 0x2C; /* dComIfGp_getWindow */
    gabi::call(0x025F1EAC, cam + 0x144, cam + 0xDC, cam + 0xE8, cam + 0xF4, (s16)gabi::load<s16>(cam + 0x100)); /* mDoMtx_lookAt */
    PSMTXCopy(gabi::at<Mtx34>(cam + 0x144), gabi::at<Mtx34>(cam + 0x1E4));
    gabi::store<f32>(cam + 0x1F0, 0.0f);
    gabi::store<f32>(cam + 0x200, 0.0f);
    gabi::store<f32>(cam + 0x210, 0.0f);
    gabi::store<u32>(dComIfGp_ea() + 0x5F9C, window); /* dComIfGd_setWindow */
    gabi::store<u32>(dComIfGp_ea() + 0x5FA0, window); /* dComIfGd_setViewport */
    gabi::store<u32>(dComIfGp_ea() + 0x5FA4, cam);    /* dComIfGd_setView */
    if (gabi::load<u8>(dComIfGp_ea() + 0x5BB3) != 0) { /* dComIfGp_getScopeMesgStatus() */
        gabi::call(0x025F11AC, gabi::load<f32>(cam + 0xD4), gabi::load<f32>(cam + 0xD8), gabi::load<f32>(cam + 0xCC),
                   gabi::load<f32>(cam + 0xD0)); /* mDoLib_clipper::setup(fovy, aspect, near, far) */
    } else {
        u32 stage = dComIfGp_ea() + 0x5150;
        u32 info = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(stage) + 0x15C), stage); /* getStagInfo() */
        f32 far = (f32)gabi::load<u16>(info + 0x12); /* dStage_stagInfo_GetCullPoint */
        gabi::call(0x025F11AC, gabi::load<f32>(cam + 0xD4), gabi::load<f32>(cam + 0xD8), gabi::load<f32>(cam + 0xCC), far);
    }
}
VERIFY(0x024F8068, view_setup);

/* 024FF8A0. HD: when the trim is suppressed (play byte 0x5292, or for the cinema-scope size the
 * global 101D6010 == 1) the bars shrink away instead */
void dCamera_c::CalcTrimSize() {
    WWHD_FUNC(0x024FF8A0, void, this);
    u32 size = mTrimSize;
    f32 h;
    switch (size) {
    case 0:
        break;
    case 1:
        if (gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0) break;
        h = mTrimHeight;
        mTrimHeight = gabi::fmadds(gabi::load<f32>(ea() + 0x7A4) - h, 0.25f, h); /* VistaTrimHeight */
        return;
    case 2:
        if (gabi::load<s32>(0x101D6010) == 1) break;
        if (gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0) break;
        h = mTrimHeight;
        mTrimHeight = gabi::fmadds(gabi::load<f32>(ea() + 0x7A8) - h, 0.25f, h); /* CinemaScopeTrimHeight */
        return;
    case 3:
        if (gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0) break;
        gabi::store<u32>(ea() + 0x5FC, gabi::load<u32>(ea() + 0x7A8));
        return;
    case 4:
        mTrimHeight = 0.0f;
        return;
    default:
        return;
    }
    h = mTrimHeight;
    mTrimHeight = gabi::fnmsubs(h, 0.25f, h); /* h += (0 - h) * 0.25 */
}
VERIFY(0x024FF8A0, &dCamera_c::CalcTrimSize);

/* 024FFA3C. HD: fixed 16:9 aspect; auto focus is switched on every frame */
static BOOL camera_execute(camera_class* i_this) {
    WWHD_FUNC(0x024FFA3C, BOOL, i_this);
    u32 cam = cam_ea(i_this);
    dCamera_c* body = gabi::at<dCamera_c>(cam + CAMERA_BODY);
    /* preparation() */
    s32 camId = fopCamM_GetParam(i_this);
    s32 winId = gabi::load<s8>(dComIfGp_ea() + camId * 0x34 + 0x5AFC);
    u32 vp = dComIfGp_ea() + winId * 0x2C;
    u32 wBits = gabi::load<u32>(vp + 0x5AD4);
    u32 hBits = gabi::load<u32>(vp + 0x5AD8);
    gabi::store<u32>(gabi::ea(&body->mWindowWidth), wBits); /* SetWindow */
    gabi::store<f32>(cam + 0xD8, 1.7777778f);             /* fopCamM_SetAspect */
    gabi::store<u32>(gabi::ea(&body->mWindowHeight), hBits);
    body->mWindowAspectRatio = gabi::load<f32>(vp + 0x5AD4) / gabi::load<f32>(vp + 0x5AD8);
    u32 stat = dComIfGp_ea() + camId * 0x34 + 0x5B00;
    gabi::store<u32>(stat, gabi::load<u32>(stat) & ~0x23u); /* dComIfGp_offCameraAttentionStatus(0x23) */

    u32 demo = gabi::load<u32>(0x101D5FFC);
    if (demo == 0) {
        JUT_ASSERT_fail(STR(0x1004AB04), 0x23E, STR(0x1004AAF4));
        demo = gabi::load<u32>(0x101D5FFC);
    }
    if (gabi::call<u32>(0x025283F8, demo) != 0) { /* dComIfGp_demo_getCamera() */
        gabi::call(0x024F8C54, body, i_this); /* ResetView */
    }
    gabi::store<u8>(0x101F4828, 1); /* mDoGph_gInf_c::onAutoForcus() */
    if (body->Active() && !body->Pause()) {
        gabi::call(0x024FE3E8, body); /* Run */
    } else {
        gabi::call(0x024FF6C0, body); /* NotRun */
    }
    body->CalcTrimSize();
    gabi::call(0x024F8430, i_this); /* store */
    view_setup(i_this);
    return TRUE;
}
VERIFY(0x024FFA3C, camera_execute);

/* 0250043C. HD: no player-2 id */
static s32 init_phase1(camera_class* i_this) {
    WWHD_FUNC(0x0250043C, s32, i_this);
    u32 cam = cam_ea(i_this);
    s32 camId = fopCamM_GetParam(i_this);
    u32 off = camId * 0x34;
    gabi::store<u32>(dComIfGp_ea() + off + 0x5AF8, cam);                               /* dComIfGp_setCamera */
    gabi::store<u8>(cam + 0x230, gabi::load<u8>(dComIfGp_ea() + off + 0x5AFC));      /* prm1: window id */
    gabi::store<u8>(cam + 0x231, gabi::load<u8>(dComIfGp_ea() + off + 0x5AFD));      /* prm2: player 1 id */
    gabi::store<u8>(cam + 0x232, gabi::load<u8>(dComIfGp_ea() + off + 0x5AFE));      /* prm3: player 2 id */
    gabi::Local<cXyz> pos;
    pos->copy(*gabi::at<cXyz>(0x1004AB24)); /* {1e7, 1e7, 1e7} */
    gabi::call(0x025E1B44, pos.get(), 0x104B45F8u, camId); /* mDoAud_getCameraInfo(&pos, j3dSys.getViewMtx(), camId) */
    gabi::store<u8>(dComIfGp_ea() + 0x5AC9, 0); /* dComIfGp_setWindowNum(0) */
    gabi::store<u8>(0x101F4828, 1);             /* mDoGph_gInf_c::onAutoForcus() */
    return cPhs_NEXT_e;
}
VERIFY(0x0250043C, init_phase1);

/* 02500508: compiler-generated constructor of the BG block (HD 0x11C: two dBgS_CamGndChk and the
 * angles/globes that follow them) */
static u8* dCamera_BG_ct(u8* p) {
    WWHD_FUNC(0x02500508, u8*, p);
    if (p == nullptr) {
        p = (u8*)operator_new(0x11C);
        if (p == nullptr) return p;
    }
    u32 o = gabi::ea(p);
    gabi::call(0x02008E0C, o + 4); /* cBgS_GndChk() */
    gabi::store<u8>(o + 0x4E, 0);
    gabi::store<u8>(o + 0x49, 1);
    gabi::store<u8>(o + 0x48, 0);
    gabi::store<u32>(o + 0x08, o + 0x50);
    gabi::store<u8>(o + 0x4B, 0);
    gabi::store<u8>(o + 0x4C, 0);
    gabi::store<u32>(o + 0x54, 1);
    gabi::store<u32>(o + 0x14, 0x1004A53C);
    gabi::store<u32>(o + 0x50, 0x1004A55C);
    gabi::store<u32>(o + 0x04, o + 0x44);
    gabi::store<u32>(o + 0x24, 0x1004A54C);
    gabi::store<u8>(o + 0x4D, 0);
    gabi::store<u32>(o + 0x44, 0x1004A56C);
    gabi::store<u8>(o + 0x4A, 0);
    gabi::call(0x02008E0C, o + 0x60);
    gabi::store<u32>(o + 0xB0, 1);
    gabi::store<u32>(o + 0x64, o + 0xAC);
    gabi::store<u32>(o + 0xAC, 0x1004A55C);
    gabi::store<u32>(o + 0x60, o + 0xA0);
    gabi::store<u32>(o + 0x80, 0x1004A54C);
    gabi::store<u32>(o + 0xA0, 0x1004A56C);
    gabi::store<u8>(o + 0xAA, 0);
    gabi::store<u8>(o + 0xA8, 0);
    gabi::store<u8>(o + 0xA7, 0);
    gabi::store<u8>(o + 0xA9, 0);
    gabi::store<u8>(o + 0xA4, 0);
    gabi::store<u8>(o + 0xA6, 0);
    gabi::store<u32>(o + 0x70, 0x1004A53C);
    gabi::store<u8>(o + 0xA5, 1);
    gabi::call(0x020065FC, o + 0xDC); /* cSAngle() */
    gabi::call(0x020065FC, o + 0xDE);
    gabi::call(0x02007100, o + 0xE4); /* cSGlobe() */
    gabi::call(0x02007100, o + 0xEC);
    return p;
}
VERIFY(0x02500508, dCamera_BG_ct);

/* 02501F98. HD: fixed 16:9 aspect; the body's sead camera and projection are attached to the
 * render layers */
static s32 init_phase2(camera_class* i_this) {
    WWHD_FUNC(0x02501F98, s32, i_this);
    u32 cam = cam_ea(i_this);
    u32 body = cam + CAMERA_BODY;
    fopCamM_GetParam(i_this);
    s32 camId = fopCamM_GetParam(i_this);
    s32 playerId = gabi::load<s8>(dComIfGp_ea() + camId * 0x34 + 0x5AFD); /* dComIfGp_getCameraPlayer1ID */
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + playerId * 8 + 0x5B2C));
    if (player == nullptr) return cPhs_INIT_e;
    fopAcM_setStageLayer(player);
    gabi::store<u8>(dComIfGp_ea() + 0x5AC9, 1); /* dComIfGp_setWindowNum(1) */
    if (body != 0) gabi::call(0x02501B8C, body, i_this); /* new (body) dCamera_c(i_this) */
    f32 farPlane = 160000.0f;
    u32 stage = dComIfGp_ea() + 0x5150;
    if (gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(stage) + 0x15C), stage) != 0) {
        stage = dComIfGp_ea() + 0x5150;
        gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(stage) + 0x15C), stage);
        u32 info = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(stage) + 0x15C), stage);
        farPlane = gabi::load<f32>(info + 4); /* mFarPlane */
    }
    dComIfGp_ea(); /* get_window(camera_id): unused in HD */
    dComIfGp_ea();
    gabi::store<f32>(cam + 0xD0, farPlane);   /* fopCamM_SetFar */
    gabi::store<f32>(cam + 0xD4, 30.0f);      /* fopCamM_SetFovy */
    gabi::store<f32>(cam + 0xCC, 1.0f);       /* fopCamM_SetNear */
    gabi::store<f32>(cam + 0xD8, 1.7777778f); /* fopCamM_SetAspect */
    u32 pos = gabi::ea(&player->current.pos);
    u32 py = gabi::load<u32>(pos + 4);
    u32 px = gabi::load<u32>(pos);
    u32 pz = gabi::load<u32>(pos + 8);
    gabi::store<u32>(cam + 0xE8, px); /* fopCamM_SetCenter */
    gabi::store<u32>(cam + 0xEC, py);
    gabi::store<s16>(cam + 0x100, 0); /* fopCamM_SetBank */
    gabi::store<u32>(cam + 0xF0, pz);
    gabi::call(0x024F8430, i_this); /* store */
    view_setup(i_this);
    setRenderLayerCamera(body + 0x614, body + 0x66C);
    return cPhs_NEXT_e;
}
VERIFY(0x02501F98, init_phase2);

/* 025021B8 dCamera_c::~dCamera_c (HD: destroys the sead camera/projection, setup and param) */
static void dCamera_dt(dCamera_c* i_this, s32 flags) {
    WWHD_FUNC(0x025021B8, void, i_this, flags);
    if (i_this == nullptr) return;
    u32 o = i_this->ea();
    gabi::store<u32>(0x101F3084, 0); /* fopAc_ac_c::setStopStatus(0) */
    gabi::call(0x024F7318, o + 0x8A4, 2); /* ~dCamParam_c */
    gabi::call(0x024F7858, o + 0x73C, 2); /* ~dCamSetup_c */
    gabi::call(0x02738A6C, o + 0x66C, 2); /* ~projection */
    gabi::call(0x0274CCB0, o + 0x614, 2); /* ~LookAtCamera */
    gabi::store<u32>(o + 0x300, 0x1004A52C);
    gabi::store<u32>(o + 0x2E0, 0x1004A50C);
    gabi::store<u32>(o + 0x30C, 0x1004A4EC);
    gabi::call(0x02008DAC, o + 0x2C0, 0); /* ~dBgS_CamGndChk (m5C) */
    gabi::store<u32>(o + 0x284, 0x1004A50C);
    gabi::store<u32>(o + 0x2A4, 0x1004A52C);
    gabi::store<u32>(o + 0x2B0, 0x1004A4EC);
    gabi::call(0x02008DAC, o + 0x264, 0); /* ~dBgS_CamGndChk (m00) */
    gabi::call(0x0252CCFC, o + 0x1B4, 0); /* ~dDlst_effectLine_c */
    if (flags & 1) operator_delete(i_this);
}
VERIFY(0x025021B8, dCamera_dt);

/* 02502288. HD: clears the play object's draw view and detaches the render layers */
static BOOL camera_delete(camera_class* i_this) {
    WWHD_FUNC(0x02502288, BOOL, i_this);
    u32 cam = cam_ea(i_this);
    s32 camId = fopCamM_GetParam(i_this);
    gabi::call(0x025021B8, cam + CAMERA_BODY, 2); /* body->~dCamera_c() */
    gabi::store<u32>(dComIfGp_ea() + camId * 0x34 + 0x5AF8, 0); /* dComIfGp_setCamera(camId, NULL) */
    gabi::store<u32>(dComIfGp_ea() + 0x5F9C, 0);
    gabi::store<u32>(dComIfGp_ea() + 0x5FA0, 0);
    gabi::store<u32>(dComIfGp_ea() + 0x5FA4, 0);
    setRenderLayerCamera(0, 0);
    gabi::call(0x027C1F70, gabi::load<u32>(0x101F95D0), 0);
    return TRUE;
}
VERIFY(0x02502288, camera_delete);

/* 025021A4 camera_create (matcher: unnamed). The phase handler (02525FE4, dComLbG_PhaseHandler)
 * runs l_method (101D55C0: init_phase1, init_phase2) on the process's phase request at +0x240 */
static s32 camera_create(u32 i_this) {
    WWHD_FUNC(0x025021A4, s32, i_this);
    return gabi::call<s32>(0x02525FE4, i_this + 0x240, 0x101D55C0, i_this);
}
VERIFY(0x025021A4, camera_create);

/* 02502364. HD-only: an entry of the play object's table at 0x5C38 selected by the indices at
 * 0x5C34 then 0x5C30 (1..10; the later one wins) */
static u32 dCamera_hdTableEntry() {
    WWHD_FUNC(0x02502364, u32);
    u32 base = dComIfGp_ea() + 0x5C30;
    u32 b = gabi::load<u32>(base + 4);
    u32 a = gabi::load<u32>(base);
    u32 r = 0;
    if (b - 1 < 10) r = gabi::load<u32>(base + 8 + (b - 1) * 4);
    if (a - 1 < 10) r = gabi::load<u32>(base + 8 + (a - 1) * 4);
    return r;
}
VERIFY(0x02502364, dCamera_hdTableEntry);

/* 025023C4. HD-only: the index at 0x5C34 or 0x5C30 when it is beyond the table (> 10) */
static s32 dCamera_hdTableIndex() {
    WWHD_FUNC(0x025023C4, s32);
    u32 base = dComIfGp_ea() + 0x5C30;
    s32 b = gabi::load<s32>(base + 4);
    s32 a = gabi::load<s32>(base);
    s32 r = 0;
    if (!((u32)(b - 1) < 10) && b > 0) r = b;
    if (!((u32)(a - 1) < 10) && a > 0) r = a;
    return r;
}
VERIFY(0x025023C4, dCamera_hdTableIndex);

/* 024FF6C0. HD: the telescope view is 1280x720 */
bool dCamera_c::NotRun() {
    WWHD_FUNC(0x024FF6C0, bool, this);
    mEventFlags = mEventFlags & ~0x90149C21u;
    checkGroundInfo();
    clrComStat(0x80);
    if (gabi::load<s32>(dComIfGp_ea() + 0x52E4) != 0 /* dComIfGp_evmng_cameraPlay() */ || chkFlag(0x20000000)) {
        s32 ev = mCamTypeEvent;
        if (mCurType != ev) {
            pushPos();
            mEventData.field_0x0c = (s32)mCurType;
            ev = mCamTypeEvent;
        }
        mCurType = ev;
        gabi::call<bool>(0x024FF164, this, (s32)camTypeStyle(ev, 3)); /* eventCamera */
        m07C = m07C + 1;
        m118 = m118 + 1;
        m11C = m11C + 1;
        m108 = m108 + 1;
    }
    if (gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0 /* dComIfGp_event_runCheck() */) clrComStat(0x48);
    setComStat(0x14);
    mEventFlags = mEventFlags & ~0x90080u;
    gabi::call<bool>(0x024F8D4C, &mForcusLine); /* mForcusLine.Off() */
    gabi::call<f32>(0x024FC108, this);           /* shakeCamera() */
    mPause = 0;
    if (getComStat(8)) {
        if (chkFlag(0x400000)) {
            setView(160.0f, 35.0f, 320.0f, 320.0f);
        } else {
            setView(0.0f, 0.0f, 1280.0f, 720.0f);
        }
    }
    return true;
}
VERIFY(0x024FF6C0, &dCamera_c::NotRun);

static inline void memzero(u32 p, u32 n) { gabi::call(0x028F521C, p, n); } /* value-initialisation of PODs */
static inline void cSAngle_ct_at(u32 p) { gabi::call(0x020065FC, p); }
static inline void cSGlobe_ct_at(u32 p) { gabi::call(0x02007100, p); }

/* 02501B8C dCamera_c::dCamera_c(camera_class*). HD: value-initialises its members and builds
 * the HD-only sead camera (identity matrix, eye (0,0,10), at 0, up +Y) and projection */
static dCamera_c* dCamera_ct(dCamera_c* i_this, camera_class* cam) {
    WWHD_FUNC(0x02501B8C, dCamera_c*, i_this, cam);
    if (i_this == nullptr) {
        i_this = (dCamera_c*)operator_new(0x8E0);
        if (i_this == nullptr) return i_this;
    }
    u32 o = i_this->ea();
    gabi::store<u32>(o + 0x0, 0);
    gabi::store<u8>(o + 0x4, 0);
    gabi::store<u8>(o + 0x5, 0);
    cSGlobe_ct_at(o + 0x8);
    cSAngle_ct_at(o + 0x34);
    gabi::store<f32>(o + 0x38, 0.0f);
    memzero(o + 0x3C, 0x28);
    cSGlobe_ct_at(o + 0x3C);
    cSAngle_ct_at(o + 0x5C);
    gabi::store<f32>(o + 0x64, 0.0f);
    gabi::store<u32>(o + 0x68, 0);
    cSAngle_ct_at(o + 0x6C);
    gabi::store<u32>(o + 0x80, 0);
    gabi::store<u32>(o + 0x7C, 0);
    memzero(o + 0x84, 0x20);
    cSAngle_ct_at(o + 0xA0);
    gabi::call(0x028F0020, o + 0xA4, 2, 0x20, 0x0251558Cu, 0); /* __construct_array(m0A4, 2, 0x20, PosSet ctor, NULL) */
    memzero(o + 0xE4, 8);
    memzero(o + 0xEC, 0x14);
    memzero(o + 0x100, 0x14);
    cSAngle_ct_at(o + 0x104);
    cSAngle_ct_at(o + 0x106);
    memzero(o + 0x114, 8);
    memzero(o + 0x11C, 4);
    gabi::store<u32>(o + 0x128, 0);
    gabi::store<u32>(o + 0x124, 0);
    gabi::store<u32>(o + 0x12C, 0);
    gabi::store<u32>(o + 0x120, 0);
    memzero(o + 0x130, 0xC);
    gabi::store<u32>(o + 0x144, 0);
    gabi::store<u32>(o + 0x13C, 0);
    gabi::store<u32>(o + 0x140, 0);
    cSAngle_ct_at(o + 0x148);
    gabi::store<f32>(o + 0x150, 0.0f);
    gabi::store<f32>(o + 0x154, 0.0f);
    gabi::store<f32>(o + 0x14C, 0.0f);
    memzero(o + 0x158, 0x5C);
    memzero(o + 0x1B4, 0x70);
    gabi::call(0x0252CD20, o + 0x1B4); /* dCamForcusLine() */
    memzero(o + 0x224, 6);
    cSAngle_ct_at(o + 0x226);
    cSAngle_ct_at(o + 0x228);
    memzero(o + 0x22C, 0x20);
    memzero(o + 0x24C, 0x14);
    memzero(o + 0x260, 0x11C);
    gabi::call(0x02500508, o + 0x260); /* BG block */
    memzero(o + 0x37C, 0x80);
    memzero(o + 0x3FC, 0x114);
    gabi::call(0x025C0BD0, o + 0x4CC); /* d2DBSplinePath() */
    gabi::store<u32>(o + 0x52C, 0);
    gabi::store<u8>(o + 0x530, 0);
    gabi::store<u32>(o + 0x510, 0);
    gabi::store<u32>(o + 0x524, 0);
    gabi::store<u32>(o + 0x514, 0);
    gabi::store<u8>(o + 0x518, 0);
    gabi::store<u32>(o + 0x51C, 0);
    gabi::store<u32>(o + 0x520, 0);
    gabi::store<u32>(o + 0x528, 0);
    memzero(o + 0x534, 0x14);
    memzero(o + 0x548, 0x7C);
    cSAngle_ct_at(o + 0x588);
    memzero(o + 0x5C4, 0x2C);
    memzero(o + 0x5F0, 8);
    memzero(o + 0x5F8, 8);
    gabi::store<u32>(o + 0x604, 0);
    gabi::store<f32>(o + 0x60C, 0.0f);
    gabi::store<f32>(o + 0x608, 0.0f);
    gabi::store<u32>(o + 0x644, 0x101450D0); /* sead::Camera vtable */
    gabi::store<u8>(o + 0x610, 0);
    gabi::store<u32>(o + 0x600, 0);
    for (u32 k = 0; k < 0x30; k += 4) gabi::store<u32>(o + 0x614 + k, gabi::load<u32>(0x104A041C + k)); /* Matrix34f::ident */
    gabi::store<u32>(o + 0x644, 0x101450F8); /* sead::LookAtCamera vtable */
    u32 pos = o + 0x648;
    if (pos == 0) pos = gabi::ea(operator_new(0xC));
    if (pos != 0) {
        gabi::store<f32>(pos + 4, 0.0f);
        gabi::store<f32>(pos + 0, 0.0f);
        gabi::store<f32>(pos + 8, 10.0f);
    }
    u32 at = o + 0x654;
    if (at == 0) at = gabi::ea(operator_new(0xC));
    if (at != 0) {
        gabi::store<f32>(at + 4, 0.0f);
        gabi::store<f32>(at + 0, 0.0f);
        gabi::store<f32>(at + 8, 0.0f);
    }
    u32 up = o + 0x660;
    if (up == 0) up = gabi::ea(operator_new(0xC));
    if (up != 0) {
        gabi::store<f32>(up + 8, 0.0f);
        gabi::store<f32>(up + 4, 1.0f);
        gabi::store<f32>(up + 0, 0.0f);
    }
    gabi::call(0x027389F8, o + 0x66C); /* sead projection */
    if (o + 0x730 == 0) operator_new(8);
    gabi::store<u8>(o + 0x738, 0);
    gabi::call(0x024F75E4, o + 0x73C); /* dCamSetup_c() */
    gabi::call(0x024F72BC, o + 0x8A4, 0); /* dCamParam_c(0) */
    memzero(o + 0x8B0, 0x24);
    memzero(o + 0x8D4, 0xC);
    /* initialize(cam, get_player_actor(cam), get_camera_id(cam), get_controller_id(cam)) */
    s32 camId = fopCamM_GetParam(cam);
    s32 playerId = gabi::load<s8>(dComIfGp_ea() + camId * 0x34 + 0x5AFD);
    u32 player = gabi::load<u32>(dComIfGp_ea() + playerId * 8 + 0x5B2C);
    s32 infoIdx = fopCamM_GetParam(cam);
    s32 camId2 = fopCamM_GetParam(cam);
    s32 padId = gabi::load<s8>(dComIfGp_ea() + 0x12A0 + camId2 * 0x34 + 0x485D);
    gabi::call(0x02500984, i_this, cam, player, infoIdx, padId);
    return i_this;
}
VERIFY(0x02501B8C, dCamera_ct);

/* is_current_map(name): HD compares sead::SafeString temporaries (literal vs the play object's
 * start stage name at 0x5134), calling each one's virtual 0x14 (terminate) first */
static bool is_current_map(u32 literal) {
    struct SafeString_l { be<u32> top; be<u32> vtable; };
    gabi::Local<SafeString_l> a, b;
    a->top = literal;
    a->vtable = 0x1004A4C4;
    u32 stage = dComIfGp_ea() + 0x5134;
    b->vtable = 0x1004A4C4;
    b->top = stage;
    gabi::call_ptr(gabi::load<u32>((u32)a->vtable + 0x14), a.get());
    gabi::call_ptr(gabi::load<u32>((u32)a->vtable + 0x14), a.get());
    u32 vt = b->vtable;
    u32 aTop = a->top;
    gabi::call_ptr(gabi::load<u32>(vt + 0x14), b.get());
    if (aTop == (u32)b->top) return true;
    u32 x = a->top, y = b->top;
    for (u32 n = 0x40001; n != 0; n--) {
        u32 cx = gabi::load<u8>(x), cy = gabi::load<u8>(y);
        if (cx != cy) return false;
        if (cx == 0) return true;
        x++;
        y++;
    }
    return false;
}

/* float -> u32 (GHS: values >= 2^31 are converted after subtracting 2^31) */
static u32 ftou(f32 f) {
    if (f < 2147483648.0f) return (u32)gabi::ftoi(f);
    return (u32)gabi::ftoi(f - 2147483648.0f) + 0x80000000u;
}

/* 02500984. HD: the 0x730/0x734 floats (HD-only) are initialised from 104A0920; the ship offset
 * is computed into m538 as on GameCube */
void dCamera_c::initialize(camera_class* camera, fopAc_ac_c* player, u32 infoIdx, u32 padId) {
    WWHD_FUNC(0x02500984, void, this, camera, player, infoIdx, padId);
    mpPlayerActor = player;
    mCameraID = (s32)infoIdx;
    mPadId = (s32)padId;
    mActive = 1;
    mpCamera = camera;
    mPause = 0;
    initMonitor();
    initPad();
    gabi::call(0x02500880, &mForcusLine); /* mForcusLine.Init() */
    mCamTypeField = GetCameraTypeFromCameraName(STR(0x1004AB58) /* "Field" */);
    mCamTypeEvent = GetCameraTypeFromCameraName(STR(0x1004AB60) /* "Event" */);
    mCamTypeWater = GetCameraTypeFromCameraName(STR(0x1004AB68) /* "Water" */);
    mCamTypeSubject = GetCameraTypeFromCameraName(STR(0x1004AB44) /* "Subject" */);
    mCamTypeBoat = GetCameraTypeFromCameraName(STR(0x1004AB90) /* "Boat" */);
    mCamTypeBoatBattle = GetCameraTypeFromCameraName(STR(0x1004ABC8) /* "BoatBattle" */);
    mCamTypeRestrict = GetCameraTypeFromCameraName(STR(0x1004ABD4) /* "Restrict" */);
    s32 keep = GetCameraTypeFromCameraName(STR(0x1004AB98) /* "Keep" */);
    mCurMode = 0;
    s32 field = mCamTypeField;
    mMapToolType = field;
    m258 = 0;
    m254 = 0;
    mCurType = field;
    m248[0] = 0x85E; /* JA_SE_MAN_CAMERA_NG */
    m14C = 0.0f;
    m144 = 1;
    m524 = 0xFF;
    m528 = nullptr;
    mEventFlags = 0;
    gabi::store<u8>(ea() + 0x518, 0); /* m514 */
    m248[1] = 0x828;  /* JA_SE_CAMERA_TO_MANUAL */
    m248[2] = 0x381B; /* JA_SE_ATM_PRT_SHIP_CREAK */
    mCamTypeKeep = keep;
    m148 = gabi::load<s16>(0x101FF354); /* cSAngle::_0 */
    m07C = 0;
    f32 rnd = cM_rndFX(32767.0f);
    m080 = ftou(rnd);
    m064 = 1.0f;
    m5F4 = 0.0f;
    mTrimSize = 0;
    mTrimHeight = 0.0f;
    mTrimTypeForce = -1;
    u32 stage = dComIfGp_ea() + 0x12A0 + 0x3EB0; /* &dComIfGp_getStage() */
    if (stage != 0) {
        u32 info = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(stage) + 0x15C), stage); /* getStagInfo() */
        if (info != 0) {
            u32 toolId = gabi::load<u8>(info + 8); /* mCameraMapToolID (u8: never -1) */
            s32 type = GetCameraTypeFromMapToolID((s32)toolId, -1);
            if (type != 0xFF && Chtyp(type)) mMapToolType = type;
        }
    }
    s16 style = camTypeStyle(mCurType, mCurMode);
    u32 grp = gabi::load<u32>(ea() + 0x2B4);
    m318 = -1000000000.0f;
    mBG_m5C.m58 = -1000000000.0f;
    m31D = 0;
    m31C = 0;
    mRoomNo = -1;
    mStageMapToolCameraIdx = 0xFF;
    mEventData.field_0x14 = -1;
    mBG_m00.m58 = -1000000000.0f;
    gabi::store<u32>(ea() + 0x2B4, (grp & ~1u) | 2); /* mBG.m00.m04.OffNormalGrp(); OnWaterGrp() */
    mLockOnActorId = fpcM_ERROR_PROCESS_ID_e;
    mEventData.field_0x18 = -1;
    mEventData.mStaffIdx = -1;
    mEventData.field_0x0c = -1;
    m0E8 = -1;
    mCurStyle = style;
    m32C.copy(*gabi::at<cXyz>(0x101FFBA8)); /* cXyz::Zero */
    m33C = nullptr;
    m320.copy(m32C);
    s16 zero = gabi::load<s16>(0x101FF354);
    m350 = 0;
    m364 = 0;
    mRoomMapToolCameraIdx = 0xFF;
    m33A = zero;
    m338 = zero;
    m368 = 0.0f;
    m354 = -1000000000.0f;
    gabi::store<u32>(ea() + 0x60C, gabi::load<u32>(ea() + 0x880)); /* m608 = mCamSetup.mBGChk.WallUpDistance() */

    m780 = is_current_map(0x1004AB54) /* "sea" */;
    m788 = is_current_map(0x1004ABA0) /* "kaze" */;
    m789 = is_current_map(0x1004AB70) /* "M_Dai" */;
    m78B = is_current_map(0x1004AB78) /* "kazeB" */;
    m784 = is_current_map(0x1004ABA8) /* "GanonK" */;
    m785 = is_current_map(0x1004ABB0) /* "GTower" */;
    m781 = is_current_map(0x1004AB80) /* "Asoko" */ || is_current_map(0x1004ABB8) /* "Abship" */ ||
           is_current_map(0x1004AB88) /* "PShip" */;
    m782 = is_current_map(0x1004ABC0) /* "Obshop" */;
    bool umikz = is_current_map(0x1004AB4C) /* "A_umikz" */;
    m534 = 0;
    m536 = 0x180;
    m783 = umikz;
    f32 ofs;
    if (m781 != 0) {
        m540 = 1.0f;
        m530 = 1;
        ofs = gabi::call<f32>(0x025269A8, &m534, &m536, 130.0f) * m540; /* daObjPirateship::getShipOffsetY */
    } else if (m782 != 0) {
        m530 = 2;
        m540 = 0.12f;
        ofs = gabi::call<f32>(0x025269A8, &m534, &m536, 130.0f) * m540;
    } else if (m783 != 0) {
        m540 = 1.0f;
        m530 = 3;
        ofs = gabi::call<f32>(0x025269A8, &m534, &m536, 130.0f) * m540;
    } else {
        m530 = 0;
        ofs = 0.0f;
        m540 = 0.0f;
    }
    s32 curStyle = mCurStyle;
    m538 = ofs;
    gabi::call(0x024F727C, ea() + 0x8A4, curStyle); /* mCamParam.Change(mCurStyle) */

    gabi::Local<cXyz> attnPos, center, xyz, eyeOfs;
    gabi::Local<cSAngle_l> dir, zeroAng;
    gabi::Local<cSGlobe_l> globe;
    attentionPos(attnPos, mpPlayerActor.get());
    f32 h = gabi::call<f32>(0x024F7464, ea() + 0x8A4, 0.0f); /* mCamParam.CenterHeight(0.0f) */
    attnPos->y = gabi::fadds_ppc(attnPos->y, h);
    directionOf(dir, mpPlayerActor.get());
    cSAngle_l* z = gabi::call<cSAngle_l*>(0x0200658C, zeroAng.get(), (s16)0);
    gabi::call(0x02007248, globe.get(), 0.0f, z, dir.get()); /* cSGlobe(0.0f, cSAngle(0), directionOf(player)) */
    gabi::call(0x020073AC, globe.get(), xyz.get());         /* .Xyz() */
    cXyz_pl(attnPos, center, xyz);
    mViewCache.mCenter.copy(*center);
    mCenter.copy(*center);
    directionOf(dir, mpPlayerActor.get());
    s16 inv = cSAngle_Inv(dir);
    gabi::call(0x02006FE4, &mViewCache.mDirection, 200.0f, (s16)0, inv); /* mViewCache.mDirection.Val(200.0f, 0, inv) */
    gabi::store<u32>(ea() + 0x8, gabi::load<u32>(ea() + 0x3C)); /* mDirection = mViewCache.mDirection */
    gabi::store<u32>(ea() + 0xC, gabi::load<u32>(ea() + 0x40));
    gabi::call(0x020073AC, &mViewCache.mDirection, xyz.get());
    cXyz_pl(&mViewCache.mCenter, eyeOfs, xyz);
    mViewCache.mEye.copy(*eyeOfs);
    mEye.copy(*eyeOfs);
    s16 u = cSAngle_Inv(&mDirection.mU);
    cSAngle_l* ay = gabi::call<cSAngle_l*>(0x0200658C, dir.get(), u);
    s16 angleY = *ay;
    mUp.x = 0.0f;
    mAngleY = angleY;
    mBank = zero;
    mUp.y = 1.0f;
    mViewCache.mBank = zero;
    mUp.z = 0.0f;
    f32 fovy = gabi::call<f32>(0x024F7478, ea() + 0x8A4, 0.0f); /* mCamParam.Fovy(0.0f) */
    mFovy = fovy;
    mViewCache.mFovy = fovy;
    mDMCSystem.field_0x0 = 0;
    gabi::store<u32>(ea() + 0x730, gabi::load<u32>(0x104A0920)); /* HD */
    gabi::store<u32>(ea() + 0x734, gabi::load<u32>(0x104A0924));
}
VERIFY(0x02500984, &dCamera_c::initialize);

/* SafeString temporaries compared with each other (HD inline sead::SafeString::isEqual) */
struct SafeString_l { be<u32> top; be<u32> vtable; };
static bool safeStringEqual(SafeString_l* a, SafeString_l* b, bool directTerminate, bool indirect) {
    if (directTerminate) {
        if (indirect) gabi::call_ptr(0x025155D4, a); /* a.terminate (inline copy, called through a register) */
        else gabi::call(0x025155D4, a);
    }
    gabi::call_ptr(gabi::load<u32>((u32)a->vtable + 0x14), a);
    u32 vt = b->vtable;
    u32 aTop = a->top;
    gabi::call_ptr(gabi::load<u32>(vt + 0x14), b);
    if (aTop == (u32)b->top) return true;
    u32 x = a->top, y = b->top;
    for (u32 n = 0x40001; n != 0; n--) {
        u32 cx = gabi::load<u8>(x), cy = gabi::load<u8>(y);
        if (cx != cy) return false;
        if (cx == 0) return true;
        x++;
        y++;
    }
    return false;
}

/* 024F8184. HD-only: near plane for the current stage: at least 3; on the "Hyrule" stage (or stage
 * types 5/7) it grows with the camera height (3 at y <= 1000 .. 10 at y >= 8000); 10 during "Demo07" */
static void dCamera_hdAdjustNear(dCamera_c* body, be<f32>* nearP, be<f32>* farP) {
    WWHD_FUNC(0x024F8184, void, body, nearP, farP);
    u32 stage = dComIfGp_ea() + 0x5150;
    u32 info = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(stage) + 0x15C), stage); /* getStagInfo() */
    u32 type = (gabi::load<u32>(info + 0xC) >> 16) & 7;
    gabi::Local<SafeString_l> a, b, c, d;
    u32 name = dComIfGp_ea() + 0x5134;
    a->vtable = 0x1004A4C4;
    a->top = name;
    bool big = true;
    if (type != 7 && type != 5) {
        b->vtable = 0x1004A4C4;
        b->top = 0x1004A754; /* "Hyrule" */
        big = safeStringEqual(a, b, true, true);
    }
    f32 n = *nearP;
    if (n < 3.0f) {
        n = 3.0f;
        *nearP = n;
    }
    if (big && n < 10.0f) {
        f32 y = body->mEye.y;
        if (y > 1000.0f) {
            f32 t = (y - 1000.0f) / 7000.0f;
            t = (t - 1.0f >= 0.0f) ? 1.0f : t;     /* fsel */
            f32 v = gabi::fmadds(7.0f, t, 3.0f);
            *nearP = (v - n >= 0.0f) ? v : n;       /* fsel */
        }
    }
    c->vtable = 0x1004A4C4;
    c->top = 0x1004A75C; /* "Demo07" */
    d->vtable = 0x1004A4C4;
    d->top = 0x1047E6B8; /* current demo name */
    if (safeStringEqual(c, d, true, true)) *nearP = 10.0f;
}
VERIFY(0x024F8184, dCamera_hdAdjustNear);

/* 024F8430. HD: during demo "Demo46" frame 878 the cut is smoothed (the jump is remembered in
 * function-local statics and decays by 0.96 per frame); the near/far planes come from
 * dCamera_hdAdjustNear unless the telescope/picto box view is active */
static void store(camera_class* i_this) {
    WWHD_FUNC(0x024F8430, void, i_this);
    u32 cam = cam_ea(i_this);
    dCamera_c* body = gabi::at<dCamera_c>(cam + CAMERA_BODY);
    s32 camId = fopCamM_GetParam(i_this);
    dComIfGp_ea();
    dComIfGp_ea();
    gabi::Local<cXyz> C, E, saveC, saveE, t;
    gabi::Local<cSAngle_l> bank, tmp;
    u32 c = gabi::ea(C.get()), e = gabi::ea(E.get());
    u32 upY = gabi::load<u32>(cam + 0xF8);
    gabi::store<u32>(c + 4, gabi::load<u32>(cam + 0xEC));
    gabi::store<u32>(e + 0, gabi::load<u32>(cam + 0xDC));
    u32 upX = gabi::load<u32>(cam + 0xF4);
    gabi::store<u32>(e + 4, gabi::load<u32>(cam + 0xE0));
    gabi::store<u32>(c + 0, gabi::load<u32>(cam + 0xE8));
    gabi::store<u32>(e + 8, gabi::load<u32>(cam + 0xE4));
    u32 upZ = gabi::load<u32>(cam + 0xFC);
    gabi::store<u32>(c + 8, gabi::load<u32>(cam + 0xF0));
    gabi::call(0x0200658C, bank.get(), (s16)gabi::load<s16>(cam + 0x100)); /* cSAngle bank(fopCamM_GetBank) */
    u32 demo = gabi::load<u32>(0x101D5FFC);
    u32 fovy = gabi::load<u32>(cam + 0xD4);
    if (demo == 0) {
        JUT_ASSERT_fail(STR(0x1004A794), 0x23E, STR(0x1004A784));
        demo = gabi::load<u32>(0x101D5FFC);
    }
    u32 dc = gabi::call<u32>(0x025283F8, demo); /* dComIfGp_demo_getCamera() */
    if (dc != 0) {
        saveC->copy(*C);
        saveE->copy(*E);
        u32 en = gabi::load<u8>(dc + 4);
        s16 bank0 = *bank;
        if (en & 0x40) { C->copy(*gabi::at<cXyz>(dc + 0x30)); en = gabi::load<u8>(dc + 4); } /* getTarget() */
        if (en & 0x10) { E->copy(*gabi::at<cXyz>(dc + 0x18)); en = gabi::load<u8>(dc + 4); } /* getTrans() */
        if (en & 0x20) { /* getUp() */
            upY = gabi::load<u32>(dc + 0x28);
            upX = gabi::load<u32>(dc + 0x24);
            upZ = gabi::load<u32>(dc + 0x2C);
        }
        if (en & 0x80) { /* bank = cAngle::d2s(-getRoll()) */
            s32 r = gabi::ftoi(-(gabi::load<f32>(dc + 0x3C) * 182.04445f));
            cSAngle_l* a = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)r);
            *bank = (s16)*a;
            en = gabi::load<u8>(dc + 4);
        }
        if (en & 4) fovy = gabi::load<u32>(dc + 0x10); /* getFovy() */
        gabi::Local<SafeString_l> a, b;
        a->vtable = 0x1004A4C4;
        b->vtable = 0x1004A4C4;
        a->top = 0x1004A77C; /* "Demo46" */
        b->top = 0x1047E6B8; /* current demo name */
        if (safeStringEqual(a, b, true, false)) {
            const u32 S1 = 0x1046EF90, S2 = 0x1046EF9C;
            if (gabi::load<u32>(0x1046EFA8) == 0) {
                gabi::at<cXyz>(S1)->copy(*gabi::at<cXyz>(0x101FFBA8));
                gabi::store<u32>(0x1046EFA8, 1);
            }
            if (gabi::load<u32>(0x1046EFAC) == 0) {
                gabi::at<cXyz>(S2)->copy(*gabi::at<cXyz>(0x101FFBA8));
                gabi::store<u32>(0x1046EFAC, 1);
            }
            s32 frame = gabi::load<s32>(0x101D600C);
            if (frame == 0x36E) {
                if (gabi::load<s32>(0x101D5530) != 0x36E) {
                    cXyz_mi(C, t, saveC);
                    gabi::at<cXyz>(S1)->copy(*t);
                    cXyz_mi(E, t, saveE);
                    gabi::at<cXyz>(S2)->copy(*t);
                    s16 bk = *bank;
                    gabi::store<s32>(0x101D5530, frame);
                    gabi::store<s16>(0x101D5534, (s16)(bk - bank0));
                    goto out;
                }
                gabi::call(0x028E8E64, S1, S1, 0.96f); /* PSVECScale */
                gabi::call(0x028E8E64, S2, S2, 0.96f);
                s32 db = gabi::ftoi((f32)gabi::load<s16>(0x101D5534) * 0.96f);
                gabi::store<s16>(0x101D5534, (s16)db);
                cXyz_pl(saveC, t, gabi::at<cXyz>(S1));
                C->copy(*t);
                cXyz_pl(saveE, t, gabi::at<cXyz>(S2));
                E->copy(*t);
                cSAngle_l* a2 = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)(bank0 + gabi::load<s16>(0x101D5534)));
                *bank = (s16)*a2;
            }
            gabi::store<s32>(0x101D5530, frame);
        }
    } else if (!(body->mEventFlags & 1)) {
        cXyz_pl(&body->mCenter, t, &body->mCenterShake); /* body->Center() */
        C->copy(*t);
        cXyz_pl(&body->mEye, t, &body->mEyeShake);       /* body->Eye() */
        E->copy(*t);
        upX = gabi::load<u32>(gabi::ea(&body->mUp.x));       /* body->Up() */
        gabi::store<u32>(gabi::ea(t.get()), upX);
        upY = gabi::load<u32>(gabi::ea(&body->mUp.y));
        gabi::store<u32>(gabi::ea(t.get()) + 4, upY);
        upZ = gabi::load<u32>(gabi::ea(&body->mUp.z));
        gabi::store<u32>(gabi::ea(t.get()) + 8, upZ);
        gabi::call(0x02006894, &body->mBank, tmp.get(), &body->mBankShake); /* body->Bank() */
        *bank = (s16)*tmp;
        fovy = std::bit_cast<u32>(body->mFovy + body->mFovYShake);    /* body->Fovy() */
    }
out:
    gabi::store<u32>(cam + 0xE8, gabi::load<u32>(c + 0)); /* fopCamM_SetCenter */
    gabi::store<u32>(cam + 0xEC, gabi::load<u32>(c + 4));
    gabi::store<u32>(cam + 0xF0, gabi::load<u32>(c + 8));
    gabi::store<u32>(cam + 0xDC, gabi::load<u32>(e + 0)); /* fopCamM_SetEye */
    gabi::store<u32>(cam + 0xE0, gabi::load<u32>(e + 4));
    gabi::store<u32>(cam + 0xE4, gabi::load<u32>(e + 8));
    gabi::store<u32>(cam + 0xF4, upX); /* fopCamM_SetUp */
    gabi::store<u32>(cam + 0xF8, upY);
    gabi::store<u32>(cam + 0xFC, upZ);
    gabi::store<s16>(cam + 0x100, *bank); /* fopCamM_SetBank */
    gabi::store<u32>(cam + 0xD4, fovy);    /* fopCamM_SetFovy */

    u32 stage = dComIfGp_ea() + 0x5150;
    u32 stat = gabi::load<u32>(dComIfGp_ea() + camId * 0x34 + 0x5B00);
    if ((stat & 0x48) && !(gabi::load<u32>(dComIfGp_ea() + 0x5CDC) & 0x100000)) {
        gabi::store<f32>(cam + 0xCC, 30.0f); /* telescope: fopCamM_SetNear(30) */
        if (stage != 0) {
            u32 info = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(stage) + 0x15C), stage);
            gabi::store<u32>(cam + 0xD0, gabi::load<u32>(info + 4)); /* fopCamM_SetFar(mFarPlane) */
        }
    } else if (stage != 0) {
        gabi::Local<be<f32>> nearV, farV;
        u32 info = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(stage) + 0x15C), stage);
        gabi::store<u32>(gabi::ea(nearV.get()), gabi::load<u32>(info + 0)); /* mNearPlane */
        info = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(stage) + 0x15C), stage);
        gabi::store<u32>(gabi::ea(farV.get()), gabi::load<u32>(info + 4)); /* mFarPlane */
        dCamera_hdAdjustNear(body, nearV, farV);
        gabi::store<u32>(cam + 0xCC, gabi::load<u32>(gabi::ea(nearV.get())));
        gabi::store<u32>(cam + 0xD0, gabi::load<u32>(gabi::ea(farV.get())));
    }
    gabi::store<s16>(cam + 0x236, cSAngle_Inv(&body->mDirection.mU)); /* fopCamM_SetAngleY */
    gabi::store<s16>(cam + 0x234, (s16)body->mDirection.mV);          /* fopCamM_SetAngleX */
}
VERIFY(0x024F8430, store);

/* stack dBgS_GndChk (HD: constructor partly inlined; same as in d_camera_bg.cpp) */
static void gndChk_ct_at(u32 o, const cXyz* pos) {
    gabi::call(0x02008E0C, o);
    gabi::store<u8>(o + 0x44, 0);
    gabi::store<u8>(o + 0x45, 0);
    gabi::store<u8>(o + 0x46, 0);
    gabi::store<u8>(o + 0x47, 0);
    gabi::store<u8>(o + 0x48, 0);
    gabi::store<u8>(o + 0x49, 0);
    gabi::store<u8>(o + 0x4A, 0);
    gabi::store<u32>(o + 0x10, 0x1004A4FC);
    gabi::store<u32>(o + 0x20, 0x1004A50C);
    gabi::store<u32>(o + 0x40, 0x1004A52C);
    gabi::store<u32>(o + 0x4C, 0x1004A51C);
    gabi::store<u32>(o + 0x50, 1);
    gabi::store<u32>(o + 0x00, o + 0x40);
    gabi::store<u32>(o + 0x04, o + 0x4C);
    gabi::at<cXyz>(o + 0x24)->copy(*pos);
}

/* 024FFC40. HD: besides the GameCube work (scissor below/above the trim bars, projection, view,
 * audio camera info, ground sound, map draw) the body's sead::LookAtCamera (eye, centre, up rotated
 * by the bank around the view direction) and sead projection (near/far/fovy/aspect; 4:3 for the
 * cinema-scope trim in one display mode; a zoom factor eased towards 1) are updated, and the sead
 * camera matrix is copied to j3dSys's view matrix */
static BOOL camera_draw(camera_class* i_this) {
    WWHD_FUNC(0x024FFC40, BOOL, i_this);
    u32 cam = cam_ea(i_this);
    dCamera_c* body = gabi::at<dCamera_c>(cam + CAMERA_BODY);
    u32 b = body->ea();
    s32 camId = fopCamM_GetParam(i_this);
    s32 winId = gabi::load<s8>(dComIfGp_ea() + camId * 0x34 + 0x5AFC);
    u32 window = dComIfGp_ea() + 0x5ACC + winId * 0x2C;
    s32 camId2 = fopCamM_GetParam(i_this);
    f32 trim = (f32)gabi::ftoi(body->mTrimHeight);
    gabi::call(0x0252CCA8, window, 0.0f, trim, 1280.0f, 720.0f - (trim + trim)); /* window->setScissor */
    gabi::call(0x028E9948, cam + 0x104, gabi::load<f32>(cam + 0xD4), gabi::load<f32>(cam + 0xD8), gabi::load<f32>(cam + 0xCC),
               gabi::load<f32>(cam + 0xD0)); /* C_MTXPerspective(proj, fovy, aspect, near, far) */
    gabi::call(0x025F1EAC, cam + 0x144, cam + 0xDC, cam + 0xE8, cam + 0xF4, (s16)gabi::load<s16>(cam + 0x100)); /* mDoMtx_lookAt */

    /* sead camera: eye E, centre C, up U, direction D */
    gabi::Local<cXyz> E, C, U, D, Dsave;
    gabi::Local<be<f32>[4]> q;
    gabi::Local<be<f32>[9]> m;
    u32 e = gabi::ea(E.get()), c = gabi::ea(C.get()), u = gabi::ea(U.get());
    for (u32 k = 0; k < 12; k += 4) {
        gabi::store<u32>(e + k, gabi::load<u32>(cam + 0xDC + k));
        gabi::store<u32>(c + k, gabi::load<u32>(cam + 0xE8 + k));
        gabi::store<u32>(u + k, gabi::load<u32>(cam + 0xF4 + k));
    }
    f32 dx = C->x - E->x;
    f32 dy = C->y - E->y;
    f32 dz = C->z - E->z;
    D->x = dx;
    D->y = dy;
    D->z = dz;
    f32 len = gabi::call<f32>(0x025155D8, D.get()); /* sead::Vector3f::normalize */
    if (len == 0.0f) {
        C->z = C->z + 1.0f;
        D->x = dx;
        D->y = dy;
        D->z = C->z - E->z;
        gabi::call<f32>(0x025155D8, D.get());
    }
    f32 ux = U->x;
    const f32 eps = 3.8146973e-06f;
    f32 uy, uz;
    if (-eps > ux || ux > eps) {
        uz = U->z;
        uy = U->y;
    } else {
        uy = U->y;
        if (-eps > uy || uy > eps) {
            uz = U->z;
        } else {
            uz = U->z;
            if (!(-eps > uz) && !(uz > eps)) uy = 1.0f;
        }
    }
    f32 bank = (f32)gabi::load<s16>(cam + 0x100) * 9.58738e-05f;
    gabi::call(0x02515644, q.get(), D.get(), bank); /* sead::Quatf::setAxisAngle(dir, bank) */
    gabi::call(0x025156EC, m.get(), q.get());       /* sead::Matrix33f::fromQuat */
    be<f32>* M = *m;
    U->x = gabi::fmadds(M[2], uz, gabi::fmadds(M[0], ux, M[1] * uy));
    U->y = gabi::fmadds(M[5], uz, gabi::fmadds(M[3], ux, M[4] * uy));
    U->z = gabi::fmadds(M[8], uz, gabi::fmadds(M[6], ux, M[7] * uy));
    for (u32 k = 0; k < 12; k += 4) {
        gabi::store<u32>(b + 0x648 + k, gabi::load<u32>(e + k)); /* LookAtCamera pos */
        gabi::store<u32>(b + 0x654 + k, gabi::load<u32>(c + k)); /* at */
        gabi::store<u32>(b + 0x660 + k, gabi::load<u32>(u + k)); /* up */
    }
    gabi::call<f32>(0x025155D8, b + 0x660);
    u32 lookAt = b + 0x614;
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(b + 0x644) + 0x24), lookAt, lookAt); /* camera.doUpdateMatrix(&mtx) */
    gabi::call(0x0274E04C, b + 0x66C, gabi::load<f32>(cam + 0xCC), gabi::load<f32>(cam + 0xD0),
               gabi::load<f32>(cam + 0xD4) * 0.017453292f, gabi::load<f32>(cam + 0xD8)); /* projection.set(near, far, fovy, aspect) */
    s32 trimSize = body->mTrimSize;
    if ((trimSize == 2 || trimSize == 3) && gabi::load<s32>(0x101D6010) == 1) {
        gabi::store<f32>(b + 0x730, 1.3333334f);
        gabi::store<f32>(b + 0x734, 1.3333334f);
        gabi::call(0x02738AC0, b + 0x66C, b + 0x730);
    } else {
        gabi::Local<be<f32>> zoom;
        gabi::store<u32>(gabi::ea(zoom.get()), gabi::load<u32>(b + 0x730));
        if (gabi::call<u32>(0x025BDC60) != 0) {
            *zoom = 2.0f;
            gabi::store<u8>(b + 0x738, 1);
            u32 p = gabi::call<u32>(0x02739728);
            f32 k = 63.0f / gabi::load<f32>(p + 4);
            gabi::store<f32>(b + 0x71C, 0.0f);
            gabi::store<u8>(b + 0x66C, 1);
            gabi::store<f32>(b + 0x720, k * *zoom);
        } else if (gabi::load<u8>(b + 0x738) != 0) {
            *zoom = 1.0f;
            gabi::store<u8>(b + 0x738, 0);
            gabi::store<u32>(b + 0x71C, gabi::load<u32>(0x104A0908));
            gabi::store<u8>(b + 0x66C, 1);
            gabi::store<u32>(b + 0x720, gabi::load<u32>(0x104A090C));
        } else {
            gabi::call<f32>(0x0200ECD4, zoom.get(), 1.0f, 0.2f, 0.3f, 0.1f); /* cLib_addCalc */
        }
        gabi::store<u32>(b + 0x730, gabi::load<u32>(gabi::ea(zoom.get())));
        gabi::store<u32>(b + 0x734, gabi::load<u32>(gabi::ea(zoom.get())));
        gabi::call(0x02738AC0, b + 0x66C, b + 0x730);
    }
    for (u32 k = 0; k < 0x30; k += 4) gabi::store<u32>(0x104B45F8 + k, gabi::load<u32>(lookAt + k)); /* j3dSys.setViewMtx */
    u32 proj = gabi::call<u32>(0x0274D83C, b + 0x66C);
    gabi::call(0x028E8970, proj, 0x104B470Cu);
    gabi::call(0x028E91EC, cam + 0x144, cam + 0x174); /* PSMTXInverse(view, invView) */
    gabi::call(0x025E1B44, cam + 0xDC, 0x104B45F8u, camId2); /* mDoAud_getCameraInfo */

    gabi::Local<u8[0x54]> gnd;
    u32 g = gabi::ea(gnd.get());
    gndChk_ct_at(g, gabi::at<cXyz>(cam + 0xDC));
    f32 groundY = gabi::call<f32>(0x02008974, dComIfGp_ea() + PLAY_BGS, g); /* GroundCross */
    if (groundY != -1000000000.0f) {
        u32 snd = gabi::call<u32>(0x024EECAC, dComIfGp_ea() + PLAY_BGS, g + 0x14); /* GetMtrlSndId */
        gabi::call(0x025E1B60, snd);                                              /* mDoAud_getCameraMapInfo */
        u32 grp = gabi::call<u32>(0x024EEDB8, dComIfGp_ea() + PLAY_BGS, g + 0x14); /* GetGrpSoundId */
        gabi::call(0x025E1DE4, grp & 0xFF);                                       /* mDoAud_setCameraGroupInfo */
        u32 iface = gabi::load<u32>(0x101FFC78);
        gabi::Local<cXyz> polyPos;
        gabi::store<u32>(gabi::ea(polyPos.get()), gabi::load<u32>(cam + 0xDC));
        polyPos->y = groundY;
        gabi::store<u32>(gabi::ea(polyPos.get()) + 8, gabi::load<u32>(cam + 0xE4));
        if (iface != 0) gabi::call(0x02027754, iface, polyPos.get()); /* setCameraPolygonPos */
    } else {
        u32 iface = gabi::load<u32>(0x101FFC78);
        if (iface != 0) gabi::call(0x02027754, iface, 0);
    }
    PSMTXCopy(gabi::at<Mtx34>(cam + 0x144), gabi::at<Mtx34>(cam + 0x1E4));
    gabi::store<f32>(cam + 0x1F0, 0.0f);
    gabi::store<f32>(cam + 0x200, 0.0f);
    gabi::store<f32>(cam + 0x210, 0.0f);
    gabi::call(0x025F1FC4, cam + 0x104, cam + 0x144, cam + 0x1A4); /* mDoMtx_concatProjView */
    body->Draw();
    if (gabi::call<s32>(0x025DF2B8, i_this) != 1) { /* fpcM_DrawPriority */
        fopCamM_GetParam(i_this);
        dComIfGp_ea();
        dComIfGp_ea();
        if (!gabi::call<bool>(0x025DBE38)) { /* fopOvlpM_IsDoingReq */
            f32 px = gabi::load<f32>(gabi::load<u32>(dComIfGp_ea() + 0x5B2C) + 0x314);
            f32 pz = gabi::load<f32>(gabi::load<u32>(dComIfGp_ea() + 0x5B2C) + 0x31C);
            s32 stay = gabi::load<s8>(0x1047E6C8); /* dComIfGp_roomControl_getStayNo() */
            f32 py = gabi::load<f32>(gabi::load<u32>(dComIfGp_ea() + 0x5B2C) + 0x318);
            gabi::call(0x0258FD88, stay, px, pz, py); /* dComIfGp_map_draw */
        }
    }
    gabi::store<u32>(g + 0x20, 0x1004A50C);
    gabi::store<u32>(g + 0x40, 0x1004A52C);
    gabi::store<u32>(g + 0x4C, 0x1004A4EC);
    gabi::call(0x02008DAC, g, 0);
    return TRUE;
}
VERIFY(0x024FFC40, camera_draw);
