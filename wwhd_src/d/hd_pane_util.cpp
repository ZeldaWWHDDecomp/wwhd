/** HD pane hierarchy helpers. */
#include "d/hd_pane_util.h"
#include "bindings.h"
static u32 sPaneUtilHeaderStatics;
static u32 sPaneUtilHeaderDescriptors;
static u32 sPaneUtilHeaderZero;
static u32 sPaneUtilPiConstants;

/* 02704D2C */
void hdPaneUtil_c::getGlobalTranslation(cXyz* out, PaneUtilPane_l* pane) {
    if (!pane) {
        out->set(0.0f, 0.0f, 0.0f);
        return;
    }
    f32 x = pane->mGlobalMatrix.m[0][3];
    f32 y = pane->mGlobalMatrix.m[1][3];
    out->set(x, y, 0.0f);
}

/* 02704DE4 */
void hdPaneUtil_c::getAccumulatedTranslation(cXyz* out, PaneUtilPane_l* pane) {
    if (!pane) {
        out->set(0.0f, 0.0f, 0.0f);
        return;
    }
    f32 z = pane->mTranslation.z;
    f32 x = pane->mTranslation.x;
    f32 y = pane->mTranslation.y;
    for (PaneUtilPane_l* parent = pane->mpParent; parent; parent = parent->mpParent) {
        x = gabi::fadds_ppc(x, parent->mTranslation.x);
        y = gabi::fadds_ppc(y, parent->mTranslation.y);
        z = gabi::fadds_ppc(z, parent->mTranslation.z);
    }
    out->set(x, y, z);
}

/* 02704EA8 */
Pair32 hdPaneUtil_c::getAccumulatedScale(PaneUtilPane_l* pane) {
    f32 x = 1.0f, y = 1.0f;
    if (pane) {
        x = pane->mScaleX;
        y = pane->mScaleY;
        for (PaneUtilPane_l* parent = pane->mpParent; parent; parent = parent->mpParent) {
            x = gabi::fmuls_ppc(x, parent->mScaleX);
            y = gabi::fmuls_ppc(y, parent->mScaleY);
        }
    }
    u32 xBits, yBits;
    __builtin_memcpy(&xBits, &x, sizeof(xBits));
    __builtin_memcpy(&yBits, &y, sizeof(yBits));
    return {xBits, yBits};
}

static void* sMaterialLock;

/* 02704CD8 */
static void hdPaneAccessor_destroy(PaneUtilAccessor_l* accessor, u32 flags) {
    if (accessor) {
        PaneUtilAccessor_destroy(accessor, 0);
        if (flags & 1) PaneUtil_delete(accessor);
    }
}

/* 02704F3C */
static void hdPane_setMaterial(PaneUtilAccessor_l* accessor, void* value, u32 paneIndex, u32 materialIndex) {
    PaneUtilMaterialList_l* list = gabi::call_ptr<PaneUtilMaterialList_l*>(accessor->mpVTable[0x4C / 4].get(), accessor, paneIndex);
    PaneUtil_lock(sMaterialLock);
    PaneUtilMaterial_set(&list->mpMaterials[materialIndex], value);
    PaneUtil_unlock(sMaterialLock);
}

/* 02704FC0 */
static void hdPane_setName(PaneUtilOwner_l* owner, PaneUtilString_l* paneName, PaneUtilString_l* name) {
    PaneUtilNamedPane_l* pane;
    gfn* table;
    if (paneName->mpBuffer[0] != 0) {
        gabi::call_ptr<void>(paneName->mpVTable[0x14 / 4].get(), paneName);
        pane = PaneUtil_findPane(owner->mpLayout, paneName->mpBuffer);
        if (pane) {
            table = pane->mpVTable;
            gabi::call_ptr<void>(name->mpVTable[0x14 / 4].get(), name);
            gabi::call_ptr<void>(table[0x5C / 4].get(), pane, name->mpBuffer.get(), 1);
            return;
        }
    }
    PaneUtilString_l* value = name;
    PaneUtilLayout_l* layout = owner->mpLayout;
    pane = layout->mpRootPane;
    table = pane->mpVTable;
    gabi::call_ptr<void>(value->mpVTable[0x14 / 4].get(), value);
    gabi::call_ptr<void>(table[0x5C / 4].get(), pane, value->mpBuffer.get(), 1);
}

/* 027050A4 */
static void __sinit_hd_pane_util_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sPaneUtilHeaderStatics), gabi::ea(&sPaneUtilHeaderDescriptors), gabi::ea(&sPaneUtilHeaderZero), gabi::ea(&sPaneUtilPiConstants));
}

/* WWHD glue
data sPaneUtilHeaderStatics 1049E7B4 nocheck
data sPaneUtilHeaderDescriptors 101F73F0 nocheck
data sPaneUtilHeaderZero 1049E7C0 nocheck
data sPaneUtilPiConstants 1010C050 nocheck
 data sMaterialLock 101F8B18
 ext 020022E4 void PaneUtilAccessor_destroy(PaneUtilAccessor_l*, u32)
 ext 0273AF40 void PaneUtil_delete(void*)
 ext 0274FBF8 void PaneUtil_lock(void*)
 ext 0274FCCC void PaneUtil_unlock(void*)
 ext 02875448 void PaneUtilMaterial_set(PaneUtilMaterial_l*, void*)
 ext 0287FE10 PaneUtilNamedPane_l* PaneUtil_findPane(PaneUtilLayout_l*, const be<u8>*)
*/
