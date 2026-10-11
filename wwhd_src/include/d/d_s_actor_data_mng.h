/* Actor data manager. WWHD layouts. */
#pragma once
#include "bindings.h"

#include "SSystem/SComponent/c_data_tbl.h"

class dADM_CharTbl : public cDT {
public:
    be<u32> mIndex_ARG;
    be<u32> mIndex_N_ITEM[16];
    be<u32> mIndex_percent;
    be<u32> mIndex_ITEM[8];
    void SetUpIndex();
    void SetData(u32, u32, u32, u32, u32, u32, u32);
    s32 GetNameIndex2(u32, u32);
};
WWHD_OFFSET(dADM_CharTbl, mIndex_ARG, 0x28);
WWHD_OFFSET(dADM_CharTbl, mIndex_N_ITEM, 0x2C);
WWHD_OFFSET(dADM_CharTbl, mIndex_percent, 0x6C);
WWHD_OFFSET(dADM_CharTbl, mIndex_ITEM, 0x70);
WWHD_SIZE(dADM_CharTbl, 0x90);

struct ADM_Block {
    be<u32> mTag;
    be<u32> mSize;
    be<u32> mOffset;
};
WWHD_SIZE(ADM_Block, 0xC);
class dADM {
public:
    be<s32> mBlockCount;
    gptr<ADM_Block> mpData;
    dADM_CharTbl mCharTbl;
    // HD: the virtual-destructor table follows the embedded character table.
    be<u32> __vtbl;
    s32 FindTag(u32, be<u32>*, be<u32>*);
    void SetData(void*);
};
WWHD_OFFSET(dADM, mpData, 4);
WWHD_OFFSET(dADM, mCharTbl, 8);
WWHD_OFFSET(dADM, __vtbl, 0x98);
WWHD_SIZE(dADM, 0x9C);

// HD: explicit GHS allocating/deleting entry points preserve construction order and flags.
dADM_CharTbl* dADM_CharTbl__ct(dADM_CharTbl*);
void dADM_CharTbl__dt(dADM_CharTbl*, u32);
dADM* dADM__ct(dADM*);
void dADM__dt(dADM*, u32);
void* ADM_allocate(u32);
void ADM_delete(void*);
void cDT__ct(cDT*);
void cDT__dt(cDT*, u32);
s32 cDT_NamePTbl_GetIndex(cDT_NamePTbl*, const char*, u32);
void cDT_Set(cDT*, u32, u32, u32, u32, u32);
u32 cDT_GetInf(cDT*, u32, u32);
void ADM_assert(const char*, s32, const char*);

struct ADM_Outputs {
    be<u32> row, name, dat_size;
    be<u32> rowOffs, nameOffs, dataOffs;
};
WWHD_SIZE(ADM_Outputs, 0x18);
enum ADM_Tag : u32 {
    ADM_ACFN = 0x4143464E,
    ADM_ACNA = 0x41434E41,
    ADM_ACDS = 0x41434453,
};
