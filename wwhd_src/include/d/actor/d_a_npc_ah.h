/* daNpcAh_c (Old Man Ho Ho), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the functions of d_a_npc_ah: daNpcAh_c derives from
 * fopNpc_npc_c (HD 0x7DC, fopNpc_npc_c_l in d_a_npc_ob1.h), so mPhs..mBtpAnm are +0x118;
 * mDoExt_btpAnm grew from 0x14 to 0x74 and mShadowId (GameCube 0x6EC) is gone (HD shadows), so
 * everything from mPathRun on is +0x174. Size 0x8C8 (constructor 021E5398 allocates 0x8C8;
 * GameCube 0x754). The actor's vtable is 10015FE4 (destructor +0x0C, next_msgStatus +0x14,
 * getMsg +0x1C, anmAtr +0x24 = fopNpc_npc_c::anmAtr). */
#pragma once
#include "d/actor/d_a_npc_ob1.h"

struct sAhAnmDat {
    /* 0x00 */ be<u8> mBckIdx; /* 0xFF: end */
    /* 0x01 */ be<u8> mMorf;
    /* 0x02 */ be<s8> field_0x02; /* > 0: play n times, then the next entry */
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
    /* 0x14 */ be<f32> field_0x14; /* dNpc_playerEyePos offset */
    /* 0x18 */ be<f32> field_0x18; /* attention position (x, y, z) */
    /* 0x1C */ be<f32> field_0x1C;
    /* 0x20 */ be<f32> field_0x20;
    /* 0x24 */ be<f32> field_0x24; /* eye height */
    /* 0x28 */ be<f32> field_0x28; /* talk distance */
    /* 0x2C */ be<f32> field_0x2C; /* look distance */
    /* 0x30 */ be<s16> field_0x30; /* talk angle */
    /* 0x32 */ be<s16> field_0x32; /* look speed */
    /* 0x34 */ be<s16> field_0x34;
    /* 0x36 */ be<s16> field_0x36;
    /* 0x38 */ be<f32> field_0x38; /* cylinder radius */
    /* 0x3C */ be<f32> field_0x3C;
    /* 0x40 */ be<f32> field_0x40;
    /* 0x44 */ be<s16> field_0x44;
    /* 0x46 */ be<s16> field_0x46;
    /* 0x48 */ be<s16> field_0x48;
    /* 0x4A */ be<s16> field_0x4A;
    /* 0x4C */ be<s16> field_0x4C;
    /* 0x4E */ be<s16> field_0x4E;
    /* 0x50 */ be<s16> field_0x50; /* look timer */
    /* 0x52 */ be<s8> field_0x52;
    /* 0x53 */ be<s8> field_0x53;
};
WWHD_SIZE(NpcDatStruct, 0x54);

struct daNpcAh_c : fopNpc_npc_c_l {
    /* 021E5398 */ static daNpcAh_c* ct(daNpcAh_c* p);
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
    /* 0x7F0 */ u8 mBtpAnm[0x74]; /* mDoExt_btpAnm (HD 0x74); HD: no mShadowId after it */
    /* 0x864 */ dNpc_PathRun_c_l mPathRun;
    /* 0x86C */ cXyz field_0x6F4;
    /* 0x878 */ cXyz mEyePos;
    /* 0x884 */ gptr<sAhAnmDat> mpAnmDat;
    /* 0x888 */ be<u32> mpMsgNo; /* u32* */
    /* 0x88C */ be<f32> field_0x718;
    /* 0x890 */ be<f32> field_0x71C; /* next morf (< 0: none) */
    /* 0x894 */ be<f32> field_0x720;
    /* 0x898 */ be<u32> mItemNo;
    /* 0x89C */ be<u8> mHeadOnlyFollow;
    /* 0x89D */ u8 _89D;
    /* 0x89E */ be<s16> field_0x72A;
    /* 0x8A0 */ be<s16> field_0x72C;
    /* 0x8A2 */ be<s16> field_0x72E;
    /* 0x8A4 */ be<s16> field_0x730;
    /* 0x8A6 */ be<s16> field_0x732;
    /* 0x8A8 */ be<u16> field_0x734;
    /* 0x8AA */ be<s16> field_0x736;
    /* 0x8AC */ be<s16> field_0x738;
    /* 0x8AE */ be<s16> mTimer;
    /* 0x8B0 */ be<s16> mTargetAngle;
    /* 0x8B2 */ be<s16> mLookAtMaxVel;
    /* 0x8B4 */ be<s16> mTargetYRot;
    /* 0x8B6 */ be<u8> field_0x742;
    /* 0x8B7 */ be<u8> field_0x743;
    /* 0x8B8 */ be<u8> field_0x744;
    /* 0x8B9 */ be<u8> mBtpFrame;
    /* 0x8BA */ be<u8> mMoveState;
    /* 0x8BB */ be<u8> field_0x747; /* resource flag */
    /* 0x8BC */ be<u8> field_0x748;
    /* 0x8BD */ be<u8> mBckIdx;
    /* 0x8BE */ be<u8> field_0x74A;
    /* 0x8BF */ be<s8> field_0x74B;
    /* 0x8C0 */ be<s8> mActIdx;
    /* 0x8C1 */ be<u8> field_0x74D;
    /* 0x8C2 */ be<s8> field_0x74E;
    /* 0x8C3 */ be<u8> field_0x74F;
    /* 0x8C4 */ be<u8> field_0x750;
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
