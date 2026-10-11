/**
 * d_a_npc_md.cpp (WWHD)
 * Player - Medli
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_md.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * The other functions are in d_a_npc_md_<part>.cpp (separate verification units).
 */
#include "d/actor/d_a_npc_md_main.h"
using namespace MdMain;

static u8 m_flying;
static u8 m_mirror;
static s16 m_flyingTimer;
static u32 playerOffsetGuard;
static cXyz playerOffset;
static cXyz l_ms_light_local_start;
static cXyz l_ms_light_local_vec;
static f32 l_fan_light_radius;
static SavePointer saveInfo;
static ActionWords action_waitNpc;
static ActionWords action_waitPlayer;
static ActionWords action_jumpPlayer;
static ActionWords action_land03Npc;
static ActionWords action_walkPlayer;
static ActionWords action_jumpNpc;
static ActionWords action_returnToPlayerNpc;
static ActionWords action_kyohiNpc;
static ActionWords action_landPlayer;
static inline void mdMain_copyAction(ProcFunc_l* dst, const ActionWords& src) {
    auto* words = reinterpret_cast<ActionWords*>(dst);
    words->mWords[0] = src.mWords[0];
    words->mWords[1] = src.mWords[1];
}

/* Inlined helpers. */
/* mDoAud_monsSeStart(id, pos, procId, reverb) (fopAcM_monsSeStart inlined)  */
static inline void mDoAud_monsSeStart_md(u32 id, cXyz* pos, u32 procId, s32 reverb) { mdMain_monsSeStart(id, pos, procId, reverb); }
/* cXyz::outprod(const Vec&) const: result through a hidden pointer (r4)  */
static inline void cXyz_outprod_md(const cXyz* a, cXyz* out, const cXyz* b) { mdMain_outprod(a, out, b); }

static inline u32 md_procId(fopAc_ac_c* actor) { return actor->mBsPcId; } /* base.base.mBsPcId */
static inline be<u32>& md_attnFlags(fopAc_ac_c* actor) { return actor->attention_info.flags; }

/* ---- HIO (constructors allocate when this == NULL) ---- */

/* dNpc_HIO_c::dNpc_HIO_c  */
static inline void dNpc_HIO_c_ct(dNpc_HIO_l* p) { mdMain_constructNpcHio(p); }

/*  deleting destructor of a class without member destructors (vtable ;
 * new: compiler-generated) */

/*  deleting destructor: member at +0x14 (destructor ); not referenced
 * (new: compiler-generated) */

/*  deleting destructor: members at +0x158 () and +4 (); not referenced
 * (new: compiler-generated) */

/* cLib_calcTimer<s16> (out of line)  */
static inline s16 md_calcTimer(be<s16>* t) { return mdMain_calcTimer(t); }
/* setNpcAction / setPlayerAction with a PMF constant (by-value copy on the stack) */
#define MD_SET_NPC_ACTION(pmf)              \
    do {                                    \
        gabi::Local<ProcFunc_l> fn_;        \
        mdMain_copyAction(fn_, pmf);              \
        setNpcAction(fn_, nullptr);         \
    } while (0)
#define MD_SET_PLAYER_ACTION(pmf)           \
    do {                                    \
        gabi::Local<ProcFunc_l> fn_;        \
        mdMain_copyAction(fn_, pmf);              \
        setPlayerAction(fn_, nullptr);      \
    } while (0)
/* chkAttention(cXyz pos, ...): pos by value (a stack copy through FPRs) */
/* the result is a byte register (GHS bool): callers store it unnormalised */
/* inlined into daNpc_Md_c::piyo2NpcAction */
static inline u8 md_chkAttention(daNpc_Md_c* i_this, cXyz* pos, s16 angle, int p) {
    cXyz tmp;
    f32 x = pos->x, y = pos->y;
    tmp.x = x;
    f32 z = pos->z;
    tmp.y = y;
    tmp.z = z;
    return mdMain_checkAttention(i_this, &tmp, angle, p);
}

/*  dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16, s16, bool) */
static inline void md_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, bool headOnly) {
    mdMain_lookAtTarget(j, outY, target, eye, yrot, vel, headOnly);
}

/* this TU's copies of the dBgS_GndChk / dBgS_LinChk vtables */
static const dBgS_GndChk_vt MD_GNDCHK_VT = {GROUND_CHECK_VTABLE, GROUND_POLY_INFO_VTABLE, GROUND_POLY_PASS_VTABLE, GROUND_GROUP_PASS_VTABLE};
static const dBgS_LinChk_vt MD_LINCHK_VT = {LINE_CHECK_VTABLE, LINE_BASE_VTABLE, LINE_POLY_PASS_VTABLE, LINE_GROUP_PASS_VTABLE};
static inline void md_GndChk_dt(dBgS_GndChk* check) {
    check->cBgS_PolyInfo::__vtbl = GROUND_POLY_INFO_VTABLE;
    check->dBgS_PolyPassChk::__vtbl = GROUND_POLY_PASS_VTABLE;
    check->dBgS_GrpPassChk::__vtbl = GROUP_PASS_VTABLE;
    mdMain_destroyGroundCheck(check, 0);
}
static inline void md_LinChk_dt(dBgS_LinChk* check) {
    check->dBgS_PolyPassChk::__vtbl = LINE_POLY_PASS_VTABLE;
    check->dBgS_GrpPassChk::__vtbl = GROUP_PASS_VTABLE;
    check->cBgS_PolyInfo::__vtbl = LINE_POLY_INFO_VTABLE;
    mdMain_destroyLineCheck(check, 0);
}

/*  cLib_offsetPos(cXyz* dst, const cXyz* base, s16 angle, const cXyz* offset) */
static inline void md_cLib_offsetPos(cXyz* dst, cXyz* base, s16 angle, cXyz* ofs) { mdMain_offsetPos(dst, base, angle, ofs); }

/*  HD-only npc action (new; PMF ): once no event runs, Medli is placed
 * behind the player (offset (-100, 0, -150) in the player's frame), takes the current room as
 * her home and goes back to waitNpcAction */

/* CPad_GET_STICK_VALUE(port) (HD: out of line)  */
static inline f32 md_stickValue(s32 port) { return mdMain_stickValue(port); }
/* setPlayerAction(PMF, &arg) */
static inline void md_setPlayerAction(daNpc_Md_c* i_this, const ActionWords& pmf, void* arg) {
    gabi::Local<ProcFunc_l> fn;
    mdMain_copyAction(fn, pmf);
    i_this->setPlayerAction(fn, arg);
}

/* dAttention_c::Lockon(): LockonTruth() || (flags (+0x20) & ); attention at play+0x5804 */
static inline bool md_attention_Lockon(Attention* att) {
    if (mdMain_lockonTruth(att) != 0) /* dAttention_c::LockonTruth */
        return true;
    return (att->mFlags & 0x20000000) != 0;
}

/* mDoExt_McaMorf2::isMorf(): the morf rate (+0xB4) below 1 */
static inline bool md_isMorf(mDoExt_McaMorf2* m) { return reinterpret_cast<Morf2*>(m)->mCurMorf < 1.0f; }

/* setNpcAction(PMF, &arg) */
static inline void md_setNpcAction(daNpc_Md_c* i_this, const ActionWords& pmf, void* arg) {
    gabi::Local<ProcFunc_l> fn;
    mdMain_copyAction(fn, pmf);
    i_this->setNpcAction(fn, arg);
}

/* daPy_npc_c::setRestart(s8)  */
static inline void md_setRestart(daNpc_Md_c* i_this, s8 opt) { mdMain_setRestart(i_this, opt); }
/* cLib_calcTimer<u8> (out of line)  */
static inline u8 md_calcTimerU8(be<u8>* t) { return mdMain_calcTimerU8(t); }
/* cLib_getRndValue<int>(base, range) (another TU's copy)  */
static inline s32 md_getRndValue(s32 base, s32 range) { return mdMain_getRndValue(base, range); }

