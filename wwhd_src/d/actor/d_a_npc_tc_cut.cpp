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

#include "d/actor/d_a_npc_tc_cut.h"
#include "gc_api/d/d_com_inf_game.h"

static daNpc_Tc_HIO_cut l_cutHIO;
static TcActionTable action_table;
using GuestByte = be<u8>;
static GuestByte l_staffName;
static GuestByte l_itemType;
static GuestByte l_animationName;
static GuestByte l_getAnimation;
static GuestByte l_animationNameProc;
static GuestByte l_assertFile;
static GuestByte l_assertItem;

BOOL tc_isStageTbox(s32 stageNo, s32 no);
u32 tc_createItemForPresentDemo(cXyz *, s32, u8, s32, s32, csXyz *, cXyz *);
u8 tc_calcTimer(be<s8> *);

// HD: GHS omitted the GameCube sound inline's NULL checks.
static void tc_seStart(fopAc_ac_c *actor, u32 sound, u32 param = 0) {
    mDoAud_seStart(sound, &actor->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
}

// HD: inline unsigned-byte strcmp, including reads from guest read-only strings.
static int tc_strcmp(const GuestByte *a, const GuestByte *b) {
    u8 ca, cb;
    do {
        ca = *a++;
        cb = *b++;
    } while (ca == cb && ca != 0);
    return int(ca) - int(cb);
}

enum {
    ANM_PRM_IDX_WAIT01 = 1,
    ANM_PRM_IDX_JAMP_A = 6,
    ANM_PRM_IDX_JAMP_B = 7,
    ANM_PRM_IDX_JAMP_C1 = 8,
    ANM_PRM_IDX_JAMP_C2 = 9,
    ANM_PRM_IDX_GUARD = 10,
    ANM_PRM_IDX_GET = 15,
};

enum {
    dItemNo_TINGLE_TUNER_e = 0x21,
    dItemNo_COLLECT_MAP_35_e = 0xDC,
    dItemNo_TINGLE_RUPEE_1_e = 0xB3,
    dItemNo_TINGLE_RUPEE_6_e = 0xB8,
    dItemNo_TRIFORCE_MAP_1_e = 0x79,
    ITEMNO_NULL = 0xFF,
    ID_IT_SN_PF_BIKON00 = 0x8152,
    JA_SE_CM_CMN_NOTICE = 0x58BD,
    JA_SE_OBJ_DOOR_N_OPEN = 0x696E,
    JA_SE_OBJ_DOOR_N_CLOSE_1 = 0x696F,
    JA_SE_OBJ_DOOR_N_CLOSE_2 = 0x6970,
    STAGE_DRC = 3,
    STAGE_FW = 4,
    STAGE_TOTG = 5,
    STAGE_WT = 7,
    STAGE_ET = 6,
    UNK_1240 = 0x1240,
    UNK_1D08 = 0x1D08,
    UNK_1D04 = 0x1D04,
    UNK_1D02 = 0x1D02,
    UNK_1D01 = 0x1D01,
};

/* 022E9D60 */
void daNpc_Tc_c::cutProc() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    // action_table: SIT_TO_JUMP, PRESENT, SET_ANM, BACKJUMP, EFFECT,
    // DOOR_OPEN, DOOR_CLOSE, DOOR_CLOSE2, PAY.
    int staffIdx = dComIfGp_evmng_getMyStaffId(
        reinterpret_cast<const char *>(&l_staffName) /* "Tc" */, nullptr, 0);
    if (staffIdx != -1) {
        int actIdx = dComIfGp_evmng_getMyActIdx(staffIdx, action_table.entries, 9, TRUE, 0);
        if (actIdx == -1) {
            dComIfGp_evmng_cutEnd(staffIdx);
        } else {
            if (dComIfGp_evmng_getIsAddvance(staffIdx)) {
                switch (actIdx) {
                case 0:
                    cutSitToJumpStart(staffIdx);
                    break;
                case 1: /* HD: empty cutPresentStart is not called. */
                    break;
                case 2:
                    cutSetAnmStart(staffIdx);
                    break;
                case 3:
                    cutBackJumpStart(staffIdx);
                    break;
                case 4:
                    cutEffectStart(staffIdx);
                    break;
                case 5:
                    cutDoorOpenStart(staffIdx);
                    break;
                case 6:
                    cutDoorCloseStart(staffIdx);
                    break;
                case 7:
                    cutDoorClose2Start(staffIdx);
                    break;
                case 8:
                    cutPayStart(staffIdx);
                    break;
                }
            }
            switch (actIdx) {
            case 0:
                cutSitToJumpProc(staffIdx);
                break;
            case 1:
                cutPresentProc(staffIdx);
                break;
            case 2:
                cutSetAnmProc(staffIdx);
                break;
            case 3:
                cutBackJumpProc(staffIdx);
                break;
            case 4:
                cutEffectProc(staffIdx);
                break;
            case 5:
                cutDoorOpenProc(staffIdx);
                break;
            case 6:
                cutDoorCloseProc(staffIdx);
                break;
            case 7:
                cutDoorClose2Proc(staffIdx);
                break;
            case 8:
                cutPayProc(staffIdx);
                break;
            }
        }
    }
}

