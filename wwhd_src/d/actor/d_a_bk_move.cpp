/**
 * d_a_bk_move.cpp (WWHD)
 * Enemy - Bokoblin: Bk_move, with the actions it inlines in WWHD (jyunkai, walk_set, stand,
 * stand2, path_run, search_target, daBk_wepon_view_check's call).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bk_local.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E1AA4 mDoAud_monsSeStart(id, pos, procId, param, reverb); fopAcM_monsSeStart is an HD
 * inline with the actor/eyePos null checks of fopAcM_seStart */
static inline void fopAcM_monsSeStart_mv(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        gabi::call(0x025E1AA4, id, &a->eyePos, pid, param, reverb);
    }
}
/* 02516C14 dCcMassS_Mng::Set(cCcD_Obj*, u8 priority) on play+PLAY_CCMASS */
static inline void dComIfG_Ccsp_SetMass(void* obj, u8 prio) {
    gabi::call(0x02516C14, gabi::at<u8>(dComIfGp_ea() + PLAY_CCMASS), obj, prio);
}
/* 0209AB64 (matcher: search_wepon) is daBk_wepon_view_check with search_wepon inlined (d_a_bk.cpp) */
#define daBk_wepon_view_check_mv daBk_wepon_view_check
/* 020A47A4 (unnamed) is carry(): speed.y = 0 */
static inline void carry_mv(bk_class* i_this) { gabi::call(0x020A47A4, i_this); }
/* REGn_F / REGn_S */
#define REG8_F(i) REG_F(8, i)
#define REG8_S(i) REG_S(8, i)
/* daObj_Search::Act_c: setChildId (+0x96C), setBkControl (+0x780) */
static inline void daObj_Search_setChildId(daObj_Search_Act_c* s, u32 id) { gabi::store<u32>(gabi::ea(s) + 0x96C, id); }
static inline void daObj_Search_setBkControl(daObj_Search_Act_c* s, bool v) { gabi::store<u8>(gabi::ea(s) + 0x780, v); }
static inline fopAc_ac_c* search_ac(daObj_Search_Act_c* s) { return (fopAc_ac_c*)(void*)s; }

static inline void set_ken(fopAc_ac_c* k) { gabi::store<u32>(0x1046231C, gabi::ea(k)); } /* static ken */

/* dPath / dPnt (unchanged from GameCube); points are addressed from the path pointer with plain
 * address arithmetic (after a failed JUT_ASSERT the original reads through a null path) */
struct dPnt_mv {
    /* 0x0 */ u8 _0[3];
    /* 0x3 */ be<u8> mArg3;
    /* 0x4 */ cXyz m_position;
};
static inline u32 path_ea(bk_class* i_this) { return gabi::load<u32>(gabi::ea(&i_this->ppd)); }
static inline u16 path_num(bk_class* i_this) { return gabi::load<u16>(path_ea(i_this) + 0); }
static inline u16 path_nextID(bk_class* i_this) { return gabi::load<u16>(path_ea(i_this) + 2); }
static inline bool path_closed(bk_class* i_this) { return (gabi::load<u8>(path_ea(i_this) + 5) & 1) != 0; }
static inline dPnt_mv* path_point(bk_class* i_this, s8 idx) {
    u32 pts = gabi::load<u32>(path_ea(i_this) + 8);
    return gabi::at<dPnt_mv>(pts + (s32)idx * 0x10);
}

/* lfs/stfs copies: the recompiled code moves the bits unchanged (no SNaN quieting) */
static inline void fcopy(be<f32>& dst, const be<f32>& src) { gabi::store<u32>(dst.addr(), gabi::load<u32>(src.addr())); }
static inline void copy_f(cXyz* dst, const cXyz* src) { fcopy(dst->x, src->x); fcopy(dst->y, src->y); fcopy(dst->z, src->z); }

enum {
    JA_SE_CV_BK_SEARCH = 0x482A,
    JA_SE_CV_BK_FOUND_LINK = 0x482B,
    JA_SE_CV_BK_LOST_BOKO = 0x482C,
    JA_SE_CV_BK_SEARCH_BOKO = 0x482D,
    JA_SE_CV_BK_JUMP = 0x482F,
    JA_SE_CV_BK_NOBI = 0x4831,
};
enum { fopAcStts_UNK4000_e = 0x4000 };

static inline int morf_frame(bk_class* i_this) { return gabi::ftoi(i_this->mpMorf->getFrame()); }

/* GameCube walk_set (inlined in jyunkai) */
static inline void walk_set(bk_class* i_this) {
    if (i_this->m0B30 != 0 || i_this->m1214 != 0) {
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_WALK_e, 10.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_WALK_e);
    } else if (i_this->m11F3 == 0) {
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_NIGERU_e, 5.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_NIGERU_e);
    } else {
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_WALK_e, 10.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_WALK_e);
    }
    if (i_this->m0B30 == 0 && i_this->m121F != 0) {
        fopAcM_monsSeStart_mv(i_this, JA_SE_CV_BK_LOST_BOKO, 0);
        i_this->m121F = 0;
    }
}

