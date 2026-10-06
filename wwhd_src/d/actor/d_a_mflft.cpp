/**
 * d_a_mflft.cpp (WWHD)
 * Object - Dragon Roost Cavern - Flame lift (platform lifted up by lava plume)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_mflft.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x100149EC
#define MFLFT_VTBL 0x10014BE8 /* HD: mflft_class vtable (dtor +0xC, setLiftUp +0x14) */
#define AAB_VTBL 0x10014A04   /* this TU's cM3dGAab vtable */

enum {
    dRes_INDEX_MFLFT_BDL_MFLFT_e = 4,
    dRes_INDEX_MFLFT_DZB_MFLFT_e = 7,
    dRes_INDEX_ALWAYS_BTI_ROPE_e = 0x7E,
};

/* statics: wind_vec, wy, wp */
#define wind_vec_g (*gabi::at<be<u32>>(0x10464FBC))
#define wy_g (*gabi::at<be<s16>>(0x10464FCC))
#define wp_g (*gabi::at<be<u32>>(0x10464FC0))

/* mDoExt_3DlineMat1_c, HD 0x188: vtable at +0x130, the line array at +0x184 (0x10 per line:
 * +0 positions, +4 sizes) */
struct mDoExt_3DlineMat1_l {
    /* 0x000 */ u8 _000[0x130];
    /* 0x130 */ be<u32> __vtbl;
    /* 0x134 */ u8 _134[0x184 - 0x134];
    /* 0x184 */ be<u32> mpLines;
    cXyz* getPos(s32 i) { return gabi::at<cXyz>(gabi::load<u32>(mpLines + 0x10 * i)); }
    u8* getSize(s32 i) { return gabi::at<u8>(gabi::load<u32>(mpLines + 0x10 * i + 4)); }
};
WWHD_SIZE(mDoExt_3DlineMat1_l, 0x188);

/* GameCube +0x11C up to mLineMat (HD 0x188 instead of 0x3C); size 0x9E4 */
struct mflft_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ be<s16> m298;
    /* 0x3B6 */ be<s16> m29A;
    /* 0x3B8 */ gptr<J3DModel> mpModel;
    /* 0x3BC */ be<u8> m2A0;
    /* 0x3BD */ be<u8> m2A1;
    /* 0x3BE */ u8 _3BE[2];
    /* 0x3C0 */ cXyz m2A4;
    /* 0x3CC */ be<f32> m2B0;
    /* 0x3D0 */ u8 _3D0[4];
    /* 0x3D4 */ be<f32> m2B8;
    /* 0x3D8 */ be<f32> m2BC;
    /* 0x3DC */ u8 _3DC[4];
    /* 0x3E0 */ be<f32> m2C4;
    /* 0x3E4 */ csXyz m2C8;
    /* 0x3EA */ be<s16> m2CE;
    /* 0x3EC */ be<f32> m2D0;
    /* 0x3F0 */ be<s8> m2D4[3];
    /* 0x3F3 */ u8 _3F3;
    /* 0x3F4 */ cXyz m2D8[3];
    /* 0x418 */ cXyz m2FC[3];
    /* 0x43C */ dCcD_Stts mStts;
    /* 0x478 */ dCcD_Cyl mCyls[3];
    /* 0x808 */ be<s16> m6EC[3];
    /* 0x80E */ be<s8> m6F2[3];
    /* 0x811 */ be<s8> m6F5;
    /* 0x812 */ be<s16> m6F6;
    /* 0x814 */ be<s16> m6F8;
    /* 0x816 */ be<s16> m6FA;
    /* 0x818 */ u8 _818[4];
    /* 0x81C */ Mtx34 m700;
    /* 0x84C */ gptr<dBgW> pm_bgw;
    /* 0x850 */ be<f32> m734;
    /* 0x854 */ mDoExt_3DlineMat1_l mLineMat;
    /* 0x9DC */ gptr<JPABaseEmitter> m774;
    /* 0x9E0 */ be<s8> m778;
    /* 0x9E1 */ u8 _9E1[3];

    void setLiftUp(cXyz* arg1);
};
WWHD_OFFSET(mflft_class, mStts, 0x43C);
WWHD_OFFSET(mflft_class, m6EC, 0x808);
WWHD_OFFSET(mflft_class, m700, 0x81C);
WWHD_OFFSET(mflft_class, mLineMat, 0x854);
WWHD_OFFSET(mflft_class, m778, 0x9E0);
WWHD_SIZE(mflft_class, 0x9E4);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void __construct_array_l(void* p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void __destroy_arr_l(void* p, u32 n, u32 size, u32 dtor, u32 flags) { gabi::call(0x028F0164, p, n, size, dtor, flags); }
static inline cXyz* dKyw_get_wind_vec() { return gabi::call<cXyz*>(0x0257DAA8); }
static inline be<f32>* dKyw_get_wind_power() { return gabi::call<be<f32>*>(0x0257DB04); }
/* 0201A4DC csXyz::operator+(const csXyz&): the result comes back in r3:r4 */
static inline void csXyz_pl(const csXyz* a, const csXyz* b, csXyz* out) {
    gabi::call(0x0201A4DC, a, b);
    u32 hi = gabi::cpu->r[3], lo = gabi::cpu->r[4];
    out->x = (s16)(hi >> 16);
    out->y = (s16)hi;
    out->z = (s16)(lo >> 16);
}
static inline dSv_event_c* dComIfGs_getEvent() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline void fopAcM_SetMin(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D672C, a, x, y, z); }
static inline void fopAcM_SetMax(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D673C, a, x, y, z); }
/* 02518DB0 at_power_check(CcAtInfo*) */
struct CcAtInfo_l {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ u8 _09[0x1C - 9];
};
static inline void at_power_check(CcAtInfo_l* i) { gabi::call(0x02518DB0, i); }
/* JPABaseEmitter (HD): becomeInvalidEmitter = stopCreateParticle (+0x254 |= 1) + mMaxFrame = -1 (+0x5C) */
static inline void JPABaseEmitter_becomeInvalidEmitter(JPABaseEmitter* e) {
    u32 b = gabi::ea(e);
    gabi::store<s32>(b + 0x5C, -1);
    gabi::store<u32>(b + 0x254, gabi::load<u32>(b + 0x254) | 1);
}
/* mDoExt_3DlineMat1_c */
static inline void lineMat_update(mDoExt_3DlineMat1_l* m, s32 n, const GXColor* c, dKy_tevstr_c* t) { gabi::call(0x025ED1BC, m, n, c, t); }
static inline BOOL lineMat_init(mDoExt_3DlineMat1_l* m, s32 a, s32 b, void* img, s32 c) { return gabi::call<BOOL>(0x025EBA58, m, a, b, img, c); }
/* dComIfGd_set3DlineMat (HD): the play's sort packets (play+0x5FB4, 0x9C each) by the material id (virtual +0x14) */
static inline void dComIfGd_set3DlineMat(mDoExt_3DlineMat1_l* m) {
    u32 pkt = dComIfGp_ea() + 0x5FB4;
    s32 id = gabi::call_ptr<s32>(gabi::load<u32>(m->__vtbl + 0x14), m);
    gabi::call(0x025EDD04, pkt + id * 0x9C, m);
}
/* dBgS_GndChk / dBgS_ObjGndChk_Yogan on the stack (this TU's vtables) */
static const dBgS_GndChk_vt GNDCHK_VT = {0x10014A24, 0x10014A34, 0x10014A54, 0x10014A44};
static const dBgS_GndChk_vt YOGAN_VT = {0x10014AA4, 0x10014AB4, 0x10014AD4, 0x10014AC4};
static inline void yogan_ct(void* c) {
    dBgS_GndChk_ct(c, YOGAN_VT, true);
    gabi::store<u32>(gabi::ea(c) + 0x50, 4); /* lava */
}
static inline void dBgS_GndChk_dt(void* c) {
    u32 b = gabi::ea(c);
    gabi::store<u32>(b + 0x20, 0x10014A34);
    gabi::store<u32>(b + 0x40, 0x10014A54);
    gabi::store<u32>(b + 0x4C, 0x10014A14);
    gabi::call(0x02008DAC, c, 0); /* cBgS_GndChk::~cBgS_GndChk */
}
/* lfs -> stfs with no arithmetic in between is bit-exact in the recompiled original (a signalling
 * NaN is not quieted): such copies are written as word copies */
