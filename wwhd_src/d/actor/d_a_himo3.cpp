/**
 * d_a_himo3.cpp (WWHD)
 * Object - Climbable Rope (Pirate Ship rope minigame, Forsaken Fortress, Puppet Ganon's room)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_himo3.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x10011170
#define HIMO3_VTBL 0x100111C8 /* HD: himo3_class vtable */

enum {
    dRes_INDEX_HIMO3_BMD_H3_GA_e = 3,
    dRes_INDEX_BGN_BTI_NOT_CUT1_e = 0x2E,
    dRes_INDEX_ALWAYS_BTI_ROPE_e = 0x7E,
};
enum { AT_TYPE_WIND = 0x00200000 };
enum { dPa_name_ID_AK_JN_TORCH = 0x1EA, dPa_name_ID_AK_JP_O_KAGEROU00 = 0x4004 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline BOOL fopAcM_createHeap(fopAc_ac_c* a, u32 size, u32 align) { return gabi::call<BOOL>(0x025D5FEC, a, size, align); }
static inline void fopAcM_adjustHeap(fopAc_ac_c* a) { gabi::call(0x025D6100, a); }
static inline cXyz* dKyw_get_wind_vec() { return gabi::call<cXyz*>(0x0257DAA8); }
static inline u32 dKyw_get_wind_power() { return gabi::call<u32>(0x0257DB04); }
static inline void mDoMtx_XrotS(Mtx34* m, s16 x) { gabi::call(0x025F18EC, m, x); }
static inline void __construct_array(void* p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void __destroy_arr(void* p, u32 n, u32 size, u32 dtor, u32 flags) { gabi::call(0x028F0164, p, n, size, dtor, flags); }
/* sead::SafeString equality (HD inline): both sides' virtual assureTermination (+0x14), then a
 * byte compare bounded by 0x40001 (as d_a_kb) */
static inline bool SafeString_eq(SafeString* a, SafeString* b) {
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a);
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a);
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b);
    if (s1 == b->mStringTop) return true;
    u32 p = a->mStringTop, q = b->mStringTop;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c = gabi::load<u8>(p + i);
        if (c != gabi::load<u8>(q + i)) return false;
        if (c == 0) return true;
    }
    return false;
}
/* strcmp(dComIfGp_getStartStageName(), lit) == 0 (HD: SafeString comparison; the start stage name at play+0x5134) */
static inline bool dComIfGp_isStartStage(u32 lit) {
    gabi::Local<SafeString> a;
    a->mStringTop = lit;
    a->__vtbl = SAFESTRING_VTBL;
    gabi::Local<SafeString> b;
    b->mStringTop = dComIfGp_ea() + 0x5134;
    b->__vtbl = SAFESTRING_VTBL;
    return SafeString_eq(a.get(), b.get());
}

/* himo3HIO_c (HD: vtable after the members) */
struct himo3HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01;
    /* 0x02 */ be<s16> m06;
    /* 0x04 */ be<f32> m08;
    /* 0x08 */ be<f32> m0C;
    /* 0x0C */ be<s16> m10;
    /* 0x0E */ be<s16> m12;
    /* 0x10 */ be<s16> m14;
    /* 0x12 */ u8 _12[2];
    /* 0x14 */ be<f32> m18;
    /* 0x18 */ be<u32> __vtbl;
};
WWHD_SIZE(himo3HIO_c, 0x1C);
static himo3HIO_c& l_HIO() { return *gabi::at<himo3HIO_c>(0x104646E0); }
static be<f32>& HIMO3_SCALE() { return *gabi::at<be<f32>>(0x104646C0); }
static be<u8>& hio_set() { return *gabi::at<be<u8>>(0x101B6EAC); }

struct himo3_s {
    /* 0x00 */ cXyz m00;
    /* 0x0C */ cXyz m0C;
};
WWHD_SIZE(himo3_s, 0x18);

struct h3_ga_s {
    /* 0x00 */ gptr<J3DModel> mpModel;
    /* 0x04 */ cXyz mPos;
    /* 0x10 */ cXyz m10;
    /* 0x1C */ csXyz m1C;
    /* 0x22 */ u8 _22[2];
    /* 0x24 */ be<f32> m24;
    /* 0x28 */ be<f32> m28;
    /* 0x2C */ be<s16> m2C;
    /* 0x2E */ be<u8> m2E;
    /* 0x2F */ be<u8> m2F;
};
WWHD_SIZE(h3_ga_s, 0x30);

/* GameCube +0x11C up to m15C0; HD inserts a mDoExt_J3DModelPacketS (0xB0) after m15C0 and the
 * line material grew (0x3C -> 0x18C); m20F8 (shadow size) is gone. Size 0x2634. */
