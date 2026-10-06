/**
 * d_a_kamome.cpp (WWHD), part: daKamome_Execute and the move functions GHS inlined into it
 * (heisou_control, ko_check, kamome_heisou_move, search_master, kamome_path_move,
 * kamome_imouto_move, search_imouto, kamome_imouto2_move).
 *
 * Separate translation unit while d_a_kamome.cpp is
 * edited in parallel; the bindings below for functions of d_a_kamome.cpp go away on merge.
 *
 * Float comparisons: GHS sometimes branches on the negated condition (`bgt skip` for `a <= b`),
 * which differs from C for NaN. Those are written as !(a > b) etc.
 */
#include "d/actor/d_a_kamome.h"

/* ---- functions of d_a_kamome.cpp (guest calls, same signatures) ---- */
static void anm_init(kamome_class* i_this, int a, float m, unsigned char l, float s, int snd) {
    gabi::call(0x02185558, i_this, a, m, l, s, snd);
}
static fopAc_ac_c* search_esa(kamome_class* i_this) { return gabi::call<fopAc_ac_c*>(0x02185808, i_this); }
static void kamome_pos_move(kamome_class* i_this) { gabi::call(0x02185B78, i_this); }
static void kamome_bgcheck(kamome_class* i_this) { gabi::call(0x02185DF0, i_this); }
static void kamome_ground_pos_move(kamome_class* i_this) { gabi::call(0x02185E80, i_this); }
static void daKamome_setMtx(kamome_class* i_this) { gabi::call(0x021860B0, i_this); }
static void kamome_auto_move(kamome_class* i_this) { gabi::call(0x02189270, i_this); }

/* ---- more bindings ---- */

static inline void McaMorf_play(mDoExt_McaMorf_c* m, cXyz* pos, u32 se, u8 reverb) { gabi::call(0x025E535C, m, pos, se, reverb); }
static inline s32 McaMorf_getFrame(mDoExt_McaMorf_c* m) { return gabi::ftoi(gabi::load<f32>(gabi::ea(m) + 0x9C)); }
/* J3DFrameCtrl: rate +0x98, state +0xA7 */
static inline bool McaMorf_isStop(mDoExt_McaMorf_c* m) {
    return (gabi::load<u8>(gabi::ea(m) + 0xA7) & 1) || gabi::load<f32>(gabi::ea(m) + 0x98) == 0.0f;
}
static inline fopAc_ac_c* dComIfGp_getPlayer() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(gabi::ea(dComIfGp_get()) + 0x5B2C)); }
static inline bool dComIfGp_att_chkEnemySound() { return (gabi::load<u32>(gabi::ea(dComIfGp_get()) + 0x5824) >> 8) & 1; }
static inline bool dComIfGp_checkPlayerStatus0_SHIP_RIDE() { return (gabi::load<u32>(gabi::ea(dComIfGp_get()) + 0x5CD8) & 0x10000) != 0; }
static inline fopAc_ac_c* fopAcM_searchPlayer() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(gabi::ea(dComIfGp_get()) + 0x12A0 + 0x488C)); }
/* HD: cXyz::operator- is out of line: (this, result, other) */
/* cXyz::abs() of a difference, as HD computes it: a - b into a temporary, copied, squared mag */
static inline f32 diff_abs(cXyz* a, cXyz* b) {
    gabi::Local<cXyz> res;
    res->set(0.0f, 0.0f, 0.0f); /* the harness compares the (uninitialised) result buffer: start from a fresh slot */
    cXyz_mi(a, res, b);
    gabi::Local<cXyz> tmp;
    tmp->copy(*res);
    return std_sqrtf(PSVECSquareMag(tmp));
}
static inline void cM3dGSph_SetC(void* sph, cXyz* c) { gabi::call(0x02018D40, sph, c); }
static inline void cM3dGSph_SetR(void* sph, f32 r) { gabi::call(0x02018C8C, sph, r); }
static inline BOOL dCcD_GObjInf_ChkCoHit(void* obj) { return gabi::call<BOOL>(0x02516464, obj); }
/* cM_ssin: sin/cos table (8 bytes per entry) at 0x104A44F8 */

static inline be<s16>& timer(kamome_class* i_this, int i) { return i_this->mTimers[i]; }

/* dPath / dPnt (unchanged from GameCube) */
struct dPnt_l {
    /* 0x0 */ u8 _0[3];
    /* 0x3 */ be<u8> mArg3;
    /* 0x4 */ cXyz m_position;
};
struct dPath_l {
    /* 0x0 */ be<u16> m_num;
    /* 0x2 */ be<u16> m_nextID;
    /* 0x4 */ u8 _4;
    /* 0x5 */ be<u8> m_closed;
    /* 0x6 */ u8 _6[2];
    /* 0x8 */ gptr<dPnt_l> m_points;
};
static inline dPath_l* path(kamome_class* i_this) { return (dPath_l*)(dPath*)i_this->mpPath; }

/* l_kamomeHIO (0x104648C8) */
struct kamomeHIO_l {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> m05;
    /* 0x02 */ be<s16> m06;
    /* 0x04 */ be<s16> m08;
    /* 0x06 */ u8 _06[2];
    /* 0x08 */ be<f32> m0C;
    /* 0x0C */ be<f32> m10;
};
static inline kamomeHIO_l& hio() { return *gabi::at<kamomeHIO_l>(0x104648C8); }
static be<s32>& ko_count() { return *gabi::at<be<s32>>(0x104648A8); }

