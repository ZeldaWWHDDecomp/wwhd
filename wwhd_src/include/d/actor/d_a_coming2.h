// Sea barrel course actor. HD: the GameCube members are +0x118; size0x5CC.
#pragma once
#include "gc_api/gc_api.h"

namespace daComing2 {
struct _barrel_info_ {
    be<u32> unk_00;
    be<s16> unk_04;
    be<u8> unk_06;
    u8 padding;
    cXyz unk_08;
};
struct _start_buoy_ {
    be<u32> unk_00;
    cXyz unk_04;
};
struct Act_c : fopAc_ac_c {
    // HD: BOOL lifecycle results and full signed process argument retain the verified register
    // widths.
    BOOL coming_process_init(s32 process);
    void coming_process_main();
    BOOL request_barrel_exitN(s32 index);
    BOOL request_all_barrel_exit();
    BOOL request_all_flag_exit();
    void barrel_execute(s32 index);
    void barrel_main();
    void coming_clear_main();
    s32 _create();
    void chase_ship();
    BOOL check_in_large_sea(const cXyz *position);
    BOOL make_coming_param(cXyz *position, be<s32> *type, be<s32> *item, be<u8> *hasFlag);
    void coming_wait_main();
    BOOL coming_setF_init();
    BOOL _execute();
    void renew_scope_info();
    u32 no_ship_obstacle(cXyz *position);
    BOOL get_water_height(be<f32> *height, const cXyz *position);
    void coming_checkS_main();
    void coming_game_main();
    void coming_setF_main();
    void init_barrelN_info(s32 index);
    void init_barrel_info();
    void init_coming_info();
    void init_flag_info();
    BOOL coming_clear_init();
    BOOL coming_wait_init();
    BOOL coming_checkS_init();
    BOOL coming_game_init();
    /* 0x3AC */ be<s32> unk_294;
    /* 0x3B0 */ be<s8> unk_298, unk_299;
    /* 0x3B2 */ be<s16> unk_29A, unk_29C;
    u8 pad3B6[2];
    /* 0x3B8 */ be<f32> unk_2A0;
    /* 0x3BC */ be<s16> unk_2A4, unk_2A6, unk_2A8, unk_2AA;
    /* 0x3C4 */ _barrel_info_
        mBarrelInfo[20]; // Capacity 20; the actual active pool has 15 entries.
    struct _start_stat_ {
        be<s16> unk_00;
    } mStartStat;
    u8 pad556[2];
    /* 0x558 */ cXyz unk_440;
    /* 0x564 */ _start_buoy_ mStartBuoy[2];
    /* 0x584 */ be<s16> unk_46C, unk_46E;
    u8 pad588[6];
    /* 0x58E */ be<u8> unk_476;
    u8 pad58F[0x31];
    /* 0x5C0 */ cXyz unk_4A8;
};
WWHD_SIZE(_barrel_info_, 0x14);
WWHD_SIZE(_start_buoy_, 0x10);
WWHD_SIZE(Act_c, 0x5CC); // HD: actor profile size.
WWHD_OFFSET(Act_c, unk_294, 0x3AC);
WWHD_OFFSET(Act_c, mBarrelInfo, 0x3C4);
WWHD_OFFSET(Act_c, mStartStat, 0x554);
WWHD_OFFSET(Act_c, mStartBuoy, 0x564);
WWHD_OFFSET(Act_c, unk_46C, 0x584);
WWHD_OFFSET(Act_c, unk_4A8, 0x5C0);
} // namespace daComing2

