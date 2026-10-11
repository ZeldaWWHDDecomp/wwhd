/* d_a_pw.cpp (WWHD): Poe enemy, verified against cking.rpx. */
#include "d/actor/d_a_pw.h"
using namespace gabi;
const gc_tu_data gc_tu = {
    .safeString = 0x10038364,
    .objAcch = {0x100383AC, 0x100383CC, 0x100383BC},
    .objAcchDt = {0x100383BC, 0x100383CC},
};

// HD: GHS emits separate archive literals in the two inlined state helpers.
static u8 l_idleArchive[3] = {80, 119, 0};
static u8 l_damageArchive[3] = {80, 119, 0};
struct PwHeaderZeros {
    be<u32> values[4];
};
static PwHeaderZeros l_headerZeros;
struct PwHeaderPi {
    be<f32> values[2];
};
static PwHeaderPi l_headerPi;
struct PwDrawBuffers {
    be<u32> opaque;
    be<u32> translucent;
};
static PwDrawBuffers l_drawBuffers;
static u32 l_calcMtx;
static u32 l_resourceControl;
static u32 l_saveInfo;
static u32 l_currentModel;
static u8 TORITUKI_ON;
static s16 l_behaviorOverride;
static f32 l_alphaAmplitude = 30.0f;
static f32 l_piPair[2] = {-3.1415927410125732f, 3.1415927410125732f};
static f32 l_fireSize[10] = {1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f};
static u8 l_fireColor[10] = {1, 2, 3, 5, 7, 8, 9, 11, 18, 22};
struct PwSphereSource {
    be<u32> mFlags;
    be<u32> mAtType;
    u8 mAtPower;
    u8 _pad[3];
    be<u32> mAtSPrm;
    be<u32> mTgType;
    be<u32> mTgSPrm;
    be<u32> mCoSPrm;
    u8 mAtSe;
    u8 mAtHitMark;
    u8 mAtSpl;
    u8 mAtMaterial;
    be<u32> mGAtSPrm;
    u8 mTgSe;
    u8 mTgHitMark;
    u8 mTgSpl;
    u8 mTgMaterial;
    be<u32> mGTgSPrm;
    be<u32> mGCoSPrm;

    be<f32> mCenter[3];
    be<f32> mRadius;
};
struct PwCylinderSource {
    be<u32> mFlags;
    be<u32> mAtType;
    u8 mAtPower;
    u8 _pad[3];
    be<u32> mAtSPrm;
    be<u32> mTgType;
    be<u32> mTgSPrm;
    be<u32> mCoSPrm;
    u8 mAtSe;
    u8 mAtHitMark;
    u8 mAtSpl;
    u8 mAtMaterial;
    be<u32> mGAtSPrm;
    u8 mTgSe;
    u8 mTgHitMark;
    u8 mTgSpl;
    u8 mTgMaterial;
    be<u32> mGTgSPrm;
    be<u32> mGCoSPrm;

    be<f32> mCenter[3];
    be<f32> mRadius;
    be<f32> mHeight;
};
static PwSphereSource l_sph_src = {
    0x0, 0x1000, 2,   0x5, 0xff1dfeff,         0xf,  0x71, 5, 0, 0, 0, 0x0, 0, 12,
    0,   0,      0x3, 0x0, {0.0f, 0.0f, 0.0f}, 15.0f};
static PwCylinderSource l_cyl_src = {
    0x0, 0x1000, 0,   0x15, 0x900000,           0x3,   0x0, 0, 0, 0, 0, 0x0, 0, 0,
    0,   0,      0x6, 0x0,  {0.0f, 0.0f, 0.0f}, 15.0f, 0.0f};

namespace {
// HD: preserve the original word copies, including noncanonical float bit patterns.
f32 add(f32 a, f32 b) {
    volatile double x = (double)a, y = (double)b;
    return (f32)(x + y);
}
f32 sub(f32 a, f32 b) {
    volatile double x = (double)a, y = (double)b;
    return (f32)(x - y);
}
} // namespace

/* 0244B3AC */
void first_mode_change(pw_class* actor) {
    u32 a = ea(actor), flags = static_cast<u32>(actor->actor_status);
    s16 yaw = static_cast<s16>(actor->shape_angle.y);
    (actor->m340 = 0);
    (actor->m38C = yaw);
    (actor->current.angle.y = yaw);
    (actor->m39C = 100);
    (actor->attention_info.flags = 4);
    (actor->m39A = 100);
    (actor->actor_status = flags | 0x20);
}

/* 0244DD64 */
s32 daPW_IsDelete(pw_class* actor) { return 1; }

/* 0244A768 */
void BG_check(pw_class* actor) {
    u32 a = ea(actor);
    Pw_dBgS_AcchCir_SetWall(pwAddress(actor, offsetof(pw_class, mAcchCir)), 70.f, 140.f);
    f32 y = static_cast<f32>(actor->current.pos.y), offset = static_cast<f32>(actor->mCorrectionOffsetY),
        oldY = static_cast<f32>(actor->old.pos.y);
    (actor->current.pos.y = sub(y, offset));
    (actor->old.pos.y = sub(oldY, offset));
    u32 play = Pw_dComIfGp_get();
    Pw_dBgS_Acch_CrrPos(pwAddress(actor, offsetof(pw_class, mAcch)), play + 0x12A0);
    offset = static_cast<f32>(actor->mCorrectionOffsetY);
    y = static_cast<f32>(actor->current.pos.y);
    oldY = static_cast<f32>(actor->old.pos.y);
    (actor->current.pos.y = add(y, offset));
    (actor->old.pos.y = add(oldY, offset));
}

/* 0244DD6C */
s32 daPW_Delete(pw_class* actor) {
    u32 a = ea(actor);
    Pw_dComIfG_resDelete(pwAddress(actor, offsetof(pw_class, mPhase)), gabi::guest_str("Pw"));
    if (static_cast<u32>(actor->heap.v))
        Pw_mDoExt_McaMorf_stopZelAnime(static_cast<u32>(actor->mpMorf));
    if (static_cast<u8>(actor->m343)) {
        (actor->m343 = 0);
        (TORITUKI_ON = 0);
    }
    u32 vtable = static_cast<u32>(actor->m5C4.__vtbl);
    pwEndFollow(vtable, a + offsetof(pw_class, m5C4));
    Pw_enemy_fire_remove(pwAddress(actor, offsetof(pw_class, mEnemyFire)));
    if (static_cast<s16>(actor->mMode) == 0x51) {
        u32 play = Pw_dComIfGp_get(), player = static_cast<u32>(pwViewPwPlayView(play)->mpControlPlayer);
        (pwViewdaPy_py_c(player)->mNoResetFlg1 =
             static_cast<u32>(pwViewdaPy_py_c(player)->mNoResetFlg1) & ~0x100u);
    }
    return 1;
}

/* 0244E94C */
void pw_small_delete(u32 object, u32 flags) {
    if (object && (flags & 1))
        Pw_dl(object);
}

/* 02450D7C */
void pw_empty_virtual(pw_class* actor) {}

/* 0244A9E4 */
void alpha_anime(pw_class* actor) {
    u32 a = ea(actor);
    if (!static_cast<u8>(actor->m33E)) {
        (actor->m38E = (s16)(static_cast<s16>(actor->m38E) + 0x400));
        Pw_cLib_addCalcAngleS2(pwAddress(actor, offsetof(pw_class, m39C)), 100, 1, 10);
        f32 base = (f32) static_cast<s16>(actor->m39C);
        u16 angle = static_cast<u16>(actor->m38E);
        f32 sine = cM_ssin(angle);
        f32 alpha = fmadds(sine, l_alphaAmplitude, base);
        (actor->m39A = (s16)ftoi(alpha));
    }
}

/* 02449ECC */
void pw_color_to_float(u32 output, u32 color) {
    f32 red = (f32) static_cast<u8>(pwViewGXColor(color)->r),
        green = (f32) static_cast<u8>(pwViewGXColor(color)->g);
    f32 blue = (f32) static_cast<u8>(pwViewGXColor(color)->b),
        alpha = (f32) static_cast<u8>(pwViewGXColor(color)->a);
    red = (f32)((double)red / 255.0);
    green = (f32)((double)green / 255.0);
    blue = (f32)((double)blue / 255.0);
    alpha = (f32)((double)alpha / 255.0);
    (pwViewPwFloatColor(output)->r = red);
    (pwViewPwFloatColor(output)->g = green);
    (pwViewPwFloatColor(output)->b = blue);
    (pwViewPwFloatColor(output)->a = alpha);
}

/* 0244E184 */
u32 pw_fire_constructor(u32 object) {
    if (!object) {
        object = Pw_operator_new(0x22Cu);
        if (!object)
            return 0;
    }
    if (object + 0x8C == 0)
        Pw_operator_new(12u);
    Pw_cCcD_Stts_ct(object + 0xA0);
    Pw_dCcD_GStts_dCcD_GStts(object + 0xBC);
    (pwViewPwEnemyFire(object)->mStts.__vtbl = PW_STATUS_VTABLE);
    (pwViewPwEnemyFire(object)->mStts.__vtbl_gstts = PW_GAME_STATUS_VTABLE);
    Pw_dCcD_Sph_ct(object + 0xDC);
    (pwViewPwEnemyFire(object)->mScale = 1.f);
    return object;
}

/* 0244E210 */
u32 pw_constructor(u32 a) {
    if (!a) {
        a = Pw_operator_new(0xF8Cu);
        if (!a)
            return 0;
    }
    pw_class* actor = gabi::at<pw_class>(a);
    Pw_fopAc_ac_c_fopAc_ac_c(a);
    (actor->__vtbl = PW_ACTOR_VTABLE);
    Pw_dBgS_AcchCir_dBgS_AcchCir(pwAddress(actor, offsetof(pw_class, mAcchCir)));
    Pw_dBgS_Acch_dBgS_Acch(pwAddress(actor, offsetof(pw_class, mAcch)));
    (actor->mAcch.cBgS_Chk::__vtbl = gc_tu.objAcch.v10);
    (actor->mAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20);
    (actor->mAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14);
    (actor->mAcch.mbObjThrough = 1);
    Pw_dPa_followEcallBack_dPa_followEcallBack(pwAddress(actor, offsetof(pw_class, m5C4)), 0u, 0u);
    Pw_cCcD_Stts_ct(pwAddress(actor, offsetof(pw_class, mStts)));
    Pw_dCcD_GStts_dCcD_GStts(pwAddress(actor, offsetof(pw_class, mStts.__vtbl_gstts)));
    (actor->mStts.__vtbl = PW_STATUS_VTABLE);
    (actor->mStts.__vtbl_gstts = PW_GAME_STATUS_VTABLE);
    Pw_dCcD_GObjInf_dCcD_GObjInf(pwAddress(actor, offsetof(pw_class, mCyl)));
    (pwViewPwShapeAttribute(a + offsetof(pw_class, mCyl.mShapeAttr))->mVtable = PW_GEOMETRY_ATTR_VTABLE);
    (pwViewPwShapeAttribute(a + offsetof(pw_class, mCyl.mShapeAttr))->mBaseVtable = PW_CYLINDER_ATTR_VTABLE);
    Pw_cM3dGCyl_ct(pwAddress(actor, offsetof(pw_class, mCyl.mCyl)));
    (actor->mCyl.__vtbl_hitinf = PW_HIT_INFO_VTABLE);
    (pwViewPwShapeAttribute(a + offsetof(pw_class, mCyl.mShapeAttr))->mVtable = PW_CYLINDER_SHAPE_VTABLE);
    (actor->mCyl.mCyl.__vtbl = PW_CYLINDER_GEOMETRY_VTABLE);
    Pw_dCcD_Sph_ct(pwAddress(actor, offsetof(pw_class, mSph)));
    Pw_cCcD_Stts_ct(pwAddress(actor, offsetof(pw_class, mEnemyIce.mStts)));
    Pw_dCcD_GStts_dCcD_GStts(pwAddress(actor, offsetof(pw_class, mEnemyIce.mStts.__vtbl_gstts)));
    (actor->mEnemyIce.mStts.__vtbl = PW_STATUS_VTABLE);
    (actor->mEnemyIce.mStts.__vtbl_gstts = PW_GAME_STATUS_VTABLE);
    Pw_dCcD_GObjInf_dCcD_GObjInf(pwAddress(actor, offsetof(pw_class, mEnemyIce.mCyl)));
    (pwViewPwShapeAttribute(a + offsetof(pw_class, mEnemyIce.mCyl.mShapeAttr))->mBaseVtable =
         PW_CYLINDER_ATTR_VTABLE);
    (pwViewPwShapeAttribute(a + offsetof(pw_class, mEnemyIce.mCyl.mShapeAttr))->mVtable =
         PW_GEOMETRY_ATTR_VTABLE);
    Pw_cM3dGCyl_ct(pwAddress(actor, offsetof(pw_class, mEnemyIce.mCyl.mCyl)));
    (actor->mEnemyIce.mCyl.mCyl.__vtbl = PW_CYLINDER_GEOMETRY_VTABLE);
    (pwViewPwShapeAttribute(a + offsetof(pw_class, mEnemyIce.mCyl.mShapeAttr))->mVtable =
         PW_CYLINDER_SHAPE_VTABLE);
    (actor->mEnemyIce.mCyl.__vtbl_hitinf = PW_HIT_INFO_VTABLE);
    Pw_dBgS_AcchCir_dBgS_AcchCir(pwAddress(actor, offsetof(pw_class, mEnemyIce.mAcchCir)));
    Pw_dBgS_Acch_dBgS_Acch(pwAddress(actor, offsetof(pw_class, mEnemyIce.mAcch)));
    (actor->mEnemyIce.mAcch.mbObjThrough = 1);
    (actor->mEnemyIce.mAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20);
    (actor->mEnemyIce.mAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14);
    (actor->mEnemyIce.mAcch.cBgS_Chk::__vtbl = gc_tu.objAcch.v10);
    Pw_enemy_fire_ct(pwAddress(actor, offsetof(pw_class, mEnemyFire)));
    Pw_mDoExt_invisibleModel_ct(pwAddress(actor, offsetof(pw_class, mInvisibleModel)));
    return a;
}

/* 0244E8B8 */
void pw_static_initialize() {
    (l_headerZeros.values[2] = 0);
    (l_headerZeros.values[0] = 0);
    (l_headerZeros.values[3] = 0);
    (l_headerZeros.values[1] = 0);
    Pw_register_global_object(PW_HEADER_REGISTRATION);
    f32 low = l_piPair[0], high = l_piPair[1];
    (l_headerPi.values[0] = low);
    (l_headerPi.values[1] = high);
    Pw_GHS_section_ct(PW_HEADER_SECTION);
    Pw_register_global_object(PW_SECTION_REGISTRATION);
    Pw_GHS_header_ct(PW_HEADER_OBJECT);
    Pw_register_global_object(PW_OBJECT_REGISTRATION);
}

/* 02450C68 */
void pw_destructor(u32 a, u32 flags) {
    pw_class* actor = gabi::at<pw_class>(a);
    if (!a)
        return;
    Pw_mDoExt_invisibleModel_dt(pwAddress(actor, offsetof(pw_class, mInvisibleModel)), 2);
    Pw_dCcD_Sph_dt(pwAddress(actor, offsetof(pw_class, mEnemyFire.mSph)), 2);
    Pw_dCcD_Stts_dt(pwAddress(actor, offsetof(pw_class, mEnemyFire.mStts)), 2);
    (actor->mEnemyIce.mAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20);
    (actor->mEnemyIce.mAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14);
    Pw_dBgS_Acch_dt(pwAddress(actor, offsetof(pw_class, mEnemyIce.mAcch)), 0);
    Pw_dBgS_AcchCir_dt(pwAddress(actor, offsetof(pw_class, mEnemyIce.mAcchCir.m_cir)), 2);
    Pw_dCcD_GObjInf_dt(pwAddress(actor, offsetof(pw_class, mEnemyIce.mCyl)), 2);
    Pw_dCcD_Stts_dt(pwAddress(actor, offsetof(pw_class, mEnemyIce.mStts)), 2);
    Pw_dCcD_Sph_dt(pwAddress(actor, offsetof(pw_class, mSph)), 2);
    Pw_dCcD_GObjInf_dt(pwAddress(actor, offsetof(pw_class, mCyl)), 2);
    Pw_dCcD_Stts_dt(pwAddress(actor, offsetof(pw_class, mStts)), 2);
    (actor->mAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20);
    (actor->mAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14);
    Pw_dBgS_Acch_dt(pwAddress(actor, offsetof(pw_class, mAcch)), 0);
    Pw_dBgS_AcchCir_dt(pwAddress(actor, offsetof(pw_class, mAcchCir.m_cir)), 2);
    Pw_fopAc_ac_c_fopAc_ac_c(a, 0);
    if (flags & 1)
        Pw_dl(a);
}

// HD: both Jalhalla checks return their verified status value.
/* 0244A5A8 */
s32 Big_pow_gattai_check(pw_class* actor) {
    u32 a = ea(actor), id = static_cast<u32>(actor->mJalhallaID);
    if (id == 0xFFFFFFFF)
        return 0;
    Local<be<u32>> found;
    if (!Pw_fopAcM_SearchByID(id, gabi::ea(found.get())))
        return 0;
    u32 other = found->get();
    if (!other || static_cast<s16>(pwViewfopAc_ac_c(other)->mProcName) != 0xD3 ||
        !static_cast<u8>(pwViewPwJalhallaView(other)->mMergeState))
        return 0;
    u32 flags = static_cast<u32>(actor->mCyl.mObjAt.mSPrm),
        target = static_cast<u32>(actor->mCyl.mObjTg.mSPrm);
    (actor->m340 = 0);
    (actor->mCyl.mObjAt.mSPrm = flags & ~1u);
    (actor->mCyl.mObjTg.mSPrm = target & ~1u);
    Pw_dCcD_GObjInf_ClrTgHit(pwAddress(actor, offsetof(pw_class, mCyl)));
    s16 angle = static_cast<s16>(Pw_fopAcM_searchActorAngleY(a, found->get()));
    (actor->m38C = angle);
    (actor->mAction = PW_ACTION_BIG_DEMO);
    (actor->mMode = 0xAA);
    return 1;
}

/* 0244A67C */
s32 Big_pow_down_check(pw_class* actor) {
    u32 a = ea(actor);
    if (static_cast<s8>(actor->health) > 0 || static_cast<u8>(actor->m345))
        return 0;
    u32 id = static_cast<u32>(actor->mJalhallaID);
    if (id == 0xFFFFFFFF)
        return 0;
    Local<be<u32>> found;
    if (!Pw_fopAcM_SearchByID(id, gabi::ea(found.get())))
        return 0;
    u32 other = found->get();
    if (!other || static_cast<s16>(pwViewfopAc_ac_c(other)->mProcName) != 0xD3)
        return 0;
    if (static_cast<s16>(pwViewPwJalhallaView(other)->mKnockdown) == 0x6F &&
        static_cast<s16>(pwViewPwJalhallaView(other)->mMergeAllowed) > 3) {
        (pwViewfopAc_ac_c(other)->health = (u8)(static_cast<u8>(pwViewfopAc_ac_c(other)->health) - 1));
        if (static_cast<s8>(pwViewfopAc_ac_c(found->get())->health) <= 0)
            (actor->m344 = 1);
        (actor->m345 = 1);
        return 0;
    }
    (actor->health = 4);
    return 1;
}

// HD: loop mode occupies the original full-width argument register.
/* 0244A37C */
void anm_init(pw_class* actor, s32 animation, f32 blend, u32 loop, f32 speed, s32 sound) {
    u32 a = ea(actor);
    (actor->mBckIdx = animation);
    struct String {
        be<u32> data;
        be<u32> vtable;
    };
    Local<String> archive;
    archive->data = gabi::guest_str("Pw");
    archive->vtable = gc_tu.safeString;
    u32 transform = Pw_dRes_control_c_getRes(l_resourceControl, gabi::ea(archive.get()), animation),
        soundData = 0;
    if (sound >= 0) {
        Local<String> soundArchive;
        soundArchive->data = gabi::guest_str("Pw");
        soundArchive->vtable = gc_tu.safeString;
        soundData = Pw_dRes_control_c_getRes(l_resourceControl, gabi::ea(soundArchive.get()), sound);
    }
    Pw_mDoExt_McaMorf_setAnm(static_cast<u32>(actor->mpMorf), transform, loop, blend, speed, 0.f, -1.f,
                             soundData);
}

