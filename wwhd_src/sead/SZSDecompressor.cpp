/* WWHD sead SZS (Yaz0) decompressor: construction for the "szs" extension, the whole-image decoder (which
 * zeroes each 32-byte cache block ahead of its writes), the streaming decoder for files larger than the
 * work buffer, loading and decompressing a file through a device, and this translation unit's copies of
 * the decompressor destructors, text and file-handle classes.
 * Reconstructed from this game's instructions; Yaz0 header layout from the public format description
 * (facts only). */
// format facts: Yaz0 (SZS) compression format (public description)
#include "sead/SZSDecompressor.h"

using namespace sead;

static void* allocateObject(u32 size);
static void* allocateArrayFromHeap(u32 size, RegistryHeap* heap, s32 alignment);
static void releaseObject(void* object);
static void releaseArray(void* storage);
static void constructDisposer(Disposer* disposer);
static void destroyDisposer(Disposer* disposer, u32 flags);
static void* OSBlockMove(void* destination, const void* source, u32 size, u32 flush);

static constexpr u32 kDestroyRelease = 1;
static constexpr u32 kYaz0 = 0x59617A30;          // 'Yaz0'
static constexpr s32 kErrorFormat = -1;
static constexpr s32 kErrorDestinationSize = -2;
static constexpr u32 kHeaderSize = 0x10;
static constexpr u32 kCacheBlock = 0x20;
static constexpr s32 kFastLoopReserve = 0x132;    // the decoder's checked tail: the longest match plus one
static constexpr u32 kWorkAlignment = 0x40;       // work buffers: whole file-device blocks

/* the platform's byte order and the 32-bit converters indexed by it */
struct ByteOrderSetting { be<u32> mValue; };
struct ByteOrderConverters32 { gfn mConvert[2]; };

static ByteOrderSetting sNativeByteOrder;
static ByteOrderConverters32 sConvertBigU32;

static DecompressorVtable sDecompressorVtable;
static DisposerVtable sDecompressorDisposerVtable;
static DecompressorVtable sSZSDecompressorVtable;
static DisposerVtable sSZSDecompressorDisposerVtable;
static SafeTextVtable sTextVtable;
static SafeTextVtable sFixedTextVtable;
static SafeTextVtable sExtensionTextVtable;
static DisposerVtable sFileHandleVtable;
static u32 sDirectResourceTypeGuard;
static RuntimeType sDirectResourceType;
static u32 sDirectResourceTypeTable;
static ResourceMgr* sResourceMgr;

template <class T>
static T* memberStorage(T* member) {
    if (gabi::ea(member) == 0) member = static_cast<T*>(allocateObject(sizeof(T)));
    return member;
}

/* inlined: length of a text, 0 when it runs past kMaxTextLength */
static s32 textLength(const be<u8>* text) {
    s32 length = 0;
    if (text[0] != 0) {
        do {
            ++length;
            if (length > kMaxTextLength) return 0;
        } while (text[length] != 0);
    }
    return length;
}

/* dcbz: the 32-byte cache block holding the address becomes zero */
static void zeroCacheBlock(u32 address) {
    be<u32>* block = gabi::at<be<u32>>(address & ~(kCacheBlock - 1));
    for (u32 i = 0; i < kCacheBlock / 4; ++i) block[i] = 0;
}

/* slw: shift amounts of 32 and more give 0 */
static u32 shiftLeftWord(u32 value, u32 amount) {
    amount &= 0x3F;
    return amount < 32 ? value << amount : 0;
}

/* inlined: the header word in the platform's byte order */
static u32 bigToHost(u32 value) {
    return gabi::call_ptr<u32>(sConvertBigU32.mConvert[sNativeByteOrder.mValue].get(), value);
}

/* inlined: the file handle's destructor */
static void closeHandle(FileHandle* handle) {
    FileDevice* device = handle->mSourceDevice;
    handle->mDisposer.mVtable = &sFileHandleVtable;
    if (device != NULL) device->tryClose(handle);
    destroyDisposer(&handle->mDisposer, 0);
}

