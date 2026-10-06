/**
 * d_a_bdkobj.cpp (WWHD)
 * Object - Helmaroc King fight objects (blocks, poles and the tower bridge, with fragments).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bdkobj.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define BDKOBJ_VTBL 0x100084A4     /* bdkobj_class vtable (HD virtual destructor) */
#define SAFESTRING_VTBL 0x1000835C /* this TU's sead::SafeString vtable */
#define CM3DGAAB_VTBL 0x10008374   /* this TU's cM3dGAab vtable */
#define cc_cyl_src gabi::at<dCcD_SrcCyl>(0x10190B34)
#define hahen_sph_src gabi::at<dCcD_SrcSph>(0x10190AF4)
#define non_pos gabi::at<cXyz>(0x10461988) /* static cXyz non_pos(10000, -10000, 20000) */

enum { dRes_INDEX_BDKOBJ_DZB_S_TOWER_BRIDGE_e = 0xC };
enum : s16 { fpcNm_PLAYER_e = 0xA8 };
enum : u16 {
    ID_IT_SN_DK_BLOCK_S00 = 0x813E,
    ID_IT_SN_DK_BLOCK_L00 = 0x813F,
    ID_IT_SN_DK_POLE_S00 = 0x8141,
    ID_IT_SN_DK_POLE_L00 = 0x8142,
    ID_IT_SN_DK_BRIDGE_L00 = 0x8143,
    ID_IT_SN_DK_BRIDGE_S00 = 0x8144,
    ID_IT_SN_DK_BRIDGE_TENITA00 = 0x8145,
    ID_IT_ST_DK_BLOCK_SMOKE00 = 0xA13D,
    ID_IT_ST_DK_POLE_SMOKE00 = 0xA140,
    ID_IT_ST_DK_BRIDGE_SMOKE00 = 0xA146,
};
enum { JA_SE_OBJ_MJ_WBOARD_BRK = 0x6968 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
struct CcAtInfo_l {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B;
    /* 0x0C */ csXyz m0C;
    /* 0x12 */ be<u16> mPlCutBit;
};
/* 02518DB0 at_power_check(CcAtInfo*) */
static inline u32 at_power_check(CcAtInfo_l* info) { return gabi::call<u32>(0x02518DB0, info); }
/* 0201A554 csXyz::operator+=(const csXyz&) (out of line) */
static inline void csXyz_apl(csXyz* a, csXyz* b) { gabi::call(0x0201A554, a, b); }
/* 025A5F88 dPa_smokeEcallBack::end() (HD: called directly for remove()) */
static inline void dPa_smokeEcallBack_end_l(u32 cb) { gabi::call(0x025A5F88, cb); }
/* 025A5B18 dPa_smokeEcallBack::dPa_smokeEcallBack(u8) */
static inline void dPa_smokeEcallBack_ct1(u32 cb, u8 a) { gabi::call(0x025A5B18, cb, a); }
/* 025D8870 fopAcM_createItem(pos, itemNo, itemBitNo, roomNo, type, angle, action, scale) */
static inline fpc_ProcID fopAcM_createItem(cXyz* pos, s32 itemNo, s32 bitNo, s32 roomNo, s32 type, csXyz* angle, s32 action, cXyz* scale) {
    return gabi::call<fpc_ProcID>(0x025D8870, pos, itemNo, bitNo, roomNo, type, angle, action, scale);
}
/* cBgS::GroundCross / LineCross on dComIfG_Bgsp(); cBgS::Release */
static inline f32 bgs_GroundCross(u32 chk) { return gabi::call<f32>(0x02008974, dComIfG_Bgsp(), chk); }
static inline BOOL bgs_LineCross(u32 chk) { return gabi::call<BOOL>(0x02008860, dComIfG_Bgsp(), chk); }
static inline void fopAcM_SetMin_l(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D672C, a, x, y, z); }
static inline void fopAcM_SetMax_l(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D673C, a, x, y, z); }
/* GHS __construct_array / __destroy_arr */
static inline void construct_array(u32 p, s32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void destroy_arr(u32 p, s32 n, u32 size, u32 dtor) { gabi::call(0x028F0164, p, n, size, dtor, 0, 0); }
/* debug registers (HD addresses; the GameCube REGn indices do not all carry over) */
static inline f32 REGF(u32 a) { return gabi::load<f32>(a); }
static inline s16 REGS(u32 a) { return gabi::load<s16>(a); }

/* ---- inline dBgS check objects on the stack (HD vtable values of this TU) ---- */
enum : u32 { GNDCHK_SIZE = 0x54, LINCHK_SIZE = 0x6C };
static inline void dBgS_GndChk_ct(u32 g) {
    gabi::call(0x02008E0C, g); /* cBgS_GndChk::cBgS_GndChk */
    for (u32 i = 0x44; i <= 0x4A; i++) gabi::store<u8>(g + i, 0);
    gabi::store<u32>(g + 0x10, 0x100083A4);
    gabi::store<u32>(g + 0x04, g + 0x4C);
    gabi::store<u32>(g + 0x20, 0x100083B4);
    gabi::store<u32>(g + 0x4C, 0x100083C4);
    gabi::store<u32>(g + 0x50, 1);
    gabi::store<u32>(g + 0x40, 0x100083D4);
    gabi::store<u32>(g + 0x00, g + 0x40);
}
static inline void dBgS_ObjGndChk_Spl_ct(u32 g) {
    gabi::call(0x02008E0C, g); /* cBgS_GndChk::cBgS_GndChk */
    for (u32 i = 0x45; i <= 0x4A; i++) gabi::store<u8>(g + i, 0);
    gabi::store<u32>(g + 0x4C, 0x10008444);
    gabi::store<u32>(g + 0x00, g + 0x40);
    gabi::store<u32>(g + 0x10, 0x10008424);
    gabi::store<u32>(g + 0x50, 0xE);
    gabi::store<u32>(g + 0x04, g + 0x4C);
    gabi::store<u32>(g + 0x20, 0x10008434);
    gabi::store<u32>(g + 0x40, 0x10008454);
    gabi::store<u8>(g + 0x44, 1);
}
static inline void dBgS_GndChk_SetPos(u32 g, f32 x, f32 y, f32 z) {
    gabi::store<f32>(g + 0x24, x);
    gabi::store<f32>(g + 0x28, y);
    gabi::store<f32>(g + 0x2C, z);
}
static inline void dBgS_LinChk_ct(u32 l) {
    gabi::call(0x02008FEC, l); /* cBgS_LinChk::cBgS_LinChk */
    gabi::store<u32>(l + 0x20, 0x10008474);
    for (u32 i = 0x5C; i <= 0x62; i++) gabi::store<u8>(l + i, 0);
    gabi::store<u32>(l + 0x04, l + 0x64);
    gabi::store<u32>(l + 0x64, 0x10008484);
    gabi::store<u32>(l + 0x58, 0x10008494);
    gabi::store<u32>(l + 0x10, 0x10008464);
    gabi::store<u32>(l + 0x00, l + 0x58);
    gabi::store<u32>(l + 0x68, 1);
}
static inline void chk_dt(u32 l, u32 g) {
    gabi::store<u32>(l + 0x58, 0x10008494);
    gabi::store<u32>(l + 0x64, 0x10008394);
    gabi::store<u32>(l + 0x20, 0x10008384);
    gabi::call(0x02008B4C, l, 0); /* cBgS_LinChk::~cBgS_LinChk */
    gabi::store<u32>(g + 0x20, 0x100083B4);
    gabi::store<u32>(g + 0x40, 0x100083D4);
    gabi::store<u32>(g + 0x4C, 0x10008394);
    gabi::call(0x02008DAC, g, 0); /* cBgS_GndChk::~cBgS_GndChk */
}

struct bdo_eff_s {
    /* 0x000 */ be<s8> m000;
    /* 0x001 */ u8 _001[3];
    /* 0x004 */ gptr<J3DModel> mpModel;
    /* 0x008 */ cXyz m008;
    /* 0x014 */ cXyz m014;
    /* 0x020 */ be<f32> m020;
    /* 0x024 */ be<f32> m024x; /* HD: horizontal speed of the stair fragments */
    /* 0x028 */ be<f32> m024z;
    /* 0x02C */ be<f32> m024;
    /* 0x030 */ be<f32> m028;  /* scale */
    /* 0x034 */ be<f32> m02C;
    /* 0x038 */ csXyz m030;
    /* 0x03E */ csXyz m036;
    /* 0x044 */ dCcD_Stts mStts;
    /* 0x080 */ dCcD_Sph mSph;
    /* 0x1AC */ be<f32> m1A4;
    /* 0x1B0 */ be<s16> m1A8;
    /* 0x1B2 */ be<s16> m1AA;
    /* 0x1B4 */ be<s16> m1AC;
    /* 0x1B6 */ u8 _1B6[2];
};
WWHD_OFFSET(bdo_eff_s, mSph, 0x80);
WWHD_SIZE(bdo_eff_s, 0x1B8);

struct bdkobj_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ be<u8> m298;
    /* 0x3B5 */ be<s8> m299;
    /* 0x3B6 */ u8 _3B6[2];
    /* 0x3B8 */ bdo_eff_s mEffs[3];
    /* 0x8E0 */ dCcD_Stts mStts;
    /* 0x91C */ dCcD_Cyl mCyl;
    /* 0xA4C */ u8 m918[0x20]; /* dPa_smokeEcallBack: vtable +0, emitter +4, follow flag +0x12, colour +0x16 */
    /* 0xA6C */ be<u8> m938;
    /* 0xA6D */ u8 _A6D[3];
    /* 0xA70 */ Mtx34 mMtx;
    /* 0xAA0 */ gptr<dBgW> pm_bgw;
};
WWHD_OFFSET(bdkobj_class, mStts, 0x8E0);
WWHD_OFFSET(bdkobj_class, mCyl, 0x91C);
WWHD_OFFSET(bdkobj_class, mMtx, 0xA70);
WWHD_SIZE(bdkobj_class, 0xAA4);

