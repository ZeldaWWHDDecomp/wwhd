#pragma once
#include "wwhd.h"
#include "JSystem/JKernel/JKRArchive.h"
#include "JSystem/JSupport/JSUList.h"
namespace archive_lookup_support {
struct ArchiveDispatch_l {
    u8 mPrefix[0x2C]; be<u32> mFindTypedResource;
    u8 mPadding[12]; be<u32> mReadTypedResource;
};
struct MountedArchive_l : JKRArchive { be<u32> mArchiveKind; };
struct StaticRegistrationState { be<u32> mWords[4]; };
WWHD_OFFSET(ArchiveDispatch_l,mFindTypedResource,0x2C);
WWHD_OFFSET(ArchiveDispatch_l,mReadTypedResource,0x3C); WWHD_SIZE(ArchiveDispatch_l,0x40);
WWHD_OFFSET(MountedArchive_l,mArchiveKind,0x24); WWHD_SIZE(MountedArchive_l,0x28);
WWHD_SIZE(StaticRegistrationState,16);
}
void* findArchiveResource(u32,const be<u8>*,archive_lookup_support::MountedArchive_l*);
u32 readArchiveResource(void*,u32,u32,const be<u8>*,archive_lookup_support::MountedArchive_l*);
void archiveLookupRegisterStatic(void*);
