/**
 * d_a_sss.cpp (WWHD)
 * Enemy - Dexivine (Forbidden Woods, Wind Temple).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_sss.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x1003D39C
#define SSS_VTBL 0x1003D404
#define non_pos gabi::at<cXyz>(0x1046DFC8)
#define tg_sph_src 0x101D0DA0
#define bm_sph_src 0x101D0DE0
static inline f32 size_d(s32 i) { return gabi::load<f32>(0x101D0D30 + 4 * i); }
static inline f32 g_d(s32 i) { return gabi::load<f32>(0x101D0D78 + 4 * i); }

enum {
    dRes_INDEX_SSS_BCK_SSS_HIRAKU_e = 5, dRes_INDEX_SSS_BCK_SSS_TOJIRU_e = 6,
    dRes_INDEX_SSS_BMD_SSS_HAND_e = 9, dRes_INDEX_SSS_BTI_SSS_e = 0xC,
};
enum {
    JA_SE_OBJ_SVINE_OUT_WATER = 0x58F8, JA_SE_OBJ_SVINE_OUT = 0x58F7, JA_SE_OBJ_SVINE_GRASP = 0x58F9,
    JA_SE_OBJ_ATK_VINE_MP_SUCK = 0x7050, JA_SE_LK_LAST_HIT = 0x2828, JA_SE_OBJ_SVINE_CRASH = 0x69B0,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* mDoExt_3DlineMat1_c (HD 0x188; GameCube 0x3C): vtable at +0x130, mpLines at +0x184; a line is
 * {cXyz* mpSegments; u8* mpSize; ...}. Constructor 025EB82C, destructor 025EB8B8 (the matcher
 * names it mDoExt_3DlineMat1_c::draw), init 025EBA58, update 025ED1BC. */
struct mDoExt_3DlineMat1_l { u8 _[0x188]; };
static inline u32 lineMat_lines(mDoExt_3DlineMat1_l* l) { return gabi::load<u32>(gabi::ea(l) + 0x184); }
static inline BOOL mDoExt_3DlineMat1_init(mDoExt_3DlineMat1_l* l, u16 numLines, u16 numSegs, void* tex, BOOL hasSize) {
    return gabi::call<BOOL>(0x025EBA58, l, numLines, numSegs, tex, hasSize);
}
static inline void mDoExt_3DlineMat1_update(mDoExt_3DlineMat1_l* l, s32 segs, u32 color, dKy_tevstr_c* tev) {
    gabi::call(0x025ED1BC, l, segs, color, tev);
}
/* dComIfGd_set3DlineMat (HD): the sort packet play+0x5FB4 + getMaterialID() * 0x9C */
static inline void dComIfGd_set3DlineMat(mDoExt_3DlineMat1_l* l) {
    u32 pkt = dComIfGp_ea() + 0x5FB4;
    s32 id = gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(gabi::ea(l) + 0x130) + 0x14), l);
    gabi::call(0x025EDD04, pkt + id * 0x9C, l);
}
/* 025E4A98 mDoExt_McaMorf::setAnm / 025E54D8 updateDL / 025E535C play */
static inline void McaMorf_setAnm(u32 m, void* anm, s32 mode, f32 morf, f32 speed, f32 start, f32 end, void* snd) {
    gabi::call(0x025E4A98, m, anm, mode, morf, speed, start, end, snd);
}
static inline u32 McaMorf_getModel(u32 m) { return gabi::load<u32>(m + 0x90); }
/* 0201AD78 / 0201ADE0 cXyz operator+ / - (this, result, arg) */
static inline void XyzPl(const cXyz* a, cXyz* r, const cXyz* b) { cXyz_pl(a, r, b); }
/* JPABaseEmitter (HD): becomeInvalidEmitter (+0x5C = -1, flags +0x254 |= 1); setGlobalTranslation
 * negates y when the byte +0x262 is >= 7; setGlobalRotation: 028245AC JPAGetXYZRotateMtx(x, y, z, +0x1F0) */
static inline void JPA_becomeInvalidEmitter(u32 e) {
    u32 f = gabi::load<u32>(e + 0x254);
    gabi::store<u32>(e + 0x5C, 0xFFFFFFFF);
    gabi::store<u32>(e + 0x254, f | 1);
}
static inline void JPA_setGlobalTranslation(u32 e, f32 x, f32 y, f32 z) {
    if (gabi::load<u8>(e + 0x262) >= 7)
        y = -y;
    gabi::store<f32>(e + 0x22C, x);
    gabi::store<f32>(e + 0x230, y);
    gabi::store<f32>(e + 0x234, z);
}
static inline void JPAGetXYZRotateMtx(s16 x, s16 y, s16 z, u32 m) { gabi::call(0x028245AC, x, y, z, m); }
static inline void MtxPull_() { gabi::call(0x0200FD38); }
static inline s32 at_power_check(void* info) { return gabi::call<s32>(0x02518DB0, info); }
/* fopAcM_seStart (HD inline, `this` known non-null): eyePos address check only */
#define SE_START(a, id)                                                                         \
    do {                                                                                        \
        if (gabi::ea(&(a)->eyePos) != 0) {                                                      \
            s32 rev_ = dComIfGp_getReverb((a)->current.roomNo);                                 \
            mDoAud_seStart(id, &(a)->eyePos, 0, rev_);                                          \
        }                                                                                       \
    } while (0)

struct sss_s {
    /* 0x00 */ cXyz pos;
    /* 0x0C */ u8 _0C[0xC];
    /* 0x18 */ be<f32> field_0x18;
};
WWHD_SIZE(sss_s, 0x1C);

