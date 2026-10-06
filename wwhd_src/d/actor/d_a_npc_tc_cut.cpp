/**
 * d_a_npc_tc_cut.cpp (WWHD)
 * NPC - Tingle, Ankle, David Jr., Knuckle: event cuts (GameCube d_a_npc_tc_cut.inc; the HD assert
 * names the file "d_a_npc_tc_dproc.inc") and the monument checks of d_a_npc_tc_msg_red.inc /
 * analysisCollectMap of d_a_npc_tc_msg_normal2.inc, which lie in the same WWHD range.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww) to the WWHD layout and code, verified against cking.rpx. "HD:" marks where
 * WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_tc.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline dSv_event_c* tc_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(tc_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(tc_event(), f); }
/* 02520864 dComIfGs_isStageTbox(stageNo, no) */
static inline BOOL dComIfGs_isStageTbox(s32 stageNo, s32 no) { return gabi::call<BOOL>(0x02520864, stageNo, no); }
/* dSv_player_map_c at save + 0xE4 (dComIfGs_isCollectMapTriforce(i) etc. take i - 1) */
static inline u32 tc_playerMap() { return gabi::load<u32>(0x101F84DC) + 0xE4; }
static inline BOOL dComIfGs_isCollectMapTriforce(int i) { return gabi::call<BOOL>(0x025B8308, tc_playerMap(), i - 1); }
static inline BOOL dComIfGs_isGetCollectMap(int i) { return gabi::call<BOOL>(0x025B7F68, tc_playerMap(), i - 1); }
static inline void dComIfGs_onCollectMapTriforce(int i) { gabi::call(0x025B82A0, tc_playerMap(), i - 1); }
static inline void* dComIfGp_evmng_getMyIntegerP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 3); }
static inline void* dComIfGp_evmng_getMyStringP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 4); }
static inline s32 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
/* fopAcM_createItemForPresentDemo(pos, itemNo, argFlag, itemBitNo, roomNo, angle, scale) */
static inline u32 fopAcM_createItemForPresentDemo(cXyz* pos, s32 itemNo, u8 argFlag, s32 bitNo, s32 roomNo, csXyz* angle, cXyz* scale) {
    return gabi::call<u32>(0x025D7DEC, pos, itemNo, argFlag, bitNo, roomNo, angle, scale);
}
static inline void dComIfGp_event_setItemPartnerId(u32 id) { gabi::store<u32>(dComIfGp_ea() + 0x52A0, id); }
/* dComIfGp_setItemRupeeCount(n): play + 0x5B48 += n */
static inline void dComIfGp_setItemRupeeCount(s32 n) {
    u32 p = dComIfGp_ea();
    gabi::store<s32>(p + 0x5B48, gabi::load<s32>(p + 0x5B48) + n);
}
/* fopAcM_seStart on this actor: GHS drops the inline's NULL checks */
static inline void tc_seStart(fopAc_ac_c* a, u32 id, u32 param = 0) {
    mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* 022ED850 cLib_calcTimer<char> (this TU's copy) */
static inline u8 cLib_calcTimer(be<s8>* t) { return gabi::call<u8>(0x022ED850, t); }
/* inline strcmp (byte loop) */
static inline int tc_strcmp(u32 a, u32 b) {
    u8 ca, cb;
    do {
        ca = gabi::load<u8>(a++);
        cb = gabi::load<u8>(b++);
    } while (ca == cb && ca != 0);
    return (int)ca - (int)cb;
}
/* JPABaseEmitter: global scale (+0x238/+0x23C/+0x240 HD) */
static inline void JPABaseEmitter_setGlobalParticleScale(JPABaseEmitter* e, f32 x, f32 y) {
    gabi::store<f32>(gabi::ea(e) + 0x238, x);
    gabi::store<f32>(gabi::ea(e) + 0x23C, y);
}

/* l_HIO (daNpc_Tc_HIO_c) at 0x104687F4: the GameCube offsets */
static inline f32 l_HIO_f(u32 off) { return gabi::load<f32>(0x104687F4 + off); }

enum {
    ANM_PRM_IDX_WAIT01 = 1, ANM_PRM_IDX_JAMP_A = 6, ANM_PRM_IDX_JAMP_B = 7, ANM_PRM_IDX_JAMP_C1 = 8,
    ANM_PRM_IDX_JAMP_C2 = 9, ANM_PRM_IDX_GUARD = 10, ANM_PRM_IDX_GET = 15,
};

/* 022E9D60 */
void daNpc_Tc_c::cutProc() {
    WWHD_FUNC(0x022E9D60, void, this);
    /* static char* action_table[9] = {"SIT_TO_JUMP", "PRESENT", "SET_ANM", "BACKJUMP", "EFFECT",
     * "DOOR_OPEN", "DOOR_CLOSE", "DOOR_CLOSE2", "PAY"} */
    int staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x10022A78) /* "Tc" */, nullptr, 0);
    if (staffIdx != -1) {
        int actIdx = dComIfGp_evmng_getMyActIdx(staffIdx, 0x101C6810, 9, TRUE, 0);
        if (actIdx == -1) {
            dComIfGp_evmng_cutEnd(staffIdx);
        } else {
            if (dComIfGp_evmng_getIsAddvance(staffIdx)) {
                switch (actIdx) {
                case 0: cutSitToJumpStart(staffIdx); break;
                case 1: /* cutPresentStart: empty, not called */ break;
                case 2: cutSetAnmStart(staffIdx); break;
                case 3: cutBackJumpStart(staffIdx); break;
                case 4: cutEffectStart(staffIdx); break;
                case 5: cutDoorOpenStart(staffIdx); break;
                case 6: cutDoorCloseStart(staffIdx); break;
                case 7: cutDoorClose2Start(staffIdx); break;
                case 8: cutPayStart(staffIdx); break;
                }
            }
            switch (actIdx) {
            case 0: cutSitToJumpProc(staffIdx); break;
            case 1: cutPresentProc(staffIdx); break;
            case 2: cutSetAnmProc(staffIdx); break;
            case 3: cutBackJumpProc(staffIdx); break;
            case 4: cutEffectProc(staffIdx); break;
            case 5: cutDoorOpenProc(staffIdx); break;
            case 6: cutDoorCloseProc(staffIdx); break;
            case 7: cutDoorClose2Proc(staffIdx); break;
            case 8: cutPayProc(staffIdx); break;
            }
        }
    }
}
VERIFY(0x022E9D60, &daNpc_Tc_c::cutProc);

