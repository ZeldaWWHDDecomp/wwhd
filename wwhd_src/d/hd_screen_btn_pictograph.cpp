/** HD pictograph thumbnail button, position animation and best-shot child. Private source. */
#include "d/hd_screen_btn_pictograph.h"
#include <cmath>

void hdPictoRegisterGlobalPictoButton(void*);
void hdPictoInitializeLifetimePictoButton(void*);
void hdPictoInitializeExitBytePictoButton(void*);
void hdPictoBasicButtonDestroyPictoButton(hdBasicButton_c*, s32);
struct hdPictographHeader_l { be<f32> mMinAngle, mMaxAngle; be<u8> mLifetime[4]; be<u32> mWords[4]; };
struct hdPictographRegistration_l { u8 mBytes[12]; };
static hdPictographHeader_l sPictographHeader;
static hdPictographRegistration_l sPictographHeaderRegistration;
static hdPictographRegistration_l sPictographAngleRegistration;
static hdPictographRegistration_l sPictographLifetimeRegistration;
static hdUiStringTable sPictographStringVtable;
static hdUiSafeString sPictographGroup;
static hdUiSafeString sPictographBestShotName;
static hdPartsVtbl_c sPictographButtonVtable;
static u32 sPictographHolderVtable;
static u32 sPictographSecondaryVtable;
static hdUiSafeString sPictographPartName;
static hdUiSafeString sPictographClassName;
static hdUiSafeString sBestShotPartNamePictoButton;
static hdPartsStateId_c sBestShotLoopPictoButton;
static hdPartsStateId_c sBestShotInvisiblePictoButton;
static hdScreenUiSystem* sPictographUiSystem;

struct hdPictoChildDescriptor_l {
    u8 _reserved00[8];
    hdUiSafeString mResourceName;
    u8 _reserved10[4];
    hdUiSafeString mClassName;
};
WWHD_OFFSET(hdPictoChildDescriptor_l, mResourceName, 8);
WWHD_OFFSET(hdPictoChildDescriptor_l, mClassName, 0x14);

/* 02641F5C */
void hdPictographButton_c::destroy(hdPictographButton_c* part, s32 flags) {
    if (part != NULL) {
        part->mButton.mStateHolder.mHolderVtable = &sPictographHolderVtable;
        part->mButton.mSecondaryVtable = &sPictographSecondaryVtable;
        hdPictoBasicButtonDestroyPictoButton(part, 0);
        if (flags & 1) operator_delete(part);
    }
}

/* 02641FC8 */
hdPictographButton_c* hdPictographButton_c::construct(hdPictographButton_c* part) {
    if (part == NULL) {
        part = (hdPictographButton_c*)operator_new(sizeof(hdPictographButton_c));
        if (part == NULL) return NULL;
    }
    hdPartsBasicButton_construct(&part->mButton);
    part->mButton.mStateHolder.mHolderVtable = &sPictographHolderVtable;
    part->mButton.mVtable = &sPictographButtonVtable;
    part->mButton.mSecondaryVtable = &sPictographSecondaryVtable;
    part->mTexture.mTexture = 0;
    part->mTexture.mHeight = 0;
    part->mTexture.mWidth = 0;
    return part;
}

/* 02642044 */
u32 hdPictographButton_c::initialize(u32 arg1, u32 arg2, hdPartsDesc_c* desc) {
    if (!hdPartsBasicButton_init(&mButton, &sPictographPartName, arg1, arg2, desc)) return FALSE;
    hdPartsPaneSRT_c* pane = hdPictoRoot(&mButton)->find("PF_Pictograph_00");
    hdPictoMaterial_l* material = gabi::call_ptr<hdPictoMaterial_l*>(pane->srtVtbl()->mGetMaterial.get(), pane, 0);
    hdPictoCopyTexture(material->mpTextureMap, &mTexture);
    return TRUE;
}