struct sss_class : fopAc_ac_c {
    /* 0x3AC */ u8 _3AC[0x3C8 - 0x3AC];
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ be<u32> mpMorf;
    /* 0x3D4 */ be<u8> field_0x2B8;
    /* 0x3D5 */ be<u8> field_0x2B9;
    /* 0x3D6 */ be<u8> field_0x2BA;
    /* 0x3D7 */ u8 _3D7;
    /* 0x3D8 */ be<s16> field_0x2BC;
    /* 0x3DA */ u8 _3DA[2];
    /* 0x3DC */ be<s16> field_0x2C0;
    /* 0x3DE */ be<s16> field_0x2C2[3];
    /* 0x3E4 */ cXyz field_0x2C8;
    /* 0x3F0 */ cXyz field_0x2D4;
    /* 0x3FC */ be<s16> field_0x2E0;
    /* 0x3FE */ be<s16> field_0x2E2;
    /* 0x400 */ u8 _400[4];
    /* 0x404 */ be<f32> field_0x2E8;
    /* 0x408 */ u8 _408[4];
    /* 0x40C */ be<f32> field_0x2F0;
    /* 0x410 */ be<f32> field_0x2F4;
    /* 0x414 */ be<f32> field_0x2F8;
    /* 0x418 */ be<f32> field_0x2FC;
    /* 0x41C */ mDoExt_3DlineMat1_l field_0x300;
    /* 0x5A4 */ sss_s field_0x33C[10];
    /* 0x6BC */ mDoExt_3DlineMat1_l field_0x454;
    /* 0x844 */ sss_s field_0x490[5];
    /* 0x8D0 */ dCcD_Stts mStts;
    /* 0x90C */ dCcD_Sph field_0x558[3];
    /* 0xC90 */ dCcD_Sph field_0x8DC;
    /* 0xDBC */ be<f32> field_0xA08;
    /* 0xDC0 */ be<s16> field_0xA0C;
    /* 0xDC2 */ u8 _DC2[2];
    /* 0xDC4 */ be<u32> mpEmitter1;
    /* 0xDC8 */ be<u32> mpEmitter2;
    /* 0xDCC */ be<u8> field_0xA18;
    /* 0xDCD */ u8 _DCD[3];
};
WWHD_OFFSET(sss_class, field_0x33C, 0x5A4);
WWHD_OFFSET(sss_class, field_0x490, 0x844);
WWHD_OFFSET(sss_class, field_0x558, 0x90C);
WWHD_OFFSET(sss_class, mpEmitter1, 0xDC4);
WWHD_SIZE(sss_class, 0xDD0);

/* 0248EC3C: daSss_Draw (hand_draw inlined) */
static BOOL daSss_Draw(sss_class* i_this) {
    WWHD_FUNC(0x0248EC3C, BOOL, i_this);
    if (i_this->field_0x2B8 != 0) {
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->eyePos, &i_this->tevStr);
        /* hand_draw */
        J3DModel* model = gabi::at<J3DModel>(McaMorf_getModel(i_this->mpMorf));
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, model, &i_this->tevStr);
        gabi::call(0x025E54D8, (u32)i_this->mpMorf); /* updateDL */
        mDoExt_3DlineMat1_update(&i_this->field_0x300, 10, 0x101D0D28 /* {FF,FF,FF,FF} */, &i_this->tevStr);
        dComIfGd_set3DlineMat(&i_this->field_0x300);
        if (i_this->field_0x2FC > 0.1f) {
            mDoExt_3DlineMat1_update(&i_this->field_0x454, 5, 0x101D0D2C, &i_this->tevStr);
            dComIfGd_set3DlineMat(&i_this->field_0x454);
        }
    }
    return TRUE;
}
VERIFY(0x0248EC3C, daSss_Draw);

/* 0248ED4C */
static void hand_open(sss_class* i_this) {
    WWHD_FUNC(0x0248ED4C, void, i_this);
    void* anm = dComIfG_getObjectRes(STR(0x1003D424) /* "Sss" */, dRes_INDEX_SSS_BCK_SSS_HIRAKU_e, SAFESTRING_VTBL);
    McaMorf_setAnm(i_this->mpMorf, anm, 0, 1.0f, 1.0f, 0.0f, -1.0f, nullptr);
}
VERIFY(0x0248ED4C, hand_open);

/* 0248EDD0: HD keeps the debug register REG12_S(1) (0x1047BD4A) */
static void hand_mtx_set(sss_class* i_this) {
    WWHD_FUNC(0x0248EDD0, void, i_this);
    MtxTrans(i_this->field_0x2D4.x, i_this->field_0x2D4.y, i_this->field_0x2D4.z, false);
    mDoMtx_XrotM(calc_mtx(), i_this->field_0x2E0);
    mDoMtx_YrotM(calc_mtx(), i_this->field_0x2E2);
    s16 reg = gabi::load<s16>(0x1047BD4A);
    mDoMtx_XrotM(calc_mtx(), (s16)(reg - 0x4000));
    MtxScale(0.5f, 0.2f, 0.5f, true);
    MtxTrans(0.0f, -130.0f, 0.0f, true);
    u32 model = McaMorf_getModel(i_this->mpMorf);
    mtx_copy(gabi::at<Mtx34>(model + 0xC8), calc_mtx());
}
VERIFY(0x0248EDD0, hand_mtx_set);

