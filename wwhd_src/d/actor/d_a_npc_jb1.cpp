// Jabun NPC: typed tww layout, with verified HD animation, lighting and action semantics.
#include "d/actor/d_a_npc_jb1.h"
BOOL daNpc_Jb1_IsDelete(daNpc_Jb1_c* actor);
void daNpc_Jb1_HioDestroy(void* object, s32 flags);
BOOL daNpc_Jb1_Delete(daNpc_Jb1_c* actor);
void daNpc_Jb1_Destroy(daNpc_Jb1_c* actor, s32 flags);
BOOL daNpc_Jb1_Execute(daNpc_Jb1_c* actor);
BOOL daNpc_Jb1_Draw(daNpc_Jb1_c* actor);
daNpc_Jb1_c* daNpc_Jb1_Construct(daNpc_Jb1_c* actor);
void* daNpc_Jb1_HioConstruct(void* object);
void daNpc_Jb1_StaticInit();
BOOL CheckCreateHeap(daNpc_Jb1_c* actor);
s32 daNpc_Jb1_Create(daNpc_Jb1_c* actor);

struct JbSafeString;
const gc_tu_data gc_tu = {.safeString = 0x1001AC50, .headerStatics = {0x104671EC, 0x101BEA18, 0x10467218}};
static jb1::Hio l_HIO;
static jb1::HioParameters a_prm_tbl;
static jb1::PiPair l_pi;
static Mtx34 l_matrix;

void* jbPlay();
void jbOrderSpeak(daNpc_Jb1_c*);
s32 jbStaffId(void*, u32, u32, u32);
void* jbEventSubstance(void*, s32, const char*, s32);
void jbSetNpcAnm(void*, s32, f32, f32, s32, s32, const char*);
BOOL jbLockOn(void*);
void* jbLockTarget(void*, u32);
void* jbAttentionTarget(void*, u32);
void jbDelete(void*);
s32 jbActionIndex(void*, s32, void*, s32, u32, u32);
void jbCutEnd(void*, s32);
BOOL jbAdvance(void*, s32);
BOOL jbEventCut(dNpc_EventCut_c*);
void jbAssert(const char*, s32, const char*);
void jbDemoActor(void*, u32);
void jbDemoUpdate(daNpc_Jb1_c*, u32, void*, const char*, u32, u32, u32, u32);
void jbResDelete(request_of_phase_process_class*, const char*);
void jbCutLight(LIGHT_INFLUENCE*);
void jbStopAnimation(void*);
void jbHioRemove(s32);
void jbBckDestroy(be<u32>*, u32);
void jbCylinderDestroy(dCcD_Cyl*, u32);
void jbStatusDestroy(dCcD_Stts*, u32);
void jbCircleDestroy(void*, u32);
void jbAcchDestroy(dBgS_ObjAcch*, u32);
void jbActorDestroy(fopAc_ac_c*, u32);
void jbSetActorInfo(dNpc_EventCut_c*, const char*, fopNpc_npc_c*);
void jbSetLight(LIGHT_INFLUENCE*);
void jbSetFrame(void*, f32);
void jbPositionMove(fopAc_ac_c*, dCcD_Stts*);
void* jbEnvironment();
void jbSetTev(void*, s32, const cXyz*, dKy_tevstr_c*);
void jbApplyTev(void*, void*, dKy_tevstr_c*);
void jbEntryBrk(mDoExt_baseAnm*, void*, f32);
void jbEntryModel(void*);
void jbModelEntry(void*);
void jbMemcpy(void*, const void*, u32);
void* jbAllocate(u32);
void jbNpcConstruct(fopNpc_npc_c*);
void jbFrameConstruct(mDoExt_baseAnm*, u32);
void jbMtxCalcConstruct(jb1::MtxCalc*);
void jbBtkConstruct(mDoExt_baseAnm*);
void jbBrkConstruct(mDoExt_baseAnm*);
void jbEventConstruct(dNpc_EventCut_c*);
void jbLineConstruct(void*);
void jbRandomInit(void*);
void jbRandomInit2(void*);
void jbMatrixTranslate(void*, f32, f32, f32);
void jbMatrixRotateY(void*, s16);
void jbMatrixVector(void*, const void*, cXyz*);
void jbCircleWall(dBgS_AcchCir*, f32, f32);
void jbAcchSet(dBgS_ObjAcch*, cXyz*, cXyz*, fopAc_ac_c*, u32, dBgS_AcchCir*, cXyz*, u32, u32);
s32 jbResLoad(request_of_phase_process_class*, const char*);
s32 jbHioAdd(const char*, void*);
BOOL jbEntryHeap(fopAc_ac_c*, u32, u32);
void* jbResource(void*, JbSafeString*, s32);
// HD: allocation and null callback registers remain explicit.
void* jbMorfConstruct(void*, void*, void*, void*, void*, s32, f32, s32, s32, s32, void*, u32, u32);
s32 jbInitBrk(mDoExt_baseAnm*, void*, void*, u32, s32, f32, s32, s32, u32);
void* jbJointNames(void*);
s32 jbFindName(void*, const char*);
void* jbModelCreate(void*, u32, u32);
s32 jbMorfPlay(void*, cXyz*, u32, u32);
void jbCorrectPosition(dBgS_ObjAcch*, void*);
s32 jbRoom(void*, void*);
s32 jbPolyColor(void*, void*);
void jbMorfCalc(void*);
void jbMatrixCopy(void*, void*);
void jbTranslateM(f32, f32, f32);
void jbScaleM(f32, f32, f32);
void jbModelCalc(void*);
static jb1::LightParameters l_defaultLight;
static s32 a_bck_resID_tbl[2] = {1, 0};
static JbAnimationParameters l_anmPrm[2] = {{-1, 0, 0.0f, 0.0f, -1}, {0, 0, 8.0f, 1.0f, 2}};
static JbAnimationParameters l_atrPrm[1] = {{0, 0, 8.0f, 1.0f, 2}};
static u32 l_heapSize;
static u32 l_cutNames;
static JbActionDescriptor l_initialAction;
static f32 l_zero;
static f32 l_attentionHeight;
static f32 l_attentionForward;
static f32 l_lightY;
static f32 l_scale;
static f32 l_one;
static u32 l_demoObjects;
static u32 l_debugGuard0;
static u32 l_debugGuard1;
static u32 l_debugGuard2;
static u32 l_debugColor0;
static u32 l_debugColor1;
static u32 l_debugColor2;
static u32 l_color0;
static u32 l_color1;
static u32 l_color2;
namespace {
void* play() {
    return jbPlay();
}
} // namespace

