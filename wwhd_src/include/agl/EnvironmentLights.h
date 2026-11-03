// The built-in light classes (agl::env lights on EnvironmentObject) and the per-translation-unit copies of the
// environment parameter accessors that AGL#2's tables point at.
#pragma once
#include "wwhd.h"
#include "sead/RenderMath.h"
#include "agl/EnvironmentObject.h"
namespace agl { namespace env {
// Layout-only twins of the merged parameter types for the light translation unit's accessor copies (027727EC-
// 02772864; the light parameter tables point at these, the merged types' tables at their own copies; GHS emits one
// copy per translation unit, instruction-identical).
struct LightBooleanParameter : EnvironmentBooleanParameter {
    u32 zeroCode(); be<u8>* value(); const be<u8>* value() const; u32 valueBytes() const; u32 valueKind() const;
};
struct LightScalarParameter : EnvironmentScalarParameter {
    be<f32>* value(); const be<f32>* value() const; u32 valueBytes() const; u32 valueKind() const;
};
struct LightVectorParameter : EnvironmentVectorParameter {
    ParameterVector3* value(); const ParameterVector3* value() const; u32 valueBytes() const; u32 valueKind() const;
};
struct LightColorParameter : EnvironmentColorParameter {
    ParameterColor* value(); const ParameterColor* value() const; u32 valueBytes() const;
};
// The common base of the five light classes: EnvironmentObject with the light translation unit's copy of the base
// method set (table slots as in sEnvironmentObjectMethods; 027727B0-027728BC).
struct Light : EnvironmentObject {
    const sead::SafeText* groupText() const;
    const sead::SafeText* nameData() const;
    s32 typeId() const;
    u32 enabled() const;
    void emptyQuery();
    u32 matchesResource(const EnvironmentResourceReference*) const;
    void onPrepare();
    void onUpdate();
    void onPostUpdate();
    void onEmptyHook();
    const sead::SafeText* emptyHook() const;
    const sead::SafeText* groupData() const;
};
// A light's parameter-I/O interface at this+0x5C (mParameterIoMethods): the thunks of the light I/O tables (e.g.
// 10161218, adjustment 0xFFA4 = -0x5C; 027709F8-02770A10) call the light base methods on the object.
struct LightIo {
    gptr<sead::VtableSlot> mMethods;
    Light* object();
    const sead::SafeText* groupText();
    const sead::SafeText* nameData();
    s32 typeId();
    u32 enabled();
};
// The per-view transformed directions/positions of a light (heap array of `mCount` vectors).
struct LightOutputArray { be<s32> mCount; gptr<ParameterVector3> mValues; };
struct AmbientLight : Light {
    EnvironmentColorParameter mColor; EnvironmentScalarParameter mIntensity;
    u32 isType(const EnvironmentTypeInfo*) const; static s32 kindIndex(); static AmbientLight* create(void* heap);
    static AmbientLight* construct(AmbientLight* light);
};
struct HemisphereLight : Light {
    EnvironmentColorParameter mSkyColor,mGroundColor; EnvironmentScalarParameter mIntensity;
    EnvironmentVectorParameter mDirection; LightOutputArray mOutputs;
    u32 isType(const EnvironmentTypeInfo*) const; static s32 kindIndex(); static HemisphereLight* create(void* heap);
    static HemisphereLight* construct(HemisphereLight* light);
    void allocateOutputs(s32 count,void* heap); void transform(const sead::RenderMatrix34* matrix,u32 pass,u32 index);
};
struct DirectionalLight : Light {
    EnvironmentColorParameter mDiffuseColor,mSpecularColor,mBacksideColor;
    EnvironmentScalarParameter mIntensity; EnvironmentVectorParameter mDirection;
    EnvironmentBooleanParameter mViewCoordinates; LightOutputArray mOutputs;
    sead::RenderMatrix34 mMatrix; ParameterVector3 mWorldDirection; be<f32> mYaw,mPitch;
    void refreshAngles();
    u32 isType(const EnvironmentTypeInfo*) const; static s32 kindIndex(); static DirectionalLight* create(void* heap);
    static DirectionalLight* construct(DirectionalLight* light);
    void allocateOutputs(s32 count,void* heap); void transform(const sead::RenderMatrix34* matrix,u32 pass,u32 index);
};
struct PointLight : Light {
    EnvironmentColorParameter mDiffuseColor,mSpecularColor; EnvironmentVectorParameter mPosition;
    EnvironmentScalarParameter mRadius,mIntensity,mDamping; LightOutputArray mOutputs;
    u32 isType(const EnvironmentTypeInfo*) const; static s32 kindIndex(); static PointLight* create(void* heap);
    static PointLight* construct(PointLight* light);
    void allocateOutputs(s32 count,void* heap); void transform(const sead::RenderMatrix34* matrix,u32 pass,u32 index);
};
struct ConeLight : Light {
    EnvironmentColorParameter mDiffuseColor,mSpecularColor; EnvironmentScalarParameter mIntensity;
    EnvironmentScalarParameter mLength; EnvironmentVectorParameter mDirection,mPosition;
    EnvironmentScalarParameter mAngle,mDistanceDamping,mAngleDamping;
    LightOutputArray mDirectionOutputs,mPositionOutputs;
    u32 isType(const EnvironmentTypeInfo*) const; static s32 kindIndex(); static ConeLight* create(void* heap);
    static ConeLight* construct(ConeLight* light);
    void allocateOutputs(s32 count,void* heap); void transform(const sead::RenderMatrix34* matrix,u32 pass,u32 index);
};
}}
WWHD_SIZE(agl::env::LightOutputArray,8);
WWHD_SIZE(agl::env::Light,0xE8);
WWHD_SIZE(agl::env::AmbientLight,0x114);
WWHD_OFFSET(agl::env::HemisphereLight,mOutputs,0x148);
WWHD_SIZE(agl::env::HemisphereLight,0x150);
WWHD_OFFSET(agl::env::DirectionalLight,mDirection,0x14C);
WWHD_OFFSET(agl::env::DirectionalLight,mViewCoordinates,0x164);
WWHD_OFFSET(agl::env::DirectionalLight,mOutputs,0x174);
WWHD_OFFSET(agl::env::DirectionalLight,mYaw,0x1B8);
WWHD_SIZE(agl::env::DirectionalLight,0x1C0);
WWHD_OFFSET(agl::env::PointLight,mPosition,0x120);
WWHD_OFFSET(agl::env::PointLight,mOutputs,0x168);
WWHD_SIZE(agl::env::PointLight,0x170);
WWHD_OFFSET(agl::env::ConeLight,mDirection,0x140);
WWHD_OFFSET(agl::env::ConeLight,mPosition,0x158);
WWHD_OFFSET(agl::env::ConeLight,mDirectionOutputs,0x1A0);
WWHD_OFFSET(agl::env::ConeLight,mPositionOutputs,0x1A8);
WWHD_SIZE(agl::env::ConeLight,0x1B0);