/* 022E90F8 (unnamed by the matcher) */
void daNpc_Tc_c::cutSitToJumpStart(int i_staffIdx) {
    WWHD_FUNC(0x022E90F8, void, this, i_staffIdx);
    mAnmPrmIdx = ANM_PRM_IDX_JAMP_A;
}
VERIFY(0x022E90F8, &daNpc_Tc_c::cutSitToJumpStart);

/* 022E93D4 */
void daNpc_Tc_c::cutSitToJumpProc(int i_staffIdx) {
    WWHD_FUNC(0x022E93D4, void, this, i_staffIdx);
    if (mpMorf->isStop() && mAnmPrmIdx == ANM_PRM_IDX_JAMP_A) {
        speedF = l_HIO_f(0x38);
        speed.y = l_HIO_f(0x40);
        gravity = l_HIO_f(0x3C);
        mAnmPrmIdx = ANM_PRM_IDX_JAMP_B;
    }
    if (!(speed.y > 0.0f) && mAnmPrmIdx == ANM_PRM_IDX_JAMP_B) { /* GHS: NaN takes this branch */
        mAnmPrmIdx = ANM_PRM_IDX_JAMP_C1;
    }
    if (mObjAcch.ChkGroundHit() && mAnmPrmIdx == ANM_PRM_IDX_JAMP_C1) {
        speedF = 0.0f;
        mAnmPrmIdx = ANM_PRM_IDX_JAMP_C2;
        mJumpLandingTimer = 0x33;
        return;
    }
    if (cLib_calcTimer(&mJumpLandingTimer) == 0 && mAnmPrmIdx == ANM_PRM_IDX_JAMP_C2) {
        mAnmPrmIdx = ANM_PRM_IDX_WAIT01;
        dComIfGp_evmng_cutEnd(i_staffIdx);
    }
}
VERIFY(0x022E93D4, &daNpc_Tc_c::cutSitToJumpProc);

