/* Mila method bindings used when a decompiled part is absent. */
// HD: these local bindings use Mila's SafeString vtable.
#define SAFESTRING_VTBL 0x1001C250
#include "d/actor/d_a_npc_kk1_pending.h"

// HD: retain weak linkage, verified return values and explicit by-value/hidden-result pointers.
__attribute__((weak)) void daNpc_Kk1_c::_nodeCB_Head(J3DNode* node, J3DModel* model) {
    kk1Pending__nodeCB_Head(this, node, model);
}

__attribute__((weak)) void daNpc_Kk1_c::_nodeCB_BackBone(J3DNode* node, J3DModel* model) {
    kk1Pending__nodeCB_BackBone(this, node, model);
}

__attribute__((weak)) BOOL daNpc_Kk1_c::setBtp(s8 i_num, u32 i_modify) {
    return kk1Pending_setBtp(this, i_num, i_modify);
}

__attribute__((weak)) BOOL daNpc_Kk1_c::init_texPttrnAnm(s8 i_num, u32 i_modify) {
    return kk1Pending_init_texPttrnAnm(this, i_num, i_modify);
}

__attribute__((weak)) bool daNpc_Kk1_c::bodyCreateHeap() {
    return kk1Pending_bodyCreateHeap(this);
}

__attribute__((weak)) bool daNpc_Kk1_c::effcCreateHeap() {
    return kk1Pending_effcCreateHeap(this);
}

__attribute__((weak)) bool daNpc_Kk1_c::CreateHeap() {
    return kk1Pending_CreateHeap(this);
}

__attribute__((weak)) bool daNpc_Kk1_c::decideType(int i_type) {
    return kk1Pending_decideType(this, i_type);
}

__attribute__((weak)) BOOL daNpc_Kk1_c::set_action(ProcFunc_l* action, void* userData) {
    return kk1Pending_set_action(this, action, userData);
}

__attribute__((weak)) bool daNpc_Kk1_c::init_KK1_0() {
    return kk1Pending_init_KK1_0(this);
}

__attribute__((weak)) void daNpc_Kk1_c::play_btp_anm() {
    kk1Pending_play_btp_anm(this);
}

__attribute__((weak)) void daNpc_Kk1_c::setBikon(cXyz* position) {
    kk1Pending_setBikon(this, position);
}

__attribute__((weak)) void daNpc_Kk1_c::play_eff_anm() {
    kk1Pending_play_eff_anm(this);
}

__attribute__((weak)) void daNpc_Kk1_c::play_animation() {
    kk1Pending_play_animation(this);
}

__attribute__((weak)) void daNpc_Kk1_c::flwAse() {
    kk1Pending_flwAse(this);
}

__attribute__((weak)) void daNpc_Kk1_c::setAttention(u32 i_force) {
    kk1Pending_setAttention(this, i_force);
}

__attribute__((weak)) void daNpc_Kk1_c::setMtx(u32 i_force) {
    kk1Pending_setMtx(this, i_force);
}

__attribute__((weak)) bool daNpc_Kk1_c::createInit() {
    return kk1Pending_createInit(this);
}

__attribute__((weak)) cPhs_State daNpc_Kk1_c::_create() {
    return kk1Pending__create(this);
}

__attribute__((weak)) void daNpc_Kk1_c::delAse() {
    kk1Pending_delAse(this);
}

__attribute__((weak)) BOOL daNpc_Kk1_c::_delete() {
    return kk1Pending__delete(this);
}

__attribute__((weak)) bool daNpc_Kk1_c::partner_search_sub(u32 searchFunc) {
    return kk1Pending_partner_search_sub(this, searchFunc);
}

__attribute__((weak)) void daNpc_Kk1_c::partner_search() {
    kk1Pending_partner_search(this);
}

__attribute__((weak)) void daNpc_Kk1_c::setAse() {
    kk1Pending_setAse(this);
}

__attribute__((weak)) void daNpc_Kk1_c::setAnm_anm(anm_prm_c* animation) {
    kk1Pending_setAnm_anm(this, animation);
}

__attribute__((weak)) void daNpc_Kk1_c::setAnm_NUM(s32 animationNumber, s32 modify) {
    kk1Pending_setAnm_NUM(this, animationNumber, modify);
}

__attribute__((weak)) bool daNpc_Kk1_c::checkCommandTalk() {
    return kk1Pending_checkCommandTalk(this);
}

