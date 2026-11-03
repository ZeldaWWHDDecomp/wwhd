#pragma once
#include "wwhd.h"
struct JKRFileLoaderStaticBlock { be<u32> mWords[4]; };
WWHD_SIZE(JKRFileLoaderStaticBlock,16);
void JKRFileLoaderConstructRegister(void*);

class JSUPtrList;
void JKRFileLoaderInitiateList(JSUPtrList*);


void JKRFileLoaderListDestroy(JSUPtrList*,u32);



struct JKRFileLoaderVtable {
    u8 mHeader[0x24];
    be<u32> mGetResource;
    be<u32> mGetTypedResourceThis;
    be<u32> mGetTypedResource;
};
class JKRFileLoader {
public:
    static const be<u8>* fetchVolumeName(be<u8>*,s32,const be<u8>*);
    static JKRFileLoader* findVolume(gptr<const be<u8>>*);
    static void* getGlbResource(const be<u8>*);
    static void* getGlbResourceFromVolume(const be<u8>*,JKRFileLoader*);
    u8 mHeader[0xc];
    gptr<JKRFileLoaderVtable> mVtable;
    u8 mPaddingName[0x10];
    gptr<const be<u8>> mVolumeName;
};
struct JKRVolumeNameBuffer { be<u8> mBytes[0x101]; };
WWHD_OFFSET(JKRFileLoader,mVtable,0xc);
WWHD_OFFSET(JKRFileLoader,mVolumeName,0x20);
WWHD_SIZE(JKRFileLoader,0x24);
WWHD_SIZE(JKRFileLoaderVtable,0x30);
WWHD_SIZE(JKRVolumeNameBuffer,0x101);
void* JKRFileLoaderCopyString(void*,const void*);


struct JKRFileLoaderRootPath { be<u8> mSlash; be<u8> mTerminator; };
WWHD_SIZE(JKRFileLoaderRootPath,2);

// The mounted-volume list (104A2430), owned by JSystem/JKernel/JKRFileLoaderStatics.cpp.
extern JSUPtrList& sVolumeList;