struct JbVector {
    be<f32> x, y, z;
};

struct JbSafeString {
    be<u32> text, vtable;
};
namespace {
void* resource(JbSafeString* archive, s32 id) {
    void* control = dComIfG_resControl();
    return jbResource(control, archive, id);
}
void assertion(const char* file, s32 line, const char* expression) {
    jbAssert(file, line, expression);
}
} // namespace

namespace {
// Load all twelve matrix components before any store, preserving overlap.
void copyBaseMatrix(void* model, void* source) {
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = f32(jb1::matrixFloats(source)[i]);
    for (u32 i = 0; i < 12; ++i)
        jb1::modelMatrix(model)[i] = values[i];
}

} // namespace

/* 02247A18 */
BOOL daNpc_Jb1_c::init_JB1_0() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<JbActionDescriptor> action;
    action->adjustment = s16(l_initialAction.adjustment);
    action->selector = s16(l_initialAction.selector);
    action->target = u32(l_initialAction.target);
    set_action(action.get(), nullptr);
    return 1;
}

/* 02247E84 */
BOOL daNpc_Jb1_c::createInit() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    f32 zero = l_zero;
    gravity = zero;
    jbSetActorInfo(&mEventCut, "Jb1", this);
    s8 character = field_0x921;
    field_0x91C = 2;
    if (character != 0 || !init_JB1_0())
        return 0;
    u16 x = current.angle.x, z = current.angle.z, y = current.angle.y;
    shape_angle.z = z;
    shape_angle.y = y;
    shape_angle.x = x;
    jbSetLight(&field_0x7F8);
    void* morph = mpMorf.get();
    jbSetFrame(morph, zero);
    setMtx();
    return 1;
}

/* 02247B7C */
void daNpc_Jb1_c::setMtx() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    void* animation = m_lmp_brk.get();
    f32 zero = l_zero, offsetY = l_lightY;
    gabi::Local<JbVector> offset;
    offset->z = zero;
    offset->y = offsetY;
    offset->x = zero;
    playBrkAnm(animation, &field_0x834);
    animation = m_lgt_brk.get();
    playBrkAnm(animation, &field_0x858);
    if (u8(field_0x917) == 0) {
        void* morph = mpMorf.get();
        u8 wrapped = jbMorfPlay(morph, &eyePos, 0, 0);
        morph = mpMorf.get();
        f32 old = mFrame;
        field_0x908 = wrapped;
        f32 frame = jb1::morphFrame(morph);
        if (frame < old) {
            morph = mpMorf.get();
            field_0x908 = 1;
        }
        mFrame = jb1::morphFrame(morph);
        void* state = play();
        jbCorrectPosition(&mObjAcch, &jb1::gamePlay(state)->mBgS);
    }
    void* state = play();
    u8 room = jbRoom(&jb1::gamePlay(state)->mBgS, static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mRoomNo = room;
    state = play();
    u8 color = jbPolyColor(&jb1::gamePlay(state)->mBgS, static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mEnvrIdxOverride = color;
    void* matrix = &l_matrix;
    f32 y = current.pos.y, x = current.pos.x, z = current.pos.z;
    jbMatrixTranslate(matrix, x, y, z);
    s16 angle = current.angle.y;
    jbMatrixRotateY(matrix, angle);
    void* morph = mpMorf.get();
    void* model = jb1::morphModel(morph);
    copyBaseMatrix(model, matrix);
    morph = mpMorf.get();
    jbMorfCalc(morph);
    void* joint = jb1::jointMatrix(this);
    jbMatrixCopy(joint, matrix);
    jbMatrixVector(matrix, offset.get(), &field_0x6D0);
    joint = jb1::jointMatrix(this);
    jbMatrixCopy(joint, matrix);
    jbTranslateM(zero, offsetY, zero);
    f32 scale = l_scale;
    jbScaleM(scale, scale, scale);
    // The second matrix snapshot precedes the light-model pointer read.
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = f32(jb1::matrixFloats(matrix)[i]);
    model = m_lgt_mdl.get();
    for (u32 i = 0; i < 12; ++i)
        jb1::modelMatrix(model)[i] = values[i];
    model = m_lgt_mdl.get();
    jbModelCalc(model);
    u32 py = jb1::words(&field_0x6D0)->y, pz = jb1::words(&field_0x6D0)->z, px = jb1::words(&field_0x6D0)->x;
    jb1::words(&field_0x7F8.mPos)->z = pz;
    jb1::words(&field_0x7F8.mPos)->y = py;
    jb1::words(&field_0x7F8.mPos)->x = px;
    field_0x7F8.mColorR = s16(l_HIO.mPrmTbl.field_0x06);
    field_0x7F8.mColorG = s16(l_HIO.mPrmTbl.field_0x08);
    field_0x7F8.mColorB = s16(l_HIO.mPrmTbl.field_0x0A);
    field_0x7F8.mPower = f32(l_HIO.mPrmTbl.field_0x0C);
    field_0x7F8.mFluctuation = f32(l_HIO.mPrmTbl.field_0x10);
    setAttention();
}

/* 02247A58 */
void daNpc_Jb1_c::playBrkAnm(void* animation, be<s16>* frame) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!animation)
        return;
    s16 next = s16(u16(s16(*frame)) + 1);
    *frame = next;
    s32 maximum = jb1::animationMaximum(animation);
    if (next >= maximum)
        *frame = 0;
}

