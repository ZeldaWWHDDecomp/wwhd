// Separate method-table instances of the environment's formatted text.
#pragma once
#include "sead/FileDevice.h"
namespace agl { namespace env {
struct EnvironmentShortFormattedText : sead::FixedSafeText<256> {
    static EnvironmentShortFormattedText* construct(EnvironmentShortFormattedText*,const char*,u32,u32,u32,u32,u32,u32);
};
struct EnvironmentManagerFormattedText : sead::FixedSafeText<1024> {
    static EnvironmentManagerFormattedText* construct(EnvironmentManagerFormattedText*,const char*,u32,u32,u32,u32,u32,u32);
};
struct EnvironmentSetFormattedText : sead::FixedSafeText<1024> {
    static EnvironmentSetFormattedText* construct(EnvironmentSetFormattedText*,const char*,u32,u32,u32,u32,u32,u32);
};
// The environment object's icon label text (0277077C, AGL#2; method tables 101607F4 -> 1016084C -> 1016087C).
struct EnvironmentObjectFormattedText : sead::FixedSafeText<1024> {
    static EnvironmentObjectFormattedText* construct(EnvironmentObjectFormattedText*,const char*,u32,u32,u32,u32,u32,u32);
};
}}
WWHD_SIZE(agl::env::EnvironmentManagerFormattedText,0x40C);
WWHD_SIZE(agl::env::EnvironmentSetFormattedText,0x40C);
WWHD_SIZE(agl::env::EnvironmentObjectFormattedText,0x40C);

WWHD_SIZE(agl::env::EnvironmentShortFormattedText,0x10C);
