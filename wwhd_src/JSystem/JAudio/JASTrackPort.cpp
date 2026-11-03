/* Based on CC0 tww JASTrackPort.cpp. HD: port operations have no debug assertions. */
#include "JSystem/JAudio/JASTrackPort.h"
using namespace JASystem;

static TrackPortHeaderStatics headerStatics;
static TrackPortDestructorRecord destructorRecord;
namespace JASystem {
/* 0281A13C */
void TTrackPort::init() {
    for (u32 i = 0; i < PORT_COUNT; ++i) {
        mImportFlag[i] = 0;
        mExportFlag[i] = 0;
        mValue[i] = 0;
    }
}
/* 0281A16C */
u16 TTrackPort::readImport(s32 port) {
    mImportFlag[port] = 0;
    return mValue[port];
}
/* 0281A184 */
u16 TTrackPort::readExport(s32 port) {
    mExportFlag[port] = 0;
    return mValue[port];
}
/* 0281A1A0 */
void TTrackPort::writeImport(s32 port, u32 value) {
    mImportFlag[port] = 1;
    mValue[port] = value;
}
/* 0281A1B8 */
void TTrackPort::writeExport(s32 port, u32 value) {
    mExportFlag[port] = 1;
    mValue[port] = value;
}
}
/* 0281A1D4 */
void __sinit_JASTrackPort_cpp() {
    headerStatics.mWords[2] = 0;
    headerStatics.mWords[0] = 0;
    headerStatics.mWords[3] = 0;
    headerStatics.mWords[1] = 0;
    registerTrackPortStatics(&destructorRecord);
}
/* WWHD glue
data headerStatics 104B56A0
data destructorRecord 101F9FDC
ext 028F026C void registerTrackPortStatics(TrackPortDestructorRecord*)
*/
