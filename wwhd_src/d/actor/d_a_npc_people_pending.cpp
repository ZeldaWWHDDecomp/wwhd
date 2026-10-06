/* d_a_npc_people: member functions of the translation unit not decompiled yet, as plain guest
 * calls (so the decompiled parts can call them naturally). Weak: a decompiled definition in a part
 * replaces it. */
#include "d/actor/d_a_npc_people.h"

__attribute__((weak)) BOOL daNpcPeople_c::createHeap() { return gabi::call<BOOL>(0x022BE334, this); }
__attribute__((weak)) cPhs_State daNpcPeople_c::createInit() { return gabi::call<cPhs_State>(0x022BF510, this); }
__attribute__((weak)) bool daNpcPeople_c::_delete() { return gabi::call<bool>(0x022BFF00, this); }
__attribute__((weak)) bool daNpcPeople_c::_draw() { return gabi::call<bool>(0x022C36D4, this); }
__attribute__((weak)) bool daNpcPeople_c::_execute() { return gabi::call<bool>(0x022C305C, this); }
__attribute__((weak)) bool daNpcPeople_c::executeCommon() { return gabi::call<bool>(0x022C3AC4, this); }
__attribute__((weak)) void daNpcPeople_c::executeSetMode(u32 p0) { gabi::call(0x022C06F4, this, p0); }
__attribute__((weak)) s32 daNpcPeople_c::executeWaitInit() { return gabi::call<s32>(0x022C3D88, this); }
__attribute__((weak)) void daNpcPeople_c::executeWait() { gabi::call(0x022C4074, this); }
__attribute__((weak)) s32 daNpcPeople_c::executeTalkInit() { return gabi::call<s32>(0x022C43A8, this); }
__attribute__((weak)) void daNpcPeople_c::executeTalk() { gabi::call(0x022C4404, this); }
__attribute__((weak)) s32 daNpcPeople_c::executeWalkInit() { return gabi::call<s32>(0x022C4600, this); }
__attribute__((weak)) void daNpcPeople_c::executeWalk() { gabi::call(0x022C46D0, this); }
__attribute__((weak)) s32 daNpcPeople_c::executeTurnInit() { return gabi::call<s32>(0x022C48B8, this); }
__attribute__((weak)) void daNpcPeople_c::executeTurn() { gabi::call(0x022C4A04, this); }
__attribute__((weak)) s32 daNpcPeople_c::executeBikkuriInit() { return gabi::call<s32>(0x022C4B30, this); }
__attribute__((weak)) void daNpcPeople_c::executeBikkuri() { gabi::call(0x022C4B80, this); }
__attribute__((weak)) s32 daNpcPeople_c::executeFurueInit() { return gabi::call<s32>(0x022C4BF4, this); }
__attribute__((weak)) void daNpcPeople_c::executeFurue() { gabi::call(0x022C4CA8, this); }
__attribute__((weak)) s32 daNpcPeople_c::executeKyoroInit() { return gabi::call<s32>(0x022C4DDC, this); }
__attribute__((weak)) void daNpcPeople_c::executeKyoro() { gabi::call(0x022C4E98, this); }
__attribute__((weak)) s32 daNpcPeople_c::executeLetterInit() { return gabi::call<s32>(0x022C4F54, this); }
__attribute__((weak)) void daNpcPeople_c::executeLetter() { gabi::call(0x022C4FA8, this); }
__attribute__((weak)) s32 daNpcPeople_c::executeLookInit() { return gabi::call<s32>(0x022C509C, this); }
__attribute__((weak)) void daNpcPeople_c::executeLook() { gabi::call(0x022C511C, this); }
__attribute__((weak)) s32 daNpcPeople_c::executeLook2Init() { return gabi::call<s32>(0x022C5208, this); }
__attribute__((weak)) void daNpcPeople_c::executeLook2() { gabi::call(0x022C5268, this); }
__attribute__((weak)) s32 daNpcPeople_c::executeUgWalkInit() { return gabi::call<s32>(0x022C5368, this); }
__attribute__((weak)) void daNpcPeople_c::executeUgWalk() { gabi::call(0x022C5434, this); }
__attribute__((weak)) s32 daNpcPeople_c::executeUgTurnInit() { return gabi::call<s32>(0x022C5AC4, this); }
__attribute__((weak)) void daNpcPeople_c::executeUgTurn() { gabi::call(0x022C5D4C, this); }
__attribute__((weak)) s32 daNpcPeople_c::executeUgLookInit() { return gabi::call<s32>(0x022C5DFC, this); }
__attribute__((weak)) void daNpcPeople_c::executeUgLook() { gabi::call(0x022C5E5C, this); }
__attribute__((weak)) s32 daNpcPeople_c::executeUgLook2Init() { return gabi::call<s32>(0x022C5EF4, this); }
__attribute__((weak)) void daNpcPeople_c::executeUgLook2() { gabi::call(0x022C5F44, this); }
__attribute__((weak)) s32 daNpcPeople_c::executeUgSitInit() { return gabi::call<s32>(0x022C5FE8, this); }
__attribute__((weak)) void daNpcPeople_c::executeUgSit() { gabi::call(0x022C603C, this); }
__attribute__((weak)) void daNpcPeople_c::checkOrder() { gabi::call(0x022C045C, this); }
__attribute__((weak)) void daNpcPeople_c::eventOrder() { gabi::call(0x022C2A04, this); }
__attribute__((weak)) void daNpcPeople_c::eventMove() { gabi::call(0x022C2958, this); }
__attribute__((weak)) void daNpcPeople_c::privateCut() { gabi::call(0x022C25E8, this); }
__attribute__((weak)) void daNpcPeople_c::eventMesSetTpInit(int p0) { gabi::call(0x022C0EC0, this, p0); }
__attribute__((weak)) void daNpcPeople_c::eventMesSetInit(int p0) { gabi::call(0x022C0BF8, this, p0); }
__attribute__((weak)) bool daNpcPeople_c::eventMesSet() { return gabi::call<bool>(0x022C1EEC, this); }
__attribute__((weak)) bool daNpcPeople_c::eventMesSet2() { return gabi::call<bool>(0x022C2170, this); }
__attribute__((weak)) void daNpcPeople_c::eventFlagSetInit(int p0) { gabi::call(0x022C0F20, this, p0); }
__attribute__((weak)) void daNpcPeople_c::eventGetItemInit(int p0) { gabi::call(0x022C0FC8, this, p0); }
__attribute__((weak)) void daNpcPeople_c::eventTurnToPlayerInit() { gabi::call(0x022C1094, this); }
__attribute__((weak)) bool daNpcPeople_c::eventTurnToPlayer() { return gabi::call<bool>(0x022C21D8, this); }
__attribute__((weak)) void daNpcPeople_c::eventUb1TalkInit(int p0) { gabi::call(0x022C1228, this, p0); }
__attribute__((weak)) bool daNpcPeople_c::eventUb1Talk() { return gabi::call<bool>(0x022C21F0, this); }
__attribute__((weak)) void daNpcPeople_c::eventUb1TalkXyInit(int p0) { gabi::call(0x022C1384, this, p0); }
__attribute__((weak)) bool daNpcPeople_c::eventUb1TalkXy() { return gabi::call<bool>(0x022C22B8, this); }
__attribute__((weak)) bool daNpcPeople_c::eventUb2Talk() { return gabi::call<bool>(0x022C2380, this); }
__attribute__((weak)) bool daNpcPeople_c::eventUbSetAnm() { return gabi::call<bool>(0x022C2458, this); }
__attribute__((weak)) void daNpcPeople_c::eventAreaMaxInit() { gabi::call(0x022C167C, this); }
__attribute__((weak)) void daNpcPeople_c::eventCameraStopInit() { gabi::call(0x022C16A4, this); }
__attribute__((weak)) void daNpcPeople_c::eventCameraStartInit() { gabi::call(0x022C17C0, this); }
__attribute__((weak)) void daNpcPeople_c::eventCoCylRInit(int p0) { gabi::call(0x022C17FC, this, p0); }
__attribute__((weak)) bool daNpcPeople_c::eventLookPo() { return gabi::call<bool>(0x022C2518, this); }
__attribute__((weak)) void daNpcPeople_c::eventMesSetPoInit(int p0) { gabi::call(0x022C1890, this, p0); }
__attribute__((weak)) bool daNpcPeople_c::eventMesSetPo() { return gabi::call<bool>(0x022C2588, this); }
__attribute__((weak)) u16 daNpcPeople_c::talk2(int p0, fopAc_ac_c* p1) { return gabi::call<u16>(0x022C1BBC, this, p0, p1); }
__attribute__((weak)) u16 daNpcPeople_c::talk3(int p0) { return gabi::call<u16>(0x022C1FD8, this, p0); }
__attribute__((weak)) u16 daNpcPeople_c::next_msgStatus(be<u32>* p0) { return gabi::call<u16>(0x022C6108, this, p0); }
__attribute__((weak)) u32 daNpcPeople_c::getMsg() { return gabi::call<u32>(0x022C6C68, this); }
__attribute__((weak)) u32 daNpcPeople_c::getMsg3() { return gabi::call<u32>(0x022C1F20, this); }
__attribute__((weak)) void daNpcPeople_c::chkMsg() { gabi::call(0x022C192C, this); }
__attribute__((weak)) void daNpcPeople_c::setMessage(u32 p0) { gabi::call(0x022C0BF0, this, p0); }
__attribute__((weak)) void daNpcPeople_c::setMessageUb(sUbMsgDat* p0) { gabi::call(0x022C1168, this, p0); }
__attribute__((weak)) void daNpcPeople_c::setAnmFromMsgTag() { gabi::call(0x022C1B24, this); }
__attribute__((weak)) u32 daNpcPeople_c::setAnmFromMsgTagUo(int p0) { return gabi::call<u32>(0x022C8550, this, p0); }
__attribute__((weak)) u32 daNpcPeople_c::setAnmFromMsgTagUb(int p0) { return gabi::call<u32>(0x022C1148, this, p0); }
__attribute__((weak)) u32 daNpcPeople_c::setAnmFromMsgTagUw(int p0) { return gabi::call<u32>(0x022C8570, this, p0); }
__attribute__((weak)) u32 daNpcPeople_c::setAnmFromMsgTagUm(int p0) { return gabi::call<u32>(0x022C85DC, this, p0); }
__attribute__((weak)) u32 daNpcPeople_c::setAnmFromMsgTagSa(int p0) { return gabi::call<u32>(0x022C86D0, this, p0); }
__attribute__((weak)) u32 daNpcPeople_c::setAnmFromMsgTagUg(int p0) { return gabi::call<u32>(0x022C86F0, this, p0); }
__attribute__((weak)) u8 daNpcPeople_c::getPrmNpcNo() { return gabi::call<u8>(0x022BE7B0, this); }
__attribute__((weak)) u8 daNpcPeople_c::getPrmRailID() { return gabi::call<u8>(0x022BEB50, this); }
__attribute__((weak)) u8 daNpcPeople_c::getPrmArg0() { return gabi::call<u8>(0x022BE784, this); }
__attribute__((weak)) void daNpcPeople_c::setMtx() { gabi::call(0x022BF3A0, this); }
__attribute__((weak)) void daNpcPeople_c::chkAttention() { gabi::call(0x022C0084, this); }
__attribute__((weak)) void daNpcPeople_c::lookBack() { gabi::call(0x022C2E50, this); }
__attribute__((weak)) BOOL daNpcPeople_c::initTexPatternAnm(u32 p0) { return gabi::call<BOOL>(0x022BE1E8, this, p0); }
__attribute__((weak)) void daNpcPeople_c::playTexPatternAnm() { gabi::call(0x022C2CC0, this); }
__attribute__((weak)) void daNpcPeople_c::playAnm() { gabi::call(0x022C2D6C, this); }
__attribute__((weak)) void daNpcPeople_c::setAnm(u32 p0, int p1, f32 p2, f32 p3) { gabi::call(0x022BEB7C, this, p0, p1, p2, p3); }
__attribute__((weak)) bool daNpcPeople_c::setAnmTbl(sPeopleAnmDat* p0, int p1) { return gabi::call<bool>(0x022BED70, this, p0, p1); }
__attribute__((weak)) void daNpcPeople_c::setWaitAnm() { gabi::call(0x022BEEDC, this); }
__attribute__((weak)) s16 daNpcPeople_c::XyCheckCB(int p0) { return gabi::call<s16>(0x022BF05C, this, p0); }
__attribute__((weak)) s16 daNpcPeople_c::XyEventCB(int p0) { return gabi::call<s16>(0x022BF220, this, p0); }
__attribute__((weak)) s16 daNpcPeople_c::photoCB(int p0) { return gabi::call<s16>(0x022BF330, this, p0); }
__attribute__((weak)) int daNpcPeople_c::getRand(int p0) { return gabi::call<int>(0x022BFF78, this, p0); }
__attribute__((weak)) BOOL daNpcPeople_c::isPhoto(u8 p0) { return gabi::call<BOOL>(0x022BEFF4, this, p0); }
__attribute__((weak)) BOOL daNpcPeople_c::isColor() { return gabi::call<BOOL>(0x022C6910, this); }
__attribute__((weak)) void daNpcPeople_c::setCollision(dCcD_Cyl* p0, cXyz* p1, f32 p2, f32 p3) { gabi::call(0x022BF480, this, p0, p1, p2, p3); }
__attribute__((weak)) BOOL daNpcPeople_c::chkSurprise() { return gabi::call<BOOL>(0x022C39F0, this); }
__attribute__((weak)) BOOL daNpcPeople_c::chkEndEvent() { return gabi::call<BOOL>(0x022C0840, this); }
__attribute__((weak)) BOOL daNpcPeople_c::is1GetMap20() { return gabi::call<BOOL>(0x022BF304, this); }
__attribute__((weak)) BOOL daNpcPeople_c::is1DayGetMap20() { return gabi::call<BOOL>(0x022C1108, this); }
__attribute__((weak)) int daNpcPeople_c::getWindDir() { return gabi::call<int>(0x022C60D0, this); }
__attribute__((weak)) BOOL daNpcPeople_c::isUo1FdaiAll() { return gabi::call<BOOL>(0x022C693C, this); }
__attribute__((weak)) BOOL daNpcPeople_c::isUo1FdaiOne() { return gabi::call<BOOL>(0x022C6978, this); }
__attribute__((weak)) s32 daNpcPeople_c::chkDaiza() { return gabi::call<s32>(0x022C69A0, this); }
__attribute__((weak)) BOOL daNpcPeople_c::checkPig() { return gabi::call<BOOL>(0x022C3E78, this); }
__attribute__((weak)) BOOL daNpcPeople_c::isPigOk() { return gabi::call<BOOL>(0x022C6C00, this); }
__attribute__((weak)) s16 daNpcPeople_c::getPigTimer() { return gabi::call<s16>(0x022C6BDC, this); }
__attribute__((weak)) void daNpcPeople_c::resetPig() { gabi::call(0x022C1894, this); }
__attribute__((weak)) void daNpcPeople_c::initUgSearchArea() { gabi::call(0x022C077C, this); }
__attribute__((weak)) void daNpcPeople_c::getDirDistToPos(cXyz* p0, s16 p1, f32 p2) { gabi::call(0x022C5398, this, p0, p1, p2); }
__attribute__((weak)) void daNpcPeople_c::warp() { gabi::call(0x022BFFE4, this); }
