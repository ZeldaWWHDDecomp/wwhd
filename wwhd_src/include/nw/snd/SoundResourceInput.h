/* Stream-backed resource reads and aligned transfer buffers from WWHD instructions. */
#pragma once
#include "nw/snd/SoundResourceDirectories.h"
namespace nw { namespace snd {
struct ResourceStreamVtable_l {
    u8 mPrefix[0x1C]; gabi::gfn mClose;
    be<u32> mReserved20; gabi::gfn mRead;
    u8 mReserved28[0x44]; gabi::gfn mCanRead;
    u8 mReserved70[0x1C]; gabi::gfn mSize;
    be<u32> mReserved90; gabi::gfn mSeek;
    u8 mReserved98[0x24]; gabi::gfn mTell;
};
struct ResourceStream_l { u8 mPrefix[0xC]; gptr<const ResourceStreamVtable_l> mVtable; };
struct SoundResourceInput {
    gptr<ResourceStream_l> mStream;
    be<u32> mSeekOffset,mRegionOffset;
    be<u16> mRegionRecordSize; u8 mReservedE[2];
    u32 readMetadata(StreamMetadataReader* reader,void* storage,u32 capacity);
    u32 readSeekHistories(be<s16>* first,be<s16>* second,u32 blockIndex,u32 channels);
    u32 readRegion(be<u32>* output,u32 regionIndex);
};
WWHD_OFFSET(ResourceStreamVtable_l,mRead,0x24);
WWHD_OFFSET(ResourceStreamVtable_l,mSeek,0x94);
WWHD_SIZE(SoundResourceInput,16);
}}
