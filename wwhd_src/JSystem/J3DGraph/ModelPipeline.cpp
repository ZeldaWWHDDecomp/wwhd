// Model-side HD pipeline updates; private companion fields await canonical header adoption.
#include "JSystem/J3DGraph/ModelPipeline.h"
#include "bindings.h"
using namespace j3d::pipeline;
MaterialCountHeader_l* pipelineMaterialHeader(ModelDataPipeline_l*);
void preparePipelineBuffer(void*,u32);
void updatePipelineBuffer(void*,u32);
void finishPipelineBuffer(void*,u32);
namespace j3d::pipeline {
/* 027F5018 */
void ModelPipeline_l::updatePacketBindings() {
    gabi::AutoFrame frame{0x027F5018u};
    ModelDataPipeline_l* data=mModelData;
    u16 index=0;
    MaterialCountHeader_l* header=pipelineMaterialHeader(data);
    while(index<header->mMaterialCount) {
        u32 count=data->mMaterialCount;
        ModelMaterial_l* material=data->mMaterials;
        if(index<count) material=&material[index];
        if(material->mApplyObject.get()!=nullptr) {
            MaterialApplyObject_l* object=material->mApplyObject;
            MaterialApplyTable_l* methods=object->mMethods;
            u32 target=methods->mApply.get();
            MaterialPacket_l* packets=mMaterialPackets;
            gabi::call_ptr<void>(target,object,&packets[index]);
            data=mModelData;
            header=pipelineMaterialHeader(data);
        }
        index=static_cast<u16>(index+1);
    }
    u32 bufferIndex=mBufferIndex;
    preparePipelineBuffer(mBuffer,bufferIndex);
    bufferIndex=mBufferIndex;
    updatePipelineBuffer(mBuffer,bufferIndex);
    bufferIndex=mBufferIndex;
    finishPipelineBuffer(mBuffer,bufferIndex);
}
/* 027F50F8 */
void ModelPipeline_l::updatePacketValues() {
    s32 count=mPacketCount;
    ModelDataPipeline_l* data=mModelData;
    MaterialValueTable_l* values=data->mValueTable;
    s32 packetIndex=0;
    while(packetIndex<count) {
        MaterialPacket_l* packets=mMaterialPackets;
        MaterialPacket_l* packet=&packets[packetIndex];
        u32 materialCount=data->mMaterialCount;
        u16 materialIndex=static_cast<u16>(packetIndex);
        ModelMaterial_l* material=data->mMaterials;
        if(materialIndex<materialCount) material=&material[materialIndex];
        MaterialPacketHeader_l* packetHeader=packet->mHeader;
        u8 valueCount=packetHeader->mValueCount;
        u32 valueStart=material->mValueStart;
        if(valueCount!=0) {
            for(u32 index=0;index!=valueCount;++index) {
                u16 valueIndex=static_cast<u16>(valueStart+index);
                MaterialValue_l* table=values->mValues;
                be<u32>* output=packet->mValues;
                u32 word=table[valueIndex].mWord;
                output[index]=word;
            }
            count=mPacketCount;
        }
        ++packetIndex;
        if(packetIndex<count) data=mModelData;
    }
}
}
/* WWHD glue
ext 027F3F8C j3d::pipeline::MaterialCountHeader_l* pipelineMaterialHeader(j3d::pipeline::ModelDataPipeline_l*)
ext 027DE824 void preparePipelineBuffer(void*,u32)
ext 027DE778 void updatePipelineBuffer(void*,u32)
ext 027DE780 void finishPipelineBuffer(void*,u32)
*/
