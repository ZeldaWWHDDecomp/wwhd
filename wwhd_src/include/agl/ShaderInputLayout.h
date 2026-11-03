/* WWHD compilation records: the 0x48 allocation belongs to the constructor
 * forward. Variant-weight accumulation has a separate AGL#15-owned domain view. */
#pragma once
#include "sead/Foundation.h"
namespace agl {
struct ShaderVariation_l;
struct ShaderVariantRadixView_l {
    u8 mPrefix[0x18];
    be<u16> mDivisor;
    u8 mPadding[2];
};
// Borrowed from the active AGL#15 ShaderVariantDomain declaration, pending shared fold.
struct ShaderVariantDomainView_l {
    u8 mPrefix[12];
    be<u32> mCount;
    gptr<ShaderVariantRadixView_l> mRadices;
    u32 weightedSum(const be<u32>* values);
};
struct ShaderInputLayout_l {
    gptr<be<u8>> mSourceData;
    u8 mPrefixRemainder[4];
    gptr<void> mProgramData;
    u8 mReserved[4];
    be<s32> mNameCount, mNameCapacity;
    gptr<gptr<be<u8>>> mNames;
    be<s32> mValueCount, mValueCapacity;
    gptr<gptr<be<u8>>> mValues;
    u8 mTail[0x44-0x28];
    gptr<ShaderVariation_l> mVariation;
    static ShaderInputLayout_l* construct(ShaderInputLayout_l* layout);
    static void destroy(ShaderInputLayout_l* layout, u32 flags);
};
WWHD_OFFSET(ShaderVariantRadixView_l,mDivisor,0x18);
WWHD_SIZE(ShaderVariantRadixView_l,0x1C);
WWHD_OFFSET(ShaderVariantDomainView_l,mCount,0xC);
WWHD_OFFSET(ShaderVariantDomainView_l,mRadices,0x10);
WWHD_SIZE(ShaderVariantDomainView_l,0x14);
WWHD_OFFSET(ShaderInputLayout_l,mProgramData,8);
WWHD_OFFSET(ShaderInputLayout_l,mNameCount,0x10);
WWHD_OFFSET(ShaderInputLayout_l,mNames,0x18);
WWHD_OFFSET(ShaderInputLayout_l,mValueCount,0x1C);
WWHD_OFFSET(ShaderInputLayout_l,mValues,0x24);
WWHD_OFFSET(ShaderInputLayout_l,mVariation,0x44);
WWHD_SIZE(ShaderInputLayout_l,0x48);
}
