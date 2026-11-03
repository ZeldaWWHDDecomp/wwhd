// Function-value bank object lifecycle, using the tww fvb.cpp reference.
#include "JSystem/JStudio/fvb.h"
#include <bit>
using namespace JStudio::fvb;
void* JStudio_allocate(u32);
void JStudio_delete(void*);
void JStudio_registerGlobalObject(void*);
void JStudio_destroyFunctionValue(FunctionValue*, u32);
void JStudio_destroyVector(void*, u32);
void JStudio_destroyList(JGadget::TNodeLinkList*, u32);
void JStudio_destroyParser(TParse*, u32);
bool JStudio_equalObjectID(TObject*, const ObjectID*);
void JStudio_insertNode(JGadget::TNodeLinkList*, gptr<JGadget::TLinkListNode>*, JGadget::TLinkListNode*);
void JStudio_eraseNode(JGadget::TNodeLinkList*, JGadget::TLinkListNode*);
void JStudio_parseFVBParagraph(gptr<const u8>*, data::TParagraph*);
void JStudio_insertReference(ReferenceVector*, void*, gptr<FunctionValue>*);
void JStudio_setRange(AttributeRange*, f32, f32);
void JStudio_setParameterData(ParameterValue*, const be<f32>*, u32);
void JStudio_setHermiteData(HermiteValue*, const be<f32>*, u32, u32);
namespace {
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
static u32 sObjectVTable;
static u32 sControlVTable;
static u32 sParseVTable;
static const CompositeOperation saCompositeOperation_[8] = {
    {0x00000000, 0x00000000},
    {0x02848D18, 0x028391A0},
    {0x02848D60, 0x028391C4},
    {0x02848F3C, 0x028391E8},
    {0x02848FA0, 0x02839204},
    {0x0284903C, 0x02839220},
    {0x02849150, 0x0283923C},
    {0x028491EC, 0x02839258},
};
struct Signature { be<u8> bytes[4]; };
static const Signature ga4cSignature = {{0x46,0x56,0x42,0}};
static u32 sCompositeVTable;
static u32 sConstantVTable;
static u32 sTransitionVTable;
static u32 sListVTable;
static u32 sListParameterVTable;
static u32 sHermiteVTable;
}
void JStudio_construct_composite(void*);
TObject_composite* TObject_composite_construct(TObject_composite*, const data::TParse_TBlock&);
void JStudio_construct_constant(void*);
TObject_constant* TObject_constant_construct(TObject_constant*, const data::TParse_TBlock&);
void JStudio_construct_transition(void*);
TObject_transition* TObject_transition_construct(TObject_transition*, const data::TParse_TBlock&);
void JStudio_construct_list(void*);
TObject_list* TObject_list_construct(TObject_list*, const data::TParse_TBlock&);
void JStudio_construct_list_parameter(void*);
TObject_list_parameter* TObject_list_parameter_construct(TObject_list_parameter*, const data::TParse_TBlock&);
void JStudio_construct_hermite(void*);
TObject_hermite* TObject_hermite_construct(TObject_hermite*, const data::TParse_TBlock&);

/* 0283852C */
void TObject_destroy(TObject* object, u32 flags) {
    if (!object) return;
    object->__vtbl = reinterpret_cast<ObjectVTable*>(&sObjectVTable);
    if (flags & 1) JStudio_delete(object);
}

// HD: explicit construction entry retains GHS allocation failure and field order.
/* 028388A4 */
TObject_composite* TObject_composite_construct(TObject_composite* object, const data::TParse_TBlock& block) {
    if (!object) object = static_cast<TObject_composite*>(JStudio_allocate(sizeof(TObject_composite)));
    if (!object) return object;
    const data::TBlock* raw = block.raw;
    u32 idSize = raw->id_size;
    const u8* id = idSize ? reinterpret_cast<const u8*>(raw + 1) : nullptr;
    object->pID = id;
    object->id_size = idSize;
    object->mNode.mNext = nullptr;
    object->pfv_ = reinterpret_cast<FunctionValue*>(&object->fnValue);
    object->__vtbl = reinterpret_cast<ObjectVTable*>(&sCompositeVTable);
    object->mNode.mPrev = nullptr;
    JStudio_construct_composite(&object->fnValue);
    return object;
}

