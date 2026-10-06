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
#include "bindings.h"

/* GHS pointer to member function (8 bytes): {s16 this delta, s16 vtable index (0 NULL, < 0 plain),
 * u32 function (its low half is the vtable offset for virtual members)} */
struct BtswPmf_l {
    /* 0x0 */ be<s16> d;
    /* 0x2 */ be<s16> i;
    /* 0x4 */ be<u32> f;
};
WWHD_SIZE(BtswPmf_l, 8);

/* SwMail2_c: one letter of the sorting game (HD 0xBC) */
struct SwMail2_c {
    /* 0x00 */ BtswPmf_l mFunc;
    /* 0x08 */ gptr<J3DModel> mpModel;
    /* 0x0C */ u8 mBtpAnm[0x74];          /* mDoExt_btpAnm (GameCube field_0x10) */
    /* 0x80 */ cXyz mPos;                 /* GameCube field_0x24 */
    /* 0x8C */ cXyz mSpeed;               /* GameCube field_0x30 (throw target, then velocity) */
    /* 0x98 */ cXyz mOffset;              /* GameCube field_0x3C */
    /* 0xA4 */ csXyz mAngle;              /* GameCube field_0x48 */
    /* 0xAA */ csXyz mBaseAngle;          /* GameCube field_0x4E */
    /* 0xB0 */ be<u8> mNo;                /* GameCube field_0x54: the letter's box (texture frame) */
    /* 0xB1 */ be<u8> mAimNo;             /* GameCube field_0x55: the box it was thrown at */
    /* 0xB2 */ be<u8> mStep;              /* GameCube field_0x56 */
    /* 0xB3 */ u8 _B3;
    /* 0xB4 */ gptr<cXyz> mpEye;          /* GameCube field_0x58 */
    /* 0xB8 */ gptr<cXyz> mpCenter;       /* GameCube field_0x5C */

    BOOL MailCreateInit(cXyz*, cXyz*);
    static u8 getNextNo(u8);
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
WWHD_OFFSET(SwMail2_c, mpCenter, 0xB8);
WWHD_SIZE(SwMail2_c, 0xBC);

/* SwCam2_c (0x20) */
struct SwCam2_c {
    /* 0x00 */ cXyz field_0x00;           /* getEyeP() */
    /* 0x0C */ cXyz field_0x0C;           /* getCenterP() */
    /* 0x18 */ be<f32> mFovy;
    /* 0x1C */ be<s8> mAimX;
    /* 0x1D */ be<s8> mAimY;
    /* 0x1E */ be<u8> mActive;
    /* 0x1F */ u8 _1F;
    void Move();
};
WWHD_SIZE(SwCam2_c, 0x20);

/* STControl (HD 0x28: GameCube members 0x04..0x28 at 0x00, the vtable at +0x24) */
struct BtswSTControl_l {
    /* 0x00 */ u8 _00[0x24];
    /* 0x24 */ be<u32> __vtbl;
};
WWHD_SIZE(BtswSTControl_l, 0x28);

struct daNpc_Btsw_c : fopNpc_npc_c {
    enum { ACTION_STARTING = 0, ACTION_ONGOING = 1, ACTION_UNK_2 = 2, ACTION_ENDING = -1 };

    /* 0x7DC */ be<s8> m_handL;
    /* 0x7DD */ be<s8> m_handR;
    /* 0x7DE */ u8 _7DE[2];
    /* 0x7E0 */ u8 mPhs[8];
    /* 0x7E8 */ gptr<J3DModel> mpLetterModel;   /* GameCube field_0x6D0 */
    /* 0x7EC */ u8 mLetterTevStr[0xA14 - 0x7EC]; /* dKy_tevstr_c (GameCube field_0x6D4) */
    /* 0xA14 */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0xA18 */ u8 mBtpAnm[0x74];               /* GameCube field_0x7E8 */
    /* 0xA8C */ be<u8> mBtpFrame;               /* GameCube field_0x7FC */
    /* 0xA8D */ u8 _A8D[3];
    /* 0xA90 */ SwMail2_c mSwMail[3];
    /* 0xCC4 */ gptr<SwMail2_c> mpMail[3];      /* GameCube field_0x920 */
    /* 0xCD0 */ be<u8> mMailIdx;                /* GameCube field_0x92C */
    /* 0xCD1 */ u8 _CD1[3];
    /* 0xCD4 */ SwCam2_c mSwCam;
    /* 0xCF4 */ be<s16> field_0x950;
    /* 0xCF6 */ be<u8> mTimerCreated;           /* GameCube field_0x952 */
    /* 0xCF7 */ be<u8> mMailActive;             /* GameCube field_0x953 */
    /* 0xCF8 */ be<s16> mBlinkTimer;            /* GameCube field_0x954 */
    /* 0xCFA */ u8 _CFA[2];
    /* 0xCFC */ be<u32> mpShopCursor;
    /* 0xD00 */ cXyz mAttPos;
    /* 0xD0C */ u8 _D0C[2];
    /* 0xD0E */ be<s16> mHeadTurnVel;           /* GameCube field_0x96A */
    /* 0xD10 */ u8 _D10[2];
    /* 0xD12 */ csXyz mHomeAngle;               /* GameCube field_0x96E */
    /* 0xD18 */ u8 _D18[4];
    /* 0xD1C */ BtswSTControl_l mStick;         /* GameCube field_0x978 */
    /* 0xD44 */ be<s8> mCursorX;                /* GameCube field_0x9A0 */
    /* 0xD45 */ be<s8> mCursorY;                /* GameCube field_0x9A1 */
    /* 0xD46 */ u8 _D46[2];
    /* 0xD48 */ be<s32> mNextTimerSe;           /* GameCube field_0x9A4 */
    /* 0xD4C */ be<u8> mHasAttention;
    /* 0xD4D */ be<u8> mTalkAccepted;           /* GameCube field_0x9A9 */
    /* 0xD4E */ u8 _D4E[6];
    /* 0xD54 */ be<u32> mNextMsgNo;             /* GameCube field_0x9B0 */
    /* 0xD58 */ be<s8> field_0x9B4;
    /* 0xD59 */ u8 _D59[3];
    /* 0xD5C */ BtswPmf_l mCurrActionFunc;
    /* 0xD64 */ be<s8> mTexIdx;                 /* GameCube field_0x9C4 */
    /* 0xD65 */ be<s8> mAnm;                    /* GameCube field_0x9C5 */
    /* 0xD66 */ be<s8> mWaitCount;              /* GameCube field_0x9C6 */
    /* 0xD67 */ be<s8> mOrder;                  /* GameCube field_0x9C7 */
    /* 0xD68 */ be<s16> mEventIdx;              /* GameCube field_0x9C8 */
    /* 0xD6A */ be<s8> mMode;                   /* GameCube field_0x9CA */
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
WWHD_OFFSET(daNpc_Btsw_c, mSwMail, 0xA90);
WWHD_OFFSET(daNpc_Btsw_c, mpMail, 0xCC4);
WWHD_OFFSET(daNpc_Btsw_c, mSwCam, 0xCD4);
WWHD_OFFSET(daNpc_Btsw_c, mpShopCursor, 0xCFC);
WWHD_OFFSET(daNpc_Btsw_c, mStick, 0xD1C);
WWHD_OFFSET(daNpc_Btsw_c, mNextMsgNo, 0xD54);
WWHD_OFFSET(daNpc_Btsw_c, mCurrActionFunc, 0xD5C);
WWHD_OFFSET(daNpc_Btsw_c, mActionStatus, 0xD6D);
WWHD_SIZE(daNpc_Btsw_c, 0xD70);

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
