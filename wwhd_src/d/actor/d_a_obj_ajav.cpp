/** Stone barrier at Jabun's cave, in zeldaret/tww layout and source order.
 * WWHD functions are verified against the original.
 */
#include "d/actor/d_a_obj_ajav.h"
const gc_tu_data gc_tu = {.safeString = 0x100247C0,
                          .headerStatics = {0x10468F20, 0x101C7A64, 0x10468F2C}};

// HD: autoglue does not yet register external functions passed as callbacks.
static gabi::HostFn l_cylinderDestructorBinding((const void*)ajavCylinderDt, 0x02515A70);
static gabi::HostFn l_statusDestructorBinding((const void*)ajavStatusDt, 0x02515860);

static cXyz l_daObjAjav_co_offset[3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
static cXyz l_daObjAjav_offset[6] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0},
                                     {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
static cXyz l_daObjAjav_cyl_offset[6] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0},
                                         {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
static cXyz l_daObjAjav_rock_pos_table[6] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0},
                                             {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
static cXyz l_flawPos[6] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
static const f32 l_daObjAjav_hint_cyl_h_talbe[3] = {3300.0f, 2360.0f, 1050.0f};
static const f32 l_daObjAjav_cyl_r[6] = {629.0f, 294.8f, 491.0f, 216.0f, 563.0f, 85.3f};
static const f32 l_daObjAjav_cyl_h[6] = {773.0f, 458.5f, 1100.5f, 1401.9f, 1133.0f, 451.9f};
// Resource order: AJAVA through AJAVF; literals keep the glue byte check enabled.
static const s32 l_daObjAjav_idx_table[6] = {4,5,6,7,8,9};
static AjavTrigTable l_sincosTable;
static cXyz l_baseX = {1, 0, 0};
static cXyz l_baseZ = {0, 0, 1};
static csXyz l_zeroRotation = {0, 0, 0};
static u32 l_flawPosInitialized = 0;
static const char* l_daObjAjav_ev_name[3] = {"ajav_destroy0", "ajav_destroy1", "ajav_uzu"};
static const u8 l_daObjAjav_sph_data[64] = {0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0,  0,   0, 0,
                                            0, 0, 0, 32, 0, 0, 0, 15, 0, 0, 0, 0, 0,  0,   0, 0,
                                            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 4, 0,  0,   0, 0,
                                            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 67, 140, 0, 0};
static const u8 l_daObjAjav_cyl_data[68] = {
    0, 0, 0, 0, 0, 0, 0,   0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0,  32, 0,  0, 0,
    9, 0, 0, 0, 0, 0, 0,   0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0,  0,  6,  0, 0,
    0, 0, 0, 0, 0, 0, 194, 32, 0, 0, 0, 0, 0, 0, 68, 67, 0, 0, 69, 78, 64, 0};
static const u8 l_daObjAjav_hint_cyl_data[68] = {
    0, 0, 0, 0, 0, 0, 0,   0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0,  32, 0,  0, 0,
    9, 0, 0, 0, 0, 0, 0,   0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0,  0,  6,  0, 0,
    0, 0, 0, 0, 0, 0, 194, 32, 0, 0, 0, 0, 0, 0, 68, 67, 0, 0, 69, 78, 64, 0};

static BOOL daObjAjav_solidHeapCB(daObjAjav::Act_c*);
static void constructStatus(dCcD_Stts*);
static void constructCylinder(dCcD_Cyl*);
static void* daObjAjav_Stts_constructor(void*);
static void* daObjAjav_Cyl_constructor(void*);

// HD: the resource SafeString lives above the callee linkage scratch.
static void* objectResource(const char* archive, s32 index) {
    gabi::Local<SafeString> name;
    gabi::Local<u8[16]> resourceLinkage;
    name->mStringTop = gabi::guest_addr(archive);
    name->__vtbl = gc_tu.safeString;
    return ajavObjectResource(dComIfG_resControl(), name.get(), index);
}

static void trans(const cXyz* vector) {
    f32 x = vector->x, y = vector->y, z = vector->z;
    mDoMtx_stack_c::transM(x, y, z);
}

static void rotate(const csXyz* angle) {
    s16 x = angle->x, y = angle->y, z = angle->z;
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), x, y, z);
}

static void finishMatrix(daObjAjav::Part_c* p) {
    J3DModel_setBaseTRMtx(p->mpModel, mDoMtx_stack_c::get());
    p->mpModel->calc();
}

static GXColor* getColor(AjavMaterial* material) { return material->mTevBlock->getTevKColor(); }

static void constructStatus(dCcD_Stts* status) { dCcD_Stts_ct(status); }

static void constructCylinder(dCcD_Cyl* cylinder) {
    auto shape = reinterpret_cast<AjavCylinderShape*>(cylinder);
    ajavObjectInfoCtor(cylinder);
    shape->mShapeVtable = 0x100015A8;
    shape->mAabVtable = 0x100247D8;
    ajavCylinderGeometryCtor(&cylinder->mCyl);
    cylinder->__vtbl_hitinf = 0x1004B108;
    cylinder->mCyl.__vtbl = 0x1004B150;
    shape->mShapeVtable = 0x1004B160;
}

static daObjAjav::Part_c* partAt(daObjAjav::Act_c* actor, u32 index) {
    return &actor->mRockParts[index];
}

/* 02314108 */
void daObjAjav::Part_c::make_hamon(cXyz* i_hamonPos, f32 i_scale) {
    gabi::Local<cXyz> scale;
    gabi::Local<be<f32>> height;
    gabi::Local<u8[16]> callerLinkage5;
    f32 value = 2.0f + i_scale / 2000.0f;
    scale->x = value;
    scale->y = value;
    scale->z = value;
    i_hamonPos->y = (f32)i_hamonPos->y + 10.0f;
    if (ajavWaterHeight(i_hamonPos, (be<f32>*)height) == 1)
        i_hamonPos->y = (f32)*height + 0.1f;
    dComIfGp_particle_set(0x3F, i_hamonPos, nullptr, scale);
}

/* 02313AA8 */
void daObjAjav::Part_c::no_proc(daObjAjav::Act_c* i_actor) {}

