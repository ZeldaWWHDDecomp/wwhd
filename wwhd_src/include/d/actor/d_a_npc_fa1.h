/* Recovery fairy layout.
 * HD: the NPC base grows by 0x118; the point light includes its HD multiplier. */
#pragma once
#include "gc_api/gc_api.h"
namespace fa1_hd {
struct dBgS_ObjGndChk_All : dBgS_ObjGndChk {};
WWHD_SIZE(dBgS_ObjGndChk_All, 0x54);
} // namespace fa1_hd
struct daNpc_Fa1_McaMorfCallBack1_c {
    // HD: preserve the verified full BOOL result.
    BOOL execute(u16 joint, void *transform);
    be<u32> __vtbl;
    be<s16> mNeckAngle;
    be<u16> mNeckJoint;
};
struct daNpc_Fa1_c : fopNpc_npc_c {
    enum Type_e {
        Type_NORMAL_e = 0,
        Type_TIMER_e = 1,
        Type_LINK_e = 2,
        Type_BABA_e = 3,
        Type_AREA_e = 4,
        Type_LINK_DOWN_e = 5,
        Type_HOVER_e = 6,
    };

    enum Mode_e {
        Mode_NORMAL_MOVE_e = 0,
        Mode_PLAYER_MOVE_e = 1,
        Mode_ESCAPE_MOVE_e = 2,
        Mode_BOTTLE_APPEAR_MOVE_e = 3,
        Mode_BOTTLE_BABA_WAIT_e = 4,
        Mode_BOTTLE_BABA_MOVE_e = 5,
        Mode_BOTTLE_BABA_MOVE2_e = 6,
        Mode_BIGELF_CHANGE_e = 7,
        Mode_HOVER_MOVE_e = 8,
    };

    enum Normal_SubMode_e {
        NormalSubMode_STRAIGHT_e = 0,
        NormalSubMode_TURN_e = 1,
        NormalSubMode_AREAMOVE_e = 2,
        NormalSubMode_AREAOUTMOVE_e = 3,
    };

    enum Bottle_SubMode_e {
        BottleSubMode_UP1_e = 0,
        BottleSubMode_DOWN_e = 1,
        BottleSubMode_UP2_e = 2,
    };

    enum BottleBaba_SubMode_e {
        BottleBabaSubMode_BABADOWN_e = 0,
        BottleBabaSubMode_UP2_e = 1,
    };

    void init_bottle_baba_wait();
    void init_bottle_appear_move();
    void init_hover_move();
    void init_areaMove();
    void init_straight2();
    void bottle_baba_wait();
    void baba_down();
    void init_down();
    void up1();
    void init_up2();
    void down();
    void init_up1();
    void setPointLightParam();
    void init_normal_move();
    void init_escape_move();
    void init_areaOutMove();
    void up2();
    void init_straight();
    void init_turn();
    void init_baba_up();
    void init_baba_down();
    void init_bottle_baba_move2();
    void init_bottle_baba_move();
    void straight();
    void turn();
    void position_move(f32 height, f32 verticalSpeed);
    BOOL _delete();
    BOOL _draw();
    void BGCheck();
    void findPlayer();
    BOOL CreateHeap();
    void setMtx();
    void init_get_player_move();
    void hover_move();
    void init_bigelf_change();
    void bigelf_change();
    BOOL checkBinCatch();
    void areaMove();
    void areaOutMove();
    BOOL createInit();
    s32 _create();
    BOOL _execute();
    void normal_move();
    void escape_move();
    void get_player_move();
    void bottle_appear_move();
    void bottle_baba_move();
    void bottle_baba_move2();

