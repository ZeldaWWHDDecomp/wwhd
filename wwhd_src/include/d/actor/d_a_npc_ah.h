// Old Man Ho Ho: GameCube member names with the verified WWHD layout.
// HD: NPC base grows to 0x7DC, shifting mPhs through mBtpAnm by 0x118.
// HD: mDoExt_btpAnm grows to 0x74 and mShadowId is removed; later fields shift by 0x174.
// HD: size 0x8C8; virtual dispatch uses the actor vtable at 0xB4.
#pragma once
#include "gc_api/d/d_npc.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"

struct sAhAnmDat {
    /* 0x00 */ be<u8> mBckIdx; /* 0xFF: end */
    /* 0x01 */ be<u8> mMorf;
    /* 0x02 */ be<s8> mLoopCount;  /* was field_0x02: > 0: play n times, then the next entry; sAhAnmDat: setAnmTbl 021E56E4: the entry's count, copied into mAnmTimer; above 0 plays it that often, else loops (tww the same); same code as sPhotoAnmDat::mLoopCount */
};
WWHD_SIZE(sAhAnmDat, 3);

/* l_npc_dat (.data 101BB378) */
struct NpcDatStruct {
    /* 0x00 */ be<s16> mMax_head_x;
    /* 0x02 */ be<s16> mMax_head_y;
    /* 0x04 */ be<s16> mMax_backbone_x;
    /* 0x06 */ be<s16> mMax_backbone_y;
    /* 0x08 */ be<s16> mMin_head_x;
    /* 0x0A */ be<s16> mMin_head_y;
    /* 0x0C */ be<s16> mMin_backbone_x;
    /* 0x0E */ be<s16> mMin_backbone_y;
    /* 0x10 */ be<s16> mMax_turn_step;
    /* 0x12 */ be<s16> field_0x12;
    /* 0x14 */ be<f32> mPlayerEyeOffsetY;  /* was field_0x14: dNpc_playerEyePos offset; chkAttention 021E5BE0: passed to dNpc_playerEyePos, the look target above the player's head (tww the same); same role as NpcDatStruct::mPlayerEyeOffsetY (photo, roten) */
    /* 0x18 */ be<f32> mAttnOffsetX;  /* was field_0x18: attention position (x, y, z); _execute 021E688C: attention_info.position = current.pos + the vector (it, mAttnOffsetY, mAttnOffsetZ) Y-rotated by current.angle.y (tww the same); same role as daNpcPhoto_c NpcDatStruct::mAttnOffsetX */
    /* 0x1C */ be<f32> mAttnOffsetY;  /* was field_0x1C: _execute 021E688C: the Y of the attention position offset (see mAttnOffsetX) (tww the same); same role as daNpcPhoto_c NpcDatStruct::mAttnOffsetY */
    /* 0x20 */ be<f32> mAttnOffsetZ;  /* was field_0x20: _execute 021E688C: the Z of the attention position offset (see mAttnOffsetX) (tww the same); same role as daNpcPhoto_c NpcDatStruct::mAttnOffsetZ */
    /* 0x24 */ be<f32> mEyeYOffset;  /* was field_0x24: eye height; _execute 021E688C: eyePos = current.pos + (0, it, 0) (tww the same); same role as NpcDatStruct::mEyeYOffset (photo, roten) */
    /* 0x28 */ be<f32> mMaxAttnDistXZ;  /* was field_0x28: talk distance; createInit 021E5920 copies it into mAttnDistXZ, chkAttention 021E5BE0's attention distance (tww the same); same role as NpcDatStruct::mMaxAttnDistXZ (photo, roten) */
    /* 0x2C */ be<f32> mLookDistXZ;  /* was field_0x2C: look distance; chkAttention 021E5BE0: outside the attention, the player is still looked at while closer than it (tww the same); same role as MtNpcDat::mLookDistXZ */
    /* 0x30 */ be<s16> mMaxAttnAngleY;  /* was field_0x30: talk angle; createInit 021E5920 copies it into mAttnAngle, chkAttention 021E5BE0's attention angle (tww the same); same role as NpcDatStruct::mMaxAttnAngleY (photo, roten) */
    /* 0x32 */ be<s16> mMaxHeadTurnVel;  /* was field_0x32: look speed; chkAttention 021E5BE0: mTargetAngle, the speed lookBack 021E6680 eases the look toward (tww the same); same role as NpcDatStruct::mMaxHeadTurnVel (photo, roten) */
    /* 0x34 */ be<s16> field_0x34;
    /* 0x36 */ be<s16> field_0x36;
    /* 0x38 */ be<f32> mCylRadius;  /* was field_0x38: cylinder radius; createInit 021E5920 / _execute 021E688C: setCollision(it, 150), the body cylinder's radius (tww the same); same role as daNpcRoten_c NpcDatStruct::mCylRadius */
    /* 0x3C */ be<f32> field_0x3C;
    /* 0x40 */ be<f32> field_0x40;
    /* 0x44 */ be<s16> field_0x44;
    /* 0x46 */ be<s16> field_0x46;
    /* 0x48 */ be<s16> field_0x48;
    /* 0x4A */ be<s16> field_0x4A;
    /* 0x4C */ be<s16> field_0x4C;
    /* 0x4E */ be<s16> field_0x4E;
    /* 0x50 */ be<s16> mTurnBackTime;  /* was field_0x50: look timer; chkAttention 021E5BE0 loads mTurnBackTimer with it when the player leaves the attention (tww the same); same role as MtNpcDat::mTurnBackTime */
    /* 0x52 */ be<s8> mBodyTurn;  /* was field_0x52: createInit 021E5920 copies it into daNpcAh_c::mBodyTurn; chkAttention 021E5BE0 turns the body while it is set (mHeadOnlyFollow 0), the head only while 0; lookBack 021E6680 turns the body while talking with it set (tww the same); same role as daNpcPhoto_c NpcDatStruct::mBodyTurn */
    /* 0x53 */ be<s8> mLookAtPlayer;  /* was field_0x53: createInit 021E5920 copies it into daNpcAh_c::mLookAtPlayer; chkAttention 021E5BE0 turns back home after the look while 0 (tww the same); same role as daNpcPhoto_c NpcDatStruct::mLookAtPlayer */
};
WWHD_SIZE(NpcDatStruct, 0x54);

