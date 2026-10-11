/* d_save — Stage, dungeon and room flags, restart positions and saved memories.
 * GameCube names with verified WWHD layouts and behavior. Unit: d_save. */
#include "bindings.h"
#include "d/d_save_readable.h"

namespace d_save_cpp {
#include "d/d_save_local.h"

static inline void setWordBit(u32 a, u32 no, u32 lim, const char* file, s32 line, const char* msg) {
    if (no >= lim) saveAssert(file, line, msg);
    be<u32>& flags = indexed(view<be<u32>>(a), (u32)((s32)no >> 5));
    flags = (u32)flags | (1u << (no & 31));
}
static inline void clearWordBit(u32 a, u32 no, u32 lim, const char* file, s32 line, const char* msg) {
    if (no >= lim) saveAssert(file, line, msg);
    be<u32>& flags = indexed(view<be<u32>>(a), (u32)((s32)no >> 5));
    flags = (u32)flags & ~(1u << (no & 31));
}
static inline BOOL testWordBit(u32 a, u32 no, u32 lim, const char* file, s32 line, const char* msg) {
    if (no >= lim) saveAssert(file, line, msg);
    be<u32>& flags = indexed(view<be<u32>>(a), (u32)((s32)no >> 5));
    return ((u32)flags & (1u << (no & 31))) ? TRUE : FALSE;
}
static inline BOOL reverseWordBit(u32 a, u32 no, u32 lim, const char* file, s32 line, const char* msg) {
    if (no >= lim) saveAssert(file, line, msg);
    be<u32>& flags = indexed(view<be<u32>>(a), (u32)((s32)no >> 5));
    u32 sw = 1u << (no & 31);
    u32 v = (u32)flags ^ sw;
    flags = v;
    return (v & sw) ? TRUE : FALSE;
}

// HD: stores 32 memory item flags rather than the GameCube 64.
/* 025B8BC4 */
void d_save_cpp::dSv_memBit_c::init() {
    mTbox = 0;
    for (u32 i = 0; i < 4; i++) indexed(mSwitch, i) = 0;
    mItem[0] = 0;
    for (u32 i = 0; i < 2; i++) indexed(mVisitedRoom, i) = 0;
    mKeyNum = 0;
    mDungeonItem = 0;
}

/* 025B8C0C */
void d_save_cpp::dSv_memBit_c::onTbox(u32 no) {
    setScalarWordBit(gabi::ea(&mTbox), no, 0x20, "d_save.cpp", 0x912, "0 <= i_no && i_no < 32");
}

/* 025B8C74 */
BOOL d_save_cpp::dSv_memBit_c::isTbox(u32 no) {
    return testScalarWordBit(gabi::ea(&mTbox), no, 0x20, "d_save.cpp", 0x92E, "0 <= i_no && i_no < 32");
}

/* 025B8CE0 */
void d_save_cpp::dSv_memBit_c::onSwitch(u32 no) {
    setWordBit(gabi::ea(mSwitch), no, 0x80, "d_save.cpp", 0x94D, "0 <= i_no && i_no < 128");
}

/* 025B8D54 */
void d_save_cpp::dSv_memBit_c::offSwitch(u32 no) {
    clearWordBit(gabi::ea(mSwitch), no, 0x80, "d_save.cpp", 0x95B, "0 <= i_no && i_no < 128");
}

/* 025B8DC8 */
BOOL d_save_cpp::dSv_memBit_c::isSwitch(u32 no) {
    return testWordBit(gabi::ea(mSwitch), no, 0x80, "d_save.cpp", 0x969, "0 <= i_no && i_no < 128");
}

/* 025B8E40 */
BOOL d_save_cpp::dSv_memBit_c::revSwitch(u32 no) {
    return reverseWordBit(gabi::ea(mSwitch), no, 0x80, "d_save.cpp", 0x977, "0 <= i_no && i_no < 128");
}

/* 025B8EC0 */
void d_save_cpp::dSv_memBit_c::onItem(u32 no) {
    setWordBit(gabi::ea(mItem), no, 0x20, "d_save.cpp", 0x98A, "0 <= i_no && i_no < ITEM_MAX");
}

/* 025B8F34 */
BOOL d_save_cpp::dSv_memBit_c::isItem(u32 no) {
    return testWordBit(gabi::ea(mItem), no, 0x20, "d_save.cpp", 0x9A6, "0 <= i_no && i_no < ITEM_MAX");
}

/* 025B8FAC */
void d_save_cpp::dSv_memBit_c::onVisitedRoom(u32 no) {
    setWordBit(gabi::ea(mVisitedRoom), no, 0x40, "d_save.cpp", 0x9C6, "0 <= i_no && i_no < 64");
}

/* 025B9020 */
BOOL d_save_cpp::dSv_memBit_c::isVisitedRoom(u32 no) {
    return testWordBit(gabi::ea(mVisitedRoom), no, 0x40, "d_save.cpp", 0x9E2, "0 <= i_no && i_no < 64");
}

/* 025B9098 */
void d_save_cpp::dSv_memBit_c::onDungeonItem(u32 no) {
    setByteBit(gabi::ea(&mDungeonItem), no, 6, "d_save.cpp", 0xA02, "0 <= i_no && i_no < 6");
}

/* 025B9100 */
BOOL d_save_cpp::dSv_memBit_c::isDungeonItem(u32 no) {
    return testByteBit(gabi::ea(&mDungeonItem), no, 6, "d_save.cpp", 0xA1F, "0 <= i_no && i_no < 6");
}

/* 025B9170 */
void d_save_cpp::dSv_memory_c::init() {
    view<dSv_memBit_c>(gabi::ea(&mMembit))->init();
}

// HD: clears dungeon state only when the full input differs from its signed saved stage byte.
/* 025B9174 */
s32 d_save_cpp::dSv_danBit_c::init(u32 stageNo) {
    if (stageNo != (u32)(s32)(s8)mStageNo) {
        mStageNo = (s8)stageNo;
        mSwitch[0] = 0;
        mGbaRupeeCount = 0;
        mSwitch[1] = 0;
        return 1;
    }
    return 0;
}

/* 025B91AC */
void d_save_cpp::dSv_danBit_c::onSwitch(u32 no) {
    setWordBit(gabi::ea(mSwitch), no, 0x40, "d_save.cpp", 0xB4D, "0 <= i_no && i_no < 64");
}

/* 025B9220 */
void d_save_cpp::dSv_danBit_c::offSwitch(u32 no) {
    clearWordBit(gabi::ea(mSwitch), no, 0x40, "d_save.cpp", 0xB5B, "0 <= i_no && i_no < 64");
}

/* 025B9294 */
BOOL d_save_cpp::dSv_danBit_c::isSwitch(u32 no) {
    return testWordBit(gabi::ea(mSwitch), no, 0x40, "d_save.cpp", 0xB69, "0 <= i_no && i_no < 64");
}

/* 025B930C */
BOOL d_save_cpp::dSv_danBit_c::revSwitch(u32 no) {
    return reverseWordBit(gabi::ea(mSwitch), no, 0x40, "d_save.cpp", 0xB77, "0 <= i_no && i_no < 64");
}

/* 025B938C */
void d_save_cpp::dSv_zoneBit_c::init() {
    for (u32 i = 0; i < 3; i++) indexed(mSwitch, i) = 0;
    mItem = 0;
}

/* 025B93B0 */
void d_save_cpp::dSv_zoneBit_c::clearRoomSwitch() {
    mSwitch[2] = 0;
}

/* u16 switch words of the zone: word (no >> 4), bit (no & 15) */

/* 025B93BC */
void d_save_cpp::dSv_zoneBit_c::onSwitch(u32 no) {
    if (no >= 0x30) saveAssert("d_save.cpp", 0xBA3, "0 <= i_no && i_no < SWITCH_MAX");
    be<u16>& flags = indexed(mSwitch, (u32)((s32)no >> 4));
    flags = (u16)((u16)flags | (1u << (no & 15)));
}

/* 025B9430 */
void d_save_cpp::dSv_zoneBit_c::offSwitch(u32 no) {
    if (no >= 0x30) saveAssert("d_save.cpp", 0xBB1, "0 <= i_no && i_no < SWITCH_MAX");
    be<u16>& flags = indexed(mSwitch, (u32)((s32)no >> 4));
    flags = (u16)((u16)flags & ~(1u << (no & 15)));
}

/* 025B94A4 */
BOOL d_save_cpp::dSv_zoneBit_c::isSwitch(u32 no) {
    if (no >= 0x30) saveAssert("d_save.cpp", 0xBBF, "0 <= i_no && i_no < SWITCH_MAX");
    return ((u16)indexed(mSwitch, (u32)((s32)no >> 4)) & (1u << (no & 15))) ? TRUE : FALSE;
}

/* 025B951C */
BOOL d_save_cpp::dSv_zoneBit_c::revSwitch(u32 no) {
    if (no >= 0x30) saveAssert("d_save.cpp", 0xBCD, "0 <= i_no && i_no < SWITCH_MAX");
    be<u16>& flags = indexed(mSwitch, (u32)((s32)no >> 4));
    u32 sw = 1u << (no & 15);
    u16 v = (u16)((u16)flags ^ sw);
    flags = v;
    return (v & sw) ? TRUE : FALSE;
}

/* 025B95A0 */
void d_save_cpp::dSv_zoneBit_c::onItem(u32 no) {
    if (no >= 0x10) saveAssert("d_save.cpp", 0xBE0, "0 <= i_no && i_no < 16");
    mItem = (u16)((u16)mItem | slw(1, no));
}

/* 025B9608 */
BOOL d_save_cpp::dSv_zoneBit_c::isItem(u32 no) {
    if (no >= 0x10) saveAssert("d_save.cpp", 0xBFC, "0 <= i_no && i_no < 16");
    return ((u16)mItem & slw(1, no)) ? TRUE : FALSE;
}

/* 025B9674 */
void d_save_cpp::dSv_zoneActor_c::init() {
    for (u32 i = 0; i < 0x10; i++) indexed(mActorFlags, i) = 0;
}

/* 025B9690 */
void d_save_cpp::dSv_zoneActor_c::on(u32 id) {
    setWordBit(gabi::ea(mActorFlags), id, 0x200, "d_save.cpp", 0xC29, "0 <= i_id && i_id < ACTOR_MAX");
}

/* 025B9700 */
BOOL d_save_cpp::dSv_zoneActor_c::is(u32 id) {
    return testWordBit(gabi::ea(mActorFlags), id, 0x200, "d_save.cpp", 0xC45, "0 <= i_id && i_id < ACTOR_MAX");
}

// HD: clears both room-flag blocks during construction before marking the room unused.
/* 025B9774 */
u32 d_save_cpp::dSv_zone_c::__ct() {
    u32 t = gabi::ea(this);
    if (t == 0) {
        t = saveAllocate(0x4C);
        if (t == 0) return 0;
    }
    dSv_zone_c* zone_p = view<dSv_zone_c>(t);
    zone_p->mRoomNo = 0;
    saveZero(gabi::ea(&zone_p->mZoneBit), sizeof(dSv_zoneBit_c));
    saveZero(gabi::ea(&zone_p->mZoneActor), sizeof(dSv_zoneActor_c));
    zone_p->mRoomNo = -1;
    return t;
}

/* 025B97DC */
void d_save_cpp::dSv_zone_c::init(u32 roomNo) {
    mRoomNo = (s8)roomNo;
    view<dSv_zoneBit_c>(gabi::ea(&mZoneBit))->init();
    view<dSv_zoneActor_c>(gabi::ea(&mZoneActor))->init();
}

/* 025B9810 */
void d_save_cpp::dSv_restart_c::setRoom(const cXyz* pos, u32 angle, u32 roomNo) {
    mRestartRoom = (s8)roomNo;
    wordPosition(&mRestartPos).x = (u32)wordPosition(pos).x;
    wordPosition(&mRestartPos).y = (u32)wordPosition(pos).y;
    u32 z = (u32)wordPosition(pos).z;
    mRestartAngle = (s16)angle;
    wordPosition(&mRestartPos).z = z;
}

/* 025B9834 */
void d_save_cpp::dSv_restart_c::setRestartOption(u32 option, const cXyz* pos, u32 angle, u32 roomNo) {
    wordPosition(&mOptionRoomPos).x = (u32)wordPosition(pos).x;
    wordPosition(&mOptionRoomPos).y = (u32)wordPosition(pos).y;
    u32 z = (u32)wordPosition(pos).z;
    mOptionRoomAngleY = (s16)angle;
    wordPosition(&mOptionRoomPos).z = z;
    mOptionRoomNo = (s8)roomNo;
    mOptionPoint = -1;
    mOption = (s8)option;
}

/* 025B9864 */
void d_save_cpp::dSv_restart_c::setRestartOption(u32 option) {

    if ((u32)l_offsetPosGuard == 0) {
        l_offsetPosGuard = 1;
        l_offsetPos.z = 0.0f;
        l_offsetPos.x = 100.0f;
        l_offsetPos.y = 0.0f;
    }
    gabi::Local<cXyz> pos;
    u32 p = gabi::ea(pos.get());
    pos.get()->x = (f32)mRestartPos.x;
    pos.get()->y = (f32)mRestartPos.y;
    pos.get()->z = (f32)mRestartPos.z;
    saveOffsetPosition(p, p, (s32)(s16)mRestartAngle, gabi::ea(&l_offsetPos));
    s32 room = (s8)mRestartRoom;
    s32 angle = (s16)mRestartAngle;
    setRestartOption(option, pos.get(), (u32)angle, (u32)room);
}

// HD: reserves 80 bytes in each saved record.
/* 025B9918 */
void d_save_cpp::dSv_reserve_c::init() {
    for (u32 i = 0; i < 0x50; i++) indexed(mReserve, i) = 0;
}

/* 025B9938 */
void d_save_cpp::dSv_save_c::init() {
    view<dSv_player_c>(gabi::ea(&mPlayer))->init();
    for (u32 i = 0; i < 0x10; i++) view<dSv_memory_c>(gabi::ea(&indexed(mMemory, i)))->init();
    view<dSv_ocean_c>(gabi::ea(&mOcean))->init();
    view<dSv_event_c>(gabi::ea(&mEvent))->init();
}

/* 025B998C */
void d_save_cpp::dSv_turnRestart_c::set(const cXyz* pos, u32 angle, u32 roomNo, u32 param, const cXyz* shipPos, u32 shipAngle,
                                  u32 hasShip) {
    wordPosition(&mPosition).x = (u32)wordPosition(pos).x;
    wordPosition(&mPosition).y = (u32)wordPosition(pos).y;
    u32 z = (u32)wordPosition(pos).z;
    mParam = param;
    wordPosition(&mPosition).z = z;
    mRoomNo = (s8)roomNo;
    mAngleY = (s16)angle;
    field_0x13 = 0;
    wordPosition(&mShipPos).x = (u32)wordPosition(shipPos).x;
    wordPosition(&mShipPos).y = (u32)wordPosition(shipPos).y;
    u32 sz = (u32)wordPosition(shipPos).z;
    mHasShip = (s32)hasShip;
    wordPosition(&mShipPos).z = sz;
    mShipAngleY = (s16)shipAngle;
}

}  // namespace d_save_cpp

