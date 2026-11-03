/* Based on CC0 tww JASBasicBank.cpp. Canonical Audio3 bank layout. */
#include "JSystem/JAudio/JASBankStatics.h"
#include "JSystem/JAudio/JASCalc.h"
using namespace JASystem;
static BankVTable basicBankVtable;
static BankHeaderStatics headerStatics;
static BankDestructorRecord destructorRecord;
/* 028441B0 */
TBasicBank::TBasicBank() {
    mInstTable = nullptr;
    mVtable = &basicBankVtable;
    mInstCount = 0;
    mWaveBank = nullptr;
}
/* 02844200 */
TInst* TBasicBank::getInst(u32 index) const {
    if (index >= mInstCount) return nullptr;
    return mInstTable[index];
}
/* 02844224 */
void TBasicBank::setInstCount(u32 count) {
    deleteBasicBankInstTable(mInstTable);
    void* heap = TBank::getCurrentHeap();
    u32 bytes = count * 4;
    mInstTable = (gptr<TInst>*)allocateBasicBankInstTable(bytes,heap,4);
    JASystem::Calc::bzero(mInstTable,bytes);
    mInstCount = count;
}
/* 02844290 */
void TBasicBank::setInst(s32 index,TInst* instrument) { mInstTable[(u32)index] = instrument; }
/* 028442A0 */
void __sinit_JASBasicBank_cpp() {
    headerStatics.mWords[2] = 0;
    headerStatics.mWords[0] = 0;
    headerStatics.mWords[3] = 0;
    headerStatics.mWords[1] = 0;
    registerBasicBankStatics(&destructorRecord);
}
/* WWHD glue
data basicBankVtable 1017A4A4
data headerStatics 104B65E4
data destructorRecord 101FC230
ext 0273AFC8 void deleteBasicBankInstTable(void*)
ext 0273B0D4 void* allocateBasicBankInstTable(u32,void*,u32)
*/
