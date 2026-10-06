/** Forest Firefly, HD layout and actor behavior. */
#include "bindings.h"
#include "d/actor/d_a_nh.h"
#include "d/d_bg_s.h"

static daNh_HIO_c& nhHio() { return *gabi::at<daNh_HIO_c>(0x10465908); }

BOOL daNh_c::checkTimer() {
    WWHD_FUNC(0x021E0B18, BOOL, this);
    if (mType != TYPE_BOTTLE) return FALSE;
    if (gabi::call<s32>(0x0211D2F8, &mBottleTimer) != 0) return FALSE;
    s32 alpha = static_cast<s32>(static_cast<u32>(mAlpha) - 4u);
    if (alpha < 0) alpha = 0;
    mAlpha = alpha;
    if (alpha == 0) {
        fopAcM_delete(this);
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x021E0B18, &daNh_c::checkTimer);

void daNh_c::airMove() {
    WWHD_FUNC(0x021E0CCC, void, this);
    const f32 idealY = f32(mGroundY) + f32(nhHio().heightAboveGround);
    if (f32(current.pos.y) < idealY - 10.0f)
        maxFallSpeed = nhHio().ascentSpeed;
    else if (f32(current.pos.y) > idealY + 10.0f)
        maxFallSpeed = nhHio().descentSpeed;
}
VERIFY(0x021E0CCC, &daNh_c::airMove);

BOOL daNh_c::moveProc(f32 targetSpeed, f32 speedStep, u32 targetAngle) {
    WWHD_FUNC(0x021E17A0, BOOL, this, targetSpeed, speedStep, targetAngle);
    // GHS forwards the incoming angle register without narrowing it.
    gabi::call<BOOL>(0x0200F8D0, &current.angle.y, targetAngle, 0x400);
    shape_angle.y = current.angle.y;
    return cLib_chaseF(&speedF, targetSpeed, speedStep) && targetSpeed == 0.0f;
}
VERIFY(0x021E17A0, &daNh_c::moveProc);

f32 daNh_c::getHomeDistance() {
    WWHD_FUNC(0x021E1868, f32, this);
    gabi::Local<cXyz> delta, horizontal;
    // The reference vector subtraction has destination in its second argument.
    gabi::call(0x0201ADE0, &home.pos, delta.get(), &current.pos);
    horizontal->x = delta->x;
    horizontal->y = 0.0f;
    horizontal->z = delta->z;
    gabi::call<f32>(0x028E8DD0, horizontal.get());
    return gabi::call<f32>(0x028F4384);
}
VERIFY(0x021E1868, &daNh_c::getHomeDistance);

static u32 NhPlayBrk(daNh_c* actor) {
    WWHD_FUNC(0x021E09DC, u32, actor);
    return gabi::call<u32>(0x025E742C, actor->mBrkAnm);
}
VERIFY(0x021E09DC, NhPlayBrk);

static BOOL NhIsDelete(daNh_c* actor) {
    WWHD_FUNC(0x021E133C, BOOL, actor);
    return TRUE;
}
VERIFY(0x021E133C, NhIsDelete);

static BOOL NhDelete(daNh_c* actor) {
    WWHD_FUNC(0x021E1344, BOOL, actor);
    gabi::call(0x0255A374, &actor->mLightPos);
    return TRUE;
}
VERIFY(0x021E1344, NhDelete);

static daNh_HIO_c* NhHioCtor(daNh_HIO_c* hio) {
    WWHD_FUNC(0x021E1D48, daNh_HIO_c*, hio);
    if (!hio) hio = gabi::call<daNh_HIO_c*>(0x0273AD10, 0x48);
    if (!hio) return nullptr;
    hio->disposerVtable = 0x10015A30;
    hio->mNo = -1;
    // Fifteen initialized words include the packed angle, alpha and lifetime.
    auto* parameters = gabi::at<be<u32>>(gabi::ea(hio) + 4);
    auto* defaults = gabi::at<be<u32>>(0x10015AD0);
    for (int i = 0; i < 15; ++i) parameters[i] = defaults[i];
    return hio;
}
VERIFY(0x021E1D48, NhHioCtor);

static void NhStaticInit() {
    WWHD_FUNC(0x021E1DC0, void);
    auto* globals = gabi::at<be<u32>>(0x104658F8);
    globals[2] = 0; globals[0] = 0; globals[3] = 0; globals[1] = 0;
    gabi::call(0x028F026C, gabi::at<void>(0x101BB0EC));
    *gabi::at<be<f32>>(0x104658EC) = *gabi::at<be<f32>>(0x10015B10);
    *gabi::at<be<f32>>(0x104658F0) = *gabi::at<be<f32>>(0x10015B14);
    gabi::call(0x028ED6F8, gabi::at<void>(0x104658F4));
    gabi::call(0x028F026C, gabi::at<void>(0x101BB0F8));
    gabi::call(0x028EAB2C, gabi::at<void>(0x104658F5));
    gabi::call(0x028F026C, gabi::at<void>(0x101BB104));
    NhHioCtor(gabi::at<daNh_HIO_c>(0x10465908));
}
VERIFY(0x021E1DC0, NhStaticInit);

static void NhSafeStringDtor(SafeString* key, u32 flags) {
    WWHD_FUNC(0x021E1E60, void, key, flags);
    if (key && (flags & 1)) gabi::call(0x0273AF40, key);
}
VERIFY(0x021E1E60, NhSafeStringDtor);

static void NhSafeStringVirtualEmpty(SafeString* key, void* context) {
    WWHD_FUNC(0x021E1E74, void, key, context);
}
VERIFY(0x021E1E74, NhSafeStringVirtualEmpty);

static s32 NhRandomRange(s32 base, s32 width) {
    WWHD_FUNC(0x021E1E78, s32, base, width);
    const f32 start = f32(base);
    const f32 range = f32(width);
    const f32 random = gabi::call<f32>(0x020198D8, range);
    return gabi::ftoi(f32(start + random));
}
VERIFY(0x021E1E78, NhRandomRange);

BOOL daNh_c::initBrkAnm(u32 modify) {
    WWHD_FUNC(0x021E07A4, BOOL, this, modify);
    J3DModelData* modelData = *gabi::at<gptr<J3DModelData>>(gabi::ea(mpModel.get()) + 0xAC);
    void* animation = dComIfG_getObjectRes(gabi::at<const char>(0x10015A44), 0x4E, 0x100158F8);
    if (!animation)
        JUT_ASSERT_fail(gabi::at<const char>(0x10015A4C), 0x37F, gabi::at<const char>(0x10015A58));
    return gabi::call<BOOL>(0x025E8154, mBrkAnm, modelData, animation,
                           1, 2, 1.0f, 0, -1, modify, 0) != 0;
}
VERIFY(0x021E07A4, &daNh_c::initBrkAnm);

BOOL daNh_c::createHeap() {
    WWHD_FUNC(0x021E0868, BOOL, this);
    auto* modelData = static_cast<J3DModelData*>(dComIfG_getObjectRes(
        gabi::at<const char>(0x10015A68), 0x34, 0x100158F8));
    if (!modelData)
        JUT_ASSERT_fail(gabi::at<const char>(0x10015A70), 0x167, gabi::at<const char>(0x10015A7C));
    mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    return mpModel.get() && initBrkAnm(0);
}
VERIFY(0x021E0868, &daNh_c::createHeap);

static BOOL NhCreateHeap(daNh_c* actor) {
    WWHD_FUNC(0x021E0938, BOOL, actor);
    return actor->createHeap();
}
VERIFY(0x021E0938, NhCreateHeap);

BOOL daNh_c::draw() {
    WWHD_FUNC(0x021E093C, BOOL, this);
    void* lighting = gabi::call<void*>(0x02555D0C);
    gabi::call(0x025626A4, lighting, 0, &current.pos, &tevStr);
    lighting = gabi::call<void*>(0x02555D0C);
    gabi::call(0x02562F5C, lighting, mpModel.get(), &tevStr);
    J3DModelData* data = *gabi::at<gptr<J3DModelData>>(gabi::ea(mpModel.get()) + 0xAC);
    const f32 frame = *gabi::at<be<f32>>(gabi::ea(this) + 0x72C);
    gabi::call(0x025E83FC, mBrkAnm, data, frame);
    gabi::call(0x025E2DE0, mpModel.get(), 0);
    *gabi::at<be<u32>>(gabi::ea(data) + 0x48) = 0;
    cLib_chaseF(&mLightBlend, 100.0f, 10.0f);
    return TRUE;
}
VERIFY(0x021E093C, &daNh_c::draw);

static BOOL NhDraw(daNh_c* actor) {
    WWHD_FUNC(0x021E09D8, BOOL, actor);
    return actor->draw();
}
VERIFY(0x021E09D8, NhDraw);

static void nhInvoke(daNh_c* actor, const NhAction& action, void* context) {
    const s16 adjustment = action.thisAdjustment;
    const s16 index = action.virtualIndex;
    void* receiver = gabi::at<void>(gabi::ea(actor) + s32(adjustment));
    u32 function;
    if (index < 0) {
        function = action.target;
    } else {
        const s16 tableOffset = s16(u32(action.target));
        const u32 table = gabi::load<u32>(gabi::ea(receiver) + s32(tableOffset));
        function = gabi::load<u32>(table + u32(s32(index)) * 8 + 4);
    }
    gabi::call_ptr(function, receiver, context);
}

BOOL daNh_c::setAction(const NhAction* next, void* context) {
    WWHD_FUNC(0x021E0D1C, BOOL, this, next, context);
    const s16 nextIndex = next->virtualIndex;
    const s16 oldIndex = mCurrAction.virtualIndex;
    if (oldIndex == nextIndex && oldIndex == 0) return TRUE;
    const s16 adjustment = next->thisAdjustment;
    const u32 target = next->target;
    if (oldIndex == nextIndex && s16(mCurrAction.thisAdjustment) == adjustment &&
        u32(mCurrAction.target) == target) return TRUE;
    if (oldIndex != 0) {
        mActionStatus = -1;
        nhInvoke(this, mCurrAction, context);
    }
    mCurrAction.target = target;
    mCurrAction.virtualIndex = nextIndex;
    mCurrAction.thisAdjustment = adjustment;
    unk812 = 0; unk810 = 0; mActionStatus = 0;
    mEscapeTimer = 0; unk80E = 0; unk818 = 0.0f;
    nhInvoke(this, mCurrAction, context);
    return TRUE;
}
VERIFY(0x021E0D1C, &daNh_c::setAction);

void daNh_c::action(void* context) {
    WWHD_FUNC(0x021E0FA4, void, this, context);
    if (s16(mCurrAction.virtualIndex) == 0) {
        speedF = 0.0f;
        gabi::Local<NhAction> wait;
        wait->thisAdjustment = gabi::load<s16>(0x100158D8);
        wait->virtualIndex = gabi::load<s16>(0x100158DA);
        wait->target = gabi::load<u32>(0x100158DC);
        setAction(wait.get(), nullptr);
    }
    nhInvoke(this, mCurrAction, context);
}
VERIFY(0x021E0FA4, &daNh_c::action);

static void nhAcchTables(daNh_c* actor, bool constructing) {
    const u32 base = gabi::ea(actor);
    if (constructing) gabi::store<u32>(base + 0x3C8, 0x10015A00);
    gabi::store<u32>(base + 0x3D8, 0x10015A10);
    gabi::store<u32>(base + 0x3CC, 0x10015A20);
    if (constructing) gabi::store<u8>(base + 0x3D0, 1);
}

static daNh_c* NhActorCtor(daNh_c* actor) {
    WWHD_FUNC(0x021E136C, daNh_c*, actor);
    if (!actor) actor = gabi::call<daNh_c*>(0x0273AD10, 0x844);
    if (!actor) return nullptr;
    fopAc_ac_c_ct(actor);
    actor->__vtbl = 0x10015B18;
    gabi::call(0x024F0474, actor->mAcch);
    nhAcchTables(actor, true);
    gabi::call(0x024EFE94, actor->mAcchCir);
    gabi::call(0x0200BD2C, &actor->mStts);
    gabi::call(0x02515DA0, &actor->mStts.__vtbl_gstts);
    actor->mStts.__vtbl = 0x1004AE88;
    actor->mStts.__vtbl_gstts = 0x1004AEC0;
    gabi::call(0x02515FB8, &actor->mCyl);
    const u32 base = gabi::ea(actor);
    // The cylinder contains ShapeAttr, bounding-box and geometry base subobjects.
    gabi::store<u32>(base + 0x70C, 0x100015A8);
    gabi::store<u32>(base + 0x708, 0x10015910);
    gabi::call(0x02018590, &actor->mCyl.mCyl);
    gabi::store<u32>(base + 0x634, 0x1004B108);
    gabi::store<u32>(base + 0x724, 0x1004B150);
    gabi::store<u32>(base + 0x70C, 0x1004B160);
    gabi::call(0x025E80D0, actor->mBrkAnm);
    gabi::store<u16>(base + 0x7AC, 0xFFFF);
    gabi::store<u32>(base + 0x7B4, 0xFFFFFFFF);
    gabi::store<u16>(base + 0x7AE, 0x100);
    gabi::store<u32>(base + 0x7B0, 0);
    gabi::store<u32>(base + 0x7B8, 0x10015920);
    actor->mLightIntensity = 1.0f;
    return actor;
}
VERIFY(0x021E136C, NhActorCtor);

static void NhActorDtor(daNh_c* actor, u32 flags) {
    WWHD_FUNC(0x021E16D4, void, actor, flags);
    if (!actor) return;
    actor->__vtbl = 0x10015B18;
    const s8 child = nhHio().mNo;
    if (child >= 0) {
        gabi::call(0x025F0A18, child);
        nhHio().mNo = -1;
    }
    gabi::call(0x02515A70, &actor->mCyl, 2);
    gabi::call(0x02515860, &actor->mStts, 2);
    gabi::call(0x02018034, gabi::at<void>(gabi::ea(actor) + 0x590), 2);
    nhAcchTables(actor, false);
    gabi::call(0x024EFD9C, actor->mAcch, 0);
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1) gabi::call(0x0273AF40, actor);
}
VERIFY(0x021E16D4, NhActorDtor);

