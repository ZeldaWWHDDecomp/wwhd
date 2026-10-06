/* WWHD HUD range 0259359C..<02599584. */
#include "bindings.h"
#include <initializer_list>
namespace d_meter_01_cpp {
static float loadf(u32 p) {return gabi::load<float>(p);}
static void storef(u32 p,float v){gabi::store<float>(p,v);}
static void resetHud(u32 self){
 WWHD_FUNC(0x0259359C,void,self);
 const u32 hio=0x1047ACEC;
 gmem_st8(0x1047A961,0);gmem_st8(0x1047A962,0);
 gmem_st16(0x1047A948,gmem_ld16(hio+0x10));gmem_st8(0x1047A963,0);
 gmem_st16(0x1047A950,gmem_ld16(hio+6));gmem_st16(0x1047A94A,gmem_ld16(hio+0x12));
 gmem_st16(0x1047A94E,gmem_ld16(hio+4));gmem_st8(0x1047A960,1);
 float z=loadf(0x100512B8);storef(0x1047A88C,z);gmem_st8(0x1047A958,0);storef(0x1047A890,z);
 gmem_st8(self,0);gmem_st8(self+2,0);gmem_st16(self+4,0);gmem_st8(self+1,0);
}
VERIFY(0x0259359C,resetHud);
static s32 isReady(){WWHD_FUNC(0x02595848,s32);return 1;}
VERIFY(0x02595848,isReady);
static u32 initSmallHio(u32 self){
 WWHD_FUNC(0x02598254,u32,self);
 if(!self){self=gabi::call<u32>(0x0273AD10,0x20);if(!self)return 0;}
 gmem_st16(self+8,-180);storef(self+0x18,loadf(0x100512B8));gmem_st16(self+0x12,150);
 gmem_st8(self+3,1);gmem_st32(self+0x1c,0x10051114);gmem_st16(self+4,35);
 gmem_st8(self+0x14,0);gmem_st16(self+6,338);gmem_st8(self+1,0);gmem_st8(self+2,200);
 storef(self+0xc,loadf(0x100514EC));gmem_st8(self,0);gmem_st16(self+0x10,590);return self;
}
VERIFY(0x02598254,initSmallHio);
static void drawOverlay(){
 WWHD_FUNC(0x025982F4,void);
 u32 port=gmem_ld32(gabi::call<u32>(0x025200D4)+0x5d10);
 gabi::call_ptr(gmem_ld32(gmem_ld32(port+0xb8)+0x24),port);
 float z=loadf(0x100512B8);
 gabi::call(0x027F0688,gmem_ld32(0x1047B0A8),port,z,z);
 gabi::call(0x027F0688,gmem_ld32(0x1047B0A0),port,z,z);
 u32 popup=gmem_ld32(0x101EA0F4);
 if(popup){gmem_st8(popup+0xcc,1);gabi::call(0x027F0688,gmem_ld32(0x101EA0F4),port,z,z);}
 u32 extra=gmem_ld32(0x101EA0F8);if(extra)gabi::call(0x027F0688,extra,port,z,z);
 gabi::call(0x027F0688,gmem_ld32(0x101EA0FC),port,z,z);
}
VERIFY(0x025982F4,drawOverlay);
static void drawSingle(){
 WWHD_FUNC(0x025983E8,void);
 u32 port=gmem_ld32(gabi::call<u32>(0x025200D4)+0x5d10);
 gabi::call_ptr(gmem_ld32(gmem_ld32(port+0xb8)+0x24),port);
 float z=loadf(0x100512B8);gabi::call(0x027F0688,gmem_ld32(0x1047B0A4),port,z,z);
}
VERIFY(0x025983E8,drawSingle);
static void destroyHioArray(){WWHD_FUNC(0x0259868C,void);gabi::call(0x028F0164,0x1047AAA0,3,0xc4,0x0259CFB4,0,0);}
VERIFY(0x0259868C,destroyHioArray);
static u8 getterA(){WWHD_FUNC(0x025986B0,u8);return gmem_ld8(0x101EA06C);}
VERIFY(0x025986B0,getterA);
static u8 getterB(){WWHD_FUNC(0x025986BC,u8);return gmem_ld8(0x101EA069);}
VERIFY(0x025986BC,getterB);
static void deletingDestructor(u32 self,s32 flags){WWHD_FUNC(0x025986C8,void,self,flags);if(self&&(flags&1))gabi::call(0x0273AF40,self);}
VERIFY(0x025986C8,deletingDestructor);
static u32 hio_02597E34(u32 self){
 WWHD_FUNC(0x02597E34,u32,self);
 if(!self){self=gabi::call<u32>(0x0273AD10,0x88);if(!self)return 0;}
 gmem_st8(self + 0x00000005,0x1e);
 gmem_st8(self + 0x00000011,0x1e);
 gmem_st32(self + 0x00000000,0x100510e4);
 gmem_st8(self + 0x0000000A,0x4b);
 gmem_st8(self + 0x0000001F,0xff);
 gmem_st8(self + 0x00000013,0x0);
 gmem_st8(self + 0x00000007,0xd7);
 gmem_st16(self + 0x00000058,0x0);
 gmem_st8(self + 0x00000018,0x78);
 gmem_st8(self + 0x0000000C,0x1e);
 gmem_st8(self + 0x00000086,0x0);
 gmem_st8(self + 0x0000000F,0xdc);
 gmem_st8(self + 0x00000012,0xb4);
 gmem_st8(self + 0x00000010,0x1e);
 gmem_st16(self + 0x00000064,0xf);
 gmem_st8(self + 0x00000006,0x1e);
 gmem_st8(self + 0x0000001E,0x3c);
 gmem_st8(self + 0x0000000E,0x1e);
 gmem_st8(self + 0x00000022,0xff);
 gmem_st8(self + 0x00000008,0x1e);
 gmem_st8(self + 0x00000019,0x78);
 gmem_st8(self + 0x00000027,0xff);
 gmem_st8(self + 0x00000021,0xff);
 gmem_st8(self + 0x00000084,0x0);
 gmem_st8(self + 0x00000016,0x0);
 gmem_st16(self + 0x00000056,0x0);
 gmem_st16(self + 0x00000062,0x0);
 gmem_st8(self + 0x00000004,0x1e);
 gmem_st8(self + 0x00000014,0x0);
 gmem_st8(self + 0x00000017,0xff);
 gmem_st8(self + 0x00000082,0x0);
 gmem_st8(self + 0x0000000D,0x1e);
 gmem_st8(self + 0x00000020,0x0);
 gmem_st8(self + 0x00000009,0x1e);
 gmem_st8(self + 0x0000001C,0xff);
 gmem_st8(self + 0x00000026,0xff);
 gmem_st8(self + 0x00000015,0xff);
 gmem_st8(self + 0x00000060,0x0);
 gmem_st8(self + 0x0000000B,0x0);
 gmem_st8(self + 0x00000085,0x0);
 gmem_st8(self + 0x0000007E,0x2);
 gmem_st16(self + 0x0000005A,0x1e);
 gmem_st8(self + 0x00000025,0x0);
 gmem_st8(self + 0x0000006E,0x0);
 gmem_st16(self + 0x00000068,0x0);
 gmem_st8(self + 0x0000007A,0xa);
 gmem_st16(self + 0x0000006C,0x17);
 gmem_st8(self + 0x0000007F,0x0);
 gmem_st8(self + 0x00000081,0x0);
 gmem_st8(self + 0x00000024,0xff);
 gmem_st8(self + 0x0000001D,0xff);
 gmem_st8(self + 0x0000007C,0xb4);
 gmem_st8(self + 0x00000023,0xff);
 gmem_st16(self + 0x00000074,0x0);
 gmem_st16(self + 0x0000005C,0xd);
 gmem_st8(self + 0x0000001A,0xff);
 gmem_st8(self + 0x00000080,0x1);
 gmem_st8(self + 0x0000007B,0xaa);
 storef(self + 0x00000050,loadf(0x100514dc));
 gmem_st8(self + 0x00000054,0x1d);
 gmem_st8(self + 0x00000083,0xa);
 gmem_st16(self + 0x00000066,0x0);
 gmem_st8(self + 0x0000001B,0xff);
 gmem_st16(self + 0x0000006A,0xf0);
 gmem_st16(self + 0x00000048,0x4);
 gmem_st16(self + 0x00000044,0x4);
 gmem_st16(self + 0x0000005E,0x0);
 gmem_st8(self + 0x0000002C,0xff);
 gmem_st8(self + 0x0000002A,0x50);
 gmem_st16(self + 0x00000078,0x46);
 gmem_st16(self + 0x00000046,0x2);
 gmem_st8(self + 0x0000002E,0x96);
 gmem_st16(self + 0x00000076,0x0);
 gmem_st16(self + 0x00000070,0x80);
 gmem_st8(self + 0x00000029,0x50);
 storef(self + 0x00000034,loadf(0x100514e0));
 storef(self + 0x00000038,loadf(0x100514e4));
 gmem_st8(self + 0x0000007D,0x46);
 gmem_st8(self + 0x0000002D,0x96);
 storef(self + 0x0000003C,loadf(0x100512c8));
 gmem_st16(self + 0x00000042,0x3);
 gmem_st8(self + 0x0000002B,0x96);
 storef(self + 0x0000004C,loadf(0x100514d8));
 gmem_st16(self + 0x00000040,0x5);
 gmem_st8(self + 0x0000002F,0xff);
 gmem_st8(self + 0x00000028,0xff);
 gmem_st16(self + 0x00000072,0xa);
 return self;
}
VERIFY(0x02597E34,hio_02597E34);
static u32 hio_02598058(u32 self){
 WWHD_FUNC(0x02598058,u32,self);
 if(!self){self=gabi::call<u32>(0x0273AD10,0x80);if(!self)return 0;}
 gmem_st8(self + 0x00000004,0x0);
 gmem_st8(self + 0x0000002B,0xff);
 gmem_st8(self + 0x00000012,0xff);
 gmem_st32(self + 0x00000000,0x100510fc);
 gmem_st8(self + 0x00000009,0xff);
 gmem_st8(self + 0x00000028,0x80);
 gmem_st8(self + 0x0000000A,0xff);
 gmem_st8(self + 0x0000002D,0xb4);
 gmem_st8(self + 0x00000023,0xff);
 gmem_st8(self + 0x00000005,0x0);
 gmem_st8(self + 0x00000011,0xff);
 gmem_st8(self + 0x00000027,0x0);
 gmem_st8(self + 0x00000008,0xff);
 gmem_st8(self + 0x0000001C,0x0);
 gmem_st8(self + 0x0000002A,0xff);
 gmem_st16(self + 0x00000034,0x2a);
 gmem_st8(self + 0x00000029,0xff);
 gmem_st8(self + 0x00000022,0xff);
 gmem_st8(self + 0x00000024,0xff);
 gmem_st8(self + 0x00000025,0xff);
 gmem_st8(self + 0x00000016,0xff);
 gmem_st8(self + 0x00000010,0xff);
 gmem_st8(self + 0x00000014,0xff);
 gmem_st8(self + 0x00000013,0xff);
 gmem_st8(self + 0x00000007,0xff);
 gmem_st8(self + 0x0000000C,0xff);
 gmem_st8(self + 0x00000020,0xff);
 gmem_st8(self + 0x00000017,0xff);
 gmem_st8(self + 0x00000021,0xff);
 gmem_st8(self + 0x0000002C,0xff);
 gmem_st8(self + 0x0000001F,0xff);
 gmem_st8(self + 0x0000000E,0xff);
 gmem_st8(self + 0x0000002F,0x4b);
 gmem_st8(self + 0x0000000B,0xff);
 gmem_st8(self + 0x0000000D,0xff);
 gmem_st8(self + 0x00000038,0x1);
 gmem_st8(self + 0x00000026,0xff);
 gmem_st8(self + 0x00000019,0xff);
 gmem_st8(self + 0x0000000F,0x0);
 gmem_st16(self + 0x00000032,0xfffe);
 gmem_st16(self + 0x00000036,0xf);
 gmem_st16(self + 0x0000003A,0x16);
 gmem_st8(self + 0x0000001E,0x0);
 gmem_st8(self + 0x00000006,0x0);
 gmem_st8(self + 0x0000002E,0xa0);
 gmem_st8(self + 0x0000001B,0x0);
 gmem_st8(self + 0x00000046,0x0);
 gmem_st8(self + 0x00000015,0xff);
 gmem_st8(self + 0x00000039,0xa);
 gmem_st8(self + 0x00000040,0x0);
 storef(self + 0x0000003C,loadf(0x100514e8));
 gmem_st8(self + 0x00000018,0xff);
 gmem_st8(self + 0x00000031,0x1b);
 gmem_st16(self + 0x00000072,0x1);
 gmem_st16(self + 0x00000076,0x4);
 gmem_st16(self + 0x00000078,0x4);
 gmem_st16(self + 0x0000007E,0x18c);
 gmem_st16(self + 0x0000005E,0x0);
 gmem_st16(self + 0x0000007A,0x2);
 gmem_st8(self + 0x00000049,0xff);
 gmem_st8(self + 0x00000074,0x0);
 gmem_st8(self + 0x0000004C,0x0);
 gmem_st8(self + 0x0000004A,0xff);
 gmem_st8(self + 0x00000030,0x0);
 gmem_st16(self + 0x00000042,0x1);
 gmem_st16(self + 0x0000006C,0x1);
 gmem_st8(self + 0x00000059,0x0);
 gmem_st16(self + 0x0000007C,0x258);
 gmem_st8(self + 0x0000005A,0x0);
 gmem_st16(self + 0x00000050,0x0);
 gmem_st8(self + 0x00000048,0xff);
 gmem_st8(self + 0x00000058,0x0);
 gmem_st8(self + 0x0000004F,0x0);
 gmem_st8(self + 0x00000056,0x0);
 gmem_st8(self + 0x0000004D,0x0);
 gmem_st16(self + 0x0000006E,0x1);
 gmem_st16(self + 0x00000070,0x1);
 gmem_st16(self + 0x00000064,0x0);
 gmem_st16(self + 0x00000044,0x0);
 gmem_st16(self + 0x0000005C,0x0);
 gmem_st8(self + 0x0000001A,0xff);
 gmem_st16(self + 0x00000054,0x0);
 gmem_st16(self + 0x00000066,0x0);
 gmem_st16(self + 0x00000068,0x1);
 gmem_st16(self + 0x00000060,0x0);
 gmem_st16(self + 0x0000006A,0x1);
 gmem_st8(self + 0x00000047,0x1);
 gmem_st16(self + 0x00000062,0x0);
 gmem_st8(self + 0x0000001D,0x0);
 gmem_st16(self + 0x00000052,0x0);
 gmem_st8(self + 0x0000004B,0xff);
 gmem_st8(self + 0x0000004E,0x0);
 gmem_st8(self + 0x00000057,0xff);
 return self;
}
VERIFY(0x02598058,hio_02598058);
static u32 hio_025974DC(u32 self){
 WWHD_FUNC(0x025974DC,u32,self);
 if(!self){self=gabi::call<u32>(0x0273AD10,0x158);if(!self)return 0;}
 gmem_st16(self + 0x00000004,0x0);
 storef(self + 0x0000000C,loadf(0x100512b8));
 gmem_st32(self + 0x00000000,0x100510b4);
 gmem_st16(self + 0x00000006,0x0);
 storef(self + 0x00000010,loadf(0x100512b8));
 gmem_st8(self + 0x0000002C,0x0);
 storef(self + 0x00000014,loadf(0x100512b8));
 gmem_st16(self + 0x0000002E,0x0);
 storef(self + 0x00000018,loadf(0x100512b8));
 gmem_st16(self + 0x00000030,0x0);
 storef(self + 0x0000001C,loadf(0x100512b8));
 gmem_st16(self + 0x00000046,0x0);
 gmem_st16(self + 0x0000006A,0x0);
 gmem_st16(self + 0x00000044,0x0);
 gmem_st16(self + 0x00000042,0x0);
 gmem_st16(self + 0x00000050,0x0);
 storef(self + 0x0000004C,loadf(0x100512b8));
 gmem_st16(self + 0x00000008,0x0);
 gmem_st16(self + 0x00000040,0x0);
 gmem_st16(self + 0x00000052,0x0);
 storef(self + 0x00000048,loadf(0x100512b8));
 gmem_st16(self + 0x00000054,0x0);
 gmem_st16(self + 0x00000038,0x0);
 gmem_st16(self + 0x00000056,0x0);
 storef(self + 0x0000003C,loadf(0x100512b8));
 storef(self + 0x00000058,loadf(0x100512b8));
 gmem_st16(self + 0x00000036,0x0);
 storef(self + 0x0000005C,loadf(0x100512b8));
 storef(self + 0x00000028,loadf(0x100512b8));
 gmem_st8(self + 0x00000060,0x0);
 gmem_st16(self + 0x00000034,0x0);
 gmem_st8(self + 0x00000061,0x0);
 storef(self + 0x00000024,loadf(0x100512b8));
 gmem_st8(self + 0x00000062,0x0);
 gmem_st16(self + 0x00000032,0x0);
 gmem_st8(self + 0x00000063,0x0);
 storef(self + 0x00000020,loadf(0x100512b8));
 gmem_st16(self + 0x00000068,0x0);
 gmem_st16(self + 0x00000066,0x0);
 gmem_st8(self + 0x0000002D,0x0);
 gmem_st16(self + 0x00000064,0x0);
 gabi::call(0x028F521C,self + 0x0000006C,0x6);
 gabi::call(0x028F521C,self + 0x00000072,0x6);
 gabi::call(0x028F521C,self + 0x00000078,0x6);
 gmem_st16(self + 0x00000086,0x0);
 gmem_st16(self + 0x0000008C,0x0);
 gmem_st16(self + 0x00000088,0x0);
 gmem_st16(self + 0x00000082,0x0);
 gmem_st16(self + 0x0000007E,0x0);
 gmem_st16(self + 0x0000009C,0x0);
 gmem_st16(self + 0x00000092,0x0);
 gmem_st16(self + 0x00000090,0x0);
 gmem_st16(self + 0x0000009A,0x0);
 gmem_st16(self + 0x00000094,0x0);
 gmem_st16(self + 0x00000098,0x0);
 gmem_st16(self + 0x00000096,0x0);
 gmem_st16(self + 0x00000080,0x0);
 gmem_st16(self + 0x0000008A,0x0);
 gmem_st16(self + 0x0000009E,0x0);
 gmem_st16(self + 0x00000084,0x0);
 gmem_st16(self + 0x0000008E,0x0);
 gabi::call(0x028F521C,self + 0x000000A0,0x8);
 gabi::call(0x028F521C,self + 0x000000A8,0x8);
 gabi::call(0x028F521C,self + 0x000000B0,0x8);
 gabi::call(0x028F521C,self + 0x000000B8,0x8);
 gabi::call(0x028F521C,self + 0x000000C0,0x8);
 gabi::call(0x028F521C,self + 0x000000C8,0x8);
 gabi::call(0x028F521C,self + 0x000000D0,0x8);
 gabi::call(0x028F521C,self + 0x000000D8,0x8);
 gabi::call(0x028F521C,self + 0x000000E0,0x8);
 gabi::call(0x028F521C,self + 0x000000E8,0x8);
 gabi::call(0x028F521C,self + 0x000000F0,0x8);
 gabi::call(0x028F521C,self + 0x000000F8,0x8);
 gabi::call(0x028F521C,self + 0x00000100,0x8);
 gabi::call(0x028F521C,self + 0x00000108,0x8);
 gabi::call(0x028F521C,self + 0x00000110,0x8);
 gabi::call(0x028F521C,self + 0x00000118,0x8);
 gmem_st16(self + 0x0000012A,0x0);
 gmem_st16(self + 0x0000013E,0x0);
 gmem_st8(self + 0x00000121,0x0);
 gmem_st16(self + 0x00000128,0x0);
 gmem_st8(self + 0x00000122,0x0);
 gmem_st16(self + 0x00000136,0x0);
 gmem_st16(self + 0x00000130,0x0);
 gmem_st16(self + 0x0000012C,0x0);
 gmem_st16(self + 0x00000140,0x0);
 gmem_st16(self + 0x00000132,0x0);
 gmem_st16(self + 0x00000138,0x0);
 gmem_st16(self + 0x00000124,0x0);
 gmem_st16(self + 0x0000012E,0x0);
 gmem_st16(self + 0x00000126,0x0);
 gmem_st16(self + 0x00000134,0x0);
 gmem_st16(self + 0x0000013A,0x0);
 gmem_st8(self + 0x00000123,0x0);
 gmem_st8(self + 0x00000120,0x0);
 gmem_st16(self + 0x0000013C,0x0);
 gabi::call(0x028F521C,self + 0x00000142,0x4);
 gabi::call(0x028F521C,self + 0x00000146,0x4);
 gabi::call(0x028F521C,self + 0x0000014A,0x4);
 gmem_st16(self + 0x00000030,0x0);
 gmem_st8(self + 0x00000063,0x0);
 storef(self + 0x00000014,loadf(0x100512b8));
 gmem_st16(self + 0x00000038,0x0);
 gmem_st16(self + 0x00000036,0x16);
 storef(self + 0x0000005C,loadf(0x100514a0));
 storef(self + 0x0000000C,loadf(0x100512b4));
 storef(self + 0x0000001C,loadf(0x100512b4));
 gmem_st16(self + 0x00000032,0x0);
 gmem_st16(self + 0x00000040,0x5a);
 storef(self + 0x00000018,loadf(0x1005148c));
 gmem_st8(self + 0x0000002C,0xff);
 storef(self + 0x0000003C,loadf(0x10051490));
 storef(self + 0x00000020,loadf(0x10051494));
 gmem_st8(self + 0x00000062,0x0);
 gmem_st16(self + 0x00000064,0x0);
 gmem_st16(self + 0x00000004,0xa);
 storef(self + 0x00000048,loadf(0x1005149c));
 gmem_st16(self + 0x00000068,0x0);
 gmem_st16(self + 0x00000044,0x64);
 gmem_st16(self + 0x0000002E,0x0);
 storef(self + 0x00000024,loadf(0x10051498));
 storef(self + 0x00000058,loadf(0x100514a4));
 gmem_st8(self + 0x00000061,0x64);
 gmem_st16(self + 0x00000034,0x0);
 gmem_st16(self + 0x0000006A,0x0);
 storef(self + 0x00000010,loadf(0x10051488));
 gmem_st16(self + 0x00000042,0x64);
 gmem_st16(self + 0x00000052,0xfff4);
 gmem_st16(self + 0x00000050,0x7);
 gmem_st16(self + 0x00000056,0xffa8);
 storef(self + 0x0000004C,loadf(0x100514a0));
 gmem_st8(self + 0x0000002D,0x8c);
 gmem_st16(self + 0x00000066,0x0);
 gmem_st8(self + 0x00000060,0x96);
 gmem_st16(self + 0x00000054,0x0);
 gmem_st16(self + 0x00000088,0xf);
 gmem_st16(self + 0x00000046,0x5a);
 gmem_st16(self + 0x00000096,0x3);
 gmem_st16(self + 0x00000132,0x27);
 gmem_st16(self + 0x0000013C,0xa);
 gmem_st16(self + 0x0000012E,0x2);
 gmem_st16(self + 0x0000013E,0x0);
 gmem_st16(self + 0x0000008E,0x5);
 gmem_st16(self + 0x00000092,0xc);
 gmem_st8(self + 0x00000147,0x0);
 gmem_st8(self + 0x0000014C,0x32);
 gmem_st8(self + 0x00000123,0x1);
 gmem_st8(self + 0x00000143,0x0);
 gmem_st16(self + 0x0000009A,0x0);
 gmem_st8(self + 0x0000014A,0xff);
 gmem_st8(self + 0x00000121,0xc8);
 gmem_st16(self + 0x0000014E,0xa);
 gmem_st16(self + 0x0000013A,0x0);
 gmem_st16(self + 0x00000130,0x14);
 gmem_st8(self + 0x00000144,0x0);
 gmem_st8(self + 0x00000122,0x96);
 gmem_st8(self + 0x00000149,0x0);
 gmem_st16(self + 0x000000AC,0x0);
 gmem_st16(self + 0x00000136,0x4);
 storef(self + 0x00000028,loadf(0x100514a8));
 gmem_st16(self + 0x0000008A,0x5);
 gmem_st16(self + 0x00000090,0x14);
 gmem_st8(self + 0x00000145,0xff);
 gmem_st8(self + 0x00000148,0x0);
 gmem_st16(self + 0x00000134,0x3);
 gmem_st16(self + 0x0000007E,0x14);
 gmem_st16(self + 0x0000012C,0x3);
 gmem_st16(self + 0x00000126,0xa);
 gmem_st8(self + 0x00000120,0x8c);
 gmem_st16(self + 0x00000138,0xa);
 gmem_st16(self + 0x00000140,0x0);
 gmem_st16(self + 0x00000098,0x3);
 gmem_st16(self + 0x000000A0,0x0);
 gmem_st16(self + 0x0000008C,0x5);
 gmem_st16(self + 0x0000012A,0x3);
 gmem_st16(self + 0x00000124,0x5);
 gmem_st8(self + 0x00000142,0xb4);
 gmem_st16(self + 0x000000A4,0x0);
 gmem_st16(self + 0x00000128,0x6);
 gmem_st16(self + 0x000000A6,0x0);
 gmem_st8(self + 0x00000146,0x0);
 gmem_st16(self + 0x00000152,0x4);
 gmem_st8(self + 0x0000014B,0x32);
 gmem_st16(self + 0x000000A8,0x0);
 gmem_st16(self + 0x000000C6,0x0);
 gmem_st16(self + 0x000000B0,0x19);
 gmem_st16(self + 0x000000B8,0xffe2);
 gmem_st16(self + 0x000000B2,0x23);
 gmem_st16(self + 0x000000BA,0xffea);
 gmem_st16(self + 0x000000F8,0x0);
 gmem_st16(self + 0x000000D2,0x28);
 gmem_st16(self + 0x000000B4,0xffdf);
 gmem_st16(self + 0x000000AE,0x0);
 gmem_st16(self + 0x000000A2,0x3c);
 gmem_st16(self + 0x000000DA,0xffc4);
 gmem_st16(self + 0x000000C2,0x0);
 gmem_st16(self + 0x000000FE,0x0);
 gmem_st16(self + 0x000000BE,0x0);
 gmem_st16(self + 0x000000C4,0x0);
 gmem_st16(self + 0x00000094,0x3);
 gmem_st16(self + 0x000000BC,0xfffb);
 gmem_st16(self + 0x000000B6,0xfffa);
 gmem_st16(self + 0x000000E0,0x19);
 gmem_st16(self + 0x0000010C,0xffb8);
 gmem_st16(self + 0x000000E4,0x0);
 gmem_st16(self + 0x000000C0,0x0);
 gmem_st16(self + 0x00000102,0x0);
 gmem_st16(self + 0x000000D8,0xffc4);
 gmem_st16(self + 0x000000FC,0x0);
 gmem_st16(self + 0x000000CC,0xffec);
 gmem_st16(self + 0x000000DE,0x0);
 gmem_st16(self + 0x000000AA,0xffc4);
 gmem_st16(self + 0x000000EC,0xffc4);
 gmem_st16(self + 0x000000CA,0xffec);
 gmem_st16(self + 0x0000010A,0xffc4);
 gmem_st16(self + 0x00000150,0x14);
 gmem_st16(self + 0x000000EA,0xffea);
 gmem_st16(self + 0x0000006C,0xffea);
 gmem_st16(self + 0x000000EE,0x0);
 gmem_st16(self + 0x000000D0,0x28);
 gmem_st16(self + 0x00000074,0xfffd);
 gmem_st16(self + 0x000000F4,0x0);
 gmem_st16(self + 0x0000006E,0xffdd);
 gmem_st16(self + 0x00000154,0x32);
 gmem_st16(self + 0x00000076,0xfff3);
 gmem_st16(self + 0x0000007A,0x82);
 gmem_st16(self + 0x000000E6,0xfffa);
 gmem_st16(self + 0x0000011C,0xfff5);
 gmem_st16(self + 0x000000F0,0x0);
 gmem_st16(self + 0x0000009E,0x0);
 gmem_st16(self + 0x000000CE,0x0);
 gmem_st16(self + 0x00000104,0x32);
 gmem_st16(self + 0x000000FA,0x0);
 gmem_st16(self + 0x00000078,0x8c);
 gmem_st16(self + 0x000000D6,0x0);
 gmem_st16(self + 0x000000C8,0xffec);
 gmem_st16(self + 0x0000011E,0x7);
 gmem_st16(self + 0x00000106,0x32);
 gmem_st16(self + 0x00000072,0xffd5);
 gmem_st16(self + 0x00000070,0xfffa);
 gmem_st16(self + 0x0000009C,0x0);
 gmem_st16(self + 0x00000100,0x0);
 gmem_st16(self + 0x000000DC,0x0);
 gmem_st16(self + 0x0000010E,0xffaf);
 gmem_st16(self + 0x000000F6,0x0);
 gmem_st16(self + 0x00000110,0x0);
 gmem_st16(self + 0x0000011A,0xfff4);
 gmem_st16(self + 0x000000E2,0x23);
 gmem_st16(self + 0x0000007C,0x8c);
 gmem_st16(self + 0x000000D4,0x0);
 gmem_st16(self + 0x00000116,0xff9e);
 gmem_st16(self + 0x000000F2,0x0);
 gmem_st16(self + 0x00000114,0xffe2);
 gmem_st16(self + 0x00000112,0x11);
 gmem_st16(self + 0x00000108,0xffc4);
 gmem_st16(self + 0x000000E8,0xffe2);
 gmem_st16(self + 0x00000118,0xffeb);
 return self;
}
VERIFY(0x025974DC,hio_025974DC);
static u32 hio_02597B00(u32 self){
 WWHD_FUNC(0x02597B00,u32,self);
 if(!self){self=gabi::call<u32>(0x0273AD10,0xc8);if(!self)return 0;}
 gmem_st32(self + 0x00000000,0x100510cc);
 gmem_st16(self + 0x0000002C,0x0);
 storef(self + 0x00000008,loadf(0x100512b8));
 gmem_st16(self + 0x00000024,0x0);
 gmem_st16(self + 0x00000036,0x0);
 storef(self + 0x00000014,loadf(0x100512b8));
 storef(self + 0x00000018,loadf(0x100512b8));
 gmem_st16(self + 0x00000026,0x0);
 gmem_st16(self + 0x00000034,0x0);
 gmem_st16(self + 0x0000002A,0x0);
 gmem_st16(self + 0x00000030,0x0);
 storef(self + 0x00000004,loadf(0x100512b8));
 storef(self + 0x0000001C,loadf(0x100512b8));
 gmem_st16(self + 0x0000002E,0x0);
 gmem_st16(self + 0x00000032,0x0);
 gmem_st16(self + 0x00000028,0x0);
 storef(self + 0x00000010,loadf(0x100512b8));
 storef(self + 0x0000000C,loadf(0x100512b8));
 storef(self + 0x00000020,loadf(0x100512b8));
 gabi::call(0x028F521C,self + 0x00000068,0x4);
 gabi::call(0x028F521C,self + 0x0000006C,0x4);
 gabi::call(0x028F521C,self + 0x00000070,0x4);
 gabi::call(0x028F521C,self + 0x00000074,0x4);
 gmem_st8(self + 0x000000BA,0x0);
 gmem_st16(self + 0x00000098,0x0);
 storef(self + 0x00000048,loadf(0x100512b8));
 gmem_st16(self + 0x0000007A,0x3);
 storef(self + 0x0000000C,loadf(0x100514ac));
 gmem_st8(self + 0x000000BC,0x1);
 storef(self + 0x00000008,loadf(0x1005133c));
 storef(self + 0x0000003C,loadf(0x100514b0));
 storef(self + 0x00000050,loadf(0x100514bc));
 storef(self + 0x00000004,loadf(0x10051488));
 storef(self + 0x00000040,loadf(0x100514b4));
 storef(self + 0x00000054,loadf(0x100514c0));
 gmem_st8(self + 0x00000072,0xff);
 storef(self + 0x0000004C,loadf(0x100514b8));
 gmem_st8(self + 0x0000006B,0xff);
 gmem_st8(self + 0x000000BE,0xa);
 storef(self + 0x00000038,loadf(0x100512b8));
 gmem_st16(self + 0x0000007C,0x3);
 gmem_st8(self + 0x0000006F,0xff);
 gmem_st8(self + 0x000000C0,0x0);
 gmem_st8(self + 0x000000BD,0x0);
 gmem_st16(self + 0x00000078,0x0);
 gmem_st8(self + 0x00000068,0x32);
 gmem_st8(self + 0x0000006E,0xff);
 gmem_st8(self + 0x0000006A,0x32);
 gmem_st8(self + 0x00000069,0x32);
 gmem_st8(self + 0x00000073,0xff);
 gmem_st8(self + 0x0000006C,0xff);
 gmem_st8(self + 0x00000070,0xff);
 gmem_st8(self + 0x0000006D,0xff);
 storef(self + 0x00000058,loadf(0x100514c4));
 storef(self + 0x00000044,loadf(0x100512b8));
 storef(self + 0x0000005C,loadf(0x100512b4));
 gmem_st8(self + 0x000000BF,0x4);
 storef(self + 0x00000014,loadf(0x100514cc));
 storef(self + 0x00000060,loadf(0x100514c8));
 gmem_st8(self + 0x00000077,0xff);
 storef(self + 0x00000064,loadf(0x100514c8));
 gmem_st8(self + 0x000000C3,0x1);
 gmem_st16(self + 0x00000086,0x0);
 gmem_st16(self + 0x00000080,0x5);
 gmem_st8(self + 0x00000074,0xc8);
 storef(self + 0x00000010,loadf(0x100514d4));
 gmem_st8(self + 0x00000076,0xc8);
 gmem_st8(self + 0x00000075,0xc8);
 gmem_st8(self + 0x000000C1,0x2);
 gmem_st8(self + 0x000000C5,0x0);
 storef(self + 0x00000018,loadf(0x100514d0));
 gmem_st16(self + 0x0000007E,0x8);
 gmem_st8(self + 0x000000C2,0x0);
 gmem_st16(self + 0x00000084,0x3);
 gmem_st16(self + 0x0000008E,0x7);
 gmem_st16(self + 0x000000B0,0x6);
 gmem_st8(self + 0x00000071,0xff);
 gmem_st16(self + 0x00000034,0x1a);
 gmem_st16(self + 0x00000024,0xffec);
 gmem_st8(self + 0x00000094,0xff);
 gmem_st8(self + 0x00000095,0xff);
 gmem_st16(self + 0x000000AE,0x0);
 gmem_st16(self + 0x00000088,0xff38);
 gmem_st16(self + 0x0000002C,0x0);
 gmem_st16(self + 0x00000082,0x0);
 gmem_st8(self + 0x000000C4,0x9);
 gmem_st16(self + 0x00000096,0x0);
 storef(self + 0x0000001C,loadf(0x100512c8));
 gmem_st8(self + 0x000000C6,0x0);
 storef(self + 0x00000090,loadf(0x100512b4));
 gmem_st16(self + 0x000000B2,0x46);
 gmem_st16(self + 0x00000036,0xfffd);
 gmem_st16(self + 0x0000008C,0x3);
 gmem_st16(self + 0x00000032,0x7);
 gmem_st16(self + 0x00000030,0x5);
 gmem_st8(self + 0x000000BB,0x0);
 gmem_st16(self + 0x000000A8,0x2);
 gmem_st16(self + 0x000000A0,0x6);
 gmem_st16(self + 0x000000B6,0x5);
 gmem_st8(self + 0x000000B4,0x96);
 gmem_st16(self + 0x00000026,0x10);
 gmem_st16(self + 0x000000AC,0x3c);
 gmem_st16(self + 0x000000B8,0x3);
 gmem_st16(self + 0x000000AA,0x1e);
 storef(self + 0x00000020,loadf(0x10051488));
 gmem_st16(self + 0x000000A4,0x2);
 gmem_st16(self + 0x0000002E,0x8);
 gmem_st16(self + 0x0000008A,0x15e);
 gmem_st16(self + 0x000000A6,0x2);
 storef(self + 0x0000009C,loadf(0x100512b4));
 gmem_st16(self + 0x00000028,0x18);
 gmem_st16(self + 0x0000002A,0x17);
 gmem_st16(self + 0x000000A2,0x2);
 return self;
}
VERIFY(0x02597B00,hio_02597B00);

static s32 deleteHud(u32 self){
 WWHD_FUNC(0x02593630,s32,self);
 static const u16 paneOffsets[]={0x2fc,0x104,0x13c,0x174,0x254,0x28c,0x2c4,0x36c,0x3a4,0x3dc,0x414,0x44c,0x484,0x4bc,0x4f4,0x52c,0x564,0x59c,0x60c,0x644,0xaa4,0x67c,0xadc,0x6b4,0xb14,0x6ec,0xb4c,0x724,0xb84,0x75c,0xbbc,0x794,0xbf4,0x7cc,0xc2c,0x804,0xc64,0x83c,0xc9c,0x874,0xcd4,0x8ac,0xd0c,0x8e4,0xd44,0x91c,0xd7c,0x954,0xdb4,0x98c,0xdec,0x9c4,0xe24,0x9fc,0xe5c,0xa34,0xe94,0xa6c,0xecc,0xf3c,0xf74,0xfac,0xfe4,0x101c,0x1054,0x108c,0x10c4,0x10fc,0x11a4,0x11dc,0x1214,0x12f4,0x139c,0x13d4,0x140c,0x1444,0x147c,0x186c,0x14b4,0x186c,0x14ec,0x186c,0x1524,0x155c,0x1594,0x171c,0x15cc,0x171c,0x1604,0x171c,0x163c,0x171c,0x1674,0x16ac,0x16e4,0x17fc,0x1834,0x1914,0x194c,0x1984,0x19bc,0x1ad4,0x1c24,0x1b0c,0x1c5c,0x1c94,0x1d04,0x282c,0x2864,0x289c,0x28d4,0x1d3c,0x1e1c,0x1dac,0x1e54,0x1de4,0x274c,0x2784,0x27bc,0x27f4,0x290c,0x1fdc,0x1f34,0x2084,0x212c,0x2324,0x23cc,0x2474,0x2014,0x1f6c,0x20bc,0x2164,0x235c,0x2404,0x24ac,0x204c,0x1fa4,0x20f4,0x219c,0x2394,0x243c,0x24e4,0x251c,0x2554,0x25c4,0x2634,0x258c,0x25fc,0x266c,0x1ccc,0x26a4,0x26dc,0x19f4,0x1b44,0x1a2c,0x1b7c,0x1a64,0x1bb4,0x1a9c,0x1bec,0x2a24,0x2a5c};
 for(u16 off:paneOffsets)gabi::call(0x025DB678,self+off);
 if(gmem_ld32(0x101EA0F8)){for(int i=0;i<9;i++)gabi::call(0x025DB678,self+0x2a94+0x38*i);gabi::call(0x025DB678,self+0x2c8c);gabi::call(0x025DB678,self+0x2cc4);}
 if(gmem_ld32(0x101EA0F4))for(u32 off:{0x2cfcu,0x2da4u,0x2d34u,0x2ddcu})gabi::call(0x025DB678,self+off);
 for(int i=0;i<4;i++)gabi::call(0x025DB678,self+0x2e14+0x38*i);
 if(!gabi::call<s32>(0x025DBE38)){
 auto unlink=[](u32 obj,u32 offset){u32 list=gabi::call<u32>(0x025200D4)+0x5d30;gabi::call(0x0252CDC0,list,list+offset,list+offset+4,obj);};
 unlink(0x1047A994,0x1dc);for(int i=0;i<3;i++)unlink(0x1047AAA0+0xc4*i,0x1dc);
 if(gmem_ld32(0x101EA0F4)&&gmem_ld32(0x101EA0F8))unlink(0x1047A9DC,0x1dc);
 unlink(0x1047A998,0xd4);}
 return 1;
}
VERIFY(0x02593630,deleteHud);

static void initializeStatics(){
 WWHD_FUNC(0x02598444,void);
 for(int i=3;i>=0;i--)gmem_st32(0x1047A9CC+4*i,0);
 gabi::call(0x028F026C,0x101EA0A0);
 storef(0x1047A99C,loadf(0x100514F0));storef(0x1047A9A0,loadf(0x100514F4));
 gabi::call(0x028ED6F8,0x1047A9C4);gabi::call(0x028F026C,0x101EA0AC);
 gabi::call(0x028EAB2C,0x1047A9C5);gabi::call(0x028F026C,0x101EA0B8);
 storef(0x1047A9A8,loadf(0x100514F8));storef(0x1047A9AC,loadf(0x100514FC));storef(0x1047A9B0,loadf(0x100514FC));
 u32 b=0x1047B03C;gmem_st32(b+0x38,0);
 for(u32 o:{0x62u,0x63u,0x3eu,0x3fu,0x40u,0x41u,0x42u,0x43u,0x44u,0x5au,0x5bu,0x5cu,0x5du,0x5eu,0x5fu})gmem_st8(b+o,0);
 for(u32 o=0x18;o<=0x34;o+=4)storef(b+o,loadf(0x100512B8));
 gmem_st16(b+0x3c,0);storef(0x1047A9A4,loadf(0x100514F8));gmem_st8(b+0x60,0);gmem_st8(b+0x61,0);
 for(int i=0;i<7;i++){gmem_st8(b+0x53+i,0);gmem_st8(b+0x45+i,0);gmem_st8(b+0x4c+i,0);}
 gabi::call(0x02598254,0x1047ACEC);gabi::call(0x025974DC,0x1047AD14);gabi::call(0x02597B00,0x1047AE6C);
 gabi::call(0x02597E34,0x1047AF34);gabi::call(0x02598058,0x1047AFBC);
 gabi::call(0x0252CCBC,0x1047A994);gmem_st32(0x1047A994,0x10051518);gabi::call(0x028F026C,0x101EA0C4);
 gabi::call(0x0252CCBC,0x1047A998);gmem_st32(0x1047A998,0x10051530);gabi::call(0x028F026C,0x101EA0D0);
 gabi::call(0x0252CCBC,0x1047A9DC);gmem_st32(0x1047A9DC,0x1004CBD8);gabi::call(0x028F026C,0x101EA0DC);
 gabi::call(0x028EFFD0,0x1047AAA0,3,0xc4,0x0259CF60);gabi::call(0x028F026C,0x101EA0E8);
}
VERIFY(0x02598444,initializeStatics);

static s32 removeHud(u32 self){
 WWHD_FUNC(0x02595850,s32,self);
 u32 oldHeap=gabi::call<u32>(0x025E3570,gmem_ld32(self+0x100));
 u8 setting=gmem_ld8(0x1047AD0C);gmem_st8(gabi::call<u32>(0x025200D4)+0x5c23,setting==0);
 if(gmem_ld8(0x101EA076)){gabi::call(0x025E1988,0x83a);gmem_st8(0x101EA076,0);gmem_st8(0x101EA075,0);}
 if(gmem_ld32(0x101EA040)!=0xffffffff){if(gabi::call<u32>(0x025DE50C,gmem_ld32(0x101EA040)))gabi::call(0x025DF944,gabi::call<u32>(0x025DE50C,gmem_ld32(0x101EA040)));gmem_st8(0x101EA074,0);gmem_st32(0x101EA040,-1);}
 for(u32 off:{0x2f24u,0x2f28u,0x2f2cu,0x2f30u,0x2f34u}){
  u32 emitter=gmem_ld32(self+off);if(emitter){u32 flags=gmem_ld32(emitter+0x254);gmem_st32(emitter+0x5c,-1);gmem_st32(emitter+0x254,flags|1);emitter=gmem_ld32(self+off);gmem_st32(emitter+0x254,gmem_ld32(emitter+0x254)&~0x40u);}}
 auto freeTexture=[&](u32 off){u32 heap=gmem_ld32(self+0x100);u32 target=gmem_ld32(gmem_ld32(heap+0xc)+0x3c);gabi::call_ptr(target,heap,gmem_ld32(self+off));};
 for(int i=0;i<3;i++){freeTexture(0x2f40+4*i);freeTexture(0x2f4c+4*i);}
 freeTexture(0x2f38);freeTexture(0x2f64);freeTexture(0x2f3c);
 auto destroy=[](u32 global){u32 obj=gmem_ld32(global);if(obj)gabi::call_ptr(gmem_ld32(gmem_ld32(obj+0xc8)+0xc),obj,3);};
 destroy(0x1047B0A0);destroy(0x1047B0A4);destroy(0x1047B0A8);
 if(gmem_ld32(0x101EA0F4)&&gmem_ld32(0x101EA0F8)){
  u32 heap=gmem_ld32(gabi::call<u32>(0x025200D4)+0x5aa0);gabi::call_ptr(gmem_ld32(gmem_ld32(heap+0xc)+0x44),heap);
  destroy(0x101EA0F4);destroy(0x101EA0F8);gmem_st32(0x101EA0F8,0);gmem_st32(0x101EA0F4,0);}
 destroy(0x101EA0FC);u32 last=gmem_ld32(0x101EA100);gmem_st32(0x101EA0FC,0);
 if(last)gabi::call_ptr(gmem_ld32(gmem_ld32(last+0xc8)+0xc),last,3);
 gmem_st32(0x101EA100,0);gabi::call(0x025E3570,oldHeap);gabi::call(0x025DB8F4,gmem_ld32(self+0x100));gabi::call(0x025F0A18,(s32)(s8)gmem_ld8(0x1047ACEC));
 u32 b=0x1047B03C;gmem_st32(b+0x38,0);for(u32 o:{0x62u,0x63u,0x3eu,0x3fu,0x40u,0x41u,0x42u,0x43u,0x44u,0x5au,0x5bu,0x5cu,0x5du,0x5eu,0x5fu})gmem_st8(b+o,0);for(u32 o=0x18;o<=0x34;o+=4)storef(b+o,loadf(0x100512B8));
 gmem_st16(b+0x3c,0);gmem_st8(b+0x60,0);gmem_st8(b+0x61,0);
 for(int i=0;i<7;i++){gmem_st8(b+0x53+i,0);gmem_st8(b+0x45+i,0);gmem_st8(b+0x4c+i,0);}return 1;
}
VERIFY(0x02595850,removeHud);
// Updates HUD flags, transition timers and per-frame pane behavior.
static void updateHudStatus(u32 self){
 WWHD_FUNC(0x02593B10,void,self);
 gabi::Local<u8[0x1c]> scratch;
 u32 t0=0,t3=self,t4=0,t5=0,t6=0,t7=0,t8=0,t9=0,t10=0,t11=0,t12=0,t25=0,t26=0,t27=0,previousHeap=0,zero=0,saveSegmentBase=0,hud=0;
 double f0=0,f1=0,f6=0,f7=0,f8=0,f9=0,f10=0,f11=0,f12=0,f13=0,f29=0,f30=0,f31=0;
 u32 target=0;u8 carry=0;bool conditions[32]={};
auto compares=[&](int n,s32 a,s32 b){conditions[4*n]=a<b;conditions[4*n+1]=a>b;conditions[4*n+2]=a==b;};
 auto compareu=[&](int n,u32 a,u32 b){conditions[4*n]=a<b;conditions[4*n+1]=a>b;conditions[4*n+2]=a==b;};
 auto comparef=[&](int n,double a,double b){conditions[4*n]=a<b;conditions[4*n+1]=a>b;conditions[4*n+2]=a==b;conditions[4*n+3]=std::isnan(a)||std::isnan(b);};
hud = t3;
t3 = gmem_ld32(hud + 0x00000100u);
t3=gabi::call<u32>(0x025E3570 ,t3);
zero = 0x00000000u;
gmem_st8(hud + 0x00003028u, zero);
previousHeap = t3;
gmem_st8(hud + 0x00003038u, zero);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st32(hud + 0x00003008u, zero);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t7 = gmem_ld8(t3 + 0x00005C22u);
compares(0,(int32_t)t7,0);
saveSegmentBase = 0x10200000u;
if (conditions[2]) { goto L_02593B8C; }
t3=gabi::call<u32>(0x024F8044 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld32(t3 + 0x00000510u);
t4 = rotl32(t0, 7) & 0x00000001u; compares(0,(s32)t4,0);
if (conditions[2]) { goto L_02593BB4; }
L_02593B8C: ;
t6 = gmem_ld32(hud + 0x00003008u);
t6 = t6 | 0x4000u;
t27 = 0x101F0000u;
gmem_st32(hud + 0x00003008u, t6);
t27 = gmem_ld8(t27 + 0xFFFFA069u);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
compares(0,(int32_t)t27,0);
t12 = t3 + 0x000012A0u;
if (!conditions[2]) { goto L_02593E58; }
goto L_02593EE0;
L_02593BB4: ;
t8 = 0x101F0000u;
t8 = gmem_ld8(t8 + 0xFFFFA06Cu);
compares(0,(int32_t)t8,0);
if (conditions[2]) { goto L_02593BD4; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BB2u);
compares(0,(int32_t)t0,0);
if (conditions[2]) { goto L_02593C34; }
L_02593BD4: ;
t5 = gmem_ld32(saveSegmentBase + 0xFFFF84DCu);
t4 = 0x00000408u;
t3 = t5 + 0x00001178u;
t3=gabi::call<u32>(0x025B8B94 ,t3,t4);
compares(0,(int32_t)t3,0);
if (conditions[2]) { goto L_02593BFC; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t9 = gmem_ld8(t3 + 0x00005BB2u);
compares(0,(int32_t)t9,0);
if (conditions[2]) { goto L_02593C34; }
L_02593BFC: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t4 = 0x10050000u;
t3 = t3 + 0x000052C4u;
t4 = t4 + 0x00001124u;
t3=gabi::call<u32>(0x025445B8 ,t3,t4);
compares(0,(int32_t)t3,0);
if (!conditions[2]) { goto L_02593C34; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t4 = 0x10050000u;
t3 = t3 + 0x000052C4u;
t4 = t4 + 0x00001194u;
t3=gabi::call<u32>(0x025445B8 ,t3,t4);
compares(0,(int32_t)t3,0);
if (conditions[2]) { goto L_02593C6C; }
L_02593C34: ;
t0 = gmem_ld32(hud + 0x00003008u);
t0 = t0 | 0x00080000u;
gmem_st32(hud + 0x00003008u, t0);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BB6u, zero);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t27 = 0x101F0000u;
gmem_st8(t3 + 0x00005BB7u, zero);
t27 = gmem_ld8(t27 + 0xFFFFA069u);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
compares(0,(int32_t)t27,0);
t12 = t3 + 0x000012A0u;
if (!conditions[2]) { goto L_02593E58; }
goto L_02593EE0;
L_02593C6C: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t27 = gmem_ld8(t3 + 0x00005BD0u);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
compares(0,(int32_t)t27,2);
t11 = t3 + 0x000012A0u;
if (!conditions[2]) { goto L_02593CC0; }
t6 = 0x00000017u;
gmem_st8(t11 + 0x0000491Au, t6);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t9 = 0x00000007u;
gmem_st8(t3 + 0x00005BB9u, t9);
t10 = gmem_ld32(hud + 0x00003008u);
t10 = t10 | 0x00400000u;
t27 = 0x101F0000u;
gmem_st32(hud + 0x00003008u, t10);
t27 = gmem_ld8(t27 + 0xFFFFA069u);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
compares(0,(int32_t)t27,0);
t12 = t3 + 0x000012A0u;
if (!conditions[2]) { goto L_02593E58; }
goto L_02593EE0;
L_02593CC0: ;
t11 = gmem_ld8(t11 + 0x00004A4Au);
compares(0,(int32_t)t11,8);
if (!conditions[2]) { goto L_02593D1C; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld32(t3 + 0x00005CD8u);
t12 = t0 & 0x00000010u; compares(0,(s32)t12,0);
if (!conditions[2]) { goto L_02593D1C; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t4 = 0x0000003Du;
gmem_st8(t3 + 0x00005BBAu, t4);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t5 = 0x0000003Eu;
gmem_st8(t3 + 0x00005BB9u, t5);
t6 = gmem_ld32(hud + 0x00003008u);
t6 = t6 | 0x01000000u;
t27 = 0x101F0000u;
gmem_st32(hud + 0x00003008u, t6);
t27 = gmem_ld8(t27 + 0xFFFFA069u);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
compares(0,(int32_t)t27,0);
t12 = t3 + 0x000012A0u;
if (!conditions[2]) { goto L_02593E58; }
goto L_02593EE0;
L_02593D1C: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t8 = gmem_ld32(t3 + 0x00005CDCu);
t7 = t8 & 0x00000001u; compares(0,(s32)t7,0);
if (conditions[2]) { goto L_02593D64; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t9 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t9,7);
if (!conditions[2]) { goto L_02593D64; }
t10 = gmem_ld32(hud + 0x00003008u);
t10 = t10 | 0x00200000u;
t27 = 0x101F0000u;
gmem_st32(hud + 0x00003008u, t10);
t27 = gmem_ld8(t27 + 0xFFFFA069u);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
compares(0,(int32_t)t27,0);
t12 = t3 + 0x000012A0u;
if (!conditions[2]) { goto L_02593E58; }
goto L_02593EE0;
L_02593D64: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = gmem_ld8(t3 + 0x00005292u);
{ uint64_t t = (uint64_t)t12 + 0xFFFFFFFFu; t0 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
{ uint64_t t = (uint64_t)(uint32_t)~t0 + t12 + carry; t11 = (uint32_t)t; carry = (uint8_t)(t >> 32); } compares(0,(s32)t11,0);
if (conditions[2]) { goto L_02593E40; }
t7 = 0x101F0000u;
t4 = gmem_ld8(t7 + 0xFFFFA067u);
compares(0,(int32_t)t4,4);
if (conditions[2]) { goto L_02593E40; }
t5 = 0x101D0000u;
t5 = gmem_ld32(t5 + 0x00006010u);
compares(0,(int32_t)t5,1);
if (conditions[2]) { goto L_02593E34; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t5 = gmem_ld8(t3 + 0x00005BB2u);
compares(0,(int32_t)t5,0);
if (conditions[2]) { goto L_02593DE0; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t6 = gmem_ld8(t3 + 0x00005BD1u);
compares(0,(int32_t)t6,0);
if (!conditions[2]) { goto L_02593DE0; }
t7 = gmem_ld32(hud + 0x00003008u);
t7 = t7 | 0x0100u;
t27 = 0x101F0000u;
gmem_st32(hud + 0x00003008u, t7);
t27 = gmem_ld8(t27 + 0xFFFFA069u);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
compares(0,(int32_t)t27,0);
t12 = t3 + 0x000012A0u;
if (!conditions[2]) { goto L_02593E58; }
goto L_02593EE0;
L_02593DE0: ;
t8 = 0x101D0000u;
t8 = gmem_ld32(t8 + 0x00006010u);
compares(0,(int32_t)t8,1);
if (conditions[2]) { goto L_02593E34; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t8 = gmem_ld8(t3 + 0x00005BD1u);
compares(0,(int32_t)t8,0);
if (conditions[2]) { goto L_02593E34; }
t9 = gmem_ld32(hud + 0x00003008u);
t9 = t9 | 0x00200000u;
gmem_st32(hud + 0x00003008u, t9);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t10 = 0x0000003Eu;
t27 = 0x101F0000u;
gmem_st8(t3 + 0x00005BB9u, t10);
t27 = gmem_ld8(t27 + 0xFFFFA069u);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
compares(0,(int32_t)t27,0);
t12 = t3 + 0x000012A0u;
if (!conditions[2]) { goto L_02593E58; }
goto L_02593EE0;
L_02593E34: ;
t11 = gmem_ld32(hud + 0x00003008u);
t11 = t11 | 0x0040u;
gmem_st32(hud + 0x00003008u, t11);
L_02593E40: ;
t27 = 0x101F0000u;
t27 = gmem_ld8(t27 + 0xFFFFA069u);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
compares(0,(int32_t)t27,0);
t12 = t3 + 0x000012A0u;
if (conditions[2]) { goto L_02593EE0; }
L_02593E58: ;
t0 = gmem_ld32(t12 + 0x00004A3Cu);
t12 = t0 & 0x00000008u; compares(0,(s32)t12,0);
if (!conditions[2]) { goto L_02593E84; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t4 = gmem_ld8(t3 + 0x00005BE8u);
compares(0,(int32_t)t4,2);
if (conditions[2]) { goto L_02593E84; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t5 = gmem_ld8(t3 + 0x00005BE8u);
compares(0,(int32_t)t5,3);
if (!conditions[2]) { goto L_02594230; }
L_02593E84: ;
t6 = gmem_ld32(hud + 0x00003008u);
t6 = t6 | 0x00020000u;
gmem_st32(hud + 0x00003008u, t6);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t7 = gmem_ld8(t3 + 0x00005BE8u);
compares(0,(int32_t)t7,2);
if (!conditions[2]) { goto L_02594230; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t8 = 0x00000017u;
gmem_st8(t3 + 0x00005BBAu, t8);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t9 = 0x00000007u;
gmem_st8(t3 + 0x00005BB9u, t9);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
{ uint32_t ea = t3 + 0x00005150u; t4 = gmem_ld32(ea); t3 = ea; }
t5 = gmem_ld32(t4 + 0x0000015Cu);
target = t5;
t3=gabi::call_ptr<u32>(target,t3);
t6 = gmem_ld32(t3 + 0x0000000Cu);
t7 = rotl32(t6, 16) & 0x00000007u;
compares(0,(int32_t)t7,1);
if (conditions[2]) { goto L_02594254; }
goto L_0259427C;
L_02593EE0: ;
t27 = gmem_ld32(t12 + 0x00004898u);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t10 = gmem_ld32(t3 + 0x00005B2Cu);
compareu(0,t27,t10);
if (conditions[2]) { goto L_02593F04; }
t5 = 0x101D0000u;
t11 = gmem_ld8(t5 + 0x00005F45u);
compares(0,(int32_t)t11,0);
if (conditions[2]) { goto L_02593F3C; }
L_02593F04: ;
t12 = gmem_ld32(hud + 0x00003008u);
t12 = t12 | 0x00040000u;
gmem_st32(hud + 0x00003008u, t12);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t0,53);
if (!conditions[2]) { goto L_02593F78; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BB6u, zero);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = gmem_ld32(t3 + 0x00005CD8u);
t11 = t12 & 0x00000010u; compares(0,(s32)t11,0);
if (!conditions[2]) { goto L_02593F88; }
goto L_02593FBC;
L_02593F3C: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t5 = gmem_ld32(t3 + 0x00005CD8u);
t4 = t5 & 0x00002000u; compares(0,(s32)t4,0);
if (!conditions[2]) { goto L_02593F6C; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t7 = gmem_ld32(t3 + 0x00005B00u);
t6 = t7 & 0x00000100u; compares(0,(s32)t6,0);
if (conditions[2]) { goto L_02593F78; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t9 = gmem_ld32(t3 + 0x00005CD8u);
t8 = t9 & 0x08000000u; compares(0,(s32)t8,0);
if (conditions[2]) { goto L_02593F78; }
L_02593F6C: ;
t10 = gmem_ld32(hud + 0x00003008u);
t10 = t10 | 0x00100000u;
gmem_st32(hud + 0x00003008u, t10);
L_02593F78: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = gmem_ld32(t3 + 0x00005CD8u);
t11 = t12 & 0x00000010u; compares(0,(s32)t11,0);
if (conditions[2]) { goto L_02593FBC; }
L_02593F88: ;
t0 = gmem_ld32(hud + 0x00003008u);
t0 = t0 | 0x0100u;
gmem_st32(hud + 0x00003008u, t0);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
{ uint32_t ea = t3 + 0x00005150u; t4 = gmem_ld32(ea); t3 = ea; }
t5 = gmem_ld32(t4 + 0x0000015Cu);
target = t5;
t3=gabi::call_ptr<u32>(target,t3);
t6 = gmem_ld32(t3 + 0x0000000Cu);
t7 = rotl32(t6, 16) & 0x00000007u;
compares(0,(int32_t)t7,1);
if (conditions[2]) { goto L_02594254; }
goto L_0259427C;
L_02593FBC: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t5 = gmem_ld32(t3 + 0x00005CD8u);
t4 = t5 & 0x00200000u; compares(0,(s32)t4,0);
if (conditions[2]) { goto L_02594000; }
t6 = gmem_ld32(hud + 0x00003008u);
t6 = t6 | 0x0080u;
gmem_st32(hud + 0x00003008u, t6);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
{ uint32_t ea = t3 + 0x00005150u; t4 = gmem_ld32(ea); t3 = ea; }
t5 = gmem_ld32(t4 + 0x0000015Cu);
target = t5;
t3=gabi::call_ptr<u32>(target,t3);
t6 = gmem_ld32(t3 + 0x0000000Cu);
t7 = rotl32(t6, 16) & 0x00000007u;
compares(0,(int32_t)t7,1);
if (conditions[2]) { goto L_02594254; }
goto L_0259427C;
L_02594000: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t8 = gmem_ld32(t3 + 0x00005CDCu);
t7 = t8 & 0x00000008u; compares(0,(s32)t7,0);
if (!conditions[2]) { goto L_02594030; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t9 = gmem_ld8(t3 + 0x00005BE8u);
compares(0,(int32_t)t9,2);
if (conditions[2]) { goto L_02594030; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t10 = gmem_ld8(t3 + 0x00005BE8u);
compares(0,(int32_t)t10,3);
if (!conditions[2]) { goto L_02594064; }
L_02594030: ;
t11 = gmem_ld32(hud + 0x00003008u);
t11 = t11 | 0x00020000u;
gmem_st32(hud + 0x00003008u, t11);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
{ uint32_t ea = t3 + 0x00005150u; t4 = gmem_ld32(ea); t3 = ea; }
t5 = gmem_ld32(t4 + 0x0000015Cu);
target = t5;
t3=gabi::call_ptr<u32>(target,t3);
t6 = gmem_ld32(t3 + 0x0000000Cu);
t7 = rotl32(t6, 16) & 0x00000007u;
compares(0,(int32_t)t7,1);
if (conditions[2]) { goto L_02594254; }
goto L_0259427C;
L_02594064: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld32(t3 + 0x00005CD8u);
t12 = t0 & 0x00010000u; compares(0,(s32)t12,0);
if (conditions[2]) { goto L_025940A8; }
t0 = gmem_ld32(hud + 0x00003008u);
t0 = t0 | 0x0400u;
gmem_st32(hud + 0x00003008u, t0);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
{ uint32_t ea = t3 + 0x00005150u; t4 = gmem_ld32(ea); t3 = ea; }
t5 = gmem_ld32(t4 + 0x0000015Cu);
target = t5;
t3=gabi::call_ptr<u32>(target,t3);
t6 = gmem_ld32(t3 + 0x0000000Cu);
t7 = rotl32(t6, 16) & 0x00000007u;
compares(0,(int32_t)t7,1);
if (conditions[2]) { goto L_02594254; }
goto L_0259427C;
L_025940A8: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t6 = gmem_ld32(t3 + 0x00005CD8u);
t0 = t6 & 0x08000000u; compares(0,(s32)t0,0);
if (conditions[2]) { goto L_025940EC; }
t7 = gmem_ld32(hud + 0x00003008u);
t7 = t7 | 0x0200u;
gmem_st32(hud + 0x00003008u, t7);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
{ uint32_t ea = t3 + 0x00005150u; t4 = gmem_ld32(ea); t3 = ea; }
t5 = gmem_ld32(t4 + 0x0000015Cu);
target = t5;
t3=gabi::call_ptr<u32>(target,t3);
t6 = gmem_ld32(t3 + 0x0000000Cu);
t7 = rotl32(t6, 16) & 0x00000007u;
compares(0,(int32_t)t7,1);
if (conditions[2]) { goto L_02594254; }
goto L_0259427C;
L_025940EC: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t8 = gmem_ld32(t3 + 0x00005CD8u);
t27 = t8 & 0x00800000u;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
compares(0,(int32_t)t27,0);
t5 = t3 + 0x000012A0u;
if (conditions[2]) { goto L_02594188; }
t3 = gmem_ld32(t5 + 0x0000488Cu);
t10 = gmem_ld32(t3 + 0x000000B4u);
t11 = gmem_ld32(t10 + 0x000000D4u);
target = t11;
t3=gabi::call_ptr<u32>(target,t3);
compares(0,(int32_t)t3,0);
t12 = gmem_ld32(hud + 0x00003008u);
if (conditions[2]) { goto L_02594158; }
t12 = t12 | 0x0800u;
gmem_st32(hud + 0x00003008u, t12);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
{ uint32_t ea = t3 + 0x00005150u; t4 = gmem_ld32(ea); t3 = ea; }
t5 = gmem_ld32(t4 + 0x0000015Cu);
target = t5;
t3=gabi::call_ptr<u32>(target,t3);
t6 = gmem_ld32(t3 + 0x0000000Cu);
t7 = rotl32(t6, 16) & 0x00000007u;
compares(0,(int32_t)t7,1);
if (conditions[2]) { goto L_02594254; }
goto L_0259427C;
L_02594158: ;
t0 = t12 | 0x1000u;
gmem_st32(hud + 0x00003008u, t0);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
{ uint32_t ea = t3 + 0x00005150u; t4 = gmem_ld32(ea); t3 = ea; }
t5 = gmem_ld32(t4 + 0x0000015Cu);
target = t5;
t3=gabi::call_ptr<u32>(target,t3);
t6 = gmem_ld32(t3 + 0x0000000Cu);
t7 = rotl32(t6, 16) & 0x00000007u;
compares(0,(int32_t)t7,1);
if (conditions[2]) { goto L_02594254; }
goto L_0259427C;
L_02594188: ;
t5 = gmem_ld32(t5 + 0x00004A38u);
t4 = t5 & 0x00100000u; compares(0,(s32)t4,0);
if (conditions[2]) { goto L_025941C8; }
t6 = gmem_ld32(hud + 0x00003008u);
t6 = t6 | 0x2000u;
gmem_st32(hud + 0x00003008u, t6);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
{ uint32_t ea = t3 + 0x00005150u; t4 = gmem_ld32(ea); t3 = ea; }
t5 = gmem_ld32(t4 + 0x0000015Cu);
target = t5;
t3=gabi::call_ptr<u32>(target,t3);
t6 = gmem_ld32(t3 + 0x0000000Cu);
t7 = rotl32(t6, 16) & 0x00000007u;
compares(0,(int32_t)t7,1);
if (conditions[2]) { goto L_02594254; }
goto L_0259427C;
L_025941C8: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t8 = 0x02000000u;
t9 = gmem_ld32(t3 + 0x00005CD8u);
t8 = t8 + 0x00000100u;
t7 = t8 & t9; compares(0,(s32)t7,0);
if (conditions[2]) { goto L_02594214; }
t10 = gmem_ld32(hud + 0x00003008u);
t10 = t10 | 0x8000u;
gmem_st32(hud + 0x00003008u, t10);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
{ uint32_t ea = t3 + 0x00005150u; t4 = gmem_ld32(ea); t3 = ea; }
t5 = gmem_ld32(t4 + 0x0000015Cu);
target = t5;
t3=gabi::call_ptr<u32>(target,t3);
t6 = gmem_ld32(t3 + 0x0000000Cu);
t7 = rotl32(t6, 16) & 0x00000007u;
compares(0,(int32_t)t7,1);
if (conditions[2]) { goto L_02594254; }
goto L_0259427C;
L_02594214: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = gmem_ld32(t3 + 0x00005CD8u);
t11 = t12 & 0x04000000u; compares(0,(s32)t11,0);
if (conditions[2]) { goto L_02594230; }
t0 = gmem_ld32(hud + 0x00003008u);
t0 = t0 | 0x00010000u;
gmem_st32(hud + 0x00003008u, t0);
L_02594230: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
{ uint32_t ea = t3 + 0x00005150u; t4 = gmem_ld32(ea); t3 = ea; }
t5 = gmem_ld32(t4 + 0x0000015Cu);
target = t5;
t3=gabi::call_ptr<u32>(target,t3);
t6 = gmem_ld32(t3 + 0x0000000Cu);
t7 = rotl32(t6, 16) & 0x00000007u;
compares(0,(int32_t)t7,1);
if (!conditions[2]) { goto L_0259427C; }
L_02594254: ;
t0 = gmem_ld32(hud + 0x00003008u);
t0 = t0 | 0x0004u;
gmem_st32(hud + 0x00003008u, t0);
t12 = gmem_ld32(saveSegmentBase + 0xFFFF84DCu);
t3 = t12 + 0x000012C0u;
t3=gabi::call<u32>(0x027200D0 ,t3);
t3=gabi::call<u32>(0x0271FC8C ,t3);
compares(0,(int32_t)t3,0);
if (!conditions[2]) { goto L_025942C8; }
goto L_025942D4;
L_0259427C: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
{ uint32_t ea = t3 + 0x00005150u; t8 = gmem_ld32(ea); t3 = ea; }
t9 = gmem_ld32(t8 + 0x0000015Cu);
target = t9;
t3=gabi::call_ptr<u32>(target,t3);
t10 = gmem_ld32(t3 + 0x0000000Cu);
t8 = rotl32(t10, 16) & 0x00000007u;
compares(0,(int32_t)t8,2);
if (conditions[2]) { goto L_025942A4; }
t8 = 0x00000001u;
L_025942A4: ;
t0 = gmem_ld32(hud + 0x00003008u);
t0 = t0 | t8;
gmem_st32(hud + 0x00003008u, t0);
t12 = gmem_ld32(saveSegmentBase + 0xFFFF84DCu);
t3 = t12 + 0x000012C0u;
t3=gabi::call<u32>(0x027200D0 ,t3);
t3=gabi::call<u32>(0x0271FC8C ,t3);
compares(0,(int32_t)t3,0);
if (conditions[2]) { goto L_025942D4; }
L_025942C8: ;
t4 = gmem_ld32(hud + 0x00003008u);
t4 = t4 | 0x02000000u;
gmem_st32(hud + 0x00003008u, t4);
L_025942D4: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t5 = gmem_ld8(t3 + 0x00005C22u);
t4 = 0x10050000u;
compares(0,(int32_t)t5,0);
{ uint32_t ea = t4 + 0x000012B8u; { double v = gabi::load<float>(ea); f31 = v; } }
if (!conditions[2]) { goto L_0259434C; }
{ uint32_t ea = hud + 0x00002F70u; { double v = gabi::load<float>(ea); f0 = v; } }
comparef(0,f0,f31);
if (!conditions[1]) { goto L_02594330; }
t12 = 0x10480000u;
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x00002FDCu);
t3 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t12 + 0xFFFFAD18u);
t5 = 0x00000000u;
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
t6 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x00002FDCu);
{ uint32_t ea = hud + 0x00002F70u; gabi::store<float>(ea, f1); }
t6 = t6 + 0xFFFFFFFFu;
gmem_st16(hud + 0x00002FDCu, t6);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = gmem_ld8(t3 + 0x00005CEAu);
compares(0,(int32_t)t12,6);
if (conditions[2]) { goto L_02594468; }
goto L_025944A8;
L_02594330: ;
{ uint32_t ea = hud + 0x00002F70u; gabi::store<float>(ea, f31); }
gmem_st16(hud + 0x00002FDCu, zero);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = gmem_ld8(t3 + 0x00005CEAu);
compares(0,(int32_t)t12,6);
if (conditions[2]) { goto L_02594468; }
goto L_025944A8;
L_0259434C: ;
t10 = 0x101F0000u;
t7 = gmem_ld8(t10 + 0xFFFFA069u);
t11 = 0x10050000u;
compares(0,(int32_t)t7,0);
{ uint32_t ea = t11 + 0x000012B4u; { double v = gabi::load<float>(ea); f29 = v; } }
if (conditions[2]) { goto L_02594404; }
t4 = 0x101F0000u;
t4 = gmem_ld32(t4 + 0xFFFFA02Cu);
t6 = 0x10050000u;
compares(0,(int32_t)t4,5);
{ uint32_t ea = t6 + 0x000012C8u; { double v = gabi::load<float>(ea); f30 = v; } }
if (conditions[1]) { goto L_025943C4; }
t3 = 0x00000005u;
t5 = 0x00000000u;
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
t12 = 0x101F0000u;
t0 = gmem_ld8(t12 + 0xFFFFA067u);
compares(0,(int32_t)t0,3);
{ uint32_t ea = hud + 0x00002F70u; { double v = gabi::load<float>(ea); f6 = v; } }
if (conditions[2]) { goto L_025943A4; }
{ double v = to_single(f29 - f1); f13 = v; }
{ double v = to_single(f13 * round25(f30) + f30); f1 = v; }
L_025943A4: ;
comparef(0,f6,f1);
if (!conditions[1]) { goto L_02594458; }
{ uint32_t ea = hud + 0x00002F70u; gabi::store<float>(ea, f1); }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = gmem_ld8(t3 + 0x00005CEAu);
compares(0,(int32_t)t12,6);
if (conditions[2]) { goto L_02594468; }
goto L_025944A8;
L_025943C4: ;
t4 = 0x101F0000u;
t9 = gmem_ld8(t4 + 0xFFFFA067u);
compares(0,(int32_t)t9,3);
if (!conditions[2]) { goto L_025943EC; }
{ uint32_t ea = hud + 0x00002F70u; gabi::store<float>(ea, f31); }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = gmem_ld8(t3 + 0x00005CEAu);
compares(0,(int32_t)t12,6);
if (conditions[2]) { goto L_02594468; }
goto L_025944A8;
L_025943EC: ;
{ uint32_t ea = hud + 0x00002F70u; gabi::store<float>(ea, f30); }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = gmem_ld8(t3 + 0x00005CEAu);
compares(0,(int32_t)t12,6);
if (conditions[2]) { goto L_02594468; }
goto L_025944A8;
L_02594404: ;
{ uint32_t ea = hud + 0x00002F70u; { double v = gabi::load<float>(ea); f10 = v; } }
comparef(0,f10,f29);
if (!conditions[0]) { goto L_02594448; }
t7 = 0x10480000u;
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x00002FDCu);
t3 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t7 + 0xFFFFAD18u);
t5 = 0x00000000u;
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
t10 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x00002FDCu);
{ uint32_t ea = hud + 0x00002F70u; gabi::store<float>(ea, f1); }
t10 = t10 + 0x00000001u;
gmem_st16(hud + 0x00002FDCu, t10);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = gmem_ld8(t3 + 0x00005CEAu);
compares(0,(int32_t)t12,6);
if (conditions[2]) { goto L_02594468; }
goto L_025944A8;
L_02594448: ;
t8 = 0x10480000u;
{ uint32_t ea = hud + 0x00002F70u; gabi::store<float>(ea, f29); }
t11 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t8 + 0xFFFFAD18u);
gmem_st16(hud + 0x00002FDCu, t11);
L_02594458: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = gmem_ld8(t3 + 0x00005CEAu);
compares(0,(int32_t)t12,6);
if (!conditions[2]) { goto L_025944A8; }
L_02594468: ;
t6 = 0x101F0000u;
t5 = 0x00000001u;
t27 = 0x10480000u;
t8 = 0x101D0000u;
gmem_st8(t6 + 0xFFFFA078u, t5);
t27 = t27 + 0xFFFFB03Cu;
t0 = 0x00000003u;
t4 = gmem_ld8(t8 + 0x00005F44u);
gmem_st8(t27 + 0x00000044u, t0);
gmem_st8(t27 + 0x00000043u, t4);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = gmem_ld32(t3 + 0x00005B2Cu);
t4 = gmem_ld32(t12 + 0x000003BCu);
t0 = t4 & 0x00008000u; compares(0,(s32)t0,0);
if (conditions[2]) { goto L_0259485C; }
goto L_025947B8;
L_025944A8: ;
t10 = 0x101F0000u;
t7 = gmem_ld8(t10 + 0xFFFFA078u);
compares(0,(int32_t)t7,0);
if (conditions[2]) { goto L_025944E8; }
t3 = hud;
t3=gabi::call<u32>(0x02591B4C ,t3);
t9 = 0x101F0000u;
t27 = 0x10480000u;
gmem_st8(t9 + 0xFFFFA078u, zero);
t27 = t27 + 0xFFFFB03Cu;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = gmem_ld32(t3 + 0x00005B2Cu);
t4 = gmem_ld32(t12 + 0x000003BCu);
t0 = t4 & 0x00008000u; compares(0,(s32)t0,0);
if (conditions[2]) { goto L_0259485C; }
goto L_025947B8;
L_025944E8: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t8 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t3 + 0x00005B5Eu);
compares(0,(int32_t)t8,0);
if (!conditions[2]) { goto L_02594518; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
{ uint32_t ea = t3 + 0x00005B44u; { double v = gabi::load<float>(ea); f9 = v; } }
f11 = u64_as_f64(ppc_fctiwz(f9));
t9 = scratch.a + 0x00000010u;
gmem_st32(0u + t9, (uint32_t)f64_as_u64(f11));
t9 = (uint32_t)(int32_t)(int16_t)gmem_ld16(scratch.a + 0x00000012u);
t9 = (uint32_t)(int32_t)(int16_t)t9; compares(0,(s32)t9,0);
if (conditions[2]) { goto L_02594684; }
L_02594518: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t10 = gmem_ld32(saveSegmentBase + 0xFFFF84DCu);
t12 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t3 + 0x00005B5Eu);
t11 = gmem_ld16(t10 + 0x00000020u);
t26 = t11 + t12;
t26 = (uint32_t)(int32_t)(int16_t)t26;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t3 + 0x00005B5Eu);
compares(0,(int32_t)t0,0);
if (!conditions[1]) { goto L_02594564; }
{ int32_t s = (int32_t)t26; carry = s < 0 && (s & 0x00000003); t5 = (uint32_t)(s >> 2); }
{ uint64_t t = (uint64_t)t5 + carry; t4 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
t27 = rotl32(t4, 2) & 0xFFFFFFFCu;
t27 = (uint32_t)(int32_t)(int16_t)t27;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t9 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t3 + 0x00005B5Eu);
compares(0,(int32_t)t9,0);
if (conditions[2]) { goto L_02594600; }
goto L_0259459C;
L_02594564: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
{ uint32_t ea = t3 + 0x00005B44u; { double v = gabi::load<float>(ea); f13 = v; } }
t6 = gmem_ld32(saveSegmentBase + 0xFFFF84DCu);
f12 = u64_as_f64(ppc_fctiwz(f13));
t7 = scratch.a + 0x00000010u;
t8 = gmem_ld16(t6 + 0x00000022u);
gmem_st32(0u + t7, (uint32_t)f64_as_u64(f12));
t7 = (uint32_t)(int32_t)(int16_t)gmem_ld16(scratch.a + 0x00000012u);
t27 = t8 + t7;
t27 = (uint32_t)(int32_t)(int16_t)t27;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t9 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t3 + 0x00005B5Eu);
compares(0,(int32_t)t9,0);
if (conditions[2]) { goto L_02594600; }
L_0259459C: ;
compares(0,(int32_t)t26,80);
t11 = gmem_ld8(hud + 0x00003022u);
if (!conditions[1]) { goto L_025945D0; }
t26 = 0x00000050u;
t10 = t26 - t11;
gmem_st16(hud + 0x00003010u, t10);
t12 = gmem_ld32(saveSegmentBase + 0xFFFF84DCu);
gmem_st16(t12 + 0x00000020u, t26);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t4 = 0x00000001u;
gmem_st16(t3 + 0x00005B5Eu, zero);
gmem_st16(hud + 0x0000108Au, t4);
goto L_02594600;
L_025945D0: ;
compares(0,(int32_t)t26,0);
if (!conditions[0]) { goto L_025945DC; }
t26 = 0x00000000u;
L_025945DC: ;
t10 = t26 - t11;
gmem_st16(hud + 0x00003010u, t10);
t12 = gmem_ld32(saveSegmentBase + 0xFFFF84DCu);
t0 = t26 & 0x000000FFu;
gmem_st16(t12 + 0x00000020u, t0);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t4 = 0x00000001u;
gmem_st16(t3 + 0x00005B5Eu, zero);
gmem_st16(hud + 0x0000108Au, t4);
L_02594600: ;
{ int32_t s = (int32_t)t26; carry = s < 0 && (s & 0x00000003); t6 = (uint32_t)(s >> 2); }
{ uint64_t t = (uint64_t)t6 + carry; t5 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
t0 = rotl32(t5, 2) & 0xFFFFFFFCu;
compares(0,(int32_t)t27,(int32_t)t0);
t8 = gmem_ld8(hud + 0x00003023u);
if (!conditions[1]) { goto L_02594644; }
t7 = t0 - t8;
gmem_st16(hud + 0x0000300Eu, t7);
t9 = gmem_ld32(saveSegmentBase + 0xFFFF84DCu);
t27 = t0 & 0x000000FFu;
gmem_st16(t9 + 0x00000022u, t27);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
{ uint32_t ea = t3 + 0x00005B44u; gabi::store<float>(ea, f31); }
t10 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x0000300Eu);
compares(0,(int32_t)t10,4);
if (conditions[0]) { goto L_02594684; }
goto L_02594678;
L_02594644: ;
compares(0,(int32_t)t27,0);
if (!conditions[0]) { goto L_02594650; }
t27 = 0x00000000u;
L_02594650: ;
t7 = t27 - t8;
gmem_st16(hud + 0x0000300Eu, t7);
t9 = gmem_ld32(saveSegmentBase + 0xFFFF84DCu);
t27 = t27 & 0x000000FFu;
gmem_st16(t9 + 0x00000022u, t27);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
{ uint32_t ea = t3 + 0x00005B44u; gabi::store<float>(ea, f31); }
t10 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x0000300Eu);
compares(0,(int32_t)t10,4);
if (conditions[0]) { goto L_02594684; }
L_02594678: ;
t5 = 0x101F0000u;
t0 = 0x00000001u;
gmem_st8(t5 + 0xFFFFA077u, t0);
L_02594684: ;
t0 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x00003010u);
compares(0,(int32_t)t0,0);
t11 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x0000300Eu);
if (conditions[0]) { goto L_025946C0; }
if (!conditions[1]) { goto L_025946D8; }
t12 = gmem_ld8(hud + 0x00003022u);
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x00003010u);
t12 = t12 + 0x00000001u;
gmem_st8(hud + 0x00003022u, t12);
t4 = t4 + 0xFFFFFFFFu;
compares(0,(int32_t)t11,0);
gmem_st16(hud + 0x00003010u, t4);
if (conditions[0]) { goto L_02594774; }
if (conditions[1]) { goto L_02594704; }
goto L_025946E4;
L_025946C0: ;
t5 = gmem_ld8(hud + 0x00003022u);
t6 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x00003010u);
t5 = t5 + 0xFFFFFFFFu;
t6 = t6 + 0x00000001u;
gmem_st8(hud + 0x00003022u, t5);
gmem_st16(hud + 0x00003010u, t6);
L_025946D8: ;
compares(0,(int32_t)t11,0);
if (conditions[0]) { goto L_02594774; }
if (conditions[1]) { goto L_02594704; }
L_025946E4: ;
t27 = 0x10480000u;
t27 = t27 + 0xFFFFB03Cu;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = gmem_ld32(t3 + 0x00005B2Cu);
t4 = gmem_ld32(t12 + 0x000003BCu);
t0 = t4 & 0x00008000u; compares(0,(s32)t0,0);
if (conditions[2]) { goto L_0259485C; }
goto L_025947B8;
L_02594704: ;
t7 = gmem_ld8(hud + 0x00003023u);
t11 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x0000300Eu);
t7 = t7 + 0x00000001u;
t11 = t11 + 0xFFFFFFFFu;
gmem_st8(hud + 0x00003023u, t7);
t11 = (uint32_t)(int32_t)(int16_t)t11;
t12 = 0x101F0000u;
gmem_st16(hud + 0x0000300Eu, t11);
t8 = gmem_ld8(t12 + 0xFFFFA077u);
compares(0,(int32_t)t8,0);
if (conditions[2]) { goto L_0259478C; }
t10 = gmem_ld8(hud + 0x00003023u);
t9 = t10 & 0x00000003u; compares(0,(s32)t9,0);
if (!conditions[2]) { goto L_02594748; }
t3 = 0x0000087Cu;
t3=gabi::call<u32>(0x025E1988 ,t3);
t11 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x0000300Eu);
L_02594748: ;
compares(0,(int32_t)t11,0);
if (!conditions[2]) { goto L_0259478C; }
t10 = 0x101F0000u;
gmem_st8(t10 + 0xFFFFA077u, zero);
t26 = gmem_ld8(hud + 0x00003023u);
gmem_st16(hud + 0x00000F3Au, zero);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t27 = 0x10480000u;
gmem_st16(t3 + 0x00005BACu, t26);
t27 = t27 + 0xFFFFB03Cu;
goto L_025947A4;
L_02594774: ;
t11 = gmem_ld8(hud + 0x00003023u);
t12 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x0000300Eu);
t11 = t11 + 0xFFFFFFFFu;
t12 = t12 + 0x00000001u;
gmem_st8(hud + 0x00003023u, t11);
gmem_st16(hud + 0x0000300Eu, t12);
L_0259478C: ;
gmem_st16(hud + 0x00000F3Au, zero);
t26 = gmem_ld8(hud + 0x00003023u);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t27 = 0x10480000u;
t27 = t27 + 0xFFFFB03Cu;
gmem_st16(t3 + 0x00005BACu, t26);
L_025947A4: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = gmem_ld32(t3 + 0x00005B2Cu);
t4 = gmem_ld32(t12 + 0x000003BCu);
t0 = t4 & 0x00008000u; compares(0,(s32)t0,0);
if (conditions[2]) { goto L_0259485C; }
L_025947B8: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t5 = gmem_ld8(t3 + 0x00005CEAu);
compares(0,(int32_t)t5,6);
if (conditions[2]) { goto L_0259485C; }
t6 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x00000ADAu);
compares(0,(int32_t)t6,0);
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x00000B12u);
if (!conditions[2]) { goto L_025947E0; }
t7 = 0x00000001u;
gmem_st16(hud + 0x00000ADAu, t7);
L_025947E0: ;
compares(0,(int32_t)t4,29);
if (conditions[0]) { goto L_0259480C; }
t4 = 0x00000000u;
t3 = 0x0000000Fu;
t5 = t4;
gmem_st16(hud + 0x00000B12u, t4);
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
t6 = gmem_ld32(hud + 0x00003008u);
t9 = t6 & 0x00004000u; compares(0,(s32)t9,0);
if (conditions[2]) { goto L_02594878; }
goto L_02594900;
L_0259480C: ;
t4 = t4 + 0x00000001u;
t4 = (uint32_t)(int32_t)(int16_t)t4;
compares(0,(int32_t)t4,15);
gmem_st16(hud + 0x00000B12u, t4);
if (!conditions[0]) { goto L_0259483C; }
t3 = 0x0000000Fu;
t5 = 0x00000000u;
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
t6 = gmem_ld32(hud + 0x00003008u);
t9 = t6 & 0x00004000u; compares(0,(s32)t9,0);
if (conditions[2]) { goto L_02594878; }
goto L_02594900;
L_0259483C: ;
{ uint64_t t = (uint64_t)(uint32_t)~t4 + 0x0000001Eu + 1; t4 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
t3 = 0x0000000Fu;
t5 = 0x00000000u;
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
t6 = gmem_ld32(hud + 0x00003008u);
t9 = t6 & 0x00004000u; compares(0,(s32)t9,0);
if (conditions[2]) { goto L_02594878; }
goto L_02594900;
L_0259485C: ;
t8 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x00000ADAu);
compares(0,(int32_t)t8,1);
if (!conditions[2]) { goto L_0259486C; }
gmem_st16(hud + 0x00000ADAu, zero);
L_0259486C: ;
t6 = gmem_ld32(hud + 0x00003008u);
t9 = t6 & 0x00004000u; compares(0,(s32)t9,0);
if (!conditions[2]) { goto L_02594900; }
L_02594878: ;
t10 = t6 & 0x00000040u; compares(0,(s32)t10,0);
if (conditions[2]) { goto L_025948B4; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t11 = gmem_ld8(t3 + 0x00005292u);
{ uint64_t t = (uint64_t)t11 + 0xFFFFFFFFu; t12 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
{ uint64_t t = (uint64_t)(uint32_t)~t12 + t11 + carry; t0 = (uint32_t)t; carry = (uint8_t)(t >> 32); } compares(0,(s32)t0,0);
if (conditions[2]) { goto L_025948B4; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld16(t3 + 0x000052A6u);
t4 = t0 & 0x00000010u; compares(0,(s32)t4,0);
if (conditions[2]) { goto L_025948B4; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t6 = gmem_ld32(t3 + 0x00005CDCu);
t0 = t6 & 0x00002000u; compares(0,(s32)t0,0);
if (conditions[2]) { goto L_02594900; }
L_025948B4: ;
t5 = 0x101F0000u;
t7 = gmem_ld8(t5 + 0xFFFFA073u);
compares(0,(int32_t)t7,4);
if (conditions[2]) { goto L_02594900; }
t11 = 0x00080000u;
t0 = 0x00000080u;
t10 = t11 | 0x0020u;
t9 = t0 | 0x00320000u;
t0 = t10 | 0x01000000u;
t6 = gmem_ld32(hud + 0x00003008u);
t4 = t9 | t0;
t5 = t4 & t6; compares(0,(s32)t5,0);
if (!conditions[2]) { goto L_02594900; }
t4 = 0x101F0000u;
t7 = gmem_ld8(t4 + 0xFFFFA06Cu);
compares(0,(int32_t)t7,0);
if (!conditions[2]) { goto L_02594900; }
t8 = t6 & 0x00C00000u; compares(0,(s32)t8,0);
if (conditions[2]) { goto L_02594948; }
L_02594900: ;
t3 = 0x00000000u;
t3=gabi::call<u32>(0x02591AB4 ,t3);
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x00003018u);
compares(0,(int32_t)t4,0);
if (conditions[1]) { goto L_025949A0; }
t4 = 0x00000000u;
t3 = 0x00000005u;
t5 = t4;
gmem_st16(hud + 0x00003018u, t4);
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
{ uint32_t ea = hud + 0x00002FC0u; gabi::store<float>(ea, f1); }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t26 = gmem_ld8(t3 + 0x00005BBAu);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
compares(0,(int32_t)t26,0);
t6 = t3 + 0x000012A0u;
if (!conditions[2]) { goto L_02594AB0; }
goto L_02594ACC;
L_02594948: ;
t9 = t6 & 0x00000018u; compares(0,(s32)t9,0);
t3 = 0x00000000u;
if (conditions[2]) { goto L_02594A0C; }
t3=gabi::call<u32>(0x02591AB4 ,t3);
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x00003018u);
compares(0,(int32_t)t4,3);
if (conditions[1]) { goto L_025949A0; }
if (!conditions[0]) { goto L_025949D8; }
t4 = t4 + 0x00000001u;
t3 = 0x00000005u;
t4 = (uint32_t)(int32_t)(int16_t)t4;
t5 = 0x00000000u;
gmem_st16(hud + 0x00003018u, t4);
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
{ uint32_t ea = hud + 0x00002FC0u; gabi::store<float>(ea, f1); }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t26 = gmem_ld8(t3 + 0x00005BBAu);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
compares(0,(int32_t)t26,0);
t6 = t3 + 0x000012A0u;
if (!conditions[2]) { goto L_02594AB0; }
goto L_02594ACC;
L_025949A0: ;
t4 = t4 + 0xFFFFFFFFu;
t3 = 0x00000005u;
t4 = (uint32_t)(int32_t)(int16_t)t4;
t5 = 0x00000000u;
gmem_st16(hud + 0x00003018u, t4);
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
{ uint32_t ea = hud + 0x00002FC0u; gabi::store<float>(ea, f1); }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t26 = gmem_ld8(t3 + 0x00005BBAu);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
compares(0,(int32_t)t26,0);
t6 = t3 + 0x000012A0u;
if (!conditions[2]) { goto L_02594AB0; }
goto L_02594ACC;
L_025949D8: ;
t4 = 0x00000003u;
t3 = 0x00000005u;
t5 = 0x00000000u;
gmem_st16(hud + 0x00003018u, t4);
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
{ uint32_t ea = hud + 0x00002FC0u; gabi::store<float>(ea, f1); }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t26 = gmem_ld8(t3 + 0x00005BBAu);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
compares(0,(int32_t)t26,0);
t6 = t3 + 0x000012A0u;
if (!conditions[2]) { goto L_02594AB0; }
goto L_02594ACC;
L_02594A0C: ;
t3=gabi::call<u32>(0x02591B00 ,t3);
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x00003018u);
compares(0,(int32_t)t4,5);
if (!conditions[0]) { goto L_02594A38; }
t10 = t4 + 0x00000001u;
gmem_st16(hud + 0x00003018u, t10);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t11 = gmem_ld8(t3 + 0x00005CEAu);
compares(0,(int32_t)t11,6);
if (conditions[2]) { goto L_02594A50; }
goto L_02594A80;
L_02594A38: ;
t10 = 0x00000005u;
gmem_st16(hud + 0x00003018u, t10);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t11 = gmem_ld8(t3 + 0x00005CEAu);
compares(0,(int32_t)t11,6);
if (!conditions[2]) { goto L_02594A80; }
L_02594A50: ;
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x00003018u);
t3 = 0x00000005u;
t5 = 0x00000000u;
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
{ uint32_t ea = hud + 0x00002FC0u; gabi::store<float>(ea, f1); }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t26 = gmem_ld8(t3 + 0x00005BBAu);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
compares(0,(int32_t)t26,0);
t6 = t3 + 0x000012A0u;
if (!conditions[2]) { goto L_02594AB0; }
goto L_02594ACC;
L_02594A80: ;
t3=gabi::call<u32>(0x025E1C84 ,t3,t4,t5,t6,t7,t8,t9,t10);
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x00003018u);
t3 = 0x00000005u;
t5 = 0x00000000u;
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
{ uint32_t ea = hud + 0x00002FC0u; gabi::store<float>(ea, f1); }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t26 = gmem_ld8(t3 + 0x00005BBAu);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
compares(0,(int32_t)t26,0);
t6 = t3 + 0x000012A0u;
if (conditions[2]) { goto L_02594ACC; }
L_02594AB0: ;
t26 = gmem_ld8(t6 + 0x0000491Au);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BB7u, t26);
t12 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x000027F2u);
compares(0,(int32_t)t12,0);
if (conditions[2]) { goto L_02594D0C; }
goto L_02594C60;
L_02594ACC: ;
t6 = gmem_ld32(hud + 0x00003008u);
t12 = t6 & 0x00004000u; compares(0,(s32)t12,0);
if (!conditions[2]) { goto L_02594B2C; }
t0 = t6 & 0x00000040u; compares(0,(s32)t0,0);
if (conditions[2]) { goto L_02594B44; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t4 = gmem_ld8(t3 + 0x00005292u);
{ uint64_t t = (uint64_t)t4 + 0xFFFFFFFFu; t5 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
{ uint64_t t = (uint64_t)(uint32_t)~t5 + t4 + carry; t0 = (uint32_t)t; carry = (uint8_t)(t >> 32); } compares(0,(s32)t0,0);
if (conditions[2]) { goto L_02594B04; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t6 = gmem_ld16(t3 + 0x000052A6u);
t7 = t6 & 0x00000004u; compares(0,(s32)t7,0);
if (!conditions[2]) { goto L_02594B20; }
L_02594B04: ;
t10 = 0x00000020u;
t9 = t10 | 0x00800000u;
t6 = gmem_ld32(hud + 0x00003008u);
t11 = t9 | 0x0018u;
t12 = t11 & t6; compares(0,(s32)t12,0);
if (conditions[2]) { goto L_02594B9C; }
goto L_02594B58;
L_02594B20: ;
t6 = gmem_ld32(hud + 0x00003008u);
t8 = t6 & 0x00000020u; compares(0,(s32)t8,0);
if (!conditions[2]) { goto L_02594B44; }
L_02594B2C: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BB7u, zero);
t12 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x000027F2u);
compares(0,(int32_t)t12,0);
if (conditions[2]) { goto L_02594D0C; }
goto L_02594C60;
L_02594B44: ;
t10 = 0x00000020u;
t9 = t10 | 0x00800000u;
t11 = t9 | 0x0018u;
t12 = t11 & t6; compares(0,(s32)t12,0);
if (conditions[2]) { goto L_02594B9C; }
L_02594B58: ;
t12 = 0x101F0000u;
t0 = gmem_ld8(t12 + 0xFFFFA073u);
compares(0,(int32_t)t0,5);
if (conditions[2]) { goto L_02594B78; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t4 = 0x00000007u;
gmem_st8(t3 + 0x00005BB6u, t4);
t6 = gmem_ld32(hud + 0x00003008u);
L_02594B78: ;
t5 = t6 & 0x00800000u; compares(0,(s32)t5,0);
if (conditions[2]) { goto L_02594C54; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t6 = 0x00000017u;
gmem_st8(t3 + 0x00005BB7u, t6);
t12 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x000027F2u);
compares(0,(int32_t)t12,0);
if (conditions[2]) { goto L_02594D0C; }
goto L_02594C60;
L_02594B9C: ;
t7 = t6 & 0x00000100u; compares(0,(s32)t7,0);
if (conditions[2]) { goto L_02594C54; }
t26 = gmem_ld8(t27 + 0x00000062u);
compares(0,(int32_t)t26,1);
if (!conditions[2]) { goto L_02594BD0; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t8 = 0x00000019u;
gmem_st8(t3 + 0x00005BB7u, t8);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BBAu);
compares(0,(int32_t)t0,0);
if (conditions[2]) { goto L_02594C2C; }
goto L_02594C24;
L_02594BD0: ;
compares(0,(int32_t)t26,3);
if (!conditions[2]) { goto L_02594BF8; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t9 = 0x00000017u;
gmem_st8(t3 + 0x00005BB7u, t9);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BBAu);
compares(0,(int32_t)t0,0);
if (conditions[2]) { goto L_02594C2C; }
goto L_02594C24;
L_02594BF8: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
compares(0,(int32_t)t26,4);
t11 = 0x00000000u;
t7 = t3 + 0x000012A0u;
if (!conditions[2]) { goto L_02594C10; }
t11 = 0x00000046u;
L_02594C10: ;
gmem_st8(t7 + 0x00004917u, t11);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BBAu);
compares(0,(int32_t)t0,0);
if (conditions[2]) { goto L_02594C2C; }
L_02594C24: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BBAu, zero);
L_02594C2C: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t11 = gmem_ld8(t3 + 0x00005BB9u);
compares(0,(int32_t)t11,0);
if (conditions[2]) { goto L_02594C54; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t26 = gmem_ld8(t3 + 0x00005BBAu);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BB6u, t26);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BB9u, zero);
L_02594C54: ;
t12 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x000027F2u);
compares(0,(int32_t)t12,0);
if (conditions[2]) { goto L_02594D0C; }
L_02594C60: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t0,0);
if (conditions[2]) { goto L_02594CF4; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t4 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t4,26);
if (!conditions[2]) { goto L_02594C9C; }
t3 = 0x0000085Fu;
t3=gabi::call<u32>(0x025E1988 ,t3);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BB7u);
gmem_st16(hud + 0x000027F2u, zero);
gmem_st8(hud + 0x0000301Fu, t0);
goto L_02594D0C;
L_02594C9C: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t5 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t5,49);
if (conditions[2]) { goto L_02594CFC; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t6 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t6,45);
if (conditions[2]) { goto L_02594CFC; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t7 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t7,46);
if (conditions[2]) { goto L_02594CFC; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t8 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t8,61);
if (conditions[2]) { goto L_02594CFC; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t9 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t9,7);
if (conditions[2]) { goto L_02594CF0; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
L_02594CF0: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
L_02594CF4: ;
t10 = 0x00000001u;
gmem_st16(hud + 0x00001E52u, t10);
L_02594CFC: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BB7u);
gmem_st16(hud + 0x000027F2u, zero);
gmem_st8(hud + 0x0000301Fu, t0);
L_02594D0C: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t11 = gmem_ld8(hud + 0x0000301Fu);
t12 = gmem_ld8(t3 + 0x00005BB7u);
compareu(0,t11,t12);
if (conditions[2]) { goto L_02594D28; }
t4 = 0x00000001u;
gmem_st16(hud + 0x000027F2u, t4);
L_02594D28: ;
t0 = gmem_ld8(t27 + 0x0000003Eu);
t6 = gmem_ld32(hud + 0x00003008u);
gmem_st8(scratch.a + 0x00000008u, t0);
t5 = t6 & 0x00004000u; compares(0,(s32)t5,0);
t26 = 0x101F0000u;
if (!conditions[2]) { goto L_02594E94; }
t7 = t6 & 0x00000040u; compares(0,(s32)t7,0);
if (conditions[2]) { goto L_02594D94; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t8 = gmem_ld8(t3 + 0x00005292u);
{ uint64_t t = (uint64_t)t8 + 0xFFFFFFFFu; t9 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
{ uint64_t t = (uint64_t)(uint32_t)~t9 + t8 + carry; t5 = (uint32_t)t; carry = (uint8_t)(t >> 32); } compares(0,(s32)t5,0);
if (conditions[2]) { goto L_02594D6C; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t10 = gmem_ld16(t3 + 0x000052A6u);
t11 = t10 & 0x00000002u; compares(0,(s32)t11,0);
if (!conditions[2]) { goto L_02594D88; }
L_02594D6C: ;
t4 = 0x00100000u;
t0 = t4 | 0x0080u;
t6 = gmem_ld32(hud + 0x00003008u);
t5 = t0 | 0x01000000u;
t7 = t5 & t6; compares(0,(s32)t7,0);
if (conditions[2]) { goto L_02594DA8; }
goto L_02594E94;
L_02594D88: ;
t6 = gmem_ld32(hud + 0x00003008u);
t12 = t6 & 0x00000020u; compares(0,(s32)t12,0);
if (conditions[2]) { goto L_02594E94; }
L_02594D94: ;
t4 = 0x00100000u;
t0 = t4 | 0x0080u;
t5 = t0 | 0x01000000u;
t7 = t5 & t6; compares(0,(s32)t7,0);
if (!conditions[2]) { goto L_02594E94; }
L_02594DA8: ;
t8 = t6 & 0x00200000u; compares(0,(s32)t8,0);
if (conditions[2]) { goto L_02594DB8; }
t9 = t6 & 0x00000030u; compares(0,(s32)t9,0);
if (conditions[2]) { goto L_02594E94; }
L_02594DB8: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t10 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t10,62);
if (conditions[2]) { goto L_02594E94; }
t6 = gmem_ld32(hud + 0x00003008u);
t11 = t6 & 0x00000020u; compares(0,(s32)t11,0);
if (conditions[2]) { goto L_02594EE4; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
{ uint32_t ea = t3 + 0x00005150u; t12 = gmem_ld32(ea); t3 = ea; }
t0 = gmem_ld32(t12 + 0x0000015Cu);
target = t0;
t3=gabi::call_ptr<u32>(target,t3);
t5 = gmem_ld16(t3 + 0x0000000Au);
t4 = t5 & 0x00000003u;
compares(0,(int32_t)t4,1);
if (conditions[2]) { goto L_02594EC0; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
{ uint32_t ea = t3 + 0x00005150u; t6 = gmem_ld32(ea); t3 = ea; }
t7 = gmem_ld32(t6 + 0x0000015Cu);
target = t7;
t3=gabi::call_ptr<u32>(target,t3);
t9 = gmem_ld16(t3 + 0x0000000Au);
t8 = t9 & 0x00000003u; compares(0,(s32)t8,0);
if (!conditions[2]) { goto L_0259506C; }
t10 = gmem_ld8(t27 + 0x00000063u);
compareu(0,t10,0x0003u);
if (!conditions[1]) { goto L_02594E4C; }
compareu(0,t10,0x0006u);
if (conditions[0]) { goto L_02594EC0; }
if (conditions[2]) { goto L_02594E94; }
compareu(0,t10,0x000Bu);
if (conditions[0]) { goto L_02594E70; }
if (conditions[2]) { goto L_02594E94; }
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t26 + 0xFFFFA04Eu);
compares(0,(int32_t)t4,9);
if (conditions[2]) { goto L_02594ED8; }
goto L_02594ECC;
L_02594E4C: ;
t11 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t26 + 0xFFFFA04Eu);
compares(0,(int32_t)t11,8);
if (conditions[2]) { goto L_02594E64; }
t3 = hud + 0x0000302Au;
t4 = 0x00000001u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02594E64: ;
t0 = 0x00000008u;
gmem_st16(t26 + 0xFFFFA04Eu, t0);
goto L_0259506C;
L_02594E70: ;
t12 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t26 + 0xFFFFA04Eu);
compares(0,(int32_t)t12,11);
if (conditions[2]) { goto L_02594E88; }
t3 = hud + 0x0000302Au;
t4 = 0x00000001u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02594E88: ;
t0 = 0x0000000Bu;
gmem_st16(t26 + 0xFFFFA04Eu, t0);
goto L_0259506C;
L_02594E94: ;
t0 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t26 + 0xFFFFA04Eu);
compares(0,(int32_t)t0,1);
if (conditions[2]) { goto L_02594EB4; }
t3 = hud + 0x0000302Au;
t4 = 0x00000001u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
t3 = scratch.a + 0x00000008u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02594EB4: ;
t0 = 0x00000001u;
gmem_st16(t26 + 0xFFFFA04Eu, t0);
goto L_0259506C;
L_02594EC0: ;
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t26 + 0xFFFFA04Eu);
compares(0,(int32_t)t4,9);
if (conditions[2]) { goto L_02594ED8; }
L_02594ECC: ;
t3 = hud + 0x0000302Au;
t4 = 0x00000001u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02594ED8: ;
t0 = 0x00000009u;
gmem_st16(t26 + 0xFFFFA04Eu, t0);
goto L_0259506C;
L_02594EE4: ;
t5 = t6 & 0x00020000u; compares(0,(s32)t5,0);
if (conditions[2]) { goto L_02594F10; }
t6 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t26 + 0xFFFFA04Eu);
compares(0,(int32_t)t6,2);
if (conditions[2]) { goto L_02594F04; }
t3 = hud + 0x0000302Au;
t4 = 0x00000001u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02594F04: ;
t0 = 0x00000002u;
gmem_st16(t26 + 0xFFFFA04Eu, t0);
goto L_0259506C;
L_02594F10: ;
t11 = 0x101F0000u;
t11 = gmem_ld8(t11 + 0xFFFFA069u);
compares(0,(int32_t)t11,0);
if (conditions[2]) { goto L_02594F94; }
t10 = 0x101F0000u;
t10 = gmem_ld8(t10 + 0xFFFFA06Au);
compares(0,(int32_t)t10,1);
if (conditions[2]) { goto L_02594F48; }
t12 = 0x101F0000u;
t12 = gmem_ld8(t12 + 0xFFFFA067u);
compares(0,(int32_t)t12,1);
if (!conditions[2]) { goto L_02594F7C; }
compares(0,(int32_t)t10,0);
if (!conditions[2]) { goto L_02594F7C; }
L_02594F48: ;
t11 = 0x101F0000u;
t7 = gmem_ld8(t11 + 0xFFFFA072u);
compares(0,(int32_t)t7,0);
if (!conditions[2]) { goto L_02595028; }
t8 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t26 + 0xFFFFA04Eu);
compares(0,(int32_t)t8,3);
if (conditions[2]) { goto L_02594F70; }
t3 = hud + 0x0000302Au;
t4 = 0x00000001u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02594F70: ;
t0 = 0x00000003u;
gmem_st16(t26 + 0xFFFFA04Eu, t0);
goto L_0259506C;
L_02594F7C: ;
compares(0,(int32_t)t10,2);
if (conditions[2]) { goto L_02595028; }
compares(0,(int32_t)t12,2);
if (!conditions[2]) { goto L_02594F94; }
compares(0,(int32_t)t10,0);
if (conditions[2]) { goto L_02595028; }
L_02594F94: ;
t9 = t6 & 0x00800000u; compares(0,(s32)t9,0);
if (!conditions[2]) { goto L_02595028; }
t10 = t6 & 0x00040000u; compares(0,(s32)t10,0);
if (conditions[2]) { goto L_02594FC8; }
t11 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t26 + 0xFFFFA04Eu);
compares(0,(int32_t)t11,5);
if (conditions[2]) { goto L_02594FBC; }
t3 = hud + 0x0000302Au;
t4 = 0x00000001u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02594FBC: ;
t0 = 0x00000005u;
gmem_st16(t26 + 0xFFFFA04Eu, t0);
goto L_0259506C;
L_02594FC8: ;
t12 = t6 & 0x00000100u; compares(0,(s32)t12,0);
if (conditions[2]) { goto L_02594FF4; }
t0 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t26 + 0xFFFFA04Eu);
compares(0,(int32_t)t0,7);
if (conditions[2]) { goto L_02594FE8; }
t3 = hud + 0x0000302Au;
t4 = 0x00000001u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02594FE8: ;
t0 = 0x00000007u;
gmem_st16(t26 + 0xFFFFA04Eu, t0);
goto L_0259506C;
L_02594FF4: ;
t4 = t6 & 0x00080000u; compares(0,(s32)t4,0);
if (conditions[2]) { goto L_02595020; }
t5 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t26 + 0xFFFFA04Eu);
compares(0,(int32_t)t5,6);
if (conditions[2]) { goto L_02595014; }
t3 = hud + 0x0000302Au;
t4 = 0x00000001u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02595014: ;
t0 = 0x00000006u;
gmem_st16(t26 + 0xFFFFA04Eu, t0);
goto L_0259506C;
L_02595020: ;
t6 = t6 & 0x00400000u; compares(0,(s32)t6,0);
if (conditions[2]) { goto L_0259504C; }
L_02595028: ;
t7 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t26 + 0xFFFFA04Eu);
compares(0,(int32_t)t7,4);
if (conditions[2]) { goto L_02595040; }
t3 = hud + 0x0000302Au;
t4 = 0x00000001u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02595040: ;
t0 = 0x00000004u;
gmem_st16(t26 + 0xFFFFA04Eu, t0);
goto L_0259506C;
L_0259504C: ;
t8 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t26 + 0xFFFFA04Eu);
compares(0,(int32_t)t8,0);
if (conditions[2]) { goto L_02595064; }
t3 = hud + 0x0000302Au;
t4 = 0x00000001u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02595064: ;
t0 = 0x00000000u;
gmem_st16(t26 + 0xFFFFA04Eu, t0);
L_0259506C: ;
t9 = gmem_ld8(hud + 0x0000302Au);
compares(0,(int32_t)t9,15);
t11 = 0x101F0000u;
if (conditions[2]) { goto L_025950F8; }
t3 = hud + 0x0000302Au;
t4 = 0x00000001u;
t3=gabi::call<u32>(0x02591718 ,t3,t4);
compares(0,(int32_t)t3,0);
if (!conditions[2]) { goto L_02595124; }
t10 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t26 + 0xFFFFA04Eu);
compares(0,(int32_t)t10,1);
if (conditions[2]) { goto L_025950A8; }
t3 = scratch.a + 0x00000008u;
t4 = 0x00000001u;
t3=gabi::call<u32>(0x0259172C ,t3,t4);
L_025950A8: ;
t12 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t11 + 0xFFFFA04Cu);
compares(0,(int32_t)t12,5);
if (conditions[0]) { goto L_025950DC; }
t12 = 0x00000005u;
t3 = hud + 0x0000302Au;
t4 = 0x00000001u;
gmem_st16(t11 + 0xFFFFA04Cu, t12);
t3=gabi::call<u32>(0x0259172C ,t3,t4);
t5 = gmem_ld8(t27 + 0x0000003Eu);
t6 = gmem_ld8(scratch.a + 0x00000008u);
compareu(0,t6,t5);
if (conditions[2]) { goto L_02595138; }
goto L_02595134;
L_025950DC: ;
t6 = gmem_ld8(scratch.a + 0x00000008u);
t0 = t12 + 0x00000001u;
t5 = gmem_ld8(t27 + 0x0000003Eu);
compareu(0,t6,t5);
gmem_st16(t11 + 0xFFFFA04Cu, t0);
if (conditions[2]) { goto L_02595138; }
goto L_02595134;
L_025950F8: ;
t12 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t11 + 0xFFFFA04Cu);
compares(0,(int32_t)t12,0);
if (conditions[1]) { goto L_0259511C; }
t6 = gmem_ld8(scratch.a + 0x00000008u);
t5 = gmem_ld8(t27 + 0x0000003Eu);
compareu(0,t6,t5);
gmem_st16(t11 + 0xFFFFA04Cu, zero);
if (conditions[2]) { goto L_02595138; }
goto L_02595134;
L_0259511C: ;
t4 = t12 + 0xFFFFFFFFu;
gmem_st16(t11 + 0xFFFFA04Cu, t4);
L_02595124: ;
t6 = gmem_ld8(scratch.a + 0x00000008u);
t5 = gmem_ld8(t27 + 0x0000003Eu);
compareu(0,t6,t5);
if (conditions[2]) { goto L_02595138; }
L_02595134: ;
gmem_st8(t27 + 0x0000003Eu, t6);
L_02595138: ;
t8 = gmem_ld32(hud + 0x00003008u);
t7 = t8 & 0x00000020u; compares(0,(s32)t7,0);
t26 = 0x101F0000u;
if (conditions[2]) { goto L_02595228; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
{ uint32_t ea = t3 + 0x00005150u; t10 = gmem_ld32(ea); t3 = ea; }
t11 = gmem_ld32(t10 + 0x0000015Cu);
target = t11;
t3=gabi::call_ptr<u32>(target,t3);
t0 = gmem_ld16(t3 + 0x0000000Au);
t12 = t0 & 0x00000003u; compares(0,(s32)t12,0);
if (!conditions[2]) { goto L_02595228; }
t4 = gmem_ld8(t27 + 0x00000063u);
compareu(0,t4,0x0002u);
if (conditions[0]) { goto L_02595228; }
compareu(0,t4,0x0003u);
if (!conditions[1]) { goto L_0259518C; }
compareu(0,t4,0x0007u);
if (conditions[0]) { goto L_02595228; }
compareu(0,t4,0x0009u);
if (conditions[1]) { goto L_02595228; }
L_0259518C: ;
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t26 + 0xFFFFA050u);
compares(0,(int32_t)t4,3);
if (conditions[0]) { goto L_025951CC; }
if (!conditions[1]) { goto L_025951FC; }
t4 = t4 + 0xFFFFFFFFu;
t3 = 0x00000005u;
t4 = (uint32_t)(int32_t)(int16_t)t4;
t5 = 0x00000000u;
gmem_st16(t26 + 0xFFFFA050u, t4);
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
f31 = f1;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t9 = gmem_ld8(t3 + 0x00005CEAu);
compares(0,(int32_t)t9,8);
if (!conditions[2]) { goto L_025952AC; }
goto L_02595294;
L_025951CC: ;
t4 = t4 + 0x00000001u;
t3 = 0x00000005u;
t4 = (uint32_t)(int32_t)(int16_t)t4;
t5 = 0x00000000u;
gmem_st16(t26 + 0xFFFFA050u, t4);
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
f31 = f1;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t9 = gmem_ld8(t3 + 0x00005CEAu);
compares(0,(int32_t)t9,8);
if (!conditions[2]) { goto L_025952AC; }
goto L_02595294;
L_025951FC: ;
t4 = 0x00000003u;
t3 = 0x00000005u;
t5 = 0x00000000u;
gmem_st16(t26 + 0xFFFFA050u, t4);
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
f31 = f1;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t9 = gmem_ld8(t3 + 0x00005CEAu);
compares(0,(int32_t)t9,8);
if (!conditions[2]) { goto L_025952AC; }
goto L_02595294;
L_02595228: ;
t5 = gmem_ld8(hud + 0x00003028u);
t5 = t5 | 0x0001u;
gmem_st8(hud + 0x00003028u, t5);
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t26 + 0xFFFFA050u);
compares(0,(int32_t)t4,5);
if (!conditions[0]) { goto L_0259525C; }
t6 = t4 + 0x00000001u;
gmem_st16(t26 + 0xFFFFA050u, t6);
t7 = gmem_ld8(hud + 0x00003038u);
t7 = t7 | 0x0001u;
gmem_st8(hud + 0x00003038u, t7);
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t26 + 0xFFFFA050u);
goto L_02595274;
L_0259525C: ;
t6 = 0x00000005u;
gmem_st16(t26 + 0xFFFFA050u, t6);
t7 = gmem_ld8(hud + 0x00003038u);
t7 = t7 | 0x0001u;
gmem_st8(hud + 0x00003038u, t7);
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t26 + 0xFFFFA050u);
L_02595274: ;
t3 = 0x00000005u;
t5 = 0x00000000u;
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
f31 = f1;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t9 = gmem_ld8(t3 + 0x00005CEAu);
compares(0,(int32_t)t9,8);
if (!conditions[2]) { goto L_025952AC; }
L_02595294: ;
t3 = 0x025E0000u;
t0 = 0x00000076u;
t4 = scratch.a + 0x0000000Au;
t3 = t3 + 0x0000121Cu;
gmem_st16(scratch.a + 0x0000000Au, t0);
t3=gabi::call<u32>(0x025D5218 ,t3,t4);
L_025952AC: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t0,0);
if (!conditions[2]) { goto L_0259539C; }
t3 = hud + 0x00001D3Cu;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = hud + 0x00001E54u;
t25 = hud + 0x00001E1Cu;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = hud + 0x00001DE4u;
t26 = hud + 0x00001DACu;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = hud + 0x0000274Cu;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = hud + 0x00002784u;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = hud + 0x000027BCu;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = hud + 0x000027F4u;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
f1 = f31;
t3 = t25;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
f1 = f31;
t3 = t26;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
f1 = f31;
t3 = hud + 0x0000290Cu;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BBAu, zero);
t3 = hud;
t3=gabi::call<u32>(0x025986DC ,t3);
t3 = hud;
t3=gabi::call<u32>(0x02599584 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259A5B0 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259ACD4 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259B698 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259C0EC ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259C3CC ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259C454 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259C7FC ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259CCA8 ,t3);
t3=gabi::call<u32>(0x0259188C ,t3,t4,t5,t6,t7,t8,t9,t10);
t3 = hud;
t3=gabi::call<u32>(0x0259CDC4 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259C2BC ,t3);
t10 = gmem_ld32(hud + 0x00003008u);
t0 = t10 & 0x00000002u; compares(0,(s32)t0,0);
if (conditions[2]) { goto L_025956CC; }
goto L_02595690;
L_0259539C: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t8 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t8,26);
if (conditions[2]) { goto L_025953DC; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t9 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t9,49);
if (conditions[2]) { goto L_025953DC; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t10 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t10,45);
if (conditions[2]) { goto L_025953DC; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t12,46);
if (!conditions[2]) { goto L_025954BC; }
L_025953DC: ;
t3 = hud + 0x00001D3Cu;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
f1 = f31;
t25 = hud + 0x00001E1Cu;
t3 = hud + 0x00001E54u;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
f1 = f31;
t26 = hud + 0x00001DACu;
t3 = hud + 0x00001DE4u;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
t3 = t25;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = t26;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = hud + 0x0000274Cu;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = hud + 0x00002784u;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = hud + 0x000027BCu;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = hud + 0x000027F4u;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
f1 = f31;
t3 = hud + 0x0000290Cu;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BBAu, zero);
t3 = hud;
t3=gabi::call<u32>(0x025986DC ,t3);
t3 = hud;
t3=gabi::call<u32>(0x02599584 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259A5B0 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259ACD4 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259B698 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259C0EC ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259C3CC ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259C454 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259C7FC ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259CCA8 ,t3);
t3=gabi::call<u32>(0x0259188C ,t3,t4,t5,t6,t7,t8,t9,t10);
t3 = hud;
t3=gabi::call<u32>(0x0259CDC4 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259C2BC ,t3);
t10 = gmem_ld32(hud + 0x00003008u);
t0 = t10 & 0x00000002u; compares(0,(s32)t0,0);
if (conditions[2]) { goto L_025956CC; }
goto L_02595690;
L_025954BC: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t7 = gmem_ld8(t3 + 0x00005BB7u);
t3 = hud + 0x00001D3Cu;
t26 = hud + 0x00001DACu;
compares(0,(int32_t)t7,61);
t25 = hud + 0x00001E1Cu;
if (!conditions[2]) { goto L_025955BC; }
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = hud + 0x00001E54u;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = hud + 0x00001DE4u;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
f1 = f31;
t3 = t25;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
f1 = f31;
t3 = t26;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
f1 = f31;
t3 = hud + 0x0000274Cu;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
f1 = f31;
t3 = hud + 0x00002784u;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
f1 = f31;
t3 = hud + 0x000027BCu;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
f1 = f31;
t3 = hud + 0x000027F4u;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
f1 = f31;
t3 = hud + 0x0000290Cu;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BBAu, zero);
t3 = hud;
t3=gabi::call<u32>(0x025986DC ,t3);
t3 = hud;
t3=gabi::call<u32>(0x02599584 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259A5B0 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259ACD4 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259B698 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259C0EC ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259C3CC ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259C454 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259C7FC ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259CCA8 ,t3);
t3=gabi::call<u32>(0x0259188C ,t3,t4,t5,t6,t7,t8,t9,t10);
t3 = hud;
t3=gabi::call<u32>(0x0259CDC4 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259C2BC ,t3);
t10 = gmem_ld32(hud + 0x00003008u);
t0 = t10 & 0x00000002u; compares(0,(s32)t0,0);
if (conditions[2]) { goto L_025956CC; }
goto L_02595690;
L_025955BC: ;
f1 = f31;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
t3 = hud + 0x00001E54u;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = hud + 0x00001DE4u;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
f1 = f31;
t3 = t25;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
f1 = f31;
t3 = t26;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
t3 = hud + 0x0000274Cu;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = hud + 0x00002784u;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = hud + 0x000027BCu;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = hud + 0x000027F4u;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
f1 = f31;
t3 = hud + 0x0000290Cu;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BBAu, zero);
t3 = hud;
t3=gabi::call<u32>(0x025986DC ,t3);
t3 = hud;
t3=gabi::call<u32>(0x02599584 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259A5B0 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259ACD4 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259B698 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259C0EC ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259C3CC ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259C454 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259C7FC ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259CCA8 ,t3);
t3=gabi::call<u32>(0x0259188C ,t3,t4,t5,t6,t7,t8,t9,t10);
t3 = hud;
t3=gabi::call<u32>(0x0259CDC4 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x0259C2BC ,t3);
t10 = gmem_ld32(hud + 0x00003008u);
t0 = t10 & 0x00000002u; compares(0,(s32)t0,0);
if (conditions[2]) { goto L_025956CC; }
L_02595690: ;
t6 = 0x10200000u;
t6 = gmem_ld32(t6 + 0xFFFF8344u);
t3 = gmem_ld32(t6 + 0x0000017Cu);
t3=gabi::call<u32>(0x02693E50 ,t3);
compares(0,(int32_t)t3,0);
if (conditions[2]) { goto L_025956CC; }
t7 = gmem_ld8(hud + 0x00003028u);
t4 = 0x00000002u;
gmem_st8(hud + 0x00003039u, t4);
t7 = t7 & 0x0013u; compares(0,(s32)t7,0);
t0 = gmem_ld8(hud + 0x00003038u);
gmem_st8(hud + 0x00003028u, t7);
t0 = t0 & 0x0013u; compares(0,(s32)t0,0);
gmem_st8(hud + 0x00003038u, t0);
goto L_025956F8;
L_025956CC: ;
t8 = gmem_ld8(hud + 0x00003039u);
compares(0,(int32_t)t8,0);
if (conditions[2]) { goto L_025956F8; }
t7 = gmem_ld8(hud + 0x00003028u);
t5 = t8 + 0xFFFFFFFFu;
gmem_st8(hud + 0x00003039u, t5);
t7 = t7 & 0x0013u; compares(0,(s32)t7,0);
t0 = gmem_ld8(hud + 0x00003038u);
gmem_st8(hud + 0x00003028u, t7);
t0 = t0 & 0x0013u; compares(0,(s32)t0,0);
gmem_st8(hud + 0x00003038u, t0);
L_025956F8: ;
t26 = gmem_ld8(hud + 0x00003028u);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BCCu, t26);
t3 = hud;
t3=gabi::call<u32>(0x0259CEA0 ,t3);
t3=gabi::call<u32>(0x025986BC ,t3,t4,t5,t6,t7,t8,t9,t10);
compares(0,(int32_t)t3,0);
if (!conditions[2]) { goto L_025957D0; }
t3=gabi::call<u32>(0x025AF2A4 ,t3,t4,t5,t6,t7,t8,t9,t10);
compares(0,(int32_t)t3,0);
if (!conditions[2]) { goto L_025957D0; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = gmem_ld8(t3 + 0x00005292u);
compares(0,(int32_t)t12,0);
if (!conditions[2]) { goto L_025957D0; }
t4 = 0x00020000u;
t5 = gmem_ld32(hud + 0x00003008u);
t4 = t4 + 0x00000080u;
t0 = t4 & t5; compares(0,(s32)t0,0);
if (!conditions[2]) { goto L_02595754; }
t6 = gmem_ld32(saveSegmentBase + 0xFFFF84DCu);
t3 = t6 + 0x00000086u;
t3=gabi::call<u32>(0x025B614C ,t3);
L_02595754: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t7 = gmem_ld8(t3 + 0x00005BDFu);
compares(0,(int32_t)t7,1);
t9 = t3 + 0x000012A0u;
if (!conditions[2]) { goto L_025957D0; }
t10 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t9 + 0x0000490Au);
compares(0,(int32_t)t10,0);
if (!conditions[1]) { goto L_025957C8; }
t8 = t10 + 0xFFFFFFFFu;
t3 = previousHeap;
gmem_st16(t9 + 0x0000490Au, t8);
t3=gabi::call<u32>(0x025E3570 ,t3);
t9 = gmem_ld32(hud + 0x00003008u);
gmem_st32(t27 + 0x00000038u, t9);
t9 = gmem_ld8(hud + 0x00003038u);
gmem_st8(t27 + 0x00000041u, t9);
{ uint32_t ea = hud + 0x00002FC0u; { double v = gabi::load<float>(ea); f0 = v; } }
{ uint32_t ea = t27 + 0x00000018u; gabi::store<float>(ea, f0); }
{ uint32_t ea = hud + 0x00002FC4u; { double v = gabi::load<float>(ea); f7 = v; } }
{ uint32_t ea = t27 + 0x0000001Cu; gabi::store<float>(ea, f7); }
{ uint32_t ea = hud + 0x00003030u; { double v = gabi::load<float>(ea); f8 = v; } }
{ uint32_t ea = t27 + 0x00000020u; gabi::store<float>(ea, f8); }
{ uint32_t ea = hud + 0x00003034u; { double v = gabi::load<float>(ea); f12 = v; } }
{ uint32_t ea = t27 + 0x00000024u; gabi::store<float>(ea, f12); }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005CEAu);
t3 = 0x00000001u;
gmem_st16(t27 + 0x0000003Cu, t0);
goto L_02595818;
L_025957C8: ;
gmem_st8(t9 + 0x0000493Fu, zero);
gmem_st16(t9 + 0x0000490Au, zero);
L_025957D0: ;
t3 = previousHeap;
t3=gabi::call<u32>(0x025E3570 ,t3);
t9 = gmem_ld32(hud + 0x00003008u);
gmem_st32(t27 + 0x00000038u, t9);
t9 = gmem_ld8(hud + 0x00003038u);
gmem_st8(t27 + 0x00000041u, t9);
{ uint32_t ea = hud + 0x00002FC0u; { double v = gabi::load<float>(ea); f0 = v; } }
{ uint32_t ea = t27 + 0x00000018u; gabi::store<float>(ea, f0); }
{ uint32_t ea = hud + 0x00002FC4u; { double v = gabi::load<float>(ea); f7 = v; } }
{ uint32_t ea = t27 + 0x0000001Cu; gabi::store<float>(ea, f7); }
{ uint32_t ea = hud + 0x00003030u; { double v = gabi::load<float>(ea); f8 = v; } }
{ uint32_t ea = t27 + 0x00000020u; gabi::store<float>(ea, f8); }
{ uint32_t ea = hud + 0x00003034u; { double v = gabi::load<float>(ea); f12 = v; } }
{ uint32_t ea = t27 + 0x00000024u; gabi::store<float>(ea, f12); }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005CEAu);
t3 = 0x00000001u;
gmem_st16(t27 + 0x0000003Cu, t0);
L_02595818: ;
return;
}
VERIFY(0x02593B10,updateHudStatus);

