// Model light/fog uploads, preserving per-call reloads and original local-object placement.
// Consumed layouts follow own instructions and the canonical shared interface.
// The historical tww declaration cross-check supplies no layout acceptance evidence.
#include "JSystem/J3DGraph/ModelLighting.h"
#include "bindings.h"
using namespace j3d::pipeline;
static MatrixRows sPipelineDefaultMatrix;
static void* sPipelineFrameParameters;
static u8 sPipelineLightingContext;
void preparePipelineLighting(LightingPageArray_l*,u32,const MatrixRows*,void*,void*);
void finishPipelineLighting(LightingPageArray_l*,u32);
void scalePipelineColor(sead::ColorChannels*,const sead::ColorChannels*,f32);
namespace j3d::pipeline {
/* 027F53CC */
void ModelPipeline_l::updateLighting(LightingPageArray_l* pages,s32 count,const MatrixRows* input) {
    gabi::AutoFrame frame{0x027F53CCu};
    gabi::FrameLocal<MatrixRows> suppliedMatrix(0x58),defaultMatrix(0x88),matrix(0x28);
    gabi::FrameLocal<sead::ColorChannels> byteColor(8),signedColor(0x18);
    if(input!=nullptr) {
        Conversions::copyMatrix(suppliedMatrix.get(),input);
        seadCopyViewMatrix(suppliedMatrix.get(),matrix.get());
    } else {
        Conversions::copyMatrix(defaultMatrix.get(),&sPipelineDefaultMatrix);
        seadCopyViewMatrix(defaultMatrix.get(),matrix.get());
    }
    ModelDataPipeline_l* data=mModelData;
    ModelMaterial_l* materialBase=data->mMaterials;
    if(count==-1) count=mLightingCount;
    if(count<=0) { __atomic_thread_fence(__ATOMIC_ACQUIRE);return; }
    u32 pageIndex=0;
    do {
        constexpr u32 kLightingParameterOffset=0x240;
        void* parameters=sPipelineFrameParameters;
        void* lightingParameters=gabi::at<void>(gabi::ea(parameters)+kLightingParameterOffset);
        preparePipelineLighting(pages,pageIndex,matrix.get(),&sPipelineLightingContext,lightingParameters);
        u32 pageCount=pages->mCount;
        LightingPage_l* page=pages->mPages;
        if(pageIndex<pageCount) page=&page[pageIndex];
        for(u32 index=0;index!=8;++index) {
            ModelDataPipeline_l* currentData=mModelData;
            ModelMaterial_l* material=currentData->mMaterials;
            PipelineLightProvider_l* provider=material->mLightProvider;
            PipelineLightMethods_l* methods=provider->mMethods;
            PipelineLight_l* light=gabi::call_ptr<PipelineLight_l*>(methods->mLight.get(),provider,index);
            if(light!=nullptr) {
                u32 x=light->mPosition.x.bits;
                u32 z=light->mPosition.z.bits;
                u32 y=light->mPosition.y.bits;
                page->mPositions[index].x.bits=x;
                page->mPositions[index].z.bits=z;
                page->mPositions[index].y.bits=y;
                Conversions::expandByteColor(byteColor.get(),&light->mColor);
                f32 scale=light->mColorScale;
                scalePipelineColor(&page->mLightColors[index],byteColor.get(),scale);
                if(index<2) {
                    Conversions::expandSignedColor(signedColor.get(),&light->mSignedColor);
                    scale=light->mSignedScale;
                    scalePipelineColor(&page->mSignedColors[index],signedColor.get(),scale);
                }
            }
        }
        PipelineFogProvider_l* fogProvider=materialBase->mFogProvider;
        PipelineFogMethods_l* fogMethods=fogProvider->mMethods;
        PipelineFog_l* fog=gabi::call_ptr<PipelineFog_l*>(fogMethods->mFog.get(),fogProvider);
        if(fog!=nullptr && fog->mKind==2) {
            Conversions::expandByteColor(&page->mFogColor,&fog->mColor);
            u32 nearBits=fog->mNear.bits;
            page->mFogNear.bits=nearBits;
            f32 far=fog->mFar.value;
            f32 near=fog->mNear.value;
            f32 alpha=page->mFogColor.a.value;
            if(far==near) page->mFogScale=gabi::fmuls_ppc(1.0f,alpha);
            else {
                f32 distance=gabi::fsubs_ppc(far,near);
                f32 inverse=gabi::fdivs_ppc(1.0f,distance);
                page->mFogScale=gabi::fmuls_ppc(inverse,alpha);
            }
        }
        finishPipelineLighting(pages,pageIndex);
        --count;
        ++pageIndex;
    } while(count!=0);
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}
/* 027F7590 */
void PipelineLightProvider_l::setLight(u32 index,PipelineLight_l* light) { mLights[index]=light; }
/* 027F75A0 */
PipelineLight_l* PipelineLightProvider_l::light(u32 index) const { return mLights[index]; }
/* 027F7634 */
void PipelineFogProvider_l::setFog(PipelineFog_l* fog) { mFog=fog; }
/* 027F763C */
PipelineFog_l* PipelineFogProvider_l::fog() const { return mFog; }
/* 027F7644 */
be<u8>* PipelineFogProvider_l::flagStorage() { return &mFlags[0]; }
/* 027F764C */
be<u8>* PipelineFogProvider_l::otherFlagStorage() { return &mOtherFlag; }
}
/* WWHD glue
data sPipelineDefaultMatrix 104B45F8
data sPipelineFrameParameters 104B4708
data sPipelineLightingContext 104B470C
ext 028E90D4 void seadCopyViewMatrix(const sead::ViewTransform*,sead::ViewTransform*)
ext 0274D458 void scalePipelineColor(sead::ColorChannels*,const sead::ColorChannels*,f32)
ext 027FDA54 void preparePipelineLighting(j3d::pipeline::LightingPageArray_l*,u32,const j3d::pipeline::MatrixRows*,void*,void*)
ext 027FDFF4 void finishPipelineLighting(j3d::pipeline::LightingPageArray_l*,u32)
*/
