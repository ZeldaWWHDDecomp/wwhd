/* GHS variadic ABI adapter used by archive path construction. */
#pragma once
#include "m_Do/m_Do_printf.h"
s32 layoutFormatBounded(be<u8>* destination, u32 capacity, u32 limit, const be<u8>* format, u32 a1, u32 a2, u32 a3, u32 a4);
s32 archiveFormatV(be<u8>*, u32, const be<u8>*, OSReportVaList*);
s32 formatArchiveLocalName(be<u8>*, u32, u32, const char*, const be<u8>*, const be<u8>*);
s32 formatArchiveQualifiedName(be<u8>*, u32, u32, const char*, const be<u8>*, const be<u8>*, const be<u8>*);
