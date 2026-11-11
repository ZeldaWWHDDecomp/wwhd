/* daNpc_Btsw_c (Baito, the Rito postman's mail-sorting game), WWHD layout.
 *
 * GameCube -> WWHD, measured from the out-of-line constructor (022186EC, allocates 0xD70),
 * CreateHeap and the other functions of the unit:
 * - fopNpc_npc_c 0x7DC (GameCube 0x6C4, vtable merged into fopAc_ac_c): m_handL.. +0x118;
 * - dKy_tevstr_c of the letter model at 0x7EC (HD larger: GameCube 0x6D4..0x7E4 -> 0x7EC..0xA14);
 * - mDoExt_btpAnm 0x74 (GameCube 0x14): the members after it +0x290;
 * - SwMail2_c 0xBC (GameCube 0x60: btpAnm +0x60, pointer to member 8 bytes instead of 12);
 * - from SwCam2_c (0xCD4, GameCube 0x930) on: +0x3A4; the action pointer to member is 8 bytes,
 *   so the members after it are +0x3A0. Size 0xD70 (GameCube 0x9D0). */
#pragma once
#include "JSystem/J3DGraph.h"
#include "bindings.h"
#include "d/d_shop.h"

// HD resource IDs; the TWW generated resource header is absent from this checkout.
enum BtswResourceId {
    dRes_ID_BTSW_BDL_QMAIL_e = 4,
    dRes_ID_BTSW_BMD_SHOP_CURSOR01_e = 5,
    dRes_ID_BTSW_BRK_SHOP_CURSOR01_e = 8,
    dRes_ID_BTSW_BTP_QMAIL_e = 10,
    dRes_ID_BTSW_BDL_BN_e = 13,
    dRes_ID_BTSW_BTP_BN_MABA_e = 14,
    dRes_ID_BTSW_BCK_BN_KASIGE_e = 15,
    dRes_ID_BTSW_BCK_BN_ONEGAI_e = 16,
    dRes_ID_BTSW_BCK_BN_SIWAKE01_e = 17,
    dRes_ID_BTSW_BCK_BN_SIWAKE02_e = 18,
    dRes_ID_BTSW_BCK_BN_SUGOI_e = 19,
    dRes_ID_BTSW_BCK_BN_TALK01_e = 20,
    dRes_ID_BTSW_BCK_BN_TALK02_e = 21,
    dRes_ID_BTSW_BCK_BN_WAIT01_e = 22,
    dRes_ID_BTSW_BCK_BN_WAIT02_e = 23,
    dRes_ID_BTSW_BCK_BN_WALK_e = 24,
    dRes_ID_BTSW_BDL_BM_LETTER_e = 25,
};

/* GHS pointer to member function (8 bytes): {s16 this delta, s16 vtable index (0 NULL, < 0 plain),
 * u32 function (its low half is the vtable offset for virtual members)} */
struct BtswActionFunc {
    /* 0x0 */ be<s16> d;
    /* 0x2 */ be<s16> i;
    /* 0x4 */ be<u32> f;
};
WWHD_SIZE(BtswActionFunc, 8);

/* SwMail2_c: one letter of the sorting game (HD 0xBC) */
u8 Btsw_getNextNo(u8 previous_no);

struct SwMail2_c {
    /* 0x00 */ BtswActionFunc mFunc;
    /* 0x08 */ gptr<J3DModel> mpModel;
    /* 0x0C */ mDoExt_btpAnm mMailBtpAnm;  /* was field_0x10: MailCreateInit 022181EC sets it up with the letter's texture pattern; SwMail2_c::draw 022197C8 enters it at mBoxNo (tww the same) */
    /* 0x80 */ cXyz mPos;  /* was field_0x24: init 02217E74 starts it below the camera point; Appear 02219C1C / Wait 02219D88 / Throw 02219F20 / End 0221A1E0 move it; set_mtx 02217D98 / set_mtx_throw 022199FC place the model at it plus mPosOffset (tww the same) */
    /* 0x8C */ cXyz field_0x30;          /* GameCube field_0x30 (throw target, then velocity) */
    /* 0x98 */ cXyz mPosOffset;  /* was field_0x3C: init 02217E74 zero; Throw 02219F20 raises its y in an arc while the letter flies, End 0221A1E0 eases it back to zero; set_mtx 02217D98 / set_mtx_throw 022199FC add it to mPos (tww the same) */
    /* 0xA4 */ csXyz mAngle;  /* was field_0x48: the letter model's rotation: set_mtx 02217D98 (ZXY) / set_mtx_throw 022199FC (X, Y) rotate by it; Appear / Wait / Throw / End turn it (tww the same) */
    /* 0xAA */ csXyz field_0x4E;         /* GameCube field_0x4E */
    /* 0xB0 */ be<u8> mBoxNo;  /* was field_0x54: the box the letter belongs to (the texture frame): MailCreateInit 022181EC / init 02217E74 0, DummyInit 02218114 / AppearInit 02219B24 getNextNo; Throw 02219F20 scores when it equals mThrownBoxNo; draw 022197C8 enters mMailBtpAnm at it (tww the same) */
    /* 0xB1 */ be<u8> mThrownBoxNo;  /* was field_0x55: ThrowInit 02219EA0 stores the box the cursor threw the letter at; Throw 02219F20 / End 0221A1E0 compare it with mBoxNo (right / wrong); init 02217E74 0 (tww the same) */
    /* 0xB2 */ be<u8> mThrowPhase;  /* was field_0x56: ThrowInit 02219EA0 / init 02217E74 / AppearInit 02219B24 0; Throw 02219F20 moves the letter to the throw start while 0 and sets 1 to fly it to the box (tww the same) */
    /* 0xB3 */ u8 _B3;
    /* 0xB4 */ gptr<cXyz> mpCamCenter;  /* was field_0x58: MailCreateInit 022181EC stores &mSwCam.mCenter there (CreateHeap 022182F4 passes it first); Appear 02219C1C / Wait 02219D88 / Throw 02219F20 turn the letter by cLib_targetAngleY(it, mpCamEye), Appear / Wait place the letter 120 from mpCamEye towards it (tww the same; tww's accessor getEyeP() returns this point, its getEyeP / getCenterP names are swapped: dCamera_c::Set takes (center, eye)) */
    /* 0xB8 */ gptr<cXyz> mpCamEye;  /* was field_0x5C: MailCreateInit 022181EC stores &mSwCam.mEye there (CreateHeap 022182F4 passes it second); init 02217E74 / DummyInit 02218114 / AppearInit 02219B24 start the letter 200 below it; Appear 02219C1C / Wait 02219D88 place the letter 120 from it towards mpCamCenter; Throw 02219F20 starts the throw from it (tww the same; tww's accessor getCenterP() returns this point, its getEyeP / getCenterP names are swapped: dCamera_c::Set takes (center, eye)) */

