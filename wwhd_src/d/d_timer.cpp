#include "gabi.h"
using namespace gabi;
namespace d_timer {
void draw_stub() {
 WWHD_FUNC(0x025C5770,void);

}
VERIFY(0x025C5770,draw_stub);
s32 draw_result() {
 WWHD_FUNC(0x025C5774,s32);
return 1;
}
VERIFY(0x025C5774,draw_result);
void hide() {
 WWHD_FUNC(0x025C577C,void);
store<u8>(0x1047B09A,0);store<u8>(0x1047B099,0);
}
VERIFY(0x025C577C,hide);
u32 stop(void *self,u32 reason) {
 WWHD_FUNC(0x025C5794,u32,self,reason);
u32 p=ea(self);
if(load<u8>(p+0x122)==1 || load<u8>(p+0x123)!=0 || load<u8>(p+0x124)!=2)return 0;
store<u8>(p+0x123,reason);store<u8>(p+0x122,1);return 1;
}
VERIFY(0x025C5794,stop);
u32 restart(void *self,u32 reason) {
 WWHD_FUNC(0x025C57D4,u32,self,reason);
u32 p=ea(self);
if(load<u8>(p+0x122)!=1 || load<u8>(p+0x123)!=reason || load<u8>(p+0x124)!=2)return 0;
store<u8>(p+0x122,0);store<u8>(p+0x123,0);return 1;
}
VERIFY(0x025C57D4,restart);
s32 remaining_ms(void *self) {
 WWHD_FUNC(0x025C5814,s32,self);
return s32(load<u32>(ea(self)+0x10C)*1000)/30;
}
VERIFY(0x025C5814,remaining_ms);
u32 stock_start(void *self) {
 WWHD_FUNC(0x025C583C,u32,self);
u32 p=ea(self);if(load<u8>(p+0x124)!=3)return 0;store<u8>(p+0x124,2);return 1;
}
VERIFY(0x025C583C,stock_start);
u32 start(void *self) {
 WWHD_FUNC(0x025C5864,u32,self);
u32 p=ea(self);u8 state=load<u8>(p+0x124);if(state!=0 && state!=1)return 0;store<u8>(p+0x124,2);return 1;
}
VERIFY(0x025C5864,start);
u32 elapsed_frames(void *self) {
 WWHD_FUNC(0x025C5890,u32,self);
u32 p=ea(self);return load<u32>(p+0x108)-load<u32>(p+0x10C);
}
VERIFY(0x025C5890,elapsed_frames);
u32 limit_frames(void *self) {
 WWHD_FUNC(0x025C58A0,u32,self);
return load<u32>(ea(self)+0x108);
}
VERIFY(0x025C58A0,limit_frames);
u32 end(void *self,s32 delay) {
 WWHD_FUNC(0x025C58A8,u32,self,delay);
u32 p=ea(self);if(load<u8>(p+0x124)!=2)return 0;store<u8>(p+0x124,4);if(delay!=-1)store<s32>(p+0x118,delay);return 1;
}
VERIFY(0x025C58A8,end);
u32 delete_request(void *self) {
 WWHD_FUNC(0x025C58D8,u32,self);
store<u8>(ea(self)+0x124,6);return 1;
}
VERIFY(0x025C58D8,delete_request);
void show(void *self) {
 WWHD_FUNC(0x025C58E8,void,self);
u32 p=ea(self);if(load<u8>(p+0x134)&1)store<u8>(0x1047B099,1);if(load<u8>(p+0x134)&2)store<u8>(0x1047B09A,1);
}
VERIFY(0x025C58E8,show);
u32 rest_check(void *self,s32 threshold) {
 WWHD_FUNC(0x025C591C,u32,self,threshold);
s32 now=call<s32>(0x025C5814,self);return now<=threshold && threshold>load<s32>(ea(self)+0x110);
}
VERIFY(0x025C591C,rest_check);
void sound(void *self) {
 WWHD_FUNC(0x025C5960,void,self);
u32 p=ea(self);
if(load<u8>(p+0x122)==0 && load<u32>(p+0x114)!=4){
 s32 now=call<s32>(0x025C5814,self);if(now/1000<load<s32>(p+0x110)/1000)call<void>(0x025E1988,u32(0x8EA));
}
if(!call<u32>(0x025C591C,self,load<u32>(p+0x128)))return;
u32 mode=load<u32>(p+0x114),table;if(mode==2)table=0x101EE750;else if(mode==3)table=0x101EE7B8;else return;
u32 idx=load<u32>(p+0x12C),sound=load<u32>(table+idx*8+4);if(sound==0xFFFFFFFF)return;
call<void>(0x025E1988,sound);
idx=load<u32>(p+0x12C)+1;store<u32>(p+0x12C,idx);store<u32>(p+0x128,load<u32>(table+idx*8));
}
VERIFY(0x025C5960,sound);
s32 execute(void *self) {
 WWHD_FUNC(0x025C5A8C,s32,self);
u32 p=ea(self),play=call<u32>(0x025200D4);
if(load<u32>(play+0x5CD8)&0x20000000){store<u8>(p+0x122,1);call<void>(0x025C577C,self);return 1;}
u8 state=load<u8>(p+0x124);
if((state==0 || state==2) && (load<u32>(p+0x114)==3 || load<u32>(p+0x114)==2)){
 bool should_stop=call<u32>(0x025986BC)!=0;
 if(!should_stop)should_stop=call<u32>(0x025AF2A4)!=0;
 if(!should_stop)should_stop=load<u8>(call<u32>(0x025200D4)+0x5292)==1;
 if(should_stop)call<u32>(0x025C5794,self,u32(1));else call<u32>(0x025C57D4,self,u32(1));
}
if(load<u8>(p+0x122)!=1){
 store<u32>(p+0x110,call<u32>(0x025C5814,self));state=load<u8>(p+0x124);
 if(state==1 || state==3){
  s16 delay=s16(u16(load<u16>(p+0x120)-1));store<s16>(p+0x120,delay);
  if(delay<=0){if(load<u8>(p+0x124)==3)call<u32>(0x025C583C,self);else call<u32>(0x025C5864,self);}
 }else if(state==2){
  store<u32>(p+0x10C,load<u32>(p+0x10C)-1);
  u32 value=call<u32>(0x025C5890,self);store<u32>(call<u32>(0x025200D4)+0x5CF4,value);
  value=call<u32>(0x025C58A0,self);store<u32>(call<u32>(0x025200D4)+0x5CF8,value);
  if(call<s32>(0x025C5814,self)<=0){value=call<u32>(0x025C5890,self);store<u32>(call<u32>(0x025200D4)+0x5CF4,value);if(load<u32>(p+0x114)==3)call<u32>(0x025C58A8,self,s32(30));}
 }
}
state=load<u8>(p+0x124);
if(state==6)call<void>(0x025DF944,self);
else if(state==4){
 s32 delay=load<s32>(p+0x118);
 if(delay>0)store<u32>(p+0x118,u32(delay)-1);
 else if(load<u32>(p+0x114)==3)call<u32>(0x025C58D8,self);
 else store<u8>(p+0x124,5);
}
u32 show_delay=load<u32>(p+0x11C);if(show_delay){--show_delay;store<u32>(p+0x11C,show_delay);if(show_delay==0)call<void>(0x025C58E8,self);}
call<void>(0x025C5960,self);return 1;
}
VERIFY(0x025C5A8C,execute);
s32 execute_wrapper(void *self) {
 WWHD_FUNC(0x025C5D18,s32,self);
return call<s32>(0x025C5A8C,self);
}
VERIFY(0x025C5D18,execute_wrapper);
s32 is_delete() {
 WWHD_FUNC(0x025C5D1C,s32);
return 1;
}
VERIFY(0x025C5D1C,is_delete);
s32 destroy(void *self) {
 WWHD_FUNC(0x025C5D24,s32,self);
u32 p=ea(self);
if(load<u32>(p+0x114)==3){
 u8 state=load<u8>(p+0x124);store<u32>(call<u32>(0x025200D4)+0x5CFC,(state==6 || state==5 || state==4)?u32(-1):3);
 u32 value=call<u32>(0x025C5890,self);store<u32>(call<u32>(0x025200D4)+0x5CF4,value);
 value=call<u32>(0x025C58A0,self);store<u32>(call<u32>(0x025200D4)+0x5CF8,value);
}else{store<u32>(call<u32>(0x025200D4)+0x5CF4,0);store<u32>(call<u32>(0x025200D4)+0x5CF8,0);store<u32>(call<u32>(0x025200D4)+0x5CFC,u32(-1));}
store<u32>(call<u32>(0x025200D4)+0x5CF0,0);call<void>(0x025C577C,self);return 1;
}
VERIFY(0x025C5D24,destroy);
s32 delete_wrapper(void *self) {
 WWHD_FUNC(0x025C5E50,s32,self);
return call<s32>(0x025C5D24,self);
}
VERIFY(0x025C5E50,delete_wrapper);
void set_show_type(void *self,u32 bits) {
 WWHD_FUNC(0x025C5E54,void,self,bits);
u32 p=ea(self);u8 initialized=load<u8>(p+0x135);store<u8>(p+0x134,bits);if(initialized){store<u8>(0x1047B099,bits&1);store<u8>(0x1047B09A,(load<u8>(p+0x134)>>1)&1);}
}
VERIFY(0x025C5E54,set_show_type);
void set_icon_type(u32 ignored,u32 type) {
 WWHD_FUNC(0x025C5E84,void,ignored,type);
store<u8>(0x1047B07E,type);
}
VERIFY(0x025C5E84,set_icon_type);
u32 stock_start_delay(void *self,s16 delay) {
 WWHD_FUNC(0x025C5E90,u32,self,delay);
u32 p=ea(self);if(load<u8>(p+0x124)!=0)return 0;store<s16>(p+0x120,delay);store<u8>(p+0x124,3);return 1;
}
VERIFY(0x025C5E90,stock_start_delay);
s32 create(void *self) {
 WWHD_FUNC(0x025C5EB8,s32,self);
u32 p=ea(self),append=load<u32>(p+0xAC);store<u8>(p+0x135,0);if(!append)return 5;
call<void>(0x025C5E54,self,u32(load<u8>(append+0x22)));call<void>(0x025C5E84,self,u32(load<u8>(append+0x23)));
u32 mode=load<u32>(append+0x1C);store<u32>(p+0x114,mode);
if(mode==7){
 store<u32>(p+0x108,load<u32>(call<u32>(0x025200D4)+0x5CF8));
 u32 play=call<u32>(0x025200D4);store<u32>(p+0x10C,load<u32>(p+0x108)-load<u32>(play+0x5CF4));
 u32 value=call<u32>(0x025C5890,self);store<u32>(call<u32>(0x025200D4)+0x5CF4,value);
 value=call<u32>(0x025C58A0,self);store<u32>(call<u32>(0x025200D4)+0x5CF8,value);
 mode=load<u32>(call<u32>(0x025200D4)+0x5CFC);store<u8>(p+0x124,0);store<u32>(p+0x114,mode);
 store<u32>(call<u32>(0x025200D4)+0x5CFC,mode);store<u32>(call<u32>(0x025200D4)+0x5CF0,p);
 if(load<u32>(p+0x114)==3)call<void>(0x025C5E54,self,u32(1));
 call<u32>(0x025C5E90,self,s16(10));
}else{
 u32 frames=load<u16>(append+0x20)*30;store<u32>(p+0x108,frames);store<u32>(p+0x10C,frames);
 u32 value=call<u32>(0x025C5890,self);store<u32>(call<u32>(0x025200D4)+0x5CF4,value);
 value=call<u32>(0x025C58A0,self);store<u32>(call<u32>(0x025200D4)+0x5CF8,value);
 mode=load<u32>(p+0x114);store<u8>(p+0x124,0);store<u32>(call<u32>(0x025200D4)+0x5CFC,mode);store<u32>(call<u32>(0x025200D4)+0x5CF0,p);
}
store<u8>(p+0x123,0);store<u8>(p+0x122,0);store<u32>(p+0x118,120);store<u32>(p+0x128,0);store<u32>(p+0x11C,6);store<u32>(p+0x12C,0);
mode=load<u32>(p+0x114);
if(mode==2 || mode==3){
 u32 table=mode==2?0x101EE750:0x101EE7B8,idx=load<u32>(p+0x12C);
 for(;;){u32 threshold=load<u32>(table+idx*8);store<u32>(p+0x128,threshold);s32 now=call<s32>(0x025C5814,self);if(now>s32(threshold))break;
 idx=load<u32>(p+0x12C)+1;u32 old=load<u32>(p+0x128);store<u32>(p+0x12C,idx);if(s32(old)<0)break;}
}
store<u8>(p+0x135,1);return 4;
}
VERIFY(0x025C5EB8,create);
s32 create_wrapper(void *self) {
 WWHD_FUNC(0x025C60F0,s32,self);
return call<s32>(0x025C5EB8,self);
}
VERIFY(0x025C60F0,create_wrapper);
u32 create_timer(u32 mode,u32 seconds,u32 bits,u32 icon,f32 x,f32 y,f32 rx,f32 ry) {
 WWHD_FUNC(0x025C60F4,u32,mode,seconds,bits,icon,x,y,rx,ry);
if(load<s32>(call<u32>(0x025200D4)+0x5CFC)!=-1)return u32(-1);
return call<u32>(0x025DB468,u32(0x1E0),mode&0xFF,seconds,bits,icon,u32(0),x,y,rx,ry);
}
VERIFY(0x025C60F4,create_timer);
u32 create_stock_timer() {
 WWHD_FUNC(0x025C61E4,u32);
if(load<s32>(call<u32>(0x025200D4)+0x5CFC)==-1)return u32(-1);
f32 x=load<f32>(0x1005608C),rx=load<f32>(0x10056094),ry=load<f32>(0x10056098),y=load<f32>(0x10056090);
return call<u32>(0x025DB468,u32(0x1E0),u32(7),u32(0),u32(3),u32(0),u32(0),x,y,rx,ry);
}
VERIFY(0x025C61E4,create_stock_timer);
u32 start_delay(void *self,s16 delay) {
 WWHD_FUNC(0x025C6250,u32,self,delay);
u32 p=ea(self);if(load<u8>(p+0x124)!=0)return 0;store<s16>(p+0x120,delay);store<u8>(p+0x124,1);return 1;
}
VERIFY(0x025C6250,start_delay);
u32 delete_check(void *self) {
 WWHD_FUNC(0x025C6278,u32,self);
return load<u8>(ea(self)+0x124)==5;
}
VERIFY(0x025C6278,delete_check);
void static_init() {
 WWHD_FUNC(0x025C628C,void);
store<u32>(0x10487210,0);store<u32>(0x1048720C,0);store<u32>(0x10487208,0);store<u32>(0x10487204,0);
call<void>(0x028F026C,at<void>(0x101EE820));
f32 first=load<f32>(0x100560A0),second=load<f32>(0x100560A4);store<f32>(0x104871E8,first);store<f32>(0x104871EC,second);
call<void>(0x028ED6F8,at<void>(0x10487200));call<void>(0x028F026C,at<void>(0x101EE82C));call<void>(0x028EAB2C,at<void>(0x10487201));call<void>(0x028F026C,at<void>(0x101EE838));
first=load<f32>(0x100560A8);second=load<f32>(0x100560AC);store<f32>(0x104871F0,first);store<f32>(0x104871F8,second);store<f32>(0x104871F4,first);store<f32>(0x104871FC,second);
}
VERIFY(0x025C628C,static_init);
}
