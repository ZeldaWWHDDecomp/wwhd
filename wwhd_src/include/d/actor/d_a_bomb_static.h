/* Bomb static utility TU. Game-derived, */
#pragma once
#include "bindings.h"
namespace BombStatic {
// Separate address-specific views; do not redefine merged daBomb_c/daBomb2::Act_c.
u32 getVersion(const void* self);
void checkVersion(const void* self);
s16 getRestTime(const void* self);
u8 getCheckFlag(const void* self);
void setCheckFlag(void* self);
void setFire(void* self);
void setNoHit(void* self);
void disableCo(void* self);
void enableCo(void* self);
void removeEffects(void* self);
void setRestTime(void* self, s16 time);
void setNoGravityTime(void* self, s16 time);
u32 makeParam(u32 state, u32 cheap, u32 zeroAngle);
u32 getState(const void* self);
bool checkState(const void* self, u32 state);
void changeState(void* self, u32 state);
bool getInstantExplosion(const void* self);
bool getCheapEffect(const void* self);
bool getZeroAngle(const void* self);
void flowerRemoveEffects(void* self);
void flowerSetTime(void* self, s32 time);
s32 flowerGetTime(const void* self);
u8 flowerCheckEat(const void* self);
void flowerSetEat(void* self);
void flowerSetNoHit(void* self);
bool flowerCheckExplosion(const void* self);
u32 abstractParam(const void* self, u32 width, u32 shift);
void initializeHeaderStatics();
}
