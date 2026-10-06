/**
 * d_a_klft.cpp (WWHD)
 * Object - Forbidden Woods - Lift (wooden platform hanging from two ropes, swung by the wind
 * and the Deku Leaf).
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_klft.cpp) has only "Nonmatching" stubs for this unit: every function here is
 * written from the WWHD code (cking.rpx) and verified against it. Names follow the GameCube
 * symbols; field names are descriptive guesses (GameCube offsets unknown).
 */
#include "bindings.h"

#define M_arcname_del STR(0x10013094)  /* "Klft" (Delete) */
#define M_arcname_heap STR(0x1001309C) /* "Klft" (CallbackCreateHeap) */
#define M_arcname_crt STR(0x100130DC)  /* "Klft" (Create) */
#define SAFESTRING_VTBL 0x10012FCC
#define KLFT_VTBL 0x10012FF4           /* HD: klft_class vtable */
#define AAB_VTBL 0x10012FE4            /* this TU's cM3dGAab vtable */
#define utiwa_sph_src gabi::at<dCcD_SrcSph>(0x101B8448)
#define p_co_cyl_src gabi::at<dCcD_SrcCyl>(0x101B8488)
#define l_himo_color gabi::at<GXColor>(0x101B8424)
/* file statics */
#define wind_vec_ea 0x10464BC4   /* cXyz* */
#define wind_power_ea 0x10464BC8 /* f32* */
#define wind_angle_ea 0x10464BD4 /* s16 */

enum { PROC_PLAYER = 0xA8 };

struct mDoExt_3DlineMat1_l {
    /* 0x000 */ u8 _000[0x130];
    /* 0x130 */ be<u32> __vtbl;
    /* 0x134 */ u8 _134[0x184 - 0x134];
    /* 0x184 */ be<u32> mpLines; /* {cXyz* pos; u8* size; ...} */
};

