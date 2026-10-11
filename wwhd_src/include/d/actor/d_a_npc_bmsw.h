/* daNpc_Bmsw_c (Koboli, the Rito mail sorter) and its mail/camera helpers, WWHD layout.
 *
 * GameCube -> WWHD, measured from the WWHD code of d_a_npc_bmsw (constructor 0220E0D4 allocates
 * 0xD84; GameCube 0x9E4):
 * - base fopNpc_npc_c 0x7DC (d/d_npc.h): m_ArmR .. field_0x6DC +0x118 (mPhs 0x7E4);
 * - the second dKy_tevstr_c (field_0x6E0) is HD-sized 0x1C8 (GameCube 0xB0): mpMorfHand ..
 *   m_head_tex_pattern +0x230;
 * - mDoExt_btpAnm grew from 0x14 to 0x74 (constructor 025E7820): field_0x80C +0x290;
 * - SwMail_c: its pointer-to-member mFunc is 8 bytes (GHS) and its btpAnm 0x74: 0xBC (GameCube
 *   0x60), so mSwMail0/1/2 at 0xAA0/0xB5C/0xC18 and field_0x930 .. field_0x9C4 +0x3A4;
 * - STControl at 0xD2C keeps its size 0x28 (HD vtable at +0x24);
 * - the actor's pointer-to-member mCurrActionFunc is 8 bytes: field_0x9D4 .. field_0x9E0 +0x3A0.
 * The HD vtable at 0xB4 is 0x10018390. */
#pragma once
#include "JSystem/J3DGraph.h"
#include "bindings.h"
#include "d/d_shop.h"

struct daNpc_Bmsw_c;
struct SwMail_c;
using BmswActionFunc = gabi::ptmf<BOOL (daNpc_Bmsw_c::*)(void*)>;
using BmswMailFunc = gabi::ptmf<void (SwMail_c::*)()>;
WWHD_SIZE(BmswActionFunc, 8);
WWHD_SIZE(BmswMailFunc, 8);
#include "d/d_npc.h"

struct SwMail_c {
    /* methods (GameCube names) */
    BOOL MailCreateInit(cXyz*, cXyz*);
    static u8 getNextNo(u8);
    static be<u8>& m_same_count;
    static be<u8>& m_no_buff;
    void init();
    void set_mtx();
    void set_mtx_throw();
    void DummyInit();
    void Dummy();
    void AppearInit();
    void Appear();
    void WaitInit();
    void Wait();
    void ThrowInit(cXyz*, u8); /* cXyz by value: GHS passes a pointer to a copy */
    void Throw();
    void EndInit();
    void End();
    void SeDelete();
    void move();
    void draw(dKy_tevstr_c*);

    /* 0x00 */ BmswMailFunc mFunc; /* HD: 8 bytes (GameCube 12) */
    /* 0x08 */ gptr<J3DModel> mpModel;
    /* 0x0C */ mDoExt_btpAnm field_0x10; /* mDoExt_btpAnm (HD 0x74) */
    /* 0x80 */ cXyz field_0x24;
    /* 0x8C */ cXyz field_0x30;
    /* 0x98 */ cXyz field_0x3C;
    /* 0xA4 */ csXyz field_0x48;
    /* 0xAA */ csXyz field_0x4E;
    /* 0xB0 */ be<u8> field_0x54;
    /* 0xB1 */ be<u8> field_0x55;
    /* 0xB2 */ be<u8> field_0x56;
    /* 0xB3 */ u8 field_0x57;
    /* 0xB4 */ gptr<cXyz> field_0x58;
    /* 0xB8 */ gptr<cXyz> field_0x5C;
};
WWHD_OFFSET(SwMail_c, field_0x24, 0x80);
WWHD_OFFSET(SwMail_c, field_0x54, 0xB0);
WWHD_SIZE(SwMail_c, 0xBC);

struct SwCam_c {
    void Move();

    /* 0x00 */ cXyz mCenter;
    /* 0x0C */ cXyz mEye;
    /* 0x18 */ be<f32> mFovY;
    /* 0x1C */ be<s8> field_0x1C;
    /* 0x1D */ be<s8> field_0x1D;
    /* 0x1E */ be<u8> mActive;
    /* 0x1F */ u8 _1F;
};
WWHD_SIZE(SwCam_c, 0x20);