/* 0275E9C0 */
void sead::SZSDecompressorBase::destroyFromDisposer(Disposer* disposer, u32 flags) {
    reinterpret_cast<SZSDecompressorBase*>(reinterpret_cast<u8*>(disposer) - offsetof(Decompressor, mDisposer))
        ->destroy(flags);
}

/* 0275E9C8 */
void sead::SZSDecompressor::destroyFromDisposer(Disposer* disposer, u32 flags) {
    reinterpret_cast<SZSDecompressor*>(reinterpret_cast<u8*>(disposer) - offsetof(Decompressor, mDisposer))
        ->destroy(flags);
}

/* 0275E9D0 */
SZSDecompressor* sead::SZSDecompressor::construct(SZSDecompressor* decompressor, u32 workSize, u8* workBuffer) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (decompressor == NULL) {
        decompressor = static_cast<SZSDecompressor*>(allocateObject(sizeof(SZSDecompressor)));
        if (decompressor == NULL) return NULL;
    }
    gabi::FrameLocal<SafeText> defaultExtension(8);
    defaultExtension->mVtable = &sTextVtable;
    defaultExtension->mText.v = gabi::guest_str("szs");
    TypedListNode* node = memberStorage(&decompressor->mNode);
    if (node != NULL) {
        node->mNext = NULL;
        node->mData = decompressor;
        node->mPrevious = NULL;
        node->mList = NULL;
    }
    constructDisposer(&decompressor->mDisposer);
    decompressor->mVtable = &sDecompressorVtable;
    decompressor->mDisposer.mVtable = &sDecompressorDisposerVtable;

    /* the extension: a copy of "szs" */
    FixedSafeText<32>* extension = memberStorage(&decompressor->mExtension);
    if (extension != NULL) {
        be<u8>* out = &extension->mStorage[0];
        extension->mText = out;
        extension->mStorage[31] = 0;
        extension->mVtable = &sFixedTextVtable;
        extension->mCapacity = 32;
        defaultExtension->assureTermination();
        s32 length = textLength(defaultExtension->mText);
        if (length >= extension->mCapacity) length = extension->mCapacity - 1;
        defaultExtension->assureTermination();
        OSBlockMove(out, defaultExtension->mText, length, 0);
        out[length] = 0;
        extension->mVtable = &sExtensionTextVtable;
    }
    decompressor->mVtable = &sSZSDecompressorVtable;
    decompressor->mDisposer.mVtable = &sSZSDecompressorDisposerVtable;
    if (workBuffer == NULL) {
        decompressor->mWorkBuffer = NULL;
        decompressor->mWorkSize = (workSize + kWorkAlignment - 1) & ~(kWorkAlignment - 1);
    } else {
        decompressor->mWorkBuffer = workBuffer;
        decompressor->mWorkSize = workSize;
    }
    return decompressor;
}

/* 0275EB8C */
u32 sead::SZSDecompressor::getDecompSize(const SzsHeader* header) {
    return gabi::tail_ptr<u32>(sConvertBigU32.mConvert[sNativeByteOrder.mValue].get(), static_cast<u32>(header->mDecompressedSize));
}

