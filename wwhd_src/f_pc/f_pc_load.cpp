/* f_pc_load (WWHD): the GameCube dynamic actor-load APIs are absent here.
 * Native unit retains only its static initializer025DF870..025DF900.
 * Attribution by neighboring unit boundaries; */
#include "gabi.h"
namespace f_pc_load_cpp {
void init() {
 WWHD_FUNC(0x025DF870,void);
 gabi::store<u32>(0x1048A988,0);
 gabi::store<u32>(0x1048A980,0);
 gabi::store<u32>(0x1048A98C,0);
 gabi::store<u32>(0x1048A984,0);
 gabi::call(0x028F026C,0x101F3C64u);
 f32 first=gabi::load<f32>(0x10058378),second=gabi::load<f32>(0x1005837C);
 gabi::store<f32>(0x1048A974,first);
 gabi::store<f32>(0x1048A978,second);
 gabi::call(0x028ED6F8,0x1048A97Cu);
 gabi::call(0x028F026C,0x101F3C70u);
 gabi::call(0x028EAB2C,0x1048A97Du);
 gabi::call(0x028F026C,0x101F3C7Cu);
}
VERIFY(0x025DF870,init);
}
