/* d_a_npc_ds1: member functions of the translation unit as plain guest calls (so the decompiled
 * parts can call each other naturally). Weak: a decompiled definition in a part replaces it.
 */
#include "d/actor/d_a_npc_ds1.h"

const gc_tu_data gc_tu = {.safeString = 0x1001997C, .objAcch = {0x100199A4, 0x100199C4, 0x100199B4}, .objAcchDt = {0x100199B4, 0x100199C4}, .headerStatics = {0x10466EE4, 0x101BE21C, 0x10466F20}};
const Ds1TuData ds1_tu = {0x100199D4, 0x100199E4, 0x100199F4, 0x10050788, 0x10019994, 0x022311F4, 0x022312F8, 0x0223164C, 0x022328E4};
static daNpc_Ds1_HIO_c l_HIO;
static Ds1Word l_messageManager;
static Ds1MessageID l_messageID;
static Ds1SoundPositions l_soundPositions;
static dCcD_SrcCyl l_cylinderSource;
static Ds1BtpTable l_btpResources;
static Ds1AnmIntTable l_anmLoop;
static Ds1AnmIntTable l_anmResources;
static Ds1AnmFloatTable l_anmMorph;
static Ds1AnmFloatTable l_anmSpeed;
static Ds1CutNames l_cutNames;
static Ds1Colors l_particlePrmColors;
static Ds1Colors l_particleEnvColors;
static Ds1Word l_prmColorGuard;
static Ds1Word l_envColorGuard;
static Mtx34 l_currentMatrix;
static Mtx34 l_identityMatrix;
static Ds1Word l_currentModel;