/* GameCube jyunkai (inlined) */
static inline void jyunkai(bk_class* i_this) {
    fopAc_ac_c* i_actor = i_this;
    dComIfGp_get(); /* HD: unused accessor call */

    if (i_this->mType == 4 || i_this->mType == 0xA) {
        i_this->dr.mAction = 1;
        i_this->dr.mMode = 50;
        return;
    } else if (i_this->mType == 6) {
        i_this->dr.mAction = 2;
        i_this->dr.mMode = 50;
        return;
    } else if (i_this->mType == 9) {
        i_this->dr.mAction = 3;
        i_this->dr.mMode = 0;
        i_this->m0300[2] = 0;
        i_this->m120C = 0;
        i_this->m1210 = 0;
        path_check(i_this, 0);
        return;
    }

    switch ((s16)i_this->dr.mMode) {
    case 0:
        walk_set(i_this);
        // Fall-through
    case -1:
        i_this->dr.mMode = 1;
        if (i_this->m0B30 != 0 || i_this->m11F3 != 0) {
            if (i_this->m1215 != 0) {
                s8 idx = i_this->m1216 + i_this->m1217;
                i_this->m1216 = idx;
                if (idx >= (s8)path_num(i_this)) {
                    if (path_closed(i_this)) {
                        i_this->m1216 = 0;
                    } else {
                        i_this->m1217 = -1;
                        i_this->m1216 = (s8)(path_num(i_this) - 2);
                    }
                    if (path_nextID(i_this) != 0xFFFF) {
                        i_this->ppd = dPath_GetRoomPath(path_nextID(i_this), fopAcM_GetRoomNo(i_this));
                        if (i_this->ppd == nullptr) {
                            JUT_ASSERT_fail(STR(0x1000958C), 0xB95, STR(0x10009598));
                        }
                    }
                } else if (idx < 0) {
                    i_this->m1217 = 1;
                    i_this->m1216 = 1;
                }
                dPnt_mv* point = path_point(i_this, i_this->m1216);
                copy_f(&i_this->m0320, &point->m_position);
            } else {
                way_pos_check(i_this, &i_this->m0320);
            }
        } else {
            way_pos_check(i_this, &i_this->m0320);
            i_this->m0300[1] = (s16)gabi::ftoi(30.0f + cM_rndF(25.0f));
        }
        i_this->m0300[2] = 30;
        // Fall-through
    case 1: {
        s16 r29;
        f32 f31;
        if (i_this->m0B30 != 0 || i_this->m11F3 != 0) {
            r29 = 0x400;
            f31 = l_bkHIO().m04C;
        } else {
            r29 = 0x1000;
            f31 = l_bkHIO().m050;
        }

        f32 x = i_this->m0320.x - i_this->current.pos.x;
        f32 z = i_this->m0320.z - i_this->current.pos.z;
        i_this->dr.m4D0 = cM_atan2s(x, z);

        if (i_this->m1215 != 0 && (i_this->m0B30 != 0 || i_this->m11F3 != 0)) {
            f32 d = std_sqrtf(gabi::fmadds(x, x, z * z));
            f32 t = f31 * 0.25f;
            if (d < t + t) {
                if (path_point(i_this, i_this->m1216)->mArg3 == 3) {
                    wait_set(i_this);
                    i_this->dr.mMode = 2;
                } else {
                    i_this->dr.mMode = -1;
                }
            }
        } else {
            f32 d = std_sqrtf(gabi::fmadds(x, x, z * z));
            f32 t = f31 * 0.25f;
            if (d < t + t ||
                (i_this->m0300[2] == 0 &&
                 (i_this->dr.mAcch.ChkWallHit() || ground_4_check(i_this, 1, i_this->current.angle.y, 200.0f)))) {
                wait_set(i_this);
                i_this->dr.mMode = 2;
            }
        }
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.m4D0, 4, r29);
        cLib_addCalc2(&i_this->speedF, f31, 1.0f, 5.0f);
        break;
    }
    case 2:
        i_this->speedF = 0.0f;
        if (i_this->m0B30 == 0 && i_this->m11F3 == 0) {
            int frame = morf_frame(i_this);
            if ((frame == 0xB || frame == 0x19) && cM_rndF(1.0f) < 0.5f) {
                fopAcM_monsSeStart_mv(i_this, JA_SE_CV_BK_SEARCH, 0);
            }
        }
        if (i_this->m0300[1] == 0) {
            i_this->dr.mMode = 0;
            if (i_this->m0B30 == 0 && i_this->m11F3 == 0 && i_this->m030A == 0) {
                i_this->m11F3 = 1;
            }
        }
        break;
    case 3: {
        i_this->speedF = 0.0f;
        int frame = morf_frame(i_this);
        if (frame == 3 || frame == 0x35) {
            fopAcM_monsSeStart_mv(i_this, JA_SE_CV_BK_SEARCH, 0);
        }
        if (i_this->m0300[1] == 0) {
            i_this->dr.mMode = 0;
        }
        break;
    }
    }

    s32 r3 = fopAcM_otoCheck(i_actor, 1000.0f);
    r3 += search_sp();

    if (i_this->m0B30 != 0 || i_this->m11F3 != 0) {
        if (r3 != 0 ||
            (i_this->mPlayerDistance < l_bkHIO().m028 &&
             daBk_player_view_check(i_this, &i_this->dr.m714->current.pos, i_this->m0332, l_bkHIO().m034))) {
            i_this->dr.mAction = 4;
            i_this->m0300[1] = 0;
            i_this->dr.mMode = 0;
        }
    } else if (i_this->m11F3 == 0 && i_this->dr.mMode == 2) {
        int frame = morf_frame(i_this);
        if ((frame == 0xB || frame == 0x19) && cM_rndF(1.0f) < 0.5f) {
            fopAcM_monsSeStart_mv(i_this, JA_SE_CV_BK_SEARCH_BOKO, 0);
        }
    }

    /* HD: daBk_wepon_view_check is an out-of-line function (0209AB64) */
    if (i_this->m0B30 == 0 && daBk_wepon_view_check_mv(i_this)) {
        i_this->dr.mAction = 12;
        i_this->dr.mMode = -1;
    }

    if (daBk_bomb_view_check(i_this)) {
        i_this->dr.mAction = 9;
        i_this->dr.mMode = 0;
    }
}

