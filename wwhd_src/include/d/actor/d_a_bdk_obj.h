#pragma once
#include "d/actor/d_a_bdk.h"
#include "d/d_save.h"

// HD: model's base transform begins at +0xC8; retain the full load-before-store copy.
struct BdkObjectModel { u8 mPrefix[0xC8]; Mtx34 mBaseTRMtx; };
WWHD_OFFSET(BdkObjectModel, mBaseTRMtx, 0xC8);
WWHD_SIZE(BdkObjectModel, 0xF8);
static inline void bdkObject_setBaseTRMtx(J3DModel* model, const Mtx34* source) {
    Mtx34* destination = &reinterpret_cast<BdkObjectModel*>(model)->mBaseTRMtx;
    f32 elements[3][4];
    for (u32 row = 0; row < 3; ++row)
        for (u32 column = 0; column < 4; ++column)
            elements[row][column] = source->m[row][column];
    for (u32 row = 0; row < 3; ++row)
        for (u32 column = 0; column < 4; ++column)
            destination->m[row][column] = elements[row][column];
}
struct BdkObjectSaveSystem { u8 mPrefix[0x20]; dSv_info_c mSave; };
WWHD_OFFSET(BdkObjectSaveSystem, mSave.mMemory.mMembit, 0x798);
struct BdkObjectSavePointer { gptr<BdkObjectSaveSystem> mpSave; };
WWHD_SIZE(BdkObjectSavePointer, 4);
struct BdkObjectGuard { be<u32> mInitialized; };
WWHD_SIZE(BdkObjectGuard, 4);
BOOL BdkObject_isDungeonItem(dSv_memBit_c*, s32);