/*  HD: when the player is at a door (eventInfo command 3), Medli switches to the
 * HD-only action  instead of only refusing to follow */

/* daNpc_Md_c::getMaxFlyingTimer() (static, out of line in another TU)  */
static inline s16 md_getMaxFlyingTimer() { return mdMain_getMaxFlyingTimer(); }
/* /  CPad_CHECK_TRIG_A / _B(port) (HD: out of line)  */
static inline BOOL md_trigA(s32 port) { return mdMain_trigA(port); }
static inline BOOL md_trigB(s32 port) { return mdMain_trigB(port); }
/* dComIfGs_isEventBit / onEventBit: dSv_event_c at *() + 0x644 */
static inline dSv_event_c* md_event() { return reinterpret_cast<dSv_event_c*>(&saveInfo.mpInfo->mEvent); }

static daNpc_Md_HIO6_c* daNpc_Md_HIO6_c_ct(daNpc_Md_HIO6_c* i_this);
static daNpc_Md_HIO5_c* daNpc_Md_HIO5_c_ct(daNpc_Md_HIO5_c* i_this);
static daNpc_Md_HIO4_c* daNpc_Md_HIO4_c_ct(daNpc_Md_HIO4_c* i_this);
static daNpc_Md_HIO3_c* daNpc_Md_HIO3_c_ct(daNpc_Md_HIO3_c* i_this);
static daNpc_Md_HIO2_c* daNpc_Md_HIO2_c_ct(daNpc_Md_HIO2_c* i_this);
static daNpc_Md_HIO_c* daNpc_Md_HIO_c_ct(daNpc_Md_HIO_c* i_this);

/* 02294CD0 */
static daNpc_Md_HIO6_c* daNpc_Md_HIO6_c_ct(daNpc_Md_HIO6_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Md_HIO6_c*)operator_new(0x18);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = HIO6_VTABLE;
    i_this->m04 = 500.0f;
    i_this->m08 = 430;
    i_this->m0A = 9000;
    i_this->m0C = -5000;
    i_this->m0E = -9000;
    i_this->m10 = 0;
    i_this->m12 = 0;
    i_this->m14 = 0;
    i_this->m16 = 0;
    return i_this;
}

/* 02294D50 */
static daNpc_Md_HIO5_c* daNpc_Md_HIO5_c_ct(daNpc_Md_HIO5_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Md_HIO5_c*)operator_new(0xC);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = HIO5_VTABLE;
    i_this->m4 = 16.0f;
    i_this->m8 = 0.6f;
    return i_this;
}

/* 02294DA8 */
static daNpc_Md_HIO4_c* daNpc_Md_HIO4_c_ct(daNpc_Md_HIO4_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Md_HIO4_c*)operator_new(0xC);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = HIO4_VTABLE;
    i_this->m4 = 86.0f;
    i_this->m8 = 0;
    return i_this;
}

/* 02294DFC */
static daNpc_Md_HIO3_c* daNpc_Md_HIO3_c_ct(daNpc_Md_HIO3_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Md_HIO3_c*)operator_new(0x28);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = HIO3_VTABLE;
    i_this->m04 = 0.8f;
    i_this->m08 = 0.2f;
    i_this->m0C = -5.0f;
    i_this->m10 = -5.0f;
    i_this->m14 = 0.0f;
    i_this->m18 = 0;
    i_this->m1A = 0;
    i_this->m1C = 0;
    i_this->m1E = 3800;
    i_this->m20 = 5800;
    i_this->m22 = 0x7FFF;
    i_this->m24 = -0x7FFF;
    return i_this;
}

/* 02294EA0 */
static daNpc_Md_HIO2_c* daNpc_Md_HIO2_c_ct(daNpc_Md_HIO2_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Md_HIO2_c*)operator_new(0x2C);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = HIO2_VTABLE;
    i_this->m26 = 0x19;
    i_this->m28 = 0x7FFF;
    i_this->m2A = 1;
    i_this->m04 = 5.0f;
    i_this->m08 = 17.0f;
    i_this->m0C = 7.0f;
    i_this->m10 = 6.0f;
    i_this->m14 = 0.7f;
    i_this->m20 = 3300;
    i_this->m22 = 2900;
    i_this->m24 = 5;
    i_this->m18 = 2.6f;
    i_this->m1C = 0.6f;
    return i_this;
}

/* 02294F64 */
static daNpc_Md_HIO_c* daNpc_Md_HIO_c_ct(daNpc_Md_HIO_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Md_HIO_c*)operator_new(0x1CC);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = HIO_VTABLE;
    daNpc_Md_HIO2_c_ct(&i_this->m008);
    daNpc_Md_HIO3_c_ct(&i_this->m034);
    daNpc_Md_HIO4_c_ct(&i_this->m05C);
    daNpc_Md_HIO5_c_ct(&i_this->m068);
    daNpc_Md_HIO6_c_ct(&i_this->m074);
    dNpc_HIO_c_ct(&i_this->mNpc);
    i_this->mNpc.m04 = -25.0f;
    i_this->mNpc.mMaxHeadX = 0x2328;
    i_this->mNpc.mMaxHeadY = 0x2328;
    i_this->mNpc.mMaxBackboneX = 0x0;
    i_this->mNpc.mMaxBackboneY = 0x1F40;
    i_this->mNpc.mMinHeadX = -0x2328;
    i_this->mNpc.mMinHeadY = -0x2328;
    i_this->mNpc.mMinBackboneX = 0x0;
    i_this->mNpc.mMinBackboneY = -0x1F40;
    i_this->mNpc.mMaxTurnStep = 0x1000;
    i_this->mNpc.mMaxHeadTurnVel = 0x800;
    i_this->mNpc.mAttnYOffset = 130.0f;
    i_this->mNpc.mMaxAttnAngleY = 0x4000;
    i_this->mNpc.m22 = 0x0;
    i_this->mNpc.mMaxAttnDistXZ = 150.0f;
    i_this->mpActor = nullptr;
    i_this->m0B8 = 770.0f;
    i_this->m0BC = 250.0f;
    i_this->m0C0 = -450.0f;
    i_this->m0C4 = 90.0f;
    i_this->m0C8 = 400.0f;
    i_this->m0CC = 600.0f;
    i_this->m0D0 = 0.05f;
    i_this->m0D4 = 17.0f;
    i_this->m0D8 = 0.0f;
    i_this->m0DC = 0.5f;
    i_this->m0E0 = 0.3f;
    i_this->m0E4 = 0.9f;
    i_this->m0E8 = 15.0f;
    i_this->m0EC = 22.0f;
    i_this->m0F0 = 9.0f;
    i_this->m0F4 = 4.0f;
    i_this->m0F8 = -1.51367f;
    i_this->m0FC = -1.09863f;
    i_this->m100 = -0.56152f;
    i_this->m104 = 0.1f;
    i_this->m108 = 0.2f;
    i_this->m10C = 10.0f;
    i_this->m110 = -50.0f;
    i_this->m114 = 1.3f;
    i_this->m128 = 1200.0f;
    i_this->m12C = 1500.0f;
    i_this->m118 = 1.0f;
    i_this->m11C = 1.5f;
    i_this->m120 = 2.0f;
    i_this->m124 = 1.6f;
    i_this->m130 = 3.0f;
    i_this->m134 = 1.25f;
    i_this->m138 = 0.2f;
    i_this->m13C = 100.0f;
    i_this->m140 = 50.0f;
    i_this->m144 = 9.0f;
    i_this->m1B4 = 0x17;
    i_this->m1B6 = 0x14;
    i_this->m1C7 = 0x0;
    i_this->m148 = 0.9f;
    i_this->m1C6 = 0x1;
    i_this->m14C = 6.0f;
    i_this->m1B8 = 0x4000;
    i_this->m150 = 10.0f;
    i_this->m1BA = 0x78;
    i_this->m1BC = 0x1F4;
    i_this->m154 = 5.0f;
    i_this->m158 = 2.2f;
    i_this->m15C = -11.681f;
    i_this->m160 = 1.3f;
    i_this->m164 = 88.692f;
    i_this->m168 = 57.066f;
    i_this->m16C = 179.286f;
    i_this->m1C8 = 0x0;
    i_this->m170 = 0.0f;
    i_this->m174 = 1.0f;
    i_this->m178 = 1.5f;
    i_this->m17C = -100.0f;
    i_this->m180 = 100.0f;
    i_this->m184 = 10.0f;
    i_this->m188 = 1.0f;
    i_this->m18C = -10.0f;
    i_this->m1BE = 0x46;
    i_this->m1C0 = 0x5;
    i_this->m1C2 = 0x1C2;
    i_this->m1C4 = 0x96;
    i_this->m190 = 43.0f;
    i_this->m194 = 14.65f;
    i_this->m198 = 13.99f;
    i_this->m19C = 0.0f;
    i_this->m1A0 = -98.0f;
    i_this->m1A4 = 0.0f;
    i_this->m1A8 = 3.0f;
    i_this->m1AC = 1000.0f;
    i_this->m1B0 = 1000.0f;
    i_this->mNo = -1;
    return i_this;
}

