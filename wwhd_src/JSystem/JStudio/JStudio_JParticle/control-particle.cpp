// JStudio_JParticle object creation, reconstructed from WWHD using the tww control.cpp reference.
#include "JSystem/JStudio/JStudio_JParticle/control-particle.h"
using namespace JStudio_JParticle;
void JStudioParticle_destroyBase(JStudio::TCreateObject*, u32);
void JStudioParticle_delete(void*);
JStudio::TAdaptor* JStudioParticle_constructAdaptor(JStudio::TAdaptor*, jst_adaptors::JPAEmitterManager* manager, const JStage::TSystem* system);
JStudio::TObject* JStudioParticle_constructObject(JStudio::TObject*, const JStudio::stb::data::TParse_TBlock&, JStudio::TAdaptor*);
void JStudioParticle_registerGlobal(void*);
namespace {
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
}
/* 0283E60C */
void TCreateObject_destroy(TCreateObject* creator, u32 flags) {
    if (!creator) return;
    JStudioParticle_destroyBase(creator, 0);
    if (flags & 1) JStudioParticle_delete(creator);
}
/* 0283E660 */
static JStudio::TObject* createObject_PARTICLE(const JStudio::stb::data::TParse_TBlock& block, jst_adaptors::JPAEmitterManager* manager, const JStage::TSystem* system) {
    JStudio::TAdaptor* adaptor = JStudioParticle_constructAdaptor(nullptr, manager, system);
    if (!adaptor) return nullptr;
    JStudio::TObject* object = JStudioParticle_constructObject(nullptr, block, adaptor);
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
/* 0283E6E8 */
bool TCreateObject::create(gptr<JStudio::TObject>* output, const JStudio::stb::data::TParse_TBlock& block) {
    if (u32(block.raw->type) != 0x4A505443) return false;
    *output = createObject_PARTICLE(block, mEmitterMgr, mSystem);
    return true;
}
/* 0283E75C */
static void initializeFactory() {
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[0] = 0;
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[1] = 0;
    gabi::tail_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(&JStudioParticle_registerGlobal)), &sStaticRegister);
}
/* WWHD glue
data sStaticBlock 104B6164
data sStaticRegister 101FAE1C
ext 028396BC void JStudioParticle_destroyBase(JStudio::TCreateObject*, u32)
ext 0273AF40 void JStudioParticle_delete(void*)
ext 0283E784 JStudio::TAdaptor* JStudioParticle_constructAdaptor(JStudio::TAdaptor*, jst_adaptors::JPAEmitterManager* manager, const JStage::TSystem* system)
ext 0283B708 JStudio::TObject* JStudioParticle_constructObject(JStudio::TObject*, const JStudio::stb::data::TParse_TBlock&, JStudio::TAdaptor*)
ext 028F026C void JStudioParticle_registerGlobal(void*) callback
*/
