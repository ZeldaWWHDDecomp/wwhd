// STB lifecycle, using tww stb.cpp and WWHD's tail-vtable object layout.
#include "JSystem/JStudio/stb-companion.h"
using namespace JStudio::stb;
void* JStudioSTB_allocate(u32);
void JStudioSTB_delete(void*);
namespace {
static u32 sObjectVTable;
struct ObjectID { gptr<const void> id; be<u32> size; };
WWHD_SIZE(ObjectID, 8);
WWHD_OFFSET(ObjectID, size, 4);
static ObjectID* objectID(TObject* object) { return reinterpret_cast<ObjectID*>(object->id); }
}
/* 0283CB48 */
TObject* TObject_construct(TObject* object, u32 signature, const void* id, u32 size) {
    if (!object) object = static_cast<TObject*>(JStudioSTB_allocate(sizeof(TObject)));
    if (!object) return object;
    object->signature = signature;
    objectID(object)->size = size;
    object->bSequence_ = 0;
    object->mStatus = 0;
    object->__vtbl = reinterpret_cast<jst_timeline::ObjectVTable*>(&sObjectVTable);
    objectID(object)->id = id;
    object->ocObject_.next = nullptr;
    object->pSequence = nullptr;
    object->mFlag = 0;
    object->pControl = nullptr;
    object->pSequence_next = nullptr;
    object->ocObject_.prev = nullptr;
    object->_20 = 0;
    object->u32Wait_ = 0;
    return object;
}
/* 0283CBE4 */
TObject* TObject_constructBlock(TObject* object, const data::TParse_TBlock_object& block) {
    if (!object) object = static_cast<TObject*>(JStudioSTB_allocate(sizeof(TObject)));
    if (!object) return object;
    const data::ObjectBlock* raw = block.raw;
    u32 idSize = raw->idSize;
    object->ocObject_.prev = nullptr;
    object->__vtbl = reinterpret_cast<jst_timeline::ObjectVTable*>(&sObjectVTable);
    objectID(object)->id = raw->content;
    object->ocObject_.next = nullptr;
    object->pControl = nullptr;
    objectID(object)->size = idSize;
    object->signature = block.raw->type;
    object->bSequence_ = 0;
    object->mFlag = block.raw->flags;
    object->_20 = 0;
    object->pSequence = nullptr;
    const u8* sequence = reinterpret_cast<const u8*>(block.raw.get()) + ((u32(block.raw->idSize) + 3) & ~3u) + 0xC;
    object->mStatus = 0;
    object->u32Wait_ = 0;
    object->pSequence_next = sequence;
    return object;
}
/* 0283CC9C */
void TObject_destroy(TObject* object, u32 flags) {
    if (!object) return;
    object->__vtbl = reinterpret_cast<jst_timeline::ObjectVTable*>(&sObjectVTable);
    if (flags & 1) gabi::tail_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(&JStudioSTB_delete)), object);
}
/* 0283CD0C */
void TObject::reset(const void* sequence) {
    pSequence_next = static_cast<const u8*>(sequence);
    mStatus = 0;
    u32Wait_ = 0;
    bSequence_ = 0;
}
/* 0283D5C0 */
void TFactory_destroy(TFactory* factory, u32 flags) {
    if (factory && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(&JStudioSTB_delete)), factory);
}
/* 0283D5D4 */
void TFactory::destroy(TObject* object) {
    if (!object) return;
    struct DestroyVTable { u8 prefix[0xC]; gfn destroy; };
    auto* table = reinterpret_cast<DestroyVTable*>(object->__vtbl.get());
    gabi::tail_ptr<void>(table->destroy.get(), object, 3u);
}
/* 0283D954 */
void TObject::do_begin() {}
/* 0283D958 */
void TObject::do_end() {}
/* 0283D95C */
void TObject::do_paragraph(u32 type, const void* content, u32 size) {}
/* 0283D960 */
void TObject::do_wait(u32 frames) {}
/* 0283D964 */
void TObject::do_data(const void* id, u32 idSize, const void* content, u32 size) {}
/* 0283D9BC */
TObject* TFactory::create(const data::TParse_TBlock_object& block) { return nullptr; }
/* 0283D9C4 */
bool TParse::parseHeader(const data::TParse_THeader& header, u32 flags) { return true; }
/* WWHD glue
data sObjectVTable 10173214
ext 0273AD10 void* JStudioSTB_allocate(u32)
ext 0273AF40 void JStudioSTB_delete(void*) callback
*/