static BOOL nhChangeAction(daNh_c* actor, u32 descriptor) {
    gabi::Local<NhAction> next;
    next->thisAdjustment = gabi::load<s16>(descriptor);
    next->virtualIndex = gabi::load<s16>(descriptor + 2);
    next->target = gabi::load<u32>(descriptor + 4);
    return actor->setAction(next.get(), nullptr);
}

BOOL daNh_c::waitAction(void* context) {
    WWHD_FUNC(0x021E18BC, BOOL, this, context);
    if (s8(mActionStatus) == 0) {
        mActionStatus = 1;
        mPlayerDist = fopAcM_searchPlayerDistance(this);
    } else if (s8(mActionStatus) != -1) {
        gabi::call<f32>(0x0200ECD4, &speedF, 0.0f, 0.1f, 10.0f, 1.0f);
        if (getHomeDistance() > 50.0f) nhChangeAction(this, 0x100158E8);
    }
    return TRUE;
}
VERIFY(0x021E18BC, &daNh_c::waitAction);

BOOL daNh_c::checkEscapeEnd() {
    WWHD_FUNC(0x021E1988, BOOL, this);
    gabi::Local<cXyz> delta, horizontal;
    gabi::call(0x0201ADE0, &home.pos, delta.get(), &current.pos);
    if (mType == TYPE_BOTTLE) return FALSE;
    if (gabi::call<u32>(0x02055B64, &mEscapeTimer) == 0) {
        nhChangeAction(this, 0x100158D8);
        return TRUE;
    }
    horizontal->x = delta->x; horizontal->y = 0.0f; horizontal->z = delta->z;
    const f32 distance2 = gabi::call<f32>(0x028E8DD0, horizontal.get());
    const f32 radius = nhHio().maxHomeDist;
    if (distance2 > f32(radius * radius)) {
        nhChangeAction(this, 0x100158E8);
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x021E1988, &daNh_c::checkEscapeEnd);

BOOL daNh_c::escapeAction(void* context) {
    WWHD_FUNC(0x021E1A90, BOOL, this, context);
    if (s8(mActionStatus) == 0) {
        mActionStatus = 1;
        mWobbleTimer = 0; mEscapeTimer = 150; mWobbleDir = 0;
    } else if (s8(mActionStatus) != -1 && !checkEscapeEnd()) {
        s16 target = s16(u16(fopAcM_searchPlayerAngleY(this)) + 0x8000u);
        if (gabi::call<u32>(0x0207A9A0, &mWobbleTimer) == 0) {
            mWobbleDir ^= 1;
            mWobbleTimer = u8(NhRandomRange(15, 20));
        }
        target = s16(s32(target) + (mWobbleDir ? -0x2000 : 0x2000));
        moveProc(5.0f, 0.5f, u32(s32(target)));
    }
    return TRUE;
}
VERIFY(0x021E1A90, &daNh_c::escapeAction);

BOOL daNh_c::returnAction(void* context) {
    WWHD_FUNC(0x021E1B88, BOOL, this, context);
    if (s8(mActionStatus) == 0) {
        mActionStatus = 1;
        mWobbleTimer = 0; mEscapeTimer = 150; mWobbleDir = 0;
    } else if (s8(mActionStatus) != -1) {
        if (getHomeDistance() < 50.0f) {
            nhChangeAction(this, 0x100158D8);
        } else {
            s16 target = cLib_targetAngleY(&current.pos, &home.pos);
            gabi::Local<cXyz> delta, horizontal;
            gabi::call(0x0201ADE0, &home.pos, delta.get(), &current.pos);
            horizontal->x = delta->x; horizontal->y = 0.0f; horizontal->z = delta->z;
            const f32 distance2 = gabi::call<f32>(0x028E8DD0, horizontal.get());
            const f32 radius = nhHio().maxHomeDist;
            if (distance2 < f32(radius * radius)) {
                const s16 difference = s16(s32(target) - s32(fopAcM_searchPlayerAngleY(this)));
                if (std::abs(s32(difference)) < 0x1000)
                    target = s16(s32(target) + (difference < 0 ? -0x4000 : 0x4000));
            }
            if (gabi::call<u32>(0x0207A9A0, &mWobbleTimer) == 0) {
                mWobbleDir ^= 1;
                mWobbleTimer = u8(NhRandomRange(15, 20));
            }
            target = s16(s32(target) + (mWobbleDir ? -0x2000 : 0x2000));
            moveProc(5.0f, 0.5f, u32(s32(target)));
        }
    }
    return TRUE;
}
VERIFY(0x021E1B88, &daNh_c::returnAction);

BOOL daNh_c::searchPlayer() {
    WWHD_FUNC(0x021E0E5C, BOOL, this);
    if (mType == TYPE_BOTTLE) {
        nhChangeAction(this, 0x100158E0);
        return TRUE;
    }
    gabi::Local<cXyz> delta, horizontal;
    fopAc_ac_c* player = dComIfGp_getPlayer0();
    gabi::call(0x0201ADE0, &player->old.pos, delta.get(), &player->current.pos);
    const f32 distance = fopAcM_searchPlayerDistance(this);
    const f32 change = f32(mPlayerDist) - distance;
    horizontal->x = delta->x; horizontal->z = delta->z;
    mPlayerDist = distance;
    horizontal->y = 0.0f;
    gabi::call<f32>(0x028E8DD0, horizontal.get());
    const f32 movement = gabi::call<f32>(0x028F4384);
    if (movement > 0.001f && distance < 600.0f && change > f32(nhHio().minFrightenSpeed)) {
        nhChangeAction(this, 0x100158E0);
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x021E0E5C, &daNh_c::searchPlayer);

BOOL daNh_c::checkBinCatch() {
    WWHD_FUNC(0x021E0BC0, BOOL, this);
    if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 6) {
        fopAcM_delete(this);
        return TRUE;
    }
    const s16 angle = nhHio().catchAngle;
    const f32 distance = nhHio().catchDistance;
    const f32 lower = nhHio().catchLower;
    const f32 upper = nhHio().catchUpper;
    u8* game = dComIfGp_get();
    gabi::call(0x024EBB70, game + 0x5934, this, 0x58, distance, upper, lower, angle, 1);
    auto* condition = gabi::at<be<u16>>(gabi::ea(this) + 0xFA);
    *condition |= 0x40;
    return FALSE;
}
VERIFY(0x021E0BC0, &daNh_c::checkBinCatch);

void daNh_c::BGCheck() {
    WWHD_FUNC(0x021E09E4, void, this);
    gabi::Local<dBgS_GndChk> ground;
    const dBgS_GndChk_vt tables = {0x100159C0, 0x100159D0, 0x100159F0, 0x100159E0};
    dBgS_GndChk_ct(ground.get(), tables, true);
    dBgS_GndChk_SetPos(ground.get(), &current.pos);
    // This actor checks all background groups.
    gabi::store<u32>(gabi::ea(ground.get()) + 0x50, 15);
    const f32 height = gabi::call<f32>(0x02008974, dComIfGp_get() + 0x12A0, ground.get());
    if (height != -1000000000.0f) {
        mGroundY = height;
        auto* polygon = dBgS_GndChk_PolyInfo(ground.get());
        const u32 room = gabi::call<u32>(0x024EF130, dComIfGp_get() + 0x12A0, polygon);
        tevStr.mRoomNo = s8(room);
        current.roomNo = s8(room);
        const u32 color = gabi::call<u32>(0x024EEEB8, dComIfGp_get() + 0x12A0, polygon);
        gabi::store<u8>(gabi::ea(this) + 0x1CA, u8(color));
    }
    gabi::store<u32>(gabi::ea(ground.get()) + 0x20, 0x10015950);
    gabi::store<u32>(gabi::ea(ground.get()) + 0x40, 0x10015970);
    gabi::store<u32>(gabi::ea(ground.get()) + 0x4C, 0x10015930);
    gabi::call(0x02008DAC, ground.get(), 0);
}
VERIFY(0x021E09E4, &daNh_c::BGCheck);

void daNh_c::setBaseMtx() {
    WWHD_FUNC(0x021E105C, void, this);
    J3DModel* model = mpModel.get();
    const f32 modelScale = nhHio().modelScale;
    scale.y = modelScale; scale.z = modelScale; scale.x = modelScale;
    auto* modelScaleVector = gabi::at<cXyz>(gabi::ea(model) + 0xBC);
    modelScaleVector->x = modelScale; modelScaleVector->y = modelScale; modelScaleVector->z = modelScale;
    void* matrix = gabi::at<void>(0x1048D0CC);
    gabi::call(0x028E93CC, matrix, f32(current.pos.x), f32(current.pos.y), f32(current.pos.z));
    gabi::call(0x025F1C28, matrix, s16(shape_angle.y));
    auto* source = gabi::at<be<f32>>(0x1048D0CC);
    f32 snapshot[12];
    for (int i = 0; i < 12; ++i) snapshot[i] = source[i];
    auto* modelMatrix = gabi::at<be<f32>>(gabi::ea(model) + 0xC8);
    for (int i = 0; i < 12; ++i) modelMatrix[i] = snapshot[i];
    gabi::Local<cXyz> glowOffset, glowPosition;
    glowOffset->x = 0.0f; glowOffset->y = nhHio().glowOffsetY; glowOffset->z = 0.0f;
    gabi::call(0x028E8F64, matrix, glowOffset.get(), glowPosition.get());
    gabi::call(0x028E93CC, matrix, f32(glowPosition->x), f32(glowPosition->y), f32(glowPosition->z));
    const f32 glowScale = nhHio().glowScale;
    gabi::call(0x025F2518, glowScale, glowScale, glowScale);
    gabi::call(0x028E90D4, matrix, mGlowMtx);
}
VERIFY(0x021E105C, &daNh_c::setBaseMtx);

BOOL daNh_c::init() {
    WWHD_FUNC(0x021E14B4, BOOL, this);
    mType = u8(mParameters);
    speed.y = 1.0f;
    gravity = nhHio().gravity;
    mPlayerDist = 0.0f;
    mGlowAlpha = u8(s16(nhHio().defaultGlowAlpha));
    mAlpha = 255;
    mGroundY = 0.0f;
    mBottleTimer = s16(nhHio().bottleLifetime);
    BGCheck();
    mStts.Init(255, 255, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101BB0A8));
    mCyl.SetStts(&mStts);
    setBaseMtx();
    mLightPos.copy(current.pos);
    eyePos.copy(current.pos);
    gabi::at<cXyz>(gabi::ea(this) + 0x390)->copy(current.pos);
    mLightRed = 300; mLightGreen = 300; mLightBlue = 300;
    mLightParameter = 250.0f;
    mLightBlend = 0.0f;
    gabi::call(0x025564B4, &mLightPos);
    return TRUE;
}
VERIFY(0x021E14B4, &daNh_c::init);

