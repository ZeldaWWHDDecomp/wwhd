// JStudio timeline objects and adaptor operations, using tww jstudio-object.cpp.
#include "JSystem/JStudio/jstudio-object.h"
#include "sead/ElementAt.h"
#include "JSystem/JStudio/JST_adaptors.h"
using namespace JStudio;
void* JStudio_allocate(u32);
void JStudio_delete(void*);
void JStudio_constructSTBObject(TObject*, const stb::data::TParse_TBlock&);
void JStudio_destroySTBObject(TObject*, u32);
void JStudio_objectRegisterGlobalObject(void*);
// The shared default output object (104B605C): its vtable is sOutputNoneVTable; exported in jstudio-object.h.
static jst_timeline::TOutput sJStudioOutputNone;
static const jst_adaptors::Index4 sJStudioLightColorIndices = {{0,1,2,3}};
static const jst_adaptors::Index3 sJStudioLightPositionIndices = {{4,5,6}};
static const jst_adaptors::Index3 sJStudioLightTargetIndices = {{7,8,9}};
namespace {
static u32 sactorObjectVTable;
static u32 sambientLightObjectVTable;
static u32 scameraObjectVTable;
static u32 sfogObjectVTable;
static u32 slightObjectVTable;
static u32 smessageObjectVTable;
static u32 sparticleObjectVTable;
static u32 ssoundObjectVTable;
static u32 sObjectVTable;
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
struct IndexTable2 { be<u32> values[2]; };
struct IndexTable3 { be<u32> values[3]; };
struct IndexTable4 { be<u32> values[4]; };
static const IndexTable3 sauVariableValue_actor_TRANSLATION_XYZ = {{3,4,5}};
static const IndexTable3 sauVariableValue_actor_ROTATION_XYZ = {{6,7,8}};
static const IndexTable3 sauVariableValue_actor_SCALING_XYZ = {{9,10,11}};
static const IndexTable3 sauVariableValue_ambientLight_COLOR_RGB = {{0,1,2}};
static const IndexTable4 sauVariableValue_ambientLight_COLOR_RGBA = {{0,1,2,3}};
static const IndexTable3 sauVariableValue_camera_POSITION_XYZ = {{0,1,2}};
static const IndexTable3 sauVariableValue_camera_TARGET_POSITION_XYZ = {{3,4,5}};
static const IndexTable2 sauVariableValue_camera_DISTANCE_NEAR_FAR = {{8,9}};
static const IndexTable3 sauVariableValue_fog_COLOR_RGB = {{0,1,2}};
static const IndexTable4 sauVariableValue_fog_COLOR_RGBA = {{0,1,2,3}};
static const IndexTable2 sauVariableValue_fog_RANGE_BEGIN_END = {{4,5}};
static const IndexTable3 sauVariableValue_light_COLOR_RGB = {{0,1,2}};
static const IndexTable2 sauVariableValue_light_DIRECTION_THETA_PHI = {{10,11}};
static const IndexTable3 sauVariableValue_particle_TRANSLATION_XYZ = {{0,1,2}};
static const IndexTable3 sauVariableValue_particle_ROTATION_XYZ = {{3,4,5}};
static const IndexTable3 sauVariableValue_particle_SCALING_XYZ = {{6,7,8}};
static const IndexTable3 sauVariableValue_particle_COLOR_RGB = {{9,10,11}};
static const IndexTable4 sauVariableValue_particle_COLOR_RGBA = {{9,10,11,12}};
static const IndexTable3 sauVariableValue_particle_COLOR1_RGB = {{9,10,11}};
static const IndexTable4 sauVariableValue_particle_COLOR1_RGBA = {{9,10,11,12}};
static const IndexTable3 sauVariableValue_sound_POSITION_XYZ = {{0,1,2}};

static u32 sOutputNoneVTable;
static BoolOutput sactorParentOutput;
static u32 sactorParentRegistration;
static BoolOutput sactorRelationOutput;
static u32 sactorRelationRegistration;
static BoolOutput scameraParentOutput;
static u32 scameraParentRegistration;
static BoolOutput slightEnableOutput;
static u32 slightEnableRegistration;
static BoolOutput sparticleParentOutput;
static u32 sparticleParentRegistration;
static BoolOutput ssoundLocatedOutput;
static u32 ssoundLocatedRegistration;
static u32 sactorOutputVTable;
static u32 scameraOutputVTable;
static u32 slightOutputVTable;
static u32 sparticleOutputVTable;
static u32 ssoundOutputVTable;

}