/* common tail of stand / stand2: player sighting and bombs */
static inline void stand_sight(bk_class* i_this) {
    f32 f1;
    if (i_this->m02B5 != 0xFF) {
        f1 = (f32)(u32)i_this->m02B5 * 10.0f;
    } else {
        f1 = 500.0f;
    }

    if (i_this->mPlayerDistance < f1 &&
        daBk_player_view_check(i_this, &i_this->dr.m714->current.pos, i_this->m0332, l_bkHIO().m034)) {
        if (i_this->dr.mMode >= 50) {
            i_this->dr.mAction = 4;
            i_this->m0300[1] = 0;
            i_this->dr.mMode = 0;
        } else {
            i_this->dr.mMode = 20;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_BIKKURI_e, 3.0f, 0, 1.0f, -1);
            i_this->m0300[1] = 30;
            fopAcM_monsSeStart_mv(i_this, JA_SE_CV_BK_FOUND_LINK, 0);
        }
    }

    if (daBk_bomb_view_check(i_this)) {
        i_this->dr.mAction = 9;
        i_this->dr.mMode = 0;
    }
}

/* GameCube stand (inlined) */
static inline void stand(bk_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    i_this->dr.m710 = 0;
    bool r28 = false;
    bool r27 = false;

    s32 r31 = fopAcM_otoCheck(i_this, 1000.0f);

    switch ((s16)i_this->dr.mMode) {
    case -20:
        i_this->actor_status |= fopAcStts_UNK4000_e;
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_WAIT_e, 10.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_WAIT_e);
        i_this->dr.mMode = -19;
        i_this->m0300[1] = 20;
        i_this->m02B5 = 0;
        // Fall-through
    case -19:
        if (i_this->m0300[1] == 0) {
            fopAc_ac_c* ken = (fopAc_ac_c*)fpcM_Search(0x0209B67C /* ken_s_sub */, i_this);
            set_ken(ken);
            if (ken) {
                gabi::Local<cXyz> tmp;
                gabi::Local<cXyz> sp28;
                cXyz_mi(&player->current.pos, tmp, &ken->current.pos);
                sp28->copy(*tmp);
                if (std_sqrtf(PSVECSquareMag(sp28)) < 800.0f) {
                    i_this->m1234 = 1;
                    i_this->dr.mMode = -18;
                }
            } else {
                i_this->dr.mMode = 1;
                break;
            }
        }
        r27 = true;
        break;
    case 0: {
        f32 r = cM_rndF((f32)(s32)(l_bkHIO().m106 - l_bkHIO().m104));
        i_this->m0300[1] = (s16)gabi::ftoi(r + (f32)(s32)l_bkHIO().m104);
    }
        // Fall-through
    case -1:
        i_this->dr.mMode = 1;
        if (i_this->m02DC != 0) {
            if (cM_rndF(1.0f) < 0.5f) {
                anm_init(i_this, dRes_INDEX_BK_BCK_BK_NOZOKU_e, 10.0f, 2, 1.0f, -1);
                i_this->m0300[1] = (s16)gabi::ftoi(cM_rndF(200.0f) + 200.0f);
            } else {
                anm_init(i_this, dRes_INDEX_BK_BCK_BK_TATAKU_e, 10.0f, 2, 1.0f, -1);
                i_this->m0300[1] = (s16)gabi::ftoi(cM_rndF(100.0f) + 100.0f);
            }
        } else {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_WAIT_e, 10.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_WAIT_e);
        }
        i_this->m121E++;
        // Fall-through
    case 1:
        cLib_addCalc2(&i_this->current.pos.x, i_this->home.pos.x, 0.5f, i_this->speed.x * 0.25f);
        cLib_addCalc2(&i_this->current.pos.z, i_this->home.pos.z, 0.5f, i_this->speed.z * 0.25f);
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->home.angle.y, 2, 0x800);
        i_this->speedF = 0.0f;
        if (i_this->m0300[1] == 0) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_NOBI_e, 10.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_NOBI_e);
            fopAcM_monsSeStart_mv(i_this, JA_SE_CV_BK_NOBI, 0);
            if (i_this->m121E < 3) {
                i_this->dr.mMode = 2;
            } else {
                i_this->dr.mMode = 3;
                i_this->m121E = 0;
            }
        }
        break;
    case 2:
        r28 = true;
        i_this->m02CA = 2;
        if (i_this->mpMorf->isStop()) {
            i_this->dr.mMode = 0;
        }
        break;
    case 3:
        r28 = true;
        i_this->m02CA = 2;
        if (i_this->mpMorf->isStop()) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_SLEEP_e, 10.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_SLEEP_e);
            f32 r = cM_rndF((f32)(s32)(l_bkHIO().m10A - l_bkHIO().m108));
            i_this->m0300[1] = (s16)gabi::ftoi(r + (f32)(s32)l_bkHIO().m108);
            i_this->dr.mMode += 1;
        }
        break;
    case 4:
        r28 = true;
        i_this->m02CA = 2;
        if (i_this->m0300[1] == 0) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_NOBI_e, 10.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_NOBI_e);
            fopAcM_monsSeStart_mv(i_this, JA_SE_CV_BK_NOBI, 0);
            i_this->dr.mMode = 2;
        }
        break;
    case 10:
        if (i_this->m0300[1] == 30) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_BIKKURI_e, 3.0f, 0, 1.0f, -1);
        }
        if (i_this->m0300[1] == 0) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_KYORO2_e, 10.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_KYORO2_e);
            i_this->dr.mMode = 11;
            i_this->m0300[3] = (s16)gabi::ftoi(30.0f + cM_rndF(30.0f));
        }
        break;
    case 11:
        if (i_this->m0300[3] == 0) {
            i_this->dr.mMode = 0;
        }
        break;
    case 20:
        if (i_this->m0300[1] < 10) {
            i_this->dr.m710 = 1;
        }
        if (i_this->m0300[1] == 0) {
            i_this->dr.mAction = 4;
            i_this->m0300[1] = 0;
            i_this->dr.mMode = 0;
            return;
        }
        break;
    case 50:
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_KYORO1_e, 10.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_KYORO1_e);
        i_this->dr.mMode += 1;
        i_this->m0300[1] = 50;
        // Fall-through
    case 51:
        i_this->speedF = 0.0f;
        if (i_this->m0300[1] > 25) {
            r28 = true;
        }
        if (i_this->m0300[1] == 0) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_RUN_e, 10.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_RUN_e);
            i_this->dr.mMode += 1;
            i_this->m0300[2] = 60;
        }
        break;
    case 52: {
        gabi::Local<cXyz> tmp;
        gabi::Local<cXyz> sp28;
        cXyz_mi(&i_this->home.pos, tmp, &i_this->current.pos);
        copy_f(sp28, tmp);
        i_this->dr.m4D0 = cM_atan2s(sp28->x, sp28->z);
        f32 d = std_sqrtf(gabi::fmadds(sp28->x, sp28->x, sp28->z * sp28->z));
        if (d < l_bkHIO().m054 * 0.25f * 5.0f) {
            i_this->dr.mMode = 0;
        }
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.m4D0, 4, 0x1000);
        cLib_addCalc2(&i_this->speedF, l_bkHIO().m054, 1.0f, 5.0f);

        if (i_this->dr.mAcch.ChkGroundHit() && i_this->dr.mAcch.ChkWallHit()) {
            if (i_this->m02DC != 0) {
                i_this->dr.mMode = 60;
                i_this->m0300[2] = 20;
                break;
            }

            i_this->speed.y = REG0_F(16) + 100.0f;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP1_e, 2.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_JUMP1_e);
            fopAcM_monsSeStart_mv(i_this, JA_SE_CV_BK_JUMP, 0);
            i_this->dr.mMode = 53;
        }
        break;
    }
    case 53:
        i_this->speedF = l_bkHIO().m054 * 0.5f;
        if (!i_this->dr.mAcch.ChkGroundHit()) {
            break;
        }
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP2_e, 0.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_JUMP2_e);
        i_this->dr.mMode += 1;
        break;
    case 54:
        i_this->speedF = 0.0f;
        if (i_this->mpMorf->isStop()) {
            i_this->dr.mMode = 51;
        }
        break;
    case 60:
        cLib_addCalcAngleS2(&i_this->current.angle.y, (s16)(i_this->dr.m4D0 + 0x4000), 4, 0x1000);
        cLib_addCalc2(&i_this->speedF, l_bkHIO().m054, 1.0f, 5.0f);
        if (i_this->m0300[2] == 0) {
            i_this->dr.mMode = 52;
        }
        break;
    }

    if (r27) {
        return;
    }
    if (i_this->dr.mMode < 10 && r31 != 0) {
        i_this->dr.mMode = 10;
        i_this->m0300[1] = (s16)gabi::ftoi(cM_rndF(10.0f) + 45.0f);
    }
    if (!r28 && i_this->m0300[2] == 0 && i_this->dr.mMode != 20) {
        stand_sight(i_this);
    }
    if (i_this->m0B30 == 0 && daBk_wepon_view_check_mv(i_this)) {
        i_this->dr.mAction = 12;
        i_this->dr.mMode = -1;
    }
}