struct klft_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ be<s16> mCounter;
    /* 0x3B6 */ u8 _3B6[2];
    /* 0x3B8 */ gptr<J3DModel> mpModel;
    /* 0x3BC */ gptr<mDoExt_McaMorf> mpMorf[2];   /* rope anchors */
    /* 0x3C4 */ cXyz mHimoTop[2];                 /* path points 2 and 3 */
    /* 0x3DC */ be<s16> mAnchorRotY;
    /* 0x3DE */ be<u8> mType;
    /* 0x3DF */ be<u8> mSwitchNo;
    /* 0x3E0 */ cXyz mSway;
    /* 0x3EC */ be<f32> mSwayX;
    /* 0x3F0 */ u8 _3F0[4];
    /* 0x3F4 */ be<f32> mSwayZ;
    /* 0x3F8 */ be<f32> mSwayXTarget;
    /* 0x3FC */ u8 _3FC[4];
    /* 0x400 */ be<f32> mSwayZTarget;
    /* 0x404 */ be<f32> mRotXTarget;
    /* 0x408 */ u8 _408[4];
    /* 0x40C */ be<f32> mRotZTarget;
    /* 0x410 */ be<f32> mRotX;
    /* 0x414 */ u8 _414[4];
    /* 0x418 */ be<f32> mRotZ;
    /* 0x41C */ csXyz mShakeAngle;
    /* 0x422 */ u8 _422[6];
    /* 0x428 */ Mtx34 mBgMtx;
    /* 0x458 */ gptr<dBgW> mpBgW;
    /* 0x45C */ mDoExt_3DlineMat1_l mLineMat;
    /* 0x5E4 */ cXyz mPathP0;
    /* 0x5F0 */ cXyz mPathP1;
    /* 0x5FC */ cXyz mPathDir;
    /* 0x608 */ cXyz mCenter;
    /* 0x614 */ be<s16> mTiltDir;
    /* 0x616 */ be<s16> mTilt;
    /* 0x618 */ be<s16> mWheelRot;
    /* 0x61A */ be<s16> mHitTimer;
    /* 0x61C */ be<f32> mWheelSe;
    /* 0x620 */ be<f32> mSink;
    /* 0x624 */ be<f32> mSinkTarget;
    /* 0x628 */ be<f32> mPos;       /* 20..80 along the path */
    /* 0x62C */ be<f32> mSpeed;
    /* 0x630 */ be<f32> mSpeedTarget;
    /* 0x634 */ dCcD_Stts mStts;
    /* 0x670 */ dCcD_Sph mSph;      /* Deku Leaf wind target */
    /* 0x79C */ dCcD_Cyl mCoCyl[2];
    /* 0x9FC */ dCcD_Sph mWindSph[2];
    /* 0xC54 */ be<s16> mRideTimer;
    /* 0xC56 */ u8 _C56[2];
};
WWHD_OFFSET(klft_class, mLineMat, 0x45C);
WWHD_OFFSET(klft_class, mStts, 0x634);
WWHD_OFFSET(klft_class, mWindSph, 0x9FC);
WWHD_OFFSET(klft_class, mRideTimer, 0xC54);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* J3D (HD): model user area +0xB8, matrix block +0x2C (flags +4, matrices +0x10) */
static inline u32 j3dSys_model() { return gabi::load<u32>(0x104B462C); }
static inline Mtx34* getAnmMtx_l(u32 model, s32 jntNo) {
    u32 blk = gabi::load<u32>(model + 0x2C);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10)); /* HD: marks the joint matrices dirty */
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jntNo * 0x30);
}
/* 027F3F94 (matcher: __nw): J3DModelData joint tree; joint count (u16) at +8 */
static inline u16 J3DModelData_getJointNum_l(u32 d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
static inline void J3DModelData_setJointCallBack_l(u32 data, u32 i, u32 cb) {
    u32 n = gabi::load<u32>(data + 4);
    u32 p = gabi::load<u32>(data + 8);
    if (i < n) p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
static inline void mDoMtx_XrotS_l(Mtx34* m, s16 x) { gabi::call(0x025F18EC, m, x); }
/* 0200796C / 02007990: CPad main stick X / Y of a port (f1) */
static inline f32 CPad_GET_STICK_POS_X(u32 port) { return gabi::call<f32>(0x0200796C, port); }
static inline f32 CPad_GET_STICK_POS_Y(u32 port) { return gabi::call<f32>(0x02007990, port); }
static inline f32 std_fabsf(f32 x) { return std::fabs(x); }
/* 02518CC8 def_se_set(actor, hit obj, material) */
static inline void def_se_set(fopAc_ac_c* a, void* obj, u32 p) { gabi::call(0x02518CC8, a, obj, p); }
static inline cXyz* dKyw_get_wind_vec() { return gabi::call<cXyz*>(0x0257DAA8); }
static inline u32 dKyw_get_wind_power() { return gabi::call<u32>(0x0257DB04); }
static inline s16 cM_rad2s(f32 r) { return gabi::call<s16>(0x02019510, r); }
/* 0201A4DC csXyz::operator+(const csXyz&) const: the result comes back in r3:r4 (x<<16|y, z<<16) */
static inline void csXyz_pl(const csXyz* a, const csXyz* b, csXyz* out) {
    u32 r3 = gabi::call<u32>(0x0201A4DC, a, b);
    u32 r4 = gabi::cpu->r[4];
    out->x = (s16)(r3 >> 16);
    out->y = (s16)(r3 & 0xFFFF);
    out->z = (s16)(r4 >> 16);
}
/* fopAcM_seStart on current.pos (HD inline: null checks on the actor and the position) */
static inline void klft_seStartCurrent(fopAc_ac_c* a, u32 id, u32 prm) {
    if (a != nullptr && gabi::ea(&a->current.pos) != 0)
        mDoAud_seStart(id, &a->current.pos, prm, dComIfGp_getReverb(a->current.roomNo));
}
/* mDoExt_3DlineMat1_c (HD): constructor 025EB82C, init 025EBA58, update 025ED1BC, destructor
 * 025EB8B8 (the matcher names it draw); dComIfGd_set3DlineMat: sort packets at play+0x5FB4 */
static inline BOOL mDoExt_3DlineMat1_init(mDoExt_3DlineMat1_l* l, u16 a, u16 b, void* tex, s32 c) {
    return gabi::call<BOOL>(0x025EBA58, l, a, b, tex, c);
}
static inline void mDoExt_3DlineMat1_update(mDoExt_3DlineMat1_l* l, u16 segs, GXColor* color, dKy_tevstr_c* tev) {
    gabi::call(0x025ED1BC, l, segs, color, tev);
}
static inline void dComIfGd_set3DlineMat(mDoExt_3DlineMat1_l* l) {
    u32 pkt = dComIfGp_ea() + 0x5FB4;
    s32 id = gabi::call_ptr<s32>(gabi::load<u32>(l->__vtbl + 0x14), l); /* getMaterialID() */
    gabi::call(0x025EDD04, pkt + id * 0x9C, l);
}
static inline void __construct_array(void* p, s32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void __destroy_arr(void* p, s32 n, u32 size, u32 dtor) { gabi::call(0x028F0164, p, n, size, dtor, 0, 0); }
static inline void dCcD_Sph_ct_l(void* p) { gabi::call(0x025166F0, p); }
static inline f32 REG0F(int i) { return REG_F(0, i); }

/* 0219F608 */
static void ride_call_back(dBgW* bgw, fopAc_ac_c* i_actor, fopAc_ac_c* rider) {
    WWHD_FUNC(0x0219F608, void, bgw, i_actor, rider);
    klft_class* i_this = (klft_class*)i_actor;
    gabi::Local<cXyz> tmp;
    gabi::Local<cXyz> arg;
    gabi::Local<cXyz> now;
    gabi::Local<cXyz> old;

    mDoMtx_YrotS(calc_mtx(), (s16)-i_this->current.angle.y);
    cXyz_mi(&rider->current.pos, tmp.get(), &i_this->mCenter);
    arg->copy(*tmp.get());
    MtxPosition(arg.get(), now.get());
    cXyz_mi(&rider->old.pos, tmp.get(), &i_this->mCenter);
    arg->copy(*tmp.get());
    MtxPosition(arg.get(), old.get());

    f32 x = now->x;
    f32 z = now->z;
    if (rider != nullptr && fpcM_GetName(rider) == PROC_PLAYER) {
        i_this->mRideTimer = 10;
    }
    i_this->mSinkTarget = -50.0f;
    f32 dist = std_sqrtf(gabi::fmadds(x, x, z * z));
    s16 tilt = (s16)gabi::ftoi(dist * ((REG0F(0) + 30.0f) / i_this->scale.z));
    cLib_addCalcAngleS2(&i_this->mTilt, tilt, 10, 0x800);
    cLib_addCalcAngleS2(&i_this->mTiltDir, cM_atan2s(now->x, now->z), 2, 0x2000);

    f32 sx = CPad_GET_STICK_POS_X(0);
    f32 sy = CPad_GET_STICK_POS_Y(0);
    if (std_fabsf(sx) + std_fabsf(sy) > 0.1f || std_fabsf(i_this->mSpeed) > 0.0001f) {
        f32 dz = std_fabsf(now->z - old->z) * (REG0F(4) + 100.0f);
        if (dz > REG0F(6) + 200.0f) {
            i_this->mRotXTarget = dz;
        }
        f32 dx = std_fabsf(now->x - old->x) * (REG0F(4) + 100.0f);
        if (dx > REG0F(6) + 200.0f) {
            i_this->mRotZTarget = dx;
        }
        f32 k = REG0F(8) + 2.0f;
        dx = std_fabsf(now->x - old->x) * k;
        if (dx > 10.0f && i_this->mSwayX < dx) {
            i_this->mSwayXTarget = dx;
        }
        dz = std_fabsf(now->z - old->z) * (REG0F(8) + 2.0f);
        if (dz > 10.0f && i_this->mSwayZ < dz) {
            i_this->mSwayZTarget = dz;
        }
    }
}
VERIFY(0x0219F608, ride_call_back);

/* 0219F8A4: joint 1 of the lift model (the wheel) turns with mWheelRot */
static BOOL nodeCallBack_main(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0219F8A4, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 model = j3dSys_model();
        klft_class* i_this = gabi::at<klft_class>(gabi::load<u32>(model + 0xB8));
        s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (i_this != nullptr && jntNo == 1) {
            PSMTXCopy(getAnmMtx_l(model, 1), calc_mtx());
            mDoMtx_XrotM(calc_mtx(), i_this->mWheelRot);
            Mtx34* dst = getAnmMtx_l(model, 1);
            mtx_copy(dst, calc_mtx());
            PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868));
        }
    }
    return TRUE;
}
VERIFY(0x0219F8A4, nodeCallBack_main);

