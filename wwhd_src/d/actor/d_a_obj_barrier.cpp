/** Hyrule electric barrier. */
// HD: Expanded animation containers and a texture-object replacement path.
// Preserve the render-list changes, collision order and specialized BCK lifetime.
#include "d/actor/d_a_obj_barrier.h"
#include "gc_api/tu.h"
const gc_tu_data gc_tu = {.safeString = 0x1002602C};
void* barrierResource(void*, BarrierArchive*, s32);
BarrierGameInfo* barrierGameInfo();
void barrierAssert(const char*, s32, const char*);
void* barrierAllocate(u32);
void barrierConstructFrame(void*, s32);
void barrierConstructBinding(void*);
void barrierDestroyArray(void*, u32, u32, void*, u32, u32);
void barrierDestroyCylinder(BarrierCylinder*, u32);
void barrierDestroyStatus(dCcD_Stts*, u32);
void barrierDestroyActor(void*, u32);
void barrierDelete(void*);
void barrierRegisterDestructor(void*);
void barrierInitRandom(void*);
void barrierInitRandomByte(void*);
void barrierReleaseBg(dBgS*, dBgW*);
void barrierConstructStatus(dCcD_Stts*);
void barrierSetCollider(void*, BarrierCylinder*);
void barrierCylinderCenter(cM3dGCyl*, cXyz*);
void barrierCylinderHeight(cM3dGCyl*, f32);
void barrierCylinderRadius(cM3dGCyl*, f32);
void barrierConstructCylinder(cM3dGCyl*);
u32 barrierAtan2(f32, f32);
void barrierConstructAngle(csXyz*, s32, s32, s32);
void barrierSubtract(cXyz*, cXyz*, cXyz*);
s32 barrierRegisterBg(dBgS*, dBgW*, daObjBarrier_c*);
dBgW* barrierNewBackground(void*, u32, Mtx34*);
void barrierMoveBg(dBgW*);
fopAc_ac_c* barrierHitActor(void*);
void barrierConstructGlobalStatus(void*);
void barrierMoveStatus(void*);
void barrierInitStatus(dCcD_Stts*, s32, s32, daObjBarrier_c*);
void barrierConstructCollider(BarrierCylinder*);
void barrierClearAt(BarrierCylinder*);
s32 barrierCheckAt(BarrierCylinder*);
void barrierClearTg(BarrierCylinder*);
s32 barrierCheckTg(BarrierCylinder*);
u32 barrierTgHitObject(BarrierCylinder*);
void barrierSetCylinder(BarrierCylinder*, void*);
s32 barrierLoad(request_of_phase_process_class*, const char*);
void barrierUnload(request_of_phase_process_class*, const char*);
s32 barrierEventIndex(void*, const char*, u32);
s32 barrierEventEnd(void*, s32);
void barrierParticle(void*, u32, u32, cXyz*, csXyz*, u32, u32, u32, s32, u32, u32, u32);
void barrierSetEvent(void*, u32);
s32 barrierCheckEvent(void*, u32);
void barrierConstructActor(daObjBarrier_c*);
void barrierDeleteActor(daObjBarrier_c*);
s32 barrierEnterHeap(daObjBarrier_c*, void*, u32);
void barrierOrderEvent(daObjBarrier_c*, s32, u32, u32, u32, u32);
void barrierUpdateModel(J3DModel*, u32);
J3DModel* barrierNewModel(void*, u32, u32);
void barrierConstructBtk(mDoExt_btkAnm*);
s32 barrierInitBtk(mDoExt_btkAnm*, void*, void*, u32, s32, f32, s32, s32, u32, u32);
void barrierEntryBtk(mDoExt_btkAnm*, J3DModelData*, f32);
void barrierConstructBrk(BarrierBrkAnm*);
s32 barrierInitBrk(BarrierBrkAnm*, void*, void*, u32, s32, f32, s32, s32, u32, u32);
void barrierEntryBrk(BarrierBrkAnm*, J3DModelData*, f32);
s32 barrierInitBck(mDoExt_bckAnm*, void*, void*, u32, s32, f32, s32, s32, u32);
void barrierEntryBck(mDoExt_bckAnm*, J3DModelData*, f32);
void barrierChangeScene(u32, u32);
void barrierRotateZXY(Mtx34*, s32, s32, s32);
be<u8>* barrierTextureName(JUTNameTab*, u32);
BarrierTextureObject* barrierTextureCtor(void*, void*);
void barrierMatrixDtor(void*, u32);
void* barrierTextureHandle(void*, u32);
f64 barrierSquareMag(cXyz*);
void barrierCopyMtx(Mtx34*, Mtx34*);
void barrierConstructArray(void*, u32, u32, void*);
f64 barrierSqrt(f64);
void* barrier_bckConstructor(void*);
void barrier_bckDestructor(void*, u32);
BOOL barrier_heapCallback(daObjBarrier_c*);
u32 barrier_parameter(daObjBarrier_c*, u32, u32);
static u8 sActorVtable;
static u8 sStatusVtable;
static u8 sGlobalStatusVtable;
static u8 sInitialShapeVtable;
static u8 sCylinderAttrVtable;
static u8 sHitInfoVtable;
static u8 sShapeVtable;
static u8 sCylinderVtable;
static u8 sInitialMatrixVtable;
static u8 sBindingVtable;
static u8 sMatrixVtable;
static void* sResourceControl;
static void* sTextureControl;
static BarrierSave* sSave;
static u32 sDrawList;
static u32 sXluDrawList;
static BarrierHeaderStatics sHeader;
static BarrierPiLimits sPiLimits;
static BarrierTrigTable sTrig;
static BarrierDummyName sDummyName;
static u8 sPiRegistration;
static u8 sRandomRegistration;
static u8 sRandomByteRegistration;
static u8 sAttackSource;
static u8 sTargetSource;
static u8 sBtkConstructor;
static u8 sBrkConstructor;
static void* resource(s32 index) {
    gabi::Local<BarrierArchive> arc;
    arc->mpName = gabi::at<char>(gabi::guest_addr("Ycage"));
    arc->mVtable = gc_tu.safeString;
    return barrierResource(sResourceControl, arc.get(), index);
}
static BarrierGameInfo* play() { return barrierGameInfo(); }
static BarrierPlayer* player() { return play()->mpPlayer; }
static void assertion(const char* file, s32 line, const char* expression) {
    barrierAssert(file, line, expression);
}
static void setInvisibleList() {
    sDrawList = play()->mInvisibleDrawList;
    sXluDrawList = play()->mInvisibleXluDrawList;
}
static void setNormalList() {
    sDrawList = play()->mDrawList;
    sXluDrawList = play()->mXluDrawList;
}
static void copyVectorWord(be<f32>* dst, const be<f32>* src) {
    gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(gabi::ea(src)));
}
namespace {
u32 shiftLeft(u32 value, u32 shift) { return (shift & 32) ? 0 : value << (shift & 31); }
u32 shiftRight(u32 value, u32 shift) { return (shift & 32) ? 0 : value >> (shift & 31); }
} // namespace

