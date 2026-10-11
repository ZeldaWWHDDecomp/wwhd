/** Title Screen manager and ship logo, ported to the WWHD layout.
 * The HD 2D layout owns logo fades; all marked entry points retain their
 * verified semantics. */
#include "d/actor/d_a_title.h"
using namespace title_actor;
const gc_tu_data gc_tu = {.safeString = 0x100400C4,
                          .headerStatics = {0x1046E6FC, 0x101D23F4, 0x1046E708}};

static constexpr u32 TITLE_TRIGGER_MASK = 0x861B;
enum TitleEnterMode { ENTER_INTRO, ENTER_WAIT, ENTER_REQUEST, ENTER_LEAVING };
enum TitleParticle {
    ID_AK_S1_TITLESMOKE00 = 0x83F9,
    ID_AK_S1_TITLEWIND00,
    ID_AK_S2_TITLEKIRAKIRA00
};
enum TitleSound { JA_SE_TITLE_KIRA = 0x909, JA_SE_TITLE_WIND = 0x90B, JA_SE_OP_ENTER_GAME = 0x82F };
enum TitleScene { TITLE_NAME_SCENE = 9, TITLE_SCENE = 13 };
static AttrWordsHD titleAttrValues;
static WordHD titleAttrVtable;
static WordHD titleProcVtable;
static WordHD titleActorVtable;
static WordHD titleTransformBaseVtable;
static WordHD titleMtxCalcVtable;
static WordHD titleBckVtable;
static LayoutPointerHD titleLayout;
static PadPointerHD titlePad;
static PadStatusPointerHD titlePadStatus;
static ResetPointerHD titleReset;
static WordHD titleRoomProcId;
static ByteHD titleKirakiraSoundFlag;
static GXColor titleBlackColor;
static J3DSysHD titleJ3dSys;
static ViewportPointerHD titleViewportIndex;
static RendererPointerHD titleRenderer;
static RuntimeTypeHD titleRuntimeType;
static WordHD titleRuntimeGuard;
static WordHD titleRuntimeVtable;
static ColorChannelsHD titleDrawChannels;

static void titleDestroySolidHeap(HeapHD*);
static void titleStartSound(u32 id);
static void titleSetPaneAlpha(void* pane);
static BOOL titleIsOverlayPeek();
static void* titleSearchScene(u32 id);
static BOOL titleChangeScene(void* scene, s16 name, s16 p2, u16 p3, u32 p4);
static void titleSetFadeColor(const GXColor* color);
static u32 titleConsumeTrigger(PadHD* pad, u32 mask);
static HeapHD* titleCreateSolidHeap(u32 size, u32 align);
static void titleRestoreHeap();
static void titleInvisibleEntryDL(void* invisible);
static void titleDrawListConstruct(daTitle_proc_c* proc);
static void titleDrawListDestruct(daTitle_proc_c* proc, s32 flags);
static void titleMtxCalcConstruct(void* calc);
static void titleTransformDestruct(void* transform, s32 flags);
static BOOL titleBpkInit(mDoExt_bpkAnm* anm, J3DModelData* data, void* resource, s32 play, s32 mode,
                         f32 speed, s32 start, s32 end, s32 modify, s32 flags);
static void titleActorDestruct(daTitle_c* actor, s32 flags);
static void titleLayoutStartIntro(LayoutHD* layout);
static void titleLayoutEnterGame(LayoutHD* layout);
static LayoutPositionHD* titleLayoutSmokePosition(LayoutHD* layout);
static f32 titleLayoutSmokeScale(LayoutHD* layout);
static f32 titleLayoutSmokeParticleScale(LayoutHD* layout);
static LayoutPositionHD* titleLayoutKiraPosition(LayoutHD* layout);
static f32 titleLayoutKiraScale(LayoutHD* layout);
static f32 titleLayoutKiraParticleScale(LayoutHD* layout);
static LayoutPositionHD* titleLayoutWindPosition(LayoutHD* layout);
static f32 titleLayoutWindScale(LayoutHD* layout);
static f32 titleLayoutWindParticleScale(LayoutHD* layout);
static s32 titleLayoutWindInterval(LayoutHD* layout);
static f32 titleLayoutParticleDepth(LayoutHD* layout);
static s32 titleLayoutKiraStart(LayoutHD* layout);
static s32 titleLayoutKiraEnd(LayoutHD* layout);
static daTitle_proc_c* daTitle_proc_ct(daTitle_proc_c* p);

