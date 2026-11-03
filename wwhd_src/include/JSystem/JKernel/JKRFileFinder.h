#pragma once
#include "wwhd.h"
struct JKRFileFinderStaticBlock { be<u32> mWords[4]; };
WWHD_SIZE(JKRFileFinderStaticBlock,16);
void JKRFileFinderConstructRegister(void*);


#include "JSystem/JSupport/JSUList.h"
void JKRFileFinderListDestroy(JSUPtrList*,u32);
