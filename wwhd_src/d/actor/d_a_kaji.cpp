/**
 * d_a_kaji.cpp (WWHD)
 * Object - Ship's wheel (Tetra's Ship)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kaji.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x100122F8) /* "Kaji" */
#define SAFESTRING_VTBL 0x1001226C
#define KAJI_VTBL 0x10012284 /* HD: daKaji_c vtable */

enum { /* HD archive order */
    dRes_INDEX_KAJI_BDL_ASODA_e = 0x11,
    dRes_INDEX_KAJI_BCK_KJ_WAIT_e = 0xE,
    dRes_INDEX_KAJI_BAS_KJ_WAIT_e = 8,
};

/* daObjPirateship::Act_c, only the fields used here (HD: GameCube 0x2CC/0x2D0 + 0x118) */
namespace daObjPirateship {
struct Act_c : fopAc_ac_c {
    /* 0x3AC */ u8 _3AC[0x3E4 - 0x3AC];
    /* 0x3E4 */ be<u8> m2CC;
    /* 0x3E5 */ u8 _3E5[3];
    /* 0x3E8 */ gptr<J3DModel> mModel;
};
WWHD_OFFSET(Act_c, m2CC, 0x3E4);
WWHD_OFFSET(Act_c, mModel, 0x3E8);
}  // namespace daObjPirateship

/* static daObjPirateship::Act_c* l_p_ship */
static gptr<daObjPirateship::Act_c>& l_p_ship() { return *gabi::at<gptr<daObjPirateship::Act_c>>(0x10464884); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E5434 mDoExt_McaMorf::update() */
static inline void McaMorf_update(mDoExt_McaMorf* m) { gabi::call(0x025E5434, m); }

struct daKaji_c : fopAc_ac_c {
    void set_mtx() {
        J3DModel_setBaseScale(mpMorf->getModel(), &scale);
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
        J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
        PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
    }
    cPhs_State _create();
    bool _delete();
    bool _execute();
    bool _draw();
    BOOL CreateHeap();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ u8 pad[4];
    /* 0x3B8 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3BC */ Mtx34 mMtx;
};
WWHD_OFFSET(daKaji_c, mpMorf, 0x3B8);
WWHD_OFFSET(daKaji_c, mMtx, 0x3BC);

/* 02184F4C */
BOOL daKaji_c::CreateHeap() {
    WWHD_FUNC(0x02184F4C, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_KAJI_BDL_ASODA_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0x55, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x100122A0), 0x55, STR(0x100122B0));

    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10012298) /* "Kaji" */, dRes_INDEX_KAJI_BCK_KJ_WAIT_e, SAFESTRING_VTBL);
    void* bas = dComIfG_getObjectRes(STR(0x10012298), dRes_INDEX_KAJI_BAS_KJ_WAIT_e, SAFESTRING_VTBL);
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, J3DFrameCtrl::EMode_LOOP, 0.0f, 0,
                                                  -1, 1, bas, 0x00080000, 0x11000002);
    mpMorf = morf;

    return morf && morf->getModel(); /* HD: tests the new pointer, not a reload of mpMorf */
}
VERIFY(0x02184F4C, &daKaji_c::CreateHeap);

/* 02185068 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02185068, BOOL, i_this);
    return ((daKaji_c*)i_this)->CreateHeap();
}
VERIFY(0x02185068, CheckCreateHeap);

/* daKaji_c::_create, inlined in daKajiCreate */
cPhs_State daKaji_c::_create() {
    /* fopAcM_ct(this, daKaji_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = KAJI_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, 0x02185068 /* CheckCreateHeap */, 0x660)) {
            mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
            mDoMtx_stack_c::YrotM(shape_angle.y);
            mDoMtx_stack_scaleM(scale.x, scale.y, scale.z);
            PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);

            l_p_ship() = (daObjPirateship::Act_c*)fopAcM_SearchByID(parentActorID);
        } else {
            return cPhs_ERROR_e;
        }

        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
        /* HD: GameCube box (-80, -80, -20)..(80, 80, 20) */
        fopAcM_setCullSizeBox(this, -80.0f, 20.0f, 50.0f, 80.0f, 180.0f, 90.0f);

        PSMTXCopy(J3DModel_getBaseTRMtx(mpMorf->getModel()), &mMtx);
    }

    return phase_state;
}

