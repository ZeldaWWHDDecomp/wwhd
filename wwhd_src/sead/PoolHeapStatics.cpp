// The core index -> worker slot table of the pool-heap workers (104A0BC0, .bss), shared with the units that read it
// (SEAD#19 reads entries 0..2); declared in sead/PoolHeap.h.
#include "sead/PoolHeap.h"
static u32 sCoreReverse[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
/* WWHD glue
data sCoreReverse 104A0BC0
*/
