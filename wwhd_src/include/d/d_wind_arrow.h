/** Wind arrow. */
#pragma once
#include "bindings.h"
#include "f_op/f_op_kankyo.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_kankyo_wether.h"

struct WindArrowHeapVtable {
    u8 _00[0x5C]; gfn getStartAddr;
    u8 _60[0xC]; gfn getHeapSize;
};
struct WindArrowSolidHeap {
    u8 _00[0xC]; gptr<WindArrowHeapVtable> __vtbl;
    gptr<const char> mName;
};
// HD: invisible model occupies eight bytes; the animation occupies 0x74.
struct mDoExt_invisibleModel { u8 storage[8]; };
struct dWindArrow_modelInfo_c {
    gptr<J3DModel> mpModel;
    mDoExt_invisibleModel mInvisibleModel;
    mDoExt_btkAnm mBtkAnm;
};
struct dWindArrow_c : kankyo_class {
    gptr<WindArrowSolidHeap> heap;
    dWindArrow_modelInfo_c mModelInfo;
    u32 draw(); u32 execute(); u32 createHeap(); void adjustHeap(); cPhs_State create();
};
WWHD_OFFSET(dWindArrow_c, heap, 0xFC);
WWHD_OFFSET(dWindArrow_c, mModelInfo, 0x100);
WWHD_SIZE(dWindArrow_modelInfo_c, 0x80);
struct WindArrowModelData {
    u8 _00[0x44]; be<u32> mpTextureSRTAnimator;
};
struct WindArrowDrawBuffers { gptr<void> mpOpa; gptr<void> mpXlu; };
struct WindArrowPlayLists {
    u8 _00[0x5D58]; gptr<void> mpP1Opa;
    u8 _5D5C[4]; gptr<void> mpP1Xlu;
    u8 _5D64[0x14]; gptr<void> mpOpa; gptr<void> mpXlu;
    u8 _5D80[4]; gptr<void> mpMaskOffOpa; gptr<void> mpMaskOffXlu;
};
inline WindArrowPlayLists* dWindArrow_getLists() {
    return reinterpret_cast<WindArrowPlayLists*>(dComIfGp_get());
}
struct WindArrowHeaderStatics {
    be<f32> negativePi, positivePi;
    u8 firstObject, secondObject, _0A[2];
    be<u32> zero[4];
};
struct WindArrowPiConstants { be<f32> negativePi, positivePi; };
void* dWindArrow_alloc(u32);
void dWindArrow_free(void*);
void mDoExt_invisibleModel_ct(mDoExt_invisibleModel*);
void mDoExt_invisibleModel_dt(mDoExt_invisibleModel*, u32);
u32 mDoExt_invisibleModel_create(mDoExt_invisibleModel*, J3DModel*);
void mDoExt_invisibleModel_remove(mDoExt_invisibleModel*);
void mDoExt_invisibleModel_entryMaskOff(mDoExt_invisibleModel*);
void dWindArrow_base_ct(dWindArrow_c*);
void dWindArrow_base_dt(dWindArrow_c*, u32);
WindArrowSolidHeap* mDoExt_createSolidHeapFromGameToCurrent(u32, u32);
void mDoExt_destroySolidHeap(WindArrowSolidHeap*);
void mDoExt_restoreCurrentHeap();
s32 mDoExt_adjustSolidHeap(WindArrowSolidHeap*);
void DCStoreRangeNoSync(u32, u32);
void* dWindArrow_getRes(dRes_control_c*, SafeString*, u32);
void dWindArrow_assert(const char*, u32, const char*);
struct WindArrowWindPower { be<f32> mPower; };
WWHD_SIZE(WindArrowWindPower, 4);
WindArrowWindPower* dKyw_get_wind_power();
void dWindArrow_firstHeader_ct(u8*);
void dWindArrow_secondHeader_ct(u8*);
dWindArrow_modelInfo_c* dWindArrow_modelInfo_ct(dWindArrow_modelInfo_c*);
void dWindArrow_modelInfo_dt(dWindArrow_modelInfo_c*, u32);
dWindArrow_c* dWindArrow_ct(dWindArrow_c*);
void dWindArrow_dt(dWindArrow_c*, u32);

WWHD_OFFSET(WindArrowHeapVtable, getStartAddr, 0x5C);
WWHD_OFFSET(WindArrowHeapVtable, getHeapSize, 0x6C);
WWHD_SIZE(WindArrowHeapVtable, 0x70);
WWHD_OFFSET(WindArrowSolidHeap, __vtbl, 0xC);
WWHD_OFFSET(WindArrowSolidHeap, mName, 0x10);
WWHD_SIZE(WindArrowSolidHeap, 0x14);
WWHD_SIZE(mDoExt_invisibleModel, 8);
WWHD_OFFSET(dWindArrow_modelInfo_c, mInvisibleModel, 4);
WWHD_OFFSET(dWindArrow_modelInfo_c, mBtkAnm, 0xC);
WWHD_SIZE(dWindArrow_c, 0x180);
WWHD_OFFSET(WindArrowModelData, mpTextureSRTAnimator, 0x44);
WWHD_SIZE(WindArrowModelData, 0x48);
WWHD_SIZE(WindArrowDrawBuffers, 8);
WWHD_OFFSET(WindArrowPlayLists, mpP1Opa, 0x5D58);
WWHD_OFFSET(WindArrowPlayLists, mpP1Xlu, 0x5D60);
WWHD_OFFSET(WindArrowPlayLists, mpOpa, 0x5D78);
WWHD_OFFSET(WindArrowPlayLists, mpXlu, 0x5D7C);
WWHD_OFFSET(WindArrowPlayLists, mpMaskOffOpa, 0x5D84);
WWHD_OFFSET(WindArrowPlayLists, mpMaskOffXlu, 0x5D88);
WWHD_SIZE(WindArrowPlayLists, 0x5D8C);
WWHD_OFFSET(WindArrowHeaderStatics, firstObject, 8);
WWHD_OFFSET(WindArrowHeaderStatics, zero, 0xC);
WWHD_SIZE(WindArrowHeaderStatics, 0x1C);
WWHD_SIZE(WindArrowPiConstants, 8);