/* 0218506C */
static cPhs_State daKajiCreate(void* i_this) {
    WWHD_FUNC(0x0218506C, cPhs_State, i_this);
    return ((daKaji_c*)i_this)->_create();
}
VERIFY(0x0218506C, daKajiCreate);

/* 02185214 (_delete inlined) */
static BOOL daKajiDelete(void* i_this) {
    WWHD_FUNC(0x02185214, BOOL, i_this);
    dComIfG_resDelete(&((daKaji_c*)i_this)->mPhs, M_arcname); /* dComIfG_resDeleteDemo */
    return TRUE;
}
VERIFY(0x02185214, daKajiDelete);

/* daKaji_c::_execute, inlined in daKajiExecute */
bool daKaji_c::_execute() {
    /* Copy the ship's transform (plus an offset) to the helm. */
    gabi::Local<cXyz> offset;
    offset->x = 0.0f;
    offset->y = 740.0f + REG_F(10, 10);
    offset->z = -858.0f + REG_F(10, 11);
    PSMTXMultVec(J3DModel_getBaseTRMtx(l_p_ship()->mModel), offset.get(), &current.pos); /* cMtx_multVec */

    daObjPirateship::Act_c* ship = l_p_ship();
    s16 x = ship->shape_angle.x;
    shape_angle.x = x;
    s16 y = ship->shape_angle.y;
    shape_angle.y = y;
    s16 z = ship->shape_angle.z;
    shape_angle.z = z;
    current.angle.x = x;
    current.angle.y = y;
    current.angle.z = z;

    mpMorf->play(nullptr, 0, 0);

    set_mtx();

    return FALSE;
}

/* 02185244 */
static BOOL daKajiExecute(void* i_this) {
    WWHD_FUNC(0x02185244, BOOL, i_this);
    return ((daKaji_c*)i_this)->_execute();
}
VERIFY(0x02185244, daKajiExecute);

/* daKaji_c::_draw, inlined in daKajiDraw */
bool daKaji_c::_draw() {
    if (!l_p_ship()->m2CC) {
        return true;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    mDoExt_McaMorf* morf = mpMorf;
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, morf->getModel(), &tevStr);
    McaMorf_update(mpMorf);
    return true;
}

/* 021853D8 */
static BOOL daKajiDraw(void* i_this) {
    WWHD_FUNC(0x021853D8, BOOL, i_this);
    return ((daKaji_c*)i_this)->_draw();
}
VERIFY(0x021853D8, daKajiDraw);

/* 021854F8 */
static BOOL daKajiIsDelete(void* i_this) {
    WWHD_FUNC(0x021854F8, BOOL, i_this);
    return TRUE;
}
VERIFY(0x021854F8, daKajiIsDelete);

/* 02185450 */
static void __sinit_d_a_kaji_cpp() {
    WWHD_FUNC(0x02185450, void, (u32)0);
    sinit_header_statics(0x10464888, 0x101B7BAC);
}
VERIFY(0x02185450, __sinit_d_a_kaji_cpp);

/* 021854E4: sead::SafeString deleting destructor (this TU's SafeString vtable) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021854E4, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x021854E4, SafeString_dt);

/* 02185500: daKaji_c deleting destructor (vtable +0xC) */
static void daKaji_c_dt(daKaji_c* i_this, s32 flags) {
    WWHD_FUNC(0x02185500, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02185500, daKaji_c_dt);

/* 02185554: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void* p) { WWHD_FUNC(0x02185554, void, p); }
VERIFY(0x02185554, SafeString_assureTerminationImpl);