/* 022E90F8 */
void daNpc_Tc_c::cutSitToJumpStart(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mAnmPrmIdx = ANM_PRM_IDX_JAMP_A;
}

/* 022E93D4 */
void daNpc_Tc_c::cutSitToJumpProc(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mpMorf->isStop() && mAnmPrmIdx == ANM_PRM_IDX_JAMP_A) {
        speedF = l_cutHIO.field_0x38;
        speed.y = l_cutHIO.field_0x40;
        gravity = l_cutHIO.field_0x3C;
        mAnmPrmIdx = ANM_PRM_IDX_JAMP_B;
    }
    if (!(speed.y > 0.0f) &&
        mAnmPrmIdx == ANM_PRM_IDX_JAMP_B) { // HD: GHS also takes this branch for NaN.
        mAnmPrmIdx = ANM_PRM_IDX_JAMP_C1;
    }
    if (mObjAcch.ChkGroundHit() && mAnmPrmIdx == ANM_PRM_IDX_JAMP_C1) {
        speedF = 0.0f;
        mAnmPrmIdx = ANM_PRM_IDX_JAMP_C2;
        mJumpLandingTimer = 0x33;
        return;
    }
    if (tc_calcTimer(&mJumpLandingTimer) == 0 && mAnmPrmIdx == ANM_PRM_IDX_JAMP_C2) {
        mAnmPrmIdx = ANM_PRM_IDX_WAIT01;
        dComIfGp_evmng_cutEnd(i_staffIdx);
    }
}

/* 022E9854 */
void daNpc_Tc_c::cutPresentProc(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    be<s32> *pItemType = (be<s32> *)dComIfGp_evmng_getMyIntegerP(
        i_staffIdx, reinterpret_cast<const char *>(&l_itemType) /* "ItemType" */);
    u32 temp;
    if (pItemType == nullptr) {
        temp = 0;
    } else {
        temp = *pItemType;
    }
    u32 itemNo = ITEMNO_NULL; // HD: initialise to dItem_data::ITEMNO_NULL for the assertion below.
    switch (temp) {
    case 0:
        itemNo = dItemNo_TINGLE_TUNER_e;
        break;
    case 1:
        itemNo = dItemNo_COLLECT_MAP_35_e;
        break;
    case 2:
        if (mType == TYPE_RED) {
            if (checkAllMonumentFee()) {
                itemNo = dItemNo_TINGLE_RUPEE_6_e;
            } else {
                u32 n = (u32)checkAllMonumentPay();
                if (n >= 1 && n <= 5)
                    itemNo = dItemNo_TINGLE_RUPEE_1_e + n - 1;
            }
        } else if (mType == TYPE_NORMAL2) {
            u32 n = (u32)analysisCollectMap();
            if (n >= 1 && n <= 8)
                itemNo = dItemNo_TRIFORCE_MAP_1_e + n - 1;
        }
        break;
    }
    if (itemNo == ITEMNO_NULL) /* JUT_ASSERT(207, item_name != dItem_data::ITEMNO_NULL) */
        JUT_ASSERT_fail(reinterpret_cast<const char *>(&l_assertFile), 0xCF,
                        reinterpret_cast<const char *>(&l_assertItem));
    u32 itemPID = tc_createItemForPresentDemo(&current.pos, itemNo, 0, -1, fopAcM_GetRoomNo(this),
                                              nullptr, nullptr);
    if (itemPID != 0xFFFFFFFF) {
        dComIfGp_event_setItemPartnerId(itemPID);
        dComIfGp_evmng_cutEnd(i_staffIdx);
    }
}