/* 0275EBC0 */
s32 sead::SZSDecompressor::decode(u8* destination, const SzsHeader* source) {
    /* hand-written in the original: the size word is read as is, the output pointer and the input pointer
     * advance before each access, and outside the checked tail every new cache block is zeroed first */
    u32 total = source->mDecompressedSize;
    s32 remaining = total;
    be<u8>* out = reinterpret_cast<be<u8>*>(destination) - 1;
    const be<u8>* in = reinterpret_cast<const be<u8>*>(source) + kHeaderSize - 1;
    u32 mask = 0;
    u32 flags = 0;

    if (remaining > kFastLoopReserve) {
        remaining -= kFastLoopReserve;
        for (;;) {
            mask >>= 1;
            if (mask == 0) {
                flags = *++in;
                mask = 0x80;
            }
            u32 code = *++in;
            if (flags & mask) {
                if ((gabi::ea(out) & (kCacheBlock - 1)) == 0) zeroCacheBlock(gabi::ea(out) + kCacheBlock);
                *++out = code;
                if (--remaining == 0) break;
                continue;
            }
            u32 low = *++in;
            u32 count = code >> 4;
            if (count == 0) count = *++in + 0x10;
            count += 2;
            u32 distance = ((code & 0xF) << 8) | low;
            remaining -= count;
            do {
                u32 byte = out[-static_cast<s32>(distance)];
                if ((gabi::ea(out) & (kCacheBlock - 1)) == 0) zeroCacheBlock(gabi::ea(out) + kCacheBlock);
                *++out = byte;
            } while (--count != 0);
            if (remaining <= 0) break;
        }
        remaining += kFastLoopReserve;
        if (remaining <= 0) return total;
    }

    /* the tail: no cache-block zeroing, a match may not overrun */
    for (;;) {
        mask >>= 1;
        if (mask == 0) {
            flags = *++in;
            mask = 0x80;
        }
        u32 code = *++in;
        if (flags & mask) {
            *++out = code;
            if (--remaining == 0) break;
            continue;
        }
        u32 low = *++in;
        u32 count = code >> 4;
        if (count == 0) count = *++in + 0x10;
        count += 2;
        u32 distance = ((code & 0xF) << 8) | low;
        remaining -= count;
        if (remaining < 0) break;
        do {
            u32 byte = out[-static_cast<s32>(distance)];
            *++out = byte;
        } while (--count != 0);
        if (remaining <= 0) break;
    }
    return total;
}

/* 0275ED14 */
s32 sead::SZSDecompressor::decomp(u8* destination, u32 destinationSize, const SzsHeader* source, u32 sourceSize) {
    if (bigToHost(source->mMagic) != kYaz0) return kErrorFormat;
    if (destinationSize < getDecompSize(source)) return kErrorDestinationSize;
    return decode(destination, source);
}

/* 0275EDC8 */
s32 sead::SZSDecompressor::readHeader(DecompContext* context, const be<u8>* source, u32 length) {
    s32 read = 0;
    u32 remainingHeader = context->mHeaderSize;
    if (remainingHeader != 0) {
        for (;;) {
            remainingHeader = (remainingHeader - 1) & 0xFF;
            context->mHeaderSize = remainingHeader;
            if (remainingHeader == 0xF) {
                if (source[0] != 'Y') return -1;
            } else if (remainingHeader == 0xE) {
                if (source[0] != 'a') return -1;
            } else if (remainingHeader == 0xD) {
                if (source[0] != 'z') return -1;
            } else if (remainingHeader == 0xC) {
                if (source[0] != '0') return -1;
            } else if (remainingHeader >= 8) {
                /* the decompressed size, big endian */
                context->mDestCount =
                    context->mDestCount | shiftLeftWord(source[0], (remainingHeader - 8) * 8);
                remainingHeader = context->mHeaderSize;
            }
            --length;
            ++read;
            ++source;
            if (length == 0) {
                if (remainingHeader != 0) return read;
                break;
            }
            if (remainingHeader == 0) break;
        }
    }
    s32 forced = context->mForceDestCount;
    if (forced > 0 && forced < context->mDestCount) context->mDestCount = forced;
    return read;
}

