#pragma once
#include "m_Do/m_Do_printf.h"
struct JUTConsoleStaticBlock { be<u32> mWords[4]; };
struct JUTConsoleText { be<u8> mBytes[256]; };
WWHD_SIZE(JUTConsoleStaticBlock,16);
WWHD_SIZE(JUTConsoleText,256);
void JUTConsoleConstructRegister(void*);
s32 JUTConsoleVsnprintf(void*,u32,const void*,OSReportVaList*);
s32 JUTReportConsole_f_va(const void*,OSReportVaList*);
