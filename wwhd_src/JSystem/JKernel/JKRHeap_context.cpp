#include "JSystem/JKernel/JKRHeap_context.h"

static HeapModuleManagerSlot moduleManager;
static HeapContextSlot heapContext;

/* 02756170 */
JKRHeap *becomeCurrentHeap(void * /* ignored */, JKRHeap *incoming) {
    // HD: this entry ignores its receiver and selects the calling thread's heap.
    HeapModuleManager *manager = moduleManager.mpManager;
    HeapThreadContext *context = OSGetThreadSpecific(manager->mThreadSpecificKey);
    JKRHeap *previous = context->mpCurrentHeap;
    context->mpCurrentHeap = incoming;
    return previous;
}

/* 027EC230 */
JKRHeap *getCurrentHeap() { return HeapContext_getCurrentHeap(heapContext.mpContext); }

/* WWHD glue
 data moduleManager 101F8B94
 data heapContext 101F8B4C
 ext C0009CC8 HeapThreadContext* OSGetThreadSpecific(u32)
 ext 02756140 JKRHeap* HeapContext_getCurrentHeap(void*)
*/
