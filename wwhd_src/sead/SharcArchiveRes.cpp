/* WWHD sead SARC archive resource: construction, preparing an image (block signatures, byte order,
 * version, sizes), finding a file by path (name hash, binary search over the sorted table, then the names
 * of equal hashes), file access by entry, directory listing, runtime types, and this translation unit's
 * text copies.
 * Reconstructed from this game's instructions; SARC block layout from the public format description
 * (facts only). */
// format facts: SARC archive format (public description)
#include "sead/ArchiveRes.h"

using namespace sead;

static void* allocateObject(u32 size);
static void releaseObject(void* object);
static void* OSBlockMove(void* destination, const void* source, u32 size, u32 flush);
static s32 formatEntryHash(SafeTextBuffer* text, const char* format, u32 hash);

static constexpr u32 kDestroyRelease = 1;
static constexpr u32 kNameUnit = 4;          // file names are aligned to 4 bytes
static constexpr u32 kNameOffsetMask = 0xFFFFFF;

enum ByteOrder { cBigEndian = 0, cLittleEndian = 1 };

/* the platform's byte order */
struct ByteOrderSetting { be<u32> mValue; };

static ResourceVtable sSharcVtable;
static SafeTextVtable sSharcTextVtable;
static ByteOrderSetting sNativeByteOrder;

/* the runtime types along the class chain (guarded statics shared between translation units) */
static u32 sSharcTypeGuard;
static RuntimeType sSharcType;
static u32 sSharcTypeTable;
static u32 sArchiveTypeGuard;
static RuntimeType sArchiveType;
static u32 sArchiveTypeTable;
static u32 sDirectResourceTypeGuard;
static RuntimeType sDirectResourceType;
static u32 sDirectResourceTypeTable;
static u32 sResourceTypeGuard;
static RuntimeType sResourceType;
static u32 sResourceTypeTable;

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

/* guest addresses wrap at 32 bits like the original's pointer arithmetic */
static inline u32 addressAfter(const void* base, u32 offset) { return gabi::ea(base) + offset; }

/* inlined: a block starts with its four signature characters */
static bool hasSignature(const be<u8>* block, const char* signature) {
    const be<u8>* expected = gabi::at<const be<u8>>(gabi::guest_str(signature));
    for (u32 i = 0; i < 4; ++i) {
        if (block[i] != expected[i]) return false;
    }
    return true;
}

/* 0275E11C */
SharcArchiveRes* sead::SharcArchiveRes::construct(SharcArchiveRes* archive) {
    if (archive == NULL) {
        archive = static_cast<SharcArchiveRes*>(allocateObject(sizeof(SharcArchiveRes)));
        if (archive == NULL) return NULL;
    }
    DirectResource::construct(archive);
    archive->mEnable = 0;
    archive->mFatBlockHeader = NULL;
    archive->mVtable = &sSharcVtable;
    archive->mSarcHeader = NULL;
    archive->mFntBlock = NULL;
    SarcFatEntryBuffer* entries = memberStorage(&archive->mFatEntries);
    if (entries != NULL) {
        entries->mBuffer = NULL;
        entries->mSize = 0;
    }
    archive->mEndian = cBigEndian;
    archive->mDataBlock = NULL;
    return archive;
}