/* 022854CC */
void daNpc_Md_c::setNpcAction(ProcFunc_l* actionFunc, void* arg) {
    m_flying = 0; /* offFlying() */
    mCurrPlayerActionFunc.f = 0;
    mCurrPlayerActionFunc.d = 0;
    mCurrPlayerActionFunc.i = 0;
    gabi::Local<ProcFunc_l> fn; /* by-value copy, two words */
    mdMain_copyAction(fn, *reinterpret_cast<ActionWords*>(actionFunc));
    setAction(&mCurrNpcActionFunc, fn, arg);
}

/* 02291F50 */
BOOL daNpc_Md_c::piyo2NpcAction(void* arg) {
    if (mActionStatus == ACTION_STARTING) {
        md_attnFlags(this) |= TALK_ATTENTION; /* fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e */
        maxFallSpeed = -100.0f;
        gravity = l_HIO().m0F4;
        setAnm(0x14);
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        shape_angle.x = 0;
        shape_angle.z = 0;
        speedF = 0.0f;
        m30F8 = 120.0f;
        m3144 = l_HIO().m1BA;
        if (mType == 4 || mType == 5) { /* isTypeEdaichi() || isTypeM_Dai() */
            m3144 = (s16)(m3144 >> 1);
        }
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        fopAcM_seStart(this, JA_SE_CM_MD_PIYO /* JA_SE_CM_MD_PIYO */, 0);
        s16 angle = shape_angle.y + mJntCtrl.mAngles[0][1] + mJntCtrl.mAngles[1][1];
        if (md_calcTimer(&m3144) == 0) {
            MD_SET_NPC_ACTION(action_waitNpc);
        }
        gabi::Local<be<s32>> attentionResult;
        u8 att = md_chkAttention(this, &current.pos, angle, 0);
        m312C = att;
        *attentionResult = att;
        if (att) {
            if ((u8)mType <= 3) { /* Atorizk, Adanmae, M_Dra09, Sea */
                md_attnFlags(this) |= TALK_ATTENTION;
            } else {
                md_attnFlags(this) &= ~TALK_ATTENTION;
            }
            if (mType == 1) { /* isTypeAdanmae() */
                mCurEventMode = 2;
            }
        }
        NpcCall(attentionResult);
        lookBack(1, 1, 0);
        current.angle.y = shape_angle.y;
        setAttention(true);
    }
    return TRUE;
}

/* 0229216C */
BOOL daNpc_Md_c::deleteNpcAction(void* arg) {
    if (mActionStatus == ACTION_STARTING) {
        mActionStatus = ACTION_ONGOING_1; /* ACTION_ONGOING_1 */
    } else if (mActionStatus != ACTION_ENDING) {
        fopAcM_delete(this);
    }
    return TRUE;
}

/* 022921B4 */
BOOL daNpc_Md_c::demoFlyNpcAction(void* arg) {
    if (mActionStatus == ACTION_STARTING) {
        md_attnFlags(this) = 0;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        current.angle.y = 0x5700;
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus == ACTION_ENDING) {
        emitterDelete(m0508);
    } else {
        md_attnFlags(this) = 0;
        if (mActionStatus == ACTION_ONGOING_1) {
            if (mJntCtrl.mbTrn != 0) { /* trnChk() */
                cLib_addCalcAngleS2(&m3110, l_HIO().mNpc.mMaxHeadTurnVel, 4, 0x800);
            } else {
                m3110 = 0;
                setAnm(0x18);
                mActionStatus = mActionStatus + 1;
            }
            cXyz lookTarget;
            cXyz eye;
            lookTarget.z = 150.0f;
            lookTarget.x = -1511.0f;
            lookTarget.y = 14500.0f;
            f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
            eye.x = x;
            eye.y = y;
            eye.z = z;
            md_lookAtTarget(&mJntCtrl, &shape_angle.y, &lookTarget, &eye, current.angle.y, m3110, false);
        } else if (mActionStatus == ACTION_ONGOING_2) {
            if (m312A != 0) {
                setAnm(0x19);
                setBitStatus(daMdStts_UNK1);
                setWingEmitter();
                speed.y = 0.0f;
                gravity = 1.0f;
                maxFallSpeed = 100.0f;
                mActionStatus = mActionStatus + 1;
            }
        } else if (mActionStatus == ACTION_ONGOING_3) {
            if (m312A != 0) {
                setAnm(10);
            }
            if (current.pos.y > 14500.0f) {
                setAnm(0xb);
                maxFallSpeed = 0.0f;
                mActionStatus = mActionStatus + 1;
                speedF = 17.0f;
                speed.y = 0.0f;
                gravity = 0.0f;
            }
        } else if (mActionStatus == 4 && (actor_condition & 4)) {
            fopAcM_delete(this);
        }
        setAttention(true);
    }
    return TRUE;
}

/* 022926D0 */
void daNpc_Md_c::routeAngCheck(cXyz* normal, be<s16>* outAngle) {
    cXyz temp;
    cXyz_outprod_md(&m32A4, &temp, normal);
    s16 angle = cM_atan2s(temp.x, temp.z);
    if ((!(m32A4.y < 1.0f) && cLib_distanceAngleS(angle, *outAngle) > 0x4000) ||
        temp.y * fopAcM_searchPlayerDistanceY(this) < 0.0f) {
        angle += 0x8000;
    }
    *outAngle = angle;
}

/* 0229278C */
void daNpc_Md_c::routeWallCheck(cXyz* start, cXyz* end, be<s16>* angle) {
    dBgS_LinChk lin_chk;
    dBgS_LinChk_ct(&lin_chk, MD_LINCHK_VT, false);
    dBgS_LinChk_Set(&lin_chk, start, end, nullptr);
    if (cBgS_LineCross(dComIfG_Bgsp(), &lin_chk)) {
        cM3dGPla* pla = reinterpret_cast<cM3dGPla*>(dBgS_GetTriPla(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&lin_chk)));
        if (pla != nullptr) {
            routeAngCheck(&pla->mNormal, angle);
        }
    }
    md_LinChk_dt(&lin_chk);
}

/* 0229249C */
f32 daNpc_Md_c::checkForwardGroundY(s16 angle) {
    if (mAcchCir[0].ChkWallHit()) {
        cM3dGPla* pla = reinterpret_cast<cM3dGPla*>(dBgS_GetTriPla(dComIfG_Bgsp(), &mAcchCir[0]));
        if (pla != nullptr && cLib_distanceAngleS(angle, cM_atan2s(pla->mNormal.x, pla->mNormal.z)) > 0x4000) {
            dBgS_GndChk gnd_chk;
            dBgS_GndChk_ct(&gnd_chk, MD_GNDCHK_VT, false);
            gnd_chk.mFlag = gnd_chk.mFlag & ~2u; /* OffWall() */
            f32 x = gabi::fmadds(80.0f, cM_ssin(angle), current.pos.x);
            f32 y = current.pos.y + 80.0f;
            f32 z = gabi::fmadds(80.0f, cM_scos(angle), current.pos.z);
            cXyz* pos = &gnd_chk.m_pos; /* SetPos */
            pos->x = x;
            pos->y = y;
            pos->z = z;
            f32 ret = cBgS_GroundCross(dComIfG_Bgsp(), &gnd_chk);
            md_GndChk_dt(&gnd_chk);
            return ret;
        }
    }
    return -10000000.0f;
}

