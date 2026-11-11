# Field names: save data (d_save.h)

Offset-named fields of the d_save.h save classes (the zeldaret/tww GameCube offset names) get names in the field naming
pass (2026-11-10). A name comes from how the original code uses the field (address of the use), from names
already in this decomp, or from the CC0 zeldaret/tww and zeldaret/tp decomps (same field of the same class, e.g. TWW
accessors). Only fields with real uses are named; renames only (objcmp: object-identical). The header keeps the old
name (`was field_0x…`) with the evidence. Functions are the d_save_3.cpp members (d_save_cpp::...).

## Batch S1: map flags, priest record, ocean bits

| Class | Offset | Old | New | Evidence |
|---|---|---|---|---|
| dSv_player_map_c | 0x00 | `field_0x0` | `mMapFlags[4][4]` | four 128-bit map flag sets: [1] onGetMap 025B7EF4 / isGetMap 025B7F68, [2] onOpenMap 025B7FE0 / offOpenMap 025B8054 / isOpenMap 025B80C8, [3] onCompleteMap 025B8140 / offCompleteMap 025B81B4 / isCompleteMap 025B8228 (bit no >> 5 word, no & 31 bit); init 025B7E74 clears all four ([0] has no other use); same code TWW d_save.cpp |
| dSv_player_map_c | 0x81 | `field_0x81` | `mTriforceBits` | onTriforce 025B82A0 / isTriforce 025B8308 set and test bit no (0..7); init 025B7E74 sets 0xEA (TWW: 0); same code TWW d_save.cpp dSv_player_map_c::onTriforce / offTriforce / isTriforce |
| dSv_player_priest_c | 0x0 | `field_0x0` | `mPos` | set 025B8880 stores the position; daNpc_Cb1_c create 0221F700 passes the record to the restart setup; TWW accessor getPos |
| dSv_player_priest_c | 0xC | `field_0xc` | `mRotate` | set 025B8880 stores the angle; daNpc_Cb1_c create 0221F700 reads it for the restart option; TWW accessor getRotate |
| dSv_player_priest_c | 0xE | `field_0xe` | `mRoomNo` | set 025B8880 stores the room number; daNpc_Cb1_c create 0221F700 reads it for the restart option; TWW accessor getRoomNo |
| dSv_player_priest_c | 0xF | `field_0xf` | `mFlag` | set 025B8880 stores its first argument, init 025B8874 clears it; daNpc_Cb1_c create 0221F700 restarts at the record when it is 1; TWW accessor getFlag |
| dSv_ocean_c | 0x00 | `field_0x0` | `mOceanSvBits[50]` | one u16 of save bits per sea grid: onOceanSvBit 025B89B4 sets and isOceanSvBit 025B8A50 tests bit `bit` of [grid]; init 025B8998 clears them; same code TWW d_save.cpp |

## Open (no use beyond init, or no use at all)

- dSv_player_status_a_c field_0x6, field_0x8, field_0x15, field_0x16; dSv_player_status_b_c field_0x8;
  dSv_player_item_max_c field_0x3; dSv_player_collect_c field_0x8, field_0xc; dSv_turnRestart_c field_0x13: only
  cleared by their init functions (025B4DC0, 025B5028, 025B61E4, 025B790C, 025B998C).
- dSv_player_map_c field_0x71: only cleared by init 025B7E74. dSv_player_info_c field_0x25 / field_0x36: init 025B8630
  only copies the default name into them (no reader).
- dSv_player_return_place_c unk_0xa / unk_0xb, dSv_player_get_bag_item_c unk_0x6, dSv_player_info_c field_0x0 /
  field_0x5a, dSv_player_config_c field_0x4, dSv_player_map_c field_0x82, dSv_turnRestart_c field_0x14: no use
  found (clang AST over d_save*.cpp, member-path search over wwhd_src).