namespace daComing2 {
struct SafetyCallback {
    gptr<cXyz> pos;
    be<s32> ret;
};

} // namespace daComing2
using daComing2::SafetyCallback;
// HD: free namespace callback bindings avoid autoglue treating namespace qualification as a this
// method.
BOOL coming2_position_is_none_obj(cXyz *);
void *coming2_position_is_safety_call_back(fopAc_ac_c *, SafetyCallback *);
WWHD_SIZE(SafetyCallback, 8);
// HD: guest eight-byte GHS descriptors preserve direct and virtual tails.
struct Coming2MethodHD {
    be<s16> adjustment, slot;
    be<u32> target;
};
WWHD_SIZE(Coming2MethodHD, 8);
struct Coming2BarrelHD : fopAc_ac_c {
    u8 pad[0x590 - 0x3AC];
    be<u8> mExitReq;
    u8 pad591;
    be<u8> mItemGive;
};
WWHD_OFFSET(Coming2BarrelHD, mExitReq, 0x590);
WWHD_OFFSET(Coming2BarrelHD, mItemGive, 0x592);
struct Coming2ShipHD : fopAc_ac_c {
    u8 pad[0x704 - 0x3AC];
    be<u32> mTornadoActor;
    u8 pad708[4];
    be<u32> mWhirlActor;
};
WWHD_OFFSET(Coming2ShipHD, mTornadoActor, 0x704);
WWHD_OFFSET(Coming2ShipHD, mWhirlActor, 0x70C);
struct Coming2PlayHD {
    u8 pad0[0x5292];
    be<u8> mEventRunning;
    u8 pad5293[0x5B2C - 0x5293];
    gptr<fopAc_ac_c> mpPlayer;
    u8 pad5B30[0x5CD8 - 0x5B30];
    be<u32> mPlayerStatus;
};
WWHD_OFFSET(Coming2PlayHD, mpPlayer, 0x5B2C);
WWHD_OFFSET(Coming2PlayHD, mPlayerStatus, 0x5CD8);
struct Coming2GroundHD {
    gptr<u8> mGroup, mPoly;
    be<u32> mActorPid;
    u8 padC[4];
    be<u32> mVtable;
    u8 mPolyInfo[12];
    be<u32> mCheckVtable;
    cXyz mPos;
    u8 pad30[0x40 - 0x30];
    be<u32> mGroupVtable;
    be<u8> mGroupFlags[8];
    be<u32> mPolyVtable, mFlags;
};
WWHD_OFFSET(Coming2GroundHD, mPos, 0x24);
WWHD_OFFSET(Coming2GroundHD, mPolyVtable, 0x4C);
WWHD_SIZE(Coming2GroundHD, 0x54);
struct Coming2WaterHD {
    u8 pad0[0x20];
    be<u32> mVtable, mGroupVtable;
    u8 pad28[8];
    be<u32> mPolyVtable;
    u8 pad34[4];
    cXyz mPos;
    be<f32> mUpper, mHeight;
};
WWHD_OFFSET(Coming2WaterHD, mPos, 0x38);
WWHD_SIZE(Coming2WaterHD, 0x4C);
struct Coming2LineHD {
    be<u32> mPoly, mGroup;
    u8 pad8[8];
    be<u32> mVtable;
    u8 pad14[12];
    be<u32> mCheckVtable;
    u8 pad24[0x58 - 0x24];
    be<u32> mPolyVtable;
    be<u8> mPass[7];
    u8 pad63;
    be<u32> mGroupVtable, mFlags;
};
WWHD_OFFSET(Coming2LineHD, mPolyVtable, 0x58);
WWHD_SIZE(Coming2LineHD, 0x6C);
struct Coming2StaticsHD {
    be<f32> minimum, maximum;
    be<u8> one, zero;
    u8 padA[2];
    be<u32> counters[4];
};
WWHD_SIZE(Coming2StaticsHD, 0x1C);