    BOOL MailCreateInit(cXyz*, cXyz*);
    static u8 getNextNo(u8 previous_no) { return Btsw_getNextNo(previous_no); }
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
    void draw(void* tevStr);
};
WWHD_OFFSET(SwMail2_c, mPos, 0x80);
WWHD_OFFSET(SwMail2_c, mpCamEye, 0xB8);
WWHD_SIZE(SwMail2_c, 0xBC);

/* SwCam2_c (0x20) */
struct SwCam2_c {
    /* 0x00 */ cXyz mCenter;  /* was field_0x00: the camera's centre: daNpc_Btsw_c_ct 022186EC / talk01 0221ACDC load it from camera_center_data[0][1]; SwCam2_c::Move 0221949C eases it to camera_center_data[mAimY][mAimX + 1] and passes it as dCamera_c::Set's first argument, the center (02514F88, tww d_camera.cpp Set(cXyz center, cXyz eye, ...)) (tww the same; tww's accessor getEyeP() returns it, its getEyeP / getCenterP names are swapped) */
    /* 0x0C */ cXyz mEye;  /* was field_0x0C: the camera's eye: daNpc_Btsw_c_ct 022186EC / talk01 0221ACDC load it from camera_eye; SwCam2_c::Move 0221949C passes it as dCamera_c::Set's second argument, the eye (02514F88, tww d_camera.cpp Set(cXyz center, cXyz eye, ...)) (tww the same; tww's accessor getCenterP() returns it, its getEyeP / getCenterP names are swapped) */
    /* 0x18 */ be<f32> mFovy;  /* was field_0x18: talk01 0221ACDC 58.0; SwCam2_c::Move 0221949C passes it to the camera Set as the field of view (tww the same) */
    /* 0x1C */ be<s8> mAimX;  /* was field_0x1C: shiwake_game_action 0221B3A8 sets it to mCursorX (tww setAimIdx); SwCam2_c::Move 0221949C picks the camera_center_data column + 1 by it; talk01 0221ACDC 0 */
    /* 0x1D */ be<s8> mAimY;  /* was field_0x1D: shiwake_game_action 0221B3A8 sets it to mCursorY (tww setAimIdx); SwCam2_c::Move 0221949C picks the camera_center_data row by it; talk01 0221ACDC 0 */
    /* 0x1E */ be<u8> mbActive;  /* was field_0x1E: talk01 0221ACDC sets 1 (tww ActiveOn) at the game-start message; shiwake_game_action 0221B3A8 0 at the end (ActiveOff); SwCam2_c::Move 0221949C drives the camera only while it is set (tww the same) */
    /* 0x1F */ u8 _1F;
    void Move();
};
WWHD_SIZE(SwCam2_c, 0x20);

/* STControl (HD 0x28: GameCube members 0x04..0x28 at 0x00, the vtable at +0x24) */
struct BtswSTControl {
    /* 0x00 */ u8 _00[0x24];
    /* 0x24 */ be<u32> __vtbl;
};
WWHD_SIZE(BtswSTControl, 0x28);