enum {
    BAS_FLY1 = 0x5, BAS_LAND1 = 0x6, BAS_MOVE1 = 0x7, BAS_WAIT1 = 0x8, BAS_WAIT2 = 0x9,
    BCK_EAT1 = 0xC, BCK_FLY1 = 0xD, BCK_LAND1 = 0xE, BCK_MOVE1 = 0xF, BCK_SING1 = 0x10, BCK_SING2 = 0x11,
    BCK_WAIT1 = 0x12, BCK_WAIT2 = 0x13, BCK_WAIT3 = 0x14,
};
enum { EMode_NONE = 0, EMode_LOOP = 2 };
enum { JA_SE_CV_KAMOME = 0x4805 };

/* dPath_GetNext: the stage's path table through dStage_stageDt_c::getPathInf() (virtual) */
static inline void path_next_point(kamome_class* i_this) {
    s8 idx = (s8)(i_this->mCurPointIdx + i_this->mPathIdxIncr);
    i_this->mCurPointIdx = idx;
    dPath_l* p = path(i_this);
    if (idx >= (s8)(u8)p->m_num) {
        u16 nextID;
        if (p->m_closed & 1) {
            i_this->mCurPointIdx = 0;
            nextID = path(i_this)->m_nextID;
        } else {
            i_this->mPathIdxIncr = -1;
            i_this->mCurPointIdx = (s8)(path(i_this)->m_num - 2);
            nextID = p->m_nextID;
        }
        if (nextID != 0xFFFF) {
            u32 stage = gabi::ea(dComIfGp_get()) + 0x5150;
            u32 fn = gabi::load<u32>(gabi::load<u32>(stage) + 0x18C);
            u32 pathInf = gabi::ea(gabi::call_ptr<void*>(fn, stage));
            i_this->mpPath = gabi::at<dPath>(gabi::load<u32>(pathInf + 4) + nextID * 0xC);
        }
    } else if (idx < 0) {
        i_this->mPathIdxIncr = 1;
        i_this->mCurPointIdx = 1;
    }
}

static inline dPnt_l* cur_point(kamome_class* i_this) {
    return gabi::at<dPnt_l>(gabi::ea(path(i_this)->m_points.get()) + (s8)i_this->mCurPointIdx * 0x10);
}

/* fall back to the ground after landing (case 0x14 of the move functions) */
static inline void landing(kamome_class* i_this, f32 groundSpeedY, s8 nextState) {
    fopAc_ac_c* a_this = &i_this->actor;
    s32 frame = McaMorf_getFrame(i_this->mpMorf);
    a_this->current.pos.y += a_this->speed.y;
    if (frame > REG_S(6, 2) + 0xf) {
        a_this->speed.y -= REG_F(6, 7) + 0.8f;
        cLib_addCalcAngleS2(&a_this->current.angle.y, i_this->m2FC, 2, 1000);
    }
    cLib_addCalcAngleS2(&a_this->current.angle.x, 0, 5, 0x800);
    cLib_addCalcAngleS2(&a_this->current.angle.z, 0, 5, 0x800);
    kamome_bgcheck(i_this);

    if (i_this->mAcch.ChkGroundHit()) {
        a_this->speed.y = groundSpeedY;
    } else {
        cLib_addCalc2(&a_this->current.pos.x, i_this->mTargetPos.x, 0.1f, std::fabs((f32)a_this->speed.x));
        cLib_addCalc2(&a_this->current.pos.z, i_this->mTargetPos.z, 0.1f, std::fabs((f32)a_this->speed.z));
    }

    if (McaMorf_isStop(i_this->mpMorf)) {
        anm_init(i_this, BCK_WAIT3, 5.0f, EMode_LOOP, 1.0f, 0);
    }

    if (timer(i_this, 2) == 0) {
        i_this->mMoveState = nextState;
    }
}

/* start landing when close to the target (case 10) */
static inline bool start_landing(kamome_class* i_this, f32 dist) {
    fopAc_ac_c* a_this = &i_this->actor;
    if (dist < gabi::fmadds(REG_F(6, 9), 10.0f, 300.0f)) {
        i_this->mMoveState = 0x14;
        anm_init(i_this, BCK_LAND1, 5.0f, EMode_NONE, 1.0f, BAS_LAND1);
        i_this->mAnimState = 10;
        a_this->speed.y = 0.0f;
        s16 r = (s16)gabi::ftoi(cM_rndFX(15000.0f));
        i_this->m2FC = a_this->current.angle.y + r;
        timer(i_this, 2) = 0x3c;
        return true;
    }
    return false;
}

static inline f32 dist3(kamome_class* i_this, f32 dy) {
    fopAc_ac_c* a_this = &i_this->actor;
    f32 x = i_this->mTargetPos.x - a_this->current.pos.x;
    f32 z = i_this->mTargetPos.z - a_this->current.pos.z;
    return std_sqrtf(gabi::fmadds(z, z, gabi::fmadds(x, x, dy * dy)));
}

