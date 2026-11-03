/** JUTDbPrint keeps the GC 0x14-byte layout. */
#pragma once
#include "wwhd.h"
struct JUTFont;
struct JUTDbPrint {
    gptr<void> mFirst;
    gptr<JUTFont> mFont;
    be<u32> mColor;
    be<u8> mVisible;
    u8 mPadding[3];
    gptr<JKRHeap> mHeap;
    static JUTDbPrint* start(JUTFont*, JKRHeap*);
    void flush();
    JUTFont* changeFont(JUTFont*);
    void destroyEntries();
};
WWHD_OFFSET(JUTDbPrint,mFirst,0);
WWHD_OFFSET(JUTDbPrint,mFont,4);
WWHD_OFFSET(JUTDbPrint,mColor,8);
WWHD_OFFSET(JUTDbPrint,mVisible,0xc);
WWHD_OFFSET(JUTDbPrint,mHeap,0x10);
WWHD_SIZE(JUTDbPrint,0x14);
JKRHeap* JUTDbPrintCurrentHeap();
#include "m_Do/m_Do_printf.h"
struct JUTReportText { be<u8> mBytes[256]; };
struct JUTReportStringVtable { u8 mHeader[0x14]; be<u32> mAssureTermination; };
struct JUTReportString { gptr<const be<u8>> mText; gptr<JUTReportStringVtable> mVtable; };
struct JUTReportBufferedString {
    gptr<be<u8>> mText; gptr<void> mVtable; be<u32> mCapacity; be<u8> mBytes[0x104];
};
WWHD_SIZE(JUTReportText,256);
WWHD_SIZE(JUTReportString,8);
WWHD_SIZE(JUTReportBufferedString,0x110);
WWHD_SIZE(JUTReportStringVtable,0x18);
s32 JUTReportVsnprintf(void*,u32,const void*,OSReportVaList*);
void JUTReportAssureTermination(JUTReportString*);
void* JUTReportBlockMove(void*,const void*,u32,u32);
