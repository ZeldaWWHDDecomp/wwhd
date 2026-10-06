/* d_a_npc_ji1: functions of the translation unit not decompiled yet, as plain guest calls (so the
 * decompiled parts can call them naturally). Weak: a decompiled definition in a part replaces it.
 */
#include "d/actor/d_a_npc_ji1.h"

__attribute__((weak)) BOOL daNpc_Ji1_c::isGuardAnim() { return gabi::call<BOOL>(0x0224AB7C, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::isAttackAnim() { return gabi::call<BOOL>(0x0224D4F8, this); }
__attribute__((weak)) int daNpc_Ji1_c::isAttackFrame() { return gabi::call<int>(0x0224D524, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::isItemWaitAnim() { return gabi::call<BOOL>(0x0224A9F0, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::isClearRecord(s16 a0) { return gabi::call<BOOL>(0x0224D5BC, this, a0); }
__attribute__((weak)) void daNpc_Ji1_c::setClearRecord(s16 a0) { gabi::call(0x0224D654, this, a0); }
__attribute__((weak)) void daNpc_Ji1_c::normalSubActionHarpoonGuard(s16 a0) { gabi::call(0x0224FCC4, this, a0); }
__attribute__((weak)) void daNpc_Ji1_c::normalSubActionGuard(s16 a0) { gabi::call(0x022504E4, this, a0); }
__attribute__((weak)) BOOL daNpc_Ji1_c::normalAction(void* a0) { return gabi::call<BOOL>(0x02250A1C, this, a0); }
__attribute__((weak)) BOOL daNpc_Ji1_c::kaitenExpAction(void* a0) { return gabi::call<BOOL>(0x02251690, this, a0); }
__attribute__((weak)) BOOL daNpc_Ji1_c::kaitenspeakAction(void* a0) { return gabi::call<BOOL>(0x02251424, this, a0); }
__attribute__((weak)) BOOL daNpc_Ji1_c::kaitenwaitAction(void* a0) { return gabi::call<BOOL>(0x02250D54, this, a0); }
__attribute__((weak)) BOOL daNpc_Ji1_c::kaitenAction(void* a0) { return gabi::call<BOOL>(0x02251ED0, this, a0); }
__attribute__((weak)) u32 daNpc_Ji1_c::getMsg1stType() { return gabi::call<u32>(0x0224D8E0, this); }
__attribute__((weak)) u32 daNpc_Ji1_c::getMsg2ndType() { return gabi::call<u32>(0x0224D718, this); }
__attribute__((weak)) u32 daNpc_Ji1_c::getMsg() { return gabi::call<u32>(0x0224DA20, this); }
__attribute__((weak)) u16 daNpc_Ji1_c::next_msgStatus(be<u32>* a0) { return gabi::call<u16>(0x0224DA74, this, a0); }
__attribute__((weak)) BOOL daNpc_Ji1_c::talkAction(void* a0) { return gabi::call<BOOL>(0x0225268C, this, a0); }
__attribute__((weak)) BOOL daNpc_Ji1_c::speakAction(void* a0) { return gabi::call<BOOL>(0x02252FD0, this, a0); }
__attribute__((weak)) BOOL daNpc_Ji1_c::speakBadAction(void* a0) { return gabi::call<BOOL>(0x02254A64, this, a0); }
__attribute__((weak)) void* daNpc_Ji1_c::initPosObject(void* a0, void* a1) { return gabi::call<void*>(0x022542A0, a0, a1); }
__attribute__((weak)) void daNpc_Ji1_c::initPos(int a0) { gabi::call(0x02254390, this, a0); }
__attribute__((weak)) void daNpc_Ji1_c::createItem() { gabi::call(0x02258574, this); }
__attribute__((weak)) void daNpc_Ji1_c::set_mtx() { gabi::call(0x0224AEF0, this); }
__attribute__((weak)) s32 daNpc_Ji1_c::getEventActionNo(int a0) { return gabi::call<s32>(0x022513D8, this, a0); }
__attribute__((weak)) BOOL daNpc_Ji1_c::eventAction(void* a0) { return gabi::call<BOOL>(0x022596CC, this, a0); }
__attribute__((weak)) u32 daNpc_Ji1_c::evn_init_pos_init(int a0) { return gabi::call<u32>(0x02258508, this, a0); }
__attribute__((weak)) u32 daNpc_Ji1_c::evn_setAnm_init(int a0) { return gabi::call<u32>(0x022582E4, this, a0); }
__attribute__((weak)) u32 daNpc_Ji1_c::evn_talk_init(int a0) { return gabi::call<u32>(0x022583A4, this, a0); }
__attribute__((weak)) u32 daNpc_Ji1_c::evn_talk() { return gabi::call<u32>(0x02258CB0, this); }
__attribute__((weak)) u32 daNpc_Ji1_c::evn_continue_talk_init(int a0) { return gabi::call<u32>(0x02258BE8, this, a0); }
__attribute__((weak)) u32 daNpc_Ji1_c::evn_continue_talk() { return gabi::call<u32>(0x02259178, this); }
__attribute__((weak)) u32 daNpc_Ji1_c::evn_setAngle_init(int a0) { return gabi::call<u32>(0x02258C4C, this, a0); }
__attribute__((weak)) u32 daNpc_Ji1_c::evn_sound_proc_init(int a0) { return gabi::call<u32>(0x02258794, this, a0); }
__attribute__((weak)) u32 daNpc_Ji1_c::evn_head_swing_init(int a0) { return gabi::call<u32>(0x02258894, this, a0); }
__attribute__((weak)) u32 daNpc_Ji1_c::evn_harpoon_proc_init(int a0) { return gabi::call<u32>(0x0225896C, this, a0); }
__attribute__((weak)) u32 daNpc_Ji1_c::evn_RollAtControl_init(int a0) { return gabi::call<u32>(0x02258A14, this, a0); }
__attribute__((weak)) u32 daNpc_Ji1_c::evn_RollAtControl() { return gabi::call<u32>(0x02258E6C, this); }
__attribute__((weak)) u32 daNpc_Ji1_c::evn_game_mode_init(int a0) { return gabi::call<u32>(0x02258A78, this, a0); }
__attribute__((weak)) u32 daNpc_Ji1_c::evn_turn_to_player() { return gabi::call<u32>(0x0225911C, this); }
__attribute__((weak)) u32 daNpc_Ji1_c::evn_hide_init(int a0) { return gabi::call<u32>(0x02258B48, this, a0); }
__attribute__((weak)) void daNpc_Ji1_c::AnimeControlToWait() { gabi::call(0x022595EC, this); }
__attribute__((weak)) u32 daNpc_Ji1_c::privateCut() { return gabi::call<u32>(0x02259334, this); }
__attribute__((weak)) u32 daNpc_Ji1_c::setParticle(int a0, f32 a1, f32 a2) { return gabi::call<u32>(0x0224FBAC, this, a0, a1, a2); }
__attribute__((weak)) void daNpc_Ji1_c::dtParticle() { gabi::call(0x0224FB98, this); }
__attribute__((weak)) u32 daNpc_Ji1_c::setParticleAT(int a0, f32 a1, f32 a2) { return gabi::call<u32>(0x0225B720, this, a0, a1, a2); }
__attribute__((weak)) void daNpc_Ji1_c::dtParticleAT() { gabi::call(0x0225AEB4, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::startspeakAction(void* a0) { return gabi::call<BOOL>(0x02253880, this, a0); }
__attribute__((weak)) BOOL daNpc_Ji1_c::endspeakAction(void* a0) { return gabi::call<BOOL>(0x02253EE8, this, a0); }
__attribute__((weak)) BOOL daNpc_Ji1_c::reiAction(void* a0) { return gabi::call<BOOL>(0x02254434, this, a0); }
__attribute__((weak)) BOOL daNpc_Ji1_c::plmoveAction(void* a0) { return gabi::call<BOOL>(0x02257C9C, this, a0); }
__attribute__((weak)) BOOL daNpc_Ji1_c::teachMove(f32 a0) { return gabi::call<BOOL>(0x0224EF14, this, a0); }
__attribute__((weak)) BOOL daNpc_Ji1_c::teachSpRollCutMove(f32 a0) { return gabi::call<BOOL>(0x0224F368, this, a0); }
__attribute__((weak)) f32 daNpc_Ji1_c::calcCoCorrectValue() { return gabi::call<f32>(0x0224F528, this); }
__attribute__((weak)) f32 daNpc_Ji1_c::calcBgCorrectValue() { return gabi::call<f32>(0x0224F59C, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::MoveToPlayer(f32 a0, u8 a1) { return gabi::call<BOOL>(0x0224F628, this, a0, a1); }
__attribute__((weak)) void daNpc_Ji1_c::teachSubActionAttackInit() { gabi::call(0x0225589C, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::teachSubActionAttack() { return gabi::call<BOOL>(0x02255654, this); }
__attribute__((weak)) void daNpc_Ji1_c::teachSubActionJumpInit() { gabi::call(0x022552E4, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::teachSubActionJump() { return gabi::call<BOOL>(0x022554D0, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::teachAction(void* a0) { return gabi::call<BOOL>(0x02255908, this, a0); }
__attribute__((weak)) BOOL daNpc_Ji1_c::teachSPRollCutAction(void* a0) { return gabi::call<BOOL>(0x02256F1C, this, a0); }
__attribute__((weak)) void daNpc_Ji1_c::battleGameSetTimer() { gabi::call(0x02259A6C, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::battleMove(f32 a0) { return gabi::call<BOOL>(0x02259EC4, this, a0); }
__attribute__((weak)) void daNpc_Ji1_c::battleSubActionWaitInit() { gabi::call(0x0225A2C8, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::battleSubActionWait() { return gabi::call<BOOL>(0x0225B344, this); }
__attribute__((weak)) void daNpc_Ji1_c::battleSubActionNockBackInit(int a0) { gabi::call(0x02248E0C, this, a0); }
__attribute__((weak)) BOOL daNpc_Ji1_c::battleSubActionNockBack() { return gabi::call<BOOL>(0x0225C208, this); }
__attribute__((weak)) void daNpc_Ji1_c::battleSubActionAttackInit() { gabi::call(0x0225B2B8, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::battleSubActionAttack() { return gabi::call<BOOL>(0x0225BF44, this); }
__attribute__((weak)) void daNpc_Ji1_c::battleSubActionTateAttackInit() { gabi::call(0x0225B22C, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::battleSubActionTateAttack() { return gabi::call<BOOL>(0x0225B85C, this); }
__attribute__((weak)) void daNpc_Ji1_c::battleSubActionYokoAttackInit() { gabi::call(0x0225B1A0, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::battleSubActionYokoAttack() { return gabi::call<BOOL>(0x0225BBA4, this); }
__attribute__((weak)) void daNpc_Ji1_c::battleSubActionJumpInit() { gabi::call(0x0225A2F4, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::battleSubActionJump() { return gabi::call<BOOL>(0x0225C358, this); }
__attribute__((weak)) void daNpc_Ji1_c::battleSubActionDamageInit() { gabi::call(0x0225A384, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::battleSubActionDamage() { return gabi::call<BOOL>(0x0225CA04, this); }
__attribute__((weak)) void daNpc_Ji1_c::battleSubActionJpGuardInit() { gabi::call(0x0225A4A4, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::battleSubActionJpGuard() { return gabi::call<BOOL>(0x0225C660, this); }
__attribute__((weak)) void daNpc_Ji1_c::battleSubActionGuardInit() { gabi::call(0x0225A424, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::battleSubActionGuard() { return gabi::call<BOOL>(0x0225C7EC, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::battleAtSet() { return gabi::call<BOOL>(0x0225B4AC, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::battleGuardCheck() { return gabi::call<BOOL>(0x0225A534, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::battleAction(void* a0) { return gabi::call<BOOL>(0x0225AEC8, this, a0); }
__attribute__((weak)) BOOL daNpc_Ji1_c::checkCutType(int a0, int a1) { return gabi::call<BOOL>(0x02255100, this, a0, a1); }
__attribute__((weak)) void daNpc_Ji1_c::setAnimFromMsgNo(u32 a0) { gabi::call(0x02251A8C, this, a0); }
__attribute__((weak)) BOOL daNpc_Ji1_c::setAnm(int a0, f32 a1, int a2) { return gabi::call<BOOL>(0x0224DFF0, this, a0, a1, a2); }
__attribute__((weak)) cPhs_State daNpc_Ji1_c::_create() { return gabi::call<cPhs_State>(0x0224D280, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::CreateHeap() { return gabi::call<BOOL>(0x02249884, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::CreateInit() { return gabi::call<BOOL>(0x0224C650, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::_delete() { return gabi::call<BOOL>(0x0224C550, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::_execute() { return gabi::call<BOOL>(0x0224B268, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::_draw() { return gabi::call<BOOL>(0x02249638, this); }
__attribute__((weak)) BOOL daNpc_Ji1_c::chkAttention(cXyz* a0, s16 a1) { return gabi::call<BOOL>(0x0224AA24, this, a0, a1); }
__attribute__((weak)) BOOL daNpc_Ji1_c::lookBack() { return gabi::call<BOOL>(0x0224ABA4, this); }
__attribute__((weak)) void daNpc_Ji1_c::setHitParticle(cXyz* a0, u32 a1) { gabi::call(0x02255358, this, a0, a1); }
__attribute__((weak)) void daNpc_Ji1_c::setGuardParticle() { gabi::call(0x02248D3C, this); }
__attribute__((weak)) void daNpc_Ji1_c::BackSlideInit() { gabi::call(0x0224DE14, this); }
__attribute__((weak)) void daNpc_Ji1_c::BackSlide(f32 a0, f32 a1) { gabi::call(0x022525B4, this, a0, a1); }
__attribute__((weak)) void daNpc_Ji1_c::harpoonRelease(cXyz* a0) { gabi::call(0x0224DE3C, this, a0); }
__attribute__((weak)) void daNpc_Ji1_c::harpoonMove() { gabi::call(0x0224A2B8, this); }
__attribute__((weak)) s16 daNpc_Ji1_XyCheckCB(void* a0, int a1) { return gabi::call<s16>(0x02248C64, a0, a1); }
__attribute__((weak)) void daJi1_CoHitCallback(fopAc_ac_c* a0, dCcD_GObjInf* a1, fopAc_ac_c* a2, dCcD_GObjInf* a3) { gabi::call(0x02248CA4, a0, a1, a2, a3); }
__attribute__((weak)) void daJi1_TgHitCallback(fopAc_ac_c* a0, dCcD_GObjInf* a1, fopAc_ac_c* a2, dCcD_GObjInf* a3) { gabi::call(0x02248CF0, a0, a1, a2, a3); }
__attribute__((weak)) void daJi1_AtHitCallback(fopAc_ac_c* a0, dCcD_GObjInf* a1, fopAc_ac_c* a2, dCcD_GObjInf* a3) { gabi::call(0x02248EA0, a0, a1, a2, a3); }
__attribute__((weak)) BOOL daNpc_Ji1_plRoomOutCheck() { return gabi::call<BOOL>(0x02248F58); }
__attribute__((weak)) u32 playerCutAtCheck() { return gabi::call<u32>(0x02249058); }
__attribute__((weak)) BOOL nodeCallBack1(J3DNode* a0, int a1) { return gabi::call<BOOL>(0x02249084, a0, a1); }
__attribute__((weak)) BOOL nodeCallBack2(J3DNode* a0, int a1) { return gabi::call<BOOL>(0x02249224, a0, a1); }
__attribute__((weak)) BOOL nodeCallBack3(J3DNode* a0, int a1) { return gabi::call<BOOL>(0x022494C8, a0, a1); }
