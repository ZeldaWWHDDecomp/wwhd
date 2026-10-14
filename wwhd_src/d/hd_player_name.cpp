// HD saved player name conversion and copying. Private decomp-derived source.
#include "d/hd_picture_statics.h"
#include "d/hd_player_name.h"
static u32 sWideNameVtable;
static u32 sPlayerNameSafeStringVtable;
static u32 sNarrowNameVtable;
static u32 sPlayerNameVtable;
static hdPictureStaticState_c sPlayerNameHeaderState;
static hdPictureStaticTokens_c sPlayerNameHeaderTokens;
static const hdPictureAngles_c sPlayerNameHeaderAngles;
/* 02726424 */
void hdPlayerName_c::initialize() {
    // HD: named glue spellings distinguish the two separate empty string copies.
    playerNameFormatWide(&mWideName, "wide-name-empty");
    playerNameFormat(&mNarrowName, "narrow-name-empty");
}
/* 02726470 */
static hdPlayerName_c* constructPlayerName(hdPlayerName_c* self) {
    if (!self) {
        self = static_cast<hdPlayerName_c*>(playerNameAllocate(sizeof(hdPlayerName_c)));
        if (!self) return NULL;
    }
    self->mWideName.mpText = reinterpret_cast<u8*>(self->mWideName.mBuffer);
    self->mWideName.mVtable.v = gabi::ea(&sWideNameVtable);
    self->mWideName.mCapacity = 9;
    reinterpret_cast<be<u16>*>(self->mWideName.mBuffer)[0] = 0;
    reinterpret_cast<be<u16>*>(self->mWideName.mBuffer)[8] = 0;
    hdNameString_c* narrow = gabi::at<hdNameString_c>(gabi::ea(self) + offsetof(hdPlayerName_c, mNarrowName));
    if (!narrow) {
        narrow = static_cast<hdNameString_c*>(playerNameAllocate(sizeof(hdNameString_c)));
        if (!narrow) { self->initialize(); return self; }
    }
    narrow->mpText = reinterpret_cast<u8*>(narrow->mBuffer);
    narrow->mVtable.v = gabi::ea(&sPlayerNameSafeStringVtable);
    narrow->mCapacity = 18;
    narrow->mBuffer[17] = 0;
    narrow->mVtable.v = gabi::ea(&sNarrowNameVtable);
    *reinterpret_cast<be<u8>*>(narrow->mpText.get()) = 0;
    narrow->mVtable.v = gabi::ea(&sPlayerNameVtable);
    self->initialize();
    return self;
}
/* 02726578 */
void hdPlayerName_c::setNarrowName(const char* name) {
    playerNameFormat(&mNarrowName, name);
    mWideName.assureTermination();
    playerNameConvertWide(mWideName.mpText, 9, name, -1);
}
/* 027265E0 */
void hdPlayerName_c::setDefault() {
    initialize();
    hdNameString_c name;
    name.mpText = reinterpret_cast<u8*>(name.mBuffer); name.mVtable.v = gabi::ea(&sPlayerNameVtable); name.mCapacity = 18;
    name.mBuffer[0] = 0; name.mBuffer[17] = 0;
    playerNameFormat(&name, "Link");
    name.assureTermination();
    setNarrowName(reinterpret_cast<char*>(name.mpText.get()));
}
/* 02726668 */
void hdPlayerName_c::setWideName(u8* name) {
    playerNameFormatWide(&mWideName, reinterpret_cast<char*>(name));
    mNarrowName.assureTermination();
    playerNameConvertNarrow(mNarrowName.mpText, 18, name, 8);
}
/* inlined into copyWideName and copy */
static void copyWideString(hdNameString_c* dest, hdNameString_c* source) {
    u8* output = dest->mpText;
    source->assureTermination();
    be<u16>* text = reinterpret_cast<be<u16>*>(source->mpText.get());
    s32 length = 0;
    while (text[length] != 0) {
        if (++length > 0x40000) { length = 0; break; }
    }
    s32 capacity = dest->mCapacity;
    if (length >= capacity) length = capacity - 1;
    source->assureTermination();
    playerNameCopy(output, source->mpText.get(), u32(length) * 2, 0);
    reinterpret_cast<be<u16>*>(output)[length] = 0;
}
/* 027266CC */
void hdPlayerName_c::copyWideName(hdNameString_c* name) {
    mNarrowName.assureTermination();
    u8* output = mNarrowName.mpText;
    name->assureTermination();
    u32 capacity = mNarrowName.mCapacity;
    playerNameConvertNarrow(output, capacity, name->mpText, -1);
    playerNameFormatWide(&mWideName, "wide-name-empty");
    copyWideString(&mWideName, name);
}
/* 027267FC */
void hdPlayerName_c::copy(hdPlayerName_c* name) {
    playerNameFormatWide(&mWideName, "wide-name-empty");
    copyWideString(&mWideName, &name->mWideName);
    name->mNarrowName.assureTermination();
    playerNameFormatText(&mNarrowName, "%s", reinterpret_cast<char*>(name->mNarrowName.mpText.get()));
}
/* 02726914 */
static void initializePlayerNameStatics() {
    initializePictureHeaderStatics(&sPlayerNameHeaderState, &sPlayerNameHeaderTokens, &sPlayerNameHeaderAngles);
}
/* WWHD glue
 data sWideNameVtable 10140C80 nocheck
 data sPlayerNameSafeStringVtable 10140C50 nocheck
 data sNarrowNameVtable 10140C98 nocheck
 data sPlayerNameVtable 10140CB0 nocheck
 data sPlayerNameHeaderState 1049F830
 data sPlayerNameHeaderTokens 101F85BC
 data sPlayerNameHeaderAngles 10140CF4
 str 10140CC8 "wide-name-empty" renamed in 02726424
 str 10140CDC "narrow-name-empty" renamed in 02726424
 str 10140CC8 "wide-name-empty" renamed in 027266CC
 str 10140CC8 "wide-name-empty" renamed in 027267FC
 str 10140CE8 "Link"
 str 10140CF0 "%s" in 027267FC
 ext 0273AD10 void* playerNameAllocate(u32)
 ext 02759D3C void playerNameFormatWide(hdNameString_c*, const char*)
 ext 02759C28 void playerNameFormat(hdNameString_c*, const char*)
 ext 02759C28 void playerNameFormatText(hdNameString_c*, const char*, const char*)
 ext 0275BAA0 void playerNameConvertWide(u8*, u32, const char*, s32)
 ext 0275BC8C void playerNameConvertNarrow(u8*, u32, u8*, s32)
 ext C0009988 void playerNameCopy(void*, const void*, u32, u32)
*/