    /* 0x7DC */ u8 m6C4[8];
    /* 0x7E4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x7E8 */ daNpc_Fa1_McaMorfCallBack1_c mMcaMorfCallback1;
    /* 0x7F0 */ dPa_followEcallBack mSparklePtclCallback;
    /* 0x804 */ gptr<void> mpEmitter;
    /* 0x808 */ fa1_hd::dBgS_ObjGndChk_All mGndChk;
    /* 0x85C */ LIGHT_INFLUENCE mPointLight;
    /* 0x880 */ gptr<fopAc_ac_c> mpTargetActor;  /* was m764: init_bottle_baba_move 022347F4 searches Grandma (fpcNm_NPC_BA1_e) into it, init_bottle_baba_move2 02234778 the player; bottle_baba_move 02234894 / bottle_baba_move2 02234A60 head the fairy at it and turn back above it, baba_down 02234CA0 turns up below it; createInit 0223350C null (tww the same) */
    /* 0x884 */ cXyz mPlayerOffset;  /* was m768: init_up1 02233154 sets it to current.pos minus the player's position; get_player_move 02234204 and bottle_appear_move 022344EC turn it round the player (radius mPlayerR), raise it and put current.pos at the player's position plus it; down 02235238 tests its y (tww the same) */
    /* 0x890 */ cXyz mGroundNormal;  /* was m774: BGCheck 02233CD8 copies the ground polygon's normal into it (0, 1, 0 without ground); tww _draw passes it to dComIfGd_setSimpleShadow, a submission the WWHD _draw 02233B58 no longer makes */
    /* 0x89C */ be<f32> mPlayerR;
    /* 0x8A0 */ be<f32> mGroundY;
    /* 0x8A4 */ be<u16> mTimer;
    /* 0x8A6 */ be<s16> mOrbitSpeed;  /* was m78A: get_player_move 02234204 and bottle_appear_move 022344EC subtract it from current.angle.y each frame (the turn round the player) and raise it by the acceleration up to the limit; init_get_player_move 02233EA8 HIO mGetOrbitSpeed, init_up1 02233154 0 (tww the same) */
    /* 0x8A8 */ be<s16> mOrbitSpeedMax;  /* was m78C: bottle_appear_move 022344EC caps mOrbitSpeed at it; init_up1 02233154 0, init_down 022351A0 HIO mDownOrbitSpeedMax (tww the same) */
    /* 0x8AA */ be<s16> mOrbitAccel;  /* was m78E: bottle_appear_move 022344EC adds it to mOrbitSpeed each frame; init_up1 02233154 0, init_down 022351A0 HIO mDownOrbitAccel (tww the same) */
    /* 0x8AC */ be<u8> mMode;
    /* 0x8AD */ be<u8> mMoveTimer;
    /* 0x8AE */ be<u8> mSubMode;
    /* 0x8AF */ be<u8> mStatus;
    /* 0x8B0 */ be<s8> mType;
    /* 0x8B1 */ u8 m795[3];
    /* 0x8B4 */ be<s16> mWanderTimer;  /* was m798: areaMove 02234FA4 / areaOutMove 0223509C count it down and, at 0, flip mWanderDir and restart it at a random 15-35 frames; init_areaMove 022332A0 / init_areaOutMove 02234F80 0; the bottle-baba inits 022347F4 / 02234778 store 1 / 0 that those modes never read (tww the same) */
    /* 0x8B6 */ be<s16> mWanderDir;  /* was m79A: areaMove 02234FA4 / areaOutMove 0223509C toggle it when mWanderTimer runs out and steer -0x2000 / +0x2000 off the course by it; init_areaMove 022332A0 / init_areaOutMove 02234F80 0 (tww the same) */
    /* 0x8B8 */ be<f32> mBabaDownMaxSpeed;  /* was m79C: init_bottle_baba_move 022347F4 / init_bottle_baba_move2 02234778 set it from the bottle-baba HIO; init_baba_down 022346FC makes it maxFallSpeed for the downward phase (tww the same) */
    /* 0x8BC */ be<f32> mBabaUpMaxSpeed;  /* was m7A0: init_bottle_baba_move 022347F4 / init_bottle_baba_move2 02234778 set it from the bottle-baba HIO; init_baba_up 02234C3C makes it maxFallSpeed for the upward phase (tww the same) */
};
WWHD_SIZE(daNpc_Fa1_McaMorfCallBack1_c, 8);
WWHD_OFFSET(daNpc_Fa1_c, mpMorf, 0x7E4);
WWHD_OFFSET(daNpc_Fa1_c, mPointLight, 0x85C);
WWHD_OFFSET(daNpc_Fa1_c, mTimer, 0x8A4);
WWHD_OFFSET(daNpc_Fa1_c, mMode, 0x8AC);
WWHD_SIZE(daNpc_Fa1_c, 0x8C0);

