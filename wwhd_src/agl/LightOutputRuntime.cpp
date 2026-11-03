// Light output storage and transforms; heap and matrix operations remain calls through their interfaces.
#include "agl/EnvironmentLights.h"
#include "sead/HeapRegistry.h"
using namespace agl::env;
sead::RegistryHeap* currentLightHeap(sead::RegistryManagerState* manager);
sead::RegistryHeap* containingLightHeap(sead::RegistryManagerState* manager,void* storage);
void destroyLightEnvironmentBase(EnvironmentObject* object,u32 flags);
void deleteLightObject(void* object);
void multiplyLightDirection(const sead::RenderMatrix34* matrix,const agl::env::ParameterVector3* source,agl::env::ParameterVector3* destination);
void multiplyLightPosition(const sead::RenderMatrix34* matrix,const agl::env::ParameterVector3* source,agl::env::ParameterVector3* destination);
void normalizeLightDirection(agl::env::ParameterVector3* vector);
static agl::env::ParameterVector3* lightOutputElement(agl::env::ParameterVector3* values,u32 index) {
    return gabi::at<agl::env::ParameterVector3>(gabi::ea(values)+index*12);
}
static sead::VtableSlot sAmbientIoMethods;
static EnvironmentObjectMethods sHemisphereMethods;
static sead::VtableSlot sHemisphereIoMethods;
static EnvironmentObjectMethods sDirectionalMethods;
static sead::VtableSlot sDirectionalIoMethods;
static EnvironmentObjectMethods sPointMethods;
static sead::VtableSlot sPointIoMethods;
static EnvironmentObjectMethods sConeMethods;
static sead::VtableSlot sConeIoMethods;
static void releaseLightOutputs(LightOutputArray* outputs) {
    agl::env::ParameterVector3* values=outputs->mValues;
    if(values!=nullptr) {
        u32 count=outputs->mCount;
        (void)count;
        sead::RegistryHeap* heap=containingLightHeap(sRegistryManager,values);
        u32 release=heap->dispatch()->mRelease.mFunction;
        gabi::call_ptr<void>(release,heap,outputs->mValues.get());
        outputs->mCount=0;outputs->mValues=nullptr;
    }
}
static void allocateLightOutputs(LightOutputArray* outputs,s32 count,sead::RegistryHeap* heap) {
    if(count>0) {
        if(heap==nullptr) heap=currentLightHeap(sRegistryManager);
        u32 allocate=heap->dispatch()->mAllocate.mFunction;
        agl::env::ParameterVector3* values=gabi::call_ptr<agl::env::ParameterVector3*>(allocate,heap,u32(count)*12,4);
        if(values!=nullptr) { outputs->mValues=values;outputs->mCount=count; }
    }
}
/* 02770A18 */
static void destroyHemisphereLight(HemisphereLight* light,u32 flags) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(light!=nullptr) {
        light->mMethods=&sHemisphereMethods;light->mParameterIoMethods=&sHemisphereIoMethods;
        releaseLightOutputs(&light->mOutputs);
        destroyLightEnvironmentBase(light,0);
        if(flags&1) deleteLightObject(light);
    }
}
/* 02770AC4 */
static void destroyDirectionalLight(DirectionalLight* light,u32 flags) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(light!=nullptr) {
        light->mMethods=&sDirectionalMethods;light->mParameterIoMethods=&sDirectionalIoMethods;
        releaseLightOutputs(&light->mOutputs);
        destroyLightEnvironmentBase(light,0);
        if(flags&1) deleteLightObject(light);
    }
}
/* 02770B70 */
static void destroyPointLight(PointLight* light,u32 flags) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(light!=nullptr) {
        light->mMethods=&sPointMethods;light->mParameterIoMethods=&sPointIoMethods;
        releaseLightOutputs(&light->mOutputs);
        destroyLightEnvironmentBase(light,0);
        if(flags&1) deleteLightObject(light);
    }
}
/* 02770C1C */
static void destroyConeLight(ConeLight* light,u32 flags) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(light!=nullptr) {
        light->mMethods=&sConeMethods;light->mParameterIoMethods=&sConeIoMethods;
        releaseLightOutputs(&light->mPositionOutputs);
        releaseLightOutputs(&light->mDirectionOutputs);
        destroyLightEnvironmentBase(light,0);
        if(flags&1) deleteLightObject(light);
    }
}
/* 02771218 */
void HemisphereLight::allocateOutputs(s32 count,void* heap) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    allocateLightOutputs(&mOutputs,count,static_cast<sead::RegistryHeap*>(heap));
}
/* 027712A0 */
void HemisphereLight::transform(const sead::RenderMatrix34* matrix,u32,u32 index) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    u32 count=mOutputs.mCount;agl::env::ParameterVector3* output=mOutputs.mValues;
    if(index<count) output=lightOutputElement(output,index);
    multiplyLightDirection(matrix,&mDirection.mValue,output);
    count=mOutputs.mCount;output=mOutputs.mValues;
    if(index<count) output=lightOutputElement(output,index);
    normalizeLightDirection(output);
}
/* 02771888 */
void DirectionalLight::allocateOutputs(s32 count,void* heap) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    allocateLightOutputs(&mOutputs,count,static_cast<sead::RegistryHeap*>(heap));
}
/* 02771910 */
void DirectionalLight::transform(const sead::RenderMatrix34* matrix,u32,u32 index) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    u8 view=mViewCoordinates.mValue;
    u32 count=mOutputs.mCount;agl::env::ParameterVector3* output=mOutputs.mValues;
    if(view==0) {
        if(index<count) output=lightOutputElement(output,index);
        multiplyLightDirection(matrix,&mDirection.mValue,output);
    } else {
        if(index<count) output=lightOutputElement(output,index);
        output->x=(f32)mDirection.mValue.x;
        output->y=(f32)mDirection.mValue.y;
        output->z=(f32)mDirection.mValue.z;
    }
    count=mOutputs.mCount;output=mOutputs.mValues;
    if(index<count) output=lightOutputElement(output,index);
    normalizeLightDirection(output);
}
/* 02771DC0 */
void PointLight::allocateOutputs(s32 count,void* heap) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    allocateLightOutputs(&mOutputs,count,static_cast<sead::RegistryHeap*>(heap));
}
/* 02771E48 */
void PointLight::transform(const sead::RenderMatrix34* matrix,u32,u32 index) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    u32 count=mOutputs.mCount;agl::env::ParameterVector3* output=mOutputs.mValues;
    if(index<count) output=lightOutputElement(output,index);
    multiplyLightPosition(matrix,&mPosition.mValue,output);
}
/* 0277243C */
void ConeLight::allocateOutputs(s32 count,void* heap) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    allocateLightOutputs(&mPositionOutputs,count,static_cast<sead::RegistryHeap*>(heap));
    allocateLightOutputs(&mDirectionOutputs,count,static_cast<sead::RegistryHeap*>(heap));
}
/* 0277252C */
void ConeLight::transform(const sead::RenderMatrix34* matrix,u32,u32 index) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    u32 count=mPositionOutputs.mCount;agl::env::ParameterVector3* output=mPositionOutputs.mValues;
    if(index<count) output=lightOutputElement(output,index);
    multiplyLightPosition(matrix,&mPosition.mValue,output);
    count=mDirectionOutputs.mCount;output=mDirectionOutputs.mValues;
    if(index<count) output=lightOutputElement(output,index);
    multiplyLightDirection(matrix,&mDirection.mValue,output);
    count=mDirectionOutputs.mCount;output=mDirectionOutputs.mValues;
    if(index<count) output=lightOutputElement(output,index);
    normalizeLightDirection(output);
}
/* 02772750 */
static void destroyAmbientLight(EnvironmentObject* light,u32 flags) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(light!=nullptr) {
        light->mParameterIoMethods=&sAmbientIoMethods;
        destroyLightEnvironmentBase(light,0);
        if(flags&1) deleteLightObject(light);
    }
}
/* 0277294C */
static void emptyAmbientLightCallback() {}