/* GameCube stand2 (inlined): beside a search light */
static inline void stand2(bk_class* i_this) {
    dComIfGp_get(); /* HD: unused accessor call */
    i_this->dr.m710 = 0;
    s32 r31 = fopAcM_otoCheck(i_this, 1000.0f);

    if (i_this->mpSearchLight == nullptr) {
        daObj_Search_Act_c* light = (daObj_Search_Act_c*)fpcM_Search(0x0209B6D8 /* s_s_sub */, i_this);
        i_this->mpSearchLight = light;
        if (light != nullptr) {
            daObj_Search_setChildId(light, fopAcM_GetID(i_this));
        } else {
            return;
        }
    }

    if (i_this->mpSearchLight != nullptr) {
        cMtx_YrotS(calc_mtx(), search_ac(i_this->mpSearchLight)->current.angle.y);
        gabi::Local<cXyz> sp24;
        sp24->x = REG8_F(11) + 320.0f;
        sp24->y = REG8_F(12) + 114.0f;
        sp24->z = REG8_F(13) + -55.0f;
        MtxPosition(sp24, &i_this->home.pos);
        PSVECAdd(&i_this->home.pos, &search_ac(i_this->mpSearchLight)->current.pos, &i_this->home.pos);
        i_this->home.angle.y = search_ac(i_this->mpSearchLight)->current.angle.y + REG8_S(4);
    }

    gabi::Local<cXyz> tmp;
    gabi::Local<cXyz> sp24;
    switch ((s16)i_this->dr.mMode) {
    case 0:
        i_this->dr.mMode = 1;
        // Fall-through
    case 1: {
        if (i_this->mpSearchLight != nullptr) {
            daObj_Search_setBkControl(i_this->mpSearchLight, true);
        }
        i_this->dr.m710 = 6;
        if (i_this->m0300[3] == 0) {
            i_this->m0300[3] = (s16)gabi::ftoi(cM_rndF(150.0f) + 80.0f);
            i_this->m1212 = (s16)gabi::ftoi(cM_rndF(30.0f));
        } else if (i_this->m0300[3] < (s16)(i_this->m1212 + 30)) {
            i_this->dr.m71A = 10000;
            i_this->dr.m718 = -10000;
        } else if (i_this->m0300[4] == 0) {
            i_this->m0300[4] = (s16)gabi::ftoi(cM_rndF(30.0f) + 10.0f);
            i_this->dr.m71A = (s16)gabi::ftoi(3000.0f - cM_rndF(10000.0f));
            i_this->dr.m718 = (s16)gabi::ftoi(-cM_rndF(2000.0f));
        }

        i_this->m0B7B = 1;
        s16 r28 = i_this->m1224 - i_this->m1228;
        if (i_this->m122A >= 0 && r28 < 0) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_MIGIROT_e, 5.0f, 2, 1.0f, -1);
        }
        if (i_this->m122A <= 0 && r28 > 0) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_HIDARIROT_e, 5.0f, 2, 1.0f, -1);
        }
        i_this->m122A = r28;
        i_this->m1228 = i_this->m1224;

        cLib_addCalc2(&i_this->current.pos.x, i_this->home.pos.x, 0.5f, gabi::fmadds(i_this->speed.x, 0.25f, 10.0f));
        cLib_addCalc2(&i_this->current.pos.z, i_this->home.pos.z, 0.5f, gabi::fmadds(i_this->speed.z, 0.25f, 10.0f));
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->home.angle.y, 2, 0x800);
        i_this->speedF = 0.0f;
        break;
    }
    case 10:
        i_this->m0B7B = 1;
        if (i_this->m0300[1] == 30) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_BIKKURI_e, 3.0f, 0, 1.0f, -1);
        }
        if (i_this->m0300[1] == 0) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_KYORO2_e, 10.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_KYORO2_e);
            i_this->dr.mMode = 11;
            i_this->m0300[3] = (s16)gabi::ftoi(30.0f + cM_rndF(30.0f));
        }
        break;
    case 11:
        if (i_this->m0300[3] == 0) {
            i_this->dr.mMode = 0;
        }
        break;
    case 20:
        i_this->m0B7B = 1;
        if (i_this->m0300[1] < 10) {
            i_this->dr.m710 = 1;
        }
        if (i_this->m0300[1] == 0) {
            i_this->dr.mAction = 4;
            i_this->m0300[1] = 0;
            i_this->dr.mMode = 0;
            return;
        }
        break;
    case 50:
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_KYORO1_e, 10.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_KYORO1_e);
        i_this->dr.mMode += 1;
        i_this->m0300[1] = 50;
        // Fall-through
    case 51:
        i_this->speedF = 0.0f;
        if (i_this->m0300[1] == 0) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_RUN_e, 10.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_RUN_e);
            i_this->dr.mMode += 1;
            path_check(i_this, 1);
            if (i_this->m1216 >= 4) {
                i_this->m1217 = 1;
            } else {
                i_this->m1217 = -1;
            }
            i_this->m0300[2] = 60;
        }
        break;
    case 52: {
        dPnt_mv* pnt = path_point(i_this, i_this->m1216);
        copy_f(&i_this->m0320, &pnt->m_position);
        cXyz_mi(&i_this->m0320, tmp, &i_this->current.pos);
        copy_f(sp24, tmp);
        goto temp_568;
    }
    case 60:
        cXyz_mi(&i_this->home.pos, tmp, &i_this->current.pos);
        copy_f(sp24, tmp);
    temp_568: {
        i_this->dr.m4D0 = cM_atan2s(sp24->x, sp24->z);
        f32 d = std_sqrtf(gabi::fmadds(sp24->x, sp24->x, sp24->z * sp24->z));
        if (d < l_bkHIO().m054 * 0.25f * 5.0f) {
            if (i_this->dr.mMode == 60) {
                i_this->dr.mMode = 0;
            } else if (i_this->m1216 == 0) {
                i_this->dr.mMode = 60;
            } else {
                s8 idx = i_this->m1216 + i_this->m1217;
                i_this->m1216 = idx;
                u16 num = path_num(i_this);
                if (idx >= (s8)num) {
                    i_this->m1216 = 0;
                } else if (idx < 0) {
                    i_this->m1216 = (s8)(num - 1);
                }
            }
        }
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.m4D0, 4, 0x1000);
        cLib_addCalc2(&i_this->speedF, l_bkHIO().m054, 1.0f, 5.0f);
        break;
    }
    }

    if (i_this->dr.mMode < 10 && r31 != 0) {
        i_this->dr.mMode = 10;
        i_this->m0300[1] = (s16)gabi::ftoi(cM_rndF(10.0f) + 45.0f);
    }

    if (i_this->m0300[2] == 0 && i_this->dr.mMode != 20) {
        stand_sight(i_this);
    }

    if (i_this->m0B30 == 0 && daBk_wepon_view_check_mv(i_this)) {
        i_this->dr.mAction = 12;
        i_this->dr.mMode = -1;
    }
}