/* 0275EEB8 */
s32 sead::SZSDecompressor::streamDecomp(DecompContext* context, const be<u8>* source, u32 length) {
    if (context->mHeaderSize != 0) {
        s32 read = readHeader(context, source, length);
        if (read < 0) return read;
        length -= read;
        source += read;
        if (length == 0) {
            if (context->mHeaderSize != 0) return -1;
            return context->mDestCount;
        }
    }
    s32 remaining = context->mDestCount;
    u32 packHigh = context->mPackHigh;
    be<u8>* out = reinterpret_cast<be<u8>*>(context->mDestination.get());
    u32 flags = context->mFlags;
    u32 mask = context->mFlagMask;
    s32 step = context->mStep;
    u32 lzOffset = context->mLzOffset;

    while (remaining > 0) {
        if (step == cExpectCountByte) {
            u32 count = *source++ + 0x12;
            --length;
            if (count > static_cast<u32>(remaining)) {
                if (context->mForceDestCount == 0) return kErrorDestinationSize;
                count = remaining & 0xFFFF;
            }
            remaining -= count;
            for (u32 offset = kCacheBlock; offset < count; offset += kCacheBlock) {
                zeroCacheBlock(gabi::ea(out) + offset);
            }
            do {
                *out = out[-static_cast<s32>(lzOffset)];
                ++out;
            } while (--count != 0);
            step = cExpectToken;
        } else if (step == cExpectPairLow) {
            u32 pack = (packHigh << 8) | *source++;
            --length;
            u32 count = pack >> 12;
            lzOffset = (pack & 0xFFF) + 1;
            if (count == 0) {
                step = cExpectCountByte;
            } else {
                count += 2;
                if (count > static_cast<u32>(remaining)) {
                    if (context->mForceDestCount == 0) return kErrorDestinationSize;
                    count = remaining & 0xFFFF;
                }
                if ((gabi::ea(out) & (kCacheBlock - 1)) + count > kCacheBlock) zeroCacheBlock(gabi::ea(out) + count);
                remaining -= count;
                do {
                    *out = out[-static_cast<s32>(lzOffset)];
                    ++out;
                } while (--count != 0);
                step = cExpectToken;
            }
        } else {
            if (mask == 0) {
                --length;
                mask = 0x80;
                flags = *source++;
                if (length == 0) break;
            }
            if (flags & mask) {
                if ((gabi::ea(out) & (kCacheBlock - 1)) == 0) zeroCacheBlock(gabi::ea(out));
                *out++ = *source++;
                --remaining;
            } else {
                packHigh = *source++;
                step = cExpectPairLow;
            }
            --length;
            mask >>= 1;
        }
        if (length == 0) break;
    }

    context->mDestination = reinterpret_cast<u8*>(out);
    context->mFlags = flags;
    context->mFlagMask = mask;
    context->mStep = step;
    context->mPackHigh = packHigh;
    context->mDestCount = remaining;
    context->mLzOffset = lzOffset;
    /* a finished stream that still has more than a block of input is malformed */
    if (remaining == 0 && context->mForceDestCount == 0 && length > kCacheBlock) return -1;
    return remaining;
}

/* 0275F164 */
u32 sead::SZSDecompressor::getDecompAlignment(const SzsHeader* header) {
    return gabi::tail_ptr<u32>(sConvertBigU32.mConvert[sNativeByteOrder.mValue].get(), static_cast<u32>(header->mAlignment));
}

/* 0275F188 */
void sead::SZSDecompressor::DecompContext::initialize(u8* destination) {
    mDestination = destination;
    mLzOffset = 0;
    mHeaderSize = kHeaderSize;
    mForceDestCount = 0;
    mStep = cExpectToken;
    mFlags = 0;
    mPackHigh = 0;
    mFlagMask = 0;
    mDestCount = 0;
}

/* 0275F1B8 */
SZSDecompressor::DecompContext* sead::SZSDecompressor::DecompContext::construct(DecompContext* context,
                                                                                u8* destination) {
    if (context == NULL) {
        context = static_cast<DecompContext*>(allocateObject(sizeof(DecompContext)));
        if (context == NULL) return NULL;
    }
    context->initialize(destination);
    return context;
}