/* 022E9854 */
void daNpc_Tc_c::cutPresentProc(int i_staffIdx) {
    WWHD_FUNC(0x022E9854, void, this, i_staffIdx);
    be<s32>* pItemType = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, STR(0x10022A20) /* "ItemType" */);
    u32 temp;
    if (pItemType == nullptr) {
        temp = 0;
    } else {
        temp = *pItemType;
    }
    u32 itemNo = 0xFF; /* HD: dItem_data::ITEMNO_NULL, asserted below */
    switch (temp) {
    case 0:
        itemNo = 0x21; /* dItemNo_TINGLE_TUNER_e */
        break;
    case 1:
        itemNo = 0xDC; /* dItemNo_COLLECT_MAP_35_e */
        break;
    case 2:
        if (mType == TYPE_RED) {
            if (checkAllMonumentFee()) {
                itemNo = 0xB8; /* dItemNo_TINGLE_RUPEE_6_e */
            } else {
                u32 n = (u32)checkAllMonumentPay();
                if (n >= 1 && n <= 5)
                    itemNo = 0xB3 + n - 1; /* dItemNo_TINGLE_RUPEE_1_e.. (table at 0x10022A18) */
            }
        } else if (mType == TYPE_NORMAL2) {
            u32 n = (u32)analysisCollectMap();
            if (n >= 1 && n <= 8)
                itemNo = 0x79 + n - 1; /* dItemNo_TRIFORCE_MAP_1_e.. (table at 0x10022A10) */
        }
        break;
    }
    if (itemNo == 0xFF) /* JUT_ASSERT(207, item_name != dItem_data::ITEMNO_NULL) */
        JUT_ASSERT_fail(STR(0x10022A2C), 0xCF, STR(0x10022A44));
    u32 itemPID = fopAcM_createItemForPresentDemo(&current.pos, itemNo, 0, -1, fopAcM_GetRoomNo(this), nullptr, nullptr);
    if (itemPID != 0xFFFFFFFF) {
        dComIfGp_event_setItemPartnerId(itemPID);
        dComIfGp_evmng_cutEnd(i_staffIdx);
    }
}
VERIFY(0x022E9854, &daNpc_Tc_c::cutPresentProc);

/* 022E9104 */
void daNpc_Tc_c::cutSetAnmStart(int i_staffIdx) {
    WWHD_FUNC(0x022E9104, void, this, i_staffIdx);
    void* pName = dComIfGp_evmng_getMyStringP(i_staffIdx, STR(0x10022A08) /* "Name" */);
    if (pName != nullptr && tc_strcmp(gabi::ea(pName), 0x10022A04 /* "GET" */) == 0) {
        mAnmPrmIdx = ANM_PRM_IDX_GET;
    } else {
        mAnmPrmIdx = ANM_PRM_IDX_WAIT01;
    }
}
VERIFY(0x022E9104, &daNpc_Tc_c::cutSetAnmStart);

/* 022E9AB0 */
void daNpc_Tc_c::cutSetAnmProc(int i_staffIdx) {
    WWHD_FUNC(0x022E9AB0, void, this, i_staffIdx);
    dComIfGp_evmng_getMyStringP(i_staffIdx, STR(0x10022A6C) /* "Name" */);
    if (mAnmPrmIdx == ANM_PRM_IDX_GET) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
    }
    if (mpMorf->isStop()) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
    }
}
VERIFY(0x022E9AB0, &daNpc_Tc_c::cutSetAnmProc);

/* 022E919C */
void daNpc_Tc_c::cutBackJumpStart(int i_staffIdx) {
    WWHD_FUNC(0x022E919C, void, this, i_staffIdx);
    mTargetSpeed = 0.0f;
    speedF = l_HIO_f(0x48);
    speed.y = l_HIO_f(0x50);
    gravity = l_HIO_f(0x4C);
    mAnmPrmIdx = ANM_PRM_IDX_GUARD;
}
VERIFY(0x022E919C, &daNpc_Tc_c::cutBackJumpStart);

