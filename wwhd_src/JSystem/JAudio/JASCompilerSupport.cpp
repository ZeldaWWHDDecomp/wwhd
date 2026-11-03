/* HD compiler support; profile state is zeroed while the GC probing is stripped. */
#include "JSystem/JAudio/JASCompilerSupport.h"
static AudioCompanionStatics profileHeaderStatics;
static AudioCompanionRecord profileDestructorRecord;
/* 02844088 */
AudioProfileState* constructAudioProfileState(AudioProfileState* state) {
    if (!state) state = (AudioProfileState*)allocateAudioSupport(sizeof(AudioProfileState));
    if (state) state->mState = 0;
    return state;
}
/* 028440C4 */
void destroyAudioProfileState(void* object,u32 flags) {
    if (object && (flags & 1)) freeAudioSupport(object);
}
/* 028440D8 */
void __sinit_JASProfileSupport_cpp() {
    profileHeaderStatics.mWords[2] = 0;
    profileHeaderStatics.mWords[0] = 0;
    profileHeaderStatics.mWords[3] = 0;
    profileHeaderStatics.mWords[1] = 0;
    registerAudioSupport(&profileDestructorRecord);
}
/* 02844100 */
void destroyAudioStringBase(void* object,u32 flags) {
    if (object && (flags & 1)) freeAudioSupport(object);
}
/* 02844114 */
void terminateAudioString(AudioTerminatedBuffer* string) {
    string->mBuffer[(u32)string->mCapacity - 1] = 0;
}
/* 0284412C */
void destroyAudioStringBuffer(void* object,u32 flags) {
    if (object && (flags & 1)) freeAudioSupport(object);
}
/* 02844140 */
void audioStringBaseNoOp() {}
/* 02844144 */
void destroyAudioStringBufferCopy(void* object,u32 flags) {
    if (object && (flags & 1)) freeAudioSupport(object);
}
/* 02844158 */
void destroyAudioStringBufferStorage(void* object,u32 flags) {
    if (object && (flags & 1)) freeAudioSupport(object);
}
/* WWHD glue
data profileHeaderStatics 104B65C4
data profileDestructorRecord 101FC214
ext 0273AD10 void* allocateAudioSupport(u32)
ext 0273AF40 void freeAudioSupport(void*)
ext 028F026C void registerAudioSupport(AudioCompanionRecord*)
*/
