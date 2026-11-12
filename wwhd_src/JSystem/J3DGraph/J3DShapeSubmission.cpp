// HD shape submission and its preparation-only counterpart.
#include "JSystem/J3DGraph/J3DShapeSubmission.h"
#include "port_native_draw.h"
using namespace j3d;
extern j3d::RenderContext& sRenderContext;
FrameRecords* shapeFrameRegistry();
s32 findShapeFrame(FrameSource*,const sead::SafeText*);
void activateShapeFrame(FrameRecord*,FrameSourceRecord*,void*);
void refreshShapeShader(ShaderProgram_l*,u32);
void bindShapeShaderProgram(void*);
void callShapeDisplayList(void*,u32);
void bindShapeShaderState(ShaderProgram_l*);
void bindShapePixelUniform(s32,u32,const void*);
void bindShapeVertexUniform(s32,u32,const void*);
void bindShapeGeometryUniform(s32,u32,const void*);
void bindShapeBuffer(void*,s32,u32);
void bindDirectShapeMaterial(void*,PacketMaterialBinding_l*,u32);
void bindShapeFrameInput(ShapeDrawInput_l*,FrameRecord*);
void bindShapeMaterialState(void*,PacketMaterialBinding_l*);
void bindShapeMaterialProgram(void*);
void setShapeDrawStateA(void*,u32);
void setShapeDrawStateB(void*,u32);
void bindShapeDrawState(ModelMaterialDrawState_l*);
void setShapeAlphaState(void*,u32);
void constructShapeFixedState(DrawFixedState_l*);
u32 shapeStateOpaque(void*);
u32 shapeStateAlternative(void*);
void applyShapeProjection(DrawProjection_l*);
void applyShapeFixedState(DrawFixedState_l*);
void applyShapeBlendState(void*);
void submitShapePrimitive(void*,u32,u32,u32);
static sead::SafeTextVtable sShapeSubmissionTextMethods;
/* inlined into both shape submission entries */
static ModelState_l* shapeModel(ShapePacket* packet) {
    return reinterpret_cast<ModelState_l*>(static_cast<void*>(packet->mModel));
}
/* inlined into both shape submission entries */
static u32 shapePassBit(u32 pass) { return (pass&0x20)!=0?0:u32(1)<<(pass&31); }
/* inlined into both shape submission entries */
// An out-of-range index gives the original's null record, whose fields it then reads at low guest addresses:
// the record stays a guest address (0 for null) and is read with gabi::load, never through a C++ null pointer.
static u32 shapeProgram(ModelState_l* model,u32 material,u32 pass) {
    ModelProgramData_l* data=reinterpret_cast<ModelProgramData_l*>(static_cast<J3DModelData*>(model->mModelData));
    u32 index=material*4+pass;
    if(index>=u32(data->mProgramCount)) return 0;
    return gabi::ea(static_cast<ModelProgram_l*>(data->mPrograms))+index*sizeof(ModelProgram_l);
}
/* inlined into both shape submission entries */
static u32 shapeDrawState(ModelState_l* model,u32 index) {
    if(index>=u32(model->mMaterialCount)) return 0;
    return gabi::ea(static_cast<void*>(model->mMaterials))+index*sizeof(ModelMaterialDrawState_l);
}
/* inlined into both shape submission entries */
static FrameRecord* shapeProgramFrame(u32 program) {
    return gabi::at<FrameRecord>(gabi::load<u32>(program+offsetof(ModelProgram_l,mFrame)));
}
/* inlined into both shape submission entries */
static FrameRecord* shapeNamedFrame(sead::SafeText* name,const char* text) {
    name->mVtable=&sShapeSubmissionTextMethods;
    name->mText=gabi::at<be<u8>>(gabi::guest_str(text));
    FrameRecords* registry=shapeFrameRegistry();
    s32 index=findShapeFrame(registry->mSource,name);
    if(index<0) return NULL;
    u32 count=registry->mCount;
    FrameRecord* records=registry->mRecords;
    FrameRecord* frame=u32(index)<count?records+index:records;
    if(frame->mActive==0) {
        FrameSource* source=registry->mSource;
        FrameSourceRecord* sourceRecord=NULL;
        if(u32(index)<u32(source->mCount)) {
            FrameSourceRecord* sourceRecords=source->mRecords;
            sourceRecord=sourceRecords+index;
        }
        activateShapeFrame(frame,sourceRecord,NULL);
        count=registry->mCount;
        records=registry->mRecords;
    }
    return u32(index)<count?records+index:records;
}
/* inlined into both shape submission entries */
static void* fallbackShapeProgram(ModelState_l* model,u32 material) {
    ModelUniformBindings_l* bindings=reinterpret_cast<ModelUniformBindings_l*>(model);
    return gabi::at<void>(gabi::ea(static_cast<void*>(bindings->mFallbackPrograms))+material*0x1D8);
}
/* inlined into both shape submission entries */
static bool changeShapeShader(FrameRecord* frame,bool prepareOnly) {
    ContextMode* context=sRenderContext.currentCoreMode();
    ShaderProgram_l* shader=reinterpret_cast<FrameShader_l*>(frame)->mShader;
    if(gabi::ea(shader)==u32(context->mArgument)) return false;
    u32 flags=shader->mFlags;
    u32 oldProgram=context->mMode;
    if((flags&2)!=0) {
        shader->mFlags=flags&~u32(2);
        refreshShapeShader(shader,0);
    }
    ShaderState_l* state=shader->mState;
    void* program=state->mProgram;
    if(oldProgram!=gabi::ea(program)) bindShapeShaderProgram(program);
    u32 displayBytes=shader->mDisplayListBytes;
    if(displayBytes!=0) {
        callShapeDisplayList(shader->mDisplayList,displayBytes);
        context->mMode=gabi::ea(program);
        context->mArgument=gabi::ea(shader);
    } else {
        bindShapeShaderState(shader);
        // The preparation entry stores the program first in both branches.
        if(prepareOnly) {
            context->mMode=gabi::ea(program);
            context->mArgument=gabi::ea(shader);
        } else {
            context->mArgument=gabi::ea(shader);
            context->mMode=gabi::ea(program);
        }
    }
    return true;
}
/* inlined into both shape submission entries */
static UniformBuffer_l* shapeAnimationBuffer(AnimationUniforms_l* animations,u32 index) {
    AnimationUniform_l* entries=animations->mEntries;
    if(index<u32(animations->mCount)) entries+=index;
    return reinterpret_cast<UniformBuffer_l*>(entries);
}
/* inlined into both shape submission entries */
static UniformBufferSlot_l* shapeActiveBuffer(UniformBuffer_l* buffer) {
    u32 index=buffer->mActive;
    return gabi::at<UniformBufferSlot_l>(gabi::ea(buffer)+index*0x1C);
}
/* inlined into both shape submission entries */
static UniformSlots_l* shapeUniformSlots(FrameRecord* record,u32 index) {
    FrameShader_l* shader=reinterpret_cast<FrameShader_l*>(record);
    u32 count=shader->mUniformCount;
    FrameUniform_l* uniform=NULL;
    if(count>index) {
        FrameUniform_l* uniforms=shader->mUniforms;
        uniform=uniforms+index;
    }
    return gabi::at<UniformSlots_l>(gabi::ea(uniform)+0xC);
}
/* inlined into both shape submission entries */
static void bindShapeUniforms(UniformBuffer_l* buffer,UniformSlots_l* slots) {
    UniformBufferSlot_l* active=shapeActiveBuffer(buffer);
    void* data=active->mData;
    u32 bytes=active->mBytes;
    // All slot indices and buffer operands are captured before the first callback.
    s32 vertex=slots->mVertex;
    s32 pixel=slots->mPixel;
    s32 geometry=slots->mGeometry;
    if(pixel!=-1) bindShapePixelUniform(pixel,bytes,data);
    if(vertex!=-1) bindShapeVertexUniform(vertex,bytes,data);
    if(geometry!=-1) bindShapeGeometryUniform(geometry,bytes,data);
}
/* inlined into both shape submission entries */
static void* shapeRelativeState(PacketMaterialBinding_l* material) {
    PacketMaterialHeader_l* header=material->mHeader;
    u32 offset=header->mStateOffset;
    return offset==0?NULL:gabi::at<void>(gabi::ea(&header->mStateOffset)+offset);
}
/* inlined into both shape submission entries */
static void* shapeStatePart(PacketMaterialBinding_l* material,u32 offset) {
    return gabi::at<void>(gabi::ea(shapeRelativeState(material))+offset);
}
/* inlined into both shape submission entries */
static void bindShapeExtraUniform(ShapeDrawInput_l* input,FrameRecord* frame) {
    UniformBuffer_l* extra=input->mExtra;
    if(extra==NULL) return;
    // The extra slot uses the same active ring, starting sixteen bytes later.
    UniformBufferSlot_l* active=shapeActiveBuffer(extra);
    UniformSlots_l* slots=shapeUniformSlots(frame,4);
    void* data=active->mData;
    u32 bytes=active->mBytes;
    s32 vertex=slots->mVertex;
    s32 pixel=slots->mPixel;
    s32 geometry=slots->mGeometry;
    if(pixel!=-1) bindShapePixelUniform(pixel,bytes,data);
    if(vertex!=-1) bindShapeVertexUniform(vertex,bytes,data);
    if(geometry!=-1) bindShapeGeometryUniform(geometry,bytes,data);
}
/* inlined into both shape submission entries */
static void applyShapePassState(ShapeDrawInput_l* input,PacketMaterialBinding_l* material,DrawFixedState_l* fixed) {
    constructShapeFixedState(fixed);
    u32 mode;
    if(shapeStateOpaque(shapeStatePart(material,4))!=0) mode=0;
    else mode=shapeStateAlternative(shapeStatePart(material,4))!=0?1:2;
    DrawProjection_l* projection=input->mProjection;
    fixed->mMode=mode;
    if(projection!=NULL) {
        applyShapeProjection(projection);
        projection=input->mProjection;
        u32 middle=projection->mFlags[1];
        u32 first=projection->mFlags[0];
        u32 last=projection->mFlags[2];
        fixed->mFlags[0]=first;
        fixed->mFlags[1]=middle;
        fixed->mFlags[2]=last;
    }
    applyShapeFixedState(fixed);
}
/* inlined into ShapePacket::submitShape */
static void submitShapeGeometry(ShapePacket* packet) {
    ShapeResource_l* resource=static_cast<ShapeResource_l*>(static_cast<void*>(packet->mResource));
    ShapeResourceHeader_l* header=resource->mHeader;
    u32 offset=header->mPrimitiveOffset;
    u32 primitive=offset==0?0:gabi::ea(&header->mPrimitiveOffset)+offset;
    u32 count=gabi::load<u16>(primitive+offsetof(PacketPrimitive_l,mIndex));
    submitShapePrimitive(gabi::at<void>(primitive),0,count,1);
}
/* 027F16E8 */
void ShapePacket::submitShape(ShapeDrawInput_l* input) {
    gabi::AutoFrame stack{gabi::current_function()};
    ShapeResource_l* resource=static_cast<ShapeResource_l*>(static_cast<void*>(mResource));
    if(resource==NULL) return;
    ShapeResourceHeader_l* header=resource->mHeader;
    u32 materialIndex=header->mMaterialIndex;
    u32 bindingIndex=header->mBindingIndex;
    u32 pass=input->mPass;
    ModelState_l* model=shapeModel(this);
    u32 directProgram=shapeProgram(model,materialIndex,pass);
    PacketMaterialBinding_l* binding=gabi::at<PacketMaterialBinding_l>(gabi::ea(static_cast<PacketMaterialBinding_l*>(model->mMaterialState.mBindings))+bindingIndex*sizeof(PacketMaterialBinding_l));
    ModelMaterialState_l* materialState=&model->mMaterialState;
    u32 view=input->mView;
    u32 draw=shapeDrawState(model,bindingIndex);
    if((gabi::load<u32>(draw+offsetof(ModelMaterialDrawState_l,mPassMask))&shapePassBit(pass))==0) return;
    if(pass==2 && mDirty!=0) directProgram=shapeProgram(model,materialIndex,3);
    gabi::FrameLocal<sead::SafeText> name(8);
    FrameRecord* frame;
    void* program;
    if(model->mDirectPrograms!=0) {
        frame=shapeProgramFrame(directProgram);
        program=gabi::at<void>(directProgram);
    } else {
        frame=shapeNamedFrame(name.get(),"wii_pipeline");
        program=fallbackShapeProgram(model,materialIndex);
    }
    UniformSlots_l* viewSlots=shapeUniformSlots(frame,0);
    UniformSlots_l* modelSlots=shapeUniformSlots(frame,1);
    UniformSlots_l* shapeSlots=shapeUniformSlots(frame,2);
    changeShapeShader(frame,false);
    if(pass==0) bindShapeUniforms(shapeAnimationBuffer(input->mViews,view),viewSlots);
    else if(view==2) {
        AnimationUniforms_l* views=input->mViews;
        UniformBuffer_l* first=reinterpret_cast<UniformBuffer_l*>(static_cast<AnimationUniform_l*>(views->mEntries));
        bindShapeUniforms(first,viewSlots);
    } else {
        model=shapeModel(this);
        bindShapeUniforms(shapeAnimationBuffer(reinterpret_cast<AnimationUniforms_l*>(&model->mAnimation[0]),view),viewSlots);
    }
    model=shapeModel(this);
    s32 modelSlot=modelSlots->mVertex;
    if(modelSlot!=-1) {
        ModelUniformBindings_l* bindings=gabi::at<ModelUniformBindings_l>(gabi::ea(materialState)-0x14);
        void* matrix=bindings->mMatrix;
        if(gabi::load<u32>(gabi::ea(matrix)+0x1C)!=0) {
            bindShapeBuffer(gabi::at<void>(gabi::ea(matrix)+0x18),modelSlot,model->mRenderEnabled);
            model=shapeModel(this);
        }
    }
    s32 shapeSlot=shapeSlots->mVertex;
    if(shapeSlot!=-1) {
        ShapeBufferSource_l* source=static_cast<ShapeBufferSource_l*>(static_cast<void*>(mResource));
        u32 multiplier=source->mBufferCount;
        u32 offset=(view==2?multiplier:view*multiplier)*0x14;
        bindShapeBuffer(gabi::at<void>(gabi::ea(static_cast<void*>(source->mBuffers))+offset),shapeSlot,model->mRenderEnabled);
        model=shapeModel(this);
    }
    if(model->mDirectPrograms!=0) bindDirectShapeMaterial(program,binding,model->mRenderEnabled);
    else {
        ProgramUniforms_l* uniforms=static_cast<ProgramUniforms_l*>(program);
        bindShapeUniforms(reinterpret_cast<UniformBuffer_l*>(static_cast<ModelRenderState_l*>(model->mRenderState)),&uniforms->mSlots);
    }
    if(pass==2) bindShapeExtraUniform(input,frame);
    bindShapeFrameInput(input,frame);
    bindShapeMaterialState(gabi::at<void>(gabi::ea(program)+0x84),binding);
    bindShapeMaterialProgram(gabi::at<void>(gabi::ea(program)+0x18));
    if(pass!=0) {
        if(mBufferType==1) {
            model=shapeModel(this);
            draw=shapeDrawState(model,bindingIndex);
            if((gabi::load<u8>(draw+offsetof(ModelMaterialDrawState_l,mDirty))&1)==0) {
                draw=shapeDrawState(model,bindingIndex);
                void* state=gabi::at<void>(draw+4);
                setShapeDrawStateA(state,1);
                setShapeDrawStateB(state,1);
            }
        }
        if(view!=2) {
            bindShapeDrawState(gabi::at<ModelMaterialDrawState_l>(shapeDrawState(shapeModel(this),bindingIndex)));
            if(pass==2) setShapeAlphaState(shapeStatePart(binding,0x18),1);
        }
    } else {
        gabi::FrameLocal<DrawFixedState_l> fixed(0x10);
        applyShapePassState(input,binding,fixed.get());
        applyShapeBlendState(shapeStatePart(binding,0xC));
    }
#if PORT_NATIVE_DRAW
    // the J3D material cache (port_native_draw.h): this draw's key; every GX2 call above and the draw stay the site's
    wwhd_native_j3d(gabi::ea(resource),gabi::ea(binding),pass,view,
                    shapeModel(this)->mDirectPrograms!=0?directProgram:gabi::ea(frame));
#endif
    submitShapeGeometry(this);
}
/* 027F1FA8 */
void ShapePacket::prepareShape(ShapeDrawInput_l* input) {
    gabi::AutoFrame stack{gabi::current_function()};
    ShapeResource_l* resource=static_cast<ShapeResource_l*>(static_cast<void*>(mResource));
    if(resource==NULL) return;
    u32 pass=input->mPass;
    ModelState_l* model=shapeModel(this);
    if((u32(model->mChannels)&shapePassBit(pass))==0) return;
    ShapeResourceHeader_l* header=resource->mHeader;
    u32 materialIndex=header->mMaterialIndex;
    u32 bindingIndex=header->mBindingIndex;
    u32 directProgram=shapeProgram(model,materialIndex,pass);
    PacketMaterialBinding_l* binding=gabi::at<PacketMaterialBinding_l>(gabi::ea(static_cast<PacketMaterialBinding_l*>(model->mMaterialState.mBindings))+bindingIndex*sizeof(PacketMaterialBinding_l));
    u32 view=input->mView;
    if(pass==2 && mDirty!=0) directProgram=shapeProgram(model,materialIndex,3);
    gabi::FrameLocal<sead::SafeText> name(8);
    FrameRecord* frame;
    void* program;
    if(model->mDirectPrograms!=0) { frame=shapeProgramFrame(directProgram); program=gabi::at<void>(directProgram); }
    else { frame=shapeNamedFrame(name.get(),"wii_pipeline"); program=fallbackShapeProgram(model,materialIndex); }
    UniformSlots_l* slots=shapeUniformSlots(frame,0);
    if(changeShapeShader(frame,true)) {
        if(pass==0) bindShapeUniforms(shapeAnimationBuffer(input->mViews,view),slots);
        else {
            if(view==2) {
                AnimationUniforms_l* views=input->mViews;
                bindShapeUniforms(reinterpret_cast<UniformBuffer_l*>(static_cast<AnimationUniform_l*>(views->mEntries)),slots);
            }
            model=shapeModel(this);
            bindShapeUniforms(shapeAnimationBuffer(reinterpret_cast<AnimationUniforms_l*>(&model->mAnimation[0]),view),slots);
        }
    }
    model=shapeModel(this);
    if(model->mDirectPrograms!=0) bindDirectShapeMaterial(program,binding,model->mRenderEnabled);
    else {
        ProgramUniforms_l* uniforms=static_cast<ProgramUniforms_l*>(program);
        bindShapeUniforms(reinterpret_cast<UniformBuffer_l*>(static_cast<ModelRenderState_l*>(model->mRenderState)),&uniforms->mSlots);
    }
    if(pass==2) bindShapeExtraUniform(input,frame);
    bindShapeFrameInput(input,frame);
    bindShapeMaterialState(gabi::at<void>(gabi::ea(program)+0x84),binding);
    bindShapeMaterialProgram(gabi::at<void>(gabi::ea(program)+0x18));
    if(pass!=0) {
        if(view!=2) {
            bindShapeDrawState(gabi::at<ModelMaterialDrawState_l>(shapeDrawState(shapeModel(this),bindingIndex)));
            if(pass==2) setShapeAlphaState(shapeStatePart(binding,0x18),1);
        }
    } else {
        gabi::FrameLocal<DrawFixedState_l> fixed(0x10);
        applyShapePassState(input,binding,fixed.get());
    }
    mProgram=frame;
}
/* WWHD glue
 ext 027FFCBC j3d::FrameRecords* shapeFrameRegistry()
 ext 027B90AC s32 findShapeFrame(j3d::FrameSource*,const sead::SafeText*)
 ext 02800B0C void activateShapeFrame(j3d::FrameRecord*,j3d::FrameSourceRecord*,void*)
 ext 027BB9E0 void refreshShapeShader(j3d::ShaderProgram_l*,u32)
 ext 027B9F68 void bindShapeShaderProgram(void*)
 ext C00060E0 void callShapeDisplayList(void*,u32)
 ext 027BB7CC void bindShapeShaderState(j3d::ShaderProgram_l*)
 ext C0006900 void bindShapePixelUniform(s32,u32,const void*)
 ext C0006A38 void bindShapeVertexUniform(s32,u32,const void*)
 ext C00068A8 void bindShapeGeometryUniform(s32,u32,const void*)
 ext 027E13B4 void bindShapeBuffer(void*,s32,u32)
 ext 02774778 void bindDirectShapeMaterial(void*,j3d::PacketMaterialBinding_l*,u32)
 ext 027FFE54 void bindShapeFrameInput(j3d::ShapeDrawInput_l*,j3d::FrameRecord*)
 ext 02774210 void bindShapeMaterialState(void*,j3d::PacketMaterialBinding_l*)
 ext 02773E1C void bindShapeMaterialProgram(void*)
 ext 027E18C4 void setShapeDrawStateA(void*,u32)
 ext 027E18D4 void setShapeDrawStateB(void*,u32)
 ext 027E220C void bindShapeDrawState(j3d::ModelMaterialDrawState_l*)
 ext 027E1984 void setShapeAlphaState(void*,u32)
 ext 0274FDBC void constructShapeFixedState(j3d::DrawFixedState_l*)
 ext 027E18AC u32 shapeStateOpaque(void*)
 ext 027E18B8 u32 shapeStateAlternative(void*)
 ext 0274FEB0 void applyShapeProjection(j3d::DrawProjection_l*)
 ext 027501D8 void applyShapeFixedState(j3d::DrawFixedState_l*)
 ext 027E1924 void applyShapeBlendState(void*)
 ext 027E27B8 void submitShapePrimitive(void*,u32,u32,u32)
 str 1016E2CC "wii_pipeline" in 027F16E8
 str 1016E2DC "wii_pipeline" in 027F1FA8
 data sShapeSubmissionTextMethods 1016E1F8
*/
