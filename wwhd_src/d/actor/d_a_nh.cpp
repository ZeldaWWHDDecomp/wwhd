/** Forest Firefly actor. Verified HD behavior; private derived source. */
#include "d/actor/d_a_nh.h"
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/tu.h"
const gc_tu_data gc_tu = {.safeString = 0x100158F8,
                          .safeStringAssure = 0x021E1E74,
                          .objAcch = {0x10015A00, 0x10015A20, 0x10015A10},
                          .objAcchDt = {0x10015A10, 0x10015A20},
                          .gndChk = {0x100159C0, 0x100159D0, 0x100159F0, 0x100159E0},
                          .gndChkDt = {0x10015950, 0x10015970, 0x10015930}};
namespace {
constexpr u32 NH_HIO_VTABLE = 0x10015A30;
constexpr u32 NH_ACTOR_VTABLE = 0x10015B18;
constexpr u32 NH_SHAPE_ATTR_VTABLE = 0x100015A8;
constexpr u32 NH_AAB_VTABLE = 0x10015910;
constexpr u32 NH_CYL_HITINFO_VTABLE = 0x1004B108;
constexpr u32 NH_CYL_GEOMETRY_VTABLE = 0x1004B150;
constexpr u32 NH_CYL_SHAPE_VTABLE = 0x1004B160;
constexpr u32 NH_POLYINFO_VTABLE = 0x10015920;
// HD: autoglue does not register external bindings as callbacks; retain the named guest target.
constexpr u32 NH_HEAP_CALLBACK = 0x021E0938;
constexpr u32 dItemNo_FIREFLY_BOTTLE_e = 0x58;
enum AlwaysResource { dRes_INDEX_ALWAYS_BDL_NH_e = 0x34, dRes_INDEX_ALWAYS_BRK_TNH_e = 0x4E };
static daNh_HIO_c l_HIO;
static NhHioDefaults l_hioDefaults;
static NhHeaderStatics l_headerObject;
static NhPiPair l_headerPi;
static NhPiPair l_headerPiDefaults;
static NhGuestByte l_headerByte0;
static NhGuestByte l_headerByte1;
static NhDtorRecord l_headerDtor0;
static NhDtorRecord l_headerDtor1;
static NhDtorRecord l_headerDtor2;
static NhAction l_waitAction;
static NhAction l_escapeAction;
static NhAction l_returnAction;
static Mtx34 l_matrixStack;
static NhSourceCylinder l_cyl_src;
} // namespace

static void nhAcchTables(daNh_c *actor, bool constructing) {
    if (constructing)
        actor->mAcch.cBgS_Chk::__vtbl = gc_tu.objAcch.v10;
    actor->mAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20;
    actor->mAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14;
    if (constructing)
        actor->mAcch.mbObjThrough = 1;
}

static u32 daNh_playBrkAnm(daNh_c *actor);
static BOOL daNh_IsDelete(daNh_c *actor);
static BOOL daNh_Delete(daNh_c *actor);
static daNh_HIO_c *daNh_HIO_constructHD(daNh_HIO_c *hio);
static void __sinit_d_a_nh_cpp();
static void daNh_safeStringDtorHD(SafeString *key, u32 flags);
static void daNh_safeStringVirtualEmptyHD(SafeString *key, void *context);
static s32 cLib_getRndValue(s32 base, s32 width);
static BOOL checkCreateHeap(daNh_c *actor);
static BOOL daNh_Draw(daNh_c *actor);
static daNh_c *daNh_constructHD(daNh_c *actor);
static void daNh_destroyHD(daNh_c *actor, u32 flags);
static s32 daNh_createBodyHD(daNh_c *actor);
static s32 daNh_Create(daNh_c *actor);
static BOOL daNh_Execute(daNh_c *actor);

static BOOL nhChangeAction(daNh_c *actor, const NhAction &descriptor) {
    gabi::Local<NhAction> next;
    next->thisAdjustment = descriptor.thisAdjustment.get();
    next->virtualIndex = descriptor.virtualIndex.get();
    next->target = descriptor.target.get();
    return actor->setAction(next.get(), nullptr);
}

