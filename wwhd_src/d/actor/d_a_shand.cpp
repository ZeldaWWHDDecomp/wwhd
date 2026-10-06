/**
 * d_a_shand.cpp (WWHD)
 * Object - Forbidden Woods - Ceiling tentacle / 汎用触手 (general purpose tentacle)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_shand.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x10039F9C /* this TU's sead::SafeString vtable */
#define SHAND_VTBL 0x1003A0F4      /* shand_class vtable (HD virtual destructor) */
#define SHAND_HIO_VTBL 0x1003A0E4  /* daShand_HIO_c vtable */
#define CYL_AAB_VTBL 0x10039FB4    /* this TU's cM3dGAab vtable */
#define tg_cyl_src gabi::at<dCcD_SrcCyl>(0x101D02B0)
#define bm_sph_src gabi::at<dCcD_SrcSph>(0x101D0270)
#define hand_color gabi::at<GXColor>(0x101D0248) /* {0x50, 0x96, 0x96, 0xFF} */

/* this TU's line/ground check vtables */
static const dBgS_LinChk_vt l_linchk_vt = {0x1003A0A4, 0x1003A0B4, 0x1003A0D4, 0x1003A0C4};
static const dBgS_GndChk_vt l_gndchk_vt = {0x10039FE4, 0x10039FF4, 0x1003A014, 0x1003A004};
static const dBgS_GndChk_vt l_splchk_vt = {0x1003A064, 0x1003A074, 0x1003A094, 0x1003A084};

