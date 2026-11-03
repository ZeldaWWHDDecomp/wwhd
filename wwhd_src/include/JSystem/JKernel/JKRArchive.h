#pragma once
#include "JSystem/JKernel/JKRFileLoader.h"
struct JKRArchiveVtable : JKRFileLoaderVtable {
    u8 mArchiveMethods[0x44];
    be<u32> mGetResSize;
};
struct JKRArchive : JKRFileLoader {
    u32 getResSize(const void* resource) {
        JKRArchiveVtable* table=static_cast<JKRArchiveVtable*>(mVtable.get());
        return gabi::call_ptr<u32>(table->mGetResSize,this,resource);
    }
};
WWHD_OFFSET(JKRArchiveVtable,mGetResSize,0x74);WWHD_SIZE(JKRArchiveVtable,0x78);
WWHD_SIZE(JKRArchive,0x24);
