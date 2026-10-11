/* King of Red Lions method bindings used when a decompiled part is absent. */
#include "d/actor/d_a_ship_pending.h"

// HD: weak methods defer to the original implementation until a decompiled part replaces them.

// HD: retain return values from seStart, decrementShipSpeed and setCrashData.
__attribute__((weak)) void daShip_c::getMaxWaterY(cXyz* pos) {
    shipPending_getMaxWaterY(this, pos);
}

__attribute__((weak)) BOOL daShip_c::procTornadoUp_init() {
    return shipPending_procTornadoUp_init(this);
}

__attribute__((weak)) void daShip_c::setTornadoActor() {
    shipPending_setTornadoActor(this);
}

__attribute__((weak)) BOOL daShip_c::procWhirlDown_init() {
    return shipPending_procWhirlDown_init(this);
}

__attribute__((weak)) void daShip_c::setWhirlActor() {
    shipPending_setWhirlActor(this);
}

__attribute__((weak)) u32 daShip_c::seStart(u32 se, cXyz* pos) {
    return shipPending_seStart(this, se, pos);
}

__attribute__((weak)) BOOL daShip_c::procTalkReady_init() {
    return shipPending_procTalkReady_init(this);
}

__attribute__((weak)) BOOL daShip_c::procTalk_init() {
    return shipPending_procTalk_init(this);
}

__attribute__((weak)) BOOL daShip_c::checkOutRange() {
    return shipPending_checkOutRange(this);
}

__attribute__((weak)) void daShip_c::firstDecrementShipSpeed(f32 speed) {
    shipPending_firstDecrementShipSpeed(this, speed);
}

__attribute__((weak)) BOOL daShip_c::procCraneUp_init() {
    return shipPending_procCraneUp_init(this);
}

__attribute__((weak)) void daShip_c::setControllAngle(s16 angle) {
    shipPending_setControllAngle(this, angle);
}

__attribute__((weak)) s16 daShip_c::getAimControllAngle(s16 ref) {
    return shipPending_getAimControllAngle(this, ref);
}

__attribute__((weak)) void daShip_c::setRoomInfo() {
    shipPending_setRoomInfo(this);
}

__attribute__((weak)) f32 daShip_c::getWaterY() {
    return shipPending_getWaterY(this);
}

__attribute__((weak)) void daShip_c::setYPos() {
    shipPending_setYPos(this);
}

__attribute__((weak)) void daShip_c::setWaveAngle(be<s16>* a, be<s16>* b) {
    shipPending_setWaveAngle(this, a, b);
}

__attribute__((weak)) void daShip_c::setHeadAnm() {
    shipPending_setHeadAnm(this);
}

__attribute__((weak)) f32 daShip_c::getAnglePartRate() {
    return shipPending_getAnglePartRate(this);
}

__attribute__((weak)) void daShip_c::incRopeCnt(int len, int min) {
    shipPending_incRopeCnt(this, len, min);
}

__attribute__((weak)) void daShip_c::setRopePos() {
    shipPending_setRopePos(this);
}

__attribute__((weak)) void daShip_c::setEffectData(f32 y, s16 angle) {
    shipPending_setEffectData(this, y, angle);
}

__attribute__((weak)) BOOL daShip_c::execute() {
    return shipPending_execute(this);
}

__attribute__((weak)) BOOL daShip_c::shipDelete() {
    return shipPending_shipDelete(this);
}

__attribute__((weak)) BOOL daShip_c::createHeap() {
    return shipPending_createHeap(this);
}

__attribute__((weak)) void daShip_c::setPartOffAnime() {
    shipPending_setPartOffAnime(this);
}

__attribute__((weak)) void daShip_c::setPartOnAnime(u8 part) {
    shipPending_setPartOnAnime(this, part);
}

__attribute__((weak)) void daShip_c::setPartAnimeInit(u8 part) {
    shipPending_setPartAnimeInit(this, part);
}

__attribute__((weak)) BOOL daShip_c::procSteerMove_init() {
    return shipPending_procSteerMove_init(this);
}

__attribute__((weak)) BOOL daShip_c::procPaddleMove_init() {
    return shipPending_procPaddleMove_init(this);
}

__attribute__((weak)) BOOL daShip_c::procStartModeWarp_init() {
    return shipPending_procStartModeWarp_init(this);
}

__attribute__((weak)) BOOL daShip_c::procStartModeThrow_init() {
    return shipPending_procStartModeThrow_init(this);
}

__attribute__((weak)) BOOL daShip_c::procWait_init() {
    return shipPending_procWait_init(this);
}

__attribute__((weak)) cPhs_State daShip_c::create() {
    return shipPending_create(this);
}

__attribute__((weak)) void daShip_c::setSailAngle() {
    shipPending_setSailAngle(this);
}

__attribute__((weak)) void daShip_c::setMoveAngle(s16 angle) {
    shipPending_setMoveAngle(this, angle);
}