enum { dRes_ID_AH_BDL_AH_e = 2, dRes_ID_AH_BTP_MABA_e = 4 };
struct daNpcAh_c : fopNpc_npc_c {
    enum Prm_e { PRM_SWSAVE_S = 8, PRM_SWSAVE_W = 0, PRM_SWSAVE2_S = 8, PRM_SWSAVE2_W = 8 };
    cPhs_State _create();
    BOOL createHeap();
    cPhs_State createInit();
    BOOL _delete();
    BOOL _draw();
    BOOL _execute();
    u8 executeCommon();
    void executeSetMode(u8);
    BOOL executeWaitInit();
    void executeWait();
    BOOL executeTalkInit();
    void executeTalk();
    void checkOrder();
    void eventOrder();
    void eventMove();
    void privateCut();
    void eventMesSetInit(int);
    BOOL eventMesSet(); /* bool */
    void eventGetItemInit();
    u16 talk2(int);
    u16 next_msgStatus(be<u32>*);
    u32 getMsg();
    void setMessage(u32);
    void setAnmFromMsgTag();
    u8 getPrmArg0();
    u8 getSwBit();
    void setMtx();
    void chkAttention();
    void lookBack();
    BOOL initTexPatternAnm(u32); /* bool, passed on unnormalised */
    void playTexPatternAnm();
    void playAnm();
    void setAnm(u8, int, f32);
    BOOL setAnmTbl(sAhAnmDat*); /* bool */
    void setCollision(dCcD_Cyl*, cXyz*, f32, f32); /* cXyz by value: pointer to a copy */

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ request_of_phase_process_class mPhsMethod;
    /* 0x7EC */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0x7F0 */ mDoExt_btpAnm mBtpAnm; /* mDoExt_btpAnm (HD 0x74); HD: no mShadowId after it */
    /* 0x864 */ dNpc_PathRun_c mPathRun;
    /* 0x86C */ cXyz field_0x6F4;
    /* 0x878 */ cXyz mEyePos;
    /* 0x884 */ gptr<sAhAnmDat> mpAnmDat;
    /* 0x888 */ be<u32> mpMsgNo; /* u32* */
    /* 0x88C */ be<f32> field_0x718;
    /* 0x890 */ be<f32> mNextMorf;  /* was field_0x71C: setAnm 021E55D8: when it is >= 0 it replaces the blend (morf) of the next animation and is reset to -1 (tww f32 tempMorf = it; it = -1); Ah_actorCtor 021E5398 -1; same code as daNpcMt_c::mNextMorf (npcs.md) */
    /* 0x894 */ be<f32> mAttnDistXZ;  /* was field_0x720: createInit 021E5920: l_npc_dat mMaxAttnDistXZ (tww); chkAttention 021E5BE0: the player counts as in attention while closer than it (+40 once in attention) (tww f32 temp720 = it; if (temp720 > distXZ && ..)) */
    /* 0x898 */ be<u32> mItemNo;
    /* 0x89C */ be<u8> mHeadOnlyFollow;
    /* 0x89D */ u8 _89D;
    /* 0x89E */ be<s16> field_0x72A;
    /* 0x8A0 */ be<s16> field_0x72C;
    /* 0x8A2 */ be<s16> mTurnBackTimer;  /* was field_0x72E: chkAttention 021E5BE0: l_npc_dat mTurnBackTime when the player leaves the attention, counted down, and at 0 the NPC turns back to mHomeAngleY (tww it = l_npc_dat.field_0x50; if (it != 0) it--; else { mTargetYRot = m732; .. }) */
    /* 0x8A4 */ be<s16> mAttnAngle;  /* was field_0x730: createInit 021E5920: l_npc_dat mMaxAttnAngleY (tww); chkAttention 021E5BE0: the player counts as in attention while the angle to them is below it (+0x71C once in attention) (tww s32 temp730 = it; .. temp730 > abs(angle)) */
    /* 0x8A6 */ be<s16> mHomeAngleY;  /* was field_0x732: Ah_actorCtor 021E5398: home.angle.y (tww it = home.angle.y); chkAttention 021E5BE0 turns back to it (tww mTargetYRot = it); same code as daNpcMt_c::mHomeAngleY (npcs.md) */
    /* 0x8A8 */ be<u16> mMsgStatus;  /* was field_0x734: talk2 021E6088: the message status, 0xFFFF on a new message, then the status that next_msgStatus returned (tww it = -1; if (it == fopMsgStts_MSG_CONTINUES_e) ..; it = msg_status); same code as daNpcMt_c::mMsgStatus (npcs.md) */
    /* 0x8AA */ be<s16> field_0x736;
    /* 0x8AC */ be<s16> field_0x738;
    /* 0x8AE */ be<s16> mTimer;
    /* 0x8B0 */ be<s16> mTargetAngle;
    /* 0x8B2 */ be<s16> mLookAtMaxVel;
    /* 0x8B4 */ be<s16> mTargetYRot;
    /* 0x8B6 */ be<u8> mTalking;  /* was field_0x742: checkOrder 021E5F38 sets 1 on the talk command (tww); executeTalk 021E6CAC clears it; executeCommon 021E6BC0 returns it and enters the talk mode while it is 1 (tww); lookBack 021E6680 tests it (tww); same code as daNpcMt_c::mTalking (npcs.md) */
    /* 0x8B7 */ be<u8> mInAttention;  /* was field_0x743: chkAttention 021E5BE0: 1 once the player comes into the attention range, 0 when they leave (tww); executeCommon 021E6BC0 sets mOrderState 1 while it is set, else 0 (tww); createInit 021E5920 0; same code as daNpcMt_c::mInAttention (npcs.md) */
    /* 0x8B8 */ be<u8> mOrderState;  /* was field_0x744: executeCommon 021E6BC0 1 / 0 from mInAttention; eventMesSet 021E623C 3 (tww); checkOrder 021E5F38 starts the talk for 1 / 2 and eventOrder 021E64E0 orders by it (tww); same code as daNpcMt_c::mOrderState (npcs.md) */
    /* 0x8B9 */ be<u8> mBtpFrame;
    /* 0x8BA */ be<u8> mMoveState;
    /* 0x8BB */ be<u8> mResFlag;  /* was field_0x747: TWW accessor setResFlag(u8 flag) { field_0x747 = flag; }: Ah_actorCtor 021E5398 setResFlag(0), phase_1 021E549C setResFlag(1); _delete 021E5B78 deletes the resource only while it is set (tww) */
    /* 0x8BC */ be<u8> mItemFlags;  /* was field_0x748: eventMesSet 021E623C: when the box closes, bit 0x1 gives item 7 and bit 0x2 item 5 (mItemNo, mOrderState 3), each bit cleared once used (tww the same); never set in WWHD or tww */
    /* 0x8BD */ be<u8> mBckIdx;
    /* 0x8BE */ be<u8> mAnmFlags;  /* was field_0x74A: playAnm 021E65B0: clears bit 0x1 each call and sets it when the animation table moves on to its next entry (setAnmTbl returns true) (tww the same); never read in WWHD or tww */
    /* 0x8BF */ be<s8> mAnmTimer;  /* was field_0x74B: setAnmTbl 021E56E4: mpAnmDat->mLoopCount (tww it = mpAnmDat->field_0x02); playAnm 021E65B0 counts it down and acts at 0 (tww if (it > 0) { it += -1; if (it == 0) .. }); same code as daNpcMt_c::mAnmTimer (npcs.md) */
    /* 0x8C0 */ be<s8> mActIdx;
    /* 0x8C1 */ be<u8> field_0x74D;
    /* 0x8C2 */ be<s8> mLookMode;  /* was field_0x74E: chkAttention 021E5BE0: 1 looks at mEyePos, 2 faces mTargetYRot, 0 neither (tww); lookBack 021E6680 switches on it (tww); Ah_actorCtor 021E5398 0; same code as daNpcMt_c::mLookMode (npcs.md) */
    /* 0x8C3 */ be<u8> mBodyTurn;  /* was field_0x74F: createInit 021E5920 copies l_npc_dat mBodyTurn; chkAttention 021E5BE0 turns the body (mHeadOnlyFollow 0, m_jnt.mbTrn 1) while set, else the head only; lookBack 021E6680 turns the body while talking with it set (tww the same); same role as daNpcPeople_c::mBodyTurn */
    /* 0x8C4 */ be<u8> mLookAtPlayer;  /* was field_0x750: createInit 021E5920 copies l_npc_dat mLookAtPlayer; chkAttention 021E5BE0: while 0 the player in range only turns him to mHomeAngleY (mLookMode 2) instead of being looked at (tww the same); same role as daNpcPeople_c::mLookAtPlayer */
    /* 0x8C5 */ be<u8> field_0x751;
    /* 0x8C6 */ u8 _8C6[2];
};
WWHD_OFFSET(daNpcAh_c, mBtpAnm, 0x7F0);
WWHD_OFFSET(daNpcAh_c, mPathRun, 0x864);
WWHD_OFFSET(daNpcAh_c, mEyePos, 0x878);
WWHD_OFFSET(daNpcAh_c, mHeadOnlyFollow, 0x89C);
WWHD_OFFSET(daNpcAh_c, mTimer, 0x8AE);
WWHD_OFFSET(daNpcAh_c, mBckIdx, 0x8BD);
WWHD_OFFSET(daNpcAh_c, field_0x751, 0x8C5);
WWHD_SIZE(daNpcAh_c, 0x8C8);

