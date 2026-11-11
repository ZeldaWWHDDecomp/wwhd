/* Carlov: GameCube names with the verified HD NPC base and animation expansion.
 * HD: the shadow ID is absent; the photo queue and message manager replace GC save/message forms.
 * Derived game source, local only. */
#pragma once
#include "gc_api/gc_api.h"
#include "gc_api/d/d_npc.h"

struct sMtAnmDat {
    be<u8> mAnmIdx;  /* was field_0x00: sMtAnmDat: setAnmTbl 022A02B0: the entry's animation number for setAnm (0xFF ends the table); playAnm 022A1638 replays it (tww the same); same code as sPhotoAnmDat::mAnmIdx */
    be<u8> mMorf;  /* was field_0x01: sMtAnmDat: setAnmTbl 022A02B0: the entry's morf frames, passed to setAnm (tww the same); same code as sPhotoAnmDat::mMorf */
    be<s8> mLoopCount;  /* was field_0x02: sMtAnmDat: setAnmTbl 022A02B0: the entry's count, copied into mAnmTimer; above 0 plays it that often, else loops (tww the same); same code as sPhotoAnmDat::mLoopCount */
};
WWHD_SIZE(sMtAnmDat, 3);

// HD: virtual entries retain GHS's four-byte slots and guest dispatch.
struct MtNpcVtableHD {
    u8 mPrefix[0x14];
    be<u32> next_msgStatus;
    be<u32> mReserved18;
    be<u32> getMsg;
    be<u32> mReserved20;
    be<u32> anmAtr;
};
WWHD_OFFSET(MtNpcVtableHD, next_msgStatus, 0x14);
WWHD_OFFSET(MtNpcVtableHD, getMsg, 0x1C);
WWHD_OFFSET(MtNpcVtableHD, anmAtr, 0x24);

struct MtMessageManagerHD {
    u16 getStatus16();
    u32 getStatus();
    void setStatus(u32 status);
    s32 messageSet(u32 number, cXyz* position);
};

struct daNpcMt_c : fopNpc_npc_c {
    u32 dispatchGetMsg() {
        auto* table = gabi::at<MtNpcVtableHD>(__vtbl);
        return gabi::call_ptr<u32>(table->getMsg, this);
    }
    u32 dispatchNextMsgStatus() {
        auto* table = gabi::at<MtNpcVtableHD>(__vtbl);
        return gabi::call_ptr<u32>(table->next_msgStatus, this, &mCurrMsgNo);
    }
    void dispatchAnmAtr(u16 status) {
        auto* table = gabi::at<MtNpcVtableHD>(__vtbl);
        gabi::call_ptr<void>(table->anmAtr, this, status);
    }
    u8 getPrmNpcNo();
    u8 isFigureGet(u8 figure);
    s32 getFigureMakeNum();
    BOOL isComp();
    void clearPhotoSelection();
    void setFigure(u8 figure);
    u8 getPhotoSelection();
    void setCollision(dCcD_Cyl* cylinder, cXyz* center, f32 radius, f32 height);
    s32 _create();
    u32 getPhoto(u32 index);
    void executeSetMode(u8 mode);
    BOOL chkEndEvent();
    void setMessage(u32 message);
    void eventGetItemInit();
    BOOL eventMesSet();
    void setAnmFromMsgTag();
    void eventMove();
    u32 executeCommon();
    s32 executeWaitInit();
    void executeWait();
    s32 executeTalkInit();
    void executeTalk();
    void submitPhoto(u32 photo);
    BOOL setAnmTbl(sMtAnmDat* table);
    s16 XyCheckCB(s32 index);
    void setMtx();
    void checkOrder();
    void eventMesSetInit(s32 staff);
    void eventOrder();
    void playTexPatternAnm();
    void playAnm();
    BOOL hasPhoto(u32 figure);
    BOOL initTexPatternAnm(u32 modify);
    void setAnm(u8 animation, s32 attr, f32 blend);
    u16 talk2(s32 init);
    void privateCut();
    BOOL _draw();
    u16 next_msgStatus(be<u32>* messageOut);
    void makeFigures();
    s32 createInit();
    void lookBack();
    BOOL _execute();
    BOOL createHeap();
    BOOL _delete();
    void chkAttention();
    u32 getMsg();

