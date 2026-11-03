#include "JSystem/JStage/JSGAmbientLight.h"
// JStage defaults reconstructed from Cafe, using the GameCube interface names.
#include "JSystem/JStage/JSGLight.h"
void JStage_delete(void*);
void JStage_register(void*);

/* 02842688 */
s32 JStage::TLight::JSGFGetType() const {
    return 5;
}

/* 02842288 */
void JStage::TLight::destroy(s32 flags) {
    if (!this) return;
    TObject::destroy(0);
    if (flags & 1) JStage_delete(this);
}

/* 02842690 */
void JStage::TLight::JSGGetDirection(void*) const {

}

/* 02842694 */
void JStage::TLight::JSGSetDirection(const void*) {

}

static const u32 sWhiteColor;
void JStage_copyColor(void*,const void*,u32);

/* 028424F4 */
u32 JStage::TLight::JSGGetColor() const {
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

/* 028426EC */
static void __sinit_JSGLight_cpp() {
    sStaticBlock.words[2]=0;
    sStaticBlock.words[0]=0;
    sStaticBlock.words[3]=0;
    sStaticBlock.words[1]=0;
    JStage_register(&sStaticRegister);
}



/* WWHD glue:
data sWhiteColor 10173A24
data sStaticBlock 104B6484
data sStaticRegister 101FAF18
*/
