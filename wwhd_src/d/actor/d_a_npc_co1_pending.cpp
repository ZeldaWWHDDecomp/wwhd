/* d_a_npc_co1: member functions of the translation unit not yet decompiled in the part being built,
 * as plain guest calls (so the decompiled parts can call each other naturally). Weak: a decompiled
 * definition in a part replaces it. */
#include "d/actor/d_a_npc_co1.h"

__attribute__((weak)) void daNpc_Co1_c::nodeCo1Control(J3DNode* a0, J3DModel* a1) { gabi::call(0x022271C0, this, a0, a1); }
__attribute__((weak)) J3DModelData* daNpc_Co1_c::create_Anm() { return gabi::call<J3DModelData*>(0x022273A8, this); }
__attribute__((weak)) u32 daNpc_Co1_c::btpNum_toResID(int a0) { return gabi::call<u32>(0x022275E0, this, a0); }
__attribute__((weak)) BOOL daNpc_Co1_c::setBtp(u32 a0, int a1) { return gabi::call<BOOL>(0x022275F4, this, a0, a1); }
__attribute__((weak)) u32 daNpc_Co1_c::iniTexPttrnAnm(u32 a0) { return gabi::call<u32>(0x022276E0, this, a0); }
__attribute__((weak)) J3DModelData* daNpc_Co1_c::create_prl_Anm() { return gabi::call<J3DModelData*>(0x022276EC, this); }
__attribute__((weak)) BOOL daNpc_Co1_c::setBtk(u32 a0) { return gabi::call<BOOL>(0x02227884, this, a0); }
__attribute__((weak)) bool daNpc_Co1_c::create_itm_Mdl() { return gabi::call<bool>(0x02227964, this); }
__attribute__((weak)) BOOL daNpc_Co1_c::CreateHeap() { return gabi::call<BOOL>(0x02227A04, this); }
__attribute__((weak)) bool daNpc_Co1_c::charDecide(int a0) { return gabi::call<bool>(0x02227C34, this, a0); }
__attribute__((weak)) BOOL daNpc_Co1_c::set_action(ProcFunc_l* a0, void* a1) { return gabi::call<BOOL>(0x02227C48, this, a0, a1); }
__attribute__((weak)) bool daNpc_Co1_c::init_CO1_0() { return gabi::call<bool>(0x02227D74, this); }
__attribute__((weak)) void daNpc_Co1_c::plyTexPttrnAnm() { gabi::call(0x02227DFC, this); }
__attribute__((weak)) void daNpc_Co1_c::setAttention(u32 a0) { gabi::call(0x02227F1C, this, a0); }
__attribute__((weak)) void daNpc_Co1_c::setMtx(u32 a0) { gabi::call(0x02228014, this, a0); }
__attribute__((weak)) bool daNpc_Co1_c::createInit() { return gabi::call<bool>(0x0222843C, this); }
__attribute__((weak)) cPhs_State daNpc_Co1_c::_create() { return gabi::call<cPhs_State>(0x02228598, this); }
__attribute__((weak)) BOOL daNpc_Co1_c::_delete() { return gabi::call<BOOL>(0x022286C4, this); }
__attribute__((weak)) void daNpc_Co1_c::checkOrder() { gabi::call(0x02228728, this); }
__attribute__((weak)) u8 daNpc_Co1_c::demo() { return gabi::call<u8>(0x02228848, this); }
__attribute__((weak)) s32 daNpc_Co1_c::isEventEntry() { return gabi::call<s32>(0x022289FC, this); }
__attribute__((weak)) u32 daNpc_Co1_c::setAnm_tex(s8 a0) { return gabi::call<u32>(0x02228A3C, this, a0); }
__attribute__((weak)) u32 daNpc_Co1_c::anmNum_toResID(int a0) { return gabi::call<u32>(0x02228A5C, this, a0); }
__attribute__((weak)) u32 daNpc_Co1_c::anmNum_toResID_prl(int a0) { return gabi::call<u32>(0x02228A70, this, a0); }
__attribute__((weak)) BOOL daNpc_Co1_c::setAnm_anm(anm_prm_c* a0) { return gabi::call<BOOL>(0x02228A84, this, a0); }
__attribute__((weak)) bool daNpc_Co1_c::setAnm() { return gabi::call<bool>(0x02228B70, this); }
__attribute__((weak)) void daNpc_Co1_c::setStt(s8 a0) { gabi::call(0x02228BF4, this, a0); }
__attribute__((weak)) void daNpc_Co1_c::endEvent() { gabi::call(0x02228C38, this); }
__attribute__((weak)) void daNpc_Co1_c::setAnm_NUM(int a0, int a1) { gabi::call(0x02228C78, this, a0, a1); }
__attribute__((weak)) void daNpc_Co1_c::eInit_MDR_() { gabi::call(0x02228CE4, this); }
__attribute__((weak)) void daNpc_Co1_c::eInit_RED_LTR_() { gabi::call(0x02228D20, this); }
__attribute__((weak)) void daNpc_Co1_c::event_actionInit(int a0) { gabi::call(0x02228D2C, this, a0); }
__attribute__((weak)) u32 daNpc_Co1_c::eMove_MDR_() { return gabi::call<u32>(0x02228DDC, this); }
__attribute__((weak)) u32 daNpc_Co1_c::eMove_RED_LTR_() { return gabi::call<u32>(0x02228E34, this); }
__attribute__((weak)) u32 daNpc_Co1_c::event_action() { return gabi::call<u32>(0x02228EB8, this); }
__attribute__((weak)) void daNpc_Co1_c::privateCut(int a0) { gabi::call(0x02228EFC, this, a0); }
__attribute__((weak)) void daNpc_Co1_c::lookBack() { gabi::call(0x02228FD0, this); }
__attribute__((weak)) void daNpc_Co1_c::event_proc(int a0) { gabi::call(0x02229210, this, a0); }
__attribute__((weak)) void daNpc_Co1_c::eventOrder() { gabi::call(0x02229378, this); }
__attribute__((weak)) void daNpc_Co1_c::setCollision_SP_() { gabi::call(0x022293E8, this); }
__attribute__((weak)) BOOL daNpc_Co1_c::_execute() { return gabi::call<BOOL>(0x02229558, this); }
__attribute__((weak)) BOOL daNpc_Co1_c::_draw() { return gabi::call<BOOL>(0x02229738, this); }
__attribute__((weak)) void daNpc_Co1_c::set_target(int a0) { gabi::call(0x02229904, this, a0); }
__attribute__((weak)) void daNpc_Co1_c::setAnm_ATR(int a0) { gabi::call(0x02229A38, this, a0); }
__attribute__((weak)) void daNpc_Co1_c::chg_anmAtr(u8 a0) { gabi::call(0x02229AA8, this, a0); }
__attribute__((weak)) void daNpc_Co1_c::control_anmAtr() { gabi::call(0x02229B74, this); }
__attribute__((weak)) void daNpc_Co1_c::anmAtr(u16 a0) { gabi::call(0x02229C90, this, a0); }
__attribute__((weak)) bool daNpc_Co1_c::chk_talk() { return gabi::call<bool>(0x02229D58, this); }
__attribute__((weak)) u8 daNpc_Co1_c::chk_partsNotMove() { return gabi::call<u8>(0x02229DD8, this); }
__attribute__((weak)) u16 daNpc_Co1_c::next_msgStatus(be<u32>* a0) { return gabi::call<u16>(0x02229E18, this, a0); }
__attribute__((weak)) u32 daNpc_Co1_c::getMsg_CO1_0() { return gabi::call<u32>(0x02229F00, this); }
__attribute__((weak)) u32 daNpc_Co1_c::getMsg() { return gabi::call<u32>(0x02229FEC, this); }
__attribute__((weak)) u8 daNpc_Co1_c::chkAttention() { return gabi::call<u8>(0x0222A024, this); }
__attribute__((weak)) BOOL daNpc_Co1_c::wait_1() { return gabi::call<BOOL>(0x0222A0AC, this); }
__attribute__((weak)) BOOL daNpc_Co1_c::wait_2() { return gabi::call<BOOL>(0x0222A130, this); }
__attribute__((weak)) BOOL daNpc_Co1_c::wakeup() { return gabi::call<BOOL>(0x0222A1DC, this); }
__attribute__((weak)) BOOL daNpc_Co1_c::talk_1() { return gabi::call<BOOL>(0x0222A218, this); }
__attribute__((weak)) BOOL daNpc_Co1_c::toru_1() { return gabi::call<BOOL>(0x0222A450, this); }
__attribute__((weak)) BOOL daNpc_Co1_c::read_1() { return gabi::call<BOOL>(0x0222A4C4, this); }
__attribute__((weak)) BOOL daNpc_Co1_c::modoru() { return gabi::call<BOOL>(0x0222A500, this); }
__attribute__((weak)) BOOL daNpc_Co1_c::wait_action1(void* a0) { return gabi::call<BOOL>(0x0222A568, this, a0); }
