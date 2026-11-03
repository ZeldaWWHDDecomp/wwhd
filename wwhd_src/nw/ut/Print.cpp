/* Cafe diagnostics and the GHS variadic argument area, reconstructed from WWHD. */
#include "nw/ut/Print.h"
/* 0286DB50 */
void nw::ut::reportV(u32 format,OSReportVaList* arguments) { utPlatformReport(format,arguments); }
/* 0286DB54 */
void nw::ut::halt() {
    // the debugger break (twu r31,r31 at 0286DB70) when a debugger is attached: compared as a trap event
    if(utDebuggerPresent()) { gabi::trap(0x0286DB70); return; }
    gabi::cpu->cr[6]=0;
    utPlatformPanic("../ut/os/./platform/ut_Print_Cafe.cpp",65,"halt");
}
/* 0286DBB0 */
void nw::ut::report(u32 format,u32 a4,u32 a5,u32 a6,u32 a7,u32 a8,u32 a9,u32 a10) {
    u32 entry=gabi::cpu->r[1];
    bool floats=gabi::cpu->cr[6]!=0;
    gabi::Local<OSReportFrame> frame;
    const u32 registers[8]={format,a4,a5,a6,a7,a8,a9,a10};
    for(u32 i=0;i<8;++i) frame->gpr[i]=registers[i];
    if(floats) {
        for(u32 i=0;i<8;++i) {
            u64 bits;
            memcpy(&bits,&gabi::cpu->f[i+1].ps0,8);
            frame->fpr[i].high=bits>>32;
            frame->fpr[i].low=bits;
        }
    }
    frame->args.overflow_arg_area=entry+8;
    frame->args.fpr=0;
    frame->args.reg_save_area=gabi::ea(frame->gpr);
    frame->args.gpr=1;
    reportV(format,&frame->args);
}
/* WWHD glue
ext C000A1D8 void utPlatformReport(u32,OSReportVaList*)
ext C0009DD0 u32 utDebuggerPresent()
ext C0009EB0 void utPlatformPanic(const char*,u32,const char*)
str 1017E990
str 1017E988
*/