// HD: explicit construction entry retains GHS allocation failure and field order.
/* 028389AC */
TObject_constant* TObject_constant_construct(TObject_constant* object, const data::TParse_TBlock& block) {
    if (!object) object = static_cast<TObject_constant*>(JStudio_allocate(sizeof(TObject_constant)));
    if (!object) return object;
    const data::TBlock* raw = block.raw;
    u32 idSize = raw->id_size;
    const u8* id = idSize ? reinterpret_cast<const u8*>(raw + 1) : nullptr;
    object->pID = id;
    object->id_size = idSize;
    object->mNode.mNext = nullptr;
    object->pfv_ = reinterpret_cast<FunctionValue*>(&object->fnValue);
    object->__vtbl = reinterpret_cast<ObjectVTable*>(&sConstantVTable);
    object->mNode.mPrev = nullptr;
    JStudio_construct_constant(&object->fnValue);
    return object;
}

// HD: explicit construction entry retains GHS allocation failure and field order.
/* 02838A4C */
TObject_transition* TObject_transition_construct(TObject_transition* object, const data::TParse_TBlock& block) {
    if (!object) object = static_cast<TObject_transition*>(JStudio_allocate(sizeof(TObject_transition)));
    if (!object) return object;
    const data::TBlock* raw = block.raw;
    u32 idSize = raw->id_size;
    const u8* id = idSize ? reinterpret_cast<const u8*>(raw + 1) : nullptr;
    object->pID = id;
    object->id_size = idSize;
    object->mNode.mNext = nullptr;
    object->pfv_ = reinterpret_cast<FunctionValue*>(&object->fnValue);
    object->__vtbl = reinterpret_cast<ObjectVTable*>(&sTransitionVTable);
    object->mNode.mPrev = nullptr;
    JStudio_construct_transition(&object->fnValue);
    return object;
}

// HD: explicit construction entry retains GHS allocation failure and field order.
/* 02838AF4 */
TObject_list* TObject_list_construct(TObject_list* object, const data::TParse_TBlock& block) {
    if (!object) object = static_cast<TObject_list*>(JStudio_allocate(sizeof(TObject_list)));
    if (!object) return object;
    const data::TBlock* raw = block.raw;
    u32 idSize = raw->id_size;
    const u8* id = idSize ? reinterpret_cast<const u8*>(raw + 1) : nullptr;
    object->pID = id;
    object->id_size = idSize;
    object->mNode.mNext = nullptr;
    object->pfv_ = reinterpret_cast<FunctionValue*>(&object->fnValue);
    object->__vtbl = reinterpret_cast<ObjectVTable*>(&sListVTable);
    object->mNode.mPrev = nullptr;
    JStudio_construct_list(&object->fnValue);
    return object;
}

// HD: explicit construction entry retains GHS allocation failure and field order.
/* 02838BA4 */
TObject_list_parameter* TObject_list_parameter_construct(TObject_list_parameter* object, const data::TParse_TBlock& block) {
    if (!object) object = static_cast<TObject_list_parameter*>(JStudio_allocate(sizeof(TObject_list_parameter)));
    if (!object) return object;
    const data::TBlock* raw = block.raw;
    u32 idSize = raw->id_size;
    const u8* id = idSize ? reinterpret_cast<const u8*>(raw + 1) : nullptr;
    object->pID = id;
    object->id_size = idSize;
    object->mNode.mNext = nullptr;
    object->pfv_ = reinterpret_cast<FunctionValue*>(&object->fnValue);
    object->__vtbl = reinterpret_cast<ObjectVTable*>(&sListParameterVTable);
    object->mNode.mPrev = nullptr;
    JStudio_construct_list_parameter(&object->fnValue);
    return object;
}

