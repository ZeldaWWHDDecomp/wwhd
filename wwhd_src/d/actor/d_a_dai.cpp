/* Merchant display pedestal. GameCube source style with verified HD layout/ABI differences. */
#include "d/actor/d_a_dai.h"
const gc_tu_data gc_tu = {.objAcchDt = {0x1000CBDC, 0x1000CBEC}};
static u16 m_savelabel[256] = {}; // HD: byte-indexed original table; reads retain the full index span.
static u8 mNowDaizaNum;
static u8 mNowItemNum;
static u8 l_cyl_src[0x44] = {}; // Original collision source record; shared dCcD_SrcCyl is opaque.
static f32 l_clipperFar;
static f32 l_zero = 0.0f;
static dai_resourceControlPointer_c l_resourceControl;
static u32 dai_saved(u8 id) { return dai_readEventRegister(dComIfGs_getEvent(), m_savelabel[id]); }
static dSv_player_bag_item_c* dai_bag() { return &dComIfGs_info()->mSavedata.mPlayer.mBagItem; }
static fopAc_ac_c* dai_findActor(u32 id) {
    if (id == fpcM_ERROR_PROCESS_ID_e) return nullptr;
    gabi::Local<be<u32>> argument;
    *argument = id;
    gabi::Local<dai_linkage_c> callerLinkage; // HD: preserve this caller's GHS stack layout.
    return static_cast<fopAc_ac_c*>(fopAcIt_Judge(0x025E1234, argument));
}
static u32 dai_grabActorID(fopAc_ac_c* player) {
    dai_playerVtable_c* table = gabi::at<dai_playerVtable_c>(player->__vtbl);
    return gabi::call_ptr<u32>(table->mGetGrabActorID, player);
}
/* 02115DDC */
BOOL daDai_c::CreateHeap() {
    gabi::Local<dai_resourceName_c> name;
    gabi::Local<dai_linkage_c> callerLinkage;
    name->mpText.v = gabi::guest_addr("Fdai");
    name->mVtable = 0x1000CBB4;
    J3DModelData* data = dai_getObjectResource(l_resourceControl.mpValue, name, 17);
    if (!data) dai_assert("d_a_dai.cpp", 0x1A2, "modelData != NULL");
    J3DModel* model = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
    mpModel = model;
    return model != nullptr;
}
/* 02115E78 */
static BOOL CheckCreateHeap(fopAc_ac_c* actor) { return static_cast<daDai_c*>(actor)->CreateHeap(); }
/* 02115E7C */
s16 daDai_c::XyCheckCB(s32 button) {
    u8 item = dComIfGp_getSelectItem(button);
    if (dai_isDaizaItem(item) && !dai_saved(mSaveID)) { m84A = item; return 1; }
    return 0;
}
/* 02115F1C */
static s16 daDai_XyCheckCB(void* actor, int button) { return static_cast<daDai_c*>(actor)->XyCheckCB(button); }
/* 02115F20 */
s16 daDai_c::XyEventCB(s32 button) {
    u8 item = dComIfGp_getSelectItem(button);
    return dai_isDaizaItem(item) ? s16(mEvtDaiItemIdx) : s16(mEvtDefaultTalkIdx);
}
/* 02115F78 */
static s16 daDai_XyEventCB(void* actor, int button) { return static_cast<daDai_c*>(actor)->XyEventCB(button); }
/* 02115F7C */
void daDai_c::set_mtx() {
    f32 sx = scale.x, sy = scale.y;
    J3DModel* model = mpModel;
    f32 sz = scale.z;
    model->mBaseScale.x = sx;
    model->mBaseScale.y = sy;
    model->mBaseScale.z = sz;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    // HD: all twelve matrix loads precede the model reload and stores.
    f32 values[12];
    for (u32 i = 0; i < 12; ++i) values[i] = mDoMtx_stack_c::get()->m[i / 4][i % 4];
    model = mpModel;
    for (u32 i = 0; i < 12; ++i) model->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
}
/* 02116054 */
void daDai_c::CreateInit() {
    J3DModel* model = mpModel;
    fopAcM_SetMtx(this, model ? &model->mBaseTransformMtx : nullptr);
    fopAcM_setCullSizeBox(this, -50.0f, 0.0f, -50.0f, 50.0f, 200.0f, 50.0f);
    mStts.Init(255, 255, this);
    mCyl.Set(reinterpret_cast<const dCcD_SrcCyl*>(l_cyl_src));
    mCyl.SetStts(&mStts);
    f32 far = l_clipperFar;
    if (far > 1.0f) fopAcM_setCullSizeFar(this, 5000.0f / far);
    set_mtx();
    u16 y = current.angle.y, z = current.angle.z, x = current.angle.x;
    shape_angle.x = x;
    shape_angle.y = y;
    shape_angle.z = z;
    eventInfo.setXyCheckCB(daDai_XyCheckCB);
    eventInfo.setXyEventCB(daDai_XyEventCB);
    attention_info.flags = u32(attention_info.flags) | fopAc_Attn_TALKFLAG_CHECK_e | fopAc_Attn_ACTION_SPEAK_e;
    mEvtDaiItemIdx = dComIfGp_evmng_getEventIdx("DAI_ITEM");
    mEvtDefaultTalkIdx = dComIfGp_evmng_getEventIdx("DEFAULT_TALK");
    u32 parameters = fopAcM_GetParam(this);
    m850 = fpcM_ERROR_PROCESS_ID_e;
    mSaveID = u8(parameters);
    if (dai_saved(u8(parameters))) {
        u32 item = dai_saved(mSaveID);
        fopAc_ac_c* created = dai_fastCreate(fpcNm_STANDITEM_e, item, &current.pos, fopAcM_GetRoomNo(this), &current.angle, nullptr, -1, nullptr, 0);
        m850 = fopAcM_GetID(created);
        mNowItemNum = u8(mNowItemNum + 1);
    }
    mNowDaizaNum = u8(mNowDaizaNum + 1);
}
/* 02116258 */
cPhs_State daDai_c::_create() {
    fopAcM_ct(this, daDai_c);
    if (!checkItemGet(dItemNo_DELIVERY_BAG_e, 1)) return cPhs_ERROR_e;
    cPhs_State phase = dComIfG_resLoad(&mPhsFdai, "Fdai");
    if (phase != cPhs_COMPLEATE_e) return phase;
    cPhs_State cloth = dComIfG_resLoad(&mPhsCloth, "Cloth");
    if (cloth != cPhs_COMPLEATE_e) return cloth;
    if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0x4C0)) return cPhs_ERROR_e;
    CreateInit();
    return phase;
}
/* 021163BC */
static cPhs_State daDai_Create(daDai_c* actor) { return actor->_create(); }
/* 021163C0 */
BOOL daDai_c::_delete() {
    dComIfG_resDelete(&mPhsFdai, "Fdai");
    dComIfG_resDelete(&mPhsCloth, "Cloth");
    return TRUE;
}
/* 0211640C */
static BOOL daDai_Delete(daDai_c* actor) { return actor->_delete(); }
/* 02116410 */
BOOL daDai_c::_draw() {
    g_env_light.settingTevStruct(0, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    mDoExt_modelUpdateDL(mpModel, 0);
    return TRUE;
}
/* 0211646C */
static BOOL daDai_Draw(daDai_c* actor) { return actor->_draw(); }
/* 02116470 */
void daDai_c::checkOrder() {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if (!eventInfo.checkCommandTalk()) return;
    if (!dComIfGp_event_chkTalkXY()) { m84C = 1; return; }
    if (!dai_isDaizaItem(u8(m84A)) || dai_saved(mSaveID)) m84C = 1;
    s16 event = mEvtDaiItemIdx;
    if (dComIfGp_evmng_startCheck(event)) {
        u32 id = dai_grabActorID(player);
        if (id != fpcM_ERROR_PROCESS_ID_e) {
            m850 = dai_grabActorID(player);
            fopAc_ac_c* item = dai_findActor(m850);
            if (item) m84B = static_cast<daStandItem_c*>(item)->mItemNo;
        }
    }
    event = mEvtDaiItemIdx;
    if (dComIfGp_evmng_endCheck(event)) {
        fopAc_ac_c* item = dai_findActor(m850);
        if (item) {
            // HD: preserve the original interleaved word copy, including float NaN payloads.
            dai_positionWords_c* dst = reinterpret_cast<dai_positionWords_c*>(&item->current.pos);
            dai_positionWords_c* src = reinterpret_cast<dai_positionWords_c*>(&current.pos);
            for (u32 i = 0; i < 3; ++i) dst->mWords[i] = u32(src->mWords[i]);
        }
        u16 label = m_savelabel[u8(mSaveID)];
        u8 placed = m84B;
        dComIfGs_setEventReg(label, placed);
        dComIfGs_setReserveItemEmpty();
        mNowItemNum = u8(mNowItemNum + 1);
        dComIfGp_event_reset();
    }
}
/* 02116648 */
void daDai_c::proc() {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<cXyz> difference, horizontal;
    gabi::Local<dai_linkage_c> callerLinkage;
    dai_subtractVector(&player->current.pos, difference, &current.pos);
    horizontal->x = difference->x;
    horizontal->y = l_zero;
    horizontal->z = difference->z;
    f32 square = PSVECSquareMag(horizontal);
    // HD: keep the double result for the 100.0 comparison; GC absXZ rounds to f32.
    f64 distance = dai_sqrt(f64(square));
    f32 dy = player->current.pos.y - current.pos.y;
    m844 = 0;
    if (distance < 100.0 && __builtin_fabsf(dy) < 10.0f) {
        s16 angle = cLib_targetAngleY(&player->current.pos, &current.pos);
        if (cLib_distanceAngleS(angle, player->shape_angle.y) < 12000) m844 = 1;
    }
    if (m84C && talk(1) == fopMsgStts_BOX_CLOSED_e) {
        dComIfGp_event_reset();
        m84C = 0;
    }
}
/* 02116750 */
void daDai_c::eventOrder() {
    if (m844 == 1) eventInfo.onCondition(dEvtCnd_CANTALK_e | dEvtCnd_CANTALKITEM_e);
}
/* 0211676C */
BOOL daDai_c::_execute() {
    checkOrder(); proc(); eventOrder();
    mCyl.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mCyl);
    return TRUE;
}
/* 021167CC */
static BOOL daDai_Execute(daDai_c* actor) { return actor->_execute(); }
enum dai_message_e { MSG_PLACED = 0xF0D, MSG_CANNOT_REMOVE = 0xF0E, MSG_REMOVED = 0xF0F, MSG_EMPTY = 0xF11, MSG_EMPTY_NEXT = 0xF12, MSG_PLACE_ITEM = 0xF13 };
/* 021167D0 */
u32 daDai_c::getMsg() {
    if (dComIfGp_event_chkTalkXY()) return MSG_PLACE_ITEM;
    return dai_saved(mSaveID) ? MSG_PLACED : MSG_EMPTY;
}
/* 02116848 */
u16 daDai_c::next_msgStatus(be<u32>* message) {
    u32 current = *message;
    msg_class* manager = mpCurrMsg;
    if (current == MSG_EMPTY) { *message = MSG_EMPTY_NEXT; return fopMsgStts_MSG_CONTINUES_e; }
    if (current != MSG_PLACED || manager->mSelectNum) return fopMsgStts_MSG_ENDS_e;
    if (dai_checkReserveEmpty(dai_bag()) && dai_getRotenItemCount() < 3) {
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        mDoAud_seStart(0x2871, &eyePos, 0, reverb);
        *message = MSG_REMOVED;
        fopAc_ac_c* item = dai_findActor(m850);
        if (item) fopAcM_delete(item);
        u8 oldItem = u8(dai_saved(mSaveID));
        u16 label = m_savelabel[u8(mSaveID)];
        dComIfGs_setEventReg(label, 0);
        dai_setReserve(dai_bag(), oldItem);
        mNowItemNum = u8(mNowItemNum - 1);
        m84A = 0;
    } else *message = MSG_CANNOT_REMOVE;
    return fopMsgStts_MSG_CONTINUES_e;
}
/* 021169A8 */
static void __sinit_d_a_dai_cpp() { sinit_header_statics_rodata(0x10462F90, 0x101B3DF4, 0x10462F9C, 0x1000CC58); }
/* 02116A3C */
static void daDai_safeStringDestroy(void* object, s32 flags) { if (object && (u32(flags) & 1)) operator_delete(object); }
/* 02116A58 */
static void daDai_destroy(daDai_c* actor, s32 flags) {
    if (!actor) return;
    __dt(&actor->mCyl);
    __dt(&actor->mStts);
    __dt(static_cast<fopNpc_npc_c*>(actor));
    if (u32(flags) & 1) operator_delete(actor);
}
/* 02116A50 */
static BOOL daDai_IsDelete(daDai_c* actor) { return TRUE; }
/* 02116B0C */
static void daDai_assureTermination(void* object, s32 flags) {}