/* 0219F9C0: joint 3 of the rope anchors turns with mAnchorRotY */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0219F9C0, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 model = j3dSys_model();
        klft_class* i_this = gabi::at<klft_class>(gabi::load<u32>(model + 0xB8));
        s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (i_this != nullptr) {
            PSMTXCopy(getAnmMtx_l(model, jntNo), calc_mtx());
            mDoMtx_YrotM(calc_mtx(), i_this->mAnchorRotY);
            Mtx34* dst = getAnmMtx_l(model, jntNo);
            mtx_copy(dst, calc_mtx());
            PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868));
        }
    }
    return TRUE;
}
VERIFY(0x0219F9C0, nodeCallBack);

/* 0219FAE4: himo_Draw inlined */
static BOOL daKlft_Draw(klft_class* i_this) {
    WWHD_FUNC(0x0219FAE4, BOOL, i_this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(i_this->mpModel);
    dComIfGd_setList();
    mDoExt_3DlineMat1_update(&i_this->mLineMat, 0x14, l_himo_color, &i_this->tevStr);
    dComIfGd_set3DlineMat(&i_this->mLineMat);
    for (int i = 0; i < 2; i++) {
        mDoExt_McaMorf* morf = i_this->mpMorf[i];
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, morf->mpModel, &i_this->tevStr);
        i_this->mpMorf[i]->updateDL();
    }
    return TRUE;
}
VERIFY(0x0219FAE4, daKlft_Draw);

