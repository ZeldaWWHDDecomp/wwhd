#pragma once
#include "wwhd.h"
struct JKRDvdFileStaticBlock { be<u32> mWords[4]; };
WWHD_SIZE(JKRDvdFileStaticBlock,16);
void JKRDvdFileConstructRegister(void*);

class JSUPtrList;
void JKRDvdFileInitiateList(JSUPtrList*);


#include "JSystem/JSupport/JSUList.h"
struct JKRDvdFileVtable {
    u8 padding[0x34];
    be<u32> mGetFileSize;
};
/* JKRDvdFile (tww JKRDvdFile.h; canonical, merges JS-base's view with the
 * DVD-ripper adapters' one). HD: the DVD file info is the native file's tail block. */
class JKRDvdFile {
public:
    u32 open(const void*);
    void close();
    u8 mDisposer[0xC];                  /* JKRDisposer base */
    gptr<JKRDvdFileVtable> __vtbl;
    be<u8> mIsAvailable;
    u8 mPaddingFileInfo[0x6b];
    u8 mFileInfo[0xdc];
    JSUPtrLink mDvdLink;
    u8 mTrailing[4];
};
struct JKRDvdPath { be<u8> mBytes[0x101]; };
WWHD_OFFSET(JKRDvdFile,__vtbl,0xc);
WWHD_OFFSET(JKRDvdFile,mIsAvailable,0x10);
WWHD_OFFSET(JKRDvdFile,mFileInfo,0x7c);
WWHD_OFFSET(JKRDvdFile,mDvdLink,0x158);
WWHD_SIZE(JKRDvdFile,0x16c);
WWHD_SIZE(JKRDvdPath,0x101);
void JKRDvdCopyPath(void*,const void*,u32);
u32 JKRDvdOpenPath(void*,void*);
u32 JKRDvdGetStatus(JKRDvdFile*);
u32 JKRDvdCloseInfo(void*);
u32 JKRDvdAppend(JSUPtrList*,JSUPtrLink*);
u32 JKRDvdRemove(JSUPtrList*,JSUPtrLink*);
void JKRDvdCloseAssert(const char*,s32,const char*);
