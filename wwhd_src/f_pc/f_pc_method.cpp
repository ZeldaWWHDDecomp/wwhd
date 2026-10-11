/** Process method dispatch, GameCube structure verified against WWHD. Local only. */
#include "f_pc/f_pc_method.h"

/* 025DFCAC */
s32 fpcMtd_Method(u32 i_methodFunc, void* i_data) {
    if (i_methodFunc != 0) {
        return gabi::call_ptr<s32>(i_methodFunc, i_data);
    } else {
        return TRUE;
    }
}

/* 025DFCC4 */
BOOL fpcMtd_Execute(process_method_class* i_methods, void* i_data) {
    return fpcMtd_Method(i_methods->mpExecuteFunc.get(), i_data);
}

/* 025DFCCC */
BOOL fpcMtd_IsDelete(process_method_class* i_methods, void* i_data) {
    return fpcMtd_Method(i_methods->mpIsDeleteFunc.get(), i_data);
}

/* 025DFCD4 */
BOOL fpcMtd_Delete(process_method_class* i_methods, void* i_data) {
    return fpcMtd_Method(i_methods->mpDeleteFunc.get(), i_data);
}

/* 025DFCDC */
cPhs_State fpcMtd_Create(process_method_class* i_methods, void* i_data) {
    return static_cast<cPhs_State>(fpcMtd_Method(i_methods->mpCreateFunc.get(), i_data));
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */
/* 025DFCE4 */
static void __sinit_f_pc_method_cpp() {
    sinit_header_statics_rodata(0x1048AA60, 0x101F3CAC, 0x1048AA6C, 0x100583C8);
}

/* WWHD glue (tools/verify/autoglue.py)
*/
