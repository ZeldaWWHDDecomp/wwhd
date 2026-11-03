#pragma once
#include "wwhd.h"
struct JPADynamicsBlockStaticBlock { be<u32> mWords[4]; };
struct JPADynamicsBlockData;
class JPADynamicsBlockArc {
public:
    gptr<void> mVtable;
    gptr<const JPADynamicsBlockData> mpData;
};
WWHD_SIZE(JPADynamicsBlockStaticBlock,16);
WWHD_OFFSET(JPADynamicsBlockArc,mpData,4);
WWHD_SIZE(JPADynamicsBlockArc,8);
void JPADynamicsBlockConstructRegister(void*);
