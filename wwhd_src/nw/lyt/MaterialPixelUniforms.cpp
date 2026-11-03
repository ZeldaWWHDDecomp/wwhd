/* Material colors, stage selectors and indirect-coordinate uniforms. */
#include "nw/lyt/MaterialPixelUniforms.h"
using namespace nw::lyt;
struct MaterialColorConversion { be<u32> high,low; };
struct MaterialFloatUniform { be<f32> values[4]; };
struct MaterialIntegerUniform { be<u32> values[4]; };
struct MaterialColorScratch { be<u32> value; };
// Twelve stage selectors precede the adjacent alpha-function table in the binary.
struct MaterialStageKinds { be<u32> values[12]; };
static MaterialStageKinds sMaterialStageKinds;
static u32 materialStageShift(u32 value,u32 shift) { return (shift&32)?0:value<<(shift&31); }
/* 028830FC */
void MaterialGpu::uploadPixelUniforms(LayoutDrawContext* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto* device=reinterpret_cast<LayoutRenderStorage*>(context->device.get());
    gabi::FrameLocal<MaterialColorScratch> color(0x60);
    color->value=static_cast<u32>(colors[1].value);
    u32 colorWord=color->value;
    u32 features=featureFlags;
    u32 red=*layoutAtOffset<be<u8>>(this,4);
    u32 green=(colorWord>>16)&255;
    u32 blue=(colorWord>>8)&255;
    u32 alpha=colorWord&255;
    gabi::FrameLocal<MaterialColorConversion> conversion(8);
    conversion->high=0x43300000;
    conversion->low=red^0x80000000;
    f32 endRed=gabi::fdivs_ppc(static_cast<f32>(red),255.0f);
    conversion->low=green^0x80000000;
    f32 endGreen=gabi::fdivs_ppc(static_cast<f32>(green),255.0f);
    conversion->low=blue^0x80000000;
    f32 endBlue=gabi::fdivs_ppc(static_cast<f32>(blue),255.0f);
    conversion->low=alpha^0x80000000;
    f32 endAlpha=gabi::fdivs_ppc(static_cast<f32>(alpha),255.0f);
    if ((features>>30)==0) {
        gabi::FrameLocal<MaterialFloatUniform> end(0x20);
        end->values[0]=endRed; end->values[1]=endGreen;
        end->values[2]=endBlue; end->values[3]=endAlpha;
        u32 location=context->uniformLocation(20);
        materialPixelUniforms(location,4,end.get());
    } else {
        gabi::FrameLocal<MaterialFloatUniform> start(0x10);
        u32 startRed=*layoutAtOffset<be<u8>>(this,0);
        conversion->low=startRed^0x80000000;
        u32 startGreen=*layoutAtOffset<be<u8>>(this,1);
        conversion->low=startGreen^0x80000000;
        u32 startBlue=*layoutAtOffset<be<u8>>(this,2);
        conversion->low=startBlue^0x80000000;
        u32 startAlpha=*layoutAtOffset<be<u8>>(this,3);
        conversion->low=startAlpha^0x80000000;
        start->values[0]=gabi::fdivs_ppc(static_cast<f32>(startRed),255.0f);
        start->values[1]=gabi::fdivs_ppc(static_cast<f32>(startGreen),255.0f);
        start->values[3]=gabi::fdivs_ppc(static_cast<f32>(startAlpha),255.0f);
        start->values[2]=gabi::fdivs_ppc(static_cast<f32>(startBlue),255.0f);
        u32 location=context->uniformLocation(20);
        f32 greenStart=start->values[1];
        f32 redStart=start->values[0];
        f32 redDelta=gabi::fsubs_ppc(endRed,redStart);
        f32 blueStart=start->values[2];
        f32 greenDelta=gabi::fsubs_ppc(endGreen,greenStart);
        f32 alphaStart=start->values[3];
        f32 blueDelta=gabi::fsubs_ppc(endBlue,blueStart);
        gabi::FrameLocal<MaterialFloatUniform> difference(0x30);
        difference->values[0]=redDelta;
        f32 alphaDelta=gabi::fsubs_ppc(endAlpha,alphaStart);
        difference->values[1]=greenDelta;
        difference->values[2]=blueDelta;
        difference->values[3]=alphaDelta;
        materialPixelUniforms(location,4,difference.get());
        location=context->uniformLocation(21);
        materialPixelUniforms(location,4,start.get());
    }
    features=featureFlags;
    bool hasStages=(features&0x03800000)!=0;
    u32 location;
    if (!hasStages) {
        features=featureFlags;
        context->flagB5=1;
        context->flagB4=features>>30;
        location=context->uniformLocation(19);
        features=featureFlags;
        u32 textureCount=features>>30;
        if (textureCount>=2) {
            gabi::FrameLocal<MaterialIntegerUniform> mode(0xA4);
            mode->values[1]=0; mode->values[3]=0; mode->values[2]=0; mode->values[0]=2;
            materialPixelUniforms(location,4,mode.get());
        } else {
            u32 selectedProgram=programIndex();
            auto* program=device->programCache.program(selectedProgram);
            u32 usesColor=device->programCache.vertexColorEnabled(program);
            if (!usesColor) return;
            if (textureCount==0) {
                gabi::FrameLocal<MaterialIntegerUniform> mode(0x84);
                mode->values[0]=0; mode->values[3]=0; mode->values[1]=0; mode->values[2]=0;
                materialPixelUniforms(location,4,mode.get());
            } else {
                gabi::FrameLocal<MaterialIntegerUniform> mode(0x94);
                mode->values[1]=0; mode->values[3]=0; mode->values[2]=0; mode->values[0]=1;
                materialPixelUniforms(location,4,mode.get());
            }
        }
        return;
    }
    location=context->uniformLocation(19);
    features=featureFlags;
    if ((features>>30)<2) {
        u32 selectedProgram=programIndex();
        auto* program=device->programCache.program(selectedProgram);
        u32 usesColor=device->programCache.vertexColorEnabled(program);
        if (usesColor) {
            features=featureFlags;
            gabi::FrameLocal<MaterialIntegerUniform> mode(0x74);
            mode->values[2]=0; mode->values[3]=0;
            mode->values[0]=features>>30; mode->values[1]=0;
            materialPixelUniforms(location,4,mode.get());
        }
        return;
    }
    features=featureFlags;
    u32 stageCount=(features>>23)&7;
    u32 packed=0;
    if (stageCount) {
        u32 flags=layoutFlags;
        u32 offset=(flags>>30)*28+((flags>>28)&3)*20+(((flags>>26)&3)+((flags>>22)&1))*8+
            ((flags>>20)&3)*4+((flags>>19)&1)*12;
        auto* stages=storage.get()+offset;
        for (u32 i=0;i<stageCount;++i) {
            u32 kind=stages[i*4];
            u32 mode=stages[i*4+1];
            packed|=materialStageShift(sMaterialStageKinds.values[kind],i*8);
            if (mode==1) packed|=materialStageShift(1,i*8+7);
        }
    }
    gabi::FrameLocal<MaterialIntegerUniform> modes(0x64);
    modes->values[1]=0; modes->values[0]=packed;
    modes->values[3]=0; modes->values[2]=0;
    materialPixelUniforms(location,4,modes.get());
    u32 flags=layoutFlags;
    if ((flags&0x00080000)==0) return;
    flags=layoutFlags;
    u32 offset=(flags>>30)*28+((flags>>28)&3)*20+(((flags>>26)&3)+((flags>>22)&1))*8+((flags>>20)&3)*4;
    auto* indirect=reinterpret_cast<MaterialIndirect*>(storage.get()+offset);
    f32 rotation=indirect->rotation;
    gabi::FrameLocal<LayoutTextureMatrix> matrix(0xB4);
    matrix->configureIndirect(reinterpret_cast<const PaneSize*>(&indirect->scaleX),rotation);
    u32 firstLocation=context->uniformLocation(25);
    u32 secondLocation=context->uniformLocation(26);
    f32 horizontalScale=matrix->horizontalScale;
    f32 horizontalCross=matrix->horizontalCross;
    f32 horizontalOffset=matrix->horizontalOffset;
    gabi::FrameLocal<MaterialFloatUniform> first(0x40);
    first->values[2]=0; first->values[3]=horizontalOffset;
    first->values[1]=horizontalCross; first->values[0]=horizontalScale;
    materialPixelUniforms(firstLocation,4,first.get());
    f32 verticalCross=matrix->verticalCross;
    f32 verticalOffset=matrix->verticalOffset;
    gabi::FrameLocal<MaterialFloatUniform> second(0x50);
    second->values[0]=verticalCross; second->values[3]=verticalOffset;
    f32 verticalScale=matrix->verticalScale;
    second->values[2]=0; second->values[1]=verticalScale;
    materialPixelUniforms(secondLocation,4,second.get());
}
/* WWHD glue
data sMaterialStageKinds 10181AD0
ext C0006908 void materialPixelUniforms(u32,u32,const void*)
*/