/* 0244B308 */
void move_sound(pw_class* actor) {
    u32 a = ea(actor);
    f32 volume = (f32)((double)static_cast<f32>(actor->speedF) * 4.0);
    u32 level =
        !(volume < 2147483648.f) ? (u32)ftoi(sub(volume, 2147483648.f)) + 0x80000000u : (u32)ftoi(volume);
    if (pwAddress(actor, offsetof(pw_class, eyePos))) {
        s32 reverb = Pw_dComIfGp_getReverb((s32) static_cast<s8>(actor->current.roomNo));
        Pw_cc_seStart(0x7051u, pwAddress(actor, offsetof(pw_class, eyePos)), level, reverb);
    }
}

/* 0244B058 */
s32 hani_check(pw_class* actor) {
    u32 a = ea(actor);
    if (static_cast<u32>(actor->mJalhallaID) != 0xFFFFFFFF)
        return 0;
    bool path = static_cast<u8>(actor->mPathIndex) != 0xFF && static_cast<u32>(actor->mpPath) != 0;
    f32 z = static_cast<f32>(path ? actor->m2FC.z : actor->m2F0.z),
        actorZ = static_cast<f32>(actor->current.pos.z), actorX = static_cast<f32>(actor->current.pos.x);
    f32 dz = sub(z, actorZ), x = static_cast<f32>(path ? actor->m2FC.x : actor->m2F0.x), dx = sub(x, actorX);
    f32 squareZ = (f32)((double)dz * dz);
    f32 radius = path ? 500.f : 1000.f;
    f32 distance = Pw_sqrtf_HD(fmadds(dx, dx, squareZ));
    bool far = path ? !(distance <= radius) : distance > radius;
    if (!far) {
        if (static_cast<s16>(actor->mMode) == 0x5B || !Pw_kougen_hani_check(a, 1))
            return 0;
    }
    (actor->m38C = static_cast<s16>(Pw_cM_atan2s(dx, dz)));
    return 1;
}

/* 0244A4A8 */
void kantera_break(pw_class* actor) {
    u32 a = ea(actor), play = Pw_dComIfGp_get(), id = static_cast<u32>(actor->mKanteraID);
    u32 player = static_cast<u32>(pwViewPwPlayView(play)->mpPlayer);
    (actor->m346 = 0);
    if (id == 0xFFFFFFFF)
        return;
    Local<be<u32>> found;
    if (!Pw_fopAcM_SearchByID(id, gabi::ea(found.get())))
        return;
    u32 lantern = found->get();
    if (!lantern || static_cast<s16>(pwViewfopAc_ac_c(lantern)->mProcName) != 0xC1)
        return;
    f32 px = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.x),
        lx = static_cast<f32>(pwViewfopAc_ac_c(lantern)->current.pos.x);
    f32 pz = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.z),
        lz = static_cast<f32>(pwViewfopAc_ac_c(lantern)->current.pos.z);
    s32 angle = Pw_cM_atan2s(sub(lx, px), sub(lz, pz));
    Pw_mDoMtx_YrotS(l_calcMtx, angle);

    Local<cXyz> velocity;
    velocity->x = 0.f;
    velocity->y = 0.f;
    velocity->z = 20.f;
    Pw_MtxPosition(gabi::ea(velocity.get()), found->get() + 0x33C);
    (pwViewfopAc_ac_c(found->get())->speed.y = 10.f);
    (pwViewPwLanternTail(lantern)->mState = 5);
    (actor->mKanteraID = 0xFFFFFFFF);
}

/* 0244ACBC */
s32 kougen_hani_check(pw_class* actor, s32 mode) {
    u32 a = ea(actor), play = Pw_dComIfGp_get();
    f32 x = static_cast<f32>(actor->current.pos.x);
    u32 player = static_cast<u32>(pwViewPwPlayView(play)->mpPlayer);

    Local<cXyz> position;
    position->x = x;
    position->y = static_cast<f32>(actor->current.pos.y);
    f32 z = static_cast<f32>(actor->current.pos.z);
    position->z = z;
    if (mode == 0) {
        f32 ax = static_cast<f32>(actor->current.pos.x),
            px = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.x);
        f32 az = static_cast<f32>(actor->current.pos.z),
            pz = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.z);
        f32 dx = (f32)((double)sub(ax, px) * 0.33000001311302185f);
        f32 dz = (f32)((double)sub(az, pz) * 0.33000001311302185f);
        for (s32 i = 0; i < 3; ++i) {
            position->x = add(x, dx);
            position->z = add(z, dz);
            play = Pw_dComIfGp_get();
            if (Pw_dDetect_c_chk_light(play + 0x5A20, gabi::ea(position.get())))
                return 1;
            x = position->x;
            z = position->z;
        }
        return 0;
    }
    Pw_mDoMtx_YrotS(l_calcMtx, (s32) static_cast<s16>(actor->current.angle.y));
    Local<cXyz> offset;
    offset->x = 0.f;
    offset->y = 0.f;
    offset->z = 200.f;
    Pw_MtxPosition(gabi::ea(offset.get()), gabi::ea(position.get()));
    Pw_PSVECAdd(gabi::ea(position.get()), pwAddress(actor, offsetof(pw_class, current.pos)),
                gabi::ea(position.get()));
    play = Pw_dComIfGp_get();
    return Pw_dDetect_c_chk_light(play + 0x5A20, gabi::ea(position.get())) != 0;
}

/* 0244B1B8 */
void next_dousa_check(pw_class* actor) {
    u32 a = ea(actor), play = Pw_dComIfGp_get(), flags = static_cast<u32>(actor->mCyl.mGObjTg.mSPrm);
    u32 player = static_cast<u32>(pwViewPwPlayView(play)->mpPlayer);
    (actor->mCyl.mTgType = 0x900000);
    (actor->mCyl.mGObjTg.mSPrm = flags & ~2u);

    Local<cXyz> target;
    target->x = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.x);
    target->y = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.y);
    target->z = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.z);
    if (!Pw_Line_check(a, gabi::ea(target.get()), 1) && !Pw_hani_check(a)) {
        play = Pw_dComIfGp_get();
        f32 distance = Pw_fopAcM_searchActorDistance(a, static_cast<u32>(pwViewPwPlayView(play)->mpPlayer));
        if (distance < 500.f) {
            f32 playerY = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.y),
                actorY = static_cast<f32>(actor->current.pos.y);
            f32 difference = sub(actorY, playerY);
            if (__builtin_fabsf(difference) < 100.f) {
                (actor->mAction = PW_ACTION_KOUGEKI);
                (actor->mMode = 0x20);
                return;
            }
        }
    }
    f32 random = Pw_cM_rndF(70.f);
    s32 timer = ftoi(add(random, 70.f));
    u8 path = static_cast<u8>(actor->mPathIndex);
    (actor->m37C = (s16)timer);
    if (path == 0xFF) {
        f32 x = static_cast<f32>(actor->current.pos.x), z = static_cast<f32>(actor->current.pos.z);
        (actor->m2F0.x = x);
        (actor->m2F0.z = z);
    }
    (actor->mAction = PW_ACTION_DOUSA);
    (actor->mMode = 0xD);
}

/* 02449DA8 */
void draw_SUB(pw_class* actor) {
    u32 a = ea(actor), morf = static_cast<u32>(actor->mpMorf);
    f32 sy = static_cast<f32>(actor->scale.y);
    u32 model = static_cast<u32>(pwViewmDoExt_McaMorf(morf)->mpModel.v);
    f32 sx = static_cast<f32>(actor->scale.x), sz = static_cast<f32>(actor->scale.z);
    (pwViewJ3DModel(model)->mBaseScale.x = sx);
    (pwViewJ3DModel(model)->mBaseScale.z = sz);
    (pwViewJ3DModel(model)->mBaseScale.y = sy);
    constexpr u32 matrix = 0x1048D0CC;
    f32 offset = static_cast<f32>(actor->m3A4), y = static_cast<f32>(actor->current.pos.y),
        x = static_cast<f32>(actor->current.pos.x), z = static_cast<f32>(actor->current.pos.z);
    Pw_PSMTXTrans(matrix, x, add(y, offset), z);
    Pw_mDoMtx_YrotM(matrix, (s32) static_cast<s16>(actor->shape_angle.y));
    Pw_mDoMtx_XrotM(matrix, (s32) static_cast<s16>(actor->shape_angle.x));
    Pw_mDoMtx_ZrotM(matrix, (s32) static_cast<s16>(actor->shape_angle.z));
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = static_cast<f32>(pwMatrixElement(matrix, i));
    const u32 order[12] = {0, 10, 8, 5, 11, 9, 3, 1, 2, 7, 6, 4};
    for (u32 i : order)
        (pwMatrixElement(model + offsetof(J3DModel, mBaseTransformMtx), i) = values[i]);
    Pw_mDoExt_McaMorf_calc(static_cast<u32>(actor->mpMorf));
    Pw_enemy_fire(pwAddress(actor, offsetof(pw_class, mEnemyFire)));
    u32 lighting = Pw_dKy_getEnvlight();
    Pw_dScnKy_env_light_c_settingTevStruct(lighting, 0, pwAddress(actor, offsetof(pw_class, current.pos)),
                                           pwAddress(actor, offsetof(pw_class, tevStr)));
}

/* 02449C60 */
s32 nodeCallBack(u32 joint, s32 phase) {
    if (phase)
        return 1;
    u32 info = Pw_J3DJoint_getKind_HD(joint), model = l_currentModel;
    u32 actor = static_cast<u32>(pwViewJ3DModel(model)->mUserArea);
    u16 index = static_cast<u16>(pwViewPwJointNumber(info)->mJointNo);
    if (!actor || index != 0x17)
        return 1;
    u32 data = static_cast<u32>(pwViewJ3DModel(model)->mpMtxBuffer.v);
    u16 flags = static_cast<u16>(pwViewJ3DMtxBuffer_hd(data)->mFlags);
    u32 matrices = static_cast<u32>(pwViewJ3DModelData(data)->mMaterials);
    (pwViewJ3DMtxBuffer_hd(data)->mFlags = flags | 0x10);
    Pw_PSMTXCopy(matrices + 0x450, l_calcMtx);
    u32 x = static_cast<u32>(pwVectorWord(&pwViewpw_class(actor)->m2CC, 0)),
        y = static_cast<u32>(pwVectorWord(&pwViewpw_class(actor)->m2CC, 1));
    (pwVectorWord(&pwViewpw_class(actor)->m2E4, 0) = x);
    u32 z = static_cast<u32>(pwVectorWord(&pwViewpw_class(actor)->m2CC, 2));
    (pwVectorWord(&pwViewpw_class(actor)->m2E4, 1) = y);
    (pwVectorWord(&pwViewpw_class(actor)->m2E4, 2) = z);

    Local<cXyz> origin;
    origin->x = 0.f;
    origin->y = 0.f;
    origin->z = 0.f;
    Pw_MtxPosition(gabi::ea(origin.get()), actor + 0x3E8);
    data = static_cast<u32>(pwViewJ3DModel(model)->mpMtxBuffer.v);
    u32 matrix = l_calcMtx;
    flags = static_cast<u16>(pwViewJ3DMtxBuffer_hd(data)->mFlags);
    matrices = static_cast<u32>(pwViewJ3DModelData(data)->mMaterials);
    (pwViewJ3DMtxBuffer_hd(data)->mFlags = flags | 0x10);
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = static_cast<f32>(pwMatrixElement(matrix, i));
    const u32 order[12] = {8, 6, 9, 5, 11, 0, 7, 4, 2, 1, 3, 10};
    for (u32 i : order)
        (pwMatrixElement(matrices + 23 * sizeof(Mtx34), i) = values[i]);
    Pw_PSMTXCopy(l_calcMtx, 0x104B4868u);
    return 1;
}

/* 0244AA98 */
void fuwafuwa_calc(pw_class* actor) {
    u32 a = ea(actor), play = Pw_dComIfGp_get();
    f32 ground = static_cast<f32>(actor->mAcch.m_ground_h);
    s16 angle = (s16)(static_cast<s16>(actor->m390) + 700);
    f32 height = static_cast<f32>(actor->m2F0.y);
    u32 player = static_cast<u32>(pwViewPwPlayView(play)->mpPlayer);
    (actor->m390 = angle);
    if (ground != -1000000000.f) {
        play = Pw_dComIfGp_get();
        if (Pw_cBgS_ChkPolySafe(play + 0x12A0,
                                pwAddress(actor, offsetof(pw_class, mAcch.m_gnd.mPolyIndex)))) {
            play = Pw_dComIfGp_get();
            if (Pw_dBgS_GetGroundCode(play + 0x12A0,
                                      pwAddress(actor, offsetof(pw_class, mAcch.m_gnd.mPolyIndex))) == 4) {
                if (static_cast<s16>(actor->mAction) == PW_ACTION_KOUGEKI)
                    height = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.y);
                (actor->current.pos.y = add(height, 30.f));
                return;
            }
        }
        angle = static_cast<s16>(actor->m390);
    }
    u8 airborne = static_cast<u8>(actor->mHoversAtInitialYPos);
    s16 mode = static_cast<s16>(actor->mMode);
    if (!airborne)
        height = static_cast<f32>(actor->mAcch.m_ground_h);
    if (mode == 0x21 || mode == 0x28)
        height = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.y);
    f32 sine = cM_ssin((u16)angle);
    Pw_cLib_addCalc2(pwAddress(actor, offsetof(pw_class, current.pos.y)),
                     fmadds(sine, 30.f, add(height, 30.f)), 1.f, 3.f);
    if (static_cast<s16>(actor->mMode) == 0x26) {
        u16 phase = (u16)(static_cast<s16>(actor->m392) + 1000);
        f32 z = static_cast<f32>(actor->m320.z), x = static_cast<f32>(actor->m320.x);
        (actor->m392 = phase);
        f32 sx = cM_ssin(phase), sz = cM_ssin(phase);
        f32 targetX = fmadds(sx, 50.f, add(x, 50.f)), targetZ = fmadds(sz, 50.f, add(z, 50.f));
        Pw_cLib_addCalc2(pwAddress(actor, offsetof(pw_class, current.pos)), targetX, 1.f, 5.f);
        Pw_cLib_addCalc2(pwAddress(actor, offsetof(pw_class, current.pos.z)), targetZ, 1.f, 5.f);
    }
}

/* 0244AE40 */
void kyori_sub(pw_class* actor) {
    u32 a = ea(actor), play = Pw_dComIfGp_get(), player = static_cast<u32>(pwViewPwPlayView(play)->mpPlayer);
    play = Pw_dComIfGp_get();
    if (Pw_fopAcM_searchActorDistance(a, static_cast<u32>(pwViewPwPlayView(play)->mpPlayer)) > 300.f)
        return;
    u32 matrix = l_calcMtx;
    play = Pw_dComIfGp_get();
    s32 angle = Pw_fopAcM_searchActorAngleY(a, static_cast<u32>(pwViewPwPlayView(play)->mpPlayer));
    Pw_mDoMtx_YrotS(matrix, angle);

    Local<cXyz> offset, near, far;
    offset->x = 0.f;
    offset->y = 0.f;
    offset->z = -300.f;
    Pw_MtxPosition(gabi::ea(offset.get()), gabi::ea(near.get()));
    Pw_PSVECAdd(gabi::ea(near.get()), player + 0x314, gabi::ea(near.get()));
    play = Pw_dComIfGp_get();
    if (Pw_dDetect_c_chk_light(play + 0x5A20, gabi::ea(near.get())))
        return;
    offset->x = 0.f;
    offset->y = 0.f;
    offset->z = -400.f;
    Pw_MtxPosition(gabi::ea(offset.get()), gabi::ea(far.get()));
    Pw_PSVECAdd(gabi::ea(far.get()), player + 0x314, gabi::ea(far.get()));
    play = Pw_dComIfGp_get();
    if (Pw_dDetect_c_chk_light(play + 0x5A20, gabi::ea(far.get())))
        return;
    f32 speed = __builtin_fabsf(static_cast<f32>(pwViewfopAc_ac_c(player)->speedF));
    f32 volume = (f32)((double)speed * 4.f);
    u32 level =
        !(volume < 2147483648.f) ? (u32)ftoi(sub(volume, 2147483648.f)) + 0x80000000u : (u32)ftoi(volume);
    if (level > 100)
        level = 100;
    if (a && pwAddress(actor, offsetof(pw_class, eyePos))) {
        s32 reverb = Pw_dComIfGp_getReverb((s32) static_cast<s8>(actor->current.roomNo));
        Pw_cc_seStart(0x7051u, pwAddress(actor, offsetof(pw_class, eyePos)), level, reverb);
    }
    f32 step = (f32)((double)speed * 1.5f);
    Pw_cLib_addCalc2(pwAddress(actor, offsetof(pw_class, current.pos)), (f32)near->x, 1.f, step);
    Pw_cLib_addCalc2(pwAddress(actor, offsetof(pw_class, current.pos.z)), (f32)near->z, 1.f, step);
}

// HD: target is passed by guest address; the GC source used a value argument.
/* 0244A7F0 */
s32 Line_check(pw_class* actor, u32 target, s32 mode) {
    u32 a = ea(actor);
    Local<dBgS_LinChk> line;
    u32 l = ea(line.get());
    Pw_cBgS_LinChk_ct(gabi::ea(line.get()));
    (line->cBgS_Chk::__vtbl = PW_LINE_CHECK_VTABLE);
    (line->mbCamThrough = 0);
    f32 x = static_cast<f32>(pwViewcXyz(target)->x), y = static_cast<f32>(pwViewcXyz(target)->y),
        z = static_cast<f32>(pwViewcXyz(target)->z);
    (actor->m314.x = x);
    (actor->m314.y = y);
    (line->mGrpPassChk = l + 0x64);
    (line->mbArrowThrough = 0);
    (line->cBgS_PolyInfo::__vtbl = PW_LINE_POLYGON_VTABLE);
    (line->mbBombThrough = 0);
    (actor->m314.z = z);
    (line->mbBoomerangThrough = 0);
    (line->mbObjThrough = 0);
    (line->mbRopeThrough = 0);
    (line->mGrp = 1);
    (line->mPolyPassChk = l + 0x58);
    (line->mbLinkThrough = 0);
    (line->dBgS_PolyPassChk::__vtbl = PW_LINE_PASS_VTABLE);
    (line->dBgS_GrpPassChk::__vtbl = PW_LINE_GROUP_VTABLE);
    Local<cXyz> offset, sum;
    if (!mode) {
        Pw_mDoMtx_YrotS(l_calcMtx, (s32) static_cast<s16>(actor->current.angle.y));
        offset->x = 0.f;
        offset->y = 0.f;
        offset->z = 200.f;
        Pw_MtxPosition(gabi::ea(offset.get()), target);
        Pw_cXyz__pl(target, gabi::ea(sum.get()), pwAddress(actor, offsetof(pw_class, current.pos)));
        x = sum->x;
        y = sum->y;
        (actor->m314.x = x);
        z = sum->z;
        (actor->m314.y = y);
        (actor->m314.z = z);
    }
    y = add(y, 80.f);
    x = static_cast<f32>(actor->m314.x);
    (actor->m314.y = y);
    (pwViewcXyz(target)->x = x);
    (pwViewcXyz(target)->y = y);
    z = static_cast<f32>(actor->m314.z);
    (pwViewcXyz(target)->z = z);
    (pwVectorWord(&actor->m308, 0) = static_cast<u32>(pwVectorWord(&actor->current.pos, 0)));
    y = static_cast<f32>(actor->current.pos.y);
    u32 zbits = static_cast<u32>(pwVectorWord(&actor->current.pos, 2));
    (pwVectorWord(&actor->m308, 2) = zbits);
    (actor->m308.y = add(y, 80.f));
    Pw_dBgS_LinChk_Set(gabi::ea(line.get()), pwAddress(actor, offsetof(pw_class, m308)), target, a);
    u32 play = Pw_dComIfGp_get();
    bool hit = Pw_cBgS_LineCross(play + 0x12A0, gabi::ea(line.get())) != 0;
    if (hit) {
        s16 yaw = static_cast<s16>(actor->current.angle.y);
        (line->cBgS_PolyInfo::__vtbl = PW_LINE_POLYGON_BASE_VTABLE);
        (line->dBgS_GrpPassChk::__vtbl = PW_LINE_GROUP_BASE_VTABLE);
        (line->dBgS_PolyPassChk::__vtbl = PW_LINE_PASS_VTABLE);
        (actor->m38C = (u16)(yaw + 0x8000));
    } else {
        (line->dBgS_PolyPassChk::__vtbl = PW_LINE_PASS_VTABLE);
        (line->cBgS_PolyInfo::__vtbl = PW_LINE_POLYGON_BASE_VTABLE);
        (line->dBgS_GrpPassChk::__vtbl = PW_LINE_GROUP_BASE_VTABLE);
    }
    Pw_cBgS_LinChk_dt(gabi::ea(line.get()), 0);
    return hit;
}