/* 024B8B44 */
void daTitle_proc_c::proc_init3D() {
    m_solid_heap = titleCreateSolidHeap(0x40000U, 0x20);
    if (m_solid_heap == 0) /* JUT_ASSERT(0xC0, m_solid_heap != NULL) */
        JUT_ASSERT_fail("d_a_title.cpp", 0xC0, "m_solid_heap != (0)");
    // HD: the guest heap name is Shift-JIS for "for the title"; this glue alias
    // keeps its original bytes.
    m_solid_heap->mName.v = gabi::guest_addr("title");

    J3DModelData* modelData_ship =
        (J3DModelData*)dComIfG_getObjectRes("Tlogo", dRes_INDEX_TLOGO_BDL_TITLE_SHIP_e);
    if (modelData_ship == NULL)
        JUT_ASSERT_fail("d_a_title.cpp", 0xC9, "modelData_ship != (0)");

    mModel_ship = mDoExt_J3DModel__create(modelData_ship, 0x80000U, 0x37441423U);
    if (!mModel_ship)
        JUT_ASSERT_fail("d_a_title.cpp", 0xCE, "mModel_ship != (0)");

    // HD: the ship uses an invisible model wrapper.
    if (mDoExt_invisibleModel_create(mInvisible, mModel_ship) != 1)
        JUT_ASSERT_fail("d_a_title.cpp", 0xD3, "rt == 1");

    J3DModelData* modelData_sub =
        (J3DModelData*)dComIfG_getObjectRes("Tlogo", dRes_INDEX_TLOGO_BDL_SUBTITLE_START_ANIM_e);
    if (modelData_sub == NULL)
        JUT_ASSERT_fail("d_a_title.cpp", 0xD8, "modelData_sub != (0)");

    mModel_subtitle = mDoExt_J3DModel__create(modelData_sub, 0x80000U, 0x37441422U);
    if (!mModel_subtitle)
        JUT_ASSERT_fail("d_a_title.cpp", 0xDD, "mModel_subtitle != (0)");

    J3DModelData* modelData_kirari =
        (J3DModelData*)dComIfG_getObjectRes("Tlogo", dRes_INDEX_TLOGO_BDL_SUBTITLE_KIRARI_e);
    if (modelData_kirari == NULL)
        JUT_ASSERT_fail("d_a_title.cpp", 0xE1, "modelData_kirari != (0)");

    mModel_kirari = mDoExt_J3DModel__create(modelData_kirari, 0x80000U, 0x37441422U);
    if (!mModel_kirari)
        JUT_ASSERT_fail("d_a_title.cpp", 0xE6, "mModel_kirari != (0)");

    J3DAnmTransform* bck_ship =
        (J3DAnmTransform*)dComIfG_getObjectRes("Tlogo", dRes_INDEX_TLOGO_BCK_TITLE_SHIP_e);
    if (bck_ship == NULL)
        JUT_ASSERT_fail("d_a_title.cpp", 0xEB, "bck_ship != 0");

    BOOL ok_bck =
        mBckShip.init(modelData_ship, bck_ship, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false);
    if (ok_bck == FALSE)
        JUT_ASSERT_fail("d_a_title.cpp", 0xF1, "ok_bck != 0");

    void* bpk_ship = dComIfG_getObjectRes("Tlogo", dRes_INDEX_TLOGO_BPK_TITLE_SHIP_e);
    if (bpk_ship == 0)
        JUT_ASSERT_fail("d_a_title.cpp", 0xF6, "bpk_ship != 0");

    BOOL ok_bpk = titleBpkInit(&mBpkShip, modelData_ship, bpk_ship, 1, J3DFrameCtrl::EMode_LOOP,
                               1.0f, 0, -1, 0, 0);
    if (ok_bpk == FALSE)
        JUT_ASSERT_fail("d_a_title.cpp", 0xFC, "ok_bpk != 0");

    bpkFrameCtrl().setRate(1.0f);
    bpkFrameCtrl().setFrame(0.0f);

    J3DAnmTextureSRTKey* btk_sub = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(
        "Tlogo", dRes_INDEX_TLOGO_BTK_SUBTITLE_START_ANIM_e);
    if (btk_sub == NULL)
        JUT_ASSERT_fail("d_a_title.cpp", 0x103, "btk_sub != 0");

    BOOL ok_btk_subtitle =
        mBtkSub.init(modelData_sub, btk_sub, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0);
    if (ok_btk_subtitle == FALSE)
        JUT_ASSERT_fail("d_a_title.cpp", 0x10A, "ok_btk_subtitle != 0");

    J3DAnmTextureSRTKey* btk_kirari =
        (J3DAnmTextureSRTKey*)dComIfG_getObjectRes("Tlogo", dRes_INDEX_TLOGO_BTK_SUBTITLE_KIRARI_e);
    if (btk_kirari == NULL)
        JUT_ASSERT_fail("d_a_title.cpp", 0x10F, "btk_kirari != 0");

    BOOL ok_btk_kirari = mBtkKirari.init(modelData_kirari, btk_kirari, true,
                                         J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0);
    if (ok_btk_kirari == FALSE)
        JUT_ASSERT_fail("d_a_title.cpp", 0x115, "ok_btk_kirari != 0");

    titleRestoreHeap();
    set_mtx();
}

