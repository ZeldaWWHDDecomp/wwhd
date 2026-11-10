# Field names: shared types

Fields the decomp still called by offset (`mXXXX`, `field_0x…`, `unk…`; zeldaret/tww keeps many of these GameCube offset
names) get names unit by unit (field naming pass, 2026-11-10). Rules: a name comes from how the original
code uses the field (address of the use and what it shows), from names already in this decomp, or from the CC0
zeldaret/tww and zeldaret/tp decomps ("TP name": the same field in TP's `dCamera_c`, matched by position and use).
Renames only: offsets, types and code are unchanged (objcmp: object-identical). The header line keeps the old name
(`was mXXX`) with the evidence. Fields without enough evidence keep their offset name and are listed as open.

## dNpc_HeadAnm_c, dNpc_EventCut_c (d_npc.h), dSv_player_info_c (d_save.h)

| Class | WWHD offset | Old | New | Evidence |
|---|---|---|---|---|
| dNpc_HeadAnm_c | 0x00 | `field_0x00` | `mSwingAngle` | swing_vertical 0259F448 / swing_horizone 0259F5A4 ease .x/.y toward the swing target, move 0259F67C eases both to 0 without a procedure |
| dNpc_HeadAnm_c | 0x10 | `field_0x14` | `mVerticalAmplitude` | swing_vertical_init 0259F36C stores the amplitude argument; swing_vertical 0259F448 multiplies cM_ssin(phase) by it for angle.x |
| dNpc_HeadAnm_c | 0x14 | `field_0x18` | `mHorizontalAmplitude` | swing_horizone_init 0259F514 stores the amplitude argument; swing_horizone 0259F5A4 multiplies cM_ssin(phase) by it for angle.y |
| dNpc_HeadAnm_c | 0x18 | `field_0x1C` | `mSwingPhase` | both inits clear it; swing_vertical 0259F448 / swing_horizone 0259F5A4 add mSwingPhaseStep and take cM_ssin of it |
| dNpc_HeadAnm_c | 0x1A | `field_0x1E` | `mSwingPhaseStep` | swing_vertical_init 0259F36C / swing_horizone_init 0259F514 store the step argument; added to mSwingPhase each frame |
| dNpc_HeadAnm_c | 0x1C | `field_0x20` | `mSwingCount` | both inits store the count argument; the swing procedures subtract 1 per completed cycle (phase wraps to >= 0) and stop at 0 |
| dNpc_EventCut_c | 0x44 | `field_0x44` | `mLastTurnDiff` | cutTurnToPosProc 025A0B3C / cutTurnToActorProc 0259FE88 store |target yaw - actor yaw| each frame and count the timer down while it is unchanged; the Start procs (025A0984, 0259FAB0) clear it |
| dNpc_EventCut_c | 0x52 | `field_0x52` | `mTalkActive` | cutTalkMsgStart 025A1090 sets 1 after setting the message numbers; cutTalkMsgProc 025A1388 ends the cut when it is 0 and clears it when the talk returns 18 |
| dSv_player_info_c | 0x10 | `field_0x10` | `mSaveCount` | saveMgrSessionCounter 02721A68 adds 1 while it is below MaxSaveCount; called by SaveMgr::saveCard 02721C44 on each save; init 025B8630 clears it |

Open: dNpc_EventCut_c `field_0x14`, `field_0x20` (only cleared by the constructor 0259F740); dSv_player_info_c `field_0x0`,
`field_0x25`, `field_0x36`, `field_0x5a` (no use in the decomp); the other d_save.h types are not done yet.
