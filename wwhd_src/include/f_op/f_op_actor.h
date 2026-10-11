/* fopAc_ac_c: WWHD layout.
 *
 * GameCube -> WWHD: unchanged up to 0xC0; +4 from 0xC4 (the C++ vtable pointer at 0xB4 of the
 * HD classes pushes the tail of leafdraw_class); dKy_tevstr_c grew from 0xB0 to 0x1C8 (HD
 * lighting), so every field from setID on is +0x11C. Offsets from tools/decomp/layout.py and
 * the verified functions. Size 0x3AC (GameCube 0x290). */
#pragma once
#include "SSystem/SComponent/c_xyz.h"
#include "f_pc/f_pc_base.h"
#include "wwhd.h"

struct actor_place {
    /* 0x00 */ cXyz pos;
    /* 0x0C */ csXyz angle;
    /* 0x12 */ be<s8> roomNo;
    /* 0x13 */ be<u8> field_0x13;
};
WWHD_SIZE(actor_place, 0x14);

/* dKy_tevstr_c, HD 0x1C8 (GameCube 0xB0): the HD light object is 0x10 bigger (J3DLightObj 0x84,
 * GameCube 0x74), so the GameCube members follow at +0x10; HD appends light parameter copies at
 * 0xC0..0x1C8 (dKy_tevstr_init 0255FFF4 fills them; settingTevStruct 025626A4). Offsets from the
 * verified d_kankyo_5/6 functions and the actors reading mColorC0 (+0x90), mColorK0 (+0x98),
 * mEnvrIdxOverride (+0xBA) [v d_kankyo, mkiek, bdkobj, bb, npc_p1, os_a]. */
struct dKy_tevstr_c {
    /* 0x00 */ u8 mLightObj[0x84];       /* J3DLightObj (HD 0x84): position +0, direction +0xC, colour +0x18 */
    /* 0x84 */ cXyz mLightPosWorld;      /* GameCube 0x74 */
    /* 0x90 */ GXColorS10 mColorC0;      /* GameCube 0x80 */
    /* 0x98 */ GXColor mColorK0;
    /* 0x9C */ GXColor mColorK1;
    /* 0xA0 */ GXColorS10 mFogColor;
    /* 0xA8 */ be<f32> mFogStartZ;
    /* 0xAC */ be<f32> mFogEndZ;
    /* 0xB0 */ be<f32> mColpatBlend;
    /* 0xB4 */ be<u8> mInitTimer;
    /* 0xB5 */ be<u8> mEnvrIdxCurr;
    /* 0xB6 */ be<u8> mEnvrIdxPrev;
    /* 0xB7 */ be<u8> mColpatCurr;
    /* 0xB8 */ be<u8> mColpatPrev;
    /* 0xB9 */ be<s8> mRoomNo;           /* GameCube 0xA9 */
    /* 0xBA */ be<u8> mEnvrIdxOverride;
    /* 0xBB */ be<u8> mLightMode;
    /* 0xBC */ be<u8> mInitType;         /* 123 after dKy_tevstr_init, 124 once set */
    /* 0xBD */ u8 _BD[3];
    /* 0xC0 */ u8 mHdLightParam[0x1C8 - 0xC0]; /* HD: copies of the light block (positions, colours, factors) */
};
WWHD_OFFSET(dKy_tevstr_c, mLightPosWorld, 0x84);
WWHD_OFFSET(dKy_tevstr_c, mColorC0, 0x90);
WWHD_OFFSET(dKy_tevstr_c, mColorK0, 0x98);
WWHD_OFFSET(dKy_tevstr_c, mFogColor, 0xA0);
WWHD_OFFSET(dKy_tevstr_c, mColpatBlend, 0xB0);
WWHD_OFFSET(dKy_tevstr_c, mRoomNo, 0xB9);
WWHD_OFFSET(dKy_tevstr_c, mInitType, 0xBC);
WWHD_SIZE(dKy_tevstr_c, 0x1C8);

/* event commands (dEvt_info_c::mCommand; GameCube d_event names) */
enum dEvt_Command_e {
    dEvtCmd_NONE_e = 0x0000,
    dEvtCmd_INTALK_e = 0x0001,
    dEvtCmd_INDEMO_e = 0x0002,
    dEvtCmd_INDOOR_e = 0x0003,
    dEvtCmd_INGETITEM_e = 0x0004,
    dEvtCmd_INCATCH_e = 0x0006,
};

