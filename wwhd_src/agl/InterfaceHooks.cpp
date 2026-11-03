// WWHD interface leaves; names describe local roles, not asserted original symbols.
#include "agl/InterfaceHooks.h"
void releaseAglEntryStorage(void*);

/* 027A8C7C */
static void renderHook() {}

/* 027A8CDC */
static void surfaceHook() {}

/* 027A8CE8 */
static void resourceHook() {}

/* 027AAAB8 */
static void drawHook() {}

/* 027ADA78 */
static void effectHook() {}

/* 027AE4B0 */
static void parameterHook() {}

/* 027AE51C */
static void valueHook() {}

/* 027AE5A8 */
static void archiveHook() {}

/* 027AE5C8 */
static void readHook() {}

/* 027AE5CC */
static void writeHook() {}

/* 027AE5D0 */
static void prepareHook() {}

/* 027AE5D4 */
static void finishHook() {}

/* 027A8CD4 */
static u32 acceptSurface() { return 1; }

/* 027A8CE0 */
static u32 acceptResource() { return 1; }

/* 027AE5A0 */
static u32 acceptArchive() { return 1; }

/* 027A8C98 */
static void releaseName(void* object, u32 flags) {
    if (object != NULL && (flags & 1))
        return gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseAglEntryStorage), object);
}

/* 027A8CAC */
static void releaseWideName(void* object, u32 flags) {
    if (object != NULL && (flags & 1))
        return gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseAglEntryStorage), object);
}

/* 027A8CC0 */
static void releaseResourceName(void* object, u32 flags) {
    if (object != NULL && (flags & 1))
        return gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseAglEntryStorage), object);
}

/* 027AE49C */
void releaseEffectName(void* object, u32 flags) {
    if (object != NULL && (flags & 1))
        return gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseAglEntryStorage), object);
}

/* 027AE4CC */
static void releaseByteParameter(void* object, u32 flags) {
    if (object != NULL && (flags & 1))
        return gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseAglEntryStorage), object);
}

/* 027AE4E0 */
static void releaseWordParameter(void* object, u32 flags) {
    if (object != NULL && (flags & 1))
        return gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseAglEntryStorage), object);
}

/* 027AE4F4 */
static void releaseFloatParameter(void* object, u32 flags) {
    if (object != NULL && (flags & 1))
        return gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseAglEntryStorage), object);
}

/* 027AE508 */
static void releasePairParameter(void* object, u32 flags) {
    if (object != NULL && (flags & 1))
        return gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseAglEntryStorage), object);
}

/* 027AE618 */
static void releaseEmptyInterface(void* object, u32 flags) {
    if (object != NULL && (flags & 1))
        return gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseAglEntryStorage), object);
}

/* 027A8CEC */
static u32 matchesResourceType(agl::ResourceObject_l* object, gptr<agl::ResourceDescriptor>* descriptor) {
    return u32(object->mTypeHash) == u32((*descriptor)->mTypeHash);
}
/* 027AE5AC */
static u32 matchesResourceName(agl::ResourceObject_l* object, gptr<agl::ResourceDescriptor>* descriptor) {
    return u32(object->mNameHash) == u32((*descriptor)->mNameHash);
}

static sead::SafeText sEmptyText;
static u32 sEmptyTextGuard;
struct EmptyCharacter { be<u8> mValue; };
static EmptyCharacter sEmptyCharacters;
static sead::SafeTextVtable sEmptyTextMethods;
WWHD_OPAQUE(ExitRegistration_l);
static ExitRegistration_l sEmptyTextRegistration;
void registerAglExit(ExitRegistration_l*);

/* 027AE62C */
sead::SafeText* agl::emptyText() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (sEmptyTextGuard == 0) {
        sEmptyTextGuard = 1;
        sEmptyText.mText = &sEmptyCharacters.mValue;
        sEmptyText.mVtable = &sEmptyTextMethods;
        registerAglExit(&sEmptyTextRegistration);
    }
    return &sEmptyText;
}
static u32 sEmptyInterfaceTable;
void* allocateAglStorage(u32 size);
/* 027AE5D8 */
static agl::EmptyInterface* createEmptyInterface(agl::EmptyInterface* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = (agl::EmptyInterface*)allocateAglStorage(sizeof(agl::EmptyInterface));
    if (object != NULL) object->mVtable = gabi::ea(&sEmptyInterfaceTable);
    return object;
}
/* WWHD glue
 data sEmptyInterfaceTable 10168E5C
 data sEmptyText 104A2080
 data sEmptyTextGuard 104A2088
 data sEmptyCharacters 10168E54
 data sEmptyTextMethods 10168E1C
 data sEmptyTextRegistration 101F8DF4
 ext 028F026C void registerAglExit(ExitRegistration_l*)
*/