static inline void fcopy(be<f32>* dst, const be<f32>* src) { gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(gabi::ea(src))); }
static inline void J3DModel_setBaseScale_bits(J3DModel* m, const cXyz* s) {
    for (u32 i = 0; i < 12; i += 4) gabi::store<u32>(gabi::ea(m) + 0xBC + i, gabi::load<u32>(gabi::ea(s) + i));
}
static inline f32 fabsf_(f32 v) { return v < 0.0f ? -v : (v == 0.0f ? 0.0f * v + 0.0f : v); }

/* 021C3ECC (virtual; the cXyz is passed by value: a pointer to a copy) */
void mflft_class::setLiftUp(cXyz* arg1) {
    WWHD_FUNC(0x021C3ECC, void, this, arg1);
    if ((m29A != 0) || (fabsf_(arg1->y - current.pos.y) < 200.0f)) {
        fcopy(&current.pos.x, &arg1->x);
        fcopy(&current.pos.z, &arg1->z);
        cLib_addCalc2(&current.pos.y, arg1->y, 0.2f, 1000.0f * m2D0);
        cLib_addCalc2(&m2D0, 1.0f, 1.0f, 0.05f);
        speed.y = 0.0f;
        if (m2CE == 0) {
            gabi::Local<cXyz> v;
            dVibration_c* vib = dComIfGp_getVibration();
            v->set(0.0f, 1.0f, 0.0f);
            gabi::call(0x025CB374, vib, REG0_S(2) + 8, -0x21, v.get()); /* StartShock */
        }
        m2CE = 5;
    }
}
VERIFY(0x021C3ECC, &mflft_class::setLiftUp);