/* 02839E5C */
void TVariableValue_TOutput_destroy(jst_timeline::TOutput* output, u32 flags) {
    if (output && (flags & 1)) JStudio_delete(output);
}
/* 02839E70 */
void JStudio::TAdaptor::adaptor_setVariableValue_VOID_(TAdaptor* adaptor, TObject* object, u32 index, const void* content, u32 size) {
    (&sead::elementAt(adaptor->mVariableValues, index))->field_0x8.v = 0;
}
/* 02839E88 */
void JStudio::TAdaptor::adaptor_setVariableValue_IMMEDIATE_(TAdaptor* adaptor, TObject* object, u32 index, const void* content, u32 size) {
    TVariableValue* value = &sead::elementAt(adaptor->mVariableValues, index);
    // HD: lfs/stfs without arithmetic preserves signalling-NaN bits.
    u32 immediate = *static_cast<const be<u32>*>(content);
    value->field_0x8.v = gabi::guest_fn(reinterpret_cast<const void*>(TVariableValue_update_immediate_));
    value->field_0x4 = 0;
    value->field_0xc.bits = immediate;
}
/* 02839EB4 */
void JStudio::TAdaptor::adaptor_setVariableValue_TIME_(TAdaptor* adaptor, TObject* object, u32 index, const void* content, u32 size) {
    TVariableValue* value = &sead::elementAt(adaptor->mVariableValues, index);
    // HD: the time parameter is copied bitwise before later arithmetic uses it.
    u32 time = *static_cast<const be<u32>*>(content);
    value->field_0x8.v = gabi::guest_fn(reinterpret_cast<const void*>(TVariableValue_update_time_));
    value->field_0x4 = 0;
    value->field_0xc.bits = time;
}
/* 02839EE0 */
void JStudio::TAdaptor::adaptor_setVariableValue_FVR_NAME_(TAdaptor* adaptor, TObject* object, u32 index, const void* content, u32 size) {
    auto control = reinterpret_cast<TControl*>(object->pControl.get());
    TVariableValue* value = &sead::elementAt(adaptor->mVariableValues, index);
    fvb::TObject* bankObject = control->fvb_Control.getObject(content, size);
    fvb::FunctionValue* function = bankObject ? bankObject->pfv_.get() : nullptr;
    value->field_0xc.fv = reinterpret_cast<jst_timeline::TFunctionValue*>(function);
    value->field_0x4 = 0;
    value->field_0x8.v = gabi::guest_fn(reinterpret_cast<const void*>(TVariableValue_update_functionValue_));
}
/* 02839F48 */
void JStudio::TAdaptor::adaptor_setVariableValue_FVR_INDEX_(TAdaptor* adaptor, TObject* object, u32 index, const void* content, u32 size) {
    auto control = reinterpret_cast<TControl*>(object->pControl.get());
    TVariableValue* value = &sead::elementAt(adaptor->mVariableValues, index);
    u32 bankIndex = *static_cast<const be<u32>*>(content);
    fvb::TObject* bankObject = control->fvb_Control.getObject_index(bankIndex);
    fvb::FunctionValue* function = bankObject ? bankObject->pfv_.get() : nullptr;
    value->field_0xc.fv = reinterpret_cast<jst_timeline::TFunctionValue*>(function);
    value->field_0x4 = 0;
    value->field_0x8.v = gabi::guest_fn(reinterpret_cast<const void*>(TVariableValue_update_functionValue_));
}
/* 02839FAC */
void JStudio::TAdaptor::adaptor_setVariableValue(TObject* object, u32 index, u32 operation, const void* content, u32 size) {
    switch (operation) {
    case OP_VOID: adaptor_setVariableValue_VOID_(this, object, index, content, size); break;
    case OP_IMMEDIATE: adaptor_setVariableValue_IMMEDIATE_(this, object, index, content, size); break;
    case OP_TIME: adaptor_setVariableValue_TIME_(this, object, index, content, size); break;
    case OP_FVR_NAME: adaptor_setVariableValue_FVR_NAME_(this, object, index, content, size); break;
    case OP_FVR_INDEX: adaptor_setVariableValue_FVR_INDEX_(this, object, index, content, size); break;
    }
}
/* 0283A020 */
void JStudio::TAdaptor::adaptor_setVariableValue_n(TObject* object, const be<u32>* indices, u32 count, u32 operation, const void* content, u32 size) {
    u32 stride;
    switch (operation) {
    case OP_VOID: stride = 0; break;
    case OP_IMMEDIATE:
    case OP_TIME:
    case OP_FVR_INDEX: stride = 4; break;
    default: return;
    }
    // HD: the byte extent wraps to 32 bits before forming the typed range.
    u32 bytes = count * sizeof(be<u32>);
    const be<u32>* end = indices + bytes / sizeof(be<u32>);
    while (indices != end) {
        u32 index = *indices++;
        switch (operation) {
        case OP_VOID: gabi::call_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(adaptor_setVariableValue_VOID_)), this, object, index, content, stride); break;
        case OP_IMMEDIATE: gabi::call_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(adaptor_setVariableValue_IMMEDIATE_)), this, object, index, content, stride); break;
        case OP_TIME: gabi::call_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(adaptor_setVariableValue_TIME_)), this, object, index, content, stride); break;
        case OP_FVR_INDEX: gabi::call_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(adaptor_setVariableValue_FVR_INDEX_)), this, object, index, content, stride); break;
        }
        content = static_cast<const u8*>(content) + stride;
    }
}
/* 0283A118 */
void JStudio::TAdaptor::adaptor_setVariableValue_immediate(const TSetVariableValue_immediate* values) {
    while (u32(values->index) != 0xFFFFFFFF) {
        u32 index = values->index;
        TVariableValue* value = &sead::elementAt(mVariableValues, index);
        u32 immediate = *reinterpret_cast<const be<u32>*>(&values->value);
        value->field_0x8.v = gabi::guest_fn(reinterpret_cast<const void*>(TVariableValue_update_immediate_));
        value->field_0x4 = 0;
        value->field_0xc.bits = immediate;
        ++values;
    }
}
/* 0283A160 */
void JStudio::TAdaptor::adaptor_setVariableValue_Vec(const be<u32>* indices, const Vec& vector) {
    const be<u32>* components = reinterpret_cast<const be<u32>*>(&vector);
    for (u32 i = 0; i < 3; ++i) {
        u32 index = indices[i];
        TVariableValue* value = &sead::elementAt(mVariableValues, index);
        u32 component = components[i];
        value->field_0x8.v = gabi::guest_fn(reinterpret_cast<const void*>(TVariableValue_update_immediate_));
        value->field_0x4 = 0;
        value->field_0xc.bits = component;
    }
}
/* 0283A1E0 */
void JStudio::TAdaptor::adaptor_getVariableValue_Vec(Vec* vector, const be<u32>* indices) const {
    // HD: these pure lfs/stfs copies retain signalling-NaN bits.
    be<u32>* components = reinterpret_cast<be<u32>*>(vector);
    for (u32 i = 0; i < 3; ++i) {
        u32 index = indices[i];
        TVariableValue* value = &sead::elementAt(mVariableValues, index);
        components[i] = u32(value->mValueBits);
    }
}
/* 0283A220 */
void JStudio::TAdaptor::adaptor_setVariableValue_GXColor(const be<u32>* indices, const Color& color) {
    for (u32 i = 0; i < 4; ++i) {
        u32 index = indices[i];
        u32 component = color.components[i];
        TVariableValue* value = &sead::elementAt(mVariableValues.get(), index);
        value->field_0x8.v = gabi::guest_fn(reinterpret_cast<const void*>(TVariableValue_update_immediate_));
        value->field_0x4 = 0;
        value->field_0xc.val = f32(component);
    }
}
/* 0283A318 */
void JStudio::TAdaptor::adaptor_getVariableValue_GXColor(Color* color, const be<u32>* indices) const {
    for (u32 i = 0; i < 4; ++i) {
        f32 value = sead::elementAt(mVariableValues.get(), u32(indices[i])).mValue;
        // HD: GHS's first negated comparison maps unordered values to zero.
        u32 component = !(value > 0.0f) ? 0 : value >= 255.0f ? 255 : u32(gabi::ftoi(value));
        color->components[i] = component;
    }
}
/* 0283A53C */
void JStudio_TObject_destroy(TObject* object, u32 flags) {
    if (!object) return;
    auto adaptor = reinterpret_cast<TAdaptor*>(object->mpAdaptor.get());
    object->__vtbl = reinterpret_cast<jst_timeline::ObjectVTable*>(&sObjectVTable);
    if (adaptor) {
        auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::call_ptr<void>(table->methods[ADAPTOR_DESTROY].target.get(), adaptor, 3u);
    }
    JStudio_destroySTBObject(object, 0);
    if (flags & 1) JStudio_delete(object);
}
/* 0283A630 */
void JStudio::TObject::do_begin() {
    auto adaptor = reinterpret_cast<TAdaptor*>(mpAdaptor.get());
    if (adaptor) {
        auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::tail_ptr<void>(table->methods[ADAPTOR_BEGIN].target.get(), adaptor, this);
    }
}
/* 0283A650 */
void JStudio::TObject::do_end() {
    auto adaptor = reinterpret_cast<TAdaptor*>(mpAdaptor.get());
    if (adaptor) {
        auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::tail_ptr<void>(table->methods[ADAPTOR_END].target.get(), adaptor, this);
    }
}
/* 0283A670 */
void JStudio::TObject::do_wait(u32 frames) { forward_value(frames); }
/* 0283A674 */
void JStudio::TObject::do_data(const void* id, u32 idSize, const void* content, u32 size) {
    auto adaptor = reinterpret_cast<TAdaptor*>(mpAdaptor.get());
    if (adaptor) {
        auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::call_ptr<void>(table->methods[ADAPTOR_DATA].target.get(), adaptor, this, id, idSize, content, size);
    }
}
/* 0283A6D8 */
TObject* JStudio_TObject_construct(TObject* object, const stb::data::TParse_TBlock& block, TAdaptor* adaptor) {
    if (!object) object = static_cast<TObject*>(JStudio_allocate(sizeof(TObject)));
    if (!object) return object;
    JStudio_constructSTBObject(object, block);
    object->mpAdaptor = adaptor;
    object->__vtbl = reinterpret_cast<jst_timeline::ObjectVTable*>(&sObjectVTable);
    return object;
}
/* 0283C128 */
void TVariableValue_outputNone(jst_timeline::TOutput* output, f32 value, TAdaptor* adaptor) {}
/* 0283C12C */
void JStudio::TAdaptor::adaptor_do_prepare(const TObject* object) {}
/* 0283C130 */
void JStudio::TAdaptor::adaptor_do_begin(const TObject* object) {}
/* 0283C134 */
void JStudio::TAdaptor::adaptor_do_end(const TObject* object) {}
/* 0283C138 */
void JStudio::TAdaptor::adaptor_do_update(const TObject* object, u32 frames) {}
/* 0283C13C */
void JStudio::TAdaptor::adaptor_do_data(const TObject* object, const void* id, u32 idSize, const void* content, u32 size) {}


