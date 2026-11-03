#pragma once
#include "JSystem/JUtility/JUTDbPrint.h"
struct JUTPrintEntry { gptr<JUTPrintEntry> mNext; };
struct JUTDbPrintStatic { be<u32> words[4]; };
WWHD_SIZE(JUTPrintEntry,4); WWHD_SIZE(JUTDbPrintStatic,16);
void printFree(JKRHeap*,void*);
JUTDbPrint* printConstruct(JUTDbPrint*,JUTFont*,JKRHeap*);
void printRegisterStatic(void*);
