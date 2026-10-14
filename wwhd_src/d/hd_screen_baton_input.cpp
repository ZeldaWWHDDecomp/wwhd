/* HD baton input factories and timing point. Existing metronome code stays in
 * d_metronome.cpp; the one-time framework base uses its merged canonical header. */
#include "d/hd_screen_baton_input.h"
#include "d/hd_screen_baton_completion_point.h"
#include "d/hd_screen_baton_sign.h"

extern void* batonAllocate(u32);
extern void batonDelete(void*);
extern void batonRegisterStatic(void*);
extern void batonConstructMinimum(void*);
extern void batonConstructMaximum(void*);

struct BatonUiSystemSlot { gptr<hdScreenUiSystem> mpSystem; };
static BatonUiSystemSlot sBatonUiSystem;
static u8 sBatonPointVtable;
static u8 sBatonPointHolderVtable;
static u8 sBatonPointSecondaryVtable;
static u8 sBatonSafeStringVtable;
static hdUiSafeString sBatonPointPaneName;
static hdUiSafeString sBatonPointResourceName;
struct BatonPointStatics {
    be<f32> mMinimum, mMaximum;
    be<u8> mMinimumConstructed, mMaximumConstructed;
    u8 mPadding[2];
    be<u32> mWords[4];
};
WWHD_OFFSET(BatonPointStatics, mMinimumConstructed, 8);
WWHD_OFFSET(BatonPointStatics, mWords, 0xC);
static BatonPointStatics sBatonPointStatics;
static u8 sBatonPointDestructor;
static u8 sBatonMinimumDestructor;
static u8 sBatonMaximumDestructor;
static f32 sBatonMinimumAngle;
static f32 sBatonMaximumAngle;

/* 0261D7D0 */
u32 hdBatonInputStateKey(hdPartsStateId_c* state) { return u32(state->mNumber); }

/* 026200A8 */
hdBatonInputPoint_c* hdBatonCreateTimingPane(u32 first, u32 second, void* descriptor) {
    auto* point = static_cast<hdBatonInputPoint_c*>(hdParts_new(sizeof(hdBatonInputPoint_c), sBatonUiSystem.mpSystem->mHeap, 4));
    if (point) point = hdBatonInputPoint_c::construct(point);
    if (!point) return nullptr;
    point->initialize(first, second, descriptor);
    return point;
}
/* 02620158 */
hdParts_c* hdBatonCreateNotePane(u32 first, u32 second, void* descriptor) {
    auto* point = static_cast<hdBatonCompletionPoint_c*>(hdParts_new(sizeof(hdBatonCompletionPoint_c), sBatonUiSystem.mpSystem->mHeap, 4));
    if (point) point = hdBatonCompletionPoint_c::construct(point);
    if (!point) return nullptr;
    point->load(first, second, descriptor);
    return point;
}
/* 02620208 */
hdParts_c* hdBatonCreateBeatPane(u32 first, u32 second, void* descriptor) {
    auto* point = static_cast<hdBatonSign_c*>(hdParts_new(sizeof(hdBatonSign_c), sBatonUiSystem.mpSystem->mHeap, 4));
    if (point) point = hdBatonSign_c::construct(point);
    if (!point) return nullptr;
    point->initialize(first, second, static_cast<hdPartsDesc_c*>(descriptor));
    return point;
}
/* 026202B8 */
void hdBatonInputPoint_c::destroy(hdBatonInputPoint_c* point, u32 flags) {
    if (point) {
        point->mStateHolder.mHolderVtable = &sBatonPointHolderVtable;
        point->mSecondaryVtable = &sBatonPointSecondaryVtable;
        point->OneTimePart_l::destroy(0);
        if (flags & 1) batonDelete(point);
    }
}
/* 02620324 */
hdBatonInputPoint_c* hdBatonInputPoint_c::construct(hdBatonInputPoint_c* point) {
    if (!point) point = static_cast<hdBatonInputPoint_c*>(batonAllocate(sizeof(hdBatonInputPoint_c)));
    if (point) {
        OneTimePart_l::construct(point);
        point->mVtable = reinterpret_cast<hdPartsVtbl_c*>(&sBatonPointVtable);
        point->mStateHolder.mHolderVtable = &sBatonPointHolderVtable;
        point->mSecondaryVtable = &sBatonPointSecondaryVtable;
    }
    return point;
}
/* 02620390 */
u32 hdBatonInputPoint_c::initialize(u32 first, u32 second, void* descriptor) {
    return OneTimePart_l::create(&sBatonPointResourceName, gabi::at<u8>(first), second, gabi::ea(descriptor));
}
/* 026203A8 */
void hdBatonInitializePointStatics() {
    sBatonPointStatics.mWords[2] = 0;
    sBatonPointStatics.mWords[0] = 0;
    sBatonPointStatics.mWords[3] = 0;
    sBatonPointStatics.mWords[1] = 0;
    batonRegisterStatic(&sBatonPointDestructor);
    sBatonPointStatics.mMaximum = sBatonMaximumAngle;
    sBatonPointStatics.mMinimum = sBatonMinimumAngle;
    batonConstructMinimum(&sBatonPointStatics.mMinimumConstructed);
    batonRegisterStatic(&sBatonMinimumDestructor);
    batonConstructMaximum(&sBatonPointStatics.mMaximumConstructed);
    batonRegisterStatic(&sBatonMaximumDestructor);
    sBatonPointPaneName.mVtable = reinterpret_cast<hdUiStringTable*>(&sBatonSafeStringVtable);
    sBatonPointPaneName.mText.v = gabi::guest_addr("G_All_00");
    sBatonPointResourceName.mVtable = reinterpret_cast<hdUiStringTable*>(&sBatonSafeStringVtable);
    sBatonPointResourceName.mText.v = gabi::guest_addr("BatonInputPoint_00");
}

