// Sea barrel course actor. HD: the GameCube members are +0x118; size0x5CC.
#pragma once
#include "gc_api/gc_api.h"

namespace daComing2 {
struct _barrel_info_ {
    /* 0x00 */ be<u32> mBarrelId;  /* was unk_00: the barrel's process ID (fpcM_ERROR_PROCESS_ID_e = none); init_barrelN_info 0211321C clears it, coming_game_main 02114630 stores the made barrel, barrel_execute 02113754 fopAcM_SearchByID it (tww) */
    /* 0x04 */ be<s16> mExitTimer;  /* was unk_04: coming_game_main 02114630 sets 0x93 - mBarrelIdx; barrel_execute 02113754 counts it down and calls request_barrel_exitN at 0 (tww) */
    /* 0x06 */ be<u8> mItemGive;  /* was unk_06: barrel_execute 02113754 copies the barrel's item-give flag (tww barrel->chk_item_give()) */
    /* 0x07 */ u8 padding;
    /* 0x08 */ cXyz mPos;  /* was unk_08: coming_game_main 02114630 stores the barrel's position; renew_scope_info 021134F8 measures from the previous barrel's mPos */
};
struct _start_buoy_ {
    /* 0x00 */ be<u32> mBuoyId;  /* was unk_00: the start buoy's process ID (fpcM_ERROR_PROCESS_ID_e = none); init_flag_info 021132F4 clears both, coming_setF_main 02114298 stores the made buoys (tww) */
    /* 0x04 */ cXyz mPos;  /* was unk_04: coming_setF_main 02114298 stores the buoy position; coming_checkS_main 021144C4 checks the ship crossing the line mStartBuoy[0].mPos - mStartBuoy[1].mPos (tww checkLineCrossXZ) */
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
    /* 0x3AC */ be<s32> mGameCount;  /* was unk_294: coming_game_init 02114604 counts it up once per game start (tww unk_294++) */
    /* 0x3B0 */ be<s8> mStartRoomNo;  /* was unk_298: coming_setF_init 021140E0 stores the player's room; coming_game_init 02114604 copies it back into mPlayerRoomNo */
    /* 0x3B1 */ be<s8> mPlayerRoomNo;  /* was unk_299: the player's last room: _create 021133BC -1; coming_wait_main 02113FD4 / coming_checkS_main 021144C4 / coming_clear_init 02113F18 store current.roomNo; coming_wait_main starts the course when the player has left it (tww) */
    /* 0x3B2 */ be<s16> mMakeReq;  /* was unk_29A: barrel_execute 02113754 / coming_game_init 02114604 set 1; coming_game_main 02114630 makes the next barrel while it is 1 and clears it once made; init_coming_info 02113288 0 */
    /* 0x3B4 */ be<s16> mScopeSide;  /* was unk_29C: init_coming_info 02113288: cM_rndF(2) & 1; renew_scope_info 021134F8 picks the turn sign ((mBarrelIdx + it) & 1 ? -0x546 : 0x546) */
    /* 0x3B6 */ u8 pad3B6[2];
    /* 0x3B8 */ be<f32> mSpeed;  /* was unk_2A0: renew_scope_info 021134F8: the ship's speed (tww get_speed(this)); coming_wait_main 02113FD4 needs it above 35; coming_setF_main 02114298 places the start ahead by it * 120 */
    /* 0x3BC */ be<s16> mBarrelIdx;  /* was unk_2A4: the next barrel's index in mBarrelInfo (barrels made so far): init_coming_info 02113288 / coming_wait_init 02113FBC 0; coming_game_main 02114630 advances it; barrel_execute 02113754 ends at 15; make_coming_param 02113E50 / renew_scope_info 021134F8 index by it */
    /* 0x3BE */ be<s16> mWaitTimer;  /* was unk_2A6: init_coming_info 02113288 / coming_wait_init 02113FBC 600; coming_wait_main 02113FD4 counts it down and starts the course only at 0 */
    /* 0x3C0 */ be<s16> mMakeTimer;  /* was unk_2A8: coming_game_main 02114630: frames since the last barrel attempt, retried at 60 (tww unk_2A8++ >= 60); init_coming_info 02113288 / coming_game_init 02114604 0 */
    /* 0x3C2 */ be<s16> unk_2AA;
    /* 0x3C4 */ _barrel_info_
        mBarrelInfo[20]; // Capacity 20; the actual active pool has 15 entries.
    struct _start_stat_ {
        be<s16> mAngleY;  /* was unk_00: coming_wait_main 02113FD4 / _create 021133BC store shape_angle.y with mStartPos; renew_scope_info 021134F8 and the buoy / barrel rotations start from it (tww) */
    } mStartStat;
    /* 0x556 */ u8 pad556[2];
    /* 0x558 */ cXyz mStartPos;  /* was unk_440: _create 021133BC / coming_wait_main 02113FD4: current.pos saved together with mStartStat (the start angle) when the course starts (tww) */
    /* 0x564 */ _start_buoy_ mStartBuoy[2];
    /* 0x584 */ be<s16> mCheckTimer;  /* was unk_46C: coming_checkS_init 021144B4 0; coming_checkS_main 021144C4 counts it up to 180 */
    /* 0x586 */ be<s16> mProcessIdx;  /* was unk_46E: coming_process_init 02113304 stores the process index; coming_process_main 02113680 calls main_table[it] (as daComing3::Act_c::mProcessIdx) */
    /* 0x588 */ u8 pad588[6];
    /* 0x58E */ be<u8> unk_476;
    /* 0x58F */ u8 pad58F[0x31];
    /* 0x5C0 */ cXyz mScopePos;  /* was unk_4A8: renew_scope_info 021134F8: the point ahead of the ship (matrix times the forward vector, tww cMtx_multVec(.., &unk_4A8)); tww make_coming_param places the next barrel from it */
};
WWHD_SIZE(_barrel_info_, 0x14);
WWHD_SIZE(_start_buoy_, 0x10);
WWHD_SIZE(Act_c, 0x5CC); // HD: actor profile size.
WWHD_OFFSET(Act_c, mGameCount, 0x3AC);
WWHD_OFFSET(Act_c, mBarrelInfo, 0x3C4);
WWHD_OFFSET(Act_c, mStartStat, 0x554);
WWHD_OFFSET(Act_c, mStartBuoy, 0x564);
WWHD_OFFSET(Act_c, mCheckTimer, 0x584);
WWHD_OFFSET(Act_c, mScopePos, 0x5C0);
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
