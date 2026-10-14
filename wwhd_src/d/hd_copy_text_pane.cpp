// Copy text pane construction, resource binding and initialization.
// HD-only: reconstructed from the original executable; equivalence gates pending.
#include "d/hd_copy_text_pane.h"
using namespace gabi;
extern void* copyAllocate(u32);
extern void copyPaneConstruct(CopyTextPane_l*, void*, u32, u32);
extern void copyPaneCopyConstruct(CopyTextPane_l*, CopyTextPane_l*);
extern void copyMessageConstruct(void*);
extern CopyFont_l* copyGetFont(CopyTextPane_l*);
static u8 sCopyVTable;
static u8 sCopyStringVTable;
static u8 sCopyBufferedStringVTable;
static u8 sCopyFixedStringVTable;

/* inlined into CopyTextPane constructors */
static void constructCopyString(CopyTextPane_l* pane) {
 CopyString_l* string = &pane->mCopyName;
 if (!ea(string)) string = static_cast<CopyString_l*>(copyAllocate(0x10C));
 if (string) {
  CopyString_l* header = string;
  if (!header) header = static_cast<CopyString_l*>(copyAllocate(0xC));
  if (header) {
   header->mpText = reinterpret_cast<char*>(string->mBytes);
   header->mpVTable = at<gfn>(ea(&sCopyStringVTable));
   header->mCapacity = 256;
   string->mBytes[255] = 0;
  }
  char* buffer = string->mpText.get();
  string->mpVTable = at<gfn>(ea(&sCopyBufferedStringVTable));
  *reinterpret_cast<be<u8>*>(buffer) = 0;
  string->mpVTable = at<gfn>(ea(&sCopyFixedStringVTable));
 }
 pane->mpArray = nullptr;
 pane->mArrayCount = 0;
 copyMessageConstruct(pane->mMessage);
 pane->mScaleWidth = 1.0f;
 pane->mScaleHeight = 1.0f;
 *reinterpret_cast<be<u8>*>(pane->mCopyName.mpText.get()) = 0;
}
/* 027030FC */
CopyTextPane_l* CopyTextPane_l::construct(CopyTextPane_l* pane, void* layout, u32 mode, u32 flags) {
 if (!pane) pane = static_cast<CopyTextPane_l*>(copyAllocate(0xB68));
 if (pane) {
  copyPaneConstruct(pane, layout, mode, flags);
  pane->mpVTable = at<gfn>(ea(&sCopyVTable));
  pane->mpOwner = nullptr;
  pane->mSavedColor = -1;
  pane->mHeightScale = 0.0f;
  pane->mScalable = 0;
  pane->mOriginalWidth.mBits = 0;
  pane->mpText = nullptr;
  pane->mOriginalHeight.mBits = 0;
  pane->mWidthScale = 0.0f;
  pane->mpFont = nullptr;
  constructCopyString(pane);
 }
 return pane;
}
/* 0270324C */
CopyTextPane_l* CopyTextPane_l::copyConstruct(CopyTextPane_l* pane, CopyTextPane_l* source) {
 if (!pane) pane = static_cast<CopyTextPane_l*>(copyAllocate(0xB68));
 if (pane) {
  copyPaneCopyConstruct(pane, source);
  pane->mpVTable = at<gfn>(ea(&sCopyVTable));
  pane->mOriginalWidth.mValue = f32(source->mOriginalWidth.mValue);
  // GHS loads the height before clearing fields that may overlap the source.
  f32 originalHeight = source->mOriginalHeight.mValue;
  pane->mWidthScale = 0.0f;
  pane->mHeightScale = 0.0f;
  pane->mpFont = nullptr;
  pane->mOriginalHeight.mValue = originalHeight;
  pane->mpText = nullptr;
  pane->mSavedColor = -1;
  u8 scalable = source->mScalable;
  pane->mpOwner = nullptr;
  pane->mScalable = scalable;
  constructCopyString(pane);
 }
 return pane;
}