/* STControl (0x28, HD vtable at +0x24) */
struct BmswSTControl {
    /* 0x00 */ u8 _00[0x24];
    /* 0x24 */ be<u32> __vtbl;
};
WWHD_SIZE(BmswSTControl, 0x28);

struct daNpc_Bmsw_c : fopNpc_npc_c {
    enum ActionStatus {
        ACTION_STARTING = 0,
        ACTION_ONGOING = 1,
        ACTION_UNK_2 = 2,
        ACTION_ENDING = -1
    };

    /* methods: GameCube declarations; return types as the WWHD code returns them */
    BOOL initTexPatternAnm(u32); /* bool, passed on unnormalised */
    void playTexPatternAnm();
    void setAnm(s8);
    u8 chkAttention(cXyz*, s16); /* bool (stored as is); cXyz by value: pointer to a copy */
    void eventOrder();
    void checkOrder();
    u16 next_msgStatus(be<u32>*);
    u32 getMsg();
    void anmAtr(u16);
    BOOL CreateInit();
    void set_mtx();
    void setAttention();
    void lookBack();
    void wait01();
    void talk01();
    BOOL wait_action(void*);
    BOOL checkNextMailThrowOK();
    void setGameGetRupee(s16);
    void TimerCountDown();
    BOOL shiwake_game_action(void*);
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL CreateHeap();

    /* 0x7DC */ be<s8> m_ArmR;
    /* 0x7DD */ be<s8> m_ArmL;
    /* 0x7DE */ be<s8> m_handR;
    /* 0x7DF */ be<s8> m_handL;
    /* 0x7E0 */ be<s8> m_body_ArmR;
    /* 0x7E1 */ be<s8> m_body_ArmL;
    /* 0x7E2 */ be<s8> m_neck_jnt_num;
    /* 0x7E3 */ u8 _7E3;
    /* 0x7E4 */ request_of_phase_process_class mPhs;
    /* 0x7EC */ gptr<J3DModel> field_0x6D4; /* head */
    /* 0x7F0 */ gptr<J3DModel> field_0x6D8; /* bag */
    /* 0x7F4 */ gptr<J3DModel> field_0x6DC; /* letter */
    /* 0x7F8 */ dKy_tevstr_c field_0x6E0;   /* HD 0x1C8 */
    /* 0x9C0 */ gptr<mDoExt_McaMorf> mpMorfHand;
    /* 0x9C4 */ Mtx34 field_0x794; /* arm L */
    /* 0x9F4 */ Mtx34 field_0x7C4; /* arm R */
    /* 0xA24 */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0xA28 */ mDoExt_btpAnm field_0x7F8; /* mDoExt_btpAnm (HD 0x74) */
    /* 0xA9C */ be<u8> field_0x80C;
    /* 0xA9D */ u8 _A9D[3];
    /* 0xAA0 */ SwMail_c mSwMail0;
    /* 0xB5C */ SwMail_c mSwMail1;
    /* 0xC18 */ SwMail_c mSwMail2;
    /* 0xCD4 */ gptr<SwMail_c> field_0x930[3];
    /* 0xCE0 */ be<u8> field_0x93C;
    /* 0xCE1 */ u8 _CE1[3];
    /* 0xCE4 */ SwCam_c mSwCam;
    /* 0xD04 */ be<s16> field_0x960;
    /* 0xD06 */ be<u8> field_0x962;
    /* 0xD07 */ be<u8> field_0x963;
    /* 0xD08 */ be<s16> field_0x964;
    /* 0xD0A */ u8 _D0A[2];
    /* 0xD0C */ gptr<ShopCursor_c> mpShopCursor;
    /* 0xD10 */ cXyz mAttPos;
    /* 0xD1C */ u8 _D1C[2];
    /* 0xD1E */ be<s16> field_0x97A;
    /* 0xD20 */ u8 _D20[2];
    /* 0xD22 */ csXyz field_0x97E;
    /* 0xD28 */ u8 _D28[4];
    /* 0xD2C */ BmswSTControl field_0x988;
    /* 0xD54 */ be<s8> field_0x9B0;
    /* 0xD55 */ be<s8> field_0x9B1;
    /* 0xD56 */ u8 _D56[2];
    /* 0xD58 */ be<s32> field_0x9B4;
    /* 0xD5C */ be<u8> mHasAttention;
    /* 0xD5D */ be<u8> field_0x9B9;
    /* 0xD5E */ u8 _D5E[6];
    /* 0xD64 */ be<u32> field_0x9C0;
    /* 0xD68 */ be<s8> field_0x9C4;
    /* 0xD69 */ u8 _D69[3];
    /* 0xD6C */ BmswActionFunc mCurrActionFunc; /* HD: 8 bytes (GameCube 12) */
    /* 0xD74 */ be<s8> field_0x9D4;
    /* 0xD75 */ be<s8> field_0x9D5;
    /* 0xD76 */ be<s8> field_0x9D6;
    /* 0xD77 */ be<s8> field_0x9D7;
    /* 0xD78 */ be<s16> field_0x9D8;
    /* 0xD7A */ be<s8> field_0x9DA;
    /* 0xD7B */ u8 _D7B[2];
    /* 0xD7D */ be<s8> mActionStatus;
    /* 0xD7E */ u8 _D7E[2];
    /* 0xD80 */ be<u32> field_0x9E0;
};
WWHD_OFFSET(daNpc_Bmsw_c, mPhs, 0x7E4);
WWHD_OFFSET(daNpc_Bmsw_c, mpMorfHand, 0x9C0);
WWHD_OFFSET(daNpc_Bmsw_c, field_0x7F8, 0xA28);
WWHD_OFFSET(daNpc_Bmsw_c, mSwMail0, 0xAA0);
WWHD_OFFSET(daNpc_Bmsw_c, mSwCam, 0xCE4);
WWHD_OFFSET(daNpc_Bmsw_c, field_0x988, 0xD2C);
WWHD_OFFSET(daNpc_Bmsw_c, mCurrActionFunc, 0xD6C);
WWHD_OFFSET(daNpc_Bmsw_c, field_0x9E0, 0xD80);
WWHD_SIZE(daNpc_Bmsw_c, 0xD84);