/* 0224876C */
s32 daNpc_Jb1_c::anmNum_toResID(s32 animation) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return *jb1::atIndex(a_bck_resID_tbl, u32(animation));
}

/* 02248780 */
BOOL daNpc_Jb1_c::setAnm_anm(JbAnimationParameters* parameters) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 animation = parameters->field_0x0;
    if (s8(field_0x91C) == animation)
        return 1;
    field_0x91C = animation;
    s32 resource = anmNum_toResID(animation);
    // The original keeps the parameter pointer across the resource lookup, then
    // reads the parameters and actor's current morph controller afterwards.
    s32 loop = parameters->mLoopMode;
    void* morph = mpMorf.get();
    f32 speed = parameters->mPlaySpeed, blend = parameters->mMorf;
    jbSetNpcAnm(morph, loop, blend, speed, resource, -1, "Jb");
    field_0x908 = 0;
    mFrame = l_zero;
    field_0x909 = 0;
    return 1;
}

/* 02248814 */
BOOL daNpc_Jb1_c::setAnm() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    auto parameters = jb1::atIndex(l_anmPrm, u32(s32(s8(field_0x91E))));
    if (parameters->field_0x0 >= 0)
        setAnm_anm(parameters);
    return 1;
}

/* 02248874 */
void daNpc_Jb1_c::chg_anmAtr(u8 attribute) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (attribute != 0 || u8(field_0x91A) == 0)
        return;
    field_0x91A = 0;
    setAnm_ATR();
}

/* 0224885C */
BOOL daNpc_Jb1_c::setAnm_ATR() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return setAnm_anm(jb1::atIndex(l_atrPrm, u8(field_0x91A)));
}

/* 02248894 */
void daNpc_Jb1_c::anmAtr(u16 status) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (status == 14) {
        field_0x923 = 0;
        return;
    }
    if (status != 6)
        return;
    if (s8(field_0x923) == 0) {
        field_0x91A = 255;
        void* state = play();
        u8 attribute = u8(jb1::gamePlay(state)->mMesgAnime);
        chg_anmAtr(attribute);
        field_0x923 = u8(field_0x923) + 1;
    }
    void* state = play();
    u8 tag = u8(jb1::gamePlay(state)->mMesgAnimeTagInfo);
    state = play();
    jb1::gamePlay(state)->mMesgAnimeTagInfo = 255;
    if (tag != 255 && u8(field_0x91B) != tag)
        field_0x91B = tag;
}

/* 0224840C */
void daNpc_Jb1_c::eventOrder() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 order = field_0x91D;
    if (order != 1 && order != 2)
        return;
    order = field_0x91D;
    eventInfo.mCondition = eventInfo.mCondition | dEvtCnd_CANTALK_e;
    if (order == 1)
        jbOrderSpeak(this);
}

/* 02248128 */
void daNpc_Jb1_c::checkOrder() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (eventInfo.mCommand != dEvtCmd_INTALK_e)
        return;
    s8 order = field_0x91D;
    if (order == 1 || order == 2) {
        field_0x91D = 0;
        field_0x915 = 1;
    }
}

/* 0224896C */
u16 daNpc_Jb1_c::next_msgStatus(u32* message) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 16;
}

/* 02248974 */
u32 daNpc_Jb1_c::getMsg() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 0;
}

/* 0224897C */
BOOL daNpc_Jb1_c::chkAttention() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    void* state = play();
    void* attention = &jb1::gamePlay(state)->mAttention;
    BOOL locked = jbLockOn(attention);
    void* target = locked ? jbLockTarget(attention, 0) : jbAttentionTarget(attention, 0);
    return this == target;
}

/* 02247AC4 */
void daNpc_Jb1_c::setAttention() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<JbVector> offset;
    f32 x = current.pos.x;
    f32 zero = l_zero;
    f32 y = current.pos.y, height = l_attentionHeight;
    offset->x = zero;
    offset->y = height;
    offset->z = l_attentionForward;
    f32 z = current.pos.z;
    void* matrix = &l_matrix;
    jbMatrixTranslate(matrix, x, y, z);
    s16 angle = current.angle.y;
    jbMatrixRotateY(matrix, angle);
    jbMatrixVector(matrix, offset.get(), &field_0x8F4);
    y = field_0x8F4.y;
    height = f32(l_HIO.mPrmTbl.field_0x00);
    eyePos.y = y;
    f32 raised = y + height;
    x = field_0x8F4.x;
    z = field_0x8F4.z;
    attention_info.position.y = raised;
    eyePos.x = x;
    attention_info.position.z = z;
    attention_info.position.x = x;
    eyePos.z = z;
}

/* 022478D8 */
BOOL daNpc_Jb1_c::charDecide(s32 character) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    field_0x921 = 0;
    field_0x920 = 0;
    return 1;
}

/* 02248280 */
void daNpc_Jb1_c::event_actionInit(s32 staff) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    void* state = play();
    void* number = jbEventSubstance(&jb1::gamePlay(state)->mEvtManager, staff, "ActNo", 3);
    if (number)
        mActNo = u32(*static_cast<be<u32>*>(number));
}

