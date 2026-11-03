// Named priority comparison and lazy initialization of a material descriptor.
#include "JSystem/J3DGraph/J3DMaterialResources.h"
#include "JSystem/J3DGraph/J3DAdapterUtilities.h"
using namespace j3d;
MaterialPriority* findMaterialPriority(void*,const char*);
void* allocateMaterialTrackArray(u32);
static MaterialDefaultsGuard sMaterialDefaultsGuard;
static MaterialDefaults sMaterialDefaults;
static MaterialDefaults sInitialMaterialDefaults;
/* 027F34AC */
void ResourceView::initializeTracks(const ResourceHandle* archive) {
    gabi::AutoFrame frame{gabi::current_function()};
    void* archiveHeader=archive->get();
    ResourceHeader* initialHeader=mHeader;
    mArchive=archiveHeader;
    u32 count=initialHeader->mTrackCount;
    MaterialTrackBinding* output=static_cast<MaterialTrackBinding*>(allocateMaterialTrackArray(count*sizeof(MaterialTrackBinding)));
    mTracks=output;
    for(u32 index=0;index<count;++index) {
        ResourceHeader* header=mHeader;
        u32 tracksOffset=header->mTracksOffset;
        // A zero offset selects guest address 0, which the original reads through: the records stay guest addresses
        // and are read with gabi::load, never through a host null pointer the compiler may assume away.
        u32 tracks=tracksOffset==0?0:gabi::ea(&header->mTracksOffset)+tracksOffset;
        u32 track=tracks+index*sizeof(MaterialTrackRecord);
        u32 linkField=track+offsetof(MaterialTrackRecord,mNameLinkOffset);
        u32 linkOffset=gabi::load<u32>(linkField);
        u32 link=linkOffset==0?0:linkField+linkOffset;
        u32 nameField=link+offsetof(MaterialTrackNameLink,mNameOffset);
        u32 nameOffset=gabi::load<u32>(nameField);
        u32 name=nameOffset==0?0:nameField+nameOffset-4;
        u32 channel=gabi::load<u8>(name+offsetof(MaterialTrackName,mChannel));
        output[index].mChannel=channel-'0';
        if(index+1<count) output=mTracks;
    }
}
/* inlined into comparePriority */
static void* materialParameterList(const MaterialParameterHeader* resource) {
    u32 offset=resource->mParametersOffset;
    return offset==0?NULL:gabi::at<void>(gabi::ea(&resource->mParametersOffset)+offset);
}
/* 027F3FB4 */
s32 MaterialResourceReference::comparePriority(const MaterialResourceReference* first,const MaterialResourceReference* second) {
    gabi::AutoFrame frame{gabi::current_function()};
    MaterialPriority* left=findMaterialPriority(materialParameterList(first->mResource),"sort_priority");
    MaterialPriority* right=findMaterialPriority(materialParameterList(second->mResource),"sort_priority");
    if(left==NULL || right==NULL) return 0;
    s32 leftValue=left->mValue;
    s32 rightValue=right->mValue;
    if(leftValue<rightValue) return 1;
    if(leftValue>rightValue) return -1;
    return 0;
}
/* 027F3CD0 */
MaterialDefaults* MaterialDefaults::get() {
    if(sMaterialDefaultsGuard.mReady!=0) return &sMaterialDefaults;
    u32 first=sInitialMaterialDefaults.mWords[0];
    u32 seventh=sInitialMaterialDefaults.mWords[6];
    u32 second=sInitialMaterialDefaults.mWords[1];
    sMaterialDefaults.mWords[6]=seventh;
    u32 fourth=sInitialMaterialDefaults.mWords[3];
    sMaterialDefaults.mWords[1]=second;
    sMaterialDefaults.mWords[3]=fourth;
    u32 sixth=sInitialMaterialDefaults.mWords[5];
    u32 eighth=sInitialMaterialDefaults.mWords[7];
    sMaterialDefaults.mWords[5]=sixth;
    sMaterialDefaults.mWords[7]=eighth;
    u32 third=sInitialMaterialDefaults.mWords[2];
    sMaterialDefaultsGuard.mReady=1;
    sMaterialDefaults.mWords[0]=first;
    u32 fifth=sInitialMaterialDefaults.mWords[4];
    sMaterialDefaults.mWords[2]=third;
    sMaterialDefaults.mWords[4]=fifth;
    return &sMaterialDefaults;
}
/* WWHD glue
 ext 027DFA24 j3d::MaterialPriority* findMaterialPriority(void*,const char*)
 ext 0273ADAC void* allocateMaterialTrackArray(u32)
 str 1016E7E4 "sort_priority"
 data sMaterialDefaultsGuard 104B48D8
 data sMaterialDefaults 104B48DC
 data sInitialMaterialDefaults 1016E464
*/
