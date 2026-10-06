/**
 * d_a_kamome.cpp (WWHD): kamome_auto_move. 
 * Separate translation unit while d_a_kamome.cpp is edited in parallel; to be merged.
 */
#include "d/actor/d_a_kamome.h"

/* ---- functions of d_a_kamome.cpp (verified there) ---- */
static void anm_init(kamome_class* i_this, int a, float m, unsigned char l, float s, int snd) { gabi::call(0x02185558, i_this, a, m, l, s, snd); }
static fopAc_ac_c* search_esa(kamome_class* i_this) { return gabi::call<fopAc_ac_c*>(0x02185808, i_this); }
static void kamome_pos_move(kamome_class* i_this) { gabi::call(0x02185B78, i_this); }
static void kamome_bgcheck(kamome_class* i_this) { gabi::call(0x02185DF0, i_this); }
static void kamome_ground_pos_move(kamome_class* i_this) { gabi::call(0x02185E80, i_this); }

/* ---- bindings used here ---- */

/* mDoExt_McaMorf frame control (HD: J3DFrameCtrl at +0x98: rate, +0x9C frame, +0xA7 state) */
static s32 getFrame(mDoExt_McaMorf_c* m) { return gabi::ftoi(gabi::load<f32>(gabi::ea(m) + 0x9C)); }
static bool isStop(mDoExt_McaMorf_c* m) {
    return (gabi::load<u8>(gabi::ea(m) + 0xA7) & 1) || gabi::load<f32>(gabi::ea(m) + 0x98) == 0.0f;
}
static f32 GetGroundH(kamome_class* i_this) { return gabi::load<f32>(gabi::ea(&i_this->mAcch) + 0x94); }
static bool ChkWallHit(kamome_class* i_this) { return (i_this->mAcch.m_flags & 0x10) != 0; }

static f32 REG6_F(int i) { return REG_F(6, i); }
static s16 REG6_S(int i) { return REG_S(6, i); }

/* l_kamomeHIO (d_a_kamome.cpp) */
struct kamomeHIO_l {
    be<s8> mNo;
    be<u8> m05;
    be<s16> m06;
    be<s16> m08;
    u8 _06[2];
    be<f32> m0C;
};
static kamomeHIO_l& l_kamomeHIO() { return *gabi::at<kamomeHIO_l>(0x104648C8); }

/* esa_class fields used */
static void esa_set_field_0x298(fopAc_ac_c* esa, u8 v) { gabi::store<u8>(gabi::ea(esa) + 0x3B4, v); }

enum {
    BAS_FLY1 = 0x5, BAS_LAND1 = 0x6, BAS_MOVE1 = 0x7, BAS_WAIT1 = 0x8, BAS_WAIT2 = 0x9,
    BCK_EAT1 = 0xC, BCK_FLY1 = 0xD, BCK_LAND1 = 0xE, BCK_MOVE1 = 0xF, BCK_SING1 = 0x10, BCK_SING2 = 0x11,
    BCK_WAIT1 = 0x12, BCK_WAIT2 = 0x13, BCK_WAIT3 = 0x14,
};
enum { EMode_NONE = 0, EMode_LOOP = 2 };
enum { JA_SE_CV_KAMOME = 0x4805 };

/* HD: dBgS_GndChk on the stack (0x54 bytes; sub-object vtables after the members) */
struct dBgS_GndChk_l {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x40 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x4C */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ cXyz m_pos;
    /* 0x30 */ u8 _30[0x40 - 0x30];
    /* 0x40 */ be<u32> __vtbl_40;     /* dBgS_PolyPassChk */
    /* 0x44 */ be<u8> mPass[7];
    /* 0x4B */ u8 _4B;
    /* 0x4C */ be<u32> __vtbl_4C;     /* dBgS_GrpPassChk */
    /* 0x50 */ be<u32> mGrp;
};
WWHD_SIZE(dBgS_GndChk_l, 0x54);

static void dBgS_GndChk_ct(dBgS_GndChk_l* c) {
    gabi::call(0x02008E0C, c); /* cBgS_GndChk::cBgS_GndChk */
    for (int i = 0; i < 7; i++) c->mPass[i] = 0;
    c->mGrp = 1;
    c->mpGrpPassChk = gabi::ea(c) + 0x4C;
    c->__vtbl_10 = 0x1001233C;
    c->__vtbl_4C = 0x1001235C;
    c->__vtbl_40 = 0x1001236C;
    c->__vtbl_20 = 0x1001234C;
    c->mpPolyPassChk = gabi::ea(c) + 0x40;
}
static void dBgS_GndChk_dt(dBgS_GndChk_l* c) {
    c->__vtbl_20 = 0x1001234C;
    c->__vtbl_40 = 0x1001236C;
    c->__vtbl_4C = 0x1001232C;
    gabi::call(0x02008DAC, c, 0); /* destructor */
}

