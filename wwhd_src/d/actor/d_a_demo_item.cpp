/**
 * d_a_demo_item.cpp (WWHD)
 * Item - Cutscene Item (the item Link holds up in get-item cutscenes).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_demo_item.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_itembase.h"

#define DITEM_VTBL 0x1000DBE4
#define m_effect_type(no) gabi::load<u8>(0x1000DAE4 + (no))  /* daDitem_c::m_effect_type[0x100] */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0254DA38 execItemGet(u8) (as in d_a_npc_bms1.h) */
static inline void execItemGet(u8 item) { gabi::call(0x0254DA38, item); }
/* 025F1AA4 mDoMtx_ZXYrotS (as in d_a_obj_buoyflag.h) */
static inline void mDoMtx_ZXYrotS(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F1AA4, m, x, y, z); }
/* 028245AC JPAGetXYZRotateMtx(x, y, z, Mtx) (as in d_a_npc_md_exec) */
static inline void JPAGetXYZRotateMtx(s16 x, s16 y, s16 z, u32 mtx) { gabi::call(0x028245AC, x, y, z, mtx); }
/* fopAcM_onDraw: fopDwTg_ToDrawQ(&draw_tag (+0xDC), fpcLf_GetPriority(this)) (as in d_a_npc_md_exec) */
static inline void fopAcM_onDraw(fopAc_ac_c* a) {
    s16 prio = gabi::call<s16>(0x025DF2B8, a);
    gabi::call(0x025DA874, gabi::ea(a) + 0xDC, prio);
}
/* dComIfGp_getCamera(0): camera_class* at play+0x5AF8; dCam_getAngleX (024F8008: lha +0x254),
 * dCam_getAngleY (024F8000: cSAngle::Inv of +0x256) */
static inline u32 dComIfGp_getCamera0() { return gabi::load<u32>(dComIfGp_ea() + 0x5AF8); }
static inline s16 dCam_getAngleX(u32 cam) { return gabi::call<s16>(0x024F8008, cam); }
static inline s16 dCam_getAngleY(u32 cam) { return gabi::call<s16>(0x024F8000, cam); }
/* dComIfGd_getInvViewMtx(): the view (play+0x5FA4) +0x174 */
static inline Mtx34* dComIfGd_getInvViewMtx() { return gabi::at<Mtx34>(gabi::load<u32>(dComIfGp_ea() + 0x5FA4) + 0x174); }
/* 0252CA4C dDlst_texSpecmapST(eye, tevstr, J3DModel*, f32): HD passes the model (as in d_a_itembase) */
static inline void dDlst_texSpecmapST(cXyz* eye, dKy_tevstr_c* tev, J3DModel* model, f32 scale) {
    gabi::call(0x0252CA4C, eye, tev, model, scale);
}
/* daItemBase_c methods (d_a_itembase, by address) */
static inline void itemBase_hide(daItemBase_c* a) { gabi::call(0x021842B8, a); }
static inline bool itemBase_chkDraw(daItemBase_c* a) { return gabi::call<bool>(0x021842D8, a); }
static inline bool itemBase_chkDead(daItemBase_c* a) { return gabi::call<bool>(0x0218433C, a); }
static inline void itemBase_animPlay(daItemBase_c* a, f32 b1, f32 b2, f32 t1, f32 t2, f32 bck) {
    gabi::call(0x021837B0, a, b1, b2, t1, t2, bck);
}
static inline BOOL itemBase_DeleteBase(daItemBase_c* a, const char* arc) { return gabi::call<BOOL>(0x02183788, a, arc); }
/* JPABaseEmitter (HD): flags +0x254, global translation +0x22C, global rotation matrix +0x1F0,
 * +0x262 a u8 whose value >= 7 flips the translation's y, alpha +0x247, max frame +0x5C */
static inline u32 em(JPABaseEmitter* e) { return gabi::ea(e); }

struct daDitem_c : daItemBase_c {
    enum ArgFlag { FLAG_UNK01 = 0x01, FLAG_UNK02 = 0x02, FLAG_UNK04 = 0x04, FLAG_UNK08 = 0x08 };

    BOOL chkArgFlag(u8 flag) { return mArgFlag & flag; }

    BOOL Delete();
    cPhs_State create();
    BOOL execute();
    void setParticle();
    bool CreateInit();
    void set_effect();
    void set_pos();
    void anim_control();
    void set_mtx();
    void settingBeforeDraw();

