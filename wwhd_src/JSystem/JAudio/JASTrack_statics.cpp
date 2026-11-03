// JASTrack statics shared with other translation units: the sequence sync callback word, exported in
// JASTrack.h (written by TTrack::registerSeqCallback, called by TSeqParser::cmdSyncCPU). Kept apart from
// JASTrack.cpp so a unit that needs only this word links it without JASTrack.cpp's glue methods.
#include "JSystem/JAudio/JASTrack.h"
static u32 sCallBackFunc;
/* WWHD glue
data sCallBackFunc 101F9FCC
*/