static inline f32 dist2(kamome_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    f32 x = i_this->mTargetPos.x - a_this->current.pos.x;
    f32 z = i_this->mTargetPos.z - a_this->current.pos.z;
    return std_sqrtf(gabi::fmadds(x, x, z * z));
}

/* the shared animation states 0..2 */
static inline void anim_wait_sing(kamome_class* i_this, int animState, bool pathMove) {
    fopAc_ac_c* a_this = &i_this->actor;
    switch (animState) {
    case 0: {
        s32 frame = McaMorf_getFrame(i_this->mpMorf);
        bool go = pathMove ? (a_this->current.pos.y > i_this->mTargetPos.y && frame == REG0_S(0) + 9)
                           : (timer(i_this, 0) == 0 && frame == REG0_S(0) + 9);
        if (go) {
            i_this->mAnimState = 1;
            if (pathMove) timer(i_this, 0) = (s16)gabi::ftoi(cM_rndF(200.0f) + 50.0f);
            anm_init(i_this, BCK_WAIT1, REG0_F(0) + 12.0f, EMode_LOOP, 1.0f, BAS_WAIT1);
        }
        break;
    }
    case 1:
        if ((i_this->mGlobalTimer & 0x3f) == 0 && cM_rndF(1.0f) < 0.5f) {
            i_this->mGlobalTimer = (s16)gabi::ftoi(cM_rndF(10000.0f));
            i_this->mAnimState = 2;
            anm_init(i_this, BCK_SING1, 5.0f, EMode_NONE, 1.0f, 0);
        } else if (pathMove) {
            if (!(a_this->current.pos.y > i_this->mTargetPos.y)) {
                i_this->mAnimState = 0;
                anm_init(i_this, BCK_WAIT2, 5.0f, EMode_LOOP, 1.0f, BAS_WAIT2);
            }
        } else if (timer(i_this, 0) == 0 && a_this->current.pos.y < i_this->mTargetPos.y) {
            i_this->mAnimState = 0;
            timer(i_this, 0) = (s16)gabi::ftoi(cM_rndF(60.0f) + 20.0f);
            anm_init(i_this, BCK_WAIT2, 5.0f, EMode_LOOP, 1.0f, BAS_WAIT2);
        }
        break;
    case 2:
        if (McaMorf_getFrame(i_this->mpMorf) == 8) {
            fopAcM_seStart(a_this, JA_SE_CV_KAMOME, 0);
        }
        if (McaMorf_isStop(i_this->mpMorf)) {
            i_this->mAnimState = 1;
            if (pathMove) timer(i_this, 0) = (s16)gabi::ftoi(cM_rndF(100.0f) + 20.0f);
            anm_init(i_this, BCK_WAIT1, 5.0f, EMode_LOOP, 1.0f, BAS_WAIT1);
        }
        break;
    }
}

/* inlined: heisou_control (mType 6, "escort": spawns followers while Link rides the boat) */
static inline void heisou_control(kamome_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer();
    i_this->mbNoDraw = 1;
    f32 t = dComIfGs_getTime();
    if (!(t < 90.0f) && !(t > 270.0f) && dKy_rain_check() < 10) {
        if (dComIfGp_checkPlayerStatus0_SHIP_RIDE() && (i_this->mGlobalTimer & 0x7f) == 0) {
            /* ko_check */
            ko_count() = 0;
            fpcM_Search(0x02185FE4 /* ko_s_sub */, &i_this->actor);
            s32 ko = ko_count();
            if (ko < i_this->mKoMaxCount) {
                gabi::Local<cXyz> sp08;
                Vec3f p = player->current.pos.get();
                *sp08 = p;
                sp08->y = p.y + 4000.0f;
                fopAcM_create(0xC2 /* KAMOME */, (u32)ko << 8 | 0xffff0007, sp08, fopAcM_GetRoomNo(&i_this->actor), nullptr,
                              nullptr, -1, 0);
            }
        }
    }
}