/* 0219FBFC: klft_move and himo_move inlined */
static BOOL daKlft_Execute(klft_class* i_this) {
    WWHD_FUNC(0x0219FBFC, BOOL, i_this);
    dComIfGp_get();
    if (i_this->mHitTimer != 0) {
        i_this->mHitTimer = (s16)(i_this->mHitTimer - 1);
    }
    if (i_this->mRideTimer != 0) {
        i_this->mRideTimer = (s16)(i_this->mRideTimer - 1);
    }
    cXyz* wind_vec = dKyw_get_wind_vec();
    gabi::store<u32>(wind_vec_ea, gabi::ea(wind_vec));
    gabi::store<s16>(wind_angle_ea, cM_atan2s(wind_vec->x, wind_vec->z));
    gabi::store<u32>(wind_power_ea, dKyw_get_wind_power());

    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    i_this->mCounter = (s16)(i_this->mCounter + 1);

    /* klft_move */
    s16 ang;
    f32 pow;
    f32 spd_step = 0.01f;
    if (i_this->mRideTimer != 0) {
        pow = 1.0f;
        ang = player->shape_angle.y;
    } else {
        ang = (s16)(player->shape_angle.y + 0x8000);
        pow = 0.5f;
    }
    if (i_this->mSph.ChkTgHit() || i_this->mWindSph[0].ChkTgHit() || i_this->mWindSph[1].ChkTgHit()) {
        gabi::Local<cXyz> v;
        gabi::Local<cXyz> out;
        if (i_this->mSph.ChkTgHit()) {
            mDoMtx_YrotS(calc_mtx(), (s16)(ang - i_this->current.angle.y));
            v->x = 0.0f;
            v->y = 0.0f;
            v->z = (REG0F(13) + 0.5f) * pow;
            MtxPosition(v.get(), out.get());
            i_this->mSpeedTarget = out->z;
        } else {
            s16 a = i_this->mWindSph[0].ChkTgHit() ? -0x8000 : 0;
            mDoMtx_YrotS(calc_mtx(), a);
            v->y = 0.0f;
            v->x = 0.0f;
            v->z = (REG0F(16) + 0.85f) * pow;
            spd_step = 0.1f;
            MtxPosition(v.get(), out.get());
            i_this->mSpeedTarget = out->z;
        }
        v->z = 0.0f;
        v->y = 0.0f;
        v->x = (REG0F(15) + 500.0f) * pow;
        MtxPosition(v.get(), (cXyz*)&i_this->mRotXTarget);
        i_this->mHitTimer = (s16)(REG0_S(5) + 0x3C);
        mDoMtx_YrotS(calc_mtx(), ang);
        v->x = 0.0f;
        v->y = 0.0f;
        v->z = (REG0F(16) + 20.0f) * pow;
        MtxPosition(v.get(), (cXyz*)&i_this->mSwayXTarget);
    }
    if (i_this->mHitTimer == 0x28) {
        klft_seStartCurrent(i_this, 0x3821, 0);
    }
    cLib_addCalc2(&i_this->mSpeed, i_this->mSpeedTarget, 1.0f, spd_step);
    cLib_addCalc0(&i_this->mSpeedTarget, 1.0f, REG0F(14) + 0.001f);
    f32 p = i_this->mPos + i_this->mSpeed;
    if (p > 80.0f) {
        i_this->mPos = 80.0f;
        i_this->mSpeed = 0.0f;
    } else if (p < 20.0f) {
        i_this->mPos = 20.0f;
        i_this->mSpeed = 0.0f;
    } else {
        i_this->mPos = p;
    }
    i_this->mWheelRot = (s16)(i_this->mWheelRot + (s16)gabi::ftoi(i_this->mSpeed * -2500.0f));
    if (std_fabsf(i_this->mSpeed) > 0.01f) {
        f32 v = std_fabsf(i_this->mSpeed) * (REG0F(0) + 500.0f);
        u32 vol;
        if (!(v < 2147483648.0f)) {
            vol = (u32)gabi::ftoi(v - 2147483648.0f) + 0x80000000u;
        } else {
            vol = (u32)gabi::ftoi(v);
        }
        if (vol > 100) {
            vol = 100;
        }
        for (int i = 0; i < 2; i++) {
            mDoAud_seStart(0x61B1, &i_this->mHimoTop[i], vol, dComIfGp_getReverb(i_this->current.roomNo));
        }
        f32 se = i_this->mWheelSe + std_fabsf(i_this->mSpeed);
        i_this->mWheelSe = se;
        f32 lim = REG0F(1) + 3.0f;
        if (se > lim) {
            i_this->mWheelSe = se - lim;
            klft_seStartCurrent(i_this, 0x3822, vol);
        }
    }
    for (int i = 0; i < 2; i++) {
        if (i_this->mCoCyl[i].ChkTgHit()) {
            def_se_set(i_this, i_this->mCoCyl[i].GetTgHitObj(), 0xB);
        }
    }
    cLib_addCalcAngleS2(&i_this->mTilt, 0, 10, 0x200);
    cLib_addCalcAngleS2(&i_this->current.angle.x, 0, 10, 0x200);
    cLib_addCalcAngleS2(&i_this->current.angle.z, 0, 10, 0x200);

    s32 t = i_this->mCounter;
    f32 rx = i_this->mRotX;
    i_this->mShakeAngle.x = (s16)gabi::ftoi(cM_ssin(t * 0x5DC) * rx);
    f32 rz = i_this->mRotZ;
    i_this->mShakeAngle.z = (s16)gabi::ftoi(cM_ssin(t * 0x514) * rz);

    if (i_this->mHitTimer != 0) {
        cLib_addCalc2(&i_this->mRotX, i_this->mRotXTarget, 1.0f, 10.0f);
    } else if (std_fabsf(i_this->mRotXTarget) > REG0F(2) + 100.0f) {
        cLib_addCalc2(&i_this->mRotX, i_this->mRotXTarget, 1.0f, REG0F(4) + 10.0f);
    } else {
        cLib_addCalc2(&i_this->mRotX, i_this->mRotXTarget, 1.0f, REG0F(3) + 2.0f);
    }
    if (i_this->mHitTimer != 0) {
        cLib_addCalc2(&i_this->mRotZ, i_this->mRotZTarget, 1.0f, 40.0f);
    } else if (std_fabsf(i_this->mRotZTarget) > REG0F(2) + 100.0f) {
        cLib_addCalc2(&i_this->mRotZ, i_this->mRotZTarget, 1.0f, REG0F(4) + 10.0f);
    } else {
        cLib_addCalc2(&i_this->mRotZ, i_this->mRotZTarget, 1.0f, REG0F(3) + 2.0f);
    }
    f32 pos = i_this->mPos;
    if (i_this->mHitTimer == 0) {
        i_this->mRotZTarget = 0.0f;
        i_this->mRotXTarget = 0.0f;
    }

    s16 sa = cM_rad2s(pos * 0.01f * 3.1415927f);
    t = i_this->mCounter;
    f32 s = cM_ssin(sa);
    i_this->mSway.x = cM_ssin(t * 0x2EE) * i_this->mSwayX * s;
    i_this->mSway.z = cM_ssin(t * 0x384) * i_this->mSwayZ * s;
    i_this->mSway.y = cM_ssin(t * 0x5DC) * (i_this->mSwayX + i_this->mSwayZ) * (REG0F(1) + 1.0f) * s;

    if (i_this->mHitTimer != 0) {
        cLib_addCalc2(&i_this->mSwayX, i_this->mSwayXTarget, 1.0f, 10.0f);
    } else {
        cLib_addCalc2(&i_this->mSwayX, i_this->mSwayXTarget, 1.0f,
                      (10.0f - std_fabsf(i_this->mSwayXTarget) >= 0.0f) ? 0.25f : 1.2f);
    }
    if (i_this->mHitTimer != 0) {
        cLib_addCalc2(&i_this->mSwayZ, i_this->mSwayZTarget, 1.0f, 10.0f);
    } else {
        cLib_addCalc2(&i_this->mSwayZ, i_this->mSwayZTarget, 1.0f,
                      (10.0f - std_fabsf(i_this->mSwayZTarget) >= 0.0f) ? 0.25f : 1.2f);
    }
    if (i_this->mHitTimer == 0) {
        i_this->mSwayXTarget = 0.0f;
        i_this->mSwayZTarget = 0.0f;
    }

    csXyz_pl(&i_this->current.angle, &i_this->mShakeAngle, &i_this->shape_angle);
    {
        gabi::Local<cXyz> a;
        gabi::Local<cXyz> b;
        gabi::Local<cXyz> c;
        cXyz_ml(&i_this->mPathDir, a.get(), i_this->mPos);
        cXyz_ml(a.get(), b.get(), 0.01f);
        cXyz_pl(&i_this->mPathP0, c.get(), b.get());
        i_this->home.pos.x = c->x;
        f32 y = c->y;
        i_this->home.pos.z = c->z;
        f32 k = (i_this->mSink + -100.0f) + REG0F(0);
        i_this->home.pos.y = y + gabi::fmadds(s, k, REG0F(2) + -100.0f);
    }
    cLib_addCalc2(&i_this->mSink, i_this->mSinkTarget, 0.1f, 5.0f);
    i_this->mSinkTarget = 0.0f;
    {
        gabi::Local<cXyz> a;
        cXyz_pl(&i_this->home.pos, a.get(), &i_this->mSway);
        i_this->current.pos.copy(*a.get());
    }

    /* himo_move */
    {
        u32 lines = i_this->mLineMat.mpLines;
        u32 size_p = gabi::load<u32>(lines + 4);
        u32 pos_p = gabi::load<u32>(lines + 0);
        gabi::Local<cXyz> d;
        gabi::Local<cXyz> d0;
        gabi::Local<cXyz> d1;
        gabi::Local<cXyz> m;
        gabi::Local<cXyz> r;
        cXyz_mi(&i_this->current.pos, d.get(), &i_this->mPathP0);
        d0->copy(*d.get());
        cXyz_mi(&i_this->current.pos, d.get(), &i_this->mPathP1);
        d1->x = d->x;
        d0->y = d0->y - 25.0f;
        d1->y = d->y - 25.0f;
        d1->z = d->z;
        for (int k = 0; k < 20; k++) {
            f32 f;
            cXyz* base;
            cXyz* dir;
            if (k < 10) {
                f = (f32)k / 9.0f;
                base = &i_this->mPathP0;
                dir = d0.get();
            } else {
                f = (f32)(19 - k) / 9.0f;
                base = &i_this->mPathP1;
                dir = d1.get();
            }
            cXyz_ml(dir, m.get(), f);
            cXyz_pl(base, r.get(), m.get());
            cXyz* pt = gabi::at<cXyz>(pos_p + 0xC * k);
            pt->copy(*r.get());
            s16 sa2 = cM_rad2s(f * 3.1415927f);
            pt->y = gabi::fmadds(cM_ssin(sa2), REG0F(7) + -20.0f, pt->y);
            gabi::store<u8>(size_p + k, 8);
        }
    }

    MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
    mDoMtx_YrotM(calc_mtx(), i_this->shape_angle.y);
    mDoMtx_YrotM(calc_mtx(), i_this->mTiltDir);
    mDoMtx_XrotM(calc_mtx(), i_this->mTilt);
    mDoMtx_YrotM(calc_mtx(), (s16)-i_this->mTiltDir);
    mDoMtx_XrotM(calc_mtx(), i_this->shape_angle.x);
    mDoMtx_ZrotM(calc_mtx(), i_this->shape_angle.z);
    {
        Mtx34* src = calc_mtx();
        J3DModel_setBaseTRMtx(i_this->mpModel, src);
    }
    PSMTXCopy(calc_mtx(), &i_this->mBgMtx);
    MtxTrans(0.0f, REG0F(7) - 400.0f, 0.0f, 1);
    {
        gabi::Local<cXyz> z;
        z->set(0.0f, 0.0f, 0.0f);
        MtxPosition(z.get(), &i_this->mCenter);
    }
    i_this->mStts.Move();
    i_this->mSph.SetC(&i_this->mCenter);
    dComIfG_Ccsp_Set(&i_this->mSph);
    for (int i = 0; i < 2; i++) {
        gabi::Local<cXyz> c;
        f32 h = REG_F(0, 11) + 35.0f;
        cXyz* p = &i_this->mHimoTop[i];
        c->x = p->x;
        c->z = p->z;
        c->y = p->y + h;
        i_this->mWindSph[i].SetC(c.get());
        dComIfG_Ccsp_Set(&i_this->mWindSph[i]);
        i_this->mCoCyl[i].SetC(c.get());
        dComIfG_Ccsp_Set(&i_this->mCoCyl[i]);
    }
    dBgW_Move(i_this->mpBgW);
    i_this->mAnchorRotY = (s16)(i_this->mAnchorRotY + (s16)gabi::ftoi(i_this->mSpeed * (REG0F(8) + 8000.0f)));
    for (int i = 0; i < 2; i++) {
        MtxTrans(i_this->mHimoTop[i].x, i_this->mHimoTop[i].y, i_this->mHimoTop[i].z, 0);
        mDoExt_McaMorf* morf = i_this->mpMorf[i];
        Mtx34* src = calc_mtx();
        J3DModel_setBaseTRMtx(morf->mpModel, src);
    }
    return TRUE;
}
VERIFY(0x0219FBFC, daKlft_Execute);

