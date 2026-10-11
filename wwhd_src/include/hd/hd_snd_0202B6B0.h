/** HD sound-bank accessor and subsystem container; private decomp source. */
#pragma once
#include "wwhd.h"
#include "hd/hd_sys_02035444.h"
struct hdSoundArchiveAccessor {
    be<u32> mVtable;
    be<u32> field_0x4; // HD: constructor bookkeeping words; their roles are unconfirmed.
    be<u32> field_0x8;
    gptr<void> mBankTable;
    u16 bankBase(s32 bank) const {
        // HD: bank -1 and overflow retain the original 32-bit table-address wrapping.
        u32 address = gabi::ea(mBankTable.get()) + u32(bank) * 32 + 0x2E;
        return *gabi::at<be<u16>>(address);
    }
};
WWHD_OFFSET(hdSoundArchiveAccessor, mBankTable, 0xC);
WWHD_SIZE(hdSoundArchiveAccessor, 0x10);
struct hdSoundArchiveSafeString {
    gptr<const char> mText;
    be<u32> mVtable;
};
WWHD_SIZE(hdSoundArchiveSafeString, 8);
struct hdSoundArchiveLoadArg {
    hdSoundArchiveSafeString mName;
    be<u32> mSize;
    be<u32> mBufferSize;
    be<u32> mAlignment;
    be<u32> mBufferAlignment;
    be<u32> field_0x18; // HD: zeroed request options; exact meanings are unconfirmed.
    be<u32> field_0x1C;
    be<u32> field_0x20;
    be<u32> field_0x24;
    be<u32> field_0x28;
};
WWHD_OFFSET(hdSoundArchiveLoadArg, mSize, 8);
WWHD_OFFSET(hdSoundArchiveLoadArg, mAlignment, 0x10);
WWHD_OFFSET(hdSoundArchiveLoadArg, field_0x28, 0x28);
WWHD_SIZE(hdSoundArchiveLoadArg, 0x2C);
struct hdSoundArchiveResourceVtable {
    u8 _padding00[0xC];
    gfn mCheckRuntimeType;
};
struct hdSoundArchiveResource {
    u8 _padding00[0x10];
    gptr<hdSoundArchiveResourceVtable> mVtable;
    gptr<void> mData;
    static void* dataAfterCheck(hdSoundArchiveResource* resource) {
        // HD: a failed type check still loads the data word at guest NULL plus its field offset.
        u32 address = gabi::ea(resource) + offsetof(hdSoundArchiveResource, mData);
        return *gabi::at<gptr<void>>(address);
    }
    u32 checkType(hdTaskRuntimeTypeInfo* type) {
        return gabi::call_ptr<u32>(mVtable->mCheckRuntimeType.get(), this, type);
    }
};
WWHD_OFFSET(hdSoundArchiveResource, mVtable, 0x10);
WWHD_OFFSET(hdSoundArchiveResource, mData, 0x14);
struct hdSoundContainerSubsystemVtable {
    u8 _padding00[0x1C];
    gfn mDestroyFirst;
    u8 _padding20[4];
    gfn mDestroyLast;
};
WWHD_OFFSET(hdSoundContainerSubsystemVtable, mDestroyLast, 0x24);
struct hdSoundContainerSubsystem {
    gptr<hdSoundContainerSubsystemVtable> mVtable;
    void destroyFirst() { gabi::call_ptr<void>(mVtable->mDestroyFirst.get(), this, 3); }
    void destroyLast() { gabi::call_ptr<void>(mVtable->mDestroyLast.get(), this, 3); }
};
struct hdSoundSystemContainer {
    gptr<hdSoundContainerSubsystem> mSubsystems[6];
};
WWHD_SIZE(hdSoundSystemContainer, 0x18);
using hdSoundSubsystemConstructor = hdSoundContainerSubsystem* (*)(hdSoundContainerSubsystem*);
hdSoundArchiveSafeString* hdSoundArchiveGetTablePath();
hdSoundArchiveResource* hdSoundArchiveLoadResource(void*, hdSoundArchiveLoadArg*);
void* hdSoundContainerAllocate(u32, void*, u32);
hdSoundContainerSubsystem* hdSoundContainerConstructFirst(hdSoundContainerSubsystem*);
hdSoundContainerSubsystem* hdSoundContainerConstructSecond(hdSoundContainerSubsystem*);
hdSoundContainerSubsystem* hdSoundContainerConstructThird(hdSoundContainerSubsystem*);
hdSoundContainerSubsystem* hdSoundContainerConstructFourth(hdSoundContainerSubsystem*, u32, void*);
hdSoundContainerSubsystem* hdSoundContainerConstructFifth(hdSoundContainerSubsystem*);
hdSoundContainerSubsystem* hdSoundContainerConstructSixth(hdSoundContainerSubsystem*);
void hdSoundContainerDestroyFourth(hdSoundContainerSubsystem*, u32);