/* 022482E0 */
BOOL daNpc_Jb1_c::event_action() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 1;
}

/* 022482E8 */
void daNpc_Jb1_c::privateCut(s32 staff) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (staff == -1)
        return;
    void* state = play();
    s8 index = jbActionIndex(&jb1::gamePlay(state)->mEvtManager, staff, &l_cutNames, 1, 1, 0);
    mActIdx = index;
    state = play();
    if (index == -1) {
        jbCutEnd(&jb1::gamePlay(state)->mEvtManager, staff);
        return;
    }
    BOOL advanced = jbAdvance(&jb1::gamePlay(state)->mEvtManager, staff);
    if (advanced && s8(mActIdx) == 0)
        event_actionInit(staff);
    if (s8(mActIdx) == 0 && !event_action())
        return;
    state = play();
    jbCutEnd(&jb1::gamePlay(state)->mEvtManager, staff);
}

/* 02248240 */
s32 daNpc_Jb1_c::isEventEntry() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 name = mEventCut.mpEvtStaffName;
    void* state = play();
    return jbStaffId(&jb1::gamePlay(state)->mEvtManager, name, 0, 0);
}

/* 022483BC */
void daNpc_Jb1_c::event_proc(s32 staff) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!jbEventCut(&mEventCut))
        privateCut(staff);
}

// HD: the incoming action descriptor is eight bytes, with selector zero handled specially.
/* 022478EC */
BOOL daNpc_Jb1_c::set_action(JbActionDescriptor* action, void* argument) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s16 oldSelector = mCurrActionFunc.i, selector = action->selector;
    s16 adjustment = action->adjustment;
    u32 target = action->target;
    bool unchanged = oldSelector == selector &&
                     (oldSelector == 0 || (mCurrActionFunc.d == adjustment && mCurrActionFunc.f == target));
    if (unchanged)
        return 1;
    if (oldSelector != 0) {
        field_0x922 = 9;
        jb1::dispatchAction(this, argument);
    }
    mCurrActionFunc.f = target;
    mCurrActionFunc.d = adjustment;
    mCurrActionFunc.i = selector;
    field_0x922 = 0;
    jb1::dispatchAction(this, argument);
    return 1;
}

/* 02248A04 */
BOOL daNpc_Jb1_c::setStt(s8 state) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    field_0x91E = state;
    return setAnm();
}

/* 02248A0C */
BOOL daNpc_Jb1_c::wait_action1(void* argument) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 phase = field_0x922;
    if (phase == 0) {
        setStt(1);
        field_0x922 = u8(field_0x922) + 1;
    } else if (u32(s32(phase)) <= 3) {
        BOOL attention = chkAttention();
        s8 state = field_0x91E;
        mHasAttention = attention;
        if (state == 1)
            field_0x910 = 1;
    }
    return 1;
}

/* 02248168 */
u8 daNpc_Jb1_c::demo() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (demoActorID == 0) {
        u8 active = field_0x917;
        if (active) {
            field_0x917 = 0;
            active = 0;
        }
        return active;
    }
    u8 id = demoActorID;
    field_0x917 = 1;
    if (id && id <= 32) {
        void* demo = jb1::demoObjects(l_demoObjects);
        if (!demo) {
            jbAssert("d_demo.h", 0x23A, "m_object != NULL");
            demo = jb1::demoObjects(l_demoObjects);
        }
        jbDemoActor(demo, id);
    }
    void* morph = mpMorf.get();
    jbDemoUpdate(this, 0x6A, morph, "Jb", 0, 0, 0, 0);
    return field_0x917;
}

/* 022485C4 */
BOOL daNpc_Jb1_c::_draw() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    void* lightModel = m_lgt_mdl.get();
    void* morph = mpMorf.get();
    u8 hidden = field_0x90C;
    void* model = jb1::morphModel(morph);
    void* lightData = jb1::modelData(lightModel);
    void* modelData = jb1::modelData(model);
    if (hidden || field_0x90E)
        return 1;
    void* environment = jbEnvironment();
    jbSetTev(environment, 0, &current.pos, &tevStr);
    environment = jbEnvironment();
    jbApplyTev(environment, model, &tevStr);
    jbEntryBrk(&mBrkAnm, modelData, f32(field_0x834));
    morph = mpMorf.get();
    jbEntryModel(morph);
    jb1::modelDataView(modelData)->animation = 0;
    jbEntryBrk(&mBrkAnm2, lightData, f32(field_0x858));
    lightModel = m_lgt_mdl.get();
    jbModelEntry(lightModel);
    jb1::modelDataView(lightData)->animation = 0;
    // HD: the debug branch initializes three guarded color words.
    if (jb1::debugEnabled(&l_HIO)) {
        if (l_debugGuard0 == 0) {
            l_debugGuard0 = 1;
            jbMemcpy(&l_debugColor0, &l_color0, 4);
        }
        if (l_debugGuard1 == 0) {
            l_debugGuard1 = 1;
            jbMemcpy(&l_debugColor1, &l_color1, 4);
        }
        if (l_debugGuard2 == 0) {
            l_debugGuard2 = 1;
            jbMemcpy(&l_debugColor2, &l_color2, 4);
        }
    }
    return 1;
}