/* 022E9104 */
void daNpc_Tc_c::cutSetAnmStart(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    void *pName = dComIfGp_evmng_getMyStringP(
        i_staffIdx, reinterpret_cast<const char *>(&l_animationName) /* "Name" */);
    if (pName != nullptr &&
        tc_strcmp(reinterpret_cast<const GuestByte *>(pName), &l_getAnimation) == 0) {
        mAnmPrmIdx = ANM_PRM_IDX_GET;
    } else {
        mAnmPrmIdx = ANM_PRM_IDX_WAIT01;
    }
}

/* 022E9AB0 */
void daNpc_Tc_c::cutSetAnmProc(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfGp_evmng_getMyStringP(i_staffIdx,
                                reinterpret_cast<const char *>(&l_animationNameProc) /* "Name" */);
    if (mAnmPrmIdx == ANM_PRM_IDX_GET) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
    }
    if (mpMorf->isStop()) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
    }
}

/* 022E919C */
void daNpc_Tc_c::cutBackJumpStart(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mTargetSpeed = 0.0f;
    speedF = l_cutHIO.field_0x48;
    speed.y = l_cutHIO.field_0x50;
    gravity = l_cutHIO.field_0x4C;
    mAnmPrmIdx = ANM_PRM_IDX_GUARD;
}

/* 022E9B54 */
void daNpc_Tc_c::cutBackJumpProc(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dPa_smokeEcallBack_end((dPa_smokeEcallBack *)(void *)&mSmokeCallBack);
    if (mObjAcch.ChkGroundLanding()) {
        speedF = 0.0f;
        speed.y = 0.0f;
        mSmokePos.copy(current.pos);
        mSmokeAngle.x = current.angle.x;
        mSmokeAngle.y = current.angle.y;
        mSmokeAngle.z = current.angle.z;
        smoke_set(2.0f, 0.25f, 0.0f, 5.0f, 20.0f);
    } else if (mObjAcch.ChkGroundHit() && speed.y == 0.0f) {
        speedF = 0.0f;
        dComIfGp_evmng_cutEnd(i_staffIdx);
    }
}

/* 022E91D4 */
void daNpc_Tc_c::cutEffectStart(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<cXyz> pos;
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    pos->z = current.pos.z;
    pos->y = pos->y - 80.0f;
    gabi::Local<cXyz> particleScale;
    particleScale->x = 1.0f;
    particleScale->y = 1.0f;
    particleScale->z = 1.0f;
    JPABaseEmitter *pEmitter =
        dComIfGp_particle_set(ID_IT_SN_PF_BIKON00, pos, nullptr, particleScale);
    /* HD: setGlobalParticleScale also stores 1.0 at +0x240 first; no NULL check */
    auto *emitter = reinterpret_cast<TcEmitter *>(pEmitter);
    emitter->mGlobalParticleScale.z = 1.0f;
    emitter->mGlobalParticleScale.x = 0.62f;
    emitter->mGlobalParticleScale.y = 0.6f;
    tc_seStart(this, JA_SE_CM_CMN_NOTICE, 0);
}

/* 022E9C48 */
void daNpc_Tc_c::cutEffectProc(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfGp_evmng_cutEnd(i_staffIdx);
}

/* 022E92D0 */
void daNpc_Tc_c::cutDoorOpenStart(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    tc_seStart(this, JA_SE_OBJ_DOOR_N_OPEN, 0);
}

/* 022E9C80 */
void daNpc_Tc_c::cutDoorOpenProc(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfGp_evmng_cutEnd(i_staffIdx);
}

