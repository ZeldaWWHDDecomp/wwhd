/* Windfall townspeople fallback bindings. Strong companion definitions override these methods. */
#include "d/actor/d_a_npc_people_pending.h"

// HD: retain register-width arguments and returns, including the hidden cXyz result pointer.
__attribute__((weak)) BOOL daNpcPeople_c::createHeap() {
    return peoplePending_createHeap(this);
}

__attribute__((weak)) cPhs_State daNpcPeople_c::createInit() {
    return peoplePending_createInit(this);
}

__attribute__((weak)) bool daNpcPeople_c::_delete() {
    return peoplePending__delete(this);
}

__attribute__((weak)) bool daNpcPeople_c::_draw() {
    return peoplePending__draw(this);
}

__attribute__((weak)) bool daNpcPeople_c::_execute() {
    return peoplePending__execute(this);
}

__attribute__((weak)) bool daNpcPeople_c::executeCommon() {
    return peoplePending_executeCommon(this);
}

__attribute__((weak)) void daNpcPeople_c::executeSetMode(u32 proc) {
    peoplePending_executeSetMode(this, proc);
}

__attribute__((weak)) s32 daNpcPeople_c::executeWaitInit() {
    return peoplePending_executeWaitInit(this);
}

__attribute__((weak)) void daNpcPeople_c::executeWait() {
    peoplePending_executeWait(this);
}

__attribute__((weak)) s32 daNpcPeople_c::executeTalkInit() {
    return peoplePending_executeTalkInit(this);
}

__attribute__((weak)) void daNpcPeople_c::executeTalk() {
    peoplePending_executeTalk(this);
}

__attribute__((weak)) s32 daNpcPeople_c::executeWalkInit() {
    return peoplePending_executeWalkInit(this);
}

__attribute__((weak)) void daNpcPeople_c::executeWalk() {
    peoplePending_executeWalk(this);
}

__attribute__((weak)) s32 daNpcPeople_c::executeTurnInit() {
    return peoplePending_executeTurnInit(this);
}

__attribute__((weak)) void daNpcPeople_c::executeTurn() {
    peoplePending_executeTurn(this);
}

__attribute__((weak)) s32 daNpcPeople_c::executeBikkuriInit() {
    return peoplePending_executeBikkuriInit(this);
}

__attribute__((weak)) void daNpcPeople_c::executeBikkuri() {
    peoplePending_executeBikkuri(this);
}

__attribute__((weak)) s32 daNpcPeople_c::executeFurueInit() {
    return peoplePending_executeFurueInit(this);
}

__attribute__((weak)) void daNpcPeople_c::executeFurue() {
    peoplePending_executeFurue(this);
}

__attribute__((weak)) s32 daNpcPeople_c::executeKyoroInit() {
    return peoplePending_executeKyoroInit(this);
}

__attribute__((weak)) void daNpcPeople_c::executeKyoro() {
    peoplePending_executeKyoro(this);
}

__attribute__((weak)) s32 daNpcPeople_c::executeLetterInit() {
    return peoplePending_executeLetterInit(this);
}

__attribute__((weak)) void daNpcPeople_c::executeLetter() {
    peoplePending_executeLetter(this);
}

__attribute__((weak)) s32 daNpcPeople_c::executeLookInit() {
    return peoplePending_executeLookInit(this);
}

__attribute__((weak)) void daNpcPeople_c::executeLook() {
    peoplePending_executeLook(this);
}

__attribute__((weak)) s32 daNpcPeople_c::executeLook2Init() {
    return peoplePending_executeLook2Init(this);
}

__attribute__((weak)) void daNpcPeople_c::executeLook2() {
    peoplePending_executeLook2(this);
}

__attribute__((weak)) s32 daNpcPeople_c::executeUgWalkInit() {
    return peoplePending_executeUgWalkInit(this);
}

__attribute__((weak)) void daNpcPeople_c::executeUgWalk() {
    peoplePending_executeUgWalk(this);
}

