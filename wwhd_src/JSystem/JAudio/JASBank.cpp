/* Based on CC0 tww JASBank.cpp heap selection. */
#include "JSystem/JAudio/JASBankStatics.h"
using namespace JASystem;
using BankHeapPointer = gptr<void>;
static BankHeapPointer sCurrentHeap;
static BankHeapPointer JASDram;
static BankHeaderStatics headerStatics;
static BankDestructorRecord destructorRecord;
/* 0284416C */
void* TBank::getCurrentHeap() {
    void* heap = sCurrentHeap;
    return heap ? heap : (void*)JASDram;
}
/* 02844188 */
void __sinit_JASBank_cpp() {
    headerStatics.mWords[2] = 0;
    headerStatics.mWords[0] = 0;
    headerStatics.mWords[3] = 0;
    headerStatics.mWords[1] = 0;
    registerBasicBankStatics(&destructorRecord);
}
/* WWHD glue
data sCurrentHeap 101FC22C
data JASDram 101F9DA8
data headerStatics 104B65D4
data destructorRecord 101FC220
ext 028F026C void registerBasicBankStatics(BankDestructorRecord*)
*/
