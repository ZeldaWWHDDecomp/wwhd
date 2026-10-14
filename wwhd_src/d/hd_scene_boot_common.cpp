/* HD boot-scene descriptor access and TU-local static initialization. */
#include "d/hd_scene_boot_common.h"
static hdSceneHeaderWords_c sBootSupportHeaderWords;
static hdSceneHeaderRange_c sBootSupportHeaderRange;
static hdSceneRegistration_c sBootSupportHeaderRegistration;
static hdSceneRegistration_c sBootSupportSignedRegistration;
static hdSceneRegistration_c sBootSupportUnsignedRegistration;
static hdSceneTypeName_c sBootSupportTypeName;
static u32 sBootSupportTypeVtable;
static u32 sBootSupportTypeString;
static f32 sBootSupportLower;
static f32 sBootSupportUpper;

/* 027079AC */
hdStateId* hdSceneStateHandle_c::descriptor() { return mpDescriptor; }

/* 02707AAC */
void hdBootSupportStaticInit() {
    sBootSupportHeaderWords.mWords[3] = 0;
    sBootSupportHeaderWords.mWords[2] = 0;
    sBootSupportHeaderWords.mWords[1] = 0;
    sBootSupportHeaderWords.mWords[0] = 0;
    hdSceneRegisterGlobal(&sBootSupportHeaderRegistration);
    f32 lower = sBootSupportLower;
    f32 upper = sBootSupportUpper;
    sBootSupportHeaderRange.mLower = lower;
    sBootSupportHeaderRange.mUpper = upper;
    hdSceneConstructSigned(&sBootSupportHeaderRange.mSigned);
    hdSceneRegisterGlobal(&sBootSupportSignedRegistration);
    hdSceneConstructUnsigned(&sBootSupportHeaderRange.mUnsigned);
    hdSceneRegisterGlobal(&sBootSupportUnsignedRegistration);
    sBootSupportTypeName.mpVtable = &sBootSupportTypeVtable;
    sBootSupportTypeName.mpName = &sBootSupportTypeString;
}
/* WWHD glue
data sBootSupportHeaderWords 1049E908
data sBootSupportHeaderRange 1049E8FC
data sBootSupportHeaderRegistration 101F8220
data sBootSupportSignedRegistration 101F822C
data sBootSupportUnsignedRegistration 101F8238
data sBootSupportTypeName 1049E8F4
data sBootSupportTypeVtable 1013E798
data sBootSupportTypeString 1013E7E8
data sBootSupportLower 1013E7E0
data sBootSupportUpper 1013E7E4
ext 028F026C void hdSceneRegisterGlobal(hdSceneRegistration_c*)
ext 028ED6F8 void hdSceneConstructSigned(be<u8>*)
ext 028EAB2C void hdSceneConstructUnsigned(be<u8>*)
*/
