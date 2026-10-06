/**
 * d_a_gnd.cpp (WWHD)
 * Boss - Ganondorf
 *
 * The GameCube source of this unit is all "Nonmatching"
 * stubs: every function here is written from the WWHD code (cking.rpx) and verified against it.
 * GameCube names are kept where the function map matched them.
 */
#include "d/actor/d_a_gnd.h"

#define STR_GND_ANM STR(0x100100C4) /* "Gnd" (anm_init) */

/* 0215413C */
BOOL checkGround(gnd_class* i_this, f32 y) {
    WWHD_FUNC(0x0215413C, BOOL, i_this, y);
    return i_this->current.pos.y < y + 1.0f;
}
VERIFY(0x0215413C, checkGround);

/* 0215415C */
void splash_set(gnd_class* i_this) {
    WWHD_FUNC(0x0215415C, void, i_this);
    JPABaseEmitter* emitter = dComIfGp_particle_set(0x833C, &i_this->current.pos);
    if (emitter != nullptr) {
        u32 e = gabi::ea(emitter);
        u8 r = GF(u8, 0x4CD);
        u8 b = GF(u8, 0x4D1);
        gabi::store<u8>(e + 0x244, r);
        u8 g = GF(u8, 0x4CF);
        gabi::store<u8>(e + 0x246, b);
        gabi::store<u8>(e + 0x245, g);
    }
}
VERIFY(0x0215415C, splash_set);

/* 021541E4 */
void attack_eff_remove(gnd_class* i_this) {
    WWHD_FUNC(0x021541E4, void, i_this);
    for (int i = 0; i < 6; i++) {
        u32 e = GF(u32, 0x1734 + i * 4);
        if (e != 0) {
            /* JPABaseEmitter::becomeInvalidEmitter: mMaxFrame (+0x5C) = -1, flags (+0x254) |= 1 */
            u32 flags = gabi::load<u32>(e + 0x254);
            gabi::store<s32>(e + 0x5C, -1);
            gabi::store<u32>(e + 0x254, flags | 1);
            GF(u32, 0x1734 + i * 4) = 0;
        }
    }
}
VERIFY(0x021541E4, attack_eff_remove);

/* 02154228 */
void anm_init(gnd_class* i_this, int anm, f32 morf, u8 mode, f32 speed, int bas) {
    WWHD_FUNC(0x02154228, void, i_this, anm, morf, mode, speed, bas);
    if (bas >= 0) {
        void* a = dComIfG_getObjectRes(STR_GND_ANM, anm, GND_SAFESTRING_VTBL);
        void* b = dComIfG_getObjectRes(STR_GND_ANM, bas, GND_SAFESTRING_VTBL);
        i_this->mpMorf->setAnm((J3DAnmTransform*)a, mode, morf, speed, 0.0f, -1.0f, b);
    } else {
        void* a = dComIfG_getObjectRes(STR_GND_ANM, anm, GND_SAFESTRING_VTBL);
        i_this->mpMorf->setAnm((J3DAnmTransform*)a, mode, morf, speed, 0.0f, -1.0f, nullptr);
    }
    attack_eff_remove(i_this);
    /* the animation -> effect table: a local copy of u32[18] (0x100100C8); effects u32[18][6]
     * (0x101B5614) */
    for (int i = 0; i < 18; i++) {
        if ((u32)anm == gabi::load<u32>(0x100100C8 + i * 4)) {
            for (int j = 0; j < 6; j++) {
                u32 id = gabi::load<u32>(0x101B5614 + (i * 6 + j) * 4);
                if (id != 0) {
                    JPABaseEmitter* emitter = dComIfGp_particle_set((u16)id, &i_this->current.pos);
                    GF(u32, 0x1734 + j * 4) = gabi::ea(emitter);
                    if (j <= 3) {
                        u32 e = gabi::ea(emitter);
                        u8 r = GF(u8, 0x4CD);
                        u8 b = GF(u8, 0x4D1);
                        gabi::store<u8>(e + 0x244, r);
                        u8 g = GF(u8, 0x4CF);
                        gabi::store<u8>(e + 0x246, b);
                        gabi::store<u8>(e + 0x245, g);
                    }
                }
            }
            return;
        }
    }
}
VERIFY(0x02154228, anm_init);

/* 0215449C */
void* z_s_sub(void* a, void*) {
    WWHD_FUNC(0x0215449C, void*, a, (u32)0);
    if (fopAc_IsActor(a) && a != nullptr && fpcM_GetName(a) == 0xD2) {
        return a;
    }
    return nullptr;
}
VERIFY(0x0215449C, z_s_sub);

/* 021547B0 */
BOOL player_view_check(gnd_class* i_this, s16 angle) {
    WWHD_FUNC(0x021547B0, BOOL, i_this, angle);
    dComIfGp_getPlayer(0); /* the player pointer is fetched but not used */
    s16 d = i_this->shape_angle.y - angle;
    if (d < 0) d = -d;
    return (u16)d < 0x4000;
}
VERIFY(0x021547B0, player_view_check);

/* 02154C34 */
void pos_move(gnd_class* i_this, s8 noTurn) {
    WWHD_FUNC(0x02154C34, void, i_this, noTurn);
    if (noTurn == 0) {
        gabi::Local<cXyz> d;
        cXyz_mi(GXYZ(0x3F0), d, &i_this->current.pos);
        f32 dx = d->x;
        f32 dz = d->z;
        s16 target = cM_atan2s(dx, dz);
        f32 step = (f32)GF(f32, 0x40C) * (f32)GF(f32, 0x410);
        cLib_addCalcAngleS2(&i_this->current.angle.y, target, 5, (s16)gabi::ftoi(step));
        cLib_addCalc2(&GF(f32, 0x410), 1.0f, 1.0f, 0.05f);
    }
    cLib_addCalc2(&i_this->speedF, GF(f32, 0x414), 1.0f, 6.0f);
    gabi::Local<cXyz> v;
    v->x = 0.0f;
    v->y = 0.0f;
    v->z = i_this->speedF;
    mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);
    gabi::Local<cXyz> out;
    MtxPosition(v, out);
    i_this->speed.x = out->x;
    i_this->speed.z = out->z;
    PSVECAdd(&i_this->current.pos, &i_this->speed, &i_this->current.pos);
    f32 sy = i_this->speed.y;
    f32 py = i_this->current.pos.y;
    f32 g = i_this->gravity;
    i_this->current.pos.y = gabi::fadds_ppc(py, sy);
    i_this->speed.y = gabi::fadds_ppc(sy, g);
    i_this->gravity = -5.0f;
}
VERIFY(0x02154C34, pos_move);

/* 02154D88 */
void wait_set(gnd_class* i_this) {
    WWHD_FUNC(0x02154D88, void, i_this);
    if ((s8)GF(u8, 0x60C) <= 1) {
        anm_init(i_this, 0x57, 10.0f, 2, 1.0f, -1);
        gnd_monsSeStart(i_this, 0x4946, 0);
    } else {
        anm_init(i_this, 0x58, 10.0f, 2, 1.0f, -1);
    }
}
VERIFY(0x02154D88, wait_set);

/* 02154E4C */
void* shot_s_sub(void* a, void*) {
    WWHD_FUNC(0x02154E4C, void*, a, (u32)0);
    BOOL actor = fopAc_IsActor(a);
    if (a != nullptr) {
        s16 name = fpcM_GetName(a);
        if ((actor && name == 0x1BE) || name == 0x1B0 || name == 0xA9 || name == 0x1D8) {
            if (!(((fopAc_ac_c*)a)->speedF < 10.0f)) {
                return a;
            }
        }
    }
    return nullptr;
}
VERIFY(0x02154E4C, shot_s_sub);

