/**
 * d_a_dai_item.cpp (WWHD)
 * Object - Stand item (Windfall display-stand items: flowers, flags, pinwheel,
 * idols, statues)
 *
 * Ported from the GameCube
 * decompilation (zeldaret/tww) and checked against WWHD.
 */
#include "d/actor/d_a_dai_item.h"

namespace {
using Actor = daStandItem_c;
void invokeMember(Actor *actor, const StandItemMethodHD *method) {
    // HD: eight-byte GHS member descriptors; host member pointers have a
    // different ABI.
    s32 index = method->mVtableIndex;
    u32 object = gabi::ea(actor) + (s32)method->mThisAdjustment;
    u32 target;
    if (index < 0)
        target = method->mTarget;
    else {
        s32 offset = (s16)(u32)method->mTarget;
        auto *table = gabi::at<gptr<StandItemMethodHD>>(object + offset)->get();
        target = table[index].mTarget;
    }
    gabi::call_ptr(target, gabi::at<Actor>(object));
}
} // namespace

namespace {
void invalidateEmitter(gptr<StandItemEmitterHD> &slot, bool immortal = false) {
    auto *emitter = slot.get();
    if (!emitter)
        return;
    if (immortal) {
        emitter->mFlags = (u32)emitter->mFlags & ~EMITTER_IMMORTAL;
        emitter = slot.get();
    }
    u32 flags = emitter->mFlags;
    emitter->mMaxFrame = -1;
    emitter->mFlags = flags | EMITTER_INVALID;
    slot = nullptr;
}
s16 randomTimer(s16 lo, s16 hi) {
    s16 mid = (s16)(((s32)lo + hi) / 2), width = (s16)(((s32)hi - lo) / 2);
    f32 deviation = standItemRandomFX((f32)width);
    return (s16)gabi::ftoi((f32)mid + deviation);
}
void restartAnimation(Actor *a) {
    a->mpBckAnm.get()->mRate = 1.0f;
    a->mpBckAnm.get()->mFrame = 0.0f;
}
} // namespace

namespace {
u32 createParticle(Actor *a, u32 effect, bool colored) {
    s32 room = colored ? (s32)(s8)a->current.roomNo : -1;
    u32 play = gabi::ea(dComIfGp_get()),
        control = gabi::ea(gabi::at<StandItemPlayHD>(play)->mpParticle.get());
    return standItemParticleSet(gabi::at<u8>(control), 0, effect, &a->current.pos,
                                &a->current.angle, 0, 0xFF, 0, room,
                                colored ? &a->tevStr.mColorK0 : nullptr, 0, 0);
}
void setEmitterMatrix(u32 emitter, const Mtx34 *matrix) {
    standItemEmitterMatrix(matrix, &gabi::at<StandItemEmitterHD>(emitter)->mGlobalRotation,
                           &gabi::at<StandItemEmitterHD>(emitter)->mGlobalTranslation);
}
} // namespace

namespace {
void attachJointCallback(Actor *a, const char *name) {
    u32 model = gabi::ea(a->mpModel.get()),
        data = gabi::ea(gabi::at<J3DModel>(model)->mModelData.get());
    u32 table = standItemJointNameTable(gabi::at<u8>(data));
    u32 relative = (u32)gabi::at<StandItemJointNameHD>(table)->mNameTableOffset,
        names = relative ? table + 0x10 + relative : 0;
    s32 index = standItemJointIndex(gabi::at<u8>(names), name);
    if (index >= 0) {
        model = gabi::ea(a->mpModel.get());
        data = gabi::ea(gabi::at<J3DModel>(model)->mModelData.get());
        u32 count = (u32)gabi::at<J3DModelData>(data)->mJointNum,
            joints = (u32)gabi::at<J3DModelData>(data)->mJointNodes, slot = (u16)index;
        if (slot < count)
            joints += slot * 0x1C;
        gabi::at<J3DJoint>(joints)->mCallBack = STAND_ITEM_NODE_CALLBACK;
    }
}
} // namespace

/* 021177AC */
BOOL daStandItem_c::_delete() {
    invalidateEmitter(m694);
    invalidateEmitter(m690);
    invalidateEmitter(m698, true);
    standItemResourceDelete(&mPhsDai, "Fdai");
    standItemResourceDelete(&mPhsCloth, "Cloth");
    return 1;
}

/* 02116E28 */
BOOL CheckCreateHeap(daStandItem_c *a) { return a->CreateHeap(); }

