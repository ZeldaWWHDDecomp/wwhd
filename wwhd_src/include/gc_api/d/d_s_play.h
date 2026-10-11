// GameCube API bindings for verified HD callers.
#pragma once
#include "bindings.h"
/* 025AF2A4 [v d_a_bmd] */
inline BOOL dScnPly_isPause() { return gabi::call<BOOL>(0x025AF2A4); }

/* 101EACB7 [v d_a_bmd]: HD scene pause counter. */
inline void dScnPly_setPauseTimer(u8 frames) { *gabi::at<be<u8>>(0x101EACB7) = frames; }
