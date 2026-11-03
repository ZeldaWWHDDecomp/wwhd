#pragma once
#include "JSystem/J3DGraph/PipelineConversions.h"
namespace j3d::pipeline {
union ByteColorSlot_l { ByteColor mColor; be<u32> mBits; };
// Consumed register-bank prefix: four 8-byte colors followed by four 4-byte colors.
struct PipelineColorRegisters {
    u8 mPrefix[0x18];
    SignedColor mSignedColors[4];
    ByteColorSlot_l mByteColors[4];
    void setSignedColor(u32,const SignedColor*);
    SignedColor* signedColor(u32);
    void setByteColor(u32,const ByteColor*);
    ByteColor* byteColor(u32);
};
}
WWHD_SIZE(j3d::pipeline::ByteColorSlot_l,4);
WWHD_OFFSET(j3d::pipeline::PipelineColorRegisters,mSignedColors,0x18);
WWHD_OFFSET(j3d::pipeline::PipelineColorRegisters,mByteColors,0x38);
WWHD_SIZE(j3d::pipeline::PipelineColorRegisters,0x48);