    /* 0x750 */ gptr<JPABaseEmitter> mpEmitters[4];
    /* 0x760 */ cXyz mOffsetPos;
    /* 0x76C */ be<u8> mFlag;
    /* 0x76D */ be<u8> mArgFlag;
    /* 0x76E */ u8 _76E[2];
};
WWHD_OFFSET(daDitem_c, mpEmitters, 0x750);
WWHD_OFFSET(daDitem_c, mArgFlag, 0x76D);
WWHD_SIZE(daDitem_c, 0x770);

/* 021233CC. HD: no unused scale/position temporaries */
void daDitem_c::setParticle() {
    WWHD_FUNC(0x021233CC, void, this);
    if (mpEmitters[0] || mpEmitters[1] || mpEmitters[2] || mpEmitters[3]) {
        return;
    }
    if (chkArgFlag(FLAG_UNK02) || chkArgFlag(FLAG_UNK04) || chkArgFlag(FLAG_UNK08)) {
        return;
    }

    gabi::Local<csXyz> angle;
    angle->x = (s16)(dCam_getAngleX(dComIfGp_getCamera0()) - 0x2000);
    angle->y = dCam_getAngleY(dComIfGp_getCamera0());
    angle->z = 0;

    switch (m_effect_type(m_itemNo)) {
    case 0:
        mpEmitters[0] = dComIfGp_particle_set(0x1F7 /* ID_IT_JN_GETITEM_FLASH_L00 */, &current.pos, angle);
    case 1:
        mpEmitters[1] = dComIfGp_particle_set(0x1F8 /* ID_IT_JN_GETITEM_FLASH_S00 */, &current.pos, angle);
    case 2:
        mpEmitters[2] = dComIfGp_particle_set(0x1F9 /* ID_IT_JN_GETITEM_HALO00 */, &current.pos, angle);
    case 3:
        mpEmitters[3] = dComIfGp_particle_set(0x1FA /* ID_IT_JN_GETITEM_STAR00 */, &current.pos, angle);
    case 4:
        break;
    }
}
VERIFY(0x021233CC, &daDitem_c::setParticle);

/* 02123164 */
bool daDitem_c::CreateInit() {
    WWHD_FUNC(0x02123164, bool, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel));
    itemBase_hide(this);
    mFlag = 0; /* clrFlag() */
    u8 argFlag = (fopAcM_GetParam(this) >> 0x10) & 0xFF;
    mArgFlag = argFlag;
    if (!(argFlag & FLAG_UNK02) && !(argFlag & FLAG_UNK04) && !(argFlag & FLAG_UNK08)) {
        current.angle.y = -0x2000;
    }
    for (int i = 0; i < 4; i++) {
        mpEmitters[i] = nullptr;
    }
    return true;
}
VERIFY(0x02123164, &daDitem_c::CreateInit);

/* 021235A4 */
void daDitem_c::set_effect() {
    WWHD_FUNC(0x021235A4, void, this);
    s16 angleX = (s16)(dCam_getAngleX(dComIfGp_getCamera0()) - 0x2000);
    s16 angleY = dCam_getAngleY(dComIfGp_getCamera0());
    for (int i = 0; i < 4; i++) {
        if (mpEmitters[i] == nullptr) {
            continue;
        }
        /* playCreateParticle() */
        u32 e = em(mpEmitters[i]);
        gabi::store<u32>(e + 0x254, gabi::load<u32>(e + 0x254) & ~1u);
        /* setGlobalTranslation(current.pos); HD: the y is negated when +0x262 >= 7 */
        e = em(mpEmitters[i]);
        f32 y = current.pos.y;
        u8 kind = gabi::load<u8>(e + 0x262);
        f32 x = current.pos.x;
        f32 z = current.pos.z;
        gabi::store<f32>(e + 0x22C, x);
        gabi::store<f32>(e + 0x234, z);
        gabi::store<f32>(e + 0x230, y);
        if (kind >= 7) {
            gabi::store<f32>(e + 0x230, -gabi::load<f32>(e + 0x230));
        }
        /* setGlobalRotation(rot) */
        JPAGetXYZRotateMtx(angleX, angleY, 0, em(mpEmitters[i]) + 0x1F0);
    }
}
VERIFY(0x021235A4, &daDitem_c::set_effect);