/* 021E105C */
void daNh_c::setBaseMtx() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    J3DModel *model = mpModel.get();
    const f32 modelScale = l_HIO.prm.mModelScale;
    scale.y = modelScale;
    scale.z = modelScale;
    scale.x = modelScale;
    cXyz *modelScaleVector = nhModelScale(model);
    modelScaleVector->x = modelScale;
    modelScaleVector->y = modelScale;
    modelScaleVector->z = modelScale;
    Mtx34 *matrix = &l_matrixStack;
    nhTranslateMatrix(matrix, f32(current.pos.x), f32(current.pos.y), f32(current.pos.z));
    nhRotateMatrixY(matrix, s16(shape_angle.y));
    auto *source = reinterpret_cast<be<f32> *>(&l_matrixStack);
    f32 snapshot[12];
    for (int i = 0; i < 12; ++i)
        snapshot[i] = source[i];
    auto *modelMatrix = reinterpret_cast<be<f32> *>(nhModelMatrix(model));
    for (int i = 0; i < 12; ++i)
        modelMatrix[i] = snapshot[i];
    gabi::Local<cXyz> glowOffset, glowPosition;
    glowOffset->x = 0.0f;
    glowOffset->y = l_HIO.prm.mGlowOffsetY;
    glowOffset->z = 0.0f;
    PSMTXMultVec(matrix, glowOffset.get(), glowPosition.get());
    nhTranslateMatrix(matrix, f32(glowPosition->x), f32(glowPosition->y), f32(glowPosition->z));
    const f32 glowScale = l_HIO.prm.mGlowScale;
    nhScaleMatrix(glowScale, glowScale, glowScale);
    PSMTXCopy(matrix, reinterpret_cast<Mtx34 *>(mGlowMtx));
}

/* 021E0868 */
BOOL daNh_c::createHeap() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    auto *modelData = static_cast<J3DModelData *>(
        dComIfG_getObjectRes("Always", dRes_INDEX_ALWAYS_BDL_NH_e, gc_tu.safeString));
    if (!modelData)
        JUT_ASSERT_fail("d_a_nh.cpp", 0x167, "modelData != NULL");
    mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    return mpModel.get() && initBrkAnm(0);
}

/* 021E0938 */
static BOOL checkCreateHeap(daNh_c *actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return actor->createHeap();
}

// HD: free creation entry preserves the null-receiver constructor guard and guest member reads.
/* 021E15DC */
static s32 daNh_createBodyHD(daNh_c *actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    auto *condition = nhCreationCondition(actor);
    if (!(u32(*condition) & fopAcCnd_INIT_e)) {
        if (actor)
            daNh_constructHD(actor);
        *condition |= fopAcCnd_INIT_e;
    }
    if (!nhEntrySolidHeap(actor, NH_HEAP_CALLBACK, 0x4000))
        return cPhs_ERROR_e;
    J3DModel *model = nhActorModel(actor);
    *nhCullMatrix(actor) = model ? gabi::ea(&model->mBaseTransformMtx) : 0;
    if (s8(l_HIO.mNo) < 0) {
        // HD: ASCII source label for Shift-JIS title; renamed glue retains the original guest
        // bytes.
        const s32 child = nhCreateHioChild("Forest Firefly", &l_HIO);
        l_HIO.mpActor = actor;
        l_HIO.mNo = s8(child);
    }
    return actor->init() ? cPhs_COMPLEATE_e : cPhs_ERROR_e;
}

