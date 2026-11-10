#include "JSystem/JStudio/jstudio-object.h"
// Light adaptor lifecycle and initial values, from WWHD and the tww JStudio reference.
#include "JSystem/JStudio/JStudio_JStage/object-light.h"
using namespace JStudio_JStage;
void* JStudioLight_allocate(u32);
void JStudioLight_delete(void*);
void JStudioLight_clear(void*,u32);
void JStudioLight_constructArray(void*,u32,u32,u32);
void JStudioLight_destroyArray(void*,u32,u32,u32,u32,u32);
void JStudioLight_destroyValue(jst_timeline::TVariableValue*,u32);
void JStudioLight_destroyBase(jst_adaptors::TAdaptor*,u32);
void JStudioLight_register(void*);
void JStudioLight_setColor(jst_adaptors::TAdaptor*,const jst_adaptors::Index4*,const jst_adaptors::Color*);
void JStudioLight_setVec(jst_adaptors::TAdaptor*,const jst_adaptors::Index3*,const jst_adaptors::Vec*);
void JStudioLight_multVec(const Mtx34*,const jst_adaptors::Vec*,jst_adaptors::Vec*);
void JStudioLight_multVecSR(const Mtx34*,const jst_adaptors::Vec*,jst_adaptors::Vec*);
void JStudioLight_addVec(const jst_adaptors::Vec*,const jst_adaptors::Vec*,jst_adaptors::Vec*);
f64 JStudioLight_sqrt(f64);
f64 JStudioLight_atan2(f64,f64);
void JStudioLight_immediate(jst_timeline::TVariableValue*,f64,jst_adaptors::TAdaptor*);
void JStudioLight_forwardData(jst_adaptors::StageObject*,const void*,u32,const void*,u32);
void JStudioLight_enable(jst_adaptors::StageObject*,u32,const be<u32>*,u32);
jst_timeline::TVariableValue* JStudioLight_constructVariableValue(jst_timeline::TVariableValue*);
void JStudioLight_destroyDirection(LightDirectionOutput*,u32);
namespace {
static u32 sBaseVtable;
static u32 sLightVtable;
static u32 sDirectionVtable;
static const f32 sDegreesPerRadian;
struct DirectionOutputs { LightDirectionOutput values[6]; };
static DirectionOutputs sDirectionOutputs;
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
static u32 sDirectionRegister;
static LightVariablePrefix* prefix(TAdaptor_light* adaptor) { return reinterpret_cast<LightVariablePrefix*>(adaptor->pad04); }
static LightVtable* methods(jst_adaptors::StageObject* object) { return reinterpret_cast<LightVtable*>(object->mVtable.get()); }
}

/* 02841910 */
TAdaptor_light* TAdaptor_light_construct(TAdaptor_light* adaptor,const void* system,jst_adaptors::StageObject* object) {
    if (!adaptor) adaptor=static_cast<TAdaptor_light*>(JStudioLight_allocate(sizeof(*adaptor)));
    if (adaptor) {
        auto* state=prefix(adaptor);
        state->mCount=13; state->__vtbl=&sBaseVtable;
        adaptor->mVariableValues=reinterpret_cast<jst_adaptors::TVariableValues*>(state->mValues);
        JStudioLight_constructArray(state->mValues,13,20,gabi::guest_fn(reinterpret_cast<const void*>(&JStudioLight_constructVariableValue)));
        adaptor->mObject=object; adaptor->_118=0; state->__vtbl=&sLightVtable; state->mSystem=system;
    }
    return adaptor;
}

/* 028419B0 */
void TAdaptor_light::adaptor_do_end(const jst_adaptors::TObject*) {
    auto* object=mObject.get(); auto* table=methods(object);
    u32 get=table->getFlags.get(),set=table->setFlags.get();
    u32 flags=gabi::call_ptr<u32>(get,object);
    gabi::call_ptr<void>(set,object,flags&~1u);
}

/* 02841A08 */
void TAdaptor_light_destroy(TAdaptor_light* adaptor,u32 flags) {
    if (!adaptor) return;
    prefix(adaptor)->__vtbl=&sLightVtable;
    adaptor->adaptor_do_end(nullptr);
    JStudioLight_destroyBase(adaptor,0);
    if (flags&1) JStudioLight_delete(adaptor);
}

