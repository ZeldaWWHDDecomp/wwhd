// The environment object's own method set, the text-parameter and text-buffer method copies, and the light
// classes' base methods, parameter accessor copies, runtime types and kind registration.
#include "agl/EnvironmentLights.h"
using namespace agl::env;
void releaseParameterStorage(void* storage);
static EnvironmentRttiTable sEnvironmentObjectTypeMethods;
/* 027706DC */
const sead::SafeText* EnvironmentObject::groupText() const {
    return gabi::tail_ptr<const sead::SafeText*>(mMethods->mGroupText.mFunction,this);
}
/* 027706EC */
const sead::SafeText* EnvironmentObject::nameData() const { return &mName.mValue; }
/* 027706F4 */
s32 EnvironmentObject::typeId() const { return gabi::tail_ptr<s32>(mMethods->mTypeId.mFunction,this); }
/* 02770704 */
u32 EnvironmentObject::enabled() const { return (u8)mFlags & 1; }
/* 02770710 */
void EnvironmentObject::emptyQuery() {}
// The mAssureTermination body of every environment text table (10160808, 10160830, 10160848, 10160860, 10160878,
// 10160890 hold it at +0x14).
/* 02770714 */
void assureTextBufferTermination(sead::SafeTextBuffer* text) {
    text->mText.get()[(u32)text->mCapacity-1]=0;
}
// Deleting destructors of the environment text classes (mDestroy of the tables 101607F4, 10160834, 1016084C,
// 10160864 and 1016087C).
/* 0277072C */
static void destroyBufferBaseText(sead::SafeTextBuffer* text,u32 flags) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(text!=nullptr && (flags&1)) releaseParameterStorage(text);
}
/* 02770740 */
static void destroyFixedText(sead::SafeTextBuffer* text,u32 flags) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(text!=nullptr && (flags&1)) releaseParameterStorage(text);
}
/* 02770754 */
static void destroyIntermediateFormattedText(sead::SafeTextBuffer* text,u32 flags) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(text!=nullptr && (flags&1)) releaseParameterStorage(text);
}
/* 02770768 */
static void destroySecondFixedText(sead::SafeTextBuffer* text,u32 flags) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(text!=nullptr && (flags&1)) releaseParameterStorage(text);
}
/* 0277089C */
static void destroyFormattedText(sead::SafeTextBuffer* text,u32 flags) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(text!=nullptr && (flags&1)) releaseParameterStorage(text);
}
/* 027708B0 */
void EnvironmentObjectTextParameter::onValueChanged() {}
/* 027708B4 */
u32 EnvironmentObjectTextParameter::zeroCode() { return 0; }
/* 027708BC */
sead::FixedSafeText<32>* EnvironmentObjectTextParameter::valueText() { return &mValue; }
/* 027708C4 */
const sead::FixedSafeText<32>* EnvironmentObjectTextParameter::valueText() const { return &mValue; }
/* 027708CC */
u32 EnvironmentObjectTextParameter::unitCode() { return 1; }
/* 027708D4 */
u32 EnvironmentObjectTextParameter::valueKind() const { return 7; }
/* 027708DC */
be<u8>* EnvironmentObjectTextParameter::value() {
    auto frame=gabi::AutoFrame(gabi::current_function());
    mValue.assureTermination();
    return mValue.mText;
}
/* 0277091C */
const be<u8>* EnvironmentObjectTextParameter::value() const {
    auto frame=gabi::AutoFrame(gabi::current_function());
    const_cast<sead::FixedSafeText<32>*>(&mValue)->assureTermination();
    return mValue.mText;
}
/* 0277095C */
u32 EnvironmentObjectTextParameter::valueBytes() const { return sizeof(mValue.mStorage); }
/* 02770964 */
u32 EnvironmentObjectTextParameter::firstUnitCode() { return 1; }
/* 0277096C */
u32 EnvironmentObjectTextParameter::secondUnitCode() { return 1; }
/* 02770974 */
void EnvironmentObjectTextParameter::onFinalize() {}
/* 02770978 */
u32 EnvironmentObject::matchesResource(const EnvironmentResourceReference* reference) const {
    u32 resourceHash=reference->mObject->mNameHash; return resourceHash==(u32)mNameHash;
}
/* 02770994 */
u32 EnvironmentObject::isType(const EnvironmentTypeInfo* type) const {
    if(!sEnvironmentTypeReady) { sEnvironmentTypeReady=1;sEnvironmentType.mMethods=&sEnvironmentObjectTypeMethods; }
    return type==&sEnvironmentType;
}
/* 027709D0 */
void EnvironmentObject::onPrepare() {}
/* 027709D4 */
void EnvironmentObject::onUpdate() {}
/* 027709D8 */
void EnvironmentObject::onPostUpdate() {}
/* 027709DC */
void EnvironmentObject::onEmptyHook() {}
/* 027709E0 */
const sead::SafeText* EnvironmentObject::emptyHook() const {
    return gabi::tail_ptr<const sead::SafeText*>(mMethods->mEmptyHook.mFunction,this);
}
/* 027709F0 */
const sead::SafeText* EnvironmentObject::groupData() const { return &mGroup.mValue; }
Light* LightIo::object() {
    return &gabi::object_at<Light>(gabi::ea(this)-offsetof(EnvironmentObject,mParameterIoMethods));
}
/* 027709F8 */
const sead::SafeText* LightIo::groupText() {
    auto frame=gabi::AutoFrame(gabi::current_function()); return object()->groupText(); }
