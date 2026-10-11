/* d_a_npc_co1: member functions of the translation unit not yet decompiled in the part being built,
 * as plain guest calls (so the decompiled parts can call each other naturally). Weak: a decompiled
 * definition in a part replaces it. */
#include "d/actor/d_a_npc_co1.h"

__attribute__((weak)) void daNpc_Co1_c::nodeCo1Control(J3DNode* a0, J3DModel* a1) { co1Pending_nodeCo1Control(this, a0, a1); }
__attribute__((weak)) J3DModelData* daNpc_Co1_c::create_Anm() { return co1Pending_create_Anm(this); }
__attribute__((weak)) u32 daNpc_Co1_c::btpNum_toResID(int a0) { return co1Pending_btpNum_toResID(this, a0); }
__attribute__((weak)) BOOL daNpc_Co1_c::setBtp(u32 a0, int a1) { return co1Pending_setBtp(this, a0, a1); }
__attribute__((weak)) u32 daNpc_Co1_c::iniTexPttrnAnm(u32 a0) { return co1Pending_iniTexPttrnAnm(this, a0); }
__attribute__((weak)) J3DModelData* daNpc_Co1_c::create_prl_Anm() { return co1Pending_create_prl_Anm(this); }
__attribute__((weak)) BOOL daNpc_Co1_c::setBtk(u32 a0) { return co1Pending_setBtk(this, a0); }
__attribute__((weak)) bool daNpc_Co1_c::create_itm_Mdl() { return co1Pending_create_itm_Mdl(this); }
__attribute__((weak)) BOOL daNpc_Co1_c::CreateHeap() { return co1Pending_CreateHeap(this); }
__attribute__((weak)) bool daNpc_Co1_c::charDecide(int a0) { return co1Pending_charDecide(this, a0); }
__attribute__((weak)) BOOL daNpc_Co1_c::set_action(Co1ProcFunc* a0, void* a1) { return co1Pending_set_action(this, a0, a1); }
__attribute__((weak)) bool daNpc_Co1_c::init_CO1_0() { return co1Pending_init_CO1_0(this); }
__attribute__((weak)) void daNpc_Co1_c::plyTexPttrnAnm() { co1Pending_plyTexPttrnAnm(this); }
__attribute__((weak)) void daNpc_Co1_c::setAttention(u32 a0) { co1Pending_setAttention(this, a0); }
__attribute__((weak)) void daNpc_Co1_c::setMtx(u32 a0) { co1Pending_setMtx(this, a0); }
__attribute__((weak)) bool daNpc_Co1_c::createInit() { return co1Pending_createInit(this); }
__attribute__((weak)) cPhs_State daNpc_Co1_c::_create() { return co1Pending__create(this); }
__attribute__((weak)) BOOL daNpc_Co1_c::_delete() { return co1Pending__delete(this); }
__attribute__((weak)) void daNpc_Co1_c::checkOrder() { co1Pending_checkOrder(this); }
__attribute__((weak)) u8 daNpc_Co1_c::demo() { return co1Pending_demo(this); }
__attribute__((weak)) s32 daNpc_Co1_c::isEventEntry() { return co1Pending_isEventEntry(this); }
__attribute__((weak)) u32 daNpc_Co1_c::setAnm_tex(s8 a0) { return co1Pending_setAnm_tex(this, a0); }
__attribute__((weak)) u32 daNpc_Co1_c::anmNum_toResID(int a0) { return co1Pending_anmNum_toResID(this, a0); }
__attribute__((weak)) u32 daNpc_Co1_c::anmNum_toResID_prl(int a0) { return co1Pending_anmNum_toResID_prl(this, a0); }
__attribute__((weak)) BOOL daNpc_Co1_c::setAnm_anm(anm_prm_c* a0) { return co1Pending_setAnm_anm(this, a0); }
__attribute__((weak)) bool daNpc_Co1_c::setAnm() { return co1Pending_setAnm(this); }
__attribute__((weak)) void daNpc_Co1_c::setStt(s8 a0) { co1Pending_setStt(this, a0); }
__attribute__((weak)) void daNpc_Co1_c::endEvent() { co1Pending_endEvent(this); }
__attribute__((weak)) void daNpc_Co1_c::setAnm_NUM(int a0, int a1) { co1Pending_setAnm_NUM(this, a0, a1); }
__attribute__((weak)) void daNpc_Co1_c::eInit_MDR_() { co1Pending_eInit_MDR_(this); }
__attribute__((weak)) void daNpc_Co1_c::eInit_RED_LTR_() { co1Pending_eInit_RED_LTR_(this); }
__attribute__((weak)) void daNpc_Co1_c::event_actionInit(int a0) { co1Pending_event_actionInit(this, a0); }
__attribute__((weak)) u32 daNpc_Co1_c::eMove_MDR_() { return co1Pending_eMove_MDR_(this); }
__attribute__((weak)) u32 daNpc_Co1_c::eMove_RED_LTR_() { return co1Pending_eMove_RED_LTR_(this); }
__attribute__((weak)) u32 daNpc_Co1_c::event_action() { return co1Pending_event_action(this); }
__attribute__((weak)) void daNpc_Co1_c::privateCut(int a0) { co1Pending_privateCut(this, a0); }
__attribute__((weak)) void daNpc_Co1_c::lookBack() { co1Pending_lookBack(this); }
__attribute__((weak)) void daNpc_Co1_c::event_proc(int a0) { co1Pending_event_proc(this, a0); }
__attribute__((weak)) void daNpc_Co1_c::eventOrder() { co1Pending_eventOrder(this); }
__attribute__((weak)) void daNpc_Co1_c::setCollision_SP_() { co1Pending_setCollision_SP_(this); }
__attribute__((weak)) BOOL daNpc_Co1_c::_execute() { return co1Pending__execute(this); }
__attribute__((weak)) BOOL daNpc_Co1_c::_draw() { return co1Pending__draw(this); }
__attribute__((weak)) void daNpc_Co1_c::set_target(int a0) { co1Pending_set_target(this, a0); }
__attribute__((weak)) void daNpc_Co1_c::setAnm_ATR(int a0) { co1Pending_setAnm_ATR(this, a0); }
__attribute__((weak)) void daNpc_Co1_c::chg_anmAtr(u8 a0) { co1Pending_chg_anmAtr(this, a0); }
__attribute__((weak)) void daNpc_Co1_c::control_anmAtr() { co1Pending_control_anmAtr(this); }
__attribute__((weak)) void daNpc_Co1_c::anmAtr(u16 a0) { co1Pending_anmAtr(this, a0); }
__attribute__((weak)) bool daNpc_Co1_c::chk_talk() { return co1Pending_chk_talk(this); }
__attribute__((weak)) u8 daNpc_Co1_c::chk_partsNotMove() { return co1Pending_chk_partsNotMove(this); }
__attribute__((weak)) u16 daNpc_Co1_c::next_msgStatus(be<u32>* a0) { return co1Pending_next_msgStatus(this, a0); }
__attribute__((weak)) u32 daNpc_Co1_c::getMsg_CO1_0() { return co1Pending_getMsg_CO1_0(this); }
__attribute__((weak)) u32 daNpc_Co1_c::getMsg() { return co1Pending_getMsg(this); }
__attribute__((weak)) u8 daNpc_Co1_c::chkAttention() { return co1Pending_chkAttention(this); }
__attribute__((weak)) BOOL daNpc_Co1_c::wait_1() { return co1Pending_wait_1(this); }
__attribute__((weak)) BOOL daNpc_Co1_c::wait_2() { return co1Pending_wait_2(this); }
__attribute__((weak)) BOOL daNpc_Co1_c::wakeup() { return co1Pending_wakeup(this); }
__attribute__((weak)) BOOL daNpc_Co1_c::talk_1() { return co1Pending_talk_1(this); }
__attribute__((weak)) BOOL daNpc_Co1_c::toru_1() { return co1Pending_toru_1(this); }
__attribute__((weak)) BOOL daNpc_Co1_c::read_1() { return co1Pending_read_1(this); }
__attribute__((weak)) BOOL daNpc_Co1_c::modoru() { return co1Pending_modoru(this); }
__attribute__((weak)) BOOL daNpc_Co1_c::wait_action1(void* a0) { return co1Pending_wait_action1(this, a0); }