/* inlined: kamome_heisou_move (mType 7: flies along with Link's boat) */
static inline void kamome_heisou_move(kamome_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    fopAc_ac_c* player = dComIfGp_getPlayer();

    anim_wait_sing(i_this, i_this->mAnimState, false);

    i_this->m2B4++;

    if (i_this->mMoveState == 0) {
        s16 c = i_this->m2B4;
        gabi::Local<cXyz> sp44;
        f32 s1 = cM_ssin(c * (REG_S(17, 5) + 200));
        f32 s2 = cM_ssin(c * (REG_S(17, 6) + 30));
        sp44->x = s1 * (REG_F(17, 5) + 300.0f);
        sp44->y = i_this->m2B8 + 600.0f + REG_F(17, 6);
        sp44->z = gabi::fmadds(s2, REG_F(17, 7) + 300.0f, REG_F(17, 4) + 150.0f);
        cMtx_YrotS(calc_mtx(), player->shape_angle.y);
        gabi::Local<cXyz> sp34;
        MtxPosition(sp44, sp34);
        f32 tx = sp34->x + player->current.pos.x;
        i_this->mTargetPos.x = tx;
        f32 tz = sp34->z + player->current.pos.z;
        i_this->mTargetPos.z = tz;
        if (daSea_ChkArea(tx, tz)) {
            f32 w = daSea_calcWave(i_this->mTargetPos.x, i_this->mTargetPos.z);
            i_this->mTargetPos.y = sp34->y + w;
        } else {
            i_this->mTargetPos.y = player->current.pos.y + sp34->y;
        }
    }

    BOOL iVar2 = FALSE;
    f32 t = dComIfGs_getTime();
    if (!(t < 90.0f) && !(t > 270.0f) && dKy_rain_check() < 10 && !dComIfGp_att_chkEnemySound() &&
        dComIfGp_checkPlayerStatus0_SHIP_RIDE()) {
        /* search_master */
        kamome_class* master = (kamome_class*)fpcM_Search(0x02186050 /* h_s_sub */, a_this);
        if (master != nullptr && i_this->mKoMaxCount < master->mKoMaxCount) iVar2 = TRUE;
    }

    f32 fVar6;
    i_this->mVelocityFwdTarget = diff_abs(&player->old.pos, &player->current.pos) * (REG_F(17, 8) + 1.2f);
    if (i_this->mVelocityFwdTarget > REG_F(17, 13) + 30.0f && iVar2) {
        fVar6 = 0.0f;
        if (diff_abs(&i_this->mTargetPos, &a_this->current.pos) < REG_F(17, 9) + 200.0f) {
            i_this->mVelocityFwdTarget = 0.0f;
        }
    } else {
        fVar6 = a_this->home.pos.y;
        i_this->mVelocityFwdTarget = 20.0f;
    }

    cLib_addCalc2(&i_this->m2B8, fVar6, 1.0f, 20.0f);
    i_this->mVelocityFwdTargetMaxVel = REG_F(17, 10) + 0.25f;
    i_this->mRotVel = REG_F(17, 11) + 300.0f;
    kamome_pos_move(i_this);
    s16 ang = fopAcM_searchActorAngleY(a_this, fopAcM_searchPlayer());
    i_this->mJointRotYTarget = (s16)(ang - a_this->current.angle.y);

    s16 sVar3 = REG_S(17, 3) + 5000;
    if (i_this->mJointRotYTarget > sVar3) {
        i_this->mJointRotYTarget = sVar3;
    } else if (i_this->mJointRotYTarget < (s16)-sVar3) {
        i_this->mJointRotYTarget = -sVar3;
    }

    if (a_this->current.pos.y > 3000.0f) {
        i_this->mbNoDraw = 1;
    }
}

