/** HD demo point lights. */
#include "d/d_kankyo_demo.h"

static f32 initialPower = 0.0f;
static f32 pointFluctuation = 255.0f;
static f32 pointBrightness = 500.0f;
static f32 effectFluctuation = 55.0f;
static f32 effectBrightness = 437.0f;
static f32 specialFluctuation = 71.0f;
static f32 specialBrightness = 532.0f;
static f32 brightnessScale = 0.1f;
static f32 brightnessMaxStep = 10.0f;

/* 02563ABC */
void dKydm_demo_plight_entry(void* light, void* pos, s32 type, u32 group, u32 delay) {
    // HD: Every operation first invokes the lazy environment accessor.
    dKy_getEnvlight();
    if (type < 0) return;
    DEMO_PLIGHT* plight = (DEMO_PLIGHT*)light;
    cXyz* position = (cXyz*)pos;
    plight->mLightInfluence.mPos.x= position->x;
    plight->mLightInfluence.mPos.y= position->y;
    u32 combined = (u32)type + group * 100u;
    plight->field_0x04 = (s32)combined;
    plight->mLightInfluence.mPos.z= position->z;
    plight->mLightInfluence.mPower = initialPower;
    if ((s32)combined < 100) {
        plight->mLightInfluence.mColorB = 128;
        plight->mLightInfluence.mColorR = 255;
        plight->mBrightness = pointBrightness;
        plight->mLightInfluence.mColorG = 255;
        plight->mLightInfluence.mFluctuation = pointFluctuation;
        dKy_plight_set(&plight->mLightInfluence);
        plight->mUpdateDelay = delay;
        plight->field_0x00 = 0;
        plight->field_0x04 = type;
    } else {
        plight->mLightInfluence.mColorR = 255;
        plight->mLightInfluence.mColorB = 120;
        plight->mLightInfluence.mColorG = 255;
        if (combined != 103) {
            plight->mLightInfluence.mFluctuation = effectFluctuation;
            plight->mBrightness = effectBrightness;
        } else {
            plight->mLightInfluence.mFluctuation = specialFluctuation;
            plight->mBrightness = specialBrightness;
        }
        dKy_efplight_set(&plight->mLightInfluence);
        plight->mUpdateDelay = delay;
        plight->field_0x04 = type;
        plight->field_0x00 = 0;
    }
}

/* 02563BF0 */
void dKydm_demo_plight_delete(void* light) {
    dKy_getEnvlight();
    if (!light) return;
    DEMO_PLIGHT* plight = (DEMO_PLIGHT*)light;
    if (plight->field_0x04 < 100) dKy_plight_cut(&plight->mLightInfluence);
    else dKy_efplight_cut(&plight->mLightInfluence);
}

/* 02563C50 */
void dKydm_demo_plight_execute(void* light, void* pos) {
    dKy_getEnvlight();
    if (!light) return;
    DEMO_PLIGHT* plight = (DEMO_PLIGHT*)light;
    u32 delay = plight->mUpdateDelay;
    // HD: Delay updates, then smooth power with a random brightness variation.
    if (delay) { plight->mUpdateDelay = delay - 1; return; }
    cXyz* position = (cXyz*)pos;
    plight->mLightInfluence.mPos.x= position->x;
    f32 brightness = plight->mBrightness;
    plight->mLightInfluence.mPos.y= position->y;
    f32 scale = brightnessScale;
    plight->mLightInfluence.mPos.z= position->z;
    f32 random = cM_rndF(gabi::fmuls_ppc(brightness, scale));
    cLib_addCalc(&plight->mLightInfluence.mPower, gabi::fadds_ppc(brightness, random), scale, brightnessMaxStep, scale);
}

/* 02563D44 */
void __sinit_d_kankyo_demo_cpp() {
    sinit_header_statics_rodata(0x104773F4, 0x101E99CC, 0x10477400, 0x1004F398);
}

/* WWHD glue
ext 0200ECD4 f32 cLib_addCalc(be<f32>*, f32, f32, f32, f32)
data initialPower 1004F370
data pointFluctuation 1004F374
data pointBrightness 1004F378
data effectFluctuation 1004F37C
data effectBrightness 1004F380
data specialFluctuation 1004F384
data specialBrightness 1004F388
data brightnessScale 1004F38C
data brightnessMaxStep 1004F390
*/