__attribute__((weak)) s32 daNpcPeople_c::executeUgTurnInit() {
    return peoplePending_executeUgTurnInit(this);
}

__attribute__((weak)) void daNpcPeople_c::executeUgTurn() {
    peoplePending_executeUgTurn(this);
}

__attribute__((weak)) s32 daNpcPeople_c::executeUgLookInit() {
    return peoplePending_executeUgLookInit(this);
}

__attribute__((weak)) void daNpcPeople_c::executeUgLook() {
    peoplePending_executeUgLook(this);
}

__attribute__((weak)) s32 daNpcPeople_c::executeUgLook2Init() {
    return peoplePending_executeUgLook2Init(this);
}

__attribute__((weak)) void daNpcPeople_c::executeUgLook2() {
    peoplePending_executeUgLook2(this);
}

__attribute__((weak)) s32 daNpcPeople_c::executeUgSitInit() {
    return peoplePending_executeUgSitInit(this);
}

__attribute__((weak)) void daNpcPeople_c::executeUgSit() {
    peoplePending_executeUgSit(this);
}

__attribute__((weak)) void daNpcPeople_c::checkOrder() {
    peoplePending_checkOrder(this);
}

__attribute__((weak)) void daNpcPeople_c::eventOrder() {
    peoplePending_eventOrder(this);
}

__attribute__((weak)) void daNpcPeople_c::eventMove() {
    peoplePending_eventMove(this);
}

__attribute__((weak)) void daNpcPeople_c::privateCut() {
    peoplePending_privateCut(this);
}

__attribute__((weak)) void daNpcPeople_c::eventMesSetTpInit(int staffIdx) {
    peoplePending_eventMesSetTpInit(this, staffIdx);
}

__attribute__((weak)) void daNpcPeople_c::eventMesSetInit(int staffIdx) {
    peoplePending_eventMesSetInit(this, staffIdx);
}

__attribute__((weak)) bool daNpcPeople_c::eventMesSet() {
    return peoplePending_eventMesSet(this);
}

__attribute__((weak)) bool daNpcPeople_c::eventMesSet2() {
    return peoplePending_eventMesSet2(this);
}

__attribute__((weak)) void daNpcPeople_c::eventFlagSetInit(int staffIdx) {
    peoplePending_eventFlagSetInit(this, staffIdx);
}

__attribute__((weak)) void daNpcPeople_c::eventGetItemInit(int staffIdx) {
    peoplePending_eventGetItemInit(this, staffIdx);
}

__attribute__((weak)) void daNpcPeople_c::eventTurnToPlayerInit() {
    peoplePending_eventTurnToPlayerInit(this);
}

__attribute__((weak)) bool daNpcPeople_c::eventTurnToPlayer() {
    return peoplePending_eventTurnToPlayer(this);
}

__attribute__((weak)) void daNpcPeople_c::eventUb1TalkInit(int staffIdx) {
    peoplePending_eventUb1TalkInit(this, staffIdx);
}

__attribute__((weak)) bool daNpcPeople_c::eventUb1Talk() {
    return peoplePending_eventUb1Talk(this);
}

__attribute__((weak)) void daNpcPeople_c::eventUb1TalkXyInit(int staffIdx) {
    peoplePending_eventUb1TalkXyInit(this, staffIdx);
}

__attribute__((weak)) bool daNpcPeople_c::eventUb1TalkXy() {
    return peoplePending_eventUb1TalkXy(this);
}

__attribute__((weak)) bool daNpcPeople_c::eventUb2Talk() {
    return peoplePending_eventUb2Talk(this);
}

__attribute__((weak)) bool daNpcPeople_c::eventUbSetAnm() {
    return peoplePending_eventUbSetAnm(this);
}

__attribute__((weak)) void daNpcPeople_c::eventAreaMaxInit() {
    peoplePending_eventAreaMaxInit(this);
}

__attribute__((weak)) void daNpcPeople_c::eventCameraStopInit() {
    peoplePending_eventCameraStopInit(this);
}

