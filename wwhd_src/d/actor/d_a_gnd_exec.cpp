/**
 * d_a_gnd_exec.cpp (WWHD)
 * Boss - Ganondorf: daGnd_Execute (02154EE0), with the collision update, the sword spheres and
 * demo_camera inlined.
 *
 * The GameCube source of this unit is all "Nonmatching"
 * stubs: written from the WWHD code (cking.rpx) and verified against it.
 */
#include "d/actor/d_a_gnd.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void* gnd_dDemo_getActor(u32 obj, u8 id) { return gabi::call<void*>(0x02526E70, obj, id); }
static inline void* gnd_dDemo_getP_BtpData(void* ac, const char* arc) { return gabi::call<void*>(0x02527828, ac, arc); }
static inline void* gnd_dDemo_getP_BtkData(void* ac, const char* arc) { return gabi::call<void*>(0x025279C8, ac, arc); }
static inline void* gnd_dDemo_getP_BrkData(void* ac, const char* arc) { return gabi::call<void*>(0x02527A98, ac, arc); }
static inline BOOL gnd_dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7) {
    return gabi::call<BOOL>(0x02527028, a, flags, morf, arc, n, ids, p6, p7);
}
static inline s32 gnd_GetAttributeCode(dBgS* bgs, void* poly) { return gabi::call<s32>(0x024EF0F4, bgs, poly); }
static inline void gnd_JPASetRMtxTVecfromMtx(Mtx34* m, u32 r, u32 t) { gabi::call(0x028249B0, m, r, t); }
static inline BOOL gnd_orderPotentialEvent(fopAc_ac_c* a, u16 type, u16 flag, u16 p) { return gabi::call<BOOL>(0x025D7B24, a, type, flag, p); }
static inline void gnd_cam_Stop(u32 cam) { gabi::call(0x02514F2C, cam); }
static inline void gnd_cam_Start(u32 cam) { gabi::call(0x02514F38, cam); }
static inline void gnd_cam_SetTrimSize(u32 cam, s32 s) { gabi::call(0x02515280, cam, s); }
static inline void gnd_cam_Set(u32 cam, cXyz* center, cXyz* eye, s16 bank, f32 fovy) { gabi::call(0x02514FE8, cam, center, eye, bank, fovy); }
static inline void gnd_setNextStage(const char* stage, s16 point, s8 roomNo, s8 layer, f32 lastSpeed, u32 lastMode, s32 setPoint, s8 wipe) {
    gabi::call(0x0252012C, stage, point, roomNo, layer, lastSpeed, lastMode, setPoint, wipe);
}
/* 025E1904 mDoAud_bgmStop(frames), 025E1950 mDoAud_changeBgmStatus(status) */
static inline void gnd_bgmStop(u32 frames) { gabi::call(0x025E1904, frames); }
static inline void gnd_changeBgmStatus(s32 st) { gabi::call(0x025E1950, st); }
/* 025F05E8 (probably mDoGph_gInf_c::fadeOut(s8 rate)), 025F05F8 (probably setFadeColor(GXColor&)) */
static inline void gnd_fadeOut(s8 rate) { gabi::call(0x025F05E8, rate); }
static inline void gnd_setFadeColor(u32 color) { gabi::call(0x025F05F8, color); }
/* 0259169C: sets play+0x5C22 = 1 (identity unknown) */
static inline void gnd_play5C22_on() { gabi::call(0x0259169C); }
/* daPy_py_c virtuals (C++ vtable at +0xB4): +0x114 setPlayerPosAndAngle(cXyz*, s16), +0xE4 (probably voiceStart-like) */
static inline void daPy_setPlayerPosAndAngle(u32 vt, fopAc_ac_c* pl, cXyz* pos, s16 ang) {
    gabi::call_ptr(gabi::load<u32>(vt + 0x114), pl, pos, ang);
}

static inline bool gnd_isStop(mDoExt_McaMorf* morf) { return (morf->mFrameCtrl.mState & 1) || morf->mFrameCtrl.mRate == 0.0f; }
/* fopAcM_monsSeStart, inline with the actor and eyePos checks */
static inline void gnd_mons_se(gnd_class* a, u32 id) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        gabi::call(0x025E1AA4, id, &a->eyePos, pid, 0, reverb);
    }
}
static inline void gnd_se(gnd_class* a, u32 id) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        mDoAud_seStart(id, &a->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
    }
}
static inline void gnd_shock(s32 strength) {
    gabi::Local<cXyz> p;
    u32 vib = gabi::ea(dComIfGp_getVibration());
    s16 reg = REG0_S(2);
    p->x = 0.0f;
    p->y = 1.0f;
    p->z = 0.0f;
    gabi::call<BOOL>(0x025CB374, vib, reg + strength, -0x21, p.get());
}

#define ZELDA_PTR 0x10464008u /* the Zelda actor (fpcEx_Search(z_s_sub)) */
#define REGF(i) REG0_F(i)