struct BtswLightDefaultBlock {
    be<f32> mVectors[6];
    be<u8> mColor[4];
    be<s16> mSignedColor[4];
    be<f32> mFactors[8];
};
WWHD_SIZE(BtswLightDefaultBlock, 0x44);
// HD: Baito reserves a 0x228-byte letter light object and copies three default blocks.
struct BtswLetterLight : dKy_tevstr_c {
    u8 pad1C8[0x60];
    BtswLightDefaultBlock& defaultBlock(u32 index) {
        u32 offset = index == 0 ? 0 : index == 1 ? 0xC0 : 0x144;
        return gabi::object_at<BtswLightDefaultBlock>(gabi::ea(this) + offset);
    }
    void copyDefaults(const BtswLightDefaultBlock& source) {
        for (u32 i = 0; i < 6; ++i)
            for (u32 copy = 0; copy < 3; ++copy)
                defaultBlock(copy).mVectors[i] = source.mVectors[i].get();
        for (u32 i = 0; i < 4; ++i)
            for (u32 copy = 0; copy < 3; ++copy)
                defaultBlock(copy).mColor[i] = source.mColor[i].get();
        for (u32 i = 0; i < 4; ++i)
            for (u32 copy = 0; copy < 3; ++copy)
                defaultBlock(copy).mSignedColor[i] = source.mSignedColor[i].get();
        for (u32 i = 0; i < 8; ++i)
            for (u32 copy = 0; copy < 3; ++copy)
                defaultBlock(copy).mFactors[i] = source.mFactors[i].get();
    }
};
WWHD_SIZE(BtswLetterLight, 0x228);

struct daNpc_Btsw_c : fopNpc_npc_c {
    enum { ACTION_STARTING = 0, ACTION_ONGOING = 1, ACTION_UNK_2 = 2, ACTION_ENDING = -1 };

