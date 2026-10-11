/** DVD loading and streaming Yaz0 decompression, qualified HD SDK subset.
 * Verified against the native functions. */
#include "JKRDvdRipper_adapters.h"
// HD: qualified two-entry SDK subset; private stream state retains native load/store order.
using DVDByte = be<u8>;
using DVDWord = be<u32>;
static JKRDvdStreamState l_streamState;
static DVDByte l_isInitMutex;
static DVDWord l_szpBufferSize;
static DVDByte l_errorRetry;
static u8 l_decompMutex;
static u8 l_ripperFile;
static u8 l_unsupportedSubArchive;
static u8 l_unsupportedOffset;
static u8 l_decompFile;
static u8 l_bufferAssertion;
static u8 l_referenceAssertion;
static u8 l_refillFile;
static u8 l_refillAssertion;

static bool readRetry(JKRDvdFile *file, void *destination, u32 size, u32 offset) {
    s32 result = DVDRipper_DVDReadPrio(&file->mFileInfo, destination, size, offset, 2);
    while (result < 0) {
        if (result == -3 || !u8(l_errorRetry))
            return false;
        DVDRipper_VIWaitForRetrace();
        result = DVDRipper_DVDReadPrio(&file->mFileInfo, destination, size, offset, 2);
    }
    return true;
}
static bool streamRead(u32 destination, u32 size) {
    s32 result = DVDRipper_DVDReadPrio(&l_streamState.srcFile->mFileInfo, gabi::at<void>(destination), size,
                                       l_streamState.srcOffset, 2);
    while (result < 0) {
        if (result == -3 || !u8(l_errorRetry))
            return false;
        DVDRipper_VIWaitForRetrace();
        result = DVDRipper_DVDReadPrio(&l_streamState.srcFile->mFileInfo, gabi::at<void>(destination), size,
                                       l_streamState.srcOffset, 2);
    }
    return true;
}
static u32 detect(void *header) {
    // HD: ASR is treated as an uncompressed resource by this adapter.
    u32 compression = DVDRipper_checkCompressed(header);
    return compression == COMPRESSION_ASR ? COMPRESSION_NONE : compression;
}
// HD: cursors use unsigned guest words to preserve 32-bit wrap and address comparisons.
/* 027EAE70 */
static s32 JKRDecompressFromDVD(JKRDvdFile *file, void *destination, u32 fileSize, u32 maxDestination,
                                u32 fileOffset, u32 sourceOffset, be<u32> *outputSize) {
    u32 dst = gabi::ea(destination);
    u32 sizeOut = gabi::ea(outputSize);
    u32 interruptState = DVDRipper_disableInterrupts();
    if (!u8(l_isInitMutex)) {
        DVDRipper_initMutex(&l_decompMutex);
        l_isInitMutex = 1;
    }
    DVDRipper_restoreInterrupts(interruptState);
    DVDRipper_lockMutex(&l_decompMutex);
    u32 bufferSize = u32(l_szpBufferSize);
    u32 buffer = gabi::ea(DVDRipper_allocFromSysHeap(bufferSize, -32));
    l_streamState.szpBuf = gabi::at<be<u8>>(buffer);
    if (!buffer) {
        DVDRipper_panic(&l_decompFile, 0x3AB, &l_bufferAssertion);
        buffer = gabi::ea(l_streamState.szpBuf.get());
    }
    l_streamState.szpEnd = gabi::at<be<u8>>(buffer + bufferSize);
    if (fileOffset) {
        u32 ref = gabi::ea(DVDRipper_allocFromSysHeap(0x1120u, -4));
        l_streamState.refBuf = gabi::at<be<u8>>(ref);
        if (!ref) {
            DVDRipper_panic(&l_decompFile, 0x3B4, &l_referenceAssertion);
            ref = gabi::ea(l_streamState.refBuf.get());
        }
        l_streamState.srcFile = file;
        l_streamState.refCurrent = gabi::at<be<u8>>(ref);
        l_streamState.maxDest = maxDestination;
        l_streamState.refEnd = gabi::at<be<u8>>(ref + 0x1120);
        l_streamState.readCount = 0;
        l_streamState.fileOffset = fileOffset;
        l_streamState.transLeft = fileSize - sourceOffset;
        l_streamState.srcOffset = sourceOffset;
    } else {
        l_streamState.refBuf = gabi::at<be<u8>>(0);
        l_streamState.srcFile = file;
        l_streamState.maxDest = maxDestination;
        l_streamState.readCount = 0;
        l_streamState.fileOffset = fileOffset;
        l_streamState.transLeft = fileSize - sourceOffset;
        l_streamState.srcOffset = sourceOffset;
    }
    if (!sizeOut)
        sizeOut = gabi::ea(&l_streamState.decompressedSize);
    l_streamState.pDecompressedSize = gabi::at<be<u32>>(sizeOut);
    *l_streamState.pDecompressedSize.get() = 0;
    s32 status = -1;
    do {
        u32 input = gabi::ea(l_streamState.szpBuf.get()), endBuffer = gabi::ea(l_streamState.szpEnd.get());
        l_streamState.srcLimit = gabi::at<be<u8>>(endBuffer - 25);
        u32 initialSize = endBuffer - input;
        if (initialSize > u32(l_streamState.transLeft))
            initialSize = u32(l_streamState.transLeft);
        if (!streamRead(input, initialSize))
            break;
        DVDRipper_invalidateRange(gabi::at<void>(input), initialSize);
        {
            u32 next = u32(l_streamState.srcOffset) + initialSize;
            u32 left = u32(l_streamState.transLeft) - initialSize;
            l_streamState.srcOffset = next;
            l_streamState.transLeft = left;
        }
        if (!input)
            break;
        if (u8(*gabi::at<be<u8>>(input)) != 'Y' || u8(*gabi::at<be<u8>>(input + 1)) != 'a' ||
            u8(*gabi::at<be<u8>>(input + 2)) != 'z' || u8(*gabi::at<be<u8>>(input + 3)) != '0')
            break;
        {
            u32 out = dst,
                end = dst + (u32(gabi::at<SYaz0Header>(input)->length) - u32(l_streamState.fileOffset));
            u32 limit = dst + u32(l_streamState.maxDest);
            if (end > limit)
                end = limit;
            u32 written = 0, code = 0, bits = 0;
            input += 16;
            bool decodeFailed = false;
            // Native do-loop consumes at least one symbol, even when the requested output limit is zero.
            for (;;) {
                if (!bits) {
                    if (input > gabi::ea(l_streamState.srcLimit.get()) && u32(l_streamState.transLeft)) {
                        u32 remainder = gabi::ea(l_streamState.szpEnd.get()) - input;
                        u32 relocated = gabi::ea(l_streamState.szpBuf.get());
                        if (remainder & 31)
                            relocated += 32 - (remainder & 31);
                        DVDRipper_move(gabi::at<void>(relocated), gabi::at<void>(input), remainder);
                        u32 nextRead = relocated + remainder;
                        u32 count = gabi::ea(l_streamState.szpEnd.get()) - nextRead;
                        if (count > u32(l_streamState.transLeft))
                            count = u32(l_streamState.transLeft);
                        if (!count)
                            DVDRipper_panic(&l_refillFile, 0x4D6, &l_refillAssertion);
                        if (!streamRead(nextRead, count)) {
                            // Native refill-failure cleanup explicitly supplies r4=0 to the first free call.
                            DVDRipper_freeRefillFailure(l_streamState.szpBuf.get(), 0);
                            u32 ref = gabi::ea(l_streamState.refBuf.get());
                            if (ref)
                                DVDRipper_free(gabi::at<void>(ref));
                            DVDRipper_storeRangeNoSync(gabi::at<void>(dst),
                                                       *l_streamState.pDecompressedSize.get());
                            DVDRipper_unlockMutex(&l_decompMutex);
                            return -1;
                        }
                        DVDRipper_invalidateRange(gabi::at<void>(nextRead), count);
                        u32 next = u32(l_streamState.srcOffset) + count;
                        u32 left = u32(l_streamState.transLeft) - count;
                        l_streamState.srcOffset = next;
                        l_streamState.transLeft = left;
                        if (!left)
                            l_streamState.srcLimit = gabi::at<be<u8>>(nextRead + count);
                        input = relocated;
                        if (!input) {
                            decodeFailed = true;
                            break;
                        }
                    }
                    code = u8(*gabi::at<be<u8>>(input++));
                    bits = 8;
                }
                u32 skip = u32(l_streamState.fileOffset);
                if (code & 0x80) {
                    if (skip) {
                        u32 consumed = u32(l_streamState.readCount);
                        if (consumed >= skip) {
                            *gabi::at<be<u8>>(out++) = *gabi::at<be<u8>>(input);
                            ++written;
                            if (out == end)
                                break;
                        }
                        u32 cursor = gabi::ea(l_streamState.refCurrent.get());
                        *gabi::at<be<u8>>(cursor) = *gabi::at<be<u8>>(input);
                        cursor = gabi::ea(l_streamState.refCurrent.get()) + 1;
                        if (cursor == gabi::ea(l_streamState.refEnd.get()))
                            cursor = gabi::ea(l_streamState.refBuf.get());
                        u32 consumedNext = u32(l_streamState.readCount) + 1;
                        ++input;
                        l_streamState.refCurrent = gabi::at<be<u8>>(cursor);
                        l_streamState.readCount = consumedNext;
                    } else {
                        *gabi::at<be<u8>>(out++) = *gabi::at<be<u8>>(input++);
                        ++written;
                        if (out == end)
                            break;
                        l_streamState.readCount = u32(l_streamState.readCount) + 1;
                    }
                } else {
                    u32 first = u8(*gabi::at<be<u8>>(input)), second = u8(*gabi::at<be<u8>>(input + 1));
                    input += 2;
                    u32 distance = ((first & 15) << 8) | second;
                    u32 count = first >> 4;
                    u32 copy;
                    if (skip) {
                        copy = gabi::ea(l_streamState.refCurrent.get()) - distance - 1;
                        u32 start = gabi::ea(l_streamState.refBuf.get());
                        if (copy < start)
                            copy += gabi::ea(l_streamState.refEnd.get()) - start;
                    } else
                        copy = out - distance - 1;
                    if (!count)
                        count = u8(*gabi::at<be<u8>>(input++)) + 18;
                    else
                        count += 2;
                    for (u32 i = 0; i < count; ++i) {
                        if (skip) {
                            if (u32(l_streamState.readCount) >= u32(l_streamState.fileOffset)) {
                                *gabi::at<be<u8>>(out++) = *gabi::at<be<u8>>(copy);
                                ++written;
                                if (out == end)
                                    break;
                            }
                            u32 cursor = gabi::ea(l_streamState.refCurrent.get());
                            *gabi::at<be<u8>>(cursor) = *gabi::at<be<u8>>(copy);
                            cursor = gabi::ea(l_streamState.refCurrent.get()) + 1;
                            u32 refEnd = gabi::ea(l_streamState.refEnd.get());
                            if (cursor == refEnd)
                                cursor = gabi::ea(l_streamState.refBuf.get());
                            ++copy;
                            l_streamState.refCurrent = gabi::at<be<u8>>(cursor);
                            if (copy == refEnd)
                                copy = gabi::ea(l_streamState.refBuf.get());
                            l_streamState.readCount = u32(l_streamState.readCount) + 1;
                        } else {
                            *gabi::at<be<u8>>(out++) = *gabi::at<be<u8>>(copy);
                            ++written;
                            if (out == end)
                                break;
                            l_streamState.readCount = u32(l_streamState.readCount) + 1;
                            ++copy;
                        }
                    }
                }
                --bits;
                code <<= 1;
                if (out >= end)
                    break;
            }
            if (decodeFailed)
                break;
            *l_streamState.pDecompressedSize.get() = written;
            status = 0;
        }
    } while (false);
    DVDRipper_free(l_streamState.szpBuf.get());
    {
        u32 ref = gabi::ea(l_streamState.refBuf.get());
        if (ref)
            DVDRipper_free(gabi::at<void>(ref));
    }
    DVDRipper_storeRangeNoSync(gabi::at<void>(dst), *l_streamState.pDecompressedSize.get());
    DVDRipper_unlockMutex(&l_decompMutex);
    return status;
}

