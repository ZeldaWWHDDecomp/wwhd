/* hd_snd_t60.h (WWHD, true 60 build): the HD sound player's per-frame work on true 60
 * (§5 port runtime/src/ PLAYER).
 *
 * The sound engine stays a 30 Hz system: updateSoundPlayer 0202FA3C (the NW4F players' and the BGM faders' frame: a fade
 * of n frames) runs once per 30 Hz frame, on the frame's tick30 step. The port decides (wwhd_t60_audio_player: true at
 * N = 1 and on every 30 Hz fallback frame, so the original runs unchanged there); outside the game build (verify) it
 * always runs. */
#pragma once
#include "true60.h"

#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" {
__attribute__((visibility("default"))) int wwhd_t60_audio_player();
}
inline bool t60_audio_player_runs() { return wwhd_t60_audio_player() != 0; }
#else
inline bool t60_audio_player_runs() { return true; }
#endif
