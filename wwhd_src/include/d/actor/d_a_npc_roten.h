/* daNpcRoten_c (Traveling Merchants), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the allocating constructor and the functions of d_a_npc_roten:
 * - fopNpc_npc_c is 0x7DC (d/d_npc.h), so members are +0x118 up to mBtpAnm;
 * - mDoExt_btpAnm grew from 0x14 to 0x74 (+0x60) and mShadowId (GameCube 0x6F4) is gone (HD
 *   shadows, -4): everything from mItemPid on is +0x174.
 * Size 0xB40 (GameCube 0x9CC). */
#pragma once
#include "gc_api/gc_api.h"

struct sRotenAnmDat {
    /* 0x00 */ be<u8> mAnmIdx;  /* was field_0x00: setAnmTbl 022D44D0: the entry's animation index for setAnm (0xFF ends the table); playAnm 022D5174 replays it (tww the same) */
    /* 0x01 */ be<u8> mMorf;  /* was field_0x01: setAnmTbl 022D44D0: the entry's morf frames, passed to setAnm (tww the same) */
    /* 0x02 */ be<u8> mLoopCount;  /* was field_0x02: setAnmTbl 022D44D0: the entry's count, copied into mAnmLoopCount; 0 or less loops (tww the same) */
};
WWHD_SIZE(sRotenAnmDat, 3);

/* dNpc_PathRun_c (8 bytes) */
struct dNpc_PathRun_r {
    /* 0x00 */ gptr<dPath> mPath;
    /* 0x04 */ be<u8> mCurrPointIndex;
    /* 0x05 */ be<u8> mIdx;
    /* 0x06 */ be<u8> mbDir;
    /* 0x07 */ be<u8> field_0x07;
    bool isPath() { return mPath.get() != nullptr; }
    u32 getDir() { return mbDir != 0; }
    void turnDir() { mbDir = mbDir ^ 1; }
};
WWHD_SIZE(dNpc_PathRun_r, 8);