/* 024B7CE4 */
void daTitle_proc_c::setEnterMode() {
    if (mEnterMode == ENTER_WAIT) {
        mEnterMode = ENTER_REQUEST;
    }
}

/* 024B83F4 */
void daTitle_proc_c::set_mtx() {
    f32 s = mAttr.field_0x08;
    J3DModel* model = mModel_ship;
    model->mBaseScale.z = s;
    model->mBaseScale.y = s;
    model->mBaseScale.x = s;
    // HD: ship depth 100, reversed quarter-turn, and the quadratic offset.
    mDoMtx_stack_c::transS(mAttr.field_0x00 + m094, mAttr.field_0x04, 100.0f);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), 0, -0x4000, 0);
    J3DModel_setBaseTRMtx(mModel_ship, mDoMtx_stack_c::get());
    f32 sx = mAttr.field_0x18, sy = mAttr.field_0x1C;
    model = mModel_subtitle;
    model->mBaseScale.x = sx;
    model->mBaseScale.y = sy;
    model->mBaseScale.z = 1;
    model = mModel_kirari;
    model->mBaseScale.x = sx;
    model->mBaseScale.y = sy;
    model->mBaseScale.z = 1;
    f32 px = mAttr.field_0x10, py = mAttr.field_0x14;
    mDoMtx_stack_c::transS(px, py, -10000.0f);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), 0, -0x8000, 0);
    J3DModel_setBaseTRMtx(mModel_subtitle, mDoMtx_stack_c::get());
    mDoMtx_stack_c::transS(px, py, -10010.0f);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), 0, -0x8000, 0);
    J3DModel_setBaseTRMtx(mModel_kirari, mDoMtx_stack_c::get());
}

/* m018 > v, with v computed first (m018 is loaded after the call) */
static inline bool seq_gt(s32 v, be<s32>& m018) { return m018 > v; }

// HD: emitter kinds seven and above negate the global Y translation.
static void emitter_setGlobalTranslation(EmitterHD* e, f32 x, f32 y, f32 z) {
    if (e->mEmitterType >= 7)
        y = -y;
    e->mGlobalTranslation.x = x;
    e->mGlobalTranslation.y = y;
    e->mGlobalTranslation.z = z;
}

