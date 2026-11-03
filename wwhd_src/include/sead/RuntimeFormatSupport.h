/* Wii U byte-order helpers and UTF-16 format parsing, from this game's instructions. */
#pragma once
#include "wwhd.h"
namespace sead { namespace format {
struct VariadicState {
    be<u32> mRegisterState;
    gptr<void> mOverflowArguments;
    gptr<void> mRegisterArguments;
};
struct DigitBuffer;
struct FormatState {
    VariadicState mArguments;
    be<u32> mFlags;
    be<u32> mWidth;
    be<u32> mPrecision;
    be<u8> mHasPrecision;
    u8 mPadding[3];
    be<u32> mLength;
    enum Flags { LeftAligned=1, ShowPlus=2, ZeroPadded=4, BlankSign=8, Alternate=16 };
    enum Length { Byte=0, Halfword=1, Word=2, LongWord=3, DoubleWord=4 };
    void reset();
    void writeCharacter(DigitBuffer*);
    void writeString(DigitBuffer*);
    void writeByteDigits(DigitBuffer*,be<u8>*,u32,u32,u32);
    void writeHalfwordDigits(DigitBuffer*,be<u8>*,u32,u32,u32);
    void writeWordDigits(DigitBuffer*,be<u8>*,u32,u32,u32);
    void writeDoubleWordDigits(DigitBuffer*,be<u8>*,u32,u32,u32);
    be<u16>* parseFlags(be<u16>* text);
    be<u16>* parseWidth(be<u16>* text);
    be<u16>* parsePrecision(be<u16>* text);
    be<u16>* parseLength(be<u16>* text);
};
struct DigitBuffer {
    gptr<be<u16>> mText;
    be<s32> mCapacity;
    be<s32> mCount;
    be<u8> mOverflow;
    u8 mPadding[3];
    void writeWord(u32 radix,u32 uppercase,u32 value);
    void writeDoubleWord(u32 radix,u32 uppercase,u32 alignmentWord,u32 high,u32 low);
};
struct ByteTextBuffer {
    gptr<be<u8>> mText; gptr<void> mVtable; be<s32> mCapacity;
    s32 formatArguments(const char*,VariadicState*);
    s32 format(const char*,u32,u32,u32,u32,u32,u32);
};
struct WideTextBuffer {
    gptr<be<u16>> mText; gptr<void> mVtable; be<s32> mCapacity;
    s32 formatArguments(const be<u16>*,VariadicState*);
    s32 format(const be<u16>*,u32,u32,u32,u32,u32,u32);
};
WWHD_OFFSET(DigitBuffer,mCapacity,4);
WWHD_OFFSET(DigitBuffer,mCount,8);
WWHD_OFFSET(DigitBuffer,mOverflow,12);
WWHD_SIZE(DigitBuffer,16);
WWHD_OFFSET(ByteTextBuffer,mCapacity,8);
WWHD_SIZE(ByteTextBuffer,12);
WWHD_OFFSET(WideTextBuffer,mCapacity,8);
WWHD_SIZE(WideTextBuffer,12);
WWHD_SIZE(VariadicState,0xC);
WWHD_OFFSET(FormatState,mFlags,0xC);
WWHD_OFFSET(FormatState,mWidth,0x10);
WWHD_OFFSET(FormatState,mPrecision,0x14);
WWHD_OFFSET(FormatState,mHasPrecision,0x18);
WWHD_OFFSET(FormatState,mLength,0x1C);
WWHD_SIZE(FormatState,0x20);

/* The register save area of a GHS variadic function (the formatted-string constructors and
 * format()): its va_list (register indices, overflow area, save area), then r3..r10 and f1..f8 as
 * word pairs; the padding stays untouched. */
struct VariadicSaveArea {
    VariadicState mArguments;
    u8 mPadding[4];
    be<u32> mIntegers[8];
    be<u32> mFloatWords[16];
};
WWHD_OFFSET(VariadicSaveArea,mIntegers,0x10);
WWHD_OFFSET(VariadicSaveArea,mFloatWords,0x30);
WWHD_SIZE(VariadicSaveArea,0x70);

/* The prologue of a GHS variadic function: r3..r10 always, f1..f8 when the caller set CR bit 6. */
inline void saveVariadicRegisters(VariadicSaveArea* save, const u32 (&registers)[8]) {
    for (u32 index = 0; index < 8; ++index) save->mIntegers[index] = registers[index];
    if (gabi::cpu->cr[6]) {
        for (u32 index = 0; index < 8; ++index) {
            u64 bits;
            memcpy(&bits, &gabi::cpu->f[index + 1].ps0, 8);
            save->mFloatWords[index * 2] = u32(bits >> 32);
            save->mFloatWords[index * 2 + 1] = u32(bits);
        }
    }
}
/* va_start after the two named arguments (r3, r4): the overflow area is the caller's parameter
 * area (entry SP + 8); `frameSize` is the function's own frame. */
inline void startVariadicArguments(VariadicSaveArea* save, u32 frameSize) {
    auto indices = reinterpret_cast<be<u8>*>(&save->mArguments.mRegisterState);
    indices[0] = 2;  // r3 and r4 are named
    indices[1] = 0;
    save->mArguments.mOverflowArguments = gabi::at<void>(gabi::native_sp() + frameSize + 8);
    save->mArguments.mRegisterArguments = save->mIntegers;
}
} }