/* 02248444 */
BOOL daNpc_Jb1_c::_execute() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (field_0x90F == 0) {
        u16 y = current.angle.y;
        u32 z = jb1::words(&current.pos)->z;
        mCurrentRot.y = y;
        u16 x = current.angle.x;
        u32 py = jb1::words(&current.pos)->y;
        jb1::words(&mCurrentPos)->z = z;
        u16 rz = current.angle.z;
        u32 px = jb1::words(&current.pos)->x;
        mCurrentRot.z = rz;
        jb1::words(&mCurrentPos)->x = px;
        jb1::words(&mCurrentPos)->y = py;
        field_0x90F = 1;
        mCurrentRot.x = x;
    }
    if (field_0x90C && demoActorID == 0)
        return 1;
    field_0x90C = 0;
    checkOrder();
    if (!demo()) {
        void* state = play();
        s32 staff = -1;
        if (u8(jb1::gamePlay(state)->mEvtCtrl.mMode) && eventInfo.mCommand != dEvtCmd_INTALK_e)
            staff = isEventEntry();
        if (staff >= 0)
            event_proc(staff);
        else
            jb1::dispatchAction(this, nullptr);
        jbPositionMove(this, &mStts);
        if (field_0x90D == 0) {
            u16 z = current.angle.z, y = current.angle.y;
            shape_angle.z = z;
            u16 x = current.angle.x;
            shape_angle.y = y;
            shape_angle.x = x;
        }
    }
    eventOrder();
    setMtx();
    return 1;
}

/* 02248098 */
BOOL daNpc_Jb1_c::_delete() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    jbResDelete(&mPhs, "Jb");
    jbCutLight(&field_0x7F8);
    if (heap) {
        void* morph = mpMorf.get();
        if (morph)
            jbStopAnimation(morph);
    }
    s32 users = s32(l_HIO.field_0x8);
    if (users >= 0) {
        users = s32(u32(users) - 1);
        l_HIO.field_0x8 = users;
        if (users < 0)
            jbHioRemove(s8(l_HIO.mNo));
    }
    return 1;
}

/* 02247698 */
BOOL CheckCreateHeap(daNpc_Jb1_c* actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return actor->CreateHeap();
}

/* 02247F64 */
s32 daNpc_Jb1_c::_create() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 flags = actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (this) {
            daNpc_Jb1_Construct(this);
            flags = actor_condition;
        }
        actor_condition = flags | fopAcCnd_INIT_e;
    }
    s32 phase = jbResLoad(&mPhs, "Jb");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (!charDecide((u8)(u32)mParameters))
        return cPhs_ERROR_e;
    s32 users = s32(l_HIO.field_0x8);
    if (users < 0) {
        s32 child = jbHioAdd("Jabun", &l_HIO);
        users = s32(l_HIO.field_0x8);
        l_HIO.mNo = u8(child);
    }
    u32 size = u32(l_heapSize);
    l_HIO.field_0x8 = u32(users) + 1;
    if (!jbEntryHeap(this, gabi::guest_fn((const void*)&CheckCreateHeap), size))
        return cPhs_ERROR_e;
    void* morph = mpMorf.get();
    u32 model = gabi::ea(jb1::morphModel(morph));
    cullMtx = model ? model + 0xC8 : 0;
    return createInit() ? phase : cPhs_ERROR_e;
}

/* 02247260 */
void* daNpc_Jb1_c::create_Anm() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<JbSafeString> archive1, archive2, archive3;
    archive1->vtable = gc_tu.safeString;
    archive1->text = gabi::guest_addr("Jb");
    void* data = resource(archive1.get(), dRes_ID_JB_BDL_JB_e);
    if (!data)
        assertion("d_a_npc_jb1.cpp", 0x469, "a_mdl_dat != NULL");
    archive2->vtable = gc_tu.safeString;
    archive2->text = gabi::guest_addr("Jb");
    void* animation = resource(archive2.get(), dRes_ID_JB_BCK_JB_WAIT01_e);
    f32 speed = l_one;
    void* morph = jbMorfConstruct(nullptr, data, nullptr, nullptr, animation, 2, speed, 0, -1, 1, nullptr,
                                  0x80000, 0x11020022);
    mpMorf = static_cast<mDoExt_McaMorf*>(morph);
    if (!morph)
        return nullptr;
    // HD: destroy the incomplete morph object before clearing the actor pointer.
    if (gabi::ea(jb1::morphModel(morph)) == 0) {
        jb1::destroyMorf(morph, 3);
        mpMorf = 0;
        return nullptr;
    }
    archive3->vtable = gc_tu.safeString;
    archive3->text = gabi::guest_addr("Jb");
    animation = resource(archive3.get(), dRes_ID_JB_BRK_JB_LAMP_e);
    m_lmp_brk = static_cast<J3DAnmTevRegKey*>(animation);
    if (!animation) {
        assertion("d_a_npc_jb1.cpp", 0x47E, "m_lmp_brk != NULL");
        animation = m_lmp_brk.get();
    }
    morph = mpMorf.get();
    void* model = jb1::morphModel(morph);
    void* modelData = jb1::modelData(model);
    // HD: this call passes nine arguments; no GameCube trailing animation-index argument.
    BOOL initialized = jbInitBrk(&mBrkAnm, modelData, animation, 1, 2, speed, 0, -1, 0) != 0;
    if (!initialized) {
        mpMorf = 0;
        return nullptr;
    }
    morph = mpMorf.get();
    field_0x834 = 0;
    model = jb1::morphModel(morph);
    modelData = jb1::modelData(model);
    void* names = jbJointNames(modelData);
    void* nameTable = jb1::nameTable(names);
    s8 joint = jbFindName(nameTable, "jb_case");
    m_cse_jnt_num = joint;
    if (joint < 0)
        assertion("d_a_npc_jb1.cpp", 0x48D, "m_cse_jnt_num >= 0");
    return data;
}

