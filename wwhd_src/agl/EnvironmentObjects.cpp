// Projector and Fog object queries and their local runtime-type initialization.
#include "agl/EnvironmentObject.h"
#include <atomic>
using namespace agl::env;
static sead::VtableSlot sProjectorIoMethods;
static u32 sProjectorTypeReady;
static u32 sFogTypeReady;
static u32 sEnvironmentTypeReady;
static EnvironmentTypeInfo sProjectorType;
static EnvironmentTypeInfo sFogType;
static EnvironmentTypeInfo sEnvironmentType;
static EnvironmentRttiTable sProjectorTypeMethods;
static EnvironmentRttiTable sFogTypeMethods;
static EnvironmentRttiTable sProjectorBaseTypeMethods;
static EnvironmentRttiTable sFogBaseTypeMethods;
static EnvironmentClassDescriptor* sProjectorClass;
static EnvironmentClassDescriptor* sFogClass;

/* 0276D804 */
const sead::SafeText* Projector::nameData() const { return &mName.mValue; }
/* 0276D81C */
u32 Projector::enabled() const { return u32(mFlags) & 1; }
/* 0276D8A4 */
u32 Projector::matchesResource(const EnvironmentResourceReference* reference) const {
    const EnvironmentResourceObject* resource = reference->mObject;
    u32 ownHash = mNameHash;
    u32 resourceHash = resource->mNameHash;
    return ownHash == resourceHash;
}
/* 0276D8DC */
const sead::SafeText* Projector::groupData() const { return &mGroup.mValue; }
/* 0276D8E4 */
u32 Projector::isType(const EnvironmentTypeInfo* type) const {
    if (sProjectorTypeReady == 0) {
        sProjectorTypeReady = 1;
        sProjectorType.mMethods = &sProjectorTypeMethods;
    }
    if (type == &sProjectorType) return 1;
    if (sEnvironmentTypeReady == 0) {
        sEnvironmentTypeReady = 1;
        sEnvironmentType.mMethods = &sProjectorBaseTypeMethods;
    }
    return type == &sEnvironmentType;
}
/* 0276D95C */
s32 Projector::registeredTypeId() { return sProjectorClass->mTypeId; }

/* 0276F204 */
const sead::SafeText* Fog::nameData() const { return &mName.mValue; }
/* 0276F21C */
u32 Fog::enabled() const { return u32(mFlags) & 1; }
/* 0276F2A4 */
u32 Fog::matchesResource(const EnvironmentResourceReference* reference) const {
    const EnvironmentResourceObject* resource = reference->mObject;
    u32 ownHash = mNameHash;
    u32 resourceHash = resource->mNameHash;
    return ownHash == resourceHash;
}
/* 0276F2E0 */
const sead::SafeText* Fog::groupData() const { return &mGroup.mValue; }
/* 0276F2E8 */
u32 Fog::isType(const EnvironmentTypeInfo* type) const {
    if (sFogTypeReady == 0) {
        sFogTypeReady = 1;
        sFogType.mMethods = &sFogTypeMethods;
    }
    if (type == &sFogType) return 1;
    if (sEnvironmentTypeReady == 0) {
        sEnvironmentTypeReady = 1;
        sEnvironmentType.mMethods = &sFogBaseTypeMethods;
    }
    return type == &sEnvironmentType;
}
/* 0276F360 */
s32 Fog::registeredTypeId() { return sFogClass->mTypeId; }

void releaseProjectorObject(void*);
void destroyProjectorEnvironmentBase(EnvironmentObject*,u32);

/* 0276D780 */
void ProjectorClassDescriptor::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseProjectorObject),this);
}
/* 0276D794 */
void Projector::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mParameterIoMethods = &sProjectorIoMethods;
    destroyProjectorEnvironmentBase(this,0);
    if (flags & 1) releaseProjectorObject(this);
}
/* 0276D7F4 */
const sead::SafeText* Projector::groupText() const {
    return gabi::tail_ptr<const sead::SafeText*>(mMethods->mGroupText.mFunction,this);
}
/* 0276D80C */
s32 Projector::typeId() const {
    return gabi::tail_ptr<s32>(mMethods->mTypeId.mFunction,this);
}
/* 0276D8CC */
const sead::SafeText* Projector::emptyHook() const {
    return gabi::tail_ptr<const sead::SafeText*>(mMethods->mEmptyHook.mFunction,this);
}

void concatenateProjectionView(sead::RenderMatrix44*,const sead::RenderMatrix44*,const sead::RenderMatrix34*);
/* 0276D6D0 */
void Projector::updateViewProjection() {
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)concatenateProjectionView),&mViewProjection,&mProjection,&mView);
}


