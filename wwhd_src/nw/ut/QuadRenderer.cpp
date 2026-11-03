/* Library quad rendering and shader setup, reconstructed from WWHD. */
#include "nw/ut/QuadRenderer.h"
#include "sead/ElementAt.h"
using namespace nw::ut;
struct StreamDefaults { be<u32> words[20]; };
static StreamDefaults sStreamDefaults;
static u8 sRendererVtable;
/* 028F6CD8 */
u32 QuadRenderer::requiredSize(u32 count,const gptr<const void>* archive) {
    u32 total=count*8;
    for(u32 variant=0;variant<3;++variant) {
        u32 size=quadFetchSize(3,0,0)+0x13E;
        size+=quadVertexHeaderSize(variant,*archive)+0xFF;
        size+=quadVertexProgramSize(variant,*archive)+0x3F;
        size+=quadPixelHeaderSize(variant,*archive)+0xFF;
        size+=quadPixelProgramSize(variant,*archive)+0x21F;
        total+=size;
    }
    return total;
}
/* 028F6D84 */
ShaderState* ShaderState::create(ShaderState* shader) {
    if(!shader) shader=(ShaderState*)quadAllocate(sizeof(ShaderState));
    if(shader) {
        shader->fetchProgram=0;shader->displayCapacity=0;shader->vertex=nullptr;
        shader->positionLocation=0;shader->transformUniform=0;shader->shadowOffset=0;
        shader->uvLocation=0;shader->interpolateWidth=0;shader->interpolateOffset=0;
        shader->colorLocation=0;shader->shadowWidth=0;shader->displaySize=0;
        shader->displayList=0;shader->textureUnit=0;shader->pixel=nullptr;
    }
    return shader;
}
/* 028F6DF8 */
QuadRenderer::QuadRenderer() {
    vtable=gabi::ea(&sRendererVtable);capacity=0;
    quadConstructArray(shaders,3,sizeof(ShaderState),gabi::guest_fn((const void*)quadConstructShader));
    indices=0;
}
/* 028F6E70 */
u32 ShaderState::loadVertex(gptr<VertexShader>* output,u32 index,const void* archive,u32 memory) {
    if(!archive || !output || !memory) return 0;
    if(index>=quadVertexCount(archive)) return 0;
    u32 headerSize=quadVertexHeaderSize(index,archive);
    u32 programSize=quadVertexProgramSize(index,archive);
    if(!headerSize || !programSize) return 0;
    u32 header=(memory+63)&~63u;
    u32 program=(header+headerSize+255)&~255u;
    u32 end=program+programSize;
    VertexShader* shader=gabi::at<VertexShader>(header);
    if(!quadLoadVertex(shader,program,index,archive)) return 0;
    quadInvalidate(0x40,shader->program,shader->programSize);
    *output=shader;
    return end;
}
/* 028F6F58 */
u32 ShaderState::loadPixel(gptr<PixelShader>* output,u32 index,const void* archive,u32 memory) {
    if(!archive || !output || !memory) return 0;
    if(index>=quadPixelCount(archive)) return 0;
    u32 headerSize=quadPixelHeaderSize(index,archive);
    u32 programSize=quadPixelProgramSize(index,archive);
    if(!headerSize || !programSize) return 0;
    u32 header=(memory+63)&~63u;
    u32 program=(header+headerSize+255)&~255u;
    u32 end=program+programSize;
    PixelShader* shader=gabi::at<PixelShader>(header);
    if(!quadLoadPixel(shader,program,index,archive)) return 0;
    quadInvalidate(0x40,shader->program,shader->programSize);
    *output=shader;
    return end;
}
static bool nameEquals(const be<u8>* text,const char* name) {
    for(u32 index=0;;++index) {
        u8 a=text[index],b=name[index];
        if(a!=b) return false;
        if(!a) return true;
    }
}
static u32 findBlock(const ShaderUniform* uniforms,u32 count,const char* name) {
    for(u32 i=0;i<count;++i) if(nameEquals(uniforms[i].name,name)) return uniforms[i].location;
    return 0xFFFFFFFF;
}
static u32 findAttribute(const ShaderAttribute* attributes,u32 count,const char* name) {
    for(u32 i=0;i<count;++i) if(nameEquals(attributes[i].name,name)) return attributes[i].location;
    return 0xFFFFFFFF;
}
static u32 findSampler(const ShaderSampler* samplers,u32 count,const char* name) {
    for(u32 i=0;i<count;++i) if(nameEquals(samplers[i].name,name)) return samplers[i].location;
    return 0xFFFFFFFF;
}
/* 028F7040 */
u32 ShaderState::initialize(u32 memory,u32 variant,const gptr<const void>* archive) {
    fetchProgram=memory;
    u32 fetchSize=quadFetchSize(3,0,0);
    u32 next=(u32)fetchProgram+fetchSize;
    quadVertexCount(*archive);
    next=loadVertex(&vertex,variant,*archive,next);
    quadPixelCount(*archive);
    next=loadPixel(&pixel,variant,*archive,next);
    VertexShader* vertexShader=vertex;
    transformUniform=findBlock(vertexShader->blocks,vertexShader->blockCount,"uUser");
    vertexShader=vertex;
    positionLocation=findAttribute(vertexShader->attributes,vertexShader->attributeCount,"aVertex");
    vertexShader=vertex;
    colorLocation=findAttribute(vertexShader->attributes,vertexShader->attributeCount,"aColor");
    vertexShader=vertex;
    uvLocation=findAttribute(vertexShader->attributes,vertexShader->attributeCount,"aTexCoord0");
    PixelShader* pixelShader=pixel;
    textureUnit=findSampler(pixelShader->samplers,pixelShader->samplerCount,"uTextureSrc");
    pixelShader=pixel;
    interpolateOffset=findBlock(pixelShader->blocks,pixelShader->blockCount,"uInterpolateOffset");
    pixelShader=pixel;
    interpolateWidth=findBlock(pixelShader->blocks,pixelShader->blockCount,"uInterpolateWidth");
    pixelShader=pixel;
    shadowOffset=findBlock(pixelShader->blocks,pixelShader->blockCount,"uShadowInterpolateOffset");
    pixelShader=pixel;
    shadowWidth=findBlock(pixelShader->blocks,pixelShader->blockCount,"uShadowInterpolateWidth");
    streams[0].location=positionLocation;streams[0].indexType=0;streams[0].format=0x811;
    streams[0].endian=3;streams[0].divisor=0;streams[0].mask=sStreamDefaults.words[17];streams[0].offset=0;streams[0].buffer=0;
    streams[1].location=colorLocation;streams[1].endian=3;streams[1].indexType=0;
    streams[1].mask=sStreamDefaults.words[10];streams[1].offset=12;streams[1].format=10;streams[1].divisor=0;streams[1].buffer=0;
    streams[2].location=uvLocation;streams[2].buffer=0;streams[2].endian=3;
    streams[2].offset=16;streams[2].format=0x811;streams[2].mask=sStreamDefaults.words[17];streams[2].indexType=0;streams[2].divisor=0;
    quadInitializeFetch(&fetch,fetchProgram,3,streams,0,0);
    quadInvalidate(0x40,fetchProgram,fetchSize);
    return next;
}
/* 028F78AC */
void QuadRenderer::initializeIndices(be<u16>* output,u32 count) {
    for(u32 quad=0;quad<count;++quad) {
        u32 base=quad*4;
        output[base]=base;output[base+1]=base+2;output[base+2]=base+3;output[base+3]=base+1;
    }
}
/* 028F78F4 */
void QuadRenderer::initialize(u32 memory,u32 count,const gptr<const void>* archive) {
    indices=memory;capacity=count;
    quadInitializeSampler(sampler,0,1);
    quadSamplerClamping(sampler,1,0);
    u32 size=count*8;
    u32 next=(memory+size+255)&~255u;
    for(u32 variant=0;variant<3;++variant) {
        ShaderState* shader=&shaders[variant];
        u32 end=shader->initialize(next,variant,archive);
        shader->displayCapacity=0x200;
        u32 list=(end+31)&~31u;
        shader->displayList=list;
        quadInvalidate(0x40,list,0x200);
        u32 display=shader->displayList;
        if(!display || !(u32)shader->displaySize) {
            display=shader->displayList;
            if(display) quadBeginList(display,shader->displayCapacity,1);
            VertexShader* vertex=shader->vertex;
            PixelShader* pixel=shader->pixel;
            quadApplyFetch(&shader->fetch);
            quadApplyVertex(vertex);
            quadApplyPixel(pixel);
            quadApplySampler(sampler,shader->textureUnit);
            display=shader->displayList;
            if(display) shader->displaySize=quadEndList(display);
        }
        next=(list+0x2FF)&~255u;
    }
    initializeIndices(gabi::at<be<u16>>(indices),count);
    quadInvalidate(0x40,indices,size);
}
/* 028F823C */
void ShaderState::apply() {
    u32 size=displaySize;
    if(size) quadCallList(displayList,size);
}
/* 028F7A4C */
void QuadRenderer::group(QuadBatch* batch,u32 shadow) {
    u32 remaining=batch->quadCount;
    u32 count=0;
    batch->groupCount=0;
    for(u32 index=0;remaining;--remaining,++index) {
        Quad* quad=&batch->quads[index];
        Texture* texture=quad->texture;
        u32 groupIndex=0;
        while(groupIndex<count && batch->groups[groupIndex].texture.get()!=texture) ++groupIndex;
        if(groupIndex<count) {
            ++batch->groups[groupIndex].quadCount;
            count=batch->groupCount;
            continue;
        }
        DrawGroup* entry=&batch->groups[count];
        entry->texture=texture;entry->quadCount=1;entry->flags=0;
        u32 flags=0;
        if((u8)texture->transformed) { flags=1;entry->flags=flags; }
        if((u8)quad->flags&1) entry->flags=flags|2;
        count=(u32)batch->groupCount+1;
        batch->groupCount=count;
        if(count>=8) break;
    }
    if(shadow && count>1) {
        u32 originalCount=count;
        u32 doubled=count*2;
        if(doubled>8) doubled=8;
        batch->groupCount=doubled;
        u32 offset=0;
        for(u32 i=0;i<originalCount;++i) {
            u32 quads=batch->groups[i].quadCount;
            batch->groups[i].firstVertex=offset;
            offset+=quads*4;
        }
        for(u32 index=originalCount,source=0;index<(u32)batch->groupCount;++index,++source) {
            batch->groups[index].firstVertex=offset;
            batch->groups[index].texture=batch->groups[source].texture;
            u32 quads=batch->groups[source].quadCount;
            batch->groups[index].quadCount=quads;
            batch->groups[index].flags=batch->groups[source].flags;
            offset+=quads*4;
        }
    } else {
        u32 offset=0;
        for(u32 index=0;index<count;++index) {
            batch->groups[index].firstVertex=offset;
            u32 quads=batch->groups[index].quadCount;
            if(shadow) { quads*=2;batch->groups[index].quadCount=quads; }
            count=batch->groupCount;
            offset+=quads*4;
        }
    }
}
/* 028F7C7C */
void QuadRenderer::build(QuadBatch* batch,const ShadowParameters* shadow) {
    u32 count=batch->quadCount;
    u32 total=count*(shadow?2:1);
    if(!total) { batch->groupCount=0;batch->vertexCount=0;batch->dirty=1;return; }
    if((u32)capacity<total || (u32)batch->capacity<total) return;
    group(batch,shadow!=nullptr);
    u32 buffer=(u32)batch->bufferIndex+1;
    if(buffer>=2) buffer=0;
    batch->bufferIndex=buffer;
    u32 memory=batch->vertexBuffers[buffer];
    u32 size=total*0x70;
    if(size>0x40000) size=0x40000;
    u32 aligned=(memory+31)&~31u;
    quadZeroRange(aligned,(memory+size-aligned)&~31u);
    for(u32 i=0;i<(u32)batch->groupCount;++i) batch->groups[i].quadCount=0;
    if(shadow) {
        for(u32 index=0;index<(u32)batch->quadCount;++index) {
            Quad* quad=&batch->quads[index];
            u32 active=batch->bufferIndex;
            Vertex* output=gabi::at<Vertex>(batch->vertexBuffers[active]);
            Texture* texture=quad->texture;
            for(u32 groupIndex=0;groupIndex<(u32)batch->groupCount;++groupIndex) {
                DrawGroup* entry=&batch->groups[groupIndex];
                if(entry->texture.get()==texture) {
                    u32 used=entry->quadCount;
                    u32 first=entry->firstVertex;
                    entry->quadCount=used+1;
                    output+=first+used*4;
                    break;
                }
            }
            f32 height=quad->height;
            f32 scaledHeight=height*(f32)shadow->heightScale;
            f32 inset=height-scaledHeight;
            f32 left=(f32)quad->x+(f32)shadow->x;
            f32 top=(f32)quad->y-(f32)shadow->y;
            f32 bearing=(f32)(s16)quad->bearing;
            f32 bearingScale=shadow->bearingScale;
            f32 advance=gabi::fmadds((f32)shadow->advance,bearingScale,bearing);
            top=top+inset;
            f32 shiftedLeft=left+advance;
            f32 u=quad->left,v=quad->top,width=quad->width;
            output[0].y=top;output[0].x=shiftedLeft;output[0].z=0;
            output[0].color=shadow->topColor;output[0].u=u;output[0].v=v;
            output[0].layer=-(f32)(u8)quad->layer-1.0f;
            f32 right=gabi::fmadds(width,bearingScale,left);
            u=quad->right;v=quad->top;
            output[1].y=top;output[1].x=right+advance;output[1].z=0;
            output[1].color=shadow->topColor;output[1].u=u;output[1].v=v;
            output[1].layer=-(f32)(u8)quad->layer-1.0f;
            u=quad->left;f32 bottom=top+scaledHeight;v=quad->bottom;
            output[2].z=0;output[2].y=bottom;output[2].x=left;
            output[2].color=shadow->bottomColor;output[2].v=v;output[2].u=u;
            output[2].layer=-(f32)(u8)quad->layer-1.0f;
            v=quad->bottom;u=quad->right;
            output[3].y=bottom;output[3].x=right;output[3].z=0;
            output[3].color=shadow->bottomColor;output[3].u=u;
            output[3].layer=-(f32)(u8)quad->layer-1.0f;output[3].v=v;
        }
    }
    for(u32 index=0;index<(u32)batch->quadCount;++index) {
        Quad* quad=&batch->quads[index];
        u32 active=batch->bufferIndex;
        u32 groups=batch->groupCount;
        Texture* texture=quad->texture;
        Vertex* output=gabi::at<Vertex>(batch->vertexBuffers[active]);
        for(s32 groupIndex=(s32)(groups-1);groupIndex>=0;--groupIndex) {
            DrawGroup* entry=&batch->groups[groupIndex];
            if(entry->texture.get()==texture) {
                u32 used=entry->quadCount,first=entry->firstVertex;
                entry->quadCount=used+1;output+=first+used*4;break;
            }
        }
        f32 bearing=(f32)(s16)quad->bearing;
        f32 y=quad->y,u=quad->left,v=quad->top;
        f32 x=quad->x,width=quad->width,height=quad->height;
        f32 shifted=x+bearing;
        output[0].y=y;output[0].z=0;output[0].x=shifted;
        output[0].color=quad->topColor;output[0].u=u;output[0].v=v;output[0].layer=(f32)(u8)quad->layer;
        f32 right=x+width;f32 shiftedRight=right+bearing;
        v=quad->top;u=quad->right;
        output[1].y=y;output[1].x=shiftedRight;output[1].z=0;
        output[1].color=quad->topColor;output[1].u=u;output[1].layer=(f32)(u8)quad->layer;output[1].v=v;
        u=quad->left;y=y+height;v=quad->bottom;
        output[2].y=y;output[2].x=x;output[2].z=0;
        output[2].color=quad->bottomColor;output[2].layer=(f32)(u8)quad->layer;output[2].v=v;output[2].u=u;
        u=quad->right;v=quad->bottom;
        output[3].y=y;output[3].x=right;output[3].z=0;
        output[3].color=quad->bottomColor;output[3].u=u;output[3].layer=(f32)(u8)quad->layer;output[3].v=v;
    }
    batch->dirty=1;batch->vertexCount=total*28;
    u32 active=batch->bufferIndex;
    quadInvalidate(0x40,batch->vertexBuffers[active],(u32)batch->vertexCount*4);
}
// Paired-single matrix products round each multiply-add to single precision.
static void multiplyMatrix(Matrix4* output,const Matrix4* left,const Matrix4* right) {
    for(u32 row=0;row<4;++row) for(u32 column=0;column<4;++column) {
        f32 value=(f32)right->values[0][column]*(f32)left->values[row][0];
        value=gabi::fmadds((f32)right->values[1][column],(f32)left->values[row][1],value);
        value=gabi::fmadds((f32)right->values[2][column],(f32)left->values[row][2],value);
        value=gabi::fmadds((f32)right->values[3][column],(f32)left->values[row][3],value);
        output->values[row][column]=value;
    }
}
static void copyAffine(Matrix4* output,const MatrixRows3* source) {
    be<u32>* words=(be<u32>*)output;
    for(u32 row=0;row<3;++row) for(u32 column=0;column<4;++column) words[row*4+column]=source->values[row][column];
    output->values[3][0]=0;output->values[3][1]=0;output->values[3][2]=0;output->values[3][3]=1;
}
/* 028F8250 */
void QuadRenderer::draw(const DrawParameters* parameters) {
    gabi::Local<Matrix4> affine,viewProjection,transform;
    copyAffine(affine.get(),parameters->view);
    multiplyMatrix(viewProjection.get(),parameters->projection,affine.get());
    copyAffine(affine.get(),parameters->model);
    multiplyMatrix(transform.get(),viewProjection.get(),affine.get());
    struct Uniform { be<f32> values[4]; };
    gabi::Local<Uniform> interpolateWidth,shadowWidth,zero,one;
    for(u32 i=0;i<3;++i) interpolateWidth->values[i]=(f32)parameters->interpolateTo[i]-(f32)parameters->interpolateFrom[i];
    interpolateWidth->values[3]=parameters->interpolateTo[3];
    for(u32 i=0;i<3;++i) shadowWidth->values[i]=(f32)parameters->shadowTo[i]-(f32)parameters->shadowFrom[i];
    shadowWidth->values[3]=parameters->shadowTo[3];
    QuadBatch* batch=parameters->batch;
    u32 count=batch->groupCount;
    ShaderState* previous=nullptr;
    for(u32 index=0;index<count;++index) {
        DrawGroup* entry=&sead::elementAt(batch->groups, index);
        if(!(u32)entry->quadCount) continue;
        u32 flags=entry->flags;
        ShaderState* shader;
        if(flags&2) shader=&shaders[((u32)parameters->flags&1)?2:1];
        else shader=&shaders[0];
        if(shader!=previous) {
            shader->apply();
            quadVertexUniform(shader->transformUniform,16,transform.get());
            quadPixelUniform(shader->shadowOffset,4,parameters->shadowFrom);
            quadPixelUniform(shader->shadowWidth,4,shadowWidth.get());
            flags=entry->flags;previous=shader;
        }
        if(flags&1) {
            quadPixelUniform(shader->interpolateOffset,4,parameters->interpolateFrom);
            quadPixelUniform(shader->interpolateWidth,4,interpolateWidth.get());
        } else {
            for(u32 i=0;i<4;++i) zero->values[i]=0;
            quadPixelUniform(shader->interpolateOffset,4,zero.get());
            one->values[0]=1;one->values[2]=1;one->values[1]=1;one->values[3]=interpolateWidth->values[3];
            quadPixelUniform(shader->interpolateWidth,4,one.get());
        }
        QuadBatch* current=parameters->batch;
        u32 vertices=(u32)entry->quadCount*0x70;
        u32 active=current->bufferIndex;
        u32 first=(u32)entry->firstVertex*28;
        u32 memory=sead::elementAt(current->vertexBuffers, active);
        quadAttributeBuffer(0,vertices,28,memory+first);
        Texture* texture=entry->texture;
        quadApplyTexture(texture->descriptor,shader->textureUnit);
        u32 quads=entry->quadCount;
        quadDrawIndexed(0x13,quads*4,4,indices,0,1);
        batch=parameters->batch;count=batch->groupCount;
    }
}
/* WWHD glue
ext 0273AD10 void* quadAllocate(u32)
ext C00088C8 void quadZeroRange(u32,u32)
ext C0006A40 void quadVertexUniform(u32,u32,const void*)
ext C0006908 void quadPixelUniform(u32,u32,const void*)
ext C0006798 void quadAttributeBuffer(u32,u32,u32,u32)
ext C00068F8 void quadApplyTexture(void*,u32)
ext C0006178 void quadDrawIndexed(u32,u32,u32,u32,u32,u32)
data sStreamDefaults 1018AB10
data sRendererVtable 1018AC08
ext C00060B8 u32 quadFetchSize(u32,u32,u32)
ext 028E8428 u32 quadVertexCount(const void*)
ext 028E8434 u32 quadPixelCount(const void*)
ext 028E8518 u32 quadVertexHeaderSize(u32,const void*)
ext 028E8538 u32 quadVertexProgramSize(u32,const void*)
ext 028E8528 u32 quadPixelHeaderSize(u32,const void*)
ext 028E8548 u32 quadPixelProgramSize(u32,const void*)
ext 028E8624 u32 quadLoadVertex(nw::ut::VertexShader*,u32,u32,const void*)
ext 028E87A8 u32 quadLoadPixel(nw::ut::PixelShader*,u32,u32,const void*)
ext C00064A8 void quadInvalidate(u32,u32,u32)
ext C0006400 void quadInitializeFetch(nw::ut::FetchShader*,u32,u32,const nw::ut::AttributeStream*,u32,u32)
ext 028F6D84 nw::ut::ShaderState* quadConstructShader(nw::ut::ShaderState*) callback
ext 028EFFD0 void quadConstructArray(void*,u32,u32,u32)
ext C0006438 void quadInitializeSampler(void*,u32,u32)
ext C0006478 void quadSamplerClamping(void*,u32,u32)
ext C0006090 void quadBeginList(u32,u32,u32)
ext C0006190 u32 quadEndList(u32)
ext C0006860 void quadApplyFetch(nw::ut::FetchShader*)
ext C0006A28 void quadApplyVertex(nw::ut::VertexShader*)
ext C00068F0 void quadApplyPixel(nw::ut::PixelShader*)
ext C00068E0 void quadApplySampler(void*,u32)
ext C00060E0 void quadCallList(u32,u32)
str 1018AB68
str 1018AB60
str 1018AB70
str 1018ABB0
str 1018AB90
str 1018ABBC
str 1018AB9C
str 1018ABD0
str 1018AB78
*/
