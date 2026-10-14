// Text-pane scaling from layout attributes.
// HD-only: reconstructed from the original executable; equivalence gates pending.
#include "d/hd_parts_scalable.h"
using namespace gabi;
extern void* copyAllocate(u32);
extern void copyPaneConstruct(CopyTextPane_l*, void*, u32, u32);
extern void copyPaneCopyConstruct(CopyTextPane_l*, CopyTextPane_l*);
extern void copyMessageConstruct(void*);
extern CopyFont_l* copyGetFont(CopyTextPane_l*);
static u8 sScalableSafeStringVTable;
extern void copyEnsureString(hdUiSafeString*);
extern void copyBytes(char*, char*, s32, u32);
extern void copyFormat(CopyString_l*, const char*, char*, be<u8>*);
extern void* copyLayoutAccessor(CopyTextPane_l*);
extern void* copyPaneAccessor(CopyTextPane_l*);
extern CopyAttribute_l* copyFindAttribute(char*, void*, void*, u32);
extern void copyRegister(PartRegistration_l*);
using CopyWord_l = be<u32>;
static CopyWord_l sCopyScaleGuard;
static hdUiSafeString sCopyScaleName;
static PartRegistration_l sCopyScaleRegistration;
/* 02703B00 */
void CopyTextPane_l::applyScale() {
 if (!sCopyScaleGuard) {
  sCopyScaleGuard = 1;
  sCopyScaleName.mText = at<be<u8>>(gabi::guest_addr("Scalable"));
  sCopyScaleName.mVtable = at<hdUiStringTable>(ea(&sScalableSafeStringVTable));
  copyRegister(&sCopyScaleRegistration);
 }
 call_ptr<void>(sCopyScaleName.mVtable->mAssureTermination.get(), &sCopyScaleName);
 char* key = reinterpret_cast<char*>(sCopyScaleName.mText.get());
 void* layout = copyLayoutAccessor(this);
 void* pane = copyPaneAccessor(this);
 CopyAttribute_l* attribute = copyFindAttribute(key, layout, pane, 2);
 if (attribute) {
  mScalable = 1;
  f32 factor = *at<be<f32>>(ea(attribute)+u32(attribute->mValueOffset));
  f32 width = f32(mOriginalWidth.mValue);
  mScaleWidth = fmuls_ppc(width, factor) / 100.0f;
  factor = *at<be<f32>>(ea(attribute)+u32(attribute->mValueOffset));
  mScaleHeight = fmuls_ppc(mWidthScale, factor) / 100.0f;
 }
}
/* WWHD glue (tools/verify/autoglue.py)
data sScalableSafeStringVTable 1010BDF0
data sCopyScaleGuard 1049E7B0
data sCopyScaleName 1049E784
data sCopyScaleRegistration 101F73C0
str 1010BF84 "Scalable" in 02703B00
*/
