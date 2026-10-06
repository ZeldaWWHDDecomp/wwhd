/**
 * d_a_npc_os_c.cpp (WWHD)
 * NPC - Os (the stone-head helper of the Earth/Wind temples): NPC actions, route checks, player actions
 * (part C, 022AE504..022B01A4).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_os.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_os_local.h"

/* ---- this TU's statics (HD addresses) ---- */
#define OS_L_HIO 0x10467E44u   /* daNpc_Os_HIO_c l_HIO (HD offsets = GameCube offsets - 4 from 0x5C on) */
#define OS_L_MSGID 0x10467DF0u /* fpc_ProcID l_msgId */
static inline f32 os_hioF(u32 off) { return gabi::load<f32>(OS_L_HIO + off); }
static inline s16 os_hioS(u32 off) { return gabi::load<s16>(OS_L_HIO + off); }

/* pointers to member (8-byte constants in .data: {s16 d = 0, s16 i = -1, u32 f}) */
enum : u32 {
    OS_PMF_waitNpcAction = 0x1001F260,
    OS_PMF_waitPlayerAction = 0x1001F268,
    OS_PMF_walkPlayerAction = 0x1001F270,
    OS_PMF_throwNpcAction = 0x1001F278,
    OS_PMF_carryNpcAction = 0x1001F280,
    OS_PMF_finish01NpcAction = 0x1001F288,
    OS_PMF_searchNpcAction = 0x1001F290,
    OS_PMF_finish02NpcAction = 0x1001F298,
    OS_PMF_jumpNpcAction = 0x1001F2A0,
    OS_PMF_talkNpcAction = 0x1001F2A8,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* functions of the other parts (by address) */
static inline void os_setAnm(daNpc_Os_c* t, s32 n) { gabi::call(0x022AA88C, t, n); }
static inline u32 os_wakeupCheck(daNpc_Os_c* t) { return gabi::call<u32>(0x022A9E0C, t); }
static inline u32 os_finishCheck(daNpc_Os_c* t) { return gabi::call<u32>(0x022A9EAC, t); }
static inline void os_initBrkAnm(daNpc_Os_c* t, u8 n, u32 b) { gabi::call(0x022A9F4C, t, n, b); }
static inline u32 os_getRestartNumber(daNpc_Os_c* t) { return gabi::call<u32>(0x022AA42C, t); }
static inline void os_setNpcAction(daNpc_Os_c* t, ProcFunc_l* f, void* p) { gabi::call(0x022AA778, t, f, p); }
static inline void os_setPlayerAction(daNpc_Os_c* t, ProcFunc_l* f, void* p) { gabi::call(0x022ABAC8, t, f, p); }
static inline void os_smokeSet(daNpc_Os_c* t, u16 id) { gabi::call(0x022AB350, t, id); }
static inline void os_setAttention(daNpc_Os_c* t, u32 b) { gabi::call(0x022AD6CC, t, b); }
static inline void os_walkProc(daNpc_Os_c* t, f32 spd, s16 ang) { gabi::call(0x022AD668, t, spd, ang); }
static inline u32 os_talk_init(daNpc_Os_c* t) { return gabi::call<u32>(0x022ADBCC, t); }
static inline u32 os_talk(daNpc_Os_c* t) { return gabi::call<u32>(0x022ADC88, t); }
static inline u32 os_setAnm_brkAnm(daNpc_Os_c* t, s32 n) { return gabi::call<u32>(0x022ADA3C, t, n); }
static inline u32 os_chkAttention(daNpc_Os_c* t, cXyz* pos, s16 ang) { return gabi::call<u32>(0x022AE014, t, pos, ang); }
static inline u32 os_chkArea(daNpc_Os_c* t, cXyz* pos) { return gabi::call<u32>(0x022AE140, t, pos); }
static inline u32 os_getMsg(daNpc_Os_c* t) { return gabi::call<u32>(0x022AE1CC, t); }
static inline void os_lookBack(daNpc_Os_c* t, s32 a, s32 b, s32 c) { gabi::call(0x022AE1D4, t, a, b, c); }
static inline s32 os_wallHitCheck(daNpc_Os_c* t) { return gabi::call<s32>(0x022AE304, t); }
static inline s16 os_getStickAngY(daNpc_Os_c* t) { return gabi::call<s16>(0x022AE340, t); }
static inline s8 os_getWakeupOrderEventNum(daNpc_Os_c* t) { return gabi::call<s8>(0x022AE394, t); }
static inline s32 os_calcStickPos(daNpc_Os_c* t, s16 a, cXyz* p) { return gabi::call<s32>(0x022AE3CC, t, a, p); }
/* daPy_npc_c::setRestart 02445438 (int) */
static inline void os_setRestart(daNpc_Os_c* t, u32 n) { gabi::call(0x02445438, t, n); }
/* dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (copy), s16 yrot, s16 vel, bool headOnly) */
static inline void os_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u32 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
/* eye argument copied from cXyz::Zero (0x101FFBA8) through FPRs */
static inline void os_lookAtZero(daNpc_Os_c* t, be<s16>* outY, s16 yrot, s16 vel, u32 headOnly) {
    gabi::Local<cXyz> e;
    cXyz* z = gabi::at<cXyz>(0x101FFBA8);
    e->x = z->x;
    e->y = z->y;
    e->z = z->z;
    os_lookAtTarget(&t->mJntCtrl, outY, nullptr, e, yrot, vel, headOnly);
}
static inline void os_pmf_npc(daNpc_Os_c* t, u32 pmf, void* arg) {
    gabi::Local<ProcFunc_l> fn;
    md_pmf_load(fn, pmf);
    os_setNpcAction(t, fn, arg);
}
static inline void os_pmf_player(daNpc_Os_c* t, u32 pmf, void* arg) {
    gabi::Local<ProcFunc_l> fn;
    md_pmf_load(fn, pmf);
    os_setPlayerAction(t, fn, arg);
}
static inline be<u32>* os_attnFlags(daNpc_Os_c* t) { return gabi::at<be<u32>>(gabi::ea(t) + 0x39C); }
static inline be<u32>* os_npcFlags(daNpc_Os_c* t) { return gabi::at<be<u32>>(gabi::ea(t) + 0x3BC); } /* daPy_py_c mNoResetFlg1 */
static inline u32 os_acchFlags(daNpc_Os_c* t) { return gabi::load<u32>(gabi::ea(t) + 0x464); } /* mAcch.m_flags */
/* mpMorf->isMorf(): morf ratio (+0xB0) < 1.0 */
static inline u32 os_isMorf(daNpc_Os_c* t) { return gabi::load<f32>(gabi::ea(t->mpMorf.get()) + 0xB0) < 1.0f; }
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
static inline f64 os_stickValue() { return gabi::call<f64>(0x020079B4, 0); }
/* dAttention_c::LockonTruth 024EDFCC */
static inline u32 os_LockonTruth(u32 att) { return gabi::call<u32>(0x024EDFCC, att); }
enum : u32 { JA_SE_OBJ_OSTATUE_PUT = 0x58FE, ID_AK_ST_OTOMOSMOKE00 = 0xA328, ID_AK_ST_OTOMOSMOKE01 = 0xA33B };

/* ====================================================================== */
/* NPC actions                                                            */
/* ====================================================================== */

/* 022AE504 */
BOOL daNpc_Os_c::waitNpcAction(void* param_1) {
    WWHD_FUNC(0x022AE504, BOOL, this, param_1);
    if (field_0x7A9 == 0) {
        maxFallSpeed = -100.0f;
        gravity = os_hioF(0x88);
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
        if (os_wakeupCheck(this) && gabi::load<u32>(dComIfGp_ea() + 0x5B38) == gabi::ea(this) /* dComIfGp_getCb1Player() */) {
            if (*os_npcFlags(this) & 0x40 /* checkNpcNotChange() */) {
                *os_npcFlags(this) &= ~0x40u;
                os_initBrkAnm(this, 4, 1);
                field_0x7A1 = 0;
                field_0x7A9 = field_0x7A9 + 1;
            } else {
                field_0x7A9 = field_0x7A9 + 1;
            }
        }
    } else {
        *os_attnFlags(this) |= 0x10; /* fopAc_Attn_ACTION_CARRY_e */
        field_0x7A4 = (u8)os_chkAttentionHere(this);
        if (os_chkArea(this, &current.pos)) {
            os_lookBack(this, field_0x7A4, 0, 1);
        }
        current.angle.y = shape_angle.y;
        if (os_finishCheck(this)) {
            os_pmf_npc(this, OS_PMF_finish01NpcAction, nullptr);
        }
    }

    *os_attnFlags(this) &= ~0xAu; /* LOCKON_TALK | ACTION_SPEAK */
    f32 dist_sq = fopAcM_searchActorDistance2(this, dComIfGp_getPlayer(0));
    if (!(*os_npcFlags(this) & 2) /* !checkNpcCallCommand() */) {
        f32 r = os_hioF(0x60);
        if (dist_sq < r * r) {
            fopAc_ac_c* link = dComIfGp_getLinkPlayer();
            *gabi::at<be<u32>>(gabi::ea(link) + 0x3BC) |= 2; /* onNpcCall */
        }
    } else if (os_wakeupCheck(this)) {
        f32 r = os_hioF(0x5C);
        if (!(dist_sq < r * r)) {
            os_pmf_npc(this, OS_PMF_searchNpcAction, nullptr);
        }
    } else {
        field_0x7A5 = os_getWakeupOrderEventNum(this);
    }
    os_setAttention(this, 1);
    return TRUE;
}
VERIFY(0x022AE504, &daNpc_Os_c::waitNpcAction);

/* 022AE834 */
BOOL daNpc_Os_c::finish01NpcAction(void* param_1) {
    WWHD_FUNC(0x022AE834, BOOL, this, param_1);
    if (field_0x7A9 == 0) {
        if (gabi::load<u32>(dComIfGp_ea() + 0x5B38) == gabi::ea(this)) {
            *os_npcFlags(this) |= 0x40; /* onNpcNotChange */
            gabi::store<u32>(dComIfGp_ea() + 0x5B38, 0); /* dComIfGp_setCb1Player(NULL) */
        }
        os_setAnm(this, 0);
        os_initBrkAnm(this, 0, 1);
        field_0x7A1 = 0;
        maxFallSpeed = -100.0f;
        gravity = os_hioF(0x88);
        speedF = 0.0f;
        field_0x788 = 120.0f;
        field_0x7A9 = field_0x7A9 + 1;
        return TRUE;
    }
    if (field_0x7A9 != -1) {
        os_lookAtZero(this, &shape_angle.y, shape_angle.y, field_0x798, 1);
        if (field_0x7A1 != 0) {
            os_pmf_npc(this, OS_PMF_finish02NpcAction, nullptr);
        }
        os_setAttention(this, 1);
    }
    return TRUE;
}
VERIFY(0x022AE834, &daNpc_Os_c::finish01NpcAction);

/* 022AE980 */
BOOL daNpc_Os_c::finish02NpcAction(void* param_1) {
    WWHD_FUNC(0x022AE980, BOOL, this, param_1);
    if (field_0x7A9 == 0) {
        if (param_1 == nullptr) {
            s32 anm = 1;
            if (dSv_event_isEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), 0x1B01)) {
                anm = 7;
            }
            os_setAnm_brkAnm(this, anm);
        }
        field_0x784 |= 1; /* onFinish */
        maxFallSpeed = -100.0f;
        gravity = os_hioF(0x88);
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
VERIFY(0x022AE980, &daNpc_Os_c::finish02NpcAction);

/* talkNpcAction's common tail */
static inline void os_talkTail(daNpc_Os_c* t) {
    t->mJntCtrl.mbTrn = 1; /* setTrn */
    os_lookBack(t, t->field_0x7A4, 0, 0);
    t->current.angle.y = t->shape_angle.y;
    os_setAttention(t, os_isMorf(t));
}

/* 022AEA8C */
BOOL daNpc_Os_c::talkNpcAction(void* param_1) {
    WWHD_FUNC(0x022AEA8C, BOOL, this, param_1);
    if (field_0x7A9 == 0) {
        gabi::store<u32>(OS_L_MSGID, 0xFFFFFFFFu);
        field_0x780 = os_getMsg(this);
        field_0x7A9 = field_0x7A9 + 1;
        *os_attnFlags(this) &= ~0x10u;
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
            os_pmf_npc(this, OS_PMF_waitNpcAction, nullptr);
            u32 p = dComIfGp_ea(); /* dComIfGp_event_reset() */
            gabi::store<u16>(p + 0x52B8, (u16)(gabi::load<u16>(p + 0x52B8) | 8));
        }
    } else if (field_0x7A9 == 3) {
        os_pmf_npc(this, OS_PMF_waitNpcAction, nullptr);
    }
    os_talkTail(this);
    return TRUE;
}
VERIFY(0x022AEA8C, &daNpc_Os_c::talkNpcAction);

