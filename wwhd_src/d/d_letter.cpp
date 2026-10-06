#include "bindings.h"
static s32 no_send(u32 index) {WWHD_FUNC(0x02586D24,s32,index);return gabi::call<u32>(0x025B8BB0,gabi::load<u32>(0x101F84DC)+0x644,index)==0;}
VERIFY(0x02586D24,no_send);
static void send(u32 index) {WWHD_FUNC(0x02586D5C,void,index);gabi::call(0x025B8AF4,gabi::load<u32>(0x101F84DC)+0x644,index,1);}
VERIFY(0x02586D5C,send);
static s32 is_send(u32 index) {WWHD_FUNC(0x02586D74,s32,index);return gabi::call<u32>(0x025B8BB0,gabi::load<u32>(0x101F84DC)+0x644,index)==1;}
VERIFY(0x02586D74,is_send);
static void stock(u32 index) {WWHD_FUNC(0x02586DB0,void,index);gabi::call(0x025B8AF4,gabi::load<u32>(0x101F84DC)+0x644,index,2);}
VERIFY(0x02586DB0,stock);
static s32 is_stock(u32 index) {WWHD_FUNC(0x02586DC8,s32,index);return gabi::call<u32>(0x025B8BB0,gabi::load<u32>(0x101F84DC)+0x644,index)==2;}
VERIFY(0x02586DC8,is_stock);
static void mark_read(u32 index) {WWHD_FUNC(0x02586E04,void,index);gabi::call(0x025B8AF4,gabi::load<u32>(0x101F84DC)+0x644,index,3);}
VERIFY(0x02586E04,mark_read);
static s32 is_read(u32 index) {WWHD_FUNC(0x02586E1C,s32,index);return gabi::call<u32>(0x025B8BB0,gabi::load<u32>(0x101F84DC)+0x644,index)==3;}
VERIFY(0x02586E1C,is_read);
static void deliver(u32 index) {WWHD_FUNC(0x02586E58,void,index);if(gabi::call<s32>(0x02586D74,index))gabi::call(0x02586DB0,index);}
VERIFY(0x02586E58,deliver);
static void auto_stock(u32 index) {WWHD_FUNC(0x02586E94,void,index);if(gabi::call<s32>(0x02586D24,index))gabi::call(0x02586DB0,index);}
VERIFY(0x02586E94,auto_stock);
static u32 is_delivery(u32 index) {WWHD_FUNC(0x02586ED0,u32,index);return (gabi::call<u32>(0x02586D24,index)^1)&255;}
VERIFY(0x02586ED0,is_delivery);
static void letter_init() {WWHD_FUNC(0x02586EF8,void,(u32)0);for(u32 i=0;i<4;i++)gabi::store<u32>(0x1047761C+i*4,0);gabi::call(0x028F026C,0x101E9B90);gabi::store<f32>(0x10477610,gabi::load<f32>(0x100505E4));gabi::store<f32>(0x10477614,gabi::load<f32>(0x100505E8));gabi::call(0x028ED6F8,0x10477618);gabi::call(0x028F026C,0x101E9B9C);gabi::call(0x028EAB2C,0x10477619);gabi::call(0x028F026C,0x101E9BA8);}
VERIFY(0x02586EF8,letter_init);
