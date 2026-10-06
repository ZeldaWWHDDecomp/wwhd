/* daTsubo::Act_c (small carriable objects: pots, skulls, barrels, ...), WWHD layout.
 *
 * GameCube -> WWHD: every member +0x11C (fopAc_ac_c); size 0x93C (constructor 024CA1DC
 * allocates 0x93C; GameCube 0x820). The HD vtable pointer is at +0xB4. The data tables keep
 * their GameCube layouts (Data_c 0xCC, M_data at 0x10040FAC). */
#pragma once
#include "bindings.h"

namespace daTsubo {

/* dBgS_ObjGndChk_Yogan (0x54, the lava ground check: mGrp 4) */
struct dBgS_GndChk_l {
    u8 _[0x54];
};

/* dPa_followEcallBack (0x14), see bindings.h; remove() is the virtual at vtable +0x44 */

struct Act_c : fopAc_ac_c {
    enum Prm_e {
        PRM_ITEMNO_W = 6, PRM_ITEMNO_S = 0,
        PRM_SPEC_W = 6, PRM_SPEC_S = 8,
        PRM_MOVEBG_W = 2, PRM_MOVEBG_S = 14,
        PRM_ITEMSAVE_W = 7, PRM_ITEMSAVE_S = 16,
        PRM_TYPE_W = 4, PRM_TYPE_S = 24,
        PRM_CULL_W = 3, PRM_CULL_S = 28,
        PRM_STICK_W = 1, PRM_STICK_S = 31,
    };
    enum DataFlag_e {
        DATA_FLAG_1_e = 0x1, DATA_FLAG_2_e = 0x2, DATA_FLAG_4_e = 0x4,
        DATA_FLAG_8_e = 0x8, DATA_FLAG_10_e = 0x10, DATA_FLAG_20_e = 0x20,
    };

    struct Data_c {
        /* 0x00 */ be<f32> mGravity;
        /* 0x04 */ be<f32> m04;
        /* 0x08 */ be<f32> mAttnY;
        /* 0x0C */ be<f32> mModelScale;
        /* 0x10 */ be<u8> m10;
        /* 0x11 */ u8 _11[3];
        /* 0x14 */ be<f32> m14;
        /* 0x18 */ be<f32> m18;
        /* 0x1C */ be<f32> m1C;
        /* 0x20 */ be<f32> m20;
        /* 0x24 */ be<f32> m24;
        /* 0x28 */ be<s16> m28;
        /* 0x2A */ be<s16> m2A;
        /* 0x2C */ be<s16> m2C;
        /* 0x2E */ u8 _2E[2];
        /* 0x30 */ be<f32> m30;
        /* 0x34 */ be<f32> m34;
        /* 0x38 */ be<f32> m38;
        /* 0x3C */ be<f32> m3C;
        /* 0x40 */ be<f32> m40;
        /* 0x44 */ be<f32> m44;
        /* 0x48 */ be<f32> m48;
        /* 0x4C */ be<f32> m4C;
        /* 0x50 */ be<f32> m50;
        /* 0x54 */ be<s16> m54;
        /* 0x56 */ be<s16> m56;
        /* 0x58 */ be<f32> m58;
        /* 0x5C */ be<f32> m5C;
        /* 0x60 */ be<u8> m60;
        /* 0x61 */ be<u8> m61;
        /* 0x62 */ be<u8> m62;
        /* 0x63 */ be<u8> m63;
        /* 0x64 */ be<u8> m64;
        /* 0x65 */ be<u8> m65;
        /* 0x66 */ u8 _66[2];
        /* 0x68 */ be<u32> mFlag;
        /* 0x6C */ be<u16> m6C;
        /* 0x6E */ be<u8> m6E;
        /* 0x6F */ be<u8> mAcchCirRad;
        /* 0x70 */ be<u8> m70;
        /* 0x71 */ be<u8> mAcchRoofHeight;
        /* 0x72 */ be<u8> mAttnDist;
        /* 0x73 */ u8 _73;
        /* 0x74 */ cXyz m74;
        /* 0x80 */ be<u32> m80;
        /* 0x84 */ be<s32> mSoundID_Break;
        /* 0x88 */ be<s32> m88;
        /* 0x8C */ be<s32> mSoundID_FallLava;
        /* 0x90 */ be<s32> mSoundID_FallWater;
        /* 0x94 */ be<s32> mSoundID_Hit;
        /* 0x98 */ be<s16> mCullSphX_Move;
        /* 0x9A */ be<s16> mCullSphY_Move;
        /* 0x9C */ be<s16> mCullSphZ_Move;
        /* 0x9E */ be<s16> mCullSphR_Move;
        /* 0xA0 */ u8 mA0[4];
        /* 0xA4 */ be<s16> mCullSphX_Draw;
        /* 0xA6 */ be<s16> mCullSphY_Draw;
        /* 0xA8 */ be<s16> mCullSphZ_Draw;
        /* 0xAA */ be<s16> mCullSphR_Draw;
        /* 0xAC */ u8 mAC[4];
        /* 0xB0 */ be<f32> mB0;
        /* 0xB4 */ u8 mB4[4];
        /* 0xB8 */ be<u32> mHeapSize;
        /* 0xBC */ be<f32> mBC;
        /* 0xC0 */ be<f32> mC0;
        /* 0xC4 */ be<f32> mC4;
        /* 0xC8 */ be<f32> mC8;
    };

