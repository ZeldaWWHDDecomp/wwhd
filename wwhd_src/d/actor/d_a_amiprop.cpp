/**
 * d_a_amiprop.cpp (WWHD)
 * Object - Rotating mesh propeller (Hami1)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_amiprop.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_amiprop.h"

// HD: GHS emits translation unit specific SafeString and actor vtables.
const gc_tu_data gc_tu = {.safeString = 0x10007124};
static const char* m_arcname = "Hami1";
static dCcD_SrcCyl l_cyl_src;

enum {
    dRes_INDEX_HAMI1_BDL_HAMI1_e = 4,
    dRes_INDEX_HAMI1_DZB_HAMI1_e = 7,
};
enum { JA_SE_OBJ_AMI_PROP = 0x6166 };

// HD: inline construction stores this translation unit's cylinder AABB vtable.
daAmiProp_c::daAmiProp_c() {
    fopAc_ac_c_ct(this);
    __vtbl = 0x1000714C;
    dCcD_Stts_ct(&mStts);
    dCcD_Cyl_ct(&mCyl, 0x1000713C);
}

namespace daAmiProp_prm {
inline u8 getSwitchNo(daAmiProp_c* i_this) { return (fopAcM_GetParam(i_this) >> 0) & 0xFF; }
inline u8 getType(daAmiProp_c* i_this) { return (fopAcM_GetParam(i_this) >> 8) & 0xF; }
} // namespace daAmiProp_prm

/* 02050C1C */
bool daAmiProp_c::_delete() {
    if (heap != nullptr) {
        dComIfG_Bgsp()->Release(mpBgW);
    }
    dComIfG_resDelete(&mPhase, m_arcname);
    return true;
}

/* 02050914 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return ((daAmiProp_c*)i_this)->CreateHeap();
}

/* 02050800 */
BOOL daAmiProp_c::CreateHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arcname, dRes_INDEX_HAMI1_BDL_HAMI1_e);
    JUT_ASSERT(255, modelData != NULL);

    J3DModel* model = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    mpModel = model;
    if (model == nullptr) {
        return FALSE;
    }
    model->setUserArea(reinterpret_cast<uintptr_t>(this));
    setMoveBGMtx();
    mpBgW = reinterpret_cast<daAmiProp_bgW_c*>(new_dBgW());
    if (mpBgW != nullptr) {
        cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes(m_arcname, dRes_INDEX_HAMI1_DZB_HAMI1_e);
        if (mpBgW->Set(dzb, dBgW::MOVE_BG_e, &unk_40C) == true) {
            return FALSE;
        }
        mpBgW->SetCrrFunc(0x024EE708 /* dBgS_MoveBGProc_TypicalRotY */);
    } else {
        return FALSE;
    }
    return TRUE;
}

/* 02050A04 */
void daAmiProp_c::CreateInit() {
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    fopAcM_setCullSizeBox(this, -850.0f, -30.0f, -850.0f, 850.0f, 30.0f, 850.0f);
    fopAcM_setCullSizeFar(this, 1.0f);
    mStts.Init(255, 255, this);
    mCyl.Set(&l_cyl_src);
    mCyl.SetStts(&mStts);
    unk_43C = daAmiProp_prm::getSwitchNo(this);
    unk_446 = (s16)(current.angle.x + 0x4000);
    dComIfG_Bgsp()->Regist(mpBgW, this);
    set_mtx();
    mpBgW->Move();
}

/* 02050AE0 */
cPhs_State daAmiProp_c::_create() {
    fopAcM_ct(this, daAmiProp_c);
    cPhs_State ret = dComIfG_resLoad(&mPhase, m_arcname);
    if (ret == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0xC00)) {
            return cPhs_ERROR_e;
        }
        CreateInit();
    }
    return ret;
}

/* 02050918 */
void daAmiProp_c::set_mtx() {
    mpModel->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    MTXCopy(mDoMtx_stack_c::get(), &unk_40C);
}

/* 02050788 */
void daAmiProp_c::setMoveBGMtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    mDoMtx_stack_c::scaleM(scale.x, scale.y, scale.z);
    MTXCopy(mDoMtx_stack_c::get(), &unk_40C);
}

/* 02050D14 */
bool daAmiProp_c::_execute() {
    s32 is_switch = fopAcM_isSwitch(this, unk_43C);
    if ((u32)is_switch != (u32)unk_440) {
        unk_444 = true;
    }

    if (unk_444) {
        s16 tmp;
        switch (daAmiProp_prm::getType(this)) {
        case 0:
            tmp = cLib_addCalcAngleS(&current.angle.x, unk_446, 10, 0x200, 0x10);
            break;
        case 1:
            tmp = cLib_addCalcAngleS(&current.angle.y, unk_446, 10, 0x200, 0x10);
            break;
        default:
            // HD: GHS takes the sound path for types outside the two rotation axes.
            tmp = 1;
            break;
        }
        if (tmp == 0) {
            unk_444 = false;
            unk_446 = (s16)(unk_446 + 0x4000);
            dVibration_c* vib = dComIfGp_getVibration();
            gabi::Local<cXyz> pos;
            pos->x = 0.0f;
            pos->y = 1.0f;
            pos->z = 0.0f;
            vib->StartShock(4, -0x21, pos.get());
        } else {
            // HD: fopAcM_seStart is inlined without null checks here.
            s32 reverb = dComIfGp_getReverb(current.roomNo);
            mDoAud_seStart(JA_SE_OBJ_AMI_PROP, &eyePos, 0, reverb);
        }
    }
    unk_440 = is_switch;
    set_mtx();
    mpBgW->Move();
    return true;
}

/* 02050C78 */
bool daAmiProp_c::_draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return true;
}

/* method table entries (HD: tail branches) */
/* 02050C18 */
static cPhs_State daAmiProp_Create(void* i_this) {
    return ((daAmiProp_c*)i_this)->_create();
}
/* 02050C74 */
static BOOL daAmiProp_Delete(void* i_this) {
    return ((daAmiProp_c*)i_this)->_delete();
}
/* 02050D10 */
static BOOL daAmiProp_Draw(void* i_this) {
    return ((daAmiProp_c*)i_this)->_draw();
}
/* 02050ECC */
static BOOL daAmiProp_Execute(void* i_this) {
    return ((daAmiProp_c*)i_this)->_execute();
}
/* 02050F78 */
static BOOL daAmiProp_IsDelete(void*) {
    return TRUE;
}

/* ---- compiler-generated (HD) ---- */

/* 02050ED0 */
static void __sinit_d_a_amiprop_cpp() {
    sinit_header_statics(0x104613F8, 0x1018FC50); // HD: shared-header static objects.
}

/* 02050F64 */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02050F80 */
static void daAmiProp_c_dt(daAmiProp_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02050FEC */
static void empty_virtual(void* p) {
}

/* WWHD glue (tools/verify/autoglue.py)
str 100071A0
str 1000715C
str 1000716C "modelData != NULL"
data l_cyl_src 1018FC0C
*/
