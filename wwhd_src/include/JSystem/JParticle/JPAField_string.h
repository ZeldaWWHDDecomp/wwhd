// Compiler-emitted string helpers in the Cafe field translation unit.
#pragma once
#include "d/d_com_inf_game.h"
struct JPAFieldBufferedString {
    union {
        SafeString mBase;
        struct { gptr<be<u8>> mBuffer; be<u32> __vtbl; };
    };
    be<u32> mBufferSize;
    void assureTermination();
};
WWHD_SIZE(JPAFieldBufferedString, 0xC);
WWHD_OFFSET(JPAFieldBufferedString, mBufferSize, 8);
