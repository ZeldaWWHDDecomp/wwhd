/* Sequence file block validation, reconstructed from WWHD. */
#include "nw/snd/SequenceFileReader.h"
using namespace nw::snd;
void* allocateSequenceReader(u32 size);
GroupBlockHeader* sequenceDataBlock(const GroupFileHeader* header);
GroupBlockHeader* sequenceLabelBlock(const GroupFileHeader* header);
u32 findSequenceLabel(const be<u8>* labels, const char* label, be<u32>* offset);
/* 028AE1B4 */
u32 SequenceFileReader::isSupported(const GroupFileHeader* header) {
    if (header->mMagic != 0x46534551u) return 0;
    return u32(static_cast<u32>(header->mVersion)-0x10000u)<0x10001u;
}
/* 028AE1F4 */
SequenceFileReader* SequenceFileReader::construct(SequenceFileReader* reader,const GroupFileHeader* header) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (reader==nullptr) reader=static_cast<SequenceFileReader*>(allocateSequenceReader(sizeof(SequenceFileReader)));
    if (reader==nullptr) return reader;
    reader->mLabels=nullptr;
    reader->mHeader=nullptr;
    reader->mData=nullptr;
    if (!isSupported(header)) return reader;
    reader->mHeader=header;
    GroupBlockHeader* data=sequenceDataBlock(header);
    if (data->mMagic!=0x44415441u) return reader;
    GroupBlockHeader* labels=sequenceLabelBlock(reader->mHeader);
    if (labels->mMagic!=0x4C41424Cu) return reader;
    reader->mData=gabi::at<const be<u8>>(gabi::ea(data)+8);
    reader->mLabels=gabi::at<const be<u8>>(gabi::ea(labels)+8);
    return reader;
}
/* 028AE2B0 */
const be<u8>* SequenceFileReader::data() const { return mData; }
/* 028AE2B8 */
u32 SequenceFileReader::findLabel(const char* label, be<u32>* offset) const {
    gabi::AutoFrame frame{gabi::current_function()};
    return findSequenceLabel(mLabels, label, offset);
}
/* WWHD glue
ext 0273AD10 void* allocateSequenceReader(u32)
ext 028B1F14 nw::snd::GroupBlockHeader* sequenceDataBlock(const nw::snd::GroupFileHeader*)
ext 028B1F70 nw::snd::GroupBlockHeader* sequenceLabelBlock(const nw::snd::GroupFileHeader*)
ext 028B1FF4 u32 findSequenceLabel(const be<u8>*,const char*,be<u32>*)
*/
