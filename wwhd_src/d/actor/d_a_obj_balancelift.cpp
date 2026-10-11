/** Suspended balance lift: coupled weights, spring motion, and collision impulses.
 * HD: guest word copies, fused arithmetic, and callback ordering retain the verified behavior.
 */
#include "d/actor/d_a_obj_balancelift.h"
static daBalancelift_HIO_c l_HIO;
static BalanceDebugTuning sDebugTuning;
static BalanceRotationBlend sRotationBlend;
static cXyz sZero;
static BalanceQuat sIdentity;
static BalanceHeaderStatics sHeader;
static BalanceWordBlock sHeaderZero;
static BalanceDestructorRecords sDestructorRecords;
static void* sResourceManager;
static void* sOpaqueDrawBuffer;
static BalanceCylinderSource l_cyl_src;

/* inlined into daBalancelift_c::set_mtx, CreateInit, and calc_quat */
// HD: sequential word copies preserve signaling-NaN bits and overlapping aliases.
static void copyWords(void* dst, const void* src, u32 count) {
    auto* output = reinterpret_cast<be<u32>*>(dst);
    const auto* input = reinterpret_cast<const be<u32>*>(src);
    for (u32 i = 0; i < count; ++i)
        output[i] = (u32)input[i];
}
/* 0231AE94 */
s32 daObjBlift_ride_actor_check(fopAc_ac_c* actor) {

    if (!balanceIsActor(actor) || !actor)
        return 0;
    s16 type = (s16)actor->mProcName;
    if (type == BALANCE_STONE)
        return 1;
    if (type == cc_uty::fpcNm_PLAYER_e) {
        auto* player = reinterpret_cast<BalancePlayer*>(actor);
        s32 carried = player->getCarryActorID();
        if (carried == -1)
            return 1;
        gabi::Local<be<s32>> id;
        *id = carried;
        auto* child = balanceFindActor(0x025E1234, id.get());
        return child ? daObjBlift_ride_actor_check(child) + 1 : 1;
    }
    return type == BALANCE_AM2 || type == BALANCE_NPC_OS || type == BALANCE_NPC_CB1;
}
/* 0231AF88 */
void ride_call_back(void* bg, daBalancelift_c* lift, fopAc_ac_c* rider) {

    s32 added = daObjBlift_ride_actor_check(rider);
    gabi::Local<cXyz> riderPos, delta;
    riderPos->z = rider->current.pos.z;
    riderPos->y = rider->current.pos.y;
    riderPos->x = rider->current.pos.x;
    balanceSubtract(riderPos.get(), delta.get(), &lift->mPivot);
    f32 factor = (f32)l_HIO.mRideForce;
    balanceScaleVector(delta.get(), delta.get(), factor);
    balanceAddVector(&lift->mPivotVelocity, delta.get(), &lift->mPivotVelocity);
    s16 second = lift->mIsSecond;
    auto* sharedWeight = (be<s16>*)lift->mSharedWeight;
    s16 value = *sharedWeight;
    *sharedWeight = (s16)(second ? value - added : value + added);
}

