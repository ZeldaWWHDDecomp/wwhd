// Pirate ship rope minigame platforms.
#include "d/actor/d_a_obj_hlift.h"
using daObjHlift::Act_c;
const gc_tu_data gc_tu = {.safeString = 0x1002A608, .headerStatics = {0x10469E74, 0x101C9F58, 0x10469E80}};
// HD: unit data retains the tww names; arrays use guest-layout wrappers.
struct HliftDistanceTable {
    be<s16> mValues[8];
};
struct HliftSizeTable {
    Act_c::size_data mValues[2];
};
struct HliftLagTable {
    be<u8> mValues[5];
};
struct HliftModes {
    gabi::ptmf<void (Act_c::*)()> mProc[5];
};
struct HliftVtable {
    be<u32> mEntry;
};
static HliftDistanceTable M_up_dist;
static HliftSizeTable M_data_size;
static HliftLagTable L_time_lag;
static HliftModes mode_proc;
static HliftVtable M_actorVtable;
struct HliftProcessID {
    be<u32> mValue;
};
static HliftProcessID M_control_id;
struct HliftLagIndex {
    be<s32> mValue;
};
static HliftLagIndex L_time_lag_num;
struct HliftMoveFlag {
    be<u8> mValue;
};
static HliftMoveFlag M_lift_move_flag;
static Mtx34 M_tmp_mtx;
enum { JA_SE_OBJ_PRT_LIFT_UD = 0x6949, JA_SE_OBJ_PRT_LIFT_STOP = 0x694A, JA_SE_OBJ_PRT_LIFT_WORK = 0x301A };
void Hlift_soundWhole(u32);
// HD: the MoveBG create API takes a raw guest callback address; no reference forwarding.
void Hlift_MoveBGProc(dBgW*, fopAc_ac_c*, cBgS_PolyInfo*, cXyz*, csXyz*, csXyz*, bool);
static gabi::HostFn M_moveBgBinding(reinterpret_cast<const void*>(Hlift_MoveBGProc), 0x024EE708);
/* 02357208 */
static s32 Hlift_PrmAbstract(Act_c* a, s32 width, s32 shift) {
    u32 bits = a->mParameters;
    u32 w = u32(width) & 63, sh = u32(shift) & 63;
    u32 mask = w >= 32 ? 0 : 1u << w;
    return (sh >= 32 ? 0 : bits >> sh) & (mask - 1);
}

static f32 distance(Act_c* a) {
    return f32(M_up_dist.mValues[u32(s32(a->field_0x2D8))]);
}

static void sound(Act_c* a, u32 id) {
    s8 room = a->current.roomNo;
    s32 reverb = dComIfGp_getReverb(room);
    mDoAud_seStart(id, &a->eyePos, 0, reverb);
}

static void lagSet(Act_c* a) {
    s32 lag = s32(L_time_lag_num.mValue);
    a->field_0x2F4 = L_time_lag.mValues[u32(lag)];
    lag = s32(L_time_lag_num.mValue);
    s32 next = s32(u32(lag) + 1);
    L_time_lag_num.mValue = next % 5;
}

/* 023561E4 */
s32 Act_c::CreateHeap() {
    s16 resource = M_data_size.mValues[u32(s32(field_0x2DC))].field_0x00;
    auto* model_data = static_cast<J3DModelData*>(dComIfG_getObjectRes("Hlift", resource, gc_tu.safeString));
    if (!model_data)
        JUT_ASSERT_fail("d_a_obj_hlift.cpp", 0x120, "model_data != 0");
    mModel1 = mDoExt_J3DModel__create(model_data, 0x80000, 0x11000022);
    auto* model_data_c = static_cast<J3DModelData*>(dComIfG_getObjectRes("Hlift", 6, gc_tu.safeString));
    if (!model_data_c)
        JUT_ASSERT_fail("d_a_obj_hlift.cpp", 0x12A, "model_data_c != 0");
    J3DModel* model = mDoExt_J3DModel__create(model_data_c, 0x80000, 0x11000022);
    J3DModel* first = mModel1;
    mModel2 = model;
    return first && model;
}