// Merchant tuning data, three entries.
struct NpcDatStruct {
    /* 0x00 */ be<f32> mPlayerEyeOffsetY;  /* was field_0x00: chkAttention 022D3B9C: passed to dNpc_playerEyePos, whose look target is the player's head top + (0, it, 0) (tww dNpc_playerEyePos(f32 offsetY)) */
    /* 0x04 */ be<s16> mMaxHeadX;  /* was field_0x04: _execute 022D5580: m_jnt.setParam's maxHeadX (d_npc.h setParam order; the same layout as dNpc_HIO_c mMaxHeadX) */
    /* 0x06 */ be<s16> mMaxBackboneX;  /* was field_0x06: _execute 022D5580: m_jnt.setParam's maxBackboneX (0 while mNoBackboneTurn) */
    /* 0x08 */ be<s16> mMaxHeadY;  /* was field_0x08: _execute 022D5580: m_jnt.setParam's maxHeadY */
    /* 0x0A */ be<s16> mMaxBackboneY;  /* was field_0x0A: _execute 022D5580: m_jnt.setParam's maxBackboneY (0 while mNoBackboneTurn) */
    /* 0x0C */ be<s16> mMinHeadX;  /* was field_0x0C: _execute 022D5580: m_jnt.setParam's minHeadX */
    /* 0x0E */ be<s16> mMinBackboneX;  /* was field_0x0E: _execute 022D5580: m_jnt.setParam's minBackboneX (0 while mNoBackboneTurn) */
    /* 0x10 */ be<s16> mMinHeadY;  /* was field_0x10: _execute 022D5580: m_jnt.setParam's minHeadY */
    /* 0x12 */ be<s16> mMinBackboneY;  /* was field_0x12: _execute 022D5580: m_jnt.setParam's minBackboneY (0 while mNoBackboneTurn) */
    /* 0x14 */ be<s16> mMaxTurnStep;  /* was field_0x14: _execute 022D5580: m_jnt.setParam's maxTurnStep */
    /* 0x16 */ be<s16> mMaxHeadTurnVel;  /* was field_0x16: chkAttention 022D3B9C: mLookMaxVelTarget, the speed lookBack eases the look toward (dNpc_HIO_c mMaxHeadTurnVel at the same offset) */
    /* 0x18 */ be<s16> mWalkHeadTurnVel;  /* was field_0x18: executeWalk 022D5C54: mLookMaxVelTarget while walking the path */
    /* 0x1A */ be<s16> field_0x1A;
    /* 0x1C */ be<f32> mAttnYOffset;  /* was field_0x1C: _execute 022D5580: attention_info.position = current.pos + (0, it, 0) (tww the same) */
    /* 0x20 */ be<s16> mMaxAttnAngleY;  /* was field_0x20: chkAttention 022D3B9C: the player is in attention while the angle to them is below it (+0x71C while mInAttention) */
    /* 0x22 */ u8 _22[2];
    /* 0x24 */ be<f32> mMaxAttnDistXZ;  /* was field_0x24: chkAttention 022D3B9C: the player is in attention while closer than it (+40 while mInAttention) */
    /* 0x28 */ be<f32> mEyeYOffset;  /* was field_0x28: _execute 022D5580: eyePos = current.pos + (0, it, 0) (tww the same) */
    /* 0x2C */ be<f32> mCylRadius;  /* was field_0x2C: createInit 022D3710 / _execute 022D5580: setCollision(it, 200), the body cylinder's radius (tww the same) */
    /* 0x30 */ be<f32> mCyl2Radius;  /* was field_0x30: setCollisionB 022D35C0: radius of mCyl2 */
    /* 0x34 */ be<f32> mCyl2Dist;  /* was field_0x34: setCollisionB 022D35C0: mCyl2 sits at the joint position pushed it along the angle from the actor */
    /* 0x38 */ be<f32> mSphRadius;  /* was field_0x38: setCollisionH 022D5238: radius of the head sphere mSph */
    /* 0x3C */ be<f32> mSphOffsetX;  /* was field_0x3C: setCollisionH 022D5238: mSph sits at the head joint matrix times (it, 0, 0) */
    /* 0x40 */ be<f32> field_0x40;
    /* 0x44 */ be<f32> mWalkSpeed;  /* was field_0x44: executeWalk 022D5C54: mSpeedF while walking the path */
    /* 0x48 */ be<s16> field_0x48;
    /* 0x4A */ be<s16> field_0x4A;
    /* 0x4C */ be<s16> mWaitTimeMin;  /* was field_0x4C: executeWaitInit 022D5970: mWaitTimer = it + rnd(mWaitTimeMax - it) */
    /* 0x4E */ be<s16> mWaitTimeMax;  /* was field_0x4E: executeWaitInit 022D5970: mWaitTimer = mWaitTimeMin + rnd(it - mWaitTimeMin) */
    /* 0x50 */ be<s16> mWalkTimeMin;  /* was field_0x50: executeWalkInit 022D5B7C / executeTurnInit 022D5DC0: mWaitTimer = it + rnd(mWalkTimeMax - it) */
    /* 0x52 */ be<s16> mWalkTimeMax;  /* was field_0x52: executeWalkInit 022D5B7C / executeTurnInit 022D5DC0: mWaitTimer = mWalkTimeMin + rnd(it - mWalkTimeMin) */
};
WWHD_SIZE(NpcDatStruct, 0x54);

struct daNpcRoten_c : fopNpc_npc_c {
    enum TradeItem {
        dItemNo_TOWN_FLOWER_e = 0x8C,
        dItemNo_SHOP_GURU_STATUE_e = 0x97,
        dItemNo_XXX_039_e = 0xA2
    };
    enum ItemButton { dItemBtn_X_e, dItemBtn_Y_e, dItemBtn_Z_e, dItemBtn_NONE_e };
    enum TalkButton { dTalkBtn_X_e = 1, dTalkBtn_Y_e, dTalkBtn_Z_e };
    enum Mode { MODE_WAIT, MODE_TALK, MODE_WALK, MODE_TURN, MODE_WIND };
    enum Animation {
        ANM_WAIT,
        ANM_TALK,
        ANM_WALK,
        ANM_TALK2,
        ANM_TALK3,
        ANM_TALK4,
        ANM_TALK5,
        ANM_WATASU,
        ANM_ITEMWAIT,
        ANM_WIND
    };
    enum Cut {
        CUT_INIT,
        CUT_MES_SET,
        CUT_SET_ITEM,
        CUT_CLR_ITEM,
        CUT_GET_ITEM,
        CUT_SET_ANGLE,
        CUT_ON_PLR,
        CUT_OFF_PLR
    };
    enum Prm_e {
        PRM_RAIL_ID_W = 0x08,
        PRM_RAIL_ID_S = 0x10,
        PRM_NPC_NO_W = 0x08,
        PRM_NPC_NO_S = 0x18,
    };

