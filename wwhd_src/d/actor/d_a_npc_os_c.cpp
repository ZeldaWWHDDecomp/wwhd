/**
 * d_a_npc_os_c.cpp (WWHD)
 * NPC - Os (the stone-head helper of the Earth/Wind temples): NPC actions, route checks, player actions
 * (part C).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_os.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_os_c.h"

// HD: part A owns the action descriptors; this view introduces no duplicate data object.
static const OsRouteActionsHD* os_actions() { return gabi::at<const OsRouteActionsHD>(0x1001F260); }

/* dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (copy), s16 yrot, s16 vel, bool headOnly) */
static inline void os_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot,
                                   s16 vel, u32 headOnly) {
    Os_lookAtTargetHD(j, outY, target, eye, yrot, vel, headOnly);
}
// HD: retain the floating-point copy of the zero vector.
static inline void os_lookAtZero(daNpc_Os_c* t, be<s16>* outY, s16 yrot, s16 vel, u32 headOnly) {
    gabi::Local<cXyz> e;
    cXyz* z = gabi::at<cXyz>(0x101FFBA8);
    e->x = z->x;
    e->y = z->y;
    e->z = z->z;
    os_lookAtTarget(&t->mJntCtrl, outY, nullptr, e, yrot, vel, headOnly);
}
static inline void os_pmf_npc(daNpc_Os_c* t, const ProcFunc_l* pmf, void* arg) {
    gabi::Local<ProcFunc_l> fn;
    Os_copyRouteActionHD(fn, pmf);
    os_setNpcAction(t, fn, arg);
}
static inline void os_pmf_player(daNpc_Os_c* t, const ProcFunc_l* pmf, void* arg) {
    gabi::Local<ProcFunc_l> fn;
    Os_copyRouteActionHD(fn, pmf);
    os_setPlayerAction(t, fn, arg);
}
/* mpMorf->isMorf(): morf ratio (+0xB0) < 1.0 */
static inline u32 os_isMorf(daNpc_Os_c* t) { return t->mpMorf->mCurMorf < 1.0f; }
/* chkAttention(current.pos (copy), shape + head_y + backbone_y) */
static inline u32 os_chkAttentionHere(daNpc_Os_c* t) {
    s16 angle = (s16)(t->shape_angle.y + t->mJntCtrl.mAngles[0][1] + t->mJntCtrl.mAngles[1][1]);
    gabi::Local<cXyz> pos;
    pos->x = t->current.pos.x;
    pos->y = t->current.pos.y;
    pos->z = t->current.pos.z;
    return os_chkAttention(t, pos, angle);
}
/* fopAcM_seStartCurrent (HD inline: actor known non-null) */
static inline void os_seStartCurrent(daNpc_Os_c* t, u32 id) {
    s32 reverb = dComIfGp_getReverb(t->current.roomNo);
    mDoAud_seStart(id, &t->current.pos, 0, reverb);
}
/* CPad_GET_STICK_VALUE(0) (f1 unrounded) */
static inline f64 os_stickValue() { return Os_stickValueHD(0); }
enum : u32 { JA_SE_OBJ_OSTATUE_PUT = 0x58FE, ID_AK_ST_OTOMOSMOKE00 = 0xA328, ID_AK_ST_OTOMOSMOKE01 = 0xA33B };

/* talkNpcAction's common tail */
static inline void os_talkTail(daNpc_Os_c* t) {
    t->mJntCtrl.mbTrn = 1; /* setTrn */
    os_lookBack(t, t->field_0x7A4, 0, 0);
    t->current.angle.y = t->shape_angle.y;
    os_setAttention(t, os_isMorf(t));
}

