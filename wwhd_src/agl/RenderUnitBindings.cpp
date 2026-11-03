// Unit-wide bindings of the merged sead registry helpers and agl recording policy that several of these sources call
// (one definition shared by all of their callers).
#include "sead/HeapRegistry.h"
#include "agl/RenderRecording.h"
/* WWHD glue
ext 0273AD10 void* allocateRegistryStorage(u32)
ext 0273AF40 void releaseRegistryStorage(void*)
ext 02756140 sead::RegistryHeap* registryCurrentHeap(void*)
ext 02755FEC sead::RegistryHeap* findRegistryHeap(void*,void*)
ext 027C07FC u32 agl::RecordingPolicy::alignment()
*/
