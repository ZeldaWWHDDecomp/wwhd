// agl parameter object: a named group of parameters (the parameter_tree::Value of agl/ParameterTree.h) with its
// construction, parameter chain, copy, and application of a parameter-object resource, plain or blended.
// Layouts from WWHD's constructor and instructions; the resource views are agl/ResParameterArchive.h's.
#pragma once
#include "agl/ParameterBase.h"
#include "agl/ParameterTree.h"
namespace agl {

// The object's dispatch (GHS vtable, 8-byte slots).
struct ParameterObjectMethods {
    sead::VtableSlot mHeader;
    sead::VtableSlot mReserved[2];
    sead::VtableSlot mBeginApply;   // +0x18: false skips applying a resource
    sead::VtableSlot mEndApply;     // +0x20
    sead::VtableSlot mBeginCopy;    // +0x28: false skips copying another object
    sead::VtableSlot mEndCopy;      // +0x30
    sead::VtableSlot mIsApply;      // +0x38
};

struct ParameterObject : parameter_tree::Value {
    ParameterBase* firstParameter() const { return gabi::at<ParameterBase>(u32(mPrefix)); }
    ParameterObjectMethods* objectMethods() const { return gabi::at<ParameterObjectMethods>(gabi::ea(mVtable.get())); }
    u32 beginApply() { return gabi::call_ptr<u32>(objectMethods()->mBeginApply.mFunction, this); }
    void endApply() { gabi::call_ptr<void>(objectMethods()->mEndApply.mFunction, this); }
    u32 beginCopy() { return gabi::call_ptr<u32>(objectMethods()->mBeginCopy.mFunction, this); }
    void endCopy() { gabi::call_ptr<void>(objectMethods()->mEndCopy.mFunction, this); }

    static ParameterObject* create(ParameterObject* storage);
    void appendParameter(ParameterBase* parameter);
    void copyParameters(ParameterBase* first, ParameterBase* last, ParameterBase* source, ParameterBase* sourceLast);
    ParameterBase* findParameter(u32 nameHash);
    void copy(ParameterObject* source);
    void applyResource(u32 blend, const gptr<ResParameterObjData>* first, const gptr<ResParameterObjData>* second,
                       parameter_tree::Node* list, f64 weight);
    void applyResourceBlend(const gptr<ResParameterObjData>* first, const gptr<ResParameterObjData>* second,
                            parameter_tree::Node* list, f64 weight);
};

// Text class whose vtable this translation unit stores (only its out-of-line virtual exists here).
namespace parameter_object_text {
struct Text : sead::SafeText { void destroy(u32 flags); };
}
}
WWHD_OFFSET(agl::ParameterObjectMethods, mBeginApply, 0x18);
WWHD_OFFSET(agl::ParameterObjectMethods, mBeginCopy, 0x28);
WWHD_OFFSET(agl::ParameterObjectMethods, mIsApply, 0x38);
WWHD_SIZE(agl::ParameterObject, 0x5C);