/* 021236A8 */
void daDitem_c::set_pos() {
    WWHD_FUNC(0x021236A8, void, this);
    /* static cXyz offset_tbl[3] (function-local static, 0x10463B34; guard 0x10463B58) */
    u32 tbl = 0x10463B34;
    if (gabi::load<u32>(0x10463B58) == 0) {
        gabi::store<f32>(tbl + 0x18, 30.0f);
        gabi::store<f32>(tbl + 0xC, 0.0f);
        gabi::store<f32>(tbl + 0x1C, 140.0f);
        gabi::store<f32>(tbl + 0x14, 50.0f);
        gabi::store<f32>(tbl + 0x4, 130.0f);
        gabi::store<u32>(0x10463B58, 1);
        gabi::store<f32>(tbl + 0x20, 20.0f);
        gabi::store<f32>(tbl + 0x8, 0.0f);
        gabi::store<f32>(tbl + 0x10, 90.0f);
        gabi::store<f32>(tbl + 0x0, 0.0f);
    }

    gabi::Local<cXyz> offset;
    gabi::Local<cXyz> pos;
    if (chkArgFlag(FLAG_UNK02)) {
        offset->copy(*gabi::at<cXyz>(tbl + 0xC));
    } else if (chkArgFlag(FLAG_UNK04)) {
        offset->copy(*gabi::at<cXyz>(tbl + 0x18));
    } else if (chkArgFlag(FLAG_UNK08)) {
        offset->copy(mOffsetPos);
    } else {
        offset->copy(*gabi::at<cXyz>(tbl));
    }

    if (!chkArgFlag(FLAG_UNK08)) {
        s16 ax = dComIfGp_getPlayer(0)->current.angle.x;
        s16 ay = dComIfGp_getPlayer(0)->shape_angle.y;
        s16 az = dComIfGp_getPlayer(0)->current.angle.z;
        mDoMtx_ZXYrotS(mDoMtx_stack_c::get(), ax, ay, az);
        PSMTXMultVec(mDoMtx_stack_c::get(), offset, offset);
        pos->copy(dComIfGp_getPlayer(0)->current.pos);
    } else {
        pos->copy(home.pos);
    }

    PSVECAdd(pos, offset, pos);
    current.pos.copy(*pos);
}
VERIFY(0x021236A8, &daDitem_c::set_pos);

/* 0212366C. HD: the bottle cases are the range 0x4F..0x60 */
void daDitem_c::anim_control() {
    WWHD_FUNC(0x0212366C, void, this);
    u8 no = m_itemNo;
    if (no >= 0x4F /* dItemNo_EMPTY_BSHIP_e */ && no <= 0x60 /* dItemNo_UNK_BOTTLE_60_e */) {
        if (m_timer > 30) {
            itemBase_animPlay(this, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        }
    } else {
        itemBase_animPlay(this, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    }
}
VERIFY(0x0212366C, &daDitem_c::anim_control);

/* 021238EC */
void daDitem_c::set_mtx() {
    WWHD_FUNC(0x021238EC, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    gabi::call(0x025D677C, this, (s16)(current.angle.y + 0x0111), (s16)0x0111); /* fopAcM_addAngleY */

    if (chkArgFlag(FLAG_UNK02) || chkArgFlag(FLAG_UNK04) || chkArgFlag(FLAG_UNK08)) {
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::YrotM(current.angle.y);
    } else {
        PSMTXCopy(dComIfGd_getInvViewMtx(), mDoMtx_stack_c::get());
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), current.angle.z);
        mDoMtx_XrotM(mDoMtx_stack_c::get(), 0x12C0);
        mDoMtx_stack_c::YrotM(current.angle.y);
        mDoMtx_stack_c::get()->m[0][3] = current.pos.x;
        mDoMtx_stack_c::get()->m[1][3] = current.pos.y;
        mDoMtx_stack_c::get()->m[2][3] = current.pos.z;
    }
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x021238EC, &daDitem_c::set_mtx);

/* 02123B70 */
void daDitem_c::settingBeforeDraw() {
    WWHD_FUNC(0x02123B70, void, this);
    if (isBomb(m_itemNo) || m_itemNo == 0x31 /* dItemNo_BOMB_BAG_e */ || m_itemNo == 0x33 /* dItemNo_SKULL_HAMMER_e */ ||
        m_itemNo == 0x15 /* dItemNo_SMALL_KEY_e */) {
        dDlst_texSpecmapST(&eyePos, &tevStr, mpModel, 1.0f);
    }
}
VERIFY(0x02123B70, &daDitem_c::settingBeforeDraw);

/* 02123C70: setListStart (empty) */
static void daDitem_setListStart(daDitem_c* i_this) {
    WWHD_FUNC(0x02123C70, void, i_this);
}
VERIFY(0x02123C70, daDitem_setListStart);

/* 021230C8: daDitem_Delete (Delete inlined) */
BOOL daDitem_c::Delete() {
    WWHD_FUNC(0x021230C8, BOOL, this);
    if (!chkArgFlag(FLAG_UNK01)) {
        execItemGet(m_itemNo);
    }

    for (int i = 0; i < 4; i++) {
        if (mpEmitters[i]) {
            gabi::store<u8>(em(mpEmitters[i]) + 0x247, 0); /* setGlobalAlpha(0) */
            /* becomeInvalidEmitter() (HD: max frame -1, stop flag) */
            u32 e = em(mpEmitters[i]);
            u32 flags = gabi::load<u32>(e + 0x254);
            gabi::store<s32>(e + 0x5C, -1);
            gabi::store<u32>(e + 0x254, flags | 1);
            mpEmitters[i] = nullptr;
        }
    }

    return itemBase_DeleteBase(this, gabi::at<const char>(gabi::load<u32>(dItem_data::item_resource(m_itemNo))));
}
VERIFY(0x021230C8, &daDitem_c::Delete);

/* 021231F8: daDitem_Create (create inlined) */
cPhs_State daDitem_c::create() {
    WWHD_FUNC(0x021231F8, cPhs_State, this);
    /* fopAcM_ct(this, daDitem_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = 0x10012158; /* daItemBase_c */
            const dBgS_ObjAcch_vt acchVt = {0x1000DA84, 0x1000DAA4, 0x1000DA94};
            dBgS_ObjAcch_ct(&mAcch, acchVt);
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, 0x1000DA74);
            __vtbl = DITEM_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    u8 no = (u8)fopAcM_GetParam(this);
    m_itemNo = no;
    u32 res = dItem_data::item_resource(no);
    u32 arcName = gabi::load<u32>(res);
    if (gabi::load<s16>(res + 8) == -1 || arcName == 0) { /* getBmdIdx, getArcname */
        m_itemNo = 1; /* dItemNo_GREEN_RUPEE_e */
        arcName = gabi::load<u32>(dItem_data::item_resource(1));
    }

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, gabi::at<const char>(arcName));
    if (phase_state == cPhs_COMPLEATE_e) {
        u32 heap_size = gabi::load<u16>(dItem_data::item_resource(m_itemNo) + 0x20); /* getHeapSize */
        if (!fopAcM_entrySolidHeap(this, 0x021841D0 /* CheckItemCreateHeap */, heap_size)) {
            return cPhs_ERROR_e;
        }
        CreateInit();
    }
    return phase_state;
}
VERIFY(0x021231F8, &daDitem_c::create);