// Unit-private ABI bindings preserve HD register widths.
void Bmsw_particleCallbackConstruct(dPa_followEcallBack* arg0);
void* Bmsw_resourceGetID(dRes_control_c* arg0, SafeString* arg1, s32 arg2);
s16 Bmsw_calcTimer(be<s16>* arg0);
int Bmsw_btpInit(void* arg0, J3DModelData* arg1, void* arg2, s32 arg3, s32 arg4, f32 arg5, s32 arg6,
                 s32 arg7, u32 arg8, s32 arg9);
void Bmsw_seStart(u32 arg0);
s16 Bmsw_targetAngleX(const cXyz* arg0, const cXyz* arg1);
f32 Bmsw_addCalcPos(cXyz* arg0, const cXyz* arg1, f32 arg2, f32 arg3, f32 arg4);
void Bmsw_addCalcPos2(cXyz* arg0, const cXyz* arg1, f32 arg2, f32 arg3);
bool Bmsw_normalizeRS(cXyz* arg0);
void Bmsw_ZXYrotS(Mtx34* arg0, s16 arg1, s16 arg2, s16 arg3);
void Bmsw_seStartPosition(u32 arg0, cXyz* arg1);
void Bmsw_playerEyePos(cXyz* arg0, f32 arg1);
void Bmsw_jointLookAtTarget(dNpc_JntCtrl_c* arg0, be<s16>* arg1, cXyz* arg2, cXyz* arg3, s16 arg4,
                            s16 arg5, u8 arg6);
void Bmsw_assureTermination(SafeString* arg0);
void Bmsw_stickSetWaitParm(BmswSTControl* arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4, f32 arg5,
                           f32 arg6, s16 arg7, s16 arg8);
void Bmsw_XYZrotM(Mtx34* arg0, s16 arg1, s16 arg2, s16 arg3);
u32 Bmsw_createChildByName(const char* arg0, u32 arg1, u32 arg2, cXyz* arg3, s32 arg4, csXyz* arg5,
                           cXyz* arg6, u32 arg7);