/* 0231BB1C */
void daBalancelift_c::calc_quat() {

    gabi::Local<cXyz> desired, delta, force, topDirection, baseDirection;
    gabi::Local<BalanceQuat> rotation, result;
    f32 x = current.pos.x, length = mChainLength, y = current.pos.y, z = current.pos.z;
    desired->x = x;
    desired->z = z;
    desired->y = y - length;
    balanceSubtract(desired.get(), delta.get(), &mPivot);
    // HD: copy the callee-produced words without floating-point conversion.
    copyWords(force.get(), delta.get(), 3);
    f32 stiffness = (f32)l_HIO.mPivotStiffness;
    balanceScaleVector(force.get(), force.get(), stiffness);
    balanceAddVector(&mPivotVelocity, force.get(), &mPivotVelocity);
    f32 damping = (f32)l_HIO.mPivotDamping;
    balanceScaleVector(&mPivotVelocity, &mPivotVelocity, damping);
    balanceAddVector(&mPivot, &mPivotVelocity, &mPivot);
    topDirection->z = mAnchorTop.z;
    topDirection->x = mAnchorTop.x;
    topDirection->y = mAnchorTop.y;
    baseDirection->z = current.pos.z;
    baseDirection->y = current.pos.y;
    baseDirection->x = current.pos.x;
    balanceSubtractVector(topDirection.get(), &mPivot, topDirection.get());
    balanceSubtractVector(baseDirection.get(), &mPivot, baseDirection.get());
    balanceQuatRotBaseY2(rotation.get(), baseDirection.get());
    balanceQuatNormalize(rotation.get(), rotation.get());
    f32 blend = (f32)sRotationBlend.mSupport + 0.1f;
    balanceQuatSlerp(&mSupportRotation, rotation.get(), result.get(), blend);
    copyWords(&mSupportRotation, result.get(), 4);
    balanceQuatRotBaseY2(rotation.get(), topDirection.get());
    balanceQuatNormalize(rotation.get(), rotation.get());
    blend = (f32)sRotationBlend.mPlatform + 0.15f;
    balanceQuatSlerp(&mPlatformRotation, rotation.get(), result.get(), blend);
    copyWords(&mPlatformRotation, result.get(), 4);
}

/* 0231B1CC */
void daBalancelift_c::set_mtx() {

    Mtx34* matrix = mDoMtx_stack_c::get();
    f32 x = current.pos.x, z = current.pos.z, y = current.pos.y;
    balanceTranslate(matrix, x, y, z);
    balanceQuatMatrix(&mSupportRotation);
    y = mChainLength;
    balanceTranslateM(0.0f, -y, 0.0f);
    gabi::Local<cXyz> upper, origin;
    upper->z = 0.0f;
    upper->x = 0.0f;
    upper->y = 300.0f;
    balanceMultVec(matrix, upper.get(), &mAnchorTop);
    balanceMultVec(matrix, &mChainPosition, &mChainTop);
    origin->x = 0.0f;
    origin->y = 0.0f;
    origin->z = 0.0f;
    balanceMultVec(matrix, origin.get(), &mPlatformCenter);
    balanceQuatMatrix(&mPlatformRotation);
    x = (f32)l_HIO.mScaleX;
    y = (f32)l_HIO.mScaleY;
    z = (f32)l_HIO.mScaleZ;
    balanceScaleMatrix(x, y, z);
    J3DModel_setBaseTRMtx((J3DModel*)mModel, matrix);
    y = (f32)sDebugTuning.mBackgroundScaleY + 0.6f;
    balanceScaleMatrix(1.0f, y, 1.0f);
    y = (f32)sDebugTuning.mBackgroundOffsetY - 17.0f;
    balanceTranslateM(0.0f, y, 0.0f);
    balanceCopyMatrix(matrix, &mBackgroundMatrix);
    BalanceChainPacket* packet = mChainPacket;
    cXyz* vertices = packet->mpVertices;
    // HD: retain sequential word copies for aliased chain vertices.
    copyWords(&vertices[0], &mPlatformCenter, 3);
    copyWords(&vertices[1], &mChainTop, 3);
    copyWords(&vertices[2], &current.pos, 3);
}

