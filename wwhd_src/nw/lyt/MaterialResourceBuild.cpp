// format facts: BFLYT material record fields and flags; implementation reconstructed from WWHD.
/* Material resource construction and shader-program selection, reconstructed from WWHD. */
#include "nw/lyt/MaterialResourceBuild.h"
using namespace nw::lyt;
struct MaterialResourcePrograms { be<u8> values[8]; };
struct MaterialResourceScratch {
    be<u32> projectionCount,extra;
    gptr<be<u8>> secondBlend,firstBlend,alpha;
    be<u8> indirectPresent,secondBlendPresent,firstBlendPresent,alphaPresent;
    gptr<MaterialTextureReference> textures;
    be<u8> coordinateCount; u8 padding[3];
    gptr<be<u8>> extraInput;
    be<u32> color;
};
static MaterialResourcePrograms sMaterialResourcePrograms;
static u32 materialResourceCoordinateOffset(u32 flags) { return (flags>>30)*28+((flags>>28)&3)*20; }
static u32 materialResourceBlendOffset(u32 flags) {
    return materialResourceCoordinateOffset(flags)+(((flags>>26)&3)+((flags>>22)&1))*8;
}
static u32 materialResourceStageOffset(u32 flags) {
    return materialResourceBlendOffset(flags)+((flags>>20)&3)*4+((flags>>19)&1)*12;
}
static void materialResourceCopyWords(void* output,const void* input,u32 words) {
    for (u32 i=0;i<words;++i) {
        u32 word=*layoutAtOffset<be<u32>>(input,i*4);
        layoutAtOffset<be<u32>>(output,i*4)->set(word);
    }
}
/* 028815D8 */
MaterialGpu* MaterialGpu::createFromResource(MaterialGpu* material,const MaterialResourceHeader* input,const MaterialResourceSet* resources) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!material) material=static_cast<MaterialGpu*>(materialCopyAllocate(sizeof(MaterialGpu)));
    if (!material) return material;
    material->methods=materialGpuMethodTable();
    materialCopyConstructArray(material,2,4,createPixelUniformLocation);
    material->initialize();
    u32 length=0;
    for (;length<28;++length) {
        u32 character=input->name[length];
        if (!character) break;
        material->name[length]=character;
    }
    layoutAtOffset<be<u8>>(material->name,length)->set(0);
    gabi::FrameLocal<MaterialResourceScratch> scratch(8);
    for (u32 i=0;i<2;++i) {
        u32 word=0;
        for (u32 byte=0;byte<4;++byte) word=(word<<8)|static_cast<u32>(*layoutAtOffset<be<u8>>(input,0x1C+i*4+byte));
        scratch->color=word;
        material->colors[i].value=static_cast<u32>(scratch->color);
    }
    u32 flags=input->flags;
    u32 transformCount=(flags>>2)&3;
    u32 coordinateCount=(flags>>4)&3;
    u32 stageCount=(flags>>6)&7;
    u32 textureCount=flags&3;
    u32 projectionCount=(flags>>15)&3;
    auto* textureInput=layoutAtOffset<MaterialTextureReference>(input,0x28);
    scratch->textures=textureInput;
    u32 offset=0x28+textureCount*4;
    auto* transformInput=layoutAtOffset<MaterialTextureTransform>(input,offset);
    offset+=transformCount*20;
    auto* coordinateInput=layoutAtOffset<MaterialCoordinate>(input,offset);
    offset+=coordinateCount*8;
    auto* stageInput=layoutAtOffset<be<u8>>(input,offset);
    offset+=stageCount*4;
    scratch->alpha=layoutAtOffset<be<u8>>(input,offset);
    u32 alpha=(flags>>9)&1;
    offset+=alpha*8;
    scratch->firstBlend=layoutAtOffset<be<u8>>(input,offset);
    u32 firstBlend=(flags>>10)&1;
    offset+=firstBlend*4;
    scratch->secondBlend=layoutAtOffset<be<u8>>(input,offset);
    u32 secondBlend=(flags>>12)&1;
    offset+=secondBlend*4;
    auto* indirectInput=layoutAtOffset<be<u8>>(input,offset);
    u32 indirect=(flags>>14)&1;
    offset+=indirect*12;
    auto* projectionInput=layoutAtOffset<MaterialProjectionTransform>(input,offset);
    offset+=projectionCount*20;
    scratch->extraInput=layoutAtOffset<be<u8>>(input,offset);
    if (textureCount>3) textureCount=3;
    if (transformCount>3) transformCount=3;
    if (coordinateCount>3) coordinateCount=3;
    if (stageCount>6) stageCount=6;
    if (projectionCount>6) projectionCount=6;
    scratch->alphaPresent=alpha;
    scratch->firstBlendPresent=firstBlend;
    scratch->secondBlendPresent=secondBlend;
    scratch->indirectPresent=indirect;
    scratch->coordinateCount=coordinateCount;
    u32 extra=(flags>>17)&1;
    scratch->projectionCount=projectionCount;
    scratch->extra=extra;
    u32 blends=secondBlend?2:firstBlend;
    material->reserve(textureCount,transformCount,coordinateCount,stageCount,alpha,blends,indirect,projectionCount,extra);
    flags=input->flags;
    u32 ownership=material->ownershipFlags;
    material->ownershipFlags=(ownership&~2u)|(((flags>>11)&1)<<1);
    if (material->storage) {
        u32 i=0;
        for (;;) {
            u32 capacity=static_cast<u32>(material->layoutFlags)>>30;
            if (i>=capacity) break;
            auto* binding=reinterpret_cast<TextureBinding*>(material->storage.get()+i*28);
            if (binding) TextureBinding::create(binding);
            ++i;
        }
        material->featureFlags=(static_cast<u32>(material->featureFlags)&~0xC0000000u)|(textureCount<<30);
        if (textureCount) {
            auto* nameOffsets=resources->textures->offsets;
            auto* textures=reinterpret_cast<TextureBinding*>(material->storage.get());
            for (u32 i=0;i<textureCount;++i) {
                auto* reference=&scratch->textures[i];
                auto* provider=resources->provider.get();
                u32 index=reference->index;
                u32 target=provider->methods->load;
                u32 nameOffset=nameOffsets[index];
                auto* name=layoutAtOffset<be<u8>>(nameOffsets,nameOffset);
                auto* texture=gabi::call_ptr<TextureResourceView*>(target,provider,name);
                textures[i].assignResource(texture);
                u32 horizontal=reference->horizontal;
                u32 vertical=reference->vertical;
                textures[i].setClamping(horizontal&3,vertical&3);
                horizontal=reference->horizontal;
                vertical=reference->vertical;
                textures[i].setFiltering((horizontal>>2)&3,(vertical>>2)&3);
            }
        }
        flags=material->layoutFlags;
        auto* transforms=reinterpret_cast<MaterialTextureTransform*>(material->storage.get()+(flags>>30)*28);
        for (u32 i=0;;++i) {
            u32 capacity=(static_cast<u32>(material->layoutFlags)>>28)&3;
            if (i>=capacity) break;
            if (&transforms[i]) materialResourceZero(&transforms[i],20);
        }
        material->featureFlags=(static_cast<u32>(material->featureFlags)&~0x30000000u)|(transformCount<<28);
        for (u32 i=0;i<transformCount;++i) {
            materialResourceCopyWords(&transforms[i],&transformInput[i],2);
            materialResourceCopyWords(&transforms[i].rotation,&transformInput[i].rotation,1);
            materialResourceCopyWords(&transforms[i].scaleX,&transformInput[i].scaleX,2);
        }
        flags=material->layoutFlags;
        auto* coordinates=reinterpret_cast<MaterialCoordinate*>(material->storage.get()+materialResourceCoordinateOffset(flags));
        for (u32 i=0;;++i) {
            u32 capacity=(static_cast<u32>(material->layoutFlags)>>26)&3;
            if (i>=capacity) break;
            if (&coordinates[i]) { coordinates[i].kind=0; coordinates[i].source=0; }
        }
        material->featureFlags=(static_cast<u32>(material->featureFlags)&~0x0C000000u)|(static_cast<u32>(scratch->coordinateCount)<<26);
        u32 projectionOffset=0;
        for (u32 i=0;i<((static_cast<u32>(material->featureFlags)>>26)&3);++i) {
            materialResourceCopyWords(&coordinates[i],&coordinateInput[i],2);
            flags=material->layoutFlags;
            u32 source=coordinates[i].source;
            auto* projections=material->storage.get()+materialResourceStageOffset(flags)+((flags>>23)&7)*4;
            if (source-3<=2) {
                coordinates[i].projection=projections+projectionOffset;
                projectionOffset+=20;
            } else coordinates[i].projection=nullptr;
        }
        if (stageCount) {
            flags=material->layoutFlags;
            auto* stages=material->storage.get()+materialResourceStageOffset(flags);
            for (u32 i=0;;++i) {
                u32 capacity=(static_cast<u32>(material->layoutFlags)>>23)&7;
                if (i>=capacity) break;
                auto* stage=stages+i*4;
                if (stage) { stage[0]=0; stage[1]=0; }
            }
            material->featureFlags=(static_cast<u32>(material->featureFlags)&~0x03800000u)|(stageCount<<23);
            materialResourceCopyWords(stages,stageInput,stageCount);
        }
        if (scratch->alphaPresent) {
            flags=material->layoutFlags;
            auto* output=material->storage.get()+materialResourceCoordinateOffset(flags)+((flags>>26)&3)*8;
            materialResourceCopyWords(output,scratch->alpha,2);
        }
        if (scratch->firstBlendPresent) {
            flags=material->layoutFlags;
            materialResourceCopyWords(material->storage.get()+materialResourceBlendOffset(flags),scratch->firstBlend,1);
        }
        if (scratch->secondBlendPresent) {
            flags=material->layoutFlags;
            materialResourceCopyWords(material->storage.get()+materialResourceBlendOffset(flags)+4,scratch->secondBlend,1);
        }
        if (scratch->indirectPresent) {
            flags=material->layoutFlags;
            auto* output=material->storage.get()+materialResourceBlendOffset(flags)+((flags>>20)&3)*4;
            materialResourceCopyWords(output,indirectInput,3);
        }
        if (projectionCount) {
            flags=material->layoutFlags;
            auto* projections=material->storage.get()+materialResourceStageOffset(flags)+((flags>>23)&7)*4;
            for (u32 i=0;;++i) {
                u32 capacity=(static_cast<u32>(material->layoutFlags)>>16)&7;
                if (i>=capacity) break;
                auto* projection=projections+i*20;
                if (projection) materialResourceZero(projection,20);
            }
            material->featureFlags=(static_cast<u32>(material->featureFlags)&~0x00070000u)|(projectionCount<<16);
            for (u32 i=0;i<projectionCount;++i)
                materialResourceCopyWords(projections+i*20,&projectionInput[i],5);
        }
        if (extra) {
            flags=material->layoutFlags;
            auto* output=material->storage.get()+materialResourceStageOffset(flags)+((flags>>23)&7)*4+((flags>>16)&7)*20;
            materialResourceCopyWords(output,scratch->extraInput,2);
        }
    }
    u32 features=material->featureFlags;
    stageCount=(features>>23)&7;
    bool hasSpecial=false;
    if (stageCount) {
        flags=material->layoutFlags;
        auto* stages=material->storage.get()+materialResourceStageOffset(flags);
        for (u32 i=0;i<stageCount;++i) {
            u32 kind=stages[i*4];
            if (kind-9<=2) { hasSpecial=true; break; }
        }
    }
    features=material->featureFlags;
    textureCount=features>>30;
    if (textureCount==0) { material->setProgramIndex(1); return material; }
    if (textureCount==1) { material->setProgramIndex(2); return material; }
    if (hasSpecial) {
        if (textureCount==2) material->setProgramIndex(12);
        else if (textureCount==3) {
            material->setProgramIndex(13);
            features=material->featureFlags;
            if (((features>>23)&7)!=0) {
                flags=material->layoutFlags;
                auto* stages=material->storage.get()+materialResourceStageOffset(flags);
                if (stages[4]==11) material->setProgramIndex(14);
            }
        }
        u32 previousKind=0;
        u32 i=0;
        auto* textureStorage=material->storage.get();
        for (;;) {
            features=material->featureFlags;
            if (i>=((features>>23)&7)) break;
            flags=material->layoutFlags;
            auto* stage=material->storage.get()+materialResourceStageOffset(flags)+i*4;
            u32 kind=stage[0];
            u32 next=i+1;
            u32 mode=0;
            if ((kind-9<=2 || previousKind==10) && next<(static_cast<u32>(material->featureFlags)>>30)) {
                auto* texture=reinterpret_cast<TextureBinding*>(textureStorage+next*28);
                u32 format=texture->format;
                if (format==2 || format==3 || format==17) mode=1;
                else {
                    format=texture->format;
                    if (format==6 || format==5 || format==12 || format==21) mode=2;
                }
                if (mode) {
                    auto* surface=texture->surface.get();
                    layoutAtOffset<be<u32>>(surface,0x84)->set(mode==1?0x10405:0x10402);
                    materialResourceTextureRegisters(surface);
                }
            }
            ++i;
            previousKind=stage[0];
        }
        return material;
    }
    if (textureCount>=3) {
        flags=material->layoutFlags;
        material->setProgramIndex(0);
        if ((flags&0x03800000)==0) return material;
        features=material->featureFlags;
        stageCount=(features>>23)&7;
        if (!stageCount) return material;
        flags=material->layoutFlags;
        auto* stages=material->storage.get()+materialResourceStageOffset(flags);
        for (u32 i=0;i<stageCount;++i) {
            u32 kind=stages[i*4];
            if (kind!=0 && kind!=1 && kind!=2 && kind!=4 && kind!=5) {
                material->setProgramIndex(11);
                break;
            }
        }
        return material;
    }
    flags=material->layoutFlags;
    auto* stages=material->storage.get()+materialResourceStageOffset(flags);
    u32 kind=stages[0];
    u32 selector=3;
    if (kind>=1 && kind<=3) selector=sMaterialResourcePrograms.values[kind+3];
    else if (kind>=5 && kind<=8) selector=sMaterialResourcePrograms.values[kind-5];
    material->setProgramIndex(selector);
    return material;
}
/* WWHD glue
data sMaterialResourcePrograms 10181A9C
ext 028753E4 nw::lyt::TextureBinding* nw::lyt::TextureBinding::create(nw::lyt::TextureBinding*)
ext 02875448 void nw::lyt::TextureBinding::assignResource(nw::lyt::TextureResourceView*)
ext 0287547C void nw::lyt::TextureBinding::setClamping(u32,u32)
ext 028754B4 void nw::lyt::TextureBinding::setFiltering(u32,u32)
ext 028F521C void materialResourceZero(void*,u32)
ext C0006498 void materialResourceTextureRegisters(nw::lyt::TextureObject*)
*/
