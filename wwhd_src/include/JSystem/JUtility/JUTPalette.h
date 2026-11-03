#pragma once
#include "wwhd.h"
struct JUTPaletteStaticBlock { be<u32> mWords[4]; };
WWHD_SIZE(JUTPaletteStaticBlock,16);
void JUTPaletteConstructRegister(void*);


struct ResTLUT { be<u8> format; be<u8> transparency; be<u16> numColors; u8 padding[0x1c]; };
class JUTPalette {
public:
    void storeTLUT(u32,ResTLUT*);
    void storeTLUTRaw(u32,u32,u32,u32,void*);
    u32 load();
    u8 mTlutObj[0xc];
    be<u8> mTlutName;
    be<u8> mFormat;
    u8 mPadding[2];
    gptr<void> mColorTable;
    be<u16> mNumColors;
    be<u8> mTransparency;
    u8 mTrailing;
};
WWHD_OFFSET(JUTPalette,mTlutName,0xc);
WWHD_OFFSET(JUTPalette,mFormat,0xd);
WWHD_OFFSET(JUTPalette,mColorTable,0x10);
WWHD_OFFSET(JUTPalette,mNumColors,0x14);
WWHD_OFFSET(JUTPalette,mTransparency,0x16);
WWHD_SIZE(JUTPalette,0x18);
WWHD_SIZE(ResTLUT,0x20);
void JUTPaletteReport(const char*,s32,const char*);
void JUTPaletteAssert(const char*,s32,const char*);