/* 0231B904 */
void daBalancelift_c::calc_weight() {

    auto* sharedWeight = (be<s16>*)mSharedWeight;
    s16 second = mIsSecond, value = *sharedWeight;
    f32 length = (f32)l_HIO.mChainLength;
    if (!second) {
        if (value == 0) {
            auto* flags = (be<s16>*)mSharedFlags;
            *flags = (s16)((s16)*flags | 1);
            flags = (be<s16>*)mSharedFlags;
            s16 bits = *flags;
            length = gabi::fnmsubs((f32)l_HIO.mLengthRange, 0.5f, length);
            if (bits == 3) {
                sharedWeight = (be<s16>*)mSharedWeight;
                *sharedWeight = 0;
                flags = (be<s16>*)mSharedFlags;
                *flags = 0;
            }
        } else {
            if (value <= 0)
                length -= (f32)l_HIO.mLengthRange;
            auto* flags = (be<s16>*)mSharedFlags;
            *flags = (s16)((s16)*flags | 1);
            flags = (be<s16>*)mSharedFlags;
            if ((s16)*flags == 3) {
                sharedWeight = (be<s16>*)mSharedWeight;
                *sharedWeight = 0;
                flags = (be<s16>*)mSharedFlags;
                *flags = 0;
            }
        }
    } else {
        if (value == 0)
            length = gabi::fnmsubs((f32)l_HIO.mLengthRange, 0.5f, length);
        else if (value > 0)
            length -= (f32)l_HIO.mLengthRange;
        auto* flags = (be<s16>*)mSharedFlags;
        *flags = (s16)((s16)*flags | 2);
        flags = (be<s16>*)mSharedFlags;
        if ((s16)*flags == 3) {
            sharedWeight = (be<s16>*)mSharedWeight;
            *sharedWeight = 0;
            flags = (be<s16>*)mSharedFlags;
            *flags = 0;
        }
    }
    gabi::Local<cXyz> horizontal, delta;
    horizontal->y = 0.0f;
    horizontal->x = mChainPosition.x;
    horizontal->z = mChainPosition.z;
    f32 distance = balanceSquaredMagnitude(horizontal.get());
    f32 squared = gabi::fmsubs(length, length, distance);
    f32 target = balanceSqrt(squared);
    f32 old = mChainLength, velocity = mVerticalSpeed;
    velocity = gabi::fmadds(0.005f, target - old, velocity) * 0.92f;
    mVerticalSpeed = velocity;
    mChainLength = old + velocity;
    balanceSubtract(&mTargetChainPosition, delta.get(), &mChainPosition);
    f32 force = (f32)l_HIO.mChainForce;
    balanceScaleVector(delta.get(), delta.get(), force);
    balanceAddVector(&mChainVelocity, delta.get(), &mChainVelocity);
    f32 damping = (f32)l_HIO.mChainDamping;
    balanceScaleVector(&mChainVelocity, &mChainVelocity, damping);
    balanceAddVector(&mChainPosition, &mChainVelocity, &mChainPosition);
    if ((f32)mVerticalSpeed > 1.0f) {
        s8 room = (s8)current.roomNo;
        s32 reverb = balanceGetReverb(room);
        balanceStartSound(0x3024, &current.pos, 0, reverb);
    }
}

/* 0231B1C8 */
BOOL CheckCreateHeap(daBalancelift_c* lift) { return lift->CreateHeap(); }

/* 0231B05C */
BOOL daBalancelift_c::CreateHeap() {

    gabi::Local<SafeString> modelName, bgName;
    modelName->__vtbl = 0x1002542C;
    modelName->mStringTop = gabi::guest_addr("Hten1");
    void* data = balanceGetResource(sResourceManager.get(), modelName.get(), HTEN1_BDL_PLATFORM);
    if (!data)
        balanceAssert("d_a_obj_balancelift.cpp", 0x1AA, "modelData != (0)");
    mModel = balanceCreateModel(data, 0, 0x11020203);
    if (!mModel)
        balanceAssert("d_a_obj_balancelift.cpp", 0x1AC, "M_mdl != (0)");
    mChainPacket = balanceCreateChain(3, &tevStr, 2.0f);
    if (!mChainPacket)
        balanceAssert("d_a_obj_balancelift.cpp", 0x1AE, "m_chain != (0)");
    mBackground = balanceCreateBackground(0);
    if (!mBackground)
        balanceAssert("d_a_obj_balancelift.cpp", 0x1B1, "pm_bgw != (0)");
    bgName->__vtbl = 0x1002542C;
    bgName->mStringTop = gabi::guest_addr("Hten1");
    data = balanceGetResource(sResourceManager.get(), bgName.get(), HTEN1_DZB_PLATFORM);
    BalanceBackground* bg = mBackground;
    balanceSetBackground(bg, data, 1, &mBackgroundMatrix);
    bg = mBackground;
    bg->mCorrection = 0x024EE658;
    if (!mModel)
        return FALSE;
    if (!mChainPacket)
        return FALSE;
    return mBackground != nullptr;
}

