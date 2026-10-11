/* HD Gaship2 full actor reconstruction derived from zeldaret/tww. */
#include "d/actor/d_a_obj_gaship2.h"
const gc_tu_data gc_tu = {.safeString = 0x10029630};
enum { dRes_INDEX_YAKEROM_BDL_YAKEROM_e = 4, dRes_INDEX_YAKEROM_DZB_YAKEROM_e = 7 };
static f32 l_minAngle;
static f32 l_maxAngle;
static u8 l_initA;
static u8 l_initB;
static GashipHeaderZeros l_headerZeros;
/* 0234AA30 */
BOOL daObjGaship2::Act_c::create_heap() {
    auto *data = (J3DModelData *)dComIfG_getObjectRes("YakeRom", dRes_INDEX_YAKEROM_BDL_YAKEROM_e);
    if (!data)
        JUT_ASSERT_fail(STR(0x10029658), 0x5A, STR(0x1002966C));
    mpModel = mDoExt_J3DModel__create(data, 0, 0x11000002);
    set_mtx();
    auto *bgData = (cBgD_t *)dComIfG_getObjectRes("YakeRom", dRes_INDEX_YAKEROM_DZB_YAKEROM_e);
    if (!bgData)
        JUT_ASSERT_fail(STR(0x10029658), 0x67, STR(0x1002967C));
    else {
        mpBgW = new_dBgW();
        if (mpBgW && mpBgW->Set(bgData, dBgW::MOVE_BG_e, &mMtx))
            return FALSE;
    }
    return data && mpModel && bgData && mpBgW;
}
/* 0234AB80 */
BOOL solidHeapCB(fopAc_ac_c *actor) {
    return ((daObjGaship2::Act_c *)actor)->create_heap();
}
/* 0234A93C */
void daObjGaship2::Act_c::set_mtx() {
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    MTXCopy(mDoMtx_stack_c::get(), &mMtx);
    gashipModelCalc(mpModel);
}
/* 0234AB84 */
s32 daObjGaship2::Act_c::_create() {
    fopAcM_ct(this, Act_c);
    s32 phase = dComIfG_resLoad(&mphs, "YakeRom");
    if (phase == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, solidHeapCB, 0))
            return cPhs_ERROR_e;
        J3DModel *model = mpModel;
        cullMtx = model ? gabi::ea(J3DModel_getBaseTRMtx(model)) : 0;
        dComIfG_Bgsp()->Regist(mpBgW, this);
        gashipSetCrrFunc(mpBgW, dBgS_MoveBGProc_Typical);
    }
    return phase;
}
/* 0234AC64 */
BOOL daObjGaship2::Act_c::_delete() {
    if (heap && mpBgW && mpBgW->ChkUsed())
        dComIfG_Bgsp()->Release(mpBgW);
    dComIfG_resDelete(&mphs, "YakeRom");
    return TRUE;
}
/* 0234AD0C */
BOOL daObjGaship2::Act_c::_draw() {
    auto *light = dKy_getEnvlight();
    settingTevStruct(light, 1, &current.pos, &tevStr);
    dComIfGd_setListBG();
    light = dKy_getEnvlight();
    setLightTevColorType(light, mpModel, &tevStr);
    mDoExt_modelUpdateDL(mpModel, 0);
    dComIfGd_setList();
    return TRUE;
}
/* 0234ACD4 */
BOOL daObjGaship2::Act_c::_execute() {
    set_mtx();
    mpBgW->Move();
    return TRUE;
}
/* 0234ADB4 */
void staticInitialize() {

    // HD: preserve the translation unit's header-static zeroing order.
    l_headerZeros.words[2] = 0;
    l_headerZeros.words[0] = 0;
    l_headerZeros.words[3] = 0;
    l_headerZeros.words[1] = 0;
    __register_global_object(0x101C97E4);
    l_minAngle = -3.1415927410125732f;
    l_maxAngle = 3.1415927410125732f;
    gashipInitA(&l_initA);
    __register_global_object(0x101C97F0);
    gashipInitB(&l_initB);
    __register_global_object(0x101C97FC);
}
/* 0234AE48 */
void deleteStatic(void *p, s32 flags) {
    if (p && (flags & 1))
        operator_delete(p);
}
/* 0234AE60 */
void destruct(daObjGaship2::Act_c *actor, s32 flags) {
    if (actor) {
        gashipActorDt(actor, 0);
        if (flags & 1)
            operator_delete(actor);
    }
}
/* 0234AE5C */
void emptyVirtual() {
}
/* 0234AEB4 */
BOOL IsDelete(daObjGaship2::Act_c *actor) {
    return TRUE;
}

/* 0234ADA4 */
s32 createWrapper(daObjGaship2::Act_c *p) {
    return p->_create();
}
/* 0234ADA8 */
BOOL deleteWrapper(daObjGaship2::Act_c *p) {
    return p->_delete();
}
/* 0234ADAC */
BOOL executeWrapper(daObjGaship2::Act_c *p) {
    return p->_execute();
}
/* 0234ADB0 */
BOOL drawWrapper(daObjGaship2::Act_c *p) {
    return p->_draw();
}

daObjGaship2::Act_c::Act_c() {
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}
/* WWHD glue
str 10029698 "YakeRom"
data l_minAngle 10469C3C
data l_maxAngle 10469C40
data l_initA 10469C44
data l_initB 10469C45
data l_headerZeros 10469C48
ext 027F4D5C void gashipModelCalc(J3DModel*)
ext 025D50BC void gashipActorDt(void*, s32)
ext 028ED6F8 void gashipInitA(void*)
ext 028EAB2C void gashipInitB(void*)
*/
