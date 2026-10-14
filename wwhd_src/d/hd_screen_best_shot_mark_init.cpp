/** HD BestShotMark layout initialization and state descriptors; private source. */
#include "d/hd_screen_best_shot_mark.h"

void hdBestShotResetMessage(hdTextPane_c*, hdUiSafeString*, hdUiSafeString*);

void hdPictoRegisterGlobalBestShotInit(void*);
void hdPictoInitializeLifetimeBestShotInit(void*);
void hdPictoInitializeExitByteBestShotInit(void*);
struct hdBestShotGroups_l { hdUiSafeString mEntries[2]; };
struct hdBestShotHeaderStatics_l { be<f32> mMinAngle, mMaxAngle; be<u8> mLifetime[4]; be<u32> mWords[4]; };
struct hdBestShotRegistration_l { u8 mBytes[12]; };
static hdBestShotHeaderStatics_l sBestShotHeader;
static hdBestShotRegistration_l sBestShotHeaderRegistration;
static hdBestShotRegistration_l sBestShotAngleRegistration;
static hdBestShotRegistration_l sBestShotLifetimeRegistration;
static hdUiStringTable sBestShotStringVtableBestShotInit;
static hdUiSafeString sBestShotPartNameBestShotInit;
static hdUiSafeString sBestShotLabelBestShotInit;
static hdBestShotGroups_l sBestShotGroups;
static hdPartsStateId_c sBestShotInBestShotInit;
static hdPartsStateId_c sBestShotLoopBestShotInit;
static hdPartsStateId_c sBestShotOutBestShotInit;
static hdPartsStateId_c sBestShotInvisibleBestShotInit;
static hdPartsStateId_c sBestShotBaseIn;
static hdPartsStateId_c sBestShotInitialState;
static hdPartsStateId_c sBestShotBaseLoop;
static hdPartsStateId_c sBestShotBaseOut;
static hdPartsStateId_c sBestShotBaseWaitBestShotInit;
static hdStateId sBestShotNullState;
static hdPartsStateIdVtbl_c sBestShotStateVtable;
static u32 sStateCounterReady;
static s32 sStateCounter;

/* 02624FF4 */
u32 hdBestShotMark_c::initialize(u32 arg1, u32 arg2, hdPartsDesc_c* desc) {
    if (!hdPartsInOutLoop_c::init(&sBestShotPartNameBestShotInit, arg1, arg2, desc)) return FALSE;
    hdAnimSet_operationCC(animSet(), 0, 0);
    vCalcLayout();
    startState(&sBestShotInitialState);
    initializePane();
    hdUiSafeString label;
    label.mVtable = &sBestShotStringVtableBestShotInit;
    label.mText.v = gabi::guest_addr("PictographList_00");
    hdBestShotResetMessage(mpBestShotText, &label, &sBestShotLabelBestShotInit);
    return TRUE;
}

