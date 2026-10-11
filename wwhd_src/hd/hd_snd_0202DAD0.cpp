/** HD sound archive access and resource paths.
 * GameCube-style source verified as hd_snd_0202DAD0; no GameCube counterpart. */
#include "hd/hd_snd_0202DAD0.h"

static hdSound_playerGlobal_c sSoundPlayer = {};
static u32 sArchiveTypeGuard = {};
static u32 sHeapTypeGuard = {};
static hdSound_typeInfo_c sArchiveType = {};
static hdSound_typeInfo_c sHeapType = {};
static hdSound_string_c sPathParts[12] = {};
static hdSound_fixedString256_c sPaths[5] = {};
enum PathPart { COMMON, CAFE, JP, EU, US, AUDIO_RES, JAUDIO_RES,
                COMMON_ARCHIVE, INITIAL_FILE, BANKS, SEQUENCES, SOUND_INFO };
enum SoundPath { ARCHIVE_PATH, INITIAL_FILE_PATH, BANKS_PATH, SEQUENCES_PATH, SOUND_INFO_PATH };
static const u32 archiveTypeParent = 0x10004248;
static const u32 heapTypeParent = 0x10004258;
static const u32 safeStringVtable = 0x10004270;
static const u32 fixedString256Vtable = 0x100042B8;
static const u32 fixedString128Vtable = 0x100042E8;

static hdSound_object_c* hdSound_checkedObject(hdSound_object_c* object, be<u32>* guard,
                                              hdSound_typeInfo_c* type, u32 parent) {
    if (!*guard) {
        *guard = 1;
        type->mParent = parent;
    }
    if (!object) return nullptr;
    if (!gabi::call_ptr<u32>(object->mVtable->mIsDerived, object, type)) return nullptr;
    return object;
}

/* 0202DAD0 */
static hdSound_object_c* hdSound_getArchiveObject() {
    return hdSound_checkedObject(sSoundPlayer.mpPlayer->mpArchive, &sArchiveTypeGuard, &sArchiveType, archiveTypeParent);
}

/* 0202DB6C */
static hdSound_object_c* hdSound_getHeapObject() {
    return hdSound_checkedObject(sSoundPlayer.mpPlayer->mpHeap, &sHeapTypeGuard, &sHeapType, heapTypeParent);
}

/* 0202DC08 */
static void* hdSound_getArchive() {
    hdSound_object_c* object = hdSound_getArchiveObject();
    return hdSound_archiveFromHandle(object->mpArchiveHandle);
}

/* 0202DC30 */
static u32 hdSound_getIdByName(void* name) {
    void* archive = hdSound_getArchive();
    return hdSound_archiveIdByName(archive, name);
}

/* 0202DC64 */
static u32 hdSound_getItemById(u32 id) {
    void* archive = hdSound_getArchive();
    return hdSound_archiveItemById(archive, id);
}

static s32 hdSound_stringLength(be<u8>* text) {
    s32 length = 0;
    if (!*text) return 0;
    for (;;) {
        ++length;
        ++text;
        // HD: the verified SafeString loop treats overlong strings as empty.
        if (length > 0x40000) return 0;
        if (!*text) return length;
    }
}

static void hdSound_copy(hdSound_bufferedString_c* dest, hdSound_string_c* source) {
    // HD: cache the destination buffer and source virtual before invoking it.
    u32 assure = source->mVtable->mAssureTermination;
    be<u8>* data = dest->mStringTop;
    gabi::call_ptr<void>(assure, source);
    s32 length = hdSound_stringLength(source->mStringTop);
    s32 capacity = dest->mCapacity;
    if (!(length < capacity)) length = capacity - 1;
    source->assureTermination();
    OSBlockMove(data, source->mStringTop, length, 0);
    // HD: be<> indexing retains 32-bit guest-address wrap for a negative clamped length.
    data[length] = 0;
}

static void hdSound_append(hdSound_bufferedString_c* dest, hdSound_string_c* source) {
    u32 assure = dest->mVtable->mAssureTermination;
    be<u8>* data = dest->mStringTop;
    gabi::call_ptr<void>(assure, dest);
    s32 current = hdSound_stringLength(dest->mStringTop);
    s32 base = current < 0 ? 0 : current;
    source->assureTermination();
    s32 length = hdSound_stringLength(source->mStringTop);
    s32 room = s32(u32(dest->mCapacity) - u32(base));
    if (!(length < room)) length = room - 1;
    if (length <= 0) return;
    source->assureTermination();
    OSBlockMove(data + base, source->mStringTop, length, 0);
    if (base + length > current) data[base + length] = 0;
}

/* 0202DD2C */
static void hdSound_buildPaths() {
    hdSound_copy(&sPaths[ARCHIVE_PATH], &sPathParts[COMMON]);
    hdSound_append(&sPaths[ARCHIVE_PATH], &sPathParts[AUDIO_RES]);
    hdSound_append(&sPaths[ARCHIVE_PATH], &sPathParts[COMMON_ARCHIVE]);
    gabi::Local<hdSound_fixedString128_c> base;
    base->mBuffer[127] = 0;
    base->mStringTop = base->mBuffer;
    base->mCapacity = 128;
    base->mBuffer[0] = 0;
    base->setVtable(fixedString128Vtable);
    hdSound_copy(base.get(), &sPathParts[CAFE]);
    hdSound_append(base.get(), &sPathParts[US]);
    hdSound_append(base.get(), &sPathParts[AUDIO_RES]);
    hdSound_append(base.get(), &sPathParts[JAUDIO_RES]);
    hdSound_copy(&sPaths[INITIAL_FILE_PATH], base.get());
    hdSound_append(&sPaths[INITIAL_FILE_PATH], &sPathParts[INITIAL_FILE]);
    hdSound_copy(&sPaths[BANKS_PATH], base.get());
    hdSound_append(&sPaths[BANKS_PATH], &sPathParts[BANKS]);
    hdSound_copy(&sPaths[SEQUENCES_PATH], base.get());
    hdSound_append(&sPaths[SEQUENCES_PATH], &sPathParts[SEQUENCES]);
    hdSound_copy(&sPaths[SOUND_INFO_PATH], base.get());
    hdSound_append(&sPaths[SOUND_INFO_PATH], &sPathParts[SOUND_INFO]);
}