static u8 sCopySafeStringVTable;
extern void copyEnsureString(hdUiSafeString*);
extern void copyBytes(char*, char*, s32, u32);
extern void copyFormat(CopyString_l*, const char*, char*, be<u8>*);
extern void* copyLayoutAccessor(CopyTextPane_l*);
extern void* copyPaneAccessor(CopyTextPane_l*);
extern CopyAttribute_l* copyFindAttribute(char*, void*, void*, u32);
extern void copyRegister(PartRegistration_l*);
using CopyWord_l = be<u32>;
static CopyWord_l sCopyAttributeGuard;
static hdUiSafeString sCopyAttributeName;
static PartRegistration_l sCopyAttributeRegistration;
static CopyWord_l sCopyFallbackGuard;
static hdUiSafeString sCopyFallbackName;
static PartRegistration_l sCopyFallbackRegistration;
/* inlined into CopyTextPane name setters */
static void copyBoundedName(CopyTextPane_l* pane, hdUiSafeString* source) {
 char* output = pane->mCopyName.mpText.get();
 copyEnsureString(source);
 char* chars = reinterpret_cast<char*>(source->mText.get());
 s32 length = 0;
 while (*reinterpret_cast<be<u8>*>(chars)) {
  ++length;
  ++chars;
  if (length > 0x40000) { length = 0; break; }
 }
 s32 capacity = pane->mCopyName.mCapacity;
 if (length >= capacity) length = capacity - 1;
 call_ptr<void>(source->mVtable->mAssureTermination.get(), source);
 copyBytes(output, reinterpret_cast<char*>(source->mText.get()), length, 0);
 *at<be<u8>>(ea(output)+u32(length)) = 0;
}
/* 02703390 */
void CopyTextPane_l::setCopyName(CopyResource_l* resource) {
    gabi::AutoFrame automatic{gabi::Activation::current};
 CopyOwner_l* owner = mpOwner.get();
 CopyResource_l* original = owner->mpResource.get();
 if (u32(original->mFlags)) {
  copyFormat(&mCopyName, "%s-%s", reinterpret_cast<char*>(&original->mNameOffset), mName);
 } else {
  FrameLocal<hdUiSafeString> name(8);
  name->mText = reinterpret_cast<be<u8>*>(mName);
  name->mVtable = at<hdUiStringTable>(ea(&sCopySafeStringVTable));
  copyBoundedName(this, name.get());
 }
 u32 offset = resource->mNameOffset;
 if (offset && ea(resource)+offset) {
  FrameLocal<hdUiSafeString> name(0x10);
  name->mText = at<be<u8>>(ea(resource)+offset);
  name->mVtable = at<hdUiStringTable>(ea(&sCopySafeStringVTable));
  copyBoundedName(this, name.get());
 }
}