/* 023135F0 */
void daObjAjav::Part_c::init_data(const cXyz* i_actorPos, const cXyz* i_rockOffset,
                                  dKy_tevstr_c* i_tevStr, const cXyz* i_rockPos) {
    (mCenterPos) = (*i_rockOffset);
    gabi::Local<cXyz> temporary;
    gabi::Local<u8[16]> callerLinkage1;
    ajavSubtractPosition(&cXyz::Zero, (cXyz*)temporary, &mCenterPos);
    (mCenterToOriginTrans) = (*(cXyz*)temporary);
    mAccumulatedRotation = l_zeroRotation;
    (mRockDisplacement) = (*&cXyz::Zero);
    u32 zeroX = reinterpret_cast<const AjavVectorWords*>(&cXyz::Zero)->x;
    reinterpret_cast<AjavVectorWords*>(&mRockDisplacementRate)->x = zeroX;
    u32 zeroY = reinterpret_cast<const AjavVectorWords*>(&cXyz::Zero)->y;
    reinterpret_cast<AjavVectorWords*>(&mRockDisplacementRate)->y = zeroY;
    u32 zeroZ = reinterpret_cast<const AjavVectorWords*>(&cXyz::Zero)->z;
    mpTevStr = i_tevStr;
    reinterpret_cast<AjavVectorWords*>(&mRockDisplacementRate)->z = zeroZ;
    ajavAddPosition(i_rockPos, (cXyz*)temporary, i_actorPos);
    (mRockParticlePos) = (*(cXyz*)temporary);
    setExeProc(daObjAjav::Part_c::PROC_NONE);
    setDrawProc(daObjAjav::Part_c::PROC_DRAW_NORMAL);
}

/* 02313714 */
BOOL daObjAjav::Part_c::set_mdl_area(const char* i_arcName, s32 i_index, u32 i_differedDlistFlag) {
    auto data = static_cast<J3DModelData*>(objectResource(i_arcName, i_index));
    if (!data) {
        JUT_ASSERT_fail("d_a_obj_ajav.cpp", 0x25C, "mdl_data != 0");
        return FALSE;
    }
    mpModel = mDoExt_J3DModel__create(data, 0x80000, i_differedDlistFlag);
    return TRUE;
}

/* 023137B0 */
void daObjAjav::Part_c::init_mtx(const cXyz* i_pos, const csXyz* i_angle, const cXyz* i_scale) {
    f32 sy = i_scale->y, sx = i_scale->x, sz = i_scale->z;
    mpModel->mBaseScale.x = sx;
    mpModel->mBaseScale.y = sy;
    mpModel->mBaseScale.z = sz;
    f32 x = i_pos->x, y = i_pos->y, z = i_pos->z;
    mDoMtx_stack_c::transS(x, y, z);
    s16 ax = i_angle->x, ay = i_angle->y, az = i_angle->z;
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), ax, ay, az);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    mpModel->calc();
}

/* 023139C0 */
void daObjAjav::Part_c::set_flaw_mtx(const cXyz* i_pos, const csXyz* i_angle) {
    f32 x = i_pos->x, y = i_pos->y, z = i_pos->z;
    mDoMtx_stack_c::transS(x, y, z);
    rotate(i_angle);
    trans(&mRockDisplacement);
    finishMatrix(this);
}

/* 023138A4 */
void daObjAjav::Part_c::set_fall_mtx(const cXyz* i_pos, const csXyz* i_angle) {
    f32 x = i_pos->x, y = i_pos->y, z = i_pos->z;
    mDoMtx_stack_c::transS(x, y, z);
    rotate(i_angle);
    trans(&mRockDisplacement);
    trans(&mCenterPos);
    rotate(&mAccumulatedRotation);
    trans(&mCenterToOriginTrans);
    finishMatrix(this);
}

/* 02313E70 */
void daObjAjav::Part_c::fall_init(const cXyz* i_initDispRate, const csXyz* i_initRotSpeed,
                                  s16 i_angleLimit, u16 i_timerTrigger) {
    (mRockDisplacementRate) = (*i_initDispRate);
    mRotationSpeed = *i_initRotSpeed;
    setExeProc(daObjAjav::Part_c::PROC_FALL_0);
    mbHasSplashed = 0;
    mTimer = 0;
    mAngleLimit = i_angleLimit;
    mTimerTrigger = i_timerTrigger;
}

/* 02313ED0 */
static BOOL daObjAjav_check_angle(be<s16>* i_anglePtr, s32 i_angleLimit) {
    s32 value = *i_anglePtr, absolute = value < 0 ? -value : value;
    if (absolute < i_angleLimit)
        return FALSE;
    s16 excess = (s16)((u32)absolute - (u32)i_angleLimit);
    u32 reflected = (u32)i_angleLimit - (u32)(s32)excess;
    *i_anglePtr = (s16)(value > 0 ? reflected : 0u - reflected);
    return TRUE;
}

