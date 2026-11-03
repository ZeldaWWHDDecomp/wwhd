// Message object creation, from WWHD with the tww JStudio_JMessage/control.cpp reference.
#include "JSystem/JStudio/JStudio_JMessage/control-message.h"
using namespace JStudio_JMessage;
TAdaptor_message* TAdaptor_message_construct(TAdaptor_message*, JMessage::TControl*);
JStudio::TObject* JStudioMessage_constructObject(JStudio::TObject*, const JStudio::stb::data::TParse_TBlock&, JStudio::TAdaptor*);
void JStudioMessage_destroyCreator(JStudio::TCreateObject*, u32);
void JStudioMessageFactory_delete(void*);
void JStudioMessage_registerCreator(void*);
namespace {
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
}
/* 0283E31C */
static JStudio::TObject* createObject_MESSAGE(const JStudio::stb::data::TParse_TBlock& block, JMessage::TControl* control) {
    TAdaptor_message* adaptor = TAdaptor_message_construct(nullptr, control);
    if (!adaptor) return nullptr;
    JStudio::TObject* object = JStudioMessage_constructObject(nullptr, block, adaptor);
    if (object) {
        jst_timeline::TAdaptor* actual = object->mpAdaptor;
        if (actual) {
            auto* table = reinterpret_cast<JStudio::AdaptorVTable*>(actual->__vtbl.get());
            u32 prepare = table->methods[JStudio::ADAPTOR_PREPARE].target.get();
            gabi::call_ptr<void>(prepare, actual, object);
        }
    }
    return object;
}
/* 0283E3A8 */
bool TCreateObject_base::create(gptr<JStudio::TObject>* output, const JStudio::stb::data::TParse_TBlock& block) {
    if (u32(block.raw->type) != 0x4A4D5347) return false;
    auto* table = reinterpret_cast<CreateVTable*>(__vtbl.get());
    JMessage::TControl* system = gabi::call_ptr<JMessage::TControl*>(table->find.get(), this, &block);
    if (!system) return false;
    *output = createObject_MESSAGE(block, system);
    return true;
}
/* 0283E43C */
void TCreateObject_base_destroy(TCreateObject_base* creator, u32 flags) {
    if (!creator) return;
    JStudioMessage_destroyCreator(creator, 0);
    if (flags & 1) JStudioMessageFactory_delete(creator);
}
/* 0283E490 */
void TCreateObject_destroy(TCreateObject* creator, u32 flags) {
    if (!creator) return;
    TCreateObject_base_destroy(creator, 0);
    if (flags & 1) JStudioMessageFactory_delete(creator);
}
/* 0283E4E4 */
JMessage::TControl* TCreateObject::find(const JStudio::stb::data::TParse_TBlock& block) {
    return mControl;
}
/* 0283E4EC */
static void initializeFactory() {
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[0] = 0;
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[1] = 0;
    gabi::tail_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(&JStudioMessage_registerCreator)), &sStaticRegister);
}
/* WWHD glue
data sStaticBlock 104B6144
data sStaticRegister 101FAE04
ext 0283B654 JStudio::TObject* JStudioMessage_constructObject(JStudio::TObject*, const JStudio::stb::data::TParse_TBlock&, JStudio::TAdaptor*)
ext 028396BC void JStudioMessage_destroyCreator(JStudio::TCreateObject*, u32)
ext 0273AF40 void JStudioMessageFactory_delete(void*)
ext 028F026C void JStudioMessage_registerCreator(void*) callback
*/