/* 02116B10 */
BOOL daStandItem_c::CreateHeap() {
    s32 modelIndex = (s16)standItemTables()->m_bmdidx[(u32)mItemType];
    u32 data = gabi::ea(dComIfG_getObjectRes("Fdai", modelIndex, 0x1000CC9C));
    if (!data)
        standItemPanic("d_a_dai_item.cpp", 0x239, "modelData != (0)");
    bool statue = mItemNo == 0x97;
    u32 model = standItemModelCreate(gabi::at<u8>(data), statue ? 0u : 0x80000u,
                                     statue ? 0x11020203u : 0x11000022u);
    mpModel = gabi::at<J3DModel>(model);
    if (!model)
        return 0;
    s32 animationIndex = (s16)standItemTables()->m_bckidx[(u32)mItemType];
    if (animationIndex != -1) {
        u32 animationData = gabi::ea(dComIfG_getObjectRes("Fdai", animationIndex, 0x1000CC9C));
        if (!animationData)
            standItemPanic("d_a_dai_item.cpp", 0x250, "pbck != (0)");
        u32 animation = standItemNew(0x8C);
        if (animation) {
            standItemAnimationConstruct(gabi::at<u8>(animation), 0);
            gabi::at<StandItemBckCtorHD>(animation)->__vtbl = STAND_ITEM_ANIMATION_BASE_VTABLE;
            standItemFrameCtrlConstruct(&gabi::at<StandItemBckCtorHD>(animation)->mFrameCtrl);
            gabi::at<StandItemBckCtorHD>(animation)->mAnimation80 = 0;
            gabi::at<StandItemBckCtorHD>(animation)->mAnimation58 = 0;
            gabi::at<StandItemBckCtorHD>(animation)->mFrameCtrlVtable =
                STAND_ITEM_FRAME_CTRL_VTABLE;
            gabi::at<StandItemBckCtorHD>(animation)->mAnimation84 = 0;
            gabi::at<StandItemBckCtorHD>(animation)->__vtbl = STAND_ITEM_ANIMATION_VTABLE;
            gabi::at<StandItemBckCtorHD>(animation)->mAnimation7C = 0;
            gabi::at<StandItemBckCtorHD>(animation)->mAnimation88 = 0;
        }
        mpBckAnm = gabi::at<StandItemBckCtorHD>(animation);
        if (!animation)
            return 0;
        u32 mode = (u32)(*gabi::at<be<u32>[12]>(0x1000CD40))[(u32)mItemType];
        if (!standItemAnimationInit(gabi::at<u8>(animation), gabi::at<u8>(data),
                                    gabi::at<u8>(animationData), 1, mode, 1.0f, 0, -1, 0))
            return 0;
        mpBckAnm.get()->mRate = 0.0f;
    }
    u8 item = mItemNo;
    if ((item >= 0x8F && item <= 0x91) || item == 0x93) {
        u32 type = (item >= 0x8F && item <= 0x91)
                       ? (u8)(*gabi::at<be<u8>[3]>(0x1000CD3C))[item - 0x8F]
                       : 3;
        mClothType = (u8)type;
        u32 texture = gabi::ea(dComIfG_getObjectRes("Fdai", (s32)(32 + type), 0x1000CC9C));
        u32 toon = gabi::ea(dComIfG_getObjectRes("Cloth", 3, 0x1000CC9C));
        u32 actualType = (u8)mClothType;
        struct Factories {
            be<u32> entries[4];
        };
        gabi::Local<Factories> factories;
        factories->entries[0] = STAND_ITEM_CLOTH03_FACTORY;
        factories->entries[1] = STAND_ITEM_CLOTH04_FACTORY;
        factories->entries[2] = STAND_ITEM_CLOTH05_FACTORY;
        factories->entries[3] = STAND_ITEM_CLOTH07_FACTORY;
        u32 positions = (u32)(*gabi::at<be<u32>[4]>(0x101B3E68))[actualType];
        u32 cloth = gabi::call_ptr<u32>((u32)factories->entries[actualType], gabi::at<u8>(texture),
                                        gabi::at<u8>(toon), &tevStr, gabi::at<u8>(positions));
        mpCloth = gabi::at<StandItemClothHD>(cloth);
        if (!cloth)
            return 0;
    } else
        mpCloth = nullptr;
    return 1;
}

