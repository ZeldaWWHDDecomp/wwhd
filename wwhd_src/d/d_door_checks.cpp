/** d_door_checks.cpp — door visibility, side and interaction checks.
 * Based on tww d/d_door.cpp.
 */
#include "d/d_door_checks.h"
using door_checks::cSAngle;
using door_checks::cSGlobe;
static s8 mStayNo;
static f32 doorZero;

/* 0252AA64 */
BOOL dDoor_info_c::adjoinPlayer() {
    u32 front = getFRoomNo();
    u32 back = getBRoomNo();
    if (front == 0x3F || back == 0x3F)
        return TRUE;
    if (dDoor_getRoomControl()->checkRoomDisp(front))
        return TRUE;
    return dDoor_getRoomControl()->checkRoomDisp(back) != 0;
}

/* 0252AB08 */
u32 dDoor_info_c::getViewRoomNo() {
    cXyz delta = dDoor_getView()->mEye - current.pos;
    f32 z = delta.z;
    f32 sz = mAngleVec.z;
    f32 sx = mAngleVec.x;
    f32 product = gabi::fmuls_ppc(z, sz);
    f32 x = delta.x;
    f32 dot = gabi::fmadds(x, sx, product); // HD: fused single-precision dot product.
    if (dot < doorZero)
        return getBRoomNo();
    return getFRoomNo();
}

/* 0252AB90 */
BOOL dDoor_info_c::frontCheckOld() {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    cXyz delta = player->current.pos - current.pos;
    cSGlobe globe;
    cSGlobe_fromVector(&globe, &delta);
    cSAngle angle;
    cSAngle difference;
    cSAngle_init(&angle);
    s32 yaw = current.angle.y;
    cSAngle_sub(&globe.mInclination, &difference, yaw);
    angle.mAngle = difference.mAngle;
    return angle.Abs() >= 0x4000;
}

/* 0252AC10 */
u32 dDoor_info_c::frontCheck() {
    u32 stay = static_cast<u32>(mStayNo);
    u32 front = getFRoomNo();
    u32 back = getBRoomNo();
    if (front == back)
        return frontCheckOld();
    if (stay == front)
        return 0;
    if (stay == back)
        return 1;
    return 2;
}

/* 0252AC98 */
s32 dDoor_info_c::checkExecute() {
    u32 side = frontCheck();
    u32 flags = actor_status;
    mFrontCheck = side;
    if (flags & fopAcStts_UNK1000_e)
        return 1;
    u16 command = eventInfo.mCommand;
    if (command == dEvtCmd_INDEMO_e || command == dEvtCmd_INDOOR_e)
        return 2;
    s32 stay = mStayNo;
    s32 room = mRoomNo2;
    if (room != stay || side == 2)
        return 0;
    return adjoinPlayer() ? 2 : 0;
}

/* 0252AD50 */
void dDoor_info_c::startDemoProc() {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s32 staff = dComIfGp_evmng_getMyStaffId("SHUTTER_DOOR", nullptr, 0);
    s32 angle = current.angle.y;
    mStaffId = staff;
    shape_angle.y = angle;
    if (!player)
        JUTAssertion_showAssert("d_door.cpp", 0x169, "player");
    s32 other = player->home.angle.y;
    s32 door = home.angle.y;
    s32 delta = static_cast<s16>(other - door);
    if (delta < 0)
        delta = static_cast<s16>(-delta);
    // HD: preserve the unsigned GHS range check after the 16-bit absolute value.
    if (static_cast<u32>(delta + 0x3E7) < 0x13E7) {
        angle = shape_angle.y;
        shape_angle.y = angle + 0x7FFF;
    }
}

/* 0252AE04 */
BOOL dDoor_info_c::checkArea(f32 lateral, f32 forward, f32 distance) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    cXyz delta;
    cXyz flat;
    cXyz normalized;
    delta = player->current.pos - current.pos;
    f32 x = delta.x;
    f32 zero = doorZero;
    f32 z = delta.z;
    flat.y = zero;
    flat.x = x;
    flat.z = z;
    f32 square = PSVECSquareMag(&flat);
    if (square > distance)
        return FALSE;
    // HD: retain the hidden result vector; the call also updates delta.
    cXyz_normalize(&delta, &normalized);
    z = delta.z;
    f32 sz = mAngleVec.z;
    f32 sx = mAngleVec.x;
    f32 product = gabi::fmuls_ppc(z, sz);
    x = delta.x;
    f32 dot = gabi::fmadds(x, sx, product);
    f32 project = gabi::fmuls_ppc(square, dot);
    project = gabi::fmuls_ppc(project, dot);
    if (project > forward)
        return FALSE;
    if (gabi::fsubs_ppc(square, project) > lateral)
        return FALSE;
    u8 side = mFrontCheck;
    s32 yaw = current.angle.y;
    s32 playerYaw = player->current.angle.y;
    if (side == 1)
        yaw = static_cast<s16>(yaw + 0x7FFF);
    s32 difference = static_cast<s16>(yaw - playerYaw);
    s32 absolute = difference < 0 ? -difference : difference;
    return absolute >= 0x5000;
}

/* 0252AF98 */
s32 dDoor_info_c::drawCheck_local() {
    if (!adjoinPlayer()) {
        u16 command = eventInfo.mCommand;
        if (command != dEvtCmd_INDEMO_e && command != dEvtCmd_INDOOR_e)
            return 0;
    }
    u32 front = getFRoomNo();
    bool boundary = front == 0x3F;
    if (!boundary)
        boundary = getBRoomNo() == 0x3F;
    if (boundary) {
        u8 stay = mStayNo;
        tevStr.mRoomNo = stay;
        current.roomNo = stay;
    } else {
        u32 room = getViewRoomNo();
        tevStr.mRoomNo = room;
        current.roomNo = room;
    }
    u32 side = frontCheck();
    s32 from = -1;
    if (side != 2)
        from = mStayNo;
    s32 room = tevStr.mRoomNo;
    mRoomNo = from;
    if (!dDoor_getRoomControl()->checkRoomDisp(room))
        return 1;
    u32 view = getViewRoomNo();
    if (view != 0x3F)
        return 2;
    front = getFRoomNo();
    if (front != 0x3F)
        return 1;
    return getBRoomNo() == 0x3F ? 2 : 1;
}

/* 0252B0FC */
BOOL dDoor_info_c::drawCheck(s32 mode) {
    u32 result = drawCheck_local();
    u32 flags = actor_status;
    if (result)
        flags = (flags & ~0x3Fu) | (mode ? 0x2A : 0x29);
    else
        flags &= ~static_cast<u32>(fopAcStts_SHOWMAP_e);
    actor_status = flags;
    return result == 2;
}

/* WWHD glue (tools/verify/autoglue.py)
data mStayNo 1047E6C8
data doorZero 1004C590
str 1004C7A0
str 1004C7B0
str 1004C798
ext 0252A37C u32 dDoor_info_c::getFRoomNo()
ext 0252A388 u32 dDoor_info_c::getBRoomNo()
ext 025C4334 u32 door_checks::roomControl_view::checkRoomDisp(u32 room)
ext 02007324 void cSGlobe_fromVector(door_checks::cSGlobe* globe, cXyz* vector)
ext 020065FC void cSAngle_init(door_checks::cSAngle* angle)
ext 02006908 void cSAngle_sub(door_checks::cSAngle* angle, door_checks::cSAngle* result, s32 value)
ext 020067EC u32 door_checks::cSAngle::Abs()
ext 0201B31C void cXyz_normalize(cXyz* vector, cXyz* result)
*/
