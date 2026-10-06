/* d_a_pw.h (WWHD): Poe layout. */
#pragma once
#include "wwhd.h"

// HD profile 101CF1A4 allocates F8C bytes; named offsets are confirmed
// by this unit’s HD loads/stores. Unreviewed embedded layouts remain opaque.
struct pw_class {
    u8 opaque_000[0x3C8];
    u8 phase[8]; // 0x3C8
    be<u32> morph; // 0x3D0
    u8 opaque_3D4[0x4];
    be<u32> texturePattern; // 0x3D8
    be<u32> registerAnimations[3]; // 0x3DC
    be<f32> lanternPosition[3]; // 0x3E8
    be<f32> lanternScale[3]; // 0x3F4
    be<f32> previousLanternPosition[3]; // 0x400
    u8 opaque_40C[0x48];
    u8 behaviorType; // 0x454
    u8 hoverAtInitialY; // 0x455
    u8 noticeRangeParameter; // 0x456
    u8 colorIndex; // 0x457
    u8 hitType; // 0x458
    u8 opaque_459[0x1];
    u8 visible; // 0x45A
    u8 opaque_45B[0x2];
    u8 lanternMotionMode; // 0x45D
    u8 lanternHitLatched; // 0x45E
    u8 possessing; // 0x45F
    u8 opaque_460[0x2];
    be<s16> lanternHeld; // 0x462
    be<u32> path; // 0x464
    u8 opaque_468[0xC];
    s8 pathPointIndex; // 0x474
    u8 pathIndex; // 0x475
    u8 opaque_476[0xC];
    be<s16> action; // 0x482
    be<s16> mode; // 0x484
    u8 opaque_486[0x2];
    be<u32> jalhallaID; // 0x488
    be<u32> lanternID; // 0x48C
    be<s32> animationIndex; // 0x490
    be<s16> timers[6]; // 0x494
    u8 opaque_4A0[0x14];
    be<s16> lanternHealth; // 0x4B4
    be<s16> alpha; // 0x4B6
    u8 opaque_4B8[0x4];
    be<f32> correctionY; // 0x4BC
    u8 opaque_4C0[0x4];
    be<f32> noticeRange; // 0x4C4
    u8 opaque_4C8[0x10];
    u8 groundCircle[0x40]; // 0x4D8
    u8 groundCollision[0x1C8]; // 0x518
    u8 opaque_6E0[0x44];
    u8 collisionStatus[0x20]; // 0x724
    u8 bodyCollider[0x130]; // 0x744
    u8 lanternCollider[0x12C]; // 0x874
    u8 opaque_9A0[0x3B8];
    u8 fireState[0x22C]; // 0xD58
    u8 invisibleModel[8]; // 0xF84
};
WWHD_SIZE(pw_class, 0xF8C);
WWHD_OFFSET(pw_class, phase, 0x3C8);
WWHD_OFFSET(pw_class, morph, 0x3D0);
WWHD_OFFSET(pw_class, texturePattern, 0x3D8);
WWHD_OFFSET(pw_class, registerAnimations, 0x3DC);
WWHD_OFFSET(pw_class, lanternPosition, 0x3E8);
WWHD_OFFSET(pw_class, lanternScale, 0x3F4);
WWHD_OFFSET(pw_class, previousLanternPosition, 0x400);
WWHD_OFFSET(pw_class, behaviorType, 0x454);
WWHD_OFFSET(pw_class, hoverAtInitialY, 0x455);
WWHD_OFFSET(pw_class, noticeRangeParameter, 0x456);
WWHD_OFFSET(pw_class, colorIndex, 0x457);
WWHD_OFFSET(pw_class, hitType, 0x458);
WWHD_OFFSET(pw_class, visible, 0x45A);
WWHD_OFFSET(pw_class, lanternMotionMode, 0x45D);
WWHD_OFFSET(pw_class, lanternHitLatched, 0x45E);
WWHD_OFFSET(pw_class, possessing, 0x45F);
WWHD_OFFSET(pw_class, lanternHeld, 0x462);
WWHD_OFFSET(pw_class, path, 0x464);
WWHD_OFFSET(pw_class, pathPointIndex, 0x474);
WWHD_OFFSET(pw_class, pathIndex, 0x475);
WWHD_OFFSET(pw_class, action, 0x482);
WWHD_OFFSET(pw_class, mode, 0x484);
WWHD_OFFSET(pw_class, jalhallaID, 0x488);
WWHD_OFFSET(pw_class, lanternID, 0x48C);
WWHD_OFFSET(pw_class, animationIndex, 0x490);
WWHD_OFFSET(pw_class, timers, 0x494);
WWHD_OFFSET(pw_class, lanternHealth, 0x4B4);
WWHD_OFFSET(pw_class, alpha, 0x4B6);
WWHD_OFFSET(pw_class, correctionY, 0x4BC);
WWHD_OFFSET(pw_class, noticeRange, 0x4C4);
WWHD_OFFSET(pw_class, groundCircle, 0x4D8);
WWHD_OFFSET(pw_class, groundCollision, 0x518);
WWHD_OFFSET(pw_class, collisionStatus, 0x724);
WWHD_OFFSET(pw_class, bodyCollider, 0x744);
WWHD_OFFSET(pw_class, lanternCollider, 0x874);
WWHD_OFFSET(pw_class, fireState, 0xD58);
WWHD_OFFSET(pw_class, invisibleModel, 0xF84);
