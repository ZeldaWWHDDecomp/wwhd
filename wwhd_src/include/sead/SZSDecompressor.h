/* WWHD sead SZS (Yaz0) decompressor: a decompressor for the "szs" extension with a work buffer for file
 * reads, whole-image decoding and the streaming decoder state.
 * Layouts from this game's constructors (SZSDecompressor 0x58, DecompContext 0x18) and instructions; the
 * Yaz0 header layout from the public format description (facts only). */
#pragma once
#include "sead/ResourceMgr.h"

namespace sead {

/* Yaz0 file header */
struct SzsHeader {
    be<u32> mMagic;               // 'Yaz0'
    be<u32> mDecompressedSize;
    be<u32> mAlignment;           // 0: the resource's own alignment
    be<u32> mReserved;
};

struct SZSDecompressor : Decompressor {
    be<u32> mWorkSize;
    gptr<u8> mWorkBuffer;         // NULL: allocated for each load

    enum Step { cExpectToken = 0, cExpectPairLow = 1, cExpectCountByte = 2 };

    /* streaming state: survives between reads of the compressed file */
    struct DecompContext {
        gptr<u8> mDestination;
        be<s32> mDestCount;       // bytes still to write
        be<s32> mForceDestCount;  // when nonzero: stop after this many bytes
        be<u8> mFlagMask;
        be<u8> mFlags;
        be<u8> mPackHigh;
        u8 mPadding;
        be<s32> mStep;
        be<u16> mLzOffset;
        be<u8> mHeaderSize;       // header bytes still to read
        u8 mPadding2;

        static DecompContext* construct(DecompContext* context, u8* destination);
        void initialize(u8* destination);
    };

    static SZSDecompressor* construct(SZSDecompressor* decompressor, u32 workSize, u8* workBuffer);
    void destroy(u32 flags);
    static void destroyFromDisposer(Disposer* disposer, u32 flags);
    u8* tryDecompFromDevice(ResourceLoadArg* arg, Resource* resource, be<u32>* size, be<u32>* bufferSize,
                            be<u8>* ownsData);
    static u32 getDecompSize(const SzsHeader* header);
    static u32 getDecompAlignment(const SzsHeader* header);
    static s32 decode(u8* destination, const SzsHeader* source);
    static s32 decomp(u8* destination, u32 destinationSize, const SzsHeader* source, u32 sourceSize);
    static s32 readHeader(DecompContext* context, const be<u8>* source, u32 length);
    static s32 streamDecomp(DecompContext* context, const be<u8>* source, u32 length);
};

/* the decompressor base's copy of the destructor in this translation unit */
struct SZSDecompressorBase : Decompressor {
    void destroy(u32 flags);
    static void destroyFromDisposer(Disposer* disposer, u32 flags);
};

/* per-TU copies of the decompressor translation unit's text and file-handle classes */
namespace szs_text {
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct TextBuffer : SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct FixedText : SafeTextBuffer { void destroy(u32 flags); };        // FixedSafeText<32> base
struct ExtensionText : SafeTextBuffer { void destroy(u32 flags); };    // the decompressor's extension
struct FileHandle : sead::FileHandle { void destroy(u32 flags); };
}

}

WWHD_OFFSET(sead::SzsHeader, mDecompressedSize, 4);
WWHD_OFFSET(sead::SzsHeader, mAlignment, 8);
WWHD_SIZE(sead::SzsHeader, 0x10);
WWHD_OFFSET(sead::SZSDecompressor, mWorkSize, 0x50);
WWHD_OFFSET(sead::SZSDecompressor, mWorkBuffer, 0x54);
WWHD_SIZE(sead::SZSDecompressor, 0x58);
WWHD_OFFSET(sead::SZSDecompressor::DecompContext, mDestCount, 4);
WWHD_OFFSET(sead::SZSDecompressor::DecompContext, mForceDestCount, 8);
WWHD_OFFSET(sead::SZSDecompressor::DecompContext, mFlagMask, 0xC);
WWHD_OFFSET(sead::SZSDecompressor::DecompContext, mFlags, 0xD);
WWHD_OFFSET(sead::SZSDecompressor::DecompContext, mPackHigh, 0xE);
WWHD_OFFSET(sead::SZSDecompressor::DecompContext, mStep, 0x10);
WWHD_OFFSET(sead::SZSDecompressor::DecompContext, mLzOffset, 0x14);
WWHD_OFFSET(sead::SZSDecompressor::DecompContext, mHeaderSize, 0x16);
WWHD_SIZE(sead::SZSDecompressor::DecompContext, 0x18);
