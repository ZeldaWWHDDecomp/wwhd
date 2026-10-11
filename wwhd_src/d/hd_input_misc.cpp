/* HD-only pane event receiver, initialization and GamePad orientation. */
#include "d/hd_input_misc.h"

static u32 l_receiverVtable;
static f32 l_zero;
static f32 l_half;
static f32 l_three;
static HdInputPointer l_controllerManager;
static u32 l_orientationInitialized;
static HdInputCallback l_orientationCallback;
static u32 l_orientationVtable;
static HdInputInitGlobals l_globals1;
static HdInputInitRange l_range1;
static f32 l_lower1;
static f32 l_upper1;
static HdInputRegistration l_registration1_1;
static HdInputRegistration l_registration1_2;
static HdInputRegistration l_registration1_3;
static HdInputInitGlobals l_globals2;
static HdInputInitRange l_range2;
static f32 l_lower2;
static f32 l_upper2;
static HdInputRegistration l_registration2_1;
static HdInputRegistration l_registration2_2;
static HdInputRegistration l_registration2_3;
static HdInputInitGlobals l_globals3;
static HdInputInitRange l_range3;
static f32 l_lower3;
static f32 l_upper3;
static HdInputRegistration l_registration3_1;
static HdInputRegistration l_registration3_2;
static HdInputRegistration l_registration3_3;

enum HdInputSlot : u32 { HdInput_HitTest = 5, HdInput_ReadOrientation = 3 };
enum HdInputConstants : u32 { HdInput_Delete = 1, HdInput_GamePad = 8 };

/* 02616ED4 */
static HdInputReceiver* HdInputReceiver__ct(HdInputReceiver* self, void* owner, u32 value) {
    if (self == NULL) {
        self = static_cast<HdInputReceiver*>(hdInputAllocate(sizeof(HdInputReceiver)));
        if (self == NULL) return NULL;
    }
    hdInputReceiverConstruct(self, owner);
    self->mUserValue = value;
    self->mVtable = reinterpret_cast<gfn*>(&l_receiverVtable);
    self->mFlags[1] = 0;
    self->mFlags[2] = 0;
    self->mFlags[0] = 0;
    return self;
}

/* 02616F58 */
static void HdInputReceiver__dt(HdInputReceiver* self, u32 flags) {
    if (self == NULL) return;
    hdInputReceiverDestroy(self, 0);
    if (flags & HdInput_Delete) hdInputDelete(self);
}

// HD: copy matrix words, preserving signaling NaNs in the UI transform.
static void copyPaneMatrix(Mtx34* destination, HdInputReceiver* receiver) {
    be<u32>* source = reinterpret_cast<be<u32>*>(&receiver->mpPane->mGlobalMtx);
    be<u32>* words = reinterpret_cast<be<u32>*>(destination);
    for (u32 i = 0; i < 12; i++) words[i] = source[i];
}

/* 02616FAC */
u8 HdInputReceiver::hitsCorner(HdInputReceiver* item) {
    gabi::Local<HdInputCornerWorkspace> workspace;
    copyPaneMatrix(&workspace->mMtx, item);
    hdInputGetPaneRect(item->mpPane, &workspace->mRect);
    f32 tx = workspace->mMtx.m[0][3];
    f32 ty = workspace->mMtx.m[1][3];
    f32 left = gabi::fadds_ppc(tx, workspace->mRect.mLeft);
    f32 right = gabi::fadds_ppc(tx, workspace->mRect.mRight);
    f32 bottom = gabi::fadds_ppc(ty, workspace->mRect.mBottom);
    f32 top = gabi::fadds_ppc(ty, workspace->mRect.mTop);
    const f32 xs[4] = {left, right, left, right};
    const f32 ys[4] = {top, top, bottom, bottom};
    for (u32 i = 0; i < 4; i++) {
        if (gabi::call_ptr<u32>(mVtable[HdInput_HitTest].get(), this, xs[i], ys[i])) return 1;
    }
    return 0;
}

/* 02617174 */
f32 HdInputReceiver::paneDistance(HdInputReceiver* other) {
    gabi::Local<HdInputDistanceWorkspace> workspace;
    copyPaneMatrix(&workspace->mFirst, this);
    copyPaneMatrix(&workspace->mSecond, other);
    f32 dy = gabi::fsubs_ppc(workspace->mFirst.m[1][3], workspace->mSecond.m[1][3]);
    f32 dy2 = gabi::fmuls_ppc(dy, dy);
    f32 dx = gabi::fsubs_ppc(workspace->mFirst.m[0][3], workspace->mSecond.m[0][3]);
    f32 distanceSquared = gabi::fmadds(dx, dx, dy2);
    f32 inverse = l_zero;
    if (distanceSquared > inverse) {
        // HD: retain the double reciprocal estimate and PowerPC operand rounding.
        f64 estimate = gabi::frsqrte(distanceSquared);
        f32 estimateSquared = f32(estimate * round25(estimate));
        f32 halfEstimate = f32(estimate * f64(l_half));
        inverse = gabi::fmuls_ppc(gabi::fnmsubs(estimateSquared, distanceSquared, l_three), halfEstimate);
    }
    return gabi::fmuls_ppc(inverse, distanceSquared);
}