/* ---- hand_move's inlined helpers ---- */
static void control1(sss_class* i_this) {
    i_this->field_0x33C[0].pos.copy(i_this->current.pos);
    mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);
    mDoMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    gabi::Local<cXyz> sp3C, sp30, sp24, sp18, tmp;
    sp3C->y = 0.0f;
    sp3C->z = i_this->field_0x2F0;
    sp3C->x = 0.0f;
    MtxPosition(sp3C, sp24);
    sp3C->z = i_this->field_0x2E8;
    f32 f27 = i_this->field_0x2F4;
    for (s32 i = 1; i < 9; i++) {
        s32 t = i_this->field_0x2BC;
        sp30->x = cM_ssin(t * 1100 + i * 4000) * f27;
        sp30->y = g_d(i);
        sp30->z = cM_scos(t * 800 + i * 4000) * f27;
        MtxPosition(sp30, sp18);
        sss_s* cur = &i_this->field_0x33C[i];
        sss_s* prev = &i_this->field_0x33C[i - 1];
        f32 x = ((cur->pos.x - prev->pos.x) + sp24->x) + sp18->x;
        f32 y = ((cur->pos.y - prev->pos.y) + sp24->y) + sp18->y;
        f32 z = ((cur->pos.z - prev->pos.z) + sp24->z) + sp18->z;
        s16 xz = cM_atan2s(x, z);
        s16 ya = (s16)-cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
        MtxPush();
        mDoMtx_YrotS(calc_mtx(), xz);
        mDoMtx_XrotM(calc_mtx(), ya);
        MtxPosition(sp3C, sp30);
        MtxPull_();
        XyzPl(&prev->pos, tmp, sp30);
        cur->pos.copy(*tmp.get());
    }
}

static void control2(sss_class* i_this) {
    i_this->field_0x33C[9].pos.copy(i_this->field_0x2C8);
    gabi::Local<cXyz> sp34, sp28, tmp;
    f32 e0 = i_this->field_0x2E8;
    sp34->y = 0.0f;
    sp34->x = 0.0f;
    sp34->z = e0;
    for (s32 i = 8; i >= 1; i--) {
        sss_s* cur = &i_this->field_0x33C[i];
        sss_s* next = &i_this->field_0x33C[i + 1];
        f32 x = cur->pos.x - next->pos.x;
        f32 z = cur->pos.z - next->pos.z;
        f32 y = cur->pos.y - next->pos.y;
        s16 r27 = cM_atan2s(x, z);
        s16 r28 = (s16)-cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
        mDoMtx_YrotS(calc_mtx(), r27);
        mDoMtx_XrotM(calc_mtx(), r28);
        f32 e = i_this->field_0x2E8;
        if (i == 8) {
            f32 zz = e - 10.0f;
            if (zz < 0.0f)
                zz = 0.0f;
            sp34->z = zz;
        } else {
            sp34->z = e;
        }
        MtxPosition(sp34, sp28);
        XyzPl(&next->pos, tmp, sp28);
        cur->pos.copy(*tmp.get());
    }
    i_this->field_0x2D4.copy(i_this->field_0x33C[9].pos);
    cXyz_mi(&i_this->field_0x33C[8].pos, tmp, &i_this->field_0x33C[9].pos);
    f32 x = tmp->x, y = tmp->y, z = tmp->z;
    sp34->x = x;
    sp34->y = y;
    sp34->z = z;
    i_this->field_0x2E0 = (s16)-cM_atan2s(y, z);
    i_this->field_0x2E2 = cM_atan2s(sp34->x, std_sqrtf(gabi::fmadds(sp34->y, sp34->y, sp34->z * sp34->z)));
    hand_mtx_set(i_this);
}

/* HD: the phase terms are subtracted (GameCube adds them) */
static void cut_control1(sss_class* i_this) {
    i_this->field_0x490[0].pos.copy(i_this->current.pos);
    mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);
    mDoMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    gabi::Local<cXyz> sp50, sp44, sp38, tmp;
    f32 z0 = i_this->field_0x2FC;
    sp50->y = 0.0f; /* HD: x and y are zeroed */
    sp50->x = 0.0f;
    sp50->z = z0;
    for (s32 i = 1; i < 5; i++) {
        s32 t = i_this->field_0x2BC;
        sp44->x = cM_ssin(t * 4100 - i * 10000) * 50.0f;
        sp44->y = 50.0f;
        sp44->z = cM_scos(t * 4400 - i * 10000) * 50.0f;
        MtxPosition(sp44, sp38);
        sss_s* cur = &i_this->field_0x490[i];
        sss_s* prev = &i_this->field_0x490[i - 1];
        f32 x = (cur->pos.x - prev->pos.x) + sp38->x;
        f32 y = (cur->pos.y - prev->pos.y) + sp38->y;
        f32 z = (cur->pos.z - prev->pos.z) + sp38->z;
        s16 xz = cM_atan2s(x, z);
        s16 ya = (s16)-cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
        MtxPush();
        mDoMtx_YrotS(calc_mtx(), xz);
        mDoMtx_XrotM(calc_mtx(), ya);
        MtxPosition(sp50, sp44);
        MtxPull_();
        XyzPl(&prev->pos, tmp, sp44);
        cur->pos.copy(*tmp.get());
    }
    if (i_this->mpEmitter1 == 0) {
        if ((s8)i_this->field_0xA18 == 0)
            return;
        JPABaseEmitter* e = dComIfGp_particle_set(0x8184 /* ID_IT_SN_TSURU_TAIEKI00 */, &i_this->field_0x490[4].pos);
        i_this->mpEmitter1 = gabi::ea(e);
        i_this->field_0x2C2[1] = 60;
        if (e == nullptr)
            return;
    }
    cXyz_mi(&i_this->field_0x490[4].pos, tmp, &i_this->field_0x490[3].pos);
    f32 x = tmp->x, y = tmp->y, z = tmp->z;
    sp50->x = x;
    sp50->y = y;
    sp50->z = z;
    s16 rot_y = cM_atan2s(x, z);
    s16 rot_x = (s16)-cM_atan2s(sp50->y, std_sqrtf(gabi::fmadds(sp50->x, sp50->x, sp50->z * sp50->z)));
    u32 e = i_this->mpEmitter1;
    f32 tx = i_this->field_0x490[4].pos.x, ty = i_this->field_0x490[4].pos.y, tz = i_this->field_0x490[4].pos.z;
    JPA_setGlobalTranslation(e, tx, ty, tz);
    JPAGetXYZRotateMtx(rot_x, rot_y, 0, (u32)i_this->mpEmitter1 + 0x1F0);
    if (i_this->field_0x2C2[1] == 1) {
        JPA_becomeInvalidEmitter(i_this->mpEmitter1);
        i_this->mpEmitter1 = 0;
    }
}

