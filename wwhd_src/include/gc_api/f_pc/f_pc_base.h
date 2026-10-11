/* f_pc_base: compiler-generated base process lifetime calls in WWHD.
 * Local game-derived API. */
#pragma once
#include "f_pc/f_pc_base.h"

/* 025DD5F0 [v d_kyeff2]: verified caller discards the constructor's result. */
inline void __ct(base_process_class* p) { gabi::call<void>(0x025DD5F0, p); }
/* 025DD630 [v d_kyeff2]: non-deleting base destructor. */
inline void __dt(base_process_class* p) { gabi::call<void>(0x025DD630, p, 0); }