enum {
    dRes_INDEX_SHAND_BTI_SHAND_e = 3,
    dRes_INDEX_SHAND_BTI_VHLIF_VINE_e = 4,
};
enum { JA_SE_OBJ_VINE_S_RECOVER = 0x5868 }; /* HD id */
enum { ID_AK_JN_SIBOUBAKUEN = 0x13, ID_AK_JN_SIBOUFLASH = 0x16 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02518DB0 at_power_check(CcAtInfo*) -> attacking actor; 02518CC8 def_se_set(actor, cCcD_Obj*, mtrl) */
static inline u32 at_power_check(void* info) { return gabi::call<u32>(0x02518DB0, info); }
static inline void def_se_set(fopAc_ac_c* a, u32 obj, u32 mtrl) { gabi::call(0x02518CC8, a, obj, mtrl); }
static inline void __construct_array_l(void* p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void __destroy_arr_l(void* p, u32 n, u32 size, u32 dtor, u32 flags) { gabi::call(0x028F0164, p, n, size, dtor, flags); }
/* 02008DAC cBgS_Chk::~cBgS_Chk (ground checks), 02008B4C cBgS_LinChk::~cBgS_LinChk */
static inline void cBgS_GndChk_dt(void* c, s32 f) { gabi::call(0x02008DAC, c, f); }
static inline void cBgS_LinChk_dt_l(void* c, s32 f) { gabi::call(0x02008B4C, c, f); }

/* mDoExt_3DlineMat1_c (HD 0x188): vtable at +0x130, line array pointer at +0x184
 * (line 0: segment positions at +0, segment sizes (u8) at +4) */
struct mDoExt_3DlineMat1_l {
    /* 0x000 */ u8 _000[0x130];
    /* 0x130 */ be<u32> __vtbl;
    /* 0x134 */ u8 _134[0x184 - 0x134];
    /* 0x184 */ be<u32> mpLines;
};
WWHD_SIZE(mDoExt_3DlineMat1_l, 0x188);

struct shand_s {
    /* 0x00 */ cXyz mPos;
    /* 0x0C */ u8 _pad[0x18 - 0xC];
    /* 0x18 */ be<f32> field_18;
};
WWHD_SIZE(shand_s, 0x1C);

struct shand_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ be<s16> mExecuteCount;
    /* 0x3D2 */ be<s16> unused_2B6;
    /* 0x3D4 */ be<s16> mState;
    /* 0x3D6 */ be<s16> field_2BA;
    /* 0x3D8 */ be<s16> field_2BC[4];
    /* 0x3E0 */ be<s16> field_2C4;
    /* 0x3E2 */ u8 _3E2[2];
    /* 0x3E4 */ cXyz field_2C8;
    /* 0x3F0 */ cXyz field_2D4;
    /* 0x3FC */ u8 unused_2E0[0x40C - 0x3FC];
    /* 0x40C */ be<f32> field_2F0;
    /* 0x410 */ be<f32> field_2F4;
    /* 0x414 */ be<f32> field_2F8;
    /* 0x418 */ be<f32> field_2FC;
    /* 0x41C */ be<f32> field_300;
    /* 0x420 */ be<f32> field_304;
    /* 0x424 */ be<u32> field_308;
    /* 0x428 */ gptr<fopAc_ac_c> field_30C;
    /* 0x42C */ gptr<cXyz> field_310;
    /* 0x430 */ gptr<be<u8>> field_314;
    /* 0x434 */ be<s16> field_318;
    /* 0x436 */ u8 _436[2];
    /* 0x438 */ shand_s field_31C[20];
    /* 0x668 */ mDoExt_3DlineMat1_l mLineMat;
    /* 0x7F0 */ dCcD_Stts mStts;
    /* 0x82C */ dCcD_Sph mSph;
    /* 0x958 */ dCcD_Cyl mCylArr[5];
    /* 0xF48 */ be<f32> ground_y;
    /* 0xF4C */ be<u8> mHasHIO;
};
WWHD_OFFSET(shand_class, field_2F0, 0x40C);
WWHD_OFFSET(shand_class, field_31C, 0x438);
WWHD_OFFSET(shand_class, mStts, 0x7F0);
WWHD_OFFSET(shand_class, mCylArr, 0x958);
WWHD_OFFSET(shand_class, mHasHIO, 0xF4C);

/* HD: the vtable pointer follows the members (+8); size 0xC */
struct daShand_HIO_c {
    /* 0x0 */ be<s8> mNo;
    /* 0x1 */ u8 _1;
    /* 0x2 */ be<s16> field_6;
    /* 0x4 */ be<s16> field_8;
    /* 0x6 */ u8 _6[2];
    /* 0x8 */ be<u32> __vtbl;
};
WWHD_SIZE(daShand_HIO_c, 0xC);
#define l_HIO (*gabi::at<daShand_HIO_c>(0x1046DCB8))
#define hio_set (*gabi::at<be<u8>>(0x101D024C))

static inline u32 linePos(shand_class* i_this) { return gabi::load<u32>(i_this->mLineMat.mpLines + 0); }

/* cXyz copy through integer registers (GHS struct copy) */
static inline void xyz_copy(cXyz* dst, const cXyz* src) {
    gabi::store<u32>(gabi::ea(dst) + 0, gabi::load<u32>(gabi::ea(src) + 0));
    gabi::store<u32>(gabi::ea(dst) + 4, gabi::load<u32>(gabi::ea(src) + 4));
    gabi::store<u32>(gabi::ea(dst) + 8, gabi::load<u32>(gabi::ea(src) + 8));
}

/* 02471064: daShand_HIO_c::daShand_HIO_c (HD: allocates when this == NULL) */
static daShand_HIO_c* daShand_HIO_c_ct(daShand_HIO_c* i_this) {
    WWHD_FUNC(0x02471064, daShand_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daShand_HIO_c*)operator_new(0xC);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = SHAND_HIO_VTBL;
    i_this->mNo = -1;
    i_this->field_6 = -3;
    i_this->field_8 = 50;
    return i_this;
}
VERIFY(0x02471064, daShand_HIO_c_ct);

/* 0246FAEC (hand_draw inlined) */
static BOOL daShand_Draw(shand_class* i_this) {
    WWHD_FUNC(0x0246FAEC, BOOL, i_this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->eyePos, &i_this->tevStr);
    /* hand_draw: mLineMat.update(0x14, color, &tevStr); dComIfGd_set3DlineMat(&mLineMat) */
    gabi::call(0x025ED1BC, &i_this->mLineMat, 0x14, hand_color, &i_this->tevStr); /* mDoExt_3DlineMat1_c::update */
    u32 packets = dComIfGp_ea() + 0x5FB4;
    s32 matId = gabi::call_ptr<s32>(gabi::load<u32>(i_this->mLineMat.__vtbl + 0x14), &i_this->mLineMat);
    gabi::call(0x025EDD04, packets + matId * 0x9C, &i_this->mLineMat); /* mDoExt_3DlineMatSortPacket::setMat */
    return TRUE;
}
VERIFY(0x0246FAEC, daShand_Draw);

/* one link of the chain: rotate towards (x, y, z) and place `seg` at prev + offset */
static inline void chain_link(shand_s* seg, shand_s* prev, f32 x, f32 y, f32 z, cXyz* offset, cXyz* out, cXyz* sum) {
    s16 yAng = cM_atan2s(x, z);
    s16 xAng = -cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
    mDoMtx_YrotS(calc_mtx(), yAng);
    mDoMtx_XrotM(calc_mtx(), xAng);
    MtxPosition(offset, out);
    cXyz_pl(&prev->mPos, sum, out);
    xyz_copy(&seg->mPos, sum);
}

/* control1 (inlined). HD: the sway factor of the last links is computed with fnmsubs */
static inline void control1(shand_class* i_this) {
    xyz_copy(&i_this->field_31C[0].mPos, &i_this->current.pos);
    mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);