u32 Bmsw_createActorByName(const char* arg0, u32 arg1, cXyz* arg2, s32 arg3, csXyz* arg4,
                           cXyz* arg5, u32 arg6);
void Bmsw_tevstrInit(dKy_tevstr_c* arg0, s8 arg1, u8 arg2);
void Bmsw_registFigure(u8 arg0, fopAc_ac_c* arg1, cXyz* arg2, s16 arg3, f32 arg4, f32 arg5,
                       f32 arg6);
void Bmsw_npcHioConstruct(u32 arg0);
void Bmsw_xyzInit(int arg0);
void Bmsw_angleInit(int arg0);
void Bmsw_cameraStay(u32 arg0);
void Bmsw_cameraSet(u32 arg0, cXyz* arg1, cXyz* arg2, f32 arg3, int arg4);
int Bmsw_isStageBossEnemy(int arg0);
void Bmsw_actorConstruct(daNpc_Bmsw_c* arg0);
s8 Bmsw_getRoomId(dBgS* arg0, u32 arg1);
u8 Bmsw_getPolyColor(dBgS* arg0, u32 arg1);
void Bmsw_cursorDraw(ShopCursor_c* arg0);
u32 Bmsw_jointNameHeader(J3DModelData* arg0);
s8 Bmsw_jointNameIndex(u32 arg0, const char* arg1);
u32 Bmsw_jointTreeHeader(J3DModelData* arg0);
ShopCursor_c* Bmsw_cursorCreate(void* arg0, void* arg1, f32 arg2);
void Bmsw_npcConstruct(daNpc_Bmsw_c* arg0);
void Bmsw_btpConstruct(void* arg0);
void Bmsw_stickInit(BmswSTControl* arg0);
void Bmsw_stickDestroy(BmswSTControl* arg0, int arg1);
void Bmsw_cylinderDestroy(dCcD_Cyl* arg0, int arg1);
void Bmsw_statusDestroy(dCcD_Stts* arg0, int arg1);
void Bmsw_circleDestroy(u32 arg0, int arg1);
void Bmsw_acchDestroy(dBgS_ObjAcch* arg0, int arg1);
void Bmsw_actorDestroy(daNpc_Bmsw_c* arg0, int arg1);
void Bmsw_startMiniGame(u32 arg0);
void Bmsw_endMiniGame(u32 arg0);
u32 Bmsw_getEventIdx(dEvent_manager_c* arg0, const char* arg1, int arg2);
int Bmsw_orderOtherEventId(daNpc_Bmsw_c* arg0, u32 arg1, int arg2, u16 arg3, int arg4, int arg5);
void Bmsw_timerCreate(int arg0, u16 arg1, int arg2, int arg3, f32 arg4, f32 arg5, f32 arg6,
                      f32 arg7);
void Bmsw_timerStart(u32 arg0);
void Bmsw_timerStartWithLimit(u32 arg0, s16 arg1);
void Bmsw_cursorSetPos(ShopCursor_c* arg0, cXyz* arg1);
void Bmsw_padRepeatOn(u32 arg0, int arg1, int arg2);
void Bmsw_padRepeatOff(u32 arg0);
int Bmsw_padCheckTriggerA(int arg0);
void Bmsw_cursorAnimationPlay(ShopCursor_c* arg0);
void Bmsw_timerEnd(u32 arg0, s32 arg1);
int Bmsw_timerDeleteCheck(u32 arg0);
void Bmsw_timerDeleteRequest(u32 arg0);

