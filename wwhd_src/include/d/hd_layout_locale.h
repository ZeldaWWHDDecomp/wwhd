#pragma once
#include "wwhd.h"
struct LayoutLocaleString_l { gptr<const be<u8>> mpBuffer; gptr<gfn> mpVTable; };
struct LayoutLocaleBuffer_l { u8 mReserved[0xC]; be<u8> mBuffer[1]; };
struct LayoutLocalePane_l {};
struct LayoutLocaleRoot_l { u8 mReserved[0xC]; gptr<LayoutLocalePane_l> mpRoot; };
struct LayoutLocaleEntry_l {
    be<u32> mNameOffset;
    be<u32> mReserved;
    be<u16> mSize;
    be<u8> mType;
    u8 mPadding;
};
WWHD_OFFSET(LayoutLocaleBuffer_l, mBuffer, 0xC);
WWHD_OFFSET(LayoutLocaleRoot_l, mpRoot, 0xC);
WWHD_OFFSET(LayoutLocaleEntry_l, mType, 0xA);
WWHD_SIZE(LayoutLocaleEntry_l, 0xC);
WWHD_SIZE(LayoutLocaleString_l, 8);
class hdLayoutLocale_c {
public:
    static u32 getSuffixLength();
    static u32 getSuffixCount();
    static u32 hasAnySuffix(LayoutLocaleString_l* name);
    static u32 hasCurrentSuffix(LayoutLocaleString_l* name);
    static u32 bufferHasAnySuffix(LayoutLocaleBuffer_l* buffer);
    static u32 bufferHasCurrentSuffix(LayoutLocaleBuffer_l* buffer);
    static u32 bufferHasOtherSuffix(LayoutLocaleBuffer_l* buffer);
    static LayoutLocalePane_l* findPane(LayoutLocaleString_l* name, LayoutLocalePane_l* root, LayoutLocaleString_l* suffix);
    static LayoutLocalePane_l* findCurrentPane(LayoutLocaleString_l* name, LayoutLocaleRoot_l* layout);
    static LayoutLocalePane_l* findCurrentPaneBuffer(LayoutLocaleBuffer_l* buffer, LayoutLocaleRoot_l* layout);
    static LayoutLocaleEntry_l* findEntry(const be<u8>* name, LayoutLocaleEntry_l* entries, s32 count, u32 type);
};
const be<u8>* LayoutLocale_currentSuffix();
