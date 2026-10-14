/* HD resource conversion tool path globals. Private decomp source. */
#pragma once
#include "bindings.h"

struct hdResourceToolPathsHeaderZero_c { be<u32> mWords[4]; };
WWHD_SIZE(hdResourceToolPathsHeaderZero_c,16);
struct hdResourceToolPathsHeaderDescriptor_c { u8 mData[12]; };
WWHD_SIZE(hdResourceToolPathsHeaderDescriptor_c,12);
