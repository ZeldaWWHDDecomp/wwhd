// STB parser lifecycle and block dispatch, following tww stb.cpp and WWHD's version checks.
#include "JSystem/JStudio/stb-parser.h"
using namespace JStudio::stb;
void* JStudioSTBParse_allocate(u32);
void JStudioSTBParse_delete(void*);
void JStudioSTBParse_destroyBase(TParse*, u32);
void JStudioSTBParse_registerGlobal(void*);
namespace {
static u32 sParseVTable;
struct Signature { be<u8> bytes[4]; };
static const Signature ga4cSignature = {{0x53, 0x54, 0x42, 0}};
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
static const u8* sequence(const data::TParse_TBlock_object& block) {
    const data::ObjectBlock* raw = block.raw;
    return reinterpret_cast<const u8*>(raw) + ((u32(raw->idSize) + 3) & ~3u) + 0xC;
}
}
/* 0283D5F0 */
TParse* TParse_construct(TParse* parser, TControl* control) {
    if (!parser) parser = static_cast<TParse*>(JStudioSTBParse_allocate(sizeof(TParse)));
    if (parser) {
        parser->pControl = control;
        parser->__vtbl = &sParseVTable;
    }
    return parser;
}
/* 0283D640 */
void TParse_destroy(TParse* parser, u32 flags) {
    if (!parser) return;
    JStudioSTBParse_destroyBase(parser, 0);
    if (flags & 1) JStudioSTBParse_delete(parser);
}
/* 0283D694 */
u32 TParse::parseHeader_next(gptr<const u8>* input, be<u32>* blockCount, u32 flags) {
    const u8* raw = *input;
    gabi::Local<data::TParse_THeader> header;
    header->raw = reinterpret_cast<const data::THeader*>(raw);
    *input = raw + 0x20;
    const HeaderPrefix* prefix = reinterpret_cast<const HeaderPrefix*>(raw);
    *blockCount = prefix->blockNumber;
    for (u32 i = 0; i < 4; ++i) {
        if (u8(reinterpret_cast<const be<u8>*>(raw)[i]) != u8(ga4cSignature.bytes[i])) return 0;
    }
    if (u16(prefix->byteOrder) != 0xFEFF) return 0;
    u32 version = prefix->version;
    if (version - 1 >= 3) return 0;
    auto* table = reinterpret_cast<ParseVTable*>(__vtbl.get());
    return gabi::call_ptr<u32>(table->parseHeader.get(), this, header.get(), flags);
}
/* 0283D738 */
u32 TParse::parseBlock_next(gptr<const u8>* input, be<u32>* size, u32 flags) {
    const u8* raw = *input;
    gabi::Local<data::TParse_TBlock> block;
    block->raw = reinterpret_cast<const data::TBlock*>(raw);
    *input = raw + u32(reinterpret_cast<const data::TBlock*>(raw)->size);
    *size = reinterpret_cast<const data::TBlock*>(raw)->size;
    auto* table = reinterpret_cast<ParseVTable*>(__vtbl.get());
    return gabi::call_ptr<u32>(table->parseBlock.get(), this, block.get(), flags);
}
/* 0283D788 */
u32 TParse::parseBlock_block(const data::TParse_TBlock& block, u32 flags) {
    gabi::Local<data::TParse_TBlock_object> object;
    object->raw = reinterpret_cast<const data::ObjectBlock*>(block.raw.get());
    auto* table = reinterpret_cast<ParseVTable*>(__vtbl.get());
    return gabi::call_ptr<u32>(table->parseObject.get(), this, object.get(), flags);
}
/* 0283D7C0 */
u32 TParse::parseBlock_object(const data::TParse_TBlock_object& block, u32 flags) {
    const data::ObjectBlock* raw = block.raw;
    u32 type = raw->type;
    TControl* control = static_cast<TControl*>(pControl.get());
    if (type == 0xFFFFFFFF) {
        reinterpret_cast<TObject*>(&control->mObject_control)->reset(sequence(block));
        return 1;
    }
    if (flags & 0x10) {
        TObject* object = control->getObject(raw->content, raw->idSize);
        if (object) {
            object->reset(sequence(block));
            return 1;
        }
    }
    if (flags & 0x20) return 1;
    TFactory* factory = static_cast<TFactory*>(control->pFactory.get());
    if (factory) {
        auto* table = reinterpret_cast<FactoryVTable*>(factory->__vtbl.get());
        TObject* object = gabi::call_ptr<TObject*>(table->create.get(), factory, &block);
        if (object) {
            control->appendObject(object);
            return 1;
        }
        return (flags & 0x40) ? 1 : 0;
    }
    return 0;
}
/* 0283D92C */
static void initializeSTB() {
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[0] = 0;
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[1] = 0;
    gabi::tail_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(&JStudioSTBParse_registerGlobal)), &sStaticRegister);
}
/* WWHD glue
data sParseVTable 1017325C
data ga4cSignature 1017319C
data sStaticBlock 104B60D8
data sStaticRegister 101FADBC
ext 0273AD10 void* JStudioSTBParse_allocate(u32)
ext 0273AF40 void JStudioSTBParse_delete(void*)
ext 027ED8AC void JStudioSTBParse_destroyBase(JStudio::stb::TParse*, u32)
ext 028F026C void JStudioSTBParse_registerGlobal(void*) callback
*/
