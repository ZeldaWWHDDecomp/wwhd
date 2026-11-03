// Parameter names and intrusive tree links reconstructed from assigned WWHD routines.
#pragma once
#include "sead/FileDevice.h"
namespace agl { namespace parameter_tree {
struct Value;
// Record prefix used by this unit's count and relative-step traversal.
struct InputRecord_l { be<u32> mByteStep; be<u32> mNameHash; be<u32> mChildCount; be<u32> mValueCount; };
struct NodeApplyDispatch { u8 mPrefix[0x1C]; be<u32> mBeforeApply; be<u32> mReserved; be<u32> mAfterApply; };
struct NodeMatchDispatch { u8 mPrefix[0x2C]; be<u32> mMatchesRecord; };
// The node's report slots (GHS vtable, 8-byte slots): a parameter of one of its objects that no resource sets
// (object, parameter, &resource), or that two resources cannot blend (object, parameter, &first, &second,
// &firstParameter, &secondParameter, weight).
struct NodeReportDispatch {
    u8 mPrefix[0x30];
    sead::VtableSlot mParameterNotApplied;
    sead::VtableSlot mParameterNotBlended;
};
struct ValueVtable { u8 mPrefix[0x3C]; be<u32> mMatchesRecord; };
struct MatchRecord { be<u32> mPrefix; be<u32> mSecondaryValue; be<u32> mComparisonValue; };
struct Value {
    be<u32> mPrefix;
    sead::FixedSafeText<64> mName;
    be<u32> mNameHash;
    gptr<Value> mNext;
    gptr<ValueVtable> mVtable;
    u32 constantOneHook(); u32 alternateConstantOneHook();
    void emptyHook(); void alternateEmptyHook();
    u32 matchesRecord(const gptr<MatchRecord>* record);
};
struct Node {
    gptr<Value> mValues; gptr<Node> mChildren;
    sead::FixedSafeText<64> mName;
    be<u32> mNameHash;
    gptr<Node> mNextSibling; gptr<Node> mParent;
    be<u32> mTable;
    static Node* create(Node* node);
    u32 constantOneHook(); void emptyHook();
    u32 matchesRecord(const gptr<MatchRecord>* record);
    void trailingEmptyHook(); void alternateTrailingEmptyHook(); void finalEmptyHook();
    void applyRecord(const gptr<InputRecord_l>* record);
    void applyBlend(const gptr<InputRecord_l>* first, const gptr<InputRecord_l>* second, f64 weight);
    void applySecondValues(u32 blend, const gptr<InputRecord_l>* second, f64 weight);
    void applySecondChildren(u32 blend, const gptr<InputRecord_l>* second, f64 weight);
    void blendRecords(u32 blend, const gptr<InputRecord_l>* first, const gptr<InputRecord_l>* second, f64 weight);
    void setName(sead::SafeText* name);
    void appendChild(Node* child, sead::SafeText* name);
    void appendValue(Value* value, sead::SafeText* name);
    MatchRecord* findValueInput(const gptr<InputRecord_l>* input, Value* value);
    InputRecord_l* findChildInput(const gptr<InputRecord_l>* input, Node* child);
    Node* findChild(const gptr<MatchRecord>* record);
    Value* findValue(const gptr<MatchRecord>* record);
};
}}
WWHD_OFFSET(agl::parameter_tree::Value, mName, 4);
WWHD_OFFSET(agl::parameter_tree::Value, mNameHash, 0x50);
WWHD_OFFSET(agl::parameter_tree::Value, mNext, 0x54);
WWHD_OFFSET(agl::parameter_tree::Value, mVtable, 0x58);
WWHD_SIZE(agl::parameter_tree::Value, 0x5C);
WWHD_OFFSET(agl::parameter_tree::Node, mChildren, 4);
WWHD_OFFSET(agl::parameter_tree::Node, mName, 8);
WWHD_OFFSET(agl::parameter_tree::Node, mNameHash, 0x54);
WWHD_OFFSET(agl::parameter_tree::Node, mNextSibling, 0x58);
WWHD_OFFSET(agl::parameter_tree::Node, mParent, 0x5C);
WWHD_OFFSET(agl::parameter_tree::Node, mTable, 0x60);
WWHD_SIZE(agl::parameter_tree::Node, 0x64);
WWHD_OFFSET(agl::parameter_tree::ValueVtable, mMatchesRecord, 0x3C);

WWHD_OFFSET(agl::parameter_tree::NodeMatchDispatch, mMatchesRecord, 0x2C);
WWHD_OFFSET(agl::parameter_tree::NodeReportDispatch, mParameterNotApplied, 0x30);
WWHD_OFFSET(agl::parameter_tree::NodeReportDispatch, mParameterNotBlended, 0x38);

WWHD_SIZE(agl::parameter_tree::InputRecord_l, 0x10);
WWHD_OFFSET(agl::parameter_tree::InputRecord_l, mChildCount, 8);
WWHD_OFFSET(agl::parameter_tree::InputRecord_l, mValueCount, 12);

WWHD_OFFSET(agl::parameter_tree::NodeApplyDispatch, mBeforeApply, 0x1C);
WWHD_OFFSET(agl::parameter_tree::NodeApplyDispatch, mAfterApply, 0x24);
