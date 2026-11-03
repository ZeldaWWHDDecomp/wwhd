/* An archive backed by a caller-owned sound-memory buffer; reconstructed from WWHD.
 * Archive facade and reader: the canonical nw::snd SoundArchive layouts (SoundArchive.h). */
#include "nw/snd/MemorySoundArchive.h"
using namespace nw::snd;
struct MemoryArchiveVtable { be<u32> entry; };
static MemoryArchiveVtable sMemoryArchiveVtable;
/* 0288A27C */
MemorySoundArchive* MemorySoundArchive::create(MemorySoundArchive* archive) {
    if (!archive) {
        archive = (MemorySoundArchive*)runtimeAllocate(sizeof(MemorySoundArchive));
        if (!archive) return archive;
    }
    SoundArchive::create(archive);
    archive->buffer = nullptr;
    archive->vtable = &sMemoryArchiveVtable;
    SoundArchiveReader::create(&archive->reader);
    return archive;
}
/* 0288A2E0 */
void MemorySoundArchive::destroy(u32 flags) {
    if (!gabi::ea(this)) return;
    SoundArchive::destroy(this,0);
    if (flags & 1) runtimeDelete(this);
}
/* 0288A334 */
u32 MemorySoundArchive::open(const u8* data) {
    reader.readHeader(reinterpret_cast<const SoundArchiveHeader*>(data));
    setReader(&reader);
    u32 offset = reader.header.infoBlockOffset();
    reader.setInfoBlock(gabi::at<void>(gabi::ea(data) + offset));
    u32 stringOffset = reader.header.stringBlockOffset();
    u32 stringSize = reader.header.stringBlockSize();
    if (stringOffset != 0xFFFFFFFFu && stringSize != 0xFFFFFFFFu) {
        reader.setStringBlock(gabi::at<void>(gabi::ea(data) + stringOffset));
    }
    buffer = data;
    return 1;
}
/* 0288A3D0 */
void MemorySoundArchive::close() {
    buffer = nullptr;
    reader.clear();
    clearReader();
}
/* 0288A410 */
u8* MemorySoundArchive::itemData(u32 id) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<ArchiveFileSummary> info(0x30);
    info->size = 0xFFFFFFFFu;
    info->offset = 0xFFFFFFFFu;
    info->externalPath = nullptr;
    if (readFileInfo(id,info.get()) && info->offset != 0xFFFFFFFFu) {
        u32 base = reader.header.fileBlockOffset();
        const u8* data = buffer;
        s32 offset = info->offset;
        return gabi::at<u8>(gabi::ea(data) + (u32)offset + base);
    }
    u32 groups = groupCount();
    for (u32 group = 0; group < groups; ++group) {
        gabi::FrameLocal<ArchiveGroupSummary> groupInfo(0x10);
        groupInfo->fileId = 0xFFFFFFFFu;
        groupInfo->size = 0;
        if (!readGroupInfo(group | (ArchiveItem_Group << 24),groupInfo.get())) continue;
        u32 fileId = groupInfo->fileId;
        gabi::FrameLocal<ArchiveFileSummary> groupFile(0x18);
        groupFile->size = 0xFFFFFFFFu;
        groupFile->externalPath = nullptr;
        groupFile->offset = 0xFFFFFFFFu;
        if (!readFileInfo(fileId,groupFile.get())) continue;
        u32 base = reader.header.fileBlockOffset();
        s32 offset = groupFile->offset;
        const u8* data = buffer;
        gabi::FrameLocal<ArchiveResourceReader> resource(0x24);
        memoryArchiveResourceOpen(resource.get(),gabi::at<const u8>(gabi::ea(data) + (u32)offset + base));
        u32 entries = resource->header->count;
        for (u32 index = 0; index < entries; ++index) {
            gabi::FrameLocal<ArchiveResourceEntry> entry(8);
            if (memoryArchiveResourceEntry(resource.get(),entry.get(),index) && entry->id == id) {
                u8* result = entry->data;
                if (result) return result;
            }
        }
    }
    return nullptr;
}
/* 0288A5EC */
SoundMemoryStream* MemorySoundArchive::makeStream(SoundMemoryStream* stream,u32 storageBytes,u32 offset,u32 length) {
    const u8* data = buffer;
    if (!data || storageBytes < sizeof(SoundMemoryStream)) return nullptr;
    if (stream) stream = SoundMemoryStream::create(stream,gabi::at<const u8>(gabi::ea(data) + offset),length);
    return stream;
}
/* WWHD glue
data sMemoryArchiveVtable 10181E8C
ext 0288E878 nw::snd::SoundArchive* nw::snd::SoundArchive::create(nw::snd::SoundArchive*)
ext 0288E8D0 void nw::snd::SoundArchive::destroy(nw::snd::SoundArchive*,u32)
ext 02890208 nw::snd::SoundArchiveReader* nw::snd::SoundArchiveReader::create(nw::snd::SoundArchiveReader*)
ext 0289028C u32 nw::snd::SoundArchiveReader::readHeader(const nw::snd::SoundArchiveHeader*)
ext 0288F304 void nw::snd::SoundArchive::setReader(nw::snd::SoundArchiveReader*)
ext 0288F458 u32 nw::snd::SoundArchiveHeader::infoBlockOffset()
ext 02890338 void nw::snd::SoundArchiveReader::setInfoBlock(void*)
ext 0288F430 u32 nw::snd::SoundArchiveHeader::stringBlockOffset()
ext 0288F3E0 u32 nw::snd::SoundArchiveHeader::stringBlockSize()
ext 0289032C void nw::snd::SoundArchiveReader::setStringBlock(void*)
ext 028902E0 void nw::snd::SoundArchiveReader::clear()
ext 0288F33C void nw::snd::SoundArchive::clearReader()
ext 0288EEFC u32 nw::snd::SoundArchive::readFileInfo(u32,nw::snd::ArchiveFileSummary*)
ext 0288E8F4 u32 nw::snd::SoundArchive::groupCount()
ext 0288EE74 u32 nw::snd::SoundArchive::readGroupInfo(u32,nw::snd::ArchiveGroupSummary*)
ext 0288F480 u32 nw::snd::SoundArchiveHeader::fileBlockOffset()
ext 028AA7F4 void memoryArchiveResourceOpen(nw::snd::ArchiveResourceReader*,const u8*)
ext 028AA8FC u32 memoryArchiveResourceEntry(nw::snd::ArchiveResourceReader*,nw::snd::ArchiveResourceEntry*,u32)
ext 0273AF40 void runtimeDelete(void*)
*/