    cPhs_State _create();
    BOOL createHeap();
    cPhs_State createInit();
    bool _delete();
    bool _draw();
    bool _execute();
    u8 executeCommon(); /* returns mTalking as stored */
    void executeSetMode(u8);
    s32 executeWaitInit();
    void executeWait();
    s32 executeTalkInit();
    void executeTalk();
    s32 executeWalkInit();
    void executeWalk();
    s32 executeTurnInit();
    void executeTurn();
    s32 executeWindInit();
    void executeWind();
    void checkOrder();
    void eventOrder();
    void eventMove();
    void privateCut();
    void eventMesSetInit(int);
    bool eventMesSet();
    void eventSetItemInit();
    bool eventSetItem();
    void eventClrItemInit();
    void eventGetItemInit(int);
    void eventSetAngleInit();
    void eventOnPlrInit();
    void eventOffPlrInit();
    u16 next_msgStatus(be<u32>*);
    u32 getMsg();
    void setMessage(u32);
    void setAnmFromMsgTag();
    u8 getPrmNpcNo();
    u8 getPrmRailID();
    void setMtx();
    void chkAttention();
    void lookBack();
    BOOL initTexPatternAnm(u8); /* bool, passed on unnormalised */
    void playTexPatternAnm();
    void playAnm();
    void setAnm(u8, int, f32);
    bool setAnmTbl(sRotenAnmDat*);
    BOOL isHaitatuItem(u8);
    BOOL isKoukanItem(u8);
    BOOL isGetMap(u8);
    s16 XyEventCB(int);
    void setCollisionB();
    void setCollisionH();

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ request_of_phase_process_class mPhs2;
    /* 0x7EC */ gptr<J3DModel> mpHeadModel;  /* was field_0x6D4: createHeap 022D2CCC: the head morf's model; _draw 022D5790 sets its light / TEV colors */
    /* 0x7F0 */ gptr<mDoExt_McaMorf> mpHeadMorf;  /* was field_0x6D8: createHeap 022D2CCC creates it (mDoExt_McaMorf) from the head model data and head animation; executeWindInit 022D5FC8 / execute play it */
    /* 0x7F4 */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0x7F8 */ mDoExt_btpAnm mBtpAnm; // HD: the texture animator grows; mShadowId is absent.
    /* 0x86C */ be<u32> mItemPid;  /* was field_0x6F8: eventSetItemInit 022D4330 stores the created item's process ID; privateCut 022D4828 / eventClrItem 022D45B4 find the item with it (SearchByID) */
    /* 0x870 */ be<u32> field_0x6FC;
    /* 0x874 */ dNpc_PathRun_r mPathRun;
    /* 0x87C */ cXyz mLookPos;  /* was field_0x708: chkAttention 022D3B9C copies the event cut's attention position; lookBack 022D5374 passes it to lookAtTarget as the target */
    /* 0x888 */ cXyz mHandItemPos;  /* was field_0x714: privateCut 022D4828: during ANM_WATASU (frame 60+) the left-hand joint position + (20, -30, -30), where the present item is shown; during ANM_ITEMWAIT copied into mLookPos */
    /* 0x894 */ cXyz mWindVec;  /* was field_0x720: no use in this decomp; TWW getWindVec() { return field_0x720; } (CC0 zeldaret/tww d217e2b) */
    /* 0x8A0 */ dCcD_Cyl mCyl2;
    /* 0x9D0 */ dCcD_Sph mSph;
    /* 0xAFC */ gptr<sRotenAnmDat> mpAnmTbl;  /* was field_0x988: setAnmTbl 022D44D0 stores the table; playAnm 022D5174 steps to the next entry */
    /* 0xB00 */ gptr<be<u32>> mpMsgList;  /* was field_0x98C: eventMesSetInit 022D40A4 points it at a message number list; next_msgStatus 022D60DC / getMsg 022D6304 step through it */
    /* 0xB04 */ be<f32> mSpeedF;  /* was field_0x990: _execute 022D5580 sets speedF to it; executeWalk 022D5C54 sets the walk speed, executeSetMode 022D3FDC clears it */
    /* 0xB08 */ be<f32> mMorf;  /* was field_0x994: setAnm 022D43B0 uses it as the morf frames when not negative; the constructor 022D30F4 sets -1 */
    /* 0xB0C */ be<s32> mAnmPlayMode;  /* was field_0x998: setAnm 022D43B0 stores the play mode it passes to mpMorf->setAnm (setAnmTbl 022D44D0: 0 for counted entries, 2 for looping ones); setAnmTbl restarts an unchanged animation only while it is 0 (tww the same) */
    /* 0xB10 */ be<u8> mLookHeadOnly;  /* was field_0x99C: lookBack 022D5374 passes it to lookAtTarget as the head-only flag (d_npc.h lookAtTarget headOnly) unless talking with mBodyTurn; chkAttention 022D3B9C sets 1 / 0 (0 = turn the body), executeWalk 022D5C54 / executeTurn 022D5F10 clear it; the constructor 022D30F4 sets 1 (tww the same) */
    /* 0xB11 */ u8 _B11;
    /* 0xB12 */ be<s16> mExchange1stEventIdx;  /* was field_0x99E: createInit 022D3710: dComIfGp_evmng_getEventIdx("ROTEN_EXCHANGE_1ST"); XyEventCB 022D3388 returns it */
    /* 0xB14 */ be<s16> mExchange2ndEventIdx;  /* was field_0x9A0: createInit 022D3710: dComIfGp_evmng_getEventIdx("ROTEN_EXCHANGE_2ND"); checkOrder 022D3EA0 / eventMove 022D4C40 test it */
    /* 0xB16 */ be<s16> mChangeItemEventIdx;  /* was field_0x9A2: createInit 022D3710: dComIfGp_evmng_getEventIdx("ROTEN_CHANGE_ITEM"); checkOrder 022D3EA0 / eventMove 022D4C40 test it */
    /* 0xB18 */ be<s16> mGetMapEventIdx;  /* was field_0x9A4: createInit 022D3710: dComIfGp_evmng_getEventIdx("ROTEN_GET_MAP"); XyEventCB 022D3388 returns it */
    /* 0xB1A */ be<s16> mWaitTimer;  /* was field_0x9A6: executeWaitInit 022D5970 sets npc dat 0x4C + random; executeWait 022D5A48 counts it down before walking the path */
    /* 0xB1C */ be<s16> mHomeTurnDelay;  /* was field_0x9A8: chkAttention 022D3B9C sets 30 when the player leaves the attention range and, without a path, counts it down before turning back to home.angle.y (tww the same) */
    /* 0xB1E */ be<s16> mBlinkTimer;  /* was field_0x9AA: initTexPatternAnm 022D2BB8 clears it; playTexPatternAnm 022D50D4 counts it down and restarts the head texture pattern */
    /* 0xB20 */ be<s16> mLookMaxVelTarget;  /* was field_0x9AC: lookBack 022D5374 sets npc dat 0x16 (executeWalk 022D5C54: 0x18) and eases mLookMaxVel toward it */
    /* 0xB22 */ be<s16> mLookMaxVel;  /* was field_0x9AE: lookBack 022D5374 eases it toward mLookMaxVelTarget (cLib_addCalcAngleS2 4, 0x800), else clears it, and passes it to lookAtTarget as maxVel */
    /* 0xB24 */ be<s16> mLookAngleY;  /* was field_0x9B0: lookBack 022D5374: the target angle in look mode 2; executeWalk 022D5C54 / executeTurn 022D5F10 set the path angle, chkAttention 022D3B9C home.angle.y (tww the same) */
    /* 0xB26 */ be<u16> mEventFlags;  /* was field_0x9B2: bit field: eventOrder 022D4EF0 sets 0x4000 when it orders an event and eventMove 022D4C40 waits for its end while set; XyEventCB 022D3388 sets 0xC000 (during a talk, _execute 022D5580 runs eventMove instead of the mode while 0x8000 is set); next_msgStatus 022D60DC sets 8 / 0x10 / 0x20 / 0x40, which getMsg 022D6304 and eventMove turn into the follow-up messages and events (tww the same) */
    /* 0xB28 */ be<u8> mTalking;  /* was field_0x9B4: checkOrder 022D3EA0 sets 1 when the talk starts; executeCommon 022D58F4 switches to MODE_TALK while 1 and returns it; executeTalk 022D5ABC / eventMove 022D4C40 clear it; chkAttention 022D3B9C widens the angle while set (tww the same) */
    /* 0xB29 */ be<u8> mInAttention;  /* was field_0x9B5: chkAttention 022D3B9C: 1 once the player is in the look range (distance +40 and angle +0x71C while set), 0 when they leave; executeWalk 022D5C54 stops and executeWait 022D5A48 does not start walking while set (tww the same); same pattern as daNpcMt_c::mInAttention */
    /* 0xB2A */ be<u8> mCanTalk;  /* was field_0x9B6: chkAttention 022D3B9C: 1 while the player is in the talk range (or the event attention is on), else 0; executeCommon 022D58F4 sets mOrderEvent 1 (can talk) while it is set outside MODE_WIND (tww the same) */
    /* 0xB2B */ be<u8> mOrderEvent;  /* was field_0x9B7: eventOrder 022D4EF0 orders by it: 1 / 2 the talk (2 orders the speak event), 3-6 the ROTEN_EXCHANGE_1ST / _2ND / CHANGE_ITEM / GET_MAP events; checkOrder 022D3EA0 clears 3-6 once the event starts; executeCommon 022D58F4 / eventMove 022D4C40 set it (tww the same); after daNpc_Cb1_c::mOrderEvent */
    /* 0xB2C */ be<u8> mBtpFrame;  /* was field_0x9B8: initTexPatternAnm 022D2BB8 clears it; playTexPatternAnm 022D50D4 advances it; _draw 022D5790 enters the btp animation at it */
    /* 0xB2D */ be<s8> m_hand_L_jnt_num;
    /* 0xB2E */ be<s8> m_bag_jnt_num;
    /* 0xB2F */ be<u8> mExecuteMode;  /* was field_0x9BB: _execute 022D5580 calls moveProc[it]; executeSetMode 022D3FDC sets it; executeCommon 022D58F4 tests MODE_WIND */
    /* 0xB30 */ be<u8> mResFlag;  /* was field_0x9BC: phase_1 022D3234 sets 1 once the resources are loaded (GameCube setResFlag(1)) */
    /* 0xB31 */ be<u8> mNpcNo;
    /* 0xB32 */ be<u8> mTradeItemIdx;  /* was field_0x9BE: getMsg 022D6304 / XyEventCB 022D3388: the shown item's number minus dItemNo_TOWN_FLOWER; eventMesSetInit 022D40A4 / eventSetItemInit 022D4330 / eventGetItemInit 022D4620 / next_msgStatus 022D60DC index the exchange messages and items by it (tww the same) */
    /* 0xB33 */ be<u8> field_0x9BF;
    /* 0xB34 */ be<u8> mAnmIdx;  /* was field_0x9C0: setAnm 022D43B0 stores the animation index (l_bck_ix_tbl); createHeap 022D2CCC loads the bck by it; setAnmTbl 022D44D0 / privateCut 022D4828 (ANM_WATASU, ANM_ITEMWAIT) test it (tww the same) */
    /* 0xB35 */ be<u8> mAnmTblFlags;  /* was field_0x9C1: bit 1: playAnm 022D5174 sets it when the animation table reaches its end; setAnmTbl 022D44D0 clears it; executeWind 022D6074 goes back to MODE_WAIT once it is set (tww the same) */
    /* 0xB36 */ be<s8> mAnmLoopCount;  /* was field_0x9C2: setAnmTbl 022D44D0 sets the table entry's count; playAnm 022D5174 counts it down before the next entry */
    /* 0xB37 */ be<s8> mCutNo;  /* was field_0x9C3: privateCut 022D4828 stores the event cut index and switches on it */
    /* 0xB38 */ be<s8> mLookMode;  /* was field_0x9C4: lookBack 022D5374 switches on it: 1 looks at mLookPos, 2 turns to mLookAngleY, 0 neither; chkAttention 022D3B9C, executeWalk 022D5C54, executeTurn 022D5F10, privateCut 022D4828 set it (tww the same) */
    /* 0xB39 */ be<u8> mShownItemBtn;
    /* 0xB3A */ be<u8> mBodyTurn;  /* was field_0x9C6: checkOrder 022D3EA0 (talk start) and eventClrItemInit 022D45B4 set 1; executeTalk 022D5ABC / eventMove 022D4C40 / privateCut 022D4828 clear it; while set, chkAttention 022D3B9C and lookBack 022D5374 (when talking) turn the body (mLookHeadOnly 0, setTrn) (tww the same) */
    /* 0xB3B */ be<u8> mNoBackboneTurn;  /* was field_0x9C7: _execute 022D5580 passes 0 for the four backbone limits of m_jnt.setParam while set; privateCut 022D4828 sets 1 (ANM_ITEMWAIT), eventClrItemInit 022D45B4 clears it (tww the same) */
    /* 0xB3C */ be<u8> mDrawHead;  /* was field_0x9C8: _draw 022D5790 puts the head model on the head joint and draws it only while set; the constructor 022D30F4 sets 1, nothing clears it (tww the same) */
    /* 0xB3D */ be<u8> field_0x9C9;
    /* 0xB3E */ be<u8> mWindHit;  /* was field_0x9CA: setCollisionH 022D5238 sets it (and MODE_WIND) on the first TG hit of the head sphere; chkAttention 022D3B9C ignores the player while set; executeWind 022D6074 clears it when the wind animation table ends (tww the same) */
    /* 0xB3F */ u8 _B3F;
};
WWHD_OFFSET(daNpcRoten_c, mBtpAnm, 0x7F8);
WWHD_OFFSET(daNpcRoten_c, mItemPid, 0x86C);
WWHD_OFFSET(daNpcRoten_c, mCyl2, 0x8A0);
WWHD_OFFSET(daNpcRoten_c, mSph, 0x9D0);
WWHD_OFFSET(daNpcRoten_c, mpAnmTbl, 0xAFC);
WWHD_OFFSET(daNpcRoten_c, mEventFlags, 0xB26);
WWHD_OFFSET(daNpcRoten_c, mNpcNo, 0xB31);
WWHD_SIZE(daNpcRoten_c, 0xB40);