/* 02617270 */
u8 HdInputReceiver::getPrimaryFlag() { return mFlags[0]; }
/* 02617278 */
u8 HdInputReceiver::getSecondaryFlag() { return mFlags[1]; }
/* 02617280 */
u8 HdInputReceiver::getTertiaryFlag() { return mFlags[2]; }

static void initializeGlobals(HdInputInitGlobals* globals, HdInputRegistration* first,
                              be<f32>* lower, be<f32>* upper, HdInputInitRange* range,
                              HdInputRegistration* second, HdInputRegistration* third) {
    globals->mThird = 0;
    globals->mFirst = 0;
    globals->mFourth = 0;
    globals->mSecond = 0;
    hdInputRegister(first);
    f32 lowerValue = *lower;
    f32 upperValue = *upper;
    range->mLower = lowerValue;
    range->mUpper = upperValue;
    hdInputInitSigned(&range->mSignedInitialized);
    hdInputRegister(second);
    hdInputInitUnsigned(&range->mUnsignedInitialized);
    hdInputRegister(third);
}

/* 02617288 */
static void __sinit_hd_input_misc_cpp() {
    initializeGlobals(&l_globals1, &l_registration1_1, &l_lower1, &l_upper1, &l_range1,
                      &l_registration1_2, &l_registration1_3);
}
/* 0261731C */
static void __sinit_hd_input_misc_second_cpp() {
    initializeGlobals(&l_globals2, &l_registration2_1, &l_lower2, &l_upper2, &l_range2,
                      &l_registration2_2, &l_registration2_3);
}

/* 026173B0 */
static void hdInput_readGamePadOrientation(HdInputOrientation* out) {
    HdInputController* controller = hdInputGetController(l_controllerManager.mpValue, HdInput_GamePad);
    if (!l_orientationInitialized) {
        l_orientationInitialized = 1;
        l_orientationCallback.mVtable = &l_orientationVtable;
    }
    if (controller == NULL) return;
    if (!gabi::call_ptr<u32>(controller->mVtable[HdInput_ReadOrientation].get(), controller,
                             &l_orientationCallback)) return;
    if (controller->mSampleCount <= 0) return;
    gabi::Local<HdInputAxes> vectors;
    be<f32>* source = reinterpret_cast<be<f32>*>(controller->mOrientation);
    be<f32>* temporary = reinterpret_cast<be<f32>*>(vectors.get());
    for (u32 i = 0; i < 9; i++) temporary[i] = (f32)source[i];
    out->mAxes[0].y = vectors->mAxes[0].y;
    out->mAxes[0].z = vectors->mAxes[0].z;
    out->mAxes[1].y = vectors->mAxes[1].y;
    out->mAxes[0].x = vectors->mAxes[0].x;
    out->mAxes[1].z = vectors->mAxes[1].z;
    out->mAxes[1].x = vectors->mAxes[1].x;
    out->mAxes[2].x = vectors->mAxes[2].x;
    out->mAxes[2].z = vectors->mAxes[2].z;
    out->mAxes[2].y = vectors->mAxes[2].y;
    hdInputMakeOrientationMatrix(&out->mMatrix, &vectors->mAxes[0], &vectors->mAxes[1], &vectors->mAxes[2]);
}

/* 026174FC */
static void __sinit_hd_input_misc_third_cpp() {
    initializeGlobals(&l_globals3, &l_registration3_1, &l_lower3, &l_upper3, &l_range3,
                      &l_registration3_2, &l_registration3_3);
}

/* WWHD glue (tools/verify/autoglue.py): HD receiver and controller interfaces.
data l_receiverVtable 100E28F8
data l_zero 100E28E0
data l_half 100E28E4
data l_three 100E28E8
data l_controllerManager 101F8AE8
data l_orientationInitialized 101FD694
data l_orientationCallback 101FD968
data l_orientationVtable 100E2968
data l_globals1 1048DE20
data l_range1 1048DE14
data l_lower1 100E28EC
data l_upper1 100E28F0
data l_registration1_1 101F4FF8
data l_registration1_2 101F5004
data l_registration1_3 101F5010
data l_globals2 1048DE3C
data l_range2 1048DE30
data l_lower2 100E2950
data l_upper2 100E2954
data l_registration2_1 101F501C
data l_registration2_2 101F5028
data l_registration2_3 101F5034
data l_globals3 1048DE58
data l_range3 1048DE4C
data l_lower3 100E2978
data l_upper3 100E297C
data l_registration3_1 101F5040
data l_registration3_2 101F504C
data l_registration3_3 101F5058
ext 0273AD10 void* hdInputAllocate(u32)
ext 02001A00 void hdInputReceiverConstruct(HdInputReceiver*, void*)
ext 02001ABC void hdInputReceiverDestroy(HdInputReceiver*, u32)
ext 0273AF40 void hdInputDelete(void*)
ext 02876368 void hdInputGetPaneRect(HdInputPane*, HdInputRect*)
ext 028F026C void hdInputRegister(void*)
ext 028ED6F8 void hdInputInitSigned(be<u8>*)
ext 028EAB2C void hdInputInitUnsigned(be<u8>*)
ext 0273CB8C HdInputController* hdInputGetController(void*, u32)
ext 026175B4 void hdInputMakeOrientationMatrix(Mtx34*, cXyz*, cXyz*, cXyz*)
*/