// HD: explicit construction entry retains GHS allocation failure and field order.
/* 02838C48 */
TObject_hermite* TObject_hermite_construct(TObject_hermite* object, const data::TParse_TBlock& block) {
    if (!object) object = static_cast<TObject_hermite*>(JStudio_allocate(sizeof(TObject_hermite)));
    if (!object) return object;
    const data::TBlock* raw = block.raw;
    u32 idSize = raw->id_size;
    const u8* id = idSize ? reinterpret_cast<const u8*>(raw + 1) : nullptr;
    object->pID = id;
    object->id_size = idSize;
    object->mNode.mNext = nullptr;
    object->pfv_ = reinterpret_cast<FunctionValue*>(&object->fnValue);
    object->__vtbl = reinterpret_cast<ObjectVTable*>(&sHermiteVTable);
    object->mNode.mPrev = nullptr;
    JStudio_construct_hermite(&object->fnValue);
    return object;
}

/* 02838A3C */
void JStudio::fvb::TObject_constant::prepare_data_(const data::TParagraph& paragraph, TControl* control) {
    const data::TFloatData* content = reinterpret_cast<const data::TFloatData*>(paragraph.content.get());
    fnValue.value = f64(f32(content->values[0]));
}
/* 02838ADC */
void JStudio::fvb::TObject_transition::prepare_data_(const data::TParagraph& paragraph, TControl* control) {
    const data::TFloatData* content = reinterpret_cast<const data::TFloatData*>(paragraph.content.get());
    f64 begin = f32(content->values[0]);
    f64 end = f32(content->values[1]);
    fnValue.value_begin = begin;
    fnValue.value_end = end;
}
/* 02838B84 */
void JStudio::fvb::TObject_list::prepare_data_(const data::TParagraph& paragraph, TControl* control) {
    const data::TListData* content = reinterpret_cast<const data::TListData*>(paragraph.content.get());
    fnValue.interval = f64(f32(content->interval));
    u32 count = content->count;
    fnValue.values = reinterpret_cast<const be<f32>*>(content + 1);
    fnValue.count = count;
}
/* 02838C34 */
void JStudio::fvb::TObject_list_parameter::prepare_data_(const data::TParagraph& paragraph, TControl* control) {
    const data::TParameterData* content = reinterpret_cast<const data::TParameterData*>(paragraph.content.get());
    u32 count = content->count;
    JStudio_setParameterData(&fnValue, reinterpret_cast<const be<f32>*>(content + 1), count);
}
/* 02838CD8 */
void JStudio::fvb::TObject_hermite::prepare_data_(const data::TParagraph& paragraph, TControl* control) {
    const data::TParameterData* content = reinterpret_cast<const data::TParameterData*>(paragraph.content.get());
    u32 packed = content->count;
    JStudio_setHermiteData(&fnValue, reinterpret_cast<const be<f32>*>(content + 1), packed & 0x0FFFFFFF, packed >> 28);
}

/* 0283929C */
void TObject_composite_destroy(TObject_composite* object, u32 flags) {
    if (!object) return;
    // HD: the embedded reference vector is destroyed before its function value base.
    JStudio_destroyVector(&object->fnValue.refer, 0);
    JStudio_destroyFunctionValue(reinterpret_cast<FunctionValue*>(&object->fnValue), 0);
    TObject_destroy(object, 0);
    if (flags & 1) JStudio_delete(object);
}

/* 02839314 */
void TObject_constant_destroy(TObject_constant* object, u32 flags) {
    if (!object) return;
    JStudio_destroyFunctionValue(reinterpret_cast<FunctionValue*>(&object->fnValue), 0);
    TObject_destroy(object, 0);
    if (flags & 1) JStudio_delete(object);
}

/* 02839374 */
void TObject_transition_destroy(TObject_transition* object, u32 flags) {
    if (!object) return;
    JStudio_destroyFunctionValue(reinterpret_cast<FunctionValue*>(&object->fnValue), 0);
    TObject_destroy(object, 0);
    if (flags & 1) JStudio_delete(object);
}

/* 028393D4 */
void TObject_list_destroy(TObject_list* object, u32 flags) {
    if (!object) return;
    JStudio_destroyFunctionValue(reinterpret_cast<FunctionValue*>(&object->fnValue), 0);
    TObject_destroy(object, 0);
    if (flags & 1) JStudio_delete(object);
}

