/* cBgS_LinChk: GameCube layout, unchanged in HD. The unit-local implementation
 * derives from the shared layout so its methods need no shared header changes.
 * HD: the Native suffix distinguishes this executable view from that layout. */
#pragma once
#include "bindings.h"

struct cBgS_vectorWords {
    be<u32> x, y, z;
};
WWHD_SIZE(cBgS_vectorWords, 0xC);

class cBgS_LinChkNative : public ::cBgS_LinChk {
public:
    cBgS_LinChkNative();
    void ct();
    void Set2(cXyz* pi_start, cXyz* pi_end, fpc_ProcID pi_actor);
    enum { HIT_e = 0x10 };
};
WWHD_SIZE(cBgS_LinChkNative, 0x58);
WWHD_OFFSET(cBgS_LinChkNative, mLin, 0x24);
WWHD_OFFSET(cBgS_LinChkNative, field_0x40, 0x40);
WWHD_OFFSET(cBgS_LinChkNative, mFlag, 0x4C);
WWHD_OFFSET(cBgS_LinChkNative, mFrontFlag, 0x53);
WWHD_OFFSET(cBgS_LinChkNative, mBackFlag, 0x54);
void cM3dGLin_SetStartEnd(cM3dGLin* line, const cXyz* start, const cXyz* end);
void* cBgS_Chk_construct(cBgS_Chk* object);
