/* d_a_npc_kk1: member functions of the translation unit not yet decompiled in the part being built,
 * as plain guest calls (so the decompiled parts can call each other naturally). Weak: a decompiled
 * definition in a part replaces it. */
#define SAFESTRING_VTBL 0x1001C250
#include "d/actor/d_a_npc_kk1.h"

__attribute__((weak)) void daNpc_Kk1_c::_nodeCB_Head(J3DNode* a0, J3DModel* a1) { gabi::call(0x0226AAF8, this, a0, a1); }
__attribute__((weak)) void daNpc_Kk1_c::_nodeCB_BackBone(J3DNode* a0, J3DModel* a1) { gabi::call(0x0226ACB8, this, a0, a1); }
__attribute__((weak)) BOOL daNpc_Kk1_c::setBtp(s8 a0, u32 a1) { return gabi::call<BOOL>(0x0226AE30, this, a0, a1); }
__attribute__((weak)) BOOL daNpc_Kk1_c::init_texPttrnAnm(s8 a0, u32 a1) { return gabi::call<BOOL>(0x0226AF1C, this, a0, a1); }
__attribute__((weak)) bool daNpc_Kk1_c::bodyCreateHeap() { return gabi::call<bool>(0x0226AF20, this); }
__attribute__((weak)) bool daNpc_Kk1_c::effcCreateHeap() { return gabi::call<bool>(0x0226B1E0, this); }
__attribute__((weak)) bool daNpc_Kk1_c::CreateHeap() { return gabi::call<bool>(0x0226B3E8, this); }
__attribute__((weak)) bool daNpc_Kk1_c::decideType(int a0) { return gabi::call<bool>(0x0226B550, this, a0); }
__attribute__((weak)) BOOL daNpc_Kk1_c::set_action(ProcFunc_l* a0, void* a1) { return gabi::call<BOOL>(0x0226B704, this, a0, a1); }
__attribute__((weak)) bool daNpc_Kk1_c::init_KK1_0() { return gabi::call<bool>(0x0226B830, this); }
__attribute__((weak)) void daNpc_Kk1_c::play_btp_anm() { gabi::call(0x0226B8E4, this); }
__attribute__((weak)) void daNpc_Kk1_c::setBikon(cXyz* a0) { gabi::call(0x0226B984, this, a0); }
__attribute__((weak)) void daNpc_Kk1_c::play_eff_anm() { gabi::call(0x0226BA48, this); }
__attribute__((weak)) void daNpc_Kk1_c::play_animation() { gabi::call(0x0226BB5C, this); }
__attribute__((weak)) void daNpc_Kk1_c::flwAse() { gabi::call(0x0226BCD4, this); }
__attribute__((weak)) void daNpc_Kk1_c::setAttention(u32 a0) { gabi::call(0x0226BD78, this, a0); }
__attribute__((weak)) void daNpc_Kk1_c::setMtx(u32 a0) { gabi::call(0x0226BDCC, this, a0); }
__attribute__((weak)) bool daNpc_Kk1_c::createInit() { return gabi::call<bool>(0x0226BFD0, this); }
__attribute__((weak)) cPhs_State daNpc_Kk1_c::_create() { return gabi::call<cPhs_State>(0x0226C28C, this); }
__attribute__((weak)) void daNpc_Kk1_c::delAse() { gabi::call(0x0226C438, this); }
__attribute__((weak)) BOOL daNpc_Kk1_c::_delete() { return gabi::call<BOOL>(0x0226C464, this); }
__attribute__((weak)) bool daNpc_Kk1_c::partner_search_sub(u32  a0) { return gabi::call<bool>(0x0226C4C4, this, a0); }
__attribute__((weak)) void daNpc_Kk1_c::partner_search() { gabi::call(0x0226C570, this); }
__attribute__((weak)) void daNpc_Kk1_c::setAse() { gabi::call(0x0226C5F0, this); }
__attribute__((weak)) void daNpc_Kk1_c::setAnm_anm(anm_prm_c* a0) { gabi::call(0x0226C660, this, a0); }
__attribute__((weak)) void daNpc_Kk1_c::setAnm_NUM(s32 a0, s32 a1) { gabi::call(0x0226C730, this, a0, a1); }
__attribute__((weak)) bool daNpc_Kk1_c::checkCommandTalk() { return gabi::call<bool>(0x0226C7A0, this); }
__attribute__((weak)) void daNpc_Kk1_c::checkOrder() { gabi::call(0x0226C7DC, this); }
__attribute__((weak)) u8 daNpc_Kk1_c::demo() { return gabi::call<u8>(0x0226C8D8, this); }
__attribute__((weak)) s32 daNpc_Kk1_c::isEventEntry() { return gabi::call<s32>(0x0226CA94, this); }
__attribute__((weak)) void daNpc_Kk1_c::setAnm() { gabi::call(0x0226CAD4, this); }
__attribute__((weak)) void daNpc_Kk1_c::setStt(s8 a0) { gabi::call(0x0226CB44, this, a0); }
__attribute__((weak)) void daNpc_Kk1_c::endEvent() { gabi::call(0x0226CBE8, this); }
__attribute__((weak)) bool daNpc_Kk1_c::cut_init_RUN_START(s32 a0) { return gabi::call<bool>(0x0226CC80, this, a0); }
__attribute__((weak)) void daNpc_Kk1_c::cut_init_RUN(s32 a0) { gabi::call(0x0226CD04, this, a0); }
__attribute__((weak)) void daNpc_Kk1_c::cut_init_CATCH_START(s32 a0) { gabi::call(0x0226CD94, this, a0); }
__attribute__((weak)) void daNpc_Kk1_c::cut_init_CATCH_END(s32 a0) { gabi::call(0x0226CDF0, this, a0); }
__attribute__((weak)) void daNpc_Kk1_c::cut_init_TRN(s32 a0) { gabi::call(0x0226CE6C, this, a0); }
__attribute__((weak)) void daNpc_Kk1_c::cut_init_BYE_START(s32 a0) { gabi::call(0x0226D0A4, this, a0); }
__attribute__((weak)) void daNpc_Kk1_c::cut_init_BYE(s32 a0) { gabi::call(0x0226D0E8, this, a0); }
__attribute__((weak)) void daNpc_Kk1_c::cut_init_BYE_END(s32 a0) { gabi::call(0x0226D1F0, this, a0); }
__attribute__((weak)) void daNpc_Kk1_c::cut_init_OTOBOKE(s32 a0) { gabi::call(0x0226D2B8, this, a0); }
__attribute__((weak)) void daNpc_Kk1_c::cut_init_PLYER_MOV(s32 a0) { gabi::call(0x0226D34C, this, a0); }
__attribute__((weak)) void daNpc_Kk1_c::cut_init_RUNAWAY_START(s32 a0) { gabi::call(0x0226D454, this, a0); }
__attribute__((weak)) void daNpc_Kk1_c::cut_init_RUNAWAY_END(s32 a0) { gabi::call(0x0226D60C, this, a0); }
__attribute__((weak)) void daNpc_Kk1_c::cut_init_BYE_CONTINUE(s32 a0) { gabi::call(0x0226D664, this, a0); }
__attribute__((weak)) bool daNpc_Kk1_c::cut_move_RUN_START() { return gabi::call<bool>(0x0226D6CC, this); }
__attribute__((weak)) bool daNpc_Kk1_c::event_move(bool a0) { return gabi::call<bool>(0x0226D83C, this, a0); }
__attribute__((weak)) s32 daNpc_Kk1_c::cut_move_RUN() { return gabi::call<s32>(0x0226DB64, this); }
__attribute__((weak)) bool daNpc_Kk1_c::cut_move_CATCH_START() { return gabi::call<bool>(0x0226DBB0, this); }
__attribute__((weak)) bool daNpc_Kk1_c::cut_move_TRN() { return gabi::call<bool>(0x0226DBD4, this); }
__attribute__((weak)) bool daNpc_Kk1_c::cut_move_BYE() { return gabi::call<bool>(0x0226DC70, this); }
__attribute__((weak)) bool daNpc_Kk1_c::cut_move_OTOBOKE() { return gabi::call<bool>(0x0226DD7C, this); }
__attribute__((weak)) bool daNpc_Kk1_c::cut_move_RUNAWAY_START() { return gabi::call<bool>(0x0226DDD8, this); }
__attribute__((weak)) bool daNpc_Kk1_c::cut_move_BYE_CONTINUE() { return gabi::call<bool>(0x0226DE88, this); }
__attribute__((weak)) void daNpc_Kk1_c::privateCut(s32 a0) { gabi::call(0x0226DEF0, this, a0); }
__attribute__((weak)) void daNpc_Kk1_c::event_proc(s32 a0) { gabi::call(0x0226E21C, this, a0); }
__attribute__((weak)) void daNpc_Kk1_c::kyoroPos(cXyz* a0, s32 a1) { gabi::call(0x0226E498, this, a0, a1); }
__attribute__((weak)) bool daNpc_Kk1_c::kyorokyoro() { return gabi::call<bool>(0x0226E560, this); }
__attribute__((weak)) void daNpc_Kk1_c::lookBack() { gabi::call(0x0226E604, this); }
__attribute__((weak)) void daNpc_Kk1_c::eventOrder() { gabi::call(0x0226E92C, this); }
__attribute__((weak)) BOOL daNpc_Kk1_c::_execute() { return gabi::call<BOOL>(0x0226E99C, this); }
__attribute__((weak)) BOOL daNpc_Kk1_c::_draw() { return gabi::call<BOOL>(0x0226ECA8, this); }
__attribute__((weak)) void daNpc_Kk1_c::setAnm_ATR() { gabi::call(0x0226EF4C, this); }
__attribute__((weak)) void daNpc_Kk1_c::chngAnmAtr(u8 a0) { gabi::call(0x0226EFB4, this, a0); }
__attribute__((weak)) void daNpc_Kk1_c::ctrlAnmAtr() { gabi::call(0x0226F088, this); }
__attribute__((weak)) void daNpc_Kk1_c::anmAtr(u16 a0) { gabi::call(0x0226F170, this, a0); }
__attribute__((weak)) u16 daNpc_Kk1_c::next_msgStatus(be<u32>* a0) { return gabi::call<u16>(0x0226F230, this, a0); }
__attribute__((weak)) u32 daNpc_Kk1_c::getMsg_KK1_0() { return gabi::call<u32>(0x0226F538, this); }
__attribute__((weak)) u32 daNpc_Kk1_c::getMsg() { return gabi::call<u32>(0x0226F61C, this); }
__attribute__((weak)) bool daNpc_Kk1_c::chk_talk() { return gabi::call<bool>(0x0226F654, this); }
__attribute__((weak)) u8 daNpc_Kk1_c::chk_parts_notMov() { return gabi::call<u8>(0x0226F6EC, this); }
__attribute__((weak)) BOOL daNpc_Kk1_c::chkAttention() { return gabi::call<BOOL>(0x0226F72C, this); }
__attribute__((weak)) void daNpc_Kk1_c::createTama(f32 a0) { gabi::call(0x0226F7B4, this, a0); }
__attribute__((weak)) bool daNpc_Kk1_c::chk_areaIN(f32 a0, cXyz* a1) { return gabi::call<bool>(0x0226F908, this, a0, a1); }
__attribute__((weak)) bool daNpc_Kk1_c::startEvent_check() { return gabi::call<bool>(0x0226FABC, this); }
__attribute__((weak)) BOOL daNpc_Kk1_c::chkHitPlayer() { return gabi::call<BOOL>(0x0226FB70, this); }
__attribute__((weak)) u8 daNpc_Kk1_c::chk_attn() { return gabi::call<u8>(0x0226FC1C, this); }
__attribute__((weak)) s32 daNpc_Kk1_c::wait_1() { return gabi::call<s32>(0x0226FE78, this); }
__attribute__((weak)) s32 daNpc_Kk1_c::walk_1() { return gabi::call<s32>(0x022700E0, this); }
__attribute__((weak)) s32 daNpc_Kk1_c::wait_2() { return gabi::call<s32>(0x022703B4, this); }
__attribute__((weak)) void daNpc_Kk1_c::init_CMT_WAI() { gabi::call(0x0227051C, this); }
__attribute__((weak)) void daNpc_Kk1_c::move_CMT_WAI() { gabi::call(0x02270570, this); }
__attribute__((weak)) void daNpc_Kk1_c::init_CMT_TRN() { gabi::call(0x0227066C, this); }
__attribute__((weak)) void daNpc_Kk1_c::move_CMT_TRN() { gabi::call(0x022706E4, this); }
__attribute__((weak)) void daNpc_Kk1_c::init_CMT_PCK() { gabi::call(0x022708B8, this); }
__attribute__((weak)) void daNpc_Kk1_c::move_CMT_PCK() { gabi::call(0x02270918, this); }
__attribute__((weak)) s32 daNpc_Kk1_c::cmmt_1() { return gabi::call<s32>(0x02270AE0, this); }
__attribute__((weak)) s32 daNpc_Kk1_c::wait_3() { return gabi::call<s32>(0x02270C7C, this); }
__attribute__((weak)) s32 daNpc_Kk1_c::wait_4() { return gabi::call<s32>(0x02270DA8, this); }
__attribute__((weak)) BOOL daNpc_Kk1_c::talk_1() { return gabi::call<BOOL>(0x02270F28, this); }
__attribute__((weak)) BOOL daNpc_Kk1_c::wait_action1(void* a0) { return gabi::call<BOOL>(0x02271084, this, a0); }
__attribute__((weak)) s32 daNpc_Kk1_c::btpResID(int a0) { return gabi::call<s32>(0x0226AE24, this, a0); }
__attribute__((weak)) void daNpc_Kk1_c::set_pthPoint(u8 a0) { gabi::call(0x0226B690, this, a0); }
__attribute__((weak)) s32 daNpc_Kk1_c::bckResID(int a0) { return gabi::call<s32>(0x0226C5DC, this, a0); }
__attribute__((weak)) fopAc_ac_c* daNpc_Kk1_c::searchByID(fpc_ProcID a0, be<s32>* a1) { return gabi::call<fopAc_ac_c*>(0x0226CC2C, this, a0, a1); }
