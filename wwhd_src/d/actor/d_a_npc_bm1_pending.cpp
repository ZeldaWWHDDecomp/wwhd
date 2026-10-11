/* Generic Rito method bindings used when a decompiled part is absent. */
#include "d/actor/d_a_npc_bm1_pending.h"

// HD: weak methods defer to guest implementations until a decompiled part replaces them.

// HD: preserve the verified register return values and explicit hidden-result parameter.
__attribute__((weak)) void daNpc_Bm1_c::nodeBm1Control(J3DNode* i_node, J3DModel* i_model) {
    bm1Pending_nodeBm1Control(this, i_node, i_model);
}

__attribute__((weak)) bool daNpc_Bm1_c::chk_appCnd() {
    return bm1Pending_chk_appCnd(this);
}

__attribute__((weak)) bool daNpc_Bm1_c::init_PST_0() {
    return bm1Pending_init_PST_0(this);
}

__attribute__((weak)) bool daNpc_Bm1_c::init_PST_1() {
    return bm1Pending_init_PST_1(this);
}

__attribute__((weak)) bool daNpc_Bm1_c::init_PST_2() {
    return bm1Pending_init_PST_2(this);
}

__attribute__((weak)) bool daNpc_Bm1_c::init_PST_3() {
    return bm1Pending_init_PST_3(this);
}

__attribute__((weak)) bool daNpc_Bm1_c::init_PST_4() {
    return bm1Pending_init_PST_4(this);
}

__attribute__((weak)) bool daNpc_Bm1_c::init_BMC_1() {
    return bm1Pending_init_BMC_1(this);
}

__attribute__((weak)) bool daNpc_Bm1_c::init_BMC_2() {
    return bm1Pending_init_BMC_2(this);
}

__attribute__((weak)) bool daNpc_Bm1_c::init_BMC_3() {
    return bm1Pending_init_BMC_3(this);
}

__attribute__((weak)) bool daNpc_Bm1_c::init_BMD_1() {
    return bm1Pending_init_BMD_1(this);
}

__attribute__((weak)) bool daNpc_Bm1_c::createInit() {
    return bm1Pending_createInit(this);
}

__attribute__((weak)) void daNpc_Bm1_c::setMtx(u32 i_param_1) {
    bm1Pending_setMtx(this, i_param_1);
}

__attribute__((weak)) u32 daNpc_Bm1_c::btpNum_toResID(int i_param_1) {
    return bm1Pending_btpNum_toResID(this, i_param_1);
}

__attribute__((weak)) bool daNpc_Bm1_c::setBtp(u32 i_param_1, int i_btp_num) {
    return bm1Pending_setBtp(this, i_param_1, i_btp_num);
}

__attribute__((weak)) void daNpc_Bm1_c::plyTexPttrnAnm() {
    bm1Pending_plyTexPttrnAnm(this);
}

