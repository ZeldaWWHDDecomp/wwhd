/* Door transition and event helpers; verified WWHD layout and call order. */
#include "d/d_door_info.h"
#include "bindings.h"

namespace {
enum : u8 { ROOM_STATUS_DOOR = 0x08 };
using DoorByte = be<u8>;
using SavePointer = gptr<dDoor_saveInfoHD>;
static DoorByte l_doorVtable;
static DoorByte l_demoDoorFlag;
static SavePointer l_saveInfo;
struct dDoor_roomsHD {
    dDoor_roomStatusHD mStatus[64];
};
static dDoor_roomsHD l_roomStatus;
static const char* action_table[] = {
    "WAIT",          "STOP_OPEN", "STOP_CLOSE", "OPEN",       "CLOSE",    "SMOKE",
    "SMOKE_END",     "SETGOAL",   "UNLOCK",     "SETSTART",   "SETANGLE", "ADJUSTMENT",
    "OPEN_PUSH",     "OPEN_PULL", "OPEN_PUSH2", "OPEN_PULL2", "TALK",     "SETSTART_PUSH",
    "SETSTART_PULL", "END_CHECK", "DROP_BF",    "DROP_AF"};
static const char* table[] = {"DEFAULT_STOP_OPEN",
                              "DEFAULT_STOP_OPEN",
                              "DEFAULT_SHUTTER_DOOR_F",
                              "DEFAULT_SHUTTER_DOOR_F",
                              "DEFAULT_SHUTTER_DOOR_F_STOP",
                              "DEFAULT_SHUTTER_DOOR_F_STOP",
                              "DEFAULT_BS_SHUTTER_F",
                              "DEFAULT_KNOB_DOOR_F_OPEN",
                              "DEFAULT_KNOB_DOOR_B_OPEN",
                              "DEFAULT_KNOB_TALK",
                              "HKYO_DOOR",
                              "SHUTTER_DROP_CARRY"};
} // namespace

/* 0252AA14 */
u32 dDoor_info_c::getSwbit() { return u32(mParameters) & 255; }

/* 0252AA20 */
u32 dDoor_info_c::getSwbit2() { return (u32(mParameters) >> 20) & 255; }

/* 0252AA2C */
u32 dDoor_info_c::getType() { return (u32(mParameters) >> 8) & 15; }

/* 0252AA38 */
void dDoor_info_c::setType(u32 type) {
    // HD: one combined parameter-word store, unlike the two GC inline writes.
    if (type < 16)
        mParameters = (u32(mParameters) & 0xFFFFF0FF) | (type << 8);
}

/* 0252A898 */
u32 dDoor_info_c::getEventNo() { return (u32(mParameters) >> 12) & 255; }

/* 0252A37C */
u32 dDoor_info_c::getFRoomNo() { return u32(s16(home.angle.x)) & 63; }

/* 0252A388 */
u32 dDoor_info_c::getBRoomNo() { return (u32(s16(home.angle.x)) >> 6) & 63; }

/* 0252A394 */
u32 dDoor_info_c::getShipId() { return u32(s16(home.angle.z)) & 63; }

/* 0252AA58 */
u32 dDoor_info_c::getArg1() { return (u32(s16(home.angle.z)) >> 8) & 255; }

/* 0252A8A4 */
void dDoor_info_c::makeEventId(u32 spl) {
    for (u32 n = 0; n < 12; ++n) {
        u32 no = getEventNo();
        auto* game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        u32 tool = dDoor_getToolIdHD(&game->play.mEvtManager, no, n);
        mToolId[n] = tool;
        const char* name = table[n];
        game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        mEventIdx[n] = dDoor_getEventIdxHD(&game->play.mEvtManager, name, tool);
    }
    if (spl == 1 || spl == 2) {
        const char* name = spl == 1 ? "DEFAULT_SHUTTER_DOOR_10" : "DEFAULT_SHUTTER_DOOR_12";
        u32 tool = mToolId[2];
        auto* game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        mEventIdx[2] = dDoor_getEventIdxHD(&game->play.mEvtManager, name, tool);
        tool = mToolId[3];
        game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        mEventIdx[3] = dDoor_getEventIdxHD(&game->play.mEvtManager, name, tool);
    }
}

/* 0252A9E0 */
void dDoor_info_c::initProc(u32 spl) {
    f32 sn = cM_ssin(u16(home.angle.y));
    f32 cs = cM_scos(u16(home.angle.y));
    mAngleVec.y = 0.0f;
    mAngleVec.z = cs;
    mAngleVec.x = sn;
    makeEventId(spl);
}

/* 0252A2DC */
void dDoor_info_c::initOpenDemo(s32 evt) {
    if (m2C6 != 9)
        l_demoDoorFlag = 1;
    u8 side = mFrontCheck;
    s32 angle = current.angle.y;
    shape_angle.y = angle;
    if (side == 1)
        shape_angle.y = angle + 0x7FFF;
    auto* game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    mStaffId = dDoor_getMyStaffIdHD(&game->play.mEvtManager, "SHUTTER_DOOR", 0, 0);
    if (evt) {
        game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        game->play.mEvtCtrl.onEventFlag(2);
    }
}

