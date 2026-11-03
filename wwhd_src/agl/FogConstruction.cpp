// Native Fog parameter construction, reconstructed from local game instructions.
#include "agl/EnvironmentObject.h"
using namespace agl::env;
typedef be<u8> ParameterTextByte;
static EnvironmentObjectMethods sFogObjectMethods;
static sead::VtableSlot sFogIoMethods;
static sead::VtableSlot sFogScalarMethods;
static sead::VtableSlot sFogColorMethods;
static sead::VtableSlot sFogVectorMethods;
static sead::VtableSlot sFogDirectionMethods;
static sead::SafeTextVtable sFogTextMethods;
static ParameterTextByte sStartName;
static ParameterTextByte sEndName;
static ParameterTextByte sColorName;
static ParameterTextByte sDirectionName;
static ParameterTextByte sStartDescription;
static ParameterTextByte sEndDescription;
static ParameterTextByte sColorDescription;
static ParameterTextByte sDirectionDescription;
static ParameterTextByte sEmptyHint;
static ParameterTextByte sColorHint;
static ParameterTextByte sDirectionHint;
static f32 sStartDefault;
static f32 sEndDefault;
static f32 sDirectionFlat;
static f32 sDirectionDepth;
static f32 sColorZero;
static f32 sColorAlpha;
static ParameterColor sDefaultColor;
void* allocateFogStorage(u32);
EnvironmentObject* constructEnvironmentObject(EnvironmentObject*);
ParameterBinding* constructParameterBinding(ParameterBinding*);
void registerEnvironmentParameter(ParameterBinding*,const sead::SafeText*,const sead::SafeText*,const sead::SafeText*,EnvironmentObject*);

/* inlined into Fog::construct */
template <class T> static T* placementParameter(T* value) {
    if (gabi::ea(value) == 0) value = static_cast<T*>(allocateFogStorage(sizeof(T)));
    return value;
}
/* inlined into Fog::construct */
static void setParameterText(sead::SafeText* text,be<u8>* value) {
    text->mText = value;
    text->mVtable = &sFogTextMethods;
}
/* inlined into Fog::construct */
static void copyParameterWord(be<f32>* destination,const be<f32>* source) {
    // The original uses integer loads/stores here, preserving signalling-NaN bits.
    reinterpret_cast<be<u32>&>(*destination) = (u32)reinterpret_cast<const be<u32>&>(*source);
}

/* 0276EE70 */
Fog* Fog::construct(Fog* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) {
        object = static_cast<Fog*>(allocateFogStorage(sizeof(Fog)));
        if (object == NULL) return NULL;
    }
    constructEnvironmentObject(object);
    object->mMethods = &sFogObjectMethods;
    object->mParameterIoMethods = &sFogIoMethods;
    gabi::FrameLocal<sead::SafeText> startName(8), startDescription(16), startHint(72);
    setParameterText(startName.get(),&sStartName);
    setParameterText(startDescription.get(),&sStartDescription);
    EnvironmentScalarParameter* start = placementParameter(&object->mStart);
    if (start != NULL) {
        constructParameterBinding(&start->mBinding);
        start->mBinding.mMethods = &sFogScalarMethods;
        setParameterText(startHint.get(),&sEmptyHint);
        registerEnvironmentParameter(&start->mBinding,startName.get(),startDescription.get(),startHint.get(),object);
        start->mValue = sStartDefault;
    }
    gabi::FrameLocal<sead::SafeText> endName(24), endDescription(32), endHint(80);
    setParameterText(endName.get(),&sEndName);
    setParameterText(endDescription.get(),&sEndDescription);
    EnvironmentScalarParameter* end = placementParameter(&object->mEnd);
    if (end != NULL) {
        constructParameterBinding(&end->mBinding);
        end->mBinding.mMethods = &sFogScalarMethods;
        setParameterText(endHint.get(),&sEmptyHint);
        registerEnvironmentParameter(&end->mBinding,endName.get(),endDescription.get(),endHint.get(),object);
        end->mValue = sEndDefault;
    }
    gabi::FrameLocal<sead::SafeText> colorName(40), colorDescription(48), colorHint(88);
    setParameterText(colorName.get(),&sColorName);
    setParameterText(colorDescription.get(),&sColorDescription);
    EnvironmentColorParameter* color = placementParameter(&object->mColor);
    if (color != NULL) {
        constructParameterBinding(&color->mBinding);
        color->mBinding.mMethods = &sFogColorMethods;
        f32 zero = sColorZero;
        color->mValue.r = zero;
        color->mValue.b = zero;
        color->mValue.g = zero;
        color->mValue.a = sColorAlpha;
        setParameterText(colorHint.get(),&sColorHint);
        registerEnvironmentParameter(&color->mBinding,colorName.get(),colorDescription.get(),colorHint.get(),object);
        copyParameterWord(&color->mValue.r,&sDefaultColor.r);
        copyParameterWord(&color->mValue.g,&sDefaultColor.g);
        copyParameterWord(&color->mValue.b,&sDefaultColor.b);
        copyParameterWord(&color->mValue.a,&sDefaultColor.a);
    }
    gabi::FrameLocal<sead::SafeText> directionName(56), directionDescription(64), directionHint(96);
    setParameterText(directionName.get(),&sDirectionName);
    setParameterText(directionDescription.get(),&sDirectionDescription);
    EnvironmentVectorParameter* direction = placementParameter(&object->mDirection);
    if (direction != NULL) {
        constructParameterBinding(&direction->mBinding);
        direction->mBinding.mMethods = &sFogVectorMethods;
        setParameterText(directionHint.get(),&sDirectionHint);
        registerEnvironmentParameter(&direction->mBinding,directionName.get(),directionDescription.get(),directionHint.get(),object);
        f32 depth = sDirectionDepth;
        f32 flat = sDirectionFlat;
        direction->mValue.z = depth;
        direction->mValue.y = flat;
        direction->mValue.x = flat;
    }
    object->mDirection.mBinding.mMethods = &sFogDirectionMethods;
    return object;
}