// HD: initialize a point light alongside eye and attention positions.
/* 021E14B4 */
BOOL daNh_c::init() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mType = u8(mParameters);
    speed.y = 1.0f;
    gravity = l_HIO.prm.mGravity;
    mPlayerDist = 0.0f;
    mGlowAlpha = u8(s16(l_HIO.prm.mDefaultGlowAlpha));
    mAlpha = 255;
    mGroundY = 0.0f;
    mBottleTimer = s16(l_HIO.prm.mBottleLifetime);
    BGCheck();
    mStts.Init(255, 255, this);
    mCyl.Set(reinterpret_cast<dCcD_SrcCyl *>(&l_cyl_src));
    mCyl.SetStts(&mStts);
    setBaseMtx();
    mLightPos.copy(current.pos);
    eyePos.copy(current.pos);
    attention_info.position.copy(current.pos);
    mLightRed = 300;
    mLightGreen = 300;
    mLightBlue = 300;
    mLightParameter = 250.0f;
    mLightBlend = 0.0f;
    nhSetPointLight(&mLightPos);
    return TRUE;
}

/* 021E0FA4 */
void daNh_c::action(void *context) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (s16(mCurrActionFunc.virtualIndex) == 0) {
        speedF = 0.0f;
        gabi::Local<NhAction> wait;
        wait->thisAdjustment = l_waitAction.thisAdjustment.get();
        wait->virtualIndex = l_waitAction.virtualIndex.get();
        wait->target = l_waitAction.target.get();
        setAction(wait.get(), nullptr);
    }
    nhInvoke(this, mCurrActionFunc, context);
}

/* 021E0D1C */
BOOL daNh_c::setAction(const NhAction *next, void *context) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    const s16 nextIndex = next->virtualIndex;
    const s16 oldIndex = mCurrActionFunc.virtualIndex;
    if (oldIndex == nextIndex && oldIndex == 0)
        return TRUE;
    const s16 adjustment = next->thisAdjustment;
    const u32 target = next->target;
    if (oldIndex == nextIndex && s16(mCurrActionFunc.thisAdjustment) == adjustment &&
        u32(mCurrActionFunc.target) == target)
        return TRUE;
    if (oldIndex != 0) {
        mActionStatus = ACTION_ENDING;
        nhInvoke(this, mCurrActionFunc, context);
    }
    mCurrActionFunc.target = target;
    mCurrActionFunc.virtualIndex = nextIndex;
    mCurrActionFunc.thisAdjustment = adjustment;
    unk69A = 0;
    unk698 = 0;
    mActionStatus = ACTION_STARTING;
    mEscapeTimer = 0;
    unk696 = 0;
    unk6A0 = 0.0f;
    nhInvoke(this, mCurrActionFunc, context);
    return TRUE;
}

/* 021E0BC0 */
BOOL daNh_c::checkBinCatch() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (eventInfo.mCommand == dEvtCmd_INCATCH_e) {
        fopAcM_delete(this);
        return TRUE;
    }
    const s16 angle = l_HIO.prm.field_0x3c;
    const f32 distance = l_HIO.prm.field_0x08;
    const f32 lower = l_HIO.prm.field_0x10;
    const f32 upper = l_HIO.prm.field_0x0c;
    NhCatch *attention = nhCatchController(dComIfGp_getAttention());
    nhCatchRequest(attention, this, dItemNo_FIREFLY_BOTTLE_e, distance, upper, lower, angle, 1);
    eventInfo.onCondition(dEvtCnd_CANCATCH_e);
    return FALSE;
}

/* 021E0E5C */
BOOL daNh_c::searchPlayer() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mType == TYPE_BOTTLE) {
        nhChangeAction(this, l_escapeAction);
        return TRUE;
    }
    gabi::Local<cXyz> delta, horizontal;
    fopAc_ac_c *player = dComIfGp_getPlayer0();
    cXyz_mi(&player->old.pos, delta.get(), &player->current.pos);
    const f32 distance = fopAcM_searchPlayerDistance(this);
    const f32 change = f32(mPlayerDist) - distance;
    horizontal->x = delta->x;
    horizontal->z = delta->z;
    mPlayerDist = distance;
    horizontal->y = 0.0f;
    PSVECSquareMag(horizontal.get());
    // GHS forwards the previous magnitude result in f1 without a new explicit argument.
    const f32 movement = nhSqrtCurrentFloat();
    if (movement > 0.001f && distance < 600.0f && change > f32(l_HIO.prm.mMinFrightenSpeed)) {
        nhChangeAction(this, l_escapeAction);
        return TRUE;
    }
    return FALSE;
}