    /* 0x7DC */ be<s8> m_handL;
    /* 0x7DD */ be<s8> m_handR;
    /* 0x7DE */ u8 _7DE[2];
    /* 0x7E0 */ u8 mPhs[8];
    /* 0x7E8 */ gptr<J3DModel> mpLetterModel;  /* was field_0x6D0: CreateHeap 022182F4 creates it from the letter model data; set_mtx 02218958 places it; _draw 02219840 lights it with mLetterTevStr and draws it (tww the same) */
    /* 0x7EC */ BtswLetterLight mLetterTevStr;  /* was field_0x6D4: CreateInit 02218D58 dKy_tevstr_init; _draw 02219840 sets it up for the actor, overrides mColorK0 / mColorC0 and lights mpLetterModel with it (tww the same) */
    /* 0xA14 */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0xA18 */ mDoExt_btpAnm mBtpAnm;  /* was field_0x7E8: initTexPatternAnm 02217C94 sets it up with m_head_tex_pattern; _draw 02219840 enters it at mBtpFrame and removes it (tww the same) */
    /* 0xA8C */ be<u8> mBtpFrame;  /* was field_0x7FC: initTexPatternAnm 02217C94 0; playTexPatternAnm 0221914C steps it through m_head_tex_pattern's frames once mBlinkTimer runs out; _draw 02219840 enters mBtpAnm at it (tww the same); same role as daNpc_Btsw2_c::mBtpFrame */
    /* 0xA8D */ u8 _A8D[3];
    /* 0xA90 */ SwMail2_c mSwMail0, mSwMail1, mSwMail2;
    /* 0xCC4 */ gptr<SwMail2_c> mpMails[3];  /* was field_0x920: the three sorting-game letters (mSwMail0-2): shiwake_game_action 0221B3A8 inits, appears, throws and moves them; checkNextMailThrowOK 0221B300 tests the next one's proc; _draw 02219840 draws them while mbDrawMails is set (tww the same) */
    /* 0xCD0 */ be<u8> mMailIdx;  /* was field_0x92C: shiwake_game_action 0221B3A8 0 at the start, throws mpMails[it] and moves on to the next (0-2); checkNextMailThrowOK 0221B300 looks one past it (tww the same) */
    /* 0xCD1 */ u8 _CD1[3];
    /* 0xCD4 */ SwCam2_c mSwCam;
    /* 0xCF4 */ be<s16> field_0x950;
    /* 0xCF6 */ be<u8> mbTimerCreated;  /* was field_0x952: shiwake_game_action 0221B3A8 creates the game timer (and starts the minigame) only while it is 0, then sets 1; 0 at the action's end and in CreateInit 02218D58 (tww the same) */
    /* 0xCF7 */ be<u8> mbDrawMails;  /* was field_0x953: shiwake_game_action 0221B3A8 sets 1 when the game starts and 0 at the action's end; _draw 02219840 draws mpMails only while it is set; CreateInit 02218D58 0 (tww the same) */
    /* 0xCF8 */ be<s16> mBlinkTimer;  /* was field_0x954: initTexPatternAnm 02217C94 0; playTexPatternAnm 0221914C holds the head pattern while it counts down and restarts it at 30-130 (tww the same); same role as daNpc_Ba1_c::mBlinkTimer */
    /* 0xCFA */ u8 _CFA[2];
    /* 0xCFC */ be<u32> mpShopCursor;
    /* 0xD00 */ cXyz mAttPos;
    /* 0xD0C */ u8 _D0C[2];
    /* 0xD0E */ be<s16> mLookAtMaxVel;  /* was field_0x96A: lookBack 0221AA40: cLib_addCalcAngleS2 moves it toward the HIO mMaxHeadTurnVel while turning (else 0) and passes it as the look-at speed (tww the same); same code as daNpc_Ba1_c::mLookAtMaxVel */
    /* 0xD10 */ u8 _D10[2];
    /* 0xD12 */ csXyz field_0x96E; /* GameCube field_0x96E */
    /* 0xD18 */ u8 _D18[4];
    /* 0xD1C */ BtswSTControl mStickControl;  /* was field_0x978: the STControl: CreateInit 02218D58 / _execute 02219618 setWaitParm on it (tww shiwake_game_action reads the cursor triggers from it; WWHD reads the pad trigger words instead) */
    /* 0xD44 */ be<s8> mCursorX;  /* was field_0x9A0: shiwake_game_action 0221B3A8: the sorting cursor's column (-1..1), 0 at the start, moved left / right, picks the cursor and aim position and the box number (tww the same) */
    /* 0xD45 */ be<s8> mCursorY;  /* was field_0x9A1: shiwake_game_action 0221B3A8: the sorting cursor's row (0..1), 0 at the start, moved up / down (tww the same) */
    /* 0xD46 */ u8 _D46[2];
    /* 0xD48 */ be<s32> mNextTimerSeMs;  /* was field_0x9A4: TimerCountDown 0221B254: plays the countdown sound once the game timer's rest time drops below it, then lowers it by 1000 (10000 above 10 s); shiwake_game_action 0221B3A8 20000 at the start (tww the same) */
    /* 0xD4C */ be<u8> mHasAttention;
    /* 0xD4D */ be<u8> mTalking;  /* was field_0x9A9: checkOrder 02219210 sets 1 on the talk command; wait01 0221ABCC switches mMode to the talk (2) while it is set; talk01 0221ACDC clears it when the talk ends (tww the same); same code as daNpc_Btsw2_c::mTalking */
    /* 0xD4E */ u8 _D4E[6];
    /* 0xD54 */ be<u32> mPendingMsgNo;  /* was field_0x9B0: shiwake_game_action 0221B3A8 stores the game-end message; getMsg 0221A840 returns it once instead of the normal message and resets it to fpcM_ERROR_PROCESS_ID_e; CreateInit 02218D58 the same (tww the same); same role as daNpc_Ds1_c::mPendingMsgNo */
    /* 0xD58 */ be<s8> field_0x9B4;
    /* 0xD59 */ u8 _D59[3];
    /* 0xD5C */ BtswActionFunc mCurrActionFunc;
    /* 0xD64 */ be<s8> mBtpIdx;  /* was field_0x9C4: CreateHeap 022182F4 0; tww initTexPatternAnm loads l_btp_ix_tbl[it] (WWHD's initTexPatternAnm 02217C94 always takes entry 0); same as daNpc_Btsw2_c::mBtpIdx */
    /* 0xD65 */ be<s8> mAnmIdx;  /* was field_0x9C5: setAnm 0221A3CC skips a repeat, stores the animation number and plays its table entries; anmAtr 0221A960 tests it (tww the same); same name as daNpc_Btsw2_c::mAnmIdx */
    /* 0xD66 */ be<s8> mIdleLoopCount;  /* was field_0x9C6: wait01 0221ABCC counts it down at each end of the wait animation and switches between the idle animations 9 / 10 at 0 (reloads 1 or 2-5); talk01 0221ACDC / wait_action 0221AF2C 1 (tww the same) */
    /* 0xD67 */ be<s8> mOrderEvent;  /* was field_0x9C7: eventOrder 022195A4: 1 / 2 the talk (1 orders the speak event), 3 GETMOTHERLETTER, 4 mEventIdx; checkOrder 02219210 clears it when those start (tww the same); same code as daNpc_Btsw2_c::mOrderEvent */
    /* 0xD68 */ be<s16> mEventIdx;  /* was field_0x9C8: shiwake_game_action 0221B3A8 BTSW_TALK_SUGOI's index (-1 otherwise); eventOrder 022195A4 orders it at mOrderEvent 4; dummy_event_action 0221B03C waits for its end; CreateInit 02218D58 -1 (tww the same) */
    /* 0xD6A */ be<s8> mMode;  /* was field_0x9CA: wait_action 0221AF2C runs wait01 at 1, talk01 at 2; wait01 0221ABCC sets 2, talk01 0221ACDC 1, shiwake_game_action 3; lookBack 0221AA40 looks at Link at 1 / 2 (tww the same); same code as daNpc_Btsw2_c::mMode */
    /* 0xD6B */ u8 _D6B[2];
    /* 0xD6D */ be<s8> mActionStatus;
    /* 0xD6E */ u8 _D6E[2];

