/* JAudio 1 track ports. Based on CC0 tww; WWHD omits the debug bounds checks. */
#pragma once
#include "bindings.h"

namespace JASystem {
class TTrackPort {
public:
    enum { PORT_COUNT = 16 };
    void init();
    u16 readImport(s32 port);
    u16 readExport(s32 port);
    void writeImport(s32 port, u32 value);
    void writeExport(s32 port, u32 value);

    be<u8> mImportFlag[PORT_COUNT];
    be<u8> mExportFlag[PORT_COUNT];
    be<u16> mValue[PORT_COUNT];
};
WWHD_OFFSET(TTrackPort, mImportFlag, 0);
WWHD_OFFSET(TTrackPort, mExportFlag, 0x10);
WWHD_OFFSET(TTrackPort, mValue, 0x20);
WWHD_SIZE(TTrackPort, 0x40);
}

struct TrackPortHeaderStatics { be<u32> mWords[4]; };
struct TrackPortDestructorRecord { be<u32> mWords[3]; };
WWHD_SIZE(TrackPortHeaderStatics, 16);
WWHD_SIZE(TrackPortDestructorRecord, 12);
void registerTrackPortStatics(TrackPortDestructorRecord*);
