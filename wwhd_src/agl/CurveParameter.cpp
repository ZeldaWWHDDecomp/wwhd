/* Parameter kind, payload access and two bounded curve records in the WWHD library. */
// References: WWHD's own instructions and strings only.
#include "agl/CurveParameter.h"
#include "agl/ParameterCollections.h"     // ParameterText (AGL#13)
// AGL#13's text method table (sParameterTextMethods in agl/ParameterCollections.cpp), bound by this TU's data line.
static sead::SafeTextVtable sParameterComparisonTextMethods_l;
static void* moveBytes(void* destination, const void* source, u32 bytes, u32 flags);
static const agl::CurveDefaults sCurveDefaults;
// Image owns the contents; this definition supplies the data binding only.
static const agl::CurveEvaluatorTable_l sCurveEvaluators_l;

/* 027B4FCC */
u32 agl::CurveParameter_l::kind() { return 10; }
/* 027B4FD4 */
const void* agl::CurveParameter_l::readData() { return mCurves; }
/* 027B4FDC */
void* agl::CurveParameter_l::writeData() { return mCurves; }
/* 027B4FE4 */
u32 agl::CurveParameter_l::byteSize() { return 256; }
/* 027B4FEC */
void agl::CurveParameter_l::initializeCurves() {
    gabi::AutoFrame frame{gabi::current_function()};
    for (u32 index = 0; index < 2; ++index) {
        auto values = mCurves[index].mValues;
        moveBytes(values, &sCurveDefaults, 36, 0);
        for (u32 value = 9; value < 30; ++value)
            values[value] = 1.0f;
        mCurves[index].mKind = 7;
        mCurves[index].mCount = 9;
        mDescriptors[index].mCapacity = 30;
        mDescriptors[index].mKind = 7;
        mDescriptors[index].mCount = 9;
        mDescriptors[index].mValues = values;
    }
}
/* 027B50BC */
void agl::CurveParameter_l::setCurve(u32 index, u32 kind, const be<f32>* values, u32 count) {
    mDescriptors[index].mCount = count;
    mDescriptors[index].mKind = kind;
    mCurves[index].mKind = kind;
    mCurves[index].mCount = count;
    for (u32 value = 0; value < count; ++value) {
        f32 sample = values[value];
        mCurves[index].mValues[value] = sample;
    }
}
/* 027B5108 */
f32 agl::CurveParameter_l::evaluate(u32 index, f32 position) const {
    const auto& descriptor = mDescriptors[index];
    u32 kind = descriptor.mKind;
    u32 evaluator = sCurveEvaluators_l.mFunctions[kind];
    auto samples = reinterpret_cast<const agl_gx2_support::CurveSamples*>(&descriptor.mKind);
    auto values = descriptor.mValues.get();
    return gabi::tail_ptr<f32>(evaluator, samples, values, position);
}
/* 027B5134 */
void agl::CurveParameter_l::copyFrom(CurveParameter_l* source) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto sourceMethods = source->mMethods.get();
    u32 destinationKind = gabi::call_ptr<u32>(mMethods->mKind, this);
    u32 sourceKind = gabi::call_ptr<u32>(sourceMethods->mKind, source);
    if (destinationKind != sourceKind)
        return;
    void* destinationData = gabi::call_ptr<void*>(mMethods->mWriteData, this);
    const void* sourceData = gabi::call_ptr<const void*>(source->mMethods->mReadData, source);
    u32 bytes = gabi::call_ptr<u32>(mMethods->mByteSize, this);
    moveBytes(destinationData, sourceData, bytes, 0);
    for (u32 index = 0; index < 2; ++index) {
        mDescriptors[index].mKind = (u8)source->mDescriptors[index].mKind;
        mDescriptors[index].mCount = (u8)source->mDescriptors[index].mCount;
    }
}
/* 027B5370 */
void agl::CurveParameter_l::restoreDescriptors(const void* storage, u32 bytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 expected = gabi::call_ptr<u32>(mMethods->mByteSize, this);
    for (u32 index = 0; index < 2; ++index) {
        if (bytes == expected) {
            u32 kind = mCurves[index].mKind;
            mDescriptors[index].mValues = mCurves[index].mValues;
            mDescriptors[index].mKind = kind;
            mDescriptors[index].mCapacity = 30;
            mDescriptors[index].mCount = (u32)mCurves[index].mCount;
        } else {
            mDescriptors[index].mCapacity = 30;
            mDescriptors[index].mValues = mCurves[index].mValues;
        }
    }
}
/* 027B5234 */
u32 agl::CurveParameter_l::compareAndCopy(CurveParameter_l* source) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto methods = mMethods.get();
    u32 query = methods->mKind;
    auto sourceMethods = source->mMethods.get();
    u32 kind = gabi::call_ptr<u32>(query, this);
    u32 sourceKind = gabi::call_ptr<u32>(sourceMethods->mKind, source);
    if (kind != sourceKind)
        return 0;
    u32 empty = sFileDeviceEmptyText.mText.v;
    gabi::FrameLocal<sead::SafeText> first(8), second(0x10);
    second->mVtable = &sParameterComparisonTextMethods_l;
    first->mText.v = empty;
    second->mText.v = empty;
    first->mVtable = &sParameterComparisonTextMethods_l;
    static_cast<agl::ParameterText*>(first.get())->assureTermination();
    first->assureTermination();
    auto identity = first->mText.get();
    second->assureTermination();
    auto right = second->mText.get();
    bool same = identity == right;
    if (!same) {
        auto left = first->mText.get();
        for (u32 index = 0; index < 0x40001; ++index) {
            u32 firstByte = left[index];
            u32 secondByte = right[index];
            if (firstByte != secondByte)
                break;
            if (firstByte == 0) {
                same = true;
                break;
            }
        }
    }
    if (!same)
        return 0;
    gabi::call_ptr<void>(mMethods->mCopy, this, source);
    return 1;
}
/* WWHD glue
data sParameterComparisonTextMethods_l 10168EC4
ext 027B1A30 void agl::ParameterText::assureTermination()
ext C0009988 void* moveBytes(void* destination, const void* source, u32 bytes, u32 flags)
data sCurveDefaults 10169734
data sCurveEvaluators_l 101F96C8
*/