/* 0215758C */
BOOL daGnd_IsDelete(gnd_class*) {
    WWHD_FUNC(0x0215758C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0215758C, daGnd_IsDelete);

/* 02157594 */
BOOL daGnd_Delete(gnd_class* i_this) {
    WWHD_FUNC(0x02157594, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhs, STR(0x100101D8) /* "Gnd" */);
    dPa_rippleEcallBack_end((dPa_rippleEcallBack*)i_this->mRipple);
    if (i_this->mHIOInit) {
        s8 no = gabi::load<s8>(L_HIO);
        gabi::store<u8>(GND_HIO_INIT, 0);
        mDoHIO_deleteChild(no);
    }
    return TRUE;
}
VERIFY(0x02157594, daGnd_Delete);

/* 02158120: sead::SafeString deleting destructor (this TU's copy, SafeString vtable +0xC) */
void gnd_SafeString_dt(void* str, s32 flags) {
    WWHD_FUNC(0x02158120, void, str, flags);
    if (str != nullptr && (flags & 1)) operator_delete(str);
}
VERIFY(0x02158120, gnd_SafeString_dt);

/* 02159F9C */
void demowait(gnd_class* i_this) {
    WWHD_FUNC(0x02159F9C, void, i_this);
    GF(s16, 0x428) = 5;
    GF(u8, 0x172E) = 0;
}
VERIFY(0x02159F9C, demowait);

/* 02159FB0 */
void yawait(gnd_class* i_this) {
    WWHD_FUNC(0x02159FB0, void, i_this);
    GF(u8, 0x172E) = 0;
    i_this->current.pos.y = 0.0f;
}
VERIFY(0x02159FB0, yawait);

/* 0215D268: sead::SafeString::assureTermination (this TU's copy, SafeString vtable +0x14): empty */
void gnd_SafeString_assureTermination(void*) {
    WWHD_FUNC(0x0215D268, void, (u32)0);
}
VERIFY(0x0215D268, gnd_SafeString_assureTermination);

/* 0215D188: gnd_class::~gnd_class */
void gnd_class_dt(gnd_class* i_this, s32 flags) {
    WWHD_FUNC(0x0215D188, void, i_this, flags);
    if (i_this == nullptr) return;
    gnd_destroy_arr(i_this->mWeponSph, 2, 0x12C, 0x02515AE8, 0, 0);
    gnd_Sph_dt(&i_this->mChestSph, 2);
    gnd_Sph_dt(&i_this->mHeadSph, 2);
    dCcD_Cyl_dt(&i_this->mCyl, 2);
    dCcD_Stts_dt(&i_this->mStts, 2);
    /* dBgS_Acch::~dBgS_Acch (inline): this TU's vtables, then the base destructor */
    GF(u32, 0xF40) = 0x10010034;
    GF(u32, 0xF34) = 0x10010044;
    gabi::call(0x024EFD9C, &i_this->mAcch, 0);
    /* dBgS_AcchCir::~dBgS_AcchCir (inline): its cM3dGCir member */
    gabi::call(0x02018034, GP(0xEF4), 2);
    mDoExt_3DlineMat0_dt(i_this->mLineMat, 2);
    gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) operator_delete(i_this);
}
VERIFY(0x0215D188, gnd_class_dt);

