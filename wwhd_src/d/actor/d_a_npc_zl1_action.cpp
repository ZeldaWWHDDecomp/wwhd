/**
 * d_a_npc_zl1_action.cpp (WWHD)
 * NPC - Tetra: action, talk and message functions
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_zl1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_zl1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 028E8DE8 PSVECSquareDistance(a, b) */
static inline f32 PSVECSquareDistance(const cXyz* a, const cXyz* b) { return gabi::call<f32>(0x028E8DE8, a, b); }
static inline u32 acch_flags(fopNpc_npc_c* a) { return gabi::load<u32>(gabi::ea(&a->mObjAcch) + 0x28); }
static inline void setPlaySpeed(mDoExt_McaMorf* m, f32 s) { gabi::store<f32>(gabi::ea(m) + 0x98, s); }

/* 02308DFC */
u8 daNpc_Zl1_c::chk_parts_notMov() {
    WWHD_FUNC(0x02308DFC, u8, this);
    return field_0x794.y != m_jnt.mAngles[0][1] || field_0x794.z != m_jnt.mAngles[1][1] || field_0x794.x != current.angle.y;
}
VERIFY(0x02308DFC, &daNpc_Zl1_c::chk_parts_notMov);

/* 02308E3C */
u8 daNpc_Zl1_c::chkAttention() {
    WWHD_FUNC(0x02308E3C, u8, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}
VERIFY(0x02308E3C, &daNpc_Zl1_c::chkAttention);

/* 02308D64 */
u8 daNpc_Zl1_c::chk_talk() {
    WWHD_FUNC(0x02308D64, u8, this);
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd()) {
            mItemNo = dComIfGp_event_getPreItemNo();
            return true;
        }
        return false;
    }
    mItemNo = 0xFF;
    return true;
}
VERIFY(0x02308D64, &daNpc_Zl1_c::chk_talk);

/* 02308C4C */
u32 daNpc_Zl1_c::getMsg_ZL1_2() {
    WWHD_FUNC(0x02308C4C, u32, this);
    u32 temp;
    if (dComIfGs_checkCollect(1) != 0) {
        temp = 0xCA2 + (dComIfGs_isEventBit(0x2908) != 0 ? 1 : 0);
    } else {
        temp = 0xCA1;
    }
    return temp;
}
VERIFY(0x02308C4C, &daNpc_Zl1_c::getMsg_ZL1_2);

/* 02308CBC */
u32 daNpc_Zl1_c::getMsg_ZL1_4() {
    WWHD_FUNC(0x02308CBC, u32, this);
    return 0x1005 + (dComIfGs_isEventBit(0x0810) != 0 ? 1 : 0);
}
VERIFY(0x02308CBC, &daNpc_Zl1_c::getMsg_ZL1_4);

/* 02308D08 */
u32 daNpc_Zl1_c::getMsg() {
    WWHD_FUNC(0x02308D08, u32, this);
    u32 msg = 0;
    switch ((u32)(s32)field_0x84F) {
    case 2:
        msg = getMsg_ZL1_2();
        break;
    case 4:
        msg = getMsg_ZL1_4();
        break;
    case 5:
        msg = 0x15EE; /* getMsg_ZL1_5() */
        break;
    }
    return msg;
}
VERIFY(0x02308D08, &daNpc_Zl1_c::getMsg);

/* 02308B90 */
u16 daNpc_Zl1_c::next_msgStatus(be<u32>* pMsgNo) {
    WWHD_FUNC(0x02308B90, u16, this, pMsgNo);
    u16 msg_status = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    u32 msg = msgMgr();
    switch (*pMsgNo) {
    case 0xCA2:
    case 0xCA3:
        *pMsgNo = 0xC90;
        break;
    case 0xC90:
        /* HD: the select number comes from the message manager (+0x948) */
        switch (gabi::load<u32>(msg + 0x948)) {
        case 0:
            msg_status = 0x10; /* fopMsgStts_MSG_ENDS_e */
            break;
        case 1:
            *pMsgNo = 0xC91;
            break;
        }
        break;
    case 0x15E1:
        *pMsgNo = 0x15E2;
        break;
    case 0x15E2:
        *pMsgNo = 0x15E3;
        break;
    case 0x15E3:
        *pMsgNo = 0x15E4;
        break;
    case 0x15E7:
        *pMsgNo = 0x15E8;
        break;
    default:
        msg_status = 0x10;
        break;
    }
    return msg_status;
}
VERIFY(0x02308B90, &daNpc_Zl1_c::next_msgStatus);