/* 0231B3D0 */
s32 daBalancelift_c::CreateInit() {

    u32 pathNo = (mParameters >> 16) & 0xFF;
    be<s16>* weightSlot = &mWeight;
    be<s16>* flagsSlot = &mWeightFlags;
    if (pathNo != 0xFF) {
        s8 room = (s8)current.roomNo;
        mPath = balanceGetPath(pathNo, room);
        BalancePath* p = mPath;
        if (p && (u16)p->mCount) {
            BalancePathPoint* points = p->mpPoints;
            f32 x = (f32)points[0].mPos.x;
            current.pos.x = x;
            f32 y = (f32)points[0].mPos.y;
            p = mPath;
            current.pos.y = y;
            f32 z = (f32)points[0].mPos.z;
            home.pos.x = x;
            current.pos.z = z;
            home.pos.y = y;
            home.pos.z = z;
            if ((u16)p->mCount > 1) {
                points = p->mpPoints;
                gabi::Local<cXyz> childPos;
                childPos->x = (f32)points[1].mPos.x;
                childPos->y = (f32)points[1].mPos.y;
                s8 layer = (s8)tevStr.mRoomNo;
                u32 parent = mBsPcId;
                childPos->z = (f32)points[1].mPos.z;
                u32 id = balanceCreateChild(BALANCE_LIFT, parent, -1, childPos.get(), layer, 0, 0,
                                            -1, 0);
                parentActorID = id;
            }
        }
        mWeight = 0;
        mWeightFlags = 0;
        mIsSecond = 0;
    } else {
        u32 parent = parentActorID;
        mWeight = 0;
        mWeightFlags = 0;
        if (parent != 0xFFFFFFFF) {
            gabi::Local<be<u32>> id;
            *id = parent;
            auto* other = balanceFindLift(0x025E1234, id.get());
            if (other) {
                flagsSlot = &other->mWeightFlags;
                weightSlot = &other->mWeight;
                mIsSecond = 1;
            } else
                mIsSecond = 0;
        } else
            mIsSecond = 0;
    }
    mSharedWeight = weightSlot;
    mSharedFlags = flagsSlot;
    f32 length = (f32)l_HIO.mChainLength;
    f32 range = (f32)l_HIO.mLengthRange;
    mVerticalSpeed = 0.0f;
    mChainLength = gabi::fnmsubs(range, 0.5f, length);
    BalanceGameInfo* play = balanceGetGameInfo();
    BalanceBackground* bg = mBackground;
    balanceRegisterBackground(&play->mBgS, bg, this);
    f32 y = current.pos.y, offset = mChainLength, x = current.pos.x, z = current.pos.z;
    mPivot.x = x;
    mPivot.z = z;
    mPivot.y = y - offset;
    copyWords(&mPivotVelocity, &sZero, 3);
    copyWords(&mChainVelocity, &sZero, 3);
    copyWords(&mChainPosition, &sZero, 3);
    copyWords(&mTargetChainPosition, &sZero, 3);
    copyWords(&mSupportRotation, &sIdentity, 4);
    copyWords(&mPlatformRotation, &sIdentity, 4);
    scale.z = 2.0f;
    scale.x = 2.0f;
    scale.y = 2.0f;
    set_mtx();
    bg = mBackground;
    bg->mRideCallback = gabi::guest_fn((const void*)ride_call_back);
    balanceInitializeStatus(&mCollisionStatus, 0xC0, 0xFF, this);
    balanceSetCylinder(&mCylinder, &l_cyl_src);
    u32 flags = (u32)mCylinder.mGObjTg.mSPrm;
    mCylinder.mStts = &mCollisionStatus;
    mCylinder.mGObjTg.mSPrm = flags | 3;
    if ((s8)l_HIO.mChild < 0) {
        s32 child = balanceCreateHioChild("Vtg", &l_HIO);
        l_HIO.mChild = (u8)child;
    }
    return cPhs_COMPLEATE_e;
}