static CopyWord_l sCopyOwnerGuard;
static hdUiAdaptorType sCopyOwnerType;
static CopyWord_l sCopyOwnerBaseGuard;
static hdUiAdaptorType sCopyOwnerBaseType;
static CopyWord_l sCopyPaneGuard;
static hdUiAdaptorType sCopyPaneType;
static CopyWord_l sCopyPaneBaseGuard;
static hdUiAdaptorType sCopyPaneBaseType;
static CopyWord_l sCopyPaneAncestorGuard;
static hdUiAdaptorType sCopyPaneAncestorType;
static CopyWord_l sCopyRootGuard;
static hdUiAdaptorType sCopyRootType;
/* 02703538 */
void CopyTextPane_l::initialize(CopyResource_l* resource, CopyResource_l* overrideResource, CopyContext_l* context) {
 if (overrideResource) resource = overrideResource;
 mOriginalWidth.mBits = mFontWidth;
 mOriginalHeight.mBits = mFontHeight;
 mpFont = copyGetFont(this);
 mSavedColor = mColor;
 f32 width = *reinterpret_cast<be<f32>*>(&mFontWidth);
 f32 height = *reinterpret_cast<be<f32>*>(&mFontHeight);
 mScaleWidth = f32(mOriginalWidth.mValue);
 CopyFont_l* font = copyGetFont(this);
 if (font) {
  s32 fontWidth = call_ptr<s32>(font->mpVTable[0x1C/4].get(), font);
  mWidthScale = width / f32(fontWidth);
  s32 fontHeight = call_ptr<s32>(font->mpVTable[0x24/4].get(), font);
  mHeightScale = height / f32(fontHeight);
 }
 mScaleHeight = mWidthScale;
 CopyOwner_l* owner = context->mpOwner.get();
 if (!sCopyOwnerGuard) {
  u32 base = sCopyOwnerBaseGuard;
  sCopyOwnerGuard = 1;
  if (!base) { sCopyOwnerBaseGuard = 1; sCopyOwnerBaseType.mParent = nullptr; }
  sCopyOwnerType.mParent = &sCopyOwnerBaseType;
 }
 bool matches = false;
 if (owner) {
  hdUiAdaptorType* type = call_ptr<hdUiAdaptorType*>(owner->mpTypeVTable[0xC/4].get(), owner);
  while (type && type != &sCopyOwnerType) type = type->mParent.get();
  matches = type != nullptr;
 }
 mpOwner = matches ? owner : nullptr;
 setCopyName(resource);
 if (resource->mCount >= 2) {
  mpArray = at<void>(ea(resource)+u32(resource->mArrayOffset));
  mArrayCount = resource->mCount;
 }
}
/* inlined into CopyTextPane resource copy */
static CopyAttribute_l* lookupCopyName(CopyTextPane_l* pane, hdUiSafeString* name) {
 call_ptr<void>(name->mVtable->mAssureTermination.get(), name);
 char* key = reinterpret_cast<char*>(name->mText.get());
 void* layout = copyLayoutAccessor(pane);
 void* target = copyPaneAccessor(pane);
 return copyFindAttribute(key, layout, target, 0);
}
/* 02703764 */
void CopyTextPane_l::copyFromResource() {
 CopyOwner_l* owner = mpOwner.get();
 CopyResource_l* resource = owner->mpResource.get();
 if (u32(resource->mFlags)) return;
 if (!sCopyAttributeGuard) {
  sCopyAttributeGuard = 1;
  sCopyAttributeName.mVtable = at<hdUiStringTable>(ea(&sCopySafeStringVTable));
  sCopyAttributeName.mText = at<be<u8>>(gabi::guest_addr("Copy"));
  copyRegister(&sCopyAttributeRegistration);
 }
 CopyAttribute_l* attribute = lookupCopyName(this, &sCopyAttributeName);
 if (!attribute) {
  if (!sCopyFallbackGuard) {
   sCopyFallbackGuard = 1;
   sCopyFallbackName.mVtable = at<hdUiStringTable>(ea(&sCopySafeStringVTable));
   sCopyFallbackName.mText = at<be<u8>>(gabi::guest_addr("copy"));
   copyRegister(&sCopyFallbackRegistration);
  }
  attribute = lookupCopyName(this, &sCopyFallbackName);
 }
 if (!attribute) return;
 u32 valueAddress = ea(attribute)+u32(attribute->mValueOffset);
 if (!valueAddress) return;
 resource = mpOwner.get()->mpResource.get();
 struct ResourceLookup_l { u8 mReserved[8]; gptr<gfn> mpVTable; };
 auto* lookup = reinterpret_cast<ResourceLookup_l*>(resource);
 CopyTextPane_l* source = call_ptr<CopyTextPane_l*>(lookup->mpVTable[0x5C/4].get(), lookup, at<void>(valueAddress), 1u);
 if (!sCopyPaneGuard) {
  u32 base = sCopyPaneBaseGuard;
  sCopyPaneGuard = 1;
  if (!base) {
   u32 ancestor = sCopyPaneAncestorGuard;
   sCopyPaneBaseGuard = 1;
   if (!ancestor) {
    u32 root = sCopyRootGuard;
    sCopyPaneAncestorGuard = 1;
    if (!root) { sCopyRootType.mParent = nullptr; sCopyRootGuard = 1; }
    sCopyPaneAncestorType.mParent = &sCopyRootType;
   }
   sCopyPaneBaseType.mParent = &sCopyPaneAncestorType;
  }
  sCopyPaneType.mParent = &sCopyPaneBaseType;
 }
 if (!source) return;
 auto* pane = reinterpret_cast<CopyPaneTypeView_l*>(source);
 hdUiAdaptorType* type = call_ptr<hdUiAdaptorType*>(pane->mpTypeVTable[0x14/4].get(), pane);
 while (type && type != &sCopyPaneType) type = type->mParent.get();
 if (!type) return;
 char* output = mCopyName.mpText.get();
 CopyString_l* string = &source->mCopyName;
 call_ptr<void>(string->mpVTable[0x14/4].get(), string);
 char* chars = string->mpText.get();
 s32 length = 0;
 while (*reinterpret_cast<be<u8>*>(chars)) {
  ++length; ++chars;
  if (length > 0x40000) { length = 0; break; }
 }
 s32 capacity = mCopyName.mCapacity;
 if (length >= capacity) length = capacity - 1;
 call_ptr<void>(string->mpVTable[0x14/4].get(), string);
 copyBytes(output, string->mpText.get(), length, 0);
 *at<be<u8>>(ea(output)+u32(length)) = 0;
 mpArray = source->mpArray.get();
 mArrayCount = source->mArrayCount;
}
/* WWHD glue (tools/verify/autoglue.py)
ext 0273AD10 void* copyAllocate(u32)
ext 020021FC void copyPaneConstruct(CopyTextPane_l*,void*,u32,u32)
ext 02002280 void copyPaneCopyConstruct(CopyTextPane_l*,CopyTextPane_l*)
ext 025F65F4 void copyMessageConstruct(void*)
ext 02878098 CopyFont_l* copyGetFont(CopyTextPane_l*)
data sCopyVTable 1010BE80
data sCopyStringVTable 1010BE08
data sCopyBufferedStringVTable 1010BE38
data sCopyFixedStringVTable 1010BE50
data sCopySafeStringVTable 1010BDF0
ext 02704BAC void copyEnsureString(hdUiSafeString*)
ext C0009988 void copyBytes(char*,char*,s32,u32)
ext 02759C28 void copyFormat(CopyString_l*,const char*,char*,be<u8>*)
str 1010BF5C "%s-%s"
ext 028764B8 void* copyLayoutAccessor(CopyTextPane_l*)
ext 028764A0 void* copyPaneAccessor(CopyTextPane_l*)
ext 02705F7C CopyAttribute_l* copyFindAttribute(char*,void*,void*,u32)
ext 028F026C void copyRegister(PartRegistration_l*)
data sCopyAttributeGuard 1049E7A8
data sCopyAttributeName 1049E774
data sCopyAttributeRegistration 101F73A8
data sCopyFallbackGuard 1049E7AC
data sCopyFallbackName 1049E77C
data sCopyFallbackRegistration 101F73B4
data sCopyOwnerGuard 101FD8C4
data sCopyOwnerType 101FD900
data sCopyOwnerBaseGuard 101FD894
data sCopyOwnerBaseType 101FD904
data sCopyPaneGuard 101FDD40
data sCopyPaneType 101FDD3C
data sCopyPaneBaseGuard 101FD8F0
data sCopyPaneBaseType 101FD8F4
data sCopyPaneAncestorGuard 101FD89C
data sCopyPaneAncestorType 101FD8F8
data sCopyRootGuard 101FD888
data sCopyRootType 101FD8FC
str 1010BF70 "Copy" in 02703764
str 1010BF78 "copy" in 02703764
*/