/* 02839434 */
void TObject_list_parameter_destroy(TObject_list_parameter* object, u32 flags) {
    if (!object) return;
    JStudio_destroyFunctionValue(reinterpret_cast<FunctionValue*>(&object->fnValue), 0);
    TObject_destroy(object, 0);
    if (flags & 1) JStudio_delete(object);
}

/* 02839494 */
void TObject_hermite_destroy(TObject_hermite* object, u32 flags) {
    if (!object) return;
    JStudio_destroyFunctionValue(reinterpret_cast<FunctionValue*>(&object->fnValue), 0);
    TObject_destroy(object, 0);
    if (flags & 1) JStudio_delete(object);
}

// HD: compiler-generated initialization of this translation unit.
/* 02839274 */
static void __sinit_fvb_cpp() {
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[0] = 0;
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[1] = 0;
    JStudio_registerGlobalObject(&sStaticRegister);
}


/* 0283854C */
TObject* JStudio::fvb::TControl::getObject(const void* id, u32 size) {
    ObjectID equal;
    equal.id = id;
    equal.size = size;
    JGadget::TLinkListNode* end = &ocObject_.sentinel;
    JGadget::TLinkListNode* node = ocObject_.sentinel.mNext;
    while (node != end) {
        TObject* object = reinterpret_cast<TObject*>(reinterpret_cast<u8*>(node) - offsetof(TObject, mNode));
        if (JStudio_equalObjectID(object, &equal)) break;
        node = node->mNext;
    }
    return node == end ? nullptr : reinterpret_cast<TObject*>(reinterpret_cast<u8*>(node) - offsetof(TObject, mNode));
}
/* 028385CC */
TObject* JStudio::fvb::TControl::getObject_index(u32 index) {
    if (index >= u32(ocObject_.count)) return nullptr;
    JGadget::TLinkListNode* node = ocObject_.sentinel.mNext;
    for (u32 i = 0; i < index; ++i) node = node->mNext;
    return reinterpret_cast<TObject*>(reinterpret_cast<u8*>(node) - offsetof(TObject, mNode));
}
/* 02838CF4 */
TControl* TControl_construct(TControl* control) {
    if (!control) control = static_cast<TControl*>(JStudio_allocate(sizeof(TControl)));
    if (!control) return control;
    control->pFactory = nullptr;
    control->__vtbl = &sControlVTable;
    JGadget::TNodeLinkList* list = &control->ocObject_;
    // HD: retain the null-preserving inlined list-constructor allocation branch.
    if (!gabi::ea(list)) list = static_cast<JGadget::TNodeLinkList*>(JStudio_allocate(sizeof(JGadget::TNodeLinkList)));
    if (gabi::ea(list)) {
        JGadget::TLinkListNode* sentinel = &list->sentinel;
        list->count = 0;
        sentinel->mPrev = sentinel;
        sentinel->mNext = sentinel;
    }
    return control;
}
/* 02838D78 */
void TControl_destroy(TControl* control, u32 flags) {
    if (!control) return;
    control->__vtbl = &sControlVTable;
    JStudio_destroyList(&control->ocObject_, 0);
    if (flags & 1) JStudio_delete(control);
}
/* 02838DD8 */
void JStudio::fvb::TControl::appendObject(TObject* object) {
    gabi::Local<gptr<JGadget::TLinkListNode>> end;
    *end = &ocObject_.sentinel;
    JStudio_insertNode(&ocObject_, end.get(), &object->mNode);
}
/* 02838E10 */
void JStudio::fvb::TControl::removeObject(TObject* object) {
    JStudio_eraseNode(&ocObject_, &object->mNode);
}
/* 02838E1C */
void JStudio::fvb::TControl::destroyObject(TObject* object) {
    removeObject(object);
    TFactory* factory = pFactory;
    u32 destroy = factory->__vtbl.get()->destroy.get();
    gabi::call_ptr<void>(destroy, factory, object);
}
/* 02838E6C */
void JStudio::fvb::TControl::destroyObject_all() {
    while (u32(ocObject_.count) != 0) {
        JGadget::TLinkListNode* node = ocObject_.sentinel.mPrev;
        TObject* object = reinterpret_cast<TObject*>(reinterpret_cast<u8*>(node) - offsetof(TObject, mNode));
        destroyObject(object);
    }
}
/* 02838ECC */
void TFactory_destroy(TFactory* factory, u32 flags) {
    if (factory && (flags & 1)) JStudio_delete(factory);
}
/* 02838EE0 */
TObject* JStudio::fvb::TFactory::create(const data::TParse_TBlock& block) {
    switch (u16(block.raw.get()->type)) {
    case 1: return TObject_composite_construct(nullptr, block);
    case 2: return TObject_constant_construct(nullptr, block);
    case 3: return TObject_transition_construct(nullptr, block);
    case 4: return TObject_list_construct(nullptr, block);
    case 5: return TObject_list_parameter_construct(nullptr, block);
    case 6: return TObject_hermite_construct(nullptr, block);
    default: return nullptr;
    }
}
/* 02838F5C */
void JStudio::fvb::TFactory::destroy(TObject* object) {
    if (object) {
        u32 destroy = object->__vtbl.get()->destroy.get();
        gabi::tail_ptr<void>(destroy, object, 3u);
    }
}
/* 02838F78 */
TParse* TParse_construct(TParse* parser, TControl* control) {
    // HD: this address is a constructor, despite the GameCube matcher label.
    if (!parser) parser = static_cast<TParse*>(JStudio_allocate(sizeof(TParse)));
    if (parser) {
        parser->pControl_ = control;
        parser->__vtbl = &sParseVTable;
    }
    return parser;
}
/* 02838FC8 */
void TParse_destroy(TParse* parser, u32 flags) {
    if (!parser) return;
    JStudio_destroyParser(parser, 0);
    if (flags & 1) JStudio_delete(parser);
}
/* 0283901C */
bool JStudio::fvb::TParse::parseHeader_next(gptr<const u8>* cursor, be<u32>* blocks, u32 flags) {
    const data::THeader* header = reinterpret_cast<const data::THeader*>(cursor->get());
    *cursor = reinterpret_cast<const u8*>(header + 1);
    *blocks = u32(header->block_number);
    // HD: release build has no warning callbacks on malformed headers.
    for (u32 i = 0; i < 4; ++i)
        if (u8(header->signature[i]) != u8(ga4cSignature.bytes[i])) return false;
    if (u16(header->byte_order) != 0xFEFF) return false;
    return u32(u16(header->version) - 2) < 0xFF;
}


