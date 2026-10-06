/**
 * d_a_arrow_lighteff.cpp (WWHD)
 * Arrow light effect: the fire / ice / light glow attached to an elemental arrow.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_arrow_lighteff.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define LINK_ARC STR(0x100075C8) /* "Link" */
#define SAFESTRING_VTBL 0x10007598
#define LIGHTEFF_VTBL 0x100075B0
#define FILE_NAME STR(0x100075D0)

enum {
    dRes_INDEX_LINK_BDL_GARWFI00_e = 0x32,
    dRes_INDEX_LINK_BDL_GARWFI01_e = 0x33,
    dRes_INDEX_LINK_BDL_GARWG00_e = 0x34,
    dRes_INDEX_LINK_BRK_GARWFI00_e = 0x52,
    dRes_INDEX_LINK_BRK_GARWFI01_e = 0x53,
    dRes_INDEX_LINK_BRK_GARWG00_e = 0x54,
    dRes_INDEX_LINK_BTK_GARWFI00_e = 0x5E,
    dRes_INDEX_LINK_BTK_GARWFI01_e = 0x5F,
    dRes_INDEX_LINK_BTK_GARWG00_e = 0x60,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E80D0 mDoExt_brkAnm::mDoExt_brkAnm (matcher: "mDoExt_brkAnm::init") (as in d_a_obj_mkiek) */
static inline void mDoExt_brkAnm_ct(void* p) { gabi::call(0x025E80D0, p); }
/* 025E8154 mDoExt_brkAnm::init(data, key, anmPlay, mode, rate (f1), start, end, modify, entry (stack)) (as in d_a_obj_mkiek) */
static inline BOOL mDoExt_brkAnm_init(void* a, J3DModelData* d, void* key, s32 play, s32 mode, f32 rate, s16 start, s16 end,
                                      bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, key, play, mode, rate, start, end, modify, entry);
}
/* dComIfGp_particle_setP1: dPa_control_c::set with group 1 */
static inline JPABaseEmitter* dComIfGp_particle_setP1(u16 id, const cXyz* pos, const csXyz* angle, const cXyz* scale, u8 alpha,
                                                      dPa_levelEcallBack* cb, s8 setup, const GXColor* prm, const GXColor* env,
                                                      const cXyz* scale2D) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 1, id, pos, angle, scale, alpha, cb, setup, prm, env, scale2D);
}
/* fopAcM_seStartCurrent (HD inline: no null checks) (as in d_a_obj_mkiek) */
static inline void fopAcM_seStartCurrent(fopAc_ac_c* a, u32 id, u32 param) {
    mDoAud_seStart(id, &a->current.pos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* mDoGph_gInf_c::isMonotone(): the byte at 0x101F4829 (as in d_a_itembase) */
static inline bool mDoGph_isMonotone() { return gabi::load<u8>(0x101F4829) != 0; }
/* dComIfGp_getPlayerCameraID(0): play+0x5B30 (s8); dComIfGp_checkCameraAttentionStatus(id, flag):
 * the status word at play+0x5B00+id*0x34 */
static inline s32 dComIfGp_getPlayerCameraID0() { return gabi::load<s8>(dComIfGp_ea() + 0x5B30); }
static inline bool dComIfGp_checkCameraAttentionStatus(s32 id, u32 flag) {
    return (gabi::load<u32>(dComIfGp_ea() + id * 0x34 + 0x5B00) & flag) != 0;
}
/* JPABaseEmitter::setGlobalAlpha (HD +0x247), setGlobalScale (HD: +0x220.. and +0x238..) (as in d_a_kb.h) */
static inline void JPABaseEmitter_setGlobalAlpha(JPABaseEmitter* e, u8 a) { gabi::store<u8>(gabi::ea(e) + 0x247, a); }
static inline void JPABaseEmitter_setGlobalScale(JPABaseEmitter* e, f32 x, f32 y, f32 z) {
    u32 p = gabi::ea(e);
    gabi::store<f32>(p + 0x220, x);
    gabi::store<f32>(p + 0x224, y);
    gabi::store<f32>(p + 0x228, z);
    gabi::store<f32>(p + 0x238, x);
    gabi::store<f32>(p + 0x23C, y);
    gabi::store<f32>(p + 0x240, z);
}

/* daArrow_c (GameCube +0x118 here): only the members this unit reads */
static inline u8 daArrow_isSetByZelda(fopAc_ac_c* a) { return gabi::load<u8>(gabi::ea(a) + 0x3AC); }
static inline Mtx34* daArrow_mtx(fopAc_ac_c* a) { return gabi::at<Mtx34>(gabi::ea(a) + 0x7CC); }  /* GameCube field_0x6b4 */
static inline u8 daArrow_m6E4(fopAc_ac_c* a) { return gabi::load<u8>(gabi::ea(a) + 0x7FC); }       /* GameCube field_0x6e4 */
static inline csXyz* daArrow_m6E6(fopAc_ac_c* a) { return gabi::at<csXyz>(gabi::ea(a) + 0x7FE); }  /* GameCube field_0x6e6 */
/* daPy_py_c::onUseArrowEffect / offUseArrowEffect: bit 0x2000 of the word at player+0x3BC */
static inline void daPy_onUseArrowEffect(fopAc_ac_c* p) { u32 a = gabi::ea(p) + 0x3BC; gabi::store<u32>(a, gabi::load<u32>(a) | 0x2000); }
static inline void daPy_offUseArrowEffect(fopAc_ac_c* p) { u32 a = gabi::ea(p) + 0x3BC; gabi::store<u32>(a, gabi::load<u32>(a) & ~0x2000u); }

struct daArrow_Lighteff_c : fopAc_ac_c {
    cPhs_State _create();
    bool _delete();
    bool _draw();
    bool _execute();
    void brk_play();

    void setTopPos();
    void setPointLight();
    void delete_particle();
    BOOL CreateHeap();
    void CreateInit();
    void set_mtx();

    /* 0x3AC */ u8 _3AC[0x3B4 - 0x3AC];
    /* 0x3B4 */ gptr<J3DModel> field_0x298;
    /* 0x3B8 */ cXyz field_0x29C;           /* top position */
    /* 0x3C4 */ cXyz field_0x2A8;           /* model scale */
    /* 0x3D0 */ u8 mBrk[0x78];              /* mDoExt_brkAnm (HD 0x78, frame ctrl first) */
    /* 0x448 */ mDoExt_btkAnm mBtk;
    /* 0x4BC */ be<f32> field_0x2E0;        /* brk frame */
    /* 0x4C0 */ be<s32> field_0x2E4;
    /* 0x4C4 */ be<u8> field_0x2E8;         /* type */
    /* 0x4C5 */ be<u8> field_0x2E9;         /* draw */
    /* 0x4C6 */ be<u8> field_0x2EA;
    /* 0x4C7 */ u8 _4C7;
    /* 0x4C8 */ be<u32> field_0x2EC;
    /* 0x4CC */ be<u32> field_0x2F0;
    /* 0x4D0 */ dPa_followEcallBack field_0x2F4;
    /* 0x4E4 */ dPa_followEcallBack field_0x308;
    /* 0x4F8 */ LIGHT_INFLUENCE field_0x31C;
    /* 0x51C */ be<f32> field_0x33C;
};
WWHD_OFFSET(daArrow_Lighteff_c, field_0x298, 0x3B4);
WWHD_OFFSET(daArrow_Lighteff_c, mBtk, 0x448);
WWHD_OFFSET(daArrow_Lighteff_c, field_0x2E0, 0x4BC);
WWHD_OFFSET(daArrow_Lighteff_c, field_0x2F4, 0x4D0);
WWHD_OFFSET(daArrow_Lighteff_c, field_0x31C, 0x4F8);
WWHD_SIZE(daArrow_Lighteff_c, 0x520);

static inline f32 brk_getFrame(daArrow_Lighteff_c* i) { return gabi::load<f32>(gabi::ea(i->mBrk) + 4); }
static inline f32 brk_getEndFrame(daArrow_Lighteff_c* i) { return (f32)gabi::load<s16>(gabi::ea(i->mBrk) + 0xA); }
static inline void brk_setFrame(daArrow_Lighteff_c* i, f32 f) { gabi::store<f32>(gabi::ea(i->mBrk) + 4, f); }

/* 020570BC */
void daArrow_Lighteff_c::setTopPos() {
    WWHD_FUNC(0x020570BC, void, this);
    fopAc_ac_c* arrow = fopAcM_SearchByID(parentActorID);
    if (arrow) {
        PSMTXCopy(daArrow_mtx(arrow), mDoMtx_stack_c::get());
        mDoMtx_stack_c::transM(0.0f, 0.0f, 62.0f);
        /* mDoMtx_stack_c::multVecZero(&field_0x29C) */
        Mtx34* m = mDoMtx_stack_c::get();
        field_0x29C.x = m->m[0][3];
        field_0x29C.y = m->m[1][3];
        field_0x29C.z = m->m[2][3];
    }
}
VERIFY(0x020570BC, &daArrow_Lighteff_c::setTopPos);

/* 0205777C */
void daArrow_Lighteff_c::setPointLight() {
    WWHD_FUNC(0x0205777C, void, this);
    if ((field_0x2EC == 0 || field_0x2EC == 1) && field_0x2E8 != 0) {
        cLib_addCalc2(&field_0x33C, cM_rndF(0.2f) + 1.0f, 0.5f, 0.05f);
    } else {
        field_0x33C = 0.0f;
    }

    /* light_color (0x1000763C): {r, g, b} s16 per type */
    u32 color = 0x1000763C + field_0x2E8 * 6;
    field_0x31C.mPos.copy(field_0x29C);
    field_0x31C.mColorR = gabi::load<s16>(color);
    field_0x31C.mColorG = gabi::load<s16>(color + 2);
    field_0x31C.mColorB = gabi::load<s16>(color + 4);
    field_0x31C.mPower = (f32)(s16)gabi::ftoi(field_0x33C * 150.0f);
    field_0x31C.mFluctuation = 250.0f;
}
VERIFY(0x0205777C, &daArrow_Lighteff_c::setPointLight);

/* 02057510 */
void daArrow_Lighteff_c::delete_particle() {
    WWHD_FUNC(0x02057510, void, this);
    if (field_0x2F4.getEmitter()) {
        if (field_0x2E8 == 3) {
            JPABaseEmitter_setGlobalAlpha(field_0x2F4.getEmitter(), 0);
        }
        field_0x2F4.remove();
    }
    if (field_0x308.getEmitter()) {
        if (field_0x2E8 == 3) {
            JPABaseEmitter_setGlobalAlpha(field_0x308.getEmitter(), 0);
        }
        field_0x308.remove();
    }
}
VERIFY(0x02057510, &daArrow_Lighteff_c::delete_particle);

/* 02056B54. HD: also the solid heap callback (02056E24 is a branch to it) */
BOOL daArrow_Lighteff_c::CreateHeap() {
    WWHD_FUNC(0x02056B54, BOOL, this);
    u8 type = (u8)fopAcM_GetParam(this);
    field_0x2E8 = type;

    J3DModelData* modelData;
    if (type == 1) {
        modelData = (J3DModelData*)dComIfG_getObjectRes(LINK_ARC, dRes_INDEX_LINK_BDL_GARWFI00_e, SAFESTRING_VTBL);
    } else if (type == 2) {
        modelData = (J3DModelData*)dComIfG_getObjectRes(LINK_ARC, dRes_INDEX_LINK_BDL_GARWFI01_e, SAFESTRING_VTBL);
    } else {
        modelData = (J3DModelData*)dComIfG_getObjectRes(LINK_ARC, dRes_INDEX_LINK_BDL_GARWG00_e, SAFESTRING_VTBL);
    }
    if (modelData == nullptr) /* JUT_ASSERT(188, modelData != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0xBC, STR(0x100075E8));

    field_0x298 = mDoExt_J3DModel__create(modelData, 0x80000, 0x3F441422);
    if (field_0x298 == nullptr) {
        return FALSE;
    }

    J3DAnmTextureSRTKey* btk;
    void* brk;
    if (field_0x2E8 == 1) {
        btk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(LINK_ARC, dRes_INDEX_LINK_BTK_GARWFI00_e, SAFESTRING_VTBL);
        brk = dComIfG_getObjectRes(LINK_ARC, dRes_INDEX_LINK_BRK_GARWFI00_e, SAFESTRING_VTBL);
    } else if (field_0x2E8 == 2) {
        btk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(LINK_ARC, dRes_INDEX_LINK_BTK_GARWFI01_e, SAFESTRING_VTBL);
        brk = dComIfG_getObjectRes(LINK_ARC, dRes_INDEX_LINK_BRK_GARWFI01_e, SAFESTRING_VTBL);
    } else {
        btk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(LINK_ARC, dRes_INDEX_LINK_BTK_GARWG00_e, SAFESTRING_VTBL);
        brk = dComIfG_getObjectRes(LINK_ARC, dRes_INDEX_LINK_BRK_GARWG00_e, SAFESTRING_VTBL);
    }
    if (btk == nullptr) /* JUT_ASSERT(217, btk != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0xD9, STR(0x100075FC));
    if (brk == nullptr) /* JUT_ASSERT(218, brk != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0xDA, STR(0x10007608));

    if (!mBtk.init(modelData, btk, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }
    if (!mDoExt_brkAnm_init(mBrk, modelData, brk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    field_0x2E0 = 0.0f;
    return TRUE;
}
VERIFY(0x02056B54, &daArrow_Lighteff_c::CreateHeap);

/* 02056E24 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02056E24, BOOL, i_this);
    return static_cast<daArrow_Lighteff_c*>(i_this)->CreateHeap();
}
VERIFY(0x02056E24, CheckCreateHeap);

/* 0205715C */
void daArrow_Lighteff_c::CreateInit() {
    WWHD_FUNC(0x0205715C, void, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(field_0x298));
    field_0x2A8.z = 1.0f;
    field_0x2A8.y = 1.0f;
    field_0x2A8.x = 1.0f;
    fopAcM_setCullSizeBox(this, -100.0f, -100.0f, -100.0f, 100.0f, 100.0f, 100.0f);
    cullSizeFar = 1.0f;
    /* HD: the scale comes from the particle manager when there is one */
    dPa_control_c* pa = dComIfGp_getParticle();
    if (pa != nullptr) {
        f32 s = gabi::load<f32>(gabi::ea(pa) + 0x110);
        field_0x2A8.x = s;
        field_0x2A8.z = s;
        field_0x2A8.y = s;
    }

    set_mtx();

    field_0x2EC = 0;
    field_0x2E4 = 0;
    field_0x2F0 = 0;

    setTopPos();

    if (field_0x2E8 == 1) {
        if (field_0x2F4.getEmitter() == nullptr) {
            dComIfGp_particle_setP1(0x299 /* ID_IT_JN_ARWF_HINOKO00 */, &field_0x29C, &current.angle, nullptr, 0xFF,
                                  (dPa_levelEcallBack*)&field_0x2F4, -1, nullptr, nullptr, nullptr);
        }
    } else if (field_0x2E8 == 2) {
        if (field_0x2F4.getEmitter() == nullptr) {
            dComIfGp_particle_setP1(0x29C /* ID_IT_JN_ARWI_KIRAKIRA00 */, &field_0x29C, &current.angle, nullptr, 0xFF,
                                  (dPa_levelEcallBack*)&field_0x2F4, -1, nullptr, nullptr, nullptr);
        }
        if (field_0x308.getEmitter() == nullptr) {
            dComIfGp_particle_setP1(0x29D /* ID_IT_JN_ARWI_REIKI00 */, &field_0x29C, &current.angle, nullptr, 0xFF,
                                  (dPa_levelEcallBack*)&field_0x308, -1, nullptr, nullptr, nullptr);
        }
    } else if (field_0x2E8 == 3) {
        if (field_0x2F4.getEmitter() == nullptr) {
            dComIfGp_particle_setP1(0x29F /* ID_IT_JN_ARWG_TSUBU00 */, &field_0x29C, &current.angle, nullptr, 0xFF,
                                  (dPa_levelEcallBack*)&field_0x2F4, -1, nullptr, nullptr, nullptr);
        }
        if (field_0x308.getEmitter() == nullptr) {
            dComIfGp_particle_setP1(0x2A0 /* ID_IT_JN_ARWG_FLASH00 */, &field_0x29C, &current.angle, nullptr, 0xFF,
                                  (dPa_levelEcallBack*)&field_0x308, -1, nullptr, nullptr, nullptr);
        }
    }

    /* HD: the emitters take the model scale */
    f32 sx = field_0x2A8.x;
    f32 sy = field_0x2A8.y;
    f32 sz = field_0x2A8.z;
    if (field_0x2F4.getEmitter()) {
        JPABaseEmitter_setGlobalScale(field_0x2F4.getEmitter(), sx, sy, sz);
    }
    if (field_0x308.getEmitter()) {
        JPABaseEmitter_setGlobalScale(field_0x308.getEmitter(), sx, sy, sz);
    }

    field_0x2E9 = 1;
    dKy_plight_set(&field_0x31C);
}
VERIFY(0x0205715C, &daArrow_Lighteff_c::CreateInit);

/* 02056EB8 */
void daArrow_Lighteff_c::set_mtx() {
    WWHD_FUNC(0x02056EB8, void, this);
    J3DModel_setBaseScale(field_0x298, &field_0x2A8);

    fopAc_ac_c* arrow = fopAcM_SearchByID(parentActorID);
    if (arrow) {
        PSMTXCopy(daArrow_mtx(arrow), mDoMtx_stack_c::get());
        mDoMtx_stack_c::transM(0.0f, 0.0f, 62.0f);
        J3DModel_setBaseTRMtx(field_0x298, mDoMtx_stack_c::get());
    } else {
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, 0);
        mDoMtx_stack_c::transM(0.0f, 0.0f, 62.0f);
        J3DModel_setBaseTRMtx(field_0x298, mDoMtx_stack_c::get());
    }
}
VERIFY(0x02056EB8, &daArrow_Lighteff_c::set_mtx);

/* 02056E28: daArrow_Lighteff_c::daArrow_Lighteff_c (allocates when this == NULL) */
static daArrow_Lighteff_c* daArrow_Lighteff_c_ct(daArrow_Lighteff_c* self) {
    WWHD_FUNC(0x02056E28, daArrow_Lighteff_c*, self);
    if (self == nullptr) {
        self = (daArrow_Lighteff_c*)operator_new(0x520);
        if (self == nullptr)
            return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = LIGHTEFF_VTBL;
    mDoExt_brkAnm_ct(self->mBrk);
    mDoExt_btkAnm::ct(&self->mBtk);
    dPa_followEcallBack_ct(&self->field_0x2F4, 0, 0);
    dPa_followEcallBack_ct(&self->field_0x308, 0, 0);
    self->field_0x31C.mHD20 = 1.0f; /* LIGHT_INFLUENCE (HD) */
    return self;
}
VERIFY(0x02056E28, daArrow_Lighteff_c_ct);

/* 02057478 */
cPhs_State daArrow_Lighteff_c::_create() {
    WWHD_FUNC(0x02057478, cPhs_State, this);
    /* fopAcM_ct(this, daArrow_Lighteff_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            daArrow_Lighteff_c_ct(this);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    field_0x2EA = 0;
    if (!fopAcM_entrySolidHeap(this, 0x02056E24 /* CheckCreateHeap */, 0x2660)) {
        return cPhs_ERROR_e;
    }

    CreateInit();
    return cPhs_COMPLEATE_e;
}
VERIFY(0x02057478, &daArrow_Lighteff_c::_create);

/* 020575A4: daArrow_Lighteff_Delete (_delete inlined) */
bool daArrow_Lighteff_c::_delete() {
    WWHD_FUNC(0x020575A4, bool, this);
    delete_particle();

    if (field_0x2EA != 0) { /* GameCube: == 1 */
        fopAc_ac_c* link = dComIfGp_getPlayer(0);
        fopAc_ac_c* arrow = fopAcM_SearchByID(parentActorID);
        if (arrow) {
            if (!daArrow_isSetByZelda(arrow)) {
                daPy_offUseArrowEffect(link);
            }
        } else {
            daPy_offUseArrowEffect(link);
        }
    }

    dKy_plight_cut(&field_0x31C);
    return true;
}
VERIFY(0x020575A4, &daArrow_Lighteff_c::_delete);

/* 0205763C: daArrow_Lighteff_Draw (_draw inlined). HD: the demo version's structure (draw only
 * when field_0x2E9 is set) */
bool daArrow_Lighteff_c::_draw() {
    WWHD_FUNC(0x0205763C, bool, this);
    if (field_0x2E9 != 0) {
        J3DModelData* modelData = J3DModel_getModelData(field_0x298);
        bool monotone = mDoGph_isMonotone();
        u32 play = dComIfGp_ea();
        if (monotone) { /* dComIfGd_setListP1() */
            gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D58));
            gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D60));
        } else { /* dComIfGd_setListMaskOff() */
            gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D84));
            gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D88));
        }

        mBtk.entry(modelData, mBtk.getFrame());
        mDoExt_brkAnm_entry((mDoExt_brkAnm*)mBrk, modelData, brk_getFrame(this));
        mDoExt_modelUpdateDL(field_0x298);
        /* mBrk.remove(modelData); mBtk.remove(modelData) */
        gabi::store<u32>(gabi::ea(modelData) + 0x44, 0);
        gabi::store<u32>(gabi::ea(modelData) + 0x48, 0);

        dComIfGd_setList();
    }
    return true;
}
VERIFY(0x0205763C, &daArrow_Lighteff_c::_draw);

