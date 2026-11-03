// HD: archive resources are memory-resident; asynchronous work uses the JAudio task queue.
#include "JSystem/JAudio/JASResArcLoader.h"
#include "sead/ElementAt.h"
using namespace JASystem::ResArcLoader;
using namespace gabi;
struct Initialization { be<u32> words[4]; };
struct Registration { be<u32> words[3]; };
static Initialization sInitialization;
static Registration sRegistration;
using ResourceCallback = void (*)(ResourceRequest*);
extern void* copyResourceMemory(void*, const void*, u32);
extern u32 sendResourceMessage(JASMessageQueue*, Message*, u32);
extern void initializeResourceQueue(JASMessageQueue*, Message*, u32);
extern u32 receiveResourceMessage(JASMessageQueue*, Message*, u32);
extern void* getResourceTaskThread();
extern u32 submitResourceTask(void*, ResourceCallback, ResourceRequest*, u32);
extern void registerResourceGlobals(void*);
namespace JASystem { namespace ResArcLoader {
/* 02810660 */
u32 getResSize(Archive* archive, u32 id) {
    return sead::elementAt(archive->mEntries, id).mSize;
}
/* 02810670 */
void loadResourceCallback(ResourceRequest* request) {
    Archive* archive = request->mArchive;
    u32 id = request->mId;
    u32 size = 0;
    if (id < archive->mCount) {
        void* buffer = request->mBuffer;
        void* data = archive->mEntries[id].mData;
        u32 requested = request->mSize;
        copyResourceMemory(buffer, data, requested);
        size = request->mSize;
    }
    u32 callback = request->mCallback.get();
    if (callback)
        gabi::call_ptr<void>(callback, size, (u32)request->mCallbackArgument);
    JASMessageQueue* queue = request->mQueue;
    if (queue) {
        Message message;
        message.mWords[0] = size ? 0 : 0xFFFFFFFF;
        sendResourceMessage(queue, &message, 1);
    }
}
/* 02810744 */
u32 loadResource(Archive* archive, u32 id, void* buffer, u32 size) {
    // HD: the retained queue pointer preserves its position relative to the request.
    ResourceLoadLocals locals;
    JASMessageQueue& queue = locals.mQueue;
    Message& messages = locals.mMessages;
    initializeResourceQueue(&queue, &messages, 1);
    ResourceRequest& request = locals.mRequest;
    request.mArchive = archive;
    request.mBuffer = buffer;
    request.mCallback.v = 0;
    request.mCallbackArgument = 0;
    request.mSize = size;
    request.mId = (u16)id;
    request.mQueue = &queue;
    void* thread = getResourceTaskThread();
    if (!submitResourceTask(thread, loadResourceCallback, &request, sizeof(request)))
        return 0;
    Message& result = locals.mResult;
    receiveResourceMessage(&queue, &result, 1);
    if (result.mWords[0] != 0)
        return 0;
    return size;
}
/* 02810828 */
void loadResourceAsync(Archive* archive, u32 id, void* buffer, u32 size, u32 callback, u32 callbackArgument) {
    ResourceRequest request;
    request.mBuffer = buffer;
    request.mSize = size;
    request.mQueue = nullptr;
    request.mArchive = archive;
    request.mCallbackArgument = callbackArgument;
    request.mId = (u16)id;
    request.mCallback.v = callback;
    void* thread = getResourceTaskThread();
    submitResourceTask(thread, loadResourceCallback, &request, sizeof(request));
}
}}
/* 0281087C */
void __sinit_JASResArcLoader_cpp() {
    sInitialization.words[2] = 0;
    sInitialization.words[0] = 0;
    sInitialization.words[3] = 0;
    sInitialization.words[1] = 0;
    registerResourceGlobals(&sRegistration);
}
/* WWHD glue
data sInitialization 104B5274
data sRegistration 101F9DAC
ext C000A848 void* copyResourceMemory(void*, const void*, u32)
ext C0009FC8 u32 sendResourceMessage(JASMessageQueue*, Message*, u32)
ext C0009D58 void initializeResourceQueue(JASMessageQueue*, Message*, u32)
ext C0009ED0 u32 receiveResourceMessage(JASMessageQueue*, Message*, u32)
ext 0280ED64 void* getResourceTaskThread()
ext 02810AA4 u32 submitResourceTask(void*, ResourceCallback, ResourceRequest*, u32)
ext 028F026C void registerResourceGlobals(void*)
*/
