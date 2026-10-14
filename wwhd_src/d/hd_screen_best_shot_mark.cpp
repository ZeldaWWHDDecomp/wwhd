/** HD BestShotMark_00 label states; private binary-derived source. */
#include "d/hd_screen_best_shot_mark.h"

void hdPictoDestroyStringsBestShot(void*, u32, u32, u32, u32, u32);
void hdPictoRegisterGlobalBestShot(void*);

static hdPartsVtbl_c sBestShotVtable;
static u32 sBestShotHolderVtable;
static u32 sBestShotSecondaryVtable;
static hdUiSafeString sBestShotPaneName;
static u32 sBestShotPaneNameReady;
struct hdPictoRegistration_l { u8 mBytes[12]; };
static hdPictoRegistration_l sBestShotNameRegistration;
static hdUiStringTable sBestShotStringVtableBestShot;
static u32 sTextPaneTypeReadyBestShot;
static u32 sTextPaneBaseTypeReadyBestShot;
static u32 sPaneTypeReadyBestShot;
static u32 sRootTypeReadyBestShot;
static hdPartsTypeInfo_c sTextPaneTypeBestShot;
static hdPartsTypeInfo_c sTextPaneBaseTypeBestShot;
static hdPartsTypeInfo_c sPaneTypeBestShot;
static hdPartsTypeInfo_c sRootTypeBestShot;
static hdPartsStateId_c sBestShotInBestShot;
static hdPartsStateId_c sBestShotLoopBestShot;
static hdPartsStateId_c sBestShotOutBestShot;
static hdPartsStateId_c sBestShotInvisibleBestShot;
static hdPartsStateId_c sBestShotBaseWaitBestShot;

static hdUiSafeString sBestShotLabelBestShot;
static hdUiSafeString sBestShotPartNameBestShot;

/* 02624A70 */
static u32 bestShotStateNumber(hdPartsStateId_c* state) { return state->mNumber; }

/* 02624B70 */
static void destroyBestShotPaneName() {
    hdPictoDestroyStringsBestShot(&sBestShotPaneName, 1, sizeof(hdUiSafeString), 0x026253D0, 0, 0);
}

/* 02624B94 */
void hdBestShotMark_c::destroy(hdBestShotMark_c* part, s32 flags) {
    if (part != NULL) {
        part->mStateHolder.mHolderVtable = &sBestShotHolderVtable;
        part->mSecondaryVtable = &sBestShotSecondaryVtable;
        hdPartsInOutLoop_c::destroy(part, 0);
        if (flags & 1) operator_delete(part);
    }
}

/* 02624C00 */
void hdBestShotMark_c::inEnter() {
    hdAnimSet_operationCC(animSet(), 0, 0);
    hdPartsInOutLoop_c::startAnim(0);
}

/* 02624C48 */
void hdBestShotMark_c::inExecute() {
    if (hdAnimSet_isEnd(animSet(), 0)) changeState(&sBestShotLoopBestShot);
    vCalcLayout();
}

/* 02624CAC */
void hdBestShotMark_c::loopEnter() {
    hdPictoRepeatAnimation(animSet(), 0, 0);
    hdPartsInOutLoop_c::startAnim(2);
}

/* 02624CF4 */
void hdBestShotMark_c::loopExecute() { vCalcLayout(); }

/* 02624D04 */
void hdBestShotMark_c::outEnter() { hdPartsInOutLoop_c::startAnim(1); }

/* 02624D0C */
void hdBestShotMark_c::outExecute() {
    if (hdAnimSet_isEnd(animSet(), 1)) changeState(&sBestShotBaseWaitBestShot);
    vCalcLayout();
}

/* 02624D70 */
void hdBestShotMark_c::invisibleEnter() { hdAnimSet_operationCC(animSet(), 0, 0); }

/* 02624D84 */
hdBestShotMark_c* hdBestShotMark_c::construct(hdBestShotMark_c* part) {
    if (part == NULL) {
        part = (hdBestShotMark_c*)operator_new(sizeof(hdBestShotMark_c));
        if (part == NULL) return NULL;
    }
    hdPartsInOutLoop_c::construct(part);
    part->mVtable = &sBestShotVtable;
    part->mSecondaryVtable = &sBestShotSecondaryVtable;
    part->mStateHolder.mHolderVtable = &sBestShotHolderVtable;
    // GHS preserves the embedded trivial constructors' wrapped NULL allocation cases.
    if (u32(gabi::ea(part) + offsetof(hdBestShotMark_c, mpBestShotText)) == 0) operator_new(4);
    if (u32(gabi::ea(part) + offsetof(hdBestShotMark_c, mLabelStorage)) == 0) operator_new(4);
    return part;
}