/* 0244F730 */
void kantera_calc(pw_class* actor) {
    u32 a = ea(actor);
    if (static_cast<s16>(actor->m346) <= 0 || static_cast<s16>(actor->m382) != 0)
        return;
    u32 id = static_cast<u32>(actor->mKanteraID);
    if (id == 0xFFFFFFFF)
        return;
    Local<be<u32>> found;
    if (!Pw_fopAcM_SearchByID(id, gabi::ea(found.get())))
        return;
    u32 lantern = found->get();
    if (!lantern || static_cast<s16>(pwViewfopAc_ac_c(lantern)->mProcName) != 0xC1)
        return;
    (pwVectorWord(&pwViewfopAc_ac_c(lantern)->current.pos, 0) =
         static_cast<u32>(pwVectorWord(&actor->m2CC, 0)));
    (pwVectorWord(&pwViewfopAc_ac_c(lantern)->current.pos, 1) =
         static_cast<u32>(pwVectorWord(&actor->m2CC, 1)));
    (pwVectorWord(&pwViewfopAc_ac_c(lantern)->current.pos, 2) =
         static_cast<u32>(pwVectorWord(&actor->m2CC, 2)));
    (pwViewfopAc_ac_c(found->get())->current.angle.y = static_cast<s16>(actor->shape_angle.y));
    u8 mode = static_cast<u8>(actor->m341);
    if (mode == 0 || mode == 1) {
        Pw_cLib_addCalcAngleS2(pwAddress(actor, offsetof(pw_class, mFloatPhaseStep)), mode == 0 ? 3000 : 4000,
                               1, 100);
        Pw_cLib_addCalc2(pwAddress(actor, offsetof(pw_class, m3B0)), mode == 0 ? 5000.f : 10000.f, 1.f,
                         1000.f);
        Pw_cLib_addCalc2(pwAddress(actor, offsetof(pw_class, mLightRadius)), mode == 0 ? -1000.f : -10000.f,
                         1.f, 1000.f);
        mode = static_cast<u8>(actor->m341);
    }
    if (mode != 2) {
        u16 angle = (u16)(static_cast<s16>(actor->mFloatPhase) + static_cast<s16>(actor->mFloatPhaseStep));
        f32 amplitude = static_cast<f32>(actor->m3B0);
        (actor->mFloatPhase = angle);
        f32 sine = cM_ssin(angle);
        (pwViewPwLanternTail(lantern)->mJointRot[1].x = (s16)ftoi((f32)((double)sine * amplitude)));
        angle = static_cast<u16>(actor->mFloatPhase);
        f32 cosine = cM_scos(angle), depth = static_cast<f32>(actor->mLightRadius);
        (pwViewPwLanternTail(lantern)->mJointRot[1].z = (s16)ftoi((f32)((double)cosine * depth)));
        (pwViewfopAc_ac_c(found->get())->attention_info.flags = 0);
    }
    (pwVectorWord(&actor->m2D8, 0) =
         static_cast<u32>(pwVectorWord(&pwViewPwLanternTail(lantern)->mBonPos, 0)));
    (pwVectorWord(&actor->m2D8, 1) =
         static_cast<u32>(pwVectorWord(&pwViewPwLanternTail(lantern)->mBonPos, 1)));
    (pwVectorWord(&actor->m2D8, 2) =
         static_cast<u32>(pwVectorWord(&pwViewPwLanternTail(lantern)->mBonPos, 2)));
}

/* 0244DE14 */
s32 useHeapInit(pw_class* actor) {
    u32 a = ea(actor);
    struct String {
        be<u32> data, vtable;
    };
    Local<String> archive[6];
    auto resource = [&](u32 slot, s32 index) {
        archive[slot]->data = gabi::guest_str("Pw");
        archive[slot]->vtable = gc_tu.safeString;
        return Pw_dRes_control_c_getRes(l_resourceControl, gabi::ea(archive[slot].get()), index);
    };
    u32 modelData = resource(0, dRes_INDEX_PW_BDL_PW_e), animation = resource(1, dRes_INDEX_PW_BCK_WAIT1_e);
    u32 morf = Pw_mDoExt_McaMorf_mDoExt_McaMorf(0u, modelData, 0u, 0u, animation, 2, 1.f, 0, -1, 1, 0u,
                                                0x80000u, 0x37441422u);
    (actor->mpMorf = morf);
    if (!morf || !static_cast<u32>(pwViewmDoExt_McaMorf(morf)->mpModel.v))
        return 0;
    (pwViewJ3DModel(pwViewmDoExt_McaMorf(morf)->mpModel.v)->mUserArea = a);
    u32 info = Pw_J3DModelData_getJointTree_HD(static_cast<u32>(
        pwViewJ3DModel(static_cast<u32>(pwViewmDoExt_McaMorf(actor->mpMorf)->mpModel.v))->mModelData.v));
    u16 index = 0;
    morf = static_cast<u32>(actor->mpMorf);
    while (index < static_cast<u16>(pwViewPwJointTreeView(info)->mJointNum)) {
        u32 table = static_cast<u32>(
            pwViewJ3DModel(static_cast<u32>(pwViewmDoExt_McaMorf(morf)->mpModel.v))->mModelData.v);
        u32 count = static_cast<u32>(pwViewJ3DModelData(table)->mJointNum),
            entry = static_cast<u32>(pwViewJ3DModelData(table)->mJointNodes);
        if (index < count)
            entry += index * 0x1C;
        (pwViewJ3DJoint(entry)->mCallBack = gabi::guest_fn((const void*)nodeCallBack));
        morf = static_cast<u32>(actor->mpMorf);
        index = (u16)(index + 1);
        info = Pw_J3DModelData_getJointTree_HD(static_cast<u32>(
            pwViewJ3DModel(static_cast<u32>(pwViewmDoExt_McaMorf(morf)->mpModel.v))->mModelData.v));
        morf = static_cast<u32>(actor->mpMorf);
    }
    u32 model = static_cast<u32>(pwViewmDoExt_McaMorf(morf)->mpModel.v), btk = Pw_operator_new(0x74);
    if (btk)
        btk = Pw_mDoExt_btpAnm_ct(btk);
    (actor->m2BC = btk);
    if (!btk)
        return 0;
    u32 currentModel = static_cast<u32>(pwViewmDoExt_McaMorf(actor->mpMorf)->mpModel.v),
        btkData = resource(2, dRes_INDEX_PW_BTP_IROGAE1_e);
    if (!Pw_mDoExt_btpAnm_init(static_cast<u32>(actor->m2BC),
                               static_cast<u32>(pwViewJ3DModel(currentModel)->mModelData.v), btkData, 1, 1,
                               1.f, 0, -1, 0, 0))
        return 0;

    const s32 resources[3] = {dRes_INDEX_PW_BRK_JITTAIKA1_e, dRes_INDEX_PW_BRK_HIT_e,
                              dRes_INDEX_PW_BRK_DEFAULT_e};
    for (u32 i = 0; i < 3; ++i) {
        u32 brk = Pw_operator_new(0x78);
        if (brk)
            brk = Pw_mDoExt_brkAnm_ct(brk);
        (pwRegisterAnimation(actor, i) = brk);
        if (!brk)
            return 0;
        u32 data = resource(i + 3, resources[i]);
        if (!Pw_mDoExt_brkAnm_init(static_cast<u32>(pwRegisterAnimation(actor, i)),
                                   static_cast<u32>(pwViewJ3DModel(model)->mModelData.v), data, 1, i ? 2 : 0,
                                   1.f, 0, -1, 0, 0))
            return 0;
    }
    return Pw_mDoExt_invisibleModel_create(
               pwAddress(actor, offsetof(pw_class, mInvisibleModel)),
               static_cast<u32>(pwViewmDoExt_McaMorf(actor->mpMorf)->mpModel.v)) != 0;
}

/* 0244FE24 */
void action_torituku(pw_class* actor) {
    u32 a = ea(actor), play = Pw_dComIfGp_get(), player = static_cast<u32>(pwViewPwPlayView(play)->mpPlayer);
    play = Pw_dComIfGp_get();
    s16 mode = static_cast<s16>(actor->mMode);
    f32 playerZ = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.z);
    u32 controlled = static_cast<u32>(pwViewPwPlayView(play)->mpControlPlayer);
    f32 playerX = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.x);
    if (mode != 0x50 && mode != 0x51) {
        Pw_cLib_addCalcAngleS2(pwAddress(actor, offsetof(pw_class, m39A)), 255, 1, 10);
        return;
    }
    auto sound = [&](u32 id) {
        s32 reverb = Pw_dComIfGp_getReverb((s32) static_cast<s8>(actor->current.roomNo));
        Pw_cc_seStart(id, pwAddress(actor, offsetof(pw_class, eyePos)), 0, reverb);
    };
    if (mode == 0x50) {
        for (u32 i = 0; i < 4; ++i)
            pwDamageTimer(actor, i) = 0;
        u32 flags = static_cast<u32>(actor->actor_status);
        mode = static_cast<s16>(actor->mMode);
        (actor->m378 = 300);
        (actor->m33E = 1);
        (actor->actor_status = flags | 0x4000);
        (actor->mMode = (s16)(mode + 1));
        if (pwAddress(actor, offsetof(pw_class, eyePos)))
            sound(0x107F);
    } else if (a && pwAddress(actor, offsetof(pw_class, eyePos)))
        sound(0x107F);
    (actor->m384 = (s16)(static_cast<s16>(actor->m384) + 700));
    (actor->shape_angle.y = (s16)(static_cast<s16>(actor->shape_angle.y) + 1000));
    bool leave = true;
    if (player == controlled && static_cast<s16>(actor->m378) != 0) {
        play = Pw_dComIfGp_get();
        if (!Pw_dDetect_c_chk_light(play + 0x5A20, pwAddress(actor, offsetof(pw_class, current.pos)))) {
            play = Pw_dComIfGp_get();
            if (!(static_cast<u32>(pwViewPwPlayView(play)->mButtonHold) & 0x2000) &&
                static_cast<s16>(pwViewdaPy_py_c(controlled)->mDamageWaitTimer) == 0 &&
                !(static_cast<u32>(pwViewdaPy_py_c(controlled)->mResetFlg0) & 0x2000000))
                leave = false;
        }
    }
    bool hit = false;
    if (Pw_dCcD_GObjInf_ChkTgHit(pwAddress(actor, offsetof(pw_class, mCyl)))) {
        u32 object = Pw_dCcD_GObjInf_GetTgHitObj(pwAddress(actor, offsetof(pw_class, mCyl)));
        if (object && (static_cast<u32>(pwViewdCcD_GObjInf(object)->mAtType) & 0x800000))
            hit = true;
    }
    if (hit || leave) {
        if (static_cast<u8>(actor->m343)) {
            (actor->m343 = 0);
            (TORITUKI_ON = 0);
            if (pwAddress(actor, offsetof(pw_class, eyePos)))
                sound(0x5933);
        }
        (actor->m38E = 0);
        (actor->m39A = 255);
        (actor->m38A = 1);
        (pwViewdaPy_py_c(controlled)->mNoResetFlg1 =
             static_cast<u32>(pwViewdaPy_py_c(controlled)->mNoResetFlg1) & ~0x100u);
        (actor->mMode = 0x3C);
        (actor->mAction = PW_ACTION_ITAI);
    } else {
        (pwViewdaPy_py_c(controlled)->mNoResetFlg1 =
             static_cast<u32>(pwViewdaPy_py_c(controlled)->mNoResetFlg1) | 0x100);
        u16 phase = static_cast<u16>(actor->m384);
        f32 y = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.y);
        f32 sine = cM_ssin(phase);
        (actor->current.pos.z = playerZ);
        (actor->current.pos.x = playerX);
        (actor->current.pos.y = fmadds(sine, 10.f, add(y, 100.f)));
    }
    Pw_cLib_addCalcAngleS2(pwAddress(actor, offsetof(pw_class, m39A)), 255, 1, 10);
}

/* 02449F80 */
s32 daPW_Draw(pw_class* actor) {
    u32 a = ea(actor), model = static_cast<u32>(pwViewmDoExt_McaMorf(actor->mpMorf)->mpModel.v),
        data = static_cast<u32>(pwViewJ3DModel(model)->mModelData.v);
    u32 lighting = Pw_dKy_getEnvlight();
    Pw_dScnKy_env_light_setLightTevColorType_HD(lighting, model,
                                                pwAddress(actor, offsetof(pw_class, tevStr)));
    Pw_dSnap_RegistFig(0xB8, a, 1.f, 1.f, 1.f);
    u32 info = Pw_J3DMaterial_getShapePacketModelData_HD(data);
    u16 index = 0;
    struct Color {
        be<f32> r, g, b, a;
    };
    Local<Color> linear, converted;
    while (index < static_cast<u16>(pwViewPwJointTreeView(info)->mMaterialNum)) {
        u32 count = static_cast<u32>(pwViewJ3DModelData(data)->mMaterialNum),
            material = static_cast<u32>(pwViewJ3DModelData(data)->mMaterials);
        if (index < count)
            material += index * 0x39C;
        u32 object = static_cast<u32>(pwViewPwMaterialView(material)->mpColorBlock);
        u32 color = pwGetKonstColor(object, 3);
        (pwViewGXColor(color)->a = (u8) static_cast<s16>(actor->m39A));
        object = static_cast<u32>(pwViewPwMaterialView(material)->mpColorBlock);
        color = pwGetKonstColor(object, 3);
        object = static_cast<u32>(pwViewPwMaterialView(material)->mpColorBlock);
        pwSetKonstColor(object, 3, color);
        Pw_GXColor_to_float(gabi::ea(linear.get()), color);
        Pw_GXColor4f_scaleRGB(gabi::ea(converted.get()), gabi::ea(linear.get()), 1.f);
        (pwViewPwMaterialView(material)->mChanged =
             static_cast<u32>(pwViewPwMaterialView(material)->mChanged) | 0x400);
        u32 output = Pw_J3DMaterial_getColorReg_HD(material + 0xA0, 10);
        f32 alpha = (f32)((double)(f32) static_cast<u8>(pwViewGXColor(color)->a) / 255.f);
        f32 b = converted->b, g = converted->g, r = converted->r;
        (pwViewPwFloatColor(output)->g = g);
        (pwViewPwFloatColor(output)->b = b);
        (pwViewPwFloatColor(output)->r = r);
        (pwViewPwFloatColor(output)->a = alpha);
        index = (u16)(index + 1);
        info = Pw_J3DMaterial_getShapePacketModelData_HD(data);
    }
    u32 play;
    if (!static_cast<u8>(actor->m33E)) {
        play = Pw_dComIfGp_get();
        (l_drawBuffers.opaque = static_cast<u32>(pwViewPwPlayView(play)->mpMaskOffOpa));
        play = Pw_dComIfGp_get();
        (l_drawBuffers.translucent = static_cast<u32>(pwViewPwPlayView(play)->mpMaskOffXlu));
        Pw_J3DModel_setModelXlu_HD(model, 1);
    } else
        Pw_J3DModel_setModelXlu_HD(model, 0);
    if (static_cast<s16>(actor->mEnemyIce.mFreezeTimer) > 20) {
        Pw_dMat_ice_c_entryDL(static_cast<u32>(actor->mpMorf), -1,
                              pwAddress(actor, offsetof(pw_class, mInvisibleModel)));
        play = Pw_dComIfGp_get();
        (l_drawBuffers.opaque = static_cast<u32>(pwViewPwPlayView(play)->mpOpa));
        play = Pw_dComIfGp_get();
        (l_drawBuffers.translucent = static_cast<u32>(pwViewPwPlayView(play)->mpXlu));
        return 1;
    }
    Pw_dComIfGp_get();
    u8 visible = static_cast<u8>(actor->m33E);
    data = static_cast<u32>(pwViewJ3DModel(model)->mModelData.v);
    u32 brk =
        static_cast<u32>(visible ? actor->m2C0 : (static_cast<u8>(actor->m33F) ? actor->m2C4 : actor->m2C8));
    Pw_mDoExt_brkAnm_entry(brk, data, static_cast<f32>(pwViewJ3DFrameCtrl(brk)->mFrame));
    u32 btk = static_cast<u32>(actor->m2BC);
    s16 frame = (s16)ftoi(static_cast<f32>(pwViewJ3DFrameCtrl(btk)->mFrame));
    Pw_mDoExt_btpAnm_entry(btk, static_cast<u32>(pwViewJ3DModel(model)->mModelData.v), (s32)frame);
    u8 texture = static_cast<u8>(actor->mColorIndex);
    (pwViewJ3DFrameCtrl(actor->m2BC)->mFrame = (f32)texture);
    Pw_mDoExt_McaMorf_entryDL(static_cast<u32>(actor->mpMorf));
    // HD: retain the visibility reload before clearing material animation slots.
    (void)static_cast<u8>(actor->m33E);
    (pwViewPwMaterialAnimationSlots(pwViewJ3DModel(model)->mModelData.v)->mpBrk = 0);
    (pwViewPwMaterialAnimationSlots(pwViewJ3DModel(model)->mModelData.v)->mpBtp = 0);
    play = Pw_dComIfGp_get();
    (l_drawBuffers.opaque = static_cast<u32>(pwViewPwPlayView(play)->mpOpa));
    play = Pw_dComIfGp_get();
    (l_drawBuffers.translucent = static_cast<u32>(pwViewPwPlayView(play)->mpXlu));
    if (static_cast<u8>(actor->m33E))
        Pw_invisibleEntry(a + offsetof(pw_class, mInvisibleModel));
    else
        Pw_invisibleEntryMaskOff(a + offsetof(pw_class, mInvisibleModel));
    return 1;
}