static void cut_control2(sss_class* i_this) {
    gabi::Local<cXyz> sp60, sp54, tmp;
    sp60->x = 0.0f;
    i_this->field_0x33C[9].pos.copy(i_this->field_0x2C8);
    f32 f27 = i_this->field_0x2F4;
    sp60->z = i_this->field_0x2E8;
    sp60->y = 0.0f;
    for (s32 i = 8; i >= 0; i--) {
        s32 t = i_this->field_0x2BC;
        sss_s* cur = &i_this->field_0x33C[i];
        sss_s* next = &i_this->field_0x33C[i + 1];
        f32 sA = cM_ssin(t * 2500 - i * 3000);
        f32 sB = cM_ssin(t * 2950 - i * 4000);
        f32 cC = cM_scos(t * 2800 - i * 3500);
        f32 x_val = gabi::fmadds(sA, f27, cur->pos.x - next->pos.x);
        f32 y_val = gabi::fmadds(sB, f27, cur->pos.y - 10.0f);
        f32 f0 = i_this->field_0x2F8 + 5.0f;
        if (y_val < f0)
            y_val = f0;
        f32 z_val = (cur->pos.z - next->pos.z) + cC * f27;
        f32 f28 = y_val - next->pos.y;
        s16 xz = cM_atan2s(x_val, z_val);
        s16 ya = (s16)-cM_atan2s(f28, std_sqrtf(gabi::fmadds(x_val, x_val, z_val * z_val)));
        mDoMtx_YrotS(calc_mtx(), xz);
        mDoMtx_XrotM(calc_mtx(), ya);
        f32 e = i_this->field_0x2E8;
        if (i == 8) {
            f32 zz = e - 10.0f;
            if (zz < 0.0f)
                zz = 0.0f;
            sp60->z = zz;
        } else {
            sp60->z = e;
        }
        MtxPosition(sp60, sp54);
        XyzPl(&next->pos, tmp, sp54);
        cur->pos.copy(*tmp.get());
    }
    i_this->field_0x2D4.copy(i_this->field_0x33C[9].pos);
    cXyz_mi(&i_this->field_0x33C[8].pos, tmp, &i_this->field_0x33C[9].pos);
    {
        f32 x = tmp->x, y = tmp->y, z = tmp->z;
        sp60->x = x;
        sp60->y = y;
        sp60->z = z;
        i_this->field_0x2E0 = (s16)-cM_atan2s(y, z);
        i_this->field_0x2E2 = cM_atan2s(sp60->x, std_sqrtf(gabi::fmadds(sp60->y, sp60->y, sp60->z * sp60->z)));
    }
    hand_mtx_set(i_this);
    if (i_this->mpEmitter2 == 0) {
        if ((s8)i_this->field_0xA18 == 0)
            return;
        JPABaseEmitter* e = dComIfGp_particle_set(0x8184, &i_this->field_0x33C[0].pos);
        i_this->mpEmitter2 = gabi::ea(e);
        if (e == nullptr)
            return;
    }
    cXyz_mi(&i_this->field_0x33C[0].pos, tmp, &i_this->field_0x33C[1].pos);
    f32 x = tmp->x, y = tmp->y, z = tmp->z;
    sp60->x = x;
    sp60->y = y;
    sp60->z = z;
    s16 rot_y = cM_atan2s(x, z);
    s16 rot_x = (s16)-cM_atan2s(sp60->y, std_sqrtf(gabi::fmadds(sp60->x, sp60->x, sp60->z * sp60->z)));
    u32 e = i_this->mpEmitter2;
    f32 tx = i_this->field_0x33C[0].pos.x, ty = i_this->field_0x33C[0].pos.y, tz = i_this->field_0x33C[0].pos.z;
    JPA_setGlobalTranslation(e, tx, ty, tz);
    JPAGetXYZRotateMtx(rot_x, rot_y, 0, (u32)i_this->mpEmitter2 + 0x1F0);
    if (i_this->field_0x2C2[1] == 1) {
        JPA_becomeInvalidEmitter(i_this->mpEmitter2);
        i_this->mpEmitter2 = 0;
        i_this->field_0xA18 = 0;
    }
}

static void lines_copy(mDoExt_3DlineMat1_l* l, sss_s* s, s32 n) {
    u32 line = lineMat_lines(l);
    u32 sizes = gabi::load<u32>(line + 4);
    u32 segs = gabi::load<u32>(line + 0);
    for (s32 i = 0; i < n; i++) {
        gabi::at<cXyz>(segs + 12 * i)->copy(s[i].pos);
        gabi::store<u8>(sizes + i, (u8)gabi::ftoi(s[i].field_0x18));
    }
}

/* the inline dBgS_GndChk constructor / destructor (this TU's vtables) */
static const dBgS_GndChk_vt GNDCHK_VT = {0x1003D3C4, 0x1003D3D4, 0x1003D3F4, 0x1003D3E4};
static void gndchk_dt(void* g) {
    u32 b = gabi::ea(g);
    gabi::store<u32>(b + 0x20, 0x1003D3D4);
    gabi::store<u32>(b + 0x40, 0x1003D3F4);
    gabi::store<u32>(b + 0x4C, 0x1003D3B4);
    gabi::call(0x02008DAC, g, 0); /* cBgS_Chk::~cBgS_Chk */
}

