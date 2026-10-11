/* m_Do_ext (GameCube names) beyond m_Do/m_Do_ext.h. */
#pragma once
#include "bindings.h"
#include "gc_api/tu.h"

struct ResTIMG;

/* mDoExt_3DlineMat1_c, HD 0x188 (GameCube 0x3C): vtable at +0x130, line array at +0x184
 * (each line: +0 segment positions; GameCube stride 0x18) [v nz, nz enemy, bmdhand] */
struct mDoExt_3DlineMat1_c {
    /* 0x000 */ u8 _000[0x130];
    /* 0x130 */ be<u32> __vtbl;
    /* 0x134 */ u8 _134[0x184 - 0x134];
    /* 0x184 */ be<u32> mpLines;

    /* 025EBA58 init(numLines, numSegments, img, hasSize) [v nz] */
    BOOL init(u16 numLines, u16 numSegments, ResTIMG* img, BOOL hasSize) {
        return gabi::call<BOOL>(0x025EBA58, this, numLines, numSegments, img, hasSize);
    }
    /* 025EC62C update(segs, size, color, space, tevStr): HD passes the size in f1 last [v nz] */
    void update(u16 segs, f32 size, const GXColor& color, u16 space, dKy_tevstr_c* tevStr) {
        gabi::call(0x025EC62C, this, segs, &color, space, tevStr, size);
    }
    cXyz* getPos(int i) { return gabi::at<cXyz>(gabi::load<u32>(mpLines + (u32)i * 0x18)); }
    /* virtual getMaterialID (vtable +0x14) */
    int getMaterialID() { return gabi::call_ptr<int>(gabi::load<u32>(__vtbl + 0x14), this); }
};
WWHD_SIZE(mDoExt_3DlineMat1_c, 0x188);
/* compiler-generated construction/destruction (HD): 025EB82C constructor, 025EB8B8 destructor
 * (matcher: "mDoExt_3DlineMat1_c::draw") */
inline void __ct(mDoExt_3DlineMat1_c* m) { gabi::call(0x025EB82C, m); }
inline void __dt(mDoExt_3DlineMat1_c* m) { gabi::call(0x025EB8B8, m, 2); }

/* Thunder's heap and opaque animation/invisibility wrappers [v d_ky_thunder]. */
/* 025E2F64 */
inline JKRHeap* mDoExt_getGameHeap(u32 index) { return gabi::call<JKRHeap*>(0x025E2F64, index); }
/* 025E35BC */
inline JKRSolidHeap* mDoExt_createSolidHeapToCurrent(u32 size, JKRHeap* parent, u32 alignment) { return gabi::call<JKRSolidHeap*>(0x025E35BC, size, parent, alignment); }
/* 025E3868 */
inline void mDoExt_destroySolidHeap(JKRSolidHeap* heap) { gabi::call(0x025E3868, heap); }
/* 025E3678 */
inline s32 mDoExt_adjustSolidHeap(JKRSolidHeap* heap) { return gabi::call<s32>(0x025E3678, heap); }
/* 025E895C / 025E89F8 / 025E8A48 / 025E8CD8 */
inline void mDoExt_invisibleModel_ct(void* p) { gabi::call(0x025E895C, p); }
inline void mDoExt_invisibleModel_dt(void* p) { gabi::call(0x025E89F8, p, 2u); }
inline u32 mDoExt_invisibleModel_create(void* p, J3DModel* model) { return gabi::call<u32>(0x025E8A48, p, model); }
inline void mDoExt_invisibleModel_entryMaskOff(void* p) { gabi::call(0x025E8CD8, p); }
/* HD current-game heap pointer at 1047C8B4 [v d_ky_thunder]. */
inline JKRHeap* mDoExt_getCurrentGameHeap() { return gabi::at<JKRHeap>(gabi::load<u32>(0x1047C8B4)); }
/* HD inline removal: model data's btk/brk pointers at +0x44/+0x48 [v d_ky_thunder]. */
inline void mDoExt_btkAnm_remove(J3DModelData* data) { gabi::store<u32>(gabi::ea(data) + 0x44, 0); }
inline void mDoExt_brkAnm_remove(J3DModelData* data) { gabi::store<u32>(gabi::ea(data) + 0x48, 0); }

/* HD constructors return the constructed pointer [v d_a_bmd]. */
/* 025E80D0 */
inline mDoExt_brkAnm* __ct(mDoExt_brkAnm* p) { return gabi::call<mDoExt_brkAnm*>(0x025E80D0, p); }
/* 025E7C6C */
inline mDoExt_btkAnm* __ct(mDoExt_btkAnm* p) { return gabi::call<mDoExt_btkAnm*>(0x025E7C6C, p); }
