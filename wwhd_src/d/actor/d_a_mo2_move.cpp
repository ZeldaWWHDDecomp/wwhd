/** Moblin patrol, combat movement and spear collision.
 * WWHD behavior verified against cking.rpx; names/layout follow zeldaret/tww. */
#include "d/actor/d_a_mo2_move.h"
#include "d/actor/d_a_mo2_t60.h"
#include <cmath>
#include <unordered_map>
using namespace mo2Move;
// HD: inline collision construction uses this Moblin translation unit's
// vtables.
static constexpr u32 moveSafeStringVtable = 0x10014D94;
static const MoveLineCheckVtables moveLineCheckVtables = {
    0x10014E4C, 0x10014E5C, 0x10014E7C, 0x10014E6C, 0x10014DCC, 0x10014DBC,
};

// HD: these aliases map the shared Moblin TU storage; execution/creation own
// their names.
static mo2Move::mo2HIO_c moveHIO;
static s8 moveSearchSpecial;
static u8 moveJailMode;
static MoveAttackPower moveAttackPower;
static MoveAttackFloat moveBreakTime;
static MoveAttackKind moveAttackKind;
static MoveAttackKind moveAttackSoundKind;
static MoveAttackPower moveAttackSound;
static MoveAttackInfo moveAttackInfo;

/* 0255F530 dKy_Sound_get(): SND_INFLUENCE* (position first) */

static inline fopAc_ac_c *hit_obj_actor(void *obj) {
  if (obj == nullptr)
    return nullptr;
  mo2Move::HitStatus *stts = reinterpret_cast<mo2Move::HitObject *>(obj)->mStts;
  if (stts == nullptr)
    return nullptr;
  return stts->mpActor;
}

static inline bool has_weapon(mo2_class *i) {
  return i->mbHasInnateWeapon != 0 || i->mWeaponSearchGaveUp != 0;
}

static inline void to_jyunkai(mo2_class *i, s16 mode) {
  i->mDamageReaction.mAction = ACTION_JYUNKAI;
  path_check(i);
  wait_set(i);
  i->mDamageReaction.mMode = mode;
}

static inline void set_m05A4_rnd(mo2_class *i, int k, f32 r, f32 b) {
  i->mTimers[k] = (s16)gabi::ftoi(cM_rndF(r) + b);
}

/* inlined into Mo2_move */
static inline void path_check2(mo2_class *i_this) {
  gabi::Local<MoveLineCheckHD> chk;
  dBgS_LinChk_ct(chk, moveLineCheckVtables);
  gabi::Local<cXyz> local_9c;
  gabi::Local<cXyz> local_a8;
  *local_9c = i_this->actor.current.pos.get();
  local_9c->y = i_this->actor.current.pos.y + (REG_F(13, 2) + 20.0f);
  *local_a8 = i_this->mPathTargetPos.get();
  local_a8->y = i_this->mPathTargetPos.y + (REG_F(13, 3) + 10.0f);
  dBgS_LinChk_Set(chk, local_9c, local_a8, &i_this->actor);
  if (cBgS_LineCross(dComIfG_Bgsp(), chk)) {
    i_this->mPathOn = 0;
  }
  dBgS_LinChk_dt(chk, moveLineCheckVtables);
}

/* inlined into Mo2_move */
static inline void walk_set(mo2_class *i_this) {
  if (i_this->mbHasInnateWeapon != 0) {
    if (i_this->mMode == 1) {
      anm_init(i_this, dRes_INDEX_MO2_BCK_KWALK_e, 10.0f,
               J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_KWALK_e);
      return;
    }
    anm_init(i_this, dRes_INDEX_MO2_BCK_WALK_e, 10.0f, J3DFrameCtrl::EMode_LOOP,
             1.0f, dRes_INDEX_MO2_BAS_WALK_e);
    return;
  }
  if (i_this->mWeaponSearchGaveUp == 0) {
    anm_init(i_this, dRes_INDEX_MO2_BCK_NWALK_e, 10.0f,
             J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_NWALK_e);
  } else {
    anm_init(i_this, dRes_INDEX_MO2_BCK_NYWALK_e, 10.0f,
             J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_NYWALK_e);
  }
}

/* true60: the frame's per-frame work in the dispatch goes on its last dispatch call: the tick30 step, or
 * the in-between step whose execute changed the AI state (held after it; d_a_mo2_t60.h). Mo2_move's entry state and
 * whether damage_check already changed it on this step. */
static uint64_t s_mo2T60EntryState;
static bool s_mo2T60ChangedBefore;
/* the first-step AI-timer rule: the deciding per-frame work (the dash count, the guard decision, the push
 * count) goes on the frame's FIRST step, before the frame's motion, once per frame (d_a_bk_t60.h bk_t60_frame_work). */
static inline bool mo2_t60_frame_work(mo2_class *i_this) {
  (void)i_this;
  return bk_t60_first();
}
/* the floor hit's smoke and sound (an effect read with the frame-end attack frame) keep the frame's last dispatch call:
 * the tick30 step, or the in-between step whose execute changed the AI state (the form before the first-step AI-timer rule) */
static inline bool mo2_t60_frame_end_work(mo2_class *i_this) {
  return bk_t60_end() || s_mo2T60ChangedBefore || mo2_t60_ai_state(i_this) != s_mo2T60EntryState;
}

/* true60 (the animation crossing-check rule): the animation frame entered v on this step (once per pass, as the 30 Hz frame check) */
static inline bool morf_frame_is(mo2_class *i_this, int v) {
  return bk_t60_frame_is(i_this->mpMorf->getFrame(), i_this->mpMorf->getPlaySpeed(), v);
}

