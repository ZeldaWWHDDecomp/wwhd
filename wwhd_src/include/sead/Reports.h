// WWHD report argument save area follows the GHS variadic calling convention.
#pragma once
#include "wwhd.h"
struct ReportArguments {
    be<u8> mIntegerIndex;
    be<u8> mFloatIndex;
    u8 mPadding[2];
    be<u32> mOverflow;
    be<u32> mSavedRegisters;
};
struct ReportSave {
    ReportArguments mArguments;
    u8 mPadding[4];
    be<u32> mIntegers[8];
    be<u32> mFloatWords[16];
};
struct AssertionSave : ReportSave {
    be<u8> mMessage[384];
    u8 mTail[16];
};
WWHD_SIZE(ReportArguments,12);
WWHD_OFFSET(ReportSave,mArguments,0);
WWHD_OFFSET(ReportSave,mIntegers,16);
WWHD_OFFSET(ReportSave,mFloatWords,48);
WWHD_SIZE(ReportSave,112);
WWHD_OFFSET(AssertionSave,mMessage,112);
WWHD_SIZE(AssertionSave,512);
void sendReport(const char*,ReportArguments*);
void reportWarningPrefix(const char*,const char*,u32);
s32 formatAssertion(be<u8>*,u32,const char*,const char*,u32);
s32 formatAssertionEnd(be<u8>*,u32,const char*);
s32 formatAssertionArguments(be<u8>*,u32,const char*,ReportArguments*);
void writeReportBytes(const char*,s32);
void notifyReportListeners(const char*);
void stopAfterAssertion();
void writeOsReport(const char*,const char*);
u32 debuggerIsPresent();
void assertionPanic(const char*,u32,const char*);

void sendOsReport(const char*,ReportArguments*);
void reportAssertion(const char*,u32,const char*,const char*);
