/* HD chart-button animation states and selection helpers. */
#include "d/hd_btn_map_icon_extra.h"
extern void mapIconPlay(hdAnimSet*,u32,u32,f32);
extern void mapIconStop(hdAnimSet*,u32,u32);
extern void mapIconReset(hdAnimSet*,u32,u32);
extern void mapIconSetFrame(hdAnimSet*,u32,u32,f32);
extern u32 mapIconAnimationEnded(hdAnimSet*,u32);
extern void mapIconSound(const char*);
extern void mapIconBaseDestroy(hdBasicButton_c*,u32);
extern void mapIconDelete(void*);
struct MapIconVtableWords_c { be<u32> mWords[2]; };
static MapIconVtableWords_c sMapIconStateVtable;
static MapIconVtableWords_c sMapIconOwnerVtable;
struct hdMapReceiverState_c { u8 mPrefix[0x18]; be<u32> mEnabled; };
static hdPartsStateId_c sMapIconPrimaryEndState;
static hdPartsStateId_c sMapIconSecondaryEndState;
static hdPartsStateId_c sMapIconClosePrimaryState;
static hdPartsStateId_c sMapIconCloseSecondaryState;
/* 02636EF8 */
void hdBtnMapIcon_c::destroy(hdBtnMapIcon_c* icon,u32 flags) {
    if (!icon) return;
    icon->mButton.mStateHolder.mHolderVtable = sMapIconStateVtable.mWords;
    icon->mButton.mSecondaryVtable = sMapIconOwnerVtable.mWords;
    mapIconBaseDestroy(icon,0);
    if (flags&1) mapIconDelete(icon);
}
/* 02636F64 */
void hdBtnMapIcon_c::zoomWaitEnter() {
    mapIconPlay(mButton.animSet(),7,2,1.0f);
}
/* 02636F90 */
void hdBtnMapIcon_c::zoomInEnter() {
    mapIconPlay(mButton.animSet(),8,5,1.0f);
    mapIconSound("SE_MENU_ZOOM_IN");
}
/* 02636FE4 */
void hdBtnMapIcon_c::zoomOutEnter() {
    mapIconPlay(mButton.animSet(),9,5,1.0f);
    mapIconSound("SE_MENU_ZOOM_IN");
}
/* 02637038 */
void hdBtnMapIcon_c::islandEnter() {
    mapIconPlay(mButton.animSet(),10,6,1.0f);
}
/* 02637064 */
void hdBtnMapIcon_c::treasureEnter() {
    mapIconPlay(mButton.animSet(),11,5,1.0f);
}
/* 02637090 */
void hdBtnMapIcon_c::shipEnter() {
    mapIconPlay(mButton.animSet(),12,5,1.0f);
}
/* 026370BC */
void hdBtnMapIcon_c::fishEnter() {
    mapIconPlay(mButton.animSet(),13,5,1.0f);
}
/* 026370E8 */
void hdBtnMapIcon_c::mapPatternEnter() {
    mapIconPlay(mButton.animSet(),14,8,1.0f);
}
/* 02639C28 */
void hdBtnMapIcon_c::revealIconEnter() {
    mapIconPlay(mButton.animSet(),14,5,1.0f);
}
/* 02636F80 */
void hdBtnMapIcon_c::zoomWaitMove() { mButton.vCalcLayout(); }
/* 02636FD4 */
void hdBtnMapIcon_c::zoomInMove() { mButton.vCalcLayout(); }
/* 02637028 */
void hdBtnMapIcon_c::zoomOutMove() { mButton.vCalcLayout(); }
/* 02637054 */
void hdBtnMapIcon_c::islandMove() { mButton.vCalcLayout(); }
/* 02637080 */
void hdBtnMapIcon_c::treasureMove() { mButton.vCalcLayout(); }
/* 026370AC */
void hdBtnMapIcon_c::shipMove() { mButton.vCalcLayout(); }
/* 026370D8 */
void hdBtnMapIcon_c::fishMove() { mButton.vCalcLayout(); }
/* 02637104 */
void hdBtnMapIcon_c::mapPatternMove() { mButton.vCalcLayout(); }
/* 026376D0 */
void hdBtnMapIcon_c::finishMapSelectionCalc() { mButton.vCalcLayout(); }
/* 02637114 */
void hdBtnMapIcon_c::enableReceiver(u32 enabled) {
    if (enabled && mButton.mpReceiver)
        reinterpret_cast<hdMapReceiverState_c*>(mButton.mpReceiver.get())->mEnabled = enabled;
}
/* 02637130 */
void hdBtnMapIcon_c::showIsland(hdMapIconSelection_c* selection) {
    if (!selection) return;
    u32 island=selection->mIsland;
    if (island>=49) return;
    if (mMapMode==1) mapIconStop(mButton.animSet(),8,5);
    mapIconSetFrame(mButton.animSet(),10,6,f32(s32(island)));
}
/* 026371CC */
void hdBtnMapIcon_c::hideIsland() {
    mapIconReset(mButton.animSet(),8,5);
    mapIconStop(mButton.animSet(),10,6);
}
/* 0263721C */
void hdBtnMapIcon_c::updateIsland(hdMapIconSelection_c* selection) {
    if (!selection) return;
    if (selection->mKnown) showIsland(selection); else hideIsland();
}
/* 02639C44 */
u32 hdBtnMapIcon_c::revealIconEnded() { return mapIconAnimationEnded(mButton.animSet(),14); }
/* 02639FB4 */
void hdBtnMapIcon_c::setShipVisible(u32 visible) {
    HdMapIconPane* primary=mpPrimaryPane;
    HdMapIconPane* secondary=mpSecondaryPane;
    if (!primary || !secondary) return;
    primary->mFlags=(primary->mFlags&0xFE)|visible;
    secondary->mFlags=(secondary->mFlags&0xFE)|visible;
}
/* 02639FF0 */
void hdBtnMapIcon_c::setTreasureVisible(u32 visible) {
    HdMapIconPane* pane=mpTreasurePane;
    if (pane) pane->mFlags=(pane->mFlags&0xFE)|visible;
}
/* 0263A010 */
void hdBtnMapIcon_c::setClipBounds(hdMapIconBounds_c* bounds) { mpClipBounds=bounds; }
/* 0263A38C */
void hdBtnMapIcon_c::closePrimary() { mButton.changeState(&sMapIconClosePrimaryState); }
/* 0263A39C */
void hdBtnMapIcon_c::closeSecondary() { mButton.changeState(&sMapIconCloseSecondaryState); }
struct hdMapActiveSelection_c { u8 mPrefix[0x10]; be<u32> mButtonId; };
struct hdMapSelectionManager_c;
struct hdMapSelectionManagerPointer_c { gptr<hdMapSelectionManager_c> mpManager; };
static hdMapSelectionManagerPointer_c sMapSelectionManager;
extern hdMapActiveSelection_c* mapIconGetActiveSelection(hdMapSelectionManager_c*);
extern u32 mapIconGetButtonId(hdBtnMapIcon_c*);
extern void mapIconSetActiveSelection(hdMapSelectionManager_c*,u32);
struct hdMapIconPaneWidth_c { u8 mPrefix[0x3C]; be<f32> mWidth; };
/* 02637238 */
void hdBtnMapIcon_c::updateOrigin(hdMapIconSelection_c* selection) {
    if (!selection) return;
    HdMapInfo* info=reinterpret_cast<HdMapInfo*>(selection);
    mOrigin.x=(f32)(s32)info->mOffsetX;
    mOrigin.y=(f32)(s32)info->mOffsetY;
    HdMapIconPane* centered=mpCenteredPane;
    HdMapIconPane* origin=mpOriginPane;
    HdMapIconPane* width=mpWidthPane;
    HdMapIconPane* parent=mpParentPane;
    mOriginOffset.x=0.0f;
    mOriginOffset.y=0.0f;
    mPosition=mOrigin;
    if (!origin || !centered || !width || !parent) return;
    f32 x=(f32)mOrigin.x*(f32)mTranslateScale.x;
    f32 y=-((f32)mOrigin.y*(f32)mTranslateScale.y);
    parent->mTranslateX=x;
    parent->mTranslateY=y;
    parent->mTranslateZ=0.0f;
    parent->mFlags=parent->mFlags|0x10;
    origin->mTranslateY=y;
    origin->mTranslateX=x;
    origin->mFlags=origin->mFlags|0x10;
    origin->mTranslateZ=0.0f;
    u32 flags=centered->mFlags;
    f32 centeredX=gabi::fnmsubs(reinterpret_cast<hdMapIconPaneWidth_c*>(width)->mWidth,0.5f,x);
    centered->mTranslateY=y;
    centered->mFlags=flags|0x10;
    centered->mTranslateZ=0.0f;
    centered->mTranslateX=centeredX;
}
/* 02637358 */
void hdBtnMapIcon_c::updateSelection(hdMapIconSelection_c* selection) {
    if (selection) {
        updateIsland(selection);
        updateOrigin(selection);
    }
}
/* 026373A8 */
void hdBtnMapIcon_c::preparePrimary() {
    hdMapIconSelection_c* selection=reinterpret_cast<hdMapIconSelection_c*>((HdMapInfo*)mpOwner->mpInfo);
    if (!selection) return;
    hdMapActiveSelection_c* active=mapIconGetActiveSelection(sMapSelectionManager.mpManager);
    if (active && active->mButtonId==mapIconGetButtonId(this))
        mapIconSetActiveSelection(sMapSelectionManager.mpManager,0);
    enableReceiver(mpReceiverGatePane.v.get());
    mMapMode=4;
    mButton.mTouchEnabled=0;
    mapIconReset(mButton.animSet(),8,5);
    mapIconReset(mButton.animSet(),0,0);
    updateSelection(selection);
}
/* 02637528 */
void hdBtnMapIcon_c::prepareSecondary() {
    hdMapActiveSelection_c* active=mapIconGetActiveSelection(sMapSelectionManager.mpManager);
    if (active && active->mButtonId==mapIconGetButtonId(this))
        mapIconSetActiveSelection(sMapSelectionManager.mpManager,0);
    enableReceiver(mpReceiverGatePane.v.get());
    mButton.mTouchEnabled=0;
}
/* 0263747C */
void hdBtnMapIcon_c::showPrimaryEnter() {
    preparePrimary();
    mapIconPlay(mButton.animSet(),15,4,1.0f);
}
/* 026374C4 */
void hdBtnMapIcon_c::showPrimaryMove() {
    if (mapIconAnimationEnded(mButton.animSet(),15))
        mButton.changeState(&sMapIconPrimaryEndState);
    mButton.vCalcLayout();
}
/* 026375A8 */
void hdBtnMapIcon_c::showSecondaryEnter() {
    prepareSecondary();
    mapIconPlay(mButton.animSet(),16,4,1.0f);
}
/* 026375F0 */
void hdBtnMapIcon_c::showSecondaryMove() {
    if (mapIconAnimationEnded(mButton.animSet(),16))
        mButton.changeState(&sMapIconSecondaryEndState);
    mButton.vCalcLayout();
}
struct hdMapSelectedItem_c { u8 mPrefix[0x9C]; be<u32> mSelection; };
struct hdMapSelectionOwner_c { u8 mPrefix[0x120]; gptr<hdMapSelectedItem_c> mpSelectedItem; };
/* 02637654 */
void hdBtnMapIcon_c::finishMapSelection() {
    if (mMapMode==3) {
        hdMapSelectedItem_c* item=reinterpret_cast<hdMapSelectionOwner_c*>((HdMapIconOwner*)mpOwner)->mpSelectedItem;
        if (item) mapIconSetActiveSelection(sMapSelectionManager.mpManager,item->mSelection);
    }
    mMapMode=4;
}
/* 026376B0 */
void hdBtnMapIcon_c::finishMapSelectionMove() { finishMapSelection(); }
struct hdMapPanePoint_c { be<f32> x,y,z; };
extern void mapIconPanePosition(hdMapPanePoint_c*,HdMapIconPane*);
extern f32 mapIconChasePoint(HdMapPoint*,HdMapPoint*,f32,f32,f32);
/* 02639D8C */
void hdBtnMapIcon_c::captureTranslations() {
    HdMapIconPane* text=mpTextPane;
    HdMapIconPane* bounds=mpBoundsPane;
    HdMapIconPane* root=reinterpret_cast<HdMapIconPane*>((hdPartsPane_c*)mButton.mpLayout->lyt()->mpRootPane);
    if (!text || !bounds || !root) return;
    hdMapPanePoint_c textPoint;
    hdMapPanePoint_c boundsPoint;
    mapIconPanePosition(&textPoint,text);
    mapIconPanePosition(&boundsPoint,bounds);
    f32 dx=(f32)textPoint.x-(f32)boundsPoint.x;
    f32 dy=(f32)textPoint.y-(f32)boundsPoint.y;
    f32 x=root->mTranslateX;
    f32 y=root->mTranslateY;
    mOriginalTranslation.x=x;
    mCenteredTranslation.x=x-dx;
    mOriginalTranslation.y=y;
    mSavedTranslation=mOriginalTranslation;
    mCenteredTranslation.y=y-dy;
}
/* 02639E50 */
void hdBtnMapIcon_c::moveToCenteredTranslation() {
    HdMapIconPane* root=reinterpret_cast<HdMapIconPane*>((hdPartsPane_c*)mButton.mpLayout->lyt()->mpRootPane);
    if (!root) return;
    mapIconChasePoint(&mSavedTranslation,&mCenteredTranslation,0.5f,50.0f,5.0f);
    f32 x=mSavedTranslation.x;
    f32 y=mSavedTranslation.y;
    u32 flags=root->mFlags;
    root->mTranslateZ=0.0f;
    root->mTranslateY=y;
    root->mTranslateX=x;
    root->mFlags=flags|0x10;
}
/* 02639EE0 */
void hdBtnMapIcon_c::restoreTranslation() {
    HdMapIconPane* root=reinterpret_cast<HdMapIconPane*>((hdPartsPane_c*)mButton.mpLayout->lyt()->mpRootPane);
    if (!root) return;
    mapIconChasePoint(&mSavedTranslation,&mOriginalTranslation,0.5f,50.0f,5.0f);
    f32 x=mSavedTranslation.x;
    f32 y=mSavedTranslation.y;
    u32 flags=root->mFlags;
    root->mTranslateZ=0.0f;
    root->mTranslateY=y;
    root->mTranslateX=x;
    root->mFlags=flags|0x10;
}
/* 026376B4 */
void hdBtnMapIcon_c::finishMapSelectionEnter() { mapIconPlay(mButton.animSet(),17,4,1.0f); }
/* 02638060 */
void hdBtnMapIcon_c::setMapReady() { mMapMode=1; }
/* 0263806C */
void hdBtnMapIcon_c::enableMapSelection() { mMapMode=3; enableReceiver(mpSelectionGatePane.v.get()); mButton.mTouchEnabled=1; }
/* 02638144 */
void hdBtnMapIcon_c::showPrimaryImmediately() { preparePrimary(); mapIconStop(mButton.animSet(),15,4); mButton.changeState(&sMapIconPrimaryEndState); }
/* 02638194 */
void hdBtnMapIcon_c::showFish() { mapIconStop(mButton.animSet(),13,7); mFishVisible=1; }
/* 026381D8 */
void hdBtnMapIcon_c::hideFish() { mapIconReset(mButton.animSet(),13,7); mFishVisible=0; }
/* 0263A3AC */
void hdBtnMapIcon_c::showSecondaryImmediately() { prepareSecondary(); finishMapSelection(); mapIconStop(mButton.animSet(),16,4); mButton.changeState(&sMapIconSecondaryEndState); }
/* 0263A404 */
void hdBtnMapIcon_c::showPrimaryIdle() { showPrimaryImmediately(); mMapMode=0; }
/* 0263A438 */
void hdBtnMapIcon_c::zoomPrimaryIn() { showPrimaryIdle(); mapIconPlay(mButton.animSet(),8,5,1.0f); mapIconSound("SE_MENU_ZOOM_IN"); }
/* 0263A48C */
void hdBtnMapIcon_c::zoomPrimaryImmediately() { showPrimaryIdle(); setMapReady(); mapIconStop(mButton.animSet(),8,5); }
/* 0263A4D4 */
void hdBtnMapIcon_c::selectSecondaryImmediately() { zoomPrimaryImmediately(); mMapMode=2; captureTranslations(); mapIconSetActiveSelection(sMapSelectionManager.mpManager,mButton.mpReceiver.v.get()); }
/* 0263A520 */
void hdBtnMapIcon_c::zoomSecondaryOut() { selectSecondaryImmediately(); mapIconPlay(mButton.animSet(),9,5,1.0f); mapIconSound("SE_MENU_ZOOM_IN"); }
/* 02639E4C */
void hdBtnMapIcon_c::captureTranslationsMove() { captureTranslations(); }
/* 02639F70 */
void hdBtnMapIcon_c::showIslandAtOrigin(hdMapIconSelection_c* selection) {
    showIsland(selection);
    updateOrigin(selection);
}
extern hdPartsLayout_c* mapIconAllocateLayout(u32,void*,u32);
extern void mapIconBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*);
static hdMapLayoutHeapPointer_c sMapMainLayoutHeap;
struct sMapMainLayoutIndices_c { be<u32> mEntries[11]; };
static sMapMainLayoutIndices_c sMapMainLayoutIndices;
struct sMapMainLayoutGroups_c { hdUiSafeString mEntries[11]; };
static sMapMainLayoutGroups_c sMapMainLayoutGroups;
struct sMapMainLayoutAnimations_c { hdUiSafeString mEntries[6]; };
static sMapMainLayoutAnimations_c sMapMainLayoutAnimations;
/* 02639B10 */
u32 hdBtnMapIcon_c::createLayout(void* name,u32 first,u32 second,u32 third,void* descriptor) {
    hdPartsLayout_c* layout=mapIconAllocateLayout(sizeof(hdPartsLayout_c),sMapMainLayoutHeap.mpContext->mpHeap,4);
    if (layout) layout=hdPartsLayout_c::construct(layout);
    mButton.mpLayout=layout;
    if (!layout || !layout->vLoad(name,first,second,third,18,9,descriptor)) return 0;
    for (u32 i=0;i<11;++i)
        mapIconBindAnimation(mButton.animSet(),i+7,&sMapMainLayoutGroups.mEntries[i],&sMapMainLayoutAnimations.mEntries[sMapMainLayoutIndices.mEntries[i]]);
    return 1;
}
struct hdMapIconPaneSize_c { u8 mPrefix[0x3C]; be<f32> mWidth,mHeight; };
/* 02637E68 */
void hdBtnMapIcon_c::updateTranslateScale() {
    hdMapIconPaneSize_c* pane=reinterpret_cast<hdMapIconPaneSize_c*>((HdMapIconPane*)mpTextPane);
    if (pane) {
        f32 width=pane->mWidth;
        f32 height=pane->mHeight;
        mTranslateScale.x=width/100000.0f;
        mTranslateScale.y=height/100000.0f;
    } else { mTranslateScale.x=1.0f; mTranslateScale.y=1.0f; }
}
/* 02637EAC */
void hdBtnMapIcon_c::updatePointScale() {
    hdMapIconPaneSize_c* pane=reinterpret_cast<hdMapIconPaneSize_c*>((HdMapIconPane*)mpTextPane);
    if (pane) {
        f32 width=pane->mWidth;
        f32 height=pane->mHeight;
        mPointScale.x=width/20000.0f;
        mPointScale.y=height/20000.0f;
    } else { mPointScale.x=1.0f; mPointScale.y=1.0f; }
}
/* 02639C54 */
void hdBtnMapIcon_c::moveOriginToOffset() {
    HdMapIconPane* parent=mpParentPane;
    if (!parent) return;
    mapIconChasePoint(&mPosition,&mOriginOffset,0.5f,5000.0f,500.0f);
    f32 x=(f32)mPosition.x*(f32)mTranslateScale.x;
    f32 y=(f32)mPosition.y*(f32)mTranslateScale.y;
    u32 flags=parent->mFlags;
    parent->mTranslateX=x;
    parent->mFlags=flags|0x10;
    parent->mTranslateZ=0.0f;
    parent->mTranslateY=-y;
}
/* 02639CF0 */
void hdBtnMapIcon_c::restoreOrigin() {
    HdMapIconPane* parent=mpParentPane;
    if (!parent) return;
    mapIconChasePoint(&mPosition,&mOrigin,0.5f,5000.0f,500.0f);
    f32 x=(f32)mPosition.x*(f32)mTranslateScale.x;
    f32 y=(f32)mPosition.y*(f32)mTranslateScale.y;
    u32 flags=parent->mFlags;
    parent->mTranslateX=x;
    parent->mFlags=flags|0x10;
    parent->mTranslateZ=0.0f;
    parent->mTranslateY=-y;
}
static HdMapIconPane* findMapIconPane(hdPartsLayout_c* layout,hdUiSafeString* name) {
    hdPartsLyt_c* lyt=layout->lyt();
    name->assureTermination();
    hdScreenResourceObject* root=reinterpret_cast<hdScreenResourceObject*>((hdPartsPane_c*)lyt->mpRootPane);
    return reinterpret_cast<HdMapIconPane*>(root->findPane(reinterpret_cast<const char*>(name->mText.get())));
}
struct sMapFindTransformPanesNames_c { hdUiSafeString mNames[3]; };
static sMapFindTransformPanesNames_c sMapFindTransformPanesNames;
/* 026379D0 */
void hdBtnMapIcon_c::findTransformPanes() {
    mpScrollPane=findMapIconPane(mButton.mpLayout,&sMapFindTransformPanesNames.mNames[0]);
    mpTextPane=findMapIconPane(mButton.mpLayout,&sMapFindTransformPanesNames.mNames[1]);
    mpParentPane=findMapIconPane(mButton.mpLayout,&sMapFindTransformPanesNames.mNames[2]);
}
struct sMapFindIconPanesNames_c { hdUiSafeString mNames[5]; };
static sMapFindIconPanesNames_c sMapFindIconPanesNames;
/* 02637A84 */
void hdBtnMapIcon_c::findIconPanes() {
    mpSecondaryPane=findMapIconPane(mButton.mpLayout,&sMapFindIconPanesNames.mNames[0]);
    mpOriginPane=findMapIconPane(mButton.mpLayout,&sMapFindIconPanesNames.mNames[1]);
    mpCenteredPane=findMapIconPane(mButton.mpLayout,&sMapFindIconPanesNames.mNames[2]);
    mpWidthPane=findMapIconPane(mButton.mpLayout,&sMapFindIconPanesNames.mNames[3]);
    mpTreasurePane=findMapIconPane(mButton.mpLayout,&sMapFindIconPanesNames.mNames[4]);
}
struct sMapFindReceiverGatePanesNames_c { hdUiSafeString mNames[2]; };
static sMapFindReceiverGatePanesNames_c sMapFindReceiverGatePanesNames;
/* 02637BB4 */
void hdBtnMapIcon_c::findReceiverGatePanes() {
    mpReceiverGatePane=findMapIconPane(mButton.mpLayout,&sMapFindReceiverGatePanesNames.mNames[0]);
    mpSelectionGatePane=findMapIconPane(mButton.mpLayout,&sMapFindReceiverGatePanesNames.mNames[1]);
}
static hdUiSafeString sMapFindContentBoundsPaneName;
/* 02637B38 */
void hdBtnMapIcon_c::findContentBoundsPane() { mpContentBoundsPane=findMapIconPane(mButton.mpLayout,&sMapFindContentBoundsPaneName); }
static hdUiSafeString sMapFindPrimaryPaneName;
/* 02637C68 */
void hdBtnMapIcon_c::findPrimaryPane() { mpPrimaryPane=findMapIconPane(mButton.mpLayout,&sMapFindPrimaryPaneName); }
/* 026380A0 */
void hdBtnMapIcon_c::updateZoomAnimation() {
    if (mMapMode==0) {
        if (mapIconAnimationEnded(mButton.animSet(),8)) setMapReady();
    } else if (mMapMode==2) {
        if (mapIconAnimationEnded(mButton.animSet(),9)) enableMapSelection();
    }
}
/* 02638378 */
void hdBtnMapIcon_c::revealFish() { mapIconPlay(mButton.animSet(),13,7,1.0f); mFishVisible=1; }
static hdUiSafeString sMapNumberGroup;
static hdUiSafeString sMapNumberName;
extern HdMapIconPane* mapIconFindNumberPane(hdPartsLayout_c*,hdUiSafeString*,hdUiSafeString*);
/* 02637CE4 */
void hdBtnMapIcon_c::findNumberPane() {
    mpNumberPane=mapIconFindNumberPane(mButton.mpLayout,&sMapNumberGroup,&sMapNumberName);
}
/* 0263A018 */
void hdBtnMapIcon_c::updatePaneBounds() {
    HdMapIconPane* pane=mpBoundsPane;
    if (!pane) {
        mPaneBounds.mBottom=0.0f;
        mPaneBounds.mLeft=0.0f;
        mPaneBounds.mRight=0.0f;
        mPaneBounds.mTop=0.0f;
        return;
    }
    hdMapIconPaneSize_c* size=reinterpret_cast<hdMapIconPaneSize_c*>(pane);
    f32 halfHeight=(f32)size->mHeight*0.5f;
    f32 halfWidth=(f32)size->mWidth*0.5f;
    f32 x=pane->mTranslateX;
    f32 y=pane->mTranslateY;
    f32 left=x-halfWidth,right=x+halfWidth;
    f32 bottom=y-halfHeight,top=y+halfHeight;
    if (left<right) { mPaneBounds.mRight=right; mPaneBounds.mLeft=left; }
    else { mPaneBounds.mRight=left; mPaneBounds.mLeft=right; }
    if (bottom<top) { mPaneBounds.mBottom=bottom; mPaneBounds.mTop=top; }
    else { mPaneBounds.mBottom=top; mPaneBounds.mTop=bottom; }
}
/* 0263879C */
void hdBtnMapIcon_c::updateSelectionTranslation() {
    HdMapIconPane* pane=reinterpret_cast<HdMapIconPane*>((hdPartsPane_c*)mButton.mpLayout->lyt()->mpRootPane);
    if (!pane) return;
    if (mMapMode==2) {
        mapIconChasePoint(&mSavedTranslation,&mCenteredTranslation,0.7f,50.0f,5.0f);
        pane->mFlags=pane->mFlags|0x10;
        pane->mTranslateY=mSavedTranslation.y;
        pane->mTranslateZ=0.0f;
        pane->mTranslateX=mSavedTranslation.x;
    } else if (mMapMode==3) {
        pane->mTranslateZ=0.0f;
        pane->mTranslateY=mCenteredTranslation.y;
        pane->mFlags=pane->mFlags|0x10;
        pane->mTranslateX=mCenteredTranslation.x;
    }
}
/* 0263887C */
void hdBtnMapIcon_c::updateOriginTranslation() {
    HdMapIconPane* pane=mpParentPane;
    if (!pane) return;
    if (mMapMode==0) {
        mapIconChasePoint(&mPosition,&mOriginOffset,0.7f,5000.0f,500.0f);
        f32 x=(f32)mPosition.x*(f32)mTranslateScale.x;
        f32 y=(f32)mPosition.y*(f32)mTranslateScale.y;
        u32 flags=pane->mFlags;
        pane->mTranslateX=x;
        pane->mTranslateZ=0.0f;
        pane->mFlags=flags|0x10;
        pane->mTranslateY=-y;
    } else if ((u32)mMapMode-1<=2) {
        pane->mTranslateZ=0.0f;
        pane->mTranslateY=mOriginOffset.y;
        pane->mFlags=pane->mFlags|0x10;
        pane->mTranslateX=mOriginOffset.x;
    }
}
static HdMapPoint sMapIconExtents;
/* 02638DE8 */
u32 hdBtnMapIcon_c::pointInsideMap(HdMapPoint* point) {
    f32 x=sMapIconExtents.x,y=sMapIconExtents.y;
    f32 left=-x,right=x,bottom=-y,top=y;
    if (!(left<right)) { left=x; right=-x; }
    if (!(bottom<top)) { bottom=y; top=-y; }
    f32 px=point->x,py=point->y;
    return !(left>px || px>right || bottom>py || py>top);
}
struct hdMapChartFlags_c { u8 mPrefix[0x5CD8]; be<u32> mFlags; };
extern hdMapChartFlags_c* mapIconChartFlags();
/* 02638FA8 */
u32 hdBtnMapIcon_c::isAlternateChart() { return (mapIconChartFlags()->mFlags&0x10000)!=0; }
/* 0263A2BC */
void hdBtnMapIcon_c::updateScrollTransforms() {
    HdMapIconPane* root=reinterpret_cast<HdMapIconPane*>((hdPartsPane_c*)mButton.mpLayout->lyt()->mpRootPane);
    if (root) {
        f32 x=mScrollPosition.x,y=mScrollPosition.y;
        root->mTranslateZ=0.0f;
        u32 flags=root->mFlags;
        root->mTranslateX=x;
        root->mTranslateY=y;
        root->mFlags=flags|0x10;
    }
    HdMapIconPane* scroll=mpScrollPane;
    if (scroll) {
        f32 x=mScrollOffset;
        u32 flags=scroll->mFlags;
        scroll->mTranslateX=x;
        scroll->mTranslateY=0.0f;
        scroll->mFlags=flags|0x10;
        scroll->mTranslateZ=0.0f;
    }
    mapIconSetFrame(mButton.animSet(),7,2,mScrollUp?1.0f:0.0f);
}
static MapIconVtableWords_c sMapIconVtable;
extern void* mapPatternAllocate(u32);
extern hdBasicButton_c* mapPatternBasicConstruct(hdBasicButton_c*);
template<class T> static T* mapIconEmbedded(hdBtnMapIcon_c* icon,u32 offset) {
    return gabi::at<T>(gabi::ea(icon)+offset);
}
static void initializeMapIconPoint(HdMapPoint* point,f32 value) {
    if (!point) point=(HdMapPoint*)mapPatternAllocate(sizeof(HdMapPoint));
    if (point) { point->y=value; point->x=value; }
}
static void initializeMapIconBounds(hdMapIconBounds_c* bounds) {
    if (!bounds) bounds=(hdMapIconBounds_c*)mapPatternAllocate(sizeof(hdMapIconBounds_c));
    if (bounds) {
        bounds->mBottom=3.4028234663852886e38f;
        bounds->mRight=-3.4028234663852886e38f;
        bounds->mLeft=3.4028234663852886e38f;
        bounds->mTop=-3.4028234663852886e38f;
    }
}
/* 026376E0 */
hdBtnMapIcon_c* hdBtnMapIcon_c::construct(hdBtnMapIcon_c* icon) {
    if (!icon) icon=(hdBtnMapIcon_c*)mapPatternAllocate(sizeof(hdBtnMapIcon_c));
    if (!icon) return icon;
    mapPatternBasicConstruct(icon);
    icon->mButton.mVtable=reinterpret_cast<hdPartsVtbl_c*>(sMapIconVtable.mWords);
    icon->mButton.mSecondaryVtable=sMapIconOwnerVtable.mWords;
    icon->mButton.mStateHolder.mHolderVtable=sMapIconStateVtable.mWords;
    // HD: embedded containers retain the original allocation side effects at wrapped guest NULL.
    if (!mapIconEmbedded<u8>(icon,offsetof(hdBtnMapIcon_c,mpScrollPane))) mapPatternAllocate(12);
    if (!mapIconEmbedded<u8>(icon,offsetof(hdBtnMapIcon_c,mpSecondaryPane))) mapPatternAllocate(20);
    if (!mapIconEmbedded<u8>(icon,offsetof(hdBtnMapIcon_c,mpContentBoundsPane))) mapPatternAllocate(4);
    if (!mapIconEmbedded<u8>(icon,offsetof(hdBtnMapIcon_c,mpReceiverGatePane))) mapPatternAllocate(8);
    if (!mapIconEmbedded<u8>(icon,offsetof(hdBtnMapIcon_c,mpPrimaryPane))) mapPatternAllocate(4);
    if (!mapIconEmbedded<u8>(icon,offsetof(hdBtnMapIcon_c,mpNumberPane))) mapPatternAllocate(4);
    icon->mpClipBounds=nullptr;
    initializeMapIconPoint(mapIconEmbedded<HdMapPoint>(icon,offsetof(hdBtnMapIcon_c,mInitialRootTranslation)),0.0f);
    initializeMapIconPoint(mapIconEmbedded<HdMapPoint>(icon,offsetof(hdBtnMapIcon_c,mScrollPosition)),0.0f);
    icon->mOriginOffset.y=0.0f;
    icon->mSavedTranslation.y=0.0f;
    icon->mCenteredTranslation.y=0.0f;
    icon->mPosition.x=0.0f;
    icon->mCenteredTranslation.x=0.0f;
    icon->mOriginalTranslation.y=0.0f;
    icon->mOriginOffset.x=0.0f;
    icon->mOriginalTranslation.x=0.0f;
    icon->mOrigin.y=0.0f;
    icon->mpBoundsPane=nullptr;
    icon->mOrigin.x=0.0f;
    icon->mSavedTranslation.x=0.0f;
    icon->mPosition.y=0.0f;
    initializeMapIconBounds(mapIconEmbedded<hdMapIconBounds_c>(icon,offsetof(hdBtnMapIcon_c,mPaneBounds)));
    initializeMapIconBounds(mapIconEmbedded<hdMapIconBounds_c>(icon,offsetof(hdBtnMapIcon_c,mContentBounds)));
    hdMapIconBounds_c* padding=mapIconEmbedded<hdMapIconBounds_c>(icon,offsetof(hdBtnMapIcon_c,mBoundsPadding));
    if (!padding) padding=(hdMapIconBounds_c*)mapPatternAllocate(sizeof(hdMapIconBounds_c));
    if (padding) { padding->mRight=0.0f; padding->mBottom=0.0f; padding->mLeft=0.0f; padding->mTop=40.0f; }
    icon->mScrollOffset=0.0f;
    icon->mMapMode=5;
    icon->mScrollUp=0;
    icon->mExtraHeight=10.0f;
    initializeMapIconPoint(mapIconEmbedded<HdMapPoint>(icon,offsetof(hdBtnMapIcon_c,mTranslateScale)),1.0f);
    initializeMapIconPoint(mapIconEmbedded<HdMapPoint>(icon,offsetof(hdBtnMapIcon_c,mPointScale)),1.0f);
    icon->mpOwner=nullptr;
    icon->mFishOffset.x=0.0f;
    icon->mFishOffset.y=0.0f;
    icon->mFishVisible=0;
    icon->mpScrollPane=nullptr; icon->mpTextPane=nullptr; icon->mpParentPane=nullptr;
    icon->mpSecondaryPane=nullptr; icon->mpOriginPane=nullptr; icon->mpCenteredPane=nullptr;
    icon->mpWidthPane=nullptr; icon->mpTreasurePane=nullptr;
    icon->mpContentBoundsPane=nullptr; icon->mpReceiverGatePane=nullptr; icon->mpSelectionGatePane=nullptr;
    icon->mpNumberPane=nullptr; icon->mpPrimaryPane=nullptr;
    return icon;
}
static hdUiSafeString sMapIconLayoutName;
static hdUiSafeString sMapIconButtonSound;
extern u32 mapPatternBaseInitialize(hdBasicButton_c*,hdUiSafeString*,u32,u32,u32);
extern void mapIconSetSound(hdBasicButton_c*,hdUiSafeString*,u32);
/* 02637EF0 */
void hdBtnMapIcon_c::initializePanes() {
    findTransformPanes(); findIconPanes(); findContentBoundsPane();
    findReceiverGatePanes(); findPrimaryPane(); findNumberPane();
    updateContentBounds(); updateTranslateScale(); updatePointScale();
    mButton.mTouchEnabled=0;
    mapIconReset(mButton.animSet(),13,7);
    mapIconStop(mButton.animSet(),14,5);
    mapIconReset(mButton.animSet(),15,4);
    mButton.vCalcLayout();
    mButton.startState(&sMapIconSecondaryEndState);
}
/* 02637FE0 */
u32 hdBtnMapIcon_c::initialize(u32 first,u32 second,u32 third) {
    if (!mapPatternBaseInitialize(this,&sMapIconLayoutName,first,second,third)) return 0;
    initializePanes();
    mapIconSetSound(this,&sMapIconButtonSound,0);
    return 1;
}
extern void mapIconPaneBounds(HdMapIconPane*,hdMapIconBounds_c*);
/* 02637D28 */
void hdBtnMapIcon_c::updateContentBounds() {
    mapIconStop(mButton.animSet(),15,4);
    mButton.vCalcLayout();
    HdMapIconPane* content=mpContentBoundsPane;
    if (!content) return;
    hdMapIconBounds_c bounds;
    mapIconPaneBounds(content,&bounds);
    HdMapIconPane* root=reinterpret_cast<HdMapIconPane*>((hdPartsPane_c*)mButton.mpLayout->lyt()->mpRootPane);
    f32 x=root->mTranslateX,y=root->mTranslateY;
    hdMapPanePoint_c position;
    mapIconPanePosition(&position,content);
    f32 dx=(f32)position.x-x,dy=(f32)position.y-y;
    f32 right=(f32)bounds.mRight+dx,left=(f32)bounds.mLeft+dx;
    f32 top=(f32)bounds.mTop+dy,bottom=(f32)bounds.mBottom+dy;
    mInitialRootTranslation.y=y;
    mInitialRootTranslation.x=x;
    if (left<right) { mContentBounds.mRight=right; mContentBounds.mLeft=left; }
    else { mContentBounds.mRight=left; mContentBounds.mLeft=right; }
    if (top<bottom) { mContentBounds.mTop=bottom; mContentBounds.mBottom=top; }
    else { mContentBounds.mBottom=bottom; mContentBounds.mTop=top; }
}
static HdMapManagerPointer sMapIconLocationManager;
struct hdMapFishPosition_c { be<s16> x,y; u8 mPadding[4]; };
struct hdMapFishPositions_c { u8 mPrefix[0xA]; hdMapFishPosition_c mPositions[4]; };
struct hdMapFishSave_c { u8 mPrefix[0x1C1]; be<u8> mPositionIndex; };
struct hdMapFishSavePointer_c { gptr<hdMapFishSave_c> mpSave; };
static hdMapFishSavePointer_c sMapFishSave;
extern hdMapFishPositions_c* mapIconFindFish(HdMapLocations*,u32);
extern u32 mapIconFishKnown(HdMapLocations*,u32);
/* 0263821C */
void hdBtnMapIcon_c::refreshFish(hdMapIconSelection_c* selection) {
    u32 island=selection->mIsland;
    HdMapLocations* locations=sMapIconLocationManager.mpManager->mpLocations;
    hdMapFishPositions_c* fish=mapIconFindFish(locations,island);
    if (!fish) return;
    u32 index=sMapFishSave.mpSave->mPositionIndex;
    if (index<4) {
        mFishOffset.x=(f32)(s32)fish->mPositions[index].x;
        mFishOffset.y=(f32)(s32)fish->mPositions[index].y;
    }
    f32 x=(f32)mFishOffset.x*(f32)mTranslateScale.x;
    f32 y=-((f32)mFishOffset.y*(f32)mTranslateScale.y);
    HdMapIconPane* pane=mpTreasurePane;
    if (pane) {
        u32 flags=pane->mFlags;
        pane->mTranslateY=y;
        pane->mTranslateX=x;
        pane->mTranslateZ=0.0f;
        pane->mFlags=flags|0x10;
    }
    if (mapIconFishKnown(locations,island)) showFish(); else hideFish();
}
/* 026383C4 */
void hdBtnMapIcon_c::positionFishOnMap() {
    HdMapIconPane* pane=mpTreasurePane;
    if (!pane) return;
    f32 x=mFishOffset.x,y=mFishOffset.y;
    HdMapIconOwner* owner=mpOwner;
    if (owner) {
        HdMapInfo* info=owner->mpInfo;
        if (info) { x=x-(f32)(s32)info->mOffsetX; y=y-(f32)(s32)info->mOffsetY; }
    }
    x=x*(f32)mPointScale.x;
    y=-(y*(f32)mPointScale.y);
    f32 sx=1.0f,sy=1.0f;
    HdMapIconPane* parent=mpParentPane;
    if (parent) { sx=parent->mScaleX; sy=parent->mScaleY; }
    f32 dx=(f32)mPosition.x*(f32)mTranslateScale.x;
    f32 dy=-((f32)mPosition.y*(f32)mTranslateScale.y);
    x=gabi::fmadds(x,sx,dx);
    y=gabi::fmadds(y,sy,dy);
    u32 flags=pane->mFlags;
    pane->mTranslateX=x; pane->mTranslateY=y; pane->mTranslateZ=0.0f;
    pane->mFlags=flags|0x10;
}
/* 026384BC */
void hdBtnMapIcon_c::positionFishInDetail() {
    HdMapIconPane* pane=mpTreasurePane;
    if (!pane) return;
    f32 x=mFishOffset.x,y=mFishOffset.y;
    HdMapIconOwner* owner=mpOwner;
    if (owner) {
        HdMapInfo* info=owner->mpInfo;
        if (info) { x=x-(f32)(s32)info->mOffsetX; y=y-(f32)(s32)info->mOffsetY; }
    }
    x=x*(f32)mPointScale.x;
    y=-(y*(f32)mPointScale.y);
    f32 sx=1.0f,sy=1.0f;
    HdMapIconPane* parent=mpParentPane;
    if (parent) { sx=parent->mScaleX; sy=parent->mScaleY; }
    u32 flags=pane->mFlags;
    x=x*sx;
    pane->mFlags=flags|0x10;
    y=y*sy;
    pane->mTranslateX=x; pane->mTranslateY=y; pane->mTranslateZ=0.0f;
}
/* 02638598 */
void hdBtnMapIcon_c::updateFish(hdMapIconSelection_c* selection) {
    if (!mFishVisible && mapIconFishKnown(sMapIconLocationManager.mpManager->mpLocations,selection->mIsland)) revealFish();
    if (mMapMode==0 || mMapMode==4) positionFishOnMap();
    else if ((u32)mMapMode<4) positionFishInDetail();
}
struct hdMapIconRotation_c { u8 mPrefix[0x28]; be<f32> x,y,z; };
struct hdMapIconChartAngles_c {
    u8 mPrefix[0x38]; be<f32> mPrimaryAngle; u8 mMiddle[0xC];
    be<f32> mSecondaryAngle; u8 mGap[2]; be<u8> mShipVisible;
};
struct hdMapIconChartStatus_c { u8 mPrefix[0xA0]; be<u8> mAlternate; };
struct hdMapIconChartManager_c {
    u8 mPrefix[0x1C4]; gptr<hdMapIconChartStatus_c> mpStatus;
    u8 mGap[0x218-0x1C8]; gptr<hdMapIconChartAngles_c> mpAngles;
};
struct hdMapIconChartManagerPointer_c { gptr<hdMapIconChartManager_c> mpManager; };
static hdMapIconChartManagerPointer_c sMapIconChartManager;
extern u32 mapIconUsePrimaryAngle(HdMapLocations*);
/* 02638FD8 */
void hdBtnMapIcon_c::updatePrimaryRotation() {
    HdMapIconPane* pane=mpNumberPane;
    if (!pane) return;
    hdMapIconRotation_c* rotation=reinterpret_cast<hdMapIconRotation_c*>(pane);
    f32 x=rotation->x,y=rotation->y;
    bool primary=true;
    if (isAlternateChart() && !sMapIconChartManager.mpManager->mpStatus->mAlternate)
        primary=mapIconUsePrimaryAngle(sMapIconLocationManager.mpManager->mpLocations)!=0;
    f32 angle=primary?(f32)sMapIconChartManager.mpManager->mpAngles->mPrimaryAngle:(f32)sMapIconChartManager.mpManager->mpAngles->mSecondaryAngle;
    u32 flags=pane->mFlags;
    rotation->x=x; rotation->z=angle; rotation->y=y;
    pane->mFlags=flags|0x10;
}
/* 0263958C */
void hdBtnMapIcon_c::updateSecondaryRotation() {
    HdMapIconPane* pane=mpSecondaryPane;
    if (!pane) return;
    hdMapIconRotation_c* rotation=reinterpret_cast<hdMapIconRotation_c*>(pane);
    f32 x=rotation->x,y=rotation->y;
    f32 angle=sMapIconChartManager.mpManager->mpAngles->mSecondaryAngle;
    rotation->x=x;
    u32 flags=pane->mFlags;
    rotation->z=angle; rotation->y=y;
    pane->mFlags=flags|0x10;
}
/* 026390C4 */
void hdBtnMapIcon_c::updatePrimaryIcon() {
    HdMapIconPane* pane=mpPrimaryPane;
    if (!pane) return;
    u32 flags=pane->mFlags&0xFE;
    pane->mFlags=flags;
    if (!sMapIconChartManager.mpManager->mpAngles->mShipVisible) { pane->mFlags=flags; return; }
    pane->mFlags=flags|1;
    HdMapIcon* icon=reinterpret_cast<HdMapIcon*>(this);
    if (mMapMode==0 || mMapMode==4) icon->updatePrimaryInside();
    else if ((u32)mMapMode<4) icon->updatePrimaryOutside();
    updatePrimaryRotation();
}
/* 026395EC */
void hdBtnMapIcon_c::updateSecondaryIcon() {
    HdMapIconPane* pane=mpSecondaryPane;
    if (!pane) return;
    if (isAlternateChart()) { pane->mFlags=pane->mFlags&0xFE; return; }
    u32 flags=(pane->mFlags&0xFE)+1;
    pane->mFlags=flags;
    if (!sMapIconChartManager.mpManager->mpAngles->mShipVisible) { pane->mFlags=flags&0xFE; return; }
    pane->mFlags=(flags&0xFE)+1;
    HdMapIcon* icon=reinterpret_cast<HdMapIcon*>(this);
    if (mMapMode==0 || mMapMode==4) icon->updateSecondaryInside();
    else if ((u32)mMapMode<4) icon->updateSecondaryOutside();
    updateSecondaryRotation();
}
struct hdMapSelectionManager_c { u8 mPrefix[0xD0]; be<u32> mCurrentButton; };
struct hdMapIconOwnerChanges_c {
    u8 mPrefix[0x118]; gptr<hdMapIconSelection_c> mpSelection;
    u8 mGap[0x138-0x11C]; be<u8> mSelectionChanged;
    u8 mReserved[2]; be<u8> mForcePrimary; be<u8> mFishChanged;
};
extern u32 mapIconChartPaused(HdMapLocations*);
extern hdPartsStateId_c* mapIconCurrentState(hdStateMachine*);
extern void mapIconBaseUpdate(hdBasicButton_c*);
/* 026396FC */
void hdBtnMapIcon_c::update() {
    if (!mapIconChartPaused(sMapIconLocationManager.mpManager->mpLocations)) {
        updateZoomAnimation();
        hdPartsStateId_c* state=mapIconCurrentState(mButton.stateMachine());
        hdPartsStateIdVtbl_c* comparisonVtable=sMapIconSecondaryEndState.mVtable;
        u32 current=state->getKey();
        u32 secondary=gabi::call_ptr<u32>(comparisonVtable->mGetKey.get(),&sMapIconSecondaryEndState);
        if (current!=secondary) {
            bool reportButton=mMapMode==0 || mMapMode==2;
            if (!reportButton) {
                state=mapIconCurrentState(mButton.stateMachine());
                comparisonVtable=sMapIconClosePrimaryState.mVtable;
                current=state->getKey();
                reportButton=current==gabi::call_ptr<u32>(comparisonVtable->mGetKey.get(),&sMapIconClosePrimaryState);
                if (!reportButton) {
                    state=mapIconCurrentState(mButton.stateMachine());
                    comparisonVtable=sMapIconCloseSecondaryState.mVtable;
                    current=state->getKey();
                    reportButton=current==gabi::call_ptr<u32>(comparisonVtable->mGetKey.get(),&sMapIconCloseSecondaryState);
                }
            }
            if (reportButton) {
                hdMapSelectionManager_c* manager=sMapSelectionManager.mpManager;
                manager->mCurrentButton=mapIconGetButtonId(this);
            }
            hdMapIconOwnerChanges_c* owner=reinterpret_cast<hdMapIconOwnerChanges_c*>((HdMapIconOwner*)mpOwner);
            if (owner && owner->mpSelection) {
                hdMapIconSelection_c* selection=owner->mpSelection;
                if (owner->mSelectionChanged || owner->mFishChanged || owner->mForcePrimary) {
                    if (owner->mForcePrimary) showPrimaryImmediately();
                    updateSelection(selection);
                    refreshFish(selection);
                }
                updateFish(selection);
                updateSelectionTranslation(); updateOriginTranslation();
                updatePrimaryIcon(); updateSecondaryIcon();
            }
        }
    }
    mapIconBaseUpdate(this);
}
/* 0263A0B0 */
void hdBtnMapIcon_c::constrainScroll() {
    if (!mpScrollPane || !mpClipBounds) return;
    hdMapIconBounds_c* clip=mpClipBounds;
    f32 centerX=((f32)clip->mLeft+(f32)clip->mRight)*0.5f;
    f32 centerY=((f32)clip->mBottom+(f32)clip->mTop)*0.5f;
    f32 height=(f32)clip->mTop-(f32)clip->mBottom;
    f32 halfHeight=gabi::fmadds(height,0.5f,mExtraHeight);
    f32 upperY=centerY+halfHeight;
    f32 lowerY=centerY-halfHeight;
    f32 left=(f32)mContentBounds.mLeft+centerX;
    f32 right=(f32)mContentBounds.mRight+centerX;
    f32 top=(f32)mContentBounds.mTop+upperY;
    updatePaneBounds();
    f32 paneLeft=(f32)mPaneBounds.mLeft+(f32)mBoundsPadding.mLeft;
    f32 paneBottom=(f32)mPaneBounds.mBottom+(f32)mBoundsPadding.mBottom;
    f32 paneRight=(f32)mPaneBounds.mRight+(f32)mBoundsPadding.mRight;
    f32 paneTop=(f32)mPaneBounds.mTop+(f32)mBoundsPadding.mTop;
    mScrollUp=0;
    mScrollOffset=0.0f;
    if (left<paneLeft) mScrollOffset=-(left-paneLeft);
    else if (paneRight<right) mScrollOffset=paneRight-right;
    if (paneTop<top) { mScrollPosition.x=centerX; mScrollPosition.y=lowerY; mScrollUp=1; }
    else { mScrollPosition.x=centerX; mScrollPosition.y=upperY; }
    (void)paneBottom;
}
/* 0263A348 */
void hdBtnMapIcon_c::resetScroll() {
    constrainScroll();
    updateScrollTransforms();
    mapIconReset(mButton.animSet(),0,0);
}
struct hdMapIconChildDescriptor_c {
    u8 mPrefix[8]; u8 mResource[0xC]; hdUiSafeString mName; u8 mGap[8]; gptr<const char> mClassName;
};
static hdUiStringTable sMapIconChildStringVtable;
static hdUiSafeString sMapIconChildClassName;
extern u32 mapIconBaseCreateChild(hdBtnMapIcon_c*,u32,hdPartsDesc_c*);
extern hdParts_c* mapIconConstructChild(hdParts_c*);
extern u32 mapIconInitializeChild(hdParts_c*,void*,u32,u32,hdPartsDesc_c*);
static bool mapIconStringsEqual(be<u8>* first,be<u8>* second) {
    for (u32 i=0;i<0x40001;++i) {
        u32 a=first[i],b=second[i];
        if (a!=b) return false;
        if (!a) return true;
    }
    return false;
}
/* 026398F0 */
u32 hdBtnMapIcon_c::createMapChild(hdPartsDesc_c* descriptor) {
    hdMapIconChildDescriptor_c* child=reinterpret_cast<hdMapIconChildDescriptor_c*>(descriptor);
    void* heap=sMapMainLayoutHeap.mpContext->mpHeap;
    hdUiSafeString className;
    className.mVtable=&sMapIconChildStringVtable;
    className.mText.v=child->mClassName.v.get();
    child->mName.assureTermination(); child->mName.assureTermination();
    be<u8>* childName=child->mName.mText;
    sMapFindPrimaryPaneName.assureTermination();
    if (childName!=sMapFindPrimaryPaneName.mText.get() &&
        !mapIconStringsEqual(child->mName.mText,sMapFindPrimaryPaneName.mText)) return 1;
    className.assureTermination(); className.assureTermination();
    be<u8>* target=className.mText;
    sMapIconChildClassName.assureTermination();
    if (target!=sMapIconChildClassName.mText.get() &&
        !mapIconStringsEqual(className.mText,sMapIconChildClassName.mText)) return mapIconBaseCreateChild(this,0,descriptor);
    gptr<hdParts_c>* slot=mButton.childSlot(0);
    hdParts_c* allocation=reinterpret_cast<hdParts_c*>(mapIconAllocateLayout(0x50,heap,4));
    if (allocation) allocation=mapIconConstructChild(allocation);
    *slot=allocation;
    hdParts_c* part=mButton.child(0);
    if (!part) return 0;
    return mapIconInitializeChild(part,child->mResource,0,0,descriptor);
}
struct MapIconStaticList_c { be<u32> mWords[4]; };
struct MapIconRegistration_c { be<u32> mWords[3]; };
struct MapIconStaticFlag_c { be<u8> mValue; };
struct MapIconStaticBounds_c { be<f32> mMinimumAngle,mMaximumAngle; HdMapPoint mWorldHalf,mExtents; };
struct MapIconCounter_c { be<u32> mValue; };
static MapIconStaticList_c sMapIconStaticList;
static MapIconRegistration_c sMapIconRegistrationList;
static MapIconRegistration_c sMapIconRegistrationFirstFlag;
static MapIconRegistration_c sMapIconRegistrationSecondFlag;
static MapIconStaticFlag_c sMapIconFlagFirst;
static MapIconStaticFlag_c sMapIconFlagSecond;
static MapIconStaticBounds_c sMapIconStaticBounds;
static hdUiStringTable sMapIconStaticStringVtable;
static hdPartsStateIdVtbl_c sMapIconStaticStateVtable;
static hdStateId sMapIconStaticNullState;
static MapIconCounter_c sMapIconCounterGuard;
static MapIconCounter_c sMapIconCounter;
extern void mapCountRegister(void*);
extern void mapCountConstructFirstFlag(void*);
extern void mapCountConstructSecondFlag(void*);
static hdUiSafeString sMapIconOptionalPaneName;
static hdPartsStateId_c sMapIconStateReverse;
static hdPartsStateId_c sMapIconStateZoomIn;
static hdPartsStateId_c sMapIconStateFullZoomIn;
static hdPartsStateId_c sMapIconStateMapPattern;
static hdPartsStateId_c sMapIconStateFullZoomOut;
static hdPartsStateId_c sMapIconStateZoomOut;
static hdPartsStateId_c sMapIconStateTreasureGet;
static hdPartsStateId_c sMapIconStateOpenDemo;
static hdPartsStateId_c sMapIconStateIn;
static hdPartsStateId_c sMapIconStateOut;
static hdPartsStateId_c sMapIconStateWait;
/* 0263A574 */
void hdBtnMapIcon_c::initializeStatics() {
    sMapIconStaticList.mWords[3]=0; sMapIconStaticList.mWords[2]=0;
    sMapIconStaticList.mWords[1]=0; sMapIconStaticList.mWords[0]=0;
    mapCountRegister(&sMapIconRegistrationList);
    sMapIconStaticBounds.mMinimumAngle=-3.1415927410125732f;
    sMapIconStaticBounds.mMaximumAngle=3.1415927410125732f;
    mapCountConstructFirstFlag(&sMapIconFlagFirst);
    mapCountRegister(&sMapIconRegistrationFirstFlag);
    mapCountConstructSecondFlag(&sMapIconFlagSecond);
    mapCountRegister(&sMapIconRegistrationSecondFlag);
    sMapIconStaticBounds.mWorldHalf.x=50000.0f; sMapIconStaticBounds.mWorldHalf.y=50000.0f;
    sMapIconStaticBounds.mExtents.x=10000.0f; sMapIconStaticBounds.mExtents.y=10000.0f;
    sMapFindContentBoundsPaneName.mVtable=&sMapIconStaticStringVtable;
    sMapFindContentBoundsPaneName.mText.v=gabi::guest_addr("W_MapBalloon_00");
    sMapNumberGroup.mVtable=&sMapIconStaticStringVtable;
    sMapNumberGroup.mText.v=gabi::guest_addr("L_LinkPos_00");
    sMapNumberName.mVtable=&sMapIconStaticStringVtable;
    sMapNumberName.mText.v=gabi::guest_addr("N_Rotate_00");
    sMapFindPrimaryPaneName.mVtable=&sMapIconStaticStringVtable;
    sMapFindPrimaryPaneName.mText.v=gabi::guest_addr("L_LinkPos_00");
    sMapIconLayoutName.mVtable=&sMapIconStaticStringVtable;
    sMapIconLayoutName.mText.v=gabi::guest_addr("BtnMapBalloon_00");
    sMapIconOptionalPaneName.mVtable=&sMapIconStaticStringVtable;
    sMapIconOptionalPaneName.mText.v=gabi::guest_addr("LinkPos_00");
    sMapFindTransformPanesNames.mNames[0].mVtable=&sMapIconStaticStringVtable;
    sMapFindTransformPanesNames.mNames[0].mText.v=gabi::guest_addr("N_Trans_00");
    sMapFindTransformPanesNames.mNames[1].mVtable=&sMapIconStaticStringVtable;
    sMapFindTransformPanesNames.mNames[1].mText.v=gabi::guest_addr("N_StandardPos_00");
    sMapFindTransformPanesNames.mNames[2].mVtable=&sMapIconStaticStringVtable;
    sMapFindTransformPanesNames.mNames[2].mText.v=gabi::guest_addr("N_MapBig_00");
    sMapFindIconPanesNames.mNames[0].mVtable=&sMapIconStaticStringVtable;
    sMapFindIconPanesNames.mNames[0].mText.v=gabi::guest_addr("P_ShipPos_00");
    sMapFindIconPanesNames.mNames[1].mVtable=&sMapIconStaticStringVtable;
    sMapFindIconPanesNames.mNames[1].mText.v=gabi::guest_addr("P_DummyMapSmall_00");
    sMapFindIconPanesNames.mNames[2].mVtable=&sMapIconStaticStringVtable;
    sMapFindIconPanesNames.mNames[2].mText.v=gabi::guest_addr("P_DotLine_04");
    sMapFindIconPanesNames.mNames[3].mVtable=&sMapIconStaticStringVtable;
    sMapFindIconPanesNames.mNames[3].mText.v=gabi::guest_addr("P_DotLine_05");
    sMapFindIconPanesNames.mNames[4].mVtable=&sMapIconStaticStringVtable;
    sMapFindIconPanesNames.mNames[4].mText.v=gabi::guest_addr("P_TreasureGet_00");
    sMapFindReceiverGatePanesNames.mNames[0].mVtable=&sMapIconStaticStringVtable;
    sMapFindReceiverGatePanesNames.mNames[0].mText.v=gabi::guest_addr("B_Btn_00");
    sMapFindReceiverGatePanesNames.mNames[1].mVtable=&sMapIconStaticStringVtable;
    sMapFindReceiverGatePanesNames.mNames[1].mText.v=gabi::guest_addr("B_BtnFullZoom_00");
    sMapMainLayoutAnimations.mEntries[0].mVtable=&sMapIconStaticStringVtable;
    sMapMainLayoutAnimations.mEntries[0].mText.v=gabi::guest_addr("G_Reverse_00");
    sMapMainLayoutAnimations.mEntries[1].mVtable=&sMapIconStaticStringVtable;
    sMapMainLayoutAnimations.mEntries[1].mText.v=gabi::guest_addr("G_Btn_00");
    sMapMainLayoutAnimations.mEntries[2].mVtable=&sMapIconStaticStringVtable;
    sMapMainLayoutAnimations.mEntries[2].mText.v=gabi::guest_addr("G_All_00");
    sMapMainLayoutAnimations.mEntries[3].mVtable=&sMapIconStaticStringVtable;
    sMapMainLayoutAnimations.mEntries[3].mText.v=gabi::guest_addr("G_Zoom_00");
    sMapMainLayoutAnimations.mEntries[4].mVtable=&sMapIconStaticStringVtable;
    sMapMainLayoutAnimations.mEntries[4].mText.v=gabi::guest_addr("G_MapPattern_00");
    sMapMainLayoutAnimations.mEntries[5].mVtable=&sMapIconStaticStringVtable;
    sMapMainLayoutAnimations.mEntries[5].mText.v=gabi::guest_addr("G_TreasureGet_00");
    sMapMainLayoutGroups.mEntries[0].mVtable=&sMapIconStaticStringVtable;
    sMapMainLayoutGroups.mEntries[0].mText.v=gabi::guest_addr("Reverse");
    sMapMainLayoutGroups.mEntries[1].mVtable=&sMapIconStaticStringVtable;
    sMapMainLayoutGroups.mEntries[1].mText.v=gabi::guest_addr("ZoomIn");
    sMapMainLayoutGroups.mEntries[2].mVtable=&sMapIconStaticStringVtable;
    sMapMainLayoutGroups.mEntries[2].mText.v=gabi::guest_addr("FullZoomIn");
    sMapMainLayoutGroups.mEntries[3].mVtable=&sMapIconStaticStringVtable;
    sMapMainLayoutGroups.mEntries[3].mText.v=gabi::guest_addr("MapPattern");
    sMapMainLayoutGroups.mEntries[4].mVtable=&sMapIconStaticStringVtable;
    sMapMainLayoutGroups.mEntries[4].mText.v=gabi::guest_addr("FullZoomOut");
    sMapMainLayoutGroups.mEntries[5].mVtable=&sMapIconStaticStringVtable;
    sMapMainLayoutGroups.mEntries[5].mText.v=gabi::guest_addr("ZoomOut");
    sMapMainLayoutGroups.mEntries[6].mVtable=&sMapIconStaticStringVtable;
    sMapMainLayoutGroups.mEntries[6].mText.v=gabi::guest_addr("TreasureGet");
    sMapMainLayoutGroups.mEntries[7].mVtable=&sMapIconStaticStringVtable;
    sMapMainLayoutGroups.mEntries[7].mText.v=gabi::guest_addr("OpenDemo");
    sMapMainLayoutGroups.mEntries[8].mVtable=&sMapIconStaticStringVtable;
    sMapMainLayoutGroups.mEntries[8].mText.v=gabi::guest_addr("In");
    sMapMainLayoutGroups.mEntries[9].mVtable=&sMapIconStaticStringVtable;
    sMapMainLayoutGroups.mEntries[9].mText.v=gabi::guest_addr("Out");
    sMapMainLayoutGroups.mEntries[10].mVtable=&sMapIconStaticStringVtable;
    sMapMainLayoutGroups.mEntries[10].mText.v=gabi::guest_addr("Wait");
    u32 number;
    if (sMapIconCounterGuard.mValue) number=sMapIconCounter.mValue;
    else { sMapIconCounterGuard.mValue=1; number=0; }
    sMapIconStateReverse.setup(++number,"::StateID_Reverse",&sMapIconStaticStateVtable,75,&sMapIconStaticNullState);
    sMapIconStateZoomIn.setup(++number,"::StateID_ZoomIn",&sMapIconStaticStateVtable,78,&sMapIconStaticNullState);
    sMapIconStateFullZoomIn.setup(++number,"::StateID_FullZoomIn",&sMapIconStaticStateVtable,81,&sMapIconStaticNullState);
    sMapIconStateMapPattern.setup(++number,"::StateID_MapPattern",&sMapIconStaticStateVtable,84,&sMapIconStaticNullState);
    sMapIconStateFullZoomOut.setup(++number,"::StateID_FullZoomOut",&sMapIconStaticStateVtable,87,&sMapIconStaticNullState);
    sMapIconStateZoomOut.setup(++number,"::StateID_ZoomOut",&sMapIconStaticStateVtable,90,&sMapIconStaticNullState);
    sMapIconStateTreasureGet.setup(++number,"::StateID_TreasureGet",&sMapIconStaticStateVtable,93,&sMapIconStaticNullState);
    sMapIconStateOpenDemo.setup(++number,"::StateID_OpenDemo",&sMapIconStaticStateVtable,96,&sMapIconStaticNullState);
    sMapIconStateIn.setup(++number,"::StateID_In",&sMapIconStaticStateVtable,65,&sMapIconStaticNullState);
    sMapIconStateOut.setup(++number,"::StateID_Out",&sMapIconStaticStateVtable,68,&sMapIconStaticNullState);
    sMapIconStateWait.setup(++number,"::StateID_Wait",&sMapIconStaticStateVtable,99,&sMapIconStaticNullState);
    sMapIconCounter.mValue=number;
}
/* WWHD glue
data sMapIconStaticList 104901FC
data sMapIconRegistrationList 101F5580
data sMapIconRegistrationFirstFlag 101F558C
data sMapIconRegistrationSecondFlag 101F5598
data sMapIconFlagFirst 104901F8
data sMapIconFlagSecond 104901F9
data sMapIconStaticBounds 104901E0
data sMapIconStaticStringVtable 100E948C
data sMapIconStaticStateVtable 100E9518
data sMapIconStaticNullState 101FF32C
data sMapIconCounterGuard 101FD8E4
data sMapIconCounter 101FDD50
data sMapIconOptionalPaneName 104901D8
str 100E961C "W_MapBalloon_00" in 0263A574
str 100E9778 "L_LinkPos_00" in 0263A574
str 100E9648 "N_Rotate_00" in 0263A574
str 100E9778 "L_LinkPos_00" in 0263A574
str 100E97F4 "BtnMapBalloon_00" in 0263A574
str 100E97E8 "LinkPos_00" in 0263A574
str 100E96E0 "N_Trans_00" in 0263A574
str 100E96EC "N_StandardPos_00" in 0263A574
str 100E963C "N_MapBig_00" in 0263A574
str 100E9700 "P_ShipPos_00" in 0263A574
str 100E9710 "P_DummyMapSmall_00" in 0263A574
str 100E9724 "P_DotLine_04" in 0263A574
str 100E9734 "P_DotLine_05" in 0263A574
str 100E9744 "P_TreasureGet_00" in 0263A574
str 100E9758 "B_Btn_00" in 0263A574
str 100E9764 "B_BtnFullZoom_00" in 0263A574
str 100E9788 "G_Reverse_00" in 0263A574
str 100E9798 "G_Btn_00" in 0263A574
str 100E97A4 "G_All_00" in 0263A574
str 100E966C "G_Zoom_00" in 0263A574
str 100E962C "G_MapPattern_00" in 0263A574
str 100E97B0 "G_TreasureGet_00" in 0263A574
str 100E95F4 "Reverse" in 0263A574
str 100E9608 "ZoomIn" in 0263A574
str 100E97C4 "FullZoomIn" in 0263A574
str 100E97D0 "MapPattern" in 0263A574
str 100E9654 "FullZoomOut" in 0263A574
str 100E95FC "ZoomOut" in 0263A574
str 100E9660 "TreasureGet" in 0263A574
str 100E97DC "OpenDemo" in 0263A574
str 100E9610 "In" in 0263A574
str 100E9604 "Out" in 0263A574
str 100E9614 "Wait" in 0263A574
data sMapIconStateReverse 104902E4
str 100E9678 "::StateID_Reverse" in 0263A574
data sMapIconStateZoomIn 10490314
str 100E9808 "::StateID_ZoomIn" in 0263A574
data sMapIconStateFullZoomIn 10490344
str 100E981C "::StateID_FullZoomIn" in 0263A574
data sMapIconStateMapPattern 10490374
str 100E9834 "::StateID_MapPattern" in 0263A574
data sMapIconStateFullZoomOut 104903A4
str 100E968C "::StateID_FullZoomOut" in 0263A574
data sMapIconStateZoomOut 104903D4
str 100E96A4 "::StateID_ZoomOut" in 0263A574
data sMapIconStateTreasureGet 10490404
str 100E96B8 "::StateID_TreasureGet" in 0263A574
data sMapIconStateOpenDemo 10490434
str 100E984C "::StateID_OpenDemo" in 0263A574
data sMapIconStateIn 10490464
str 100E9860 "::StateID_In" in 0263A574
data sMapIconStateOut 10490494
str 100E96D0 "::StateID_Out" in 0263A574
data sMapIconStateWait 104904C4
str 100E9870 "::StateID_Wait" in 0263A574
ext 026F92C0 u32 mapIconBaseCreateChild(hdBtnMapIcon_c*,u32,hdPartsDesc_c*)
ext 02700ADC hdParts_c* mapIconConstructChild(hdParts_c*)
ext 02700B48 u32 mapIconInitializeChild(hdParts_c*,void*,u32,u32,hdPartsDesc_c*)
data sMapIconChildStringVtable 100E948C
data sMapIconChildClassName 1049E518
ext 026890A8 u32 mapIconChartPaused(HdMapLocations*)
ext 02006478 hdPartsStateId_c* mapIconCurrentState(hdStateMachine*)
ext 026F1F70 void mapIconBaseUpdate(hdBasicButton_c*)
ext 026890E8 u32 mapIconUsePrimaryAngle(HdMapLocations*)
data sMapIconChartManager 101F8344
ext 0268A720 hdMapFishPositions_c* mapIconFindFish(HdMapLocations*,u32)
ext 0268A7A0 u32 mapIconFishKnown(HdMapLocations*,u32)
data sMapIconLocationManager 101F8344
data sMapFishSave 101F84DC
ext 02876368 void mapIconPaneBounds(HdMapIconPane*,hdMapIconBounds_c*)
ext 026F2540 void mapIconSetSound(hdBasicButton_c*,hdUiSafeString*,u32)
data sMapIconLayoutName 104904F4
data sMapIconButtonSound 104A0CD8
data sMapIconVtable 100E9884
ext 025200D4 hdMapChartFlags_c* mapIconChartFlags()
data sMapIconExtents 104901F0
ext 02704FC0 HdMapIconPane* mapIconFindNumberPane(hdPartsLayout_c*,hdUiSafeString*,hdUiSafeString*)
data sMapNumberGroup 104901C0
data sMapNumberName 104901C8
data sMapFindTransformPanesNames 1049020C
data sMapFindIconPanesNames 10490224
data sMapFindReceiverGatePanesNames 1049024C
data sMapFindContentBoundsPaneName 104901B8
data sMapFindPrimaryPaneName 104901D0
data sMapMainLayoutHeap 1018C404
data sMapMainLayoutIndices 100E94A4
data sMapMainLayoutGroups 1049028C
data sMapMainLayoutAnimations 1049025C
ext 0273B050 hdPartsLayout_c* mapIconAllocateLayout(u32,void*,u32)
ext 02004E04 void mapIconBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
ext 02704DE4 void mapIconPanePosition(hdMapPanePoint_c*,HdMapIconPane*)
ext 0263862C f32 mapIconChasePoint(HdMapPoint*,HdMapPoint*,f32,f32,f32)
ext 02615140 hdMapActiveSelection_c* mapIconGetActiveSelection(hdMapSelectionManager_c*)
ext 026F5B74 u32 mapIconGetButtonId(hdBtnMapIcon_c*)
ext 02615154 void mapIconSetActiveSelection(hdMapSelectionManager_c*,u32)
data sMapSelectionManager 101F4FF0
ext 020053E4 void mapIconPlay(hdAnimSet*,u32,u32,f32)
ext 02005708 void mapIconStop(hdAnimSet*,u32,u32)
ext 02005674 void mapIconReset(hdAnimSet*,u32,u32)
ext 0200552C void mapIconSetFrame(hdAnimSet*,u32,u32,f32)
ext 02005840 u32 mapIconAnimationEnded(hdAnimSet*,u32)
ext 02030B38 void mapIconSound(const char*)
ext 026F0B8C void mapIconBaseDestroy(hdBasicButton_c*,u32)
ext 0273AF40 void mapIconDelete(void*)
data sMapIconStateVtable 100E9BB4
data sMapIconOwnerVtable 100E9BC4
data sMapIconPrimaryEndState 1049D830
data sMapIconSecondaryEndState 1049DD60
data sMapIconClosePrimaryState 10490464
data sMapIconCloseSecondaryState 10490494
str 100E95C4 "SE_MENU_ZOOM_IN" in 0263A438
str 100E95D4 "SE_MENU_ZOOM_IN" in 0263A520
str 100E9564 "SE_MENU_ZOOM_IN" in 02636F90
str 100E9574 "SE_MENU_ZOOM_IN" in 02636FE4
*/