/* 026250E4 */
static void initializeBestShotStatics() {
    sBestShotHeader.mWords[3] = 0;
    sBestShotHeader.mWords[2] = 0;
    sBestShotHeader.mWords[1] = 0;
    sBestShotHeader.mWords[0] = 0;
    hdPictoRegisterGlobalBestShotInit(&sBestShotHeaderRegistration);
    sBestShotHeader.mMinAngle = -3.1415927410125732f;
    sBestShotHeader.mMaxAngle = 3.1415927410125732f;
    hdPictoInitializeLifetimeBestShotInit(sBestShotHeader.mLifetime);
    hdPictoRegisterGlobalBestShotInit(&sBestShotAngleRegistration);
    hdPictoInitializeExitByteBestShotInit(&sBestShotHeader.mLifetime[1]);
    hdPictoRegisterGlobalBestShotInit(&sBestShotLifetimeRegistration);
    sBestShotGroups.mEntries[0].mVtable = &sBestShotStringVtableBestShotInit;
    sBestShotGroups.mEntries[1].mVtable = &sBestShotStringVtableBestShotInit;
    sBestShotGroups.mEntries[1].mText.v = gabi::guest_addr("G_Loop_00");
    sBestShotLabelBestShotInit.mVtable = &sBestShotStringVtableBestShotInit;
    sBestShotLabelBestShotInit.mText.v = gabi::guest_addr("L_BtnPictograph_00_L_BestShotMark_00-T_BestShot_00");
    sBestShotPartNameBestShotInit.mVtable = &sBestShotStringVtableBestShotInit;
    sBestShotGroups.mEntries[0].mText.v = gabi::guest_addr("G_InOut_00");
    sBestShotPartNameBestShotInit.mText.v = gabi::guest_addr("BestShotMark_00");
    s32 number;
    if (sStateCounterReady != 0) number = sStateCounter;
    else { sStateCounterReady = 1; number = 0; }
    number = u32(number) + 1;
    sBestShotInBestShotInit.setup(number, "::StateID_In", &sBestShotStateVtable, 28, (hdStateId*)&sBestShotBaseIn);
    number = u32(number) + 1;
    sBestShotLoopBestShotInit.setup(number, "::StateID_Loop", &sBestShotStateVtable, 31, (hdStateId*)&sBestShotBaseLoop);
    number = u32(number) + 1;
    sBestShotOutBestShotInit.setup(number, "::StateID_Out", &sBestShotStateVtable, 34, (hdStateId*)&sBestShotBaseOut);
    number = u32(number) + 1;
    sBestShotInvisibleBestShotInit.setup(number, "::StateID_Invisible", &sBestShotStateVtable, 37, &sBestShotNullState);
    sStateCounter = number;
}

/* WWHD glue
 ext 027045B0 void hdBestShotResetMessage(hdTextPane_c*, hdUiSafeString*, hdUiSafeString*)
 ext 026F7844 u32 hdPartsInOutLoop_c::init(void*, u32, u32, hdPartsDesc_c*)
 data sBestShotHeader 1048EDF8 nocheck
 data sBestShotHeaderRegistration 101F52F0 nocheck
 data sBestShotAngleRegistration 101F52FC nocheck
 data sBestShotLifetimeRegistration 101F5308 nocheck
 data sBestShotStringVtableBestShotInit 100E4E84 nocheck
 data sBestShotPartNameBestShotInit 1048EEE8 nocheck
 data sBestShotLabelBestShotInit 1048EDF0 nocheck
 data sBestShotGroups 1048EE14 nocheck
 data sBestShotInBestShotInit 1048EE28 nocheck
 data sBestShotLoopBestShotInit 1048EE58 nocheck
 data sBestShotOutBestShotInit 1048EE88 nocheck
 data sBestShotInvisibleBestShotInit 1048EEB8 nocheck
 data sBestShotInitialState 1049DD60 nocheck
 data sBestShotBaseIn 1049DB98 nocheck
 data sBestShotBaseLoop 1049DBC8 nocheck
 data sBestShotBaseOut 1049DBF8 nocheck
 data sBestShotBaseWaitBestShotInit 1049DB68 nocheck
 data sBestShotNullState 101FF32C nocheck
 data sBestShotStateVtable 100E4EE4 nocheck
 data sStateCounterReady 101FD8E4 nocheck
 data sStateCounter 101FDD50 nocheck
 str 100E4F44 "PictographList_00"
 str 100E4F60 "BestShotMark_00" in 026250E4
 str 100E4F70 "::StateID_Invisible"
 str 100E4F84 "G_Loop_00"
 str 100E4F90 "::StateID_Out"
 str 100E4FA0 "G_InOut_00"
 str 100E4FAC "L_BtnPictograph_00_L_BestShotMark_00-T_BestShot_00"
 str 100E4FE0 "::StateID_In"
 str 100E4FF0 "::StateID_Loop"
 ext 028F026C void hdPictoRegisterGlobalBestShotInit(void*)
 ext 028ED6F8 void hdPictoInitializeLifetimeBestShotInit(void*)
 ext 028EAB2C void hdPictoInitializeExitByteBestShotInit(void*)

*/