struct himo3_class : fopAc_ac_c {
    /* 0x03AC */ request_of_phase_process_class mPhase;
    /* 0x03B4 */ be<u8> m0298;
    /* 0x03B5 */ be<u8> m0299;
    /* 0x03B6 */ be<u8> m029A;
    /* 0x03B7 */ be<s8> m029B;
    /* 0x03B8 */ be<s8> m029C;
    /* 0x03B9 */ be<s8> m029D;
    /* 0x03BA */ u8 _3BA[2];
    /* 0x03BC */ be<u32> ppd; /* dPath* */
    /* 0x03C0 */ be<s8> m02A4;
    /* 0x03C1 */ u8 _3C1[3];
    /* 0x03C4 */ cXyz m02A8;
    /* 0x03D0 */ be<s16> m02B4[3];
    /* 0x03D6 */ be<s16> m02BA;
    /* 0x03D8 */ be<s16> m02BC;
    /* 0x03DA */ be<u8> m02BE;
    /* 0x03DB */ u8 _3DB;
    /* 0x03DC */ himo3_s m02C0[200];
    /* 0x169C */ u8 mLineMat[0x18C]; /* mDoExt_3DlineMat1_c (HD 0x18C; vtable at +0x130, positions at +0x184) */
    /* 0x1828 */ be<s32> m15C0;
    /* 0x182C */ u8 mPacket[0xB0]; /* HD: mDoExt_J3DModelPacketS */
    /* 0x18DC */ cXyz m15C4;
    /* 0x18E8 */ be<f32> m15D0;
    /* 0x18EC */ be<f32> m15D4;
    /* 0x18F0 */ be<f32> m15D8;
    /* 0x18F4 */ be<f32> m15DC;
    /* 0x18F8 */ be<f32> m15E0;
    /* 0x18FC */ be<f32> m15E4;
    /* 0x1900 */ be<f32> m15E8;
    /* 0x1904 */ u8 _1904[4];
    /* 0x1908 */ be<f32> m15F0;
    /* 0x190C */ be<f32> m15F4;
    /* 0x1910 */ be<s16> m15F8;
    /* 0x1912 */ be<s16> m15FA;
    /* 0x1914 */ be<f32> m15FC;
    /* 0x1918 */ LIGHT_INFLUENCE m1600;
    /* 0x193C */ be<f32> m1620;
    /* 0x1940 */ cXyz m1624;
    /* 0x194C */ cXyz m1630;
    /* 0x1958 */ dBgS_AcchCir mAcchCir;
    /* 0x1998 */ dBgS_ObjAcch mAcch;
    /* 0x1B5C */ Mtx34 m1840;
    /* 0x1B8C */ be<f32> m1870;
    /* 0x1B90 */ u8 _1B90[4];
    /* 0x1B94 */ be<f32> m1878;
    /* 0x1B98 */ be<f32> m187C;
    /* 0x1B9C */ gptr<J3DModel> mpModel;
    /* 0x1BA0 */ dCcD_Stts mStts;
    /* 0x1BDC */ dCcD_Sph mSphs[5];
    /* 0x21B8 */ dCcD_Sph mSph;
    /* 0x22E4 */ dCcD_Cyl mCyl;
    /* 0x2414 */ dPa_followEcallBack m20FC;
    /* 0x2428 */ be<u8> m2110;
    /* 0x2429 */ be<u8> m2111;
    /* 0x242A */ u8 _242A[2];
    /* 0x242C */ h3_ga_s m2114[1];
    /* 0x245C */ dKy_tevstr_c m2144;
    /* 0x2624 */ cXyz m21F4;
    /* 0x2630 */ be<s16> m2200;
    /* 0x2632 */ u8 _2632[2];
};
WWHD_OFFSET(himo3_class, m02C0, 0x3DC);
WWHD_OFFSET(himo3_class, m15C0, 0x1828);
WWHD_OFFSET(himo3_class, m15C4, 0x18DC);
WWHD_OFFSET(himo3_class, m1600, 0x1918);
WWHD_OFFSET(himo3_class, mAcch, 0x1998);
WWHD_OFFSET(himo3_class, mpModel, 0x1B9C);
WWHD_OFFSET(himo3_class, mCyl, 0x22E4);
WWHD_OFFSET(himo3_class, m2114, 0x242C);
WWHD_OFFSET(himo3_class, m21F4, 0x2624);
WWHD_SIZE(himo3_class, 0x2634);

/* dPath (HD layout as GameCube): m_num u16 +0, m_nextID u16 +2, loop flag byte +5, points +8 (0x10 each:
 * mArg3 +3, position +4) */
static inline u32 dPath_point(u32 ppd, s32 i) { return gabi::load<u32>(ppd + 8) + i * 16; }

/* daPy_py_c (HD): status word 2 at +0x3C0 (rope grab right hand: 4), left hand +0x3F0, right hand +0x3FC */
static inline void copy_f3(cXyz* dst, u32 src) {
    f32 z = gabi::load<f32>(src + 8);
    f32 y = gabi::load<f32>(src + 4);
    f32 x = gabi::load<f32>(src + 0);
    dst->y = y;
    dst->x = x;
    dst->z = z;
}

/* ---- ga (bat) ---- */

/* 021727D4 */
static BOOL daHimo3_Draw(himo3_class* i_this) {
    WWHD_FUNC(0x021727D4, BOOL, i_this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    f32 fVar1;
    if (i_this->m0298 == 0xF) {
        fVar1 = 4.625f; /* HD: no REG0_F(0) */
    } else {
        fVar1 = 3.75f;
    }

    /* i_this->mLineMat.update(m15C0, fVar1, (GXColor){200, 150, 50, 255}, 0, &tevStr) */
    gabi::call(0x025EC62C, i_this->mLineMat, (u16)gabi::load<u16>(gabi::ea(i_this) + 0x182A), fVar1, (u32)0x101B6EE4, (u16)0,
               &i_this->tevStr);
    /* dComIfGd_set3DlineMat(&mLineMat): the sort packet of the line material's id */
    u32 packets = dComIfGp_ea() + 0x5FB4;
    s32 matId = gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(gabi::ea(i_this) + 0x17CC) + 0x14), i_this->mLineMat);
    gabi::call(0x025EDD04, packets + matId * 0x9C, i_this->mLineMat); /* mDoExt_3DlineMatSortPacket::setMat */

    if (i_this->m0298 != 0xF) {
        J3DModel* model = i_this->mpModel;
        setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
        mDoExt_modelUpdateDL(model);

        if (i_this->m0298 == 0) {
            /* HD: no alpha model and no simple shadow */
            /* ga_draw (inlined) */
            h3_ga_s* ga = &i_this->m2114[0];
            if (ga->m2E == 1) {
                MtxTrans(ga->mPos.x, ga->mPos.y, ga->mPos.z, 0);
                cMtx_YrotM(calc_mtx(), ga->m1C.y);
                cMtx_XrotM(calc_mtx(), ga->m1C.x);
                f32 s = ga->m24;
                MtxScale(s, s * ga->m28, s, true);
                MtxTrans(0.0f, REG_F(10, 9) + -2.0f /* HD: GameCube -18.0f */, 0.0f, 1);
                J3DModel_setBaseTRMtx(ga->mpModel, calc_mtx());
                mDoExt_modelUpdateDL(ga->mpModel);
            }
        }
    }
    return TRUE;
}
VERIFY(0x021727D4, daHimo3_Draw);