__attribute__((weak)) void daNpcPeople_c::eventCameraStartInit() {
    peoplePending_eventCameraStartInit(this);
}

__attribute__((weak)) void daNpcPeople_c::eventCoCylRInit(int staffIdx) {
    peoplePending_eventCoCylRInit(this, staffIdx);
}

__attribute__((weak)) bool daNpcPeople_c::eventLookPo() {
    return peoplePending_eventLookPo(this);
}

__attribute__((weak)) void daNpcPeople_c::eventMesSetPoInit(int staffIdx) {
    peoplePending_eventMesSetPoInit(this, staffIdx);
}

__attribute__((weak)) bool daNpcPeople_c::eventMesSetPo() {
    return peoplePending_eventMesSetPo(this);
}

__attribute__((weak)) u16 daNpcPeople_c::talk2(int initializeMessage, fopAc_ac_c* speaker) {
    return peoplePending_talk2(this, initializeMessage, speaker);
}

__attribute__((weak)) u16 daNpcPeople_c::talk3(int initializeMessage) {
    return peoplePending_talk3(this, initializeMessage);
}

__attribute__((weak)) u16 daNpcPeople_c::next_msgStatus(be<u32>* pMsgNo) {
    return peoplePending_next_msgStatus(this, pMsgNo);
}

__attribute__((weak)) u32 daNpcPeople_c::getMsg() {
    return peoplePending_getMsg(this);
}

__attribute__((weak)) u32 daNpcPeople_c::getMsg3() {
    return peoplePending_getMsg3(this);
}

__attribute__((weak)) void daNpcPeople_c::chkMsg() {
    peoplePending_chkMsg(this);
}

__attribute__((weak)) void daNpcPeople_c::setMessage(u32 msgNo) {
    peoplePending_setMessage(this, msgNo);
}

__attribute__((weak)) void daNpcPeople_c::setMessageUb(sUbMsgDat* messageData) {
    peoplePending_setMessageUb(this, messageData);
}

__attribute__((weak)) void daNpcPeople_c::setAnmFromMsgTag() {
    peoplePending_setAnmFromMsgTag(this);
}

__attribute__((weak)) u32 daNpcPeople_c::setAnmFromMsgTagUo(int messageAnimation) {
    return peoplePending_setAnmFromMsgTagUo(this, messageAnimation);
}

__attribute__((weak)) u32 daNpcPeople_c::setAnmFromMsgTagUb(int messageAnimation) {
    return peoplePending_setAnmFromMsgTagUb(this, messageAnimation);
}

__attribute__((weak)) u32 daNpcPeople_c::setAnmFromMsgTagUw(int messageAnimation) {
    return peoplePending_setAnmFromMsgTagUw(this, messageAnimation);
}

__attribute__((weak)) u32 daNpcPeople_c::setAnmFromMsgTagUm(int messageAnimation) {
    return peoplePending_setAnmFromMsgTagUm(this, messageAnimation);
}

__attribute__((weak)) u32 daNpcPeople_c::setAnmFromMsgTagSa(int messageAnimation) {
    return peoplePending_setAnmFromMsgTagSa(this, messageAnimation);
}

__attribute__((weak)) u32 daNpcPeople_c::setAnmFromMsgTagUg(int messageAnimation) {
    return peoplePending_setAnmFromMsgTagUg(this, messageAnimation);
}

__attribute__((weak)) u8 daNpcPeople_c::getPrmNpcNo() {
    return peoplePending_getPrmNpcNo(this);
}

__attribute__((weak)) u8 daNpcPeople_c::getPrmRailID() {
    return peoplePending_getPrmRailID(this);
}

__attribute__((weak)) u8 daNpcPeople_c::getPrmArg0() {
    return peoplePending_getPrmArg0(this);
}

__attribute__((weak)) void daNpcPeople_c::setMtx() {
    peoplePending_setMtx(this);
}

__attribute__((weak)) void daNpcPeople_c::chkAttention() {
    peoplePending_chkAttention(this);
}

