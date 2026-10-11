/** Actor data manager. WWHD, verified unit d_s_actor_data_mng. */
#include "d/d_s_actor_data_mng.h"

/* 025AB7C4 */
void dADM_CharTbl::SetUpIndex() {
    mIndex_ARG = cDT_NamePTbl_GetIndex(&mFmt, "ARG", 0);
    mIndex_N_ITEM[0] = cDT_NamePTbl_GetIndex(&mFmt, "N_ITEM0", 0);
    mIndex_N_ITEM[1] = cDT_NamePTbl_GetIndex(&mFmt, "N_ITEM1", 0);
    mIndex_N_ITEM[2] = cDT_NamePTbl_GetIndex(&mFmt, "N_ITEM2", 0);
    mIndex_N_ITEM[3] = cDT_NamePTbl_GetIndex(&mFmt, "N_ITEM3", 0);
    mIndex_N_ITEM[4] = cDT_NamePTbl_GetIndex(&mFmt, "N_ITEM4", 0);
    mIndex_N_ITEM[5] = cDT_NamePTbl_GetIndex(&mFmt, "N_ITEM5", 0);
    mIndex_N_ITEM[6] = cDT_NamePTbl_GetIndex(&mFmt, "N_ITEM6", 0);
    mIndex_N_ITEM[7] = cDT_NamePTbl_GetIndex(&mFmt, "N_ITEM7", 0);
    mIndex_N_ITEM[8] = cDT_NamePTbl_GetIndex(&mFmt, "N_ITEM8", 0);
    mIndex_N_ITEM[9] = cDT_NamePTbl_GetIndex(&mFmt, "N_ITEM9", 0);
    mIndex_N_ITEM[10] = cDT_NamePTbl_GetIndex(&mFmt, "N_ITEM10", 0);
    mIndex_N_ITEM[11] = cDT_NamePTbl_GetIndex(&mFmt, "N_ITEM11", 0);
    mIndex_N_ITEM[12] = cDT_NamePTbl_GetIndex(&mFmt, "N_ITEM12", 0);
    mIndex_N_ITEM[13] = cDT_NamePTbl_GetIndex(&mFmt, "N_ITEM13", 0);
    mIndex_N_ITEM[14] = cDT_NamePTbl_GetIndex(&mFmt, "N_ITEM14", 0);
    mIndex_N_ITEM[15] = cDT_NamePTbl_GetIndex(&mFmt, "N_ITEM15", 0);
    mIndex_percent = cDT_NamePTbl_GetIndex(&mFmt, "percent", 0);
    mIndex_ITEM[0] = cDT_NamePTbl_GetIndex(&mFmt, "ITEM0", 0);
    mIndex_ITEM[1] = cDT_NamePTbl_GetIndex(&mFmt, "ITEM1", 0);
    mIndex_ITEM[2] = cDT_NamePTbl_GetIndex(&mFmt, "ITEM2", 0);
    mIndex_ITEM[3] = cDT_NamePTbl_GetIndex(&mFmt, "ITEM3", 0);
    mIndex_ITEM[4] = cDT_NamePTbl_GetIndex(&mFmt, "ITEM4", 0);
    mIndex_ITEM[5] = cDT_NamePTbl_GetIndex(&mFmt, "ITEM5", 0);
    mIndex_ITEM[6] = cDT_NamePTbl_GetIndex(&mFmt, "ITEM6", 0);
    mIndex_ITEM[7] = cDT_NamePTbl_GetIndex(&mFmt, "ITEM7", 0);
}

/* 025ABA58 */
dADM_CharTbl* dADM_CharTbl__ct(dADM_CharTbl* i_this) {
    if (i_this == NULL) {
        i_this = static_cast<dADM_CharTbl*>(ADM_allocate(sizeof(dADM_CharTbl)));
    }
    if (i_this != NULL) {
        cDT__ct(i_this);
    }
    return i_this;
}

/* 025ABAA0 */
void dADM_CharTbl__dt(dADM_CharTbl* i_this, u32 flags) {
    if (i_this != NULL) {
        cDT__dt(i_this, 0);
        if (flags & 1) {
            ADM_delete(i_this);
        }
    }
}

/* 025ABAF4 */
void dADM_CharTbl::SetData(u32 offs, u32 row_num, u32 row_offs, u32 colum_num,
                           u32 colum_offs, u32 dat_size, u32 data_offs) {
    be<u32>* row = gabi::at<be<u32>>(row_offs);
    for (u32 i = 0; i < row_num; ++i) {
        row[i] = u32(row[i]) + offs;
    }
    be<u32>* colum = gabi::at<be<u32>>(colum_offs);
    for (u32 i = 0; i < colum_num; ++i) {
        colum[i] = u32(colum[i]) + offs;
    }
    if (dat_size != row_num * colum_num) {
        // HD: assertion line 0x3A rather than GameCube's 0x39.
        ADM_assert("d_s_actor_data_mng.cpp: dADM_CharTbl", 0x3A, "dat_size == row_num * colum_num");
    }
    cDT_Set(this, row_num, row_offs, colum_num, colum_offs, data_offs);
    SetUpIndex();
}

