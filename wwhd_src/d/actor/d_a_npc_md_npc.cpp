/**
 * d_a_npc_md_npc.cpp (WWHD)
 * Player - Medli: NPC action functions (waitNpcAction .. land03NpcAction, windProc)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_md.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_md.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* dCamera_c::ForceLockOff(id) on dComIfGp_getCamera(0)->mCamera (camera at play+0x5AF8, mCamera +0x248) */
static inline void md_camForceLockOff(fopAc_ac_c* a) {
    u32 cam = gabi::load<u32>(dComIfGp_ea() + 0x5AF8);
    gabi::call(0x025052BC, cam + 0x248, gabi::load<u32>(gabi::ea(a) + 4) /* fopAcM_GetID */);
}
/* mDoExt_McaMorf2: J3DFrameCtrl at +0xA4 */
static inline J3DFrameCtrl* md_morf2Frame(mDoExt_McaMorf2* m) { return gabi::at<J3DFrameCtrl>(gabi::ea(m) + 0xA4); }
static inline void mDoAud_bgmStart_md(u32 id) { gabi::call(0x025E18EC, id); }

/* setNpcAction(&daNpc_Md_c::xxx, arg): the pointer to member is copied to a stack temporary */
static inline void md_setNpcAction(daNpc_Md_c* t, u32 pmf, void* arg = nullptr) {
    gabi::Local<ProcFunc_l> fn;
    md_pmf_load(fn, pmf);
    t->setNpcAction(fn, arg);
}
/* fopAcM_monsSeStart(this, id, &current.pos, dComIfGp_getReverb(roomNo)): 025E1A7C(id, pos, actorId, reverb) */
static inline void md_monsSeStart(fopAc_ac_c* a, u32 id) {
    s32 rv = dComIfGp_getReverb(a->current.roomNo);
    gabi::call(0x025E1A7C, id, &a->current.pos, gabi::load<u32>(gabi::ea(a) + 4), rv);
}
static inline dSv_event_c* md_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit_md(u16 f) { return dSv_event_isEventBit(md_event(), f); }
/* attention_info.flags (fopAc_ac_c +0x39C) */
static inline be<u32>& md_attnFlags(fopAc_ac_c* a) { return *gabi::at<be<u32>>(gabi::ea(a) + 0x39C); }
enum { fopAc_Attn_LOCKON_TALK_e = 0x2, fopAc_Attn_ACTION_SPEAK_e = 0x8, fopAc_Attn_ACTION_CARRY_e = 0x10 };
/* m312C = chkAttention(current.pos, shape_angle.y + getHead_y() + getBackbone_y(), 0)
 * (the cXyz argument is copied through FPRs) */
static inline u8 md_chkAttentionSelf(daNpc_Md_c* t) {
    gabi::Local<cXyz> pos;
    pos->x = (f32)t->current.pos.x;
    pos->y = (f32)t->current.pos.y;
    pos->z = (f32)t->current.pos.z;
    s16 angle = (s16)(t->shape_angle.y + t->mJntCtrl.mAngles[0][1] + t->mJntCtrl.mAngles[1][1]);
    /* chkAttention 0228DAF4: the caller stores the full r3 byte (bool result not normalised) */
    return gabi::call<u8>(0x0228DAF4, t, pos.get(), angle, 0);
}
/* dComIfGp_getVibration().StartShock(strength, flags, cXyz(0, 1, 0)) */
static inline void md_startShock(s32 strength, s32 flags) {
    gabi::Local<cXyz> up;
    up->x = 0.0f;
    up->y = 1.0f;
    up->z = 0.0f;
    dComIfGp_getVibration_StartShock(strength, flags, up);
}
/* 0201B3C0 cXyz::normalizeZP: normalises in place; HD passes a result slot in r4 */
static inline void cXyz_normalizeZP_md(cXyz* a, cXyz* out) { gabi::call(0x0201B3C0, a, out); }
/* 0257E1B8 dKyw_get_AllWind_vec(cXyz* pos, cXyz* outVec, f32* outPow) */
static inline void dKyw_get_AllWind_vec_md(cXyz* pos, cXyz* vec, be<f32>* pow) { gabi::call(0x0257E1B8, pos, vec, pow); }
static inline BOOL dCcD_ChkTgHit_md(dCcD_GObjInf* o) { return gabi::call<BOOL>(0x025162A4, o); }
static inline u32 dCcD_GetTgHitObj_md(dCcD_GObjInf* o) { return gabi::call<u32>(0x02516300, o); }
static inline s16 cLib_calcTimer_md(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
/* mDoExt_McaMorf::getFrame (frame at +0x9C) */
static inline f32 md_morfFrame(mDoExt_McaMorf* m) { return gabi::load<f32>(gabi::ea(m) + 0x9C); }
#define MD_L_MSGID 0x10467A34 /* static fpc_ProcID l_msgId */
/* mDoExt_McaMorf2::isMorf(): morf ratio (+0xB4) < 1.0f */
static inline bool md_morf2IsMorf(mDoExt_McaMorf2* m) { return gabi::load<f32>(gabi::ea(m) + 0xB4) < 1.0f; }
/* dComIfGp_getShipActor(): play+0x5B3C */
static inline fopAc_ac_c* md_getShipActor() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B3C)); }
/* daPy_getPlayerLinkActorClass()->onNpcCall(): link (play+0x5B34) mNoResetFlg1 |= 2 */
static inline void md_linkOnNpcCall() {
    u32 pl = gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYERPTR);
    gabi::store<u32>(pl + 0x3BC, gabi::load<u32>(pl + 0x3BC) | 2);
}
/* dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16 yrot, s16 vel, bool headOnly) */
static inline void md_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
static inline void dComIfGs_onEventBit_md(u16 f) { dSv_event_onEventBit(md_event(), f); }
/* daPy_lk_c::checkAutoJumpFlying() */
static inline s32 daPy_lk_checkAutoJumpFlying(u32 link) { return gabi::call<s32>(0x024430A4, link); }
/* daPy_npc_c::setRestart(s8) */
static inline void daPy_npc_setRestart(fopAc_ac_c* a, s8 opt) { gabi::call(0x02445438, a, opt); }
/* daPy_py_c::getBaseAnimeFrame(): virtual (HD vtable +0xB4, slot +0xA4) */
static inline f32 daPy_getBaseAnimeFrame(fopAc_ac_c* pl) {
    u32 vt = gabi::load<u32>(gabi::ea(pl) + 0xB4);
    return gabi::call_ptr<f32>(gabi::load<u32>(vt + 0xA4), pl);
}
/* mDoExt_McaMorf2 (HD): J3DFrameCtrl at +0xA4 (rate +0xA4, frame +0xA8, end +0xAE) */
static inline void mDoExt_McaMorf2_setMorf(mDoExt_McaMorf2* m, f32 f) { gabi::call(0x025E5D1C, m, f); }
/* McaMorf2::setFrame(f): HD converts the frame through s16 */
static inline f32 md_frameS16(f32 f) { return (f32)(s16)gabi::ftoi(f); }
enum : u32 { PMF_land02NpcAction = PMF_02291D50, PMF_land03NpcAction = PMF_02291E50 };