/* GameCube path_run (inlined) */
static inline void path_run(bk_class* i_this) {
    dComIfGp_get(); /* HD: unused accessor call */
    i_this->dr.m710 = 0;

    switch ((s16)i_this->dr.mMode) {
    case 0:
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_RUN_e, 10.0f, 2, l_bkHIO().m070, dRes_INDEX_BK_BAS_BK_RUN_e);
        i_this->dr.mMode = 1;
        // Fall-through
    case 1: {
        dPnt_mv* point = path_point(i_this, i_this->m1216);
        f32 x = point->m_position.x + i_this->m0320.x;
        f32 z = point->m_position.z + i_this->m0320.z;
        f32 dx = x - i_this->current.pos.x;
        f32 dz = z - i_this->current.pos.z;
        i_this->dr.m4D0 = cM_atan2s(dx, dz);

        if (std_sqrtf(gabi::fmadds(dx, dx, dz * dz)) < 100.0f) {
            s8 idx = i_this->m1216 + i_this->m1217;
            i_this->m1216 = idx;
            u16 num = path_num(i_this);
            if (idx >= (s8)num) {
                i_this->m1216 = (s8)(num - 1);
                i_this->m1217 = -1;
                i_this->m121C = 1;
            } else if (idx < 0) {
                i_this->m1216 = 0;
                i_this->m1217 = 1;
                i_this->m121C = 1;
            }
        }

        if (i_this->m0300[1] == 0) {
            i_this->m0300[1] = (s16)gabi::ftoi(cM_rndF(50.0f) + 30.0f);
            i_this->m0320.x = cM_rndFX(50.0f);
            i_this->m0320.z = cM_rndFX(50.0f);
        }

        if (i_this->dr.mAcch.ChkGroundHit()) {
            s16 maxStep = 0x600;
            if (i_this->m120C != 0) {
                maxStep = 0x2000;
            }
            cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.m4D0, 4, maxStep);
        }

        if (i_this->m120C == 0) {
            i_this->speedF = l_bkHIO().m054;
            if (i_this->m0300[2] == 0) {
                i_this->m0300[2] = (s16)gabi::ftoi(cM_rndF(100.0f) + 50.0f);
                anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP1_e, 2.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_JUMP1_e);
                i_this->m120C = 1;
                i_this->m1210 = 0;
            }
        } else {
            switch (i_this->m1210) {
            case 0:
                i_this->speedF = l_bkHIO().m054 * 1.2f;
                if (i_this->dr.mAcch.ChkGroundHit() && i_this->mpMorf->isStop()) {
                    anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP2_e, 0.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_JUMP2_e);
                    i_this->m1210 = 1;
                    i_this->m034C = l_bkHIO().m00C + 2;
                    i_this->m034E = 4;
                }
                break;
            case 1:
                i_this->speedF = 0.0f;
                if (i_this->dr.mAcch.ChkGroundHit() && i_this->mpMorf->isStop()) {
                    i_this->m1210 = 0;
                    anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP1_e, 2.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_JUMP1_e);
                    f32 r = cM_rndF(REG8_F(7) + 10.0f);
                    i_this->speed.y = r + 65.0f + REG8_F(8);
                    fopAcM_monsSeStart_mv(i_this, JA_SE_CV_BK_JUMP, 0);
                }

                if (i_this->m0300[2] == 0) {
                    i_this->m0300[2] = (s16)gabi::ftoi(cM_rndF(100.0f) + 50.0f);
                    anm_init(i_this, dRes_INDEX_BK_BCK_BK_RUN_e, 10.0f, 2, l_bkHIO().m070, dRes_INDEX_BK_BAS_BK_RUN_e);
                    i_this->m120C = 0;
                }
                break;
            }
        }
        break;
    }
    }

    if (i_this->mPlayerDistance < l_bkHIO().m028 &&
        daBk_player_view_check(i_this, &i_this->dr.m714->current.pos, i_this->m0332, l_bkHIO().m034)) {
        i_this->dr.mAction = 4;
        i_this->m0300[1] = 0;
        i_this->dr.mMode = 0;
    }
}

