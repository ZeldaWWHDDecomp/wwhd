// Narrow method-table view for the graphics singleton query used by JSystem.
#pragma once
#include "wwhd.h"
struct NativeGraphicsQueryTable {
    u8 mOtherMethods[0x9C];
    be<u32> mQuery;
};
struct NativeGraphicsQueryDevice {
    u8 mObjectLinks[12];
    gptr<NativeGraphicsQueryTable> mTable;
};
WWHD_OFFSET(NativeGraphicsQueryTable,mQuery,0x9C);
WWHD_SIZE(NativeGraphicsQueryTable,0xA0);
WWHD_OFFSET(NativeGraphicsQueryDevice,mTable,12);
WWHD_SIZE(NativeGraphicsQueryDevice,16);
