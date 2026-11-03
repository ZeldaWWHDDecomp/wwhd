/* Wave archive (FWAR) header blocks, file table and loaded-address table, reconstructed from WWHD. */
#include "nw/snd/WaveArchiveReader.h"
using namespace nw::snd;
void* allocateWaveArchiveReader(u32 size);

// The object `offset` bytes after `base`, with the file's 32-bit offset arithmetic.
template <class T> static T* atOffset(const void* base, u32 offset) {
    return gabi::at<T>(gabi::ea(base) + offset);
}

/* 0289F908 */
const WaveArchiveBlockEntry* WaveArchiveHeader::findBlock(u32 kind) const {
    for (u32 index = 0; index < 2; ++index)
        if (mBlocks[index].mKind == kind) return &mBlocks[index];
    return nullptr;
}

/* 0289F938 */
u32 WaveArchiveHeader::tableOffset() const {
    gabi::AutoFrame frame{0x0289F938u};
    return findBlock(WaveArchiveTableBlock)->mOffset;
}

/* 0289F960 */
const be<u8>* WaveArchiveHeader::tableAddress() const {
    gabi::AutoFrame frame{0x0289F960u};
    return atOffset<const be<u8>>(this, tableOffset());
}

/* 0289F988 */
u32 WaveArchiveHeader::dataOffset() const {
    gabi::AutoFrame frame{0x0289F988u};
    return findBlock(WaveArchiveDataBlock)->mOffset;
}

/* 0289F9B0 */
const be<u8>* WaveArchiveHeader::dataAddress() const {
    gabi::AutoFrame frame{0x0289F9B0u};
    return atOffset<const be<u8>>(this, dataOffset());
}

/* 0289F9D8 */
u32 WaveArchiveHeader::isSupported() const {
    return mMagic == WaveArchiveMagic && mVersion == WaveArchiveVersion;
}

/* 0289FA10 */
u32 WaveArchiveReader::hasAddressCache() const {
    gabi::AutoFrame frame{0x0289FA10u};
    if (!mValid) return 0;
    const WaveArchiveHeader* header = mHeader;
    const u32 offset = header->dataOffset();
    return *atOffset<const be<u32>>(header, offset) == WaveArchiveAddressMagic;
}

/* 0289FA7C */
void WaveArchiveReader::attach(const WaveArchiveHeader* header, u32 useCache) {
    gabi::AutoFrame frame{0x0289FA7Cu};
    if (!header || !header->isSupported()) return;
    mHeader = header;
    // The table block's 8-byte block header precedes the table.
    mTable = atOffset<const WaveArchiveTable>(header->tableAddress(), 8);
    mValid = 1;
    mCachedAddresses = nullptr;
    if (useCache && hasAddressCache()) mCachedAddresses = atOffset<be<u32>>(header, mHeader->dataOffset() + 4);
}

/* 0289FB20 */
WaveArchiveReader* WaveArchiveReader::construct(WaveArchiveReader* reader, const WaveArchiveHeader* header, u32 useCache) {
    gabi::AutoFrame frame{0x0289FB20u};
    if (!reader) reader = static_cast<WaveArchiveReader*>(allocateWaveArchiveReader(sizeof(WaveArchiveReader)));
    if (!reader) return reader;
    reader->mTable = nullptr;
    reader->mValid = 0;
    reader->mCachedAddresses = nullptr;
    reader->mHeader = nullptr;
    reader->attach(header, useCache);
    return reader;
}

/* 0289FB9C */
WaveArchiveReader* WaveArchiveReader::constructEmpty(WaveArchiveReader* reader) {
    gabi::AutoFrame frame{0x0289FB9Cu};
    if (!reader) reader = static_cast<WaveArchiveReader*>(allocateWaveArchiveReader(sizeof(WaveArchiveReader)));
    if (!reader) return reader;
    reader->mValid = 0;
    reader->mHeader = nullptr;
    reader->mCachedAddresses = nullptr;
    reader->mTable = nullptr;
    return reader;
}

/* 0289FBE4 */
void WaveArchiveReader::detach() {
    if (!mValid) return;
    mValid = 0;
    mHeader = nullptr;
    mCachedAddresses = nullptr;
    mTable = nullptr;
}

/* 0289FC08 */
u32 WaveArchiveReader::count() const { return mValid ? u32(mTable->mCount) : 0; }

/* 0289FC28 */
void WaveArchiveReader::clearCachedAddresses() {
    gabi::AutoFrame frame{0x0289FC28u};
    for (u32 index = 0; index < count(); ++index) mCachedAddresses[index] = 0;
}

/* 0289FC80 */
u32 WaveArchiveReader::fileSize(u32 index) const {
    if (!mValid) return 0;
    return mTable->entry(index)->mSize;
}

// The table has a variable-length tail: index it with the guest stride.
const WaveArchiveBlockEntry* WaveArchiveTable::entry(u32 index) const {
    return atOffset<const WaveArchiveBlockEntry>(this, offsetof(WaveArchiveTable, mEntries) + index * sizeof(WaveArchiveBlockEntry));
}

/* 0289FCA8 */
u32 WaveArchiveReader::fileDataOffset(u32 index) const {
    gabi::AutoFrame frame{0x0289FCA8u};
    if (!mValid) return 0;
    const WaveArchiveHeader* header = mHeader;
    const WaveArchiveTable* table = mTable;
    const u32 offset = header->dataOffset();
    // Past the data block's 8-byte block header.
    return offset + 8 + u32(table->entry(index)->mOffset);
}

/* WWHD glue
ext 0273AD10 void* allocateWaveArchiveReader(u32)
*/
