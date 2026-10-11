/* Tetra weak fallbacks. Decompiled definitions override these guest bindings. */
#include "d/actor/d_a_npc_zl1_pending.h"

__attribute__((weak)) void daNpc_Zl1_c::_nodeCB_Head(J3DNode* i_node, J3DModel* i_pModel) {
    zl1Pending__nodeCB_Head(this, i_node, i_pModel);
}

__attribute__((weak)) void daNpc_Zl1_c::_nodeCB_BackBone(J3DNode* i_node, J3DModel* i_pModel) {
    zl1Pending__nodeCB_BackBone(this, i_node, i_pModel);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::set_startPos(int positionSet) {
    return zl1Pending_set_startPos(this, positionSet);
}

__attribute__((weak)) bool daNpc_Zl1_c::init_ZL1_0() {
    return zl1Pending_init_ZL1_0(this);
}

__attribute__((weak)) bool daNpc_Zl1_c::init_ZL1_1() {
    return zl1Pending_init_ZL1_1(this);
}

__attribute__((weak)) bool daNpc_Zl1_c::init_ZL1_2() {
    return zl1Pending_init_ZL1_2(this);
}

__attribute__((weak)) bool daNpc_Zl1_c::init_ZL1_3() {
    return zl1Pending_init_ZL1_3(this);
}

__attribute__((weak)) bool daNpc_Zl1_c::init_ZL1_4() {
    return zl1Pending_init_ZL1_4(this);
}

__attribute__((weak)) bool daNpc_Zl1_c::init_ZL1_5() {
    return zl1Pending_init_ZL1_5(this);
}

__attribute__((weak)) bool daNpc_Zl1_c::createInit() {
    return zl1Pending_createInit(this);
}

__attribute__((weak)) void daNpc_Zl1_c::play_animation() {
    zl1Pending_play_animation(this);
}

__attribute__((weak)) bool daNpc_Zl1_c::swoon_OnShip() {
    return zl1Pending_swoon_OnShip(this);
}

__attribute__((weak)) void daNpc_Zl1_c::setMtx(u32 updatePosition) {
    zl1Pending_setMtx(this, updatePosition);
}

__attribute__((weak)) u32 daNpc_Zl1_c::bckResID(int animation) {
    return zl1Pending_bckResID(this, animation);
}

__attribute__((weak)) u32 daNpc_Zl1_c::btpResID(int animation) {
    return zl1Pending_btpResID(this, animation);
}

__attribute__((weak)) u32 daNpc_Zl1_c::btkResID(int animation) {
    return zl1Pending_btkResID(this, animation);
}

__attribute__((weak)) u32 daNpc_Zl1_c::setBtp(s32 animation, u32 modify) {
    return zl1Pending_setBtp(this, animation, modify);
}

__attribute__((weak)) void daNpc_Zl1_c::setMat() {
    zl1Pending_setMat(this);
}

__attribute__((weak)) u32 daNpc_Zl1_c::setBtk(s32 animation, u32 modify) {
    return zl1Pending_setBtk(this, animation, modify);
}

__attribute__((weak)) u32 daNpc_Zl1_c::init_texPttrnAnm(s8 animation, u32 modify) {
    return zl1Pending_init_texPttrnAnm(this, animation, modify);
}

__attribute__((weak)) void daNpc_Zl1_c::play_btp_anm() {
    zl1Pending_play_btp_anm(this);
}

__attribute__((weak)) void daNpc_Zl1_c::eye_ctrl() {
    zl1Pending_eye_ctrl(this);
}

__attribute__((weak)) void daNpc_Zl1_c::play_btk_anm() {
    zl1Pending_play_btk_anm(this);
}

__attribute__((weak)) void daNpc_Zl1_c::setAnm_anm(anm_prm_c* parameters) {
    zl1Pending_setAnm_anm(this, parameters);
}

__attribute__((weak)) void daNpc_Zl1_c::setAnm_NUM(int animation, int updateTexture) {
    zl1Pending_setAnm_NUM(this, animation, updateTexture);
}

__attribute__((weak)) void daNpc_Zl1_c::setAnm() {
    zl1Pending_setAnm(this);
}

__attribute__((weak)) void daNpc_Zl1_c::chngAnmAtr(u8 attribute) {
    zl1Pending_chngAnmAtr(this, attribute);
}

__attribute__((weak)) void daNpc_Zl1_c::setAnm_ATR() {
    zl1Pending_setAnm_ATR(this);
}

__attribute__((weak)) void daNpc_Zl1_c::anmAtr(u16 mesgNo) {
    zl1Pending_anmAtr(this, mesgNo);
}

__attribute__((weak)) u16 daNpc_Zl1_c::next_msgStatus(be<u32>* pMsgNo) {
    return zl1Pending_next_msgStatus(this, pMsgNo);
}

__attribute__((weak)) u32 daNpc_Zl1_c::getMsg_ZL1_2() {
    return zl1Pending_getMsg_ZL1_2(this);
}

__attribute__((weak)) u32 daNpc_Zl1_c::getMsg_ZL1_4() {
    return zl1Pending_getMsg_ZL1_4(this);
}

__attribute__((weak)) u32 daNpc_Zl1_c::getMsg() {
    return zl1Pending_getMsg(this);
}

__attribute__((weak)) void daNpc_Zl1_c::eventOrder() {
    zl1Pending_eventOrder(this);
}

__attribute__((weak)) void daNpc_Zl1_c::checkOrder() {
    zl1Pending_checkOrder(this);
}

__attribute__((weak)) u8 daNpc_Zl1_c::chk_talk() {
    return zl1Pending_chk_talk(this);
}

__attribute__((weak)) u8 daNpc_Zl1_c::chk_parts_notMov() {
    return zl1Pending_chk_parts_notMov(this);
}

__attribute__((weak)) u8 daNpc_Zl1_c::partner_search_sub(u32 judgeFunc) {
    return zl1Pending_partner_search_sub(this, judgeFunc);
}

__attribute__((weak)) void daNpc_Zl1_c::partner_search() {
    zl1Pending_partner_search(this);
}

__attribute__((weak)) void daNpc_Zl1_c::lookBack() {
    zl1Pending_lookBack(this);
}

__attribute__((weak)) u8 daNpc_Zl1_c::chkAttention() {
    return zl1Pending_chkAttention(this);
}

__attribute__((weak)) void daNpc_Zl1_c::setAttention(u32 enabled) {
    zl1Pending_setAttention(this, enabled);
}

__attribute__((weak)) u8 daNpc_Zl1_c::decideType(int type) {
    return zl1Pending_decideType(this, type);
}

__attribute__((weak)) f32 daNpc_Zl1_c::get_prmFloat(be<f32>* value, f32 fallback) {
    return zl1Pending_get_prmFloat(this, value, fallback);
}

__attribute__((weak)) void daNpc_Zl1_c::set_LightPos(cXyz* result) {
    zl1Pending_set_LightPos(this, result);
}

__attribute__((weak)) void daNpc_Zl1_c::init_Light() {
    zl1Pending_init_Light(this);
}

__attribute__((weak)) void daNpc_Zl1_c::incEnvironment() {
    zl1Pending_incEnvironment(this);
}

__attribute__((weak)) void daNpc_Zl1_c::decEnvironment() {
    zl1Pending_decEnvironment(this);
}

__attribute__((weak)) void daNpc_Zl1_c::darkProc() {
    zl1Pending_darkProc(this);
}

__attribute__((weak)) void daNpc_Zl1_c::cut_init_LOK_PLYER(int i_staffIdx) {
    zl1Pending_cut_init_LOK_PLYER(this, i_staffIdx);
}

__attribute__((weak)) void daNpc_Zl1_c::cut_init_LOK_PARTNER(int i_staffIdx) {
    zl1Pending_cut_init_LOK_PARTNER(this, i_staffIdx);
}

__attribute__((weak)) void daNpc_Zl1_c::cut_init_CHG_ANM_ATR(int i_staffIdx) {
    zl1Pending_cut_init_CHG_ANM_ATR(this, i_staffIdx);
}

__attribute__((weak)) void daNpc_Zl1_c::cut_init_PLYER_TRN_PARTNER(int i_staffIdx) {
    zl1Pending_cut_init_PLYER_TRN_PARTNER(this, i_staffIdx);
}

__attribute__((weak)) void daNpc_Zl1_c::cut_init_PLYER_TRN_TETRA(int i_staffIdx) {
    zl1Pending_cut_init_PLYER_TRN_TETRA(this, i_staffIdx);
}

__attribute__((weak)) void daNpc_Zl1_c::cut_init_MAJYU_START(int i_staffIdx) {
    zl1Pending_cut_init_MAJYU_START(this, i_staffIdx);
}

__attribute__((weak)) void daNpc_Zl1_c::cut_init_OKIRU(int i_staffIdx) {
    zl1Pending_cut_init_OKIRU(this, i_staffIdx);
}

__attribute__((weak)) u32 daNpc_Zl1_c::cut_move_OKIRU() {
    return zl1Pending_cut_move_OKIRU(this);
}

__attribute__((weak)) void daNpc_Zl1_c::cut_init_OKIRU_2(int i_staffIdx) {
    zl1Pending_cut_init_OKIRU_2(this, i_staffIdx);
}

__attribute__((weak)) u32 daNpc_Zl1_c::cut_move_OKIRU_2() {
    return zl1Pending_cut_move_OKIRU_2(this);
}

__attribute__((weak)) void daNpc_Zl1_c::cut_init_DRW_ONOFF(int i_staffIdx) {
    zl1Pending_cut_init_DRW_ONOFF(this, i_staffIdx);
}

__attribute__((weak)) void daNpc_Zl1_c::cut_init_PLYER_DRW_ONOFF(int i_staffIdx) {
    zl1Pending_cut_init_PLYER_DRW_ONOFF(this, i_staffIdx);
}

__attribute__((weak)) void daNpc_Zl1_c::cut_init_JMP_OFF(int i_staffIdx) {
    zl1Pending_cut_init_JMP_OFF(this, i_staffIdx);
}

__attribute__((weak)) u32 daNpc_Zl1_c::cut_move_JMP_OFF() {
    return zl1Pending_cut_move_JMP_OFF(this);
}

__attribute__((weak)) void daNpc_Zl1_c::cut_init_OMAMORI_ONOFF(int i_staffIdx) {
    zl1Pending_cut_init_OMAMORI_ONOFF(this, i_staffIdx);
}

__attribute__((weak)) void daNpc_Zl1_c::cut_init_SURPRISED(int i_staffIdx) {
    zl1Pending_cut_init_SURPRISED(this, i_staffIdx);
}

__attribute__((weak)) void daNpc_Zl1_c::privateCut(int i_staffIdx) {
    zl1Pending_privateCut(this, i_staffIdx);
}

__attribute__((weak)) void daNpc_Zl1_c::endEvent() {
    zl1Pending_endEvent(this);
}

__attribute__((weak)) s32 daNpc_Zl1_c::isEventEntry() {
    return zl1Pending_isEventEntry(this);
}

__attribute__((weak)) void daNpc_Zl1_c::event_proc(int i_staffIdx) {
    zl1Pending_event_proc(this, i_staffIdx);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::set_action(ProcFunc_l* action, void* actionData) {
    return zl1Pending_set_action(this, action, actionData);
}

__attribute__((weak)) void daNpc_Zl1_c::setStt(s8 state) {
    zl1Pending_setStt(this, state);
}

__attribute__((weak)) u8 daNpc_Zl1_c::chk_areaIN(f32 radius, f32 height, s16 angle,
                                                 cXyz* position) {
    return zl1Pending_chk_areaIN(this, radius, height, angle, position);
}

__attribute__((weak)) void daNpc_Zl1_c::setWaterRipple() {
    zl1Pending_setWaterRipple(this);
}

__attribute__((weak)) void daNpc_Zl1_c::setWaterSplash() {
    zl1Pending_setWaterSplash(this);
}

__attribute__((weak)) void daNpc_Zl1_c::set_simpleLand(u32 enabled) {
    zl1Pending_set_simpleLand(this, enabled);
}

__attribute__((weak)) void daNpc_Zl1_c::setEff() {
    zl1Pending_setEff(this);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::setFrontWallType() {
    return zl1Pending_setFrontWallType(this);
}

__attribute__((weak)) u8 daNpc_Zl1_c::move_jmp() {
    return zl1Pending_move_jmp(this);
}

__attribute__((weak)) void daNpc_Zl1_c::kyoroPos(cXyz* result, int positionIndex) {
    zl1Pending_kyoroPos(this, result, positionIndex);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::kyorokyoro() {
    return zl1Pending_kyorokyoro(this);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::wait_1() {
    return zl1Pending_wait_1(this);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::talk_1() {
    return zl1Pending_talk_1(this);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::demo_1() {
    return zl1Pending_demo_1(this);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::demo_2() {
    return zl1Pending_demo_2(this);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::demo_3() {
    return zl1Pending_demo_3(this);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::demo_4() {
    return zl1Pending_demo_4(this);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::optn_1() {
    return zl1Pending_optn_1(this);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::optn_2() {
    return zl1Pending_optn_2(this);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::optn_3() {
    return zl1Pending_optn_3(this);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::wait_action1(void* actionData) {
    return zl1Pending_wait_action1(this, actionData);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::demo_action1(void* actionData) {
    return zl1Pending_demo_action1(this, actionData);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::demo_action2(void* actionData) {
    return zl1Pending_demo_action2(this, actionData);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::optn_action1(void* actionData) {
    return zl1Pending_optn_action1(this, actionData);
}

__attribute__((weak)) u8 daNpc_Zl1_c::demo() {
    return zl1Pending_demo(this);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::_draw() {
    return zl1Pending__draw(this);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::_execute() {
    return zl1Pending__execute(this);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::_delete() {
    return zl1Pending__delete(this);
}

__attribute__((weak)) cPhs_State daNpc_Zl1_c::_create() {
    return zl1Pending__create(this);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::bodyCreateHeap() {
    return zl1Pending_bodyCreateHeap(this);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::itemCreateHeap() {
    return zl1Pending_itemCreateHeap(this);
}

__attribute__((weak)) BOOL daNpc_Zl1_c::CreateHeap() {
    return zl1Pending_CreateHeap(this);
}

__attribute__((weak)) bool daNpc_Zl1_c::init_ZL1_6() {
    return zl1Pending_init_ZL1_6(this);
}

__attribute__((weak)) bool daNpc_Zl1_c::init_ZL1_7() {
    return zl1Pending_init_ZL1_7(this);
}

__attribute__((weak)) void daNpc_Zl1_c::setEyeCtrl() {
    zl1Pending_setEyeCtrl(this);
}

__attribute__((weak)) void daNpc_Zl1_c::clrEyeCtrl() {
    zl1Pending_clrEyeCtrl(this);
}

// HD: Preserve full-register arguments and returns, and the explicit cXyz result pointers.
/* WWHD glue (tools/verify/autoglue.py)
ext 023020F8 void zl1Pending__nodeCB_Head(daNpc_Zl1_c* npc, J3DNode* i_node, J3DModel* i_pModel)
ext 023022C0 void zl1Pending__nodeCB_BackBone(daNpc_Zl1_c* npc, J3DNode* i_node, J3DModel* i_pModel)
ext 02303D14 BOOL zl1Pending_set_startPos(daNpc_Zl1_c* npc, int positionSet)
ext 023039E4 bool zl1Pending_init_ZL1_0(daNpc_Zl1_c* npc)
ext 02303A7C bool zl1Pending_init_ZL1_1(daNpc_Zl1_c* npc)
ext 02303B14 bool zl1Pending_init_ZL1_2(daNpc_Zl1_c* npc)
ext 02303BBC bool zl1Pending_init_ZL1_3(daNpc_Zl1_c* npc)
ext 02303CBC bool zl1Pending_init_ZL1_4(daNpc_Zl1_c* npc)
ext 02303E98 bool zl1Pending_init_ZL1_5(daNpc_Zl1_c* npc)
ext 0230503C bool zl1Pending_createInit(daNpc_Zl1_c* npc)
ext 023045E0 void zl1Pending_play_animation(daNpc_Zl1_c* npc)
ext 0230473C bool zl1Pending_swoon_OnShip(daNpc_Zl1_c* npc)
ext 02304C98 void zl1Pending_setMtx(daNpc_Zl1_c* npc, u32 updatePosition)
ext 0230600C u32 zl1Pending_bckResID(daNpc_Zl1_c* npc, int animation)
ext 02302484 u32 zl1Pending_btpResID(daNpc_Zl1_c* npc, int animation)
ext 02302834 u32 zl1Pending_btkResID(daNpc_Zl1_c* npc, int animation)
ext 023024F4 u32 zl1Pending_setBtp(daNpc_Zl1_c* npc, s32 animation, u32 modify)
ext 023028A4 void zl1Pending_setMat(daNpc_Zl1_c* npc)
ext 02302984 u32 zl1Pending_setBtk(daNpc_Zl1_c* npc, s32 animation, u32 modify)
ext 02302A90 u32 zl1Pending_init_texPttrnAnm(daNpc_Zl1_c* npc, s8 animation, u32 modify)
ext 02304268 void zl1Pending_play_btp_anm(daNpc_Zl1_c* npc)
ext 02304310 void zl1Pending_eye_ctrl(daNpc_Zl1_c* npc)
ext 02304548 void zl1Pending_play_btk_anm(daNpc_Zl1_c* npc)
ext 0230607C void zl1Pending_setAnm_anm(daNpc_Zl1_c* npc, daNpc_Zl1_c::anm_prm_c* parameters)
ext 023066D0 void zl1Pending_setAnm_NUM(daNpc_Zl1_c* npc, int animation, int updateTexture)
ext 02306114 void zl1Pending_setAnm(daNpc_Zl1_c* npc)
ext 0230883C void zl1Pending_chngAnmAtr(daNpc_Zl1_c* npc, u8 attribute)
ext 02306458 void zl1Pending_setAnm_ATR(daNpc_Zl1_c* npc)
ext 02308AC0 void zl1Pending_anmAtr(daNpc_Zl1_c* npc, u16 mesgNo)
ext 02308B90 u16 zl1Pending_next_msgStatus(daNpc_Zl1_c* npc, be<u32>* pMsgNo)
ext 02308C4C u32 zl1Pending_getMsg_ZL1_2(daNpc_Zl1_c* npc)
ext 02308CBC u32 zl1Pending_getMsg_ZL1_4(daNpc_Zl1_c* npc)
ext 02308D08 u32 zl1Pending_getMsg(daNpc_Zl1_c* npc)
ext 02307988 void zl1Pending_eventOrder(daNpc_Zl1_c* npc)
ext 023055A4 void zl1Pending_checkOrder(daNpc_Zl1_c* npc)
ext 02308D64 u8 zl1Pending_chk_talk(daNpc_Zl1_c* npc)
ext 02308DFC u8 zl1Pending_chk_parts_notMov(daNpc_Zl1_c* npc)
ext 0230545C u8 zl1Pending_partner_search_sub(daNpc_Zl1_c* npc, u32 judgeFunc)
ext 02305508 void zl1Pending_partner_search(daNpc_Zl1_c* npc)
ext 023075B0 void zl1Pending_lookBack(daNpc_Zl1_c* npc)
ext 02308E3C u8 zl1Pending_chkAttention(daNpc_Zl1_c* npc)
ext 02304C44 void zl1Pending_setAttention(daNpc_Zl1_c* npc, u32 enabled)
ext 023037F4 u8 zl1Pending_decideType(daNpc_Zl1_c* npc, int type)
ext 0230683C f32 zl1Pending_get_prmFloat(daNpc_Zl1_c* npc, be<f32>* value, f32 fallback)
ext 02306A40 void zl1Pending_set_LightPos(daNpc_Zl1_c* npc, cXyz* result)
ext 023069E8 void zl1Pending_init_Light(daNpc_Zl1_c* npc)
ext 023079F8 void zl1Pending_incEnvironment(daNpc_Zl1_c* npc)
ext 02307A30 void zl1Pending_decEnvironment(daNpc_Zl1_c* npc)
ext 02307A58 void zl1Pending_darkProc(daNpc_Zl1_c* npc)
ext 023062C4 void zl1Pending_cut_init_LOK_PLYER(daNpc_Zl1_c* npc, int i_staffIdx)
ext 023063A0 void zl1Pending_cut_init_LOK_PARTNER(daNpc_Zl1_c* npc, int i_staffIdx)
ext 023064C0 void zl1Pending_cut_init_CHG_ANM_ATR(daNpc_Zl1_c* npc, int i_staffIdx)
ext 02306528 void zl1Pending_cut_init_PLYER_TRN_PARTNER(daNpc_Zl1_c* npc, int i_staffIdx)
ext 023065D4 void zl1Pending_cut_init_PLYER_TRN_TETRA(daNpc_Zl1_c* npc, int i_staffIdx)
ext 02306624 void zl1Pending_cut_init_MAJYU_START(daNpc_Zl1_c* npc, int i_staffIdx)
ext 023066B4 void zl1Pending_cut_init_OKIRU(daNpc_Zl1_c* npc, int i_staffIdx)
ext 02306D34 u32 zl1Pending_cut_move_OKIRU(daNpc_Zl1_c* npc)
ext 02306740 void zl1Pending_cut_init_OKIRU_2(daNpc_Zl1_c* npc, int i_staffIdx)
ext 02306D48 u32 zl1Pending_cut_move_OKIRU_2(daNpc_Zl1_c* npc)
ext 0230674C void zl1Pending_cut_init_DRW_ONOFF(daNpc_Zl1_c* npc, int i_staffIdx)
ext 023067B4 void zl1Pending_cut_init_PLYER_DRW_ONOFF(daNpc_Zl1_c* npc, int i_staffIdx)
ext 0230684C void zl1Pending_cut_init_JMP_OFF(daNpc_Zl1_c* npc, int i_staffIdx)
ext 02306ED4 u32 zl1Pending_cut_move_JMP_OFF(daNpc_Zl1_c* npc)
ext 02306BD8 void zl1Pending_cut_init_OMAMORI_ONOFF(daNpc_Zl1_c* npc, int i_staffIdx)
ext 02306CDC void zl1Pending_cut_init_SURPRISED(daNpc_Zl1_c* npc, int i_staffIdx)
ext 02306F30 void zl1Pending_privateCut(daNpc_Zl1_c* npc, int i_staffIdx)
ext 02306280 void zl1Pending_endEvent(daNpc_Zl1_c* npc)
ext 02305FCC s32 zl1Pending_isEventEntry(daNpc_Zl1_c* npc)
ext 023071C4 void zl1Pending_event_proc(daNpc_Zl1_c* npc, int i_staffIdx)
ext 023038B8 BOOL zl1Pending_set_action(daNpc_Zl1_c* npc, ProcFunc_l* action, void* actionData)
ext 02306184 void zl1Pending_setStt(daNpc_Zl1_c* npc, s8 state)
ext 02309210 u8 zl1Pending_chk_areaIN(daNpc_Zl1_c* npc, f32 radius, f32 height, s16 angle, cXyz* position)
ext 02304824 void zl1Pending_setWaterRipple(daNpc_Zl1_c* npc)
ext 023048F4 void zl1Pending_setWaterSplash(daNpc_Zl1_c* npc)
ext 0230498C void zl1Pending_set_simpleLand(daNpc_Zl1_c* npc, u32 enabled)
ext 02304A6C void zl1Pending_setEff(daNpc_Zl1_c* npc)
ext 02308EC4 BOOL zl1Pending_setFrontWallType(daNpc_Zl1_c* npc)
ext 02306DB8 u8 zl1Pending_move_jmp(daNpc_Zl1_c* npc)
ext 0230741C void zl1Pending_kyoroPos(daNpc_Zl1_c* npc, cXyz* result, int positionIndex)
ext 023074E4 BOOL zl1Pending_kyorokyoro(daNpc_Zl1_c* npc)
ext 02309394 BOOL zl1Pending_wait_1(daNpc_Zl1_c* npc)
ext 02309478 BOOL zl1Pending_talk_1(daNpc_Zl1_c* npc)
ext 023095E4 BOOL zl1Pending_demo_1(daNpc_Zl1_c* npc)
ext 02309640 BOOL zl1Pending_demo_2(daNpc_Zl1_c* npc)
ext 023096CC BOOL zl1Pending_demo_3(daNpc_Zl1_c* npc)
ext 0230976C BOOL zl1Pending_demo_4(daNpc_Zl1_c* npc)
ext 02309794 BOOL zl1Pending_optn_1(daNpc_Zl1_c* npc)
ext 023099F4 BOOL zl1Pending_optn_2(daNpc_Zl1_c* npc)
ext 02309CC4 BOOL zl1Pending_optn_3(daNpc_Zl1_c* npc)
ext 02309CEC BOOL zl1Pending_wait_action1(daNpc_Zl1_c* npc, void* actionData)
ext 02309E10 BOOL zl1Pending_demo_action1(daNpc_Zl1_c* npc, void* actionData)
ext 02309EB8 BOOL zl1Pending_demo_action2(daNpc_Zl1_c* npc, void* actionData)
ext 02309FC8 BOOL zl1Pending_optn_action1(daNpc_Zl1_c* npc, void* actionData)
ext 02305778 u8 zl1Pending_demo(daNpc_Zl1_c* npc)
ext 02308090 BOOL zl1Pending__draw(daNpc_Zl1_c* npc)
ext 02307D18 BOOL zl1Pending__execute(daNpc_Zl1_c* npc)
ext 023053EC BOOL zl1Pending__delete(daNpc_Zl1_c* npc)
ext 023052B4 cPhs_State zl1Pending__create(daNpc_Zl1_c* npc)
ext 02302B08 BOOL zl1Pending_bodyCreateHeap(daNpc_Zl1_c* npc)
ext 02303400 BOOL zl1Pending_itemCreateHeap(daNpc_Zl1_c* npc)
ext 023034DC BOOL zl1Pending_CreateHeap(daNpc_Zl1_c* npc)
ext 02304148 bool zl1Pending_init_ZL1_6(daNpc_Zl1_c* npc)
ext 023041A0 bool zl1Pending_init_ZL1_7(daNpc_Zl1_c* npc)
ext 023041F8 void zl1Pending_setEyeCtrl(daNpc_Zl1_c* npc)
ext 02304230 void zl1Pending_clrEyeCtrl(daNpc_Zl1_c* npc)
*/