/* 02313F24 */
void daObjAjav::Part_c::fall_0(daObjAjav::Act_c* i_actor) {
    ajavAddRotation(&mAccumulatedRotation, &mRotationSpeed);
    BOOL reflected = daObjAjav_check_angle(&mAccumulatedRotation.x, mAngleLimit);
    s16 limit = mAngleLimit;
    if (reflected)
        mRotationSpeed.x = (s16) - (s16)mRotationSpeed.x;
    if (daObjAjav_check_angle(&mAccumulatedRotation.y, limit))
        mRotationSpeed.y = (s16) - (s16)mRotationSpeed.y;
    PSVECAdd(&mRockDisplacement, &mRockDisplacementRate, &mRockDisplacement);
    f32 random = cM_rnd();
    f32 factor = gabi::fmadds(0.2f, random, 0.8f);
    u16 timer = (u16)((u16)mTimer + 1), trigger = mTimerTrigger;
    f32 speedZ = (f32)mRockDisplacementRate.z * factor;
    mTimer = timer;
    mRockDisplacementRate.z = speedZ;
    if (timer == trigger) {
        gabi::Local<cXyz> reduced;
        gabi::Local<u8[16]> callerLinkage9;
        ajavScalePosition(&mRockDisplacementRate, (cXyz*)reduced, 0.4f);
        f32 x = reduced->x, z = reduced->z;
        setExeProc(daObjAjav::Part_c::PROC_FALL_1);
        mTimer = 0;
        mRockDisplacementRate.x = x;
        mRockDisplacementRate.y = 5.0f;
        mRockDisplacementRate.z = z;
        // HD: csXyz returns its third component in the high half of r4.
        u32 packed = ajavScaleRotation(&mRotationSpeed, 0.3f);
        u32 last = gabi::cpu->r[4];
        mRotationSpeed.x = (s16)(packed >> 16);
        mRotationSpeed.y = (s16)packed;
        mRotationSpeed.z = (s16)(last >> 16);
        s32 sign = gabi::ftoi(-2.0f * cM_rnd());
        s32 magnitude = gabi::ftoi(511.0f * cM_rnd());
        mRotationSpeed.z = (s16)((u32)sign * (u32)magnitude);
    }
    gabi::Local<cXyz> position;
    gabi::Local<csXyz> angle;
    gabi::Local<u8[16]> callerLinkage10;
    position->x = i_actor->current.pos.x;
    position->y = i_actor->current.pos.y;
    position->z = i_actor->current.pos.z;
    *angle = i_actor->shape_angle;
    set_fall_mtx(position, angle);
}

/* 023141CC */
void daObjAjav::Part_c::fall_1(daObjAjav::Act_c* i_actor) {
    PSVECAdd(&mRockDisplacement, &mRockDisplacementRate, &mRockDisplacement);
    f32 elapsed = (f32)(u16)mTimer;
    mRockDisplacement.y = (f32)mRockDisplacement.y - (elapsed + elapsed);
    ajavAddRotation(&mAccumulatedRotation, &mRotationSpeed);
    f32 y = mRockDisplacement.y, originY = mCenterToOriginTrans.y;
    u16 timer = mTimer;
    u8 splashed = mbHasSplashed;
    mTimer = (u16)(timer + 1);
    if (!splashed && !(y > originY - 100.0f)) {
        gabi::Local<cXyz> displaced, sum, position, scale;
        gabi::Local<u8[16]> callerLinkage11;
        ajavAddPosition(&i_actor->current.pos, (cXyz*)displaced, &mRockDisplacement);
        ajavAddPosition((cXyz*)displaced, (cXyz*)sum, &mCenterPos);
        f32 size = 4.0f + (f32)mCenterPos.y / 2000.0f, x = sum->x, z = sum->z;
        position->x = x;
        position->y = 0.0f;
        position->z = z;
        scale->x = size;
        scale->y = 4.0f;
        scale->z = size;
        dComIfGp_particle_set(0x3C, position, nullptr, scale);
        f32 centerY = mCenterPos.y;
        mbHasSplashed = 1;
        position->x = x;
        position->y = 0.0f;
        position->z = z;
        make_hamon(position, centerY);
        mSePos.x = x;
        mSePos.y = 0.0f;
        mSePos.z = z;
        s32 reverb = dComIfGp_getReverb(i_actor->current.roomNo);
        mDoAud_seStart(0x6A29, &mSePos, 0, reverb);
        originY = mCenterToOriginTrans.y;
        y = mRockDisplacement.y;
    }
    if (!(y > originY - 1000.0f)) {
        setExeProc(daObjAjav::Part_c::PROC_NONE);
        setDrawProc(daObjAjav::Part_c::PROC_NONE);
    }
    gabi::Local<cXyz> position;
    gabi::Local<csXyz> angle;
    gabi::Local<u8[16]> callerLinkage12;
    position->x = i_actor->current.pos.x;
    position->y = i_actor->current.pos.y;
    position->z = i_actor->current.pos.z;
    *angle = i_actor->shape_angle;
    set_fall_mtx(position, angle);
}

/* 02314460 */
void daObjAjav::Part_c::flaw(daObjAjav::Act_c* i_actor) {
    PSVECAdd(&mRockDisplacement, &mFlawPos, &mRockDisplacement);
    gabi::Local<cXyz> position;
    gabi::Local<csXyz> angle;
    gabi::Local<u8[16]> callerLinkage2;
    position->x = i_actor->current.pos.x;
    position->y = i_actor->current.pos.y;
    position->z = i_actor->current.pos.z;
    *angle = i_actor->shape_angle;
    set_flaw_mtx(position, angle);
    setExeProc(daObjAjav::Part_c::PROC_NONE);
}

/* 02313AAC */
void daObjAjav::Part_c::draw_normal(daObjAjav::Act_c* i_actor) {
    auto environment = dKy_getEnvlight();
    setLightTevColorType(environment, mpModel, mpTevStr);
    mDoExt_modelUpdateDL(mpModel);
}

/* 02313AF0 */
void daObjAjav::Part_c::draw_flashing(daObjAjav::Act_c* i_actor) {
    if (!(dKy_tevstr_c*)mpTevStr)
        return;
    auto environment = dKy_getEnvlight();
    setLightTevColorType(environment, mpModel, mpTevStr);
    auto data = reinterpret_cast<AjavModelData*>((J3DModelData*)mpModel->mModelData);
    auto joint = (AjavJoint*)data->mJointNodePointer;
    auto material = (AjavMaterial*)joint->mMesh;
    u8 red = 0, green = 0, blue = 0;
    if (material) {
        f32 ratio = (f32)(u16)mTimer / (f32)(u16)mTimerTrigger;
        u16 theta = (u16)((u16)gabi::ftoi(20479.0f * ratio) + 0x3000);
        f32 strength = std::fabs((f32)l_sincosTable.entries[theta >> 3].sine);
        red = getColor(material)->r;
        green = getColor(material)->g;
        blue = getColor(material)->b;
        getColor(material)->r = (u8)gabi::ftoi(85.0f * strength);
        getColor(material)->g = (u8)gabi::ftoi(11.0f * strength);
        getColor(material)->b = (u8)gabi::ftoi(9.0f * strength);
        GXColor* color = getColor(material);
        AjavTevBlock* block = material->mTevBlock;
        block->setTevKColor(color);
        // HD: normalize and convert the color into the material uniform buffer.
        gabi::Local<ColorFloat> normalized, converted;
        gabi::Local<u8[16]> callerLinkage13;
        ajavNormalizeColor((ColorFloat*)normalized, color);
        ajavConvertColor((ColorFloat*)converted, (ColorFloat*)normalized, 1.0f);
        material->mUniform.mFlags |= 0x100;
        auto output = ajavUniformBuffer(&material->mUniform, 8);
        f32 alpha = (f32)(u8)color->a / 255.0f;
        f32 x = converted->value[0], y = converted->value[1], z = converted->value[2];
        output->r = x;
        output->g = y;
        output->b = z;
        output->a = alpha;
    }
    mDoExt_modelUpdateDL(mpModel);
    if (material) {
        getColor(material)->r = red;
        getColor(material)->g = green;
        getColor(material)->b = blue;
    }
}

