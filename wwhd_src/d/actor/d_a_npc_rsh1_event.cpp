/**
 * d_a_npc_rsh1_event.cpp (WWHD): Zunari's event cuts, event/dummy actions, static init and destructors.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_rsh1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_rsh1.h"

#define rsh1_l_msgId (*gabi::at<be<u32>>(0x104684BC)) /* static fpc_ProcID l_msgId (HD: no l_msg) */

/* 022DAF30 */
bool daNpc_Rsh1_c::evn_talk_init(int i_staffIdx) {
    WWHD_FUNC(0x022DAF30, bool, this, i_staffIdx);
    be<u32>* msg_no_p = (be<u32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, STR(0x10021880) /* "MsgNo" */, 3);
    be<u32>* end_msg_no_p = (be<u32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, STR(0x10021888) /* "EndMsgNo" */, 3);
    rsh1_l_msgId = 0xFFFFFFFF;
    m778 = msg_no_p ? (u32)*msg_no_p : 0;
    m784 = end_msg_no_p ? (u32)*end_msg_no_p : 0;
    return true;
}
VERIFY(0x022DAF30, &daNpc_Rsh1_c::evn_talk_init);

/* 022DAFD8 */
bool daNpc_Rsh1_c::evn_continue_talk_init(int i_staffIdx) {
    WWHD_FUNC(0x022DAFD8, bool, this, i_staffIdx);
    be<u32>* end_msg_no_p = (be<u32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, STR(0x10021894) /* "EndMsgNo" */, 3);
    m784 = end_msg_no_p ? (u32)*end_msg_no_p : 0;
    return true;
}
VERIFY(0x022DAFD8, &daNpc_Rsh1_c::evn_continue_talk_init);

/* 022DB03C */
bool daNpc_Rsh1_c::evn_setAnm_init(int i_staffIdx) {
    WWHD_FUNC(0x022DB03C, bool, this, i_staffIdx);
    be<s32>* anm_no_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, STR(0x100218A0) /* "AnmNo" */, 3);
    if (anm_no_p) {
        setAnm((s8)(s32)*anm_no_p);
    }
    return true;
}
VERIFY(0x022DB03C, &daNpc_Rsh1_c::evn_setAnm_init);

/* 022DB0A8 */
bool daNpc_Rsh1_c::evn_turn_init(int i_staffIdx) {
    WWHD_FUNC(0x022DB0A8, bool, this, i_staffIdx);
    be<s32>* prm_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, STR(0x100218A8) /* "prm" */, 3);
    fopAc_ac_c* link_p = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B34));
    if (prm_p && *prm_p == 1) {
        current.angle.y = cLib_targetAngleY(&current.pos, &link_p->current.pos);
    }
    return true;
}
VERIFY(0x022DB0A8, &daNpc_Rsh1_c::evn_turn_init);

/* 022DB12C */
BOOL daNpc_Rsh1_c::evn_talk() {
    WWHD_FUNC(0x022DB12C, BOOL, this);
    u32 mng = gabi::load<u32>(0x101F4B5C); /* HD message manager */
    if (rsh1_l_msgId == 0xFFFFFFFF) {
        u32 id = gabi::call<u32>(0x025F7DB0, mng, (u32)m778, &eyePos); /* fopMsgM_messageSet */
        rsh1_l_msgId = id;
        if (id != 0xFFFFFFFF) {
            gabi::call(0x025DB58C); /* fopMsgM_demoMsgFlagOn */
        }
        return FALSE;
    }
    setAnmFromMsgTag();
    if (gabi::call<u16>(0x025F795C, mng) == 0xE /* MSG_DISPLAYED */) {
        u32 st = next_msgStatus(&m778, nullptr);
        gabi::call(0x025F74D0, mng, st);
        if (gabi::call<u16>(0x025F795C, mng) == 0xF /* MSG_CONTINUES */) {
            gabi::call<u32>(0x025F7DB0, mng, (u32)m778, (u32)0);
        }
        return FALSE;
    }
    if (gabi::call<u16>(0x025F795C, mng) == 0x12 /* BOX_CLOSED */) {
        gabi::call(0x025F74D0, mng, 0x13 /* MSG_DESTROYED */);
        rsh1_l_msgId = 0xFFFFFFFF;
        return TRUE;
    }
    if ((gabi::call<u16>(0x025F795C, mng) == 2 || gabi::call<u16>(0x025F795C, mng) == 6) && m778 == m784) {
        m784 = 0;
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x022DB12C, &daNpc_Rsh1_c::evn_talk);

/* 022DB29C */
BOOL daNpc_Rsh1_c::evn_turn() {
    WWHD_FUNC(0x022DB29C, BOOL, this);
    fopAc_ac_c* link_p = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B34));
    s16 target = cLib_targetAngleY(&current.pos, &link_p->current.pos);
    cLib_addCalcAngleS(&current.angle.y, target, 8, 0x1000, 0x100);
    return cLib_distanceAngleS(current.angle.y, target) < 0x100;
}
VERIFY(0x022DB29C, &daNpc_Rsh1_c::evn_turn);

