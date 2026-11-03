/* Stream manager interface from CC0 tww; HD leaves these playback hooks empty. */
#include "JSystem/JAudio/JAIStreamMgr.h"
using namespace JAInter::StreamMgr;
static AudioHeaderStatics staticState;
static AudioDestructorRecord staticRecord;
static AudioDestructorRecord controlRecord;
struct StreamCriticalStorage { u8 bytes[0x3C]; };
static StreamCriticalStorage streamCritical;
namespace JAInter { namespace StreamMgr {
/* 0280E07C */
void storeStreamBuffer(gptr<JAISound>*,Actor*,u32,u32,u8,JAISoundInfoView*) {}
/* 0280E080 */
void releaseStreamBuffer(JAISound*,u32) {}
/* 0280E134 */
void init() {}
/* 0280E138 */
void processGFrameStream() {}
}}
/* 0280E084 */
static void __sinit_JAIStreamMgr_cpp() {
    staticState.mWords[2]=0;staticState.mWords[0]=0;
    staticState.mWords[3]=0;staticState.mWords[1]=0;
    streamRegister(&staticRecord);
    streamConstructCritical(&streamCritical);
    streamRegister(&controlRecord);
}
/* 0280E0E0 */
static void destroyStreamSoundLink(JAInter::LinkSound* link,s32 flags) {
    if(link) {
        streamDestroyCritical(link->criticalSection,2);
        if(flags&1) streamDelete(link);
    }
}
/* WWHD glue
data staticState 104B50D4
data staticRecord 101F9CD8
data controlRecord 101F9CE4
data streamCritical 104B50F0
ext 02760084 void streamConstructCritical(void*)
ext 02760168 void streamDestroyCritical(void*,s32)
ext 0273AF40 void streamDelete(void*)
ext 028F026C void streamRegister(void*)
*/