/* 021C2064 */
static void ride_call_back(dBgW*, fopAc_ac_c* arg1, fopAc_ac_c* arg2) {
    WWHD_FUNC(0x021C2064, void, (u32)0, arg1, arg2);
    mflft_class* i_this = (mflft_class*)arg1;

    cMtx_YrotS(calc_mtx(), -i_this->current.angle.y);
    gabi::Local<cXyz> tmp;
    gabi::Local<cXyz> sp38;
    gabi::Local<cXyz> sp2C;
    gabi::Local<cXyz> sp20;
    cXyz_mi(&arg2->current.pos, tmp.get(), &i_this->current.pos);
    sp38->copy(*tmp);
    MtxPosition(sp38.get(), sp2C.get());
    cXyz_mi(&arg2->old.pos, tmp.get(), &i_this->current.pos);
    sp38->copy(*tmp);
    MtxPosition(sp38.get(), sp20.get());
    f32 k = REG0_F(0) + 10.0f;
    s16 iVar2 = (s16)gabi::ftoi(sp2C->z * (k / i_this->scale.z));
    s16 iVar1 = (s16)gabi::ftoi(-(sp2C->x * (k / i_this->scale.x)));

    cLib_addCalcAngleS2(&i_this->current.angle.x, iVar2, 10, 0x800);
    cLib_addCalcAngleS2(&i_this->current.angle.z, iVar1, 10, 0x800);

    f32 fVar3 = fabsf_(sp2C->z - sp20->z) * (REG0_F(4) + 50.0f);
    if (i_this->m2BC < fVar3) {
        i_this->m2BC = fVar3;
    }
    fVar3 = fabsf_(sp2C->x - sp20->x) * (REG0_F(4) + 50.0f);
    if (i_this->m2C4 < fVar3) {
        i_this->m2C4 = fVar3;
    }
    fVar3 = fabsf_(sp2C->x - sp20->x) * (REG0_F(8) + 5.0f);
    if ((fVar3 > 10.0f) && (i_this->m2B0 < fVar3)) {
        cLib_addCalc2(&i_this->m2B0, fVar3, 1.0f, REG0_F(7) + 1.2f);
    }
    fVar3 = fabsf_(sp2C->z - sp20->z) * (REG0_F(8) + 5.0f);
    if ((fVar3 > 10.0f) && (i_this->m2B8 < fVar3)) {
        cLib_addCalc2(&i_this->m2B8, fVar3, 1.0f, REG0_F(7) + 1.2f);
    }
}
VERIFY(0x021C2064, ride_call_back);