/* 022DB314 */
bool daNpc_Rsh1_c::privateCut() {
    WWHD_FUNC(0x022DB314, bool, this);
    s32 staff_idx = dComIfGp_evmng_getMyStaffId(STR(gabi::load<u32>(gabi::ea(&mEventCut))) /* getActorName() */, nullptr, 0);
    if (staff_idx == -1) {
        return false;
    }
    s32 act_idx = gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staff_idx, 0x101C6168 /* cut_name_tbl */, 4, 1, 0);
    if (act_idx == -1) {
        dComIfGp_evmng_cutEnd(staff_idx);
        return true;
    }
    if (dComIfGp_evmng_getIsAddvance(staff_idx)) {
        switch (act_idx) {
        case 0: evn_talk_init(staff_idx); break;
        case 1: evn_continue_talk_init(staff_idx); break;
        case 2: evn_setAnm_init(staff_idx); break;
        case 3: evn_turn_init(staff_idx); break;
        }
    }
    BOOL result;
    switch (act_idx) {
    case 0:
    case 1:
        result = evn_talk();
        break;
    case 3:
        result = evn_turn();
        break;
    default:
        result = TRUE;
        break;
    }
    if (result) {
        dComIfGp_evmng_cutEnd(staff_idx);
    }
    return true;
}
VERIFY(0x022DB314, &daNpc_Rsh1_c::privateCut);

/* 022DB4A4 (the matcher lists it without source) */
BOOL daNpc_Rsh1_c::event_action(void*) {
    WWHD_FUNC(0x022DB4A4, BOOL, this, (u32)0);
    if (mActionStatus == 0) {
        dComIfGp_evmng_getMyStaffId(STR(0x100218D4) /* "Rsh1" */, nullptr, 0);
        (void)dComIfGp_get(); /* HD: a dead dComIfGp_getPlayer-style fetch remains (025200D4) */
        m95C = (s8)m95D;
        m95B = 0;
        mActionStatus = (s8)(mActionStatus + 1);
    } else if (mActionStatus != -1) {
        privateCut();
        if (dComIfGp_evmng_endCheck(mShopOutEventIdx)) {
            if (actor_status & 0x4000) {
                actor_status = actor_status & ~0x4000u;
            }
            u32 a = dComIfGp_ea() + 0x52B8; /* dComIfGp_event_reset() */
            gabi::store<u16>(a, (u16)(gabi::load<u16>(a) | 8));
            rsh1_setAction(this, RSH1_wait_action);
        }
        lookBack();
    }
    return TRUE;
}
VERIFY(0x022DB4A4, &daNpc_Rsh1_c::event_action);

/* 022DB674 */
BOOL daNpc_Rsh1_c::dummy_action(void*) {
    WWHD_FUNC(0x022DB674, BOOL, this, (u32)0);
    if (mActionStatus == 0) {
        mActionStatus = 1;
    }
    return TRUE;
}
VERIFY(0x022DB674, &daNpc_Rsh1_c::dummy_action);