/* the sword spheres' At: positions and radius per attack type m1718[i] (3, >= 10, others) */
static inline void gnd_weponSphUpdate(gnd_class* i_this, J3DModel* model, fopAc_ac_c* player, int i, cXyz* local, cXyz* tmp) {
    dCcD_Sph* sph = &i_this->mWeponSph[i];
    s32 jnt = gabi::load<s32>(0x101B57D0 + i * 4);
    PSMTXCopy(gnd_getAnmMtx(model, jnt), calc_mtx());
    local->y = 0.0f;
    local->z = 0.0f;
    local->x = 0.0f;
    cXyz* center = GXYZ(0x1928 + i * 0xC);
    MtxPosition(local, center);
    s8 k = GF(s8, 0x1718 + i);
    if (k == 0 || (gabi::load<u8>(gabi::ea(player) + 0x3AC) == 5 && GF(s8, 0x60C) < 2)) {
        GF(u8, 0x172C + i) = 0;
        sph->mSph.SetC(gabi::at<cXyz>(0x1046402C));
        dComIfG_Ccsp_Set(sph);
        return;
    }
    u32 s = gabi::ea(sph);
    gabi::store<u8>(s + 0x6E, 0xD);
    k = GF(s8, 0x1718 + i);
    cXyz* at = GXYZ(0x1700 + i * 0xC);
    if (k == 3) {
        MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
        mDoMtx_YrotM(calc_mtx(), i_this->shape_angle.y);
        s16 idx = gabi::load<s16>(L_HIO + 0x80);
        f32 x = REGF(14);
        f32 z = REGF(16) + 300.0f;
        GF(u32, 0x171C + i * 4) = gabi::load<u32>(0x101B57D8 + idx * 4);
        f32 y = REGF(15) + 100.0f;
        local->z = z;
        sph->SetR(GF(f32, 0x1724 + i * 4));
        local->x = x;
        local->y = y;
    } else if (k >= 10) {
        MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
        k = GF(s8, 0x1718 + i);
        local->x = 0.0f;
        f32 r = REGF(17);
        if (k == 10) {
            local->y = REGF(15) + 60.0f;
        } else {
            local->y = REGF(16) + 230.0f;
        }
        local->z = 0.0f;
        GF(f32, 0x1724 + i * 4) = r + 100.0f;
        u32 f = gabi::load<u32>(s + 0x94);
        if (gabi::load<u8>(L_HIO + 0x41) == 0) {
            gabi::store<u32>(s + 0x94, f | 1);
            gabi::store<u32>(s + 0x94, gabi::load<u32>(s + 0x94) & ~4u);
        } else {
            gabi::store<u32>(s + 0x94, f & ~1u);
            gabi::store<u32>(s + 0x94, gabi::load<u32>(s + 0x94) | 4);
        }
        sph->SetR(GF(f32, 0x1724 + i * 4));
    } else {
        s32 jnt2 = gabi::load<s32>(0x101B57C8 + i * 4);
        PSMTXCopy(gnd_getAnmMtx(model, jnt2), calc_mtx());
        s16 idx = gabi::load<s16>(L_HIO + 0x7E);
        f32 y = REGF(10);
        GF(u32, 0x171C + i * 4) = gabi::load<u32>(0x101B57D8 + idx * 4);
        f32 x = REGF(9) + 50.0f;
        f32 z = REGF(11);
        k = GF(s8, 0x1718 + i);
        local->x = x;
        local->y = y;
        local->z = z;
        if (k == 4) gabi::store<u8>(s + 0x6E, 0xF);
        sph->SetR(GF(f32, 0x1724 + i * 4));
    }
    gabi::store<u8>(s + 0x6F, (u8)GF(u32, 0x171C + i * 4));
    MtxPosition(local, at);
    sph->SetC(at);
    mDoMtx_YrotS(calc_mtx(), i_this->shape_angle.y);
    local->x = 0.0f;
    local->y = 0.0f;
    local->z = 1.0f;
    MtxPosition(local, tmp);
    gabi::at<cXyz>(s + 0x7C)->copy(*tmp);
    k = GF(s8, 0x1718 + i);
    u32 at_sprm;
    if (k >= 10) {
        gabi::store<u32>(s, gabi::load<u32>(s) & ~1u);
    } else {
        f32 d = player->current.pos.y - i_this->mAcch.m_ground_h;
        at_sprm = gabi::load<u32>(s);
        if (d < 50.0f) { /* bge: taken on NaN */
            gabi::store<u32>(s, at_sprm | 1);
        } else {
            gabi::store<u32>(s, at_sprm & ~1u);
        }
    }
    GF(s8, 0x1718 + i) = 0;
    GF(f32, 0x1724 + i * 4) = REGF(13) + 150.0f;
    dComIfG_Ccsp_Set(sph);
}

/* demo_camera's waypoint tables: centers cXyz[5] (0x10464044), eyes cXyz[5] (0x10464080), fovy f32[5] (0x101B554C) */
#define DC_CENTER(n) gabi::at<cXyz>(0x10464044 + (n) * 0xC)
#define DC_EYE(n) gabi::at<cXyz>(0x10464080 + (n) * 0xC)
#define DC_FOVY(n) gabi::load<f32>(0x101B554C + (n) * 4)

/* the demo camera (GameCube demo_camera, inlined): the cut by m18BE, the timer m18C0 */
static inline void gnd_demo_order(gnd_class* i_this, u16 type) {
    gnd_orderPotentialEvent(i_this, type, 0xFFFF, 0);
    u32 c = gabi::ea(i_this) + 0xFA;
    gabi::store<u16>(c, (u16)(gabi::load<u16>(c) | 2));
}
static inline bool gnd_cmdDemo(gnd_class* i_this) { return gabi::load<u16>(gabi::ea(i_this) + 0xF8) == 2; }
static inline void gnd_camStart(gnd_class* i_this, u32 camc, s16 m) {
    GF(s16, 0x18BE) = m + 1;
    gnd_cam_Stop(camc);
    gnd_cam_SetTrimSize(camc, 2);
}
static inline fopAc_ac_c* gnd_zelda() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(ZELDA_PTR)); }