f32 normalizeDirectionInput(agl::env::ParameterVector3* output,const agl::env::ParameterVector3* input);
f32 normalizeDirectionPlane(sead::RenderVector2* vector);
f32 lightAtan2(f32 y,f32 x);
f32 lightArcSine(f32 value);
/* 02771314 */
void DirectionalLight::refreshAngles() {
    auto frame=gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<agl::env::ParameterVector3> direction(0x10);
    f32 magnitude=normalizeDirectionInput(direction.get(),&mDirection.mValue);
    if(magnitude>0.0f) {
        gabi::FrameLocal<sead::RenderVector2> plane(8);
        plane->x=-(f32)direction->z;
        plane->y=-(f32)direction->x;
        f32 planeMagnitude=normalizeDirectionPlane(plane.get());
        if(planeMagnitude>0.0f) mYaw=lightAtan2(plane->y,plane->x);
        f32 pitch=-(f32)direction->y;
        if(pitch < -1.0f) pitch=-1.0f;
        else if(pitch > 1.0f) pitch=1.0f;
        mPitch=lightArcSine(pitch);
    }
}
/* 027719CC */
static void refreshDirectionalAngles(DirectionalLight* light) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    light->refreshAngles();
}
/* WWHD glue
ext 02756140 sead::RegistryHeap* currentLightHeap(sead::RegistryManagerState*)
ext 02755FEC sead::RegistryHeap* containingLightHeap(sead::RegistryManagerState*,void*)
ext 0276FC20 void destroyLightEnvironmentBase(agl::env::EnvironmentObject*,u32)
ext 0273AF40 void deleteLightObject(void*)
ext 028E9044 void multiplyLightDirection(const sead::RenderMatrix34*,const agl::env::ParameterVector3*,agl::env::ParameterVector3*)
ext 028E8F64 void multiplyLightPosition(const sead::RenderMatrix34*,const agl::env::ParameterVector3*,agl::env::ParameterVector3*)
ext 025155D8 void normalizeLightDirection(agl::env::ParameterVector3*)
data sAmbientIoMethods 10161218
data sHemisphereMethods 101613E0
data sHemisphereIoMethods 10161250
data sDirectionalMethods 10161490
data sDirectionalIoMethods 10161288
data sPointMethods 10161540
data sPointIoMethods 101612C0
data sConeMethods 101615F0
data sConeIoMethods 101612F8

ext 02772C74 f32 normalizeDirectionInput(agl::env::ParameterVector3*,const agl::env::ParameterVector3*)
ext 0261AAA0 f32 normalizeDirectionPlane(sead::RenderVector2*)
ext 028F4D28 f32 lightAtan2(f32,f32)
ext 028F4F20 f32 lightArcSine(f32)
*/