    /* 0x7DC */ request_of_phase_process_class mPhsArcname;
    /* 0x7E4 */ request_of_phase_process_class mPhs;
    /* 0x7EC */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0x7F0 */ mDoExt_btpAnm mBtpAnm;
    /* 0x864 */ be<u32> m6F0;
    /* 0x868 */ u8 m6F4[0x10];
    /* 0x878 */ cXyz mLookAtPos;
    /* 0x884 */ gptr<sMtAnmDat> mpAnmDat;
    /* 0x888 */ gptr<be<u32>> mpMsgTbl;
    /* 0x88C */ be<f32> m718;
    /* 0x890 */ be<f32> mNextMorf;  /* was m71C: setAnm 022A01A4: when it is >= 0 it replaces the blend (morf) of the next animation and is reset to -1 (tww if (it >= 0.0f) { morf = it; it = -1.0f; }); daNpcMt_construct 0229FD7C -1 */
    /* 0x894 */ be<f32> mAttnDistXZ;  /* was m720: createInit 022A0618: l_npc_dat mMaxAttnDistXZ; chkAttention 022A0B20: the player counts as in attention while closer than it (+40 once in attention) (tww the same); same code as daNpcAh_c::mAttnDistXZ */
    /* 0x898 */ be<u32> mItemNo;
    /* 0x89C */ be<u8> mHeadOnlyFollow;  /* was m728: lookBack 022A1708 passes it as the head-only flag of the joint look-at (tww bool headOnlyFollow = it; m_jnt.lookAtTarget(.., headOnlyFollow)); chkAttention 022A0B20 / faceHome set 0 or 1 (tww); daNpcMt_construct 0229FD7C 1 */
    /* 0x89D */ be<u8> m729;
    /* 0x89E */ be<s16> mMtGetItemEventIdx;
    /* 0x8A0 */ be<s16> m72C;
    /* 0x8A2 */ be<s16> mTurnBackTimer;  /* was m72E: chkAttention 022A0B20: l_npc_dat mTurnBackTime when the player leaves the attention, counted down while no message is set, and at 0 faceHome turns the NPC back home (tww the same); same code as daNpcAh_c::mTurnBackTimer */
    /* 0x8A4 */ be<s16> mAttnAngle;  /* was m730: createInit 022A0618: l_npc_dat mMaxAttnAngleY; chkAttention 022A0B20: the player counts as in attention while the angle to them is below it (+0x71C once in attention) (tww the same); same code as daNpcAh_c::mAttnAngle */
    /* 0x8A6 */ be<s16> mHomeAngleY;  /* was m732: daNpcMt_construct 0229FD7C: home.angle.y (tww it = home.angle.y); faceHome / chkAttention 022A0B20 turn back to it (tww m740 = it) */
    /* 0x8A8 */ be<u16> mMsgStatus;  /* was m734: talk2 022A10BC: the message status, 0xFFFF on a new message, then the status that next_msgStatus returned (tww it = -1; if (it == fopMsgStts_MSG_CONTINUES_e) ..; it = status); after daNpcPeople_c::mMsgStatus (npcs.md) */
    /* 0x8AA */ be<s16> m736;
    /* 0x8AC */ u8 m738[2];
    /* 0x8AE */ be<s16> mTimer;
    /* 0x8B0 */ be<s16> mTurnSpeedTarget;  /* was m73C: chkAttention 022A0B20: l_npc_dat mMaxHeadTurnVel (tww); lookBack 022A1708 eases mTurnSpeed to it unless the event cut gives a turn speed (tww short target = it; cLib_addCalcAngleS2(&m73E, target, 4, 0x800)) */
    /* 0x8B2 */ be<s16> mTurnSpeed;  /* was m73E: lookBack 022A1708: eased to mTurnSpeedTarget while turning, else 0, and passed as the look-at's maxVel (tww m_jnt.lookAtTarget(.., desiredYRot, it, ..); dNpc_JntCtrl_c::lookAtTarget(.., s16 defaultY, s16 maxVel, ..)); createInit 022A0618 0 */
    /* 0x8B4 */ be<s16> mDesiredYRot;  /* was m740: lookBack 022A1708: the look-at's default Y angle in look mode 2 (tww desiredYRot = it); faceHome / chkAttention 022A0B20 set it to mHomeAngleY (tww it = m732) */
    /* 0x8B6 */ be<u8> mTalking;  /* was m742: checkOrder 022A0E84 sets 1 on the talk command and enters the talk mode; executeTalk 022A1D7C clears it when the box closes; executeCommon 022A1C4C returns it and enters the talk mode while it is 1 (tww); lookBack 022A1708 turns the whole body while it and mBodyTurn are set (tww); createInit 022A0618 0 */
    /* 0x8B7 */ be<u8> mInAttention;  /* was m743: chkAttention 022A0B20: 1 once the player comes close, 0 when they leave (tww); executeCommon 022A1C4C sets mOrderState 1 while it is set, else 0 (tww); createInit 022A0618 0 */
    /* 0x8B8 */ be<u8> mOrderState;  /* was m744: executeCommon 022A1C4C 1 / 0 from mInAttention; eventMesSet 022A1270 3 when a message flag asks for the item event; checkOrder 022A0E84 starts the talk for 1 / 2 and clears 3 once the get-item event (mMtGetItemEventIdx) starts; eventOrder 022A1500 orders by it (tww) */
    /* 0x8B9 */ be<u8> mFrame;
    /* 0x8BA */ be<u8> mMoveProcIdx;  /* was m746: executeSetMode 022A0DFC: the value of l_execute_init[mode] (tww it = (this->*l_execute_init[param_1])()); _execute 022A1914 calls (this->*moveProc[it])() (tww); executeCommon 022A1C4C tests it; daNpcMt_construct 0229FD7C 0; same name as daObjFigure_c::mMoveProcIdx */
    /* 0x8BB */ be<u8> mResFlag;  /* was m747: tww accessor setResFlag(u8 flag) { mResFlag = flag; }: daNpcMt_construct 0229FD7C 0, phase_1 022A0018 1 once the resources are loaded (tww setResFlag(1)); tww demo _delete deletes the resource only while it is set; same name as daNpcAh_c / daNpcRoten_c::mResFlag */
    /* 0x8BC */ be<u8> mNpcNo;
    /* 0x8BD */ be<u8> m749;
    /* 0x8BE */ be<u8> mAnmIdx;  /* was m74A: setAnm 022A01A4 stores the animation number; createHeap 0229FABC loads l_bck_ix_tbl[it] (tww); executeWait 022A1D2C goes back to the wait animation while it is not 0; setAnmTbl 022A02B0 compares with it (tww it != mpAnmDat->field_0x00) */
    /* 0x8BF */ be<u8> mAnmFlag;
    /* 0x8C0 */ be<s8> mAnmTimer;  /* was m74C: setAnmTbl 022A02B0: mpAnmDat->mLoopCount (tww it = mpAnmDat->field_0x02); playAnm 022A1638 counts it down and acts at 0 (tww if (it > 0) { it--; if (it == 0) .. }) */
    /* 0x8C1 */ be<s8> mActIdx;  /* was m74D: privateCut 022A12FC: the event action index (tww it = dComIfGp_evmng_getMyActIdx(..), -1 ends, switch (it)); same name as the other actors' cut indices */
    /* 0x8C2 */ be<u8> m74E;
    /* 0x8C3 */ be<s8> mLookMode;  /* was m74F: lookBack 022A1708: 1 looks at mLookAtPos, 2 faces mDesiredYRot, 0 neither (tww switch (it)); chkAttention 022A0B20 / faceHome set it; daNpcMt_construct 0229FD7C 0 */
    /* 0x8C4 */ be<u8> mBodyTurn;  /* was m750: createInit 022A0618 copies l_npc_dat mBodyTurn; while set chkAttention 022A0B20 (event look) / its playerGaze turn the body (mHeadOnlyFollow 0, mbTrn 1) and lookBack 022A1708 turns the body while mTalking (tww the same); same role as daNpcPhoto_c / daNpcPeople_c::mBodyTurn */
    /* 0x8C5 */ be<u8> mLookAtPlayer;  /* was m751: createInit 022A0618 copies l_npc_dat mLookAtPlayer; playerGaze (chkAttention 022A0B20): while 0 the NPC turns back home (faceHome) right after looking at the player (tww if (mLookAtPlayer == 0) ..); same role as daNpcPhoto_c / daNpcPeople_c::mLookAtPlayer */
    /* 0x8C6 */ be<u8> m752;
    /* 0x8C7 */ be<u8> mAttnEnabled;  /* was m753: after tww attnOn() / attnOff() (mAttnEnabled = true / false); chkAttention 022A0B20: while set the player is looked at outside the attention range too; createInit 022A0618 0; daKnob00_c::actionFigure 021A43F8 sets 1, actionTalk 021A4F70 0 (tww the same); same name as the KnobMt view (d_a_knob00.h) */
    /* 0x8C8 */ u8 m754[4];
};
WWHD_OFFSET(daNpcMt_c, mPhsArcname, 0x7DC);
WWHD_OFFSET(daNpcMt_c, mPhs, 0x7E4);
WWHD_OFFSET(daNpcMt_c, m_head_tex_pattern, 0x7EC);
WWHD_OFFSET(daNpcMt_c, mBtpAnm, 0x7F0);
WWHD_OFFSET(daNpcMt_c, m6F0, 0x864);
WWHD_OFFSET(daNpcMt_c, m6F4, 0x868);
WWHD_OFFSET(daNpcMt_c, mLookAtPos, 0x878);
WWHD_OFFSET(daNpcMt_c, mpAnmDat, 0x884);
WWHD_OFFSET(daNpcMt_c, mpMsgTbl, 0x888);
WWHD_OFFSET(daNpcMt_c, m718, 0x88C);
WWHD_OFFSET(daNpcMt_c, mNextMorf, 0x890);
WWHD_OFFSET(daNpcMt_c, mAttnDistXZ, 0x894);
WWHD_OFFSET(daNpcMt_c, mItemNo, 0x898);
WWHD_OFFSET(daNpcMt_c, mHeadOnlyFollow, 0x89C);
WWHD_OFFSET(daNpcMt_c, m729, 0x89D);
WWHD_OFFSET(daNpcMt_c, mMtGetItemEventIdx, 0x89E);
WWHD_OFFSET(daNpcMt_c, m72C, 0x8A0);
WWHD_OFFSET(daNpcMt_c, mTurnBackTimer, 0x8A2);
WWHD_OFFSET(daNpcMt_c, mAttnAngle, 0x8A4);
WWHD_OFFSET(daNpcMt_c, mHomeAngleY, 0x8A6);
WWHD_OFFSET(daNpcMt_c, mMsgStatus, 0x8A8);
WWHD_OFFSET(daNpcMt_c, m736, 0x8AA);
WWHD_OFFSET(daNpcMt_c, m738, 0x8AC);
WWHD_OFFSET(daNpcMt_c, mTimer, 0x8AE);
WWHD_OFFSET(daNpcMt_c, mTurnSpeedTarget, 0x8B0);
WWHD_OFFSET(daNpcMt_c, mTurnSpeed, 0x8B2);
WWHD_OFFSET(daNpcMt_c, mDesiredYRot, 0x8B4);
WWHD_OFFSET(daNpcMt_c, mTalking, 0x8B6);
WWHD_OFFSET(daNpcMt_c, mInAttention, 0x8B7);
WWHD_OFFSET(daNpcMt_c, mOrderState, 0x8B8);
WWHD_OFFSET(daNpcMt_c, mFrame, 0x8B9);
WWHD_OFFSET(daNpcMt_c, mMoveProcIdx, 0x8BA);
WWHD_OFFSET(daNpcMt_c, mResFlag, 0x8BB);
WWHD_OFFSET(daNpcMt_c, mNpcNo, 0x8BC);
WWHD_OFFSET(daNpcMt_c, m749, 0x8BD);
WWHD_OFFSET(daNpcMt_c, mAnmIdx, 0x8BE);
WWHD_OFFSET(daNpcMt_c, mAnmFlag, 0x8BF);
WWHD_OFFSET(daNpcMt_c, mAnmTimer, 0x8C0);
WWHD_OFFSET(daNpcMt_c, mActIdx, 0x8C1);
WWHD_OFFSET(daNpcMt_c, m74E, 0x8C2);
WWHD_OFFSET(daNpcMt_c, mLookMode, 0x8C3);
WWHD_OFFSET(daNpcMt_c, mBodyTurn, 0x8C4);
WWHD_OFFSET(daNpcMt_c, mLookAtPlayer, 0x8C5);
WWHD_OFFSET(daNpcMt_c, m752, 0x8C6);
WWHD_OFFSET(daNpcMt_c, mAttnEnabled, 0x8C7);
WWHD_OFFSET(daNpcMt_c, m754, 0x8C8);
WWHD_SIZE(daNpcMt_c, 0x8CC);