// A free entry preserves the reference's null-receiver constructor guard.
// Member views use guest addresses so the test harness also handles address zero.
static s32 NhCreateBody(daNh_c* actor) {
    WWHD_FUNC(0x021E15DC, s32, actor);
    const u32 base = gabi::ea(actor);
    auto* condition = gabi::at<be<u32>>(base + 0x2E4);
    if (!(u32(*condition) & fopAcCnd_INIT_e)) {
        if (actor) NhActorCtor(actor);
        *condition |= fopAcCnd_INIT_e;
    }
    if (!gabi::call<BOOL>(0x025D63E8, actor, gabi::at<void>(0x021E0938), 0x4000)) return 5;
    J3DModel* model = *gabi::at<gptr<J3DModel>>(base + 0x3B4);
    *gabi::at<be<u32>>(base + 0x348) = model ? gabi::ea(model) + 0xC8 : 0;
    if (s8(nhHio().mNo) < 0) {
        const s32 child = gabi::call<s32>(0x025F0A10, gabi::at<const char>(0x10015AB4), &nhHio());
        nhHio().mpActor = actor;
        nhHio().mNo = s8(child);
    }
    return gabi::call<BOOL>(0x021E14B4, actor) ? 4 : 5;
}
VERIFY(0x021E15DC, NhCreateBody);

