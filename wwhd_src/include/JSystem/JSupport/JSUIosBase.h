#pragma once
#include "wwhd.h"
struct JSUIosBaseStaticBlock { be<u32> mWords[4]; };
WWHD_SIZE(JSUIosBaseStaticBlock,16);
void JSUIosBaseConstructRegister(void*);