// HD: GHS integer copies preserve signaling NaNs when saving gaze vectors.
inline u32 Mt_floatWord(const be<f32>& value) { return *reinterpret_cast<const be<u32>*>(&value); }
inline void Mt_storeFloatWord(be<f32>& value, u32 bits) { *reinterpret_cast<be<u32>*>(&value) = bits; }

// HD: a queue of submitted figures lives beside the expanded save object.
struct MtPhotoQueueHD {
    u8 mQueueState[0x10];
    be<u8> mNumPhotos;
    u32 get(u32 index);
    void set(u32 index, u32 photo);
    void setActive(u32 active);
};
struct MtPhotoAlbumHD {
    u8 mState;
};
struct MtPhotoSaveHD {
    u8 mSavePrefix[0x20];
    dSv_info_c mSaveInfo;
    MtPhotoAlbumHD mAlbum;
};
struct MtPictureAlbumHD {
    u8 mPictureState[0x3C030C];
    be<u8> mPictureCount;
};
WWHD_OFFSET(MtPhotoQueueHD, mNumPhotos, 0x10);
WWHD_SIZE(MtPhotoQueueHD, 0x11);
WWHD_OFFSET(MtPhotoSaveHD, mAlbum, 0x12C0);
WWHD_SIZE(MtPhotoSaveHD, 0x12C1);
WWHD_OFFSET(MtPictureAlbumHD, mPictureCount, 0x3C030C);
WWHD_SIZE(MtPictureAlbumHD, 0x3C030D);
MtPhotoQueueHD* MtPhotoAlbum_getQueue(MtPhotoAlbumHD*);
MtPictureAlbumHD* MtPhotoAlbum_getPictures(MtPhotoAlbumHD*);
void MtPictureAlbum_erase(MtPictureAlbumHD*, u8 index);