static s32 NhCreate(daNh_c* actor) {
    WWHD_FUNC(0x021E16D0, s32, actor);
    return NhCreateBody(actor);
}
VERIFY(0x021E16D0, NhCreate);

BOOL daNh_c::execute() {
    WWHD_FUNC(0x021E11A4, BOOL, this);
    NhPlayBrk(this);
    const f32 fallSpeed = maxFallSpeed;
    const f32 verticalSpeed = speed.y;
    const u16 angle = u16(current.angle.y);
    mGlowAlpha = u8(s16(nhHio().defaultGlowAlpha));
    if (fallSpeed < verticalSpeed) {
        const f32 next = verticalSpeed - f32(gravity);
        speed.y = next;
        if (next < f32(maxFallSpeed)) speed.y = maxFallSpeed;
    } else if (fallSpeed > verticalSpeed) {
        const f32 next = verticalSpeed + f32(gravity);
        speed.y = next;
        if (next > f32(maxFallSpeed)) speed.y = maxFallSpeed;
    }
    const f32 forwardSpeed = speedF;
    speed.x = forwardSpeed * cM_ssin(s16(angle));
    speed.z = forwardSpeed * cM_scos(s16(angle));
    gabi::call(0x025D6800, this, &mStts);
    mLightPos.copy(current.pos);
    BGCheck();
    gabi::Local<cXyz> center;
    center->x = current.pos.x;
    center->y = f32(current.pos.y) - 10.0f;
    center->z = current.pos.z;
    mCyl.SetC(center.get());
    gabi::call(0x0200E240, dComIfGp_get() + 0x26A4, &mCyl);
    checkTimer();
    if (!checkBinCatch()) {
        airMove();
        searchPlayer();
        action(nullptr);
    }
    setBaseMtx();
    eyePos.copy(current.pos);
    gabi::at<cXyz>(gabi::ea(this) + 0x390)->copy(current.pos);
    return TRUE;
}
VERIFY(0x021E11A4, &daNh_c::execute);

static BOOL NhExecute(daNh_c* actor) {
    WWHD_FUNC(0x021E1338, BOOL, actor);
    return actor->execute();
}
VERIFY(0x021E1338, NhExecute);
