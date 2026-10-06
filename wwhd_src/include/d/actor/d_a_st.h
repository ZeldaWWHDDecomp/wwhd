#pragma once
#include "f_op/f_op_actor.h"
// HD producers: ctor02495AD0 and creation02495C70. Remaining opaque
// storage is deliberately not assigned GameCube names without an HD access
// proof.
struct st_p {
  gptr<void> model;
  be<s8> state, mode;
  u8 pad06[2];
  cXyz pos, previous, velocity;
  csXyz angle, angularSpeed;
  be<s16> timer38, timer3A, timer3C, timer3E;
  be<s8> wait;
  u8 pad41[3];
  be<f32> factor;
  u8 smoke[0x20], pad68[4];
};
WWHD_SIZE(st_p, 0x6C);
struct st_class : fopEn_enemy_c {
  request_of_phase_process_class phase;
  be<u8> behavior, sightRange, unusedParam, ambushSwitch, deathSwitch;
  be<s8> ambushState;
  u8 pad3D6[2];
  gptr<void> bodyAnimation, headAnimation;
  be<s16> state, action;
  be<s8> fightBehavior;
  u8 pad3E5;
  be<s16> soundCounter;
  be<s8> attackSide;
  u8 pad3E9[3];
  cXyz playerPosition;
  be<f32> targetSpeed, speedStep;
  be<s16> turnStep;
  u8 pad402[2];
  be<u32> frameCounter;
  be<s16> timers[5], collisionInvulnerability, lifetime;
  be<s8> hidden;
  be<u8> ownsHio;
  csXyz headAngle;
  be<s16> forwardAngle, spinAngle;
  u8 pad422[2];
  be<f32> spinBlend;
  u8 acchCir[0x40], acch[0x1C4], status[0x3C], spheres[7][0x12C],
      weaponSphere[0x12C];
  cXyz weaponPosition, weaponTip, previousWeaponTip;
  be<s8> collisionMode, attackMode, assembledParts, partCommand;
  st_p parts[26];
  u8 opaque1AE8[0x1E6C - 0x1AE8];
  u8 lineMaterial[0x148];
  u8 maceChain[0x5C];
  be<s8> maceState;
  u8 pad2011[3];
  be<u32> weaponID;
  be<s8> weaponState;
  u8 pad2019[3];
  be<u32> upperBodyID;
  be<s8> headState;
  u8 pad2021;
  be<s16> headHeading, recoverTimer, knockbackAngle;
  be<f32> knockbackSpeed;
  be<s8> shakeIntensity, otherShake;
  csXyz partAngles[26];
  be<s16> headShakeTimer;
  be<s8> bodyHealth, deathCountdown, upperBodyPresent, otherFlag;
  u8 smoke[3][0x20];
  gptr<void> emitters[20];
  be<s16> emitterTimers[20], emitterTimer;
  u8 pad21AA[2];
  be<f32> effectScale;
  be<s8> recoveryEffect;
  u8 pad21B1[3];
  cXyz spinSmokePosition;
  csXyz spinSmokeAngle;
  be<s16> spinSmokeHeading, spinSmokeTimer;
  be<s8> spinSmokeMode, spinSmokeCount, smokeMode;
  u8 pad21CD[3];
  u8 spinSmoke[0x20], ice[0x3B8];
  be<u32> shadowID;
  be<s8> emergenceDelay;
  be<u8> bodyForm;
  u8 pad25AE[2];
};
WWHD_OFFSET(st_class, phase, 0x3C8);
WWHD_OFFSET(st_class, bodyAnimation, 0x3D8);
WWHD_OFFSET(st_class, acchCir, 0x428);
WWHD_OFFSET(st_class, parts, 0xFF0);
WWHD_OFFSET(st_class, smoke, 0x20D0);
WWHD_OFFSET(st_class, shadowID, 0x25A8);
WWHD_SIZE(st_class, 0x25B0);
