/* HD Picto Box background texture, opacity and layout loading. */
#include "d/hd_screen_pictograph.h"
using namespace gabi;

static hdPartsVtbl_c sBackdropVtable;
static u8 sBackdropHolderVtable;
static u8 sBackdropSecondaryVtable;
static u8 sBackdropStringVtable;
static hdUiSafeString sBackdropArchive;
static hdPictoStaticStorage sBackdropStatics;
static hdPictoRegistration sBackdropStorageRegistration;
static hdPictoRegistration sBackdropFirstRegistration;
static hdPictoRegistration sBackdropSecondRegistration;
static f32 sBackdropAlphaScale;
static f32 sBackdropMinimum;
static f32 sBackdropMaximum;
static hdScreenUiSystem* sBackdropUi;

void pictographDestroyNoControl(hdParts_c*, u32);
void* pictographAllocate(u32);
void pictographFree(void*);
void pictographSetPaneTexture(hdPartsPaneSRT_c*, void*, u32, u32);
void pictographRegisterStatic(hdPictoRegistration*);
void pictographConstructFirstFlag(be<u8>*);
void pictographConstructSecondFlag(be<u8>*);

/* 026C0F7C */
u32 hdPictoStateKey::number() { return mNumber; }

/* 026C107C */
void hdPictographBackdrop::destroy(hdPictographBackdrop* self, u32 flags) {
    if (!self) return;
    self->mStateHolder.mHolderVtable = &sBackdropHolderVtable;
    self->mSecondaryVtable = &sBackdropSecondaryVtable;
    pictographDestroyNoControl(self, 0);
    if (flags & 1) pictographFree(self);
}
/* 026C10E8 */
hdPictographBackdrop* hdPictographBackdrop::construct(hdPictographBackdrop* self) {
    if (!self) {
        self = static_cast<hdPictographBackdrop*>(pictographAllocate(sizeof(hdPictographBackdrop)));
        if (!self) return NULL;
    }
    hdPartsNoControl_construct(self);
    self->mVtable = &sBackdropVtable;
    self->mStateHolder.mHolderVtable = &sBackdropHolderVtable;
    self->mSecondaryVtable = &sBackdropSecondaryVtable;
    return self;
}
/* 026C1154 */
u32 hdPictographBackdrop::loadBackdrop(u32 heap, u32 resource, hdPartsDesc_c* desc) {
    return hdPartsNoControl_init(this, &sBackdropArchive, heap, resource, desc);
}
/* 026C116C */
void hdPictographBackdrop::setTexture(void* texture) {
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    hdPartsPaneSRT_c* pane = root->find("PF_Pictograph_00");
    pictographSetPaneTexture(pane, texture, 0, 0);
}
/* 026C11CC */
void hdPictographBackdrop::setAlpha(f32 alpha) {
    f32 scaled = fmuls_ppc(alpha, sBackdropAlphaScale);
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    root->mAlpha = u8(ftoi(scaled));
}
/* 026C1204 */
u32 hdPictographBackdrop::loadLayout(hdUiSafeString* name, u32 accessor, u32 heap, u32 resource, void* desc) {
    auto* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sBackdropUi->mHeap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, accessor, heap, resource, 0, 0, desc)) return 0;
    return 1;
}
/* 026C12C0 */
void initializePictographBackdrop() {
    sBackdropStatics.mWords[3] = 0;
    sBackdropStatics.mWords[2] = 0;
    sBackdropStatics.mWords[1] = 0;
    sBackdropStatics.mWords[0] = 0;
    pictographRegisterStatic(&sBackdropStorageRegistration);
    f32 minimum = sBackdropMinimum;
    f32 maximum = sBackdropMaximum;
    sBackdropStatics.mMinimum = minimum;
    sBackdropStatics.mMaximum = maximum;
    pictographConstructFirstFlag(&sBackdropStatics.mFirstFlag);
    pictographRegisterStatic(&sBackdropFirstRegistration);
    pictographConstructSecondFlag(&sBackdropStatics.mSecondFlag);
    pictographRegisterStatic(&sBackdropSecondRegistration);
    sBackdropArchive.mVtable = reinterpret_cast<hdUiStringTable*>(&sBackdropStringVtable);
    sBackdropArchive.mText.v = guest_addr("Pictograph_00");
}
/* WWHD glue
data sBackdropVtable 10100548
data sBackdropHolderVtable 10100610
data sBackdropSecondaryVtable 10100620
data sBackdropStringVtable 101004F4
data sBackdropArchive 10499608
data sBackdropStatics 104995EC
data sBackdropStorageRegistration 101F6878
data sBackdropFirstRegistration 101F6884
data sBackdropSecondRegistration 101F6890
data sBackdropAlphaScale 10100528
data sBackdropMinimum 1010052C
data sBackdropMaximum 10100530
data sBackdropUi 1018C404
str 10100514 "PF_Pictograph_00" in 026C116C
str 10100534 "Pictograph_00" in 026C12C0
ext 02700A70 void pictographDestroyNoControl(hdParts_c*,u32)
ext 0273AD10 void* pictographAllocate(u32)
ext 0273AF40 void pictographFree(void*)
ext 02704F3C void pictographSetPaneTexture(hdPartsPaneSRT_c*,void*,u32,u32)
ext 028F026C void pictographRegisterStatic(hdPictoRegistration*)
ext 028ED6F8 void pictographConstructFirstFlag(be<u8>*)
ext 028EAB2C void pictographConstructSecondFlag(be<u8>*)
*/
