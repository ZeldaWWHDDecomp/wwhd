#pragma once
#include "f_op/f_op_actor.h"

namespace daComing2 {
struct BarrelInfo {
  be<u32> processId;
  be<s16> timer;
  be<u8> itemGiven;
  u8 padding;
  cXyz position;
};
struct StartBuoy {
  be<u32> processId;
  cXyz position;
};
struct Act_c : fopAc_ac_c {
  be<s32> gamesStarted;
  be<s8> startingRoom, previousRoom;
  be<s16> spawnRequested, side;
  u8 pad3B6[2];
  be<f32> shipSpeed;
  be<s16> nextBarrel, waitTimer, spawnTimer, reserved3C2;
  BarrelInfo barrels[20]; // Capacity 20; the actual active pool has 15 entries.
  be<s16> startingAngle;
  u8 pad556[2];
  cXyz startingPosition;
  StartBuoy buoys[2];
  be<s16> crossingTimer, process;
  u8 pad588[6];
  be<u8> reserved58E;
  u8 pad58F[0x31];
  cXyz spawnPosition;
};
WWHD_SIZE(BarrelInfo, 0x14);
WWHD_SIZE(StartBuoy, 0x10);
WWHD_SIZE(Act_c, 0x5CC); // HD profile 101B3CA0+10.
WWHD_OFFSET(Act_c, gamesStarted, 0x3AC);
WWHD_OFFSET(Act_c, barrels, 0x3C4);
WWHD_OFFSET(Act_c, startingAngle, 0x554);
WWHD_OFFSET(Act_c, buoys, 0x564);
WWHD_OFFSET(Act_c, crossingTimer, 0x584);
WWHD_OFFSET(Act_c, spawnPosition, 0x5C0);
}
