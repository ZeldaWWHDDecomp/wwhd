/** HD save-slot post captions, stage labels and layout animations; private source. */
#include "d/hd_screen_pictograph_post.h"
#include "d/hd_msg_text.h"

void hdPostResetPaneMessage(hdTextPane_c*, hdUiSafeString*, hdUiSafeString*);

void hdPictoRegisterGlobalPost(void*);
void hdPictoInitializeLifetimePost(void*);
void hdPictoInitializeExitBytePost(void*);
// The fallback index 59 reaches the lazy guards following the 59 initialized records.
struct hdPostSafeStrings_l { hdUiSafeString mLabels[60]; };
struct hdPostSummaryLabels_l { hdUiSafeString mLabels[5]; };
struct hdPostGroupNames_l { hdUiSafeString mNames[3]; };
struct hdPostAnimationNames_l { hdUiSafeString mNames[2]; };
struct hdPostZeroObject_l { be<u32> mWords[4]; };
struct hdPostRange_l { be<f32> mMin, mMax; be<u8> mLifetime[4]; };
struct hdPostRegistration_l { u8 mBytes[12]; };
struct hdPostMessageResource_l { u8 _reserved00[0x1C]; gptr<be<u8>> mpName; };
struct hdPostNameBuffer_l { PaneString_l mString; be<u16> mBuffer[20]; };
struct hdPostPlaceBuffer_l { PaneString_l mString; be<u16> mBuffer[51]; u8 _padding[2]; };
WWHD_SIZE(hdPostNameBuffer_l, 0x34);
WWHD_SIZE(hdPostPlaceBuffer_l, 0x74);
static hdPostSafeStrings_l sPostPlaceLabels;
static hdPostSummaryLabels_l sPostSummaryLabels;
static hdPostGroupNames_l sPostGroupNames;
static hdPostAnimationNames_l sPostAnimationNames;
static hdUiSafeString sPostPartName;
static hdUiStringTable sPostStringVtablePost;
static u32 sPostWideStringVtable;
static u32 sPostNameBufferVtable;
static u32 sPostPlaceBufferVtable;
static hdPostZeroObject_l sPostZero;
static hdPostRange_l sPostRange;
static hdPostRegistration_l sPostZeroRegistration;
static hdPostRegistration_l sPostRangeRegistration;
static hdPostRegistration_l sPostLifetimeRegistration;
static hdScreenUiSystem* sPostUiSystem;
static u32 sPostAnimationGroups[2] = {1, 2};
static void* sPostMessageManager;

/* 02642EEC */
void hdPictographPost_c::refreshSummary() {
    hdPostMessageResource_l* resource = (hdPostMessageResource_l*)mButton.mpLayout->lyt()->mpResourceAccessor.get();
    u32 slot = mSelectedSlot;
    if (slot > 3) slot = 3;
    mSelectedSlot = slot;
    u32 captionSlot = u8(slot);
    if (s32(captionSlot) < 0) captionSlot = 0;
    else if (captionSlot > 4) captionSlot = 4;
    hdUiSafeString caption;
    caption.mVtable = &sPostStringVtablePost;
    caption.mText = resource->mpName;
    hdPostResetPaneMessage(mpTextPanes[4], &caption, &sPostSummaryLabels.mLabels[captionSlot]);
    mpSummaries->refresh();
    hdPictureSlotSummary_c* summary = mpSummaries->slot(mSelectedSlot);
    if (summary == NULL) return;
    hdAnimSet* animations = mButton.animSet();
    if (summary->mRupees == 0) {
        hdAnimSet_setFrame(animations, 8, 2, 0.0f);
        hdUiSafeString emptyCaption;
        emptyCaption.mVtable = &sPostStringVtablePost;
        emptyCaption.mText = resource->mpName;
        hdPostResetPaneMessage(mpTextPanes[3], &emptyCaption, &sPostSummaryLabels.mLabels[3]);
    } else {
        hdAnimSet_setFrame(animations, 8, 2, 2.0f);
        PaneWork_l* work = &mpTextPanes[2]->mWork;
        hdPostCalendar_c calendar;
        u32 highTicks = summary->mTime[0];
        u32 lowTicks = summary->mTime[1];
        hdPostTicksToCalendar(highTicks, lowTicks, &calendar);
        hdUiSafeString timeCaption;
        timeCaption.mVtable = &sPostStringVtablePost;
        timeCaption.mText = resource->mpName;
        hdPostResetWork(work, &timeCaption, &sPostSummaryLabels.mLabels[4]);
        PaneWork_l* dateArguments = work;
        dateArguments->mYear = calendar.mYear;
        dateArguments->mMonth = u32(calendar.mMonth) + 1;
        dateArguments->mDay = calendar.mDay;
        dateArguments->mHour = calendar.mHour;
        dateArguments->mMinute = calendar.mMinute;
        mpTextPanes[2]->prepareMessage(work, 1);
        hdPostNameBuffer_l name;
        name.mString.mpBuffer = name.mBuffer;
        name.mString.mpVTable = (gfn*)&sPostNameBufferVtable;
        name.mString.mCapacity = 20;
        name.mBuffer[19] = 0;
        name.mBuffer[0] = 0;
        mpTextPanes[0]->setStringPrepared((PaneString_l*)&summary->mPlayerName, NULL);
    }
    u32 hasExtra = summary->mPlayerState != 0 ? 1 : 0;
    hdAnimSet_setFrame(mButton.animSet(), 7, 3, f32(hasExtra));
    mButton.vCalcLayout();
}

