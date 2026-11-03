#pragma once
#include "agl/gx2/GpuUtilitySupport.h"
namespace uniform_update_support {
struct UniformResource_l {
    u8 mHeader[0x12]; be<u16> mConversionCount; u8 mConversionsHeader[0x20];
    be<s32> mConversionsRelative; u8 mMaskHeader[12]; be<s32> mDefaultMaskRelative;
};
struct ConvertedUniform { be<u32> mWords[16]; };
struct MaterialUniforms_l {
    gptr<UniformResource_l> mResource; be<u16> mFlags; u8 mHeader[0x22];
    gptr<be<u8>> mSource; gptr<void> mTextures; gptr<void> mConversionContext;
    u8 mTail[8];
    void writeSwapped(be<u8>*,be<u32>*);
    void writeNative(be<u8>*,be<u32>*);
};
WWHD_OFFSET(UniformResource_l,mConversionCount,0x12);
WWHD_OFFSET(UniformResource_l,mConversionsRelative,0x34);
WWHD_OFFSET(UniformResource_l,mDefaultMaskRelative,0x44); WWHD_SIZE(UniformResource_l,0x48);
WWHD_SIZE(ConvertedUniform,64); WWHD_OFFSET(MaterialUniforms_l,mSource,0x28);
WWHD_OFFSET(MaterialUniforms_l,mConversionContext,0x30); WWHD_SIZE(MaterialUniforms_l,0x3C);
}
void uniformOutputCopy(void*,const void*,u32);