/* inlined: kamome_path_move */
static inline void kamome_path_move(kamome_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    s8 moveType = 0;
    dComIfGp_get(); /* HD: unused player lookup */

    switch (i_this->mAnimState) {
    case 0:
    case 1:
    case 2:
        anim_wait_sing(i_this, i_this->mAnimState, true);
        break;
    case 20:
        if (McaMorf_isStop(i_this->mpMorf)) {
            i_this->mAnimState = 0;
            anm_init(i_this, BCK_WAIT2, 0.0f, EMode_LOOP, 1.0f, BAS_WAIT2);
        }
        break;
    }

    switch (i_this->mMoveState) {
    case 0:
        path_next_point(i_this);
        /* fallthrough */
    case -1: {
        i_this->mMoveState = 1;
        dPnt_l* pnt = cur_point(i_this);
        if (i_this->m308 != 0) {
            i_this->m308 = 0;
            i_this->mVelocityFwdTarget = 25.0f;
            i_this->mVelocityFwdTargetMaxVel = 2.0f;
        } else if (a_this->current.pos.y < i_this->mTargetPos.y) {
            i_this->mVelocityFwdTarget = cM_rndF(5.0f) + 20.0f;
            i_this->mVelocityFwdTargetMaxVel = REG0_F(11) + 1.0f;
        } else {
            i_this->mVelocityFwdTarget = cM_rndF(5.0f) + 30.0f;
            i_this->mVelocityFwdTargetMaxVel = REG0_F(13) + 1.0f;
        }
        i_this->mRotVelFade = REG0_F(7);
        if (pnt->mArg3 == 1) {
            i_this->mMoveState = 10;
            i_this->mTargetPos = pnt->m_position.get();
        } else {
            f32 r = cM_rndFX(150.0f);
            i_this->mTargetPos.x = pnt->m_position.x + r;
            r = cM_rndFX(150.0f);
            i_this->mTargetPos.y = pnt->m_position.y + r;
            r = cM_rndFX(150.0f);
            i_this->mTargetPos.z = pnt->m_position.z + r;
        }

        if (timer(i_this, 3) == 0) {
            fopAc_ac_c* esa = search_esa(i_this);
            if (esa != nullptr) {
                i_this->mEsaProcID = fopAcM_GetID(esa);
                i_this->mMoveState = 10;
                i_this->mRotVel = 1000.0f;
                i_this->mbUsePathMovement = 0;
            }
        }
        break;
    }

    case 1: {
        cLib_addCalc2(&i_this->mRotVelFade, 1.0f, 1.0f, 0.04f);
        f32 y = i_this->mTargetPos.y - a_this->current.pos.y;
        f32 sqrt = dist3(i_this, y);
        f32 rv = gabi::fmadds(REG0_F(5), 10.0f, 100.0f) + gabi::fmadds(REG0_F(6), 1000.0f, 500000.0f) / sqrt;
        i_this->mRotVel = rv;
        f32 lim = gabi::fmadds(REG0_F(4), 10.0f, 5000.0f);
        if (rv > lim) {
            i_this->mRotVel = lim;
        }

        if (sqrt < gabi::fmadds(REG0_F(10), 10.0f, 300.0f)) {
            dPnt_l* pnt = cur_point(i_this);
            i_this->mMoveState = 0;
            if (pnt->mArg3 == 6) {
                fopAcM_delete(a_this);
            }
        }
        break;
    }

    case 10: {
        i_this->mVelocityFwdTarget = 20.0f;
        f32 y = gabi::fmadds(REG_F(6, 8), 10.0f, i_this->mTargetPos.y + 50.0f) - a_this->current.pos.y;
        if (start_landing(i_this, dist3(i_this, y))) moveType = -1;
        break;
    }

    case 0x14:
        moveType = -1;
        landing(i_this, -0.5f, 0x17);
        break;

    case 0x16:
        moveType = 1;
        i_this->m2AB = 1;
        if (timer(i_this, 2) == 0) {
            path_next_point(i_this);
            dPnt_l* pnt = cur_point(i_this);
            i_this->mTargetPos = pnt->m_position.get();
            if (pnt->mArg3 == 2) {
                i_this->mMoveState = 0x19;
                anm_init(i_this, BCK_FLY1, 0.0f, EMode_NONE, 1.0f, BAS_FLY1);
            } else {
                i_this->mMoveState = 0x17;
                i_this->mRotVel = 2500.0f;
                i_this->mRotVelFade = 1.0f;
                anm_init(i_this, BCK_MOVE1, 3.0f, EMode_LOOP, 1.0f, BAS_MOVE1);
            }
        }
        i_this->mVelocityFwdTarget = 0.0f;
        a_this->speedF = 0.0f;
        break;

    case 0x13:
        moveType = 1;
        if (McaMorf_isStop(i_this->mpMorf)) {
            i_this->mMoveState = 0x16;
        }
        i_this->mVelocityFwdTarget = 0.0f;
        a_this->speedF = 0.0f;
        break;

    case 0x17: {
        moveType = 1;
        s32 frame = McaMorf_getFrame(i_this->mpMorf);
        if (frame >= hio().m06 && frame <= hio().m08) {
            f32 v = 5.0f * i_this->mScale;
            a_this->speedF = v;
            i_this->mVelocityFwdTarget = v;
        } else {
            i_this->mVelocityFwdTarget = 0.0f;
            a_this->speedF = 0.0f;
            if (dist2(i_this) < 50.0f) {
                dPnt_l* pnt = cur_point(i_this);
                if (cM_rndF(1.0f) < 0.5f) {
                    i_this->mMoveState = 0x13;
                    anm_init(i_this, BCK_SING2, 5.0f, EMode_NONE, 1.0f, 0);
                } else {
                    i_this->mMoveState = 0x16;
                    if (pnt->mArg3 == 3) {
                        anm_init(i_this, BCK_WAIT3, 5.0f, EMode_LOOP, 1.0f, 0);
                        timer(i_this, 2) = (s16)gabi::ftoi(cM_rndF(50.0f) + 50.0f);
                    }
                }
            }
        }
        break;
    }

    case 0x19:
        moveType = 1;
        a_this->speed.y = 0.0f;
        if (McaMorf_getFrame(i_this->mpMorf) < 5) {
            i_this->mVelocityFwdTarget = 0.0f;
            i_this->mVelocityFwdTargetMaxVel = 1.0f;
        } else {
            i_this->mMoveState = -1;
            i_this->mRotVel = gabi::fmadds(REG0_F(4), 10.0f, 5000.0f);
            i_this->mVelocityFwdTarget = 25.0f;
            i_this->mVelocityFwdTargetMaxVel = 2.0f;
            a_this->speedF = 0.0f;
            i_this->mAnimState = 20;
            i_this->mRiseTimer = REG0_S(4) + 10;
        }
        break;
    }

    switch (moveType) {
    case 0:
        kamome_pos_move(i_this);
        break;
    case 1:
        kamome_ground_pos_move(i_this);
        break;
    }
}

/* dBgS_GndChk (stack object, HD layout) */
struct dBgS_GndChk_l {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x40 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x4C */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ cXyz m_pos;
    /* 0x30 */ u8 _30[0x40 - 0x30];
    /* 0x40 */ be<u32> __vtbl_40;
    /* 0x44 */ be<u8> mPass[7];
    /* 0x4B */ u8 _4B;
    /* 0x4C */ be<u32> __vtbl_4C;
    /* 0x50 */ be<u32> mGrp;
};
WWHD_SIZE(dBgS_GndChk_l, 0x54);

static inline void ground_chk_dt(dBgS_GndChk_l* chk) {
    chk->__vtbl_20 = 0x1001234C;
    chk->__vtbl_40 = 0x1001236C;
    chk->__vtbl_4C = 0x1001232C;
    gabi::call(0x02008DAC, chk, 0); /* cBgS_Chk::~cBgS_Chk */
}

