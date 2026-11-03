/* Model packet entry, following tww J3DModel with Cafe material/packet arrays. */
#include "JSystem/J3DGraph.h"
#include "JSystem/J3DGraph/J3DModel_entry.h"
/* 027F4F1C */
void J3DModel::entry() {
    u32 count=mPacketCount;
    J3DEntryPacket* packet=mPackets;
    J3DEntryPacket* end=packet+count;
    while(packet!=end) {
        u16 lookupIndex=packet->mShape->mHeader->mMaterialLookupIndex;
        J3DEntryMaterialLookup* lookup=mMaterialLookup;
        J3DModelData* data=mModelData;
        u16 materialIndex=lookup[lookupIndex].mHeader->mMaterialIndex;
        J3DMaterial* material=data->getMaterialNodePointer(materialIndex);
        if(material->mShape->mVisible) {
            u8 drawType=mDrawType;
            J3DEntryPacketVtable* table=packet->mVtable;
            packet->mDrawType=drawType;
            gabi::call_ptr<void>(table->mEntry,packet);
            count=mPacketCount;J3DEntryPacket* start=mPackets;end=start+count;
        }
        ++packet;
    }
}
/* 027F4FE4 */
void J3DModel::update() { calc();entry(); }
/* WWHD glue
*/