/* 02624E10 */
void hdBestShotMark_c::initializePane() {
    hdPartsPaneSRT_c* root;
    if (sBestShotPaneNameReady != 0) {
        root = hdPictoRoot(this);
    } else {
        sBestShotPaneNameReady = 1;
        sBestShotPaneName.mVtable = &sBestShotStringVtableBestShot;
        sBestShotPaneName.mText.v = gabi::guest_addr("T_BestShot_00");
        hdPictoRegisterGlobalBestShot(&sBestShotNameRegistration);
        root = hdPictoRoot(this);
    }
    hdPartsPaneSRTVtbl_c* table = root->srtVtbl();
    sBestShotPaneName.assureTermination();
    hdPartsPaneSRT_c* pane = gabi::call_ptr<hdPartsPaneSRT_c*>(table->mFindPane.get(), root, sBestShotPaneName.mText.get(), 1);
    if (sTextPaneTypeReadyBestShot == 0) {
        sTextPaneTypeReadyBestShot = 1;
        if (sTextPaneBaseTypeReadyBestShot == 0) {
            sTextPaneBaseTypeReadyBestShot = 1;
            if (sPaneTypeReadyBestShot == 0) {
                sPaneTypeReadyBestShot = 1;
                if (sRootTypeReadyBestShot == 0) {
                    sRootTypeReadyBestShot = 1;
                    sRootTypeBestShot.mParent = NULL;
                }
                sPaneTypeBestShot.mParent = &sRootTypeBestShot;
            }
            sTextPaneBaseTypeBestShot.mParent = &sPaneTypeBestShot;
        }
        sTextPaneTypeBestShot.mParent = &sTextPaneBaseTypeBestShot;
    }
    if (pane != NULL) {
        hdPartsTypeInfo_c* type = pane->getRuntimeTypeInfo();
        while (type != NULL) {
            if (gabi::ea(type) == gabi::ea(&sTextPaneTypeBestShot)) {
                mpBestShotText = (hdTextPane_c*)pane;
                return;
            }
            type = type->mParent;
        }
    }
    mpBestShotText = NULL;
}

/* WWHD glue
 ext 026F7520 void hdPartsInOutLoop_c::destroy(hdPartsInOutLoop_c*, s32)
 ext 026F7754 hdPartsInOutLoop_c* hdPartsInOutLoop_c::construct(hdPartsInOutLoop_c*)
 ext 026F75C4 void hdPartsInOutLoop_c::startAnim(u32)
 data sBestShotVtable 100E5000 nocheck
 data sBestShotHolderVtable 100E5140 nocheck
 data sBestShotSecondaryVtable 100E5150 nocheck
 data sBestShotPaneName 1048EDE8 nocheck
 data sBestShotPaneNameReady 1048EE24 nocheck
 data sBestShotNameRegistration 101F52E4 nocheck
 data sBestShotStringVtableBestShot 100E4E84 nocheck
 data sTextPaneTypeReadyBestShot 101FDD40 nocheck
 data sTextPaneBaseTypeReadyBestShot 101FD8F0 nocheck
 data sPaneTypeReadyBestShot 101FD89C nocheck
 data sRootTypeReadyBestShot 101FD888 nocheck
 data sTextPaneTypeBestShot 101FDD3C nocheck
 data sTextPaneBaseTypeBestShot 101FD8F4 nocheck
 data sPaneTypeBestShot 101FD8F8 nocheck
 data sRootTypeBestShot 101FD8FC nocheck
 data sBestShotInBestShot 1048EE28 nocheck
 data sBestShotLoopBestShot 1048EE58 nocheck
 data sBestShotOutBestShot 1048EE88 nocheck
 data sBestShotInvisibleBestShot 1048EEB8 nocheck
 data sBestShotBaseWaitBestShot 1049DB68 nocheck
 data sBestShotLabelBestShot 1048EDF0 nocheck
 data sBestShotPartNameBestShot 1048EEE8 nocheck
 str 100E4F34 "T_BestShot_00"
 ext 028F0164 void hdPictoDestroyStringsBestShot(void*, u32, u32, u32, u32, u32)
 ext 028F026C void hdPictoRegisterGlobalBestShot(void*)
 ext 02005708 void hdPictoRepeatAnimation(hdAnimSet*, u32, u32)
*/