/* 02292650 */
f32 daNpc_Md_c::checkWallJump(s16 angle) {
    f32 temp = checkForwardGroundY(angle) - current.pos.y;
    if (0.0f < temp && temp < 80.0f) {
        return std_sqrtf(temp) * 3.6f;
    }
    return -1.0f;
}

/* 022928A8 */
BOOL daNpc_Md_c::routeCheck(f32 distanceSquared, be<s16>* angle) {
    gabi::Local<be<f32>> jumpSpeed;
    if (!mAcch.ChkGroundHit()) {
        cXyz temp;
        f32 x = current.pos.x, z = current.pos.z;
        temp.x = x;
        temp.z = z;
        current.pos.z = old.pos.z; /* struct copy (integer words) */
        copyFloatWord(&current.pos.x, &old.pos.x);
        f32 y = current.pos.y;
        speedF = 0.0f;
        temp.y = y;
        m3131 = 1;
        copyFloatWord(&current.pos.y, &old.pos.y);
        dBgS_LinChk lin_chk;
        dBgS_LinChk_ct(&lin_chk, MD_LINCHK_VT, false);
        dBgS_LinChk_Set(&lin_chk, &temp, &current.pos, nullptr);
        if (cBgS_LineCross(dComIfG_Bgsp(), &lin_chk)) {
            cM3dGPla* pla = reinterpret_cast<cM3dGPla*>(dBgS_GetTriPla(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&lin_chk)));
            if (pla != nullptr) {
                s16 nrm = cM_atan2s(pla->mNormal.x, pla->mNormal.z);
                if (cLib_distanceAngleS(*angle, nrm) > 0x4000) {
                    md_LinChk_dt(&lin_chk);
                    return TRUE;
                }
            }
        }
        if (mAcch.GetGroundH() - temp.y < -100.0f) {
            md_LinChk_dt(&lin_chk);
            return FALSE;
        }
        *jumpSpeed = 20.0f;
        md_setNpcAction(this, action_jumpNpc, jumpSpeed);
        md_LinChk_dt(&lin_chk);
    } else {
        if (mAcch.ChkWallHit()) {
            f32 t = checkWallJump(*angle);
            *jumpSpeed = t;
            if (!(t < 0.0f)) {
                md_setNpcAction(this, action_jumpNpc, jumpSpeed);
                return TRUE;
            }
            if (distanceSquared > 360000.0f /* SQUARE(600.0f) */) {
                return FALSE;
            }
        }
        cXyz temp;
        cXyz tempB;
        s16 a = *angle;
        f32 y = current.pos.y + 80.0f;
        temp.x = current.pos.x;
        tempB.x = gabi::fmadds(80.0f, cM_ssin(a), current.pos.x);
        tempB.y = y;
        temp.y = y;
        temp.z = current.pos.z;
        tempB.z = gabi::fmadds(80.0f, cM_scos(a), current.pos.z);
        routeWallCheck(&temp, &tempB, angle);
    }
    return TRUE;
}

/* 02292C48 */
BOOL daNpc_Md_c::searchNpcAction(void* arg) {
    if (mActionStatus == ACTION_STARTING) {
        md_attnFlags(this) |= fopAc_Attn_ACTION_CARRY_e; /* fopAc_Attn_ACTION_CARRY_e */
        setBitStatus(daMdStts_UNK4);
        mAcchCir[1].SetWall(60.0f, 20.0f);
        if (m3104 == 0x11 || m3104 == 0x16 || m3104 == 0x17) {
            setAnm(0x13);
            mActionStatus = mActionStatus + 1;
        } else if (m3104 == 0x20) {
            setAnm(0x21);
            m312A = 0;
            mActionStatus = mActionStatus + 1;
        } else {
            setAnm(2);
            mActionStatus = mActionStatus + 2;
        }
        if (!mAcch.ChkGroundHit()) {
            f32 gndY = mAcch.GetGroundH();
            f32 delta = gndY - current.pos.y;
            if (delta < 0.0f && !(delta < -30.1f)) {
                speed.y = 0.0f;
                current.pos.y = gndY;
                mAcch.m_flags = mAcch.m_flags | 0x20; /* SetGroundHit() */
            }
        }
    } else if (mActionStatus != ACTION_ENDING) {
        if (mActionStatus == ACTION_ONGOING_1) {
            if (m312A != 0) {
                setAnm(2);
                mActionStatus = mActionStatus + 1;
            }
        } else {
            m312C = 1;
            fopAc_ac_c* player = dComIfGp_getPlayer(0);
            if (player->eventInfo.mCommand == 3) { /* eventInfo.checkCommandDoor() */
                MD_SET_NPC_ACTION(action_returnToPlayerNpc);
                return TRUE;
            }
            f32 dist_sq = fopAcM_searchPlayerDistanceXZ2(this);
            f32 temp;
            f32 sq = l_HIO().m0C4 * l_HIO().m0C4;
            if (dist_sq < sq) {
                temp = 0.0f;
            } else {
                f32 fVar2 = dist_sq - sq;
                if (fVar2 > 90000.0f) {
                    fVar2 = 90000.0f;
                }
                temp = fVar2 / 90000.0f;
                if (temp < 0.5f) {
                    temp = 0.5f;
                }
            }
            gabi::Local<be<s16>> adjustedAngle;
            s16 angle = fopAcM_searchPlayerAngleY(this);
            *adjustedAngle = angle;
            BOOL temp3 = FALSE;
            if (routeCheck(dist_sq, adjustedAngle) && cLib_distanceAngleS(angle, *adjustedAngle) <= 0x2000) {
                temp3 = TRUE;
            }
            if (!temp3 || (dComIfGp_getPlayerStatus0(0) & 0x02000101) /* dComIfGp_checkPlayerStatus0 */ ||
                (reinterpret_cast<daPy_py_l*>(player)->mResetFlg0 & 0x10000) /* player->checkAttentionLock() */) {
                mNoResetFlg1 = mNoResetFlg1 & ~2u; /* offNpcCallCommand() */
                temp = 0.0f;
                s32 reverb = dComIfGp_getReverb(current.roomNo);
                mDoAud_monsSeStart_md(JA_SE_CV_MD_LEFT_ALONE /* JA_SE_CV_MD_LEFT_ALONE */, &current.pos, md_procId(this), reverb);
                if (!temp3) {
                    MD_SET_NPC_ACTION(action_kyohiNpc);
                    return TRUE;
                }
            } else {
                md_setRestart(this, 2);
                if (md_calcTimerU8(&m3130) == 0) {
                    m312F = m312F ^ 1;
                    m3130 = (u8)md_getRndValue(8, 20);
                }
            }
            walkProc(temp, *adjustedAngle);
            cLib_addCalcAngleS(&shape_angle.y, current.angle.y, l_HIO().m008.m24, (s16)(l_HIO().m008.m20 * 2), (s16)(l_HIO().m008.m22 * 2));
            gabi::Local<be<s16>> turnAngle;
            *turnAngle = shape_angle.y;
            cXyz vec2;
            dNpc_playerEyePos(&vec2, l_HIO().mNpc.m04);
            cXyz vec;
            f32 ey = eyePos.y, z = current.pos.z;
            vec.y = ey;
            vec.z = z;
            s16 ang = shape_angle.y;
            vec.x = current.pos.x;
            md_lookAtTarget(&mJntCtrl, turnAngle, &vec2, &vec, ang, l_HIO().mNpc.mMaxTurnStep, false);
            if (speedF < 0.001f) {
                MD_SET_NPC_ACTION(action_waitNpc);
            } else {
                shape_angle.y = *turnAngle;
            }
            lookBack(1, 0, 0);
            setAttention(true);
        }
    }
    return TRUE;
}