/* 0231B6CC */
s32 daBalanceliftCreate(daBalancelift_c* lift) {

    u32 flags = lift->actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (lift) {
            balanceConstructActor(lift);
            lift->__vtbl = 0x10025464;
            balanceConstructAnimation(&lift->mAnimation);
            dCcD_Stts_ct(&lift->mCollisionStatus);
            balanceConstructGObj(&lift->mCylinder);
            auto* shape = reinterpret_cast<BalanceShapeAttr*>(&lift->mCylinder.mShapeAttr);
            shape->mShapeVtable = 0x100015A8;
            shape->mAabVtable = 0x10025444;
            balanceConstructCyl(&lift->mCylinder.mCyl);
            lift->mCylinder.__vtbl_hitinf = 0x1004B108;
            shape->mShapeVtable = 0x1004B160;
            flags = lift->actor_condition;
            lift->mCylinder.mCyl.__vtbl = 0x1004B150;
        }
        lift->actor_condition = flags | fopAcCnd_INIT_e;
    }
    s32 phase = balanceLoadResource(&lift->mPhase, "Hten1");
    if (phase == cPhs_COMPLEATE_e) {
        if (!balanceEntryHeap(lift, gabi::guest_fn((const void*)CheckCreateHeap), 0xE40))
            return cPhs_ERROR_e;
        phase = lift->CreateInit();
        J3DModel* model = lift->mModel;
        lift->cullMtx = model ? gabi::ea(&model->mBaseTransformMtx) : 0;
        balanceSetCullBox(lift, -150.0f, -150.0f, -150.0f, 150.0f, 1000.0f, 150.0f);
        model = lift->mModel;
        balanceCopyMatrix(model ? &model->mBaseTransformMtx : nullptr, &lift->mBackgroundMatrix);
    }
    return phase;
}
/* 0231B86C */
BOOL daBalanceliftDelete(daBalancelift_c* lift) {

    BalanceGameInfo* play = balanceGetGameInfo();
    balanceReleaseBackground(&play->mBgS, lift->mBackground.get());
    balanceDeleteResource(&lift->mPhase, "Hten1");
    BalanceChainPacket* packet = lift->mChainPacket;
    if (packet)
        packet->destroy(3);
    lift->mChainPacket = nullptr;
    s8 child = (s8)l_HIO.mChild;
    if (child >= 0) {
        balanceDeleteHioChild(child);
        l_HIO.mChild = 0xFF;
    }
    return TRUE;
}