/* 023564F4 */
s32 Act_c::Create() {
    field_0x2EC = 0;
    s32 sw = Hlift_PrmAbstract(this, Act_c::PRM_SWSAVE_W, Act_c::PRM_SWSAVE_S);
    if (fopAcM_isSwitch(this, sw))
        mode_upper_init();
    else
        mode_lower_init();
    field_0x2E4 = Mode_UNK;
    field_0x2F6 = 0;
    u8 event = Hlift_PrmAbstract(this, Act_c::PRM_EVENTID_W, Act_c::PRM_EVENTID_S);
    s16 eid = dComIfGp_evmng_getEventIdx("Hlift_up", event);
    u32 model = gabi::ea(mModel1.get());
    mEventId = eid;
    cullMtx = model ? model + 0xC8 : 0;
    init_mtx();
    fopAcM_setCullSizeBox(this, -151.f, -1005.f, -151.f, 151.f, 1.f, 151.f);
    if (u32(M_control_id.mValue) == fpcM_ERROR_PROCESS_ID_e)
        M_control_id.mValue = u32(mBsPcId);
    return 1;
}

/* 02356090 */
cPhs_State Act_c::Mthd_Create() {
    if (!(u32(actor_condition) & fopAcCnd_INIT_e)) {
        if (this) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = gabi::ea(&M_actorVtable);
        }
        actor_condition = u32(actor_condition) | fopAcCnd_INIT_e;
    }
    cPhs_State state = dComIfG_resLoad(&mPhase, "Hlift");
    if (state == 4) {
        field_0x2D8 = Hlift_PrmAbstract(this, Act_c::PRM_DIST_W, Act_c::PRM_DIST_S);
        s32 size = Hlift_PrmAbstract(this, Act_c::PRM_SIZE_W, Act_c::PRM_SIZE_S);
        field_0x2DC = size;
        auto* row = &M_data_size.mValues[u32(size)];
        u32 heap = row->field_0x02;
        s16 dzb = row->field_0x01;
        state = MoveBGCreate("Hlift", dzb, gabi::guest_fn(reinterpret_cast<const void*>(Hlift_MoveBGProc)), heap);
        if (state != 4 && state != 5)
            JUT_ASSERT_fail("d_a_obj_hlift.cpp", 0x16E,
                            "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)");
    }
    return state;
}

/* 023569DC */
s32 Act_c::Delete() {
    u32 id = u32(mBsPcId);
    if (u32(M_control_id.mValue) == id)
        M_control_id.mValue = fpcM_ERROR_PROCESS_ID_e;
    return 1;
}

/* 02356198 */
s32 Act_c::Mthd_Delete() {
    s32 result = MoveBGDelete();
    dComIfG_resDelete(&mPhase, "Hlift");
    return result;
}

/* 02356344 */
void Act_c::mode_lower_init() {
    f32 y = this->home.pos.y;
    field_0x2E0 = Mode_LOWER;
    current.pos.y = y;
}

/* 02356AC8 */
void Act_c::mode_lower() {
    s32 sw = Hlift_PrmAbstract(this, Act_c::PRM_SWSAVE_W, Act_c::PRM_SWSAVE_S);
    if (fopAcM_isSwitch(this, sw))
        mode_demoreq_init(Mode_L_U);
}

/* 02356B2C */
void Act_c::mode_l_u_init() {
    vib_set();
    field_0x2F0 = 0;
    field_0x2E0 = Mode_L_U;
    sound(this, JA_SE_OBJ_PRT_LIFT_UD);
    lagSet(this);
}

/* 02356BD0 */
void Act_c::mode_l_u() {
    s16 lag = field_0x2F4;
    if (lag > 0) {
        field_0x2F4 = lag - 1;
        return;
    }
    f32 y = current.pos.y, home = this->home.pos.y;
    f32 height = y - home;
    if (height < 50.f)
        cLib_chaseF(&field_0x2F0, 10.f, 0.3f);
    else if (height < distance(this) - 50.f)
        field_0x2F0 = 10.f;
    else
        cLib_chaseF(&field_0x2F0, 2.f, 1.2f);
    f32 d = distance(this);
    home = this->home.pos.y;
    f32 speed = field_0x2F0;
    if (cLib_chaseF(&current.pos.y, home + d, speed)) {
        vib_set();
        sound(this, JA_SE_OBJ_PRT_LIFT_STOP);
        mode_upper_init();
    }
}

/* 023562EC */
void Act_c::mode_upper_init() {
    f32 d = distance(this);
    f32 y = this->home.pos.y;
    field_0x2E0 = Mode_UPPER;
    current.pos.y = y + d;
}