/* 0244E398 */
s32 daPW_Create(pw_class* actor) {
    u32 a = ea(actor), flags = static_cast<u32>(actor->actor_condition);
    if (!(flags & 8)) {
        if (a) {
            Pw_pw_class_ct(a);
            flags = static_cast<u32>(actor->actor_condition);
        }
        (actor->actor_condition = flags | 8);
    }
    s32 phase = Pw_dComIfG_resLoad(pwAddress(actor, offsetof(pw_class, mPhase)), gabi::guest_str("Pw"));
    if (phase != 4)
        return phase;
    u32 parameters = static_cast<u32>(actor->mParameters);
    (actor->mBehaviorType = (u8)parameters);
    (actor->mHoversAtInitialYPos = (parameters >> 8) & 1);
    (actor->mNoticeRangeParam = (parameters >> 16) & 255);
    (actor->mColorIndex = (parameters >> 9) & 127);
    (actor->mPathIndex = (u8)(parameters >> 24));
    if ((u8)parameters == 255)
        (actor->mBehaviorType = 0);
    u8 radius = (parameters >> 16) & 255;
    (actor->mNoticeRange = radius == 255 ? 1000.f : (f32)((double)(f32)radius * 10.f));
    if (static_cast<u8>(actor->mColorIndex) == 127)
        (actor->mColorIndex = 0);
    if (!Pw_fopAcM_entrySolidHeap(a, gabi::guest_fn((const void*)useHeapInit), 0x2540))
        return 5;
    (actor->gbaName = 13);
    (actor->m3AC = add(static_cast<f32>(actor->mNoticeRange), 5000.f));
    s16 overrideType = l_behaviorOverride;
    u8 path = static_cast<u8>(actor->mPathIndex);
    if (overrideType)
        (actor->mBehaviorType = (u8)(overrideType - 1));
    if (path != 255)
        (actor->mpPath = Pw_dPath_GetRoomPath((u32)path, (s32) static_cast<s8>(actor->current.roomNo)));
    u8 texture = static_cast<u8>(actor->mColorIndex);
    u32 x = static_cast<u32>(pwVectorWord(&actor->current.pos, 0)),
        y = static_cast<u32>(pwVectorWord(&actor->current.pos, 1));
    if (texture > 5)
        (actor->mColorIndex = 0);
    (pwVectorWord(&actor->m2CC, 0) = x);
    u32 z = static_cast<u32>(pwVectorWord(&actor->current.pos, 2));
    (pwVectorWord(&actor->m2CC, 1) = y);
    (pwVectorWord(&actor->m2CC, 2) = z);
    u32 play = Pw_dComIfGp_get();
    (actor->itemTableIdx = Pw_cDT_NamePTbl_GetIndex(play + 0x50AC, gabi::guest_str("pow"), 0));
    x = static_cast<u32>(pwVectorWord(&actor->current.pos, 0));
    (actor->max_health = 4);
    (actor->health = 4);
    y = static_cast<u32>(pwVectorWord(&actor->current.pos, 1));
    (pwVectorWord(&actor->m2F0, 0) = x);
    (pwVectorWord(&actor->m2F0, 1) = y);
    (actor->m398 = 4);
    z = static_cast<u32>(pwVectorWord(&actor->current.pos, 2));
    u32 morf = static_cast<u32>(actor->mpMorf);
    (pwVectorWord(&actor->m2F0, 2) = z);
    (actor->m39E = 0x1000);
    u32 model = static_cast<u32>(pwViewmDoExt_McaMorf(morf)->mpModel.v);
    if (model)
        model += 0xC8;
    (actor->cullMtx = model);
    Pw_fopAcM_setCullSizeBox(a, -100.f, -50.f, -50.f, 100.f, 200.f, 100.f);
    (actor->attention_info.flags = 4);
    Pw_dBgS_Acch_Set(
        pwAddress(actor, offsetof(pw_class, mAcch)), pwAddress(actor, offsetof(pw_class, current.pos)),
        pwAddress(actor, offsetof(pw_class, old.pos)), a, 1, pwAddress(actor, offsetof(pw_class, mAcchCir)),
        pwAddress(actor, offsetof(pw_class, speed)), 0, 0);
    Pw_dCcD_Stts_Init(pwAddress(actor, offsetof(pw_class, mStts)), 0, 1, a);
    (actor->mEnemyIce.mpActor = a);
    (actor->mEnemyIce.mHeight = 50.f);
    (actor->mEnemyFire.mpMorf = static_cast<u32>(actor->mpMorf));
    (actor->mEnemyFire.mpActor = a);
    (actor->mEnemyIce.mScale = 200.f);
    for (u32 i = 0; i < 10; ++i) {
        (actor->mEnemyFire.mColors[i] = l_fireColor[i]);
        (actor->mEnemyFire.mSizes[i] = l_fireSize[i]);
    }
    Pw_dCcD_Cyl_Set(pwAddress(actor, offsetof(pw_class, mCyl)), gabi::ea(&l_cyl_src));
    (actor->mCyl.mStts.v = pwAddress(actor, offsetof(pw_class, mStts)));
    Pw_dCcD_Sph_Set(pwAddress(actor, offsetof(pw_class, mSph)), gabi::ea(&l_sph_src));
    x = static_cast<u32>(pwVectorWord(&actor->current.pos, 0));
    s16 yaw = static_cast<s16>(actor->current.angle.y);
    (actor->mJalhallaID = 0xFFFFFFFF);
    (actor->mKanteraID = 0xFFFFFFFF);
    (actor->mSph.mStts.v = pwAddress(actor, offsetof(pw_class, mStts)));
    (actor->m38C = yaw);
    u32 sphere = static_cast<u32>(actor->mSph.mObjAt.mSPrm);
    (actor->stealItemLeft = 1);
    (actor->mAction = PW_ACTION_DOUSA);
    (pwVectorWord(&actor->m2D8, 0) = x);
    u32 cylinder = static_cast<u32>(actor->mCyl.mGObjTg.mSPrm);
    u8 type = static_cast<u8>(actor->mBehaviorType);
    (actor->mMode = 10);
    y = static_cast<u32>(pwVectorWord(&actor->current.pos, 1));
    (actor->m346 = -1);
    (pwVectorWord(&actor->m2D8, 1) = y);
    (actor->mSph.mObjAt.mSPrm = sphere & ~1u);
    z = static_cast<u32>(pwVectorWord(&actor->current.pos, 2));
    (actor->mCyl.mGObjTg.mSPrm = cylinder & ~2u);
    (pwVectorWord(&actor->m2D8, 2) = z);
    if (type == 1 || type == 2) {
        u32 status = static_cast<u32>(actor->actor_status);
        (actor->attention_info.flags = 0);
        (actor->mMode = type == 1 ? 0 : 9);
        (actor->actor_status = status & ~0x20u);
    } else if (type == 3 || type == 4) {
        u32 parent = static_cast<u32>(actor->parentActorID);
        (actor->mJalhallaID = parent);
        if (parent == 0xFFFFFFFF)
            return 5;
        cylinder = static_cast<u32>(actor->mCyl.mGObjTg.mSPrm);
        type = static_cast<u8>(actor->mBehaviorType);
        (actor->m39A = 255);
        (actor->m38E = 0);
        (actor->max_health = 4);
        (actor->mCyl.mTgType = 0xFF3DFEFF);
        (actor->mCyl.mGObjTg.mSPrm = cylinder | 2);
        (actor->m346 = 0);
        (actor->health = 4);
        if (type == 3) {
            (actor->mAction = PW_ACTION_DOUSA);
            (actor->mMode = 0x6E);
            (actor->m33E = 1);
        } else {
            (actor->actor_status = static_cast<u32>(actor->actor_status) | 0x4000);
            (actor->m390 = (s16)ftoi(Pw_cM_rndFX(16384.f)));
            f32 random = Pw_cM_rndFX(150.f);
            (actor->current.pos.x = add(static_cast<f32>(actor->current.pos.x), random));
            random = Pw_cM_rndFX(150.f);
            f32 currentZ = static_cast<f32>(actor->current.pos.z);
            (actor->mAction = PW_ACTION_BIG_DEMO);
            (actor->mMode = 150);
            (actor->current.pos.z = add(currentZ, random));
        }
    }
    Pw_BG_check(a);
    Pw_draw_SUB(a);
    return phase;
}

/* 0244F960 */
void action_demo(pw_class* actor) {
    u32 a = ea(actor), play = Pw_dComIfGp_get(), player = static_cast<u32>(pwViewPwPlayView(play)->mpPlayer);
    play = Pw_dComIfGp_get();
    s32 cameraIndex = static_cast<s8>(pwViewPwPlayView(play)->mCameraId);
    play = Pw_dComIfGp_get();
    f32 playerY = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.y);
    s16 mode = static_cast<s16>(actor->mMode);
    f32 playerX = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.x),
        playerZ = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.z);
    u32 camera = static_cast<u32>(pwViewPwPlayView(play + (u32)(cameraIndex * 0x34))->mpCamera);
    if ((u32)(s32)mode < 0x46u || (u32)(s32)mode > 0x49u)
        return;
    if (mode == 0x46) {
        u32 flags = static_cast<u32>(actor->actor_status);
        s16 current = static_cast<s16>(actor->mMode);
        (actor->actor_status = flags | 0x4000);
        (actor->mMode = (s16)(current + 1));
        mode = 0x47;
    }
    if (mode == 0x47) {
        if (static_cast<u16>(actor->eventInfo.mCommand) != 2) {
            play = Pw_dComIfGp_get();
            (pwViewPwPlayView(play)->mCollisionFlags =
                 static_cast<u16>(pwViewPwPlayView(play)->mCollisionFlags) | 1);
            Pw_fopAcM_orderPotentialEvent(a, 2, 65535, 0);
            (actor->eventInfo.mCondition = static_cast<u16>(actor->eventInfo.mCondition) | 2);
            return;
        }
        (pwViewdaPy_py_c(player)->mDemo.mParam0 = 0);
        (pwViewdaPy_py_c(player)->mDemo.mDemoType = 3);
        (pwViewdaPy_py_c(player)->mDemo.mDemoMode = 1);
        Pw_dCamera_c_Stop(camera + 0x248);
        Pw_dCamera_c_SetTrimSize(camera + 0x248, 2);
        play = Pw_dComIfGp_get();
        (actor->m38C = static_cast<s16>(
             Pw_fopAcM_searchActorAngleY(a, static_cast<u32>(pwViewPwPlayView(play)->mpPlayer))));
        if (pwAddress(actor, offsetof(pw_class, eyePos))) {
            u32 id = a ? static_cast<u32>(actor->mBsPcId) : 0xFFFFFFFF;
            s32 reverb = Pw_dComIfGp_getReverb((s32) static_cast<s8>(actor->current.roomNo));
            Pw_fopAcM_monsSeStart_HD(0x48FD, pwAddress(actor, offsetof(pw_class, eyePos)), id, 0, reverb);
            reverb = Pw_dComIfGp_getReverb((s32) static_cast<s8>(actor->current.roomNo));
            Pw_cc_seStart(0x5932, pwAddress(actor, offsetof(pw_class, eyePos)), 0, reverb);
        }
        Pw_anm_init(a, 0x21, 4.f, 0, 1.f, -1);
        (actor->mMode = (s16)(static_cast<s16>(actor->mMode) + 1));
        return;
    }
    if (mode == 0x48) {
        Pw_cLib_addCalc2(pwAddress(actor, offsetof(pw_class, current.pos)), playerX, 1.f, 5.f);
        Pw_cLib_addCalc2(pwAddress(actor, offsetof(pw_class, current.pos.z)),
                         static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.z), 1.f, 5.f);
        u32 morf = static_cast<u32>(actor->mpMorf);
        if (!(static_cast<u8>(pwViewmDoExt_McaMorf(morf)->mFrameCtrl.mState) & 1) &&
            static_cast<f32>(pwViewmDoExt_McaMorf(morf)->mFrameCtrl.mRate) != 0.f)
            return;
        Pw_anm_init(a, 0x22, 4.f, 2, 1.f, -1);
        (actor->shape_angle.y = 0);
        (actor->shape_angle.z = 0);
        (actor->current.pos.z = playerZ);
        (actor->m340 = 1);
        (actor->attention_info.flags = 0);
        (actor->current.pos.y = add(playerY, 100.f));
        (actor->current.pos.x = playerX);
        (actor->shape_angle.x = 0);
        s16 yaw = static_cast<s16>(pwViewfopAc_ac_c(player)->shape_angle.y),
            current = static_cast<s16>(actor->mMode);
        (actor->shape_angle.y = yaw);
        (actor->m378 = 30);
        (actor->mMode = (s16)(current + 1));
        return;
    }
    if (static_cast<s16>(actor->m378) != 0)
        return;
    Pw_dComIfGp_get();
    play = Pw_dComIfGp_get();
    Pw_dVibration_c_StopQuake(play + 0x599C, 32);
    Pw_dCamera_c_Start(camera + 0x248);
    Pw_dCamera_c_SetTrimSize(camera + 0x248, 0);
    (pwViewdaPy_py_c(player)->mDemo.mDemoType = 2);
    (pwViewdaPy_py_c(player)->mDemo.mDemoMode = 1);
    play = Pw_dComIfGp_get();
    (pwViewPwPlayView(play)->mCollisionFlags = static_cast<u16>(pwViewPwPlayView(play)->mCollisionFlags) | 8);
    u32 flags = static_cast<u32>(actor->actor_status);
    (actor->mMode = 0x50);
    (actor->mAction = PW_ACTION_TORITUKU);
    (actor->actor_status = flags & ~0x4000u);
}

/* 0244F28C */
s32 kantera_atari_check(pw_class* actor) {
    u32 a = ea(actor), play = Pw_dComIfGp_get();
    s16 held = static_cast<s16>(actor->m346);
    u32 player = static_cast<u32>(pwViewPwPlayView(play)->mpPlayer);
    if (held <= 0)
        return 0;
    u32 id = static_cast<u32>(actor->mKanteraID);
    if (id == 0xFFFFFFFF)
        return 0;
    Local<be<u32>> found;
    if (!Pw_fopAcM_SearchByID(id, gabi::ea(found.get())))
        return 0;
    u32 lantern = found->get();
    if (!lantern || static_cast<s16>(pwViewfopAc_ac_c(lantern)->mProcName) != 0xC1)
        return 0;
    s16 before = static_cast<s16>(actor->m398);
    Pw_dCcD_GStts_Move(pwAddress(actor, offsetof(pw_class, mStts.__vtbl_gstts)));
    (actor->_459 = 0);
    auto sound = [&](u32 id) {
        if (a && pwAddress(actor, offsetof(pw_class, eyePos))) {
            s32 reverb = Pw_dComIfGp_getReverb((s32) static_cast<s8>(actor->current.roomNo));
            Pw_cc_seStart(id, pwAddress(actor, offsetof(pw_class, eyePos)), 64, reverb);
        }
    };
    s16 health;
    if (!Pw_dCcD_GObjInf_ChkTgHit(pwAddress(actor, offsetof(pw_class, mSph)))) {
        health = static_cast<s16>(actor->m398);
        (actor->m342 = 0);
    } else {
        if (static_cast<u8>(actor->m342))
            return 0;
        u32 object = Pw_dCcD_GObjInf_GetTgHitObj(pwAddress(actor, offsetof(pw_class, mSph)));
        if (!object)
            return 0;
        (actor->m342 = 1);
        u32 type = static_cast<u32>(pwViewdCcD_GObjInf(object)->mAtType);
        if (type == 2) {
            sound(0x2803);
            health = (s16)(static_cast<s16>(actor->m398) - 1);
            (actor->m398 = health);
            u8 attack = static_cast<u8>(pwViewPwPlayerActionView(player)->mCutType);
            bool strong = (attack >= 6 && attack <= 10) || attack == 12 || (attack >= 14 && attack <= 16) ||
                          attack == 21 || attack == 23;
            if (strong) {
                health = (s16)(static_cast<s16>(actor->m398) - 1);
                (actor->_459 = 1);
                (actor->m398 = health);
            }
        } else if (type == 0x200000) {
            health = static_cast<s16>(actor->m398);
            (actor->_459 = 3);
        } else if (type == 0x40 || type == 0x80) {
            if (type == 0x40)
                (actor->_459 = 4);
            health = (s16)(static_cast<s16>(actor->m398) - 1);
            (actor->m398 = health);
            sound(0x2833);
            health = static_cast<s16>(actor->m398);
        } else if (type == 0x10000) {
            sound(0x2855);
            (actor->_459 = 7);
            if (static_cast<u8>(pwViewPwPlayerActionView(player)->mCutType) == 17)
                (actor->_459 = 8);
            health = 0;
            (actor->m398 = 0);
        } else if (type == 0x20) {
            health = (s16)(static_cast<s16>(actor->m398) - 2);
            (actor->_459 = 6);
            (actor->m398 = health);
        } else {
            if (type == 0x40000 || type == 0x80000 || type == 0x100000)
                (actor->m398 = (s16)(static_cast<s16>(actor->m398) - 2));
            health = (s16)(static_cast<s16>(actor->m398) - 1);
            (actor->_459 = 0);
            (actor->m398 = health);
            sound(0x2834);
            health = static_cast<s16>(actor->m398);
        }
    }
    if (before == health)
        return 0;
    if (health <= 0) {
        Local<cXyz> position, scale;
        f32 x = static_cast<f32>(actor->mSph.mGObjTg.mHitPos.x);
        (actor->m398 = 0);
        position->x = x;
        position->y = static_cast<f32>(actor->mSph.mGObjTg.mHitPos.y);
        position->z = static_cast<f32>(actor->mSph.mGObjTg.mHitPos.z);
        play = Pw_dComIfGp_get();
        Pw_dPa_control_c_set(static_cast<u32>(pwViewPwPlayView(play)->mpParticle), 0, 16,
                             gabi::ea(position.get()), 0, 0, 255, 0, -1, 0, 0, 0);
        scale->x = 1.f;
        scale->y = 1.f;
        scale->z = 1.f;
        play = Pw_dComIfGp_get();
        Pw_dPa_control_c_set(static_cast<u32>(pwViewPwPlayView(play)->mpParticle), 0, 15,
                             gabi::ea(position.get()), player + 0x328, gabi::ea(scale.get()), 255, 0, -1, 0,
                             0, 0);
    }
    (actor->mAction = PW_ACTION_ITAI);
    (actor->mMode = 50);
    return 1;
}

/* 0244E960 */
s32 body_atari_check(pw_class* actor) {
    u32 a = ea(actor), play = Pw_dComIfGp_get(), player = static_cast<u32>(pwViewPwPlayView(play)->mpPlayer);
    Pw_dCcD_GStts_Move(pwAddress(actor, offsetof(pw_class, mStts.__vtbl_gstts)));
    s8 health = static_cast<s8>(actor->health);
    (actor->mHitByWhat = 0);
    if (health <= 0 || static_cast<s16>(actor->mAction) == PW_ACTION_BIG_DEMO)
        return 0;
    play = Pw_dComIfGp_get();
    if (Pw_dDetect_c_chk_light(play + 0x5A20, pwAddress(actor, offsetof(pw_class, current.pos)))) {
        (actor->mHitByWhat = 9);
        if (static_cast<s16>(actor->m39A) != 255 && !static_cast<s16>(actor->m382) &&
            static_cast<s16>(actor->m346) != -1) {
            s16 mode = static_cast<s16>(actor->mMode);
            if (mode != 0x37 && mode != 0x3C && mode != 0x3D &&
                static_cast<u32>(actor->mJalhallaID) == 0xFFFFFFFF) {
                (actor->mAction = PW_ACTION_ITAI);
                (actor->mMode = 0x34);
                return 1;
            }
        }
    }
    if (!Pw_dCcD_GObjInf_ChkTgHit(pwAddress(actor, offsetof(pw_class, mCyl))))
        return 0;
    u32 object = Pw_dCcD_GObjInf_GetTgHitObj(pwAddress(actor, offsetof(pw_class, mCyl)));
    if (!object)
        return 0;
    Local<cXyz> position, scale;
    f32 x = static_cast<f32>(actor->mCyl.mGObjTg.mHitPos.x),
        z = static_cast<f32>(actor->mCyl.mGObjTg.mHitPos.z),
        y = static_cast<f32>(actor->mCyl.mGObjTg.mHitPos.y);
    position->z = z;
    position->y = y;
    position->x = x;
    Local<PwAttackInfo> attack;
    u32 at = ea(attack.get());
    (attack->mSound = 0);
    (attack->mpCollider = Pw_dCcD_GObjInf_GetTgHitObj(pwAddress(actor, offsetof(pw_class, mCyl))));
    u8 visible = static_cast<u8>(actor->m33E);
    u32 type = static_cast<u32>(pwViewdCcD_GObjInf(object)->mAtType);
    auto sound = [&](u32 id) {
        if (pwAddress(actor, offsetof(pw_class, eyePos))) {
            s32 reverb = Pw_dComIfGp_getReverb((s32) static_cast<s8>(actor->current.roomNo));
            Pw_cc_seStart(id, pwAddress(actor, offsetof(pw_class, eyePos)), 32, reverb);
        }
    };
    auto particle = [&](s32 id, u32 p, u32 rotation, u32 size) {
        u32 state = Pw_dComIfGp_get();
        Pw_dPa_control_c_set(static_cast<u32>(pwViewPwPlayView(state)->mpParticle), 0, id, p, rotation, size,
                             255, 0, -1, 0, 0, 0);
    };
    auto finalSpecial = [&]() -> s32 {
        u8 kind = static_cast<u8>(actor->mHitByWhat);
        if (kind != 4 && kind != 12)
            return 0;
        (actor->mAction = PW_ACTION_ITAI);
        (actor->mMode = 0x3A);
        return 1;
    };
    if (!visible) {
        if (type & 0x800000)
            (actor->mHitByWhat = 9);
        else if (type & 0x100000) {
            (actor->mEnemyIce.mAlpha = 1.f);
            (actor->mEnemyIce.mRadius = 80.f);
            (actor->mHitByWhat = 10);
            (actor->attention_info.flags = 0);
            (actor->mEnemyIce.mState = 1);
            (actor->m39A = 255);
            Pw_Big_pow_down_check(a);
            Pw_kantera_break(a);
        } else
            return finalSpecial();
    } else {
        switch (type) {
        case 0x800000:
            return finalSpecial();
        case 0x8000:
            sound(0x2834);
            (actor->mHitByWhat = 11);
            (actor->mMode = 27);
            return 0;
        case 0x200000:
            (actor->mHitByWhat = 3);
            (actor->mAction = PW_ACTION_ITAI);
            (actor->mMode = 0x38);
            return 1;
        case 0x8000000:
            if (static_cast<s8>(actor->stealItemLeft) > 0) {
                u8 previous = static_cast<u8>(actor->health);
                (actor->health = 10);
                (attack->mpCollider =
                     Pw_dCcD_GObjInf_GetTgHitObj(pwAddress(actor, offsetof(pw_class, mCyl))));
                Pw_cc_at_check(a, gabi::ea(attack.get()));
                (actor->health = previous);
            }
            particle(0x27B, pwAddress(actor, offsetof(pw_class, attention_info.position)), 0, 0);
            (actor->mHitByWhat = 12);
            sound(0x2834);
            return finalSpecial();
        case 0x40:
            (actor->mHitByWhat = 4);
            particle(0x27B, pwAddress(actor, offsetof(pw_class, attention_info.position)), 0, 0);
            sound(0x2833);
            return finalSpecial();
        case 0x80:
        case 0x1000000:
            sound(0x2833);
            break;
        case 2:
        case 0x400:
        case 0x800:
        case 0x4000000:
        case 0x10000000: {
            sound(0x2803);
            u8 move = static_cast<u8>(pwViewPwPlayerActionView(player)->mCutType);
            bool strong = (move >= 5 && move <= 10) || move == 12 || (move >= 14 && move <= 16) ||
                          move == 21 || move == 23 || (move >= 25 && move <= 27) ||
                          (move >= 30 && move <= 31);
            if (strong)
                (actor->mHitByWhat = 1);
            break;
        }
        case 0x10000:
            sound(0x2855);
            (actor->mHitByWhat = 7);
            if (static_cast<u8>(pwViewPwPlayerActionView(player)->mCutType) == 17)
                (actor->mHitByWhat = 8);
            break;
        case 0x20:
            (actor->mHitByWhat = 6);
            break;
        case 0x80000:
            (actor->mEnemyIce.mTimer = 200);
            Pw_enemy_fire_remove(pwAddress(actor, offsetof(pw_class, mEnemyFire)));
            (actor->mHitByWhat = 5);
            sound(0x2834);
            (actor->health = 0);
            (actor->attention_info.flags = 0);
            if (Pw_Big_pow_down_check(a))
                return finalSpecial();
            (actor->mAction = PW_ACTION_ITAI);
            (actor->mMode = 0x3C);
            return 1;
        case 0x100000:
            (actor->mEnemyIce.mRadius = 80.f);
            (actor->mEnemyIce.mState = 1);
            (actor->mHitByWhat = 5);
            (actor->attention_info.flags = 0);
            (actor->mEnemyIce.mAlpha = 1.f);
            sound(0x2834);
            Pw_Big_pow_down_check(a);
            return finalSpecial();
        case 0x200:
        case 0x40000:
            (actor->mEnemyFire.mTimer = 100);
            (actor->mHitByWhat = 0);
            sound(0x2834);
            break;
        default:
            (actor->mHitByWhat = 0);
            sound(0x2834);
            break;
        }
    }
    if (!static_cast<u8>(actor->m33E)) {
        if (static_cast<s16>(actor->mMode) != 0x35) {
            (actor->mAction = PW_ACTION_ITAI);
            (actor->mMode = 0x34);
        }
        return 1;
    }
    Pw_cc_at_check(a, gabi::ea(attack.get()));
    u8 kind = static_cast<u8>(actor->mHitByWhat);
    if (kind == 1 || kind == 7 || kind == 8 || static_cast<s8>(actor->health) <= 0) {
        if (Pw_Big_pow_down_check(a))
            return 0;
        particle(16, ea(position.get()), 0, 0);
        scale->x = 2.f;
        scale->y = 2.f;
        scale->z = 2.f;
        particle(15, ea(position.get()), player + 0x328, ea(scale.get()));
        if (static_cast<u8>(actor->mHitByWhat) == 7) {
            (actor->mAction = PW_ACTION_ITAI);
            (actor->gravity = -3.f);
            (actor->speedF = 0.f);
            (actor->mMode = 0x3E);
            Pw_anm_init(a, 0x1C, 3.f, 0, 1.f, -1);
            return 1;
        }
    } else
        particle(13, ea(position.get()), player + 0x328, 0);
    if (static_cast<s16>(actor->mMode) != 0x37) {
        (actor->mAction = PW_ACTION_ITAI);
        (actor->mMode = 0x36);
    }
    return 1;
}