/* 022AECD4 HD: a roof hit (no wall) throws towards the current angle (GameCube: mAcchCir[-1]) */
BOOL daNpc_Os_c::carryNpcAction(void* param_1) {
    WWHD_FUNC(0x022AECD4, BOOL, this, param_1);
    if (field_0x7A9 == 0) {
        os_setAnm(this, 0);
        *os_attnFlags(this) &= ~0x10u;
        *os_npcFlags(this) &= ~2u; /* offNpcCallCommand */
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
    cLib_chaseF(&field_0x7E4.x, os_hioF(0x8C), 1.0f);
    cLib_chaseF(&field_0x7E4.y, os_hioF(0x90), 1.0f);
    cLib_chaseF(&field_0x7E4.z, os_hioF(0x94), 1.0f);

    s32 wallHit = os_wallHitCheck(this);
    if (wallHit >= 0 || (os_acchFlags(this) & 0x200) /* mAcch.ChkRoofHit() */) {
        fopAcM_cancelCarryNow(this);
        gabi::Local<be<s16>> temp;
        *temp = current.angle.y;
        if (wallHit >= 0) {
            *temp = gabi::load<s16>(gabi::ea(&mAcchCir[wallHit]) + 0x3C); /* GetWallAngleY */
        }
        os_pmf_npc(this, OS_PMF_throwNpcAction, temp.get());
        return TRUE;
    }
    if (!(actor_status & 0x2000) /* !fopAcM_checkCarryNow */) {
        if (speedF > 0.0f) {
            os_pmf_npc(this, OS_PMF_throwNpcAction, nullptr);
            return TRUE;
        }
        os_seStartCurrent(this, JA_SE_OBJ_OSTATUE_PUT);
        os_smokeSet(this, ID_AK_ST_OTOMOSMOKE00);
        os_pmf_npc(this, OS_PMF_waitNpcAction, nullptr);
        return TRUE;
    }
    os_lookAtZero(this, &shape_angle.y, shape_angle.y, 0, 0);
    current.angle.y = shape_angle.y;
    return TRUE;
}
VERIFY(0x022AECD4, &daNpc_Os_c::carryNpcAction);

/* 022AEF70 */
BOOL daNpc_Os_c::throwNpcAction(void* param_1) {
    WWHD_FUNC(0x022AEF70, BOOL, this, param_1);
    if (field_0x7A9 == 0) {
        if (param_1 != nullptr) {
            speedF = 8.0f;
            speed.y = 0.0f;
            current.angle.y = *(be<s16>*)param_1;
        } else {
            speedF = os_hioF(0x7C);
            speed.y = os_hioF(0x80);
            current.angle.y = shape_angle.y;
        }
        maxFallSpeed = -100.0f;
        gravity = os_hioF(0x88);
        field_0x7A9 = field_0x7A9 + 1;
        return TRUE;
    }
    if (field_0x7A9 != -1) {
        if (os_acchFlags(this) & 0x20 /* mAcch.ChkGroundHit() */) {
            os_seStartCurrent(this, JA_SE_OBJ_OSTATUE_PUT);
            os_smokeSet(this, ID_AK_ST_OTOMOSMOKE01);
            os_pmf_npc(this, OS_PMF_waitNpcAction, nullptr);
        }
        os_setAttention(this, 1);
    }
    return TRUE;
}
VERIFY(0x022AEF70, &daNpc_Os_c::throwNpcAction);

/* ====================================================================== */
/* route checks                                                           */
/* ====================================================================== */

/* this TU's dBgS_GndChk / dBgS_LinChk vtables */
static const dBgS_GndChk_vt OS_GNDCHK_VT = {0x1001F3A4, 0x1001F3B4, 0x1001F3D4, 0x1001F3C4};
static const dBgS_LinChk_vt OS_LINCHK_VT = {0x1001F3E4, 0x1001F3F4, 0x1001F414, 0x1001F404};
struct os_chk54 { u8 _[0x54]; };
struct os_chk6C { u8 _[0x6C]; };
/* ~dBgS_GndChk (inline vtable stores, then cBgS_Chk's destructor) */
static inline void os_gndChk_dt(void* chk) {
    u32 g = gabi::ea(chk);
    gabi::store<u32>(g + 0x20, 0x1001F3B4);
    gabi::store<u32>(g + 0x40, 0x1001F3D4);
    gabi::store<u32>(g + 0x4C, 0x1001F394);
    gabi::call(0x02008DAC, chk, 0); /* cBgS_Chk::~cBgS_Chk */
}
/* ~dBgS_LinChk (inline vtable stores, then cBgS_LinChk's destructor) */
static inline void os_linChk_dt(void* chk) {
    u32 b = gabi::ea(chk);
    gabi::store<u32>(b + 0x58, 0x1001F414);
    gabi::store<u32>(b + 0x64, 0x1001F394);
    gabi::store<u32>(b + 0x20, 0x1001F384);
    cBgS_LinChk_dt(chk, 0);
}

/* 022AF0B4 */
f32 daNpc_Os_c::checkForwardGroundY(s16 param_1) {
    WWHD_FUNC(0x022AF0B4, f32, this, param_1);
    s32 wallHit = os_wallHitCheck(this);
    if (wallHit >= 0) {
        dBgS* bgs = dComIfG_Bgsp();
        u32 cir = gabi::ea(&mAcchCir[wallHit]);
        void* pla = cBgS_GetTriPla(bgs, gabi::load<u16>(cir + 2), gabi::load<u16>(cir));
        if (pla) {
            u32 pp = gabi::ea(pla);
            s16 a = cM_atan2s(gabi::load<f32>(pp), gabi::load<f32>(pp + 8));
            if (cLib_distanceAngleS(param_1, a) > 0x4000) {
                gabi::Local<os_chk54> gnd_chk;
                dBgS_GndChk_ct(gnd_chk, OS_GNDCHK_VT, false);
                u32 g = gabi::ea(gnd_chk.get());
                gabi::store<u32>(g + 0x30, gabi::load<u32>(g + 0x30) & ~2u); /* OffWall */
                f32 x = gabi::fmadds(80.0f, cM_ssin(param_1), current.pos.x);
                f32 y = current.pos.y + 80.0f;
                f32 z = gabi::fmadds(80.0f, cM_scos(param_1), current.pos.z);
                gabi::store<f32>(g + 0x24, x); /* SetPos */
                gabi::store<f32>(g + 0x28, y);
                gabi::store<f32>(g + 0x2C, z);
                f32 r = cBgS_GroundCross(dComIfG_Bgsp(), gnd_chk);
                os_gndChk_dt(gnd_chk);
                return r;
            }
        }
    }
    return -1e+7f;
}
VERIFY(0x022AF0B4, &daNpc_Os_c::checkForwardGroundY);

/* 022AF270 */
f32 daNpc_Os_c::checkWallJump(s16 param_1) {
    WWHD_FUNC(0x022AF270, f32, this, param_1);
    f64 g = gabi::call<f64>(0x022AF0B4, this, param_1); /* checkForwardGroundY (f1 unrounded) */
    f32 delta = (f32)(g - (f64)(f32)current.pos.y);
    if (0.0f < delta && delta < 80.0f) {
        f64 s = gabi::call<f64>(0x028F4384, delta); /* std::sqrtf */
        return (f32)(s * (f64)3.6f);
    }
    return -1.0f;
}
VERIFY(0x022AF270, &daNpc_Os_c::checkWallJump);

/* 022AF2F0 */
void daNpc_Os_c::routeAngCheck(cXyz* param_1, be<s16>* param_2) {
    WWHD_FUNC(0x022AF2F0, void, this, param_1, param_2);
    gabi::Local<cXyz> cross;
    gabi::call(0x0201B080, &field_0x7F0, cross.get(), param_1); /* field_0x7F0.outprod(*param_1) */
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
VERIFY(0x022AF2F0, &daNpc_Os_c::routeAngCheck);

/* 022AF3AC */
void daNpc_Os_c::routeWallCheck(cXyz* param_1, cXyz* param_2, be<s16>* param_3) {
    WWHD_FUNC(0x022AF3AC, void, this, param_1, param_2, param_3);
    gabi::Local<os_chk6C> lin_chk;
    dBgS_LinChk_ct(lin_chk, OS_LINCHK_VT, false);
    dBgS_LinChk_Set(lin_chk, param_1, param_2, nullptr);
    if (cBgS_LineCross(dComIfG_Bgsp(), lin_chk)) {
        dBgS* bgs = dComIfG_Bgsp();
        u32 c = gabi::ea(lin_chk.get());
        u16 poly = gabi::load<u16>(c + 0x14);
        u16 bg = gabi::load<u16>(c + 0x16);
        void* pla = cBgS_GetTriPla(bgs, bg, poly);
        if (pla) {
            routeAngCheck((cXyz*)pla, param_3); /* &plane->mNormal */
        }
    }
    os_linChk_dt(lin_chk);
}
VERIFY(0x022AF3AC, &daNpc_Os_c::routeWallCheck);

/* 022AF4C8 */
BOOL daNpc_Os_c::routeCheck(f32 param_1, be<s16>* param_2) {
    WWHD_FUNC(0x022AF4C8, BOOL, this, param_1, param_2);
    if (!(os_acchFlags(this) & 0x20) /* !mAcch.ChkGroundHit() */) {
        current.pos.copy(old.pos);
        speedF = 0.0f;
        field_0x7A8 = 1;

        gabi::Local<os_chk54> gnd_chk;
        dBgS_GndChk_ct(gnd_chk, OS_GNDCHK_VT, false);
        u32 g = gabi::ea(gnd_chk.get());
        gabi::store<u32>(g + 0x30, gabi::load<u32>(g + 0x30) & ~2u); /* OffWall */
        s16 a = *param_2;
        gabi::Local<cXyz> temp;
        temp->x = gabi::fmadds(80.0f, cM_ssin(a), current.pos.x);
        temp->y = current.pos.y + 80.0f;
        temp->z = gabi::fmadds(80.0f, cM_scos(a), current.pos.z);
        gabi::store<f32>(g + 0x24, temp->x); /* SetPos */
        gabi::store<f32>(g + 0x28, temp->y);
        gabi::store<f32>(g + 0x2C, temp->z);

        f32 gy = cBgS_GroundCross(dComIfG_Bgsp(), gnd_chk);
        f32 y = current.pos.y;
        if (gy - y > -100.0f) {
            if (cLib_distanceAngleS(current.angle.y, *param_2) < 0x800) {
                os_pmf_npc(this, OS_PMF_jumpNpcAction, nullptr);
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

    if (os_acchFlags(this) & 0x10 /* mAcch.ChkWallHit() */) {
        f64 jump = gabi::call<f64>(0x022AF270, this, (s16)*param_2); /* checkWallJump (f1 unrounded) */
        gabi::Local<be<f32>> temp4;
        *temp4 = (f32)jump;
        if (!(jump < 0.0f)) {
            os_pmf_npc(this, OS_PMF_jumpNpcAction, temp4.get());
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
VERIFY(0x022AF4C8, &daNpc_Os_c::routeCheck);

/* 022AF8D4 HD: no door branch (GameCube: play speed 4.0 while the player opens a door) */
BOOL daNpc_Os_c::searchNpcAction(void* param_1) {
    WWHD_FUNC(0x022AF8D4, BOOL, this, param_1);
    if (field_0x7A9 == 0) {
        *os_attnFlags(this) |= 0x10;
        os_setAnm(this, 1);
        field_0x7A9 = field_0x7A9 + 1;
        return TRUE;
    }
    if (field_0x7A9 == -1) {
        return TRUE;
    }
    field_0x7A4 = 1;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    BOOL door = gabi::load<u16>(gabi::ea(player) + 0xF8) == 3; /* eventInfo.checkCommandDoor() */
    f64 dist_sq = gabi::call<f64>(0x025D69AC, this, dComIfGp_getPlayer(0)); /* fopAcM_searchPlayerDistanceXZ2 */
    f32 temp;
    f32 r = os_hioF(0x5C);
    if (dist_sq < (f64)(f32)(r * r)) {
        temp = 0.0f;
    } else {
        temp = os_hioF(0xA4);
    }
    s16 angle = fopAcM_searchActorAngleY(this, dComIfGp_getPlayer(0));
    gabi::Local<be<s16>> adjusted;
    *adjusted = angle;
    BOOL temp3 = FALSE;
    if (gabi::call<u32>(0x022AF4C8, this, dist_sq, adjusted.get()) /* routeCheck */ &&
        cLib_distanceAngleS(angle, *adjusted) <= 0x2000) {
        temp3 = TRUE;
    }
    if (door || !temp3 || (gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x02000101) /* dComIfGp_checkPlayerStatus0 */ ||
        (gabi::load<u32>(gabi::ea(player) + 0x3C0) & 0x10000) /* player->checkAttentionLock() */) {
        temp = 0.0f;
        *os_npcFlags(this) &= ~2u; /* offNpcCallCommand */
    } else {
        os_setRestart(this, os_getRestartNumber(this));
        if (gabi::call<u8>(0x0207A9A0, &field_0x7A7) == 0) { /* cLib_calcTimer<u8> */
            field_0x7A6 = field_0x7A6 ^ 1;
            field_0x7A7 = (u8)gabi::call<s32>(0x021E1E78, 8, 20); /* cLib_getRndValue */
        }
        gabi::store<f32>(gabi::ea(mpMorf.get()) + 0x98, 2.0f); /* mpMorf->setPlaySpeed(2.0f) */
    }

    os_walkProc(this, temp, *adjusted);
    cLib_addCalcAngleS(&shape_angle.y, current.angle.y, os_hioS(0x28), (s16)(os_hioS(0x24) * 2), (s16)(os_hioS(0x26) * 2));
    s16 temp4 = shape_angle.y;
    os_lookBack(this, 1, 0, 0);
    if (temp < 0.001f) {
        os_pmf_npc(this, OS_PMF_waitNpcAction, nullptr);
    } else {
        shape_angle.y = temp4;
    }
    os_setAttention(this, 1);
    return TRUE;
}
VERIFY(0x022AF8D4, &daNpc_Os_c::searchNpcAction);

/* 022AFBF4 */
BOOL daNpc_Os_c::jumpNpcAction(void* param_1) {
    WWHD_FUNC(0x022AFBF4, BOOL, this, param_1);
    if (field_0x7A9 == 0) {
        os_setAnm(this, 0);
        if (param_1 != nullptr) {
            speed.y = gabi::load<f32>(gabi::ea(param_1));
        }
        speedF = 50.0f;
        maxFallSpeed = -100.0f;
        gravity = os_hioF(0x88);
        field_0x7A9 = field_0x7A9 + 1;
        return TRUE;
    }
    if (field_0x7A9 != -1) {
        if (os_acchFlags(this) & 0x20 /* mAcch.ChkGroundHit() */) {
            os_smokeSet(this, ID_AK_ST_OTOMOSMOKE01);
            os_pmf_npc(this, OS_PMF_waitNpcAction, nullptr);
        }
        os_setAttention(this, 1);
    }
    return TRUE;
}
VERIFY(0x022AFBF4, &daNpc_Os_c::jumpNpcAction);

/* ====================================================================== */
/* player actions                                                         */
/* ====================================================================== */

/* lookAtTarget(&temp2, &temp, current.pos (copy), shape_angle.y, mNpc.mMaxTurnStep, false) */
static inline void os_lookAtStick(daNpc_Os_c* t, be<s16>* temp2, cXyz* temp) {
    gabi::Local<cXyz> eye;
    eye->x = t->current.pos.x;
    eye->y = t->current.pos.y;
    eye->z = t->current.pos.z;
    s16 yrot = t->shape_angle.y;
    *temp2 = yrot;
    os_lookAtTarget(&t->mJntCtrl, temp2, temp, eye, yrot, os_hioS(0x44), 0);
}

/* 022AFCE0 */
BOOL daNpc_Os_c::waitPlayerAction(void* param_1) {
    WWHD_FUNC(0x022AFCE0, BOOL, this, param_1);
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
    u32 att = dComIfGp_ea() + 0x5804; /* dComIfGp_getAttention() */
    if (!(os_stickValue() < os_hioF(0x98)) || os_LockonTruth(att) || (gabi::load<u32>(att + 0x20) & 0x20000000)) {
        s16 target = os_getStickAngY(this);
        cLib_addCalcAngleS(&current.angle.y, target, 0x19, 0x7FFF, 1);
        gabi::Local<cXyz> temp;
        s32 stickPos = os_calcStickPos(this, target, temp);
        gabi::Local<be<s16>> temp2;
        if (stickPos != 0 && os_stickValue() < os_hioF(0x9C)) {
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
        if (!(os_stickValue() < os_hioF(0x9C)) && stickPos == 0) {
            current.angle.y = target;
            os_pmf_player(this, OS_PMF_walkPlayerAction, nullptr);
        }
    } else {
        os_lookAtZero(this, &shape_angle.y, shape_angle.y, 0, 0);
        current.angle.y = shape_angle.y;
    }
    os_setAttention(this, os_isMorf(this));
    return TRUE;
}
VERIFY(0x022AFCE0, &daNpc_Os_c::waitPlayerAction);

/* 022AFF94 */
BOOL daNpc_Os_c::walkPlayerAction(void* param_1) {
    WWHD_FUNC(0x022AFF94, BOOL, this, param_1);
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
        if (frame > os_hioF(0xAC) && frame < os_hioF(0xA8)) {
            f32 s = (f32)((f64)os_hioF(0x08) * stickValue);
            speedF = s * os_hioF(0x20);
            stickPos = os_calcStickPos(this, target, temp);
        } else {
            speedF = 0.0f;
            cLib_addCalcAngleS(&current.angle.y, target, os_hioS(0x28), os_hioS(0x24), os_hioS(0x26));
            stickPos = os_calcStickPos(this, target, temp);
        }
        if (stickPos == 0) {
            cLib_addCalcAngleS(&shape_angle.y, current.angle.y, os_hioS(0x28), (s16)(os_hioS(0x24) * 2), (s16)(os_hioS(0x26) * 2));
        }
        gabi::Local<be<s16>> temp2;
        os_lookAtStick(this, temp2, temp);
        if (stickPos > 0) {
            shape_angle.y = *temp2;
        }
        if (stickPos != 0) {
            os_pmf_player(this, OS_PMF_waitPlayerAction, nullptr);
        }
    } else {
        os_pmf_player(this, OS_PMF_waitPlayerAction, nullptr);
    }
    os_setAttention(this, 1);
    return TRUE;
}
VERIFY(0x022AFF94, &daNpc_Os_c::walkPlayerAction);