/* 024B7CFC */
void daTitle_proc_c::calc_2d_alpha() {
    s32 cnt = m018 + 1;
    s32 mode = mEnterMode;
    LayoutHD* t2d = titleLayout;
    m018 = cnt;
    if (mode == ENTER_INTRO) {
        if (cnt >= 200) {
            mEnterMode = ENTER_WAIT;
        } else {
            if (cnt == 30)
                titleLayoutStartIntro(titleLayout.get());
            if (m098 < 0) {
                m098 = m098 + 1;
            }
        }
    }

    /* HD: the blink counter runs in every mode */
    if (m028 >= 100) {
        m028 = 0;
    } else {
        m028 = m028 + 1;
    }

    if (m020 == 0) {
        f32 r = cM_rndF(mAttr.field_0x28);
        m020 = gabi::ftoi(r + mAttr.field_0x2C);
        if (titleLayoutWindInterval(t2d) != 0)
            m020 = titleLayoutWindInterval(t2d);
        f32 x = titleLayoutWindPosition(t2d)->x;
        f32 y = titleLayoutWindPosition(t2d)->y;
        f32 z = titleLayoutParticleDepth(t2d);
        cXyz pos;
        pos.z = z;
        pos.y = y;
        pos.x = x;
        csXyz angle;
        s16 ax = (s16)gabi::ftoi(cM_rndFX(32768.0f));
        angle.y = 0;
        angle.x = ax;
        angle.z = (s16)gabi::ftoi(cM_rndFX(4000.0f));
        EmitterHD* e = reinterpret_cast<EmitterHD*>(
            dPa_control_set(dComIfGp_getParticle(), 8, ID_AK_S1_TITLEWIND00, &pos, &angle, nullptr,
                            0xFF, nullptr, -1, nullptr, nullptr, nullptr));
        f32 s = titleLayoutWindScale(t2d);
        e->mGlobalScale.x = s;
        e->mGlobalScale.y = s;
        e->mGlobalScale.z = s;
        s = titleLayoutWindParticleScale(t2d);
        e->mGlobalParticleScale.x = s;
        e->mGlobalParticleScale.y = s;
        e->mGlobalParticleScale.z = s;
    } else {
        m020 = m020 - 1;
    }

    /* HD: no kirari animation; m094 sign flipped against GameCube */
    s32 sq = m098 * m098;
    if (m098 > 0) {
        m094 = -((f32)sq * mAttr.field_0x0C);
        bpkFrameCtrl().setFrame(100.0f - (f32)(m098 * 2));
    } else {
        m094 = (f32)sq * mAttr.field_0x0C;
        bpkFrameCtrl().setFrame((f32)(m098 * 2) + 100.0f);
    }

    if (mEnterMode == ENTER_INTRO) {
        EmitterHD* e = mpEmitter;
        f32 x = titleLayoutSmokePosition(t2d)->x;
        x = x + m094;
        x = x + mAttr.field_0x30;
        f32 y = titleLayoutSmokePosition(t2d)->y;
        y = y + mAttr.field_0x34;
        f32 z = titleLayoutParticleDepth(t2d);
        cXyz pos;
        pos.x = x;
        pos.z = z;
        pos.y = y;
        if (e == 0) {
            mpEmitter = reinterpret_cast<EmitterHD*>(
                dPa_control_set(dComIfGp_getParticle(), 8, ID_AK_S1_TITLESMOKE00, &pos, nullptr,
                                nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr));
            f32 s = titleLayoutSmokeScale(t2d);
            mpEmitter->mGlobalScale.x = s;
            mpEmitter->mGlobalScale.y = s;
            mpEmitter->mGlobalScale.z = s;
            s = titleLayoutSmokeParticleScale(t2d);
            EmitterHD* e1 = mpEmitter;
            e1->mGlobalParticleScale.x = s;
            e1->mGlobalParticleScale.y = s;
            e1->mGlobalParticleScale.z = s;
        } else {
            emitter_setGlobalTranslation(mpEmitter.get(), x, y, z);
        }

        s32 kiraStart = titleLayoutKiraStart(t2d);
        if ((u32)m018 == (u32)kiraStart) {
            if (u8(titleKirakiraSoundFlag) == 1) { /* daTitle_Kirakira_Sound_flag */
                titleStartSound(JA_SE_TITLE_KIRA);
                titleKirakiraSoundFlag = 0;
            }
            f32 kx = titleLayoutKiraPosition(t2d)->x;
            f32 ky = titleLayoutKiraPosition(t2d)->y;
            m00C.x = kx;
            m00C.y = ky;
            m00C.z = 0.0f;
            mpEmitter2 = reinterpret_cast<EmitterHD*>(
                dPa_control_set(dComIfGp_getParticle(), 7, ID_AK_S2_TITLEKIRAKIRA00, &m00C, nullptr,
                                nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr));
            f32 s = titleLayoutKiraScale(t2d);
            EmitterHD* e2 = mpEmitter2;
            e2->mGlobalScale.x = s;
            e2->mGlobalScale.y = s;
            e2->mGlobalScale.z = 1.0f;
            s = titleLayoutKiraParticleScale(t2d);
            e2 = mpEmitter2;
            e2->mGlobalParticleScale.x = s;
            e2->mGlobalParticleScale.y = s;
            e2->mGlobalParticleScale.z = 1.0f;
        } else if (seq_gt(titleLayoutKiraStart(t2d), m018) &&
                   !seq_gt(titleLayoutKiraEnd(t2d), m018) && mpEmitter2 != 0) {
            s32 end = titleLayoutKiraEnd(t2d);
            s32 start = titleLayoutKiraStart(t2d);
            m00C.x = m00C.x + 320.0f / (f32)(end - start);
            emitter_setGlobalTranslation(mpEmitter2.get(), m00C.x, m00C.y, m00C.z);
        }
    } else {
        if (mpEmitter == 0) {
            f32 x = titleLayoutSmokePosition(t2d)->x;
            f32 y = titleLayoutSmokePosition(t2d)->y;
            f32 z = titleLayoutParticleDepth(t2d);
            cXyz pos;
            pos.x = x;
            pos.z = z;
            pos.y = y;
            EmitterHD* e = reinterpret_cast<EmitterHD*>(
                dPa_control_set(dComIfGp_getParticle(), 8, ID_AK_S1_TITLESMOKE00, &pos, nullptr,
                                nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr));
            EmitterHD* e2 = mpEmitter2;
            mpEmitter = e;
            if (e2 == 0)
                return;
        } else {
            f32 x = titleLayoutSmokePosition(t2d)->x;
            x = x - m094;
            x = x + mAttr.field_0x30;
            f32 y = titleLayoutSmokePosition(t2d)->y;
            y = y + mAttr.field_0x34;
            f32 z = titleLayoutParticleDepth(t2d);
            emitter_setGlobalTranslation(mpEmitter.get(), x, y, z);
            if (mpEmitter2 == 0)
                return;
        }
        EmitterHD* e2 = mpEmitter2; /* mpEmitter2->becomeInvalidEmitter() */
        u32 fl = e2->mFlags;
        e2->mMaxFrame = -1;
        e2->mFlags = fl | 1;
        mpEmitter2 = 0;
    }
}