/* 021E17A0 */
BOOL daNh_c::moveProc(f32 targetSpeed, f32 speedStep, u32 targetAngle) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    // GHS forwards the incoming angle register without narrowing it.
    nhChaseAngleRegister(&current.angle.y, targetAngle, 0x400);
    shape_angle.y = current.angle.y;
    return cLib_chaseF(&speedF, targetSpeed, speedStep) && targetSpeed == 0.0f;
}

/* 021E1868 */
f32 daNh_c::getHomeDistance() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<cXyz> delta, horizontal;
    // The reference vector subtraction has destination in its second argument.
    cXyz_mi(&home.pos, delta.get(), &current.pos);
    horizontal->x = delta->x;
    horizontal->y = 0.0f;
    horizontal->z = delta->z;
    // GHS leaves the magnitude result in f1; the following sqrt forwards that live register.
    PSVECSquareMag(horizontal.get());
    return nhSqrtCurrentFloat();
}

/* 021E0B18 */
BOOL daNh_c::checkTimer() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mType != TYPE_BOTTLE)
        return FALSE;
    if (nhCalcTimerInt(&mBottleTimer) != 0)
        return FALSE;
    s32 alpha = static_cast<s32>(static_cast<u32>(mAlpha) - 4u);
    if (alpha < 0)
        alpha = 0;
    mAlpha = alpha;
    if (alpha == 0) {
        fopAcM_delete(this);
        return TRUE;
    }
    return FALSE;
}

/* 021E09E4 */
void daNh_c::BGCheck() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<dBgS_GndChk> ground;
    dBgS_GndChk_ct(ground.get(), gc_tu.gndChk, true);
    dBgS_GndChk_SetPos(ground.get(), &current.pos);
    // This actor checks all background groups.
    ground->mGrp = 15;
    const f32 height = nhGroundCross(dComIfG_Bgsp(), ground.get());
    if (height != -1000000000.0f) {
        mGroundY = height;
        auto *polygon = dBgS_GndChk_PolyInfo(ground.get());
        const u32 room = nhGetRoomId(dComIfG_Bgsp(), polygon);
        tevStr.mRoomNo = s8(room);
        current.roomNo = s8(room);
        const u32 color = nhGetPolyColor(dComIfG_Bgsp(), polygon);
        tevStr.mEnvrIdxOverride = u8(color);
    }
    ground->cBgS_PolyInfo::__vtbl = gc_tu.gndChkDt[0];
    ground->dBgS_PolyPassChk::__vtbl = gc_tu.gndChkDt[1];
    ground->dBgS_GrpPassChk::__vtbl = gc_tu.gndChkDt[2];
    nhDestroyGroundCheck(ground.get(), 0);
}

/* 021E0CCC */
void daNh_c::airMove() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    const f32 idealY = f32(mGroundY) + f32(l_HIO.prm.mHeightAboveGround);
    if (f32(current.pos.y) < idealY - 10.0f)
        maxFallSpeed = l_HIO.prm.mAscentSpeed;
    else if (f32(current.pos.y) > idealY + 10.0f)
        maxFallSpeed = l_HIO.prm.mDescentSpeed;
}

/* 021E18BC */
BOOL daNh_c::waitAction(void *context) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (s8(mActionStatus) == ACTION_STARTING) {
        mActionStatus = ACTION_ONGOING;
        mPlayerDist = fopAcM_searchPlayerDistance(this);
    } else if (s8(mActionStatus) != ACTION_ENDING) {
        nhAddCalc(&speedF, 0.0f, 0.1f, 10.0f, 1.0f);
        if (getHomeDistance() > 50.0f)
            nhChangeAction(this, l_returnAction);
    }
    return TRUE;
}