/* 024500C0 */
void action_big_demo(pw_class* actor) {
    u32 a = ea(actor);
    Pw_dComIfGp_get();
    Local<be<u32>> id, found;
    u32 parentId = static_cast<u32>(actor->mJalhallaID), parent = 0;
    *id.get() = parentId;
    if (parentId != 0xFFFFFFFF) {
        parent = Pw_fopAcIt_Judge(PW_JUDGE_BY_ID, gabi::ea(id.get()));
        parentId = static_cast<u32>(actor->mJalhallaID);
    }
    *found.get() = parent;
    s32 searched = Pw_fopAcM_SearchByID(parentId, gabi::ea(found.get()));
    parent = found->get();
    if (!parent || (searched && static_cast<s16>(pwViewfopAc_ac_c(parent)->mProcName) != 0xD3))
        return;
    s16 mode = static_cast<s16>(actor->mMode);
    constexpr u32 parameters = 0x1047B608;
    auto animation = [&](s32 index, f32 blend, u32 loop, s32 sound) {
        Pw_anm_init(a, index, blend, loop, 1.f, sound);
    };
    auto advance = [&]() { (actor->mMode = (s16)(static_cast<s16>(actor->mMode) + 1)); };
    auto voice = [&](u32 id, bool pair) {
        if (!(pwAddress(actor, offsetof(pw_class, eyePos))))
            return;
        u32 process = a ? static_cast<u32>(actor->mBsPcId) : 0xFFFFFFFF;
        s32 reverb = Pw_dComIfGp_getReverb((s32) static_cast<s8>(actor->current.roomNo));
        Pw_fopAcM_monsSeStart_HD(id, pwAddress(actor, offsetof(pw_class, eyePos)), process, 0, reverb);
        if (pair) {
            reverb = Pw_dComIfGp_getReverb((s32) static_cast<s8>(actor->current.roomNo));
            Pw_cc_seStart(id == 0x48F4 ? 0x5930 : 0x5932, pwAddress(actor, offsetof(pw_class, eyePos)), 0,
                          reverb);
        }
    };
    auto frameVoice = [&]() {
        if (Pw_J3DFrameCtrl_checkPass(static_cast<u32>(actor->mpMorf) + 0x98, 0.f) && a)
            voice(0x48F1, false);
    };
    auto angleToParent = [&]() { return Pw_fopAcM_searchActorAngleY(a, found->get()); };
    if (mode == 150) {
        for (u32 i = 0; i < 4; ++i)
            pwDamageTimer(actor, i) = 0;
        (actor->m39A = 255);
        (actor->m38E = 0);
        (actor->speedF = add(static_cast<f32>(pwViewPwDebugRegisters(parameters)->mApproachSpeed), 17.f));
        animation(dRes_INDEX_PW_BCK_NIGERU1_e, 7.f, 2, 8);
        advance();
        mode = 151;
    }
    if (mode == 151) {
        frameVoice();
        if (!static_cast<s16>(actor->m37E)) {
            f32 random = Pw_cM_rndFX(16384.f);
            s16 yaw = static_cast<s16>(actor->m38C);
            (actor->m38C = (s16)(yaw + (s16)ftoi(random)));
            random = Pw_cM_rndF(15.f);
            (actor->m37E = (s16)ftoi(add(random, 15.f)));
        }
        if (!static_cast<s16>(actor->m37A)) {
            Local<cXyz> endpoint;
            endpoint->x = static_cast<f32>(actor->current.pos.x);
            endpoint->y = static_cast<f32>(actor->current.pos.y);
            endpoint->z = static_cast<f32>(actor->current.pos.z);
            bool turn = Pw_Line_check(a, gabi::ea(endpoint.get()), 0) != 0;
            if (!turn) {
                f32 homeZ = static_cast<f32>(actor->m2F0.z),
                    currentZ = static_cast<f32>(actor->current.pos.z),
                    currentX = static_cast<f32>(actor->current.pos.x);
                f32 dz = sub(homeZ, currentZ), dx = sub(static_cast<f32>(actor->m2F0.x), currentX);
                f32 length = Pw_sqrtf_HD(fmadds(dx, dx, (f32)((double)dz * dz)));
                if (length >
                    add(static_cast<f32>(pwViewPwDebugRegisters(parameters)->mDemoDistance), 1000.f)) {
                    (actor->m38C = static_cast<s16>(Pw_cM_atan2s(dx, dz)));
                    turn = true;
                }
            }
            if (turn) {
                f32 random = Pw_cM_rndF(20.f);
                (actor->m37A = (s16)ftoi(add(random, 20.f)));
                random = Pw_cM_rndF(20.f);
                (actor->m37E = (s16)ftoi(add(random, 20.f)));
            }
        }
        if (static_cast<u8>(pwViewPwJalhallaView(parent)->mAction)) {
            f32 random = Pw_cM_rndF(7.f);
            animation(dRes_INDEX_PW_BCK_WAIT2_e, add(random, 2.f), 2, -1);
            advance();
        }
        return;
    }
    if (mode == 152) {
        Pw_cLib_addCalc0(pwAddress(actor, offsetof(pw_class, speedF)), 1.f, 2.f);
        if (static_cast<u8>(pwViewPwJalhallaView(parent)->mAction) == 2) {
            u32 play = Pw_dComIfGp_get();
            s32 yaw = Pw_fopAcM_searchActorAngleY(a, static_cast<u32>(pwViewPwPlayView(play)->mpPlayer));
            s16 current = static_cast<s16>(actor->mMode);
            (actor->m38C = (s16)yaw);
            (actor->mMode = (s16)(current + 1));
        }
        return;
    }
    if (mode == 153) {
        if (static_cast<u8>(pwViewPwJalhallaView(parent)->mAction) == 3) {
            f32 random = Pw_cM_rndF(7.f);
            animation(dRes_INDEX_PW_BCK_KIDUKU2_e, add(random, 2.f), 0, -1);
            advance();
        }
        return;
    }
    if (mode == 154) {
        if (static_cast<u8>(pwViewPwJalhallaView(parent)->mAction) == 4) {
            animation(dRes_INDEX_PW_BCK_NIGERU1_e, 7.f, 2, 8);
            advance();
        }
        return;
    }
    if (mode == 155) {
        frameVoice();
        s16 alpha = (s16)(static_cast<s16>(actor->m39A) - 3);
        (actor->speedF = add(static_cast<f32>(pwViewPwDebugRegisters(parameters)->mDemoApproachSpeed), 10.f));
        (actor->m39A = alpha < 100 ? 100 : alpha);
        u8 stage = static_cast<u8>(pwViewPwJalhallaView(parent)->mAction);
        if (stage == 4) {
            s16 target =
                (s16)ftoi(add(static_cast<f32>(pwViewPwDebugRegisters(parameters)->mDemoTurnLimit), 2000.f));
            s16 step =
                (s16)ftoi(add(static_cast<f32>(pwViewPwDebugRegisters(parameters)->mDemoTurnStep), 100.f));
            Pw_cLib_addCalcAngleS2(pwAddress(actor, offsetof(pw_class, m39E)), (s32)target, 1, (s32)step);
            (actor->speedF = add(static_cast<f32>(pwViewPwDebugRegisters(parameters)->mDemoSpeed), 40.f));
            (actor->m38C = (s16)angleToParent());
            return;
        }
        if (stage == 5) {
            (actor->m39E = (s16)ftoi(
                 add(static_cast<f32>(pwViewPwDebugRegisters(parameters)->mDemoTurnSpeed), 1000.f)));
            s32 yaw = angleToParent();
            s16 timer = static_cast<s16>(actor->m384);
            (actor->m38C = (s16)(yaw - 0x8000));
            if (!timer)
                (actor->m384 = (s16)ftoi(Pw_cM_rndF(
                     add(static_cast<f32>(pwViewPwDebugRegisters(parameters)->mDemoRandomTimer), 40.f))));
            return;
        }
        if (stage == 6) {
            s16 timer = static_cast<s16>(actor->m384);
            if (timer > 0) {
                (actor->m384 = (s16)(timer - 1));
                return;
            }
            animation(dRes_INDEX_PW_BCK_TORITUKI2_e, 1.f, 2, -1);
            voice(0x48F4, true);
            (actor->m39E = (s16)ftoi(
                 add(static_cast<f32>(pwViewPwDebugRegisters(parameters)->mDemoFastTurnSpeed), 2000.f)));
            (actor->m38C = (s16)angleToParent());
            (actor->mMode = 171);
        }
        return;
    }
    if (mode == 170) {
        animation(dRes_INDEX_PW_BCK_TORITUKI2_e, 4.f, 2, -1);
        if (a)
            voice(0x48F4, true);
        (actor->m33E = 0);
        f32 speed = add(static_cast<f32>(pwViewPwDebugRegisters(parameters)->mMergeSpeed), 20.f);
        s16 current = static_cast<s16>(actor->mMode);
        (actor->speedF = speed);
        parent = found->get();
        (actor->mMode = (s16)(current + 1));
        mode = 171;
    }
    if (mode == 171) {
        s16 alpha = (s16)(static_cast<s16>(actor->m39A) - 3);
        (actor->m39A = alpha < 100 ? 100 : alpha);
        if (!parent)
            return;
        s32 yaw = Pw_fopAcM_searchActorAngleY(a, parent);
        parent = found->get();
        f32 az = static_cast<f32>(actor->current.pos.z);
        (actor->m38C = (s16)yaw);
        f32 pz = static_cast<f32>(pwViewfopAc_ac_c(parent)->current.pos.z),
            px = static_cast<f32>(pwViewfopAc_ac_c(parent)->current.pos.x),
            ax = static_cast<f32>(actor->current.pos.x);
        f32 dz = sub(pz, az), dx = sub(px, ax);
        f32 distance = Pw_sqrtf_HD(fmadds(dx, dx, (f32)((double)dz * dz)));
        if (!(distance < add(static_cast<f32>(pwViewPwDebugRegisters(parameters)->mMergeDistance), 200.f)))
            return;
        (actor->speedF = 0.f);
        voice(0x48FD, true);
        animation(dRes_INDEX_PW_BCK_TORITUKI3_e, 0.f, 0, -1);
        advance();
        return;
    }
    if (mode == 172) {
        if (parent) {
            f32 height = add(add(static_cast<f32>(pwViewfopAc_ac_c(parent)->current.pos.y), 200.f),
                             static_cast<f32>(pwViewPwDebugRegisters(parameters)->mMergeHeight));
            Pw_cLib_addCalc2(pwAddress(actor, offsetof(pw_class, current.pos.y)), height, 1.f, 10.f);
            Pw_cLib_addCalc2(pwAddress(actor, offsetof(pw_class, current.pos)),
                             static_cast<f32>(pwViewfopAc_ac_c(found->get())->current.pos.x), 1.f, 10.f);
            Pw_cLib_addCalc2(pwAddress(actor, offsetof(pw_class, current.pos.z)),
                             static_cast<f32>(pwViewfopAc_ac_c(found->get())->current.pos.z), 1.f, 10.f);
        }
        u32 morf = static_cast<u32>(actor->mpMorf);
        if (!(static_cast<u8>(pwViewmDoExt_McaMorf(morf)->mFrameCtrl.mState) & 1) &&
            static_cast<f32>(pwViewmDoExt_McaMorf(morf)->mFrameCtrl.mRate) != 0.f)
            return;
        parent = found->get();
        if (parent) {
            s16 count = static_cast<s16>(pwViewPwJalhallaView(parent)->mPoeCount);
            (pwViewPwJalhallaView(parent)->mPoeMerged = 1);
            (pwViewPwJalhallaView(parent)->mPoeCount = (s16)(count + 1));
        }
        Pw_fopAcM_delete(a);
    }
}