/* path_move (inlined into daHimo3_Execute) */
static inline void path_move(himo3_class* i_this) {
    fopAc_ac_c* actor = i_this;

    switch (i_this->m02A4) {
    case 0: {
        i_this->m029C = (s8)(i_this->m029C + i_this->m029D);
        u32 ppd = i_this->ppd;
        if (i_this->m029C >= (s8)gabi::load<u8>(ppd + 1) /* (s8)ppd->m_num */) {
            u32 p7 = i_this->ppd;
            if (gabi::load<u8>(ppd + 5) & 1 /* dPath_ChkClose */) {
                i_this->m029C = 0;
            } else {
                i_this->m029D = -1;
                i_this->m029C = (s8)(gabi::load<u16>(i_this->ppd) - 2);
            }

            u16 next = gabi::load<u16>(p7 + 2);
            if (next != 0xFFFF) {
                i_this->ppd = gabi::ea(dPath_GetRoomPath(next, fopAcM_GetRoomNo(actor)));
                if (i_this->ppd == 0) /* JUT_ASSERT(732, i_this->ppd != NULL) */
                    JUT_ASSERT_fail(STR(0x100111E8), 0x2DC, STR(0x10011208));
            }
        } else if (i_this->m029C < 0) {
            i_this->m029D = 1;
            i_this->m029C = 1;
        }

        i_this->m02A4 = 1;
        u32 pnt = dPath_point(i_this->ppd, i_this->m029C);
        i_this->m02A8.x = gabi::load<f32>(pnt + 4);
        i_this->m02A8.y = gabi::load<f32>(pnt + 8);
        i_this->m02A8.z = gabi::load<f32>(pnt + 0xC);
    }
        // Fall-through
    case 1: {
        gabi::Local<cXyz> tmp;
        cXyz_mi(&i_this->m02A8, tmp.get(), &actor->current.pos);
        gabi::Local<cXyz> sp10;
        sp10->copy(*tmp);
        if (i_this->m02BE == 0) {
            u8 uStack_14 = gabi::load<u8>(dPath_point(i_this->ppd, i_this->m029C) + 3);
            if (uStack_14 != 0 && uStack_14 != 0xFF) {
                f32 f = (f32)uStack_14;
                cLib_addCalc2(&actor->speedF, f, 1.0f, f * 0.1f);
            } else {
                uStack_14 = i_this->m0299;
                f32 f = (f32)uStack_14;
                cLib_addCalc2(&actor->speedF, f, 1.0f, f * 0.1f);
            }
        } else {
            cLib_addCalc0(&actor->speedF, 1.0f, 1.0f);
        }

        gabi::Local<cXyz> sp28;
        sp28->x = 0.0f;
        sp28->y = 0.0f;
        sp28->z = actor->speedF;
        Mtx34* m = calc_mtx();
        s16 ay = cM_atan2s(sp10->x, sp10->z);
        mDoMtx_YrotS(m, ay);
        f32 x = sp10->x, z = sp10->z;
        m = calc_mtx();
        f32 fVar8 = std_sqrtf(gabi::fmadds(x, x, z * z));
        s16 ax = cM_atan2s(sp10->y, fVar8);
        cMtx_XrotM(m, (s16)-ax);
        MtxPosition(sp28.get(), &actor->speed);
        PSVECAdd(&actor->current.pos, &actor->speed, &actor->current.pos);
        f32 d = std_sqrtf(PSVECSquareMag(sp10.get())); /* sp10.abs() */
        f32 sp = actor->speedF;
        if (d < sp + sp) {
            i_this->m02A4 = 0;
        }
        break;
    }
    }
}