static inline u32 m918_ea(bdkobj_class* i_this) { return gabi::ea(i_this->m918); }
static inline u32 m918_emitter(bdkobj_class* i_this) { return gabi::load<u32>(m918_ea(i_this) + 4); }
static inline J3DModel* actor_model(fopAc_ac_c* a) { return gabi::at<J3DModel>(a->model); }

/* 02072888 */
static void ride_call_back(dBgW* param1, fopAc_ac_c* param2, fopAc_ac_c* param3) {
    WWHD_FUNC(0x02072888, void, param1, param2, param3);
    if (param3 == nullptr || fpcM_GetName(param3) != fpcNm_PLAYER_e) { /* HD: fopAcM_GetName null check */
        return;
    }
    param2->health = 0xA;
}
VERIFY(0x02072888, ride_call_back);

/* 020728A8 (hahen_draw inlined) */
static BOOL daBdkobj_Draw(bdkobj_class* i_this) {
    WWHD_FUNC(0x020728A8, BOOL, i_this);
    fopAc_ac_c* a_this = i_this;
    if (a_this->model != 0) {
        J3DModel* model = actor_model(a_this);
        setLightTevColorType(dKy_getEnvlight(), model, &a_this->tevStr);
        if (i_this->m298 < 2) {
            u32 camera = gabi::load<u32>(dComIfGp_ea() + 0x5AF8);
            gabi::Local<cXyz> vec;
            cXyz_mi(&a_this->current.pos, vec, gabi::at<cXyz>(camera + 0xDC));
            f32 d = std_sqrtf(PSVECSquareMag(vec));
            if (d > REGF(0x1047BABC) + 300.0f) {
                mDoExt_modelUpdateDL(model);
            }
        } else {
            mDoExt_modelUpdateDL(model);
        }
    }
    for (s32 i = 0; i < 3; i++) {
        bdo_eff_s* fragment = &i_this->mEffs[i];
        if (fragment->m000 != 0) {
            J3DModel* model = fragment->mpModel;
            if (model != nullptr) {
                setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
                mDoExt_modelUpdateDL(model);
            }
        }
    }
    return TRUE;
}
VERIFY(0x020728A8, daBdkobj_Draw);

