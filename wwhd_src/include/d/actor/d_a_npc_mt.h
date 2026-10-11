/* Carlov: GameCube names with the verified HD NPC base and animation expansion.
 * HD: the shadow ID is absent; the photo queue and message manager replace GC save/message forms.
 * Derived game source, local only. */
#pragma once
#include "gc_api/gc_api.h"
#include "gc_api/d/d_npc.h"

struct sMtAnmDat {
    be<u8> field_0x00;
    be<u8> field_0x01;
    be<s8> field_0x02;
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
    /* 0x890 */ be<f32> m71C;
    /* 0x894 */ be<f32> m720;
    /* 0x898 */ be<u32> mItemNo;
    /* 0x89C */ be<u8> m728;
    /* 0x89D */ be<u8> m729;
    /* 0x89E */ be<s16> mMtGetItemEventIdx;
    /* 0x8A0 */ be<s16> m72C;
    /* 0x8A2 */ be<s16> m72E;
    /* 0x8A4 */ be<s16> m730;
    /* 0x8A6 */ be<s16> m732;
    /* 0x8A8 */ be<u16> m734;
    /* 0x8AA */ be<s16> m736;
    /* 0x8AC */ u8 m738[2];
    /* 0x8AE */ be<s16> mTimer;
    /* 0x8B0 */ be<s16> m73C;
    /* 0x8B2 */ be<s16> m73E;
    /* 0x8B4 */ be<s16> m740;
    /* 0x8B6 */ be<u8> m742;
    /* 0x8B7 */ be<u8> m743;
    /* 0x8B8 */ be<u8> m744;
    /* 0x8B9 */ be<u8> mFrame;
    /* 0x8BA */ be<u8> m746;
    /* 0x8BB */ be<u8> m747;
    /* 0x8BC */ be<u8> mNpcNo;
    /* 0x8BD */ be<u8> m749;
    /* 0x8BE */ be<u8> m74A;
    /* 0x8BF */ be<u8> mAnmFlag;
    /* 0x8C0 */ be<s8> m74C;
    /* 0x8C1 */ be<s8> m74D;
    /* 0x8C2 */ be<u8> m74E;
    /* 0x8C3 */ be<s8> m74F;
    /* 0x8C4 */ be<u8> m750;
    /* 0x8C5 */ be<u8> m751;
    /* 0x8C6 */ be<u8> m752;
    /* 0x8C7 */ be<u8> m753;
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
WWHD_OFFSET(daNpcMt_c, m71C, 0x890);
WWHD_OFFSET(daNpcMt_c, m720, 0x894);
WWHD_OFFSET(daNpcMt_c, mItemNo, 0x898);
WWHD_OFFSET(daNpcMt_c, m728, 0x89C);
WWHD_OFFSET(daNpcMt_c, m729, 0x89D);
WWHD_OFFSET(daNpcMt_c, mMtGetItemEventIdx, 0x89E);
WWHD_OFFSET(daNpcMt_c, m72C, 0x8A0);
WWHD_OFFSET(daNpcMt_c, m72E, 0x8A2);
WWHD_OFFSET(daNpcMt_c, m730, 0x8A4);
WWHD_OFFSET(daNpcMt_c, m732, 0x8A6);
WWHD_OFFSET(daNpcMt_c, m734, 0x8A8);
WWHD_OFFSET(daNpcMt_c, m736, 0x8AA);
WWHD_OFFSET(daNpcMt_c, m738, 0x8AC);
WWHD_OFFSET(daNpcMt_c, mTimer, 0x8AE);
WWHD_OFFSET(daNpcMt_c, m73C, 0x8B0);
WWHD_OFFSET(daNpcMt_c, m73E, 0x8B2);
WWHD_OFFSET(daNpcMt_c, m740, 0x8B4);
WWHD_OFFSET(daNpcMt_c, m742, 0x8B6);
WWHD_OFFSET(daNpcMt_c, m743, 0x8B7);
WWHD_OFFSET(daNpcMt_c, m744, 0x8B8);
WWHD_OFFSET(daNpcMt_c, mFrame, 0x8B9);
WWHD_OFFSET(daNpcMt_c, m746, 0x8BA);
WWHD_OFFSET(daNpcMt_c, m747, 0x8BB);
WWHD_OFFSET(daNpcMt_c, mNpcNo, 0x8BC);
WWHD_OFFSET(daNpcMt_c, m749, 0x8BD);
WWHD_OFFSET(daNpcMt_c, m74A, 0x8BE);
WWHD_OFFSET(daNpcMt_c, mAnmFlag, 0x8BF);
WWHD_OFFSET(daNpcMt_c, m74C, 0x8C0);
WWHD_OFFSET(daNpcMt_c, m74D, 0x8C1);
WWHD_OFFSET(daNpcMt_c, m74E, 0x8C2);
WWHD_OFFSET(daNpcMt_c, m74F, 0x8C3);
WWHD_OFFSET(daNpcMt_c, m750, 0x8C4);
WWHD_OFFSET(daNpcMt_c, m751, 0x8C5);
WWHD_OFFSET(daNpcMt_c, m752, 0x8C6);
WWHD_OFFSET(daNpcMt_c, m753, 0x8C7);
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
    /* 0x14 */ be<f32> field_0x14;
    /* 0x18 */ be<f32> field_0x18;
    /* 0x1C */ be<f32> field_0x1C;
    /* 0x20 */ be<f32> field_0x20;
    /* 0x24 */ be<f32> field_0x24;
    /* 0x28 */ be<s16> field_0x28;
    /* 0x2A */ be<s16> field_0x2A;
    /* 0x2C */ be<s16> field_0x2C;
    /* 0x2E */ be<s16> field_0x2E;
    /* 0x30 */ be<f32> field_0x30;
    /* 0x34 */ be<f32> field_0x34;
    /* 0x38 */ be<f32> field_0x38;
    /* 0x3C */ be<s16> field_0x3C;
    /* 0x3E */ be<s16> field_0x3E;
    /* 0x40 */ be<s16> field_0x40;
    /* 0x42 */ be<s16> field_0x42;
    /* 0x44 */ be<s16> field_0x44;
    /* 0x46 */ be<s16> field_0x46;
    /* 0x48 */ be<s16> field_0x48;
    /* 0x4A */ be<u8> field_0x4A;
    /* 0x4B */ be<u8> field_0x4B;
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
WWHD_OFFSET(MtNpcDat, field_0x14, 0x14);
WWHD_OFFSET(MtNpcDat, field_0x18, 0x18);
WWHD_OFFSET(MtNpcDat, field_0x1C, 0x1C);
WWHD_OFFSET(MtNpcDat, field_0x20, 0x20);
WWHD_OFFSET(MtNpcDat, field_0x24, 0x24);
WWHD_OFFSET(MtNpcDat, field_0x28, 0x28);
WWHD_OFFSET(MtNpcDat, field_0x2A, 0x2A);
WWHD_OFFSET(MtNpcDat, field_0x2C, 0x2C);
WWHD_OFFSET(MtNpcDat, field_0x2E, 0x2E);
WWHD_OFFSET(MtNpcDat, field_0x30, 0x30);
WWHD_OFFSET(MtNpcDat, field_0x34, 0x34);
WWHD_OFFSET(MtNpcDat, field_0x38, 0x38);
WWHD_OFFSET(MtNpcDat, field_0x3C, 0x3C);
WWHD_OFFSET(MtNpcDat, field_0x3E, 0x3E);
WWHD_OFFSET(MtNpcDat, field_0x40, 0x40);
WWHD_OFFSET(MtNpcDat, field_0x42, 0x42);
WWHD_OFFSET(MtNpcDat, field_0x44, 0x44);
WWHD_OFFSET(MtNpcDat, field_0x46, 0x46);
WWHD_OFFSET(MtNpcDat, field_0x48, 0x48);
WWHD_OFFSET(MtNpcDat, field_0x4A, 0x4A);
WWHD_OFFSET(MtNpcDat, field_0x4B, 0x4B);