    gabi::Local<cXyz> local94, cStack_a0, localac, sum;
    local94->y = i_this->field_2F8;
    local94->x = 0.0f;
    local94->z = i_this->field_2FC;
    MtxPosition(local94, localac);
    cLib_addCalc2(&i_this->field_2F8, REG0_F(7), 1.0f, 0.1f);
    cLib_addCalc2(&i_this->field_2FC, REG0_F(8), 1.0f, 1.0f);
    local94->z = i_this->field_2F4;
    f32 fVar1 = i_this->field_300;
    shand_s* shand_i = &i_this->field_31C[1];
    for (s32 i = 1; i < 19; i++, shand_i++) {
        f32 idkx = cM_ssin(i_this->mExecuteCount * (REG0_S(5) + 1100) + i * (REG0_S(6) + 4000)) * fVar1;
        f32 idkz = cM_scos(i_this->mExecuteCount * (REG0_S(7) + 800) + i * (REG0_S(8) + 4000)) * fVar1;
        f32 fVar2 = (i < 15) ? 1.0f : gabi::fnmsubs((f32)(i - 15), 0.2f, 1.0f);

        f32 fVar_y = (shand_i->mPos.y - shand_i[-1].mPos.y) + localac->y;
        f32 fVar_x = gabi::fmadds(idkx, fVar2, gabi::fmadds(localac->x, fVar2, shand_i->mPos.x - shand_i[-1].mPos.x));
        f32 fVar_z = gabi::fmadds(idkz, fVar2, gabi::fmadds(localac->z, fVar2, shand_i->mPos.z - shand_i[-1].mPos.z));
        chain_link(shand_i, &shand_i[-1], fVar_x, fVar_y, fVar_z, local94, cStack_a0, sum);
    }
}

/* control2 (inlined) */
static inline void control2(shand_class* i_this) {
    gabi::Local<cXyz> rel_offset, abs_offset, sum;
    rel_offset->x = 0.0f;
    rel_offset->y = 0.0f;
    rel_offset->z = i_this->field_2F4;

    cLib_addCalc2(&i_this->field_31C[19].mPos.x, i_this->field_2D4.x, 1.0f, i_this->field_2F0 * 50.0f);
    cLib_addCalc2(&i_this->field_31C[19].mPos.y, i_this->field_2D4.y, 1.0f, i_this->field_2F0 * 50.0f);
    cLib_addCalc2(&i_this->field_31C[19].mPos.z, i_this->field_2D4.z, 1.0f, i_this->field_2F0 * 50.0f);
    cLib_addCalc2(&i_this->field_2F0, 1.0f, 1.0f, 0.01f);

    shand_s* shand_i = &i_this->field_31C[18];
    for (s32 i = 18; i >= 1; i--, shand_i--) {
        f32 dx = shand_i->mPos.x - shand_i[1].mPos.x;
        f32 dz = shand_i->mPos.z - shand_i[1].mPos.z;
        f32 dy = shand_i->mPos.y - shand_i[1].mPos.y;
        chain_link(shand_i, &shand_i[1], dx, dy, dz, rel_offset, abs_offset, sum);
    }
}

/* control3 / cut_control3 (inlined): the thickness of each link */
static inline void set_thickness(shand_class* i_this, f32 step) {
    shand_s* shand_i = i_this->field_31C;
    for (s32 counter = 0; counter < 20; counter++, shand_i++) {
        if (counter < 12) {
            shand_i->field_18 = i_this->field_304;
        } else {
            shand_i->field_18 = i_this->field_304 * gabi::fnmsubs((f32)(counter - 10), step, 1.0f);
        }
    }
}

