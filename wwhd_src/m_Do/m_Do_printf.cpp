#include "bindings.h"
namespace m_Do_printf_cpp {
static void on(){WWHD_FUNC(0x025F26F0,void,(u32)0);gabi::store<u8>(0x1048D524,1);}
VERIFY(0x025F26F0,on);
static void off(){WWHD_FUNC(0x025F2700,void,(u32)0);gabi::store<u8>(0x1048D524,0);}
VERIFY(0x025F2700,off);
/* Original GHS frame: descriptor at+8 (12 bytes), GPR/FPR save area at+18 (96 bytes).
   Preserve untouched padding and point overflow arguments into the caller's frame. */
static void save(u32 f,const u32* r,bool floats){
 for(u32 i=0;i<8;++i)gabi::store<u32>(f+0x18+4*i,r[i]);
 if(floats)for(u32 i=0;i<8;++i){u64 b;memcpy(&b,&gabi::cpu->f[i+1].ps0,8);gabi::store<u32>(f+0x38+8*i,b>>32);gabi::store<u32>(f+0x3C+8*i,b);}
}
static void report(u32 f,u32 entry,u32 fmt,u32 count,u32 disabled,bool warning){
 gabi::store<u32>(count,gabi::load<u32>(count)+1);if(gabi::load<u8>(disabled))return;
 gabi::store<u32>(f+0xC,entry+8);gabi::store<u32>(f+0x10,f+0x18);gabi::store<u8>(f+8,1);gabi::store<u8>(f+9,0);
 gabi::call(0x025F26F0);
 if(warning){gabi::cpu->cr[6]=0;gabi::call(0xC0009EE8,0x100590B0u);}else gabi::call(0x028F24C4,0x100590A0u);
 gabi::call(0xC000A1D8,fmt,f+8);
 if(warning){gabi::cpu->cr[6]=0;gabi::call(0xC0009EE8,0x100590ACu);}else gabi::call(0x028F24C4,0x1005909Cu);
 gabi::call(0x025F2700);
}
static void Error(u32 fmt,u32 a4,u32 a5,u32 a6,u32 a7,u32 a8,u32 a9,u32 a10){
 WWHD_FUNC(0x025F2710,void,fmt,a4,a5,a6,a7,a8,a9,a10);
 u32 entry=gabi::cpu->r[1];bool floats=gabi::cpu->cr[6]!=0;gabi::Local<u8[0x80]> frame;
 const u32 regs[8]={fmt,a4,a5,a6,a7,a8,a9,a10};u32 f=gabi::ea(frame.get());save(f,regs,floats);report(f,entry,fmt,0x101F4948,0x1048D539,false);
}
VERIFY(0x025F2710,Error);
static void Warning(u32 fmt,u32 a4,u32 a5,u32 a6,u32 a7,u32 a8,u32 a9,u32 a10){
 WWHD_FUNC(0x025F27E8,void,fmt,a4,a5,a6,a7,a8,a9,a10);
 u32 entry=gabi::cpu->r[1];bool floats=gabi::cpu->cr[6]!=0;gabi::Local<u8[0x80]> frame;
 const u32 regs[8]={fmt,a4,a5,a6,a7,a8,a9,a10};u32 f=gabi::ea(frame.get());save(f,regs,floats);report(f,entry,fmt,0x101F494C,0x1048D53A,true);
}
VERIFY(0x025F27E8,Warning);
static void initialize(){WWHD_FUNC(0x025F28C8,void,(u32)0);
 gabi::store<u32>(0x1048D530,0);gabi::store<u32>(0x1048D528,0);gabi::store<u32>(0x1048D534,0);gabi::store<u32>(0x1048D52C,0);
 gabi::call(0x028F026C,0x101F4924u);
 gabi::store<f32>(0x1048D51C,gabi::load<f32>(0x100590C4));gabi::store<f32>(0x1048D520,gabi::load<f32>(0x100590C8));
 gabi::call(0x028ED6F8,0x1048D525u);gabi::call(0x028F026C,0x101F4930u);gabi::call(0x028EAB2C,0x1048D526u);gabi::call(0x028F026C,0x101F493Cu);
}
VERIFY(0x025F28C8,initialize);
}