s32 fa1_atan2s(f32 x, f32 z);

s32 fa1_targetAngleY(const cXyz *a, const cXyz *b);

s32 fa1_getReverb(s8 room);

f32 fa1_sqrtf(f32 value);

u8 fa1_calcMoveTimer(be<u8> *timer);

s16 fa1_calcAreaTimer(be<s16> *timer);

u32 fa1_randomTimer(u32 base, u32 range);

f32 fa1_yuragiRatio(f32 value);

struct daNpc_Fa1_HIO3_c {
    struct hio_prm_c {
        /* mHoverSpeedY (+0x04) was m08: init_hover_move 0223326C maxFallSpeed = -it; hover_move 0223447C passes it to position_move as the vertical speed (tww the same) */
        /* mHoverGravity (+0x08) was m0C: init_hover_move 0223326C gravity (tww the same) */
        /* mHoverHeight (+0x0C) was m10: hover_move 0223447C passes it to position_move, which turns the vertical speed round this far above / below home.pos.y (tww the same) */
        /* mBigelfSpeed (+0x10) was m14: init_bigelf_change 02234CB8 speedF (tww the same) */
        be<f32> mHoverSpeedY, mHoverGravity, mHoverHeight, mBigelfSpeed;
        /* mBigelfAngleOffset (+0x14) was m18: bigelf_change 02234D1C: current.angle.y = the angle to home.pos plus it (tww the same) */
        be<s16> mBigelfAngleOffset;
        u8 _1A[2];
    };
    be<s8> mNo;
    u8 _01[3];
    hio_prm_c prm;
    be<u32> __vtbl;
};
struct daNpc_Fa1_HIO2_c {
    struct hio_prm_c {
        /* mBabaSpeed (+0x04) was m08: init_bottle_baba_move 022347F4 / init_bottle_baba_move2 02234778 and every bottle_baba_move frame (Fa1_babaPosition) speedF (tww the same) */
        /* mBabaMoveUpMaxSpeed (+0x08) was m0C: init_bottle_baba_move 022347F4 mBabaUpMaxSpeed (tww the same) */
        /* mBabaMoveDownMaxSpeed (+0x0C) was m10: init_bottle_baba_move 022347F4 mBabaDownMaxSpeed (tww the same) */
        /* mBabaMove2UpMaxSpeed (+0x10) was m14: init_bottle_baba_move2 02234778 mBabaUpMaxSpeed (tww the same) */
        /* mBabaMove2DownMaxSpeed (+0x14) was m18: init_bottle_baba_move2 02234778 mBabaDownMaxSpeed (tww the same) */
        /* mBabaGravity (+0x18) was m1C: init_baba_down 022346FC gravity (tww the same) */
        be<f32> mBabaSpeed, mBabaMoveUpMaxSpeed, mBabaMoveDownMaxSpeed, mBabaMove2UpMaxSpeed, mBabaMove2DownMaxSpeed, mBabaGravity;
        /* mBabaAngleOffset (+0x1C) was m20: bottle_baba_move (Fa1_babaPosition) / bottle_baba_move2: current.angle.y = the angle to mpTargetActor plus it (tww the same) */
        /* mBabaMoveTimer (+0x1E) was m22: init_bottle_baba_move 022347F4 mTimer (tww the same) */
        /* mBabaWaitTimer (+0x20) was m24: init_bottle_baba_wait 0223311C mTimer; bottle_baba_wait 022347E4 starts bottle_baba_move2 at 0 (tww the same) */
        be<s16> mBabaAngleOffset, mBabaMoveTimer, mBabaWaitTimer;
        u8 _26[2];
    };
    be<s8> mNo;
    u8 _01[3];
    hio_prm_c prm;
    be<u32> __vtbl;
};
struct daNpc_Fa1_HIO_c {
    struct hio_prm_c {
        /* mNormalSpeed (+0x04) was m08: init_normal_move 022332E0 / normal_move 02234078 speedF (tww the same) */
        /* mGetRiseSpeed (+0x08) was m0C: get_player_move 02234204 adds it to mPlayerOffset.y each frame while the fairy circles the player (tww the same) */
        /* mMoveRadius (+0x0C) was m10: createInit 0223350C / straight 02234E60 scale.x, the distance from home.pos past which straight turns back (tww the same) */
        /* mEscapeMaxSpeed (+0x10) was m14: init_escape_move 0223404C maxFallSpeed, the rise speed escape_move climbs to (tww the same) */
        /* mEscapeGravity (+0x14) was m18: init_escape_move 0223404C gravity, escape_move's upward acceleration (tww the same) */
        /* mCatchHorizontalDist (+0x18) was m1C: checkBinCatch 02233F40 bottle catch request (tww dComIfGp_att_CatchRequest horizontalDist) */
        /* mCatchUpDist (+0x1C) was m20: checkBinCatch 02233F40 bottle catch request (tww dComIfGp_att_CatchRequest upDist) */
        /* mCatchDownDist (+0x20) was m24: checkBinCatch 02233F40 bottle catch request (tww dComIfGp_att_CatchRequest downDist) */
        /* mUp1SpeedY (+0x24) was m28: init_up1 02233154 speed.y, the bottle fairy's first rise (tww the same) */
        /* mUp2MaxSpeed (+0x28) was m2C: init_up2 022351DC maxFallSpeed (tww the same) */
        /* mUp1MaxSpeed (+0x2C) was m30: init_up1 02233154 maxFallSpeed (tww the same) */
        /* mUp1Gravity (+0x30) was m34: init_up1 02233154 gravity (tww the same) */
        /* mDownEndY (+0x34) was m38: down 02235238 starts up2 once mPlayerOffset.y is below it (tww the same) */
        be<f32> mNormalSpeed, mGetRiseSpeed, mMoveRadius, mEscapeMaxSpeed, mEscapeGravity, mCatchHorizontalDist, mCatchUpDist, mCatchDownDist, mUp1SpeedY, mUp2MaxSpeed, mUp1MaxSpeed, mUp1Gravity, mDownEndY;
        /* mGetOrbitSpeed (+0x38) was m3C: init_get_player_move 02233EA8 mOrbitSpeed (tww the same) */
        /* mGetOrbitSpeedMax (+0x3A) was m3E: get_player_move 02234204 caps mOrbitSpeed at it (tww the same) */
        /* mGetOrbitAccel (+0x3C) was m40: get_player_move 02234204 adds it to mOrbitSpeed each frame (tww the same) */
        /* mGetTimer (+0x3E) was m42: init_get_player_move 02233EA8 mTimer; get_player_move 02234204 gives the recovery fairy and deletes at 0 (tww the same) */
        /* mNormalTimer (+0x40) was m44: init_normal_move 022332E0 mTimer = a random value from it (+0-60); normal_move 02234078 escapes at 0 when status bit 1 is set (tww the same) */
        /* mEscapeTimer (+0x42) was m46: init_escape_move 0223404C mTimer; escape_move 0223434C deletes the fairy at 0 (tww init_turn also reloads it; the WWHD init_turn 02234E14 does not) */
        /* mCatchAngle (+0x44) was m48: checkBinCatch 02233F40 bottle catch request (tww dComIfGp_att_CatchRequest angle) */
        /* mBottleTimer (+0x46) was m4A: init_up1 02233154 mTimer; bottle_appear_move 022344EC gives the item and deletes at 0 (tww the same) */
        /* mDownOrbitSpeedMax (+0x48) was m4C: init_down 022351A0 mOrbitSpeedMax (tww the same) */
        /* mDownOrbitAccel (+0x4A) was m4E: init_down 022351A0 mOrbitAccel (tww the same) */
        be<s16> mGetOrbitSpeed, mGetOrbitSpeedMax, mGetOrbitAccel, mGetTimer, mNormalTimer, mEscapeTimer, mCatchAngle, mBottleTimer, mDownOrbitSpeedMax, mDownOrbitAccel;
    };
    // HD: all three HIO vtables move to the end of their respective objects.
    be<s8> mNo;
    u8 _01[3];
    hio_prm_c prm;
    daNpc_Fa1_HIO2_c mBottleBabaHio;  /* was m50: the daNpc_Fa1_HIO2_c whose prm only the bottle-baba modes read (init_bottle_baba_wait 0223311C .. init_baba_down 022346FC); daNpc_Fa1_HIO_c__ct 02235350 constructs it (tww l_HIO.m50) */
    daNpc_Fa1_HIO3_c mHoverHio;  /* was m78: the daNpc_Fa1_HIO3_c whose prm hover_move 0223447C and bigelf_change 02234D1C read; daNpc_Fa1_HIO_c__ct 02235350 constructs it (tww l_HIO.m78) */
    be<u32> __vtbl;
};
WWHD_SIZE(daNpc_Fa1_HIO3_c, 0x1C);
WWHD_SIZE(daNpc_Fa1_HIO2_c, 0x28);
WWHD_OFFSET(daNpc_Fa1_HIO_c, mBottleBabaHio, 0x4C);
WWHD_OFFSET(daNpc_Fa1_HIO_c, mHoverHio, 0x74);
WWHD_SIZE(daNpc_Fa1_HIO_c, 0x94);
namespace fa1_hd {
using MoveFunction = void (daNpc_Fa1_c::*)();
using MovePtmf = gabi::ptmf<MoveFunction>;
struct MoveTable {
    MovePtmf mData[9];
};
struct MoveSubTable {
    MovePtmf mData[4];
};
struct BottleMoveSubTable {
    MovePtmf mData[3];
};
struct BottleBabaSubTable {
    MovePtmf mData[2];
};
} // namespace fa1_hd