// HD: full-width animation modify argument and message-manager ABI are retained.
void Ah_npcCtor(void*);
cPhs_State Ah_phaseHandler(request_of_phase_process_class*, u32, void*);
s32 Ah_createPresent(cXyz*, s32, u8, s32, s32, csXyz*, cXyz*);
void Ah_setMessageStatus(u32, u32);
BOOL Ah_initBtp(void*, u32, J3DAnmTexPattern*, s32, s32, f32, s16, s16, s32, s32);
u32 Ah_messageStatus(u32);
u32 Ah_messageSet(u32, u32, cXyz*);
void Ah_circleDt(void*, s32);
void Ah_acchDt(void*, s32);
void Ah_actorDt(void*, s32);
struct AhActorVtable {
    u8 _00[0x14];
    be<u32> mNextMessage;
    u8 _18[4];
    be<u32> mGetMessage;
    u8 _20[4];
    be<u32> mAnimationAttribute;
};
WWHD_OFFSET(AhActorVtable, mNextMessage, 0x14);
WWHD_OFFSET(AhActorVtable, mGetMessage, 0x1C);
WWHD_OFFSET(AhActorVtable, mAnimationAttribute, 0x24);
struct AhModelDataAnimation { u8 _00[0x38]; be<u32> mTexPatternAnimation; };
WWHD_OFFSET(AhModelDataAnimation, mTexPatternAnimation, 0x38);
struct AhAcchDestructor { u8 _00[0x14]; be<u32> mCheckVtable; u8 _18[8]; be<u32> mActorCheckVtable; };
WWHD_OFFSET(AhAcchDestructor, mCheckVtable, 0x14);
WWHD_OFFSET(AhAcchDestructor, mActorCheckVtable, 0x20);
template<class T, class U> inline T* Ah_view(U* p) { return gabi::at<T>(gabi::ea(p)); }
inline be<u32>& Ah_word(void* p) { return *Ah_view<be<u32>>(p); }
inline be<u32>* Ah_messagePointer(u32 address) { return gabi::at<be<u32>>(address); }
inline AhActorVtable* Ah_vtable(fopNpc_npc_c* actor) { return gabi::at<AhActorVtable>(actor->__vtbl); }
inline u32 Ah_getMessage(fopNpc_npc_c* actor) { return gabi::call_ptr<u32>(Ah_vtable(actor)->mGetMessage, actor); }
inline u32 Ah_nextMessage(fopNpc_npc_c* actor, be<u32>* message) { return gabi::call_ptr<u32>(Ah_vtable(actor)->mNextMessage, actor, message); }
inline void Ah_animationAttribute(fopNpc_npc_c* actor, u16 status) { gabi::call_ptr(Ah_vtable(actor)->mAnimationAttribute, actor, status); }

s16 Ah_calcTimer(be<s16>* timer);

void Ah_lookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8);

struct AhAnimationVtable { u8 _00[0x14]; be<u32> mGetFrameMax; };
struct AhAnimation { u8 _00[4]; gptr<AhAnimationVtable> mVtable; };
WWHD_OFFSET(AhAnimation, mVtable, 4);
WWHD_OFFSET(AhAnimationVtable, mGetFrameMax, 0x14);
inline s32 Ah_frameMax(J3DAnmTexPattern* pattern) { return gabi::call_ptr<s32>(Ah_view<AhAnimation>(pattern)->mVtable->mGetFrameMax, pattern); }
void Ah_cutEnd(dEvent_manager_c*, s32);
BOOL Ah_advance(dEvent_manager_c*, s32);

WWHD_SIZE(AhActorVtable, 0x28);
WWHD_SIZE(AhModelDataAnimation, 0x3C);
WWHD_SIZE(AhAcchDestructor, 0x24);