struct MtNpcDat {
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
    /* 0x14 */ be<f32> mPlayerEyeOffsetY;  /* was field_0x14: chkAttention 022A0B20: passed to dNpc_playerEyePos, the look target above the player's head (tww the same); same role as NpcDatStruct::mPlayerEyeOffsetY (photo, roten) */
    /* 0x18 */ be<f32> mAttnYOffset;  /* was field_0x18: _execute 022A1914: attention_info.position = current.pos + (0, it, 0) (tww the same); same role as daNpcRoten_c NpcDatStruct::mAttnYOffset */
    /* 0x1C */ be<f32> mEyeYOffset;  /* was field_0x1C: _execute 022A1914: eyePos = current.pos + (0, it, 0) (tww the same); same role as NpcDatStruct::mEyeYOffset (photo, roten) */
    /* 0x20 */ be<f32> mMaxAttnDistXZ;  /* was field_0x20: createInit 022A0618 copies it into mAttnDistXZ, chkAttention 022A0B20's attention distance (tww the same); same role as NpcDatStruct::mMaxAttnDistXZ (photo, roten) */
    /* 0x24 */ be<f32> mLookDistXZ;  /* was field_0x24: chkAttention 022A0B20: outside the attention, the player is still looked at while closer than it (sets m752) (tww the same) */
    /* 0x28 */ be<s16> mMaxAttnAngleY;  /* was field_0x28: createInit 022A0618 copies it into mAttnAngle, chkAttention 022A0B20's attention angle (tww the same); same role as NpcDatStruct::mMaxAttnAngleY (photo, roten) */
    /* 0x2A */ be<s16> mMaxHeadTurnVel;  /* was field_0x2A: chkAttention 022A0B20: mTurnSpeedTarget, the speed lookBack 022A1708 eases the look toward (tww the same); same role as NpcDatStruct::mMaxHeadTurnVel (photo, roten) */
    /* 0x2C */ be<s16> field_0x2C;
    /* 0x2E */ be<s16> field_0x2E;
    /* 0x30 */ be<f32> mCylRadius;  /* was field_0x30: createInit 022A0618 / _execute 022A1914: setCollision(it, 150), the body cylinder's radius (tww the same); same role as daNpcRoten_c NpcDatStruct::mCylRadius */
    /* 0x34 */ be<f32> field_0x34;
    /* 0x38 */ be<f32> field_0x38;
    /* 0x3C */ be<s16> field_0x3C;
    /* 0x3E */ be<s16> field_0x3E;
    /* 0x40 */ be<s16> field_0x40;
    /* 0x42 */ be<s16> field_0x42;
    /* 0x44 */ be<s16> field_0x44;
    /* 0x46 */ be<s16> field_0x46;
    /* 0x48 */ be<s16> mTurnBackTime;  /* was field_0x48: chkAttention 022A0B20 loads mTurnBackTimer with it when the player leaves the attention (tww the same) */
    /* 0x4A */ be<u8> mBodyTurn;  /* was field_0x4A: createInit 022A0618 copies it into daNpcMt_c::mBodyTurn; chkAttention 022A0B20 / its playerGaze turn the body while it is set (mHeadOnlyFollow 0), the head only while 0 (tww the same); same role as daNpcPhoto_c NpcDatStruct::mBodyTurn */
    /* 0x4B */ be<u8> mLookAtPlayer;  /* was field_0x4B: createInit 022A0618 copies it into daNpcMt_c::mLookAtPlayer; playerGaze (chkAttention 022A0B20) turns back home (faceHome) after the look while 0 (tww the same); same role as daNpcPhoto_c NpcDatStruct::mLookAtPlayer */
};
WWHD_SIZE(MtNpcDat, 0x4C);

