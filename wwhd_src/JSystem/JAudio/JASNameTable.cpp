/* HD adds a name table for effect IDs; flag bits are removed before lookup. */
#include "JSystem/JAudio/JASNameTable.h"
static AudioEffectNameTable effectNames;
static AudioCompanionStatics nameHeaderStatics;
static AudioCompanionRecord nameDestructorRecord;
/* 02843CA8 */
void audioNameTableNoOp() {}
/* 02843CAC */
const char* lookupAudioEffectName(u32 id) {
    id &= ~0xE000u;
    if (id >= AUDIO_EFFECT_NAME_COUNT) return nullptr;
    return effectNames.mNames[id];
}
/* 02843CCC */
void __sinit_JASNameTable_cpp() {
    nameHeaderStatics.mWords[2] = 0;
    nameHeaderStatics.mWords[0] = 0;
    nameHeaderStatics.mWords[3] = 0;
    nameHeaderStatics.mWords[1] = 0;
    registerAudioNameTable(&nameDestructorRecord);
}
/* WWHD glue
data effectNames 101FB02C
data nameHeaderStatics 104B6594
data nameDestructorRecord 101FB020
ext 028F026C void registerAudioNameTable(AudioCompanionRecord*)
*/