/* inlined into Mo2_move */
static inline void jyunkai(mo2_class *i_this) {
  dComIfGp_get();
  fopAcM_SearchByID(i_this->mWeaponPcId); /* result unused */
  i_this->mCoCyl.SetR(REG0_F(4) + 40.0f);
  switch ((u16)i_this->mDamageReaction.mMode) {
  case (u16)-10:
    i_this->mTimers[1] = 0x3c;
    i_this->mDamageReaction.mMode = -9;
    mo2_t60_ai_ran(i_this); /* true60: mode -9's body runs in this call (fall-through): not held */
    /* fallthrough */
  case (u16)-9:
    if (i_this->mTimers[1] == 0) {
      i_this->mDamageReaction.mMode = 2;
    }
    i_this->actor.speedF = 0.0f;
    if (i_this->mTimers[1] > 0x1e) {
      return;
    }
    break;
  case 0:
    walk_set(i_this);
    /* fallthrough */
  case (u16)-1:
    i_this->mDamageReaction.mMode = 1;
    if (has_weapon(i_this)) {
      if (i_this->mPathOn != 0) {
        s8 n = (s8)(i_this->mPathPoint + i_this->mPathDir);
        i_this->mPathPoint = n;
        if (n >= (s8)i_this->ppd->m_num) {
          if (i_this->ppd->m_closed & 1) { /* dPath_ChkClose */
            i_this->mPathPoint = 0;
          } else {
            i_this->mPathDir = -1;
            i_this->mPathPoint = (s8)(i_this->ppd->m_num - 2);
          }
          u16 nextID = i_this->ppd->m_nextID;
          if (nextID != 0xffff) {
            i_this->ppd = reinterpret_cast<mo2Move::dPath *>(
                dPath_GetRoomPath(nextID, fopAcM_GetRoomNo(&i_this->actor)));
            if (i_this->ppd == nullptr)
              JUT_ASSERT_fail("d_a_mo2.cpp", 0xAFB, "i_this->ppd != (0)");
          }
        } else if (n < 0) {
          i_this->mPathDir = 1;
          i_this->mPathPoint = 1;
        }
        mo2Move::dPnt *point = &i_this->ppd->m_points[i_this->mPathPoint];
        i_this->mPathTargetPos.x = point->m_position.x;
        i_this->mPathTargetPos.y = point->m_position.y;
        i_this->mPathTargetPos.z = point->m_position.z;
      } else {
        way_pos_check(i_this, &i_this->mPathTargetPos);
      }
    } else {
      way_pos_check(i_this, &i_this->mPathTargetPos);
      set_m05A4_rnd(i_this, 1, 25.0f, 30.0f);
    }
    i_this->mTimers[2] = 0x1e;
    mo2_t60_ai_ran(i_this); /* true60: mode 1's body runs in this call (fall-through): not held */
    /* fallthrough */
  case 1: {
    s16 maxSpeed;
    f32 dVar12;
    if (has_weapon(i_this)) {
      maxSpeed = 0x400;
      dVar12 = moveHIO.mPatrolSpeedArmed;
    } else {
      maxSpeed = 0x1000;
      dVar12 = moveHIO.mPatrolSpeedUnarmed;
    }
    f32 x = i_this->mPathTargetPos.x - i_this->actor.current.pos.x;
    f32 z = i_this->mPathTargetPos.z - i_this->actor.current.pos.z;
    i_this->mDamageReaction.mTurnTargetY = cM_atan2s(x, z);
    if (i_this->mPathOn != 0 && has_weapon(i_this)) {
      f32 d = std_sqrtf(gabi::fmadds(x, x, z * z));
      if (d < (dVar12 * 0.25f) * 15.0f) {
        mo2Move::dPnt *point = &i_this->ppd->m_points[i_this->mPathPoint];
        if (point->mArg3 == 3 || point->mArg3 == 7 || point->mArg3 == 8) {
          wait_set(i_this);
          if (point->mArg3 >= 7) {
            set_m05A4_rnd(i_this, 1, 80.0f, 70.0f);
          }
          i_this->mDamageReaction.mMode = 2;
        } else {
          i_this->mDamageReaction.mMode = -1;
        }
      }
      path_check2(i_this);
      if ((i_this->mCoCyl.ChkCoHit() ||
           i_this->mDamageReaction.mAcch.ChkWallHit()) &&
          i_this->mTimers[2] == 0) {
        wait_set(i_this);
        set_m05A4_rnd(i_this, 1, 80.0f, 70.0f);
        i_this->mPathDir = -i_this->mPathDir;
        i_this->mDamageReaction.mMode = 2;
      }
    } else {
      f32 d = std_sqrtf(gabi::fmadds(x, x, z * z));
      if (d < (dVar12 * 0.25f) + (dVar12 * 0.25f) ||
          (i_this->mTimers[2] == 0 &&
           (i_this->mDamageReaction.mAcch.ChkWallHit() ||
            ground_4_check(i_this, 1, i_this->actor.current.angle.y,
                           200.0f)))) {
        wait_set(i_this);
        i_this->mDamageReaction.mMode = 2;
      }
    }
    cLib_addCalcAngleS2(&i_this->actor.current.angle.y,
                        i_this->mDamageReaction.mTurnTargetY, 4, maxSpeed);
    cLib_addCalc2(&i_this->actor.speedF, dVar12, 1.0f, 5.0f);
    break;
  }
  case 2:
    i_this->actor.speedF = 0.0f;
    if (i_this->mTimers[1] == 0) {
      if (i_this->mPathOn != 0 && has_weapon(i_this)) {
        mo2Move::dPnt *point = &i_this->ppd->m_points[i_this->mPathPoint];
        if (point->mArg3 == 7 || point->mArg3 == 8) {
          i_this->mDamageReaction.mMode = 4;
          set_m05A4_rnd(i_this, 1, 100.0f, 100.0f);
          anm_init(i_this, dRes_INDEX_MO2_BCK_KKEIKAI_e, 5.0f,
                   J3DFrameCtrl::EMode_LOOP, 1.0f,
                   dRes_INDEX_MO2_BAS_KKEIKAI_e);
          if (point->mArg3 == 7) {
            i_this->mDamageReaction.mTurnTargetY += -0x4000;
          } else {
            i_this->mDamageReaction.mTurnTargetY += 0x4000;
          }
          break;
        }
      }
      i_this->mDamageReaction.mMode = 0;
      if (i_this->mPathIndex != 0xFF && i_this->mPathOn == 0) {
        path_check(i_this);
      }
      if (i_this->mbHasInnateWeapon == 0 && i_this->mWeaponSearchGaveUp == 0 &&
          i_this->mWeaponRunTimer == 0) {
        i_this->mWeaponSearchGaveUp = 1;
      }
    }
    break;
  case 4:
    cLib_addCalcAngleS2(&i_this->actor.current.angle.y,
                        i_this->mDamageReaction.mTurnTargetY, 4, 0x1000);
    /* fallthrough */
  case 3: {
    i_this->actor.speedF = 0.0f;
    /* true60 (the animation crossing-check rule): the animation frame entered 3 / 53 on this step */
    if (morf_frame_is(i_this, 3) || morf_frame_is(i_this, 0x35)) {
      fopAcM_monsSeStart(&i_this->actor, JA_SE_CV_MO_SEARCH, 0);
    }
    if (i_this->mTimers[1] == 0) {
      i_this->mDamageReaction.mMode = 0;
      if (i_this->mPathIndex != 0xFF && i_this->mPathOn == 0) {
        path_check(i_this);
      }
    }
    break;
  }
  }
  s32 iVar6 = fopAcM_otoCheck(&i_this->actor, 1000.0f);
  iVar6 += moveSearchSpecial;
  /* true60 (first-step input addendum): the notice decision reads Link (and the sound contact): tick30 step */
  if (has_weapon(i_this) && bk_t60_end()) {
    if (iVar6 != 0 || (i_this->mTargetDist < moveHIO.mSearchDist &&
                       daMo2_player_view_check(
                           i_this, &i_this->mDamageReaction.mpLookTarget->current.pos,
                           i_this->mTargetAngleY, moveHIO.mViewAngle))) {
      if (i_this->mMode == 1) {
        i_this->mDamageReaction.mAction = ACTION_NAGE;
        if (moveJailMode != 0) {
          i_this->mDamageReaction.mMode = -10;
        } else {
          i_this->mDamageReaction.mMode = 0;
        }
        if (iVar6 != 0) {
          u8 *sound = dKy_Sound_get();
          i_this->mSoundTargetPos.copy(
              reinterpret_cast<mo2Move::Sound *>(sound)->field_0x0);
          i_this->mSoundTargetTimer = 0x1e;
        }
      } else {
        i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
        i_this->mDamageReaction.mMode = 0;
        i_this->mTimers[1] = REG_S(18, 0); // HD: GameCube uses zero.
      }
      if (i_this != nullptr) {
        fopAcM_monsSeStart(&i_this->actor, JA_SE_CV_MO_FIND_ENEMY, 0);
      }
    }
  } else if (!has_weapon(i_this) && i_this->mWeaponSearchGaveUp == 0 && i_this->mDamageReaction.mMode == 2) {
    /* true60 (the animation crossing-check rule): the animation frame entered 11 / 25 on this step */
    if ((morf_frame_is(i_this, 0xb) || morf_frame_is(i_this, 0x19)) && cM_rndF(1.0f) < 0.5f) {
      if (i_this != nullptr) {
        fopAcM_monsSeStart(&i_this->actor, JA_SE_CV_MO_LOSE_LANCE, 0);
      }
    }
  }
  if (i_this->mbHasInnateWeapon == 0 && daMo2_wepon_view_check(i_this)) {
    i_this->mDamageReaction.mAction = ACTION_WEPON_SEARCH;
    i_this->mDamageReaction.mMode = -1;
  }
  if (bk_t60_end() && daMo2_bomb_view_check(i_this)) { /* true60: reads the bombs and Link: tick30 step */
    i_this->mDamageReaction.mAction = ACTION_B_NIGE;
    i_this->mDamageReaction.mMode = 0;
  }
}

// HD: in Hyrule, keep the Moblin out of the wall behind it, as in d_mahi.
static inline void back_wall_check(mo2_class *i_this) {
  if (!SafeString_eq(
          "Hyrule",
          reinterpret_cast<mo2Move::Play *>(dComIfGp_get())->mStartStageName,
          moveSafeStringVtable)) {
    return;
  }
  gabi::Local<MoveLineCheckHD> linChk;
  dBgS_LinChk_ct(linChk, moveLineCheckVtables);
  gabi::Local<cXyz> sp34;
  *sp34 = i_this->actor.current.pos.get();
  sp34->y += 20.0f;
  cMtx_YrotS(calc_mtx(), i_this->actor.shape_angle.y);
  gabi::Local<cXyz> sp54;
  sp54->x = 0.0f;
  sp54->y = 20.0f;
  sp54->z = REG_F(18, 7) + -50.0f;
  gabi::Local<cXyz> sp48;
  MtxPosition(sp54, sp48);
  gabi::Local<cXyz> sp94;
  cXyz_pl(&i_this->actor.current.pos, sp94, sp48);
  gabi::Local<cXyz> spA0;
  spA0->copy(*sp94);
  dBgS_LinChk_Set(linChk, sp34, spA0, &i_this->actor);
  if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
    /* true60 (the per-step movement rule): a fifth of the offset per frame: x dt */
    f32 k = mo2_t60_rate(REG_F(18, 9) + 0.2f);
    i_this->actor.current.pos.x = gabi::fnmsubs(sp48->x, k, i_this->actor.current.pos.x);
    i_this->actor.current.pos.z = gabi::fnmsubs(sp48->z, k, i_this->actor.current.pos.z);
    hd_debug_print(0x1E, 0x190, "MO2 BACK KABE HIT!");
  }
  dBgS_LinChk_dt(linChk, moveLineCheckVtables);
}

/* true60 (True60 pair f45 shape / current angle.y -1133 A / -220 B): the turn
 * at fight_run's head runs BEFORE the mode switch, so A's call that takes mode 0 -> 1 (the fall-through into mode 1's
 * body) gates it with mode 0: no turn in that frame, the turn starts the next frame (set this frame, act next frame).
 * At N steps the fall-through on step 0 leaves mode 1 for the frame's later steps (not held, mo2_t60_ai_ran: mode 1's
 * body continues there), and their head turned with mode 1's 0x800 on f45's tick30 step, one frame early. The head's
 * gate (and its max speed) reads the mode fight_run had on the frame's first step, as A's one call reads it. Host
 * memory keyed by the actor, this unit only (split-actor state (1)). Negative control WWHD_T60_MO2TURN_NEGCTRL=1 (game build: the
 * step's own mode, the form before). dt = 1: the original (the call's own mode). */