__attribute__((weak)) void daNpc_Kk1_c::checkOrder() {
    kk1Pending_checkOrder(this);
}

__attribute__((weak)) u8 daNpc_Kk1_c::demo() {
    return kk1Pending_demo(this);
}

__attribute__((weak)) s32 daNpc_Kk1_c::isEventEntry() {
    return kk1Pending_isEventEntry(this);
}

__attribute__((weak)) void daNpc_Kk1_c::setAnm() {
    kk1Pending_setAnm(this);
}

__attribute__((weak)) void daNpc_Kk1_c::setStt(s8 status) {
    kk1Pending_setStt(this, status);
}

__attribute__((weak)) void daNpc_Kk1_c::endEvent() {
    kk1Pending_endEvent(this);
}

__attribute__((weak)) bool daNpc_Kk1_c::cut_init_RUN_START(s32 cutIndex) {
    return kk1Pending_cut_init_RUN_START(this, cutIndex);
}

__attribute__((weak)) void daNpc_Kk1_c::cut_init_RUN(s32 cutIndex) {
    kk1Pending_cut_init_RUN(this, cutIndex);
}

__attribute__((weak)) void daNpc_Kk1_c::cut_init_CATCH_START(s32 cutIndex) {
    kk1Pending_cut_init_CATCH_START(this, cutIndex);
}

__attribute__((weak)) void daNpc_Kk1_c::cut_init_CATCH_END(s32 cutIndex) {
    kk1Pending_cut_init_CATCH_END(this, cutIndex);
}

__attribute__((weak)) void daNpc_Kk1_c::cut_init_TRN(s32 cutIndex) {
    kk1Pending_cut_init_TRN(this, cutIndex);
}

__attribute__((weak)) void daNpc_Kk1_c::cut_init_BYE_START(s32 cutIndex) {
    kk1Pending_cut_init_BYE_START(this, cutIndex);
}

__attribute__((weak)) void daNpc_Kk1_c::cut_init_BYE(s32 cutIndex) {
    kk1Pending_cut_init_BYE(this, cutIndex);
}

__attribute__((weak)) void daNpc_Kk1_c::cut_init_BYE_END(s32 cutIndex) {
    kk1Pending_cut_init_BYE_END(this, cutIndex);
}

__attribute__((weak)) void daNpc_Kk1_c::cut_init_OTOBOKE(s32 cutIndex) {
    kk1Pending_cut_init_OTOBOKE(this, cutIndex);
}

__attribute__((weak)) void daNpc_Kk1_c::cut_init_PLYER_MOV(s32 cutIndex) {
    kk1Pending_cut_init_PLYER_MOV(this, cutIndex);
}

__attribute__((weak)) void daNpc_Kk1_c::cut_init_RUNAWAY_START(s32 cutIndex) {
    kk1Pending_cut_init_RUNAWAY_START(this, cutIndex);
}

__attribute__((weak)) void daNpc_Kk1_c::cut_init_RUNAWAY_END(s32 cutIndex) {
    kk1Pending_cut_init_RUNAWAY_END(this, cutIndex);
}

__attribute__((weak)) void daNpc_Kk1_c::cut_init_BYE_CONTINUE(s32 cutIndex) {
    kk1Pending_cut_init_BYE_CONTINUE(this, cutIndex);
}

__attribute__((weak)) bool daNpc_Kk1_c::cut_move_RUN_START() {
    return kk1Pending_cut_move_RUN_START(this);
}

__attribute__((weak)) bool daNpc_Kk1_c::event_move(bool moveFlag) {
    return kk1Pending_event_move(this, moveFlag);
}

__attribute__((weak)) s32 daNpc_Kk1_c::cut_move_RUN() {
    return kk1Pending_cut_move_RUN(this);
}

__attribute__((weak)) bool daNpc_Kk1_c::cut_move_CATCH_START() {
    return kk1Pending_cut_move_CATCH_START(this);
}

__attribute__((weak)) bool daNpc_Kk1_c::cut_move_TRN() {
    return kk1Pending_cut_move_TRN(this);
}

__attribute__((weak)) bool daNpc_Kk1_c::cut_move_BYE() {
    return kk1Pending_cut_move_BYE(this);
}

__attribute__((weak)) bool daNpc_Kk1_c::cut_move_OTOBOKE() {
    return kk1Pending_cut_move_OTOBOKE(this);
}

