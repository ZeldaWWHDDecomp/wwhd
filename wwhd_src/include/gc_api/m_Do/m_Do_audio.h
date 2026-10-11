/* m_Do_audio / JAZelAudio (GameCube names). */
#pragma once
#include "bindings.h"

enum {
    JA_SE_CV_M_NZ_TALK_IN = 0x4923,
    JA_SE_CV_M_NZ_TALK_OUT = 0x4924,
};

/* 025E19CC: the two-argument far-thunder overload [v d_ky_thunder]. */
inline void mDoAud_seStart(u32 soundId, cXyz* pos) { gabi::call(0x025E19CC, soundId, pos); }

// HD: isolate these overloads from legacy private global bindings.
namespace mDoAudioApi {
/* HD overloads and music controls [v d_a_bmd]. */
/* 025E1A7C */
inline void monsSeStart(u32 id, cXyz* pos, u32 param, s32 reverb) { gabi::call(0x025E1A7C, id, pos, param, reverb); }
/* 025E1AA4 */
inline void monsSeStart(u32 id, cXyz* pos, u32 actorId, u32 param, s32 reverb) { gabi::call(0x025E1AA4, id, pos, actorId, param, reverb); }
/* 025E18EC */
inline void bgmStart(u32 id) { gabi::call(0x025E18EC, id); }
/* 025E1904 */
inline void bgmStop(u32 frames) { gabi::call(0x025E1904, frames); }
/* 025E1934 */
inline void bgmStreamPrepare(u32 id) { gabi::call(0x025E1934, id); }
/* 025E1944 */
inline void bgmStreamPlay() { gabi::call(0x025E1944); }
} // namespace mDoAudioApi
