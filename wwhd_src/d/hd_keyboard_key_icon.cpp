// HD keyboard key-icon component. Private source.
#include "d/hd_keyboard_screen.h"
static u32 sKeyIconVtable;
static u32 sKeyIconDelegateA;
static u32 sKeyIconDelegateB;
static u32 sKeyIconStringVtable;
static hdKeyboardStatRange_c sKeyIconRange;
static hdKeyboardStatRegistrations_c sKeyIconRegistrations;
static const hdKeyboardStatLimits_c sKeyIconLimits;
static hdUiSafeString sKeyIconLayoutName;
extern void keyboardKeyIconConstruct(hdPartsKeyIcon_c*);
extern void keyboardKeyIconDestroy(hdPartsKeyIcon_c*, u32);
extern u32 keyboardKeyIconCreate(hdPartsKeyIcon_c*, hdUiSafeString*, u32, u32, u32);
/* 02673FF8 */
static hdScreenType* keyIconBaseType(hdScreenType* type) {
    return type->mParent;
}
/* 026740F8 */
void hdKeyboardKeyIcon_c::destroy(u32 flags) {
    if (!this)
        return;
    mBase.mStateHolder.mHolderVtable.v = gabi::ea(&sKeyIconDelegateA);
    mBase.mSecondaryVtable.v = gabi::ea(&sKeyIconDelegateB);
    keyboardKeyIconDestroy(&mBase, 0);
    if (flags & 1)
        keyboardFree(this);
}
/* 02674164 */
hdKeyboardKeyIcon_c* constructKeyboardKeyIcon(hdKeyboardKeyIcon_c* self) {
    if (!self) {
        self = static_cast<hdKeyboardKeyIcon_c*>(keyboardAllocate(sizeof(hdKeyboardKeyIcon_c)));
        if (!self)
            return NULL;
    }
    keyboardKeyIconConstruct(&self->mBase);
    self->mBase.mVtable.v = gabi::ea(&sKeyIconVtable);
    self->mBase.mStateHolder.mHolderVtable.v = gabi::ea(&sKeyIconDelegateA);
    self->mBase.mSecondaryVtable.v = gabi::ea(&sKeyIconDelegateB);
    return self;
}
/* 026741D0 */
u32 hdKeyboardKeyIcon_c::create(u32 parent, u32 archive, u32 layout) {
    return keyboardKeyIconCreate(&mBase, &sKeyIconLayoutName, parent, archive, layout);
}
/* 026741E8 */
static void initializeKeyboardKeyIcon() {
    initializeKeyboardStatics(&sKeyIconRange, &sKeyIconRegistrations, &sKeyIconLimits);
    sKeyIconLayoutName.mVtable.v = gabi::ea(&sKeyIconStringVtable);
    sKeyIconLayoutName.mText.v = gabi::guest_str("KeyIcon_00");
}
/* WWHD glue (tools/verify/autoglue.py)
 data sKeyIconVtable 100F6E38 nocheck
 data sKeyIconDelegateA 100F6F60 nocheck
 data sKeyIconDelegateB 100F6F70 nocheck
 data sKeyIconStringVtable 100F6E04 nocheck
 data sKeyIconRange 10494FC8
 data sKeyIconRegistrations 101F6128
 data sKeyIconLimits 100F6E24
 data sKeyIconLayoutName 10494FE4
 str 100F6E2C "KeyIcon_00"
 ext 0273AD10 void* keyboardAllocate(u32)
 ext 0273AF40 void keyboardFree(void*)
 ext 026F80FC void keyboardKeyIconConstruct(hdPartsKeyIcon_c*)
 ext 026F7DAC void keyboardKeyIconDestroy(hdPartsKeyIcon_c*,u32)
 ext 026F8174 u32 keyboardKeyIconCreate(hdPartsKeyIcon_c*,hdUiSafeString*,u32,u32,u32)
 ext 028F026C void keyboardRegister(hdKeyboardRegistration_c*)
 ext 028ED6F8 void keyboardFlagA(be<u8>*)
 ext 028EAB2C void keyboardFlagB(be<u8>*)
*/
