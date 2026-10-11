/* Wind Temple elevator. GameCube names/layout with verified WWHD semantics. */
#include "d/actor/d_a_obj_hbrf1.h"
using daObjHbrf1::Act_c;
const gc_tu_data gc_tu = {.safeString = 0x10029F78};
static const char* M_arcname = "Hbrf1";
static Mtx34 M_tmp_mtx;
// HD: the translation callback entry is passed directly to MoveBGCreate.
static const u32 dBgS_MoveBGProc_Trans = 0x024EE76C;
// HD: tww's M_evname is ami_cam, but this actor requests LiftMove.
static const char* M_evname = "LiftMove";
enum { dRes_INDEX_HBRF1_BDL_HBRF1_e = 4, dRes_INDEX_HBRF1_DZB_HBRF1_e = 7 };
enum { JA_SE_OBJ_B_LIFT_STOP = 0x69C9, JA_SE_OBJ_B_LIFT_MOVE = 0x61C8 };
namespace dSv_event_flag_c {
enum { UNK_1540 = 0x1540, UNK_1510 = 0x1510, UNK_1520 = 0x1520, UNK_1508 = 0x1508 };
}
// HD: placement construction stores this translation unit's actor vtable.
Act_c::Act_c() {
    dBgS_MoveBgActor::ct(this);
    __vtbl = 0x1002A048;
}

/* 02351BF4 */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    u32 w = (u32)width & 63, s = (u32)shift & 63;
    return (s < 32 ? (u32)a->mParameters >> s : 0) & ((w < 32 ? 1u << w : 0) - 1);
}
s32 Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }
s32 Act_c::prm_get_Type() { return PrmAbstract(this, PRM_TYPE_W, PRM_TYPE_S); }
s32 Act_c::prm_get_Event() { return PrmAbstract(this, PRM_EVENT_W, PRM_EVENT_S); }
static bool sw(Act_c* a) {
    s32 n = a->prm_get_swSave();
    return dComIfGs_isSwitch(n, a->home.roomNo) != 0;
}
static void order(Act_c* a) { fopAcM_orderOtherEventId(a, a->mEventIdx, 255, 65535, 0, 1); }
static void sound(Act_c* a, u32 id) {
    s32 reverb = dComIfGp_getReverb(a->current.roomNo);
    mDoAud_seStart(id, &a->eyePos, 0, reverb);
}

/* 02350EDC */
s32 Act_c::CreateHeap() {
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_HBRF1_BDL_HBRF1_e);
    JUT_ASSERT(0x5E, model_data != NULL);
    mpModel = mDoExt_J3DModel__create(model_data, 0, 0x11020203);
    return mpModel != nullptr;
}

/* 0235106C */
s32 Act_c::Create() {
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    // HD: retain GHS parameter calls and cached values across switch checks.
    s32 type = prm_get_Type();
    bool raised = false;
    if (type == 0) {
        if (!sw(this))
            raised = true;
        else
            type = prm_get_Type();
    }
    if (!raised && type == 1)
        raised = sw(this);
    if (raised) {
        mMode = Mode_UP_STOP_e;
        current.pos.y = (f32)current.pos.y + 750.0f;
    } else
        mMode = Mode_DOWN_STOP_e;
    init_mtx();
    fopAcM_setCullSizeBox(this, -800.0f, -1000.0f, -800.0f, 800.0f, 1000.0f, 800.0f);
    mYOffset = 10.0f;
    s16 index = dComIfGp_evmng_getEventIdx(M_evname);
    eyePos.copy(home.pos);
    mEventIdx = index;
    m2E4 = 0;
    return TRUE;
}

/* 02350DBC */
s32 Act_c::Mthd_Create() {
    fopAcM_ct(this, Act_c);
    s32 phase = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase == cPhs_COMPLEATE_e) {
        phase = MoveBGCreate(M_arcname, dRes_INDEX_HBRF1_DZB_HBRF1_e, dBgS_MoveBGProc_Trans, 0);
        JUT_ASSERT(0x93, (phase == cPhs_COMPLEATE_e) || (phase == cPhs_ERROR_e)); // HD: line 147.
    }
    return phase;
}

/* 02351B98 */
s32 Act_c::Delete() { return TRUE; }

