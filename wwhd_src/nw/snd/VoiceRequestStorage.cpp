// The hardware-voice request pools and the destructor registration records of the two arrays: a data-only owner,
// declared in nw/snd/HardwareVoice.h (the static initializer 028A6288 constructs and registers them).
#include "nw/snd/HardwareVoice.h"
static nw::snd::MainRequestPool sMainRequests;
static nw::snd::RemoteRequestPool sRemoteRequests;
static nw::snd::StaticExitRecord sMainRequestsExitRecord;
static nw::snd::StaticExitRecord sRemoteRequestsExitRecord;
/* WWHD glue
data sMainRequests 104BC30C
data sRemoteRequests 104C130C
data sMainRequestsExitRecord 101FCB40
data sRemoteRequestsExitRecord 101FCB4C
*/