void roten_constructNpc(daNpcRoten_c* actor);
void roten_constructSphere(dCcD_Sph* sphere);
s32 roten_initBtp(mDoExt_btpAnm* animator, J3DModelData* model, J3DAnmTexPattern* animation,
                  s32 play, s32 mode, s32 start, s32 end, u32 modify, s32 entry, f32 speed);
s8 roten_getJointIndex(u32 table, const char* name);
cPhs_State roten_phaseHandler(request_of_phase_process_class* phase, void* handlers,
                              daNpcRoten_c* actor);
void roten_destroySphere(dCcD_Sph* sphere, s32 flags);
void roten_destroyCircle(void* circle, s32 flags);
void roten_destroyAcch(dBgS_ObjAcch* acch, s32 flags);
void roten_destroyActor(fopAc_ac_c* actor, s32 flags);
u32 roten_createPresentItem(cXyz* position, u32 itemNo, u32 flags, s32 bitNo, s32 room, u32 angle,
                            u32 scale);
s8 roten_getActionIndex(void* manager, s32 staff, u32 names, s32 count, BOOL force, s32 nameType);

struct RotenInitTable {
    gabi::ptmf<s32 (daNpcRoten_c::*)()> entries[5];
};
struct RotenMoveTable {
    gabi::ptmf<void (daNpcRoten_c::*)()> entries[5];
};
WWHD_SIZE(RotenInitTable, 40);
WWHD_SIZE(RotenMoveTable, 40);
extern RotenInitTable& l_execute_init;
extern RotenMoveTable& moveProc;
struct RotenCutNames {
    gptr<char> entries[8];
};
WWHD_SIZE(RotenCutNames, 32);
extern RotenCutNames& cut_name_tbl;
struct RotenPhaseHandlers {
    be<u32> phase1, phase2, end;
};
WWHD_SIZE(RotenPhaseHandlers, 12);
struct RotenAcchVtables {
    u8 _00[0x14];
    be<u32> dBgS_Chk_vtable;
    u8 _18[8];
    be<u32> cBgS_Chk_vtable;
};
WWHD_OFFSET(RotenAcchVtables, dBgS_Chk_vtable, 0x14);
WWHD_OFFSET(RotenAcchVtables, cBgS_Chk_vtable, 0x20);
WWHD_SIZE(RotenAcchVtables, 0x24);
struct RotenActorVtable {
    u8 _00[0x1C];
    be<u32> getMsg;
};
WWHD_OFFSET(RotenActorVtable, getMsg, 0x1C);
WWHD_SIZE(RotenActorVtable, 0x20);
struct RotenAnimationVtable {
    u8 _00[0x14];
    be<u32> getFrameMax;
};
WWHD_OFFSET(RotenAnimationVtable, getFrameMax, 0x14);
WWHD_SIZE(RotenAnimationVtable, 0x18);
struct RotenAnimationView {
    u8 _00[4];
    gptr<RotenAnimationVtable> vtable;
};
WWHD_OFFSET(RotenAnimationView, vtable, 4);
WWHD_SIZE(RotenAnimationView, 8);