/* 0229310C */
BOOL MdMain::Actor::returnToPlayerNpcAction(void* arg) {
    if (mActionStatus == ACTION_STARTING) {
        speedF = 0.0f;
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        s8 stayNo = dComIfGp_roomControl_getStayNo(); /* dStage_roomControl_c::mStayNo */
        current.roomNo = stayNo;
        if (!dComIfGp_event_runCheck()) { /* !dComIfGp_event_runCheck() */
            /* function-local static cXyz, initialised on first use */
            if (playerOffsetGuard == 0) {
                playerOffsetGuard = 1;
                cXyz* o = &playerOffset;
                o->x = -100.0f;
                o->y = 0.0f;
                o->z = -150.0f;
            }
            fopAc_ac_c* player = dComIfGp_getPlayer(0);
            s16 angle = player->shape_angle.y;
            shape_angle.y = angle;
            md_cLib_offsetPos(&current.pos, &player->current.pos, angle, &playerOffset);
            old.pos.copy(current.pos);
            home.pos.copy(current.pos);
            home.roomNo = stayNo;
            MD_SET_NPC_ACTION(action_waitNpc);
        }
    }
    return TRUE;
}

/* 0229325C */
BOOL daNpc_Md_c::hitNpcAction(void* hitAngle) {
    if (mActionStatus == ACTION_STARTING) {
        mDamageFogTimer = 5 * 30;
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_monsSeStart_md(JA_SE_CV_MD_CRASH /* JA_SE_CV_MD_CRASH */, &current.pos, md_procId(this), reverb);
        s16 angle = 0;
        if (hitAngle != nullptr) {
            angle = *(be<s16>*)hitAngle;
        }
        current.angle.y = angle;
        speedF = 10.0f;
        speed.y = 20.0f;
        md_attnFlags(this) &= ~fopAc_Attn_ACTION_CARRY_e; /* fopAc_Attn_ACTION_CARRY_e */
        mAcchCir[1].SetWall(60.0f, 20.0f);
        clearStatus(daMdStts_UNK1 | daMdStts_UNK4);
        setBitStatus(daMdStts_UNK2);
        m30F8 = 120.0f;
        setAnm(0xD);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (mAcch.ChkGroundHit()) {
            speedF = 0.0f;
            MD_SET_NPC_ACTION(action_land03Npc /* land03NpcAction */);
        }
        setAttention(true);
    }
    return TRUE;
}

/* 022933A0 */
BOOL daNpc_Md_c::jumpNpcAction(void* jumpSpeed) {
    if (mActionStatus == ACTION_STARTING) {
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        setBitStatus(daMdStts_UNK4);
        if (jumpSpeed != nullptr) {
            speed.y = *(be<f32>*)jumpSpeed;
        }
        shape_angle.x = 0;
        shape_angle.z = 0;
        speedF = 4.0f;
        gravity = l_HIO().m0F4;
        maxFallSpeed = -100.0f;
        setAnm(0xc);
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (mActionStatus == ACTION_ONGOING_1) {
            if (mAcch.ChkGroundHit()) {
                speedF = 0.0f;
                setAnm(0xe);
                m312A = 0;
                mActionStatus = mActionStatus + 1;
            }
        } else if (m312A != 0) {
            speedF = 0.0f;
            MD_SET_NPC_ACTION(action_waitNpc);
        }
        setAttention(true);
    }
    return TRUE;
}

/* 02293500 */
BOOL daNpc_Md_c::escapeNpcAction(void* arg) {
    if (mActionStatus == ACTION_STARTING) {
        setBitStatus(daMdStts_UNK4);
        md_attnFlags(this) |= fopAc_Attn_ACTION_CARRY_e; /* fopAc_Attn_ACTION_CARRY_e */
        mAcchCir[1].SetWall(60.0f, 20.0f);
        if (m3104 == 0x11 || m3104 == 0x16 || m3104 == 0x17) {
            setAnm(0x13);
            mActionStatus = mActionStatus + 1;
        } else if (m3104 == 0x20) {
            setAnm(0x21);
            m312A = 0;
            mActionStatus = mActionStatus + 1;
        } else {
            setAnm(2);
            mActionStatus = mActionStatus + 2;
        }
        if (!mAcch.ChkGroundHit()) {
            f32 gndY = mAcch.GetGroundH();
            f32 delta = gndY - current.pos.y;
            if (delta < 0.0f && !(delta < -30.1f)) {
                speed.y = 0.0f;
                current.pos.y = gndY;
                mAcch.m_flags = mAcch.m_flags | 0x20; /* SetGroundHit() */
            }
        }
        m3144 = 0;
        m3146 = 0x1e;
    } else if (mActionStatus != ACTION_ENDING) {
        if (mActionStatus == ACTION_ONGOING_1) {
            if (m312A != 0) {
                setAnm(2);
                mActionStatus = mActionStatus + 1;
            }
        } else {
            m312C = 1;
            md_calcTimer(&m3146);
            gabi::Local<be<s16>> adjustedShapeAngle;
            s16 sVar5;
            if (m313F != 0) {
                s16 ofs = (m313F < 0) ? (s16)-0x4000 : (s16)0x4000;
                sVar5 = (s16)(cM_atan2s(m30C4.x, m30C4.z) + ofs);
                *adjustedShapeAngle = sVar5;
                m3144 = sVar5;
            } else if (m3146 == 0) {
                MD_SET_NPC_ACTION(action_waitNpc);
                return TRUE;
            } else {
                sVar5 = m3144;
                *adjustedShapeAngle = sVar5;
            }
            if (routeCheck(250000.0f, adjustedShapeAngle)) {
                cLib_distanceAngleS(sVar5, *adjustedShapeAngle);
            }
            walkProc(1.0f, *adjustedShapeAngle);
            cLib_addCalcAngleS(&shape_angle.y, current.angle.y, l_HIO().m008.m24, (s16)(l_HIO().m008.m20 * 2), (s16)(l_HIO().m008.m22 * 2));
            s16 turnAngle = shape_angle.y;
            if (speedF < 0.001f) {
                MD_SET_NPC_ACTION(action_waitNpc);
            } else {
                shape_angle.y = turnAngle;
            }
            lookBack(0, 0, 0);
            setAttention(true);
        }
    }
    return TRUE;
}

/* 02293840 */
BOOL daNpc_Md_c::waitPlayerAction(void* arg) {
    if (mActionStatus == ACTION_STARTING) {
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_FLY);
        setBitStatus(daMdStts_UNK4);
        setAnm(0);
        speedF = 0.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        m311A = 0;
        m310C = 0.0f;
        m3114 = 0;
        m3116 = 0;
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING && !flyCheck()) {
        Attention* attention = reinterpret_cast<Attention*>(dComIfGp_getAttention()); /* dComIfGp_getAttention() */
        if (!(md_stickValue(0) < l_HIO().m104) || md_attention_Lockon(attention)) {
            s16 stickAngle = getStickAngY(0);
            cLib_addCalcAngleS(&current.angle.y, stickAngle, l_HIO().m008.m26, l_HIO().m008.m28, l_HIO().m008.m2A);
            cXyz stickPos;
            int temp = calcStickPos(stickAngle, &stickPos);
            if (temp == 0 || !(md_stickValue(0) < l_HIO().m108)) {
                shape_angle.y = current.angle.y;
            }
            {
                gabi::Local<be<s16>> tempAngle;
                cXyz eye;
                f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
                s16 ang = shape_angle.y;
                eye.x = x;
                eye.y = y;
                eye.z = z;
                *tempAngle = ang;
                md_lookAtTarget(&mJntCtrl, tempAngle, &stickPos, &eye, ang, l_HIO().mNpc.mMaxTurnStep, false);
                if (temp > 0) {
                    shape_angle.y = *tempAngle;
                }
            }
            current.angle.y = shape_angle.y;
            if (!(md_stickValue(0) < l_HIO().m108)) {
                if (temp == 0) {
                    current.angle.y = stickAngle;
                    MD_SET_PLAYER_ACTION(action_walkPlayer);
                }
            }
        } else {
            cXyz zero;
            cXyz* z0 = &cXyz::Zero; /* cXyz::Zero */
            f32 y = z0->y, x = z0->x, z = z0->z;
            zero.y = y;
            zero.z = z;
            zero.x = x;
            md_lookAtTarget(&mJntCtrl, &shape_angle.y, nullptr, &zero, shape_angle.y, 0, false);
            waitGroundCheck();
            current.angle.y = shape_angle.y;
        }
        setAttention(md_isMorf(mpMorf.get()));
    }
    return TRUE;
}