__attribute__((weak)) void daNpcPeople_c::lookBack() {
    peoplePending_lookBack(this);
}

__attribute__((weak)) BOOL daNpcPeople_c::initTexPatternAnm(u32 modify) {
    return peoplePending_initTexPatternAnm(this, modify);
}

__attribute__((weak)) void daNpcPeople_c::playTexPatternAnm() {
    peoplePending_playTexPatternAnm(this);
}

__attribute__((weak)) void daNpcPeople_c::playAnm() {
    peoplePending_playAnm(this);
}

__attribute__((weak)) void daNpcPeople_c::setAnm(u32 anmIdx, int loopMode, f32 morf, f32 speed) {
    peoplePending_setAnm(this, anmIdx, loopMode, morf, speed);
}

__attribute__((weak)) bool daNpcPeople_c::setAnmTbl(sPeopleAnmDat* anmData, int allowEnd) {
    return peoplePending_setAnmTbl(this, anmData, allowEnd);
}

__attribute__((weak)) void daNpcPeople_c::setWaitAnm() {
    peoplePending_setWaitAnm(this);
}

__attribute__((weak)) s16 daNpcPeople_c::XyCheckCB(int itemButton) {
    return peoplePending_XyCheckCB(this, itemButton);
}

__attribute__((weak)) s16 daNpcPeople_c::XyEventCB(int itemButton) {
    return peoplePending_XyEventCB(this, itemButton);
}

__attribute__((weak)) s16 daNpcPeople_c::photoCB(int photoIndex) {
    return peoplePending_photoCB(this, photoIndex);
}

__attribute__((weak)) int daNpcPeople_c::getRand(int max) {
    return peoplePending_getRand(this, max);
}

__attribute__((weak)) BOOL daNpcPeople_c::isPhoto(u8 photoType) {
    return peoplePending_isPhoto(this, photoType);
}

__attribute__((weak)) BOOL daNpcPeople_c::isColor() {
    return peoplePending_isColor(this);
}

__attribute__((weak)) void daNpcPeople_c::setCollision(dCcD_Cyl* cyl, cXyz* center, f32 radius,
                                                       f32 height) {
    peoplePending_setCollision(this, cyl, center, radius, height);
}

__attribute__((weak)) BOOL daNpcPeople_c::chkSurprise() {
    return peoplePending_chkSurprise(this);
}

__attribute__((weak)) BOOL daNpcPeople_c::chkEndEvent() {
    return peoplePending_chkEndEvent(this);
}

__attribute__((weak)) BOOL daNpcPeople_c::is1GetMap20() {
    return peoplePending_is1GetMap20(this);
}

__attribute__((weak)) BOOL daNpcPeople_c::is1DayGetMap20() {
    return peoplePending_is1DayGetMap20(this);
}

__attribute__((weak)) int daNpcPeople_c::getWindDir() {
    return peoplePending_getWindDir(this);
}

__attribute__((weak)) BOOL daNpcPeople_c::isUo1FdaiAll() {
    return peoplePending_isUo1FdaiAll(this);
}

__attribute__((weak)) BOOL daNpcPeople_c::isUo1FdaiOne() {
    return peoplePending_isUo1FdaiOne(this);
}

__attribute__((weak)) s32 daNpcPeople_c::chkDaiza() {
    return peoplePending_chkDaiza(this);
}

__attribute__((weak)) BOOL daNpcPeople_c::checkPig() {
    return peoplePending_checkPig(this);
}

__attribute__((weak)) BOOL daNpcPeople_c::isPigOk() {
    return peoplePending_isPigOk(this);
}

__attribute__((weak)) s16 daNpcPeople_c::getPigTimer() {
    return peoplePending_getPigTimer(this);
}

__attribute__((weak)) void daNpcPeople_c::resetPig() {
    peoplePending_resetPig(this);
}

__attribute__((weak)) void daNpcPeople_c::initUgSearchArea() {
    peoplePending_initUgSearchArea(this);
}

