/* HD Cafelmp full actor reconstruction derived from zeldaret/tww. */
#include "d/actor/d_a_obj_cafelmp.h"
const gc_tu_data gc_tu = {.safeString = 0x100269F4};
enum { dRes_INDEX_CAFELMP_BDL_YLAMP_e = 3 };
static f32 l_cullLow;
static f32 l_cullHigh;
static f32 l_cullLowY;
static f32 l_cullHighY;
static f32 l_cullFar;
static f32 l_minAngle;
static f32 l_maxAngle;
static u8 l_initA;
static u8 l_initB;
static CafelmpHeaderZeros l_headerZeros;
/* 0232F934 */
BOOL daObjCafelmp_c::CreateHeap() {
    auto *data =
        (J3DModelData *)dComIfG_getObjectRes(STR(0x10026A1C), dRes_INDEX_CAFELMP_BDL_YLAMP_e);
    if (!data)
        JUT_ASSERT_fail(STR(0x10026A24), 0x51, STR(0x10026A38));
    mpModel = mDoExt_J3DModel__create(data, 0, 0x11020203);
    return mpModel != nullptr;
}
/* 0232F9D0 */
BOOL CheckCreateHeap(fopAc_ac_c *actor) {
    return ((daObjCafelmp_c *)actor)->CreateHeap();
}
/* 0232F9D4 */
void daObjCafelmp_c::set_mtx() {
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
/* 0232FAAC */
void daObjCafelmp_c::CreateInit() {
    J3DModel *model = mpModel;
    f32 highY = l_cullHighY, low = l_cullLow, high = l_cullHigh, lowY = l_cullLowY;
    cullMtx = model ? gabi::ea(J3DModel_getBaseTRMtx(model)) : 0;
    fopAcM_setCullSizeBox(this, low, lowY, low, high, highY, high);
    fopAcM_setCullSizeFar(this, l_cullFar);
    set_mtx();
}
/* 0232FB2C */
s32 daObjCafelmp_c::_create() {
    fopAcM_ct(this, daObjCafelmp_c);
    s32 phase = dComIfG_resLoad(&mPhs, STR(0x100269E8));
    if (phase == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0x680))
            return cPhs_ERROR_e;
        CreateInit();
    }
    return phase;
}
/* 0232FBF0 */
BOOL daObjCafelmp_c::_delete() {
    dComIfG_resDelete(&mPhs, STR(0x10026A60));
    return TRUE;
}
/* 0232FC20 */
BOOL daObjCafelmp_c::_draw() {
    auto *light = dKy_getEnvlight();
    settingTevStruct(light, 1, &current.pos, &tevStr);
    light = dKy_getEnvlight();
    setLightTevColorType(light, mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel, 0);
    dComIfGd_setList();
    return TRUE;
}
/* 0232FCB8 */
BOOL daObjCafelmp_c::_execute() {
    u16 angle = current.angle.y;
    current.angle.y = (u16)(angle + 0xDAu);
    set_mtx();
    return TRUE;
}
/* 0232FCE8 */
void staticInitialize() {
    // HD: retain this translation unit's header-static zeroing order.
    l_headerZeros.words[2] = 0;
    l_headerZeros.words[0] = 0;
    l_headerZeros.words[3] = 0;
    l_headerZeros.words[1] = 0;
    __register_global_object(0x101C88EC);
    l_minAngle = -3.1415927410125732f;
    l_maxAngle = 3.1415927410125732f;
    cafelmp_initA(&l_initA);
    __register_global_object(0x101C88F8);
    cafelmp_initB(&l_initB);
    __register_global_object(0x101C8904);
}
/* 0232FD7C */
void deleteStatic(void *p, s32 flags) {
    if (p && (flags & 1))
        operator_delete(p);
}
/* 0232FD98 */
void destruct(daObjCafelmp_c *actor, s32 flags) {
    if (actor) {
        cafelmp_actor_dt(actor, 0);
        if (flags & 1)
            operator_delete(actor);
    }
}
/* 0232FDEC */
void emptyVirtual() {
}
/* 0232FD90 */
BOOL IsDelete(daObjCafelmp_c *actor) {
    return TRUE;
}

daObjCafelmp_c::daObjCafelmp_c() {
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}
/* WWHD glue
data l_cullLow 10026A4C
data l_cullHigh 10026A50
data l_cullLowY 10026A54
data l_cullHighY 10026A58
data l_cullFar 10026A5C
data l_minAngle 10469428
data l_maxAngle 1046942C
data l_initA 10469430
data l_initB 10469431
data l_headerZeros 10469434
ext 025D50BC void cafelmp_actor_dt(void*, s32)
ext 028ED6F8 void cafelmp_initA(void*)
ext 028EAB2C void cafelmp_initB(void*)
*/
