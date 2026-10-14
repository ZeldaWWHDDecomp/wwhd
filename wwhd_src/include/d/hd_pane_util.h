#pragma once
#include "wwhd.h"
#include "SSystem/SComponent/c_xyz.h"
// HD pane utility layout; nw4f remains an opaque library dependency.
struct PaneUtilPane_l {
    u8 mReserved[0xC];
    gptr<PaneUtilPane_l> mpParent;
    u8 mReserved10[0xC];
    cXyz mTranslation;
    u8 mReserved28[0xC];
    be<f32> mScaleX, mScaleY;
    u8 mReserved3C[0xC];
    Mtx34 mGlobalMatrix;
};
WWHD_OFFSET(PaneUtilPane_l, mpParent, 0xC);
WWHD_OFFSET(PaneUtilPane_l, mTranslation, 0x1C);
WWHD_OFFSET(PaneUtilPane_l, mScaleX, 0x34);
WWHD_OFFSET(PaneUtilPane_l, mGlobalMatrix, 0x48);
class hdPaneUtil_c {
public:
    static void getGlobalTranslation(cXyz* out, PaneUtilPane_l* pane);
    static void getAccumulatedTranslation(cXyz* out, PaneUtilPane_l* pane);
    static Pair32 getAccumulatedScale(PaneUtilPane_l* pane);
};

struct PaneUtilMaterial_l { u8 mReserved[0x1C]; };
struct PaneUtilMaterialList_l { u8 mReserved[0x10]; gptr<PaneUtilMaterial_l> mpMaterials; };
struct PaneUtilAccessor_l { u8 mReserved[8]; gptr<gfn> mpVTable; };
struct PaneUtilString_l { gptr<const be<u8>> mpBuffer; gptr<gfn> mpVTable; };
struct PaneUtilNamedPane_l { u8 mReserved[8]; gptr<gfn> mpVTable; };
struct PaneUtilLayout_l { u8 mReserved[0xC]; gptr<PaneUtilNamedPane_l> mpRootPane; };
struct PaneUtilOwner_l { u8 mReserved[4]; gptr<PaneUtilLayout_l> mpLayout; };
WWHD_OFFSET(PaneUtilMaterialList_l, mpMaterials, 0x10);
WWHD_SIZE(PaneUtilString_l, 8);
void PaneUtilAccessor_destroy(PaneUtilAccessor_l*, u32);
void PaneUtil_delete(void*);
void PaneUtil_lock(void*);
void PaneUtil_unlock(void*);
void PaneUtilMaterial_set(PaneUtilMaterial_l*, void*);
PaneUtilNamedPane_l* PaneUtil_findPane(PaneUtilLayout_l*, const be<u8>*);