/* 025ABBBC */
s32 dADM_CharTbl::GetNameIndex2(u32 pName, u32 index) {
    u32 start = 0;
    for (;;) {
        s32 found = cDT_NamePTbl_GetIndex(&mName, gabi::at<const char>(pName), start);
        if (found == -1) {
            return -1;
        }
        if (cDT_GetInf(this, mIndex_ARG, u32(found)) == index) {
            return found;
        }
        start = u32(found) + 1;
    }
}

static u32 ADM_vtable;

/* 025ABC48 */
dADM* dADM__ct(dADM* i_this) {
    if (i_this == NULL) {
        i_this = static_cast<dADM*>(ADM_allocate(sizeof(dADM)));
    }
    if (i_this != NULL) {
        // HD: install the vtable before constructing the embedded character table.
        i_this->__vtbl = gabi::ea(&ADM_vtable);
        dADM_CharTbl__ct(&i_this->mCharTbl);
        i_this->mpData = NULL;
        i_this->mBlockCount = 0;
    }
    return i_this;
}

/* 025ABCA8 */
void dADM__dt(dADM* i_this, u32 flags) {
    if (i_this != NULL) {
        dADM_CharTbl__dt(&i_this->mCharTbl, 2);
        if (flags & 1) {
            ADM_delete(i_this);
        }
    }
}

/* 025ABCFC */
s32 dADM::FindTag(u32 tag, be<u32>* pSize, be<u32>* pOffs) {
    s32 count = mBlockCount;
    ADM_Block* pData = mpData;
    for (s32 i = 0; i < count; ++i, ++pData) {
        if (pData->mTag == tag) {
            *pSize = pData->mSize;
            *pOffs = pData->mOffset;
            return TRUE;
        }
    }
    return FALSE;
}

/* 025ABD4C */
void dADM::SetData(void* pData) {
    s32 count = *static_cast<be<s32>*>(pData);
    mBlockCount = count;
    ADM_Block* pHeader = reinterpret_cast<ADM_Block*>(static_cast<be<u32>*>(pData) + 1);
    mpData = pHeader;
    for (s32 i = 0; i < count; ++i, ++pHeader) {
        pHeader->mOffset = u32(pHeader->mOffset) + gabi::ea(pData);
        // HD: overlapping input may change the loop bound while relocating blocks.
        count = mBlockCount;
    }
    gabi::Local<ADM_Outputs> out;
    if (!FindTag(ADM_ACFN, &out->row, &out->rowOffs)) {
        return;
    }
    if (!FindTag(ADM_ACNA, &out->name, &out->nameOffs)) {
        return;
    }
    if (!FindTag(ADM_ACDS, &out->dat_size, &out->dataOffs)) {
        return;
    }
    u32 row = out->row;
    u32 name = out->name;
    u32 dat_size = out->dat_size;
    if (row * name != dat_size) {
        // HD: assertion line 0xD7 rather than GameCube's 202.
        ADM_assert("d_s_actor_data_mng.cpp: dADM", 0xD7, "row * name == dat_size");
        row = out->row;
        name = out->name;
        dat_size = out->dat_size;
    }
    u32 nameOffs = out->nameOffs;
    u32 rowOffs = out->rowOffs;
    u32 dataOffs = out->dataOffs;
    mCharTbl.SetData(gabi::ea(pData), row, rowOffs, name, nameOffs, dat_size, dataOffs);
}

/* 025ABE6C */
// HD: this translation unit's header statics and destructor registrations.
static void __sinit_d_s_actor_data_mng_cpp() {
    sinit_header_statics_rodata(0x1047B360, 0x101EA5CC, 0x1047B36C, 0x100526BC);
}

/* WWHD glue (tools/verify/autoglue.py): format-name strings, vtable, and cDT runtime calls
str 100525C0
str 10052568
str 10052570
str 10052578
str 10052580
str 10052588
str 10052590
str 10052598
str 100525A0
str 100525A8
str 100525B0
str 10052604
str 10052610
str 1005261C
str 10052628
str 10052634
str 10052640
str 100525B8
str 100525C4
str 100525CC
str 100525D4
str 100525DC
str 100525E4
str 100525EC
str 100525F4
str 100525FC
str 1005266C "d_s_actor_data_mng.cpp: dADM_CharTbl" renamed
str 10052688 "d_s_actor_data_mng.cpp: dADM" renamed
str 1005264C "dat_size == row_num * colum_num" renamed
str 100526A0 "row * name == dat_size" renamed
data ADM_vtable 100526C8
ext 0273AD10 void* ADM_allocate(u32)
ext 0273AF40 void ADM_delete(void*)
ext 0200E9BC void cDT__ct(cDT*)
ext 0200EA14 void cDT__dt(cDT*, u32)
ext 0200E814 s32 cDT_NamePTbl_GetIndex(cDT_NamePTbl*, const char*, u32)
ext 0200EA28 void cDT_Set(cDT*, u32, u32, u32, u32, u32)
ext 0200EA74 u32 cDT_GetInf(cDT*, u32, u32)
ext 0273AA24 void ADM_assert(const char*, s32, const char*)
*/