/* this TU's dBgS_GndChk / dBgS_LinChk vtables */
static const dBgS_GndChk_vt OS_GNDCHK_VT = {0x1001F3A4, 0x1001F3B4, 0x1001F3D4, 0x1001F3C4};
static const dBgS_LinChk_vt OS_LINCHK_VT = {0x1001F3E4, 0x1001F3F4, 0x1001F414, 0x1001F404};
/* ~dBgS_GndChk (inline vtable stores, then cBgS_Chk's destructor) */
static inline void os_gndChk_dt(OsGroundCheckHD* storage) {
    dBgS_GndChk* chk = &storage->mCheck;
    chk->cBgS_PolyInfo::__vtbl = OS_GNDCHK_VT.v20;
    chk->dBgS_PolyPassChk::__vtbl = OS_GNDCHK_VT.v40;
    chk->dBgS_GrpPassChk::__vtbl = Os_groupPassVtableHD;
    Os_groundCheckDestructHD(chk, 0); /* cBgS_Chk::~cBgS_Chk */
}
/* ~dBgS_LinChk (inline vtable stores, then cBgS_LinChk's destructor) */
static inline void os_linChk_dt(OsLineCheckHD* storage) {
    dBgS_LinChk* chk = &storage->mCheck;
    chk->dBgS_PolyPassChk::__vtbl = OS_LINCHK_VT.v58;
    chk->dBgS_GrpPassChk::__vtbl = Os_groupPassVtableHD;
    chk->cBgS_PolyInfo::__vtbl = Os_polyInfoVtableHD;
    cBgS_LinChk_dt(chk, 0);
}

/* lookAtTarget(&temp2, &temp, current.pos (copy), shape_angle.y, mNpc.mMaxTurnStep, false) */
static inline void os_lookAtStick(daNpc_Os_c* t, be<s16>* temp2, cXyz* temp) {
    gabi::Local<cXyz> eye;
    eye->x = t->current.pos.x;
    eye->y = t->current.pos.y;
    eye->z = t->current.pos.z;
    s16 yrot = t->shape_angle.y;
    *temp2 = yrot;
    os_lookAtTarget(&t->mJntCtrl, temp2, temp, eye, yrot, Os_actionHioHD()->mNpc.mMaxTurnStep, 0);
}

/* 022AE504 */
BOOL daNpc_Os_c::waitNpcAction(void* param_1) {
    if (field_0x7A9 == 0) {
        maxFallSpeed = -100.0f;
        gravity = Os_actionHioHD()->field_0x8C;
        speedF = 0.0f;
        field_0x788 = 120.0f;
        field_0x7A9 = field_0x7A9 + 1;
        return TRUE;
    }
    if (field_0x7A9 == -1) {
        return TRUE;
    }
    if (field_0x78C != 0 && mReachedAnimEnd != 0) {
        os_setAnm(this, 0);
    }

    if (field_0x7A9 == 1) {
        if (os_wakeupCheck(this) && dComIfGp_getCb1Player() == this) {
            if (mNoResetFlg1 & 0x40 /* checkNpcNotChange() */) {
                mNoResetFlg1 &= ~0x40u;
                os_initBrkAnm(this, 4, 1);
                field_0x7A1 = 0;
                field_0x7A9 = field_0x7A9 + 1;
            } else {
                field_0x7A9 = field_0x7A9 + 1;
            }
        }
    } else {
        attention_info.flags |= fopAc_Attn_ACTION_CARRY_e;
        field_0x7A4 = (u8)os_chkAttentionHere(this);
        if (os_chkArea(this, &current.pos)) {
            os_lookBack(this, field_0x7A4, 0, 1);
        }
        current.angle.y = shape_angle.y;
        if (os_finishCheck(this)) {
            os_pmf_npc(this, &os_actions()->mFinish01Npc, nullptr);
        }
    }

    attention_info.flags &= ~u32(fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e);
    f32 dist_sq = fopAcM_searchActorDistance2(this, dComIfGp_getPlayer(0));
    if (!(mNoResetFlg1 & 2) /* !checkNpcCallCommand() */) {
        f32 r = Os_actionHioHD()->field_0x64;
        if (dist_sq < r * r) {
            fopAc_ac_c* link = dComIfGp_getLinkPlayer();
            reinterpret_cast<daPy_py_l*>(link)->mNoResetFlg1 |= 2; /* onNpcCall */
        }
    } else if (os_wakeupCheck(this)) {
        f32 r = Os_actionHioHD()->field_0x60;
        if (!(dist_sq < r * r)) {
            os_pmf_npc(this, &os_actions()->mSearchNpc, nullptr);
        }
    } else {
        field_0x7A5 = os_getWakeupOrderEventNum(this);
    }
    os_setAttention(this, 1);
    return TRUE;
}