/* 02293B5C */
BOOL daNpc_Md_c::walkPlayerAction(void* arg) {
    if (mActionStatus == ACTION_STARTING) {
        setBitStatus(daMdStts_UNK4);
        setAnm(2);
        mMaxNormalSpeed = l_HIO().m008.m08;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING && !flyCheck()) {
        cXyz targetDirection;
        gabi::Local<be<f32>> targetDistance;
        f32 dVar4 = md_stickValue(0);
        s16 sVar3 = getStickAngY(0);
        walkProc(dVar4, sVar3);
        int iVar1 = calcStickPos(sVar3, &targetDirection);
        if (iVar1 == 0) {
            cLib_addCalcAngleS(&shape_angle.y, current.angle.y, l_HIO().m008.m24, (s16)(l_HIO().m008.m20 * 2), (s16)(l_HIO().m008.m22 * 2));
        }
        {
            gabi::Local<be<s16>> tempAngle;
            cXyz eye;
            f32 z = current.pos.z, y = current.pos.y, x = current.pos.x;
            eye.z = z;
            eye.x = x;
            s16 ang = shape_angle.y;
            eye.y = y;
            *tempAngle = ang;
            md_lookAtTarget(&mJntCtrl, tempAngle, &targetDirection, &eye, ang, l_HIO().mNpc.mMaxTurnStep, false);
            if (iVar1 > 0) {
                shape_angle.y = *tempAngle;
            }
        }
        if (speedF < 0.001f || iVar1 != 0) {
            MD_SET_PLAYER_ACTION(action_waitPlayer);
        } else if (!mAcch.ChkGroundHit()) {
            *targetDistance = 10.0f;
            md_setPlayerAction(this, action_jumpPlayer, targetDistance);
        } else if (mAcch.ChkWallHit()) {
            f32 v = checkWallJump(current.angle.y);
            *targetDistance = v;
            if (!(v < 0.0f)) {
                md_setPlayerAction(this, action_jumpPlayer, targetDistance);
                return TRUE;
            }
        }
        setAttention(true);
    }
    return TRUE;
}

/* 02293DDC */
BOOL daNpc_Md_c::hitPlayerAction(void* hitAngle) {
    if (mActionStatus == ACTION_STARTING) {
        {
            cXyz dir;
            dir.set(0.0f, 1.0f, 0.0f);
            dComIfGp_getVibration_StartShock(5, -0x21, &dir);
        }
        mDamageFogTimer = 5 * 30;
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_monsSeStart_md(JA_SE_CV_MD_CRASH /* JA_SE_CV_MD_CRASH */, &current.pos, md_procId(this), reverb);
        s16 sVar2 = 0;
        if (hitAngle != nullptr) {
            sVar2 = *(be<s16>*)hitAngle;
        }
        current.angle.y = sVar2;
        speedF = 10.0f;
        speed.y = 20.0f;
        maxFallSpeed = -100.0f;
        gravity = l_HIO().m0F4;
        md_attnFlags(this) &= ~fopAc_Attn_ACTION_CARRY_e; /* fopAc_Attn_ACTION_CARRY_e */
        mAcchCir[1].SetWall(60.0f, 20.0f);
        clearStatus(daMdStts_UNK1 | daMdStts_UNK4);
        setBitStatus(daMdStts_UNK2);
        m30F8 = 120.0f;
        setAnm(0xd);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (mActionStatus == ACTION_ONGOING_1) {
            if (mAcch.ChkGroundHit()) {
                speedF = 0.0f;
                setAnm(0xf);
                m312A = 0;
                mActionStatus = mActionStatus + 1;
            }
        } else if (m312A != 0) {
            m4E4 = m4E4 | 1; /* returnLink() */
        }
        setAttention(true);
    }
    return TRUE;
}

/* 02293FA8 */
BOOL daNpc_Md_c::jumpPlayerAction(void* param_1) {
    if (mActionStatus == ACTION_STARTING) {
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        setBitStatus(daMdStts_UNK4);
        if (param_1 != nullptr) {
            speed.y = *(be<f32>*)param_1;
        }
        shape_angle.x = 0;
        shape_angle.z = 0;
        gravity = l_HIO().m0F4;
        maxFallSpeed = -100.0f;
        setAnm(0xc);
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
        m3150 = current.pos.y;
    } else if (mActionStatus != ACTION_ENDING) {
        if (mActionStatus == ACTION_ONGOING_1) {
            if (mAcch.ChkGroundHit()) {
                if (!(m3150 - current.pos.y < l_HIO().m1B0)) { /* NaN: shakes */
                    cXyz dir;
                    dir.set(0.0f, 1.0f, 0.0f);
                    dComIfGp_getVibration_StartShock(6, -0x21, &dir);
                }
                speedF = 0.0f;
                setAnm(0xe);
                m312A = 0;
                mActionStatus = mActionStatus + 1;
            }
        } else if (m312A != 0) {
            MD_SET_PLAYER_ACTION(action_waitPlayer);
        }
        setAttention(true);
    }
    return TRUE;
}

