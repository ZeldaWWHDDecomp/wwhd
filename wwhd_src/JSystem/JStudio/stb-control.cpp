// STB object ownership and control lifecycle, reconstructed from WWHD with the tww stb.cpp reference.
#include "JSystem/JStudio/stb-control.h"
using namespace JStudio::stb;
TObject* TObject_construct(TObject*, u32, const void*, u32);
void TObject_destroy(TObject*, u32);
void* JStudioSTBControl_allocate(u32);
void JStudioSTBControl_delete(void*);
void JStudioSTBControl_destroyList(JGadget::TNodeLinkList*, u32);
void JStudioSTBControl_insert(JGadget::TNodeLinkList*, gptr<JGadget::TLinkListNode>*, JGadget::TLinkListNode*);
void JStudioSTBControl_erase(JGadget::TNodeLinkList*, JGadget::TLinkListNode*);
bool JStudioSTBControl_equalID(TObject*, const JStudio::fvb::ObjectID*);
namespace {
static u32 sControlObjectVTable;
static u32 sControlVTable;
static JGadget::TNodeLinkList* objectList(TControl* control) {
    // The canonical flat timeline view names the count and the sentinel separately.
    return reinterpret_cast<JGadget::TNodeLinkList*>(&control->objectCount);
}
static JGadget::TLinkListNode* node(TObject* object) {
    return reinterpret_cast<JGadget::TLinkListNode*>(&object->ocObject_);
}
static TObject* nodeObject(JGadget::TLinkListNode* link) {
    return reinterpret_cast<TObject*>(reinterpret_cast<u8*>(link) - offsetof(TObject, ocObject_));
}
}
/* 0283D1B0 */
TObject_control* TObject_control_construct(TObject_control* object, const void* id, u32 size) {
    if (!object) object = static_cast<TObject_control*>(JStudioSTBControl_allocate(sizeof(TObject_control)));
    if (object) {
        TObject_construct(object, 0xFFFFFFFF, id, size);
        object->__vtbl = reinterpret_cast<jst_timeline::ObjectVTable*>(&sControlObjectVTable);
    }
    return object;
}
/* 0283D228 */
TControl* TControl_construct(TControl* control) {
    if (!control) control = static_cast<TControl*>(JStudioSTBControl_allocate(sizeof(TControl)));
    if (!control) return control;
    control->pFactory = nullptr;
    control->reservedState[1] = 0;
    control->__vtbl = &sControlVTable;
    control->reservedState[0] = 0;
    JGadget::TNodeLinkList* list = objectList(control);
    // GHS preserves the inlined list constructor's wrapped-null allocation path.
    if (gabi::ea(list) == 0) list = static_cast<JGadget::TNodeLinkList*>(JStudioSTBControl_allocate(sizeof(JGadget::TNodeLinkList)));
    if (list) {
        list->count = 0;
        list->sentinel.mPrev = &list->sentinel;
        list->sentinel.mNext = &list->sentinel;
    }
    TObject_control_construct(reinterpret_cast<TObject_control*>(&control->mObject_control), nullptr, 0);
    control->mStatus = 0;
    control->_54 = 0;
    control->mObject_control.pControl = reinterpret_cast<jst_timeline::TControl*>(control);
    return control;
}
/* 0283D2D0 */
void TControl_destroy(TControl* control, u32 flags) {
    if (!control) return;
    control->mObject_control.pControl = nullptr;
    control->__vtbl = &sControlVTable;
    TObject_destroy(reinterpret_cast<TObject*>(&control->mObject_control), 0);
    JStudioSTBControl_destroyList(objectList(control), 0);
    if (flags & 1) JStudioSTBControl_delete(control);
}
/* 0283D348 */
void TControl::appendObject(TObject* object) {
    object->pControl = reinterpret_cast<jst_timeline::TControl*>(this);
    gabi::Local<gptr<JGadget::TLinkListNode>> position;
    *position = &objectList(this)->sentinel;
    JStudioSTBControl_insert(objectList(this), position.get(), node(object));
}
/* 0283D384 */
void TControl::removeObject(TObject* object) {
    object->pControl = nullptr;
    gabi::tail_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(&JStudioSTBControl_erase)), objectList(this), node(object));
}
/* 0283D398 */
void TControl::destroyObject(TObject* object) {
    removeObject(object);
    TFactory* factory = static_cast<TFactory*>(pFactory.get());
    auto* table = reinterpret_cast<FactoryVTable*>(factory->__vtbl.get());
    gabi::call_ptr<void>(table->destroy.get(), factory, object);
}
/* 0283D3E8 */
void TControl::destroyObject_all() {
    if (!u32(objectList(this)->count)) return;
    auto* end = &objectList(this)->sentinel;
    do {
        auto* last = end->mPrev.get();
        destroyObject(nodeObject(last));
    } while (u32(objectList(this)->count));
}
/* 0283D448 */
TObject* TControl::getObject(const void* id, u32 size) {
    auto* end = &objectList(this)->sentinel;
    auto* current = end->mNext.get();
    gabi::Local<JStudio::fvb::ObjectID> key;
    key->id = id;
    key->size = size;
    while (current != end) {
        if (JStudioSTBControl_equalID(nodeObject(current), key.get())) break;
        current = current->mNext;
    }
    return current == end ? nullptr : nodeObject(current);
}
/* 0283D4C8 */
void TControl::reset() {
    auto* current = objectList(this)->sentinel.mNext.get();
    auto* end = &objectList(this)->sentinel;
    mStatus = 0;
    while (current != end) {
        TObject* object = nodeObject(current);
        current = current->mNext;
        object->reset(nullptr);
    }
}
/* 0283D968 */
void TObject_control_destroy(TObject_control* object, u32 flags) {
    if (!object) return;
    TObject_destroy(object, 0);
    if (flags & 1) JStudioSTBControl_delete(object);
}
/* WWHD glue
data sControlObjectVTable 101731D4
data sControlVTable 1017324C
ext 0273AD10 void* JStudioSTBControl_allocate(u32)
ext 0273AF40 void JStudioSTBControl_delete(void*)
ext 0284341C void JStudioSTBControl_destroyList(JGadget::TNodeLinkList*, u32)
ext 02843454 void JStudioSTBControl_insert(JGadget::TNodeLinkList*, gptr<JGadget::TLinkListNode>*, JGadget::TLinkListNode*)
ext 02843430 void JStudioSTBControl_erase(JGadget::TNodeLinkList*, JGadget::TLinkListNode*) callback
ext 0283C8E0 bool JStudioSTBControl_equalID(JStudio::stb::TObject*, const JStudio::fvb::ObjectID*)
*/