/* 026420F0 */
void hdPictographButton_c::setHorizontalPosition(f32 position) {
    hdPartsPaneSRT_c* pane = hdPictoRoot(&mButton);
    if (pane->mAlpha == 0) return;
    f32 rotation = 0.0f;
    f32 distance = std::fabs(position);
    if (300.0f < position) rotation = ((position - 300.0f) / 800.0f) * 30.0f;
    else if (position < -300.0f) rotation = ((position + 300.0f) / 800.0f) * 30.0f;
    distance = distance - 300.0f;
    f32 sineArgument = distance / 700.0f;
    u32 rotationX = pane->mRotationXBits;
    u32 rotationZ = pane->mRotationZBits;
    *reinterpret_cast<be<f32>*>(&pane->mRotationYBits) = -rotation;
    u8 flags = pane->mFlags;
    pane->mRotationXBits = rotationX;
    pane->mFlags = flags | 0x10;
    pane->mRotationZBits = rotationZ;
    u32 translationY = pane->mTranslationYBits;
    f64 sine = hdPictoSine(sineArgument);
    f32 depth = f32(sine * 180.0);
    if (distance < 0.0f) depth = 0.0f;
    f32 alpha = gabi::fnmsubs(depth, 1.5f, 255.0f);
    *reinterpret_cast<be<f32>*>(&pane->mTranslationXBits) = position;
    flags = pane->mFlags;
    pane->mTranslationYBits = translationY;
    *reinterpret_cast<be<f32>*>(&pane->mTranslationZBits) = depth;
    pane->mFlags = flags | 0x10;
    u8 alphaByte = u8(gabi::ftoi(alpha));
    pane->mAlpha = alphaByte == 0 ? 1 : alphaByte;
}

/* 026422AC */
BOOL hdPictographButton_c::fadeOut() {
    hdPartsPaneSRT_c* pane = hdPictoRoot(&mButton);
    u32 alpha = pane->mAlpha;
    f32 scaleX = pane->mScaleX;
    u32 next = u8(alpha - 32);
    f32 scaleY = pane->mScaleY;
    scaleX = scaleX - 0.124f;
    scaleY = scaleY - 0.124f;
    if (alpha < next || next == 0) {
        pane->mScaleX = scaleX;
        u8 flags = pane->mFlags;
        pane->mAlpha = 0;
        pane->mScaleY = scaleY;
        pane->mFlags = flags | 0x10;
        return TRUE;
    }
    pane->mAlpha = next;
    u8 flags = pane->mFlags;
    pane->mScaleX = scaleX;
    pane->mScaleY = scaleY;
    pane->mFlags = flags | 0x10;
    return FALSE;
}

/* 02642330 */
void hdPictographButton_c::resetFade() {
    hdPartsPaneSRT_c* pane = hdPictoRoot(&mButton);
    u8 flags = pane->mFlags;
    pane->mAlpha = 255;
    pane->mScaleX = 1.0f;
    pane->mScaleY = 1.0f;
    pane->mFlags = flags | 0x10;
}

/* 02642364 */
void hdPictographButton_c::hide() { hdPictoRoot(&mButton)->mAlpha = 0; }

/* 0264237C */
void hdPictographButton_c::setThumbnail(hdPictoTextureExtent_c* texture) {
    hdPartsPaneSRT_c* pane = hdPictoRoot(&mButton)->find("PF_Pictograph_00");
    hdPictoSetPaneTexture(pane, texture, 0, 0);
}

/* 026423DC */
void hdPictographButton_c::restoreThumbnail() {
    hdPartsPaneSRT_c* pane = hdPictoRoot(&mButton)->find("PF_Pictograph_00");
    hdPictoSetPaneTexture(pane, &mTexture, 0, 0);
}

/* 0264243C */
void hdPictographButton_c::showBestShot() { mButton.child(0)->changeState(&sBestShotLoopPictoButton); }

/* 0264245C */
void hdPictographButton_c::hideBestShot() { mButton.child(0)->changeState(&sBestShotInvisiblePictoButton); }

// The original snapshots the pointer before the expected string's callback, then re-reads
// the input descriptor for its byte scan (a termination callback may retain it).
static bool pictoNameEquals(hdUiSafeString* input, hdUiSafeString* expected) {
    input->assureTermination();
    input->assureTermination();
    const be<u8>* snapshot = input->mText;
    expected->assureTermination();
    if (gabi::ea(snapshot) == gabi::ea(expected->mText.get())) return true;
    const be<u8>* text = input->mText;
    const be<u8>* target = expected->mText;
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 left = *hdUiEntryAdvanceByte((be<u8>*)text, i);
        u8 right = *hdUiEntryAdvanceByte((be<u8>*)target, i);
        if (left != right) return false;
        if (left == 0) return true;
    }
    return false;
}

