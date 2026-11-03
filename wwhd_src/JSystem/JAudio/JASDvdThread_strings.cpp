// HD: per-TU string helpers emitted by the JAudio file adapter.
#include "JSystem/JAudio/JASDvdThread_strings.h"
extern void deleteAudioStringObject(void*);
/* 0280FB74 */
void destroyAudioBaseString(AudioString* string, u32 flags) {
    if (string && (flags & 1))
        deleteAudioStringObject(string);
}
/* 0280FB88 */
void AudioBoundedString::assureTermination() const {
    be<u8>* text = mString.mText;
    u32 size = mSize;
    text[size - 1] = 0;
}
/* 0280FBA0 */
void destroyAudioBufferedString(AudioBoundedString* string, u32 flags) {
    if (string && (flags & 1))
        deleteAudioStringObject(string);
}
/* 0280FBB4 */
void AudioString::assureTermination() const {}
/* 0280FBB8 */
void destroyAudioMutableString(AudioBoundedString* string, u32 flags) {
    if (string && (flags & 1))
        deleteAudioStringObject(string);
}
/* 0280FBCC */
void destroyAudioPathString(AudioBoundedString* string, u32 flags) {
    if (string && (flags & 1))
        deleteAudioStringObject(string);
}
/* WWHD glue
ext 0273AF40 void deleteAudioStringObject(void*)
*/
