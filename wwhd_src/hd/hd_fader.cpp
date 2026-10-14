/** HD Fader render layer and event subscription. Private decomp-derived source. */
#include "hd/hd_fader.h"
static u32 sBaseRenderVtable;
static u32 sStringVtable;
static u32 sDelegateVtable;
static u32 sLayerVtable;
static u32 sRenderVtable;
static u32 sSubscriptionVtable;
static hdFaderRenderer_l* sRenderer;
static u32 sDrawContext;
static void* sDrawManager;
static hdFaderInitLists_l sInitLists;
static hdFaderAngles_l sAngles;
static hdFaderInitRecord_l sFirstInitRecord;
static hdFaderInitRecord_l sSecondInitRecord;
static hdFaderInitRecord_l sThirdInitRecord;
static hdFaderAngleLimits_l sAngleLimits;
void hdFader_registerStatic(hdFaderInitRecord_l*);
void hdFader_initFlag(be<u8>*);
void hdFader_initByte(be<u8>*);
void hdFader_baseDestroy(hdFaderLayer_c*, u32);
void hdFader_baseConstruct(hdFaderLayer_c*, hdFaderName_l*);
void hdFader_disposerConstruct(hdFaderSubscription_l*);
void hdFader_registerEvent(hdFaderLayer_c*, hdFaderSubscription_l*);
hdFaderDrawMode_l* hdFader_getDrawMode(void*);
void hdFader_draw(void*, u32, hdFaderEvent_c*);
static void hdFader_dispatch(hdFaderLayer_c*, hdFaderEvent_c*);

/* 02734DE0 */
u32 hdFaderLayer_c::initialize() { return 0; }
/* 02734DE8 */
u32 hdFaderLayer_c::execute() { return 0; }
/* 02734EB8 */
void hdFaderLayer_c::destroy(u32 flags) {
    if (!this) return;
    mpRenderVtable = &sBaseRenderVtable;
    hdFader_baseDestroy(this, 0);
    if (flags & 1) operator_delete(this);
}
/* 02734F18 */
static u32 hdFader_getOutput() { return HD_FADER_OUTPUT_DRC_e; }
/* 02734FC8 */
static hdFaderLayer_c* hdFaderLayer_c__ct(hdFaderLayer_c* i_this) {
    if (!i_this) {
        i_this = static_cast<hdFaderLayer_c*>(operator_new(sizeof(hdFaderLayer_c)));
        if (!i_this) return NULL;
    }
    hdFaderName_l type;
    type.mpVtable = &sStringVtable;
    type.mText = gabi::guest_addr("Fader");
    hdFader_baseConstruct(i_this, &type);
    i_this->mpLayerVtable = &sLayerVtable;
    i_this->mpRenderVtable = &sRenderVtable;
    return i_this;
}
/* 02735044 */
void hdFaderLayer_c::attachEvent() {
    hdFaderSubscription_l* subscription = static_cast<hdFaderSubscription_l*>(operator_new(sizeof(hdFaderSubscription_l)));
    if (subscription) {
        hdFader_disposerConstruct(subscription);
        // GHS placement-new tests the wrapped 32-bit member address.
        hdFaderName_l* type = gabi::at<hdFaderName_l>(gabi::ea(subscription) + offsetof(hdFaderSubscription_l, mName));
        if (!type) type = static_cast<hdFaderName_l*>(operator_new(sizeof(hdFaderName_l)));
        if (type) {
            type->mpVtable = &sStringVtable;
            type->mText = gabi::guest_addr("");
        }
        subscription->mReserved = 0;
        subscription->mpVtable = &sSubscriptionVtable;
        hdFaderDelegate_l* delegate = gabi::at<hdFaderDelegate_l>(gabi::ea(subscription) + offsetof(hdFaderSubscription_l, mDelegate));
        if (delegate) {
            delegate->mpObject = this;
            delegate->mAdjustment = 0;
            delegate->mVirtualIndex = -1;
            delegate->mpVtable = &sDelegateVtable;
            delegate->mFunction.v = gabi::guest_fn(reinterpret_cast<const void*>(&hdFader_dispatch));
        }
        subscription->mEnabled = 1;
        subscription->mName.mText = gabi::guest_addr("LayerFader");
    }
    hdFader_registerEvent(this, subscription);
}
/* 0273512C */
void hdFaderLayer_c::drawEvent(hdFaderEvent_c* event) {
    if (event->mLayer == HD_FADER_LAYER_e && sRenderer->mOutput == HD_FADER_OUTPUT_DRC_e) {
        hdFaderDrawMode_l* mode = hdFader_getDrawMode(&sDrawContext);
        mode->mMode = HD_FADER_DRAW_MODE_e;
        mode->mFlags = 0;
        hdFader_draw(sDrawManager, HD_FADER_DRAW_PASS_e, event);
    }
}
/* 027351A4 */
static void hdFader_dispatch(hdFaderLayer_c* layer, hdFaderEvent_c* event) {
    if (event->mType == HD_FADER_EVENT_DRAW_e) layer->drawEvent(event);
}
/* 027351B4 */
static void hdFader_staticInit() {
    sInitLists.mThird = 0;
    sInitLists.mFirst = 0;
    sInitLists.mFourth = 0;
    sInitLists.mSecond = 0;
    hdFader_registerStatic(&sFirstInitRecord);
    f32 minimum = sAngleLimits.mMinimum;
    f32 maximum = sAngleLimits.mMaximum;
    sAngles.mMinimum = minimum;
    sAngles.mMaximum = maximum;
    hdFader_initFlag(&sAngles.mFirstObject);
    hdFader_registerStatic(&sSecondInitRecord);
    hdFader_initByte(&sAngles.mSecondObject);
    hdFader_registerStatic(&sThirdInitRecord);
}
/* WWHD glue
 str 10142594 "Fader"
 str 10142530 ""
 str 1014259C "LayerFader"
 data sInitLists 1049FA78
 data sAngles 1049FA6C
 data sFirstInitRecord 101F8908
 data sSecondInitRecord 101F8914
 data sThirdInitRecord 101F8920
 data sAngleLimits 101425A8
 ext 028F026C void hdFader_registerStatic(hdFaderInitRecord_l*)
 ext 028ED6F8 void hdFader_initFlag(be<u8>*)
 ext 028EAB2C void hdFader_initByte(be<u8>*)
 data sBaseRenderVtable 10142508
 data sStringVtable 1014253C
 data sDelegateVtable 10142584
 data sSubscriptionVtable 10169E3C
 data sLayerVtable 101425B0
 data sRenderVtable 10142658
 data sRenderer 101F86E8
 data sDrawContext 104B45C0
 data sDrawManager 101F9968
 ext 02734248 void hdFader_baseDestroy(hdFaderLayer_c*, u32)
 ext 027340D8 void hdFader_baseConstruct(hdFaderLayer_c*, hdFaderName_l*)
 ext 02752A84 void hdFader_disposerConstruct(hdFaderSubscription_l*)
 ext 027C2450 void hdFader_registerEvent(hdFaderLayer_c*, hdFaderSubscription_l*)
 ext 027F29D4 hdFaderDrawMode_l* hdFader_getDrawMode(void*)
 ext 027F869C void hdFader_draw(void*, u32, hdFaderEvent_c*)
*/