/* 02770A00 */
const sead::SafeText* LightIo::nameData() {
    auto frame=gabi::AutoFrame(gabi::current_function()); return object()->nameData(); }
/* 02770A08 */
s32 LightIo::typeId() {
    auto frame=gabi::AutoFrame(gabi::current_function()); return object()->typeId(); }
/* 02770A10 */
u32 LightIo::enabled() {
    auto frame=gabi::AutoFrame(gabi::current_function()); return object()->enabled(); }
/* 0277273C */
static void destroyEnvironmentRegistration(void* storage,u32 flags) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(storage!=nullptr && (flags&1)) releaseParameterStorage(storage);
}
/* 027727B0 */
const sead::SafeText* Light::groupText() const { return gabi::tail_ptr<const sead::SafeText*>(mMethods->mGroupText.mFunction,this); }
/* 027727C0 */
const sead::SafeText* Light::nameData() const { return &mName.mValue; }
/* 027727C8 */
s32 Light::typeId() const { return gabi::tail_ptr<s32>(mMethods->mTypeId.mFunction,this); }
/* 027727D8 */
u32 Light::enabled() const { return (u8)mFlags&1; }
/* 027727E4 */
void Light::emptyQuery() {}
/* 027727E8 */
static void emptyParameterCallback() {}
/* 027727EC */
u32 LightBooleanParameter::zeroCode() { return 0; }
/* 027727F4 */
be<u8>* LightBooleanParameter::value() { return &mValue; }
/* 027727FC */
const be<u8>* LightBooleanParameter::value() const { return &mValue; }
/* 02772804 */
u32 LightBooleanParameter::valueBytes() const { return 1; }
/* 0277280C */
u32 LightBooleanParameter::valueKind() const { return 1; }
/* 02772814 */
be<f32>* LightScalarParameter::value() { return &mValue; }
/* 0277281C */
const be<f32>* LightScalarParameter::value() const { return &mValue; }
/* 02772824 */
u32 LightScalarParameter::valueBytes() const { return 4; }
/* 0277282C */
u32 LightScalarParameter::valueKind() const { return 4; }
/* 02772834 */
ParameterVector3* LightVectorParameter::value() { return &mValue; }
/* 0277283C */
const ParameterVector3* LightVectorParameter::value() const { return &mValue; }
/* 02772844 */
u32 LightVectorParameter::valueBytes() const { return 12; }
/* 0277284C */
u32 LightVectorParameter::valueKind() const { return 6; }
/* 02772854 */
ParameterColor* LightColorParameter::value() { return &mValue; }
/* 0277285C */
const ParameterColor* LightColorParameter::value() const { return &mValue; }
/* 02772864 */
u32 LightColorParameter::valueBytes() const { return 16; }
/* 0277286C */
static u32 environmentFirstUnitCode() { return 1; }
/* 02772874 */
static u32 environmentSecondUnitCode() { return 1; }
/* 0277287C */
static void environmentFinalCallback() {}
/* 02772880 */
u32 Light::matchesResource(const EnvironmentResourceReference* reference) const {
    u32 resourceHash=reference->mObject->mNameHash; return resourceHash==(u32)mNameHash;
}
/* 0277289C */
void Light::onPrepare() {}
/* 027728A0 */
void Light::onUpdate() {}
/* 027728A4 */
void Light::onPostUpdate() {}
/* 027728A8 */
void Light::onEmptyHook() {}
/* 027728AC */
const sead::SafeText* Light::emptyHook() const { return gabi::tail_ptr<const sead::SafeText*>(mMethods->mEmptyHook.mFunction,this); }
/* 027728BC */
const sead::SafeText* Light::groupData() const { return &mGroup.mValue; }

