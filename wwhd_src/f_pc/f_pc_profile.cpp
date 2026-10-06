/* f_pc_profile: WWHD process profile lookup and per-unit initializer.
 * HD embeds the table instead of a mutable table pointer. */
#include "gabi.h"
namespace f_pc_profile_cpp {
u32 get(u32 profile) {
 WWHD_FUNC(0x025E10E0,u32,profile);
 // HD does not sign-extend r3 at entry. The caller supplies its intended profile index.
 return gabi::load<u32>(0x101F3EE0u + (profile << 2));
}
VERIFY(0x025E10E0,get);
void init() {
 WWHD_FUNC(0x025E10F4,void);
 gabi::store<u32>(0x1048AB60,0);
 gabi::store<u32>(0x1048AB58,0);
 gabi::store<u32>(0x1048AB64,0);
 gabi::store<u32>(0x1048AB5C,0);
 gabi::call(0x028F026C,0x101F3E98u);
 f32 first=gabi::load<f32>(0x1005846C),second=gabi::load<f32>(0x10058470);
 gabi::store<f32>(0x1048AB4C,first);
 gabi::store<f32>(0x1048AB50,second);
 gabi::call(0x028ED6F8,0x1048AB54u);
 gabi::call(0x028F026C,0x101F3EA4u);
 gabi::call(0x028EAB2C,0x1048AB55u);
 gabi::call(0x028F026C,0x101F3EB0u);
}
VERIFY(0x025E10F4,init);
}