typedef be<u8> ProjectorTextByte;
static EnvironmentObjectMethods sProjectorObjectMethods;
static sead::SafeTextVtable sProjectorTextMethods;
static sead::VtableSlot sProjectorIntegerMethods;
static sead::VtableSlot sProjectorVectorMethods;
static sead::VtableSlot sProjectorScalarMethods;
static ProjectorTextByte sProjectionTypeName;
static ProjectorTextByte sProjectionTypeDescription;
static ProjectorTextByte sProjectionTypeHint;
static ProjectorTextByte sViewPositionName;
static ProjectorTextByte sViewPositionDescription;
static ProjectorTextByte sViewPositionHint;
static ProjectorTextByte sViewTargetName;
static ProjectorTextByte sViewTargetDescription;
static ProjectorTextByte sViewTargetHint;
static ProjectorTextByte sViewUpName;
static ProjectorTextByte sViewUpDescription;
static ProjectorTextByte sViewUpHint;
static ProjectorTextByte sNearPlaneName;
static ProjectorTextByte sNearPlaneDescription;
static ProjectorTextByte sNearPlaneHint;
static ProjectorTextByte sFarPlaneName;
static ProjectorTextByte sFarPlaneDescription;
static ProjectorTextByte sFarPlaneHint;
static ProjectorTextByte sAspectName;
static ProjectorTextByte sAspectDescription;
static ProjectorTextByte sAspectHint;
static ProjectorTextByte sFieldOfViewName;
static ProjectorTextByte sFieldOfViewDescription;
static ProjectorTextByte sFieldOfViewHint;
static ProjectorTextByte sHeightName;
static ProjectorTextByte sHeightDescription;
static ProjectorTextByte sHeightHint;
static f32 sProjectorPositionFlat;
static f32 sProjectorPositionHeight;
static f32 sProjectorTargetDepth;
static f32 sProjectorUnit;
static f32 sProjectorFarDefault;
static f32 sProjectorFovDefault;

void* allocateProjectorStorage(u32);
EnvironmentObject* constructProjectorBase(EnvironmentObject*);
ParameterBinding* constructProjectorBinding(ParameterBinding*);
void registerProjectorParameter(ParameterBinding*,const sead::SafeText*,const sead::SafeText*,const sead::SafeText*,EnvironmentObject*);
void initializeProjectorView(sead::RenderMatrix34*);
void initializeProjectorProjection(sead::RenderMatrix44*);

/* inlined into Projector::construct */
static void initializeProjectorText(sead::SafeText* text,ProjectorTextByte* value) {
    text->mText = value;
    text->mVtable = &sProjectorTextMethods;
}
/* inlined into Projector::construct */
template<class T> static T* initializeProjectorParameter(T* parameter,Projector* owner,
    ProjectorTextByte* name,ProjectorTextByte* description,ProjectorTextByte* hint,
    sead::VtableSlot* methods,u32 nameSlot,u32 descriptionSlot,u32 hintSlot) {
    gabi::FrameLocal<sead::SafeText> nameText(nameSlot),descriptionText(descriptionSlot),hintText(hintSlot);
    initializeProjectorText(nameText.get(),name);
    initializeProjectorText(descriptionText.get(),description);
    if (gabi::ea(parameter) == 0) parameter = static_cast<T*>(allocateProjectorStorage(sizeof(T)));
    if (parameter != NULL) {
        constructProjectorBinding(&parameter->mBinding);
        parameter->mBinding.mMethods = methods;
        initializeProjectorText(hintText.get(),hint);
        registerProjectorParameter(&parameter->mBinding,nameText.get(),descriptionText.get(),hintText.get(),owner);
    }
    return parameter;
}