/* 022474D8 */
BOOL daNpc_Jb1_c::create_lgt() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<JbSafeString> archive1, archive2;
    archive1->vtable = gc_tu.safeString;
    archive1->text = gabi::guest_addr("Jb");
    void* data = resource(archive1.get(), dRes_ID_JB_BMD_LF_e);
    void* model = jbModelCreate(data, 0, 0x11020203);
    m_lgt_mdl = static_cast<J3DModel*>(model);
    if (!model)
        assertion("d_a_npc_jb1.cpp", 0x4D8, "m_lgt_mdl != NULL");
    archive2->vtable = gc_tu.safeString;
    archive2->text = gabi::guest_addr("Jb");
    void* animation = resource(archive2.get(), dRes_ID_JB_BRK_LF_e);
    m_lgt_brk = static_cast<J3DAnmTevRegKey*>(animation);
    if (!animation) {
        assertion("d_a_npc_jb1.cpp", 0x4DD, "m_lgt_brk != NULL");
        animation = m_lgt_brk.get();
    }
    model = m_lgt_mdl.get();
    data = jb1::modelData(model);
    f32 speed = l_one;
    // HD: this call passes nine arguments; no GameCube trailing animation-index argument.
    BOOL initialized = jbInitBrk(&mBrkAnm2, data, animation, 1, 2, speed, 0, -1, 0) != 0;
    if (initialized)
        field_0x858 = 0;
    return initialized;
}

/* 022475E0 */
BOOL daNpc_Jb1_c::CreateHeap() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!create_Anm())
        return 0;
    if (!create_lgt())
        return 0;
    f32 zero = l_zero;
    void* morph = mpMorf.get();
    void* model = jb1::morphModel(morph);
    static_cast<J3DModel*>(model)->mUserArea = 0;
    jbCircleWall(&mAcchCir, zero, zero);
    jbAcchSet(&mObjAcch, &current.pos, &old.pos, this, 1, &mAcchCir, &speed, 0, 0);
    return 1;
}

/* 02248094 */
s32 daNpc_Jb1_Create(daNpc_Jb1_c* actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return actor->_create();
}

/* 02248124 */
BOOL daNpc_Jb1_Delete(daNpc_Jb1_c* actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return actor->_delete();
}

/* 022485C0 */
BOOL daNpc_Jb1_Execute(daNpc_Jb1_c* actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return actor->_execute();
}

/* 02248760 */
BOOL daNpc_Jb1_Draw(daNpc_Jb1_c* actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return actor->_draw();
}

/* 02248764 */
BOOL daNpc_Jb1_IsDelete(daNpc_Jb1_c* actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 1;
}

// HD: compiler-generated constructors, deleting destructors and header statics.
/* 02248BA4 */
void daNpc_Jb1_HioDestroy(void* object, s32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (object && (flags & 1))
        jbDelete(object);
}

/* 02248C60 */
void daNpc_Jb1_c::emptyVirtual() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

/* 02248BB8 */
void daNpc_Jb1_Destroy(daNpc_Jb1_c* actor, s32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!actor)
        return;
    jbBckDestroy(&jb1::bck(&actor->mBckAnm)->mAnmVtable, 0);
    jbCylinderDestroy(&actor->mCyl, 2);
    jbStatusDestroy(&actor->mStts, 2);
    jbCircleDestroy(&actor->mAcchCir.m_cir, 2);
    actor->mObjAcch.dBgS_GrpPassChk::__vtbl = 0x1001AC90;
    actor->mObjAcch.dBgS_PolyPassChk::__vtbl = 0x1001ACA0;
    jbAcchDestroy(&actor->mObjAcch, 0);
    jbActorDestroy(actor, 0);
    if (flags & 1)
        jbDelete(actor);
}