/* himo3_control (inlined into daHimo3_Execute); r31 = i_this->m02C0 */
static inline void himo3_control(himo3_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s32 i = 1;
    himo3_s* r31 = &i_this->m02C0[1];
    f32 f28 = 0.0f;
    gabi::Local<cXyz> spA0;
    gabi::Local<cXyz> sp94;

    if (i_this->m02BE != 0) {
        gabi::Local<cXyz> spAC;
        s16 target;
        if (i_this->m02BE == 1) {
            if (gabi::load<u32>(gabi::ea(player) + 0x3C0) & 4 /* getRopeGrabRightHand */) {
                copy_f3(spAC.get(), gabi::ea(player) + 0x3FC); /* getRightHandPos */
            } else {
                copy_f3(spAC.get(), gabi::ea(player) + 0x3F0); /* getLeftHandPos */
            }
            target = player->shape_angle.y;
        } else {
            spAC->copy(i_this->m21F4);
            target = i_this->m2200;
        }

        if (i_this->m15C4.y == -23535.0f) {
            i_this->m15C4.copy(*spAC);
        }

        gabi::Local<cXyz> tmp;
        cXyz_mi(spAC.get(), tmp.get(), &actor->current.pos);
        gabi::Local<cXyz> sp88;
        sp88->copy(*tmp);
        f32 sqrt = std_sqrtf(PSVECSquareMag(sp88.get())); /* sp88.abs() */
        f32 q = sqrt / HIMO3_SCALE();
        s32 iVar8_2 = gabi::ftoi(q);
        f28 = q - (f32)iVar8_2;
        if (iVar8_2 >= i_this->m15C0 - 1) {
            iVar8_2 = i_this->m15C0 - 1;
        }

        if (iVar8_2 > 1) {
            f32 denom = (f32)(iVar8_2 - 1);
            for (i = 1; i < iVar8_2; i++, r31++) {
                f32 fVar12 = (f32)i / denom;
                r31->m00.x = gabi::fmadds(sp88->x, fVar12, actor->current.pos.x);
                r31->m00.y = gabi::fmadds(sp88->y, fVar12, actor->current.pos.y);
                r31->m00.z = gabi::fmadds(sp88->z, fVar12, actor->current.pos.z);
            }
        }

        cLib_addCalcAngleS2(&actor->current.angle.y, target, 2, 0x2000);

        cXyz_mi(spAC.get(), tmp.get(), &i_this->m15C4);
        spA0->copy(*tmp);
        cMtx_YrotS(calc_mtx(), (s16)-target);
        MtxPosition(spA0.get(), sp94.get());
        /* HD: no REG0_F(5) / REG0_F(6) */
        cLib_addCalc2(&i_this->m15D0, sp94->z * -10.0f, 0.1f, 10.0f * i_this->m15E4);
        cLib_addCalc2(&i_this->m15D8, sp94->x * -10.0f, 0.1f, 10.0f * i_this->m15E4);

        if (std::fabs((f32)sp94->z) > 1.0f || std::fabs((f32)sp94->x) > 1.0f) {
            cLib_addCalc2(&i_this->m15E4, 1.0f, 1.0f, 0.05f);
        } else {
            cLib_addCalc2(&i_this->m15E4, 0.1f, 1.0f, 0.05f);
        }

        if (fopAcM_GetParam(actor) == 3) {
            i_this->m15E0 = 0.0f;
            i_this->m02BE = 0;
            i_this->m02BC = 0x1E;
        }
        i_this->m15C4.copy(*spAC);
    } else {
        f32 d4 = i_this->m15D4;
        f32 d0 = i_this->m15D0 + d4;
        i_this->m15D0 = d0;
        i_this->m15D4 = gabi::fmadds(d0, -0.003f, d4); /* HD: no REG0_F(7) */
        cLib_addCalc0(&i_this->m15D0, 0.05f, 1.0f);
        f32 dc = i_this->m15DC;
        f32 d8 = i_this->m15D8 + dc;
        i_this->m15D8 = d8;
        i_this->m15DC = gabi::fmadds(d8, -0.003f, dc);
        cLib_addCalc0(&i_this->m15D8, 0.05f, 1.0f);

        if (fopAcM_GetParam(actor) == 1) {
            i_this->m15E0 = 1.0f;
            i_this->m02BE = 1;
            if (dComIfGp_isStartStage(0x10011130) /* "majroom" */ || dComIfGp_isStartStage(0x10011138) /* "Majroom" */ ||
                dComIfGp_isStartStage(0x10011150) /* "MajyuE" */)
            {
                dSv_event_onEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), 0x402 /* UNK_0402 */);
            }
        } else if (fopAcM_GetParam(actor) == 2) {
            i_this->m15E0 = 1.0f;
            i_this->m02BE = 2;
        }
        i_this->m15C4.y = -23535.0f;
    }

    cMtx_YrotS(calc_mtx(), actor->current.angle.y);
    spA0->x = i_this->m15D8;
    spA0->y = 0.0f;
    spA0->z = i_this->m15D0;
    gabi::Local<cXyz> sp7C;
    MtxPosition(spA0.get(), sp7C.get());
    cXyz* pcVar7 = dKyw_get_wind_vec();
    s16 iVar8 = cM_atan2s(pcVar7->x, pcVar7->z);
    u32 pfVar9 = dKyw_get_wind_power();

    if (i_this->m02BE != 0) {
        cLib_addCalc2(&i_this->m15E8, gabi::load<f32>(pfVar9) * 0.5f, 1.0f, 0.05f);
        if (i_this->m15F0 > 0.01f) {
            i_this->m15FA = (s16)(i_this->m15FA + 1);
        } else {
            i_this->m15FA = 0;
        }
    } else {
        cLib_addCalc2(&i_this->m15E8, gabi::load<f32>(pfVar9), 1.0f, 0.001f);
        if (i_this->m15F0 > 0.01f) {
            i_this->m15FA = (s16)(i_this->m15FA + 1);
        } else {
            i_this->m15FA = 0;
        }
    }

    cLib_addCalc2(&i_this->m15F0, i_this->m15F4, 0.2f, 0.05f);
    cLib_addCalc0(&i_this->m15F4, 1.0f, 0.005f);
    cMtx_YrotS(calc_mtx(), i_this->m15F8);
    spA0->x = 0.0f;
    spA0->y = 0.0f;
    spA0->z = cM_ssin(i_this->m15FA * 500) * 100.0f * i_this->m15F0;
    gabi::Local<cXyz> sp70;
    MtxPosition(spA0.get(), sp70.get());
    cMtx_YrotS(calc_mtx(), iVar8);
    spA0->x = 0.0f;
    spA0->y = 0.0f;
    spA0->z = cM_ssin(i_this->m02BA * 500) * 100.0f * i_this->m15E8;
    gabi::Local<cXyz> sp64;
    MtxPosition(spA0.get(), sp64.get());
    PSVECAdd(sp64.get(), sp70.get(), sp64.get()); /* sp64 += sp70 */
    spA0->x = 0.0f;
    spA0->y = 0.0f;

    s16 unaff_r24 = 0;
    s16 unaff_r23 = 0;
    int j = 0;
    f32 f27;
    if ((i_this->m02BE == 0) && (i_this->m02BC == 0)) {
        f27 = 30.0f;
    } else {
        f27 = -200.0f;
    }

    u8 bVar4 = 0;

    for (; i < i_this->m15C0; i++, r31++) {
        /* HD: tmpReg 0.0f, tmp200 -200.0f (no REG); the zero sp60 terms are gone */
        f32 x = (((f32)(r31->m00.x - r31[-1].m00.x) + r31->m0C.x) + sp7C->x) + sp64->x;
        f32 y = ((f32)(r31->m00.y - r31[-1].m00.y) + -200.0f) + r31->m0C.y;
        f32 z = (((f32)(r31->m00.z - r31[-1].m00.z) + r31->m0C.z) + sp7C->z) + sp64->z;

        unaff_r24 = (s16)-cM_atan2s(y, z);
        unaff_r23 = cM_atan2s(x, std_sqrtf(gabi::fmadds(y, y, z * z)));
        mDoMtx_XrotS(calc_mtx(), unaff_r24);
        cMtx_YrotM(calc_mtx(), unaff_r23);

        f32 sc = HIMO3_SCALE();
        if (bVar4 == 0) {
            spA0->z = gabi::fnmsubs(f28, sc, sc); /* HIMO3_SCALE - f28 * HIMO3_SCALE */
            bVar4++;
        } else {
            spA0->z = sc;
        }
        MtxPosition(spA0.get(), sp94.get());
        r31->m0C.copy(r31->m00);
        gabi::Local<cXyz> sum;
        cXyz_pl(&r31[-1].m00, sum.get(), sp94.get());
        f32 nx = sum->x;
        f32 ox = r31->m0C.x;
        r31->m00.x = nx;
        f32 ny = sum->y;
        f32 oy = r31->m0C.y;
        r31->m00.y = ny;
        f32 nz = sum->z;
        f32 oz = r31->m0C.z;
        r31->m00.z = nz;
        r31->m0C.x = (nx - ox) * 0.0f;
        r31->m0C.y = (ny - oy) * 0.0f;
        r31->m0C.z = (nz - oz) * 0.0f;

        if (i_this->m0298 == 0xF && (u32)i == (u32)(i_this->m15C0 - 1)) {
            if (i_this->m02BE == 0) {
                i_this->mCyl.SetC(&r31->m00);
            } else {
                gabi::Local<cXyz> sp4C;
                sp4C->x = -10000.0f;
                sp4C->z = 0.0f;
                sp4C->y = -10000.0f;
                i_this->mCyl.SetC(sp4C.get());
            }
            dComIfG_Ccsp_Set(&i_this->mCyl);
        } else if (((i + (i_this->m02BA * 3)) & 0xF) == 0 && j < 5) {
            i_this->mSphs[j].SetR(f27);
            i_this->mSphs[j].SetC(&r31->m00);
            dComIfG_Ccsp_Set(&i_this->mSphs[j]);
            j++;
        }
    }

    /* mLineMat.getPos(0) */
    u32 pcVar10 = gabi::load<u32>(gabi::load<u32>(gabi::ea(i_this) + 0x1820));
    r31 = i_this->m02C0;
    for (i = 0; i < i_this->m15C0; i++, r31++, pcVar10 += 12) {
        cXyz* pos = gabi::at<cXyz>(pcVar10);
        if (!(i_this->m15E0 < 0.999f)) {
            pos->copy(r31->m00);
        } else {
            cLib_addCalc2(&pos->x, r31->m00.x, 1.0f, 100.0f * i_this->m15E0);
            cLib_addCalc2(&pos->y, r31->m00.y, 1.0f, 100.0f * i_this->m15E0);
            cLib_addCalc2(&pos->z, r31->m00.z, 1.0f, 100.0f * i_this->m15E0);
        }

        if ((i_this->m0298 != 0xF) && ((u32)i == (u32)(i_this->m15C0 - 1))) {
            MtxTrans(pos->x, pos->y, pos->z, 0);
            cMtx_XrotM(calc_mtx(), unaff_r24);
            cMtx_YrotM(calc_mtx(), unaff_r23);
            f32 fVar14 = l_HIO().m08;
            if (i_this->m0298 != 1) {
                cMtx_XrotM(calc_mtx(), -0x4000);
                fVar14 = l_HIO().m0C;
            }
            MtxScale(fVar14, fVar14, fVar14, true);
            J3DModel_setBaseTRMtx(i_this->mpModel, calc_mtx());
        }
    }

    cLib_addCalc2(&i_this->m15E0, 1.0f, 1.0f, 0.05f); /* HD: no REG0_F(3) */
}

