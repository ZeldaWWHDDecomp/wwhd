#include "bindings.h"

// HD: linked message initializer and compiler companions; these are not a GameCube TU.
struct MesgCompilerString {
    be<u32> mData;
    be<u32> field_0x4;
    be<u32> mSize;
};
WWHD_SIZE(MesgCompilerString, 12);
struct MesgInitWords {
    be<u32> first;
    be<u32> second;
    be<u32> third;
    be<u32> fourth;
};
struct MesgInitFloats { be<f32> low; be<f32> high; };
struct MesgRegistration { be<u32> next; u8 storage[8]; };
static MesgInitWords initializedWords;
static MesgInitFloats initializedFloats;
static MesgRegistration wordsRegistration;
static MesgRegistration byteRegistration;
static MesgRegistration wideRegistration;
static u8 byteStringState;
static u8 wideStringState;
static f32 initialLow;
static f32 initialHigh;
void mesgRegisterGlobal(MesgRegistration*);
void mesgInitializeByteState(void*);
void mesgInitializeWideState(void*);
void mesgDeleteStorage(void*);

/* 025F8EC4 */
static void mesgInitializeGlobals() {
    initializedWords.third = 0;
    initializedWords.first = 0;
    initializedWords.fourth = 0;
    initializedWords.second = 0;
    mesgRegisterGlobal(&wordsRegistration);
    f32 low = initialLow;
    f32 high = initialHigh;
    initializedFloats.low = low;
    initializedFloats.high = high;
    mesgInitializeByteState(&byteStringState);
    mesgRegisterGlobal(&byteRegistration);
    mesgInitializeWideState(&wideStringState);
    mesgRegisterGlobal(&wideRegistration);
}

/* 025F8F58 */
// HD: distinct compiler deleting-destructor entry, identity is not established.
static void mesgDeleteCompanionA(void* object, u32 flags) {
    if (object != NULL && (flags & 1)) {
        mesgDeleteStorage(object);
    }
}

/* 025F8F6C */
// HD: distinct compiler deleting-destructor entry, identity is not established.
static void mesgDeleteCompanionB(void* object, u32 flags) {
    if (object != NULL && (flags & 1)) {
        mesgDeleteStorage(object);
    }
}

/* 025F8F9C */
// HD: distinct compiler deleting-destructor entry, identity is not established.
static void mesgDeleteCompanionC(void* object, u32 flags) {
    if (object != NULL && (flags & 1)) {
        mesgDeleteStorage(object);
    }
}

/* 025F8FD0 */
// HD: distinct compiler deleting-destructor entry, identity is not established.
static void mesgDeleteCompanionD(void* object, u32 flags) {
    if (object != NULL && (flags & 1)) {
        mesgDeleteStorage(object);
    }
}

/* 025F8FE4 */
// HD: distinct compiler deleting-destructor entry, identity is not established.
static void mesgDeleteCompanionE(void* object, u32 flags) {
    if (object != NULL && (flags & 1)) {
        mesgDeleteStorage(object);
    }
}

/* 025F8FF8 */
// HD: distinct compiler deleting-destructor entry, identity is not established.
static void mesgDeleteCompanionF(void* object, u32 flags) {
    if (object != NULL && (flags & 1)) {
        mesgDeleteStorage(object);
    }
}

/* 025F900C */
// HD: distinct compiler deleting-destructor entry, identity is not established.
static void mesgDeleteCompanionG(void* object, u32 flags) {
    if (object != NULL && (flags & 1)) {
        mesgDeleteStorage(object);
    }
}

/* 025F9020 */
// HD: distinct compiler deleting-destructor entry, identity is not established.
static void mesgDeleteCompanionH(void* object, u32 flags) {
    if (object != NULL && (flags & 1)) {
        mesgDeleteStorage(object);
    }
}

/* 025F9034 */
// HD: distinct compiler deleting-destructor entry, identity is not established.
static void mesgDeleteCompanionI(void* object, u32 flags) {
    if (object != NULL && (flags & 1)) {
        mesgDeleteStorage(object);
    }
}

/* 025F9048 */
// HD: distinct compiler deleting-destructor entry, identity is not established.
static void mesgDeleteCompanionJ(void* object, u32 flags) {
    if (object != NULL && (flags & 1)) {
        mesgDeleteStorage(object);
    }
}

/* 025F905C */
// HD: distinct compiler deleting-destructor entry, identity is not established.
static void mesgDeleteCompanionK(void* object, u32 flags) {
    if (object != NULL && (flags & 1)) {
        mesgDeleteStorage(object);
    }
}

/* 025F8F80 */
static void mesgByteCompanionNoop() {}

/* 025F8FB0 */
static void mesgWideCompanionNoop() {}

/* 025F8F84 */
static void mesgTerminateBytes(MesgCompilerString* string) {
    u32 data = string->mData;
    u32 size = string->mSize;
    // HD: retain sparse guest stores, including wrapped and unmapped string buffers.
    gabi::store<u8>(data + size - 1, 0);
}

/* 025F8FB4 */
static void mesgTerminateWords(MesgCompilerString* string) {
    u32 size = string->mSize;
    u32 data = string->mData;
    // HD: retain sparse guest stores; the wide specialization counts 16-bit units.
    gabi::store<u16>(data + 2 * size - 2, 0);
}

/* WWHD glue
data initializedWords 1048D728
data initializedFloats 1048D71C
data wordsRegistration 101F4B38
data byteRegistration 101F4B44
data wideRegistration 101F4B50
data byteStringState 1048D724
data wideStringState 1048D725
data initialLow 100E0CA8
data initialHigh 100E0CAC
ext 028F026C void mesgRegisterGlobal(MesgRegistration*)
ext 028ED6F8 void mesgInitializeByteState(void*)
ext 028EAB2C void mesgInitializeWideState(void*)
ext 0273AF40 void mesgDeleteStorage(void*)
*/
