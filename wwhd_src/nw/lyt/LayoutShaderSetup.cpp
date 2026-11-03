/* Layout shader binding ranges and fetch programs, reconstructed from WWHD. */
#include "nw/lyt/LayoutShaderSetup.h"
#include "nw/lyt/LayoutShaderLoader.h"
using namespace nw::lyt;
// All binding ranges lie in this contiguous code-defined table, including the five-record range.
struct ShaderBindingNames { ShaderBindingName records[268]; };
struct ShaderStreamDefaults { be<u32> words[20]; };
struct ShaderPositionName { be<u8> text[1]; };
static ShaderBindingNames sShaderBindingNames;
static ShaderStreamDefaults sShaderStreamDefaults;
static ShaderPositionName sShaderPositionName;
static bool equalShaderName(const be<u8>* left,const be<u8>* right) {
    u32 a,b;
    do { a=*left++; b=*right++; } while (a==b && a!=0);
    return a==b;
}
/* 0287DBAC */
ShaderBindingRange* ShaderBindingRange::createForProgram(ShaderBindingRange* range,u32 program) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 start=0,count=25;
    switch (program) {
    case 1: start=25; count=9; break;
    case 2: start=34; count=16; break;
    case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: start=50; count=21; break;
    case 12: start=71; count=23; break;
    case 13: start=94; count=27; break;
    case 14: start=121; count=27; break;
    case 15: case 26: start=148; count=21; break;
    case 16: start=0; count=5; break;
    case 17: start=174; count=12; break;
    case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25: start=181; count=17; break;
    case 27: start=198; count=19; break;
    case 28: start=217; count=23; break;
    case 29: start=240; count=23; break;
    }
    if (program!=16 && program!=17) start+=5;
    if (!range) range=static_cast<ShaderBindingRange*>(shaderSetupAllocate(sizeof(ShaderBindingRange)));
    if (range) {
        range->begin=&sShaderBindingNames.records[start];
        range->end=&sShaderBindingNames.records[start+count];
    }
    return range;
}
/* 0287E4C4 */
void LayoutShaderSet::initializePrograms(LayoutRenderStorage* device,const gptr<void>* archives) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ShaderProgramConfig> config(0x24);
    config->displayListCapacity=512;
    config->storageSize=0;
    config->programCount=1;
    for (u32 i=0;i<30;++i) config->enabled[i]=0;
    config->locationCount=27;
    config->enabled[0]=1;
    config->attributeCount=1;
    for (u32 i=0;i<30;++i) config->enabled[i]=1;
    ShaderProgramCache* cache=&device->programCache;
    u32 required=config->calculateStorageSize();
    void* storage=allocateLayoutStorage(required,4);
    cache->initialize(config.get(),storage);
    void* archive=*archives;
    u32 vertexCount=quadVertexCount(archive);
    gabi::FrameLocal<be<u32>[30]> shaderIndices(0x58);
    if (vertexCount==30) {
        for (u32 i=0;i<30;++i) (*shaderIndices)[i]=i;
    } else {
        const u32 indices[30]={0,1,2,3,3,3,3,4,4,4,4,5,6,7,8,9,10,11,12,12,12,12,13,13,13,13,14,15,16,17};
        for (u32 i=0;i<30;++i) (*shaderIndices)[i]=indices[i];
    }
    gabi::FrameLocal<be<u32>> remaining(8);
    *remaining=30;
    gabi::FrameLocal<ShaderBindingRange> bindings(0x1C);
    gabi::FrameLocal<ShaderBindingRange> copiedBindings(0x14);
    gabi::FrameLocal<ShaderBindingRange> cursor(0xC);
    gabi::FrameLocal<ShaderStreamDefaults> defaults(0xD0);
    for (u32 programIndex=0;static_cast<u32>(*remaining)!=0;++programIndex) {
        ShaderProgramSlot* slot=&slots[programIndex];
        u32 archiveIndex=(*shaderIndices)[programIndex];
        LayoutShaderLoader::loadVertex(this,&slot->vertexShader,archiveIndex,archive);
        archiveIndex=(*shaderIndices)[programIndex];
        LayoutShaderLoader::loadPixel(this,&slot->pixelShader,archiveIndex,archive);
        auto* program=cache->configureProgram(programIndex,slot->vertexShader.get(),slot->pixelShader.get());
        ShaderAttribute* stream=cache->attribute(program,0);
        nw::ut::VertexShader* vertex=slot->vertexShader;
        u32 count=vertex->attributeCount;
        nw::ut::ShaderAttribute* found=nullptr;
        nw::ut::ShaderAttribute* attributes=vertex->attributes;
        for (u32 i=0;i<count;++i) {
            if (equalShaderName(attributes[i].name,sShaderPositionName.text)) { found=&attributes[i]; break; }
        }
        u32 position=found?static_cast<u32>(found->location):0xFFFFFFFF;
        for (u32 word=0;word<20;++word) (*defaults).words[word]=static_cast<u32>(sShaderStreamDefaults.words[word]);
        stream->words[2]=0;
        stream->words[1]=0;
        stream->words[0]=position;
        stream->words[3]=0x80D;
        stream->words[4]=0;
        stream->words[6]=(*defaults).words[13];
        stream->words[5]=0;
        stream->words[7]=3;
        ShaderFetch* fetch=cache->fetchShader(program);
        u32 streamCount=cache->config.attributeCount;
        u32 size=shaderSetupFetchSize(streamCount,0,0);
        void* fetchProgram=allocateLayoutStorage(size,256);
        slot->fetchProgram=fetchProgram;
        streamCount=cache->config.attributeCount;
        ShaderAttribute* streams=cache->attributes(program);
        shaderSetupInitializeFetch(fetch,fetchProgram,streamCount,streams,0,0);
        shaderSetupInvalidate(0x40,slot->fetchProgram,size);
        ShaderBindingRange::createForProgram(bindings.get(),programIndex);
        be<s32>* locations=cache->locations(program);
        u32 locationCount=config->locationCount;
        vertex=static_cast<nw::ut::VertexShader*>(cache->vertexShader(program));
        auto* pixel=static_cast<nw::ut::PixelShader*>(cache->pixelShader(program));
        copiedBindings->end=bindings->end.get();
        copiedBindings->begin=bindings->begin.get();
        cursor->begin=copiedBindings->begin.get();
        cursor->end=copiedBindings->end.get();
        while (cursor->begin.get()!=cursor->end.get()) {
            ShaderBindingName* binding=cursor->begin;
            u32 location=binding->slot;
            if (location<locationCount) {
                u32 kind=binding->kind;
                s32 resolved=-1;
                if (kind==0) {
                    u32 count=vertex->blockCount;
                    auto* uniforms=vertex->blocks.get();
                    const be<u8>* name=binding->name;
                    for (u32 i=0;i<count;++i) {
                        if (equalShaderName(uniforms[i].name,name)) {
                            auto* match=&uniforms[i];
                            if (match) resolved=match->location;
                            break;
                        }
                    }
                    locations[location]=resolved;
                } else if (kind==1) {
                    u32 count=pixel->blockCount;
                    auto* uniforms=pixel->blocks.get();
                    const be<u8>* name=binding->name;
                    for (u32 i=0;i<count;++i) {
                        if (equalShaderName(uniforms[i].name,name)) {
                            auto* match=&uniforms[i];
                            if (match) resolved=match->location;
                            break;
                        }
                    }
                    locations[location]=resolved;
                } else if (kind==2) {
                    u32 count=pixel->samplerCount;
                    auto* samplers=pixel->samplers.get();
                    const be<u8>* name=binding->name;
                    for (u32 i=0;i<count;++i) {
                        if (equalShaderName(samplers[i].name,name)) {
                            auto* match=&samplers[i];
                            if (match) resolved=match->location;
                            break;
                        }
                    }
                    locations[location]=resolved;
                }
            }
            cursor->begin=cursor->begin.get()+1;
        }
        cache->recordProgram(program);
        *remaining=static_cast<u32>(*remaining)-1;
    }
}
/* WWHD glue
data sShaderBindingNames 10180924
data sShaderStreamDefaults 1018173C
data sShaderPositionName 1018178C
ext 0273AD10 void* shaderSetupAllocate(u32)
ext C00060B8 u32 shaderSetupFetchSize(u32,u32,u32)
ext C0006400 void shaderSetupInitializeFetch(nw::lyt::ShaderFetch*,void*,u32,nw::lyt::ShaderAttribute*,u32,u32)
ext C00064A8 void shaderSetupInvalidate(u32,void*,u32)
*/
