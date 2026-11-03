#pragma once
#include "m_Do/m_Do_printf.h"
namespace nw { namespace ut {
void reportV(u32,OSReportVaList*);
void halt();
void report(u32,u32,u32,u32,u32,u32,u32,u32);
} }
void utPlatformReport(u32,OSReportVaList*);
u32 utDebuggerPresent();
void utPlatformPanic(const char*,u32,const char*);