/* 0215788C: gnd_class::gnd_class */
gnd_class* gnd_class_ct(gnd_class* i_this) {
    WWHD_FUNC(0x0215788C, gnd_class*, i_this);
    if (i_this == nullptr) {
        i_this = (gnd_class*)operator_new(sizeof(gnd_class));
        if (i_this == nullptr) return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->m03C8 = gabi::ea(i_this);
    i_this->__vtbl = GND_VTBL;
    /* three members initialised from the same 0x44-byte constant (0x1016E414): six floats, four
     * bytes, four shorts, eight floats */
    const u32 src = 0x1016E414;
    f32 f0[6];
    u8 b[4];
    s16 h[4];
    f32 f1[8];
    for (int k = 0; k < 6; k++) f0[k] = gabi::load<f32>(src + 4 * k);
    for (int k = 0; k < 4; k++) b[k] = gabi::load<u8>(src + 0x18 + k);
    for (int k = 0; k < 4; k++) h[k] = gabi::load<s16>(src + 0x1C + 2 * k);
    for (int k = 0; k < 8; k++) f1[k] = gabi::load<f32>(src + 0x24 + 4 * k);
    static const u32 dst[3] = {0x43C, 0x4FC, 0x580};
    for (int n = 0; n < 3; n++) {
        for (int k = 0; k < 6; k++) GF(f32, dst[n] + 4 * k) = f0[k];
        for (int k = 0; k < 4; k++) GF(u8, dst[n] + 0x18 + k) = b[k];
        for (int k = 0; k < 4; k++) GF(s16, dst[n] + 0x1C + 2 * k) = h[k];
        for (int k = 0; k < 8; k++) GF(f32, dst[n] + 0x24 + 4 * k) = f1[k];
    }
    mDoExt_3DlineMat0_ct(i_this->mLineMat);
    dBgS_AcchCir_ct(&i_this->mAcchCir);
    /* dBgS_ObjAcch (inline): this TU's vtables */
    gabi::call(0x024F0474, &i_this->mAcch);
    GF(u32, 0xF30) = 0x10010024;
    GF(u32, 0xF40) = 0x10010034;
    GF(u32, 0xF34) = 0x10010044;
    GF(u8, 0xF38) = 1;
    dCcD_Stts_ct(&i_this->mStts);
    dCcD_Cyl_ct(&i_this->mCyl, 0x10010014);
    gnd_Sph_ct(&i_this->mHeadSph);
    gnd_Sph_ct(&i_this->mChestSph);
    gnd_construct_array(i_this->mWeponSph, 2, sizeof(dCcD_Sph), 0x025166F0);
    gnd_rippleEcallBack_ct(i_this->mRipple);
    return i_this;
}
VERIFY(0x0215788C, gnd_class_ct);

/* 02157D48: daGnd_HIO_c::daGnd_HIO_c (0x8C bytes, vtable at +0x88) */
void* daGnd_HIO_ct(void* hio) {
    WWHD_FUNC(0x02157D48, void*, hio);
    if (hio == nullptr) {
        hio = operator_new(0x8C);
        if (hio == nullptr) return hio;
    }
    u32 p = gabi::ea(hio);
    gabi::store<f32>(p + 0x18, 1200.0f);
    gabi::store<f32>(p + 0x20, 11.0f);
    gabi::store<f32>(p + 0x1C, 25.0f);
    gabi::store<f32>(p + 0x14, 629.0f);
    gabi::store<f32>(p + 0x2C, -30.0f);
    gabi::store<f32>(p + 0x04, 45.0f);
    gabi::store<u8>(p + 0x0D, 0);
    gabi::store<f32>(p + 0x08, 1.0f);
    gabi::store<f32>(p + 0x10, 5.0f);
    gabi::store<f32>(p + 0x4C, 5.0f);
    gabi::store<u8>(p + 0x01, 0);
    gabi::store<f32>(p + 0x24, -0.5f);
    gabi::store<f32>(p + 0x34, -2.0f);
    gabi::store<f32>(p + 0x30, 20.0f);
    gabi::store<u8>(p + 0x03, 1);
    gabi::store<f32>(p + 0x54, 0.0f);
    gabi::store<f32>(p + 0x28, 100.0f);
    gabi::store<f32>(p + 0x48, 0.0f);
    gabi::store<u8>(p + 0x02, 0);
    gabi::store<u8>(p + 0x0E, 1);
    gabi::store<f32>(p + 0x5C, -2.0f);
    gabi::store<f32>(p + 0x50, 15.0f);
    gabi::store<u32>(p + 0x88, 0x10010064); /* vtable */
    gabi::store<u8>(p + 0x42, 1);
    gabi::store<f32>(p + 0x58, 30.0f);
    gabi::store<f32>(p + 0x44, 35.0f);
    gabi::store<s16>(p + 0x6C, 10);
    gabi::store<f32>(p + 0x38, 630.0f);
    gabi::store<s8>(p + 0x00, -1); /* mNo */
    gabi::store<f32>(p + 0x60, 8.0f);
    gabi::store<s16>(p + 0x66, 30);
    gabi::store<f32>(p + 0x3C, 400.0f);
    gabi::store<s16>(p + 0x6A, 15);
    gabi::store<f32>(p + 0x74, 600.0f);
    gabi::store<u8>(p + 0x85, 0);
    gabi::store<u8>(p + 0x84, 0);
    gabi::store<u8>(p + 0x0C, 0);
    gabi::store<s16>(p + 0x80, 6);
    gabi::store<s16>(p + 0x7E, 3);
    gabi::store<u8>(p + 0x41, 1);
    gabi::store<s16>(p + 0x68, 0);
    gabi::store<s16>(p + 0x6E, 0);
    gabi::store<s16>(p + 0x70, 10);
    gabi::store<u8>(p + 0x40, 0);
    gabi::store<s16>(p + 0x78, 10);
    gabi::store<s16>(p + 0x64, 30);
    gabi::store<s16>(p + 0x82, 0x46);
    gabi::store<s16>(p + 0x7C, 30);
    gabi::store<s16>(p + 0x7A, 1);
    gabi::store<s16>(p + 0x72, 10);
    return hio;
}
VERIFY(0x02157D48, daGnd_HIO_ct);

/* 02157F10 */
void __sinit_d_a_gnd_cpp() {
    WWHD_FUNC(0x02157F10, void);
    /* per-TU header statics (the zeroed object at 0x1046401C, the float pair at 0x1046400C, the
     * objects at 0x10464018 / 0x10464019) */
    for (int i = 3; i >= 0; i--) gabi::store<u32>(0x1046401C + 4 * i, 0);
    __register_global_object(0x101B58F8);
    gabi::store<f32>(0x1046400C, -3.1415927f);
    gabi::store<f32>(0x10464010, 3.1415927f);
    gabi::call(0x028ED6F8, 0x10464018);
    __register_global_object(0x101B5904);
    gabi::call(0x028EAB2C, 0x10464019);
    __register_global_object(0x101B5910);
    /* a cXyz (-20000, -20000, -20000) */
    gabi::store<f32>(0x1046402C, -20000.0f);
    gabi::store<f32>(0x10464030, -20000.0f);
    gabi::store<f32>(0x10464034, -20000.0f);
    daGnd_HIO_ct(gabi::at<void>(L_HIO));
    /* two cXyz[5] tables from .rodata (0x1001022C.., 0x10010268..) */
    for (int k = 0; k < 15; k++) gabi::store<f32>(0x10464044 + 4 * k, gabi::load<f32>(0x1001022C + 4 * k));
    static const u8 b_src[15] = {0, 1, 2, 3, 4, 5, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    for (int k = 0; k < 15; k++) gabi::store<f32>(0x10464080 + 4 * k, gabi::load<f32>(0x10010268 + 4 * b_src[k]));
}
VERIFY(0x02157F10, __sinit_d_a_gnd_cpp);

/* 02157600 */
BOOL useHeapInit(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02157600, BOOL, a_this);
    gnd_class* i_this = (gnd_class*)a_this;
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x100101DC), 0x5C, GND_SAFESTRING_VTBL);
    void* anm = dComIfG_getObjectRes(STR(0x100101DC), 0x55, GND_SAFESTRING_VTBL);
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, (J3DAnmTransform*)anm, 2, 1.0f, 0, -1, 1,
                                                  nullptr, 0, 0x11020203);
    i_this->mpMorf = morf;
    if (morf == nullptr || morf->getModel() == nullptr) return FALSE;

    void* brk = operator_new(0x78);
    if (brk != nullptr) brk = gnd_brkAnm_ct(brk);
    i_this->mpBrk = (mDoExt_brkAnm*)brk;
    if (brk == nullptr) return FALSE;
    J3DModel* model = i_this->mpMorf->getModel();
    void* res = dComIfG_getObjectRes(STR(0x100101DC), 0x5F, GND_SAFESTRING_VTBL);
    if (!gnd_brkAnm_init(i_this->mpBrk, J3DModel_getModelData(model), res, 1, 2, 1.0f, 0, -1, false, 0)) return FALSE;

    void* btk = operator_new(0x74);
    if (btk != nullptr) btk = gnd_btkAnm_ct(btk);
    i_this->mpBtk = (mDoExt_btkAnm*)btk;
    if (btk == nullptr) return FALSE;
    model = i_this->mpMorf->getModel();
    res = dComIfG_getObjectRes(STR(0x100101DC), 0x63, GND_SAFESTRING_VTBL);
    if (!gnd_btkAnm_init(i_this->mpBtk, J3DModel_getModelData(model), res, 1, 0, 1.0f, 0, -1, false, 0)) return FALSE;

    void* btp = operator_new(0x74);
    if (btp != nullptr) btp = gnd_btpAnm_ct(btp);
    i_this->mpBtp = (mDoExt_btpAnm*)btp;
    if (btp == nullptr) return FALSE;
    model = i_this->mpMorf->getModel();
    res = dComIfG_getObjectRes(STR(0x100101DC), 0x66, GND_SAFESTRING_VTBL);
    if (!gnd_btpAnm_init(i_this->mpBtp, J3DModel_getModelData(model), res, 1, 2, 1.0f, 0, -1, false, 0)) return FALSE;

    if (!mDoExt_3DlineMat0_init(i_this->mLineMat, 4, 0x14, FALSE)) return FALSE;
    return TRUE;
}
VERIFY(0x02157600, useHeapInit);

/* 02157B30 */
cPhs_State daGnd_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02157B30, cPhs_State, a_this);
    gnd_class* i_this = (gnd_class*)a_this;
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) gnd_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    cPhs_State ret = dComIfG_resLoad(&i_this->mPhs, STR(0x100101E0) /* "Gnd" */);
    if (ret == cPhs_COMPLEATE_e) {
        gabi::store<u32>(0x10464008, 0);
        i_this->m03D4 = fopAcM_GetParam(a_this) & 0xF;
        if (!fopAcM_entrySolidHeap(a_this, 0x02157600 /* useHeapInit */, 0x96000)) {
            return cPhs_ERROR_e;
        }
        gabi::store<u32>(gabi::ea(a_this) + 0x39C, 4); /* attention_info.flags */
        gabi::store<u8>(gabi::ea(a_this) + 0x38A, 4);  /* attention_info.distances[2] */
        if (gabi::load<u8>(GND_HIO_INIT) == 0) {
            i_this->mHIOInit = 1;
            gabi::store<u8>(GND_HIO_INIT, 1);
            gabi::store<s8>(L_HIO, mDoHIO_createChild(STR(0x100101E4) /* "ガノン" */, gabi::at<void>(L_HIO)));
        }
        i_this->mBtHeight = REG0_F(6) + 400.0f;
        i_this->mBtBodyR = REG0_F(5) + 150.0f;
        i_this->mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir, &a_this->speed, nullptr, nullptr);
        GF(u8, 0xF2C) = 0;
        i_this->mAcchCir.SetWall(200.0f, 200.0f);
        i_this->mStts.Init(0xFF, 0xFF, a_this);
        i_this->mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101B58B4) /* cc_cyl_src */);
        i_this->mCyl.SetStts(&i_this->mStts);
        i_this->mHeadSph.Set(gabi::at<dCcD_SrcSph>(0x101B57F4) /* head_sph_src */);
        i_this->mHeadSph.SetStts(&i_this->mStts);
        i_this->mChestSph.Set(gabi::at<dCcD_SrcSph>(0x101B5834) /* chest_sph_src */);
        i_this->mChestSph.SetStts(&i_this->mStts);
        for (int i = 0; i < 2; i++) {
            i_this->mWeponSph[i].Set(gabi::at<dCcD_SrcSph>(0x101B5874) /* wepon_sph_src */);
            i_this->mWeponSph[i].SetStts(&i_this->mStts);
        }
        gabi::call(0x02154EE0, i_this); /* daGnd_Execute */
        i_this->max_health = 0x4B;
        i_this->health = 0x4B;
        if (i_this->m03D4 != 0) {
            mDoAud_bgmStart(0x8000004D);
        }
    }
    return ret;
}
VERIFY(0x02157B30, daGnd_Create);

