/**
 * d_a_tag_attention.cpp (WWHD)
 * Tag - Attention: makes the player look at the tag while inside a sphere or box.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tag_attention.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_tag_attention.h"
#include "gc_api/f_op/f_op_actor_mng.h"

using daTagAttention::Act_c;
static dCcD_SrcSph sph_check_src;

static void dComIfGp_att_Look2RequestF(fopAc_ac_c* actor, s32 angle, s32 type) {
    reinterpret_cast<tag_attention_play_view*>(dComIfGp_get())->mLook.requestF(actor, angle, type);
}

/* 024A5920 */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
int Act_c::prm_get_Type() { return PrmAbstract(this, PRM_TYPE_W, PRM_TYPE_S); }
int Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }

/* 024A54DC */
cPhs_State Act_c::_create() {
    /* fopAcM_ct(this, Act_c): HD: vtable, inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = Act_c::__vt;
            dCcD_Stts_ct(&mStts);
            dCcD_Sph_construct(&mSph);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    mStts.Init(0xFF, 0xFF, this);
    mSph.Set(&sph_check_src);
    mSph.SetStts(&mStts);
    return cPhs_COMPLEATE_e;
}

/* 024A5598 */
bool Act_c::_execute() {
    mSph.SetC(&current.pos);
    mSph.SetR(scale.x * 100.0f);
    dComIfG_Ccsp_Set(&mSph);
    m_b0x290 = true;
    int type = prm_get_Type(); /* evaluated once (GHS) */
    /* chk_inside(&unused): GHS knows m_b0x290 is still true for types 0/3 and reloads it
     * only after the isSwitch calls */
    if (type == 1) {
        if (fopAcM_isSwitch(this, prm_get_swSave()) == FALSE) {
            m_b0x290 = false;
            return true;
        }
        if (!m_b0x290)
            return true;
    } else if (type == 2) {
        if (fopAcM_isSwitch(this, prm_get_swSave()) != FALSE) {
            m_b0x290 = false;
            return true;
        }
        if (!m_b0x290)
            return true;
    }

    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    cXyz vec = player->current.pos - current.pos;
    if (argument == 0) {
        f32 distance = std_sqrtf(PSVECSquareMag(&vec)); // HD: single-precision square root
        if (distance > scale.x * 100.0f)
            return true;
    } else {
        f32 x = vec.x;
        if (current.angle.y != 0) {
            s16 yRotAngle = current.angle.y;
            f32 z = vec.z;
            f32 nx = gabi::fmsubs(x, cM_scos(yRotAngle), z * cM_ssin(yRotAngle));
            f32 nz = gabi::fmadds(x, cM_ssin(yRotAngle), z * cM_scos(yRotAngle));
            vec.z = nz;
            vec.x = nx;
            x = nx;
        }
        f32 r = scale.x * 100.0f;
        if (x < -r || x > r)
            return true;
        r = scale.y * 100.0f;
        if (vec.y < -r || vec.y > r)
            return true;
        r = scale.z * 100.0f;
        if (vec.z < -r || vec.z > r)
            return true;
    }
    dComIfGp_att_Look2RequestF(this, 0x6000, 1);
    return true;
}

/* method table (0x101D17B4): Create, Delete, Execute, IsDelete, Draw */
/* 024A5800 */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->_create();
}
/* 024A5908 */
static bool Mthd_Delete(void* i_this) {
    return TRUE;
}
/* 024A5804 */
static bool Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->_execute();
}
/* 024A5910 */
static BOOL Mthd_Draw(void* i_this) {
    return TRUE;
}
/* 024A5918 */
static BOOL Mthd_IsDelete(void* i_this) {
    return TRUE;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 024A5808 */
static void __sinit_d_a_tag_attention_cpp() {
    sinit_header_statics(0x1046E21C, 0x101D1760);
}

/* 024A589C */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Sph_dt(&i_this->mSph, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data sph_check_src 101D1720
ext 024EBF40 void dAttLook_c::requestF(fopAc_ac_c* actor, s32 angle, s32 type)
ext 025166F0 void dCcD_Sph_construct(dCcD_Sph* sphere)
ext 02515AE8 void dCcD_Sph_dt(dCcD_Sph* sphere, s32 flags)
*/
