// HD model construction, lifecycle, callback traversal and root-matrix preparation.
#include "JSystem/J3DGraph/J3DModelAdapter.h"
#include "JSystem/J3DGraph/J3DAdapterUtilities.h"
using namespace j3d;
void* allocateModelObject(u32);
void deallocateModelObject(void*);
void constructModelDisposer(sead::Disposer*);
void finalizeModelDisposer(sead::Disposer*,u32);
void constructModelMaterialState(void*);
void destroyModelMaterialState(void*,s32);
void constructModelAnimation(void*);
void destroyModelAnimation(void*,s32);
sead::RegistryHeap* modelArrayContainingHeap(sead::RegistryManagerState*,const void*);
// Address-only callback reference; its canonical declaration is still pending.
void modelNameAccessor(J3DModel*);
static sead::DisposerVtable sModelMethods;
static sead::SafeTextVtable sModelTextMethods;
static sead::VtableSlot sModelNameMethods;
static sead::VtableSlot sModelNameCallbackMethods;
static sead::VtableSlot sModelCalculationMethods;
static ModelCurrent sCurrentModel;
static ModelScalar sModelZero;
static ModelScalar sModelOne;
static ModelPair sModelProjectionPair;
typedef be<u8> ModelTextByte;
static ModelTextByte sModelEmptyText;
/* 027F3B28 */
void ModelAdapter::advanceJoint(void*,JointCallbackIterator_l* iterator) {
    gabi::AutoFrame frame{gabi::current_function()};
    JointCallbackHeader_l* header=iterator->mHeader;
    J3DModel* current=sCurrentModel.mModel;
    u32 identifier=header->mIdentifier;
    ModelState_l* model=reinterpret_cast<ModelState_l*>(current);
    J3DModelData* data=model->mModelData;
    u32 count=data->mJointNum;
    u32 ordinal=u16(identifier);
    JointNode_l* nodes=gabi::at<JointNode_l>(data->mJointNodes);
    JointNode_l* node=ordinal<count?nodes+ordinal:nodes;
    if(node->mCallback!=0) {
        u32 callback=node->mCallback;
        gabi::call_ptr<void>(callback,node,0);
        current=sCurrentModel.mModel;
        model=reinterpret_cast<ModelState_l*>(current);
    }
    u32 jointCount=model->mMaterialState.mMatrices->mBoneCount;
    s32 next=s32(identifier+1);
    iterator->mNext=next<s32(jointCount)?next:-1;
}
/* 027F3D78 */
void ModelAdapter::reset(J3DModel* model) {
    ModelState_l* state=reinterpret_cast<ModelState_l*>(model);
    state->mCalculateCallback=0;
    state->mRenderMode=2;
    f32 zero=sModelZero.mValue;
    f32 one=sModelOne.mValue;
    state->mBaseMatrix[9]=zero;
    state->mScale[0]=one;
    state->mBaseMatrix[10]=one;
    state->mBaseMatrix[5]=one;
    state->mBaseMatrix[0]=one;
    state->mScale[2]=one;
    state->mScale[1]=one;
    state->mBaseMatrix[3]=zero;
    state->mBaseMatrix[7]=zero;
    state->mChannels=7;
    state->mBaseMatrix[8]=zero;
    state->mDrawType=0;
    state->mRenderEnabled=0;
    state->mBaseMatrix[11]=zero;
    state->mVisible=0;
    state->mFlags=0;
    state->mBaseMatrix[2]=zero;
    state->mBaseMatrix[1]=zero;
    state->mMaterialState.mStorage=NULL;
    state->mMaterialState.mAuxiliaryBytes=0;
    state->mMaterialState.mAuxiliary=NULL;
    state->mBaseMatrix[4]=zero;
    state->mBaseMatrix[6]=zero;
    state->mModelData=NULL;
    state->mUserArea=0;
}
/* 027F3E04 */
J3DModel* ModelAdapter::create(J3DModel* model) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(model==NULL) model=static_cast<J3DModel*>(allocateModelObject(sizeof(J3DModel)));
    if(model==NULL) return model;
    ModelState_l* state=reinterpret_cast<ModelState_l*>(model);
    constructModelDisposer(&state->mDisposer);
    state->mDisposer.mVtable=&sModelMethods;
    constructModelMaterialState(&state->mMaterialState);
    constructModelAnimation(state->mAnimation);
    ModelName_l* name=&state->mName;
    if(name==NULL) name=static_cast<ModelName_l*>(allocateModelObject(sizeof(ModelName_l)));
    if(name!=NULL) {
        sead::SafeText* value=&name->mValue;
        if(value==NULL) value=static_cast<sead::SafeText*>(allocateModelObject(sizeof(sead::SafeText)));
        if(value!=NULL) {
            value->mVtable=&sModelTextMethods;
            value->mText=&sModelEmptyText;
        }
        name->mActive=0;
        name->mMethods=&sModelNameMethods;
        gabi::FrameLocal<ModelMemberCall_l> member(8);
        member->mAdjust=0;
        member->mFunction=gabi::guest_fn((const void*)&modelNameAccessor);
        member->mVirtualIndex=-1;
        ModelNameCallback_l* callback=&name->mCallback;
        if(callback!=NULL) {
            callback->mOwner=model;
            u32 first=gabi::load<u32>(gabi::ea(member.get()));
            callback->mCall.mAdjust=s16(first>>16);
            callback->mCall.mVirtualIndex=s16(first);
            u32 function=member->mFunction;
            callback->mMethods=&sModelNameCallbackMethods;
            callback->mCall.mFunction=function;
        }
    }
    if(gabi::ea(state->mWorkMatrix)==0) allocateModelObject(0x30);
    if(gabi::ea(&state->mPacketCount)==0) {
        be<u32>* pair=static_cast<be<u32>*>(allocateModelObject(8));
        if(pair!=NULL) { pair[1]=0; pair[0]=0; }
    } else { state->mPackets=NULL; state->mPacketCount=0; }
    if(gabi::ea(&state->mAuxiliaryCount)==0) {
        be<u32>* pair=static_cast<be<u32>*>(allocateModelObject(8));
        if(pair!=NULL) { pair[1]=0; pair[0]=0; }
    } else { state->mAuxiliary=NULL; state->mAuxiliaryCount=0; }
    if(gabi::ea(&state->mMaterialCount)==0) {
        be<u32>* pair=static_cast<be<u32>*>(allocateModelObject(8));
        if(pair!=NULL) { pair[1]=0; pair[0]=0; }
    } else { state->mMaterials=NULL; state->mMaterialCount=0; }
    state->mPoseCallback.mVtable=&sModelCalculationMethods;
    ModelAdapter::reset(model);
    return model;
}
/* 027F4C58 */
void ModelAdapter::destroy(J3DModel* model,s32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(model==NULL) return;
    ModelState_l* state=reinterpret_cast<ModelState_l*>(model);
    ShapePacket* packets=state->mPackets;
    state->mDisposer.mVtable=&sModelMethods;
    if(packets!=NULL) {
        s32 index=0;
        s32 count=state->mPacketCount;
        u32 offset=0;
        while(index<count) {
            ShapePacket* packet=gabi::at<ShapePacket>(gabi::ea(packets)+offset);
            gabi::call_ptr<void>(packet->mVtable->mDestroy.mFunction,packet,2);
            count=state->mPacketCount;
            ++index;
            offset+=sizeof(ShapePacket);
            packets=state->mPackets;
        }
        sead::RegistryHeap* heap=modelArrayContainingHeap(sRegistryManager,packets);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction,heap,static_cast<ShapePacket*>(state->mPackets));
        state->mPacketCount=0;
        state->mPackets=NULL;
    }
    destroyModelAnimation(state->mAnimation,2);
    destroyModelMaterialState(&state->mMaterialState,2);
    finalizeModelDisposer(&state->mDisposer,0);
    if((flags&1)!=0) deallocateModelObject(model);
}
/* 027F4D5C */
void ModelAdapter::calculate(J3DModel* model) {
    gabi::AutoFrame frame{gabi::current_function()};
    ModelState_l* state=reinterpret_cast<ModelState_l*>(model);
    sCurrentModel.mModel=model;
    J3DModelData* data=state->mModelData;
    bool hasCallback=false;
    s32 index=0;
    JointTree_l* tree=static_cast<JointTree_l*>(reinterpret_cast<ResourceHandle*>(data)->optionalData());
    while(index<s32(u16(tree->mCount))) {
        u32 count=data->mJointNum;
        JointNode_l* nodes=gabi::at<JointNode_l>(data->mJointNodes);
        u32 ordinal=u16(index);
        JointNode_l* node=ordinal<count?nodes+ordinal:nodes;
        JointCalculator_l* calculator=node->mCalculator;
        if(calculator!=NULL) {
            gabi::call_ptr<void>(calculator->mMethods->mCalculate,calculator,model);
            tree=static_cast<JointTree_l*>(reinterpret_cast<ResourceHandle*>(data)->optionalData());
            count=data->mJointNum;
            nodes=gabi::at<JointNode_l>(data->mJointNodes);
        }
        node=ordinal<count?nodes+ordinal:nodes;
        if(node->mCallback!=0) hasCallback=true;
        ++index;
    }
    if(hasCallback) {
        agl::gx2util::PaletteBuffer* matrices=state->mMaterialState.mMatrices;
        if(matrices->mCallback==NULL) {
            matrices->mCallback=&state->mPoseCallback;
            state->mMaterialState.mMatrices->mCallbackBone=0;
        }
    }
    agl::gx2util::PaletteBuffer* matrices=state->mMaterialState.mMatrices;
    f32 sx=state->mScale[0],sy=state->mScale[1],sz=state->mScale[2];
    f32 bottomX=state->mBaseMatrix[8],bottomY=state->mBaseMatrix[9];
    f32 bottomZ=state->mBaseMatrix[10],bottomW=state->mBaseMatrix[11];
    f32 topX=state->mBaseMatrix[0],topY=state->mBaseMatrix[1];
    f32 bottomScaledX=gabi::fmuls_ppc(bottomX,sx),bottomScaledY=gabi::fmuls_ppc(bottomY,sy);
    f32 topScaledX=gabi::fmuls_ppc(topX,sx),topScaledY=gabi::fmuls_ppc(topY,sy);
    f32 constant=sModelProjectionPair.mValues[1];
    gabi::FrameLocal<ModelMatrix> matrix(8);
    matrix->mValues[8]=bottomScaledX; matrix->mValues[9]=bottomScaledY;
    matrix->mValues[0]=topScaledX; matrix->mValues[1]=topScaledY;
    f32 middleZ=state->mBaseMatrix[6],middleW=state->mBaseMatrix[7];
    f32 middleX=state->mBaseMatrix[4],middleY=state->mBaseMatrix[5];
    f32 topZ=state->mBaseMatrix[2],topW=state->mBaseMatrix[3];
    matrix->mValues[6]=gabi::fmuls_ppc(middleZ,sz); matrix->mValues[7]=gabi::fmuls_ppc(middleW,constant);
    matrix->mValues[4]=gabi::fmuls_ppc(middleX,sx); matrix->mValues[5]=gabi::fmuls_ppc(middleY,sy);
    matrix->mValues[10]=gabi::fmuls_ppc(bottomZ,sz); matrix->mValues[11]=gabi::fmuls_ppc(bottomW,constant);
    matrix->mValues[2]=gabi::fmuls_ppc(topZ,sz); matrix->mValues[3]=gabi::fmuls_ppc(topW,constant);
    matrices->update(matrix.get());
    u32 callback=state->mCalculateCallback;
    if(callback!=0) gabi::call_ptr<void>(callback,model,0);
}
/* WWHD glue
 ext 0273AD10 void* allocateModelObject(u32)
 ext 0273AF40 void deallocateModelObject(void*)
 ext 02752A84 void constructModelDisposer(sead::Disposer*)
 ext 02752BEC void finalizeModelDisposer(sead::Disposer*,u32)
 ext 02772E8C void constructModelMaterialState(void*)
 ext 02772F74 void destroyModelMaterialState(void*,s32)
 ext 027FD6F4 void constructModelAnimation(void*)
 ext 027FD764 void destroyModelAnimation(void*,s32)
 ext 02755FEC sead::RegistryHeap* modelArrayContainingHeap(sead::RegistryManagerState*,const void*)
 ext 027DB1D4 void agl::gx2util::PaletteBuffer::update(void*)
 ext 027F5018 void modelNameAccessor(J3DModel*) callback
 data sModelMethods 1016E8AC
 data sModelTextMethods 1016E5A4
 data sModelNameMethods 1016E66C
 data sModelNameCallbackMethods 1016E694
 data sModelCalculationMethods 1016E8BC
 data sCurrentModel 104B462C
 data sModelZero 1016E57C
 data sModelOne 1016E578
 data sModelProjectionPair 1016E580
 data sModelEmptyText 1016E588
*/