/* dEvt_info_c (HD: vtable at the end, 0x18) [v nz]: callbacks are guest function addresses */
struct dEvt_info_c {
    typedef s16 (*CallbackFunc)(void*, int);
    /* 0x00 */ be<u16> mCommand;
    /* 0x02 */ be<u16> mCondition;
    /* 0x04 */ be<s16> mEventId;
    /* 0x06 */ be<u8> mMapToolId;
    /* 0x07 */ be<s8> mIndex;
    /* 0x08 */ be<u32> mpEventCB;
    /* 0x0C */ be<u32> mpCheckCB;
    /* 0x10 */ be<u32> mpPhotoCB;
    /* 0x14 */ be<u32> __vtbl;
    s16 getEventId() { return mEventId; }
    void setEventId(s16 id) { mEventId = id; }
    void onCondition(u16 cond) { mCondition |= cond; }
    u16 getCommand() { return mCommand; }
    BOOL checkCommandTalk() { return mCommand == dEvtCmd_INTALK_e; }
    BOOL checkCommandDemoAccrpt() { return mCommand == dEvtCmd_INDEMO_e; }
    void setXyEventCB(CallbackFunc cb) { mpEventCB = gabi::guest_fn((const void*)cb); }
    void setXyCheckCB(CallbackFunc cb) { mpCheckCB = gabi::guest_fn((const void*)cb); }
};
WWHD_SIZE(dEvt_info_c, 0x18);

/* actor_attention_types (unchanged from GameCube) [v nz] */
struct actor_attention_types {
    /* 0x00 */ be<u8> distances[8];
    /* 0x08 */ cXyz position;
    /* 0x14 */ be<u32> flags;
};
WWHD_SIZE(actor_attention_types, 0x18);

struct request_of_phase_process_class {
    /* 0x0 */ be<u32> mpHandlerTable;
    /* 0x4 */ be<s32> id;
};

WWHD_OPAQUE(JKRSolidHeap);

/* fopAc_cullSizeBox / fopAc_cullSizeSphere (unchanged from GameCube) */
struct fopAc_cullSizeBox {
    /* 0x00 */ cXyz min;
    /* 0x0C */ cXyz max;
};
struct fopAc_cullSizeSphere {
    /* 0x00 */ cXyz center;
    /* 0x0C */ be<f32> radius;
};

/* GameCube: `leafdraw_class base` as the first member; here a base class, so the process fields
 * (mParameters, mProcName, __vtbl, ...) are reached directly. */
struct fopAc_ac_c : leafdraw_class {
    /* 0x0C4 */ be<s32> actor_type;        /* GameCube 0x0C0 */
    /* 0x0C8 */ create_tag_class actor_tag;
    /* 0x0DC */ create_tag_class draw_tag;
    /* 0x0F0 */ be<u32> sub_method;        /* actor_method_class* (profile +0x24) */
    /* 0x0F4 */ gptr<JKRSolidHeap> heap;   /* GameCube 0x0F0 */
    /* 0x0F8 */ dEvt_info_c eventInfo;
    /* 0x110 */ dKy_tevstr_c tevStr;       /* GameCube 0x10C */
    /* 0x2D8 */ be<u16> setID;
    /* 0x2DA */ be<u8> group;
    /* 0x2DB */ be<u8> cullType;
    /* 0x2DC */ be<u8> demoActorID;
    /* 0x2DD */ be<s8> argument;
    /* 0x2DE */ be<u8> gbaName;
    /* 0x2DF */ u8 _2DF;
    /* 0x2E0 */ be<u32> actor_status;
    /* 0x2E4 */ be<u32> actor_condition;
    /* 0x2E8 */ be<u32> parentActorID;
    /* 0x2EC */ actor_place home;
    /* 0x300 */ actor_place old;
    /* 0x314 */ actor_place current;
    /* 0x328 */ csXyz shape_angle;
    /* 0x32E */ u8 _32E[2];
    /* 0x330 */ cXyz scale;
    /* 0x33C */ cXyz speed;
    /* 0x348 */ be<u32> cullMtx;
    union {
        /* 0x34C */ fopAc_cullSizeBox box;
        /* 0x34C */ fopAc_cullSizeSphere sphere;
    } cull;
    /* 0x364 */ be<f32> cullSizeFar;
    /* 0x368 */ be<u32> model;
    /* 0x36C */ be<u32> jntHit;
    /* 0x370 */ be<f32> speedF;
    /* 0x374 */ be<f32> gravity;
    /* 0x378 */ be<f32> maxFallSpeed;
    /* 0x37C */ cXyz eyePos;
    /* 0x388 */ actor_attention_types attention_info;
    /* 0x3A0 */ be<s8> max_health;
    /* 0x3A1 */ be<s8> health;
    /* 0x3A2 */ u8 _3A2[2];
    /* 0x3A4 */ be<s32> itemTableIdx;
    /* 0x3A8 */ be<u8> stealItemBitNo;
    /* 0x3A9 */ be<s8> stealItemLeft;
    /* 0x3AA */ be<u8> mHdConstructState; /* HD only: 0 raw, 1 constructed, 2 destroyed (fopAc_Delete calls the virtual destructor when 1) */
    /* 0x3AB */ u8 _3AB;
};
WWHD_SIZE(fopAc_ac_c, 0x3AC);
WWHD_OFFSET(fopAc_ac_c, mParameters, 0x0B0);
WWHD_OFFSET(fopAc_ac_c, __vtbl, 0x0B4);
WWHD_OFFSET(fopAc_ac_c, actor_type, 0x0C4);
WWHD_OFFSET(fopAc_ac_c, sub_method, 0x0F0);
WWHD_OFFSET(fopAc_ac_c, heap, 0x0F4);
WWHD_OFFSET(fopAc_ac_c, cull, 0x34C);
WWHD_OFFSET(fopAc_ac_c, mHdConstructState, 0x3AA);
WWHD_OFFSET(fopAc_ac_c, tevStr, 0x110);
WWHD_OFFSET(fopAc_ac_c, home, 0x2EC);
WWHD_OFFSET(fopAc_ac_c, current, 0x314);
WWHD_OFFSET(fopAc_ac_c, speedF, 0x370);
WWHD_OFFSET(fopAc_ac_c, eyePos, 0x37C);
WWHD_OFFSET(fopAc_ac_c, eventInfo, 0xF8);
WWHD_OFFSET(fopAc_ac_c, attention_info, 0x388);

