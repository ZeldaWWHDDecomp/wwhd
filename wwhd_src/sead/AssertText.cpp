/* WWHD sead assert texts: the delimiter text object's constructor and destructor, the text copies of the
 * translation unit, and its static initialiser (the assert message buffer and its mutex).
 * Reconstructed from this game's instructions. */
#include "sead/AssertText.h"

using namespace sead;

static void* allocateObject(u32 size);
static void releaseObject(void* object);
static void* OSBlockMove(void* destination, const void* source, u32 size, u32 flush);
static void registerGlobalObject(GlobalObjectRecord* record);

static constexpr u32 kDestroyRelease = 1;

typedef FixedSafeText<1024> AssertMessageText;

static u32 sDelimiterTextVtable;
static SafeTextVtable sTextVtable;
static SafeTextVtable sFixedTextVtable;
static SafeTextVtable sDelimiterFixedTextVtable;
static SafeTextVtable sMessageTextVtable;
static AssertMessageText sAssertMessage;
static ThreadMutex sAssertMutex;
static GlobalObjectRecord sAssertMessageRecord;
static GlobalObjectRecord sAssertMutexRecord;

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

/* 0275F948 */
void sead::assert_text::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275F95C */
void sead::assert_text::Text::assureTerminationImpl() {}

/* 0275F960 */
void sead::DelimiterText::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275F974 */
DelimiterText* sead::DelimiterText::construct(DelimiterText* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) {
        object = static_cast<DelimiterText*>(allocateObject(sizeof(DelimiterText)));
        if (object == NULL) return NULL;
    }
    object->mVtable = &sDelimiterTextVtable;
    gabi::FrameLocal<SafeText> delimiters(8);
    delimiters->mText.v = gabi::guest_str(" \t\r\n");
    delimiters->mVtable = &sTextVtable;
    FixedSafeText<128>* text = memberStorage(&object->mText);
    if (text == NULL) return object;
    be<u8>* out = &text->mStorage[0];
    text->mText = out;
    text->mCapacity = 128;
    text->mStorage[127] = 0;
    text->mVtable = &sFixedTextVtable;
    delimiters->assureTermination();
    s32 length = textLength(delimiters->mText);
    if (length >= text->mCapacity) length = text->mCapacity - 1;
    delimiters->assureTermination();
    OSBlockMove(out, delimiters->mText, length, 0);
    out[length] = 0;
    text->mVtable = &sDelimiterFixedTextVtable;
    return object;
}

/* 0275FAAC */
static void __sinit_AssertText_cpp() {
    sAssertMessage.mStorage[1023] = 0;
    sAssertMessage.mStorage[0] = 0;
    sAssertMessage.mText = &sAssertMessage.mStorage[0];
    sAssertMessage.mCapacity = 1024;
    sAssertMessage.mVtable = &sMessageTextVtable;
    registerGlobalObject(&sAssertMessageRecord);
    ThreadMutex::construct(&sAssertMutex);
    registerGlobalObject(&sAssertMutexRecord);
}

/* WWHD glue
 data sDelimiterTextVtable 1015E428 nocheck
 data sTextVtable 1015E340 nocheck
 data sFixedTextVtable 1015E3A0 nocheck
 data sDelimiterFixedTextVtable 1015E3B8 nocheck
 data sMessageTextVtable 1015E388 nocheck
 data sAssertMessage 104A0D80
 data sAssertMutex 104A118C
 data sAssertMessageRecord 101F8B7C nocheck
 data sAssertMutexRecord 101F8B88 nocheck
 str 1015E400 " \t\r\n"
 ext 0273AD10 void* allocateObject(u32)
 ext 0273AF40 void releaseObject(void*)
 ext C0009988 void* OSBlockMove(void*, const void*, u32, u32)
 ext 028F026C void registerGlobalObject(sead::GlobalObjectRecord*)
 ext 027603F0 sead::ThreadMutex* sead::ThreadMutex::construct(sead::ThreadMutex*)
*/