namespace {
struct Mo2T60RunMode {
  u32 frame; /* 30 Hz frame tag */
  s16 mode;  /* mDamageReaction.mMode at fight_run's head on the frame's first step */
};
std::unordered_map<u32, Mo2T60RunMode> s_mo2T60RunMode;
} // namespace
static inline s16 mo2_t60_run_turn_mode(mo2_class *i_this) {
  const s16 mode = i_this->mDamageReaction.mMode;
  if (t60::dt() == 1.0f)
    return mode;
#if defined(GABI_GAME_BUILD) && GABI_T60
  static const bool nc = [] { const char *e = getenv("WWHD_T60_MO2TURN_NEGCTRL"); return e && e[0] == '1'; }();
  if (nc)
    return mode;
#endif
  if (t60::first_step()) {
    Mo2T60RunMode &e = s_mo2T60RunMode[gabi::ea(i_this)];
    e.frame = bk_t60_frame_tag();
    e.mode = mode;
    return mode;
  }
  auto it = s_mo2T60RunMode.find(gabi::ea(i_this));
  if (it == s_mo2T60RunMode.end() || it->second.frame != bk_t60_frame_tag())
    return mode;
  return it->second.mode;
}

/* inlined into Mo2_move */
static inline bool fight_run(mo2_class *i_this) {
  fopAc_ac_c *player = dComIfGp_getPlayer(0);
  f32 dVar9 = CPad_GET_STICK_POS_X(0);
  i_this->mDamageReaction.mTurnTargetY = i_this->mTargetAngleY;
  const s16 t60TurnMode = mo2_t60_run_turn_mode(i_this); /* true60: the frame's first-step mode */
  if (i_this->mDashStopTimer == 0 && t60TurnMode != 0) {
    s16 maxSpeed = 0x400;
    if (t60TurnMode == 1) {
      maxSpeed = 0x800;
    }
    cLib_addCalcAngleS2(&i_this->actor.current.angle.y,
                        i_this->mDamageReaction.mTurnTargetY, 4, maxSpeed);
  }
  switch ((u16)i_this->mDamageReaction.mMode) {
  case 0:
    if (i_this->mTimers[1] == 0) {
      fight_run_set(i_this);
      i_this->mDamageReaction.mMode = 1;
      i_this->mDashFrames = 0;
      mo2_t60_ai_ran(i_this); /* true60: mode 1's body runs in this call (fall-through): not held */
    } else {
      i_this->actor.speedF = 0.0f;
      break;
    }
    /* fallthrough */
  case 1: {
    f32 target;
    f32 fVar10 = 20.0f;
    if (i_this->mDashFrames != 0) {
      target = moveHIO.mDashSpeed;
      i_this->mSmokeTimer = moveHIO.mSmokeTimerBase + 4;
      i_this->mSmokeType = 4;
    } else {
      if (has_weapon(i_this)) {
        target = moveHIO.mRunSpeedArmed;
      } else {
        target = moveHIO.mRunSpeedUnarmed;
      }
      fVar10 = 5.0f;
    }
    cLib_addCalc2(&i_this->actor.speedF, target, 1.0f, fVar10);
    /* true60 (first-step input addendum): the way check reads Link's facing, so the run / dash choice is a
     * decision: both branches (the dash stop, the dash start and its count, the range / attack decisions) on the tick30
     * step, once per frame as A's call; the earlier steps keep the frame's speed and motion. dt = 1: every call. */
    if (!bk_t60_end()) {
    } else if (daMo2_player_way_check(i_this)) {
      if (i_this->mDashFrames > 0x14) {
        i_this->mDashFrames = 0;
        anm_init(i_this, dRes_INDEX_MO2_BCK_GAKEDEMO_e, 3.0f,
                 J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        i_this->mDashStopTimer = (s16)gabi::ftoi(cM_rndF(10.0f) + 15.0f);
      }
      if (i_this->mDashStopTimer == 1) {
        i_this->mDashStopTimer = 0;
        fight_run_set(i_this);
      }
      if (i_this->mDashStopTimer == 0 && i_this->mTargetDist < moveHIO.mFightDist) {
        i_this->mDamageReaction.mMode = 2;
        i_this->mTimers[4] = 0x32;
      }
    } else {
      if (i_this->mDashFrames == 0 && i_this->mbHasInnateWeapon != 0) {
        i_this->mDashFrames = 1;
        anm_init(i_this, dRes_INDEX_MO2_BCK_DASH_e, 5.0f,
                 J3DFrameCtrl::EMode_LOOP, moveHIO.mDashAnmSpeed,
                 dRes_INDEX_MO2_BAS_DASH_e);
      }
      /* true60 (the 30 Hz timer rule): a per-frame count: this branch runs once per frame (the tick30 step, above) */
      if (i_this->mDashFrames != 0) {
        i_this->mDashFrames = i_this->mDashFrames + 1;
      }
      // HD: also limit the target height difference.
      if (i_this->mDashStopTimer == 0 && i_this->mTargetDist < moveHIO.mAttackDist &&
          i_this->mTargetDistY < REG_F(10, 3) + 250.0f) {
        i_this->mDamageReaction.mAction = ACTION_FIGHT;
        i_this->mDamageReaction.mMode = 0;
        return true;
      }
    }
    break;
  }
  case 2:
    /* true60: this mode only decides (Link's distance and stick): its body on the tick30 step */
    if (!bk_t60_end())
      break;
    i_this->mDashFrames = 0;
    if (cM_rndF(1.0f) < 0.3f && i_this->mbHasInnateWeapon == 0) {
      i_this->mDamageReaction.mMode = 8;
      wait_set(i_this);
      set_m05A4_rnd(i_this, 1, 20.0f, 20.0f);
    } else {
      if (std::fabs(dVar9) > 0.1f) {
        if (has_weapon(i_this)) {
          anm_init(i_this, dRes_INDEX_MO2_BCK_BWALKLR_e, 10.0f,
                   J3DFrameCtrl::EMode_LOOP, 1.0f,
                   dRes_INDEX_MO2_BAS_BWALKLR_e);
        } else {
          anm_init(i_this, dRes_INDEX_MO2_BCK_NBWALKLR_e, 10.0f,
                   J3DFrameCtrl::EMode_LOOP, 1.0f,
                   dRes_INDEX_MO2_BAS_NBWALKLR_e);
        }
        if (dVar9 > 0.0f) {
          i_this->mDamageReaction.mMode = 5;
        } else {
          i_this->mDamageReaction.mMode = 6;
        }
      } else if (i_this->mTargetDist < moveHIO.mAttackDist) {
        if (has_weapon(i_this)) {
          anm_init(i_this, dRes_INDEX_MO2_BCK_BWALKFB_e, 10.0f,
                   J3DFrameCtrl::EMode_LOOP, -1.0f,
                   dRes_INDEX_MO2_BAS_BWALKFB_e);
        } else {
          anm_init(i_this, dRes_INDEX_MO2_BCK_NBWALKFB_e, 10.0f,
                   J3DFrameCtrl::EMode_LOOP, -1.0f,
                   dRes_INDEX_MO2_BAS_NBWALKFB_e);
        }
        i_this->mDamageReaction.mMode = 4;
      } else {
        if (has_weapon(i_this)) {
          anm_init(i_this, dRes_INDEX_MO2_BCK_BWALKFB_e, 10.0f,
                   J3DFrameCtrl::EMode_LOOP, 1.0f,
                   dRes_INDEX_MO2_BAS_BWALKFB_e);
        } else {
          anm_init(i_this, dRes_INDEX_MO2_BCK_NBWALKFB_e, 10.0f,
                   J3DFrameCtrl::EMode_LOOP, 1.0f,
                   dRes_INDEX_MO2_BAS_NBWALKFB_e);
        }
        i_this->mDamageReaction.mMode = 3;
      }
      set_m05A4_rnd(i_this, 1, 20.0f, 20.0f);
    }
    break;
  case 3:
    cLib_addCalc2(&i_this->actor.speedF, moveHIO.mStrafeSpeed, 1.0f, 20.0f);
    if (i_this->mTimers[1] == 0) {
      i_this->mDamageReaction.mMode = 2;
    }
    break;
  case 4:
    if (i_this->mGroundCheck != 2) {
      cLib_addCalc2(&i_this->actor.speedF, -moveHIO.mStrafeSpeed, 1.0f, 20.0f);
      if (i_this->mTimers[1] == 0) {
        i_this->mDamageReaction.mMode = 2;
      }
    } else {
      i_this->mDamageReaction.mMode = 3;
      if (has_weapon(i_this)) {
        anm_init(i_this, dRes_INDEX_MO2_BCK_BWALKFB_e, 10.0f,
                 J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_BWALKFB_e);
      } else {
        anm_init(i_this, dRes_INDEX_MO2_BCK_NBWALKFB_e, 10.0f,
                 J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_NBWALKFB_e);
      }
    }
    break;
  case 5:
  case 6: {
    const bool moveRight = i_this->mDamageReaction.mMode == 5;
    if (moveRight) {
      if (i_this->mGroundCheck != 4) {
        i_this->mMoveAngleOffset = 0x4000;
      } else {
        i_this->mDamageReaction.mMode = 6;
        i_this->mMoveAngleOffset = -0x4000;
      }
    } else if (i_this->mGroundCheck != 8) {
      i_this->mMoveAngleOffset = -0x4000;
    } else {
      i_this->mDamageReaction.mMode = 5;
      i_this->mMoveAngleOffset = 0x4000;
    }
    cLib_addCalc2(&i_this->actor.speedF, moveHIO.mSideStepSpeed, 1.0f, 30.0f);
    if (i_this->mTimers[1] == 0) {
      i_this->mDamageReaction.mMode = 2;
    }
    break;
  }
  case 8:
    i_this->actor.speedF = 0.0f;
    if (i_this->mTimers[1] == 0) {
      i_this->mDamageReaction.mMode = 2;
    }
  }
  if (i_this->mDamageReaction.mMode >= 3 && i_this->mSideStepTimer <= 2) {
    /* true60 (first-step input addendum): the range decisions read Link: tick30 step (the guard reaction below
     * stays on the first step, the first-step AI-timer rule, as the Bokoblin's) */
    const bool t60Decide = bk_t60_end();
    if (t60Decide && i_this->mTargetDist > moveHIO.mFightDist + 75.0f) {
      to_jyunkai(i_this, 0);
    }
    if (t60Decide && i_this->mTargetDist < moveHIO.mAttackDist + 62.5f &&
        i_this->mTargetDist > moveHIO.mAttackDist - 62.5f && i_this->mTimers[4] == 0) {
      i_this->mTimers[4] = moveHIO.mAttackCheckWait;
      if (cM_rndF(100.0f) < moveHIO.mAttackChance &&
          i_this->mTargetDistY <
              REG_F(10, 3) +
                  250.0f) { // HD: also limit the target height difference.
        i_this->mDamageReaction.mAction = ACTION_FIGHT;
        i_this->mDamageReaction.mMode = 0;
      }
    }
    /* true60 (the per-step chance rule, the first-step AI-timer rule): the guard reaction to the player's cut: decided once per 30 Hz frame,
     * on its first step, before the frame's motion (the original call's decisions) */
    if (i_this->mNoGuardTimer == 0 && mo2_t60_frame_work(i_this)) {
      mo2Move::Attention *attention =
          &reinterpret_cast<mo2Move::Play *>(dComIfGp_get())->mAttention;
      if (i_this->mbHasInnateWeapon != 0 &&
          reinterpret_cast<mo2Move::Player *>(player)->mCutType != 0 &&
          (cc_pl_cut_bit_get() & i_this->mCutBits) != 0) {
        if ((dAttention_LockonTruth(attention) ||
             (attention->mFlags & 0x20000000) != 0) &&
            &i_this->actor == dAttention_LockonTarget(attention, 0)) {
          i_this->mDamageReaction.mAction = ACTION_DEFENCE;
          i_this->mDamageReaction.mMode = 0;
        }
      }
    }
  }
  /* true60 (the 30 Hz timer rule): the push count is a per-frame count: counted (or cleared) once per 30 Hz frame, on
   * the frame's last dispatch call (first-step AI-timer addendum: order beats first-step placement): it reads
   * Link's distance / view and comes after the mode 2 body in A's call; decided on the first step, the push set the
   * action there, the tick30 step was held and the mode 2 body (tick30) was lost (pair frame 41: timer[1] 34 in
   * A, 0 in B). The tick30 step, or the in-between step whose call changed the state (its tick30 step held: kmoG2_2
   * frame 33, d_a_bk_t60.h bk_t60_last_call_once). */
  if (!bk_t60_last_call_once(&i_this->mOshiCount, s_mo2T60ChangedBefore ||
                                                      mo2_t60_ai_state(i_this) != s_mo2T60EntryState)) {
  } else if (i_this->mbHasInnateWeapon != 0 && i_this->mTargetDist < moveHIO.mAttackDist - 62.5f &&
      i_this->mTargetDistY <
          REG_F(10, 3) +
              250.0f && // HD: target height also limits this transition.

      daMo2_player_view_check(i_this,
                              &i_this->mDamageReaction.mpLookTarget->current.pos,
                              i_this->mTargetAngleY, moveHIO.mViewAngle)) {
    i_this->mOshiCount++;
    if (i_this->mOshiCount >= 0xF && cM_rndF(1.0f) < 0.5f) {
      i_this->mOshiCount = 0;
      i_this->mDamageReaction.mAction = ACTION_OSHI;
      i_this->mDamageReaction.mMode = 0;
    }
  } else {
    i_this->mOshiCount = 0;
  }
  /* true60 (first-step input addendum): the line check to Link: tick30 step */
  if (bk_t60_end() && daMo2_player_bg_check(i_this,
                                            &i_this->mDamageReaction.mpLookTarget->current.pos)) {
    to_jyunkai(i_this, -10);
  }
  if (i_this->mbHasInnateWeapon == 0 && daMo2_wepon_view_check(i_this)) {
    i_this->mDamageReaction.mAction = ACTION_WEPON_SEARCH;
    i_this->mDamageReaction.mMode = -1;
  }
  if (bk_t60_end() && daMo2_bomb_view_check(i_this)) { /* true60: reads the bombs and Link: tick30 step */
    i_this->mDamageReaction.mAction = ACTION_B_NIGE;
    i_this->mDamageReaction.mMode = 0;
  }
  f32 r = REG_F(6, 7) + 90.0f;
  i_this->mGroundCheck =
      (u8)ground_4_check(i_this, 4, i_this->actor.current.angle.y, r);
  if (i_this->mSideStepTimer != 0) {
    if (std::fabs(i_this->actor.speedF) < 30.0f) {
      if (i_this->mSideStepDir == 0) {
        i_this->mDamageReaction.mLookMode = 3;
      } else if (i_this->mSideStepDir == 1) {
        i_this->mDamageReaction.mLookMode = 4;
      } else if ((i_this->mFrameCounter & 0x10) != 0) {
        i_this->mDamageReaction.mLookMode = 3;
      } else {
        i_this->mDamageReaction.mLookMode = 4;
      }
      cLib_addCalcAngleS2(&i_this->mJawAngleZ, 12000, 2, 0x1800);
    } else {
      i_this->mDamageReaction.mLookMode = 1;
    }
  } else {
    i_this->mDamageReaction.mLookMode = 1;
    if (i_this->mSideStepWait == 0) {
      i_this->mSideStepWait = (s16)gabi::ftoi(cM_rndF(100.0f) + 30.0f);
      if (i_this->mGroundCheck == 4) {
        i_this->mSideStepDir = 0;
        i_this->mSideStepTimer = 0x10;
      } else if (i_this->mGroundCheck == 8) {
        i_this->mSideStepDir = 1;
        i_this->mSideStepTimer = 0x10;
      } else if (i_this->mGroundCheck == 2) {
        i_this->mSideStepDir = 2;
        i_this->mSideStepTimer = 0x20;
      }
    }
  }
  back_wall_check(i_this);
  return false;
}

/* true60 (the animation-rate rule): the attack frame mAttackFrame is a hand-stepped animation frame (the step's play
 * speed per frame, beside the animation it follows): + speed x dt per step. Its window compares (At spheres, sounds,
 * blur) fall on whole and half frames, so a 30 Hz frame the attack ran through on every step ends exactly on the
 * original's value: the frame's start and speed are held (host memory keyed by the actor, frame tag g_Counter.mCounter0)
 * and its tick30 step stores start + speed (the original's add); a frame that changed the attack inside it (a new step
 * speed, an attack set on an in-between step) keeps the steps' sum. (The Bokoblin's bk_t60_attack_advance.) */
namespace {
struct Mo2T60AttackFrame {
  u32 frame;  /* 30 Hz frame tag */
  f32 start;  /* mAttackFrame at the frame's first step */
  f32 speed;  /* the per-frame speed added */
  f32 expect; /* mAttackFrame after the last step's add (a change from outside breaks the hold) */
  u8 steps;
  bool valid;
};
std::unordered_map<u32, Mo2T60AttackFrame> s_mo2T60AttackFrames;
/* the At positions of the frame's first step (mo2_t60_at_frame_start), key: the actor */
struct Mo2T60AtStart {
  u32 frame; /* 30 Hz frame tag */
  Vec3f pos, pos2; /* plain host values: a host cXyz would read / write guest memory at ea(host address) */
};
std::unordered_map<u32, Mo2T60AtStart> s_mo2T60AtStart;
} // namespace
/* true60 (the one-sweep-per-frame attack rule, as the Bokoblin's bk_t60_at_frame_start): the hand / spear positions the
 * execute reads come from the last model calc (the previous frame's in the original, the frame's earlier step's at N
 * steps); the tick30 registration takes the positions the frame's first step read. dt = 1: nothing. */
/* true60 (True60 pair f31 mAtPos +0x2D0C / +0x2D10 2.98 off): the Bokoblin's form
 * does not fit the Moblin's execute order. daMo2_Execute plays and calcs its morf BEFORE Mo2_move (d_a_mo2_exec.cpp,
 * the process-order read rule) while the action is below ACTION_CARRY: the original's yari_hit_check reads the spear / hand points of
 * frame f's own calc, and the tick30 step's read is that frame-end pose (on frame f - 1's base matrix, as A's). The
 * first step's read is the half-frame pose, so restoring it put the tick30 registration half a frame back (mAtPos the
 * spear head of step 0, A's f31 83.01 / 151.54 vs B 81.81 / 154.52). Only an action at or past ACTION_CARRY calcs
 * after Mo2_move (the Bokoblin's order: the last calc is the previous step's): the hold stays there. Negative control
 * WWHD_T60_MO2AT_NEGCTRL=1 (game build: the hold on every action, the form before). dt = 1: nothing. */
static inline bool mo2_t60_at_calc_before_move(mo2_class *i_this) {
#if defined(GABI_GAME_BUILD) && GABI_T60
  static const bool nc = [] { const char *e = getenv("WWHD_T60_MO2AT_NEGCTRL"); return e && e[0] == '1'; }();
#else
  const bool nc = false;
#endif
  return !nc && i_this->mDamageReaction.mAction < ACTION_CARRY;
}
static inline void mo2_t60_at_frame_start(mo2_class *i_this) {
  if (!bk_t60_at_grid() || mo2_t60_at_calc_before_move(i_this))
    return;
  if (t60::first_step()) {
    Mo2T60AtStart &e = s_mo2T60AtStart[gabi::ea(i_this)];
    e.frame = bk_t60_frame_tag();
    e.pos = i_this->mAtPos.get();
    e.pos2 = i_this->mAt2Pos.get();
  } else if (t60::tick30()) {
    auto it = s_mo2T60AtStart.find(gabi::ea(i_this));
    if (it != s_mo2T60AtStart.end() && it->second.frame == bk_t60_frame_tag()) {
      i_this->mAtPos = it->second.pos;
      i_this->mAt2Pos = it->second.pos2;
    }
  }
}
/* the step's new attack frame (dt = 1: the original's add); *frameEnd: the frame-end value the original frame reaches */
static f32 mo2_t60_attack_advance(mo2_class *i_this, f32 speed, f32 *frameEnd) {
  f32 cur = i_this->mAttackFrame;
  if (t60::dt() == 1.0f) {
    *frameEnd = cur + speed;
    return cur + speed;
  }
  u32 key = gabi::ea(i_this), tag = bk_t60_frame_tag();
  if (t60::first_step())
    s_mo2T60AttackFrames[key] = Mo2T60AttackFrame{tag, cur, speed, cur, 0, true};
  auto it = s_mo2T60AttackFrames.find(key);
  bool held = it != s_mo2T60AttackFrames.end() && it->second.valid && it->second.frame == tag;
  if (held && (it->second.speed != speed || it->second.expect != cur))
    held = it->second.valid = false;
  f32 next = cur + gabi::fmuls_ppc(speed, t60::dt());
  *frameEnd = held ? it->second.start + speed : next;
  if (held) {
    it->second.steps++;
    if (t60::tick30() && it->second.steps == (u8)t60::steps())
      next = it->second.start + speed;
    it->second.expect = next;
  }
  if (t60::tick30() && it != s_mo2T60AttackFrames.end())
    s_mo2T60AttackFrames.erase(it);
  return next;
}
/* the reversal's immediate extra play (a guard bounce) is one original frame of the animation: dt 1 */
static inline void mo2_t60_play_frame(mo2_class *i_this) {
  f32 previous = t60_set_dt(1.0f);
  i_this->mpMorf->play(&i_this->actor.eyePos, 0, 0);
  t60_set_dt(previous);
}

/* inlined into Mo2_move. true60: prev = mAttackFrame before this step's add, frameEnd = the value the
 * 30 Hz frame ends on (mo2_t60_attack_advance) */
static inline fopAc_ac_c *yari_hit_check(mo2_class *i_this, f32 prev, f32 frameEnd) {
  dComIfGp_get();
  i_this->mPrevAtPos.copy(i_this->mAtPos);
  i_this->m2940 = 0;
  i_this->mWeapon2Sph.SetR(0.0f);
  if (i_this->mAttackDir < 0) {
    return nullptr;
  }
  if (i_this->mAttackKind == 5) {
    i_this->mAtPos.copy(i_this->mRightHandPos);
  } else if (i_this->mAttackKind == 3 || i_this->mAttackKind == 4) {
    i_this->mAtPos.copy(i_this->mLeftHandPos);
  } else {
    i_this->mAtPos.copy(i_this->mSpearHeadPos);
    i_this->mAt2Pos.copy(i_this->mSpearShaftPos);
    i_this->mWeapon2Sph.SetR(62.5f);
  }
  mo2_t60_at_frame_start(i_this);
  if (i_this->mAtWaitTimer != 0) {
    /* true60 (the 30 Hz timer rule): a per-frame count, on the frame's first step (the read step) */
    if (bk_t60_first())
      i_this->mAtWaitTimer--;
    return nullptr;
  }
  /* true60: the At spheres are swept once per 30 Hz frame (with an unconverted
   * player they meet it only in the tick30 collision Move anyway): the attack keeps the original's 30 Hz grid. The frame's registration (start, or the swing since the last one, at the
   * frame-end attack frame) goes on the tick30 step; its hit is read, the window tested with the frame's end value and
   * the swing sound started on the next frame's first step, where the original frame reads them (as the Bokoblin). */
  const bool grid = bk_t60_at_grid();
  if (grid && !t60::first_step() && !t60::tick30()) {
    return nullptr;
  }
  const bool readStep = !grid || t60::first_step();
  const f32 frame = grid && t60::first_step() ? frameEnd : (f32)i_this->mAttackFrame;
  if (frame < i_this->mAtStartFrame || frame > i_this->mAtEndFrame) {
    return nullptr;
  }
  i_this->m2940 = (u8)(i_this->m2940 << 1);
  i_this->mWeaponSph.SetAtSpl((u8)moveAttackKind.values[i_this->mAttackKind]);
  i_this->mWeapon2Sph.SetAtSpl((u8)moveAttackKind.values[i_this->mAttackKind]);
  i_this->mWeaponSph.SetAtSe((u8)moveAttackSoundKind.values[i_this->mAttackKind]);
  i_this->mWeapon2Sph.SetAtSe((u8)moveAttackSoundKind.values[i_this->mAttackKind]);
  if (i_this->mAttackKind == 2 || i_this->mAttackKind == 1 || i_this->mAttackKind == 5) {
    i_this->mWeaponSph.SetAtType(AT_TYPE_UNK8);
    i_this->mWeapon2Sph.SetAtType(AT_TYPE_UNK8);
  } else {
    i_this->mWeaponSph.SetAtType(AT_TYPE_UNK800);
    i_this->mWeapon2Sph.SetAtType(AT_TYPE_UNK800);
  }
  /* true60: the swing sound once, on the step whose attack frame entered the start frame (the animation crossing-check rule) */
  if (readStep && (u32)gabi::ftoi(frame) == (u32)gabi::ftoi(i_this->mAtStartFrame) &&
      (grid || t60::dt() == 1.0f || gabi::ftoi(prev) != gabi::ftoi(i_this->mAtStartFrame) ||
       (prev == frame && t60::tick30()))) {
    s32 se = moveAttackSound.values[i_this->mAttackKind];
    if (i_this != nullptr) {
      fopAcM_monsSeStart(&i_this->actor, se, 0);
    }
  }
  if (grid) {
    if (t60::first_step()) {
      if (i_this->mAtStarted != 0 && (i_this->mWeaponSph.ChkAtHit() || i_this->mWeapon2Sph.ChkAtHit())) {
        void *obj;
        if (i_this->mWeaponSph.ChkAtHit()) {
          obj = GetAtHitObj(&i_this->mWeaponSph);
        } else {
          obj = GetAtHitObj(&i_this->mWeapon2Sph);
        }
        return hit_obj_actor(obj);
      }
      return nullptr;
    }
    if (i_this->mAtStarted == 0) {
      i_this->mAtStarted = 1;
      dCcD_Sph_StartCAt(&i_this->mWeaponSph, &i_this->mAtPos);
      dCcD_Sph_StartCAt(&i_this->mWeapon2Sph, &i_this->mAt2Pos);
    } else {
      dCcD_Sph_MoveCAt(&i_this->mWeaponSph, &i_this->mAtPos);
      dCcD_Sph_MoveCAt(&i_this->mWeapon2Sph, &i_this->mAt2Pos);
      dComIfG_Ccsp_Set(&i_this->mWeaponSph);
      dComIfG_Ccsp_Set(&i_this->mWeapon2Sph);
      if (i_this->mAttackKind == 2 || i_this->mAttackKind == 1) {
        dComIfG_Ccsp_SetMass(&i_this->mWeaponSph, 3);
        dComIfG_Ccsp_SetMass(&i_this->mWeapon2Sph, 3);
      }
    }
    return nullptr;
  }
  if (i_this->mAtStarted == 0) {
    i_this->mAtStarted = 1;
    dCcD_Sph_StartCAt(&i_this->mWeaponSph, &i_this->mAtPos);
    dCcD_Sph_StartCAt(&i_this->mWeapon2Sph, &i_this->mAt2Pos);
  } else {
    dCcD_Sph_MoveCAt(&i_this->mWeaponSph, &i_this->mAtPos);
    dCcD_Sph_MoveCAt(&i_this->mWeapon2Sph, &i_this->mAt2Pos);
    dComIfG_Ccsp_Set(&i_this->mWeaponSph);
    dComIfG_Ccsp_Set(&i_this->mWeapon2Sph);
    if (i_this->mAttackKind == 2 || i_this->mAttackKind == 1) {
      dComIfG_Ccsp_SetMass(&i_this->mWeaponSph, 3);
      dComIfG_Ccsp_SetMass(&i_this->mWeapon2Sph, 3);
    }
    if (i_this->mWeaponSph.ChkAtHit() || i_this->mWeapon2Sph.ChkAtHit()) {
      void *obj;
      if (i_this->mWeaponSph.ChkAtHit()) {
        obj = GetAtHitObj(&i_this->mWeaponSph);
      } else {
        obj = GetAtHitObj(&i_this->mWeapon2Sph);
      }
      return hit_obj_actor(obj);
    }
  }
  return nullptr;
}

/* true60 (True60 pair: the far Moblin 47C59A80's yaw +22 at f43, +88 from
 * f57, kept into its oshi: the body collider +0x2938 off from f87): fight's turn takes Link's direction as its target
 * while the attack frame is below the turn's end frame (mTurnEndFrame). A's frame f43 reads the frame's whole advance
 * (13 + 1 = 14, not below 14: the target stays f42's 2090); at N steps the first step read the in-between frame 13.5
 * and took that step's direction (2178), one half frame more of the turn toward a moving Link. The gate reads the frame
 * the 30 Hz frame ends on (mo2_t60_attack_advance's frameEnd; = the step's frame at dt = 1, the original). Negative
 * control WWHD_T60_MOPOSE_NEGCTRL=1 (game build: the step's own frame, the form before). */
static inline bool mo2_t60_pose_nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
  static const bool nc = [] { const char *e = getenv("WWHD_T60_MOPOSE_NEGCTRL"); return e && e[0] == '1'; }();
  return nc;
#else
  return false;
#endif
}