/* 02839084 */
bool JStudio::fvb::TParse::parseBlock_next(gptr<const u8>* cursor, be<u32>* size, u32 flags) {
    data::TParse_TBlock block;
    const data::TBlock* raw = reinterpret_cast<const data::TBlock*>(cursor->get());
    block.raw = raw;
    *cursor = reinterpret_cast<const u8*>(raw) + u32(raw->size);
    *size = u32(raw->size);
    TControl* control = pControl_;
    if (flags & 0x10) {
        u32 idSize = raw->id_size;
        const void* id = idSize ? raw + 1 : nullptr;
        if (control->getObject(id, idSize)) return true;
    }
    if (flags & 0x20) return true;
    TFactory* factory = control->pFactory;
    if (!factory) return false;
    u32 create = factory->__vtbl.get()->create.get();
    TObject* object = gabi::call_ptr<TObject*>(create, factory, &block);
    if (!object) return (flags & 0x40) != 0;
    object->prepare(block, control);
    control->appendObject(object);
    return true;
}

/* 02838934 */
static const CompositeOperation* getCompositeOperation_(u32 operation) {
    return &saCompositeOperation_[operation];
}
/* 02838948 */
void JStudio::fvb::TObject_composite::prepare_data_(const data::TParagraph& paragraph, TControl* control) {
    const be<u32>* content = reinterpret_cast<const be<u32>*>(paragraph.content.get());
    const CompositeOperation* operation = getCompositeOperation_(u32(content[0]));
    u32 get = u32(operation->get);
    u32 set = operation->set;
    auto value = gabi::call_ptr<Pair32>(get, content + 1);
    fnValue.composite = set;
    fnValue.data = (u64(value.r3) << 32) | value.r4;
}