/* 0276CE10 */
Projector* Projector::construct(Projector* object,u32 projectionKind) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = static_cast<Projector*>(allocateProjectorStorage(sizeof(Projector)));
    if (object != NULL) {
        constructProjectorBase(object);
        object->mMethods = &sProjectorObjectMethods;
        object->mParameterIoMethods = &sProjectorIoMethods;
        ProjectionIntegerParameter* projectionType = initializeProjectorParameter(&object->mProjectionType,object,
            &sProjectionTypeName,&sProjectionTypeDescription,&sProjectionTypeHint,&sProjectorIntegerMethods,8,16,152);
        if (projectionType != NULL) {
            projectionType->mValue = projectionKind;
        }
        f32 flat = sProjectorPositionFlat;
        f32 defaultHeight = sProjectorPositionHeight;
        ProjectionVectorParameter* viewPosition = initializeProjectorParameter(&object->mViewPosition,object,
            &sViewPositionName,&sViewPositionDescription,&sViewPositionHint,&sProjectorVectorMethods,24,32,160);
        if (viewPosition != NULL) {
            viewPosition->mValue.x = flat;
            viewPosition->mValue.z = flat;
            viewPosition->mValue.y = defaultHeight;
        }
        ProjectionVectorParameter* viewTarget = initializeProjectorParameter(&object->mViewTarget,object,
            &sViewTargetName,&sViewTargetDescription,&sViewTargetHint,&sProjectorVectorMethods,40,48,168);
        if (viewTarget != NULL) {
            viewTarget->mValue.x = flat;
            f32 targetDepth = sProjectorTargetDepth;
            viewTarget->mValue.y = flat;
            viewTarget->mValue.z = targetDepth;
        }
        f32 unit = sProjectorUnit;
        ProjectionVectorParameter* viewUp = initializeProjectorParameter(&object->mViewUp,object,
            &sViewUpName,&sViewUpDescription,&sViewUpHint,&sProjectorVectorMethods,56,64,176);
        if (viewUp != NULL) {
            viewUp->mValue.x = flat;
            viewUp->mValue.z = unit;
            viewUp->mValue.y = flat;
        }
        ProjectionScalarParameter* nearPlane = initializeProjectorParameter(&object->mNearPlane,object,
            &sNearPlaneName,&sNearPlaneDescription,&sNearPlaneHint,&sProjectorScalarMethods,72,80,184);
        if (nearPlane != NULL) {
            nearPlane->mValue = unit;
        }
        ProjectionScalarParameter* farPlane = initializeProjectorParameter(&object->mFarPlane,object,
            &sFarPlaneName,&sFarPlaneDescription,&sFarPlaneHint,&sProjectorScalarMethods,88,96,192);
        if (farPlane != NULL) {
            farPlane->mValue = sProjectorFarDefault;
        }
        ProjectionScalarParameter* aspect = initializeProjectorParameter(&object->mAspect,object,
            &sAspectName,&sAspectDescription,&sAspectHint,&sProjectorScalarMethods,104,112,200);
        if (aspect != NULL) {
            aspect->mValue = unit;
        }
        ProjectionScalarParameter* fieldOfView = initializeProjectorParameter(&object->mFieldOfView,object,
            &sFieldOfViewName,&sFieldOfViewDescription,&sFieldOfViewHint,&sProjectorScalarMethods,120,128,208);
        if (fieldOfView != NULL) {
            fieldOfView->mValue = sProjectorFovDefault;
        }
        ProjectionScalarParameter* height = initializeProjectorParameter(&object->mHeight,object,
            &sHeightName,&sHeightDescription,&sHeightHint,&sProjectorScalarMethods,136,144,216);
        if (height != NULL) {
            height->mValue = defaultHeight;
        }
        // Placement construction performs these allocation checks even though the pointers are discarded.
        if (gabi::ea(&object->mView) == 0) allocateProjectorStorage(sizeof(object->mView));
        if (gabi::ea(&object->mProjection) == 0) allocateProjectorStorage(sizeof(object->mProjection));
        if (gabi::ea(&object->mViewProjection) == 0) allocateProjectorStorage(sizeof(object->mViewProjection));
        initializeProjectorView(&object->mView);
        initializeProjectorProjection(&object->mProjection);
        initializeProjectorProjection(&object->mViewProjection);
    }
    std::atomic_thread_fence(std::memory_order_acquire);
    return object;
}


void* allocateEnvironmentFromHeap(u32,sead::RegistryHeap*,s32);
Projector* constructHeapProjector(Projector*,u32);
Fog* constructHeapFog(Fog*);

/* 0276D96C */
Projector* Projector::create(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    Projector* object = static_cast<Projector*>(allocateEnvironmentFromHeap(sizeof(Projector),heap,4));
    if (object != NULL) object = constructHeapProjector(object,0);
    return object;
}
/* 0276F370 */
Fog* Fog::create(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    Fog* object = static_cast<Fog*>(allocateEnvironmentFromHeap(sizeof(Fog),heap,4));
    if (object != NULL) object = constructHeapFog(object);
    return object;
}


Projector* createProjectorClassObject(sead::RegistryHeap*);
Fog* createFogClassObject(sead::RegistryHeap*);
EnvironmentClassDescriptor* registerEnvironmentClass(const sead::SafeText*,const sead::SafeText*,u32,u32);
static ProjectorTextByte sProjectorClassName;
static ProjectorTextByte sProjectorClassDescription;
static ProjectorTextByte sFogClassName;
static ProjectorTextByte sFogClassDescription;
static sead::SafeTextVtable sFogClassTextMethods;