/* 021E1988 */
BOOL daNh_c::checkEscapeEnd() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<cXyz> delta, horizontal;
    cXyz_mi(&home.pos, delta.get(), &current.pos);
    if (mType == TYPE_BOTTLE)
        return FALSE;
    if (nhCalcTimerShort(&mEscapeTimer) == 0) {
        nhChangeAction(this, l_waitAction);
        return TRUE;
    }
    horizontal->x = delta->x;
    horizontal->y = 0.0f;
    horizontal->z = delta->z;
    const f32 distance2 = PSVECSquareMag(horizontal.get());
    const f32 radius = l_HIO.prm.mMaxHomeDist;
    if (distance2 > f32(radius * radius)) {
        nhChangeAction(this, l_returnAction);
        return TRUE;
    }
    return FALSE;
}

/* 021E1A90 */
BOOL daNh_c::escapeAction(void *context) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (s8(mActionStatus) == ACTION_STARTING) {
        mActionStatus = ACTION_ONGOING;
        mWobbleTimer = 0;
        mEscapeTimer = 150;
        mWobbleDir = 0;
    } else if (s8(mActionStatus) != ACTION_ENDING && !checkEscapeEnd()) {
        s16 target = s16(u16(fopAcM_searchPlayerAngleY(this)) + 0x8000u);
        if (nhCalcTimerByte(&mWobbleTimer) == 0) {
            mWobbleDir ^= 1;
            mWobbleTimer = u8(cLib_getRndValue(15, 20));
        }
        target = s16(s32(target) + (mWobbleDir ? -0x2000 : 0x2000));
        moveProc(5.0f, 0.5f, u32(s32(target)));
    }
    return TRUE;
}

/* 021E1B88 */
BOOL daNh_c::returnAction(void *context) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (s8(mActionStatus) == ACTION_STARTING) {
        mActionStatus = ACTION_ONGOING;
        mWobbleTimer = 0;
        mEscapeTimer = 150;
        mWobbleDir = 0;
    } else if (s8(mActionStatus) != ACTION_ENDING) {
        if (getHomeDistance() < 50.0f) {
            nhChangeAction(this, l_waitAction);
        } else {
            s16 target = cLib_targetAngleY(&current.pos, &home.pos);
            gabi::Local<cXyz> delta, horizontal;
            cXyz_mi(&home.pos, delta.get(), &current.pos);
            horizontal->x = delta->x;
            horizontal->y = 0.0f;
            horizontal->z = delta->z;
            const f32 distance2 = PSVECSquareMag(horizontal.get());
            const f32 radius = l_HIO.prm.mMaxHomeDist;
            if (distance2 < f32(radius * radius)) {
                const s16 difference = s16(s32(target) - s32(fopAcM_searchPlayerAngleY(this)));
                if (std::abs(s32(difference)) < 0x1000)
                    target = s16(s32(target) + (difference < 0 ? -0x4000 : 0x4000));
            }
            if (nhCalcTimerByte(&mWobbleTimer) == 0) {
                mWobbleDir ^= 1;
                mWobbleTimer = u8(cLib_getRndValue(15, 20));
            }
            target = s16(s32(target) + (mWobbleDir ? -0x2000 : 0x2000));
            moveProc(5.0f, 0.5f, u32(s32(target)));
        }
    }
    return TRUE;
}

// HD: update the point-light position during movement.
/* 021E11A4 */
BOOL daNh_c::execute() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    daNh_playBrkAnm(this);
    const f32 fallSpeed = maxFallSpeed;
    const f32 verticalSpeed = speed.y;
    const u16 angle = u16(current.angle.y);
    mGlowAlpha = u8(s16(l_HIO.prm.mDefaultGlowAlpha));
    if (fallSpeed < verticalSpeed) {
        const f32 next = verticalSpeed - f32(gravity);
        speed.y = next;
        if (next < f32(maxFallSpeed))
            speed.y = maxFallSpeed;
    } else if (fallSpeed > verticalSpeed) {
        const f32 next = verticalSpeed + f32(gravity);
        speed.y = next;
        if (next > f32(maxFallSpeed))
            speed.y = maxFallSpeed;
    }
    const f32 forwardSpeed = speedF;
    speed.x = forwardSpeed * cM_ssin(s16(angle));
    speed.z = forwardSpeed * cM_scos(s16(angle));
    nhMoveActor(this, &mStts);
    mLightPos.copy(current.pos);
    BGCheck();
    gabi::Local<cXyz> center;
    center->x = current.pos.x;
    center->y = f32(current.pos.y) - 10.0f;
    center->z = current.pos.z;
    mCyl.SetC(center.get());
    nhSetCollision(dComIfG_Ccsp(), &mCyl);
    checkTimer();
    if (!checkBinCatch()) {
        airMove();
        searchPlayer();
        action(nullptr);
    }
    setBaseMtx();
    eyePos.copy(current.pos);
    attention_info.position.copy(current.pos);
    return TRUE;
}

