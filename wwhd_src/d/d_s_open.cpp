/** Opening scene: native HD UI and transition to the game.
 * Verified against cking.rpx. */
#include "d/d_s_open.h"

const gc_tu_data gc_tu = {.headerStatics = {0x1047B3E0, 0x101EA810, 0x1047B3EC}};
using OpeningUIPtr = gptr<dScnOpen_UI>;
using OpeningSavePtr = gptr<dScnOpen_SaveInfoHD>;
using OpeningPadPtr = gptr<dScnOpen_PadHD>;
using OpeningLoaderPtr = gptr<void>;
static OpeningUIPtr l_ui;
static OpeningSavePtr l_save;
static OpeningPadPtr l_pad;
static OpeningLoaderPtr l_loader;
static u8 l_phase;
// HD: profile numbers and the native controller masks used by this scene.
enum { fpcNm_OPEN_SCENE_e = 12, fpcNm_OPEN2_SCENE_e = 13, fpcNm_OPENING2_SCENE_e = 8, fpcNm_PLAY_SCENE_e = 7 };
enum { PAD_TRIGGER_START = 0x1000, PAD_HOLD_A = 0x8000 };

/* 025AEBE4 */
BOOL dScnOpen_c::draw() {
    dScnOpen_GameInfoHD* play = dScnOpen_getGameInfo();
    void* window = play->mWindow;
    play = dScnOpen_getGameInfo();
    play->mpWindow = window;
    play = dScnOpen_getGameInfo();
    play->mpViewport = window;
    for (dScnOpen_DrawTag* pTag = fopDwIt_Begin(); pTag != nullptr; pTag = fopDwIt_Next(pTag))
        fpcM_Draw(pTag->mpTagData);
    dScnOpen_proc_c* packet = mpProc;
    play = dScnOpen_getGameInfo();
    dScnOpen_DrawList* list = &play->mDrawList;
    dScnOpen_set2DOpa(list, &list->mp2DOpaHead, &list->mp2DOpaTail, packet);
    return TRUE;
}

/* 025AEC70 */
static BOOL dScnOpen_Draw(dScnOpen_c* i_this) { return i_this->draw(); }

/* 025AEC74 */
void dScnOpen_c::changeGameScene() {
    if (fopOvlpM_IsPeek()) return;
    if (mProcName == fpcNm_OPEN2_SCENE_e) {
        dComIfG_changeOpeningScene(this, fpcNm_OPENING2_SCENE_e);
        return;
    }
    // HD: transition binding carries the extra final flag.
    if (fopScnM_ChangeReq(this, fpcNm_PLAY_SCENE_e, 0, 5, 1)) {
        l_save->mRestartRoomParam = 0;
        dScnOpen_GameInfoHD* play = dScnOpen_getGameInfo();
        const char* stage = play->mNextStageName;
        play = dScnOpen_getGameInfo();
        s32 room = play->mNextStageRoomNo;
        play = dScnOpen_getGameInfo();
        s32 layer = play->mNextStageLayer;
        mDoAud_setSceneName(stage, room, layer);
    }
}

/* 025AED3C */
BOOL dScnOpen_c::execute() {
    // HD: native UI executes before the stream timer and input checks.
    dScnOpen_UI_execute(l_ui);
    u32 count = mStreamTimer;
    if ((s32)count < 90) {
        ++count;
        mStreamTimer = count;
        if (count == 40) mDoAud_bgmStreamPrepare(0xC0000024);
        else if ((s32)count >= 90) mDoAud_bgmStreamPlay();
    }
    s16 name = mProcName;
    u32 skip = 0;
    if (name == fpcNm_OPEN_SCENE_e && l_ui->mTimer > 0) {
        BOOL pressed = mDoCPd_checkStart(0);
        name = mProcName;
        if (pressed) skip = 1;
    }
    if (name == fpcNm_OPEN_SCENE_e && mDoCPd_checkStart(0)) {
        dScnOpen_UI* ui = l_ui;
        dScnOpen_proc_c* proc = ui->mpProc;
        ui->mTimer = 300;
        dScnOpen_proc_skip(proc);
    }
    if (!fopOvlpM_IsPeek() && !dComIfG_resetToOpening(this)) {
        u32 buttons = 0;
        if (mProcName == fpcNm_OPEN2_SCENE_e) {
            if (mDoCPd_checkA(0) || mDoCPd_checkB(0) || mDoCPd_checkStart(0) || mDoCPd_checkX(0) || mDoCPd_checkY(0)) {
                buttons = 1;
            } else {
                dScnOpen_PadHD* pad = l_pad;
                if ((pad->mTrigger & PAD_TRIGGER_START) || (pad->mpButtons->mButtons & PAD_HOLD_A)) buttons = 1;
            }
        }
        if (skip | buttons) {
            field_0x1d4 = 1;
            mDoAud_bgmStop(20);
        }
        if (field_0x1d4 && !mDoAud_isUsedHeapForStreamBuffer()) changeGameScene();
    }
    if (l_ui->mpProc->mIsEnd) changeGameScene();
    return TRUE;
}

/* 025AEF38 */
static BOOL dScnOpen_Execute(dScnOpen_c* i_this) { return i_this->execute(); }