/* 0230883C */
void daNpc_Zl1_c::chngAnmAtr(u8 param_1) {
    WWHD_FUNC(0x0230883C, void, this, param_1);
    switch (mCurrMsgNo) {
    case 0x15E1:
        m_jnt.mbBackBoneLock = 1; /* onBackBoneLock() */
        break;
    case 0x15E2:
    case 0x15ED:
        field_0x84D = 1;
        break;
    case 0x15E3:
        field_0x84D = 5;
        field_0x7BC = 10;
        break;
    case 0x15E4:
        field_0x84D = 1;
        field_0x7BC = -1;
        break;
    case 0x15E7: {
        field_0x84D = 2;
        field_0x7D0 = true;
        f32 c1 = cM_scos(dComIfGp_getPlayer(0)->shape_angle.y);
        f32 s1 = cM_ssin(dComIfGp_getPlayer(0)->shape_angle.y);
        field_0x758.x = field_0x830.x + gabi::fmadds(c1, -20.0f, s1 * -40.0f);
        f32 s2 = cM_ssin(dComIfGp_getPlayer(0)->shape_angle.y);
        f32 c2 = cM_scos(dComIfGp_getPlayer(0)->shape_angle.y);
        f32 z = field_0x830.z + gabi::fmsubs(c2, -40.0f, s2 * -20.0f);
        field_0x758.y = field_0x830.y;
        field_0x758.z = z;
        break;
    }
    case 0x15EC: {
        field_0x84D = 2;
        field_0x7D0 = false;
        gabi::Local<cXyz> pos;
        kyoroPos(pos, 0xB);
        field_0x758.copy(*pos);
        break;
    }
    }
    /* HD: >= 0x13 (GameCube: > 0x13) */
    if (param_1 == field_0x845 || param_1 >= 0x13) {
        return;
    }
    field_0x845 = param_1;
    setAnm_ATR();
    if (field_0x845 == 0xC) {
        mpMorf->setMorf(16.0f);
    }
}
VERIFY(0x0230883C, &daNpc_Zl1_c::chngAnmAtr);

/* 02308AC0 */
void daNpc_Zl1_c::anmAtr(u16 mesgNo) {
    WWHD_FUNC(0x02308AC0, void, this, mesgNo);
    switch (mesgNo) {
    case 6: {
        if (field_0x851 == 0) {
            chngAnmAtr(dComIfGp_getMesgAnimeAttrInfo());
            field_0x851 = field_0x851 + 1;
        }
        u8 tag = gabi::load<u8>(dComIfGp_ea() + 0x5BC6); /* dComIfGp_getMesgAnimeTagInfo() */
        if (tag != 0xFF && tag != field_0x846) {
            gabi::store<u8>(dComIfGp_ea() + 0x5BC6, 0xFF);
            field_0x846 = tag;
            /* chngAnmTag(): empty */
        }
        break;
    }
    case 0xE:
        field_0x851 = 0;
        break;
    }
    /* ctrlAnmAtr(), ctrlAnmTag(): empty */
}
VERIFY(0x02308AC0, &daNpc_Zl1_c::anmAtr);

/* 02309210 */
u8 daNpc_Zl1_c::chk_areaIN(f32 param_1, f32 param_2, s16 param_3, cXyz* param_4) {
    WWHD_FUNC(0x02309210, u8, this, param_1, param_2, param_3, param_4);
    gabi::Local<cXyz> diff;
    cXyz_mi(&dComIfGp_getPlayer(0)->current.pos, diff, param_4);
    gabi::Local<cXyz> xz;
    f32 dx = diff->x, dz = diff->z;
    xz->y = 0.0f;
    xz->x = dx;
    xz->z = dz;
    f32 distXZ = std_sqrtf(PSVECSquareMag(xz));
    f32 distY = dComIfGp_getPlayer(0)->current.pos.y - param_4->y;
    s16 angle = cLib_targetAngleY(&current.pos, &dComIfGp_getPlayer(0)->current.pos) - current.angle.y;
    if (distXZ < param_1 && std::fabs(distY) < param_2 && abs((int)angle) < (int)param_3) {
        return true;
    }
    return false;
}
VERIFY(0x02309210, &daNpc_Zl1_c::chk_areaIN);

