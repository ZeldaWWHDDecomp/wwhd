// HD decompression command preparation; reconstructed from local game instructions.
#include "bindings.h"
#include "JSystem/JKernel/JKRCommandAdapters.h"

/* 028429D8 */
static JKRDecompCommand* prepareNativeCommand(void* source,void* destination,u32 sourceLength,
                                             u32 destinationLength,void* callback) {
    gabi::AutoFrame frame{gabi::current_function()};
    void* heap = currentNativeCommandHeap();
    JKRDecompCommand* command = allocateNativeCommand(sizeof(JKRDecompCommand),heap,-4);
    if (command != NULL) command = constructNativeCommand(command);
    // The constructor's returned pointer, rather than the allocation, receives the parameters.
    NativeDecompParameters* parameters = reinterpret_cast<NativeDecompParameters*>(command->mParameters);
    parameters->mSource = source;
    command->mCallback = gabi::ea(callback);
    parameters->mDestination = destination;
    parameters->mDestinationLength = destinationLength;
    parameters->mSourceLength = sourceLength;
    return command;
}

/* WWHD glue
 ext 027EC42C void* currentNativeCommandHeap()
 ext 0273B050 JKRDecompCommand* allocateNativeCommand(u32,void*,s32)
 ext 02842974 JKRDecompCommand* constructNativeCommand(JKRDecompCommand*)
*/
