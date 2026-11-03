#include "JSystem/JStage/JSGAmbientLight.h"
// JStage defaults reconstructed from Cafe, using the GameCube interface names.
#include "JSystem/JStage/JSGFog.h"
void JStage_delete(void*);
void JStage_register(void*);

/* 02842584 */
s32 JStage::TFog::JSGFGetType() const {
    return 6;
}

/* 0284258C */
f32 JStage::TFog::JSGGetStartZ() const {
    return 0.0f;
}

/* 02842598 */
f32 JStage::TFog::JSGGetEndZ() const {
    return 3.4028234663852886e38f;
}

/* 028425A4 */
void JStage::TFog::destroy(s32 flags) {
    if (!this) return;
    TObject::destroy(0);
    if (flags & 1) JStage_delete(this);
}

static const u32 sWhiteColor;
void JStage_copyColor(void*,const void*,u32);

/* 028425F8 */
u32 JStage::TFog::JSGGetColor() const {
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

/* 02842660 */
static void __sinit_JSGFog_cpp() {
    sStaticBlock.words[2]=0;
    sStaticBlock.words[0]=0;
    sStaticBlock.words[3]=0;
    sStaticBlock.words[1]=0;
    JStage_register(&sStaticRegister);
}



/* WWHD glue:
data sWhiteColor 10173A3C
data sStaticBlock 104B6474
data sStaticRegister 101FAF0C
*/