/* 0283A74C */
void TAdaptor_actor_destroy(TAdaptor* adaptor, u32 flags) {
    if (adaptor && (flags & 1)) JStudio_delete(adaptor);
}
/* 0283A760 */
TObject_actor* TObject_actor_construct(TObject_actor* object, const stb::data::TParse_TBlock& block, TAdaptor* adaptor) {
    if (!object) object = static_cast<TObject_actor*>(JStudio_allocate(sizeof(TObject_actor)));
    if (!object) return object;
    JStudio_TObject_construct(object, block, adaptor);
    object->__vtbl = reinterpret_cast<jst_timeline::ObjectVTable*>(&sactorObjectVTable);
    return object;
}
/* 0283C140 */
void TObject_actor_destroy(TObject_actor* object, u32 flags) {
    if (!object) return;
    JStudio_TObject_destroy(object, 0);
    if (flags & 1) JStudio_delete(object);
}


/* 0283AC58 */
void TAdaptor_ambientLight_destroy(TAdaptor* adaptor, u32 flags) {
    if (adaptor && (flags & 1)) JStudio_delete(adaptor);
}
/* 0283AC6C */
TObject_ambientLight* TObject_ambientLight_construct(TObject_ambientLight* object, const stb::data::TParse_TBlock& block, TAdaptor* adaptor) {
    if (!object) object = static_cast<TObject_ambientLight*>(JStudio_allocate(sizeof(TObject_ambientLight)));
    if (!object) return object;
    JStudio_TObject_construct(object, block, adaptor);
    object->__vtbl = reinterpret_cast<jst_timeline::ObjectVTable*>(&sambientLightObjectVTable);
    return object;
}
/* 0283C194 */
void TObject_ambientLight_destroy(TObject_ambientLight* object, u32 flags) {
    if (!object) return;
    JStudio_TObject_destroy(object, 0);
    if (flags & 1) JStudio_delete(object);
}


/* 0283AD68 */
void TAdaptor_camera_destroy(TAdaptor* adaptor, u32 flags) {
    if (adaptor && (flags & 1)) JStudio_delete(adaptor);
}
/* 0283AD7C */
TObject_camera* TObject_camera_construct(TObject_camera* object, const stb::data::TParse_TBlock& block, TAdaptor* adaptor) {
    if (!object) object = static_cast<TObject_camera*>(JStudio_allocate(sizeof(TObject_camera)));
    if (!object) return object;
    JStudio_TObject_construct(object, block, adaptor);
    object->__vtbl = reinterpret_cast<jst_timeline::ObjectVTable*>(&scameraObjectVTable);
    return object;
}
/* 0283C1E8 */
void TObject_camera_destroy(TObject_camera* object, u32 flags) {
    if (!object) return;
    JStudio_TObject_destroy(object, 0);
    if (flags & 1) JStudio_delete(object);
}


/* 0283B09C */
void TAdaptor_fog_destroy(TAdaptor* adaptor, u32 flags) {
    if (adaptor && (flags & 1)) JStudio_delete(adaptor);
}
/* 0283B0B0 */
TObject_fog* TObject_fog_construct(TObject_fog* object, const stb::data::TParse_TBlock& block, TAdaptor* adaptor) {
    if (!object) object = static_cast<TObject_fog*>(JStudio_allocate(sizeof(TObject_fog)));
    if (!object) return object;
    JStudio_TObject_construct(object, block, adaptor);
    object->__vtbl = reinterpret_cast<jst_timeline::ObjectVTable*>(&sfogObjectVTable);
    return object;
}
/* 0283C23C */
void TObject_fog_destroy(TObject_fog* object, u32 flags) {
    if (!object) return;
    JStudio_TObject_destroy(object, 0);
    if (flags & 1) JStudio_delete(object);
}


/* 0283B210 */
void TAdaptor_light_destroy(TAdaptor* adaptor, u32 flags) {
    if (adaptor && (flags & 1)) JStudio_delete(adaptor);
}
/* 0283B224 */
TObject_light* TObject_light_construct(TObject_light* object, const stb::data::TParse_TBlock& block, TAdaptor* adaptor) {
    if (!object) object = static_cast<TObject_light*>(JStudio_allocate(sizeof(TObject_light)));
    if (!object) return object;
    JStudio_TObject_construct(object, block, adaptor);
    object->__vtbl = reinterpret_cast<jst_timeline::ObjectVTable*>(&slightObjectVTable);
    return object;
}
/* 0283C290 */
void TObject_light_destroy(TObject_light* object, u32 flags) {
    if (!object) return;
    JStudio_TObject_destroy(object, 0);
    if (flags & 1) JStudio_delete(object);
}