/* 022AE834 */
BOOL daNpc_Os_c::finish01NpcAction(void* param_1) {
    if (field_0x7A9 == 0) {
        if (dComIfGp_getCb1Player() == this) {
            mNoResetFlg1 |= 0x40; /* onNpcNotChange */
            Os_clearCbPlayerHD(); /* dComIfGp_setCb1Player(NULL) */
        }
        os_setAnm(this, 0);
        os_initBrkAnm(this, 0, 1);
        field_0x7A1 = 0;
        maxFallSpeed = -100.0f;
        gravity = Os_actionHioHD()->field_0x8C;
        speedF = 0.0f;
        field_0x788 = 120.0f;
        field_0x7A9 = field_0x7A9 + 1;
        return TRUE;
    }
    if (field_0x7A9 != -1) {
        os_lookAtZero(this, &shape_angle.y, shape_angle.y, field_0x798, 1);
        if (field_0x7A1 != 0) {
            os_pmf_npc(this, &os_actions()->mFinish02Npc, nullptr);
        }
        os_setAttention(this, 1);
    }
    return TRUE;
}

/* 022AE980 */
BOOL daNpc_Os_c::finish02NpcAction(void* param_1) {
    if (field_0x7A9 == 0) {
        if (param_1 == nullptr) {
            s32 anm = 1;
            if (dComIfGs_isEventBit(0x1B01)) {
                anm = 7;
            }
            os_setAnm_brkAnm(this, anm);
        }
        field_0x784 |= 1; /* onFinish */
        maxFallSpeed = -100.0f;
        gravity = Os_actionHioHD()->field_0x8C;
        field_0x788 = 120.0f;
        speedF = 0.0f;
        field_0x7A9 = field_0x7A9 + 1;
        return TRUE;
    }
    if (field_0x7A9 != -1) {
        os_lookAtZero(this, &shape_angle.y, shape_angle.y, field_0x798, 1);
    }
    return TRUE;
}

/* 022AEA8C */
BOOL daNpc_Os_c::talkNpcAction(void* param_1) {
    if (field_0x7A9 == 0) {
        *gabi::at<be<u32>>(0x10467DF0) = fpcM_ERROR_PROCESS_ID_e;
        field_0x780 = os_getMsg(this);
        field_0x7A9 = field_0x7A9 + 1;
        attention_info.flags &= ~u32(fopAc_Attn_ACTION_CARRY_e);
        field_0x7A3 = 0;
        return TRUE;
    }
    if (field_0x7A9 == -1) {
        return TRUE;
    }
    field_0x7A4 = (u8)os_chkAttentionHere(this);
    if (field_0x7A9 == 1) {
        if (os_talk_init(this)) {
            field_0x7A9 = 2;
        }
    } else if (field_0x7A9 == 2) {
        if (os_talk(this)) {
            os_pmf_npc(this, &os_actions()->mWaitNpc, nullptr);
            dComIfGp_event_reset();
        }
    } else if (field_0x7A9 == 3) {
        os_pmf_npc(this, &os_actions()->mWaitNpc, nullptr);
    }
    os_talkTail(this);
    return TRUE;
}