/* 0275F200 */
u8* sead::SZSDecompressor::tryDecompFromDevice(ResourceLoadArg* arg, Resource* resource, be<u32>* size,
                                               be<u32>* bufferSize, be<u8>* ownsData) {
    gabi::AutoFrame frame{gabi::current_function()};
    RegistryHeap* heap = arg->mLoadDataHeap;
    if (heap == NULL) heap = registryCurrentHeap(sRegistryManager.get());

    gabi::FrameLocal<FileHandle> handle(8);
    constructDisposer(&handle->mDisposer);
    FileDevice* device = arg->mDevice;
    handle->mDevice = NULL;
    handle->mDisposer.mVtable = &sFileHandleVtable;
    handle->mSourceDevice = NULL;
    handle->mReadChunkSize = 0;
    FileDevice* opened;
    if (device != NULL) opened = device->tryOpen(handle.get(), &arg->mPath, 0, arg->mReadChunkSize);
    else opened = sFileDeviceManager->tryOpen(handle.get(), &arg->mPath, 0, arg->mReadChunkSize);
    if (opened == NULL) {
        closeHandle(handle.get());
        return NULL;
    }

    u8* work = mWorkBuffer;
    if (work == NULL) {
        work = static_cast<u8*>(allocateArrayFromHeap(mWorkSize, heap, -static_cast<s32>(kWorkAlignment)));
        if (work == NULL) {
            closeHandle(handle.get());
            return NULL;
        }
    }
    u32 readSize = handle->read(work, mWorkSize);
    u8* destination = NULL;
    u32 decompSize = 0;
    u32 roundedSize = 0;
    u32 ownsDestination = 0;
    bool decoded = false;
    if (readSize >= kHeaderSize) {
        const SzsHeader* header = reinterpret_cast<const SzsHeader*>(work);
        decompSize = getDecompSize(header);
        u32 decompAlignment = getDecompAlignment(header);
        u32 limit = arg->mLoadDataBufferSize;
        destination = arg->mLoadDataBuffer;
        if (decompSize > limit && limit != 0) decompSize = limit;
        roundedSize = (decompSize + kCacheBlock - 1) & ~(kCacheBlock - 1);

        bool haveDestination = true;
        if (destination == NULL) {
            /* the image buffer: aligned as the resource wants it, in the allocation direction of the resource */
            if (sDirectResourceTypeGuard == 0) {
                sDirectResourceTypeGuard = 1;
                sDirectResourceType.mTable = &sDirectResourceTypeTable;
            }
            s32 alignment;
            if (resource != NULL && resource->dispatchIsDerivedFrom(&sDirectResourceType) && resource != NULL) {
                s32 requested = arg->mLoadDataAlignment;
                if (requested != 0) {
                    alignment = requested < (s32)kCacheBlock ? (s32)kCacheBlock : requested;
                } else {
                    s32 wanted;
                    if (decompAlignment == 0) wanted = resource->dispatchLoadDataAlignment();
                    else wanted = decompAlignment;
                    s32 direction = arg->mInstanceAlignment < 0 ? -1 : 1;
                    if (wanted < (s32)kCacheBlock) wanted = kCacheBlock;
                    alignment = direction * wanted;
                }
            } else {
                s32 direction = arg->mInstanceAlignment < 0 ? -1 : 1;
                alignment = -(direction * (s32)kCacheBlock);
            }
            destination = static_cast<u8*>(allocateArrayFromHeap(roundedSize, heap, alignment));
            if (destination == NULL) haveDestination = false;
            else ownsDestination = 1;
        }

        if (haveDestination) {
            if (readSize < mWorkSize) {
                /* the whole file is in the work buffer */
                decoded = decomp(destination, roundedSize, reinterpret_cast<const SzsHeader*>(work), mWorkSize) >= 0;
            } else {
                gabi::FrameLocal<DecompContext> context(0x44);
                DecompContext::construct(context.get(), destination);
                context->mForceDestCount = decompSize;
                for (;;) {
                    s32 left = streamDecomp(context.get(), reinterpret_cast<const be<u8>*>(work), readSize);
                    if (left < 0) break;
                    if (left == 0) {
                        decoded = true;
                        break;
                    }
                    readSize = handle->read(work, mWorkSize);
                    if (readSize == 0) {
                        decoded = true;
                        break;
                    }
                }
            }
            if (!decoded && ownsDestination) releaseArray(destination);
        }
    }

    if (mWorkBuffer == NULL) releaseArray(work);
    if (!decoded) {
        closeHandle(handle.get());
        return NULL;
    }
    if (size != NULL) *size = decompSize;
    if (bufferSize != NULL) *bufferSize = roundedSize;
    if (ownsData != NULL) *ownsData = ownsDestination;
    closeHandle(handle.get());
    return destination;
}