/* 0283B640 */
void TAdaptor_message_destroy(TAdaptor* adaptor, u32 flags) {
    if (adaptor && (flags & 1)) JStudio_delete(adaptor);
}
/* 0283B654 */
TObject_message* TObject_message_construct(TObject_message* object, const stb::data::TParse_TBlock& block, TAdaptor* adaptor) {
    if (!object) object = static_cast<TObject_message*>(JStudio_allocate(sizeof(TObject_message)));
    if (!object) return object;
    JStudio_TObject_construct(object, block, adaptor);
    object->__vtbl = reinterpret_cast<jst_timeline::ObjectVTable*>(&smessageObjectVTable);
    return object;
}
/* 0283C2E4 */
void TObject_message_destroy(TObject_message* object, u32 flags) {
    if (!object) return;
    JStudio_TObject_destroy(object, 0);
    if (flags & 1) JStudio_delete(object);
}


/* 0283B6F4 */
void TAdaptor_particle_destroy(TAdaptor* adaptor, u32 flags) {
    if (adaptor && (flags & 1)) JStudio_delete(adaptor);
}
/* 0283B708 */
TObject_particle* TObject_particle_construct(TObject_particle* object, const stb::data::TParse_TBlock& block, TAdaptor* adaptor) {
    if (!object) object = static_cast<TObject_particle*>(JStudio_allocate(sizeof(TObject_particle)));
    if (!object) return object;
    JStudio_TObject_construct(object, block, adaptor);
    object->__vtbl = reinterpret_cast<jst_timeline::ObjectVTable*>(&sparticleObjectVTable);
    return object;
}
/* 0283C338 */
void TObject_particle_destroy(TObject_particle* object, u32 flags) {
    if (!object) return;
    JStudio_TObject_destroy(object, 0);
    if (flags & 1) JStudio_delete(object);
}


/* 0283BCC4 */
void TAdaptor_sound_destroy(TAdaptor* adaptor, u32 flags) {
    if (adaptor && (flags & 1)) JStudio_delete(adaptor);
}
/* 0283BCD8 */
TObject_sound* TObject_sound_construct(TObject_sound* object, const stb::data::TParse_TBlock& block, TAdaptor* adaptor) {
    if (!object) object = static_cast<TObject_sound*>(JStudio_allocate(sizeof(TObject_sound)));
    if (!object) return object;
    JStudio_TObject_construct(object, block, adaptor);
    object->__vtbl = reinterpret_cast<jst_timeline::ObjectVTable*>(&ssoundObjectVTable);
    return object;
}
/* 0283C38C */
void TObject_sound_destroy(TObject_sound* object, u32 flags) {
    if (!object) return;
    JStudio_TObject_destroy(object, 0);
    if (flags & 1) JStudio_delete(object);
}


/* 0283C8E0 */
bool JStudio_equalObjectID(JStudio::fvb::TObject* object, const JStudio::fvb::ObjectID* id) {
    u32 count = object->id_size;
    if (count != u32(id->size)) return false;
    const be<u8>* first = reinterpret_cast<const be<u8>*>(object->pID.get());
    const be<u8>* second = reinterpret_cast<const be<u8>*>(id->id.get());
    if (first == second) return true;
    while (count >= 4) {
        if (u32(*reinterpret_cast<const be<u32>*>(first)) != u32(*reinterpret_cast<const be<u32>*>(second))) return false;
        first += 4;
        second += 4;
        count -= 4;
    }
    while (count) {
        if (u8(*first++) != u8(*second++)) return false;
        --count;
    }
    return true;
}


/* 0283C3E0 */
void TVariableValue_output_actor(const BoolOutput* output, f32 value, TAdaptor* adaptor) {
    gabi::Local<be<u32>> enabled;
    // HD: unordered comparisons take the enabled branch as GHS bge does.
    *enabled = !(value < 0.5f) ? 1 : 0;
    (adaptor->*output->mFunc)(2, enabled.get(), 4);
}
/* 0283C48C */
void TVariableValue_output_actor_destroy(BoolOutput* output, u32 flags) {
    if (!output) return;
    TVariableValue_TOutput_destroy(reinterpret_cast<jst_timeline::TOutput*>(output), 0);
    if (flags & 1) JStudio_delete(output);
}

/* 0283C4E0 */
void TVariableValue_output_camera(const BoolOutput* output, f32 value, TAdaptor* adaptor) {
    gabi::Local<be<u32>> enabled;
    // HD: unordered comparisons take the enabled branch as GHS bge does.
    *enabled = !(value < 0.5f) ? 1 : 0;
    (adaptor->*output->mFunc)(2, enabled.get(), 4);
}
/* 0283C58C */
void TVariableValue_output_camera_destroy(BoolOutput* output, u32 flags) {
    if (!output) return;
    TVariableValue_TOutput_destroy(reinterpret_cast<jst_timeline::TOutput*>(output), 0);
    if (flags & 1) JStudio_delete(output);
}

/* 0283C5E0 */
void TVariableValue_output_light(const BoolOutput* output, f32 value, TAdaptor* adaptor) {
    gabi::Local<be<u32>> enabled;
    // HD: unordered comparisons take the enabled branch as GHS bge does.
    *enabled = !(value < 0.5f) ? 1 : 0;
    (adaptor->*output->mFunc)(2, enabled.get(), 4);
}
/* 0283C68C */
void TVariableValue_output_light_destroy(BoolOutput* output, u32 flags) {
    if (!output) return;
    TVariableValue_TOutput_destroy(reinterpret_cast<jst_timeline::TOutput*>(output), 0);
    if (flags & 1) JStudio_delete(output);
}

/* 0283C6E0 */
void TVariableValue_output_particle(const BoolOutput* output, f32 value, TAdaptor* adaptor) {
    gabi::Local<be<u32>> enabled;
    // HD: unordered comparisons take the enabled branch as GHS bge does.
    *enabled = !(value < 0.5f) ? 1 : 0;
    (adaptor->*output->mFunc)(2, enabled.get(), 4);
}
/* 0283C78C */
void TVariableValue_output_particle_destroy(BoolOutput* output, u32 flags) {
    if (!output) return;
    TVariableValue_TOutput_destroy(reinterpret_cast<jst_timeline::TOutput*>(output), 0);
    if (flags & 1) JStudio_delete(output);
}

