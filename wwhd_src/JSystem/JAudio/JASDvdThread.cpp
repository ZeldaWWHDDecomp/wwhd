/* DVD task messages, following CC0 tww JASDvdThread and Cafe argument/result behavior. */
#include "JSystem/JAudio/JASDvdThread.h"
using namespace JASystem::Dvd;
static JASTaskThreadView* sThread;
static void* dvdHeap;
struct DvdStaticState { be<u32> words[4]; };
static DvdStaticState staticState;
static u8 staticRecord;
namespace JASystem { namespace Dvd {
/* 0280ED64 */
JASTaskThreadView* getThread() { return sThread; }
/* 0280EC38 */
u32 createThread(s32 priority,s32 messages,u32 stackSize,u32 flags) {
    void* storage=dvdThreadAllocate(0xA8,dvdHeap,4);
    JASTaskThreadView* thread=nullptr;
    if(storage) thread=dvdThreadConstruct(storage,priority,messages,stackSize,flags);
    sThread=thread;
    dvdThreadResume(thread->nativeThread);
    return 1;
}
/* 0280ECC8 */
void dvdThreadCheckBack(DvdMessage* message) {
    be<u32>* result=message->result;
    if(result) *result=message->value;
    u32 callback=message->callback;
    if(callback) gabi::call_ptr<void>(callback,static_cast<u32>(message->value));
}
/* 0280ECF4 */
u32 checkPassDvdT(u32 value,be<u32>* result,u32 callback) {
    gabi::AutoFrame frame;
    gabi::FrameLocal<DvdMessage> message(8);
    message->result=result;message->callback=callback;message->value=value;
    return dvdThreadSend(sThread,dvdThreadCheckBack,message.get(),12);
}
}}
/* 0280ED3C */
static void __sinit_JASDvdThread_cpp() {
    staticState.words[2]=0;staticState.words[0]=0;
    staticState.words[3]=0;staticState.words[1]=0;
    dvdThreadRegister(&staticRecord);
}
/* WWHD glue
data sThread 101F9D3C
data dvdHeap 101F9DA8
data staticState 104B51C4
data staticRecord 101F9D40
ext 0273B050 void* dvdThreadAllocate(u32,void*,s32)
ext 028108A4 JASTaskThreadView* dvdThreadConstruct(void*,s32,s32,u32,u32)
ext C0009F38 void dvdThreadResume(void*)
ext 02810AA4 u32 dvdThreadSend(JASTaskThreadView*,DvdCallback,DvdMessage*,u32)
ext 028F026C void dvdThreadRegister(void*)
*/
