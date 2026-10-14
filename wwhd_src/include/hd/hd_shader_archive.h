/* HD shader archive manager: three persistent shader families. Private decomp source. */
#pragma once
#include "bindings.h"
struct hdShaderArchive_c {
    u8 _00[0x54];
    gptr<const char> mpName;
    be<u32> mNameVtable;
};
WWHD_OFFSET(hdShaderArchive_c, mpName, 0x54);
WWHD_SIZE(hdShaderArchive_c, 0x5C);
struct hdShaderArchiveMgr_c {
    u8 _00[0xC];
    be<u32> mVtable;
    be<u32> mCapacity;
    gptr<hdShaderArchive_c> mpArchives;
    be<u32> mCount;
    be<u8> mLoaded;
    u8 _1D[3];
    void loadPrimitive();
    void loadParticle();
    void loadRenderBuffer();
    void initialize();
    hdShaderArchive_c* currentArchive() {
        u32 count = mCount;
        u32 capacity = mCapacity;
        return count < capacity ? gabi::at<hdShaderArchive_c>(gabi::ea(mpArchives.get()) + count * sizeof(hdShaderArchive_c)) : NULL;
    }
};
WWHD_OFFSET(hdShaderArchiveMgr_c, mVtable, 0xC);
WWHD_OFFSET(hdShaderArchiveMgr_c, mCapacity, 0x10);
WWHD_OFFSET(hdShaderArchiveMgr_c, mpArchives, 0x14);
WWHD_OFFSET(hdShaderArchiveMgr_c, mCount, 0x18);
WWHD_OFFSET(hdShaderArchiveMgr_c, mLoaded, 0x1C);
WWHD_SIZE(hdShaderArchiveMgr_c, 0x20);
struct hdShaderLoadArg_c {
    SafeString mPath;
    be<u32> mHeap;
    be<u32> mFlags;
    be<u32> mAlignment;
    be<u32> mBufferSize;
    be<u32> mBuffer;
    be<u32> mReadSize;
    be<u32> mFileDevice;
    be<u32> mOffset;
    be<u32> mReadFlags;
};
WWHD_SIZE(hdShaderLoadArg_c, 0x2C);
struct hdShaderFileVtable_l { u8 _00[0xC]; be<u32> mIsKindOf; };
WWHD_OFFSET(hdShaderFileVtable_l,mIsKindOf,0xC);
struct hdShaderFileResource_l {
    u8 _00[0x10];
    gptr<hdShaderFileVtable_l> mpVtable;
    gptr<void> mpBinary;
};
WWHD_OFFSET(hdShaderFileResource_l, mpVtable, 0x10);
WWHD_OFFSET(hdShaderFileResource_l, mpBinary, 0x14);
struct hdShaderHeapVtable_l { u8 _00[0x34]; be<u32> mAllocate; };
WWHD_OFFSET(hdShaderHeapVtable_l,mAllocate,0x34);
struct hdShaderHeap_l { u8 _00[0xC]; gptr<hdShaderHeapVtable_l> mpVtable; };
WWHD_OFFSET(hdShaderHeap_l,mpVtable,0xC);

struct hdShaderBinaryArg_c { gptr<void> mpBinary; };
struct hdShaderWorkArg_c { be<u32> mWork; };
WWHD_SIZE(hdShaderBinaryArg_c, 4);
WWHD_SIZE(hdShaderWorkArg_c, 4);

struct hdShaderArchiveHeaderZero_c { be<u32> mWords[4]; };
WWHD_SIZE(hdShaderArchiveHeaderZero_c,16);
struct hdShaderArchiveHeaderDescriptor_c { u8 mData[12]; };
WWHD_SIZE(hdShaderArchiveHeaderDescriptor_c,12);
