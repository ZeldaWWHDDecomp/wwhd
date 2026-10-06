/* d_a_npc_zl1: every method of daNpc_Zl1_c as a plain guest call, weak: the decompiled
 * definition in one of the d_a_npc_zl1*.cpp files replaces it when linked into the same unit.
 */
#include "d/actor/d_a_npc_zl1.h"

__attribute__((weak)) void daNpc_Zl1_c::_nodeCB_Head(J3DNode* a0, J3DModel* a1) { gabi::call(0x023020F8, this, a0, a1); }
__attribute__((weak)) void daNpc_Zl1_c::_nodeCB_BackBone(J3DNode* a0, J3DModel* a1) { gabi::call(0x023022C0, this, a0, a1); }
__attribute__((weak)) BOOL daNpc_Zl1_c::set_startPos(int a0) { return gabi::call<BOOL>(0x02303D14, this, a0); }
__attribute__((weak)) bool daNpc_Zl1_c::init_ZL1_0() { return gabi::call<bool>(0x023039E4, this); }
__attribute__((weak)) bool daNpc_Zl1_c::init_ZL1_1() { return gabi::call<bool>(0x02303A7C, this); }
__attribute__((weak)) bool daNpc_Zl1_c::init_ZL1_2() { return gabi::call<bool>(0x02303B14, this); }
__attribute__((weak)) bool daNpc_Zl1_c::init_ZL1_3() { return gabi::call<bool>(0x02303BBC, this); }
__attribute__((weak)) bool daNpc_Zl1_c::init_ZL1_4() { return gabi::call<bool>(0x02303CBC, this); }
__attribute__((weak)) bool daNpc_Zl1_c::init_ZL1_5() { return gabi::call<bool>(0x02303E98, this); }
__attribute__((weak)) bool daNpc_Zl1_c::createInit() { return gabi::call<bool>(0x0230503C, this); }
__attribute__((weak)) void daNpc_Zl1_c::play_animation() { gabi::call(0x023045E0, this); }
__attribute__((weak)) bool daNpc_Zl1_c::swoon_OnShip() { return gabi::call<bool>(0x0230473C, this); }
__attribute__((weak)) void daNpc_Zl1_c::setMtx(u32 a0) { gabi::call(0x02304C98, this, a0); }
__attribute__((weak)) u32 daNpc_Zl1_c::bckResID(int a0) { return gabi::call<u32>(0x0230600C, this, a0); }
__attribute__((weak)) u32 daNpc_Zl1_c::btpResID(int a0) { return gabi::call<u32>(0x02302484, this, a0); }
__attribute__((weak)) u32 daNpc_Zl1_c::btkResID(int a0) { return gabi::call<u32>(0x02302834, this, a0); }
__attribute__((weak)) u32 daNpc_Zl1_c::setBtp(s32 a0, u32 a1) { return gabi::call<u32>(0x023024F4, this, a0, a1); }
__attribute__((weak)) void daNpc_Zl1_c::setMat() { gabi::call(0x023028A4, this); }
__attribute__((weak)) u32 daNpc_Zl1_c::setBtk(s32 a0, u32 a1) { return gabi::call<u32>(0x02302984, this, a0, a1); }
__attribute__((weak)) u32 daNpc_Zl1_c::init_texPttrnAnm(s8 a0, u32 a1) { return gabi::call<u32>(0x02302A90, this, a0, a1); }
__attribute__((weak)) void daNpc_Zl1_c::play_btp_anm() { gabi::call(0x02304268, this); }
__attribute__((weak)) void daNpc_Zl1_c::eye_ctrl() { gabi::call(0x02304310, this); }
__attribute__((weak)) void daNpc_Zl1_c::play_btk_anm() { gabi::call(0x02304548, this); }
__attribute__((weak)) void daNpc_Zl1_c::setAnm_anm(anm_prm_c* a0) { gabi::call(0x0230607C, this, a0); }
__attribute__((weak)) void daNpc_Zl1_c::setAnm_NUM(int a0, int a1) { gabi::call(0x023066D0, this, a0, a1); }
__attribute__((weak)) void daNpc_Zl1_c::setAnm() { gabi::call(0x02306114, this); }
__attribute__((weak)) void daNpc_Zl1_c::chngAnmAtr(u8 a0) { gabi::call(0x0230883C, this, a0); }
__attribute__((weak)) void daNpc_Zl1_c::setAnm_ATR() { gabi::call(0x02306458, this); }
__attribute__((weak)) void daNpc_Zl1_c::anmAtr(u16 a0) { gabi::call(0x02308AC0, this, a0); }
__attribute__((weak)) u16 daNpc_Zl1_c::next_msgStatus(be<u32>* a0) { return gabi::call<u16>(0x02308B90, this, a0); }
__attribute__((weak)) u32 daNpc_Zl1_c::getMsg_ZL1_2() { return gabi::call<u32>(0x02308C4C, this); }
__attribute__((weak)) u32 daNpc_Zl1_c::getMsg_ZL1_4() { return gabi::call<u32>(0x02308CBC, this); }
__attribute__((weak)) u32 daNpc_Zl1_c::getMsg() { return gabi::call<u32>(0x02308D08, this); }
__attribute__((weak)) void daNpc_Zl1_c::eventOrder() { gabi::call(0x02307988, this); }
__attribute__((weak)) void daNpc_Zl1_c::checkOrder() { gabi::call(0x023055A4, this); }
__attribute__((weak)) u8 daNpc_Zl1_c::chk_talk() { return gabi::call<u8>(0x02308D64, this); }
__attribute__((weak)) u8 daNpc_Zl1_c::chk_parts_notMov() { return gabi::call<u8>(0x02308DFC, this); }
__attribute__((weak)) u8 daNpc_Zl1_c::partner_search_sub(u32 a0) { return gabi::call<u8>(0x0230545C, this, a0); }
__attribute__((weak)) void daNpc_Zl1_c::partner_search() { gabi::call(0x02305508, this); }
__attribute__((weak)) void daNpc_Zl1_c::lookBack() { gabi::call(0x023075B0, this); }
__attribute__((weak)) u8 daNpc_Zl1_c::chkAttention() { return gabi::call<u8>(0x02308E3C, this); }
__attribute__((weak)) void daNpc_Zl1_c::setAttention(u32 a0) { gabi::call(0x02304C44, this, a0); }
__attribute__((weak)) u8 daNpc_Zl1_c::decideType(int a0) { return gabi::call<u8>(0x023037F4, this, a0); }
__attribute__((weak)) f32 daNpc_Zl1_c::get_prmFloat(be<f32>* a0, f32 a1) { return gabi::call<f32>(0x0230683C, this, a0, a1); }
__attribute__((weak)) void daNpc_Zl1_c::set_LightPos(cXyz* a0) { gabi::call(0x02306A40, this, a0); }
__attribute__((weak)) void daNpc_Zl1_c::init_Light() { gabi::call(0x023069E8, this); }
__attribute__((weak)) void daNpc_Zl1_c::incEnvironment() { gabi::call(0x023079F8, this); }
__attribute__((weak)) void daNpc_Zl1_c::decEnvironment() { gabi::call(0x02307A30, this); }
__attribute__((weak)) void daNpc_Zl1_c::darkProc() { gabi::call(0x02307A58, this); }
__attribute__((weak)) void daNpc_Zl1_c::cut_init_LOK_PLYER(int a0) { gabi::call(0x023062C4, this, a0); }
__attribute__((weak)) void daNpc_Zl1_c::cut_init_LOK_PARTNER(int a0) { gabi::call(0x023063A0, this, a0); }
__attribute__((weak)) void daNpc_Zl1_c::cut_init_CHG_ANM_ATR(int a0) { gabi::call(0x023064C0, this, a0); }
__attribute__((weak)) void daNpc_Zl1_c::cut_init_PLYER_TRN_PARTNER(int a0) { gabi::call(0x02306528, this, a0); }
__attribute__((weak)) void daNpc_Zl1_c::cut_init_PLYER_TRN_TETRA(int a0) { gabi::call(0x023065D4, this, a0); }
__attribute__((weak)) void daNpc_Zl1_c::cut_init_MAJYU_START(int a0) { gabi::call(0x02306624, this, a0); }
__attribute__((weak)) void daNpc_Zl1_c::cut_init_OKIRU(int a0) { gabi::call(0x023066B4, this, a0); }
__attribute__((weak)) u32 daNpc_Zl1_c::cut_move_OKIRU() { return gabi::call<u32>(0x02306D34, this); }
__attribute__((weak)) void daNpc_Zl1_c::cut_init_OKIRU_2(int a0) { gabi::call(0x02306740, this, a0); }
__attribute__((weak)) u32 daNpc_Zl1_c::cut_move_OKIRU_2() { return gabi::call<u32>(0x02306D48, this); }
__attribute__((weak)) void daNpc_Zl1_c::cut_init_DRW_ONOFF(int a0) { gabi::call(0x0230674C, this, a0); }
__attribute__((weak)) void daNpc_Zl1_c::cut_init_PLYER_DRW_ONOFF(int a0) { gabi::call(0x023067B4, this, a0); }
__attribute__((weak)) void daNpc_Zl1_c::cut_init_JMP_OFF(int a0) { gabi::call(0x0230684C, this, a0); }
__attribute__((weak)) u32 daNpc_Zl1_c::cut_move_JMP_OFF() { return gabi::call<u32>(0x02306ED4, this); }
__attribute__((weak)) void daNpc_Zl1_c::cut_init_OMAMORI_ONOFF(int a0) { gabi::call(0x02306BD8, this, a0); }
__attribute__((weak)) void daNpc_Zl1_c::cut_init_SURPRISED(int a0) { gabi::call(0x02306CDC, this, a0); }
__attribute__((weak)) void daNpc_Zl1_c::privateCut(int a0) { gabi::call(0x02306F30, this, a0); }
__attribute__((weak)) void daNpc_Zl1_c::endEvent() { gabi::call(0x02306280, this); }
__attribute__((weak)) s32 daNpc_Zl1_c::isEventEntry() { return gabi::call<s32>(0x02305FCC, this); }
__attribute__((weak)) void daNpc_Zl1_c::event_proc(int a0) { gabi::call(0x023071C4, this, a0); }
__attribute__((weak)) BOOL daNpc_Zl1_c::set_action(ProcFunc_l* a0, void* a1) { return gabi::call<BOOL>(0x023038B8, this, a0, a1); }
__attribute__((weak)) void daNpc_Zl1_c::setStt(s8 a0) { gabi::call(0x02306184, this, a0); }
__attribute__((weak)) u8 daNpc_Zl1_c::chk_areaIN(f32 a0, f32 a1, s16 a2, cXyz* a3) { return gabi::call<u8>(0x02309210, this, a0, a1, a2, a3); }
__attribute__((weak)) void daNpc_Zl1_c::setWaterRipple() { gabi::call(0x02304824, this); }
__attribute__((weak)) void daNpc_Zl1_c::setWaterSplash() { gabi::call(0x023048F4, this); }
__attribute__((weak)) void daNpc_Zl1_c::set_simpleLand(u32 a0) { gabi::call(0x0230498C, this, a0); }
__attribute__((weak)) void daNpc_Zl1_c::setEff() { gabi::call(0x02304A6C, this); }
__attribute__((weak)) BOOL daNpc_Zl1_c::setFrontWallType() { return gabi::call<BOOL>(0x02308EC4, this); }
__attribute__((weak)) u8 daNpc_Zl1_c::move_jmp() { return gabi::call<u8>(0x02306DB8, this); }
__attribute__((weak)) void daNpc_Zl1_c::kyoroPos(cXyz* a0, int a1) { gabi::call(0x0230741C, this, a0, a1); }
__attribute__((weak)) BOOL daNpc_Zl1_c::kyorokyoro() { return gabi::call<BOOL>(0x023074E4, this); }
__attribute__((weak)) BOOL daNpc_Zl1_c::wait_1() { return gabi::call<BOOL>(0x02309394, this); }
__attribute__((weak)) BOOL daNpc_Zl1_c::talk_1() { return gabi::call<BOOL>(0x02309478, this); }
__attribute__((weak)) BOOL daNpc_Zl1_c::demo_1() { return gabi::call<BOOL>(0x023095E4, this); }
__attribute__((weak)) BOOL daNpc_Zl1_c::demo_2() { return gabi::call<BOOL>(0x02309640, this); }
__attribute__((weak)) BOOL daNpc_Zl1_c::demo_3() { return gabi::call<BOOL>(0x023096CC, this); }
__attribute__((weak)) BOOL daNpc_Zl1_c::demo_4() { return gabi::call<BOOL>(0x0230976C, this); }
__attribute__((weak)) BOOL daNpc_Zl1_c::optn_1() { return gabi::call<BOOL>(0x02309794, this); }
__attribute__((weak)) BOOL daNpc_Zl1_c::optn_2() { return gabi::call<BOOL>(0x023099F4, this); }
__attribute__((weak)) BOOL daNpc_Zl1_c::optn_3() { return gabi::call<BOOL>(0x02309CC4, this); }
__attribute__((weak)) BOOL daNpc_Zl1_c::wait_action1(void* a0) { return gabi::call<BOOL>(0x02309CEC, this, a0); }
__attribute__((weak)) BOOL daNpc_Zl1_c::demo_action1(void* a0) { return gabi::call<BOOL>(0x02309E10, this, a0); }
__attribute__((weak)) BOOL daNpc_Zl1_c::demo_action2(void* a0) { return gabi::call<BOOL>(0x02309EB8, this, a0); }
__attribute__((weak)) BOOL daNpc_Zl1_c::optn_action1(void* a0) { return gabi::call<BOOL>(0x02309FC8, this, a0); }
__attribute__((weak)) u8 daNpc_Zl1_c::demo() { return gabi::call<u8>(0x02305778, this); }
__attribute__((weak)) BOOL daNpc_Zl1_c::_draw() { return gabi::call<BOOL>(0x02308090, this); }
__attribute__((weak)) BOOL daNpc_Zl1_c::_execute() { return gabi::call<BOOL>(0x02307D18, this); }
__attribute__((weak)) BOOL daNpc_Zl1_c::_delete() { return gabi::call<BOOL>(0x023053EC, this); }
__attribute__((weak)) cPhs_State daNpc_Zl1_c::_create() { return gabi::call<cPhs_State>(0x023052B4, this); }
__attribute__((weak)) BOOL daNpc_Zl1_c::bodyCreateHeap() { return gabi::call<BOOL>(0x02302B08, this); }
__attribute__((weak)) BOOL daNpc_Zl1_c::itemCreateHeap() { return gabi::call<BOOL>(0x02303400, this); }
__attribute__((weak)) BOOL daNpc_Zl1_c::CreateHeap() { return gabi::call<BOOL>(0x023034DC, this); }
__attribute__((weak)) bool daNpc_Zl1_c::init_ZL1_6() { return gabi::call<bool>(0x02304148, this); }
__attribute__((weak)) bool daNpc_Zl1_c::init_ZL1_7() { return gabi::call<bool>(0x023041A0, this); }
__attribute__((weak)) void daNpc_Zl1_c::setEyeCtrl() { gabi::call(0x023041F8, this); }
__attribute__((weak)) void daNpc_Zl1_c::clrEyeCtrl() { gabi::call(0x02304230, this); }
