/** d_path.cpp — stage and room paths.
 * Based on tww d/d_path.cpp.
 */
#include "d/d_path.h"
using Path = path_layout::dPath;
using path_layout::dPnt;
using path_layout::dStage_dPath_c;

// HD: preserve this translation unit's original assertion strings and zero constant.
static f32 pathZero;

/* 025AAEB8 */
dPnt* dPath_GetPnt(Path* path, s32 index) {
    if (!path)
        JUTAssertion_showAssert("d_path.cpp", 30, "path != (0)");
    bool valid = index >= 0 && index < path->m_num;
    if (!valid) {
        JUTAssertion_showAssert("d_path.cpp", 31, "0 <= pnt_index && pnt_index < path->m_num");
        if (!path)
            return nullptr;
    }
    dPnt* points = path->m_points;
    if (!points || index < 0 || index >= path->m_num)
        return nullptr;
    return &points[index];
}

/* 025AAF88 */
Path* dPath_GetRoomPathGuest(s32 index, s32 room) {
    // HD: Guest suffix avoids the existing opaque-return binding of this name.
    dStage_dPath_c* info;
    if (room == -1) {
        info = dPath_getStage()->getPath2Inf();
    } else {
        if (static_cast<u32>(room) >= 64)
            JUTAssertion_showAssert("d_path.cpp", 61, "0 <= room_no && room_no < 64");
        auto* roomData = dPath_getRoomControl()->getStatusRoomDt(room);
        if (!roomData)
            return nullptr;
        info = roomData->getPath2Inf();
    }
    if (!info || index < 0 || index >= info->num)
        return nullptr;
    return &info->m_path[index];
}

/* 025AB070 */
Path* dPath_GetNextRoomPath(Path* path, s32 room) {
    // HD: get the play object once, before the room branch.
    auto* game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    dStage_dPath_c* info;
    u32 next;
    if (room == -1) {
        info = reinterpret_cast<path_layout::dStage_dt_c*>(game->play.mStageData)->getPath2Inf();
        next = path->m_nextID;
    } else {
        auto* control = reinterpret_cast<path_layout::roomControl_view*>(&game->play.mRoomCtrl);
        auto* roomData = control->getStatusRoomDt(room);
        if (!roomData)
            return nullptr;
        info = roomData->getPath2Inf();
        next = path->m_nextID;
    }
    if (!info || next == 0xFFFF)
        return nullptr;
    if (static_cast<s32>(next) >= info->num) {
        JUTAssertion_showAssert("d_path.cpp", 114, "0 <= next_id && next_id < pd->num");
        if (static_cast<s32>(next) >= info->num)
            return nullptr;
    }
    return &info->m_path[next];
}

/* 025AB184 */
s32 dPath_GetPolyRoomPathVec(cBgS_PolyInfo* poly, cXyz* out, be<s32>* arg) {
    // HD: pointer ABI for polyInfo; retain the original 32-bit result.
    s32 room = dPath_getBgsp()->GetRoomId(poly);
    s32 index = dPath_getBgsp()->GetRoomPathId(poly);
    f32 zero = pathZero;
    out->y = zero;
    out->z = zero;
    out->x = zero;
    *arg = 0;
    if (room == -1)
        return 0;
    Path* path = dPath_GetRoomPathGuest(index, room);
    if (!path)
        return 0;
    u32 sw = path->field4_0x6;
    if (sw != 0xFF && dComIfGs_isSwitch(sw, room))
        return 0;
    s32 point = dPath_getBgsp()->GetRoomPathPntNo(poly);
    if (point == 0xFF || point < 0)
        return 0;
    u32 count = path->m_num;
    if (static_cast<u32>(point) >= count)
        return 0;
    dPnt* points = path->m_points;
    dPnt* current = &points[point];
    dPnt* next = static_cast<u32>(point) == count - 1 ? points : current + 1;
    f32 a = current->m_position.x;
    f32 b = next->m_position.x;
    out->x = b - a;
    b = next->m_position.y;
    a = current->m_position.y;
    out->y = b - a;
    b = next->m_position.z;
    a = current->m_position.z;
    out->z = b - a;
    *arg = path->mArg0;
    return 1;
}

/* 025AB2D0 */
static void __sinit_d_path_cpp() {
    sinit_header_statics(0x1047B30C, 0x101EA560);
}

/* WWHD glue (tools/verify/autoglue.py)
str 10052498 in 025AAEB8
str 10052460 in 025AAEB8
str 1005246C in 025AAEB8
str 100524A4 in 025AAF88
str 100524B0 in 025AAF88
str 100524F8 in 025AB070
str 100524D4 in 025AB070
data pathZero 10052504
ext 025C11DC path_layout::dStage_dt_c* path_layout::roomControl_view::getStatusRoomDt(s32 room)
ext 024EF130 s32 path_layout::dBgS_view::GetRoomId(cBgS_PolyInfo* poly)
ext 024EF360 s32 path_layout::dBgS_view::GetRoomPathId(cBgS_PolyInfo* poly)
ext 024EF37C s32 path_layout::dBgS_view::GetRoomPathPntNo(cBgS_PolyInfo* poly)
*/