/* normal (inlined) */
static inline void normal(shand_class* i_this) {
    fopAc_ac_c* actor = i_this;

    if (i_this->field_318 != 0) {
        gabi::Local<dBgS_LinChk> local94;
        dBgS_LinChk_ct(local94.get(), l_linchk_vt, false);
        gabi::Local<cXyz> chk_start, chk_end;
        f32 sy = actor->current.pos.y;
        chk_start->x = actor->current.pos.x;
        chk_start->y = sy;
        chk_start->z = actor->current.pos.z;
        chk_end->x = actor->current.pos.x;
        f32 ey = actor->current.pos.y;
        chk_end->y = ey;
        chk_end->z = actor->current.pos.z;
        chk_start->y = sy + 50.0f;
        chk_end->y = ey + 4000.0f;
        dBgS_LinChk_Set(local94.get(), chk_start, chk_end, actor);
        if (cBgS_LineCross(dComIfG_Bgsp(), local94.get())) {
            u32 cross = gabi::ea(local94.get()) + 0x30; /* GetCross() */
            f32 x = gabi::load<f32>(cross + 0), y = gabi::load<f32>(cross + 4), z = gabi::load<f32>(cross + 8);
            i_this->field_2C8.x = x;
            i_this->field_2D4.x = x;
            i_this->field_2C8.y = y;
            i_this->field_2C8.z = z;
            i_this->field_2D4.y = y;
            i_this->field_2D4.z = z;
            i_this->field_31C[19].mPos.x = x;
            i_this->field_31C[19].mPos.y = y;
            i_this->field_31C[19].mPos.z = z;
        }
        i_this->field_318 -= 1;
        /* ~dBgS_LinChk (inline): this TU's vtables, then cBgS_LinChk::~cBgS_LinChk */
        u32 b = gabi::ea(local94.get());
        gabi::store<u32>(b + 0x64, 0x10039FD4);
        gabi::store<u32>(b + 0x58, l_linchk_vt.v58);
        gabi::store<u32>(b + 0x20, 0x10039FC4);
        cBgS_LinChk_dt_l(local94.get(), 0);
    }

    switch ((u16)i_this->field_2BA) {
    case 0:
        if (std::fabs(i_this->field_31C[19].mPos.y - i_this->field_2D4.y) < 10.0f) {
            i_this->field_2BA = 1;
            *i_this->field_314 = 2;
        }
    case 1:
        xyz_copy(&i_this->field_2D4, &i_this->field_2C8);
        break;
    }

    f32 y_diff = std::fabs(actor->home.pos.y - i_this->field_2C8.y);
    cLib_addCalc2(&i_this->field_2F4, y_diff * (REG_F(14, 11) + 0.05f), 0.1f, 1.0f);
    cLib_addCalc2(&i_this->field_300, REG_F(14, 12) + 10.0f, 0.1f, 0.5f);
    control1(i_this);
    control2(i_this);
    set_thickness(i_this, 0.05f);
}