/* 0275E1B4 */
void sead::SharcArchiveRes::destroy(u32 flags) {
    if (this == NULL) return;
    DirectResource::destroy(0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275E208 */
u8* sead::SharcArchiveRes::getFileImpl(SafeText* path, ArchiveFileInfo* info) {
    s32 entry = gabi::call_ptr<s32>(archiveVtable()->mConvertPathToEntryId.mFunction, this, path);
    if (entry < 0) return NULL;
    return gabi::call_ptr<u8*>(archiveVtable()->mGetFileFast.mFunction, this, entry, info);
}

/* 0275E288 */
u8* sead::SharcArchiveRes::getFileFastImpl(s32 entry, ArchiveFileInfo* info) {
    if (entry < 0 || entry >= mFatEntries.mSize) return NULL;
    SarcFatEntry* fat = &mFatEntries.mBuffer[entry];
    u32 start = fat->mDataStart;
    if (info != NULL) {
        u32 end = fat->mDataEnd;
        if (start > end) return NULL;
        info->mOffset = start;
        info->mSize = end - start;
    }
    return mDataBlock.get() + start;
}

/* 0275E2E0 */
static u32 calcHash32(SafeText* text, u32 key) {
    text->assureTermination();
    const be<u8>* characters = text->mText;
    u32 hash = 0;
    for (u32 i = 0; characters[i] != 0; ++i) hash = hash * key + characters[i];
    return hash;
}

/* 0275E354 */
static s32 binarySearch(u32 hash, const SarcFatEntry* entries, s32 start, s32 end) {
    for (;;) {
        s32 middle = (start + end) / 2;
        u32 entryHash = entries[middle].mHash;
        if (entryHash > hash) {
            if (end == middle) return -1;
            end = middle;
        } else if (entryHash == hash) {
            return middle;
        } else {
            if (start == middle) return -1;
            start = middle;
        }
    }
}

/* 0275E3A0 */
s32 sead::SharcArchiveRes::convertPathToEntryIdImpl(SafeText* path) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 hash = calcHash32(path, mFatBlockHeader->mHashKey);
    s32 end = mFatEntries.mSize;
    SarcFatEntry* entries = mFatEntries.mBuffer;
    s32 entry = binarySearch(hash, entries, 0, end);
    if (entry == -1) return -1;
    u32 nameOffset = entries[entry].mNameOffset;
    if (nameOffset == 0) return entry;

    /* names with the same hash follow each other; the high byte is this one's 1-based position */
    entry -= (nameOffset >> 24) - 1;
    while (entry < end) {
        SarcFatEntry* fat = &entries[entry];
        if (fat->mHash != hash) return -1;
        u32 offset = fat->mNameOffset & kNameOffsetMask;
        if (addressAfter(mFntBlock.get(), offset) > gabi::ea(mDataBlock.get())) return -1;   // name outside the name table
        gabi::FrameLocal<SafeText> name(8);
        name->mVtable = &sSharcTextVtable;
        name->mText = gabi::at<be<u8>>(addressAfter(mFntBlock.get(), offset * kNameUnit));
        path->assureTermination();
        path->assureTermination();
        be<u8>* left = path->mText;
        name->assureTermination();
        if (left == name->mText) return entry;
        const be<u8>* a = path->mText;
        const be<u8>* b = name->mText;
        for (s32 i = 0; i <= kMaxTextLength; ++i) {
            u8 ca = a[i];
            if (ca != b[i]) break;
            if (ca == 0) return entry;
        }
        if (++entry >= end) break;
        entries = mFatEntries.mBuffer;
    }
    return entry;
}

/* 0275E528 */
bool sead::SharcArchiveRes::openDirectoryImpl(be<u32>* handle, SafeText* path) {
    *handle = 0;
    return true;
}

/* 0275E538 */
u32 sead::SharcArchiveRes::readDirectoryImpl(be<u32>* handle, DirectoryEntry* entries, u32 count) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 read = 0;
    u32 index;
    while ((index = *handle + read) < mFatBlockHeader->mFileCount && read < count) {
        DirectoryEntry* entry = &entries[read];
        SarcFatEntry* fat = &mFatEntries.mBuffer[index];
        u32 nameOffset = fat->mNameOffset;
        if (nameOffset == 0) {
            /* no name table entry: the name is the hash */
            formatEntryHash(&entry->mName, "%08x", fat->mHash);
        } else {
            u32 offset = nameOffset & kNameOffsetMask;
            if (addressAfter(mFntBlock.get(), offset) > gabi::ea(mDataBlock.get())) {
                entry->mName.mText[0] = 0;
            } else {
                gabi::FrameLocal<SafeText> name(8);
                name->mVtable = &sSharcTextVtable;
                name->mText = gabi::at<be<u8>>(addressAfter(mFntBlock.get(), offset * kNameUnit));
                be<u8>* out = entry->mName.mText;
                static_cast<sharc_text::Text*>(name.get())->assureTerminationImpl();
                s32 length = textLength(name->mText);
                if (length >= entry->mName.mCapacity) length = entry->mName.mCapacity - 1;
                name->assureTermination();
                OSBlockMove(out, name->mText, length, 0);
                out[length] = 0;
            }
        }
        entry->mIsDirectory = 0;
        ++read;
    }
    *handle = index;
    return read;
}