/* ga_move (inlined into daHimo3_Execute) */
static inline void ga_move(himo3_class* i_this, cXyz* sp30) {
    h3_ga_s* ga = &i_this->m2114[0];
    if (ga->m2E) {
        if (ga->m2F != 0) {
            ga->m2F = (u8)(ga->m2F - 1);
        } else {
            ga->m2F = (u8)gabi::ftoi(cM_rndF(10.0f));
            f32 r = cM_rndFX(150.0f);
            ga->m10.x = i_this->m1624.x + r;
            r = cM_rndFX(100.0f);
            ga->m10.y = i_this->m1624.y + r;
            r = cM_rndFX(150.0f);
            ga->m10.z = i_this->m1624.z + r;
        }

        gabi::Local<cXyz> sp0C;
        cXyz_mi(&ga->m10, sp0C.get(), &ga->mPos);
        f32 x = sp0C->x, z = sp0C->z, y = sp0C->y;
        cLib_addCalcAngleS2(&ga->m1C.y, cM_atan2s(x, z), 2, 0x1000);
        cLib_addCalcAngleS2(&ga->m1C.x, (s16)-cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z))), 2, 0x1000);
        cMtx_YrotS(calc_mtx(), ga->m1C.y);
        cMtx_XrotM(calc_mtx(), ga->m1C.x);
        gabi::Local<cXyz> sp24;
        MtxPosition(sp30, sp24.get());
        PSVECAdd(&ga->mPos, sp24.get(), &ga->mPos);
        ga->m28 = cM_ssin(ga->m2C);
        ga->m2C = (s16)(ga->m2C + 0x3E00);
    }
}

