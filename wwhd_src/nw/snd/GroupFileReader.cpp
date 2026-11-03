/* Group resource headers and indexed file references, reconstructed from WWHD. */
#include "nw/snd/GroupFileReader.h"
using namespace nw::snd;
void* allocateGroupReader(u32 size);
void releaseGroupReader(void* storage);
GroupBlockHeader* groupInfoBlock(const GroupFileHeader* header);
GroupBlockHeader* groupFileBlock(const GroupFileHeader* header);
GroupBlockHeader* groupExtraBlock(const GroupFileHeader* header);

/* inlined into GroupFileReader::readFile and GroupFileReader::readExtra */
static GroupReference* groupReference(GroupReferenceTable* table,u32 index) {
    return gabi::at<GroupReference>(gabi::ea(table)+4+index*8);
}

/* 028AA7A0 */
void GroupLoadStatus::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this == nullptr) return;
    mOpen=0;
    if (flags & 1) releaseGroupReader(this);
}
/* 028AA7BC */
u32 GroupFileReader::isSupported(const GroupFileHeader* header) {
    if (header->mMagic != 0x46475250u) return 0;
    return static_cast<u32>(header->mVersion)==0x10000u;
}
/* 028AA7F4 */
GroupFileReader* GroupFileReader::construct(GroupFileReader* reader,const GroupFileHeader* header) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (reader == nullptr) reader=static_cast<GroupFileReader*>(allocateGroupReader(sizeof(GroupFileReader)));
    if (reader == nullptr) return reader;
    reader->mExtraInfo=nullptr;
    reader->mInfo=nullptr;
    reader->mFileData=nullptr;
    if (!isSupported(header)) return reader;
    GroupBlockHeader* info=groupInfoBlock(header);
    GroupBlockHeader* file=groupFileBlock(header);
    GroupBlockHeader* extra=groupExtraBlock(header);
    if (info == nullptr || file == nullptr) return reader;
    if (info->mMagic != 0x494E464Fu || file->mMagic != 0x46494C45u) return reader;
    if (extra != nullptr) {
        if (extra->mMagic != 0x494E4658u) return reader;
        reader->mExtraInfo=gabi::at<GroupReferenceTable>(gabi::ea(extra)+8);
    }
    reader->mInfo=gabi::at<GroupReferenceTable>(gabi::ea(info)+8);
    reader->mFileData=gabi::at<const be<u8>>(gabi::ea(file)+8);
    return reader;
}
/* 028AA8FC */
u32 GroupFileReader::readFile(GroupFileLocation* output,u32 index) const {
    GroupReferenceTable* table=mInfo;
    if (table == nullptr || index >= static_cast<u32>(table->mCount)) return 0;
    s32 relative=groupReference(table,index)->mOffset;
    GroupFileEntry* entry=gabi::at<GroupFileEntry>(gabi::ea(table)+u32(relative));
    if (entry == nullptr) return 0;
    output->mFileId=entry->mFileId;
    s32 fileOffset=entry->mFileOffset;
    const be<u8>* data=mFileData;
    output->mAddress=fileOffset == -1 ? nullptr : gabi::at<const be<u8>>(gabi::ea(data)+u32(fileOffset));
    return 1;
}
/* 028AA95C */
u32 GroupFileReader::extraCount() const {
    GroupReferenceTable* table=mExtraInfo;
    return table == nullptr ? 0 : static_cast<u32>(table->mCount);
}
/* 028AA978 */
u32 GroupFileReader::readExtra(GroupExtraEntry* output,u32 index) const {
    GroupReferenceTable* table=mExtraInfo;
    if (table == nullptr || index >= static_cast<u32>(table->mCount)) return 0;
    s32 relative=groupReference(table,index)->mOffset;
    GroupExtraEntry* entry=gabi::at<GroupExtraEntry>(gabi::ea(table)+u32(relative));
    if (entry == nullptr) return 0;
    output->mFileId=entry->mFileId;
    output->mFlags=entry->mFlags;
    return 1;
}
/* WWHD glue
ext 0273AD10 void* allocateGroupReader(u32)
ext 0273AF40 void releaseGroupReader(void*)
ext 028B1D44 nw::snd::GroupBlockHeader* groupInfoBlock(const nw::snd::GroupFileHeader*)
ext 028B1DA0 nw::snd::GroupBlockHeader* groupFileBlock(const nw::snd::GroupFileHeader*)
ext 028B1DFC nw::snd::GroupBlockHeader* groupExtraBlock(const nw::snd::GroupFileHeader*)
*/
