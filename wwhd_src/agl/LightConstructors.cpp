// Light parameter construction, with original descriptor slots and word-copy defaults.
#include "agl/EnvironmentLights.h"
#include "agl/ShaderCommandState.h"
using namespace agl::env;
using gabi::ea;
// Map the member EA back through guest memory so wrapped embedded addresses keep original semantics.
template <typename Field>
static Field* lightEmbeddedField(Field* field) {
    return gabi::at<Field>(ea(field));
}
void* allocateLightRecord(u32 size);
void initializeLightEnvironment(EnvironmentObject* light);
void initializeLightParameter(agl::env::ParameterBinding* parameter);
void bindLightParameter(agl::env::ParameterBinding* parameter,const sead::SafeText* name,const sead::SafeText* description,
                        const sead::SafeText* hint,EnvironmentObject* owner);
static agl::env::ParameterColor sDefaultLightColor;
static EnvironmentObjectMethods sAmbientMethods;
static sead::VtableSlot sAmbientIoMethods;
static sead::SafeTextVtable sParameterNameTable;
static sead::VtableSlot sColorParameterTable;
static sead::VtableSlot sScalarParameterTable;
static u8 sBlankParameterHint;
static u8 sAmbientColorName;
static u8 sAmbientColorDescription;
static u8 sAmbientIntensityName;
static u8 sAmbientIntensityDescription;
static u8 sAmbientIntensityHint;
/* 02770D10 */
AmbientLight* AmbientLight::construct(AmbientLight* light) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(light==nullptr) light=static_cast<AmbientLight*>(allocateLightRecord(sizeof(AmbientLight)));
    if(light!=nullptr) {
        initializeLightEnvironment(light);
        light->mMethods=&sAmbientMethods;
        light->mParameterIoMethods=&sAmbientIoMethods;
        gabi::FrameLocal<sead::SafeText> colorName(8),colorDescription(0x10);
        colorDescription->mVtable=&sParameterNameTable;
        colorName->mVtable=&sParameterNameTable;
        colorName->mText=&sAmbientColorName;
        colorDescription->mText=&sAmbientColorDescription;
        auto* color=lightEmbeddedField(&light->mColor);
        if(color==nullptr) color=static_cast<agl::env::EnvironmentColorParameter*>(allocateLightRecord(sizeof(agl::env::EnvironmentColorParameter)));
        if(color!=nullptr) {
            initializeLightParameter(&color->mBinding);
            color->mBinding.mMethods=&sColorParameterTable;
            color->mValue.g=0.0f;
            color->mValue.r=0.0f;
            color->mValue.b=0.0f;
            color->mValue.a=1.0f;
            gabi::FrameLocal<sead::SafeText> colorHint(0x30);
            colorHint->mVtable=&sParameterNameTable;
            colorHint->mText=&sBlankParameterHint;
            bindLightParameter(&color->mBinding,colorName.get(),colorDescription.get(),colorHint.get(),light);
            color->mValue=sDefaultLightColor;
        }
        gabi::FrameLocal<sead::SafeText> intensityName(0x18),intensityDescription(0x20),intensityHint(0x28);
        intensityName->mVtable=&sParameterNameTable;
        intensityHint->mVtable=&sParameterNameTable;
        intensityName->mText=&sAmbientIntensityName;
        intensityDescription->mVtable=&sParameterNameTable;
        intensityHint->mText=&sAmbientIntensityHint;
        intensityDescription->mText=&sAmbientIntensityDescription;
        auto* intensity=lightEmbeddedField(&light->mIntensity);
        if(intensity==nullptr) intensity=static_cast<agl::env::EnvironmentScalarParameter*>(allocateLightRecord(sizeof(agl::env::EnvironmentScalarParameter)));
        if(intensity!=nullptr) {
            initializeLightParameter(&intensity->mBinding);
            intensity->mBinding.mMethods=&sScalarParameterTable;
            bindLightParameter(&intensity->mBinding,intensityName.get(),intensityDescription.get(),intensityHint.get(),light);
            intensity->mValue=1.0f;
        }
    }
    return light;
}