/* 021E07A4 */
BOOL daNh_c::initBrkAnm(u32 modify) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    J3DModelData *modelData = nhModelData(mpModel);
    void *animation = dComIfG_getObjectRes("Always", dRes_INDEX_ALWAYS_BRK_TNH_e, gc_tu.safeString);
    if (!animation)
        JUT_ASSERT_fail("d_a_nh.cpp", 0x37F, "a_brk != NULL");
    return nhInitBrk(&mBrkAnm, modelData, animation, 1, 2, 1.0f, 0, -1, modify, 0) != 0;
}

// HD: retain the BRK play result register through this free wrapper (GC returns void).
/* 021E09DC */
static u32 daNh_playBrkAnm(daNh_c *actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return nhPlayBrk(&actor->mBrkAnm);
}

// HD: light blending replaces GC material-color glow extraction and alpha-model drawing.
/* 021E093C */
BOOL daNh_c::draw() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dScnKy_env_light_c *lighting = nhGetEnvironmentLight();
    settingTevStruct(lighting, 0, &current.pos, &tevStr);
    lighting = nhGetEnvironmentLight();
    setLightTevColorType(lighting, mpModel.get(), &tevStr);
    J3DModelData *data = nhModelData(mpModel);
    const f32 frame = mBrkAnm.mFrame.get();
    nhEntryBrk(&mBrkAnm, data, frame);
    mDoExt_modelUpdateDL(mpModel.get(), 0);
    *nhTevRegAnimator(data) = 0;
    cLib_chaseF(&mLightBlend, 100.0f, 10.0f);
    return TRUE;
}

/* 021E09D8 */
static BOOL daNh_Draw(daNh_c *actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return actor->draw();
}

/* 021E1338 */
static BOOL daNh_Execute(daNh_c *actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return actor->execute();
}

/* 021E133C */
static BOOL daNh_IsDelete(daNh_c *actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return TRUE;
}

// HD: delete cuts the actor point light directly.
/* 021E1344 */
static BOOL daNh_Delete(daNh_c *actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    nhCutPointLight(&actor->mLightPos);
    return TRUE;
}

/* 021E16D0 */
static s32 daNh_Create(daNh_c *actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return daNh_createBodyHD(actor);
}

// HD: compiler-generated and template code of this translation unit.
/* 021E1D48 */
static daNh_HIO_c *daNh_HIO_constructHD(daNh_HIO_c *hio) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!hio)
        hio = static_cast<daNh_HIO_c *>(nhAllocate(0x48));
    if (!hio)
        return nullptr;
    hio->disposerVtable = NH_HIO_VTABLE;
    hio->mNo = -1;
    // Fifteen initialized words include the packed angle, alpha and lifetime.
    auto *parameters = reinterpret_cast<be<u32> *>(&hio->prm);
    auto *defaults = l_hioDefaults.words;
    for (int i = 0; i < 15; ++i)
        parameters[i] = defaults[i];
    return hio;
}