/* back to free flight after being disturbed on the ground (shared by the ground states) */
static inline void take_off(kamome_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    i_this->mMoveState = 0;
    i_this->mAnimState = 0;
    timer(i_this, 0) = (s16)gabi::ftoi(cM_rndF(60.0f) + 20.0f);
    anm_init(i_this, BCK_WAIT2, 3.0f, EMode_LOOP, 1.5f, BAS_WAIT2);

    if (i_this->mPathIdx != 0xff) {
        i_this->mbUsePathMovement = i_this->mPathIdx + 1;
        timer(i_this, 3) = (s16)gabi::ftoi(cM_rndF(250.0f) + 250.0f);
        i_this->m308 = 1;
    } else {
        timer(i_this, 1) = 0x32;
        i_this->mRotVel = gabi::fmadds(REG0_F(4), 10.0f, 5000.0f);
        i_this->mVelocityFwdTarget = 30.0f;
        i_this->mVelocityFwdTargetMaxVel = 3.0f;
        a_this->speedF = 0.0f;
        MtxTrans(a_this->current.pos.x, a_this->current.pos.y, a_this->current.pos.z, 0);
        gabi::Local<cXyz> sp34;
        sp34->set(0.0f, 1000.0f, 2000.0f);
        cMtx_YrotM(calc_mtx(), a_this->current.angle.y);
        MtxPosition(sp34, &i_this->mTargetPos);
        timer(i_this, 3) = (s16)gabi::ftoi(cM_rndF(250.0f) + 250.0f);
    }
}