/* cut_control (inlined) */
static inline void cut_control(shand_class* i_this) {
    xyz_copy(&i_this->field_31C[0].mPos, &i_this->current.pos);
    mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);

    gabi::Local<cXyz> local_b8, cStack_c4, local_d0, sum;
    local_b8->x = 0.0f;
    local_b8->y = i_this->field_2F8;
    local_b8->z = i_this->field_2FC;
    MtxPosition(local_b8, local_d0);
    cLib_addCalc2(&i_this->field_2F8, REG_F(14, 7) + -5.0f, 1.0f, REG_F(12, 4) + 0.1f);
    cLib_addCalc2(&i_this->field_2FC, REG_F(14, 8) + 20.0f, 1.0f, REG_F(14, 5) + 0.2f);
    cLib_addCalc0(&i_this->field_300, 1.0f, REG_F(12, 6) + 1.0f);
    local_b8->z = i_this->field_2F4;

    shand_s* shand_i = &i_this->field_31C[1];
    for (s32 i = 1; i < 20; i++, shand_i++) {
        f32 f300 = i_this->field_300;
        s32 cnt = i_this->mExecuteCount;
        f32 e8x = cM_ssin(cnt * (REG0_S(4) + 3500) + i * (REG0_S(5) + 4000));
        f32 e8y = cM_scos(cnt * (REG0_S(6) + 4000) + i * (REG0_S(7) + 4000));
        f32 e8z = cM_scos(cnt * (REG0_S(8) + 3800) + i * (REG0_S(9) + 4000)) * f300;
        f32 factor = gabi::fnmsubs((f32)i, REG0_F(9) + 0.03763158f, 1.0f);

        f32 fVar_y = gabi::fmadds(e8y, f300, shand_i->mPos.y + local_d0->y);
        f32 fVar_x = gabi::fmadds(e8x, f300, gabi::fmadds(local_d0->x, factor, shand_i->mPos.x - shand_i[-1].mPos.x));
        if (fVar_y < i_this->ground_y)
            fVar_y = i_this->ground_y;
        f32 fVar_z = ((shand_i->mPos.z - shand_i[-1].mPos.z) + local_d0->z * factor) + e8z;
        f32 delta_y = fVar_y - shand_i[-1].mPos.y;
        chain_link(shand_i, &shand_i[-1], fVar_x, delta_y, fVar_z, local_b8, cStack_c4, sum);
    }
}

/* cut (inlined) */
static inline void cut(shand_class* i_this) {
    cLib_addCalc2(&i_this->field_2F4, REG_F(8, 12) + 20.0f, 0.1f, 0.5f);
    cut_control(i_this);
    set_thickness(i_this, 0.08f);
    if (i_this->field_2BC[0] == 0 && i_this->field_30C->health != 0) {
        i_this->field_2F0 = 0.0f;
        i_this->mState = 0;
        i_this->field_2BA = 0;
        i_this->field_300 = 0.0f;
        fopAcM_seStart(i_this, JA_SE_OBJ_VINE_S_RECOVER, 0);
    }
}

/* ground checks of the cut state (inlined) */
static inline void cut_ground(shand_class* i_this) {
    fopAc_ac_c* actor = i_this;
    if ((fopAcM_GetParam(actor) & 0xFF) != 1) {
        gabi::Local<dBgS_GndChk> local_ac;
        dBgS_GndChk_ct(local_ac.get(), l_gndchk_vt, false);
        u32 a = gabi::ea(local_ac.get());
        f32 z = actor->current.pos.z;
        f32 y = actor->current.pos.y;
        f32 x = actor->current.pos.x;
        gabi::store<f32>(a + 0x2C, z);
        gabi::store<f32>(a + 0x24, x);
        gabi::store<f32>(a + 0x28, y - 100.0f);
        i_this->ground_y = cBgS_GroundCross(dComIfG_Bgsp(), local_ac.get());

        gabi::Local<dBgS_GndChk> local_100; /* dBgS_ObjGndChk_Spl */
        dBgS_GndChk_ct(local_100.get(), l_splchk_vt, true);
        u32 s = gabi::ea(local_100.get());
        gabi::store<u32>(s + 0x50, 0xE); /* Spl: the special-material groups */
        z = actor->current.pos.z;
        y = actor->current.pos.y;
        x = actor->current.pos.x;
        gabi::store<f32>(s + 0x2C, z);
        gabi::store<f32>(s + 0x24, x);
        gabi::store<f32>(s + 0x28, y + 200.0f);
        f32 spl_ground_y = cBgS_GroundCross(dComIfG_Bgsp(), local_100.get()) + 10.0f;
        if (spl_ground_y != -1000000000.0f) {
            i_this->ground_y = spl_ground_y;
        }
        /* ~dBgS_ObjGndChk_Spl, ~dBgS_GndChk (inline): this TU's vtables, then cBgS_Chk::~cBgS_Chk */
        gabi::store<u32>(s + 0x20, l_gndchk_vt.v20);
        gabi::store<u32>(s + 0x40, l_gndchk_vt.v40);
        gabi::store<u32>(s + 0x4C, 0x10039FD4);
        cBgS_GndChk_dt(local_100.get(), 0);
        gabi::store<u32>(a + 0x40, l_gndchk_vt.v40);
        gabi::store<u32>(a + 0x4C, 0x10039FD4);
        gabi::store<u32>(a + 0x20, l_gndchk_vt.v20);
        cBgS_GndChk_dt(local_ac.get(), 0);
    } else {
        i_this->ground_y = -1000000000.0f;
    }
    i_this->mState = 2;
}

