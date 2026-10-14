/* HD collected-map count button. Formatter is an external library binding. */
#include "d/hd_btn_map_icon_number.h"
struct hdMapCountVtable_c { be<u32> mWords[2]; };
static hdMapCountVtable_c sMapCountVtable;
static hdMapCountVtable_c sMapCountStateVtable;
static hdMapCountVtable_c sMapCountOwnerVtable;
extern void* mapPatternAllocate(u32);
extern void mapIconDelete(void*);
/* 0263AE44 */
void hdBtnMapIconNumber_c::destroy(hdBtnMapIconNumber_c* icon,u32 flags) {
    if (!icon) return;
    icon->mStateHolder.mHolderVtable=sMapCountStateVtable.mWords;
    icon->mSecondaryVtable=sMapCountOwnerVtable.mWords;
    reinterpret_cast<NoDecideButton_l*>(icon)->destroy(0);
    if (flags&1) mapIconDelete(icon);
}
/* 0263AEB0 */
hdBtnMapIconNumber_c* hdBtnMapIconNumber_c::construct(hdBtnMapIconNumber_c* icon) {
    if (!icon) icon=(hdBtnMapIconNumber_c*)mapPatternAllocate(sizeof(hdBtnMapIconNumber_c));
    if (!icon) return icon;
    NoDecideButton_l::construct(reinterpret_cast<NoDecideButton_l*>(icon));
    icon->mVtable=reinterpret_cast<hdPartsVtbl_c*>(sMapCountVtable.mWords);
    icon->mSecondaryVtable=sMapCountOwnerVtable.mWords;
    icon->mStateHolder.mHolderVtable=sMapCountStateVtable.mWords;
    // HD: retain the wrapped embedded-array address for allocation at guest NULL.
    be<s32>* count=gabi::at<be<s32>>(gabi::ea(icon)+offsetof(hdBtnMapIconNumber_c,mPaneCount));
    if (!count) count=(be<s32>*)mapPatternAllocate(8);
    if (count) {
        gptr<gptr<HdMapIconPane>>* panes=reinterpret_cast<gptr<gptr<HdMapIconPane>>*>(count+1);
        *panes=nullptr;
        *count=0;
    }
    icon->mLastCount=0xFFFFFFFF;
    return icon;
}
struct hdMapCountSave_c { u8 mPrefix[0xE4]; u8 mMapData; };
struct hdMapCountSavePointer_c { gptr<hdMapCountSave_c> mpSave; };
static hdMapCountSavePointer_c sMapCountSave;
struct hdMapCountFormat_c { be<u16> mCharacters[7]; };
static hdMapCountFormat_c sMapCountFormat;
struct hdMapCountFormattedText_c { u8 mStorage[0x1C]; };
extern u32 mapCountCollectNumber(void*);
extern void mapCountFormat(hdMapCountFormattedText_c*,be<u16>*,u32,u32);
extern void mapCountSetText(HdMapIconPane*,hdMapCountFormattedText_c*,u32);
/* 0263B230 */
void hdBtnMapIconNumber_c::updateCount() {
    u32 count=mapCountCollectNumber(&sMapCountSave.mpSave->mMapData);
    if (mLastCount==count) return;
    mLastCount=count;
    hdMapCountFormattedText_c text;
    mapCountFormat(&text,sMapCountFormat.mCharacters,0xD7,count);
    mapCountSetText(mpNumberPanes[0],&text,0);
}
/* 0263AD44 */
void* hdMapIconNamePointer_c::get() { return mpTarget; }
struct hdMapCountHeapVtable_l { u8 mPrefix[0x34]; gfn mAllocate; u8 mGap[4]; gfn mFree; };
struct hdMapCountHeap_l { u8 mPrefix[0xC]; gptr<hdMapCountHeapVtable_l> mVtable; };
struct hdMapCountHeapManagerPointer_c { gptr<void> mpManager; };
static hdMapLayoutHeapPointer_c sMapCountLayoutHeap;
static hdMapCountHeapManagerPointer_c sMapCountHeapManager;
extern hdMapCountHeap_l* mapCountCurrentHeap(void*);
extern hdMapCountHeap_l* mapCountFindHeap(void*,void*);
extern u32 mapCountBaseInitialize(NoDecideButton_l*,hdUiSafeString*,u32,u32,u32);
static hdUiSafeString sMapCountLayoutName;
static hdUiSafeString sMapCountPaneName;
static hdPartsTypeInfo_c sMapCountTextType;
static hdPartsTypeInfo_c sMapCountPaneType;
static hdPartsTypeInfo_c sMapCountLayoutType;
static hdPartsTypeInfo_c sMapCountRootType;
struct hdMapCountTypeGuard_c { be<u32> mValue; };
static hdMapCountTypeGuard_c sMapCountTextTypeGuard;
static hdMapCountTypeGuard_c sMapCountPaneTypeGuard;
static hdMapCountTypeGuard_c sMapCountLayoutTypeGuard;
static hdMapCountTypeGuard_c sMapCountRootTypeGuard;
static void prepareMapCountTextType() {
    if (sMapCountTextTypeGuard.mValue) return;
    sMapCountTextTypeGuard.mValue=1;
    if (!sMapCountPaneTypeGuard.mValue) {
        sMapCountPaneTypeGuard.mValue=1;
        if (!sMapCountLayoutTypeGuard.mValue) {
            sMapCountLayoutTypeGuard.mValue=1;
            if (!sMapCountRootTypeGuard.mValue) { sMapCountRootType.mParent=nullptr; sMapCountRootTypeGuard.mValue=1; }
            sMapCountLayoutType.mParent=&sMapCountRootType;
        }
        sMapCountPaneType.mParent=&sMapCountLayoutType;
    }
    sMapCountTextType.mParent=&sMapCountPaneType;
}
/* 0263AF48 */
u32 hdBtnMapIconNumber_c::initialize(u32 first,u32 second,u32 third) {
    if (!mapCountBaseInitialize(reinterpret_cast<NoDecideButton_l*>(this),&sMapCountLayoutName,first,second,third)) return 0;
    hdMapCountHeap_l* heap=reinterpret_cast<hdMapCountHeap_l*>((void*)sMapCountLayoutHeap.mpContext->mpHeap);
    if (!heap) heap=mapCountCurrentHeap(sMapCountHeapManager.mpManager);
    void* allocation=gabi::call_ptr<void*>(heap->mVtable->mAllocate.get(),heap,4,4);
    if (allocation) { mpNumberPanes=(gptr<HdMapIconPane>*)allocation; mPaneCount=1; }
    s32 count=mPaneCount;
    for (s32 i=0;i<count;++i) mpNumberPanes[i]=nullptr;
    for (s32 i=0;i<(s32)mPaneCount;++i) {
        gptr<HdMapIconPane>* slot=&mpNumberPanes[i];
        hdPartsLyt_c* layout=mpLayout->lyt();
        // HD: the name table advances the receiver while retaining entry zero for lookup.
        hdUiSafeString* name=gabi::at<hdUiSafeString>(gabi::ea(&sMapCountPaneName)+u32(i)*sizeof(hdUiSafeString));
        gabi::call_ptr<void>(sMapCountPaneName.mVtable->mAssureTermination.get(),name);
        hdScreenResourceObject* root=reinterpret_cast<hdScreenResourceObject*>((hdPartsPane_c*)layout->mpRootPane);
        hdPartsPane_c* pane=reinterpret_cast<hdPartsPane_c*>(root->findPane(reinterpret_cast<const char*>(sMapCountPaneName.mText.get())));
        prepareMapCountTextType();
        bool isText=false;
        if (pane) {
            hdPartsTypeInfo_c* type=pane->getRuntimeTypeInfo();
            while (type) {
                if (type==&sMapCountTextType) { isText=true; break; }
                type=type->mParent;
            }
        }
        *slot=isText?reinterpret_cast<HdMapIconPane*>(pane):nullptr;
    }
    return 1;
}
/* 0263B188 */
void hdBtnMapIconNumber_c::finalize() {
    s32 count=mPaneCount;
    if (count>0) for (s32 i=0;i<count;++i) mpNumberPanes[i]=nullptr;
    if (count>0 || mpNumberPanes) {
        hdMapCountHeap_l* heap=mapCountFindHeap(sMapCountHeapManager.mpManager,mpNumberPanes);
        gabi::call_ptr(heap->mVtable->mFree.get(),heap,mpNumberPanes.get());
        mPaneCount=0;
        mpNumberPanes=nullptr;
    }
    reinterpret_cast<NoDecideButton_l*>(this)->reset();
}
struct hdMapCountList_c { be<u32> mWords[4]; };
struct hdMapCountRegistration_c { be<u32> mWords[3]; };
struct hdMapCountFlag_c { be<u8> mValue; };
struct hdMapCountAngle_c { be<f32> mValue; };
static hdMapCountList_c sMapCountList;
static hdMapCountRegistration_c sMapCountListRegistration;
static hdMapCountRegistration_c sMapCountFirstFlagRegistration;
static hdMapCountRegistration_c sMapCountSecondFlagRegistration;
static hdMapCountFlag_c sMapCountFirstFlag;
static hdMapCountFlag_c sMapCountSecondFlag;
static hdMapCountAngle_c sMapCountMinimumAngle;
static hdMapCountAngle_c sMapCountMaximumAngle;
static hdUiSafeString sMapCountGroupName;
static hdUiStringTable sMapCountStringVtable;
extern void mapCountRegister(void*);
extern void mapCountConstructFirstFlag(void*);
extern void mapCountConstructSecondFlag(void*);
/* 0263B2A8 */
void hdBtnMapIconNumber_c::initializeStatics() {
    sMapCountList.mWords[2]=0;
    sMapCountList.mWords[0]=0;
    sMapCountList.mWords[3]=0;
    sMapCountList.mWords[1]=0;
    mapCountRegister(&sMapCountListRegistration);
    sMapCountMinimumAngle.mValue=-3.1415927410125732f;
    sMapCountMaximumAngle.mValue=3.1415927410125732f;
    mapCountConstructFirstFlag(&sMapCountFirstFlag);
    mapCountRegister(&sMapCountFirstFlagRegistration);
    mapCountConstructSecondFlag(&sMapCountSecondFlag);
    mapCountRegister(&sMapCountSecondFlagRegistration);
    sMapCountGroupName.mVtable=&sMapCountStringVtable;
    sMapCountGroupName.mText.v=gabi::guest_addr("G_Btn_00");
    sMapCountPaneName.mVtable=&sMapCountStringVtable;
    sMapCountPaneName.mText.v=gabi::guest_addr("T_Num_00");
    sMapCountLayoutName.mVtable=&sMapCountStringVtable;
    sMapCountLayoutName.mText.v=gabi::guest_addr("BtnMapIcon_00");
}
/* WWHD glue
ext 028F026C void mapCountRegister(void*)
ext 028ED6F8 void mapCountConstructFirstFlag(void*)
ext 028EAB2C void mapCountConstructSecondFlag(void*)
data sMapCountList 10490518
data sMapCountListRegistration 101F55A4
data sMapCountFirstFlagRegistration 101F55B0
data sMapCountSecondFlagRegistration 101F55BC
data sMapCountFirstFlag 10490514
data sMapCountSecondFlag 10490515
data sMapCountMinimumAngle 1049050C
data sMapCountMaximumAngle 10490510
data sMapCountGroupName 104904FC
data sMapCountStringVtable 100E9BDC
str 100E9C54 "G_Btn_00" in 0263B2A8
str 100E9C60 "T_Num_00" in 0263B2A8
str 100E9C44 "BtnMapIcon_00" in 0263B2A8
ext 02756140 hdMapCountHeap_l* mapCountCurrentHeap(void*)
ext 02755FEC hdMapCountHeap_l* mapCountFindHeap(void*,void*)
ext 0270178C u32 mapCountBaseInitialize(NoDecideButton_l*,hdUiSafeString*,u32,u32,u32)
ext 02700C9C void NoDecideButton_l::destroy(u32)
ext 02701718 NoDecideButton_l* NoDecideButton_l::construct(NoDecideButton_l*)
ext 02701984 void NoDecideButton_l::reset()
data sMapCountLayoutHeap 1018C404
data sMapCountHeapManager 101F8B4C
data sMapCountLayoutName 10490528
data sMapCountPaneName 10490504
data sMapCountTextType 101FDD3C
data sMapCountPaneType 101FD8F4
data sMapCountLayoutType 101FD8F8
data sMapCountRootType 101FD8FC
data sMapCountTextTypeGuard 101FDD40
data sMapCountPaneTypeGuard 101FD8F0
data sMapCountLayoutTypeGuard 101FD89C
data sMapCountRootTypeGuard 101FD888
ext 025B8378 u32 mapCountCollectNumber(void*)
ext 0263B3C4 void mapCountFormat(hdMapCountFormattedText_c*,be<u16>*,u32,u32)
ext 027046D4 void mapCountSetText(HdMapIconPane*,hdMapCountFormattedText_c*,u32)
data sMapCountVtable 100E9C70
data sMapCountStateVtable 100E9E70
data sMapCountOwnerVtable 100E9E80
data sMapCountSave 101F84DC
data sMapCountFormat 100E9C2C
*/