// Builds HUD resources and initial pane/item state.
static s32 createHud(u32 self){
 WWHD_FUNC(0x02595C6C,s32,self);
 gabi::Local<u8[0x130]> scratch;
 u32 t0=0,t3=self,t4=0,t5=0,t6=0,t7=0,t8=0,t9=0,t10=0,t11=0,t12=0,t17=0,t18=0,t19=0,t20=0,t21=0,t22=0,t23=0,t24=0,t25=0,t26=0,t27=0,t28=0,t29=0,zero=0,hud=0;
 double f0=0,f1=0,f6=0,f7=0,f8=0,f9=0,f10=0,f11=0,f12=0,f13=0,f29=0,f30=0,f31=0;
 u32 target=0;u8 carry=0;bool conditions[32]={};
auto compares=[&](int n,s32 a,s32 b){conditions[4*n]=a<b;conditions[4*n+1]=a>b;conditions[4*n+2]=a==b;};
 auto compareu=[&](int n,u32 a,u32 b){conditions[4*n]=a<b;conditions[4*n+1]=a>b;conditions[4*n+2]=a==b;};
 auto comparef=[&](int n,double a,double b){conditions[4*n]=a<b;conditions[4*n+1]=a>b;conditions[4*n+2]=a==b;conditions[4*n+3]=std::isnan(a)||std::isnan(b);};
hud = t3;
t10 = 0x10050000u;
zero = 0x00000000u;
t8 = 0x101F0000u;
{ uint32_t ea = t10 + 0x000012B8u; { double v = gabi::load<float>(ea); f29 = v; } }
t6 = 0x101F0000u;
gmem_st8(t8 + 0xFFFFA071u, zero);
t12 = 0x101F0000u;
{ uint32_t ea = t6 + 0xFFFFA03Cu; gabi::store<float>(ea, f29); }
t10 = 0x101F0000u;
gmem_st8(t12 + 0xFFFFA06Eu, zero);
t5 = 0x101F0000u;
t11 = 0x101F0000u;
{ uint32_t ea = t10 + 0xFFFFA038u; gabi::store<float>(ea, f29); }
t10 = 0x101F0000u;
gmem_st8(t11 + 0xFFFFA066u, zero);
t12 = 0x101F0000u;
t9 = 0x101F0000u;
{ uint32_t ea = t5 + 0xFFFFA030u; gabi::store<float>(ea, f29); }
gmem_st8(t9 + 0xFFFFA06Du, zero);
t11 = 0x101F0000u;
gmem_st8(t10 + 0xFFFFA075u, zero);
t9 = 0x101F0000u;
gmem_st8(t11 + 0xFFFFA070u, zero);
t5 = 0x101F0000u;
t6 = 0x101F0000u;
gmem_st8(t9 + 0xFFFFA068u, zero);
gmem_st8(t6 + 0xFFFFA06Cu, zero);
t8 = 0x101F0000u;
gmem_st8(t5 + 0xFFFFA072u, zero);
t5 = 0x101F0000u;
gmem_st8(t12 + 0xFFFFA06Bu, zero);
t12 = 0x101F0000u;
gmem_st8(t5 + 0xFFFFA067u, zero);
t10 = 0x101F0000u;
{ uint32_t ea = t8 + 0xFFFFA034u; gabi::store<float>(ea, f29); }
t8 = 0x101F0000u;
gmem_st8(t12 + 0xFFFFA06Au, zero);
t12 = 0x101F0000u;
gmem_st8(t8 + 0xFFFFA076u, zero);
t3 = 0x00030000u;
gmem_st16(t10 + 0xFFFFA044u, zero);
t8 = 0x101F0000u;
gmem_st8(t12 + 0xFFFFA073u, zero);
t4 = 0x101F0000u;
gmem_st32(t8 + 0xFFFFA02Cu, zero);
t6 = 0x101F0000u;
gmem_st8(t4 + 0xFFFFA06Fu, zero);
t3 = t3 + 0xFFFFA819u;
gmem_st8(t6 + 0xFFFFA069u, zero);
t3=gabi::call<u32>(0x025DB8A4 ,t3);
compares(0,(int32_t)t3,0);
gmem_st32(hud + 0x00000100u, t3);
if (!conditions[2]) { goto L_02595D94; }
t3 = 0x10050000u;
t5 = 0x10050000u;
t4 = 0x00003894u;
t3 = t3 + 0x0000138Cu;
t5 = t5 + 0x000013B4u;
t3=gabi::call<u32>(0x0273AA24 ,t3,t4,t5);
t3 = gmem_ld32(hud + 0x00000100u);
L_02595D94: ;
t3=gabi::call<u32>(0x025E3570 ,t3);
t4 = gmem_ld32(hud + 0x00000100u);
t28 = t3;
t3 = 0x000000D4u;
t5 = 0x00000004u;
t3=gabi::call<u32>(0x0273B050 ,t3,t4,t5);
t29 = t3; compares(0,(s32)t29,0);
t11 = 0x10050000u;
t10 = 0x10050000u;
{ uint32_t ea = t11 + 0x00001328u; { double v = gabi::load<float>(ea); f31 = v; } }
{ uint32_t ea = t10 + 0x0000132Cu; { double v = gabi::load<float>(ea); f30 = v; } }
if (conditions[2]) { goto L_02595E08; }
{ uint32_t ea = scratch.a + 0x00000010u; gabi::store<float>(ea, f29); }
t3 = t29;
t4 = 0x00000000u;
{ uint32_t ea = scratch.a + 0x00000014u; gabi::store<float>(ea, f29); }
t5 = 0x00000001u;
t6 = 0x726F0000u;
{ uint32_t ea = scratch.a + 0x00000018u; gabi::store<float>(ea, f31); }
t7 = scratch.a + 0x00000010u;
t6 = t6 + 0x00006F74u;
{ uint32_t ea = scratch.a + 0x0000001Cu; gabi::store<float>(ea, f30); }
t3=gabi::call<u32>(0x027EE5DC ,t3,t4,t5,t6,t7);
t9 = 0x10050000u;
gmem_st8(t29 + 0x000000CCu, zero);
t8 = 0xFFFFFFFFu;
t9 = t9 + 0x000011A4u;
gmem_st32(t29 + 0x000000CDu, t8);
gmem_st32(t29 + 0x000000C8u, t9);
L_02595E08: ;
t23 = 0x10480000u;
compares(0,(int32_t)t29,0);
{ uint32_t ea = t23 + 0xFFFFB0A0u; gmem_st32(ea, t29); t23 = ea; }
if (!conditions[2]) { goto L_02595E30; }
t3 = 0x10050000u;
t5 = 0x10050000u;
t4 = 0x00003899u;
t3 = t3 + 0x0000138Cu;
t5 = t5 + 0x000013CCu;
t3=gabi::call<u32>(0x0273AA24 ,t3,t4,t5);
L_02595E30: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t7 = t3 + 0x000012A0u;
t3 = gmem_ld32(t23 + 0x00000000u);
t4 = 0x10050000u;
t5 = gmem_ld32(t7 + 0x000047A0u);
t4 = t4 + 0x0000134Cu;
t3=gabi::call<u32>(0x027F05BC ,t3,t4,t5);
t4 = gmem_ld32(hud + 0x00000100u);
t3 = 0x000000D4u;
t5 = 0x00000004u;
t3=gabi::call<u32>(0x0273B050 ,t3,t4,t5);
t29 = t3; compares(0,(s32)t29,0);
if (conditions[2]) { goto L_02595EA8; }
{ uint32_t ea = scratch.a + 0x00000020u; gabi::store<float>(ea, f29); }
t3 = t29;
t4 = 0x00000000u;
{ uint32_t ea = scratch.a + 0x00000024u; gabi::store<float>(ea, f29); }
t5 = 0x00000001u;
t6 = 0x726F0000u;
{ uint32_t ea = scratch.a + 0x00000028u; gabi::store<float>(ea, f31); }
t7 = scratch.a + 0x00000020u;
t6 = t6 + 0x00006F74u;
{ uint32_t ea = scratch.a + 0x0000002Cu; gabi::store<float>(ea, f30); }
t3=gabi::call<u32>(0x027EE5DC ,t3,t4,t5,t6,t7);
t10 = 0x10170000u;
gmem_st8(t29 + 0x000000CCu, zero);
t10 = t10 + 0xFFFFE084u;
t11 = 0xFFFFFFFFu;
gmem_st32(t29 + 0x000000C8u, t10);
gmem_st32(t29 + 0x000000CDu, t11);
L_02595EA8: ;
t24 = 0x10480000u;
compares(0,(int32_t)t29,0);
{ uint32_t ea = t24 + 0xFFFFB0A4u; gmem_st32(ea, t29); t24 = ea; }
if (!conditions[2]) { goto L_02595ED0; }
t3 = 0x10050000u;
t5 = 0x10050000u;
t4 = 0x0000389Du;
t3 = t3 + 0x0000138Cu;
t5 = t5 + 0x000013E0u;
t3=gabi::call<u32>(0x0273AA24 ,t3,t4,t5);
L_02595ED0: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t11 = t3 + 0x000012A0u;
t3 = gmem_ld32(t24 + 0x00000000u);
t4 = 0x10050000u;
t5 = gmem_ld32(t11 + 0x000047A0u);
t4 = t4 + 0x0000135Cu;
t3=gabi::call<u32>(0x027F05BC ,t3,t4,t5);
t4 = gmem_ld32(hud + 0x00000100u);
t3 = 0x000000D4u;
t5 = 0x00000004u;
t3=gabi::call<u32>(0x0273B050 ,t3,t4,t5);
t29 = t3; compares(0,(s32)t29,0);
if (conditions[2]) { goto L_02595F48; }
{ uint32_t ea = scratch.a + 0x00000030u; gabi::store<float>(ea, f29); }
t3 = t29;
t4 = 0x00000000u;
{ uint32_t ea = scratch.a + 0x00000034u; gabi::store<float>(ea, f29); }
t5 = 0x00000001u;
t6 = 0x726F0000u;
{ uint32_t ea = scratch.a + 0x00000038u; gabi::store<float>(ea, f31); }
t7 = scratch.a + 0x00000030u;
t6 = t6 + 0x00006F74u;
{ uint32_t ea = scratch.a + 0x0000003Cu; gabi::store<float>(ea, f30); }
t3=gabi::call<u32>(0x027EE5DC ,t3,t4,t5,t6,t7);
t12 = 0x10170000u;
gmem_st8(t29 + 0x000000CCu, zero);
t12 = t12 + 0xFFFFE084u;
t0 = 0xFFFFFFFFu;
gmem_st32(t29 + 0x000000C8u, t12);
gmem_st32(t29 + 0x000000CDu, t0);
L_02595F48: ;
t27 = 0x10480000u;
compares(0,(int32_t)t29,0);
{ uint32_t ea = t27 + 0xFFFFB0A8u; gmem_st32(ea, t29); t27 = ea; }
if (!conditions[2]) { goto L_02595F70; }
t3 = 0x10050000u;
t5 = 0x10050000u;
t4 = 0x000038A1u;
t3 = t3 + 0x0000138Cu;
t5 = t5 + 0x000013F4u;
t3=gabi::call<u32>(0x0273AA24 ,t3,t4,t5);
L_02595F70: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t4 = t3 + 0x000012A0u;
t5 = gmem_ld32(t4 + 0x000047A0u);
t4 = 0x10050000u;
t3 = gmem_ld32(t27 + 0x00000000u);
t4 = t4 + 0x0000136Cu;
t3=gabi::call<u32>(0x027F05BC ,t3,t4,t5);
t4 = gmem_ld32(hud + 0x00000100u);
t3 = 0x000000D4u;
t5 = 0x00000004u;
t3=gabi::call<u32>(0x0273B050 ,t3,t4,t5);
t29 = t3; compares(0,(s32)t29,0);
if (conditions[2]) { goto L_02595FE8; }
{ uint32_t ea = scratch.a + 0x00000040u; gabi::store<float>(ea, f29); }
t3 = t29;
t4 = 0x00000000u;
{ uint32_t ea = scratch.a + 0x00000044u; gabi::store<float>(ea, f29); }
t5 = 0x00000001u;
t6 = 0x726F0000u;
{ uint32_t ea = scratch.a + 0x00000048u; gabi::store<float>(ea, f31); }
t7 = scratch.a + 0x00000040u;
t6 = t6 + 0x00006F74u;
{ uint32_t ea = scratch.a + 0x0000004Cu; gabi::store<float>(ea, f30); }
t3=gabi::call<u32>(0x027EE5DC ,t3,t4,t5,t6,t7);
t4 = 0x10170000u;
gmem_st8(t29 + 0x000000CCu, zero);
t4 = t4 + 0xFFFFE084u;
t5 = 0xFFFFFFFFu;
gmem_st32(t29 + 0x000000C8u, t4);
gmem_st32(t29 + 0x000000CDu, t5);
L_02595FE8: ;
t9 = 0x101F0000u;
compares(0,(int32_t)t29,0);
gmem_st32(t9 + 0xFFFFA0FCu, t29);
if (!conditions[2]) { goto L_02596010; }
t3 = 0x10050000u;
t5 = 0x10050000u;
t4 = 0x000038A5u;
t3 = t3 + 0x0000138Cu;
t5 = t5 + 0x00001408u;
t3=gabi::call<u32>(0x0273AA24 ,t3,t4,t5);
L_02596010: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t5 = t3 + 0x000012A0u;
t3 = 0x101F0000u;
t5 = gmem_ld32(t5 + 0x000047A0u);
t4 = 0x10050000u;
t3 = gmem_ld32(t3 + 0xFFFFA0FCu);
t4 = t4 + 0x0000137Cu;
t3=gabi::call<u32>(0x027F05BC ,t3,t4,t5);
t22 = 0x00000003u;
t25 = 0x00000000u;
t29 = hud + 0x00002F4Cu;
t26 = hud + 0x00002F40u;
L_02596040: ;
t3 = gmem_ld32(hud + 0x00000100u);
t6 = gmem_ld32(t3 + 0x0000000Cu);
t7 = gmem_ld32(t6 + 0x00000034u);
target = t7;
t4 = 0x00000C00u;
t5 = 0x00000020u;
t3=gabi::call_ptr<u32>(target,t3,t4,t5);
compares(0,(int32_t)t3,0);
gmem_st32(t26 + t25, t3);
if (!conditions[2]) { goto L_02596080; }
t3 = 0x10050000u;
t5 = 0x10050000u;
t4 = 0x000038AAu;
t3 = t3 + 0x0000138Cu;
t5 = t5 + 0x0000141Cu;
t3=gabi::call<u32>(0x0273AA24 ,t3,t4,t5);
L_02596080: ;
t3 = gmem_ld32(hud + 0x00000100u);
t8 = gmem_ld32(t3 + 0x0000000Cu);
t9 = gmem_ld32(t8 + 0x00000034u);
target = t9;
t4 = 0x00000C00u;
t5 = 0x00000020u;
t3=gabi::call_ptr<u32>(target,t3,t4,t5);
compares(0,(int32_t)t3,0);
gmem_st32(t29 + t25, t3);
if (!conditions[2]) { goto L_025960C0; }
t3 = 0x10050000u;
t5 = 0x10050000u;
t4 = 0x000038ACu;
t3 = t3 + 0x0000138Cu;
t5 = t5 + 0x0000143Cu;
t3=gabi::call<u32>(0x0273AA24 ,t3,t4,t5);
L_025960C0: ;
{ uint64_t t = (uint64_t)t22 + 0xFFFFFFFFu; t22 = (uint32_t)t; carry = (uint8_t)(t >> 32); } compares(0,(s32)t22,0);
t25 = t25 + 0x00000004u;
if (!conditions[2]) { goto L_02596040; }
t29 = hud + 0x00002F38u;
t22 = 0x00000000u;
t26 = t29;
goto L_025960EC;
L_025960DC: ;
t25 = t26;
goto L_02596130;
L_025960E4: ;
compares(0,(int32_t)t22,0);
if (!conditions[2]) { goto L_02596130; }
L_025960EC: ;
t3 = gmem_ld32(hud + 0x00000100u);
t10 = gmem_ld32(t3 + 0x0000000Cu);
t11 = gmem_ld32(t10 + 0x00000034u);
target = t11;
t4 = 0x00000C00u;
t5 = 0x00000020u;
t3=gabi::call_ptr<u32>(target,t3,t4,t5);
compares(0,(int32_t)t3,0);
gmem_st32(hud + 0x00002F64u, t3);
if (!conditions[2]) { goto L_025960DC; }
t3 = 0x10050000u;
t5 = 0x10050000u;
t4 = 0x000038B3u;
t3 = t3 + 0x0000138Cu;
t5 = t5 + 0x0000145Cu;
t3=gabi::call<u32>(0x0273AA24 ,t3,t4,t5);
t25 = t26;
L_02596130: ;
t3 = gmem_ld32(hud + 0x00000100u);
t12 = gmem_ld32(t3 + 0x0000000Cu);
t0 = gmem_ld32(t12 + 0x00000034u);
target = t0;
t4 = 0x00000C00u;
t5 = 0x00000020u;
t3=gabi::call_ptr<u32>(target,t3,t4,t5);
compares(0,(int32_t)t3,0);
gmem_st32(t25 + 0x00000000u, t3);
if (!conditions[2]) { goto L_02596170; }
t3 = 0x10050000u;
t5 = 0x10050000u;
t4 = 0x000038B6u;
t3 = t3 + 0x0000138Cu;
t5 = t5 + 0x00001398u;
t3=gabi::call<u32>(0x0273AA24 ,t3,t4,t5);
L_02596170: ;
t22 = t22 + 0x00000001u;
t25 = t25 + 0x00000004u;
compares(0,(int32_t)t22,2);
t26 = t26 + 0x00000004u;
if (conditions[0]) { goto L_025960E4; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BBAu, zero);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BB8u, zero);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BB9u, zero);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t5 = 0x0000004Du;
t25 = 0x10480000u;
gmem_st8(t3 + 0x00005C2Fu, t5);
t3 = 0x10050000u;
t25 = t25 + 0xFFFFACECu;
t3 = t3 + 0x0000147Cu;
t4 = t25;
t3=gabi::call<u32>(0x025F0A10 ,t3,t4,t5,t6,t7,t8,t9,t10);
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = 0x77690000u;
gmem_st8(t25 + 0x00000000u, t3);
t3 = hud + 0x000002FCu;
t5 = t5 + 0x00007931u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x79720000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00000104u;
t5 = t5 + 0x00006967u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x00790000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x0000013Cu;
t5 = t5 + 0x00007570u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x796C0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00000174u;
t5 = t5 + 0x00006566u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00000254u;
t5 = 0x00000072u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x0000028Cu;
t5 = 0x0000006Cu;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x63720000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x000002C4u;
t5 = t5 + 0x00007331u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
{ uint32_t ea = hud + 0x00000154u; { double v = gabi::load<float>(ea); f10 = v; } }
t6 = 0x10050000u;
{ uint32_t ea = hud + 0x00000118u; { double v = gabi::load<float>(ea); f0 = v; } }
{ uint32_t ea = t6 + 0x00001330u; { double v = gabi::load<float>(ea); f11 = v; } }
t5 = 0x10050000u;
{ uint32_t ea = hud + 0x00000188u; { double v = gabi::load<float>(ea); f12 = v; } }
{ double v = to_single(f0 - f11); f0 = v; }
{ uint32_t ea = t5 + 0x000012B4u; { double v = gabi::load<float>(ea); f9 = v; } }
{ double v = to_single(f10 - f11); f10 = v; }
{ double v = to_single(f12 - f9); f12 = v; }
{ uint32_t ea = hud + 0x00000118u; gabi::store<float>(ea, f0); }
t3 = hud + 0x0000036Cu;
{ uint32_t ea = hud + 0x00000154u; gabi::store<float>(ea, f10); }
t5 = 0x62650000u;
{ uint32_t ea = hud + 0x00000188u; gabi::store<float>(ea, f12); }
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = t5 + 0x00007931u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x62650000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x000003A4u;
t5 = t5 + 0x00007932u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x656E0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x000003DCu;
t5 = t5 + 0x0000656Du;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x656E0000u;
t3 = hud + 0x00000414u;
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = t5 + 0x00006D32u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x62650000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x0000044Cu;
t5 = t5 + 0x00006E31u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x62650000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00000484u;
t5 = t5 + 0x00006E32u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x62650000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x000004BCu;
t5 = t5 + 0x00006E33u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x62650000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x000004F4u;
t5 = t5 + 0x00006E34u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x656E0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x0000052Cu;
t5 = t5 + 0x00006533u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x656E0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00000564u;
t5 = t5 + 0x00006532u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x656E0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x0000059Cu;
t5 = t5 + 0x00006531u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x656E0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x000005D4u;
t5 = t5 + 0x00006B32u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x656E0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x0000060Cu;
t5 = t5 + 0x0000656Bu;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t11 = 0x00000005u;
t6 = 0x10050000u;
t7 = scratch.a + 0x0000004Cu;
target = t11;
t6 = t6 + 0x00001210u;
L_02596394: ;
t12 = gmem_ld32(t6 + 0x00000004u);
gmem_st32(t7 + 0x00000004u, t12);
t0 = gmem_ld32(t6 + 0x00000008u);
gmem_st32(t7 + 0x00000008u, t0);
t5 = gmem_ld32(t6 + 0x0000000Cu);
gmem_st32(t7 + 0x0000000Cu, t5);
{ uint32_t ea = t6 + 0x00000010u; t0 = gmem_ld32(ea); t6 = ea; }
{ uint32_t ea = t7 + 0x00000010u; gmem_st32(ea, t0); t7 = ea; }
target--; if (target != 0) { goto L_02596394; }
t12 = 0x00000005u;
t11 = 0x10050000u;
t7 = scratch.a + 0x0000009Cu;
target = t12;
t11 = t11 + 0x00001260u;
L_025963CC: ;
t0 = gmem_ld32(t11 + 0x00000004u);
gmem_st32(t7 + 0x00000004u, t0);
t5 = gmem_ld32(t11 + 0x00000008u);
gmem_st32(t7 + 0x00000008u, t5);
t6 = gmem_ld32(t11 + 0x0000000Cu);
gmem_st32(t7 + 0x0000000Cu, t6);
{ uint32_t ea = t11 + 0x00000010u; t8 = gmem_ld32(ea); t11 = ea; }
{ uint32_t ea = t7 + 0x00000010u; gmem_st32(ea, t8); t7 = ea; }
target--; if (target != 0) { goto L_025963CC; }
t22 = hud + 0x00000AA4u;
t26 = hud + 0x00000644u;
t20 = scratch.a + 0x0000009Cu;
t21 = scratch.a + 0x0000004Cu;
t19 = 0x00000000u;
t18 = 0x00000014u;
L_02596408: ;
{ uint32_t ea = t21 + 0x00000004u; t5 = gmem_ld32(ea); t21 = ea; }
t3 = t26 + t19;
t4 = gmem_ld32(t23 + 0x00000000u);
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
{ uint32_t ea = t20 + 0x00000004u; t5 = gmem_ld32(ea); t20 = ea; }
t3 = t22 + t19;
t4 = gmem_ld32(t23 + 0x00000000u);
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
{ uint64_t t = (uint64_t)t18 + 0xFFFFFFFFu; t18 = (uint32_t)t; carry = (uint8_t)(t >> 32); } compares(0,(s32)t18,0);
t19 = t19 + 0x00000038u;
if (!conditions[2]) { goto L_02596408; }
t5 = 0x68740000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = t5 + 0x0000666Cu;
t3 = hud + 0x00000F04u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t26 = 0x6D620000u;
t3 = hud + 0x00000F3Cu;
t26 = t26 + 0x00006231u;
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = t26 + 0x00001000u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = t26 + 0x00001001u;
t3 = hud + 0x00000F74u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = t26 + 0x00001002u;
t3 = hud + 0x00000FACu;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = t26 + 0x00001003u;
t3 = hud + 0x00000FE4u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = t26 + 0x00001004u;
t3 = hud + 0x0000101Cu;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = t26 + 0x00001005u;
t3 = hud + 0x00001054u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = t26 + 0x00001006u;
t3 = hud + 0x0000108Cu;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = t26 + 0x00001007u;
t3 = hud + 0x000010C4u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = t26 + 0x00000002u;
t3 = hud + 0x000010FCu;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = t26 + 0x00000001u;
t3 = hud + 0x00001134u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x000011A4u;
t5 = t26;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = t26 + 0x00000901u;
t3 = hud + 0x0000116Cu;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = t26 + 0x00000900u;
t3 = hud + 0x000011DCu;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x636D0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00001214u;
t5 = t5 + 0x00006376u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x636D0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x0000124Cu;
t5 = t5 + 0x00006D67u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x636D0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00001284u;
t5 = t5 + 0x00006D6Bu;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x636D0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x000012BCu;
t5 = t5 + 0x00007267u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x636D0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x000012F4u;
t5 = t5 + 0x00006231u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x00620000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x0000139Cu;
t5 = t5 + 0x00006172u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x726E0000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x000013D4u;
t5 = t5 + 0x00006732u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x68720000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x0000140Cu;
t5 = t5 + 0x00007A31u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x68720000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x00001444u;
t5 = t5 + 0x00007A32u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x73740000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x0000147Cu;
t5 = t5 + 0x00007235u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x73740000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x000014B4u;
t5 = t5 + 0x00007236u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x73740000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t5 = t5 + 0x00007237u;
t3 = hud + 0x000014ECu;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x6D6F0000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x00001524u;
t5 = t5 + 0x00006E32u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x73750000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x0000155Cu;
t5 = t5 + 0x00006E32u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x63690000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t5 = t5 + 0x00007231u;
t3 = hud + 0x00001594u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x63690000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x000015CCu;
t5 = t5 + 0x00007232u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x63690000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x00001604u;
t5 = t5 + 0x00007233u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x63690000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x0000163Cu;
t5 = t5 + 0x00007234u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x636E0000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x00001674u;
t5 = t5 + 0x00007432u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x636E0000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x000016ACu;
t5 = t5 + 0x00007431u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x726E0000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x000016E4u;
t5 = t5 + 0x00006731u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x73740000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x0000171Cu;
t5 = t5 + 0x00007231u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x73740000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x00001754u;
t5 = t5 + 0x00007232u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x73740000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x0000178Cu;
t5 = t5 + 0x00007233u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x73740000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x000017C4u;
t5 = t5 + 0x00007234u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x6D6F0000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x000017FCu;
t5 = t5 + 0x00006E31u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x00630000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x00001834u;
t5 = t5 + 0x00006C70u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x636C0000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t5 = t5 + 0x00006431u;
t3 = hud + 0x0000186Cu;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x636C0000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x000018A4u;
t5 = t5 + 0x00006432u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x636C0000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x000018DCu;
t5 = t5 + 0x00006433u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x73750000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t5 = t5 + 0x00006E31u;
t3 = hud + 0x00001914u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x636D0000u;
t4 = gmem_ld32(t27 + 0x00000000u);
t3 = hud + 0x0000194Cu;
t5 = t5 + 0x0000746Du;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x6B790000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00001984u;
t5 = t5 + 0x00006C31u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x6B790000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x000019BCu;
t5 = t5 + 0x00006C32u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x6E6D0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00001AD4u;
t5 = t5 + 0x00003033u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x6E6D0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00001B0Cu;
t5 = t5 + 0x00003034u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x6E6D0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00001C24u;
t5 = t5 + 0x00003038u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x6E6D0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00001C5Cu;
t5 = t5 + 0x00003039u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x6B650000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00001C94u;
t5 = t5 + 0x00007930u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x62610000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00001CCCu;
t5 = t5 + 0x00007376u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x62610000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00001D04u;
t5 = t5 + 0x00007764u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x62610000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00001D3Cu;
t5 = t5 + 0x00007770u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x62610000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00001DE4u;
t5 = t5 + 0x00006132u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x62610000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = t5 + 0x00007432u;
t3 = hud + 0x00001E54u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x00790000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00002014u;
t5 = t5 + 0x00003031u;
t20 = hud + 0x00001FDCu;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x00790000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00001F6Cu;
t5 = t5 + 0x00003130u;
t21 = hud + 0x00001F34u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x79690000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x000020BCu;
t5 = t5 + 0x0000746Du;
t22 = hud + 0x00002084u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x79690000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00002164u;
t5 = t5 + 0x0000746Bu;
t26 = hud + 0x0000212Cu;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x626C0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x0000235Cu;
t5 = t5 + 0x00007931u;
t27 = hud + 0x00002324u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x626C0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00002404u;
t5 = t5 + 0x00007932u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x626C0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x000024ACu;
t5 = t5 + 0x00007933u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x00780000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = t20;
t5 = t5 + 0x00003031u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x00780000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = t21;
t5 = t5 + 0x00003130u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x78690000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = t22;
t5 = t5 + 0x0000746Du;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x78690000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = t26;
t5 = t5 + 0x0000746Bu;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x626C0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = t27;
t5 = t5 + 0x00007831u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x626C0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x000023CCu;
t5 = t5 + 0x00007832u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x626C0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00002474u;
t5 = t5 + 0x00007833u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x007A0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = t5 + 0x00003031u;
t3 = hud + 0x0000204Cu;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x007A0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00001FA4u;
t5 = t5 + 0x00003130u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x7A690000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x000020F4u;
t5 = t5 + 0x0000746Du;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x7A690000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = t5 + 0x0000746Bu;
t3 = hud + 0x0000219Cu;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x626C0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00002394u;
t5 = t5 + 0x00007A31u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x626C0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = t27 + 0x00000118u;
t5 = t5 + 0x00007A32u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x626C0000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = t5 + 0x00007A33u;
t3 = hud + 0x000024E4u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x00620000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x0000251Cu;
t5 = t5 + 0x0000617Au;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x00620000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x0000258Cu;
t5 = t5 + 0x00006179u;
t22 = hud + 0x00002554u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x62300000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x000025FCu;
t5 = t5 + 0x00003032u;
t26 = hud + 0x000025C4u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x62610000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x0000266Cu;
t5 = t5 + 0x00003032u;
t27 = hud + 0x00002634u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x62610000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = t22;
t5 = t5 + 0x00007831u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x62300000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = t5 + 0x00003031u;
t3 = t26;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x62610000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = t27;
t5 = t5 + 0x00003031u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x00620000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00001DACu;
t5 = t5 + 0x00006161u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x62610000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00001E1Cu;
t5 = t5 + 0x00007765u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x62610000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x000026A4u;
t5 = t5 + 0x00007231u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x62610000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x000026DCu;
t5 = t5 + 0x00003072u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x00610000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x0000274Cu;
t5 = t5 + 0x00003130u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x00610000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00002784u;
t5 = t5 + 0x00003031u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x00790000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x000027BCu;
t5 = t5 + 0x0000756Du;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x79750000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x000027F4u;
t5 = t5 + 0x00006D6Bu;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x77650000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x0000282Cu;
t5 = t5 + 0x00006974u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x77690000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x00002864u;
t5 = t5 + 0x0000746Bu;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x00620000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x0000289Cu;
t5 = t5 + 0x00006162u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x62610000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t5 = t5 + 0x00006174u;
t3 = hud + 0x000028D4u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x63650000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = hud + 0x0000290Cu;
t5 = t5 + 0x00006E74u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t27 = hud + 0x00002944u;
t5 = 0x61720000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = t27;
t5 = t5 + 0x00007731u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t26 = hud + 0x000029B4u;
t5 = 0x69700000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = t26;
t5 = t5 + 0x00003030u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x61720000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = t27 + 0x00000038u;
t5 = t5 + 0x00007732u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x69700000u;
t4 = gmem_ld32(t23 + 0x00000000u);
t3 = t26 + 0x00000038u;
t5 = t5 + 0x00003032u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x72750000u;
t4 = gmem_ld32(t24 + 0x00000000u);
t5 = t5 + 0x00007031u;
t3 = hud + 0x00002A24u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x72750000u;
t4 = gmem_ld32(t24 + 0x00000000u);
t3 = hud + 0x00002A5Cu;
t5 = t5 + 0x00007032u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x6E6D0000u;
t4 = gmem_ld32(t24 + 0x00000000u);
t3 = hud + 0x000019F4u;
t5 = t5 + 0x00003030u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x6E6D0000u;
t4 = gmem_ld32(t24 + 0x00000000u);
t3 = hud + 0x00001A2Cu;
t5 = t5 + 0x00003031u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x6E6D0000u;
t4 = gmem_ld32(t24 + 0x00000000u);
t3 = hud + 0x00001A64u;
t5 = t5 + 0x00003032u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x6E6D0000u;
t4 = gmem_ld32(t24 + 0x00000000u);
t3 = hud + 0x00001A9Cu;
t5 = t5 + 0x00003033u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x6E6D0000u;
t4 = gmem_ld32(t24 + 0x00000000u);
t3 = hud + 0x00001B44u;
t5 = t5 + 0x00003034u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x6E6D0000u;
t4 = gmem_ld32(t24 + 0x00000000u);
t3 = hud + 0x00001B7Cu;
t5 = t5 + 0x00003035u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x6E6D0000u;
t4 = gmem_ld32(t24 + 0x00000000u);
t3 = hud + 0x00001BB4u;
t5 = t5 + 0x00003036u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t5 = 0x6E6D0000u;
t4 = gmem_ld32(t24 + 0x00000000u);
t3 = hud + 0x00001BECu;
t5 = t5 + 0x00003037u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t24 = 0x10200000u;
t5 = gmem_ld32(t24 + 0xFFFF84DCu);
t0 = gmem_ld8(t5 + 0x00000032u);
t3 = hud;
gmem_st16(hud + 0x00001A62u, t0);
t3=gabi::call<u32>(0x025931CC ,t3);
t4 = 0x101F0000u;
t5 = 0x79610000u;
t4 = gmem_ld32(t4 + 0xFFFFA0FCu);
t5 = t5 + 0x00007A34u;
t3 = hud + 0x00002E14u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t4 = 0x101F0000u;
t5 = 0x79610000u;
t4 = gmem_ld32(t4 + 0xFFFFA0FCu);
t3 = hud + 0x00002E4Cu;
t5 = t5 + 0x00007A33u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t4 = 0x101F0000u;
t5 = 0x79610000u;
t4 = gmem_ld32(t4 + 0xFFFFA0FCu);
t5 = t5 + 0x00007A32u;
t3 = hud + 0x00002E84u;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t4 = 0x101F0000u;
t5 = 0x79610000u;
t4 = gmem_ld32(t4 + 0xFFFFA0FCu);
t5 = t5 + 0x00007A31u;
t3 = hud + 0x00002EBCu;
t3=gabi::call<u32>(0x025DB5C0 ,t3,t4,t5);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t11 = 0x10050000u;
t4 = 0x00000001u;
{ uint32_t ea = t11 + 0x000012B4u; { double v = gabi::load<float>(ea); f13 = v; } }
t10 = 0x10480000u;
gmem_st8(t3 + 0x00005C22u, t4);
t0 = 0x000000FFu;
{ uint32_t ea = hud + 0x00002F70u; gabi::store<float>(ea, f13); }
t12 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t10 + 0xFFFFAD18u);
gmem_st8(hud + 0x0000302Bu, t0);
gmem_st16(hud + 0x00002FDCu, t12);
t8 = gmem_ld32(t24 + 0xFFFF84DCu);
t3 = t8 + 0x00000798u;
t3=gabi::call<u32>(0x025B9100 ,t3,t4);
gmem_st16(hud + 0x0000013Au, t3);
t0 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t25 + 0x00000004u);
t3 = 0x10480000u;
t8 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t25 + 0x00000008u);
{ uint32_t ea = t3 + 0xFFFFAD0Cu; gmem_st8(ea, zero); t3 = ea; }
t5 = t8 - t0;
gmem_st16(t3 + 0x00000004u, t5);
t3=gabi::call<u32>(0x0259359C ,t3);
t3 = hud;
t3=gabi::call<u32>(0x02593434 ,t3);
t6 = 0x00000004u;
t7 = hud + 0x00002E12u;
target = t6;
t0 = 0x00000000u;
L_02596DEC: ;
{ uint32_t ea = t7 + 0x00000038u; gmem_st16(ea, t0); t7 = ea; }
target--; if (target != 0) { goto L_02596DEC; }
t3 = gmem_ld32(hud + 0x00000100u);
gmem_st8(hud + 0x00003029u, zero);
t3=gabi::call<u32>(0x025E3570 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x02591B4C ,t3);
gmem_st16(hud + 0x0000290Au, zero);
gmem_st8(hud + 0x0000301Eu, zero);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BB6u, zero);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t0,7);
if (conditions[2]) { goto L_02596E2C; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
L_02596E2C: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st16(hud + 0x00002862u, zero);
gmem_st16(hud + 0x00001E52u, zero);
gmem_st16(hud + 0x000027F2u, zero);
gmem_st8(hud + 0x0000301Fu, zero);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BB7u, zero);
gmem_st32(hud + 0x00002F30u, zero);
t9 = 0x10200000u;
gmem_st32(hud + 0x00002F34u, zero);
t5 = 0x10480000u;
t0 = 0x00000008u;
gmem_st16(hud + 0x0000258Au, zero);
t9 = gmem_ld32(t9 + 0xFFFF84DCu);
target = t0;
t5 = t5 + 0xFFFFA98Bu;
t4 = t9 + 0x000000C3u;
L_02596E70: ;
{ uint32_t ea = t4 + 0x00000001u; t0 = gmem_ld8(ea); t4 = ea; }
{ uint32_t ea = t5 + 0x00000001u; gmem_st8(ea, t0); t5 = ea; }
target--; if (target != 0) { goto L_02596E70; }
t25 = hud + 0x00003025u;
t24 = 0x000000FFu;
t23 = 0x10200000u;
t22 = 0x00000003u;
t21 = 0x00000000u;
L_02596E90: ;
t7 = gmem_ld32(t23 + 0xFFFF84DCu);
t6 = t7 + t21;
t20 = gmem_ld8(t6 + 0x00000029u);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
compares(0,(int32_t)t20,255);
t12 = t3 + 0x000012A0u;
if (conditions[2]) { goto L_02597070; }
t6 = gmem_ld32(t23 + 0xFFFF84DCu);
t8 = t6 + t21;
t9 = gmem_ld8(t8 + 0x00000029u);
compares(0,(int32_t)t9,21);
t10 = t6 + 0x00000020u;
if (!conditions[0]) { goto L_02596EF0; }
t9 = t10 + t9;
t8 = gmem_ld8(t9 + 0x0000003Cu);
t5 = t12 + t21;
gmem_st8(t5 + 0x0000491Bu, t8);
t6 = gmem_ld32(t23 + 0xFFFF84DCu);
t7 = t6 + t21;
t4 = gmem_ld8(t7 + 0x00000029u);
compares(0,(int32_t)t4,21);
t10 = t6 + 0x00000020u;
if (!conditions[0]) { goto L_02596FD8; }
goto L_02596FC4;
L_02596EF0: ;
compares(0,(int32_t)t9,24);
if (conditions[0]) { goto L_02596FA0; }
compares(0,(int32_t)t9,32);
if (!conditions[0]) { goto L_02596F2C; }
t10 = t10 + t9;
t8 = gmem_ld8(t10 + 0x0000005Eu);
t5 = t12 + t21;
gmem_st8(t5 + 0x0000491Bu, t8);
t6 = gmem_ld32(t23 + 0xFFFF84DCu);
t7 = t6 + t21;
t4 = gmem_ld8(t7 + 0x00000029u);
compares(0,(int32_t)t4,21);
t10 = t6 + 0x00000020u;
if (!conditions[0]) { goto L_02596FD8; }
goto L_02596FC4;
L_02596F2C: ;
compares(0,(int32_t)t9,36);
if (conditions[0]) { goto L_02596FA0; }
compares(0,(int32_t)t9,44);
if (!conditions[0]) { goto L_02596F68; }
t11 = t10 + t9;
t8 = gmem_ld8(t11 + 0x0000005Au);
t5 = t12 + t21;
gmem_st8(t5 + 0x0000491Bu, t8);
t6 = gmem_ld32(t23 + 0xFFFF84DCu);
t7 = t6 + t21;
t4 = gmem_ld8(t7 + 0x00000029u);
compares(0,(int32_t)t4,21);
t10 = t6 + 0x00000020u;
if (!conditions[0]) { goto L_02596FD8; }
goto L_02596FC4;
L_02596F68: ;
t0 = t9 + 0xFFFFFFD0u;
compareu(0,t0,0x0008u);
if (!conditions[0]) { goto L_02596FA0; }
t4 = t10 + t9;
t8 = gmem_ld8(t4 + 0x00000056u);
t5 = t12 + t21;
gmem_st8(t5 + 0x0000491Bu, t8);
t6 = gmem_ld32(t23 + 0xFFFF84DCu);
t7 = t6 + t21;
t4 = gmem_ld8(t7 + 0x00000029u);
compares(0,(int32_t)t4,21);
t10 = t6 + 0x00000020u;
if (!conditions[0]) { goto L_02596FD8; }
goto L_02596FC4;
L_02596FA0: ;
t5 = t12 + t21;
t8 = 0x000000FFu;
gmem_st8(t5 + 0x0000491Bu, t8);
t6 = gmem_ld32(t23 + 0xFFFF84DCu);
t7 = t6 + t21;
t4 = gmem_ld8(t7 + 0x00000029u);
compares(0,(int32_t)t4,21);
t10 = t6 + 0x00000020u;
if (!conditions[0]) { goto L_02596FD8; }
L_02596FC4: ;
t9 = t10 + t4;
t8 = gmem_ld8(t9 + 0x0000003Cu);
compares(0,(int32_t)t8,255);
if (conditions[2]) { goto L_0259703C; }
goto L_02597078;
L_02596FD8: ;
compares(0,(int32_t)t4,24);
if (conditions[0]) { goto L_0259703C; }
compares(0,(int32_t)t4,32);
if (!conditions[0]) { goto L_02596FFC; }
t11 = t10 + t4;
t8 = gmem_ld8(t11 + 0x0000005Eu);
compares(0,(int32_t)t8,255);
if (conditions[2]) { goto L_0259703C; }
goto L_02597078;
L_02596FFC: ;
compares(0,(int32_t)t4,36);
if (conditions[0]) { goto L_0259703C; }
compares(0,(int32_t)t4,44);
if (!conditions[0]) { goto L_02597020; }
t4 = t10 + t4;
t8 = gmem_ld8(t4 + 0x0000005Au);
compares(0,(int32_t)t8,255);
if (conditions[2]) { goto L_0259703C; }
goto L_02597078;
L_02597020: ;
t5 = t4 + 0xFFFFFFD0u;
compareu(0,t5,0x0008u);
if (!conditions[0]) { goto L_0259703C; }
t6 = t10 + t4;
t8 = gmem_ld8(t6 + 0x00000056u);
compares(0,(int32_t)t8,255);
if (!conditions[2]) { goto L_02597078; }
L_0259703C: ;
t7 = t10 + t21;
gmem_st8(t7 + 0x00000009u, t24);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t9 = t3 + t21;
t6 = gmem_ld8(t9 + 0x00005BBBu);
t4 = t21;
t3 = hud;
gmem_st8(t25 + t21, t6);
t3=gabi::call<u32>(0x02591F68 ,t3,t4);
{ uint64_t t = (uint64_t)t22 + 0xFFFFFFFFu; t22 = (uint32_t)t; carry = (uint8_t)(t >> 32); } compares(0,(s32)t22,0);
t21 = t21 + 0x00000001u;
if (!conditions[2]) { goto L_02596E90; }
goto L_025970A0;
L_02597070: ;
t8 = t12 + t21;
gmem_st8(t8 + 0x0000491Bu, t24);
L_02597078: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t9 = t3 + t21;
t6 = gmem_ld8(t9 + 0x00005BBBu);
t4 = t21;
t3 = hud;
gmem_st8(t25 + t21, t6);
t3=gabi::call<u32>(0x02591F68 ,t3,t4);
{ uint64_t t = (uint64_t)t22 + 0xFFFFFFFFu; t22 = (uint32_t)t; carry = (uint8_t)(t >> 32); } compares(0,(s32)t22,0);
t21 = t21 + 0x00000001u;
if (!conditions[2]) { goto L_02596E90; }
L_025970A0: ;
gmem_st16(hud + 0x00001D02u, zero);
gmem_st8(hud + 0x00003020u, zero);
gmem_st16(hud + 0x00002712u, zero);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BB5u, zero);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t10 = gmem_ld8(t3 + 0x00005BB5u);
compares(0,(int32_t)t10,7);
if (conditions[2]) { goto L_025970C8; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
L_025970C8: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st16(hud + 0x00000412u, zero);
t9 = 0x10050000u;
gmem_st16(hud + 0x0000059Au, zero);
{ uint32_t ea = t9 + 0x00001334u; { double v = gabi::load<float>(ea); f1 = v; } }
gmem_st16(hud + 0x000004BAu, zero);
f1=gabi::call<float>(0x020198D8 ,(float)f1);
t8 = 0x10050000u;
{ uint32_t ea = t8 + 0x00001338u; { double v = gabi::load<float>(ea); f0 = v; } }
{ double v = to_single(f1 + f0); f10 = v; }
gmem_st16(hud + 0x000003A2u, zero);
t7 = 0x10050000u;
f12 = u64_as_f64(ppc_fctiwz(f10));
{ uint32_t ea = t7 + 0x0000133Cu; { double v = gabi::load<float>(ea); f1 = v; } }
t0 = scratch.a + 0x00000008u;
gmem_st32(0u + t0, (uint32_t)f64_as_u64(f12));
t0 = (uint32_t)(int32_t)(int16_t)gmem_ld16(scratch.a + 0x0000000Au);
gmem_st16(hud + 0x0000044Au, t0);
f1=gabi::call<float>(0x020198D8 ,(float)f1);
t5 = 0x10050000u;
{ uint32_t ea = t5 + 0x000012BCu; { double v = gabi::load<float>(ea); f6 = v; } }
{ double v = to_single(f1 + f6); f6 = v; }
t12 = 0x10050000u;
f9 = u64_as_f64(ppc_fctiwz(f6));
{ uint32_t ea = t12 + 0x00001310u; { double v = gabi::load<float>(ea); f31 = v; } }
t4 = scratch.a + 0x00000008u;
gmem_st32(0u + t4, (uint32_t)f64_as_u64(f9));
t5 = gmem_ld32(scratch.a + 0x00000008u);
{ uint32_t ea = hud + 0x00000F60u; gabi::store<float>(ea, f31); }
t0 = rotl32(t5, 1) & 0xFFFFFFFEu;
gmem_st16(hud + 0x000003DAu, t0);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BEFu);
t9 = rotl32(t0, 31) & 0x00000001u; compares(0,(s32)t9,0);
if (conditions[2]) { goto L_025971B0; }
t6 = 0x10200000u;
t7 = gmem_ld32(t6 + 0xFFFF84DCu);
t0 = gmem_ld8(t7 + 0x00000033u);
t11 = 0x10050000u;
gmem_st8(hud + 0x0000301Cu, t0);
t6 = gmem_ld32(t6 + 0xFFFF84DCu);
t0 = 0x43300000u;
{ uint32_t ea = t11 + 0x00001340u; f12 = gabi::load<double>(ea); }
t12 = gmem_ld8(t6 + 0x00000034u);
gmem_st32(scratch.a + 0x00000008u, t0);
gmem_st32(scratch.a + 0x0000000Cu, t12);
{ uint32_t ea = scratch.a + 0x00000008u; f13 = gabi::load<double>(ea); }
f0 = f13 - f12;
{ double v = to_single(f0); f6 = v; }
{ uint32_t ea = hud + 0x00000F60u; { double v = gabi::load<float>(ea); f7 = v; } }
{ double v = to_single(f6 * round25(f7)); f6 = v; }
{ double v = to_single(f6 * round25(f31)); f8 = v; }
f9 = u64_as_f64(ppc_fctiwz(f8));
t0 = scratch.a + 0x00000008u;
gmem_st32(0u + t0, (uint32_t)f64_as_u64(f9));
t6 = gmem_ld16(scratch.a + 0x0000000Au);
gmem_st16(hud + 0x0000301Au, t6);
goto L_025971C8;
L_025971B0: ;
gmem_st8(hud + 0x0000301Cu, zero);
gmem_st16(hud + 0x0000301Au, zero);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BEFu);
t0 = t0 | 0x0002u;
gmem_st8(t3 + 0x00005BEFu, t0);
L_025971C8: ;
gmem_st16(hud + 0x00000F72u, zero);
t25 = 0x43300000u;
t0 = gmem_ld8(hud + 0x0000301Cu);
gmem_st32(scratch.a + 0x00000008u, t25);
gmem_st32(scratch.a + 0x0000000Cu, t0);
{ uint32_t ea = scratch.a + 0x00000008u; f8 = gabi::load<double>(ea); }
gmem_st16(hud + 0x00000FAAu, zero);
gmem_st16(hud + 0x00000FE2u, zero);
t23 = 0x10050000u;
gmem_st16(hud + 0x0000101Au, zero);
t7 = 0x10050000u;
{ uint32_t ea = t23 + 0x00001340u; f7 = gabi::load<double>(ea); }
t24 = 0x00000001u;
gmem_st16(hud + 0x00001052u, zero);
gmem_st16(hud + 0x0000108Au, t24);
f10 = f8 - f7;
gmem_st16(hud + 0x000011DAu, zero);
gmem_st16(hud + 0x00001132u, zero);
{ double v = to_single(f10); f11 = v; }
{ uint32_t ea = t7 + 0x000012C0u; { double v = gabi::load<float>(ea); f10 = v; } }
{ uint32_t ea = hud + 0x00002FC4u; gabi::store<float>(ea, f29); }
gmem_st16(hud + 0x0000116Au, zero);
{ double v = to_single(f11 * round25(f10)); f1 = v; }
gmem_st16(hud + 0x00001212u, zero);
t3 = hud;
gmem_st8(hud + 0x0000301Du, zero);
t3=gabi::call<u32>(0x02592888 ,t3,(float)f1);
t9 = gmem_ld16(hud + 0x0000301Au);
gmem_st32(scratch.a + 0x00000008u, t25);
gmem_st32(scratch.a + 0x0000000Cu, t9);
{ uint32_t ea = scratch.a + 0x00000008u; f7 = gabi::load<double>(ea); }
{ uint32_t ea = t23 + 0x00001340u; f8 = gabi::load<double>(ea); }
f9 = f7 - f8;
t11 = 0x10050000u;
{ double v = to_single(f9); f12 = v; }
{ uint32_t ea = t11 + 0x00001348u; { double v = gabi::load<float>(ea); f0 = v; } }
{ double v = to_single(f12 / f0); f1 = v; }
t3 = hud;
t3=gabi::call<u32>(0x02592A88 ,t3,(float)f1);
t3 = hud;
t3=gabi::call<u32>(0x0259295C ,t3);
t3 = hud + 0x000010FCu;
t3=gabi::call<u32>(0x025DB67C ,t3);
t3 = hud + 0x000011A4u;
t3=gabi::call<u32>(0x025DB67C ,t3);
t3 = hud + 0x000011DCu;
t3=gabi::call<u32>(0x025DB67C ,t3);
t10 = 0x0000000Au;
gmem_st16(hud + 0x0000028Au, t10);
gmem_st16(hud + 0x000002C2u, zero);
t25 = 0x10200000u;
gmem_st16(hud + 0x000002FAu, zero);
t10 = gmem_ld32(t25 + 0xFFFF84DCu);
t4 = t24;
t3 = t10 + 0x00000798u;
t3=gabi::call<u32>(0x025B9100 ,t3,t4);
gmem_st16(hud + 0x0000013Au, t3);
t7 = 0x101F0000u;
t4 = 0x101F0000u;
t11 = 0xFFFFFFFFu;
gmem_st8(t7 + 0xFFFFA074u, zero);
gmem_st32(t4 + 0xFFFFA040u, t11);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BD0u, zero);
gmem_st8(hud + 0x0000302Cu, zero);
gmem_st16(hud + 0x00002A5Au, zero);
gmem_st16(hud + 0x00001A2Au, t24);
t11 = gmem_ld32(t25 + 0xFFFF84DCu);
t25 = gmem_ld16(t11 + 0x00000024u);
gmem_st16(hud + 0x0000300Cu, t25);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st16(t3 + 0x00005BAEu, t25);
{ uint32_t ea = hud + 0x00003030u; gabi::store<float>(ea, f29); }
gmem_st16(hud + 0x00001CCAu, zero);
gmem_st16(hud + 0x00001B0Au, zero);
gmem_st16(hud + 0x000019F2u, t24);
t12 = 0x10200000u;
gmem_st16(hud + 0x000019BAu, t24);
t12 = gmem_ld32(t12 + 0xFFFF84DCu);
t5 = gmem_ld8(t12 + 0x000007B8u);
{ uint32_t ea = hud + 0x00003034u; gabi::store<float>(ea, f29); }
gmem_st8(hud + 0x00003021u, t5);
t3=gabi::call<u32>(0x02526C9C ,t3,t4,t5,t6,t7,t8,t9,t10);
compareu(0,t3,0x0003u);
if (!conditions[1]) { goto L_02597320; }
t3 = 0x00000000u;
L_02597320: ;
t22 = 0x54490000u;
t20 = 0x00000000u;
t6 = 0x101F0000u;
t0 = rotl32(t3, 2) & 0xFFFFFFFCu;
t21 = t20;
t6 = t6 + 0xFFFFA07Cu;
t24 = hud + 0x00002F28u;
t23 = t21;
t25 = t6 + t0;
t22 = t22 + 0x00004D47u;
t19 = 0x00000002u;
L_0259734C: ;
t17 = gmem_ld32(t29 + t21);
t18 = gmem_ld32(t25 + 0x00000000u);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = t3 + 0x000012A0u;
t3 = t17;
t4 = 0x00000C00u;
t7 = gmem_ld32(t12 + 0x000047E4u);
t6 = t18;
t5 = t22;
t3=gabi::call<u32>(0x027EAA2C ,t3,t4,t5,t6,t7);
t3 = gmem_ld32(t29 + t21);
t4 = 0x00000C00u;
gabi::call(0xC00088B8,t3,t4);
t3 = t27 + t20;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = t26 + t20;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = t27 + t20;
t3=gabi::call<u32>(0x025DB678 ,t3,t4,t5,t6,t7,t8,t9,t10);
t3 = t26 + t20;
t3=gabi::call<u32>(0x025DB678 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st32(t24 + t21, t23);
{ uint64_t t = (uint64_t)t19 + 0xFFFFFFFFu; t19 = (uint32_t)t; carry = (uint8_t)(t >> 32); } compares(0,(s32)t19,0);
t21 = t21 + 0x00000004u;
t20 = t20 + 0x00000038u;
if (!conditions[2]) { goto L_0259734C; }
t3=gabi::call<u32>(0x025C61E4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t3 = t28;
t3=gabi::call<u32>(0x025E3570 ,t3);
t3 = hud;
t3=gabi::call<u32>(0x025DB2BC ,t3);
gmem_st16(hud + 0x00003040u, zero);
t6 = 0xFFFFFFFFu;
gmem_st32(hud + 0x0000303Cu, t6);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t4 = 0x10200000u;
gmem_st8(t3 + 0x00005BECu, zero);
t4 = gmem_ld32(t4 + 0xFFFF84DCu);
t3 = t4 + 0x00000086u;
t3=gabi::call<u32>(0x025B61C8 ,t3);
compares(0,(int32_t)t3,0);
if (conditions[2]) { goto L_02597400; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t7 = 0x00000001u;
gmem_st8(t3 + 0x00005BE0u, t7);
L_02597400: ;
t4 = 0x10480000u;
t4 = t4 + 0xFFFFB03Cu;
gmem_st8(t4 + 0x00000062u, zero);
gmem_st8(t4 + 0x00000063u, zero);
gmem_st8(t4 + 0x0000003Eu, zero);
gmem_st8(t4 + 0x0000003Fu, zero);
gmem_st8(t4 + 0x00000040u, zero);
gmem_st8(t4 + 0x00000041u, zero);
gmem_st8(t4 + 0x00000042u, zero);
gmem_st8(t4 + 0x00000043u, zero);
gmem_st8(t4 + 0x00000044u, zero);
gmem_st8(t4 + 0x0000005Au, zero);
gmem_st8(t4 + 0x0000005Bu, zero);
gmem_st8(t4 + 0x0000005Cu, zero);
gmem_st8(t4 + 0x0000005Du, zero);
gmem_st8(t4 + 0x0000005Eu, zero);
{ uint32_t ea = t4 + 0x00000018u; gabi::store<float>(ea, f29); }
t7 = 0x00000000u;
gmem_st8(t4 + 0x0000005Fu, zero);
t0 = t7;
gmem_st8(t4 + 0x00000060u, zero);
t9 = 0x00000007u;
gmem_st32(t4 + 0x00000038u, zero);
t8 = t4 + 0x0000004Cu;
{ uint32_t ea = t4 + 0x0000001Cu; gabi::store<float>(ea, f29); }
{ uint32_t ea = t4 + 0x00000020u; gabi::store<float>(ea, f29); }
{ uint32_t ea = t4 + 0x00000024u; gabi::store<float>(ea, f29); }
{ uint32_t ea = t4 + 0x00000028u; gabi::store<float>(ea, f29); }
{ uint32_t ea = t4 + 0x0000002Cu; gabi::store<float>(ea, f29); }
{ uint32_t ea = t4 + 0x00000030u; gabi::store<float>(ea, f29); }
gmem_st16(t4 + 0x0000003Cu, zero);
t10 = t4 + 0x00000045u;
t12 = t4 + 0x00000053u;
{ uint32_t ea = t4 + 0x00000034u; gabi::store<float>(ea, f29); }
target = t9;
gmem_st8(t4 + 0x00000061u, zero);
L_02597490: ;
gmem_st8(t12 + t7, t0);
gmem_st8(t10 + t7, t0);
gmem_st8(t8 + t7, t0);
t7 = t7 + 0x00000001u;
target--; if (target != 0) { goto L_02597490; }
gmem_st8(hud + 0x00003039u, zero);
t3 = 0x00000004u;
return t3;
}
VERIFY(0x02595C6C,createHud);

// Drives the item-button transition counters and stage-specific visibility.
static void updateButtonCounter(u32 self){
 WWHD_FUNC(0x025986DC,void,self);
 gabi::Local<u8[0x58]> scratch;
 u32 t0=0,t3=self,t4=0,t5=0,t6=0,t7=0,t8=0,t9=0,t10=0,t11=0,t12=0,t24=0,t25=0,t26=0,t27=0,t28=0,hud=0,zero=0,t31=0;
 double f0=0,f1=0,f6=0,f7=0,f8=0,f9=0,f10=0,f11=0,f12=0,f13=0,f31=0;
 u32 target=0;u8 carry=0;bool conditions[32]={};
auto compares=[&](int n,s32 a,s32 b){conditions[4*n]=a<b;conditions[4*n+1]=a>b;conditions[4*n+2]=a==b;};
 auto compareu=[&](int n,u32 a,u32 b){conditions[4*n]=a<b;conditions[4*n+1]=a>b;conditions[4*n+2]=a==b;};
 auto comparef=[&](int n,double a,double b){conditions[4*n]=a<b;conditions[4*n+1]=a>b;conditions[4*n+2]=a==b;conditions[4*n+3]=std::isnan(a)||std::isnan(b);};
hud = t3;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t10 = gmem_ld8(t3 + 0x00005BB9u);
compares(0,(int32_t)t10,0);
if (conditions[2]) { goto L_0259871C; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t31 = gmem_ld8(t3 + 0x00005BB9u);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BB6u, t31);
L_0259871C: ;
t11 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x00002862u);
compares(0,(int32_t)t11,0);
zero = 0x00000000u;
if (conditions[2]) { goto L_025987D8; }
t12 = gmem_ld8(hud + 0x0000301Eu);
compares(0,(int32_t)t12,37);
if (!conditions[2]) { goto L_0259873C; }
gmem_st16(hud + 0x00001D3Au, zero);
L_0259873C: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t0,0);
if (conditions[2]) { goto L_025987D4; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t4 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t4,53);
if (conditions[2]) { goto L_025987D4; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t5 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t5,55);
if (conditions[2]) { goto L_025987D4; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t6 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t6,56);
if (conditions[2]) { goto L_025987D4; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t7 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t7,57);
if (conditions[2]) { goto L_025987D4; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t8 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t8,58);
if (conditions[2]) { goto L_025987D4; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t9 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t9,59);
if (conditions[2]) { goto L_025987D4; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t10 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t10,60);
if (conditions[2]) { goto L_025987D4; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t0,7);
if (conditions[2]) { goto L_025987D0; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
L_025987D0: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
L_025987D4: ;
gmem_st16(hud + 0x00002862u, zero);
L_025987D8: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t5 = gmem_ld8(hud + 0x0000301Eu);
t6 = gmem_ld8(t3 + 0x00005BB6u);
compareu(0,t5,t6);
t31 = hud + 0x000028D4u;
if (conditions[2]) { goto L_02598808; }
t7 = 0x00000001u;
gmem_st16(hud + 0x00002862u, t7);
gmem_st16(hud + 0x0000290Au, t7);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t9 = gmem_ld8(t3 + 0x00005BB6u);
gmem_st8(hud + 0x0000301Eu, t9);
L_02598808: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t0,37);
t10 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x00001D72u);
if (!conditions[2]) { goto L_025989B8; }
t4 = t10 + 0x00000001u;
t4 = (uint32_t)(int32_t)(int16_t)t4;
compares(0,(int32_t)t4,20);
if (conditions[0]) { goto L_025988B0; }
t4 = 0x00000000u;
t3 = 0x0000000Au;
t5 = t4;
gmem_st16(hud + 0x00001D72u, t4);
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
t4 = 0x10050000u;
t11 = gmem_ld8(hud + 0x00001D38u);
t5 = 0x43300000u;
t12 = 0x10050000u;
{ uint32_t ea = t4 + 0x00001340u; f0 = gabi::load<double>(ea); }
t7 = t11 + 0xFFFFFF88u;
gmem_st32(scratch.a + 0x00000030u, t5);
t8 = t7 ^ 0x80000000u;
{ uint32_t ea = t12 + 0x00001308u; f13 = gabi::load<double>(ea); }
gmem_st32(scratch.a + 0x00000034u, t11);
{ uint32_t ea = scratch.a + 0x00000030u; f9 = gabi::load<double>(ea); }
gmem_st32(scratch.a + 0x00000034u, t8);
{ uint32_t ea = scratch.a + 0x00000030u; f12 = gabi::load<double>(ea); }
f10 = f9 - f0;
f0 = f12 - f13;
{ double v = to_single(f10); f6 = v; }
{ double v = to_single(f0); f11 = v; }
{ double v = to_single(-(f11 * round25(f1) - f6)); f7 = v; }
f8 = u64_as_f64(ppc_fctiwz(f7));
t10 = scratch.a + 0x00000030u;
gmem_st32(0u + t10, (uint32_t)f64_as_u64(f8));
t9 = gmem_ld8(scratch.a + 0x00000033u);
gmem_st8(hud + 0x00001D71u, t9);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t12,49);
if (!conditions[2]) { goto L_025989DC; }
goto L_025989FC;
L_025988B0: ;
compares(0,(int32_t)t4,10);
gmem_st16(hud + 0x00001D72u, t4);
if (!conditions[0]) { goto L_02598938; }
t3 = 0x0000000Au;
t5 = 0x00000000u;
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
t4 = 0x10050000u;
t11 = gmem_ld8(hud + 0x00001D38u);
t5 = 0x43300000u;
t12 = 0x10050000u;
{ uint32_t ea = t4 + 0x00001340u; f0 = gabi::load<double>(ea); }
t7 = t11 + 0xFFFFFF88u;
gmem_st32(scratch.a + 0x00000030u, t5);
t8 = t7 ^ 0x80000000u;
{ uint32_t ea = t12 + 0x00001308u; f13 = gabi::load<double>(ea); }
gmem_st32(scratch.a + 0x00000034u, t11);
{ uint32_t ea = scratch.a + 0x00000030u; f9 = gabi::load<double>(ea); }
gmem_st32(scratch.a + 0x00000034u, t8);
{ uint32_t ea = scratch.a + 0x00000030u; f12 = gabi::load<double>(ea); }
f10 = f9 - f0;
f0 = f12 - f13;
{ double v = to_single(f10); f6 = v; }
{ double v = to_single(f0); f11 = v; }
{ double v = to_single(-(f11 * round25(f1) - f6)); f7 = v; }
f8 = u64_as_f64(ppc_fctiwz(f7));
t10 = scratch.a + 0x00000030u;
gmem_st32(0u + t10, (uint32_t)f64_as_u64(f8));
t9 = gmem_ld8(scratch.a + 0x00000033u);
gmem_st8(hud + 0x00001D71u, t9);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t12,49);
if (!conditions[2]) { goto L_025989DC; }
goto L_025989FC;
L_02598938: ;
{ uint64_t t = (uint64_t)(uint32_t)~t4 + 0x00000014u + 1; t4 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
t3 = 0x0000000Au;
t5 = 0x00000000u;
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
t4 = 0x10050000u;
t11 = gmem_ld8(hud + 0x00001D38u);
t5 = 0x43300000u;
t12 = 0x10050000u;
{ uint32_t ea = t4 + 0x00001340u; f0 = gabi::load<double>(ea); }
t7 = t11 + 0xFFFFFF88u;
gmem_st32(scratch.a + 0x00000030u, t5);
t8 = t7 ^ 0x80000000u;
{ uint32_t ea = t12 + 0x00001308u; f13 = gabi::load<double>(ea); }
gmem_st32(scratch.a + 0x00000034u, t11);
{ uint32_t ea = scratch.a + 0x00000030u; f9 = gabi::load<double>(ea); }
gmem_st32(scratch.a + 0x00000034u, t8);
{ uint32_t ea = scratch.a + 0x00000030u; f12 = gabi::load<double>(ea); }
f10 = f9 - f0;
f0 = f12 - f13;
{ double v = to_single(f10); f6 = v; }
{ double v = to_single(f0); f11 = v; }
{ double v = to_single(-(f11 * round25(f1) - f6)); f7 = v; }
f8 = u64_as_f64(ppc_fctiwz(f7));
t10 = scratch.a + 0x00000030u;
gmem_st32(0u + t10, (uint32_t)f64_as_u64(f8));
t9 = gmem_ld8(scratch.a + 0x00000033u);
gmem_st8(hud + 0x00001D71u, t9);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t12,49);
if (!conditions[2]) { goto L_025989DC; }
goto L_025989FC;
L_025989B8: ;
compares(0,(int32_t)t10,0);
if (conditions[2]) { goto L_025989CC; }
t11 = gmem_ld8(hud + 0x00001D38u);
gmem_st16(hud + 0x00001D72u, zero);
gmem_st8(hud + 0x00001D71u, t11);
L_025989CC: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t12 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t12,49);
if (conditions[2]) { goto L_025989FC; }
L_025989DC: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t0,45);
if (conditions[2]) { goto L_025989FC; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t4 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t4,46);
if (!conditions[2]) { goto L_02598AA4; }
L_025989FC: ;
t5 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x00001E8Au);
t4 = t5 + 0x00000001u;
t4 = (uint32_t)(int32_t)(int16_t)t4;
compares(0,(int32_t)t4,20);
if (!conditions[0]) { goto L_02598A3C; }
gmem_st16(hud + 0x00001E8Au, t4);
compares(0,(int32_t)t4,10);
t5 = 0x00000000u;
t3 = 0x0000000Au;
if (conditions[0]) { goto L_02598A90; }
L_02598A24: ;
{ uint64_t t = (uint64_t)(uint32_t)~t4 + 0x00000014u + 1; t4 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
t8 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t31 + 0x00000036u);
compares(0,(int32_t)t8,0);
if (conditions[2]) { goto L_02598AFC; }
goto L_02598AB4;
L_02598A3C: ;
gmem_st16(hud + 0x00001E8Au, zero);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t6 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t6,45);
if (!conditions[2]) { goto L_02598A60; }
t3 = 0x000008F8u;
t3=gabi::call<u32>(0x025E1988 ,t3);
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x00001E8Au);
goto L_02598A80;
L_02598A60: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t7 = gmem_ld8(t3 + 0x00005BB7u);
compares(0,(int32_t)t7,46);
t3 = 0x00000852u;
if (!conditions[2]) { goto L_02598A78; }
t3 = 0x0000090Au;
L_02598A78: ;
t3=gabi::call<u32>(0x025E1988 ,t3);
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(hud + 0x00001E8Au);
L_02598A80: ;
compares(0,(int32_t)t4,10);
t5 = 0x00000000u;
t3 = 0x0000000Au;
if (!conditions[0]) { goto L_02598A24; }
L_02598A90: ;
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
t8 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t31 + 0x00000036u);
compares(0,(int32_t)t8,0);
if (conditions[2]) { goto L_02598AFC; }
goto L_02598AB4;
L_02598AA4: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t8 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t31 + 0x00000036u);
compares(0,(int32_t)t8,0);
if (conditions[2]) { goto L_02598AFC; }
L_02598AB4: ;
compares(0,(int32_t)t8,12);
if (conditions[1]) { goto L_02598AFC; }
t8 = t8 + 0x00000001u;
t8 = (uint32_t)(int32_t)(int16_t)t8;
compares(0,(int32_t)t8,12);
if (conditions[1]) { goto L_02598AF8; }
gmem_st16(t31 + 0x00000036u, t8);
t27 = 0x10480000u;
t7 = gmem_ld32(hud + 0x00003008u);
t27 = t27 + 0xFFFFB03Cu;
t28 = 0x101F0000u;
t9 = gmem_ld8(t27 + 0x0000003Eu);
t28 = t28 + 0xFFFFA048u;
t8 = t7 & 0x00004000u; compares(0,(s32)t8,0);
gmem_st8(scratch.a + 0x00000008u, t9);
if (!conditions[2]) { goto L_02598CBC; }
goto L_02598B20;
L_02598AF8: ;
gmem_st16(t31 + 0x00000036u, zero);
L_02598AFC: ;
t27 = 0x10480000u;
t7 = gmem_ld32(hud + 0x00003008u);
t27 = t27 + 0xFFFFB03Cu;
t28 = 0x101F0000u;
t9 = gmem_ld8(t27 + 0x0000003Eu);
t28 = t28 + 0xFFFFA048u;
t8 = t7 & 0x00004000u; compares(0,(s32)t8,0);
gmem_st8(scratch.a + 0x00000008u, t9);
if (!conditions[2]) { goto L_02598CBC; }
L_02598B20: ;
t10 = t7 & 0x00000040u; compares(0,(s32)t10,0);
if (conditions[2]) { goto L_02598B68; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t11 = gmem_ld8(t3 + 0x00005292u);
{ uint64_t t = (uint64_t)t11 + 0xFFFFFFFFu; t12 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
{ uint64_t t = (uint64_t)(uint32_t)~t12 + t11 + carry; t0 = (uint32_t)t; carry = (uint8_t)(t >> 32); } compares(0,(s32)t0,0);
if (conditions[2]) { goto L_02598B4C; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld16(t3 + 0x000052A6u);
t4 = t0 & 0x00000004u; compares(0,(s32)t4,0);
if (!conditions[2]) { goto L_02598B5C; }
L_02598B4C: ;
t7 = gmem_ld32(hud + 0x00003008u);
t6 = t7 & 0x00000080u; compares(0,(s32)t6,0);
if (!conditions[2]) { goto L_02598B70; }
goto L_02598B84;
L_02598B5C: ;
t7 = gmem_ld32(hud + 0x00003008u);
t5 = t7 & 0x00000020u; compares(0,(s32)t5,0);
if (conditions[2]) { goto L_02598CBC; }
L_02598B68: ;
t6 = t7 & 0x00000080u; compares(0,(s32)t6,0);
if (conditions[2]) { goto L_02598B84; }
L_02598B70: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t7 = gmem_ld8(t3 + 0x00005BB3u);
compares(0,(int32_t)t7,11);
if (!conditions[2]) { goto L_02598CBC; }
t7 = gmem_ld32(hud + 0x00003008u);
L_02598B84: ;
t0 = t7 & 0x00000100u; compares(0,(s32)t0,0);
if (conditions[2]) { goto L_02598B9C; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t9 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t9,39);
if (!conditions[2]) { goto L_02598CBC; }
L_02598B9C: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t10 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t10,62);
if (conditions[2]) { goto L_02598CBC; }
t7 = gmem_ld32(hud + 0x00003008u);
t11 = t7 & 0x01080000u; compares(0,(s32)t11,0);
if (conditions[2]) { goto L_02598BDC; }
t12 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t28 + 0x00000000u);
compares(0,(int32_t)t12,11);
if (conditions[2]) { goto L_02598BD0; }
t3 = hud + 0x0000302Au;
t4 = 0x00000002u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02598BD0: ;
t7 = 0x0000000Bu;
gmem_st16(t28 + 0x00000000u, t7);
goto L_02598EBC;
L_02598BDC: ;
t0 = t7 & 0x00000020u; compares(0,(s32)t0,0);
if (conditions[2]) { goto L_02598D0C; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
{ uint32_t ea = t3 + 0x00005150u; t4 = gmem_ld32(ea); t3 = ea; }
t5 = gmem_ld32(t4 + 0x0000015Cu);
target = t5;
t3=gabi::call_ptr<u32>(target,t3);
t7 = gmem_ld16(t3 + 0x0000000Au);
t6 = t7 & 0x00000003u;
compares(0,(int32_t)t6,1);
if (conditions[2]) { goto L_02598CE8; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
{ uint32_t ea = t3 + 0x00005150u; t8 = gmem_ld32(ea); t3 = ea; }
t9 = gmem_ld32(t8 + 0x0000015Cu);
target = t9;
t3=gabi::call_ptr<u32>(target,t3);
t11 = gmem_ld16(t3 + 0x0000000Au);
t10 = t11 & 0x00000003u; compares(0,(s32)t10,0);
if (!conditions[2]) { goto L_02598EBC; }
t0 = gmem_ld8(t27 + 0x00000063u);
compares(0,(int32_t)t0,0);
if (conditions[2]) { goto L_02598C4C; }
compares(0,(int32_t)t0,1);
if (conditions[2]) { goto L_02598C4C; }
compares(0,(int32_t)t0,2);
if (conditions[2]) { goto L_02598C4C; }
compares(0,(int32_t)t0,3);
if (!conditions[2]) { goto L_02598C70; }
L_02598C4C: ;
t12 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t28 + 0x00000000u);
compares(0,(int32_t)t12,8);
if (conditions[2]) { goto L_02598C64; }
t3 = hud + 0x0000302Au;
t4 = 0x00000002u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02598C64: ;
t7 = 0x00000008u;
gmem_st16(t28 + 0x00000000u, t7);
goto L_02598EBC;
L_02598C70: ;
compares(0,(int32_t)t0,7);
if (conditions[2]) { goto L_02598C90; }
compares(0,(int32_t)t0,8);
if (conditions[2]) { goto L_02598C90; }
compares(0,(int32_t)t0,9);
if (conditions[2]) { goto L_02598C90; }
compares(0,(int32_t)t0,10);
if (!conditions[2]) { goto L_02598CB4; }
L_02598C90: ;
t0 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t28 + 0x00000000u);
compares(0,(int32_t)t0,10);
if (conditions[2]) { goto L_02598CA8; }
t3 = hud + 0x0000302Au;
t4 = 0x00000002u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02598CA8: ;
t7 = 0x0000000Au;
gmem_st16(t28 + 0x00000000u, t7);
goto L_02598EBC;
L_02598CB4: ;
compares(0,(int32_t)t0,6);
if (!conditions[2]) { goto L_02598CE8; }
L_02598CBC: ;
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t28 + 0x00000000u);
compares(0,(int32_t)t4,1);
if (conditions[2]) { goto L_02598CDC; }
t3 = hud + 0x0000302Au;
t4 = 0x00000002u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
t3 = scratch.a + 0x00000008u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02598CDC: ;
t7 = 0x00000001u;
gmem_st16(t28 + 0x00000000u, t7);
goto L_02598EBC;
L_02598CE8: ;
t5 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t28 + 0x00000000u);
compares(0,(int32_t)t5,9);
if (conditions[2]) { goto L_02598D00; }
t3 = hud + 0x0000302Au;
t4 = 0x00000002u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02598D00: ;
t7 = 0x00000009u;
gmem_st16(t28 + 0x00000000u, t7);
goto L_02598EBC;
L_02598D0C: ;
t8 = 0x00100000u;
t6 = t8 | 0x0080u;
t9 = t6 & t7; compares(0,(s32)t9,0);
if (!conditions[2]) { goto L_02598D2C; }
t10 = t7 & 0x00200000u; compares(0,(s32)t10,0);
if (conditions[2]) { goto L_02598D50; }
t11 = t7 & 0x00000010u; compares(0,(s32)t11,0);
if (!conditions[2]) { goto L_02598D50; }
L_02598D2C: ;
t12 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t28 + 0x00000000u);
compares(0,(int32_t)t12,2);
if (conditions[2]) { goto L_02598D44; }
t3 = hud + 0x0000302Au;
t4 = 0x00000002u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02598D44: ;
t7 = 0x00000002u;
gmem_st16(t28 + 0x00000000u, t7);
goto L_02598EBC;
L_02598D50: ;
t4 = t7 & 0x00020000u; compares(0,(s32)t4,0);
if (conditions[2]) { goto L_02598D7C; }
t5 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t28 + 0x00000000u);
compares(0,(int32_t)t5,3);
if (conditions[2]) { goto L_02598D70; }
t3 = hud + 0x0000302Au;
t4 = 0x00000002u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02598D70: ;
t7 = 0x00000003u;
gmem_st16(t28 + 0x00000000u, t7);
goto L_02598EBC;
L_02598D7C: ;
t8 = 0x101F0000u;
t6 = gmem_ld8(t8 + 0xFFFFA069u);
compares(0,(int32_t)t6,0);
if (conditions[2]) { goto L_02598E00; }
t11 = 0x101F0000u;
t11 = gmem_ld8(t11 + 0xFFFFA06Au);
compares(0,(int32_t)t11,1);
if (conditions[2]) { goto L_02598DB4; }
t12 = 0x101F0000u;
t12 = gmem_ld8(t12 + 0xFFFFA067u);
compares(0,(int32_t)t12,1);
if (!conditions[2]) { goto L_02598DE8; }
compares(0,(int32_t)t11,0);
if (!conditions[2]) { goto L_02598DE8; }
L_02598DB4: ;
t4 = 0x101F0000u;
t7 = gmem_ld8(t4 + 0xFFFFA072u);
compares(0,(int32_t)t7,0);
if (!conditions[2]) { goto L_02598E3C; }
t0 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t28 + 0x00000000u);
compares(0,(int32_t)t0,4);
if (conditions[2]) { goto L_02598DDC; }
t3 = hud + 0x0000302Au;
t4 = 0x00000002u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02598DDC: ;
t7 = 0x00000004u;
gmem_st16(t28 + 0x00000000u, t7);
goto L_02598EBC;
L_02598DE8: ;
compares(0,(int32_t)t11,2);
if (conditions[2]) { goto L_02598E3C; }
compares(0,(int32_t)t12,2);
if (!conditions[2]) { goto L_02598E00; }
compares(0,(int32_t)t11,0);
if (conditions[2]) { goto L_02598E3C; }
L_02598E00: ;
t9 = t7 & 0x00800000u; compares(0,(s32)t9,0);
if (!conditions[2]) { goto L_02598E3C; }
t10 = t7 & 0x00040000u; compares(0,(s32)t10,0);
if (conditions[2]) { goto L_02598E34; }
t11 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t28 + 0x00000000u);
compares(0,(int32_t)t11,6);
if (conditions[2]) { goto L_02598E28; }
t3 = hud + 0x0000302Au;
t4 = 0x00000002u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02598E28: ;
t7 = 0x00000006u;
gmem_st16(t28 + 0x00000000u, t7);
goto L_02598EBC;
L_02598E34: ;
t12 = t7 & 0x00400000u; compares(0,(s32)t12,0);
if (conditions[2]) { goto L_02598E60; }
L_02598E3C: ;
t0 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t28 + 0x00000000u);
compares(0,(int32_t)t0,5);
if (conditions[2]) { goto L_02598E54; }
t3 = hud + 0x0000302Au;
t4 = 0x00000002u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02598E54: ;
t7 = 0x00000005u;
gmem_st16(t28 + 0x00000000u, t7);
goto L_02598EBC;
L_02598E60: ;
t0 = t7 & 0x00000100u; compares(0,(s32)t0,0);
if (conditions[2]) { goto L_02598E9C; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t5 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t5,39);
if (!conditions[2]) { goto L_02598E9C; }
t6 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t28 + 0x00000000u);
compares(0,(int32_t)t6,7);
if (conditions[2]) { goto L_02598E90; }
t3 = hud + 0x0000302Au;
t4 = 0x00000002u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02598E90: ;
t7 = 0x00000007u;
gmem_st16(t28 + 0x00000000u, t7);
goto L_02598EBC;
L_02598E9C: ;
t7 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t28 + 0x00000000u);
compares(0,(int32_t)t7,0);
if (conditions[2]) { goto L_02598EB4; }
t3 = hud + 0x0000302Au;
t4 = 0x00000002u;
t3=gabi::call<u32>(0x0259173C ,t3,t4);
L_02598EB4: ;
t7 = 0x00000000u;
gmem_st16(t28 + 0x00000000u, t7);
L_02598EBC: ;
t8 = gmem_ld8(hud + 0x0000302Au);
compares(0,(int32_t)t8,15);
t6 = 0x101F0000u;
if (conditions[2]) { goto L_02598F48; }
t3 = hud + 0x0000302Au;
t4 = 0x00000002u;
t3=gabi::call<u32>(0x02591718 ,t3,t4);
compares(0,(int32_t)t3,0);
if (!conditions[2]) { goto L_02598F74; }
t9 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t28 + 0x00000000u);
compares(0,(int32_t)t9,1);
if (conditions[2]) { goto L_02598EF8; }
t3 = scratch.a + 0x00000008u;
t4 = 0x00000002u;
t3=gabi::call<u32>(0x0259172C ,t3,t4);
L_02598EF8: ;
t7 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t6 + 0xFFFFA046u);
compares(0,(int32_t)t7,5);
if (conditions[0]) { goto L_02598F2C; }
t10 = 0x00000005u;
t3 = hud + 0x0000302Au;
t4 = 0x00000002u;
gmem_st16(t6 + 0xFFFFA046u, t10);
t3=gabi::call<u32>(0x0259172C ,t3,t4);
t0 = gmem_ld8(t27 + 0x0000003Eu);
t12 = gmem_ld8(scratch.a + 0x00000008u);
compareu(0,t12,t0);
if (!conditions[2]) { goto L_02598F84; }
goto L_02598F88;
L_02598F2C: ;
t12 = gmem_ld8(scratch.a + 0x00000008u);
t11 = t7 + 0x00000001u;
t0 = gmem_ld8(t27 + 0x0000003Eu);
compareu(0,t12,t0);
gmem_st16(t6 + 0xFFFFA046u, t11);
if (!conditions[2]) { goto L_02598F84; }
goto L_02598F88;
L_02598F48: ;
t7 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t6 + 0xFFFFA046u);
compares(0,(int32_t)t7,0);
if (conditions[1]) { goto L_02598F6C; }
t12 = gmem_ld8(scratch.a + 0x00000008u);
t0 = gmem_ld8(t27 + 0x0000003Eu);
compareu(0,t12,t0);
gmem_st16(t6 + 0xFFFFA046u, zero);
if (!conditions[2]) { goto L_02598F84; }
goto L_02598F88;
L_02598F6C: ;
t12 = t7 + 0xFFFFFFFFu;
gmem_st16(t6 + 0xFFFFA046u, t12);
L_02598F74: ;
t12 = gmem_ld8(scratch.a + 0x00000008u);
t0 = gmem_ld8(t27 + 0x0000003Eu);
compareu(0,t12,t0);
if (conditions[2]) { goto L_02598F88; }
L_02598F84: ;
gmem_st8(t27 + 0x0000003Eu, t12);
L_02598F88: ;
t7 = gmem_ld32(hud + 0x00003008u);
t0 = 0x00000000u;
t28 = 0x00000001u;
t27 = 0x00040000u;
t4 = t7 & 0x00000400u; compares(0,(s32)t4,0);
t27 = t27 + 0x00000001u;
if (conditions[2]) { goto L_02598FB8; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t5 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t5,53);
if (conditions[2]) { goto L_0259916C; }
t7 = gmem_ld32(hud + 0x00003008u);
L_02598FB8: ;
t6 = t7 & 0x00002000u; compares(0,(s32)t6,0);
if (conditions[2]) { goto L_02598FD4; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t8 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t8,2);
if (!conditions[2]) { goto L_0259916C; }
t7 = gmem_ld32(hud + 0x00003008u);
L_02598FD4: ;
t9 = t7 & 0x00000002u; compares(0,(s32)t9,0);
if (conditions[2]) { goto L_02599158; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t10 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t10,53);
if (!conditions[2]) { goto L_02599104; }
t25 = 0x10050000u;
t12 = 0x10050000u;
t25 = t25 + 0x0000107Cu;
t12 = t12 + 0x0000103Cu;
gmem_st32(scratch.a + 0x00000010u, t25);
t0 = 0x00000001u;
gmem_st32(scratch.a + 0x0000000Cu, t12);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t6 = gmem_ld32(scratch.a + 0x00000010u);
t5 = t3 + 0x00005134u;
gmem_st32(scratch.a + 0x0000003Cu, t25);
gmem_st32(scratch.a + 0x00000038u, t5);
t7 = gmem_ld32(t6 + 0x00000014u);
target = t7;
t3 = scratch.a + 0x0000000Cu;
t3=gabi::call_ptr<u32>(target,t3);
t10 = gmem_ld32(scratch.a + 0x00000010u);
t0 = gmem_ld32(t10 + 0x00000014u);
target = t0;
t3 = scratch.a + 0x0000000Cu;
t3=gabi::call_ptr<u32>(target,t3);
t5 = gmem_ld32(scratch.a + 0x0000003Cu);
t7 = gmem_ld32(t5 + 0x00000014u);
target = t7;
t26 = gmem_ld32(scratch.a + 0x0000000Cu);
t3 = scratch.a + 0x00000038u;
t3=gabi::call_ptr<u32>(target,t3);
t9 = gmem_ld32(scratch.a + 0x00000038u);
compareu(0,t26,t9);
if (conditions[2]) { goto L_02599104; }
t6 = gmem_ld32(scratch.a + 0x00000038u);
target = t27;
t12 = gmem_ld32(scratch.a + 0x0000000Cu);
t6 = t6 + 0xFFFFFFFFu;
L_02599074: ;
t4 = gmem_ld8(t12 + 0x00000000u);
{ uint32_t ea = t6 + 0x00000001u; t0 = gmem_ld8(ea); t6 = ea; }
compareu(0,t4,t0);
if (!conditions[2]) { goto L_02599094; }
compares(0,(int32_t)t4,0);
if (conditions[2]) { goto L_02599104; }
t12 = t12 + 0x00000001u;
target--; if (target != 0) { goto L_02599074; }
L_02599094: ;
t5 = 0x10050000u;
gmem_st32(scratch.a + 0x00000018u, t25);
t5 = t5 + 0x00001044u;
t0 = 0x00000001u;
gmem_st32(scratch.a + 0x00000014u, t5);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t10 = gmem_ld32(scratch.a + 0x00000018u);
t8 = t3 + 0x00005134u;
gmem_st32(scratch.a + 0x00000044u, t25);
gmem_st32(scratch.a + 0x00000040u, t8);
t0 = gmem_ld32(t10 + 0x00000014u);
target = t0;
t3 = scratch.a + 0x00000014u;
t3=gabi::call_ptr<u32>(target,t3);
t12 = gmem_ld32(scratch.a + 0x00000018u);
t4 = gmem_ld32(t12 + 0x00000014u);
target = t4;
t3 = scratch.a + 0x00000014u;
t3=gabi::call_ptr<u32>(target,t3);
t5 = gmem_ld32(scratch.a + 0x00000044u);
t6 = gmem_ld32(t5 + 0x00000014u);
target = t6;
t26 = gmem_ld32(scratch.a + 0x00000014u);
t3 = scratch.a + 0x00000040u;
t3=gabi::call_ptr<u32>(target,t3);
t8 = gmem_ld32(scratch.a + 0x00000040u);
compareu(0,t26,t8);
if (!conditions[2]) { goto L_02599114; }
L_02599104: ;
t7 = gmem_ld32(hud + 0x00003008u);
t0 = t7 & 0x00008000u; compares(0,(s32)t0,0);
if (!conditions[2]) { goto L_0259916C; }
goto L_02599160;
L_02599114: ;
t10 = gmem_ld32(scratch.a + 0x00000040u);
target = t27;
t8 = gmem_ld32(scratch.a + 0x00000014u);
t10 = t10 + 0xFFFFFFFFu;
L_02599124: ;
t6 = gmem_ld8(t8 + 0x00000000u);
{ uint32_t ea = t10 + 0x00000001u; t9 = gmem_ld8(ea); t10 = ea; }
compareu(0,t6,t9);
if (!conditions[2]) { goto L_0259916C; }
compares(0,(int32_t)t6,0);
if (conditions[2]) { goto L_02599104; }
t8 = t8 + 0x00000001u;
target--; if (target != 0) { goto L_02599124; }
compares(0,(int32_t)t28,0);
t28 = 0x101F0000u;
t28 = t28 + 0xFFFFA04Au;
if (!conditions[2]) { goto L_0259917C; }
goto L_02599218;
L_02599158: ;
t0 = t7 & 0x00008000u; compares(0,(s32)t0,0);
if (!conditions[2]) { goto L_0259916C; }
L_02599160: ;
t4 = t7 & 0x00010000u; compares(0,(s32)t4,0);
if (!conditions[2]) { goto L_0259916C; }
t28 = 0x00000000u;
L_0259916C: ;
compares(0,(int32_t)t28,0);
t28 = 0x101F0000u;
t28 = t28 + 0xFFFFA04Au;
if (conditions[2]) { goto L_02599218; }
L_0259917C: ;
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t28 + 0x00000000u);
compares(0,(int32_t)t4,3);
if (conditions[0]) { goto L_025991BC; }
if (!conditions[1]) { goto L_025991EC; }
t4 = t4 + 0xFFFFFFFFu;
t3 = 0x00000005u;
t4 = (uint32_t)(int32_t)(int16_t)t4;
t5 = 0x00000000u;
gmem_st16(t28 + 0x00000000u, t4);
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
f31 = f1;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t0,0);
if (conditions[2]) { goto L_02599420; }
goto L_0259945C;
L_025991BC: ;
t4 = t4 + 0x00000001u;
t3 = 0x00000005u;
t4 = (uint32_t)(int32_t)(int16_t)t4;
t5 = 0x00000000u;
gmem_st16(t28 + 0x00000000u, t4);
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
f31 = f1;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t0,0);
if (conditions[2]) { goto L_02599420; }
goto L_0259945C;
L_025991EC: ;
t4 = 0x00000003u;
t3 = 0x00000005u;
t5 = 0x00000000u;
gmem_st16(t28 + 0x00000000u, t4);
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
f31 = f1;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t0,0);
if (conditions[2]) { goto L_02599420; }
goto L_0259945C;
L_02599218: ;
t5 = gmem_ld32(hud + 0x00003008u);
t10 = 0x00000000u;
t0 = t5 & 0x00000002u; compares(0,(s32)t0,0);
t26 = t10;
if (conditions[2]) { goto L_025993A8; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t8 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t8,53);
if (conditions[2]) { goto L_0259925C; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t10 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t10,0);
if (conditions[2]) { goto L_0259925C; }
t12 = 0x101F0000u;
t11 = gmem_ld8(t12 + 0xFFFFA069u);
compares(0,(int32_t)t11,0);
if (conditions[2]) { goto L_025993A8; }
L_0259925C: ;
t25 = 0x10050000u;
t0 = 0x10050000u;
t25 = t25 + 0x0000107Cu;
{ uint64_t t = (uint64_t)t0 + 0x0000103Cu; t0 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
gmem_st32(scratch.a + 0x00000020u, t25);
t10 = 0x00000001u;
gmem_st32(scratch.a + 0x0000001Cu, t0);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t4 = gmem_ld32(scratch.a + 0x00000020u);
t0 = t3 + 0x00005134u;
gmem_st32(scratch.a + 0x0000004Cu, t25);
gmem_st32(scratch.a + 0x00000048u, t0);
t5 = gmem_ld32(t4 + 0x00000014u);
target = t5;
t3 = scratch.a + 0x0000001Cu;
t3=gabi::call_ptr<u32>(target,t3);
t7 = gmem_ld32(scratch.a + 0x00000020u);
t0 = gmem_ld32(t7 + 0x00000014u);
target = t0;
t3 = scratch.a + 0x0000001Cu;
t3=gabi::call_ptr<u32>(target,t3);
t9 = gmem_ld32(scratch.a + 0x0000004Cu);
t10 = gmem_ld32(t9 + 0x00000014u);
target = t10;
t24 = gmem_ld32(scratch.a + 0x0000001Cu);
t3 = scratch.a + 0x00000048u;
t3=gabi::call_ptr<u32>(target,t3);
t11 = gmem_ld32(scratch.a + 0x00000048u);
compareu(0,t24,t11);
if (conditions[2]) { goto L_025993A8; }
t6 = gmem_ld32(scratch.a + 0x00000048u);
target = t27;
t4 = gmem_ld32(scratch.a + 0x0000001Cu);
t6 = t6 + 0xFFFFFFFFu;
L_025992E4: ;
t7 = gmem_ld8(t4 + 0x00000000u);
{ uint32_t ea = t6 + 0x00000001u; t0 = gmem_ld8(ea); t6 = ea; }
compareu(0,t7,t0);
if (!conditions[2]) { goto L_02599304; }
compares(0,(int32_t)t7,0);
if (conditions[2]) { goto L_025993A8; }
t4 = t4 + 0x00000001u;
target--; if (target != 0) { goto L_025992E4; }
L_02599304: ;
t0 = 0x10050000u;
gmem_st32(scratch.a + 0x00000028u, t25);
{ uint64_t t = (uint64_t)t0 + 0x00001044u; t0 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
t10 = 0x00000001u;
gmem_st32(scratch.a + 0x00000024u, t0);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t5 = gmem_ld32(scratch.a + 0x00000028u);
t0 = t3 + 0x00005134u;
gmem_st32(scratch.a + 0x00000054u, t25);
gmem_st32(scratch.a + 0x00000050u, t0);
t6 = gmem_ld32(t5 + 0x00000014u);
target = t6;
t3 = scratch.a + 0x00000024u;
t3=gabi::call_ptr<u32>(target,t3);
t7 = gmem_ld32(scratch.a + 0x00000028u);
t8 = gmem_ld32(t7 + 0x00000014u);
target = t8;
t3 = scratch.a + 0x00000024u;
t3=gabi::call_ptr<u32>(target,t3);
t9 = gmem_ld32(scratch.a + 0x00000054u);
t10 = gmem_ld32(t9 + 0x00000014u);
target = t10;
t25 = gmem_ld32(scratch.a + 0x00000024u);
t3 = scratch.a + 0x00000050u;
t3=gabi::call_ptr<u32>(target,t3);
t11 = gmem_ld32(scratch.a + 0x00000050u);
compareu(0,t25,t11);
if (conditions[2]) { goto L_025993A8; }
t11 = gmem_ld32(scratch.a + 0x00000050u);
target = t27;
t8 = gmem_ld32(scratch.a + 0x00000024u);
t11 = t11 + 0xFFFFFFFFu;
L_02599384: ;
t9 = gmem_ld8(t8 + 0x00000000u);
{ uint32_t ea = t11 + 0x00000001u; t12 = gmem_ld8(ea); t11 = ea; }
compareu(0,t9,t12);
if (!conditions[2]) { goto L_025993A4; }
compares(0,(int32_t)t9,0);
if (conditions[2]) { goto L_025993A8; }
t8 = t8 + 0x00000001u;
target--; if (target != 0) { goto L_02599384; }
L_025993A4: ;
t26 = 0x00000001u;
L_025993A8: ;
t0 = t26 ? (uint32_t)__builtin_clz(t26) : 32;
t0 = rotl32(t0, 27) & 0x07FFFFFFu; compares(0,(s32)t0,0);
if (conditions[2]) { goto L_025993C0; }
t4 = gmem_ld8(hud + 0x00003028u);
t4 = t4 | 0x0002u;
gmem_st8(hud + 0x00003028u, t4);
L_025993C0: ;
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t28 + 0x00000000u);
compares(0,(int32_t)t4,5);
if (!conditions[0]) { goto L_025993E8; }
t5 = t4 + 0x00000001u;
gmem_st16(t28 + 0x00000000u, t5);
t6 = gmem_ld8(hud + 0x00003038u);
t6 = t6 | 0x0002u;
gmem_st8(hud + 0x00003038u, t6);
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t28 + 0x00000000u);
goto L_02599400;
L_025993E8: ;
t5 = 0x00000005u;
gmem_st16(t28 + 0x00000000u, t5);
t6 = gmem_ld8(hud + 0x00003038u);
t6 = t6 | 0x0002u;
gmem_st8(hud + 0x00003038u, t6);
t4 = (uint32_t)(int32_t)(int16_t)gmem_ld16(t28 + 0x00000000u);
L_02599400: ;
t3 = 0x00000005u;
t5 = 0x00000000u;
f1=gabi::call<float>(0x025DB6F4 ,t3,t4,t5);
f31 = f1;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t0,0);
if (!conditions[2]) { goto L_0259945C; }
L_02599420: ;
t3 = hud + 0x00001D04u;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = hud + 0x0000282Cu;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = hud + 0x00002864u;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
f1 = f31;
t3 = hud + 0x0000289Cu;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
f1 = f31;
t3 = t31;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BB9u, zero);
goto L_02599564;
L_0259945C: ;
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t0,53);
if (conditions[2]) { goto L_025994CC; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t0 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t0,55);
if (conditions[2]) { goto L_025994CC; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t4 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t4,56);
if (conditions[2]) { goto L_025994CC; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t5 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t5,57);
if (conditions[2]) { goto L_025994CC; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t6 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t6,58);
if (conditions[2]) { goto L_025994CC; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t7 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t7,59);
if (conditions[2]) { goto L_025994CC; }
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
t8 = gmem_ld8(t3 + 0x00005BB6u);
compares(0,(int32_t)t8,60);
if (!conditions[2]) { goto L_0259951C; }
L_025994CC: ;
t3 = hud + 0x00001D04u;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
f1 = f31;
t3 = hud + 0x0000282Cu;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
f1 = f31;
t3 = hud + 0x00002864u;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
f1 = f31;
t3 = hud + 0x0000289Cu;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
f1 = f31;
t3 = t31;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
f1 = f31;
t3 = hud + 0x0000290Cu;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BB9u, zero);
goto L_02599564;
L_0259951C: ;
f1 = f31;
t3 = hud + 0x00001D04u;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
t3 = hud + 0x0000282Cu;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
t3 = hud + 0x00002864u;
t3=gabi::call<u32>(0x025DB6E0 ,t3);
f1 = f31;
t3 = hud + 0x0000289Cu;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
f1 = f31;
t3 = t31;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
f1 = f31;
t3 = hud + 0x0000290Cu;
t3=gabi::call<u32>(0x025DB690 ,t3,(float)f1);
t3=gabi::call<u32>(0x025200D4 ,t3,t4,t5,t6,t7,t8,t9,t10);
gmem_st8(t3 + 0x00005BB9u, zero);
L_02599564: ;
return;
}
VERIFY(0x025986DC,updateButtonCounter);
}