/* 02322A54 */
void daObjBarrier_c::init_mtx() {
    J3DModel* model = mAnm.mpModel;
    J3DModel_setBaseScale(model, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    J3DModel_setBaseTRMtx(mAnm.mpModel.get(), mDoMtx_stack_c::get());
    mDoMtx_stack_c::scaleM(scale.x, scale.y, scale.z);
    barrierCopyMtx(mDoMtx_stack_c::get(), &mBgMtx);
}
/* 02322A50 */
BOOL barrier_heapCallback(daObjBarrier_c* barrier) { return barrier->create_heap(); }
/* 02322390 */
BOOL daObjBarrier_anm_c::init() {
    void* data = resource(dRes_INDEX_YCAGE_BDL_YCAGE00_e);
    void* btk = resource(dRes_INDEX_YCAGE_BTK_YCAGE00_e);
    void* brk = resource(dRes_INDEX_YCAGE_BRK_YCAGE00_e);
    BOOL ok = 1;
    if (!data || !btk || !brk) {
        assertion("d_a_obj_barrier.cpp", 411, "0");
        ok = 0;
    } else {
        mpModel = barrierNewModel(data, 0x80000, 0x01000200);
        s32 btkOK = barrierInitBtk(&mBtk, data, btk, 1, 2, 1.f, 0, -1, 0, 0);
        s32 brkOK = barrierInitBrk(&mBrk, data, brk, 1, 2, 1.f, 0, -1, 0, 0);
        if (!mpModel || !btkOK || !brkOK)
            ok = 0;
    }
    return ok;
}
/* 023229A4 */
BOOL daObjBarrier_c::create_heap() {
    s32 anm = mAnm.init(), effect = mEffect.init();
    BOOL ok = 1;
    if (!anm || !effect)
        ok = 0;
    else {
        void* dzb = resource(dRes_INDEX_YCAGE_DZB_KEKKAI_e);
        mpBgW = barrierNewBackground(dzb, 1, &mBgMtx);
        if (!mpBgW)
            ok = 0;
    }
    return ok;
}
/* 023235B4 */
BOOL daObjBarrier_c::checkCollision_Tg() {
    if (barrierCheckTg(&mTgCyl)) {
        fopAc_ac_c* actor = barrierHitActor(&mTgCyl.mGObjTg);
        if (actor) {
            f32 z = actor->current.pos.z, y = actor->current.pos.y, x = actor->current.pos.x;
            s32 target = 1;
            if (actor == player()) {
                target = 0;
                if (barrierTgHitObject(&mTgCyl)) {
                    y = mTgCyl.mGObjTg.mHitPos.y;
                    x = mTgCyl.mGObjTg.mHitPos.x;
                    z = mTgCyl.mGObjTg.mHitPos.z;
                }
            }
            gabi::Local<cXyz> center, hit;
            center->x = mTgCyl.mCyl.mCenter.x;
            center->y = mTgCyl.mCyl.mCenter.y;
            center->z = mTgCyl.mCyl.mCenter.z;
            f32 radius = 1000.f * scale.x;
            hit->x = x;
            hit->y = y;
            hit->z = z;
            mEffect.birth(actor, center.get(), hit.get(), target, radius);
        }
        barrierClearTg(&mTgCyl);
    }
    return 0;
}
/* 023234F0 */
void daObjBarrier_c::checkCollision_At() {
    if (barrierCheckAt(&mAtCyl)) {
        fopAc_ac_c* actor = barrierHitActor(&mAtCyl.mGObjAt);
        BarrierPlayer* link = player();
        if (actor && actor == link) {
            gabi::Local<cXyz> center, hit;
            center->x = mAtCyl.mCyl.mCenter.x;
            center->y = mAtCyl.mCyl.mCenter.y;
            center->z = mAtCyl.mCyl.mCenter.z;
            f32 radius = 1000.f * scale.x;
            hit->x = actor->current.pos.x;
            hit->y = actor->current.pos.y;
            hit->z = actor->current.pos.z;
            mEffect.birth(actor, center.get(), hit.get(), 1, radius);
        }
        barrierClearAt(&mAtCyl);
    }
}
/* 023236E8 */
void daObjBarrier_c::registCollisionTable() {
    gabi::Local<cXyz> center, delta;
    center->x = current.pos.x;
    center->y = current.pos.y - 300.f;
    center->z = current.pos.z;
    f32 height = gabi::fmadds(10000.f, scale.y, 300.f), radius = 1000.f * scale.x;
    barrierCylinderCenter(&mAtCyl.mCyl, center.get());
    barrierCylinderRadius(&mAtCyl.mCyl, radius - 60.f);
    barrierCylinderHeight(&mAtCyl.mCyl, height);
    barrierCylinderCenter(&mTgCyl.mCyl, center.get());
    barrierCylinderRadius(&mTgCyl.mCyl, radius - 20.f);
    barrierCylinderHeight(&mTgCyl.mCyl, height);
    barrierSubtract(&current.pos, delta.get(), &player()->current.pos);
    copyVectorWord(&mAtCyl.mGObjAt.mVec.x, &delta->x);
    copyVectorWord(&mAtCyl.mGObjAt.mVec.z, &delta->z);
    copyVectorWord(&mAtCyl.mGObjAt.mVec.y, &delta->y);
    barrierSetCollider(play()->mCollision, &mAtCyl);
    barrierSetCollider(play()->mCollision, &mTgCyl);
}

/* 02322F14 */
void daObjBarrier_c::brkAnmPlay() {
    gabi::Local<cXyz> delta, horizontal;
    barrierSubtract(&player()->current.pos, delta.get(), &current.pos);
    horizontal->x = delta->x;
    horizontal->y = 0.f;
    horizontal->z = delta->z;
    f64 distance = barrierSqrt(barrierSquareMag(horizontal.get()));
    f32 radius = gabi::fmsubs(1000.f, scale.x, 150.f);
    f32 depth = distance > radius ? 0.f : f32(radius - distance);
    mAnm.mBrkFrame = depth > 700.f ? 1.f : gabi::fmadds(1.f - depth / 700.f, 59.f, 1.f);
}
/* 0232384C */
void daObjBarrier_c::break_start_wait_proc() {
    if (barrierCheckEvent(sSave->mEvent, 0x3980) != 1)
        return;
    BarrierPlayer* link = player();
    gabi::Local<cXyz> delta, horizontal;
    barrierSubtract(&link->current.pos, delta.get(), &current.pos);
    horizontal->x = delta->x;
    horizontal->y = 0.f;
    horizontal->z = delta->z;
    f64 distance = barrierSqrt(barrierSquareMag(horizontal.get()));
    if (distance < 8800.f)
        return;
    if (sSave->mEquippedSword != 0x3E)
        return;
    if (!play()->mSwordActive)
        return;
    u8 cut = link->mCutType;
    if (cut < 1 || cut > 10)
        return;
    barrierSetEvent(sSave->mEvent, 0x2C02);
    mEventID = barrierEventIndex(play()->mEventManager, "seal", 255);
    mBarrierProc = PROC_BREAK_ORDER_e;
}
/* 02323958 */
void daObjBarrier_c::break_order_proc() {
    if (eventInfo.mCommand == 2) {
        mBarrierProc = PROC_BREAK_END_WAIT_e;
        return;
    }
    barrierOrderEvent(this, (s32)(s16)mEventID, 255, 65535, 0, 1);
    eventInfo.mCondition = (u16)eventInfo.mCondition | 2;
}
/* 023239D4 */
void daObjBarrier_c::break_end_wait_proc() {
    s32 event = (s16)mEventID;
    if (barrierEventEnd(play()->mEventManager, event)) {
        BarrierGameInfo* game = play();
        game->mEvent.mEventFlag = (u16)game->mEvent.mEventFlag | 8;
        barrierDeleteActor(this);
        barrierChangeScene(26, 0);
    }
}
/* 02323A44 */
BOOL daObjBarrier_c::break_check() {
    if (mMoya)
        return 0;
    switch ((u32)mBarrierProc) {
    case PROC_BREAK_START_WAIT_e:
        this->break_start_wait_proc();
        return 0;
    case PROC_BREAK_ORDER_e:
        this->break_order_proc();
        return 1;
    case PROC_BREAK_END_WAIT_e:
        this->break_end_wait_proc();
        return 1;
    default:
        return 0;
    }
}
/* 02322518 */
void daObjBarrier_ef_c::setDummyTexture(u32 index) {
    gabi::Local<BarrierTextureScratch> scratch;
    BarrierImageWords* image = &scratch->mImage;
    auto* data = reinterpret_cast<BarrierModelData*>(mpModel[index]->mModelData.get());
    BarrierTexture* texture = data->mpTexture;
    JUTNameTab* names = data->mpTextureNames;
    if (!texture)
        assertion("d_a_obj_barrier.cpp", 821, "texture != (0)");
    if (!names)
        assertion("d_a_obj_barrier.cpp", 822, "textureName != (0)");
    for (u16 i = 0; i < texture->mNum; i++) {
        be<u8>* name = barrierTextureName( names, (u32)i);
        if (!name) {
            assertion("d_a_obj_barrier.cpp", 827, "name != (0)");
            continue;
        }
        u32 j = 0;
        bool match;
        do {
            u8 a = name[j], b = sDummyName.mBytes[j];
            match = a == b;
            if (!match || !a)
                break;
            j++;
        } while (true);
        if (!match)
            continue;
        void* handle = barrierTextureHandle(sTextureControl, 6);
        BarrierTextureObject* object =
            barrierTextureCtor( barrierAllocate(0xC0), handle);
        image->mImage.mpTexture = object;
        image->mImage.mWidth = object->mWidth;
        image->mImage.mIndexTexture = 0;
        image->mImage.mHeight = object->mHeight;
        BarrierImageWords* dest = &texture->mpImages[i];
        for (u32 k = 0; k < 9; k++)
            dest->mWords[k] = image->mWords[k];
        dest = &texture->mpImages[i];
        dest->mImage.mImageOffset =
            (u32)dest->mImage.mImageOffset + gabi::ea(image) - gabi::ea(dest);
        dest = &texture->mpImages[i];
        dest->mImage.mPaletteOffset =
            (u32)dest->mImage.mPaletteOffset + gabi::ea(image) - gabi::ea(dest);
        dest = &texture->mpImages[i];
        dest->mImage.mpTexture = image->mImage.mpTexture.get();
        u32 hi = texture->mMaskHi, lo = texture->mMaskLo, shift = i < 64 ? i : (u32)i - 64;
        u32 shiftedHi =
                shiftLeft(hi, shift) | shiftRight(lo, 32 - shift) | shiftLeft(lo, shift + 32),
            shiftedLo = shiftLeft(lo, shift);
        if (i < 64) {
            u32 oldLo = texture->mUsedLo, oldHi = texture->mUsedHi;
            texture->mUsedHi = oldHi | shiftedHi;
            texture->mUsedLo = oldLo | shiftedLo;
        } else {
            u32 oldHi = texture->mUsedHiExtra, oldLo = texture->mUsedLoExtra;
            texture->mUsedLoExtra = oldLo | shiftedLo;
            texture->mUsedHiExtra = oldHi | shiftedHi;
        }
    }
}

/* 02323000 */
BOOL daObjBarrier_ef_c::checkHitActor(void* actor) {
    for (u32 i = 0; i < 4; i++)
        if (mHitActor[i].get() == actor)
            return 1;
    return 0;
}
/* 0232302C */
void daObjBarrier_ef_c::birth(void* actor, cXyz* center, cXyz* hit, s32 check, f32 radius) {
    if (check && this->checkHitActor(actor))
        return;
    gabi::Local<csXyz> angle;
    gabi::Local<cXyz> position;
    // Real callees write the caller linkage word at SP+4. Keep it outside locals.
    gabi::Local<BarrierArchive> outgoingLinkage;
    barrierConstructAngle(angle.get(), 0, 0, 0);
    u32 yaw = barrierAtan2((f32)center->x - (f32)hit->x, (f32)center->z - (f32)hit->z);
    angle->y = yaw;
    u32 trig = (yaw & 0xFFFF) >> 3;
    position->z = gabi::fnmsubs(radius, sTrig.mEntries[trig].mCos, center->z);
    position->x = gabi::fnmsubs(radius, sTrig.mEntries[trig].mSin, center->x);
    u32 flags = mActiveEffFlags;
    position->y = hit->y;
    s32 chosen = -1;
    for (u32 i = 0; i < 4; i++)
        if (!(flags & (1u << i))) {
            chosen = i;
            break;
        }
    if (chosen == -1) {
        f32 maximum = -1.f;
        for (u32 i = 0; i < 4; i++)
            if (mBtk[i].mFrameCtrl.mFrame > maximum) {
                maximum = mBtk[i].mFrameCtrl.mFrame;
                chosen = i;
            }
    }
    for (u32 i = 0; i < 4; i++)
        if (flags & (1u << i)) {
            if (i == (u32)chosen)
                break;
            mBtk[i].mFrameCtrl.mRate =
                ((f32)(s16)mBtk[i].mFrameCtrl.mEnd - (f32)mBtk[i].mFrameCtrl.mFrame) / 3.f;
            break;
        }
    if (chosen == -1)
        assertion("d_a_obj_barrier.cpp", 952, "idx != -1");
    u32 index = (u32)chosen;
    u32 bit = (index & 32) ? 0 : 1u << (index & 31);
    mActiveEffFlags = (u32)mActiveEffFlags | bit;
    mPos[index].x = position->x;
    mPos[index].y = position->y;
    mPos[index].z = position->z;
    mAngle[index] = angle->y;
    mHitActor[index] = actor;
    J3DModelData* data = mpModel[index]->mModelData;
    void* btk = resource(dRes_INDEX_YCAGE_BTK_YHRBR00_e);
    if (!btk)
        assertion("d_a_obj_barrier.cpp", 969, "btk_anm_p != (0)");
    void* bck = resource(dRes_INDEX_YCAGE_BCK_YHRBR00_e);
    if (!bck)
        assertion("d_a_obj_barrier.cpp", 974, "bck_anm_p != (0)");
    void* brk = resource(dRes_INDEX_YCAGE_BRK_YHRBR00_e);
    if (!brk)
        assertion("d_a_obj_barrier.cpp", 979, "brk_anm_p != (0)");
    barrierInitBtk(&mBtk[index], data, btk, 1, 0, 1.f, 0, -1, 1, 0);
    barrierInitBck(&mBck[index], data, bck, 1, 0, 0.f, 0, -1, 1);
    barrierInitBrk(&mBrk[index], data, brk, 1, 0, 0.f, 0, -1, 1, 0);
    mDoMtx_stack_c::transS(position->x, position->y, position->z);
    barrierRotateZXY(mDoMtx_stack_c::get(), 0, (s32)(s16)angle->y, 0);
    J3DModel_setBaseTRMtx(mpModel[index].get(), mDoMtx_stack_c::get());
    barrierParticle(play()->mpParticle.get(), 0, 0x81A9, position.get(), angle.get(), 0, 255, 0, -1,
                    0, 0, 0);
}
/* 02322760 */
BOOL daObjBarrier_ef_c::init() {
    void* data = resource(dRes_INDEX_YCAGE_BDL_YHRBR00_e);
    void* btk = resource(dRes_INDEX_YCAGE_BTK_YHRBR00_e);
    void* bck = resource(dRes_INDEX_YCAGE_BCK_YHRBR00_e);
    void* brk = resource(dRes_INDEX_YCAGE_BRK_YHRBR00_e);
    if (!data || !btk || !bck || !brk) {
        assertion("d_a_obj_barrier.cpp", 1048, "0");
        return 0;
    }
    for (u32 i = 0; i < 4; i++) {
        mpModel[i] = barrierNewModel(data, 0x80000, 0x05020200);
        this->setDummyTexture(i);
        s32 btkOK = barrierInitBtk(&mBtk[i], data, btk, 1, 0, 1.f, 0, -1, 0, 0);
        s32 bckOK = barrierInitBck(&mBck[i], data, bck, 1, 0, 0.f, 0, -1, 0);
        s32 brkOK = barrierInitBrk(&mBrk[i], data, brk, 1, 0, 0.f, 0, -1, 0, 0);
        if (!mpModel[i] || !btkOK || !bckOK || !brkOK)
            return 0;
    }
    return 1;
}
/* 02322B48 */
void daObjBarrier_ef_c::create() {
    for (u32 i = 0; i < 4; i++) {
        J3DModel* model = mpModel[i];
        f32 z = sHeader.mEffectScale.z, x = sHeader.mEffectScale.x, y = sHeader.mEffectScale.y;
        model->mBaseScale.x = x;
        model->mBaseScale.z = z;
        model->mBaseScale.y = y;
    }
}
/* 02323AB4 */
void daObjBarrier_ef_c::execute() {
    u32 flags = mActiveEffFlags;
    for (u32 i = 0; i < 4; i++)
        if (flags & (1u << i)) {
            mBtk[i].play();
            if (mBtk[i].mFrameCtrl.checkState(1) || mBtk[i].mFrameCtrl.mRate == 0.f) {
                flags ^= 1u << i;
                mHitActor[i] = 0;
            }
        }
    mActiveEffFlags = flags;
}
/* 02323C04 */
void daObjBarrier_ef_c::draw() {
    u32 flags = mActiveEffFlags;
    for (u32 i = 0; i < 4; i++)
        if (flags & (1u << i)) {
            J3DModel* model = mpModel[i];
            barrierEntryBtk(&mBtk[i], model->mModelData.get(), (f32)mBtk[i].mFrameCtrl.mFrame);
            f32 frame = (s16)gabi::ftoi(mBtk[i].mFrameCtrl.mFrame);
            barrierEntryBck(&mBck[i], model->mModelData.get(), frame);
            frame = (s16)gabi::ftoi(mBtk[i].mFrameCtrl.mFrame);
            barrierEntryBrk(&mBrk[i], model->mModelData.get(), frame);
            setInvisibleList();
            barrierUpdateModel(model, 0);
            setNormalList();
        }
}
/* 02322B7C */
BOOL daObjBarrier_c::_create() {
    s32 phase = 5;
    if (!(actor_condition & 8)) {
        if (gabi::ea(this) != 0) {
            barrierConstructActor(this);
            __vtbl = gabi::ea(&sActorVtable);
            barrierConstructBtk(&mAnm.mBtk);
            barrierConstructBrk(&mAnm.mBrk);
            mAnm.mBrkFrame = 0.f;
            barrierConstructStatus(&mAtStts);
            barrierConstructGlobalStatus(&mAtStts.__vtbl_gstts);
            mAtStts.__vtbl = gabi::ea(&sStatusVtable);
            mAtStts.__vtbl_gstts = gabi::ea(&sGlobalStatusVtable);
            barrierConstructStatus(&mTgStts);
            barrierConstructGlobalStatus(&mTgStts.__vtbl_gstts);
            mTgStts.__vtbl = gabi::ea(&sStatusVtable);
            mTgStts.__vtbl_gstts = gabi::ea(&sGlobalStatusVtable);
            barrierConstructCollider(&mAtCyl);
            mAtCyl.mShapeVtable = gabi::ea(&sInitialShapeVtable);
            mAtCyl.mAttrVtable = gabi::ea(&sCylinderAttrVtable);
            barrierConstructCylinder(&mAtCyl.mCyl);
            mAtCyl.__vtbl_hitinf = gabi::ea(&sHitInfoVtable);
            mAtCyl.mShapeVtable = gabi::ea(&sShapeVtable);
            mAtCyl.mCyl.__vtbl = gabi::ea(&sCylinderVtable);
            barrierConstructCollider(&mTgCyl);
            mTgCyl.mShapeVtable = gabi::ea(&sInitialShapeVtable);
            mTgCyl.mAttrVtable = gabi::ea(&sCylinderAttrVtable);
            barrierConstructCylinder(&mTgCyl.mCyl);
            mTgCyl.__vtbl_hitinf = gabi::ea(&sHitInfoVtable);
            mTgCyl.mCyl.__vtbl = gabi::ea(&sCylinderVtable);
            mTgCyl.mShapeVtable = gabi::ea(&sShapeVtable);
            barrierConstructArray(&mEffect.mBtk, 4, 0x74, &sBtkConstructor);
            barrierConstructArray(
                &mEffect.mBck, 4, 0x8C,
                gabi::at<void>(gabi::guest_fn((const void*)barrier_bckConstructor)));
            barrierConstructArray(&mEffect.mBrk, 4, 0x78, &sBrkConstructor);
            mEffect.mActiveEffFlags = 0;
        }
        actor_condition = actor_condition | 8;
    }
    if (mInitState) {
        if (!mBarrierActive)
            return phase;
    } else {
        s32 moya = barrier_parameter(this, PRM_MOYA_W, PRM_MOYA_S);
        mMoya = moya;
        if (!moya && barrierCheckEvent(sSave->mEvent, 0x2C02) == 1)
            mBarrierActive = 0;
        else
            mBarrierActive = 1;
        if (!mBarrierActive)
            return phase;
    }
    phase = barrierLoad(&mPhase, "Ycage");
    if (phase != 4)
        return phase;
    if (!barrierEnterHeap(this, gabi::at<void>(gabi::guest_fn((const void*)barrier_heapCallback)),
                          0x30E0))
        return 5;
    if (barrierRegisterBg(&play()->mBgS, mpBgW.get(), this))
        return 5;
    reinterpret_cast<BarrierBackground*>(mpBgW.get())->mCorrectionCallback = 0;
    J3DModel* model = mAnm.mpModel;
    scale.z = scale.x;
    cullMtx = model ? gabi::ea(&model->mBaseTransformMtx) : 0;
    this->init_mtx();
    barrierInitStatus(&mAtStts, 255, 255, this);
    barrierSetCylinder(&mAtCyl, &sAttackSource);
    mAtCyl.mStts = &mAtStts;
    mAtCyl.mFlags = mAtCyl.mFlags | 2;
    barrierInitStatus(&mTgStts, 255, 255, this);
    barrierSetCylinder(&mTgCyl, &sTargetSource);
    mTgCyl.mStts = &mTgStts;
    mTgCyl.mFlags = mTgCyl.mFlags | 2;
    if (barrier_parameter(this, PRM_DAMAGE_W, PRM_DAMAGE_S))
        mAtCyl.mAtp = 0;
    mEffect.create();
    return phase;
}

/* 02322E8C */
BOOL daObjBarrier_c::_delete() {
    if (mBarrierActive) {
        barrierUnload(&mPhase, "Ycage");
        if (heap && mpBgW) {
            if (mpBgW->m_id < 256)
                barrierReleaseBg(&play()->mBgS, mpBgW.get());
            mpBgW = 0;
        }
    }
    return 1;
}
/* 02323B74 */
BOOL daObjBarrier_c::_execute() {
    mAnm.mBtk.play();
    this->brkAnmPlay();
    barrierMoveBg(mpBgW.get());
    barrierMoveStatus(&mAtStts.__vtbl_gstts);
    barrierMoveStatus(&mTgStts.__vtbl_gstts);
    this->checkCollision_At();
    if (!this->checkCollision_Tg())
        this->registCollisionTable();
    if (!this->break_check())
        mEffect.execute();
    return 1;
}
/* 02323D60 */
BOOL daObjBarrier_c::_draw() {
    if (!mBarrierProc) {
        if (!mMoya && mAnm.mBrkFrame > 1.f) {
            barrierEntryBtk(&mAnm.mBtk, mAnm.mpModel->mModelData.get(),
                            (f32)mAnm.mBtk.mFrameCtrl.mFrame);
            f32 frame = (s16)gabi::ftoi(mAnm.mBrkFrame);
            barrierEntryBrk(&mAnm.mBrk, mAnm.mpModel->mModelData.get(), frame);
            barrierUpdateModel(mAnm.mpModel.get(), 0);
        }
        mEffect.draw();
    }
    return 1;
}
/* 02322E88 */
BOOL daObjBarrier_Create(daObjBarrier_c* barrier) { return barrier->_create(); }
/* 02322F10 */
BOOL daObjBarrier_Delete(daObjBarrier_c* barrier) { return barrier->_delete(); }
/* 02323C00 */
BOOL daObjBarrier_Execute(daObjBarrier_c* barrier) { return barrier->_execute(); }
/* 02323E28 */
BOOL daObjBarrier_Draw(daObjBarrier_c* barrier) { return barrier->_draw(); }
/* 02323E2C */
BOOL daObjBarrier_IsDelete() { return 1; }

// HD: Compiler-generated and template code of this translation unit.
/* 02323E34 */
void barrier_staticInit() {
    for (s32 i = 3; i >= 0; --i)
        sHeader.mSeeds[i] = 0;
    barrierRegisterDestructor(&sPiRegistration);
    sHeader.mMin = sPiLimits.mMin;
    sHeader.mMax = sPiLimits.mMax;
    barrierInitRandom(&sHeader.mRandom);
    barrierRegisterDestructor(&sRandomRegistration);
    barrierInitRandomByte(&sHeader.mRandomBytes[1]);
    barrierRegisterDestructor(&sRandomByteRegistration);
    sHeader.mEffectScale.x = 1.f;
    sHeader.mEffectScale.y = 1.f;
    sHeader.mEffectScale.z = 1.f;
}
/* 02323EE0 */
void* barrier_bckConstructor(void* object) {
    auto* anm = static_cast<BarrierBckView*>(object);
    if (!anm) {
        anm = static_cast<BarrierBckView*>(barrierAllocate(0x8C));
        if (!anm)
            return nullptr;
    }
    barrierConstructFrame(anm, 0);
    anm->mMatrixCalcVtable = gabi::ea(&sInitialMatrixVtable);
    barrierConstructBinding(anm->mBinding);
    anm->mLast = 0;
    anm->mBindingVtable = gabi::ea(&sBindingVtable);
    anm->mAnimation = 0;
    anm->mReserved = 0;
    anm->mMatrixCalcVtable = gabi::ea(&sMatrixVtable);
    anm->mFlags = 0;
    anm->mState = 0;
    return anm;
}

/* 02323F70 */
void barrier_trivialDestructor(void* object, u32 flags) {
    if (object && (flags & 1))
        barrierDelete(object);
}
/* 02323F84 */
void barrier_bckDestructor(void* object, u32 flags) {
    if (object) {
        barrierMatrixDtor(&static_cast<BarrierBckView*>(object)->mMatrixCalcVtable, 0);
        if (flags & 1)
            barrierDelete(object);
    }
}
/* 02323FD8 */
void barrier_actorDestructor(void* object, u32 flags) {
    if (!object)
        return;
    auto* actor = static_cast<daObjBarrier_c*>(object);
    barrierDestroyArray(actor->mEffect.mBck, 4, 0x8C,
                        gabi::at<void>(gabi::guest_fn((const void*)barrier_bckDestructor)), 0, 0);
    barrierDestroyCylinder(&actor->mTgCyl, 2);
    barrierDestroyCylinder(&actor->mAtCyl, 2);
    barrierDestroyStatus(&actor->mTgStts, 2);
    barrierDestroyStatus(&actor->mAtStts, 2);
    barrierDestroyActor(object, 0);
    if (flags & 1)
        barrierDelete(object);
}
/* 0232407C */
void barrier_noop(void* object) {}

/* 02324080 */
u32 barrier_parameter(daObjBarrier_c* barrier, u32 width, u32 shift) {
    u32 mask = (width & 32) ? 0 : (1u << (width & 31));
    u32 value = (shift & 32) ? 0 : ((u32)barrier->mParameters >> (shift & 31));
    return value & (mask - 1);
}

// clang-format off
/* WWHD glue
data sActorVtable 1002607C
data sStatusVtable 1004AE88
data sGlobalStatusVtable 1004AEC0
data sInitialShapeVtable 100015A8
data sCylinderAttrVtable 10026044
data sHitInfoVtable 1004B108
data sShapeVtable 1004B160
data sCylinderVtable 1004B150
data sInitialMatrixVtable 1016E54C
data sBindingVtable 1016D820
data sMatrixVtable 10026054
data sResourceControl 101F4F28
data sTextureControl 101F9968
data sSave 101F84DC
data sDrawList 104B4634
data sXluDrawList 104B4638
data sHeader 1046927C
data sPiLimits 10026238
data sTrig 104A44F8
data sDummyName 10026130
data sPiRegistration 101C7FA4
data sRandomRegistration 101C7FB0
data sRandomByteRegistration 101C7FBC
data sAttackSource 1002608C
data sTargetSource 100260D0
data sBtkConstructor 025E7C6C
data sBrkConstructor 025E80D0
ext 026066C4 void* barrierResource(void*, BarrierArchive*, s32)
ext 025200D4 BarrierGameInfo* barrierGameInfo()
ext 0273AA24 void barrierAssert(const char*, s32, const char*)
ext 0273AD10 void* barrierAllocate(u32)
ext 027F2BC0 void barrierConstructFrame(void*, s32)
ext 027DA984 void barrierConstructBinding(void*)
ext 028F0164 void barrierDestroyArray(void*, u32, u32, void*, u32, u32)
ext 02515A70 void barrierDestroyCylinder(BarrierCylinder*, u32)
ext 02515860 void barrierDestroyStatus(dCcD_Stts*, u32)
ext 025D50BC void barrierDestroyActor(void*, u32)
ext 0273AF40 void barrierDelete(void*)
ext 028F026C void barrierRegisterDestructor(void*)
ext 028ED6F8 void barrierInitRandom(void*)
ext 028EAB2C void barrierInitRandomByte(void*)
ext 020087EC void barrierReleaseBg(dBgS*, dBgW*)
ext 0200BD2C void barrierConstructStatus(dCcD_Stts*)
ext 0200E240 void barrierSetCollider(void*, BarrierCylinder*)
ext 020182E0 void barrierCylinderCenter(cM3dGCyl*, cXyz*)
ext 02018428 void barrierCylinderHeight(cM3dGCyl*, f32)
ext 020184DC void barrierCylinderRadius(cM3dGCyl*, f32)
ext 02018590 void barrierConstructCylinder(cM3dGCyl*)
ext 020195B0 u32 barrierAtan2(f32, f32)
ext 0201A478 void barrierConstructAngle(csXyz*, s32, s32, s32)
ext 0201ADE0 void barrierSubtract(cXyz*, cXyz*, cXyz*)
ext 024EEA6C s32 barrierRegisterBg(dBgS*, dBgW*, daObjBarrier_c*)
ext 024F2478 dBgW* barrierNewBackground(void*, u32, Mtx34*)
ext 024F43DC void barrierMoveBg(dBgW*)
ext 02515BBC fopAc_ac_c* barrierHitActor(void*)
ext 02515DA0 void barrierConstructGlobalStatus(void*)
ext 02515E50 void barrierMoveStatus(void*)
ext 02515F14 void barrierInitStatus(dCcD_Stts*, s32, s32, daObjBarrier_c*)
ext 02515FB8 void barrierConstructCollider(BarrierCylinder*)
ext 02516094 void barrierClearAt(BarrierCylinder*)
ext 025160DC s32 barrierCheckAt(BarrierCylinder*)
ext 0251621C void barrierClearTg(BarrierCylinder*)
ext 025162A4 s32 barrierCheckTg(BarrierCylinder*)
ext 02516300 u32 barrierTgHitObject(BarrierCylinder*)
ext 02516518 void barrierSetCylinder(BarrierCylinder*, void*)
ext 02520460 s32 barrierLoad(request_of_phase_process_class*, const char*)
ext 025204C8 void barrierUnload(request_of_phase_process_class*, const char*)
ext 02543F10 s32 barrierEventIndex(void*, const char*, u32)
ext 025440C8 s32 barrierEventEnd(void*, s32)
ext 025A847C void barrierParticle(void*, u32, u32, cXyz*, csXyz*, u32, u32, u32, s32, u32, u32, u32)
ext 025B8B68 void barrierSetEvent(void*, u32)
ext 025B8B94 s32 barrierCheckEvent(void*, u32)
ext 025D4ED0 void barrierConstructActor(daObjBarrier_c*)
ext 025D57E0 void barrierDeleteActor(daObjBarrier_c*)
ext 025D63E8 s32 barrierEnterHeap(daObjBarrier_c*, void*, u32)
ext 025D7A58 void barrierOrderEvent(daObjBarrier_c*, s32, u32, u32, u32, u32)
ext 025E2DE0 void barrierUpdateModel(J3DModel*, u32)
ext 025E38E0 J3DModel* barrierNewModel(void*, u32, u32)
ext 025E7C6C void barrierConstructBtk(mDoExt_btkAnm*)
ext 025E7CE0 s32 barrierInitBtk(mDoExt_btkAnm*, void*, void*, u32, s32, f32, s32, s32, u32, u32)
ext 025E7FC4 void barrierEntryBtk(mDoExt_btkAnm*, J3DModelData*, f32)
ext 025E80D0 void barrierConstructBrk(BarrierBrkAnm*)
ext 025E8154 s32 barrierInitBrk(BarrierBrkAnm*, void*, void*, u32, s32, f32, s32, s32, u32, u32)
ext 025E83FC void barrierEntryBrk(BarrierBrkAnm*, J3DModelData*, f32)
ext 025E8508 s32 barrierInitBck(mDoExt_bckAnm*, void*, void*, u32, s32, f32, s32, s32, u32)
ext 025E86B8 void barrierEntryBck(mDoExt_bckAnm*, J3DModelData*, f32)
ext 025F05D0 void barrierChangeScene(u32, u32)
ext 025F1B48 void barrierRotateZXY(Mtx34*, s32, s32, s32)
ext 027ED1F0 be<u8>* barrierTextureName(JUTNameTab*, u32)
ext 02773680 BarrierTextureObject* barrierTextureCtor(void*, void*)
ext 027F3628 void barrierMatrixDtor(void*, u32)
ext 027F81A4 void* barrierTextureHandle(void*, u32)
ext 028E8DD0 f64 barrierSquareMag(cXyz*)
ext 028E90D4 void barrierCopyMtx(Mtx34*, Mtx34*)
ext 028EFFD0 void barrierConstructArray(void*, u32, u32, void*)
ext 028F4384 f64 barrierSqrt(f64)
str 10026024
str 1002611C in 02322390
str 10026118 in 02322390
str 10026138 in 02322518
str 1002614C in 02322518
str 10026158 in 02322518
str 10026168 in 02322518
str 10026184 in 02322760
str 10026180 in 02322760
str 100261BC in 0232302C
str 100261D0 in 0232302C
str 100261DC in 0232302C
str 100261F0 in 0232302C
str 10026204 in 0232302C
str 1002622C in 0232384C
*/
// clang-format on
