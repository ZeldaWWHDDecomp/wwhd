// STB parser callback tables and header prefix, checked against WWHD's loads.
#pragma once
#include "JSystem/JStudio/stb-control.h"
namespace JStudio::stb {
struct ParseVTable {
    u8 prefix[0x24]; gfn parseHeader;
    u8 gapHeader[4]; gfn parseBlock;
    u8 gapBlock[4]; gfn parseObject;
};
struct HeaderPrefix {
    be<u8> signature[4];
    be<u16> byteOrder, version;
    be<u32> reserved, blockNumber;
};
WWHD_OFFSET(ParseVTable, parseHeader, 0x24);
WWHD_OFFSET(ParseVTable, parseBlock, 0x2C);
WWHD_OFFSET(ParseVTable, parseObject, 0x34);
WWHD_SIZE(HeaderPrefix, 0x10);
WWHD_OFFSET(HeaderPrefix, blockNumber, 0xC);
}