/* 028391A0 */
static Pair32 getCompositeData_raw_(const be<u32>* content) {
    // HD: the union's unused low word retains the incoming stack contents.
    gabi::AutoFrame frame{gabi::current_function()};
    struct DataWords { be<u32> high, low; };
    gabi::FrameLocal<DataWords> value(0x10);
    value->high = u32(*content);
    return {u32(value->high), u32(value->low)};
}

/* 028391C4 */
static Pair32 getCompositeData_index_(const be<u32>* content) {
    // HD: the union's unused low word retains the incoming stack contents.
    gabi::AutoFrame frame{gabi::current_function()};
    struct DataWords { be<u32> high, low; };
    gabi::FrameLocal<DataWords> value(0x10);
    value->high = u32(*content);
    return {u32(value->high), u32(value->low)};
}

/* 028391E8 */
static Pair32 getCompositeData_parameter_(const be<f32>* content) {
    u64 bits = std::bit_cast<u64>(gabi::fpr_from_f32(f32(*content))) /* lfs + stfd: bit-exact */;
    return {u32(bits >> 32), u32(bits)};
}

/* 02839204 */
static Pair32 getCompositeData_add_(const be<f32>* content) {
    u64 bits = std::bit_cast<u64>(gabi::fpr_from_f32(f32(*content))) /* lfs + stfd: bit-exact */;
    return {u32(bits >> 32), u32(bits)};
}

/* 02839220 */
static Pair32 getCompositeData_subtract_(const be<f32>* content) {
    u64 bits = std::bit_cast<u64>(gabi::fpr_from_f32(f32(*content))) /* lfs + stfd: bit-exact */;
    return {u32(bits >> 32), u32(bits)};
}

/* 0283923C */
static Pair32 getCompositeData_multiply_(const be<f32>* content) {
    u64 bits = std::bit_cast<u64>(gabi::fpr_from_f32(f32(*content))) /* lfs + stfd: bit-exact */;
    return {u32(bits >> 32), u32(bits)};
}

/* 02839258 */
static Pair32 getCompositeData_divide_(const be<f32>* content) {
    u64 bits = std::bit_cast<u64>(gabi::fpr_from_f32(f32(*content))) /* lfs + stfd: bit-exact */;
    return {u32(bits >> 32), u32(bits)};
}


