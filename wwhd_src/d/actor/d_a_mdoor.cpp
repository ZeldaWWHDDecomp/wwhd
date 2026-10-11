/* Wooden and metal bars. WWHD behavior verified against the original. */
#include "d/actor/d_a_mdoor.h"

const gc_tu_data gc_tu = {.safeString = 0x100148F4,
                          .safeStringAssure = 0x021C2060,
                          .headerStatics = {0x10464FA0, 0x101BA408, 0x10464FAC}};
const char daMdoor_c::M_arcname[] = "Mdoor";
static MdoorActionNames action_table;
static u32 actionInitialized;
static MdoorActionTable l_action;
// HD: GHS copies the original action targets into a mutable guarded table.
// Keep guest target words so arbitrary initialized dispatch targets retain their full width.
static MdoorActionTable initial_actions;

/* 021C0EA0 */
u8 daMdoor_c::getShapeType() {
    return mParameters >> 24;
}
/* 021C0EAC */
void daMdoor_c::calcMtx() {
    f32 height = m2CC, y = current.pos.y, x = current.pos.x, z = current.pos.z;
    PSMTXTrans(mDoMtx_stack_c::get(), x, y + height, z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), home.angle.y);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
}
// SafeString payload remains above every nonleaf caller linkage save.
/* inlined into CreateHeap */
static void* objectResource(s32 index) {
    SafeString name;
    // HD: preserve the caller linkage space below the SafeString temporary.
    gabi::Local<u8[16]> callerLinkage;
    name.__vtbl = gc_tu.safeString;
    name.mStringTop = gabi::guest_str(daMdoor_c::M_arcname);
    return mdoorGetResource(dComIfG_resControl(), &name, index);
}
/* 021C0F70 */
BOOL daMdoor_c::CreateHeap() {
    bool second = getShapeType() == 1;
    s32 collisionIndex = second ? dRes_INDEX_MDOOR_DZB_MORI2_e : dRes_INDEX_MDOOR_DZB_MORI1_e;
    void* data =
        objectResource(second ? dRes_INDEX_MDOOR_BDL_MORI2_e : dRes_INDEX_MDOOR_BDL_MORI1_e);
    if (!data)
        JUT_ASSERT_fail("d_a_mdoor.cpp", 0x90, "modelData != NULL");
    mpModel = mDoExt_J3DModel__create((J3DModelData*)data, 0, 0x11020203);
    if (!gabi::ea(mpModel))
        return FALSE;
    mpBgW = new_dBgW();
    if (!gabi::ea(mpBgW))
        return FALSE;
    data = objectResource(collisionIndex);
    if (!data)
        return FALSE;
    calcMtx();
    J3DModel* model = mpModel;
    dBgW* background = mpBgW;
    return mdoorSetBackground(background, (cBgD_t*)data, cBgW_MOVE_BG_e,
                              J3DModel_getBaseTRMtx(model))
               ? FALSE
               : TRUE;
}
/* 021C10B8 */
static BOOL CheckCreateHeap(daMdoor_c* i_this) {
    return i_this->CreateHeap();
}
/* 021C10BC */
static BOOL daMdoor_actionWait(daMdoor_c* i_this) {
    return TRUE;
}
/* 021C10C4 */
void daMdoor_c::smokeInit() {
    s8 room = current.roomNo;
    auto play = dComIfGp_get();
    dPa_control_c* control = ((dComIfG_inf_c*)play)->play.mParticle;
    MdoorEmitter* emitter = (MdoorEmitter*)control->set(
        0, dPa_name::ID_AK_JT_ELEMENTSMOKE00, &current.pos, &shape_angle, nullptr, 0xAA,
        (dPa_levelEcallBack*)&m290, room, nullptr, nullptr, nullptr);
    if (emitter) {
        emitter->mRate = 16.0f;
        emitter->mSpread = 0.35f;
        for (u32 i = 0; i < 3; ++i) {
            emitter->mGlobalScale[i] = 2.0f;
            emitter->mGlobalScaleOut[i] = 2.0f;
        }
    }
}
static void debris(daMdoor_c* i_this) {
    dComIfGp_particle_set(0x81B2, &i_this->current.pos, &i_this->current.angle, nullptr, 0xFF,
                          nullptr, -1);
}
static void sound(daMdoor_c* i_this, u32 id, bool checkNull = false) {
    if (checkNull && (!gabi::ea(i_this) || !(u32)(gabi::ea(i_this) + 0x37C)))
        return;
    s8 room = i_this->current.roomNo;
    s32 reverb = dComIfGp_getReverb(room);
    mDoAud_seStart(id, &i_this->eyePos, 0, reverb);
}
static void cutEnd(daMdoor_c* i_this) {
    s32 staff = i_this->m2C0;
    dComIfGp_evmng_cutEnd(staff);
}
static void move(daMdoor_c* i_this) {
    i_this->calcMtx();
    i_this->mpBgW->Move();
}
/* 021C1174 */
void daMdoor_c::demoProc() {
    s32 staff = m2C0;

    u32 action = dComIfGp_evmng_getMyActIdx(staff, action_table.entries, 3, 0, 0);
    staff = m2C0;

    if (dComIfGp_evmng_getIsAddvance(staff)) {
        if (action == 1) {
            u16 flags = m2CA;
            speedF = 0.0f;
            m2CA = flags | 1;
            sound(this, JA_SE_OBJ_CAGE_CLOSE);
        } else if (action == 2) {
            speedF = 0.0f;
            if (getShapeType() != 1)
                debris(this);
            sound(this, JA_SE_OBJ_CAGE_OPEN);
        }
    }
    if (action == 1) {
        cLib_chaseF(&speedF, 60.0f, 6.0f);
        if (cLib_chaseF(&m2CC, 0.0f, speedF)) {
            cutEnd(this);
            u16 flags = m2CA;
            if (flags & 1) {
                m2CA = flags & 0xFFFE;
                if (getShapeType() != 1)
                    debris(this);
                smokeInit();
            }
        }
        move(this);
    } else if (action == 2) {
        cLib_chaseF(&speedF, 30.0f, 3.0f);
        if (cLib_chaseF(&m2CC, 280.0f, speedF))
            cutEnd(this);
        move(this);
    } else
        cutEnd(this);
}
/* 021C1418 */
static BOOL daMdoor_actionDemoWait(daMdoor_c* i_this) {
    s16 event = i_this->m2C6;

    if (dComIfGp_evmng_startCheck(event)) {
        i_this->mAction = (u8)((u8)i_this->mAction + 1);

        i_this->m2C0 = dComIfGp_evmng_getMyStaffId("Mori1", 0, 0);
        i_this->demoProc();
    }
    return TRUE;
}
/* 021C149C */
static BOOL daMdoor_actionDemo(daMdoor_c* i_this) {
    s16 event = i_this->m2C6;

    if (dComIfGp_evmng_endCheck(event))
        i_this->mAction = (u8)((u8)i_this->mAction + 1);
    else
        i_this->demoProc();
    return TRUE;
}
/* 021C1508 */
u8 daMdoor_c::getType() {
    return (mParameters >> 8) & 0xFF;
}
static void order(daMdoor_c* i_this) {
    s16 event = i_this->m2C6;
    u8 tool = i_this->m2C8;
    fopAcM_orderOtherEventId(i_this, event, tool, 0xFFFF, 0, 1);
}
/* 021C1514 */
static BOOL daMdoor_actionGenocide(daMdoor_c* i_this) {
    if (!mdoorSearchEnemy((s8)i_this->current.roomNo)) {
        s16 timer = i_this->m2C4;
        if (timer > 0)
            i_this->m2C4 = (s16)(timer - 1);
        else {
            if (i_this->getType() == 1)
                mDoAud_subBgmStop();
            order(i_this);
            i_this->mAction = (u8)((u8)i_this->mAction + 1);
        }
    }
    return TRUE;
}
static void resetEvent() {
    dComIfGp_event_reset();
}
/* 021C15B0 */
static BOOL daMdoor_actionOpen(daMdoor_c* i_this) {
    s16 event = i_this->m2C6;

    if (dComIfGp_evmng_endCheck(event)) {
        i_this->mAction = (u8)((u8)i_this->mAction + 1);
        resetEvent();
        return TRUE;
    }
    s16 timer = i_this->m2C4;
    if (timer < 0)
        return TRUE;
    if (timer > 0) {
        timer = (s16)(timer - 1);
        i_this->m2C4 = timer;
        if (timer)
            return TRUE;
        i_this->speedF = 0.0f;
        if (i_this->getShapeType() != 1)
            debris(i_this);
        sound(i_this, JA_SE_OBJ_CAGE_OPEN, true);
        if ((s16)i_this->m2C4 != 0)
            return TRUE;
    }
    cLib_chaseF(&i_this->speedF, 30.0f, 3.0f);
    cLib_chaseF(&i_this->m2CC, 280.0f, i_this->speedF);
    move(i_this);
    return TRUE;
}
/* 021C1718 */
u8 daMdoor_c::getSwbit() {
    return (u8)mParameters;
}
/* 021C1724 */
static BOOL daMdoor_actionSwitch(daMdoor_c* i_this) {
    u8 sw = i_this->getSwbit();
    s8 room = i_this->current.roomNo;
    dSv_info_c* save = dComIfGs_info();
    if (save->isSwitch(sw, room)) {
        order(i_this);
        i_this->mAction = (u8)((u8)i_this->mAction + 1);
    }
    return TRUE;
}
/* 021C17A4 */
static BOOL daMdoor_actionReady(daMdoor_c* i_this) {
    if ((u16)i_this->eventInfo.mCommand == dEvtCmd_INDEMO_e) {

        i_this->m2C0 = dComIfGp_evmng_getMyStaffId("SHUTTER_DOOR", 0, 0);
        i_this->demoProc();
        i_this->mAction = (u8)((u8)i_this->mAction + 1);
        u8 sw = i_this->getSwbit();
        if (sw != 0xFF) {
            dSv_info_c* save = dComIfGs_info();
            save->isSwitch(sw, (s8)i_this->current.roomNo);
        }
        if (i_this->getType() == 1) {
            dSv_info_c* save = dComIfGs_info();
            save->mSavedata.mEvent.onEventBit(0x1140);
        }
    } else
        order(i_this);
    return TRUE;
}
/* 021C188C */
static BOOL daMdoor_actionReadyOpen(daMdoor_c* i_this) {
    if ((u16)i_this->eventInfo.mCommand == dEvtCmd_INDEMO_e) {
        i_this->mAction = (u8)((u8)i_this->mAction + 1);
        u8 sw = i_this->getSwbit();
        if (sw != 0xFF) {
            dSv_info_c* save = dComIfGs_info();
            save->isSwitch(sw, (s8)i_this->current.roomNo);
        }
        i_this->m2C4 = 20;
    } else
        order(i_this);
    return TRUE;
}
/* 021C1924 */
static BOOL daMdoor_actionEvent(daMdoor_c* i_this) {
    s16 event = i_this->m2C6;

    if (dComIfGp_evmng_endCheck(event)) {
        i_this->mAction = (u8)((u8)i_this->mAction + 1);
        resetEvent();
    } else
        i_this->demoProc();
    return TRUE;
}
/* 021C19A0 */
static BOOL daMdoor_Draw(daMdoor_c* i_this) {
    auto light = dKy_getEnvlight();
    settingTevStruct(light, 1, &i_this->current.pos, &i_this->tevStr);
    light = dKy_getEnvlight();
    setLightTevColorType(light, i_this->mpModel, &i_this->tevStr);
    mDoExt_modelUpdateDL(i_this->mpModel);
    return TRUE;
}
/* 021C19FC */
void daMdoor_c::smokeEnd() {
    m290.end();
}
/* 021C1A04 */
static BOOL daMdoor_Execute(daMdoor_c* i_this) {
    i_this->smokeEnd();
    if (!actionInitialized) {
        actionInitialized = 1;
        mdoorCopyActions(&l_action, &initial_actions, sizeof(initial_actions));
    }
    u8 action = i_this->mAction;
    gabi::call_ptr(l_action.entries[action], i_this);
    return TRUE;
}
/* 021C1A88 */
static BOOL daMdoor_IsDelete(daMdoor_c* i_this) {
    return TRUE;
}
/* 021C1A90 */
static BOOL daMdoor_Delete(daMdoor_c* i_this) {
    if (i_this->heap) {
        auto space = dComIfG_Bgsp();
        space->Release(i_this->mpBgW);
    }
    i_this->smokeEnd();
    dComIfG_resDelete(&i_this->mPhase, "Mdoor");
    return TRUE;
}
/* 021C1AF0 */
u8 daMdoor_c::getToolId() {
    return (mParameters >> 16) & 0xFF;
}
static void raiseAttention(daMdoor_c* i_this) {
    f32 attention = (f32)i_this->attention_info.position.y, eye = i_this->eyePos.y;
    i_this->attention_info.position.y = attention + 150.0f;
    i_this->eyePos.y = eye + 150.0f;
}
/* 021C1AFC */
BOOL daMdoor_c::CreateInit() {
    u8 sw = getSwbit(), type = getType();
    auto space = dComIfG_Bgsp();
    if (space->Regist(mpBgW, this))
        JUT_ASSERT_fail("d_a_mdoor.cpp", 0xEC, "0");
    s8 room = current.roomNo;
    m2CC = 0.0f;
    tevStr.mRoomNo = room;
    m2C8 = 0xFF;
    if (type == 0) {
        dSv_info_c* save = dComIfGs_info();
        bool done = save->mSavedata.mEvent.isEventBit(0x1101);
        if (done)
            mAction = ACT_WAIT3_e;
        else {
            m2CC = 280.0f;
            mAction = ACT_DEMO_WAIT_e;
        }

        s16 event = dComIfGp_evmng_getEventIdx("MORI1_EVENT", 0xFF);
        m2C6 = event;
    } else if (type == 1) {
        dSv_info_c* save = dComIfGs_info();
        if (save->mSavedata.mEvent.isEventBit(0x1101)) {
            mAction = ACT_WAIT7_e;
            m2CC = 280.0f;
        } else {
            save = dComIfGs_info();
            mdoorOffEventBit(&save->mSavedata.mEvent, 0x1140);
            mAction = ACT_GENOCIDE_e;

            s16 event = dComIfGp_evmng_getEventIdx("DEFAULT_STOP_OPEN", 0xFF);
            m2C4 = 1;
            m2C6 = event;
        }
    } else if (type == 2) {
        bool done = sw == 0xFF;
        if (!done) {
            dSv_info_c* save = dComIfGs_info();
            done = save->isSwitch(sw, (s8)current.roomNo);
        }
        if (done) {
            mAction = ACT_WAIT11_e;
            m2CC = 280.0f;
        } else {
            m2C8 = getToolId();
            u8 tool = getToolId();

            s16 event = dComIfGp_evmng_getEventIdx("DEFAULT_STOP_OPEN", tool);
            u8 savedTool = m2C8;
            m2C6 = event;
            mAction = savedTool == 0xFF ? ACT_SWITCH8_e : ACT_SWITCH12_e;
        }
    } else
        mAction = ACT_WAIT0_e;
    raiseAttention(this);
    move(this);
    return TRUE;
}
/* 021C1E8C */
s32 daMdoor_c::create() {
    s32 phase = dComIfG_resLoad(&mPhase, "Mdoor");
    u32 condition = actor_condition;
    if (!(condition & fopAcCnd_INIT_e)) {
        if (this) {
            mdoorActorConstruct(this);
            __vtbl = __vt;
            __ct(&m290, 1);
            condition = actor_condition;
        }
        actor_condition = condition | fopAcCnd_INIT_e;
    }
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)CheckCreateHeap), 0x1640))
        return cPhs_ERROR_e;
    CreateInit();
    return cPhs_COMPLEATE_e;
}
/* 021C1F60 */
static s32 daMdoor_Create(daMdoor_c* i_this) {
    return i_this->create();
}
/* 021C1F64 */
// HD: compiler-generated and template code of this translation unit.
static void __sinit_d_a_mdoor_cpp() {
    __sinit_header_statics();
}
/* 021C1FF8 */
static void SafeString_dtor(void* p, s32 flags) {
    if (p && (flags & 1))
        operator_delete(p);
}
/* 021C200C */
static void daMdoor_c_dtor(daMdoor_c* i_this, s32 flags) {
    if (i_this) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}
/* 021C2060 */
static void SafeString_assureTerminationImpl(void* p) {
}
/* WWHD glue
ext C000A848 void mdoorCopyActions(void*, const void*, u32)
ext 025B8B7C void mdoorOffEventBit(dSv_event_c*, u16)
ext 025D4ED0 void mdoorActorConstruct(fopAc_ac_c*)
ext 026066C4 void* mdoorGetResource(dRes_control_c*, SafeString*, s32)
ext 0200A030 BOOL mdoorSetBackground(dBgW*, cBgD_t*, u32, Mtx34*)
ext 025D98E8 u32 mdoorSearchEnemy(s8)
ext 025E1928 void mDoAud_subBgmStop()
data action_table 101BA3FC
data actionInitialized 101FDAF8
data l_action 101FDAFC
data initial_actions 101BA39C
str 1001491C "d_a_mdoor.cpp" in 021C0F70
str 1001492C "modelData != NULL"
str 10014980 "Mori1"
str 10014988 "SHUTTER_DOOR"
str 100149AC "d_a_mdoor.cpp" in 021C1AFC
str 1001499C "0"
str 100149A0 "MORI1_EVENT"
str 100149BC "DEFAULT_STOP_OPEN"
str 100149E0 "Mdoor"
*/
