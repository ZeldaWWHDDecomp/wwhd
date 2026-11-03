// HD DVD lifetime adapter; canonical JKRDvdFile storage is reused unchanged.
#pragma once
#include "JSystem/JKernel/JKRDvdFile.h"
#include "sead/FileDevice.h"
struct JKRDvdFileLifetime : JKRDvdFile {
    void destroy(u32 flags);
};
WWHD_SIZE(JKRDvdFileLifetime,0x16C);
void destroyNativeDvdLink(JSUPtrLink*,u32);
void destroyNativeDisposer(sead::Disposer*,u32);
void deleteNativeDvdFile(void*);
