/* NW-SND#6: the archive player's sound pools (allocation by priority) and its two work pools, which
 * take back an entry once its background load has finished (per-frame callbacks of the frame driver). */
#pragma once
#include "nw/snd/ArchiveStartContext.h"
#include "nw/snd/WaveSound.h"
void emptyWorkPoolHook();