/* 024B8670 */
void daTitle_proc_c::proc_execute() {
    /* HD: no expanded-heap switch */
    if (m01C > 0) {
        m01C = m01C - 1;

        if (m01C == 0) {
            titleStartSound(JA_SE_TITLE_WIND);
        }
    } else {
        calc_2d_alpha();
    }

    PadHD* pad = titlePad;
    if (mEnterMode == ENTER_INTRO &&
        (u32(pad->mTrigger) & TITLE_TRIGGER_MASK) != 0) { /* trigger A / B / START / ... */
        m098 = 0;
        m01C = 0;
        PadStatusHD* o = titlePadStatus;
        o->mTriggerConsumed = (u8)titleConsumeTrigger(pad, TITLE_TRIGGER_MASK);
        mEnterMode = ENTER_LEAVING; /* HD: skips mode 1/2 */
    }

    for (int paneIdx = 0; paneIdx < 4; paneIdx++) {
        titleSetPaneAlpha(pane[paneIdx]);
    }

    if (mEnterMode == ENTER_REQUEST) {
        mEnterMode = ENTER_LEAVING;
    } else if (mEnterMode == ENTER_LEAVING) {
        m098 = m098 + 1;
    }

    mBckShip.play();
    set_mtx();
}

/* 024B7A6C */
void daTitle_proc_c::model_draw() {
    // HD: select a viewport target and check its sead runtime type before
    // drawing.
    be<s16>* indexPointer = titleViewportIndex;
    s16 idx = *indexPointer;
    ViewportTableHD* table = titleJ3dSys.mpViewports;
    u32 count = table->mEntryCount;
    ViewportEntryHD* entries = table->mpEntries;
    ViewportEntryHD* entry = u32(idx) < count ? entries + idx : entries;
    DrawTargetHD* target = nullptr;
    if (entry->mGeneration != 0) {
        u32 targetCount = table->mTargetCount;
        ViewportEntryHD* entry2 = u32(idx) < count ? entries + idx : entries;
        u16 k = entry2->mIndex;
        gptr<DrawTargetHD>* slot = nullptr;
        if (k < targetCount)
            slot = table->mpTargets.get() + k;
        target = gabi::at<DrawTargetHD>(readTargetSlot(slot));
    }
    if (titleRuntimeGuard == 0) {
        titleRuntimeGuard = 1;
        titleRuntimeType.mVtable = gabi::ea(&titleRuntimeVtable);
    }
    if (target != nullptr) {
        RuntimeVtableHD* vt = target->mRuntimeVtable;
        if (gabi::call_ptr<u32>(vt->mCheckDerived, target, &titleRuntimeType) == 0)
            target = nullptr;
    }
    DrawParametersHD* parameters = drawParameters(target);
    // HD: these channels are integer copies, including any signaling-NaN bit
    // patterns.
    for (u32 i = 0; i < 4; ++i)
        parameters->mChannels[i] = titleDrawChannels.words[i];
    parameters->mOffset = 0;
    parameters->mFar = 35000;
    parameters->mDistance = 30000;
    titleJ3dSys.mOpaDrawBuffer = titleGameInfo()->m2DDrawBuffer;
    titleJ3dSys.mXluDrawBuffer = titleGameInfo()->m2DDrawBuffer;
    // HD: only the ship is rendered here; the 2D layout owns the subtitle and
    // sparkle.
    if (bpkFrameCtrl().getFrame() != 0.0f) {
        J3DModelData* data = mModel_ship->getModelData();
        RendererHD* renderer = titleRenderer;
        u32 n = renderer->mDrawBufferCount;
        WordHD* bufferSlot = renderer->mpDrawBuffers;
        if (n > 3)
            bufferSlot = drawBufferSlot(bufferSlot, 12);
        u32 buffer = *bufferSlot;
        mBckShip.entry(data, mBckShip.mFrameCtrl.getFrame());
        mBpkShip.entry(mModel_ship->getModelData(), bpkFrameCtrl().getFrame());
        mDoExt_modelUpdateDL(mModel_ship, buffer);
        titleInvisibleEntryDL(mInvisible);
        ModelDataHD* modelData = reinterpret_cast<ModelDataHD*>(mModel_ship->getModelData());
        modelData->mpColorAnimation = 0;
        modelData = reinterpret_cast<ModelDataHD*>(mModel_ship->getModelData());
        modelData->mpJointTree->mpTransformAnimation = 0;
    }
    titleJ3dSys.mOpaDrawBuffer = titleGameInfo()->mOpaDrawBuffer;
    titleJ3dSys.mXluDrawBuffer = titleGameInfo()->mXluDrawBuffer;
}