    BOOL initTexPatternAnm(u32 i_modify); /* bool, passed as the full register */
    void playTexPatternAnm();
    void setAnm(s8);
    bool chkAttention(cXyz* i_pos, s16 i_angle); /* cXyz by value: GHS passes a pointer to a copy */
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
    BOOL dummy_event_action(void*);
    BOOL checkNextMailThrowOK();
    void TimerCountDown();
    BOOL shiwake_game_action(void*);
    BOOL getdemo_action(void*);
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL CreateHeap();
};
WWHD_OFFSET(daNpc_Btsw_c, mpLetterModel, 0x7E8);
WWHD_OFFSET(daNpc_Btsw_c, m_head_tex_pattern, 0xA14);
WWHD_OFFSET(daNpc_Btsw_c, mSwMail0, 0xA90);
WWHD_OFFSET(daNpc_Btsw_c, mSwMail1, 0xB4C);
WWHD_OFFSET(daNpc_Btsw_c, mSwMail2, 0xC08);
WWHD_OFFSET(daNpc_Btsw_c, mpMails, 0xCC4);
WWHD_OFFSET(daNpc_Btsw_c, mSwCam, 0xCD4);
WWHD_OFFSET(daNpc_Btsw_c, mpShopCursor, 0xCFC);
WWHD_OFFSET(daNpc_Btsw_c, mStickControl, 0xD1C);
WWHD_OFFSET(daNpc_Btsw_c, mPendingMsgNo, 0xD54);
WWHD_OFFSET(daNpc_Btsw_c, mCurrActionFunc, 0xD5C);
WWHD_OFFSET(daNpc_Btsw_c, mActionStatus, 0xD6D);
WWHD_SIZE(daNpc_Btsw_c, 0xD70);

// HD: the embedded NPC parameters precede the derived vtable.
struct daNpc_Btsw_HIO_c {
    be<s8> mNo;
    u8 pad01[3];
    dNpc_HIO_c mNpc;
    /* mTimerLimit (+0x2C) was field_0x30: shiwake_game_action 0221B3A8 creates the game timer with it (dTimer_createTimer's u16 time; HIO default 30, tww the same) */
    be<s16> mTimerLimit, field_0x32, field_0x34;
    be<s16> r_1, g_1, b_1, r_2, g_2, b_2;
    u8 pad3E[2];
    /* mMailThrowSpeed (+0x40) was field_0x44: SwMail2_c::Throw 02219F20: cLib_addCalcPos maxStep of the flying letter (tww the same); default 40.0 */
    be<f32> mMailThrowSpeed, field_0x48, field_0x4C, field_0x50, field_0x54;
    be<u8> field_0x58;
    u8 pad55[3];
    /* mAnm56MorfFrames (+0x58) was field_0x5C: setAnm 0221A3CC morphs with it instead of a_morf_frame_tbl when the current animation is 5 or 6 (tww the same); default 10.0 */
    be<f32> mAnm56MorfFrames;
    be<u32> __vtbl;
};
WWHD_SIZE(daNpc_Btsw_HIO_c, 0x60);
WWHD_OFFSET(daNpc_Btsw_HIO_c, mTimerLimit, 0x2C);
WWHD_OFFSET(daNpc_Btsw_HIO_c, mMailThrowSpeed, 0x40);
WWHD_OFFSET(daNpc_Btsw_HIO_c, __vtbl, 0x5C);

/* addresses of this unit */
#define BTSW_wait_action 0x0221AF2Cu
#define BTSW_dummy_event_action 0x0221B03Cu
#define BTSW_shiwake_game_action 0x0221B3A8u
#define BTSW_getdemo_action 0x0221BE68u
#define BTSW_MAIL_Dummy 0x02219B20u
#define BTSW_MAIL_Appear 0x02219C1Cu
#define BTSW_MAIL_Wait 0x02219D88u
#define BTSW_MAIL_Throw 0x02219F20u
#define BTSW_MAIL_End 0x0221A1E0u
#define BTSW_SAFESTRING_VTBL 0x100186D0u /* this TU's sead::SafeString vtable */
#define BTSW_VTBL 0x100189D0u            /* daNpc_Btsw_c vtable (HD virtual destructor) */
#define BTSW_HIO 0x10466958u             /* l_HIO (HD 0x60: GameCube members at -4, vtable +0x5C) */
#define BTSW_ARC 0x10018898u             /* "Btsw" (each use has its own literal) */