// HD: this resource overload takes the full-width resource index.
void* Mt_getObjectResHD(dRes_control_c*, SafeString*, u32 resource);

s32 Mt_phaseHandlerHD(request_of_phase_process_class*, be<u32>*, daNpcMt_c*);

// HD: these cuts reuse a cached manager pointer.
void Mt_cutEndHD(dEvent_manager_c*, s32 staff);
BOOL Mt_getIsAddvanceHD(dEvent_manager_c*, s32 staff);

void Mt_offEventBitHD(dSv_event_c*, u16 bit);

// HD: preserve the callers' full-width photo/item inputs and narrow photo-table result.
u8 dSnap_PhotoIndex2TableIndexHD(u32 photo);
u32 dSnap_GetFigRoomIdHD(u32 figure);
u32 Mt_createItemForPresentDemoHD(cXyz*, u32 item, u32 flags, s32 bit, s32 room, csXyz*, cXyz*);

void Mt_assertHD(const char* file, u32 line, const char* condition);

// HD: the texture animation and SafeString interfaces dispatch through guest vtables.
struct MtPatternVtableHD {
    u8 mPrefix[0x14];
    be<u32> getFrameMax;
};
struct MtPatternAnimationHD {
    be<u32> mData;
    gptr<MtPatternVtableHD> mVtable;
};
inline s32 Mt_patternFrameMax(J3DAnmTexPattern* pattern) {
    auto* animation = reinterpret_cast<MtPatternAnimationHD*>(pattern);
    return gabi::call_ptr<s32>(animation->mVtable->getFrameMax, pattern);
}
struct MtSafeStringVtableHD {
    u8 mPrefix[0x14];
    be<u32> assureTermination;
};
inline void Mt_assureTermination(SafeString* string) {
    auto* table = gabi::at<MtSafeStringVtableHD>(string->__vtbl);
    gabi::call_ptr<void>(table->assureTermination, string);
}
// HD: this call has no trailing GC entry argument and keeps modify as a full word.
BOOL Mt_initBtpHD(mDoExt_btpAnm*, J3DModelData*, J3DAnmTexPattern*, u32 play, s32 mode, f32 speed, s32 start,
                  s32 end, u32 modify);

