/* Qualified SDK initializer following d_save; gameplay event setup is inlined into merged d_save. */
#include "bindings.h"
namespace d_save_init_cpp {
static void initialize(){
 WWHD_FUNC(0x025BAF90,void,(u32)0);
 gabi::store<u32>(0x1047C940,0);gabi::store<u32>(0x1047C938,0);
 gabi::store<u32>(0x1047C944,0);gabi::store<u32>(0x1047C93C,0);
 gabi::call(0x028F026C,0x101EADFCu);
 f32 first=gabi::load<f32>(0x10054554),second=gabi::load<f32>(0x10054558);
 gabi::store<f32>(0x1047C92C,first);gabi::store<f32>(0x1047C930,second);
 gabi::call(0x028ED6F8,0x1047C934u);gabi::call(0x028F026C,0x101EAE08u);
 gabi::call(0x028EAB2C,0x1047C935u);gabi::call(0x028F026C,0x101EAE14u);
}
VERIFY(0x025BAF90,initialize);
}