/* 02309394 */
BOOL daNpc_Zl1_c::wait_1() {
    WWHD_FUNC(0x02309394, BOOL, this);
    if (field_0x84F == 4) {
        cLib_addCalcAngleS(&current.angle.y, field_0x738.y, 4, 0x800, 0x80);
    }
    if (field_0x7D7) {
        if (chk_talk()) {
            setStt(2);
            field_0x84D = 1;
            field_0x7D8 = false;
            m_jnt.mbTrn = 1; /* setTrn() */
        }
        return TRUE;
    }
    field_0x84A = 2;
    field_0x7D8 = true;
    if (mHasAttention) {
        field_0x7B6 = (s16)cLib_getRndValue(0xF, 0x1E);
    }
    if (cLib_calcTimer(&field_0x7B6) != 0) {
        field_0x84D = 1;
        return TRUE;
    }
    field_0x84D = 0;
    return TRUE;
}
VERIFY(0x02309394, &daNpc_Zl1_c::wait_1);

/* 02309478 */
BOOL daNpc_Zl1_c::talk_1() {
    WWHD_FUNC(0x02309478, BOOL, this);
    BOOL ret = chk_parts_notMov();
    if (field_0x7D7) {
        talk(1);
        /* HD: the message status comes from the message manager (GameCube: mpCurrMsg->mStatus) */
        if (!mbHasMsg) {
            return ret;
        }
        if (fopMsgM_getStatus() == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
            switch (mCurrMsgNo) {
            case 0xC91:
                dComIfGs_onEventBit(0x2908);
                break;
            case 0x1005:
                dComIfGs_onEventBit(0x0810);
                break;
            case 0xC90:
                /* HD: room 0x2C */
                dComIfGp_setNextStage(STR(0x1002426C) /* "sea" */, 0xCD, 0x2C, 10, 0.0f, 0, 1, 0);
                field_0x7D7 = false;
                return ret;
            }
            field_0x7D7 = false;
            mItemNo = 0xFF;
            setStt(field_0x84C);
            field_0x7B6 = (s16)cLib_getRndValue(0xF, 0x1E);
            endEvent();
        }
    }
    return ret;
}
VERIFY(0x02309478, &daNpc_Zl1_c::talk_1);

/* 023095E4 */
BOOL daNpc_Zl1_c::demo_1() {
    WWHD_FUNC(0x023095E4, BOOL, this);
    if (field_0x84F == 0 && dComIfGs_isEventBit(0x0001)) {
        fopAcM_delete(this);
    }
    return TRUE;
}
VERIFY(0x023095E4, &daNpc_Zl1_c::demo_1);

/* 02309640 */
BOOL daNpc_Zl1_c::demo_2() {
    WWHD_FUNC(0x02309640, BOOL, this);
    if (field_0x84A == 1 || field_0x84A >= 3) {
        return TRUE;
    }
    gabi::Local<cXyz> pos; /* passed by value: a copy */
    pos->x = field_0x764.x;
    pos->y = field_0x764.y;
    pos->z = field_0x764.z;
    if (chk_areaIN(l_HIO().mPrmTbl.field_24, 100.0f, 0x7FFF, pos)) {
        field_0x84A = 4;
    }
    return TRUE;
}
VERIFY(0x02309640, &daNpc_Zl1_c::demo_2);

/* 023096CC */
BOOL daNpc_Zl1_c::demo_3() {
    WWHD_FUNC(0x023096CC, BOOL, this);
    if (field_0x84A == 1 || field_0x84A >= 3) {
        return TRUE;
    }
    if (dComIfGs_isEventBit(0x0801)) {
        fopAcM_delete(this);
        return TRUE;
    }
    f32 dist = std_sqrtf(PSVECSquareDistance(&field_0x764, &dComIfGp_getPlayer(0)->current.pos));
    if (dist < l_HIO().mPrmTbl.field_28) {
        field_0x84A = 5;
    }
    return TRUE;
}
VERIFY(0x023096CC, &daNpc_Zl1_c::demo_3);

/* 0230976C */
BOOL daNpc_Zl1_c::demo_4() {
    WWHD_FUNC(0x0230976C, BOOL, this);
    if (field_0x84A == 1 || field_0x84A >= 3) {
        return TRUE;
    }
    field_0x84A = 3;
    return TRUE;
}
VERIFY(0x0230976C, &daNpc_Zl1_c::demo_4);

