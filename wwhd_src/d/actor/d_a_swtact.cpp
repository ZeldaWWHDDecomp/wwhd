/**
 * d_a_swtact.cpp (WWHD)
 * Object - Wind Crest (Wind's Requiem blue floor decoration)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_swtact.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

/* daSwTact_c::m_arcname: a guest pointer variable (0x101D15B0 -> "Itact") */
#define M_arcname gabi::at<const char>(gabi::load<u32>(0x101D15B0))
#define SAFESTRING_VTBL 0x1003ED54 /* this TU's copy of the sead::SafeString vtable */
#define SWTACT_VTBL 0x1003ED6C
#define m_heapsize 0x3000

enum { dRes_INDEX_ITACT_BDL_ITACT_e = 3 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* dComIfGp_checkPlayerStatus0/1(0, flag): play +0x5CD8 / +0x5CDC */
static inline u32 dComIfGp_checkPlayerStatus0(u32 flag) { return gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & flag; }
static inline u32 dComIfGp_checkPlayerStatus1(u32 flag) { return gabi::load<u32>(dComIfGp_ea() + 0x5CDC) & flag; }
/* dComIfGp_event_runCheck(): play +0x5292 (dEvt_control_c::mMode) */
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
enum { daPyStts0_SHIP_RIDE_e = 0x10000, daPyStts1_WIND_WAKER_CONDUCT_e = 0x1 };

class daSwTact_c : public fopAc_ac_c {
public:
    f32 getR() { return mRadius * scale.x; }

    bool _delete();
    BOOL CreateHeap();
    void CreateInit();
    void set_mtx();
    s32 getAnswer();
    cPhs_State _create();
    bool _execute();
    bool _draw();

    /* 0x3AC */ request_of_phase_process_class mPhs; /* GameCube 0x290 */
    /* 0x3B4 */ gptr<J3DModel> model;
    /* 0x3B8 */ be<f32> mRadius;
    /* 0x3BC */ be<u32> mSwitchNo;
    /* 0x3C0 */ be<u8> mAnswer;
    /* 0x3C1 */ be<u8> mTrigger;
    /* 0x3C2 */ be<u8> mPlayerStatus;
    /* 0x3C3 */ u8 _3C3;
};
WWHD_OFFSET(daSwTact_c, model, 0x3B4);
WWHD_OFFSET(daSwTact_c, mPlayerStatus, 0x3C2);

namespace daSwTact_prm {
inline u32 getSwitchNo(daSwTact_c* i_this) { return (fopAcM_GetParam(i_this) >> 0) & 0xFF; }
inline u32 getAnswer(daSwTact_c* i_this) { return (fopAcM_GetParam(i_this) >> 8) & 0xFF; }
inline u32 getModel(daSwTact_c* i_this) { return (fopAcM_GetParam(i_this) >> 16) & 0x0F; }
}  // namespace daSwTact_prm

/* 024A38A8 */
bool daSwTact_c::_delete() {
    WWHD_FUNC(0x024A38A8, bool, this);
    dComIfG_resDelete(&mPhs, M_arcname);
    return true;
}
VERIFY(0x024A38A8, &daSwTact_c::_delete);

/* 024A3710 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x024A3710, BOOL, i_this);
    return ((daSwTact_c*)i_this)->CreateHeap();
}
VERIFY(0x024A3710, CheckCreateHeap);

/* 024A3674 */
BOOL daSwTact_c::CreateHeap() {
    WWHD_FUNC(0x024A3674, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_ITACT_BDL_ITACT_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0xe1, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x1003ED7C), 0xE1, STR(0x1003ED8C));
    model = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (!model)
        return FALSE;
    return TRUE;
}
VERIFY(0x024A3674, &daSwTact_c::CreateHeap);