__attribute__((weak)) bool daNpc_Kk1_c::cut_move_RUNAWAY_START() {
    return kk1Pending_cut_move_RUNAWAY_START(this);
}

__attribute__((weak)) bool daNpc_Kk1_c::cut_move_BYE_CONTINUE() {
    return kk1Pending_cut_move_BYE_CONTINUE(this);
}

__attribute__((weak)) void daNpc_Kk1_c::privateCut(s32 cutIndex) {
    kk1Pending_privateCut(this, cutIndex);
}

__attribute__((weak)) void daNpc_Kk1_c::event_proc(s32 eventIndex) {
    kk1Pending_event_proc(this, eventIndex);
}

__attribute__((weak)) void daNpc_Kk1_c::kyoroPos(cXyz* o_pos, s32 i_idx) {
    kk1Pending_kyoroPos(this, o_pos, i_idx);
}

__attribute__((weak)) bool daNpc_Kk1_c::kyorokyoro() {
    return kk1Pending_kyorokyoro(this);
}

__attribute__((weak)) void daNpc_Kk1_c::lookBack() {
    kk1Pending_lookBack(this);
}

__attribute__((weak)) void daNpc_Kk1_c::eventOrder() {
    kk1Pending_eventOrder(this);
}

__attribute__((weak)) BOOL daNpc_Kk1_c::_execute() {
    return kk1Pending__execute(this);
}

__attribute__((weak)) BOOL daNpc_Kk1_c::_draw() {
    return kk1Pending__draw(this);
}

__attribute__((weak)) void daNpc_Kk1_c::setAnm_ATR() {
    kk1Pending_setAnm_ATR(this);
}

__attribute__((weak)) void daNpc_Kk1_c::chngAnmAtr(u8 attribute) {
    kk1Pending_chngAnmAtr(this, attribute);
}

__attribute__((weak)) void daNpc_Kk1_c::ctrlAnmAtr() {
    kk1Pending_ctrlAnmAtr(this);
}

__attribute__((weak)) void daNpc_Kk1_c::anmAtr(u16 messageStatus) {
    kk1Pending_anmAtr(this, messageStatus);
}

__attribute__((weak)) u16 daNpc_Kk1_c::next_msgStatus(be<u32>* messageNumber) {
    return kk1Pending_next_msgStatus(this, messageNumber);
}

__attribute__((weak)) u32 daNpc_Kk1_c::getMsg_KK1_0() {
    return kk1Pending_getMsg_KK1_0(this);
}

__attribute__((weak)) u32 daNpc_Kk1_c::getMsg() {
    return kk1Pending_getMsg(this);
}

__attribute__((weak)) bool daNpc_Kk1_c::chk_talk() {
    return kk1Pending_chk_talk(this);
}

__attribute__((weak)) u8 daNpc_Kk1_c::chk_parts_notMov() {
    return kk1Pending_chk_parts_notMov(this);
}

__attribute__((weak)) BOOL daNpc_Kk1_c::chkAttention() {
    return kk1Pending_chkAttention(this);
}

__attribute__((weak)) void daNpc_Kk1_c::createTama(f32 value) {
    kk1Pending_createTama(this, value);
}

__attribute__((weak)) bool daNpc_Kk1_c::chk_areaIN(f32 radius, cXyz* position) {
    return kk1Pending_chk_areaIN(this, radius, position);
}

__attribute__((weak)) bool daNpc_Kk1_c::startEvent_check() {
    return kk1Pending_startEvent_check(this);
}

__attribute__((weak)) BOOL daNpc_Kk1_c::chkHitPlayer() {
    return kk1Pending_chkHitPlayer(this);
}

__attribute__((weak)) u8 daNpc_Kk1_c::chk_attn() {
    return kk1Pending_chk_attn(this);
}

__attribute__((weak)) s32 daNpc_Kk1_c::wait_1() {
    return kk1Pending_wait_1(this);
}

__attribute__((weak)) s32 daNpc_Kk1_c::walk_1() {
    return kk1Pending_walk_1(this);
}

__attribute__((weak)) s32 daNpc_Kk1_c::wait_2() {
    return kk1Pending_wait_2(this);
}

__attribute__((weak)) void daNpc_Kk1_c::init_CMT_WAI() {
    kk1Pending_init_CMT_WAI(this);
}

__attribute__((weak)) void daNpc_Kk1_c::move_CMT_WAI() {
    kk1Pending_move_CMT_WAI(this);
}