/* 02350E90 */
s32 Act_c::Mthd_Delete() {
    s32 result = MoveBGDelete();
    dComIfG_resDelete(&mPhs, M_arcname);
    return result;
}

/* 02350F78 */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    MTXCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
}

/* 0235104C */
void Act_c::init_mtx() {
    J3DModel* model = mpModel;
    // HD: all scale components are loaded before any model store.
    f32 x = scale.x, y = scale.y, z = scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    set_mtx();
}

/* 02351278 */
void Act_c::daObjHbrf1_down_stop() {
    // HD: retain GHS parameter calls and cached values across switch checks.
    s32 type = prm_get_Type();
    bool trigger = false;
    if (type == 0) {
        if (!sw(this))
            trigger = true;
        else
            type = prm_get_Type();
    }
    if (!trigger && type != 0)
        trigger = sw(this);
    if (trigger) {
        s32 event = prm_get_Event();
        bool first = false;
        if (event == 0) {
            if (!dComIfGs_isEventBit(dSv_event_flag_c::UNK_1540))
                first = true;
            else
                event = prm_get_Event();
        }
        if (first) {
            dComIfGs_onEventBit(dSv_event_flag_c::UNK_1540);
            order(this);
            mMode = Mode_UP_DEMO_WAIT_e;
            m2E4 = 1;
        } else if (event == 1 && !dComIfGs_isEventBit(dSv_event_flag_c::UNK_1510)) {
            dComIfGs_onEventBit(dSv_event_flag_c::UNK_1510);
            order(this);
            m2E4 = 1;
            mMode = Mode_UP_DEMO_WAIT_e;
        } else
            mMode = Mode_UP_DEMO_e;
    }
}

/* 02351430 */
void Act_c::daObjHbrf1_up_demo_wait() {
    if (eventInfo.checkCommandDemoAccrpt()) {
        mTimer = 30;
        mMode = Mode_UP_DEMO_TIMER_e;
    }
}

/* 02351450 */
void Act_c::daObjHbrf1_up_demo_timer() {
    s32 timer = (s32)((u32)mTimer - 1);
    mTimer = timer;
    if (timer == 0)
        mMode = Mode_UP_DEMO_e;
}

/* 0235146C */
void Act_c::daObjHbrf1_up_demo() {
    // HD: unordered floating comparisons take the stop branch.
    f32 next = (f32)current.pos.y + (f32)mYOffset, target = (f32)home.pos.y + 750.0f;
    current.pos.y = next;
    if (!(next < target)) {
        current.pos.y = target;
        if (m2E4 != 0) {
            dComIfGp_event_reset();
            m2E4 = 0;
        }
        mMode = Mode_UP_STOP_e;
        sound(this, JA_SE_OBJ_B_LIFT_STOP);
        gabi::Local<cXyz> direction;
        direction->x = 0;
        direction->y = 1;
        direction->z = 0;
        dComIfGp_getVibration_StartShock(4, -33, direction);
    } else
        sound(this, JA_SE_OBJ_B_LIFT_MOVE);
}

/* 02351580 */
void Act_c::daObjHbrf1_up_stop() {
    // HD: retain GHS parameter calls and cached values across switch checks.
    s32 type = prm_get_Type();
    bool trigger = false;
    if (type == 0) {
        if (sw(this))
            trigger = true;
        else
            type = prm_get_Type();
    }
    if (!trigger && type != 0)
        trigger = !sw(this);
    if (trigger) {
        s32 event = prm_get_Event();
        bool first = false;
        if (event == 0) {
            if (!dComIfGs_isEventBit(dSv_event_flag_c::UNK_1520))
                first = true;
            else
                event = prm_get_Event();
        }
        if (first) {
            dComIfGs_onEventBit(dSv_event_flag_c::UNK_1520);
            order(this);
            mMode = Mode_DOWN_DEMO_WAIT_e;
            m2E4 = 1;
        } else if (event == 1 && !dComIfGs_isEventBit(dSv_event_flag_c::UNK_1508)) {
            dComIfGs_onEventBit(dSv_event_flag_c::UNK_1508);
            order(this);
            m2E4 = 1;
            mMode = Mode_DOWN_DEMO_WAIT_e;
        } else
            mMode = Mode_DOWN_DEMO_e;
    }
}