void* allocateLightInHeap(u32 size,void* heap,s32 alignment);
static EnvironmentRttiTable sEnvironmentBaseTypeMethods;
static EnvironmentRttiTable sLightTypeMethods;
static u32 sAmbientTypeGuard;
static EnvironmentTypeInfo sAmbientType;
static EnvironmentClassDescriptor* sAmbientClass;
AmbientLight* constructAmbientLight(AmbientLight* light);
/* 027728C4 */
u32 AmbientLight::isType(const EnvironmentTypeInfo* type) const {
    if(!sAmbientTypeGuard) { sAmbientTypeGuard=1;sAmbientType.mMethods=&sLightTypeMethods; }
    if(type==&sAmbientType) return 1;
    if(!sEnvironmentTypeReady) { sEnvironmentTypeReady=1;sEnvironmentType.mMethods=&sEnvironmentBaseTypeMethods; }
    return type==&sEnvironmentType;
}
/* 0277293C */
s32 AmbientLight::kindIndex() { return sAmbientClass->mTypeId; }
/* 02772950 */
AmbientLight* AmbientLight::create(void* heap) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    auto* light=static_cast<AmbientLight*>(allocateLightInHeap(276,heap,4));
    if(light!=nullptr) light=constructAmbientLight(light);
    return light;
}
static u32 sHemisphereTypeGuard;
static EnvironmentTypeInfo sHemisphereType;
static EnvironmentClassDescriptor* sHemisphereClass;
HemisphereLight* constructHemisphereLight(HemisphereLight* light);
/* 02772984 */
u32 HemisphereLight::isType(const EnvironmentTypeInfo* type) const {
    if(!sHemisphereTypeGuard) { sHemisphereTypeGuard=1;sHemisphereType.mMethods=&sLightTypeMethods; }
    if(type==&sHemisphereType) return 1;
    if(!sEnvironmentTypeReady) { sEnvironmentTypeReady=1;sEnvironmentType.mMethods=&sEnvironmentBaseTypeMethods; }
    return type==&sEnvironmentType;
}
/* 027729FC */
s32 HemisphereLight::kindIndex() { return sHemisphereClass->mTypeId; }
/* 02772A0C */
HemisphereLight* HemisphereLight::create(void* heap) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    auto* light=static_cast<HemisphereLight*>(allocateLightInHeap(336,heap,4));
    if(light!=nullptr) light=constructHemisphereLight(light);
    return light;
}
static u32 sDirectionalTypeGuard;
static EnvironmentTypeInfo sDirectionalType;
static EnvironmentClassDescriptor* sDirectionalClass;
DirectionalLight* constructDirectionalLight(DirectionalLight* light);
/* 02772A40 */
u32 DirectionalLight::isType(const EnvironmentTypeInfo* type) const {
    if(!sDirectionalTypeGuard) { sDirectionalTypeGuard=1;sDirectionalType.mMethods=&sLightTypeMethods; }
    if(type==&sDirectionalType) return 1;
    if(!sEnvironmentTypeReady) { sEnvironmentTypeReady=1;sEnvironmentType.mMethods=&sEnvironmentBaseTypeMethods; }
    return type==&sEnvironmentType;
}
/* 02772AB8 */
s32 DirectionalLight::kindIndex() { return sDirectionalClass->mTypeId; }
/* 02772AC8 */
DirectionalLight* DirectionalLight::create(void* heap) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    auto* light=static_cast<DirectionalLight*>(allocateLightInHeap(448,heap,4));
    if(light!=nullptr) light=constructDirectionalLight(light);
    return light;
}
static u32 sPointTypeGuard;
static EnvironmentTypeInfo sPointType;
static EnvironmentClassDescriptor* sPointClass;
PointLight* constructPointLight(PointLight* light);
/* 02772AFC */
u32 PointLight::isType(const EnvironmentTypeInfo* type) const {
    if(!sPointTypeGuard) { sPointTypeGuard=1;sPointType.mMethods=&sLightTypeMethods; }
    if(type==&sPointType) return 1;
    if(!sEnvironmentTypeReady) { sEnvironmentTypeReady=1;sEnvironmentType.mMethods=&sEnvironmentBaseTypeMethods; }
    return type==&sEnvironmentType;
}
/* 02772B74 */
s32 PointLight::kindIndex() { return sPointClass->mTypeId; }
/* 02772B84 */
PointLight* PointLight::create(void* heap) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    auto* light=static_cast<PointLight*>(allocateLightInHeap(368,heap,4));
    if(light!=nullptr) light=constructPointLight(light);
    return light;
}
static u32 sConeTypeGuard;
static EnvironmentTypeInfo sConeType;
static EnvironmentClassDescriptor* sConeClass;
ConeLight* constructConeLight(ConeLight* light);
/* 02772BB8 */
u32 ConeLight::isType(const EnvironmentTypeInfo* type) const {
    if(!sConeTypeGuard) { sConeTypeGuard=1;sConeType.mMethods=&sLightTypeMethods; }
    if(type==&sConeType) return 1;
    if(!sEnvironmentTypeReady) { sEnvironmentTypeReady=1;sEnvironmentType.mMethods=&sEnvironmentBaseTypeMethods; }
    return type==&sEnvironmentType;
}
/* 02772C30 */
s32 ConeLight::kindIndex() { return sConeClass->mTypeId; }
/* 02772C40 */
ConeLight* ConeLight::create(void* heap) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    auto* light=static_cast<ConeLight*>(allocateLightInHeap(432,heap,4));
    if(light!=nullptr) light=constructConeLight(light);
    return light;
}
using LightFactory=void* (*)(void*);
EnvironmentClassDescriptor* registerLightKind(const sead::SafeText*,const sead::SafeText*,LightFactory,u32);
static sead::SafeTextVtable sLightKindNameTable;
static u8 sAmbientKindName;
static u8 sAmbientKindDescription;
void* createAmbientKind(void*);
static u8 sHemisphereKindName;
static u8 sHemisphereKindDescription;
void* createHemisphereKind(void*);
static u8 sDirectionalKindName;
static u8 sDirectionalKindDescription;
void* createDirectionalKind(void*);
static u8 sPointKindName;
static u8 sPointKindDescription;
void* createPointKind(void*);
static u8 sConeKindName;
static u8 sConeKindDescription;
void* createConeKind(void*);
/* 027725D0 */
void registerBuiltinLightKinds() {
    auto frame=gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<sead::SafeText> ambientName(8),ambientDescription(16);
    ambientName->mText=&sAmbientKindName;ambientName->mVtable=&sLightKindNameTable;
    ambientDescription->mText=&sAmbientKindDescription;ambientDescription->mVtable=&sLightKindNameTable;
    sAmbientClass=registerLightKind(ambientName.get(),ambientDescription.get(),createAmbientKind,0);
    gabi::FrameLocal<sead::SafeText> hemisphereName(24),hemisphereDescription(32);
    hemisphereName->mText=&sHemisphereKindName;hemisphereName->mVtable=&sLightKindNameTable;
    hemisphereDescription->mText=&sHemisphereKindDescription;hemisphereDescription->mVtable=&sLightKindNameTable;
    sHemisphereClass=registerLightKind(hemisphereName.get(),hemisphereDescription.get(),createHemisphereKind,0);
    gabi::FrameLocal<sead::SafeText> directionalName(40),directionalDescription(48);
    directionalName->mText=&sDirectionalKindName;directionalName->mVtable=&sLightKindNameTable;
    directionalDescription->mText=&sDirectionalKindDescription;directionalDescription->mVtable=&sLightKindNameTable;
    sDirectionalClass=registerLightKind(directionalName.get(),directionalDescription.get(),createDirectionalKind,0);
    gabi::FrameLocal<sead::SafeText> pointName(56),pointDescription(64);
    pointName->mText=&sPointKindName;pointName->mVtable=&sLightKindNameTable;
    pointDescription->mText=&sPointKindDescription;pointDescription->mVtable=&sLightKindNameTable;
    sPointClass=registerLightKind(pointName.get(),pointDescription.get(),createPointKind,0);
    gabi::FrameLocal<sead::SafeText> coneName(72),coneDescription(80);
    coneName->mText=&sConeKindName;coneName->mVtable=&sLightKindNameTable;
    coneDescription->mText=&sConeKindDescription;coneDescription->mVtable=&sLightKindNameTable;
    sConeClass=registerLightKind(coneName.get(),coneDescription.get(),createConeKind,0);
}
/* WWHD glue
ext 02770250 agl::env::EnvironmentClassDescriptor* registerLightKind(const sead::SafeText*,const sead::SafeText*,LightFactory,u32)
data sLightKindNameTable 10160BA8
data sAmbientKindName 10161190
data sAmbientKindDescription 101611A0
ext 02772950 void* createAmbientKind(void*) callback
data sHemisphereKindName 10161174
data sHemisphereKindDescription 101611B4
ext 02772A0C void* createHemisphereKind(void*) callback
data sDirectionalKindName 101611C0
data sDirectionalKindDescription 101611D4
ext 02772AC8 void* createDirectionalKind(void*) callback
data sPointKindName 101611EC
data sPointKindDescription 101611F8
ext 02772B84 void* createPointKind(void*) callback
data sConeKindName 10161184
data sConeKindDescription 10161208
ext 02772C40 void* createConeKind(void*) callback

ext 0273AF40 void releaseParameterStorage(void*)
data sEnvironmentObjectTypeMethods 1016080C

data sEnvironmentBaseTypeMethods 10160BC0
data sLightTypeMethods 10160BD0
ext 0273B050 void* allocateLightInHeap(u32,void*,s32)
data sAmbientTypeGuard 101FD7C8
data sAmbientType 101FDFD0
data sAmbientClass 104A1F64
ext 02770D10 AmbientLight* constructAmbientLight(AmbientLight*)
data sHemisphereTypeGuard 101FD7CC
data sHemisphereType 101FDFD4
data sHemisphereClass 104A1F68
ext 02770EC0 HemisphereLight* constructHemisphereLight(HemisphereLight*)
data sDirectionalTypeGuard 101FD7D0
data sDirectionalType 101FDFD8
data sDirectionalClass 104A1F6C
ext 027713EC DirectionalLight* constructDirectionalLight(DirectionalLight*)
data sPointTypeGuard 101FD7D4
data sPointType 101FDCCC
data sPointClass 104A1F70
ext 027719D0 PointLight* constructPointLight(PointLight*)
data sConeTypeGuard 101FD7D8
data sConeType 101FDFDC
data sConeClass 104A1F74
ext 02771E70 ConeLight* constructConeLight(ConeLight*)
*/