/* 0276D71C */
void Projector::registerClass() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::SafeText> name(8),description(16);
    initializeProjectorText(name.get(),&sProjectorClassName);
    initializeProjectorText(description.get(),&sProjectorClassDescription);
    sProjectorClass = registerEnvironmentClass(name.get(),description.get(),
        gabi::guest_fn((const void*)createProjectorClassObject),2);
}
/* 0276F11C */
void Fog::registerClass() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::SafeText> name(8),description(16);
    name->mVtable = &sFogClassTextMethods;
    description->mVtable = &sFogClassTextMethods;
    name->mText = &sFogClassName;
    description->mText = &sFogClassDescription;
    sFogClass = registerEnvironmentClass(name.get(),description.get(),
        gabi::guest_fn((const void*)createFogClassObject),1);
}

/* WWHD glue
 ext 0276D96C Projector* createProjectorClassObject(sead::RegistryHeap*) callback
 ext 0276F370 Fog* createFogClassObject(sead::RegistryHeap*) callback
 ext 02770250 EnvironmentClassDescriptor* registerEnvironmentClass(const sead::SafeText*,const sead::SafeText*,u32,u32)
 data sProjectorClassName 10160024
 data sProjectorClassDescription 1016001C
 data sFogClassName 101606B4
 data sFogClassDescription 101606B8
 data sFogClassTextMethods 1016044C
 ext 0273B050 void* allocateEnvironmentFromHeap(u32,sead::RegistryHeap*,s32)
 ext 0276CE10 Projector* constructHeapProjector(Projector*,u32)
 ext 0276EE70 Fog* constructHeapFog(Fog*)
 data sProjectorObjectMethods 10160030
 data sProjectorTextMethods 1015FDC0
 data sProjectorIntegerMethods 1015FDF8
 data sProjectorVectorMethods 1015FEE8
 data sProjectorScalarMethods 1015FE70
 data sProjectionTypeName 1015FFD4
 data sProjectionTypeDescription 1015FF84
 data sProjectionTypeHint 1015FDBC
 data sViewPositionName 1015FFE0
 data sViewPositionDescription 1015FFEC
 data sViewPositionHint 1015FDBE
 data sViewTargetName 1015FF70
 data sViewTargetDescription 1015FF8C
 data sViewTargetHint 1015FDBE
 data sViewUpName 1015FF78
 data sViewUpDescription 1015FF94
 data sViewUpHint 1015FDBE
 data sNearPlaneName 1015FF9C
 data sNearPlaneDescription 1015FFA4
 data sNearPlaneHint 1015FDBD
 data sFarPlaneName 1015FF80
 data sFarPlaneDescription 1015FFAC
 data sFarPlaneHint 1015FDBD
 data sAspectName 1015FFB4
 data sAspectDescription 1015FFF8
 data sAspectHint 1015FDBD
 data sFieldOfViewName 1015FFBC
 data sFieldOfViewDescription 10160004
 data sFieldOfViewHint 1015FDBD
 data sHeightName 1015FFC4
 data sHeightDescription 1015FFCC
 data sHeightHint 1015FDBD
 data sProjectorPositionFlat 1015FDB4
 data sProjectorPositionHeight 1015FF60
 data sProjectorTargetDepth 1015FF64
 data sProjectorUnit 1015FDB8
 data sProjectorFarDefault 1015FF68
 data sProjectorFovDefault 1015FF6C
 ext 0273AD10 void* allocateProjectorStorage(u32)
 ext 0276FCA0 EnvironmentObject* constructProjectorBase(EnvironmentObject*)
 ext 0279E674 ParameterBinding* constructProjectorBinding(ParameterBinding*)
 ext 0279E618 void registerProjectorParameter(ParameterBinding*,const sead::SafeText*,const sead::SafeText*,const sead::SafeText*,EnvironmentObject*)
 ext 028E9098 void initializeProjectorView(sead::RenderMatrix34*)
 ext 028E892C void initializeProjectorProjection(sead::RenderMatrix44*)
 ext 0272D680 void concatenateProjectionView(sead::RenderMatrix44*,const sead::RenderMatrix44*,const sead::RenderMatrix34*) callback
 data sProjectorIoMethods 101600E0
 ext 0273AF40 void releaseProjectorObject(void*) callback
 ext 0276FC20 void destroyProjectorEnvironmentBase(EnvironmentObject*,u32)
 data sProjectorTypeReady 101FD7E0
 data sFogTypeReady 101FD7DC
 data sEnvironmentTypeReady 101FD7C4
 data sProjectorType 101FDD14
 data sFogType 101FDCE0
 data sEnvironmentType 101FDCD0
 data sProjectorTypeMethods 1015FDE8
 data sFogTypeMethods 10160474
 data sProjectorBaseTypeMethods 1015FDD8
 data sFogBaseTypeMethods 10160464
 data sProjectorClass 104A145C
 data sFogClass 104A1460
*/