/* rotation and scale matrix of a fragment, into its model */
static inline void eff_set_mtx(bdo_eff_s* i_eff, f32 y) {
    MtxTrans(i_eff->m008.x, y, i_eff->m008.z, 0);
    cMtx_YrotM(calc_mtx(), i_eff->m030.y);
    cMtx_XrotM(calc_mtx(), i_eff->m030.x);
    cMtx_ZrotM(calc_mtx(), i_eff->m030.z);
    f32 scale = i_eff->m028;
    MtxScale(scale, scale, scale, 1);
    J3DModel_setBaseTRMtx(i_eff->mpModel, calc_mtx());
}

/* top_hahen_move (inlined into Execute) */
static inline void top_hahen_move(bdkobj_class* i_this, bdo_eff_s* i_eff, u32 gnd, u32 lin) {
    gabi::Local<cXyz> local_14c;
    gabi::Local<cXyz> cStack344;

    dBgS_GndChk_ct(gnd);
    dBgS_LinChk_ct(lin);

    f32 dVar8 = i_eff->m1A4;
    i_eff->m014.copy(i_eff->m008);
    if (dVar8 > 0.1f) {
        f32 dVar9 = 20.0f * (1.0f - i_eff->m028);
        Mtx34* m = calc_mtx();
        s16 a = i_eff->m1A8;
        if (dVar8 > dVar9) {
            dVar8 = dVar9;
        }
        cMtx_YrotS(m, a);
        local_14c->x = 0.0f;
        local_14c->y = 0.0f;
        local_14c->z = dVar8;
        MtxPosition(local_14c, cStack344);
        PSVECAdd(&i_eff->m008, cStack344, &i_eff->m008);
        i_eff->m030.y = i_eff->m030.y + i_eff->m1AA;
    }
    cLib_addCalc0(&i_eff->m1A4, 1.0f, REGF(0x1047B658) + 0.2f);
    cMtx_YrotS(calc_mtx(), i_eff->m030.y);

    local_14c->x = 0.0f;
    local_14c->y = i_eff->m024;
    local_14c->z = i_eff->m020;
    MtxPosition(local_14c, cStack344);
    PSVECAdd(&i_eff->m008, cStack344, &i_eff->m008);
    i_eff->m024 = i_eff->m024 - 5.0f;

    /* GetCCMoveP(): &mStts, never NULL */
    i_eff->m008.x = i_eff->m008.x + i_eff->mStts.m_cc_move.x;
    i_eff->m008.z = i_eff->m008.z + i_eff->mStts.m_cc_move.z;

    {
        f32 x = i_eff->m008.x;
        f32 z = i_eff->m008.z;
        f32 y = i_eff->m008.y + 200.0f;
        dBgS_GndChk_SetPos(gnd, x, y, z);
    }

    f32 ground = bgs_GroundCross(gnd);
    f32 fVar10 = gabi::fmadds(REGF(0x1047B61C) + 50.0f, i_eff->m028, ground);
    if (!(i_eff->m008.y > fVar10)) {
        i_eff->m008.y = fVar10;
        if (i_eff->m024 < REGF(0x1047BAB4) + -20.0f) {
            i_eff->m024 = -(i_eff->m024 * (REGF(0x1047BAB8) + 0.3f));
            s16 r = (s16)gabi::ftoi(cM_rndFX(5000.0f)); /* HD: 5000 (GameCube 8000) */
            i_eff->m030.y = i_eff->m030.y + r;
            i_eff->m02C = cM_rndFX(100.0f); /* HD: 100 (GameCube 200) */
        } else {
            i_eff->m024 = 0.0f;
            cLib_addCalc0(&i_eff->m020, 1.0f, REGF(0x1047BBCC) + 2.0f); /* HD: 2.0 (GameCube 0.75) */
            i_eff->m036.z = 0;
            f32 v = i_eff->m020;
            s16 sx = (s16)gabi::ftoi(v * (REGF(0x1047BBD0) + 300.0f));
            s16 sy = (s16)gabi::ftoi(v * i_eff->m02C);
            i_eff->m036.x = sx;
            i_eff->m036.y = sy;
        }
    }
    {
        gabi::Local<cXyz> d;
        cXyz_mi(&i_eff->m008, d, &i_eff->m014);
        gabi::store<u32>(gabi::ea(local_14c.get()), gabi::load<u32>(gabi::ea(d.get())));
        local_14c->y = 0.0f;
        gabi::store<u32>(gabi::ea(local_14c.get()) + 8, gabi::load<u32>(gabi::ea(d.get()) + 8));
    }

    if (std_sqrtf(PSVECSquareMag(local_14c)) > 0.0f) {
        Mtx34* m = calc_mtx();
        s16 a = cM_atan2s(local_14c->x, local_14c->z);
        cMtx_YrotS(m, a);
        local_14c->x = 0.0f;
        local_14c->y = 30.0f;
        local_14c->z = 70.0f * i_eff->m028;
        MtxPosition(local_14c, cStack344);
        local_14c->x = i_eff->m008.x;
        f32 y = i_eff->m008.y;
        local_14c->y = y;
        local_14c->z = i_eff->m008.z;
        local_14c->y = y + 30.0f;
        PSVECAdd(cStack344, &i_eff->m008, cStack344);
        dBgS_LinChk_Set(gabi::at<void>(lin), local_14c, cStack344, i_this);
        if (bgs_LineCross(lin)) {
            i_eff->m008.x = i_eff->m014.x;
            i_eff->m008.z = i_eff->m014.z;
            i_eff->m020 = 0.0f;
        }
    }
    csXyz_apl(&i_eff->m030, &i_eff->m036);
    eff_set_mtx(i_eff, i_eff->m008.y);

    i_eff->mSph.SetC(&i_eff->m008);
    i_eff->mSph.SetR((REGF(0x1047BACC) + 85.0f) * i_eff->m028);
    dComIfG_Ccsp_Set(&i_eff->mSph);
    if (i_eff->mSph.ChkTgHit() && i_eff->m1A4 < 1.0f) {
        gabi::Local<CcAtInfo_l> hit_atInfo;
        hit_atInfo->mpObj = gabi::ea(i_eff->mSph.GetTgHitObj());
        hit_atInfo->mpActor = at_power_check(hit_atInfo);
        if (hit_atInfo->mResultingAttackType == 0x8) {
            f32 k = REGF(0x1047BE00) + 15.0f;
            f32 s = 1.0f - i_eff->m028;
            i_eff->m1A4 = s * (k + cM_rndF(1.0f));

            u32 at = hit_atInfo->mpActor;
            if (at != 0) {
                s16 r = (s16)gabi::ftoi(cM_rndFX(2000.0f));
                i_eff->m1A8 = gabi::load<s16>(at + 0x32A) + r;
            }
            i_eff->m1AA = (s16)gabi::ftoi(cM_rndFX(400.0f));
        } else if (hit_atInfo->mResultingAttackType == 0x9) {
            /* HD: a type-9 hit breaks the fragment and drops an item (with a sound and an effect) */
            i_eff->m000 = 0;
            fopAcM_createItem(&i_eff->m008, 0, -1, -1, 0, nullptr, 4, nullptr);
            mDoAud_seStart(0x692B, &i_eff->m008, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
            /* static cXyz scale(3.0f, 3.0f, 3.0f) */
            be<u32>& guard = *gabi::at<be<u32>>(0x10461968);
            cXyz* scale = gabi::at<cXyz>(0x10461994);
            if (guard == 0) {
                guard = 1;
                scale->set(3.0f, 3.0f, 3.0f);
            }
            s8 room = fopAcM_GetRoomNo(i_this);
            dPa_control_set(dComIfGp_getParticle(), 0, 0x3E3, &i_eff->m008, nullptr, scale, 0xFF, nullptr, room,
                            gabi::at<GXColor>(gabi::ea(i_this) + 0x1A8), gabi::at<GXColor>(gabi::ea(i_this) + 0x1A8), nullptr);
        }
    }

    if (i_eff->m008.y < 8500.0f) {
        i_eff->m000 = 0;
    }
    chk_dt(lin, gnd);
}

/* kaidan_hahen_move (inlined into Execute). HD: the stair fragments also move horizontally (with
 * a wall check) and rock on the ground with sizes depending on their scale */
static inline void kaidan_hahen_move(bdkobj_class* i_this, bdo_eff_s* i_eff, u32 gnd, u32 lin) {
    gabi::Local<cXyz> local_78;
    gabi::Local<cXyz> out;

    dBgS_ObjGndChk_Spl_ct(gnd);
    csXyz_apl(&i_eff->m030, &i_eff->m036);

    {
        f32 vy = i_eff->m024;
        f32 y = i_eff->m008.y + vy;
        f32 x = i_eff->m008.x + i_eff->m024x;
        u32 ox = gabi::load<u32>(gabi::ea(&i_eff->m008) + 0);
        i_eff->m008.y = y;
        u32 ny = gabi::load<u32>(gabi::ea(&i_eff->m008) + 4);
        i_eff->m024 = vy - 3.0f;
        f32 z = i_eff->m008.z + i_eff->m024z;
        i_eff->m008.x = x;
        u32 oz = gabi::load<u32>(gabi::ea(&i_eff->m008) + 8);
        i_eff->m008.z = z;
        gabi::store<u32>(gabi::ea(&i_eff->m014) + 0, ox);
        gabi::store<u32>(gabi::ea(&i_eff->m014) + 4, ny);
        gabi::store<u32>(gabi::ea(&i_eff->m014) + 8, oz);
        f32 vx = i_eff->m024x;
        i_eff->m024x = vx * (REGF(0x1047C03C) + 0.985f);
        f32 vz = i_eff->m024z;
        i_eff->m024z = vz * (REGF(0x1047C03C) + 0.985f);
    }

    dBgS_LinChk_ct(lin);
    {
        gabi::Local<cXyz> d;
        cXyz_mi(&i_eff->m008, d, &i_eff->m014);
        gabi::store<u32>(gabi::ea(local_78.get()), gabi::load<u32>(gabi::ea(d.get())));
        local_78->y = 0.0f;
        gabi::store<u32>(gabi::ea(local_78.get()) + 8, gabi::load<u32>(gabi::ea(d.get()) + 8));
    }
    if (std_sqrtf(PSVECSquareMag(local_78)) > 0.0f) {
        Mtx34* m = calc_mtx();
        s16 a = cM_atan2s(local_78->x, local_78->z);
        cMtx_YrotS(m, a);
        local_78->x = 0.0f;
        local_78->y = 30.0f;
        local_78->z = 50.0f;
        MtxPosition(local_78, out);
        local_78->x = i_eff->m008.x;
        f32 y = i_eff->m008.y;
        local_78->y = y;
        local_78->z = i_eff->m008.z;
        local_78->y = y + 30.0f;
        PSVECAdd(out, &i_eff->m008, out);
        dBgS_LinChk_Set(gabi::at<void>(lin), local_78, out, i_this);
        if (bgs_LineCross(lin)) {
            f32 x = i_eff->m014.x;
            f32 z = i_eff->m014.z;
            i_eff->m008.x = x;
            i_eff->m008.z = z;
        }
    }
    dBgS_GndChk_SetPos(gnd, i_eff->m008.x, i_eff->m008.y + 5000.0f, i_eff->m008.z);

    f32 fVal1 = bgs_GroundCross(gnd);
    f32 fVal2 = 0.0f;
    if (fVal1 != -1000000000.0f) {
        if (!(i_eff->m008.y > fVal1)) {
            i_eff->m008.y = fVal1;
            i_eff->m024 = 0.0f;
            cLib_addCalcAngleS2(&i_eff->m036.y, REGS(0x1047C0A8), 1, 10); /* HD: a debug-register target */
            cLib_addCalcAngleS2(&i_eff->m036.z, 0, 1, 10);
            f32 k = (i_eff->m028 - 0.7f) * 2000.0f;
            s16 s1 = (s16)gabi::ftoi(1500.0f - k);
            f32 sin1 = cM_ssin((u16)(i_eff->m1AC * s1));
            s16 s2 = (s16)gabi::ftoi(1800.0f - k);
            s16 target = (s16)gabi::ftoi(sin1 * (REGF(0x1047BBDC) + 800.0f));
            cLib_addCalcAngleS2(&i_eff->m030.x, target, 10, 400);
            f32 sin2 = cM_ssin((u16)(i_eff->m1AC * s2));
            fVal2 = (sin2 * (REGF(0x1047BBD8) + 5.0f)) * i_eff->m028;
        }
    } else {
        i_eff->m000 = 0;
    }

    eff_set_mtx(i_eff, i_eff->m008.y + fVal2);
    chk_dt(lin, gnd);
}

/* tower_kaidan_move (inlined into Execute) */
static inline void tower_kaidan_move(bdkobj_class* i_this) {
    if (i_this->pm_bgw != nullptr) {
        s8 h = i_this->health;
        u32 prm = fopAcM_GetParam(i_this);
        if (h != 0) {
            i_this->health = h - 1;
        }
        if ((prm & 0xf) == 0xf) {
            cBgS_Release(dComIfG_Bgsp(), i_this->pm_bgw);
            i_this->pm_bgw = nullptr;
            i_this->model = 0;
            i_this->m299 = 100;

            dComIfGp_particle_set(ID_IT_SN_DK_BRIDGE_L00, &i_this->current.pos, &i_this->current.angle);
            dComIfGp_particle_set(ID_IT_SN_DK_BRIDGE_S00, &i_this->current.pos, &i_this->current.angle);
            dComIfGp_particle_set(ID_IT_SN_DK_BRIDGE_TENITA00, &i_this->current.pos, &i_this->current.angle);
            dPa_smokeEcallBack_end_l(m918_ea(i_this)); /* m918.remove() */
            /* m918.setColor({0x46, 0x3C, 0x28, 0xB4}) */
            gabi::store<u32>(m918_ea(i_this) + 0x16, 0x463C28B4);
            s8 room = fopAcM_GetRoomNo(i_this);
            /* dComIfGp_particle_setToon: the toon group (2) */
            dPa_control_set(dComIfGp_getParticle(), 2, ID_IT_ST_DK_BRIDGE_SMOKE00, &i_this->current.pos, &i_this->current.angle,
                            nullptr, 0xB4, (dPa_levelEcallBack*)(void*)i_this->m918, room, nullptr, nullptr, nullptr);
            u32 emitter = m918_emitter(i_this);
            if (emitter != 0) {
                /* becomeImmortalEmitter */
                gabi::store<u32>(emitter + 0x254, gabi::load<u32>(emitter + 0x254) | 0x40);
                i_this->m938 = 0xB4;
            }

            /* fopAcM_seStartCurrent (HD inline, null-checked) */
            if (gabi::ea(&i_this->current.pos) != 0)
                mDoAud_seStart(JA_SE_OBJ_MJ_WBOARD_BRK, &i_this->current.pos, 0, dComIfGp_getReverb(i_this->current.roomNo));
            for (s32 i = 0; i < 2; i++) {
                bdo_eff_s* e = &i_this->mEffs[i];
                e->m000 = 2;
                e->m1AC = (s16)gabi::ftoi(cM_rndF(65536.0f));
                e->m028 = cM_rndFX(0.25f) + 0.7f;
                e->m008.x = i_this->current.pos.x + cM_rndFX(500.0f);
                e->m008.y = i_this->current.pos.y;
                e->m008.z = i_this->current.pos.z + cM_rndFX(500.0f);
                e->m030.y = (s16)gabi::ftoi(cM_rndF(65536.0f));
                e->m030.z = (s16)gabi::ftoi(cM_rndF(65536.0f));
                e->m036.y = (s16)gabi::ftoi(cM_rndFX(800.0f));
                e->m030.x = (s16)gabi::ftoi(cM_rndFX(10000.0f));
                e->m036.z = (s16)gabi::ftoi(cM_rndFX(800.0f));
            }
            i_this->actor_status = i_this->actor_status & ~0x100u; /* fopAcM_OffStatus(fopAcStts_CULL_e) */
        }
    } else {
        u32 emitter = m918_emitter(i_this);
        if (emitter != 0) {
            u8 a = i_this->m938;
            if (i_this->m299 <= 0x5A && a != 0) {
                a = a - 2;
                emitter = m918_emitter(i_this);
                i_this->m938 = a;
            }
            gabi::store<u8>(emitter + 0x247, a); /* setGlobalAlpha */
        }
    }
}

static inline void set_actor_mtx(bdkobj_class* i_this) {
    MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
    cMtx_YrotM(calc_mtx(), i_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    cMtx_ZrotM(calc_mtx(), i_this->current.angle.z);
    Mtx34* m = calc_mtx();
    J3DModel_setBaseTRMtx(actor_model(i_this), m);
}

/* 020729A8 */
static BOOL daBdkobj_Execute(bdkobj_class* i_this) {
    WWHD_FUNC(0x020729A8, BOOL, i_this);
    if (i_this->m298 == 2) {
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
        tower_kaidan_move(i_this);
    } else {
        settingTevStruct(dKy_getEnvlight(), 4 /* TEV_TYPE_BG3 */, &i_this->current.pos, &i_this->tevStr);
    }

    if (i_this->m299 != 0) {
        i_this->m299 = i_this->m299 - 1;
    }

    if (i_this->m298 == 2) {
        if (i_this->model != 0) {
            set_actor_mtx(i_this);
            if (i_this->pm_bgw != nullptr) {
                PSMTXCopy(calc_mtx(), &i_this->mMtx);
                dBgW_Move(i_this->pm_bgw);
            }
        }
    } else {
        if (i_this->model != 0) {
            i_this->mCyl.SetC(&i_this->current.pos);
            if (i_this->m298 == 0) {
                i_this->mCyl.SetH(REGF(0x1047B970) + 100.0f);
                i_this->mCyl.SetR(REGF(0x1047B974) + 100.0f);
            } else if (i_this->m298 == 1) {
                i_this->mCyl.SetH(REGF(0x1047B978) + 900.0f);
                i_this->mCyl.SetR(REGF(0x1047B97C) + 100.0f);
            }

            if (i_this->mCyl.ChkTgHit()) {
                gabi::Local<CcAtInfo_l> hit_atInfo;
                hit_atInfo->mpObj = gabi::ea(i_this->mCyl.GetTgHitObj());
                u32 at = at_power_check(hit_atInfo);
                if (at != 0 && hit_atInfo->mResultingAttackType == 0xB) {
                    i_this->eyePos.copy(i_this->current.pos);

                    u16 uVar8, uVar6, uVar4;
                    if (i_this->m298 == 0) {
                        uVar8 = ID_IT_ST_DK_BLOCK_SMOKE00;
                        uVar6 = ID_IT_SN_DK_BLOCK_S00;
                        uVar4 = ID_IT_SN_DK_BLOCK_L00;
                    } else {
                        uVar8 = ID_IT_ST_DK_POLE_SMOKE00;
                        uVar6 = ID_IT_SN_DK_POLE_S00;
                        uVar4 = ID_IT_SN_DK_POLE_L00;
                    }
                    /* static csXyz eff_ang = at->shape_angle; (guard 0x104619A0) */
                    be<u32>& guard = *gabi::at<be<u32>>(0x104619A0);
                    csXyz* eff_ang = gabi::at<csXyz>(0x1046196C);
                    if (guard == 0) {
                        guard = 1;
                        gabi::store<u16>(0x1046196C, gabi::load<u16>(at + 0x328));
                        gabi::store<u16>(0x1046196E, gabi::load<u16>(at + 0x32A));
                    }
                    eff_ang->z = 0;
                    eff_ang->x = 0;

                    dPa_smokeEcallBack_end_l(m918_ea(i_this)); /* m918.remove() */
                    {
                        s8 room = fopAcM_GetRoomNo(i_this);
                        dPa_control_set(dComIfGp_getParticle(), 2, uVar8, &i_this->current.pos, eff_ang, nullptr, 0xB9,
                                        (dPa_levelEcallBack*)(void*)i_this->m918, room, nullptr, nullptr, nullptr);
                    }
                    GXColor* k0 = gabi::at<GXColor>(gabi::ea(i_this) + 0x1A8); /* tevStr.mColorK0 */
                    {
                        s8 room = fopAcM_GetRoomNo(i_this);
                        dPa_control_set(dComIfGp_getParticle(), 0, uVar6, &i_this->current.pos, eff_ang, nullptr, 0xFF, nullptr, room, k0, k0,
                                        nullptr);
                    }
                    {
                        s8 room = fopAcM_GetRoomNo(i_this);
                        dPa_control_set(dComIfGp_getParticle(), 0, uVar4, &i_this->current.pos, gabi::at<csXyz>(at + 0x328), nullptr, 0xFF,
                                        nullptr, room, k0, k0, nullptr);
                    }

                    i_this->model = 0;
                    for (int i = 0; i < 3; i++) {
                        bdo_eff_s* e = &i_this->mEffs[i];
                        e->m000 = 1;
                        e->m028 = cM_rndFX(0.25f) + 0.7f;
                        f32 px = i_this->current.pos.x;
                        e->m008.x = px;
                        f32 py = i_this->current.pos.y;
                        e->m008.y = py;
                        e->m008.z = i_this->current.pos.z;
                        e->m008.y = py + (REGF(0x1047BAC0) + 100.0f);
                        e->m024 = (cM_rndF(20.0f) + 60.0f) + REGF(0x1047BAD0);
                        s16 r = (s16)gabi::ftoi(cM_rndFX(10000.0f));
                        e->m030.y = gabi::load<s16>(at + 0x32A) + r;
                        e->m020 = (cM_rndF(15.0f) + 20.0f) + REGF(0x1047BAC8);
                        e->m036.x = (s16)gabi::ftoi(cM_rndFX(6000.0f));
                        e->m036.z = (s16)gabi::ftoi(cM_rndFX(6000.0f));
                    }
                    i_this->actor_status = i_this->actor_status & ~0x100u; /* fopAcM_OffStatus(fopAcStts_CULL_e) */
                    return TRUE;
                }
            }
        } else {
            i_this->mCyl.SetC(non_pos);
        }

        if (i_this->model != 0) {
            set_actor_mtx(i_this);
            dComIfG_Ccsp_Set(&i_this->mCyl);
        }
    }

    /* hahen_move */
    for (s32 i = 0; i < 3; i++) {
        bdo_eff_s* fragment = &i_this->mEffs[i];
        if (fragment->m000 != 0) {
            fragment->m1AC = fragment->m1AC + 1;
            gabi::Local<u8[GNDCHK_SIZE]> gnd;
            gabi::Local<u8[LINCHK_SIZE]> lin;
            if (fragment->m000 == 1) {
                top_hahen_move(i_this, fragment, gabi::ea(gnd.get()), gabi::ea(lin.get()));
            } else {
                kaidan_hahen_move(i_this, fragment, gabi::ea(gnd.get()), gabi::ea(lin.get()));
            }
        }
    }
    return TRUE;
}
VERIFY(0x020729A8, daBdkobj_Execute);

/* 020742BC */
static BOOL daBdkobj_IsDelete(bdkobj_class*) {
    WWHD_FUNC(0x020742BC, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x020742BC, daBdkobj_IsDelete);

/* 020742C4: HD: dComIfG_resDelete (GameCube resDeleteDemo) */
static BOOL daBdkobj_Delete(bdkobj_class* i_this) {
    WWHD_FUNC(0x020742C4, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, STR(0x10008540) /* "Bdkobj" */);
    u32 cb = m918_ea(i_this);
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(cb) + 0x44), cb); /* m918.remove() (virtual) */
    if (i_this->pm_bgw != nullptr) {
        dBgS* bgs = dComIfG_Bgsp();
        cBgS_Release(bgs, i_this->pm_bgw);
    }
    return TRUE;
}
VERIFY(0x020742C4, daBdkobj_Delete);

/* static u16 bdl_data[3] (0x10190AC4), hahen_bdl_data[3] (0x10190ACC) */
static inline u16 bdl_data(s32 i) { return gabi::load<u16>(0x10190AC4 + 2 * i); }
static inline u16 hahen_bdl_data(s32 i) { return gabi::load<u16>(0x10190ACC + 2 * i); }

/* 02074330 */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02074330, BOOL, a_this);
    bdkobj_class* i_this = (bdkobj_class*)a_this;

    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x10008548) /* "Bdkobj" */, bdl_data(i_this->m298), SAFESTRING_VTBL);
    J3DModel* model = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    a_this->model = gabi::ea(model);
    if (model == nullptr) {
        return FALSE;
    }
    s32 iVar4 = 0;
    if (i_this->m298 == 2) {
        i_this->pm_bgw = new_dBgW();
        if (i_this->pm_bgw == nullptr) /* JUT_ASSERT(0x394, i_this->pm_bgw != NULL) */
            JUT_ASSERT_fail(STR(0x10008568), 0x394, STR(0x10008550));
        cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes(STR(0x10008548), dRes_INDEX_BDKOBJ_DZB_S_TOWER_BRIDGE_e, SAFESTRING_VTBL);
        if (cBgW_Set(i_this->pm_bgw, dzb, cBgW_MOVE_BG_e, &i_this->mMtx)) {
            return FALSE;
        }
        gabi::store<u32>(gabi::ea(i_this->pm_bgw.get()) + 0xA8, 0x024EE658); /* SetCrrFunc(dBgS_MoveBGProc_Typical) */
        gabi::store<u32>(gabi::ea(i_this->pm_bgw.get()) + 0xB0, 0x02072888); /* SetRideCallback(ride_call_back) */
        iVar4 = -1;
    }
    for (s32 i = 0; i < iVar4 + 3; i++) {
        J3DModelData* hahen_modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x10008548), hahen_bdl_data(i_this->m298), SAFESTRING_VTBL);
        i_this->mEffs[i].mpModel = mDoExt_J3DModel__create(hahen_modelData, 0x80000, 0x11000022);
        if (i_this->mEffs[i].mpModel == nullptr) {
            return FALSE;
        }
    }
    return TRUE;
}
VERIFY(0x02074330, useHeapInit);