// HD: a roof hit without a wall throws towards the current angle.
/* 022AECD4 */
BOOL daNpc_Os_c::carryNpcAction(void* param_1) {
    if (field_0x7A9 == 0) {
        os_setAnm(this, 0);
        attention_info.flags &= ~u32(fopAc_Attn_ACTION_CARRY_e);
        mNoResetFlg1 &= ~2u; /* offNpcCallCommand */
        fopAc_ac_c* pl = dComIfGp_getPlayer(0);
        s16 a = shape_angle.y;
        field_0x788 = 120.0f;
        field_0x7A9 = field_0x7A9 + 1;
        field_0x7AC = (s16)(a - pl->shape_angle.y); /* fopAcM_toPlayerShapeAngleY */
        return TRUE;
    }
    if (field_0x7A9 == -1) {
        field_0x7E4.y = 0.0f;
        field_0x7E4.z = 0.0f;
        return TRUE;
    }
    os_setRestart(this, os_getRestartNumber(this));
    dComIfGp_get(); /* HD: a dead dComIfGp_get() before the HIO reads */
    cLib_chaseF(&field_0x7E4.x, Os_actionHioHD()->field_0x90, 1.0f);
    cLib_chaseF(&field_0x7E4.y, Os_actionHioHD()->field_0x94, 1.0f);
    cLib_chaseF(&field_0x7E4.z, Os_actionHioHD()->field_0x98, 1.0f);

    s32 wallHit = os_wallHitCheck(this);
    if (wallHit >= 0 || ((u32)mAcch.m_flags & 0x200) /* mAcch.ChkRoofHit() */) {
        fopAcM_cancelCarryNow(this);
        gabi::Local<be<s16>> temp;
        *temp = current.angle.y;
        if (wallHit >= 0) {
            *temp = mAcchCir[wallHit].GetWallAngleY();
        }
        os_pmf_npc(this, &os_actions()->mThrowNpc, temp.get());
        return TRUE;
    }
    if (!(actor_status & fopAcStts_CARRY_e) /* !fopAcM_checkCarryNow */) {
        if (speedF > 0.0f) {
            os_pmf_npc(this, &os_actions()->mThrowNpc, nullptr);
            return TRUE;
        }
        os_seStartCurrent(this, JA_SE_OBJ_OSTATUE_PUT);
        os_smokeSet(this, ID_AK_ST_OTOMOSMOKE00);
        os_pmf_npc(this, &os_actions()->mWaitNpc, nullptr);
        return TRUE;
    }
    os_lookAtZero(this, &shape_angle.y, shape_angle.y, 0, 0);
    current.angle.y = shape_angle.y;
    return TRUE;
}

/* 022AEF70 */
BOOL daNpc_Os_c::throwNpcAction(void* param_1) {
    if (field_0x7A9 == 0) {
        if (param_1 != nullptr) {
            speedF = 8.0f;
            speed.y = 0.0f;
            current.angle.y = *(be<s16>*)param_1;
        } else {
            speedF = Os_actionHioHD()->field_0x80;
            speed.y = Os_actionHioHD()->field_0x84;
            current.angle.y = shape_angle.y;
        }
        maxFallSpeed = -100.0f;
        gravity = Os_actionHioHD()->field_0x8C;
        field_0x7A9 = field_0x7A9 + 1;
        return TRUE;
    }
    if (field_0x7A9 != -1) {
        if ((u32)mAcch.m_flags & 0x20 /* mAcch.ChkGroundHit() */) {
            os_seStartCurrent(this, JA_SE_OBJ_OSTATUE_PUT);
            os_smokeSet(this, ID_AK_ST_OTOMOSMOKE01);
            os_pmf_npc(this, &os_actions()->mWaitNpc, nullptr);
        }
        os_setAttention(this, 1);
    }
    return TRUE;
}

/* 022AFBF4 */
BOOL daNpc_Os_c::jumpNpcAction(void* param_1) {
    if (field_0x7A9 == 0) {
        os_setAnm(this, 0);
        if (param_1 != nullptr) {
            speed.y = *static_cast<be<f32>*>(param_1);
        }
        speedF = 50.0f;
        maxFallSpeed = -100.0f;
        gravity = Os_actionHioHD()->field_0x8C;
        field_0x7A9 = field_0x7A9 + 1;
        return TRUE;
    }
    if (field_0x7A9 != -1) {
        if ((u32)mAcch.m_flags & 0x20 /* mAcch.ChkGroundHit() */) {
            os_smokeSet(this, ID_AK_ST_OTOMOSMOKE01);
            os_pmf_npc(this, &os_actions()->mWaitNpc, nullptr);
        }
        os_setAttention(this, 1);
    }
    return TRUE;
}

/* 022AF2F0 */
void daNpc_Os_c::routeAngCheck(cXyz* param_1, be<s16>* param_2) {
    gabi::Local<cXyz> cross;
    Os_crossProductHD(&field_0x7F0, cross.get(), param_1); /* field_0x7F0.outprod(*param_1) */
    s16 angle = cM_atan2s(cross->x, cross->z);
    bool flip;
    if (!(field_0x7F0.y < 1.0f) && cLib_distanceAngleS(angle, *param_2) > 0x4000) {
        flip = true;
    } else {
        fopAc_ac_c* pl = dComIfGp_getPlayer(0);
        f32 y = current.pos.y;
        f32 dy = pl->current.pos.y - y; /* fopAcM_searchPlayerDistanceY */
        flip = cross->y * dy < 0.0f;
    }
    if (flip) {
        angle = (s16)(angle - 0x8000);
    }
    *param_2 = angle;
}

