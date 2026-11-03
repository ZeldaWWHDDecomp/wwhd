#ifndef WWHD_JASINSTEFFECT_H
#define WWHD_JASINSTEFFECT_H
#include "wwhd.h"
using namespace gabi;
namespace JASystem {
// HD: target precedes the vtable pointer.
class TInstEffect {
public:
    void setTarget(s32 target);
    be<u8> mTarget;
    u8 padding[3];
    gptr<void> mVtable;
};
}
WWHD_OFFSET(JASystem::TInstEffect, mTarget, 0);
WWHD_OFFSET(JASystem::TInstEffect, mVtable, 4);
WWHD_SIZE(JASystem::TInstEffect, 8);
#endif
