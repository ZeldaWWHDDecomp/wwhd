#pragma once
#include "wwhd.h"
// Jabun HD actor. Constructor allocates 0xD50; embedded NPC animation/event
// objects occupy the intervening region, audited separately from this tail.
struct daNpc_Jb1_c {
    u8 base[0xD2C];
    be<f32> previousFrame;
    u8 unknownD30[4];
    be<u8> animationWrapped, animationChanged;
    u8 unknownD36[6];
    be<s32> waitResult;
    be<u8> hasAttention, talking, unknownD42, demoActive;
    be<u8> unknownD44, actionNumber, animationAttribute, animationTag;
    be<s8> animationNumber, eventOrder, animationState, unknownD4B;
    be<u8> characterIndex, characterKind;
    be<s8> actionPhase, attributePhase;
};
WWHD_SIZE(daNpc_Jb1_c, 0xD50);
WWHD_OFFSET(daNpc_Jb1_c, previousFrame, 0xD2C);
WWHD_OFFSET(daNpc_Jb1_c, actionPhase, 0xD4E);
struct JbAnimationParameters {
    be<s8> animation;
    u8 pad[3];
    be<f32> morph, speed;
    be<s32> loopMode;
};
WWHD_SIZE(JbAnimationParameters,16);
