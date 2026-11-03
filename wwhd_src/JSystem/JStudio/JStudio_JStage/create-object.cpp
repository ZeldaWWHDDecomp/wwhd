// Per-adaptor stage construction helpers, reconstructed from WWHD template instantiations.
#include "JSystem/JStudio/JStudio_JStage/create-object.h"
using namespace JStudio_JStage;
JStudio::TAdaptor* JStudioStageObject_constructActorAdaptor(JStudio::TAdaptor*, const StageSystem*, StageObject*);
JStudio::TObject* JStudioStageObject_constructActor(JStudio::TObject*, const JStudio::stb::data::TParse_TBlock&, JStudio::TAdaptor*);
JStudio::TAdaptor* JStudioStageObject_constructCameraAdaptor(JStudio::TAdaptor*, const StageSystem*, StageObject*);
JStudio::TObject* JStudioStageObject_constructCamera(JStudio::TObject*, const JStudio::stb::data::TParse_TBlock&, JStudio::TAdaptor*);
JStudio::TAdaptor* JStudioStageObject_constructAmbientAdaptor(JStudio::TAdaptor*, const StageSystem*, StageObject*);
JStudio::TObject* JStudioStageObject_constructAmbient(JStudio::TObject*, const JStudio::stb::data::TParse_TBlock&, JStudio::TAdaptor*);
JStudio::TAdaptor* JStudioStageObject_constructLightAdaptor(JStudio::TAdaptor*, const StageSystem*, StageObject*);
JStudio::TObject* JStudioStageObject_constructLight(JStudio::TObject*, const JStudio::stb::data::TParse_TBlock&, JStudio::TAdaptor*);
JStudio::TAdaptor* JStudioStageObject_constructFogAdaptor(JStudio::TAdaptor*, const StageSystem*, StageObject*);
JStudio::TObject* JStudioStageObject_constructFog(JStudio::TObject*, const JStudio::stb::data::TParse_TBlock&, JStudio::TAdaptor*);
static void prepareObject(JStudio::TObject* object) {
    if (!object) return;
    jst_timeline::TAdaptor* adaptor = object->mpAdaptor;
    if (adaptor) {
        auto* table = reinterpret_cast<JStudio::AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::call_ptr<void>(table->methods[JStudio::ADAPTOR_PREPARE].target.get(), adaptor, object);
    }
}
/* 0283F7B8 */
JStudio::TObject* JStudioStage_createActor(const JStudio::stb::data::TParse_TBlock& block, StageObject* object, const StageSystem* system) {
    JStudio::TAdaptor* adaptor = JStudioStageObject_constructActorAdaptor(nullptr, system, object);
    if (!adaptor) return nullptr;
    JStudio::TObject* result = JStudioStageObject_constructActor(nullptr, block, adaptor);
    prepareObject(result);
    return result;
}
/* 0283F850 */
JStudio::TObject* JStudioStage_createCamera(const JStudio::stb::data::TParse_TBlock& block, StageObject* object, const StageSystem* system) {
    JStudio::TAdaptor* adaptor = JStudioStageObject_constructCameraAdaptor(nullptr, system, object);
    if (!adaptor) return nullptr;
    JStudio::TObject* result = JStudioStageObject_constructCamera(nullptr, block, adaptor);
    prepareObject(result);
    return result;
}
/* 0283F8E8 */
JStudio::TObject* JStudioStage_createAmbient(const JStudio::stb::data::TParse_TBlock& block, StageObject* object, const StageSystem* system) {
    JStudio::TAdaptor* adaptor = JStudioStageObject_constructAmbientAdaptor(nullptr, system, object);
    if (!adaptor) return nullptr;
    JStudio::TObject* result = JStudioStageObject_constructAmbient(nullptr, block, adaptor);
    prepareObject(result);
    return result;
}
/* 0283F980 */
JStudio::TObject* JStudioStage_createLight(const JStudio::stb::data::TParse_TBlock& block, StageObject* object, const StageSystem* system) {
    JStudio::TAdaptor* adaptor = JStudioStageObject_constructLightAdaptor(nullptr, system, object);
    if (!adaptor) return nullptr;
    JStudio::TObject* result = JStudioStageObject_constructLight(nullptr, block, adaptor);
    prepareObject(result);
    return result;
}
/* 0283FA18 */
JStudio::TObject* JStudioStage_createFog(const JStudio::stb::data::TParse_TBlock& block, StageObject* object, const StageSystem* system) {
    JStudio::TAdaptor* adaptor = JStudioStageObject_constructFogAdaptor(nullptr, system, object);
    if (!adaptor) return nullptr;
    JStudio::TObject* result = JStudioStageObject_constructFog(nullptr, block, adaptor);
    prepareObject(result);
    return result;
}
/* WWHD glue
ext 0283FAB0 JStudio::TAdaptor* JStudioStageObject_constructActorAdaptor(JStudio::TAdaptor*, const JStudio_JStage::StageSystem*, JStudio_JStage::StageObject*)
ext 0283A760 JStudio::TObject* JStudioStageObject_constructActor(JStudio::TObject*, const JStudio::stb::data::TParse_TBlock&, JStudio::TAdaptor*)
ext 02840BC0 JStudio::TAdaptor* JStudioStageObject_constructCameraAdaptor(JStudio::TAdaptor*, const JStudio_JStage::StageSystem*, JStudio_JStage::StageObject*)
ext 0283AD7C JStudio::TObject* JStudioStageObject_constructCamera(JStudio::TObject*, const JStudio::stb::data::TParse_TBlock&, JStudio::TAdaptor*)
ext 028408C8 JStudio::TAdaptor* JStudioStageObject_constructAmbientAdaptor(JStudio::TAdaptor*, const JStudio_JStage::StageSystem*, JStudio_JStage::StageObject*)
ext 0283AC6C JStudio::TObject* JStudioStageObject_constructAmbient(JStudio::TObject*, const JStudio::stb::data::TParse_TBlock&, JStudio::TAdaptor*)
ext 02841910 JStudio::TAdaptor* JStudioStageObject_constructLightAdaptor(JStudio::TAdaptor*, const JStudio_JStage::StageSystem*, JStudio_JStage::StageObject*)
ext 0283B224 JStudio::TObject* JStudioStageObject_constructLight(JStudio::TObject*, const JStudio::stb::data::TParse_TBlock&, JStudio::TAdaptor*)
ext 028413B0 JStudio::TAdaptor* JStudioStageObject_constructFogAdaptor(JStudio::TAdaptor*, const JStudio_JStage::StageSystem*, JStudio_JStage::StageObject*)
ext 0283B0B0 JStudio::TObject* JStudioStageObject_constructFog(JStudio::TObject*, const JStudio::stb::data::TParse_TBlock&, JStudio::TAdaptor*)
*/