/* 02309794 */
BOOL daNpc_Zl1_c::optn_1() {
    WWHD_FUNC(0x02309794, BOOL, this);
    f32 temp = l_HIO().mPrmTbl.field_34 + 100.0f;
    f32 actorDistSq = fopAcM_searchActorDistance2(this, dComIfGp_getPlayer(0)); /* fopAcM_searchPlayerDistance2 */
    if (field_0x7D7) {
        if (chk_talk()) {
            setStt(2);
            field_0x7D8 = false;
            m_jnt.mbTrn = 1; /* setTrn() */
            field_0x84D = 1;
        }
        return TRUE;
    }
    field_0x7D8 = true;
    field_0x84A = 0;
    if (!(actorDistSq < temp * temp)) {
        s16 angle = fopAcM_searchActorAngleY(this, dComIfGp_getPlayer(0)); /* fopAcM_searchPlayerAngleY */
        cLib_addCalcAngleS(&current.angle.y, angle, 4, 0x800, 0x80);
        if (abs((int)(s16)(angle - current.angle.y)) < 0x1800) {
            setStt(4);
            if (actorDistSq > l_HIO().mPrmTbl.field_4C * l_HIO().mPrmTbl.field_4C) {
                field_0x7AE = l_HIO().mPrmTbl.field_32;
                field_0x7B0 = l_HIO().mPrmTbl.field_2E;
            } else {
                field_0x7AE = l_HIO().mPrmTbl.field_30;
                field_0x7B0 = l_HIO().mPrmTbl.field_2C;
            }
            field_0x7B2 = 0;
        } else {
            field_0x84A = 2;
        }
        field_0x84D = 1;
        return TRUE;
    }
    field_0x84A = 2;
    if (field_0x849 == 0xC) {
        if (field_0x7C3 != 0 && cLib_calcTimer(&field_0x7BA) == 0) {
            setAnm_NUM(8, 1);
            field_0x7B8 = (s16)cLib_getRndValue(0x5A, 0xB4);
        }
        field_0x84D = 0;
        return TRUE;
    }
    if (cLib_calcTimer(&field_0x7B8) == 0) {
        setAnm_NUM(0xC, 1);
        field_0x84D = 0;
        field_0x7BA = (s16)((g_Counter_mCounter0() & 1) + 1);
        return TRUE;
    }
    field_0x84D = 1;
    return TRUE;
}
VERIFY(0x02309794, &daNpc_Zl1_c::optn_1);

/* 023099F4 */
BOOL daNpc_Zl1_c::optn_2() {
    WWHD_FUNC(0x023099F4, BOOL, this);
    if (field_0x7CC) {
        field_0x7CC = move_jmp();
        return TRUE;
    }
    f32 actorDist = fopAcM_searchActorDistance2(this, dComIfGp_getPlayer(0));
    f32 f34 = l_HIO().mPrmTbl.field_34;
    f32 temp = gabi::fnmsubs(f34, f34, actorDist); /* actorDist - SQUARE(field_34) */
    f32 temp2 = 0.0f;
    if (temp > 0.0f) {
        temp2 = std_sqrtf(temp) * l_HIO().mPrmTbl.field_38;
        f32 lim = l_HIO().mPrmTbl.field_3C;
        temp2 = (temp2 - lim >= 0.0f) ? lim : temp2; /* cLib_maxLimit (fsel) */
    }
    s16 angle = fopAcM_searchActorAngleY(this, dComIfGp_getPlayer(0));
    cLib_addCalcAngleS(&current.angle.y, angle, 4, 0x800, 0x80);
    cLib_chaseF(&speedF, temp2, l_HIO().mPrmTbl.field_44);
    if (gabi::ftoi(temp2) == 0 && gabi::ftoi(speedF) == 0) {
        setStt(3);
        field_0x7D8 = false;
        field_0x84D = 0;
    }
    if ((acch_flags(this) & 0x10) /* ChkWallHit() */ && setFrontWallType()) {
        setAnm_NUM(10, 1);
        speedF = l_HIO().mPrmTbl.field_58;
        field_0x7CC = true;
        speed.y = l_HIO().mPrmTbl.field_54;
        return TRUE;
    }
    if (!(acch_flags(this) & 0x20) /* !ChkGroundHit() */ && mObjAcch.GetGroundH() - current.pos.y < -30.1f) {
        setAnm_NUM(10, 1);
        field_0x7CC = true;
        speed.y = l_HIO().mPrmTbl.field_54;
        return TRUE;
    }
    f32 sq = l_HIO().mPrmTbl.field_4C * l_HIO().mPrmTbl.field_4C;
    if (field_0x849 == 0xB) {
        if (!(temp > sq)) {
            setAnm_NUM(9, 1);
        }
    } else if (temp > sq) {
        setAnm_NUM(0xB, 1);
    }
    f32 spd = speedF * l_HIO().mPrmTbl.field_50;
    spd = (spd - 0.9f >= 0.0f) ? spd : 0.9f; /* cLib_minLimit (fsel) */
    setPlaySpeed(mpMorf, spd);
    field_0x84D = cLib_calcTimer(&field_0x7AE) != 0 ? 1 : 5;
    return TRUE;
}
VERIFY(0x023099F4, &daNpc_Zl1_c::optn_2);

