/**
 * d_a_spc_item01.cpp (WWHD)
 * Item - Special item (shield, joy pendant, knight's crest, ... placed by events)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_spc_item01.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_itembase.h"

#define DASPCITEM01_VTBL 0x1003D0A8
#define DAITEMBASE_VTBL 0x10012158
#define l_cyl_src gabi::at<dCcD_SrcCyl>(0x101D0A80)

enum {
    dItemNo_JOY_PENDANT_e = 0x1F,
    dItemNo_SHIELD_e = 0x3B,
    dItemNo_KNIGHTS_CREST_e = 0x48,
};
enum { TEV_TYPE_BG1 = 2 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02551B90 dItem_data::checkAppearEffect(u8), 02551BBC dItem_data::getAppearEffect(u8) */
static inline BOOL dItem_data_checkAppearEffect(u8 no) { return gabi::call<BOOL>(0x02551B90, no); }
static inline u16 dItem_data_getAppearEffect(u8 no) { return gabi::call<u16>(0x02551BBC, no); }
/* dComIfGs_isEventBit: the event block at *(0x101F84DC) + 0x644 (dSv_event_c) */
static inline BOOL dComIfGs_isEventBit(u16 flag) {
    return dSv_event_isEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), flag);
}
/* daItemBase_c::getHeight / getR / setLoadError (verified in d_a_itembase_static) */
static inline u32 itemBase_getHeight(daItemBase_c* p) { return gabi::call<u8>(0x02184288, p); }
static inline u32 itemBase_getR(daItemBase_c* p) { return gabi::call<u8>(0x021842A0, p); }

struct daSpcItem01_c : daItemBase_c {
    void set_mtx();
    BOOL _delete();
    cPhs_State _create();
    BOOL CreateInit();
    BOOL _execute();
    void set_effect();
    void scale_anim();
    void anim_play();
    void move();
    void rotate_item();
    BOOL _draw();
    void setTevStr();

    /* 0x750 */ be<f32> field_0x63C;
    /* 0x754 */ be<s16> field_0x640;
    /* 0x756 */ be<u8> field_0x642;
    /* 0x757 */ be<u8> field_0x643;
    /* 0x758 */ be<u16> field_0x644;
    /* 0x75A */ be<u8> field_0x646;
    /* 0x75B */ be<u8> field_0x647;
};
WWHD_OFFSET(daSpcItem01_c, field_0x63C, 0x750);
WWHD_OFFSET(daSpcItem01_c, field_0x644, 0x758);

namespace daSpcItem01_prm {
inline u8 getItemNo(daSpcItem01_c* i_this) { return fopAcM_GetParam(i_this) >> 0 & 0xFF; }
inline u16 getFlag(daSpcItem01_c* i_this) { return fopAcM_GetParam(i_this) >> 8 & 0xFFFF; }
};  // namespace daSpcItem01_prm

static inline const char* getFieldArc(u32 no) { return gabi::at<const char>(gabi::load<u32>(dItem_data::field_item_res(no))); }

/* 0248A0F0 */
void daSpcItem01_c::set_mtx() {
    WWHD_FUNC(0x0248A0F0, void, this);
    s16 angleX = current.angle.x, angleY = current.angle.y, angleZ = current.angle.z; /* csXyz angle = current.angle */
    f32 offsetY = 0.0f;
    switch (m_itemNo) {
    case dItemNo_KNIGHTS_CREST_e:
        offsetY = -24.0f;
        break;
    }
    J3DModel_setBaseScale(mpModel, &scale);

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y + offsetY, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), angleX, angleY, angleZ);

    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x0248A0F0, &daSpcItem01_c::set_mtx);

/* 0248A288 */
BOOL daSpcItem01_c::_delete() {
    WWHD_FUNC(0x0248A288, BOOL, this);
    return gabi::call<BOOL>(0x02183788, this, getFieldArc(m_itemNo)); /* DeleteBase(dItem_data::getFieldArc(m_itemNo)) */
}
VERIFY(0x0248A288, &daSpcItem01_c::_delete);

/* 0248A448 */
cPhs_State daSpcItem01_c::_create() {
    WWHD_FUNC(0x0248A448, cPhs_State, this);
    /* fopAcM_ct(this, daSpcItem01_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = DAITEMBASE_VTBL;
            dBgS_ObjAcch_ct(&mAcch, {0x1003D03C, 0x1003D05C, 0x1003D04C});
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, 0x1003D02C);
            __vtbl = DASPCITEM01_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    m_itemNo = daSpcItem01_prm::getItemNo(this);
    if (m_itemNo == dItemNo_SHIELD_e && dComIfGs_isEventBit(0x0E20 /* dSv_event_flag_c::UNK_0E20 */)) {
        gabi::call(0x02184350, this); /* setLoadError() */
        return cPhs_ERROR_e;
    }

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, getFieldArc(m_itemNo));
    if (phase_state == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, 0x0218422C /* CheckFieldItemCreateHeap */,
                                   gabi::load<u16>(dItem_data::item_resource(m_itemNo) + 0x20) /* getHeapSize */))
        {
            return cPhs_ERROR_e;
        }
        CreateInit();
    }

    return phase_state;
}
VERIFY(0x0248A448, &daSpcItem01_c::_create);

