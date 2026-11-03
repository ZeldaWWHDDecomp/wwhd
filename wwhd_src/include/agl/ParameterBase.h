// agl parameter base: the 12-byte named value every agl parameter derives from (name hash, next
// parameter of its owner, methods), with its name hashing, construction, copy and resource application.
// AGL#1's environment and projection parameters name it agl::env::ParameterBinding (agl/ParameterValue.h).
// format facts: AAMP parameter type ids (public format documentation).
#pragma once
#include "sead/FileDevice.h"
#include "agl/ResParameterArchive.h"
namespace agl {

// Type ids returned by a parameter's valueKind() method.
enum ParameterType : u32 {
    ParameterType_Bool = 0,
    ParameterType_F32 = 1,
    ParameterType_Int = 2,
    ParameterType_Vec2 = 3,
    ParameterType_Vec3 = 4,
    ParameterType_Vec4 = 5,
    ParameterType_Color = 6,
    ParameterType_String32 = 7,
    ParameterType_String64 = 8,
    ParameterType_Curve1 = 9,
    ParameterType_Curve2 = 10,
    ParameterType_Curve3 = 11,
    ParameterType_Curve4 = 12,
    ParameterType_BufferInt = 13,
    ParameterType_BufferF32 = 14,
    ParameterType_String256 = 15,
};

// GHS vtable of a parameter: 8-byte entries {adjust, function}.
struct ParameterMethods {
    sead::VtableSlot mHeader;
    sead::VtableSlot mCopy;              // +0x08
    sead::VtableSlot mCopyUnchecked;     // +0x10
    sead::VtableSlot mReservedA[3];      // +0x18
    sead::VtableSlot mValueKind;         // +0x30
    sead::VtableSlot mConstValue;        // +0x38
    sead::VtableSlot mValue;             // +0x40
    sead::VtableSlot mReservedB[2];      // +0x48
    sead::VtableSlot mValueBytes;        // +0x58
    sead::VtableSlot mReservedC;         // +0x60
    sead::VtableSlot mOnResourceApplied; // +0x68
    sead::VtableSlot mReservedD;         // +0x70
};

// One parameter record of a parameter resource (agl/ResParameterArchive.h's ResParameterData: byte size with the
// header, type, name hash) viewed with its payload.
struct ParameterRecord : ResParameterData {
    be<u8> mData[4];
    u32 payloadSize() const { return static_cast<u32>(mSize) - u32(sizeof(ResParameterData)); }
};

namespace parameter_tree { struct Value; }  // AGL#9's parameter object (owner of a parameter chain)

struct ParameterBase {
    be<u32> mNameHash;
    gptr<ParameterBase> mNext;
    gptr<sead::VtableSlot> mMethods;

    ParameterMethods* methods() const { return gabi::at<ParameterMethods>(gabi::ea(mMethods.get())); }
    u32 valueKind() { return gabi::call_ptr<u32>(methods()->mValueKind.mFunction, this); }
    void* value() { return gabi::call_ptr<void*>(methods()->mValue.mFunction, this); }
    const void* constValue() { return gabi::call_ptr<const void*>(methods()->mConstValue.mFunction, this); }
    u32 valueBytes() { return gabi::call_ptr<u32>(methods()->mValueBytes.mFunction, this); }

    static u32 hashName(sead::SafeText* name);
    void initialize(sead::SafeText* name, sead::SafeText* label, sead::SafeText* meta, parameter_tree::Value* owner);
    static ParameterBase* create(ParameterBase* storage);
    static ParameterBase* create(ParameterBase* storage, sead::SafeText* name, sead::SafeText* label, parameter_tree::Value* owner);
    u32 copy(ParameterBase* source);
    void copyUnchecked(ParameterBase* source);
    u32 makeZero();
    u32 isFloatType();
    void applyResource(gptr<ParameterRecord>* record);
    void addWeightedResource(gptr<ParameterRecord>* record, f64 weight);
    void onResourceApplied(const be<u8>* data, u32 size);
    void notifyResourceApplied(const be<u8>* data, u32 size) {
        gabi::call_ptr<void>(methods()->mOnResourceApplied.mFunction, this, data, size);
    }
};

// Float payloads of the interpolated types, viewed as values or as their raw words.
template <u32 N> union ParameterFloats { be<f32> mValues[N]; be<u32> mWords[N]; };

// Text copies emitted by the parameter translation unit (only their out-of-line virtuals).
namespace parameter_text {
struct Text : sead::SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct TextBuffer : sead::SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct FixedTextA : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextB : sead::SafeTextBuffer { void destroy(u32 flags); };
}
}
WWHD_SIZE(agl::ParameterBase, 12);
WWHD_OFFSET(agl::ParameterBase, mNext, 4);
WWHD_OFFSET(agl::ParameterBase, mMethods, 8);
WWHD_OFFSET(agl::ParameterMethods, mValueKind, 0x30);
WWHD_OFFSET(agl::ParameterMethods, mConstValue, 0x38);
WWHD_OFFSET(agl::ParameterMethods, mValue, 0x40);
WWHD_OFFSET(agl::ParameterMethods, mValueBytes, 0x58);
WWHD_OFFSET(agl::ParameterMethods, mOnResourceApplied, 0x68);
WWHD_SIZE(agl::ParameterMethods, 0x78);
WWHD_OFFSET(agl::ParameterRecord, mNameHash, 8);
WWHD_OFFSET(agl::ParameterRecord, mData, 12);

// Its environment name: agl::env::ParameterBinding (sources that include only this header keep that name too).
#include "agl/ParameterValue.h"
