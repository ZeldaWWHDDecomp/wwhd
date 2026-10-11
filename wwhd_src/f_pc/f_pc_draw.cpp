/** Process drawing. */
#include "f_pc/f_pc_draw.h"

static s32 g_fpcLf_type;
static DrawHeaderStatics l_headerStatics;
static const DrawPiPair l_piPair;

/* 025DE2CC */
BOOL fpcDw_Execute(base_process_class* i_proc) {
    if (fpcPause_IsEnable(i_proc, 2) != 0) {
        return FALSE;
    }
    layer_class* curLay = fpcLy_CurrentLayer();
    process_node_class* process = reinterpret_cast<process_node_class*>(i_proc);
    u32 subtype = process->mSubType;
    // HD: both method tables have the draw slot at the same offset; retain the type-check call.
    fpcBs_Is_JustOfType(g_fpcLf_type, subtype);
    nodedraw_method_class* methods = reinterpret_cast<nodedraw_method_class*>(i_proc->mpPcMtd.get());
    layer_class* layer = i_proc->mLyTg.mpLayer;
    u32 draw_func = methods->mpDrawFunc.get();
    fpcLy_SetCurrentLayer(layer);
    BOOL ret = gabi::call_ptr<s32>(draw_func, i_proc);
    fpcLy_SetCurrentLayer(curLay);
    return ret;
}

/* 025DE37C */
BOOL fpcDw_Handler(fpcDw_HandlerFuncFunc i_iterHandler, fpcDw_HandlerFunc i_func) {
    cAPIGph_BeforeOfDraw();
    BOOL ret = gabi::call_ptr<s32>(i_iterHandler, i_func);
    cAPIGph_AfterOfDraw();
    return ret;
}

// HD: compiler-generated and template code of this translation unit.
/* 025DE3CC */
static void __sinit_f_pc_draw_cpp() {
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101F3A4C);
    f32 negativePi = l_piPair.negativePi;
    f32 positivePi = l_piPair.positivePi;
    l_headerStatics.negativePi = negativePi;
    l_headerStatics.positivePi = positivePi;
    DrawFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F3A58);
    DrawSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F3A64);
}

/* WWHD glue (tools/verify/autoglue.py): translation-unit data and other-unit calls
 data l_headerStatics 1048A780
 data l_piPair 100582BC
 data g_fpcLf_type 101F3BD8
 ext 025E0B20 s32 fpcPause_IsEnable(base_process_class*, u32)
 ext 025DD258 BOOL fpcBs_Is_JustOfType(int, int)
 ext 02007E18 void cAPIGph_BeforeOfDraw()
 ext 02007E28 void cAPIGph_AfterOfDraw()
 ext 028ED6F8 void DrawFirstHeaderObject_ct(u8*)
 ext 028EAB2C void DrawSecondHeaderObject_ct(u8*)
*/