/* 02291BE4 */
BOOL daNpc_Md_c::land01NpcAction(void* param_1) {
    WWHD_FUNC(0x02291BE4, BOOL, this, param_1);
    if (mActionStatus == ACTION_STARTING) {
        md_monsSeStart(this, 0x48A7 /* JA_SE_CV_MD_LANDING */);
        if (checkStatus(daMdStts_UNK1)) {
            m3135 = m3135 | 1; /* setBitEffectStatus(1) */
        }
        maxFallSpeed = -100.0f;
        gravity = l_HIO().m0F4;
        setAnm(0xE);
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        shape_angle.x = 0;
        shape_angle.z = 0;
        speedF = 0.0f;
        m30F8 = 120.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
        m3144 = param_1 != nullptr ? 1 : 0;
    } else if (mActionStatus != ACTION_ENDING) {
        if (m3144 != 0) {
            if (!chkAdanmaeDemoOrder() && m312A != 0) {
                md_setNpcAction(this, PMF_waitNpcAction);
            }
        } else if (m312A != 0) {
            md_setNpcAction(this, PMF_waitNpcAction);
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x02291BE4, &daNpc_Md_c::land01NpcAction);

/* 02291D50 land02NpcAction (not named by the matcher) */
BOOL daNpc_Md_c::land02NpcAction(void*) {
    WWHD_FUNC(0x02291D50, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        maxFallSpeed = -100.0f;
        gravity = l_HIO().m0F4;
        setAnm(0xF);
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        shape_angle.x = 0;
        shape_angle.z = 0;
        speedF = 0.0f;
        m30F8 = 120.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (m312A != 0) {
            md_setNpcAction(this, PMF_piyo2NpcAction);
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x02291D50, &daNpc_Md_c::land02NpcAction);

/* 02291E50 land03NpcAction (not named by the matcher) */
BOOL daNpc_Md_c::land03NpcAction(void*) {
    WWHD_FUNC(0x02291E50, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        maxFallSpeed = -100.0f;
        gravity = l_HIO().m0F4;
        setAnm(0xF);
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        shape_angle.x = 0;
        shape_angle.z = 0;
        speedF = 0.0f;
        m30F8 = 120.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (m312A != 0) {
            md_setNpcAction(this, PMF_waitNpcAction);
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x02291E50, &daNpc_Md_c::land03NpcAction);

/* 0228FC24 */
BOOL daNpc_Md_c::kyohiNpcAction(void*) {
    WWHD_FUNC(0x0228FC24, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_FLY);
        setBitStatus(daMdStts_UNK4);
        setAnm(0x27);
        speedF = 0.0f;
        m30F8 = 120.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        m3144 = 2;
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (m312A != 0) {
            m3144 = m3144 - 1;
            if (m3144 == 0) {
                md_setNpcAction(this, PMF_waitNpcAction);
            }
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x0228FC24, &daNpc_Md_c::kyohiNpcAction);

/* 02291644 */
BOOL daNpc_Md_c::fallNpcAction(void*) {
    WWHD_FUNC(0x02291644, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        clearStatus(daMdStts_UNK1 | daMdStts_UNK4);
        setBitStatus(daMdStts_FLY);
        setAnm(0xC);
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (mAcch.ChkGroundHit()) {
            md_setNpcAction(this, PMF_land01NpcAction);
            md_camForceLockOff(this);
        } else if (mAcch.ChkWallHit()) {
            md_setNpcAction(this, PMF_wallHitNpcAction);
            md_camForceLockOff(this);
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x02291644, &daNpc_Md_c::fallNpcAction);

/* 02290234 */
BOOL daNpc_Md_c::squatdownNpcAction(void*) {
    WWHD_FUNC(0x02290234, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_FLY);
        setBitStatus(daMdStts_UNK4);
        setAnm(3);
        speedF = 0.0f;
        m30F8 = 75.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        m312C = md_chkAttentionSelf(this);
        if (m312A != 0) {
            md_setNpcAction(this, PMF_sqwait01NpcAction);
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x02290234, &daNpc_Md_c::squatdownNpcAction);

/* 02290358 */
BOOL daNpc_Md_c::sqwait01NpcAction(void*) {
    WWHD_FUNC(0x02290358, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        md_attnFlags(this) = md_attnFlags(this) | fopAc_Attn_ACTION_CARRY_e;
        if (dComIfGs_isEventBit_md(0x0E02 /* MEDLI_GAVE_FATHERS_LETTER */)) {
            md_attnFlags(this) = md_attnFlags(this) & ~(fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e);
        }
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_FLY);
        setBitStatus(daMdStts_UNK4);
        setAnm(4);
        maxFallSpeed = -100.0f;
        gravity = l_HIO().m0F4;
        speedF = 0.0f;
        m30F8 = 75.0f;
        shape_angle.x = 0;
        shape_angle.z = 0;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        m312C = md_chkAttentionSelf(this);
        if ((s8)(u8)mType >= 4) {
            md_setNpcAction(this, PMF_waitNpcAction);
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x02290358, &daNpc_Md_c::sqwait01NpcAction);

/* 0228F424 */
BOOL daNpc_Md_c::harpWaitNpcAction(void*) {
    WWHD_FUNC(0x0228F424, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        setHarpPlayNum(1);
        maxFallSpeed = -100.0f;
        gravity = l_HIO().m0F4;
        m30F0 = daMdStts_UNK80; /* clearStatus(); setBitStatus(UNK80) */
        shape_angle.x = 0;
        shape_angle.z = 0;
        speedF = 0.0f;
        m30F8 = 120.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
        m3144 = 0;
        m3146 = 0xC;
    } else if (mActionStatus != ACTION_ENDING) {
        if (mActionStatus == ACTION_ONGOING_1) {
            if (m312A != 0) {
                m3146 = m3146 - 1;
                if (m3146 != 0) {
                    setHarpPlayNum(1);
                } else {
                    setAnm(0x11);
                    mActionStatus = mActionStatus + 1;
                    m3146 = 0x78;
                }
            }
        } else {
            m3146 = m3146 - 1;
            if (m3146 == 0) {
                setHarpPlayNum(1);
                m3146 = 0xC;
                mActionStatus = ACTION_ONGOING_1;
            }
        }
        if (mType == 6 /* isTypeM_DaiB */ && m3146 == 0xC) {
            BOOL play = FALSE;
            if (m3104 == 0x16) {
                if (md_morf2Frame(mpMorf.get())->checkPass(11.0f)) {
                    play = TRUE;
                }
            } else if (m3104 == 0x17 && md_morf2Frame(mpMorf.get())->checkPass(11.0f)) {
                play = TRUE;
            }
            if (play) {
                mDoAud_bgmStart_md(0x80000060 /* JA_BGM_MEDORI_TAKT_8 */);
            }
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x0228F424, &daNpc_Md_c::harpWaitNpcAction);

/* 02291764 */
BOOL daNpc_Md_c::fall02NpcAction(void* param_1) {
    WWHD_FUNC(0x02291764, BOOL, this, param_1);
    if (mActionStatus == ACTION_STARTING) {
        clearStatus(daMdStts_UNK1 | daMdStts_UNK4);
        setBitStatus(daMdStts_FLY);
        maxFallSpeed = l_HIO().m110;
        gravity = l_HIO().m114;
        m30F8 = 120.0f;
        setAnm(0xD);
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
        m3144 = param_1 != nullptr ? 1 : 0;
        m3150 = current.pos.y;
    } else if (mActionStatus != ACTION_ENDING) {
        if (mType == 5 /* isTypeM_Dai */ && mAcch.ChkGroundHit()) {
            if (!(m3150 - current.pos.y < l_HIO().m1B0)) {
                md_startShock(6, -0x21);
            }
        }
        if (m3144 != 0) {
            if (!chkAdanmaeDemoOrder()) {
                if (mAcch.ChkGroundHit()) {
                    md_setNpcAction(this, PMF_land03NpcAction);
                    md_camForceLockOff(this);
                } else if (mAcch.ChkWallHit()) {
                    md_setNpcAction(this, PMF_wallHitNpcAction);
                    md_camForceLockOff(this);
                }
            }
        } else {
            if (mAcch.ChkGroundHit()) {
                md_setNpcAction(this, PMF_land03NpcAction);
                md_camForceLockOff(this);
            } else if (mAcch.ChkWallHit()) {
                md_setNpcAction(this, PMF_wallHitNpcAction);
                md_camForceLockOff(this);
            }
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x02291764, &daNpc_Md_c::fall02NpcAction);

/* 022919D4 */
BOOL daNpc_Md_c::wallHitNpcAction(void* param_1) {
    WWHD_FUNC(0x022919D4, BOOL, this, param_1);
    if (mActionStatus == ACTION_STARTING) {
        md_monsSeStart(this, 0x48A9 /* JA_SE_CV_MD_CRASH */);
        maxFallSpeed = -100.0f;
        gravity = l_HIO().m0F4;
        setAnm(0xD);
        shape_angle.x = 0;
        shape_angle.z = 0;
        int i = wallHitCheck();
        if (i >= 0) {
            current.angle.y = mAcchCir[i].m_wall_angle_y;
        }
        speedF = l_HIO().m150;
        clearStatus(daMdStts_UNK1 | daMdStts_UNK4);
        setBitStatus(daMdStts_UNK2 | daMdStts_FLY);
        m30F8 = 120.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
        md_startShock(5, -0x11);
        m3144 = param_1 != nullptr ? 1 : 0;
    } else if (mActionStatus != ACTION_ENDING) {
        if (m3144 != 0) {
            if (!chkAdanmaeDemoOrder() && mAcch.ChkGroundHit()) {
                md_setNpcAction(this, PMF_land02NpcAction);
                md_camForceLockOff(this);
            }
        } else if (mAcch.ChkGroundHit()) {
            md_setNpcAction(this, PMF_land02NpcAction);
            md_camForceLockOff(this);
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x022919D4, &daNpc_Md_c::wallHitNpcAction);

/* 02291094 */
BOOL daNpc_Md_c::throwNpcAction(void*) {
    WWHD_FUNC(0x02291094, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        md_monsSeStart(this, 0x48A6 /* JA_SE_CV_MD_THROW */);
        m314C = 1;
        m314E = l_HIO().m1B4;
        m3148 = 0;
        m3154 = 0.0f;
        speedF = l_HIO().m0E8;
        speed.y = l_HIO().m0EC;
        maxFallSpeed = l_HIO().m110;
        gravity = l_HIO().m114;
        m30A0.copy(current.pos);
        clearStatus(daMdStts_UNK4);
        setBitStatus(daMdStts_UNK1 | daMdStts_FLY);
        setAnm(6);
        setWingEmitter();
        mAcchCir[1].SetWall(60.0f, 60.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus == ACTION_ENDING) {
        emitterDelete(m0508);
    } else {
        f32 frame = md_morfFrame(mpWingMorf.get());
        if (frame == 23.0f && speed.y < l_HIO().m0F0) {
            speed.y = l_HIO().m0F0;
        }
        if (frame > l_HIO().m154) { /* ble: taken on NaN in the recompiled code */
            if (windProc() == 0) {
                m314C = 0;
            }
        }
        if (mAcch.ChkGroundHit()) {
            md_setNpcAction(this, PMF_land01NpcAction);
            md_camForceLockOff(this);
        } else if (mAcch.ChkWallHit()) {
            md_setNpcAction(this, PMF_wallHitNpcAction);
            md_camForceLockOff(this);
        } else if (m312A != 0) {
            md_setNpcAction(this, PMF_glidingNpcAction);
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x02291094, &daNpc_Md_c::throwNpcAction);

/* 02291304 */
BOOL daNpc_Md_c::glidingNpcAction(void*) {
    WWHD_FUNC(0x02291304, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        if (m314C == 1) {
            m3148 = 0;
            setAnm(0xB);
        } else {
            m3148 = 1;
            setAnm(0xA);
        }
        m314A = 2;
        clearStatus(daMdStts_UNK4);
        setBitStatus(daMdStts_UNK1 | daMdStts_FLY);
        mAcchCir[1].SetWall(60.0f, 60.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (!chkAdanmaeDemoOrder()) {
            if (mAcch.ChkGroundHit()) {
                md_setNpcAction(this, PMF_land01NpcAction, mType == 1 /* isTypeAdanmae */ ? (void*)&m3144 : nullptr);
                md_camForceLockOff(this);
            } else if (mAcch.ChkWallHit()) {
                md_setNpcAction(this, PMF_wallHitNpcAction, mType == 1 ? (void*)&m3144 : nullptr);
                md_camForceLockOff(this);
            } else {
                if (m3148 == 0) {
                    /* the full r3 of windProc is compared with the sign-extended m314C */
                    s32 wind = gabi::call<s32>(0x02290CF8, this);
                    if ((u32)(s32)m314C != (u32)wind) {
                        if (wind == 0) {
                            setAnm(0xA);
                            m3148 = 1;
                        } else {
                            setAnm(0xB);
                        }
                    }
                    m314C = (s16)wind;
                } else if (m312A != 0) {
                    m314A = m314A - 1;
                    if (m314A != 0) {
                        setAnm(0xA);
                    } else {
                        md_setNpcAction(this, PMF_fall02NpcAction, mType == 1 ? (void*)&m3144 : nullptr);
                        return TRUE;
                    }
                }
                gabi::Local<cXyz> tmp; /* cXyz cStack_68(current.pos - m30A0), unused */
                cXyz_mi(&current.pos, tmp, &m30A0);
            }
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x02291304, &daNpc_Md_c::glidingNpcAction);

/* 02290CF8 */
s16 daNpc_Md_c::windProc() {
    WWHD_FUNC(0x02290CF8, s16, this);
    s16 ret = 1;
    gabi::Local<be<f32>> pow;
    gabi::Local<cXyz> wind;
    if (checkStatus(daMdStts_UNK8)) {
        dKyw_get_AllWind_vec_md(&current.pos, wind, pow);
    } else {
        wind->z = 0.0f;
        wind->y = 0.0f;
        wind->x = 0.0f;
        *pow = 0.0f;
    }
    if (l_HIO().m1C6 != 0 && dCcD_ChkTgHit_md(&mCyl2)) {
        u32 obj = dCcD_GetTgHitObj_md(&mCyl2);
        if (obj != 0 && (gabi::load<u32>(obj + 0x10) & 0x200000 /* AT_TYPE_WIND */)) {
            cXyz* rvec = gabi::at<cXyz>(gabi::ea(&mCyl2) + 0xC0); /* GetTgRVecP */
            f32 z = rvec->z, y = rvec->y, x = rvec->x;
            wind->z = z;
            wind->y = y;
            wind->x = x;
            gabi::Local<cXyz> res;
            cXyz_normalizeZP_md(wind, res);
            *pow = (f32)l_HIO().m148;
        }
    }
    f32 wx = wind->x;
    f32 sinY = cM_ssin(shape_angle.y);
    f32 cosY = cM_scos(shape_angle.y);
    f32 rate = l_HIO().m134;
    gabi::Local<cXyz> windXZ; /* cStack_64 */
    gabi::Local<cXyz> tmpXZ;  /* abs2XZ temporary */
    windXZ->y = 0.0f;
    tmpXZ->y = 0.0f;
    f32 wz = wind->z;
    windXZ->z = wz;
    tmpXZ->z = wz;
    tmpXZ->x = wx;
    windXZ->x = wx;
    f32 sq = PSVECSquareMag(tmpXZ);
    if (!(std::fabs(sq) < 3.814697265625e-06f)) {
        gabi::Local<cXyz> res;
        cXyz_normalizeZP_md(windXZ, res);
        f32 dot = gabi::fmadds(sinY, windXZ->x, cosY * windXZ->z);
        if (!(dot > 0.7071f)) {
            ret = 0;
            rate = l_HIO().m130;
        }
    }
    gabi::Local<cXyz> add;  /* local_34 * local_a4 */
    gabi::Local<cXyz> sum;  /* speed + add */
    gabi::Local<cXyz> sumXZ;
    if (ret == 0) {
        f32 limit = *pow * rate;
        f32 v = m3154 + l_HIO().m138;
        if (v > limit) {
            v = limit;
        }
        m3154 = v;
        *pow = v;
        cXyz_ml(wind, add, v);
    } else {
        f32 v = *pow * rate;
        *pow = v;
        cXyz_ml(wind, add, v);
    }
    cXyz_pl(&speed, sum, add);
    current.angle.y = cM_atan2s(sum->x, sum->z);
    sumXZ->x = (f32)sum->x;
    sumXZ->y = 0.0f;
    sumXZ->z = (f32)sum->z;
    f32 spd = std_sqrtf(PSVECSquareMag(sumXZ));
    speedF = spd;
    f32 maxSpd = l_HIO().m13C;
    speed.y = (f32)sum->y;
    speedF = (spd - maxSpd >= 0.0f) ? maxSpd : spd; /* cLib_maxLimit (fsel) */
    if (m3148 == 0) {
        maxFallSpeed = l_HIO().m110;
        gravity = l_HIO().m114;
    }
    if (!(std::fabs((f32)*pow) < 3.814697265625e-06f)) {
        s16 t = cLib_calcTimer_md(&m314E);
        s16 lim = (s16)(l_HIO().m1B4 - l_HIO().m1B6);
        if (ret == 1 && t > 0 && t < lim && (l_HIO().m1C7 != 0 || m3148 == 0)) {
            maxFallSpeed = l_HIO().m140;
            gravity = l_HIO().m144;
        }
    }
    return ret;
}
VERIFY(0x02290CF8, &daNpc_Md_c::windProc);

/* 0228F610 */
BOOL daNpc_Md_c::talkNpcAction(void*) {
    WWHD_FUNC(0x0228F610, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        gabi::store<u32>(MD_L_MSGID, fpcM_ERROR_PROCESS_ID_e);
        mMsgNo = getMsg();
        md_attnFlags(this) = md_attnFlags(this) & ~fopAc_Attn_ACTION_CARRY_e;
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_FLY);
        setBitStatus(daMdStts_UNK4);
        m312B = 0;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        m312C = md_chkAttentionSelf(this);
        if (mActionStatus == ACTION_ONGOING_1) {
            if (XYTalkCheck()) {
                mActionStatus = ACTION_ONGOING_2;
            }
        } else if (mActionStatus == ACTION_ONGOING_2) {
            if (talk_init()) {
                mActionStatus = ACTION_ONGOING_3;
            }
        } else if (mActionStatus == ACTION_ONGOING_3) {
            if (talk(0)) {
                if (mType == 0 /* isTypeAtorizk */ || mType == 2 /* isTypeM_Dra09 */) {
                    mActionStatus = 4;
                } else if (mType == 1 /* isTypeAdanmae */) {
                    if (dComIfGs_isEventBit_md(0x1104)) {
                        md_setNpcAction(this, PMF_squatdownNpcAction);
                    }
                } else {
                    md_setNpcAction(this, PMF_waitNpcAction);
                }
                dComIfGp_event_reset();
            }
        } else if (mType == 0) {
            if (dComIfGs_isEventBit_md(0x0E02 /* MEDLI_GAVE_FATHERS_LETTER */)) {
                md_setNpcAction(this, PMF_waitNpcAction);
            }
        } else if (mType == 2 && dComIfGs_isEventBit_md(0x1101)) {
            md_setNpcAction(this, PMF_waitNpcAction);
        }
        mJntCtrl.mbTrn = 1; /* setTrn() */
        lookBack(1, 0, 0);
        current.angle.y = shape_angle.y;
        setAttention(md_morf2IsMorf(mpMorf.get()));
    }
    return TRUE;
}
VERIFY(0x0228F610, &daNpc_Md_c::talkNpcAction);

/* 0228FA44 */
BOOL daNpc_Md_c::shipTalkNpcAction(void*) {
    WWHD_FUNC(0x0228FA44, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        mNoResetFlg1 = mNoResetFlg1 | 0x40; /* onNpcNotChange() */
        setBitStatus(daMdStts_SHIP_RIDE);  /* onShipRide() */
        gabi::store<u32>(MD_L_MSGID, fpcM_ERROR_PROCESS_ID_e);
        mMsgNo = getMsg();
        setAnm(0x2B);
        m312B = 0;
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus == ACTION_ENDING) {
        clearStatus(daMdStts_SHIP_RIDE); /* offShipRide() */
    } else {
        m312C = md_chkAttentionSelf(this);
        if (mActionStatus == ACTION_ONGOING_1) {
            if (talk_init()) {
                mActionStatus = ACTION_ONGOING_2;
            }
        } else {
            if (talk(0)) {
                dComIfGp_event_reset();
                md_setNpcAction(this, PMF_shipNpcAction);
            }
        }
        lookBack(1, 0, 1);
        current.angle.y = shape_angle.y;
        setAttention(md_morf2IsMorf(mpMorf.get()));
    }
    return TRUE;
}
VERIFY(0x0228FA44, &daNpc_Md_c::shipTalkNpcAction);

/* 0228FD20 */
BOOL daNpc_Md_c::shipNpcAction(void*) {
    WWHD_FUNC(0x0228FD20, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        mNoResetFlg1 = mNoResetFlg1 | 0x40; /* onNpcNotChange() */
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_FLY);
        setBitStatus(daMdStts_UNK4);
        setAnm(0x23);
        speedF = 0.0f;
        m30F8 = 120.0f;
        if (md_getShipActor() != nullptr) {
            setBitStatus(daMdStts_SHIP_RIDE);
        }
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus == ACTION_ENDING) {
        clearStatus(daMdStts_SHIP_RIDE);
    } else {
        md_attnFlags(this) = md_attnFlags(this) & ~(fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e);
        fopAc_ac_c* ship = md_getShipActor();
        if (ship != nullptr) {
            setBitStatus(daMdStts_SHIP_RIDE);
            if (std::fabs((f32)ship->speedF) < 0.001f) {
                setAnm(0x2A);
                if (!(gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER_STATUS0) & 0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
                    m312C = md_chkAttentionSelf(this);
                    if (m312C != 0) {
                        md_attnFlags(this) = md_attnFlags(this) | (fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e);
                        mCurEventMode = 2;
                    }
                }
            } else {
                setAnm(0x23);
            }
        }
        if (m3104 == 0x23) {
            gabi::Local<cXyz> eye; /* cXyz::Zero by value (FPR copy) */
            cXyz* zero = gabi::at<cXyz>(0x101FFBA8);
            f32 x = zero->x, z = zero->z, y = zero->y;
            eye->z = z;
            eye->x = x;
            eye->y = y;
            md_lookAtTarget(&mJntCtrl, &shape_angle.y, nullptr, eye, shape_angle.y, 0, 0);
        } else {
            lookBack(1, 0, 1);
        }
        current.angle.y = shape_angle.y;
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x0228FD20, &daNpc_Md_c::shipNpcAction);

/* 0228FF5C */
BOOL daNpc_Md_c::mwaitNpcAction(void*) {
    WWHD_FUNC(0x0228FF5C, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        md_attnFlags(this) = md_attnFlags(this) | fopAc_Attn_ACTION_CARRY_e;
        mNoResetFlg1 = mNoResetFlg1 & ~2u; /* offNpcCallCommand() */
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        setAnm(0x20);
        shape_angle.x = 0;
        shape_angle.z = 0;
        gravity = l_HIO().m0F4;
        maxFallSpeed = -100.0f;
        speedF = 0.0f;
        speed.y = 0.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (mActionStatus == ACTION_ONGOING_1) {
            if (!checkStatus(daMdStts_LIGHT_BODY_HIT)) {
                mActionStatus = ACTION_ONGOING_3;
            } else {
                md_attnFlags(this) = md_attnFlags(this) & ~(fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e);
                f32 dist2 = fopAcM_searchActorDistance2(this, dComIfGp_getPlayer(0));
                if (!(mNoResetFlg1 & 2) /* !checkNpcCallCommand() */) {
                    if (dist2 < l_HIO().m0C8 * l_HIO().m0C8) {
                        md_linkOnNpcCall();
                    }
                } else {
                    mActionStatus = mActionStatus + 1;
                    f32 r = l_HIO().m0C4 + l_HIO().m0C4; /* 2.0f * m0C4 */
                    if (dist2 < r * r) {
                        mActionStatus = mActionStatus + 1;
                    }
                }
            }
            if (mActionStatus != ACTION_ONGOING_1) {
                m311A = 0;
                m310C = 0.0f;
                m3114 = 0;
                m3116 = 0;
                setAnm(0x21);
                m312A = 0;
            }
        } else {
            if (lookBackWaist(m311A, m310C) && m312A != 0) {
                m3114 = 0;
                m3116 = 0;
                if (mActionStatus == ACTION_ONGOING_2) {
                    md_setNpcAction(this, PMF_searchNpcAction);
                } else {
                    setAnm(0);
                    md_setNpcAction(this, PMF_waitNpcAction);
                }
            }
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x0228FF5C, &daNpc_Md_c::mwaitNpcAction);

/* 0228EB88 */
BOOL daNpc_Md_c::waitNpcAction(void*) {
    WWHD_FUNC(0x0228EB88, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        md_attnFlags(this) = md_attnFlags(this) & ~fopAc_Attn_ACTION_CARRY_e;
        if (mType == 3 /* isTypeSea */) {
            s32 m = m312D; /* compared unsigned after sign extension */
            if (m3104 == 0x1E || m3104 == 0x29 || (u32)m == 0xE || (u32)m == 0x12 || (u32)m == 0x13 ||
                (u32)m == 0x1A || (u32)m == 0x25) {
                setHarpPlayNum(1);
            } else {
                setAnm(0x12);
            }
        } else if (mType == 2 /* isTypeM_Dra09 */ && !dComIfGs_isEventBit_md(0x1140)) {
            setAnm(0x22);
        } else if (mType == 5 || mType == 4 /* isTypeM_Dai || isTypeEdaichi */) {
            if (!checkStatus(0x80)) {
                setAnm(0);
            }
        } else {
            setAnm(0);
        }
        maxFallSpeed = -100.0f;
        gravity = l_HIO().m0F4;
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_FLY);
        if (mType != 3) {
            setBitStatus(daMdStts_UNK4);
        } else {
            clearStatus(daMdStts_UNK4);
        }
        shape_angle.x = 0;
        shape_angle.z = 0;
        speedF = 0.0f;
        m30F8 = 120.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        m3144 = 150;
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (m312A != 0) {
            if (m3104 == 0x12) {
                setHarpPlayNum(1);
            } else if (m3104 == 0x13 || m3104 == 0x21) {
                setAnm(0);
            } else if (m3104 == 0x16 || m3104 == 0x17 || m3104 == 0x11) {
                setHarpPlayNum(1);
            }
        }
        if (mType == 1 /* isTypeAdanmae */ && dComIfGs_isEventBit_md(0x1102)) {
            fopAcM_delete(this);
            return TRUE;
        }
        gabi::Local<be<s32>> sp08;
        *sp08 = 0;
        if (mType == 3 && !dComIfGs_isEventBit_md(0x1402)) {
            /* HD: only while the player is Link */
            u32 link = gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYERPTR);
            if (link == gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER)) {
                f32 distXZ2 = fopAcM_searchPlayerDistanceXZ2(this);
                f32 distY = gabi::load<f32>(link + 0x318) - current.pos.y;
                if (distXZ2 < l_HIO().m0CC * l_HIO().m0CC && distY < l_HIO().m0BC && distY > l_HIO().m0C0) {
                    mCurEventMode = 0xB;
                }
            }
        } else {
            gabi::Local<cXyz> pos;
            pos->x = (f32)current.pos.x;
            pos->y = (f32)current.pos.y;
            pos->z = (f32)current.pos.z;
            s16 headAngle = (s16)(shape_angle.y + mJntCtrl.mAngles[0][1] + mJntCtrl.mAngles[1][1]);
            md_attnFlags(this) = md_attnFlags(this) & ~(fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e);
            u32 attn = gabi::call<u32>(0x0228DAF4, this, pos.get(), headAngle, 1); /* chkAttention */
            *sp08 = attn;
            m312C = (u8)attn;
            if (attn != 0) {
                if (mType == 0 || mType == 1 || mType == 3) {
                    md_attnFlags(this) = md_attnFlags(this) | (fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e);
                }
                if (mType == 0 || mType == 1) {
                    mCurEventMode = 2;
                } else if (mType == 2) {
                    if (dComIfGs_isEventBit_md(0x1140)) {
                        if (dComIfGs_isEventBit_md(0x1101)) {
                            md_attnFlags(this) = md_attnFlags(this) | fopAc_Attn_ACTION_SPEAK_e;
                            if (gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER_STATUS0) & 0x800000 /* daPyStts0_UNK800000_e */) {
                                dComIfGs_onEventBit_md(0x1280);
                            }
                            if (dComIfGs_isEventBit_md(0x1280)) {
                                mCurEventMode = 2;
                            } else {
                                mCurEventMode = 1;
                            }
                        } else {
                            md_attnFlags(this) = md_attnFlags(this) | (fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e);
                            mCurEventMode = 2;
                        }
                    } else {
                        setAnm(0x22);
                    }
                } else if (mType == 3) {
                    if (mCurEventMode == 0 && dComIfGs_isEventBit_md(0x1402)) {
                        mCurEventMode = 3;
                    }
                } else {
                    md_attnFlags(this) = md_attnFlags(this) | fopAc_Attn_ACTION_CARRY_e;
                }
            }
            NpcCall(sp08);
        }
        if (mType == 2 && dComIfGs_isEventBit_md(0x1101) && dComIfGs_isSwitch(m3100, current.roomNo)) {
            md_setNpcAction(this, PMF_demoFlyNpcAction);
        }
        if (mType == 5 || mType == 4) {
            if (m3104 == 0) {
                if (cLib_calcTimer_md(&m3144) == 0) {
                    setAnm(40);
                }
            } else if (m3104 == 0x28 && m312A != 0) {
                setAnm(0);
                m3144 = (s16)(gabi::ftoi(cM_rndF(180.0f)) + 60);
            }
        }
        if (mType == 0) {
            if (chkArea(&current.pos)) {
                lookBack(*sp08, 0, 1);
            } else {
                lookBack(0, 0, 0);
            }
        } else if (mType == 2) {
            if (dComIfGs_isEventBit_md(0x1140)) {
                lookBack(1, 0, 0);
                setAnm(0);
            } else {
                lookBack(1, 0, 1);
            }
        } else {
            if (m3104 == 0x16 || m3104 == 0x17) {
                gabi::Local<cXyz> eye; /* cXyz::Zero by value (FPR copy) */
                cXyz* zero = gabi::at<cXyz>(0x101FFBA8);
                f32 y = zero->y, x = zero->x, z = zero->z;
                eye->y = y;
                eye->z = z;
                eye->x = x;
                md_lookAtTarget(&mJntCtrl, &shape_angle.y, nullptr, eye, shape_angle.y, 0, 0);
            }
            if (chkArea(&current.pos)) {
                if (mType == 3) {
                    lookBack(*sp08, 0, 1);
                } else {
                    lookBack(*sp08, 0, 0);
                }
            } else {
                lookBack(0, 0, 0);
            }
        }
        waitGroundCheck();
        current.angle.y = shape_angle.y;
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x0228EB88, &daNpc_Md_c::waitNpcAction);

/* 022904DC */
BOOL daNpc_Md_c::carryNpcAction(void*) {
    WWHD_FUNC(0x022904DC, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        md_attnFlags(this) = md_attnFlags(this) & ~fopAc_Attn_ACTION_CARRY_e;
        mNoResetFlg1 = (mNoResetFlg1 | 0x40) & ~2u; /* onNpcNotChange(); offNpcCallCommand() */
        fopAc_ac_c* pl = dComIfGp_getPlayer(0);
        m3144 = (s16)(shape_angle.y - pl->shape_angle.y); /* fopAcM_toPlayerShapeAngleY */
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        u32 se;
        if (checkStatus(daMdStts_CARRY_ACTION) /* isNoCarryAction */) {
            setAnm(0x24);
            setBitStatus(daMdStts_UNK1);
            setHane02Emitter();
            se = 0x48A9; /* JA_SE_CV_MD_CRASH */
        } else {
            if (m3104 == 4) {
                setAnm(5);
            } else {
                setAnm(3);
                m312A = 0;
            }
            se = 0x48A5; /* JA_SE_CV_MD_LIFT_UP */
        }
        md_monsSeStart(this, se);
        m30F8 = 75.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus == ACTION_ENDING) {
        m3298.y = 0.0f;
        m3298.z = 0.0f;
        mNoResetFlg1 = mNoResetFlg1 & ~0x40u; /* offNpcNotChange() */
        clearStatus(daMdStts_CARRY_ACTION);   /* offNoCarryAction() */
        deleteHane02Emitter();
        deleteHane03Emitter();
        emitterDelete(m0508);
    } else {
        m3131 = 0;
        BOOL landed = FALSE;
        if (actor_status & 0x2000 /* fopAcM_checkCarryNow */) {
            daPy_npc_setRestart(this, 2);
        }
        if (!checkStatus(daMdStts_CARRY_ACTION)) {
            s32 flying = daPy_lk_checkAutoJumpFlying(gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYERPTR));
            fopAc_ac_c* player = dComIfGp_getPlayer(0);
            if (mActionStatus == ACTION_ONGOING_1) {
                if (flying > 0) {
                    mActionStatus = ACTION_ONGOING_2;
                    setAnm(9);
                    setBitStatus(daMdStts_UNK1);
                    setWingEmitter();
                    setHane03Emitter();
                    m312A = 0;
                    md_monsSeStart(this, 0x491F /* JA_SE_CV_MD_FLY_WITH_LINK */);
                    if (dComIfGs_isEventBit_md(0x4001)) {
                        dComIfGs_onEventBit_md(0x4180);
                    }
                } else if (m3104 == 5) {
                    f32 spdF = player->speedF;
                    u32 morf = gabi::ea(mpMorf.get());
                    f32 oldRate = gabi::load<f32>(morf + 0xA4); /* mpMorf->getPlaySpeed() */
                    if (spdF == 0.0f) {
                        f32 f = daPy_getBaseAnimeFrame(player);
                        f32 end = (f32)gabi::load<s16>(morf + 0xAE); /* mpMorf->getEndFrame() */
                        morf = gabi::ea(mpMorf.get());
                        if (!(f > end)) {
                            f32 fr = daPy_getBaseAnimeFrame(player);
                            gabi::store<f32>(morf + 0xA8, md_frameS16(fr));
                            gabi::store<f32>(gabi::ea(mpMorf.get()) + 0xA4, 0.0f);
                            u32 arm = gabi::ea(mpArmMorf.get());
                            fr = daPy_getBaseAnimeFrame(player);
                            gabi::store<f32>(arm + 0xA8, md_frameS16(fr));
                            gabi::store<f32>(gabi::ea(mpArmMorf.get()) + 0xA4, 0.0f);
                        } else {
                            gabi::store<f32>(morf + 0xA4, 1.0f);
                            gabi::store<f32>(gabi::ea(mpArmMorf.get()) + 0xA4, 1.0f);
                        }
                    } else {
                        gabi::store<f32>(morf + 0xA4, 1.0f);
                        gabi::store<f32>(gabi::ea(mpArmMorf.get()) + 0xA4, 1.0f);
                    }
                    mDoExt_McaMorf2* m = mpMorf.get();
                    if (!(gabi::load<f32>(gabi::ea(m) + 0xA4) == oldRate)) {
                        mDoExt_McaMorf2_setMorf(m, 4.0f);
                        mDoExt_McaMorf2_setMorf(mpArmMorf.get(), 4.0f);
                    }
                    cLib_chaseF(&m3298.x, l_HIO().m0F8, 1.0f);
                    cLib_chaseF(&m3298.y, l_HIO().m0FC, 1.0f);
                    cLib_chaseF(&m3298.z, l_HIO().m100, 1.0f);
                } else if (m312A != 0) {
                    setAnm(5);
                }
            } else if (mActionStatus == ACTION_ONGOING_2) {
                if (flying < 0x1E) {
                    mActionStatus = ACTION_ONGOING_3;
                    gabi::store<f32>(gabi::ea(mpMorf.get()) + 0xA4, l_HIO().m120);      /* setPlaySpeed */
                    gabi::store<f32>(gabi::ea(mpWingMorf.get()) + 0x98, l_HIO().m120); /* setPlaySpeed */
                }
            } else if (flying <= 0) {
                mActionStatus = ACTION_ONGOING_1;
                m3135 = m3135 | 1; /* setBitEffectStatus(1) */
                deleteHane03Emitter();
                setAnm(5);
                clearStatus(daMdStts_UNK1);
                landed = TRUE;
                md_monsSeStart(this, 0x4920 /* JA_SE_CV_MD_FLY_END */);
            }
        }
        if (!(actor_status & 0x2000)) {
            if (checkStatus(daMdStts_CARRY_ACTION)) {
                if (speedF > 0.0f) {
                    md_setNpcAction(this, PMF_throwNpcAction);
                } else {
                    md_setNpcAction(this, PMF_land03NpcAction);
                }
            } else if ((m30F0 & daMdStts_UNK1) || landed) {
                if (!(current.pos.y - mAcch.GetGroundH() < l_HIO().m1AC)) {
                    md_setNpcAction(this, PMF_fall02NpcAction);
                } else {
                    md_setNpcAction(this, PMF_fallNpcAction);
                }
            } else if (speedF > 0.0f) {
                md_setNpcAction(this, PMF_throwNpcAction);
            } else {
                md_setNpcAction(this, PMF_sqwait01NpcAction);
            }
        }
        gabi::Local<cXyz> eye; /* cXyz::Zero by value (FPR copy) */
        cXyz* zero = gabi::at<cXyz>(0x101FFBA8);
        f32 x = zero->x, z = zero->z, y = zero->y;
        eye->x = x;
        eye->z = z;
        eye->y = y;
        md_lookAtTarget(&mJntCtrl, &shape_angle.y, nullptr, eye, shape_angle.y, m3110, 0);
        setAttention(true); /* HD */
        current.angle.y = shape_angle.y;
    }
    return TRUE;
}
VERIFY(0x022904DC, &daNpc_Md_c::carryNpcAction);