/* 022E9B54 */
void daNpc_Tc_c::cutBackJumpProc(int i_staffIdx) {
    WWHD_FUNC(0x022E9B54, void, this, i_staffIdx);
    dPa_smokeEcallBack_end((dPa_smokeEcallBack*)(void*)&mSmokeCallBack);
    if (mObjAcch.ChkGroundLanding()) {
        speedF = 0.0f;
        speed.y = 0.0f;
        mSmokePos.copy(current.pos);
        mSmokeAngle.x = current.angle.x;
        mSmokeAngle.y = current.angle.y;
        mSmokeAngle.z = current.angle.z;
        gabi::call(0x022E7CE4, this, 2.0f, 0.25f, 0.0f, 5.0f, 20.0f); /* smoke_set (d_a_npc_tc.cpp) */
    } else if (mObjAcch.ChkGroundHit() && speed.y == 0.0f) {
        speedF = 0.0f;
        dComIfGp_evmng_cutEnd(i_staffIdx);
    }
}
VERIFY(0x022E9B54, &daNpc_Tc_c::cutBackJumpProc);

/* 022E91D4 */
void daNpc_Tc_c::cutEffectStart(int i_staffIdx) {
    WWHD_FUNC(0x022E91D4, void, this, i_staffIdx);
    gabi::Local<cXyz> pos;
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    pos->z = current.pos.z;
    pos->y = pos->y - 80.0f;
    gabi::Local<cXyz> particleScale;
    particleScale->x = 1.0f;
    particleScale->y = 1.0f;
    particleScale->z = 1.0f;
    JPABaseEmitter* pEmitter = dComIfGp_particle_set(0x8152 /* dPa_name::ID_IT_SN_PF_BIKON00 */, pos, nullptr, particleScale);
    /* HD: setGlobalParticleScale also stores 1.0 at +0x240 first; no NULL check */
    gabi::store<f32>(gabi::ea(pEmitter) + 0x240, 1.0f);
    JPABaseEmitter_setGlobalParticleScale(pEmitter, 0.62f, 0.6f);
    tc_seStart(this, 0x58BD /* JA_SE_CM_CMN_NOTICE */, 0);
}
VERIFY(0x022E91D4, &daNpc_Tc_c::cutEffectStart);

/* 022E9C48 (unnamed by the matcher) */
void daNpc_Tc_c::cutEffectProc(int i_staffIdx) {
    WWHD_FUNC(0x022E9C48, void, this, i_staffIdx);
    dComIfGp_evmng_cutEnd(i_staffIdx);
}
VERIFY(0x022E9C48, &daNpc_Tc_c::cutEffectProc);

/* 022E92D0 */
void daNpc_Tc_c::cutDoorOpenStart(int i_staffIdx) {
    WWHD_FUNC(0x022E92D0, void, this, i_staffIdx);
    tc_seStart(this, 0x696E /* JA_SE_OBJ_DOOR_N_OPEN */, 0);
}
VERIFY(0x022E92D0, &daNpc_Tc_c::cutDoorOpenStart);

/* 022E9C80 (unnamed by the matcher) */
void daNpc_Tc_c::cutDoorOpenProc(int i_staffIdx) {
    WWHD_FUNC(0x022E9C80, void, this, i_staffIdx);
    dComIfGp_evmng_cutEnd(i_staffIdx);
}
VERIFY(0x022E9C80, &daNpc_Tc_c::cutDoorOpenProc);

/* 022E9318 */
void daNpc_Tc_c::cutDoorCloseStart(int i_staffIdx) {
    WWHD_FUNC(0x022E9318, void, this, i_staffIdx);
    tc_seStart(this, 0x696F /* JA_SE_OBJ_DOOR_N_CLOSE_1 */, 0);
}
VERIFY(0x022E9318, &daNpc_Tc_c::cutDoorCloseStart);

/* 022E9CB8 (unnamed by the matcher) */
void daNpc_Tc_c::cutDoorCloseProc(int i_staffIdx) {
    WWHD_FUNC(0x022E9CB8, void, this, i_staffIdx);
    dComIfGp_evmng_cutEnd(i_staffIdx);
}
VERIFY(0x022E9CB8, &daNpc_Tc_c::cutDoorCloseProc);

/* 022E9360 */
void daNpc_Tc_c::cutDoorClose2Start(int i_staffIdx) {
    WWHD_FUNC(0x022E9360, void, this, i_staffIdx);
    tc_seStart(this, 0x6970 /* JA_SE_OBJ_DOOR_N_CLOSE_2 */, 0);
}
VERIFY(0x022E9360, &daNpc_Tc_c::cutDoorClose2Start);

