// Local WWHD parameter interfaces reconstructed from their instruction bodies.
#pragma once
#include "sead/FileDevice.h"
namespace agl {
struct ParameterHeader {
    be<u32> mNameHash;
    gptr<ParameterHeader> mNext;
    be<u32> mVtable;
};
struct ByteParameter : ParameterHeader {
    be<u8> mValue;
    u8 _padding[3];
    u32 kindZero();
    void* data();
    void* dataConst();
    u32 byteSize();
};
struct WordParameter : ParameterHeader {
    be<u32> mValue;
    u32 kindWord();
    void* data();
    void* dataConst();
    u32 byteSize();
};
struct FloatParameter : ParameterHeader {
    be<f32> mValue;
    u32 kindFloat();
    void* data();
    void* dataConst();
    u32 byteSize();
};
struct PairParameter : ParameterHeader {
    be<f32> mValue[2];
    u32 kindPair();
    void* data();
    void* dataConst();
    u32 byteSize();
};
}
WWHD_OFFSET(agl::ByteParameter,mValue,0xC);
WWHD_SIZE(agl::ByteParameter,0x10);
WWHD_OFFSET(agl::WordParameter,mValue,0xC);
WWHD_SIZE(agl::WordParameter,0x10);
WWHD_OFFSET(agl::FloatParameter,mValue,0xC);
WWHD_SIZE(agl::FloatParameter,0x10);
WWHD_OFFSET(agl::PairParameter,mValue,0xC);
WWHD_SIZE(agl::PairParameter,0x14);
