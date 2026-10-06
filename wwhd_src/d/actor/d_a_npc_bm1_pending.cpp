/* d_a_npc_bm1: member functions of the translation unit not yet decompiled in the part being built,
 * as plain guest calls (so the decompiled parts can call each other naturally). Weak: a decompiled
 * definition in a part replaces it. */
#include "d/actor/d_a_npc_bm1.h"

__attribute__((weak)) void daNpc_Bm1_c::nodeBm1Control(J3DNode* a0, J3DModel* a1) { gabi::call(0x021FADE8, this, a0, a1); }
__attribute__((weak)) bool daNpc_Bm1_c::chk_appCnd() { return gabi::call<bool>(0x021FC5DC, this); }
__attribute__((weak)) bool daNpc_Bm1_c::init_PST_0() { return gabi::call<bool>(0x021FC884, this); }
__attribute__((weak)) bool daNpc_Bm1_c::init_PST_1() { return gabi::call<bool>(0x021FC910, this); }
__attribute__((weak)) bool daNpc_Bm1_c::init_PST_2() { return gabi::call<bool>(0x021FC950, this); }
__attribute__((weak)) bool daNpc_Bm1_c::init_PST_3() { return gabi::call<bool>(0x021FC9CC, this); }
__attribute__((weak)) bool daNpc_Bm1_c::init_PST_4() { return gabi::call<bool>(0x021FCA68, this); }
__attribute__((weak)) bool daNpc_Bm1_c::init_BMC_1() { return gabi::call<bool>(0x021FCC14, this); }
__attribute__((weak)) bool daNpc_Bm1_c::init_BMC_2() { return gabi::call<bool>(0x021FCCB0, this); }
__attribute__((weak)) bool daNpc_Bm1_c::init_BMC_3() { return gabi::call<bool>(0x021FCD4C, this); }
__attribute__((weak)) bool daNpc_Bm1_c::init_BMD_1() { return gabi::call<bool>(0x021FCDB0, this); }
__attribute__((weak)) bool daNpc_Bm1_c::createInit() { return gabi::call<bool>(0x021FDB44, this); }
__attribute__((weak)) void daNpc_Bm1_c::setMtx(u32 a0) { gabi::call(0x021FD41C, this, a0); }
__attribute__((weak)) u32 daNpc_Bm1_c::btpNum_toResID(int a0) { return gabi::call<u32>(0x021FB4C4, this, a0); }
__attribute__((weak)) bool daNpc_Bm1_c::setBtp(u32 a0, int a1) { return gabi::call<bool>(0x021FB5B8, this, a0, a1); }
__attribute__((weak)) void daNpc_Bm1_c::plyTexPttrnAnm() { gabi::call(0x021FCE10, this); }
__attribute__((weak)) u32 daNpc_Bm1_c::setAnm_tex(s8 a0) { return gabi::call<u32>(0x021FE774, this, a0); }
__attribute__((weak)) BOOL daNpc_Bm1_c::setAnm_anm(anm_prm_c* a0) { return gabi::call<BOOL>(0x021FEAB4, this, a0); }
__attribute__((weak)) void daNpc_Bm1_c::setAnm_NUM(int a0, int a1) { gabi::call(0x021FECF4, this, a0, a1); }
__attribute__((weak)) bool daNpc_Bm1_c::setAnm() { return gabi::call<bool>(0x02201258, this); }
__attribute__((weak)) void daNpc_Bm1_c::setPlaySpd(f32 a0) { gabi::call(0x02200AB4, this, a0); }
__attribute__((weak)) void daNpc_Bm1_c::chg_anmAtr(u8 a0) { gabi::call(0x0220134C, this, a0); }
__attribute__((weak)) void daNpc_Bm1_c::control_anmAtr() { gabi::call(0x0220141C, this); }
__attribute__((weak)) void daNpc_Bm1_c::setAnm_ATR(int a0) { gabi::call(0x022012DC, this, a0); }
__attribute__((weak)) void daNpc_Bm1_c::anmAtr(u16 a0) { gabi::call(0x02201460, this, a0); }
__attribute__((weak)) void daNpc_Bm1_c::eventOrder() { gabi::call(0x02200CD0, this); }
__attribute__((weak)) void daNpc_Bm1_c::checkOrder() { gabi::call(0x021FE408, this); }
__attribute__((weak)) u8 daNpc_Bm1_c::chk_manzai() { return gabi::call<u8>(0x02201558, this); }
__attribute__((weak)) bool daNpc_Bm1_c::chk_talk() { return gabi::call<bool>(0x0220163C, this); }
__attribute__((weak)) u8 daNpc_Bm1_c::chk_partsNotMove() { return gabi::call<u8>(0x022016BC, this); }
__attribute__((weak)) void daNpc_Bm1_c::lookBack() { gabi::call(0x021FFFFC, this); }
__attribute__((weak)) u16 daNpc_Bm1_c::next_msgStatus(be<u32>* a0) { return gabi::call<u16>(0x022016FC, this, a0); }
__attribute__((weak)) s32 daNpc_Bm1_c::getBitMask() { return gabi::call<s32>(0x02201C70, this); }
__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_PST_1() { return gabi::call<u32>(0x02201CC4, this); }
__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_PST_3() { return gabi::call<u32>(0x02201D04, this); }
__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_SKT_0() { return gabi::call<u32>(0x02201D44, this); }
__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_KKT_0() { return gabi::call<u32>(0x02201E1C, this); }
__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_BMB_2() { return gabi::call<u32>(0x022020EC, this); }
__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_BMC_0() { return gabi::call<u32>(0x02202280, this); }
__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_BMC_2() { return gabi::call<u32>(0x022022B4, this); }
__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_BMD_1() { return gabi::call<u32>(0x0220254C, this); }
__attribute__((weak)) u32 daNpc_Bm1_c::getMsg() { return gabi::call<u32>(0x0220269C, this); }
__attribute__((weak)) u8 daNpc_Bm1_c::chkAttention() { return gabi::call<u8>(0x022027B0, this); }
__attribute__((weak)) bool daNpc_Bm1_c::partner_srch_sub(u32 a0) { return gabi::call<bool>(0x021FE218, this, a0); }
__attribute__((weak)) void daNpc_Bm1_c::partner_srch() { gabi::call(0x021FE2F8, this); }
__attribute__((weak)) u32 daNpc_Bm1_c::bm_movPass(u32 a0) { return gabi::call<u32>(0x0220057C, this, a0); }
__attribute__((weak)) void daNpc_Bm1_c::bm_setFlyAnm() { gabi::call(0x021FEEB0, this); }
__attribute__((weak)) void daNpc_Bm1_c::bm_clcFlySpd() { gabi::call(0x02200420, this); }
__attribute__((weak)) void daNpc_Bm1_c::bm_clcMovSpd() { gabi::call(0x02200A40, this); }
__attribute__((weak)) bool daNpc_Bm1_c::bm_flyMove() { return gabi::call<bool>(0x02200714, this); }
__attribute__((weak)) void daNpc_Bm1_c::bm_nMove() { gabi::call(0x02200AF8, this); }
__attribute__((weak)) void daNpc_Bm1_c::setPrtcl_Flyaway() { gabi::call(0x021FCF54, this); }
__attribute__((weak)) void daNpc_Bm1_c::delPrtcl_Flyaway() { gabi::call(0x021FCEE4, this); } /* matcher: 021FD0A0 (that is delPrtcl_Land0) */
/* part A: functions identified in 021FA778 .. 021FFFFC (unnamed by the matcher) */
__attribute__((weak)) void daNpc_Bm1_c::nodeWngControl(J3DNode* a0, J3DModel* a1) { gabi::call(0x021FA998, this, a0, a1); }
__attribute__((weak)) void daNpc_Bm1_c::nodeArmControl(J3DNode* a0, J3DModel* a1) { gabi::call(0x021FABC0, this, a0, a1); }
__attribute__((weak)) u32 daNpc_Bm1_c::iniTexPttrnAnm(u32 a0) { return gabi::call<u32>(0x021FB78C, this, a0); }
__attribute__((weak)) bool daNpc_Bm1_c::init_SKT_0() { return gabi::call<bool>(0x021FCB00, this); }
__attribute__((weak)) u32 daNpc_Bm1_c::init_KKT_0() { return gabi::call<u32>(0x021FCB4C, this); }
__attribute__((weak)) bool daNpc_Bm1_c::init_BMB_0() { return gabi::call<bool>(0x021FCB50, this); }
__attribute__((weak)) u32 daNpc_Bm1_c::init_BMB_1() { return gabi::call<u32>(0x021FCB90, this); }
__attribute__((weak)) bool daNpc_Bm1_c::init_BMB_2() { return gabi::call<bool>(0x021FCB94, this); }
__attribute__((weak)) bool daNpc_Bm1_c::init_BMC_0() { return gabi::call<bool>(0x021FCBD4, this); }
__attribute__((weak)) u32 daNpc_Bm1_c::init_BMD_0() { return gabi::call<u32>(0x021FCDAC, this); }
__attribute__((weak)) void daNpc_Bm1_c::delPrtcl_Land0() { gabi::call(0x021FD0A0, this); }
__attribute__((weak)) void daNpc_Bm1_c::flwPrtcl_Hane0() { gabi::call(0x021FD25C, this); }
__attribute__((weak)) void daNpc_Bm1_c::flwPrtcl_Hane1() { gabi::call(0x021FD30C, this); }
__attribute__((weak)) void daNpc_Bm1_c::setAttention(u32 a0) { gabi::call(0x021FD3BC, this, a0); }
__attribute__((weak)) void daNpc_Bm1_c::delPrtcl_Hane0() { gabi::call(0x021FE0BC, this); }
__attribute__((weak)) void daNpc_Bm1_c::delPrtcl_Hane1() { gabi::call(0x021FE10C, this); }
__attribute__((weak)) u32 daNpc_Bm1_c::eInit_DEL_ACTOR_() { return gabi::call<u32>(0x021FE70C, this); }
__attribute__((weak)) void daNpc_Bm1_c::eInit_INI_EVN_1_() { gabi::call(0x021FE768, this); }
__attribute__((weak)) u32 daNpc_Bm1_c::anmNum_toResID(int a0) { return gabi::call<u32>(0x021FE794, this, a0); }
__attribute__((weak)) u32 daNpc_Bm1_c::wingAnmNum_toResID(int a0) { return gabi::call<u32>(0x021FE808, this, a0); }
__attribute__((weak)) u32 daNpc_Bm1_c::headAnmNum_toResID(int a0) { return gabi::call<u32>(0x021FE87C, this, a0); }
__attribute__((weak)) void daNpc_Bm1_c::eInit_SET_ANM_(be<s32>* a0) { gabi::call(0x021FED60, this, a0); }
__attribute__((weak)) void daNpc_Bm1_c::eInit_setLocFlag(be<s32>* a0) { gabi::call(0x021FF2B8, this, a0); }
__attribute__((weak)) void daNpc_Bm1_c::eInit_setShapeAngleY(be<s32>* a0, s16 a1) { gabi::call(0x021FF2F8, this, a0, a1); }
__attribute__((weak)) bool daNpc_Bm1_c::eMove_FLY_() { return gabi::call<bool>(0x021FFC84, this); }
__attribute__((weak)) u8 daNpc_Bm1_c::eMove_KMA_FLY_() { return gabi::call<u8>(0x021FFCA0, this); }
__attribute__((weak)) u32 daNpc_Bm1_c::eMove_ATTENTION_() { return gabi::call<u32>(0x021FFCA8, this); }
__attribute__((weak)) void daNpc_Bm1_c::setPrtcl_Land0() { gabi::call(0x021FD110, this); }
__attribute__((weak)) void daNpc_Bm1_c::setPrtcl_Hane0() { gabi::call(0x021FE9E0, this); }
__attribute__((weak)) void daNpc_Bm1_c::setPrtcl_Hane1() { gabi::call(0x021FE8EC, this); }
__attribute__((weak)) bool daNpc_Bm1_c::decideType(int a0, int a1) { return gabi::call<bool>(0x021FC36C, this, a0, a1); }
__attribute__((weak)) void daNpc_Bm1_c::eInit_setEvTimer(be<s32>* a0) { gabi::call(0x021FF328, this, a0); }
__attribute__((weak)) cXyz* daNpc_Bm1_c::eInit_calcRelativPos(cXyz* a0, cXyz* a1, be<s32>* a2) { return gabi::call<cXyz*>(0x021FF0CC, this, a0, a1, a2); }
__attribute__((weak)) void daNpc_Bm1_c::eInit_ATTENTION_(be<s32>* a0, be<s32>* a1, be<s32>* a2, cXyz* a3, be<s32>* a4, be<s32>* a5, be<s32>* a6) { gabi::call(0x021FF344, this, a0, a1, a2, a3, a4, a5, a6); }
__attribute__((weak)) void daNpc_Bm1_c::eInit_SET_PLYER_GOL_(be<s32>* a0, cXyz* a1, be<s32>* a2) { gabi::call(0x021FF210, this, a0, a1, a2); }
__attribute__((weak)) f32 daNpc_Bm1_c::eInit_prmFloat(be<f32>* a0, f32 a1) { return gabi::call<f32>(0x021FEEA0, this, a0, a1); }
__attribute__((weak)) void daNpc_Bm1_c::eInit_FLY_(be<s32>* a0, be<f32>* a1, be<f32>* a2, be<f32>* a3, be<f32>* a4) { gabi::call(0x021FEECC, this, a0, a1, a2, a3, a4); }
__attribute__((weak)) void daNpc_Bm1_c::eInit_WLK_(be<s32>* a0, be<f32>* a1, be<f32>* a2, cXyz* a3, be<s32>* a4, be<s32>* a5, be<s32>* a6) { gabi::call(0x021FF5D0, this, a0, a1, a2, a3, a4, a5, a6); }
__attribute__((weak)) void daNpc_Bm1_c::eInit_SET_NXT_PTH_INF_() { gabi::call(0x021FE710, this); }
__attribute__((weak)) void daNpc_Bm1_c::eInit_MOV_PTH_POINT_(be<s32>* a0, be<s32>* a1, be<s32>* a2, be<s32>* a3) { gabi::call(0x021FED7C, this, a0, a1, a2, a3); }
__attribute__((weak)) void daNpc_Bm1_c::event_actionInit(int a0) { gabi::call(0x021FF888, this, a0); }
__attribute__((weak)) bool daNpc_Bm1_c::eMove_WLK_() { return gabi::call<bool>(0x021FFCFC, this); }
__attribute__((weak)) u32 daNpc_Bm1_c::event_action() { return gabi::call<u32>(0x021FFD64, this); }
__attribute__((weak)) void daNpc_Bm1_c::cut_init_360_TRN(int a0) { gabi::call(0x021FFC48, this, a0); }
__attribute__((weak)) bool daNpc_Bm1_c::cut_move_360_TRN() { return gabi::call<bool>(0x021FFDCC, this); }
__attribute__((weak)) void daNpc_Bm1_c::privateCut(int a0) { gabi::call(0x021FFED0, this, a0); }
__attribute__((weak)) void daNpc_Bm1_c::endEvent() { gabi::call(0x021FE6CC, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::isEventEntry() { return gabi::call<BOOL>(0x021FE68C, this); }
__attribute__((weak)) void daNpc_Bm1_c::event_proc(int a0) { gabi::call(0x0220026C, this, a0); }
__attribute__((weak)) BOOL daNpc_Bm1_c::set_action(ProcFunc_l* a0, void* a1) { return gabi::call<BOOL>(0x021FC758, this, a0, a1); }
__attribute__((weak)) void daNpc_Bm1_c::setStt(s8 a0) { gabi::call(0x02202838, this, a0); }
__attribute__((weak)) BOOL daNpc_Bm1_c::d_wait() { return gabi::call<BOOL>(0x02202B48, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::lookup() { return gabi::call<BOOL>(0x02202BA8, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::orooro() { return gabi::call<BOOL>(0x02202BDC, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::wait_1() { return gabi::call<BOOL>(0x02202C2C, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::talk_1() { return gabi::call<BOOL>(0x02202CF0, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::talk_2() { return gabi::call<BOOL>(0x02202FF4, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::manzai() { return gabi::call<BOOL>(0x02203084, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::wait_4() { return gabi::call<BOOL>(0x02203124, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::flyawy() { return gabi::call<BOOL>(0x02203200, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::wait_5() { return gabi::call<BOOL>(0x0220325C, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::h_wait() { return gabi::call<BOOL>(0x02203348, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::wait_7() { return gabi::call<BOOL>(0x02203414, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::wait_3() { return gabi::call<BOOL>(0x02203508, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::wait_8() { return gabi::call<BOOL>(0x02203648, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::wait_2() { return gabi::call<BOOL>(0x022036CC, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::walk_1() { return gabi::call<BOOL>(0x02203734, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::CHKwai() { return gabi::call<BOOL>(0x02203830, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::demo_action1(void* a0) { return gabi::call<BOOL>(0x0220390C, this, a0); }
__attribute__((weak)) BOOL daNpc_Bm1_c::wait_action1(void* a0) { return gabi::call<BOOL>(0x022039E0, this, a0); }
__attribute__((weak)) BOOL daNpc_Bm1_c::wait_action2(void* a0) { return gabi::call<BOOL>(0x02203A9C, this, a0); }
__attribute__((weak)) BOOL daNpc_Bm1_c::wait_action3(void* a0) { return gabi::call<BOOL>(0x02203BAC, this, a0); }
__attribute__((weak)) BOOL daNpc_Bm1_c::wait_action4(void* a0) { return gabi::call<BOOL>(0x02203C44, this, a0); }
__attribute__((weak)) BOOL daNpc_Bm1_c::wait_action5(void* a0) { return gabi::call<BOOL>(0x02203C98, this, a0); }
__attribute__((weak)) BOOL daNpc_Bm1_c::wait_action6(void* a0) { return gabi::call<BOOL>(0x02203D48, this, a0); }
__attribute__((weak)) BOOL daNpc_Bm1_c::wait_action7(void* a0) { return gabi::call<BOOL>(0x02203DF8, this, a0); }
__attribute__((weak)) BOOL daNpc_Bm1_c::wait_action8(void* a0) { return gabi::call<BOOL>(0x02203ED8, this, a0); }
__attribute__((weak)) BOOL daNpc_Bm1_c::wait_action9(void* a0) { return gabi::call<BOOL>(0x02203FAC, this, a0); }
__attribute__((weak)) BOOL daNpc_Bm1_c::wait_actionA(void* a0) { return gabi::call<BOOL>(0x02204044, this, a0); }
__attribute__((weak)) u8 daNpc_Bm1_c::demo() { return gabi::call<u8>(0x021FE4DC, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::_draw() { return gabi::call<BOOL>(0x02200F90, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::_execute() { return gabi::call<BOOL>(0x02200D58, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::_delete() { return gabi::call<BOOL>(0x021FE17C, this); }
__attribute__((weak)) cPhs_State daNpc_Bm1_c::_create() { return gabi::call<cPhs_State>(0x021FDF08, this); }
__attribute__((weak)) J3DModelData* daNpc_Bm1_c::create_Anm() { return gabi::call<J3DModelData*>(0x021FB010, this); }
__attribute__((weak)) J3DModelData* daNpc_Bm1_c::create_hed_Anm() { return gabi::call<J3DModelData*>(0x021FB354, this); }
__attribute__((weak)) J3DModelData* daNpc_Bm1_c::create_wng_Anm() { return gabi::call<J3DModelData*>(0x021FB798, this); }
__attribute__((weak)) J3DModelData* daNpc_Bm1_c::create_arm_Anm() { return gabi::call<J3DModelData*>(0x021FBA3C, this); }
__attribute__((weak)) bool daNpc_Bm1_c::create_itm_Mdl() { return gabi::call<bool>(0x021FBD80, this); }
__attribute__((weak)) BOOL daNpc_Bm1_c::CreateHeap() { return gabi::call<BOOL>(0x021FBFF0, this); }
__attribute__((weak)) fopAc_ac_c* daNpc_Bm1_c::searchByID(fpc_ProcID a0) { return gabi::call<fopAc_ac_c*>(0x021FE2C4, this, a0); } /* matcher: cLib_calcTimer<s> */
__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_BMB_0() { return gabi::call<u32>(0x02201EF4, this); }
__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_BMB_1() { return gabi::call<u32>(0x02201FF0, this); }
__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_BMC_3() { return gabi::call<u32>(0x02202354, this); }
__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_BMD_0() { return gabi::call<u32>(0x02202450, this); }