__attribute__((weak)) void daNpcPeople_c::getDirDistToPos(cXyz* result, s16 angle, f32 mag) {
    peoplePending_getDirDistToPos(this, result, angle, mag);
}

__attribute__((weak)) void daNpcPeople_c::warp() {
    peoplePending_warp(this);
}

/* WWHD glue (tools/verify/autoglue.py)
ext 022BE334 BOOL peoplePending_createHeap(daNpcPeople_c* npc)
ext 022BF510 cPhs_State peoplePending_createInit(daNpcPeople_c* npc)
ext 022BFF00 bool peoplePending__delete(daNpcPeople_c* npc)
ext 022C36D4 bool peoplePending__draw(daNpcPeople_c* npc)
ext 022C305C bool peoplePending__execute(daNpcPeople_c* npc)
ext 022C3AC4 bool peoplePending_executeCommon(daNpcPeople_c* npc)
ext 022C06F4 void peoplePending_executeSetMode(daNpcPeople_c* npc, u32 proc)
ext 022C3D88 s32 peoplePending_executeWaitInit(daNpcPeople_c* npc)
ext 022C4074 void peoplePending_executeWait(daNpcPeople_c* npc)
ext 022C43A8 s32 peoplePending_executeTalkInit(daNpcPeople_c* npc)
ext 022C4404 void peoplePending_executeTalk(daNpcPeople_c* npc)
ext 022C4600 s32 peoplePending_executeWalkInit(daNpcPeople_c* npc)
ext 022C46D0 void peoplePending_executeWalk(daNpcPeople_c* npc)
ext 022C48B8 s32 peoplePending_executeTurnInit(daNpcPeople_c* npc)
ext 022C4A04 void peoplePending_executeTurn(daNpcPeople_c* npc)
ext 022C4B30 s32 peoplePending_executeBikkuriInit(daNpcPeople_c* npc)
ext 022C4B80 void peoplePending_executeBikkuri(daNpcPeople_c* npc)
ext 022C4BF4 s32 peoplePending_executeFurueInit(daNpcPeople_c* npc)
ext 022C4CA8 void peoplePending_executeFurue(daNpcPeople_c* npc)
ext 022C4DDC s32 peoplePending_executeKyoroInit(daNpcPeople_c* npc)
ext 022C4E98 void peoplePending_executeKyoro(daNpcPeople_c* npc)
ext 022C4F54 s32 peoplePending_executeLetterInit(daNpcPeople_c* npc)
ext 022C4FA8 void peoplePending_executeLetter(daNpcPeople_c* npc)
ext 022C509C s32 peoplePending_executeLookInit(daNpcPeople_c* npc)
ext 022C511C void peoplePending_executeLook(daNpcPeople_c* npc)
ext 022C5208 s32 peoplePending_executeLook2Init(daNpcPeople_c* npc)
ext 022C5268 void peoplePending_executeLook2(daNpcPeople_c* npc)
ext 022C5368 s32 peoplePending_executeUgWalkInit(daNpcPeople_c* npc)
ext 022C5434 void peoplePending_executeUgWalk(daNpcPeople_c* npc)
ext 022C5AC4 s32 peoplePending_executeUgTurnInit(daNpcPeople_c* npc)
ext 022C5D4C void peoplePending_executeUgTurn(daNpcPeople_c* npc)
ext 022C5DFC s32 peoplePending_executeUgLookInit(daNpcPeople_c* npc)
ext 022C5E5C void peoplePending_executeUgLook(daNpcPeople_c* npc)
ext 022C5EF4 s32 peoplePending_executeUgLook2Init(daNpcPeople_c* npc)
ext 022C5F44 void peoplePending_executeUgLook2(daNpcPeople_c* npc)
ext 022C5FE8 s32 peoplePending_executeUgSitInit(daNpcPeople_c* npc)
ext 022C603C void peoplePending_executeUgSit(daNpcPeople_c* npc)
ext 022C045C void peoplePending_checkOrder(daNpcPeople_c* npc)
ext 022C2A04 void peoplePending_eventOrder(daNpcPeople_c* npc)
ext 022C2958 void peoplePending_eventMove(daNpcPeople_c* npc)
ext 022C25E8 void peoplePending_privateCut(daNpcPeople_c* npc)
ext 022C0EC0 void peoplePending_eventMesSetTpInit(daNpcPeople_c* npc, int staffIdx)
ext 022C0BF8 void peoplePending_eventMesSetInit(daNpcPeople_c* npc, int staffIdx)
ext 022C1EEC bool peoplePending_eventMesSet(daNpcPeople_c* npc)
ext 022C2170 bool peoplePending_eventMesSet2(daNpcPeople_c* npc)
ext 022C0F20 void peoplePending_eventFlagSetInit(daNpcPeople_c* npc, int staffIdx)
ext 022C0FC8 void peoplePending_eventGetItemInit(daNpcPeople_c* npc, int staffIdx)
ext 022C1094 void peoplePending_eventTurnToPlayerInit(daNpcPeople_c* npc)
ext 022C21D8 bool peoplePending_eventTurnToPlayer(daNpcPeople_c* npc)
ext 022C1228 void peoplePending_eventUb1TalkInit(daNpcPeople_c* npc, int staffIdx)
ext 022C21F0 bool peoplePending_eventUb1Talk(daNpcPeople_c* npc)
ext 022C1384 void peoplePending_eventUb1TalkXyInit(daNpcPeople_c* npc, int staffIdx)
ext 022C22B8 bool peoplePending_eventUb1TalkXy(daNpcPeople_c* npc)
ext 022C2380 bool peoplePending_eventUb2Talk(daNpcPeople_c* npc)
ext 022C2458 bool peoplePending_eventUbSetAnm(daNpcPeople_c* npc)
ext 022C167C void peoplePending_eventAreaMaxInit(daNpcPeople_c* npc)
ext 022C16A4 void peoplePending_eventCameraStopInit(daNpcPeople_c* npc)
ext 022C17C0 void peoplePending_eventCameraStartInit(daNpcPeople_c* npc)
ext 022C17FC void peoplePending_eventCoCylRInit(daNpcPeople_c* npc, int staffIdx)
ext 022C2518 bool peoplePending_eventLookPo(daNpcPeople_c* npc)
ext 022C1890 void peoplePending_eventMesSetPoInit(daNpcPeople_c* npc, int staffIdx)
ext 022C2588 bool peoplePending_eventMesSetPo(daNpcPeople_c* npc)
ext 022C1BBC u16 peoplePending_talk2(daNpcPeople_c* npc, int initializeMessage, fopAc_ac_c* speaker)
ext 022C1FD8 u16 peoplePending_talk3(daNpcPeople_c* npc, int initializeMessage)
ext 022C6108 u16 peoplePending_next_msgStatus(daNpcPeople_c* npc, be<u32>* pMsgNo)
ext 022C6C68 u32 peoplePending_getMsg(daNpcPeople_c* npc)
ext 022C1F20 u32 peoplePending_getMsg3(daNpcPeople_c* npc)
ext 022C192C void peoplePending_chkMsg(daNpcPeople_c* npc)
ext 022C0BF0 void peoplePending_setMessage(daNpcPeople_c* npc, u32 msgNo)
ext 022C1168 void peoplePending_setMessageUb(daNpcPeople_c* npc, sUbMsgDat* messageData)
ext 022C1B24 void peoplePending_setAnmFromMsgTag(daNpcPeople_c* npc)
ext 022C8550 u32 peoplePending_setAnmFromMsgTagUo(daNpcPeople_c* npc, int messageAnimation)
ext 022C1148 u32 peoplePending_setAnmFromMsgTagUb(daNpcPeople_c* npc, int messageAnimation)
ext 022C8570 u32 peoplePending_setAnmFromMsgTagUw(daNpcPeople_c* npc, int messageAnimation)
ext 022C85DC u32 peoplePending_setAnmFromMsgTagUm(daNpcPeople_c* npc, int messageAnimation)
ext 022C86D0 u32 peoplePending_setAnmFromMsgTagSa(daNpcPeople_c* npc, int messageAnimation)
ext 022C86F0 u32 peoplePending_setAnmFromMsgTagUg(daNpcPeople_c* npc, int messageAnimation)
ext 022BE7B0 u8 peoplePending_getPrmNpcNo(daNpcPeople_c* npc)
ext 022BEB50 u8 peoplePending_getPrmRailID(daNpcPeople_c* npc)
ext 022BE784 u8 peoplePending_getPrmArg0(daNpcPeople_c* npc)
ext 022BF3A0 void peoplePending_setMtx(daNpcPeople_c* npc)
ext 022C0084 void peoplePending_chkAttention(daNpcPeople_c* npc)
ext 022C2E50 void peoplePending_lookBack(daNpcPeople_c* npc)
ext 022BE1E8 BOOL peoplePending_initTexPatternAnm(daNpcPeople_c* npc, u32 modify)
ext 022C2CC0 void peoplePending_playTexPatternAnm(daNpcPeople_c* npc)
ext 022C2D6C void peoplePending_playAnm(daNpcPeople_c* npc)
ext 022BEB7C void peoplePending_setAnm(daNpcPeople_c* npc, u32 anmIdx, int loopMode, f32 morf, f32 speed)
ext 022BED70 bool peoplePending_setAnmTbl(daNpcPeople_c* npc, sPeopleAnmDat* anmData, int allowEnd)
ext 022BEEDC void peoplePending_setWaitAnm(daNpcPeople_c* npc)
ext 022BF05C s16 peoplePending_XyCheckCB(daNpcPeople_c* npc, int itemButton)
ext 022BF220 s16 peoplePending_XyEventCB(daNpcPeople_c* npc, int itemButton)
ext 022BF330 s16 peoplePending_photoCB(daNpcPeople_c* npc, int photoIndex)
ext 022BFF78 int peoplePending_getRand(daNpcPeople_c* npc, int max)
ext 022BEFF4 BOOL peoplePending_isPhoto(daNpcPeople_c* npc, u8 photoType)
ext 022C6910 BOOL peoplePending_isColor(daNpcPeople_c* npc)
ext 022BF480 void peoplePending_setCollision(daNpcPeople_c* npc, dCcD_Cyl* cyl, cXyz* center, f32 radius, f32 height)
ext 022C39F0 BOOL peoplePending_chkSurprise(daNpcPeople_c* npc)
ext 022C0840 BOOL peoplePending_chkEndEvent(daNpcPeople_c* npc)
ext 022BF304 BOOL peoplePending_is1GetMap20(daNpcPeople_c* npc)
ext 022C1108 BOOL peoplePending_is1DayGetMap20(daNpcPeople_c* npc)
ext 022C60D0 int peoplePending_getWindDir(daNpcPeople_c* npc)
ext 022C693C BOOL peoplePending_isUo1FdaiAll(daNpcPeople_c* npc)
ext 022C6978 BOOL peoplePending_isUo1FdaiOne(daNpcPeople_c* npc)
ext 022C69A0 s32 peoplePending_chkDaiza(daNpcPeople_c* npc)
ext 022C3E78 BOOL peoplePending_checkPig(daNpcPeople_c* npc)
ext 022C6C00 BOOL peoplePending_isPigOk(daNpcPeople_c* npc)
ext 022C6BDC s16 peoplePending_getPigTimer(daNpcPeople_c* npc)
ext 022C1894 void peoplePending_resetPig(daNpcPeople_c* npc)
ext 022C077C void peoplePending_initUgSearchArea(daNpcPeople_c* npc)
ext 022C5398 void peoplePending_getDirDistToPos(daNpcPeople_c* npc, cXyz* result, s16 angle, f32 mag)
ext 022BFFE4 void peoplePending_warp(daNpcPeople_c* npc)
*/