/* 0248A2A4 */
BOOL daSpcItem01_c::CreateInit() {
    WWHD_FUNC(0x0248A2A4, BOOL, this);
    set_mtx();
    cullMtx = mpModel ? gabi::ea(mpModel.get()) + 0xC8 : 0; /* fopAcM_SetMtx(this, mpModel->getBaseTRMtx()) */
    mStts.Init(0, 0xFF, this);
    mCyl.Set(l_cyl_src);
    mCyl.SetStts(&mStts);
    f32 tempVar1 = (f32)itemBase_getHeight(this);
    f32 tempVar2 = (f32)itemBase_getR(this);
    if (scale.x > 1.0f) {
        tempVar2 *= scale.x;
        tempVar1 *= scale.x;
    }
    mCyl.SetR(tempVar2);
    mCyl.SetH(tempVar1);
    mAcchCir.SetWall(30.0f, 30.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed);

    field_0x644 = daSpcItem01_prm::getFlag(this);
    gravity = -4.0f; /* fopAcM_SetGravity */
    switch (m_itemNo) {
    case dItemNo_SHIELD_e:
        scale.x = 1.5f;
        scale.y = 1.5f;
        scale.z = 1.5f;
        current.angle.x = 4000;
        current.angle.y = 4200;
        current.angle.z = 5200;
        gravity = 0.0f;
        break;
    }

    return TRUE;
}
VERIFY(0x0248A2A4, &daSpcItem01_c::CreateInit);

/* 0248A1F4 */
BOOL daSpcItem01_c::_execute() {
    WWHD_FUNC(0x0248A1F4, BOOL, this);
    eyePos.copy(current.pos);
    gabi::at<cXyz>(gabi::ea(this) + 0x390)->copy(current.pos); /* attention_info.position */
    m_timer = m_timer + 1;
    set_effect();
    scale_anim();
    anim_play();
    move();
    rotate_item();
    /* setCol(): empty, not called in HD */
    set_mtx();
    return TRUE;
}
VERIFY(0x0248A1F4, &daSpcItem01_c::_execute);

/* 02489E3C */
void daSpcItem01_c::set_effect() {
    WWHD_FUNC(0x02489E3C, void, this);
    if ((field_0x644 & 0x01) && dItem_data_checkAppearEffect(m_itemNo) && !field_0x642 && m_itemNo != dItemNo_KNIGHTS_CREST_e) {
        dComIfGp_particle_setSimple(dItem_data_getAppearEffect(m_itemNo), &current.pos);
    }
}
VERIFY(0x02489E3C, &daSpcItem01_c::set_effect);

/* 02489ED0 */
void daSpcItem01_c::scale_anim() {
    WWHD_FUNC(0x02489ED0, void, this);
    if (isRupee(m_itemNo)) {
        cLib_chaseF(&scale.x, 1.0f, 0.05f);
        cLib_chaseF(&scale.y, 1.0f, 0.05f);
        cLib_chaseF(&scale.z, 1.0f, 0.05f);
    }
}
VERIFY(0x02489ED0, &daSpcItem01_c::scale_anim);

/* 02489F74 */
void daSpcItem01_c::anim_play() {
    WWHD_FUNC(0x02489F74, void, this);
    f32 animPlayParam = 1.0f;
    if (m_itemNo == dItemNo_KNIGHTS_CREST_e) {
        animPlayParam = 0.0f;
    }
    animPlay(1.0f, 1.0f, 1.0f, 1.0f, animPlayParam);
}
VERIFY(0x02489F74, &daSpcItem01_c::anim_play);

