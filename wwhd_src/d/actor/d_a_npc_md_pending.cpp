/* Medli fallback bindings. Strong definitions in each companion unit override these methods. */
#include "d/actor/d_a_npc_md_pending.h"

// HD: preserve register-width returns, boolean arguments, and pointer-to-member copies.
__attribute__((weak)) s16 daNpc_Md_c::XyCheckCB(int itemButton) {
    return mdPending_XyCheckCB(this, itemButton);
}

__attribute__((weak)) s16 daNpc_Md_c::XyEventCB(int itemButton) {
    return mdPending_XyEventCB(this, itemButton);
}

__attribute__((weak)) BOOL daNpc_Md_c::initLightBtkAnm(u32 modify) {
    return mdPending_initLightBtkAnm(this, modify);
}

__attribute__((weak)) BOOL daNpc_Md_c::initTexPatternAnm(u8 btpAnmTblIdx, u32 modify) {
    return mdPending_initTexPatternAnm(this, btpAnmTblIdx, modify);
}

__attribute__((weak)) BOOL daNpc_Md_c::createHeap() {
    return mdPending_createHeap(this);
}

__attribute__((weak)) BOOL daNpc_Md_c::setAction(ProcFunc_l* currentAction, ProcFunc_l* newAction,
                                                 void* arg) {
    return mdPending_setAction(this, currentAction, newAction, arg);
}

