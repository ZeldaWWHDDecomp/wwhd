/* Quake, light-volume and attention detection. */
#include "d/d_detect.h"
static const f32 zero = 0.0f;
static const f32 maxDist2XZ = 250000.0f;
static const f32 maxY = 500.0f;
static const f32 minY = -200.0f;
static const f32 one = 1.0f;
static const f32 coneRatioScale = 0.06666667014360428f;
static const f32 boxXMin = -50.0f;
static const f32 boxXMax = 50.0f;
static const f32 boxYMin = 0.0f;
static const f32 boxYMax = 100.0f;
static const f32 boxZMin = -50.0f;
static const f32 boxZMax = 50.0f;
static const f32 coneLower = 0.0f;
static const f32 coneUpper = 100.0f;
static const f32 coneRadius = 50.0f;
#define zeroPosition (*gabi::at<cXyz>(0x101FFBA8))
/* 02529C1C */
s32 dDetectPlace_c::chk_enable() {
    auto *self = this;
    return self->mTimer != 0;
}

/* 0252A154 */
void dDetectState_c::proc() {
    for (auto *timer : {&mPlace[0].mTimer, &mTimer}) {
        s32 value = *timer;
        if (value > 0)
            *timer = value - 1;
        else if (value < 0)
            *timer = 1;
    }
}

/* 02529CE8 */
s32 dDetectState_c::chk_quake(const cXyz *pos) {
    auto *self = this;
    DetectGame *game = dComIfG_getGameInfo();
    s16 timer = self->mTimer;
    DetectPlayer *player = game->mPlayer.get();
    s32 result = 0;
    if (timer > 0)
        return 1;
    if ((player->mResetFlg0 & DetectPlayer::daPyRFlg0_FRONT_ROLL_CRASH) != 0) {
        if (pos == 0 || chk_quake_area(pos) != 0)
            result = 1;
    }
    if (self->mPlace[0].mTimer > 0)
        result = 1;
    return result;
}

/* 02529D7C */
void dDetectState_c::set_quake(const cXyz *pos) {
    auto *self = this;
    if (pos == 0) {
        self->mTimer = -1;
        return;
    }
    if (self->mPlace[0].chk_enable() != 0)
        return;
    self->mPlace[0].mTimer = -1;
    self->mPlace[0].mPos.copy(*pos);
}

/* 02529C2C */
// HD: retain the unordered float comparisons and horizontal vector call.
u8 dDetectState_c::chk_quake_area(const cXyz *pos) {
    DetectGame *game = dComIfG_getGameInfo();
    DetectPlayer *player = game->mPlayer.get();
    f32 x = (f32)pos->x;
    f32 pz = (f32)player->current.pos.z;
    f32 px = (f32)player->current.pos.x;
    cXyz a;
    cXyz b;
    b.x = x;
    a.x = px;
    f32 z = (f32)pos->z;
    f32 zeroY = zero;
    b.z = z;
    b.y = zeroY;
    a.z = pz;
    a.y = zeroY;
    f32 dist = PSVECSquareDistance(&a, &b);
    f32 py = (f32)player->current.pos.y;
    f32 bound = maxDist2XZ;
    f32 y = (f32)pos->y;
    f32 diff = y - py;
    if (dist > bound)
        return 0;
    if (diff > maxY)
        return 0;
    if (diff < minY)
        return 0;
    return 1;
}

/* 02529DE8 */
void *search_tag_light(void *i_proc, void *i_pos) {
    auto *actor = (DetectLight *)i_proc;
    auto *pos = (const cXyz *)i_pos;
    if (fopAcM_IsActor(actor) == 0)
        return 0;
    if (actor == 0 || actor->mProcName != fpcNm_Tag_Light_e || actor->mEnabled == 0)
        return 0;
    cXyz local;
    PSMTXMultVec(&actor->mInvMtx, pos, &local);
    s32 shape = actor->mShape;
    if (shape == DetectLight::BOX) {
        f32 x = local.x;
        if (x < boxXMin || x > boxXMax)
            return 0;
        f32 y = local.y;
        if (y < boxYMin || y > boxYMax)
            return 0;
        f32 z = local.z;
        if (z < boxZMin || z > boxZMax)
            return 0;
        return actor;
    }
    if (shape != DetectLight::CONE)
        return 0;
    f32 y = local.y;
    f32 low = coneLower;
    if (y < low)
        return 0;
    f32 high = coneUpper;
    if (y > high)
        return 0;
    f32 x = local.x;
    f32 zeroY = zero;
    f32 z = local.z;
    cXyz radial;
    radial.y = zeroY;
    radial.x = x;
    radial.z = z;
    f32 dist2 = PSVECSquareMag(&radial);
    // HD: retain the double result and fused cone-radius arithmetic.
    double radius = sqrtDouble((double)dist2);
    y = local.y;
    f32 range = high - low;
    f32 dy = y - low;
    f32 unitRatio = one;
    f32 ratio = dy / range;
    s32 param = daObj_PrmAbstract(actor, 4u, 10u);
    f32 p = (f32)param;
    f32 scale = coneRatioScale;
    f32 inv = unitRatio - ratio;
    f32 pp = p * scale;
    f32 factor = gabi::fmadds(ratio, pp, inv);
    f32 bound = factor * coneRadius;
    if (radius > bound)
        return 0;
    return actor;
}

