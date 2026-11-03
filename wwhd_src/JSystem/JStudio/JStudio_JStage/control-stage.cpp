// Stage object factory dispatch, reconstructed from WWHD with the tww control.cpp reference.
#include "JSystem/JStudio/JStudio_JStage/control-stage.h"
using namespace JStudio_JStage;
void JStudioStage_destroyBase(JStudio::TCreateObject*, u32);
void JStudioStage_delete(void*);
void JStudioStage_registerGlobal(void*);
JStudio::TObject* JStudioStage_createActor(const JStudio::stb::data::TParse_TBlock&, StageObject*, const StageSystem*);
JStudio::TObject* JStudioStage_createCamera(const JStudio::stb::data::TParse_TBlock&, StageObject*, const StageSystem*);
JStudio::TObject* JStudioStage_createAmbient(const JStudio::stb::data::TParse_TBlock&, StageObject*, const StageSystem*);
JStudio::TObject* JStudioStage_createLight(const JStudio::stb::data::TParse_TBlock&, StageObject*, const StageSystem*);
JStudio::TObject* JStudioStage_createFog(const JStudio::stb::data::TParse_TBlock&, StageObject*, const StageSystem*);
namespace {
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
}
/* 0283F490 */
void TCreateObject_destroy(TCreateObject* creator, u32 flags) {
    if (!creator) return;
    JStudioStage_destroyBase(creator, 0);
    if (flags & 1) JStudioStage_delete(creator);
}
/* 0283F4E4 */
bool TCreateObject::create(gptr<JStudio::TObject>* output, const JStudio::stb::data::TParse_TBlock& block) {
    using Creator = JStudio::TObject* (*)(const JStudio::stb::data::TParse_TBlock&, StageObject*, const StageSystem*);
    Creator creator;
    u32 type;
    switch (u32(block.raw->type)) {
    case BLOCK_ACTOR: creator = JStudioStage_createActor; type = OBJECT_ACTOR; break;
    case BLOCK_CAMERA: creator = JStudioStage_createCamera; type = OBJECT_CAMERA; break;
    case BLOCK_AMBIENT: creator = JStudioStage_createAmbient; type = OBJECT_AMBIENT; break;
    case BLOCK_LIGHT: creator = JStudioStage_createLight; type = OBJECT_LIGHT; break;
    case BLOCK_FOG: creator = JStudioStage_createFog; type = OBJECT_FOG; break;
    default: return false;
    }
    auto* table = reinterpret_cast<CreatorVTable*>(__vtbl.get());
    StageObject* object = gabi::call_ptr<StageObject*>(table->find.get(), this, &block, type);
    if (!object) return false;
    *output = creator(block, object, mSystem);
    return true;
}
/* 0283F700 */
StageObject* TCreateObject::find(const JStudio::stb::data::TParse_TBlock& block, u32 type) {
    const StageSystem* system = mSystem;
    const char* id = reinterpret_cast<const char*>(block.raw.get()) + 0xC;
    StageObject* object = gabi::call_ptr<StageObject*>(system->__vtbl->findObject.get(), system, id, type);
    if (!object) return nullptr;
    u32 actual = gabi::call_ptr<u32>(object->__vtbl->getType.get(), object);
    return actual == type ? object : nullptr;
}
/* 0283F790 */
static void initializeStageFactory() {
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[0] = 0;
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[1] = 0;
    gabi::tail_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(&JStudioStage_registerGlobal)), &sStaticRegister);
}
/* WWHD glue
data sStaticBlock 104B6220
data sStaticRegister 101FAE4C
ext 028396BC void JStudioStage_destroyBase(JStudio::TCreateObject*, u32)
ext 0273AF40 void JStudioStage_delete(void*)
ext 028F026C void JStudioStage_registerGlobal(void*) callback
*/