// HD: free ABI adapter for the static SDK entry until autoglue supports static methods.
/* 027EB5F4 */
static void *JKRDvdRipper_loadToMainRAM(JKRDvdFile *dvdFile, void *destination, u32 expandSwitch,
                                        u32 dstLength, void *heap, u32 allocDirection, u32 offset,
                                        be<u32> *pCompression) {
    // HD: the ninth integer argument remains in the incoming stack slot.
    be<u32> *returnSize = gabi::at<be<u32>>(u32(*gabi::at<be<u32>>(gabi::cpu->r[1] + 8)));
    JKRDvdFileVtable *table = dvdFile->__vtbl;
    u32 fileSizeAligned = (gabi::call_ptr<u32>(table->mGetFileSize, dvdFile) + 31) & ~31u;
    u32 compression = COMPRESSION_NONE;
    u32 expandSize = 0;
    void *mem = nullptr;
    bool hasAllocated = false;
    // HD: retain the 128-byte slot and 64-byte alignment used by the native 32-byte header read.
    gabi::Local<u8[128]> buffer;
    void *header = gabi::at<void>((gabi::ea(buffer.get()) + 63) & ~63u);
    if (expandSwitch == EXPAND_SWITCH_UNKNOWN1) {
        if (!readRetry(dvdFile, header, 32, 0))
            return nullptr;
        DVDRipper_invalidateRange(header, 32);
        compression = detect(header);
        // HD: the expanded-size header is read as four bytes, preserving the native access order.
        be<u8> *bytes = static_cast<be<u8> *>(header);
        expandSize = (u32(bytes[4]) << 24) | (u32(bytes[5]) << 16) | (u32(bytes[6]) << 8) | u32(bytes[7]);
    }
    if (pCompression)
        *pCompression = compression;
    if (expandSwitch == EXPAND_SWITCH_UNKNOWN1 && compression) {
        if (dstLength && expandSize > dstLength)
            expandSize = dstLength;
        if (!destination) {
            destination = DVDRipper_allocFromHeap(heap, expandSize,
                                                  allocDirection == ALLOC_DIRECTION_FORWARD ? 64 : -64);
            hasAllocated = true;
        }
        if (!destination)
            return nullptr;
        if (compression == COMPRESSION_YAY0) {
            mem = DVDRipper_allocFromHeap(heap, fileSizeAligned, 64);
            if (!mem && hasAllocated) {
                DVDRipper_free(destination);
                return nullptr;
            }
        }
    } else {
        if (!destination) {
            u32 size = fileSizeAligned - offset;
            // HD: cap the uncompressed allocation to the requested destination length.
            if (dstLength && size > dstLength)
                size = dstLength;
            destination =
                DVDRipper_allocFromHeap(heap, size, allocDirection == ALLOC_DIRECTION_FORWARD ? 64 : -64);
            hasAllocated = true;
        }
        if (!destination)
            return nullptr;
    }
    if (compression == COMPRESSION_NONE) {
        u32 compression2 = COMPRESSION_NONE;
        if (offset) {
            if (!readRetry(dvdFile, header, 32, offset)) {
                if (hasAllocated)
                    DVDRipper_free(destination);
                return nullptr;
            }
            DVDRipper_invalidateRange(header, 32);
            compression2 = detect(header);
        }
        if (!compression2 || expandSwitch == EXPAND_SWITCH_UNKNOWN2 ||
            expandSwitch == EXPAND_SWITCH_UNKNOWN0) {
            u32 size = fileSizeAligned - offset;
            if (dstLength && dstLength < size)
                size = dstLength;
            if (!readRetry(dvdFile, destination, size, offset)) {
                if (hasAllocated)
                    DVDRipper_free(destination);
                return nullptr;
            }
            if (returnSize)
                *returnSize = size;
            return destination;
        }
        if (compression2 == COMPRESSION_YAZ0) {
            JKRDecompressFromDVD(dvdFile, destination, fileSizeAligned, dstLength, 0, offset, returnSize);
            return destination;
        }
        DVDRipper_panic(&l_ripperFile, 0x15D, &l_unsupportedSubArchive);
        return destination;
    }
    if (compression == COMPRESSION_YAY0) {
        if (offset)
            DVDRipper_panic(&l_ripperFile, 0x167, &l_unsupportedOffset);
        if (!readRetry(dvdFile, mem, fileSizeAligned, 0)) {
            if (hasAllocated)
                DVDRipper_free(destination);
            DVDRipper_free(mem);
            return nullptr;
        }
        DVDRipper_invalidateRange(mem, fileSizeAligned);
        DVDRipper_decompress(mem, destination, expandSize, offset);
        DVDRipper_free(mem);
        if (returnSize)
            *returnSize = expandSize;
        return destination;
    }
    if (compression == COMPRESSION_YAZ0) {
        if (JKRDecompressFromDVD(dvdFile, destination, fileSizeAligned, expandSize, offset, 0, returnSize)) {
            if (hasAllocated)
                DVDRipper_free(destination);
            destination = nullptr;
        }
        return destination;
    }
    if (hasAllocated)
        DVDRipper_free(destination);
    return nullptr;
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data l_streamState 104A2380
data l_isInitMutex 101F9730
data l_szpBufferSize 101F974C
data l_errorRetry 101F9750
data l_decompMutex 104A23C8
data l_ripperFile 1016DBDC
data l_unsupportedSubArchive 1016DBB8
data l_unsupportedOffset 1016DBF0
data l_decompFile 1016DBA4
data l_bufferAssertion 1016DB8C
data l_referenceAssertion 1016DB98
data l_refillFile 1016DB78
data l_refillAssertion 1016DB68
ext 0284B6EC s32 DVDRipper_DVDReadPrio(void*, void*, u32, u32, u32)
ext 0284B990 void DVDRipper_VIWaitForRetrace()
ext 028428FC u32 DVDRipper_checkCompressed(void*)
ext 027EC0D8 void* DVDRipper_allocFromHeap(void*, u32, s32)
ext 027EC0B4 void* DVDRipper_allocFromSysHeap(u32, s32)
ext 027EC0B0 void DVDRipper_free(void*)
ext 027EC0B0 void DVDRipper_freeRefillFailure(void*, u32)
ext C00088A8 void DVDRipper_invalidateRange(void*, u32)
ext C00088B8 void DVDRipper_storeRangeNoSync(void*, u32)
ext 02842B38 void DVDRipper_decompress(void*, void*, u32, u32)
ext 0273AA24 void DVDRipper_panic(void*, u32, void*)
ext C0009A60 u32 DVDRipper_disableInterrupts()
ext C0009F30 void DVDRipper_restoreInterrupts(u32)
ext C0009D68 void DVDRipper_initMutex(void*)
ext C0009E40 void DVDRipper_lockMutex(void*)
ext C000A1C8 void DVDRipper_unlockMutex(void*)
ext C000A848 void DVDRipper_move(void*, void*, u32)
*/