/* inlined into Mo2_move */
static inline void fight(mo2_class *i_this) {
  fopAc_ac_c *player = dComIfGp_getPlayer(0);
  i_this->mEyeAnmWait = 2;
  switch ((u16)i_this->mDamageReaction.mMode) {
  case 0:
    attack_set(i_this, 0);
    tex_anm_set(i_this, 6);
    i_this->mDamageReaction.mMode = 1;
    i_this->mTimers[2] = 8;
    mo2_t60_ai_ran(i_this); /* true60: mode 1's body runs in this call (fall-through): not held */
    /* fallthrough */
  case 1: {
    i_this->mWeaponSph.SetAtAtp((u8)moveAttackPower.values[i_this->mAttackKind]);
    i_this->mWeapon2Sph.SetAtAtp((u8)moveAttackPower.values[i_this->mAttackKind]);
    attack_info_s *info = moveAttackInfo.values[i_this->mAttackKind];
    f32 r3 = info[i_this->mAttackStep].speed;
    f32 attackPrev = i_this->mAttackFrame, attackEnd = attackPrev;
    i_this->mAttackFrame = mo2_t60_attack_advance(i_this, r3, &attackEnd); /* true60: + r3 x dt (the animation-rate rule) */
    cLib_addCalc2(&i_this->actor.speedF, 0.0f, 1.0f, 20.0f);
    if (i_this->mAttackKind == 3 && i_this->mAttackStep == 0) {
      i_this->actor.speedF = 30.0f;
    } else if (i_this->mAttackKind == 5 && i_this->mAttackStep == 1) {
      if (i_this->mAttackDir > 0) {
        i_this->actor.speedF = 70.0f;
      } else {
        i_this->actor.speedF = -30.0f;
      }
      i_this->mSmokeTimer = moveHIO.mSmokeTimerBase + 3;
      i_this->mSmokeType = 4;
    }
    if (i_this->mAttackFrame > i_this->mLookStartFrame &&
        daMo2_player_view_check(i_this,
                                &i_this->mDamageReaction.mpLookTarget->current.pos,
                                i_this->mTargetAngleY, moveHIO.mViewAngle)) {
      i_this->mDamageReaction.mLookMode = 1;
    }
    if (i_this->mAttackDir > 0) {
      if (i_this->mAttackKind == 1) {
        s32 r3 = moveHIO.mAttack1BlurStart + moveHIO.mAttack1BlurPreLen + moveHIO.mAttack1BlurBLen + moveHIO.mAttack1BlurPostLen;
        s32 t = gabi::ftoi(i_this->mAttackFrame);
        if (t >= moveHIO.mAttack1BlurStart && t <= r3) {
          i_this->mBlurOn = 1;
          t = gabi::ftoi(i_this->mAttackFrame);
          if (t >= moveHIO.mAttack1BlurStart + moveHIO.mAttack1BlurPreLen &&
              t < moveHIO.mAttack1BlurBLen + (moveHIO.mAttack1BlurStart + moveHIO.mAttack1BlurPreLen)) {
            i_this->mBlurUseB = 1;
            t = gabi::ftoi(i_this->mAttackFrame);
          }
          s32 iVar6 = t - moveHIO.mAttack1BlurStart;
          if (iVar6 < 10) {
            i_this->mBlurScaleX = moveHIO.mAttack1BlurScaleX[iVar6];
          }
        }
      } else if (i_this->mAttackKind == 2) {
        s32 r3 = moveHIO.mAttack2BlurStart + moveHIO.mAttack2BlurPreLen + moveHIO.mAttack2BlurBLen + moveHIO.mAttack2BlurPostLen;
        s32 t = gabi::ftoi(i_this->mAttackFrame);
        if (t >= moveHIO.mAttack2BlurStart && t <= r3) {
          i_this->mBlurOn = 1;
          t = gabi::ftoi(i_this->mAttackFrame);
          if (t >= moveHIO.mAttack2BlurStart + moveHIO.mAttack2BlurPreLen &&
              t < moveHIO.mAttack2BlurBLen + (moveHIO.mAttack2BlurStart + moveHIO.mAttack2BlurPreLen)) {
            i_this->mBlurUseB = 1;
            t = gabi::ftoi(i_this->mAttackFrame);
          }
          s32 iVar6 = t - moveHIO.mAttack2BlurStart;
          if (iVar6 < 10) {
            i_this->mBlurScaleX = moveHIO.mAttack2BlurScaleX[iVar6];
          }
        }
      }
    }
    if (i_this->mBlurOn == 0) {
      f32 br = moveBreakTime.values[i_this->mAttackKind];
      /* `>=` written as !(<): GHS branches on the negated comparison (NaN) */
      if (!(i_this->mAttackFrame < br) && !(i_this->mAttackFrame > br + 2.0f) &&
          i_this->mAttackKind == 2) {
        i_this->mSmokeTimer = moveHIO.mSmokeTimerBase + 0x10;
        i_this->mSmokeType = 0;
        i_this->mSmokeAngleY = i_this->actor.current.angle.y + REG0_S(8) + 0x2000;
      }
    }
    /* true60 (the one-emitter-per-event rule): the floor hit's smoke and sound per original frame in the window: on the tick30
     * step, with the frame-end attack frame */
    if (i_this->mAttackKind == 1 && !(i_this->mAttackFrame < 31.0f) &&
        !(i_this->mAttackFrame > 32.1f) && mo2_t60_frame_end_work(i_this)) {
      i_this->mSmokeTimer = moveHIO.mSmokeTimerBase + 8;
      i_this->mSmokeType = 2;
      s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(&i_this->actor));
      mDoAud_seStart(JA_SE_CM_LANCE_HIT_FLOOR, &i_this->mSpearHeadPos, 0, reverb);
    }
    /* true60 (the first-step AI-timer rule): the timer one-shot on the frame's first step (the only step the timer
     * changes) */
    if (i_this->mAttackKind == 2 && i_this->mTimers[2] == 1 && bk_t60_first()) {
      i_this->mSmokeTimer = moveHIO.mSmokeTimerBase + 4;
      i_this->mSmokeType = 4;
    }
    if (fopAcM_searchPlayerDistance(&i_this->actor) < 500.0f) {
      /* true60: the frame-end attack frame (above); dt = 1: the field, as the original reads it */
      const f32 t60TurnFrame =
          (t60::dt() == 1.0f || mo2_t60_pose_nc()) ? (f32)i_this->mAttackFrame : attackEnd;
      if (t60TurnFrame < i_this->mTurnEndFrame || i_this->mTurnAlways != 0) {
        i_this->mDamageReaction.mTurnTargetY = i_this->mTargetAngleY;
      }
      cLib_addCalcAngleS2(&i_this->actor.current.angle.y,
                          i_this->mDamageReaction.mTurnTargetY, 4, 0x800);
    }
    fopAc_ac_c *hitActor = yari_hit_check(i_this, attackPrev, attackEnd);
    if (hitActor != nullptr) {
      if (fpcM_GetName(hitActor) == fpcNm_PLAYER_e) {
        if (reinterpret_cast<mo2Move::Player *>(player)->checkPlayerGuard() &&
            i_this->mAttackKind != 3) {
          i_this->mpMorf->setPlaySpeed(-1.0f);
          if (i_this->mSmokeTimer != 0) {
            i_this->mSmokeTimer = moveHIO.mSmokeTimerBase + 6;
          }
          i_this->mAttackDir = -1;
          mo2_t60_play_frame(i_this); /* true60: the bounce's extra play is one frame */
        }
      } else if (fpcM_GetName(hitActor) == fpcNm_MO2_e) {
        i_this->mHitActorId = fopAcM_GetID(hitActor);
      }
    } else {
      i_this->mWeaponSph.ClrAtHit();
      i_this->mWeapon2Sph.ClrAtHit();
      if (i_this->m2940 != 0) {
        if (i_this->m2940 == 2) {
          i_this->mpMorf->setPlaySpeed(-1.0f);
          i_this->mBlurOn = 0;
          if (i_this->mSmokeTimer != 0) {
            i_this->mSmokeTimer = moveHIO.mSmokeTimerBase + 6;
          }
          i_this->mAttackDir = -1;
          mo2_t60_play_frame(i_this); /* true60: the bounce's extra play is one frame */
        } else {
          to_jyunkai(i_this, 2);
        }
        i_this->mSideStepDir = 1;
        i_this->mSideStepTimer = 0x10;
        gabi::Local<cXyz> local_38;
        local_38->x = local_38->y = local_38->z = 1.0f;
        dComIfGp_particle_set(0xC /* ID_AK_JN_NG */, &i_this->mClangEffectPos, nullptr,
                              local_38);
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(&i_this->actor));
        mDoAud_seStart(JA_SE_LK_ROPE_HOOK_METAL, &i_this->mClangEffectPos, 0, reverb);
      }
    }
    if (i_this->mpMorf->isStop()) {
      if ((i_this->mAttackStep == 2 && i_this->mAttackDir > 0) ||
          (i_this->mAttackKind == 4 && i_this->mAttackStep == 0) ||
          (i_this->mAttackDir < 0 && i_this->mAttackStep == 0) ||
          (i_this->mAttackKind == 5 && i_this->mAttackDir < 0 && i_this->mAttackStep == 1)) {
        /* true60: the attack's end reads Link's distance / view: decided on the tick30 step (the
         * stopped animation stays stopped over the frame, the held end, animation-driven decision) */
        if (!bk_t60_end()) {
        } else if (i_this->mAttackKind == 3) {
          attack_set(i_this, 1);
          i_this->mDamageReaction.mTurnTargetY = i_this->mTargetAngleY;
        } else if (i_this->mHitActorId != fpcM_ERROR_PROCESS_ID_e) {
          i_this->mDamageReaction.mAction = ACTION_AITE_MIRU;
          i_this->mDamageReaction.mMode = 0;
        } else if (i_this->mTargetDist < moveHIO.mFightDist) {
          if (daMo2_player_view_check(
                  i_this, &i_this->mDamageReaction.mpLookTarget->current.pos,
                  i_this->mTargetAngleY, moveHIO.mViewAngle)) {
            if (cM_rndF(1.0f) < 0.5f || i_this->mAttackDir < 0) {
              i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
              i_this->mDamageReaction.mMode = 2;
              i_this->mTimers[1] = 0;
            } else {
              i_this->mDamageReaction.mMode = 0;
            }
          } else if (has_weapon(i_this)) {
            i_this->mDamageReaction.mAction = ACTION_P_LOST;
            i_this->mDamageReaction.mMode = 0;
            i_this->mTimers[1] = 0;
          } else {
            to_jyunkai(i_this, -10);
          }
        } else {
          to_jyunkai(i_this, -10);
        }
      } else {
        attack_info_s *info = moveAttackInfo.values[i_this->mAttackKind];
        f32 fVar11;
        if (i_this->mAttackDir > 0) {
          i_this->mAttackStep++;
          fVar11 = info[i_this->mAttackStep].speed;
        } else {
          i_this->mAttackStep--;
          fVar11 = -info[i_this->mAttackStep].speed;
        }
        anm_init(i_this, info[i_this->mAttackStep].bckFileIdx, 0.0f,
                 J3DFrameCtrl::EMode_NONE, fVar11,
                 info[i_this->mAttackStep].soundFileIdx);
      }
    }
  }
  }
}

