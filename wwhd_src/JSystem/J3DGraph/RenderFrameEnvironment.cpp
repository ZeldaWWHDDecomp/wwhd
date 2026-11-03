// J3D HD renderer: per-frame environment update (model/projection matrices, lights, colours, fog).
// Names are our own; written from the WWHD binary.
#include "JSystem/J3DGraph/RenderEnvironmentViews.h"
#include <atomic>

// Runtime types of the three environment kinds (type index, first-use guard, type node); they belong to the
// environment classes' translation units and are referenced here by address.
static j3d::RenderEnvironmentTypeIndex_l* sPositionEnvironmentIndex;
static j3d::RenderEnvironmentTypeIndex_l* sColorEnvironmentIndex;
static j3d::RenderEnvironmentTypeIndex_l* sFogEnvironmentIndex;
static u32 sPositionEnvironmentGuard;
static u32 sColorEnvironmentGuard;
static u32 sFogEnvironmentGuard;
static sead::RuntimeType sPositionEnvironmentType;
static sead::RuntimeType sColorEnvironmentType;
static sead::RuntimeType sFogEnvironmentType;
static u32 sRenderEnvironmentTypeTable;
static j3d::RenderWideWords sDefaultEnvironmentValue;

/* 027FDA54 */
void j3d::RenderFrameEnvironmentEntries::update(RenderFrameArray* frames, u32 index, const Mtx34* model,
                                               const Mtx44* projection, const RenderEnvironmentCollection_l* environment) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    RenderFrameUniforms* output = frames->frames();
    if (index < (u32)frames->mCount) output += index;
    copyRenderMatrix34(model, &output->mPrimaryMatrix);
    composeRenderFrameProjection(&output->mSecondaryMatrix, projection, model);
    writeRenderFrameProjection(projection, &output->mFinalMatrix);

    // A missing type index deliberately falls back to the first range, as in
    // the original. A populated range must index readable pointer storage.
    auto selectObject = [&](const RenderEnvironmentTypeIndex_l* type, u32 element) -> RenderEnvironmentObject_l* {
        u32 kind = (u32)(s32)type->mIndex;
        const RenderEnvironmentRange_l* range = environment->mRanges;
        if (kind < environment->mRangeCount) range += kind;
        if (element >= (u16)range->mCount) return NULL;
        u32 objectIndex = (u16)range->mFirst + element;
        const gptr<RenderEnvironmentObject_l>* entry = guestNull<gptr<RenderEnvironmentObject_l>>();
        if (objectIndex < environment->mObjectCount) entry = &environment->mObjects.get()[objectIndex];
        return *entry;
    };

    if (environment != NULL) {
        const RenderEnvironmentTypeIndex_l* positionIndex = sPositionEnvironmentIndex;
        for (u32 element = 0; element < 8; ++element) {
            RenderEnvironmentObject_l* object = selectObject(positionIndex, element);
            if (sPositionEnvironmentGuard == 0) {
                sPositionEnvironmentType.mTable = &sRenderEnvironmentTypeTable;
                sPositionEnvironmentGuard = 1;
            }
            if (object != NULL &&
                gabi::call_ptr<u32>(object->mMethods->mMatchesType, object, &sPositionEnvironmentType) != 0 &&
                object->mEnabled != 0) {
                auto* positioned = reinterpret_cast<RenderPositionEnvironment_l*>(object);
                output->mVectors[element].mX = positioned->mPosition.mX;
                output->mVectors[element].mY = positioned->mPosition.mY;
                output->mVectors[element].mZ = positioned->mPosition.mZ;
                for (u32 word = 0; word < 4; ++word)
                    output->mWideValues[element].mWords[word] = positioned->mColor.mWords[word];
            } else {
                output->mVectors[element].mX = 0.0f;
                output->mVectors[element].mY = 0.0f;
                output->mVectors[element].mZ = 0.0f;
                for (u32 word = 0; word < 4; ++word)
                    output->mWideValues[element].mWords[word] = sDefaultEnvironmentValue.mWords[word];
            }
        }
        const RenderEnvironmentTypeIndex_l* colorIndex = sColorEnvironmentIndex;
        for (u32 element = 0; element < 2; ++element) {
            RenderEnvironmentObject_l* object = selectObject(colorIndex, element);
            if (sColorEnvironmentGuard == 0) {
                sColorEnvironmentType.mTable = &sRenderEnvironmentTypeTable;
                sColorEnvironmentGuard = 1;
            }
            if (object != NULL &&
                gabi::call_ptr<u32>(object->mMethods->mMatchesType, object, &sColorEnvironmentType) != 0 &&
                object->mEnabled != 0) {
                auto* colored = reinterpret_cast<RenderColorEnvironment_l*>(object);
                for (u32 word = 0; word < 4; ++word)
                    output->mExtraValues[element].mWords[word] = colored->mColor.mWords[word];
            } else {
                for (u32 word = 0; word < 4; ++word)
                    output->mExtraValues[element].mWords[word] = sDefaultEnvironmentValue.mWords[word];
            }
        }
        const RenderEnvironmentTypeIndex_l* fogIndex = sFogEnvironmentIndex;
        RenderEnvironmentObject_l* object = selectObject(fogIndex, 0);
        if (sFogEnvironmentGuard == 0) {
            sFogEnvironmentType.mTable = &sRenderEnvironmentTypeTable;
            sFogEnvironmentGuard = 1;
        }
        if (object != NULL &&
            gabi::call_ptr<u32>(object->mMethods->mMatchesType, object, &sFogEnvironmentType) != 0 &&
            object->mEnabled != 0) {
            auto* fog = reinterpret_cast<RenderFogEnvironment_l*>(object);
            for (u32 word = 0; word < 4; ++word)
                output->mReferenceValue.mWords[word] = fog->mColor.mWords[word];
            output->mFirstLimit = fog->mNear;
            f32 far = fog->mFar;
            f32 near = fog->mNear;
            f32 factor = 1.0f;
            if (far != near) factor = gabi::fdivs_ppc(1.0f, gabi::fsubs_ppc(far, near));
            output->mSecondLimit = factor;
            f32 alpha = gabi::f32_from_bits(fog->mColor.mWords[3]);
            output->mSecondLimit = gabi::fmuls_ppc(factor, alpha);
        } else {
            output->mSecondLimit = 1000000.0f;
            output->mFirstLimit = 1000000.0f;
        }
    } else {
        for (u32 element = 0; element < 8; ++element) {
            output->mVectors[element].mX = 0.0f;
            output->mVectors[element].mY = 0.0f;
            output->mVectors[element].mZ = 0.0f;
            for (u32 word = 0; word < 4; ++word)
                output->mWideValues[element].mWords[word] = sDefaultEnvironmentValue.mWords[word];
        }
        for (u32 element = 0; element < 2; ++element)
            for (u32 word = 0; word < 4; ++word)
                output->mExtraValues[element].mWords[word] = sDefaultEnvironmentValue.mWords[word];
        output->mSecondLimit = 1000000.0f;
        output->mFirstLimit = 1000000.0f;
    }
    std::atomic_thread_fence(std::memory_order_acquire);
}

/* WWHD glue
 data sPositionEnvironmentIndex 104A1F70
 data sColorEnvironmentIndex 104A1F64
 data sFogEnvironmentIndex 104A1460
 data sPositionEnvironmentGuard 101FD7D4
 data sColorEnvironmentGuard 101FD7C8
 data sFogEnvironmentGuard 101FD7DC
 data sPositionEnvironmentType 101FDCCC
 data sColorEnvironmentType 101FDFD0
 data sFogEnvironmentType 101FDCE0
 data sRenderEnvironmentTypeTable 1016EE40
 data sDefaultEnvironmentValue 104A01CC
 ext 0272D680 void composeRenderFrameProjection(Mtx44*,const Mtx44*,const Mtx34*)
 ext 028E8970 void writeRenderFrameProjection(const Mtx44*,Mtx44*)
*/