/* 02841A74 */
void TAdaptor_light::adaptor_do_prepare(const jst_adaptors::TObject*) {
    for (auto* output=sDirectionOutputs.values;s32(output->mValueIndex)!=-1;++output) {
        auto* values=reinterpret_cast<jst_timeline::TVariableValue*>(mVariableValues.get());
        values[u32(output->mValueIndex)].pOutput_=output;
    }
}

/* 02841AAC */
void TAdaptor_light::adaptor_do_begin(const jst_adaptors::TObject* object) {
    auto frameScope = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<LightBeginFrame> frame(8);
    auto* light=mObject.get(); auto* table=methods(light);
    u32 get=table->getFlags.get(),set=table->setFlags.get();
    u32 flags=gabi::call_ptr<u32>(get,light);
    gabi::call_ptr<void>(set,light,flags|1u);
    light=mObject;
    u32 colorFunction=methods(light)->getColor.get();
    auto* control=reinterpret_cast<JStudio::TControl*>(object->mControl.get());
    u32 color=gabi::call_ptr<u32>(colorFunction,light);
    frame->packed.word=color; frame->color.word=frame->packed.word;
    JStudioLight_setColor(this,&sJStudioLightColorIndices,&frame->color);
    light=mObject;
    gabi::call_ptr<void>(methods(light)->getPosition.get(),light,&frame->source[0]);
    light=mObject;
    gabi::call_ptr<void>(methods(light)->getDirection.get(),light,&frame->source[1]);
    bool transformed=control->mTransformOnGet!=0;
    jst_adaptors::Vec* vectors=&frame->source[0];
    if (transformed) {
        JStudioLight_multVec(&control->mTransformOnGet_Matrix,&frame->source[0],&frame->transformed[0]);
        JStudioLight_multVecSR(&control->mTransformOnGet_Matrix,&frame->source[1],&frame->transformed[1]);
        vectors=&frame->transformed[0];
    }
    JStudioLight_setVec(this,&sJStudioLightPositionIndices,vectors);
    f32 z=vectors[1].z,x=vectors[1].x;
    f32 square=gabi::fmuls_ppc(z,z);
    f32 distanceSquared=gabi::fmadds(x,x,square);
    f32 y=vectors[1].y;
    f64 distance=JStudioLight_sqrt(distanceSquared);
    f64 yaw=JStudioLight_atan2(x,z);
    f64 pitch=JStudioLight_atan2(y,distance);
    f32 degrees=sDegreesPerRadian;
    auto* values=reinterpret_cast<jst_timeline::TVariableValue*>(mVariableValues.get());
    f32 yawDegrees=f32(yaw*degrees);
    values[10].mAge=0;
    values[10].mUpdate.v=gabi::guest_fn(reinterpret_cast<const void*>(&JStudioLight_immediate));
    values[10].mParam.val=yawDegrees;
    values=reinterpret_cast<jst_timeline::TVariableValue*>(mVariableValues.get());
    f32 pitchDegrees=f32(pitch*degrees);
    if (transformed) {
        values[11].mParam.val=pitchDegrees;
        values[11].mAge=0;
        values[11].mUpdate.v=gabi::guest_fn(reinterpret_cast<const void*>(&JStudioLight_immediate));
    } else {
        values[11].mUpdate.v=gabi::guest_fn(reinterpret_cast<const void*>(&JStudioLight_immediate));
        values[11].mAge=0; values[11].mParam.val=pitchDegrees;
    }
    JStudioLight_addVec(vectors,vectors+1,&frame->target);
    JStudioLight_setVec(this,&sJStudioLightTargetIndices,&frame->target);
}

/* 02841F8C */
void TAdaptor_light::adaptor_do_data(const jst_adaptors::TObject*,const void* id,u32 idSize,const void* data,u32 size) {
    JStudioLight_forwardData(mObject,id,idSize,data,size);
}

/* 02841FA4 */
void TAdaptor_light::adaptor_do_FACULTY(u32 operation,const be<u32>* data,u32) {
    if (operation!=2) return;
    u32 faculty=*data;
    if (faculty<0x301 || faculty>0x303) return;
    auto* light=mObject.get();
    gabi::call_ptr<void>(methods(light)->setLightType.get(),light,faculty-0x300);
}

/* 02841FE0 */
void TAdaptor_light::adaptor_do_ENABLE(u32 operation,const be<u32>* data,u32 size) {
    JStudioLight_enable(mObject,operation,data,size);
}