// Private exact-width HD bindings.

static constexpr u32 l_actorVtable = 0x10021614;

// HD: both source parts refer to these single mapped table definitions in the main source.
struct SaveDatStruct {
    be<u16> mFirstTalkBit, mExchangeCountReg, field_0x04, mItemShownBit;  /* mFirstTalkBit was field_0x00: getMsg 022D6304: the event bit set with the first talk (l_msg_1st_talk); eventOrder 022D4EF0 allows the item talk (CANTALKITEM) once it is set (tww the same); mExchangeCountReg was field_0x02: the event register counting exchanges: next_msgStatus 022D60DC adds 1; getMsg 022D6304 / XyEventCB 022D3388 switch to the exchange-done messages at 3 (tww the same); mItemShownBit was field_0x06: getMsg 022D6304 / eventMesSetInit 022D40A4 set this event bit when a trade item is shown; getMsg then gives l_msg_exchange (tww the same) */
};
WWHD_SIZE(SaveDatStruct, 8);
struct RotenStringTable {
    gptr<const char> entries[3];
};
struct RotenNpcTable {
    NpcDatStruct entries[3];
};
struct RotenIndexTable {
    be<s32> entries[3];
};
struct RotenAnimationTable {
    be<s32> entries[3][10];
};
struct RotenSaveTable {
    SaveDatStruct entries[3];
};
struct RotenItemTable {
    be<u8> entries[3][12];
};
struct RotenWindAnmTable {
    sRotenAnmDat entries[3];
};
WWHD_SIZE(RotenStringTable, 12);
WWHD_SIZE(RotenNpcTable, 0xFC);
WWHD_SIZE(RotenIndexTable, 12);
WWHD_SIZE(RotenAnimationTable, 120);
WWHD_SIZE(RotenSaveTable, 24);
WWHD_SIZE(RotenItemTable, 36);
WWHD_SIZE(RotenWindAnmTable, 9);
extern RotenStringTable& l_arcname_tbl;
extern RotenStringTable& l_npc_staff_id;
extern RotenNpcTable& l_npc_dat;
extern RotenIndexTable& l_bmd_ix_tbl;
extern RotenIndexTable& l_head_bmd_ix_tbl;
extern RotenIndexTable& l_head_bck_ix_tbl;
extern RotenIndexTable& l_btp_ix_tbl;
extern RotenAnimationTable& l_bck_ix_tbl;
extern RotenSaveTable& l_save_dat;
extern RotenItemTable& l_item_dat;
extern sRotenAnmDat& l_npc_anm_wait;
extern sRotenAnmDat& l_npc_anm_walk;
extern RotenWindAnmTable& l_npc_anm_wind;
extern dCcD_SrcSph& l_sph_src;
extern dCcD_SrcCyl& dNpc_cyl_src;
static inline NpcDatStruct& npc_dat(u32 no) { return l_npc_dat.entries[no]; }
static inline const char* arcname(u32 no) { return l_arcname_tbl.entries[no].get(); }
static inline u16 save_dat(u32 no, u32 field) {
    const auto& dat = l_save_dat.entries[no];
    switch (field) {
    case 0:
        return dat.mFirstTalkBit;
    case 2:
        return dat.mExchangeCountReg;
    case 4:
        return dat.field_0x04;
    default:
        return dat.mItemShownBit;
    }
}
static inline u8 item_dat(u32 no, u32 idx) { return l_item_dat.entries[no][idx]; }

