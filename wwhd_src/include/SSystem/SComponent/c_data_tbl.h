/* Data tables: tww names and WWHD layouts (GHS trailing virtual tables).
 * HD/glue: getters omit host const qualification for VERIFY member-pointer registration.
 * Guest char** is represented by a pointer to four-byte gptr<char> entries. */
#pragma once
#include "bindings.h"

struct cDT_NamePTbl {
    /* 0x00 */ be<u32> mCount;
    /* 0x04 */ gptr<gptr<char>> mpNames;
    /* 0x08 */ be<u32> __vtbl;
    void Set(u32 count, gptr<char>* pNames);
    // HD: the starting index is unsigned.
    s32 GetIndex(const char* pName, u32 idx);
};
WWHD_OFFSET(cDT_NamePTbl, mpNames, 4);
WWHD_OFFSET(cDT_NamePTbl, __vtbl, 8);
WWHD_SIZE(cDT_NamePTbl, 12);
struct cDT_Format : cDT_NamePTbl {};
struct cDT_Name : cDT_NamePTbl {};
WWHD_SIZE(cDT_Format, 12);
WWHD_SIZE(cDT_Name, 12);

struct cDT_DataSrc {
    /* 0x00 */ be<u32> mRowNum;
    /* 0x04 */ be<u32> mColNum;
    /* 0x08 */ gptr<u8> mpData;
    /* 0x0C */ be<u32> __vtbl;
    void Set(u32 rowNum, u32 colNum, u8* pData);
    u8 GetInf(s32 row, s32 col);
};
WWHD_OFFSET(cDT_DataSrc, mColNum, 4);
WWHD_OFFSET(cDT_DataSrc, mpData, 8);
WWHD_OFFSET(cDT_DataSrc, __vtbl, 12);
WWHD_SIZE(cDT_DataSrc, 16);

struct cDT {
    /* 0x00 */ cDT_Format mFmt;
    /* 0x0C */ cDT_Name mName;
    /* 0x18 */ cDT_DataSrc mSrc;
    void Set(u32 fmtCount, gptr<char>* pFmt, u32 nameCount, gptr<char>* pName, u8* pData);
    u8 GetInf(s32 row, s32 col);
};
WWHD_OFFSET(cDT, mName, 12);
WWHD_OFFSET(cDT, mSrc, 24);
WWHD_SIZE(cDT, 40);