/* 022E9CF0 (unnamed by the matcher) */
void daNpc_Tc_c::cutDoorClose2Proc(int i_staffIdx) {
    WWHD_FUNC(0x022E9CF0, void, this, i_staffIdx);
    dComIfGp_evmng_cutEnd(i_staffIdx);
}
VERIFY(0x022E9CF0, &daNpc_Tc_c::cutDoorClose2Proc);

/* 022E93A8 (unnamed by the matcher) */
void daNpc_Tc_c::cutPayStart(int i_staffIdx) {
    WWHD_FUNC(0x022E93A8, void, this, i_staffIdx);
    dComIfGp_setItemRupeeCount(-398);
}
VERIFY(0x022E93A8, &daNpc_Tc_c::cutPayStart);

/* 022E9D28 (unnamed by the matcher) */
void daNpc_Tc_c::cutPayProc(int i_staffIdx) {
    WWHD_FUNC(0x022E9D28, void, this, i_staffIdx);
    dComIfGp_evmng_cutEnd(i_staffIdx);
}
VERIFY(0x022E9D28, &daNpc_Tc_c::cutPayProc);

/* ---- monument checks (d_a_npc_tc_msg_red.inc) ---- */

/* 022E972C */
bool daNpc_Tc_c::checkMonumentFee(u16 i_stageNo, u16 i_eventId) {
    WWHD_FUNC(0x022E972C, bool, this, i_stageNo, i_eventId);
    if (dComIfGs_isStageTbox(i_stageNo, 0xF)) {
        return dComIfGs_isEventBit(i_eventId) != 0;
    }
    return false;
}
VERIFY(0x022E972C, &daNpc_Tc_c::checkMonumentFee);

/* 022E979C */
bool daNpc_Tc_c::checkAllMonumentFee() {
    WWHD_FUNC(0x022E979C, bool, this);
    if (checkMonumentFee(3 /* STAGE_DRC */, 0x1240) && checkMonumentFee(4 /* STAGE_FW */, 0x1D08) &&
        checkMonumentFee(5 /* STAGE_TOTG */, 0x1D04) && checkMonumentFee(7 /* STAGE_WT */, 0x1D02) &&
        checkMonumentFee(6 /* STAGE_ET */, 0x1D01)) {
        return true;
    }
    return false;
}
VERIFY(0x022E979C, &daNpc_Tc_c::checkAllMonumentFee);

/* 022E95A8 */
int daNpc_Tc_c::checkAllMonumentPay() {
    WWHD_FUNC(0x022E95A8, int, this);
    int amountOfMonumentsPaid = 0;
    if (checkMonumentPay(3, 0x1240)) amountOfMonumentsPaid++;
    if (checkMonumentPay(4, 0x1D08)) amountOfMonumentsPaid++;
    if (checkMonumentPay(5, 0x1D04)) amountOfMonumentsPaid++;
    if (checkMonumentPay(7, 0x1D02)) amountOfMonumentsPaid++;
    if (checkMonumentPay(6, 0x1D01)) amountOfMonumentsPaid++;
    return amountOfMonumentsPaid;
}
VERIFY(0x022E95A8, &daNpc_Tc_c::checkAllMonumentPay);

/* 022E9508 */
bool daNpc_Tc_c::checkMonumentPay(u16 i_stageNo, u16 i_eventId) {
    WWHD_FUNC(0x022E9508, bool, this, i_stageNo, i_eventId);
    if (dComIfGs_isEventBit(i_eventId)) {
        return false;
    }
    if (dComIfGs_isStageTbox(i_stageNo, 0xF)) {
        dComIfGs_onEventBit(i_eventId);
        return true;
    }
    return false;
}
VERIFY(0x022E9508, &daNpc_Tc_c::checkMonumentPay);

/* 022E9668 (unnamed by the matcher; d_a_npc_tc_msg_normal2.inc) */
int daNpc_Tc_c::analysisCollectMap() {
    WWHD_FUNC(0x022E9668, int, this);
    for (int i = 1; i <= 8; i++) {
        if (!dComIfGs_isCollectMapTriforce(i) && dComIfGs_isGetCollectMap(i)) {
            dComIfGs_onCollectMapTriforce(i);
            return i;
        }
    }
    return 0;
}
VERIFY(0x022E9668, &daNpc_Tc_c::analysisCollectMap);
