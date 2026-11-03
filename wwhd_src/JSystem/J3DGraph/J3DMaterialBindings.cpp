// Resource selection, matrix attachment and insertion into material chains.
#include "JSystem/J3DGraph/J3DMaterialBindings.h"
using namespace j3d;
void setMaterialProgramResource(MaterialProgramState_l*,ShaderResource_l*);
u32 materialProgramKind(ShaderResource_l*,void*);
void prepareMaterialProgram(ShaderResource_l*);
void bindPreparedMaterialProgram(MaterialProgramState_l*,void*);
void bindMaterialProgram(MaterialProgramState_l*,void*);
void activateMaterialProgram(MaterialProgramState_l*);
void bindMaterialMatrixInput(MaterialProgramState_l*,void*);
void initializeMaterialChain(MaterialChain*,void*);
/* 027F363C */
void MaterialBinding::setInput(ShaderInput_l* input) {
    gabi::AutoFrame frame{gabi::current_function()};
    mInput=input;
    if(input==NULL) return;
    setMaterialProgramResource(&mState,input->mResource);
    ShaderResource_l* resource=mState.mResource;
    u32 offset=resource->mCodeOffset;
    void* code=mCode;
    void* expected=offset==0?NULL:gabi::at<void>(gabi::ea(&resource->mCodeOffset)+offset);
    if(gabi::ea(code)==gabi::ea(expected)) {
        bindMaterialProgram(&mState,mCode);
    } else {
        ShaderInput_l* current=mInput;
        if(u16(materialProgramKind(current->mResource,code))==0) bindMaterialProgram(&mState,mCode);
        else {
            current=mInput;
            prepareMaterialProgram(current->mResource);
            bindPreparedMaterialProgram(&mState,mCode);
        }
    }
}
/* 027F3BD0 */
void MaterialBinding::attachModel(J3DModel* model) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(mState.mResource==NULL) return;
    ModelMatrixInput_l* input=reinterpret_cast<ModelMatrixInput_l*>(model);
    input->mMatrices->mBinding=this;
    input->mMatrices->mJoint=0;
    activateMaterialProgram(&mState);
    bindMaterialMatrixInput(&mState,&input->mInput);
}
/* 027F3C38 */
void MaterialChain::attach(void* context,MaterialChainNode* node) {
    gabi::AutoFrame frame{gabi::current_function()};
    initializeMaterialChain(this,context);
    if(node==NULL) return;
    MaterialChainNode* current=mFirst;
    if(current==NULL) {
        mFirst=node;
        mFlags=u8(mFlags)&0xF0;
        return;
    }
    MaterialChainNode* next=current->mNext;
    while(next!=NULL) { current=next; next=current->mNext; }
    current->mNext=node;
    if(mFirst!=NULL) mFlags=u8(mFlags)&0xF0;
}
/* WWHD glue
 ext 027E0368 void setMaterialProgramResource(j3d::MaterialProgramState_l*,j3d::ShaderResource_l*)
 ext 027E2AF4 u32 materialProgramKind(j3d::ShaderResource_l*,void*)
 ext 027E2CFC void prepareMaterialProgram(j3d::ShaderResource_l*)
 ext 027DFF1C void bindPreparedMaterialProgram(j3d::MaterialProgramState_l*,void*)
 ext 027E00AC void bindMaterialProgram(j3d::MaterialProgramState_l*,void*)
 ext 027E0288 void activateMaterialProgram(j3d::MaterialProgramState_l*)
 ext 027E0360 void bindMaterialMatrixInput(j3d::MaterialProgramState_l*,void*)
 ext 027F7870 void initializeMaterialChain(j3d::MaterialChain*,void*)
*/
