/**
 * Medli event bindings and private WWHD layout projections.
 * HD: message-manager, morph and full GHS event frames preserve verified HD behavior.
 * Shared actor layout stays in d_a_npc_md.h.
 */
#pragma once
#include "d/actor/d_a_npc_md.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/f_op/f_op_msg.h"
#include "gc_api/d/d_com_inf_game.h"
s16 mdEvent_calcTimer(be<s16>* timer);
s32 mdEvent_getRndValue(s32 start, s32 range);
void mdEvent_monsSeStart(u32 id, cXyz* pos, u32 pid, s32 reverb);
bool mdEvent_checkEndTactMusic(fopAc_ac_c* player);
void mdEvent_strcpy(u32 dst, u32 src);
s16 mdEvent_stickAngle(s32 port);
bool mdEvent_trigA(s32 port);
BOOL mdEvent_trigB(s32 port);
s16 mdEvent_cameraAngleY(u32 camera);
u32 mdEvent_lockonList(dAttention_c* attention, s32 index);
u32 mdEvent_actionList(dAttention_c* attention, s32 index);
bool mdEvent_lockonTruth(dAttention_c* attention);
fopAc_ac_c* mdEvent_listActor(u32 list);
f32 mdEvent_addCalc(be<f32>* value, f32 target, f32 scale, f32 maxStep, f32 minStep);
s32 mdEvent_specialCode(dBgS* bgs, u32 poly);
bool mdEvent_presentEnd(dEvent_manager_c* manager);
u16 mdEvent_messageStatus(u32 manager);
void mdEvent_setMessageStatus(u32 manager, u32 status);
u32 mdEvent_messageSet(u32 manager, u32 number, cXyz* pos);
s16 mdEvent_priority(void* actor);
void mdEvent_drawQueue(u32 tag, s16 priority);
void mdEvent_nextStage(const char* stage, s16 point, s8 room, s8 layer, f32 speed, u32 mode, s32 setPoint, s8 wipe);
BOOL mdEvent_startQuake(dVibration_c* vibration, s32 mode, s32 id, cXyz* pos);
BOOL mdEvent_stopQuake(dVibration_c* vibration, s32 id);
u32 mdEvent_actName(dEvent_manager_c* manager, s32 staff);
void mdEvent_lookAtTarget(dNpc_JntCtrl_c* joint, be<s16>* angle, cXyz* target, cXyz* eye, s16 rotation, s16 velocity, bool headOnly);
u32 mdEvent_nextMessageStatus(daNpc_Md_c* actor, be<u32>* number);
void mdEvent_setMorf2(mDoExt_McaMorf2* morf, f32 frame);
void mdEvent_setMorf(mDoExt_McaMorf* morf, f32 frame);
u32 mdEvent_option(u32 option);
void mdEvent_groundCheckCtor(u32 check);
void mdEvent_checkDtor(u32 check, s32 flag);
u32 mdEvent_runEventName(dEvent_manager_c* manager);
s32 mdEvent_checkInitialRoom(daNpc_Md_c* actor);
struct MdEventLookActor : daNpc_Md_c { void lookBack(cXyz*, int, int); };
WWHD_SIZE(MdEventLookActor,0x43DC);

namespace MdEvent {}

