/* daGoal_Flag_c (finish-line flag), WWHD layout. */
#pragma once
#include "bindings.h"

// WWHD embeds a shader packet rather than the GameCube GX display-list packet.
// The arrays begin at 0x134C. The model matrix and actorTev precede them.
struct GoalFlagPacketLayout {
    u8 pad0[0x98];
    be<s16> normalFlutterPhase;
    be<s16> wavePhase;
    be<u32> materialCount;
    be<u32> texturePairCount;
    gptr<void> texturePairs;
    u8 vertexBuffers[0x968];
    be<u32> shaderMaterialCount;
    gptr<void> shaderMaterials;
    u8 padA18[0x1318-0xA18];
    be<f32> modelMatrix[12];
    gptr<void> tev;
    cXyz positions[2][45];
    cXyz displayPositions[2][45];
    cXyz normals[2][45];
    cXyz backNormals[2][45];
    cXyz velocity[45];
    be<u8> currentArray;
    u8 pad2649[3];
};
WWHD_OFFSET(GoalFlagPacketLayout, materialCount,0x9C);
WWHD_OFFSET(GoalFlagPacketLayout, shaderMaterialCount,0xA10);
WWHD_OFFSET(GoalFlagPacketLayout, tev,0x1348);
WWHD_OFFSET(GoalFlagPacketLayout, positions,0x134C);
WWHD_OFFSET(GoalFlagPacketLayout, displayPositions,0x1784);
WWHD_OFFSET(GoalFlagPacketLayout, normals,0x1BBC);
WWHD_OFFSET(GoalFlagPacketLayout, backNormals,0x1FF4);
WWHD_OFFSET(GoalFlagPacketLayout, modelMatrix,0x1318);
WWHD_OFFSET(GoalFlagPacketLayout, velocity,0x242C);
WWHD_OFFSET(GoalFlagPacketLayout,currentArray,0x2648);
WWHD_SIZE(GoalFlagPacketLayout,0x264C);
// Velocity precedes the current-array selector (0x2648).
struct GoalFlagActorLayout : fopAc_ac_c {
    GoalFlagPacketLayout packet;
    u8 clothPhase[8];
    u8 flagPhase[8];
    be<f32> baseMatrix[12];
    cXyz poles[2];
    be<s16> windPhase;
    u8 pad2A52[2];
    be<s32> timerId;
    be<s32> starterId;
    be<f32> previousSide;
    be<s16> raceEndState;
    be<s16> cameraFrames;
    be<u8> raceStartState;
    u8 pad2A65[3];
    gptr<void> ropePaths[4];
    be<s32> buoyCounts[4];
    be<s32> ropeCount;
    u8 ropeLines[4][0x148];
    be<s16> actionAdjustment;
    be<s16> actionDispatch;
    be<u32> actionTarget;
    be<u8> timerEnded;
    u8 pad2FB5[3];
};
WWHD_OFFSET(GoalFlagActorLayout,packet,0x3AC);
WWHD_OFFSET(GoalFlagActorLayout,poles,0x2A38);
WWHD_OFFSET(GoalFlagActorLayout,previousSide,0x2A5C);
WWHD_OFFSET(GoalFlagActorLayout,ropePaths,0x2A68);
WWHD_OFFSET(GoalFlagActorLayout,buoyCounts,0x2A78);
WWHD_OFFSET(GoalFlagActorLayout,ropeCount,0x2A88);
WWHD_OFFSET(GoalFlagActorLayout,ropeLines,0x2A8C);
WWHD_OFFSET(GoalFlagActorLayout,actionTarget,0x2FB0);
WWHD_SIZE(GoalFlagActorLayout,0x2FB8);