/* 0252A3A0 */
void dDoor_info_c::openInitCom(s32 ship) {
    u32 toRoom;
    if (!mFrontCheck) {
        mFromRoomNo = getFRoomNo();
        toRoom = getBRoomNo();
    } else {
        mFromRoomNo = getBRoomNo();
        toRoom = getFRoomNo();
    }
    u32 fromRoom = mFromRoomNo;
    mToRoomNo = toRoom;
    // HD: compare the full getter result before the stored destination is narrowed to a byte.
    if (fromRoom != toRoom && fromRoom != 63 && toRoom != 63) {
        dComIfGp_get();
        l_roomStatus.mStatus[toRoom].mFlags &= u8(~ROOM_STATUS_DOOR);
        // HD also tells the environment lighting code the new door destination.
        auto* env = dDoor_getEnvlightHD();
        env->mDoorDestinationRoom = mToRoomNo;
    }
    if (ship) {
        u32 room = getShipId();
        if (room != 63)
            dDoor_setShipRoomHD(room, u8(mToRoomNo));
    }
}

/* 0252A48C */
void dDoor_info_c::openProcCom() {
    auto* game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    s32 angle = shape_angle.y;
    auto* player = game->play.mPlayerInfo[0].mpPlayer.get();
    f32 x = current.pos.x;
    f32 z = current.pos.z;
    f32 sn = cM_ssin(angle + 0x7FFF);
    f32 cs = cM_scos(angle + 0x7FFF);
    // HD: keep the fused operations and their single-precision rounding order.
    f32 goalX = gabi::fmadds(sn, -100.0f, x);
    f32 playerX = player->current.pos.x;
    f32 goalZ = gabi::fmadds(cs, -100.0f, z);
    cXyz playerPos;
    playerPos.x = playerX;
    f32 mixX = gabi::fmuls_ppc(goalX, 0.1f);
    f32 playerY = player->current.pos.y;
    f32 mixZ = gabi::fmuls_ppc(goalZ, 0.1f);
    playerPos.y = playerY;
    f32 playerZ = player->current.pos.z;
    playerPos.x = gabi::fmadds(playerX, 0.9f, mixX);
    playerPos.z = gabi::fmadds(playerZ, 0.9f, mixZ);
    u32 target = gabi::at<dDoor_playerVtableHD>(player->__vtbl)->mSetPlayerPosAndAngle;
    s32 yaw = player->current.angle.y;
    gabi::call_ptr<void>(target, player, &playerPos, yaw);
}

/* 0252A550 */
void dDoor_info_c::closeEndCom() {
    u32 back = mToRoomNo;
    u32 front = mFromRoomNo;
    if (front != back && front != 63 && back != 63) {
        dComIfGp_get();
        l_roomStatus.mStatus[front].mFlags |= ROOM_STATUS_DOOR;
    }
    auto* game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    auto* player = game->play.mPlayerInfo[0].mpPlayer.get();
    cXyz delta;
    cXyz pos;
    delta = player->current.pos - current.pos;
    f32 dz = delta.z;
    f32 sz = mAngleVec.z;
    f32 dx = delta.x;
    f32 product = gabi::fmuls_ppc(dz, sz);
    f32 sx = mAngleVec.x;
    f32 dot = gabi::fmadds(dx, sx, product);
    // HD: the >= selection also preserves the reference result for an unordered dot product.
    f32 rad = dot >= 0.0f ? -180.0f : 180.0f;
    f32 x = current.pos.x;
    f32 z = current.pos.z;
    f32 y = current.pos.y;
    pos.y = y;
    pos.x = gabi::fnmsubs(rad, sx, x);
    pos.z = gabi::fnmsubs(rad, sz, z);
    s32 room;
    s32 yaw;
    dDoor_saveInfoHD* save;
    if (dot > 0.0f) {
        room = player->current.roomNo;
        save = l_saveInfo;
        yaw = current.angle.y;
    } else {
        yaw = s16(current.angle.y + 0x8000);
        room = player->current.roomNo;
        save = l_saveInfo;
    }
    dDoor_setRestartRoomHD(&save->mSave.mRestart, &pos, yaw, room);
}

/* 0252A684 */
s32 dDoor_info_c::getDemoAction() {
    u32 staff = mStaffId;
    auto* game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    return dDoor_getMyActIdxHD(&game->play.mEvtManager, staff, action_table, 22, 0, 0);
}