enum {
    fopAcCnd_NOEXEC_e = 0x02,
    fopAcCnd_INIT_e = 0x08,
    fopAcCnd_NODRAW_e = 0x04, /* HD: fopAc_Draw sets/clears bit 0x04 */
};

/* 025D4ED0 fopAc_ac_c::fopAc_ac_c() */
inline void fopAc_ac_c_ct(fopAc_ac_c* p) { gabi::call(0x025D4ED0, p); }

/* fopEn_enemy_c: GameCube 0x290..0x2AC, +0x11C like the rest of the base (size 0x3C8). Confirmed by
 * verified code: d_a_mo2 create (initBt: mBtBodyR, mBtHeight) and exec (setBtAttackData, setBtMaxDis,
 * setBtNowFrame); every enemy class's first member at 0x3C8 (nz, wz, gnd, bmdfoot, fganon, npc_ji1). */
struct fopEn_enemy_c : fopAc_ac_c {
    enum ParryOpeningType {
        OPENING_NONE = 0,
        OPENING_JUMP_PARRY = 1,
        OPENING_ROLL_PARRY = 2,
        OPENING_VERTICAL_JUMP_PARRY = 3,
        OPENING_GANONDORF_FINISHER_PARRY = 4,
    };
    void initBt(f32 height, f32 radius) {
        setBtHeight(height);
        setBtBodyR(radius);
    }
    f32 getBtHeight() { return mBtHeight; }
    void setBtHeight(f32 height) { mBtHeight = height; }
    f32 getBtBodyR() { return mBtBodyR; }
    void setBtBodyR(f32 radius) { mBtBodyR = radius; }
    void setBtAttackData(f32 startFrame, f32 endFrame, f32 maxDis, u8 attackType) {
        setBtStartFrame(startFrame);
        setBtEndFrame(endFrame);
        setBtMaxDis(maxDis);
        setBtAttackType(attackType);
    }
    f32 getBtStartFrame() { return mBtStartFrame; }
    void setBtStartFrame(f32 frame) { mBtStartFrame = frame; }
    f32 getBtEndFrame() { return mBtEndFrame; }
    void setBtEndFrame(f32 frame) { mBtEndFrame = frame; }
    f32 getBtNowFrame() { return mBtNowFrame; }
    void setBtNowFrame(f32 frame) { mBtNowFrame = frame; }
    u8 getBtAttackType() { return mBtAttackType; }
    void setBtAttackType(u8 type) { mBtAttackType = type; }
    f32 getBtMaxDis() { return mBtMaxDis; }
    void setBtMaxDis(f32 dis) { mBtMaxDis = dis; }

    /* 0x3AC */ be<f32> mBtHeight;
    /* 0x3B0 */ be<f32> mBtBodyR;
    /* 0x3B4 */ be<f32> mBtMaxDis;
    /* 0x3B8 */ be<f32> mBtStartFrame;
    /* 0x3BC */ be<f32> mBtEndFrame;
    /* 0x3C0 */ be<f32> mBtNowFrame;
    /* 0x3C4 */ be<u8> mBtAttackType;
    /* 0x3C5 */ u8 _3C5[3];
};
WWHD_SIZE(fopEn_enemy_c, 0x3C8);