// Execute's ordinary movement states. External actor helpers retain their HD calls.
static void action_dousa(pw_class* actor) {
    u32 a = ea(actor);
    constexpr u32 parameters = 0x1047B608;
    u32 play = Pw_dComIfGp_get(), player = static_cast<u32>(pwViewPwPlayView(play)->mpPlayer);
    play = Pw_dComIfGp_get();
    s16 mode = static_cast<s16>(actor->mMode);
    u32 camera = static_cast<u32>(pwViewPwPlayView(play)->mpCamera);
    Local<cXyz> vector;
    Local<be<u32>> found;
    auto animation = [&](s32 index, f32 blend, u32 loop, s32 sound = -1) {
        Pw_anm_init(a, index, blend, loop, 1.f, sound);
    };
    auto advance = [&]() {
        s16 next = (s16)(static_cast<s16>(actor->mMode) + 1);
        (actor->mMode = next);
        return next;
    };
    auto finished = [&]() {
        u32 morf = static_cast<u32>(actor->mpMorf);
        return (static_cast<u8>(pwViewmDoExt_McaMorf(morf)->mFrameCtrl.mState) & 1) ||
               static_cast<f32>(pwViewmDoExt_McaMorf(morf)->mFrameCtrl.mRate) == 0.f;
    };
    auto distance = [&]() {
        u32 p = Pw_dComIfGp_get();
        return Pw_fopAcM_searchActorDistance(a, static_cast<u32>(pwViewPwPlayView(p)->mpPlayer));
    };
    auto angle = [&](bool background) {
        u32 p = Pw_dComIfGp_get();
        return Pw_fopAcM_searchActorAngleY(a, static_cast<u32>(pwViewPwPlayView(p)->mpPlayer));
    };
    auto randomTimer = [&](be<s16>& timer, f32 span) {
        f32 r = Pw_cM_rndF(span);
        timer = (s16)ftoi(add(r, span));
    };
    auto endpoint = [&]() {
        vector->x = static_cast<f32>(actor->current.pos.x);
        vector->y = static_cast<f32>(actor->current.pos.y);
        vector->z = static_cast<f32>(actor->current.pos.z);
    };
    auto particle = [&](u32 id, u32 position, u32 rotation, u32 callback) {
        u32 p = Pw_dComIfGp_get();
        return Pw_dPa_control_c_set(static_cast<u32>(pwViewPwPlayView(p)->mpParticle), 0, id, position,
                                    rotation, 0, 255, callback, -1, 0, 0, 0);
    };
    switch (mode) {
    case 0:
        animation(dRes_INDEX_PW_BCK_WAIT1_e, 7.f, 2);
        (actor->m346 = 0);
        advance();
        break;
    case 1:
        if (!(distance() < 500.f))
            break;
        animation(dRes_INDEX_PW_BCK_DERUA1_e, 3.f, 0);
        {
            s32 yaw = angle(true);
            (actor->current.angle.y = (s16)yaw);
            advance();
            (actor->m38C = (s16)yaw);
            (actor->m3A4 = -80.f);
            (actor->shape_angle.y = (s16)yaw);
            (actor->m340 = 1);
        }
        break;
    case 2: {
        (actor->shape_angle.y = (s16)(static_cast<s16>(actor->shape_angle.y) + 0x1000));
        Pw_cLib_addCalc0(pwAddress(actor, offsetof(pw_class, m3A4)), 1.f, 3.f);
        s16 alpha = (s16)(static_cast<s16>(actor->m39A) + 7);
        (actor->m39A = alpha > 100 ? 100 : alpha);
        if (!Pw_J3DFrameCtrl_checkPass(static_cast<u32>(actor->mpMorf) + 0x98, 25.f))
            break;
        (actor->m3A4 = 0.f);
        Pw_first_mode_change(a);
        (actor->m38C = (s16)angle(true));
        u32 emitter = particle(0x82EE, pwAddress(actor, offsetof(pw_class, m2CC)),
                               pwAddress(actor, offsetof(pw_class, shape_angle)), 0);
        if (emitter) {
            u32 data = static_cast<u32>(
                pwViewJ3DModel(static_cast<u32>(pwViewmDoExt_McaMorf(actor->mpMorf)->mpModel.v))
                    ->mpMtxBuffer.v);
            u16 flags = static_cast<u16>(pwViewJ3DMtxBuffer_hd(data)->mFlags);
            u32 matrices = static_cast<u32>(pwViewJ3DModelData(data)->mMaterials);
            (pwViewJ3DMtxBuffer_hd(data)->mFlags = flags | 0x10);
            Pw_JPASetRMtxTVecfromMtx(matrices + 0x450, emitter + 0x1F0, emitter + 0x22C);
        }
        (actor->attention_info.flags = 0);
        (actor->mMode = 10);
        return;
    }
    case 9:
        (actor->m39A = 0);
        (actor->m39C = 0);
        advance();
        [[fallthrough]];
    case 10: {
        if (static_cast<u8>(actor->mBehaviorType) == 1)
            (actor->m38C = (s16)angle(true));
        u32 id = Pw_fopAcM_create(0xC1, 0xFF000001u, pwAddress(actor, offsetof(pw_class, m2CC)),
                                  (s32) static_cast<s8>(actor->current.roomNo), 0, 0, -1, 0);
        (actor->mKanteraID = id);
        if (id == 0xFFFFFFFF)
            break;
        (actor->m382 = 5);
        u8 type = static_cast<u8>(actor->mBehaviorType);
        if (type == 1) {
            (actor->mMode = 11);
            return;
        }
        if (type == 2) {
            (actor->m341 = 2);
            (actor->mMode = 8);
            return;
        }
        (actor->m346 = 1);
        (actor->mMode = 13);
        break;
    }
    case 11: {
        Pw_dKyr_get_vectle_calc(pwAddress(actor, offsetof(pw_class, current.pos)), camera + 0xDC,
                                gabi::ea(vector.get()));
        u32 id = static_cast<u32>(actor->mKanteraID);
        f32 x = static_cast<f32>(actor->m2CC.x), vx = vector->x, z = static_cast<f32>(actor->m2CC.z),
            vz = vector->z, y = static_cast<f32>(actor->m2CC.y);
        (actor->mActorPlace.pos.x = fmadds(vx, 150.f, x));
        (actor->mActorPlace.pos.y = y);
        (actor->mActorPlace.pos.z = fmadds(vz, 150.f, z));
        if (id == 0xFFFFFFFF || !Pw_fopAcM_SearchByID(id, gabi::ea(found.get())))
            break;
        u32 lantern = found->get();
        if (!lantern || static_cast<s16>(pwViewfopAc_ac_c(lantern)->mProcName) != 0xC1)
            break;
        (pwVectorWord(&pwViewfopAc_ac_c(lantern)->current.pos, 0) =
             static_cast<u32>(pwVectorWord(&actor->m2CC, 0)));
        (pwVectorWord(&pwViewfopAc_ac_c(lantern)->current.pos, 1) =
             static_cast<u32>(pwVectorWord(&actor->m2CC, 1)));
        (pwVectorWord(&pwViewfopAc_ac_c(lantern)->current.pos, 2) =
             static_cast<u32>(pwVectorWord(&actor->m2CC, 2)));
        (pwViewfopAc_ac_c(found->get())->current.angle.y = static_cast<s16>(actor->shape_angle.y));
        lantern = found->get();
        for (u32 i = 0; i < 3; ++i)
            pwMatrixElement(lantern + offsetof(fopAc_ac_c, scale), i) = 1.f;
        if (!finished())
            break;
        (actor->m341 = 0);
        (actor->mBehaviorType = 0);
        Pw_first_mode_change(a);
        (actor->m382 = 3);
        (actor->m346 = 1);
        (actor->mMode = 13);
        break;
    }
    case 6: {
        s16 alpha = (s16)(static_cast<s16>(actor->m39A) + 10);
        u32 morf = static_cast<u32>(actor->mpMorf);
        (actor->m39A = alpha > 100 ? 100 : alpha);
        if (!(static_cast<u8>(pwViewmDoExt_McaMorf(morf)->mFrameCtrl.mState) & 1) &&
            static_cast<f32>(pwViewmDoExt_McaMorf(morf)->mFrameCtrl.mRate) != 0.f)
            break;
        (actor->mBehaviorType = 0);
        (actor->m346 = 1);
        Pw_first_mode_change(a);
        (actor->mMode = 13);
        break;
    }
    case 7: {
        f32 d = distance();
        if (!(d < static_cast<f32>(actor->mNoticeRange)))
            break;
        s32 yaw = angle(true);
        (actor->current.angle.y = (s16)yaw);
        (actor->m38C = (s16)yaw);
        (actor->shape_angle.y = static_cast<s16>(actor->m38C));
        animation(dRes_INDEX_PW_BCK_DERUB2_e, 3.f, 0);
        (actor->mMode = 6);
        return;
    }
    case 8: {
        (actor->m380 = 2);
        (actor->m346 = 1);
        f32 d = distance();
        if (!(d < static_cast<f32>(actor->m3AC)))
            break;
        animation(dRes_INDEX_PW_BCK_DERUB1_e, 3.f, 2);
        (actor->mMode = 7);
        return;
    }
    case 13: {
        for (u32 i = 0; i < 4; ++i)
            pwDamageTimer(actor, i) = 0;
        (actor->m340 = 0);
        (actor->m341 = 0);
        randomTimer(actor->m378, 60.f);
        s16 held = static_cast<s16>(actor->m346);
        s32 current = static_cast<s32>(actor->mBckIdx), desired = held == 1 ? 35 : 36;
        if (current != desired)
            animation(desired, 7.f, 2);
        advance();
        [[fallthrough]];
    }
    case 14:
        Pw_cLib_addCalc0(pwAddress(actor, offsetof(pw_class, speedF)), 1.f, 1.f);
        if (static_cast<s16>(actor->m378))
            break;
        advance();
        [[fallthrough]];
    case 15: {
        randomTimer(actor->m378, 120.f);
        s32 current = static_cast<s32>(actor->mBckIdx),
            desired = static_cast<s16>(actor->m346) == 1 ? 20 : 21;
        if (current != desired)
            animation(desired, 7.f, 2);
        f32 r = Pw_cM_rndFX(16384.f);
        s16 yaw = static_cast<s16>(actor->m38C);
        u8 path = static_cast<u8>(actor->mPathIndex);
        (actor->m38C = (s16)(yaw + (s16)ftoi(r)));
        (actor->mMode = path != 255 && static_cast<u32>(actor->mpPath) ? 20 : 16);
        break;
    }
    case 16:
        (actor->speedF = 5.f);
        Pw_move_sound(a);
        if (static_cast<s16>(actor->m37A))
            break;
        endpoint();
        if (Pw_Line_check(a, gabi::ea(vector.get()), 0) || Pw_hani_check(a)) {
            (actor->m37A = 10);
            break;
        }
        if (!static_cast<s16>(actor->m378))
            (actor->mMode = 15);
        break;
    case 20: {
        (pwVectorWord(&actor->m2FC, 0) = static_cast<u32>(pwVectorWord(&actor->current.pos, 0)));
        (pwVectorWord(&actor->m2FC, 1) = static_cast<u32>(pwVectorWord(&actor->current.pos, 1)));
        (pwVectorWord(&actor->m2FC, 2) = static_cast<u32>(pwVectorWord(&actor->current.pos, 2)));
        if (static_cast<u8>(actor->mPathIndex) == 255 || !static_cast<u32>(actor->mpPath))
            break;
        (actor->speedF = 5.f);
        Pw_move_sound(a);
        u32 path = static_cast<u32>(actor->mpPath);
        s32 index = static_cast<s8>(actor->mPathPntIdx);
        f32 z = static_cast<f32>(actor->current.pos.z), x = static_cast<f32>(actor->current.pos.x);
        u32 point = static_cast<u32>(pwViewPwPathView(path)->mpPoints) + (u32)(index * 16);
        f32 dx = sub(static_cast<f32>(pwViewPwPathPoint(point)->x), x),
            dz = sub(static_cast<f32>(pwViewPwPathPoint(point)->z), z);
        s32 yaw = Pw_cM_atan2s(dx, dz);
        (actor->m38C = (s16)yaw);
        if (Pw_sqrtf_HD(fmadds(dx, dx, (f32)((double)dz * dz))) < 80.f) {
            s32 next = (s8)(static_cast<u8>(actor->mPathPntIdx) + 1);
            path = static_cast<u32>(actor->mpPath);
            (actor->mPathPntIdx = (u8)next);
            if (next >= static_cast<u16>(pwViewPwPathView(path)->mNum))
                (actor->mPathPntIdx = 0);
        }
        break;
    }
    case 25:
        (actor->m38C = (s16)angle(false));
        animation(dRes_INDEX_PW_BCK_DAMAGE_K1_e, 9.f, 0);
        advance();
        (actor->speedF = -20.f);
        [[fallthrough]];
    case 26:
        Pw_cLib_addCalc0(pwAddress(actor, offsetof(pw_class, speedF)), 1.f, 1.f);
        if (__builtin_fabsf(static_cast<f32>(actor->speedF)) < 0.2f) {
            (actor->mAction = PW_ACTION_KOUGEKI);
            (actor->mMode = 37);
        }
        break;
    case 27:
        animation(dRes_INDEX_PW_BCK_DOKI1_e, 3.f, 2);
        (actor->speedF = 0.f);
        (actor->speed.x = 0.f);
        advance();
        (actor->speed.y = 0.f);
        (actor->speed.z = 0.f);
        [[fallthrough]];
    case 28:
        if (!(static_cast<u32>(actor->actor_status) & 0x100000)) {
            (actor->mMode = 90);
            break;
        }
        break;
    case 100:
        animation(dRes_INDEX_PW_BCK_JITTAIKA1_e, 6.f, 0);
        advance();
        break;
    case 101:
        if (!finished())
            break;
        (actor->mMode = 90);
        [[fallthrough]];
    case 90: {
        (actor->speedF = 12.f);
        (actor->m378 = 360);
        u32 parent = static_cast<u32>(actor->mJalhallaID);
        s32 current = static_cast<s32>(actor->mBckIdx);
        if (parent != 0xFFFFFFFF)
            (actor->speedF = add(static_cast<f32>(pwViewPwDebugRegisters(parameters)->mApproachSpeed), 9.f));
        if (current != 27)
            animation(dRes_INDEX_PW_BCK_NIGERU1_e, 7.f, 2, 8);
        (actor->attention_info.flags = 4);
        s32 yaw = angle(false);
        s16 state = static_cast<s16>(actor->mMode);
        (actor->m340 = 0);
        (actor->m38C = (s16)(yaw + 0x8000));
        (actor->mMode = (s16)(state + 1));
        [[fallthrough]];
    }
    case 91: {
        if (Pw_J3DFrameCtrl_checkPass(static_cast<u32>(actor->mpMorf) + 0x98, 0.f) && a &&
            pwAddress(actor, offsetof(pw_class, eyePos))) {
            u32 id = static_cast<u32>(actor->mBsPcId);
            s32 reverb = Pw_dComIfGp_getReverb((s32) static_cast<s8>(actor->current.roomNo));
            Pw_fopAcM_monsSeStart_HD(0x48F1, pwAddress(actor, offsetof(pw_class, eyePos)), id, 0, reverb);
        }
        if (!static_cast<s16>(actor->m37E)) {
            f32 r = Pw_cM_rndFX(16384.f);
            s16 yaw = static_cast<s16>(actor->m38C);
            (actor->m38C = (s16)(yaw + (s16)ftoi(r)));
            randomTimer(actor->m37E, 15.f);
        }
        if (!static_cast<s16>(actor->m37A)) {
            endpoint();
            if (Pw_Line_check(a, gabi::ea(vector.get()), 0) || Pw_hani_check(a)) {
                randomTimer(actor->m37A, 20.f);
                randomTimer(actor->m37E, 20.f);
            }
        }
        if (!static_cast<s16>(actor->m378) && static_cast<u32>(actor->mJalhallaID) == 0xFFFFFFFF) {
            play = Pw_dComIfGp_get();
            if (!Pw_dDetect_c_chk_light(play + 0x5A20, pwAddress(actor, offsetof(pw_class, current.pos)))) {
                (actor->mCyl.mGObjTg.mSPrm = static_cast<u32>(actor->mCyl.mGObjTg.mSPrm) & ~2u);
                animation(dRes_INDEX_PW_BCK_ANSIN1_e, 3.f, 0);
                s16 state = static_cast<s16>(actor->mMode);
                u8 altered = static_cast<u8>(actor->m33F);
                (actor->m33E = 0);
                (actor->mMode = (s16)(state + 1));
                if (altered) {
                    u32 model = static_cast<u32>(pwViewmDoExt_McaMorf(actor->mpMorf)->mpModel.v);
                    (actor->m33F = 0);
                    struct String {
                        be<u32> data, vtable;
                    };
                    Local<String> archive;
                    archive->data = gabi::ea(l_idleArchive);
                    archive->vtable = gc_tu.safeString;
                    u32 data = Pw_dRes_control_c_getRes(l_resourceControl, gabi::ea(archive.get()), 42);
                    Pw_mDoExt_brkAnm_init(static_cast<u32>(actor->m2C8),
                                          static_cast<u32>(pwViewJ3DModel(model)->mModelData.v), data, 1, 0,
                                          1.f, 0, -1, 1, 0);
                }
            }
        }
        Pw_Big_pow_gattai_check(a);
        break;
    }
    case 92: {
        if (a && pwAddress(actor, offsetof(pw_class, eyePos))) {
            s32 reverb = Pw_dComIfGp_getReverb((s32) static_cast<s8>(actor->current.roomNo));
            Pw_cc_seStart(0x5930, pwAddress(actor, offsetof(pw_class, eyePos)), 0, reverb);
        }
        Pw_cLib_addCalc0(pwAddress(actor, offsetof(pw_class, speedF)), 1.f, 1.f);
        s16 alpha = (s16)(static_cast<s16>(actor->m39A) - 3);
        (actor->m39A = alpha);
        if (alpha < 100 && finished()) {
            (actor->m38E = 0);
            (actor->m39A = 100);
            (actor->m39C = 100);
            Pw_next_dousa_check(a);
        }
        break;
    }
    case 110: {
        for (u32 i = 0; i < 3; ++i)
            (pwVectorWord(&actor->mActorPlace.pos, i) =
                 static_cast<u32>(pwVectorWord(&actor->current.pos, i)));
        for (u32 i = 0; i < 3; ++i)
            (pwAngleWord(&actor->mActorPlace.angle, i) =
                 static_cast<u16>(pwAngleWord(&actor->shape_angle, i)));
        particle(0x8445, pwAddress(actor, offsetof(pw_class, mActorPlace.pos)),
                 pwAddress(actor, offsetof(pw_class, mActorPlace.angle)),
                 pwAddress(actor, offsetof(pw_class, m5C4)));
        animation(dRes_INDEX_PW_BCK_ATTACK1_e, 7.f, 2);
        f32 base = add(static_cast<f32>(pwViewPwDebugRegisters(parameters)->mJumpSpeed), 45.f);
        (actor->speed.y = base);
        f32 r = Pw_cM_rndF(add(static_cast<f32>(pwViewPwDebugRegisters(parameters)->mJumpRandomSpeed), 15.f));
        (actor->speed.y = add(base, r));
        (actor->gravity = add(static_cast<f32>(pwViewPwDebugRegisters(parameters)->mGravity), -2.f));
        base = add(static_cast<f32>(pwViewPwDebugRegisters(parameters)->mRetreatSpeed), 10.f);
        (actor->speedF = base);
        r = Pw_cM_rndF(add(static_cast<f32>(pwViewPwDebugRegisters(parameters)->mRetreatRandomSpeed), 10.f));
        (actor->speedF = add(base, r));
        u32 id = static_cast<u32>(actor->mJalhallaID);
        if (id != 0xFFFFFFFF && Pw_fopAcM_SearchByID(id, gabi::ea(found.get()))) {
            u32 parent = found->get();
            if (parent && static_cast<s16>(pwViewfopAc_ac_c(parent)->mProcName) == 0xD3 &&
                static_cast<s16>(pwViewPwJalhallaView(parent)->mMode) == 2) {
                (actor->gravity = add(static_cast<f32>(pwViewPwDebugRegisters(parameters)->mGravity), -2.f));
                base = add(static_cast<f32>(pwViewPwDebugRegisters(parameters)->mRetreatSpeed), 8.f);
                (actor->speedF = base);
                r = Pw_cM_rndF(
                    add(static_cast<f32>(pwViewPwDebugRegisters(parameters)->mRetreatRandomSpeed), 5.f));
                (actor->speedF = add(base, r));
            }
        }
        advance();
        (actor->m340 = 1);
        break;
    }
    case 111:
        if (static_cast<u32>(actor->m5C4.mpEmitter)) {
            for (u32 i = 0; i < 3; ++i)
                (pwVectorWord(&actor->mActorPlace.pos, i) =
                     static_cast<u32>(pwVectorWord(&actor->current.pos, i)));
            for (u32 i = 0; i < 3; ++i)
                (pwAngleWord(&actor->mActorPlace.angle, i) =
                     static_cast<u16>(pwAngleWord(&actor->shape_angle, i)));
        }
        if (!(static_cast<u32>(actor->mAcch.m_flags) & 0x20))
            break;
        Pw_dPa_followEcallBack_end(pwAddress(actor, offsetof(pw_class, m5C4)));
        (actor->speedF = 0.f);
        (actor->speed.x = 0.f);
        (actor->speed.y = 0.f);
        (actor->speed.z = 0.f);
        (actor->gravity = 0.f);
        animation(dRes_INDEX_PW_BCK_SIRIMOTI1_e, 0.f, 0);
        advance();
        [[fallthrough]];
    case 112:
        if (finished())
            (actor->mMode = 90);
        break;
    default:
        break;
    }
    mode = static_cast<s16>(actor->mMode);
    if (mode <= 11)
        return;
    if (!static_cast<s16>(actor->m37C) && mode < 90 && (mode == 14 || mode == 16 || mode == 20)) {
        bool attack = false;
        if (!Pw_hani_check(a)) {
            play = Pw_dComIfGp_get();
            if (Pw_fopAcM_searchActorDistance(a, static_cast<u32>(pwViewPwPlayView(play)->mpPlayer)) <
                    500.f &&
                __builtin_fabsf(sub(static_cast<f32>(actor->current.pos.y),
                                    static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.y))) < 100.f) {
                vector->x = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.x);
                vector->y = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.y);
                vector->z = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.z);
                if (!Pw_Line_check(a, gabi::ea(vector.get()), 1) &&
                    (static_cast<s16>(actor->m346) == 1 || !TORITUKI_ON))
                    attack = true;
            }
        }
        if (attack) {
            (actor->mAction = PW_ACTION_KOUGEKI);
            (actor->mMode = 30);
            Pw_alpha_anime(a);
        } else {
            mode = static_cast<s16>(actor->mMode);
            if ((u32)(mode - 10) < 80)
                Pw_alpha_anime(a);
        }
    } else if ((u32)(mode - 10) < 80)
        Pw_alpha_anime(a);
    if (static_cast<u32>(actor->mJalhallaID) == 0xFFFFFFFF)
        Pw_fuwafuwa_calc(a);
}

