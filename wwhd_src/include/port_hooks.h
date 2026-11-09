/**
 * port_hooks.h (WWHD)
 * Points where the port's decomp game build lets the port decide, instead of keeping the function recompiled behind
 * an instruction hook. Game build only (GABI_GAME_BUILD, gabi.h: defined by the port's decomp build, never by the
 * verification harness): there every macro below is a constant and the function compiles exactly as without it.
 */
#pragma once

#ifdef GABI_GAME_BUILD
extern "C" int wwhd_classic_wind_waker(void); /* the port: runtime/src/mods/speedrun.cpp (Settings > Speedrun) */
/* Classic Wind Waker: HD's two Wind Waker checks off (procTactWait's falling check, endDemoMode's wait reset) */
#define PORT_CLASSIC_TACT() (wwhd_classic_wind_waker() != 0)
#else
#define PORT_CLASSIC_TACT() 0
#endif