/* 02313E0C */
void daObjAjav::Part_c::draw_flashing_normal(daObjAjav::Act_c* i_actor) {
    draw_flashing(i_actor);
    u16 timer = (u16)((u16)mTimer + 1), trigger = mTimerTrigger;
    mTimer = timer;
    if (timer == trigger)
        setDrawProc(daObjAjav::Part_c::PROC_DRAW_NORMAL);
}

/* 02314500 */
void daObjAjav::Part_c::make_fall_rock(BOOL i_increasedRate) {
    auto emitter = dComIfGp_particle_set(0x8427, &mRockParticlePos);
    if (emitter && (dKy_tevstr_c*)mpTevStr) {
        if (i_increasedRate)
            emitter->setRate(20.0f);
        auto environment = dKy_getEnvlight();
        settingTevStruct(environment, 1, &mRockParticlePos, mpTevStr);
        auto output = reinterpret_cast<AjavEmitter*>(emitter);
        u8 red = mpTevStr->mColorK0.r, green = mpTevStr->mColorK0.g, blue = mpTevStr->mColorK0.b;
        output->mGlobalPrmColor.r = red;
        output->mGlobalPrmColor.g = green;
        output->mGlobalPrmColor.b = blue;
    }
}

/* 023146B4 */
static BOOL daObjAjav_solidHeapCB(daObjAjav::Act_c* i_actor) { return i_actor->create_heap(); }

/* 023145D8 */
BOOL daObjAjav::Act_c::create_heap() {
    BOOL result = mRockParts[0].set_mdl_area("Ajav", dRes_INDEX_AJAV_BDL_AJAVA_e, 0x11000002);
    if (!result)
        return FALSE;
    for (u32 i = 1; i < PART_COUNT; ++i) {
        s32 index = l_daObjAjav_idx_table[i];
        result = mRockParts[i].set_mdl_area("Ajav", index, 0x15021202);
        if (!result)
            return FALSE;
    }
    void* resource = objectResource("Ajav", dRes_INDEX_AJAV_DZB_AJAV_e);
    dBgW* background = ajavNewBackground(resource, 1, &mMtx);
    mpBgW = background;
    return background ? result : FALSE;
}

/* 023148B4 */
s32 daObjAjav::Act_c::_create() {
    u32 address = gabi::ea(this), condition = actor_condition;
    if (!(condition & fopAcCnd_INIT_e)) {
        if (address) {
            ajavActorCtor(this);
            __vtbl = 0x10024800;
            constructStatus(&mSphStts);
            ajavSphereCtor(&mSph);
            constructStatus(&mCylStts);
            constructCylinder(&mCyl);
            ajavConstructArray(mHintCylStts, 2, 0x3C, daObjAjav_Stts_constructor);
            ajavConstructArray(mHintCyls, 2, 0x130, daObjAjav_Cyl_constructor);
            condition = actor_condition;
        }
        actor_condition = condition | fopAcCnd_INIT_e;
    }
    u32 switchNo = (u32)mParameters & 0xFF;
    mbResLoaded = 0;
    mSwNo = switchNo;
    if (!dComIfGs_isEventBit(ENDLESS_NIGHT))
        return cPhs_ERROR_e;
    s8 room = home.roomNo;
    if (dComIfGs_isSwitch(mSwNo, room))
        return cPhs_ERROR_e;
    s32 phase = dComIfG_resLoad(&mPhs, "Ajav");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)daObjAjav_solidHeapCB), 0))
        return cPhs_ERROR_e;
    for (u32 i = 0; i < PART_COUNT; ++i) {
        gabi::Local<cXyz> position, offset;
        gabi::Local<u8[16]> callerLinkage14;
        position->x = current.pos.x;
        position->y = current.pos.y;
        position->z = current.pos.z;
        offset->x = l_daObjAjav_offset[i].x;
        offset->y = l_daObjAjav_offset[i].y;
        offset->z = l_daObjAjav_offset[i].z;
        mRockParts[i].init_data(position, offset, &tevStr, &l_daObjAjav_rock_pos_table[i]);
    }
    init_mtx();
    MTXCopy(J3DModel_getBaseTRMtx(mRockParts[5].mpModel), &mMtx);
    set_tex();
    mSphStts.Init(0, 255, this);
    mSph.SetStts(&mSphStts);
    mSph.Set((dCcD_SrcSph*)l_daObjAjav_sph_data);
    mCylStts.Init(0, 255, this);
    mCyl.SetStts(&mCylStts);
    mCyl.Set((dCcD_SrcCyl*)l_daObjAjav_cyl_data);
    mCyl.SetC(&current.pos);
    for (u32 i = 0; i < 2; ++i) {
        auto status = &mHintCylStts[i];
        auto cylinder = &mHintCyls[i];
        status->Init(0, 255, this);
        cylinder->SetStts(status);
        cylinder->Set((dCcD_SrcCyl*)l_daObjAjav_hint_cyl_data);
    }
    set_co_offset();
    auto space = dComIfG_Bgsp();
    space->Regist(mpBgW, this);
    mHintCylHits = 0;
    mActionIdx = ACTION_WAIT;
    mbResLoaded = 1;
    return phase;
}