/* 0231BCE4 */
BOOL daBalanceliftExecute(daBalancelift_c* lift) {

    lift->calc_weight();
    lift->calc_quat();
    lift->set_mtx();
    balanceMoveBackground(lift->mBackground.get());
    balanceMoveStatus(&lift->mCollisionStatus.__vtbl_gstts);
    if (balanceCheckTgHit(&lift->mCylinder)) {
        auto* attacker = balanceGetHitActor(&lift->mCylinder.mGObjTg);
        auto* hit = balanceGetHitObject(&lift->mCylinder);
        s32 actor = balanceIsActor(attacker);
        gabi::Local<cXyz> direction, offset, impulse, distanceVector;
        if (actor && attacker && (s16)attacker->mProfName == cc_uty::fpcNm_BOMB_e) {
            offset->x = lift->mPlatformCenter.x;
            offset->z = lift->mPlatformCenter.z;
            offset->y = (f32)lift->mPlatformCenter.y - 50.0f;
            balanceSubtract(offset.get(), direction.get(), &attacker->current.pos);
            if (!balanceNormalizeVector(direction.get())) {
                direction->z = 1.0f;
                direction->y = 0.0f;
                direction->x = 0.0f;
            }
            balanceScaleTo(direction.get(), impulse.get(), -20.0f);
            balanceAddVector(&lift->mPivotVelocity, impulse.get(), &lift->mPivotVelocity);
            f32 dy = direction->y;
            lift->mVerticalSpeed = (f32)lift->mVerticalSpeed - (dy + dy);
            balanceSetCylCenter(&lift->mCylinder.mCyl, &lift->mPlatformCenter);
            BalanceGameInfo* play = balanceGetGameInfo();
            balanceRegisterCylinder(&play->mCcS, &lift->mCylinder);
            return 0;
        }
        if (hit) {
            direction->y = (f32)lift->mCylinder.mGObjTg.mRVec.y;
            direction->x = (f32)lift->mCylinder.mGObjTg.mRVec.x;
            direction->z = (f32)lift->mCylinder.mGObjTg.mRVec.z;
            if (!balanceNormalizeVector(direction.get())) {
                direction->z = 1.0f;
                direction->y = 0.0f;
                direction->x = 0.0f;
            }
            u32 flags = (u32)hit->mAtType;
            if (flags & cc_uty::AT_TYPE_WIND) {
                balanceScaleTo(direction.get(), impulse.get(), -12.0f);
                balanceAddVector(&lift->mPivotVelocity, impulse.get(), &lift->mPivotVelocity);
                f32 dy = direction->y;
                lift->mVerticalSpeed = (f32)lift->mVerticalSpeed - (dy + dy);
                balanceSetCylCenter(&lift->mCylinder.mCyl, &lift->mPlatformCenter);
                BalanceGameInfo* play = balanceGetGameInfo();
                balanceRegisterCylinder(&play->mCcS, &lift->mCylinder);
                return 0;
            }
            if (flags & BALANCE_AT_SWORD) {
                balanceScaleTo(direction.get(), impulse.get(), -8.0f);
                balanceAddVector(&lift->mPivotVelocity, impulse.get(), &lift->mPivotVelocity);
                f32 dy = direction->y;
                lift->mVerticalSpeed = (f32)lift->mVerticalSpeed - (dy + dy);
                auto* impact = &lift->mCylinder.mGObjTg.mHitPos;
                if (impact) {
                    balanceSubtract(impact, distanceVector.get(), &lift->mPlatformCenter);
                    f32 square = balanceSquaredMagnitude(distanceVector.get());
                    f32 distance = balanceSqrt(square);
                    f32 force = (f32)l_HIO.mImpactForce;
                    balanceScaleTo(direction.get(), impulse.get(), force);
                    balanceAddVector(&lift->mChainVelocity, impulse.get(), &lift->mChainVelocity);
                    lift->mTargetChainPosition.y = distance;
                    lift->mChainPosition.y = distance;
                    BalanceGameInfo* play = balanceGetGameInfo();
                    auto* particles = play->mpParticles.get();
                    balanceSpawnParticle(particles, 0, 0xC, impact, 0, 0, 0xFF, 0, -1, 0, 0, 0);
                }
                s8 room = (s8)lift->current.roomNo;
                s32 reverb = balanceGetReverb(room);
                balanceStartSound(0x6817, &lift->eyePos, 0, reverb);
            }
        }
    }
    balanceSetCylCenter(&lift->mCylinder.mCyl, &lift->mPlatformCenter);
    BalanceGameInfo* play = balanceGetGameInfo();
    balanceRegisterCylinder(&play->mCcS, &lift->mCylinder);
    return 0;
}
/* 0231C03C */
BOOL daBalanceliftDraw(daBalancelift_c* lift) {

    void* light = balanceGetEnvironment();
    balanceSetLight(light, 0, &lift->current.pos, &lift->tevStr);
    light = balanceGetEnvironment();
    balanceApplyLight(light, (J3DModel*)lift->mModel, &lift->tevStr);
    balanceUpdateModel((J3DModel*)lift->mModel, 0);
    void* buffer = sOpaqueDrawBuffer;
    BalanceChainPacket* packet = lift->mChainPacket;
    balanceEntryChain(buffer, packet, 0);
    balanceDrawChain(lift->mChainPacket.get());
    return TRUE;
}
/* 0231C228 */
BOOL daBalanceliftIsDelete(daBalancelift_c* lift) { return TRUE; }
// HD: compiler-generated initialization and deleting destructors.
/* 0231C0B4 */
daBalancelift_HIO_c* hioConstructor(daBalancelift_HIO_c* p) {

    if (!p)
        p = balanceAllocateHio(0x38);
    if (p) {

        p->mChild = 0xFF;
        p->mRotationMax = 45.0f;
        p->mRideForce = 0.005f;
        p->mChainLength = 1800.0f;
        p->mPivotDamping = 0.9f;
        p->mChainForce = 0.9f;
        p->mScaleZ = 2.0f;
        p->mImpactForce = 25.0f;
        p->__vtbl = 0x10025454;
        p->mPivotStiffness = 0.05f;
        p->mLengthRange = 440.0f;
        p->mChainDamping = 0.65f;
        p->mScaleX = 2.0f;
        p->mScaleY = 2.0f;
    }
    return p;
}
/* 0231C174 */
void staticInitialize() {

    sHeaderZero.mWords[2] = 0;
    sHeaderZero.mWords[0] = 0;
    sHeaderZero.mWords[3] = 0;
    sHeaderZero.mWords[1] = 0;
    balanceRegisterGlobal(&sDestructorRecords.mRecords[0]);
    sHeader.mMinAngle = -3.1415927410125732f;
    sHeader.mMaxAngle = 3.1415927410125732f;
    balanceInitializeHeaderByte(&sHeader.mHeaderByte);
    balanceRegisterGlobal(&sDestructorRecords.mRecords[1]);
    balanceInitializeHeaderFlag(&sHeader.mHeaderFlag);
    balanceRegisterGlobal(&sDestructorRecords.mRecords[2]);
    hioConstructor(&l_HIO);
}
/* 0231C214 */
void hioDestructor(void* p, s32 flags) {

    if (p && (flags & 1))
        balanceDeleteObject(p);
}
/* 0231C230 */
void actorDestructor(daBalancelift_c* lift, s32 flags) {

    if (lift) {
        balanceDestroyCylinder(&lift->mCylinder, 2);
        balanceDestroyStatus(&lift->mCollisionStatus, 2);
        balanceDestroyAnimation(&lift->mAnimation, 2);
        balanceDestroyActor(lift, 0);
        if (flags & 1)
            balanceDeleteObject(lift);
    }
}
/* 0231C2A8 */
void emptyVirtual() {}

