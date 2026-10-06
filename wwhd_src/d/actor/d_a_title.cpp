/**
 * d_a_title.cpp (WWHD)
 * Title Screen manager & logo (3D ship; HD: the 2D logo lives in an HD title layout object)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_title.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define ARCNAME_CREATE STR(0x100400B4) /* "Tlogo" (each use has its own literal) */
#define ARCNAME_DELETE STR(0x100400BC)
#define ARCNAME_RES STR(0x100401F0)
#define SAFESTRING_VTBL 0x100400C4
#define TITLE_VTBL 0x1004017C
#define TITLE_PROC_VTBL 0x10040340
#define TITLE_ATTR_VTBL 0x10040358
#define FILE_NAME STR(0x1004024C)

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void mDoAud_seStart_1(u32 id) { gabi::call(0x025E1988, id); } /* HD: seStart(id) */
static inline void fopMsgM_setAlpha(void* pane) { gabi::call(0x025DB678, pane); }
static inline BOOL fopOvlpM_IsPeek() { return gabi::call<BOOL>(0x025DBE00); }
static inline void* fopScnM_SearchByID(u32 id) { return gabi::call<void*>(0x025DC80C, id); }
/* 025DC86C fopScnM_ChangeReq(scene, procName, p2, p3) (HD: a fifth argument) */
static inline BOOL fopScnM_ChangeReq(void* scene, s16 name, s16 p2, u16 p3, u32 p4) {
    return gabi::call<BOOL>(0x025DC86C, scene, name, p2, p3, p4);
}
/* 025F05F8 mDoGph_gInf_c::setFadeColor(JUtility::TColor&) */
static inline void mDoGph_setFadeColor(u32 color) { gabi::call(0x025F05F8, color); }
/* 026184D4: pad trigger consume (pad object at *0x101F5088, mask) -> u8 stored at *(0x101F8378)+0x1C */
static inline u32 pad_026184D4(u32 pad, u32 mask) { return gabi::call<u32>(0x026184D4, pad, mask); }
static inline u32 mDoExt_createSolidHeapFromGameToCurrent(u32 size, u32 align) { return gabi::call<u32>(0x025E3630, size, align); }
static inline void mDoExt_restoreCurrentHeap() { gabi::call(0x025E37D8); }
static inline void mDoExt_destroySolidHeap(u32 heap) { gabi::call(0x025E3868, heap); }
static inline BOOL mDoExt_invisibleModel_create(void* inv, J3DModel* model) { return gabi::call<BOOL>(0x025E8A48, inv, model); }
static inline void mDoExt_invisibleModel_entryDL(void* inv) { gabi::call(0x025E8EC0, inv); }
static inline void mDoExt_modelUpdateDL2(J3DModel* m, u32 buf) { gabi::call(0x025E2DE0, m, buf); } /* HD: draw buffer */
/* HD title 2D layout object (*0x101F83FC); its getters forward to the layout at +0x50 */
static inline u32 title2d() { return gabi::load<u32>(0x101F83FC); }
static inline void t2d_call(u32 fn, u32 o) { gabi::call(fn, o); }
static inline u32 t2d_ptr(u32 fn, u32 o) { return gabi::call<u32>(fn, o); }
static inline f32 t2d_f(u32 fn, u32 o) { return gabi::call<f32>(fn, o); }
static inline s32 t2d_i(u32 fn, u32 o) { return gabi::call<s32>(fn, o); }

/* HD: L_attr became an object (vtable + the 14 floats, copied from 0x10040144) inside the proc */
struct daTitle_Attr_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<f32> field_0x00;
    /* 0x08 */ be<f32> field_0x04;
    /* 0x0C */ be<f32> field_0x08;
    /* 0x10 */ be<f32> field_0x0C;
    /* 0x14 */ be<f32> field_0x10;
    /* 0x18 */ be<f32> field_0x14;
    /* 0x1C */ be<f32> field_0x18;
    /* 0x20 */ be<f32> field_0x1C;
    /* 0x24 */ be<f32> field_0x20;
    /* 0x28 */ be<f32> field_0x24;
    /* 0x2C */ be<f32> field_0x28;
    /* 0x30 */ be<f32> field_0x2C;
    /* 0x34 */ be<f32> field_0x30;
    /* 0x38 */ be<f32> field_0x34;
};
WWHD_SIZE(daTitle_Attr_c, 0x3C);

struct daTitle_proc_c {
    void proc_init3D();
    void setEnterMode();
    void set_mtx();
    void calc_2d_alpha();
    void proc_execute();
    void model_draw();
    s32 getEnterMode() { return mEnterMode; }