/* 024A3714 */
void daSwTact_c::CreateInit() {
    WWHD_FUNC(0x024A3714, void, this);
    /* cull_size[] = {-120, -10, -120, 120, 10, 120} */
    mSwitchNo = daSwTact_prm::getSwitchNo(this);
    mAnswer = daSwTact_prm::getAnswer(this);
    if (daSwTact_prm::getModel(this) == 1) {
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(model)); /* fopAcM_SetMtx */
        f32 scaleZ = scale.z;
        f32 scaleX = scale.x;
        fopAcM_setCullSizeBox(this, -120.0f * scaleX, -10.0f, -120.0f * scaleZ, 120.0f * scaleX, 10.0f, 120.0f * scaleZ);
        mRadius = 100.0f;
    } else {
        mRadius = 50.0f;
    }
}
VERIFY(0x024A3714, &daSwTact_c::CreateInit);

/* 024A396C */
void daSwTact_c::set_mtx() {
    WWHD_FUNC(0x024A396C, void, this);
    if (daSwTact_prm::getModel(this) == 1) {
        J3DModel_setBaseScale(model, &scale);
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    }
}
VERIFY(0x024A396C, &daSwTact_c::set_mtx);

/* 024A37CC */
cPhs_State daSwTact_c::_create() {
    WWHD_FUNC(0x024A37CC, cPhs_State, this);
    /* fopAcM_ct(this, daSwTact_c): HD: fopAc_ac_c has a vtable */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = SWTACT_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    s32 result = cPhs_COMPLEATE_e;
    if (daSwTact_prm::getModel(this) == 1) {
        result = dComIfG_resLoad(&mPhs, M_arcname);

        if (result == cPhs_COMPLEATE_e) {
            if (!fopAcM_entrySolidHeap(this, 0x024A3710 /* CheckCreateHeap */, m_heapsize)) {
                return cPhs_ERROR_e;
            }
        }
    }

    if (result == cPhs_COMPLEATE_e) {
        CreateInit();
    }

    return result;
}
VERIFY(0x024A37CC, &daSwTact_c::_create);

/* 024A3948 (HD: the switch became a byte table {0, 1, 5, 2, 3, 4} at 0x1003EDB8) */
s32 daSwTact_c::getAnswer() {
    WWHD_FUNC(0x024A3948, s32, this);
    switch (mAnswer) {
    case 0:
        return 0;
    case 1:
        return 1;
    case 2:
        return 5;
    case 3:
        return 2;
    case 4:
        return 3;
    case 5:
        return 4;
    case 0xFF:
    default:
        return -1;
    }
}
VERIFY(0x024A3948, &daSwTact_c::getAnswer);

/* 024A3A48 */
bool daSwTact_c::_execute() {
    WWHD_FUNC(0x024A3A48, bool, this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0); /* daPy_getPlayerActorClass() */
    u32 stts1 = dComIfGp_checkPlayerStatus1(daPyStts1_WIND_WAKER_CONDUCT_e);

    if (player == nullptr || dComIfGp_checkPlayerStatus0(daPyStts0_SHIP_RIDE_e))
        return true;

    /* (player->current.pos - current.pos).absXZ() */
    gabi::Local<cXyz> d;
    cXyz_mi(&player->current.pos, d, &current.pos);
    gabi::Local<cXyz> xz;
    xz->x = d->x;
    xz->y = 0.0f;
    xz->z = d->z;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    if (!(dist > getR())) {
        if (mPlayerStatus != stts1 && stts1 != 0) {
            /* player->setTactZev(fopAcM_GetID(this), getAnswer(), NULL): virtual, slot 0x84 */
            u32 fn = gabi::load<u32>(player->__vtbl + 0x84);
            s32 answer = getAnswer();
            gabi::call_ptr(fn, player, gabi::load<u32>(gabi::ea(this) + 4), answer, (u32)0);
        }

        /* player->getTactMusic(): virtual, slot 0x2C */
        s32 tactMusic = gabi::call_ptr<s32>(gabi::load<u32>(player->__vtbl + 0x2C), player);
        switch (mAnswer) {
        case 0:
            if (tactMusic == 0)
                mTrigger = true;
            break;
        case 1:
            if (tactMusic == 1)
                mTrigger = true;
            break;
        case 2:
            if (tactMusic == 5)
                mTrigger = true;
            break;
        case 3:
            if (tactMusic == 2)
                mTrigger = true;
            break;
        case 4:
            if (tactMusic == 3)
                mTrigger = true;
            break;
        case 5:
            if (tactMusic == 4)
                mTrigger = true;
            break;
        case 0xFF:
            if (tactMusic == 0 || tactMusic == 1 || tactMusic == 2 || tactMusic == 3 || tactMusic == 4 || tactMusic == 5)
                mTrigger = true;
            break;
        }
    }

    if (mTrigger != 0 && !dComIfGp_event_runCheck()) {
        dComIfGs_onSwitch(mSwitchNo, home.roomNo); /* fopAcM_onSwitch */
        mTrigger = 0;
    }

    mPlayerStatus = stts1;
    set_mtx();

    return true;
}
VERIFY(0x024A3A48, &daSwTact_c::_execute);