/* 02117168 */
void daStandItem_c::CreateInit() {
    u32 model = gabi::ea(mpModel.get());
    cullMtx = model ? model + 0xC8 : 0;
    standItemCullBox(this, -100.0f, -0.0f, -100.0f, 100.0f, 300.0f, 100.0f);
    standItemAcchCircleSetWall(&mAcchCir, 30.0f, 30.0f);
    standItemAcchSet(&mAcch, &current.pos, &old.pos, this, 1, &mAcchCir, &speed, 0, 0);
    if ((u32)actor_status & 0x2000u)
        mode_carry_init();
    else
        mode_wait_init();
    mCarry = 0;
    u32 wind = standItemWindVector();
    standItemWord(&m6A4.x) = gabi::at<StandItemPointWordsHD>(wind)->x;
    standItemWord(&m6A4.y) = gabi::at<StandItemPointWordsHD>(wind)->y;
    standItemWord(&m6A4.z) = gabi::at<StandItemPointWordsHD>(wind)->z;
    s16 angle = standItemAtan2s((f32)m6A4.x, (f32)m6A4.z);
    m6A2 = angle;
    set_mtx();
    model = gabi::ea(mpModel.get());
    gabi::at<J3DModel>(model)->mUserArea = 0;
    u8 item = mItemNo;
    if (item == 0x92 || item == 0x95) {
        if (item == 0x92)
            attachJointCallback(this, "top");
        else {
            attachJointCallback(this, "tuboko_head");
            attachJointCallback(this, "tuboko_base");
        }
        model = gabi::ea(mpModel.get());
        gabi::at<J3DModel>(model)->mUserArea = gabi::ea(this);
        standItemModelCalc(mpModel.get());
    }
    f32 windPower = standItemWindPower();
    u32 index = (u32)mItemType * 2;
    m6BC = windPower;
    s16 animMin = (s16)standItemTables()->m_anim_min_time[index / 2],
        animMax = (s16)standItemTables()->m_anim_max_time[index / 2],
        stopMin = (s16)standItemTables()->m_stop_min_time[index / 2],
        stopMax = (s16)standItemTables()->m_stop_max_time[index / 2];
    s16 mid = (s16)(((s32)animMin + animMax) / 2), width = (s16)(((s32)animMax - animMin) / 2);
    mBckPlayTimer = 0;
    mBckStopTimer = randomTimer(stopMin, stopMax);
    if (stopMax == 0) {
        f32 deviation = standItemRandomFX((f32)width);
        mBckPlayTimer = (s16)gabi::ftoi((f32)mid + deviation);
    }
    m690 = nullptr;
    m694 = nullptr;
    m698 = nullptr;
    u32 light = gabi::ea(dKy_getEnvlight());
    standItemSettingTevStruct(gabi::at<u8>(light), 0, &current.pos, &tevStr);
}

/* 021175A8 */
s32 daStandItem_c::_create() {
    u32 status = (u32)actor_condition;
    if (!(status & 8)) {
        // HD: retain the inline GHS constructor's null-this guard and store order.
        if (this) {
            standItemActorConstruct(this);
            __vtbl = STAND_ITEM_VTABLE;
            standItemSttsConstruct(&mStts);
            standItemGSttsConstruct(&mStts.__vtbl_gstts);
            mStts.__vtbl = STAND_ITEM_STTS_VTABLE;
            mStts.__vtbl_gstts = STAND_ITEM_GSTTS_VTABLE;
            standItemCollisionConstruct(&mCyl);
            standItemCtor(this)->mCylShapeVtable = STAND_ITEM_CYL_SHAPE_BASE_VTABLE;
            standItemCtor(this)->mCylShapePad = STAND_ITEM_CYL_SHAPE_VTABLE;
            standItemCylinderConstruct(&mCyl.mCyl);
            mCyl.__vtbl_hitinf = STAND_ITEM_CYL_HIT_VTABLE;
            standItemCtor(this)->mCylShapeVtable = STAND_ITEM_CYL_SHAPE_FINAL_VTABLE;
            mCyl.mCyl.__vtbl = STAND_ITEM_CYL_VTABLE;
            standItemAcchConstruct(&mAcch);
            standItemCtor(this)->mAcchVtable = STAND_ITEM_ACCH_VTABLE;
            standItemCtor(this)->mAcchGroupVtable = STAND_ITEM_ACCH_GROUP_VTABLE;
            standItemCtor(this)->mAcchPolyVtable = STAND_ITEM_ACCH_POLY_VTABLE;
            standItemCtor(this)->mAcchObjThrough = 1;
            standItemAcchCircleConstruct(&mAcchCir);
            status = (u32)actor_condition;
        }
        actor_condition = status | 8u;
    }
    u8 item = (u8)((u32)mParameters);
    mItemNo = item;
    mItemType =
        (item >= 0x8D && item <= 0x97) ? (u8)(*gabi::at<be<u8>[11]>(0x1000CDF4))[item - 0x8D] : 0;
    s32 result = standItemResourceLoad(&mPhsDai, "Fdai");
    if (result != 4)
        return result;
    s32 clothResult = standItemResourceLoad(&mPhsCloth, "Cloth");
    if (clothResult != 4)
        return clothResult;
    u32 size = (u16)standItemTables()->m_heapsize[(u32)mItemType];
    if (!standItemSolidHeap(this, gabi::at<u8>(STAND_ITEM_HEAP_CALLBACK), size))
        return 5;
    CreateInit();
    return result;
}

