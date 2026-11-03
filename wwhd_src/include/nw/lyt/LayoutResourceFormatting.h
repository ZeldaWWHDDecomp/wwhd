/* Layout resource-name formatting preserves the GHS variadic save area. */
#pragma once
#include "m_Do/m_Do_printf.h"
namespace nw { namespace lyt {
struct LayoutFormatSaveArea { OSReportVaList args; u8 padding[4]; be<u32> gpr[8]; OSReportSavedFloat fpr[8]; };
WWHD_SIZE(LayoutFormatSaveArea,0x70);
WWHD_OFFSET(LayoutFormatSaveArea,gpr,0x10);
WWHD_OFFSET(LayoutFormatSaveArea,fpr,0x30);
s32 formatLayoutName(be<u8>* output,u32 capacity,u32 maximum,const char* format,u32 first,u32 second,u32 third,u32 fourth);
} }
s32 layoutResourceVsnprintf(be<u8>* output,u32 capacity,const char* format,OSReportVaList* args);