/* 024B914C */
static void daTitle_proc_draw(daTitle_proc_c*) {}

/* 024B7CBC */
static BOOL daTitle_Draw(daTitle_c* i_this) {
    i_this->mpTitleProc->model_draw();
    return TRUE;
}

/* 024B87A4 */
static BOOL daTitle_Execute(daTitle_c* i_this) {
    if (!titleIsOverlayPeek()) {
        titleSetFadeColor(&titleBlackColor);

        daTitle_proc_c* proc = i_this->mpTitleProc;
        PadHD* pad = titlePad;
        if (proc->mEnterMode == ENTER_WAIT && (u32(pad->mTrigger) & TITLE_TRIGGER_MASK) != 0) {
            PadStatusHD* o = titlePadStatus;
            o->mTriggerConsumed = (u8)titleConsumeTrigger(pad, TITLE_TRIGGER_MASK);
            i_this->mpTitleProc->setEnterMode();
        } else if (proc->getEnterMode() == 3) {
            void* stageProc =
                titleSearchScene(u32(titleRoomProcId) /* dStage_roomControl_c::getProcID() */);
            if (stageProc == NULL)
                JUT_ASSERT_fail("d_a_title.cpp", 0x44A, "stageProc != (0)");

            if (!i_this->m29C && titleChangeScene(stageProc, TITLE_NAME_SCENE, 0, 5, 1)) {
                titleLayoutEnterGame(titleLayout.get()); /* HD */
                titleStartSound(JA_SE_OP_ENTER_GAME);
                i_this->m29C = true;
            }
        } else if (u32(titleReset->mReset) == 0 /* !mDoRst::isReset() */ &&
                   titleGameInfo()->mEnableNextStage != 0 /* dComIfGp_isEnableNextStage() */) {
            void* stageProc = titleSearchScene(u32(titleRoomProcId));
            if (stageProc == NULL)
                JUT_ASSERT_fail("d_a_title.cpp", 0x4A2, "stageProc != (0)");

            if (!i_this->m29C) {
                titleChangeScene(stageProc,
                                 TITLE_SCENE /* HD: always returns to the title scene. */, 1, 5, 1);
                i_this->m29C = true;
            }
        }
    }

    i_this->mpTitleProc->proc_execute();
    return TRUE;
}

/* 024B9130 */
static BOOL daTitle_IsDelete(daTitle_c* i_this) { return TRUE; }

/* 024B895C */
static BOOL daTitle_Delete(daTitle_c* i_this) { return TRUE; }

/* 024B8F44 */
static cPhs_State daTitle_Create(fopAc_ac_c* a) {
    daTitle_c* i_this = static_cast<daTitle_c*>(a);
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = gabi::ea(&titleActorVtable);
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&i_this->mPhs, "Tlogo");

    if (phase_state == cPhs_COMPLEATE_e) {
        daTitle_proc_c* p = (daTitle_proc_c*)operator_new(0x3F0);
        if (p != nullptr)
            p = daTitle_proc_ct(p);
        i_this->mpTitleProc = p;

        if (p == NULL) {
            return cPhs_ERROR_e;
        }

        /* HD: no proc_init2D */
        p->proc_init3D();
        i_this->m29C = false;
    }

    return phase_state;
}

// HD: compiler-generated and template code of this translation unit.

/* 024B8964 */
static void daTitle_Attr_init(daTitle_Attr_c* p) {
    // HD: fourteen integer lanes preserve the image's floating-point bit
    // patterns.
    for (int i = 0; i < 7; ++i) {
        p->words[2 * i] = titleAttrValues.words[2 * i];
        p->words[2 * i + 1] = titleAttrValues.words[2 * i + 1];
    }
}

