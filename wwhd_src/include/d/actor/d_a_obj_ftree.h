#pragma once
#include "f_op/f_op_actor.h"
namespace daObjFtree {
struct Act_c : fopAc_ac_c {
    be<s16> mediumTiltX, mediumTiltY, largeTiltY, largeTiltZ;
    u8 phase[8];
    be<s16> effectPending;
    be<u8> smallVisible, largeVisible;
    be<f32> baseMatrix[12]; // 0x3C0
    be<u32> morphModel, largeModel; // 0x3F0, 0x3F4
    u8 brkAnimation[0x78]; // 0x3F8
    u8 groundCheck[0x54]; // 0x470
    be<f32> groundHeight; // 0x4C4
    be<s16> defaultEvent, waterEvent, lastTreeEvent, currentEvent, eventState; // 0x4C8
    u8 _4D2[2];
    u8 bodyStatus[0x3C]; // 0x4D4
    u8 bodyCylinder[0x130]; // 0x510
    be<f32> collisionRadius; // 0x640
    u8 hitStatus[0x3C]; // 0x644
    u8 hitCylinder[0x130]; // 0x680
    be<u32> brought; // 0x7B0
    be<f32> largeScale, colorBlend; // 0x7B4, 0x7B8
    be<u32> treeId; // 0x7BC
    be<s16> heartPlaced, heartLaunchTimer, eventTimer; // 0x7C0
    be<s16> originalSmallColor[4], cachedSmallColor[4], originalLargeColor[4];
    be<s16> smallColor[4], largeColor[4]; // 0x7DE, 0x7E6
    u8 _7EE[2];
    be<u32> previousCollision, mode; // 0x7F0, 0x7F4
    be<s16> colorDelay;
    u8 _7FA[2];
    be<s32> repetitions, completedRepetitions; // 0x7FC, 0x800
    be<s16> mediumDuration, mediumElapsed, mediumPhase, mediumTargetX, mediumTargetY;
    be<s16> largeDuration, largeElapsed, largePhase, largeTargetY, largeTargetZ;
    be<u32> messageId; // 0x818
    u8 _81C[4];
};
struct SearchInfo { be<u32> total, brought; };
}
WWHD_SIZE(daObjFtree::Act_c,0x820);
static_assert(offsetof(daObjFtree::Act_c,phase)==0x3B4);
static_assert(offsetof(daObjFtree::Act_c,effectPending)==0x3BC);
static_assert(offsetof(daObjFtree::Act_c,baseMatrix)==0x3C0);
static_assert(offsetof(daObjFtree::Act_c,morphModel)==0x3F0);
static_assert(offsetof(daObjFtree::Act_c,bodyCylinder)==0x510);
static_assert(offsetof(daObjFtree::Act_c,largeColor)==0x7E6);
static_assert(offsetof(daObjFtree::Act_c,mode)==0x7F4);
static_assert(offsetof(daObjFtree::Act_c,messageId)==0x818);