static void action_kougeki(pw_class* actor) {
    u32 a = ea(actor);
    u32 play = Pw_dComIfGp_get();
    s16 mode = static_cast<s16>(actor->mMode);
    u32 player = static_cast<u32>(pwViewPwPlayView(play)->mpPlayer);
    auto animation = [&](s32 index, f32 blend, u32 loop, s32 sound = -1) {
        Pw_anm_init(a, index, blend, loop, 1.f, sound);
    };
    auto advance = [&]() { (actor->mMode = (s16)(static_cast<s16>(actor->mMode) + 1)); };
    auto finished = [&]() {
        u32 morf = static_cast<u32>(actor->mpMorf);
        return (static_cast<u8>(pwViewmDoExt_McaMorf(morf)->mFrameCtrl.mState) & 1) ||
               static_cast<f32>(pwViewmDoExt_McaMorf(morf)->mFrameCtrl.mRate) == 0.f;
    };
    auto angle = [&]() {
        u32 p = Pw_dComIfGp_get();
        return Pw_fopAcM_searchActorAngleY(a, static_cast<u32>(pwViewPwPlayView(p)->mpPlayer));
    };
    auto voice = [&](s32 id) {
        if (a && pwAddress(actor, offsetof(pw_class, eyePos))) {
            u32 process = static_cast<u32>(actor->mBsPcId);
            s32 reverb = Pw_dComIfGp_getReverb((s32) static_cast<s8>(actor->current.roomNo));
            Pw_fopAcM_monsSeStart_HD(id, pwAddress(actor, offsetof(pw_class, eyePos)), process, 0, reverb);
        }
    };
    auto retreat = [&]() {
        f32 r = Pw_cM_rndF(70.f);
        (actor->m37C = (s16)ftoi(add(r, 70.f)));
        if (static_cast<u8>(actor->mPathIndex) == 255) {
            f32 x = static_cast<f32>(actor->current.pos.x), z = static_cast<f32>(actor->current.pos.z);
            (actor->m2F0.x = x);
            (actor->m2F0.z = z);
        }
        (actor->mAction = PW_ACTION_DOUSA);
        (actor->mMode = 13);
    };
    bool endAttack = false;
    switch (mode) {
    case 30:
        animation(static_cast<s16>(actor->m346) == 1 ? 24 : 25, 6.f, 0);
        advance();
        (actor->m340 = 0);
        break;
    case 31:
        Pw_kyori_sub(a);
        (actor->m38C = (s16)angle());
        Pw_cLib_addCalc0(pwAddress(actor, offsetof(pw_class, speedF)), 1.f, 0.3f);
        if (!finished())
            break;
        advance();
        [[fallthrough]];
    case 32:
        Pw_kyori_sub(a);
        for (u32 i = 0; i < 4; ++i)
            pwDamageTimer(actor, i) = 0;
        (actor->m340 = 0);
        if (static_cast<s16>(actor->m346) == 1) {
            animation(dRes_INDEX_PW_BCK_ATTACK1_e, 6.f, 0, 7);
            voice(0x48F3);
            (actor->m341 = 1);
            (actor->mMode = 33);
        } else {
            animation(dRes_INDEX_PW_BCK_TORITUKI1_e, 3.f, 2);
            (actor->m378 = 100);
            (actor->mMode = 40);
        }
        break;
    case 33:
        Pw_kyori_sub(a);
        Pw_cLib_addCalc0(pwAddress(actor, offsetof(pw_class, speedF)), 1.f, 0.3f);
        (actor->m38C = (s16)angle());
        if (Pw_J3DFrameCtrl_checkPass(static_cast<u32>(actor->mpMorf) + 0x98, 23.f)) {
            advance();
            (actor->speedF = 24.f);
        }
        break;
    case 34: {
        Pw_move_sound(a);
        if (Pw_kougen_hani_check(a, 0) || Pw_kougen_hani_check(a, 1)) {
            endAttack = true;
            break;
        }
        if (Pw_J3DFrameCtrl_checkPass(static_cast<u32>(actor->mpMorf) + 0x98, 28.f)) {
            (actor->mSph.mObjAt.mSPrm = static_cast<u32>(actor->mSph.mObjAt.mSPrm) | 1);
            (actor->mSph.mObjAt.mRPrm = 1);
        }
        u32 morf = static_cast<u32>(actor->mpMorf);
        if (!(static_cast<f32>(pwViewmDoExt_McaMorf(morf)->mFrameCtrl.mFrame) <= 28.f) &&
            (static_cast<u32>(actor->mSph.mGObjAt.mRPrm) & 1)) {
            (actor->mSph.mObjAt.mSPrm = static_cast<u32>(actor->mSph.mObjAt.mSPrm) & ~1u);
            (actor->mAction = PW_ACTION_DOUSA);
            (actor->mMode = 25);
            return;
        }
        if (Pw_J3DFrameCtrl_checkPass(morf + 0x98, 40.f)) {
            u32 flags = static_cast<u32>(actor->mSph.mObjAt.mSPrm);
            s16 current = static_cast<s16>(actor->mMode);
            (actor->mSph.mObjAt.mSPrm = flags & ~1u);
            (actor->mMode = (s16)(current + 1));
        }
        break;
    }
    case 35:
        Pw_cLib_addCalc0(pwAddress(actor, offsetof(pw_class, speedF)), 1.f, 5.f);
        if (__builtin_fabsf(static_cast<f32>(actor->speedF)) < 0.2f) {
            endAttack = true;
            break;
        }
        break;
    case 40:
        Pw_cLib_addCalc0(pwAddress(actor, offsetof(pw_class, speedF)), 1.f, 0.3f);
        (actor->m38C = (s16)angle());
        if (static_cast<s16>(actor->m378))
            break;
        (actor->m378 = 300);
        animation(dRes_INDEX_PW_BCK_TORITUKI2_e, 4.f, 2);
        voice(0x48F4);
        advance();
        break;
    case 41: {
        (actor->m38C = (s16)angle());
        if (Pw_kougen_hani_check(a, 0) || Pw_kougen_hani_check(a, 1)) {
            endAttack = true;
            break;
        }
        Pw_cLib_addCalc2(pwAddress(actor, offsetof(pw_class, speedF)), 15.f, 1.f, 3.f);
        Pw_move_sound(a);
        if (!static_cast<s16>(actor->m378)) {
            animation(dRes_INDEX_PW_BCK_IKIGIRE1_e, 6.f, 0);
            (actor->mMode = 36);
            break;
        }
        Local<cXyz> target;
        target->x = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.x);
        target->y = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.y);
        target->z = static_cast<f32>(pwViewfopAc_ac_c(player)->current.pos.z);
        if (Pw_Line_check(a, gabi::ea(target.get()), 1))
            retreat();
        break;
    }
    case 36:
        Pw_cLib_addCalc0(pwAddress(actor, offsetof(pw_class, speedF)), 1.f, 1.f);
        if (finished()) {
            endAttack = true;
            break;
        }
        break;
    case 37: {
        for (u32 i = 0; i < 4; ++i)
            pwDamageTimer(actor, i) = 0;
        s16 held = static_cast<s16>(actor->m346);
        (actor->speedF = 0.f);
        s32 desired = held == 1 ? 35 : 36;
        if (static_cast<s32>(actor->mBckIdx) != desired)
            animation(desired, 7.f, 2);
        f32 r = Pw_cM_rndF(70.f);
        (actor->m37C = (s16)ftoi(add(r, 70.f)));
        (actor->m340 = 0);
        (pwVectorWord(&actor->m320, 0) = static_cast<u32>(pwVectorWord(&actor->current.pos, 0)));
        (pwVectorWord(&actor->m320, 1) = static_cast<u32>(pwVectorWord(&actor->current.pos, 1)));
        (pwVectorWord(&actor->m320, 2) = static_cast<u32>(pwVectorWord(&actor->current.pos, 2)));
        (actor->m341 = 0);
        (actor->m392 = 0);
        advance();
        [[fallthrough]];
    }
    case 38:
        Pw_kyori_sub(a);
        (actor->m38C = (s16)angle());
        Pw_cLib_addCalc0(pwAddress(actor, offsetof(pw_class, speedF)), 1.f, 1.f);
        if (!static_cast<s16>(actor->m37C)) {
            (actor->speedF = 0.f);
            Pw_next_dousa_check(a);
        }
        break;
    default:
        break;
    }
    if (endAttack)
        (actor->mMode = 37);
    if (!static_cast<s16>(actor->m346)) {
        if (TORITUKI_ON == 1)
            retreat();
        else {
            mode = static_cast<s16>(actor->mMode);
            if ((mode == 40 || mode == 41 || mode == 38) &&
                Pw_dCcD_GObjInf_ChkAtHit(pwAddress(actor, offsetof(pw_class, mCyl)))) {
                u32 other =
                    Pw_dCcD_GAtTgCoCommonBase_GetAc(pwAddress(actor, offsetof(pw_class, mCyl.mGObjAt)));
                if (other && other == player) {
                    play = Pw_dComIfGp_get();
                    if (other == static_cast<u32>(pwViewPwPlayView(play)->mpControlPlayer)) {
                        (actor->speedF = 0.f);
                        (actor->mCyl.mObjAt.mSPrm = static_cast<u32>(actor->mCyl.mObjAt.mSPrm) & ~1u);
                        (actor->mCyl.mTgType = 0x800000);
                        (TORITUKI_ON = 1);
                        (actor->m343 = 1);
                        (actor->mAction = PW_ACTION_DEMO);
                        (actor->mMode = 70);
                    }
                }
            }
        }
    }
    Pw_alpha_anime(a);
    Pw_fuwafuwa_calc(a);
}

static void action_itai(pw_class* actor) {
    u32 a = ea(actor);
    constexpr u32 parameters = 0x1047B608;
    u32 play = Pw_dComIfGp_get();
    s16 mode = static_cast<s16>(actor->mMode);
    u32 model = static_cast<u32>(pwViewmDoExt_McaMorf(actor->mpMorf)->mpModel.v),
        player = static_cast<u32>(pwViewPwPlayView(play)->mpPlayer);
    auto animation = [&](s32 index, f32 blend, u32 loop) { Pw_anm_init(a, index, blend, loop, 1.f, -1); };
    auto advance = [&]() { (actor->mMode = (s16)(static_cast<s16>(actor->mMode) + 1)); };
    auto finished = [&]() {
        u32 morf = static_cast<u32>(actor->mpMorf);
        return (static_cast<u8>(pwViewmDoExt_McaMorf(morf)->mFrameCtrl.mState) & 1) ||
               static_cast<f32>(pwViewmDoExt_McaMorf(morf)->mFrameCtrl.mRate) == 0.f;
    };
    auto angle = [&]() {
        u32 p = Pw_dComIfGp_get();
        return Pw_fopAcM_searchActorAngleY(a, static_cast<u32>(pwViewPwPlayView(p)->mpPlayer));
    };
    auto sound = [&](s32 id) {
        if (pwAddress(actor, offsetof(pw_class, eyePos))) {
            s32 reverb = Pw_dComIfGp_getReverb((s32) static_cast<s8>(actor->current.roomNo));
            Pw_cc_seStart(id, pwAddress(actor, offsetof(pw_class, eyePos)), 0, reverb);
        }
    };
    auto voice = [&](s32 id) {
        if (pwAddress(actor, offsetof(pw_class, eyePos))) {
            u32 process = a ? static_cast<u32>(actor->mBsPcId) : 0xFFFFFFFF;
            s32 reverb = Pw_dComIfGp_getReverb((s32) static_cast<s8>(actor->current.roomNo));
            Pw_fopAcM_monsSeStart_HD(id, pwAddress(actor, offsetof(pw_class, eyePos)), process, 0, reverb);
        }
    };
    struct String {
        be<u32> data, vtable;
    };
    Local<String> archive[4];
    auto material = [&](u32 slot, be<u32>& registerAnimation, s32 resource, u32 loop) {
        archive[slot]->vtable = gc_tu.safeString;
        archive[slot]->data = gabi::ea(l_damageArchive);
        u32 data = Pw_dRes_control_c_getRes(l_resourceControl, gabi::ea(archive[slot].get()), resource);
        Pw_mDoExt_brkAnm_init(static_cast<u32>(registerAnimation),
                              static_cast<u32>(pwViewJ3DModel(model)->mModelData.v), data, 1, loop, 1.f, 0,
                              -1, 1, 0);
    };
    switch (mode) {
    case 50: {
        (actor->mBehaviorType = 0);
        (actor->m340 = 1);
        u32 flags = static_cast<u32>(actor->actor_status);
        (actor->m341 = 1);
        (actor->attention_info.flags = 4);
        (actor->actor_status = flags | 0x20);
        s32 yaw = angle();
        (actor->current.angle.y = (s16)yaw);
        (actor->m38C = (s16)yaw);
        (actor->m38E = 0);
        (actor->m39A = 200);
        (actor->shape_angle.y = (s16)yaw);
        (actor->m39C = 200);
        (actor->m37E = 3);
        if (!static_cast<s16>(actor->m398))
            Pw_kantera_break(a);
        (actor->speedF = -20.f);
        animation(dRes_INDEX_PW_BCK_DAMAGE_K1_e, 3.f, 0);
        advance();
        break;
    }
    case 51:
        Pw_cLib_addCalc0(pwAddress(actor, offsetof(pw_class, speedF)), 1.f, 1.f);
        if (finished()) {
            (actor->speedF = 0.f);
            Pw_next_dousa_check(a);
        }
        break;
    case 52: {
        Pw_kantera_break(a);
        s16 yaw = static_cast<s16>(actor->current.angle.y);
        (actor->m38C = yaw);
        (actor->m340 = 1);
        (actor->shape_angle.y = yaw);
        for (u32 i = 0; i < 4; ++i)
            pwDamageTimer(actor, i) = 0;
        if (static_cast<s32>(actor->mBckIdx) != 19)
            animation(dRes_INDEX_PW_BCK_DOKI1_e, 3.f, 2);
        (actor->m378 = 30);
        (actor->speedF = 0.f);
        advance();
        [[fallthrough]];
    }
    case 53: {
        u8 hit = static_cast<u8>(actor->mHitByWhat);
        if (hit == 9 || hit == 10) {
            (actor->m378 = 10);
            (actor->m39A = (s16)(static_cast<s16>(actor->m39A) + 5));
            if (!static_cast<u8>(actor->m33F)) {
                (actor->m33F = 1);
                material(0, actor->m2C4, dRes_INDEX_PW_BRK_HIT_e, 2);
            }
            if (static_cast<u8>(actor->mHitByWhat) == 10 || static_cast<s16>(actor->m39A) >= 255) {
                (actor->m39A = 255);
                (actor->m38E = 0);
                if (a) {
                    sound(0x5936);
                    voice(0x48F2);
                }
                u32 flags = static_cast<u32>(actor->mCyl.mGObjTg.mSPrm);
                (actor->m33E = 1);
                (actor->mCyl.mGObjTg.mSPrm = flags | 2);
                material(1, actor->m2C0, dRes_INDEX_PW_BRK_JITTAIKA1_e, 0);
                (actor->mAction = PW_ACTION_DOUSA);
                (actor->mMode = 100);
                (actor->mCyl.mTgType = 0xFF3DFEFF);
                return;
            }
            if (a)
                sound(0x5135);
        } else {
            if (static_cast<u8>(actor->m33F)) {
                (actor->m33F = 0);
                material(2, actor->m2C8, dRes_INDEX_PW_BRK_DEFAULT_e, 0);
            }
            (actor->m39C = 0);
            (actor->m39A = 0);
            (actor->m38E = 0);
            Pw_next_dousa_check(a);
        }
        break;
    }
    case 54: {
        (actor->m340 = 1);
        (actor->m380 = 0);
        Pw_dPa_followEcallBack_end(pwAddress(actor, offsetof(pw_class, m5C4)));
        s16 yaw = static_cast<s16>(actor->shape_angle.y);
        (actor->m38C = yaw);
        (actor->current.angle.y = (s16)angle());
        (actor->shape_angle.y = static_cast<s16>(actor->m38C));
        s8 health = static_cast<s8>(actor->health);
        (actor->speedF = -24.f);
        if (health <= 0) {
            u32 flags = static_cast<u32>(actor->mCyl.mObjAt.mSPrm),
                target = static_cast<u32>(actor->mCyl.mObjTg.mSPrm);
            (actor->mCyl.mObjAt.mSPrm = flags & ~1u);
            (actor->mCyl.mObjTg.mSPrm = target & ~1u);
            Pw_dCcD_GObjInf_ClrTgHit(pwAddress(actor, offsetof(pw_class, mCyl)));
            (actor->speedF = (f32)((double)static_cast<f32>(actor->speedF) * 1.5f));
        }
        voice(0x48F5);
        animation(dRes_INDEX_PW_BCK_DAMAGE1_e, 3.f, 0);
        advance();
        break;
    }
    case 55:
        if (Pw_Big_pow_gattai_check(a))
            break;
        Pw_cLib_addCalc0(pwAddress(actor, offsetof(pw_class, speedF)), 1.f, 1.f);
        if (!finished())
            break;
        {
            s8 health = static_cast<s8>(actor->health);
            (actor->speedF = 0.f);
            (actor->mMode = health > 0 ? 90 : 60);
            if (health > 0)
                (actor->mAction = PW_ACTION_DOUSA);
        }
        return;
    case 56:
        (actor->current.angle.y = static_cast<s16>(pwViewfopAc_ac_c(player)->shape_angle.y));
        (actor->m340 = 1);
        (actor->speedF = 24.f);
        (pwViewmDoExt_McaMorf(actor->mpMorf)->mFrameCtrl.mRate = 0.25f);
        advance();
        [[fallthrough]];
    case 57:
        if (Pw_Big_pow_gattai_check(a))
            break;
        Pw_cLib_addCalc0(pwAddress(actor, offsetof(pw_class, speedF)), 1.f, 1.f);
        if (static_cast<f32>(actor->speedF) < 0.2f) {
            s16 yaw = static_cast<s16>(actor->shape_angle.y);
            (actor->speedF = 0.f);
            (actor->current.angle.y = yaw);
            (actor->m340 = 0);
            (pwViewmDoExt_McaMorf(actor->mpMorf)->mFrameCtrl.mRate = 1.f);
            (actor->mAction = PW_ACTION_DOUSA);
            (actor->mMode = 90);
            return;
        }
        break;
    case 58: {
        u8 kind = static_cast<u8>(actor->mHitByWhat);
        (actor->m340 = 1);
        (actor->speedF = 0.f);
        if (kind == 4) {
            Pw_enemy_piyo_set(a);
            sound(0x50BC);
            (actor->m378 = (s16)(static_cast<s16>(pwViewPwDebugRegisters(parameters)->mStunTime) + 75));
        }
        animation(dRes_INDEX_PW_BCK_KIZETU1_e, 3.f, 0);
        advance();
        [[fallthrough]];
    }
    case 59:
        if (!Pw_Big_pow_gattai_check(a) && !static_cast<s16>(actor->m378)) {
            (actor->mAction = PW_ACTION_DOUSA);
            (actor->mMode = 90);
            return;
        }
        break;
    case 60:
        animation(dRes_INDEX_PW_BCK_SYOUTEN1_e, 2.f, 2);
        (actor->m33E = 0);
        (actor->attention_info.flags = 0);
        Pw_dPa_followEcallBack_end(pwAddress(actor, offsetof(pw_class, m5C4)));
        if (!static_cast<s16>(actor->m38A)) {
            sound(0x5934);
            voice(0x48F6);
        }
        if (static_cast<u8>(actor->m33F)) {
            (actor->m33F = 0);
            material(3, actor->m2C8, dRes_INDEX_PW_BRK_DEFAULT_e, 0);
        }
        (actor->gravity = 0.6f);
        advance();
        [[fallthrough]];
    case 61: {
        s16 alpha = (s16)(static_cast<s16>(actor->m39A) - 8);
        (actor->m39A = alpha);
        if (alpha >= 0)
            break;
        s16 disappear = static_cast<s16>(actor->m38A);
        (actor->m38E = 0);
        (actor->m39A = 0);
        if (!disappear) {
            Local<cXyz> position;
            position->x = static_cast<f32>(actor->current.pos.x);
            f32 y = static_cast<f32>(actor->current.pos.y);
            position->y = y;
            position->z = static_cast<f32>(actor->current.pos.z);
            position->y = add(y, 60.f);
            Pw_fopAcM_createDisappear(a, gabi::ea(position.get()), 5, 0, 255);
        }
        Pw_fopAcM_delete(a);
        s32 room = static_cast<s8>(actor->home.roomNo);
        u32 save = l_saveInfo;
        Pw_dSv_info_c_onActor(save + 32, (u32) static_cast<u16>(actor->setID), room);
        break;
    }
    case 62:
        if (finished()) {
            (actor->mMode = 60);
            return;
        }
        break;
    default:
        break;
    }
    mode = static_cast<s16>(actor->mMode);
    if (mode <= 51) {
        if (!static_cast<s16>(actor->m37E))
            Pw_alpha_anime(a);
        Pw_fuwafuwa_calc(a);
    }
}