__attribute__((weak)) void daNpc_Md_c::setNpcAction(ProcFunc_l* action, void* arg) {
    mdPending_setNpcAction(this, action, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::shipRideCheck() {
    return mdPending_shipRideCheck(this);
}

__attribute__((weak)) BOOL daNpc_Md_c::init() {
    return mdPending_init(this);
}

__attribute__((weak)) void daNpc_Md_c::setAttention(bool force) {
    mdPending_setAttention(this, force);
}

__attribute__((weak)) void daNpc_Md_c::setBaseMtx() {
    mdPending_setBaseMtx(this);
}

__attribute__((weak)) cPhs_State daNpc_Md_c::create() {
    return mdPending_create(this);
}

__attribute__((weak)) void daNpc_Md_c::checkPlayerRoom() {
    mdPending_checkPlayerRoom(this);
}

__attribute__((weak)) bool daNpc_Md_c::dNpc_Md_setAnm(mDoExt_McaMorf2* pMorf, f32 blendRate,
                                                      int loopMode, f32 morf, f32 speed,
                                                      const char* fileName1, const char* filename2,
                                                      const char* arcName) {
    return mdPending_dNpc_Md_setAnm(this, pMorf, blendRate, loopMode, morf, speed, fileName1,
                                    filename2, arcName);
}

__attribute__((weak)) bool daNpc_Md_c::dNpc_Md_setAnm(mDoExt_McaMorf* pMorf, int loopMode, f32 morf,
                                                      f32 speed, const char* animFilename,
                                                      const char* arcName) {
    return mdPending_dNpc_Md_setAnm(this, pMorf, loopMode, morf, speed, animFilename, arcName);
}

__attribute__((weak)) void daNpc_Md_c::deletePiyoPiyo() {
    mdPending_deletePiyoPiyo(this);
}

__attribute__((weak)) BOOL daNpc_Md_c::setAnm(int anmIdx) {
    return mdPending_setAnm(this, anmIdx);
}

__attribute__((weak)) void daNpc_Md_c::playTexPatternAnm() {
    mdPending_playTexPatternAnm(this);
}

__attribute__((weak)) u32 daNpc_Md_c::playLightBtkAnm() {
    return mdPending_playLightBtkAnm(this);
}

__attribute__((weak)) void daNpc_Md_c::animationPlay() {
    mdPending_animationPlay(this);
}

__attribute__((weak)) BOOL daNpc_Md_c::isFallAction() {
    return mdPending_isFallAction(this);
}

__attribute__((weak)) BOOL daNpc_Md_c::lightHitCheck() {
    return mdPending_lightHitCheck(this);
}

__attribute__((weak)) void daNpc_Md_c::setCollision() {
    mdPending_setCollision(this);
}

__attribute__((weak)) BOOL daNpc_Md_c::checkCommandTalk() {
    return mdPending_checkCommandTalk(this);
}

__attribute__((weak)) void daNpc_Md_c::returnLinkPlayer() {
    mdPending_returnLinkPlayer(this);
}

__attribute__((weak)) BOOL daNpc_Md_c::eventProc() {
    return mdPending_eventProc(this);
}

__attribute__((weak)) void daNpc_Md_c::setPlayerAction(ProcFunc_l* action, void* arg) {
    mdPending_setPlayerAction(this, action, arg);
}

__attribute__((weak)) void daNpc_Md_c::playerAction(void* arg) {
    mdPending_playerAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::returnLinkCheck() {
    return mdPending_returnLinkCheck(this);
}

__attribute__((weak)) BOOL daNpc_Md_c::checkCollision(int playerControlled) {
    return mdPending_checkCollision(this, playerControlled);
}

__attribute__((weak)) void daNpc_Md_c::carryCheck() {
    mdPending_carryCheck(this);
}

__attribute__((weak)) void daNpc_Md_c::checkOrder() {
    mdPending_checkOrder(this);
}

__attribute__((weak)) void daNpc_Md_c::npcAction(void* arg) {
    mdPending_npcAction(this, arg);
}

__attribute__((weak)) void daNpc_Md_c::eventOrder() {
    mdPending_eventOrder(this);
}

__attribute__((weak)) BOOL daNpc_Md_c::execute() {
    return mdPending_execute(this);
}

__attribute__((weak)) BOOL daNpc_Md_c::draw() {
    return mdPending_draw(this);
}

__attribute__((weak)) void daNpc_Md_c::emitterTrace(JPABaseEmitter* emitter, Mtx34* mtx,
                                                    csXyz* angle) {
    mdPending_emitterTrace(this, emitter, mtx, angle);
}

__attribute__((weak)) void daNpc_Md_c::emitterDelete(be<u32>* emitter) {
    mdPending_emitterDelete(this, emitter);
}

__attribute__((weak)) void daNpc_Md_c::deleteHane02Emitter() {
    mdPending_deleteHane02Emitter(this);
}

__attribute__((weak)) void daNpc_Md_c::deleteHane03Emitter() {
    mdPending_deleteHane03Emitter(this);
}

__attribute__((weak)) void daNpc_Md_c::changeCaught02() {
    mdPending_changeCaught02(this);
}

__attribute__((weak)) void daNpc_Md_c::initialDefault(int staffIdx) {
    mdPending_initialDefault(this, staffIdx);
} /* restartPoint: vtable slot 028F036C */

__attribute__((weak)) void daNpc_Md_c::lookBack(int lookAtPlayer, int slowTurn, int fixedBody) {
    mdPending_lookBack(this, lookAtPlayer, slowTurn, fixedBody);
}

__attribute__((weak)) BOOL daNpc_Md_c::actionDefault(int staffIdx) {
    return mdPending_actionDefault(this, staffIdx);
}

__attribute__((weak)) void daNpc_Md_c::initialWaitEvent(int staffIdx) {
    mdPending_initialWaitEvent(this, staffIdx);
}

__attribute__((weak)) BOOL daNpc_Md_c::actionWaitEvent(int staffIdx) {
    return mdPending_actionWaitEvent(this, staffIdx);
}

__attribute__((weak)) void daNpc_Md_c::initialLetterEvent(int staffIdx) {
    mdPending_initialLetterEvent(this, staffIdx);
}

__attribute__((weak)) void daNpc_Md_c::initialMsgSetEvent(int staffIdx) {
    mdPending_initialMsgSetEvent(this, staffIdx);
}

__attribute__((weak)) BOOL daNpc_Md_c::talk_init() {
    return mdPending_talk_init(this);
}

__attribute__((weak)) BOOL daNpc_Md_c::actionMsgSetEvent(int staffIdx) {
    return mdPending_actionMsgSetEvent(this, staffIdx);
}

__attribute__((weak)) int daNpc_Md_c::getAnmType(u8 messageAnimation) {
    return mdPending_getAnmType(this, messageAnimation);
}

__attribute__((weak)) u16 daNpc_Md_c::next_msgStatus(be<u32>* currentMessage) {
    return mdPending_next_msgStatus(this, currentMessage);
}

__attribute__((weak)) BOOL daNpc_Md_c::talk(int allowCancel) {
    return mdPending_talk(this, allowCancel);
}

__attribute__((weak)) void daNpc_Md_c::setHarpPlayNum(int playNumber) {
    mdPending_setHarpPlayNum(this, playNumber);
}

__attribute__((weak)) void daNpc_Md_c::initialEndEvent(int staffIdx) {
    mdPending_initialEndEvent(this, staffIdx);
}

__attribute__((weak)) BOOL daNpc_Md_c::actionMsgEndEvent(int staffIdx) {
    return mdPending_actionMsgEndEvent(this, staffIdx);
}

__attribute__((weak)) void daNpc_Md_c::initialMovePosEvent(int staffIdx) {
    mdPending_initialMovePosEvent(this, staffIdx);
}

__attribute__((weak)) void daNpc_Md_c::particle_set(be<u32>* emitter, u16 particleID) {
    mdPending_particle_set(this, emitter, particleID);
}

__attribute__((weak)) void daNpc_Md_c::setWingEmitter() {
    mdPending_setWingEmitter(this);
}

__attribute__((weak)) void daNpc_Md_c::initialFlyEvent(int staffIdx) {
    mdPending_initialFlyEvent(this, staffIdx);
}

__attribute__((weak)) BOOL daNpc_Md_c::actionFlyEvent(int staffIdx) {
    return mdPending_actionFlyEvent(this, staffIdx);
}

__attribute__((weak)) void daNpc_Md_c::initialGlidingEvent(int staffIdx) {
    mdPending_initialGlidingEvent(this, staffIdx);
}

__attribute__((weak)) BOOL daNpc_Md_c::actionGlidingEvent(int staffIdx) {
    return mdPending_actionGlidingEvent(this, staffIdx);
}

__attribute__((weak)) void daNpc_Md_c::initialLandingEvent(int staffIdx) {
    mdPending_initialLandingEvent(this, staffIdx);
}

__attribute__((weak)) BOOL daNpc_Md_c::actionLandingEvent(int staffIdx) {
    return mdPending_actionLandingEvent(this, staffIdx);
}

__attribute__((weak)) void daNpc_Md_c::initialWalkEvent(int staffIdx) {
    mdPending_initialWalkEvent(this, staffIdx);
}

__attribute__((weak)) int daNpc_Md_c::wallHitCheck() {
    return mdPending_wallHitCheck(this);
}

__attribute__((weak)) BOOL daNpc_Md_c::actionWalkEvent(int staffIdx) {
    return mdPending_actionWalkEvent(this, staffIdx);
}

__attribute__((weak)) BOOL daNpc_Md_c::actionDashEvent(int staffIdx) {
    return mdPending_actionDashEvent(this, staffIdx);
}

__attribute__((weak)) BOOL daNpc_Md_c::actionTactEvent(int staffIdx) {
    return mdPending_actionTactEvent(this, staffIdx);
}

__attribute__((weak)) void daNpc_Md_c::initialTakeOffEvent(int staffIdx) {
    mdPending_initialTakeOffEvent(this, staffIdx);
}

__attribute__((weak)) BOOL daNpc_Md_c::actionTakeOffEvent(int staffIdx) {
    return mdPending_actionTakeOffEvent(this, staffIdx);
}

__attribute__((weak)) void daNpc_Md_c::initialOnetimeEvent(int staffIdx) {
    mdPending_initialOnetimeEvent(this, staffIdx);
}

__attribute__((weak)) BOOL daNpc_Md_c::actionOnetimeEvent(int staffIdx) {
    return mdPending_actionOnetimeEvent(this, staffIdx);
}

__attribute__((weak)) void daNpc_Md_c::initialQuake(int staffIdx) {
    mdPending_initialQuake(this, staffIdx);
}

__attribute__((weak)) void daNpc_Md_c::initialHarpPlayEvent(int staffIdx) {
    mdPending_initialHarpPlayEvent(this, staffIdx);
}

__attribute__((weak)) BOOL daNpc_Md_c::actionHarpPlayEvent(int staffIdx) {
    return mdPending_actionHarpPlayEvent(this, staffIdx);
}

__attribute__((weak)) void daNpc_Md_c::initialOffLinkEvent(int staffIdx) {
    mdPending_initialOffLinkEvent(this, staffIdx);
}

__attribute__((weak)) void daNpc_Md_c::initialOnLinkEvent(int staffIdx) {
    mdPending_initialOnLinkEvent(this, staffIdx);
}

__attribute__((weak)) void daNpc_Md_c::initialTurnEvent(int staffIdx) {
    mdPending_initialTurnEvent(this, staffIdx);
}

__attribute__((weak)) void daNpc_Md_c::lookBack(cXyz* target, int enabled, int fixedBody) {
    mdPending_lookBack(this, target, enabled, fixedBody);
}

__attribute__((weak)) BOOL daNpc_Md_c::actionTurnEvent(int staffIdx) {
    return mdPending_actionTurnEvent(this, staffIdx);
}

__attribute__((weak)) void daNpc_Md_c::initialSetAnmEvent(int staffIdx) {
    mdPending_initialSetAnmEvent(this, staffIdx);
}

__attribute__((weak)) u32 daNpc_Md_c::initialLookDown(int staffIdx) {
    return mdPending_initialLookDown(this, staffIdx);
}

__attribute__((weak)) BOOL daNpc_Md_c::actionLookDown(int staffIdx) {
    return mdPending_actionLookDown(this, staffIdx);
}

__attribute__((weak)) u32 daNpc_Md_c::initialLookUp(int staffIdx) {
    return mdPending_initialLookUp(this, staffIdx);
}

__attribute__((weak)) void daNpc_Md_c::setMessageAnimation(u8 msgAnmAtr) {
    mdPending_setMessageAnimation(this, msgAnmAtr);
}

__attribute__((weak)) bool daNpc_Md_c::chkAttention(cXyz* position, s16 angle, int ignoreAngle) {
    return mdPending_chkAttention(this, position, angle, ignoreAngle);
}

__attribute__((weak)) bool daNpc_Md_c::chkArea(cXyz* position) {
    return mdPending_chkArea(this, position);
}

__attribute__((weak)) u32 daNpc_Md_c::getMsg() {
    return mdPending_getMsg(this);
}

__attribute__((weak)) s32 daNpc_Md_c::lookBackWaist(s16 angle, f32 stickValue) {
    return mdPending_lookBackWaist(this, angle, stickValue);
}

__attribute__((weak)) void daNpc_Md_c::setHane02Emitter() {
    mdPending_setHane02Emitter(this);
}

__attribute__((weak)) void daNpc_Md_c::setHane03Emitter() {
    mdPending_setHane03Emitter(this);
}

__attribute__((weak)) void daNpc_Md_c::setNormalSpeedF(f32 rate, f32 acceleration, f32 scale,
                                                       f32 maxStep, f32 minStep) {
    mdPending_setNormalSpeedF(this, rate, acceleration, scale, maxStep, minStep);
}

__attribute__((weak)) void daNpc_Md_c::setSpeedAndAngleNormal(f32 rate, s16 angle) {
    mdPending_setSpeedAndAngleNormal(this, rate, angle);
}

__attribute__((weak)) void daNpc_Md_c::walkProc(f32 rate, s16 angle) {
    mdPending_walkProc(this, rate, angle);
}

__attribute__((weak)) s16 daNpc_Md_c::getStickAngY(int relativeToCamera) {
    return mdPending_getStickAngY(this, relativeToCamera);
}

__attribute__((weak)) int daNpc_Md_c::calcStickPos(s16 angle, cXyz* outPosition) {
    return mdPending_calcStickPos(this, angle, outPosition);
}

__attribute__((weak)) BOOL daNpc_Md_c::flyCheck() {
    return mdPending_flyCheck(this);
}

__attribute__((weak)) BOOL daNpc_Md_c::mirrorCancelCheck() {
    return mdPending_mirrorCancelCheck(this);
}

__attribute__((weak)) void daNpc_Md_c::NpcCall(be<s32>* called) {
    mdPending_NpcCall(this, called);
}

__attribute__((weak)) void daNpc_Md_c::waitGroundCheck() {
    mdPending_waitGroundCheck(this);
}

__attribute__((weak)) BOOL daNpc_Md_c::chkAdanmaeDemoOrder() {
    return mdPending_chkAdanmaeDemoOrder(this);
}

__attribute__((weak)) BOOL daNpc_Md_c::XYTalkCheck() {
    return mdPending_XYTalkCheck(this);
}

__attribute__((weak)) BOOL daNpc_Md_c::waitNpcAction(void* arg) {
    return mdPending_waitNpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::harpWaitNpcAction(void* arg) {
    return mdPending_harpWaitNpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::talkNpcAction(void* arg) {
    return mdPending_talkNpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::shipTalkNpcAction(void* arg) {
    return mdPending_shipTalkNpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::kyohiNpcAction(void* arg) {
    return mdPending_kyohiNpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::shipNpcAction(void* arg) {
    return mdPending_shipNpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::mwaitNpcAction(void* arg) {
    return mdPending_mwaitNpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::squatdownNpcAction(void* arg) {
    return mdPending_squatdownNpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::sqwait01NpcAction(void* arg) {
    return mdPending_sqwait01NpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::carryNpcAction(void* arg) {
    return mdPending_carryNpcAction(this, arg);
}

__attribute__((weak)) s16 daNpc_Md_c::windProc() {
    return mdPending_windProc(this);
}

__attribute__((weak)) BOOL daNpc_Md_c::throwNpcAction(void* arg) {
    return mdPending_throwNpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::glidingNpcAction(void* arg) {
    return mdPending_glidingNpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::fallNpcAction(void* arg) {
    return mdPending_fallNpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::fall02NpcAction(void* arg) {
    return mdPending_fall02NpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::wallHitNpcAction(void* arg) {
    return mdPending_wallHitNpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::land01NpcAction(void* arg) {
    return mdPending_land01NpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::land02NpcAction(void* arg) {
    return mdPending_land02NpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::land03NpcAction(void* arg) {
    return mdPending_land03NpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::piyo2NpcAction(void* arg) {
    return mdPending_piyo2NpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::deleteNpcAction(void* arg) {
    return mdPending_deleteNpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::demoFlyNpcAction(void* arg) {
    return mdPending_demoFlyNpcAction(this, arg);
}

__attribute__((weak)) f32 daNpc_Md_c::checkForwardGroundY(s16 angle) {
    return mdPending_checkForwardGroundY(this, angle);
}

__attribute__((weak)) f32 daNpc_Md_c::checkWallJump(s16 angle) {
    return mdPending_checkWallJump(this, angle);
}

__attribute__((weak)) void daNpc_Md_c::routeAngCheck(cXyz* normal, be<s16>* angle) {
    mdPending_routeAngCheck(this, normal, angle);
}

__attribute__((weak)) void daNpc_Md_c::routeWallCheck(cXyz* start, cXyz* end, be<s16>* angle) {
    mdPending_routeWallCheck(this, start, end, angle);
}

__attribute__((weak)) BOOL daNpc_Md_c::routeCheck(f32 distanceSquared, be<s16>* angle) {
    return mdPending_routeCheck(this, distanceSquared, angle);
}

__attribute__((weak)) BOOL daNpc_Md_c::searchNpcAction(void* arg) {
    return mdPending_searchNpcAction(this, arg);
}

// HD: this action has no identified GameCube counterpart; retain its shared method name.
__attribute__((weak)) BOOL daNpc_Md_c::npcAction_0229310C(void* arg) {
    return mdPending_unidentifiedNpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::hitNpcAction(void* arg) {
    return mdPending_hitNpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::jumpNpcAction(void* arg) {
    return mdPending_jumpNpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::escapeNpcAction(void* arg) {
    return mdPending_escapeNpcAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::waitPlayerAction(void* arg) {
    return mdPending_waitPlayerAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::walkPlayerAction(void* arg) {
    return mdPending_walkPlayerAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::hitPlayerAction(void* arg) {
    return mdPending_hitPlayerAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::jumpPlayerAction(void* arg) {
    return mdPending_jumpPlayerAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::flyPlayerAction(void* arg) {
    return mdPending_flyPlayerAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::landPlayerAction(void* arg) {
    return mdPending_landPlayerAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::mkamaePlayerAction(void* arg) {
    return mdPending_mkamaePlayerAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::carryPlayerAction(void* arg) {
    return mdPending_carryPlayerAction(this, arg);
}

__attribute__((weak)) BOOL daNpc_Md_c::isTagCheckOK() {
    return mdPending_isTagCheckOK(this);
}

/* WWHD glue (tools/verify/autoglue.py)
ext 02283480 s16 mdPending_XyCheckCB(daNpc_Md_c* npc, int itemButton)
ext 02283588 s16 mdPending_XyEventCB(daNpc_Md_c* npc, int itemButton)
ext 0228461C BOOL mdPending_initLightBtkAnm(daNpc_Md_c* npc, u32 modify)
ext 022846EC BOOL mdPending_initTexPatternAnm(daNpc_Md_c* npc, u8 btpAnmTblIdx, u32 modify)
ext 022847F8 BOOL mdPending_createHeap(daNpc_Md_c* npc)
ext 02285384 BOOL mdPending_setAction(daNpc_Md_c* npc, ProcFunc_l* currentAction, ProcFunc_l* newAction, void* arg)
ext 022854CC void mdPending_setNpcAction(daNpc_Md_c* npc, ProcFunc_l* action, void* arg)
ext 02285520 BOOL mdPending_shipRideCheck(daNpc_Md_c* npc)
ext 02285580 BOOL mdPending_init(daNpc_Md_c* npc)
ext 02285984 void mdPending_setAttention(daNpc_Md_c* npc, bool force)
ext 02285A6C void mdPending_setBaseMtx(daNpc_Md_c* npc)
ext 02286084 cPhs_State mdPending_create(daNpc_Md_c* npc)
ext 02286B5C void mdPending_checkPlayerRoom(daNpc_Md_c* npc)
ext 02286BC0 bool mdPending_dNpc_Md_setAnm(daNpc_Md_c* npc, mDoExt_McaMorf2* pMorf, f32 blendRate, int loopMode, f32 morf, f32 speed, const char* fileName1, const char* filename2, const char* arcName)
ext 02286D04 bool mdPending_dNpc_Md_setAnm(daNpc_Md_c* npc, mDoExt_McaMorf* pMorf, int loopMode, f32 morf, f32 speed, const char* animFilename, const char* arcName)
ext 02286D18 void mdPending_deletePiyoPiyo(daNpc_Md_c* npc)
ext 02286D7C BOOL mdPending_setAnm(daNpc_Md_c* npc, int anmIdx)
ext 0228729C void mdPending_playTexPatternAnm(daNpc_Md_c* npc)
ext 0228735C u32 mdPending_playLightBtkAnm(daNpc_Md_c* npc)
ext 02287364 void mdPending_animationPlay(daNpc_Md_c* npc)
ext 02287684 BOOL mdPending_isFallAction(daNpc_Md_c* npc)
ext 02287754 BOOL mdPending_lightHitCheck(daNpc_Md_c* npc)
ext 02287A74 void mdPending_setCollision(daNpc_Md_c* npc)
ext 02287DF0 BOOL mdPending_checkCommandTalk(daNpc_Md_c* npc)
ext 02287EAC void mdPending_returnLinkPlayer(daNpc_Md_c* npc)
ext 02287EFC BOOL mdPending_eventProc(daNpc_Md_c* npc)
ext 02288240 void mdPending_setPlayerAction(daNpc_Md_c* npc, ProcFunc_l* action, void* arg)
ext 02288290 void mdPending_playerAction(daNpc_Md_c* npc, void* arg)
ext 022884D4 BOOL mdPending_returnLinkCheck(daNpc_Md_c* npc)
ext 02288544 BOOL mdPending_checkCollision(daNpc_Md_c* npc, int playerControlled)
ext 022886B4 void mdPending_carryCheck(daNpc_Md_c* npc)
ext 022886FC void mdPending_checkOrder(daNpc_Md_c* npc)
ext 022887DC void mdPending_npcAction(daNpc_Md_c* npc, void* arg)
ext 022888C4 void mdPending_eventOrder(daNpc_Md_c* npc)
ext 02288B08 BOOL mdPending_execute(daNpc_Md_c* npc)
ext 02289E2C BOOL mdPending_draw(daNpc_Md_c* npc)
ext 02288AB4 void mdPending_emitterTrace(daNpc_Md_c* npc, JPABaseEmitter* emitter, Mtx34* mtx, csXyz* angle)
ext 0228A1C0 void mdPending_emitterDelete(daNpc_Md_c* npc, be<u32>* emitter)
ext 0228A1FC void mdPending_deleteHane02Emitter(daNpc_Md_c* npc)
ext 0228A228 void mdPending_deleteHane03Emitter(daNpc_Md_c* npc)
ext 0228A4E4 void mdPending_changeCaught02(daNpc_Md_c* npc)
ext 0228A520 void mdPending_initialDefault(daNpc_Md_c* npc, int staffIdx)
ext 0228A524 void mdPending_lookBack(daNpc_Md_c* npc, int lookAtPlayer, int slowTurn, int fixedBody)
ext 0228A67C BOOL mdPending_actionDefault(daNpc_Md_c* npc, int staffIdx)
ext 0228A6AC void mdPending_initialWaitEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228A7D4 BOOL mdPending_actionWaitEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228A800 void mdPending_initialLetterEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228A928 void mdPending_initialMsgSetEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228AA7C BOOL mdPending_talk_init(daNpc_Md_c* npc)
ext 0228AAF4 BOOL mdPending_actionMsgSetEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228AB40 int mdPending_getAnmType(daNpc_Md_c* npc, u8 messageAnimation)
ext 0228AB60 u16 mdPending_next_msgStatus(daNpc_Md_c* npc, be<u32>* currentMessage)
ext 0228B1B8 BOOL mdPending_talk(daNpc_Md_c* npc, int allowCancel)
ext 0228B3CC void mdPending_setHarpPlayNum(daNpc_Md_c* npc, int playNumber)
ext 0228B418 void mdPending_initialEndEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228B610 BOOL mdPending_actionMsgEndEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228B68C void mdPending_initialMovePosEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228C104 void mdPending_particle_set(daNpc_Md_c* npc, be<u32>* emitter, u16 particleID)
ext 0228C1B4 void mdPending_setWingEmitter(daNpc_Md_c* npc)
ext 0228C1C4 void mdPending_initialFlyEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228C234 BOOL mdPending_actionFlyEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228C338 void mdPending_initialGlidingEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228C354 BOOL mdPending_actionGlidingEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228C3E8 void mdPending_initialLandingEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228C454 BOOL mdPending_actionLandingEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228C4A4 void mdPending_initialWalkEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228C4D0 int mdPending_wallHitCheck(daNpc_Md_c* npc)
ext 0228C50C BOOL mdPending_actionWalkEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228C898 BOOL mdPending_actionDashEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228CB88 BOOL mdPending_actionTactEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228CCC0 void mdPending_initialTakeOffEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228CCF8 BOOL mdPending_actionTakeOffEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228CD40 void mdPending_initialOnetimeEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228CDFC BOOL mdPending_actionOnetimeEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228CE84 void mdPending_initialQuake(daNpc_Md_c* npc, int staffIdx)
ext 0228D0A4 void mdPending_initialHarpPlayEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228D0AC BOOL mdPending_actionHarpPlayEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228D0F0 void mdPending_initialOffLinkEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228D120 void mdPending_initialOnLinkEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228D150 void mdPending_initialTurnEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228D208 void mdPending_lookBack(daNpc_Md_c* npc, cXyz* target, int enabled, int fixedBody)
ext 0228D32C BOOL mdPending_actionTurnEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228D564 void mdPending_initialSetAnmEvent(daNpc_Md_c* npc, int staffIdx)
ext 0228D840 u32 mdPending_initialLookDown(daNpc_Md_c* npc, int staffIdx)
ext 0228D848 BOOL mdPending_actionLookDown(daNpc_Md_c* npc, int staffIdx)
ext 0228D948 u32 mdPending_initialLookUp(daNpc_Md_c* npc, int staffIdx)
ext 0228DA10 void mdPending_setMessageAnimation(daNpc_Md_c* npc, u8 msgAnmAtr)
ext 0228DAF4 bool mdPending_chkAttention(daNpc_Md_c* npc, cXyz* position, s16 angle, int ignoreAngle)
ext 0228DD60 bool mdPending_chkArea(daNpc_Md_c* npc, cXyz* position)
ext 0228DDEC u32 mdPending_getMsg(daNpc_Md_c* npc)
ext 0228DFF8 s32 mdPending_lookBackWaist(daNpc_Md_c* npc, s16 angle, f32 stickValue)
ext 0228E2D4 void mdPending_setHane02Emitter(daNpc_Md_c* npc)
ext 0228E328 void mdPending_setHane03Emitter(daNpc_Md_c* npc)
ext 0228E37C void mdPending_setNormalSpeedF(daNpc_Md_c* npc, f32 rate, f32 acceleration, f32 scale, f32 maxStep, f32 minStep)
ext 0228E4C0 void mdPending_setSpeedAndAngleNormal(daNpc_Md_c* npc, f32 rate, s16 angle)
ext 0228E5CC void mdPending_walkProc(daNpc_Md_c* npc, f32 rate, s16 angle)
ext 0228E658 s16 mdPending_getStickAngY(daNpc_Md_c* npc, int relativeToCamera)
ext 0228E6CC int mdPending_calcStickPos(daNpc_Md_c* npc, s16 angle, cXyz* outPosition)
ext 0228E804 BOOL mdPending_flyCheck(daNpc_Md_c* npc)
ext 0228E8AC BOOL mdPending_mirrorCancelCheck(daNpc_Md_c* npc)
ext 0228E8D8 void mdPending_NpcCall(daNpc_Md_c* npc, be<s32>* called)
ext 0228E9C0 void mdPending_waitGroundCheck(daNpc_Md_c* npc)
ext 0228EACC BOOL mdPending_chkAdanmaeDemoOrder(daNpc_Md_c* npc)
ext 0228EB50 BOOL mdPending_XYTalkCheck(daNpc_Md_c* npc)
ext 0228EB88 BOOL mdPending_waitNpcAction(daNpc_Md_c* npc, void* arg)
ext 0228F424 BOOL mdPending_harpWaitNpcAction(daNpc_Md_c* npc, void* arg)
ext 0228F610 BOOL mdPending_talkNpcAction(daNpc_Md_c* npc, void* arg)
ext 0228FA44 BOOL mdPending_shipTalkNpcAction(daNpc_Md_c* npc, void* arg)
ext 0228FC24 BOOL mdPending_kyohiNpcAction(daNpc_Md_c* npc, void* arg)
ext 0228FD20 BOOL mdPending_shipNpcAction(daNpc_Md_c* npc, void* arg)
ext 0228FF5C BOOL mdPending_mwaitNpcAction(daNpc_Md_c* npc, void* arg)
ext 02290234 BOOL mdPending_squatdownNpcAction(daNpc_Md_c* npc, void* arg)
ext 02290358 BOOL mdPending_sqwait01NpcAction(daNpc_Md_c* npc, void* arg)
ext 022904DC BOOL mdPending_carryNpcAction(daNpc_Md_c* npc, void* arg)
ext 02290CF8 s16 mdPending_windProc(daNpc_Md_c* npc)
ext 02291094 BOOL mdPending_throwNpcAction(daNpc_Md_c* npc, void* arg)
ext 02291304 BOOL mdPending_glidingNpcAction(daNpc_Md_c* npc, void* arg)
ext 02291644 BOOL mdPending_fallNpcAction(daNpc_Md_c* npc, void* arg)
ext 02291764 BOOL mdPending_fall02NpcAction(daNpc_Md_c* npc, void* arg)
ext 022919D4 BOOL mdPending_wallHitNpcAction(daNpc_Md_c* npc, void* arg)
ext 02291BE4 BOOL mdPending_land01NpcAction(daNpc_Md_c* npc, void* arg)
ext 02291D50 BOOL mdPending_land02NpcAction(daNpc_Md_c* npc, void* arg)
ext 02291E50 BOOL mdPending_land03NpcAction(daNpc_Md_c* npc, void* arg)
ext 02291F50 BOOL mdPending_piyo2NpcAction(daNpc_Md_c* npc, void* arg)
ext 0229216C BOOL mdPending_deleteNpcAction(daNpc_Md_c* npc, void* arg)
ext 022921B4 BOOL mdPending_demoFlyNpcAction(daNpc_Md_c* npc, void* arg)
ext 0229249C f32 mdPending_checkForwardGroundY(daNpc_Md_c* npc, s16 angle)
ext 02292650 f32 mdPending_checkWallJump(daNpc_Md_c* npc, s16 angle)
ext 022926D0 void mdPending_routeAngCheck(daNpc_Md_c* npc, cXyz* normal, be<s16>* angle)
ext 0229278C void mdPending_routeWallCheck(daNpc_Md_c* npc, cXyz* start, cXyz* end, be<s16>* angle)
ext 022928A8 BOOL mdPending_routeCheck(daNpc_Md_c* npc, f32 distanceSquared, be<s16>* angle)
ext 02292C48 BOOL mdPending_searchNpcAction(daNpc_Md_c* npc, void* arg)
ext 0229310C BOOL mdPending_unidentifiedNpcAction(daNpc_Md_c* npc, void* arg)
ext 0229325C BOOL mdPending_hitNpcAction(daNpc_Md_c* npc, void* arg)
ext 022933A0 BOOL mdPending_jumpNpcAction(daNpc_Md_c* npc, void* arg)
ext 02293500 BOOL mdPending_escapeNpcAction(daNpc_Md_c* npc, void* arg)
ext 02293840 BOOL mdPending_waitPlayerAction(daNpc_Md_c* npc, void* arg)
ext 02293B5C BOOL mdPending_walkPlayerAction(daNpc_Md_c* npc, void* arg)
ext 02293DDC BOOL mdPending_hitPlayerAction(daNpc_Md_c* npc, void* arg)
ext 02293FA8 BOOL mdPending_jumpPlayerAction(daNpc_Md_c* npc, void* arg)
ext 02294160 BOOL mdPending_flyPlayerAction(daNpc_Md_c* npc, void* arg)
ext 02294838 BOOL mdPending_landPlayerAction(daNpc_Md_c* npc, void* arg)
ext 02294984 BOOL mdPending_mkamaePlayerAction(daNpc_Md_c* npc, void* arg)
ext 02294C00 BOOL mdPending_carryPlayerAction(daNpc_Md_c* npc, void* arg)
ext 0228D950 BOOL mdPending_isTagCheckOK(daNpc_Md_c* npc)
*/