/* 02172A00 */
static BOOL daHimo3_Execute(himo3_class* i_this) {
    WWHD_FUNC(0x02172A00, BOOL, i_this);
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    if (i_this->m02BC != 0) {
        i_this->m02BC = (s16)(i_this->m02BC - 1);
    }

    if (i_this->m029B != 0) {
        path_move(i_this);
    }

    i_this->m02BA = (s16)(i_this->m02BA + 1);
    i_this->m02C0[0].m00.copy(actor->current.pos);
    himo3_control(i_this);

    if (i_this->m0298 != 0xF) {
        if (i_this->mSph.ChkTgHit()) {
            void* pcVar5 = i_this->mSph.GetTgHitObj();
            if (pcVar5 != nullptr && cCcD_Obj_ChkAtType(pcVar5, AT_TYPE_WIND)) {
                i_this->m15F4 = 1.0f; /* HD: no REG0_F(0) */
                i_this->m15F8 = player->shape_angle.y;
            }
        }

        dBgS* bgs = dComIfG_Bgsp();
        i_this->mAcch.CrrPos(bgs);
        J3DModel* model = i_this->mpModel;
        i_this->m1870 = l_HIO().m18;
        PSMTXCopy(J3DModel_getBaseTRMtx(model), calc_mtx());

        if (i_this->m0298 == 0) {
            MtxTrans(10.0f, -140.0f, -15.0f, 1);
        } else {
            MtxTrans(0.0f, 0.0f, 35.0f, 1);
        }

        MtxScale(i_this->m1870, i_this->m1870, i_this->m1870, true);
        PSMTXCopy(calc_mtx(), &i_this->m1840);

        gabi::Local<cXyz> sp28;
        sp28->z = 0.0f;
        sp28->y = 0.0f;
        sp28->x = 0.0f;
        MtxPosition(sp28.get(), &i_this->m1624);
        i_this->mSph.SetC(&i_this->m1624);
        dComIfG_Ccsp_Set(&i_this->mSph);

        if (i_this->m0298 == 0) {
            for (int i = 0; i < 3; i++) {
                if (i_this->m02B4[i] != 0) {
                    i_this->m02B4[i] = (s16)(i_this->m02B4[i] - 1);
                }
            }

            if (i_this->m02B4[0] == 0) {
                i_this->m02B4[0] = (s16)gabi::ftoi(cM_rndF(10.0f) + 5.0f);
                i_this->m187C = cM_rndF(16.0f) + 4.0f;
            }

            if (i_this->m02B4[1] == 0) {
                i_this->m02B4[1] = (s16)gabi::ftoi(cM_rndF(6.0f) + 3.0f);
            }

            cLib_addCalc2(&i_this->m1878, i_this->m187C, 1.0f, 0.1f);
            JPABaseEmitter* pJVar6 = i_this->m20FC.getEmitter();
            if (pJVar6 == nullptr) {
                /* static cXyz fire_scale(0.7f, 0.7f, 0.7f); (guard 0x10464708) */
                cXyz* fire_scale = gabi::at<cXyz>(0x104646FC);
                if (gabi::load<u32>(0x10464708) == 0) {
                    gabi::store<u32>(0x10464708, 1);
                    fire_scale->x = 0.7f;
                    fire_scale->y = 0.7f;
                    fire_scale->z = 0.7f;
                }
                dComIfGp_particle_set(dPa_name_ID_AK_JN_TORCH, &i_this->m1624, nullptr, fire_scale, 0xFF,
                                      (dPa_levelEcallBack*)&i_this->m20FC);
                pJVar6 = i_this->m20FC.getEmitter();
                i_this->m1620 = 1.0f;
            }

            if (pJVar6 != nullptr) {
                f32 x = (f32)(i_this->m1624.x - i_this->m1630.x) * -0.03f;
                if (x > 1.0f) {
                    x = 1.0f;
                } else if (x < -1.0f) {
                    x = -1.0f;
                }

                f32 z = (f32)(i_this->m1624.z - i_this->m1630.z) * -0.03f;
                if (z > 1.0f) {
                    z = 1.0f;
                } else if (z < -1.0f) {
                    z = -1.0f;
                }

                u32 e = gabi::ea(pJVar6);
                gabi::store<f32>(e + 0x28, x); /* setDirection */
                gabi::store<f32>(e + 0x2C, 0.1f);
                gabi::store<f32>(e + 0x30, z);

                f32 s = std_sqrtf(gabi::fmadds(x, x, z * z));
                f32 y = (s + s) + 1.0f;
                if (y > 4.0f) {
                    y = 4.0f;
                }

                gabi::store<f32>(e + 0x238, 1.0f); /* setGlobalParticleScale */
                gabi::store<f32>(e + 0x23C, y);
                gabi::store<f32>(e + 0x240, 1.0f);
                cLib_addCalc2(&i_this->m1620, cM_rndF(0.2f) + 1.0f, 0.5f, 0.02f);
            } else {
                i_this->m1620 = 0.0f;
            }

            gabi::Local<cXyz> sp1C;
            sp1C->z = i_this->m1624.z;
            sp1C->x = i_this->m1624.x;
            sp1C->y = i_this->m1624.y + 20.0f;
            dComIfGp_particle_setSimple(dPa_name_ID_AK_JP_O_KAGEROU00, sp1C.get());
            i_this->m1600.mPos.copy(i_this->m1624);
            i_this->m1600.mColorR = 600;
            i_this->m1600.mColorG = 400;
            i_this->m1600.mColorB = 0x78;
            i_this->m1600.mPower = (f32)(s16)gabi::ftoi(150.0f * i_this->m1620);
            i_this->m1600.mFluctuation = 250.0f;
            gabi::Local<cXyz> sp30;
            sp30->z = 10.0f;
            sp30->y = 0.0f;
            sp30->x = 0.0f;
            ga_move(i_this, sp30.get());
        }
    }

    i_this->m1630.copy(i_this->m1624);

    return TRUE;
}
VERIFY(0x02172A00, daHimo3_Execute);