void fa1_seStart(u32 id, void *pos, void *param, s32 reverb);

void fa1_vectorSubtract(const void *a, cXyz *result, const void *b);

void fa1_vectorAdd(const void *a, cXyz *result, const void *b);

void *fa1_operatorNew(u32 size);

void fa1_operatorDelete(void *object);

void fa1_npcConstructor(fopNpc_npc_c *actor);

void fa1_followCallbackConstructor(void *callback, s32 param, s32 flags);

void fa1_groundCheckConstructor(void *check);

void fa1_groundCheckDestructor(void *check, s32 flags);

void fa1_cylinderDestructor(void *cylinder, s32 flags);

void fa1_collisionStatusDestructor(void *status, s32 flags);

void fa1_circleDestructor(void *circle, s32 flags);

void fa1_actorGroundDestructor(void *acch, s32 flags);

void fa1_actorDestructor(fopAc_ac_c *actor, s32 flags);

void fa1_registerGlobalObject(void *descriptor);

void fa1_headerStaticConstructor(void *object);

void fa1_headerStaticByteConstructor(void *object);

void fa1_searchByName(s32 name, gptr<fopAc_ac_c> *result);

void fa1_followCallbackRemove(void *callback);

void fa1_effectPointLightCut(void *light);

void fa1_hioDeleteChild(s8 child);