// HD: sole owner of TU data for both split source specs.
const gc_tu_data gc_tu = {
    .safeString = 0x10019380,
    .safeStringAssure = 0x0222A8F8,
    .objAcchDt = {0x10019398, 0x100193A8},
    .headerStatics = {0x10466CA4, 0x101BDEB0, 0x10466CE8},
};
static daNpc_Co1_HIO_c l_HIO;
static u32 l_btpResID[7] = {dRes_ID_CO_BTP_FUAN_e, dRes_ID_CO_BTP_KIRI_e, dRes_ID_CO_BTP_NIKO_e, dRes_ID_CO_BTP_OKORI_e, dRes_ID_CO_BTP_NAGAME_e, dRes_ID_CO_BTP_OKIRU_e, dRes_ID_CO_BTP_MIGAMAE_e};
static u32 l_bckResID[11] = {dRes_ID_CO_BCK_CO_WAIT01_e, dRes_ID_CO_BCK_CO_WAIT02_e, dRes_ID_CO_BCK_CO_MIGAMAE_e, dRes_ID_CO_BCK_CO_OKIRU_e, dRes_ID_CO_BCK_CO_READ_e, dRes_ID_CO_BCK_CO_TALK01_e, dRes_ID_CO_BCK_CO_TALK02_e, dRes_ID_CO_BCK_CO_SWAIT01_e, dRes_ID_CO_BCK_CO_UKETORI_e, dRes_ID_CO_BCK_CO_OKIRU_e, dRes_ID_CO_BCK_CO_MIGAMAE_e};
static u32 l_pearlBckResID[11] = {dRes_ID_CO_BCK_COPEAL_WAIT01_e, dRes_ID_CO_BCK_COPEAL_WAIT02_e, dRes_ID_CO_BCK_COPEAL_MIGAMAE_e, dRes_ID_CO_BCK_COPEAL_OKIRU_e, dRes_ID_CO_BCK_COPEAL_READ_e, dRes_ID_CO_BCK_COPEAL_TALK01_e, dRes_ID_CO_BCK_COPEAL_TALK02_e, dRes_ID_CO_BCK_COPEAL_SWAIT01_e, dRes_ID_CO_BCK_COPEAL_UKETORI_e, dRes_ID_CO_BCK_COPEAL_OKIRU_e, dRes_ID_CO_BCK_COPEAL_MIGAMAE_e};
static u32 l_heapSize[1] = {160480};
static daNpc_Co1_c::anm_prm_c l_stateAnmPrm[8] = {
    {-1, -1, 0, 0.0f, 0.0f, -1},
    {1, 4, 0, 8.0f, 1.0f, 2},
    {-1, -1, 0, 0.0f, 0.0f, -1},
    {3, 5, 0, 8.0f, 1.0f, 0},
    {7, 0, 0, 8.0f, 1.0f, 2},
    {-1, -1, 0, 0.0f, 0.0f, -1},
    {4, 4, 0, 8.0f, 1.0f, 2},
    {10, 6, 0, 8.0f, -1.0f, 0}
};
static daNpc_Co1_c::anm_prm_c l_numberAnmPrm[12] = {
    {0, 0, 0, 8.0f, 1.0f, 2},
    {1, 4, 0, 8.0f, 1.0f, 2},
    {2, 6, 0, 8.0f, 1.0f, 0},
    {3, 5, 0, 8.0f, 1.0f, 0},
    {4, 4, 0, 8.0f, 1.0f, 2},
    {5, 0, 0, 8.0f, 1.0f, 2},
    {6, 3, 0, 8.0f, 1.0f, 2},
    {7, 4, 0, 8.0f, 1.0f, 2},
    {8, 0, 0, 8.0f, 1.0f, 0},
    {9, 5, 0, 8.0f, -1.0f, 0},
    {10, 6, 0, 8.0f, -1.0f, 0},
    {7, 0, 0, 8.0f, 1.0f, 2}
};
static daNpc_Co1_c::anm_prm_c l_attributeAnmPrm[14] = {
    {0, 0, 0, 8.0f, 1.0f, 2},
    {1, 4, 0, 8.0f, 1.0f, 2},
    {3, 5, 0, 8.0f, 1.0f, 0},
    {8, 0, 0, 8.0f, 1.0f, 0},
    {5, 0, 0, 8.0f, 1.0f, 2},
    {6, 3, 0, 8.0f, 1.0f, 2},
    {7, 4, 0, 8.0f, 1.0f, 2},
    {2, 6, 0, 8.0f, 1.0f, 0},
    {10, 6, 0, 8.0f, -1.0f, 0},
    {9, 5, 0, 8.0f, -1.0f, 0},
    {4, 4, 0, 8.0f, 1.0f, 2},
    {5, 4, 0, 8.0f, 1.0f, 2},
    {5, 0, 0, 8.0f, 1.0f, 2},
    {6, 3, 0, 8.0f, 1.0f, 2}
};