#define W __attribute__((weak))
W BOOL daNpc_Ds1_c::wait_action(void* a) { return ds1Guest_wait_action(this, a); }
W BOOL daNpc_Ds1_c::getdemo_action(void* a) { return ds1Guest_getdemo_action(this, a); }
W BOOL daNpc_Ds1_c::dummy_action(void* a) { return ds1Guest_dummy_action(this, a); }
W BOOL daNpc_Ds1_c::event_action(void* a) { return ds1Guest_event_action(this, a); }
W s16 daNpc_Ds1_c::XyEventCB(int i) { return ds1Guest_XyEventCB(this, i); }
W BOOL daNpc_Ds1_c::initTexPatternAnm(u8 m) { return ds1Guest_initTexPatternAnm(this, m); }
W BOOL daNpc_Ds1_c::CreateHeap() { return ds1Guest_CreateHeap(this); }
W void daNpc_Ds1_c::RoomEffectSet() { ds1Guest_RoomEffectSet(this); }
W BOOL daNpc_Ds1_c::CreateInit() { return ds1Guest_CreateInit(this); }
W cPhs_State daNpc_Ds1_c::_create() { return ds1Guest__create(this); }
W void daNpc_Ds1_c::RoomEffectDelete() { ds1Guest_RoomEffectDelete(this); }
W BOOL daNpc_Ds1_c::_delete() { return ds1Guest__delete(this); }
W void daNpc_Ds1_c::playTexPatternAnm() { ds1Guest_playTexPatternAnm(this); }
W void daNpc_Ds1_c::talkInit() { ds1Guest_talkInit(this); }
W void daNpc_Ds1_c::checkOrder() { ds1Guest_checkOrder(this); }
W void daNpc_Ds1_c::eventOrder() { ds1Guest_eventOrder(this); }
W void daNpc_Ds1_c::setCollision() { ds1Guest_setCollision(this); }
W BOOL daNpc_Ds1_c::_execute() { return ds1Guest__execute(this); }
W BOOL daNpc_Ds1_c::_draw() { return ds1Guest__draw(this); }
W void daNpc_Ds1_c::setAnm(s8 i, f32 m) { ds1Guest_setAnm(this, i, m); }
W u32 daNpc_Ds1_c::setTexAnm(s8 i) { return ds1Guest_setTexAnm(this, i); }
W void daNpc_Ds1_c::setAnmFromMsgTag() { ds1Guest_setAnmFromMsgTag(this); }
W bool daNpc_Ds1_c::chkAttention(cXyz* p, s16 a) { return ds1Guest_chkAttention(this, p, a); }
W u16 daNpc_Ds1_c::next_msgStatus(be<u32>* p, be<u32>* q) { return ds1Guest_next_msgStatus(this, p, q); }
W u32 daNpc_Ds1_c::getMsg() { return ds1Guest_getMsg(this); }
W u16 daNpc_Ds1_c::normal_talk() { return ds1Guest_normal_talk(this); }
W u16 daNpc_Ds1_c::shop_talk() { return ds1Guest_shop_talk(this); }
W u16 daNpc_Ds1_c::talk() { return ds1Guest_talk(this); }
W void daNpc_Ds1_c::setAttention(u32 b) { ds1Guest_setAttention(this, b); }
W void daNpc_Ds1_c::lookBack() { ds1Guest_lookBack(this); }
W BOOL daNpc_Ds1_c::wait01() { return ds1Guest_wait01(this); }
W BOOL daNpc_Ds1_c::talk01() { return ds1Guest_talk01(this); }
W BOOL daNpc_Ds1_c::evn_talk_init(int i) { return ds1Guest_evn_talk_init(this, i); }
W BOOL daNpc_Ds1_c::evn_continue_talk_init(int i) { return ds1Guest_evn_continue_talk_init(this, i); }
W BOOL daNpc_Ds1_c::evn_ItemModel_init(int i) { return ds1Guest_evn_ItemModel_init(this, i); }
W BOOL daNpc_Ds1_c::evn_head_swing_init(int i) { return ds1Guest_evn_head_swing_init(this, i); }
W BOOL daNpc_Ds1_c::evn_setAnm_init(int i) { return ds1Guest_evn_setAnm_init(this, i); }
W BOOL daNpc_Ds1_c::evn_move_pos_init(int i) { return ds1Guest_evn_move_pos_init(this, i); }
W BOOL daNpc_Ds1_c::evn_init_pos_init(int i) { return ds1Guest_evn_init_pos_init(this, i); }
W BOOL daNpc_Ds1_c::evn_jnt_lock_init(int i) { return ds1Guest_evn_jnt_lock_init(this, i); }
W BOOL daNpc_Ds1_c::evn_player_hide_init(int i) { return ds1Guest_evn_player_hide_init(this, i); }
W BOOL daNpc_Ds1_c::evn_talk() { return ds1Guest_evn_talk(this); }
W BOOL daNpc_Ds1_c::evn_Anm() { return ds1Guest_evn_Anm(this); }
W BOOL daNpc_Ds1_c::evn_move_pos() { return ds1Guest_evn_move_pos(this); }
W BOOL daNpc_Ds1_c::privateCut() { return ds1Guest_privateCut(this); }