/* 0202EA80 */
static hdSound_fixedString256_c* hdSound_getArchivePath() { return &sPaths[ARCHIVE_PATH]; }
/* 0202EA8C */
static hdSound_fixedString256_c* hdSound_getInitialFilePath() { return &sPaths[INITIAL_FILE_PATH]; }
/* 0202EA98 */
static hdSound_fixedString256_c* hdSound_getBanksPath() { return &sPaths[BANKS_PATH]; }
/* 0202EAA4 */
static hdSound_fixedString256_c* hdSound_getSequencesPath() { return &sPaths[SEQUENCES_PATH]; }
/* 0202EAB0 */
static hdSound_fixedString256_c* hdSound_getInfoPath() { return &sPaths[SOUND_INFO_PATH]; }

/* ---------------- HD: compiler-generated companion code ---------------- */

/* 0202DC98 */
static void __sinit_hd_sound_player_cpp() {
    sinit_header_statics_rodata(0x101FFDD4, 0x1018ECB4, 0x101FFDE0, 0x10004268);
}

static void hdSound_initPathPart(PathPart part, const char* text) {
    sPathParts[part].setVtable(safeStringVtable);
    sPathParts[part].setText(text);
}

/* 0202EABC */
static void __sinit_hd_sound_paths_cpp() {
    sinit_header_statics_rodata(0x101FFE50, 0x1018ECD8, 0x101FFE5C, 0x10004300);
    hdSound_initPathPart(COMMON, "Common/");
    hdSound_initPathPart(JP, "JP/");
    hdSound_initPathPart(CAFE, "Cafe/");
    hdSound_initPathPart(EU, "EU/");
    hdSound_initPathPart(US, "US/");
    hdSound_initPathPart(AUDIO_RES, "Audiores/");
    hdSound_initPathPart(BANKS, "Banks/");
    hdSound_initPathPart(SOUND_INFO, "JaiInfo.asn");
    hdSound_initPathPart(SEQUENCES, "Seqs/");
    hdSound_initPathPart(INITIAL_FILE, "JaiInit.aaf");
    hdSound_initPathPart(COMMON_ARCHIVE, "c_king.bfsar");
    hdSound_initPathPart(JAUDIO_RES, "JAudioRes/");
    for (u32 i = 0; i < 5; ++i) {
        hdSound_fixedString256_c* path = &sPaths[i];
        path->mBuffer[0] = 0;
        path->mBuffer[255] = 0;
        path->mStringTop = path->mBuffer;
        path->mCapacity = 256;
        path->setVtable(fixedString256Vtable);
        __register_global_object(0x1018ECFC + i * 0xC);
    }
}

// HD: six deleting template companions share this body; their exact specializations are unconfirmed.
/* 0202ED74 */
static void hdSound_destroyStringStorageA(void* i_this, s32 flags) {
    if (i_this && (flags & 1)) operator_delete(i_this);
}
/* 0202ED88 */
static void hdSound_assureNone() {}
/* 0202ED8C */
static void hdSound_assureTermination(hdSound_bufferedString_c* i_this) {
    be<u8>* data = i_this->mStringTop;
    u32 capacity = i_this->mCapacity;
    data[capacity - 1] = 0;
}
/* 0202EDA4 */
static void hdSound_destroyStringStorageB(void* i_this, s32 flags) {
    if (i_this && (flags & 1)) operator_delete(i_this);
}
/* 0202EDB8 */
static void hdSound_destroyStringStorageC(void* i_this, s32 flags) {
    if (i_this && (flags & 1)) operator_delete(i_this);
}
/* 0202EDCC */
static void hdSound_destroyStringStorageD(void* i_this, s32 flags) {
    if (i_this && (flags & 1)) operator_delete(i_this);
}
/* 0202EDE0 */
static void hdSound_destroyStringStorageE(void* i_this, s32 flags) {
    if (i_this && (flags & 1)) operator_delete(i_this);
}
/* 0202EDF4 */
static void hdSound_destroyStringStorageF(void* i_this, s32 flags) {
    if (i_this && (flags & 1)) operator_delete(i_this);
}

/* WWHD glue
data sSoundPlayer 101F8BA8 nocheck
data sArchiveTypeGuard 101FD634 nocheck
data sHeapTypeGuard 101FD648 nocheck
data sArchiveType 101FD950 nocheck
data sHeapType 101FD958 nocheck
data sPathParts 101FFDF0 nocheck
data sPaths 101FFE6C nocheck
str 10004308 "Common/"
str 10004310 "JP/"
str 1000431C "Cafe/"
str 10004314 "EU/"
str 10004318 "US/"
str 1000434C "Audiores/"
str 1000432C "Banks/"
str 10004340 "JaiInfo.asn"
str 10004324 "Seqs/"
str 10004334 "JaiInit.aaf"
str 10004364 "c_king.bfsar"
str 10004358 "JAudioRes/"
ext 02764774 void* hdSound_archiveFromHandle(void* handle)
ext 0288E91C u32 hdSound_archiveIdByName(void* archive, void* name)
ext 0288E914 u32 hdSound_archiveItemById(void* archive, u32 id)
ext C0009988 void OSBlockMove(be<u8>* dest, be<u8>* source, s32 size, u32 flags)
*/