/* 02351740 */
void Act_c::daObjHbrf1_down_demo_wait() {
    if (eventInfo.checkCommandDemoAccrpt()) {
        mTimer = 30;
        mMode = Mode_DOWN_DEMO_TIMER_e;
    } else
        order(this);
}

/* 0235177C */
void Act_c::daObjHbrf1_down_demo_timer() {
    s32 timer = (s32)((u32)mTimer - 1);
    mTimer = timer;
    if (timer == 0)
        mMode = Mode_DOWN_DEMO_e;
}

/* 02351798 */
void Act_c::daObjHbrf1_down_demo() {
    // HD: unordered floating comparisons take the stop branch.
    f32 next = (f32)current.pos.y - (f32)mYOffset, target = home.pos.y;
    current.pos.y = next;
    if (!(next > target)) {
        current.pos.y = target;
        if (m2E4 != 0) {
            dComIfGp_event_reset();
            m2E4 = 0;
        }
        mMode = Mode_DOWN_STOP_e;
        sound(this, JA_SE_OBJ_B_LIFT_STOP);
        gabi::Local<cXyz> direction;
        direction->x = 0;
        direction->y = 1;
        direction->z = 0;
        dComIfGp_getVibration_StartShock(4, -33, direction);
    } else
        sound(this, JA_SE_OBJ_B_LIFT_MOVE);
}

/* 0235189C */
s32 Act_c::Execute(gptr<Mtx34>* matrix) {
    switch ((s32)mMode) {
    case Mode_DOWN_STOP_e:
        daObjHbrf1_down_stop();
        break;
    case Mode_UP_DEMO_WAIT_e:
        daObjHbrf1_up_demo_wait();
        break;
    case Mode_UP_DEMO_TIMER_e:
        daObjHbrf1_up_demo_timer();
        break;
    case Mode_UP_DEMO_e:
        daObjHbrf1_up_demo();
        break;
    case Mode_UP_STOP_e:
        daObjHbrf1_up_stop();
        break;
    case Mode_DOWN_DEMO_WAIT_e:
        daObjHbrf1_down_demo_wait();
        break;
    case Mode_DOWN_DEMO_TIMER_e:
        daObjHbrf1_down_demo_timer();
        break;
    case Mode_DOWN_DEMO_e:
        daObjHbrf1_down_demo();
        break;
    }
    set_mtx();
    *matrix = &M_tmp_mtx;
    return TRUE;
}

/* 02351A20 */
s32 Act_c::Draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}

/* 02351AB8 */
static s32 Mthd_Create(Act_c* a) { return a->Mthd_Create(); }

/* 02351ABC */
static s32 Mthd_Delete(Act_c* a) { return a->Mthd_Delete(); }

/* 02351AC0 */
static s32 Mthd_Execute(Act_c* a) { return a->MoveBGExecute(); }

/* 02351AC4 */
static s32 Mthd_Draw(Act_c* a) { return a->Draw_v(); }

/* 02351AD4 */
static s32 Mthd_IsDelete(Act_c* a) { return a->IsDelete_v(); }

// HD: compiler-generated and template code of this translation unit.
/* 02351AE4 */
static void __sinit_d_a_obj_hbrf1_cpp() { sinit_header_statics(0x10469DD0, 0x101C9D70); }

/* 02351B78 */
static void trivial_dt(void* a, s32 flags) {
    if (a && (flags & 1))
        operator_delete(a);
}

/* 02351B8C */
static s32 MoveBgActor_IsDelete(void* a) { return TRUE; }

/* 02351B94 */
static void SafeString_empty(void* a) {}

/* 02351BA0 */
static void Act_c_dt(Act_c* a, s32 flags) {
    if (a) {
        __dt(static_cast<fopAc_ac_c*>(a));
        if (flags & 1)
            operator_delete(a);
    }
}

/* WWHD glue (tools/verify/autoglue.py)
str 1002A040
str 1002A024
str 10029FF8
str 10029F90 in 02350DBC
str 10029FE8 "model_data != NULL" renamed # HD: assertion spells zero instead of NULL.
str 10029FA4 "(phase == cPhs_COMPLEATE_e) || (phase == cPhs_ERROR_e)" renamed
data M_tmp_mtx 10469DEC
*/
