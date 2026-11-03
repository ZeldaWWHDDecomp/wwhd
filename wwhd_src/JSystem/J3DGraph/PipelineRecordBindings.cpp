// HD pipeline record setters; record extents come from the original array strides.
#include "JSystem/J3DGraph/PipelineRecordBindings.h"
#include "JSystem/J3DGraph/ModelDataPipeline.h"
#include "bindings.h"
using namespace j3d::pipeline;
namespace j3d::pipeline {
void RecordInvoke_l::invoke(RecordWord_l* record) const {
    gabi::call_ptr<void>(get(),record);
}
/* 027F58E0 */
void PipelineRecordBindings::setRecordWords(u32 word) {
    RecordWord_l* record=mRecords;
    u32 initialCount=mRecordCount;
    RecordWord_l* end=initialCount==0?record:record+initialCount;
    while(record!=end) {
        record->mWord=word;
        u32 count=mRecordCount;
        RecordWord_l* records=mRecords;
        end=count==0?records:records+count;
        record=record+1;
    }
}
/* 027F591C */
void PipelineRecordBindings::setNodeRecordWord(const RecordNode_l* node,u32 word) {
    if(node==nullptr) return;
    RecordKeyLink_l* link=node->mLink;
    RecordKey_l* key=link->mKey;
    u16 index=key->mIndex;
    u32 indexCount=mIndexCount;
    be<u16>* indices=mIndices;
    if(index<indexCount) indices=&indices[index];
    u16 recordIndex=*indices;
    u32 recordCount=mRecordCount;
    RecordWord_l* record=mRecords;
    if(recordIndex<recordCount) record=&record[recordIndex];
    record->mWord=word;
}
/* 027F596C */
void PipelineRecordBindings::setPacketWords(u32 word) {
    s32 count=mPacketCount;
    u32 offset=0;
    for(s32 index=0;index<count;++index) {
        u32 capacity=mPacketCapacity;
        PacketWord_l* packet=nullptr;
        if(static_cast<u32>(index)<capacity) {
            PacketWord_l* packets=mPackets;
            packet=gabi::at<PacketWord_l>(gabi::ea(packets)+offset);
        }
        // Keep the original NULL fallback address calculation; valid fixtures supply capacity/storage.
        gabi::store<u32>(gabi::ea(packet)+offsetof(PacketWord_l,mWord),word);
        count=mPacketCount;
        offset+=sizeof(PacketWord_l);
    }
}
/* 027F583C */
void PipelineRecordBindings::invokeEntryRecords(const MaterialResourceEntry_l* entry) {
    gabi::AutoFrame frame{0x027F583Cu};
    ModelMaterial_l* material=entry->mFirstMaterial;
    while(material!=nullptr) {
        MaterialShapeBinding_l* binding=material->mShapeBinding;
        u8 enabled=binding->mEnabled;
        RawShapeResource_l* shape=binding->mResource;
        u16 lookupIndex=shape->mLookupIndex;
        if(enabled!=0) {
            u32 indexCount=mIndexCount;
            be<u16>* indices=mIndices;
            if(lookupIndex<indexCount) indices=&indices[lookupIndex];
            u16 recordIndex=*indices;
            u32 recordCount=mRecordCount;
            RecordWord_l* record=mRecords;
            if(recordIndex<recordCount) record=&record[recordIndex];
            RecordCallbackTable_l* methods=record->mMethods;
            // Lead22:40 identifies the installed slot as void(Record*), r3 only.
            methods->mInvoke.invoke(record);
        }
        // The callback may have changed the current material's next link.
        material=material->mNext;
    }
}
}
/* WWHD glue
*/
