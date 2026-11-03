// Base environment construction and its bounded, owned text parameters.
#include "agl/EnvironmentObject.h"
using namespace agl::env;
typedef be<u8> EnvironmentBaseTextByte;
static EnvironmentObjectMethods sEnvironmentObjectMethods;
static sead::VtableSlot sEnvironmentIoMethods;
static sead::VtableSlot sEnvironmentBooleanMethods;
static sead::VtableSlot sEnvironmentTextParameterMethods;
static sead::SafeTextVtable sEnvironmentTextMethods;
static sead::SafeTextVtable sEnvironmentBufferBaseMethods;
static sead::SafeTextVtable sEnvironmentBufferMethods;
static sead::SafeTextVtable sEnvironmentFixedTextMethods;
static EnvironmentBaseTextByte sEnabledName;
static EnvironmentBaseTextByte sEnabledDescription;
static EnvironmentBaseTextByte sEnabledHint;
static EnvironmentBaseTextByte sInitialGroup;
static EnvironmentBaseTextByte sGroupName;
static EnvironmentBaseTextByte sGroupDescription;
static EnvironmentBaseTextByte sTextHint;
static EnvironmentBaseTextByte sNameName;
static EnvironmentBaseTextByte sNameDescription;
void* allocateEnvironmentBase(u32);
void constructEnvironmentOwner(EnvironmentObject*);
void constructEnvironmentIo(sead::VtableSlot*);
void destroyEnvironmentIo(sead::VtableSlot*,u32);
void releaseEnvironmentBase(void*);
void prepareEnvironmentText(const sead::SafeText*);
ParameterBinding* constructEnvironmentBinding(ParameterBinding*);
void attachEnvironmentBinding(ParameterBinding*,const sead::SafeText*,const sead::SafeText*,const sead::SafeText*,EnvironmentObject*);
const sead::SafeText* defaultEnvironmentName();
void moveEnvironmentText(void*,const void*,u32,u32);

/* inlined into EnvironmentObject::construct */
static void setEnvironmentText(sead::SafeText* text,EnvironmentBaseTextByte* value) {
    text->mText = value;
    text->mVtable = &sEnvironmentTextMethods;
}
/* inlined into EnvironmentObject::construct */
static s32 environmentTextLength(const be<u8>* text) {
    s32 length = 0;
    while (text[length] != 0) {
        if (++length > sead::kMaxTextLength) return 0;
    }
    return length;
}
/* inlined into EnvironmentObject::construct */
static void copyEnvironmentText(be<u8>* destination,const be<s32>* capacity,const sead::SafeText* source,bool assureBefore) {
    if (assureBefore) const_cast<sead::SafeText*>(source)->assureTermination();
    s32 length = environmentTextLength(source->mText);
    s32 maximum = *capacity;
    if (length >= maximum) length = maximum - 1;
    const_cast<sead::SafeText*>(source)->assureTermination();
    moveEnvironmentText(destination,source->mText,u32(length),0);
    destination[length] = 0;
}
/* inlined into EnvironmentObject::construct */
static void initializeEnvironmentTextParameter(EnvironmentTextParameter* parameter,EnvironmentObject* owner,
    const sead::SafeText* name,const sead::SafeText* description,const sead::SafeText* hint,
    const sead::SafeText* initial) {
    if (gabi::ea(parameter) == 0) parameter = static_cast<EnvironmentTextParameter*>(allocateEnvironmentBase(sizeof(EnvironmentTextParameter)));
    if (parameter == NULL) return;
    constructEnvironmentBinding(&parameter->mBinding);
    parameter->mBinding.mMethods = &sEnvironmentTextParameterMethods;
    sead::SafeTextBuffer* value = &parameter->mValue;
    if (gabi::ea(value) == 0) value = static_cast<sead::SafeTextBuffer*>(allocateEnvironmentBase(sizeof(sead::SafeTextBuffer)));
    if (value != NULL) {
        value->mVtable = &sEnvironmentBufferBaseMethods;
        value->mText = parameter->mValue.mStorage;
        value->mCapacity = 32;
        parameter->mValue.mStorage[31] = 0;
    }
    be<u8>* firstCharacter = parameter->mValue.mText;
    parameter->mValue.mVtable = &sEnvironmentBufferMethods;
    firstCharacter[0] = 0;
    parameter->mValue.mVtable = &sEnvironmentFixedTextMethods;
    attachEnvironmentBinding(&parameter->mBinding,name,description,hint,owner);
    be<u8>* destination = parameter->mValue.mText;
    copyEnvironmentText(destination,&parameter->mValue.mCapacity,initial,true);
}