/* 022AF3AC */
void daNpc_Os_c::routeWallCheck(cXyz* param_1, cXyz* param_2, be<s16>* param_3) {
    gabi::Local<OsLineCheckHD> lin_chk;
    dBgS_LinChk_ct(lin_chk, OS_LINCHK_VT, false);
    dBgS_LinChk_Set(lin_chk, param_1, param_2, nullptr);
    if (cBgS_LineCross(dComIfG_Bgsp(), lin_chk)) {
        dBgS* bgs = dComIfG_Bgsp();
        u16 poly = lin_chk->mCheck.mPolyIndex;
        u16 bg = lin_chk->mCheck.mBgIndex;
        void* pla = cBgS_GetTriPla(bgs, bg, poly);
        if (pla) {
            routeAngCheck((cXyz*)pla, param_3); /* &plane->mNormal */
        }
    }
    os_linChk_dt(lin_chk);
}

/* 022AF0B4 */
f32 daNpc_Os_c::checkForwardGroundY(s16 param_1) {
    s32 wallHit = os_wallHitCheck(this);
    if (wallHit >= 0) {
        dBgS* bgs = dComIfG_Bgsp();
        dBgS_AcchCir* cir = &mAcchCir[wallHit];
        cXyz* pla = static_cast<cXyz*>(cBgS_GetTriPla(bgs, cir->mBgIndex, cir->mPolyIndex));
        if (pla) {
            s16 a = cM_atan2s(pla->x, pla->z);
            if (cLib_distanceAngleS(param_1, a) > 0x4000) {
                gabi::Local<OsGroundCheckHD> gnd_chk;
                dBgS_GndChk_ct(gnd_chk, OS_GNDCHK_VT, false);
                dBgS_GndChk* g = &gnd_chk->mCheck;
                g->mFlag &= ~2u; /* OffWall */
                f32 x = gabi::fmadds(80.0f, cM_ssin(param_1), current.pos.x);
                f32 y = current.pos.y + 80.0f;
                f32 z = gabi::fmadds(80.0f, cM_scos(param_1), current.pos.z);
                g->m_pos.x = x; /* SetPos */
                g->m_pos.y = y;
                g->m_pos.z = z;
                f32 r = cBgS_GroundCross(dComIfG_Bgsp(), gnd_chk);
                os_gndChk_dt(gnd_chk);
                return r;
            }
        }
    }
    return -1e+7f;
}

/* 022AF270 */
f32 daNpc_Os_c::checkWallJump(s16 param_1) {
    f64 g = Os_forwardGroundUnroundedHD(this, param_1); /* checkForwardGroundY (f1 unrounded) */
    f32 delta = (f32)(g - (f64)(f32)current.pos.y);
    if (0.0f < delta && delta < 80.0f) {
        f64 s = Os_sqrtUnroundedHD(delta); /* std::sqrtf */
        return (f32)(s * (f64)3.6f);
    }
    return -1.0f;
}