/* play object fields */
// HD: keep the event mode byte unnormalized.
static inline u8 roten_dComIfGp_event_runCheck() {
    return gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mEvtCtrl.mMode;
}
static inline s8 roten_dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force,
                                                  s32 nameType) {
    return roten_getActionIndex(dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}

/* actors */

BOOL roten_fopAcM_SearchByID_o(u32 id, gptr<fopAc_ac_c>* out);

static inline u32 roten_fopAcM_createItemForPresentDemo(cXyz* pos, u8 itemNo, u8 flag, s32 bitNo,
                                                        s32 roomNo) {
    return roten_createPresentItem(pos, itemNo, flag, bitNo, roomNo, 0, 0);
}

BOOL roten_fopAcM_orderChangeEventId(fopAc_ac_c* a, fopAc_ac_c* b, s16 ev, u16 flag, u16 hind);

void daItemBase_dead(fopAc_ac_c* a);
void daItemBase_show(fopAc_ac_c* a);

/* d_npc */

void roten_dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye,
                                     s16 yrot, s16 vel, u8 headOnly);
void roten_dNpc_PathRun_setInf(dNpc_PathRun_r* r, u8 id, s8 room, u8 b);
void roten_dNpc_PathRun_getPoint(dNpc_PathRun_r* r, cXyz* out, u8 idx);
void roten_dNpc_PathRun_incIdxLoop(dNpc_PathRun_r* r);
BOOL roten_dNpc_PathRun_chkPointPass(dNpc_PathRun_r* r, cXyz* pos, u32 dir);
BOOL roten_dNpc_PathRun_nextIdxAuto(dNpc_PathRun_r* r);

