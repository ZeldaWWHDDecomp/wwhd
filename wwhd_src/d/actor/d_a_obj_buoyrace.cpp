/* WWHD race buoy. */
#include "d/actor/d_a_obj_buoyrace.h"
static BuoyHeaderZeros l_headerZeros;
static u8 l_registrationZ;
static u8 l_registrationA;
static u8 l_registrationB;
static f32 l_minAngle;
static f32 l_maxAngle;
static u8 l_initA;
static u8 l_initB;
namespace daObjBuoyrace {

/* 0232EDE0 */
s32 Act_c::create_load() {
    s32 phase = dComIfG_resLoad(&mPhsKiba, "Kkiba_00");
    if (phase == 4)
        phase = dComIfG_resLoad(&mPhsHasi, "Khasi_00");
    return phase;
}
/* 0232EE30 */
bool Act_c::create_heap() {
    auto *kiba = (J3DModelData *)dComIfG_getObjectRes("Kkiba_00", 4, 0x10026898);
    if (!kiba)
        JUT_ASSERT_fail(STR(0x100268C0), 0x77, STR(0x100268D8));
    mpModelKiba = mDoExt_J3DModel__create(kiba, 0x80000, 0x11000022);
    auto *hasi = (J3DModelData *)dComIfG_getObjectRes("Khasi_00", 3, 0x10026898);
    if (!hasi)
        JUT_ASSERT_fail(STR(0x100268C0), 0x80, STR(0x100268EC));
    J3DModel *second = mDoExt_J3DModel__create(hasi, 0x80000, 0x11000022);
    J3DModel *first = mpModelKiba;
    mpModelHasi = second;
    return first && second;
}
/* 0232EF30 */
static BOOL solidHeapCB(fopAc_ac_c *base) {
    return static_cast<Act_c *>(base)->create_heap();
}
/* 0232EF34 */
void Act_c::set_water_pos() {
    f32 x = current.pos.x, z = current.pos.z;
    f32 h1 = daSea_calcWave(x - 32.0f, z - 32.0f);
    x = current.pos.x;
    z = current.pos.z;
    f32 h2 = daSea_calcWave(x - 32.0f, z + 32.0f);
    x = current.pos.x;
    z = current.pos.z;
    f32 h3 = daSea_calcWave(x + 32.0f, z - 32.0f);
    gabi::Local<cXyz> alongZ, alongX, cross;
    alongZ->x = 0.0f;
    alongZ->y = h2 - h1;
    alongZ->z = 32.0f;
    alongX->x = 32.0f;
    alongX->y = h3 - h1;
    alongX->z = 0.0f;
    mMeanSeaHeight = ((h1 + h2) + h3) * 0.3333333432674408f;
    buoyOutprod(alongZ.get(), cross.get(), alongX.get());
    m294.copy(*cross);
    buoyNormalize(&m294, cross.get());
}
/* 0232F918 */
static s32 parameter(Act_c *actor, s32 width, s32 shift) {
    u32 value = actor->mParameters, mask = (width & 32) ? 0 : (1u << (width & 31));
    return ((shift & 32) ? 0 : (value >> (shift & 31))) & (mask - 1);
}
/* 0232F05C */
void Act_c::set_rope_pos() {
    auto *parent = reinterpret_cast<BuoyGoalFlag *>(fopAcM_SearchByID(parentActorID));
    if (!parent)
        return;
    s32 index = parameter(this, PRM_ID_W, PRM_ID_S);
    u32 line = parameter(this, PRM_LINE_W, PRM_LINE_S);
    if (line >= 4)
        JUT_ASSERT_fail(STR(0x10026910), 0x9B, STR(0x10026930));
    auto *point = parent->getRopePos(line, index);
    if (!point)
        JUT_ASSERT_fail(STR(0x1002694C), 0x194, STR(0x10026920));
    gabi::Local<cXyz> offset;
    offset->x = 0.0f;
    offset->y = 610.0f;
    offset->z = 0.0f;
    PSMTXMultVec(mDoMtx_stack_c::get(), offset.get(), point);
}
/* 0232F170 */
void Act_c::set_mtx() {
    gabi::Local<cXyz> modelScale, direction, offset, translated;
    gabi::Local<Quaternion> rotation;
    J3DModel *kiba = mpModelKiba;
    buoyScale(&scale, modelScale.get(), 2.0f);
    f32 sx = modelScale->x, sy = modelScale->y, sz = modelScale->z;
    kiba->mBaseScale.x = sx;
    kiba->mBaseScale.y = sy;
    kiba->mBaseScale.z = sz;
    sx = scale.x;
    sz = scale.z;
    sy = scale.y;
    f32 hx = (sx + sx) * 1.75f, hz = (sz + sz) * 1.75f, hy = (sy + sy) * 2.5f;
    J3DModel *hasi = mpModelHasi;
    // HD: preserve the hasi model scale stores in z, x, y order.
    hasi->mBaseScale.z = hz;
    hasi->mBaseScale.x = hx;
    hasi->mBaseScale.y = hy;
    auto *stack = mDoMtx_stack_c::get();
    PSMTXTrans(stack, (f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
    direction->x = (f32)m2AC;
    direction->y = 1.0f;
    direction->z = (f32)m2B0;
    mDoMtx_stack_c::transM(0.0f, 150.0f, 0.0f);
    buoyQuatRotBaseY(rotation.get(), direction.get());
    mDoMtx_stack_c::quatM(rotation.get());
    mDoMtx_ZXYrotM(stack, (s16)shape_angle.x, (s16)shape_angle.y, (s16)shape_angle.z);
    mDoMtx_stack_c::transM(0.0f, -150.0f, 0.0f);
    J3DModel_setBaseTRMtx(mpModelKiba, stack);
    set_rope_pos();
    offset->x = 0.0f;
    offset->y = 290.0f;
    offset->z = 0.0f;
    PSMTXMultVecSR(stack, offset.get(), translated.get());
    f32 tx = translated->x, ty = translated->y, tz = translated->z;
    f32 y = stack->m[1][3], x = stack->m[0][3];
    x = x + tx;
    y = y + ty;
    f32 z = stack->m[2][3];
    z = z + tz;
    stack->m[0][3] = x;
    stack->m[1][3] = y;
    stack->m[2][3] = z;
    J3DModel_setBaseTRMtx(mpModelHasi, stack);
}
/* 0232F3D4 */
void Act_c::init_mtx() {
    set_mtx();
}
/* 0232F3D8 */
s32 Act_c::_create() {
    fopAcM_ct(this, Act_c);
    s32 phase = create_load();
    if (phase != 4)
        return phase;
    if (!fopAcM_entrySolidHeap(this, solidHeapCB, 0x980))
        return 5;
    set_water_pos();
    f32 bob = gabi::fmadds(0.5f, cM_ssin((u16)m2A0), -120.0f);
    current.pos.y = (f32)mMeanSeaHeight + (bob + bob);
    J3DModel *model = mpModelKiba;
    cullMtx = model ? gabi::ea(&model->mBaseTransformMtx) : 0;
    fopAcM_setCullSizeBox(this, -152.0f, -2.0f, -152.0f, 152.0f, 590.0f, 152.0f);
    init_mtx();
    return phase;
}
/* 0232F508 */
bool Act_c::_delete() {
    dComIfG_resDelete(&mPhsKiba, "Kkiba_00");
    dComIfG_resDelete(&mPhsHasi, "Khasi_00");
    return true;
}
/* 0232F554 */
void Act_c::afl_calc_sway() {
    f32 x = (f32)m294.x * 0.3499999940395355f, z = (f32)m294.z * 0.3499999940395355f;
    // HD: retain the fused instructions used by the sway integrator.
    f32 square = gabi::fmadds(x, x, z * z);
    if (square > 0.09000000357627869f) {
        f32 length = buoySqrt(square);
        f32 ratio = 0.30000001192092896f / length;
        z = z * ratio;
        x = x * ratio;
    }
    f32 oldX = m2AC, vx = m2B4, vz = m2B8, oldZ = m2B0;
    f32 dampingX = -(vx * 0.03999999910593033f), dampingZ = -(vz * 0.03999999910593033f);
    f32 accelX = gabi::fnmsubs(oldX - x, 0.019999999552965164f, dampingX);
    f32 accelZ = gabi::fnmsubs(oldZ - z, 0.019999999552965164f, dampingZ);
    vx = vx + accelX;
    vz = vz + accelZ;
    m2B4 = vx;
    m2B8 = vz;
    m2AC = oldX + vx;
    m2B0 = oldZ + vz;
}
/* 0232F650 */
void Act_c::afl_calc() {
    f32 old = m2BC, velocity = m2C0;
    velocity = gabi::fmadds(old, -0.009999999776482582f, velocity) * 0.9399999976158142f;
    f32 bob = old + velocity;
    m2C0 = velocity;
    if (bob > 30.0f)
        bob = 30.0f;
    m2BC = bob;
    f32 random = cM_rnd();
    s16 increment = (s16)gabi::ftoi(1000.0f * (random + 1.0f));
    u16 phase = u16((s16)m2A0 + increment);
    f32 mean = mMeanSeaHeight;
    m2A0 = phase;
    f32 wave = gabi::fmadds(0.5f, cM_ssin(phase), -120.0f);
    f32 target = (mean + (f32)m2BC) + (wave + wave);
    cLib_chaseF(&current.pos.y, target, 50.0f);
    afl_calc_sway();
}
/* 0232F748 */
bool Act_c::_execute() {
    set_water_pos();
    afl_calc();
    set_mtx();
    return true;
}
/* 0232F788 */
bool Act_c::_draw() {
    dKy_tevstr_c *tev = &tevStr;
    auto *env = dKy_getEnvlight();
    settingTevStruct(env, 0, &current.pos, tev);
    env = dKy_getEnvlight();
    J3DModel *model = mpModelKiba;
    setLightTevColorType(env, model, tev);
    env = dKy_getEnvlight();
    model = mpModelHasi;
    setLightTevColorType(env, model, tev);
    model = mpModelKiba;
    mDoExt_modelUpdateDL(model, 0);
    model = mpModelHasi;
    mDoExt_modelUpdateDL(model, 0);
    return true;
}
/* 0232F800 */
static s32 create(Act_c *actor) {
    return actor->_create();
}
/* 0232F804 */
static bool remove(Act_c *actor) {
    return actor->_delete();
}
/* 0232F808 */
static bool execute(Act_c *actor) {
    return actor->_execute();
}
/* 0232F80C */
static bool draw(Act_c *actor) {
    return actor->_draw();
}
/* 0232F810 */
static void static_init() {
    // HD: initialize Z storage before the P angle constants.
    l_headerZeros.words[2] = 0;
    l_headerZeros.words[0] = 0;
    l_headerZeros.words[3] = 0;
    l_headerZeros.words[1] = 0;
    __register_global_object(gabi::ea(&l_registrationZ));
    l_minAngle = -3.1415927410125732f;
    l_maxAngle = 3.1415927410125732f;
    buoyInitA(&l_initA);
    __register_global_object(gabi::ea(&l_registrationA));
    buoyInitB(&l_initB);
    __register_global_object(gabi::ea(&l_registrationB));
}
/* 0232F8A4 */
static void empty_destructor(u8 *object, s32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}
/* 0232F8B8 */
static void empty_inline(Act_c *actor) {
}
/* 0232F8BC */
static void actor_destructor(Act_c *actor, s32 flags) {
    if (actor) {
        buoyActorDt(actor, 0);
        if (flags & 1)
            operator_delete(actor);
    }
}
/* 0232F910 */
static BOOL is_delete(Act_c *actor) {
    return TRUE;
}
} // namespace daObjBuoyrace

using namespace daObjBuoyrace;
Act_c::Act_c() {
    fopAc_ac_c_ct(this);
    __vtbl = 0x100268B0;
}
/* WWHD glue
str 100269CC "Kkiba_00"
str 100269D8 "Khasi_00"
data l_headerZeros 10469418
data l_registrationZ 101C8858
data l_minAngle 1046940C
data l_maxAngle 10469410
data l_initA 10469414
data l_initB 10469415
data l_registrationA 101C8864
data l_registrationB 101C8870
ext 0201B080 void buoyOutprod(const cXyz*, cXyz*, const cXyz*)
ext 0201B31C void buoyNormalize(cXyz*, cXyz*)
ext 0201AE48 void buoyScale(const cXyz*, cXyz*, f32)
ext 023123D8 void buoyQuatRotBaseY(Quaternion*, const cXyz*)
ext 028F4384 f32 buoySqrt(f32)
ext 028ED6F8 void buoyInitA(void*)
ext 028EAB2C void buoyInitB(void*)
ext 025D50BC void buoyActorDt(void*, s32)
*/
