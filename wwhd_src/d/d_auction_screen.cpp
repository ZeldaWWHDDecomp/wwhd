#include "bindings.h"
// Qualified auction-controller flag adapters only; full legacy screen TU unresolved.
namespace auction_controller_adapters {
void talkStart() {
 WWHD_FUNC(0x0261BC08,void,(u32)0);
 u32 global=gabi::load<u32>(0x101F8344);
 u32 controller=gabi::load<u32>(global+0x1F4);
 gabi::store<u8>(controller+0x65,1);
}
VERIFY(0x0261BC08,talkStart);
void talkEnd() {
 WWHD_FUNC(0x0261BC20,void,(u32)0);
 u32 global=gabi::load<u32>(0x101F8344);
 u32 controller=gabi::load<u32>(global+0x1F4);
 gabi::store<u8>(controller+0x65,0);
}
VERIFY(0x0261BC20,talkEnd);
void slotShow() {
 WWHD_FUNC(0x0261BC38,void,(u32)0);
 u32 global=gabi::load<u32>(0x101F8344);
 u32 controller=gabi::load<u32>(global+0x1F4);
 gabi::store<u8>(controller+0x67,1);
}
VERIFY(0x0261BC38,slotShow);
void slotHide() {
 WWHD_FUNC(0x0261BC50,void,(u32)0);
 u32 global=gabi::load<u32>(0x101F8344);
 u32 controller=gabi::load<u32>(global+0x1F4);
 gabi::store<u8>(controller+0x67,0);
}
VERIFY(0x0261BC50,slotHide);
void gaugeShow() {
 WWHD_FUNC(0x0261BC68,void,(u32)0);
 u32 global=gabi::load<u32>(0x101F8344);
 u32 controller=gabi::load<u32>(global+0x1F4);
 gabi::store<u8>(controller+0x6A,1);
}
VERIFY(0x0261BC68,gaugeShow);
void gaugeHide() {
 WWHD_FUNC(0x0261BC80,void,(u32)0);
 u32 global=gabi::load<u32>(0x101F8344);
 u32 controller=gabi::load<u32>(global+0x1F4);
 gabi::store<u8>(controller+0x6A,0);
}
VERIFY(0x0261BC80,gaugeHide);
}