/* 022AF4C8 */
BOOL daNpc_Os_c::routeCheck(f32 param_1, be<s16>* param_2) {
    if (!((u32)mAcch.m_flags & 0x20) /* !mAcch.ChkGroundHit() */) {
        current.pos.copy(old.pos);
        speedF = 0.0f;
        field_0x7A8 = 1;

        gabi::Local<OsGroundCheckHD> gnd_chk;
        dBgS_GndChk_ct(gnd_chk, OS_GNDCHK_VT, false);
        dBgS_GndChk* g = &gnd_chk->mCheck;
        g->mFlag &= ~2u; /* OffWall */
        s16 a = *param_2;
        gabi::Local<cXyz> temp;
        temp->x = gabi::fmadds(80.0f, cM_ssin(a), current.pos.x);
        temp->y = current.pos.y + 80.0f;
        temp->z = gabi::fmadds(80.0f, cM_scos(a), current.pos.z);
        g->m_pos.x = temp->x; /* SetPos */
        g->m_pos.y = temp->y;
        g->m_pos.z = temp->z;

        f32 gy = cBgS_GroundCross(dComIfG_Bgsp(), gnd_chk);
        f32 y = current.pos.y;
        if (gy - y > -100.0f) {
            if (cLib_distanceAngleS(current.angle.y, *param_2) < 0x800) {
                os_pmf_npc(this, &os_actions()->mJumpNpc, nullptr);
            }
            os_gndChk_dt(gnd_chk);
            return TRUE;
        }
        if (param_1 > 360000.0f) {
            os_gndChk_dt(gnd_chk);
            return FALSE;
        }
        temp->y = y - 80.0f;
        routeWallCheck(temp, &current.pos, param_2);
        os_gndChk_dt(gnd_chk);
        return TRUE;
    }

    if ((u32)mAcch.m_flags & 0x10 /* mAcch.ChkWallHit() */) {
        f64 jump = Os_wallJumpUnroundedHD(this, (s16)*param_2); /* checkWallJump (f1 unrounded) */
        gabi::Local<be<f32>> temp4;
        *temp4 = (f32)jump;
        if (!(jump < 0.0f)) {
            os_pmf_npc(this, &os_actions()->mJumpNpc, temp4.get());
            return TRUE;
        }
        if (param_1 > 360000.0f) {
            return FALSE;
        }
    }

    gabi::Local<cXyz> temp2;
    gabi::Local<cXyz> temp3;
    s16 a = *param_2;
    f32 x = current.pos.x;
    f32 z = current.pos.z;
    f32 y = current.pos.y + 80.0f;
    temp2->x = x;
    temp2->y = y;
    temp2->z = z;
    temp3->x = gabi::fmadds(80.0f, cM_ssin(a), x);
    temp3->y = y;
    temp3->z = gabi::fmadds(80.0f, cM_scos(a), z);
    routeWallCheck(temp2, temp3, param_2);
    return TRUE;
}

// HD: the door branch no longer sets animation speed to 4.0.
/* 022AF8D4 */
BOOL daNpc_Os_c::searchNpcAction(void* param_1) {
    if (field_0x7A9 == 0) {
        attention_info.flags |= fopAc_Attn_ACTION_CARRY_e;
        os_setAnm(this, 1);
        field_0x7A9 = field_0x7A9 + 1;
        return TRUE;
    }
    if (field_0x7A9 == -1) {
        return TRUE;
    }
    field_0x7A4 = 1;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    BOOL door = player->eventInfo.mCommand == dEvtCmd_INDOOR_e;        /* eventInfo.checkCommandDoor() */
    f64 dist_sq = Os_distanceXZSquaredHD(this, dComIfGp_getPlayer(0)); /* fopAcM_searchPlayerDistanceXZ2 */
    f32 temp;
    f32 r = Os_actionHioHD()->field_0x60;
    if (dist_sq < (f64)(f32)(r * r)) {
        temp = 0.0f;
    } else {
        temp = Os_actionHioHD()->field_0xA8;
    }
    s16 angle = fopAcM_searchActorAngleY(this, dComIfGp_getPlayer(0));
    gabi::Local<be<s16>> adjusted;
    *adjusted = angle;
    BOOL temp3 = FALSE;
    if (Os_routeCheckHD(this, dist_sq, adjusted.get()) && cLib_distanceAngleS(angle, *adjusted) <= 0x2000) {
        temp3 = TRUE;
    }
    if (door || !temp3 || (dComIfGp_getPlayerStatus0() & 0x02000101) /* dComIfGp_checkPlayerStatus0 */ ||
        ((u32) reinterpret_cast<daPy_py_l*>(player)->mResetFlg0 &
         0x10000) /* player->checkAttentionLock() */) {
        temp = 0.0f;
        mNoResetFlg1 &= ~2u; /* offNpcCallCommand */
    } else {
        os_setRestart(this, os_getRestartNumber(this));
        if (Os_calcTimerHD(&field_0x7A7) == 0) {
            field_0x7A6 = field_0x7A6 ^ 1;
            field_0x7A7 = (u8)Os_randomValueHD(8, 20);
        }
        // HD: writes the first frame-control word, rather than the shared play-rate field.
        reinterpret_cast<OsAnimationControlHD*>(mpMorf.get())->mControlWord = 2.0f;
    }

    os_walkProc(this, temp, *adjusted);
    cLib_addCalcAngleS(&shape_angle.y, current.angle.y, Os_actionHioHD()->mOs2.field_0x28,
                       (s16)(Os_actionHioHD()->mOs2.field_0x24 * 2),
                       (s16)(Os_actionHioHD()->mOs2.field_0x26 * 2));
    s16 temp4 = shape_angle.y;
    os_lookBack(this, 1, 0, 0);
    if (temp < 0.001f) {
        os_pmf_npc(this, &os_actions()->mWaitNpc, nullptr);
    } else {
        shape_angle.y = temp4;
    }
    os_setAttention(this, 1);
    return TRUE;
}