/* fly towards a point 2000 ahead and 1000 up */
static void set_fly_away_target(kamome_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    MtxTrans(a_this->current.pos.x, a_this->current.pos.y, a_this->current.pos.z, 0);
    gabi::Local<cXyz> sp30;
    sp30->set(0.0f, 1000.0f, 2000.0f);
    cMtx_YrotM(calc_mtx(), a_this->current.angle.y);
    MtxPosition(sp30, &i_this->mTargetPos);
}

/* 02189270 */
static void kamome_auto_move(kamome_class* i_this) {
    WWHD_FUNC(0x02189270, void, i_this);
    fopAc_ac_c* a_this = &i_this->actor;
    dComIfGp_get(); /* fopAc_ac_c* player = dComIfGp_getPlayer(0): unused */
    s8 moveType = 0;
    s32 frame;
    f32 x, y, z;

    switch (i_this->mAnimState) {
    case 0:
        frame = getFrame(i_this->mpMorf);
        if ((i_this->mTimers[0] == 0) && (frame == REG0_S(0) + 9)) {
            i_this->mAnimState = 1;
            anm_init(i_this, BCK_WAIT1, REG0_F(0) + 12.0f, EMode_LOOP, 1.0f, BAS_WAIT1);
        }
        break;

    case 1:
        if ((i_this->mGlobalTimer & 0x3f) == 0 && cM_rndF(1.0f) < 0.5f) {
            s16 t = (s16)gabi::ftoi(cM_rndF(10000.0f));
            i_this->mAnimState = 2;
            i_this->mGlobalTimer = t;
            anm_init(i_this, BCK_SING1, 5.0f, EMode_NONE, 1.0f, 0);
        } else if ((i_this->mTimers[0] == 0) && (a_this->current.pos.y < i_this->mTargetPos.y)) {
            i_this->mAnimState = 0;
            i_this->mTimers[0] = (s16)gabi::ftoi(cM_rndF(60.0f) + 20.0f);
            anm_init(i_this, BCK_WAIT2, 5.0f, EMode_LOOP, 1.0f, BAS_WAIT2);
        }
        break;

    case 2:
        frame = getFrame(i_this->mpMorf);
        if (frame == 8) {
            fopAcM_seStart(a_this, JA_SE_CV_KAMOME, 0);
        }
        if (isStop(i_this->mpMorf)) {
            i_this->mAnimState = 1;
            anm_init(i_this, BCK_WAIT1, 5.0f, EMode_LOOP, 1.0f, BAS_WAIT1);
        }
        break;

    case 10:
        break;

    case 20:
        if (isStop(i_this->mpMorf)) {
            i_this->mAnimState = 0;
            i_this->mTimers[0] = (s16)gabi::ftoi(cM_rndF(60.0f) + 20.0f);
            anm_init(i_this, BCK_WAIT2, 0.0f, EMode_LOOP, 1.0f, BAS_WAIT2);
        }
        break;
    }

    switch (i_this->mMoveState) {
    case 0:
        if (i_this->mTimers[1] == 0) {
            /* HD: wander radius 500 (GameCube 1000) */
            f32 rx = cM_rndFX(500.0f);
            x = (a_this->home.pos.x + rx) - a_this->current.pos.x;
            f32 rz = cM_rndFX(500.0f);
            z = (a_this->home.pos.z + rz) - a_this->current.pos.z;
            if (std_sqrtf(gabi::fmadds(x, x, z * z)) > 200.0f) {
                i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(150.0f) + 50.0f);
                i_this->mTargetPos.x = x + a_this->current.pos.x;
                /* HD: height range 300 (GameCube 500) */
                f32 ry = cM_rndF(300.0f);
                i_this->mTargetPos.y = a_this->home.pos.y + ry;
                i_this->mTargetPos.z = z + a_this->current.pos.z;
                i_this->mRotVelFade = 0.0f;

                if (a_this->current.pos.y < i_this->mTargetPos.y) {
                    i_this->mVelocityFwdTarget = REG0_F(10) + 20.0f;
                    i_this->mVelocityFwdTargetMaxVel = REG0_F(11) + 0.2f;
                } else {
                    i_this->mVelocityFwdTarget = REG0_F(12) + 36.0f;
                    i_this->mVelocityFwdTargetMaxVel = REG0_F(13) + 0.5f;
                }

                i_this->mRotVel = cM_rndF(300.0f) + 200.0f;

                if (i_this->mTimers[3] == 0) {
                    fopAc_ac_c* iVar3 = search_esa(i_this);
                    if (iVar3 != nullptr) {
                        i_this->mMoveState = 10;
                        i_this->mEsaProcID = fopAcM_GetID(iVar3);
                        i_this->mRotVel = 1000.0f;
                    }
                }
            }
        }
        break;

    case 10: {
        fopAc_ac_c* pfVar4 = fopAcM_SearchByID(i_this->mEsaProcID);
        if (pfVar4 != nullptr) {
            gabi::Local<cXyz> sp30;
            sp30->x = 0.0f;
            sp30->y = 0.0f;
            sp30->z = gabi::fmadds(REG6_F(16), 10.0f, -100.0f);
            cMtx_YrotS(calc_mtx(), a_this->current.angle.y);
            gabi::Local<cXyz> sp24;
            MtxPosition(sp30, sp24);
            i_this->mTargetPos.x = pfVar4->current.pos.x + sp24->x;
            /* HD: 70 + REG6_F(8) (GameCube 80 + REG6_F(8) * 10) */
            i_this->mTargetPos.y = pfVar4->current.pos.y + 70.0f + REG6_F(8);
            i_this->mTargetPos.z = pfVar4->current.pos.z + sp24->z;
            i_this->mVelocityFwdTarget = 20.0f;
            x = i_this->mTargetPos.x - a_this->current.pos.x;
            y = i_this->mTargetPos.y - a_this->current.pos.y;
            z = i_this->mTargetPos.z - a_this->current.pos.z;
            /* HD: REG6_F(9) + 100 (GameCube REG6_F(9) * 10 + 300) */
            if (std_sqrtf(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y))) < REG6_F(9) + 100.0f) {
                i_this->mMoveState = 0x14;
                anm_init(i_this, BCK_LAND1, 5.0f, EMode_NONE, 1.0f, BAS_LAND1);
                i_this->mAnimState = 10;
                a_this->speed.y = 0.0f;
                s16 r = (s16)gabi::ftoi(cM_rndFX(15000.0f));
                i_this->m2FC = a_this->current.angle.y + r;
                i_this->mTimers[2] = 0x3c;
                moveType = -1;
            }
        } else {
            i_this->mMoveState = 0;
            i_this->mGroundKeepTimer = 0x3c; /* HD: keep above ground for 60 steps */
        }
        break;
    }

    case 0x14: {
        moveType = -1;
        frame = getFrame(i_this->mpMorf);
        a_this->current.pos.y += a_this->speed.y;
        if (frame > REG6_S(2) + 0xf) {
            a_this->speed.y -= REG6_F(7) + 0.8f;
            cLib_addCalcAngleS2(&a_this->current.angle.y, i_this->m2FC, 2, 1000);
        }
        cLib_addCalcAngleS2(&a_this->current.angle.x, 0, 5, 0x800);
        cLib_addCalcAngleS2(&a_this->current.angle.z, 0, 5, 0x800);
        kamome_bgcheck(i_this);

        /* HD: landing on water (sea or a water polygon above the ground) aborts the landing */
        bool on_ground;
        if (daSea_ChkArea(a_this->current.pos.x, a_this->current.pos.z)) {
            f32 h = daSea_calcWave(a_this->current.pos.x, a_this->current.pos.z);
            on_ground = GetGroundH(i_this) > h;
        } else {
            gabi::Local<cXyz> sp1c;
            Vec3f p = {a_this->current.pos.x, a_this->current.pos.y + 100.0f, a_this->current.pos.z};
            *sp1c = p;
            f32 h = dBgS_GetWaterHeight(sp1c);
            on_ground = h == -1000000000.0f || GetGroundH(i_this) > h;
        }
        if (!on_ground) {
            i_this->mMoveState = 0;
            i_this->mAnimState = 0;
            i_this->mTimers[0] = (s16)gabi::ftoi(cM_rndF(60.0f) + 20.0f);
            anm_init(i_this, BCK_WAIT2, 5.0f, EMode_LOOP, 1.0f, BAS_WAIT2);
            break;
        }

        if (i_this->mAcch.ChkGroundHit()) {
            a_this->speed.y = -1.0f;
        } else {
            cLib_addCalc2(&a_this->current.pos.x, i_this->mTargetPos.x, 0.1f, std::fabs((f32)a_this->speed.x));
            cLib_addCalc2(&a_this->current.pos.z, i_this->mTargetPos.z, 0.1f, std::fabs((f32)a_this->speed.z));
        }

        if (isStop(i_this->mpMorf)) {
            anm_init(i_this, BCK_WAIT3, 5.0f, EMode_LOOP, 1.0f, 0);
        }

        if (i_this->mTimers[2] == 0) {
            i_this->mMoveState = 0x16;
        }
        break;
    }

    case 0x15: {
        moveType = 1;
        fopAc_ac_c* pfVar4 = search_esa(i_this);
        if (pfVar4 == nullptr) {
            i_this->mMoveState = 0x19;
            anm_init(i_this, BCK_FLY1, 0.0f, EMode_NONE, 1.0f, BAS_FLY1);
        } else {
            i_this->mEsaProcID = fopAcM_GetID(pfVar4);
            i_this->mMoveState = 0x16;
        }
        break;
    }

    case 0x16:
        moveType = 1;
        i_this->mVelocityFwdTarget = 0.0f;
        i_this->actor.speedF = 0.0f;
        i_this->mRotVel = 0.0f;
        if (i_this->mTimers[2] == 0) {
            i_this->mMoveState = 0x17;
            fopAc_ac_c* pfVar4 = fopAcM_SearchByID(i_this->mEsaProcID);
            if (pfVar4 != nullptr) {
                i_this->mTargetPos.x = pfVar4->current.pos.x;
                i_this->mTargetPos.z = pfVar4->current.pos.z;
                anm_init(i_this, BCK_MOVE1, 3.0f, EMode_LOOP, 1.0f, BAS_MOVE1);
            } else {
                i_this->mMoveState = 0x19;
                anm_init(i_this, BCK_FLY1, 0.0f, EMode_NONE, 1.0f, BAS_FLY1);
            }
        }
        break;

    case 0x13:
        moveType = 1;
        if (isStop(i_this->mpMorf)) {
            i_this->mMoveState = 0x15;
            i_this->mTimers[2] = (s16)gabi::ftoi(cM_rndF(30.0f) + 20.0f);
            anm_init(i_this, BCK_WAIT3, 5.0f, EMode_LOOP, 1.0f, 0);
        }
        i_this->mVelocityFwdTarget = 0.0f;
        a_this->speedF = 0.0f;
        break;

    case 0x17:
        moveType = 1;
        i_this->m2AB = 1;
        frame = getFrame(i_this->mpMorf);
        if ((frame >= l_kamomeHIO().m06) && (frame <= l_kamomeHIO().m08)) {
            i_this->mVelocityFwdTarget = i_this->mScale * 5.0f;
            a_this->speedF = i_this->mScale * 5.0f;
            i_this->mRotVel = 4000.0f;
            i_this->mRotVelFade = 1.0f;
        } else {
            i_this->mVelocityFwdTarget = 0.0f;
            a_this->speedF = 0.0f;
            i_this->mRotVel = 0.0f;
            x = i_this->mTargetPos.x - a_this->current.pos.x;
            z = i_this->mTargetPos.z - a_this->current.pos.z;
            if (std_sqrtf(gabi::fmadds(x, x, z * z)) < 85.0f) {
                anm_init(i_this, BCK_EAT1, 5.0f, EMode_NONE, 1.0f, 0);
                i_this->mMoveState = 0x18;
            }

            if (ChkWallHit(i_this)) {
                i_this->mMoveState = 0x19;
                anm_init(i_this, BCK_FLY1, 0.0f, EMode_NONE, 1.0f, BAS_FLY1);
            }
        }
        break;

    case 0x18:
        moveType = 1;
        i_this->mVelocityFwdTarget = 0.0f;
        a_this->speedF = 0.0f;
        frame = getFrame(i_this->mpMorf);
        if (frame == 9) {
            fopAc_ac_c* pfVar4 = fopAcM_SearchByID(i_this->mEsaProcID);
            if (pfVar4 != nullptr) {
                fopAcM_delete(pfVar4);
            }
        }

        if (isStop(i_this->mpMorf)) {
            if (cM_rndF(1.0f) < 0.2f) {
                i_this->mMoveState = 0x13;
                anm_init(i_this, BCK_SING2, 5.0f, EMode_NONE, 1.0f, 0);
            } else {
                i_this->mMoveState = 0x15;
                i_this->mTimers[2] = (s16)gabi::ftoi(cM_rndF(20.0f));
                anm_init(i_this, BCK_WAIT3, 5.0f, EMode_LOOP, 1.0f, 0);
            }
        }
        break;

    case 0x19:
        moveType = 1;
        a_this->speed.y = 0.0f;
        frame = getFrame(i_this->mpMorf);
        if (frame < 5) {
            i_this->mVelocityFwdTarget = 0.0f;
            i_this->mVelocityFwdTargetMaxVel = 1.0f;
        } else {
            i_this->mMoveState = 0;
            i_this->mAnimState = 20;
            i_this->mTimers[3] = (s16)gabi::ftoi(cM_rndF(100.0f) + 100.0f);
            i_this->mRiseTimer = REG0_S(4) + 10;
            if (i_this->mPathIdx != 0xff) {
                i_this->mbUsePathMovement = i_this->mPathIdx + 1;
                i_this->m308 = 1;
            } else {
                i_this->mTimers[1] = 0x32;
                i_this->mRotVel = gabi::fmadds(REG0_F(4), 10.0f, 1000.0f);
                i_this->mVelocityFwdTarget = 25.0f;
                i_this->mVelocityFwdTargetMaxVel = 2.0f;
                a_this->speedF = 0.0f;
                set_fly_away_target(i_this);
            }
        }
        break;
    }

    switch (moveType) {
    case 0:
        kamome_pos_move(i_this);
        /* HD: while mGroundKeepTimer runs, stay at least 50 + REG18_F(3) above the ground */
        if (i_this->mGroundKeepTimer != 0) {
            gabi::Local<dBgS_GndChk_l> gndChk;
            dBgS_GndChk_ct(gndChk);
            Vec3f p = {a_this->current.pos.x, a_this->current.pos.y + 200.0f, a_this->current.pos.z};
            gndChk->m_pos = p;
            f32 gy = cBgS_GroundCross(dComIfG_Bgsp(), gndChk) + 50.0f + REG_F(18, 3);
            if (!(a_this->current.pos.y > gy)) {
                a_this->current.pos.y = gy;
            }
            dBgS_GndChk_dt(gndChk);
        }
        break;

    case 1:
        kamome_ground_pos_move(i_this);
        if (i_this->mMoveState != 0x19) {
            if (a_this->current.pos.y - GetGroundH(i_this) > 200.0f ||
                fopAcM_searchActorDistance(a_this, dComIfGp_getPlayer0()) < l_kamomeHIO().m0C) {
                i_this->mMoveState = 0;
                fopAc_ac_c* pfVar5 = fopAcM_SearchByID(i_this->mEsaProcID);
                if (pfVar5 != nullptr) {
                    esa_set_field_0x298(pfVar5, 0);
                }
                i_this->mAnimState = 0;
                i_this->mTimers[0] = (s16)gabi::ftoi(cM_rndF(60.0f) + 20.0f);
                anm_init(i_this, BCK_WAIT2, 3.0f, EMode_LOOP, 1.5f, BAS_WAIT2);
                if (i_this->mPathIdx != 0xff) {
                    i_this->mbUsePathMovement = i_this->mPathIdx + 1;
                    s16 t = (s16)gabi::ftoi(cM_rndF(250.0f) + 250.0f);
                    i_this->m308 = 1;
                    i_this->mTimers[3] = t;
                } else {
                    i_this->mTimers[1] = 0x32;
                    i_this->mRotVel = gabi::fmadds(REG0_F(4), 10.0f, 5000.0f);
                    i_this->mVelocityFwdTarget = 30.0f;
                    i_this->mVelocityFwdTargetMaxVel = 3.0f;
                    a_this->speedF = 0.0f;
                    set_fly_away_target(i_this);
                    i_this->mTimers[3] = (s16)gabi::ftoi(cM_rndF(250.0f) + 250.0f);
                }
            }
        }
        break;
    }
}
VERIFY(0x02189270, kamome_auto_move);

/* ---- leftover functions of the translation unit ---- */

/* 0218A4A8 kamome_class::~kamome_class (deleting; vtable slot 10012408) */
static void daKamome_c_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0218A4A8, void, p, flags);
    if (p != nullptr) {
        u32 t = gabi::ea(p);
        gabi::call(0x02515AE8, t + 0x670, 2); /* dCcD_Sph */
        gabi::call(0x02515860, t + 0x634, 2); /* dCcD_Stts */
        gabi::store<u32>(t + 0x490, 0x1001238C);
        gabi::store<u32>(t + 0x484, 0x1001239C);
        gabi::call(0x024EFD9C, t + 0x470, 0); /* line check */
        gabi::call(0x02018034, t + 0x444, 2);
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) operator_delete(p);
    }
}
VERIFY(0x0218A4A8, daKamome_c_dt);

/* 0218A544 sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 10012318, after the destructor 0218925C */
static void kamome_SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x0218A544, void, p);
}
VERIFY(0x0218A544, kamome_SafeString_assureTermination);