/* 021C2304 (himo_Draw inlined) */
static BOOL daMflft_Draw(mflft_class* i_this) {
    WWHD_FUNC(0x021C2304, BOOL, i_this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, i_this->mpModel, &i_this->tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(i_this->mpModel);
    dComIfGd_setList();
    /* himo_Draw: color {150, 150, 150, 255} (.data 0x101BA45C) */
    lineMat_update(&i_this->mLineMat, 10, gabi::at<GXColor>(0x101BA45C), &i_this->tevStr);
    dComIfGd_set3DlineMat(&i_this->mLineMat);
    return TRUE;
}
VERIFY(0x021C2304, daMflft_Draw);

/* 021C23E0 */
static void himo_cut_control(mflft_class* i_this, cXyz* arg1, u8* arg2, u8 arg3) {
    WWHD_FUNC(0x021C23E0, void, i_this, arg1, arg2, arg3);
    gabi::Local<u8[0x54]> gndChk;
    dBgS_GndChk_ct(gndChk.get(), GNDCHK_VT, false);
    gabi::Local<cXyz> sp30;
    gabi::Local<cXyz> sp18;
    gabi::Local<cXyz> sp0C;
    gabi::Local<cXyz> sp24;

    sp30->x = 0.0f;
    sp30->y = 0.0f;
    sp30->z = gabi::load<f32>(wp_g) * 7.0f;
    cMtx_YrotS(calc_mtx(), wy_g);
    MtxPosition(sp30.get(), sp18);

    if (arg3 != 0) {
        sp0C->x = 0.0f;
        sp0C->y = 0.0f;
        sp0C->z = 0.0f;
        sp30->z = REG0_F(2) + 135.0f;
    } else {
        cMtx_YrotS(calc_mtx(), i_this->m6F8 + i_this->shape_angle.y);
        sp30->z = REG0_F(11) + -4.0f;
        MtxPosition(sp30.get(), sp0C.get());
        sp30->z = REG0_F(2) + 10.0f;
    }

    arg1++;
    for (s32 i = 1; i < 10; i++, arg1++, arg2++) {
        f32 fVar2 = (arg1->x - arg1[-1].x) + sp18->x + REG0_F(3) + sp0C->x;
        f32 fVar1 = arg1->y - 10.0f;
        if (arg3 == 0) {
            f32 y = arg1->y + 50.0f;
            cXyz* gp = gabi::at<cXyz>(gabi::ea(gndChk.get()) + 0x24);
            fcopy(&gp->x, &arg1->x);
            fcopy(&sp24->x, &arg1->x);
            fcopy(&gp->z, &arg1->z);
            fcopy(&sp24->z, &arg1->z);
            sp24->y = y;
            gp->y = y;
            f32 fVar9 = cBgS_GroundCross(dComIfG_Bgsp(), gndChk.get()) + 10.0f;
            if (!(fVar1 > fVar9)) {
                fVar1 = fVar9;
            }
        }
        f32 fz = (arg1->z - arg1[-1].z) + sp18->z + sp0C->z;
        f32 fVar8 = fVar1 - arg1[-1].y;

        s16 iVar4 = cM_atan2s(fVar2, fz);
        s16 iVar5 = -cM_atan2s(fVar8, std_sqrtf(gabi::fmadds(fVar2, fVar2, fz * fz)));

        cMtx_YrotS(calc_mtx(), iVar4);
        cMtx_XrotM(calc_mtx(), iVar5);
        MtxPosition(sp30.get(), sp24.get());

        arg1->y = arg1[-1].y + sp24->y;
        arg1->x = arg1[-1].x + sp24->x;
        arg1->z = arg1[-1].z + sp24->z;
        *gabi::at<be<u8>>(gabi::ea(arg2)) = 3;
    }
    dBgS_GndChk_dt(gndChk.get());
}
VERIFY(0x021C23E0, himo_cut_control);

/* mflft_move (inlined into daMflft_Execute) */
static inline void mflft_move(mflft_class* i_this) {
    i_this->m298 = i_this->m298 + 1;
    switch ((u16)i_this->m29A) {
    case 0: {
        s32 uVar6 = 0;
        for (s32 i = 0; i < 3; i++) {
            if (i_this->m2D4[i] != 0) {
                uVar6 |= gabi::load<u8>(0x101BA460 + i); /* himo_off_check */
            }
        }
        s16 target;
        s16 sVar2 = gabi::load<s16>(0x101BA4B4 + 2 * uVar6); /* himo_off_ya */
        if (sVar2 != 1) {
            target = -0x3000;
        } else {
            i_this->m6FA = 0;
            target = 0;
        }

        if (uVar6 == 7) {
            i_this->m29A = 1;
            dSv_event_onEventBit(dComIfGs_getEvent(), 0x2A10);
            gabi::Local<u8[0x54]> gndChk;
            yogan_ct(gndChk.get());
            cXyz* gp = gabi::at<cXyz>(gabi::ea(gndChk.get()) + 0x24);
            fcopy(&gp->x, &i_this->current.pos.x);
            gp->y = i_this->current.pos.y + 1000.0f;
            fcopy(&gp->z, &i_this->current.pos.z);
            f32 fVar10 = cBgS_GroundCross(dComIfG_Bgsp(), gndChk.get());
            if (fVar10 != -1000000000.0f) {
                i_this->m734 = fVar10;
            }
            if (i_this->m2A1 != 0xff) {
                dComIfGs_onSwitch(i_this->m2A1, fopAcM_GetRoomNo(i_this));
            }
            dBgS_GndChk_dt(gndChk.get());
        } else {
            if (i_this->m6F5 != 0) {
                s8 v = (s8)(i_this->m6F5 - 1);
                i_this->m6F5 = v;
                if ((v == 0) && (uVar6 != 0)) {
                    u8 eventReg = dSv_event_getEventReg(dComIfGs_getEvent(), 0xA507);
                    if (eventReg < 6) {
                        eventReg++;
                        dSv_event_setEventReg(dComIfGs_getEvent(), 0xA507, eventReg);
                    }
                }
            }

            cLib_addCalcAngleS2(&i_this->m6FA, target, 4, 0x2000);
            cLib_addCalcAngleS2(&i_this->m6F8, sVar2, 2, 0x2000);
            cLib_addCalcAngleS2(&i_this->current.angle.x, 0, 10, 0x200);
            cLib_addCalcAngleS2(&i_this->current.angle.z, 0, 10, 0x200);

            f32 fVar10, fVar1;
            if (i_this->m2CE != 0) {
                fVar10 = 2000.0f;
                fVar1 = 250.0f;
                if (i_this->m778 == 0) {
                    i_this->m778 = 1;
                }
            } else {
                fVar10 = 100.0f;
                fVar1 = 20.0f;
                if (i_this->m778 == 2) {
                    i_this->m778 = 3;
                }
            }

            i_this->m2C8.x = (s16)gabi::ftoi(i_this->m2BC * cM_ssin(i_this->m298 * 0x5dc));
            i_this->m2C8.z = (s16)gabi::ftoi(i_this->m2C4 * cM_ssin(i_this->m298 * 0x514));
            cLib_addCalc2(&i_this->m2BC, fVar10, 1.0f, fVar1 + REG0_F(3));
            cLib_addCalc2(&i_this->m2C4, fVar10, 1.0f, fVar1 + REG0_F(3));
            i_this->m2A4.x = i_this->m2B0 * cM_ssin(i_this->m298 * 0x2ee);
            i_this->m2A4.z = i_this->m2B8 * cM_ssin(i_this->m298 * 900);
            cLib_addCalc0(&i_this->m2B0, 1.0f, REG0_F(6) + 0.25f);
            cLib_addCalc0(&i_this->m2B8, 1.0f, REG0_F(6) + 0.25f);
            csXyz_pl(&i_this->current.angle, &i_this->m2C8, &i_this->shape_angle);
            gabi::Local<cXyz> tmp;
            cXyz_pl(&i_this->home.pos, tmp.get(), &i_this->m2A4);
            i_this->current.pos.copy(*tmp);
        }
    } break;

    case 1: {
        cLib_addCalc2(&i_this->current.pos.x, i_this->home.pos.x, 0.1f, 5.0f);
        cLib_addCalc2(&i_this->current.pos.z, i_this->home.pos.z, 0.1f, 5.0f);
        cLib_addCalcAngleS2(&i_this->current.angle.x, 0, 10, 0x300);
        cLib_addCalcAngleS2(&i_this->current.angle.z, 0, 10, 0x300);
        cLib_addCalcAngleS2(&i_this->m6FA, 0, 4, 0x200);

        f32 fVar10, fVar1;
        if (i_this->m2CE != 0) {
            fVar10 = 2500.0f;
            fVar1 = 250.0f;
            if (i_this->m778 == 0) {
                i_this->m778 = 1;
            }
        } else {
            fVar10 = 400.0f;
            fVar1 = 20.0f;
            if (i_this->m778 == 2) {
                i_this->m778 = 3;
            }
        }
        i_this->m2C8.x = (s16)gabi::ftoi(i_this->m2BC * cM_ssin(i_this->m298 * 800));
        i_this->m2C8.z = (s16)gabi::ftoi(i_this->m2C4 * cM_ssin(i_this->m298 * 700));
        cLib_addCalc2(&i_this->m2BC, fVar10, 1.0f, fVar1);
        cLib_addCalc2(&i_this->m2C4, fVar10, 1.0f, fVar1);

        csXyz_pl(&i_this->current.angle, &i_this->m2C8, &i_this->shape_angle);
        i_this->current.pos.y = i_this->current.pos.y + i_this->speed.y;
        f32 sy = i_this->speed.y - 5.0f;
        if (sy < -300.0f) {
            sy = -300.0f;
        }
        i_this->speed.y = sy;

        f32 lim = i_this->m734 + 80.0f;
        if (!(i_this->current.pos.y > lim)) {
            i_this->current.pos.y = lim;
            if (i_this->speed.y < -50.0f) {
                i_this->m2C4 = 2000.0f;
                i_this->m2BC = 2000.0f;
                dComIfGp_particle_set(0x8231 /* ID_AK_SN_MAGMALIFTSPLASH00 */, &i_this->current.pos, &i_this->shape_angle);
                /* fopAcM_seStartCurrent (HD: null check on the position) */
                if (gabi::ea(&i_this->current.pos) != 0) {
                    mDoAud_seStart(0x69EB /* JA_SE_OBJ_BAL_LIFT_LANDING */, &i_this->current.pos, 0,
                                   dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
                }
                gabi::Local<cXyz> v;
                dVibration_c* vib = dComIfGp_getVibration();
                v->set(0.0f, 1.0f, 0.0f);
                gabi::call(0x025CB374, vib, REG0_S(2) + 5, -0x21, v.get()); /* StartShock */
            }
            i_this->speed.y = 0.0f;
            cLib_addCalcAngleS2(&i_this->m6FA, 0, 2, 0x2000);
        }
        break;
    }
    }
}

/* eff_cont (inlined) */
static inline void eff_cont(mflft_class* i_this) {
    gabi::Local<cXyz> sp24;
    fcopy(&sp24->x, &i_this->current.pos.x);
    f32 y = i_this->current.pos.y;
    sp24->y = y;
    fcopy(&sp24->z, &i_this->current.pos.z);
    sp24->y = y - (REG0_F(1) + 90.0f);

    switch ((u8)i_this->m778) {
    case 0:
        break;
    case 1: {
        JPABaseEmitter* e = dComIfGp_particle_set(0x8105 /* ID_AK_SN_MAGMAISLAND01 */, sp24.get());
        i_this->m774 = e;
        if (e != nullptr) {
            /* setGlobalScale(1.8): global and particle scale */
            u32 b = gabi::ea(e);
            for (u32 o : {0x220u, 0x224u, 0x228u, 0x238u, 0x23Cu, 0x240u}) gabi::store<f32>(b + o, 1.8f);
            i_this->m778 = 2;
        } else {
            i_this->m778 = 0;
        }
        break;
    }
    case 2:
        if (i_this->m774 != nullptr) {
            MtxTrans(0.0f, -90.0f, 0.0f, 1);
            /* setGlobalRTMatrix */
            Mtx34* m = calc_mtx();
            u32 b = gabi::ea(i_this->m774.get());
            gabi::call(0x028249B0, m, b + 0x1F0, b + 0x22C); /* JPASetRMtxTVecfromMtx */
        }
    case 3:
        if (i_this->m778 == 3) {
            if (i_this->m774 != nullptr) {
                JPABaseEmitter_becomeInvalidEmitter(i_this->m774);
                i_this->m774 = nullptr;
            }
            i_this->m778 = 0;
        }
        break;
    }
}

/* himo_move (inlined) */
static inline void himo_move(mflft_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    /* sp18 (csXyz) and sp48 (cXyz) are adjacent in the original frame (0x38 / 0x40): the
     * particle call compares more than the 6 bytes of the angle */
    struct Frame { csXyz sp18; u8 pad[2]; cXyz sp48; };
    gabi::Local<Frame> fr;
    cXyz* sp48 = &fr->sp48;
    csXyz* sp18 = &fr->sp18;
    gabi::Local<cXyz> sp3C;
    gabi::Local<cXyz> t1;
    gabi::Local<cXyz> t2;
    gabi::Local<cXyz> tmi;
    gabi::Local<CcAtInfo_l> ccAtInfo;

    for (s32 i = 0; i < 3; i++) {
        cMtx_YrotS(calc_mtx(), i_this->m298);
        sp48->x = 0.0f;
        sp48->y = 0.0f;
        sp48->z = (f32)(s16)i_this->m6EC[i] * cM_ssin(i_this->m298 * 15000);
        MtxPosition(sp48, sp3C.get());

        cXyz* line1 = i_this->mLineMat.getPos(i);
        cXyz* line2 = i_this->mLineMat.getPos(i + 3);

        if (i_this->m2D4[i] == 0) {
            cXyz_mi(&i_this->m2FC[i], tmi.get(), &i_this->m2D8[i]);
            sp48->copy(*tmi);
            u8* lineSize = i_this->mLineMat.getSize(i);

            for (s32 j = 0; j < 10; line1++, line2++, lineSize++, j++) {
                if (j < 9) {
                    f32 tmp = (f32)j / 27.0f;
                    cXyz_ml(sp48, t1.get(), tmp);
                    cXyz_pl(&i_this->m2D8[i], t2.get(), t1.get());
                    f32 x = t2->x, z = t2->z;
                    fcopy(&line1->x, &t2->x);
                    fcopy(&line1->y, &t2->y);
                    fcopy(&line1->z, &t2->z);
                    line1->x = gabi::fmadds(sp3C->x * gabi::load<f32>(0x101BA464 + 4 * j), REG0_F(0) + 1.0f, x);
                    line1->z = gabi::fmadds(sp3C->z * gabi::load<f32>(0x101BA464 + 4 * j), REG0_F(0) + 1.0f, z);
                } else {
                    line1->copy(i_this->m2FC[i]);
                }
                line2->copy(i_this->m2D8[i]);
                *gabi::at<be<u8>>(gabi::ea(lineSize)) = 3;
            }

            dCcD_Cyl* cyl = &i_this->mCyls[i];
            if (cyl->ChkTgHit() && (i_this->m6EC[i] < 10)) {
                i_this->m6EC[i] = REG0_S(3) + 0xf;
                ccAtInfo->mpObj = gabi::ea(cyl->GetTgHitObj());
                at_power_check(ccAtInfo.get());

                u8 dmg = ccAtInfo->mDamage;
                if (dmg > 1) {
                    dmg = 4;
                    ccAtInfo->mDamage = 4;
                }
                s8 hp = (s8)(i_this->m6F2[i] - dmg);
                i_this->m6F2[i] = hp;
                u32 actor = ccAtInfo->mpActor;

                s32 soundId;
                if (hp <= 0) {
                    i_this->m2D4[i] = 1;
                    i_this->m2C4 = 2000.0f;
                    i_this->m2BC = 2000.0f;
                    soundId = 0x2838; /* JA_SE_LK_CUT_SBRIDGE_ROPE */
                    i_this->m6F5 = 0x14;
                } else {
                    soundId = 0x2837; /* JA_SE_LK_HIT_SBRIDGE_ROPE */
                }

                if (actor != 0) {
                    sp48->copy(i_this->m2D8[i]);
                    fcopy(&sp48->y, gabi::at<be<f32>>(actor + 0x380)); /* eyePos.y */
                    dComIfGp_particle_set(0xD /* ID_AK_JN_OK */, sp48, &player->shape_angle);
                    /* kikuzu_set inlined */
                    fopAc_ac_c* pl = dComIfGp_getPlayer(0);
                    sp18->x = pl->shape_angle.x;
                    sp18->y = pl->shape_angle.y;
                    sp18->y = (s16)(pl->shape_angle.y - 0x8000);
                    sp18->z = pl->shape_angle.z;
                    GXColor* k0 = gabi::at<GXColor>(gabi::ea(&i_this->tevStr) + 0x98); /* tevStr.mColorK0 */
                    JPABaseEmitter* e = dComIfGp_particle_set(0x2B /* ID_AK_JN_ELEMENTKIKUZU00 */, sp48, sp18, nullptr,
                                                              0xff, nullptr, -1, k0, k0, nullptr);
                    if (e != nullptr) {
                        u32 b = gabi::ea(e);
                        gabi::store<f32>(b + 0x34, 10.0f); /* setRate */
                        gabi::store<f32>(b + 0x58, 0.2f);  /* setSpread */
                        gabi::store<s32>(b + 0x5C, 1);     /* setMaxFrame */
                        gabi::store<f32>(b + 0x7C, 0.15f); /* setVolumeSweep */
                        f32 s = REG0_F(16) + 0.7f;
                        gabi::store<f32>(b + 0x238, s); /* setGlobalParticleScale */
                        gabi::store<f32>(b + 0x23C, s);
                        gabi::store<f32>(b + 0x240, s);
                    }
                    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(i_this));
                    mDoAud_seStart(soundId, gabi::at<cXyz>(ccAtInfo->mpActor + 0x37C), 0, reverb);
                }
            }
            cyl->SetC(&i_this->m2D8[i]);
            dComIfG_Ccsp_Set(cyl);
        } else {
            line1->copy(i_this->m2D8[i]);
            himo_cut_control(i_this, line1, i_this->mLineMat.getSize(i), 0);
            line2->copy(i_this->m2FC[i]);
            himo_cut_control(i_this, line2, i_this->mLineMat.getSize(i + 3), 1);
        }
    }
}

/* 021C2748 (mflft_move, eff_cont and himo_move inlined) */
static BOOL daMflft_Execute(mflft_class* i_this) {
    WWHD_FUNC(0x021C2748, BOOL, i_this);
    dComIfGp_get(); /* HD: an unused accessor call */
    if (i_this->m2CE != 0) {
        i_this->m2CE = i_this->m2CE - 1;
    } else {
        i_this->m2D0 = 0.0f;
    }

    cXyz* wv = dKyw_get_wind_vec();
    wind_vec_g = gabi::ea(wv);
    wy_g = cM_atan2s(wv->x, wv->z);
    wp_g = gabi::ea(dKyw_get_wind_power());
    dComIfGp_get(); /* HD: an unused accessor call */
    mflft_move(i_this);

    MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
    cMtx_YrotM(calc_mtx(), i_this->shape_angle.y);
    cMtx_YrotM(calc_mtx(), i_this->m6F8);
    cMtx_XrotM(calc_mtx(), i_this->m6FA);
    cMtx_YrotM(calc_mtx(), -i_this->m6F8);
    cMtx_XrotM(calc_mtx(), i_this->shape_angle.x);
    cMtx_ZrotM(calc_mtx(), i_this->shape_angle.z);
    J3DModel_setBaseTRMtx(i_this->mpModel, calc_mtx());

    for (s32 i = 0; i < 3; i++) {
        /* static f32 xd[] = {0.0f, -117.0f, 117.0f} (0x101BA4C4), zd[] = {-134.0f, 70.0f, 70.0f} (0x101BA4D0) */
        MtxPush();
        gabi::Local<cXyz> sp08;
        sp08->x = gabi::load<f32>(0x101BA4C4 + 4 * i) * i_this->scale.x;
        sp08->y = 15.0f;
        sp08->z = gabi::load<f32>(0x101BA4D0 + 4 * i) * i_this->scale.z;
        MtxPosition(sp08.get(), &i_this->m2D8[i]);

        if (i_this->m6F6 != 0) {
            i_this->m6F6 = i_this->m6F6 - 1;
            i_this->m2FC[i].copy(i_this->m2D8[i]);
            i_this->m2FC[i].y = i_this->home.pos.y + 2000.0f + REG0_F(19);
        }
        if (i_this->m6EC[i] != 0) {
            i_this->m6EC[i] = i_this->m6EC[i] - 1;
        }
        MtxPull();
    }

    MtxPush();
    eff_cont(i_this);
    MtxPull();

    f32 y = 1.0f;
    if (i_this->m2CE != 0) {
        y = 5.0f;
    }
    MtxScale(1.0f, y, 1.0f, 1);
    PSMTXCopy(calc_mtx(), &i_this->m700);
    dBgW_Move(i_this->pm_bgw);
    himo_move(i_this);
    return TRUE;
}
VERIFY(0x021C2748, daMflft_Execute);

/* 021C38D4 */
static BOOL daMflft_IsDelete(mflft_class*) {
    WWHD_FUNC(0x021C38D4, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021C38D4, daMflft_IsDelete);

/* 021C38DC */
static BOOL daMflft_Delete(mflft_class* i_this) {
    WWHD_FUNC(0x021C38DC, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, STR(0x10014B64) /* "Mflft" */); /* dComIfG_resDeleteDemo */
    if (i_this->heap != nullptr) {
        cBgS_Release(dComIfG_Bgsp(), i_this->pm_bgw);
    }
    if (i_this->m774 != nullptr) {
        JPABaseEmitter_becomeInvalidEmitter(i_this->m774);
        i_this->m774 = nullptr;
    }
    return TRUE;
}
VERIFY(0x021C38DC, daMflft_Delete);

/* 021C395C */
static BOOL CallbackCreateHeap(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x021C395C, BOOL, a_this);
    mflft_class* actor = (mflft_class*)a_this;

    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x10014B6C), dRes_INDEX_MFLFT_BDL_MFLFT_e, SAFESTRING_VTBL);
    J3DModel* model = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    actor->mpModel = model;
    if (model == nullptr) {
        return FALSE;
    }
    if (modelData == nullptr) /* JUT_ASSERT(1053, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x10014B7C), 0x41E, STR(0x10014B8C));

    dBgW* bgw = new_dBgW();
    actor->pm_bgw = bgw;
    if (bgw == nullptr) /* JUT_ASSERT(1058, actor->pm_bgw != NULL) */
        JUT_ASSERT_fail(STR(0x10014B7C), 0x423, STR(0x10014BA0));

    cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes(STR(0x10014B6C), dRes_INDEX_MFLFT_DZB_MFLFT_e, SAFESTRING_VTBL);
    cBgW_Set(actor->pm_bgw, dzb, cBgW_MOVE_BG_e, &actor->m700);
    gabi::store<u32>(gabi::ea(actor->pm_bgw.get()) + 0xA8, 0x024EE658); /* SetCrrFunc(dBgS_MoveBGProc_Typical) */
    gabi::store<u32>(gabi::ea(actor->pm_bgw.get()) + 0xB0, 0x021C2064); /* SetRideCallback(ride_call_back) */
    void* img = dComIfG_getObjectRes(STR(0x10014B74) /* "Always" */, dRes_INDEX_ALWAYS_BTI_ROPE_e, SAFESTRING_VTBL);
    if (!lineMat_init(&actor->mLineMat, 6, 10, img, 1)) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x021C395C, CallbackCreateHeap);

