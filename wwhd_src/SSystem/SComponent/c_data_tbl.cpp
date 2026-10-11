/**
 * Data table formats, names and column-major byte values.
 * WWHD reconstruction in tww style, verified against the original c_data_tbl unit.
 */
#include "SSystem/SComponent/c_data_tbl.h"

/* 0200E808 */
void cDT_NamePTbl::Set(u32 count, gptr<char>* pNames) {
    mCount = count;
    mpNames = pNames;
}

/* 0200E814 */
s32 cDT_NamePTbl::GetIndex(const char* pName, u32 idx) {
    u32 count = mCount;
    if (idx < count) {
        gptr<char>* names = mpNames;
        for (; idx < count; ++idx) {
            const be<u8>* nameBytes = (const be<u8>*)pName;
            const be<u8>* other = (const be<u8>*)names[idx].get();
            u32 i = 0;
            u8 left, right;
            // HD: strcmp is inlined; compare unsigned bytes in the original load order.
            do {
                left = nameBytes[i];
                right = other[i];
                ++i;
            } while (left == right && left != 0);
            if (left == right) return (s32)idx;
        }
    }
    return -1;
}

/* 0200E968 */
void cDT_DataSrc::Set(u32 rowNum, u32 colNum, u8* pData) {
    mColNum = colNum;
    mpData = pData;
    mRowNum = rowNum;
}

/* 0200E978 */
u8 cDT_DataSrc::GetInf(s32 row, s32 col) {
    if (row < 0) return 0xFF;
    u32 rows = mRowNum;
    if ((u32)row >= rows || col < 0 || (u32)col >= mColNum) return 0xFF;
    // HD: row/column arithmetic and the guest data address wrap at 32 bits.
    u32 offset = (u32)row + (u32)col * rows;
    return *gabi::at<be<u8>>(gabi::ea(mpData.get()) + offset);
}

/* 0200EA28 */
void cDT::Set(u32 fmtCount, gptr<char>* pFmt, u32 nameCount, gptr<char>* pName, u8* pData) {
    mFmt.Set(fmtCount, pFmt);
    mName.Set(nameCount, pName);
    mSrc.Set(fmtCount, nameCount, pData);
}

/* 0200EA74 */
u8 cDT::GetInf(s32 row, s32 col) {
    return mSrc.GetInf(row, col);
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 0200E7BC */
static cDT_NamePTbl* cDT_NamePTbl__ct(cDT_NamePTbl* self) {
    if (!self) self = (cDT_NamePTbl*)operator_new(sizeof(cDT_NamePTbl));
    if (self) {
        self->mpNames = nullptr;
        self->__vtbl = 0x10001E60;
        self->mCount = 0;
    }
    return self;
}

/* 0200E870 */
static cDT_Format* cDT_Format__ct(cDT_Format* self) {
    if (!self) self = (cDT_Format*)operator_new(sizeof(cDT_Format));
    if (self) {
        cDT_NamePTbl__ct(self);
        self->__vtbl = 0x10001E70;
    }
    return self;
}

/* 0200E8C4 */
static cDT_Name* cDT_Name__ct(cDT_Name* self) {
    if (!self) self = (cDT_Name*)operator_new(sizeof(cDT_Name));
    if (self) {
        cDT_NamePTbl__ct(self);
        self->__vtbl = 0x10001E80;
    }
    return self;
}

/* 0200E918 */
static cDT_DataSrc* cDT_DataSrc__ct(cDT_DataSrc* self) {
    if (!self) self = (cDT_DataSrc*)operator_new(sizeof(cDT_DataSrc));
    if (self) {
        self->mpData = nullptr;
        self->__vtbl = 0x10001E90;
        self->mRowNum = 0;
        self->mColNum = 0;
    }
    return self;
}

/* 0200E9BC */
static cDT* cDT__ct(cDT* self) {
    if (!self) self = (cDT*)operator_new(sizeof(cDT));
    if (self) {
        cDT_Format__ct(&self->mFmt);
        cDT_Name__ct(&self->mName);
        cDT_DataSrc__ct(&self->mSrc);
    }
    return self;
}

/* 0200EA14 */
static void cDT__dt(cDT* self, s32 flags) {
    if (self && ((u32)flags & 1)) operator_delete(self);
}

/* 0200EA7C */
static void __sinit_c_data_tbl_cpp() {
    sinit_header_statics_rodata(0x101FF564, 0x1018C6FC, 0x101FF570, 0x10001E54);
}

// HD: three header-static-only translation units are hosted here; their original names are unknown.
/* 0200EB10 */
static void __sinit_header_after_data_tbl_first() {
    sinit_header_statics_rodata(0x101FF580, 0x1018C720, 0x101FF58C, 0x10001EA0);
}

/* 0200EBA4 */
static void __sinit_header_after_data_tbl_second() {
    sinit_header_statics_rodata(0x101FF59C, 0x1018C744, 0x101FF5A8, 0x10001EA8);
}

/* 0200EC38 */
static void __sinit_header_after_data_tbl_third() {
    sinit_header_statics_rodata(0x101FF5B8, 0x1018C768, 0x101FF5C4, 0x10001EB0);
}

/* WWHD glue (tools/verify/autoglue.py): no data bindings. */
