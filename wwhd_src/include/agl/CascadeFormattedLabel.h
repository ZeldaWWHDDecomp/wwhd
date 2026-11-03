// Fixed label and GHS argument-save envelope from the assigned formatting constructor.
#pragma once
#include "sead/RuntimeFormatSupport.h"
namespace agl {
struct CascadeFormatSave {
    sead::format::VariadicState mArguments;
    u8 mPadding[4];
    be<u32> mIntegerArguments[8];
    be<u32> mFloatingArgumentWords[16];
};
struct CascadeFormattedLabel : sead::format::ByteTextBuffer {
    be<u8> mStorage[32];
    static CascadeFormattedLabel* createFormatted(CascadeFormattedLabel* label, const char* format,
        u32 firstArgument, u32 secondArgument, u32 thirdArgument,
        u32 fourthArgument, u32 fifthArgument, u32 sixthArgument);
};
}
WWHD_SIZE(agl::CascadeFormatSave, 112);
WWHD_OFFSET(agl::CascadeFormatSave, mIntegerArguments, 16);
WWHD_OFFSET(agl::CascadeFormatSave, mFloatingArgumentWords, 48);
WWHD_OFFSET(agl::CascadeFormattedLabel, mStorage, 12);
WWHD_SIZE(agl::CascadeFormattedLabel, 44);