    /* 0x000 */ be<u32> __vtbl;          /* dDlst_base_c */
    /* 0x004 */ daTitle_Attr_c mAttr;
    /* 0x040 */ be<u32> mpEmitter;
    /* 0x044 */ be<u32> mpEmitter2;
    /* 0x048 */ cXyz m00C;
    /* 0x054 */ be<s32> m018;
    /* 0x058 */ be<s32> m01C;
    /* 0x05C */ be<s32> m020;
    /* 0x060 */ be<s32> m024;
    /* 0x064 */ be<s32> m028;
    /* 0x068 */ be<s32> m02C;
    /* 0x06C */ be<s32> mEnterMode;
    /* 0x070 */ u8 m034[4];
    /* 0x074 */ gptr<J3DModel> mModel_ship;
    /* 0x078 */ gptr<J3DModel> mModel_subtitle;
    /* 0x07C */ gptr<J3DModel> mModel_kirari;
    /* 0x080 */ u8 mInvisible[8];        /* HD: mDoExt_invisibleModel */
    /* 0x088 */ mDoExt_bckAnm mBckShip;
    /* 0x114 */ u8 mBpkShip[0x74];       /* HD: anm class with ctor 025E7480 / init 025E74FC */
    /* 0x188 */ mDoExt_btkAnm mBtkSub;
    /* 0x1FC */ mDoExt_btkAnm mBtkKirari;
    /* 0x270 */ be<u8> m090;
    /* 0x271 */ u8 _271[3];
    /* 0x274 */ be<f32> m094;
    /* 0x278 */ be<s32> m098;
    /* 0x27C */ be<u32> m_Screen;
    /* 0x280 */ be<u32> m0A0[6];
    /* 0x298 */ u8 pane[6][0x38];        /* fopMsgM_pane_class */
    /* 0x3E8 */ be<u32> m_exp_heap;
    /* 0x3EC */ be<u32> m_solid_heap;

    J3DFrameCtrl& bpkFrameCtrl() { return *gabi::at<J3DFrameCtrl>(gabi::ea(mBpkShip)); }
};
WWHD_OFFSET(daTitle_proc_c, mEnterMode, 0x6C);
WWHD_OFFSET(daTitle_proc_c, mBckShip, 0x88);
WWHD_OFFSET(daTitle_proc_c, m094, 0x274);
WWHD_OFFSET(daTitle_proc_c, pane, 0x298);
WWHD_SIZE(daTitle_proc_c, 0x3F0);

struct daTitle_c : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<daTitle_proc_c> mpTitleProc;
    /* 0x3B8 */ be<u8> m29C;
};
WWHD_OFFSET(daTitle_c, m29C, 0x3B8);

/* 024B8964: the attribute object's values (14 words from 0x10040144) */
static void daTitle_Attr_init(daTitle_Attr_c* p) {
    WWHD_FUNC(0x024B8964, void, p);
    u32 d = gabi::ea(p), s = 0x10040140;
    for (int i = 0; i < 7; i++) {
        gabi::store<u32>(d + 4 + 8 * i, gabi::load<u32>(s + 4 + 8 * i));
        gabi::store<u32>(d + 8 + 8 * i, gabi::load<u32>(s + 8 + 8 * i));
    }
}
VERIFY(0x024B8964, daTitle_Attr_init);

/* 024B898C: attribute object constructor (allocates when this == NULL) */
static daTitle_Attr_c* daTitle_Attr_ct(daTitle_Attr_c* p) {
    WWHD_FUNC(0x024B898C, daTitle_Attr_c*, p);
    if (p == nullptr) {
        p = (daTitle_Attr_c*)operator_new(0x3C);
        if (p == nullptr)
            return nullptr;
    }
    p->__vtbl = TITLE_ATTR_VTBL;
    daTitle_Attr_init(p);
    return p;
}
VERIFY(0x024B898C, daTitle_Attr_ct);