namespace MdEvent {
struct Emitter {
    u8 padding0[0x254];
    /* 0x254 */ be<u32> mFlags;
};
WWHD_OFFSET(Emitter, mFlags, 0x254);
WWHD_SIZE(Emitter, 0x258);
struct MessageManager {
    u8 padding0[0x921];
    /* 0x921 */ be<u8> mSendOn;
    u8 padding922[0x26];
    /* 0x948 */ be<s32> mSelectNum;
};
WWHD_OFFSET(MessageManager, mSendOn, 0x921);
WWHD_OFFSET(MessageManager, mSelectNum, 0x948);
WWHD_SIZE(MessageManager, 0x94C);
struct SaveInfo {
    u8 padding0[0x1C0];
    /* 0x1C0 */ be<u8> mClearCount;
    u8 padding1C1[0x483];
    /* 0x644 */ be<u8> mEvent;
    u8 padding645[0xC7B];
    /* 0x12C0 */ be<u8> mOption;
};
WWHD_OFFSET(SaveInfo, mClearCount, 0x1C0);
WWHD_OFFSET(SaveInfo, mEvent, 0x644);
WWHD_OFFSET(SaveInfo, mOption, 0x12C0);
WWHD_SIZE(SaveInfo, 0x12C1);
struct Play {
    u8 padding0[0x5292];
    /* 0x5292 */ be<u8> mEventMode;
    u8 padding5293[0x865];
    /* 0x5AF8 */ be<u32> mpCamera;
    u8 padding5AFC[0xC9];
    /* 0x5BC5 */ be<u8> mMesgAnimeAttrInfo;
    u8 padding5BC6[0xD];
    /* 0x5BD3 */ be<u8> mMesgCancelButton;
};
WWHD_OFFSET(Play, mEventMode, 0x5292);
WWHD_OFFSET(Play, mpCamera, 0x5AF8);
WWHD_OFFSET(Play, mMesgAnimeAttrInfo, 0x5BC5);
WWHD_OFFSET(Play, mMesgCancelButton, 0x5BD3);
WWHD_SIZE(Play, 0x5BD4);
struct Link {
    u8 padding0[0x3B8];
    /* 0x3B8 */ be<u32> mNoResetFlg0;
    /* 0x3BC */ be<u32> mNoResetFlg1;
    u8 padding3C0[0x6230];
    /* 0x65F0 */ be<s32> mCurProc;
};
WWHD_OFFSET(Link, mNoResetFlg0, 0x3B8);
WWHD_OFFSET(Link, mNoResetFlg1, 0x3BC);
WWHD_OFFSET(Link, mCurProc, 0x65F0);
WWHD_SIZE(Link, 0x65F4);
struct Attention {
    u8 padding0[0x20];
    /* 0x020 */ be<u32> mFlags;
};
WWHD_OFFSET(Attention, mFlags, 0x20);
WWHD_SIZE(Attention, 0x24);
struct Morf2 {
    u8 padding0[0xA4];
    /* 0x0A4 */ be<f32> mPlaySpeed;
    /* 0x0A8 */ be<f32> mFrame;
    u8 paddingAC[0x2];
    /* 0x0AE */ be<s16> mEndFrame;
    /* 0x0B0 */ be<s16> mLoopFrame;
    u8 paddingB2[0xE];
    /* 0x0C0 */ be<f32> mAnmRate;
};
WWHD_OFFSET(Morf2, mPlaySpeed, 0xA4);
WWHD_OFFSET(Morf2, mFrame, 0xA8);
WWHD_OFFSET(Morf2, mEndFrame, 0xAE);
WWHD_OFFSET(Morf2, mLoopFrame, 0xB0);
WWHD_OFFSET(Morf2, mAnmRate, 0xC0);
WWHD_SIZE(Morf2, 0xC4);
struct Morf {
    u8 padding0[0x98];
    /* 0x098 */ be<f32> mPlaySpeed;
    /* 0x09C */ be<f32> mFrame;
    u8 paddingA0[0x2];
    /* 0x0A2 */ be<s16> mEndFrame;
    /* 0x0A4 */ be<s16> mLoopFrame;
};
WWHD_OFFSET(Morf, mPlaySpeed, 0x98);
WWHD_OFFSET(Morf, mFrame, 0x9C);
WWHD_OFFSET(Morf, mEndFrame, 0xA2);
WWHD_OFFSET(Morf, mLoopFrame, 0xA4);
WWHD_SIZE(Morf, 0xA6);
struct SavePointer { gptr<SaveInfo> mpInfo; }; WWHD_SIZE(SavePointer,4);
struct MessagePointer { gptr<MessageManager> mpManager; }; WWHD_SIZE(MessagePointer,4);
struct NamePointer { be<u32> mpName; }; WWHD_SIZE(NamePointer,4);
struct HarpAnimationTable { be<s32> values[6]; }; WWHD_SIZE(HarpAnimationTable,24);
struct MessageTable { be<u32> values[3]; }; WWHD_SIZE(MessageTable,12);
struct ActionWords { union { gabi::ptmf<BOOL(daNpc_Md_c::*)(void*)> fn; be<u32> words[2]; }; }; WWHD_SIZE(ActionWords,8);
inline void copyAction(ProcFunc_l* out,const ActionWords& action) { auto* dst=reinterpret_cast<ActionWords*>(out); dst->words[0]=action.words[0]; dst->words[1]=action.words[1]; }
inline be<u32>& word(be<f32>& value) { return *reinterpret_cast<be<u32>*>(&value); }
inline const be<u32>& word(const be<f32>& value) { return *reinterpret_cast<const be<u32>*>(&value); }
inline Emitter* emitter(JPABaseEmitter* p) { return reinterpret_cast<Emitter*>(p); }
inline Morf2* morf(mDoExt_McaMorf2* p) { return reinterpret_cast<Morf2*>(p); }
inline Morf* morf(mDoExt_McaMorf* p) { return reinterpret_cast<Morf*>(p); }
inline Link* link(fopAc_ac_c* p) { return reinterpret_cast<Link*>(p); }
inline Attention* attention(dAttention_c* p) { return reinterpret_cast<Attention*>(p); }
inline Play* play() { return gabi::at<Play>(dComIfGp_ea()); }
inline MessageManager* message(u32 address) { return gabi::at<MessageManager>(address); }
inline be<u8>* bytes(u32 address) { return gabi::at<be<u8>>(address); }
inline void toEnd(be<f32>& frame, const be<s16>& end) { s16 value=end; s16 converted=(s16)gabi::ftoi((f32)value); frame=(f32)converted; }
inline void loopAtFrame(be<s16>& loop,const be<f32>& frame) { loop=(s16)gabi::ftoi(frame); }
enum : u32 { WAIT_NPC_ACTION=0x0228EB88, SEARCH_NPC_ACTION=0x02292C48, CARRY_NPC_ACTION=0x022904DC };
}

