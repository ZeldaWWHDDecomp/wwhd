/* Path data: tww names and unchanged binary layouts.
 */
#pragma once
#include "bindings.h"
#include "gc_api/JSystem/JUTAssert.h"

// HD: the shared dPath is still opaque, so keep this unit's complete view separate.
namespace path_layout {
struct dPnt {
    be<u8> mArg0, mArg1, mArg2, mArg3;
    cXyz m_position;
};
struct dPath {
    be<u16> m_num, m_nextID;
    be<u8> mArg0, m_closed, field4_0x6, field5_0x7;
    gptr<dPnt> m_points;
};
WWHD_SIZE(dPnt, 0x10);
WWHD_SIZE(dPath, 0xC);
WWHD_OFFSET(dPath, m_points, 8);
struct dStage_dPath_c {
    be<s32> num;
    gptr<dPath> m_path;
};
struct stage_vtable {
    u8 _000[0x1AC];
    gfn getPath2Inf;
};
WWHD_OFFSET(stage_vtable, getPath2Inf, 0x1AC);
struct dStage_dt_c {
    gptr<stage_vtable> __vtbl;
    dStage_dPath_c* getPath2Inf() {
        // HD: virtual dispatch through the original stage object's vtable.
        return gabi::call_ptr<dStage_dPath_c*>(__vtbl->getPath2Inf.get(), this);
    }
};
struct roomControl_view { dStage_dt_c* getStatusRoomDt(s32 room); };
struct dBgS_view {
    s32 GetRoomId(cBgS_PolyInfo* poly);
    s32 GetRoomPathId(cBgS_PolyInfo* poly);
    s32 GetRoomPathPntNo(cBgS_PolyInfo* poly);
};
}
inline path_layout::dStage_dt_c* dPath_getStage() {
    return reinterpret_cast<path_layout::dStage_dt_c*>(
        reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mStageData);
}
inline path_layout::roomControl_view* dPath_getRoomControl() {
    return reinterpret_cast<path_layout::roomControl_view*>(
        &reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mRoomCtrl);
}
inline path_layout::dBgS_view* dPath_getBgsp() {
    return reinterpret_cast<path_layout::dBgS_view*>(dComIfG_Bgsp());
}
path_layout::dPath* dPath_GetRoomPathGuest(s32 index, s32 room);
