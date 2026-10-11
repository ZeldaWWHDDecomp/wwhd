/** HD bank-table accessor and six-subsystem sound container. No GameCube counterpart.
 * Private decomp source; subsystem names remain descriptive where original names are unknown. */
#include "bindings.h"
#include "hd/hd_snd_0202B6B0.h"
static void* sSoundResourceManager;
static u32 sArchiveTypeInitialized;
static hdTaskRuntimeTypeInfo sArchiveType;
static hdSoundArchiveSafeString sArchiveName;

/* 0202B6B0 */
static hdSoundArchiveAccessor* constructSoundArchive(hdSoundArchiveAccessor* i_this) {
    if (i_this == NULL) {
        i_this = (hdSoundArchiveAccessor*)operator_new(sizeof(hdSoundArchiveAccessor));
        if (i_this == NULL) {
            return NULL;
        }
    }
    i_this->field_0x8 = 0;
    i_this->mVtable = 0x10004008;
    i_this->mBankTable = NULL;
    i_this->field_0x4 = 0;
    return i_this;
}

/* 0202B700 */
static void destroySoundArchive(hdSoundArchiveAccessor* i_this, s32 flags) {
    if (i_this != NULL && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 0202B714 */
static void setSoundArchiveTable(hdSoundArchiveAccessor* i_this, void* table) {
    i_this->mBankTable = table;
}

/* 0202B71C */
static void loadSoundArchiveTable(hdSoundArchiveAccessor* i_this, u32, u32 size) {
    hdSoundArchiveLoadArg argument;
    argument.mName.mVtable = 0x10003F98;
    argument.mName.mText = STR(0x10000160);
    argument.field_0x28 = 0;
    argument.field_0x18 = 0;
    argument.mBufferAlignment = 0;
    argument.mAlignment = 0x20;
    argument.field_0x24 = 0;
    argument.field_0x20 = 0;
    argument.mBufferSize = 0;
    argument.mSize = 0;
    argument.field_0x1C = 0;
    hdSoundArchiveSafeString* name = hdSoundArchiveGetTablePath();
    const char* text = name->mText;
    argument.mSize = size;
    argument.mAlignment = 0x40;
    argument.mBufferAlignment = 0x40;
    argument.mBufferSize = size;
    argument.mName.mText = text;
    hdSoundArchiveResource* resource = hdSoundArchiveLoadResource(sSoundResourceManager, &argument);
    if (sArchiveTypeInitialized == 0) {
        sArchiveTypeInitialized = 1;
        sArchiveType.mTypeNameAddress = 0x10003FE0;
    }
    if (resource != NULL && resource->checkType(&sArchiveType) == 0) {
        resource = NULL;
    }
    // HD: the original reads the data field even after a null or failed type check.
    setSoundArchiveTable(i_this, hdSoundArchiveResource::dataAfterCheck(resource));
}

/* 0202B828 */
static s32 getSoundArchiveBank(hdSoundArchiveAccessor*, u32 id) {
    enum { KIND_MASK = 0xC0000000u, FIRST_SPECIAL = 0x80000000u, SECOND_SPECIAL = 0xC0000000u };
    u32 kind = id & KIND_MASK;
    if (kind == 0) {
        return (id >> 12) & 0xFF;
    }
    if (kind == FIRST_SPECIAL) {
        return 0x10;
    }
    if (kind == SECOND_SPECIAL) {
        return 0x11;
    }
    return -1;
}

/* 0202B868 */
static u32 getSoundArchiveBankBase(hdSoundArchiveAccessor* i_this, s32 bank) {
    return i_this->bankBase(bank);
}

/* 0202B87C */
static void* getSoundArchiveEntry(hdSoundArchiveAccessor* i_this, u32 id) {
    void* table = i_this->mBankTable;
    if (table == NULL) {
        return NULL;
    }
    s32 bank = getSoundArchiveBank(i_this, id);
    u32 base = getSoundArchiveBankBase(i_this, bank);
    // HD: callbacks above may replace the table; entry computation uses the earlier pointer.
    return gabi::at<void>(gabi::ea(table) + 0x250 + ((base + (id & 0x3FF)) << 5));
}

/* 0202B8EC */
static u32 isSoundArchiveSequence(hdSoundArchiveAccessor* i_this, u32 id) {
    s32 bank = getSoundArchiveBank(i_this, id);
    if (bank >= 0x10) {
        return 0;
    }
    return ((id >> 11) & 1) ^ 1;
}

/* 0202B924 */
static void __sinit_hd_sound_archive_cpp() {
    sinit_header_statics_rodata(0x101FFD48, 0x1018EBF8, 0x101FFD54, 0x10003FF0);
    sArchiveName.mVtable = 0x10003F98;
    sArchiveName.mText = STR(0x10003FF8);
}

/* 0202B9D8 */
static void destroySoundArchiveCompanion(void* i_this, s32 flags) {
    if (i_this != NULL && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 0202B9EC */
static void firstSoundArchiveEmpty() {}

/* 0202B9F0 */
static void secondSoundArchiveEmpty() {}

/* 0202B9F4 */
static u32 soundArchiveCompanionZero() { return 0; }

/* 0202B9FC */
static void thirdSoundArchiveEmpty() {}

/* inlined into constructSoundSystem */
static hdSoundContainerSubsystem* makeSoundSubsystem(u32 size, void* heap, hdSoundSubsystemConstructor construct) {
    auto* subsystem = (hdSoundContainerSubsystem*)hdSoundContainerAllocate(size, heap, 4);
    if (subsystem != NULL) {
        subsystem = construct(subsystem);
    }
    return subsystem;
}

/* 0202BA00 */
static hdSoundSystemContainer* constructSoundSystem(hdSoundSystemContainer* i_this, void* heap) {
    if (i_this == NULL) {
        i_this = (hdSoundSystemContainer*)operator_new(sizeof(hdSoundSystemContainer));
        if (i_this == NULL) {
            return NULL;
        }
    }
    i_this->mSubsystems[5] = NULL;
    i_this->mSubsystems[3] = NULL;
    i_this->mSubsystems[2] = NULL;
    i_this->mSubsystems[0] = NULL;
    i_this->mSubsystems[4] = NULL;
    i_this->mSubsystems[1] = NULL;
    i_this->mSubsystems[0] = makeSoundSubsystem(0x1D0, heap, hdSoundContainerConstructFirst);
    i_this->mSubsystems[1] = makeSoundSubsystem(0x20C, heap, hdSoundContainerConstructSecond);
    i_this->mSubsystems[2] = makeSoundSubsystem(0x40, heap, hdSoundContainerConstructThird);
    auto* fourth = (hdSoundContainerSubsystem*)hdSoundContainerAllocate(4, heap, 4);
    if (fourth != NULL) {
        fourth = hdSoundContainerConstructFourth(fourth, 0x20, heap);
    }
    i_this->mSubsystems[3] = fourth;
    i_this->mSubsystems[4] = makeSoundSubsystem(0x6C, heap, hdSoundContainerConstructFifth);
    i_this->mSubsystems[5] = makeSoundSubsystem(0x10, heap, hdSoundContainerConstructSixth);
    return i_this;
}

/* 0202BB58 */
static void destroySoundSystem(hdSoundSystemContainer* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    for (int i = 0; i < 3; i++) {
        hdSoundContainerSubsystem* subsystem = i_this->mSubsystems[i];
        if (subsystem != NULL) {
            subsystem->destroyFirst();
            i_this->mSubsystems[i] = NULL;
        }
    }
    hdSoundContainerSubsystem* subsystem = i_this->mSubsystems[3];
    if (subsystem != NULL) {
        hdSoundContainerDestroyFourth(subsystem, 3);
        i_this->mSubsystems[3] = NULL;
    }
    for (int i = 4; i < 6; i++) {
        subsystem = i_this->mSubsystems[i];
        if (subsystem != NULL) {
            subsystem->destroyLast();
            i_this->mSubsystems[i] = NULL;
        }
    }
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 0202BC78 */
static void* getFirstSoundSubsystem(hdSoundSystemContainer* i_this) {
    return i_this->mSubsystems[0];
}

/* 0202BC80 */
static void* getSecondSoundSubsystem(hdSoundSystemContainer* i_this) {
    return i_this->mSubsystems[1];
}

/* 0202BC88 */
static void* getThirdSoundSubsystem(hdSoundSystemContainer* i_this) {
    return i_this->mSubsystems[2];
}

/* 0202BC90 */
static void* getFourthSoundSubsystem(hdSoundSystemContainer* i_this) {
    return i_this->mSubsystems[3];
}

/* 0202BC98 */
static void* getFifthSoundSubsystem(hdSoundSystemContainer* i_this) {
    return i_this->mSubsystems[4];
}

/* 0202BCA0 */
static void* getSixthSoundSubsystem(hdSoundSystemContainer* i_this) {
    return i_this->mSubsystems[5];
}

/* 0202BCA8 */
static void __sinit_hd_sound_system_cpp() {
    sinit_header_statics_rodata(0x101FFD64, 0x1018EC1C, 0x101FFD70, 0x100040A8);
}

/* WWHD glue (tools/verify/autoglue.py): HD sound resource and subsystem APIs
 data sSoundResourceManager 101F8B68
 data sArchiveTypeInitialized 101FD6E0
 data sArchiveType 101FD944
 data sArchiveName 101FFD40
 ext 0202EAB0 hdSoundArchiveSafeString* hdSoundArchiveGetTablePath()
 ext 0275DB94 hdSoundArchiveResource* hdSoundArchiveLoadResource(void*, hdSoundArchiveLoadArg*)
 ext 0273B050 void* hdSoundContainerAllocate(u32, void*, u32)
 ext 020305A0 hdSoundContainerSubsystem* hdSoundContainerConstructFirst(hdSoundContainerSubsystem*)
 ext 0202F048 hdSoundContainerSubsystem* hdSoundContainerConstructSecond(hdSoundContainerSubsystem*)
 ext 02030364 hdSoundContainerSubsystem* hdSoundContainerConstructThird(hdSoundContainerSubsystem*)
 ext 020317C0 hdSoundContainerSubsystem* hdSoundContainerConstructFourth(hdSoundContainerSubsystem*, u32, void*)
 ext 020312CC hdSoundContainerSubsystem* hdSoundContainerConstructFifth(hdSoundContainerSubsystem*)
 ext 0202B6B0 hdSoundContainerSubsystem* hdSoundContainerConstructSixth(hdSoundContainerSubsystem*)
 ext 02031838 void hdSoundContainerDestroyFourth(hdSoundContainerSubsystem*, u32)
*/
