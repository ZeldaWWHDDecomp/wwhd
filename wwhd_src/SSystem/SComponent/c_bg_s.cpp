#include "gabi.h"
using namespace gabi;
namespace c_bg_s {
void assert_at(u32 text, u32 line, u32 condition) { call<void>(0x0273AA24, at<void>(text), line, at<void>(condition)); }
u32 entry(void *self, u32 id) { return ea(self) + id * 20u; }
void elm_init(void *self) {
  WWHD_FUNC(0x02007F60, void, self);
  u32 a=ea(self); store<u32>(a+12,0);store<u32>(a+8,0xFFFFFFFF);store<u32>(a+4,0);store<u32>(a,0);
}
VERIFY(0x02007F60, elm_init);
void elm_regist(void *self,void *world,u32 pid,void *actor) {
  WWHD_FUNC(0x02007F7C, void, self,world,pid,actor);
  u32 a=ea(self),flags=load<u32>(a+4);store<u32>(a,ea(world));store<u32>(a+8,pid);store<u32>(a+12,ea(actor));store<u32>(a+4,flags|1);
}
VERIFY(0x02007F7C, elm_regist);
void elm_release(void *self) {
  WWHD_FUNC(0x02007F98,void,self);
  u32 a=ea(self),flags=load<u32>(a+4);store<u32>(a,0);store<u32>(a+12,0);store<u32>(a+8,0xFFFFFFFF);store<u32>(a+4,flags&~1u);
}
VERIFY(0x02007F98, elm_release);
void *construct(void *self) {
  WWHD_FUNC(0x02007FBC,void *,self);
  if(!self)self=call<void *>(0x0273AD10,0x1404);
  if(self){store<u32>(ea(self)+0x1400,0x10000E3C);call<void>(0x028EFFD0,self,256,20,at<void>(0x02008B04));}return self;
}
VERIFY(0x02007FBC, construct);
void initialize(void *self) {
  WWHD_FUNC(0x02008020,void,self);
  store<u32>(0x1018C520,0);
  for(u32 i=0;i<256;++i)call<void>(0x02007F60,at<void>(entry(self,i)));
}
VERIFY(0x02008020,initialize);
void clear(void *self) {
  WWHD_FUNC(0x0200805C,void,self);
  for(u32 i=0;i<256;++i)if(load<u32>(entry(self,i)+4)&1)call<void>(0x02007F98,at<void>(entry(self,i)));
  for(u32 i=0;i<256;++i)call<void>(0x02007F60,at<void>(entry(self,i)));
}
VERIFY(0x0200805C,clear);
void *convert(void *work) {
  WWHD_FUNC(0x020080C0,void *,work);
  u32 a=ea(work),flag=load<u32>(a+0x30);if(flag&0x80000000u)return work;
  u32 vertex=load<u32>(a+4);store<u32>(a+0x30,flag|0x80000000u);
  if(vertex&3)assert_at(0x10000C08,0x244,0x10000B88);
  if(load<u32>(a+12)&1)assert_at(0x10000C08,0x245,0x10000BA8);
  if(load<u32>(a+20)&1)assert_at(0x10000C08,0x246,0x10000BC8);
  if(load<u32>(a+28)&1)assert_at(0x10000C08,0x247,0x10000C14);
  if(load<u32>(a+36)&3)assert_at(0x10000C08,0x248,0x10000BE8);
  if(load<u32>(a+44)&3)assert_at(0x10000C08,0x249,0x10000C38);
  vertex=load<u32>(a+4);u32 groups=load<u32>(a+36);if(vertex)store<u32>(a+4,vertex+a);
  u32 trees=load<u32>(a+28),info=load<u32>(a+44);store<u32>(a+36,groups+a);
  u32 blocks=load<u32>(a+20);u32 tris=load<u32>(a+12);store<u32>(a+44,info+a);
  s32 count=load<s32>(a+32);store<u32>(a+28,trees+a);store<u32>(a+12,tris+a);store<u32>(a+20,blocks+a);
  for(s32 i=0;i<count;++i){u32 table=load<u32>(a+36),p=table+u32(i)*52u;store<u32>(p,load<u32>(p)+a);count=load<s32>(a+32);}
  return work;
}
VERIFY(0x020080C0,convert);
bool poly_safe(void *self,void *poly) {
  WWHD_FUNC(0x02008254,bool,self,poly);
  u32 p=ea(poly);if(load<u16>(p)==0xFFFF)return false;u32 id=load<u16>(p+2);if(id>=256)return false;
  u32 e=entry(self,id);if(!(load<u32>(e+4)&1))return false;
  u32 ptr=load<u32>(p+4),world=load<u32>(e),pid=load<u32>(e+8);
  if(ptr!=world)return false;return load<u32>(p+8)==pid;
}
VERIFY(0x02008254,poly_safe);
s32 tri_group(void *self,u32 id,s32 poly) {
  WWHD_FUNC(0x020082C0,s32,self,id,poly);
  if(id>=256)assert_at(0x10000CA8,0x2F5,0x10000C58);u32 e=entry(self,id);if(!(load<u32>(e+4)&1))return -1;
  u32 w=load<u32>(e),data=0;if(poly>=0)data=load<u32>(w+0x94);
  if(poly<0||poly>=load<s32>(data+8)){assert_at(0x10000CB4,0x2A2,0x10000C78);data=load<u32>(w+0x94);}
  return load<u16>(load<u32>(data+12)+u32(poly)*10u+8);
}
VERIFY(0x020082C0,tri_group);
s32 group_room(void *self,u32 id,u32 group) {
  WWHD_FUNC(0x0200839C,s32,self,id,group);
  if(id>=256)assert_at(0x10000CE0,0x306,0x10000CC0);u32 e=entry(self,id);if(!(load<u32>(e+4)&1)||group==0xFFFF)return 0xFFFF;
  u32 world=load<u32>(e),vtable=load<u32>(world+4);return call_ptr<s32>(load<u32>(vtable+0x14),at<void>(world),group);
}
VERIFY(0x0200839C,group_room);
void *actor_pointer(void *self,u32 id) {
  WWHD_FUNC(0x02008438,void *,self,id);
  if(id>=256)assert_at(0x10000D24,0x2B4,0x10000CFC);return at<void>(load<u32>(entry(self,id)+12));
}
VERIFY(0x02008438,actor_pointer);
void *world_pointer(void *self,void *poly) {
  WWHD_FUNC(0x02008498,void *,self,poly);
  u32 id=load<u16>(ea(poly)+2);if(id>=256)return nullptr;u32 e=entry(self,id);if(!(load<u32>(e+4)&1))return nullptr;return at<void>(load<u32>(e));
}
VERIFY(0x02008498,world_pointer);
void *triangle_plane(void *self,u32 id,s32 poly) {
  WWHD_FUNC(0x020084C8,void *,self,id,poly);
  if(id>=256)return nullptr;u32 e=entry(self,id);if(!(load<u32>(e+4)&1))return nullptr;u32 w=load<u32>(e);
  if(poly<0||poly>=load<s32>(load<u32>(w+0x94)+8))assert_at(0x10000D60,0x2AF,0x10000D30);
  return at<void>(load<u32>(w+0x88)+u32(poly)*24u);
}
VERIFY(0x020084C8,triangle_plane);
void triangle_points(void *self,void *poly,void *p0,void *p1,void *p2) {
  WWHD_FUNC(0x02008570,void,self,poly,p0,p1,p2);
  u32 id=load<u16>(ea(poly)+2);if(id>=256)assert_at(0x10000D80,0x373,0x10000D6C);u32 e=entry(self,id);
  if(load<u32>(e+4)&1)call<void>(0x0200A220,at<void>(load<u32>(e)),load<u16>(ea(poly)),p0,p1,p2);
}
VERIFY(0x02008570,triangle_points);
s32 group_info(void *self,void *poly,s32 group) {
  WWHD_FUNC(0x020085F8,s32,self,poly,group);
  u32 id=load<u16>(ea(poly)+2);if(id>=256)assert_at(0x10000DD4,0x3FA,0x10000D8C);u32 e=entry(self,id);if(!(load<u32>(e+4)&1))return -1;
  u32 w=load<u32>(e),data=0;if(group>=0)data=load<u32>(w+0x94);
  if(group<0||group>=load<s32>(data+32)){assert_at(0x10000DE0,0x2E1,0x10000DAC);data=load<u32>(w+0x94);}
  return load<s32>(load<u32>(data+36)+u32(group)*52u+48);
}
VERIFY(0x020085F8,group_info);
bool regist(void *self,void *world,u32 pid,void *actor) {
  WWHD_FUNC(0x020086D4,bool,self,world,pid,actor);
  if(!world||load<u32>(ea(world))<256)return true;
  if(call<s32>(0x02009E58,world))return true;
  u32 i=load<u32>(0x1018C520);if(i>=256)assert_at(0x10000E00,0xA3,0x10000DEC);
  for(;;){u32 e=entry(self,i),n=i+1;if(!(load<u32>(e+4)&1)){
    call<void>(0x02007F7C,at<void>(e),world,pid,actor);store<u32>(ea(world),i);store<u32>(0x1018C520,s32(n)<256?n:0);return false;}
    i=n;u32 start=load<u32>(0x1018C520);if(s32(i)>=256)i=0;if(start==i)break;
  }store<u32>(ea(world),256);return true;
}
VERIFY(0x020086D4,regist);
bool release(void *self,void *world) {
  WWHD_FUNC(0x020087EC,bool,self,world);
  if(!world)return true;u32 id=load<u32>(ea(world));if(id>=256)return true;u32 e=entry(self,id);if(!(load<u32>(e+4)&1))return true;
  call<void>(0x02007F98,at<void>(e));store<u32>(ea(world),256);return false;
}
VERIFY(0x020087EC,release);
bool line_cross(void *self,void *check) {
  WWHD_FUNC(0x02008860,bool,self,check);
  u32 q=ea(check);store<u32>(q+0x18,0);store<u32>(q+0x1C,0xFFFFFFFF);store<u16>(q+0x16,256);store<u16>(q+0x14,0xFFFF);
  store<u32>(q+0x4C,load<u32>(q+0x4C)&~0x10u);bool result=false;
  for(u32 i=0;i<256;++i){u32 e=entry(self,i);if(!(load<u32>(e+4)&1))continue;u32 w=load<u32>(e);if(!load<u32>(w+0x90))continue;
    if(call<s32>(0x02008BB8,check,load<u32>(e+8)))continue;
    u32 flags=load<u32>(q+0x4C);store<u8>(q+0x50,u8(((flags>>30)&1)^1));store<u8>(q+0x51,u8(((flags>>31)&1)^1));store<u8>(q+0x52,u8(((flags>>29)&1)^1));
    w=load<u32>(e);if(!call<s32>(0x0200AB4C,at<void>(w),check,load<u32>(w+0xA4),1))continue;
    u32 pid=load<u32>(e+8),ptr=load<u32>(e);store<u32>(q+0x1C,pid);store<u32>(q+0x18,ptr);store<u16>(q+0x16,u16(i));result=true;
  }if(result)store<u32>(q+0x4C,load<u32>(q+0x4C)|0x10);return result;
}
VERIFY(0x02008860,line_cross);
f32 ground_cross(void *self,void *check) {
  WWHD_FUNC(0x02008974,f32,self,check);
  u32 q=ea(check);f32 height=load<f32>(0x10000E0C);u32 flags=load<u32>(q+0x30);
  store<u16>(q+0x14,0xFFFF);store<u32>(q+0x18,0);store<u32>(q+0x1C,0xFFFFFFFF);store<u16>(q+0x16,256);store<u32>(q+0x3C,flags&1);store<f32>(q+0x34,height);store<u32>(q+0x38,flags&2);
  for(u32 i=0;i<256;++i){u32 e=entry(self,i);if(!(load<u32>(e+4)&1))continue;u32 w=load<u32>(e);if(!load<u32>(w+0x90))continue;
    if(call<s32>(0x02008BB8,check,load<u32>(e+8)))continue;
    w=load<u32>(e);if(!call<s32>(0x0200B380,at<void>(w),check,load<u32>(w+0xA4),1))continue;
    u32 pid=load<u32>(e+8),ptr=load<u32>(e);store<u32>(q+0x1C,pid);store<u32>(q+0x18,ptr);store<u16>(q+0x16,u16(i));
  }return load<f32>(q+0x34);
}
VERIFY(0x02008974,ground_cross);
void static_init() {
  WWHD_FUNC(0x02008A70,void);
  store<u32>(0x101FF3E4,0);store<u32>(0x101FF3DC,0);store<u32>(0x101FF3E8,0);store<u32>(0x101FF3E0,0);
  call<void>(0x028F026C,at<void>(0x1018C528));
  f32 first=load<f32>(0x10000E14),second=load<f32>(0x10000E18);store<f32>(0x101FF3D0,first);store<f32>(0x101FF3D4,second);
  call<void>(0x028ED6F8,at<void>(0x101FF3D8));call<void>(0x028F026C,at<void>(0x1018C534));call<void>(0x028EAB2C,at<void>(0x101FF3D9));call<void>(0x028F026C,at<void>(0x1018C540));
}
VERIFY(0x02008A70,static_init);
void *elm_construct(void *self) {
  WWHD_FUNC(0x02008B04,void *,self);
  if(!self)self=call<void *>(0x0273AD10,20);if(self){store<u32>(ea(self)+16,0x10000E24);call<void>(0x02007F60,self);}return self;
}
VERIFY(0x02008B04,elm_construct);
void move(void *self) {WWHD_FUNC(0x02008B48,void,self);}
VERIFY(0x02008B48,move);
}
