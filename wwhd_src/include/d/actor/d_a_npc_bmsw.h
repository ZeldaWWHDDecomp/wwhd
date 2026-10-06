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
#include "d/actor/d_a_npc_ba1.h" /* ProcFunc_l (GHS pointer to member, 8 bytes) */
#include "d/d_npc.h"

WWHD_OPAQUE(ShopCursor_c);

struct SwMail_c {
    /* methods (GameCube names) */
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
    void draw(dKy_tevstr_c*);

    /* 0x00 */ ProcFunc_l mFunc;          /* HD: 8 bytes (GameCube 12) */
    /* 0x08 */ gptr<J3DModel> mpModel;
    /* 0x0C */ u8 field_0x10[0x74];       /* mDoExt_btpAnm (HD 0x74) */
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
struct STControl_l {
    /* 0x00 */ u8 _00[0x24];
    /* 0x24 */ be<u32> __vtbl;
};
WWHD_SIZE(STControl_l, 0x28);

struct daNpc_Bmsw_c : fopNpc_npc_c {
    enum ActionStatus { ACTION_STARTING = 0, ACTION_ONGOING = 1, ACTION_UNK_2 = 2, ACTION_ENDING = -1 };

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
    /* 0x7EC */ gptr<J3DModel> field_0x6D4;      /* head */
    /* 0x7F0 */ gptr<J3DModel> field_0x6D8;      /* bag */
    /* 0x7F4 */ gptr<J3DModel> field_0x6DC;      /* letter */
    /* 0x7F8 */ dKy_tevstr_c field_0x6E0;        /* HD 0x1C8 */
    /* 0x9C0 */ gptr<mDoExt_McaMorf> mpMorfHand;
    /* 0x9C4 */ Mtx34 field_0x794;               /* arm L */
    /* 0x9F4 */ Mtx34 field_0x7C4;               /* arm R */
    /* 0xA24 */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0xA28 */ u8 field_0x7F8[0x74];            /* mDoExt_btpAnm (HD 0x74) */
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
    /* 0xD2C */ STControl_l field_0x988;
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
    /* 0xD6C */ ProcFunc_l mCurrActionFunc;      /* HD: 8 bytes (GameCube 12) */
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