/* 02841FE8 */
void LightDirectionOutput::output(f32,jst_adaptors::TAdaptor* adaptor) const {
    reinterpret_cast<TAdaptor_light*>(adaptor)->_118=mDirection;
}

/* 02841FF4 */
static void initializeLightAdaptor() {
    sStaticBlock.words[3]=0; sStaticBlock.words[2]=0; sStaticBlock.words[1]=0; sStaticBlock.words[0]=0;
    JStudioLight_register(&sStaticRegister);
    const s32 indices[]={10,11,7,8,9,-1};
    const u32 directions[]={1,1,2,2,2,0};
    for (u32 i=0;i<6;++i) {
        sDirectionOutputs.values[i].__vtbl=reinterpret_cast<jst_timeline::OutputVTable*>(&sDirectionVtable);
        sDirectionOutputs.values[i].mValueIndex=indices[i];
        sDirectionOutputs.values[i].mDirection=directions[i];
    }
    JStudioLight_register(&sDirectionRegister);
}

/* 028420C0 */
static void destroyLightDirections() {
    JStudioLight_destroyArray(&sDirectionOutputs,6,12,gabi::guest_fn(reinterpret_cast<const void*>(&JStudioLight_destroyDirection)),0,0);
}

/* 028420E4 */
jst_timeline::TVariableValue* JStudioLight_constructVariableValue(jst_timeline::TVariableValue* value) {
    if (!value) value=static_cast<jst_timeline::TVariableValue*>(JStudioLight_allocate(sizeof(*value)));
    if (value) { value->mAge=0; value->mUpdate.v=0; JStudioLight_clear(&value->mParam,4); value->pOutput_=&sJStudioOutputNone; }
    return value;
}

/* 02842148 */
void JStudioLight_destroyDirection(LightDirectionOutput* output,u32 flags) {
    if (!output) return;
    JStudioLight_destroyValue(reinterpret_cast<jst_timeline::TVariableValue*>(output),0);
    if (flags&1) JStudioLight_delete(output);
}

/* WWHD glue:
data sBaseVtable 10173944
data sLightVtable 1017399C
data sDirectionVtable 101739E4
data sDegreesPerRadian 1017398C
data sDirectionOutputs 104B63DC
data sStaticBlock 104B63CC
data sStaticRegister 101FAEB8
data sDirectionRegister 101FAEC4
ext 0273AD10 void* JStudioLight_allocate(u32)
ext 0273AF40 void JStudioLight_delete(void*)
ext 028F521C void JStudioLight_clear(void*,u32)
ext 028EFFD0 void JStudioLight_constructArray(void*,u32,u32,u32)
ext 028F0164 void JStudioLight_destroyArray(void*,u32,u32,u32,u32,u32)
ext 02839E5C void JStudioLight_destroyValue(jst_timeline::TVariableValue*,u32)
ext 0283B210 void JStudioLight_destroyBase(jst_adaptors::TAdaptor*,u32)
ext 028F026C void JStudioLight_register(void*)
ext 0283A220 void JStudioLight_setColor(jst_adaptors::TAdaptor*,const jst_adaptors::Index4*,const jst_adaptors::Color*)
ext 0283A160 void JStudioLight_setVec(jst_adaptors::TAdaptor*,const jst_adaptors::Index3*,const jst_adaptors::Vec*)
ext 028E8F64 void JStudioLight_multVec(const Mtx34*,const jst_adaptors::Vec*,jst_adaptors::Vec*)
ext 028E9044 void JStudioLight_multVecSR(const Mtx34*,const jst_adaptors::Vec*,jst_adaptors::Vec*)
ext 028E8D88 void JStudioLight_addVec(const jst_adaptors::Vec*,const jst_adaptors::Vec*,jst_adaptors::Vec*)
ext 028F4384 f64 JStudioLight_sqrt(f64)
ext 028F4D28 f64 JStudioLight_atan2(f64,f64)
ext 02839DA0 void JStudioLight_immediate(jst_timeline::TVariableValue*,f64,jst_adaptors::TAdaptor*) callback
ext 0284219C void JStudioLight_forwardData(jst_adaptors::StageObject*,const void*,u32,const void*,u32)
ext 028421C8 void JStudioLight_enable(jst_adaptors::StageObject*,u32,const be<u32>*,u32)
*/