/* 023146B8 */
void daObjAjav::Act_c::init_mtx() {
    for (u32 i = 0; i < PART_COUNT; ++i) {
        gabi::Local<cXyz> position, scale;
        gabi::Local<csXyz> angle;
        gabi::Local<u8[16]> callerLinkage3;
        position->x = current.pos.x;
        position->y = current.pos.y;
        position->z = current.pos.z;
        scale->x = this->scale.x;
        scale->y = this->scale.y;
        scale->z = this->scale.z;
        *angle = shape_angle;
        mRockParts[i].init_mtx(position, angle, scale);
    }
}

/* 02314760 */
void daObjAjav::Act_c::set_tex() {
    auto data = reinterpret_cast<AjavModelData*>((J3DModelData*)mRockParts[0].mpModel->mModelData);
    u32 texture = data->mTexture, names = data->mTextureName;
    for (u32 i = 1; i < PART_COUNT; ++i) {
        data = reinterpret_cast<AjavModelData*>((J3DModelData*)mRockParts[i].mpModel->mModelData);
        data->mTexture = texture;
        data->mTextureName = names;
    }
}

/* 02314794 */
void daObjAjav::Act_c::set_co_offset() {
    u32 status = M_status;
    if (status >= STATUS_MAX - 1) {
        JUT_ASSERT_fail("d_a_obj_ajav.cpp", 0x4A0,
                        "(M_status >= 0) && (M_status < STATUS_MAX - 1)");
        status = M_status;
    }
    (mSphCoOffset) = (*&l_daObjAjav_co_offset[status]);
    gabi::Local<cXyz> center;
    gabi::Local<u8[16]> callerLinkage4;
    ajavAddPosition(&current.pos, (cXyz*)center, &mSphCoOffset);
    mSph.SetC(center);
    status = M_status;
    mCyl.SetH(l_daObjAjav_hint_cyl_h_talbe[status]);
    status = M_status;
    for (u32 i = 0; i < 2; ++i) {
        u32 index = status * 2 + i;
        ajavAddPosition(&current.pos, (cXyz*)center, &l_daObjAjav_cyl_offset[index]);
        cM3dGCyl* cylinder = &mHintCyls[i].mCyl;
        cylinder->SetC(center);
        cylinder->SetR(l_daObjAjav_cyl_r[index]);
        cylinder->SetH(l_daObjAjav_cyl_h[index]);
    }
}

/* 02314BDC */
BOOL daObjAjav::Act_c::_delete() {
    if ((JKRSolidHeap*)heap != 0) {
        u32 background = gabi::ea(mpBgW);
        if (background && mpBgW->ChkUsed()) {
            auto space = dComIfG_Bgsp();
            space->Release(mpBgW);
            mpBgW = nullptr;
        }
    }
    dComIfG_resDelete(&mPhs, "Ajav");
    for (u32 i = 0; i < PART_COUNT; ++i)
        mDoAud_seDeleteObject(&mRockParts[i].mSePos);
    return TRUE;
}

/* 02314D7C */
static void daObjAjav_limit_angle(be<s16>* i_anglePtr, s32 i_angleLimit) {
    s32 value = *i_anglePtr, absolute = value < 0 ? -value : value;
    if (absolute > i_angleLimit)
        *i_anglePtr = (s16)(value > 0 ? (u32)i_angleLimit : 0u - (u32)i_angleLimit);
}

/* 0231513C */
BOOL daObjAjav::Act_c::check_all_wait() {
    for (u32 i = 0; i < PART_COUNT; ++i) {
        daObjAjav::Part_c::daObjAjavPartCallback* method = &mRockParts[i].mExeProc;
        if ((s16)method->i != -1)
            return FALSE;
        if ((s16)method->d != 0 || (u32)method->f != daObjAjav::Part_c::PROC_NONE)
            return FALSE;
    }
    return TRUE;
}

/* 02315194 */
BOOL daObjAjav::Act_c::check_end() {
    if ((u8)M_status != STATUS_MAX - 1)
        return FALSE;
    return check_all_wait();
}

/* 02314C78 */
void daObjAjav::Act_c::to_broken() {
    const char* name = l_daObjAjav_ev_name[(u8)M_status];
    auto manager = dComIfGp_getPEvtManager();
    s32 event = ajavGetEvent(manager, name, 255);
    u16 command = (u16)eventInfo.mCommand;
    mEventIdx = (s16)event;
    mActionIdx = ACTION_EVENT;
    if (command != dEvtCmd_INDEMO_e) {
        fopAcM_orderOtherEventId(this, (s16)mEventIdx, 255, 0xFFFF, 0, 1);
        eventInfo.onCondition(dEvtCnd_UNK2_e);
    }
}

/* 02314D14 */
BOOL daObjAjav::Act_c::damage_part() {
    dCcD_Sph* sphere = &mSph;
    if ((u8)M_status < STATUS_MAX - 1 && sphere->ChkTgHit()) {
        to_broken();
        sphere->ClrTgHit();
        return TRUE;
    }
    return FALSE;
}

/* 02314DAC */
void daObjAjav::Act_c::make_shot_rock() {
    gabi::Local<cXyz> position;
    gabi::Local<u8[16]> callerLinkage7;
    position->x = mSph.mSph.mCenter.x;
    position->y = mSph.mSph.mCenter.y;
    position->z = mSph.mSph.mCenter.z;
    auto emitter = dComIfGp_particle_set(0x8426, position);
    if (emitter) {
        auto environment = dKy_getEnvlight();
        settingTevStruct(environment, 1, position, &tevStr);
        u8 red = tevStr.mColorK0.r, green = tevStr.mColorK0.g, blue = tevStr.mColorK0.b;
        auto output = reinterpret_cast<AjavEmitter*>(emitter);
        output->mGlobalPrmColor.r = red;
        output->mGlobalPrmColor.g = green;
        output->mGlobalPrmColor.b = blue;
    }
}

