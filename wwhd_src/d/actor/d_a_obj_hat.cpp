/* WWHD traveling merchant hats. Derived game code: */
#include "bindings.h"

struct daObjHat_c : fopAc_ac_c {
    request_of_phase_process_class phase;
    gptr<J3DModel> model;
    gptr<mDoExt_McaMorf> morf;
    Mtx34 matrix;
    dBgS_ObjAcch acch;
    dBgS_AcchCir circle;
    dCcD_Stts status;
    dCcD_Cyl cylinder;
    cXyz moveNorm;
    be<u8> state, hatNo;
    u8 padding[2];
    BOOL createHeap(); u32 getPrmHatNo(); void setSpeed(cXyz*);
    void setMtx(); s32 createInit(); s32 create(); BOOL remove();
    BOOL execute(); BOOL draw();
};
WWHD_OFFSET(daObjHat_c, acch, 0x3EC);
WWHD_OFFSET(daObjHat_c, circle, 0x5B0);
WWHD_OFFSET(daObjHat_c, cylinder, 0x62C);
WWHD_OFFSET(daObjHat_c, moveNorm, 0x75C);
WWHD_SIZE(daObjHat_c, 0x76C);

static void* objectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::ea(arc);
    key->__vtbl = 0x10029EAC;
    u32 control = gabi::load<u32>(0x101F4F28);
    return gabi::call<void*>(0x026067F4, gabi::at<void>(control), key.get(), id);
}
BOOL daObjHat_c::createHeap() {
    WWHD_FUNC(0x0235042C, BOOL, this);
    s32 modelID = gabi::load<s32>(0x10029F04 + 4*hatNo);
    auto data = static_cast<J3DModelData*>(objectIDRes(STR(0x10029F3C), modelID));
    if (data == nullptr) return FALSE;
    s32 animationID = gabi::load<s32>(0x10029F14 + 4*hatNo);
    auto animation = static_cast<J3DAnmTransform*>(objectIDRes(STR(0x10029F3C), animationID));
    auto created = mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, animation,
                                      2, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x37441422);
    morf = created;
    if (created == nullptr || created->mpModel == nullptr) return FALSE;
    model = created->mpModel;
    circle.SetWall(30.0f, 30.0f);
    acch.Set(&current.pos, &old.pos, this, 1, &circle, &speed, &current.angle, &shape_angle);
    return TRUE;
}
VERIFY(0x0235042C, &daObjHat_c::createHeap);
static BOOL CheckCreateHeap(daObjHat_c* p) {
    WWHD_FUNC(0x02350584, BOOL, p);
    return p->createHeap();
}
VERIFY(0x02350584, CheckCreateHeap);
static u32 PrmAbstract(daObjHat_c* p, u32 width, u32 shift) {
    WWHD_FUNC(0x02350DA0, u32, p, width, shift);
    u32 parameters = p->mParameters;
    u32 mask = (width & 32) ? 0 : (1U << (width & 31));
    u32 value = (shift & 32) ? 0 : (parameters >> (shift & 31));
    return value & (mask-1);
}
VERIFY(0x02350DA0, PrmAbstract);
u32 daObjHat_c::getPrmHatNo() {
    WWHD_FUNC(0x02350588, u32, this);
    return PrmAbstract(this, 8, 0)&3;
}
VERIFY(0x02350588, &daObjHat_c::getPrmHatNo);
static daObjHat_c* construct(daObjHat_c* p) {
    WWHD_FUNC(0x023505B4, daObjHat_c*, p);
    if (p == nullptr) {
        p = static_cast<daObjHat_c*>(operator_new(0x76C));
        if (p == nullptr) return p;
    }
    fopAc_ac_c_ct(p);
    p->__vtbl = 0x10029F24;
    dBgS_ObjAcch_ct(&p->acch, {0x10029ED4, 0x10029EF4, 0x10029EE4});
    dBgS_AcchCir_ct(&p->circle);
    dCcD_Stts_ct(&p->status);
    dCcD_Cyl_ct(&p->cylinder, 0x10029EC4);
    p->hatNo = p->getPrmHatNo();
    p->state = 0;
    return p;
}
VERIFY(0x023505B4, construct);
void daObjHat_c::setSpeed(cXyz* velocity) {
    WWHD_FUNC(0x023506CC, void, this, velocity);
    velocity->y = 0.0f;
    gabi::Local<cXyz> normalized;
    gabi::call(0x0201B12C, velocity, normalized.get());
    f32 x = normalized->x, y = normalized->y, z = normalized->z;
    velocity->z = z;
    velocity->y = y;
    velocity->x = x;
    moveNorm.x = x;
    moveNorm.y = y;
    speedF = 20.0f;
    moveNorm.z = z;
    f32 vx = velocity->x*20.0f, vz = velocity->z*20.0f;
    velocity->x = vx;
    velocity->y = 50.0f;
    velocity->z = vz;
    speed.x = vx;
    speed.y = 50.0f;
    speed.z = vz;
}
VERIFY(0x023506CC, &daObjHat_c::setSpeed);
void daObjHat_c::setMtx() {
    WWHD_FUNC(0x02350778, void, this);
    f32 x = scale.x, y = scale.y, z = scale.z;
    J3DModel* m = model;
    gabi::store<f32>(gabi::ea(m)+0xBC, x);
    gabi::store<f32>(gabi::ea(m)+0xC0, y);
    gabi::store<f32>(gabi::ea(m)+0xC4, z);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)(current.angle.y+0x4000));
    gabi::call(0x025F1BF4, mDoMtx_stack_c::get(), (s16)current.angle.x);
    gabi::call(0x025F1C5C, mDoMtx_stack_c::get(), (s16)(current.angle.z+0x4000));
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
}
VERIFY(0x02350778, &daObjHat_c::setMtx);
s32 daObjHat_c::createInit() {
    WWHD_FUNC(0x02350878, s32, this);
    status.Init(2, 0xFF, this);
    cylinder.Set(gabi::at<dCcD_SrcCyl>(0x101C9CD8));
    cylinder.SetStts(&status);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(model));
    gabi::call(0x025D674C, this, -50.0f, 0.0f, -50.0f, 50.0f, 200.0f, 50.0f);
    gravity = -5.0f;
    gabi::Local<gptr<fopAc_ac_c>> parent;
    if (gabi::call<BOOL>(0x025D54C4, (u32)parentActorID, parent.get())) {
        fopAc_ac_c* actor = *parent;
        if (actor != nullptr) {
            gabi::Local<cXyz> wind;
            wind->x = gabi::load<f32>(gabi::ea(actor)+0x894);
            wind->y = gabi::load<f32>(gabi::ea(actor)+0x898);
            wind->z = gabi::load<f32>(gabi::ea(actor)+0x89C);
            setSpeed(wind);
        }
    }
    setMtx();
    return 4;
}
VERIFY(0x02350878, &daObjHat_c::createInit);
s32 daObjHat_c::create() {
    WWHD_FUNC(0x0235096C, s32, this);
    if (!(actor_condition&8)) {
        if (this != nullptr) construct(this);
        actor_condition = actor_condition | 8;
    }
    s32 phaseResult = dComIfG_resLoad(&phase, STR(0x10029F58));
    if (phaseResult == 4) {
        if (fopAcM_entrySolidHeap(this, 0x02350584, 0)) return createInit();
        return 5;
    }
    return phaseResult;
}
VERIFY(0x0235096C, &daObjHat_c::create);
static s32 daSampleCreate(daObjHat_c* p) {
    WWHD_FUNC(0x02350A14, s32, p);
    return p->create();
}
VERIFY(0x02350A14, daSampleCreate);
BOOL daObjHat_c::remove() {
    WWHD_FUNC(0x02350A18, BOOL, this);
    dComIfG_resDelete(&phase, STR(0x10029F5B));
    return TRUE;
}
VERIFY(0x02350A18, &daObjHat_c::remove);
static BOOL daSampleDelete(daObjHat_c* p) {
    WWHD_FUNC(0x02350A48, BOOL, p);
    return p->remove();
}
VERIFY(0x02350A48, daSampleDelete);
BOOL daObjHat_c::execute() {
    WWHD_FUNC(0x02350A4C, BOOL, this);
    // HD has one movement state, so its PTMF entry is fixed.
    ptmf_call(0x101C9CB0, this);
    f32 horizontal = speedF;
    f32 vx = moveNorm.x*horizontal;
    f32 vy = speed.y+gravity;
    f32 minimum = maxFallSpeed;
    f32 vz = moveNorm.z*horizontal;
    if (vy < minimum) vy = minimum;
    speed.x = vx;
    speed.y = vy;
    speed.z = vz;
    gabi::call(0x025D6800, this, &status);
    acch.CrrPos(dComIfG_Bgsp());
    if (acch.ChkWallHit()) speedF = 0.0f;
    if (acch.ChkGroundHit()) gabi::call(0x0200ECD4, &speedF, 0.0f, 0.3f, 1000.0f, 1.0f);
    cylinder.SetC(&current.pos);
    gabi::call(0x0200E240, dComIfG_Ccsp(), &cylinder);
    if (cylinder.ChkTgHit()) {
        gabi::Local<cXyz> wind;
        wind->z = cylinder.mGObjTg.mRVec.z;
        wind->y = cylinder.mGObjTg.mRVec.y;
        wind->x = cylinder.mGObjTg.mRVec.x;
        setSpeed(wind);
    }
    setMtx();
    return TRUE;
}
VERIFY(0x02350A4C, &daObjHat_c::execute);
static BOOL daSampleExecute(daObjHat_c* p) {
    WWHD_FUNC(0x02350BEC, BOOL, p);
    return p->execute();
}
VERIFY(0x02350BEC, daSampleExecute);
BOOL daObjHat_c::draw() {
    WWHD_FUNC(0x02350BF0, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), 0, &current.pos, &tevStr);
    auto light = dKy_getEnvlight();
    setLightTevColorType(light, model, &tevStr);
    mDoExt_McaMorf* animation = morf;
    animation->updateDL();
    return TRUE;
}
VERIFY(0x02350BF0, &daObjHat_c::draw);
static BOOL daSampleDraw(daObjHat_c* p) {
    WWHD_FUNC(0x02350C48, BOOL, p);
    return p->draw();
}
VERIFY(0x02350C48, daSampleDraw);
static void sinit() {
    WWHD_FUNC(0x02350C4C, void, (u32)0);
    sinit_header_statics(0x10469DB4, 0x101C9D1C);
}
VERIFY(0x02350C4C, sinit);
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02350CE0, void, p, flags);
    if (p && (flags&1)) operator_delete(p);
}
VERIFY(0x02350CE0, trivial_dt);
static BOOL daSampleIsDelete(daObjHat_c* p) {
    WWHD_FUNC(0x02350CF4, BOOL, p);
    return TRUE;
}
VERIFY(0x02350CF4, daSampleIsDelete);
static void executeNormal(daObjHat_c* p) {
    WWHD_FUNC(0x02350CFC, void, p);
}
VERIFY(0x02350CFC, executeNormal);
static void destroy(daObjHat_c* p, s32 flags) {
    WWHD_FUNC(0x02350D00, void, p, flags);
    if (p != nullptr) {
        dCcD_Cyl_dt(&p->cylinder, 2);
        dCcD_Stts_dt(&p->status, 2);
        gabi::call(0x02018034, &p->circle.m_cir, 2);
        gabi::store<u32>(gabi::ea(&p->acch)+0x20, 0x10029EE4);
        gabi::store<u32>(gabi::ea(&p->acch)+0x14, 0x10029EF4);
        gabi::call(0x024EFD9C, &p->acch, 0);
        gabi::call(0x025D50BC, p, 0);
        if (flags&1) operator_delete(p);
    }
}
VERIFY(0x02350D00, destroy);
static void emptyVirtual(void* p) {
    WWHD_FUNC(0x02350D9C, void, p);
}
VERIFY(0x02350D9C, emptyVirtual);
