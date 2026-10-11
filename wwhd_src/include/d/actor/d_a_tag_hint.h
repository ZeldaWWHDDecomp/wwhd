/* Hint trigger: GameCube member names, with the HD actor base offset delta. */
#pragma once
#include "bindings.h"
#include "d/actor/d_a_player.h"
#include "d/d_item.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor_mng.h"
namespace hint {
enum { fpcNm_Obj_Movebox_e = 0x2B };
}
struct daTag_Hint_c : fopAc_ac_c {
    /* 0x3AC */ be<u32> m290;
    /* 0x3B0 */ be<s8> m294;
    u8 pad[3];
    /* 0x3B4 */ be<u32> mMsgNo;
    /* 0x3B8 */ be<s32> m29C;
    /* 0x3BC */ be<s16> m2A0;
    /* 0x3BE */ be<u8> mCurProc;
    /* 0x3BF */ be<u8> m2A3;
    /* 0x3C0 */ be<s16> m2A4;
    /* 0x3C2 */ be<u16> m2A6;
    /* 0x3C4 */ be<f32> m2A8;
    /* 0x3C8 */ be<f32> m2AC;
    /* 0x3CC */ be<f32> m2B0;
    u8 getEventNo();
    u8 getSwbit();
    u8 getSwbit2();
    u8 getType();
    u8 getType2();
    u16 getEventFlag();
    s32 rangeCheck_local(cXyz*);
    s32 rangeCheck();
    s32 moveBoxCheck();
    s32 waitTerms();
    s32 arrivalTerms();
    void darkProc();
    s32 actionLight();
    u16 getMessage();
    u32 getMsg();
    void talkInit();
    s32 getPriority();
    s32 otherCheck();
    void makeEventId();
    void startProc();
    void setPlayerAngle();
    s32 actionHunt();
    s32 actionArrival();
    void initLight();
    void setLightPos();
    void makeLight();
    void deleteLight();
    u16 next_msgStatus(u32*);
    u16 talk();
    s32 actionEvent();
};
WWHD_OFFSET(daTag_Hint_c, m2A0, 0x3BC);
WWHD_OFFSET(daTag_Hint_c, m2B0, 0x3CC);
WWHD_SIZE(daTag_Hint_c, 0x3D0);
WWHD_OFFSET(daTag_Hint_c, m290, 0x3AC);
WWHD_OFFSET(daTag_Hint_c, m294, 0x3B0);
WWHD_OFFSET(daTag_Hint_c, mMsgNo, 0x3B4);
WWHD_OFFSET(daTag_Hint_c, m29C, 0x3B8);
WWHD_OFFSET(daTag_Hint_c, mCurProc, 0x3BE);
WWHD_OFFSET(daTag_Hint_c, m2A3, 0x3BF);
WWHD_OFFSET(daTag_Hint_c, m2A4, 0x3C0);
WWHD_OFFSET(daTag_Hint_c, m2A6, 0x3C2);
WWHD_OFFSET(daTag_Hint_c, m2A8, 0x3C4);
WWHD_OFFSET(daTag_Hint_c, m2AC, 0x3C8);

s32 hintGetEventIdx(dEvent_manager_c* manager, const char* name, u8 event);
s32 hintGetMyStaffId(dEvent_manager_c* manager, const char* name, u32 actor, u32 id);
s32 hintGetMyActIdx(dEvent_manager_c* manager, s32 staff, const void* names, s32 count, u32 force,
                    u32 prefix);
s32 hintGetIsAddvance(dEvent_manager_c* manager, s32 staff);
void hintCutEnd(dEvent_manager_c* manager, s32 staff);

void dKy_set_vrboxcol_ratio(f32 value);
void dKy_set_fogcol_ratio(f32 value);
void dKy_fog_startendz_set(f32 start, f32 end, f32 ratio);
void dKy_set_bgcol_ratio(f32 value);
void hintPlightPrioritySet(LIGHT_INFLUENCE* light);

void hintSeStart(u32 sound);
u32 hintMessageStatus(u32 control);
void hintSetMessageStatus(u32 control, u32 status);
u32 hintMessageSet(u32 control, u32 message, const void* position);
void hintRequest(void* attentionHint, daTag_Hint_c* actor, s32 priority);
void hintSetLightPosHD();
void hintMakeLightHD();

namespace hint {
enum MessageStatus {
    fopMsgStts_MSG_DISPLAYED_e = 14,
    fopMsgStts_MSG_CONTINUES_e = 15,
    fopMsgStts_MSG_ENDS_e = 16,
    fopMsgStts_BOX_CLOSED_e = 18,
    fopMsgStts_MSG_DESTROYED_e = 19
};
}

namespace hint {
// HD: preserve the 112-byte stack slot, including its four untouched tail bytes.
struct dBgS_CamLinChk : cBgS_LinChk, dBgS_Chk {
    u8 stackTail[4];
};
WWHD_SIZE(dBgS_CamLinChk, 0x70);
WWHD_OFFSET(dBgS_CamLinChk, mbCamThrough, 0x5D);
WWHD_OFFSET(dBgS_CamLinChk, mGrp, 0x68);
} // namespace hint
void hintConstructLineCheck(hint::dBgS_CamLinChk* check);
void hintSetLineCheck(hint::dBgS_CamLinChk* check, cXyz* start, cXyz* end, u32 actor);
s32 hintLineCross(dBgS* background, hint::dBgS_CamLinChk* check);
void hintDestroyLineCheck(hint::dBgS_CamLinChk* check, s32 flags);

void hintConstructActor(daTag_Hint_c* actor);

void hintConstructHeaderAngle(void* object);
void hintConstructHeaderFlag(void* object);

namespace hint {
struct PlayerVtable {
    u8 prefix[0xBC];
    be<u32> getGrabActorID;
    u8 reservedC0[4];
    be<u32> checkGrabBarrel;
    u8 reservedC8[0x1C];
    be<u32> voiceStart;
};
WWHD_OFFSET(PlayerVtable, getGrabActorID, 0xBC);
WWHD_OFFSET(PlayerVtable, checkGrabBarrel, 0xC4);
WWHD_OFFSET(PlayerVtable, voiceStart, 0xE4);
WWHD_SIZE(PlayerVtable, 0xE8);
struct SafeStringVtable {
    u8 prefix[0x14];
    be<u32> prepare;
};
WWHD_OFFSET(SafeStringVtable, prepare, 0x14);
WWHD_SIZE(SafeStringVtable, 0x18);
} // namespace hint

namespace hint {
constexpr u32 searchByIDJudge = 0x025E1234;
}

namespace hint {
enum OmamoriSound {
    JA_SE_ITM_OMAMORI_BLINK = 0x852,
    JA_SE_ITM_OMAMORI_ST_TALK = 0x853,
    JA_SE_ITM_OMAMORI_TETLA = 0x854,
    JA_SE_ITM_OMAMORI_ED_TALK = 0x855
};
}

namespace hint {
enum Action { ACTION_WAIT, ACTION_LIGHT, ACTION_ARRIVAL, ACTION_HUNT, ACTION_EVENT };
}