/* 02356DDC */
void Act_c::mode_upper() {
    s32 sw = Hlift_PrmAbstract(this, Act_c::PRM_SWSAVE_W, Act_c::PRM_SWSAVE_S);
    if (!fopAcM_isSwitch(this, sw))
        mode_demoreq_init(Mode_U_L);
}

/* 02356E40 */
void Act_c::mode_u_l_init() {
    vib_set();
    field_0x2F0 = 0;
    sound(this, JA_SE_OBJ_PRT_LIFT_UD);
    lagSet(this);
    field_0x2E0 = Mode_U_L;
}

/* 02356EE0 */
void Act_c::mode_u_l() {
    s16 lag = field_0x2F4;
    if (lag > 0) {
        field_0x2F4 = lag - 1;
        return;
    }
    f32 home = this->home.pos.y, y = current.pos.y;
    f32 height = y - home;
    if (height < 50.f)
        cLib_chaseF(&field_0x2F0, 2.f, 1.2f);
    else if (height < distance(this) - 50.f)
        field_0x2F0 = 10.f;
    else
        cLib_chaseF(&field_0x2F0, 10.f, 0.3f);
    home = this->home.pos.y;
    f32 speed = field_0x2F0;
    if (cLib_chaseF(&current.pos.y, home, speed)) {
        vib_set();
        sound(this, JA_SE_OBJ_PRT_LIFT_STOP);
        mode_lower_init();
    }
}

/* 02356A14 */
void Act_c::mode_demoreq_init(Mode_e next) {
    if (next != Mode_U_L && next != Mode_L_U)
        JUT_ASSERT_fail("d_a_obj_hlift.cpp", 0x241, "(next == Mode_U_L) || (next == Mode_L_U)");
    field_0x2E4 = next;
    field_0x2F6 = 0;
    u8 event = Hlift_PrmAbstract(this, Act_c::PRM_EVENTID_W, Act_c::PRM_EVENTID_S);
    s16 id = mEventId;
    fopAcM_orderOtherEventId(this, id, event, 0xFFFF, 0, 1);
    u16 flags = u16(eventInfo.mCondition);
    field_0x2E0 = Mode_DEMOREQ;
    eventInfo.mCondition = flags | dEvtCnd_UNK2_e;
}

/* 02357040 */
void Act_c::mode_demoreq() {
    s16 id = mEventId;
    if (dComIfGp_evmng_existence(id) && eventInfo.checkCommandDemoAccrpt())
        field_0x2F6 = 1;
    if (s32(field_0x2E4) == Mode_U_L)
        mode_u_l_init();
    else
        mode_l_u_init();
}

