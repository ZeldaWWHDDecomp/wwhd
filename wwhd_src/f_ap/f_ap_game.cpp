/* Full HD game-application TU42C4..4603. Previous HIO vtable10056DC0 links
 * preceding stubs42A0..42C0 to water-pot TU; own HIO stores vtable10056E04. */
#include "bindings.h"
namespace f_ap_game_cpp {
static void after(){
 WWHD_FUNC(0x025D42C4,void,(u32)0);
 gabi::call(0x025DC98C);gabi::call(0x025DBEEC);gabi::call(0x025DA750);
}
VERIFY(0x025D42C4,after);
static void execute(){
 WWHD_FUNC(0x025D42EC,void,(u32)0);
 gabi::call(0x025DF948,0u,0x025D42C4u);gabi::call(0x0200E6EC,0u);
}
VERIFY(0x025D42EC,execute);
static void create(){
 WWHD_FUNC(0x025D4320,void,(u32)0);
 gabi::call(0x025DFA7C);gabi::call(0x025DCA60);gabi::call(0x025DC04C);
 gabi::call(0x025DA754);gabi::call(0x025DA8AC);
 gabi::call(0x025DC91C,5u,0x7FFFu,0u,0u);
 u32 no=gabi::call<u32>(0x025F0A10,0x10056E14u,0x104873CCu);
 gabi::store<u8>(0x104873CC,no);
}
VERIFY(0x025D4320,create);
static u32 HIO_constructor(u32 self){
 WWHD_FUNC(0x025D4384,u32,self);
 if(!self){self=gabi::call<u32>(0x0273AD10,0x58u);if(!self)return 0;}
 gabi::store<u32>(self+0x42,0xFFFFFFFF);gabi::store<u32>(self+0x19,0xFFFFFFFF);
 gabi::store<u32>(self+0x36,0xFFFFFFFF);gabi::store<u32>(self+0x54,0x10056E04);
 gabi::store<u32>(self+0x3A,0xFFFFFFFF);gabi::store<u8>(self+1,1);
 gabi::store<u32>(self+0x3E,0xFFFFFFFF);
 bool dev=gabi::load<u8>(0x101F48C0)!=0;
 gabi::store<u8>(self+2,dev?1:0);
 gabi::store<u8>(self+0xC,0);gabi::store<u8>(self+0x2C,0);
 gabi::store<u8>(self+5,1);gabi::store<u8>(self+0x10,10);gabi::store<u8>(self+0x11,12);
 gabi::store<u8>(self+0xD,0);gabi::store<u8>(self+3,dev?1:0);gabi::store<u8>(self+0x17,1);
 gabi::store<u8>(self+0x14,8);gabi::store<f32>(self+8,gabi::load<f32>(0x10056E24));
 gabi::store<u8>(self+0xE,8);gabi::store<u16>(self+0x2A,480);gabi::store<u8>(self+0x13,8);
 gabi::store<u8>(self+0x18,0);gabi::store<u8>(self+0xF,8);gabi::store<u8>(self+0x12,10);
 gabi::store<u8>(self+6,1);gabi::store<u8>(self+0x16,0);gabi::store<u8>(self+0x15,255);
 gabi::store<u16>(self+0x28,640);gabi::store<u32>(self+0x19,0xFFFFFFFF);
 gabi::store<f32>(self+0x20,gabi::load<f32>(0x10056E28));gabi::store<f32>(self+0x24,gabi::load<f32>(0x10056E2C));
 gabi::store<u32>(self+0x36,0xFF9600FF);gabi::store<u32>(self+0x3A,0xFF7800FF);
 gabi::store<u32>(self+0x3E,0x000000FF);gabi::store<u32>(self+0x42,0x000000FF);
 gabi::store<u16>(self+0x48,10);gabi::store<u16>(self+0x4E,27);gabi::store<u16>(self+0x4A,0);
 gabi::store<u8>(self+0x50,130);gabi::store<u16>(self+0x4C,0);gabi::store<u16>(self+0x46,0);
 return self;
}
VERIFY(0x025D4384,HIO_constructor);
static void initialize(){
 WWHD_FUNC(0x025D4564,void,(u32)0);
 gabi::store<u32>(0x104873C4,0);gabi::store<u32>(0x104873BC,0);gabi::store<u32>(0x104873C8,0);gabi::store<u32>(0x104873C0,0);
 gabi::call(0x028F026C,0x101F3038u);
 gabi::store<f32>(0x104873B0,gabi::load<f32>(0x10056E34));gabi::store<f32>(0x104873B4,gabi::load<f32>(0x10056E38));
 gabi::call(0x028ED6F8,0x104873B8u);gabi::call(0x028F026C,0x101F3044u);
 gabi::call(0x028EAB2C,0x104873B9u);gabi::call(0x028F026C,0x101F3050u);gabi::call(0x025D4384,0x104873CCu);
}
VERIFY(0x025D4564,initialize);
}