/* 021C3AC0 */
static cPhs_State daMflft_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x021C3AC0, cPhs_State, a_this);
    mflft_class* i_this = (mflft_class*)a_this;

    /* fopAcM_ct(&i_this->actor, mflft_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            a_this->__vtbl = MFLFT_VTBL;
            dCcD_Stts_ct(&i_this->mStts);
            __construct_array_l(i_this->mCyls, 3, 0x130, 0x021C4094);
            gabi::call(0x025EB82C, &i_this->mLineMat); /* mDoExt_3DlineMat1_c::mDoExt_3DlineMat1_c */
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    dComIfGp_get(); /* HD: an unused accessor call */

    cPhs_State PVar2 = dComIfG_resLoad(&i_this->mPhase, STR(0x10014BD0) /* "Mflft" */);
    if (PVar2 == cPhs_COMPLEATE_e) {
        u8 m2A0 = fopAcM_GetParam(a_this) & 0xFF;
        if (m2A0 == 0xff) {
            m2A0 = 0;
        }
        i_this->m2A0 = m2A0;
        i_this->m2A1 = fopAcM_GetParam(a_this) >> 0x18;
        if (!fopAcM_entrySolidHeap(a_this, 0x021C395C /* CallbackCreateHeap */, 0x10000)) {
            return cPhs_ERROR_e;
        }
        if ((i_this->pm_bgw != nullptr) && dBgS_Regist(dComIfG_Bgsp(), i_this->pm_bgw, a_this)) {
            return cPhs_ERROR_e;
        }

        switch (i_this->m2A0) {
        case 1:
            a_this->scale.x = 0.9f;
            a_this->scale.z = 0.9f;
            break;
        case 2:
            a_this->scale.x = 0.8f;
            a_this->scale.z = 0.8f;
            break;
        case 3:
            a_this->scale.x = 0.7f;
            a_this->scale.z = 0.7f;
            break;
        default:
            a_this->scale.z = 1.0f;
            a_this->scale.x = 1.0f;
            break;
        }
        a_this->scale.y = 1.0f;
        a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpModel)); /* fopAcM_SetMtx */
        fopAcM_SetMin(a_this, -200.0f * a_this->scale.x, -10000.0f, -200.0f * a_this->scale.z);
        fopAcM_SetMax(a_this, 200.0f * a_this->scale.x, 10000.0f, 200.0f * a_this->scale.z);

        J3DModel_setBaseScale_bits(i_this->mpModel, &a_this->scale);
        i_this->mStts.Init(0xff, 0xff, a_this);

        for (s32 i = 0; i < 3; i++) {
            i_this->mCyls[i].Set(gabi::at<dCcD_SrcCyl>(0x101BA4DC) /* himo_cyl_src */);
            i_this->mCyls[i].SetStts(&i_this->mStts);
            i_this->m6F2[i] = 3;
        }

        i_this->m6F6 = 10;
        if ((i_this->m2A1 != 0xff) && dComIfGs_isSwitch(i_this->m2A1, fopAcM_GetRoomNo(a_this))) {
            for (s32 i = 0; i < 3; i++) {
                i_this->m2D4[i] = 1;
            }
            gabi::Local<u8[0x54]> gndChk;
            yogan_ct(gndChk.get());
            cXyz* gp = gabi::at<cXyz>(gabi::ea(gndChk.get()) + 0x24);
            fcopy(&gp->x, &a_this->current.pos.x);
            gp->y = a_this->current.pos.y + 1000.0f;
            fcopy(&gp->z, &a_this->current.pos.z);
            f32 fVar9 = cBgS_GroundCross(dComIfG_Bgsp(), gndChk.get());
            if (fVar9 != -1000000000.0f) {
                a_this->current.pos.y = fVar9;
            }
            dBgS_GndChk_dt(gndChk.get());
        }

        daMflft_Execute(i_this);
    }
    return PVar2;
}
VERIFY(0x021C3AC0, daMflft_Create);

