/** HD DVD-ripper implementation views.
 * HD: the DVD file info moves to the native file's tail; decompressed-size output is an extra argument.
 * Only verified prefixes and the private stream-state block are modeled. */
#pragma once
#include "bindings.h"
enum JKRExpandSwitch {
    EXPAND_SWITCH_UNKNOWN0,
    EXPAND_SWITCH_UNKNOWN1,
    EXPAND_SWITCH_UNKNOWN2,
};
enum JKRDvdAllocDirection {
    DEFAULT_EALLOC_DIRECTION,
    ALLOC_DIRECTION_FORWARD,
    ALLOC_DIRECTION_BACKWARD,
};
enum JKRDvdCompression { COMPRESSION_NONE, COMPRESSION_YAY0, COMPRESSION_YAZ0, COMPRESSION_ASR };
struct JKRDvdFileVtable {
    u8 padding[0x34];
    be<u32> mGetFileSize;
};
struct JKRDvdFile {
    u8 padding_0[0xC];
    gptr<JKRDvdFileVtable> __vtbl;
    u8 padding_10[0x6C];
    u8 mFileInfo;
};
struct JKRDvdStreamState {
    /* 0x00 */ gptr<be<u8>> szpBuf;
    /* 0x04 */ gptr<be<u8>> szpEnd;
    /* 0x08 */ gptr<be<u8>> refBuf;
    /* 0x0C */ gptr<be<u8>> refEnd;
    /* 0x10 */ gptr<be<u8>> refCurrent;
    /* 0x14 */ be<u32> srcOffset;
    /* 0x18 */ be<u32> transLeft;
    /* 0x1C */ gptr<be<u8>> srcLimit;
    /* 0x20 */ gptr<JKRDvdFile> srcFile;
    /* 0x24 */ be<u32> fileOffset;
    /* 0x28 */ be<u32> readCount;
    /* 0x2C */ be<u32> maxDest;
    /* 0x30 */ gptr<be<u32>> pDecompressedSize;
    /* 0x34 */ be<u32> decompressedSize;
};
struct SYaz0Header {
    be<u32> signature, length;
};
WWHD_OFFSET(JKRDvdFile, __vtbl, 0xC);
WWHD_OFFSET(JKRDvdFile, mFileInfo, 0x7C);
WWHD_SIZE(JKRDvdStreamState, 0x38);
WWHD_OFFSET(JKRDvdStreamState, srcFile, 0x20);
WWHD_OFFSET(JKRDvdStreamState, pDecompressedSize, 0x30);
WWHD_SIZE(SYaz0Header, 8);
s32 DVDRipper_DVDReadPrio(void *, void *, u32, u32, u32);
void DVDRipper_VIWaitForRetrace();
u32 DVDRipper_checkCompressed(void *);
void *DVDRipper_allocFromHeap(void *, u32, s32);
void *DVDRipper_allocFromSysHeap(u32, s32);
void DVDRipper_free(void *);
void DVDRipper_freeRefillFailure(void *, u32);
void DVDRipper_invalidateRange(void *, u32);
void DVDRipper_storeRangeNoSync(void *, u32);
void DVDRipper_decompress(void *, void *, u32, u32);
void DVDRipper_panic(void *, u32, void *);
u32 DVDRipper_disableInterrupts();
void DVDRipper_restoreInterrupts(u32);
void DVDRipper_initMutex(void *);
void DVDRipper_lockMutex(void *);
void DVDRipper_unlockMutex(void *);
void DVDRipper_move(void *, void *, u32);
