/* Quake, light-volume and attention detection. GameCube names, HD guest layouts.
 * Place/detector sizes remain 0x10/0x14; actor views use the HD base's added lighting state. */
#pragma once
#include "f_op/f_op_actor.h"
class dDetectPlace_c {
  public:
    s32 chk_enable(); // HD: verified entry returns a full register; const omitted for glue.
    /* 0x00 */ cXyz mPos;
    /* 0x0C */ be<s16> mTimer;
    u8 padding[2];
};
// HD: local dDetect_c view; the shared header currently declares that name opaque.
class dDetectState_c {
  public:
    void proc();
    s32 chk_quake(const cXyz *);
    void set_quake(const cXyz *);
    u8 chk_quake_area(const cXyz *);
    s32 chk_light(const cXyz *);
    s32 chk_attention(cXyz *);
    /* 0x00 */ dDetectPlace_c mPlace[1];
    /* 0x10 */ be<s16> mTimer;
    u8 padding[2];
};
WWHD_SIZE(dDetectPlace_c, 0x10);
WWHD_SIZE(dDetectState_c, 0x14);
// HD: minimal views of the player, light tag and game context touched by this unit.
struct DetectPlayer : fopAc_ac_c {
    u8 playerState[0x14];
    /* 0x3C0 */ be<u32> mResetFlg0;
    enum { daPyRFlg0_FRONT_ROLL_CRASH = 0x2000 };
};
struct DetectMatrix {
    be<f32> values[3][4];
};
struct DetectLight : fopAc_ac_c {
    enum Shape { BOX, CONE };
    u8 tagState[0x10];
    /* 0x3BC */ be<s32> mShape; // GameCube m2A0
    /* 0x3C0 */ DetectMatrix mMtx;
    /* 0x3F0 */ DetectMatrix mInvMtx; // GameCube m2D4
    /* 0x420 */ be<u8> mEnabled;      // GameCube m304
};
struct DetectAttention {
    u8 state[12];
    be<u32> mLook2Target;
};
struct DetectGame {
    u8 state[0x595C];
    DetectAttention mAttention;
    u8 attentionTail[0x1C0];
    /* 0x5B2C */ gptr<DetectPlayer> mPlayer;
};
WWHD_OFFSET(DetectPlayer, mResetFlg0, 0x3C0);
WWHD_OFFSET(DetectLight, mShape, 0x3BC);
WWHD_OFFSET(DetectLight, mInvMtx, 0x3F0);
WWHD_OFFSET(DetectLight, mEnabled, 0x420);
WWHD_OFFSET(DetectGame, mPlayer, 0x5B2C);
enum { fpcNm_Tag_Light_e = 469 };
void *operator_new(u32);
void operator_delete(void *);
DetectGame *dComIfG_getGameInfo();
s32 fopAcM_IsActor(void *);
f32 PSVECSquareDistance(cXyz *, cXyz *);
f32 PSVECSquareMag(cXyz *);
void PSMTXMultVec(DetectMatrix *, const cXyz *, cXyz *);
double sqrtDouble(double);
s32 daObj_PrmAbstract(void *, u32, u32);
using DetectJudgeFunc = void *(*)(void *, void *);
using DetectCtorFunc = dDetectPlace_c *(*)(dDetectPlace_c *);
void *fopAcIt_Judge(DetectJudgeFunc, void *);
fopAc_ac_c *dAttHint_convPId(DetectAttention *, u32);
void constructArray(void *, u32, u32, DetectCtorFunc);
void registerGlobalObject(void *);
void initializeHeaderByte(void *);
void initializeHeaderFlag(void *);