/* 020744C0: bdkobj_class::bdkobj_class (HD: out of line; allocates when this == NULL) */
static bdkobj_class* bdkobj_class_ct(bdkobj_class* i_this) {
    WWHD_FUNC(0x020744C0, bdkobj_class*, i_this);
    if (i_this == nullptr) {
        i_this = (bdkobj_class*)operator_new(0xAA4);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = BDKOBJ_VTBL;
    construct_array(gabi::ea(&i_this->mEffs[0]), 3, 0x1B8, 0x020748EC);
    dCcD_Stts_ct(&i_this->mStts);
    dCcD_Cyl_ct(&i_this->mCyl, CM3DGAAB_VTBL);
    dPa_smokeEcallBack_ct1(m918_ea(i_this), 1);
    return i_this;
}
VERIFY(0x020744C0, bdkobj_class_ct);

/* 020745AC */
static cPhs_State daBdkobj_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x020745AC, cPhs_State, a_this);
    bdkobj_class* i_this = (bdkobj_class*)a_this;
    /* fopAcM_ct(a_this, bdkobj_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr)
            bdkobj_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State res = dComIfG_resLoad(&i_this->mPhase, STR(0x10008580) /* "Bdkobj" */);
    if (res == cPhs_ERROR_e) {
        return cPhs_ERROR_e;
    }
    if (res != cPhs_COMPLEATE_e) {
        return res;
    }
    gabi::store<u8>(m918_ea(i_this) + 0x12, 1); /* m918.setFollowOff() */
    u8 prm = fopAcM_GetParam(i_this) & 0xFF;
    i_this->m298 = (prm == 0xFF) ? 0 : prm;

    if (!fopAcM_entrySolidHeap(i_this, 0x02074330 /* useHeapInit */, 0x5000)) {
        return cPhs_ERROR_e;
    }

    fopAcM_SetMin_l(i_this, -500.0f, -1500.0f, -500.0f);
    fopAcM_SetMax_l(i_this, 500.0f, 1500.0f, 500.0f);
    i_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(actor_model(i_this))); /* fopAcM_SetMtx */

    if (i_this->m298 == 2) {
        if (dBgS_Regist(dComIfG_Bgsp(), i_this->pm_bgw, i_this)) {
            return cPhs_ERROR_e;
        } else {
            return cPhs_COMPLEATE_e;
        }
    }

    i_this->mStts.Init(0xFF, 0xFF, a_this);
    i_this->mCyl.Set(cc_cyl_src);
    i_this->mCyl.SetStts(&i_this->mStts);
    if (i_this->m298 == 0) {
        i_this->mCyl.SetH(REGF(0x1047B970) + 300.0f);
        i_this->mCyl.SetR(REGF(0x1047B974) + 200.0f);
    } else {
        i_this->mCyl.SetH(REGF(0x1047B978) + 300.0f);
        i_this->mCyl.SetR(REGF(0x1047B97C) + 200.0f);
    }

    for (s32 i = 0; i < 3; i++) {
        i_this->mEffs[i].mStts.Init(0xC8, 0xFF, a_this);
        i_this->mEffs[i].mSph.Set(hahen_sph_src);
        i_this->mEffs[i].mSph.SetStts(&i_this->mEffs[i].mStts);
    }

    return cPhs_COMPLEATE_e;
}
VERIFY(0x020745AC, daBdkobj_Create);