__attribute__((weak)) f32 daShip_c::decrementShipSpeed(f32 speed) {
    return shipPending_decrementShipSpeed(this, speed);
}

__attribute__((weak)) BOOL daShip_c::procCannonReady_init() {
    return shipPending_procCannonReady_init(this);
}

__attribute__((weak)) BOOL daShip_c::procCraneReady_init() {
    return shipPending_procCraneReady_init(this);
}

__attribute__((weak)) void daShip_c::changeDemoEndProc() {
    shipPending_changeDemoEndProc(this);
}

__attribute__((weak)) BOOL daShip_c::setCrashData(s16 angle) {
    return shipPending_setCrashData(this, angle);
}

__attribute__((weak)) BOOL daShip_c::procGetOff_init() {
    return shipPending_procGetOff_init(this);
}

__attribute__((weak)) BOOL daShip_c::procTactWarp_init() {
    return shipPending_procTactWarp_init(this);
}

__attribute__((weak)) BOOL daShip_c::checkNextMode(int mode) {
    return shipPending_checkNextMode(this, mode);
}

__attribute__((weak)) void daShip_c::setSelfMove(int p) {
    shipPending_setSelfMove(this, p);
}

__attribute__((weak)) BOOL daShip_c::procWait() {
    return shipPending_procWait(this);
}

__attribute__((weak)) BOOL daShip_c::procReady() {
    return shipPending_procReady(this);
}

__attribute__((weak)) BOOL daShip_c::procSteerMove() {
    return shipPending_procSteerMove(this);
}

__attribute__((weak)) BOOL daShip_c::procPaddleMove() {
    return shipPending_procPaddleMove(this);
}

__attribute__((weak)) BOOL daShip_c::procCannon_init() {
    return shipPending_procCannon_init(this);
}

__attribute__((weak)) BOOL daShip_c::procCannonReady() {
    return shipPending_procCannonReady(this);
}

__attribute__((weak)) BOOL daShip_c::procCannon() {
    return shipPending_procCannon(this);
}

__attribute__((weak)) BOOL daShip_c::procCrane_init() {
    return shipPending_procCrane_init(this);
}

__attribute__((weak)) BOOL daShip_c::procCraneReady() {
    return shipPending_procCraneReady(this);
}

__attribute__((weak)) BOOL daShip_c::procCrane() {
    return shipPending_procCrane(this);
}

__attribute__((weak)) BOOL daShip_c::procCraneUp() {
    return shipPending_procCraneUp(this);
}

__attribute__((weak)) BOOL daShip_c::procGetOff() {
    return shipPending_procGetOff(this);
}

__attribute__((weak)) BOOL daShip_c::procToolDemo() {
    return shipPending_procToolDemo(this);
}

__attribute__((weak)) BOOL daShip_c::procZevDemo() {
    return shipPending_procZevDemo(this);
}

__attribute__((weak)) BOOL daShip_c::procTalkReady() {
    return shipPending_procTalkReady(this);
}

__attribute__((weak)) BOOL daShip_c::procTurn_init() {
    return shipPending_procTurn_init(this);
}

__attribute__((weak)) BOOL daShip_c::procTalk() {
    return shipPending_procTalk(this);
}

__attribute__((weak)) BOOL daShip_c::procTurn() {
    return shipPending_procTurn(this);
}

__attribute__((weak)) BOOL daShip_c::procTornadoUp() {
    return shipPending_procTornadoUp(this);
}

__attribute__((weak)) BOOL daShip_c::procStartModeWarp() {
    return shipPending_procStartModeWarp(this);
}

__attribute__((weak)) BOOL daShip_c::procTactWarp() {
    return shipPending_procTactWarp(this);
}

__attribute__((weak)) BOOL daShip_c::procWhirlDown() {
    return shipPending_procWhirlDown(this);
}