/* true60 (first-step input addendum; the process-order read rule: Link runs before the Moblin, so its reads of
 * Link take his current state on the tick30 step): the first step's sensing keeps the Moblin's frame-start position and
 * the sensed target (the heard sound's position or the look target); the tick30 step takes the sensing again from them,
 * with the target's current (frame-end) position, as A's frame computes it (the Darknut's form; the
 * Bokoblin's bk_t60_sense). Nothing when the frame's first step took no sensing. dt = 1: nothing. Host memory, this unit
 * only (split-actor state (1)). */
struct Mo2T60Sense {
  u32 frame, target;
  bool sound;
  f32 x, y, z;
};
static std::unordered_map<u32, Mo2T60Sense> s_mo2T60Sense;
static void mo2_t60_sense(mo2_class *i_this, bool sound) {
  if (t60::dt() == 1.0f)
    return;
  if (t60::first_step()) {
    Mo2T60Sense &e = s_mo2T60Sense[gabi::ea(i_this)];
    e.frame = bk_t60_frame_tag();
    e.sound = sound;
    e.target = gabi::ea(static_cast<fopAc_ac_c *>(i_this->mDamageReaction.mpLookTarget));
    e.x = i_this->actor.current.pos.x;
    e.y = i_this->actor.current.pos.y;
    e.z = i_this->actor.current.pos.z;
    return;
  }
  if (!t60::tick30())
    return;
  auto it = s_mo2T60Sense.find(gabi::ea(i_this));
  if (it == s_mo2T60Sense.end() || it->second.frame != bk_t60_frame_tag())
    return;
  const Mo2T60Sense &e = it->second;
  f32 tx, ty, tz;
  if (e.sound) {
    tx = i_this->mSoundTargetPos.x;
    ty = i_this->mSoundTargetPos.y;
    tz = i_this->mSoundTargetPos.z;
  } else {
    const fopAc_ac_c *target = gabi::at<fopAc_ac_c>(e.target);
    tx = target->current.pos.x;
    ty = target->current.pos.y;
    tz = target->current.pos.z;
  }
  const f32 x = tx - e.x, y = ty - e.y, z = tz - e.z;
  i_this->mTargetDist = std_sqrtf(gabi::fmadds(x, x, z * z));
  i_this->mTargetDistY = std::fabs(y);
  i_this->mTargetAngleY = cM_atan2s(x, z);
}