void Mt_initHeaderFirstHD(u8* object);
void Mt_initHeaderSecondHD(u8* object);

// HD: these photo-result bytes occupy the expansion after the GC picture fields.
struct MtPhotoResultHD {
    u8 mPrefix[0x5BEA];
    be<u8> mColorValid;
    be<u8> mSelectedPhoto;
};
inline MtPhotoResultHD* Mt_photoResultHD() { return reinterpret_cast<MtPhotoResultHD*>(dComIfGp_get()); }
struct MtModelDataAnimationHD {
    u8 mPrefix[0x38];
    be<u32> mpTexNoAnimator;
};
WWHD_OFFSET(MtModelDataAnimationHD, mpTexNoAnimator, 0x38);
inline void Mt_copyFloatWordsHD(cXyz* destination, const cXyz* source) {
    auto* out = reinterpret_cast<be<u32>*>(destination);
    auto* in = reinterpret_cast<const be<u32>*>(source);
    for (u32 i = 0; i < 3; ++i)
        out[i] = in[i];
}

// HD: reuse the existing stack eye vector and preserve the full enabled byte value.
void Mt_lookAtTargetHD(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16 yaw, s16 turn, u32 enabled);

// HD: this actor has its own GHS base vtable.
inline constexpr u32 MT_ACTOR_VTABLE_HD = 0x1001EB40;