/* GameCube search_target (inlined) */
static inline BOOL search_target(bk_class* i_this) {
    i_this->dr.m714 = dComIfGp_getPlayer(0);
    return FALSE;
}

/* 020A5EF4 */
void Bk_move(bk_class* i_this) {
    WWHD_FUNC(0x020A5EF4, void, i_this);
    fopAc_ac_c* actor = i_this;

    dComIfGp_get(); /* HD: unused accessor call */
    i_this->m0334 = 0;
    i_this->dr.m710 = 0;
    i_this->dr.m711 = 0;
    cLib_addCalcAngleS2(&i_this->m11F4, 0, 2, 0x800);

    if (i_this->dr.mMode <= -100) {
        i_this->m030E = 5;
        if (std::fabs((f32)i_this->dr.m478) > 40.0f && !i_this->dr.mAcch.ChkGroundHit()) {
            i_this->m1040.SetC(&i_this->dr.m100[12]);
            i_this->m1040.SetR(40.0f);
            i_this->m1040.OffAtSPrmBit(4); /* OffAtVsPlayerBit */
            i_this->m1040.SetAtSpl(1);     /* dCcG_At_Spl_UNK1 */
            dComIfG_Ccsp_Set(&i_this->m1040);
            dComIfG_Ccsp_SetMass(&i_this->m1040, 3);

            if (i_this->m1040.ChkAtHit() && actor->speed.y < -50.0f) {
                actor->speed.y = 0.0f;
                i_this->dr.m474 = 8000.0f;
            }
            i_this->m0B88.OffCoSPrmBit(1); /* OffCoSetBit */
        }
        return;
    }

    i_this->m1040.OnAtSPrmBit(4); /* OnAtVsPlayerBit */
    if (i_this->m02DE == 0) {
        i_this->m0B88.OnCoSPrmBit(1);
    } else {
        i_this->m0B88.OffCoSPrmBit(1);
    }

    if (i_this->dr.m48A != 0 && i_this->dr.m488 == 0) {
        if (i_this->dr.m48A == 1) {
            i_this->dr.mAction = 4;
            i_this->dr.mMode = 0;
            i_this->m0300[1] = 0;
        } else {
            i_this->dr.m4D0 = i_this->m0332;
            actor->speedF = -30.0f;
            i_this->m034C = l_bkHIO().m00C + 3;
            i_this->m034E = 4;
        }
    } else {
        search_target(i_this);

        f32 x = i_this->dr.m714->current.pos.x - actor->current.pos.x;
        f32 z = i_this->dr.m714->current.pos.z - actor->current.pos.z;
        i_this->mPlayerDistance = std_sqrtf(gabi::fmadds(x, x, z * z));

        i_this->m0332 = cM_atan2s(x, z);

        if (i_this->m122C != 0) {
            i_this->m122C = i_this->m122C - 1;
            if (i_this->m122C == 0) {
                /* HD: also clears dr.m468/m46C and current.angle.x/z */
                i_this->dr.m46C = 0.0f;
                i_this->dr.m468 = 0.0f;
                i_this->dr.mAction = 18;
                i_this->dr.mMode = 0;
                i_this->current.angle.z = 0;
                i_this->current.angle.x = 0;
            }
        }

        switch ((s16)i_this->dr.mAction) {
        case 0: jyunkai(i_this); break;
        case 1: stand(i_this); break;
        case 2: stand2(i_this); break;
        case 3: path_run(i_this); break;
        case 4: fight_run(i_this); break;
        case 5: fight(i_this); break;
        case 10: defence(i_this); break;
        case 7: oshi(i_this); break;
        case 8: p_lost(i_this); break;
        case 9: b_nige(i_this); break;
        case 11: hukki(i_this); break;
        case 12: wepon_search(i_this); break;
        case 14: aite_miru(i_this); break;
        case 20: fail(i_this); break;
        case 21: yogan_fail(i_this); break;
        case 22: water_fail(i_this); break;
        case 15: tubo_wait(i_this); break;
        case 19: b_hang(i_this); break;
        case 16: rope_on(i_this); break;
        case 23: d_dozou(i_this); break;
        case 30: carry_mv(i_this); break;
        case 31: carry_drop(i_this); break;
        case 18: d_mahi(i_this); break;
        case 29: z_demo_1(i_this); break;
        }
    }

    if ((i_this->mType == 4 || i_this->mType == 10 || i_this->mType == 6) && i_this->dr.mAction == 4) {
        gabi::Local<cXyz> tmp;
        gabi::Local<cXyz> sp28;
        cXyz_mi(&actor->home.pos, tmp, &actor->current.pos);
        sp28->copy(*tmp);

        f32 f31;
        if (i_this->m02B5 != 0xFF) {
            f31 = (f32)(u32)i_this->m02B5 * 10.0f * 1.5f;
        } else {
            f31 = 750.0f;
        }

        if (std_sqrtf(PSVECSquareMag(sp28)) > f31) {
            if (i_this->mType == 4 || i_this->mType == 10) {
                i_this->dr.mAction = 1;
            }
            if (i_this->mType == 6) {
                i_this->dr.mAction = 2;
            }
            i_this->dr.mMode = 51;
            i_this->m0300[1] = 0;
            i_this->m0300[2] = 60;
        }
    }

    if (i_this->dr.mAction != 30) {
        gabi::Local<cXyz> sp28;
        gabi::Local<cXyz> sp1C;
        sp28->x = 0.0f;
        sp28->y = 0.0f;
        fcopy(sp28->z, actor->speedF);
        if (i_this->dr.mAction != 11 && i_this->dr.mAction != 20 && i_this->dr.m48A == 0) {
            i_this->dr.m482 = actor->current.angle.y;
            cMtx_YrotS(calc_mtx(), (s16)(actor->current.angle.y + i_this->m0334));
        } else {
            cMtx_YrotS(calc_mtx(), i_this->dr.m4D0);
        }
        MtxPosition(sp28, sp1C);
        fcopy(actor->speed.x, sp1C->x);
        fcopy(actor->speed.z, sp1C->z);
    }
}
VERIFY(0x020A5EF4, Bk_move);

/* weak guest-call stubs for the functions defined in d_a_bk.cpp (so this part links alone in
 * its temporary unit; the real definitions replace them in the merged unit) */
__attribute__((weak)) void anm_init(bk_class* a, int b, f32 m, u8 l, f32 s, int f) { gabi::call(0x02098DA4, a, b, m, l, s, f); }
__attribute__((weak)) BOOL daBk_wepon_view_check(bk_class* a) { return gabi::call<BOOL>(0x0209AB64, a); }
__attribute__((weak)) void wait_set(bk_class* a) { gabi::call(0x0209B264, a); }
__attribute__((weak)) BOOL daBk_bomb_view_check(bk_class* a) { return gabi::call<BOOL>(0x0209AE40, a); }
__attribute__((weak)) BOOL daBk_player_view_check(bk_class* a, cXyz* p, s16 b, s16 c) { return gabi::call<BOOL>(0x0209B05C, a, p, b, c); }