/* 02838600 */
void JStudio::fvb::TObject::prepare(const data::TParse_TBlock& block, TControl* control) {
    AttributeSet attributes;
    FunctionValue* value = pfv_;
    u32 getAttributes = value->__vtbl.get()->getAttributeSet.get();
    gabi::call_ptr<void>(getAttributes, &attributes, value);
    const data::TBlock* raw = block.raw;
    u32 idSize = raw->id_size;
    const u8* end = reinterpret_cast<const u8*>(raw) + u32(raw->size);
    const u8* content = reinterpret_cast<const u8*>(raw + 1) + ((idSize + 3) & ~3u);
    ReferenceVector* refer = attributes.refer;
    AttributeRange* range = attributes.range;
    be<u32>* interpolate = attributes.interpolate;
    gabi::Local<gptr<const u8>> cursor;
    data::TParagraph paragraph;
    gabi::Local<gptr<FunctionValue>> referenceByID;
    gabi::Local<gptr<FunctionValue>> referenceByIndex;
    bool finished = false;
    while (gabi::ea(content) < gabi::ea(end) && !finished) {
        *cursor = content;
        JStudio_parseFVBParagraph(cursor.get(), &paragraph);
        u32 type = paragraph.type;
        const be<u32>* words = reinterpret_cast<const be<u32>*>(paragraph.content.get());
        switch (type) {
        case 0:
            finished = true;
            break;
        case 1: {
            u32 prepareData = __vtbl.get()->prepare_data.get();
            gabi::call_ptr<void>(prepareData, this, &paragraph, control);
            break;
        }
        case 0x10:
            if (refer) {
                u32 count = words[0];
                const be<u32>* item = words + 1;
                for (u32 i = 0; i < count; ++i) {
                    u32 size = item[0];
                    TObject* object = control->getObject(item + 1, size);
                    if (object) {
                        *referenceByID = object->pfv_.get();
                        void* last = refer->end;
                        JStudio_insertReference(refer, last, referenceByID.get());
                    }
                    item = reinterpret_cast<const be<u32>*>(reinterpret_cast<const u8*>(item + 1) + ((size + 3) & ~3u));
                }
            }
            break;
        case 0x11:
            if (refer) {
                u32 count = words[0];
                for (u32 i = 0; i < count; ++i) {
                    TObject* object = control->getObject_index(u32(words[i + 1]));
                    if (object) {
                        *referenceByIndex = object->pfv_.get();
                        void* last = refer->end;
                        JStudio_insertReference(refer, last, referenceByIndex.get());
                    }
                }
            }
            break;
        case 0x12:
            if (range) {
                const data::TFloatData* values = reinterpret_cast<const data::TFloatData*>(words);
                JStudio_setRange(range, values->values[0], values->values[1]);
            }
            break;
        case 0x13:
            if (range) range->progress = u32(words[0]);
            break;
        case 0x14:
            if (range) range->adjust = u32(words[0]);
            break;
        case 0x15:
            if (range) {
                const be<u16>* values = reinterpret_cast<const be<u16>*>(words);
                u32 begin = values[0];
                u32 finish = values[1];
                range->outside_begin = begin;
                range->outside_end = finish;
            }
            break;
        case 0x16:
            if (interpolate) *interpolate = u32(words[0]);
            break;
        }
        content = paragraph.next;
    }
    value = pfv_;
    u32 initialize = value->__vtbl.get()->initialize.get();
    gabi::call_ptr<void>(initialize, value);
}

/* WWHD glue
data sStaticBlock 104B600C
data sStaticRegister 101FAD14
data sObjectVTable 10172B74
data sControlVTable 10172C64
data sParseVTable 10172C94
data ga4cSignature 1017B324
data saCompositeOperation_ 10172B8C
ext 0273AD10 void* JStudio_allocate(u32)
ext 0273AF40 void JStudio_delete(void*)
ext 028F026C void JStudio_registerGlobalObject(void*)
ext 02848928 void JStudio_destroyFunctionValue(FunctionValue*, u32)
ext 02843544 void JStudio_destroyVector(void*, u32)
ext 0284341C void JStudio_destroyList(JGadget::TNodeLinkList*, u32)
ext 027ED8AC void JStudio_destroyParser(TParse*, u32)
ext 02843454 void JStudio_insertNode(JGadget::TNodeLinkList*, gptr<JGadget::TLinkListNode>*, JGadget::TLinkListNode*)
ext 02843430 void JStudio_eraseNode(JGadget::TNodeLinkList*, JGadget::TLinkListNode*)
ext 0284A854 void JStudio_parseFVBParagraph(gptr<const u8>*, data::TParagraph*)
ext 0284364C void JStudio_insertReference(ReferenceVector*, void*, gptr<FunctionValue>*)
ext 028489D8 void JStudio_setRange(AttributeRange*, f32, f32)
ext 02849C9C void JStudio_setParameterData(ParameterValue*, const be<f32>*, u32)
ext 0284A360 void JStudio_setHermiteData(HermiteValue*, const be<f32>*, u32, u32)
data sCompositeVTable 10172BD4
ext 02848C68 void JStudio_construct_composite(void*)
data sConstantVTable 10172BEC
ext 02849300 void JStudio_construct_constant(void*)
data sTransitionVTable 10172C04
ext 0284937C void JStudio_construct_transition(void*)
data sListVTable 10172C1C
ext 0284966C void JStudio_construct_list(void*)
data sListParameterVTable 10172C34
ext 02849C04 void JStudio_construct_list_parameter(void*)
data sHermiteVTable 10172C4C
ext 0284A2C4 void JStudio_construct_hermite(void*)
*/