/* J3D (HD) */
/* J3DModel::getAnmMtx (HD: the matrix block's dirty flag is set on access) */
static inline Mtx34* getAnmMtx(J3DModel* model, s32 jntNo) { return model->getAnmMtx(jntNo); }
void roten_PSMTXConcat(Mtx34* a, Mtx34* b, Mtx34* ab);
static inline s32 J3DAnm_getFrameMax(J3DAnmTexPattern* animation) {
    auto* view = reinterpret_cast<RotenAnimationView*>(animation);
    return gabi::call_ptr<s32>(view->vtable->getFrameMax, animation);
}

struct RotenMessageNumbers {
    be<u32> entries[3];
};
struct RotenForceMessages {
    be<u32> entries[7];
};
struct RotenMapMessages {
    be<u32> entries[2];
};
struct RotenMessageTable {
    gptr<be<u32>> entries[3];
};
struct RotenExchangeMessages {
    gptr<be<u32>> entries[3][12];
};
struct RotenMessageAnimations {
    gptr<sRotenAnmDat> entries[9];
};
struct RotenItemNumber {
    be<u32> itemNo;
};
WWHD_SIZE(RotenMessageNumbers, 12);
WWHD_SIZE(RotenForceMessages, 28);
WWHD_SIZE(RotenMapMessages, 8);
WWHD_SIZE(RotenMessageTable, 12);
WWHD_SIZE(RotenExchangeMessages, 144);
WWHD_SIZE(RotenMessageAnimations, 36);
WWHD_SIZE(RotenItemNumber, 4);
// HD: model and message fields used only by this merchant translation unit.
struct RotenModelData : J3DModelData {
    u8 _14[0x38 - 0x14];
    be<u32> mpTexNoAnm;
};
WWHD_OFFSET(RotenModelData, mpTexNoAnm, 0x38);
WWHD_SIZE(RotenModelData, 0x3C);
struct RotenMessageManager {
    u8 _00[0x948];
    be<u32> mSelectNum;
};
WWHD_OFFSET(RotenMessageManager, mSelectNum, 0x948);
WWHD_SIZE(RotenMessageManager, 0x94C);
struct RotenCurrentModel {
    gptr<J3DModel> model;
};
struct RotenMessageManagerPtr {
    gptr<RotenMessageManager> manager;
};
struct RotenPresentItem : fopAc_ac_c {
    u8 _3AC[0x760 - 0x3AC];
    cXyz mOffsetPos;
};
WWHD_OFFSET(RotenPresentItem, mOffsetPos, 0x760);
WWHD_SIZE(RotenPresentItem, 0x76C);