/* WWHD glue
data m_savelabel 1004BD4C nocheck
data mNowDaizaNum 10475655 nocheck
data mNowItemNum 10475656 nocheck
data l_cyl_src 101B3DB0 nocheck
data l_clipperFar 1048D04C nocheck
data l_zero 1000CC24
data l_resourceControl 101F4F28 nocheck
str 1000CC68 "Fdai"
str 1000CC60 "Cloth"
str 1000CBFC "d_a_dai.cpp" in 02115DDC
str 1000CC08 "modelData != NULL" in 02115DDC
str 1000CC34 "DAI_ITEM" in 02116054
str 1000CC40 "DEFAULT_TALK" in 02116054
ext 026066C4 J3DModelData* dai_getObjectResource(void* control, dai_resourceName_c* name, s32 index)
ext 0273AA24 void dai_assert(const char* file, u32 line, const char* condition)
ext 02550FAC BOOL dai_isDaizaItem(u32 item)
ext 025B8BB0 u32 dai_readEventRegister(dSv_event_c* event, u16 label)
ext 025D5928 fopAc_ac_c* dai_fastCreate(s32 name, u32 parameters, cXyz* position, s32 room, csXyz* angle, void* scale, s32 subtype, void* callback, u32 user)
ext 0201ADE0 void dai_subtractVector(const cXyz* first, cXyz* out, const cXyz* second)
ext 028F4384 f64 dai_sqrt(f64 value)
ext 025B75A4 BOOL dai_checkReserveEmpty(dSv_player_bag_item_c* bag)
ext 025B75AC void dai_setReserve(dSv_player_bag_item_c* bag, u32 item)
ext 02550F38 s32 dai_getRotenItemCount()
*/