/* 0224769C */
daNpc_Jb1_c* daNpc_Jb1_Construct(daNpc_Jb1_c* actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!actor) {
        actor = static_cast<daNpc_Jb1_c*>(jbAllocate(0xD50));
        if (!actor)
            return nullptr;
    }
    jbNpcConstruct(actor);
    actor->__vtbl = 0x1001ADE8;
    // The HD NPC base default record initializes vector, angle, animation and
    // gaze-controller fields before constructing the embedded controllers.
    f32 default00 = l_defaultLight.position.x;
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->position.x = default00;
    f32 default04 = l_defaultLight.position.y;
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->position.y = default04;
    f32 default08 = l_defaultLight.position.z;
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->position.z = default08;
    f32 default0C = l_defaultLight.direction.x;
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->direction.x = default0C;
    f32 default10 = l_defaultLight.direction.y;
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->direction.y = default10;
    f32 default14 = l_defaultLight.direction.z;
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->direction.z = default14;
    u8 default18 = l_defaultLight.color.r;
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->color.r = default18;
    u8 default19 = l_defaultLight.color.g;
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->color.g = default19;
    u8 default1A = l_defaultLight.color.b;
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->color.b = default1A;
    u8 default1B = l_defaultLight.color.a;
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->color.a = default1B;
    s16 default1C = l_defaultLight.materialColor.r;
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->materialColor.r = default1C;
    s16 default1E = l_defaultLight.materialColor.g;
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->materialColor.g = default1E;
    s16 default20 = l_defaultLight.materialColor.b;
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->materialColor.b = default20;
    s16 default22 = l_defaultLight.materialColor.a;
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->materialColor.a = default22;
    f32 default24 = l_defaultLight.parameters[0];
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->parameters[0] = default24;
    f32 default28 = l_defaultLight.parameters[1];
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->parameters[1] = default28;
    f32 default2C = l_defaultLight.parameters[2];
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->parameters[2] = default2C;
    f32 default30 = l_defaultLight.parameters[3];
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->parameters[3] = default30;
    f32 default34 = l_defaultLight.parameters[4];
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->parameters[4] = default34;
    f32 default38 = l_defaultLight.parameters[5];
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->parameters[5] = default38;
    f32 default3C = l_defaultLight.parameters[6];
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->parameters[6] = default3C;
    f32 default40 = l_defaultLight.parameters[7];
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->materialColor.g = default1E;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->parameters[7] = default40;
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->parameters[2] = default2C;
    jb1::lightBlock(&actor->field_0x6DC, 0x0)->parameters[7] = default40;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->position.y = default04;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->color.b = default1A;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->materialColor.r = default1C;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->parameters[6] = default3C;
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->materialColor.b = default20;
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->direction.x = default0C;
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->parameters[1] = default28;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->color.r = default18;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->parameters[1] = default28;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->direction.z = default14;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->parameters[2] = default2C;
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->parameters[3] = default30;
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->parameters[7] = default40;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->parameters[0] = default24;
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->materialColor.r = default1C;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->direction.y = default10;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->direction.x = default0C;
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->materialColor.a = default22;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->position.z = default08;
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->direction.y = default10;
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->parameters[4] = default34;
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->parameters[0] = default24;
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->color.a = default1B;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->materialColor.b = default20;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->parameters[4] = default34;
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->color.g = default19;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->materialColor.a = default22;
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->direction.z = default14;
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->parameters[5] = default38;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->materialColor.g = default1E;
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->color.b = default1A;
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->position.y = default04;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->position.x = default00;
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->position.x = default00;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->color.a = default1B;
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->color.r = default18;
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->parameters[6] = default3C;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->parameters[3] = default30;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->color.g = default19;
    jb1::lightBlock(&actor->field_0x6DC, 0x144)->position.z = default08;
    jb1::lightBlock(&actor->field_0x6DC, 0xC0)->parameters[5] = default38;
    jbFrameConstruct(&actor->mBckAnm, 0);
    jb1::bck(&actor->mBckAnm)->mAnmVtable = 0x1016E54C;
    jbMtxCalcConstruct(&jb1::bck(&actor->mBckAnm)->mMtxCalc);
    jb1::bck(&actor->mBckAnm)->mMtxCalcVtable = 0x1016D820;
    jb1::bck(&actor->mBckAnm)->m58 = 0;
    jb1::bck(&actor->mBckAnm)->m7C = 0;
    jb1::bck(&actor->mBckAnm)->m80 = 0;
    jb1::bck(&actor->mBckAnm)->m88 = 0;
    jb1::bck(&actor->mBckAnm)->m84 = 0;
    jb1::bck(&actor->mBckAnm)->mAnmVtable = 0x1001AC68;
    jbBtkConstruct(&actor->mBtkAnm);
    jbBrkConstruct(&actor->mBrkAnm3);
    actor->field_0x7F8.mHD20 = l_one;
    jbBrkConstruct(&actor->mBrkAnm);
    jbBrkConstruct(&actor->mBrkAnm2);
    jbEventConstruct(&actor->mEventCut);
    return actor;
}

/* 02248A98 */
void* daNpc_Jb1_HioConstruct(void* object) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!object) {
        object = jbAllocate(0x20);
        if (!object)
            return nullptr;
    }
    static_cast<jb1::Hio*>(object)->__vtbl = 0x1001ACB0;
    jbMemcpy(&static_cast<jb1::Hio*>(object)->mPrmTbl, &a_prm_tbl, 0x14);
    static_cast<jb1::Hio*>(object)->mNo = -1;
    static_cast<jb1::Hio*>(object)->field_0x8 = -1;
    return object;
}