static inline void demo_camera(gnd_class* i_this) {
    fopAc_ac_c* pl = dComIfGp_getPlayer(0);
    s8 camIdx = gabi::load<s8>(dComIfGp_ea() + 0x5B30);
    u32 play = dComIfGp_ea();
    fopAc_ac_c* zelda = gnd_zelda();
    fopAc_ac_c* zelda0 = zelda;
    s16 m = GF(s16, 0x18BE);
    u32 cam = gabi::load<u32>(play + camIdx * 0x34 + 0x5AF8);
    u32 camc = cam + 0x248;
    gabi::Local<cXyz> zero;
    zero->x = 0.0f;
    zero->z = 0.0f;
    zero->y = 0.0f;
    f32 fovAdd = 0.0f;
    gabi::Local<cXyz> l38;
    gabi::Local<cXyz> l50;
    gabi::Local<cXyz> lsum;
    s16 t;
    s8 n;

    switch ((u32)(s32)m) {
    case 1:
        if (!gnd_cmdDemo(i_this)) {
            gnd_demo_order(i_this, 2);
            return;
        }
        gnd_camStart(i_this, camc, m);
        GF(f32, 0x1904) = 55.0f;
        GF(f32, 0x1900) = 0.0f;
        GF(s16, 0x18C0) = 0;
        gabi::store<u32>(gabi::ea(pl) + 0x428, 0);
        gabi::store<u16>(gabi::ea(pl) + 0x420, 3);
        GF(f32, 0x190C) = 0.0f;
        GF(f32, 0x1904) = REGF(6) + 55.0f;
        GF(f32, 0x18DC) = REGF(0) + 400.0f;
        GF(f32, 0x18E0) = REGF(1) + 100.0f;
        GF(f32, 0x18E4) = REGF(2) - 200.0f;
        GF(f32, 0x18E8) = REGF(3);
        GF(f32, 0x18EC) = REGF(4) + 200.0f;
        GF(f32, 0x18F0) = REGF(5);
        zelda = gnd_zelda();
        /* fall through */
    case 2: {
        f32 dx = gabi::fsubs_ppc(i_this->current.pos.x, zelda->current.pos.x);
        f32 dz = gabi::fsubs_ppc(i_this->current.pos.z, zelda->current.pos.z);
        s16 ang = cM_atan2s(dx, dz);
        s16 v = (s16)(ang + 0x4000 + REG0_S(0));
        i_this->shape_angle.y = v;
        i_this->current.angle.y = v;
        if (GF(s16, 0x18C0) > REG0_S(1) + 0x19) {
            GF(s16, 0x18BC) = 0x82;
            cLib_addCalc2(&GF(f32, 0x18DC), REGF(6) + 130.0f, 0.6f, gabi::fmuls_ppc(270.0f, GF(f32, 0x1900)));
            cLib_addCalc2(&GF(f32, 0x18E0), REGF(7) + 230.0f, 0.6f, gabi::fmuls_ppc(80.0f, GF(f32, 0x1900)));
            cLib_addCalc2(&GF(f32, 0x18E4), REGF(8), 0.6f, gabi::fmuls_ppc(100.0f, GF(f32, 0x1900)));
            cLib_addCalc2(&GF(f32, 0x18EC), REGF(9) + 280.0f, 0.6f, gabi::fmuls_ppc(80.0f, GF(f32, 0x1900)));
            cLib_addCalc2(&GF(f32, 0x1900), 1.0f, 1.0f, gabi::fmuls_ppc(0.02f, REGF(7) + 1.0f));
        } else {
            GF(f32, 0x18DC) = REGF(0) + 400.0f;
            GF(f32, 0x18E0) = REGF(1) + 150.0f;
            GF(f32, 0x18E4) = REGF(2);
            GF(f32, 0x18E8) = REGF(3);
            GF(f32, 0x18EC) = REGF(4) + 200.0f;
            GF(f32, 0x18F0) = REGF(5);
        }
        MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
        mDoMtx_YrotM(calc_mtx(), i_this->shape_angle.y);
        MtxPosition(GXYZ(0x18DC), GXYZ(0x18C4));
        MtxPosition(GXYZ(0x18E8), GXYZ(0x18D0));
        if (GF(s16, 0x18C0) > REG0_S(2) + 0x3C) {
            s16 nm = GF(s16, 0x18BE) + 1;
            GF(s16, 0x18BE) = nm;
            GF(s16, 0x18C0) = 0;
            GF(s16, 0x18BC) = 0x32;
            m = nm;
            goto check;
        }
        goto check0;
    }
    case 3: {
        if (GF(s16, 0x18C0) == 10) {
            GF(s16, 0x3EC) = GF(s16, 0x3EC) + 1;
            zelda = gnd_zelda();
        }
        MtxTrans(zelda->current.pos.x, zelda->current.pos.y, zelda->current.pos.z, 0);
        mDoMtx_YrotM(calc_mtx(), gnd_zelda()->shape_angle.y);
        GF(f32, 0x18DC) = REGF(10) + 100.0f;
        GF(f32, 0x18E0) = REGF(11) + 50.0f;
        GF(f32, 0x18E4) = REGF(12) - 300.0f;
        MtxPosition(GXYZ(0x18DC), GXYZ(0x18C4));
        GF(f32, 0x18D0) = i_this->current.pos.x;
        f32 py = i_this->current.pos.y;
        f32 pz = i_this->current.pos.z;
        GF(f32, 0x18D4) = py;
        GF(f32, 0x18D8) = pz;
        GF(f32, 0x18D4) = gabi::fadds_ppc(py, REGF(13) + 150.0f);
        u32 vt = pl->__vtbl;
        s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
        daPy_setPlayerPosAndAngle(vt, pl, &pl->current.pos, (s16)(ang + 0x8000));
        m = GF(s16, 0x18BE);
        goto check;
    }
    case 4:
        cLib_addCalc2(&GF(f32, 0x18D4), 100.0f, 0.8f, 10.0f);
        if (GF(s16, 0x18C0) < 0xF) goto check0;
        GF(s16, 0x18BE) = 5;
        GF(s16, 0x18C0) = 0;
        GF(f32, 0x1904) = 75.0f;
        /* fall through */
    case 5: {
        mDoMtx_YrotS(calc_mtx(), pl->shape_angle.y);
        l38->x = REGF(4) + -50.0f;
        l38->y = REGF(5) + 50.0f;
        l38->z = REGF(6) + 100.0f;
        MtxPosition(l38, l50);
        cXyz_pl(&pl->current.pos, lsum, l50);
        GXYZ(0x18C4)->copy(*lsum);
        GF(f32, 0x18D0) = pl->current.pos.x;
        f32 py = pl->current.pos.y;
        GF(f32, 0x18D4) = py;
        GF(f32, 0x18D8) = pl->current.pos.z;
        GF(f32, 0x18D4) = gabi::fadds_ppc(py, REGF(7) + 90.0f);
        cLib_addCalc2(&GF(f32, 0x1904), 55.0f, 0.8f, 10.0f);
        GF(s16, 0x18BC) = 0x32;
        t = GF(s16, 0x18C0);
        if (gabi::load<u8>(L_HIO + 0x84) == 0) {
            if (t == 5) {
                u32 vt = pl->__vtbl;
                gabi::store<u32>(gabi::ea(pl) + 0x430, 0x31);
                gabi::call_ptr(gabi::load<u32>(vt + 0xE4), pl);
                t = GF(s16, 0x18C0);
            }
            if (t != 0x19) goto check0;
        } else {
            if (t == 5) {
                gabi::store<u32>(gabi::ea(pl) + 0x430, 0x32);
                t = GF(s16, 0x18C0);
            }
            if (t == 0x3C) {
                gabi::store<u32>(gabi::ea(pl) + 0x430, 0x1D);
                t = GF(s16, 0x18C0);
            }
            if (t != 0x96) goto check0;
        }
        {
            J3DModel* model = i_this->mpMorf->getModel();
            void* res = dComIfG_getObjectRes(STR(0x1000FFE8) /* "Gnd" */, 0x63, GND_SAFESTRING_VTBL);
            gnd_btkAnm_init(i_this->mpBtk, J3DModel_getModelData(model), res, 1, 0, 1.0f, 0, -1, true, 0);
        }
        {
            s16 a = i_this->shape_angle.y;
            GF(s16, 0x3EA) = 0;
            i_this->current.angle.y = a;
            GF(s16, 0x3EC) = 0;
            GF(s16, 0x420) = gabi::load<s16>(L_HIO + 0x64);
            GF(s8, 0x60C) = 1;
            GF(s16, 0x18BE) = 0x96;
            m = 0x96;
        }
        goto check;
    }
    case 10:
        if (GF(s16, 0x3EA) != 0) return;
        if (!checkGround(i_this, 0.0f)) return;
        if (!gnd_cmdDemo(i_this)) {
            gnd_demo_order(i_this, 2);
            return;
        }
        gnd_camStart(i_this, camc, m);
        GF(f32, 0x1904) = 55.0f;
        GF(s16, 0x18C0) = 0;
        GF(f32, 0x1900) = 0.0f;
        gabi::store<u16>(gabi::ea(pl) + 0x420, 3);
        gabi::store<u32>(gabi::ea(pl) + 0x428, 0);
        mDoMtx_YrotS(calc_mtx(), gnd_zelda()->shape_angle.y);
        l38->x = REGF(4) + 150.0f;
        l38->y = REGF(5) + 50.0f;
        l38->z = REGF(6) + 150.0f;
        MtxPosition(l38, l50);
        cXyz_pl(&gnd_zelda()->current.pos, lsum, l50);
        GXYZ(0x18C4)->copy(*lsum);
        GXYZ(0x18D0)->copy(gnd_zelda()->eyePos);
        GF(f32, 0x18D4) = gabi::fadds_ppc(GF(f32, 0x18D4), REGF(7) - 30.0f);
        gabi::store<u32>(gabi::ea(zelda0) + 0xADC, 2);
        GF(s16, 0x3EA) = 0x14;
        GF(f32, 0x190C) = 0.0f;
        zelda = gnd_zelda();
        /* fall through */
    case 11: {
        cLib_addCalc2(&GF(f32, 0x18D0), zelda->eyePos.x, 0.1f, 20.0f);
        f32 y = gabi::fsubs_ppc(gabi::fadds_ppc(gnd_zelda()->eyePos.y, REGF(7)), 30.0f);
        if (y < 40.0f) y = 40.0f;
        cLib_addCalc2(&GF(f32, 0x18D4), y, 0.1f, 20.0f);
        cLib_addCalc2(&GF(f32, 0x18D8), gnd_zelda()->eyePos.z, 0.1f, 20.0f);
        if (GF(s16, 0x18C0) >= REG0_S(0) + 100) {
            s16 a = i_this->shape_angle.y;
            GF(s16, 0x3EA) = 0;
            GF(s16, 0x3EC) = 0;
            i_this->current.angle.y = a;
            GF(s16, 0x420) = gabi::load<s16>(L_HIO + 0x64);
            GF(s16, 0x18BE) = 0x96;
            gabi::store<u8>(gabi::ea(zelda0) + 0x858, 1);
            GF(s8, 0x60C) = 2;
            m = GF(s16, 0x18BE);
            goto check;
        }
        goto check0;
    }
    case 20:
        if (!gnd_cmdDemo(i_this)) {
            gnd_demo_order(i_this, 1);
            return;
        }
        gnd_camStart(i_this, camc, m);
        GF(f32, 0x1904) = 55.0f;
        GF(f32, 0x1900) = 0.0f;
        GF(s16, 0x18C0) = 0;
        gabi::store<u32>(gabi::ea(pl) + 0x428, 0);
        gabi::store<u16>(gabi::ea(pl) + 0x420, 3);
        gnd_shock(8);
        GF(s16, 0x18BC) = 0x64;
        GF(f32, 0x190C) = REGF(18) + 5.0f;
        /* fall through */
    case 21: {
        GF(f32, 0x18D0) = pl->current.pos.x;
        f32 py = pl->current.pos.y;
        GF(f32, 0x18D4) = py;
        GF(f32, 0x18D8) = pl->current.pos.z;
        GF(f32, 0x18D4) = gabi::fadds_ppc(py, REG_F(8, 10) + 50.0f);
        mDoMtx_YrotS(calc_mtx(), pl->shape_angle.y);
        l38->x = REG_F(8, 1) + 200.0f;
        l38->y = REG_F(8, 2) + 30.0f;
        l38->z = REG_F(8, 3) + 300.0f;
        MtxPosition(l38, l50);
        cXyz_pl(l50, lsum, &pl->current.pos);
        GXYZ(0x18C4)->copy(*lsum);
        if (GF(s16, 0x18C0) > (s16)(REG_S(12, 0) + 0xF)) {
            GF(s16, 0x18BE) = GF(s16, 0x18BE) + 1;
            GF(s16, 0x18C0) = 0;
        } else {
            fovAdd = REG_F(8, 13);
            m = GF(s16, 0x18BE);
            goto check;
        }
    }
        /* fall through */
    case 22:
        cLib_addCalc2(&GF(f32, 0x18D0), i_this->current.pos.x, 0.5f, REG_F(8, 6) + 70.0f);
        cLib_addCalc2(&GF(f32, 0x18D4), GF(f32, 0x1944) + REG_F(8, 4), 0.1f, 50.0f);
        cLib_addCalc2(&GF(f32, 0x18D8), i_this->current.pos.z + REG_F(8, 5), 0.5f, REG_F(8, 6) + 70.0f);
        mDoMtx_YrotS(calc_mtx(), i_this->shape_angle.y);
        l38->x = REG_F(8, 7) + -300.0f;
        l38->y = REG_F(8, 8) + 100.0f;
        l38->z = REG_F(8, 9) + 500.0f;
        MtxPosition(l38, l50);
        cXyz_pl(l50, lsum, &i_this->current.pos);
        GXYZ(0x18C4)->copy(*lsum);
        if (GF(s16, 0x18C0) > 0x1E) {
            s16 e = GF(s16, 0x3EA);
            GF(s16, 0x18C0) = 0;
            if (e != 0xB) {
                s16 a = i_this->shape_angle.y;
                GF(s16, 0x3EA) = 0;
                GF(s16, 0x3EC) = 0;
                i_this->current.angle.y = a;
                GF(s16, 0x420) = gabi::load<s16>(L_HIO + 0x64);
            }
            GF(s16, 0x18BE) = 0x96;
            goto cam;
        }
        goto check0;
    case 0x64:
        if (!gnd_cmdDemo(i_this)) {
            gnd_demo_order(i_this, 2);
            return;
        }
        gnd_camStart(i_this, camc, m);
        /* fall through */
    case 0x65: {
        GF(f32, 0x1904) = 55.0f;
        GF(s16, 0x18C0) = 0;
        GF(f32, 0x1900) = 0.0f;
        gabi::store<u16>(gabi::ea(pl) + 0x420, 3);
        gabi::store<u32>(gabi::ea(pl) + 0x428, 0);
        GF(u32, 0x114C) = GF(u32, 0x114C) & ~1u; /* mCyl: Co off */
        anm_init(i_this, 0x35, 1.0f, 0, 1.0f, -1);
        gabi::store<u32>(gabi::ea(pl) + 0x430, 0x46);
        gabi::store<u8>(gabi::ea(zelda0) + 0x857, 1);
        GF(s16, 0x18BE) = GF(s16, 0x18BE) + 1;
        daPy_setPlayerPosAndAngle(pl->__vtbl, pl, zero, 0);
        i_this->current.pos.copy(*zero);
        i_this->shape_angle.y = 0;
        i_this->current.angle.y = 0;
        gnd_zelda()->current.pos.copy(*zero);
        fopAc_ac_c* z = gnd_zelda();
        z->current.pos.x = gabi::fadds_ppc(z->current.pos.x, 400.0f);
        GF(u32, 0x1750) = gabi::ea(dComIfGp_particle_set(0x8384, &i_this->current.pos));
        GF(u32, 0x1754) = gabi::ea(dComIfGp_particle_set(0x8385, &i_this->current.pos));
        GF(s8, 0x18C2) = 0;
    }
        /* fall through */
    case 0x66: {
        for (int i = 0; i < 2; i++) {
            u32 e = GF(u32, 0x1750 + i * 4);
            if (e == 0) continue;
            mDoExt_McaMorf* morf = i_this->mpMorf;
            if (gnd_isStop(morf)) {
                gnd_becomeInvalidEmitter(e);
                GF(u32, 0x1750 + i * 4) = 0;
            } else {
                gnd_JPASetRMtxTVecfromMtx(gnd_getAnmMtx(morf->getModel(), 0x30), e + 0x1F0, e + 0x22C);
            }
        }
        t = GF(s16, 0x18C0);
        if (t == 0x22) {
            if (i_this == nullptr || gabi::ea(&i_this->eyePos) == 0) goto f68;
            gnd_mons_se(i_this, 0x4957);
            t = GF(s16, 0x18C0);
        }
        if (t == 0x4A) {
            if (i_this == nullptr || gabi::ea(&i_this->eyePos) == 0) goto f68;
            gnd_mons_se(i_this, 0x4958);
            t = GF(s16, 0x18C0);
        }
        if (t == 0x1E) {
            gnd_se(i_this, 0x2806);
            gnd_shock(4);
            t = GF(s16, 0x18C0);
        }
        if (t == 0x13) goto inc;
    f68:
        if ((s32)t == REG0_S(4) + 0x27 || (s32)t == REG0_S(5) + 0x43 || (s32)t == REG0_S(6) + 0x45) goto inc;
        n = GF(s8, 0x18C2);
        goto pick;
    inc:
        n = (s8)(GF(s8, 0x18C2) + 1);
        GF(s8, 0x18C2) = n;
        if (n == 2 || n == 4) {
            GF(f32, 0x18E8) = fabsf(gabi::fsubs_ppc(DC_CENTER(n)->x, DC_CENTER(n - 1)->x));
            n = GF(s8, 0x18C2);
            GF(f32, 0x18EC) = fabsf(gabi::fsubs_ppc(DC_CENTER(n)->y, DC_CENTER(n - 1)->y));
            GF(f32, 0x18F0) = fabsf(gabi::fsubs_ppc(DC_CENTER(n)->z, DC_CENTER(n - 1)->z));
            GF(f32, 0x18DC) = fabsf(gabi::fsubs_ppc(DC_EYE(n)->x, DC_EYE(n - 1)->x));
            GF(f32, 0x18E0) = fabsf(gabi::fsubs_ppc(DC_EYE(n)->y, DC_EYE(n - 1)->y));
            GF(f32, 0x18E4) = fabsf(gabi::fsubs_ppc(DC_EYE(n)->z, DC_EYE(n - 1)->z));
            GF(f32, 0x1908) = fabsf(gabi::fsubs_ppc(DC_FOVY(n), DC_FOVY(n - 1)));
        }
    pick:
        if (n == 0 || n == 1 || n == 3) {
            GXYZ(0x18D0)->copy(*DC_CENTER(n));
            n = GF(s8, 0x18C2);
            GXYZ(0x18C4)->copy(*DC_EYE(n));
            GF(f32, 0x1904) = DC_FOVY(n);
        } else {
            n = GF(s8, 0x18C2);
            f32 r = REGF(0) + 0.1f;
            GF(f32, 0x1900) = r;
            f32 step = gabi::fmuls_ppc(GF(f32, 0x18E8), r);
            f32 k = REG0_F(1) + 0.4f;
            cLib_addCalc2(&GF(f32, 0x18D0), DC_CENTER(n)->x, k, step);
            n = GF(s8, 0x18C2);
            cLib_addCalc2(&GF(f32, 0x18D4), DC_CENTER(n)->y, k, gabi::fmuls_ppc(GF(f32, 0x18EC), GF(f32, 0x1900)));
            n = GF(s8, 0x18C2);
            cLib_addCalc2(&GF(f32, 0x18D8), DC_CENTER(n)->z, k, gabi::fmuls_ppc(GF(f32, 0x18F0), GF(f32, 0x1900)));
            n = GF(s8, 0x18C2);
            cLib_addCalc2(&GF(f32, 0x18C4), DC_EYE(n)->x, k, gabi::fmuls_ppc(GF(f32, 0x18DC), GF(f32, 0x1900)));
            n = GF(s8, 0x18C2);
            cLib_addCalc2(&GF(f32, 0x18C8), DC_EYE(n)->y, k, gabi::fmuls_ppc(GF(f32, 0x18E0), GF(f32, 0x1900)));
            n = GF(s8, 0x18C2);
            cLib_addCalc2(&GF(f32, 0x18CC), DC_EYE(n)->z, k, gabi::fmuls_ppc(GF(f32, 0x18E4), GF(f32, 0x1900)));
            n = GF(s8, 0x18C2);
            cLib_addCalc2(&GF(f32, 0x1904), DC_FOVY(n), k, gabi::fmuls_ppc(GF(f32, 0x1908), GF(f32, 0x1900)));
        }
        t = GF(s16, 0x18C0);
        if (t == 0x4A) {
            gnd_se(i_this, 0x595D);
            gnd_bgmStop(10);
            gnd_shock(8);
            t = GF(s16, 0x18C0);
            if (t == 0x4A) {
                gnd_fadeOut((s8)(REG0_S(8) + 0x32));
                gnd_setFadeColor(0x101D5E9C);
                t = GF(s16, 0x18C0);
            }
        }
        if (t == 0x7C) {
            gnd_setNextStage(STR(0x1000FFF4) /* "GTower" */, 2, 0, 9, 0.0f, 0, 1, 7);
            m = GF(s16, 0x18BE);
            goto check;
        }
        goto check0;
    }
    case 0x96:
        gnd_cam_SetTrimSize(camc, 0);
        gnd_cam_Start(camc);
        gnd_play5C22_on();
        dComIfGp_event_reset();
        GF(s16, 0x18BE) = 0;
        gabi::store<u16>(gabi::ea(pl) + 0x420, 2);
        gabi::store<u32>(gabi::ea(pl) + 0x430, 1);
        GF(s16, 0x18BC) = 1;
        goto check0;
    default:
        goto check;
    }
check0:
    m = GF(s16, 0x18BE);
check:
    if (m == 0) return;
cam : {
    s16 tt = GF(s16, 0x18C0);
    f32 amp = GF(f32, 0x190C);
    f32 s1 = gabi::fmuls_ppc(cM_ssin(tt * 0x3500), amp);
    f32 c2 = gabi::fmuls_ppc(cM_scos(tt * 0x3900), amp);
    f32 c3 = gabi::fmuls_ppc(cM_scos((s16)GF(s16, 0x3E8) * 0x1C00), amp);
    gabi::Local<cXyz> center;
    gabi::Local<cXyz> eye;
    center->x = gabi::fadds_ppc(GF(f32, 0x18D0), s1);
    center->y = gabi::fadds_ppc(GF(f32, 0x18D4), c2);
    center->z = GF(f32, 0x18D8);
    eye->x = gabi::fadds_ppc(GF(f32, 0x18C4), s1);
    eye->y = gabi::fadds_ppc(GF(f32, 0x18C8), c2);
    eye->z = GF(f32, 0x18CC);
    s16 roll = (s16)gabi::ftoi(gabi::fmadds(c3, 7.5f, fovAdd));
    gnd_cam_Set(camc, center, eye, roll, GF(f32, 0x1904));
    cLib_addCalc0(&GF(f32, 0x190C), 1.0f, REGF(16) + 1.0f);
    gabi::call(0x027EC9E8, 0x19A, 0x1AE, 0x100100A8 /* "K SUB  C" */, (s32)GF(s16, 0x18C0));
    GF(s16, 0x18C0) = GF(s16, 0x18C0) + 1;
}
}