__attribute__((weak)) u32 daNpc_Bm1_c::setAnm_tex(s8 i_param_1) {
    return bm1Pending_setAnm_tex(this, i_param_1);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::setAnm_anm(anm_prm_c* i_anmPrmP) {
    return bm1Pending_setAnm_anm(this, i_anmPrmP);
}

__attribute__((weak)) void daNpc_Bm1_c::setAnm_NUM(int i_param_1, int i_param_2) {
    bm1Pending_setAnm_NUM(this, i_param_1, i_param_2);
}

__attribute__((weak)) bool daNpc_Bm1_c::setAnm() {
    return bm1Pending_setAnm(this);
}

__attribute__((weak)) void daNpc_Bm1_c::setPlaySpd(f32 i_speed) {
    bm1Pending_setPlaySpd(this, i_speed);
}

__attribute__((weak)) void daNpc_Bm1_c::chg_anmAtr(u8 i_param_1) {
    bm1Pending_chg_anmAtr(this, i_param_1);
}

__attribute__((weak)) void daNpc_Bm1_c::control_anmAtr() {
    bm1Pending_control_anmAtr(this);
}

__attribute__((weak)) void daNpc_Bm1_c::setAnm_ATR(int i_param_1) {
    bm1Pending_setAnm_ATR(this, i_param_1);
}

__attribute__((weak)) void daNpc_Bm1_c::anmAtr(u16 i_param_1) {
    bm1Pending_anmAtr(this, i_param_1);
}

__attribute__((weak)) void daNpc_Bm1_c::eventOrder() {
    bm1Pending_eventOrder(this);
}

__attribute__((weak)) void daNpc_Bm1_c::checkOrder() {
    bm1Pending_checkOrder(this);
}

__attribute__((weak)) u8 daNpc_Bm1_c::chk_manzai() {
    return bm1Pending_chk_manzai(this);
}

__attribute__((weak)) bool daNpc_Bm1_c::chk_talk() {
    return bm1Pending_chk_talk(this);
}

__attribute__((weak)) u8 daNpc_Bm1_c::chk_partsNotMove() {
    return bm1Pending_chk_partsNotMove(this);
}

__attribute__((weak)) void daNpc_Bm1_c::lookBack() {
    bm1Pending_lookBack(this);
}

__attribute__((weak)) u16 daNpc_Bm1_c::next_msgStatus(be<u32>* i_msg_no) {
    return bm1Pending_next_msgStatus(this, i_msg_no);
}

__attribute__((weak)) s32 daNpc_Bm1_c::getBitMask() {
    return bm1Pending_getBitMask(this);
}

__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_PST_1() {
    return bm1Pending_getMsg_PST_1(this);
}

__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_PST_3() {
    return bm1Pending_getMsg_PST_3(this);
}

__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_SKT_0() {
    return bm1Pending_getMsg_SKT_0(this);
}

__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_KKT_0() {
    return bm1Pending_getMsg_KKT_0(this);
}

__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_BMB_2() {
    return bm1Pending_getMsg_BMB_2(this);
}

__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_BMC_0() {
    return bm1Pending_getMsg_BMC_0(this);
}

__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_BMC_2() {
    return bm1Pending_getMsg_BMC_2(this);
}

__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_BMD_1() {
    return bm1Pending_getMsg_BMD_1(this);
}

__attribute__((weak)) u32 daNpc_Bm1_c::getMsg() {
    return bm1Pending_getMsg(this);
}

__attribute__((weak)) u8 daNpc_Bm1_c::chkAttention() {
    return bm1Pending_chkAttention(this);
}

__attribute__((weak)) bool daNpc_Bm1_c::partner_srch_sub(u32 i_searchFunc) {
    return bm1Pending_partner_srch_sub(this, i_searchFunc);
}

__attribute__((weak)) void daNpc_Bm1_c::partner_srch() {
    bm1Pending_partner_srch(this);
}

__attribute__((weak)) u32 daNpc_Bm1_c::bm_movPass(u32 i_param_1) {
    return bm1Pending_bm_movPass(this, i_param_1);
}

__attribute__((weak)) void daNpc_Bm1_c::bm_setFlyAnm() {
    bm1Pending_bm_setFlyAnm(this);
}

__attribute__((weak)) void daNpc_Bm1_c::bm_clcFlySpd() {
    bm1Pending_bm_clcFlySpd(this);
}

__attribute__((weak)) void daNpc_Bm1_c::bm_clcMovSpd() {
    bm1Pending_bm_clcMovSpd(this);
}

__attribute__((weak)) bool daNpc_Bm1_c::bm_flyMove() {
    return bm1Pending_bm_flyMove(this);
}

__attribute__((weak)) void daNpc_Bm1_c::bm_nMove() {
    bm1Pending_bm_nMove(this);
}

__attribute__((weak)) void daNpc_Bm1_c::setPrtcl_Flyaway() {
    bm1Pending_setPrtcl_Flyaway(this);
}

/* matcher: 021FD0A0 (that is delPrtcl_Land0) */
__attribute__((weak)) void daNpc_Bm1_c::delPrtcl_Flyaway() {
    bm1Pending_delPrtcl_Flyaway(this);
}

__attribute__((weak)) void daNpc_Bm1_c::nodeWngControl(J3DNode* i_node, J3DModel* i_model) {
    bm1Pending_nodeWngControl(this, i_node, i_model);
}

__attribute__((weak)) void daNpc_Bm1_c::nodeArmControl(J3DNode* i_node, J3DModel* i_model) {
    bm1Pending_nodeArmControl(this, i_node, i_model);
}

__attribute__((weak)) u32 daNpc_Bm1_c::iniTexPttrnAnm(u32 i_param_1) {
    return bm1Pending_iniTexPttrnAnm(this, i_param_1);
}

__attribute__((weak)) bool daNpc_Bm1_c::init_SKT_0() {
    return bm1Pending_init_SKT_0(this);
}

__attribute__((weak)) u32 daNpc_Bm1_c::init_KKT_0() {
    return bm1Pending_init_KKT_0(this);
}

__attribute__((weak)) bool daNpc_Bm1_c::init_BMB_0() {
    return bm1Pending_init_BMB_0(this);
}

__attribute__((weak)) u32 daNpc_Bm1_c::init_BMB_1() {
    return bm1Pending_init_BMB_1(this);
}

__attribute__((weak)) bool daNpc_Bm1_c::init_BMB_2() {
    return bm1Pending_init_BMB_2(this);
}

__attribute__((weak)) bool daNpc_Bm1_c::init_BMC_0() {
    return bm1Pending_init_BMC_0(this);
}

__attribute__((weak)) u32 daNpc_Bm1_c::init_BMD_0() {
    return bm1Pending_init_BMD_0(this);
}

__attribute__((weak)) void daNpc_Bm1_c::delPrtcl_Land0() {
    bm1Pending_delPrtcl_Land0(this);
}

__attribute__((weak)) void daNpc_Bm1_c::flwPrtcl_Hane0() {
    bm1Pending_flwPrtcl_Hane0(this);
}

__attribute__((weak)) void daNpc_Bm1_c::flwPrtcl_Hane1() {
    bm1Pending_flwPrtcl_Hane1(this);
}

__attribute__((weak)) void daNpc_Bm1_c::setAttention(u32 i_setEyePos) {
    bm1Pending_setAttention(this, i_setEyePos);
}

__attribute__((weak)) void daNpc_Bm1_c::delPrtcl_Hane0() {
    bm1Pending_delPrtcl_Hane0(this);
}

__attribute__((weak)) void daNpc_Bm1_c::delPrtcl_Hane1() {
    bm1Pending_delPrtcl_Hane1(this);
}

__attribute__((weak)) u32 daNpc_Bm1_c::eInit_DEL_ACTOR_() {
    return bm1Pending_eInit_DEL_ACTOR_(this);
}

__attribute__((weak)) void daNpc_Bm1_c::eInit_INI_EVN_1_() {
    bm1Pending_eInit_INI_EVN_1_(this);
}

__attribute__((weak)) u32 daNpc_Bm1_c::anmNum_toResID(int i_anmNum) {
    return bm1Pending_anmNum_toResID(this, i_anmNum);
}

__attribute__((weak)) u32 daNpc_Bm1_c::wingAnmNum_toResID(int i_anmNum) {
    return bm1Pending_wingAnmNum_toResID(this, i_anmNum);
}

__attribute__((weak)) u32 daNpc_Bm1_c::headAnmNum_toResID(int i_anmNum) {
    return bm1Pending_headAnmNum_toResID(this, i_anmNum);
}

__attribute__((weak)) void daNpc_Bm1_c::eInit_SET_ANM_(be<s32>* i_anmno_p) {
    bm1Pending_eInit_SET_ANM_(this, i_anmno_p);
}

__attribute__((weak)) void daNpc_Bm1_c::eInit_setLocFlag(be<s32>* i_param_1) {
    bm1Pending_eInit_setLocFlag(this, i_param_1);
}

__attribute__((weak)) void daNpc_Bm1_c::eInit_setShapeAngleY(be<s32>* i_setShapeAngle,
                                                             s16 i_shapeAngle) {
    bm1Pending_eInit_setShapeAngleY(this, i_setShapeAngle, i_shapeAngle);
}

__attribute__((weak)) bool daNpc_Bm1_c::eMove_FLY_() {
    return bm1Pending_eMove_FLY_(this);
}

__attribute__((weak)) u8 daNpc_Bm1_c::eMove_KMA_FLY_() {
    return bm1Pending_eMove_KMA_FLY_(this);
}

__attribute__((weak)) u32 daNpc_Bm1_c::eMove_ATTENTION_() {
    return bm1Pending_eMove_ATTENTION_(this);
}

__attribute__((weak)) void daNpc_Bm1_c::setPrtcl_Land0() {
    bm1Pending_setPrtcl_Land0(this);
}

__attribute__((weak)) void daNpc_Bm1_c::setPrtcl_Hane0() {
    bm1Pending_setPrtcl_Hane0(this);
}

__attribute__((weak)) void daNpc_Bm1_c::setPrtcl_Hane1() {
    bm1Pending_setPrtcl_Hane1(this);
}

__attribute__((weak)) bool daNpc_Bm1_c::decideType(int i_type_param, int i_spawn_cond_param) {
    return bm1Pending_decideType(this, i_type_param, i_spawn_cond_param);
}

__attribute__((weak)) void daNpc_Bm1_c::eInit_setEvTimer(be<s32>* i_param_1) {
    bm1Pending_eInit_setEvTimer(this, i_param_1);
}

__attribute__((weak)) cXyz* daNpc_Bm1_c::eInit_calcRelativPos(cXyz* result, cXyz* i_param_2,
                                                              be<s32>* arg2) {
    return bm1Pending_eInit_calcRelativPos(this, result, i_param_2, arg2);
}

__attribute__((weak)) void daNpc_Bm1_c::eInit_ATTENTION_(be<s32>* i_prm_0_p, be<s32>* i_prm_1_p,
                                                         be<s32>* i_prm_2_p, cXyz* i_offset_p,
                                                         be<s32>* i_angle_p, be<s32>* i_index_p,
                                                         be<s32>* i_timer_p) {
    bm1Pending_eInit_ATTENTION_(this, i_prm_0_p, i_prm_1_p, i_prm_2_p, i_offset_p, i_angle_p,
                                i_index_p, i_timer_p);
}

__attribute__((weak)) void daNpc_Bm1_c::eInit_SET_PLYER_GOL_(be<s32>* i_prm_0_p, cXyz* i_offset_p,
                                                             be<s32>* i_angle_p) {
    bm1Pending_eInit_SET_PLYER_GOL_(this, i_prm_0_p, i_offset_p, i_angle_p);
}

__attribute__((weak)) f32 daNpc_Bm1_c::eInit_prmFloat(be<f32>* param_1, f32 param_2) {
    return bm1Pending_eInit_prmFloat(this, param_1, param_2);
}

__attribute__((weak)) void daNpc_Bm1_c::eInit_FLY_(be<s32>* i_prm_0_p, be<f32>* i_speed_p,
                                                   be<f32>* i_spd_y_p, be<f32>* i_accel_p,
                                                   be<f32>* i_acc_y_p) {
    bm1Pending_eInit_FLY_(this, i_prm_0_p, i_speed_p, i_spd_y_p, i_accel_p, i_acc_y_p);
}

__attribute__((weak)) void daNpc_Bm1_c::eInit_WLK_(be<s32>* i_prm_0_p, be<f32>* i_speed_p,
                                                   be<f32>* i_accel_p, cXyz* i_offset_p,
                                                   be<s32>* i_angle_p, be<s32>* i_index_p,
                                                   be<s32>* i_timer_p) {
    bm1Pending_eInit_WLK_(this, i_prm_0_p, i_speed_p, i_accel_p, i_offset_p, i_angle_p, i_index_p,
                          i_timer_p);
}

__attribute__((weak)) void daNpc_Bm1_c::eInit_SET_NXT_PTH_INF_() {
    bm1Pending_eInit_SET_NXT_PTH_INF_(this);
}

__attribute__((weak)) void daNpc_Bm1_c::eInit_MOV_PTH_POINT_(be<s32>* i_prm_0_p,
                                                             be<s32>* i_anm_no_p,
                                                             be<s32>* i_index_p,
                                                             be<s32>* i_angle_p) {
    bm1Pending_eInit_MOV_PTH_POINT_(this, i_prm_0_p, i_anm_no_p, i_index_p, i_angle_p);
}

__attribute__((weak)) void daNpc_Bm1_c::event_actionInit(int arg0) {
    bm1Pending_event_actionInit(this, arg0);
}

__attribute__((weak)) bool daNpc_Bm1_c::eMove_WLK_() {
    return bm1Pending_eMove_WLK_(this);
}

__attribute__((weak)) u32 daNpc_Bm1_c::event_action() {
    return bm1Pending_event_action(this);
}

__attribute__((weak)) void daNpc_Bm1_c::cut_init_360_TRN(int cutIndex) {
    bm1Pending_cut_init_360_TRN(this, cutIndex);
}

__attribute__((weak)) bool daNpc_Bm1_c::cut_move_360_TRN() {
    return bm1Pending_cut_move_360_TRN(this);
}

__attribute__((weak)) void daNpc_Bm1_c::privateCut(int arg0) {
    bm1Pending_privateCut(this, arg0);
}

__attribute__((weak)) void daNpc_Bm1_c::endEvent() {
    bm1Pending_endEvent(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::isEventEntry() {
    return bm1Pending_isEventEntry(this);
}

__attribute__((weak)) void daNpc_Bm1_c::event_proc(int arg0) {
    bm1Pending_event_proc(this, arg0);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::set_action(ProcFunc_l* i_action, void* i_param_2) {
    return bm1Pending_set_action(this, i_action, i_param_2);
}

__attribute__((weak)) void daNpc_Bm1_c::setStt(s8 i_status) {
    bm1Pending_setStt(this, i_status);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::d_wait() {
    return bm1Pending_d_wait(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::lookup() {
    return bm1Pending_lookup(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::orooro() {
    return bm1Pending_orooro(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::wait_1() {
    return bm1Pending_wait_1(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::talk_1() {
    return bm1Pending_talk_1(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::talk_2() {
    return bm1Pending_talk_2(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::manzai() {
    return bm1Pending_manzai(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::wait_4() {
    return bm1Pending_wait_4(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::flyawy() {
    return bm1Pending_flyawy(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::wait_5() {
    return bm1Pending_wait_5(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::h_wait() {
    return bm1Pending_h_wait(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::wait_7() {
    return bm1Pending_wait_7(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::wait_3() {
    return bm1Pending_wait_3(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::wait_8() {
    return bm1Pending_wait_8(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::wait_2() {
    return bm1Pending_wait_2(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::walk_1() {
    return bm1Pending_walk_1(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::CHKwai() {
    return bm1Pending_CHKwai(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::demo_action1(void* userData) {
    return bm1Pending_demo_action1(this, userData);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::wait_action1(void* userData) {
    return bm1Pending_wait_action1(this, userData);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::wait_action2(void* userData) {
    return bm1Pending_wait_action2(this, userData);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::wait_action3(void* userData) {
    return bm1Pending_wait_action3(this, userData);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::wait_action4(void* userData) {
    return bm1Pending_wait_action4(this, userData);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::wait_action5(void* userData) {
    return bm1Pending_wait_action5(this, userData);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::wait_action6(void* userData) {
    return bm1Pending_wait_action6(this, userData);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::wait_action7(void* userData) {
    return bm1Pending_wait_action7(this, userData);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::wait_action8(void* userData) {
    return bm1Pending_wait_action8(this, userData);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::wait_action9(void* userData) {
    return bm1Pending_wait_action9(this, userData);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::wait_actionA(void* userData) {
    return bm1Pending_wait_actionA(this, userData);
}

__attribute__((weak)) u8 daNpc_Bm1_c::demo() {
    return bm1Pending_demo(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::_draw() {
    return bm1Pending__draw(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::_execute() {
    return bm1Pending__execute(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::_delete() {
    return bm1Pending__delete(this);
}

__attribute__((weak)) cPhs_State daNpc_Bm1_c::_create() {
    return bm1Pending__create(this);
}

__attribute__((weak)) J3DModelData* daNpc_Bm1_c::create_Anm() {
    return bm1Pending_create_Anm(this);
}

__attribute__((weak)) J3DModelData* daNpc_Bm1_c::create_hed_Anm() {
    return bm1Pending_create_hed_Anm(this);
}

__attribute__((weak)) J3DModelData* daNpc_Bm1_c::create_wng_Anm() {
    return bm1Pending_create_wng_Anm(this);
}

__attribute__((weak)) J3DModelData* daNpc_Bm1_c::create_arm_Anm() {
    return bm1Pending_create_arm_Anm(this);
}

__attribute__((weak)) bool daNpc_Bm1_c::create_itm_Mdl() {
    return bm1Pending_create_itm_Mdl(this);
}

__attribute__((weak)) BOOL daNpc_Bm1_c::CreateHeap() {
    return bm1Pending_CreateHeap(this);
}

/* matcher: cLib_calcTimer<s> */
__attribute__((weak)) fopAc_ac_c* daNpc_Bm1_c::searchByID(fpc_ProcID i_procID) {
    return bm1Pending_searchByID(this, i_procID);
}

__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_BMB_0() {
    return bm1Pending_getMsg_BMB_0(this);
}

__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_BMB_1() {
    return bm1Pending_getMsg_BMB_1(this);
}

__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_BMC_3() {
    return bm1Pending_getMsg_BMC_3(this);
}

__attribute__((weak)) u32 daNpc_Bm1_c::getMsg_BMD_0() {
    return bm1Pending_getMsg_BMD_0(this);
}
/* WWHD glue (tools/verify/autoglue.py)
ext 021FADE8 void bm1Pending_nodeBm1Control(daNpc_Bm1_c* npc, J3DNode* i_node, J3DModel* i_model)
ext 021FC5DC bool bm1Pending_chk_appCnd(daNpc_Bm1_c* npc)
ext 021FC884 bool bm1Pending_init_PST_0(daNpc_Bm1_c* npc)
ext 021FC910 bool bm1Pending_init_PST_1(daNpc_Bm1_c* npc)
ext 021FC950 bool bm1Pending_init_PST_2(daNpc_Bm1_c* npc)
ext 021FC9CC bool bm1Pending_init_PST_3(daNpc_Bm1_c* npc)
ext 021FCA68 bool bm1Pending_init_PST_4(daNpc_Bm1_c* npc)
ext 021FCC14 bool bm1Pending_init_BMC_1(daNpc_Bm1_c* npc)
ext 021FCCB0 bool bm1Pending_init_BMC_2(daNpc_Bm1_c* npc)
ext 021FCD4C bool bm1Pending_init_BMC_3(daNpc_Bm1_c* npc)
ext 021FCDB0 bool bm1Pending_init_BMD_1(daNpc_Bm1_c* npc)
ext 021FDB44 bool bm1Pending_createInit(daNpc_Bm1_c* npc)
ext 021FD41C void bm1Pending_setMtx(daNpc_Bm1_c* npc, u32 i_param_1)
ext 021FB4C4 u32 bm1Pending_btpNum_toResID(daNpc_Bm1_c* npc, int i_param_1)
ext 021FB5B8 bool bm1Pending_setBtp(daNpc_Bm1_c* npc, u32 i_param_1, int i_btp_num)
ext 021FCE10 void bm1Pending_plyTexPttrnAnm(daNpc_Bm1_c* npc)
ext 021FE774 u32 bm1Pending_setAnm_tex(daNpc_Bm1_c* npc, s8 i_param_1)
ext 021FEAB4 BOOL bm1Pending_setAnm_anm(daNpc_Bm1_c* npc, daNpc_Bm1_c::anm_prm_c* i_anmPrmP)
ext 021FECF4 void bm1Pending_setAnm_NUM(daNpc_Bm1_c* npc, int i_param_1, int i_param_2)
ext 02201258 bool bm1Pending_setAnm(daNpc_Bm1_c* npc)
ext 02200AB4 void bm1Pending_setPlaySpd(daNpc_Bm1_c* npc, f32 i_speed)
ext 0220134C void bm1Pending_chg_anmAtr(daNpc_Bm1_c* npc, u8 i_param_1)
ext 0220141C void bm1Pending_control_anmAtr(daNpc_Bm1_c* npc)
ext 022012DC void bm1Pending_setAnm_ATR(daNpc_Bm1_c* npc, int i_param_1)
ext 02201460 void bm1Pending_anmAtr(daNpc_Bm1_c* npc, u16 i_param_1)
ext 02200CD0 void bm1Pending_eventOrder(daNpc_Bm1_c* npc)
ext 021FE408 void bm1Pending_checkOrder(daNpc_Bm1_c* npc)
ext 02201558 u8 bm1Pending_chk_manzai(daNpc_Bm1_c* npc)
ext 0220163C bool bm1Pending_chk_talk(daNpc_Bm1_c* npc)
ext 022016BC u8 bm1Pending_chk_partsNotMove(daNpc_Bm1_c* npc)
ext 021FFFFC void bm1Pending_lookBack(daNpc_Bm1_c* npc)
ext 022016FC u16 bm1Pending_next_msgStatus(daNpc_Bm1_c* npc, be<u32>* i_msg_no)
ext 02201C70 s32 bm1Pending_getBitMask(daNpc_Bm1_c* npc)
ext 02201CC4 u32 bm1Pending_getMsg_PST_1(daNpc_Bm1_c* npc)
ext 02201D04 u32 bm1Pending_getMsg_PST_3(daNpc_Bm1_c* npc)
ext 02201D44 u32 bm1Pending_getMsg_SKT_0(daNpc_Bm1_c* npc)
ext 02201E1C u32 bm1Pending_getMsg_KKT_0(daNpc_Bm1_c* npc)
ext 022020EC u32 bm1Pending_getMsg_BMB_2(daNpc_Bm1_c* npc)
ext 02202280 u32 bm1Pending_getMsg_BMC_0(daNpc_Bm1_c* npc)
ext 022022B4 u32 bm1Pending_getMsg_BMC_2(daNpc_Bm1_c* npc)
ext 0220254C u32 bm1Pending_getMsg_BMD_1(daNpc_Bm1_c* npc)
ext 0220269C u32 bm1Pending_getMsg(daNpc_Bm1_c* npc)
ext 022027B0 u8 bm1Pending_chkAttention(daNpc_Bm1_c* npc)
ext 021FE218 bool bm1Pending_partner_srch_sub(daNpc_Bm1_c* npc, u32 i_searchFunc)
ext 021FE2F8 void bm1Pending_partner_srch(daNpc_Bm1_c* npc)
ext 0220057C u32 bm1Pending_bm_movPass(daNpc_Bm1_c* npc, u32 i_param_1)
ext 021FEEB0 void bm1Pending_bm_setFlyAnm(daNpc_Bm1_c* npc)
ext 02200420 void bm1Pending_bm_clcFlySpd(daNpc_Bm1_c* npc)
ext 02200A40 void bm1Pending_bm_clcMovSpd(daNpc_Bm1_c* npc)
ext 02200714 bool bm1Pending_bm_flyMove(daNpc_Bm1_c* npc)
ext 02200AF8 void bm1Pending_bm_nMove(daNpc_Bm1_c* npc)
ext 021FCF54 void bm1Pending_setPrtcl_Flyaway(daNpc_Bm1_c* npc)
ext 021FCEE4 void bm1Pending_delPrtcl_Flyaway(daNpc_Bm1_c* npc)
ext 021FA998 void bm1Pending_nodeWngControl(daNpc_Bm1_c* npc, J3DNode* i_node, J3DModel* i_model)
ext 021FABC0 void bm1Pending_nodeArmControl(daNpc_Bm1_c* npc, J3DNode* i_node, J3DModel* i_model)
ext 021FB78C u32 bm1Pending_iniTexPttrnAnm(daNpc_Bm1_c* npc, u32 i_param_1)
ext 021FCB00 bool bm1Pending_init_SKT_0(daNpc_Bm1_c* npc)
ext 021FCB4C u32 bm1Pending_init_KKT_0(daNpc_Bm1_c* npc)
ext 021FCB50 bool bm1Pending_init_BMB_0(daNpc_Bm1_c* npc)
ext 021FCB90 u32 bm1Pending_init_BMB_1(daNpc_Bm1_c* npc)
ext 021FCB94 bool bm1Pending_init_BMB_2(daNpc_Bm1_c* npc)
ext 021FCBD4 bool bm1Pending_init_BMC_0(daNpc_Bm1_c* npc)
ext 021FCDAC u32 bm1Pending_init_BMD_0(daNpc_Bm1_c* npc)
ext 021FD0A0 void bm1Pending_delPrtcl_Land0(daNpc_Bm1_c* npc)
ext 021FD25C void bm1Pending_flwPrtcl_Hane0(daNpc_Bm1_c* npc)
ext 021FD30C void bm1Pending_flwPrtcl_Hane1(daNpc_Bm1_c* npc)
ext 021FD3BC void bm1Pending_setAttention(daNpc_Bm1_c* npc, u32 i_setEyePos)
ext 021FE0BC void bm1Pending_delPrtcl_Hane0(daNpc_Bm1_c* npc)
ext 021FE10C void bm1Pending_delPrtcl_Hane1(daNpc_Bm1_c* npc)
ext 021FE70C u32 bm1Pending_eInit_DEL_ACTOR_(daNpc_Bm1_c* npc)
ext 021FE768 void bm1Pending_eInit_INI_EVN_1_(daNpc_Bm1_c* npc)
ext 021FE794 u32 bm1Pending_anmNum_toResID(daNpc_Bm1_c* npc, int i_anmNum)
ext 021FE808 u32 bm1Pending_wingAnmNum_toResID(daNpc_Bm1_c* npc, int i_anmNum)
ext 021FE87C u32 bm1Pending_headAnmNum_toResID(daNpc_Bm1_c* npc, int i_anmNum)
ext 021FED60 void bm1Pending_eInit_SET_ANM_(daNpc_Bm1_c* npc, be<s32>* i_anmno_p)
ext 021FF2B8 void bm1Pending_eInit_setLocFlag(daNpc_Bm1_c* npc, be<s32>* i_param_1)
ext 021FF2F8 void bm1Pending_eInit_setShapeAngleY(daNpc_Bm1_c* npc, be<s32>* i_setShapeAngle, s16 i_shapeAngle)
ext 021FFC84 bool bm1Pending_eMove_FLY_(daNpc_Bm1_c* npc)
ext 021FFCA0 u8 bm1Pending_eMove_KMA_FLY_(daNpc_Bm1_c* npc)
ext 021FFCA8 u32 bm1Pending_eMove_ATTENTION_(daNpc_Bm1_c* npc)
ext 021FD110 void bm1Pending_setPrtcl_Land0(daNpc_Bm1_c* npc)
ext 021FE9E0 void bm1Pending_setPrtcl_Hane0(daNpc_Bm1_c* npc)
ext 021FE8EC void bm1Pending_setPrtcl_Hane1(daNpc_Bm1_c* npc)
ext 021FC36C bool bm1Pending_decideType(daNpc_Bm1_c* npc, int i_type_param, int i_spawn_cond_param)
ext 021FF328 void bm1Pending_eInit_setEvTimer(daNpc_Bm1_c* npc, be<s32>* i_param_1)
ext 021FF0CC cXyz* bm1Pending_eInit_calcRelativPos(daNpc_Bm1_c* npc, cXyz* result, cXyz* i_param_2, be<s32>* arg2)
ext 021FF344 void bm1Pending_eInit_ATTENTION_(daNpc_Bm1_c* npc, be<s32>* i_prm_0_p, be<s32>* i_prm_1_p, be<s32>* i_prm_2_p, cXyz* i_offset_p, be<s32>* i_angle_p, be<s32>* i_index_p, be<s32>* i_timer_p)
ext 021FF210 void bm1Pending_eInit_SET_PLYER_GOL_(daNpc_Bm1_c* npc, be<s32>* i_prm_0_p, cXyz* i_offset_p, be<s32>* i_angle_p)
ext 021FEEA0 f32 bm1Pending_eInit_prmFloat(daNpc_Bm1_c* npc, be<f32>* param_1, f32 param_2)
ext 021FEECC void bm1Pending_eInit_FLY_(daNpc_Bm1_c* npc, be<s32>* i_prm_0_p, be<f32>* i_speed_p, be<f32>* i_spd_y_p, be<f32>* i_accel_p, be<f32>* i_acc_y_p)
ext 021FF5D0 void bm1Pending_eInit_WLK_(daNpc_Bm1_c* npc, be<s32>* i_prm_0_p, be<f32>* i_speed_p, be<f32>* i_accel_p, cXyz* i_offset_p, be<s32>* i_angle_p, be<s32>* i_index_p, be<s32>* i_timer_p)
ext 021FE710 void bm1Pending_eInit_SET_NXT_PTH_INF_(daNpc_Bm1_c* npc)
ext 021FED7C void bm1Pending_eInit_MOV_PTH_POINT_(daNpc_Bm1_c* npc, be<s32>* i_prm_0_p, be<s32>* i_anm_no_p, be<s32>* i_index_p, be<s32>* i_angle_p)
ext 021FF888 void bm1Pending_event_actionInit(daNpc_Bm1_c* npc, int arg0)
ext 021FFCFC bool bm1Pending_eMove_WLK_(daNpc_Bm1_c* npc)
ext 021FFD64 u32 bm1Pending_event_action(daNpc_Bm1_c* npc)
ext 021FFC48 void bm1Pending_cut_init_360_TRN(daNpc_Bm1_c* npc, int cutIndex)
ext 021FFDCC bool bm1Pending_cut_move_360_TRN(daNpc_Bm1_c* npc)
ext 021FFED0 void bm1Pending_privateCut(daNpc_Bm1_c* npc, int arg0)
ext 021FE6CC void bm1Pending_endEvent(daNpc_Bm1_c* npc)
ext 021FE68C BOOL bm1Pending_isEventEntry(daNpc_Bm1_c* npc)
ext 0220026C void bm1Pending_event_proc(daNpc_Bm1_c* npc, int arg0)
ext 021FC758 BOOL bm1Pending_set_action(daNpc_Bm1_c* npc, ProcFunc_l* i_action, void* i_param_2)
ext 02202838 void bm1Pending_setStt(daNpc_Bm1_c* npc, s8 i_status)
ext 02202B48 BOOL bm1Pending_d_wait(daNpc_Bm1_c* npc)
ext 02202BA8 BOOL bm1Pending_lookup(daNpc_Bm1_c* npc)
ext 02202BDC BOOL bm1Pending_orooro(daNpc_Bm1_c* npc)
ext 02202C2C BOOL bm1Pending_wait_1(daNpc_Bm1_c* npc)
ext 02202CF0 BOOL bm1Pending_talk_1(daNpc_Bm1_c* npc)
ext 02202FF4 BOOL bm1Pending_talk_2(daNpc_Bm1_c* npc)
ext 02203084 BOOL bm1Pending_manzai(daNpc_Bm1_c* npc)
ext 02203124 BOOL bm1Pending_wait_4(daNpc_Bm1_c* npc)
ext 02203200 BOOL bm1Pending_flyawy(daNpc_Bm1_c* npc)
ext 0220325C BOOL bm1Pending_wait_5(daNpc_Bm1_c* npc)
ext 02203348 BOOL bm1Pending_h_wait(daNpc_Bm1_c* npc)
ext 02203414 BOOL bm1Pending_wait_7(daNpc_Bm1_c* npc)
ext 02203508 BOOL bm1Pending_wait_3(daNpc_Bm1_c* npc)
ext 02203648 BOOL bm1Pending_wait_8(daNpc_Bm1_c* npc)
ext 022036CC BOOL bm1Pending_wait_2(daNpc_Bm1_c* npc)
ext 02203734 BOOL bm1Pending_walk_1(daNpc_Bm1_c* npc)
ext 02203830 BOOL bm1Pending_CHKwai(daNpc_Bm1_c* npc)
ext 0220390C BOOL bm1Pending_demo_action1(daNpc_Bm1_c* npc, void* userData)
ext 022039E0 BOOL bm1Pending_wait_action1(daNpc_Bm1_c* npc, void* userData)
ext 02203A9C BOOL bm1Pending_wait_action2(daNpc_Bm1_c* npc, void* userData)
ext 02203BAC BOOL bm1Pending_wait_action3(daNpc_Bm1_c* npc, void* userData)
ext 02203C44 BOOL bm1Pending_wait_action4(daNpc_Bm1_c* npc, void* userData)
ext 02203C98 BOOL bm1Pending_wait_action5(daNpc_Bm1_c* npc, void* userData)
ext 02203D48 BOOL bm1Pending_wait_action6(daNpc_Bm1_c* npc, void* userData)
ext 02203DF8 BOOL bm1Pending_wait_action7(daNpc_Bm1_c* npc, void* userData)
ext 02203ED8 BOOL bm1Pending_wait_action8(daNpc_Bm1_c* npc, void* userData)
ext 02203FAC BOOL bm1Pending_wait_action9(daNpc_Bm1_c* npc, void* userData)
ext 02204044 BOOL bm1Pending_wait_actionA(daNpc_Bm1_c* npc, void* userData)
ext 021FE4DC u8 bm1Pending_demo(daNpc_Bm1_c* npc)
ext 02200F90 BOOL bm1Pending__draw(daNpc_Bm1_c* npc)
ext 02200D58 BOOL bm1Pending__execute(daNpc_Bm1_c* npc)
ext 021FE17C BOOL bm1Pending__delete(daNpc_Bm1_c* npc)
ext 021FDF08 cPhs_State bm1Pending__create(daNpc_Bm1_c* npc)
ext 021FB010 J3DModelData* bm1Pending_create_Anm(daNpc_Bm1_c* npc)
ext 021FB354 J3DModelData* bm1Pending_create_hed_Anm(daNpc_Bm1_c* npc)
ext 021FB798 J3DModelData* bm1Pending_create_wng_Anm(daNpc_Bm1_c* npc)
ext 021FBA3C J3DModelData* bm1Pending_create_arm_Anm(daNpc_Bm1_c* npc)
ext 021FBD80 bool bm1Pending_create_itm_Mdl(daNpc_Bm1_c* npc)
ext 021FBFF0 BOOL bm1Pending_CreateHeap(daNpc_Bm1_c* npc)
ext 021FE2C4 fopAc_ac_c* bm1Pending_searchByID(daNpc_Bm1_c* npc, fpc_ProcID i_procID)
ext 02201EF4 u32 bm1Pending_getMsg_BMB_0(daNpc_Bm1_c* npc)
ext 02201FF0 u32 bm1Pending_getMsg_BMB_1(daNpc_Bm1_c* npc)
ext 02202354 u32 bm1Pending_getMsg_BMC_3(daNpc_Bm1_c* npc)
ext 02202450 u32 bm1Pending_getMsg_BMD_0(daNpc_Bm1_c* npc)
*/
