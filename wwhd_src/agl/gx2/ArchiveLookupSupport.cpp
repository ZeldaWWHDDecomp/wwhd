/* The assigned archive lookup tail belongs to JKernel-adjacent support. */
#include "agl/gx2/ArchiveLookupSupport.h"
#include "JSystem/JKernel/JKRFileLoader.h"
using namespace archive_lookup_support;
// Canonical mounted-volume storage is owned by JKRFileLoaderStatics.cpp.
static constexpr u32 RarcArchive = 0x52415243u;
static StaticRegistrationState sArchiveLookupStatic;
static u8 sArchiveLookupRegistration;
static ArchiveDispatch_l* archiveDispatch(MountedArchive_l* archive) {
    return static_cast<ArchiveDispatch_l*>(static_cast<void*>(archive->mVtable.get()));
}
/* 027EA948 */
void* findArchiveResource(u32 type,const be<u8>* name,MountedArchive_l* archive) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (archive) {
        u32 method = archiveDispatch(archive)->mFindTypedResource;
        return gabi::call_ptr<void*>(method,archive,type,name);
    }
    JSUPtrLink* link = sVolumeList.mHead;
    void* resource = nullptr;
    while (link) {
        MountedArchive_l* candidate = static_cast<MountedArchive_l*>(static_cast<void*>(link->mObject.get()));
        if (candidate->mArchiveKind == RarcArchive) {
            archive = static_cast<MountedArchive_l*>(static_cast<void*>(link->mObject.get()));
            u32 method = archiveDispatch(archive)->mFindTypedResource;
            resource = gabi::call_ptr<void*>(method,archive,type,name);
            if (resource) break;
        }
        link = link->mNext;
    }
    return resource;
}
/* 027EAA2C */
u32 readArchiveResource(void* destination,u32 capacity,u32 type,const be<u8>* name,MountedArchive_l* archive) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 result = 0;
    if (archive) {
        u32 method = archiveDispatch(archive)->mReadTypedResource;
        return gabi::call_ptr<u32>(method,archive,destination,capacity,type,name);
    }
    JSUPtrLink* link = sVolumeList.mHead;
    while (link) {
        MountedArchive_l* candidate = static_cast<MountedArchive_l*>(static_cast<void*>(link->mObject.get()));
        if (candidate->mArchiveKind == RarcArchive) {
            archive = static_cast<MountedArchive_l*>(static_cast<void*>(link->mObject.get()));
            u32 method = archiveDispatch(archive)->mReadTypedResource;
            result = gabi::call_ptr<u32>(method,archive,destination,capacity,type,name);
            if (result) break;
        }
        link = link->mNext;
    }
    return result;
}
/* 027EAAFC */
static void __sinit_ArchiveLookupSupport_cpp() {
    gabi::AutoFrame frame{gabi::current_function()};
    sArchiveLookupStatic.mWords[2] = 0;
    sArchiveLookupStatic.mWords[0] = 0;
    sArchiveLookupStatic.mWords[3] = 0;
    sArchiveLookupStatic.mWords[1] = 0;
    archiveLookupRegisterStatic(&sArchiveLookupRegistration);
}
/* WWHD glue
data sArchiveLookupStatic 104A2344
data sArchiveLookupRegistration 101F9700
ext 028F026C void archiveLookupRegisterStatic(void*)
*/