/* 0276FCA0 */
EnvironmentObject* EnvironmentObject::construct(EnvironmentObject* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = static_cast<EnvironmentObject*>(allocateEnvironmentBase(sizeof(EnvironmentObject)));
    if (object == NULL) return NULL;
    constructEnvironmentOwner(object);
    constructEnvironmentIo(reinterpret_cast<sead::VtableSlot*>(&object->mParameterIoMethods));
    object->mMethods = &sEnvironmentObjectMethods;
    object->mParameterIoMethods = &sEnvironmentIoMethods;
    object->mManager = NULL;
    gabi::FrameLocal<sead::SafeText> enabledName(16),enabledDescription(24),enabledHint(80);
    setEnvironmentText(enabledName.get(),&sEnabledName);
    setEnvironmentText(enabledDescription.get(),&sEnabledDescription);
    EnvironmentBooleanParameter* enabled = &object->mEnabled;
    if (gabi::ea(enabled) == 0) enabled = static_cast<EnvironmentBooleanParameter*>(allocateEnvironmentBase(sizeof(EnvironmentBooleanParameter)));
    if (enabled != NULL) {
        constructEnvironmentBinding(&enabled->mBinding);
        enabled->mBinding.mMethods = &sEnvironmentBooleanMethods;
        setEnvironmentText(enabledHint.get(),&sEnabledHint);
        attachEnvironmentBinding(&enabled->mBinding,enabledName.get(),enabledDescription.get(),enabledHint.get(),object);
        enabled->mValue = 0;
    }
    gabi::FrameLocal<sead::SafeText> initialGroup(8);
    setEnvironmentText(initialGroup.get(),&sInitialGroup);
    gabi::FrameLocal<sead::FixedSafeText<32>> groupValue(88);
    groupValue->mText = groupValue->mStorage;
    groupValue->mVtable = &sEnvironmentBufferMethods;
    groupValue->mCapacity = 32;
    groupValue->mStorage[31] = 0;
    prepareEnvironmentText(initialGroup.get());
    copyEnvironmentText(groupValue->mStorage,&groupValue->mCapacity,initialGroup.get(),false);
    gabi::FrameLocal<sead::SafeText> groupName(32),groupDescription(40),groupHint(48);
    setEnvironmentText(groupName.get(),&sGroupName);
    setEnvironmentText(groupDescription.get(),&sGroupDescription);
    setEnvironmentText(groupHint.get(),&sTextHint);
    groupValue->mVtable = &sEnvironmentFixedTextMethods;
    initializeEnvironmentTextParameter(&object->mGroup,object,groupName.get(),groupDescription.get(),groupHint.get(),groupValue.get());
    const sead::SafeText* defaultName = defaultEnvironmentName();
    gabi::FrameLocal<sead::FixedSafeText<32>> nameValue(132);
    nameValue->mVtable = &sEnvironmentBufferMethods;
    nameValue->mCapacity = 32;
    nameValue->mText = nameValue->mStorage;
    nameValue->mStorage[31] = 0;
    copyEnvironmentText(nameValue->mStorage,&nameValue->mCapacity,defaultName,true);
    gabi::FrameLocal<sead::SafeText> nameName(56),nameDescription(64),nameHint(72);
    setEnvironmentText(nameName.get(),&sNameName);
    setEnvironmentText(nameDescription.get(),&sNameDescription);
    setEnvironmentText(nameHint.get(),&sTextHint);
    nameValue->mVtable = &sEnvironmentFixedTextMethods;
    initializeEnvironmentTextParameter(&object->mName,object,nameName.get(),nameDescription.get(),nameHint.get(),nameValue.get());
    be<u8>* flags = &object->mFlags;
    object->mObjectIndex = 0;
    if (gabi::ea(flags) == 0) flags = static_cast<be<u8>*>(allocateEnvironmentBase(1));
    if (flags != NULL) *flags = 1;
    return object;
}
/* 0276FC20 */
void EnvironmentObject::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mParameterIoMethods = &sEnvironmentIoMethods;
    destroyEnvironmentIo(reinterpret_cast<sead::VtableSlot*>(&mParameterIoMethods),0);
    if (flags & 1) releaseEnvironmentBase(this);
}
/* 0277019C */
void EnvironmentObject::setEnabled(u32 enabled) {
    u32 previous = mEnabled.mValue != 0;
    if (previous == enabled) return;
    EnvironmentManagerState* manager = mManager;
    mEnabled.mValue = u8(enabled);
    manager->mDirtyFlags = u32(manager->mDirtyFlags) | 1;
}