/* hand_move (inlined) */
static inline void hand_move(shand_class* i_this) {
    fopAc_ac_c* actor = i_this;

    i_this->field_30C = fopAcM_SearchByID(i_this->field_308);
    if (i_this->field_30C != NULL) {
        cXyz* src = i_this->field_310;
        xyz_copy(&actor->current.pos, src);
        actor->current.angle.y = actor->home.angle.y + i_this->field_30C->shape_angle.y + REG_S(14, 3);
        switch ((u16)i_this->mState) {
        case 0:
            normal(i_this);
            gabi::store<u32>(gabi::ea(actor) + 0x39C, 1); /* attention_info.flags = fopAc_Attn_LOCKON_MISC_e */
            if (i_this->field_30C->health == 0) {
                i_this->mState = 1;
                i_this->field_2BA = 0;
            }
            break;
        case 1:
            cut_ground(i_this);
        case 2:
            cut(i_this);
            gabi::store<u32>(gabi::ea(actor) + 0x39C, 0); /* attention_info.flags */
            break;
        }
    }

    {
        u32 lines = i_this->mLineMat.mpLines;
        u32 line_data = gabi::load<u32>(lines + 0);
        u32 line_size = gabi::load<u32>(lines + 4);
        shand_s* shand_i = i_this->field_31C;
        for (s32 i = 20; i != 0; i--) {
            xyz_copy(gabi::at<cXyz>(line_data), &shand_i->mPos);
            gabi::store<u8>(line_size, (u8)gabi::ftoi(shand_i->field_18));
            shand_i++;
            line_data += 0xC;
            line_size += 1;
        }
    }

    s16 seg = l_HIO.field_6;
    u32 line_segments = linePos(i_this);
    cXyz* eye = gabi::at<cXyz>(line_segments + seg * 0xC + 10 * 0xC);
    u32 ex = gabi::load<u32>(gabi::ea(eye) + 0), ey = gabi::load<u32>(gabi::ea(eye) + 4), ez = gabi::load<u32>(gabi::ea(eye) + 8);
    gabi::store<u32>(gabi::ea(&actor->eyePos) + 0, ex);
    gabi::store<u32>(gabi::ea(&actor->eyePos) + 4, ey);
    gabi::store<u32>(gabi::ea(&actor->eyePos) + 8, ez);
    gabi::store<u32>(gabi::ea(actor) + 0x390, ex); /* attention_info.position */
    gabi::store<u32>(gabi::ea(actor) + 0x394, ey);
    gabi::store<u32>(gabi::ea(actor) + 0x398, ez);

    bool is_hit = false;
    gabi::Local<u8[0x1C]> hit_atInfo; /* CcAtInfo: mpObj +0, mpActor +4, pParticlePos +0x14 */
    u32 info = gabi::ea(hit_atInfo.get());
    gabi::store<u32>(info + 0x14, 0);
    gabi::Local<cXyz> center;
    if (i_this->field_2BC[1] == 0 && i_this->mState == 0) {
        i_this->mSph.SetC(&actor->eyePos);
        i_this->mCylArr[0].SetC(&actor->current.pos);
        for (s32 i = 0; i < 5; i++) {
            if (i_this->field_2C4 == 0 && i_this->mCylArr[i].ChkTgHit() != 0) {
                gabi::store<u32>(info + 0, gabi::ea(i_this->mCylArr[i].GetTgHitObj()));
                gabi::store<u32>(info + 0x14, gabi::ea(i_this->mCylArr[i].GetTgHitPosP()));
                is_hit = true;
                break;
            }
            if (i > 0) {
                s32 seg_idx = ((i_this->mExecuteCount & 3) + (i * 4)) % 20;
                u32 p = line_segments + seg_idx * 0xC;
                center->x = gabi::load<f32>(p + 0);
                f32 cy = gabi::load<f32>(p + 4);
                center->y = cy;
                center->z = gabi::load<f32>(p + 8);
                center->y = cy - 200.0f;
                i_this->mCylArr[i].SetC(center);
            }
        }
    } else {
        center->x = 0.0f;
        center->z = 0.0f;
        center->y = -20000.0f;
        i_this->mSph.SetC(center);
        for (s32 i = 0; i < 5; i++) {
            i_this->mCylArr[i].SetC(center);
        }
    }

    dComIfG_Ccsp_Set(&i_this->mSph);
    for (s32 i = 0; i < 5; i++) {
        dComIfG_Ccsp_Set(&i_this->mCylArr[i]);
    }

    if (is_hit || (i_this->field_2C4 == 0 && i_this->mSph.ChkTgHit() != 0)) {
        if (!is_hit) {
            gabi::store<u32>(info + 0, gabi::ea(i_this->mSph.GetTgHitObj()));
            gabi::store<u32>(info + 0x14, gabi::ea(i_this->mSph.GetTgHitPosP()));
        }
        u32 hitActor = at_power_check(hit_atInfo.get());
        gabi::store<u32>(info + 4, hitActor);
        i_this->field_2C4 = 10;
        if (hitActor != 0) {
            def_se_set(actor, gabi::load<u32>(info + 0), 33);
            if (i_this->field_30C != NULL) {
                i_this->mState = 1;
                i_this->field_2BA = 0;
                i_this->field_2BC[0] = l_HIO.field_8;
                i_this->field_2BC[1] = l_HIO.field_8 + 90 + REG0_S(2);
                i_this->field_2F8 = 3.0f;
                i_this->field_2FC = 40.0f;
                i_this->field_300 = cM_rndF(20.0f) + 30.0f;
                *i_this->field_314 = 1;
                gabi::Local<cXyz> particle_scale;
                u32 eyeA = gabi::load<u32>(info + 4) + 0x37C;
                particle_scale->y = 0.5f;
                particle_scale->z = 0.5f;
                particle_scale->x = 0.5f;
                dComIfGp_particle_set(ID_AK_JN_SIBOUBAKUEN, gabi::at<cXyz>(eyeA), NULL, particle_scale);
                u32 eyeB = gabi::load<u32>(info + 4) + 0x37C;
                dComIfGp_particle_set(ID_AK_JN_SIBOUFLASH, gabi::at<cXyz>(eyeB), NULL, particle_scale);
            }
        }
    }
}