void *fa1_getEnvironmentLight();

void fa1_settingTevStruct(void *environment, s32 type, const cXyz *pos, void *tev);

void fa1_setLightTevColorType(void *environment, void *model, void *tev);

f32 fa1_groundCross(void *background, void *check);

void *fa1_getTriPlane(void *background, u16 backgroundId, u16 index);

u8 fa1_getRoomId(void *background, void *poly);

u8 fa1_getPolyColor(void *background, void *poly);

void *fa1_getObjectResource(void *manager, SafeString *name, s32 index);

void *fa1_morfCreate(void *object, void *data, void *callback1, void *callback2, void *animation,
                     s32 loop, s32 start, s32 end, s32 param, s32 sound, s32 flags,
                     u32 displayFlags, f32 rate);

void *fa1_getJointNameResource(void *modelData);

s32 fa1_getJointIndex(void *table, const char *name);

void fa1_matrixTranslate(void *matrix, f32 x, f32 y, f32 z);

void fa1_matrixRotateY(void *matrix, s16 angle);

s32 fa1_searchActorAngleY(fopAc_ac_c *actor, void *target);

void fa1_attentionCatchRequest(void *attention, fopAc_ac_c *actor, s32 item, s16 timer, s32 flags,
                               f32 radius, f32 upper, f32 lower);

