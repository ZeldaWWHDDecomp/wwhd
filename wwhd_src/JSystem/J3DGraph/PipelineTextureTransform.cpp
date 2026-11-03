// Texture SRT construction, interpolation and mode-specific matrix composition from ownAA70.
#include "JSystem/J3DGraph/PipelineTextureTransform.h"
#include <bit>
using namespace j3d::pipeline;
Pair32 convertPipelineTextureAngle(f64);
void copyPipelineTextureMatrix(const MatrixRows*,MatrixRows*);
void concatenatePipelineTextureMatrices(const MatrixRows*,const MatrixRows*,MatrixRows*);
void translatePipelineTextureMatrix(MatrixRows*,f64,f64,f64);
void scalePipelineTextureMatrix(MatrixRows*,f64,f64,f64);
void rotatePipelineTextureMatrix(MatrixRows*,const sead::ViewVector*);
void* copyPipelineTextureRow(void*,const void*,u32);
struct PipelineTextureSinCos_l { Scalar mSin,mCos,mSinStep,mCosStep; };
struct PipelineTextureSinCosTable_l { PipelineTextureSinCos_l mEntries[256]; };
struct PipelineTextureDouble_l { be<f64> mValue; };
struct PipelineTextureProjection_l { Scalar mRows[4][4]; };
static PipelineTextureSinCosTable_l sPipelineTextureSinCos;
static PipelineTextureDouble_l sPipelineTextureIntegerBias;
static Scalar sPipelineTextureAngleScale;
static Scalar sPipelineTextureFractionScale;
static Scalar sPipelineTextureNegativeHalf;
static Scalar sPipelineTextureZero;
static Scalar sPipelineTextureOne;
static MatrixRows sPipelineTextureInitialEffect;
static MatrixRows sPipelineTextureBasisA;
static MatrixRows sPipelineTextureBasisB;
static PipelineTextureProjection_l sPipelineTextureProjection;
namespace {
f64 textureValue(const Scalar& value) { return ppc_lfs_value(value.bits.get()); }
void textureStore(Scalar& destination,f64 value) { destination.bits=ppc_stfs_bits(value); }
f64 textureAdd(f64 a,f64 b) { return gabi::fadds_fpr(a,b); }
f64 textureSubtract(f64 a,f64 b) { return gabi::fsubs_fpr(a,b); }
f64 textureMultiply(f64 a,f64 b) { return gabi::fmuls_fpr(a,b); }
// All fused-operation operands here are lfs values or already-rounded single results.
f64 textureMadd(f64 a,f64 c,f64 b) { return gabi::fmadds(static_cast<f32>(a),static_cast<f32>(c),static_cast<f32>(b)); }
f64 textureMsub(f64 a,f64 c,f64 b) { return gabi::fmsubs(static_cast<f32>(a),static_cast<f32>(c),static_cast<f32>(b)); }
void snapshotTextureMatrix(MatrixRows* destination,const MatrixRows* source) {
    u32 words[3][4];
    for(u32 row=0;row!=3;++row) for(u32 column=0;column!=4;++column) words[row][column]=source->mRows[row][column].bits;
    for(u32 row=0;row!=3;++row) for(u32 column=0;column!=4;++column) destination->mRows[row][column].bits=words[row][column];
}
struct TextureAffine_l { f64 a,negativeB,c,d,translationS,translationT; };
void writeTextureAffine(MatrixRows* matrix,const TextureAffine_l& values,bool thirdColumn,f64 zero,f64 one) {
    for(u32 row=0;row!=3;++row) for(u32 column=0;column!=4;++column) textureStore(matrix->mRows[row][column],zero);
    textureStore(matrix->mRows[0][0],values.a);
    textureStore(matrix->mRows[0][1],-values.negativeB);
    textureStore(matrix->mRows[1][0],values.c);
    textureStore(matrix->mRows[1][1],values.d);
    textureStore(matrix->mRows[2][2],one);
    u32 translationColumn=thirdColumn?2u:3u;
    textureStore(matrix->mRows[0][translationColumn],values.translationS);
    textureStore(matrix->mRows[1][translationColumn],values.translationT);
}
void writeProjectedTexture(MatrixRows* destination,const MatrixRows* basis,const TextureAffine_l& values) {
    for(u32 column=0;column!=4;++column) {
        f64 second=textureValue(basis->mRows[1][column]);
        f64 first=textureValue(basis->mRows[0][column]);
        f64 product=textureMultiply(values.negativeB,second);
        f64 combined=textureMsub(values.a,first,product);
        f64 third=textureValue(basis->mRows[2][column]);
        textureStore(destination->mRows[0][column],textureMadd(values.translationS,third,combined));
        second=textureValue(basis->mRows[1][column]);
        product=textureMultiply(values.d,second);
        first=textureValue(basis->mRows[0][column]);
        combined=textureMadd(values.c,first,product);
        third=textureValue(basis->mRows[2][column]);
        textureStore(destination->mRows[1][column],textureMadd(values.translationT,third,combined));
    }
    copyPipelineTextureRow(&destination->mRows[2][0],&basis->mRows[2][0],16);
}
}
namespace j3d::pipeline {
/* 027FAA70 */
u32 PipelineTextureTransform_l::build(MatrixRows* destination,const PipelineTextureTransform_l* parameters) {
    gabi::AutoFrame frame{0x027FAA70u};
    gabi::FrameLocal<sead::ViewVector> vector(8);
    gabi::FrameLocal<MatrixRows> work(0x14),effect(0x44),transform(0x80),operation(0xB0);
    gabi::FrameLocal<be<u64>> integerConversion(0x78);
    f64 zero=textureValue(sPipelineTextureZero);
    s32 mode=parameters->mMode;
    f64 one=textureValue(sPipelineTextureOne);
    if(mode==-1) {
        f64 rotation=textureValue(parameters->mRotation);
        f64 angleScale=textureValue(sPipelineTextureAngleScale);
        auto angle=convertPipelineTextureAngle(textureMultiply(rotation,angleScale));
        u32 word=angle.r4;
        u32 fraction=word&0xFFFFFFu;
        integerConversion->set(0x4330000000000000ull|fraction);
        f64 encoded=std::bit_cast<f64>(integerConversion->get());
        f64 widened=gabi::fsub_ppc(encoded,sPipelineTextureIntegerBias.mValue.get());
        f64 fractionValue=static_cast<f32>(widened);
        f64 fractionScale=textureValue(sPipelineTextureFractionScale);
        const PipelineTextureSinCos_l* entry=&sPipelineTextureSinCos.mEntries[word>>24];
        f64 sinBase=textureValue(entry->mSin);
        f64 sinStep=textureValue(entry->mSinStep);
        f64 cosStep=textureValue(entry->mCosStep);
        fractionValue=textureMultiply(fractionValue,fractionScale);
        f64 cosBase=textureValue(entry->mCos);
        f64 scaleT=textureValue(parameters->mScaleT);
        f64 sine=textureMadd(sinStep,fractionValue,sinBase);
        f64 translateT=textureValue(parameters->mTranslateT);
        f64 cosine=textureMadd(cosStep,fractionValue,cosBase);
        f64 scaleS=textureValue(parameters->mScaleS);
        f64 offsetT=textureMultiply(translateT,scaleT);
        f64 a=textureMultiply(scaleS,cosine);
        f64 b=textureMultiply(scaleS,sine);
        f64 sinT=textureMultiply(scaleT,sine);
        f64 d=textureMultiply(scaleT,cosine);
        f64 c=-sinT;
        f64 sumT=textureSubtract(d,sinT);
        f64 translateS=textureValue(parameters->mTranslateS);
        f64 sumS=textureAdd(a,b);
        f64 offsetS=textureMultiply(translateS,scaleS);
        f64 half=textureValue(sPipelineTextureNegativeHalf);
        sumT=textureAdd(sumT,scaleT);
        textureStore(destination->mRows[1][0],c);
        textureStore(destination->mRows[0][1],b);
        sumS=textureSubtract(sumS,scaleS);
        f64 translationT=textureMadd(half,sumT,offsetT);
        textureStore(destination->mRows[0][0],a);
        f64 translationS=textureMsub(half,sumS,offsetS);
        textureStore(destination->mRows[1][1],d);
        translationT=textureAdd(translationT,one);
        textureStore(destination->mRows[0][2],translationS);
        textureStore(destination->mRows[1][2],translationT);
        textureStore(destination->mRows[2][3],zero);
        textureStore(destination->mRows[2][1],zero);
        textureStore(destination->mRows[2][0],zero);
        textureStore(destination->mRows[1][3],zero);
        textureStore(destination->mRows[0][3],zero);
        textureStore(destination->mRows[2][2],one);
    } else {
        PipelineTexturePivot_l* pivot=parameters->mPivot;
        MatrixRows* suppliedEffect=pivot->mEffect;
        snapshotTextureMatrix(effect.get(),&sPipelineTextureInitialEffect);
        if(suppliedEffect!=nullptr) copyPipelineTextureMatrix(suppliedEffect,effect.get());
        f64 pivotS=textureValue(pivot->mS);
        f64 pivotT=textureValue(pivot->mT);
        translatePipelineTextureMatrix(transform.get(),-pivotS,-pivotT,zero);
        vector->x.bits=ppc_stfs_bits(zero);
        vector->y.bits=ppc_stfs_bits(zero);
        vector->z.bits=parameters->mRotation.bits;
        rotatePipelineTextureMatrix(operation.get(),vector.get());
        concatenatePipelineTextureMatrices(operation.get(),transform.get(),transform.get());
        f64 scaleS=textureValue(parameters->mScaleS);
        f64 scaleT=textureValue(parameters->mScaleT);
        vector->z.bits=ppc_stfs_bits(one);
        vector->x.bits=ppc_stfs_bits(scaleS);
        vector->y.bits=ppc_stfs_bits(scaleT);
        scalePipelineTextureMatrix(operation.get(),scaleS,scaleT,one);
        concatenatePipelineTextureMatrices(operation.get(),transform.get(),transform.get());
        pivotS=textureValue(pivot->mS);
        f64 translateS=textureValue(parameters->mTranslateS);
        f64 addedS=textureAdd(pivotS,translateS);
        f64 oldS=textureValue(transform->mRows[0][3]);
        f64 oldT=textureValue(transform->mRows[1][3]);
        TextureAffine_l affine{};
        affine.translationS=textureAdd(oldS,addedS);
        textureStore(transform->mRows[0][3],affine.translationS);
        pivotT=textureValue(pivot->mT);
        f64 translateT=textureValue(parameters->mTranslateT);
        f64 addedT=textureAdd(pivotT,translateT);
        affine.d=textureValue(transform->mRows[1][1]);
        affine.a=textureValue(transform->mRows[0][0]);
        affine.translationT=textureAdd(oldT,addedT);
        f64 b=textureValue(transform->mRows[0][1]);
        textureStore(transform->mRows[1][3],affine.translationT);
        mode=parameters->mMode;
        affine.negativeB=-b;
        affine.c=textureValue(transform->mRows[1][0]);
        if(mode==9 || mode==8 || mode==11) {
            if(mode==9) {
                for(u32 row=0;row!=3;++row) {
                    u32 sourceRow=row==2?3u:row;
                    for(u32 column=0;column!=4;++column) work->mRows[row][column].bits=sPipelineTextureProjection.mRows[sourceRow][column].bits;
                }
                concatenatePipelineTextureMatrices(work.get(),effect.get(),effect.get());
            }
            concatenatePipelineTextureMatrices(&sPipelineTextureBasisA,effect.get(),effect.get());
            writeProjectedTexture(destination,effect.get(),affine);
        } else if(mode==7) {
            writeTextureAffine(work.get(),affine,true,zero,one);
            concatenatePipelineTextureMatrices(work.get(),&sPipelineTextureBasisA,work.get());
            snapshotTextureMatrix(destination,work.get());
        } else if(mode==10) {
            writeTextureAffine(work.get(),affine,false,zero,one);
            concatenatePipelineTextureMatrices(&sPipelineTextureBasisB,effect.get(),effect.get());
            concatenatePipelineTextureMatrices(work.get(),effect.get(),work.get());
            snapshotTextureMatrix(destination,work.get());
        } else if(mode==6) {
            writeTextureAffine(work.get(),affine,false,zero,one);
            concatenatePipelineTextureMatrices(work.get(),&sPipelineTextureBasisB,work.get());
            snapshotTextureMatrix(destination,work.get());
        } else {
            writeTextureAffine(destination,affine,false,zero,one);
        }
    }
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
    return 0x30;
}
}
/* WWHD glue
data sPipelineTextureSinCos 1016C7E0
data sPipelineTextureIntegerBias 1016EDD0
data sPipelineTextureAngleScale 1016EDC8
data sPipelineTextureFractionScale 1016EDD8
data sPipelineTextureNegativeHalf 1016EDDC
data sPipelineTextureZero 1016EBE8
data sPipelineTextureOne 1016EBF0
data sPipelineTextureInitialEffect 104A041C
data sPipelineTextureBasisA 104B4A04
data sPipelineTextureBasisB 104B4A34
data sPipelineTextureProjection 104B470C
ext 028F5680 Pair32 convertPipelineTextureAngle(f64)
ext 028E90D4 void copyPipelineTextureMatrix(const j3d::pipeline::MatrixRows*,j3d::pipeline::MatrixRows*)
ext 028E9108 void concatenatePipelineTextureMatrices(const j3d::pipeline::MatrixRows*,const j3d::pipeline::MatrixRows*,j3d::pipeline::MatrixRows*)
ext 028E93CC void translatePipelineTextureMatrix(j3d::pipeline::MatrixRows*,f64,f64,f64)
ext 028E945C void scalePipelineTextureMatrix(j3d::pipeline::MatrixRows*,f64,f64,f64)
ext 02782C20 void rotatePipelineTextureMatrix(j3d::pipeline::MatrixRows*,const sead::ViewVector*)
ext C000A848 void* copyPipelineTextureRow(void*,const void*,u32)
*/