/* 021743E8 */
static BOOL daHimo3_IsDelete(himo3_class*) {
    WWHD_FUNC(0x021743E8, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021743E8, daHimo3_IsDelete);

/* 021743F0 */
static BOOL daHimo3_Delete(himo3_class* i_this) {
    WWHD_FUNC(0x021743F0, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, STR(0x100112C8) /* "Himo3" */);
    if (i_this->m0298 == 0) {
        dKy_plight_cut(&i_this->m1600);
    }

    if (i_this->m2110 != 0) {
        s8 no = l_HIO().mNo;
        hio_set() = false;
        mDoHIO_deleteChild(no);
    }

    i_this->m20FC.remove();
    return TRUE;
}
VERIFY(0x021743F0, daHimo3_Delete);

/* 0217447C himo3_class::himo3_class (HD: out of line; allocates when this == NULL) */
static himo3_class* himo3_class_ct(himo3_class* i_this) {
    WWHD_FUNC(0x0217447C, himo3_class*, i_this);
    if (i_this == nullptr) {
        i_this = (himo3_class*)operator_new(0x2634);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = HIMO3_VTBL;
    gabi::call(0x025EB82C, i_this->mLineMat); /* mDoExt_3DlineMat1_c::mDoExt_3DlineMat1_c */
    gabi::call(0x02080404, i_this->mPacket);  /* mDoExt_J3DModelPacketS (HD) */
    i_this->m1600.mHD20 = 1.0f;               /* LIGHT_INFLUENCE (inline) */
    dBgS_AcchCir_ct(&i_this->mAcchCir);
    gabi::call(0x024F0474, &i_this->mAcch); /* dBgS_Acch::dBgS_Acch; then the ObjAcch vtables */
    u32 a = gabi::ea(&i_this->mAcch);
    gabi::store<u32>(a + 0x10, 0x10011198);
    gabi::store<u32>(a + 0x20, 0x100111A8);
    gabi::store<u32>(a + 0x14, 0x100111B8);
    gabi::store<u8>(a + 0x18, 1);
    dCcD_Stts_ct(&i_this->mStts);
    __construct_array(i_this->mSphs, 5, 0x12C, 0x025166F0 /* dCcD_Sph::dCcD_Sph */);
    gabi::call(0x025166F0, &i_this->mSph);
    dCcD_Cyl_ct(&i_this->mCyl, 0x10011188);
    dPa_followEcallBack_ct(&i_this->m20FC, 0, 0);
    /* dKy_tevstr_c m2144: three copies of a 0x44-byte light block (template 0x1016E414) at +0, +0xC0, +0x144 */
    const u32 T = 0x1016E414;
    const u32 d[3] = {gabi::ea(&i_this->m2144), gabi::ea(&i_this->m2144) + 0xC0, gabi::ea(&i_this->m2144) + 0x144};
    for (int k = 0; k < 3; k++) {
        for (u32 o = 0; o < 0x18; o += 4) gabi::store<f32>(d[k] + o, gabi::load<f32>(T + o));
        for (u32 o = 0x18; o < 0x1C; o++) gabi::store<u8>(d[k] + o, gabi::load<u8>(T + o));
        for (u32 o = 0x1C; o < 0x24; o += 2) gabi::store<s16>(d[k] + o, gabi::load<s16>(T + o));
        for (u32 o = 0x24; o < 0x44; o += 4) gabi::store<f32>(d[k] + o, gabi::load<f32>(T + o));
    }
    return i_this;
}
VERIFY(0x0217447C, himo3_class_ct);

/* useHeapInit (inlined into daHimo3_Create) */
static inline cPhs_State useHeapInit(himo3_class* i_this) {
    fopAc_ac_c* actor = i_this;
    /* static int hook_bmd[] (0x101B6ED0) */

    void* res;
    if (i_this->m0298 == 0xF) {
        res = dComIfG_getObjectRes(STR(0x10011144) /* "Bgn" */, dRes_INDEX_BGN_BTI_NOT_CUT1_e, SAFESTRING_VTBL);
    } else {
        res = dComIfG_getObjectRes(STR(0x10011158) /* "Always" */, dRes_INDEX_ALWAYS_BTI_ROPE_e, SAFESTRING_VTBL);
    }
    if (!gabi::call<BOOL>(0x025EBA58, i_this->mLineMat, 1, 200, res, 0)) { /* mLineMat.init */
        return cPhs_ERROR_e;
    }

    J3DModelData* modelData = nullptr;
    if (i_this->m0298 != 0xF) {
        if (i_this->m0298 == 1) {
            modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x10011160) /* "Link" */, gabi::load<s32>(0x101B6ED4), SAFESTRING_VTBL);
        } else if (i_this->m0298 <= 4) {
            modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x10011148) /* "Himo3" */,
                                                            gabi::load<s32>(0x101B6ED0 + 4 * i_this->m0298), SAFESTRING_VTBL);
        }

        if (modelData == nullptr) /* JUT_ASSERT(1063, modelData != NULL) */
            JUT_ASSERT_fail(STR(0x100111F8), 0x427, STR(0x1001121C));

        i_this->mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        if (i_this->mpModel == nullptr) {
            return cPhs_ERROR_e;
        }

        if (i_this->m0298 == 0) {
            modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x10011148), dRes_INDEX_HIMO3_BMD_H3_GA_e, SAFESTRING_VTBL);
            if (modelData == nullptr) /* JUT_ASSERT(1076, modelData != NULL) */
                JUT_ASSERT_fail(STR(0x100111F8), 0x434, STR(0x1001121C));

            i_this->m2114[0].mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
            if (i_this->m2114[0].mpModel == nullptr) {
                return cPhs_INIT_e;
            }

            /* tmp == 0: always */
            i_this->m2114[0].m2E = true;
            i_this->m2114[0].mPos.copy(actor->current.pos);
            i_this->m2114[0].m24 = cM_rndF(0.3f) + 0.3f;
            i_this->m2114[0].m2C = (s16)gabi::ftoi(cM_rndF(30000.0f));
        }
    }

    /* HD: in stage 0x10011168 the shadow packet is set up */
    if (dComIfGp_isStartStage(0x10011168)) {
        gabi::call(0x0207FD38, i_this->mPacket, 0);
    }
    return cPhs_COMPLEATE_e;
}

