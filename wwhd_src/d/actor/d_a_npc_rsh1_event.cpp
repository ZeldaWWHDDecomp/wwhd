/**
 * d_a_npc_rsh1_event.cpp (WWHD): Zunari's event cuts, event/dummy actions, static init and destructors.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_rsh1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_rsh1.h"

static const f32 t1[12] = {919.0f, 670.0f, -205111.0f, 1466.0f, 670.0f, -204723.0f,
                               1090.0f, 670.0f, -204173.0f, 603.0f, 670.0f, -204461.0f};
static const f32 t2[12] = {1466.0f, 670.0f, -204723.0f, 1700.0f, 670.0f, -204518.0f,
                               1423.0f, 670.0f, -204174.0f, 1125.0f, 670.0f, -204337.0f};

/* 022DB03C */
bool daNpc_Rsh1_c::evn_setAnm_init(int i_staffIdx) {
    be<s32>* anm_no_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, "AnmNo" /* "AnmNo" */, 3);
    if (anm_no_p) {
        setAnm((s8)(s32)*anm_no_p);
    }
    return true;
}

/* 022DAF30 */
bool daNpc_Rsh1_c::evn_talk_init(int i_staffIdx) {
    be<u32>* msg_no_p = (be<u32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, "MsgNo" /* "MsgNo" */, 3);
    be<u32>* end_msg_no_p = (be<u32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, "EndMsgNo" /* "EndMsgNo" */, 3);
    rsh1_messageId() = 0xFFFFFFFF;
    m778 = msg_no_p ? (u32)*msg_no_p : 0;
    m784 = end_msg_no_p ? (u32)*end_msg_no_p : 0;
    return true;
}

/* 022DAFD8 */
bool daNpc_Rsh1_c::evn_continue_talk_init(int i_staffIdx) {
    be<u32>* end_msg_no_p = (be<u32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, "EndMsgNo" /* "EndMsgNo" */, 3);
    m784 = end_msg_no_p ? (u32)*end_msg_no_p : 0;
    return true;
}

/* 022DB12C */
BOOL daNpc_Rsh1_c::evn_talk() {
    RshMessageManager* mng = rsh1_messageManager(); /* HD message manager */
    if (rsh1_messageId() == 0xFFFFFFFF) {
        u32 id = rsh1m_msgSet(mng, m778, &eyePos); /* fopMsgM_messageSet */
        rsh1_messageId() = id;
        if (id != 0xFFFFFFFF) {
            rsh1m_demoMsgFlagOn(); /* fopMsgM_demoMsgFlagOn */
        }
        return FALSE;
    }
    setAnmFromMsgTag();
    if (rsh1m_msgGetStatus(mng) == 0xE /* MSG_DISPLAYED */) {
        u32 st = next_msgStatus(&m778, nullptr);
        rsh1m_msgSetStatus(mng, st);
        if (rsh1m_msgGetStatus(mng) == 0xF /* MSG_CONTINUES */) {
            rsh1m_msgSet(mng, m778, nullptr);
        }
        return FALSE;
    }
    if (rsh1m_msgGetStatus(mng) == 0x12 /* BOX_CLOSED */) {
        rsh1m_msgSetStatus(mng, 0x13 /* MSG_DESTROYED */);
        rsh1_messageId() = 0xFFFFFFFF;
        return TRUE;
    }
    if ((rsh1m_msgGetStatus(mng) == 2 || rsh1m_msgGetStatus(mng) == 6) && m778 == m784) {
        m784 = 0;
        return TRUE;
    }
    return FALSE;
}

/* 022DB0A8 */
bool daNpc_Rsh1_c::evn_turn_init(int i_staffIdx) {
    be<s32>* prm_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, "prm" /* "prm" */, 3);
    fopAc_ac_c* link_p = dComIfGp_getLinkPlayer();
    if (prm_p && *prm_p == 1) {
        current.angle.y = cLib_targetAngleY(&current.pos, &link_p->current.pos);
    }
    return true;
}

/* 022DB29C */
BOOL daNpc_Rsh1_c::evn_turn() {
    fopAc_ac_c* link_p = dComIfGp_getLinkPlayer();
    s16 target = cLib_targetAngleY(&current.pos, &link_p->current.pos);
    cLib_addCalcAngleS(&current.angle.y, target, 8, 0x1000, 0x100);
    return cLib_distanceAngleS(current.angle.y, target) < 0x100;
}