struct Coming2GuardsHD {
    be<u32> line, water, ground;
};
WWHD_SIZE(Coming2GuardsHD, 12);
inline Coming2GuardsHD *coming2Guards() { return gabi::at<Coming2GuardsHD>(0x10462E64); }
inline Coming2StaticsHD *coming2Statics() { return gabi::at<Coming2StaticsHD>(0x10462D38); }
s32 coming2SearchByID(u32 id, void *output);
void coming2ActorConstruct(fopAc_ac_c *actor);
u32 coming2ActorSearch(void *callback, void *data);
u32 coming2GetEventReg(void *event, u32 flag);
void coming2SetEventReg(void *event, u32 flag, u32 value);
s32 coming2IsActor(fopAc_ac_c *actor);
void coming2RegisterStatic(void *record);
void coming2InitializeOne(void *object);
void coming2InitializeZero(void *object);
void coming2GroundDestroy(void *object, s32 flags);
void coming2Delete(void *object);
void coming2CheckDestroy(void *object, s32 flags);
void coming2ActorDestroy(fopAc_ac_c *actor, s32 flags);
f32 coming2SquareRoot(f32 value);
void coming2MatrixTrans(void *matrix, f32 x, f32 y, f32 z);
void coming2MatrixYRotate(void *matrix, s32 angle);
void coming2MatrixCopy(const void *source, void *destination);
void coming2MatrixVector(const void *matrix, const cXyz *source, cXyz *destination);
void coming2LineConstruct(void *object);
void coming2LineSet(void *object, const cXyz *start, const cXyz *end, u32 actor);
u32 coming2LineCross(void *bg, void *check);
s32 coming2SeaCheckArea(f32 x, f32 z);
f32 coming2SeaWave(f32 x, f32 z);
void coming2WaterConstruct(void *object);
s32 coming2WaterCheck(void *bg, void *check);
void coming2GroundConstruct(void *object);
f32 coming2GroundCross(void *bg, void *check);
void coming2AngleConstruct(csXyz *rotation, s32 x, s32 y, s32 z);
u32 coming2ActorCreate(s32 name, u32 parameters, const cXyz *position, s32 room,
                       const csXyz *rotation, void *scale, s32 subtype, u32 parent);
f32 coming2RandomFX(f32 range);
f32 coming2RandomF(f32 range);
// HD: this translation unit retains guest callback/vtable identities.
enum : u32 {
    COMING2_INIT_TABLE = 0x1000C778,
    COMING2_LINE_CHECK_DTOR_VTABLE = 0x1000C7A4,
    COMING2_PASS_DTOR_VTABLE = 0x1000C7B4,
    COMING2_GROUND_CHECK_DTOR_VTABLE = 0x1000C7D4,
    COMING2_GROUND_GROUP_DTOR_VTABLE = 0x1000C7F4,
    COMING2_GROUND_BASE_VTABLE = 0x1000C804,
    COMING2_GROUND_CHECK_VTABLE = 0x1000C814,
    COMING2_GROUND_POLY_VTABLE = 0x1000C824,
    COMING2_GROUND_GROUP_VTABLE = 0x1000C834,
    COMING2_WATER_CHECK_VTABLE = 0x1000C844,
    COMING2_WATER_GROUP_VTABLE = 0x1000C864,
    COMING2_LINE_POLY_DTOR_VTABLE = 0x1000C8D4,
    COMING2_LINE_BASE_VTABLE = 0x1000C8E4,
    COMING2_LINE_CHECK_VTABLE = 0x1000C8F4,
    COMING2_LINE_GROUP_VTABLE = 0x1000C904,
    COMING2_LINE_POLY_VTABLE = 0x1000C914,
    COMING2_ACTOR_VTABLE = 0x1000C924,
    COMING2_MAIN_TABLE = 0x1000C944,
    COMING2_MINIMUM_INITIAL = 0x1000C994,
    COMING2_MAXIMUM_INITIAL = 0x1000C998,
    COMING2_SEARCH_BY_ID = 0x025E1234,
    COMING2_SAFETY_CALLBACK = 0x021141AC,
};

WWHD_SIZE(Coming2BarrelHD, 0x593);
WWHD_SIZE(Coming2ShipHD, 0x710);
WWHD_SIZE(Coming2PlayHD, 0x5CDC);

// HD: barrel process number differs from GameCube's process ordering.
enum Coming2Constants : u32 {
    fpcNm_Obj_Barrel2_e = 0x1C9,
    dItemNo_RECOVER_FAIRY_e = 0x16,
    COMING2_SHIP_RIDE = 0x10000,
    COMING2_POINT_EVENT_REG = 0x7EFF,
};
