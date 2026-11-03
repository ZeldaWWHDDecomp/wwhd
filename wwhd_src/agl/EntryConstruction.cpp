// WWHD base render-entry construction, including exact caller stack descriptors.
#include "agl/EntryConstruction.h"
static u32 sInitialEntryTable;
static u32 sFinalEntryTable;
static agl::RenderEntryMethods sBaseEntryMethods;
static sead::SafeTextVtable sParameterTextTable;
static u32 sByteParameterTable;
static u32 sWordParameterTable;
static EntryLabel sIdleName;
static EntryLabel sIdleLabel;
static EntryLabel sQueuedName;
static EntryLabel sIndexName;
static EntryLabel sEntryLabel;
static EntryLabel sEmptyHelp;
static EntryLabel sIndexHelp;

/* 027AB058 */
agl::RenderEntry* createRenderEntry(agl::RenderEntry* entry, u32 kind, u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (entry == NULL) entry = (agl::RenderEntry*)allocateAglStorage(sizeof(agl::RenderEntry));
    if (entry != NULL) {
        entry->mNext = NULL;
        entry->mSelf = entry;
        entry->mPrevious = NULL;
        entry->mCurrentList = NULL;
        entry->mVtable = gabi::ea(&sInitialEntryTable);
        constructEntryName(&entry->mNameObject);
        entry->mMethods = &sBaseEntryMethods;
        entry->mVtable = gabi::ea(&sFinalEntryTable);
        constructEntryContainer(entry->mParameterContainer);
        entry->mOwner = NULL;
        entry->mActiveList = NULL;
        entry->mKind = kind;
        entry->mTag = ((kind << 8) + 0x100) | (index + 1);
        gabi::FrameLocal<sead::SafeText> idleName(0x8);
        gabi::FrameLocal<sead::SafeText> idleLabel(0x10);
        gabi::FrameLocal<sead::SafeText> idleHelp(0x40);
        idleLabel->mVtable = &sParameterTextTable;
        idleName->mVtable = &sParameterTextTable;
        idleName->mText = &sIdleName.mFirst;
        idleLabel->mText = &sIdleLabel.mFirst;
        constructEntryParameter(&entry->mIdle);
        entry->mIdle.mVtable = gabi::ea(&sByteParameterTable);
        idleHelp->mVtable = &sParameterTextTable;
        idleHelp->mText = &sEmptyHelp.mFirst;
        attachEntryParameter(&entry->mIdle, idleName.get(), idleLabel.get(), idleHelp.get(), entry->mParameterContainer);
        entry->mIdle.mValue = 0;
        gabi::FrameLocal<sead::SafeText> queuedName(0x18);
        gabi::FrameLocal<sead::SafeText> queuedLabel(0x20);
        gabi::FrameLocal<sead::SafeText> queuedHelp(0x48);
        queuedName->mVtable = &sParameterTextTable;
        queuedLabel->mVtable = &sParameterTextTable;
        queuedName->mText = &sQueuedName.mFirst;
        queuedLabel->mText = &sQueuedName.mFirst;
        constructEntryParameter(&entry->mQueued);
        entry->mQueued.mVtable = gabi::ea(&sByteParameterTable);
        queuedHelp->mVtable = &sParameterTextTable;
        queuedHelp->mText = &sEmptyHelp.mFirst;
        attachEntryParameter(&entry->mQueued, queuedName.get(), queuedLabel.get(), queuedHelp.get(), entry->mParameterContainer);
        entry->mQueued.mValue = 0;
        gabi::FrameLocal<sead::SafeText> indexName(0x28);
        gabi::FrameLocal<sead::SafeText> indexLabel(0x30);
        gabi::FrameLocal<sead::SafeText> indexHelp(0x50);
        indexName->mVtable = &sParameterTextTable;
        indexLabel->mVtable = &sParameterTextTable;
        indexName->mText = &sIndexName.mFirst;
        indexLabel->mText = &sIndexName.mFirst;
        constructEntryParameter(&entry->mIndex);
        entry->mIndex.mVtable = gabi::ea(&sWordParameterTable);
        indexHelp->mVtable = &sParameterTextTable;
        indexHelp->mText = &sIndexHelp.mFirst;
        attachEntryParameter(&entry->mIndex, indexName.get(), indexLabel.get(), indexHelp.get(), entry->mParameterContainer);
        entry->mIndex.mValue = 0xFFFFFFFF;
        gabi::FrameLocal<sead::SafeText> label(0x38);
        label->mVtable = &sParameterTextTable;
        label->mText = &sEntryLabel.mFirst;
        associateEntryContainer(&entry->mNameObject, entry->mParameterContainer, label.get());
    }
    return entry;
}
/* WWHD glue
 data sInitialEntryTable 101685CC
 data sFinalEntryTable 10168BA4
 data sBaseEntryMethods 10168B5C
 data sParameterTextTable 1016858C
 data sByteParameterTable 10168664
 data sWordParameterTable 101686DC
 data sIdleName 10168844
 data sIdleLabel 1016884C
 data sQueuedName 10168854
 data sIndexName 10168868
 data sEntryLabel 1016885C
 data sEmptyHelp 10168588
 data sIndexHelp 10168589
 ext 0279CA34 void constructEntryName(agl::EntryNameObject_l*)
 ext 027A84B8 void constructEntryContainer(void*)
 ext 0279E674 void constructEntryParameter(agl::ParameterHeader*)
 ext 0279E618 void attachEntryParameter(agl::ParameterHeader*, const sead::SafeText*, const sead::SafeText*, const sead::SafeText*, void*)
 ext 0279CB9C void associateEntryContainer(agl::EntryNameObject_l*, void*, const sead::SafeText*)
*/