void releaseFogObject(void*);
void destroyFogEnvironmentBase(EnvironmentObject*,u32);

/* 0276F180 */
void FogClassDescriptor::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseFogObject),this);
}
/* 0276F194 */
void Fog::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mParameterIoMethods = &sFogIoMethods;
    destroyFogEnvironmentBase(this,0);
    if (flags & 1) releaseFogObject(this);
}
/* 0276F1F4 */
const sead::SafeText* Fog::groupText() const {
    return gabi::tail_ptr<const sead::SafeText*>(mMethods->mGroupText.mFunction,this);
}
/* 0276F20C */
s32 Fog::typeId() const {
    return gabi::tail_ptr<s32>(mMethods->mTypeId.mFunction,this);
}
/* 0276F2D0 */
const sead::SafeText* Fog::emptyHook() const {
    return gabi::tail_ptr<const sead::SafeText*>(mMethods->mEmptyHook.mFunction,this);
}

/* WWHD glue
 ext 0273AF40 void releaseFogObject(void*) callback
 ext 0276FC20 void destroyFogEnvironmentBase(EnvironmentObject*,u32)
 data sFogObjectMethods 101606C0
 data sFogIoMethods 10160770
 data sFogScalarMethods 10160484
 data sFogColorMethods 10160574
 data sFogVectorMethods 101604FC
 data sFogDirectionMethods 101605EC
 data sFogTextMethods 1016044C
 data sStartName 10160678
 data sEndName 10160674
 data sColorName 10160680
 data sDirectionName 101606A8
 data sStartDescription 10160688
 data sEndDescription 10160690
 data sColorDescription 10160698
 data sDirectionDescription 101606A0
 data sEmptyHint 10160448
 data sColorHint 1016044A
 data sDirectionHint 10160449
 data sStartDefault 10160664
 data sEndDefault 10160668
 data sDirectionFlat 1016066C
 data sDirectionDepth 10160670
 data sColorZero 10145180
 data sColorAlpha 1014517C
 data sDefaultColor 104A01EC
 ext 0273AD10 void* allocateFogStorage(u32)
 ext 0276FCA0 EnvironmentObject* constructEnvironmentObject(EnvironmentObject*)
 ext 0279E674 ParameterBinding* constructParameterBinding(ParameterBinding*)
 ext 0279E618 void registerEnvironmentParameter(ParameterBinding*,const sead::SafeText*,const sead::SafeText*,const sead::SafeText*,EnvironmentObject*)
*/