    struct AttrSpine_c {
        /* 0x00 */ be<f32> m00;
        /* 0x04 */ be<f32> m04;
        /* 0x08 */ be<f32> m08;
        /* 0x0C */ be<f32> m0C;
        /* 0x10 */ be<f32> m10;
        /* 0x14 */ be<f32> m14;
        /* 0x18 */ be<s16> m18;
        /* 0x1A */ be<s16> m1A;
        /* 0x1C */ be<s16> m1C;
        /* 0x1E */ be<s16> m1E;
        /* 0x20 */ be<f32> m20;
        /* 0x24 */ be<f32> m24;
        /* 0x28 */ be<f32> m28;
        /* 0x2C */ be<f32> m2C;
        /* 0x30 */ be<s16> m30;
        /* 0x32 */ u8 _32[2];
    };

    struct SpecBokoData {
        /* 0x00 */ be<s16> m00;
        /* 0x02 */ be<s16> m02;
        /* 0x04 */ be<f32> m04;
        /* 0x08 */ be<f32> m08;
    };

    struct M7A0 {
        /* 0x00 */ be<u32> m00;  /* fpc_ProcID */
        /* 0x04 */ be<u8> m04;
        /* 0x05 */ u8 _05;
        /* 0x06 */ be<s16> m06;
        /* 0x08 */ be<s16> m08;
        /* 0x0A */ u8 _0A[2];
        /* 0x0C */ be<f32> m0C;
        /* 0x10 */ be<f32> m10;
        /* 0x14 */ be<f32> m14;
        /* 0x18 */ be<s8> m18;
        /* 0x19 */ u8 _19[3];
    };

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<mDoExt_brkAnm> mpBrk;
    /* 0x3BC */ dBgS_ObjAcch mAcch;
    /* 0x580 */ dBgS_AcchCir mAcchCir;
    /* 0x5C0 */ dBgS_GndChk_l mGndChkYogan;
    /* 0x614 */ be<f32> m4F8;
    /* 0x618 */ be<f32> m4FC;
    /* 0x61C */ be<f32> m500;
    /* 0x620 */ be<u8> m504;
    /* 0x621 */ be<u8> m505;
    /* 0x622 */ u8 _622[2];
    /* 0x624 */ dCcD_Cyl mCyl;
    /* 0x754 */ dCcD_Stts mStts;
    /* 0x790 */ be<s32> mType;
    /* 0x794 */ be<s32> m678;
    /* 0x798 */ be<u16> mPrmZ;
    /* 0x79A */ be<u8> m67E;
    /* 0x79B */ be<u8> m67F;
    /* 0x79C */ be<u8> m680;
    /* 0x79D */ be<s8> m681;
    /* 0x79E */ be<u8> m682;
    /* 0x79F */ be<u8> m683;
    /* 0x7A0 */ be<u8> m684;
    /* 0x7A1 */ be<u8> m685;
    /* 0x7A2 */ be<u8> m686;
    /* 0x7A3 */ u8 _7A3;
    /* 0x7A4 */ be<s16> m688;          /* cSAngle */
    /* 0x7A6 */ be<s16> m68A;
    /* 0x7A8 */ be<s16> m68C;
    /* 0x7AA */ be<s16> m68E;
    /* 0x7AC */ be<s16> m690;
    /* 0x7AE */ be<s16> m692;
    /* 0x7B0 */ be<f32> m694[4];       /* Quaternion x, y, z, w */
    /* 0x7C0 */ u8 m6A4[0xC];
    /* 0x7CC */ cXyz m6B0;
    /* 0x7D8 */ Mtx34 mPoseMtx;
    /* 0x808 */ be<f32> m6EC;
    /* 0x80C */ cXyz m6F0;
    /* 0x818 */ be<f32> m6FC;
    /* 0x81C */ cXyz m700;
    /* 0x828 */ dPa_followEcallBack m70C[3];
    /* 0x864 */ dPa_followEcallBack m748[3];
    /* 0x8A0 */ dPa_followEcallBack m784;
    /* 0x8B4 */ be<u8> m798;
    /* 0x8B5 */ u8 m799[7];
    /* 0x8BC */ M7A0 m7A0[3];
    /* 0x910 */ cXyz m7F4;
    /* 0x91C */ be<s16> m800;
    /* 0x91E */ be<s16> m802;
    /* 0x920 */ be<s16> m804;
    /* 0x922 */ be<s16> m806;
    /* 0x924 */ be<f32> m808;
    /* 0x928 */ be<f32> m80C;
    /* 0x92C */ be<u8> m810;
    /* 0x92D */ be<u8> m811;
    /* 0x92E */ u8 _92E[2];
    /* 0x930 */ cXyz m814;
};
WWHD_OFFSET(Act_c, mAcch, 0x3BC);
WWHD_OFFSET(Act_c, mAcchCir, 0x580);
WWHD_OFFSET(Act_c, mCyl, 0x624);
WWHD_OFFSET(Act_c, mStts, 0x754);
WWHD_OFFSET(Act_c, mType, 0x790);
WWHD_OFFSET(Act_c, m688, 0x7A4);
WWHD_OFFSET(Act_c, mPoseMtx, 0x7D8);
WWHD_OFFSET(Act_c, m70C, 0x828);
WWHD_OFFSET(Act_c, m7A0, 0x8BC);
WWHD_OFFSET(Act_c, m814, 0x930);
WWHD_SIZE(Act_c::Data_c, 0xCC);
WWHD_SIZE(Act_c::AttrSpine_c, 0x34);
WWHD_SIZE(Act_c::M7A0, 0x1C);
WWHD_SIZE(Act_c, 0x93C);

/* ---- this TU's addresses ---- */
#define TSUBO_SAFESTRING_VTBL 0x10040AD4 /* sead::SafeString vtable (per TU) */
#define TSUBO_ACT_VTBL 0x10040BFC        /* Act_c vtable (HD virtual destructor) */
#define TSUBO_CYL_AAB_VTBL 0x10040AEC    /* cM3dGAab vtable (per TU) */
static const dBgS_ObjAcch_vt TSUBO_OBJACCH_VT = {0x10040BCC, 0x10040BEC, 0x10040BDC};
/* dBgS_ObjGndChk_Yogan vtables at +0x10/+0x20/+0x40/+0x4C */
static const dBgS_GndChk_vt TSUBO_YOGAN_VT = {0x10040B8C, 0x10040B9C, 0x10040BBC, 0x10040BAC};

/* const tables (.rodata): GHS folds the constant members of M_attrSpine / M_data_spec_boko into
 * immediates where it can; where the code reads them, these accessors give the addresses */
inline Act_c::Data_c* M_data(s32 type) { return gabi::at<Act_c::Data_c>(0x10040FAC + 0xCC * type); }
inline const char* M_arcname(s32 type) { return gabi::at<const char>(gabi::load<u32>(0x10040F00 + 4 * type)); }
inline Act_c::SpecBokoData* M_data_spec_boko(s32 i) { return gabi::at<Act_c::SpecBokoData>(0x10040F40 + 0xC * i); }
#define M_cyl_src gabi::at<dCcD_SrcCyl>(0x10040F68)
/* static fopAc_ac_c* Act_c::M_spec_act[3] */
inline gptr<fopAc_ac_c>* M_spec_act() { return gabi::at<gptr<fopAc_ac_c>>(0x1046EA94); }
/* the data of this object's type: mType is reloaded at each use */
inline Act_c::Data_c* data(Act_c* a) { return M_data(a->mType); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02048038 daObj::PrmAbstract(actor, width, shift): (mParameters >> shift) & ((1 << width) - 1)
 * (one shared out-of-line copy in HD) */
inline u32 daObj_PrmAbstract(const fopAc_ac_c* a, s32 w, s32 s) { return gabi::call<u32>(0x02048038, a, w, s); }
inline s32 prm_get_type(Act_c* a) { return daObj_PrmAbstract(a, Act_c::PRM_TYPE_W, Act_c::PRM_TYPE_S); }
inline s32 prm_get_spec(Act_c* a) { return daObj_PrmAbstract(a, Act_c::PRM_SPEC_W, Act_c::PRM_SPEC_S); }
inline s32 prm_get_cull(Act_c* a) { return daObj_PrmAbstract(a, Act_c::PRM_CULL_W, Act_c::PRM_CULL_S); }
inline s32 prm_get_itemNo(Act_c* a) { return daObj_PrmAbstract(a, Act_c::PRM_ITEMNO_W, Act_c::PRM_ITEMNO_S); }
inline s32 prm_get_itemSave(Act_c* a) { return daObj_PrmAbstract(a, Act_c::PRM_ITEMSAVE_W, Act_c::PRM_ITEMSAVE_S); }
inline bool prm_get_moveBg(Act_c* a) { return daObj_PrmAbstract(a, Act_c::PRM_MOVEBG_W, Act_c::PRM_MOVEBG_S) == 1u; }
inline u32 prm_get_stick(Act_c* a) { return daObj_PrmAbstract(a, Act_c::PRM_STICK_W, Act_c::PRM_STICK_S); }
inline void prm_set_cull_non(Act_c* a) { a->mParameters = a->mParameters & ~0x70000000u; }
inline void prm_off_moveBg(Act_c* a) { a->mParameters = a->mParameters | 0xC000u; }
inline void prm_off_stick(Act_c* a) { a->mParameters = a->mParameters & ~0x80000000u; }
inline bool is_switch(Act_c* a) { return fopAcM_isSwitch(a, a->mPrmZ & 0xFF); }
inline bool spec_is_boko(s32 spec) { return spec == 1 || spec == 2 || spec == 3; }

/* cSAngle (c_angle, out of line in HD): 020065FC cSAngle() (allocates when this == NULL),
 * 02006638 Val(const cSAngle&), 02006584 Val(s16), 020068CC operator+=(const cSAngle&),
 * 0200691C operator+=(s16), 0200692C operator-=(s16), 020069A0 operator*=(f32) */
inline void cSAngle_ct(be<s16>* a) { gabi::call(0x020065FC, a); }
inline void cSAngle_Val(be<s16>* a, const be<s16>* b) { gabi::call(0x02006638, a, b); }
inline void cSAngle_Val(be<s16>* a, s16 v) { gabi::call(0x02006584, a, v); }
inline void cSAngle_addeq(be<s16>* a, const be<s16>* b) { gabi::call(0x020068CC, a, b); }
inline void cSAngle_addeq(be<s16>* a, s16 v) { gabi::call(0x0200691C, a, v); }
inline void cSAngle_subeq(be<s16>* a, s16 v) { gabi::call(0x0200692C, a, v); }
inline void cSAngle_muleq(be<s16>* a, f32 f) { gabi::call(0x020069A0, a, f); }
#define cSAngle__0 gabi::at<be<s16>>(0x101FF354) /* cSAngle::_0 */

/* dBgS_Acch inline flag setters missing from d_bg_s.h */
inline void Acch_ClrRoofNone(dBgS_Acch* a) { a->m_flags &= ~(u32)dBgS_Acch::ROOF_NONE; }
inline void Acch_ClrWallNone(dBgS_Acch* a) { a->m_flags &= ~(u32)dBgS_Acch::WALL_NONE; }
inline void Acch_ClrWaterNone(dBgS_Acch* a) { a->m_flags &= ~(u32)dBgS_Acch::WATER_NONE; }
inline void Acch_ClrGroundLanding(dBgS_Acch* a) { a->m_flags &= ~(u32)dBgS_Acch::GROUND_LANDING; }
inline u32 Acch_MaskWaterIn(dBgS_Acch* a) { return a->m_flags & dBgS_Acch::WATER_IN; }
inline cBgS_PolyInfo* Acch_gnd(dBgS_Acch* a) { return gabi::at<cBgS_PolyInfo>(gabi::ea(a) + 0xD4 + 0x14); }

/* attention_info.flags (+0x39C), distances (+0x388..), position (+0x390) */
inline void attn_onBit(fopAc_ac_c* a, u32 b) { u32 p = gabi::ea(a) + 0x39C; gabi::store<u32>(p, gabi::load<u32>(p) | b); }
inline void attn_offBit(fopAc_ac_c* a, u32 b) { u32 p = gabi::ea(a) + 0x39C; gabi::store<u32>(p, gabi::load<u32>(p) & ~b); }
enum { fopAc_Attn_ACTION_CARRY_e = 0x10 };

/* HD fopAcM_seStart inline where the actor is known non-null (only &eyePos is checked) */
inline void fopAcM_seStart_nn(fopAc_ac_c* a, u32 id, u32 param) {
    if (gabi::ea(&a->eyePos) != 0)
        mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}

/* HD: strcmp(s, lit) == 0 as a sead::SafeString comparison (see d_a_ki_execute.cpp): `lit` is the
 * literal, `str` the other string (computed after the literal's SafeString is built) */
template <class F> inline bool sead_streq(u32 lit, F str_fn) {
    gabi::Local<SafeString> a;
    a->mStringTop = lit;
    a->__vtbl = TSUBO_SAFESTRING_VTBL;
    gabi::Local<SafeString> b;
    {
        u32 s = str_fn();
        b->__vtbl = TSUBO_SAFESTRING_VTBL;
        b->mStringTop = s;
    }
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    u32 bv = b->__vtbl;
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(bv + 0x14), b.get());
    u32 s2 = b->mStringTop;
    if (s1 == s2) return true;
    for (u32 n = 0x40001; n != 0; n--) {
        u8 c1 = gabi::load<u8>(s1);
        u8 c2 = gabi::load<u8>(s2);
        if (c1 != c2) return false;
        if (c1 == 0) return true;
        s1++;
        s2++;
    }
    return false;
}

} // namespace daTsubo