/* 021E1DC0 */
static void __sinit_d_a_nh_cpp() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    l_headerObject.words[2] = 0;
    l_headerObject.words[0] = 0;
    l_headerObject.words[3] = 0;
    l_headerObject.words[1] = 0;
    nhRegisterGlobal(&l_headerDtor0);
    l_headerPi.lo = l_headerPiDefaults.lo;
    l_headerPi.hi = l_headerPiDefaults.hi;
    nhInitHeaderByte(&l_headerByte0);
    nhRegisterGlobal(&l_headerDtor1);
    nhInitSecondHeaderByte(&l_headerByte1);
    nhRegisterGlobal(&l_headerDtor2);
    daNh_HIO_constructHD(&l_HIO);
}

/* 021E1E60 */
static void daNh_safeStringDtorHD(SafeString *key, u32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (key && (flags & 1))
        nhFree(key);
}

/* 021E1E74 */
static void daNh_safeStringVirtualEmptyHD(SafeString *key, void *context) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

/* 021E1E78 */
static s32 cLib_getRndValue(s32 base, s32 width) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    const f32 start = f32(base);
    const f32 range = f32(width);
    const f32 random = cM_rndF(range);
    return gabi::ftoi(f32(start + random));
}

/* 021E136C */
static daNh_c *daNh_constructHD(daNh_c *actor) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!actor)
        actor = static_cast<daNh_c *>(nhAllocate(0x844));
    if (!actor)
        return nullptr;
    fopAc_ac_c_ct(actor);
    actor->__vtbl = NH_ACTOR_VTABLE;
    nhConstructAcch(&actor->mAcch);
    nhAcchTables(actor, true);
    nhConstructAcchCir(&actor->mAcchCir);
    nhConstructStts(&actor->mStts);
    nhConstructGStts(&actor->mStts.__vtbl_gstts);
    actor->mStts.__vtbl = 0x1004AE88;
    actor->mStts.__vtbl_gstts = 0x1004AEC0;
    nhConstructGObjInf(&actor->mCyl);
    auto *shape = reinterpret_cast<NhShapeAttr *>(actor->mCyl.mShapeAttr);
    shape->shapeVtable = NH_SHAPE_ATTR_VTABLE;
    shape->aabVtable = NH_AAB_VTABLE;
    nhConstructCyl(&actor->mCyl.mCyl);
    actor->mCyl.__vtbl_hitinf = NH_CYL_HITINFO_VTABLE;
    actor->mCyl.mCyl.__vtbl = NH_CYL_GEOMETRY_VTABLE;
    shape->shapeVtable = NH_CYL_SHAPE_VTABLE;
    nhConstructBrk(&actor->mBrkAnm);
    actor->mPolyInfo.mPolyIndex = 0xFFFF;
    actor->mPolyInfo.mActorId = 0xFFFFFFFF;
    actor->mPolyInfo.mBgIndex = 0x100;
    actor->mPolyInfo.mpBgW = 0;
    actor->mPolyInfo.__vtbl = NH_POLYINFO_VTABLE;
    actor->mLightIntensity = 1.0f;
    return actor;
}

/* 021E16D4 */
static void daNh_destroyHD(daNh_c *actor, u32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!actor)
        return;
    actor->__vtbl = NH_ACTOR_VTABLE;
    const s8 child = l_HIO.mNo;
    if (child >= 0) {
        mDoHIO_deleteChild(child);
        l_HIO.mNo = -1;
    }
    nhDestroyCylinder(&actor->mCyl, 2);
    nhDestroyStts(&actor->mStts, 2);
    nhDestroyAcchCircle(&actor->mAcchCir.m_cir, 2);
    nhAcchTables(actor, false);
    nhDestroyAcch(&actor->mAcch, 0);
    nhDestroyActorBase(actor, 0);
    if (flags & 1)
        nhFree(actor);
}

