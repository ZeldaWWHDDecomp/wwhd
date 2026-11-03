#pragma once
#include "wwhd.h"
struct J3DEntryShapeHeader { u8 prefix[0xC];be<u16> mMaterialLookupIndex; };
struct J3DEntryShape { gptr<J3DEntryShapeHeader> mHeader; };
struct J3DEntryMaterialHeader { u8 prefix[0xE];be<u16> mMaterialIndex; };
struct J3DEntryMaterialLookup { gptr<J3DEntryMaterialHeader> mHeader;u8 padding[0x2C]; };
struct J3DEntryPacketVtable { u8 prefix[0x14];be<u32> mEntry; };
struct J3DEntryPacket {
    u8 prefix[0xC];gptr<J3DEntryPacketVtable> mVtable;u8 padding10[0x88];
    gptr<J3DEntryShape> mShape;u8 padding9C[8];be<u8> mDrawType;u8 paddingA5[7];
};
WWHD_SIZE(J3DEntryShapeHeader,0xE);WWHD_SIZE(J3DEntryShape,4);
WWHD_SIZE(J3DEntryMaterialHeader,0x10);WWHD_SIZE(J3DEntryMaterialLookup,0x30);
WWHD_SIZE(J3DEntryPacketVtable,0x18);WWHD_OFFSET(J3DEntryPacket,mShape,0x98);
WWHD_OFFSET(J3DEntryPacket,mDrawType,0xA4);WWHD_SIZE(J3DEntryPacket,0xAC);