static cXyz l_eyeOffset;
static u32 l_eyeOffsetGuard;
static u32 l_debugColorGuard;
static u32 l_debugPearlColorGuard;
static u32 l_debugMorfColorGuard;
static u8 l_debugColor[4] = {0, 0, 0, 0};
static u8 l_debugColorInitial[4] = {255, 0, 0, 128};
static u8 l_debugPearlColor[4] = {0, 0, 0, 0};
static u8 l_debugPearlColorInitial[4] = {0, 0, 255, 128};
static u8 l_debugMorfColor[4] = {0, 0, 0, 0};
static u8 l_debugMorfColorInitial[4] = {0, 255, 0, 128};
static const char* l_eventNames[3] = {"Prl_Flw", "Contact", "Red_Ltr"};
static const char* l_cutNames[1] = {"ACTION"};

static Co1HIOParameters l_hioParameters = {8192, 9000, -8192, -9000, 0, 0, 0, 0, 1024, 1024, 91.0f, 0, 0, 255, 150, 0, 40.0f, 20.0f, 512, {0, 64}};

static Co1ActionWords l_waitAction = {0x0000FFFF, 0x0222A568}; // wait_action1

/* WWHD glue (tools/verify/autoglue.py)
data l_waitAction 10019350
data l_hioParameters 101BDE84
data l_eyeOffset 10466CF8 nocheck # BSS, explicit verified first-use initialization
data l_eyeOffsetGuard 10466D04 nocheck # BSS, explicit verified first-use initialization
data l_debugColorGuard 101FDA50 nocheck # BSS, explicit verified first-use initialization
data l_debugPearlColorGuard 101FDAC0 nocheck # BSS, explicit verified first-use initialization
data l_debugMorfColorGuard 101FDA48 nocheck # BSS, explicit verified first-use initialization
data l_debugColor 101FEBF4 nocheck # BSS, memcpy on first use
data l_debugColorInitial 10019370
data l_debugPearlColor 101FEBF8 nocheck # BSS, memcpy on first use
data l_debugPearlColorInitial 10019374
data l_debugMorfColor 101FEBEC nocheck # BSS, memcpy on first use
data l_debugMorfColorInitial 10019378
str 10019358 "Prl_Flw"
str 10019360 "Contact"
str 10019368 "Red_Ltr"
data l_eventNames 101BDC50
str 10019608 "ACTION"
data l_cutNames 101BDDA0
data l_HIO 10466CB0 nocheck # BSS, initialized by the verified TU initializer
data l_btpResID 1001947C
data l_bckResID 100195A4
data l_pearlBckResID 100195D0
data l_heapSize 101BDC5C
data l_stateAnmPrm 101BDC60
data l_numberAnmPrm 101BDCE0
data l_attributeAnmPrm 101BDDA4
ext 022271C0 void co1Pending_nodeCo1Control(daNpc_Co1_c* self, J3DNode* a0, J3DModel* a1)
ext 022273A8 J3DModelData* co1Pending_create_Anm(daNpc_Co1_c* self)
ext 022275E0 u32 co1Pending_btpNum_toResID(daNpc_Co1_c* self, int a0)
ext 022275F4 BOOL co1Pending_setBtp(daNpc_Co1_c* self, u32 a0, int a1)
ext 022276E0 u32 co1Pending_iniTexPttrnAnm(daNpc_Co1_c* self, u32 a0)
ext 022276EC J3DModelData* co1Pending_create_prl_Anm(daNpc_Co1_c* self)
ext 02227884 BOOL co1Pending_setBtk(daNpc_Co1_c* self, u32 a0)
ext 02227964 bool co1Pending_create_itm_Mdl(daNpc_Co1_c* self)
ext 02227A04 BOOL co1Pending_CreateHeap(daNpc_Co1_c* self)
ext 02227C34 bool co1Pending_charDecide(daNpc_Co1_c* self, int a0)
ext 02227C48 BOOL co1Pending_set_action(daNpc_Co1_c* self, Co1ProcFunc* a0, void* a1)
ext 02227D74 bool co1Pending_init_CO1_0(daNpc_Co1_c* self)
ext 02227DFC void co1Pending_plyTexPttrnAnm(daNpc_Co1_c* self)
ext 02227F1C void co1Pending_setAttention(daNpc_Co1_c* self, u32 a0)
ext 02228014 void co1Pending_setMtx(daNpc_Co1_c* self, u32 a0)
ext 0222843C bool co1Pending_createInit(daNpc_Co1_c* self)
ext 02228598 cPhs_State co1Pending__create(daNpc_Co1_c* self)
ext 022286C4 BOOL co1Pending__delete(daNpc_Co1_c* self)
ext 02228728 void co1Pending_checkOrder(daNpc_Co1_c* self)
ext 02228848 u8 co1Pending_demo(daNpc_Co1_c* self)
ext 022289FC s32 co1Pending_isEventEntry(daNpc_Co1_c* self)
ext 02228A3C u32 co1Pending_setAnm_tex(daNpc_Co1_c* self, s8 a0)
ext 02228A5C u32 co1Pending_anmNum_toResID(daNpc_Co1_c* self, int a0)
ext 02228A70 u32 co1Pending_anmNum_toResID_prl(daNpc_Co1_c* self, int a0)
ext 02228A84 BOOL co1Pending_setAnm_anm(daNpc_Co1_c* self, daNpc_Co1_c::anm_prm_c* a0)
ext 02228B70 bool co1Pending_setAnm(daNpc_Co1_c* self)
ext 02228BF4 void co1Pending_setStt(daNpc_Co1_c* self, s8 a0)
ext 02228C38 void co1Pending_endEvent(daNpc_Co1_c* self)
ext 02228C78 void co1Pending_setAnm_NUM(daNpc_Co1_c* self, int a0, int a1)
ext 02228CE4 void co1Pending_eInit_MDR_(daNpc_Co1_c* self)
ext 02228D20 void co1Pending_eInit_RED_LTR_(daNpc_Co1_c* self)
ext 02228D2C void co1Pending_event_actionInit(daNpc_Co1_c* self, int a0)
ext 02228DDC u32 co1Pending_eMove_MDR_(daNpc_Co1_c* self)
ext 02228E34 u32 co1Pending_eMove_RED_LTR_(daNpc_Co1_c* self)
ext 02228EB8 u32 co1Pending_event_action(daNpc_Co1_c* self)
ext 02228EFC void co1Pending_privateCut(daNpc_Co1_c* self, int a0)
ext 02228FD0 void co1Pending_lookBack(daNpc_Co1_c* self)
ext 02229210 void co1Pending_event_proc(daNpc_Co1_c* self, int a0)
ext 02229378 void co1Pending_eventOrder(daNpc_Co1_c* self)
ext 022293E8 void co1Pending_setCollision_SP_(daNpc_Co1_c* self)
ext 02229558 BOOL co1Pending__execute(daNpc_Co1_c* self)
ext 02229738 BOOL co1Pending__draw(daNpc_Co1_c* self)
ext 02229904 void co1Pending_set_target(daNpc_Co1_c* self, int a0)
ext 02229A38 void co1Pending_setAnm_ATR(daNpc_Co1_c* self, int a0)
ext 02229AA8 void co1Pending_chg_anmAtr(daNpc_Co1_c* self, u8 a0)
ext 02229B74 void co1Pending_control_anmAtr(daNpc_Co1_c* self)
ext 02229C90 void co1Pending_anmAtr(daNpc_Co1_c* self, u16 a0)
ext 02229D58 bool co1Pending_chk_talk(daNpc_Co1_c* self)
ext 02229DD8 u8 co1Pending_chk_partsNotMove(daNpc_Co1_c* self)
ext 02229E18 u16 co1Pending_next_msgStatus(daNpc_Co1_c* self, be<u32>* a0)
ext 02229F00 u32 co1Pending_getMsg_CO1_0(daNpc_Co1_c* self)
ext 02229FEC u32 co1Pending_getMsg(daNpc_Co1_c* self)
ext 0222A024 u8 co1Pending_chkAttention(daNpc_Co1_c* self)
ext 0222A0AC BOOL co1Pending_wait_1(daNpc_Co1_c* self)
ext 0222A130 BOOL co1Pending_wait_2(daNpc_Co1_c* self)
ext 0222A1DC BOOL co1Pending_wakeup(daNpc_Co1_c* self)
ext 0222A218 BOOL co1Pending_talk_1(daNpc_Co1_c* self)
ext 0222A450 BOOL co1Pending_toru_1(daNpc_Co1_c* self)
ext 0222A4C4 BOOL co1Pending_read_1(daNpc_Co1_c* self)
ext 0222A500 BOOL co1Pending_modoru(daNpc_Co1_c* self)
ext 0222A568 BOOL co1Pending_wait_action1(daNpc_Co1_c* self, void* a0)
ext 026067F4 void* co1_getIDResource(dRes_control_c*, SafeString*, s32)
ext 027DF9B0 s8 co1_findJointName(u32, const char*)
ext 025E789C s32 co1_initBtp(void*, J3DModelData*, void*, s32, s32, f32, s32, s32, u32, s32)
ext 025E7CE0 s32 co1_initBtk(void*, J3DModelData*, void*, s32, s32, f32, s32, s32, u32, s32)
ext 02055B64 s16 co1_decrementTimer(be<s16>*)
ext 024EF130 s8 co1_getRoomId(dBgS*, void*)
ext 024EEEB8 u8 co1_getPolyColor(dBgS*, void*)
ext 022275F4 u32 co1_setBtpResult(daNpc_Co1_c*, u32, s32)
ext 025A1458 void co1_constructNpc(daNpc_Co1_c*)
ext 025E7C6C void co1_constructBtk(void*)
ext 025E7820 void co1_constructBtp(void*)
ext 0259F740 void co1_constructEventCut(dNpc_EventCut_c*)
ext 02526E70 void* co1_getDemoActor(u32, u32)
ext 02527828 u32 co1_getDemoBtp(void*, const char*)
ext 02527028 BOOL co1_setDemoData(daNpc_Co1_c*, s32, mDoExt_McaMorf*, const char*, s32, s32, s32, s32)
ext 02542EDC s8 co1_getActionIndex(dEvent_manager_c*, s32, u32, s32, BOOL, s32)
ext 025F795C u32 co1_getMessageStatus(u32)
ext 024EDFCC bool co1_lockonTruth(dAttention_c*)
ext 024EC8D0 fopAc_ac_c* co1_lockonTarget(dAttention_c*, s32)
ext 024EE464 fopAc_ac_c* co1_actionTarget(dAttention_c*, s32)
ext 0259D54C void co1_playerEyePosition(cXyz*, f32)
ext 0259DED0 void co1_lookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8)
ext 02018034 void co1_destroyCircle(u32, s32)
ext 025D50BC void co1_destroyActor(daNpc_Co1_c*, s32)
*/
