#pragma once
#include "wwhd.h"
struct JKRDisposerStaticBlock { be<u32> mWords[4]; };
WWHD_SIZE(JKRDisposerStaticBlock,16);
void JKRDisposerConstructRegister(void*);