/* 0264247C */
u32 hdPictographButton_c::createChild(hdPartsDesc_c* desc) {
    hdPictoChildDescriptor_l* names = (hdPictoChildDescriptor_l*)desc;
    hdScreenHeap* heap = sPictographUiSystem->mHeap;
    if (!pictoNameEquals(&names->mClassName, &sPictographClassName)) return TRUE;
    if (!pictoNameEquals(&names->mResourceName, &sBestShotPartNamePictoButton)) return hdPictoCreateDefaultChild(&mButton, 0, desc);
    gptr<hdParts_c>* slot = mButton.mChildren.mpData;
    hdBestShotMark_c* childPart = (hdBestShotMark_c*)hdParts_new(sizeof(hdBestShotMark_c), heap, 4);
    if (childPart != NULL) childPart = hdBestShotMark_c::construct(childPart);
    *slot = childPart;
    if (*mButton.mChildren.mpData.get() == NULL) return FALSE;
    return ((hdBestShotMark_c*)mButton.mChildren.mpData.get()->get())->initialize(0, 0, desc);
}

/* 02642684 */
static void initializePictographButtonStatics() {
    sPictographHeader.mWords[2] = 0;
    sPictographHeader.mWords[0] = 0;
    sPictographHeader.mWords[3] = 0;
    sPictographHeader.mWords[1] = 0;
    hdPictoRegisterGlobalPictoButton(&sPictographHeaderRegistration);
    sPictographHeader.mMinAngle = -3.1415927410125732f;
    sPictographHeader.mMaxAngle = 3.1415927410125732f;
    hdPictoInitializeLifetimePictoButton(sPictographHeader.mLifetime);
    hdPictoRegisterGlobalPictoButton(&sPictographAngleRegistration);
    hdPictoInitializeExitBytePictoButton(&sPictographHeader.mLifetime[1]);
    hdPictoRegisterGlobalPictoButton(&sPictographLifetimeRegistration);
    sPictographGroup.mVtable = &sPictographStringVtable;
    sPictographGroup.mText.v = gabi::guest_addr("G_Btn_00");
    sPictographBestShotName.mVtable = &sPictographStringVtable;
    sPictographClassName.mVtable = &sPictographStringVtable;
    sPictographClassName.mText.v = gabi::guest_addr("L_BestShotMark_00");
    sPictographBestShotName.mText.v = gabi::guest_addr("BestShotMark_00");
    sPictographPartName.mVtable = &sPictographStringVtable;
    sPictographPartName.mText.v = gabi::guest_addr("BtnPictograph_00");
}

/* WWHD glue
 data sPictographHeader 10490A50 nocheck
 data sPictographHeaderRegistration 101F5744 nocheck
 data sPictographAngleRegistration 101F5750 nocheck
 data sPictographLifetimeRegistration 101F575C nocheck
 data sPictographStringVtable 100EBD84 nocheck
 data sPictographGroup 10490A38 nocheck
 data sPictographBestShotName 10490A48 nocheck
 str 100EBE10 "BestShotMark_00" in 02642684
 str 100EBE20 "L_BestShotMark_00"
 str 100EBE34 "G_Btn_00" in 02642684
 str 100EBE40 "BtnPictograph_00"
 ext 028F026C void hdPictoRegisterGlobalPictoButton(void*)
 ext 028ED6F8 void hdPictoInitializeLifetimePictoButton(void*)
 ext 028EAB2C void hdPictoInitializeExitBytePictoButton(void*)
 data sPictographButtonVtable 100EBE58 nocheck
 data sPictographHolderVtable 100EC0B0 nocheck
 data sPictographSecondaryVtable 100EC0C0 nocheck
 data sPictographPartName 10490A6C nocheck
 data sPictographClassName 10490A40 nocheck
 data sBestShotPartNamePictoButton 1048EEE8 nocheck
 data sBestShotLoopPictoButton 1048EE58 nocheck
 data sBestShotInvisiblePictoButton 1048EEB8 nocheck
 data sPictographUiSystem 1018C404 nocheck
 str 100EBDA4 "PF_Pictograph_00" in 02642044
 str 100EBDE0 "PF_Pictograph_00" in 0264237C
 str 100EBDF4 "PF_Pictograph_00" in 026423DC
 ext 026F0B8C void hdPictoBasicButtonDestroyPictoButton(hdBasicButton_c*, s32)
 ext 028754EC void hdPictoCopyTexture(void*, hdPictoTextureExtent_c*)
 ext 02704F3C void hdPictoSetPaneTexture(hdPartsPaneSRT_c*, hdPictoTextureExtent_c*, u32, u32)
 ext 026F92C0 u32 hdPictoCreateDefaultChild(hdParts_c*, u32, hdPartsDesc_c*)
 ext 028F43F8 f64 hdPictoSine(f64)


*/
