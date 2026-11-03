#pragma once
#include "wwhd.h"
struct JMathStaticBlock { be<u32> mWords[4]; };
WWHD_SIZE(JMathStaticBlock,16);
void JMathConstructRegister(void*);


void JMAQuatLerp(Quaternion*,Quaternion*,f32,Quaternion*);