/* 021233C4 */
static BOOL daDitem_IsDelete(daDitem_c* i_this) {
    WWHD_FUNC(0x021233C4, BOOL, i_this);
    return TRUE;
}
VERIFY(0x021233C4, daDitem_IsDelete);

/* 02123AA4: daDitem_Execute (execute inlined) */
BOOL daDitem_c::execute() {
    WWHD_FUNC(0x02123AA4, BOOL, this);
    m_timer = m_timer + 1;

    if (itemBase_chkDraw(this)) {
        setParticle();
        set_effect();
        fopAcM_onDraw(this);
    } else {
        fopAcM_offDraw(this);
    }

    anim_control();

    if (itemBase_chkDead(this)) {
        fopAcM_delete(this);
    }

    set_pos();
    set_mtx();
    return TRUE;
}
VERIFY(0x02123AA4, &daDitem_c::execute);

/* 02123B60: daDitem_Draw: the virtual DrawBase */
static BOOL daDitem_Draw(daDitem_c* i_this) {
    WWHD_FUNC(0x02123B60, BOOL, i_this);
    return gabi::call_ptr<BOOL>(i_this->vfn(daItemBase_VT_DRAWBASE), i_this);
}
VERIFY(0x02123B60, daDitem_Draw);

/* 02123BDC */
static void __sinit_d_a_demo_item_cpp() {
    WWHD_FUNC(0x02123BDC, void, (u32)0);
    sinit_header_statics(0x10463B18, 0x101B43E4);
}
VERIFY(0x02123BDC, __sinit_d_a_demo_item_cpp);

/* 02123C74: daDitem_c deleting destructor */
static void daDitem_c_dt(daDitem_c* self, s32 flags) {
    WWHD_FUNC(0x02123C74, void, self, flags);
    if (self != nullptr) {
        gabi::call(0x021832F4, self, 0); /* daItemBase_c::~daItemBase_c */
        if (flags & 1)
            operator_delete(self);
    }
}
VERIFY(0x02123C74, daDitem_c_dt);