/* 02309CC4 */
BOOL daNpc_Zl1_c::optn_3() {
    WWHD_FUNC(0x02309CC4, BOOL, this);
    if (field_0x84A == 1 || field_0x84A >= 3) {
        return TRUE;
    }
    field_0x84A = 6;
    return TRUE;
}
VERIFY(0x02309CC4, &daNpc_Zl1_c::optn_3);

/* 02309CEC */
BOOL daNpc_Zl1_c::wait_action1(void*) {
    WWHD_FUNC(0x02309CEC, BOOL, this, (void*)nullptr);
    switch ((u32)(s32)field_0x850) {
    case 0:
        if (field_0x84F == 2 && !dComIfGs_isEventBit(0x0802)) {
            setStt(9);
            field_0x850 = field_0x850 + 1;
            /* HD: the event is ordered in the same frame */
            demo_4();
            eventOrder();
        } else {
            setStt(1);
            field_0x850 = field_0x850 + 1;
        }
        break;
    case 1:
    case 2:
    case 3:
        mHasAttention = chkAttention();
        switch ((u32)(s32)field_0x84B) {
        case 9:
            field_0x79C = demo_4();
            break;
        case 1:
            field_0x79C = wait_1();
            break;
        case 2:
            field_0x79C = talk_1();
            break;
        }
        break;
    }
    return TRUE;
}
VERIFY(0x02309CEC, &daNpc_Zl1_c::wait_action1);

/* 02309E10 */
BOOL daNpc_Zl1_c::demo_action1(void*) {
    WWHD_FUNC(0x02309E10, BOOL, this, (void*)nullptr);
    switch ((u32)(s32)field_0x850) {
    case 0:
        setStt(6);
        if (field_0x84F == 1) {
            setAnm_NUM(6, 1);
        } else {
            setAnm_NUM(0, 1);
        }
        field_0x850 = field_0x850 + 1;
        break;
    case 1:
    case 2:
    case 3:
        if (field_0x84B == 6) {
            field_0x79C = demo_1();
        }
        break;
    }
    return TRUE;
}
VERIFY(0x02309E10, &daNpc_Zl1_c::demo_action1);

/* 02309EB8 */
BOOL daNpc_Zl1_c::demo_action2(void*) {
    WWHD_FUNC(0x02309EB8, BOOL, this, (void*)nullptr);
    switch ((u32)(s32)field_0x850) {
    case 0:
        if (dComIfGs_isEventBit(0x0804)) {
            setStt(8);
            current.pos.set(-37692.0f, 2200.0f, 8016.0f);
            current.angle.y = -0x5C72;
            field_0x764.y = current.pos.y;
            field_0x850 = field_0x850 + 1;
        } else {
            setStt(7);
            field_0x850 = field_0x850 + 1;
        }
        break;
    case 1:
    case 2:
    case 3:
        switch ((u32)(s32)field_0x84B) {
        case 7:
            field_0x79C = demo_2();
            break;
        case 8:
            field_0x79C = demo_3();
            break;
        }
        break;
    }
    return TRUE;
}
VERIFY(0x02309EB8, &daNpc_Zl1_c::demo_action2);

/* 02309FC8 */
BOOL daNpc_Zl1_c::optn_action1(void*) {
    WWHD_FUNC(0x02309FC8, BOOL, this, (void*)nullptr);
    switch ((u32)(s32)field_0x850) {
    case 0:
        if (!dComIfGs_isEventBit(0x3804 /* HYRULE_COURTYARD_CUTSCENE */)) {
            setStt(5);
            field_0x850 = field_0x850 + 1;
        } else {
            setStt(3);
            field_0x850 = field_0x850 + 1;
        }
        break;
    case 1:
    case 2:
    case 3:
        mHasAttention = chkAttention();
        switch ((u32)(s32)field_0x84B) {
        case 3:
            field_0x79C = optn_1();
            break;
        case 4:
            field_0x79C = optn_2();
            break;
        case 5:
            field_0x79C = optn_3();
            break;
        case 2:
            field_0x79C = talk_1();
            break;
        }
        break;
    }
    return TRUE;
}
VERIFY(0x02309FC8, &daNpc_Zl1_c::optn_action1);
