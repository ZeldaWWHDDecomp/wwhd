/* HD-only crawling direction arrow layout resources; local game-derived source. */
#include "d/hd_screen_crawling_direction_arrow.h"
static hdPartsVtbl_c sArrowTable;
static u32 sArrowHolderTable;
static u32 sArrowSecondaryTable;
static hdUiStringTable sArrowStringTable;
static hdUiSafeString sArrowAnimationNames[2] = {};
static hdUiSafeString sArrowName;
typedef be<u32> hdHudWord_t;
static hdHudWord_t sArrowInitWords[4] = {};
static hdHudWord_t sArrowRegistrations[9] = {};
static f32 sArrowInitialFrame;
static f32 sArrowInitialRate;
static f32 sArrowFrame;
static f32 sArrowRate;
typedef be<u8> hdHudByte_t;
static hdHudByte_t sArrowReady;
static hdHudByte_t sArrowEnabled;
void arrowRegisterStatic(void*);
void arrowConstructByte(void*);
void arrowConstructBool(void*);

/* 02650438 */
static s32 arrowStateNumber(hdPartsStateId_c* state) { return state->mNumber; }
/* 02650538 */
void hdCrawlingDirectionArrow_c::destroy(hdCrawlingDirectionArrow_c* object, u32 flags) {
    if (!object) return;
    object->mStateHolder.mHolderVtable = &sArrowHolderTable;
    object->mSecondaryVtable = &sArrowSecondaryTable;
    hdPartsInOutLoop_c::destroy(object, 0);
    if (flags & 1) operator_delete(object);
}
/* 026505A4 */
hdCrawlingDirectionArrow_c* hdCrawlingDirectionArrow_c::construct(hdCrawlingDirectionArrow_c* object) {
    if (!object) {
        object = (hdCrawlingDirectionArrow_c*)operator_new(sizeof(hdCrawlingDirectionArrow_c));
        if (!object) return NULL;
    }
    hdPartsInOutLoop_c::construct(object);
    object->mVtable = &sArrowTable;
    object->mStateHolder.mHolderVtable = &sArrowHolderTable;
    object->mSecondaryVtable = &sArrowSecondaryTable;
    return object;
}
/* 02650610 */
u32 hdCrawlingDirectionArrow_c::setup(u32 first, u32 second, u32 third) {
    return hdPartsInOutLoop_c::init(&sArrowName, first, second, hdPartsView<hdPartsDesc_c>(third));
}
/* 02650628 */
static void __sinit_hd_screen_crawling_direction_arrow_cpp() {
    sArrowInitWords[3] = 0;
    sArrowInitWords[2] = 0;
    sArrowInitWords[1] = 0;
    sArrowInitWords[0] = 0;
    arrowRegisterStatic(&sArrowRegistrations[0]);
    sArrowFrame = sArrowInitialFrame;
    sArrowRate = sArrowInitialRate;
    arrowConstructByte(&sArrowReady);
    arrowRegisterStatic(&sArrowRegistrations[3]);
    arrowConstructBool(&sArrowEnabled);
    arrowRegisterStatic(&sArrowRegistrations[6]);
    sArrowAnimationNames[0].mVtable = &sArrowStringTable;
    sArrowAnimationNames[1].mText.v = gabi::guest_addr("G_Loop_00");
    sArrowName.mVtable = &sArrowStringTable;
    sArrowName.mText.v = gabi::guest_addr("CrawlingDirectionArrow_00");
    sArrowAnimationNames[0].mText.v = gabi::guest_addr("G_All_00");
    sArrowAnimationNames[1].mVtable = &sArrowStringTable;
}
/* WWHD glue
data sArrowTable 100EF448 nocheck
data sArrowHolderTable 100EF570 nocheck
data sArrowSecondaryTable 100EF580 nocheck
data sArrowStringTable 100EF3EC nocheck
data sArrowAnimationNames 10491D58 nocheck
data sArrowName 10491D68 nocheck
data sArrowInitWords 10491D48 nocheck
data sArrowRegistrations 101F5960 nocheck
data sArrowInitialFrame 100EF40C nocheck
data sArrowInitialRate 100EF410 nocheck
data sArrowFrame 10491D3C nocheck
data sArrowRate 10491D40 nocheck
data sArrowReady 10491D44 nocheck
data sArrowEnabled 10491D45 nocheck
str 100EF414 "G_Loop_00" in 02650628
str 100EF420 "CrawlingDirectionArrow_00" in 02650628
str 100EF43C "G_All_00" in 02650628
ext 028F026C void arrowRegisterStatic(void*)
ext 028ED6F8 void arrowConstructByte(void*)
ext 028EAB2C void arrowConstructBool(void*)
ext 026F7754 hdPartsInOutLoop_c* hdPartsInOutLoop_c::construct(hdPartsInOutLoop_c*)
ext 026F7520 void hdPartsInOutLoop_c::destroy(hdPartsInOutLoop_c*,s32)
ext 026F7844 u32 hdPartsInOutLoop_c::init(void*,u32,u32,hdPartsDesc_c*)
*/
