/* c_phase (GameCube names) not yet in wwhd.h: several raw units define cPhs_STOP_e locally, so it
 * lives here until those copies are merged into the shared headers. */
#pragma once
#include "wwhd.h"

enum { cPhs_STOP_e = cPhs_UNK3_e };
