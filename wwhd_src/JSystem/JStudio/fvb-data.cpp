// Function-value bank data TU registration.
#include "JSystem/JStudio/fvb-data.h"
void FunctionValue_register(void*);
namespace {
struct fvb_data_StaticBlock { be<u32> words[4]; };
static fvb_data_StaticBlock sfvb_dataStaticBlock;
static u32 sfvb_dataStaticRegister;
}

/* 0284A8E4 */
static void __sinit_fvb_data_cpp() {
    sfvb_dataStaticBlock.words[2]=0; sfvb_dataStaticBlock.words[0]=0; sfvb_dataStaticBlock.words[3]=0; sfvb_dataStaticBlock.words[1]=0;
    FunctionValue_register(&sfvb_dataStaticRegister);
}
/* WWHD glue:
data sfvb_dataStaticBlock 104B6704
data sfvb_dataStaticRegister 101FC790
*/