static EnvironmentObjectMethods sHemisphereMethods;
static sead::VtableSlot sHemisphereIoMethods;
static sead::VtableSlot sVectorParameterTable;
static sead::VtableSlot sDirectionParameterTable;
static agl::env::ParameterVector3 sDefaultLightDirection;
static u8 sSkyColorName;
static u8 sSkyColorDescription;
static u8 sGroundColorName;
static u8 sGroundColorDescription;
static u8 sHemisphereIntensityName;
static u8 sHemisphereIntensityDescription;
static u8 sHemisphereIntensityHint;
static u8 sHemisphereDirectionName;
static u8 sHemisphereDirectionDescription;
static u8 sVectorParameterHint;
static agl::env::EnvironmentColorParameter* initializeColorParameter(agl::env::EnvironmentColorParameter* color,sead::SafeText* name,
    sead::SafeText* description,sead::SafeText* hint,EnvironmentObject* owner,const agl::env::ParameterColor* defaults) {
    if(color==nullptr) color=static_cast<agl::env::EnvironmentColorParameter*>(allocateLightRecord(sizeof(agl::env::EnvironmentColorParameter)));
    if(color!=nullptr) {
        initializeLightParameter(&color->mBinding);
        color->mBinding.mMethods=&sColorParameterTable;
        color->mValue.r=0.0f;color->mValue.g=0.0f;color->mValue.b=0.0f;color->mValue.a=1.0f;
        hint->mText=&sBlankParameterHint;hint->mVtable=&sParameterNameTable;
        bindLightParameter(&color->mBinding,name,description,hint,owner);
        color->mValue=*defaults;
    }
    return color;
}
/* 02770EC0 */
HemisphereLight* HemisphereLight::construct(HemisphereLight* light) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(light==nullptr) light=static_cast<HemisphereLight*>(allocateLightRecord(sizeof(HemisphereLight)));
    if(light!=nullptr) {
        initializeLightEnvironment(light);
        light->mParameterIoMethods=&sHemisphereIoMethods;light->mMethods=&sHemisphereMethods;
        gabi::FrameLocal<agl::env::ParameterColor> sky(8);
        sky->g=0.84f;sky->r=0.84f;sky->a=1.0f;sky->b=0.96f;
        gabi::FrameLocal<sead::SafeText> skyName(0x28),skyDescription(0x30),skyHint(0x70);
        skyName->mVtable=&sParameterNameTable;skyName->mText=&sSkyColorName;
        skyDescription->mVtable=&sParameterNameTable;skyDescription->mText=&sSkyColorDescription;
        initializeColorParameter(lightEmbeddedField(&light->mSkyColor),skyName.get(),
            skyDescription.get(),skyHint.get(),light,sky.get());
        gabi::FrameLocal<agl::env::ParameterColor> ground(0x18);
        ground->a=1.0f;ground->r=0.47f;ground->b=0.25f;ground->g=0.35f;
        gabi::FrameLocal<sead::SafeText> groundName(0x38),groundDescription(0x40),groundHint(0x78);
        groundName->mVtable=&sParameterNameTable;groundDescription->mVtable=&sParameterNameTable;
        groundDescription->mText=&sGroundColorDescription;groundName->mText=&sGroundColorName;
        initializeColorParameter(lightEmbeddedField(&light->mGroundColor),groundName.get(),
            groundDescription.get(),groundHint.get(),light,ground.get());
        gabi::FrameLocal<sead::SafeText> intensityName(0x48),intensityDescription(0x50),intensityHint(0x58);
        intensityName->mVtable=&sParameterNameTable;intensityDescription->mVtable=&sParameterNameTable;
        intensityHint->mVtable=&sParameterNameTable;intensityName->mText=&sHemisphereIntensityName;
        intensityDescription->mText=&sHemisphereIntensityDescription;intensityHint->mText=&sHemisphereIntensityHint;
        auto* intensity=lightEmbeddedField(&light->mIntensity);
        if(intensity==nullptr) intensity=static_cast<agl::env::EnvironmentScalarParameter*>(allocateLightRecord(sizeof(agl::env::EnvironmentScalarParameter)));
        if(intensity!=nullptr) {
            initializeLightParameter(&intensity->mBinding);intensity->mBinding.mMethods=&sScalarParameterTable;
            bindLightParameter(&intensity->mBinding,intensityName.get(),intensityDescription.get(),intensityHint.get(),light);
            intensity->mValue=1.0f;
        }
        gabi::FrameLocal<sead::SafeText> directionName(0x60),directionDescription(0x68),directionHint(0x80);
        directionName->mVtable=&sParameterNameTable;directionDescription->mVtable=&sParameterNameTable;
        directionName->mText=&sHemisphereDirectionName;directionDescription->mText=&sHemisphereDirectionDescription;
        auto* direction=lightEmbeddedField(&light->mDirection);
        if(direction==nullptr) direction=static_cast<agl::env::EnvironmentVectorParameter*>(allocateLightRecord(sizeof(agl::env::EnvironmentVectorParameter)));
        if(direction!=nullptr) {
            initializeLightParameter(&direction->mBinding);direction->mBinding.mMethods=&sVectorParameterTable;
            directionHint->mVtable=&sParameterNameTable;directionHint->mText=&sVectorParameterHint;
            bindLightParameter(&direction->mBinding,directionName.get(),directionDescription.get(),directionHint.get(),light);
            direction->mValue.x=(f32)sDefaultLightDirection.x;
            direction->mValue.y=(f32)sDefaultLightDirection.y;
            direction->mValue.z=(f32)sDefaultLightDirection.z;
        }
        light->mDirection.mBinding.mMethods=&sDirectionParameterTable;
        auto* outputs=lightEmbeddedField(&light->mOutputs);
        if(outputs==nullptr) outputs=static_cast<LightOutputArray*>(allocateLightRecord(sizeof(LightOutputArray)));
        if(outputs!=nullptr) { outputs->mValues=nullptr;outputs->mCount=0; }
    }
    return light;
}
static EnvironmentObjectMethods sDirectionalMethods;
static sead::VtableSlot sDirectionalIoMethods;
static agl::env::ParameterColor sDefaultDirectColor;
static agl::env::ParameterVector3 sDefaultForward;
static sead::VtableSlot sBooleanParameterTable;
static u8 sBooleanHint;
static u8 sDirectDiffuseName;
static u8 sDirectDiffuseDescription;
static u8 sDirectSpecularName;
static u8 sDirectSpecularDescription;
static u8 sDirectBacksideName;
static u8 sDirectBacksideDescription;
static u8 sDirectIntensityName;
static u8 sDirectIntensityDescription;
static u8 sDirectIntensityHint;
static u8 sDirectDirectionName;
static u8 sDirectDirectionDescription;
static u8 sDirectCoordinatesName;
static u8 sDirectCoordinatesDescription;
void normalizeInitialLightDirection(agl::env::ParameterVector3*);
static void setLightName(sead::SafeText* name,const be<u8>* text) {
    name->mVtable=&sParameterNameTable;name->mText=const_cast<be<u8>*>(text);
}
/* 027713EC */
DirectionalLight* DirectionalLight::construct(DirectionalLight* light) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(light==nullptr) light=static_cast<DirectionalLight*>(allocateLightRecord(sizeof(DirectionalLight)));
    if(light!=nullptr) {
        initializeLightEnvironment(light);light->mMethods=&sDirectionalMethods;
        light->mParameterIoMethods=&sDirectionalIoMethods;
        gabi::FrameLocal<sead::SafeText> diffuseName(0x14),diffuseDescription(0x1C),diffuseHint(0x7C);
        setLightName(diffuseName.get(),&sDirectDiffuseName);setLightName(diffuseDescription.get(),&sDirectDiffuseDescription);
        initializeColorParameter(lightEmbeddedField(&light->mDiffuseColor),diffuseName.get(),
            diffuseDescription.get(),diffuseHint.get(),light,&sDefaultDirectColor);
        gabi::FrameLocal<sead::SafeText> specularName(0x24),specularDescription(0x2C),specularHint(0x84);
        setLightName(specularName.get(),&sDirectSpecularName);setLightName(specularDescription.get(),&sDirectSpecularDescription);
        initializeColorParameter(lightEmbeddedField(&light->mSpecularColor),specularName.get(),
            specularDescription.get(),specularHint.get(),light,&sDefaultDirectColor);
        gabi::FrameLocal<sead::SafeText> backsideName(0x34),backsideDescription(0x3C),backsideHint(0x8C);
        setLightName(backsideName.get(),&sDirectBacksideName);setLightName(backsideDescription.get(),&sDirectBacksideDescription);
        initializeColorParameter(lightEmbeddedField(&light->mBacksideColor),backsideName.get(),
            backsideDescription.get(),backsideHint.get(),light,&sDefaultLightColor);
        gabi::FrameLocal<sead::SafeText> intensityName(0x44),intensityDescription(0x4C),intensityHint(0x54);
        setLightName(intensityName.get(),&sDirectIntensityName);setLightName(intensityDescription.get(),&sDirectIntensityDescription);
        setLightName(intensityHint.get(),&sDirectIntensityHint);
        auto* intensity=lightEmbeddedField(&light->mIntensity);
        if(intensity==nullptr) intensity=static_cast<agl::env::EnvironmentScalarParameter*>(allocateLightRecord(sizeof(agl::env::EnvironmentScalarParameter)));
        if(intensity!=nullptr) {
            initializeLightParameter(&intensity->mBinding);intensity->mBinding.mMethods=&sScalarParameterTable;
            bindLightParameter(&intensity->mBinding,intensityName.get(),intensityDescription.get(),intensityHint.get(),light);
            intensity->mValue=1.0f;
        }
        gabi::FrameLocal<sead::SafeText> directionName(0x5C),directionDescription(0x64),directionHint(0x94);
        setLightName(directionName.get(),&sDirectDirectionName);setLightName(directionDescription.get(),&sDirectDirectionDescription);
        gabi::FrameLocal<agl::env::ParameterVector3> negativeForward(8),directionDefault(0xA4);
        negativeForward->z=-(f32)sDefaultForward.z;
        negativeForward->x=-(f32)sDefaultForward.x;
        negativeForward->y=-(f32)sDefaultForward.y;
        directionDefault->x=negativeForward->x;
        directionDefault->y=negativeForward->y;
        directionDefault->z=negativeForward->z;
        auto* direction=lightEmbeddedField(&light->mDirection);
        if(direction==nullptr) direction=static_cast<agl::env::EnvironmentVectorParameter*>(allocateLightRecord(sizeof(agl::env::EnvironmentVectorParameter)));
        if(direction!=nullptr) {
            initializeLightParameter(&direction->mBinding);direction->mBinding.mMethods=&sVectorParameterTable;
            setLightName(directionHint.get(),&sVectorParameterHint);
            bindLightParameter(&direction->mBinding,directionName.get(),directionDescription.get(),directionHint.get(),light);
            direction->mValue.x=(f32)directionDefault->x;direction->mValue.y=(f32)directionDefault->y;
            direction->mValue.z=(f32)directionDefault->z;
        }
        light->mDirection.mBinding.mMethods=&sDirectionParameterTable;
        gabi::FrameLocal<sead::SafeText> coordinatesName(0x6C),coordinatesDescription(0x74),coordinatesHint(0x9C);
        setLightName(coordinatesName.get(),&sDirectCoordinatesName);setLightName(coordinatesDescription.get(),&sDirectCoordinatesDescription);
        auto* coordinates=lightEmbeddedField(&light->mViewCoordinates);
        if(coordinates==nullptr) coordinates=static_cast<agl::env::EnvironmentBooleanParameter*>(allocateLightRecord(sizeof(agl::env::EnvironmentBooleanParameter)));
        if(coordinates!=nullptr) {
            initializeLightParameter(&coordinates->mBinding);coordinates->mBinding.mMethods=&sBooleanParameterTable;
            setLightName(coordinatesHint.get(),&sBooleanHint);
            bindLightParameter(&coordinates->mBinding,coordinatesName.get(),coordinatesDescription.get(),coordinatesHint.get(),light);
            coordinates->mValue=0;
        }
        auto* outputs=lightEmbeddedField(&light->mOutputs);
        if(outputs==nullptr) outputs=static_cast<LightOutputArray*>(allocateLightRecord(sizeof(LightOutputArray)));
        if(outputs!=nullptr) { outputs->mValues=nullptr;outputs->mCount=0; }
        if(lightEmbeddedField(&light->mMatrix)==nullptr) allocateLightRecord(sizeof(sead::RenderMatrix34));
        if(lightEmbeddedField(&light->mWorldDirection)==nullptr) allocateLightRecord(sizeof(agl::env::ParameterVector3));
        if(lightEmbeddedField(&light->mYaw)==nullptr) allocateLightRecord(8);
        normalizeInitialLightDirection(&light->mDirection.mValue);
        light->refreshAngles();
    }
    return light;
}
static EnvironmentObjectMethods sPointMethods;
static sead::VtableSlot sPointIoMethods;
static agl::env::ParameterVector3 sDefaultLightPosition;
static u8 sScalarHint;
static void initializeScalarParameter(agl::env::EnvironmentScalarParameter* parameter,sead::SafeText* name,
    sead::SafeText* description,sead::SafeText* hint,EnvironmentObject* owner,f32 value) {
    if(parameter==nullptr) parameter=static_cast<agl::env::EnvironmentScalarParameter*>(allocateLightRecord(sizeof(agl::env::EnvironmentScalarParameter)));
    if(parameter!=nullptr) {
        initializeLightParameter(&parameter->mBinding);parameter->mBinding.mMethods=&sScalarParameterTable;
        bindLightParameter(&parameter->mBinding,name,description,hint,owner);parameter->mValue=value;
    }
}
static u8 sPointDiffuseName;
static u8 sPointDiffuseDescription;
static u8 sPointSpecularName;
static u8 sPointSpecularDescription;
static u8 sPointPositionName;
static u8 sPointPositionDescription;
static u8 sPointRadiusName;
static u8 sPointRadiusDescription;
static u8 sPointIntensityName;
static u8 sPointIntensityDescription;
static u8 sPointIntensityHint;
static u8 sPointDampingName;
static u8 sPointDampingDescription;
/* 027719D0 */
PointLight* PointLight::construct(PointLight* light) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(light==nullptr) light=static_cast<PointLight*>(allocateLightRecord(sizeof(PointLight)));
    if(light!=nullptr) {
        initializeLightEnvironment(light);light->mMethods=&sPointMethods;light->mParameterIoMethods=&sPointIoMethods;
        gabi::FrameLocal<sead::SafeText> diffuseName(8),diffuseDescription(16),diffuseHint(112);
        setLightName(diffuseName.get(),&sPointDiffuseName);setLightName(diffuseDescription.get(),&sPointDiffuseDescription);
        initializeColorParameter(lightEmbeddedField(&light->mDiffuseColor),diffuseName.get(),
            diffuseDescription.get(),diffuseHint.get(),light,&sDefaultDirectColor);
        gabi::FrameLocal<sead::SafeText> specularName(24),specularDescription(32),specularHint(120);
        setLightName(specularName.get(),&sPointSpecularName);setLightName(specularDescription.get(),&sPointSpecularDescription);
        initializeColorParameter(lightEmbeddedField(&light->mSpecularColor),specularName.get(),
            specularDescription.get(),specularHint.get(),light,&sDefaultDirectColor);
        gabi::FrameLocal<sead::SafeText> positionName(0x28),positionDescription(0x30),positionHint(0x80);
        setLightName(positionName.get(),&sPointPositionName);setLightName(positionDescription.get(),&sPointPositionDescription);
        auto* position=lightEmbeddedField(&light->mPosition);
        if(position==nullptr) position=static_cast<agl::env::EnvironmentVectorParameter*>(allocateLightRecord(sizeof(agl::env::EnvironmentVectorParameter)));
        if(position!=nullptr) {
            initializeLightParameter(&position->mBinding);position->mBinding.mMethods=&sVectorParameterTable;
            setLightName(positionHint.get(),&sVectorParameterHint);
            bindLightParameter(&position->mBinding,positionName.get(),positionDescription.get(),positionHint.get(),light);
            position->mValue.x=(f32)sDefaultLightPosition.x;position->mValue.y=(f32)sDefaultLightPosition.y;
            position->mValue.z=(f32)sDefaultLightPosition.z;
        }
        gabi::FrameLocal<sead::SafeText> radiusName(56),radiusDescription(64),radiusHint(136);
        setLightName(radiusName.get(),&sPointRadiusName);setLightName(radiusDescription.get(),&sPointRadiusDescription);
        setLightName(radiusHint.get(),&sScalarHint);
        initializeScalarParameter(lightEmbeddedField(&light->mRadius),radiusName.get(),
            radiusDescription.get(),radiusHint.get(),light,1.0f);
        gabi::FrameLocal<sead::SafeText> intensityName(72),intensityDescription(80),intensityHint(88);
        setLightName(intensityName.get(),&sPointIntensityName);setLightName(intensityDescription.get(),&sPointIntensityDescription);
        setLightName(intensityHint.get(),&sPointIntensityHint);
        initializeScalarParameter(lightEmbeddedField(&light->mIntensity),intensityName.get(),
            intensityDescription.get(),intensityHint.get(),light,1.0f);
        gabi::FrameLocal<sead::SafeText> dampingName(96),dampingDescription(104),dampingHint(144);
        setLightName(dampingName.get(),&sPointDampingName);setLightName(dampingDescription.get(),&sPointDampingDescription);
        setLightName(dampingHint.get(),&sScalarHint);
        initializeScalarParameter(lightEmbeddedField(&light->mDamping),dampingName.get(),
            dampingDescription.get(),dampingHint.get(),light,1.0f);
        auto* outputs=lightEmbeddedField(&light->mOutputs);
        if(outputs==nullptr) outputs=static_cast<LightOutputArray*>(allocateLightRecord(sizeof(LightOutputArray)));
        if(outputs!=nullptr) { outputs->mValues=nullptr;outputs->mCount=0; }
    }
    return light;
}
static EnvironmentObjectMethods sConeMethods;
static sead::VtableSlot sConeIoMethods;
static u8 sConeDiffuseName;
static u8 sConeDiffuseDescription;
static u8 sConeSpecularName;
static u8 sConeSpecularDescription;
static u8 sConeIntensityName;
static u8 sConeIntensityDescription;
static u8 sConeIntensityHint;
// Binary parameter keys at10161090/10161150 are Length/DistDamp.
static u8 sConeLengthName;
static u8 sConeLengthDescription;
static u8 sConeDirectionName;
static u8 sConeDirectionDescription;
static u8 sConePositionName;
static u8 sConePositionDescription;
static u8 sConeAngleName;
static u8 sConeAngleDescription;
static u8 sConeDistanceDampingName;
static u8 sConeDistanceDampingDescription;
static u8 sConeAngleDampingName;
static u8 sConeAngleDampingDescription;
/* 02771E70 */
ConeLight* ConeLight::construct(ConeLight* light) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(light==nullptr) light=static_cast<ConeLight*>(allocateLightRecord(sizeof(ConeLight)));
    if(light!=nullptr) {
        initializeLightEnvironment(light);light->mMethods=&sConeMethods;light->mParameterIoMethods=&sConeIoMethods;
        gabi::FrameLocal<sead::SafeText> diffuseName(20),diffuseDescription(28),diffuseHint(172);
        setLightName(diffuseName.get(),&sConeDiffuseName);setLightName(diffuseDescription.get(),&sConeDiffuseDescription);
        initializeColorParameter(lightEmbeddedField(&light->mDiffuseColor),diffuseName.get(),
            diffuseDescription.get(),diffuseHint.get(),light,&sDefaultDirectColor);
        gabi::FrameLocal<sead::SafeText> specularName(36),specularDescription(44),specularHint(180);
        setLightName(specularName.get(),&sConeSpecularName);setLightName(specularDescription.get(),&sConeSpecularDescription);
        initializeColorParameter(lightEmbeddedField(&light->mSpecularColor),specularName.get(),
            specularDescription.get(),specularHint.get(),light,&sDefaultDirectColor);
        gabi::FrameLocal<sead::SafeText> intensityName(52),intensityDescription(60),intensityHint(68);
        setLightName(intensityName.get(),&sConeIntensityName);setLightName(intensityDescription.get(),&sConeIntensityDescription);
        setLightName(intensityHint.get(),&sConeIntensityHint);
        initializeScalarParameter(lightEmbeddedField(&light->mIntensity),intensityName.get(),
            intensityDescription.get(),intensityHint.get(),light,1.0f);
        gabi::FrameLocal<sead::SafeText> lengthName(76),lengthDescription(84),lengthHint(188);
        setLightName(lengthName.get(),&sConeLengthName);setLightName(lengthDescription.get(),&sConeLengthDescription);
        setLightName(lengthHint.get(),&sScalarHint);
        initializeScalarParameter(lightEmbeddedField(&light->mLength),lengthName.get(),
            lengthDescription.get(),lengthHint.get(),light,1.0f);
        gabi::FrameLocal<sead::SafeText> directionName(92),directionDescription(100),directionHint(196);
        setLightName(directionName.get(),&sConeDirectionName);setLightName(directionDescription.get(),&sConeDirectionDescription);
        gabi::FrameLocal<agl::env::ParameterVector3> negativeForward(8),directionDefault(0xEC);
        negativeForward->x=-(f32)sDefaultForward.x;negativeForward->z=-(f32)sDefaultForward.z;
        negativeForward->y=-(f32)sDefaultForward.y;
        directionDefault->x=negativeForward->x;
        directionDefault->y=negativeForward->y;
        directionDefault->z=negativeForward->z;
        auto* direction=lightEmbeddedField(&light->mDirection);
        if(direction==nullptr) direction=static_cast<agl::env::EnvironmentVectorParameter*>(allocateLightRecord(sizeof(agl::env::EnvironmentVectorParameter)));
        if(direction!=nullptr) {
            initializeLightParameter(&direction->mBinding);direction->mBinding.mMethods=&sVectorParameterTable;
            setLightName(directionHint.get(),&sVectorParameterHint);
            bindLightParameter(&direction->mBinding,directionName.get(),directionDescription.get(),directionHint.get(),light);
            direction->mValue.x=(f32)directionDefault->x;direction->mValue.y=(f32)directionDefault->y;
            direction->mValue.z=(f32)directionDefault->z;
        }
        light->mDirection.mBinding.mMethods=&sDirectionParameterTable;
        gabi::FrameLocal<sead::SafeText> positionName(108),positionDescription(116),positionHint(204);
        setLightName(positionName.get(),&sConePositionName);setLightName(positionDescription.get(),&sConePositionDescription);
        auto* position=lightEmbeddedField(&light->mPosition);
        if(position==nullptr) position=static_cast<agl::env::EnvironmentVectorParameter*>(allocateLightRecord(sizeof(agl::env::EnvironmentVectorParameter)));
        if(position!=nullptr) {
            initializeLightParameter(&position->mBinding);position->mBinding.mMethods=&sVectorParameterTable;
            setLightName(positionHint.get(),&sVectorParameterHint);
            bindLightParameter(&position->mBinding,positionName.get(),positionDescription.get(),positionHint.get(),light);
            position->mValue.x=(f32)sDefaultLightPosition.x;position->mValue.y=(f32)sDefaultLightPosition.y;
            position->mValue.z=(f32)sDefaultLightPosition.z;
        }
        gabi::FrameLocal<sead::SafeText> angleName(124),angleDescription(132),angleHint(212);
        setLightName(angleName.get(),&sConeAngleName);setLightName(angleDescription.get(),&sConeAngleDescription);
        setLightName(angleHint.get(),&sScalarHint);
        initializeScalarParameter(lightEmbeddedField(&light->mAngle),angleName.get(),
            angleDescription.get(),angleHint.get(),light,0.5f);
        gabi::FrameLocal<sead::SafeText> distanceDampingName(140),distanceDampingDescription(148),distanceDampingHint(220);
        setLightName(distanceDampingName.get(),&sConeDistanceDampingName);setLightName(distanceDampingDescription.get(),&sConeDistanceDampingDescription);
        setLightName(distanceDampingHint.get(),&sScalarHint);
        initializeScalarParameter(lightEmbeddedField(&light->mDistanceDamping),distanceDampingName.get(),
            distanceDampingDescription.get(),distanceDampingHint.get(),light,1.0f);
        gabi::FrameLocal<sead::SafeText> angleDampingName(156),angleDampingDescription(164),angleDampingHint(228);
        setLightName(angleDampingName.get(),&sConeAngleDampingName);setLightName(angleDampingDescription.get(),&sConeAngleDampingDescription);
        setLightName(angleDampingHint.get(),&sScalarHint);
        initializeScalarParameter(lightEmbeddedField(&light->mAngleDamping),angleDampingName.get(),
            angleDampingDescription.get(),angleDampingHint.get(),light,1.0f);
        auto* outputs416=lightEmbeddedField(&light->mDirectionOutputs);
        if(outputs416==nullptr) outputs416=static_cast<LightOutputArray*>(allocateLightRecord(sizeof(LightOutputArray)));
        if(outputs416!=nullptr) { outputs416->mValues=nullptr;outputs416->mCount=0; }
        auto* outputs424=lightEmbeddedField(&light->mPositionOutputs);
        if(outputs424==nullptr) outputs424=static_cast<LightOutputArray*>(allocateLightRecord(sizeof(LightOutputArray)));
        if(outputs424!=nullptr) { outputs424->mValues=nullptr;outputs424->mCount=0; }
        normalizeInitialLightDirection(&light->mDirection.mValue);
    }
    return light;
}
/* WWHD glue
data sConeMethods 101615F0
data sConeIoMethods 101612F8
data sConeDiffuseName 101610F0
data sConeDiffuseDescription 10161100
data sConeSpecularName 101610BC
data sConeSpecularDescription 10161114
data sConeIntensityName 101610CC
data sConeIntensityDescription 10161128
data sConeIntensityHint 101610B0
data sConeLengthName 10161090
data sConeLengthDescription 10161098
data sConeDirectionName 101610D8
data sConeDirectionDescription 101610A0
data sConePositionName 10161138
data sConePositionDescription 101610A8
data sConeAngleName 10161088
data sConeAngleDescription 10161144
data sConeDistanceDampingName 10161150
data sConeDistanceDampingDescription 1016115C
data sConeAngleDampingName 101610E4
data sConeAngleDampingDescription 10161168

data sPointMethods 10161540
data sPointIoMethods 101612C0
data sDefaultLightPosition 104A0928
data sScalarHint 10160BA5
data sPointDiffuseName 10161034
data sPointDiffuseDescription 10161044
data sPointSpecularName 1016100C
data sPointSpecularDescription 10161058
data sPointPositionName 1016106C
data sPointPositionDescription 10160FE0
data sPointRadiusName 10160FE8
data sPointRadiusDescription 10160FF0
data sPointIntensityName 1016101C
data sPointIntensityDescription 10161078
data sPointIntensityHint 10161000
data sPointDampingName 10161028
data sPointDampingDescription 10160FF8

ext 025155D8 void normalizeInitialLightDirection(agl::env::ParameterVector3*)
data sDirectionalMethods 10161490
data sDirectionalIoMethods 10161288
data sDefaultDirectColor 104A01EC
data sDefaultForward 104A0958
data sBooleanParameterTable 10160BE0
data sBooleanHint 10160BA4
data sDirectDiffuseName 10160F58
data sDirectDiffuseDescription 10160F68
data sDirectSpecularName 10160F20
data sDirectSpecularDescription 10160F7C
data sDirectBacksideName 10160F30
data sDirectBacksideDescription 10160F90
data sDirectIntensityName 10160F40
data sDirectIntensityDescription 10160F9C
data sDirectIntensityHint 10160FAC
data sDirectDirectionName 10160F4C
data sDirectDirectionDescription 10160F18
data sDirectCoordinatesName 10160FBC
data sDirectCoordinatesDescription 10160FCC
ext 0273AD10 void* allocateLightRecord(u32)
ext 0276FCA0 void initializeLightEnvironment(agl::env::EnvironmentObject*)
ext 0279E674 void initializeLightParameter(agl::env::ParameterBinding*)
ext 0279E618 void bindLightParameter(agl::env::ParameterBinding*,const sead::SafeText*,const sead::SafeText*,const sead::SafeText*,agl::env::EnvironmentObject*)
data sDefaultLightColor 104A01CC
data sAmbientMethods 10161330
data sAmbientIoMethods 10161218
data sParameterNameTable 10160BA8
data sColorParameterTable 10160D48
data sScalarParameterTable 10160C58
data sBlankParameterHint 10160BA7
data sAmbientColorName 10160E44
data sAmbientColorDescription 10160E64
data sAmbientIntensityName 10160E58
data sAmbientIntensityDescription 10160E78
data sAmbientIntensityHint 10160E4C

data sHemisphereMethods 101613E0
data sHemisphereIoMethods 10161250
data sVectorParameterTable 10160CD0
data sDirectionParameterTable 10160DC0
data sDefaultLightDirection 104A0940
data sSkyColorName 10160ED4
data sSkyColorDescription 10160EE0
data sGroundColorName 10160EA4
data sGroundColorDescription 10160EF0
data sHemisphereIntensityName 10160EBC
data sHemisphereIntensityDescription 10160F04
data sHemisphereIntensityHint 10160EB0
data sHemisphereDirectionName 10160EC8
data sHemisphereDirectionDescription 10160E9C
data sVectorParameterHint 10160BA6
*/
