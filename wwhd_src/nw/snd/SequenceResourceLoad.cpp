/* Worker-side sequence and bank loading; reconstructed from WWHD's own code. */
#include "nw/snd/SoundRuntimeSupport.h"
#include "gc_api/PowerPC_EABI_Support/Runtime/Inc/CPlusLibPPC.h"
namespace nw { namespace snd { struct SoundArchive; } }
using namespace nw::snd;
struct ResourceLoadContext_l { u8 state[0xC]; u8 loader[0x258]; };
WWHD_SIZE(ResourceLoadContext_l,0x264);
struct ResourceLoadHeap_l { u8 state[0x14]; be<u8> loadState; };
struct ResourceChoices { SoundResourceChoice value[4]; };
static u32 sDefaultResourceFlags;
void resourceContextCreate(ResourceLoadContext_l*);
void resourceContextSetArchive(ResourceLoadContext_l*,u32);
u32 resourceLoadFile(void*,u32,void*,u32,u32);
u32 resourceFileAddress(void*,u32);
u32 resourceResolveBank(SoundResourceChoice*,be<u8>*,void* bankImage,SoundArchive* archive,void* waveContext);
u32 resourceLoadWave(void*,u32,void*);
void resourceContextFinish(ResourceLoadContext_l*);
void resourceContextDestroy(ResourceLoadContext_l*,u32);

static inline void failResourceLoad(SequenceLoadTask* task,ResourceLoadContext_l* context) {
    ResourceLoadHeap_l* heap = (ResourceLoadHeap_l*)task->heap.get();
    heap->loadState = 2;
    task->loadSucceeded = 0;
    resourceContextDestroy(context,2);
}
/* 0288B678 */
void SequenceLoadTask::loadResources() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<ResourceLoadContext_l> context(0xC);
    resourceContextCreate(context.get());
    resourceContextSetArchive(context.get(),parameters[0]);
    if (parameters[4] == 0) {
        u32 id = parameters[3];
        if (id != 0xFFFFFFFFu) {
            void* loadHeap = heap;
            if (!resourceLoadFile(context->loader,id,loadHeap,1,0)) {
                failResourceLoad(this,context.get());
                return;
            }
            parameters[4] = resourceFileAddress(context->loader,id);
        }
    }
    gabi::FrameLocal<ResourceChoices> choices(0x270);
    __construct_array(choices->value,4,8,(void* (*)(void*))SoundResourceChoice::create);
    gabi::FrameLocal<be<u32>> flagWord(8);
    *flagWord = (u32)sDefaultResourceFlags;
    be<u8>* flags = reinterpret_cast<be<u8>*>(flagWord.get());
    for (u32 i = 0; i < 4; ++i) {
        be<u32>* bank = &parameters[5 + i * 2];
        u32 address = bank[1];
        if (address == 0) {
            u32 id = bank[0];
            if (id == 0xFFFFFFFFu) continue;
            void* loadHeap = heap;
            if (!resourceLoadFile(context->loader,id,loadHeap,4,0)) {
                failResourceLoad(this,context.get());
                return;
            }
            address = resourceFileAddress(context->loader,id);
            bank[1] = address;
        }
        if ((u32)bank[0] == 0xFFFFFFFFu || address == 0) continue;
        u32 wave = parameters[1];
        void* waveContext = wave ? gabi::at<void>(wave + 0xC) : nullptr;
        u32 archive = parameters[0];
        u32 result = resourceResolveBank(&choices->value[i],&flags[i],gabi::at<void>(address),gabi::at<SoundArchive>(archive),waveContext);
        if (result == 0xFFFFFFFFu || result == 2) {
            void* loadHeap = heap;
            u32 bankAddress = bank[1];
            if (!resourceLoadWave(context->loader,bankAddress,loadHeap)) {
                failResourceLoad(this,context.get());
                return;
            }
            bankAddress = bank[1];
            archive = parameters[0];
            resourceResolveBank(&choices->value[i],&flags[i],gabi::at<void>(bankAddress),gabi::at<SoundArchive>(archive),context->loader);
        }
    }
    sequence = (u32)parameters[4];
    for (u32 i = 0; i < 4; ++i) {
        u32 image = choices->value[i].value;
        u32 address = parameters[6 + i * 2];
        u8 flag = flags[i];
        banks[i] = address;
        waveArchiveImages[i] = image;
        individualWaveLoads[i] = flag;
    }
    ResourceLoadHeap_l* loadHeap = (ResourceLoadHeap_l*)heap.get();
    loadHeap->loadState = 2;
    resourceContextFinish(context.get());
    loadSucceeded = 1;
    resourceContextDestroy(context.get(),2);
}
/* WWHD glue
data sDefaultResourceFlags 10181FC4
ext 028ADAF4 void resourceContextCreate(ResourceLoadContext_l*)
ext 028ADB80 void resourceContextSetArchive(ResourceLoadContext_l*,u32)
ext 02892430 u32 resourceLoadFile(void*,u32,void*,u32,u32)
ext 028926BC u32 resourceFileAddress(void*,u32)
ext 0289CEA0 u32 resourceResolveBank(nw::snd::SoundResourceChoice*,be<u8>*,void*,nw::snd::SoundArchive*,void*)
ext 028927FC u32 resourceLoadWave(void*,u32,void*)
ext 028AD988 void resourceContextFinish(ResourceLoadContext_l*)
ext 028AD9B0 void resourceContextDestroy(ResourceLoadContext_l*,u32)
*/