/* 02314E6C */
void daObjAjav::Act_c::make_hamon2(cXyz* i_hamonPos, f32 i_scale) {
    gabi::Local<cXyz> scale;
    gabi::Local<be<f32>> height;
    gabi::Local<u8[16]> callerLinkage6;
    f32 value = 1.0f + i_scale / 3000.0f;
    scale->x = value;
    scale->y = value;
    scale->z = value;
    i_hamonPos->y = (f32)i_hamonPos->y + 10.0f;
    if (ajavWaterHeight(i_hamonPos, (be<f32>*)height) == 1)
        i_hamonPos->y = (f32)*height + 0.1f;
    auto emitter = dComIfGp_particle_set(0x8454, i_hamonPos, nullptr, scale);
    if (emitter) {
        auto environment = dKy_getEnvlight();
        settingTevStruct(environment, 2, &current.pos, &tevStr);
        auto output = reinterpret_cast<AjavEmitter*>(emitter);
        u8 red = (u8)(s16)tevStr.mColorC0.r, blue = (u8)(s16)tevStr.mColorC0.b;
        output->mGlobalPrmColor.r = red;
        u8 green = (u8)(s16)tevStr.mColorC0.g;
        output->mGlobalPrmColor.b = blue;
        output->mGlobalPrmColor.g = green;
    }
}

/* 02314F78 */
void daObjAjav::Act_c::set_hamon(f32 i_scale) {
    gabi::Local<cXyz> forward, side, current, temporary, position;
    gabi::Local<u8[16]> callerLinkage8;
    ajavYrotS(mDoMtx_stack_c::get(), (s16)this->current.angle.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), &l_baseZ, (cXyz*)forward);
    PSVECScale((cXyz*)forward, (cXyz*)forward, 1300.0f);
    ajavYrotS(mDoMtx_stack_c::get(), (s16)this->current.angle.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), &l_baseX, (cXyz*)side);
    for (u32 i = 0; i < 2; ++i) {
        (*(cXyz*)current) = (this->current.pos);
        (*(cXyz*)temporary) = (*(cXyz*)side);
        f32 factor = gabi::fmadds(-2.0f, (f32)i, 1.0f) * 400.0f;
        PSVECScale((cXyz*)temporary, (cXyz*)temporary, factor);
        PSVECAdd((cXyz*)current, (cXyz*)forward, (cXyz*)current);
        PSVECAdd((cXyz*)current, (cXyz*)temporary, (cXyz*)current);
        position->x = current->x;
        position->y = current->y;
        position->z = current->z;
        make_hamon2(position, i_scale);
    }
}

/* 023151CC */
BOOL daObjAjav::Act_c::_execute() {
    // HD: write the initialization flag before filling the static positions.
    if (!(u32)l_flawPosInitialized) {
        l_flawPosInitialized = 1;
        const f32 offsets[18] = {0, 0, 0, 0, 0, 0, 0, 40, 0, 30, 30, 0, -20, 0, 0, 20, 0, 0};
        for (u32 i = 0; i < 6; ++i) {
            l_flawPos[i].x = offsets[i * 3];
            l_flawPos[i].y = offsets[i * 3 + 1];
            l_flawPos[i].z = offsets[i * 3 + 2];
        }
    }
    for (u32 i = 0; i < PART_COUNT; ++i) {
        daObjAjav::Part_c* part = partAt(this, i);
        gabi::Local<cXyz> position, sum;
        gabi::Local<u8[16]> callerLinkage15;
        position->x = current.pos.x;
        position->y = current.pos.y;
        position->z = current.pos.z;
        ajavAddPosition((cXyz*)position, (cXyz*)sum, &part->mCenterPos);
        (part->mSePos) = (*(cXyz*)sum);
        PSVECAdd(&part->mSePos, &part->mRockDisplacement, &part->mSePos);
    }
    switch ((u8)mActionIdx) {
    case ACTION_WAIT:
        if (!damage_part()) {
            bool counted = false, broken = false;
            for (u32 i = 0; i < 2; ++i) {
                mHintCylStts[i].Move();
                dCcD_Cyl* cylinder = &mHintCyls[i];
                if (cylinder->ChkTgHit()) {
                    if (!counted) {
                        u8 count = (u8)((u8)mHintCylHits + 1);
                        mHintCylHits = count;
                        if (count >= 3) {
                            to_broken();
                            mHintCylHits = 0;
                            broken = true;
                        }
                        counted = true;
                    }
                    cylinder->ClrTgHit();
                }
            }
            if (!broken) {
                mCylStts.Move();
                dCcD_Cyl* cylinder = &mCyl;
                if (cylinder->ChkTgHit()) {
                    u32 first = (u8)M_status * 2;
                    for (u32 i = 0; i < 2; ++i) {
                        daObjAjav::Part_c* part = partAt(this, first + i);
                        part->make_fall_rock(TRUE);
                        part->mTimer = 0;
                        part->mTimerTrigger = 30;
                        part->setDrawProc(daObjAjav::Part_c::PROC_DRAW_FLASHING_NORMAL);
                    }
                    cylinder->ClrTgHit();
                }
            }
        }
        break;
    case ACTION_EVENT:
        if (eventInfo.checkCommandDemoAccrpt()) {
            u32 first = (u8)M_status * 2;
            for (u32 i = 0; i < 2; ++i) {
                daObjAjav::Part_c* part = partAt(this, first + i);
                gabi::Local<cXyz> offset, center, difference, rate;
                gabi::Local<u8[16]> callerLinkage16;
                gabi::Local<csXyz> rotation, byValueRotation;
                gabi::Local<u8[16]> callerLinkage17;
                offset->x = mSphCoOffset.x;
                offset->y = mSphCoOffset.y;
                offset->z = mSphCoOffset.z;
                center->x = part->mCenterPos.x;
                center->y = part->mCenterPos.y;
                center->z = part->mCenterPos.z;
                ajavSubtractPosition((cXyz*)offset, (cXyz*)difference, (cXyz*)center);
                rotation->x = (s16)gabi::ftoi(difference->y);
                rotation->y = (s16)gabi::ftoi(difference->x);
                rotation->z = l_zeroRotation.z;
                daObjAjav_limit_angle(&rotation->x, 511);
                daObjAjav_limit_angle(&rotation->y, 511);
                *byValueRotation = *rotation;
                rate->x = ((first + i) & 1) ? 12.0f : -12.0f;
                rate->y = -10.0f;
                rate->z = 55.0f;
                u16 trigger = (u16)((u16)gabi::ftoi(9.0f * cM_rnd()) + 7);
                part->fall_init(rate, byValueRotation, 511, trigger);
                s32 reverb = dComIfGp_getReverb(current.roomNo);
                mDoAud_seStart(0x6A28, &part->mSePos, 0, reverb);
                part->setDrawProc(daObjAjav::Part_c::PROC_DRAW_NORMAL);
                part->make_fall_rock(FALSE);
            }
            make_shot_rock();
            u32 firstNow = (u8)M_status * 2;
            set_hamon(partAt(this, firstNow)->mRockDisplacement.y);
            u8 status = (u8)((u8)M_status + 1);
            M_status = status;
            if (status < STATUS_MAX - 1) {
                for (u32 i = 0; i < 2; ++i) {
                    u32 index = status * 2 + i;
                    daObjAjav::Part_c* part = partAt(this, index);
                    (part->mFlawPos) = (*&l_flawPos[index]);
                    part->setExeProc(daObjAjav::Part_c::PROC_FLAW);
                }
                set_co_offset();
                mActionIdx = ACTION_EVENT_END;
            } else {
                mCutsceneEndDelay = 60;
                mActionIdx = ACTION_LAST_FALL;
            }
        } else {
            fopAcM_orderOtherEventId(this, (s16)mEventIdx, 255, 0xFFFF, 0, 1);
            eventInfo.onCondition(dEvtCnd_UNK2_e);
        }
        break;
    case ACTION_EVENT_END: {
        s16 event = mEventIdx;
        auto manager = dComIfGp_getPEvtManager();
        if (ajavEventEndCheck(manager, event)) {
            dComIfGp_getEvent()->reset();
            if ((u8)M_status < STATUS_MAX - 1)
                mActionIdx = ACTION_WAIT;
            else
                mActionIdx = ACTION_DONE;
        }
        break;
    }
    case ACTION_LAST_FALL:
        if (check_end()) {
            u16 delay = mCutsceneEndDelay;
            if (delay)
                mCutsceneEndDelay = (u16)(delay - 1);
            else {
                auto manager = dComIfGp_getPEvtManager();
                s32 staff = ajavGetStaff(manager, "Ajav", 0, 0);
                manager = dComIfGp_getPEvtManager();
                ajavCutEnd(manager, staff);
                if ((JKRSolidHeap*)heap) {
                    u32 background = gabi::ea(mpBgW);
                    if (background && mpBgW->ChkUsed()) {
                        auto space = dComIfG_Bgsp();
                        space->Release(mpBgW);
                        mpBgW = nullptr;
                    }
                }
                s8 room = home.roomNo;
                dComIfGs_onSwitch(mSwNo, room);
                ajavSystemSeStart(0x806);
                ajavSubBgmStop();
                u8 status = (u8)((u8)M_status + 1);
                mActionIdx = ACTION_EVENT_END;
                M_status = status;
            }
        }
        break;
    case ACTION_DONE:
        break;
    default:
        mActionIdx = ACTION_WAIT;
        break;
    }
    if ((u8)M_status < STATUS_MAX - 1) {
        auto space = dComIfG_Ccsp();
        ajavCollisionSet(space, &mSph);
        space = dComIfG_Ccsp();
        ajavCollisionSet(space, &mCyl);
        for (u32 i = 0; i < 2; ++i) {
            space = dComIfG_Ccsp();
            ajavCollisionSet(space, &mHintCyls[i]);
        }
    }
    for (u32 i = 0; i < PART_COUNT; ++i) {
        daObjAjav::Part_c* part = partAt(this, i);
        (part->*part->mExeProc)(this);
    }
    u32 background = gabi::ea(mpBgW);
    if (background && mpBgW->ChkUsed())
        mpBgW->Move();
    return TRUE;
}