/* 0275F5D4 */
void sead::szs_text::FixedText::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* inlined into both decompressor destructors: the base destructor */
static void destroyDecompressorBase(Decompressor* decompressor) {
    decompressor->mVtable = &sDecompressorVtable;
    decompressor->mDisposer.mVtable = &sDecompressorDisposerVtable;
    if (sResourceMgr != NULL) sResourceMgr->unregisterDecompressor(decompressor);
    destroyDisposer(&decompressor->mDisposer, 0);
}

/* 0275F5E8 */
void sead::SZSDecompressorBase::destroy(u32 flags) {
    if (this == NULL) return;
    destroyDecompressorBase(this);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275F66C */
void sead::SZSDecompressor::destroy(u32 flags) {
    if (this == NULL) return;
    destroyDecompressorBase(this);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275F6F0 */
void sead::szs_text::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275F704 */
void sead::szs_text::Text::assureTerminationImpl() {}

/* 0275F708 */
void sead::szs_text::TextBuffer::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275F71C */
void sead::szs_text::TextBuffer::assureTerminationImpl() {
    be<u8>* text = mText;
    s32 capacity = mCapacity;
    text[capacity - 1] = 0;
}

/* 0275F734 */
void sead::szs_text::ExtensionText::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275F75C */
void sead::szs_text::FileHandle::destroy(u32 flags) {
    if (this == NULL) return;
    FileDevice* device = mSourceDevice;
    mDisposer.mVtable = &sFileHandleVtable;
    if (device != NULL) device->tryClose(this);
    destroyDisposer(&mDisposer, 0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* WWHD glue
 data sNativeByteOrder 104A0CD4
 data sConvertBigU32 10147620 nocheck
 data sDecompressorVtable 1015E224 nocheck
 data sDecompressorDisposerVtable 1015E23C nocheck
 data sSZSDecompressorVtable 1015E2B8 nocheck
 data sSZSDecompressorDisposerVtable 1015E2D0 nocheck
 data sTextVtable 1015E20C nocheck
 data sFixedTextVtable 1015E264 nocheck
 data sExtensionTextVtable 1015E27C nocheck
 data sFileHandleVtable 1015E294 nocheck
 data sDirectResourceTypeGuard 101FD6E0
 data sDirectResourceType 101FD944
 data sDirectResourceTypeTable 1015E2A4 nocheck
 data sResourceMgr 101F8B68
 str 1015E2B4 "szs"
 ext 0273AD10 void* allocateObject(u32)
 ext 0273B0D4 void* allocateArrayFromHeap(u32, sead::RegistryHeap*, s32)
 ext 0273AF40 void releaseObject(void*)
 ext 0273AFC8 void releaseArray(void*)
 ext 02752A84 void constructDisposer(sead::Disposer*)
 ext 02752BEC void destroyDisposer(sead::Disposer*, u32)
 ext 02756140 sead::RegistryHeap* registryCurrentHeap(void*)
 ext C0009988 void* OSBlockMove(void*, const void*, u32, u32)
 ext 02740B24 sead::FileDevice* sead::FileDevice::tryOpen(sead::FileHandle*, sead::SafeText*, u32, u32)
 ext 02741CFC sead::FileDevice* sead::FileDeviceManager::tryOpen(sead::FileHandle*, sead::SafeText*, u32, u32)
 ext 0274146C u32 sead::FileHandle::read(void*, u32)
*/
