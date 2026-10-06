/* d_a_npc_bms1: member functions not defined in the part being built, as plain guest calls (so
 * the parts can call each other naturally). Weak: a decompiled definition replaces it.
 */
#include "d/actor/d_a_npc_bms1.h"

__attribute__((weak)) BOOL daNpc_Bms1_c::initTexPatternAnm(u32 a0) { return gabi::call<BOOL>(0x02208BA8, this, a0); }
__attribute__((weak)) u32 daNpc_Bms1_c::setTexAnm(s8 a0) { return gabi::call<u32>(0x02208CB4, this, a0); }
__attribute__((weak)) BOOL daNpc_Bms1_c::CreateHeap() { return gabi::call<BOOL>(0x02208CDC, this); }
__attribute__((weak)) void daNpc_Bms1_c::set_mtx() { gabi::call(0x02209438, this); }
__attribute__((weak)) BOOL daNpc_Bms1_c::CreateInit() { return gabi::call<BOOL>(0x022098D0, this); }
__attribute__((weak)) cPhs_State daNpc_Bms1_c::_create() { return gabi::call<cPhs_State>(0x02209DB4, this); }
__attribute__((weak)) BOOL daNpc_Bms1_c::_delete() { return gabi::call<BOOL>(0x0220A0CC, this); }
__attribute__((weak)) void daNpc_Bms1_c::playTexPatternAnm() { gabi::call(0x0220A160, this); }
__attribute__((weak)) void daNpc_Bms1_c::demo_end_init() { gabi::call(0x0220A224, this); }
__attribute__((weak)) BOOL daNpc_Bms1_c::demo_move() { return gabi::call<BOOL>(0x0220A230, this); }
__attribute__((weak)) void daNpc_Bms1_c::talkInit() { gabi::call(0x0220A360, this); }
__attribute__((weak)) void daNpc_Bms1_c::checkOrder() { gabi::call(0x0220A36C, this); }
__attribute__((weak)) void daNpc_Bms1_c::eventOrder() { gabi::call(0x0220A680, this); }
__attribute__((weak)) void daNpc_Bms1_c::setCollision() { gabi::call(0x0220A6F0, this); }
__attribute__((weak)) BOOL daNpc_Bms1_c::_execute() { return gabi::call<BOOL>(0x0220A7AC, this); }
__attribute__((weak)) BOOL daNpc_Bms1_c::_draw() { return gabi::call<BOOL>(0x0220A954, this); }
__attribute__((weak)) void daNpc_Bms1_c::setAnm(s8 a0, f32 a1) { gabi::call(0x0220AB80, this, a0, a1); }
__attribute__((weak)) void daNpc_Bms1_c::setAnmFromMsgTag() { gabi::call(0x0220ABEC, this); }
__attribute__((weak)) BOOL daNpc_Bms1_c::chkAttention(cXyz* a0, s16 a1) { return gabi::call<BOOL>(0x0220AEFC, this, a0, a1); }
__attribute__((weak)) u32 daNpc_Bms1_c::next_msgStatus(be<u32>* a0, be<u32>* a1) { return gabi::call<u32>(0x0220AFB4, this, a0, a1); }
__attribute__((weak)) u32 daNpc_Bms1_c::getMsg() { return gabi::call<u32>(0x0220B354, this); }
__attribute__((weak)) u16 daNpc_Bms1_c::normal_talk() { return gabi::call<u16>(0x0220B3B0, this); }
__attribute__((weak)) u16 daNpc_Bms1_c::shop_talk() { return gabi::call<u16>(0x0220B554, this); }
__attribute__((weak)) u16 daNpc_Bms1_c::talk() { return gabi::call<u16>(0x0220B6C4, this); }
__attribute__((weak)) void daNpc_Bms1_c::setAttention(bool a0) { gabi::call(0x0220B88C, this, a0); }
__attribute__((weak)) void daNpc_Bms1_c::lookBack() { gabi::call(0x0220B8C8, this); }
__attribute__((weak)) BOOL daNpc_Bms1_c::checkPlayerLanding() { return gabi::call<BOOL>(0x0220BB98, this); }
__attribute__((weak)) BOOL daNpc_Bms1_c::wait01() { return gabi::call<BOOL>(0x0220BC54, this); }
__attribute__((weak)) BOOL daNpc_Bms1_c::talk01() { return gabi::call<BOOL>(0x0220BCD8, this); }
__attribute__((weak)) BOOL daNpc_Bms1_c::wait_action(void* a0) { return gabi::call<BOOL>(0x0220BDCC, this, a0); }
__attribute__((weak)) BOOL daNpc_Bms1_c::getdemo_action(void* a0) { return gabi::call<BOOL>(0x0220BF90, this, a0); }
__attribute__((weak)) BOOL daNpc_Bms1_c::evn_talk_init(int a0) { return gabi::call<BOOL>(0x0220C1C0, this, a0); }
__attribute__((weak)) BOOL daNpc_Bms1_c::evn_continue_talk_init(int a0) { return gabi::call<BOOL>(0x0220C2A4, this, a0); }
__attribute__((weak)) BOOL daNpc_Bms1_c::evn_viblation_init(int a0) { return gabi::call<BOOL>(0x0220C308, this, a0); }
__attribute__((weak)) BOOL daNpc_Bms1_c::evn_head_swing_init(int a0) { return gabi::call<BOOL>(0x0220C35C, this, a0); }
__attribute__((weak)) BOOL daNpc_Bms1_c::evn_talk() { return gabi::call<BOOL>(0x0220C394, this); }
__attribute__((weak)) BOOL daNpc_Bms1_c::privateCut() { return gabi::call<BOOL>(0x0220C660, this); }
__attribute__((weak)) BOOL daNpc_Bms1_c::event_action(void* a0) { return gabi::call<BOOL>(0x0220C7CC, this, a0); }