/* 02154EE0 */
BOOL daGnd_Execute(gnd_class* i_this) {
    WWHD_FUNC(0x02154EE0, BOOL, i_this);
    fopAc_ac_c* player0 = dComIfGp_getPlayer(0);
    if (i_this->m03D4 == 0) GF(s16, 0x3EA) = 0x14;

    if (i_this->demoActorID != 0) {
        /* demo */
        GF(u8, 0x1910) = 1;
        bool skip = false;
        if (gabi::load<s32>(0x101D600C) == 0x6A5) {
            gabi::Local<SafeString> a;
            gabi::Local<SafeString> b;
            a->__vtbl = GND_SAFESTRING_VTBL;
            b->mStringTop = 0x1047E6B8;
            a->mStringTop = 0x1000FFEC; /* "Demo42" */
            b->__vtbl = GND_SAFESTRING_VTBL;
            gabi::call(0x0215D268, a.get()); /* assureTermination (devirtualised) */
            gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
            u32 s1 = a->mStringTop;
            gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b.get());
            if (s1 == b->mStringTop) {
                skip = true;
            } else {
                u32 p = a->mStringTop, q = b->mStringTop;
                for (u32 k = 0; k < 0x40001; k++) {
                    u8 c = gabi::load<u8>(p + k);
                    if (c != gabi::load<u8>(q + k)) break;
                    if (c == 0) {
                        skip = true;
                        break;
                    }
                }
            }
        }
        if (!skip) {
            u8 id = i_this->demoActorID;
            void* act = nullptr;
            if (id != 0 && id <= 0x20) {
                u32 obj = gabi::load<u32>(0x101D5FFC);
                if (obj == 0) {
                    JUT_ASSERT_fail(STR(0x1001009C) /* "d_demo.h" */, 0x23A, STR(0x10010074) /* "m_object != 0" */);
                    obj = gabi::load<u32>(0x101D5FFC);
                }
                act = gnd_dDemo_getActor(obj, id);
            }
            if (act != nullptr) {
                void* btp = gnd_dDemo_getP_BtpData(act, STR(0x1000FFE0));
                if (btp != nullptr) {
                    J3DModel* m = i_this->mpMorf->getModel();
                    gnd_btpAnm_init(i_this->mpBtp, J3DModel_getModelData(m), btp, 1, 0, 1.0f, 0, -1, true, 0);
                }
                void* btk = gnd_dDemo_getP_BtkData(act, STR(0x1000FFE0));
                if (btk != nullptr) {
                    J3DModel* m = i_this->mpMorf->getModel();
                    gnd_btkAnm_init(i_this->mpBtk, J3DModel_getModelData(m), btk, 1, 0, 1.0f, 0, -1, true, 0);
                }
                void* brk = gnd_dDemo_getP_BrkData(act, STR(0x1000FFE0));
                if (brk != nullptr) {
                    J3DModel* m = i_this->mpMorf->getModel();
                    gnd_brkAnm_init(i_this->mpBrk, J3DModel_getModelData(m), brk, 1, 2, 1.0f, 0, -1, true, 0);
                }
            }
            gnd_dDemo_setDemoData(i_this, 0x6A, i_this->mpMorf, STR(0x1000FFE0), 0, nullptr, 0, 0);
        }
        if (GF(u8, 0x1910) != 0) {
            J3DModel* model = i_this->mpMorf->getModel();
            f32 sz = i_this->scale.z;
            f32 sx = i_this->scale.x;
            f32 sy = i_this->scale.y;
            gabi::store<f32>(gabi::ea(model) + 0xBC, sx);
            gabi::store<f32>(gabi::ea(model) + 0xC0, sy);
            gabi::store<f32>(gabi::ea(model) + 0xC4, sz);
            mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
            mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x, i_this->shape_angle.y, i_this->shape_angle.z);
            J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
            i_this->mpMorf->calc();
            mDoExt_baseAnm_play(i_this->mpBrk);
            mDoExt_baseAnm_play(i_this->mpBtk);
            mDoExt_baseAnm_play(i_this->mpBtp);
            ke_move(i_this);
            return TRUE;
        }
    } else if (GF(u8, 0x1910) != 0) {
        GF(u8, 0x1910) = 0;
    }

    /* the water surface (attribute 0x13): ripples */
    if (checkGround(i_this, 0.0f)) {
        if ((i_this->mAcch.m_flags & 0x20) && gnd_GetAttributeCode(dComIfG_Bgsp(), GP(0x1008)) == 0x13 &&
            GF(u32, 0x1918) == 0) {
            /* function-local static cXyz scale(1, 1, 1) at 0x10464038, guard 0x10464014 */
            if (gabi::load<u32>(0x10464014) == 0) {
                gabi::store<u32>(0x10464014, 1);
                gabi::store<f32>(0x10464038, 1.0f);
                gabi::store<f32>(0x1046403C, 1.0f);
                gabi::store<f32>(0x10464040, 1.0f);
            }
            dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &i_this->current.pos, nullptr, gabi::at<cXyz>(0x10464038), 0xFF,
                            (dPa_levelEcallBack*)i_this->mRipple, -1, nullptr, nullptr, nullptr);
            if (GF(u32, 0x1918) != 0) {
                GF(f32, 0x1924) = 0.0f;
            }
        }
    } else {
        dPa_rippleEcallBack_end((dPa_rippleEcallBack*)i_this->mRipple);
    }
    if (gabi::load<u32>(ZELDA_PTR) == 0) {
        gabi::store<u32>(ZELDA_PTR, gabi::ea(fpcM_Search(0x0215449C /* z_s_sub */, i_this)));
    }
    {
        u8 hioC = gabi::load<u8>(L_HIO + 0xC);
        u8 st = gabi::load<u8>(gabi::ea(i_this) + 0x3A8);
        if (hioC != 0) GF(s8, 0x60C) = hioC - 1;
        if (st == 0x23) {
            gabi::store<u8>(gabi::ea(i_this) + 0x3A8, 0x24);
            gnd_changeBgmStatus(2);
        }
        u8 b3 = gabi::load<u8>(gabi::ea(i_this) + 0xB3);
        s16 cnt = GF(s16, 0x3E8);
        if (b3 == 0x23) {
            i_this->mParameters = 0;
            GF(s16, 0x3EA) = 0x15;
            GF(s16, 0x18BE) = 0x14;
        }
        GF(s16, 0x3E8) = cnt + 1;
        for (int k = 0; k < 5; k++) {
            s16 v = GF(s16, 0x41E + k * 2);
            if (v != 0) GF(s16, 0x41E + k * 2) = v - 1;
        }
        static const u16 timers[4] = {0x428, 0x42A, 0x434, 0x178C};
        for (int k = 0; k < 4; k++) {
            s16 v = GF(s16, timers[k]);
            if (v != 0) GF(s16, timers[k]) = v - 1;
        }
    }
    if (gabi::load<u8>(L_HIO + 1) == 0) {
        gabi::call(0x02159FC8, i_this); /* attack0 (the action dispatch) */
        i_this->mAcch.CrrPos(dComIfG_Bgsp());
        settingTevStruct(dKy_getEnvlight(), 2, &i_this->current.pos, gabi::at<dKy_tevstr_c>(gabi::ea(i_this) + 0x43C));
        i_this->mpMorf->play(&i_this->eyePos, 0, 0);
        if (gnd_isStop(i_this->mpMorf)) attack_eff_remove(i_this);
        mDoExt_baseAnm_play(i_this->mpBrk);
        mDoExt_baseAnm_play(i_this->mpBtk);
        mDoExt_baseAnm_play(i_this->mpBtp);
    }

    /* the model matrix */
    J3DModel* model = i_this->mpMorf->getModel();
    {
        f32 s = gabi::load<f32>(L_HIO + 8);
        i_this->scale.z = s;
        i_this->scale.y = s;
        i_this->scale.x = s;
        gabi::store<f32>(gabi::ea(model) + 0xBC, s);
        gabi::store<f32>(gabi::ea(model) + 0xC0, s);
        gabi::store<f32>(gabi::ea(model) + 0xC4, s);
    }
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    cLib_addCalc0(&GF(f32, 0x614), REG0_F(17) + 0.5f, REG0_F(18) + 6053.5f);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)(i_this->shape_angle.y + gabi::ftoi(GF(f32, 0x614))));
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->shape_angle.z);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    i_this->mpMorf->calc();

    /* eye, attention and the body collision */
    gabi::Local<cXyz> zero;
    zero->y = 0.0f;
    zero->z = 0.0f;
    zero->x = 0.0f;
    PSMTXCopy(gnd_getAnmMtx(model, 0x30), calc_mtx());
    MtxPosition(zero, GXYZ(0x1940));
    {
        i_this->eyePos.x = GF(f32, 0x1940);
        f32 ey = GF(f32, 0x1944);
        i_this->eyePos.y = ey;
        i_this->eyePos.z = GF(f32, 0x1948);
        f32 px = i_this->current.pos.x;
        f32 reg = REG_F(10, 5);
        gabi::store<f32>(gabi::ea(i_this) + 0x390, px);
        f32 off = gabi::fadds_ppc(reg, 80.0f);
        f32 py = i_this->current.pos.y;
        f32 pz = i_this->current.pos.z;
        f32 eyY = gabi::fsubs_ppc(ey, off);
        gabi::store<f32>(gabi::ea(i_this) + 0x398, pz);
        f32 ay = gabi::fadds_ppc(py, 290.0f);
        i_this->eyePos.y = eyY;
        gabi::store<f32>(gabi::ea(i_this) + 0x394, ay);
    }
    i_this->mCyl.SetC(&i_this->current.pos);
    i_this->mCyl.SetR(80.0f);
    i_this->mCyl.SetH(220.0f);
    dComIfG_Ccsp_Set(&i_this->mCyl);
    i_this->mHeadSph.SetC(GXYZ(0x1940));
    i_this->mHeadSph.SetR(gabi::fmuls_ppc(gabi::fadds_ppc(REG0_F(11), 70.0f), gabi::load<f32>(L_HIO + 8)));
    dComIfG_Ccsp_Set(&i_this->mHeadSph);
    gabi::Local<cXyz> tmp;
    PSMTXCopy(gnd_getAnmMtx(model, 9), calc_mtx());
    MtxPosition(zero, tmp);
    tmp->y = gabi::fsubs_ppc(tmp->y, gabi::fadds_ppc(REG0_F(10), 20.0f));
    i_this->mChestSph.SetC(tmp);
    i_this->mChestSph.SetR(gabi::fmuls_ppc(gabi::fadds_ppc(REG0_F(11), 170.0f), gabi::load<f32>(L_HIO + 8)));
    dComIfG_Ccsp_Set(&i_this->mChestSph);

    /* the Tg of the cylinder and the head: off while m60B counts down */
    {
        s8 t = GF(s8, 0x60B);
        if (t != 0) {
            GF(u32, 0x1268) = GF(u32, 0x1268) & ~1u;
            GF(s8, 0x60B) = t - 1;
            GF(u32, 0x1138) = GF(u32, 0x1138) & ~1u;
        } else {
            GF(u32, 0x1138) = GF(u32, 0x1138) | 1;
            GF(u32, 0x1268) = GF(u32, 0x1268) | 1;
        }
    }
    for (int i = 0; i < 2; i++) {
        gnd_weponSphUpdate(i_this, model, player0, i, zero, tmp);
    }

    /* the sword's attack type for the player's guard (fopEn_enemy_c) */
    {
        s8 a = GF(s8, 0x1730);
        if (a != 0) {
            GF(s8, 0x1730) = a - 1;
            GF(u8, 0x172F) = 1;
        }
        s8 b = GF(s8, 0x1731);
        if (b != 0) {
            GF(s8, 0x1731) = b - 1;
            GF(u8, 0x172F) = 2;
        }
        i_this->mBtNowFrame = 15.0f;
        u8 type = GF(u8, 0x172F);
        i_this->mBtStartFrame = 10.0f;
        i_this->mBtEndFrame = 20.0f;
        i_this->mBtMaxDis = 10000.0f;
        i_this->mBtAttackType = type;
        i_this->mBtMaxDis = REG0_F(4) + 600.0f;
        GF(u8, 0x172F) = 0;
        if (gabi::load<u8>(L_HIO + 0x85) != 0) {
            gabi::store<u8>(L_HIO + 0x85, 0);
            GF(s16, 0x18BE) = 10;
        }
    }

    demo_camera(i_this);
    ke_move(i_this);

    /* the attack effects follow the joints (u32[6] at 0x101B54DC) */
    model = i_this->mpMorf->getModel();
    for (int j = 0; j < 6; j++) {
        u32 e = GF(u32, 0x1734 + j * 4);
        if (e != 0) {
            s32 jnt = gabi::load<s32>(0x101B54DC + j * 4);
            gnd_JPASetRMtxTVecfromMtx(gnd_getAnmMtx(model, jnt), e + 0x1F0, e + 0x22C);
        }
    }
    body_flash(i_this);
    return TRUE;
}
VERIFY(0x02154EE0, daGnd_Execute);
