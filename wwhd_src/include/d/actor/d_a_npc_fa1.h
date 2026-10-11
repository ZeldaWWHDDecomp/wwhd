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
    /* 0x880 */ gptr<fopAc_ac_c> m764;
    /* 0x884 */ cXyz m768;
    /* 0x890 */ cXyz m774;
    /* 0x89C */ be<f32> mPlayerR;
    /* 0x8A0 */ be<f32> mGroundY;
    /* 0x8A4 */ be<u16> mTimer;
    /* 0x8A6 */ be<s16> m78A;
    /* 0x8A8 */ be<s16> m78C;
    /* 0x8AA */ be<s16> m78E;
    /* 0x8AC */ be<u8> mMode;
    /* 0x8AD */ be<u8> mMoveTimer;
    /* 0x8AE */ be<u8> mSubMode;
    /* 0x8AF */ be<u8> mStatus;
    /* 0x8B0 */ be<s8> mType;
    /* 0x8B1 */ u8 m795[3];
    /* 0x8B4 */ be<s16> m798;
    /* 0x8B6 */ be<s16> m79A;
    /* 0x8B8 */ be<f32> m79C;
    /* 0x8BC */ be<f32> m7A0;
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
        be<f32> m08, m0C, m10, m14;
        be<s16> m18;
        u8 _1A[2];
    };
    be<s8> mNo;
    u8 _01[3];
    hio_prm_c prm;
    be<u32> __vtbl;
};
struct daNpc_Fa1_HIO2_c {
    struct hio_prm_c {
        be<f32> m08, m0C, m10, m14, m18, m1C;
        be<s16> m20, m22, m24;
        u8 _26[2];
    };
    be<s8> mNo;
    u8 _01[3];
    hio_prm_c prm;
    be<u32> __vtbl;
};
struct daNpc_Fa1_HIO_c {
    struct hio_prm_c {
        be<f32> m08, m0C, m10, m14, m18, m1C, m20, m24, m28, m2C, m30, m34, m38;
        be<s16> m3C, m3E, m40, m42, m44, m46, m48, m4A, m4C, m4E;
    };
    // HD: all three HIO vtables move to the end of their respective objects.
    be<s8> mNo;
    u8 _01[3];
    hio_prm_c prm;
    daNpc_Fa1_HIO2_c m50;
    daNpc_Fa1_HIO3_c m78;
    be<u32> __vtbl;
};
WWHD_SIZE(daNpc_Fa1_HIO3_c, 0x1C);
WWHD_SIZE(daNpc_Fa1_HIO2_c, 0x28);
WWHD_OFFSET(daNpc_Fa1_HIO_c, m50, 0x4C);
WWHD_OFFSET(daNpc_Fa1_HIO_c, m78, 0x74);
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
WWHD_OFFSET(daNpc_Fa1_c, m764, 0x880);
WWHD_OFFSET(daNpc_Fa1_c, m768, 0x884);
WWHD_OFFSET(daNpc_Fa1_c, m774, 0x890);
WWHD_OFFSET(daNpc_Fa1_c, mPlayerR, 0x89C);
WWHD_OFFSET(daNpc_Fa1_c, m798, 0x8B4);
WWHD_OFFSET(daNpc_Fa1_c, m7A0, 0x8BC);