__attribute__((weak)) void daNpc_Kk1_c::init_CMT_TRN() {
    kk1Pending_init_CMT_TRN(this);
}

__attribute__((weak)) void daNpc_Kk1_c::move_CMT_TRN() {
    kk1Pending_move_CMT_TRN(this);
}

__attribute__((weak)) void daNpc_Kk1_c::init_CMT_PCK() {
    kk1Pending_init_CMT_PCK(this);
}

__attribute__((weak)) void daNpc_Kk1_c::move_CMT_PCK() {
    kk1Pending_move_CMT_PCK(this);
}

__attribute__((weak)) s32 daNpc_Kk1_c::cmmt_1() {
    return kk1Pending_cmmt_1(this);
}

__attribute__((weak)) s32 daNpc_Kk1_c::wait_3() {
    return kk1Pending_wait_3(this);
}

__attribute__((weak)) s32 daNpc_Kk1_c::wait_4() {
    return kk1Pending_wait_4(this);
}

__attribute__((weak)) BOOL daNpc_Kk1_c::talk_1() {
    return kk1Pending_talk_1(this);
}

__attribute__((weak)) BOOL daNpc_Kk1_c::wait_action1(void* userData) {
    return kk1Pending_wait_action1(this, userData);
}

__attribute__((weak)) s32 daNpc_Kk1_c::btpResID(int i_num) {
    return kk1Pending_btpResID(this, i_num);
}

__attribute__((weak)) void daNpc_Kk1_c::set_pthPoint(u8 i_idx) {
    kk1Pending_set_pthPoint(this, i_idx);
}

__attribute__((weak)) s32 daNpc_Kk1_c::bckResID(int i_num) {
    return kk1Pending_bckResID(this, i_num);
}