/* 021D0A14 */
static void Mo2_move(mo2_class *i_this) {
  dComIfGp_get();
  /* true60: a state entered on an earlier step of this 30 Hz frame is first run next frame (one state
   * step per frame, d_a_mo2_t60.h mo2_t60_ai_held): its remaining steps keep the outputs of the frame's dispatch (move
   * angle offset, look mode, guard pose, speed) and skip the decisions; the speed is still turned into the motion */
  const bool t60Held = mo2_t60_ai_held(i_this);
  s_mo2T60EntryState = mo2_t60_ai_state(i_this);
  s_mo2T60ChangedBefore = mo2_t60_changed_before_move(i_this);
  if (!t60Held) {
    i_this->mDamageReaction.mLookMode = 0;
    i_this->mDamageReaction.mDefencePose = 0;
    i_this->mMoveAngleOffset = 0;
  }
  cLib_addCalcAngleS2(&i_this->mJawAngleZ, 0, 2, 0x800);
  if (i_this->mDamageReaction.mMode <= -0x64) {
    /* true60: the knock-down entered on an earlier step of this frame is first run next frame (held) */
    if (t60Held)
      return;
    if (i_this->mDamageReaction.mAcch.ChkGroundLanding()) {
      tex_anm_set(i_this, 0);
    }
    i_this->mEyeAnmWait = 2;
    i_this->mNoHitTimer = 5;
    if (std::fabs(i_this->mDamageReaction.mKnockbackSpeed) > 40.0f &&
        !i_this->mDamageReaction.mAcch.ChkGroundHit()) {
      i_this->mWeaponSph.SetC(&i_this->mDamageReaction.mJntPos[0xc]);
      i_this->mWeaponSph.SetR(60.0f);
      i_this->mWeaponSph.OffAtSPrmBit(4); /* OffAtVsPlayerBit */
      i_this->mWeaponSph.SetAtSpl(1);
      i_this->mWeaponSph.OnCoSPrmBit(1); /* OnCoSetBit */
      dComIfG_Ccsp_Set(&i_this->mWeaponSph);
      dComIfG_Ccsp_SetMass(&i_this->mWeaponSph, 3);
      if (i_this->mWeaponSph.ChkAtHit() && i_this->actor.speed.y < -50.0f) {
        i_this->actor.speed.y = 0.0f;
        i_this->mDamageReaction.mShakeAmplitude = 8000.0f;
      }
      i_this->mCoCyl.OffCoSPrmBit(1);
    }
    return;
  }
  i_this->mWeaponSph.OnAtSPrmBit(4); /* OnAtVsPlayerBit */
  if (i_this->mDefeated == 0) {
    i_this->mCoCyl.OnCoSPrmBit(1);
  } else {
    i_this->mCoCyl.OffCoSPrmBit(1);
  }
  if (t60Held) {
    /* true60: held, no decisions on this step */
  } else if (i_this->mDamageReaction.mHitStunTimer != 0 && i_this->mDamageReaction.mKnockedDown == 0) {
    if (i_this->mDamageReaction.mHitStunTimer == 1) {
      if ((i_this->mMode != 1 || moveJailMode == 0) &&
          (i_this->mDamageReaction.mAction != ACTION_P_LOST ||
           i_this->mTimers[1] == 0)) {
        i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
        i_this->mDamageReaction.mMode = 0;
        i_this->mTimers[1] = 0;
      }
    } else {
      i_this->mDamageReaction.mTurnTargetY = i_this->mTargetAngleY;
      i_this->actor.speedF = -30.0f;
      i_this->mSmokeTimer = moveHIO.mSmokeTimerBase + 3;
      i_this->mSmokeType = 4;
    }
  } else {
    gabi::Local<cXyz> local_20;
    const bool t60Sound = i_this->mSoundTargetTimer != 0; /* true60: the target this call senses */
    if (i_this->mSoundTargetTimer != 0) {
      /* true60 (the 30 Hz timer rule): a per-frame count, on the tick30 step (the sound target is followed until it
       * runs out) */
      if (bk_t60_end())
        i_this->mSoundTargetTimer--;
      gabi::Local<cXyz> d;
      cXyz_mi(&i_this->mSoundTargetPos, d, &i_this->actor.current.pos);
      *local_20 = d->get();
    } else {
      i_this->mDamageReaction.mpLookTarget = dComIfGp_getPlayer(0); /* search_target */
      gabi::Local<cXyz> d;
      cXyz_mi(&i_this->mDamageReaction.mpLookTarget->current.pos, d,
              &i_this->actor.current.pos);
      *local_20 = d->get();
    }
    /* true60 (the first-step AI-timer rule; the Bokoblin's form): the frame's sensing (distance,
     * height and direction to the target) on the frame's FIRST step, as the original's call takes it before the frame's
     * motion: the decisions that read it cannot cross on a later step. dt = 1: every call (the original). */
    if (bk_t60_first()) {
    i_this->mTargetDist = std_sqrtf(
        gabi::fmadds(local_20->x, local_20->x, local_20->z * local_20->z));
    i_this->mTargetDistY = std::fabs(
        (f32)local_20->y); // HD: target height also limits this transition.

    i_this->mTargetAngleY = cM_atan2s(local_20->x, local_20->z);
    }
    mo2_t60_sense(i_this, t60Sound); /* true60: the tick30 step's sensing (first-step input addendum / process-order read) */
    i_this->mCoCyl.SetR(REG0_F(3) + 90.0f);
    /* true60 (the 30 Hz timer rule): counted on the frame's first step (damage_check's set on another step starts one
     * lower, mo2_exec) */
    if (i_this->mParalyzeDelay != 0 && bk_t60_first()) {
      i_this->mParalyzeDelay--;
      if (i_this->mParalyzeDelay == 0) {
        i_this->mDamageReaction.mCarryOffsetZ =
            0.0f; // HD: target height also limits this transition.

        i_this->mDamageReaction.mCarryOffsetY = 0.0f;
        i_this->mDamageReaction.mAction = ACTION_D_MAHI;
        i_this->mDamageReaction.mMode = 0;
        i_this->actor.current.angle.z = 0;
        i_this->actor.current.angle.x = 0;
      }
    }
    i_this->mDamageReaction.mStts.SetWeight(
        200); // HD: target height also limits this transition.

    switch ((u16)i_this->mDamageReaction.mAction) {
    case ACTION_JYUNKAI:
      jyunkai(i_this);
      break;
    case ACTION_FIGHT_RUN:
      fight_run(i_this);
      break;
    case ACTION_FIGHT:
      fight(i_this);
      break;
    case ACTION_NAGE:
      nage(i_this);
      break;
    case ACTION_DEFENCE:
      defence(i_this);
      break;
    case ACTION_OSHI:
      oshi(i_this);
      break;
    case ACTION_P_LOST:
      p_lost(i_this);
      break;
    case ACTION_B_NIGE:
      b_nige(i_this);
      break;
    case ACTION_HUKKI:
      hukki(i_this);
      break;
    case ACTION_WEPON_SEARCH:
      wepon_search(i_this);
      break;
    case ACTION_HIP_DAMAGE:
      hip_damage(i_this);
      break;
    case ACTION_AITE_MIRU:
      aite_miru(i_this);
      break;
    case ACTION_FAIL:
      fail(i_this);
      break;
    case ACTION_YOGAN_FAIL:
      yogan_fail(i_this);
      break;
    case ACTION_CARRY:
      carry(i_this);
      break;
    case ACTION_CARRY_DROP:
      carry_drop(i_this);
      break;
    case ACTION_D_SIT:
      d_sit(i_this);
      break;
    case ACTION_D_MAHI:
      d_mahi(i_this);
      i_this->mDamageReaction.mStts.SetWeight(
          0xFF); // HD: stunned and frozen Moblins have maximum collision
                 // weight.

      break;
    case ACTION_D_DOZOU:
      d_dozou(i_this);
      i_this->mDamageReaction.mStts.SetWeight(
          0xFF); // HD: stunned and frozen Moblins have maximum collision
                 // weight.

      break;
    case ACTION_E3_DEMO:
      e3_demo(i_this);
      break;
    }
  }
  if (i_this->mDamageReaction.mAction != ACTION_CARRY) {
    gabi::Local<cXyz> local_20;
    gabi::Local<cXyz> local_2c;
    local_20->y = 0.0f;
    local_20->x = 0.0f;
    local_20->z = i_this->actor.speedF;
    if (i_this->mDamageReaction.mAction != ACTION_HUKKI &&
        i_this->mDamageReaction.mAction != ACTION_FAIL &&
        i_this->mDamageReaction.mHitStunTimer == 0) {
      i_this->mDamageReaction.mKnockbackAngleY = i_this->actor.current.angle.y;
      cMtx_YrotS(calc_mtx(), i_this->actor.current.angle.y + i_this->mMoveAngleOffset);
    } else {
      cMtx_YrotS(calc_mtx(), i_this->mDamageReaction.mTurnTargetY);
    }
    MtxPosition(local_20, local_2c);
    i_this->actor.speed.x = local_2c->x;
    i_this->actor.speed.z = local_2c->z;
  }
}

