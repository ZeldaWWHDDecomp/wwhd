#pragma once
#include "wwhd.h"
struct JKRHeap_currentStaticBlock { be<u32> mWords[4]; };
WWHD_SIZE(JKRHeap_currentStaticBlock,16);
void JKRHeap_currentConstructRegister(void*);


#include "JSystem/JKernel/JKRHeap_base.h"