void fa1_collisionStatusInit(void *status, s32 weight, s32 flags, fopAc_ac_c *actor);

void fa1_cylinderSet(void *cylinder, void *source);

void fa1_effectPointLightSet(void *light);

void *fa1_particleSet(void *controller, s32 group, s32 effect, const cXyz *pos, s32 angle,
                      s32 scale, s32 alpha, void *callback, s32 setup, s32 primary, s32 environment,
                      s32 scale2D);

using Fa1HeapCallback = BOOL (*)(daNpc_Fa1_c *);
BOOL fa1_entrySolidHeap(fopAc_ac_c *actor, Fa1HeapCallback callback, u32 size);

s32 fa1_hioCreateChild(const char *name, void *hio);

void fa1_morfPlay(void *morf, s32 pos, s32 sound, s32 reverb);

void fa1_attentionLookRequest(void *attention, fopAc_ac_c *actor, s32 angle, s32 flags, f32 radius,
                              f32 upper, f32 lower);

void fa1_cylinderSetCenter(void *cylinder, const cXyz *pos);

void fa1_collisionSet(void *collision, void *object);

BOOL fa1_cylinderCheckCoHit(void *cylinder);

u16 fa1_calcTimerU16(be<u16> *timer);

u16 fa1_getRndValueU16(u16 base, u16 range);

u8 fa1_getRndValueU8(u8 base, u8 range);

daNpc_Fa1_McaMorfCallBack1_c *
daNpc_Fa1_McaMorfCallBack1_c__ct(daNpc_Fa1_McaMorfCallBack1_c *callback);

BOOL daNpc_Fa1_IsDelete(daNpc_Fa1_c *actor);

void fa1_SafeString_assureTerminationImpl_(SafeString *string);

void fa1_SafeString__dt(void *string, s32 flags);

daNpc_Fa1_c *daNpc_Fa1_c__ct(daNpc_Fa1_c *actor);

void daNpc_Fa1_c__dt(daNpc_Fa1_c *actor, s32 flags);

void *daNpc_Fa1_HIO3_c__ct(void *hio);

void *daNpc_Fa1_HIO2_c__ct(void *hio);

void *daNpc_Fa1_HIO_c__ct(void *hio);

void __sinit_d_a_npc_fa1_cpp();

BOOL daNpc_Fa1_Delete(daNpc_Fa1_c *actor);

BOOL daNpc_Fa1_Draw(daNpc_Fa1_c *actor);

BOOL CheckCreateHeap(daNpc_Fa1_c *actor);

s32 daNpc_Fa1_Create(daNpc_Fa1_c *actor);

BOOL daNpc_Fa1_Execute(daNpc_Fa1_c *actor);

