/**
 * d_a_tag_attention.cpp (WWHD)
 * Tag - Attention: makes the player look at the tag while inside a sphere or box.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tag_attention.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define ACT_VTBL 0x1003EF28           /* HD: daTagAttention::Act_c vtable */
#define sph_check_src 0x101D1720      /* dCcD_SrcSph (.data) */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 024EBF40 dAttLook_c::requestF(actor, angle, type); the look object is play + 0x595C
 * (dComIfGp_att_Look2RequestF) */
static inline void dComIfGp_att_Look2RequestF(fopAc_ac_c* a, s32 angle, s32 type) {
    gabi::call(0x024EBF40, dComIfGp_ea() + 0x595C, a, angle, type);
}
/* 02515AE8 dCcD_Sph::~dCcD_Sph (shared, out of line) */
static inline void dCcD_Sph_dt(dCcD_Sph* s, s32 flags) { gabi::call(0x02515AE8, s, flags); }

namespace daTagAttention {
struct Act_c : fopAc_ac_c {
    enum Prm_e {
        PRM_TYPE_W = 0x02,
        PRM_TYPE_S = 0x08,
        PRM_SWSAVE_W = 0x08,
        PRM_SWSAVE_S = 0x00,
    };
    int prm_get_Type();
    int prm_get_swSave();
    cPhs_State _create();
    bool _execute();

    /* 0x3AC */ be<u8> m_b0x290;   /* GameCube 0x290 */
    /* 0x3AD */ u8 _3AD[0x3B8 - 0x3AD];
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Sph mSph;
};
WWHD_OFFSET(Act_c, mStts, 0x3B8);
WWHD_OFFSET(Act_c, mSph, 0x3F4);
WWHD_SIZE(Act_c, 0x520);
}  // namespace daTagAttention
using daTagAttention::Act_c;

/* 024A5920: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU) */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    WWHD_FUNC(0x024A5920, u32, a, width, shift);
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
VERIFY(0x024A5920, PrmAbstract);
int Act_c::prm_get_Type() { return PrmAbstract(this, PRM_TYPE_W, PRM_TYPE_S); }
int Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }

/* 024A54DC */
cPhs_State Act_c::_create() {
    WWHD_FUNC(0x024A54DC, cPhs_State, this);
    /* fopAcM_ct(this, Act_c): HD: vtable, inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = ACT_VTBL;
            dCcD_Stts_ct(&mStts);
            gabi::call(0x025166F0, &mSph); /* dCcD_Sph::dCcD_Sph */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    mStts.Init(0xFF, 0xFF, this);
    mSph.Set(gabi::at<dCcD_SrcSph>(sph_check_src));
    mSph.SetStts(&mStts);
    return cPhs_COMPLEATE_e;
}
VERIFY(0x024A54DC, &Act_c::_create);

/* 024A5598: chk_inside() inlined */
bool Act_c::_execute() {
    WWHD_FUNC(0x024A5598, bool, this);
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

    gabi::Local<cXyz> vec;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    cXyz_mi(&player->current.pos, vec, &current.pos);
    if (argument == 0) {
        f32 distance = std_sqrtf(PSVECSquareMag(vec));
        if (distance > scale.x * 100.0f)
            return true;
    } else {
        f32 x = vec->x;
        if (current.angle.y != 0) {
            s16 yRotAngle = current.angle.y;
            f32 z = vec->z;
            f32 nx = gabi::fmsubs(x, cM_scos(yRotAngle), z * cM_ssin(yRotAngle));
            f32 nz = gabi::fmadds(x, cM_ssin(yRotAngle), z * cM_scos(yRotAngle));
            vec->z = nz;
            vec->x = nx;
            x = nx;
        }
        f32 r = scale.x * 100.0f;
        if (x < -r || x > r)
            return true;
        r = scale.y * 100.0f;
        if (vec->y < -r || vec->y > r)
            return true;
        r = scale.z * 100.0f;
        if (vec->z < -r || vec->z > r)
            return true;
    }
    dComIfGp_att_Look2RequestF(this, 0x6000, 1);
    return true;
}
VERIFY(0x024A5598, &Act_c::_execute);

/* method table (0x101D17B4): Create, Delete, Execute, IsDelete, Draw */
/* 024A5800 */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x024A5800, cPhs_State, i_this);
    return ((Act_c*)i_this)->_create();
}
VERIFY(0x024A5800, Mthd_Create);
/* 024A5908 */
static bool Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x024A5908, bool, i_this);
    return TRUE;
}
VERIFY(0x024A5908, Mthd_Delete);
/* 024A5804 */
static bool Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x024A5804, bool, i_this);
    return ((Act_c*)i_this)->_execute();
}
VERIFY(0x024A5804, Mthd_Execute);
/* 024A5910 */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x024A5910, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024A5910, Mthd_Draw);
/* 024A5918 */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x024A5918, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024A5918, Mthd_IsDelete);

/* 024A5808 */
static void __sinit_d_a_tag_attention_cpp() {
    WWHD_FUNC(0x024A5808, void, (u32)0);
    sinit_header_statics(0x1046E21C, 0x101D1760);
}
VERIFY(0x024A5808, __sinit_d_a_tag_attention_cpp);

/* 024A589C: Act_c deleting destructor (compiler-generated, HD virtual destructor) */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x024A589C, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Sph_dt(&i_this->mSph, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024A589C, Act_c_dt);
