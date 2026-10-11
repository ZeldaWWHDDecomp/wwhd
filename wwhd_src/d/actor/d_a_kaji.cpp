/**
 * d_a_kaji.cpp
 * Object - Ship's wheel (Tetra's Ship)
 *
 * WWHD, written in GameCube style and verified against the recompiled original
 * (tools/verify, d_a_kaji).
 */
#include "d/actor/d_a_kaji.h"

// HD: this TU's inline-constructor vtables and common header statics.
const gc_tu_data gc_tu = {
    .safeString = 0x1001226C,
    .headerStatics = {0x10464888, 0x101B7BAC, 0x10464894},
};

static daObjPirateship::Act_c* l_p_ship;
// HD: M_arcname and the two animation lookups use distinct copies of "Kaji".
static const char arcname[5] = {0x4B, 0x61, 0x6A, 0x69, 0};
const char* daKaji_c::M_arcname = reinterpret_cast<const char*>(arcname);

/* inlined into daKajiExecute */
void daKaji_c::set_mtx() {
    mpMorf->getModel()->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    mpMorf->getModel()->setBaseTRMtx(mDoMtx_stack_c::get());
    MTXCopy(mDoMtx_stack_c::get(), &mMtx);
}

/* inlined into daKajiCreate */
daKaji_c::daKaji_c() {
    // HD: GHS constructs the base and stores this class's vtable.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}

/* 02184F4C */
BOOL daKaji_c::CreateHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_KAJI_BDL_ASODA_e);
    JUT_ASSERT(0x55, modelData != NULL);

    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes("Kaji", dRes_INDEX_KAJI_BCK_KJ_WAIT_e);
    void* bas = dComIfG_getObjectRes("Kaji", dRes_INDEX_KAJI_BAS_KJ_WAIT_e);
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, J3DFrameCtrl::EMode_LOOP, 0.0f, 0,
                                                  -1, 1, bas, 0x00080000, 0x11000002);
    mpMorf = morf;

    return morf && morf->getModel(); /* HD: tests the new pointer, not a reload of mpMorf */
}

/* 02185068 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return ((daKaji_c*)i_this)->CreateHeap();
}

/* inlined into daKajiCreate */
cPhs_State daKaji_c::_create() {
    fopAcM_ct(this, daKaji_c);

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, CheckCreateHeap, 0x660)) {
            mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
            mDoMtx_stack_c::YrotM(shape_angle.y);
            mDoMtx_stack_c::scaleM(scale.x, scale.y, scale.z);
            PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);

            l_p_ship = (daObjPirateship::Act_c*)fopAcM_SearchByID(parentActorID);
        } else {
            return cPhs_ERROR_e;
        }

        fopAcM_SetMtx(this, mpMorf->getModel()->getBaseTRMtx());
        /* HD: GameCube box (-80, -80, -20)..(80, 80, 20) */
        fopAcM_setCullSizeBox(this, -80.0f, 20.0f, 50.0f, 80.0f, 180.0f, 90.0f);

        PSMTXCopy(mpMorf->getModel()->getBaseTRMtx(), &mMtx);
    }

    return phase_state;
}

/* 0218506C */
static cPhs_State daKajiCreate(void* i_this) {
    return ((daKaji_c*)i_this)->_create();
}

/* 02185214 */
static BOOL daKajiDelete(void* i_this) {
    dComIfG_resDeleteDemo(&((daKaji_c*)i_this)->mPhs, daKaji_c::M_arcname);
    return TRUE;
}

/* inlined into daKajiExecute */
bool daKaji_c::_execute() {
    /* Copy the ship's transform (plus an offset) to the helm. */
    cXyz offset;
    offset.x = 0.0f;
    offset.y = 740.0f + REG_F(10, 10);
    offset.z = -858.0f + REG_F(10, 11);
    PSMTXMultVec(l_p_ship->mModel->getBaseTRMtx(), &offset, &current.pos); /* cMtx_multVec */

    daObjPirateship::Act_c* ship = l_p_ship;
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
    return ((daKaji_c*)i_this)->_execute();
}

/* inlined into daKajiDraw */
bool daKaji_c::_draw() {
    if (!l_p_ship->m2CC) {
        return true;
    }
    g_env_light.settingTevStruct(TEV_TYPE_ACTOR, &current.pos, &tevStr);
    mDoExt_McaMorf* morf = mpMorf;
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, morf->getModel(), &tevStr);
    mDoExt_McaMorf_update(mpMorf);
    return true;
}

/* 021853D8 */
static BOOL daKajiDraw(void* i_this) {
    return ((daKaji_c*)i_this)->_draw();
}

/* 021854F8 */
static BOOL daKajiIsDelete(void* i_this) {
    return TRUE;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 02185450 */
static void __sinit_d_a_kaji_cpp() {
    __sinit_header_statics();
}

/* 021854E4 */
// HD: this TU's sead::SafeString deleting destructor
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02185500 */
// HD: compiler-generated deleting destructor
static void daKaji_c_dt(daKaji_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02185554 */
// HD: this TU's empty sead::SafeString virtual
static void SafeString_assureTerminationImpl(void* p) {
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
str 10012298 "Kaji"
str 100122A0
str 100122B0 "modelData != NULL"
data arcname 100122F8
data l_p_ship 10464884
ext 025E5434 void mDoExt_McaMorf_update(mDoExt_McaMorf*)
*/