/* 02356358 */
void Act_c::set_mtx() {
    f32 offset = field_0x2EC;
    f32 y = current.pos.y;
    y = (y + offset) + 30.0f;
    f32 z = current.pos.z;
    f32 x = current.pos.x;
    mDoMtx_stack_c::transS(x, y, z);
    s16 az = shape_angle.z, ax = shape_angle.x, ay = shape_angle.y;
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), ax, ay, az);
    mModel1->setBaseTRMtx(mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
    x = this->home.pos.x;
    y = this->home.pos.y;
    z = this->home.pos.z;
    mDoMtx_stack_c::transS(x, y, z);
    mModel2->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 023564B8 */
void Act_c::init_mtx() {
    for (int n = 0; n < 2; n++) {
        J3DModel* model = n ? mModel2.get() : mModel1.get();
        f32 x = scale.x, y = scale.y, z = scale.z;
        model->mBaseScale.set(x, y, z);
    }
    set_mtx();
}

/* 023566EC */
void Act_c::rot_set() {
    f32 y = current.pos.y, home = this->home.pos.y;
    shape_angle.y = s16(0u - u32(gabi::ftoi((y - home) * 262.144012451171875f)));
}

/* 02356A00 */
void Act_c::vib_set() {
    field_0x2E8 = 10;
    field_0x2EA = 0x4000;
}

/* 02356728 */
void Act_c::vib_proc() {
    s16 timer = field_0x2E8;
    if (timer > 0) {
        u16 angle = field_0x2EA;
        // HD: retain fused multiply-add and the original constant rounding.
        f32 factor = gabi::fmadds(f32(10 - timer), 0.05000000074505806f, 0.5f);
        f32 sine = cM_ssin(angle);
        timer = field_0x2E8;
        field_0x2EA = angle + 0x4000;
        field_0x2E8 = timer - 1;
        field_0x2EC = sine * (2.5f * factor);
    } else
        field_0x2EC = 0;
}

/* 02356664 */
u8 Act_c::chk_demo_end() {
    u8 demo = field_0x2F6;
    if (demo) {
        s16 id = mEventId;
        if (dComIfGp_evmng_endCheck(id)) {
            dComIfGp_event_reset();
            field_0x2F6 = 0;
            demo = 0;
        } else
            demo = field_0x2F6;
    }
    // HD: XOR the byte, including non-canonical values, without boolean conversion.
    return demo ^ 1;
}

/* 023567D0 */
void Act_c::se_whole() {
    u32 id = u32(mBsPcId);
    if (u32(M_control_id.mValue) == id && u8(M_lift_move_flag.mValue)) {
        Hlift_soundWhole(JA_SE_OBJ_PRT_LIFT_WORK);
        M_lift_move_flag.mValue = 0;
    }
}

/* 02356828 */
s32 Act_c::Execute(gptr<Mtx34>* out) {
    chk_demo_end();
    (this->*mode_proc.mProc[u32(s32(field_0x2E0))])();
    s32 mode = field_0x2E0;
    if (mode == Mode_L_U || mode == Mode_U_L || mode == Mode_DEMOREQ)
        M_lift_move_flag.mValue = 1;
    rot_set();
    vib_proc();
    se_whole();
    f32 x = current.pos.x, z = current.pos.z, y = current.pos.y;
    eyePos.z = z;
    eyePos.y = y;
    eyePos.x = x;
    set_mtx();
    *out = &M_tmp_mtx;
    return 1;
}

/* 02356928 */
s32 Act_c::Draw() {
    auto* env = dKy_getEnvlight();
    settingTevStruct(env, 1, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, mModel1, &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, mModel2, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mModel1);
    mDoExt_modelUpdateDL(mModel2);
    dComIfGd_setList();
    return TRUE;
}

/* 023570D4 */
static cPhs_State daObjHlift_Mthd_Create(Act_c* a) {
    return a->Mthd_Create();
}

/* 023570D8 */
static s32 daObjHlift_Mthd_Delete(Act_c* a) {
    return a->Mthd_Delete();
}

/* 023570DC */
static s32 daObjHlift_Mthd_Execute(Act_c* a) {
    return a->MoveBGExecute();
}

/* 023570F0 */
static s32 daObjHlift_Mthd_Draw(Act_c* a) {
    // HD: Draw is at the unit vtable slot 0x3C.
    return a->vcall(0x3C);
}

/* 023570E0 */
static s32 daObjHlift_Mthd_IsDelete(Act_c* a) {
    // HD: IsDelete is at the unit vtable slot 0x2C.
    return a->vcall(0x2C);
}

/* 02357100 */
static void __sinit_d_a_obj_hlift_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}

/* 02357194 */
static void Hlift_SafeString_dtor(void* a, s32 flags) {
    if (a && (flags & 1))
        operator_delete(a);
}

/* 023571A8 */
static s32 Hlift_IsDelete(Act_c* a) {
    return 1;
}

/* 023571B0 */
static void Hlift_SafeString_assureTerminationImpl(Act_c* a) {}

/* 023571B4 */
static void Hlift_dtor(Act_c* a, s32 flags) {
    if (a) {
        __dt(static_cast<fopAc_ac_c*>(a));
        if (flags & 1)
            operator_delete(a);
    }
}

/* WWHD glue
 data M_up_dist 1002A798
 data M_data_size 1002A7A8
 data L_time_lag 1002A644
 data mode_proc 1002A70C
 data M_actorVtable 1002A7C4
 data M_control_id 101C9F7C
 data L_time_lag_num 101C9F80
 data M_lift_move_flag 101C9F84
 data M_tmp_mtx 10469E90
 str 1002A790
 str 1002A7B8
 str 1002A734 in 02356A14
 str 1002A748
 str 1002A6B4 in 023561E4
 str 1002A6A4 "model_data != 0"
 str 1002A6C8 "model_data_c != 0"
 str 1002A64C "d_a_obj_hlift.cpp" in 02356090
 str 1002A660 "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)"
 ext 025E1988 void Hlift_soundWhole(u32)
 ext 024EE708 void Hlift_MoveBGProc(dBgW*, fopAc_ac_c*, cBgS_PolyInfo*, cXyz*, csXyz*, csXyz*, bool)
*/