/* 025AEF3C */
static void dScnOpen__dt(dScnOpen_c* i_this, u32 flags) {
    if (!i_this) return;
    dScnOpen_proc_c* proc = i_this->mpProc;
    if (proc) gabi::call_ptr(proc->mVtable->destroy, proc, 3);
    void* heap = i_this->solid_heap;
    if (heap) mDoExt_destroySolidHeap(heap);
    dScnOpen_resDelete(&i_this->mPhs, "Opening");
    dScnOpen_getGameInfo()->mWindowNum = 0;
    // HD: native UI is owned by its singleton rather than the scene's heap.
    dScnOpen_UI_cleanup(l_ui);
    dScnOpen_UI_destroy();
    l_phase = 0;
    base_process_class__dt(i_this, 2);
    if (flags & 1) operator_delete(i_this);
}

/* 025AEFFC */
static BOOL dScnOpen_Delete(dScnOpen_c* i_this) {
    dScnOpen__dt(i_this, 2);
    return TRUE;
}

/* 025AF024 */
static dScnOpen_c* dScnOpen__ct(dScnOpen_c* i_this) {
    if (!i_this) i_this = static_cast<dScnOpen_c*>(operator_new(sizeof(dScnOpen_c)));
    if (i_this) {
        base_process_class__ct(i_this);
        dScnOpen_initPhase(&i_this->mPhs, 8);
        i_this->mStreamTimer = 0;
        i_this->solid_heap = nullptr;
        i_this->field_0x1d4 = 0;
        i_this->mpProc = nullptr;
    }
    return i_this;
}

/* 025AF08C */
s32 dScnOpen_c::create() {
    // HD: asynchronous native UI load replaces the GC Opening archive/heap setup.
    u8 phase = l_phase;
    if (phase > 1) {
        mStreamTimer = 0;
        dScnOpen_getGameInfo()->mEnableNextStage = 0;
        dScnOpen_UI_select(0);
        dScnOpen_UI_initialize(l_ui);
        l_phase = 0;
        return cPhs_COMPLEATE_e;
    }
    u32 result = phase == 0 ? dScnOpen_requestLoad(l_loader, 2) : dScnOpen_checkLoad(l_loader);
    if (result) l_phase = (u8)(l_phase + 1);
    return cPhs_INIT_e;
}

/* 025AF138 */
static s32 dScnOpen_Create(dScnOpen_c* i_this) {
    if (i_this) dScnOpen__ct(i_this);
    return i_this->create();
}

/* 025AF208 */
static BOOL dScnOpen_IsDelete() { return TRUE; }

// HD: compiler-generated packet destructors and header statics.
/* 025AEBCC */
static void dScnOpen_packet__dt(void* packet, u32 flags) {
    if (packet && (flags & 1)) operator_delete(packet);
}
/* 025AEBE0 */
static void dScnOpen_packet_draw() {}

/* 025AF174 */
static void __sinit_d_s_open_cpp() {
    sinit_header_statics_rodata(gc_tu.headerStatics[0], gc_tu.headerStatics[1], gc_tu.headerStatics[2], 0x10052B2C);
}

/* WWHD glue
str 10052B1C # "Opening"
data l_ui 101F8318
data l_save 101F84DC
data l_pad 101F5088
data l_loader 101F7274
data l_phase 101EA834
ext 025200D4 dScnOpen_GameInfoHD* dScnOpen_getGameInfo()
ext 025DA7AC dScnOpen_DrawTag* fopDwIt_Begin()
ext 025DA7D0 dScnOpen_DrawTag* fopDwIt_Next(dScnOpen_DrawTag*)
ext 025DF904 void fpcM_Draw(void*)
ext 0252CDC0 void dScnOpen_set2DOpa(dScnOpen_DrawList*, gptr<void>*, gptr<void>*, dScnOpen_proc_c*)
ext 025DBE00 BOOL fopOvlpM_IsPeek()
ext 02520230 void dComIfG_changeOpeningScene(dScnOpen_c*, u32)
ext 025DC86C BOOL fopScnM_ChangeReq(dScnOpen_c*, u32, u32, u32, u32)
ext 025E17CC void mDoAud_setSceneName(const char*, s32, s32)
ext 0270CA94 void dScnOpen_UI_execute(dScnOpen_UI*)
ext 0270CB3C void dScnOpen_UI_cleanup(dScnOpen_UI*)
ext 0270CE94 void dScnOpen_UI_destroy()
ext 0270CD94 void dScnOpen_UI_select(u32)
ext 0270C940 void dScnOpen_UI_initialize(dScnOpen_UI*)
ext 026BF3E8 void dScnOpen_proc_skip(dScnOpen_proc_c*)
ext 026FBB8C u32 dScnOpen_requestLoad(void*, u32)
ext 026FBB7C u32 dScnOpen_checkLoad(void*)
ext 025E1934 void mDoAud_bgmStreamPrepare(u32)
ext 025E1944 void mDoAud_bgmStreamPlay()
ext 025E1904 void mDoAud_bgmStop(u32)
ext 025E20F0 BOOL mDoAud_isUsedHeapForStreamBuffer()
ext 02007898 BOOL mDoCPd_checkA(u32)
ext 020078BC BOOL mDoCPd_checkB(u32)
ext 02007940 BOOL mDoCPd_checkStart(u32)
ext 020078E8 BOOL mDoCPd_checkX(u32)
ext 02007914 BOOL mDoCPd_checkY(u32)
ext 025202F8 BOOL dComIfG_resetToOpening(dScnOpen_c*)
ext 025E3868 void mDoExt_destroySolidHeap(void*)
ext 025204C8 void dScnOpen_resDelete(request_of_phase_process_class*, const char*)
ext 025DD5F0 void base_process_class__ct(base_process_class*)
ext 025DD630 void base_process_class__dt(base_process_class*, u32)
ext 028F521C void dScnOpen_initPhase(request_of_phase_process_class*, u32)
*/