// Each comparison invokes both descriptors' termination hooks; the input's pointer
// is sampled before the literal hook and re-read for the scan afterward.
static bool postStageMatches(hdUiSafeString* stage, hdUiSafeString* expected, u32 count) {
    stage->assureTermination();
    stage->assureTermination();
    const be<u8>* snapshot = stage->mText;
    expected->assureTermination();
    if (gabi::ea(snapshot) == gabi::ea(expected->mText.get())) return true;
    const be<u8>* text = stage->mText;
    const be<u8>* target = expected->mText;
    for (u32 i = 0; i < count; ++i) {
        u8 left = *hdUiEntryAdvanceByte((be<u8>*)text, i);
        u8 right = *hdUiEntryAdvanceByte((be<u8>*)target, i);
        if (left != right) return false;
        if (left == 0) return true;
    }
    return true;
}

/* 02643108 */
void hdPictographPost_c::refreshPlace() {
    hdPictureSlotSummary_c* summary = mpSummaries->slot(mSelectedSlot);
    if (summary == NULL || summary->mRupees == 0) return;
    s32 place = s8(summary->mStage) - 1;
    if (place < 0) place = 0;
    hdUiSafeString* stage = (hdUiSafeString*)&summary->mDisplayName;
    hdUiSafeString sea;
    sea.mVtable = &sPostStringVtablePost; sea.mText.v = gabi::guest_addr("sea");
    if (!postStageMatches(stage, &sea, 0x40001)) {
        hdUiSafeString si;
        si.mVtable = &sPostStringVtablePost; si.mText.v = gabi::guest_addr("Si");
        hdUiSafeString adm;
        adm.mVtable = &sPostStringVtablePost; adm.mText.v = gabi::guest_addr("ADM");
        if (postStageMatches(stage, &si, 2) || postStageMatches(stage, &adm, 3)) place = 53;
        else {
            hdUiSafeString ki;
            ki.mVtable = &sPostStringVtablePost; ki.mText.v = gabi::guest_addr("ki");
            if (postStageMatches(stage, &ki, 2)) place = 52;
            else {
                hdUiSafeString earth;
                earth.mVtable = &sPostStringVtablePost; earth.mText.v = gabi::guest_addr("M_Da");
                if (postStageMatches(stage, &earth, 4)) place = 54;
                else {
                    hdUiSafeString wind;
                    wind.mVtable = &sPostStringVtablePost; wind.mText.v = gabi::guest_addr("kaz");
                    if (postStageMatches(stage, &wind, 3)) place = 55;
                    else {
                        hdUiSafeString tower;
                        tower.mVtable = &sPostStringVtablePost; tower.mText.v = gabi::guest_addr("M_N");
                        hdUiSafeString towerBoss;
                        towerBoss.mVtable = &sPostStringVtablePost; towerBoss.mText.v = gabi::guest_addr("M_Dr");
                        if (postStageMatches(stage, &tower, 3) || postStageMatches(stage, &towerBoss, 4)) place = 51;
                        else {
                            hdUiSafeString ganon;
                            ganon.mVtable = &sPostStringVtablePost; ganon.mText.v = gabi::guest_addr("G");
                            hdUiSafeString ganonBoss;
                            ganonBoss.mVtable = &sPostStringVtablePost; ganonBoss.mText.v = gabi::guest_addr("X");
                            if (postStageMatches(stage, &ganon, 1) || postStageMatches(stage, &ganonBoss, 1)) place = 57;
                            else {
                                hdUiSafeString hyrule;
                                hyrule.mVtable = &sPostStringVtablePost; hyrule.mText.v = gabi::guest_addr("H");
                                hdUiSafeString hyruleRoom;
                                hyruleRoom.mVtable = &sPostStringVtablePost; hyruleRoom.mText.v = gabi::guest_addr("ke");
                                if (postStageMatches(stage, &hyrule, 1) || postStageMatches(stage, &hyruleRoom, 2)) place = 56;
                                else {
                                    hdUiSafeString forsaken;
                                    forsaken.mVtable = &sPostStringVtablePost; forsaken.mText.v = gabi::guest_addr("Ma");
                                    hdUiSafeString forsakenSecond;
                                    forsakenSecond.mVtable = &sPostStringVtablePost; forsakenSecond.mText.v = gabi::guest_addr("M2");
                                    hdUiSafeString forsakenJail;
                                    forsakenJail.mVtable = &sPostStringVtablePost; forsakenJail.mText.v = gabi::guest_addr("Mj");
                                    hdUiSafeString forsakenRoom;
                                    forsakenRoom.mVtable = &sPostStringVtablePost; forsakenRoom.mText.v = gabi::guest_addr("ma");
                                    if (postStageMatches(stage, &forsaken, 2) || postStageMatches(stage, &forsakenSecond, 2) ||
                                        postStageMatches(stage, &forsakenJail, 2) || postStageMatches(stage, &forsakenRoom, 2)) {
                                        place = summary->mPlayerSlot != 0 ? 50 : 58;
                                    } else {
                                        hdUiSafeString underwater;
                                        underwater.mVtable = &sPostStringVtablePost; underwater.mText.v = gabi::guest_addr("A_um");
                                        if (postStageMatches(stage, &underwater, 4)) place = 58;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (place > 59) place = 59;
    hdUiSafeString messageSet;
    messageSet.mVtable = &sPostStringVtablePost;
    messageSet.mText.v = gabi::guest_addr("MenuMiiverseContent_00");
    HdMsgText work;
    PaneWork_l* workView = (PaneWork_l*)&work;
    hdPostConstructWork(workView);
    hdPostResetWork(workView, &messageSet, &sPostPlaceLabels.mLabels[place]);
    hdPostLookupMessage(sPostMessageManager, workView, &work.mSetName, (PaneString_l*)&work.mLabel);
    hdPostPlaceBuffer_l placeName;
    placeName.mBuffer[50] = 0;
    placeName.mBuffer[0] = 0;
    placeName.mString.mpBuffer = placeName.mBuffer;
    placeName.mString.mpVTable = (gfn*)&sPostPlaceBufferVtable;
    placeName.mString.mCapacity = 51;
    PaneLabelTable_l* labels = workView->mpLabels;
    u32 index = workView->mLabel;
    void* entry = NULL;
    if (index < labels->mCount) entry = hdPostLookupEntry(labels->mpData, index);
    be<u16>* destination = (be<u16>*)placeName.mString.mpBuffer.get();
    PaneString_l text;
    text.mpVTable = (gfn*)&sPostWideStringVtable;
    text.mpBuffer = entry;
    hdPostAssureWideString(&text);
    be<u16>* characters = hdPartsView<be<u16>>(gabi::ea(text.mpBuffer.get()));
    s32 length = 0;
    if (*characters != 0) {
        do {
            ++length;
            if (length > 0x40000) { length = 0; break; }
            characters = hdPartsView<be<u16>>(gabi::ea(characters) + 2);
        } while (*characters != 0);
    }
    s32 capacity = placeName.mString.mCapacity;
    u32 termination = hdPartsView<hdUiStringTable>(gabi::ea(text.mpVTable.get()))->mAssureTermination.get();
    if (length >= capacity) length = capacity - 1;
    gabi::call_ptr(termination, &text);
    hdPostCopyBytes(destination, text.mpBuffer, u32(length) * 2, 0);
    *hdPartsView<be<u16>>(gabi::ea(destination) + u32(length) * 2) = 0;
    mpTextPanes[1]->setStringPrepared(&placeName.mString, NULL);
}

/* 02643F7C */
u32 hdPictographPost_c::initialize(u32 arg1, u32 arg2, hdPartsDesc_c* desc) {
    if (!hdPartsBasicButton_init(&mButton, &sPostPartName, arg1, arg2, desc)) return FALSE;
    initializeButtonPanes();
    initializeTextPanes();
    initializeMarkPanes();
    refreshSummary();
    refreshPlace();
    return TRUE;
}

/* 02644008 */
void hdPictographPost_c::showPlayerName() {
    mpSummaries->refresh();
    hdPictureSlotSummary_c* summary = mpSummaries->slot(mSelectedSlot);
    PaneString_l* name = hdPartsView<PaneString_l>(gabi::ea(summary) + offsetof(hdPictureSlotSummary_c, mPlayerName));
    mpTextPanes[0]->setStringPrepared(name, NULL);
    hdAnimSet_setFrame(mButton.animSet(), 8, 2, 1.0f);
}

/* 02644074 */
void hdPictographPost_c::hidePlayerName() {
    hdAnimSet_setFrame(mButton.animSet(), 8, 2, 0.0f);
    mButton.vCalcLayout();
}

/* 026440CC */
u32 hdPictographPost_c::load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    hdScreenHeap* heap = sPostUiSystem->mHeap;
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), heap, 4);
    if (layout != NULL) layout = hdPartsLayout_c::construct(layout);
    mButton.mpLayout = layout;
    if (layout == NULL) return FALSE;
    layout->vLoad(name, arg1, arg2, arg3, 9, 5, desc);
    for (u32 i = 0; i < 2; ++i) {
        hdPostBindAnimation(mButton.animSet(), i + 7, &sPostAnimationNames.mNames[i], &sPostGroupNames.mNames[sPostAnimationGroups[i]]);
    }
    return TRUE;
}

/* 026441DC */
static void initializePostStatics() {
    sPostZero.mWords[3] = 0;
    sPostZero.mWords[2] = 0;
    sPostZero.mWords[1] = 0;
    sPostZero.mWords[0] = 0;
    hdPictoRegisterGlobalPost(&sPostZeroRegistration);
    sPostRange.mMin = -3.1415927410125732f;
    sPostRange.mMax = 3.1415927410125732f;
    hdPictoInitializeLifetimePost(sPostRange.mLifetime);
    hdPictoRegisterGlobalPost(&sPostRangeRegistration);
    hdPictoInitializeExitBytePost(&sPostRange.mLifetime[1]);
    hdPictoRegisterGlobalPost(&sPostLifetimeRegistration);
    sPostPlaceLabels.mLabels[0].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[0].mText.v = gabi::guest_addr("Topic_Tag_place00");
    sPostPlaceLabels.mLabels[1].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[1].mText.v = gabi::guest_addr("Topic_Tag_place01");
    sPostPlaceLabels.mLabels[2].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[2].mText.v = gabi::guest_addr("Topic_Tag_place02");
    sPostPlaceLabels.mLabels[3].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[3].mText.v = gabi::guest_addr("Topic_Tag_place03");
    sPostPlaceLabels.mLabels[4].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[4].mText.v = gabi::guest_addr("Topic_Tag_place04");
    sPostPlaceLabels.mLabels[5].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[5].mText.v = gabi::guest_addr("Topic_Tag_place05");
    sPostPlaceLabels.mLabels[6].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[6].mText.v = gabi::guest_addr("Topic_Tag_place06");
    sPostPlaceLabels.mLabels[7].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[7].mText.v = gabi::guest_addr("Topic_Tag_place07");
    sPostPlaceLabels.mLabels[8].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[8].mText.v = gabi::guest_addr("Topic_Tag_place08");
    sPostPlaceLabels.mLabels[9].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[9].mText.v = gabi::guest_addr("Topic_Tag_place09");
    sPostPlaceLabels.mLabels[10].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[10].mText.v = gabi::guest_addr("Topic_Tag_place10");
    sPostPlaceLabels.mLabels[11].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[11].mText.v = gabi::guest_addr("Topic_Tag_place11");
    sPostPlaceLabels.mLabels[12].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[12].mText.v = gabi::guest_addr("Topic_Tag_place12");
    sPostPlaceLabels.mLabels[13].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[13].mText.v = gabi::guest_addr("Topic_Tag_place13");
    sPostPlaceLabels.mLabels[14].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[14].mText.v = gabi::guest_addr("Topic_Tag_place14");
    sPostPlaceLabels.mLabels[15].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[15].mText.v = gabi::guest_addr("Topic_Tag_place15");
    sPostPlaceLabels.mLabels[16].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[16].mText.v = gabi::guest_addr("Topic_Tag_place16");
    sPostPlaceLabels.mLabels[17].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[17].mText.v = gabi::guest_addr("Topic_Tag_place17");
    sPostPlaceLabels.mLabels[18].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[18].mText.v = gabi::guest_addr("Topic_Tag_place18");
    sPostPlaceLabels.mLabels[19].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[19].mText.v = gabi::guest_addr("Topic_Tag_place19");
    sPostPlaceLabels.mLabels[20].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[20].mText.v = gabi::guest_addr("Topic_Tag_place20");
    sPostPlaceLabels.mLabels[21].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[21].mText.v = gabi::guest_addr("Topic_Tag_place21");
    sPostPlaceLabels.mLabels[22].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[22].mText.v = gabi::guest_addr("Topic_Tag_place22");
    sPostPlaceLabels.mLabels[23].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[23].mText.v = gabi::guest_addr("Topic_Tag_place23");
    sPostPlaceLabels.mLabels[24].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[24].mText.v = gabi::guest_addr("Topic_Tag_place24");
    sPostPlaceLabels.mLabels[25].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[25].mText.v = gabi::guest_addr("Topic_Tag_place25");
    sPostPlaceLabels.mLabels[26].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[26].mText.v = gabi::guest_addr("Topic_Tag_place26");
    sPostPlaceLabels.mLabels[27].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[27].mText.v = gabi::guest_addr("Topic_Tag_place27");
    sPostPlaceLabels.mLabels[28].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[28].mText.v = gabi::guest_addr("Topic_Tag_place28");
    sPostPlaceLabels.mLabels[29].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[29].mText.v = gabi::guest_addr("Topic_Tag_place29");
    sPostPlaceLabels.mLabels[30].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[30].mText.v = gabi::guest_addr("Topic_Tag_place30");
    sPostPlaceLabels.mLabels[31].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[31].mText.v = gabi::guest_addr("Topic_Tag_place31");
    sPostPlaceLabels.mLabels[32].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[32].mText.v = gabi::guest_addr("Topic_Tag_place32");
    sPostPlaceLabels.mLabels[33].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[33].mText.v = gabi::guest_addr("Topic_Tag_place33");
    sPostPlaceLabels.mLabels[34].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[34].mText.v = gabi::guest_addr("Topic_Tag_place34");
    sPostPlaceLabels.mLabels[35].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[35].mText.v = gabi::guest_addr("Topic_Tag_place35");
    sPostPlaceLabels.mLabels[36].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[36].mText.v = gabi::guest_addr("Topic_Tag_place36");
    sPostPlaceLabels.mLabels[37].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[37].mText.v = gabi::guest_addr("Topic_Tag_place37");
    sPostPlaceLabels.mLabels[38].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[38].mText.v = gabi::guest_addr("Topic_Tag_place38");
    sPostPlaceLabels.mLabels[39].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[39].mText.v = gabi::guest_addr("Topic_Tag_place39");
    sPostPlaceLabels.mLabels[40].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[40].mText.v = gabi::guest_addr("Topic_Tag_place40");
    sPostPlaceLabels.mLabels[41].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[41].mText.v = gabi::guest_addr("Topic_Tag_place41");
    sPostPlaceLabels.mLabels[42].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[42].mText.v = gabi::guest_addr("Topic_Tag_place42");
    sPostPlaceLabels.mLabels[43].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[43].mText.v = gabi::guest_addr("Topic_Tag_place43");
    sPostPlaceLabels.mLabels[44].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[44].mText.v = gabi::guest_addr("Topic_Tag_place44");
    sPostPlaceLabels.mLabels[45].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[45].mText.v = gabi::guest_addr("Topic_Tag_place45");
    sPostPlaceLabels.mLabels[46].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[46].mText.v = gabi::guest_addr("Topic_Tag_place46");
    sPostPlaceLabels.mLabels[47].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[47].mText.v = gabi::guest_addr("Topic_Tag_place47");
    sPostPlaceLabels.mLabels[48].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[48].mText.v = gabi::guest_addr("Topic_Tag_place48");
    sPostPlaceLabels.mLabels[49].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[49].mText.v = gabi::guest_addr("Topic_Tag_place49");
    sPostPlaceLabels.mLabels[50].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[50].mText.v = gabi::guest_addr("Topic_Tag_place50");
    sPostPlaceLabels.mLabels[51].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[51].mText.v = gabi::guest_addr("Topic_Tag_place51");
    sPostPlaceLabels.mLabels[52].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[52].mText.v = gabi::guest_addr("Topic_Tag_place52");
    sPostPlaceLabels.mLabels[53].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[53].mText.v = gabi::guest_addr("Topic_Tag_place53");
    sPostPlaceLabels.mLabels[54].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[54].mText.v = gabi::guest_addr("Topic_Tag_place54");
    sPostPlaceLabels.mLabels[55].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[55].mText.v = gabi::guest_addr("Topic_Tag_place55");
    sPostPlaceLabels.mLabels[56].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[56].mText.v = gabi::guest_addr("Topic_Tag_place56");
    sPostPlaceLabels.mLabels[57].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[57].mText.v = gabi::guest_addr("Topic_Tag_place57");
    sPostPlaceLabels.mLabels[58].mVtable = &sPostStringVtablePost;
    sPostPlaceLabels.mLabels[58].mText.v = gabi::guest_addr("Topic_Tag_place58");
    sPostGroupNames.mNames[0].mVtable = &sPostStringVtablePost;
    sPostGroupNames.mNames[0].mText.v = gabi::guest_addr("G_Btn_00");
    sPostGroupNames.mNames[1].mVtable = &sPostStringVtablePost;
    sPostGroupNames.mNames[1].mText.v = gabi::guest_addr("G_Extara_00");
    sPostGroupNames.mNames[2].mVtable = &sPostStringVtablePost;
    sPostGroupNames.mNames[2].mText.v = gabi::guest_addr("G_Save_00");
    sPostSummaryLabels.mLabels[0].mVtable = &sPostStringVtablePost;
    sPostSummaryLabels.mLabels[0].mText.v = gabi::guest_addr("L_SaveData_00-T_Btn_00");
    sPostSummaryLabels.mLabels[1].mVtable = &sPostStringVtablePost;
    sPostSummaryLabels.mLabels[1].mText.v = gabi::guest_addr("L_SaveData_01-T_Btn_00");
    sPostSummaryLabels.mLabels[2].mVtable = &sPostStringVtablePost;
    sPostSummaryLabels.mLabels[2].mText.v = gabi::guest_addr("L_SaveData_02-T_Btn_00");
    sPostSummaryLabels.mLabels[3].mVtable = &sPostStringVtablePost;
    sPostSummaryLabels.mLabels[3].mText.v = gabi::guest_addr("L_SaveData_00-T_Text_00");
    sPostSummaryLabels.mLabels[4].mVtable = &sPostStringVtablePost;
    sPostSummaryLabels.mLabels[4].mText.v = gabi::guest_addr("L_SaveData_00-T_Time_00");
    sPostAnimationNames.mNames[0].mVtable = &sPostStringVtablePost;
    sPostAnimationNames.mNames[0].mText.v = gabi::guest_addr("Extra");
    sPostAnimationNames.mNames[1].mVtable = &sPostStringVtablePost;
    sPostAnimationNames.mNames[1].mText.v = gabi::guest_addr("Save");
    sPostPartName.mVtable = &sPostStringVtablePost;
    sPostPartName.mText.v = gabi::guest_addr("BtnSaveData_00");
}

/* WWHD glue
 ext 027045B0 void hdPostResetPaneMessage(hdTextPane_c*, hdUiSafeString*, hdUiSafeString*)
 str 100EC290 "Topic_Tag_place00"
 str 100EC2A4 "Topic_Tag_place01"
 str 100EC2B8 "Topic_Tag_place02"
 str 100EC2CC "Topic_Tag_place03"
 str 100EC2E0 "Topic_Tag_place04"
 str 100EC2F4 "Topic_Tag_place05"
 str 100EC308 "Topic_Tag_place06"
 str 100EC31C "Topic_Tag_place07"
 str 100EC330 "Topic_Tag_place08"
 str 100EC344 "Topic_Tag_place09"
 str 100EC358 "Topic_Tag_place10"
 str 100EC36C "Topic_Tag_place11"
 str 100EC380 "Topic_Tag_place12"
 str 100EC394 "Topic_Tag_place13"
 str 100EC3A8 "Topic_Tag_place14"
 str 100EC3BC "Topic_Tag_place15"
 str 100EC3D0 "Topic_Tag_place16"
 str 100EC3E4 "Topic_Tag_place17"
 str 100EC3F8 "Topic_Tag_place18"
 str 100EC40C "Topic_Tag_place19"
 str 100EC420 "Topic_Tag_place20"
 str 100EC434 "Topic_Tag_place21"
 str 100EC448 "Topic_Tag_place22"
 str 100EC45C "Topic_Tag_place23"
 str 100EC470 "Topic_Tag_place24"
 str 100EC484 "Topic_Tag_place25"
 str 100EC498 "Topic_Tag_place26"
 str 100EC4AC "Topic_Tag_place27"
 str 100EC4C0 "Topic_Tag_place28"
 str 100EC4D4 "Topic_Tag_place29"
 str 100EC4E8 "Topic_Tag_place30"
 str 100EC4FC "Topic_Tag_place31"
 str 100EC510 "Topic_Tag_place32"
 str 100EC524 "Topic_Tag_place33"
 str 100EC538 "Topic_Tag_place34"
 str 100EC54C "Topic_Tag_place35"
 str 100EC560 "Topic_Tag_place36"
 str 100EC574 "Topic_Tag_place37"
 str 100EC588 "Topic_Tag_place38"
 str 100EC59C "Topic_Tag_place39"
 str 100EC5B0 "Topic_Tag_place40"
 str 100EC5C4 "Topic_Tag_place41"
 str 100EC5D8 "Topic_Tag_place42"
 str 100EC5EC "Topic_Tag_place43"
 str 100EC600 "Topic_Tag_place44"
 str 100EC614 "Topic_Tag_place45"
 str 100EC628 "Topic_Tag_place46"
 str 100EC63C "Topic_Tag_place47"
 str 100EC650 "Topic_Tag_place48"
 str 100EC664 "Topic_Tag_place49"
 str 100EC678 "Topic_Tag_place50"
 str 100EC68C "Topic_Tag_place51"
 str 100EC6A0 "Topic_Tag_place52"
 str 100EC6B4 "Topic_Tag_place53"
 str 100EC6C8 "Topic_Tag_place54"
 str 100EC6DC "Topic_Tag_place55"
 str 100EC6F0 "Topic_Tag_place56"
 str 100EC704 "Topic_Tag_place57"
 str 100EC718 "Topic_Tag_place58"
 str 100EC738 "G_Btn_00" in 026441DC
 str 100EC284 "G_Extara_00"
 str 100EC72C "G_Save_00"
 str 100EC744 "L_SaveData_00-T_Btn_00"
 str 100EC75C "L_SaveData_01-T_Btn_00"
 str 100EC774 "L_SaveData_02-T_Btn_00"
 str 100EC254 "L_SaveData_00-T_Text_00"
 str 100EC26C "L_SaveData_00-T_Time_00"
 str 100EC244 "Extra"
 str 100EC24C "Save"
 str 100EC78C "BtnSaveData_00"
 ext 028F026C void hdPictoRegisterGlobalPost(void*)
 ext 028ED6F8 void hdPictoInitializeLifetimePost(void*)
 ext 028EAB2C void hdPictoInitializeExitBytePost(void*)
 str 100EC1E0 "sea"
 str 100EC1F6 "Si"
 str 100EC1E4 "ADM"
 str 100EC1F9 "ki"
 str 100EC1FC "M_Da"
 str 100EC1E8 "kaz"
 str 100EC1EC "M_N"
 str 100EC204 "M_Dr"
 str 100EC1F0 "G"
 str 100EC1F2 "X"
 str 100EC1F4 "H"
 str 100EC20C "ke"
 str 100EC20F "Ma"
 str 100EC212 "M2"
 str 100EC215 "Mj"
 str 100EC218 "ma"
 str 100EC21C "A_um"
 str 100EC224 "MenuMiiverseContent_00"
 ext 025F65F4 void hdPostConstructWork(PaneWork_l*)
 ext 025F50E8 void hdPostLookupMessage(void*, PaneWork_l*, void*, PaneString_l*)
 ext 0273A2E8 void* hdPostLookupEntry(void*, u32)
 ext 02644720 void hdPostAssureWideString(PaneString_l*)
 ext C0009988 void hdPostCopyBytes(void*, const void*, u32, u32)
 data sPostPlaceLabels 10490B28 nocheck
 data sPostSummaryLabels 10490B00 nocheck
 data sPostGroupNames 10490AD8 nocheck
 data sPostAnimationNames 10490AF0 nocheck
 data sPostPartName 10490D0C nocheck
 data sPostStringVtablePost 100EC0DC nocheck
 data sPostWideStringVtable 100EC0F4 nocheck
 data sPostNameBufferVtable 100EC10C nocheck
 data sPostPlaceBufferVtable 100EC13C nocheck
 data sPostZero 10490AC8 nocheck
 data sPostRange 10490A74 nocheck
 data sPostZeroRegistration 101F578C nocheck
 data sPostRangeRegistration 101F5798 nocheck
 data sPostLifetimeRegistration 101F57A4 nocheck
 data sPostUiSystem 1018C404 nocheck
 data sPostAnimationGroups 100EC0D0
 data sPostMessageManager 101F4AE8 nocheck
 ext 02726FC4 void hdPictureSlotSummaries_c::refresh()
 ext 027271F0 hdPictureSlotSummary_c* hdPictureSlotSummaries_c::slot(u32)
 ext C000A180 void hdPostTicksToCalendar(u32, u32, hdPostCalendar_c*)
 ext 025F6D70 void hdPostResetWork(PaneWork_l*, hdUiSafeString*, hdUiSafeString*)
 ext 02704504 void hdTextPane_c::prepareMessage(PaneWork_l*, u32)
 ext 027046D4 void hdTextPane_c::setStringPrepared(PaneString_l*, PaneWork_l*)



 ext 02004E04 void hdPostBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*)
*/