/* 022DB824 */
static void __sinit_d_a_npc_rsh1_cpp() {
    WWHD_FUNC(0x022DB824, void);
    gabi::store<u32>(0x1046859C, 0);
    gabi::store<u32>(0x10468598, 0);
    gabi::store<u32>(0x10468594, 0);
    gabi::store<u32>(0x10468590, 0);
    gabi::call(0x028F026C, 0x101C6178); /* __register_global_object */
    gabi::store<f32>(0x104684C4, 3.1415927f);
    gabi::store<f32>(0x104684C0, -3.1415927f);
    gabi::call(0x028ED6F8, 0x104684CC);
    gabi::call(0x028F026C, 0x101C6184);
    gabi::call(0x028EAB2C, 0x104684CD);
    gabi::call(0x028F026C, 0x101C6190);
    gabi::call(0x022DB690, RSH1_HIO_ADDR); /* l_HIO: daNpc_Rsh1_HIO_c::daNpc_Rsh1_HIO_c */
    /* l_in_chk_pos1_tbl (0x104684D0), l_in_chk_pos2_tbl (0x10468500) */
    static const f32 t1[12] = {919.0f, 670.0f, -205111.0f, 1466.0f, 670.0f, -204723.0f,
                               1090.0f, 670.0f, -204173.0f, 603.0f, 670.0f, -204461.0f};
    static const f32 t2[12] = {1466.0f, 670.0f, -204723.0f, 1700.0f, 670.0f, -204518.0f,
                               1423.0f, 670.0f, -204174.0f, 1125.0f, 670.0f, -204337.0f};
    for (int i = 0; i < 12; i++) {
        gabi::store<f32>(0x10468500 + i * 4, t2[i]);
        gabi::store<f32>(0x104684D0 + i * 4, t1[i]);
    }
}
VERIFY(0x022DB824, __sinit_d_a_npc_rsh1_cpp);

/* 022DB9A4: daNpc_Rsh1_HIO_c::~daNpc_Rsh1_HIO_c (deleting destructor; nothing to destroy) */
static void daNpc_Rsh1_HIO_dt(daNpc_Rsh1_HIO_c* self, s32 flags) {
    WWHD_FUNC(0x022DB9A4, void, self, flags);
    if (self && (flags & 1)) {
        gabi::call(0x0273AF40, self); /* operator delete */
    }
}
VERIFY(0x022DB9A4, daNpc_Rsh1_HIO_dt);

/* 022DB9B8: daNpc_Rsh1_c::~daNpc_Rsh1_c (HD virtual destructor) */
static void daNpc_Rsh1_dt(daNpc_Rsh1_c* self, s32 flags) {
    WWHD_FUNC(0x022DB9B8, void, self, flags);
    if (self) {
        gabi::call(0x025886D8, &self->mSTControl, 2); /* STControl::~STControl */
        dCcD_Cyl_dt(&self->mCyl, 2);
        dCcD_Stts_dt(&self->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&self->mAcchCir) + 0x14, 2); /* dBgS_AcchCir: its cM3dGCir member */
        u32 acch = gabi::ea(&self->mAcch);
        gabi::store<u32>(acch + 0x20, 0x10021684); /* ~dBgS_ObjAcch: vtables back to dBgS_Acch */
        gabi::store<u32>(acch + 0x14, 0x10021694);
        gabi::call(0x024EFD9C, acch, 0); /* dBgS_Acch::~dBgS_Acch */
        gabi::call(0x025D50BC, self, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) {
            gabi::call(0x0273AF40, self);
        }
    }
}
VERIFY(0x022DB9B8, daNpc_Rsh1_dt);

/* 022DBA60: an empty function at the end of the TU (no direct callers; probably this TU's copy of
 * sead::SafeString::assureTerminationImpl_, as 022B5AE8 in d_a_npc_p1) */
static void rsh1_SafeString_assureTerminationImpl(SafeString* self) {
    WWHD_FUNC(0x022DBA60, void, self);
}
VERIFY(0x022DBA60, rsh1_SafeString_assureTerminationImpl);
