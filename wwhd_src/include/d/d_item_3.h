/* Collect-map handlers: private API view of the shared save layout. */
#pragma once
#include "d/d_com_inf_game.h"

void itemMap_onGetMap(dSv_player_map_c* map, s32 no);
void itemMap_offOpenMap(dSv_player_map_c* map, s32 no);
void itemMap_offCompleteMap(dSv_player_map_c* map, s32 no);

// These methods have GameCube names; the shared save class has no bindings for them yet.
struct ItemCollectMapView : dSv_player_map_c {
    void onGetMap(s32 no) { itemMap_onGetMap(this, no); }
    void offOpenMap(s32 no) { itemMap_offOpenMap(this, no); }
    void offCompleteMap(s32 no) { itemMap_offCompleteMap(this, no); }
};
WWHD_SIZE(ItemCollectMapView, sizeof(dSv_player_map_c));
inline ItemCollectMapView* itemCollectMap() {
    // HD: reload the save pointer before each call; mocked callees can change it.
    return (ItemCollectMapView*)&dComIfGs_info()->mSavedata.mPlayer.mMap;
}