// HD: the photo-number helper is a free function with full-width input and result.
u32 daNpcMt_changePhotoNo(u32 photo);

WWHD_OFFSET(MtPhotoSaveHD, mSaveInfo, 0x20);

// Mapped translation-unit tables; sizes are the verified HD strides.
struct MtFigureCompletionTable {
    be<u16> event[17];
};
WWHD_SIZE(MtFigureCompletionTable, 0x22);
struct MtMessageTable2 {
    be<u32> message[2];
};
WWHD_SIZE(MtMessageTable2, 0x8);
struct MtMessageTable4 {
    be<u32> message[4];
};
WWHD_SIZE(MtMessageTable4, 0x10);
struct MtMessageTable5 {
    be<u32> message[5];
};
WWHD_SIZE(MtMessageTable5, 0x14);
struct MtMessageTable8 {
    be<u32> message[8];
};
WWHD_SIZE(MtMessageTable8, 0x20);
struct MtMessageChoices {
    gptr<be<u32>> table[2];
};
WWHD_SIZE(MtMessageChoices, 0x8);
struct MtFigureRoomNames {
    gptr<char> name[8];
};
WWHD_SIZE(MtFigureRoomNames, 0x20);
struct MtModelResourceTable {
    be<u32> index[1];
};
WWHD_SIZE(MtModelResourceTable, 0x4);
struct MtPatternResourceTable {
    be<u32> index[1];
};
WWHD_SIZE(MtPatternResourceTable, 0x4);
struct MtAnimationResourceTable {
    be<u32> index[6];
};
WWHD_SIZE(MtAnimationResourceTable, 0x18);
struct MtMessageAnimationTable {
    gptr<sMtAnmDat> animation[7];
};
WWHD_SIZE(MtMessageAnimationTable, 0x1C);
struct MtInitFunctionTable {
    gabi::ptmf<u32 (daNpcMt_c::*)()> function[2];
};
WWHD_SIZE(MtInitFunctionTable, 0x10);
struct MtMoveFunctionTable {
    gabi::ptmf<void (daNpcMt_c::*)()> function[2];
};
WWHD_SIZE(MtMoveFunctionTable, 0x10);
struct MtPhaseTable {
    be<u32> function[3];
};
WWHD_SIZE(MtPhaseTable, 0xC);
struct MtCutActionTable {
    gptr<char> action[2];
};
WWHD_SIZE(MtCutActionTable, 0x8);
struct MtHeaderStaticsHD {
    be<f32> mPi[2];
    u8 mFirstObject;
    u8 mSecondObject;
    u8 mPadding[2];
    be<u32> mZero[4];
};
WWHD_SIZE(MtHeaderStaticsHD, 0x1C);
struct MtHeaderPiHD {
    be<f32> value[2];
};
WWHD_SIZE(MtHeaderPiHD, 0x8);
struct MtDestructorRecordsHD {
    be<u32> record[3][3];
};
WWHD_SIZE(MtDestructorRecordsHD, 0x24);

