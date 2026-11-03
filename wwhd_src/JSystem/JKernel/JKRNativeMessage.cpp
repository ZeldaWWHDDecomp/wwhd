// HD JKernel message adapters; private behavior reconstruction from local instructions.
#include "bindings.h"
#include "JSystem/JKernel/JKRNativeAdapters.h"

/* 0284B918 */
static u32 sendMessageValue(void* queue,u32 value,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<JKRNativeMessage> message(8);
    message->mValue = value;
    message->mArguments[0] = 0;
    message->mArguments[2] = 0;
    message->mArguments[1] = 0;
    return sendNativeMessage(queue,message.get(),flags);
}

/* 0284B958 */
static u32 receiveMessageValue(void* queue,be<u32>* output,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<JKRNativeMessage> message(8);
    u32 result = receiveNativeMessage(queue,message.get(),flags);
    // The original copies the output word even when the receive reports failure.
    *output = message->mValue;
    return result;
}

/* WWHD glue
 ext C0009FC8 u32 sendNativeMessage(void*,JKRNativeMessage*,u32)
 ext C0009ED0 u32 receiveNativeMessage(void*,JKRNativeMessage*,u32)
*/