/* 0252A038 */
s32 dDetectState_c::chk_light(const cXyz *pos) {
    return fopAcIt_Judge(search_tag_light, (void *)pos) != NULL;
}

/* 0252A068 */
s32 dDetectState_c::chk_attention(cXyz *out) {
    DetectGame *game = dComIfG_getGameInfo();
    u32 target = game->mAttention.mLook2Target;
    fopAc_ac_c *actor = dAttHint_convPId(&game->mAttention, target);
    if (actor == 0)
        return 0;
    out->x = (f32)actor->eyePos.x;
    out->y = (f32)actor->eyePos.y;
    out->z = (f32)actor->eyePos.z;
    return 1;
}

// HD: compiler-generated construction, destruction and header initialization.
/* 02529BC4 */
static dDetectPlace_c *dDetectPlace_c__ct(dDetectPlace_c *self) {
    if (self == 0)
        self = (dDetectPlace_c *)operator_new(sizeof(dDetectPlace_c));
    if (self != 0) {
        self->mPos.x = (f32)zeroPosition.x;
        self->mPos.y = (f32)zeroPosition.y;
        f32 z = (f32)zeroPosition.z;
        self->mTimer = 0;
        self->mPos.z = z;
    }
    return self;
}

/* 0252A0E0 */
// HD: GHS allocation and embedded-array construction.
static dDetectState_c *dDetectState_c__ct(dDetectState_c *self) {
    if (self == 0)
        self = (dDetectState_c *)operator_new(sizeof(dDetectState_c));
    if (self != 0) {
        constructArray(self, 1u, sizeof(dDetectPlace_c), dDetectPlace_c__ct);
        self->mTimer = 0;
    }
    return self;
}

/* 0252A140 */
static void dDetectPlace_c__dt(dDetectPlace_c *self, u32 flags) {
    if (self != 0 && (flags & 1) != 0)
        operator_delete(self);
}

// HD: compiler-generated initialization of this translation unit's header statics.
struct HeaderStatics {
    be<f32> piMin, piMax;
    u8 byteObject, flagObject, padding[2];
    be<u32> words[4];
};
#define headerStatics (*gabi::at<HeaderStatics>(0x104756E8))
/* 0252A1B0 */
static void __sinit_d_detect_cpp() {
    for (auto &word : headerStatics.words)
        word = 0;
    registerGlobalObject(gabi::at<void>(0x101D603C));
    headerStatics.piMin = -3.1415927f;
    headerStatics.piMax = 3.1415927f;
    initializeHeaderByte(&headerStatics.byteObject);
    registerGlobalObject(gabi::at<void>(0x101D6048));
    initializeHeaderFlag(&headerStatics.flagObject);
    registerGlobalObject(gabi::at<void>(0x101D6054));
}

/* WWHD glue (tools/verify/autoglue.py)
data zero 1004C4EC
data maxDist2XZ 1004C4F0
data maxY 1004C4F4
data minY 1004C4F8
data one 1004C4FC
data coneRatioScale 1004C508
data boxXMin 1004BCF4
data boxXMax 1004BCF8
data boxYMin 1004BCFC
data boxYMax 1004BD00
data boxZMin 1004BD04
data boxZMax 1004BD08
data coneLower 1004BD0C
data coneUpper 1004BD10
data coneRadius 1004BD14
ext 0273AD10 void* operator_new(u32)
ext 0273AF40 void operator_delete(void*)
ext 025200D4 DetectGame* dComIfG_getGameInfo()
ext 025D4604 s32 fopAcM_IsActor(void*)
ext 028E8DE8 f32 PSVECSquareDistance(cXyz*,cXyz*)
ext 028E8DD0 f32 PSVECSquareMag(cXyz*)
ext 028E8F64 void PSMTXMultVec(DetectMatrix*,const cXyz*,cXyz*)
ext 028F4384 double sqrtDouble(double)
ext 024AE670 s32 daObj_PrmAbstract(void*,u32,u32)
ext 025D5218 void* fopAcIt_Judge(DetectJudgeFunc,void*)
ext 024EC0A0 fopAc_ac_c* dAttHint_convPId(DetectAttention*,u32)
ext 028EFFD0 void constructArray(void*,u32,u32,DetectCtorFunc)
ext 028F026C void registerGlobalObject(void*)
ext 028ED6F8 void initializeHeaderByte(void*)
ext 028EAB2C void initializeHeaderFlag(void*)
*/
