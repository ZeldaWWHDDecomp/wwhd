/* d_a_npc_bmsw: member functions not yet decompiled, as plain guest calls (weak: a decompiled
 * definition replaces it). */
#include "d/actor/d_a_npc_bmsw.h"

__attribute__((weak)) BOOL SwMail_c::MailCreateInit(cXyz* a0, cXyz* a1) { return gabi::call<BOOL>(0x0220D884, this, a0, a1); }
__attribute__((weak)) u8 SwMail_c::getNextNo(u8 a0) { return gabi::call<u8>(0x0220D57C, a0); }
__attribute__((weak)) void SwMail_c::init() { gabi::call(0x0220D50C, this); }
__attribute__((weak)) void SwMail_c::set_mtx() { gabi::call(0x0220D430, this); }
__attribute__((weak)) void SwMail_c::set_mtx_throw() { gabi::call(0x0220F2A0, this); }
__attribute__((weak)) void SwMail_c::DummyInit() { gabi::call(0x0220D7AC, this); }
__attribute__((weak)) void SwMail_c::Dummy() { gabi::call(0x0220F3C4, this); }
__attribute__((weak)) void SwMail_c::AppearInit() { gabi::call(0x0220F3C8, this); }
__attribute__((weak)) void SwMail_c::Appear() { gabi::call(0x0220F4C0, this); }
__attribute__((weak)) void SwMail_c::WaitInit() { gabi::call(0x0220F4A0, this); }
__attribute__((weak)) void SwMail_c::Wait() { gabi::call(0x0220F62C, this); }
__attribute__((weak)) void SwMail_c::ThrowInit(cXyz* a0, u8 a1) { gabi::call(0x0220F744, this, a0, a1); }
__attribute__((weak)) void SwMail_c::Throw() { gabi::call(0x0220F7C4, this); }
__attribute__((weak)) void SwMail_c::EndInit() { gabi::call(0x0220F784, this); }
__attribute__((weak)) void SwMail_c::End() { gabi::call(0x0220FA84, this); }
__attribute__((weak)) void SwMail_c::SeDelete() { gabi::call(0x0220EB68, this); }
__attribute__((weak)) void SwMail_c::move() { gabi::call(0x0220F380, this); }
__attribute__((weak)) void SwMail_c::draw(dKy_tevstr_c* a0) { gabi::call(0x0220EFF4, this, a0); }
__attribute__((weak)) void SwCam_c::Move() { gabi::call(0x0220ED04, this); }
__attribute__((weak)) BOOL daNpc_Bmsw_c::initTexPatternAnm(u32 a0) { return gabi::call<BOOL>(0x0220D320, this, a0); }
__attribute__((weak)) void daNpc_Bmsw_c::playTexPatternAnm() { gabi::call(0x0220EC00, this); }
__attribute__((weak)) void daNpc_Bmsw_c::setAnm(s8 a0) { gabi::call(0x0220FC70, this, a0); }
__attribute__((weak)) u8 daNpc_Bmsw_c::chkAttention(cXyz* a0, s16 a1) { return gabi::call<u8>(0x0220FD3C, this, a0, a1); }
__attribute__((weak)) void daNpc_Bmsw_c::eventOrder() { gabi::call(0x0220EE0C, this); }
__attribute__((weak)) void daNpc_Bmsw_c::checkOrder() { gabi::call(0x0220ECC4, this); }
__attribute__((weak)) u16 daNpc_Bmsw_c::next_msgStatus(be<u32>* a0) { return gabi::call<u16>(0x0220FE68, this, a0); }
__attribute__((weak)) u32 daNpc_Bmsw_c::getMsg() { return gabi::call<u32>(0x022102CC, this); }
__attribute__((weak)) void daNpc_Bmsw_c::anmAtr(u16 a0) { gabi::call(0x022103D4, this, a0); }
__attribute__((weak)) BOOL daNpc_Bmsw_c::CreateInit() { return gabi::call<BOOL>(0x0220E6E8, this); }
__attribute__((weak)) void daNpc_Bmsw_c::set_mtx() { gabi::call(0x0220E340, this); }
__attribute__((weak)) void daNpc_Bmsw_c::setAttention() { gabi::call(0x0221047C, this); }
__attribute__((weak)) void daNpc_Bmsw_c::lookBack() { gabi::call(0x022104A4, this); }
__attribute__((weak)) void daNpc_Bmsw_c::wait01() { gabi::call(0x0221068C, this); }
__attribute__((weak)) void daNpc_Bmsw_c::talk01() { gabi::call(0x02210804, this); }
__attribute__((weak)) BOOL daNpc_Bmsw_c::wait_action(void* a0) { return gabi::call<BOOL>(0x02210AC0, this, a0); }
__attribute__((weak)) BOOL daNpc_Bmsw_c::checkNextMailThrowOK() { return gabi::call<BOOL>(0x02210DCC, this); }
__attribute__((weak)) void daNpc_Bmsw_c::setGameGetRupee(s16 a0) { gabi::call(0x02210E74, this, a0); }
__attribute__((weak)) void daNpc_Bmsw_c::TimerCountDown() { gabi::call(0x02210D20, this); }
__attribute__((weak)) BOOL daNpc_Bmsw_c::shiwake_game_action(void* a0) { return gabi::call<BOOL>(0x02210F60, this, a0); }
__attribute__((weak)) BOOL daNpc_Bmsw_c::_draw() { return gabi::call<BOOL>(0x0220F06C, this); }
__attribute__((weak)) BOOL daNpc_Bmsw_c::_execute() { return gabi::call<BOOL>(0x0220EE44, this); }
__attribute__((weak)) BOOL daNpc_Bmsw_c::_delete() { return gabi::call<BOOL>(0x0220EB70, this); }
__attribute__((weak)) cPhs_State daNpc_Bmsw_c::_create() { return gabi::call<cPhs_State>(0x0220E9D8, this); }
__attribute__((weak)) BOOL daNpc_Bmsw_c::CreateHeap() { return gabi::call<BOOL>(0x0220D98C, this); }