/* 0246FB78 */
static BOOL daShand_Execute(shand_class* i_this) {
    WWHD_FUNC(0x0246FB78, BOOL, i_this);
    dComIfGp_get(); /* unused accessor call */
    i_this->mExecuteCount += 1;

    for (s32 i = 4, j = 0; i != 0; i--, j++) {
        if (i_this->field_2BC[j] != 0)
            i_this->field_2BC[j] -= 1;
    }

    if (i_this->field_2C4 != 0)
        i_this->field_2C4 -= 1;

    hand_move(i_this);
    return TRUE;
}
VERIFY(0x0246FB78, daShand_Execute);

/* 02470D34 */
static BOOL daShand_IsDelete(shand_class*) {
    WWHD_FUNC(0x02470D34, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02470D34, daShand_IsDelete);

/* 02470D3C */
static BOOL daShand_Delete(shand_class* i_this) {
    WWHD_FUNC(0x02470D3C, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhs, STR(0x1003A160) /* "Shand" */);
    if (i_this->mHasHIO) {
        s8 no = l_HIO.mNo;
        hio_set = 0;
        mDoHIO_deleteChild(no);
    }
    return TRUE;
}
VERIFY(0x02470D3C, daShand_Delete);

/* 02470DA0 (also the solid heap callback: daShand_solidHeapCB folded into it) */
static BOOL useHeapInit(shand_class* i_this) {
    WWHD_FUNC(0x02470DA0, BOOL, i_this);
    s32 bti_idx;
    if ((fopAcM_GetParam(i_this) & 0xff) == 53) {
        bti_idx = dRes_INDEX_SHAND_BTI_VHLIF_VINE_e;
    } else {
        bti_idx = dRes_INDEX_SHAND_BTI_SHAND_e;
    }
    void* img = dComIfG_getObjectRes(STR(0x10039F94) /* "Shand" */, bti_idx, SAFESTRING_VTBL);
    /* mDoExt_3DlineMat1_c::init(1 line, 20 segments, img, TRUE) */
    if (gabi::call<BOOL>(0x025EBA58, &i_this->mLineMat, 1, 0x14, img, 1) == FALSE) {
        return FALSE;
    } else {
        return TRUE;
    }
}
VERIFY(0x02470DA0, useHeapInit);

/* 02470E24 */
static cPhs_State daShand_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02470E24, cPhs_State, i_this);
    shand_class* s_this = static_cast<shand_class*>(i_this);
    /* fopAcM_ct(i_this, shand_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = SHAND_VTBL;
            gabi::call(0x025EB82C, &s_this->mLineMat); /* mDoExt_3DlineMat1_c::mDoExt_3DlineMat1_c */
            dCcD_Stts_ct(&s_this->mStts);
            gabi::call(0x025166F0, &s_this->mSph); /* dCcD_Sph::dCcD_Sph */
            __construct_array_l(s_this->mCylArr, 5, 0x130, 0x02471170 /* dCcD_Cyl ctor (this TU) */);
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    cPhs_State ret = dComIfG_resLoad(&s_this->mPhs, STR(0x1003A174) /* "Shand" */);
    if (ret == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, 0x02470DA0 /* useHeapInit */, 0x1040) != false) {
            i_this->health = 2;
            s_this->mExecuteCount = (s16)gabi::ftoi(cM_rndF(10000.0f));
            s_this->field_318 = 10;
            s_this->field_2BA = 1;
            s_this->field_2F0 = 1.0f;
            if ((fopAcM_GetParam(i_this) & 0xff) == 1)
                s_this->field_304 = 15.75f;
            else
                s_this->field_304 = 10.5f;
            s_this->mStts.Init(0xff, 0xff, i_this);
            for (s32 i = 0; i < 5; i++) {
                s_this->mCylArr[i].Set(tg_cyl_src);
                s_this->mCylArr[i].SetStts(&s_this->mStts);
            }
            s_this->mSph.Set(bm_sph_src);
            s_this->mSph.SetStts(&s_this->mStts);
            s_this->field_2C4 = 30;
            for (s32 i = 0; i < 3; i++) {
                daShand_Execute(s_this);
            }
        } else {
            ret = cPhs_ERROR_e;
        }

        if (hio_set == 0) {
            hio_set = 1;
            s_this->mHasHIO = 1;
            l_HIO.mNo = mDoHIO_createChild(STR(0x1003A17C) /* "汎用触手" */, &l_HIO);
        }
    }
    return ret;
}
VERIFY(0x02470E24, daShand_Create);