/* 02174730 */
static cPhs_State daHimo3_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02174730, cPhs_State, a_this);
    himo3_class* i_this = (himo3_class*)a_this;

    /* fopAcM_ct(a_this, himo3_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            himo3_class_ct(i_this);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    gabi::call(0x028F5914, i_this->m02C0, 0x4B0); /* HD: clears the first 0x4B0 bytes of m02C0 */

    cPhs_State PVar1 = dComIfG_resLoad(&i_this->mPhase, STR(0x100112EC) /* "Himo3" */);
    if (PVar1 == cPhs_COMPLEATE_e) {
        u32 prm = fopAcM_GetParam(a_this);
        i_this->m0298 = (u8)prm;
        i_this->m0299 = (u8)(prm >> 16);
        i_this->m029A = (u8)(prm >> 24);

        if ((prm & 0xFF) == 0xFF) {
            i_this->m0298 = 0;
        }

        if (fopAcM_createHeap(a_this, 0x86220, 0) == 0) {
            return cPhs_ERROR_e;
        }

        PVar1 = useHeapInit(i_this);
        fopAcM_adjustHeap(a_this);

        if (PVar1 == cPhs_ERROR_e) {
            return cPhs_ERROR_e;
        }

        i_this->m15C0 = (fopAcM_GetParam(a_this) >> 8) & 0xFF;
        if (i_this->m0298 == 0xF) {
            HIMO3_SCALE() = 22.0f;
            i_this->m15C0 = 200;
            i_this->m029A = 0xFF;
        } else {
            HIMO3_SCALE() = 20.0f;
        }

        /* HD: in stage 0x100112F4, a rope near x = 500 is lowered */
        bool lower = false;
        if (dComIfGp_isStartStage(0x100112F4)) {
            if (std::fabs(500.0f - a_this->current.pos.x) < 50.0f) {
                lower = true;
            }
        }
        if (lower) {
            a_this->current.pos.y = a_this->current.pos.y - (REG_F(10, 0) + 65.0f);
        }

        s32 n = i_this->m15C0;
        f32 len = gabi::fmsubs((f32)n, HIMO3_SCALE(), 50.0f);
        i_this->m15FC = len;
        if (i_this->m0298 == 0) {
            i_this->m15FC = len - 50.0f;
        }

        if (n > 200) {
            return cPhs_ERROR_e;
        }

        if (i_this->m029A != 0xFF) {
            i_this->ppd = gabi::ea(dPath_GetRoomPath(i_this->m029A, fopAcM_GetRoomNo(a_this)));
            if (i_this->ppd == 0) {
                return cPhs_ERROR_e;
            }
            i_this->m029B = (s8)(i_this->m029A + 1);
            i_this->m029D = 1;
        }

        i_this->m15E0 = 1.0f;
        i_this->mStts.Init(0xFF, 0xFF, a_this);

        if (i_this->m0298 == 0xF) {
            i_this->mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101B6F68) /* cc_cyl_src */);
            i_this->mCyl.SetStts(&i_this->mStts);
        } else {
            for (int i = 0; i < 5; i++) {
                i_this->mSphs[i].Set(gabi::at<dCcD_SrcSph>(0x101B6EE8) /* sph_src */);
                i_this->mSphs[i].SetStts(&i_this->mStts);
            }

            i_this->mSph.Set(gabi::at<dCcD_SrcSph>(0x101B6F28) /* sph2_src */);
            i_this->mSph.SetStts(&i_this->mStts);
            i_this->mAcch.Set(&i_this->m1624, &i_this->m1630, a_this, 1, &i_this->mAcchCir, &a_this->speed);

            if (i_this->m0298 == 0) {
                i_this->mAcchCir.SetWall(40.0f, 50.0f);
                /* HD: no m20F8 (shadow size) */
                dKy_plight_set(&i_this->m1600);
            } else if (i_this->m0298 == 1) {
                i_this->mAcchCir.SetWall(20.0f, 20.0f);
                i_this->mSph.SetR(20.0f);
            }
        }

        if (!hio_set()) {
            hio_set() = true;
            i_this->m2110 = 0;
            l_HIO().mNo = mDoHIO_createChild(STR(0x100112FC) /* "ぶら下がりロープ" */, &l_HIO());
        }

        for (int i = 0; i < 10; i++) {
            daHimo3_Execute(i_this);
        }
    }
    return PVar1;
}
VERIFY(0x02174730, daHimo3_Create);

/* 02174F44 */
static void setActorHang(himo3_class* i_this, cXyz* arg1, s16 arg2) {
    WWHD_FUNC(0x02174F44, void, i_this, arg1, arg2);
    i_this->m21F4.copy(*arg1);
    i_this->m2200 = arg2;
}
VERIFY(0x02174F44, setActorHang);

/* 02174F64 */
static void __sinit_d_a_himo3_cpp() {
    WWHD_FUNC(0x02174F64, void, (u32)0);
    sinit_header_statics(0x104646C4, 0x101B6FAC);
    /* static himo3HIO_c l_HIO (inline constructor; mNo is not set) */
    himo3HIO_c& h = l_HIO();
    h.m06 = 0;
    h.m10 = 0xEB;
    h.m08 = 0.75f;
    h.m12 = 0x7D;
    h.__vtbl = 0x100111D8;
    h.m0C = 0.5f;
    h.m14 = 0;
    h.m18 = 5.0f;
}
VERIFY(0x02174F64, __sinit_d_a_himo3_cpp);

/* 02175050: deleting destructor of a class with a trivial destructor (himo3HIO_c) */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02175050, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02175050, trivial_dt);

/* 02175064: himo3_class deleting destructor (HD) */
static void himo3_class_dt(himo3_class* p, s32 flags) {
    WWHD_FUNC(0x02175064, void, p, flags);
    if (p != nullptr) {
        dCcD_Cyl_dt(&p->mCyl, 2);
        gabi::call(0x02515AE8, &p->mSph, 2); /* dCcD_Sph::~dCcD_Sph */
        __destroy_arr(p->mSphs, 5, 0x12C, 0x02515AE8, 0);
        dCcD_Stts_dt(&p->mStts, 2);
        /* dBgS_ObjAcch::~dBgS_ObjAcch (inline): the base vtables, then dBgS_Acch::~dBgS_Acch */
        u32 a = gabi::ea(&p->mAcch);
        gabi::store<u32>(a + 0x20, 0x100111A8);
        gabi::store<u32>(a + 0x14, 0x100111B8);
        gabi::call(0x024EFD9C, &p->mAcch, 0);
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&p->mAcchCir) + 0x14), 2); /* mAcchCir's cM3dGCir */
        gabi::call(0x02082DDC, p->mPacket, 2);                                 /* mDoExt_J3DModelPacketS::~ */
        gabi::call(0x025EB8B8, p->mLineMat, 2);                                /* mDoExt_3DlineMat1_c::~ (matcher: draw) */
        gabi::call(0x025D50BC, p, 0);                                          /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x02175064, himo3_class_dt);

/* 02175144: empty virtual */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x02175144, void, p);
}
VERIFY(0x02175144, empty_virtual);