/* 024B898C */
static daTitle_Attr_c* daTitle_Attr_ct(daTitle_Attr_c* p) {
    if (p == nullptr) {
        p = (daTitle_Attr_c*)operator_new(0x3C);
        if (p == nullptr)
            return nullptr;
    }
    p->__vtbl = gabi::ea(&titleAttrVtable);
    daTitle_Attr_init(p);
    return p;
}

/* 024B89D8 */
static daTitle_proc_c* daTitle_proc_ct(daTitle_proc_c* p) {
    if (p == nullptr) {
        p = (daTitle_proc_c*)operator_new(0x3F0);
        if (p == nullptr)
            return nullptr;
    }
    titleDrawListConstruct(p);
    p->__vtbl = gabi::ea(&titleProcVtable);
    daTitle_Attr_ct(&p->mAttr);
    mDoExt_invisibleModel_ct(p->mInvisible);
    p->mBckShip.mFrameCtrl.init(0);
    BckConstructorHD* bck = reinterpret_cast<BckConstructorHD*>(&p->mBckShip);
    bck->mTransformVtable = gabi::ea(&titleTransformBaseVtable);
    titleMtxCalcConstruct(bck->mMtxCalc);
    bck->mMtxCalcVtable = gabi::ea(&titleMtxCalcVtable);
    bck->mTransformVtable = gabi::ea(&titleBckVtable);
    bck->mFlags = 0;
    bck->mTail[0] = 0;
    bck->mTail[1] = 0;
    bck->mTail[2] = 0;
    bck->mTail[3] = 0;
    mDoExt_bpkAnm::ct(&p->mBpkShip);
    mDoExt_btkAnm::ct(&p->mBtkSub);
    mDoExt_btkAnm::ct(&p->mBtkKirari);
    p->m_solid_heap = 0;
    p->m098 = -50;
    p->m01C = 120;
    p->m_exp_heap = 0;
    /* HD: m094 = (m098 * m098) * attr().field_0x0C (GameCube: * -field_0x0C) */
    p->m094 = 2500.0f * p->mAttr.field_0x0C;
    p->mEnterMode = ENTER_INTRO;
    f32 r = cM_rndF(p->mAttr.field_0x28);
    p->m020 = gabi::ftoi(r + p->mAttr.field_0x2C);
    r = cM_rndF(p->mAttr.field_0x20);
    f32 v = r + p->mAttr.field_0x24 + 130.0f;
    p->m090 = 0;
    p->m024 = gabi::ftoi(v);
    p->m02C = 0;
    p->m018 = 0;
    p->mpEmitter2 = 0;
    p->mpEmitter = 0;
    return p;
}

/* 024B9010 */
static void daTitle_proc_dt(daTitle_proc_c* p, s32 flags) {
    if (p != nullptr) {
        p->__vtbl = gabi::ea(&titleProcVtable);
        /* HD: no 2D screen / expanded heap any more */
        titleDestroySolidHeap(p->m_solid_heap);
        p->m_solid_heap = 0;
        titleTransformDestruct(&reinterpret_cast<BckConstructorHD*>(&p->mBckShip)->mTransformVtable,
                               0);               /* mBckShip's member destructor */
        mDoExt_invisibleModel_dt(p->mInvisible); /* mDoExt_invisibleModel::~ */
        titleDrawListDestruct(p, 0);             /* dDlst_base_c::~dDlst_base_c */
        if (flags & 1)
            operator_delete(p);
    }
}