/* 0283C7E0 */
void TVariableValue_output_sound(const BoolOutput* output, f32 value, TAdaptor* adaptor) {
    gabi::Local<be<u32>> enabled;
    // HD: unordered comparisons take the enabled branch as GHS bge does.
    *enabled = !(value < 0.5f) ? 1 : 0;
    (adaptor->*output->mFunc)(2, enabled.get(), 4);
}
/* 0283C88C */
void TVariableValue_output_sound_destroy(BoolOutput* output, u32 flags) {
    if (!output) return;
    TVariableValue_TOutput_destroy(reinterpret_cast<jst_timeline::TOutput*>(output), 0);
    if (flags & 1) JStudio_delete(output);
}

/* 0283BFA0 */
static void __sinit_jstudio_object_cpp() {
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[1] = 0;
    sStaticBlock.words[0] = 0;
    JStudio_objectRegisterGlobalObject(&sStaticRegister);
    sactorParentOutput.mFunc.d = 0;
    sactorParentOutput.__vtbl = &sactorOutputVTable;
    sactorParentOutput.mFunc.i = 9;
    sactorParentOutput.mFunc.f = 8;
    JStudio_objectRegisterGlobalObject(&sactorParentRegistration);
    sactorRelationOutput.mFunc.d = 0;
    sactorRelationOutput.__vtbl = &sactorOutputVTable;
    sactorRelationOutput.mFunc.i = 12;
    sactorRelationOutput.mFunc.f = 8;
    JStudio_objectRegisterGlobalObject(&sactorRelationRegistration);
    scameraParentOutput.mFunc.d = 0;
    scameraParentOutput.__vtbl = &scameraOutputVTable;
    scameraParentOutput.mFunc.i = 9;
    scameraParentOutput.mFunc.f = 8;
    JStudio_objectRegisterGlobalObject(&scameraParentRegistration);
    slightEnableOutput.mFunc.d = 0;
    slightEnableOutput.__vtbl = &slightOutputVTable;
    slightEnableOutput.mFunc.i = 7;
    slightEnableOutput.mFunc.f = 8;
    JStudio_objectRegisterGlobalObject(&slightEnableRegistration);
    sparticleParentOutput.mFunc.d = 0;
    sparticleParentOutput.__vtbl = &sparticleOutputVTable;
    sparticleParentOutput.mFunc.i = 10;
    sparticleParentOutput.mFunc.f = 8;
    JStudio_objectRegisterGlobalObject(&sparticleParentRegistration);
    ssoundLocatedOutput.mFunc.d = 0;
    ssoundLocatedOutput.__vtbl = &ssoundOutputVTable;
    ssoundLocatedOutput.mFunc.f = 8;
    ssoundLocatedOutput.mFunc.i = 8;
    JStudio_objectRegisterGlobalObject(&ssoundLocatedRegistration);
    sJStudioOutputNone.__vtbl = reinterpret_cast<jst_timeline::OutputVTable*>(&sOutputNoneVTable);
}


/* 0283A7D4 */
void JStudio::TObject_actor::do_paragraph(u32 type, const void* content, u32 size) {
    auto adaptor = reinterpret_cast<TAdaptor*>(mpAdaptor.get());
    if (!adaptor) return;
    u32 operation = type & 0x1F;
    switch (type >> 5) {
    case 59: adaptor->adaptor_setVariableValue(this, 0, operation, content, size); return;
    case 75: adaptor->adaptor_setVariableValue(this, 1, operation, content, size); return;
    case 9: adaptor->adaptor_setVariableValue(this, 3, operation, content, size); return;
    case 10: adaptor->adaptor_setVariableValue(this, 4, operation, content, size); return;
    case 11: adaptor->adaptor_setVariableValue(this, 5, operation, content, size); return;
    case 13: adaptor->adaptor_setVariableValue(this, 6, operation, content, size); return;
    case 14: adaptor->adaptor_setVariableValue(this, 7, operation, content, size); return;
    case 15: adaptor->adaptor_setVariableValue(this, 8, operation, content, size); return;
    case 17: adaptor->adaptor_setVariableValue(this, 9, operation, content, size); return;
    case 18: adaptor->adaptor_setVariableValue(this, 10, operation, content, size); return;
    case 19: adaptor->adaptor_setVariableValue(this, 11, operation, content, size); return;
    case 12: adaptor->adaptor_setVariableValue_n(this, sauVariableValue_actor_TRANSLATION_XYZ.values, 3, operation, content, size); return;
    case 16: adaptor->adaptor_setVariableValue_n(this, sauVariableValue_actor_ROTATION_XYZ.values, 3, operation, content, size); return;
    case 20: adaptor->adaptor_setVariableValue_n(this, sauVariableValue_actor_SCALING_XYZ.values, 3, operation, content, size); return;
    case 57: {
        // SHAPE: slot follows the GameCube adaptor virtual-method order.
        auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::call_ptr<void>(table->methods[13].target.get(), adaptor, operation, content, size);
        return;
    }
    case 58: {
        // ANIMATION: slot follows the GameCube adaptor virtual-method order.
        auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::call_ptr<void>(table->methods[14].target.get(), adaptor, operation, content, size);
        return;
    }
    case 67: {
        // ANIMATION_MODE: slot follows the GameCube adaptor virtual-method order.
        auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::call_ptr<void>(table->methods[15].target.get(), adaptor, operation, content, size);
        return;
    }
    case 76: {
        // TEXTURE_ANIMATION: slot follows the GameCube adaptor virtual-method order.
        auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::call_ptr<void>(table->methods[16].target.get(), adaptor, operation, content, size);
        return;
    }
    case 78: {
        // TEXTURE_ANIMATION_MODE: slot follows the GameCube adaptor virtual-method order.
        auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::call_ptr<void>(table->methods[17].target.get(), adaptor, operation, content, size);
        return;
    }
    case 48: {
        // PARENT: slot follows the GameCube adaptor virtual-method order.
        auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::call_ptr<void>(table->methods[7].target.get(), adaptor, operation, content, size);
        return;
    }
    case 49: {
        // PARENT_NODE: slot follows the GameCube adaptor virtual-method order.
        auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::call_ptr<void>(table->methods[8].target.get(), adaptor, operation, content, size);
        return;
    }
    case 51: {
        // RELATION: slot follows the GameCube adaptor virtual-method order.
        auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::call_ptr<void>(table->methods[10].target.get(), adaptor, operation, content, size);
        return;
    }
    case 52: {
        // RELATION_NODE: slot follows the GameCube adaptor virtual-method order.
        auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::call_ptr<void>(table->methods[11].target.get(), adaptor, operation, content, size);
        return;
    }
    case 50:
        if (operation >= 0x10 && operation <= 0x12) {
            adaptor->adaptor_setVariableValue(this, 12, operation, content, size);
            TVariableValue* value = adaptor->mVariableValues.get() + 12;
            value->pOutput_ = reinterpret_cast<jst_timeline::TOutput*>(&sactorParentOutput);
        } else {
            // PARENT_ENABLE: direct operation when no function-value output is requested.
            auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
            gabi::call_ptr<void>(table->methods[9].target.get(), adaptor, operation, content, size);
        }
        return;
    case 53:
        if (operation >= 0x10 && operation <= 0x12) {
            adaptor->adaptor_setVariableValue(this, 13, operation, content, size);
            TVariableValue* value = adaptor->mVariableValues.get() + 13;
            value->pOutput_ = reinterpret_cast<jst_timeline::TOutput*>(&sactorRelationOutput);
        } else {
            // RELATION_ENABLE: direct operation when no function-value output is requested.
            auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
            gabi::call_ptr<void>(table->methods[12].target.get(), adaptor, operation, content, size);
        }
        return;
    default: return;
    }
}

