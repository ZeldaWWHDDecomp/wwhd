/**
 * d_a_item_basecalls.cpp (WWHD)
 * daItemBase_c methods as guest calls, for the d_a_item verification unit (the verified
 * implementations are in d_a_itembase*.cpp, unit d_a_itembase).
 *
 */
#include "d/actor/d_a_itembase.h"

/* (not WWHD_FUNC by name: these are not verified here, and mutate.py must not mutate them) */
#define GUEST_CALL(addr, ...) WWHD_FUNC(addr, __VA_ARGS__)

BOOL daItemBase_c::DeleteBase(const char* resName) { GUEST_CALL(0x02183788, BOOL, this, resName); return FALSE; }
void daItemBase_c::animPlay(f32 a, f32 b, f32 c, f32 d, f32 e) { GUEST_CALL(0x021837B0, void, this, a, b, c, d, e); }
void daItemBase_c::hide() { GUEST_CALL(0x021842B8, void, this); }
void daItemBase_c::show() { GUEST_CALL(0x021842C8, void, this); }
bool daItemBase_c::chkDraw() { GUEST_CALL(0x021842D8, bool, this); return false; }
void daItemBase_c::changeDraw() { GUEST_CALL(0x021842E4, void, this); }
void daItemBase_c::setLoadError() { GUEST_CALL(0x02184350, void, this); }