struct BmswPositionTable {
    cXyz positions[2][3];
};
WWHD_SIZE(BmswPositionTable, 0x48);
struct BmswMessageManager {
    u8 pad[0x948];
    be<s32> mSelectNum;
};
WWHD_OFFSET(BmswMessageManager, mSelectNum, 0x948);
struct BmswPad {
    u8 pad0[0x18];
    be<u32> mTrigger;
    u8 pad1C[4];
    be<u32> mRepeatTrigger;
    u8 pad24[0x100];
    be<u32> mDirectionalHold;
};
WWHD_OFFSET(BmswPad, mDirectionalHold, 0x124);
struct BmswTimer {
    u8 pad[0x124];
    be<u8> mStatus;
};
WWHD_OFFSET(BmswTimer, mStatus, 0x124);
struct BmswMiniGameSystem {
    u8 pad[0x178];
    be<u32> mpGame;
};
WWHD_OFFSET(BmswMiniGameSystem, mpGame, 0x178);
struct BmswJointData {
    u8 pad[4];
    be<u16> mNumber;
};
WWHD_OFFSET(BmswJointData, mNumber, 4);
struct BmswNameHeader {
    u8 pad[0x10];
    be<u32> mNameOffset;
};
WWHD_OFFSET(BmswNameHeader, mNameOffset, 0x10);
struct BmswJointTree {
    u8 pad[8];
    be<u16> mCount;
};
WWHD_OFFSET(BmswJointTree, mCount, 8);
struct BmswModelAnimation {
    u8 pad[0x38];
    be<u32> mpTexPattern;
};
WWHD_OFFSET(BmswModelAnimation, mpTexPattern, 0x38);
struct BmswAnimation {
    u8 pad[4];
    be<u32> mVtable;
};
WWHD_OFFSET(BmswAnimation, mVtable, 4);
struct BmswVirtualFunction {
    be<u32> mAdjustment;
    be<u32> mFunction;
};
WWHD_SIZE(BmswVirtualFunction, 8);
struct BmswVirtualTable {
    u8 pad[0x14];
    be<u32> mAssureTermination;
};
WWHD_OFFSET(BmswVirtualTable, mAssureTermination, 0x14);
struct BmswLightDefaultBlock {
    be<f32> mVectors[6];
    be<u8> mColor[4];
    be<s16> mSignedColor[4];
    be<f32> mFactors[8];
};
WWHD_SIZE(BmswLightDefaultBlock, 0x44);
struct BmswTevWork {
    u8 pad[0x90];
    GXColorS10 mColorC0;
    GXColor mColorK0;
    u8 pad9C[0x1D];
    be<s8> mRoomNo;
    be<u8> mEnvrIdxOverride;
};
WWHD_OFFSET(BmswTevWork, mColorC0, 0x90);
WWHD_OFFSET(BmswTevWork, mColorK0, 0x98);
WWHD_OFFSET(BmswTevWork, mRoomNo, 0xB9);

enum BmswResourceId {
    dRes_ID_BMSW_BTP_BMHEAD11_e = 34,
    dRes_ID_BMSW_BCK_BM_WAIT01_e = 27,
    dRes_ID_BMSW_BCK_BM_WAIT02_e = 32,
    dRes_ID_BMSW_BCK_BM_TALK02_e = 23,
    dRes_ID_BMSW_BCK_BM_TALK03_e = 31,
    dRes_ID_BMSW_BCK_BM_SHIWAKE01_e = 20,
    dRes_ID_BMSW_BCK_BM_SHIWAKE02_e = 21,
    dRes_ID_BMSW_BCK_BMARM_WAIT01_e = 26,
    dRes_ID_BMSW_BCK_BMARM_WAIT02_e = 30,
    dRes_ID_BMSW_BCK_BMARM_TALK02_e = 19,
    dRes_ID_BMSW_BCK_BMARM_TALK03_e = 29,
    dRes_ID_BMSW_BCK_BMARM_SHIWAKE01_e = 17,
    dRes_ID_BMSW_BCK_BMARM_SHIWAKE02_e = 18,

    dRes_ID_BMSW_BDL_BM_BAG_e = 2,
    dRes_ID_BMSW_BDL_BM_e = 3,
    dRes_ID_BMSW_BDL_BMARM_e = 4,
    dRes_ID_BMSW_BCK_BMARM_TALK01_e = 0,
    dRes_ID_BMSW_BDL_QMAIL_e = 15,
    dRes_ID_BMSW_BTP_QMAIL_e = 16,
    dRes_ID_BMSW_BCK_BM_TALK01_e = 22,
    dRes_ID_BMSW_BDL_BMHEAD11_e = 33,
    dRes_ID_BMSW_BDL_BM_LETTER_e = 28,
    dRes_ID_BMSW_BRK_SHOP_CURSOR01_e = 25,
    dRes_ID_BMSW_BMD_SHOP_CURSOR01_e = 24,
};