/* 0275E6B8 */
bool sead::SharcArchiveRes::prepareArchive(const u8* image) {
    if (image == NULL) return false;
    SarcHeader* header = reinterpret_cast<SarcHeader*>(const_cast<u8*>(image));
    mSarcHeader = header;
    if (!hasSignature(header->mSignature, "SARC")) return false;

    /* the byte-order mark FE FF means big endian; the archive must match the platform */
    u16 mark = header->mByteOrderMark;
    u8 first = mark >> 8;
    u8 second = mark & 0xFF;
    if (first != 0xFF && first == 0xFE && second == 0xFF) mEndian = cBigEndian;
    else mEndian = cLittleEndian;
    if (mEndian != sNativeByteOrder.mValue) return false;

    if (mSarcHeader->mVersion != cArchiveVersion) return false;
    if (mSarcHeader->mHeaderSize != sizeof(SarcHeader)) return false;
    SarcFatHeader* fat = gabi::at<SarcFatHeader>(addressAfter(image, sizeof(SarcHeader)));
    mFatBlockHeader = fat;
    if (!hasSignature(fat->mSignature, "SFAT")) return false;
    if (fat->mHeaderSize != sizeof(SarcFatHeader)) return false;
    u32 fileCount = fat->mFileCount;
    if (fileCount > cArchiveEntryMax) return false;

    u32 fatEntries = addressAfter(image, mSarcHeader->mHeaderSize + sizeof(SarcFatHeader));
    if (static_cast<s32>(fileCount) > 0 && fatEntries != 0) {
        mFatEntries.mBuffer = gabi::at<SarcFatEntry>(fatEntries);
        mFatEntries.mSize = fileCount;
    }
    SarcFntHeader* fnt = gabi::at<SarcFntHeader>(addressAfter(
        image, mSarcHeader->mHeaderSize + mFatBlockHeader->mHeaderSize +
                   mFatBlockHeader->mFileCount * sizeof(SarcFatEntry)));
    if (!hasSignature(fnt->mSignature, "SFNT")) return false;
    if (fnt->mHeaderSize != sizeof(SarcFntHeader)) return false;

    u32 namesAt = addressAfter(fnt, sizeof(SarcFntHeader));
    mFntBlock = gabi::at<u8>(namesAt);
    u32 headerAt = gabi::ea(mSarcHeader.get());
    u32 dataOffset = mSarcHeader->mDataOffset;
    /* the data block must not start inside the name table (a name table before the header passes) */
    if (namesAt >= headerAt && dataOffset < namesAt - headerAt) return false;
    mDataBlock = gabi::at<u8>(addressAfter(image, dataOffset));
    return true;
}

/* 0275E880 */
void sead::sharc_text::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275E894 */
void sead::sharc_text::Text::assureTerminationImpl() {}

/* 0275E898 */
void sead::SharcArchiveRes::postCreate() {}

/* 0275E89C */
u32 sead::SharcArchiveRes::getLoadDataAlignment() { return 0x80; }

/* 0275E8A4 */
bool sead::SharcArchiveRes::isDerivedFrom(const RuntimeType* type) const {
    if (sSharcTypeGuard == 0) {
        sSharcTypeGuard = 1;
        sSharcType.mTable = &sSharcTypeTable;
    }
    if (type == &sSharcType) return true;
    if (sArchiveTypeGuard == 0) {
        sArchiveTypeGuard = 1;
        sArchiveType.mTable = &sArchiveTypeTable;
    }
    if (type == &sArchiveType) return true;
    if (sDirectResourceTypeGuard == 0) {
        sDirectResourceTypeGuard = 1;
        sDirectResourceType.mTable = &sDirectResourceTypeTable;
    }
    if (type == &sDirectResourceType) return true;
    if (sResourceTypeGuard == 0) {
        sResourceTypeGuard = 1;
        sResourceType.mTable = &sResourceTypeTable;
    }
    return type == &sResourceType;
}

/* 0275E9A0 */
bool sead::SharcArchiveRes::closeDirectoryImpl(be<u32>* handle) { return true; }

/* WWHD glue
 data sSharcVtable 1015E184 nocheck
 data sSharcTextVtable 1015E11C nocheck
 data sNativeByteOrder 104A0CD4
 data sSharcTypeGuard 101FD77C
 data sSharcType 101FDFB0
 data sSharcTypeTable 1015E154 nocheck
 data sArchiveTypeGuard 101FD6F4
 data sArchiveType 101FD9D0
 data sArchiveTypeTable 1015E144 nocheck
 data sDirectResourceTypeGuard 101FD6E0
 data sDirectResourceType 101FD944
 data sDirectResourceTypeTable 1015E134 nocheck
 data sResourceTypeGuard 101FD6DC
 data sResourceType 101FD94C
 data sResourceTypeTable 1015E10C nocheck
 str 1015E164 "%08x"
 str 1015E16C "SARC"
 str 1015E174 "SFAT"
 str 1015E17C "SFNT"
 ext 0273AD10 void* allocateObject(u32)
 ext 0273AF40 void releaseObject(void*)
 ext C0009988 void* OSBlockMove(void*, const void*, u32, u32)
 ext 02759C28 s32 formatEntryHash(sead::SafeTextBuffer*, const char*, u32)
*/
