#pragma once
#include "wwhd.h"
struct JPAKeyBlockStaticBlock { be<u32> mWords[4]; };
struct JPAKeyBlockData;
class JPAKeyBlockArc {
public:
    gptr<void> mVtable;
    gptr<const JPAKeyBlockData> mpData;
    gptr<const be<f32>> mpKeyData;
};
WWHD_SIZE(JPAKeyBlockStaticBlock,16);
WWHD_OFFSET(JPAKeyBlockArc,mpData,4);
WWHD_SIZE(JPAKeyBlockArc,12);
void JPAKeyBlockConstructRegister(void*);
