// STB sequence and paragraph records share the verified timeline parser ABI.
#pragma once
#include "JSystem/JStudio/JST_timeline.h"
namespace JStudio { namespace stb { namespace data {
struct TParse_TParagraph_data {
    struct TData {
        be<u8> status;
        u8 padding[3];
        be<u32> entrySize, entryCount;
        gptr<const u8> content, next;
    };
    gptr<const be<u8>> raw;
    void getData(TData*) const;
};
WWHD_SIZE(TParse_TParagraph_data,4);
WWHD_SIZE(TParse_TParagraph_data::TData,0x14);
WWHD_OFFSET(TParse_TParagraph_data::TData,entrySize,4);
WWHD_OFFSET(TParse_TParagraph_data::TData,entryCount,8);
WWHD_OFFSET(TParse_TParagraph_data::TData,content,0xC);
WWHD_OFFSET(TParse_TParagraph_data::TData,next,0x10);
}}}