/* 0248F484: control1/2, cut_control1/2, control3, hand_close inlined */
static void hand_move(sss_class* i_this) {
    WWHD_FUNC(0x0248F484, void, i_this);
    fopAc_ac_c* a_this = i_this;
    fopAc_ac_c* a_player = dComIfGp_getPlayer(0);
    u32 link = gabi::load<u32>(dComIfGp_ea() + 0x5B34); /* dComIfGp_getLinkPlayer() */
    f32 f30, f6, f28, f27, target, max_speed, f24, f5, f1;
    bool b1, b2;
    gabi::Local<cXyz> sp8C, sp80, sp74, tmp;

    gabi::Local<dBgS_GndChk> gnd_chk;
    dBgS_GndChk_ct(gnd_chk.get(), GNDCHK_VT, false); /* MaskNormalGrp folded in */
    b1 = false;
    b2 = false;
    f30 = 0.1f;
    f6 = 8.0f;
    f28 = 0.1f;
    f27 = 0.0f;
    target = 30.0f;
    max_speed = 1.0f;
    f24 = 0.0f;
    f32 f4 = fopAcM_searchActorDistance(a_this, dComIfGp_getPlayer(0));
    f1 = 5.0f;
    if (i_this->field_0x2B9 != 0xFF) {
        f5 = (f32)(u8)i_this->field_0x2B9 * 10.0f;
    } else {
        f5 = 1000.0f;
    }
    mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);
    mDoMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    s8 b = 0;
    switch ((u32)(s32)i_this->field_0x2C0) {
    case 0: {
        sp74->copy(i_this->current.pos);
        target = 0.0f;
        max_speed = 0.5f;
        f24 = -20.0f;
        if (!i_this->field_0x2C2[0]) {
            if (i_this->field_0x2BA) {
                if (dComIfGs_isSwitch(i_this->field_0x2BA, i_this->current.roomNo)) {
                    i_this->field_0x2C0 = 1;
                    i_this->field_0x2C2[0] = 30;
                    hand_open(i_this);
                    b = 1;
                }
            } else {
                if (f4 < f5) {
                    i_this->field_0x2C0 = 1;
                    i_this->field_0x2C2[0] = 30;
                    hand_open(i_this);
                    b = 1;
                }
            }
        }
        if (std::fabs(i_this->field_0x2C8.y - i_this->current.pos.y) < 5.0f && b) {
            if (gabi::ea(&a_this->eyePos) != 0) {
                s8 room = a_this->current.roomNo;
                u32 id = i_this->field_0x2B8 == 1 ? JA_SE_OBJ_SVINE_OUT_WATER : JA_SE_OBJ_SVINE_OUT;
                s32 rev = dComIfGp_getReverb(room);
                mDoAud_seStart(id, &a_this->eyePos, 0, rev);
            }
        }
        break;
    }
    case 1: {
        s32 t = i_this->field_0x2BC;
        sp8C->x = cM_ssin(t * 600) * 50.0f;
        sp8C->y = 250.0f;
        sp8C->z = cM_ssin(t * 700) * 50.0f;
        MtxPosition(sp8C, sp80);
        XyzPl(&i_this->current.pos, tmp, sp80);
        sp74->copy(*tmp.get());
        if (i_this->field_0x2C2[0] == 0 && f4 < 300.0f) {
            i_this->field_0x2C0 = 2;
        }
        if (f4 > (f5 + 100.0f)) {
            i_this->field_0x2C0 = 0;
        }
        break;
    }
    case 2:
        sp74->x = a_player->current.pos.x;
        sp74->y = a_player->current.pos.y;
        sp74->z = a_player->current.pos.z;
        f6 = 15.0f;
        f28 = 0.5f;
        f27 = 10.0f;
        b1 = true;
        sp74->y = sp74->y + 70.0f;
        if (f4 > 450.0f) {
            i_this->speedF = 0.0f;
            i_this->field_0x2C0 = 1;
        }
        cXyz_mi(sp74, tmp, &i_this->field_0x2C8);
        sp8C->copy(*tmp.get());
        if (std_sqrtf(PSVECSquareMag(sp8C)) < 20.0f && gabi::ea(a_player) == link) {
            i_this->field_0x2C0 = 3;
            /* hand_close */
            void* anm = dComIfG_getObjectRes(STR(0x1003D394) /* "Sss" */, dRes_INDEX_SSS_BCK_SSS_TOJIRU_e, SAFESTRING_VTBL);
            McaMorf_setAnm(i_this->mpMorf, anm, 0, 1.0f, 1.0f, 0.0f, -1.0f, nullptr);
            SE_START(a_this, JA_SE_OBJ_SVINE_GRASP);
        } else {
            break;
        }
        // fall-through
    case 3: {
        u32 p = gabi::ea(a_player);
        gabi::store<u32>(p + 0x3BC, gabi::load<u32>(p + 0x3BC) | 0x02000000); /* onNoResetFlg1(daPyFlg1_VINE_CATCH) */
        gabi::store<u32>(p + 0x3B4, 0xA3);                                     /* setFace(daPyFace_TIYAYA) */
        if (gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x34)) {              /* dComIfGs_getMagic() */
            u32 pl = dComIfGp_ea() + 0x5BEF;
            gabi::store<u8>(pl, gabi::load<u8>(pl) | 1);
            SE_START(a_this, JA_SE_OBJ_ATK_VINE_MP_SUCK);
            if ((i_this->field_0x2BC & 0x1F) == 0) {
                u32 m = dComIfGp_ea() + 0x5B60; /* dComIfGp_setItemMagicCount(-1) */
                gabi::store<s16>(m, (s16)(gabi::load<s16>(m) - 1));
            }
        }
        if (f4 > 400.0f) {
            /* HD keeps REG6_F(0) (0x1047B970) */
            f32 p5 = (f4 - 400.0f) * (gabi::load<f32>(0x1047B970) + 0.1f);
            if (p5 > 100.0f) {
                p5 = 100.0f;
            }
            u32 vt = a_player->__vtbl;
            s16 ang = fopAcM_searchActorAngleY(a_this, dComIfGp_getPlayer(0));
            gabi::call_ptr(gabi::load<u32>(vt + 0xEC), a_player, p5, (s16)(ang + 0x8000), 0); /* setOutPower */
        }
        sp74->x = a_player->current.pos.x;
        sp74->y = a_player->current.pos.y;
        sp74->z = a_player->current.pos.z;
        f28 = 5.0f;
        i_this->field_0x2F4 = 10.0f;
        f6 = 200.0f;
        f30 = 1.0f;
        sp74->y = sp74->y + 70.0f;
        if (f4 > 800.0f) {
            i_this->speedF = 30.0f;
            i_this->field_0x2C0 = 1;
            hand_open(i_this);
        }
        break;
    }
    case 5: {
        target = 25.0f;
        b2 = true;
        f1 = 50.0f;
        max_speed = 1.0f;
        i_this->field_0x2F4 = 50.0f;
        PSVECAdd(&i_this->field_0x2C8, &i_this->speed, &i_this->field_0x2C8);
        i_this->speed.y = i_this->speed.y - 3.0f;
        i_this->field_0x2C2[2] = 5;
        cXyz* gpos = gabi::at<cXyz>(gabi::ea(gnd_chk.get()) + 0x24);
        f32 gx = i_this->field_0x2C8.x, gy = i_this->field_0x2C8.y, gz = i_this->field_0x2C8.z;
        gpos->x = gx;
        gpos->z = gz;
        gpos->y = gy + 200.0f;
        f32 g = cBgS_GroundCross(dComIfG_Bgsp(), gnd_chk.get());
        i_this->field_0x2F8 = g;
        f32 g10 = g + 10.0f;
        if (g == -1000000000.0f || !(i_this->field_0x2C8.y > g10)) {
            i_this->field_0x2C8.y = g10;
            i_this->field_0x2C2[0] = 100;
            i_this->field_0x2C0 = 6;
        }
        break;
    }
    case 6:
        b2 = true;
        f1 = 0.0f;
        i_this->field_0x2C2[2] = 10;
        if (i_this->field_0x2C2[0] < 40) {
            target = 0.0f;
            max_speed = 1.0f;
            i_this->field_0x2C8.y = i_this->field_0x2C8.y - 2.0f;
            if (i_this->field_0x2C2[0] == 0) {
                i_this->field_0x2C8.copy(i_this->current.pos);
                i_this->field_0x2E8 = 0.0f;
                i_this->field_0x2FC = 0.0f;
                i_this->field_0x2C0 = 0;
            }
        }
        break;
    }
    cLib_addCalc2(&i_this->field_0x2E8, target, 0.5f, max_speed);
    cLib_addCalc2(&i_this->field_0x2F0, f27, 1.0f, 0.2f);
    cLib_addCalc2(&i_this->field_0x2F4, f1, 1.0f, 1.5f);
    if (!b2) {
        cLib_addCalc2(&i_this->speedF, f6, 1.0f, f28);
        if (i_this->field_0xA08 > 1.0f && i_this->field_0x2C0 != 3) {
            mDoMtx_YrotS(calc_mtx(), i_this->field_0xA0C);
            sp8C->x = 0.0f;
            sp8C->y = 100.0f;
            sp8C->z = i_this->field_0xA08;
            MtxPosition(sp8C, sp80);
            gabi::Local<cXyz> t2;
            XyzPl(&i_this->current.pos, t2, sp80);
            sp74->copy(*t2.get());
            f30 = 0.1f;
            f32 s = i_this->field_0xA08 * 0.2f;
            if (s > 30.0f) {
                s = 30.0f;
            }
            i_this->speedF = s;
        }
        cLib_addCalc0(&i_this->field_0xA08, 1.0f, 5.0f);
        cLib_addCalc2(&i_this->field_0x2C8.x, sp74->x, f30, i_this->speedF);
        cLib_addCalc2(&i_this->field_0x2C8.y, sp74->y, f30, i_this->speedF);
        cLib_addCalc2(&i_this->field_0x2C8.z, sp74->z, f30, i_this->speedF);
        cLib_addCalc2(&i_this->current.pos.y, i_this->home.pos.y + f24, 0.5f, 0.5f);
        if (b1 && i_this->current.angle.x == 0) {
            cLib_addCalcAngleS2(&i_this->current.angle.y, fopAcM_searchActorAngleY(a_this, dComIfGp_getPlayer(0)), 16, 2048);
        }
        control1(i_this);
        control2(i_this);
    } else {
        cut_control1(i_this);
        cut_control2(i_this);
        lines_copy(&i_this->field_0x454, i_this->field_0x490, 5);
        cLib_addCalc0(&i_this->field_0x2FC, 1.0f, 1.0f);
    }
    /* control3 */
    for (s32 i = 0; i < 10; i++) {
        f32 temp = gabi::fmadds(cM_ssin(i_this->field_0x2BC * 500 + i * 100), 0.1f, 0.8f);
        i_this->field_0x33C[i].field_0x18 = size_d(i) * temp;
    }
    gabi::call(0x025E535C, (u32)i_this->mpMorf, 0, 0, 0); /* play(NULL, 0, 0) */
    lines_copy(&i_this->field_0x300, i_this->field_0x33C, 10);
    u32 r22 = gabi::load<u32>(lineMat_lines(&i_this->field_0x300));
    i_this->eyePos.copy(*gabi::at<cXyz>(r22 + 5 * 12));
    gabi::at<cXyz>(gabi::ea(i_this) + 0x390)->copy(i_this->eyePos); /* attention_info.position */
    i_this->mStts.Move();
    i_this->field_0x8DC.SetC(b2 ? non_pos : &i_this->eyePos);
    u8 u1 = 0;
    dComIfG_Ccsp_Set(&i_this->field_0x8DC);
    for (s32 i = 0; i < 3; i++) {
        s32 r4 = ((i_this->field_0x2BC & 3) + i * 2) % 10;
        gabi::Local<cXyz> sp68;
        sp68->copy(*gabi::at<cXyz>(r22 + r4 * 12));
        i_this->field_0x558[i].SetC(b2 ? non_pos : sp68.get());
        if (i_this->field_0x2C0 == 3) {
            i_this->field_0x558[i].OffCoSPrmBit(1);
        } else {
            i_this->field_0x558[i].OnCoSPrmBit(1);
        }
        dComIfG_Ccsp_Set(&i_this->field_0x558[i]);
    }
    for (s32 i = 0; i < 3; i++) {
        if (i_this->field_0x558[i].ChkTgHit()) {
            u1 = (u8)(i + 1);
            break;
        }
    }
    if ((u1 || i_this->field_0x8DC.ChkTgHit() != 0) && i_this->field_0x2C2[2] == 0) {
        /* CcAtInfo: mpObj +0x0, mResultingAttackType +0xA, pParticlePos +0x14 */
        struct CcAtInfo_l { u8 _[0x1C]; };
        gabi::Local<CcAtInfo_l> at_info;
        u32 ai = gabi::ea(at_info.get());
        gabi::store<u32>(ai + 0x14, 0);
        i_this->field_0x2C2[2] = 20;
        if (!u1) {
            void* obj = i_this->field_0x8DC.GetTgHitObj();
            gabi::store<u32>(ai + 0x0, gabi::ea(obj));
            gabi::store<u32>(ai + 0x14, gabi::ea(&i_this->field_0x8DC) + 0xCC); /* GetTgHitPosP */
            at_power_check(at_info.get());
            if (gabi::load<u8>(ai + 0xA) == 8) {
                i_this->field_0xA08 = 300.0f;
                i_this->field_0xA0C = (s16)(fopAcM_searchActorAngleY(a_this, dComIfGp_getPlayer(0)) + 0x8000);
                gndchk_dt(gnd_chk.get());
                return;
            }
        } else {
            dCcD_GObjInf* objInf = &i_this->field_0x558[u1 - 1];
            void* obj = objInf->GetTgHitObj();
            gabi::store<u32>(ai + 0x0, gabi::ea(obj));
            gabi::store<u32>(ai + 0x14, gabi::ea(objInf) + 0xCC);
        }
        SE_START(a_this, JA_SE_LK_LAST_HIT);
        SE_START(a_this, JA_SE_OBJ_SVINE_CRASH);
        i_this->field_0x2C0 = 5;
        i_this->speed.x = cM_rndFX(10.0f);
        i_this->speed.y = cM_rndF(10.0f) + 30.0f;
        i_this->speed.z = cM_rndFX(10.0f);
        gabi::Local<cXyz> scl;
        scl->set(0.3f, 0.3f, 0.3f);
        dComIfGp_particle_set(0x16 /* ID_AK_JN_SIBOUFLASH */, &i_this->eyePos, nullptr, scl);
        i_this->field_0xA18 = 1;
        for (s32 i = 0; i < 5; i++) {
            sss_s* r18 = &i_this->field_0x33C[i];
            sss_s* r19 = &i_this->field_0x490[i];
            r19->pos.copy(r18->pos);
            r19->field_0x18 = (f32)r18->field_0x18;
            if (i == 4) {
                gabi::Local<cXyz> d;
                cXyz_mi(&r19[0].pos, d, &r19[-1].pos);
                i_this->field_0x2FC = std_sqrtf(PSVECSquareMag(d)) * 1.5f;
            }
        }
        hand_open(i_this);
    }
    gndchk_dt(gnd_chk.get());
}
VERIFY(0x0248F484, hand_move);

