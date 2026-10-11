/* OS reporting helpers, WWHD variadic ABI.
 * HD: GHS saves eight GPRs and optionally eight FPRs before testing report disable flags. */
#pragma once
#include "bindings.h"

struct OSReportVaList {
    /* 0x00 */ be<u8> gpr;
    /* 0x01 */ be<u8> fpr;
    /* 0x02 */ u8 padding[2];
    /* 0x04 */ be<u32> overflow_arg_area;
    /* 0x08 */ be<u32> reg_save_area;
};
WWHD_SIZE(OSReportVaList, 12);
struct OSReportSavedFloat {
    be<u32> high;
    be<u32> low;
};
struct OSReportFrame {
    /* 0x00 */ u8 linkage[8];
    /* 0x08 */ OSReportVaList args;
    /* 0x14 */ u8 padding[4];
    /* 0x18 */ be<u32> gpr[8];
    /* 0x38 */ OSReportSavedFloat fpr[8];
    /* 0x78 */ u8 tailPadding[8];
};
WWHD_OFFSET(OSReportFrame, args, 8);
WWHD_OFFSET(OSReportFrame, gpr, 0x18);
WWHD_OFFSET(OSReportFrame, fpr, 0x38);
WWHD_SIZE(OSReportFrame, 0x80);

void OSReportForceEnableOn();
void OSReportForceEnableOff();
void OSVReport(u32 fmt, OSReportVaList* args);
void OSReportColor(const char* text);
void printfColor(const char* text);

/* HD: the report-enable flag separates the pi pair from the two header objects. */
struct OSReportHeaderStatics {
    /* 0x00 */ be<f32> negativePi;
    /* 0x04 */ be<f32> positivePi;
    /* 0x08 */ u8 reportEnable;
    /* 0x09 */ u8 firstObject;
    /* 0x0A */ u8 secondObject;
    /* 0x0B */ u8 padding;
    /* 0x0C */ be<u32> zero[4];
};
struct OSReportPiPair {
    be<f32> negativePi;
    be<f32> positivePi;
};
WWHD_SIZE(OSReportHeaderStatics, 0x1C);
WWHD_OFFSET(OSReportHeaderStatics, firstObject, 9);
WWHD_OFFSET(OSReportHeaderStatics, zero, 0xC);
WWHD_SIZE(OSReportPiPair, 8);
void OSReportFirstHeaderObject_ct(u8*);
void OSReportSecondHeaderObject_ct(u8*);