__attribute__((weak)) fopAc_ac_c* daNpc_Kk1_c::searchByID(fpc_ProcID i_id, be<s32>* o_res) {
    return kk1Pending_searchByID(this, i_id, o_res);
}
/* WWHD glue (tools/verify/autoglue.py)
ext 0226AAF8 void kk1Pending__nodeCB_Head(daNpc_Kk1_c* npc, J3DNode* node, J3DModel* model)
ext 0226ACB8 void kk1Pending__nodeCB_BackBone(daNpc_Kk1_c* npc, J3DNode* node, J3DModel* model)
ext 0226AE30 BOOL kk1Pending_setBtp(daNpc_Kk1_c* npc, s8 i_num, u32 i_modify)
ext 0226AF1C BOOL kk1Pending_init_texPttrnAnm(daNpc_Kk1_c* npc, s8 i_num, u32 i_modify)
ext 0226AF20 bool kk1Pending_bodyCreateHeap(daNpc_Kk1_c* npc)
ext 0226B1E0 bool kk1Pending_effcCreateHeap(daNpc_Kk1_c* npc)
ext 0226B3E8 bool kk1Pending_CreateHeap(daNpc_Kk1_c* npc)
ext 0226B550 bool kk1Pending_decideType(daNpc_Kk1_c* npc, int i_type)
ext 0226B704 BOOL kk1Pending_set_action(daNpc_Kk1_c* npc, ProcFunc_l* action, void* userData)
ext 0226B830 bool kk1Pending_init_KK1_0(daNpc_Kk1_c* npc)
ext 0226B8E4 void kk1Pending_play_btp_anm(daNpc_Kk1_c* npc)
ext 0226B984 void kk1Pending_setBikon(daNpc_Kk1_c* npc, cXyz* position)
ext 0226BA48 void kk1Pending_play_eff_anm(daNpc_Kk1_c* npc)
ext 0226BB5C void kk1Pending_play_animation(daNpc_Kk1_c* npc)
ext 0226BCD4 void kk1Pending_flwAse(daNpc_Kk1_c* npc)
ext 0226BD78 void kk1Pending_setAttention(daNpc_Kk1_c* npc, u32 i_force)
ext 0226BDCC void kk1Pending_setMtx(daNpc_Kk1_c* npc, u32 i_force)
ext 0226BFD0 bool kk1Pending_createInit(daNpc_Kk1_c* npc)
ext 0226C28C cPhs_State kk1Pending__create(daNpc_Kk1_c* npc)
ext 0226C438 void kk1Pending_delAse(daNpc_Kk1_c* npc)
ext 0226C464 BOOL kk1Pending__delete(daNpc_Kk1_c* npc)
ext 0226C4C4 bool kk1Pending_partner_search_sub(daNpc_Kk1_c* npc, u32  searchFunc)
ext 0226C570 void kk1Pending_partner_search(daNpc_Kk1_c* npc)
ext 0226C5F0 void kk1Pending_setAse(daNpc_Kk1_c* npc)
ext 0226C660 void kk1Pending_setAnm_anm(daNpc_Kk1_c* npc, daNpc_Kk1_c::anm_prm_c* animation)
ext 0226C730 void kk1Pending_setAnm_NUM(daNpc_Kk1_c* npc, s32 animationNumber, s32 modify)
ext 0226C7A0 bool kk1Pending_checkCommandTalk(daNpc_Kk1_c* npc)
ext 0226C7DC void kk1Pending_checkOrder(daNpc_Kk1_c* npc)
ext 0226C8D8 u8 kk1Pending_demo(daNpc_Kk1_c* npc)
ext 0226CA94 s32 kk1Pending_isEventEntry(daNpc_Kk1_c* npc)
ext 0226CAD4 void kk1Pending_setAnm(daNpc_Kk1_c* npc)
ext 0226CB44 void kk1Pending_setStt(daNpc_Kk1_c* npc, s8 status)
ext 0226CBE8 void kk1Pending_endEvent(daNpc_Kk1_c* npc)
ext 0226CC80 bool kk1Pending_cut_init_RUN_START(daNpc_Kk1_c* npc, s32 cutIndex)
ext 0226CD04 void kk1Pending_cut_init_RUN(daNpc_Kk1_c* npc, s32 cutIndex)
ext 0226CD94 void kk1Pending_cut_init_CATCH_START(daNpc_Kk1_c* npc, s32 cutIndex)
ext 0226CDF0 void kk1Pending_cut_init_CATCH_END(daNpc_Kk1_c* npc, s32 cutIndex)
ext 0226CE6C void kk1Pending_cut_init_TRN(daNpc_Kk1_c* npc, s32 cutIndex)
ext 0226D0A4 void kk1Pending_cut_init_BYE_START(daNpc_Kk1_c* npc, s32 cutIndex)
ext 0226D0E8 void kk1Pending_cut_init_BYE(daNpc_Kk1_c* npc, s32 cutIndex)
ext 0226D1F0 void kk1Pending_cut_init_BYE_END(daNpc_Kk1_c* npc, s32 cutIndex)
ext 0226D2B8 void kk1Pending_cut_init_OTOBOKE(daNpc_Kk1_c* npc, s32 cutIndex)
ext 0226D34C void kk1Pending_cut_init_PLYER_MOV(daNpc_Kk1_c* npc, s32 cutIndex)
ext 0226D454 void kk1Pending_cut_init_RUNAWAY_START(daNpc_Kk1_c* npc, s32 cutIndex)
ext 0226D60C void kk1Pending_cut_init_RUNAWAY_END(daNpc_Kk1_c* npc, s32 cutIndex)
ext 0226D664 void kk1Pending_cut_init_BYE_CONTINUE(daNpc_Kk1_c* npc, s32 cutIndex)
ext 0226D6CC bool kk1Pending_cut_move_RUN_START(daNpc_Kk1_c* npc)
ext 0226D83C bool kk1Pending_event_move(daNpc_Kk1_c* npc, bool moveFlag)
ext 0226DB64 s32 kk1Pending_cut_move_RUN(daNpc_Kk1_c* npc)
ext 0226DBB0 bool kk1Pending_cut_move_CATCH_START(daNpc_Kk1_c* npc)
ext 0226DBD4 bool kk1Pending_cut_move_TRN(daNpc_Kk1_c* npc)
ext 0226DC70 bool kk1Pending_cut_move_BYE(daNpc_Kk1_c* npc)
ext 0226DD7C bool kk1Pending_cut_move_OTOBOKE(daNpc_Kk1_c* npc)
ext 0226DDD8 bool kk1Pending_cut_move_RUNAWAY_START(daNpc_Kk1_c* npc)
ext 0226DE88 bool kk1Pending_cut_move_BYE_CONTINUE(daNpc_Kk1_c* npc)
ext 0226DEF0 void kk1Pending_privateCut(daNpc_Kk1_c* npc, s32 cutIndex)
ext 0226E21C void kk1Pending_event_proc(daNpc_Kk1_c* npc, s32 eventIndex)
ext 0226E498 void kk1Pending_kyoroPos(daNpc_Kk1_c* npc, cXyz* o_pos, s32 i_idx)
ext 0226E560 bool kk1Pending_kyorokyoro(daNpc_Kk1_c* npc)
ext 0226E604 void kk1Pending_lookBack(daNpc_Kk1_c* npc)
ext 0226E92C void kk1Pending_eventOrder(daNpc_Kk1_c* npc)
ext 0226E99C BOOL kk1Pending__execute(daNpc_Kk1_c* npc)
ext 0226ECA8 BOOL kk1Pending__draw(daNpc_Kk1_c* npc)
ext 0226EF4C void kk1Pending_setAnm_ATR(daNpc_Kk1_c* npc)
ext 0226EFB4 void kk1Pending_chngAnmAtr(daNpc_Kk1_c* npc, u8 attribute)
ext 0226F088 void kk1Pending_ctrlAnmAtr(daNpc_Kk1_c* npc)
ext 0226F170 void kk1Pending_anmAtr(daNpc_Kk1_c* npc, u16 messageStatus)
ext 0226F230 u16 kk1Pending_next_msgStatus(daNpc_Kk1_c* npc, be<u32>* messageNumber)
ext 0226F538 u32 kk1Pending_getMsg_KK1_0(daNpc_Kk1_c* npc)
ext 0226F61C u32 kk1Pending_getMsg(daNpc_Kk1_c* npc)
ext 0226F654 bool kk1Pending_chk_talk(daNpc_Kk1_c* npc)
ext 0226F6EC u8 kk1Pending_chk_parts_notMov(daNpc_Kk1_c* npc)
ext 0226F72C BOOL kk1Pending_chkAttention(daNpc_Kk1_c* npc)
ext 0226F7B4 void kk1Pending_createTama(daNpc_Kk1_c* npc, f32 value)
ext 0226F908 bool kk1Pending_chk_areaIN(daNpc_Kk1_c* npc, f32 radius, cXyz* position)
ext 0226FABC bool kk1Pending_startEvent_check(daNpc_Kk1_c* npc)
ext 0226FB70 BOOL kk1Pending_chkHitPlayer(daNpc_Kk1_c* npc)
ext 0226FC1C u8 kk1Pending_chk_attn(daNpc_Kk1_c* npc)
ext 0226FE78 s32 kk1Pending_wait_1(daNpc_Kk1_c* npc)
ext 022700E0 s32 kk1Pending_walk_1(daNpc_Kk1_c* npc)
ext 022703B4 s32 kk1Pending_wait_2(daNpc_Kk1_c* npc)
ext 0227051C void kk1Pending_init_CMT_WAI(daNpc_Kk1_c* npc)
ext 02270570 void kk1Pending_move_CMT_WAI(daNpc_Kk1_c* npc)
ext 0227066C void kk1Pending_init_CMT_TRN(daNpc_Kk1_c* npc)
ext 022706E4 void kk1Pending_move_CMT_TRN(daNpc_Kk1_c* npc)
ext 022708B8 void kk1Pending_init_CMT_PCK(daNpc_Kk1_c* npc)
ext 02270918 void kk1Pending_move_CMT_PCK(daNpc_Kk1_c* npc)
ext 02270AE0 s32 kk1Pending_cmmt_1(daNpc_Kk1_c* npc)
ext 02270C7C s32 kk1Pending_wait_3(daNpc_Kk1_c* npc)
ext 02270DA8 s32 kk1Pending_wait_4(daNpc_Kk1_c* npc)
ext 02270F28 BOOL kk1Pending_talk_1(daNpc_Kk1_c* npc)
ext 02271084 BOOL kk1Pending_wait_action1(daNpc_Kk1_c* npc, void* userData)
ext 0226AE24 s32 kk1Pending_btpResID(daNpc_Kk1_c* npc, int i_num)
ext 0226B690 void kk1Pending_set_pthPoint(daNpc_Kk1_c* npc, u8 i_idx)
ext 0226C5DC s32 kk1Pending_bckResID(daNpc_Kk1_c* npc, int i_num)
ext 0226CC2C fopAc_ac_c* kk1Pending_searchByID(daNpc_Kk1_c* npc, fpc_ProcID i_id, be<s32>* o_res)
*/