/* 0283ACE0 */
void JStudio::TObject_ambientLight::do_paragraph(u32 type, const void* content, u32 size) {
    auto adaptor = reinterpret_cast<TAdaptor*>(mpAdaptor.get());
    if (!adaptor) return;
    u32 operation = type & 0x1F;
    switch (type >> 5) {
    case 29: adaptor->adaptor_setVariableValue(this, 0, operation, content, size); return;
    case 30: adaptor->adaptor_setVariableValue(this, 1, operation, content, size); return;
    case 31: adaptor->adaptor_setVariableValue(this, 2, operation, content, size); return;
    case 32: adaptor->adaptor_setVariableValue(this, 3, operation, content, size); return;
    case 33: adaptor->adaptor_setVariableValue_n(this, sauVariableValue_ambientLight_COLOR_RGB.values, 3, operation, content, size); return;
    case 34: adaptor->adaptor_setVariableValue_n(this, sauVariableValue_ambientLight_COLOR_RGBA.values, 4, operation, content, size); return;
    default: return;
    }
}

/* 0283ADF0 */
void JStudio::TObject_camera::do_paragraph(u32 type, const void* content, u32 size) {
    auto adaptor = reinterpret_cast<TAdaptor*>(mpAdaptor.get());
    if (!adaptor) return;
    u32 operation = type & 0x1F;
    switch (type >> 5) {
    case 21: adaptor->adaptor_setVariableValue(this, 0, operation, content, size); return;
    case 22: adaptor->adaptor_setVariableValue(this, 1, operation, content, size); return;
    case 23: adaptor->adaptor_setVariableValue(this, 2, operation, content, size); return;
    case 25: adaptor->adaptor_setVariableValue(this, 3, operation, content, size); return;
    case 26: adaptor->adaptor_setVariableValue(this, 4, operation, content, size); return;
    case 27: adaptor->adaptor_setVariableValue(this, 5, operation, content, size); return;
    case 39: adaptor->adaptor_setVariableValue(this, 6, operation, content, size); return;
    case 38: adaptor->adaptor_setVariableValue(this, 7, operation, content, size); return;
    case 40: adaptor->adaptor_setVariableValue(this, 8, operation, content, size); return;
    case 41: adaptor->adaptor_setVariableValue(this, 9, operation, content, size); return;
    case 24: adaptor->adaptor_setVariableValue_n(this, sauVariableValue_camera_POSITION_XYZ.values, 3, operation, content, size); return;
    case 28: adaptor->adaptor_setVariableValue_n(this, sauVariableValue_camera_TARGET_POSITION_XYZ.values, 3, operation, content, size); return;
    case 42: adaptor->adaptor_setVariableValue_n(this, sauVariableValue_camera_DISTANCE_NEAR_FAR.values, 2, operation, content, size); return;
    case 48: {
        // PARENT: slot follows the GameCube adaptor virtual-method order.
        auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::call_ptr<void>(table->methods[7].target.get(), adaptor, operation, content, size);
        return;
    }
    case 49: {
        // PARENT_NODE: slot follows the GameCube adaptor virtual-method order.
        auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::call_ptr<void>(table->methods[8].target.get(), adaptor, operation, content, size);
        return;
    }
    case 50:
        if (operation >= 0x10 && operation <= 0x12) {
            adaptor->adaptor_setVariableValue(this, 10, operation, content, size);
            TVariableValue* value = adaptor->mVariableValues.get() + 10;
            value->pOutput_ = reinterpret_cast<jst_timeline::TOutput*>(&scameraParentOutput);
        } else {
            // PARENT_ENABLE: direct operation when no function-value output is requested.
            auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
            gabi::call_ptr<void>(table->methods[9].target.get(), adaptor, operation, content, size);
        }
        return;
    default: return;
    }
}

/* 0283B124 */
void JStudio::TObject_fog::do_paragraph(u32 type, const void* content, u32 size) {
    auto adaptor = reinterpret_cast<TAdaptor*>(mpAdaptor.get());
    if (!adaptor) return;
    u32 operation = type & 0x1F;
    switch (type >> 5) {
    case 29: adaptor->adaptor_setVariableValue(this, 0, operation, content, size); return;
    case 30: adaptor->adaptor_setVariableValue(this, 1, operation, content, size); return;
    case 31: adaptor->adaptor_setVariableValue(this, 2, operation, content, size); return;
    case 32: adaptor->adaptor_setVariableValue(this, 3, operation, content, size); return;
    case 43: adaptor->adaptor_setVariableValue(this, 4, operation, content, size); return;
    case 44: adaptor->adaptor_setVariableValue(this, 5, operation, content, size); return;
    case 33: adaptor->adaptor_setVariableValue_n(this, sauVariableValue_fog_COLOR_RGB.values, 3, operation, content, size); return;
    case 34: adaptor->adaptor_setVariableValue_n(this, sauVariableValue_fog_COLOR_RGBA.values, 4, operation, content, size); return;
    case 45: adaptor->adaptor_setVariableValue_n(this, sauVariableValue_fog_RANGE_BEGIN_END.values, 2, operation, content, size); return;
    default: return;
    }
}