// clang-format off
/* WWHD glue
str 10014EE0 "MO2 BACK KABE HIT!"
str 10014D84 "Hyrule"
str 10014ECC "i_this->ppd != (0)"
str 10014EAC "d_a_mo2.cpp"
ext 021C6368 void tex_anm_set(mo2Move::mo2_class* i, u16 idx)
ext 021C6440 void anm_init(mo2Move::mo2_class* i, int bck, f32 morf, u8 loopMode, f32 speed, int snd)
ext 021C7C30 void way_pos_check(mo2Move::mo2_class* i, cXyz* p)
ext 021C7EDC u32 ground_4_check(mo2Move::mo2_class* i, int n, s16 a, f32 r)
ext 021C872C s32 daMo2_wepon_view_check(mo2Move::mo2_class* i)
ext 021C89FC s32 daMo2_bomb_view_check(mo2Move::mo2_class* i)
ext 021C8A38 s32 daMo2_player_bg_check(mo2Move::mo2_class* i, cXyz* p)
ext 021C8C14 BOOL daMo2_player_view_check(mo2Move::mo2_class* i, cXyz* p, s16 a, s16 b)
ext 021C8DD4 s32 daMo2_player_way_check(mo2Move::mo2_class* i)
ext 021C8E30 void wait_set(mo2Move::mo2_class* i)
ext 021C8FB8 void fight_run_set(mo2Move::mo2_class* i)
ext 021C9004 void path_check(mo2Move::mo2_class* i)
ext 021C9310 void attack_set(mo2Move::mo2_class* i, u8 p)
ext 021CD958 void nage(mo2Move::mo2_class* i)
ext 021CDE6C void p_lost(mo2Move::mo2_class* i)
ext 021CE138 void b_nige(mo2Move::mo2_class* i)
ext 021CE46C void defence(mo2Move::mo2_class* i)
ext 021CE5B8 void oshi(mo2Move::mo2_class* i)
ext 021CE6E8 void hukki(mo2Move::mo2_class* i)
ext 021CEC44 void aite_miru(mo2Move::mo2_class* i)
ext 021CED78 void fail(mo2Move::mo2_class* i)
ext 021CEEB0 void yogan_fail(mo2Move::mo2_class* i)
ext 021CF0F4 void wepon_search(mo2Move::mo2_class* i)
ext 021CF900 void hip_damage(mo2Move::mo2_class* i)
ext 021CFBBC void d_mahi(mo2Move::mo2_class* i)
ext 021CFFA4 void d_sit(mo2Move::mo2_class* i)
ext 021D0064 void d_dozou(mo2Move::mo2_class* i)
ext 021D0284 void carry(mo2Move::mo2_class* i)
ext 021D0294 void carry_drop(mo2Move::mo2_class* i)
ext 021D0570 void e3_demo(mo2Move::mo2_class* i)
ext 0200796C f32 CPad_GET_STICK_POS_X(u32 port)
ext 0255F530 u8* dKy_Sound_get()
ext 02518B28 u32 cc_pl_cut_bit_get()
ext 024EDFCC BOOL dAttention_LockonTruth(mo2Move::Attention* att)
ext 024EC8D0 fopAc_ac_c* dAttention_LockonTarget(mo2Move::Attention* att, s32 i)
ext 025167C0 void dCcD_Sph_StartCAt(dCcD_Sph* s, cXyz* p)
ext 025167E4 void dCcD_Sph_MoveCAt(dCcD_Sph* s, cXyz* p)
ext 02516178 void* GetAtHitObj(dCcD_GObjInf* o)
ext 02516C14 void moveSetMass(void*, void*, u8)
ext 027EC9E8 void moveDebugPrint(s32, s32, const char*)
data moveHIO 104654E0
data moveSearchSpecial 104654A4
data moveJailMode 101BA692
data moveAttackPower 101BA6F4
data moveBreakTime 101BA70C
data moveAttackKind 101BA694
data moveAttackSoundKind 101BA6AC
data moveAttackSound 101BA6DC
data moveAttackInfo 101BA724
*/