/* 02315994 */
BOOL daObjAjav::Act_c::_draw() {
    auto environment = dKy_getEnvlight();
    settingTevStruct(environment, 1, &current.pos, &tevStr);
    for (u32 i = 0; i < PART_COUNT; ++i)
        ((&mRockParts[i])->*mRockParts[i].mDrawProc)(this);
    return TRUE;
}

/* 02315A4C */
static s32 daObjAjav_Mthd_Create(daObjAjav::Act_c* i_actor) { return i_actor->_create(); }

/* 02315A50 */
static BOOL daObjAjav_Mthd_Delete(daObjAjav::Act_c* i_actor) { return i_actor->_delete(); }

/* 02315A54 */
static BOOL daObjAjav_Mthd_Execute(daObjAjav::Act_c* i_actor) { return i_actor->_execute(); }

/* 02315A58 */
static BOOL daObjAjav_Mthd_Draw(daObjAjav::Act_c* i_actor) { return i_actor->_draw(); }

/* 02315F0C */
static BOOL daObjAjav_Mthd_IsDelete(daObjAjav::Act_c* i_actor) { return TRUE; }

/* ---------------- HD: compiler-generated and template code of this translation unit
 * ---------------- */

/* 02315A5C */
static void daObjAjav_sinit() {
    __sinit_header_statics();
    const f32 sphere[9] = {0, 2550, 200, 250, 1270, 200, -290, 740, 200};
    const f32 parts[18] = {0,   3000, 0, 500,  2400, 0, -300, 1900, 0,
                           500, 1400, 0, -550, 650,  0, 250,  400,  0};
    const f32 cylinders[18] = {33,  2541, 220, 465,     2083, 220, -137.7f, 1297, 220,
                               530, 511,  220, -124.5f, 118,  220, 530,     118,  220};
    const f32 particles[18] = {-170, 3087, 222, 563,  2384, 222, -371, 1880, 222,
                               615,  1391, 222, -494, 748,  222, 249,  535,  222};
    for (u32 i = 0; i < 3; ++i) {
        l_daObjAjav_co_offset[i].x = sphere[i * 3];
        l_daObjAjav_co_offset[i].y = sphere[i * 3 + 1];
        l_daObjAjav_co_offset[i].z = sphere[i * 3 + 2];
    }
    for (u32 i = 0; i < 6; ++i) {
        l_daObjAjav_offset[i].x = parts[i * 3];
        l_daObjAjav_offset[i].y = parts[i * 3 + 1];
        l_daObjAjav_offset[i].z = parts[i * 3 + 2];
    }
    for (u32 i = 0; i < 6; ++i) {
        l_daObjAjav_cyl_offset[i].x = cylinders[i * 3];
        l_daObjAjav_cyl_offset[i].y = cylinders[i * 3 + 1];
        l_daObjAjav_cyl_offset[i].z = cylinders[i * 3 + 2];
    }
    for (u32 i = 0; i < 6; ++i) {
        l_daObjAjav_rock_pos_table[i].x = particles[i * 3];
        l_daObjAjav_rock_pos_table[i].y = particles[i * 3 + 1];
        l_daObjAjav_rock_pos_table[i].z = particles[i * 3 + 2];
    }
}

