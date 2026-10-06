/* d_a_ship: the translation unit's methods as plain guest calls, weak, so each function can call the
 * others naturally; a decompiled definition replaces the stub. */
#include "d/actor/d_a_ship.h"

__attribute__((weak)) void daShip_c::getMaxWaterY(cXyz* pos) { gabi::call(0x0247204C, this, pos); }
__attribute__((weak)) BOOL daShip_c::procTornadoUp_init() { return gabi::call<BOOL>(0x02472AB0, this); }
__attribute__((weak)) void daShip_c::setTornadoActor() { gabi::call(0x02472C04, this); }
__attribute__((weak)) BOOL daShip_c::procWhirlDown_init() { return gabi::call<BOOL>(0x02472E14, this); }
__attribute__((weak)) void daShip_c::setWhirlActor() { gabi::call(0x02472F14, this); }
__attribute__((weak)) u32 daShip_c::seStart(u32 se, cXyz* pos) { return gabi::call<u32>(0x02473080, this, se, pos); }
__attribute__((weak)) BOOL daShip_c::procTalkReady_init() { return gabi::call<BOOL>(0x0247309C, this); }
__attribute__((weak)) BOOL daShip_c::procTalk_init() { return gabi::call<BOOL>(0x024749DC, this); }
__attribute__((weak)) BOOL daShip_c::checkOutRange() { return gabi::call<BOOL>(0x02474A98, this); }
__attribute__((weak)) void daShip_c::firstDecrementShipSpeed(f32 speed) { gabi::call(0x02474F4C, this, speed); }
__attribute__((weak)) BOOL daShip_c::procCraneUp_init() { return gabi::call<BOOL>(0x02475030, this); }
__attribute__((weak)) void daShip_c::setControllAngle(s16 angle) { gabi::call(0x024751E0, this, angle); }
__attribute__((weak)) s16 daShip_c::getAimControllAngle(s16 ref) { return gabi::call<s16>(0x024752C4, this, ref); }
__attribute__((weak)) void daShip_c::setRoomInfo() { gabi::call(0x024752F4, this); }
__attribute__((weak)) f32 daShip_c::getWaterY() { return gabi::call<f32>(0x024753BC, this); }
__attribute__((weak)) void daShip_c::setYPos() { gabi::call(0x024754D8, this); }
__attribute__((weak)) void daShip_c::setWaveAngle(be<s16>* a, be<s16>* b) { gabi::call(0x024759CC, this, a, b); }
__attribute__((weak)) void daShip_c::setHeadAnm() { gabi::call(0x02475DA8, this); }
__attribute__((weak)) f32 daShip_c::getAnglePartRate() { return gabi::call<f32>(0x02476188, this); }
__attribute__((weak)) void daShip_c::incRopeCnt(int len, int min) { gabi::call(0x02476204, this, len, min); }
__attribute__((weak)) void daShip_c::setRopePos() { gabi::call(0x024763B0, this); }
__attribute__((weak)) void daShip_c::setEffectData(f32 y, s16 angle) { gabi::call(0x02477278, this, y, angle); }
__attribute__((weak)) BOOL daShip_c::execute() { return gabi::call<BOOL>(0x02477A24, this); }
__attribute__((weak)) BOOL daShip_c::shipDelete() { return gabi::call<BOOL>(0x0247AE84, this); }
__attribute__((weak)) BOOL daShip_c::createHeap() { return gabi::call<BOOL>(0x0247B508, this); }
__attribute__((weak)) void daShip_c::setPartOffAnime() { gabi::call(0x0247BA38, this); }
__attribute__((weak)) void daShip_c::setPartOnAnime(u8 part) { gabi::call(0x0247BB24, this, part); }
__attribute__((weak)) void daShip_c::setPartAnimeInit(u8 part) { gabi::call(0x0247BC44, this, part); }
__attribute__((weak)) BOOL daShip_c::procSteerMove_init() { return gabi::call<BOOL>(0x0247BE9C, this); }
__attribute__((weak)) BOOL daShip_c::procPaddleMove_init() { return gabi::call<BOOL>(0x0247BEFC, this); }
__attribute__((weak)) BOOL daShip_c::procStartModeWarp_init() { return gabi::call<BOOL>(0x0247C024, this); }
__attribute__((weak)) BOOL daShip_c::procStartModeThrow_init() { return gabi::call<BOOL>(0x0247C178, this); }
__attribute__((weak)) BOOL daShip_c::procWait_init() { return gabi::call<BOOL>(0x0247C300, this); }
__attribute__((weak)) cPhs_State daShip_c::create() { return gabi::call<cPhs_State>(0x0247C358, this); }
__attribute__((weak)) void daShip_c::setSailAngle() { gabi::call(0x0247D398, this); }
__attribute__((weak)) void daShip_c::setMoveAngle(s16 angle) { gabi::call(0x0247D4B0, this, angle); }
__attribute__((weak)) f32 daShip_c::decrementShipSpeed(f32 speed) { return gabi::call<f32>(0x0247D65C, this, speed); }
__attribute__((weak)) BOOL daShip_c::procCannonReady_init() { return gabi::call<BOOL>(0x0247D684, this); }
__attribute__((weak)) BOOL daShip_c::procCraneReady_init() { return gabi::call<BOOL>(0x0247D6EC, this); }
__attribute__((weak)) void daShip_c::changeDemoEndProc() { gabi::call(0x0247D760, this); }
__attribute__((weak)) BOOL daShip_c::setCrashData(s16 angle) { return gabi::call<BOOL>(0x0247D880, this, angle); }
__attribute__((weak)) BOOL daShip_c::procGetOff_init() { return gabi::call<BOOL>(0x0247DB84, this); }
__attribute__((weak)) BOOL daShip_c::procTactWarp_init() { return gabi::call<BOOL>(0x0247DBF8, this); }
__attribute__((weak)) BOOL daShip_c::checkNextMode(int mode) { return gabi::call<BOOL>(0x0247DD90, this, mode); }
__attribute__((weak)) void daShip_c::setSelfMove(int p) { gabi::call(0x0247E2B4, this, p); }
__attribute__((weak)) BOOL daShip_c::procWait() { return gabi::call<BOOL>(0x0247E670, this); }
__attribute__((weak)) BOOL daShip_c::procReady() { return gabi::call<BOOL>(0x0247E77C, this); }
__attribute__((weak)) BOOL daShip_c::procSteerMove() { return gabi::call<BOOL>(0x0247E854, this); }
__attribute__((weak)) BOOL daShip_c::procPaddleMove() { return gabi::call<BOOL>(0x0247EEE8, this); }
__attribute__((weak)) BOOL daShip_c::procCannon_init() { return gabi::call<BOOL>(0x0247EFE0, this); }
__attribute__((weak)) BOOL daShip_c::procCannonReady() { return gabi::call<BOOL>(0x0247F024, this); }
__attribute__((weak)) BOOL daShip_c::procCannon() { return gabi::call<BOOL>(0x0247F0D0, this); }
__attribute__((weak)) BOOL daShip_c::procCrane_init() { return gabi::call<BOOL>(0x0247F57C, this); }
__attribute__((weak)) BOOL daShip_c::procCraneReady() { return gabi::call<BOOL>(0x0247F618, this); }
__attribute__((weak)) BOOL daShip_c::procCrane() { return gabi::call<BOOL>(0x0247F7CC, this); }
__attribute__((weak)) BOOL daShip_c::procCraneUp() { return gabi::call<BOOL>(0x0247FBC4, this); }
__attribute__((weak)) BOOL daShip_c::procGetOff() { return gabi::call<BOOL>(0x0247FE54, this); }
__attribute__((weak)) BOOL daShip_c::procToolDemo() { return gabi::call<BOOL>(0x0247FF54, this); }
__attribute__((weak)) BOOL daShip_c::procZevDemo() { return gabi::call<BOOL>(0x024800A8, this); }
__attribute__((weak)) BOOL daShip_c::procTalkReady() { return gabi::call<BOOL>(0x02480FD4, this); }
__attribute__((weak)) BOOL daShip_c::procTurn_init() { return gabi::call<BOOL>(0x024814F8, this); }
__attribute__((weak)) BOOL daShip_c::procTalk() { return gabi::call<BOOL>(0x02481630, this); }
__attribute__((weak)) BOOL daShip_c::procTurn() { return gabi::call<BOOL>(0x02481874, this); }
__attribute__((weak)) BOOL daShip_c::procTornadoUp() { return gabi::call<BOOL>(0x02481C94, this); }
__attribute__((weak)) BOOL daShip_c::procStartModeWarp() { return gabi::call<BOOL>(0x02481FAC, this); }
__attribute__((weak)) BOOL daShip_c::procTactWarp() { return gabi::call<BOOL>(0x02482438, this); }
__attribute__((weak)) BOOL daShip_c::procWhirlDown() { return gabi::call<BOOL>(0x024827D4, this); }
__attribute__((weak)) BOOL daShip_c::procStartModeThrow() { return gabi::call<BOOL>(0x02482970, this); }
