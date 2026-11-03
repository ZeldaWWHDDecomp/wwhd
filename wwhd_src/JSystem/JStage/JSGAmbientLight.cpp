// JStage defaults reconstructed from Cafe, using the GameCube interface names.
#include "JSystem/JStage/JSGAmbientLight.h"
void JStage_delete(void*);
void JStage_register(void*);

/* 02842414 */
s32 JStage::TAmbientLight::JSGFGetType() const {
    return 4;
}

/* 0284241C */
void JStage::TAmbientLight::destroy(s32 flags) {
    if (!this) return;
    TObject::destroy(0);
    if (flags & 1) JStage_delete(this);
}

static u32 sJStageColorInitialized;
static u32 sJStageDefaultColor;
static const u32 sWhiteColor;
void JStage_copyColor(void*,const void*,u32);

/* 02842384 */
u32 JStage::TAmbientLight::JSGGetColor() const {
    // HD: GHS shares one lazily copied color cache between the default getters.
    if (!sJStageColorInitialized) {
        sJStageColorInitialized = 1;
        JStage_copyColor(&sJStageDefaultColor, &sWhiteColor, 4);
    }
    return sJStageDefaultColor;
}



namespace {
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
}

/* 02842470 */
static void __sinit_JSGAmbientLight_cpp() {
    sStaticBlock.words[2]=0;
    sStaticBlock.words[0]=0;
    sStaticBlock.words[3]=0;
    sStaticBlock.words[1]=0;
    JStage_register(&sStaticRegister);
}



/* WWHD glue:
data sJStageColorInitialized 101FE04C
data sJStageDefaultColor 101FF011
data sWhiteColor 10173A14
ext C000A848 void JStage_copyColor(void*,const void*,u32)
data sStaticBlock 104B6454
data sStaticRegister 101FAEF4
*/