__attribute__((weak)) BOOL daShip_c::procStartModeThrow() {
    return shipPending_procStartModeThrow(this);
}
/* WWHD glue (tools/verify/autoglue.py)
ext 0247204C void shipPending_getMaxWaterY(daShip_c* ship, cXyz* pos)
ext 02472AB0 BOOL shipPending_procTornadoUp_init(daShip_c* ship)
ext 02472C04 void shipPending_setTornadoActor(daShip_c* ship)
ext 02472E14 BOOL shipPending_procWhirlDown_init(daShip_c* ship)
ext 02472F14 void shipPending_setWhirlActor(daShip_c* ship)
ext 02473080 u32 shipPending_seStart(daShip_c* ship, u32 se, cXyz* pos)
ext 0247309C BOOL shipPending_procTalkReady_init(daShip_c* ship)
ext 024749DC BOOL shipPending_procTalk_init(daShip_c* ship)
ext 02474A98 BOOL shipPending_checkOutRange(daShip_c* ship)
ext 02474F4C void shipPending_firstDecrementShipSpeed(daShip_c* ship, f32 speed)
ext 02475030 BOOL shipPending_procCraneUp_init(daShip_c* ship)
ext 024751E0 void shipPending_setControllAngle(daShip_c* ship, s16 angle)
ext 024752C4 s16 shipPending_getAimControllAngle(daShip_c* ship, s16 ref)
ext 024752F4 void shipPending_setRoomInfo(daShip_c* ship)
ext 024753BC f32 shipPending_getWaterY(daShip_c* ship)
ext 024754D8 void shipPending_setYPos(daShip_c* ship)
ext 024759CC void shipPending_setWaveAngle(daShip_c* ship, be<s16>* a, be<s16>* b)
ext 02475DA8 void shipPending_setHeadAnm(daShip_c* ship)
ext 02476188 f32 shipPending_getAnglePartRate(daShip_c* ship)
ext 02476204 void shipPending_incRopeCnt(daShip_c* ship, int len, int min)
ext 024763B0 void shipPending_setRopePos(daShip_c* ship)
ext 02477278 void shipPending_setEffectData(daShip_c* ship, f32 y, s16 angle)
ext 02477A24 BOOL shipPending_execute(daShip_c* ship)
ext 0247AE84 BOOL shipPending_shipDelete(daShip_c* ship)
ext 0247B508 BOOL shipPending_createHeap(daShip_c* ship)
ext 0247BA38 void shipPending_setPartOffAnime(daShip_c* ship)
ext 0247BB24 void shipPending_setPartOnAnime(daShip_c* ship, u8 part)
ext 0247BC44 void shipPending_setPartAnimeInit(daShip_c* ship, u8 part)
ext 0247BE9C BOOL shipPending_procSteerMove_init(daShip_c* ship)
ext 0247BEFC BOOL shipPending_procPaddleMove_init(daShip_c* ship)
ext 0247C024 BOOL shipPending_procStartModeWarp_init(daShip_c* ship)
ext 0247C178 BOOL shipPending_procStartModeThrow_init(daShip_c* ship)
ext 0247C300 BOOL shipPending_procWait_init(daShip_c* ship)
ext 0247C358 cPhs_State shipPending_create(daShip_c* ship)
ext 0247D398 void shipPending_setSailAngle(daShip_c* ship)
ext 0247D4B0 void shipPending_setMoveAngle(daShip_c* ship, s16 angle)
ext 0247D65C f32 shipPending_decrementShipSpeed(daShip_c* ship, f32 speed)
ext 0247D684 BOOL shipPending_procCannonReady_init(daShip_c* ship)
ext 0247D6EC BOOL shipPending_procCraneReady_init(daShip_c* ship)
ext 0247D760 void shipPending_changeDemoEndProc(daShip_c* ship)
ext 0247D880 BOOL shipPending_setCrashData(daShip_c* ship, s16 angle)
ext 0247DB84 BOOL shipPending_procGetOff_init(daShip_c* ship)
ext 0247DBF8 BOOL shipPending_procTactWarp_init(daShip_c* ship)
ext 0247DD90 BOOL shipPending_checkNextMode(daShip_c* ship, int mode)
ext 0247E2B4 void shipPending_setSelfMove(daShip_c* ship, int p)
ext 0247E670 BOOL shipPending_procWait(daShip_c* ship)
ext 0247E77C BOOL shipPending_procReady(daShip_c* ship)
ext 0247E854 BOOL shipPending_procSteerMove(daShip_c* ship)
ext 0247EEE8 BOOL shipPending_procPaddleMove(daShip_c* ship)
ext 0247EFE0 BOOL shipPending_procCannon_init(daShip_c* ship)
ext 0247F024 BOOL shipPending_procCannonReady(daShip_c* ship)
ext 0247F0D0 BOOL shipPending_procCannon(daShip_c* ship)
ext 0247F57C BOOL shipPending_procCrane_init(daShip_c* ship)
ext 0247F618 BOOL shipPending_procCraneReady(daShip_c* ship)
ext 0247F7CC BOOL shipPending_procCrane(daShip_c* ship)
ext 0247FBC4 BOOL shipPending_procCraneUp(daShip_c* ship)
ext 0247FE54 BOOL shipPending_procGetOff(daShip_c* ship)
ext 0247FF54 BOOL shipPending_procToolDemo(daShip_c* ship)
ext 024800A8 BOOL shipPending_procZevDemo(daShip_c* ship)
ext 02480FD4 BOOL shipPending_procTalkReady(daShip_c* ship)
ext 024814F8 BOOL shipPending_procTurn_init(daShip_c* ship)
ext 02481630 BOOL shipPending_procTalk(daShip_c* ship)
ext 02481874 BOOL shipPending_procTurn(daShip_c* ship)
ext 02481C94 BOOL shipPending_procTornadoUp(daShip_c* ship)
ext 02481FAC BOOL shipPending_procStartModeWarp(daShip_c* ship)
ext 02482438 BOOL shipPending_procTactWarp(daShip_c* ship)
ext 024827D4 BOOL shipPending_procWhirlDown(daShip_c* ship)
ext 02482970 BOOL shipPending_procStartModeThrow(daShip_c* ship)
*/