// Unit-local ABI bindings preserve the HD register widths.
f32 Btsw_cLib_addCalcPos(u32 a0, cXyz* a1, f32 a2, f32 a3, f32 a4);
f32 Btsw_cLib_addCalcPos(u32 a0, u32 a1, f32 a2, f32 a3, f32 a4);
void Btsw_cLib_addCalcPos2(u32 a0, cXyz* a1, f32 a2, f32 a3);
void Btsw_cLib_addCalcAngleS2(u32 a0, s32 a1, s32 a2, s32 a3);
s16 Btsw_cLib_targetAngleY(u32 a0, u32 a1);
s16 Btsw_cLib_targetAngleX(u32 a0, u32 a1);
s16 Btsw_cM_atan2s(f32 a0, f32 a1);
f32 Btsw_cM_rndF(f32 a0);
void Btsw_cXyz___mi(u32 a0, cXyz* a1, u32 a2);
void Btsw_cXyz___ml(cXyz* a0, cXyz* a1, f32 a2);
s32 Btsw_cXyz_normalizeRS(cXyz* a0);
void Btsw_SwCam2_c_Move(u32 a0);
u8 Btsw_daNpc_Btsw_c_chkAttention(u32 a0, cXyz* a1, s32 a2);
void Btsw_daNpc_Btsw_HIO_c_daNpc_Btsw_HIO_c(u32 a0);
u32 Btsw_dBgS_GetPolyColor(u32 a0, u32 a1);
u32 Btsw_dBgS_GetRoomId(u32 a0, u32 a1);
void Btsw_dBgS_AcchCir_SetWall(u32 a0, f32 a1, f32 a2);
void Btsw_dBgS_Acch_Set(u32 a0, u32 a1, u32 a2, u32 a3, s32 a4, u32 a5, u32 a6, u32 a7, u32 a8);
void Btsw_dBgS_Acch_CrrPos(u32 a0, u32 a1);
void Btsw_dCamera_c_Set(u32 a0, cXyz* a1, cXyz* a2, f32 a3, s32 a4);
void Btsw_dCcD_Cyl_destroy(u32 a0, s32 a1);
void Btsw_dCcD_Stts_Init(u32 a0, s32 a1, s32 a2, u32 a3);
void Btsw_dCcD_Cyl_Set(u32 a0, u32 a1);
s32 Btsw_dComIfG_resLoad(u32 a0, u32 a1);
void Btsw_dComIfG_resDelete(u32 a0, u32 a1);
u32 Btsw_dEvent_manager_c_getMyStaffId(u32 a0, u32 a1, s32 a2, s32 a3);
void Btsw_dEvent_manager_c_cutEnd(u32 a0, u32 a1);
u32 Btsw_dEvent_manager_c_getEventIdx(u32 a0, u32 a1, s32 a2);
u32 Btsw_dEvent_manager_c_endCheck(u32 a0, s32 a1);
u32 Btsw_dEvent_manager_c_endCheckOld(u32 a0, u32 a1);
u32 Btsw_dEvent_manager_c_ChkPresentEnd(u32 a0);
u32 Btsw_dKy_getEnvlight();
void Btsw_dKy_tevstr_init(u32 a0, s32 a1, s32 a2);
void Btsw_dScnKy_env_light_c_settingTevStruct(u32 a0, s32 a1, u32 a2, u32 a3);
u32 Btsw_dLetter_isNoSend(s32 a0);
void Btsw_STControl_setWaitParm(u32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, f32 a7,
                                f32 a8);
void Btsw_STControl_init(u32 a0);
void Btsw_dNpc_setAnmIDRes(u32 a0, u32 a1, f32 a2, f32 a3, u32 a4, s32 a5, u32 a6);
void Btsw_dNpc_playerEyePos(cXyz* a0, f32 a1);
void Btsw_dNpc_JntCtrl_c_lookAtTarget(u32 a0, u32 a1, u32 a2, cXyz* a3, s32 a4, s32 a5, s32 a6);
void Btsw_dNpc_JntCtrl_c_setParam(u32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7,
                                  s32 a8, s32 a9);
void Btsw_dNpc_EventCut_c_setActorInfo2(u32 a0, u32 a1, u32 a2);
u32 Btsw_dNpc_EventCut_c_cutProc(u32 a0);
u32 Btsw_fopNpc_npc_c_talk(u32 a0, s32 a1);
void Btsw_fopNpc_construct(u32 a0);
void Btsw_fopNpc_npc_c_setCollision(u32 a0, f32 a1, f32 a2);
u32 Btsw_dSv_player_collect_c_isSymbol(u32 a0, s32 a1);
void Btsw_dSv_event_c_setEventReg(u32 a0, s32 a1, s32 a2);
u32 Btsw_dSv_event_c_getEventReg(u32 a0, s32 a1);
u32 Btsw_ShopCursor_create(u32 a0, u32 a1, f32 a2);
void Btsw_ShopCursor_c_anm_play(u32 a0);
void Btsw_ShopCursor_c_draw(u32 a0);
void Btsw_dSnap_RegistFig(s32 a0, u32 a1, u32 a2, s32 a3, f32 a4, f32 a5, f32 a6);
void Btsw_dTimer_c_end(u32 a0, s32 a1);
void Btsw_dTimer_c_deleteRequest(u32 a0);
void Btsw_dTimer_c_start(u32 a0, s32 a1);
void Btsw_dVibration_c_StartShock(u32 a0, s32 a1, s32 a2, cXyz* a3);
void Btsw_fopAc_ac_c_fopAc_ac_c(u32 a0, s32 a1);
s32 Btsw_fopAcM_entrySolidHeap(u32 a0, u32 a1, s32 a2);
void Btsw_fopAcM_posMoveF(u32 a0, u32 a1);
void Btsw_fopAcM_orderSpeakEvent(daNpc_Btsw_c* a0);
void Btsw_fopAcM_orderOtherEvent2(daNpc_Btsw_c* a0, u32 a1, s32 a2, s32 a3);
void Btsw_fopAcM_orderOtherEvent2(u32 a0, u32 a1, s32 a2, s32 a3);
void Btsw_fopAcM_orderOtherEventId(daNpc_Btsw_c* a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5);
void Btsw_JAIZelBasic_seDeleteObject(u32 a0);
void Btsw_mDoExt_modelUpdateDL(u32 a0, s32 a1);
u32 Btsw_mDoExt_J3DModel__create(u32 a0, s32 a1, s32 a2);
u32 Btsw_mDoExt_McaMorf_mDoExt_McaMorf(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, s32 a5, s32 a6,
                                       s32 a7, s32 a8, s32 a9, s32 a10, s32 a11, f32 a12);