/* 021544EC */
BOOL daGnd_Draw(gnd_class* i_this) {
    WWHD_FUNC(0x021544EC, BOOL, i_this);
    s16 blur = GF(s16, 0x18BC);
    if (blur >= 1) {
        if (blur == 1) {
            GF(s16, 0x18BC) = 0;
            mDoGph_offBlure();
        } else {
            mDoGph_setBlureRate((u8)blur);
            mDoGph_onBlure();
        }
    }
    J3DModel* model = i_this->mpMorf->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    s16 flash = GF(s16, 0x178C);
    if (flash != 0) {
        /* body flash: the tevStr's C0 colour (0x1B0..) and fog offset (0x1B8) */
        s16 add = GF(s16, 0x1786);
        s16 r = (s16)(GF(s16, 0x1B0) + add);
        if (r > 0xFF) r = 0xFF;
        s16 g = (s16)(GF(s16, 0x1B2) + add);
        if (g > 0xFF) g = 0xFF;
        s16 b = (s16)(GF(s16, 0x1B2) + add / 2);
        if (b > 0xFF) b = 0xFF;
        if (flash > (s16)(REG_S(8, 5) + 20)) {
            cLib_addCalcAngleS2(&GF(s16, 0x1786), 0x118, 1, 0x1E);
            cLib_addCalc2(&GF(f32, 0x1788), -50000.0f, 1.0f, 5000.0f);
        } else {
            cLib_addCalcAngleS2(&GF(s16, 0x1786), 0, 1, 0xE);
            cLib_addCalc0(&GF(f32, 0x1788), 1.0f, 2500.0f);
        }
        GF(s16, 0x1B0) = (u8)r;
        GF(s16, 0x1B2) = (u8)g;
        GF(s16, 0x1B4) = (u8)b;
        GF(f32, 0x1B8) = gabi::fadds_ppc(GF(f32, 0x1B8), GF(f32, 0x1788));
    }
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    mDoExt_brkAnm* brk = i_this->mpBrk;
    mDoExt_brkAnm_entry(brk, J3DModel_getModelData(model), gabi::load<f32>(gabi::ea(brk) + 4));
    mDoExt_btkAnm* btk = i_this->mpBtk;
    mDoExt_btkAnm_entry(btk, J3DModel_getModelData(model), gabi::load<f32>(gabi::ea(btk) + 4));
    mDoExt_btpAnm* btp = i_this->mpBtp;
    s16 frame = (s16)gabi::ftoi(gabi::load<f32>(gabi::ea(btp) + 4));
    mDoExt_btpAnm_entry(btp, J3DModel_getModelData(model), frame);
    i_this->mpMorf->entryDL();
    dSnap_RegistFig(0xCE, i_this, 1.0f, 1.0f, 1.0f);
    if (gabi::load<u8>(L_HIO + 3) != 0) {
        mDoExt_3DlineMat0_update(i_this->mLineMat, 0x14, REG0_F(3) + 2.25f, gabi::at<GXColor>(0x101B57C4), 2, &i_this->tevStr);
        gnd_set3DlineMat(i_this->mLineMat);
    }
    return TRUE;
}
VERIFY(0x021544EC, daGnd_Draw);

/* 02158E80 */
void finish(gnd_class* i_this) {
    WWHD_FUNC(0x02158E80, void, i_this);
    dComIfGp_getPlayer(0); /* fetched, not used */
    s16 mode = GF(s16, 0x3EC);
    GF(s16, 0x428) = 5;
    GF(u8, 0x172E) = 0;
    switch ((u32)(s32)mode) {
    case 0:
        GF(s16, 0x18BE) = 100;
        GF(s16, 0x3EC) = 1;
        break;
    case 1:
        GF(s16, 0x3EC) = 2;
        break;
    case 2:
        GF(s16, 0x3EC) = 3;
        break;
    }
}
VERIFY(0x02158E80, finish);

static inline BOOL gnd_morfIsStop(mDoExt_McaMorf* morf) {
    return (morf->mFrameCtrl.mState & 1) || morf->mFrameCtrl.mRate == 0.0f;
}