/* 0207481C: static initialisation (header statics at a different layout; non_pos) */
static void __sinit_d_a_bdkobj_cpp() {
    WWHD_FUNC(0x0207481C, void, (u32)0);
    for (int i = 0; i < 4; i++) gabi::store<u32>(0x10461978 + 4 * i, 0);
    __register_global_object(0x10190B78);
    gabi::store<f32>(0x10461960, -3.1415927f);
    gabi::store<f32>(0x10461964, 3.1415927f);
    gabi::call(0x028ED6F8, 0x10461974);
    __register_global_object(0x10190B84);
    gabi::call(0x028EAB2C, 0x10461975);
    __register_global_object(0x10190B90);
    non_pos->set(10000.0f, -10000.0f, 20000.0f);
}
VERIFY(0x0207481C, __sinit_d_a_bdkobj_cpp);

/* 020748D8: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(SafeString* s, s32 flags) {
    WWHD_FUNC(0x020748D8, void, s, flags);
    if (s != nullptr && (flags & 1))
        operator_delete(s);
}
VERIFY(0x020748D8, SafeString_dt);

/* 020748EC: bdo_eff_s::bdo_eff_s (array element constructor; allocates when this == NULL) */
static bdo_eff_s* bdo_eff_s_ct(bdo_eff_s* e) {
    WWHD_FUNC(0x020748EC, bdo_eff_s*, e);
    if (e == nullptr) {
        e = (bdo_eff_s*)operator_new(0x1B8);
        if (e == nullptr)
            return e;
    }
    dCcD_Stts_ct(&e->mStts);
    gabi::call(0x025166F0, &e->mSph); /* dCcD_Sph::dCcD_Sph */
    return e;
}
VERIFY(0x020748EC, bdo_eff_s_ct);

/* 0207495C: bdo_eff_s::~bdo_eff_s (array element destructor) */
static void bdo_eff_s_dt(bdo_eff_s* e, s32 flags) {
    WWHD_FUNC(0x0207495C, void, e, flags);
    if (e != nullptr) {
        gabi::call(0x02515AE8, &e->mSph, 2); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&e->mStts, 2);
        if (flags & 1)
            operator_delete(e);
    }
}
VERIFY(0x0207495C, bdo_eff_s_dt);

/* 020749BC: bdkobj_class deleting destructor (compiler-generated, HD virtual destructor) */
static void bdkobj_class_dt(bdkobj_class* i_this, s32 flags) {
    WWHD_FUNC(0x020749BC, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        destroy_arr(gabi::ea(&i_this->mEffs[0]), 3, 0x1B8, 0x0207495C);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x020749BC, bdkobj_class_dt);

/* 02074A48: sead::SafeString virtual at vtable +0x14 (assureTerminationImpl_, empty in this copy) */
static void SafeString_assureTermination(SafeString*) {
    WWHD_FUNC(0x02074A48, void, (u32)0);
}
VERIFY(0x02074A48, SafeString_assureTermination);