extern RotenMessageNumbers& l_msg_xy_koukan_end;
extern RotenMessageNumbers& l_msg_xy_koukan_first;
extern RotenMessageNumbers& l_msg_xy_koukan_no;
extern RotenMessageNumbers& l_msg_xy_koukan_rupee;
extern RotenForceMessages& l_msg_try_force;
extern RotenMapMessages& l_msg_collect_map;
extern RotenMessageTable& l_msg_xy_exchange3;
extern RotenMessageTable& l_msg_xy_no_roten_item;
extern RotenMessageTable& l_msg_xy_invalid_item;
extern RotenMessageTable& l_msg_1st_talk;
extern RotenMessageTable& l_msg_exchange3;
extern RotenMessageTable& l_msg_exchange;
extern RotenMessageTable& l_msg_etc;
extern RotenExchangeMessages& l_msg_xy_koukan_item;
extern RotenExchangeMessages& l_msg_xy_koukan_item2;
extern RotenExchangeMessages& l_msg_xy_koukan_item3;
extern RotenMessageTable& l_msg_xy_after_get_demo;
extern RotenMessageTable& l_msg_xy_after_get_demo2;
extern RotenMessageAnimations& l_msg_anm;
extern RotenItemNumber& l_get_item_no;
extern RotenCurrentModel& l_currentModel;
extern Mtx34& l_currentMtx;
extern RotenMessageManagerPtr& l_messageManager;

static inline J3DModelData* roten_modelData(J3DModel* model) { return model->getModelData(); }