/* 0244B3E4 */
s32 daPW_Execute(pw_class* actor) {
    u32 a = ea(actor);
    Pw_fopAcM_setGbaName(a, 60, 13, 45);
    if (Pw_enemy_ice(pwAddress(actor, offsetof(pw_class, mEnemyIce)))) {
        u32 morf = static_cast<u32>(actor->mpMorf);
        constexpr u32 matrix = 0x1048D0CC;
        f32 values[12];
        for (u32 i = 0; i < 12; ++i)
            values[i] = static_cast<f32>(pwMatrixElement(matrix, i));
        u32 model = static_cast<u32>(pwViewmDoExt_McaMorf(morf)->mpModel.v);
        const u32 order[12] = {0, 1, 2, 3, 4, 8, 5, 6, 7, 9, 10, 11};
        for (u32 i : order)
            (pwMatrixElement(model + offsetof(J3DModel, mBaseTransformMtx), i) = values[i]);
        Pw_mDoExt_McaMorf_calc(static_cast<u32>(actor->mpMorf));
        return 1;
    }
    for (u32 i = 0; i < 6; ++i) {
        s16 timer = pwTimer(actor, i);
        if (timer)
            pwTimer(actor, i) = (s16)(timer - 1);
    }
    s16 action = static_cast<s16>(actor->mAction);
    switch (action) {
    case PW_ACTION_DOUSA:
        action_dousa(actor);
        break;
    case PW_ACTION_KOUGEKI:
        action_kougeki(actor);
        break;
    case PW_ACTION_ITAI:
        action_itai(actor);
        break;
    case PW_ACTION_DEMO:
        Pw_action_demo(a);
        break;
    case PW_ACTION_TORITUKU:
        Pw_action_torituku(a);
        break;
    case PW_ACTION_BIG_DEMO:
        Pw_action_big_demo(a);
        break;
    default:
        break;
    }
    if (!static_cast<u8>(actor->m340)) {
        s16 mode = static_cast<s16>(actor->mMode);
        s32 step = (mode == 15 || mode == 16 || mode == 20) ? 500 : 0x1000;
        if (static_cast<s16>(actor->mAction) == PW_ACTION_BIG_DEMO)
            step = static_cast<s16>(actor->m39E);
        Pw_cLib_addCalcAngleS2(pwAddress(actor, offsetof(pw_class, current.angle.y)),
                               (s32) static_cast<s16>(actor->m38C), 1, step);
        Pw_cLib_addCalcAngleS2(pwAddress(actor, offsetof(pw_class, shape_angle.y)),
                               (s32) static_cast<s16>(actor->current.angle.y), 1, step);
    }
    u8 visible = static_cast<u8>(actor->m33E);
    u32 animation;
    if (visible)
        animation = static_cast<u32>(actor->m2C0);
    else
        animation = static_cast<u32>(static_cast<u8>(actor->m33F) ? actor->m2C4 : actor->m2C8);
    Pw_mDoExt_baseAnm_play(animation);
    Pw_mDoMtx_YrotS(l_calcMtx, (s32) static_cast<s16>(actor->current.angle.y));
    Pw_mDoMtx_XrotM(l_calcMtx, (s32) static_cast<s16>(actor->current.angle.x));
    Local<cXyz> velocity, transformed, position;
    velocity->x = 0.f;
    velocity->y = 0.f;
    velocity->z = static_cast<f32>(actor->speedF);
    Pw_MtxPosition(gabi::ea(velocity.get()), gabi::ea(transformed.get()));
    f32 gravity = static_cast<f32>(actor->gravity), x = transformed->x, y = static_cast<f32>(actor->speed.y);
    (actor->speed.x = x);
    f32 nextY = add(y, gravity), z = transformed->z;
    (actor->speed.z = z);
    (actor->speed.y = nextY < -100.f ? -100.f : nextY);
    Local<be<u32>> found;
    if (static_cast<s16>(actor->m346) == 1) {
        u32 id = static_cast<u32>(actor->mKanteraID);
        if (id != 0xFFFFFFFF && Pw_fopAcM_SearchByID(id, gabi::ea(found.get()))) {
            u32 lantern = found->get();
            if (lantern && static_cast<s16>(pwViewfopAc_ac_c(lantern)->mProcName) == 0xC1) {
                x = static_cast<f32>(actor->m2D8.x);
                (actor->attention_info.position.x = x);
                y = static_cast<f32>(actor->m2D8.y);
                (actor->attention_info.position.y = y);
                z = static_cast<f32>(actor->m2D8.z);
                (actor->attention_info.position.z = z);
                (actor->attention_info.position.y = add(y, 40.f));
                (pwVectorWord(&actor->eyePos, 0) = static_cast<u32>(pwVectorWord(&actor->m2D8, 0)));
                (pwVectorWord(&actor->eyePos, 1) = static_cast<u32>(pwVectorWord(&actor->m2D8, 1)));
                (pwVectorWord(&actor->eyePos, 2) = static_cast<u32>(pwVectorWord(&actor->m2D8, 2)));
            }
        }
    } else {
        x = static_cast<f32>(actor->current.pos.x);
        y = static_cast<f32>(actor->current.pos.y);
        (actor->attention_info.position.x = x);
        (actor->eyePos.x = x);
        z = static_cast<f32>(actor->current.pos.z);
        (actor->attention_info.position.z = z);
        (actor->eyePos.z = z);
        (actor->attention_info.position.y = add(y, 200.f));
        (actor->eyePos.y = add(y, 100.f));
    }
    position->x = static_cast<f32>(actor->current.pos.x);
    position->y = static_cast<f32>(actor->current.pos.y);
    position->z = static_cast<f32>(actor->current.pos.z);
    Pw_cM3dGCyl_SetC(pwAddress(actor, offsetof(pw_class, mCyl.mCyl)), gabi::ea(position.get()));
    action = static_cast<s16>(actor->mAction);
    if (action == 4) {
        Pw_cM3dGCyl_SetH(pwAddress(actor, offsetof(pw_class, mCyl.mCyl)), 100.f);
        Pw_cM3dGCyl_SetR(pwAddress(actor, offsetof(pw_class, mCyl.mCyl)),
                         add(static_cast<f32>(pwViewPwDebugRegisters(0x1047B608)->mBodyRadius), 40.f));
    } else {
        Pw_cM3dGCyl_SetH(pwAddress(actor, offsetof(pw_class, mCyl.mCyl)), 200.f);
        Pw_cM3dGCyl_SetR(pwAddress(actor, offsetof(pw_class, mCyl.mCyl)), 80.f);
    }
    u32 play = Pw_dComIfGp_get();
    Pw_cCcS_Set(play + 0x26A4, pwAddress(actor, offsetof(pw_class, mCyl)));
    action = static_cast<s16>(actor->mAction);
    if (action != 3 && action != 5 && action != 4) {
        s16 mode = static_cast<s16>(actor->mMode);
        if (mode != 61 && mode != 110 && mode != 111)
            Pw_body_atari_check(a);
    }
    Pw_fopAcM_posMove(a,
                      static_cast<s16>(actor->m346) == 1 ? pwAddress(actor, offsetof(pw_class, mStts)) : 0u);
    s16 mode = static_cast<s16>(actor->mMode);
    bool advanceAnimation = true;
    if (mode != 81 && mode != 61 && mode != 8) {
        Pw_BG_check(a);
        if (static_cast<s16>(actor->mMode) < 2)
            advanceAnimation = false;
    }
    if (advanceAnimation && !static_cast<s16>(actor->m380)) {
        if (static_cast<f32>(actor->mAcch.m_ground_h) != -1000000000.f) {
            s32 material = 0;
            if (static_cast<u32>(actor->mAcch.m_flags) & 0x20) {
                play = Pw_dComIfGp_get();
                material = Pw_dBgS_GetMtrlSndId(play + 0x12A0,
                                                pwAddress(actor, offsetof(pw_class, mAcch.m_gnd.mPolyIndex)));
            }
            s32 reverb = Pw_dComIfGp_getReverb((s32) static_cast<s8>(actor->current.roomNo));
            Pw_mDoExt_McaMorf_play(static_cast<u32>(actor->mpMorf),
                                   pwAddress(actor, offsetof(pw_class, eyePos)), material, reverb);
        } else
            Pw_mDoExt_McaMorf_play(static_cast<u32>(actor->mpMorf), 0, 0, 0);
    }
    Pw_draw_SUB(a);
    if (static_cast<s16>(actor->m346) == 1) {
        Pw_kantera_calc(a);
        if (!static_cast<s16>(actor->m382)) {
            Pw_kantera_atari_check(a);
            u32 id = static_cast<u32>(actor->mKanteraID);
            if (id != 0xFFFFFFFF && Pw_fopAcM_SearchByID(id, gabi::ea(found.get()))) {
                u32 lantern = found->get();
                if (lantern && static_cast<s16>(pwViewfopAc_ac_c(lantern)->mProcName) == 0xC1) {
                    Pw_cM3dGSph_SetC(pwAddress(actor, offsetof(pw_class, mSph.mSph)),
                                     pwAddress(actor, offsetof(pw_class, m2D8)));
                    Pw_cM3dGSph_SetR(pwAddress(actor, offsetof(pw_class, mSph.mSph)), 40.f);
                    play = Pw_dComIfGp_get();
                    Pw_cCcS_Set(play + 0x26A4, pwAddress(actor, offsetof(pw_class, mSph)));
                }
            }
        }
    }
    return 1;
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
ext 025E8EC0 void Pw_invisibleEntry(u32)
ext 025E8CD8 void Pw_invisibleEntryMaskOff(u32)
data l_headerZeros 1046D3F4
data l_headerPi 1046D3E8
data l_drawBuffers 104B4634
data l_idleArchive 1003835C
data l_damageArchive 1003835F
str 10038448 "Pw" in 0244A37C
str 10038514 "Pw" in 0244DD6C
str 10038517 "Pw" in 0244DE14
str 10038524 "pow" in 0244E398
str 10038528 "Pw" in 0244E398
data l_calcMtx 1018C7B0
data l_resourceControl 101F4F28
data l_saveInfo 101F84DC
data l_currentModel 104B462C
data TORITUKI_ON 101CF0A4
data l_behaviorOverride 1047BB1A
data l_alphaAmplitude 10038470
data l_piPair 10038530
data l_fireSize 101CF108
data l_fireColor 101CF130
data l_sph_src 101CF0C8
data l_cyl_src 101CF13C
ext 02008254 s32 Pw_cBgS_ChkPolySafe(u32, u32)
ext 02008860 s32 Pw_cBgS_LineCross(u32, u32)
ext 02008B4C void Pw_cBgS_LinChk_dt(u32, u32)
ext 02008FEC void Pw_cBgS_LinChk_ct(u32)
ext 0200BD2C void Pw_cCcD_Stts_ct(u32)
ext 0200E240 void Pw_cCcS_Set(u32, u32)
ext 0200E814 u32 Pw_cDT_NamePTbl_GetIndex(u32, u32, u32)
ext 0200ED84 void Pw_cLib_addCalc2(u32, f32, f32, f32)
ext 0200EDC8 void Pw_cLib_addCalc0(u32, f32, f32)
ext 0200F428 void Pw_cLib_addCalcAngleS2(u32, u32, u32, u32)
ext 0200FCD8 void Pw_MtxPosition(u32, u32)
ext 02018034 void Pw_dBgS_AcchCir_dt(u32, u32)
ext 020182E0 void Pw_cM3dGCyl_SetC(u32, u32)
ext 02018428 void Pw_cM3dGCyl_SetH(u32, f32)
ext 020184DC void Pw_cM3dGCyl_SetR(u32, f32)
ext 02018590 void Pw_cM3dGCyl_ct(u32)
ext 02018C8C void Pw_cM3dGSph_SetR(u32, f32)
ext 02018D40 void Pw_cM3dGSph_SetC(u32, u32)
ext 020195B0 s32 Pw_cM_atan2s(f32, f32)
ext 020198D8 f32 Pw_cM_rndF(f32)
ext 02019918 f32 Pw_cM_rndFX(f32)
ext 0201AD78 void Pw_cXyz__pl(u32, u32, u32)
ext 020402C8 s32 Pw_enemy_ice(u32)
ext 02041570 void Pw_enemy_fire(u32)
ext 02041C30 void Pw_enemy_fire_remove(u32)
ext 02041CAC void Pw_enemy_piyo_set(u32)
ext 02449DA8 void Pw_draw_SUB(u32)
ext 02449ECC void Pw_GXColor_to_float(u32, u32)
ext 0244A37C void Pw_anm_init(u32, u32, f32, u32, f32, u32)
ext 0244A4A8 void Pw_kantera_break(u32)
ext 0244A5A8 s32 Pw_Big_pow_gattai_check(u32)
ext 0244A67C s32 Pw_Big_pow_down_check(u32)
ext 0244A768 void Pw_BG_check(u32)
ext 0244A7F0 s32 Pw_Line_check(u32, u32, u32)
ext 0244A9E4 void Pw_alpha_anime(u32)
ext 0244AA98 void Pw_fuwafuwa_calc(u32)
ext 0244ACBC s32 Pw_kougen_hani_check(u32, u32)
ext 0244AE40 void Pw_kyori_sub(u32)
ext 0244B058 s32 Pw_hani_check(u32)
ext 0244B1B8 void Pw_next_dousa_check(u32)
ext 0244B308 void Pw_move_sound(u32)
ext 0244B3AC void Pw_first_mode_change(u32)
ext 0244E184 void Pw_enemy_fire_ct(u32)
ext 0244E210 void Pw_pw_class_ct(u32)
ext 0244E960 void Pw_body_atari_check(u32)
ext 0244F28C void Pw_kantera_atari_check(u32)
ext 0244F730 void Pw_kantera_calc(u32)
ext 0244F960 void Pw_action_demo(u32)
ext 0244FE24 void Pw_action_torituku(u32)
ext 024500C0 void Pw_action_big_demo(u32)
ext 024EECAC s32 Pw_dBgS_GetMtrlSndId(u32, u32)
ext 024EF0BC s32 Pw_dBgS_GetGroundCode(u32, u32)
ext 024EFD9C void Pw_dBgS_Acch_dt(u32, u32)
ext 024EFE94 void Pw_dBgS_AcchCir_dBgS_AcchCir(u32)
ext 024EFF44 void Pw_dBgS_AcchCir_SetWall(u32, f32, f32)
ext 024F0474 void Pw_dBgS_Acch_dBgS_Acch(u32)
ext 024F06B4 void Pw_dBgS_Acch_Set(u32, u32, u32, u32, u32, u32, u32, u32, u32)
ext 024F08A8 void Pw_dBgS_Acch_CrrPos(u32, u32)
ext 024F1AFC void Pw_dBgS_LinChk_Set(u32, u32, u32, u32)
ext 02514F2C void Pw_dCamera_c_Stop(u32)
ext 02514F38 void Pw_dCamera_c_Start(u32)
ext 02515280 void Pw_dCamera_c_SetTrimSize(u32, u32)
ext 02515860 void Pw_dCcD_Stts_dt(u32, u32)
ext 02515A70 void Pw_dCcD_GObjInf_dt(u32, u32)
ext 02515AE8 void Pw_dCcD_Sph_dt(u32, u32)
ext 02515BBC u32 Pw_dCcD_GAtTgCoCommonBase_GetAc(u32)
ext 02515DA0 void Pw_dCcD_GStts_dCcD_GStts(u32)
ext 02515E50 void Pw_dCcD_GStts_Move(u32)
ext 02515F14 void Pw_dCcD_Stts_Init(u32, u32, u32, u32)
ext 02515FB8 void Pw_dCcD_GObjInf_dCcD_GObjInf(u32)
ext 025160DC s32 Pw_dCcD_GObjInf_ChkAtHit(u32)
ext 0251621C void Pw_dCcD_GObjInf_ClrTgHit(u32)
ext 025162A4 s32 Pw_dCcD_GObjInf_ChkTgHit(u32)
ext 02516300 u32 Pw_dCcD_GObjInf_GetTgHitObj(u32)
ext 02516518 void Pw_dCcD_Cyl_Set(u32, u32)
ext 025166F0 void Pw_dCcD_Sph_ct(u32)
ext 0251677C void Pw_dCcD_Sph_Set(u32, u32)
ext 025192A8 void Pw_cc_at_check(u32, u32)
ext 025200D4 u32 Pw_dComIfGp_get()
ext 02520460 s32 Pw_dComIfG_resLoad(u32, u32)
ext 025204C8 void Pw_dComIfG_resDelete(u32, u32)
ext 02520540 s32 Pw_dComIfGp_getReverb(u32)
ext 0252A038 s32 Pw_dDetect_c_chk_light(u32, u32)
ext 02555D0C u32 Pw_dKy_getEnvlight()
ext 025626A4 void Pw_dScnKy_env_light_c_settingTevStruct(u32, u32, u32, u32)
ext 02562F5C void Pw_dScnKy_env_light_setLightTevColorType_HD(u32, u32, u32)
ext 02563F64 void Pw_dKyr_get_vectle_calc(u32, u32, u32)
ext 0259138C void Pw_dMat_ice_c_entryDL(u32, u32, u32)
ext 025A5894 void Pw_dPa_followEcallBack_dPa_followEcallBack(u32, u32, u32)
ext 025A5AC8 void Pw_dPa_followEcallBack_end(u32)
ext 025A847C u32 Pw_dPa_control_c_set(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32)
ext 025AAF88 u32 Pw_dPath_GetRoomPath(u32, u32)
ext 025BA5D4 void Pw_dSv_info_c_onActor(u32, u32, u32)
ext 025BED80 void Pw_dSnap_RegistFig(u32, u32, f32, f32, f32)
ext 025CB610 void Pw_dVibration_c_StopQuake(u32, u32)
ext 025D4ED0 void Pw_fopAc_ac_c_fopAc_ac_c(u32)
ext 025D50BC void Pw_fopAc_ac_c_fopAc_ac_c(u32, u32)
ext 025D5218 u32 Pw_fopAcIt_Judge(u32, u32)
ext 025D54C4 s32 Pw_fopAcM_SearchByID(u32, u32)
ext 025D57E0 void Pw_fopAcM_delete(u32)
ext 025D5834 u32 Pw_fopAcM_create(u32, u32, u32, u32, u32, u32, u32, u32)
ext 025D63E8 s32 Pw_fopAcM_entrySolidHeap(u32, u32, u32)
ext 025D674C void Pw_fopAcM_setCullSizeBox(u32, f32, f32, f32, f32, f32, f32)
ext 025D6800 void Pw_fopAcM_posMove(u32, u32)
ext 025D6894 s32 Pw_fopAcM_searchActorAngleY(u32, u32)
ext 025D68EC f32 Pw_fopAcM_searchActorDistance(u32, u32)
ext 025D7B24 void Pw_fopAcM_orderPotentialEvent(u32, u32, u32, u32)
ext 025D99E8 void Pw_fopAcM_createDisappear(u32, u32, u32, u32, u32)
ext 025DA088 void Pw_fopAcM_setGbaName(u32, u32, u32, u32)
ext 025E1A40 void Pw_cc_seStart(u32, u32, u32, u32)
ext 025E1AA4 void Pw_fopAcM_monsSeStart_HD(u32, u32, u32, u32, u32)
ext 025E4A98 void Pw_mDoExt_McaMorf_setAnm(u32, u32, u32, f32, f32, f32, f32, u32)
ext 025E4F64 u32 Pw_mDoExt_McaMorf_mDoExt_McaMorf(u32, u32, u32, u32, u32, u32, f32, u32, u32, u32, u32, u32, u32)
ext 025E535C void Pw_mDoExt_McaMorf_play(u32, u32, u32, u32)
ext 025E5590 void Pw_mDoExt_McaMorf_entryDL(u32)
ext 025E55A0 void Pw_mDoExt_McaMorf_calc(u32)
ext 025E563C void Pw_mDoExt_McaMorf_stopZelAnime(u32)
ext 025E742C void Pw_mDoExt_baseAnm_play(u32)
ext 025E7820 u32 Pw_mDoExt_btpAnm_ct(u32)
ext 025E789C s32 Pw_mDoExt_btpAnm_init(u32, u32, u32, u32, u32, f32, u32, u32, u32, u32)
ext 025E7B3C void Pw_mDoExt_btpAnm_entry(u32, u32, u32)
ext 025E80D0 u32 Pw_mDoExt_brkAnm_ct(u32)
ext 025E8154 s32 Pw_mDoExt_brkAnm_init(u32, u32, u32, u32, u32, f32, u32, u32, u32, u32)
ext 025E83FC void Pw_mDoExt_brkAnm_entry(u32, u32, f32)
ext 025E895C void Pw_mDoExt_invisibleModel_ct(u32)
ext 025E89F8 void Pw_mDoExt_invisibleModel_dt(u32, u32)
ext 025E8A48 s32 Pw_mDoExt_invisibleModel_create(u32, u32)
ext 025F1884 void Pw_mDoMtx_YrotS(u32, u32)
ext 025F1BF4 void Pw_mDoMtx_XrotM(u32, u32)
ext 025F1C28 void Pw_mDoMtx_YrotM(u32, u32)
ext 025F1C5C void Pw_mDoMtx_ZrotM(u32, u32)
ext 026066C4 u32 Pw_dRes_control_c_getRes(u32, u32, u32)
ext 0273AD10 u32 Pw_operator_new(u32)
ext 0273AF40 void Pw_dl(u32)
ext 0274D458 void Pw_GXColor4f_scaleRGB(u32, u32, f32)
ext 027F2BF8 s32 Pw_J3DFrameCtrl_checkPass(u32, f32)
ext 027F3F8C u32 Pw_J3DMaterial_getShapePacketModelData_HD(u32)
ext 027F3F94 u32 Pw_J3DModelData_getJointTree_HD(u32)
ext 027F58E0 void Pw_J3DModel_setModelXlu_HD(u32, u32)
ext 027F7878 u32 Pw_J3DJoint_getKind_HD(u32)
ext 027F9F0C u32 Pw_J3DMaterial_getColorReg_HD(u32, u32)
ext 028249B0 void Pw_JPASetRMtxTVecfromMtx(u32, u32, u32)
ext 028E8D88 void Pw_PSVECAdd(u32, u32, u32)
ext 028E90D4 void Pw_PSMTXCopy(u32, u32)
ext 028E93CC void Pw_PSMTXTrans(u32, f32, f32, f32)
ext 028EAB2C void Pw_GHS_header_ct(u32)
ext 028ED6F8 void Pw_GHS_section_ct(u32)
ext 028F026C void Pw_register_global_object(u32)
ext 028F4384 f32 Pw_sqrtf_HD(f32)
*/
// clang-format on