/* WWHD glue (tools/verify/autoglue.py)
ext 0273AD10 void* batonAllocate(u32 size)
ext 0273AF40 void batonDelete(void* point)
ext 027025E0 OneTimePart_l* OneTimePart_l::construct(OneTimePart_l* point)
ext 02702574 void OneTimePart_l::destroy(u32 flags)
ext 02702674 u32 OneTimePart_l::create(void* name, void* archive, u32 first, u32 second)
ext 026204F8 hdBatonCompletionPoint_c* hdBatonCompletionPoint_c::construct(hdBatonCompletionPoint_c* point)
ext 02620564 u32 hdBatonCompletionPoint_c::load(u32 first, u32 second, void* descriptor)
ext 02620A58 hdBatonSign_c* hdBatonSign_c::construct(hdBatonSign_c* point)
ext 02620AD4 u32 hdBatonSign_c::initialize(u32 first, u32 second, hdPartsDesc_c* descriptor)
ext 028F026C void batonRegisterStatic(void* entry)
ext 028ED6F8 void batonConstructMinimum(void* flag)
ext 028EAB2C void batonConstructMaximum(void* flag)
data sBatonUiSystem 1018C404 nocheck
data sBatonPointVtable 100E3998 nocheck
data sBatonPointHolderVtable 100E3A60 nocheck
data sBatonPointSecondaryVtable 100E3A70 nocheck
data sBatonSafeStringVtable 100E394C nocheck
data sBatonPointPaneName 1048E58C nocheck
data sBatonPointResourceName 1048E5B0 nocheck
data sBatonPointStatics 1048E594 nocheck
data sBatonPointDestructor 101F51B8 nocheck
data sBatonMinimumDestructor 101F51C4 nocheck
data sBatonMaximumDestructor 101F51D0 nocheck
data sBatonMinimumAngle 100E396C nocheck
data sBatonMaximumAngle 100E3970 nocheck
str 100E3974 "G_All_00"
str 100E3980 "BatonInputPoint_00"
*/