/* 024B89D8: daTitle_proc_c::daTitle_proc_c() */
static daTitle_proc_c* daTitle_proc_ct(daTitle_proc_c* p) {
    WWHD_FUNC(0x024B89D8, daTitle_proc_c*, p);
    if (p == nullptr) {
        p = (daTitle_proc_c*)operator_new(0x3F0);
        if (p == nullptr)
            return nullptr;
    }
    u32 a = gabi::ea(p);
    gabi::call(0x0252CCBC, p); /* dDlst_base_c::dDlst_base_c */
    p->__vtbl = TITLE_PROC_VTBL;
    daTitle_Attr_ct(&p->mAttr);
    gabi::call(0x025E895C, p->mInvisible); /* mDoExt_invisibleModel::mDoExt_invisibleModel */
    /* mDoExt_bckAnm mBckShip */
    gabi::call(0x027F2BC0, &p->mBckShip, 0); /* J3DFrameCtrl::init */
    gabi::store<u32>(a + 0x98, 0x1016E54C);
    gabi::call(0x027DA984, gabi::at<u8>(a + 0x9C));
    gabi::store<u32>(a + 0xD0, 0x1016D820);
    gabi::store<u32>(a + 0x98, 0x1004011C);
    gabi::store<u32>(a + 0xE0, 0);
    gabi::store<u32>(a + 0x104, 0);
    gabi::store<u32>(a + 0x108, 0);
    gabi::store<u32>(a + 0x10C, 0);
    gabi::store<u32>(a + 0x110, 0);
    gabi::call(0x025E7480, p->mBpkShip);
    mDoExt_btkAnm::ct(&p->mBtkSub);
    mDoExt_btkAnm::ct(&p->mBtkKirari);
    p->m_solid_heap = 0;
    p->m098 = -50;
    p->m01C = 120;
    p->m_exp_heap = 0;
    /* HD: m094 = (m098 * m098) * attr().field_0x0C (GameCube: * -field_0x0C) */
    p->m094 = 2500.0f * p->mAttr.field_0x0C;
    p->mEnterMode = 0;
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
VERIFY(0x024B89D8, daTitle_proc_ct);

/* 024B9010: daTitle_proc_c deleting destructor (vtable 0x10040340) */
static void daTitle_proc_dt(daTitle_proc_c* p, s32 flags) {
    WWHD_FUNC(0x024B9010, void, p, flags);
    if (p != nullptr) {
        p->__vtbl = TITLE_PROC_VTBL;
        /* HD: no 2D screen / expanded heap any more */
        mDoExt_destroySolidHeap(p->m_solid_heap);
        p->m_solid_heap = 0;
        gabi::call(0x027F3628, gabi::at<u8>(gabi::ea(p) + 0x98), 0); /* mBckShip's member destructor */
        gabi::call(0x025E89F8, p->mInvisible, 2);                     /* mDoExt_invisibleModel::~ */
        gabi::call(0x0252CCFC, p, 0);                                 /* dDlst_base_c::~dDlst_base_c */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x024B9010, daTitle_proc_dt);

/* 024B8B44 */
void daTitle_proc_c::proc_init3D() {
    WWHD_FUNC(0x024B8B44, void, this);
    m_solid_heap = mDoExt_createSolidHeapFromGameToCurrent(0x40000U, 0x20);
    if (m_solid_heap == 0) /* JUT_ASSERT(0xC0, m_solid_heap != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0xC0, STR(0x10040220));
    gabi::store<u32>(m_solid_heap + 0x10, 0x10040294); /* HD: heap name */

    J3DModelData* modelData_ship = (J3DModelData*)dComIfG_getObjectRes(ARCNAME_RES, 0xD, SAFESTRING_VTBL);
    if (modelData_ship == NULL)
        JUT_ASSERT_fail(FILE_NAME, 0xC9, STR(0x1004025C));

    mModel_ship = mDoExt_J3DModel__create(modelData_ship, 0x80000U, 0x37441423U);
    if (!mModel_ship)
        JUT_ASSERT_fail(FILE_NAME, 0xCE, STR(0x100402A0));

    /* HD */
    if (mDoExt_invisibleModel_create(mInvisible, mModel_ship) != 1)
        JUT_ASSERT_fail(FILE_NAME, 0xD3, STR(0x100401E8));

    J3DModelData* modelData_sub = (J3DModelData*)dComIfG_getObjectRes(ARCNAME_RES, 0xC, SAFESTRING_VTBL);
    if (modelData_sub == NULL)
        JUT_ASSERT_fail(FILE_NAME, 0xD8, STR(0x100402B4));

    mModel_subtitle = mDoExt_J3DModel__create(modelData_sub, 0x80000U, 0x37441422U);
    if (!mModel_subtitle)
        JUT_ASSERT_fail(FILE_NAME, 0xDD, STR(0x100402CC));

    J3DModelData* modelData_kirari = (J3DModelData*)dComIfG_getObjectRes(ARCNAME_RES, 0xB, SAFESTRING_VTBL);
    if (modelData_kirari == NULL)
        JUT_ASSERT_fail(FILE_NAME, 0xE1, STR(0x100401F8));

    mModel_kirari = mDoExt_J3DModel__create(modelData_kirari, 0x80000U, 0x37441422U);
    if (!mModel_kirari)
        JUT_ASSERT_fail(FILE_NAME, 0xE6, STR(0x100402E4));

    J3DAnmTransform* bck_ship = (J3DAnmTransform*)dComIfG_getObjectRes(ARCNAME_RES, 8, SAFESTRING_VTBL);
    if (bck_ship == NULL)
        JUT_ASSERT_fail(FILE_NAME, 0xEB, STR(0x10040274));

    BOOL ok_bck = mBckShip.init(modelData_ship, bck_ship, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false);
    if (ok_bck == FALSE)
        JUT_ASSERT_fail(FILE_NAME, 0xF1, STR(0x10040234));

    u32 bpk_ship = gabi::ea(dComIfG_getObjectRes(ARCNAME_RES, 0x10, SAFESTRING_VTBL));
    if (bpk_ship == 0)
        JUT_ASSERT_fail(FILE_NAME, 0xF6, STR(0x10040284));

    BOOL ok_bpk = gabi::call<BOOL>(0x025E74FC, mBpkShip, modelData_ship, bpk_ship, 1, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 0, 0);
    if (ok_bpk == FALSE)
        JUT_ASSERT_fail(FILE_NAME, 0xFC, STR(0x10040240));

    bpkFrameCtrl().setRate(1.0f);
    bpkFrameCtrl().setFrame(0.0f);

    J3DAnmTextureSRTKey* btk_sub = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(ARCNAME_RES, 0x14, SAFESTRING_VTBL);
    if (btk_sub == NULL)
        JUT_ASSERT_fail(FILE_NAME, 0x103, STR(0x100402FC));

    BOOL ok_btk_subtitle = mBtkSub.init(modelData_sub, btk_sub, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0);
    if (ok_btk_subtitle == FALSE)
        JUT_ASSERT_fail(FILE_NAME, 0x10A, STR(0x1004030C));

    J3DAnmTextureSRTKey* btk_kirari = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(ARCNAME_RES, 0x13, SAFESTRING_VTBL);
    if (btk_kirari == NULL)
        JUT_ASSERT_fail(FILE_NAME, 0x10F, STR(0x10040210));

    BOOL ok_btk_kirari = mBtkKirari.init(modelData_kirari, btk_kirari, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0);
    if (ok_btk_kirari == FALSE)
        JUT_ASSERT_fail(FILE_NAME, 0x115, STR(0x10040324));

    mDoExt_restoreCurrentHeap();
    set_mtx();
}
VERIFY(0x024B8B44, &daTitle_proc_c::proc_init3D);

/* 024B7CE4 */
void daTitle_proc_c::setEnterMode() {
    WWHD_FUNC(0x024B7CE4, void, this);
    if (mEnterMode == 1) {
        mEnterMode = 2;
    }
}
VERIFY(0x024B7CE4, &daTitle_proc_c::setEnterMode);

/* 024B83F4 */
void daTitle_proc_c::set_mtx() {
    WWHD_FUNC(0x024B83F4, void, this);
    f32 s = mAttr.field_0x08; /* mModel_ship->setBaseScale(scale) */
    u32 m = gabi::ea(mModel_ship);
    gabi::store<f32>(m + 0xC4, s);
    gabi::store<f32>(m + 0xC0, s);
    gabi::store<f32>(m + 0xBC, s);

    /* HD: z 100 (GameCube 1000), rotation -0x4000 (GameCube 0x4000), m094 added */
    mDoMtx_stack_c::transS(mAttr.field_0x00 + m094, mAttr.field_0x04, 100.0f);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), 0, -0x4000, 0);
    J3DModel_setBaseTRMtx(mModel_ship, mDoMtx_stack_c::get());

    f32 sx = mAttr.field_0x18, sy = mAttr.field_0x1C;
    m = gabi::ea(mModel_subtitle);
    gabi::store<f32>(m + 0xBC, sx);
    gabi::store<f32>(m + 0xC0, sy);
    gabi::store<f32>(m + 0xC4, 1.0f);
    m = gabi::ea(mModel_kirari);
    gabi::store<f32>(m + 0xBC, sx);
    gabi::store<f32>(m + 0xC0, sy);
    gabi::store<f32>(m + 0xC4, 1.0f);

    f32 px = mAttr.field_0x10, py = mAttr.field_0x14;
    mDoMtx_stack_c::transS(px, py, -10000.0f);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), 0, -0x8000, 0);
    J3DModel_setBaseTRMtx(mModel_subtitle, mDoMtx_stack_c::get());

    mDoMtx_stack_c::transS(px, py, -10010.0f);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), 0, -0x8000, 0);
    J3DModel_setBaseTRMtx(mModel_kirari, mDoMtx_stack_c::get());
}
VERIFY(0x024B83F4, &daTitle_proc_c::set_mtx);

