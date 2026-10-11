/** HD sound archive and sound heap. No GameCube counterpart.
 * Private decomp source. */
#include "bindings.h"
#include "hd/hd_snd_020302E4.h"

static u32 sArchiveTypeInitialized;
static u32 sParentTypeInitialized;
static hdSoundRuntimeTypeInfo sArchiveType;
static hdSoundRuntimeTypeInfo sParentType;
static hdSoundArchiveName sArchiveName;
static f32 sArchiveFloatOption;
enum : u32 {
    ARCHIVE_TYPE_NAME_ADDRESS = 0x100044A0,
    PARENT_TYPE_NAME_ADDRESS = 0x10004490,
    ARCHIVE_VTABLE = 0x100046E4,
    ARCHIVE_NAME_VTABLE = 0x100046BC,
    SOUND_HEAP_VTABLE = 0x10004788,
    SOUND_HEAP_SECONDARY_VTABLE = 0x10004778,
};

/* 020302E4 */
static u32 checkDerivedRuntimeTypeInfo(void* i_this, hdSoundRuntimeTypeInfo* typeInfo) {
    if (sArchiveTypeInitialized == 0) {
        sArchiveTypeInitialized = 1;
        sArchiveType.mTypeNameAddress = ARCHIVE_TYPE_NAME_ADDRESS;
    }
    if (typeInfo == &sArchiveType) {
        return TRUE;
    }
    if (sParentTypeInitialized == 0) {
        sParentTypeInitialized = 1;
        sParentType.mTypeNameAddress = PARENT_TYPE_NAME_ADDRESS;
    }
    return typeInfo == &sParentType ? TRUE : FALSE;
}

/* 0203035C */
static u32 getSoundIdByName(hdSoundArchive* i_this, const char* name) {
    return hdSoundArchiveLookupId(i_this->mArchiveData, name);
}

/* 02030364 */
static hdSoundArchive* constructSoundArchive(hdSoundArchive* i_this) {
    if (i_this == NULL) {
        i_this = (hdSoundArchive*)operator_new(sizeof(hdSoundArchive));
        if (i_this == NULL) {
            return NULL;
        }
    }
    hdSoundArchiveConstructBase(i_this);
    i_this->mVtable = ARCHIVE_VTABLE;
    hdSoundArchiveSetName(i_this, &sArchiveName);
    const char* path = hdSoundGetArchivePath();
    hdSoundArchiveSetPath(i_this, path);
    hdSoundArchiveSetFloatOption(i_this, sArchiveFloatOption);
    hdSoundArchiveSetFirstIntegerOption(i_this, 0x28000);
    hdSoundArchiveSetSecondIntegerOption(i_this, 8);
    return i_this;
}

/* 02030404 */
static void* getSoundArchive() {
    return hdSoundGetManager()->mSoundArchive;
}

/* 02030428 */
static u32 loadSoundGroup(hdSoundArchive* i_this, const char* name) {
    void* archive = getSoundArchive();
    return hdSoundArchiveLoadGroup(archive, name, -1, 0, 0);
}

/* 02030468 */
static u32 loadStaticSoundGroup(hdSoundArchive* i_this) {
    return loadSoundGroup(i_this, STR(0x10004720)); // HD: GROUP_STATIC
}

/* 02030474 */
static u32 loadBgmWdsSoundGroup(hdSoundArchive* i_this) {
    return loadSoundGroup(i_this, STR(0x10004730)); // HD: GROUP_BGM_WDS
}

/* 02030480 */
static void __sinit_hd_sound_archive_cpp() {
    sinit_header_statics_rodata(0x10200410, 0x1018EDC0, 0x1020041C, 0x10004740);
    sArchiveName.mVtable = ARCHIVE_NAME_VTABLE;
    sArchiveName.mText = STR(0x10004748);
}

/* 02030534 */
static void destroySoundArchiveCompanion(void* i_this, s32 flags) {
    if (i_this != NULL && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 02030548 */
static void soundArchiveCompanionEmpty() {}

/* 0203054C */
static void destroySoundHeap(hdSoundHeap* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    hdSoundHeapDestroyBase(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 020305A0 */
static hdSoundHeap* constructSoundHeap(hdSoundHeap* i_this) {
    if (i_this == NULL) {
        i_this = (hdSoundHeap*)operator_new(sizeof(hdSoundHeap));
        if (i_this == NULL) {
            return NULL;
        }
    }
    hdSoundHeapConstructBase(i_this);
    i_this->mVtable = SOUND_HEAP_VTABLE;
    i_this->mSecondaryVtable = SOUND_HEAP_SECONDARY_VTABLE;
    hdSoundHeapSetFirstOption(i_this, 1, 0);
    hdSoundHeapSetSecondOption(i_this, 0);
    hdSoundHeapSetThirdOption(i_this, 0);
    hdSoundHeapSetFourthOption(i_this, 0);
    hdSoundHeapMemberArgument argument;
    hdSoundHeapConstructMemberArgument(&argument);
    argument.field_0x0 = 0;
    hdSoundHeapInitializeMember(i_this->mMemberStorage, &argument);
    return i_this;
}

/* 0203065C */
static void __sinit_hd_sound_heap_cpp() {
    sinit_header_statics_rodata(0x1020042C, 0x1018EDE4, 0x10200438, 0x10004818);
}

/* WWHD glue (tools/verify/autoglue.py): private sound globals and calls
 data sArchiveTypeInitialized 101FD634
 data sParentTypeInitialized 101FD2D8
 data sArchiveType 101FD950
 data sParentType 101FD954
 data sArchiveName 10200408
 data sArchiveFloatOption 1000471C
 ext 0288E91C u32 hdSoundArchiveLookupId(void*, const char*)
 ext 027650B4 void hdSoundArchiveConstructBase(hdSoundArchive*)
 ext 02765450 void hdSoundArchiveSetName(hdSoundArchive*, hdSoundArchiveName*)
 ext 0202EA80 const char* hdSoundGetArchivePath()
 ext 0276543C void hdSoundArchiveSetPath(hdSoundArchive*, const char*)
 ext 02765424 void hdSoundArchiveSetFloatOption(hdSoundArchive*, f32)
 ext 0276542C void hdSoundArchiveSetFirstIntegerOption(hdSoundArchive*, u32)
 ext 02765434 void hdSoundArchiveSetSecondIntegerOption(hdSoundArchive*, u32)
 ext 0202DAD0 hdSoundManagerArchiveView* hdSoundGetManager()
 ext 02764B90 u32 hdSoundArchiveLoadGroup(void*, const char*, s32, u32, u32)
 ext 027651A4 void hdSoundHeapDestroyBase(hdSoundHeap*, s32)
 ext 0276231C void hdSoundHeapConstructBase(hdSoundHeap*)
 ext 02762CE8 void hdSoundHeapSetFirstOption(hdSoundHeap*, u32, u32)
 ext 02762CE0 void hdSoundHeapSetSecondOption(hdSoundHeap*, u32)
 ext 02762D50 void hdSoundHeapSetThirdOption(hdSoundHeap*, u32)
 ext 02762D48 void hdSoundHeapSetFourthOption(hdSoundHeap*, u32)
 ext 02765EF0 void hdSoundHeapConstructMemberArgument(hdSoundHeapMemberArgument*)
 ext 02766F1C void hdSoundHeapInitializeMember(void*, hdSoundHeapMemberArgument*)
*/