// clang-format off
/* WWHD glue
data l_HIO 10469198
data sDebugTuning 1047BBB0
data sRotationBlend 1047BC0C
data sZero 101FFBA8
data sIdentity 101E9C38
data sHeader 1046918C
data sHeaderZero 104691D0
data sDestructorRecords 101C7E38
data sResourceManager 101F4F28
data sOpaqueDrawBuffer 104B4634
data l_cyl_src 101C7DF4
ext 025D4604 s32 balanceIsActor(fopAc_ac_c*)
ext 025D5218 fopAc_ac_c* balanceFindActor(u32, be<s32>*)
ext 0201ADE0 void balanceSubtract(const cXyz*, cXyz*, const cXyz*)
ext 028E8E64 void balanceScaleVector(const cXyz*, cXyz*, f32)
ext 028E8D88 void balanceAddVector(const cXyz*, const cXyz*, cXyz*)
ext 026066C4 void* balanceGetResource(void*, SafeString*, s32)
ext 0273AA24 void balanceAssert(const char*, s32, const char*)
ext 025E38E0 J3DModel* balanceCreateModel(void*, u32, u32)
ext 0251A2CC BalanceChainPacket* balanceCreateChain(s32, dKy_tevstr_c*, f32)
ext 024F23F4 BalanceBackground* balanceCreateBackground(u32)
ext 0200A030 void balanceSetBackground(BalanceBackground*, void*, u32, Mtx34*)
ext 028E93CC void balanceTranslate(Mtx34*, f32, f32, f32)
ext 025F25CC void balanceQuatMatrix(BalanceQuat*)
ext 025F24E0 void balanceTranslateM(f32, f32, f32)
ext 028E8F64 void balanceMultVec(Mtx34*, cXyz*, cXyz*)
ext 025F2518 void balanceScaleMatrix(f32, f32, f32)
ext 028E90D4 void balanceCopyMatrix(Mtx34*, Mtx34*)
ext 025AAF88 BalancePath* balanceGetPath(u32, s8)
ext 025D5A20 u32 balanceCreateChild(s16, u32, s32, cXyz*, s8, u32, u32, s32, u32)
ext 024EEA6C void balanceRegisterBackground(void*, BalanceBackground*, daBalancelift_c*)
ext 02515F14 void balanceInitializeStatus(dCcD_Stts*, s32, s32, daBalancelift_c*)
ext 02516518 void balanceSetCylinder(dCcD_Cyl*, void*)
ext 025F0A10 s32 balanceCreateHioChild(const char*, daBalancelift_HIO_c*)
ext 025D4ED0 void balanceConstructActor(daBalancelift_c*)
ext 025E9960 void balanceConstructAnimation(BalanceAnimation*)
ext 02515FB8 void balanceConstructGObj(dCcD_Cyl*)
ext 02018590 void balanceConstructCyl(cM3dGCyl*)
ext 02520460 s32 balanceLoadResource(request_of_phase_process_class*, const char*)
ext 025D63E8 s32 balanceEntryHeap(daBalancelift_c*, u32, u32)
ext 025D674C void balanceSetCullBox(daBalancelift_c*, f32, f32, f32, f32, f32, f32)
ext 020087EC void balanceReleaseBackground(void*, BalanceBackground*)
ext 025204C8 void balanceDeleteResource(request_of_phase_process_class*, const char*)
ext 025F0A18 void balanceDeleteHioChild(s32)
ext 028E8DD0 f32 balanceSquaredMagnitude(cXyz*)
ext 028F4384 f32 balanceSqrt(f32)
ext 02520540 s32 balanceGetReverb(s32)
ext 025E1A40 void balanceStartSound(u32, cXyz*, u32, s32)
ext 028E8DAC void balanceSubtractVector(const cXyz*, const cXyz*, cXyz*)
ext 02312548 void balanceQuatRotBaseY2(BalanceQuat*, cXyz*)
ext 028E8BA4 void balanceQuatNormalize(BalanceQuat*, BalanceQuat*)
ext 028E9BC0 void balanceQuatSlerp(BalanceQuat*, BalanceQuat*, BalanceQuat*, f32)
ext 024F43DC void balanceMoveBackground(BalanceBackground*)
ext 02515E50 void balanceMoveStatus(be<u32>*)
ext 025162A4 s32 balanceCheckTgHit(dCcD_Cyl*)
ext 02515BBC fopAc_ac_c* balanceGetHitActor(dCcD_GObjTg*)
ext 02516300 dCcD_GObjInf* balanceGetHitObject(dCcD_Cyl*)
ext 0201B47C s32 balanceNormalizeVector(cXyz*)
ext 0201AE48 void balanceScaleTo(cXyz*, cXyz*, f32)
ext 020182E0 void balanceSetCylCenter(cM3dGCyl*, cXyz*)
ext 025200D4 BalanceGameInfo* balanceGetGameInfo()
ext 0200E240 void balanceRegisterCylinder(void*, dCcD_Cyl*)
ext 025A847C void balanceSpawnParticle(void*, u32, u32, cXyz*, u32, u32, u32, u32, s32, u32, u32, u32)
ext 02555D0C void* balanceGetEnvironment()
ext 025626A4 void balanceSetLight(void*, s32, cXyz*, dKy_tevstr_c*)
ext 02562F5C void balanceApplyLight(void*, J3DModel*, dKy_tevstr_c*)
ext 025E2DE0 void balanceUpdateModel(J3DModel*, u32)
ext 027F0E04 void balanceEntryChain(void*, BalanceChainPacket*, u32)
ext 0251AE90 void balanceDrawChain(BalanceChainPacket*)
ext 0273AD10 daBalancelift_HIO_c* balanceAllocateHio(u32)
ext 028F026C void balanceRegisterGlobal(void*)
ext 028ED6F8 void balanceInitializeHeaderByte(void*)
ext 028EAB2C void balanceInitializeHeaderFlag(void*)
ext 0273AF40 void balanceDeleteObject(void*)
ext 02515A70 void balanceDestroyCylinder(dCcD_Cyl*, s32)
ext 02515860 void balanceDestroyStatus(dCcD_Stts*, s32)
ext 025E99E0 void balanceDestroyAnimation(BalanceAnimation*, s32)
ext 025D50BC void balanceDestroyActor(daBalancelift_c*, s32)
ext 025D5218 daBalancelift_c* balanceFindLift(u32, be<u32>*)
str 10025478
str 10025490
str 100254A0
str 100254B4
str 100254C4
str 100254EC
str 10025548
*/
