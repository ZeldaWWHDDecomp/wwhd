#pragma once
#include "wwhd.h"

// HD allocation size established by 024450CC; GC class size is 0x4EC.
// Offsets are intentionally explicit until integration chooses shared base layouts.
struct daPy_npc_c { u8 storage[0x608]; };
WWHD_SIZE(daPy_npc_c, 0x608);
struct PlayerNpcVector { be<u32> x, y, z; };
WWHD_SIZE(PlayerNpcVector, 12);
struct PlayerNpcJudge {
    be<s16> processName;
    u8 padding[2];
    PlayerNpcVector center;
    PlayerNpcVector relative;
    be<f32> radius;
    be<f32> nearestDistance;
    be<u32> actor;
};
WWHD_OFFSET(PlayerNpcJudge, center, 4);
WWHD_OFFSET(PlayerNpcJudge, relative, 0x10);
WWHD_OFFSET(PlayerNpcJudge, radius, 0x1C);
WWHD_OFFSET(PlayerNpcJudge, nearestDistance, 0x20);
WWHD_OFFSET(PlayerNpcJudge, actor, 0x24);
WWHD_SIZE(PlayerNpcJudge, 0x28);

void* daPy_npc_JudgeForPNameAndDistance(void*, PlayerNpcJudge*);
void* daPy_npc_SearchAreaByName(void*, s16, f32, PlayerNpcVector*);
daPy_npc_c* daPy_npc_Construct(daPy_npc_c*);
void daPy_npc_Destruct(daPy_npc_c*, s32);
s32 daPy_npc_check_initialRoom(daPy_npc_c*);
s32 daPy_npc_check_moveStop(daPy_npc_c*);
void daPy_npc_unconditionalSetRestart(daPy_npc_c*, s8);
void daPy_npc_setRestart(daPy_npc_c*, s8);
void daPy_npc_setOffsetHomePos(daPy_npc_c*);
void daPy_npc_setPointRestart(daPy_npc_c*, s16, s8);
s32 daPy_npc_checkRestart(daPy_npc_c*, s8);
s32 daPy_npc_initialRestartOption(daPy_npc_c*, s8, s32);
s32 daPy_npc_checkNowPosMove(daPy_npc_c*, const char*);
void daPy_npc_drawDamageFog(daPy_npc_c*);
s32 daPy_npc_chkMoveBlock(daPy_npc_c*, PlayerNpcVector*);
