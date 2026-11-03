/* Compiler-kept construction of the hardware request arrays. From WWHD's instructions only. */
#include "nw/snd/HardwareVoice.h"
using namespace nw::snd;

void constructVoiceRequestArray(void* storage, u32 count, u32 stride, u32 constructor);
VoiceRequest* constructVoiceRequest(VoiceRequest* request);
void registerVoiceRequestArray(void* registration);

/* 028A6288 */
static void initializeVoiceRequestStorage() {
    gabi::AutoFrame frame{0x028A6288};
    constructVoiceRequestArray(&sMainRequests, 512, sizeof(VoiceRequest),
        gabi::guest_fn((const void*)&constructVoiceRequest));
    registerVoiceRequestArray(&sMainRequestsExitRecord);
    constructVoiceRequestArray(&sRemoteRequests, 48, sizeof(VoiceRequest),
        gabi::guest_fn((const void*)&constructVoiceRequest));
    registerVoiceRequestArray(&sRemoteRequestsExitRecord);
}

/* WWHD glue
 ext 028EFFD0 void constructVoiceRequestArray(void*,u32,u32,u32)
 ext 028A6340 nw::snd::VoiceRequest* constructVoiceRequest(nw::snd::VoiceRequest*) callback
 ext 028F026C void registerVoiceRequestArray(void*)
*/