/* 02248B04 */
void daNpc_Jb1_StaticInit() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    sinit_header_statics_rodata(gc_tu.headerStatics[0], gc_tu.headerStatics[1], gc_tu.headerStatics[2],
                                gabi::ea(&l_pi));
    daNpc_Jb1_HioConstruct(&l_HIO);
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
data l_HIO 104671F8
data a_prm_tbl 101BEA04
data l_pi 1001ADE0
data l_matrix 1048D0CC
str 1001ADA4 "ActNo"
str 1001ADC8 "Jb" in 02248780
str 1001AD9F "Jb" in 02248168
str 1001AD9C "Jb" in 02248098
str 1001AD88 "Jb1"
str 1001AD8C "Jb" in 02247F64
str 1001AD90 "Jabun" renamed
str 1001ACE8 "Jb" in 02247260
str 1001AD38 "Jb" in 022474D8
str 1001ACE0 "jb_case"
str 1001ACEC "d_a_npc_jb1.cpp"
str 1001AD3C "d_a_npc_jb1.cpp" in 022474D8
str 1001ACFC "a_mdl_dat != NULL" renamed
str 1001AD10 "m_lmp_brk != NULL" renamed
str 1001AD24 "m_cse_jnt_num >= 0" renamed
str 1001AD4C "m_lgt_mdl != NULL" renamed
str 1001AD60 "m_lgt_brk != NULL" renamed
str 1001ACD0 "d_demo.h"
str 1001ACC0 "m_object != NULL" renamed
data l_defaultLight 1016E414
data a_bck_resID_tbl 1001ADC0
data l_anmPrm 101BE9D4
data l_atrPrm 101BE9F4
data l_heapSize 101BE9CC
data l_cutNames 101BE9D0
data l_initialAction 1001AC38
data l_zero 1001AD74
data l_attentionHeight 1001AD78
data l_attentionForward 1001AD7C
data l_lightY 1001AD80
data l_scale 1001AD84
data l_one 1001ACDC
data l_demoObjects 101D5FFC
data l_debugGuard0 101FDB64
data l_debugGuard1 101FDA50
data l_debugGuard2 101FDAC0
data l_debugColor0 101FEC00
data l_debugColor1 101FEBF4
data l_debugColor2 101FEBF8
data l_color0 1001AC40
data l_color1 1001AC44
data l_color2 1001AC48
ext 025200D4 void* jbPlay()
ext 025D76A8 void jbOrderSpeak(daNpc_Jb1_c*)
ext 02542D88 s32 jbStaffId(void*, u32, u32, u32)
ext 0254487C void* jbEventSubstance(void*, s32, const char*, s32)
ext 0259D24C void jbSetNpcAnm(void*, s32, f32, f32, s32, s32, const char*)
ext 024EDFCC BOOL jbLockOn(void*)
ext 024EC8D0 void* jbLockTarget(void*, u32)
ext 024EE464 void* jbAttentionTarget(void*, u32)
ext 0273AF40 void jbDelete(void*)
ext 02542EDC s32 jbActionIndex(void*, s32, void*, s32, u32, u32)
ext 02543280 void jbCutEnd(void*, s32)
ext 025447C8 BOOL jbAdvance(void*, s32)
ext 0259F858 BOOL jbEventCut(dNpc_EventCut_c*)
ext 0273AA24 void jbAssert(const char*, s32, const char*)
ext 02526E70 void jbDemoActor(void*, u32)
ext 02527028 void jbDemoUpdate(daNpc_Jb1_c*, u32, void*, const char*, u32, u32, u32, u32)
ext 025204C8 void jbResDelete(request_of_phase_process_class*, const char*)
ext 0255A374 void jbCutLight(LIGHT_INFLUENCE*)
ext 025E563C void jbStopAnimation(void*)
ext 025F0A18 void jbHioRemove(s32)
ext 027F3628 void jbBckDestroy(be<u32>*, u32)
ext 02515A70 void jbCylinderDestroy(dCcD_Cyl*, u32)
ext 02515860 void jbStatusDestroy(dCcD_Stts*, u32)
ext 02018034 void jbCircleDestroy(void*, u32)
ext 024EFD9C void jbAcchDestroy(dBgS_ObjAcch*, u32)
ext 025D50BC void jbActorDestroy(fopAc_ac_c*, u32)
ext 0259F814 void jbSetActorInfo(dNpc_EventCut_c*, const char*, fopNpc_npc_c*)
ext 025564B4 void jbSetLight(LIGHT_INFLUENCE*)
ext 025E4A54 void jbSetFrame(void*, f32)
ext 025D6870 void jbPositionMove(fopAc_ac_c*, dCcD_Stts*)
ext 02555D0C void* jbEnvironment()
ext 025626A4 void jbSetTev(void*, s32, const cXyz*, dKy_tevstr_c*)
ext 02562F5C void jbApplyTev(void*, void*, dKy_tevstr_c*)
ext 025E83FC void jbEntryBrk(mDoExt_baseAnm*, void*, f32)
ext 025E5590 void jbEntryModel(void*)
ext 025E2E5C void jbModelEntry(void*)
ext C000A848 void jbMemcpy(void*, const void*, u32)
ext 0273AD10 void* jbAllocate(u32)
ext 025A1458 void jbNpcConstruct(fopNpc_npc_c*)
ext 027F2BC0 void jbFrameConstruct(mDoExt_baseAnm*, u32)
ext 027DA984 void jbMtxCalcConstruct(jb1::MtxCalc*)
ext 025E7C6C void jbBtkConstruct(mDoExt_baseAnm*)
ext 025E80D0 void jbBrkConstruct(mDoExt_baseAnm*)
ext 0259F740 void jbEventConstruct(dNpc_EventCut_c*)
ext 028F026C void jbLineConstruct(void*)
ext 028ED6F8 void jbRandomInit(void*)
ext 028EAB2C void jbRandomInit2(void*)
ext 028E93CC void jbMatrixTranslate(void*, f32, f32, f32)
ext 025F1C28 void jbMatrixRotateY(void*, s16)
ext 028E8F64 void jbMatrixVector(void*, const void*, cXyz*)
ext 024EFF44 void jbCircleWall(dBgS_AcchCir*, f32, f32)
ext 024F06B4 void jbAcchSet(dBgS_ObjAcch*, cXyz*, cXyz*, fopAc_ac_c*, u32, dBgS_AcchCir*, cXyz*, u32, u32)
ext 02520460 s32 jbResLoad(request_of_phase_process_class*, const char*)
ext 025F0A10 s32 jbHioAdd(const char*, void*)
ext 025D63E8 BOOL jbEntryHeap(fopAc_ac_c*, u32, u32)
ext 026067F4 void* jbResource(void*, JbSafeString*, s32)
ext 025E4F64 void* jbMorfConstruct(void*, void*, void*, void*, void*, s32, f32, s32, s32, s32, void*, u32, u32)
ext 025E8154 s32 jbInitBrk(mDoExt_baseAnm*, void*, void*, u32, s32, f32, s32, s32, u32)
ext 027F68FC void* jbJointNames(void*)
ext 027DF9B0 s32 jbFindName(void*, const char*)
ext 025E38E0 void* jbModelCreate(void*, u32, u32)
ext 025E535C s32 jbMorfPlay(void*, cXyz*, u32, u32)
ext 024F08A8 void jbCorrectPosition(dBgS_ObjAcch*, void*)
ext 024EF130 s32 jbRoom(void*, void*)
ext 024EEEB8 s32 jbPolyColor(void*, void*)
ext 025E55A0 void jbMorfCalc(void*)
ext 028E90D4 void jbMatrixCopy(void*, void*)
ext 025F24E0 void jbTranslateM(f32, f32, f32)
ext 025F2518 void jbScaleM(f32, f32, f32)
ext 027F4D5C void jbModelCalc(void*)
*/
// clang-format on