/* 0283B298 */
void JStudio::TObject_light::do_paragraph(u32 type, const void* content, u32 size) {
    auto adaptor = reinterpret_cast<TAdaptor*>(mpAdaptor.get());
    if (!adaptor) return;
    u32 operation = type & 0x1F;
    switch (type >> 5) {
    case 29: adaptor->adaptor_setVariableValue(this, 0, operation, content, size); return;
    case 30: adaptor->adaptor_setVariableValue(this, 1, operation, content, size); return;
    case 31: adaptor->adaptor_setVariableValue(this, 2, operation, content, size); return;
    case 32: adaptor->adaptor_setVariableValue(this, 3, operation, content, size); return;
    case 21: adaptor->adaptor_setVariableValue(this, 4, operation, content, size); return;
    case 22: adaptor->adaptor_setVariableValue(this, 5, operation, content, size); return;
    case 23: adaptor->adaptor_setVariableValue(this, 6, operation, content, size); return;
    case 25: adaptor->adaptor_setVariableValue(this, 7, operation, content, size); return;
    case 26: adaptor->adaptor_setVariableValue(this, 8, operation, content, size); return;
    case 27: adaptor->adaptor_setVariableValue(this, 9, operation, content, size); return;
    case 35: adaptor->adaptor_setVariableValue(this, 10, operation, content, size); return;
    case 36: adaptor->adaptor_setVariableValue(this, 11, operation, content, size); return;
    case 33: adaptor->adaptor_setVariableValue_n(this, sauVariableValue_light_COLOR_RGB.values, 3, operation, content, size); return;
    case 34: adaptor->adaptor_setVariableValue_n(this, sJStudioLightColorIndices.values, 4, operation, content, size); return;
    case 24: adaptor->adaptor_setVariableValue_n(this, sJStudioLightPositionIndices.values, 3, operation, content, size); return;
    case 28: adaptor->adaptor_setVariableValue_n(this, sJStudioLightTargetIndices.values, 3, operation, content, size); return;
    case 37: adaptor->adaptor_setVariableValue_n(this, sauVariableValue_light_DIRECTION_THETA_PHI.values, 2, operation, content, size); return;
    case 55: {
        // FACULTY: slot follows the GameCube adaptor virtual-method order.
        auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::call_ptr<void>(table->methods[8].target.get(), adaptor, operation, content, size);
        return;
    }
    case 54:
        if (operation >= 0x10 && operation <= 0x12) {
            adaptor->adaptor_setVariableValue(this, 12, operation, content, size);
            TVariableValue* value = adaptor->mVariableValues.get() + 12;
            value->pOutput_ = reinterpret_cast<jst_timeline::TOutput*>(&slightEnableOutput);
        } else {
            // ENABLE: direct operation when no function-value output is requested.
            auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
            gabi::call_ptr<void>(table->methods[7].target.get(), adaptor, operation, content, size);
        }
        return;
    default: return;
    }
}

/* 0283B6C8 */
void JStudio::TObject_message::do_paragraph(u32 type, const void* content, u32 size) {
    auto adaptor = reinterpret_cast<TAdaptor*>(mpAdaptor.get());
    if (!adaptor) return;
    u32 operation = type & 0x1F;
    switch (type >> 5) {
    case 66: {
        // MESSAGE: slot follows the GameCube adaptor virtual-method order.
        auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::call_ptr<void>(table->methods[7].target.get(), adaptor, operation, content, size);
        return;
    }
    default: return;
    }
}

/* 0283B77C */
void JStudio::TObject_particle::do_paragraph(u32 type, const void* content, u32 size) {
    auto adaptor = reinterpret_cast<TAdaptor*>(mpAdaptor.get());
    if (!adaptor) return;
    u32 operation = type & 0x1F;
    switch (type >> 5) {
    case 9: adaptor->adaptor_setVariableValue(this, 0, operation, content, size); return;
    case 10: adaptor->adaptor_setVariableValue(this, 1, operation, content, size); return;
    case 11: adaptor->adaptor_setVariableValue(this, 2, operation, content, size); return;
    case 13: adaptor->adaptor_setVariableValue(this, 3, operation, content, size); return;
    case 14: adaptor->adaptor_setVariableValue(this, 4, operation, content, size); return;
    case 15: adaptor->adaptor_setVariableValue(this, 5, operation, content, size); return;
    case 17: adaptor->adaptor_setVariableValue(this, 6, operation, content, size); return;
    case 18: adaptor->adaptor_setVariableValue(this, 7, operation, content, size); return;
    case 19: adaptor->adaptor_setVariableValue(this, 8, operation, content, size); return;
    case 29: adaptor->adaptor_setVariableValue(this, 9, operation, content, size); return;
    case 30: adaptor->adaptor_setVariableValue(this, 10, operation, content, size); return;
    case 31: adaptor->adaptor_setVariableValue(this, 11, operation, content, size); return;
    case 32: adaptor->adaptor_setVariableValue(this, 12, operation, content, size); return;
    case 69: adaptor->adaptor_setVariableValue(this, 13, operation, content, size); return;
    case 70: adaptor->adaptor_setVariableValue(this, 14, operation, content, size); return;
    case 71: adaptor->adaptor_setVariableValue(this, 15, operation, content, size); return;
    case 72: adaptor->adaptor_setVariableValue(this, 16, operation, content, size); return;
    case 46: adaptor->adaptor_setVariableValue(this, 18, operation, content, size); return;
    case 47: adaptor->adaptor_setVariableValue(this, 19, operation, content, size); return;
    case 12: adaptor->adaptor_setVariableValue_n(this, sauVariableValue_particle_TRANSLATION_XYZ.values, 3, operation, content, size); return;
    case 16: adaptor->adaptor_setVariableValue_n(this, sauVariableValue_particle_ROTATION_XYZ.values, 3, operation, content, size); return;
    case 20: adaptor->adaptor_setVariableValue_n(this, sauVariableValue_particle_SCALING_XYZ.values, 3, operation, content, size); return;
    case 33: adaptor->adaptor_setVariableValue_n(this, sauVariableValue_particle_COLOR_RGB.values, 3, operation, content, size); return;
    case 34: adaptor->adaptor_setVariableValue_n(this, sauVariableValue_particle_COLOR_RGBA.values, 4, operation, content, size); return;
    case 73: adaptor->adaptor_setVariableValue_n(this, sauVariableValue_particle_COLOR1_RGB.values, 3, operation, content, size); return;
    case 74: adaptor->adaptor_setVariableValue_n(this, sauVariableValue_particle_COLOR1_RGBA.values, 4, operation, content, size); return;
    case 68: {
        // PARTICLE: slot follows the GameCube adaptor virtual-method order.
        auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::call_ptr<void>(table->methods[7].target.get(), adaptor, operation, content, size);
        return;
    }
    case 48: {
        // PARENT: slot follows the GameCube adaptor virtual-method order.
        auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::call_ptr<void>(table->methods[8].target.get(), adaptor, operation, content, size);
        return;
    }
    case 49: {
        // PARENT_NODE: slot follows the GameCube adaptor virtual-method order.
        auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::call_ptr<void>(table->methods[9].target.get(), adaptor, operation, content, size);
        return;
    }
    case 50:
        if (operation >= 0x10 && operation <= 0x12) {
            adaptor->adaptor_setVariableValue(this, 17, operation, content, size);
            TVariableValue* value = adaptor->mVariableValues.get() + 17;
            value->pOutput_ = reinterpret_cast<jst_timeline::TOutput*>(&sparticleParentOutput);
        } else {
            // PARENT_ENABLE: direct operation when no function-value output is requested.
            auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
            gabi::call_ptr<void>(table->methods[10].target.get(), adaptor, operation, content, size);
        }
        return;
    default: return;
    }
}

