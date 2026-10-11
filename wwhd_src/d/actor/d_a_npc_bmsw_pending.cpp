/* Weak guest fallbacks for linking Koboli independently. Strong definitions in the actor source
 * replace these methods. */
#include "d/actor/d_a_npc_bmsw.h"

__attribute__((weak)) BOOL SwMail_c::MailCreateInit(cXyz* a0, cXyz* a1) {
    return Bmsw_guest_SwMail_c_MailCreateInit(this, a0, a1);
}
__attribute__((weak)) u8 SwMail_c::getNextNo(u8 a0) { return Bmsw_getNextNo(a0); }
__attribute__((weak)) void SwMail_c::init() { Bmsw_guest_SwMail_c_init(this); }
__attribute__((weak)) void SwMail_c::set_mtx() { Bmsw_guest_SwMail_c_set_mtx(this); }
__attribute__((weak)) void SwMail_c::set_mtx_throw() { Bmsw_guest_SwMail_c_set_mtx_throw(this); }
__attribute__((weak)) void SwMail_c::DummyInit() { Bmsw_guest_SwMail_c_DummyInit(this); }
__attribute__((weak)) void SwMail_c::Dummy() { Bmsw_guest_SwMail_c_Dummy(this); }
__attribute__((weak)) void SwMail_c::AppearInit() { Bmsw_guest_SwMail_c_AppearInit(this); }
__attribute__((weak)) void SwMail_c::Appear() { Bmsw_guest_SwMail_c_Appear(this); }
__attribute__((weak)) void SwMail_c::WaitInit() { Bmsw_guest_SwMail_c_WaitInit(this); }
__attribute__((weak)) void SwMail_c::Wait() { Bmsw_guest_SwMail_c_Wait(this); }
__attribute__((weak)) void SwMail_c::ThrowInit(cXyz* a0, u8 a1) {
    Bmsw_guest_SwMail_c_ThrowInit(this, a0, a1);
}
__attribute__((weak)) void SwMail_c::Throw() { Bmsw_guest_SwMail_c_Throw(this); }
__attribute__((weak)) void SwMail_c::EndInit() { Bmsw_guest_SwMail_c_EndInit(this); }
__attribute__((weak)) void SwMail_c::End() { Bmsw_guest_SwMail_c_End(this); }
__attribute__((weak)) void SwMail_c::SeDelete() { Bmsw_guest_SwMail_c_SeDelete(this); }
__attribute__((weak)) void SwMail_c::move() { Bmsw_guest_SwMail_c_move(this); }
__attribute__((weak)) void SwMail_c::draw(dKy_tevstr_c* a0) { Bmsw_guest_SwMail_c_draw(this, a0); }
__attribute__((weak)) void SwCam_c::Move() { Bmsw_guest_SwCam_c_Move(this); }
__attribute__((weak)) BOOL daNpc_Bmsw_c::initTexPatternAnm(u32 a0) {
    return Bmsw_guest_daNpc_Bmsw_c_initTexPatternAnm(this, a0);
}
__attribute__((weak)) void daNpc_Bmsw_c::playTexPatternAnm() {
    Bmsw_guest_daNpc_Bmsw_c_playTexPatternAnm(this);
}
__attribute__((weak)) void daNpc_Bmsw_c::setAnm(s8 a0) { Bmsw_guest_daNpc_Bmsw_c_setAnm(this, a0); }
__attribute__((weak)) u8 daNpc_Bmsw_c::chkAttention(cXyz* a0, s16 a1) {
    return Bmsw_guest_daNpc_Bmsw_c_chkAttention(this, a0, a1);
}
__attribute__((weak)) void daNpc_Bmsw_c::eventOrder() { Bmsw_guest_daNpc_Bmsw_c_eventOrder(this); }
__attribute__((weak)) void daNpc_Bmsw_c::checkOrder() { Bmsw_guest_daNpc_Bmsw_c_checkOrder(this); }
__attribute__((weak)) u16 daNpc_Bmsw_c::next_msgStatus(be<u32>* a0) {
    return Bmsw_guest_daNpc_Bmsw_c_next_msgStatus(this, a0);
}
__attribute__((weak)) u32 daNpc_Bmsw_c::getMsg() { return Bmsw_guest_daNpc_Bmsw_c_getMsg(this); }
__attribute__((weak)) void daNpc_Bmsw_c::anmAtr(u16 a0) {
    Bmsw_guest_daNpc_Bmsw_c_anmAtr(this, a0);
}
__attribute__((weak)) BOOL daNpc_Bmsw_c::CreateInit() {
    return Bmsw_guest_daNpc_Bmsw_c_CreateInit(this);
}
__attribute__((weak)) void daNpc_Bmsw_c::set_mtx() { Bmsw_guest_daNpc_Bmsw_c_set_mtx(this); }
__attribute__((weak)) void daNpc_Bmsw_c::setAttention() {
    Bmsw_guest_daNpc_Bmsw_c_setAttention(this);
}
__attribute__((weak)) void daNpc_Bmsw_c::lookBack() { Bmsw_guest_daNpc_Bmsw_c_lookBack(this); }
__attribute__((weak)) void daNpc_Bmsw_c::wait01() { Bmsw_guest_daNpc_Bmsw_c_wait01(this); }
__attribute__((weak)) void daNpc_Bmsw_c::talk01() { Bmsw_guest_daNpc_Bmsw_c_talk01(this); }
__attribute__((weak)) BOOL daNpc_Bmsw_c::wait_action(void* a0) {
    return Bmsw_guest_daNpc_Bmsw_c_wait_action(this, a0);
}
__attribute__((weak)) BOOL daNpc_Bmsw_c::checkNextMailThrowOK() {
    return Bmsw_guest_daNpc_Bmsw_c_checkNextMailThrowOK(this);
}
__attribute__((weak)) void daNpc_Bmsw_c::setGameGetRupee(s16 a0) {
    Bmsw_guest_daNpc_Bmsw_c_setGameGetRupee(this, a0);
}
__attribute__((weak)) void daNpc_Bmsw_c::TimerCountDown() {
    Bmsw_guest_daNpc_Bmsw_c_TimerCountDown(this);
}
__attribute__((weak)) BOOL daNpc_Bmsw_c::shiwake_game_action(void* a0) {
    return Bmsw_guest_daNpc_Bmsw_c_shiwake_game_action(this, a0);
}
__attribute__((weak)) BOOL daNpc_Bmsw_c::_draw() { return Bmsw_guest_daNpc_Bmsw_c__draw(this); }
__attribute__((weak)) BOOL daNpc_Bmsw_c::_execute() {
    return Bmsw_guest_daNpc_Bmsw_c__execute(this);
}
__attribute__((weak)) BOOL daNpc_Bmsw_c::_delete() { return Bmsw_guest_daNpc_Bmsw_c__delete(this); }
__attribute__((weak)) cPhs_State daNpc_Bmsw_c::_create() {
    return Bmsw_guest_daNpc_Bmsw_c__create(this);
}
__attribute__((weak)) BOOL daNpc_Bmsw_c::CreateHeap() {
    return Bmsw_guest_daNpc_Bmsw_c_CreateHeap(this);
}