/* 02315D3C */
static void daObjAjav_Part_destructor(void* p, s32 flags) {
    if (p && (flags & 1))
        operator_delete(p);
}

/* 02315D50 */
static void* daObjAjav_Stts_constructor(void* p) {
    if (!p)
        p = operator_new(0x3C);
    if (p) {
        constructStatus((dCcD_Stts*)p);
    }
    return p;
}

/* 02315DB8 */
static void* daObjAjav_Cyl_constructor(void* p) {
    if (!p)
        p = operator_new(0x130);
    if (p) {
        constructCylinder((dCcD_Cyl*)p);
    }
    return p;
}

/* 02315E44 */
static void daObjAjav_emptyDestructor(void* p) {}

/* 02315E48 */
static void daObjAjav_Act_destructor(daObjAjav::Act_c* i_actor, s32 flags) {
    if (!i_actor)
        return;
    ajavDestroyArray(i_actor->mHintCyls, 2, 0x130, ajavCylinderDt, 0, 0);
    ajavDestroyArray(i_actor->mHintCylStts, 2, 0x3C, ajavStatusDt, 0, 0);
    ajavCylinderDt(&i_actor->mCyl, 2);
    ajavStatusDt(&i_actor->mCylStts, 2);
    ajavSphereDt(&i_actor->mSph, 2);
    ajavStatusDt(&i_actor->mSphStts, 2);
    ajavActorDt(i_actor, 0);
    if (flags & 1)
        operator_delete(i_actor);
}

/* WWHD glue
data l_sincosTable 104A44F8 nocheck # Shared runtime-initialized sine/cosine table
data l_daObjAjav_co_offset 10468F3C nocheck # HD: BSS populated by the original initializer
data l_daObjAjav_offset 10468F60 nocheck # HD: BSS populated by the original initializer
data l_daObjAjav_cyl_offset 10468FA8 nocheck # HD: BSS populated by the original initializer
data l_daObjAjav_rock_pos_table 10468FF0 nocheck # HD: BSS populated by the original initializer
data l_flawPos 10469038 nocheck # HD: BSS populated by the original initializer
data l_daObjAjav_hint_cyl_h_talbe 101C79D0
data l_daObjAjav_cyl_r 101C79A0
data l_daObjAjav_cyl_h 101C79B8
data l_daObjAjav_idx_table 100247E8
data l_baseX 101FFBB4 nocheck # Common-header BSS initialized by sinit_header_statics_z
data l_baseZ 101FFBCC nocheck # Common-header BSS initialized by sinit_header_statics_z
data l_zeroRotation 101FFB14 nocheck # Common-header BSS initialized by sinit_header_statics_z
data l_flawPosInitialized 10469080 nocheck # HD: BSS populated by the original initializer
data l_daObjAjav_ev_name 10024810
data l_daObjAjav_sph_data 101C7960
data l_daObjAjav_cyl_data 101C79DC
data l_daObjAjav_hint_cyl_data 101C7A20
str 10024858 in 02313714
str 10024848
str 100248BC in 02314794
str 100248D0
str 100249D8
str 10024934 in 023151CC
ext 0200E240 void ajavCollisionSet(cCcS*,void*)
ext 02018590 void ajavCylinderGeometryCtor(void*)
ext 0201A554 void ajavAddRotation(csXyz*,const csXyz*)
ext 0201A588 u32 ajavScaleRotation(csXyz*,f32)
ext 0201AD78 void ajavAddPosition(const cXyz*,cXyz*,const cXyz*)
ext 0201ADE0 void ajavSubtractPosition(const cXyz*,cXyz*,const cXyz*)
ext 0201AE48 void ajavScalePosition(const cXyz*,cXyz*,f32)
ext 0231353C void ajavNormalizeColor(ColorFloat*,GXColor*)
ext 024F2478 dBgW* ajavNewBackground(void*,u32,Mtx34*)
ext 02515860 void ajavStatusDt(void*,s32)
ext 02515A70 void ajavCylinderDt(void*,s32)
ext 02515AE8 void ajavSphereDt(void*,s32)
ext 02515FB8 void ajavObjectInfoCtor(void*)
ext 025166F0 void ajavSphereCtor(void*)
ext 02542D88 s32 ajavGetStaff(dEvent_manager_c*,const char*,s32,s32)
ext 02543280 void ajavCutEnd(dEvent_manager_c*,s32)
ext 02543F10 s32 ajavGetEvent(dEvent_manager_c*,const char*,s32)
ext 025440C8 BOOL ajavEventEndCheck(dEvent_manager_c*,s16)
ext 025D4ED0 void ajavActorCtor(fopAc_ac_c*)
ext 025D50BC void ajavActorDt(fopAc_ac_c*,s32)
ext 025D9F70 s32 ajavWaterHeight(cXyz*,be<f32>*)
ext 025E1928 void ajavSubBgmStop()
ext 025E1988 void ajavSystemSeStart(u32)
ext 025F1884 void ajavYrotS(Mtx34*,s16)
ext 026066C4 void* ajavObjectResource(dRes_control_c*,SafeString*,s32)
ext 0274D458 void ajavConvertColor(ColorFloat*,ColorFloat*,f32)
ext 027F9F0C AjavColorUniform* ajavUniformBuffer(void*,u32)
ext 028EFFD0 void ajavConstructArray(void*,u32,u32,AjavConstructor)
ext 028F0164 void ajavDestroyArray(void*,u32,u32,AjavDestructor,u32,u32)
*/
