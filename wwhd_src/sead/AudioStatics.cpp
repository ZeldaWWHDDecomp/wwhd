// Statics of the audio-services and audio-system translation units shared with their companion sources
// (sead/AudioServices.cpp, sead/AudioSystem.cpp and SEAD#18's audio-system code): the audio-services singleton,
// the audio system's runtime type, the launch rendezvous and the launch thread's name, declared in
// sead/AudioServices.h and sead/AudioSystem.h.
#include "sead/AudioSystem.h"
using namespace sead;
static AudioServices* sAudioServices;
static RuntimeType sAudioSystemType;
static u32 sAudioSystemTypeGuard;
static PlatformRendezvous sAudioRendezvous;
static SafeText sAudioLaunchThreadName;
/* WWHD glue
data sAudioServices 101F8BA8
data sAudioSystemType 101FD958
data sAudioSystemTypeGuard 101FD648
data sAudioRendezvous 104A11E0
data sAudioLaunchThreadName 104A11D8
*/