/* 0252A6D0 */
void dDoor_info_c::setGoal() {
    auto* game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    s32 angle = shape_angle.y;
    f32 z = current.pos.z;
    auto* player = game->play.mPlayerInfo[0].mpPlayer.get();
    f32 playerX = player->current.pos.x;
    f32 x = current.pos.x;
    f32 sn = cM_ssin(angle + 0x7FFF);
    f32 cs = cM_scos(angle + 0x7FFF);
    cXyz pos;
    pos.x = playerX;
    f32 firstX = gabi::fmadds(sn, 350.0f, playerX);
    f32 playerY = player->current.pos.y;
    f32 firstZ = gabi::fmadds(cs, 250.0f, z);
    f32 secondX = gabi::fmadds(sn, 250.0f, x);
    pos.y = playerY;
    f32 mixZ = gabi::fmuls_ppc(firstZ, 0.2f);
    f32 playerZ = player->current.pos.z;
    f32 mixX = gabi::fmuls_ppc(secondX, 0.2f);
    f32 secondZ = gabi::fmadds(cs, 350.0f, playerZ);
    pos.x = gabi::fmadds(firstX, 0.8f, mixX);
    pos.z = gabi::fmadds(secondZ, 0.8f, mixZ);
    game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    dDoor_setGoalHD(&game->play.mEvtManager, &pos);
}

/* 0252A798 */
void dDoor_info_c::setPlayerAngle(s32 flip) {
    s32 angle = shape_angle.y;
    auto* game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    auto* link = reinterpret_cast<dDoor_playerDemoHD*>(game->play.mpPlayerPtr[0].get());
    if (flip)
        angle = s16(angle + 0x7FFF);
    link->mDemoMoveAngle = angle;
}

/* 0252A7E8 */
void dDoor_info_c::setPosAndAngle(cXyz* pPos, s32 angle) {
    u16 command = eventInfo.mCommand;
    if (command == dEvtCmd_INDEMO_e || command == dEvtCmd_INDOOR_e)
        return;
    if (pPos) {
        // HD: the three pPos components are copied as words to preserve NaN payloads.
        gabi::store<u32>(gabi::ea(&current.pos.x), gabi::load<u32>(gabi::ea(&pPos->x)));
        gabi::store<u32>(gabi::ea(&current.pos.y), gabi::load<u32>(gabi::ea(&pPos->y)));
        f32 y = current.pos.y;
        f32 x = current.pos.x;
        u32 zBits = gabi::load<u32>(gabi::ea(&pPos->z));
        y = gabi::fadds_ppc(y, 150.0f);
        attention_info.position.x = x;
        gabi::store<u32>(gabi::ea(&current.pos.z), zBits);
        f32 z = current.pos.z;
        attention_info.position.y = y;
        attention_info.position.z = z;
        eyePos.y = y;
        eyePos.z = z;
        eyePos.x = x;
    }
    current.angle.y = angle;
    shape_angle.y = angle;
    f32 sn = cM_ssin(angle);
    f32 cs = cM_scos(angle);
    mAngleVec.x = sn;
    mAngleVec.y = 0.0f;
    mAngleVec.z = cs;
}

// HD: compiler-generated code of this translation unit.
/* 0252A244 */
u32 dDoor_info_c__ct(dDoor_info_c* self) {
    // HD: retain the GHS nullable-this allocation entry and its full register result.
    if (!self) {
        self = static_cast<dDoor_info_c*>(operator_new(sizeof(dDoor_info_c)));
        if (!self)
            return 0;
    }
    fopAc_ac_c_ct(self);
    self->mToRoomNo = 0;
    self->m2A1 = 0;
    self->__vtbl = gabi::ea(&l_doorVtable);
    self->mFromRoomNo = 0;
    self->mFrontCheck = 0;
    self->mRoomNo2 = 0;
    dDoor_bzeroHD(self->mEventIdx, sizeof(self->mEventIdx));
    dDoor_bzeroHD(self->mToolId, sizeof(self->mToolId));
    self->mStaffId = 0;
    self->mRoomNo = 0;
    self->m2C6 = 0;
    return gabi::ea(self);
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
data l_doorVtable 1004C564
data l_demoDoorFlag 1047A964
data l_saveInfo 101F84DC
data l_roomStatus 1047E6CC
data action_table 101D6060
data table 101D60B8
str 1004C574 "SHUTTER_DOOR"
str 1004C69C "DEFAULT_SHUTTER_DOOR_10"
str 1004C6B4 "DEFAULT_SHUTTER_DOOR_12"
ext 02542D88 u32 dDoor_getMyStaffIdHD(dEvent_manager_c* manager, const char* name, u32 actor, u32 tag)
ext 02544708 u32 dDoor_getToolIdHD(dEvent_manager_c* manager, u32 number, u32 index)
ext 02543F10 u32 dDoor_getEventIdxHD(dEvent_manager_c* manager, const char* name, u32 tool)
ext 02543714 void dDoor_setGoalHD(dEvent_manager_c* manager, cXyz* position)
ext 025B9810 void dDoor_setRestartRoomHD(dSv_restart_c* restart, cXyz* position, s32 angle, s32 room)
ext 028F521C void dDoor_bzeroHD(void* ptr, u32 size)
ext 02555D0C dDoor_environmentHD* dDoor_getEnvlightHD()
ext 025C23E0 void dDoor_setShipRoomHD(u32 room, u32 destination)
ext 02542EDC s32 dDoor_getMyActIdxHD(dEvent_manager_c* manager, u32 staff, const gptr<char>* names, s32 count, s32 flag, s32 extra)
*/

// clang-format on
