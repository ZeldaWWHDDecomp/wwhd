#ifndef WWHD_JASDVDTHREAD_EXTRA_H
#define WWHD_JASDVDTHREAD_EXTRA_H
#include "wwhd.h"
struct DvdRecordMonitor { u8 storage[0x3C]; };
struct AudioFileRecord {
    gptr<AudioFileRecord> mNext;
    u8 prefix[0xA7C];
    DvdRecordMonitor mMonitor;
};
struct AudioFileManager {
    u8 prefix[0xC];
    gptr<void> mVtable;
    u8 opaque[0x14];
    be<u32> mActiveCount;
    gptr<AudioFileRecord> mFreeRecords;
    void freeRecord(AudioFileRecord*);
};
// HD: facade around the native file-loader records and monitor.
class JASDvdFile {
public:
    gptr<AudioFileRecord> mRecord;
    u8 prefix[0x14];
    be<u32> mFileSize;
    u8 opaque[0x50];
    be<u8> mReady;
    u8 padding[3];
    be<u32> mRequestState;
    DvdRecordMonitor mMonitor;
    gptr<void> mVtable;
    AudioFileRecord* allocateRecord();
    void releaseRecord(AudioFileRecord*);
    u32 isReady() const;
    u32 getFileSize() const;
};
JASDvdFile* constructDvdFile(JASDvdFile*);
WWHD_OFFSET(AudioFileRecord, mMonitor, 0xA80);
WWHD_SIZE(AudioFileRecord, 0xABC);
WWHD_OFFSET(AudioFileManager, mActiveCount, 0x24);
WWHD_OFFSET(AudioFileManager, mFreeRecords, 0x28);
WWHD_SIZE(AudioFileManager, 0x2C);
WWHD_OFFSET(JASDvdFile, mFileSize, 0x18);
WWHD_OFFSET(JASDvdFile, mReady, 0x6C);
WWHD_OFFSET(JASDvdFile, mRequestState, 0x70);
WWHD_OFFSET(JASDvdFile, mMonitor, 0x74);
WWHD_OFFSET(JASDvdFile, mVtable, 0xB0);
WWHD_SIZE(JASDvdFile, 0xB4);
#endif