/* 022AFCE0 */
BOOL daNpc_Os_c::waitPlayerAction(void* param_1) {
    if (field_0x7A9 == 0) {
        speedF = 0.0f;
        os_setAnm(this, 0);
        os_initBrkAnm(this, 8, 1);
        field_0x7A9 = field_0x7A9 + 1;
        return TRUE;
    }
    if (field_0x7A9 == -1) {
        return TRUE;
    }
    auto* att = reinterpret_cast<OsAttentionFlagsHD*>(dComIfGp_getAttention());
    if (!(os_stickValue() < Os_actionHioHD()->field_0x9C) || os_LockonTruth(gabi::ea(att)) ||
        ((u32)att->mFlags & 0x20000000)) {
        s16 target = os_getStickAngY(this);
        cLib_addCalcAngleS(&current.angle.y, target, 0x19, 0x7FFF, 1);
        gabi::Local<cXyz> temp;
        s32 stickPos = os_calcStickPos(this, target, temp);
        gabi::Local<be<s16>> temp2;
        if (stickPos != 0 && os_stickValue() < Os_actionHioHD()->field_0xA0) {
            os_lookAtStick(this, temp2, temp);
        } else {
            shape_angle.y = current.angle.y;
            os_lookAtStick(this, temp2, temp);
        }
        if (stickPos > 0) {
            s16 a = *temp2;
            current.angle.y = a;
            shape_angle.y = a;
        } else {
            current.angle.y = shape_angle.y;
        }
        if (!(os_stickValue() < Os_actionHioHD()->field_0xA0) && stickPos == 0) {
            current.angle.y = target;
            os_pmf_player(this, &os_actions()->mWalkPlayer, nullptr);
        }
    } else {
        os_lookAtZero(this, &shape_angle.y, shape_angle.y, 0, 0);
        current.angle.y = shape_angle.y;
    }
    os_setAttention(this, os_isMorf(this));
    return TRUE;
}