/* 02294160 */
BOOL daNpc_Md_c::flyPlayerAction(void* arg) {
    be<s16>* flyingTimer = &m_flyingTimer;
    if (mActionStatus == ACTION_STARTING) {
        setAnm(0x18);
        m3150 = current.pos.y + 1500.0f;
        m3144 = l_HIO().m1BE;
        m3146 = l_HIO().m1C0;
        *flyingTimer = md_getMaxFlyingTimer();
        speedF = 0.0f;
        mAcchCir[1].SetWall(60.0f, 60.0f);
        m_flying = 1; /* onFlying() */
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus == ACTION_ENDING) {
        emitterDelete(m0508);
        deleteHane03Emitter();
        m_flying = 0; /* offFlying() */
    } else {
        if (md_trigA(0)) {
            m3146 = l_HIO().m1C0;
        } else {
            md_calcTimer(&m3146);
        }
        if (md_trigA(0)) {
            m3144 = l_HIO().m1BE;
        } else {
            md_calcTimer(&m3144);
        }
        if (mActionStatus == ACTION_ONGOING_1) {
            if (m312A != 0) {
                setBitStatus(daMdStts_UNK1);
                setWingEmitter();
                setAnm(0x19);
                speedF = 0.0f;
                speed.y = 20.0f;
                gravity = l_HIO().m188;
                maxFallSpeed = l_HIO().m18C;
                m3144 = l_HIO().m1BE;
                m3146 = l_HIO().m1C0;
                mActionStatus = mActionStatus + 1;
            }
        } else {
            md_calcTimer(flyingTimer); /* calcFlyingTimer() */
            if (dSv_event_isEventBit(md_event(), UNK_3320) && *flyingTimer < 300) {
                dSv_event_onEventBit(md_event(), UNK_3310);
            }
            if (*flyingTimer == 0 || m3144 == 0 || md_trigB(0)) {
                if (checkStatus(daMdStts_UNK1)) {
                    m3135 = m3135 | 1; /* setBitEffectStatus(1) */
                }
                MD_SET_PLAYER_ACTION(action_jumpPlayer);
                if (*flyingTimer == 0) {
                    s32 reverb = dComIfGp_getReverb(current.roomNo);
                    mDoAud_monsSeStart_md(JA_SE_CV_MD_FLY_END /* JA_SE_CV_MD_FLY_END */, &current.pos, md_procId(this), reverb);
                }
                return TRUE;
            }
            if (m3104 != 0x2e && m312A != 0) {
                setAnm(7);
            }
            if (*flyingTimer < l_HIO().m1C4 && m3104 == 7) {
                setAnm(0x2e);
                setHane03Emitter();
            }
            Attention* attention = reinterpret_cast<Attention*>(dComIfGp_getAttention()); /* dComIfGp_getAttention() */
            if (!(md_stickValue(0) < l_HIO().m104) || md_attention_Lockon(attention)) {
                f32 dVar8 = md_stickValue(0) * l_HIO().m10C;
                if (0.0f < dVar8 && dVar8 < l_HIO().m0D8) {
                    dVar8 = l_HIO().m0D8;
                }
                s16 sVar2 = getStickAngY(0);
                cLib_distanceAngleS(sVar2, current.angle.y);
                f32 dVar7 = l_HIO().m0DC;
                if (!(md_stickValue(0) < l_HIO().m104)) {
                    cLib_addCalcAngleS(&current.angle.y, sVar2, 8, 0x2000, 0x400);
                }
                cXyz lookTarget;
                int iVar3 = calcStickPos(sVar2, &lookTarget);
                if (iVar3 == 0) {
                    cLib_addCalcAngleS(&shape_angle.y, current.angle.y, 8, 0x2000, 0x400);
                }
                {
                    gabi::Local<be<s16>> tempAngle;
                    cXyz eye;
                    f32 z = current.pos.z, y = current.pos.y, x = current.pos.x;
                    eye.z = z;
                    eye.x = x;
                    s16 ang = shape_angle.y;
                    eye.y = y;
                    *tempAngle = ang;
                    md_lookAtTarget(&mJntCtrl, tempAngle, &lookTarget, &eye, ang, l_HIO().mNpc.mMaxTurnStep, false);
                    if (iVar3 > 0) {
                        shape_angle.y = *tempAngle;
                    } else if (md_stickValue(0) < l_HIO().m104) {
                        cLib_addCalcAngleS(&current.angle.y, shape_angle.y, 8, 0x2000, 0x400);
                    }
                }
                f32 maxF = l_HIO().m0D4;
                dVar8 = (dVar8 - maxF >= 0.0f) ? maxF : dVar8; /* fsel */
                cLib_chaseF(&speedF, dVar8, dVar7);
            } else {
                cXyz zero;
                cXyz* z0 = &cXyz::Zero; /* cXyz::Zero */
                f32 x = z0->x, y = z0->y, z = z0->z;
                zero.x = x;
                zero.y = y;
                zero.z = z;
                md_lookAtTarget(&mJntCtrl, &shape_angle.y, nullptr, &zero, shape_angle.y, 0, false);
                current.angle.y = shape_angle.y;
            }
            gravity = l_HIO().m188;
            if (m3146 != 0) {
                maxFallSpeed = 10.0f;
            } else {
                maxFallSpeed = l_HIO().m18C;
            }
            if (!(current.pos.y < m3150)) {
                current.pos.y = m3150;
            }
            if (mAcch.ChkGroundHit()) {
                *flyingTimer = 0;
                MD_SET_PLAYER_ACTION(action_landPlayer);
            }
        }
        if (!checkStatus(daMdStts_UNK2)) {
            if (checkStatus(daMdStts_UNK1)) {
                s16 sVar2 = shape_angle.y - current.angle.y;
                if (sVar2 < 0x6000 && sVar2 > -0x6000) {
                    if (sVar2 > 0x2000) {
                        sVar2 = 0x2000;
                    } else if (sVar2 < -0x2000) {
                        sVar2 = -0x2000;
                    }
                    cLib_addCalcAngleS(&shape_angle.z, sVar2, 8, 0x2000, 0x400);
                    cLib_addCalcAngleS(&shape_angle.x, 0, 8, 0x2000, 0x400);
                } else {
                    cLib_addCalcAngleS(&shape_angle.z, 0, 8, 0x2000, 0x400);
                    cLib_addCalcAngleS(&shape_angle.x, -0x2000, 8, 0x2000, 0x400);
                }
            } else {
                shape_angle.x = 0;
                current.angle.y = shape_angle.y;
                shape_angle.z = 0;
            }
        }
        setAttention(true);
    }
    return TRUE;
}

/* 02294838 */
BOOL daNpc_Md_c::landPlayerAction(void* arg) {
    if (mActionStatus == ACTION_STARTING) {
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_monsSeStart_md(JA_SE_CV_MD_LANDING /* JA_SE_CV_MD_LANDING */, &current.pos, md_procId(this), reverb);
        if (checkStatus(daMdStts_UNK1)) {
            m3135 = m3135 | 1; /* setBitEffectStatus(1) */
        }
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        setAnm(0xe);
        shape_angle.x = 0;
        shape_angle.z = 0;
        gravity = l_HIO().m0F4;
        maxFallSpeed = -100.0f;
        speedF = 0.0f;
        speed.y = 0.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (m312A != 0) {
            speedF = 0.0f;
            MD_SET_PLAYER_ACTION(action_waitPlayer);
        }
        setAttention(true);
    }
    return TRUE;
}

/* 02294984 */
BOOL daNpc_Md_c::mkamaePlayerAction(void* arg) {
    if (mActionStatus == ACTION_STARTING) {
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        setAnm(0x1F);
        shape_angle.x = 0;
        shape_angle.z = 0;
        gravity = l_HIO().m0F4;
        maxFallSpeed = -100.0f;
        speedF = 0.0f;
        speed.y = 0.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus == ACTION_ENDING) {
        m_mirror = 0; /* offMirror() */
    } else {
        if (mActionStatus == ACTION_ONGOING_1) {
            if (m312A != 0) {
                speedF = 0.0f;
                setAnm(0x20);
                m_mirror = 1; /* onMirror() */
                mActionStatus = mActionStatus + 1;
            }
        } else if (mActionStatus == ACTION_ONGOING_2) {
            if (mirrorCancelCheck() || !checkStatus(daMdStts_LIGHT_BODY_HIT)) {
                setAnm(0x21);
                m312A = 0;
                m_mirror = 0;
                mActionStatus = mActionStatus + 1;
            } else {
                Attention* attention = reinterpret_cast<Attention*>(dComIfGp_getAttention()); /* dComIfGp_getAttention() */
                if (!(md_stickValue(0) < l_HIO().m104) || md_attention_Lockon(attention)) {
                    m311A = getStickAngY(FALSE);
                    m310C = md_stickValue(0);
                } else {
                    m311A = 0;
                    m310C = 0.0f;
                }
                lookBackWaist(m311A, m310C);
            }
        } else if (m312A != 0) {
            MD_SET_PLAYER_ACTION(action_waitPlayer);
        }
        setAttention(true);
    }
    return TRUE;
}

