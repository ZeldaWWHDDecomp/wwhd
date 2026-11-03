/** Console formatting retained by the HD build; private decomp source. */
#include "bindings.h"
#include "JSystem/JUtility/JUTConsole.h"
static JUTConsoleStaticBlock sStaticBlock;
static u32 sStaticRegister;

/* 027EC9C0 */
static void __sinit_JUTConsole_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    JUTConsoleConstructRegister(&sStaticRegister);
}


// HD: explicit GHS variadic save areas retain integer and optional floating-register arguments.
static void saveConsoleArguments(OSReportFrame* frame,const u32* registers,bool floats) {
    for (u32 i = 0; i < 8; ++i) frame->gpr[i] = registers[i];
    if (floats) {
        for (u32 i = 0; i < 8; ++i) {
            u64 bits;
            memcpy(&bits,&gabi::cpu->f[i+1].ps0,8);
            frame->fpr[i].high = bits >> 32;
            frame->fpr[i].low = bits;
        }
    }
}
/* 027EC908 */
s32 JUTReportConsole_f_va(const void* format,OSReportVaList* arguments) {
    gabi::Local<JUTConsoleText> buffer;
    return JUTConsoleVsnprintf(buffer.get(),256,format,arguments);
}
/* 027EC938 */
static s32 JUTReportConsole_f(const void* format,u32 a4,u32 a5,u32 a6,u32 a7,u32 a8,u32 a9,u32 a10) {
    u32 entry = gabi::cpu->r[1];
    bool floats = gabi::cpu->cr[6] != 0;
    gabi::Local<OSReportFrame> frame;
    const u32 registers[8] = {gabi::ea(format),a4,a5,a6,a7,a8,a9,a10};
    saveConsoleArguments(frame.get(),registers,floats);
    frame->args.overflow_arg_area = entry + 8;
    frame->args.reg_save_area = gabi::ea(frame->gpr);
    frame->args.fpr = 0;
    frame->args.gpr = 1;
    return JUTReportConsole_f_va(format,&frame->args);
}
/* WWHD glue
 data sStaticBlock 104A2480
 data sStaticRegister 101F97BC
 ext 028F026C void JUTConsoleConstructRegister(void*)
 ext 028F1748 s32 JUTConsoleVsnprintf(void*,u32,const void*,OSReportVaList*)
*/
