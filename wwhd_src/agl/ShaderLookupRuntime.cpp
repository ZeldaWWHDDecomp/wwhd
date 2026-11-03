// Relative shader lookup views established from this unit's load/store offsets.
#include "wwhd.h"
#include "sead/FileDevice.h"
// Guest EA -> typed view (address zero stays null).
template <typename Target>
static Target* shaderLookupGuest(u32 address) {
    return gabi::at<Target>(address);
}
struct ShaderLookupRecord {
    u8 mBeforeCount[0x12]; be<u16> mCount; u8 mBeforeIndex[4]; be<u16> mSelectedIndex;
    u8 mBeforePair[0xA]; be<u32> mRelativePair; u8 mBeforeEntries[0xC]; be<u32> mRelativeEntries;
};
struct ShaderLookupEntry { be<u32> mWord,mResolved; u8 mBeforeName[8]; be<u32> mRelativeName; };
struct ShaderLookupPair { gptr<void> mFirst,mSecond; };
struct ShaderLookupArchivePairs { u8 mPrefix[0x34]; ShaderLookupPair mPair; };
struct ShaderLookupNameTable { u8 mPrefix[0x14]; be<u32> mFinishCallback; };
struct ShaderLookupPrimaryName { u8 mPrefix[0x18]; gptr<void> mText; };
struct ShaderLookupFallbackName { u8 mPrefix[0x14]; gptr<void> mText; };
struct ShaderLookupOwner { u8 mPrefix[0x7C]; gptr<void> mArchive; };
struct ShaderLookupValue { be<u32> mWord,mValue; };
static sead::SafeTextVtable sLookupNameTable;
ShaderLookupValue* findShaderLookupValue(void*,void*);
/* 027731C8 */
void resolveShaderLookupNames(ShaderLookupRecord* record,const ShaderLookupOwner* owner,void* selection) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<ShaderLookupPair> pair(0x10);
    auto* source=shaderLookupGuest<ShaderLookupPair>(gabi::ea(owner->mArchive.get())+offsetof(ShaderLookupArchivePairs,mPair));
    pair->mFirst=source->mFirst.get();pair->mSecond=source->mSecond.get();
    if(pair->mFirst.get()==nullptr||pair->mSecond.get()==nullptr) return;
    auto* selected=findShaderLookupValue(&pair->mSecond,selection);
    if(selected==nullptr) return;
    u32 count=record->mCount;record->mSelectedIndex=(u32)selected->mValue;
    u32 offset=0;
    while(count!=0) {
        const u32 relative=record->mRelativeEntries;
        const u32 entries=relative?gabi::ea(&record->mRelativeEntries)+relative:0;
        auto* entry=shaderLookupGuest<ShaderLookupEntry>(entries+offset);
        const u32 relativeName=entry->mRelativeName;
        gabi::FrameLocal<sead::SafeText> name(8);
        name->mText=shaderLookupGuest<be<u8>>(relativeName?gabi::ea(&entry->mRelativeName)+relativeName:0);
        name->mVtable=&sLookupNameTable;
        auto* found=findShaderLookupValue(pair.get(),name.get());
        if(found!=nullptr) entry->mResolved=(u32)found->mValue;
        --count;offset+=0x14;
    }
}
struct ShaderLookupCollection { u8 mBeforeEntries[0x20]; be<u32> mRelativeEntries; u8 mBeforeCount[0x2C]; be<u16> mCount; };
struct ShaderLookupCollectionEntry { u8 mBeforeRecord[0x24]; be<u32> mRelativeRecord; };
void resolveShaderLookupRecord(void*,void*,void*);
/* 027735DC */
void resolveShaderLookupCollection(const ShaderLookupCollection* collection,void* owner,void* selection) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    u32 count=collection->mCount;u32 offset=0;
    while(count!=0) {
        const u32 relative=collection->mRelativeEntries;
        const u32 entries=relative?gabi::ea(&collection->mRelativeEntries)+relative:0;
        auto* entry=shaderLookupGuest<ShaderLookupCollectionEntry>(entries+offset);
        const u32 relativeRecord=entry->mRelativeRecord;
        void* record=shaderLookupGuest<void>(relativeRecord?gabi::ea(&entry->mRelativeRecord)+relativeRecord:0);
        resolveShaderLookupRecord(record,owner,selection);
        --count;offset+=0x10;
    }
}
// A resource texture's runtime state (0xBC bytes; 02773680 builds it, 027E1F5C finishes it). The GX2 texture at +4
// is a GX2Surface (copied as 0x74 bytes from the resource) followed by its view, component selection and registers.
struct ResTextureSurface {
    be<u32> mDimension,mWidth,mHeight,mDepth,mMipCount,mFormat,mAntiAlias,mUsage;
    be<u32> mImageSize,mImage,mMipSize,mMipmaps;
    be<u32> mTileMode,mSwizzle,mAlignment,mPitch,mMipOffsets[13];
};
struct ResTextureState {
    be<u32> mHeader;
    ResTextureSurface mSurface;
    be<u32> mViewFirstMip,mViewMipCount,mViewFirstSlice,mViewSliceCount,mComponentSelect;
    be<u32> mRegisters[5];
    be<u32> mHandle;            // +0xA0, cleared first (role not read in this unit)
    be<u32> mUnreadA4;          // +0xA4, not written here
    be<u32> mCache[2];          // +0xA8/+0xAC, cleared
    be<s32> mImageOffset;       // +0xB0, self-relative offset of the image (0 = none)
    be<s32> mMipOffset;         // +0xB4, self-relative offset of the mip chain (0 = none)
    be<u32> mTail;              // +0xB8, cleared
};
struct ShaderLookupCodeTable { be<u32> mCodes[64]; };
static ShaderLookupCodeTable sLookupCodeTable;
void* copyShaderLookupWords(void*,const void*,u32,u32);
void prepareShaderLookupCommands(void*);
void finishShaderLookupState(ResTextureState*);
/* 02773680 */
ResTextureState* constructShaderLookupState(ResTextureState* state,const ResTextureState* source) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    state->mHandle=0;
    copyShaderLookupWords(&state->mSurface,&source->mSurface,sizeof(ResTextureSurface),0);
    state->mCache[0]=0;state->mCache[1]=0;state->mTail=0;
    const u32 image=source->mSurface.mImage;
    state->mImageOffset=image?image-gabi::ea(&state->mImageOffset):0;
    const u32 mipmaps=source->mSurface.mMipmaps;
    state->mMipOffset=mipmaps?mipmaps-gabi::ea(&state->mMipOffset):0;
    const u32 format=state->mSurface.mFormat;
    const u32 componentSelect=sLookupCodeTable.mCodes[format&63];
    state->mViewFirstMip=0;state->mComponentSelect=componentSelect;
    const u32 mipCount=state->mSurface.mMipCount;state->mViewFirstSlice=0;state->mViewMipCount=mipCount;
    state->mViewSliceCount=(u32)state->mSurface.mDepth;
    prepareShaderLookupCommands(&state->mSurface);finishShaderLookupState(state);
    return state;
}
struct ShaderLookupNestedList { u8 mPrefix[0x18]; be<u32> mRelativeEntries; u8 mBeforeCount[8]; be<u16> mCount; };
struct ShaderLookupRelativePair { be<u32> mFirst,mSecond; };
struct ShaderLookupOwnerList { u8 mPrefix[0x14]; gptr<void> mPrimary,mSecondary; be<s32> mCount; gptr<void> mEntries; };
static void finishLookupName(sead::SafeText* name) {
    const u32 callback=*shaderLookupGuest<be<u32>>(gabi::ea(name->mVtable.get())+offsetof(ShaderLookupNameTable,mFinishCallback));
    gabi::call_ptr<void>(callback,name);
}
void prepareLookupNamePair(sead::SafeText*);
static bool equalLookupText(u32 first,u32 second) {
    if(first==second) return true;
    for(u32 remaining=0x40001;remaining!=0;--remaining) {
        const u8 a=*shaderLookupGuest<be<u8>>(first),b=*shaderLookupGuest<be<u8>>(second);
        if(a!=b) return false;
        if(a==0) return true;
        ++first;++second;
    }
    return false;
}
/* 027732B0 */
void resolveShaderLookupRecord(void* recordAddress,void* ownerAddress,void* selection) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    auto* list=static_cast<ShaderLookupNestedList*>(recordAddress);
    auto* owner=static_cast<ShaderLookupOwnerList*>(ownerAddress);
    u32 count=list->mCount,offset=0;
    while(count!=0) {
        const u32 relative=list->mRelativeEntries;
        const u32 entries=relative?gabi::ea(&list->mRelativeEntries)+relative:0;
        auto* outer=shaderLookupGuest<ShaderLookupCollectionEntry>(entries+offset);
        const u32 recordRelative=outer->mRelativeRecord;
        auto* record=shaderLookupGuest<ShaderLookupRecord>(recordRelative?gabi::ea(&outer->mRelativeRecord)+recordRelative:0);
        auto* pairSlot=shaderLookupGuest<be<u32>>(gabi::ea(record)+offsetof(ShaderLookupRecord,mRelativePair));
        const u32 pairRelative=*pairSlot;
        u32 pairAddress=pairRelative?gabi::ea(pairSlot)+pairRelative:0;
        bool matches=false;
        if(pairAddress!=0) {
            auto* pair=shaderLookupGuest<ShaderLookupRelativePair>(pairAddress);
            const u32 first=pair->mFirst;
            const u32 firstTextAddress=gabi::ea(&pair->mFirst)+first;
            const u32 second=(first!=0&&firstTextAddress!=0)?(u32)pair->mSecond:0;
            const u32 secondTextAddress=gabi::ea(&pair->mSecond)+second;
            if(first!=0&&firstTextAddress!=0&&second!=0&&secondTextAddress!=0) {
                gabi::FrameLocal<sead::SafeText> ownerName(0x10),recordName(0x18);
                const u32 primary=gabi::ea(owner->mPrimary.get());
                ownerName->mText=shaderLookupGuest<be<u8>>(primary?primary+offsetof(ShaderLookupPrimaryName,mText):gabi::ea(owner->mSecondary.get())+offsetof(ShaderLookupFallbackName,mText));
                ownerName->mVtable=&sLookupNameTable;
                const u32 reloadRelative=*pairSlot;
                const u32 reloadPair=reloadRelative?gabi::ea(pairSlot)+reloadRelative:0;
                auto* reloadFirst=shaderLookupGuest<be<u32>>(reloadPair+offsetof(ShaderLookupRelativePair,mFirst));
                const u32 textRelative=*reloadFirst;
                recordName->mText=shaderLookupGuest<be<u8>>(textRelative?gabi::ea(reloadFirst)+textRelative:0);
                recordName->mVtable=&sLookupNameTable;
                prepareLookupNamePair(ownerName.get());
                finishLookupName(ownerName.get());
                const u32 firstText=gabi::ea(ownerName->mText.get());
                finishLookupName(recordName.get());
                matches=firstText==gabi::ea(recordName->mText.get())||equalLookupText(gabi::ea(ownerName->mText.get()),gabi::ea(recordName->mText.get()));
            }
        }
        if(matches) {
            s32 remaining=owner->mCount;u32 index=0,entryOffset=0;
            while(remaining>0) {
                u32 row=gabi::ea(owner->mEntries.get());
                if(index<(u32)owner->mCount) row+=entryOffset;
                auto* entryOwner=shaderLookupGuest<ShaderLookupOwner>(row);
                auto* shaderName=shaderLookupGuest<sead::SafeText>(gabi::ea(entryOwner->mArchive.get()));
                const u32 reloadRelative=*pairSlot;
                const u32 reloadPair=reloadRelative?gabi::ea(pairSlot)+reloadRelative:0;
                auto* reloadSecond=shaderLookupGuest<be<u32>>(reloadPair+offsetof(ShaderLookupRelativePair,mSecond));
                const u32 textRelative=*reloadSecond;
                gabi::FrameLocal<sead::SafeText> sought(8);
                sought->mText=shaderLookupGuest<be<u8>>(textRelative?gabi::ea(reloadSecond)+textRelative:0);
                sought->mVtable=&sLookupNameTable;
                finishLookupName(shaderName);finishLookupName(shaderName);
                const u32 firstText=gabi::ea(shaderName->mText.get());
                finishLookupName(sought.get());
                if(firstText==gabi::ea(sought->mText.get())||equalLookupText(gabi::ea(shaderName->mText.get()),gabi::ea(sought->mText.get()))) {
                    ShaderLookupOwner* selected=nullptr;
                    if(index<(u32)owner->mCount) selected=shaderLookupGuest<ShaderLookupOwner>(gabi::ea(owner->mEntries.get())+entryOffset);
                    resolveShaderLookupNames(record,selected,selection);
                    break;
                }
                --remaining;++index;entryOffset+=0x84;
            }
        }
        --count;offset+=0x10;
    }
}
static_assert(offsetof(ShaderLookupRecord,mCount)==0x12);
static_assert(offsetof(ShaderLookupRecord,mSelectedIndex)==0x18);
static_assert(offsetof(ShaderLookupRecord,mRelativePair)==0x24);
static_assert(offsetof(ShaderLookupRecord,mRelativeEntries)==0x34);
static_assert(sizeof(ShaderLookupRecord)==0x38);
static_assert(offsetof(ShaderLookupEntry,mRelativeName)==0x10);
static_assert(sizeof(ShaderLookupEntry)==0x14);
static_assert(offsetof(ShaderLookupCollection,mRelativeEntries)==0x20);
static_assert(offsetof(ShaderLookupArchivePairs,mPair)==0x34);
static_assert(offsetof(ShaderLookupNameTable,mFinishCallback)==0x14);
static_assert(offsetof(ShaderLookupPrimaryName,mText)==0x18);
static_assert(offsetof(ShaderLookupFallbackName,mText)==0x14);
static_assert(offsetof(ShaderLookupRelativePair,mSecond)==4);
static_assert(offsetof(ShaderLookupOwner,mArchive)==0x7C);
static_assert(offsetof(ShaderLookupCollection,mCount)==0x50);
static_assert(offsetof(ShaderLookupCollectionEntry,mRelativeRecord)==0x24);
static_assert(offsetof(ShaderLookupNestedList,mRelativeEntries)==0x18);
static_assert(offsetof(ShaderLookupNestedList,mCount)==0x24);
static_assert(sizeof(ResTextureSurface)==0x74);
static_assert(offsetof(ResTextureState,mViewFirstMip)==0x78);
static_assert(offsetof(ResTextureState,mComponentSelect)==0x88);
static_assert(offsetof(ResTextureState,mHandle)==0xA0);
static_assert(offsetof(ResTextureState,mImageOffset)==0xB0);
static_assert(offsetof(ResTextureState,mMipOffset)==0xB4);
static_assert(offsetof(ShaderLookupOwnerList,mEntries)==0x20);
static_assert(sizeof(ResTextureState)==0xBC);
/* WWHD glue
ext 0277367C void prepareLookupNamePair(sead::SafeText*)
ext C0009988 void* copyShaderLookupWords(void*,const void*,u32,u32)
ext 027E14AC void prepareShaderLookupCommands(void*)
ext 027E1F5C void finishShaderLookupState(ResTextureState*)
data sLookupCodeTable 101616E4
ext 027BC540 ShaderLookupValue* findShaderLookupValue(void*,void*)
data sLookupNameTable 101616CC
*/