/* inlined: kamome_imouto_move (mType 4: Aryll's seagull) */
static inline void kamome_imouto_move(kamome_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    fopAc_ac_c* player = dComIfGp_getPlayer();
    s8 cVar8 = 0;

    switch (i_this->mAnimState) {
    case 0:
    case 1:
    case 2:
        anim_wait_sing(i_this, i_this->mAnimState, false);
        break;
    case 20:
        if (McaMorf_isStop(i_this->mpMorf)) {
            i_this->mAnimState = 0;
            timer(i_this, 0) = (s16)gabi::ftoi(cM_rndF(60.0f) + 20.0f);
            anm_init(i_this, BCK_WAIT2, 0.0f, EMode_LOOP, 1.0f, BAS_WAIT2);
        }
        break;
    }

    switch (i_this->mMoveState) {
    case 0:
        if (timer(i_this, 1) == 0) {
            f32 r = cM_rndFX(1000.0f);
            f32 fVar11 = (a_this->home.pos.x + r) - a_this->current.pos.x;
            r = cM_rndFX(1000.0f);
            f32 fVar9 = (a_this->home.pos.z + r) - a_this->current.pos.z;
            if (std_sqrtf(gabi::fmadds(fVar11, fVar11, fVar9 * fVar9)) > 200.0f) {
                timer(i_this, 1) = (s16)gabi::ftoi(cM_rndF(150.0f) + 50.0f);
                i_this->mTargetPos.x = fVar11 + a_this->current.pos.x;
                r = cM_rndF(500.0f);
                f32 ty = a_this->home.pos.y + r;
                i_this->mTargetPos.y = ty;
                i_this->mTargetPos.z = fVar9 + a_this->current.pos.z;
                i_this->mRotVelFade = 0.0f;

                if (a_this->current.pos.y < ty) {
                    i_this->mVelocityFwdTarget = REG0_F(10) + 20.0f;
                    i_this->mVelocityFwdTargetMaxVel = REG0_F(11) + 0.2f;
                } else {
                    i_this->mVelocityFwdTarget = REG0_F(12) + 36.0f;
                    i_this->mVelocityFwdTargetMaxVel = REG0_F(13) + 0.5f;
                }

                i_this->mRotVel = cM_rndF(300.0f) + 200.0f;

                if (timer(i_this, 3) == 0) {
                    fopAc_ac_c* imouto = (fopAc_ac_c*)fpcM_Search(0x02185960 /* s_a_i_sub */, a_this); /* search_imouto */
                    i_this->mpTargetActor = imouto;
                    if (imouto != nullptr) {
                        if (diff_abs(&player->current.pos, &imouto->current.pos) < hio().m10) {
                            timer(i_this, 3) = 0x32;
                            i_this->mpTargetActor = nullptr;
                        } else {
                            fopAc_ac_c* t = i_this->mpTargetActor;
                            r = cM_rndFX(150.0f);
                            i_this->mTargetPos.x = t->current.pos.x + r;
                            t = i_this->mpTargetActor;
                            i_this->mTargetPos.y = t->current.pos.y + 80.0f;
                            r = cM_rndFX(150.0f);
                            i_this->mTargetPos.z = t->current.pos.z + r;
                            /* HD: only lands next to Aryll when she is on the ground */
                            gabi::Local<dBgS_GndChk_l> chk;
                            gabi::call(0x02008E0C, chk.get());
                            chk->__vtbl_10 = 0x1001233C;
                            for (int i = 0; i < 7; i++) chk->mPass[i] = 0;
                            chk->mpGrpPassChk = gabi::ea(chk.get()) + 0x4C;
                            chk->__vtbl_4C = 0x1001235C;
                            chk->__vtbl_40 = 0x1001236C;
                            chk->mpPolyPassChk = gabi::ea(chk.get()) + 0x40;
                            chk->__vtbl_20 = 0x1001234C;
                            chk->mGrp = 1;
                            chk->m_pos.copy(i_this->mTargetPos);
                            f32 gnd = gabi::call<f32>(0x02008974, dComIfG_Bgsp(), chk.get());
                            if (i_this->mpTargetActor->current.pos.y < gnd + 10.0f) {
                                i_this->mMoveState = 10;
                                i_this->mRotVel = 1000.0f;
                                i_this->mTargetPos.y = i_this->mTargetPos.y - 20.0f;
                            }
                            ground_chk_dt(chk);
                        }
                    }
                }
            }
        }
        break;

    case 10:
        if (diff_abs(&player->current.pos, &i_this->mTargetPos) < hio().m10) {
            i_this->mMoveState = 0;
            timer(i_this, 3) = 0x32;
        } else {
            f32 y = i_this->mTargetPos.y - a_this->current.pos.y;
            f32 x = i_this->mTargetPos.x - a_this->current.pos.x;
            f32 z = i_this->mTargetPos.z - a_this->current.pos.z;
            f32 sq = gabi::fmadds(z, z, gabi::fmadds(x, x, y * y));
            i_this->mVelocityFwdTarget = 20.0f;
            if (start_landing(i_this, std_sqrtf(sq))) cVar8 = -1;
        }
        break;

    case 0x14:
        cVar8 = -1;
        landing(i_this, -1.0f, 0x16);
        break;

    case 0x15:
        cVar8 = 1;
        i_this->mMoveState = 0x16;
        break;

    case 0x16:
        cVar8 = 1;
        i_this->mVelocityFwdTarget = 0.0f;
        i_this->mRotVel = 0.0f;
        i_this->m2AB = 1;
        a_this->speedF = 0.0f;

        if (timer(i_this, 2) == 0) {
            i_this->mMoveState = 0x17;
            f32 m = REG0_F(8) + 150.0f;
            fopAc_ac_c* t = i_this->mpTargetActor;
            f32 r = cM_rndFX(m);
            i_this->mTargetPos.x = t->current.pos.x + r;
            m = REG0_F(8) + 150.0f;
            t = i_this->mpTargetActor;
            r = cM_rndFX(m);
            i_this->mTargetPos.z = t->current.pos.z + r;
            anm_init(i_this, BCK_MOVE1, 3.0f, EMode_LOOP, 1.0f, BAS_MOVE1);
        }
        break;

    case 0x13:
        cVar8 = 1;
        if (McaMorf_isStop(i_this->mpMorf)) {
            i_this->mMoveState = 0x15;
            timer(i_this, 2) = (s16)gabi::ftoi(cM_rndF(30.0f) + 20.0f);
            anm_init(i_this, BCK_WAIT3, 5.0f, EMode_LOOP, 1.0f, 0);
        }
        i_this->mVelocityFwdTarget = 0.0f;
        a_this->speedF = 0.0f;
        break;

    case 0x17: {
        cVar8 = 1;
        s32 frame = McaMorf_getFrame(i_this->mpMorf);
        if ((frame >= hio().m06) && (frame <= hio().m08)) {
            f32 v = 5.0f * i_this->mScale;
            i_this->mVelocityFwdTarget = v;
            i_this->mRotVel = 4000.0f;
            a_this->speedF = v;
            i_this->mRotVelFade = 1.0f;
        } else {
            i_this->mVelocityFwdTarget = 0.0f;
            i_this->mRotVel = 0.0f;
            a_this->speedF = 0.0f;
            if (dist2(i_this) < 85.0f || (i_this->mAcch.m_flags & 0x10) || dCcD_GObjInf_ChkCoHit(&i_this->mSph)) {
                anm_init(i_this, BCK_EAT1, 5.0f, EMode_NONE, 1.0f, 0);
                i_this->mMoveState = 0x18;
            }
        }
        break;
    }

    case 0x18:
        cVar8 = 1;
        i_this->mVelocityFwdTarget = 0.0f;
        a_this->speedF = 0.0f;
        if (McaMorf_isStop(i_this->mpMorf)) {
            if (cM_rndF(1.0f) < 0.2f) {
                i_this->mMoveState = 0x13;
                anm_init(i_this, BCK_SING2, 5.0f, EMode_NONE, 1.0f, 0);
            } else {
                i_this->mMoveState = 0x15;
                timer(i_this, 2) = (s16)gabi::ftoi(cM_rndF(20.0f));
                anm_init(i_this, BCK_WAIT3, 5.0f, EMode_LOOP, 1.0f, 0);
            }
        }
        break;

    case 0x19:
        cVar8 = 1;
        a_this->speed.y = 0.0f;
        if (McaMorf_getFrame(i_this->mpMorf) < 5) {
            i_this->mVelocityFwdTarget = 0.0f;
            i_this->mVelocityFwdTargetMaxVel = 1.0f;
        } else {
            i_this->mMoveState = 0;
            i_this->mAnimState = 20;
            timer(i_this, 3) = (s16)gabi::ftoi(cM_rndF(100.0f) + 100.0f);
            i_this->mRiseTimer = REG0_S(4) + 10;
            if (i_this->mPathIdx != 0xff) {
                i_this->mbUsePathMovement = i_this->mPathIdx + 1;
                i_this->m308 = 1;
            } else {
                timer(i_this, 1) = 0x32;
                i_this->mRotVel = gabi::fmadds(REG0_F(4), 10.0f, 1000.0f);
                i_this->mVelocityFwdTarget = 25.0f;
                i_this->mVelocityFwdTargetMaxVel = 2.0f;
                a_this->speedF = 0.0f;
                MtxTrans(a_this->current.pos.x, a_this->current.pos.y, a_this->current.pos.z, 0);
                gabi::Local<cXyz> sp34;
                sp34->set(0.0f, 1000.0f, 2000.0f);
                cMtx_YrotM(calc_mtx(), a_this->current.angle.y);
                MtxPosition(sp34, &i_this->mTargetPos);
            }
        }
        break;
    }

    switch (cVar8) {
    case 0:
        kamome_pos_move(i_this);
        break;
    case 1:
        kamome_ground_pos_move(i_this);
        if (i_this->mMoveState != 0x19 &&
            (a_this->current.pos.y - gabi::load<f32>(gabi::ea(&i_this->mAcch) + 0x94) /* GetGroundH */ > 200.0f ||
             fopAcM_searchActorDistance(a_this, fopAcM_searchPlayer()) < hio().m10)) {
            take_off(i_this);
        }
        break;
    }

    void* moveP = &i_this->mStts; /* mStts.GetCCMoveP() */
    if (gabi::ea(moveP) != 0) {
        PSVECAdd(&a_this->current.pos, (cXyz*)moveP, &a_this->current.pos);
    }
    u32 sph = gabi::ea(&i_this->mSph) + 0x118; /* dCcD_Sph's cM3dGSph */
    cM3dGSph_SetC(gabi::at<void>(sph), &a_this->current.pos);
    cM3dGSph_SetR(gabi::at<void>(sph), REG_F(17, 18) + 40.0f);
    dComIfG_Ccsp_Set(&i_this->mSph);
}

