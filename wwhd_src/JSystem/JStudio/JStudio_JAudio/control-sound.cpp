// JStudio_JAudio object creation, reconstructed from WWHD using the tww control.cpp reference.
#include "JSystem/JStudio/JStudio_JAudio/control-sound.h"
using namespace JStudio_JAudio;
void JStudioSound_destroyBase(JStudio::TCreateObject*, u32);
void JStudioSound_delete(void*);
JStudio::TAdaptor* JStudioSound_constructAdaptor(JStudio::TAdaptor*, jst_adaptors::JAIBasic* basic);
JStudio::TObject* JStudioSound_constructObject(JStudio::TObject*, const JStudio::stb::data::TParse_TBlock&, JStudio::TAdaptor*);
void JStudioSound_registerGlobal(void*);
namespace {
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
}
/* 0283D9CC */
void TCreateObject_destroy(TCreateObject* creator, u32 flags) {
    if (!creator) return;
    JStudioSound_destroyBase(creator, 0);
    if (flags & 1) JStudioSound_delete(creator);
}
/* 0283DA20 */
static JStudio::TObject* createObject_SOUND(const JStudio::stb::data::TParse_TBlock& block, jst_adaptors::JAIBasic* basic) {
    JStudio::TAdaptor* adaptor = JStudioSound_constructAdaptor(nullptr, basic);
    if (!adaptor) return nullptr;
    JStudio::TObject* object = JStudioSound_constructObject(nullptr, block, adaptor);
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
/* 0283DAAC */
bool TCreateObject::create(gptr<JStudio::TObject>* output, const JStudio::stb::data::TParse_TBlock& block) {
    if (u32(block.raw->type) != 0x4A534E44) return false;
    *output = createObject_SOUND(block, mSound);
    return true;
}
/* 0283DB18 */
static void initializeFactory() {
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[0] = 0;
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[1] = 0;
    gabi::tail_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(&JStudioSound_registerGlobal)), &sStaticRegister);
}
/* WWHD glue
data sStaticBlock 104B60E8
data sStaticRegister 101FADC8
ext 028396BC void JStudioSound_destroyBase(JStudio::TCreateObject*, u32)
ext 0273AF40 void JStudioSound_delete(void*)
ext 0283DB40 JStudio::TAdaptor* JStudioSound_constructAdaptor(JStudio::TAdaptor*, jst_adaptors::JAIBasic* basic)
ext 0283BCD8 JStudio::TObject* JStudioSound_constructObject(JStudio::TObject*, const JStudio::stb::data::TParse_TBlock&, JStudio::TAdaptor*)
ext 028F026C void JStudioSound_registerGlobal(void*) callback
*/
