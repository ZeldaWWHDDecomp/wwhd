// agl parameter selection: a 32-character text parameter whose text names one entry of an owner's list
// (a HierarchyNode). The parameter is linked into the owner's entry list, finds the index of the entry its
// text names, and tells a listener when it did. Base of the environment set's record (agl/EnvironmentSet.h).
// Layouts from WWHD's constructors and instructions.
#pragma once
#include "agl/ParameterBase.h"
#include "agl/HierarchyNode.h"
namespace agl {

// A text parameter of type String32 (ParameterType_String32) with its value in a FixedSafeText<32>.
struct TextParameter32 : ParameterBase {
    sead::FixedSafeText<32> mValue;
    void onResourceApplied(const be<u8>* data, u32 size);
    u32 valueKind() const;
    const be<u8>* constValue();
    be<u8>* value();
    u32 valueBytes() const;
};

// The listener's dispatch: slot 1 is told the selection was looked up again.
struct ParameterSelectionListenerMethods {
    sead::VtableSlot mHeader;
    sead::VtableSlot mSelectionUpdated;
};
struct ParameterSelectionListener {
    gptr<ParameterSelectionListenerMethods> mMethods;
};

// The selection's own virtuals beyond the parameter interface (ParameterMethods, 0x78 bytes).
struct ParameterSelectionMethods {
    ParameterMethods mParameter;
    sead::VtableSlot mDestroy;      // +0x78
    sead::VtableSlot mEntryName;    // +0x80: name of entry i
    sead::VtableSlot mEntryCount;   // +0x88
};

struct ParameterSelection : TextParameter32 {
    enum : s32 {
        Selection_None = -1,      // the text is empty
        Selection_NotFound = -2,  // no entry has that name
    };
    be<s32> mIndex;
    sead::Link mLink;                           // in the owner's entry list (at the owner's link offset)
    gptr<ParameterSelectionListener> mListener;
    gptr<HierarchyNode> mOwner;

    ParameterSelectionMethods* selectionMethods() const {
        return gabi::at<ParameterSelectionMethods>(gabi::ea(mMethods.get()));
    }
    s32 entryCount() { return gabi::call_ptr<s32>(selectionMethods()->mEntryCount.mFunction, this); }
    sead::SafeText* entryName(s32 index) {
        return gabi::call_ptr<sead::SafeText*>(selectionMethods()->mEntryName.mFunction, this, index);
    }

    static ParameterSelection* create(ParameterSelection* storage);
    void setOwner(HierarchyNode* owner);
    void destroy(u32 flags);
    void updateIndex();
};

// Text classes whose vtables this translation unit stores (only their out-of-line virtuals exist).
namespace parameter_selection_text {
struct Text : sead::SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct TextBuffer : sead::SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct FixedTextBase32 : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedText32 : sead::SafeTextBuffer { void destroy(u32 flags); };
// Fixed-size buffers whose vtables are never stored in this binary (sizes unknown; vtable order).
struct FixedTextA : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextB : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextC : sead::SafeTextBuffer { void destroy(u32 flags); };
}
}
WWHD_OFFSET(agl::TextParameter32, mValue, 0xC);
WWHD_SIZE(agl::TextParameter32, 0x38);
WWHD_OFFSET(agl::ParameterSelection, mIndex, 0x38);
WWHD_OFFSET(agl::ParameterSelection, mLink, 0x3C);
WWHD_OFFSET(agl::ParameterSelection, mListener, 0x44);
WWHD_OFFSET(agl::ParameterSelection, mOwner, 0x48);
WWHD_SIZE(agl::ParameterSelection, 0x4C);
WWHD_OFFSET(agl::ParameterSelectionMethods, mDestroy, 0x78);
WWHD_OFFSET(agl::ParameterSelectionMethods, mEntryName, 0x80);
WWHD_OFFSET(agl::ParameterSelectionMethods, mEntryCount, 0x88);
WWHD_OFFSET(agl::ParameterSelectionListenerMethods, mSelectionUpdated, 8);
