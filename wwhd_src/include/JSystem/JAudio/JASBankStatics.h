/* Static-registration records and allocation adapters for bank companions. */
#pragma once
#include "JSystem/JAudio/JASBank.h"
struct BankHeaderStatics { be<u32> mWords[4]; };
struct BankDestructorRecord { be<u32> mWords[3]; };
void registerBasicBankStatics(BankDestructorRecord* record);
void deleteBasicBankInstTable(void* table);
void* allocateBasicBankInstTable(u32 size,void* heap,u32 alignment);
WWHD_SIZE(BankHeaderStatics,16);
WWHD_SIZE(BankDestructorRecord,12);