/* 02116EA8 */
void daStandItem_c::set_mtx() {
    f32 sx = (f32)scale.x, sz = (f32)scale.z, sy = (f32)scale.y;
    u32 model = gabi::ea(mpModel.get());
    gabi::at<J3DModel>(model)->mBaseScale.x = sx;
    gabi::at<J3DModel>(model)->mBaseScale.y = sy;
    gabi::at<J3DModel>(model)->mBaseScale.z = sz;
    u8 *matrix = gabi::at<u8>(0x1048D0CC);
    standItemMatrixTranslation(matrix, (f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
    standItemMatrixZXYRotate(matrix, (s16)current.angle.x, (s16)current.angle.y,
                             (s16)current.angle.z);
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = (*gabi::at<be<f32>[12]>(0x1048D0CC))[i];
    model = gabi::ea(mpModel.get());
    for (u32 i = 0; i < 12; ++i)
        gabi::at<J3DModel>(model)->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
    if (!gabi::ea(mpCloth.get()))
        return;
    struct Offsets {
        cXyz entries[4];
    };
    gabi::Local<Offsets> offsets;
    f32 tweak = (f32)*gabi::at<be<f32>>(0x1047BBEC), normal = tweak + 180.0f, tall = tweak + 195.0f;
    for (u32 i = 0; i < 4; ++i)
        offsets->entries[i].set(0.0f, i == 2 ? tall : normal, 0.0f);
    gabi::Local<cXyz> position, wind, copiedWind;
    u8 clothType = (u8)mClothType;
    standItemVectorAdd(&current.pos, position.get(), &offsets->entries[clothType]);
    standItemAllWind(wind.get(), position.get());
    copiedWind->copy(*wind.get());
    f32 windSq = standItemSquareMagnitude(copiedWind.get());
    f32 strength = standItemSquareRoot(windSq);
    f32 currentSq = standItemSquareMagnitude(&m6A4);
    f32 currentStrength = standItemSquareRoot(currentSq);
    if (strength - currentStrength > 0.25f) {
        f32 z = copiedWind->z, y = copiedWind->y;
        m6A4.z = z;
        f32 x = copiedWind->x;
        m6A4.y = y;
        m6A4.x = x;
        s16 angle = standItemAtan2s(x, z);
        standItemAddCalcAngle(&m6A2, angle, 4, 0x1000);
    } else {
        standItemAddCalcPos(&m6A4, copiedWind.get(), 0.1f, 0.1f);
        s16 angle = standItemAtan2s((f32)m6A4.x, (f32)m6A4.z);
        standItemAddCalcAngle(&m6A2, angle, 16, 0x400);
    }
    const cXyz *offset = &offsets->entries[(u8)mClothType];
    standItemMatrixTranslate(offset->x, offset->y, offset->z);
    standItemMatrixYRotate(matrix, (s16)((s16)m6A2 - (s16)current.angle.y));
    standItemClothSetMatrix(mpCloth.get(), matrix);
}

/* 02117A84 */
BOOL daStandItem_c::_execute() {
    mTimer = (s32)((u32)mTimer + 1u);
    execAction();
    itemProc();
    set_mtx();
    u32 cloth = gabi::ea(mpCloth.get());
    if (cloth) {
        auto *packet = gabi::at<StandItemClothHD>(cloth);
        gabi::call_ptr((u32)packet->__vtbl->mMove, packet);
    }
    return 1;
}

/* 02117A24 */
void daStandItem_c::itemProc() {
    if (mMode != MODE_WAIT)
        return;
    auto *descriptor = &(*gabi::at<StandItemMethodHD[12]>(0x101B3E98))[(u32)mItemType];
    if ((s16)descriptor->mVtableIndex)
        invokeMember(this, descriptor);
}

/* 02117E64 */
BOOL daStandItem_c::actionFobj00() {
    animTest();
    return 1;
}

/* 02117E88 */
BOOL daStandItem_c::actionFobj01() {
    animTest();
    return 1;
}

/* 02117EAC */
BOOL daStandItem_c::actionFobj02() {
    animTest();
    return 1;
}

/* 021187EC */
BOOL daStandItem_c::actionFobj03() { return 1; }

/* 021187F4 */
BOOL daStandItem_c::actionFobj04() { return 1; }

/* 02117ED0 */
BOOL daStandItem_c::actionFobj05() {
    f32 wind = standItemWindPower();
    u32 animation = gabi::ea(mpBckAnm.get());
    if (animation && wind > 0.0f)
        standItemAnimationPlay(gabi::at<u8>(animation));
    return 1;
}

/* 02117F20 */
BOOL daStandItem_c::actionFobj06() {
    gabi::Local<cXyz> wind, horizontal, zero;
    standItemAllWind(wind.get(), &current.pos);
    horizontal->set(wind->x, 0.0f, wind->z);
    f32 squared = standItemSquareMagnitude(horizontal.get());
    f32 strength = standItemSquareRoot(squared);
    zero->set(0.0f, 0.0f, 0.0f);
    s16 angle = standItemTargetAngle(zero.get(), wind.get());
    standItemDistanceAngle(angle, (s16)current.angle.y);
    f32 spin = (f32)m6C4 + __builtin_fabsf(strength);
    if (spin > 4.0f)
        spin = 4.0f;
    m6C4 = spin;
    m6B2 = (s16)gabi::ftoi(1536.0f * spin);
    f32 maxStep = standItemWindPower() + 0.3f;
    f32 minStep = standItemWindPower() + 0.1f;
    standItemAddCalc(&m6C4, 0.0f, 0.08f, maxStep, minStep);
    m6B4 = (s16)((u16)m6B4 + (u16)m6B2);
    return 1;
}

/* 02118064 */
BOOL daStandItem_c::actionFobj07() {
    f32 wind = standItemWindPower();
    u32 animation = gabi::ea(mpBckAnm.get());
    if (animation && wind > 0.0f)
        standItemAnimationPlay(gabi::at<u8>(animation));
    return 1;
}

/* 021180B4 */
BOOL daStandItem_c::actionFobj08() {
    animTest();
    return 1;
}

/* 02118390 */
BOOL daStandItem_c::actionFobj09() {
    u32 play = gabi::ea(dComIfGp_get()),
        link = gabi::ea(gabi::at<StandItemPlayHD>(play)->mpPlayer.get());
    gabi::Local<cXyz> delta, horizontal;
    standItemVectorSubtract(&gabi::at<fopAc_ac_c>(link)->current.pos, delta.get(), &current.pos);
    horizontal->set(delta->x, 0.0f, delta->z);
    f32 sq = standItemSquareMagnitude(horizontal.get()), distance = standItemSquareRoot(sq);
    animTestForOneTime();
    if (!mbBckDidPlay) {
        if (distance < 500.0f) {
            invalidateEmitter(m694);
            invalidateEmitter(m698, true);
            u32 emitter = gabi::ea(m690.get());
            if (!emitter)
                m690 = gabi::at<StandItemEmitterHD>(createParticle(this, 0x82B3, true));
            else
                setEmitterMatrix(emitter, &m660);
        } else {
            invalidateEmitter(m690);
            invalidateEmitter(m694);
            invalidateEmitter(m698, true);
        }
    } else {
        if (!m6B1) {
            invalidateEmitter(m690);
            if (!gabi::ea(m694.get()))
                m694 = gabi::at<StandItemEmitterHD>(createParticle(this, 0x82B4, true));
            u32 emitter = gabi::ea(m698.get());
            if (!emitter) {
                emitter = createParticle(this, 0x82B5, false);
                m698 = gabi::at<StandItemEmitterHD>(emitter);
            }
            if (emitter)
                gabi::at<StandItemEmitterHD>(emitter)->mFlags =
                    (u32)gabi::at<StandItemEmitterHD>(emitter)->mFlags | EMITTER_IMMORTAL;
        }
        u32 first = gabi::ea(m694.get());
        if (first)
            setEmitterMatrix(first, &m660);
        u32 second = gabi::ea(m698.get());
        if (second)
            setEmitterMatrix(second, &m630);
    }
    m6B1 = (u8)mbBckDidPlay;
    return 1;
}

/* 021186E4 */
BOOL daStandItem_c::actionFobj10() {
    animTest();
    return 1;
}

/* 02118708 */
BOOL daStandItem_c::actionFobj11() {
    mBckSpeed = 1.0f;
    animTest();
    return 1;
}

/* 02117B80 */
void daStandItem_c::animTest() {
    u32 index = (u32)mItemType * 2u;
    s16 stopMin = (s16)standItemTables()->m_stop_min_time[index / 2],
        animMax = (s16)standItemTables()->m_anim_max_time[index / 2];
    s16 animMin = (s16)standItemTables()->m_anim_min_time[index / 2],
        stopMax = (s16)standItemTables()->m_stop_max_time[index / 2];
    s16 play = mBckPlayTimer;
    mbBckDidPlay = 0;
    if (play > 0) {
        u32 animation = gabi::ea(mpBckAnm.get());
        mBckPlayTimer = (s16)(play - 1);
        if (animation) {
            standItemAnimationPlay(gabi::at<u8>(animation));
            mbBckDidPlay = 1;
        }
        if (stopMax == 0) {
            u32 animationNow = gabi::ea(mpBckAnm.get());
            mBckPlayTimer = 16;
            gabi::at<StandItemBckCtorHD>(animationNow)->mRate = 1.0f;
        }
        if (mBckPlayTimer < 15 && stopMax != 0) {
            standItemAddCalc(&mBckSpeed, 0.0f, 0.1f, 0.1f, 0.05f);
            f32 speed = mBckSpeed;
            mpBckAnm.get()->mRate = speed;
        }
        if (mBckSpeed == 0.0f || mBckPlayTimer == 0) {
            mpBckAnm.get()->mRate = 0.0f;
            mBckStopTimer = randomTimer(stopMin, stopMax);
        }
    } else if (mBckStopTimer > 0) {
        s16 remaining = (s16)(mBckStopTimer - 1);
        if (animMax == 0) {
            mBckStopTimer = 1;
            return;
        }
        mBckStopTimer = remaining;
        if (!remaining) {
            s16 timer = randomTimer(animMin, animMax);
            mBckSpeed = 1.0f;
            u32 animation = gabi::ea(mpBckAnm.get());
            mBckPlayTimer = timer;
            if (animation)
                gabi::at<StandItemBckCtorHD>(animation)->mRate = 1.0f;
        }
    }
}

/* 021180D8 */
void daStandItem_c::animTestForOneTime() {
    u32 index = (u32)mItemType * 2u;
    s16 animMin = (s16)standItemTables()->m_anim_min_time[index / 2],
        animMax = (s16)standItemTables()->m_anim_max_time[index / 2];
    s16 stopMin = (s16)standItemTables()->m_stop_min_time[index / 2],
        stopMax = (s16)standItemTables()->m_stop_max_time[index / 2];
    mbBckDidPlay = 0;
    BOOL stopped = standItemAnimationPlay(mpBckAnm.get());
    if (mBckStopTimer > 0) {
        mBckStopTimer = (s16)(mBckStopTimer - 1);
        if (mBckStopTimer == 0) {
            restartAnimation(this);
            mBckPlayTimer = randomTimer(animMin, animMax);
        }
    } else if (mBckPlayTimer > 0) {
        s16 remaining = (s16)(mBckPlayTimer - 1);
        if (remaining > 0) {
            mBckPlayTimer = remaining;
            if (stopped)
                restartAnimation(this);
        } else if (remaining == 0) {
            if (stopped) {
                mBckPlayTimer = 0;
                mBckStopTimer = randomTimer(stopMin, stopMax);
            } else {
                mbBckDidPlay = 1;
                mBckPlayTimer = 1;
                return;
            }
        } else
            mBckPlayTimer = remaining;
    }
    if (!stopped)
        mbBckDidPlay = 1;
    else if (stopMax == 0)
        restartAnimation(this);
}

/* 02117960 */
void daStandItem_c::execAction() {
    u8 carry = ((u32)actor_status >> 13) & 1;
    if (carry && !mCarry)
        mode_carry_init();
    if (mCarry && !carry)
        mode_wait_init();
    invokeMember(this, &(*gabi::at<StandItemMethodHD[3]>(0x101B3EF8))[(u32)mMode]);
    mCarry = carry;
}

/* 02116E2C */
void daStandItem_c::mode_carry_init() {
    speedF = 0.0f;
    u32 x = (u32)*gabi::at<be<u32>>(0x101FFBA8), flags = (u32)mCyl.mObjCo.mSPrm;
    standItemWord(&speed.x) = x;
    u32 y = (u32)*gabi::at<be<u32>>(0x101FFBAC);
    flags &= ~1u;
    u32 attention = (u32)attention_info.flags;
    standItemWord(&speed.y) = y;
    u32 z = (u32)*gabi::at<be<u32>>(0x101FFBB0);
    mCyl.mObjCo.mSPrm = flags;
    standItemWord(&speed.z) = z;
    mMode = MODE_CARRY;
    attention_info.flags = attention & ~0x10u;
}

/* 02117AFC */
void daStandItem_c::mode_carry() {
    if (!((u32)actor_status & 0x2000u))
        mode_drop_init();
}

/* 02116E78 */
void daStandItem_c::mode_wait_init() {
    u32 flags = (u32)mCyl.mObjCo.mSPrm;
    speed.z = 0.0f;
    speed.y = 0.0f;
    speed.x = 0.0f;
    speedF = 0.0f;
    mMode = MODE_WAIT;
    mCyl.mObjCo.mSPrm = flags | 1u;
}

/* 021187E8 */
void daStandItem_c::mode_wait() {}

/* 02117AF0 */
void daStandItem_c::mode_drop_init() { mMode = MODE_DROP; }

/* 02117B0C */
void daStandItem_c::mode_drop() {
    standItemPositionMove(this, (void *)nullptr);
    u32 play = gabi::ea(dComIfGp_get());
    standItemAcchCorrectPosition(&mAcch, gabi::at<u8>(play + 0x12A0));
    if ((u32)mAcch.m_flags & 0x20u) {
        s32 reverb = standItemReverb((s32)(s8)current.roomNo);
        standItemSoundStart(0x2870u, &eyePos, 0, reverb);
        mode_wait_init();
    }
}

/* 0211787C */
BOOL daStandItem_c::_draw() {
    u32 light = gabi::ea(dKy_getEnvlight());
    standItemSettingTevStruct(gabi::at<u8>(light), 0, &current.pos, &tevStr);
    light = gabi::ea(dKy_getEnvlight());
    standItemSetLightTevColor(gabi::at<u8>(light), mpModel.get(), &tevStr);
    u8 item = mItemNo;
    if (item == 0x92) {
        u32 model = gabi::ea(mpModel.get()),
            data = gabi::ea(gabi::at<J3DModel>(model)->mModelData.get()),
            joints = (u32)gabi::at<J3DModelData>(data)->mJointNodes;
        gabi::at<StandItemJointHD>(joints)->mBckAnm = 0;
        item = mItemNo;
    } else {
        u32 animation = gabi::ea(mpBckAnm.get());
        if (animation) {
            u32 model = gabi::ea(mpModel.get()),
                data = gabi::ea(gabi::at<J3DModel>(model)->mModelData.get());
            f32 frame = (f32)gabi::at<StandItemBckCtorHD>(animation)->mFrame;
            standItemAnimationEntry(gabi::at<u8>(animation), gabi::at<u8>(data), frame);
            item = mItemNo;
        }
    }
    if (item == 0x97)
        standItemSpecularMap(&eyePos, &tevStr, mpModel.get(), 1.0f);
    standItemModelUpdate(mpModel.get(), 0);
    u32 cloth = gabi::ea(mpCloth.get());
    if (cloth) {
        auto *packet = gabi::at<StandItemClothHD>(cloth);
        gabi::call_ptr((u32)packet->__vtbl->mDraw, packet);
    }
    return 1;
}

/* 021177A8 */
BOOL daStandItem_Create(daStandItem_c *a) { return a->_create(); }

/* 02117878 */
BOOL daStandItem_Delete(daStandItem_c *a) { return a->_delete(); }

/* 0211795C */
BOOL daStandItem_Draw(daStandItem_c *a) { return a->_draw(); }

/* 02117AEC */
BOOL daStandItem_Execute(daStandItem_c *a) { return a->_execute(); }

/* 021187E0 */
BOOL daStandItem_IsDelete(daStandItem_c *a) { return 1; }

/* 02118738 */
void staticInitialize() {
    standItemHeaderStatics()->mCounter14 = 0;
    standItemHeaderStatics()->mCounterC = 0;
    standItemHeaderStatics()->mCounter18 = 0;
    standItemHeaderStatics()->mCounter10 = 0;
    standItemInitializeLock(gabi::at<u8>(0x101B3F10));
    standItemHeaderStatics()->mMinimum = (f32)*gabi::at<be<f32>>(0x1000CE20);
    standItemHeaderStatics()->mMaximum = (f32)*gabi::at<be<f32>>(0x1000CE24);
    standItemOneByteInitialize(gabi::at<u8>(0x10462FB4));
    standItemInitializeLock(gabi::at<u8>(0x101B3F1C));
    standItemZeroByteInitialize(gabi::at<u8>(0x10462FB5));
    standItemInitializeLock(gabi::at<u8>(0x101B3F28));
}

/* 021187CC */
void globalDeletingDestructor(void *object, u32 flags) {
    if (object && (flags & 1))
        standItemDelete(object);
}

/* 021187FC */
void actorDeletingDestructor(daStandItem_c *a, u32 flags) {
    if (a) {
        standItemCircleDestroy(&a->mAcchCir.m_cir, 2);
        standItemCtor(a)->mAcchGroupVtable = STAND_ITEM_ACCH_GROUP_VTABLE;
        standItemCtor(a)->mAcchPolyVtable = STAND_ITEM_ACCH_POLY_VTABLE;
        standItemAcchDestroy(&a->mAcch, 0);
        standItemCollisionDestroy(&a->mCyl, 2);
        standItemSttsDestroy(&a->mStts, 2);
        standItemActorDestroy(a, 0);
        if (flags & 1)
            standItemDelete(a);
    }
}

/* 02118898 */
void emptyDestructor(daStandItem_c *a) {}

// clang-format off
/* WWHD glue

str 1000CE28
str 1000CE00 in 021177AC
str 1000CDEC in 021175A8
str 1000CD34 in 02116B10
str 1000CD7C
str 1000CD90
str 1000CD70
str 1000CDD0
str 1000CDD4
str 1000CDE0
ext 0200BD2C void standItemSttsConstruct(void*)
ext 0200ECD4 void standItemAddCalc(be<f32>*,f32,f32,f32,f32)
ext 0200F164 void standItemAddCalcPos(cXyz*,const cXyz*,f32,f32)
ext 0200F428 void standItemAddCalcAngle(be<s16>*,s32,s32,s32)
ext 0200FAAC void standItemDistanceAngle(s32,s32)
ext 02018034 void standItemCircleDestroy(void*,s32)
ext 02018590 void standItemCylinderConstruct(void*)
ext 0201AD78 void standItemVectorAdd(const cXyz*,cXyz*,const cXyz*)
ext 0201ADE0 void standItemVectorSubtract(const cXyz*,cXyz*,const cXyz*)
ext 024EFD9C void standItemAcchDestroy(void*,s32)
ext 024EFE94 void standItemAcchCircleConstruct(void*)
ext 024EFF44 void standItemAcchCircleSetWall(void*,f32,f32)
ext 024F0474 void standItemAcchConstruct(void*)
ext 024F06B4 void standItemAcchSet(void*,cXyz*,cXyz*,fopAc_ac_c*,s32,void*,cXyz*,void*,void*)
ext 024F08A8 void standItemAcchCorrectPosition(void*,void*)
ext 02515860 void standItemSttsDestroy(void*,s32)
ext 02515A70 void standItemCollisionDestroy(void*,s32)
ext 02515DA0 void standItemGSttsConstruct(void*)
ext 02515FB8 void standItemCollisionConstruct(void*)
ext 0251B638 void standItemClothSetMatrix(void*,const void*)
ext 025204C8 void standItemResourceDelete(void*,const char*)
ext 0252CA4C void standItemSpecularMap(const cXyz*,void*,void*,f32)
ext 025626A4 void standItemSettingTevStruct(void*,u32,const cXyz*,void*)
ext 02562F5C void standItemSetLightTevColor(void*,void*,void*)
ext 0257E34C void standItemAllWind(cXyz*,const cXyz*)
ext 025D4ED0 void standItemActorConstruct(fopAc_ac_c*)
ext 025D50BC void standItemActorDestroy(fopAc_ac_c*,s32)
ext 025D674C void standItemCullBox(fopAc_ac_c*,f32,f32,f32,f32,f32,f32)
ext 025D6870 void standItemPositionMove(fopAc_ac_c*,void*)
ext 025E1A40 void standItemSoundStart(u32,const cXyz*,u32,s32)
ext 025E2DE0 void standItemModelUpdate(void*,u32)
ext 025E742C BOOL standItemAnimationPlay(void*)
ext 025E86B8 void standItemAnimationEntry(void*,void*,f32)
ext 025F1B48 void standItemMatrixZXYRotate(void*,s32,s32,s32)
ext 025F1C28 void standItemMatrixYRotate(void*,s32)
ext 025F24E0 void standItemMatrixTranslate(f32,f32,f32)
ext 0273AA24 void standItemPanic(const char*,s32,const char*)
ext 0273AF40 void standItemDelete(void*)
ext 027DA984 void standItemFrameCtrlConstruct(void*)
ext 027F2BC0 void standItemAnimationConstruct(void*,u32)
ext 027F4D5C void standItemModelCalc(void*)
ext 028249B0 void standItemEmitterMatrix(const Mtx34*,void*,void*)
ext 028E93CC void standItemMatrixTranslation(void*,f32,f32,f32)
ext 028EAB2C void standItemZeroByteInitialize(void*)
ext 028ED6F8 void standItemOneByteInitialize(void*)
ext 028F026C void standItemInitializeLock(void*)
ext 025D63E8 BOOL standItemSolidHeap(fopAc_ac_c*,void*,u32)
ext 025E8508 BOOL standItemAnimationInit(void*,void*,void*,u32,u32,f32,s32,s32,u32)
ext 02019918 f32 standItemRandomFX(f32)
ext 02578348 f32 standItemWindPower()
ext 028E8DD0 f32 standItemSquareMagnitude(const cXyz*)
ext 028F4384 f32 standItemSquareRoot(f32)
ext 0200F93C s16 standItemTargetAngle(const cXyz*,const cXyz*)
ext 020195B0 s16 standItemAtan2s(f32,f32)
ext 02520460 s32 standItemResourceLoad(void*,const char*)
ext 02520540 s32 standItemReverb(s32)
ext 027DF9B0 s32 standItemJointIndex(void*,const char*)
ext 0257DAA8 u32 standItemWindVector()
ext 025A847C u32 standItemParticleSet(void*,u32,u32,const cXyz*,const csXyz*,void*,u32,void*,s32,const void*,const void*,u32)
ext 025E38E0 u32 standItemModelCreate(void*,u32,u32)
ext 0273AD10 u32 standItemNew(u32)
ext 027F68FC u32 standItemJointNameTable(void*)
*/