/* WWHD glue
ext 028EFFD0 void ds1Guest_constructArray(daNpc_Ds1_childHIO_c*, s32, u32, u32)
ext 025D50BC void ds1Guest_Actor_destruct(daNpc_Ds1_c*, s32)
ext 025BEBB8 void ds1Guest_registerFigure(u32, daNpc_Ds1_c*, cXyz*, s16, f32, f32, f32)
ext 025BC760 void ds1Guest_ShopItems_construct(ShopItems_c*)
ext 025BBDF0 void ds1Guest_ShopCam_construct(ShopCam_action_c*)
ext 0259F740 void ds1Guest_EventCut_construct(dNpc_EventCut_c*)
ext 0259DAA0 void ds1Guest_JntCtrl_construct(dNpc_JntCtrl_c*)
ext 0259DA18 void ds1Guest_NpcHio_construct(dNpc_HIO_c*)
ext 025886D8 void ds1Guest_Stick_destruct(STControl*, s32)
ext 025447C8 u8 ds1Guest_eventAdvanced(void*, s32)
ext 02542EDC s32 ds1Guest_eventActionIndex(void*, s32, u32, s32, s32, s32)
ext 024EFD9C void ds1Guest_Acch_destruct(dBgS_Acch*, s32)
ext 0222FECC u32 ds1Guest_nextMessageStatusRegister(daNpc_Ds1_c*, be<u32>*, be<u32>*)
ext 0222FDBC u32 ds1Guest_attentionRegister(daNpc_Ds1_c*, cXyz*, s16)
ext 0222E124 f32 ds1Guest_gammaCorrect(u32)
ext 0222D37C BOOL ds1Guest_createDrugRegister(u32)
ext 02018034 void ds1Guest_Circle_destruct(void*, s32)
str 101BE240 "Ds"
data l_HIO 10466F30
data l_messageManager 101F4B5C
data l_messageID 10466EE0
data l_soundPositions 10466EF0
data l_cylinderSource 101BE138
data l_btpResources 10019970
data l_anmLoop 101BE17C
data l_anmResources 10019A04
data l_anmMorph 101BE1A4
data l_anmSpeed 101BE1CC
data l_cutNames 101BE1F4
data l_particlePrmColors 101BE120
data l_particleEnvColors 101BE12C
data l_prmColorGuard 10466F88
data l_envColorGuard 10466F8C
data l_currentMatrix 104B4868
data l_identityMatrix 101F48F0
data l_currentModel 104B462C
ext 025F795C u32 ds1Guest_fopMsgM_SearchByID(u32)
ext 025F7DB0 u32 ds1Guest_fopMsgM_messageSet(u32, u32, cXyz*)
ext 025F74D0 void ds1Guest_fopMsgM_setStatus(u32, u32)
ext 025DB58C void ds1Guest_fopMsgM_demoMsgFlagOn()
ext 020078BC BOOL ds1Guest_CPad_CHECK_TRIG_A(s32)
ext 025BBA1C u8 ds1Guest_dShop_BoughtErrorStatus(ShopItems_c*, s32, s32)
ext 025BB698 BOOL ds1Guest_dShop_now_triggercheck(STControl*, ShopItems_c*, be<u32>*, u32, u32)
ext 025BD22C u8 ds1Guest_ShopItems_getSelectItemNo(ShopItems_c*)
ext 025BCD90 void ds1Guest_ShopItems_hideSelectItem(ShopItems_c*)
ext 025BCE04 void ds1Guest_ShopItems_showItem(ShopItems_c*)
ext 025BCC7C void ds1Guest_ShopItems_Item_ZoomUp(ShopItems_c*, cXyz*)
ext 025BD278 u32 ds1Guest_ShopItems_getSelectItemBuyMsg(ShopItems_c*)
ext 025BD0EC void ds1Guest_ShopItems_getSelectItemPos(ShopItems_c*, cXyz*)
ext 025BCFA8 void ds1Guest_ShopItems_getSelectItemBasePos(ShopItems_c*, cXyz*)
ext 025BD31C void ds1Guest_ShopCursor_setPos(u32, cXyz*)
ext 025BD290 void ds1Guest_ShopCursor_anm_play(u32)
ext 0254DA50 BOOL ds1Guest_checkItemGet(u8, s32)
ext 0254DA38 void ds1Guest_execItemGet(u8)
ext 0201B47C bool ds1Guest_cXyz_normalizeRS(cXyz*)
ext 0252012C void ds1Guest_dComIfGp_setNextStage(const char*, s16, s8, s8, f32, u32, s32, s8)
ext 0259D54C void ds1Guest_dNpc_playerEyePos(cXyz*, f32)
ext 0259DED0 void ds1Guest_lookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8)
ext 027F2BF8 BOOL ds1Guest_J3DFrameCtrl_checkPass(J3DFrameCtrl*, f32)
ext 0222D958 BOOL ds1Guest_daNpc_Ds1_shopMsgCheck(u32)
ext 0222D37C BOOL ds1Guest_daNpc_Ds1_checkCreateDrugChuchu(u8)
ext 025D7DEC u32 ds1Guest_fopAcM_createItemForPresentDemo(cXyz*, s32, u8, s32, s32, csXyz*, cXyz*)
ext 0259F514 void ds1Guest_dNpc_HeadAnm_swing_horizone_init(void*, s16, s16, s16, s32)
ext 0259F36C void ds1Guest_dNpc_HeadAnm_swing_vertical_init(void*, s16, s16, s16, s32)
ext 0200F164 void ds1Guest_cLib_addCalcPos2(cXyz*, const cXyz*, f32, f32)
ext 027DF9B0 s8 ds1Guest_JUTNameTab_getIndex(u32, const char*)
ext 02055B64 s16 ds1Guest_cLib_calcTimer(be<s16>*)
ext 025E789C BOOL ds1Guest_mDoExt_btpAnm_init(void*, J3DModelData*, J3DAnmTexPattern*, s32, s32, f32, s32, s32, u32, s32)
ext 025E7820 void ds1Guest_mDoExt_btpAnm_ct(void*)
ext 025BBD7C u32 ds1Guest_ShopCursor_create(J3DModelData*, void*, f32)
ext 025BD338 void ds1Guest_ShopCursor_draw(u32)
ext 025BCF84 void ds1Guest_ShopItems_setItemSetDataList(ShopItems_c*)
ext 025BC810 void ds1Guest_ShopItems_createItem(ShopItems_c*, s32, s32)
ext 025BCCA4 void ds1Guest_ShopItems_Item_Move(ShopItems_c*)
ext 025BBE98 void ds1Guest_ShopCam_shop_cam_action_init(ShopCam_action_c*)
ext 025BC438 void ds1Guest_ShopCam_ds_normal_cam_action_init(ShopCam_action_c*)
ext 025BC714 void ds1Guest_ShopCam_move(ShopCam_action_c*)
ext 0259F7D4 void ds1Guest_dNpc_EventCut_setActorInfo(dNpc_EventCut_c*, const char*, fopAc_ac_c*)
ext 024EECAC u32 ds1Guest_dBgS_GetMtrlSndId(dBgS*, void*)
ext 024EF130 s8 ds1Guest_dBgS_GetRoomId(dBgS*, void*)
ext 024EEEB8 u8 ds1Guest_dBgS_GetPolyColor(dBgS*, void*)
ext 025E535C BOOL ds1Guest_McaMorf_play(mDoExt_McaMorf*, cXyz*, u32, s32)
ext 0259D79C void ds1Guest_dNpc_setAnm_2(mDoExt_McaMorf*, s32, f32, f32, s32, s32, const char*)
ext 025885C4 void ds1Guest_STControl_setWaitParm(STControl*, s16, s16, s16, s16, f32, f32, s16, s16)
ext 025885E8 void ds1Guest_STControl_init(STControl*)
ext 028F4560 f32 ds1Guest_std_powf(f32, f32)
ext 022311F4 BOOL ds1Guest_wait_action(daNpc_Ds1_c*, void*)
ext 022312F8 BOOL ds1Guest_getdemo_action(daNpc_Ds1_c*, void*)
ext 0223164C BOOL ds1Guest_dummy_action(daNpc_Ds1_c*, void*)
ext 022328E4 BOOL ds1Guest_event_action(daNpc_Ds1_c*, void*)
ext 0222D3FC s16 ds1Guest_XyEventCB(daNpc_Ds1_c*, int)
ext 0222D97C BOOL ds1Guest_initTexPatternAnm(daNpc_Ds1_c*, u8)
ext 0222DA90 BOOL ds1Guest_CreateHeap(daNpc_Ds1_c*)
ext 0222E1A4 void ds1Guest_RoomEffectSet(daNpc_Ds1_c*)
ext 0222E598 BOOL ds1Guest_CreateInit(daNpc_Ds1_c*)
ext 0222E8F0 cPhs_State ds1Guest__create(daNpc_Ds1_c*)
ext 0222EA18 void ds1Guest_RoomEffectDelete(daNpc_Ds1_c*)
ext 0222EA54 BOOL ds1Guest__delete(daNpc_Ds1_c*)
ext 0222EB1C void ds1Guest_playTexPatternAnm(daNpc_Ds1_c*)
ext 0222EBE0 void ds1Guest_talkInit(daNpc_Ds1_c*)
ext 0222EBEC void ds1Guest_checkOrder(daNpc_Ds1_c*)
ext 0222F050 void ds1Guest_eventOrder(daNpc_Ds1_c*)
ext 0222F100 void ds1Guest_setCollision(daNpc_Ds1_c*)
ext 0222F1BC BOOL ds1Guest__execute(daNpc_Ds1_c*)
ext 0222F5D0 BOOL ds1Guest__draw(daNpc_Ds1_c*)
ext 0222F9C0 void ds1Guest_setAnm(daNpc_Ds1_c*, s8, f32)
ext 0222FA9C u32 ds1Guest_setTexAnm(daNpc_Ds1_c*, s8)
ext 0222FAC4 void ds1Guest_setAnmFromMsgTag(daNpc_Ds1_c*)
ext 0222FDBC bool ds1Guest_chkAttention(daNpc_Ds1_c*, cXyz*, s16)
ext 0222FECC u16 ds1Guest_next_msgStatus(daNpc_Ds1_c*, be<u32>*, be<u32>*)
ext 02230364 u32 ds1Guest_getMsg(daNpc_Ds1_c*)
ext 02230420 u16 ds1Guest_normal_talk(daNpc_Ds1_c*)
ext 02230784 u16 ds1Guest_shop_talk(daNpc_Ds1_c*)
ext 022308E0 u16 ds1Guest_talk(daNpc_Ds1_c*)
ext 02230ADC void ds1Guest_setAttention(daNpc_Ds1_c*, u32)
ext 02230B24 void ds1Guest_lookBack(daNpc_Ds1_c*)
ext 02230F2C BOOL ds1Guest_wait01(daNpc_Ds1_c*)
ext 02230F8C BOOL ds1Guest_talk01(daNpc_Ds1_c*)
ext 02231668 BOOL ds1Guest_evn_talk_init(daNpc_Ds1_c*, int)
ext 02231710 BOOL ds1Guest_evn_continue_talk_init(daNpc_Ds1_c*, int)
ext 02231774 BOOL ds1Guest_evn_ItemModel_init(daNpc_Ds1_c*, int)
ext 022317DC BOOL ds1Guest_evn_head_swing_init(daNpc_Ds1_c*, int)
ext 0223188C BOOL ds1Guest_evn_setAnm_init(daNpc_Ds1_c*, int)
ext 02231D0C BOOL ds1Guest_evn_move_pos_init(daNpc_Ds1_c*, int)
ext 02231DC8 BOOL ds1Guest_evn_init_pos_init(daNpc_Ds1_c*, int)
ext 02231EBC BOOL ds1Guest_evn_jnt_lock_init(daNpc_Ds1_c*, int)
ext 02231F6C BOOL ds1Guest_evn_player_hide_init(daNpc_Ds1_c*, int)
ext 02232000 BOOL ds1Guest_evn_talk(daNpc_Ds1_c*)
ext 022322D0 BOOL ds1Guest_evn_Anm(daNpc_Ds1_c*)
ext 022325D0 BOOL ds1Guest_evn_move_pos(daNpc_Ds1_c*)
ext 02232698 BOOL ds1Guest_privateCut(daNpc_Ds1_c*)
*/