/* 021C3FEC: the header statics sit at P+0xA / P+0xB here (wy uses P+8) */
static void __sinit_d_a_mflft_cpp() {
    WWHD_FUNC(0x021C3FEC, void, (u32)0);
    const u32 P = 0x10464FC4, D = 0x101BA520;
    for (int i = 0; i < 4; i++) gabi::store<u32>(0x10464FD0 + 4 * i, 0);
    __register_global_object(D);
    gabi::store<f32>(P, -3.1415927f);
    gabi::store<f32>(P + 4, 3.1415927f);
    gabi::call(0x028ED6F8, P + 0xA);
    __register_global_object(D + 0xC);
    gabi::call(0x028EAB2C, P + 0xB);
    __register_global_object(D + 0x18);
}
VERIFY(0x021C3FEC, __sinit_d_a_mflft_cpp);

/* 021C4080: sead::SafeString deleting destructor (this TU's vtable slot) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021C4080, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x021C4080, SafeString_dt);

/* 021C4094: array element constructor of mCyls (dCcD_Cyl::dCcD_Cyl, this TU's copy; allocates when
 * this == NULL) */
static dCcD_Cyl* mCyls_ct(dCcD_Cyl* c) {
    WWHD_FUNC(0x021C4094, dCcD_Cyl*, c);
    if (c == nullptr) {
        c = (dCcD_Cyl*)operator_new(0x130);
        if (c == nullptr)
            return c;
    }
    dCcD_Cyl_ct(c, AAB_VTBL);
    return c;
}
VERIFY(0x021C4094, mCyls_ct);

/* 021C4120: mflft_class deleting destructor (compiler-generated, vtable +0xC) */
static void mflft_class_dt(mflft_class* i_this, s32 flags) {
    WWHD_FUNC(0x021C4120, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025EB8B8, &i_this->mLineMat, 2); /* mDoExt_3DlineMat1_c::~mDoExt_3DlineMat1_c */
        __destroy_arr_l(i_this->mCyls, 3, 0x130, 0x02515A70 /* dCcD_Cyl::~dCcD_Cyl */, 0);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021C4120, mflft_class_dt);

/* 021C41AC: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x021C41AC, void, (u32)0);
}
VERIFY(0x021C41AC, SafeString_assureTerminationImpl);
