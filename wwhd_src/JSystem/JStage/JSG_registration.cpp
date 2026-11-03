// Compiler registration stubs at the end of the JStage/JMessage interface range.
#include "JSystem/JStage/JSG_registration.h"
namespace {
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sInterfaceBlock;
static StaticBlock sDecompBlock;
static u32 sInterfaceRegister;
static u32 sDecompRegister;
}
void JStage_register(void*);
/* 028428AC */
static void initializeInterfaceRegistration() {
    sInterfaceBlock.words[2]=0; sInterfaceBlock.words[0]=0; sInterfaceBlock.words[3]=0; sInterfaceBlock.words[1]=0;
    JStage_register(&sInterfaceRegister);
}
/* 028428D4 */
static void initializeDecompRegistration() {
    sDecompBlock.words[2]=0; sDecompBlock.words[0]=0; sDecompBlock.words[3]=0; sDecompBlock.words[1]=0;
    JStage_register(&sDecompRegister);
}
/* WWHD glue:
data sInterfaceBlock 104B64B4
data sDecompBlock 104B64C4
data sInterfaceRegister 101FAF3C
data sDecompRegister 101FAF48
*/