/* 021A0AEC */
static BOOL daKlft_IsDelete(klft_class*) {
    WWHD_FUNC(0x021A0AEC, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021A0AEC, daKlft_IsDelete);

/* 021A0AF4 */
static BOOL daKlft_Delete(klft_class* i_this) {
    WWHD_FUNC(0x021A0AF4, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhs, M_arcname_del);
    if (i_this->heap) {
        cBgS_Release(dComIfG_Bgsp(), i_this->mpBgW);
    }
    u8 sw = i_this->mSwitchNo;
    if (sw != 0) {
        if (i_this->mPos < 50.0f) {
            dComIfGs_offSwitch(sw, i_this->current.roomNo);
        } else {
            dComIfGs_onSwitch(sw, i_this->current.roomNo);
        }
        mDoAud_seDeleteObject(&i_this->mHimoTop[0]);
    } else {
        /* harness workaround: the matcher's GameCube signature (a JAIZelBasic member) makes the
         * harness compare r4, which 025E1B34 does not read; here r4 still holds sw (0) */
        gabi::call(0x025E1B34, &i_this->mHimoTop[0], (u32)sw);
    }
    mDoAud_seDeleteObject(&i_this->mHimoTop[1]);
    return TRUE;
}
VERIFY(0x021A0AF4, daKlft_Delete);

/* 021A0BB0 */
static BOOL CallbackCreateHeap(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x021A0BB0, BOOL, i_actor);
    klft_class* i_this = (klft_class*)i_actor;
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname_heap, 5, SAFESTRING_VTBL);
    J3DModel* model = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    i_this->mpModel = model;
    if (model == nullptr) {
        return FALSE;
    }
    gabi::store<u32>(gabi::ea(model) + 0xB8, gabi::ea(i_this)); /* setUserArea */
    for (u16 i = 0; i < J3DModelData_getJointNum_l(gabi::ea(modelData)); i++) {
        if (i == 1) {
            J3DModelData_setJointCallBack_l(gabi::ea(modelData), i, 0x0219F8A4 /* nodeCallBack_main */);
        }
    }
    i_this->mpBgW = new_dBgW();
    if (i_this->mpBgW == nullptr) /* JUT_ASSERT(0x344, actor->pm_bgw != NULL) */
        JUT_ASSERT_fail(STR(0x100130A4), 0x344, STR(0x100130B4));
    cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes(M_arcname_heap, 9, SAFESTRING_VTBL);
    cBgW_Set(i_this->mpBgW, dzb, cBgW_MOVE_BG_e, &i_this->mBgMtx);
    gabi::store<u32>(gabi::ea(i_this->mpBgW) + 0xA8, 0x024EE658); /* SetCrrFunc(dBgS_MoveBGProc_Trans) */
    gabi::store<u32>(gabi::ea(i_this->mpBgW) + 0xB0, 0x0219F608); /* SetRideCallback(ride_call_back) */
    void* tex = dComIfG_getObjectRes(M_arcname_heap, 0xC, SAFESTRING_VTBL);
    if (!mDoExt_3DlineMat1_init(&i_this->mLineMat, 1, 0x14, tex, 1)) {
        return FALSE;
    }
    for (int k = 0; k < 2; k++) {
        J3DModelData* data = (J3DModelData*)dComIfG_getObjectRes(M_arcname_heap, 6, SAFESTRING_VTBL);
        mDoExt_McaMorf* morf =
            mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, nullptr, 2, 1.0f, 0, -1, 0, nullptr, 0, 0x11020203);
        i_this->mpMorf[k] = morf;
        u32 m = gabi::ea(morf->mpModel.get());
        gabi::store<u32>(m + 0xB8, gabi::ea(i_this));
        for (u16 i = 0; i < J3DModelData_getJointNum_l(gabi::load<u32>(m + 0xAC)); i++) {
            if (i == 3) {
                J3DModelData_setJointCallBack_l(gabi::load<u32>(m + 0xAC), i, 0x0219F9C0 /* nodeCallBack */);
            }
        }
    }
    return TRUE;
}
VERIFY(0x021A0BB0, CallbackCreateHeap);