// clang-format off
/* WWHD glue:
data l_HIO 10465908
data l_hioDefaults 10015AD0
data l_headerObject 104658F8
data l_headerPi 104658EC
data l_headerPiDefaults 10015B10
data l_headerByte0 104658F4
data l_headerByte1 104658F5
data l_headerDtor0 101BB0EC
data l_headerDtor1 101BB0F8
data l_headerDtor2 101BB104
data l_waitAction 100158D8
data l_escapeAction 100158E0
data l_returnAction 100158E8
data l_matrixStack 1048D0CC
data l_cyl_src 101BB0A8
str 10015A44 "Always" in 021E07A4
str 10015A4C "d_a_nh.cpp" in 021E07A4
str 10015A58 "a_brk != NULL" in 021E07A4
str 10015A68 "Always" in 021E0868
str 10015A70 "d_a_nh.cpp" in 021E0868
str 10015A7C "modelData != NULL" in 021E0868
str 10015AB4 "Forest Firefly" renamed
ext 0211D2F8 s32 nhCalcTimerInt(be<s32>*)
ext 0200F8D0 BOOL nhChaseAngleRegister(be<s16>*, u32, u32)
ext 028F4384 f32 nhSqrtCurrentFloat()
ext 025E742C u32 nhPlayBrk(NhBrkAnm*)
ext 0255A374 void nhCutPointLight(cXyz*)
ext 0273AD10 void* nhAllocate(u32)
ext 0273AF40 void nhFree(void*)
ext 028ED6F8 void nhInitHeaderByte(NhGuestByte*)
ext 028EAB2C void nhInitSecondHeaderByte(NhGuestByte*)
ext 028F026C void nhRegisterGlobal(NhDtorRecord*)
ext 025E8154 BOOL nhInitBrk(NhBrkAnm*,J3DModelData*,void*,u32,u32,f32,u32,s32,u32,u32)
ext 025E83FC void nhEntryBrk(NhBrkAnm*,J3DModelData*,f32)
ext 024F0474 void nhConstructAcch(dBgS_ObjAcch*)
ext 024EFE94 void nhConstructAcchCir(dBgS_AcchCir*)
ext 0200BD2C void nhConstructStts(dCcD_Stts*)
ext 02515DA0 void nhConstructGStts(be<u32>*)
ext 02515FB8 void nhConstructGObjInf(dCcD_Cyl*)
ext 02018590 void nhConstructCyl(cM3dGCyl*)
ext 025E80D0 void nhConstructBrk(NhBrkAnm*)
ext 02515A70 void nhDestroyCylinder(dCcD_Cyl*,u32)
ext 02515860 void nhDestroyStts(dCcD_Stts*,u32)
ext 02018034 void nhDestroyAcchCircle(void*,u32)
ext 024EFD9C void nhDestroyAcch(dBgS_ObjAcch*,u32)
ext 025D50BC void nhDestroyActorBase(daNh_c*,u32)
ext 0200ECD4 f32 nhAddCalc(be<f32>*,f32,f32,f32,f32)
ext 02055B64 u32 nhCalcTimerShort(be<s16>*)
ext 0207A9A0 u32 nhCalcTimerByte(be<u8>*)
ext 024EBB70 void nhCatchRequest(NhCatch*,daNh_c*,u32,f32,f32,f32,s16,u32)
ext 024EF130 u32 nhGetRoomId(dBgS*,cBgS_PolyInfo*)
ext 024EEEB8 u32 nhGetPolyColor(dBgS*,cBgS_PolyInfo*)
ext 02008DAC void nhDestroyGroundCheck(dBgS_GndChk*,u32)
ext 028E93CC void nhTranslateMatrix(Mtx34*,f32,f32,f32)
ext 025F1C28 void nhRotateMatrixY(Mtx34*,s16)
ext 025F2518 void nhScaleMatrix(f32,f32,f32)
ext 025564B4 void nhSetPointLight(cXyz*)
ext 025D63E8 BOOL nhEntrySolidHeap(daNh_c*,u32,u32)
ext 025F0A10 s32 nhCreateHioChild(const char*,daNh_HIO_c*)
ext 025D6800 void nhMoveActor(daNh_c*,dCcD_Stts*)
ext 0200E240 void nhSetCollision(cCcS*,dCcD_Cyl*)
ext 02555D0C dScnKy_env_light_c* nhGetEnvironmentLight()
ext 02008974 f32 nhGroundCross(dBgS*,dBgS_GndChk*)
*/
