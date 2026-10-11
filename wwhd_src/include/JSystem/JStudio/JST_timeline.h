// JStudio timeline layouts. HD moves virtual tables to the object tails.
#pragma once
#include "wwhd.h"
namespace jst_timeline {
struct TAdaptor;
struct TControl;
struct TObject;
struct OutputVTable {
    u8 prefix[0xC];
    gfn output;
};
struct TOutput {
    gptr<OutputVTable> __vtbl;
};
struct FunctionVTable {
    u8 prefix[0x34];
    gfn getValue;
};
struct TFunctionValue {
    gptr<FunctionVTable> __vtbl;
};
struct TVariableValue {
    union {
        be<f32> mValue;
        be<u32> mValueBits;
    };
    be<u32> field_0x4;
    gfn field_0x8;
    union {
        gptr<TFunctionValue> fv;
        be<f32> val;
        be<u32> bits;
    } field_0xc;
    gptr<TOutput> pOutput_;
    void update(f64, TAdaptor*);
};
struct AdaptorVTable {
    u8 prefix[0x2C];
    gfn adaptor_do_update;
};
struct TAdaptor {
    gptr<TVariableValue> mVariableValues;
    be<u32> mCount;
    gptr<AdaptorVTable> __vtbl;
    void adaptor_updateVariableValue(TObject*, u32);
};
struct ObjectVTable {
    u8 prefix[0x14];
    gfn do_begin;
    u8 reserved18[4];
    gfn do_end;
    u8 reserved20[4];
    gfn do_paragraph;
    u8 reserved28[4];
    gfn do_wait;
    u8 reserved30[4];
    gfn do_data;
};
struct TLinkListNode {
    gptr<TLinkListNode> next, prev;
};
namespace stb {
struct TObject {
    enum TEStatus : u32 {
        STATUS_STILL = 0,
        STATUS_END = 1,
        STATUS_WAIT = 2,
        STATUS_SUSPEND = 4,
        STATUS_INACTIVE = 8
    };
    enum : u16 { FLAG_INACTIVE = 0x8000 };
    u8 id[8];
    TLinkListNode ocObject_;
    gptr<TControl> pControl;
    be<u32> signature;
    be<u16> mFlag;
    be<u8> bSequence_;
    u8 pad1B;
    be<u32> _20;
    gptr<const u8> pSequence, pSequence_next;
    be<u32> u32Wait_, mStatus;
    gptr<ObjectVTable> __vtbl;
    void setFlag_operation(u32, s32);
    void process_paragraph_reserved_(u32, const void*, u32);
    void process_sequence_();
    bool forward(u32);
};
} // namespace stb
struct TObject : stb::TObject {
    gptr<TAdaptor> mpAdaptor;
    void forward_value(u32);
};
struct TControl {
    u8 prefix[0xC];
    be<u32> objectCount;
    TLinkListNode mObjectContainer;
    be<u32> mStatus;
    stb::TObject mObject_control;
    be<s32> _54;
    u8 pad54[4];
    be<f64> mSecondPerFrame;
    bool forward(u32);
};
struct SequenceData {
    be<u8> type;
    u8 pad[3];
    be<u32> param, content, next;
};
struct ParagraphData {
    be<u32> type, size, content, next;
};
struct ParagraphContent {
    union {
        be<u32> word;
        struct {
            be<u16> reserved, idSize;
        } id;
    };
};
enum Command : u32 {
    CMD_FLAG = 1,
    CMD_WAIT = 2,
    CMD_JUMP = 3,
    CMD_SUSPEND = 4,
    CMD_PARAGRAPH = 0x80,
    CMD_DATA_ID = 0x81
};
enum FlagOperation : u32 { FLAG_OR = 1, FLAG_AND = 2, FLAG_XOR = 3 };
// HD virtual dispatch retains the original ABI and explicit tail transfers.
template <class R = void, class... A> inline R invoke(u32 fn, A... args) {
    return gabi::call_ptr<R>(fn, args...);
}
template <class... A> inline void transfer(u32 fn, A... args) { gabi::tail_ptr<void>(fn, args...); }
inline const u8* sequenceOffset(const u8* seq, u32 offset) {
    return gabi::at<const u8>(gabi::ea(seq) + offset);
}

WWHD_SIZE(TVariableValue, 0x14);
WWHD_OFFSET(TVariableValue, field_0x4, 4);
WWHD_OFFSET(TVariableValue, field_0x8, 8);
WWHD_OFFSET(TVariableValue, field_0xc, 0xC);
WWHD_OFFSET(TVariableValue, pOutput_, 0x10);
WWHD_SIZE(TAdaptor, 0xC);
WWHD_OFFSET(TAdaptor, __vtbl, 8);
WWHD_SIZE(stb::TObject, 0x34);
WWHD_OFFSET(stb::TObject, ocObject_, 8);
WWHD_OFFSET(stb::TObject, pControl, 0x10);
WWHD_OFFSET(stb::TObject, mFlag, 0x18);
WWHD_OFFSET(stb::TObject, bSequence_, 0x1A);
WWHD_OFFSET(stb::TObject, _20, 0x1C);
WWHD_OFFSET(stb::TObject, pSequence, 0x20);
WWHD_OFFSET(stb::TObject, pSequence_next, 0x24);
WWHD_OFFSET(stb::TObject, u32Wait_, 0x28);
WWHD_OFFSET(stb::TObject, mStatus, 0x2C);
WWHD_OFFSET(stb::TObject, __vtbl, 0x30);
WWHD_OFFSET(TObject, mpAdaptor, 0x34);
WWHD_SIZE(TObject, 0x38);
WWHD_OFFSET(TControl, mObjectContainer, 0x10);
WWHD_OFFSET(TControl, mStatus, 0x18);
WWHD_OFFSET(TControl, mObject_control, 0x1C);
WWHD_OFFSET(TControl, _54, 0x50);
WWHD_OFFSET(TControl, mSecondPerFrame, 0x58);
WWHD_SIZE(TControl, 0x60);
WWHD_SIZE(SequenceData, 0x10);
WWHD_SIZE(ParagraphData, 0x10);
WWHD_SIZE(ParagraphContent, 4);
WWHD_OFFSET(ObjectVTable, do_begin, 0x14);
WWHD_OFFSET(ObjectVTable, do_end, 0x1C);
WWHD_OFFSET(ObjectVTable, do_paragraph, 0x24);
WWHD_OFFSET(ObjectVTable, do_wait, 0x2C);
WWHD_OFFSET(ObjectVTable, do_data, 0x34);
} // namespace jst_timeline

// HD: free callback entry points preserve static signatures until autoglue supports static methods.
void TVariableValue_update_immediate_(jst_timeline::TVariableValue*, f64);
void TVariableValue_update_time_(jst_timeline::TVariableValue*, f64);
void TVariableValue_update_functionValue_(jst_timeline::TVariableValue*, f64);
void parseSequence(be<u32>*, jst_timeline::SequenceData*);
void parseParagraph(be<u32>*, jst_timeline::ParagraphData*);