namespace fa1_hd {
// HD: private attention view; only the request subobject addresses are used.
struct Attention {
    u8 _00[0x130];
    u8 mCatch[0x18];
    u8 mLook[0x18];
    u8 _160[0x38];
};
WWHD_OFFSET(Attention, mCatch, 0x130);
WWHD_OFFSET(Attention, mLook, 0x148);
WWHD_SIZE(Attention, 0x198);
} // namespace fa1_hd

namespace fa1_hd {
struct MainHioInitial {
    be<u32> mWords[18];
};
struct AreaHioInitial {
    be<u32> mWords[8];
};
struct HoverHioInitial {
    be<u32> mWords[5];
};
WWHD_SIZE(MainHioInitial, 0x48);
WWHD_SIZE(AreaHioInitial, 0x20);
WWHD_SIZE(HoverHioInitial, 0x14);
} // namespace fa1_hd

namespace fa1_hd {
struct TransformInfo {
    cXyz mScale;
    csXyz mRotation;
    u8 _12[2];
    cXyz mTranslate;
};
struct EmitterRatePrefix {
    u8 _00[0x34];
    be<f32> mRate;
};
struct JointNameResourcePrefix {
    u8 _00[0x10];
    be<u32> mNameTableOffset;
};
struct HeaderState {
    be<f32> mTimer, mUnused;
    u8 mFirst, mSecond, _0A[2];
    be<u32> mZero[4];
};
struct HeaderDescriptors {
    be<u32> mWords[9];
};
WWHD_SIZE(TransformInfo, 0x20);
WWHD_OFFSET(EmitterRatePrefix, mRate, 0x34);
WWHD_SIZE(EmitterRatePrefix, 0x38);
WWHD_OFFSET(JointNameResourcePrefix, mNameTableOffset, 0x10);
WWHD_SIZE(JointNameResourcePrefix, 0x14);
WWHD_SIZE(HeaderState, 0x1C);
WWHD_SIZE(HeaderDescriptors, 0x24);
} // namespace fa1_hd
// HD archive indices differ from the GameCube Always archive.
enum { dRes_INDEX_ALWAYS_BDL_FA_e = 0x1F, dRes_INDEX_ALWAYS_BCK_FA_e = 0xC };

namespace fa1_hd {
struct ResourceManagerHandle {
    gptr<dRes_control_c> mResource;
};
} // namespace fa1_hd

// Archive/process/audio IDs used by the tww fairy source, with the HD values.
enum {
    fpcNm_NPC_BA1_e = 0x14F,
    dItemNo_RECOVER_FAIRY_e = 0x16,
    dItemNo_FAIRY_BOTTLE_e = 0x57,
    JA_SE_OBJ_FAILY_GET = 0x695D,
    JA_SE_OBJ_BOTTLE_FAILY = 0x6975,
    JA_SE_OBJ_BA_FAILY_1 = 0x6A2D,
    JA_SE_OBJ_BA_FAILY_2 = 0x6A2E,
};
namespace dPa_name {
enum { ID_AK_JN_CUREFAIRY00 = 0x52 };
}
WWHD_OFFSET(daNpc_Fa1_c, mMcaMorfCallback1, 0x7E8);
WWHD_OFFSET(daNpc_Fa1_c, mSparklePtclCallback, 0x7F0);
WWHD_OFFSET(daNpc_Fa1_c, mpEmitter, 0x804);
WWHD_OFFSET(daNpc_Fa1_c, mGndChk, 0x808);
WWHD_OFFSET(daNpc_Fa1_c, mpTargetActor, 0x880);
WWHD_OFFSET(daNpc_Fa1_c, mPlayerOffset, 0x884);
WWHD_OFFSET(daNpc_Fa1_c, mGroundNormal, 0x890);
WWHD_OFFSET(daNpc_Fa1_c, mPlayerR, 0x89C);
WWHD_OFFSET(daNpc_Fa1_c, mWanderTimer, 0x8B4);
WWHD_OFFSET(daNpc_Fa1_c, mBabaUpMaxSpeed, 0x8BC);