/* 02158134 */
void attack_last(gnd_class* i_this) {
    WWHD_FUNC(0x02158134, void, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s16 mode = GF(s16, 0x3EC);
    s32 frame = gabi::ftoi(i_this->mpMorf->mFrameCtrl.mFrame);
    GF(f32, 0x1728) = REG0_F(13) + 110.0f;
    GF(u8, 0x172E) = 0;
    switch ((u32)(s32)mode) {
    case 0:
        anm_init(i_this, 0x52, 5.0f, 0, 1.0f, -1);
        gnd_monsSeStart(i_this, 0x494D, 0);
        GF(s16, 0x3EC) = 1;
        break;
    case 1:
        if (gnd_morfIsStop(i_this->mpMorf)) {
            anm_init(i_this, 0x51, 2.0f, 0, 1.0f, 0x1E);
            GF(s16, 0x3EC) = 2;
        }
        if (frame > gabi::load<s16>(L_HIO + 0x78)) {
            GF(u8, 0x172F) = 4;
        }
        break;
    case 2:
        if (frame >= gabi::load<s16>(L_HIO + 0x7A) && frame <= gabi::load<s16>(L_HIO + 0x7C)) {
            GF(u8, 0x172F) = 4;
        }
        if ((u32)(frame - 6) < 4) {
            GF(u8, 0x1719) = 1;
        }
        if (gnd_morfIsStop(i_this->mpMorf)) {
            s16 ang = i_this->shape_angle.y;
            GF(s16, 0x3EA) = 0;
            i_this->current.angle.y = ang;
            GF(s16, 0x3EC) = 0;
            GF(s16, 0x420) = 0;
        }
        break;
    }
    if (gabi::load<u32>(gabi::ea(player) + 0x3BC) & 0x20000000) {
        gabi::store<f32>(dComIfGp_ea() + 0x5B44, 0.0f);
        GF(u8, 0x172E) = 0;
        GF(s16, 0x3EA) = 0x1E;
        GF(s16, 0x3EC) = 0;
        GF(s16, 0x428) = 5;
    }
}
VERIFY(0x02158134, attack_last);

/* 0215CE60 */
void body_flash(gnd_class* i_this) {
    WWHD_FUNC(0x0215CE60, void, i_this);
    s16 t = GF(s16, 0x178E);
    J3DModel* model = i_this->mpMorf->getModel();
    if (t != 0) {
        t = t + 1;
        if (t > 100) {
            t = 0;
        }
        GF(s16, 0x178E) = t;
    }
    /* 15 flash points: start time u32[15] (0x101B559C), joint u32[15] (0x101B5560), scale f32[15]
     * (0x101B55D8); timers s16 at 0x1758, offsets cXyz at 0x1808, emitters at 0x1790 */
    for (int i = 0; i < 15; i++) {
        if ((u32)(s32)t == gabi::load<u32>(0x101B559C + i * 4)) {
            GF(s16, 0x1758 + i * 2) = 0x63;
        } else {
            s16 timer = GF(s16, 0x1758 + i * 2);
            if (timer == 0) {
                u32 e = GF(u32, 0x1790 + i * 4);
                if (e != 0) {
                    gnd_becomeInvalidEmitter(e);
                    GF(u32, 0x1790 + i * 4) = 0;
                }
                t = GF(s16, 0x178E);
                continue;
            }
            GF(s16, 0x1758 + i * 2) = timer - 1;
        }
        s32 jnt = gabi::load<s32>(0x101B5560 + i * 4);
        PSMTXCopy(gnd_getAnmMtx(model, jnt), calc_mtx());
        gabi::Local<cXyz> pos;
        MtxPosition(GXYZ(0x1808 + i * 0xC), pos);
        f32 timer = (f32)(s16)GF(s16, 0x1758 + i * 2);
        f32 k = REG_F(8, 0) + 0.01f;
        f32 scale = gabi::load<f32>(0x101B55D8 + i * 4) * timer * k;
        u32 e = GF(u32, 0x1790 + i * 4);
        if (e == 0) {
            JPABaseEmitter* emitter = dComIfGp_particle_set(0x3ED, pos);
            GF(u32, 0x1790 + i * 4) = gabi::ea(emitter);
        } else {
            u8 v = gabi::load<u8>(e + 0x262);
            f32 y = pos->y;
            f32 x = pos->x;
            f32 z = pos->z;
            if (v >= 7) y = -y;
            gabi::store<f32>(e + 0x22C, x);
            gabi::store<f32>(e + 0x230, y);
            gabi::store<f32>(e + 0x234, z);
            e = GF(u32, 0x1790 + i * 4);
            gabi::store<f32>(e + 0x220, scale);
            gabi::store<f32>(e + 0x224, scale);
            gabi::store<f32>(e + 0x228, scale);
            gabi::store<f32>(e + 0x238, scale);
            gabi::store<f32>(e + 0x23C, scale);
            gabi::store<f32>(e + 0x240, scale);
        }
        t = GF(s16, 0x178E);
    }
}
VERIFY(0x0215CE60, body_flash);

/* 02154810: ke_move (hair), with ke_control and ke_pos_set inlined. Four strands gnd_ke_s
 * (0x1E0 each at 0x618: cXyz pos[20], cXyz vel[20]); the line material's strands get a copy. */
void ke_move(gnd_class* i_this) {
    WWHD_FUNC(0x02154810, void, i_this);
    J3DModel* model = i_this->mpMorf->getModel();
    gabi::Local<cXyz> axis;
    gabi::Local<cXyz> base;
    gabi::Local<cXyz> step;
    gabi::Local<cXyz> out;
    for (int i = 0; i < 4; i++) {
        u32 ke = gabi::ea(i_this) + 0x618 + i * 0x1E0;
        cXyz* pos = gabi::at<cXyz>(ke);
        cXyz* vel = gabi::at<cXyz>(ke + 0xF0);
        /* ke_pos_set: the root at the strand's joint (u32[4] at 0x101B54F4, offset f32[4] at 0x101B5504) */
        s32 jnt = gabi::load<s32>(0x101B54F4 + i * 4);
        PSMTXCopy(gnd_getAnmMtx(model, jnt), calc_mtx());
        Mtx34* m = calc_mtx();
        f32 m00 = m->m[0][0];
        f32 m20 = m->m[2][0];
        f32 m10 = m->m[1][0];
        axis->z = m20;
        axis->x = m00;
        axis->y = m10;
        f32 scale = std_sqrtf(PSVECSquareMag(axis));
        BOOL ev = gnd_startCheckOld(STR(0x10010138) /* "endhr" */);
        f32 reg4 = REG0_F(4);
        if (ev && gabi::load<u32>(0x101D600C) >= 300) {
            scale = 0.0f;
        }
        f32 off = gabi::load<f32>(0x101B5504 + i * 4);
        base->y = off;
        base->z = off;
        base->x = gabi::fadds_ppc(reg4, -20.0f);
        MtxPosition(base, &pos[0]);
        /* ke_control */
        f32 groundY = gabi::fadds_ppc(i_this->mAcch.m_ground_h, 3.0f);
        f32 len = gabi::fmuls_ppc(gabi::fmuls_ppc(gabi::load<f32>(L_HIO + 4), scale), 0.5f);
        step->x = 0.0f;
        step->y = 0.0f;
        f32 reg1 = REG0_F(1);
        if (scale > 0.05f) { /* ble: taken on NaN */
            step->z = len;
        } else {
            step->z = 0.0f;
        }
        f32 fall = gabi::fadds_ppc(reg1, -5.0f);
        f32 damp = gabi::fadds_ppc(REG0_F(2), 0.73f);
        for (int j = 1; j < 20; j++) {
            f32 y = gabi::fadds_ppc(gabi::fadds_ppc(pos[j].y, vel[j].y), fall);
            f32 dx = gabi::fadds_ppc(gabi::fsubs_ppc(pos[j].x, pos[j - 1].x), vel[j].x);
            if (y < groundY) y = groundY; /* bge: taken on NaN */
            f32 dy = gabi::fsubs_ppc(y, pos[j - 1].y);
            f32 dz = gabi::fadds_ppc(gabi::fsubs_ppc(pos[j].z, pos[j - 1].z), vel[j].z);
            s16 ax = -cM_atan2s(dy, dz);
            s16 ay = cM_atan2s(dx, std_sqrtf(gabi::fmadds(dy, dy, dz * dz)));
            gnd_XrotS(calc_mtx(), ax);
            mDoMtx_YrotM(calc_mtx(), ay);
            MtxPosition(step, out);
            vel[j].copy(pos[j]); /* the old position, integer copy */
            pos[j].x = gabi::fadds_ppc(pos[j - 1].x, out->x);
            pos[j].y = gabi::fadds_ppc(pos[j - 1].y, out->y);
            pos[j].z = gabi::fadds_ppc(pos[j - 1].z, out->z);
            vel[j].x = gabi::fmuls_ppc(gabi::fsubs_ppc(pos[j].x, vel[j].x), damp);
            vel[j].y = gabi::fmuls_ppc(gabi::fsubs_ppc(pos[j].y, vel[j].y), damp);
            vel[j].z = gabi::fmuls_ppc(gabi::fsubs_ppc(pos[j].z, vel[j].z), damp);
        }
        /* the line material's strand i: 16-byte entries at *(mLineMat + 0x144), points at +0 */
        u32 dst = gabi::load<u32>(gabi::load<u32>(gabi::ea(i_this) + 0xEDC) + i * 0x10);
        for (int k = 0; k < 20; k++) {
            for (int w = 0; w < 3; w++) gabi::store<u32>(dst + k * 0xC + w * 4, gabi::load<u32>(ke + k * 0xC + w * 4));
        }
    }
}
VERIFY(0x02154810, ke_move);

/* 02158F10 */
void damage(gnd_class* i_this) {
    WWHD_FUNC(0x02158F10, void, i_this);
    dComIfGp_getPlayer(0); /* fetched, not used */
    s8 req = GF(s8, 0x60D);
    GF(u8, 0x172E) = 0;
    if (req != 0) {
        if (req == 1) {
            GF(u8, 0x172E) = 0;
            GF(s16, 0x3EC) = 0;
            GF(s16, 0x3EA) = 4;
            GF(s8, 0x60D) = 0;
            return;
        }
        GF(s8, 0x60D) = 0;
        GF(s16, 0x18BE) = 10;
        return;
    }
    s16 mode = GF(s16, 0x3EC);
    switch ((u32)(s32)mode) {
    case 0:
        anm_init(i_this, 0x38, 2.0f, 0, 1.0f, -1);
        GF(s16, 0x3EC) = 1;
        GF(s16, 0x41E) = gabi::load<s16>(L_HIO + 0x82);
        gnd_monsSeStart(i_this, 0x4952, 0);
        /* fall through */
    case 1:
        if (gnd_morfIsStop(i_this->mpMorf)) {
            anm_init(i_this, 0x39, 2.0f, 2, 1.0f, -1);
            GF(s16, 0x3EC) = 9;
            GF(s16, 0x420) = 10;
        }
        break;
    case 5:
        anm_init(i_this, 0x3A, 2.0f, 0, 1.0f, -1);
        gnd_monsSeStart(i_this, 0x4951, 0);
        GF(s16, 0x3EC) = 6;
        GF(s16, 0x41E) = gabi::load<s16>(L_HIO + 0x82);
        /* fall through */
    case 6:
        if (gnd_morfIsStop(i_this->mpMorf)) {
            anm_init(i_this, 0x3B, 2.0f, 2, 1.0f, -1);
            GF(s16, 0x3EC) = 10;
            GF(s16, 0x420) = 15;
        }
        break;
    case 9:
    case 10: {
        if (GF(s16, 0x420) == 0) {
            gnd_monsSeStart(i_this, 0x4953, 0);
            GF(s16, 0x420) = (mode == 9) ? 0x14 : 0x1E;
        }
        bool recover;
        if (GF(s16, 0x18BE) == 0) {
            if (GF(s16, 0x41E) >= 0x1E) break;
            if ((s8)i_this->health < 0x19 && GF(s8, 0x60C) == 2) {
                s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
                if (player_view_check(i_this, ang) &&
                    fopAcM_searchActorDistance(i_this, dComIfGp_getPlayer(0)) < gabi::load<f32>(L_HIO + 0x74)) {
                    GF(s16, 0x3EC) = 0;
                    GF(u8, 0x172E) = 1;
                    GF(s16, 0x3EA) = 5;
                    GF(s16, 0x42A) = 0x1E;
                    return;
                }
            }
        }
        recover = GF(s16, 0x41E) == 0;
        if (recover) {
            GF(s16, 0x3EA) = 0;
            GF(s16, 0x3EC) = 0;
            GF(s16, 0x42A) = 0x1E;
            GF(u8, 0x172E) = 1;
        }
        break;
    }
    case 0x14:
        anm_init(i_this, 0x3C, 3.0f, 0, 1.0f, -1);
        gnd_monsSeStart(i_this, 0x4950, 0);
        GF(s16, 0x3EC) = 10;
        break;
    }
    pos_move(i_this, 1);
    GF(f32, 0x414) = 0.0f;
}
VERIFY(0x02158F10, damage);

/* fopAcM_monsSeStart as inlined in damage_check: only the eyePos check */
static inline void gnd_monsSeStart_noActorCheck(gnd_class* a, u32 id) {
    if (gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        gabi::call(0x025E1AA4, id, &a->eyePos, pid, 0, reverb);
    }
}

static inline void gnd_emitterColor(gnd_class* i_this, JPABaseEmitter* emitter) {
    u32 e = gabi::ea(emitter);
    u8 r = GF(u8, 0x4CD);
    u8 b = GF(u8, 0x4D1);
    gabi::store<u8>(e + 0x244, r);
    u8 g = GF(u8, 0x4CF);
    gabi::store<u8>(e + 0x246, b);
    gabi::store<u8>(e + 0x245, g);
}

/* 021594B0 */
void damage_check(gnd_class* i_this) {
    WWHD_FUNC(0x021594B0, void, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    i_this->mStts.Move();
    /* the sword: parried (Tg) or hitting (At) */
    for (int i = 0; i < 2; i++) {
        dCcD_Sph* sph = &i_this->mWeponSph[i];
        if (sph->ChkTgHit()) {
            gnd_def_se_set(i_this, sph->GetTgHitObj(), 0x40);
            GF(f32, 0x42C) = REG0_F(19) + 30.0f;
            s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
            u8 n = GF(u8, 0x609);
            GF(u8, 0x60A) = 1;
            GF(s16, 0x430) = ang + 0x8000;
            GF(u8, 0x609) = n + 1;
            gabi::Local<cXyz> hit;
            cXyz* hitPos = sph->GetTgHitPosP();
            hit->x = hitPos->x;
            hit->y = hitPos->y;
            hit->z = hitPos->z;
            gnd_SordFlush_set(hit, 0);
            gabi::Local<cXyz> scale;
            scale->z = 2.0f;
            scale->x = 2.0f;
            scale->y = 2.0f;
            dComIfGp_particle_set(0xC, hitPos, &i_this->shape_angle, scale);
            dComIfGp_particle_set(0x8380, hitPos, &i_this->shape_angle);
        }
        if (sph->ChkAtHit()) {
            void* obj = gnd_GetAtHitObj(sph);
            if (obj != nullptr) {
                u32 stts = gabi::load<u32>(gabi::ea(obj) + 0x44);
                if (stts == 0) continue;
                fopAc_ac_c* ac = gabi::at<fopAc_ac_c>(gabi::load<u32>(stts + 0xC));
                if (ac != nullptr && fpcM_GetName(ac) == 0xA8) {
                    if (GF(s16, 0x3EC) == 0x16) {
                        GF(u8, 0x608) = 1;
                        gabi::Local<cXyz> vpos;
                        u32 vib = gabi::ea(dComIfGp_getVibration());
                        s16 reg = REG0_S(2);
                        vpos->x = 0.0f;
                        vpos->y = 1.0f;
                        vpos->z = 0.0f;
                        gabi::call<BOOL>(0x025CB374, vib, reg + 6, -0x21, vpos.get());
                        continue;
                    }
                    if (daPy_vfunc3C(player)) {
                        gabi::Local<cXyz> vpos;
                        u32 vib = gabi::ea(dComIfGp_getVibration());
                        vpos->x = 0.0f;
                        s16 reg = REG0_S(2);
                        vpos->y = 1.0f;
                        vpos->z = 0.0f;
                        gabi::call<BOOL>(0x025CB374, vib, reg + 3, -0x21, vpos.get());
                        s16 mode = GF(s16, 0x3EC);
                        if (mode == 2 || mode == 7) {
                            GF(u8, 0x608) = 1;
                        }
                    }
                }
            }
        }
    }
    if (GF(s16, 0x428) != 0) return;

    /* the chest */
    if (i_this->mChestSph.ChkTgHit()) {
        void* obj = i_this->mChestSph.GetTgHitObj();
        if (obj == nullptr) return;
        u32 stts = gabi::load<u32>(gabi::ea(obj) + 0x44);
        bool toBody = false;
        if (stts == 0) {
            toBody = GF(s8, 0x172E) == 0;
        } else {
            fopAc_ac_c* ac = gabi::at<fopAc_ac_c>(gabi::load<u32>(stts + 0xC));
            if (ac != nullptr && fpcM_GetName(ac) == 0x1D8 && gabi::load<u8>(gabi::ea(ac) + 0x3AC) != 0) {
                s16 d = i_this->shape_angle.y - ac->current.angle.y;
                if (d < 0) d = -d;
                if (((u16)d < 0x4000 && GF(s8, 0x60C) < 2) || gabi::load<u8>(gabi::ea(ac) + 0x780) != 0) {
                    toBody = true;
                } else {
                    GF(u8, 0x172E) = 2;
                    GF(s16, 0x3EA) = 10;
                    GF(s16, 0x3EC) = 15;
                    GF(u8, 0x609) = 0;
                    return;
                }
            } else {
                toBody = GF(s8, 0x172E) == 0;
            }
        }
        if (!toBody) {
            s8 guard = GF(s8, 0x172E);
            GF(s16, 0x3EA) = 10;
            if (guard != 1) {
                GF(u8, 0x609) = 0;
                GF(s16, 0x3EC) = 15;
                return;
            }
            gnd_def_se_set(i_this, i_this->mChestSph.GetTgHitObj(), 0x40);
            GF(f32, 0x42C) = REG0_F(19) + 30.0f;
            s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
            gabi::Local<cXyz> hit;
            cXyz* hitPos = i_this->mChestSph.GetTgHitPosP();
            hit->y = hitPos->y;
            hit->z = hitPos->z;
            GF(s16, 0x430) = ang + 0x8000;
            hit->x = hitPos->x;
            gnd_SordFlush_set(hit, 0);
            gabi::Local<cXyz> scale;
            scale->x = 2.0f;
            scale->y = 2.0f;
            scale->z = 2.0f;
            dComIfGp_particle_set(0xC, hitPos, &i_this->shape_angle, scale);
            dComIfGp_particle_set(0x8380, hitPos, &i_this->shape_angle);
            gnd_monsSeStart(i_this, 0x494E, 0);
            int anm = 0x2F;
            if (cM_rndF(1.0f) < 0.5f) anm = 0x2E;
            anm_init(i_this, anm, 2.0f, 0, 1.0f, -1);
            GF(s16, 0x41E) = 0x14;
            GF(s16, 0x3EC) = 10;
            return;
        }
    }

    /* the body */
    if (GF(s16, 0x42A) != 0 && !i_this->mChestSph.ChkTgHit()) return;
    bool head = false;
    if (!i_this->mCyl.ChkTgHit() && !i_this->mHeadSph.ChkTgHit() && !i_this->mChestSph.ChkTgHit()) return;
    GF(s16, 0x428) = 7;
    bool chest = false;
    gabi::Local<CcAtInfo_gnd> info;
    fopAc_ac_c* hitAc;
    if (i_this->mHeadSph.ChkTgHit()) {
        info->mpObj = gabi::ea(i_this->mHeadSph.GetTgHitObj());
        info->pParticlePos = gabi::ea(i_this->mHeadSph.GetTgHitPosP());
        head = true;
    } else if (i_this->mChestSph.ChkTgHit()) {
        info->mpObj = gabi::ea(i_this->mChestSph.GetTgHitObj());
        info->pParticlePos = gabi::ea(i_this->mChestSph.GetTgHitPosP());
        chest = true;
    } else {
        info->mpObj = gabi::ea(i_this->mCyl.GetTgHitObj());
        info->pParticlePos = gabi::ea(i_this->mCyl.GetTgHitPosP());
    }
    hitAc = gnd_cc_at_check(i_this, info);
    info->mpActor = gabi::ea(hitAc);
    if (info->mResultingAttackType == 1 && gabi::load<u8>(gabi::ea(player) + 0x69E8) != 0) {
        GF(s16, 0x428) = 2;
    }
    if (chest && hitAc != nullptr) {
        /* light arrow in the chest: flash */
        dComIfGp_particle_set(0x8382, gabi::at<cXyz>(info->pParticlePos), &hitAc->current.angle);
        fopAc_ac_c* ac2 = gabi::at<fopAc_ac_c>(info->mpActor);
        dComIfGp_particle_set(0x8383, gabi::at<cXyz>(info->pParticlePos), &ac2->current.angle);
        GF(s16, 0x178E) = 1;
        GF(s16, 0x178C) = REG_S(8, 4) + 0x1E;
        for (int k = 0; k < 15; k++) {
            cXyz* p = GXYZ(0x1808 + k * 0xC);
            p->x = cM_rndFX(30.0f);
            p->y = cM_rndFX(30.0f);
            p->z = cM_rndFX(30.0f);
        }
        GF(f32, 0x190C) = REG0_F(19) + 20.0f;
    } else {
        u8 t = gabi::load<u8>(gabi::ea(player) + 0x3AC);
        if (t == 5 || t == 0xF) {
            dComIfGp_particle_set(0x8381, gabi::at<cXyz>(info->pParticlePos), &i_this->shape_angle);
        } else {
            JPABaseEmitter* emitter = dComIfGp_particle_set(0x837F, gabi::at<cXyz>(info->pParticlePos), &i_this->shape_angle);
            if (emitter != nullptr) gnd_emitterColor(i_this, emitter);
        }
    }
    /* health */
    s8 h = i_this->health;
    bool toEnd = false; /* E/F selection below */
    if (h > 0x19) {
        if (h <= 0x32 && GF(s8, 0x60C) == 0) {
            fopAc_ac_c* act = gabi::at<fopAc_ac_c>(info->mpActor);
            if (act != nullptr && fpcM_GetName(act) == 0x1D8) {
                if (gabi::load<u8>(gabi::ea(act) + 0x3AC) != 0) {
                    GF(s8, 0x60D) = 1;
                    if ((s8)i_this->health < 0) i_this->health = 0;
                }
            } else {
                i_this->health = 0x32;
            }
        }
    } else {
        if (GF(s8, 0x60C) == 1) {
            i_this->health = 0x19;
            GF(s8, 0x60C) = 2;
            GF(s8, 0x60D) = 2;
        } else if (h < 0) {
            i_this->health = 0;
        }
    }
    (void)toEnd;
    if (GF(s16, 0x3EA) == 0xB) {
        GF(s16, 0x3EC) = 0x14;
    } else {
        GF(s16, 0x3EA) = 0xB;
        GF(s16, 0x3EC) = head ? 5 : 0;
        GF(u8, 0x172E) = 0;
    }
    gnd_monsSeStart_noActorCheck(i_this, 0x4809);
}
VERIFY(0x021594B0, damage_check);

/* 027EC9E8 JUTReport(x, y, fmt, ...) (debug print) */
static inline void gnd_JUTReport(s32 x, s32 y, u32 fmt, s32 v) { gabi::call(0x027EC9E8, x, y, fmt, v); }

/* defence0: the dodge animations by m610 (u32 table at 0x101B5514) */
static inline int gnd_defenceAnm(s8 i) { return gabi::load<s32>(0x101B5514 + i * 4); }

/* 02158360 */
void defence0(gnd_class* i_this) {
    WWHD_FUNC(0x02158360, void, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    int end = 0;
    GF(u8, 0x60B) = 10;
    f32 dist = fopAcM_searchActorDistance(i_this, dComIfGp_getPlayer(0));
    f32 lim = (i_this->mAcch.m_ground_h + 200.0f) + REG0_F(11);
    s16 mode = GF(s16, 0x3EC);
    if (i_this->current.pos.y > lim) {
        if (mode < 0xF) {
            mode = 0x14;
            GF(s16, 0x3EC) = mode;
        }
    }
    switch ((u32)(s32)mode) {
    case 0: {
        f32 anmMorf;
        if (GF(s8, 0x60C) == 2 &&
            (f32)(player->current.pos.y - i_this->mAcch.m_ground_h) > REG0_F(19) + 300.0f) {
            GF(s8, 0x610) = 0;
            GF(s8, 0x609) = 2;
            anmMorf = 4.0f;
        } else {
            bool set5 = false;
            if (gabi::load<u8>(gabi::ea(player) + 0x3AC) == 0xF && cM_rndF(1.0f) < 0.8f) {
                set5 = true;
            } else if (GF(s8, 0x60F) == 1 && cM_rndF(1.0f) < 0.05f && GF(s8, 0x60C) == 2) {
                set5 = true;
            }
            if (set5) {
                GF(s8, 0x610) = 5;
                s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
                GF(s16, 0x612) = ang + REG0_S(4) + 0x7800;
            }
            anmMorf = 4.0f;
        }
        anm_init(i_this, gnd_defenceAnm(GF(s8, 0x610)), anmMorf, 0, 1.0f, -1);
        GF(s8, 0x60E) = 0;
        goto jump_or_se;
    }
    case 0xA: {
        if (GF(s8, 0x60C) == 2 &&
            (f32)(player->current.pos.y - i_this->mAcch.m_ground_h) > REG0_F(19) + 300.0f &&
            GF(s8, 0x610) != 0) {
            GF(s8, 0x610) = 0;
            GF(s8, 0x609) = 2;
            anm_init(i_this, gnd_defenceAnm(0), 2.0f, 0, 1.0f, -1);
            GF(s8, 0x60E) = 0;
            goto jump_or_se;
        }
        i_this->gravity = REG0_F(11) + -4.0f;
        if (checkGround(i_this, 0.0f)) {
            GF(f32, 0x414) = 0.0f;
            bool jump = false;
            if (gabi::load<u8>(L_HIO + 0xE) == 0) {
                if (dist < REG0_F(2) + 400.0f && GF(s8, 0x610) != 5 && GF(s16, 0x420) == (s16)(REG0_S(5) + 1)) {
                    jump = true;
                }
            } else {
                s8 t = GF(s8, 0x60E);
                if (t != 0) {
                    t = t - 1;
                    GF(s8, 0x60E) = t;
                    if (t == 0) jump = true;
                }
            }
            if (jump) {
                GF(s16, 0x3EA) = 3;
                f32 r = cM_rndF(1.0f);
                GF(u8, 0x172E) = 0;
                GF(s16, 0x3EC) = (r < 0.5f) ? 0 : 1;
            }
        }
        gnd_JUTReport(0x1E, 0xC8, 0x10010084 /* "TIME 0 %d" */, GF(s16, 0x41E));
        gnd_JUTReport(0x1E, 0xDC, 0x10010090 /* "TIME 1 %d" */, GF(s16, 0x420));
        if (GF(s16, 0x41E) == 0 && GF(s16, 0x420) == 0 && checkGround(i_this, 0.0f) && GF(s8, 0x60E) == 0) {
            end = 1;
            break;
        }
        if (GF(s8, 0x60A) == 0) break;
        GF(s8, 0x60A) = 0;
        if (checkGround(i_this, 0.0f)) {
            s8 n = GF(s8, 0x609);
            bool tryY;
            if (n >= 8) {
                if (cM_rndF(1.0f) < 0.3f) {
                    GF(s8, 0x609) = 0;
                    GF(s16, 0x3EC) = 15;
                    GF(s16, 0x420) = REG_S(10, 4) + 10;
                    goto se2;
                }
                tryY = GF(s8, 0x609) >= 4;
            } else {
                tryY = n >= 4;
            }
            if (tryY && cM_rndF(1.0f) < 0.5f) {
                GF(s8, 0x1730) = REG0_S(6) + 0xD;
                GF(s16, 0x420) = REG_S(10, 4) + 10;
                goto se2;
            }
        }
        {
            s8 n = GF(s8, 0x609);
            GF(s16, 0x3EC) = 0;
            if (n <= 1) {
                GF(s8, 0x610) = 0;
            } else {
                for (int k = 20; k != 0; k--) {
                    s8 v = (s8)gabi::ftoi(cM_rndF(4.99f));
                    s8 cur = GF(s8, 0x610);
                    if ((u32)(s32)v != (u32)(s32)cur) {
                        GF(s8, 0x60F) = cur;
                        GF(s8, 0x610) = v;
                        break;
                    }
                }
            }
            GF(s16, 0x420) = REG_S(10, 4) + 10;
        }
    se2:
        if (gabi::ea(&i_this->eyePos) != 0) {
            gnd_monsSeStart_noActorCheck(i_this, GF(s16, 0x3EC) < 2 ? 0x488B : 0x488A);
        }
        break;
    }
    case 0xF: {
        anm_init(i_this, 0x30, 3.0f, 0, 1.0f, 0x12);
        GF(s16, 0x3EC) = GF(s16, 0x3EC) + 1;
        gnd_monsSeStart_noActorCheck(i_this, 0x494F);
        i_this->speed.y = REG0_F(5) + 33.0f;
        f32 x = i_this->current.pos.x;
        f32 sp = REG0_F(6) + 30.0f;
        f32 z = i_this->current.pos.z;
        i_this->speedF = sp;
        GF(f32, 0x414) = sp;
        i_this->current.angle.y = cM_atan2s(-x, -z);
        s16 t = 0;
        if (cM_rndF(1.0f) < 0.5f) t = 100;
        GF(s16, 0x420) = t;
        GF(u8, 0x172E) = 0;
        splash_set(i_this);
        GF(u8, 0x438) = 0;
        break;
    }
    case 0x10:
        i_this->gravity = REG0_F(7) + -2.0f;
        if (checkGround(i_this, 0.0f)) {
            if (GF(s8, 0x438) == 0) {
                splash_set(i_this);
                GF(u8, 0x438) = 1;
            }
            mDoExt_McaMorf* morf = i_this->mpMorf;
            GF(f32, 0x414) = 0.0f;
            if (gnd_morfIsStop(morf)) {
                end = (GF(s16, 0x420) != 0) ? 1 : 2;
            }
        } else {
            GF(u8, 0x172E) = 0;
        }
        break;
    case 0x14:
        anm_init(i_this, 0x56, 3.0f, 0, 1.0f, -1);
        GF(s16, 0x3EC) = GF(s16, 0x3EC) + 1;
        break;
    case 0x15:
        if (gnd_morfIsStop(i_this->mpMorf)) {
            end = 1;
        } else if (checkGround(i_this, 0.0f)) {
            GF(f32, 0x414) = 0.0f;
            anm_init(i_this, 0x31, 3.0f, 0, 1.0f, -1);
            GF(s16, 0x3EC) = GF(s16, 0x3EC) + 1;
        }
        break;
    case 0x16:
        if (gnd_morfIsStop(i_this->mpMorf)) end = 1;
        break;
    }
    goto tail;

jump_or_se:
    if (gabi::load<u8>(L_HIO + 0x42) != 0 && GF(s8, 0x610) != 5 && GF(s8, 0x609) >= 3 && cM_rndF(1.0f) < 0.2f) {
        bool j1;
        if (cM_rndF(1.0f) < 0.5f) {
            GF(f32, 0x614) = 65536.0f;
        } else {
            GF(f32, 0x614) = -65536.0f;
        }
        j1 = gabi::load<u8>(L_HIO + 0xE) == 1;
        if (!j1) {
            i_this->speed.y = REG0_F(9) + 15.0f;
            GF(f32, 0x414) = REG0_F(10) + -5.0f;
            GF(f32, 0x42C) = 0.0f;
            splash_set(i_this);
        } else {
            i_this->speed.y = REG0_F(7) + 20.0f;
            GF(f32, 0x414) = REG0_F(8) + -20.0f;
            s16 reg = REG0_S(2);
            GF(f32, 0x42C) = 0.0f;
            GF(s8, 0x60E) = reg + 10;
            splash_set(i_this);
        }
        gnd_monsSeStart_noActorCheck(i_this, 0x4956);
    } else {
        gnd_monsSeStart_noActorCheck(i_this, 0x494E);
    }
    GF(s16, 0x3EC) = 10;

tail:
    if (GF(s8, 0x610) == 5) {
        cLib_addCalcAngleS2(&i_this->shape_angle.y, GF(s16, 0x612), 2, (s16)(REG0_S(8) + 0x2000));
    } else {
        s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
        cLib_addCalcAngleS2(&i_this->shape_angle.y, ang, 2, (s16)(REG0_S(8) + 0x2000));
    }
    GF(u8, 0x1718) = 10;
    GF(u8, 0x1719) = 11;
    pos_move(i_this, 1);
    if (end != 0) {
        i_this->current.angle.y = i_this->shape_angle.y;
        if (end == 1) {
            GF(s16, 0x3EA) = 0;
            GF(s16, 0x3EC) = 0;
        } else {
            GF(s16, 0x3EA) = 1;
            anm_init(i_this, 0x2B, 5.0f, 0, 1.0f, 0xF);
            GF(s16, 0x3EC) = 8;
        }
    }
}
VERIFY(0x02158360, defence0);