/* 022AFF94 */
BOOL daNpc_Os_c::walkPlayerAction(void* param_1) {
    if (field_0x7A9 == 0) {
        speedF = 0.0f;
        os_setAnm(this, 1);
        os_initBrkAnm(this, 8, 1);
        field_0x7A9 = field_0x7A9 + 1;
        return TRUE;
    }
    if (field_0x7A9 == -1) {
        return TRUE;
    }
    f64 stickValue = os_stickValue();
    s16 target = os_getStickAngY(this);
    if (stickValue > 0.05f) {
        gabi::Local<cXyz> temp;
        s32 stickPos;
        f32 frame = mPrevMorfFrame;
        if (frame > Os_actionHioHD()->field_0xB0 && frame < Os_actionHioHD()->field_0xAC) {
            f32 s = (f32)((f64)Os_actionHioHD()->mOs2.field_0x08 * stickValue);
            speedF = s * Os_actionHioHD()->mOs2.field_0x20;
            stickPos = os_calcStickPos(this, target, temp);
        } else {
            speedF = 0.0f;
            cLib_addCalcAngleS(&current.angle.y, target, Os_actionHioHD()->mOs2.field_0x28,
                               Os_actionHioHD()->mOs2.field_0x24, Os_actionHioHD()->mOs2.field_0x26);
            stickPos = os_calcStickPos(this, target, temp);
        }
        if (stickPos == 0) {
            cLib_addCalcAngleS(&shape_angle.y, current.angle.y, Os_actionHioHD()->mOs2.field_0x28,
                               (s16)(Os_actionHioHD()->mOs2.field_0x24 * 2),
                               (s16)(Os_actionHioHD()->mOs2.field_0x26 * 2));
        }
        gabi::Local<be<s16>> temp2;
        os_lookAtStick(this, temp2, temp);
        if (stickPos > 0) {
            shape_angle.y = *temp2;
        }
        if (stickPos != 0) {
            os_pmf_player(this, &os_actions()->mWaitPlayer, nullptr);
        }
    } else {
        os_pmf_player(this, &os_actions()->mWaitPlayer, nullptr);
    }
    os_setAttention(this, 1);
    return TRUE;
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py): this translation unit
ext 022AA88C void os_setAnm(daNpc_Os_c* t, s32 n)
ext 022A9E0C u32 os_wakeupCheck(daNpc_Os_c* t)
ext 022A9EAC u32 os_finishCheck(daNpc_Os_c* t)
ext 022A9F4C void os_initBrkAnm(daNpc_Os_c* t, u8 n, u32 b)
ext 022AA42C u32 os_getRestartNumber(daNpc_Os_c* t)
ext 022AA778 void os_setNpcAction(daNpc_Os_c* t, ProcFunc_l* f, void* p)
ext 022ABAC8 void os_setPlayerAction(daNpc_Os_c* t, ProcFunc_l* f, void* p)
ext 022AB350 void os_smokeSet(daNpc_Os_c* t, u16 id)
ext 022AD6CC void os_setAttention(daNpc_Os_c* t, u32 b)
ext 022AD668 void os_walkProc(daNpc_Os_c* t, f32 spd, s16 ang)
ext 022ADBCC u32 os_talk_init(daNpc_Os_c* t)
ext 022ADC88 u32 os_talk(daNpc_Os_c* t)
ext 022ADA3C u32 os_setAnm_brkAnm(daNpc_Os_c* t, s32 n)
ext 022AE014 u32 os_chkAttention(daNpc_Os_c* t, cXyz* pos, s16 ang)
ext 022AE140 u32 os_chkArea(daNpc_Os_c* t, cXyz* pos)
ext 022AE1CC u32 os_getMsg(daNpc_Os_c* t)
ext 022AE1D4 void os_lookBack(daNpc_Os_c* t, s32 a, s32 b, s32 c)
ext 022AE304 s32 os_wallHitCheck(daNpc_Os_c* t)
ext 022AE340 s16 os_getStickAngY(daNpc_Os_c* t)
ext 022AE394 s8 os_getWakeupOrderEventNum(daNpc_Os_c* t)
ext 022AE3CC s32 os_calcStickPos(daNpc_Os_c* t, s16 a, cXyz* p)
ext 02445438 void os_setRestart(daNpc_Os_c* t, u32 n)
ext 024EDFCC u32 os_LockonTruth(u32 att)
ext 0259DED0 void Os_lookAtTargetHD(dNpc_JntCtrl_c* joint, be<s16>* outY, cXyz* target, cXyz* eye, s16 rotation, s16 velocity, u32 headOnly)
ext 020079B4 f64 Os_stickValueHD(s32 port)
ext 02008DAC void Os_groundCheckDestructHD(void* check, s32 flag)
ext 022AF0B4 f64 Os_forwardGroundUnroundedHD(daNpc_Os_c* actor, s16 angle)
ext 028F4384 f64 Os_sqrtUnroundedHD(f32 value)
ext 0201B080 void Os_crossProductHD(cXyz* left, cXyz* output, cXyz* right)
ext 022AF270 f64 Os_wallJumpUnroundedHD(daNpc_Os_c* actor, s16 angle)
ext 025D69AC f64 Os_distanceXZSquaredHD(daNpc_Os_c* actor, fopAc_ac_c* player)
ext 022AF4C8 u32 Os_routeCheckHD(daNpc_Os_c* actor, f64 distance, be<s16>* angle)
ext 0207A9A0 u8 Os_calcTimerHD(be<u8>* timer)
ext 021E1E78 s32 Os_randomValueHD(s32 minimum, s32 maximum)
*/
// clang-format on
