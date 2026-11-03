// HD shape-packet naming, program binding and draw-buffer dispatch.
#include "JSystem/J3DGraph/J3DShapePacketResources.h"
using namespace j3d;
s32 formatShapePacketName(sead::SafeTextBuffer*,const char*,const char*,const char*,u32,u32);
PacketProperty_l* findShapePacketProperty(void*,const char*);
void selectShapePacketRange(PacketPrimitive_l*,u32,u32,u32);
// Borrowed current draw-buffer pair, set by the draw callers.
static PacketDrawBuffers_l sPacketDrawBuffers;
// Owned image-initialized table: eight direct member-call descriptors, never written.
static PacketEntryMethods_l sPacketEntryMethods;
/* 027F15A0 */
void ShapePacket::bindResources(ShapeResource_l* shape,J3DModel* model) {
    gabi::AutoFrame frame{gabi::current_function()};
    mResource=shape;
    mModel=model;
    ShapeResourceHeader_l* header=shape->mHeader;
    u32 bindingIndex=header->mBindingIndex;
    PacketModelResources_l* resources=reinterpret_cast<PacketModelResources_l*>(model);
    PacketModelRoot_l* root=resources->mRoot;
    PacketMaterialBinding_l* bindings=resources->mBindings;
    u32 rootNameOffset=root->mNameOffset;
    PacketMaterialHeader_l* material=bindings[bindingIndex].mHeader;
    u32 materialNameOffset=material->mNameOffset;
    const char* rootName=rootNameOffset==0?NULL:gabi::at<const char>(gabi::ea(&root->mNameOffset)+rootNameOffset);
    header=shape->mHeader;
    sead::SafeTextBuffer* name=mName;
    u32 joint=header->mJointIndex;
    const char* materialName=materialNameOffset==0?NULL:gabi::at<const char>(gabi::ea(&material->mNameOffset)+materialNameOffset);
    u32 materialId=material->mIdentifier;
    gabi::cpu->cr[6]=0;
    formatShapePacketName(name,"%s>%s mat_id = %d  joint_id = %d",rootName,materialName,materialId,joint);
    u32 propertiesOffset=material->mParametersOffset;
    void* properties=propertiesOffset==0?NULL:gabi::at<void>(gabi::ea(&material->mParametersOffset)+propertiesOffset);
    PacketProperty_l* property=findShapePacketProperty(properties,"draw_buffer_type");
    if(property!=NULL) {
        mBufferType=u32(property->mValue)==4?1:0;
        return;
    }
    u32 stateOffset=material->mStateOffset;
    PacketLegacyState_l* state=stateOffset==0?NULL:gabi::at<PacketLegacyState_l>(gabi::ea(&material->mStateOffset)+stateOffset);
    if(state==NULL) { mBufferType=0; return; }
    u32 flags=state->mFlags;
    u32 mode=flags&3;
    u32 blend=(flags>>4)&3;
    bool ordinary=(mode==1 || mode==2) && blend==0;
    mBufferType=ordinary?0:1;
}
/* 027F26C4 */
void ShapePacket::bindProgram(PacketSubmission_l* submission) {
    gabi::AutoFrame frame{gabi::current_function()};
    void* program=mProgram;
    if(program==NULL) return;
    gabi::call_ptr<void>(submission->mMethods->mBindProgram,submission,program);
    ShapeResource_l* shape=static_cast<ShapeResource_l*>(static_cast<void*>(mResource));
    ShapeResourceHeader_l* header=shape->mHeader;
    u32 offset=header->mPrimitiveOffset;
    // A zero offset selects guest address 0, which the original reads through: the record stays a guest address and
    // is read with gabi::load, never through a host null pointer the compiler may assume away.
    u32 primitive=offset==0?0:gabi::ea(&header->mPrimitiveOffset)+offset;
    u32 index=gabi::load<u16>(primitive+offsetof(PacketPrimitive_l,mIndex));
    selectShapePacketRange(gabi::at<PacketPrimitive_l>(primitive),0,index,1);
}
/* 027F273C */
void ShapePacket::prepareDrawList() {
    gabi::AutoFrame frame{gabi::current_function()};
    ShapeResource_l* shape=static_cast<ShapeResource_l*>(static_cast<void*>(mResource));
    ModelState_l* model=static_cast<ModelState_l*>(static_cast<void*>(mModel));
    ShapeResourceHeader_l* header=shape->mHeader;
    u32 joint=header->mJointIndex;
    agl::gx2util::PaletteBuffer* matrices=model->mMaterialState.mMatrices;
    u32 flags=matrices->mFlags;
    Mtx34* base=static_cast<Mtx34*>(static_cast<void*>(matrices->mWorld));
    matrices->mFlags=flags|0x10;
    Mtx34* matrix=gabi::at<Mtx34>(gabi::ea(base)+joint*sizeof(Mtx34));
    sPacketDrawBuffers.mBuffers[0]->mMatrix=matrix;
    sPacketDrawBuffers.mBuffers[1]->mMatrix=matrix;
    u32 type=mBufferType;
    PacketDrawBuffer_l* buffer=sPacketDrawBuffers.mBuffers[type<2?type:0];
    u32 mode=buffer->mMode;
    u32 address=gabi::ea(&sPacketEntryMethods)+mode*sizeof(ModelMemberCall_l);
    const auto* method=gabi::at<gabi::ptmf<void (PacketDrawBuffer_l::*)(ShapePacket*)>>(address);
    (buffer->**method)(this);
}
/* WWHD glue
 ext 02759C28 s32 formatShapePacketName(sead::SafeTextBuffer*,const char*,const char*,const char*,u32,u32)
 ext 027DFA24 j3d::PacketProperty_l* findShapePacketProperty(void*,const char*)
 ext 027E27B8 void selectShapePacketRange(j3d::PacketPrimitive_l*,u32,u32,u32)
 str 1016E294 "%s>%s mat_id = %d  joint_id = %d"
 str 1016E2B8 "draw_buffer_type"
 data sPacketDrawBuffers 104B4634
 data sPacketEntryMethods 101F9898
*/