/* 024B9150 */
static void daTitle_c_dt(daTitle_c* p, s32 flags) {
    if (p != nullptr) {
        daTitle_proc_c* proc = p->mpTitleProc;
        p->__vtbl = gabi::ea(&titleActorVtable);
        if (proc != 0) { /* delete mpTitleProc (virtual) */
            deleteProc(proc);
        }
        dComIfG_resDelete(&p->mPhs, "Tlogo");
        titleActorDestruct(p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}

/* 024B909C */
static void __sinit_d_a_title_cpp() { __sinit_header_statics(); }

/* 024B9138 */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 024B91E0 */
static void SafeString_assureTerminationImpl(void*) {}
/* WWHD glue
str 100401F0 "Tlogo"
str 100400B4 "Tlogo" in 024B8F44
str 100400BC "Tlogo" in 024B9150
str 10040294 "title" renamed
str 1004024C "d_a_title.cpp"
str 10040220 "m_solid_heap != (0)"
str 1004025C "modelData_ship != (0)"
str 100402A0 "mModel_ship != (0)"
str 100401E8 "rt == 1"
str 100402B4 "modelData_sub != (0)"
str 100402CC "mModel_subtitle != (0)"
str 100401F8 "modelData_kirari != (0)"
str 100402E4 "mModel_kirari != (0)"
str 10040274 "bck_ship != 0"
str 10040234 "ok_bck != 0"
str 10040284 "bpk_ship != 0"
str 10040240 "ok_bpk != 0"
str 100402FC "btk_sub != 0"
str 1004030C "ok_btk_subtitle != 0"
str 10040210 "btk_kirari != 0"
str 10040324 "ok_btk_kirari != 0"
str 1004018C "d_a_title.cpp" in 024B87A4
str 1004019C "stageProc != (0)"
data titleAttrValues 10040144
data titleAttrVtable 10040358
data titleProcVtable 10040340
data titleActorVtable 1004017C
data titleTransformBaseVtable 1016E54C
data titleMtxCalcVtable 1016D820
data titleBckVtable 1004011C
data titleLayout 101F83FC
data titlePad 101F5088
data titlePadStatus 101F8378
data titleReset 101F4974
data titleRoomProcId 1047E6C4
data titleKirakiraSoundFlag 101D5F49
data titleBlackColor 101D5E94
data titleJ3dSys 104B45C0
data titleViewportIndex 104A1F70
data titleRenderer 101F95D0
data titleRuntimeType 101FDCCC
data titleRuntimeGuard 101FD7D4
data titleRuntimeVtable 1004010C
data titleDrawChannels 104A01EC
ext 025E3868 void titleDestroySolidHeap(HeapHD* heap)
ext 025E1988 void titleStartSound(u32 id)
ext 025DB678 void titleSetPaneAlpha(void* pane)
ext 025DBE00 BOOL titleIsOverlayPeek()
ext 025DC80C void* titleSearchScene(u32 id)
ext 025DC86C BOOL titleChangeScene(void* scene, s16 name, s16 p2, u16 p3, u32 p4)
ext 025F05F8 void titleSetFadeColor(const GXColor* color)
ext 026184D4 u32 titleConsumeTrigger(PadHD* pad, u32 mask)
ext 025E3630 HeapHD* titleCreateSolidHeap(u32 size, u32 align)
ext 025E37D8 void titleRestoreHeap()
ext 025E8EC0 void titleInvisibleEntryDL(void* invisible)
ext 0252CCBC void titleDrawListConstruct(daTitle_proc_c* proc)
ext 0252CCFC void titleDrawListDestruct(daTitle_proc_c* proc, s32 flags)
ext 027DA984 void titleMtxCalcConstruct(void* calc)
ext 027F3628 void titleTransformDestruct(void* transform, s32 flags)
ext 025E74FC BOOL titleBpkInit(mDoExt_bpkAnm* anm, J3DModelData* data, void* resource, s32 play, s32 mode, f32 speed, s32 start, s32 end, s32 modify, s32 flags)
ext 025D50BC void titleActorDestruct(daTitle_c* actor, s32 flags)
ext 0271F0E0 void titleLayoutStartIntro(LayoutHD* layout)
ext 0271F0F0 void titleLayoutEnterGame(LayoutHD* layout)
ext 0271F194 LayoutPositionHD* titleLayoutSmokePosition(LayoutHD* layout)
ext 0271F19C f32 titleLayoutSmokeScale(LayoutHD* layout)
ext 0271F1A4 f32 titleLayoutSmokeParticleScale(LayoutHD* layout)
ext 0271F1AC LayoutPositionHD* titleLayoutKiraPosition(LayoutHD* layout)
ext 0271F1B4 f32 titleLayoutKiraScale(LayoutHD* layout)
ext 0271F1BC f32 titleLayoutKiraParticleScale(LayoutHD* layout)
ext 0271F1C4 LayoutPositionHD* titleLayoutWindPosition(LayoutHD* layout)
ext 0271F1CC f32 titleLayoutWindScale(LayoutHD* layout)
ext 0271F1D4 f32 titleLayoutWindParticleScale(LayoutHD* layout)
ext 0271F1DC s32 titleLayoutWindInterval(LayoutHD* layout)
ext 0271F1E4 f32 titleLayoutParticleDepth(LayoutHD* layout)
ext 0271F1EC s32 titleLayoutKiraStart(LayoutHD* layout)
ext 0271F1F4 s32 titleLayoutKiraEnd(LayoutHD* layout)
*/
