/* Orca weak fallbacks. Decompiled definitions override these guest bindings. */
#include "d/actor/d_a_npc_ji1_pending.h"

__attribute__((weak)) BOOL daNpc_Ji1_c::isGuardAnim() {
    return ji1Pending_isGuardAnim(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::isAttackAnim() {
    return ji1Pending_isAttackAnim(this);
}

__attribute__((weak)) int daNpc_Ji1_c::isAttackFrame() {
    return ji1Pending_isAttackFrame(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::isItemWaitAnim() {
    return ji1Pending_isItemWaitAnim(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::isClearRecord(s16 hits) {
    return ji1Pending_isClearRecord(this, hits);
}

__attribute__((weak)) void daNpc_Ji1_c::setClearRecord(s16 hits) {
    ji1Pending_setClearRecord(this, hits);
}

__attribute__((weak)) void daNpc_Ji1_c::normalSubActionHarpoonGuard(s16 angle) {
    ji1Pending_normalSubActionHarpoonGuard(this, angle);
}

__attribute__((weak)) void daNpc_Ji1_c::normalSubActionGuard(s16 angle) {
    ji1Pending_normalSubActionGuard(this, angle);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::normalAction(void* actionData) {
    return ji1Pending_normalAction(this, actionData);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::kaitenExpAction(void* actionData) {
    return ji1Pending_kaitenExpAction(this, actionData);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::kaitenspeakAction(void* actionData) {
    return ji1Pending_kaitenspeakAction(this, actionData);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::kaitenwaitAction(void* actionData) {
    return ji1Pending_kaitenwaitAction(this, actionData);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::kaitenAction(void* actionData) {
    return ji1Pending_kaitenAction(this, actionData);
}

__attribute__((weak)) u32 daNpc_Ji1_c::getMsg1stType() {
    return ji1Pending_getMsg1stType(this);
}

__attribute__((weak)) u32 daNpc_Ji1_c::getMsg2ndType() {
    return ji1Pending_getMsg2ndType(this);
}

__attribute__((weak)) u32 daNpc_Ji1_c::getMsg() {
    return ji1Pending_getMsg(this);
}

__attribute__((weak)) u16 daNpc_Ji1_c::next_msgStatus(be<u32>* pMsgNo) {
    return ji1Pending_next_msgStatus(this, pMsgNo);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::talkAction(void* actionData) {
    return ji1Pending_talkAction(this, actionData);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::speakAction(void* actionData) {
    return ji1Pending_speakAction(this, actionData);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::speakBadAction(void* actionData) {
    return ji1Pending_speakBadAction(this, actionData);
}

__attribute__((weak)) void* daNpc_Ji1_c::initPosObject(void* pActor, void* pData) {
    return ji1Pending_initPosObject(pActor, pData);
}

__attribute__((weak)) void daNpc_Ji1_c::initPos(int positionIndex) {
    ji1Pending_initPos(this, positionIndex);
}

__attribute__((weak)) void daNpc_Ji1_c::createItem() {
    ji1Pending_createItem(this);
}

__attribute__((weak)) void daNpc_Ji1_c::set_mtx() {
    ji1Pending_set_mtx(this);
}

__attribute__((weak)) s32 daNpc_Ji1_c::getEventActionNo(int staffIdx) {
    return ji1Pending_getEventActionNo(this, staffIdx);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::eventAction(void* actionData) {
    return ji1Pending_eventAction(this, actionData);
}

__attribute__((weak)) u32 daNpc_Ji1_c::evn_init_pos_init(int staffIdx) {
    return ji1Pending_evn_init_pos_init(this, staffIdx);
}

__attribute__((weak)) u32 daNpc_Ji1_c::evn_setAnm_init(int staffIdx) {
    return ji1Pending_evn_setAnm_init(this, staffIdx);
}

__attribute__((weak)) u32 daNpc_Ji1_c::evn_talk_init(int staffIdx) {
    return ji1Pending_evn_talk_init(this, staffIdx);
}

__attribute__((weak)) u32 daNpc_Ji1_c::evn_talk() {
    return ji1Pending_evn_talk(this);
}

__attribute__((weak)) u32 daNpc_Ji1_c::evn_continue_talk_init(int staffIdx) {
    return ji1Pending_evn_continue_talk_init(this, staffIdx);
}

__attribute__((weak)) u32 daNpc_Ji1_c::evn_continue_talk() {
    return ji1Pending_evn_continue_talk(this);
}

__attribute__((weak)) u32 daNpc_Ji1_c::evn_setAngle_init(int staffIdx) {
    return ji1Pending_evn_setAngle_init(this, staffIdx);
}

__attribute__((weak)) u32 daNpc_Ji1_c::evn_sound_proc_init(int staffIdx) {
    return ji1Pending_evn_sound_proc_init(this, staffIdx);
}

__attribute__((weak)) u32 daNpc_Ji1_c::evn_head_swing_init(int staffIdx) {
    return ji1Pending_evn_head_swing_init(this, staffIdx);
}

__attribute__((weak)) u32 daNpc_Ji1_c::evn_harpoon_proc_init(int staffIdx) {
    return ji1Pending_evn_harpoon_proc_init(this, staffIdx);
}

__attribute__((weak)) u32 daNpc_Ji1_c::evn_RollAtControl_init(int staffIdx) {
    return ji1Pending_evn_RollAtControl_init(this, staffIdx);
}

__attribute__((weak)) u32 daNpc_Ji1_c::evn_RollAtControl() {
    return ji1Pending_evn_RollAtControl(this);
}

__attribute__((weak)) u32 daNpc_Ji1_c::evn_game_mode_init(int staffIdx) {
    return ji1Pending_evn_game_mode_init(this, staffIdx);
}

__attribute__((weak)) u32 daNpc_Ji1_c::evn_turn_to_player() {
    return ji1Pending_evn_turn_to_player(this);
}

__attribute__((weak)) u32 daNpc_Ji1_c::evn_hide_init(int staffIdx) {
    return ji1Pending_evn_hide_init(this, staffIdx);
}

__attribute__((weak)) void daNpc_Ji1_c::AnimeControlToWait() {
    ji1Pending_AnimeControlToWait(this);
}

__attribute__((weak)) u32 daNpc_Ji1_c::privateCut() {
    return ji1Pending_privateCut(this);
}

__attribute__((weak)) u32 daNpc_Ji1_c::setParticle(int max, f32 rate, f32 spread) {
    return ji1Pending_setParticle(this, max, rate, spread);
}

__attribute__((weak)) void daNpc_Ji1_c::dtParticle() {
    ji1Pending_dtParticle(this);
}

__attribute__((weak)) u32 daNpc_Ji1_c::setParticleAT(int max, f32 rate, f32 spread) {
    return ji1Pending_setParticleAT(this, max, rate, spread);
}

__attribute__((weak)) void daNpc_Ji1_c::dtParticleAT() {
    ji1Pending_dtParticleAT(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::startspeakAction(void* actionData) {
    return ji1Pending_startspeakAction(this, actionData);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::endspeakAction(void* actionData) {
    return ji1Pending_endspeakAction(this, actionData);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::reiAction(void* actionData) {
    return ji1Pending_reiAction(this, actionData);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::plmoveAction(void* actionData) {
    return ji1Pending_plmoveAction(this, actionData);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::teachMove(f32 speedScale) {
    return ji1Pending_teachMove(this, speedScale);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::teachSpRollCutMove(f32 speedScale) {
    return ji1Pending_teachSpRollCutMove(this, speedScale);
}

__attribute__((weak)) f32 daNpc_Ji1_c::calcCoCorrectValue() {
    return ji1Pending_calcCoCorrectValue(this);
}

__attribute__((weak)) f32 daNpc_Ji1_c::calcBgCorrectValue() {
    return ji1Pending_calcBgCorrectValue(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::MoveToPlayer(f32 distance, u8 mode) {
    return ji1Pending_MoveToPlayer(this, distance, mode);
}

__attribute__((weak)) void daNpc_Ji1_c::teachSubActionAttackInit() {
    ji1Pending_teachSubActionAttackInit(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::teachSubActionAttack() {
    return ji1Pending_teachSubActionAttack(this);
}

__attribute__((weak)) void daNpc_Ji1_c::teachSubActionJumpInit() {
    ji1Pending_teachSubActionJumpInit(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::teachSubActionJump() {
    return ji1Pending_teachSubActionJump(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::teachAction(void* actionData) {
    return ji1Pending_teachAction(this, actionData);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::teachSPRollCutAction(void* actionData) {
    return ji1Pending_teachSPRollCutAction(this, actionData);
}

__attribute__((weak)) void daNpc_Ji1_c::battleGameSetTimer() {
    ji1Pending_battleGameSetTimer(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::battleMove(f32 speedScale) {
    return ji1Pending_battleMove(this, speedScale);
}

__attribute__((weak)) void daNpc_Ji1_c::battleSubActionWaitInit() {
    ji1Pending_battleSubActionWaitInit(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::battleSubActionWait() {
    return ji1Pending_battleSubActionWait(this);
}

__attribute__((weak)) void daNpc_Ji1_c::battleSubActionNockBackInit(int mode) {
    ji1Pending_battleSubActionNockBackInit(this, mode);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::battleSubActionNockBack() {
    return ji1Pending_battleSubActionNockBack(this);
}

__attribute__((weak)) void daNpc_Ji1_c::battleSubActionAttackInit() {
    ji1Pending_battleSubActionAttackInit(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::battleSubActionAttack() {
    return ji1Pending_battleSubActionAttack(this);
}

__attribute__((weak)) void daNpc_Ji1_c::battleSubActionTateAttackInit() {
    ji1Pending_battleSubActionTateAttackInit(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::battleSubActionTateAttack() {
    return ji1Pending_battleSubActionTateAttack(this);
}

__attribute__((weak)) void daNpc_Ji1_c::battleSubActionYokoAttackInit() {
    ji1Pending_battleSubActionYokoAttackInit(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::battleSubActionYokoAttack() {
    return ji1Pending_battleSubActionYokoAttack(this);
}

__attribute__((weak)) void daNpc_Ji1_c::battleSubActionJumpInit() {
    ji1Pending_battleSubActionJumpInit(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::battleSubActionJump() {
    return ji1Pending_battleSubActionJump(this);
}

__attribute__((weak)) void daNpc_Ji1_c::battleSubActionDamageInit() {
    ji1Pending_battleSubActionDamageInit(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::battleSubActionDamage() {
    return ji1Pending_battleSubActionDamage(this);
}

__attribute__((weak)) void daNpc_Ji1_c::battleSubActionJpGuardInit() {
    ji1Pending_battleSubActionJpGuardInit(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::battleSubActionJpGuard() {
    return ji1Pending_battleSubActionJpGuard(this);
}

__attribute__((weak)) void daNpc_Ji1_c::battleSubActionGuardInit() {
    ji1Pending_battleSubActionGuardInit(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::battleSubActionGuard() {
    return ji1Pending_battleSubActionGuard(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::battleAtSet() {
    return ji1Pending_battleAtSet(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::battleGuardCheck() {
    return ji1Pending_battleGuardCheck(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::battleAction(void* actionData) {
    return ji1Pending_battleAction(this, actionData);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::checkCutType(int cutType, int cutGroup) {
    return ji1Pending_checkCutType(this, cutType, cutGroup);
}

__attribute__((weak)) void daNpc_Ji1_c::setAnimFromMsgNo(u32 msgNo) {
    ji1Pending_setAnimFromMsgNo(this, msgNo);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::setAnm(int animation, f32 morf, int force) {
    return ji1Pending_setAnm(this, animation, morf, force);
}

__attribute__((weak)) cPhs_State daNpc_Ji1_c::_create() {
    return ji1Pending__create(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::CreateHeap() {
    return ji1Pending_CreateHeap(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::CreateInit() {
    return ji1Pending_CreateInit(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::_delete() {
    return ji1Pending__delete(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::_execute() {
    return ji1Pending__execute(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::_draw() {
    return ji1Pending__draw(this);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::chkAttention(cXyz* position, s16 angle) {
    return ji1Pending_chkAttention(this, position, angle);
}

__attribute__((weak)) BOOL daNpc_Ji1_c::lookBack() {
    return ji1Pending_lookBack(this);
}

__attribute__((weak)) void daNpc_Ji1_c::setHitParticle(cXyz* scale, u32 soundId) {
    ji1Pending_setHitParticle(this, scale, soundId);
}

__attribute__((weak)) void daNpc_Ji1_c::setGuardParticle() {
    ji1Pending_setGuardParticle(this);
}

__attribute__((weak)) void daNpc_Ji1_c::BackSlideInit() {
    ji1Pending_BackSlideInit(this);
}

__attribute__((weak)) void daNpc_Ji1_c::BackSlide(f32 speed, f32 maxStep) {
    ji1Pending_BackSlide(this, speed, maxStep);
}

__attribute__((weak)) void daNpc_Ji1_c::harpoonRelease(cXyz* velocity) {
    ji1Pending_harpoonRelease(this, velocity);
}

__attribute__((weak)) void daNpc_Ji1_c::harpoonMove() {
    ji1Pending_harpoonMove(this);
}

__attribute__((weak)) s16 daNpc_Ji1_XyCheckCB(void* actor, int mode) {
    return ji1Pending_daNpc_Ji1_XyCheckCB(actor, mode);
}

__attribute__((weak)) void daJi1_CoHitCallback(fopAc_ac_c* actor, dCcD_GObjInf* collider,
                                               fopAc_ac_c* otherActor,
                                               dCcD_GObjInf* otherCollider) {
    ji1Pending_daJi1_CoHitCallback(actor, collider, otherActor, otherCollider);
}

__attribute__((weak)) void daJi1_TgHitCallback(fopAc_ac_c* actor, dCcD_GObjInf* collider,
                                               fopAc_ac_c* otherActor,
                                               dCcD_GObjInf* otherCollider) {
    ji1Pending_daJi1_TgHitCallback(actor, collider, otherActor, otherCollider);
}

__attribute__((weak)) void daJi1_AtHitCallback(fopAc_ac_c* actor, dCcD_GObjInf* collider,
                                               fopAc_ac_c* otherActor,
                                               dCcD_GObjInf* otherCollider) {
    ji1Pending_daJi1_AtHitCallback(actor, collider, otherActor, otherCollider);
}

__attribute__((weak)) BOOL daNpc_Ji1_plRoomOutCheck() {
    return ji1Pending_daNpc_Ji1_plRoomOutCheck();
}

__attribute__((weak)) u32 playerCutAtCheck() {
    return ji1Pending_playerCutAtCheck();
}

__attribute__((weak)) BOOL nodeCallBack1(J3DNode* node, int phase) {
    return ji1Pending_nodeCallBack1(node, phase);
}

__attribute__((weak)) BOOL nodeCallBack2(J3DNode* node, int phase) {
    return ji1Pending_nodeCallBack2(node, phase);
}

__attribute__((weak)) BOOL nodeCallBack3(J3DNode* node, int phase) {
    return ji1Pending_nodeCallBack3(node, phase);
}

// HD: Keep the guest ABI, including the static initPosObject callback without an actor argument.
/* WWHD glue (tools/verify/autoglue.py)
ext 0224AB7C BOOL ji1Pending_isGuardAnim(daNpc_Ji1_c* npc)
ext 0224D4F8 BOOL ji1Pending_isAttackAnim(daNpc_Ji1_c* npc)
ext 0224D524 int ji1Pending_isAttackFrame(daNpc_Ji1_c* npc)
ext 0224A9F0 BOOL ji1Pending_isItemWaitAnim(daNpc_Ji1_c* npc)
ext 0224D5BC BOOL ji1Pending_isClearRecord(daNpc_Ji1_c* npc, s16 hits)
ext 0224D654 void ji1Pending_setClearRecord(daNpc_Ji1_c* npc, s16 hits)
ext 0224FCC4 void ji1Pending_normalSubActionHarpoonGuard(daNpc_Ji1_c* npc, s16 angle)
ext 022504E4 void ji1Pending_normalSubActionGuard(daNpc_Ji1_c* npc, s16 angle)
ext 02250A1C BOOL ji1Pending_normalAction(daNpc_Ji1_c* npc, void* actionData)
ext 02251690 BOOL ji1Pending_kaitenExpAction(daNpc_Ji1_c* npc, void* actionData)
ext 02251424 BOOL ji1Pending_kaitenspeakAction(daNpc_Ji1_c* npc, void* actionData)
ext 02250D54 BOOL ji1Pending_kaitenwaitAction(daNpc_Ji1_c* npc, void* actionData)
ext 02251ED0 BOOL ji1Pending_kaitenAction(daNpc_Ji1_c* npc, void* actionData)
ext 0224D8E0 u32 ji1Pending_getMsg1stType(daNpc_Ji1_c* npc)
ext 0224D718 u32 ji1Pending_getMsg2ndType(daNpc_Ji1_c* npc)
ext 0224DA20 u32 ji1Pending_getMsg(daNpc_Ji1_c* npc)
ext 0224DA74 u16 ji1Pending_next_msgStatus(daNpc_Ji1_c* npc, be<u32>* pMsgNo)
ext 0225268C BOOL ji1Pending_talkAction(daNpc_Ji1_c* npc, void* actionData)
ext 02252FD0 BOOL ji1Pending_speakAction(daNpc_Ji1_c* npc, void* actionData)
ext 02254A64 BOOL ji1Pending_speakBadAction(daNpc_Ji1_c* npc, void* actionData)
ext 022542A0 void* ji1Pending_initPosObject(void* pActor, void* pData)
ext 02254390 void ji1Pending_initPos(daNpc_Ji1_c* npc, int positionIndex)
ext 02258574 void ji1Pending_createItem(daNpc_Ji1_c* npc)
ext 0224AEF0 void ji1Pending_set_mtx(daNpc_Ji1_c* npc)
ext 022513D8 s32 ji1Pending_getEventActionNo(daNpc_Ji1_c* npc, int staffIdx)
ext 022596CC BOOL ji1Pending_eventAction(daNpc_Ji1_c* npc, void* actionData)
ext 02258508 u32 ji1Pending_evn_init_pos_init(daNpc_Ji1_c* npc, int staffIdx)
ext 022582E4 u32 ji1Pending_evn_setAnm_init(daNpc_Ji1_c* npc, int staffIdx)
ext 022583A4 u32 ji1Pending_evn_talk_init(daNpc_Ji1_c* npc, int staffIdx)
ext 02258CB0 u32 ji1Pending_evn_talk(daNpc_Ji1_c* npc)
ext 02258BE8 u32 ji1Pending_evn_continue_talk_init(daNpc_Ji1_c* npc, int staffIdx)
ext 02259178 u32 ji1Pending_evn_continue_talk(daNpc_Ji1_c* npc)
ext 02258C4C u32 ji1Pending_evn_setAngle_init(daNpc_Ji1_c* npc, int staffIdx)
ext 02258794 u32 ji1Pending_evn_sound_proc_init(daNpc_Ji1_c* npc, int staffIdx)
ext 02258894 u32 ji1Pending_evn_head_swing_init(daNpc_Ji1_c* npc, int staffIdx)
ext 0225896C u32 ji1Pending_evn_harpoon_proc_init(daNpc_Ji1_c* npc, int staffIdx)
ext 02258A14 u32 ji1Pending_evn_RollAtControl_init(daNpc_Ji1_c* npc, int staffIdx)
ext 02258E6C u32 ji1Pending_evn_RollAtControl(daNpc_Ji1_c* npc)
ext 02258A78 u32 ji1Pending_evn_game_mode_init(daNpc_Ji1_c* npc, int staffIdx)
ext 0225911C u32 ji1Pending_evn_turn_to_player(daNpc_Ji1_c* npc)
ext 02258B48 u32 ji1Pending_evn_hide_init(daNpc_Ji1_c* npc, int staffIdx)
ext 022595EC void ji1Pending_AnimeControlToWait(daNpc_Ji1_c* npc)
ext 02259334 u32 ji1Pending_privateCut(daNpc_Ji1_c* npc)
ext 0224FBAC u32 ji1Pending_setParticle(daNpc_Ji1_c* npc, int max, f32 rate, f32 spread)
ext 0224FB98 void ji1Pending_dtParticle(daNpc_Ji1_c* npc)
ext 0225B720 u32 ji1Pending_setParticleAT(daNpc_Ji1_c* npc, int max, f32 rate, f32 spread)
ext 0225AEB4 void ji1Pending_dtParticleAT(daNpc_Ji1_c* npc)
ext 02253880 BOOL ji1Pending_startspeakAction(daNpc_Ji1_c* npc, void* actionData)
ext 02253EE8 BOOL ji1Pending_endspeakAction(daNpc_Ji1_c* npc, void* actionData)
ext 02254434 BOOL ji1Pending_reiAction(daNpc_Ji1_c* npc, void* actionData)
ext 02257C9C BOOL ji1Pending_plmoveAction(daNpc_Ji1_c* npc, void* actionData)
ext 0224EF14 BOOL ji1Pending_teachMove(daNpc_Ji1_c* npc, f32 speedScale)
ext 0224F368 BOOL ji1Pending_teachSpRollCutMove(daNpc_Ji1_c* npc, f32 speedScale)
ext 0224F528 f32 ji1Pending_calcCoCorrectValue(daNpc_Ji1_c* npc)
ext 0224F59C f32 ji1Pending_calcBgCorrectValue(daNpc_Ji1_c* npc)
ext 0224F628 BOOL ji1Pending_MoveToPlayer(daNpc_Ji1_c* npc, f32 distance, u8 mode)
ext 0225589C void ji1Pending_teachSubActionAttackInit(daNpc_Ji1_c* npc)
ext 02255654 BOOL ji1Pending_teachSubActionAttack(daNpc_Ji1_c* npc)
ext 022552E4 void ji1Pending_teachSubActionJumpInit(daNpc_Ji1_c* npc)
ext 022554D0 BOOL ji1Pending_teachSubActionJump(daNpc_Ji1_c* npc)
ext 02255908 BOOL ji1Pending_teachAction(daNpc_Ji1_c* npc, void* actionData)
ext 02256F1C BOOL ji1Pending_teachSPRollCutAction(daNpc_Ji1_c* npc, void* actionData)
ext 02259A6C void ji1Pending_battleGameSetTimer(daNpc_Ji1_c* npc)
ext 02259EC4 BOOL ji1Pending_battleMove(daNpc_Ji1_c* npc, f32 speedScale)
ext 0225A2C8 void ji1Pending_battleSubActionWaitInit(daNpc_Ji1_c* npc)
ext 0225B344 BOOL ji1Pending_battleSubActionWait(daNpc_Ji1_c* npc)
ext 02248E0C void ji1Pending_battleSubActionNockBackInit(daNpc_Ji1_c* npc, int mode)
ext 0225C208 BOOL ji1Pending_battleSubActionNockBack(daNpc_Ji1_c* npc)
ext 0225B2B8 void ji1Pending_battleSubActionAttackInit(daNpc_Ji1_c* npc)
ext 0225BF44 BOOL ji1Pending_battleSubActionAttack(daNpc_Ji1_c* npc)
ext 0225B22C void ji1Pending_battleSubActionTateAttackInit(daNpc_Ji1_c* npc)
ext 0225B85C BOOL ji1Pending_battleSubActionTateAttack(daNpc_Ji1_c* npc)
ext 0225B1A0 void ji1Pending_battleSubActionYokoAttackInit(daNpc_Ji1_c* npc)
ext 0225BBA4 BOOL ji1Pending_battleSubActionYokoAttack(daNpc_Ji1_c* npc)
ext 0225A2F4 void ji1Pending_battleSubActionJumpInit(daNpc_Ji1_c* npc)
ext 0225C358 BOOL ji1Pending_battleSubActionJump(daNpc_Ji1_c* npc)
ext 0225A384 void ji1Pending_battleSubActionDamageInit(daNpc_Ji1_c* npc)
ext 0225CA04 BOOL ji1Pending_battleSubActionDamage(daNpc_Ji1_c* npc)
ext 0225A4A4 void ji1Pending_battleSubActionJpGuardInit(daNpc_Ji1_c* npc)
ext 0225C660 BOOL ji1Pending_battleSubActionJpGuard(daNpc_Ji1_c* npc)
ext 0225A424 void ji1Pending_battleSubActionGuardInit(daNpc_Ji1_c* npc)
ext 0225C7EC BOOL ji1Pending_battleSubActionGuard(daNpc_Ji1_c* npc)
ext 0225B4AC BOOL ji1Pending_battleAtSet(daNpc_Ji1_c* npc)
ext 0225A534 BOOL ji1Pending_battleGuardCheck(daNpc_Ji1_c* npc)
ext 0225AEC8 BOOL ji1Pending_battleAction(daNpc_Ji1_c* npc, void* actionData)
ext 02255100 BOOL ji1Pending_checkCutType(daNpc_Ji1_c* npc, int cutType, int cutGroup)
ext 02251A8C void ji1Pending_setAnimFromMsgNo(daNpc_Ji1_c* npc, u32 msgNo)
ext 0224DFF0 BOOL ji1Pending_setAnm(daNpc_Ji1_c* npc, int animation, f32 morf, int force)
ext 0224D280 cPhs_State ji1Pending__create(daNpc_Ji1_c* npc)
ext 02249884 BOOL ji1Pending_CreateHeap(daNpc_Ji1_c* npc)
ext 0224C650 BOOL ji1Pending_CreateInit(daNpc_Ji1_c* npc)
ext 0224C550 BOOL ji1Pending__delete(daNpc_Ji1_c* npc)
ext 0224B268 BOOL ji1Pending__execute(daNpc_Ji1_c* npc)
ext 02249638 BOOL ji1Pending__draw(daNpc_Ji1_c* npc)
ext 0224AA24 BOOL ji1Pending_chkAttention(daNpc_Ji1_c* npc, cXyz* position, s16 angle)
ext 0224ABA4 BOOL ji1Pending_lookBack(daNpc_Ji1_c* npc)
ext 02255358 void ji1Pending_setHitParticle(daNpc_Ji1_c* npc, cXyz* scale, u32 soundId)
ext 02248D3C void ji1Pending_setGuardParticle(daNpc_Ji1_c* npc)
ext 0224DE14 void ji1Pending_BackSlideInit(daNpc_Ji1_c* npc)
ext 022525B4 void ji1Pending_BackSlide(daNpc_Ji1_c* npc, f32 speed, f32 maxStep)
ext 0224DE3C void ji1Pending_harpoonRelease(daNpc_Ji1_c* npc, cXyz* velocity)
ext 0224A2B8 void ji1Pending_harpoonMove(daNpc_Ji1_c* npc)
ext 02248C64 s16 ji1Pending_daNpc_Ji1_XyCheckCB(void* actor, int mode)
ext 02248CA4 void ji1Pending_daJi1_CoHitCallback(fopAc_ac_c* actor, dCcD_GObjInf* collider, fopAc_ac_c* otherActor, dCcD_GObjInf* otherCollider)
ext 02248CF0 void ji1Pending_daJi1_TgHitCallback(fopAc_ac_c* actor, dCcD_GObjInf* collider, fopAc_ac_c* otherActor, dCcD_GObjInf* otherCollider)
ext 02248EA0 void ji1Pending_daJi1_AtHitCallback(fopAc_ac_c* actor, dCcD_GObjInf* collider, fopAc_ac_c* otherActor, dCcD_GObjInf* otherCollider)
ext 02248F58 BOOL ji1Pending_daNpc_Ji1_plRoomOutCheck()
ext 02249058 u32 ji1Pending_playerCutAtCheck()
ext 02249084 BOOL ji1Pending_nodeCallBack1(J3DNode* node, int phase)
ext 02249224 BOOL ji1Pending_nodeCallBack2(J3DNode* node, int phase)
ext 022494C8 BOOL ji1Pending_nodeCallBack3(J3DNode* node, int phase)
*/
