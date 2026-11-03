// JStudio function-value bank layouts, checked against Cafe constructors.
#pragma once
#include "JSystem/JGadget/linklist.h"
namespace JStudio { namespace fvb {
struct TControl;
namespace data {
struct TBlock { be<u32> size; be<u16> type, id_size; };
struct THeader { be<u8> signature[4]; be<u16> byte_order, version; be<u32> reserved, block_number; };
struct TParse_TBlock { gptr<const TBlock> raw; };
struct TParagraph { be<u32> size, type; gptr<const u8> content, next; };
struct TFloatData { be<f32> values[2]; };
struct TListData { be<f32> interval; be<u32> count; };
struct TParameterData { be<u32> count; };
}
struct FunctionValueVTable { u8 prefix[0x1C]; gfn getAttributeSet; u8 gap[12]; gfn initialize; };
struct FunctionValue { gptr<FunctionValueVTable> __vtbl; };
struct ObjectVTable { u8 prefix[0xC]; gfn destroy; u8 gap[4]; gfn prepare_data; };
struct TObject {
    gptr<const u8> pID;
    be<u32> id_size;
    JGadget::TLinkListNode mNode;
    gptr<FunctionValue> pfv_;
    gptr<ObjectVTable> __vtbl;
    void prepare(const data::TParse_TBlock&, TControl*);
};
struct ReferenceVector { u8 prefix[8]; gptr<void> end; u8 tail[8]; };
struct CompositeValue { FunctionValue base; ReferenceVector refer; be<u32> composite; u8 pad[4]; be<u64> data; };
struct ConstantValue { FunctionValue base; u8 pad[4]; be<f64> value; };
struct TransitionValue { FunctionValue base; u8 attributes[0x44]; be<f64> value_begin, value_end; };
struct ListValue { FunctionValue base; u8 attributes[0x40]; gptr<const be<f32>> values; be<u32> count; u8 pad[4]; be<f64> interval; u8 tail[8]; };
struct ParameterValue { FunctionValue base; u8 attributes[0x5C]; };
struct HermiteValue { FunctionValue base; u8 attributes[0x64]; };
struct TObject_composite : TObject { CompositeValue fnValue; void prepare_data_(const data::TParagraph&, TControl*); };
struct TObject_constant : TObject { ConstantValue fnValue; void prepare_data_(const data::TParagraph&, TControl*); };
struct TObject_transition : TObject { TransitionValue fnValue; void prepare_data_(const data::TParagraph&, TControl*); };
struct TObject_list : TObject { ListValue fnValue; void prepare_data_(const data::TParagraph&, TControl*); };
struct TObject_list_parameter : TObject { ParameterValue fnValue; void prepare_data_(const data::TParagraph&, TControl*); };
struct TObject_hermite : TObject { HermiteValue fnValue; void prepare_data_(const data::TParagraph&, TControl*); };
struct FactoryVTable { u8 prefix[0x14]; gfn create; u8 gap[4]; gfn destroy; };
struct TFactory { gptr<FactoryVTable> __vtbl; TObject* create(const data::TParse_TBlock&); void destroy(TObject*); };
struct TControl {
    gptr<TFactory> pFactory;
    JGadget::TNodeLinkList ocObject_;
    gptr<void> __vtbl;
    void appendObject(TObject*);
    void removeObject(TObject*);
    void destroyObject(TObject*);
    void destroyObject_all();
    TObject* getObject(const void*, u32);
    TObject* getObject_index(u32);
};
struct TParse { gptr<void> __vtbl; gptr<TControl> pControl_; bool parseHeader_next(gptr<const u8>*, be<u32>*, u32); bool parseBlock_next(gptr<const u8>*, be<u32>*, u32); };
struct ObjectID { gptr<const void> id; be<u32> size; };
struct AttributeRange { u8 prefix[0x18]; be<u8> progress, adjust; u8 middle[0x16]; be<u32> outside_begin, outside_end; };
struct AttributeSet { gptr<ReferenceVector> refer; gptr<AttributeRange> range; gptr<be<u32>> interpolate; };
struct CompositeOperation { be<u32> set, get; };
WWHD_SIZE(data::TBlock,8); WWHD_SIZE(data::THeader,16); WWHD_SIZE(data::TParse_TBlock,4); WWHD_SIZE(data::TParagraph,16);
WWHD_OFFSET(TObject,pID,0); WWHD_OFFSET(TObject,mNode,8); WWHD_OFFSET(TObject,pfv_,0x10); WWHD_OFFSET(TObject,__vtbl,0x14); WWHD_SIZE(TObject,0x18);
WWHD_SIZE(TObject_composite,0x40); WWHD_OFFSET(TObject_composite,fnValue,0x18);
WWHD_SIZE(TObject_constant,0x28); WWHD_OFFSET(TObject_constant,fnValue,0x18); WWHD_OFFSET(ConstantValue,value,8);
WWHD_SIZE(TObject_transition,0x70); WWHD_OFFSET(TransitionValue,value_begin,0x48); WWHD_OFFSET(TransitionValue,value_end,0x50);
WWHD_SIZE(TObject_list,0x78); WWHD_OFFSET(ListValue,values,0x44); WWHD_OFFSET(ListValue,count,0x48); WWHD_OFFSET(ListValue,interval,0x50);
WWHD_SIZE(TObject_list_parameter,0x78); WWHD_SIZE(TObject_hermite,0x80);
WWHD_SIZE(TControl,0x14); WWHD_OFFSET(TControl,ocObject_,4); WWHD_OFFSET(TControl,__vtbl,0x10);
WWHD_SIZE(TParse,8); WWHD_OFFSET(TParse,pControl_,4);
}}