/* 024710BC: static initialisation (header statics, l_HIO) */
static void __sinit_d_a_shand_cpp() {
    WWHD_FUNC(0x024710BC, void, (u32)0);
    sinit_header_statics(0x1046DC9C, 0x101D02F4);
    daShand_HIO_c_ct(&l_HIO);
}
VERIFY(0x024710BC, __sinit_d_a_shand_cpp);

/* 0247115C: sead::SafeString deleting destructor (this TU's vtable 0x10039F9C) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0247115C, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x0247115C, SafeString_dt);

/* 02471170: dCcD_Cyl::dCcD_Cyl (this TU's out-of-line copy for __construct_array; HD: allocates
 * when this == NULL) */
static dCcD_Cyl* dCcD_Cyl_ct_tu(dCcD_Cyl* c) {
    WWHD_FUNC(0x02471170, dCcD_Cyl*, c);
    if (c == nullptr) {
        c = (dCcD_Cyl*)operator_new(0x130);
        if (c == nullptr)
            return c;
    }
    dCcD_Cyl_ct(c, CYL_AAB_VTBL);
    return c;
}
VERIFY(0x02471170, dCcD_Cyl_ct_tu);

/* 024711FC: shand_class deleting destructor (compiler-generated, HD virtual destructor) */
static void shand_class_dt(shand_class* i_this, s32 flags) {
    WWHD_FUNC(0x024711FC, void, i_this, flags);
    if (i_this != nullptr) {
        __destroy_arr_l(i_this->mCylArr, 5, 0x130, 0x02515A70 /* dCcD_Cyl::~dCcD_Cyl */, 0);
        gabi::call(0x02515AE8, &i_this->mSph, 2); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025EB8B8, &i_this->mLineMat, 2); /* ~mDoExt_3DlineMat1_c (matcher: draw) */
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024711FC, shand_class_dt);

/* 02471294: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x02471294, void, (u32)0);
}
VERIFY(0x02471294, SafeString_assureTerminationImpl);