/* 021A0E60 */
static cPhs_State daKlft_Create(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x021A0E60, cPhs_State, i_actor);
    klft_class* i_this = (klft_class*)i_actor;
    dComIfGp_get();
    /* fopAcM_ct(i_this, klft_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = KLFT_VTBL;
            gabi::call(0x025EB82C, &i_this->mLineMat); /* mDoExt_3DlineMat1_c::mDoExt_3DlineMat1_c */
            dCcD_Stts_ct(&i_this->mStts);
            dCcD_Sph_ct_l(&i_this->mSph);
            __construct_array(i_this->mCoCyl, 2, 0x130, 0x021A1448);
            __construct_array(i_this->mWindSph, 2, 0x12C, 0x025166F0);
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&i_this->mPhs, M_arcname_crt);
    if (phase_state != cPhs_COMPLEATE_e) {
        return phase_state;
    }
    i_this->mType = (u8)fopAcM_GetParam(i_this);
    i_this->mSwitchNo = (u8)i_this->current.angle.z;
    u32 path_id = (fopAcM_GetParam(i_this) >> 16) & 0xFF;
    i_this->current.angle.z = 0;
    if (i_this->mSwitchNo == 0xFF) {
        i_this->mSwitchNo = 0;
    } else if (i_this->mSwitchNo != 0) {
        if (dComIfGs_isSwitch(i_this->mSwitchNo, i_this->current.roomNo)) {
            i_this->mPos = 80.0f;
        } else {
            i_this->mPos = 20.0f;
        }
    }
    if (i_this->mType == 0xFF) {
        i_this->mType = 0;
    }

    if (!fopAcM_entrySolidHeap(i_this, 0x021A0BB0 /* CallbackCreateHeap */, 0x10000)) {
        return cPhs_ERROR_e;
    }
    if (i_this->mpBgW != nullptr && dBgS_Regist(dComIfG_Bgsp(), i_this->mpBgW, i_this)) {
        return cPhs_ERROR_e;
    }
    if (path_id == 0xFF) {
        return cPhs_ERROR_e;
    }
    dPath* path = dPath_GetRoomPath(path_id, i_this->current.roomNo);
    if (path == nullptr) {
        return cPhs_ERROR_e;
    }
    u32 pnt = gabi::load<u32>(gabi::ea(path) + 8); /* m_points: dPnt {u8 ...; cXyz m_position at +4}, 0x10 each */
    i_this->mPathP0.set(gabi::load<f32>(pnt + 0x04), gabi::load<f32>(pnt + 0x08), gabi::load<f32>(pnt + 0x0C));
    i_this->mPathP1.set(gabi::load<f32>(pnt + 0x14), gabi::load<f32>(pnt + 0x18), gabi::load<f32>(pnt + 0x1C));
    {
        gabi::Local<cXyz> d;
        cXyz_mi(&i_this->mPathP1, d.get(), &i_this->mPathP0);
        i_this->home.angle.y = (s16)(cM_atan2s(d->x, d->z) + 0x8000);
        i_this->current.angle.y = (s16)(cM_atan2s(d->x, d->z) + 0x8000);
        i_this->mPathDir.copy(*d.get());
    }
    i_this->mHimoTop[0].set(gabi::load<f32>(pnt + 0x24), gabi::load<f32>(pnt + 0x28), gabi::load<f32>(pnt + 0x2C));
    i_this->mHimoTop[1].set(gabi::load<f32>(pnt + 0x34), gabi::load<f32>(pnt + 0x38), gabi::load<f32>(pnt + 0x3C));

    f32 s;
    switch (i_this->mType) {
    case 1: s = 0.9f; break;
    case 2: s = 0.8f; break;
    case 3: s = 0.7f; break;
    default: s = 1.0f; break;
    }
    i_this->scale.x = s;
    i_this->scale.y = 1.0f;
    i_this->scale.z = s;
    {
        u32 m = gabi::ea(i_this->mpModel.get());
        gabi::store<f32>(m + 0xC0, 1.0f);
        gabi::store<f32>(m + 0xC4, s);
        gabi::store<f32>(m + 0xBC, s);
    }
    i_this->mStts.Init(0xFF, 0xFF, i_this);
    i_this->mSph.Set(utiwa_sph_src);
    i_this->mSph.SetStts(&i_this->mStts);
    for (int i = 0; i < 2; i++) {
        i_this->mWindSph[i].Set(utiwa_sph_src);
        i_this->mWindSph[i].SetStts(&i_this->mStts);
        i_this->mWindSph[i].SetR(REG_F(0, 11) + 130.0f);
        i_this->mCoCyl[i].Set(p_co_cyl_src);
        i_this->mCoCyl[i].SetStts(&i_this->mStts);
    }
    daKlft_Execute(i_this);
    return phase_state;
}
VERIFY(0x021A0E60, daKlft_Create);