/* WWHD glue
str 10053C1C "d_save.cpp" in 025B8C0C
str 10053C28 "0 <= i_no && i_no < 32" in 025B8C0C
str 10053C44 "d_save.cpp" in 025B8C74
str 10053C50 "0 <= i_no && i_no < 32" in 025B8C74
str 10053C84 "d_save.cpp" in 025B8CE0
str 10053C6C "0 <= i_no && i_no < 128" in 025B8CE0
str 10053CA8 "d_save.cpp" in 025B8D54
str 10053C90 "0 <= i_no && i_no < 128" in 025B8D54
str 10053CCC "d_save.cpp" in 025B8DC8
str 10053CB4 "0 <= i_no && i_no < 128" in 025B8DC8
str 10053CF0 "d_save.cpp" in 025B8E40
str 10053CD8 "0 <= i_no && i_no < 128" in 025B8E40
str 10053CFC "d_save.cpp" in 025B8EC0
str 10053D08 "0 <= i_no && i_no < ITEM_MAX" in 025B8EC0
str 10053D2C "d_save.cpp" in 025B8F34
str 10053D38 "0 <= i_no && i_no < ITEM_MAX" in 025B8F34
str 10053D5C "d_save.cpp" in 025B8FAC
str 10053D68 "0 <= i_no && i_no < 64" in 025B8FAC
str 10053D84 "d_save.cpp" in 025B9020
str 10053D90 "0 <= i_no && i_no < 64" in 025B9020
str 10053DC4 "d_save.cpp" in 025B9098
str 10053DAC "0 <= i_no && i_no < 6" in 025B9098
str 10053DEC "d_save.cpp" in 025B9100
str 10053DD4 "0 <= i_no && i_no < 6" in 025B9100
str 10053E0C "d_save.cpp" in 025B91AC
str 10053E18 "0 <= i_no && i_no < 64" in 025B91AC
str 10053E30 "d_save.cpp" in 025B9220
str 10053E3C "0 <= i_no && i_no < 64" in 025B9220
str 10053E54 "d_save.cpp" in 025B9294
str 10053E60 "0 <= i_no && i_no < 64" in 025B9294
str 10053E78 "d_save.cpp" in 025B930C
str 10053E84 "0 <= i_no && i_no < 64" in 025B930C
str 10053E9C "d_save.cpp" in 025B93BC
str 10053EA8 "0 <= i_no && i_no < SWITCH_MAX" in 025B93BC
str 10053EC8 "d_save.cpp" in 025B9430
str 10053ED4 "0 <= i_no && i_no < SWITCH_MAX" in 025B9430
str 10053EF4 "d_save.cpp" in 025B94A4
str 10053F00 "0 <= i_no && i_no < SWITCH_MAX" in 025B94A4
str 10053F20 "d_save.cpp" in 025B951C
str 10053F2C "0 <= i_no && i_no < SWITCH_MAX" in 025B951C
str 10053F4C "d_save.cpp" in 025B95A0
str 10053F58 "0 <= i_no && i_no < 16" in 025B95A0
str 10053F74 "d_save.cpp" in 025B9608
str 10053F80 "0 <= i_no && i_no < 16" in 025B9608
str 10053FBC "d_save.cpp" in 025B9690
str 10053F9C "0 <= i_id && i_id < ACTOR_MAX" in 025B9690
str 10053FEC "d_save.cpp" in 025B9700
str 10053FCC "0 <= i_id && i_id < ACTOR_MAX" in 025B9700
*/