void daArrow_Lighteff_c::brk_play() {
    mBtk.play();

    s32 cam = dComIfGp_getPlayerCameraID0();
    if (!dComIfGp_checkCameraAttentionStatus(cam, 0x20)) {
        if (field_0x2E0 < brk_getEndFrame(this)) {
            field_0x2E0 = field_0x2E0 + 1.0f;
            if (field_0x2E0 > brk_getEndFrame(this)) {
                field_0x2E0 = brk_getEndFrame(this);
            }
        }
    } else {
        if (field_0x2E0 > 5.0f) {
            field_0x2E0 = field_0x2E0 - 1.0f;
            if (field_0x2E0 < 5.0f) {
                field_0x2E0 = 5.0f;
            }
        } else if (field_0x2E0 < 5.0f) {
            field_0x2E0 = field_0x2E0 + 1.0f;
            if (field_0x2E0 > 5.0f) {
                field_0x2E0 = 5.0f;
            }
        }
    }

    brk_setFrame(this, field_0x2E0);
}

/* 02057930: the profile's Execute */
bool daArrow_Lighteff_c::_execute() {
    WWHD_FUNC(0x02057930, bool, this);
    field_0x2F0 = (u32)field_0x2EC;

    brk_play();

    fopAc_ac_c* link = dComIfGp_getPlayer(0);
    fopAc_ac_c* arrow = fopAcM_SearchByID(parentActorID);
    if (arrow) {
        u32 param = fopAcM_GetParam(arrow);
        field_0x2EC = param;
        if (daArrow_m6E4(arrow) != 0) { /* GameCube: == 1 */
            fopAcM_delete(this);
        } else if (param != 0) {
            if (param == 1) {
                if (field_0x2F0 != 1) {
                    if (!daArrow_isSetByZelda(arrow)) {
                        daPy_onUseArrowEffect(link);
                    }
                    field_0x2A8.z = 1.0f;
                    field_0x2EA = 1;
                } else {
                    f32 z = (f32)((f64)field_0x2A8.z + 1.0);
                    if (z < 20.0f) { /* the code tests z < 20 (blt) and x > 0.5 (bgt): NaN clamps */
                    } else {
                        z = 20.0f;
                    }
                    f32 x = field_0x2A8.x - 0.025f; /* GameCube: double */
                    field_0x2A8.z = z;
                    if (x > 0.5f) {
                    } else {
                        x = 0.5f;
                    }
                    field_0x2A8.x = x;
                    field_0x2A8.y = x;
                }
            } else if (param == 3) {
                fopAcM_delete(this);
            } else {
                field_0x2E9 = 0;
                delete_particle();
                field_0x2E4 = field_0x2E4 + 1;
                if (0x3C <= field_0x2E4) {
                    fopAcM_delete(this);
                }
            }
        }

        current.pos.copy(arrow->current.pos);
        csXyz* ang = daArrow_m6E6(arrow);
        current.angle.x = ang->x;
        current.angle.y = ang->y;
        current.angle.z = ang->z;
        setTopPos();
        setPointLight();
    } else {
        fopAcM_delete(this);
    }

    s32 cam = dComIfGp_getPlayerCameraID0();
    if (field_0x2E8 == 1) {
        fopAcM_seStartCurrent(this, 0x106F /* JA_SE_OBJ_FIRE_ARROW_AMB */, 0);
        if (!dComIfGp_checkCameraAttentionStatus(cam, 0x20)) {
            dComIfGp_particle_setSimple(0x4004 /* ID_AK_JP_O_KAGEROU00 */, &field_0x29C);
        }
    } else if (field_0x2E8 == 2) {
        fopAcM_seStartCurrent(this, 0x1070 /* JA_SE_OBJ_ICE_ARROW_AMB */, 0);
        if (dComIfGp_checkCameraAttentionStatus(cam, 0x20)) {
            if (field_0x2F4.getEmitter()) {
                JPABaseEmitter_setGlobalAlpha(field_0x2F4.getEmitter(), 0x64);
            }
            if (field_0x308.getEmitter()) {
                JPABaseEmitter_setGlobalAlpha(field_0x308.getEmitter(), 0x64);
            }
        } else {
            if (field_0x2F4.getEmitter()) {
                JPABaseEmitter_setGlobalAlpha(field_0x2F4.getEmitter(), 0xFF);
            }
            if (field_0x308.getEmitter()) {
                JPABaseEmitter_setGlobalAlpha(field_0x308.getEmitter(), 0xFF);
            }
        }
    } else {
        fopAcM_seStartCurrent(this, 0x1071 /* JA_SE_OBJ_LIGHT_ARROW_AMB */, 0);
    }

    set_mtx();
    return true;
}
VERIFY(0x02057930, &daArrow_Lighteff_c::_execute);

/* 02057EF0 */
static BOOL daArrow_Lighteff_IsDelete(void*) {
    WWHD_FUNC(0x02057EF0, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02057EF0, daArrow_Lighteff_IsDelete);

/* 02057E48 */
static void __sinit_d_a_arrow_lighteff_cpp() {
    WWHD_FUNC(0x02057E48, void, (u32)0);
    sinit_header_statics(0x104614B4, 0x1018FF3C);
}
VERIFY(0x02057E48, __sinit_d_a_arrow_lighteff_cpp);

/* 02057EDC: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02057EDC, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02057EDC, trivial_dt);

/* 02057EF8: daArrow_Lighteff_c deleting destructor */
static void daArrow_Lighteff_c_dt(daArrow_Lighteff_c* self, s32 flags) {
    WWHD_FUNC(0x02057EF8, void, self, flags);
    if (self != nullptr) {
        gabi::call(0x025D50BC, self, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(self);
    }
}
VERIFY(0x02057EF8, daArrow_Lighteff_c_dt);

/* 02057F4C: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x02057F4C, void, p);
}
VERIFY(0x02057F4C, empty_virtual);
