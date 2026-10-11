/* Hami3 rotating grate. All 27 original entry points retain verified HD behavior.
 * HD: name lookup uses relative name tables; callbacks mark matrix buffers dirty.
 * HD: the joint rotates around Y while the actor X angle follows its home angle. */
#include "d/actor/d_a_obj_hami3.h"
namespace daObjHami3 {
static GuestText M_arcname;
static GuestText M_evname;
static GuestText jointName;
static GuestText assertFile1;
static GuestText assertText1;
static GuestText assertFile2;
static GuestText assertText2;
static ActorVtable actorVtable;
static u32 safeStringVtable;
static u32 currentModel;
static u32 calcMatrix;
static u32 saveInfo;
static u32 resourceControl;
static u32 currentOpaList;
static u32 currentXluList;
static Mtx34 matrixStack;
static Mtx34 M_tmp_mtx;
static Mtx34 currentMatrix;
static f32 lowerBound;
static f32 upperBound;
static f32 cullMin;
static f32 cullMax;
// HD: aliases preserve the original static bytes; sinit initializes them at runtime.
static u32 objectIds[4] = {};
static u8 vectorInit;
static u8 angleInit;
static u32 vectorDtor;
static u32 angleDtor;
static u32 matrixDtor;

// HD: each access calls the getter again, preserving callee clobbers.
static u32 play() { return hami3_call::getPlay(); }
// Snapshot all FPR-loaded values before any store, preserving overlap and SNaNs.
static void matrix_copy(u32 src, u32 dst) {
    J3DModel::mtx_assign(gabi::at<Mtx34>(dst), gabi::at<Mtx34>(src));
}
static void order(Act_c *a) {
    hami3_call::orderOtherEvent(a, (s16)a->field_0x2DC, 0xff, 0xffff, 0, 1);
}
/* 0234F8F8 */
static s32 parameter(Act_c *a, u32 width, u32 shift) {
    // PPC variable shifts use the low six bits and yield zero for 32..63.
    u32 w = width & 63, s = shift & 63;
    return ((s < 32 ? (u32)a->mParameters >> s : 0) & ((w < 32 ? 1u << w : 0) - 1));
}
/* 0234EC78 */
static BOOL nodeCallBack(void *node, s32 timing) {
    if (timing == 0) {
        JointIndex *joint = gabi::at<JointIndex>(hami3_call::getJointData(node));
        J3DModel *model = gabi::at<J3DModel>(currentModel);
        Act_c *actor = gabi::at<Act_c>(model->mUserArea);
        u32 index = joint->mJointNo * sizeof(Mtx34);
        if (actor) {
            J3DMtxBuffer_hd *matrices = model->mpMtxBuffer;
            u16 flags = matrices->mFlags;
            u32 base = matrices->mpAnmMtx;
            matrices->mFlags = flags | MATRIX_DIRTY;
            hami3_call::matrixCopy(base + index, (u32)calcMatrix);
            u32 calc = calcMatrix;
            s16 angle = actor->field_0x2C8;
            hami3_call::matrixRotateY(calc, angle);
            matrices = model->mpMtxBuffer;
            flags = matrices->mFlags;
            calc = calcMatrix;
            base = matrices->mpAnmMtx;
            matrices->mFlags = flags | MATRIX_DIRTY;
            matrix_copy(calc, base + index);
            hami3_call::matrixCopy((u32)calcMatrix, gabi::ea(&currentMatrix));
        }
    }
    return 1;
}
/* 0234ED9C */
s32 daObjHami3::Act_c::Mthd_Create() {
    u32 self = gabi::ea(this), flags = (u32)actor_condition;
    if (!(flags & ACTOR_INITIALIZED)) {
        if (self) {
            hami3_call::constructMoveBgActor(this);
            flags = (u32)actor_condition;
            __vtbl = gabi::ea(&actorVtable);
        }
        actor_condition = flags | ACTOR_INITIALIZED;
    }
    s32 phase = hami3_call::resLoad(gabi::ea(&field_0x2CC), gabi::ea(M_arcname.bytes));
    if (phase == cPhs_COMPLEATE_e) {
        phase = hami3_call::moveBGCreate(this, gabi::ea(M_arcname.bytes),
                                         dRes_INDEX_HAMI3_DZB_HAMI3, 0x024EE658u, 0x1fc0);
        if (phase != cPhs_COMPLEATE_e && phase != cPhs_ERROR_e)
            hami3_call::assertFail(gabi::ea(assertFile1.bytes), 0xbf, gabi::ea(assertText1.bytes));
    }
    return phase;
}
/* 0234EE70 */
BOOL daObjHami3::Act_c::Mthd_Delete() {
    BOOL result = hami3_call::moveBGDelete(this);
    hami3_call::resDelete(gabi::ea(&field_0x2CC), gabi::ea(M_arcname.bytes));
    return result;
}
/* 0234EEBC */
BOOL daObjHami3::Act_c::CreateHeap() {
    gabi::Local<SafeString> name;
    name->mStringTop = gabi::ea(M_arcname.bytes);
    name->__vtbl = gabi::ea(&safeStringVtable);
    J3DModelData *data = gabi::at<J3DModelData>(hami3_call::getObjectResource(
        (u32)resourceControl, name.get(), dRes_INDEX_HAMI3_BDL_HAMI3));
    if (!data)
        hami3_call::assertFail(gabi::ea(assertFile2.bytes), 0x71, gabi::ea(assertText2.bytes));
    J3DModel *model = gabi::at<J3DModel>(hami3_call::createModel(data, 0, 0x11020203u));
    field_0x2D4 = model;
    if (!model)
        return 0;
    // HD: resource names use a relative table, replacing the retail joint-name scan.
    NameTableView *table =
        gabi::at<NameTableView>(hami3_call::getJointNameTable(model->mModelData.get()));
    u32 offset = table->mNamesOffset;
    u32 names = offset ? gabi::ea(&table->mNamesOffset) + offset : 0;
    s32 index = hami3_call::getNameIndex(names, gabi::ea(jointName.bytes));
    if (index >= 0) {
        model = field_0x2D4;
        data = model->mModelData;
        u32 count = data->mJointNum;
        u32 joints = data->mJointNodes;
        u32 i = (u16)index;
        if (i < count)
            joints += i * sizeof(JointRecord);
        gabi::at<JointRecord>(joints)->mCallback = gabi::guest_fn((const void *)nodeCallBack);
    }
    model = field_0x2D4;
    model->mUserArea = gabi::ea(this);
    return 1;
}
/* 0234EFD4 */
void daObjHami3::Act_c::set_mtx() {
    f32 x = (f32)current.pos.x, z = (f32)current.pos.z, y = (f32)current.pos.y;
    hami3_call::matrixTranslate(gabi::ea(&matrixStack), x, y, z);
    s16 ax = (s16)shape_angle.x, az = (s16)shape_angle.z, ay = (s16)shape_angle.y;
    hami3_call::matrixRotateZXY(gabi::ea(&matrixStack), ax, ay, az);
    matrix_copy(gabi::ea(&matrixStack), gabi::ea(&field_0x2D4->mBaseTransformMtx));
    hami3_call::matrixCopy(gabi::ea(&matrixStack), gabi::ea(&M_tmp_mtx));
}
/* 0234F0A8 */
void daObjHami3::Act_c::init_mtx() {
    J3DModel *model = field_0x2D4;
    f32 y = (f32)scale.y, x = (f32)scale.x, z = (f32)scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    set_mtx();
}
/* 0234F0C8 */
BOOL daObjHami3::Act_c::Create() {
    J3DModel *model = field_0x2D4;
    cullMtx = model ? gabi::ea(&model->mBaseTransformMtx) : 0;
    s32 sw = parameter(this, Act_c::PRM_SWSAVE_W, Act_c::PRM_SWSAVE_S);
    SaveView *save = gabi::at<SaveView>(saveInfo);
    s8 room = (s8)home.roomNo;
    BOOL on = hami3_call::isSwitch(gabi::ea(save->mMemory), sw, room);
    field_0x2D8 = on ? Act_c::OPEN_STOP : Act_c::CLOSE_STOP;
    s16 homeAngle = (s16)home.angle.x;
    field_0x2C8 = on ? OPEN_ANGLE : 0;
    shape_angle.x = homeAngle + (on ? OPEN_ANGLE : 0);
    init_mtx();
    hami3_call::setCullBox(this, -1200.f, -1200.f, -1200.f, 1200.f, 1200.f, 1200.f);
    PlayView *p = gabi::at<PlayView>(play());
    field_0x2DC =
        hami3_call::getEventIndex(gabi::ea(p->mEventManager), gabi::ea(M_evname.bytes), 0xff);
    return 1;
}
/* 0234F210 */
void daObjHami3::Act_c::daObjHami3_close_stop() {
    s32 sw = parameter(this, Act_c::PRM_SWSAVE_W, Act_c::PRM_SWSAVE_S);
    s8 room = (s8)home.roomNo;
    SaveView *save = gabi::at<SaveView>(saveInfo);
    if (hami3_call::isSwitch(gabi::ea(save->mMemory), sw, room)) {
        order(this);
        field_0x2D8 = Act_c::OPEN_DEMO_WAIT;
    }
}
/* 0234F290 */
void daObjHami3::Act_c::daObjHami3_open_demo_wait() {
    if ((u16)eventInfo.mCommand == dEvtCmd_INDEMO_e) {
        if (parameter(this, Act_c::PRM_SOUND_W, Act_c::PRM_SOUND_S) == 0)
            hami3_call::startSystemSound(JA_SE_READ_RIDDLE_1);
        field_0x2D8 = Act_c::OPEN_DEMO;
    } else
        order(this);
}
static void shock_reset() {
    PlayView *p = gabi::at<PlayView>(play());
    gabi::Local<cXyz> direction;
    direction->set(0.f, 1.f, 0.f);
    hami3_call::startShock(gabi::ea(p->mVibration), 4, -0x21, direction.get());
    p = gabi::at<PlayView>(play());
    p->mEventFlags |= EVENT_RESET;
}
static void sound(Act_c *a, s8 room) {
    s32 reverb = hami3_call::getReverb(room);
    hami3_call::startPositionSound(JA_SE_OBJ_AMI_MOVE, gabi::ea(&a->current.pos), 0, reverb);
}
/* 0234F318 */
void daObjHami3::Act_c::daObjHami3_open_demo() {
    s16 angle = field_0x2C8;
    s8 room = (s8)current.roomNo;
    field_0x2C8 = angle + ROTATION_STEP;
    sound(this, room);
    if ((s16)field_0x2C8 >= OPEN_ANGLE) {
        field_0x2C8 = OPEN_ANGLE;
        shock_reset();
        s32 sw = parameter(this, Act_c::PRM_SWSAVE2_W, Act_c::PRM_SWSAVE2_S);
        room = (s8)home.roomNo;
        SaveView *save = gabi::at<SaveView>(saveInfo);
        hami3_call::onSwitch(gabi::ea(save->mMemory), sw, room);
        field_0x2D8 = Act_c::OPEN_STOP;
    }
}
/* 0234F3F8 */
void daObjHami3::Act_c::daObjHami3_open_stop() {
    s32 sw = parameter(this, Act_c::PRM_SWSAVE_W, Act_c::PRM_SWSAVE_S);
    SaveView *save = gabi::at<SaveView>(saveInfo);
    s8 room = (s8)home.roomNo;
    if (!hami3_call::isSwitch(gabi::ea(save->mMemory), sw, room)) {
        order(this);
        field_0x2D8 = Act_c::CLOSE_DEMO_WAIT;
    }
}
/* 0234F478 */
void daObjHami3::Act_c::daObjHami3_close_demo_wait() {
    if ((u16)eventInfo.mCommand == dEvtCmd_INDEMO_e)
        field_0x2D8 = Act_c::CLOSE_DEMO;
    else
        order(this);
}
/* 0234F4AC */
void daObjHami3::Act_c::daObjHami3_close_demo() {
    s8 room = (s8)current.roomNo;
    s16 angle = field_0x2C8;
    field_0x2C8 = angle - ROTATION_STEP;
    sound(this, room);
    if ((s16)field_0x2C8 <= 0) {
        field_0x2C8 = 0;
        shock_reset();
        field_0x2D8 = Act_c::CLOSE_STOP;
    }
}
/* 0234F564 */
BOOL daObjHami3::Act_c::Execute(be<u32> *output) {
    switch ((s32)field_0x2D8) {
    case Act_c::CLOSE_STOP:
        daObjHami3_close_stop();
        break;
    case Act_c::OPEN_DEMO_WAIT:
        daObjHami3_open_demo_wait();
        break;
    case Act_c::OPEN_DEMO:
        daObjHami3_open_demo();
        break;
    case Act_c::OPEN_STOP:
        daObjHami3_open_stop();
        break;
    case Act_c::CLOSE_DEMO_WAIT:
        daObjHami3_close_demo_wait();
        break;
    case Act_c::CLOSE_DEMO:
        daObjHami3_close_demo();
        break;
    }
    s16 angle = field_0x2C8, homeAngle = (s16)home.angle.x;
    shape_angle.x = homeAngle + angle;
    set_mtx();
    *output = gabi::ea(&M_tmp_mtx);
    return 1;
}
/* 0234F724 */
BOOL daObjHami3::Act_c::Draw() {
    u32 env = hami3_call::getEnvironmentLight();
    hami3_call::settingTevStruct(env, 1, gabi::ea(&current.pos), gabi::ea(&tevStr));
    env = hami3_call::getEnvironmentLight();
    J3DModel *model = field_0x2D4;
    hami3_call::setLightTevColor(env, model, gabi::ea(&tevStr));
    DrawLists *p = gabi::at<DrawLists>(play());
    currentOpaList = (u32)p->mOpaList;
    p = gabi::at<DrawLists>(play());
    currentXluList = (u32)p->mXluList;
    model = field_0x2D4;
    hami3_call::modelUpdate(model, 0);
    p = gabi::at<DrawLists>(play());
    currentOpaList = (u32)p->mDefaultOpaList;
    p = gabi::at<DrawLists>(play());
    currentXluList = (u32)p->mDefaultXluList;
    return 1;
}
/* 0234F89C */
BOOL daObjHami3::Act_c::Delete() { return 1; }
/* 0234F7BC */
BOOL create_adapter(Act_c *a) { return a->Mthd_Create(); }
/* 0234F7C0 */
BOOL delete_adapter(Act_c *a) { return a->Mthd_Delete(); }
/* 0234F7C4 */
BOOL execute_adapter(Act_c *a) { return hami3_call::moveBGExecute(a); }
/* 0234F7C8 */
BOOL draw_adapter(Act_c *a) {
    ActorVtable *vtable = gabi::at<ActorVtable>(a->__vtbl);
    return gabi::call_ptr<BOOL>(vtable->mDraw.get(), a);
}
/* 0234F7D8 */
BOOL isdelete_adapter(Act_c *a) {
    ActorVtable *vtable = gabi::at<ActorVtable>(a->__vtbl);
    return gabi::call_ptr<BOOL>(vtable->mIsDelete.get(), a);
}
/* 0234F7E8 */
// HD: compiler-generated and template code of this translation unit.
void sinit() {
    for (int i = 0; i < 4; ++i)
        objectIds[i] = 0;
    hami3_call::registerDestructor(gabi::ea(&vectorDtor));
    f32 lo = lowerBound, hi = upperBound;
    cullMin = lo;
    cullMax = hi;
    hami3_call::constructVector(gabi::ea(&vectorInit));
    hami3_call::registerDestructor(gabi::ea(&angleDtor));
    hami3_call::constructAngles(gabi::ea(&angleInit));
    hami3_call::registerDestructor(gabi::ea(&matrixDtor));
}
/* 0234F87C */
void static_destructor(void *a, u32 flags) {
    if (a && (flags & 1))
        hami3_call::deleteObject(a);
}
/* 0234F890 */
BOOL IsDelete(Act_c *a) { return 1; }
/* 0234F898 */
void noop(void *a) {}
/* 0234F8A4 */
void destructor(Act_c *a, u32 flags) {
    if (a) {
        hami3_call::destroyActor(a, 0);
        if (flags & 1)
            hami3_call::deleteObject(a);
    }
}
} // namespace daObjHami3

using namespace daObjHami3;
/* WWHD glue (tools/verify/autoglue.py)
data M_arcname 10029D98 nocheck
data M_evname 10029D84 nocheck
data jointName 10029D4C nocheck
data assertFile1 10029CF4 nocheck
data assertText1 10029D08 nocheck
data assertFile2 10029D54 nocheck
data assertText2 10029D68 nocheck
data actorVtable 10029DA0 nocheck
data safeStringVtable 10029CD4
data currentModel 104B462C
data calcMatrix 1018C7B0
data saveInfo 101F84DC
data resourceControl 101F4F28
data currentOpaList 104B4634
data currentXluList 104B4638
data matrixStack 1048D0CC
data M_tmp_mtx 10469D68
data currentMatrix 104B4868
data lowerBound 10029D90
data upperBound 10029D94
data cullMin 10469D4C
data cullMax 10469D50
data objectIds 10469D58 nocheck
data vectorInit 10469D54
data angleInit 10469D55
data vectorDtor 101C9BC8
data angleDtor 101C9BD4
data matrixDtor 101C9BE0
*/
