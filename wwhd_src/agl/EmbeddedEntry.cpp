// WWHD render entry with an embedded colour correction; its operations (agl/ColorCorrection.cpp) are called by address.
#include "agl/EmbeddedEntry.h"
#include "sead/FileDevice.h"
void releaseEmbeddedStorage(agl::color_correction::ColorCorrection*);
void initializeEmbeddedStorage(agl::color_correction::ColorCorrection*, void*, u32);
void associateEntryResource(agl::EntryNameObject_l*, void*, const sead::SafeText*);
void constructEmbeddedStorage(agl::color_correction::ColorCorrection*);
void* allocateAglStorage(u32);
agl::RenderEntry* createRenderEntry(agl::RenderEntry*, u32, u32);
static u32 sEmbeddedEntryMethods;
static u32 sEmbeddedEntryVtable;
static sead::SafeTextVtable sEmbeddedTextMethods;
struct EmbeddedLabel { be<u8> mFirst; };
static EmbeddedLabel sEmbeddedLabel;

/* 027ADA0C */
static void destroyEmbeddedStorage(agl::EmbeddedStorageEntry* entry) {
    return gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseEmbeddedStorage), &entry->mStorage);
}
/* 027ADA14 */
static void initializeEmbeddedEntry(agl::EmbeddedStorageEntry* entry, void* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    initializeEmbeddedStorage(&entry->mStorage, context, 0);
    entry->mStorage.mEnable.mValue = 1;
    gabi::FrameLocal<sead::SafeText> label(0x8);
    label->mVtable = &sEmbeddedTextMethods;
    label->mText = &sEmbeddedLabel.mFirst;
    associateEntryResource(&entry->mNameObject, &entry->mStorage.mRoot, label.get());
}
/* 027AB75C */
agl::EmbeddedStorageEntry* createEmbeddedEntry(agl::EmbeddedStorageEntry* entry, u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (entry == NULL) entry = (agl::EmbeddedStorageEntry*)allocateAglStorage(sizeof(agl::EmbeddedStorageEntry));
    if (entry != NULL) {
        createRenderEntry(entry, 3, index);
        entry->mMethods = (agl::RenderEntryMethods*)&sEmbeddedEntryMethods;
        entry->mVtable = gabi::ea(&sEmbeddedEntryVtable);
        constructEmbeddedStorage(&entry->mStorage);
    }
    return entry;
}
/* WWHD glue
 ext 02783898 void releaseEmbeddedStorage(agl::color_correction::ColorCorrection*) callback
 data sEmbeddedEntryMethods 10168BB4
 data sEmbeddedEntryVtable 10168BFC
 data sEmbeddedTextMethods 1016858C
 data sEmbeddedLabel 10168AEC
 ext 02784400 void initializeEmbeddedStorage(agl::color_correction::ColorCorrection*, void*, u32)
 ext 0279CB24 void associateEntryResource(agl::EntryNameObject_l*, void*, const sead::SafeText*)
 ext 02783A70 void constructEmbeddedStorage(agl::color_correction::ColorCorrection*)
*/