namespace MdEvent {
struct TurnTargetWords { be<u32> words[3]; }; WWHD_SIZE(TurnTargetWords,12);
struct PlayerVtable { u8 padding0[0x2C]; be<u32> getTactMusic; u8 padding30[0xE4]; be<u32> setPlayerPosAndAngle; };
WWHD_OFFSET(PlayerVtable,getTactMusic,0x2C); WWHD_OFFSET(PlayerVtable,setPlayerPosAndAngle,0x114); WWHD_SIZE(PlayerVtable,0x118);
struct SafeStringVtable { u8 padding0[0x14]; be<u32> assureTermination; }; WWHD_OFFSET(SafeStringVtable,assureTermination,0x14); WWHD_SIZE(SafeStringVtable,0x18);
inline s32 tactMusic(fopAc_ac_c* actor) { return gabi::call_ptr<s32>(gabi::at<PlayerVtable>(actor->__vtbl)->getTactMusic,actor); }
inline void setPlayerPosition(fopAc_ac_c* actor,cXyz* pos,s16 angle) { gabi::call_ptr(gabi::at<PlayerVtable>(actor->__vtbl)->setPlayerPosAndAngle,actor,pos,angle); }
inline void assureTermination(SafeString* str) { gabi::call_ptr(gabi::at<SafeStringVtable>(str->__vtbl)->assureTermination,str); }
inline const char* nameAt(u32 address) { return gabi::at<const char>(address); }
inline be<u8>* passFlags(dBgS_GndChk* check) { return &check->mbObjThrough; }
enum : u32 { SAFE_STRING_VTABLE=0x1001D6A0, GROUND_VTABLE=0x1001D718, POLY_INFO_VTABLE=0x1001D728, POLY_PASS_VTABLE=0x1001D748, GROUP_PASS_VTABLE=0x1001D738, GROUP_PASS_BASE_VTABLE=0x1001D708, ARRIVAL_GND_NAME=0x1001DE64 };
}

namespace MdEvent {
enum MessageStatus { MSG_TYPING=6, MSG_DISPLAYED=0xE, MSG_DESTROYED=0x13, INPUT=0x15 };
enum ParticleId { ID_IT_SN_MD_HANE00=0x819B, ID_IT_SN_MD_HANE02=0x8217, ID_IT_SN_MD_HANE03=0x827D };
enum SoundId { JA_SE_CV_MD_LANDING=0x48A7 };
}

namespace MdEvent {
// HD: complete GHS frames preserve strcpy's verified overwrites of neighboring locals.
struct OnetimeFrame { u8 padding0[8]; u8 acStack_18[16]; u8 padding18[8]; };
WWHD_OFFSET(OnetimeFrame,acStack_18,8); WWHD_SIZE(OnetimeFrame,0x20);
struct QuakeFrame { u8 padding0[0x10]; cXyz quake; u8 acStack_1c[12]; };
WWHD_OFFSET(QuakeFrame,quake,0x10); WWHD_OFFSET(QuakeFrame,acStack_1c,0x1C); WWHD_SIZE(QuakeFrame,0x28);
struct TurnFrame { u8 padding0[0x18]; TurnTargetWords local_20; u8 padding24[4]; };
WWHD_OFFSET(TurnFrame,local_20,0x18); WWHD_SIZE(TurnFrame,0x28);
struct MoveFrame { u8 padding0[0x10]; u8 acStack_ac[12]; SafeString arrival; u8 padding24[12]; cXyz pos; SafeString eventName; dBgS_GndChk local_a0; u8 padding98[0x30]; };
WWHD_OFFSET(MoveFrame,acStack_ac,0x10); WWHD_OFFSET(MoveFrame,arrival,0x1C); WWHD_OFFSET(MoveFrame,pos,0x30); WWHD_OFFSET(MoveFrame,eventName,0x3C); WWHD_OFFSET(MoveFrame,local_a0,0x44); WWHD_SIZE(MoveFrame,0xC8);
}

namespace MdEvent { enum PlayerFlag : u32 { daPyFlg0_NO_DRAW=0x08000000 }; }

namespace MdEvent {
enum EventFlag {
    MEDLI_GAVE_FATHERS_LETTER=0x0E02,
    UNK_1101=0x1101,
    UNK_1102=0x1102,
    UNK_1104=0x1104,
    UNK_1280=0x1280,
    UNK_1402=0x1402,
    UNK_1504=0x1504,
    UNK_1608=0x1608,
    UNK_1620=0x1620,
    UNK_2C08=0x2C08,
    UNK_2E40=0x2E40,
    UNK_3B80=0x3B80,
};
enum PlayerProc { daPyProc_GRAB_UP_e=0x6F, daPyProc_GRAB_THROW_e=0x71, daPyProc_GRAB_PUT_e=0x72 };
enum AttentionFlag : u32 { AttnFlag_20000000=0x20000000 };
}
