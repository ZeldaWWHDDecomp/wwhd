/* Tower emergence statue. Private HD reconstruction in tww layout and names. */
#include "d/actor/d_a_obj_doguu_demo.h"
const gc_tu_data gc_tu = {.safeString = 0x100275D4};
enum { dRes_INDEX_DOGUUD_BDL_ESKZTO_e = 4, dRes_INDEX_DOGUUD_DZB_ESKZTO_e = 7 };
static u32 l_demoManager;
static f32 l_minAngle;
static f32 l_maxAngle;
static u8 l_initA;
static u8 l_initB;
static DoguuHeaderZeros l_headerZeros;

/* 023371E8 */
BOOL daObjDoguuD_c::CreateHeap() {
    mUnusedParam = mParameters;
    auto *data =
        (J3DModelData *)dComIfG_getObjectRes(STR(0x10027618), dRes_INDEX_DOGUUD_BDL_ESKZTO_e);
    if (!data)
        JUT_ASSERT_fail(STR(0x10027620), 0x65, STR(0x10027638));
    J3DModel *model = mDoExt_J3DModel__create(data, 0, 0x11020203);
    mpModel = model;
    if (!model)
        return FALSE;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    mDoMtx_stack_c::scaleM(scale.x, scale.y, scale.z);
    MTXCopy(mDoMtx_stack_c::get(), &mMtx);
    dBgW *bg = new_dBgW();
    mpBgW = bg;
    if (!bg)
        return FALSE;
    auto *collision =
        (cBgD_t *)dComIfG_getObjectRes(STR(0x10027618), dRes_INDEX_DOGUUD_DZB_ESKZTO_e);
    bg = mpBgW;
    return !bg->Set(collision, dBgW::MOVE_BG_e, &mMtx);
}
/* 02337328 */
BOOL CheckCreateHeap(fopAc_ac_c *actor) {
    return ((daObjDoguuD_c *)actor)->CreateHeap();
}
/* 0233732C */
void daObjDoguuD_c::set_mtx() {
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
/* 02337404 */
void daObjDoguuD_c::CreateInit() {
    J3DModel *model = mpModel;
    cullMtx = model ? gabi::ea(J3DModel_getBaseTRMtx(model)) : 0;
    fopAcM_setCullSizeBox(this, -30000.0f, -5000.0f, -30000.0f, 30000.0f, 40000.0f, 30000.0f);
    dComIfG_Bgsp()->Regist(mpBgW, this);
    mBgwRegistered = 1;
    set_mtx();
}
/* 02337494 */
s32 Create(daObjDoguuD_c *actor) {
    fopAcM_ct(actor, daObjDoguuD_c);
    actor->mBgwRegistered = 0;
    s32 phase = dComIfG_resLoad(&actor->mPhs, STR(0x100275CC));
    if (phase == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(actor, CheckCreateHeap, 0x1460))
            return cPhs_ERROR_e;
        actor->CreateInit();
    }
    return phase;
}
/* 02337560 */
BOOL Delete(daObjDoguuD_c *actor) {
    // HD: a nonzero registration flag suffices, after the heap check.
    if (actor->heap && actor->mBgwRegistered)
        dComIfG_Bgsp()->Release(actor->mpBgW);
    dComIfG_resDelete(&actor->mPhs, STR(0x1002765C));
    return TRUE;
}
/* 023375C4 */
BOOL Draw(daObjDoguuD_c *actor) {
    auto *env = dKy_getEnvlight();
    settingTevStruct(env, 1, &actor->current.pos, &actor->tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, actor->mpModel, &actor->tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(actor->mpModel, 0);
    dComIfGd_setList();
    return TRUE;
}
/* 0233765C */
BOOL Execute(daObjDoguuD_c *actor) {
    u8 demo = actor->demoActorID;
    // HD: inline demo lookup bounds the ID and asserts on a missing manager.
    if (demo && demo <= 32) {
        u32 object = l_demoManager;
        if (!object) {
            JUT_ASSERT_fail(STR(0x1002760C), 0x23A, STR(0x100275FC));
            object = l_demoManager;
        }
        DoguuDemoActor *playback = doguuGetDemoActor(doguuDemoManager(object), demo);
        if (playback && (playback->mEnable & DoguuDemoActor::ENABLE_SHAPE_e))
            actor->mShape = playback->mShapeId;
    }
    actor->set_mtx();
    return TRUE;
}
/* 02337704 */
void staticInitialize() {
    // HD: preserve the translation unit's header-static zeroing order.
    l_headerZeros.words[2] = 0;
    l_headerZeros.words[0] = 0;
    l_headerZeros.words[3] = 0;
    l_headerZeros.words[1] = 0;
    __register_global_object(0x101C8CD4);
    l_minAngle = -3.1415927410125732f;
    l_maxAngle = 3.1415927410125732f;
    doguuInitA(&l_initA);
    __register_global_object(0x101C8CE0);
    doguuInitB(&l_initB);
    __register_global_object(0x101C8CEC);
}
/* 02337798 */
void deleteStatic(void *object, u32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}
/* 023377AC */
BOOL IsDelete(daObjDoguuD_c *actor) {
    return TRUE;
}
/* 023377B4 */
void destruct(daObjDoguuD_c *actor, u32 flags) {
    if (actor) {
        doguuActorDt(actor, 0);
        if (flags & 1)
            operator_delete(actor);
    }
}
/* 02337808 */
void emptyVirtual() {
}

daObjDoguuD_c::daObjDoguuD_c() {
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}
/* WWHD glue
data l_demoManager 101D5FFC
data l_minAngle 1046965C
data l_maxAngle 10469660
data l_initA 10469664
data l_initB 10469665
data l_headerZeros 10469668
ext 02526E70 DoguuDemoActor* doguuGetDemoActor(void*, u8)
ext 025D50BC void doguuActorDt(void*, s32)
ext 028ED6F8 void doguuInitA(void*)
ext 028EAB2C void doguuInitB(void*)
*/