void Btsw_mDoExt_McaMorf_play(u32 a0, u32 a1, s32 a2, s32 a3);
void Btsw_mDoExt_McaMorf_entryDL(u32 a0);
void Btsw_mDoExt_McaMorf_calc(u32 a0);
void Btsw_mDoExt_McaMorf_stopZelAnime(u32 a0);
void Btsw_mDoExt_btpAnm_entry(u32 a0, u32 a1, u32 a2);
u8 Btsw_mDoHIO_subRoot_c_createChild(u32 a0, u32 a1);
void Btsw_mDoHIO_subRoot_c_deleteChild(s32 a0);
void Btsw_mDoMtx_XYZrotM(u32 a0, s32 a1, s32 a2, s32 a3);
void Btsw_mDoMtx_ZXYrotS(u32 a0, s32 a1, s32 a2, s32 a3);
void Btsw_mDoMtx_ZXYrotM(u32 a0, s32 a1, s32 a2, s32 a3);
void Btsw_mDoMtx_XrotM(u32 a0, s32 a1);
void Btsw_mDoMtx_YrotM(u32 a0, s32 a1);
void Btsw_mDoMtx_ZrotM(u32 a0, s32 a1);
void Btsw_mDoMtx_stack_c_transM(f32 a0, f32 a1, f32 a2);
u32 Btsw_dRes_control_c_getIDRes(u32 a0, SafeString* a1, s32 a2);
void Btsw_JUTAssertion_showAssert(u32 a0, s32 a1, u32 a2);
void Btsw_dl(u32 a0);
void Btsw_dl(void* a0);
u32 Btsw_J3DFrameCtrl_checkPass(u32 a0, f32 a1);
u32 Btsw_J3DModelData_jointTree(u32 a0);
void Btsw_PSVECAdd(cXyz* a0, cXyz* a1, cXyz* a2);
void Btsw_PSVECAdd(cXyz* a0, u32 a1, cXyz* a2);
void Btsw_PSVECAdd(u32 a0, cXyz* a1, u32 a2);
void Btsw_PSVECAdd(u32 a0, u32 a1, u32 a2);
void Btsw_PSVECScale(u32 a0, u32 a1, f32 a2);
void Btsw_PSMTXMultVec(u32 a0, u32 a1, u32 a2);
void Btsw_PSMTXMultVec(u32 a0, cXyz* a1, cXyz* a2);
void Btsw_PSMTXCopy(u32 a0, Mtx34* a1);
void Btsw_PSMTXCopy(u32 a0, u32 a1);
void Btsw_PSMTXConcat(u32 a0, Mtx34* a1, u32 a2);
void Btsw_PSMTXTrans(u32 a0, f32 a1, f32 a2, f32 a3);
void Btsw_register_global_object(u32 a0);

u32 Btsw_padCheckTriggerA(s32 a0);
void Btsw_cM3dGCir_destroy(u32 a0, s32 a1);
u32 Btsw_cLib_calcTimer(u32 a0);
void Btsw_actorConstruct(u32 a0);
void Btsw_SwMail_SeDelete(u32 a0);
void Btsw_dBgS_Acch_destroy(u32 a0, s32 a1);
void Btsw_cameraStay(u32 a0);
void Btsw_dCcD_Stts_destroy(u32 a0, s32 a1);
void Btsw_setLightTevColorType(u32 a0, u32 a1, u32 a2);
void Btsw_setLightTevColorType(u32 a0, u32 a1, void* a2);
u32 Btsw_dLetter_isRead(s32 a0);
void Btsw_STControl_destroy(u32 a0, s32 a1);
void Btsw_dNpc_HIO_construct(u32 a0);
void Btsw_ShopCursor_setPos(u32 a0, u32 a1);
void Btsw_timerStart(u32 a0);
void Btsw_dTimer_createTimer(s32 a0, u32 a1, s32 a2, s32 a3, f32 a4, f32 a5, f32 a6, f32 a7);
u32 Btsw_timerIsDelete(u32 a0);
void Btsw_seStart(u32 a0);
void Btsw_seStartAt(s32 a0, u32 a1);
void Btsw_btpConstruct(u32 a0);
s32 Btsw_btpInit(u32 a0, u32 a1, u32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8, f32 a9);
s32 Btsw_btpInit(u32 a0, u32 a1, u32 a2, s32 a3, s32 a4, s32 a5, s32 a6, u32 a7, s32 a8, f32 a9);
void Btsw_padSetRepeat(u32 a0, s32 a1, s32 a2);
void Btsw_padResetRepeat(u32 a0);
void Btsw_endMiniGame(u32 a0);
void Btsw_startMiniGame(u32 a0);
u32 Btsw_allocate(s32 a0);
s32 Btsw_jointNameIndex(u32 a0, u32 a1);
u32 Btsw_jointNameHeader(u32 a0);
u32 Btsw_jointNumber(J3DNode* a0);
void Btsw_csXyzConstruct(u32 a0);
void Btsw_cXyzConstruct(u32 a0);
f32 Btsw_squareRoot(f32 a0);
void Btsw_copyDebugColor(u32 a0, u32 a1, s32 a2);

