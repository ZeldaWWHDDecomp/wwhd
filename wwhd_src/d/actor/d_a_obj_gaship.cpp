/* HD Gaship full actor reconstruction derived from zeldaret/tww. */
#include "d/actor/d_a_obj_gaship.h"
const gc_tu_data gc_tu = {.safeString = 0x100295B8};
enum { dRes_INDEX_GASHIP_BDL_GASHIP_e = 3, fpcNm_MAJUU_FLAG_e = 0xAE };
static f32 l_minAngle;
static f32 l_maxAngle;
static u8 l_initA;
static u8 l_initB;
static GashipHeaderZeros l_headerZeros;
static GashipFlagOffsets l_flagOffsets;
static GashipFlagAngles l_flagAngles;
static GashipFlagLiteral l_flagLiteral;
static u32 l_offsetGuard;
static u32 l_angleGuard;
/* 0234A3E4 */
BOOL daObjGaship::Act_c::create_heap() {
    auto *data = (J3DModelData *)dComIfG_getObjectRes("GaShip", dRes_INDEX_GASHIP_BDL_GASHIP_e);
    if (!data)
        JUT_ASSERT_fail(STR(0x100295F0), 0x8C, STR(0x100295E0));
    if (data)
        mModel = mDoExt_J3DModel__create(data, 0, 0x11000002);
    set_mtx();
    return data && mModel != nullptr;
}
/* 0234A4B8 */
BOOL solidHeapCB(fopAc_ac_c *actor) {
    return ((daObjGaship::Act_c *)actor)->create_heap();
}
/* 0234A2F0 */
void daObjGaship::Act_c::set_mtx() {
    J3DModel_setBaseScale(mModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    J3DModel_setBaseTRMtx(mModel, mDoMtx_stack_c::get());
    MTXCopy(mDoMtx_stack_c::get(), &mtx);
    gashipModelCalc(mModel);
}
/* 0234A4BC */
s32 daObjGaship::Act_c::_create() {
    fopAcM_ct(this, Act_c);
    s32 phase = dComIfG_resLoad(&mPhs, "GaShip");
    if (phase == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, solidHeapCB, 0))
            return cPhs_ERROR_e;
        J3DModel *model = mModel;
        cullMtx = model ? gabi::ea(J3DModel_getBaseTRMtx(model)) : 0;
    }
    return phase;
}
/* 0234A578 */
BOOL daObjGaship::Act_c::_delete() {
    dComIfG_resDelete(&mPhs, "GaShip");
    return TRUE;
}
/* 0234A7C8 */
BOOL daObjGaship::Act_c::_draw() {
    auto *light = dKy_getEnvlight();
    settingTevStruct(light, 1, &current.pos, &tevStr);
    light = dKy_getEnvlight();
    setLightTevColorType(light, mModel, &tevStr);
    mDoExt_modelUpdateDL(mModel, 0);
    return TRUE;
}
/* 0234A790 */
BOOL daObjGaship::Act_c::_execute() {
    set_mtx();
    birth_flag();
    return TRUE;
}
/* 0234A834 */
void staticInitialize() {

    // HD: preserve the translation unit's header-static zeroing order.
    l_headerZeros.words[2] = 0;
    l_headerZeros.words[0] = 0;
    l_headerZeros.words[3] = 0;
    l_headerZeros.words[1] = 0;
    __register_global_object(0x101C9770);
    l_minAngle = -3.1415927410125732f;
    l_maxAngle = 3.1415927410125732f;
    gashipInitA(&l_initA);
    __register_global_object(0x101C977C);
    gashipInitB(&l_initB);
    __register_global_object(0x101C9788);
}
/* 0234A8C8 */
void deleteStatic(void *p, s32 flags) {
    if (p && (flags & 1))
        operator_delete(p);
}
/* 0234A8E0 */
void destruct(daObjGaship::Act_c *actor, s32 flags) {
    if (actor) {
        gashipActorDt(actor, 0);
        if (flags & 1)
            operator_delete(actor);
    }
}
/* 0234A8DC */
void emptyVirtual() {
}
/* 0234A934 */
BOOL IsDelete(daObjGaship::Act_c *actor) {
    return TRUE;
}

/* 0234A5A8 */
void daObjGaship::Act_c::birth_flag() {
    // HD: snapshot all literal values before either flag's guarded initialization.
    f32 a = l_flagLiteral.secondZ, b = l_flagLiteral.secondY, c = l_flagLiteral.secondX;
    f32 d = l_flagLiteral.firstZ, e = l_flagLiteral.firstY, f = l_flagLiteral.firstX;
    for (s32 i = 0; i < 2; ++i) {
        if (!birthFlag[i]) {
            u32 angleGuard = l_angleGuard;
            if (!l_offsetGuard) {
                l_flagOffsets.values[1].y = b;
                l_flagOffsets.values[0].x = f;
                l_flagOffsets.values[0].y = e;
                l_flagOffsets.values[1].z = a;
                l_flagOffsets.values[1].x = c;
                l_flagOffsets.values[0].z = d;
                l_offsetGuard = 1;
            }
            if (!angleGuard) {
                l_angleGuard = 1;
                csXyz_ct(&l_flagAngles.values[0], 0x222, 0, 0xAAA);
                csXyz_ct(&l_flagAngles.values[1], -0x38E, 0, 0xCCC);
            }
            gabi::Local<cXyz> offset;
            PSMTXMultVec(&mtx, &l_flagOffsets.values[i], offset);
            s8 room = current.roomNo;
            u16 x = l_flagAngles.values[i].x, y = l_flagAngles.values[i].y,
                z = l_flagAngles.values[i].z;
            gabi::Local<csXyz> angle;
            angle->x = x;
            angle->y = y;
            angle->z = z;
            s32 pid = fopAcM_create(fpcNm_MAJUU_FLAG_e, 1, offset, room, angle, nullptr, -1, 0);
            if (pid != -1)
                birthFlag[i] = 1;
        }
    }
}
/* 0234A824 */
s32 createWrapper(daObjGaship::Act_c *p) {
    return p->_create();
}
/* 0234A828 */
BOOL deleteWrapper(daObjGaship::Act_c *p) {
    return p->_delete();
}
/* 0234A82C */
BOOL executeWrapper(daObjGaship::Act_c *p) {
    return p->_execute();
}
/* 0234A830 */
BOOL drawWrapper(daObjGaship::Act_c *p) {
    return p->_draw();
}

daObjGaship::Act_c::Act_c() {
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}
/* WWHD glue
str 10029628 "GaShip"
data l_minAngle 10469BF4
data l_maxAngle 10469BF8
data l_initA 10469BFC
data l_initB 10469BFD
data l_flagOffsets 10469C00
data l_headerZeros 10469C18
data l_flagAngles 10469C28
data l_offsetGuard 10469C34
data l_angleGuard 10469C38
data l_flagLiteral 10029604
ext 027F4D5C void gashipModelCalc(J3DModel*)
ext 025D50BC void gashipActorDt(void*, s32)
ext 028ED6F8 void gashipInitA(void*)
ext 028EAB2C void gashipInitB(void*)
*/
