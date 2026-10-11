/** HD sound archive and heap views; private decomp source. */
#pragma once
#include "wwhd.h"

struct hdSoundRuntimeTypeInfo {
    be<u32> mTypeNameAddress;
};
struct hdSoundArchiveName {
    gptr<const char> mText;
    be<u32> mVtable;
};
WWHD_SIZE(hdSoundArchiveName, 8);
struct hdSoundArchive {
    be<u32> mVtable;
    gptr<void> mArchiveData;
    u8 _padding08[0x38];
};
WWHD_OFFSET(hdSoundArchive, mArchiveData, 4);
WWHD_SIZE(hdSoundArchive, 0x40);
struct hdSoundManagerArchiveView {
    u8 _padding00[0x144];
    gptr<void> mSoundArchive;
};
WWHD_OFFSET(hdSoundManagerArchiveView, mSoundArchive, 0x144);
struct hdSoundHeap {
    be<u32> mVtable;
    be<u32> mSecondaryVtable;
    u8 _padding08[0xF4];
    u8 mMemberStorage[0xD4]; // Opaque embedded NW4F object; only its address is passed here.
};
WWHD_OFFSET(hdSoundHeap, mMemberStorage, 0xFC);
WWHD_SIZE(hdSoundHeap, 0x1D0);
struct hdSoundHeapMemberArgument {
    be<u32> field_0x0; // HD: role unconfirmed; explicitly cleared after the parameter constructor.
    u8 _padding04[0x24];
};
WWHD_SIZE(hdSoundHeapMemberArgument, 0x28);

u32 hdSoundArchiveLookupId(void* archiveData, const char* name);
void hdSoundArchiveConstructBase(hdSoundArchive* archive);
void hdSoundArchiveSetName(hdSoundArchive* archive, hdSoundArchiveName* name);
const char* hdSoundGetArchivePath();
void hdSoundArchiveSetPath(hdSoundArchive* archive, const char* path);
// HD: original option method names are unknown; preserve their types and order.
void hdSoundArchiveSetFloatOption(hdSoundArchive* archive, f32 value);
void hdSoundArchiveSetFirstIntegerOption(hdSoundArchive* archive, u32 value);
void hdSoundArchiveSetSecondIntegerOption(hdSoundArchive* archive, u32 value);
hdSoundManagerArchiveView* hdSoundGetManager();
u32 hdSoundArchiveLoadGroup(void* archive, const char* name, s32 index, u32 option, u32 flags);
void hdSoundHeapDestroyBase(hdSoundHeap* heap, s32 flags);
void hdSoundHeapConstructBase(hdSoundHeap* heap);
void hdSoundHeapSetFirstOption(hdSoundHeap* heap, u32 first, u32 second);
void hdSoundHeapSetSecondOption(hdSoundHeap* heap, u32 value);
void hdSoundHeapSetThirdOption(hdSoundHeap* heap, u32 value);
void hdSoundHeapSetFourthOption(hdSoundHeap* heap, u32 value);
void hdSoundHeapConstructMemberArgument(hdSoundHeapMemberArgument* argument);
void hdSoundHeapInitializeMember(void* member, hdSoundHeapMemberArgument* argument);