/* 02489FA4 */
void daSpcItem01_c::move() {
    WWHD_FUNC(0x02489FA4, void, this);
    fopAcM_posMoveF(this, &mStts.m_cc_move);
    mAcch.CrrPos(dComIfG_Bgsp());
    switch (m_itemNo) {
    case dItemNo_SHIELD_e:
        break;
    case dItemNo_JOY_PENDANT_e:
        if (mAcch.ChkGroundLanding()) {
            speed.x = 0.0f;
            speed.y = 0.0f;
            speed.z = 0.0f;
            speedF = 0.0f;
        }
        break;
    default:
        if (mAcch.ChkGroundLanding()) {
            field_0x642 = field_0x642 + 1;
            f32 newGravity = field_0x63C * 0.62f;
            if (newGravity > gravity - 0.5f) {
                speedF = 0.0f;
            } else {
                speed.x = 0.0f;
                speed.y = -newGravity;
                speed.z = 0.0f;
            }
        }
        break;
    }

    if (speed.y != 0.0f) {
        field_0x63C = speed.y;
    }
}
VERIFY(0x02489FA4, &daSpcItem01_c::move);

/* 0248A08C */
void daSpcItem01_c::rotate_item() {
    WWHD_FUNC(0x0248A08C, void, this);
    if (isRupee(m_itemNo)) {
        if (field_0x642 == 0) {
            field_0x640 = current.angle.x + 0x2000;
        } else {
            field_0x640 = 0;
        }
        cLib_chaseAngleS(&current.angle.x, field_0x640, 0x2000);
    }
}
VERIFY(0x0248A08C, &daSpcItem01_c::rotate_item);

/* 02489E28 (not named by the matcher): DrawBase() is virtual in HD, called as a tail call */
BOOL daSpcItem01_c::_draw() {
    WWHD_FUNC(0x02489E28, BOOL, this);
    return gabi::call_ptr<BOOL>(vfn(daItemBase_VT_DRAWBASE), this);
}
VERIFY(0x02489E28, &daSpcItem01_c::_draw);

/* 0248A654 */
void daSpcItem01_c::setTevStr() {
    WWHD_FUNC(0x0248A654, void, this);
    s32 type = m_itemNo == dItemNo_KNIGHTS_CREST_e ? TEV_TYPE_BG1 : TEV_TYPE_ACTOR;
    dScnKy_env_light_c* env = dKy_getEnvlight();
    settingTevStruct(env, type, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, mpModel, &tevStr);

    for (s32 modelIndex = 0; modelIndex < 2; modelIndex++) {
        if (mpModelArrow[modelIndex] != nullptr) {
            env = dKy_getEnvlight();
            setLightTevColorType(env, mpModelArrow[modelIndex], &tevStr);
        }
    }
}
VERIFY(0x0248A654, &daSpcItem01_c::setTevStr);

/* 02489E38 */
static BOOL daSpcItem01_Draw(daSpcItem01_c* i_this) {
    WWHD_FUNC(0x02489E38, BOOL, i_this);
    return gabi::call<BOOL>(0x02489E28, i_this); /* i_this->_draw() (tail call) */
}
VERIFY(0x02489E38, daSpcItem01_Draw);

/* 0248A27C */
static BOOL daSpcItem01_Execute(daSpcItem01_c* i_this) {
    WWHD_FUNC(0x0248A27C, BOOL, i_this);
    return gabi::call<BOOL>(0x0248A1F4, i_this); /* i_this->_execute() (tail call) */
}
VERIFY(0x0248A27C, daSpcItem01_Execute);

/* 0248A280 */
static BOOL daSpcItem01_IsDelete(daSpcItem01_c*) {
    WWHD_FUNC(0x0248A280, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0248A280, daSpcItem01_IsDelete);

/* 0248A2A0 */
static BOOL daSpcItem01_Delete(daSpcItem01_c* i_this) {
    WWHD_FUNC(0x0248A2A0, BOOL, i_this);
    return gabi::call<BOOL>(0x0248A288, i_this); /* i_this->_delete() (tail call) */
}
VERIFY(0x0248A2A0, daSpcItem01_Delete);

/* 0248A650 */
static cPhs_State daSpcItem01_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0248A650, cPhs_State, i_this);
    return gabi::call<cPhs_State>(0x0248A448, i_this); /* ((daSpcItem01_c*)i_this)->_create() (tail call) */
}
VERIFY(0x0248A650, daSpcItem01_Create);

/* 0248A708 __sinit_d_a_spc_item01_cpp (new: compiler-generated header statics) */
static void spc_item01_sinit() {
    WWHD_FUNC(0x0248A708, void, (u32)0);
    sinit_header_statics(0x1046DF1C, 0x101D0AC4);
}
VERIFY(0x0248A708, spc_item01_sinit);

/* 0248A79C daSpcItem01_c deleting destructor (vtable 0x1003D0A8 slot 0xC): ~daItemBase_c */
static void spc_item01_dtor(daSpcItem01_c* p, s32 flags) {
    WWHD_FUNC(0x0248A79C, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x021832F4, p, 0);
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x0248A79C, spc_item01_dtor);
