// Shape packet allocation and priority ordering from the HD model setup path.
#include "JSystem/J3DGraph/J3DModelPacketSetup.h"
#include "JSystem/J3DGraph/J3DAdapterUtilities.h"
using namespace j3d;
PacketProperty_l* findModelDrawProperty(void*,const char*);
void sortModelMaterialNodes(sead::PointerBuffer*,u32);
sead::RegistryHeap* packetSetupCurrentHeap(void*);
/* inlined into ModelAdapter::initializePackets */
static void appendModelMaterial(sead::PointerBuffer* buffer,ModelMaterialLink_l* node) {
    s32 count=buffer->mCount;
    s32 capacity=buffer->mCapacity;
    if(count>=capacity) return;
    gptr<ModelMaterialLink_l>* storage=gabi::at<gptr<ModelMaterialLink_l>>(gabi::ea(static_cast<void*>(buffer->mStorage)));
    storage[count]=node;
    buffer->mCount=s32(u32(buffer->mCount)+1);
}
/* inlined into ModelAdapter::initializePackets */
static ModelMaterialLink_l* modelMaterialAt(const sead::PointerBuffer* buffer,u32 index) {
    u32 count=buffer->mCount;
    if(index>=count) return NULL;
    const gptr<ModelMaterialLink_l>* storage=gabi::at<gptr<ModelMaterialLink_l>>(gabi::ea(static_cast<void*>(buffer->mStorage)));
    return storage[index];
}
/* 027F4098 */
u32 ModelAdapter::initializePackets(J3DModel* model) {
    gabi::AutoFrame frame{gabi::current_function()};
    ModelState_l* state=reinterpret_cast<ModelState_l*>(model);
    u32 shapeCount=state->mMaterialState.mShapeCount;
    if(shapeCount==0) return 0;
    sead::RegistryHeap* heap=packetSetupCurrentHeap(sRegistryManager);
    ShapePacket* packets=gabi::call_ptr<ShapePacket*>(heap->dispatch()->mAllocate.mFunction,heap,shapeCount*u32(sizeof(ShapePacket)),4);
    u32 constructions=shapeCount>0?shapeCount:1;
    u32 packetOffset=0;
    for(u32 left=constructions;left!=0;--left) {
        ShapePacket* packet=gabi::at<ShapePacket>(gabi::ea(packets)+packetOffset);
        if(packet!=NULL) ShapePacket::create(packet);
        packetOffset+=sizeof(ShapePacket);
    }
    if(packets!=NULL) { state->mPackets=packets; state->mPacketCount=shapeCount; }
    heap=packetSetupCurrentHeap(sRegistryManager);
    void* mapping=gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction,heap,shapeCount*2,4);
    if(mapping!=NULL) { state->mAuxiliary=mapping; state->mAuxiliaryCount=shapeCount; }
    gabi::FrameLocal<sead::PointerBuffer> primary(8),remaining(0x214);
    gabi::FrameLocal<ModelMaterialPointers_l> primaryStorage(0x14),remainingStorage(0x220);
    primary->mCount=0; primary->mCapacity=0; primary->mStorage=NULL;
    primary->setStorage(128,primaryStorage.get());
    remaining->mCount=0; remaining->mCapacity=0; remaining->mStorage=NULL;
    remaining->setStorage(128,remainingStorage.get());
    s32 primaryTotal=0;
    s32 remainingTotal=0;
    J3DModelData* data=state->mModelData;
    JointTree_l* tree=static_cast<JointTree_l*>(reinterpret_cast<ResourceHandle*>(data)->optionalData());
    s32 jointIndex=0;
    while(jointIndex<s32(u16(tree->mCount))) {
        u32 count=data->mJointNum;
        u32 ordinal=u16(jointIndex);
        JointNode_l* joints=gabi::at<JointNode_l>(data->mJointNodes);
        JointNode_l* joint=ordinal<count?joints+ordinal:joints;
        ModelMaterialLink_l* material=joint->mMaterials;
        if(material!=NULL) {
            do {
                MaterialParameterHeader* resource=material->mResource;
                u32 offset=resource->mParametersOffset;
                u32 propertiesAddress=offset==0?0:gabi::ea(&resource->mParametersOffset)+offset;
                void* properties=gabi::at<void>(propertiesAddress);
                PacketProperty_l* property=findModelDrawProperty(properties,"draw_buffer_type");
                if(property!=NULL && property->mValue!=4) {
                    appendModelMaterial(primary.get(),material);
                    primaryTotal=s32(u32(primaryTotal)+1);
                } else {
                    appendModelMaterial(remaining.get(),material);
                    remainingTotal=s32(u32(remainingTotal)+1);
                }
                material=material->mNext;
            } while(material!=NULL);
            data=state->mModelData;
            tree=static_cast<JointTree_l*>(reinterpret_cast<ResourceHandle*>(data)->optionalData());
        }
        ++jointIndex;
    }
    sortModelMaterialNodes(primary.get(),gabi::guest_fn((const void*)&MaterialResourceReference::comparePriority));
    u32 index=0;
    for(s32 left=primaryTotal;left>0;--left) {
        ModelMaterialLink_l* material=modelMaterialAt(primary.get(),index);
        ShapeResource_l* shape=material->mShape;
        u32 materialId=shape->mHeader->mMaterialIndex;
        u32 mappingCount=state->mAuxiliaryCount;
        be<u16>* indices=gabi::at<be<u16>>(gabi::ea(static_cast<void*>(state->mAuxiliary)));
        if(materialId<mappingCount) indices+=materialId;
        *indices=index;
        u32 packetCount=state->mPacketCount;
        ShapePacket* packet=state->mPackets;
        if(index<packetCount) packet=gabi::at<ShapePacket>(gabi::ea(packet)+index*sizeof(ShapePacket));
        J3DEntryMaterialLookup* lookup=state->mMaterialState.mLookup;
        ShapeResource_l* source=gabi::at<ShapeResource_l>(gabi::ea(lookup)+materialId*sizeof(J3DEntryMaterialLookup));
        packet->bindResources(source,model);
        ++index;
    }
    index=0;
    for(s32 left=remainingTotal;left>0;--left) {
        u32 destination=u32(primaryTotal)+index;
        ModelMaterialLink_l* material=modelMaterialAt(remaining.get(),index);
        ShapeResource_l* shape=material->mShape;
        u32 materialId=shape->mHeader->mMaterialIndex;
        u32 mappingCount=state->mAuxiliaryCount;
        be<u16>* indices=gabi::at<be<u16>>(gabi::ea(static_cast<void*>(state->mAuxiliary)));
        if(materialId<mappingCount) indices+=materialId;
        *indices=destination;
        u32 packetCount=state->mPacketCount;
        ShapePacket* packet=state->mPackets;
        if(destination<packetCount) packet=gabi::at<ShapePacket>(gabi::ea(packet)+destination*sizeof(ShapePacket));
        J3DEntryMaterialLookup* lookup=state->mMaterialState.mLookup;
        ShapeResource_l* source=gabi::at<ShapeResource_l>(gabi::ea(lookup)+materialId*sizeof(J3DEntryMaterialLookup));
        packet->bindResources(source,model);
        ++index;
    }
    return 0;
}
/* WWHD glue
 ext 02756140 sead::RegistryHeap* packetSetupCurrentHeap(void*)
 ext 0273B560 void sead::PointerBuffer::setStorage(s32,void*)
 ext 0273B7B4 void sortModelMaterialNodes(sead::PointerBuffer*,u32)
 ext 027DFA24 j3d::PacketProperty_l* findModelDrawProperty(void*,const char*)
 str 1016E7F4 "draw_buffer_type" in 027F4098
*/