/* 022E9318 */
void daNpc_Tc_c::cutDoorCloseStart(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    tc_seStart(this, JA_SE_OBJ_DOOR_N_CLOSE_1, 0);
}

/* 022E9CB8 */
void daNpc_Tc_c::cutDoorCloseProc(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfGp_evmng_cutEnd(i_staffIdx);
}

/* 022E9360 */
void daNpc_Tc_c::cutDoorClose2Start(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    tc_seStart(this, JA_SE_OBJ_DOOR_N_CLOSE_2, 0);
}

/* 022E9CF0 */
void daNpc_Tc_c::cutDoorClose2Proc(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfGp_evmng_cutEnd(i_staffIdx);
}

/* 022E93A8 */
void daNpc_Tc_c::cutPayStart(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfGp_setItemRupeeCount(-398);
}

/* 022E9D28 */
void daNpc_Tc_c::cutPayProc(int i_staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfGp_evmng_cutEnd(i_staffIdx);
}

/* ---- monument checks (d_a_npc_tc_msg_red.inc) ---- */

/* 022E972C */
bool daNpc_Tc_c::checkMonumentFee(u16 i_stageNo, u16 i_eventId) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (tc_isStageTbox(i_stageNo, 0xF)) {
        return dComIfGs_isEventBit(i_eventId) != 0;
    }
    return false;
}

/* 022E979C */
bool daNpc_Tc_c::checkAllMonumentFee() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (checkMonumentFee(STAGE_DRC, UNK_1240) && checkMonumentFee(STAGE_FW, UNK_1D08) &&
        checkMonumentFee(STAGE_TOTG, UNK_1D04) && checkMonumentFee(STAGE_WT, UNK_1D02) &&
        checkMonumentFee(STAGE_ET, UNK_1D01)) {
        return true;
    }
    return false;
}

/* 022E95A8 */
int daNpc_Tc_c::checkAllMonumentPay() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    int amountOfMonumentsPaid = 0;
    if (checkMonumentPay(STAGE_DRC, UNK_1240))
        amountOfMonumentsPaid++;
    if (checkMonumentPay(STAGE_FW, UNK_1D08))
        amountOfMonumentsPaid++;
    if (checkMonumentPay(STAGE_TOTG, UNK_1D04))
        amountOfMonumentsPaid++;
    if (checkMonumentPay(STAGE_WT, UNK_1D02))
        amountOfMonumentsPaid++;
    if (checkMonumentPay(STAGE_ET, UNK_1D01))
        amountOfMonumentsPaid++;
    return amountOfMonumentsPaid;
}

/* 022E9508 */
bool daNpc_Tc_c::checkMonumentPay(u16 i_stageNo, u16 i_eventId) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (dComIfGs_isEventBit(i_eventId)) {
        return false;
    }
    if (tc_isStageTbox(i_stageNo, 0xF)) {
        dComIfGs_onEventBit(i_eventId);
        return true;
    }
    return false;
}

/* 022E9668 */
int daNpc_Tc_c::analysisCollectMap() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    for (int i = 1; i <= 8; i++) {
        if (!dComIfGs_isCollectMapTriforce(i) && dComIfGs_isGetCollectMap(i)) {
            dComIfGs_onCollectMapTriforce(i);
            return i;
        }
    }
    return 0;
}

// clang-format off
/* WWHD glue:
data l_cutHIO 104687F4
data action_table 101C6810
data l_staffName 10022A78
data l_itemType 10022A20
data l_animationName 10022A08
data l_getAnimation 10022A04
data l_animationNameProc 10022A6C
data l_assertFile 10022A2C
data l_assertItem 10022A44
ext 02520864 BOOL tc_isStageTbox(s32, s32)
ext 025D7DEC u32 tc_createItemForPresentDemo(cXyz*, s32, u8, s32, s32, csXyz*, cXyz*)
ext 022ED850 u8 tc_calcTimer(be<s8>*)
ext 022E7CE4 void daNpc_Tc_c::smoke_set(f32, f32, f32, f32, f32)
*/
