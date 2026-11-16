// J3D material descriptions (J3DNativeDesc.h): the game side of the port's native J3D draws. Game build with
// GABI_NATIVE_DRAW only; no function of the original lives here (the selection helpers of J3DShapeSubmission.cpp it
// needs are repeated, so that unit stays as it is without the switch).
#include "JSystem/J3DGraph/J3DNativeDesc.h"
// (the glue below declares the callees in every build; their types too)
#include "JSystem/J3DGraph/J3DShapeSubmission.h"
#include "JSystem/J3DGraph/J3DShapePacketResources.h"
#include "JSystem/J3DGraph/RenderTextureBindings.h"
#include "agl/ShaderCommandState.h"
#include "agl/gx2/BufferRuntime.h"
#include "agl/gx2/RenderResources.h"
#include "sead/ElementAt.h"
#include "agl/TextureSampler.h"
using namespace j3d;
// RenderTextureBindings::bindExtra's fallback textures (RenderTextureBindings.cpp, the same data)
struct DescFallbackTextures_l {
    u8 mPrefix[0x14];
    gptr<RenderTexture_l> mPrimary;
    u8 mGap[4];
    gptr<RenderTexture_l> mSecondary;
};
static DescFallbackTextures_l* sDescDefaultTextures;
static RenderTexture_l* sDescSystemTextureA;
static RenderTexture_l* sDescSystemTextureB;
void descRefreshShader(ShaderProgram_l*,u32);
void descSetShaderMode(void*);
void descBindStagePrograms(ShaderProgram_l*);
void descBindMaterialState(void*,PacketMaterialBinding_l*);
void descBindMaterialProgram(void*);
void descSetFrontOffset(void*,u32);
void descSetBackOffset(void*,u32);
void descBindDrawState(ModelMaterialDrawState_l*);
void descBindBlendTarget(void*,u32);
void descConstructFixedState(DrawFixedState_l*);
u32 descStateOpaque(void*);
u32 descStateAlternative(void*);
void descApplyProjection(DrawProjection_l*);
void descApplyFixedState(DrawFixedState_l*);
void descBindAlphaState(void*);
void descBindFrameInput(ShapeDrawInput_l*,FrameRecord*);
#if PORT_NATIVE_DRAW
extern j3d::RenderContext& sRenderContext;