struct BmswFloatWord {
    be<f32> value;
    be<u32>& word() { return gabi::object_at<be<u32>>(gabi::ea(&value)); }
};
WWHD_SIZE(BmswFloatWord, 4);
struct BmswPositionWords {
    be<u32> x, y, z;
};
WWHD_SIZE(BmswPositionWords, 12);

u8 Bmsw_getNextNo(u8 previous_no);

inline s16 Bmsw_virtualOffset(const BmswActionFunc& member) {
    return gabi::object_at<be<s16>>(gabi::ea(&member.f) + sizeof(s16)).get();
}
inline s16 Bmsw_virtualOffset(const BmswMailFunc& member) {
    return gabi::object_at<be<s16>>(gabi::ea(&member.f) + sizeof(s16)).get();
}

// Weak guest fallbacks for linking this actor independently.
BOOL Bmsw_guest_SwMail_c_MailCreateInit(SwMail_c* self, cXyz* a0, cXyz* a1);
void Bmsw_guest_SwMail_c_init(SwMail_c* self);
void Bmsw_guest_SwMail_c_set_mtx(SwMail_c* self);
void Bmsw_guest_SwMail_c_set_mtx_throw(SwMail_c* self);
void Bmsw_guest_SwMail_c_DummyInit(SwMail_c* self);
void Bmsw_guest_SwMail_c_Dummy(SwMail_c* self);
void Bmsw_guest_SwMail_c_AppearInit(SwMail_c* self);
void Bmsw_guest_SwMail_c_Appear(SwMail_c* self);
void Bmsw_guest_SwMail_c_WaitInit(SwMail_c* self);
void Bmsw_guest_SwMail_c_Wait(SwMail_c* self);
void Bmsw_guest_SwMail_c_ThrowInit(SwMail_c* self, cXyz* a0, u8 a1);
void Bmsw_guest_SwMail_c_Throw(SwMail_c* self);
void Bmsw_guest_SwMail_c_EndInit(SwMail_c* self);
void Bmsw_guest_SwMail_c_End(SwMail_c* self);
void Bmsw_guest_SwMail_c_SeDelete(SwMail_c* self);
void Bmsw_guest_SwMail_c_move(SwMail_c* self);
void Bmsw_guest_SwMail_c_draw(SwMail_c* self, dKy_tevstr_c* a0);
void Bmsw_guest_SwCam_c_Move(SwCam_c* self);
BOOL Bmsw_guest_daNpc_Bmsw_c_initTexPatternAnm(daNpc_Bmsw_c* self, u32 a0);
void Bmsw_guest_daNpc_Bmsw_c_playTexPatternAnm(daNpc_Bmsw_c* self);
void Bmsw_guest_daNpc_Bmsw_c_setAnm(daNpc_Bmsw_c* self, s8 a0);
u8 Bmsw_guest_daNpc_Bmsw_c_chkAttention(daNpc_Bmsw_c* self, cXyz* a0, s16 a1);
void Bmsw_guest_daNpc_Bmsw_c_eventOrder(daNpc_Bmsw_c* self);
void Bmsw_guest_daNpc_Bmsw_c_checkOrder(daNpc_Bmsw_c* self);
u16 Bmsw_guest_daNpc_Bmsw_c_next_msgStatus(daNpc_Bmsw_c* self, be<u32>* a0);
u32 Bmsw_guest_daNpc_Bmsw_c_getMsg(daNpc_Bmsw_c* self);
void Bmsw_guest_daNpc_Bmsw_c_anmAtr(daNpc_Bmsw_c* self, u16 a0);
BOOL Bmsw_guest_daNpc_Bmsw_c_CreateInit(daNpc_Bmsw_c* self);
void Bmsw_guest_daNpc_Bmsw_c_set_mtx(daNpc_Bmsw_c* self);
void Bmsw_guest_daNpc_Bmsw_c_setAttention(daNpc_Bmsw_c* self);
void Bmsw_guest_daNpc_Bmsw_c_lookBack(daNpc_Bmsw_c* self);
void Bmsw_guest_daNpc_Bmsw_c_wait01(daNpc_Bmsw_c* self);
void Bmsw_guest_daNpc_Bmsw_c_talk01(daNpc_Bmsw_c* self);
BOOL Bmsw_guest_daNpc_Bmsw_c_wait_action(daNpc_Bmsw_c* self, void* a0);
BOOL Bmsw_guest_daNpc_Bmsw_c_checkNextMailThrowOK(daNpc_Bmsw_c* self);
void Bmsw_guest_daNpc_Bmsw_c_setGameGetRupee(daNpc_Bmsw_c* self, s16 a0);
void Bmsw_guest_daNpc_Bmsw_c_TimerCountDown(daNpc_Bmsw_c* self);
BOOL Bmsw_guest_daNpc_Bmsw_c_shiwake_game_action(daNpc_Bmsw_c* self, void* a0);
BOOL Bmsw_guest_daNpc_Bmsw_c__draw(daNpc_Bmsw_c* self);
BOOL Bmsw_guest_daNpc_Bmsw_c__execute(daNpc_Bmsw_c* self);
BOOL Bmsw_guest_daNpc_Bmsw_c__delete(daNpc_Bmsw_c* self);
cPhs_State Bmsw_guest_daNpc_Bmsw_c__create(daNpc_Bmsw_c* self);
BOOL Bmsw_guest_daNpc_Bmsw_c_CreateHeap(daNpc_Bmsw_c* self);