/* 024A38DC */
bool daSwTact_c::_draw() {
    WWHD_FUNC(0x024A38DC, bool, this);
    if (daSwTact_prm::getModel(this) == 0)
        return TRUE;

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    mDoExt_modelUpdateDL(model);
    return TRUE;
}
VERIFY(0x024A38DC, &daSwTact_c::_draw);

/* 024A38A4 */
static cPhs_State daSwTact_Create(void* i_this) {
    WWHD_FUNC(0x024A38A4, cPhs_State, i_this);
    return ((daSwTact_c*)i_this)->_create();
}
VERIFY(0x024A38A4, daSwTact_Create);

/* 024A38D8 */
static BOOL daSwTact_Delete(void* i_this) {
    WWHD_FUNC(0x024A38D8, bool, i_this);
    return ((daSwTact_c*)i_this)->_delete();
}
VERIFY(0x024A38D8, daSwTact_Delete);

/* 024A3944 */
static BOOL daSwTact_Draw(void* i_this) {
    WWHD_FUNC(0x024A3944, bool, i_this);
    return ((daSwTact_c*)i_this)->_draw();
}
VERIFY(0x024A3944, daSwTact_Draw);

/* 024A3C50 */
static BOOL daSwTact_Execute(void* i_this) {
    WWHD_FUNC(0x024A3C50, bool, i_this);
    return ((daSwTact_c*)i_this)->_execute();
}
VERIFY(0x024A3C50, daSwTact_Execute);

/* 024A3CFC */
static BOOL daSwTact_IsDelete(void* i_this) {
    WWHD_FUNC(0x024A3CFC, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x024A3CFC, daSwTact_IsDelete);

/* 024A3C54: static initialisation (header statics only) */
static void __sinit_d_a_swtact_cpp() {
    WWHD_FUNC(0x024A3C54, void, (u32)0);
    sinit_header_statics(0x1046E1B8, 0x101D158C);
}
VERIFY(0x024A3C54, __sinit_d_a_swtact_cpp);

/* 024A3CE8: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(SafeString* s, s32 flags) {
    WWHD_FUNC(0x024A3CE8, void, s, flags);
    if (s != nullptr && (flags & 1))
        operator_delete(s);
}
VERIFY(0x024A3CE8, SafeString_dt);

/* 024A3D04: daSwTact_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daSwTact_c_dt(daSwTact_c* i_this, s32 flags) {
    WWHD_FUNC(0x024A3D04, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024A3D04, daSwTact_c_dt);

/* 024A3D58: sead::SafeString virtual at vtable +0x14 (assureTerminationImpl_, empty in this copy) */
static void SafeString_assureTermination(SafeString*) {
    WWHD_FUNC(0x024A3D58, void, (u32)0);
}
VERIFY(0x024A3D58, SafeString_assureTermination);