void refreshEnvironmentGroupName(EnvironmentObject*);
void disableEnvironmentObject(EnvironmentObject*,u32);

/* 02770480 */
void EnvironmentObject::resetNameAndDisable() {
    gabi::AutoFrame frame{gabi::current_function()};
    refreshEnvironmentGroupName(this);
    const sead::SafeText* initial = defaultEnvironmentName();
    be<u8>* destination = mName.mValue.mText;
    copyEnvironmentText(destination,&mName.mValue.mCapacity,initial,true);
    disableEnvironmentObject(this,0);
}
/* 02770638 */
void EnvironmentObject::finishReset() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::call_ptr<void>(mMethods->mReset.mFunction,this);
    mFlags = u8(mFlags) & 0xDF;
}

/* WWHD glue
 ext 027701D4 void refreshEnvironmentGroupName(EnvironmentObject*)
 ext 0277019C void disableEnvironmentObject(EnvironmentObject*,u32)
 data sEnvironmentObjectMethods 10160A2C
 data sEnvironmentIoMethods 10160ADC
 data sEnvironmentBooleanMethods 10160894
 data sEnvironmentTextParameterMethods 1016090C
 data sEnvironmentTextMethods 101607DC
 data sEnvironmentBufferBaseMethods 101607F4
 data sEnvironmentBufferMethods 1016081C
 data sEnvironmentFixedTextMethods 10160834
 data sEnabledName 10160990
 data sEnabledDescription 10160998
 data sEnabledHint 101607D8
 data sInitialGroup 10160984
 data sGroupName 101609A0
 data sGroupDescription 101609A8
 data sTextHint 101609B8
 data sNameName 10160988
 data sNameDescription 101609D0
 ext 0273AD10 void* allocateEnvironmentBase(u32)
 ext 027A84B8 void constructEnvironmentOwner(EnvironmentObject*)
 ext 027AE5D8 void constructEnvironmentIo(sead::VtableSlot*)
 ext 027AE618 void destroyEnvironmentIo(sead::VtableSlot*,u32)
 ext 0273AF40 void releaseEnvironmentBase(void*)
 ext 02770710 void prepareEnvironmentText(const sead::SafeText*)
 ext 0279E674 ParameterBinding* constructEnvironmentBinding(ParameterBinding*)
 ext 0279E618 void attachEnvironmentBinding(ParameterBinding*,const sead::SafeText*,const sead::SafeText*,const sead::SafeText*,EnvironmentObject*)
 ext 027AE62C const sead::SafeText* defaultEnvironmentName()
 ext C0009988 void moveEnvironmentText(void*,const void*,u32,u32)
*/
