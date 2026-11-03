// Bitmap-backed sound storage; reconstructed from the original game's instructions.
#include "nw/snd/AudioBlockPool.h"
using namespace nw::snd;
void clearAudioPoolBytes(void*, u32, u32);

/* 02899114 */
void AudioBlockPool::initialize(void* memory, u32 bytes, u32 count) {
    if (count == 0) return;
    u32 stride = bytes / count;
    allocatedCount = 0;
    storageBytes = bytes;
    storage = static_cast<u8*>(memory);
    blockCount = count;
    blockBytes = stride & ~31u;
    clearAudioPoolBytes(occupied, 0, 4);
}

/* 02899150 */
void AudioBlockPool::reset() {
    storage = nullptr;
    blockCount = 0;
    storageBytes = 0;
    blockBytes = 0;
}

/* 02899168 */
void* AudioBlockPool::allocate() {
    s32 count = blockCount;
    if (static_cast<s32>(allocatedCount) >= count) return nullptr;
    s32 bytes = static_cast<s32>((static_cast<u32>(count) + 7) & ~7u) / 8;
    if (bytes <= 0) return nullptr;
    for (s32 byte = 0; byte < bytes; ++byte) {
        u8 mask = occupied[byte];
        if (mask == 255) continue;
        for (u32 bit = 0; bit < 8; ++bit) {
            u32 flag = 1u << bit;
            if ((mask & flag) != 0) continue;
            occupied[byte] = mask | flag;
            u32 stride = blockBytes;
            u32 allocated = allocatedCount;
            allocatedCount = allocated + 1;
            return storage.get() + stride * (static_cast<u32>(byte) * 8 + bit);
        }
    }
    return nullptr;
}

/* 02899228 */
void AudioBlockPool::release(void* memory) {
    u32 offset = gabi::ea(memory) - gabi::ea(storage.get());
    u32 stride = blockBytes;
    u32 index = stride ? offset / stride : 0;
    u32 flag = 1u << (index & 7);
    u32 byte = index >> 3;
    occupied[byte] = static_cast<u8>(occupied[byte]) & ~flag;
    allocatedCount = static_cast<u32>(allocatedCount) - 1;
}

/* WWHD glue
ext C0009990 void clearAudioPoolBytes(void*, u32, u32)
*/
