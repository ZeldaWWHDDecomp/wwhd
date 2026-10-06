/* d_a_npc_ds1: member functions of the translation unit as plain guest calls (so the decompiled
 * parts can call each other naturally). Weak: a decompiled definition in a part replaces it.
 */
#include "d/actor/d_a_npc_ds1.h"

#define W __attribute__((weak))
W BOOL daNpc_Ds1_c::wait_action(void* a) { return gabi::call<BOOL>(0x022311F4, this, a); }
W BOOL daNpc_Ds1_c::getdemo_action(void* a) { return gabi::call<BOOL>(0x022312F8, this, a); }
W BOOL daNpc_Ds1_c::dummy_action(void* a) { return gabi::call<BOOL>(0x0223164C, this, a); }
W BOOL daNpc_Ds1_c::event_action(void* a) { return gabi::call<BOOL>(0x022328E4, this, a); }
W s16 daNpc_Ds1_c::XyEventCB(int i) { return gabi::call<s16>(0x0222D3FC, this, i); }
W BOOL daNpc_Ds1_c::initTexPatternAnm(u8 m) { return gabi::call<BOOL>(0x0222D97C, this, m); }
W BOOL daNpc_Ds1_c::CreateHeap() { return gabi::call<BOOL>(0x0222DA90, this); }
W void daNpc_Ds1_c::RoomEffectSet() { gabi::call(0x0222E1A4, this); }
W BOOL daNpc_Ds1_c::CreateInit() { return gabi::call<BOOL>(0x0222E598, this); }
W cPhs_State daNpc_Ds1_c::_create() { return gabi::call<cPhs_State>(0x0222E8F0, this); }
W void daNpc_Ds1_c::RoomEffectDelete() { gabi::call(0x0222EA18, this); }
W BOOL daNpc_Ds1_c::_delete() { return gabi::call<BOOL>(0x0222EA54, this); }
W void daNpc_Ds1_c::playTexPatternAnm() { gabi::call(0x0222EB1C, this); }
W void daNpc_Ds1_c::talkInit() { gabi::call(0x0222EBE0, this); }
W void daNpc_Ds1_c::checkOrder() { gabi::call(0x0222EBEC, this); }
W void daNpc_Ds1_c::eventOrder() { gabi::call(0x0222F050, this); }
W void daNpc_Ds1_c::setCollision() { gabi::call(0x0222F100, this); }
W BOOL daNpc_Ds1_c::_execute() { return gabi::call<BOOL>(0x0222F1BC, this); }
W BOOL daNpc_Ds1_c::_draw() { return gabi::call<BOOL>(0x0222F5D0, this); }
W void daNpc_Ds1_c::setAnm(s8 i, f32 m) { gabi::call(0x0222F9C0, this, i, m); }
W u32 daNpc_Ds1_c::setTexAnm(s8 i) { return gabi::call<u32>(0x0222FA9C, this, i); }
W void daNpc_Ds1_c::setAnmFromMsgTag() { gabi::call(0x0222FAC4, this); }
W bool daNpc_Ds1_c::chkAttention(cXyz* p, s16 a) { return gabi::call<bool>(0x0222FDBC, this, p, a); }
W u16 daNpc_Ds1_c::next_msgStatus(be<u32>* p, be<u32>* q) { return gabi::call<u16>(0x0222FECC, this, p, q); }
W u32 daNpc_Ds1_c::getMsg() { return gabi::call<u32>(0x02230364, this); }
W u16 daNpc_Ds1_c::normal_talk() { return gabi::call<u16>(0x02230420, this); }
W u16 daNpc_Ds1_c::shop_talk() { return gabi::call<u16>(0x02230784, this); }
W u16 daNpc_Ds1_c::talk() { return gabi::call<u16>(0x022308E0, this); }
W void daNpc_Ds1_c::setAttention(u32 b) { gabi::call(0x02230ADC, this, b); }
W void daNpc_Ds1_c::lookBack() { gabi::call(0x02230B24, this); }
W BOOL daNpc_Ds1_c::wait01() { return gabi::call<BOOL>(0x02230F2C, this); }
W BOOL daNpc_Ds1_c::talk01() { return gabi::call<BOOL>(0x02230F8C, this); }
W BOOL daNpc_Ds1_c::evn_talk_init(int i) { return gabi::call<BOOL>(0x02231668, this, i); }
W BOOL daNpc_Ds1_c::evn_continue_talk_init(int i) { return gabi::call<BOOL>(0x02231710, this, i); }
W BOOL daNpc_Ds1_c::evn_ItemModel_init(int i) { return gabi::call<BOOL>(0x02231774, this, i); }
W BOOL daNpc_Ds1_c::evn_head_swing_init(int i) { return gabi::call<BOOL>(0x022317DC, this, i); }
W BOOL daNpc_Ds1_c::evn_setAnm_init(int i) { return gabi::call<BOOL>(0x0223188C, this, i); }
W BOOL daNpc_Ds1_c::evn_move_pos_init(int i) { return gabi::call<BOOL>(0x02231D0C, this, i); }
W BOOL daNpc_Ds1_c::evn_init_pos_init(int i) { return gabi::call<BOOL>(0x02231DC8, this, i); }
W BOOL daNpc_Ds1_c::evn_jnt_lock_init(int i) { return gabi::call<BOOL>(0x02231EBC, this, i); }
W BOOL daNpc_Ds1_c::evn_player_hide_init(int i) { return gabi::call<BOOL>(0x02231F6C, this, i); }
W BOOL daNpc_Ds1_c::evn_talk() { return gabi::call<BOOL>(0x02232000, this); }
W BOOL daNpc_Ds1_c::evn_Anm() { return gabi::call<BOOL>(0x022322D0, this); }
W BOOL daNpc_Ds1_c::evn_move_pos() { return gabi::call<BOOL>(0x022325D0, this); }
W BOOL daNpc_Ds1_c::privateCut() { return gabi::call<BOOL>(0x02232698, this); }
