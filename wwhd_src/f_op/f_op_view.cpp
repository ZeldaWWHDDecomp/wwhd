/** View Process Framework. WWHD. */
#include "f_op/f_op_view.h"
static ViewHeaderStatics l_headerStatics;
static const ViewPiPair l_piPair;

/* 025DD050 */
BOOL fopVw_Draw(view_class* i_this) {
    return fpcLf_DrawMethod(i_this->sub_method, i_this);
}

/* 025DD05C */
BOOL fopVw_Execute(view_class* i_this) {
    return fpcMtd_Execute(&i_this->sub_method->base, i_this);
}

/* 025DD068 */
BOOL fopVw_IsDelete(view_class* i_this) {
    return fpcMtd_IsDelete(&i_this->sub_method->base, i_this);
}

/* 025DD074 */
BOOL fopVw_Delete(view_class* i_this) {
    return fpcMtd_Delete(&i_this->sub_method->base, i_this);
}

/* 025DD080 */
cPhs_State fopVw_Create(view_class* i_this) {
    view_process_profile_definition* profile = reinterpret_cast<view_process_profile_definition*>(i_this->mpProf.get());
    leafdraw_method_class* methods = profile->sub_method;
    i_this->sub_method = methods;
    i_this->mViewMode = profile->mViewMode;
    // HD: preserve the original method pointer across the byte store.
    return fpcMtd_Create(&methods->base, i_this);
}

/* 025DD09C */
static void __sinit_f_op_view_cpp() {
    // HD: common-header initialization retains the verified store/read/call order.
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101F3888);
    f32 first = l_piPair.negativePi;
    f32 second = l_piPair.positivePi;
    l_headerStatics.negativePi = first;
    l_headerStatics.positivePi = second;
    ViewFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F3894);
    ViewSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F38A0);
}

/* WWHD glue: data and other-unit calls
 data l_headerStatics 1048A64C
 data l_piPair 10057DC8
 ext 025DF2C0 s32 fpcLf_DrawMethod(leafdraw_method_class*, view_class*)
 ext 028ED6F8 void ViewFirstHeaderObject_ct(u8*)
 ext 028EAB2C void ViewSecondHeaderObject_ct(u8*)
*/
