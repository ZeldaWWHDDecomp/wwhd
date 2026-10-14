/* HD-only crawling direction screen and four animated children. */
#include "d/hd_screen_crawling_direction.h"
#include "d/hd_screen_crawling_direction_arrow.h"
static hdPartsVtbl_c sCrawlTable;
static u32 sCrawlHolderTable;
static u32 sCrawlSecondaryTable;
static hdPartsStateId_c sCrawlWait;
static hdPartsStateId_c sCrawlDisplay;
static hdPartsStateId_c sCrawlEnd;
static hdUiSafeString sCrawlClassNames[4] = {};
static hdUiSafeString sCrawlResources[4] = {};
static hdUiSafeString sCrawlArchives[2] = {};
static hdUiSafeString sCrawlName;
static hdUiSafeString sCrawlArrowName;
static hdUiStringTable sCrawlStringTable;
static hdPartsStateIdVtbl_c sCrawlStateTable;
static hdStateId sCrawlNullState;
typedef be<u32> hdHudWord_t;
static hdHudWord_t sCrawlStateGuard;
static hdHudWord_t sCrawlStateCounter;
static hdHudWord_t sCrawlInitWords[4] = {};
static hdHudWord_t sCrawlRegistrations[9] = {};
typedef be<u8> hdHudByte_t;
static hdHudByte_t sCrawlMasks[4] = {};
static hdHudByte_t sCrawlReady;
static hdHudByte_t sCrawlEnabled;
static f32 sCrawlInitialFrame;
static f32 sCrawlInitialRate;
static f32 sCrawlFrame;
static f32 sCrawlRate;
typedef gptr<hdScreenUiSystem> hdHudUiPointer_t;
static hdHudUiPointer_t sCrawlUi;
hdCrawlingGameInfo_c* crawlGameInfo(); u32 crawlIsHidden();
void crawlRegisterStatic(void*); void crawlConstructByte(void*); void crawlConstructBool(void*);
/* 0264F8E0 */
static s32 crawlStateNumber(hdPartsStateId_c* state) { return state->mNumber; }
/* 0264F9E0 */
void hdCrawlingDirection_c::destroy(hdCrawlingDirection_c* object, u32 flags) {
    if(!object) return;
    object->mStateHolder.mHolderVtable = &sCrawlHolderTable;
    object->mSecondaryVtable = &sCrawlSecondaryTable;
    hdParts_c::destroy(object, 0);
    if(flags&1) operator_delete(object);
}
/* 0264FA4C */
void hdCrawlingDirection_c::waitEnter() { mShown = 0; }
/* 0264FA58 */
void hdCrawlingDirection_c::waitExecute() {
    if(crawlGameInfo()->mDirections && !crawlIsHidden()) changeState(&sCrawlDisplay);
}
/* 0264FAAC */
void hdCrawlingDirection_c::updateArrows() {
    for(u32 i = 0; i<4; ++i) {
        hdParts_c* arrow = child(i);
        hdCrawlingGameInfo_c* info = crawlGameInfo();
        u8 mask = sCrawlMasks[i];
        if(info->mDirections&mask) hdPartsView<hdPartsInOutLoop_c>(gabi::ea(arrow))->requestIn(); else hdPartsView<hdPartsInOutLoop_c>(gabi::ea(arrow))->deactivate();
    }
}
/* 0264FB44 */
void hdCrawlingDirection_c::displayEnter() { updateArrows(); mShown = 1; }
/* 0264FB78 */
void hdCrawlingDirection_c::displayExecute() {
    if(!crawlGameInfo()->mDirections || crawlIsHidden()) changeState(&sCrawlEnd);
    vExecuteChildren(); vCalcLayout();
}
/* 0264FBF4 */
void hdCrawlingDirection_c::hideArrows() { for(u32 i = 0; i<4; ++i) hdPartsView<hdPartsInOutLoop_c>(gabi::ea(child(i)))->requestOut(); }
/* 0264FC6C */
void hdCrawlingDirection_c::endEnter() { hideArrows(); }
/* 0264FC70 */
u32 hdCrawlingDirection_c::allArrowsClosed() {
    u32 count = mChildren.mNum;
    gptr<hdParts_c>* data = mChildren.mpData;
    for(u32 i = 0; i<4; ++i) {
        hdPartsInOutLoop_c* arrow = (hdPartsInOutLoop_c*)data[i<count?i:0].get();
        if(!arrow->mIsClosed) return 0;
    }
    return 1;
}
/* 0264FCD0 */
void hdCrawlingDirection_c::endExecute() {
    if(allArrowsClosed()) changeState(&sCrawlWait);
    vExecuteChildren(); vCalcLayout();
}
/* 0264FD3C */
hdCrawlingDirection_c* hdCrawlingDirection_c::construct(hdCrawlingDirection_c* object) {
    if(!object) { object = (hdCrawlingDirection_c*)operator_new(sizeof(hdCrawlingDirection_c)); if(!object) return NULL; }
    hdParts_c::construct(object);
    object->mStateHolder.mHolderVtable = &sCrawlHolderTable; object->mVtable = &sCrawlTable;
    object->mSecondaryVtable = &sCrawlSecondaryTable; object->mShown = 0;
    return object;
}
/* 0264FDB0 */
u32 hdCrawlingDirection_c::setup(u32 first, u32 second, void* desc) {
    attachArchives(sCrawlArchives, 2);
    if(!vLoad(&sCrawlName, gabi::ea(mpResAccessor.get()), first, second, desc) || !gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this)) return 0;
    startState(&sCrawlWait);
    return 1;
}
/* 0264FEB4 */
void hdCrawlingDirection_c::release() { vDeleteChildren(); vDestroyLayout(); detachArchives(); }
/* 0264FF08 */
void hdCrawlingDirection_c::execute() { hdParts_executeStateMachine(stateMachine()); }
static bool crawlStringsEqual(const be<u8>* left, const be<u8>* right) {
    for(u32 i = 0; i<0x40001; ++i) { u8 a = left[i], b = right[i]; if(a != b) return false; if(!a) return true; }
    return false;
}
/* 0264FF10 */
u32 hdCrawlingDirection_c::createArrow(hdCrawlingPartDesc_c* desc) {
    hdScreenHeap* heap = sCrawlUi->mHeap;
    for(u32 i = 0; i<4; ++i) {
        desc->mClassName.assureTermination(); desc->mClassName.assureTermination();
        be<u8>* text = desc->mClassName.mText;
        hdUiSafeString* className = &sCrawlClassNames[i];
        className->assureTermination();
        if(text != className->mText.get() && !crawlStringsEqual(desc->mClassName.mText, className->mText)) continue;
        desc->mResource.assureTermination(); desc->mResource.assureTermination();
        text = desc->mResource.mText;
        sCrawlArrowName.assureTermination();
        if(text != sCrawlArrowName.mText.get() && !crawlStringsEqual(desc->mResource.mText, sCrawlArrowName.mText))
            return createChild(i, reinterpret_cast<hdPartsDesc_c*>(desc));
        gptr<hdParts_c>* slot = childSlot(i);
        hdCrawlingDirectionArrow_c* arrow = (hdCrawlingDirectionArrow_c*)hdParts_new(0x5c, heap, 4);
        if(arrow) arrow = hdCrawlingDirectionArrow_c::construct(arrow);
        *slot=arrow;
        arrow = (hdCrawlingDirectionArrow_c*)child(i);
        if(!arrow) return 0;
        return arrow->setup(0, 0, gabi::ea(desc));
    }
    return 1;
}
/* 02650168 */
static void __sinit_hd_screen_crawling_direction_cpp() {
    sCrawlInitWords[3] = 0; sCrawlInitWords[2] = 0; sCrawlInitWords[1] = 0; sCrawlInitWords[0] = 0;
    crawlRegisterStatic(&sCrawlRegistrations[0]);
    sCrawlFrame = sCrawlInitialFrame; sCrawlRate = sCrawlInitialRate;
    crawlConstructByte(&sCrawlReady); crawlRegisterStatic(&sCrawlRegistrations[3]);
    crawlConstructBool(&sCrawlEnabled); crawlRegisterStatic(&sCrawlRegistrations[6]);
    sCrawlClassNames[0].mVtable = &sCrawlStringTable;
    sCrawlClassNames[0].mText.v = gabi::guest_addr("L_Arrow_00");
    sCrawlArchives[0].mVtable = &sCrawlStringTable;
    sCrawlClassNames[1].mVtable = &sCrawlStringTable;
    sCrawlResources[3].mVtable = &sCrawlStringTable;
    sCrawlResources[1].mVtable = &sCrawlStringTable;
    sCrawlClassNames[1].mText.v = gabi::guest_addr("L_Arrow_01");
    sCrawlName.mVtable = &sCrawlStringTable;
    sCrawlClassNames[2].mVtable = &sCrawlStringTable;
    sCrawlName.mText.v = gabi::guest_addr("CrawlingDirection_00");
    sCrawlResources[1].mText.v = gabi::guest_addr("CrawlingDirectionArrow_00");
    sCrawlArchives[0].mText.v = gabi::guest_addr("CrawlingDirection_00");
    sCrawlResources[0].mText.v = gabi::guest_addr("CrawlingDirectionArrow_00");
    sCrawlClassNames[2].mText.v = gabi::guest_addr("L_Arrow_02");
    u32 guard = sCrawlStateGuard;
    sCrawlResources[2].mText.v = gabi::guest_addr("CrawlingDirectionArrow_00");
    sCrawlArchives[1].mVtable = &sCrawlStringTable;
    sCrawlArchives[1].mText.v = gabi::guest_addr("CrawlingDirectionArrow_00");
    sCrawlClassNames[3].mVtable = &sCrawlStringTable;
    sCrawlResources[2].mVtable = &sCrawlStringTable;
    sCrawlResources[0].mVtable = &sCrawlStringTable;
    sCrawlResources[3].mText.v = gabi::guest_addr("CrawlingDirectionArrow_00");
    sCrawlClassNames[3].mText.v = gabi::guest_addr("L_Arrow_03");
    u32 number;
    if(guard) number = sCrawlStateCounter; else { sCrawlStateGuard = 1; number = 0; }
    sCrawlWait.setup(++number, "::StateID_Wait", &sCrawlStateTable, 0x19, &sCrawlNullState);
    sCrawlDisplay.setup(++number, "::StateID_Disp", &sCrawlStateTable, 0x1c, &sCrawlNullState);
    sCrawlStateCounter = number+1;
    sCrawlEnd.setup(++number, "::StateID_End", &sCrawlStateTable, 0x1f, &sCrawlNullState);
}
/* WWHD glue
data sCrawlTable 100EF2B8 nocheck
data sCrawlHolderTable 100EF3C8 nocheck
data sCrawlSecondaryTable 100EF3D8 nocheck
data sCrawlWait 10491CA4 nocheck
data sCrawlDisplay 10491CD4 nocheck
data sCrawlEnd 10491D04 nocheck
data sCrawlClassNames 10491C54 nocheck
data sCrawlResources 10491C74 nocheck
data sCrawlArchives 10491C94 nocheck
data sCrawlName 10491D34 nocheck
data sCrawlArrowName 10491D68 nocheck
data sCrawlStringTable 100EF164 nocheck
data sCrawlStateTable 100EF1C4 nocheck
data sCrawlNullState 101FF32C nocheck
data sCrawlStateGuard 101FD8E4 nocheck
data sCrawlStateCounter 101FDD50 nocheck
data sCrawlInitWords 10491C44 nocheck
data sCrawlRegistrations 101F593C nocheck
data sCrawlMasks 100EF214 nocheck
data sCrawlReady 10491C40 nocheck
data sCrawlEnabled 10491C41 nocheck
data sCrawlInitialFrame 100EF218 nocheck
data sCrawlInitialRate 100EF21C nocheck
data sCrawlFrame 10491C38 nocheck
data sCrawlRate 10491C3C nocheck
data sCrawlUi 1018C404 nocheck
str 100EF220 "CrawlingDirectionArrow_00" in 02650168
str 100EF23C "::StateID_End" in 02650168
str 100EF24C "L_Arrow_00" in 02650168
str 100EF258 "L_Arrow_01" in 02650168
str 100EF264 "L_Arrow_02" in 02650168
str 100EF270 "L_Arrow_03" in 02650168
str 100EF27C "CrawlingDirection_00" in 02650168
str 100EF294 "::StateID_Wait" in 02650168
str 100EF2A4 "::StateID_Disp" in 02650168
ext 025200D4 hdCrawlingGameInfo_c* crawlGameInfo()
ext 025986BC u32 crawlIsHidden()
ext 028F026C void crawlRegisterStatic(void*)
ext 028ED6F8 void crawlConstructByte(void*)
ext 028EAB2C void crawlConstructBool(void*)
ext 026F7908 void hdPartsInOutLoop_c::requestIn()
ext 026F7918 void hdPartsInOutLoop_c::deactivate()
ext 026F7924 void hdPartsInOutLoop_c::requestOut()
*/