namespace j3ddesc {
namespace {
// ---------------------------------------------------------------- the site's selection helpers (J3DShapeSubmission.cpp)
ModelState_l* model_of(ShapePacket* packet) { return reinterpret_cast<ModelState_l*>(static_cast<void*>(packet->mModel)); }
u32 draw_state(ModelState_l* model,u32 index) {
    if(index>=u32(model->mMaterialCount)) return 0;
    return gabi::ea(static_cast<void*>(model->mMaterials))+index*sizeof(ModelMaterialDrawState_l);
}
UniformBuffer_l* animation_buffer(AnimationUniforms_l* animations,u32 index) {
    AnimationUniform_l* entries=animations->mEntries;
    if(index<u32(animations->mCount)) entries+=index;
    return reinterpret_cast<UniformBuffer_l*>(entries);
}
UniformBufferSlot_l* active_buffer(UniformBuffer_l* buffer) {
    return gabi::at<UniformBufferSlot_l>(gabi::ea(buffer)+u32(buffer->mActive)*0x1C);
}
UniformSlots_l* uniform_slots(FrameRecord* record,u32 index) {
    FrameShader_l* shader=reinterpret_cast<FrameShader_l*>(record);
    FrameUniform_l* uniform=NULL;
    if(u32(shader->mUniformCount)>index) uniform=static_cast<FrameUniform_l*>(shader->mUniforms)+index;
    return gabi::at<UniformSlots_l>(gabi::ea(uniform)+0xC);
}
u32 state_part(PacketMaterialBinding_l* material,u32 offset) {
    PacketMaterialHeader_l* header=material->mHeader;
    u32 relative=header->mStateOffset;
    u32 base=relative==0?0:gabi::ea(&header->mStateOffset)+relative;
    return base+offset;
}
// ---------------------------------------------------------------- blocks (as the bind calls compute them)
// bindShapeUniforms: the active ring slot, pixel then vertex then geometry (MARK 0x100: the pass's extra block, whose
// ring index another thread may flip while the site draws: the port counts its differences as that race)
void uniform_blocks(UniformBuffer_l* buffer,UniformSlots_l* slots,u32 mark=0) {
    UniformBufferSlot_l* active=active_buffer(buffer);
    const u32 data=gabi::ea(static_cast<void*>(active->mData)),bytes=active->mBytes;
    const s32 vertex=slots->mVertex,pixel=slots->mPixel,geometry=slots->mGeometry;
    if(pixel!=-1) wwhd_j3ddesc_block(1|mark,u32(pixel),data,bytes);
    if(vertex!=-1) wwhd_j3ddesc_block(0|mark,u32(vertex),data,bytes);
    if(geometry!=-1) wwhd_j3ddesc_block(2|mark,u32(geometry),data,bytes);
}
// StridedBuffer::bind{Vertex,Geometry,Pixel}Uniform(slot, element): mutableSlice(element), mSliceSize
void strided_block(u32 stage,u32 slot,u32 buffer,u32 element) {
    agl::gx2util::StridedBuffer* b=gabi::at<agl::gx2util::StridedBuffer>(buffer);
    const u32 size=b->mSliceSize;
    wwhd_j3ddesc_block(stage,slot,gabi::ea(static_cast<u8*>(b->mData))+size*element,size);
}
// bindExtra's slot for output slot OUT (guestNull past the count) -> mSampler = SamplerLocations_l {prefix[8], vertex,
// pixel, geometry}: bindTextureSampler touches the sampler (its dirty update included) only when one of them is bound
bool frame_texture_bound(RenderTextureSlots_l* slots,u32 out) {
    const u32 slot=out<u32(slots->mCount)?
        gabi::ea(static_cast<RenderTextureSlot_l*>(slots->mSlots))+out*sizeof(RenderTextureSlot_l):0;
    const u32 locations=slot+offsetof(RenderTextureSlot_l,mSampler);
    return gabi::load<s16>(locations+8)!=-1||gabi::load<s16>(locations+0xA)!=-1||gabi::load<s16>(locations+0xC)!=-1;
}
// ---------------------------------------------------------------- key
struct KeyBuffer {
    unsigned w[kKeyFixedWords+4+16*4+1+32*2+1+64*2];
    unsigned n=kKeyFixedWords;
    void push(unsigned v) { if(n<sizeof(w)/sizeof(w[0])) w[n++]=v; }
};
}  // namespace

// (the port's write-set proof, debug) the game memory this draw's site writes outside its GX2 calls: the render
// context's core mode, the shader's flags and display list (refresh), the draw state (polygon-offset setters), the
// frame textures bindExtra binds (their dirty byte and sampler words). A frame texture only when its slot's sampler
// locations are bound: bindTextureSampler returns before it touches the sampler when all three are -1 (such entries
// can be stale pointers into memory other code reuses: seen at the cliff, 2026-11-15)
void watch_writes(ShapePacket* packet,ShapeDrawInput_l* input,FrameRecord* frame) {
    ModelState_l* model=model_of(packet);
    // (the current core's context: which core records a draw is the job system's choice, so the port hashes its
    // bytes without the address: flag 0x80000000)
    ContextMode* context=sRenderContext.currentCoreMode();
    wwhd_j3ddesc_watch(gabi::ea(context),8|0x80000000u);
    if(gabi::ea(frame)==0) return;
    ShaderProgram_l* shader=reinterpret_cast<FrameShader_l*>(frame)->mShader;
    if(gabi::ea(shader)!=0) {
        wwhd_j3ddesc_watch(gabi::ea(shader),4);
        if(u32(shader->mDisplayListBytes)!=0)
            wwhd_j3ddesc_watch(gabi::ea(static_cast<void*>(shader->mDisplayList)),shader->mDisplayListBytes);
    }
    ShapeResource_l* resource=static_cast<ShapeResource_l*>(static_cast<void*>(packet->mResource));
    const u32 draw=draw_state(model,resource->mHeader->mBindingIndex);
    if(draw!=0) wwhd_j3ddesc_watch(draw,sizeof(ModelMaterialDrawState_l));
    static const u32 sourceIndices[5]={6,8,9,10,11};
    static const u32 outputSlots[7]={8,10,11,9,12,13,14};
    RenderTextureSlots_l* slots=reinterpret_cast<RenderTextureSlots_l*>(frame);
    const u32 primary=gabi::ea(static_cast<RenderTexture_l*>(sDescDefaultTextures->mPrimary));
    const u32 secondary=gabi::ea(static_cast<RenderTexture_l*>(sDescDefaultTextures->mSecondary));
    for(u32 i=0;i<7;++i) {
        u32 texture;
        if(i<5) {
            texture=gabi::load<u32>(gabi::ea(input)+sourceIndices[i]*4);
            if(texture==0) texture=i==3?secondary:primary;
        } else {
            texture=gabi::ea(i==5?sDescSystemTextureA:sDescSystemTextureB);
            if(texture==0) texture=secondary;
        }
        if(texture!=0&&frame_texture_bound(slots,outputSlots[i])) wwhd_j3ddesc_watch(texture,sizeof(agl::TextureSampler));
    }
}

unsigned submit(ShapePacket* packet,ShapeDrawInput_l* input,FrameRecord* frame,void* program,unsigned directProgram,
                PacketMaterialBinding_l* binding,unsigned bindingIndex) {
    if(wwhd_j3ddesc_writeset_on()) watch_writes(packet,input,frame);
    const unsigned mode=wwhd_j3ddesc_mode();
    if(mode==0) return 0;
    ModelState_l* model=model_of(packet);
    const u32 pass=input->mPass,view=input->mView;
    if(model->mDirectPrograms==0) { wwhd_j3ddesc_skip(kNamedFrame); return 0; }
    if(directProgram==0||gabi::ea(frame)==0) { wwhd_j3ddesc_skip(kNoProgram); return 0; }
    FrameShader_l* frameShader=reinterpret_cast<FrameShader_l*>(frame);
    ShaderProgram_l* shader=frameShader->mShader;
    // the site's game-memory writes that do not depend on the GX2 calls: the shader refresh (changeShapeShader: only
    // when the shader is not the context's) and the polygon-offset setters (buffer type 1, passes 1/2). Made here
    // exactly as the site makes them; the site then finds them made (the same writes again: none).
    ContextMode* context=sRenderContext.currentCoreMode();
    u32 refreshed=0;
    // the site binds the program in this draw (changeShapeShader: the shader is not the context's)
    if(gabi::ea(shader)!=u32(context->mArgument)) wwhd_j3ddesc_flag(1);
    if(gabi::ea(shader)!=u32(context->mArgument)) {
        const u32 flags=shader->mFlags;
        if((flags&2)!=0) {
            shader->mFlags=flags&~u32(2);
            descRefreshShader(shader,0);
            refreshed=1;
        }
    }
    const u32 draw=draw_state(model,bindingIndex);
    if(pass!=0&&u32(packet->mBufferType)==1&&(gabi::load<u8>(draw+offsetof(ModelMaterialDrawState_l,mDirty))&1)==0) {
        descSetFrontOffset(gabi::at<void>(draw+4),1);
        descSetBackOffset(gabi::at<void>(draw+4),1);
    }
    // ---- the description key
    KeyBuffer key;
    unsigned* k=key.w;
    ShaderState_l* state=shader->mState;
    k[kKeyBinding]=gabi::ea(binding);
    k[kKeyPass]=pass;
    k[kKeyRecord]=directProgram;
    k[kKeyFrame]=gabi::ea(frame);
    k[kKeyProgram]=gabi::ea(program);
    k[kKeyViewClass]=view==2?1:0;
    k[kKeyShader]=gabi::ea(shader);
    k[kKeyShaderFlags]=u32(shader->mFlags);
    k[kKeyRefreshed]=refreshed;
    k[kKeyShaderState]=gabi::ea(state);
    k[kKeyShaderMode]=gabi::ea(static_cast<void*>(state->mProgram));
    k[kKeyList]=gabi::ea(static_cast<void*>(shader->mDisplayList));
    k[kKeyListBytes]=shader->mDisplayListBytes;
    for(u32 i=0;i<12;++i) k[kKeyDrawState+i]=draw==0?0:gabi::load<u32>(draw+i*4);
    k[kKeyDrawDirty]=(draw==0?0:u32(gabi::load<u8>(draw+offsetof(ModelMaterialDrawState_l,mDirty))))|u32(packet->mBufferType)<<8;
    const u32 part=state_part(binding,0);
    for(u32 i=0;i<10;++i) k[kKeyStatePart+i]=part==0?0:gabi::load<u32>(part+i*4);
    DrawProjection_l* projection=input->mProjection;
    k[kKeyProjectionFlags]=pass==0&&projection!=NULL?
        u32(projection->mFlags[0])<<16|u32(projection->mFlags[1])<<8|u32(projection->mFlags[2]):0;
    agl::environment::ProgramInstance* instance=static_cast<agl::environment::ProgramInstance*>(program);
    agl::environment::DisplayCommandState* display=&instance->mDisplay;
    const u32 fetch=gabi::ea(&display->mList);
    k[kKeyFetch]=gabi::load<u32>(fetch+8);
    k[kKeyFetchProgram]=gabi::load<u32>(fetch+0xC);
    agl::environment::PixelCommandState* pixel=&instance->mPixel;
    const u32 entries=gabi::ea(static_cast<be<u32>*>(pixel->mActiveBindings.mEntries)),samplers=pixel->mActiveBindings.mCount;
    k[kKeySamplerTable]=entries;
    k[kKeySamplerCount]=samplers;
    // vertex streams (DisplayCommandState::bindStreams: StridedBuffer::bindAttribute(stream, 0))
    const u32 streams=u8(display->mActive);
    key.push(streams);
    for(u32 s=0;s<streams&&s<16;++s) {
        const u32 b=gabi::ea(static_cast<void*>(display->mSlots[s]));
        agl::gx2util::StridedBuffer* sb=gabi::at<agl::gx2util::StridedBuffer>(b);
        key.push(b);
        key.push(b==0?0:gabi::ea(static_cast<u8*>(sb->mData)));
        key.push(b==0?0:u32(sb->mSliceSize));
        key.push(b==0?0:u32(sb->mElementStride));
    }
    // material samplers (PixelCommandState::bindSamplerStages(binding)): key and sampler object per active binding
    const u32 table=gabi::load<u32>(gabi::ea(binding)+0x2C);
    const u32 samplerCount=samplers<32?samplers:32;
    key.push(samplerCount);
    u32 objects[32];
    for(u32 i=0;i<samplerCount;++i) {
        const u32 keyEa=gabi::load<u32>(entries+i*4);
        const u32 slot=gabi::load<u8>(keyEa+0x14);
        const u32 sampler=gabi::load<u32>(table+slot*4);
        key.push(keyEa);
        key.push(sampler);
        objects[i]=sampler;
    }
    // content ranges, checked once per game frame: each sampler object's GX2Texture (+4, 0x9C bytes) and each key's
    // GX2Sampler words (0x14 bytes)
    u32 ranges=0;
    for(u32 i=0;i<samplerCount;++i) ranges+=objects[i]!=0?2:0;
    key.push(ranges);
    for(u32 i=0;i<samplerCount;++i) {
        if(objects[i]==0) continue;
        key.push(objects[i]+4);
        key.push(0x9C);
        key.push(gabi::load<u32>(entries+i*4));
        key.push(0x14);
    }
    const unsigned begun=wwhd_j3ddesc_begin(key.w,key.n);
    if(begun==0) return 0;
    if(begun==2) {
        // define: the site's own bind callees, captured (never emitted), in the site's order
        wwhd_j3ddesc_segment(1);
        descSetShaderMode(state->mProgram);
        if(u32(shader->mDisplayListBytes)!=0)
            wwhd_j3ddesc_list(gabi::ea(static_cast<void*>(shader->mDisplayList)),shader->mDisplayListBytes);
        else
            descBindStagePrograms(shader);
        wwhd_j3ddesc_segment(2);
        descBindMaterialState(gabi::at<void>(gabi::ea(program)+0x84),binding);
        descBindMaterialProgram(gabi::at<void>(gabi::ea(program)+0x18));
        wwhd_j3ddesc_segment(3);
        if(pass!=0) {
            if(view!=2) {
                descBindDrawState(gabi::at<ModelMaterialDrawState_l>(draw));
                if(pass==2) descBindBlendTarget(gabi::at<void>(state_part(binding,0x18)),1);
            }
        } else {
            // applyShapePassState without the projection's own state (that is the env's), then the alpha state
            gabi::Local<DrawFixedState_l> fixed;
            descConstructFixedState(fixed.get());
            u32 fixedMode;
            if(descStateOpaque(gabi::at<void>(state_part(binding,4)))!=0) fixedMode=0;
            else fixedMode=descStateAlternative(gabi::at<void>(state_part(binding,4)))!=0?1:2;
            fixed->mMode=fixedMode;
            if(projection!=NULL) {
                fixed->mFlags[0]=u32(projection->mFlags[0]);
                fixed->mFlags[1]=u32(projection->mFlags[1]);
                fixed->mFlags[2]=u32(projection->mFlags[2]);
            }
            descApplyFixedState(fixed.get());
            descBindAlphaState(gabi::at<void>(state_part(binding,0xC)));
        }
        wwhd_j3ddesc_segment(0);
        if(wwhd_j3ddesc_define()==0) return 0;
    }
    // ---- env: the pass's frame textures (RenderTextureBindings::bindExtra on the input), pass 0: the projection
    {
        unsigned e[kEnvKeyWords];
        e[kEnvInput]=gabi::ea(input);
        e[kEnvFrame]=gabi::ea(frame);
        e[kEnvPass]=pass;
        e[kEnvView]=view;
        e[kEnvProjection]=pass==0?gabi::ea(projection):0;
        static const u32 sourceIndices[7]={6,8,9,10,11,0xFF,0xFF};
        static const u32 outputSlots[7]={8,10,11,9,12,13,14};
        RenderTextureSlots_l* slots=reinterpret_cast<RenderTextureSlots_l*>(frame);
        u32 dirty=0;
        // the textures bindExtra binds, after its fallbacks
        const u32 primary=gabi::ea(static_cast<RenderTexture_l*>(sDescDefaultTextures->mPrimary));
        const u32 secondary=gabi::ea(static_cast<RenderTexture_l*>(sDescDefaultTextures->mSecondary));
        for(u32 i=0;i<7;++i) {
            u32 texture;
            if(i<5) {
                texture=gabi::load<u32>(gabi::ea(input)+sourceIndices[i]*4);
                if(texture==0) texture=i==3?secondary:primary;
            } else {
                texture=gabi::ea(i==5?sDescSystemTextureA:sDescSystemTextureB);
                if(texture==0) texture=secondary;
            }
            e[kEnvTextures+i]=texture;
            e[kEnvDirty+i]=texture==0?0:gabi::load<u8>(texture+offsetof(agl::TextureSampler,dirty));
            // a dirty bound texture: the site's bind updates it (updateSamplerState: sampler words, dirty = 0), so the
            // env is captured (the capture runs that bind); the key's dirty bytes alone can repeat an earlier capture's
            if(e[kEnvDirty+i]!=0&&frame_texture_bound(slots,outputSlots[i])) dirty=1;
            const u32 slot=outputSlots[i];
            e[kEnvSlots+i]=slot<u32(slots->mCount)?gabi::load<u32>(gabi::ea(static_cast<RenderTextureSlot_l*>(slots->mSlots))+slot*sizeof(RenderTextureSlot_l)+4):0;
        }
        if(dirty) wwhd_j3ddesc_flag(2);
        const unsigned envState=wwhd_j3ddesc_env(e,kEnvKeyWords);
        if(envState==0) return 0;
        if(envState==2) {
            wwhd_j3ddesc_segment(4);
            descBindFrameInput(input,frame);
            if(pass==0&&projection!=NULL) descApplyProjection(projection);
            wwhd_j3ddesc_segment(0);
            if(wwhd_j3ddesc_env_define()==0) return 0;
        }
    }
    // ---- mesh: IndexedMesh::draw(0, count, 1) of the shape's primitive
    {
        ShapeResource_l* resource=static_cast<ShapeResource_l*>(static_cast<void*>(packet->mResource));
        ShapeResourceHeader_l* header=resource->mHeader;
        const u32 offset=header->mPrimitiveOffset;
        const u32 primitive=offset==0?0:gabi::ea(&header->mPrimitiveOffset)+offset;
        const u32 count=gabi::load<u16>(primitive+offsetof(PacketPrimitive_l,mIndex));
        agl::gx2util::IndexedMesh* mesh=gabi::at<agl::gx2util::IndexedMesh>(primitive);
        const u32 format=mesh->mIndexFormat;
        agl::gx2util::IndexRange* ranges=mesh->mRanges.get();
        const u32 startOffset=sead::elementAt(ranges,0).mByteOffset;
        const u32 last=count-1u;
        const u32 finalOffset=sead::elementAt(ranges,last).mByteOffset;
        const u32 finalCount=sead::elementAt(ranges,last).mCount;
        const u32 indices=((finalOffset-startOffset)>>((format&1u)+1u))+finalCount;
        agl::gx2util::StridedBuffer* buffer=mesh->mBuffer.get();
        wwhd_j3ddesc_mesh(mesh->mPrimitive,indices,format,gabi::ea(static_cast<u8*>(buffer->mData))+startOffset,
                          mesh->mBaseVertex);
    }
    // ---- blocks, in the site's bind order
    {
        UniformSlots_l* viewSlots=uniform_slots(frame,0);
        UniformSlots_l* modelSlots=uniform_slots(frame,1);
        UniformSlots_l* shapeSlots=uniform_slots(frame,2);
        if(pass==0) uniform_blocks(animation_buffer(input->mViews,view),viewSlots);
        else if(view==2) uniform_blocks(reinterpret_cast<UniformBuffer_l*>(static_cast<AnimationUniform_l*>(input->mViews->mEntries)),viewSlots);
        else uniform_blocks(animation_buffer(reinterpret_cast<AnimationUniforms_l*>(&model->mAnimation[0]),view),viewSlots);
        const u32 element=model->mRenderEnabled;
        const s32 modelSlot=modelSlots->mVertex;
        if(modelSlot!=-1) {
            ModelUniformBindings_l* bindings=gabi::at<ModelUniformBindings_l>(gabi::ea(&model->mMaterialState)-0x14);
            const u32 matrix=gabi::ea(static_cast<void*>(bindings->mMatrix));
            if(gabi::load<u32>(matrix+0x1C)!=0) strided_block(0,u32(modelSlot),matrix+0x18,element);
        }
        const s32 shapeSlot=shapeSlots->mVertex;
        if(shapeSlot!=-1) {
            ShapeBufferSource_l* source=static_cast<ShapeBufferSource_l*>(static_cast<void*>(packet->mResource));
            const u32 multiplier=source->mBufferCount;
            const u32 offset=(view==2?multiplier:view*multiplier)*0x14;
            strided_block(0,u32(shapeSlot),gabi::ea(static_cast<void*>(source->mBuffers))+offset,element);
        }
        // ProgramInstance::bindUniformBlocks(binding, element): vertex, geometry, pixel
        const s32 v=instance->mVertexIndex,p=instance->mPixelIndex,g=instance->mGeometryIndex;
        if((v!=-1||p!=-1||g!=-1)&&gabi::load<u32>(gabi::ea(binding)+0x18)!=0) {
            const u32 parameters=gabi::ea(binding)+0x14;
            if(v!=-1) strided_block(0,u32(v),parameters,element);
            if(g!=-1) strided_block(2,u32(g),parameters,element);
            if(p!=-1) strided_block(1,u32(p),parameters,element);
        }
        if(pass==2&&input->mExtra!=NULL) uniform_blocks(input->mExtra,uniform_slots(frame,4),0x100);
    }
    if(wwhd_j3ddesc_draw()==0) return 0;
    // the port draws it (mode 2): the site's remaining game-memory write, changeShapeShader's render context
    if(gabi::ea(shader)!=u32(context->mArgument)) {
        context->mMode=gabi::ea(static_cast<void*>(state->mProgram));
        context->mArgument=gabi::ea(shader);
    }
    if(wwhd_j3ddesc_writeset_on()) wwhd_j3ddesc_watch_done();
    return 1;
}
}  // namespace j3ddesc
#endif
/* WWHD glue
 ext 027BB9E0 void descRefreshShader(j3d::ShaderProgram_l*,u32)
 ext 027B9F68 void descSetShaderMode(void*)
 ext 027BB7CC void descBindStagePrograms(j3d::ShaderProgram_l*)
 ext 02774210 void descBindMaterialState(void*,j3d::PacketMaterialBinding_l*)
 ext 02773E1C void descBindMaterialProgram(void*)
 ext 027E18C4 void descSetFrontOffset(void*,u32)
 ext 027E18D4 void descSetBackOffset(void*,u32)
 ext 027E220C void descBindDrawState(j3d::ModelMaterialDrawState_l*)
 ext 027E1984 void descBindBlendTarget(void*,u32)
 ext 0274FDBC void descConstructFixedState(j3d::DrawFixedState_l*)
 ext 027E18AC u32 descStateOpaque(void*)
 ext 027E18B8 u32 descStateAlternative(void*)
 ext 0274FEB0 void descApplyProjection(j3d::DrawProjection_l*)
 ext 027501D8 void descApplyFixedState(j3d::DrawFixedState_l*)
 ext 027E1924 void descBindAlphaState(void*)
 ext 027FFE54 void descBindFrameInput(j3d::ShapeDrawInput_l*,j3d::FrameRecord*)
 data sDescDefaultTextures 101F8DEC
 data sDescSystemTextureA 104B45C0
 data sDescSystemTextureB 104B45C4
*/