/* 022DB314 */
bool daNpc_Rsh1_c::privateCut() {
    s32 staff_idx = dComIfGp_evmng_getMyStaffId(mEventCut.getActorName() /* getActorName() */, nullptr, 0);
    if (staff_idx == -1) {
        return false;
    }
    s32 act_idx = rsh1_getActIdx(dComIfGp_getPEvtManager(), staff_idx, rsh1_cutNames(), 4, 1, 0);
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

/* 022DB4A4 */
BOOL daNpc_Rsh1_c::event_action(void*) {
    if (mActionStatus == 0) {
        dComIfGp_evmng_getMyStaffId("Rsh1" /* "Rsh1" */, nullptr, 0);
        (void)dComIfGp_get(); // HD: a dead dComIfGp_getPlayer-style fetch remains (025200D4)
        m95C = (s8)m95D;
        m95B = 0;
        mActionStatus = (s8)(mActionStatus + 1);
    } else if (mActionStatus != -1) {
        privateCut();
        if (dComIfGp_evmng_endCheck(mShopOutEventIdx)) {
            if (actor_status & 0x4000) {
                actor_status = actor_status & ~0x4000u;
            }
            dComIfGp_event_reset();
            rsh1_setAction(this, RSH1_wait_action);
        }
        lookBack();
    }
    return TRUE;
}

/* 022DB674 */
BOOL daNpc_Rsh1_c::dummy_action(void*) {
    if (mActionStatus == 0) {
        mActionStatus = 1;
    }
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.

/* 022DB824 */
static void __sinit_d_a_npc_rsh1_cpp() {
    rsh1_statics().mHeaderZeros[3] = 0;
    rsh1_statics().mHeaderZeros[2] = 0;
    rsh1_statics().mHeaderZeros[1] = 0;
    rsh1_statics().mHeaderZeros[0] = 0;
    rsh1_registerGlobal(RSH1_HEADER_RECORD_0); /* __register_global_object */
    rsh1_statics().mPositivePi = 3.1415927f;
    rsh1_statics().mNegativePi = -3.1415927f;
    rsh1_initializeHeaderByte(gabi::ea(&rsh1_statics().mHeaderByte));
    rsh1_registerGlobal(RSH1_HEADER_RECORD_1);
    rsh1_initializeHeaderFlag(gabi::ea(&rsh1_statics().mHeaderFlag));
    rsh1_registerGlobal(RSH1_HEADER_RECORD_2);
    rsh1_constructHio(RSH1_HIO_ADDR); /* l_HIO: daNpc_Rsh1_HIO_c::daNpc_Rsh1_HIO_c */
    /* l_in_chk_pos1_tbl (0x104684D0), l_in_chk_pos2_tbl (0x10468500) */

    for (int i = 0; i < 12; i++) {
        rsh1_components(rsh1_statics().mShopArea2)[i] = t2[i];
        rsh1_components(rsh1_statics().mShopArea1)[i] = t1[i];
    }
}

/* 022DB9A4 */
static void daNpc_Rsh1_HIO_dt(daNpc_Rsh1_HIO_c* self, s32 flags) {
    if (self && (flags & 1)) {
        rsh1_operatorDelete(self); /* operator delete */
    }
}

/* 022DB9B8 */
static void daNpc_Rsh1_dt(daNpc_Rsh1_c* self, s32 flags) {
    if (self) {
        rsh1_stickDestroy(&self->mSTControl, 2); /* STControl::~STControl */
        dCcD_Cyl_dt(&self->mCyl, 2);
        dCcD_Stts_dt(&self->mStts, 2);
        rsh1_circleDestroy(gabi::ea(&self->mAcchCir.m_cir), 2); /* dBgS_AcchCir: its cM3dGCir member */
        dBgS_ObjAcch* acch = &self->mAcch;
        acch->dBgS_GrpPassChk::__vtbl = gc_tu.objAcchDt[0]; /* ~dBgS_ObjAcch: vtables back to dBgS_Acch */
        acch->dBgS_PolyPassChk::__vtbl = gc_tu.objAcchDt[1];
        rsh1_acchDestroy(gabi::ea(acch), 0); /* dBgS_Acch::~dBgS_Acch */
        rsh1_actorDestroy(self, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) {
            rsh1_operatorDelete(self);
        }
    }
}

/* 022DBA60 */
static void rsh1_SafeString_assureTerminationImpl(SafeString* self) {
}

/* WWHD glue

str 10021880 in 022DAF30
str 10021888 in 022DAF30
str 10021894 in 022DAFD8
str 100218A0 in 022DB03C
str 100218A8 in 022DB0A8
str 100218D4 in 022DB4A4
*/