/* inlined: kamome_imouto2_move (mType 5) */
static inline void kamome_imouto2_move(kamome_class* i_this) {
    switch (i_this->mMoveState) {
    case 0:
        i_this->m2AB = 1;
        if (timer(i_this, 1) == 0) {
            anm_init(i_this, BCK_EAT1, 5.0f, EMode_NONE, 1.0f, 0);
            i_this->mMoveState++;
        }
        break;
    case 1:
        if (McaMorf_isStop(i_this->mpMorf)) {
            anm_init(i_this, BCK_WAIT3, 5.0f, EMode_LOOP, 1.0f, 0);
            timer(i_this, 1) = (s16)gabi::ftoi(cM_rndF(50.0f) + 50.0f);
            i_this->mMoveState--;
        }
        break;
    }
}

/* 021861D4 */
static BOOL daKamome_Execute(kamome_class* i_this) {
    WWHD_FUNC(0x021861D4, BOOL, i_this);
    dComIfGp_get(); /* HD: unused player lookup */
    /* HD: based on the USA version (mbNoDraw reset here) */
    i_this->mGlobalTimer++;
    i_this->mbNoDraw = 0;

    if (i_this->mType == 6) {
        heisou_control(i_this);
        return TRUE;
    }

    if (i_this->mSwitchNo != 0) {
        if (dComIfGs_isSwitch(i_this->mSwitchNo - 1, fopAcM_GetRoomNo(&i_this->actor))) {
            i_this->mSwitchNo = 0;
            f32 r = cM_rndF(0.5f);
            anm_init(i_this, BCK_WAIT2, 1.0f, EMode_LOOP, r + 1.0f, BAS_WAIT2);
            timer(i_this, 0) = (s16)gabi::ftoi(cM_rndF(60.0f) + 40.0f);
            i_this->mVelocityFwdTarget = 30.0f;
            i_this->actor.speedF = 30.0f;
        } else {
            return TRUE;
        }
    }

    if (hio().m05 == 0) {
        for (s32 i = 0; i < 6; i++) {
            if (timer(i_this, i) != 0) {
                timer(i_this, i)--;
            }
        }

        if (i_this->mbUsePathMovement != 0) {
            kamome_path_move(i_this);
        } else if (i_this->mType == 4) {
            kamome_imouto_move(i_this);
        } else if (i_this->mType == 5) {
            kamome_imouto2_move(i_this);
        } else if (i_this->mType == 7) {
            kamome_heisou_move(i_this);
        } else {
            kamome_auto_move(i_this);
        }

        McaMorf_play(i_this->mpMorf, &i_this->actor.eyePos, 0, 0);
        cLib_addCalcAngleS2(&i_this->mJointRotY, i_this->mJointRotYTarget, 4, 0x800);
        cLib_addCalcAngleS2(&i_this->mJointRotZ, i_this->mJointRotZTarget, 4, 0x800);

        if (i_this->m2AB != 0) {
            if (timer(i_this, 5) == 0) {
                timer(i_this, 5) = (s16)gabi::ftoi(cM_rndF(20.0f) + 10.0f);
                i_this->mJointRotYTarget = (s16)gabi::ftoi(cM_rndFX(REG0_F(8) + 3000.0f));
                i_this->mJointRotZTarget = (s16)gabi::ftoi(cM_rndFX(REG0_F(8) + 3000.0f));
            }
            i_this->m2AB = 0;
        } else {
            i_this->mJointRotZTarget = 0;
            i_this->mJointRotYTarget = 0;
        }
    }

    daKamome_setMtx(i_this);

    i_this->actor.eyePos.copy(i_this->actor.current.pos);
    return TRUE;
}
VERIFY(0x021861D4, daKamome_Execute);
