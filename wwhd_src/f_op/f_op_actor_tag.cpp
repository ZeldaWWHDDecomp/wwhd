/* f_op_actor_tag: WWHD actor queue tag wrappers and TU initializer.
 * Native range025DA2F8..025DA3C0. */
#include "gabi.h"
namespace f_op_actor_tag_cpp {
u32 toActorQueue(u32 tag) {
 WWHD_FUNC(0x025DA2F8,u32,tag);
 return gabi::call<u32>(0x0201A8B4,0x101F3328u,tag);
}
VERIFY(0x025DA2F8,toActorQueue);
u32 fromActorQueue(u32 tag) {
 WWHD_FUNC(0x025DA308,u32,tag);
 return gabi::call<u32>(0x0201A724,tag);
}
VERIFY(0x025DA308,fromActorQueue);
u32 initTag(u32 tag,u32 data) {
 WWHD_FUNC(0x025DA30C,u32,tag,data);
 gabi::call(0x0201A918,tag,data);
 return 1;
}
VERIFY(0x025DA30C,initTag);
void init() {
 WWHD_FUNC(0x025DA330,void);
 gabi::store<u32>(0x10487524,0);
 gabi::store<u32>(0x1048751C,0);
 gabi::store<u32>(0x10487528,0);
 gabi::store<u32>(0x10487520,0);
 gabi::call(0x028F026C,0x101F3304u);
 f32 first=gabi::load<f32>(0x10057944),second=gabi::load<f32>(0x10057948);
 gabi::store<f32>(0x10487510,first);
 gabi::store<f32>(0x10487514,second);
 gabi::call(0x028ED6F8,0x10487518u);
 gabi::call(0x028F026C,0x101F3310u);
 gabi::call(0x028EAB2C,0x10487519u);
 gabi::call(0x028F026C,0x101F331Cu);
}
VERIFY(0x025DA330,init);
}