/* 02294C00 */
BOOL daNpc_Md_c::carryPlayerAction(void* arg) {
    if (mActionStatus == ACTION_STARTING) {
        clearStatus(daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        setAnm(0x24);
        setBitStatus(daMdStts_UNK1);
        setHane02Emitter();
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_monsSeStart_md(JA_SE_CV_MD_LIFT_UP /* JA_SE_CV_MD_LIFT_UP */, &current.pos, md_procId(this), reverb);
        m30F8 = 75.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus == ACTION_ENDING) {
        deleteHane02Emitter();
    }
    return TRUE;
}

/* 02295434 */
static BOOL daNpc_Md_Delete(daNpc_Md_c* i_this) {
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.

/* 02295330 */
static void __sinit_d_a_npc_md_cpp() {
    sinit_header_statics(HEADER_STATICS, HEADER_STATICS_INIT);
    daNpc_Md_HIO_c_ct(&l_HIO());
    /* l_ms_light_local_start(5, 7, 0), l_ms_light_local_vec(0, 0, -10000) */
    cXyz* start = &l_ms_light_local_start;
    cXyz* vec = &l_ms_light_local_vec;
    start->x = 5.0f;
    l_fan_light_radius = 70.0f; /* HD: l_fan_light_cps_src radius set at run time */
    vec->x = 0.0f;
    vec->y = 0.0f;
    start->y = 7.0f;
    start->z = 0.0f;
    vec->z = -10000.0f;
}

/* 02295420 */
static void md_safeStringDelete(void* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 0229543C */
static void md_deletePlaneMember(void* i_this, s32 flags) {
    if (i_this != nullptr) {
        mdMain_destroyPlane(&reinterpret_cast<PlaneOwner*>(i_this)->mPlane, 2);
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}

/* 02295490 */
static void md_deleteModelMembers(void* i_this, s32 flags) {
    if (i_this != nullptr) {
        mdMain_destroyModelData(&reinterpret_cast<ModelOwner*>(i_this)->mModelData, 2);
        mdMain_destroyModel(&reinterpret_cast<ModelOwner*>(i_this)->mModel, 2);
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}

/* 022954F0 */
static void md_followCallbackExecuteAfter(daNpc_Md_c* i_this) {
}

/* 022954F4 */
static void md_followCallbackDraw(daNpc_Md_c* i_this) {
}

// HD: empty inherited callback virtual.
/* 022954F8 */
static void md_followCallbackNoop(daNpc_Md_c* i_this) {
}

/* 022954FC */
static s32 md_getTactMusic(daNpc_Md_c* i_this) {
    return -1;
}

/* 02295504 */
static s32 md_getTactTimerCancel(daNpc_Md_c* i_this) {
    return 0;
}

/* 0229550C */
static s32 md_checkPlayerGuard(daNpc_Md_c* i_this) {
    return 0;
}

/* 02295514 */
static s32 md_getGrabMissActor(daNpc_Md_c* i_this) {
    return 0;
}

/* 0229551C */
static s32 md_checkPlayerFly(daNpc_Md_c* i_this) {
    return 0;
}

/* 02295524 */
static s32 md_checkFrontRoll(daNpc_Md_c* i_this) {
    return 0;
}

/* 0229552C */
static s32 md_checkBottleSwing(daNpc_Md_c* i_this) {
    return 0;
}

/* 02295534 */
static s32 md_checkCutCharge(daNpc_Md_c* i_this) {
    return 0;
}

/* 0229553C */
static s32 md_getBokoFlamePos(daNpc_Md_c* i_this) {
    return 0;
}

/* 02295544 */
static s32 md_checkTactWait(daNpc_Md_c* i_this) {
    return 0;
}

/* 0229554C */
static void md_onDekuSpReturnFlg(daNpc_Md_c* i_this) {
}

/* 02295550 */
static s32 md_checkComboCutTurn(daNpc_Md_c* i_this) {
    return 0;
}

/* 02295558 */
static s32 md_getItemID(daNpc_Md_c* i_this) {
    return -1;
}

/* 02295560 */
static s32 md_getThrowBoomerangID(daNpc_Md_c* i_this) {
    return -1;
}

/* 02295568 */
static s32 md_getGrabActorID(daNpc_Md_c* i_this) {
    return -1;
}

/* 02295570 */
static s32 md_checkGrabBarrel(daNpc_Md_c* i_this) {
    return 0;
}

/* 02295578 */
static s32 md_checkPlayerNoDraw(daNpc_Md_c* i_this) {
    return 0;
}

/* 02295580 */
static s32 md_checkRopeTag(daNpc_Md_c* i_this) {
    return 0;
}

/* 02295588 */
static s32 md_checkRopeReadyAnime(daNpc_Md_c* i_this) {
    return 0;
}

/* 02295590 */
static void md_voiceStart(daNpc_Md_c* i_this) {
}

/* 02295594 */
static void md_setOutPower(daNpc_Md_c* i_this) {
}

/* 02295598 */
static void md_onFrollCrashFlg(daNpc_Md_c* i_this) {
}

/* 0229559C */
static s32 md_setHookshotCarryOffset(daNpc_Md_c* i_this) {
    return 0;
}

/* 022955A4 */
static void md_setPlayerPosAndAngleY(daNpc_Md_c* i_this) {
}

/* 022955A8 */
static void md_setPlayerPosAndAngles(daNpc_Md_c* i_this) {
}

/* 022955AC */
static void md_setPlayerPosAndMatrix(daNpc_Md_c* i_this) {
}

/* 022955B0 */
static s32 md_setThrowDamage(daNpc_Md_c* i_this) {
    return 0;
}

/* 022955B8 */
static f32 md_getGroundY(daNpc_Md_c* i_this) {
    return i_this->mAcch.GetGroundH();
}

/* 022955C0 */
static u32 md_getLeftHandMatrix(daNpc_Md_c* i_this) {
    return i_this->cullMtx;
}

/* 022955C8 */
static u32 md_getRightHandMatrix(daNpc_Md_c* i_this) {
    return i_this->cullMtx;
}

/* 022955D0 */
static f32 md_getBaseAnimeFrameRate(daNpc_Md_c* i_this) {
    return 1.0f;
}

/* 022955DC */
static f32 md_getBaseAnimeFrame(daNpc_Md_c* i_this) {
    return 0.0f;
}

/* 022955E8 */
static void md_safeStringAssureTermination(daNpc_Md_c* i_this) {
}

/* WWHD glue
ext 025E1A7C void mdMain_monsSeStart(u32 sound, cXyz* pos, u32 process, s32 reverb)
ext 0201B080 void mdMain_outprod(const cXyz* first, cXyz* result, const cXyz* second)
ext 0259DA18 void mdMain_constructNpcHio(dNpc_HIO_l* hio)
ext 02018034 void mdMain_destroyPlane(void* plane, s32 flags)
ext 027BF880 void mdMain_destroyModelData(void* data, s32 flags)
ext 027B5CBC void mdMain_destroyModel(void* model, s32 flags)
ext 02055B64 s16 mdMain_calcTimer(be<s16>* timer)
ext 0228DAF4 u8 mdMain_checkAttention(daNpc_Md_c* actor, cXyz* pos, s16 angle, int mode)
ext 0259DED0 void mdMain_lookAtTarget(dNpc_JntCtrl_c* control, be<s16>* outY, cXyz* target, cXyz* eye, s16 angle, s16 velocity, bool headOnly)
ext 02008DAC void mdMain_destroyGroundCheck(void* check, s32 flags)
ext 02008B4C void mdMain_destroyLineCheck(void* check, s32 flags)
ext 0200FA40 void mdMain_offsetPos(cXyz* dst, cXyz* base, s16 angle, cXyz* offset)
ext 020079B4 f32 mdMain_stickValue(s32 port)
ext 024EDFCC u8 mdMain_lockonTruth(Attention* attention)
ext 02445438 void mdMain_setRestart(daNpc_Md_c* actor, s8 option)
ext 0207A9A0 u8 mdMain_calcTimerU8(be<u8>* timer)
ext 021E1E78 s32 mdMain_getRndValue(s32 base, s32 range)
ext 02526C94 s16 mdMain_getMaxFlyingTimer()
ext 02007898 BOOL mdMain_trigA(s32 port)
ext 020078BC BOOL mdMain_trigB(s32 port)
data m_flying 101D5F3E
data m_mirror 101D5F3F
data m_flyingTimer 101D5F38
data playerOffsetGuard 10467C64
data playerOffset 10467A84
data l_ms_light_local_start 10467A60
data l_ms_light_local_vec 10467A54
data l_fan_light_radius 101C03EC
data saveInfo 101F84DC
data action_waitNpc 1001D490
data action_waitPlayer 1001D498
data action_jumpPlayer 1001D4B8
data action_land03Npc 1001D510
data action_walkPlayer 1001D558
data action_jumpNpc 1001D540
data action_returnToPlayerNpc 1001D548
data action_kyohiNpc 1001D550
data action_landPlayer 1001D560
*/
