/* Resource read order, short-read gates and aligned stack transfers. */
#include "nw/snd/SoundResourceInput.h"
using namespace nw::snd;
struct ResourceHeaderScratch_l { StreamFileDirectory header; be<u8> tail[64]; };
WWHD_SIZE(ResourceHeaderScratch_l,96);
static s32 readResourceStream(ResourceStream_l* stream,void* output,u32 size) {
    return gabi::call<s32>(stream->mVtable->mRead.get(),stream,output,size);
}
static void seekResourceStream(ResourceStream_l* stream,u32 offset,u32 origin) {
    gabi::call<void>(stream->mVtable->mSeek.get(),stream,offset,origin);
}
/* 028B0574 */
u32 SoundResourceInput::readMetadata(StreamMetadataReader* reader,void* storage,u32 capacity) {
    gabi::AutoFrame frame{gabi::current_function()};
    seekResourceStream(mStream,0,0);
    u32 offset=((frame.ctx.sp+0x47)&~63u)-frame.ctx.sp;
    gabi::FrameLocal<ResourceHeaderScratch_l> scratch(offset);
    if (readResourceStream(mStream,scratch.get(),96)!=96) return 0;
    const StreamFileDirectory* header=&scratch->header;
    if (!reader->accepts(header)) return 0;
    u32 start=header->infoOffset();
    u32 size=header->infoSize();
    u32 total=start+size;
    if (total>capacity) return 0;
    seekResourceStream(mStream,0,0);
    if (static_cast<u32>(readResourceStream(mStream,storage,total))!=total) return 0;
    reader->bind(static_cast<const StreamFileDirectory*>(storage));
    header=reader->mHeader;
    if (header && header->hasSeekBlock()) {
        header=reader->mHeader;
        mSeekOffset=header->seekOffset();
    } else mSeekOffset=0;
    header=reader->mHeader;
    if (header && header->hasRegionBlock()) {
        header=reader->mHeader;
        u32 region=header->regionOffset()+8;
        const StreamInfoReferences* references=reader->mInfo;
        const void* info=references->streamInfo();
        mRegionOffset=region+gabi::load<u32>(gabi::ea(info)+0x40);
    } else mRegionOffset=0;
    const StreamInfoReferences* references=reader->mInfo;
    const void* info=references->streamInfo();
    mRegionRecordSize=gabi::load<u16>(gabi::ea(info)+0x38);
    return 1;
}
/* 028B072C */
u32 SoundResourceInput::readSeekHistories(be<s16>* first,be<s16>* second,u32 blockIndex,u32 channels) {
    gabi::AutoFrame frame{gabi::current_function()};
    seekResourceStream(mStream,mSeekOffset+8+blockIndex*channels*4,0);
    if (channels*4>32) return 0;
    u32 offset=((frame.ctx.sp+0x47)&~63u)-frame.ctx.sp;
    gabi::FrameLocal<be<s16>[32]> scratch(offset);
    if (readResourceStream(mStream,scratch.get(),64)!=64) return 0;
    for (s32 i=0;i<static_cast<s32>(channels);++i) {
        first[i]=(*scratch)[i*2];
        second[i]=(*scratch)[i*2+1];
    }
    return 1;
}
/* 028B0830 */
u32 SoundResourceInput::readRegion(be<u32>* output,u32 regionIndex) {
    gabi::AutoFrame frame{gabi::current_function()};
    seekResourceStream(mStream,mRegionOffset+static_cast<u32>(mRegionRecordSize)*regionIndex,0);
    u32 offset=((frame.ctx.sp+0x47)&~63u)-frame.ctx.sp;
    gabi::FrameLocal<be<u32>[64]> scratch(offset);
    if (readResourceStream(mStream,scratch.get(),256)!=256) return 0;
    for (u32 i=0;i<64;++i) output[i]=(*scratch)[i];
    return 1;
}
/* WWHD glue
*/