// HD: HIO vtables follow their fields; GameCube members shift down four bytes.
struct daNpc_Bmsw_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ dNpc_HIO_c mNpc;
    /* 0x2C */ be<s16> field_0x30;
    /* 0x2E */ be<s16> field_0x32;
    /* 0x30 */ be<s16> field_0x34;
    /* 0x32 */ be<s16> r_1;
    /* 0x34 */ be<s16> g_1;
    /* 0x36 */ be<s16> b_1;
    /* 0x38 */ be<s16> r_2;
    /* 0x3A */ be<s16> g_2;
    /* 0x3C */ be<s16> b_2;
    /* 0x3E */ u8 _3E[2];
    /* 0x40 */ be<f32> field_0x44;
    /* 0x44 */ be<f32> field_0x48;
    /* 0x48 */ be<f32> field_0x4C;
    /* 0x4C */ be<f32> field_0x50;
    /* 0x50 */ be<f32> field_0x54;
    /* 0x54 */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_Bmsw_HIO_c, 0x58);

struct BmswFrameVtable {
    u8 pad[0x14];
    be<u32> mGetFrameMax;
};
WWHD_OFFSET(BmswFrameVtable, mGetFrameMax, 0x14);

enum BmswSoundId : u32 {
    JA_SE_MINIGAME_TIMER_30 = 0x8AD,
    JA_SE_MINIGAME_TIMER_10 = 0x8AE,
    JA_SE_LETTER_GAME_CURSOR = 0x8B0,
    JA_SE_LETTER_GAME_OK = 0x8B1,
    JA_SE_LETTER_GAME_TIMER_0 = 0x8AF,
    JA_SE_LETTER_GAME_NEW = 0x69A5,
    JA_SE_LK_SW_KAZEKIRI_S = 0x2800,
    JA_SE_LETTER_IN_BOX = 0x69A6,
    JA_SE_MINIGAME_RIGHT = 0x8A5,
    JA_SE_MINIGAME_WRONG = 0x8A6,
};

WWHD_SIZE(BmswMessageManager, 0x94C);
WWHD_SIZE(BmswPad, 0x128);
WWHD_SIZE(BmswTimer, 0x125);
WWHD_SIZE(BmswMiniGameSystem, 0x17C);
WWHD_SIZE(BmswJointData, 6);
WWHD_SIZE(BmswNameHeader, 0x14);
WWHD_SIZE(BmswJointTree, 0xA);
WWHD_SIZE(BmswModelAnimation, 0x3C);
WWHD_SIZE(BmswAnimation, 8);
WWHD_SIZE(BmswVirtualTable, 0x18);
WWHD_SIZE(BmswFrameVtable, 0x18);
WWHD_SIZE(BmswTevWork, 0xBB);