/* 021A13A0 */
static void __sinit_d_a_klft_cpp() {
    WWHD_FUNC(0x021A13A0, void, (u32)0);
    /* the header statics in a different order: floats at 0x10464BCC, objects at +0xA/+0xB */
    for (int i = 0; i < 4; i++) gabi::store<u32>(0x10464BD8 + 4 * i, 0);
    __register_global_object(0x101B84CC);
    gabi::store<f32>(0x10464BCC, -3.1415927f);
    gabi::store<f32>(0x10464BD0, 3.1415927f);
    gabi::call(0x028ED6F8, 0x10464BD6);
    __register_global_object(0x101B84D8);
    gabi::call(0x028EAB2C, 0x10464BD7);
    __register_global_object(0x101B84E4);
}
VERIFY(0x021A13A0, __sinit_d_a_klft_cpp);

/* 021A1434: sead::SafeString deleting destructor (this TU's vtable slot) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021A1434, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x021A1434, SafeString_dt);

/* 021A158C: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x021A158C, void, (u32)0);
}
VERIFY(0x021A158C, SafeString_assureTerminationImpl);

/* 021A1448: dCcD_Cyl::dCcD_Cyl (this TU's copy for __construct_array; allocates when NULL) */
static dCcD_Cyl* dCcD_Cyl_ct_tu(dCcD_Cyl* p) {
    WWHD_FUNC(0x021A1448, dCcD_Cyl*, p);
    if (p == nullptr) {
        p = (dCcD_Cyl*)operator_new(0x130);
        if (p == nullptr)
            return nullptr;
    }
    dCcD_Cyl_ct(p, AAB_VTBL);
    return p;
}
VERIFY(0x021A1448, dCcD_Cyl_ct_tu);

/* 021A14D4: klft_class deleting destructor (HD virtual) */
static void klft_class_dt(klft_class* i_this, s32 flags) {
    WWHD_FUNC(0x021A14D4, void, i_this, flags);
    if (i_this != nullptr) {
        __destroy_arr(i_this->mWindSph, 2, 0x12C, 0x02515AE8); /* dCcD_Sph::~dCcD_Sph */
        __destroy_arr(i_this->mCoCyl, 2, 0x130, 0x02515A70);   /* dCcD_Cyl::~dCcD_Cyl */
        gabi::call(0x02515AE8, &i_this->mSph, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025EB8B8, &i_this->mLineMat, 2); /* ~mDoExt_3DlineMat1_c (matcher: draw) */
        gabi::call(0x025D50BC, i_this, 0);            /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021A14D4, klft_class_dt);