WWHD_SIZE(MtNpcVtableHD, 0x28);
WWHD_SIZE(MtPatternVtableHD, 0x18);
WWHD_OFFSET(MtPatternAnimationHD, mVtable, 4);
WWHD_SIZE(MtPatternAnimationHD, 8);
WWHD_SIZE(MtSafeStringVtableHD, 0x18);
WWHD_OFFSET(MtPhotoResultHD, mColorValid, 0x5BEA);
WWHD_OFFSET(MtPhotoResultHD, mSelectedPhoto, 0x5BEB);
WWHD_SIZE(MtPhotoResultHD, 0x5BEC);
WWHD_SIZE(MtModelDataAnimationHD, 0x3C);
WWHD_OFFSET(MtHeaderStaticsHD, mPi, 0);
WWHD_OFFSET(MtHeaderStaticsHD, mFirstObject, 8);
WWHD_OFFSET(MtHeaderStaticsHD, mSecondObject, 9);
WWHD_OFFSET(MtHeaderStaticsHD, mZero, 0xC);
WWHD_OFFSET(MtNpcDat, mMax_head_x, 0x00);
WWHD_OFFSET(MtNpcDat, mMax_head_y, 0x02);
WWHD_OFFSET(MtNpcDat, mMax_backbone_x, 0x04);
WWHD_OFFSET(MtNpcDat, mMax_backbone_y, 0x06);
WWHD_OFFSET(MtNpcDat, mMin_head_x, 0x08);
WWHD_OFFSET(MtNpcDat, mMin_head_y, 0x0A);
WWHD_OFFSET(MtNpcDat, mMin_backbone_x, 0x0C);
WWHD_OFFSET(MtNpcDat, mMin_backbone_y, 0x0E);
WWHD_OFFSET(MtNpcDat, mMax_turn_step, 0x10);
WWHD_OFFSET(MtNpcDat, field_0x12, 0x12);
WWHD_OFFSET(MtNpcDat, mPlayerEyeOffsetY, 0x14);
WWHD_OFFSET(MtNpcDat, mAttnYOffset, 0x18);
WWHD_OFFSET(MtNpcDat, mEyeYOffset, 0x1C);
WWHD_OFFSET(MtNpcDat, mMaxAttnDistXZ, 0x20);
WWHD_OFFSET(MtNpcDat, mLookDistXZ, 0x24);
WWHD_OFFSET(MtNpcDat, mMaxAttnAngleY, 0x28);
WWHD_OFFSET(MtNpcDat, mMaxHeadTurnVel, 0x2A);
WWHD_OFFSET(MtNpcDat, field_0x2C, 0x2C);
WWHD_OFFSET(MtNpcDat, field_0x2E, 0x2E);
WWHD_OFFSET(MtNpcDat, mCylRadius, 0x30);
WWHD_OFFSET(MtNpcDat, field_0x34, 0x34);
WWHD_OFFSET(MtNpcDat, field_0x38, 0x38);
WWHD_OFFSET(MtNpcDat, field_0x3C, 0x3C);
WWHD_OFFSET(MtNpcDat, field_0x3E, 0x3E);
WWHD_OFFSET(MtNpcDat, field_0x40, 0x40);
WWHD_OFFSET(MtNpcDat, field_0x42, 0x42);
WWHD_OFFSET(MtNpcDat, field_0x44, 0x44);
WWHD_OFFSET(MtNpcDat, field_0x46, 0x46);
WWHD_OFFSET(MtNpcDat, mTurnBackTime, 0x48);
WWHD_OFFSET(MtNpcDat, mBodyTurn, 0x4A);
WWHD_OFFSET(MtNpcDat, mLookAtPlayer, 0x4B);