/* 0248EEEC: hand_main (empty) inlined */
static BOOL daSss_Execute(sss_class* i_this) {
    WWHD_FUNC(0x0248EEEC, BOOL, i_this);
    i_this->field_0x2BC = i_this->field_0x2BC + 1;
    for (int i = 0; i < 2; i++) {
        if (i_this->field_0x2C2[i] != 0) {
            i_this->field_0x2C2[i] = i_this->field_0x2C2[i] - 1;
        }
    }
    if (i_this->field_0x2C2[2] != 0) {
        i_this->field_0x2C2[2] = i_this->field_0x2C2[2] - 1;
    }
    if (i_this->field_0x2B8) {
        hand_move(i_this);
    }
    return TRUE;
}
VERIFY(0x0248EEEC, daSss_Execute);

/* 0248EF68 */
static BOOL daSss_IsDelete(sss_class* i_this) {
    WWHD_FUNC(0x0248EF68, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0248EF68, daSss_IsDelete);

/* 0248EF70 */
static BOOL daSss_Delete(sss_class* i_this) {
    WWHD_FUNC(0x0248EF70, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhs, STR(0x1003D434) /* "Sss" */);
    u32 emitter = i_this->mpEmitter1;
    if (emitter) {
        JPA_becomeInvalidEmitter(emitter);
    }
    emitter = i_this->mpEmitter2;
    if (emitter) {
        JPA_becomeInvalidEmitter(emitter);
    }
    return TRUE;
}
VERIFY(0x0248EF70, daSss_Delete);

/* 0248EFEC (also the solid-heap callback: daSss_solidHeapCB folded) */
static BOOL useHeapInit(sss_class* i_this) {
    WWHD_FUNC(0x0248EFEC, BOOL, i_this);
    const char* arc = STR(0x1003D398); /* "Sss" */
    void* modelData = dComIfG_getObjectRes(arc, dRes_INDEX_SSS_BMD_SSS_HAND_e, SAFESTRING_VTBL);
    void* anm = dComIfG_getObjectRes(arc, dRes_INDEX_SSS_BCK_SSS_HIRAKU_e, SAFESTRING_VTBL);
    /* new mDoExt_McaMorf(modelData, NULL, NULL, anm, EMode_NONE, 1.0f, 0, -1, TRUE, NULL, 0, 0x11020203) */
    u32 morf = gabi::call<u32>(0x025E4F64, (u32)0, modelData, (u32)0, (u32)0, anm, (u32)0, 1.0f, (u32)0, (s32)-1,
                               (u32)1, (u32)0, (u32)0, (u32)0x11020203);
    i_this->mpMorf = morf;
    if (!McaMorf_getModel(i_this->mpMorf)) {
        return FALSE;
    }
    void* tex = dComIfG_getObjectRes(arc, dRes_INDEX_SSS_BTI_SSS_e, SAFESTRING_VTBL);
    if (!mDoExt_3DlineMat1_init(&i_this->field_0x300, 1, 10, tex, TRUE)) {
        return FALSE;
    }
    tex = dComIfG_getObjectRes(arc, dRes_INDEX_SSS_BTI_SSS_e, SAFESTRING_VTBL);
    if (!mDoExt_3DlineMat1_init(&i_this->field_0x454, 1, 5, tex, TRUE)) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x0248EFEC, useHeapInit);

/* 0248F140 */
static cPhs_State daSss_Create(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x0248F140, cPhs_State, i_actor);
    sss_class* i_this = (sss_class*)i_actor;
    /* fopAcM_ct(i_actor, sss_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = SSS_VTBL;
            gabi::call(0x025EB82C, &i_this->field_0x300); /* mDoExt_3DlineMat1_c::mDoExt_3DlineMat1_c */
            gabi::call(0x025EB82C, &i_this->field_0x454);
            dCcD_Stts_ct(&i_this->mStts);
            gabi::call(0x028EFFD0, i_this->field_0x558, 3, 0x12C, 0x025166F0); /* __construct_array(dCcD_Sph) */
            gabi::call(0x025166F0, &i_this->field_0x8DC);
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&i_this->mPhs, STR(0x1003D448) /* "Sss" */);
    if (state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_actor, 0x0248EFEC /* useHeapInit */, 0x3040)) {
            non_pos->x = 0.0f;
            non_pos->y = 30000.0f;
            non_pos->z = -20000.0f;
            u8 p = fopAcM_GetParam(i_this) & 0xFF;
            /* GameCube: 0xFF -> 0; != 1 -> 0x23 */
            i_this->field_0x2B8 = (p != 0xFF && p == 1) ? 1 : 0x23;
            i_this->field_0x2B9 = (u8)(fopAcM_GetParam(i_this) >> 8);
            u8 p2 = (u8)(fopAcM_GetParam(i_this) >> 16);
            i_this->field_0x2BA = p2 == 0xFF ? 0 : p2;
            i_this->health = 2;
            i_this->field_0x2BC = (s16)gabi::ftoi(cM_rndF(10000.0f));
            i_this->mStts.Init(0xFF, 0xFF, i_this);
            for (int i = 0; i < 3; i++) {
                i_this->field_0x558[i].Set((const dCcD_SrcSph*)gabi::at<u8>(tg_sph_src));
                i_this->field_0x558[i].SetStts(&i_this->mStts);
            }
            i_this->field_0x8DC.Set((const dCcD_SrcSph*)gabi::at<u8>(bm_sph_src));
            i_this->field_0x8DC.SetStts(&i_this->mStts);
            i_this->field_0x2C8.x = i_this->current.pos.x;
            f32 y = i_this->current.pos.y;
            i_this->field_0x2C8.y = y;
            i_this->field_0x2C8.z = i_this->current.pos.z;
            if (!i_this->field_0x2BA) {
                i_this->field_0x2C8.y = y + 230.0f;
            }
            daSss_Execute(i_this);
        } else {
            state = cPhs_ERROR_e;
        }
    }
    return state;
}
VERIFY(0x0248F140, daSss_Create);

/* 0248F3DC */
static void __sinit_d_a_sss_cpp() {
    WWHD_FUNC(0x0248F3DC, void, (u32)0);
    sinit_header_statics(0x1046DFAC, 0x101D0E20);
}
VERIFY(0x0248F3DC, __sinit_d_a_sss_cpp);

/* 0248F470: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0248F470, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0248F470, trivial_dt);

/* 02491160: sss_class deleting destructor (inline member destructors) */
static void sss_class_dt(sss_class* i_this, s32 flags) {
    WWHD_FUNC(0x02491160, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x02515AE8, &i_this->field_0x8DC, 2);                              /* dCcD_Sph::~dCcD_Sph */
        gabi::call(0x028F0164, i_this->field_0x558, 3, 0x12C, 0x02515AE8, 0, 0);     /* __destroy_arr */
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025EB8B8, &i_this->field_0x454, 2);                              /* ~mDoExt_3DlineMat1_c */
        gabi::call(0x025EB8B8, &i_this->field_0x300, 2);
        gabi::call(0x025D50BC, i_this, 0);                                            /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02491160, sss_class_dt);

/* 02491204: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x02491204, void, p);
}
VERIFY(0x02491204, empty_virtual);