/* m018 > v, with v computed first (m018 is loaded after the call) */
static inline bool seq_gt(s32 v, be<s32>& m018) { return m018 > v; }

/* sets an emitter's global translation (HD: y is negated when the emitter's byte +0x262 >= 7) */
static void emitter_setGlobalTranslation(u32 e, f32 x, f32 y, f32 z) {
    if (gabi::load<u8>(e + 0x262) >= 7)
        y = -y;
    gabi::store<f32>(e + 0x22C, x);
    gabi::store<f32>(e + 0x230, y);
    gabi::store<f32>(e + 0x234, z);
}

/* 024B7CFC. HD: rewritten around the HD title layout object; the 2D alpha fades moved there */
void daTitle_proc_c::calc_2d_alpha() {
    WWHD_FUNC(0x024B7CFC, void, this);
    s32 cnt = m018 + 1;
    s32 mode = mEnterMode;
    u32 t2d = title2d();
    m018 = cnt;
    if (mode == 0) {
        if (cnt >= 200) {
            mEnterMode = 1;
        } else {
            if (cnt == 30)
                t2d_call(0x0271F0E0, title2d());
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
        if (t2d_i(0x0271F1DC, t2d) != 0)
            m020 = t2d_i(0x0271F1DC, t2d);
        f32 x = gabi::load<f32>(t2d_ptr(0x0271F1C4, t2d) + 0);
        f32 y = gabi::load<f32>(t2d_ptr(0x0271F1C4, t2d) + 4);
        f32 z = t2d_f(0x0271F1E4, t2d);
        gabi::Local<cXyz> pos;
        pos->z = z;
        pos->y = y;
        pos->x = x;
        gabi::Local<csXyz> angle;
        s16 ax = (s16)gabi::ftoi(cM_rndFX(32768.0f));
        angle->y = 0;
        angle->x = ax;
        angle->z = (s16)gabi::ftoi(cM_rndFX(4000.0f));
        u32 e = gabi::ea(dPa_control_set(dComIfGp_getParticle(), 8, 0x83FA /* ID_AK_S1_TITLEWIND00 */, pos.get(), angle.get(),
                                         nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr));
        f32 s = t2d_f(0x0271F1CC, t2d);
        gabi::store<f32>(e + 0x238, s);
        gabi::store<f32>(e + 0x23C, s);
        gabi::store<f32>(e + 0x240, s);
        s = t2d_f(0x0271F1D4, t2d);
        gabi::store<f32>(e + 0x220, s);
        gabi::store<f32>(e + 0x224, s);
        gabi::store<f32>(e + 0x228, s);
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

    if (mEnterMode == 0) {
        u32 e = mpEmitter;
        f32 x = gabi::load<f32>(t2d_ptr(0x0271F194, t2d) + 0);
        x = x + m094;
        x = x + mAttr.field_0x30;
        f32 y = gabi::load<f32>(t2d_ptr(0x0271F194, t2d) + 4);
        y = y + mAttr.field_0x34;
        f32 z = t2d_f(0x0271F1E4, t2d);
        gabi::Local<cXyz> pos;
        pos->x = x;
        pos->z = z;
        pos->y = y;
        if (e == 0) {
            mpEmitter = gabi::ea(dPa_control_set(dComIfGp_getParticle(), 8, 0x83F9 /* ID_AK_S1_TITLESMOKE00 */, pos.get(), nullptr,
                                                 nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr));
            f32 s = t2d_f(0x0271F19C, t2d);
            gabi::store<f32>(mpEmitter + 0x238, s);
            gabi::store<f32>(mpEmitter + 0x23C, s);
            gabi::store<f32>(mpEmitter + 0x240, s);
            s = t2d_f(0x0271F1A4, t2d);
            u32 e1 = mpEmitter;
            gabi::store<f32>(e1 + 0x220, s);
            gabi::store<f32>(e1 + 0x224, s);
            gabi::store<f32>(e1 + 0x228, s);
        } else {
            emitter_setGlobalTranslation(mpEmitter, x, y, z);
        }

        s32 kiraStart = t2d_i(0x0271F1EC, t2d);
        if ((u32)m018 == (u32)kiraStart) {
            if (gabi::load<u8>(0x101D5F49) == 1) { /* daTitle_Kirakira_Sound_flag */
                mDoAud_seStart_1(0x909 /* JA_SE_TITLE_KIRA */);
                gabi::store<u8>(0x101D5F49, 0);
            }
            f32 kx = gabi::load<f32>(t2d_ptr(0x0271F1AC, t2d) + 0);
            f32 ky = gabi::load<f32>(t2d_ptr(0x0271F1AC, t2d) + 4);
            m00C.x = kx;
            m00C.y = ky;
            m00C.z = 0.0f;
            mpEmitter2 = gabi::ea(dPa_control_set(dComIfGp_getParticle(), 7, 0x83FB /* ID_AK_S2_TITLEKIRAKIRA00 */, &m00C, nullptr,
                                                  nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr));
            f32 s = t2d_f(0x0271F1B4, t2d);
            u32 e2 = mpEmitter2;
            gabi::store<f32>(e2 + 0x238, s);
            gabi::store<f32>(e2 + 0x23C, s);
            gabi::store<f32>(e2 + 0x240, 1.0f);
            s = t2d_f(0x0271F1BC, t2d);
            e2 = mpEmitter2;
            gabi::store<f32>(e2 + 0x220, s);
            gabi::store<f32>(e2 + 0x224, s);
            gabi::store<f32>(e2 + 0x228, 1.0f);
        } else if (seq_gt(t2d_i(0x0271F1EC, t2d), m018) && !seq_gt(t2d_i(0x0271F1F4, t2d), m018) && mpEmitter2 != 0) {
            s32 end = t2d_i(0x0271F1F4, t2d);
            s32 start = t2d_i(0x0271F1EC, t2d);
            m00C.x = m00C.x + 320.0f / (f32)(end - start);
            emitter_setGlobalTranslation(mpEmitter2, m00C.x, m00C.y, m00C.z);
        }
    } else {
        if (mpEmitter == 0) {
            f32 x = gabi::load<f32>(t2d_ptr(0x0271F194, t2d) + 0);
            f32 y = gabi::load<f32>(t2d_ptr(0x0271F194, t2d) + 4);
            f32 z = t2d_f(0x0271F1E4, t2d);
            gabi::Local<cXyz> pos;
            pos->x = x;
            pos->z = z;
            pos->y = y;
            u32 e = gabi::ea(dPa_control_set(dComIfGp_getParticle(), 8, 0x83F9 /* ID_AK_S1_TITLESMOKE00 */, pos.get(), nullptr,
                                             nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr));
            u32 e2 = mpEmitter2;
            mpEmitter = e;
            if (e2 == 0)
                return;
        } else {
            f32 x = gabi::load<f32>(t2d_ptr(0x0271F194, t2d) + 0);
            x = x - m094;
            x = x + mAttr.field_0x30;
            f32 y = gabi::load<f32>(t2d_ptr(0x0271F194, t2d) + 4);
            y = y + mAttr.field_0x34;
            f32 z = t2d_f(0x0271F1E4, t2d);
            emitter_setGlobalTranslation(mpEmitter, x, y, z);
            if (mpEmitter2 == 0)
                return;
        }
        u32 e2 = mpEmitter2; /* mpEmitter2->becomeInvalidEmitter() */
        u32 fl = gabi::load<u32>(e2 + 0x254);
        gabi::store<s32>(e2 + 0x5C, -1);
        gabi::store<u32>(e2 + 0x254, fl | 1);
        mpEmitter2 = 0;
    }
}
VERIFY(0x024B7CFC, &daTitle_proc_c::calc_2d_alpha);

/* 024B8670 */
void daTitle_proc_c::proc_execute() {
    WWHD_FUNC(0x024B8670, void, this);
    /* HD: no expanded-heap switch */
    if (m01C > 0) {
        m01C = m01C - 1;

        if (m01C == 0) {
            mDoAud_seStart_1(0x90B /* JA_SE_TITLE_WIND */);
        }
    } else {
        calc_2d_alpha();
    }

    u32 pad = gabi::load<u32>(0x101F5088);
    if (mEnterMode == 0 && (gabi::load<u32>(pad + 0x18) & 0x861B) != 0) { /* trigger A / B / START / ... */
        m098 = 0;
        m01C = 0;
        u32 o = gabi::load<u32>(0x101F8378);
        gabi::store<u8>(o + 0x1C, (u8)pad_026184D4(pad, 0x861B));
        mEnterMode = 3; /* HD: skips mode 1/2 */
    }

    for (int paneIdx = 0; paneIdx < 4; paneIdx++) {
        fopMsgM_setAlpha(pane[paneIdx]);
    }

    if (mEnterMode == 2) {
        mEnterMode = 3;
    } else if (mEnterMode == 3) {
        m098 = m098 + 1;
    }

    mBckShip.play();
    set_mtx();
}
VERIFY(0x024B8670, &daTitle_proc_c::proc_execute);

/* 024B7A6C */
void daTitle_proc_c::model_draw() {
    WWHD_FUNC(0x024B7A6C, void, this);
    /* HD: the draw target comes from a viewport/layer table, dynamic-cast through sead RTTI */
    u32 j3dSys = 0x104B45C0;
    u32 idxp = gabi::load<u32>(0x104A1F70);
    s16 idx = gabi::load<s16>(idxp);
    u32 tbl = gabi::load<u32>(j3dSys + 0x148);
    u32 count = gabi::load<u32>(tbl + 8);
    u32 arr = gabi::load<u32>(tbl + 0xC);
    u32 ent = (u32)idx < count ? arr + idx * 4 : arr;
    u32 obj = 0;
    if (gabi::load<u16>(ent + 2) != 0) {
        u32 count2 = gabi::load<u32>(tbl + 0x10);
        u32 ent2 = (u32)idx < count ? arr + idx * 4 : arr;
        u16 k = gabi::load<u16>(ent2);
        u32 slot = 0;
        if (k < count2)
            slot = gabi::load<u32>(tbl + 0x14) + k * 4;
        obj = gabi::load<u32>(slot);
    }
    /* function-local static RTTI object (guard 0x101FD7D4, object 0x101FDCCC) */
    if (gabi::load<u32>(0x101FD7D4) == 0) {
        gabi::store<u32>(0x101FD7D4, 1);
        gabi::store<u32>(0x101FDCCC, 0x1004010C);
    }
    if (obj != 0) {
        u32 fn = gabi::load<u32>(gabi::load<u32>(obj + 0x58) + 0x44); /* checkDerivedRuntimeTypeInfo */
        if (gabi::call_ptr<u32>(fn, obj, 0x101FDCCC) == 0)
            obj = 0;
    } else {
        obj = 0;
    }
    for (u32 i = 0; i < 16; i += 4)
        gabi::store<u32>(obj + 0xF4 + i, gabi::load<u32>(0x104A01EC + i));
    gabi::store<f32>(obj + 0x130, 0.0f);
    gabi::store<f32>(obj + 0x12C, 35000.0f);
    gabi::store<f32>(obj + 0x134, 30000.0f);

    /* dComIfGd_setList2D() */
    gabi::store<u32>(j3dSys + 0x74, gabi::load<u32>(dComIfGp_ea() + 0x5D98));
    gabi::store<u32>(j3dSys + 0x78, gabi::load<u32>(dComIfGp_ea() + 0x5D98));

    /* HD: only the ship is drawn here */
    if (bpkFrameCtrl().getFrame() != 0.0f) {
        u32 data = gabi::load<u32>(gabi::ea(mModel_ship) + 0xAC);
        u32 g = gabi::load<u32>(0x101F95D0);
        u32 n = gabi::load<u32>(g + 0x1020);
        u32 bufp = gabi::load<u32>(g + 0x1024);
        if (n > 3)
            bufp += 0xC;
        u32 buf = gabi::load<u32>(bufp);
        mBckShip.entry(gabi::at<J3DModelData>(data), mBckShip.mFrameCtrl.getFrame());
        gabi::call(0x025E779C, mBpkShip, gabi::load<u32>(gabi::ea(mModel_ship) + 0xAC), bpkFrameCtrl().getFrame());
        mDoExt_modelUpdateDL2(mModel_ship, buf);
        mDoExt_invisibleModel_entryDL(mInvisible);
        gabi::store<u32>(gabi::load<u32>(gabi::ea(mModel_ship) + 0xAC) + 0x40, 0); /* mBpkShip.remove */
        gabi::store<u32>(gabi::load<u32>(gabi::load<u32>(gabi::ea(mModel_ship) + 0xAC) + 8) + 0x14, 0); /* mBckShip.remove */
    }

    /* dComIfGd_setList() */
    gabi::store<u32>(j3dSys + 0x74, gabi::load<u32>(dComIfGp_ea() + 0x5D78));
    gabi::store<u32>(j3dSys + 0x78, gabi::load<u32>(dComIfGp_ea() + 0x5D7C));
}
VERIFY(0x024B7A6C, &daTitle_proc_c::model_draw);

/* 024B914C: daTitle_proc_c::draw() (HD: empty; the 2D screen is drawn elsewhere) */
static void daTitle_proc_draw(daTitle_proc_c*) {
    WWHD_FUNC(0x024B914C, void, (u32)0);
}
VERIFY(0x024B914C, daTitle_proc_draw);

/* 024B7CBC: daTitle_c::draw() inlined. HD: no dComIfGd_set2DOpa */
static BOOL daTitle_Draw(daTitle_c* i_this) {
    WWHD_FUNC(0x024B7CBC, BOOL, i_this);
    i_this->mpTitleProc->model_draw();
    return TRUE;
}
VERIFY(0x024B7CBC, daTitle_Draw);

/* 024B87A4: daTitle_c::execute() inlined */
static BOOL daTitle_Execute(daTitle_c* i_this) {
    WWHD_FUNC(0x024B87A4, BOOL, i_this);
    if (!fopOvlpM_IsPeek()) {
        mDoGph_setFadeColor(0x101D5E94 /* g_blackColor */);

        daTitle_proc_c* proc = i_this->mpTitleProc;
        u32 pad = gabi::load<u32>(0x101F5088);
        if (proc->mEnterMode == 1 && (gabi::load<u32>(pad + 0x18) & 0x861B) != 0) {
            u32 o = gabi::load<u32>(0x101F8378);
            gabi::store<u8>(o + 0x1C, (u8)pad_026184D4(pad, 0x861B));
            i_this->mpTitleProc->setEnterMode();
        } else if (proc->getEnterMode() == 3) {
            void* stageProc = fopScnM_SearchByID(gabi::load<u32>(0x1047E6C4) /* dStage_roomControl_c::getProcID() */);
            if (stageProc == NULL)
                JUT_ASSERT_fail(STR(0x1004018C), 0x44A, STR(0x1004019C));

            if (!i_this->m29C && fopScnM_ChangeReq(stageProc, 9 /* fpcNm_NAME_SCENE_e */, 0, 5, 1)) {
                t2d_call(0x0271F0F0, title2d()); /* HD */
                mDoAud_seStart_1(0x82F /* JA_SE_OP_ENTER_GAME */);
                i_this->m29C = true;
            }
        } else if (gabi::load<u32>(gabi::load<u32>(0x101F4974)) == 0 /* !mDoRst::isReset() */ &&
                   gabi::load<s8>(dComIfGp_ea() + 0x514C) != 0 /* dComIfGp_isEnableNextStage() */) {
            void* stageProc = fopScnM_SearchByID(gabi::load<u32>(0x1047E6C4));
            if (stageProc == NULL)
                JUT_ASSERT_fail(STR(0x1004018C), 0x4A2, STR(0x1004019C));

            if (!i_this->m29C) {
                fopScnM_ChangeReq(stageProc, 0xD /* HD: always fpcNm_TITLE_SCENE_e */, 1, 5, 1);
                i_this->m29C = true;
            }
        }
    }

    i_this->mpTitleProc->proc_execute();
    return TRUE;
}
VERIFY(0x024B87A4, daTitle_Execute);

/* 024B9130 */
static BOOL daTitle_IsDelete(daTitle_c* i_this) {
    WWHD_FUNC(0x024B9130, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x024B9130, daTitle_IsDelete);

/* 024B895C. HD: the destructor work moved into the deleting destructor (024B9150) */
static BOOL daTitle_Delete(daTitle_c* i_this) {
    WWHD_FUNC(0x024B895C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x024B895C, daTitle_Delete);

/* 024B8F44: daTitle_c::create() inlined */
static cPhs_State daTitle_Create(fopAc_ac_c* a) {
    WWHD_FUNC(0x024B8F44, cPhs_State, a);
    daTitle_c* i_this = static_cast<daTitle_c*>(a);
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = TITLE_VTBL;
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&i_this->mPhs, ARCNAME_CREATE);

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
VERIFY(0x024B8F44, daTitle_Create);

/* 024B9150: daTitle_c deleting destructor (~daTitle_c inlined; vtable 0x1004017C) */
static void daTitle_c_dt(daTitle_c* p, s32 flags) {
    WWHD_FUNC(0x024B9150, void, p, flags);
    if (p != nullptr) {
        u32 proc = gabi::ea(p->mpTitleProc);
        p->__vtbl = TITLE_VTBL;
        if (proc != 0) { /* delete mpTitleProc (virtual) */
            gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(proc) + 0xC), proc, 3);
        }
        dComIfG_resDelete(&p->mPhs, ARCNAME_DELETE);
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x024B9150, daTitle_c_dt);

/* 024B909C: __sinit_d_a_title_cpp (compiler-generated: header statics only) */
static void __sinit_d_a_title_cpp() {
    WWHD_FUNC(0x024B909C, void);
    sinit_header_statics(0x1046E6FC, 0x101D23F4);
}
VERIFY(0x024B909C, __sinit_d_a_title_cpp);

/* 024B9138: sead::SafeString deleting destructor (this TU's copy, vtable 0x100400C4) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x024B9138, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x024B9138, SafeString_dt);

/* 024B91E0: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x024B91E0, void, (u32)0);
}
VERIFY(0x024B91E0, SafeString_assureTerminationImpl);
