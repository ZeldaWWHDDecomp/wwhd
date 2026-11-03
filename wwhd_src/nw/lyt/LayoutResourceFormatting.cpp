/* Bounded layout resource-name formatting, reconstructed from the WWHD binary. */
#include "nw/lyt/LayoutResourceFormatting.h"
/* 02880C24 */
s32 nw::lyt::formatLayoutName(be<u8>* output,u32 capacity,u32 maximum,const char* format,u32 first,u32 second,u32 third,u32 fourth) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<LayoutFormatSaveArea> saved(8);
    saved->gpr[3]=gabi::ea(format);
    saved->gpr[6]=third;
    saved->gpr[5]=second;
    saved->gpr[0]=gabi::ea(output);
    saved->gpr[7]=fourth;
    saved->gpr[4]=first;
    saved->gpr[1]=capacity;
    saved->gpr[2]=maximum;
    if (gabi::cpu->cr[6]) {
        for (u32 i=0;i<8;++i) {
            u64 bits;
            memcpy(&bits,&gabi::cpu->f[i+1].ps0,sizeof(bits));
            saved->fpr[i].high=bits>>32;
            saved->fpr[i].low=bits;
        }
    }
    saved->args.overflow_arg_area=gabi::cpu->r[1]+8;
    u32 size=maximum+1;
    saved->args.gpr=4;
    if (capacity<size) size=capacity;
    saved->args.reg_save_area=gabi::ea(saved->gpr);
    saved->args.fpr=0;
    s32 result=layoutResourceVsnprintf(output,size,format,&saved->args);
    output[capacity-1]=0;
    return result;
}
/* WWHD glue
ext 028F1748 s32 layoutResourceVsnprintf(be<u8>*,u32,const char*,OSReportVaList*)
*/