/* 0283BD4C */
void JStudio::TObject_sound::do_paragraph(u32 type, const void* content, u32 size) {
    auto adaptor = reinterpret_cast<TAdaptor*>(mpAdaptor.get());
    if (!adaptor) return;
    u32 operation = type & 0x1F;
    switch (type >> 5) {
    case 21: adaptor->adaptor_setVariableValue(this, 0, operation, content, size); return;
    case 22: adaptor->adaptor_setVariableValue(this, 1, operation, content, size); return;
    case 23: adaptor->adaptor_setVariableValue(this, 2, operation, content, size); return;
    case 46: adaptor->adaptor_setVariableValue(this, 3, operation, content, size); return;
    case 47: adaptor->adaptor_setVariableValue(this, 4, operation, content, size); return;
    case 61: adaptor->adaptor_setVariableValue(this, 6, operation, content, size); return;
    case 62: adaptor->adaptor_setVariableValue(this, 7, operation, content, size); return;
    case 63: adaptor->adaptor_setVariableValue(this, 8, operation, content, size); return;
    case 64: adaptor->adaptor_setVariableValue(this, 9, operation, content, size); return;
    case 65: adaptor->adaptor_setVariableValue(this, 10, operation, content, size); return;
    case 24: adaptor->adaptor_setVariableValue_n(this, sauVariableValue_sound_POSITION_XYZ.values, 3, operation, content, size); return;
    case 60: {
        // SOUND: slot follows the GameCube adaptor virtual-method order.
        auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
        gabi::call_ptr<void>(table->methods[7].target.get(), adaptor, operation, content, size);
        return;
    }
    case 56:
        if (operation >= 0x10 && operation <= 0x12) {
            adaptor->adaptor_setVariableValue(this, 5, operation, content, size);
            TVariableValue* value = adaptor->mVariableValues.get() + 5;
            value->pOutput_ = reinterpret_cast<jst_timeline::TOutput*>(&ssoundLocatedOutput);
        } else {
            // LOCATED: direct operation when no function-value output is requested.
            auto table = reinterpret_cast<AdaptorVTable*>(adaptor->__vtbl.get());
            gabi::call_ptr<void>(table->methods[8].target.get(), adaptor, operation, content, size);
        }
        return;
    default: return;
    }
}

/* WWHD glue
data sactorObjectVTable 10172FC8
data sambientLightObjectVTable 10173000
data scameraObjectVTable 10173038
data sfogObjectVTable 10173070
data slightObjectVTable 101730A8
data smessageObjectVTable 101730E0
data sparticleObjectVTable 10173118
data ssoundObjectVTable 10173150
data sObjectVTable 10172F90
data sOutputNoneVTable 10172F78
data sJStudioOutputNone 104B605C
data sStaticBlock 104B604C
data sactorParentOutput 104B6060
data sactorParentRegistration 101FAD50
data sactorRelationOutput 104B606C
data sactorRelationRegistration 101FAD5C
data scameraParentOutput 104B6078
data scameraParentRegistration 101FAD68
data slightEnableOutput 104B6084
data slightEnableRegistration 101FAD74
data sparticleParentOutput 104B6090
data sparticleParentRegistration 101FAD80
data ssoundLocatedOutput 104B609C
data ssoundLocatedRegistration 101FAD8C
data sactorOutputVTable 10172D64
data scameraOutputVTable 10172D7C
data slightOutputVTable 10172D94
data sparticleOutputVTable 10172DAC
data ssoundOutputVTable 10172DC4
data sauVariableValue_actor_TRANSLATION_XYZ 10172EB8
data sauVariableValue_actor_ROTATION_XYZ 10172EC4
data sauVariableValue_actor_SCALING_XYZ 10172ED0
data sauVariableValue_ambientLight_COLOR_RGB 10172EDC
data sauVariableValue_ambientLight_COLOR_RGBA 10172E68
data sauVariableValue_camera_POSITION_XYZ 10172EE8
data sauVariableValue_camera_TARGET_POSITION_XYZ 10172EF4
data sauVariableValue_camera_DISTANCE_NEAR_FAR 10172E50
data sauVariableValue_fog_COLOR_RGB 10172F00
data sauVariableValue_fog_COLOR_RGBA 10172E78
data sauVariableValue_fog_RANGE_BEGIN_END 10172E58
data sauVariableValue_light_COLOR_RGB 10172F0C
data sJStudioLightColorIndices 10172E88
data sJStudioLightPositionIndices 10172F18
data sJStudioLightTargetIndices 10172F24
data sauVariableValue_light_DIRECTION_THETA_PHI 10172E60
data sauVariableValue_particle_TRANSLATION_XYZ 10172F30
data sauVariableValue_particle_ROTATION_XYZ 10172F3C
data sauVariableValue_particle_SCALING_XYZ 10172F48
data sauVariableValue_particle_COLOR_RGB 10172F54
data sauVariableValue_particle_COLOR_RGBA 10172E98
data sauVariableValue_particle_COLOR1_RGB 10172F60
data sauVariableValue_particle_COLOR1_RGBA 10172EA8
data sauVariableValue_sound_POSITION_XYZ 10172F6C
data sStaticRegister 101FAD44
ext 0283CBE4 void JStudio_constructSTBObject(TObject*, const stb::data::TParse_TBlock&)
ext 0283CC9C void JStudio_destroySTBObject(TObject*, u32)
ext 028F026C void JStudio_objectRegisterGlobalObject(void*)
ext 02839DA0 void TVariableValue_update_immediate_(jst_timeline::TVariableValue*, f64) callback
ext 02839DB4 void TVariableValue_update_time_(jst_timeline::TVariableValue*, f64) callback
ext 02839DF4 void TVariableValue_update_functionValue_(jst_timeline::TVariableValue*, f64) callback
ext 0283A5BC void jst_timeline::TObject::forward_value(u32)
*/
