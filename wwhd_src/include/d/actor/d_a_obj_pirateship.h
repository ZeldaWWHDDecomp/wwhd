#pragma once
#include "f_op/f_op_actor.h"
// HD MoveBG base is 0x3DC; the ship adds a second model and texture animation.
struct daObjPirateship_c : fopAc_ac_c {
  u8 moveBg[0x30];
  u8 reserved3DC[6];
  be<s16> tilt;
  be<u8> visible, piratesCreated, sailSound;
  u8 reserved3E7;
  be<u32> shipModel;
  request_of_phase_process_class phase;
  u8 reserved3F4[8];
  be<u32> seaModel;
  u8 btk[0x74];
  be<u32> sailActor, sailID, flagActor, flagID;
  u8 reserved484[4];
  be<u32> catapultID, rudderActor, rudderID, doorActor, doorID;
  cXyz cruiseSoundPos, sailSoundPos;
  u8 reserved4B4[10];
  be<s16> bobAngle, bobStep;
  u8 reserved4C2[0x172];
  be<s8> pathPoint;
  u8 reserved635[3];
  be<u32> path;
  u8 wake[0x4C];
  be<u32> tailEmitter, splashEmitter;
  u8 seaColor[4];
  u8 acch[0x1C4];
  cXyz wakePos;
};
WWHD_OFFSET(daObjPirateship_c, shipModel, 0x3E8);
WWHD_OFFSET(daObjPirateship_c, doorActor, 0x494);
WWHD_OFFSET(daObjPirateship_c, tailEmitter, 0x688);
WWHD_OFFSET(daObjPirateship_c, wakePos, 0x858);
WWHD_SIZE(daObjPirateship_c, 0x864);