// HD work views used where this actor deliberately retains a cached guest address.
struct BtswPadState {
    u8 pad00[0x18];
    be<u32> mTrigger;
    u8 pad1C[4];
    be<u32> mRepeatTrigger;
    u8 pad24[0x124 - 0x24];
    be<u32> mHold;
};
WWHD_OFFSET(BtswPadState, mTrigger, 0x18);
WWHD_OFFSET(BtswPadState, mRepeatTrigger, 0x20);
WWHD_OFFSET(BtswPadState, mHold, 0x124);
WWHD_SIZE(BtswPadState, 0x128);
struct BtswTimerState {
    u8 pad00[0x124];
    be<u8> mStatus;
};
WWHD_OFFSET(BtswTimerState, mStatus, 0x124);
struct BtswMiniGameSystem {
    u8 pad00[0x178];
    be<u32> mpGame;
};
WWHD_OFFSET(BtswMiniGameSystem, mpGame, 0x178);
struct BtswMessageState {
    u8 pad00[0x948];
    be<u32> mSelection;
};
WWHD_OFFSET(BtswMessageState, mSelection, 0x948);
struct BtswJointNumber {
    u8 pad00[4];
    be<u16> mNumber;
};
WWHD_OFFSET(BtswJointNumber, mNumber, 4);
struct BtswJointTree {
    u8 pad00[8];
    be<u16> mCount;
};
WWHD_OFFSET(BtswJointTree, mCount, 8);
struct BtswNameHeader {
    u8 pad00[0x10];
    be<u32> mNameOffset;
};
WWHD_OFFSET(BtswNameHeader, mNameOffset, 0x10);
struct BtswModelDataWork : J3DModelData {
    u8 pad14[0x38 - 0x14];
    be<u32> mpTexPatternAnimation;
};
WWHD_OFFSET(BtswModelDataWork, mpTexPatternAnimation, 0x38);
struct BtswVectorWords {
    be<u32> x, y, z;
};
WWHD_SIZE(BtswVectorWords, 12);

// HD: virtual PMFs use eight-byte vtable entries and the function word's low half as offset.
struct BtswVirtualEntry {
    be<u32> mAdjustment, mFunction;
};
WWHD_SIZE(BtswVirtualEntry, 8);
inline s16 Btsw_virtualOffset(const BtswActionFunc& action) {
    return gabi::object_at<be<s16>>(gabi::ea(&action.f) + 2).get();
}
inline u32 Btsw_virtualTarget(u32 receiver, s16 offset, s16 index) {
    u32 table = gabi::object_at<be<u32>>(receiver + u32(s32(offset))).get();
    return gabi::object_at<BtswVirtualEntry>(table + u32(s32(index) * 8)).mFunction;
}
struct BtswPatternVtable {
    u8 pad00[0x14];
    be<u32> mGetFrameMax;
};
struct BtswPatternWork {
    u8 pad00[4];
    gptr<BtswPatternVtable> mpVtable;
};
WWHD_OFFSET(BtswPatternWork, mpVtable, 4);
WWHD_OFFSET(BtswPatternVtable, mGetFrameMax, 0x14);
struct BtswAcchVtables {
    u8 pad00[0x14];
    be<u32> mVtableA;
    u8 pad18[8];
    be<u32> mVtableB;
};
WWHD_OFFSET(BtswAcchVtables, mVtableA, 0x14);
WWHD_OFFSET(BtswAcchVtables, mVtableB, 0x20);

struct BtswPositionTable {
    cXyz mPositions[2][3];
    cXyz& entry(s32 row, s32 column) {
        // HD: preserve 32-bit guest address wrapping for unconstrained signed indices.
        return gabi::object_at<cXyz>(gabi::ea(this) + u32(row * 3 + column) * 12);
    }
};
WWHD_SIZE(BtswPositionTable, 0x48);

WWHD_SIZE(BtswTimerState, 0x125);
WWHD_SIZE(BtswMiniGameSystem, 0x17C);
WWHD_SIZE(BtswMessageState, 0x94C);
WWHD_SIZE(BtswJointNumber, 6);
WWHD_SIZE(BtswJointTree, 0xA);
WWHD_SIZE(BtswNameHeader, 0x14);
WWHD_SIZE(BtswModelDataWork, 0x3C);
WWHD_SIZE(BtswPatternWork, 8);
WWHD_SIZE(BtswPatternVtable, 0x18);
WWHD_SIZE(BtswAcchVtables, 0x24);
