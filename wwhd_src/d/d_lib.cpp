#include "bindings.h"
using namespace gabi;
namespace d_lib {
void debug_fan(){WWHD_FUNC(0x02587684,void,(u32)0);}
VERIFY(0x02587684,debug_fan);
void get_position(u32 m,u32 out){WWHD_FUNC(0x02587C88,void,m,out);f32 y=load<f32>(m+0x1c),x=load<f32>(m+0xc),z=load<f32>(m+0x2c);store<f32>(out,x);store<f32>(out+4,y);store<f32>(out+8,z);}
VERIFY(0x02587C88,get_position);
void set_wait(u32 p,s32 first,s32 minimum,s32 step,s32 delay,s32 margin,s32 sector,f64 high,f64 low){
 WWHD_FUNC(0x025885C4,void,p,first,minimum,step,delay,margin,sector,high,low);
 store<s16>(p+0xe,first);store<s16>(p+0x10,minimum);store<s16>(p+0x12,step);store<s16>(p+0x18,delay);
 store<f32>(p,high);store<f32>(p+4,low);store<s16>(p+0x20,margin);store<s16>(p+0x22,sector);
}
VERIFY(0x025885C4,set_wait);
void init(u32 p){WWHD_FUNC(0x025885E8,void,p);s16 first=load<s16>(p+0xe);store<s16>(p+0xc,0);s16 delay=load<s16>(p+0x18);store<s16>(p+0x14,first);store<s16>(p+0x1a,delay);store<s16>(p+0x1e,0);store<s16>(p+0x1c,delay);store<u8>(p+8,0);store<s16>(p+0xa,0);store<u8>(p+9,0);store<s16>(p+0x16,first);}
VERIFY(0x025885E8,init);
void destroy(u32 p,u32 flags){WWHD_FUNC(0x025886D8,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p,flags);}
VERIFY(0x025886D8,destroy);
void x_init(u32 p){WWHD_FUNC(0x025886EC,void,p);s16 first=load<s16>(p+0xe);u8 flags=load<u8>(p+8)&0xfc;s16 delay=load<s16>(p+0x18);store<s16>(p+0xa,0);store<u8>(p+8,flags);store<s16>(p+0x14,first);store<s16>(p+0x1a,delay);}
VERIFY(0x025886EC,x_init);
void y_init(u32 p){WWHD_FUNC(0x02588714,void,p);s16 first=load<s16>(p+0xe);u8 flags=load<u8>(p+8)&0xf3;s16 delay=load<s16>(p+0x18);store<s16>(p+0xc,0);store<u8>(p+8,flags);store<s16>(p+0x16,first);store<s16>(p+0x1c,delay);}
VERIFY(0x02588714,y_init);
static s32 repeat(u32 p,u8 bit,u32 timer,u32 current,u32 delay){
 if(load<s16>(p+timer)!=0)return 0;
 if(!(load<u8>(p+8)&bit))return 0;
 s16 remaining=load<s16>(p+delay),value=load<s16>(p+current);store<s16>(p+timer,value);
 if(remaining==0){s16 next=s16(load<s16>(p+current)-load<s16>(p+0x12)),minimum=load<s16>(p+0x10);store<s16>(p+current,next);if(next<minimum)store<s16>(p+current,minimum);}
 else store<s16>(p+delay,remaining-1);
 return 1;
}
s32 left(u32 p){WWHD_FUNC(0x02588A0C,s32,p);return repeat(p,1,0xa,0x14,0x1a);}
VERIFY(0x02588A0C,left);
s32 right(u32 p){WWHD_FUNC(0x02588A7C,s32,p);return repeat(p,2,0xa,0x14,0x1a);}
VERIFY(0x02588A7C,right);
s32 up(u32 p){WWHD_FUNC(0x02588AEC,s32,p);return repeat(p,4,0xc,0x16,0x1c);}
VERIFY(0x02588AEC,up);
s32 down(u32 p){WWHD_FUNC(0x02588B5C,s32,p);return repeat(p,8,0xc,0x16,0x1c);}
VERIFY(0x02588B5C,down);
f64 get_value(){WWHD_FUNC(0x02588BCC,f64,(u32)0);return call<f64>(0x020079B4,u32(0));}
VERIFY(0x02588BCC,get_value);
s32 get_angle(){WWHD_FUNC(0x02588BD4,s32,(u32)0);return call<s32>(0x02007A1C,u32(0));}
VERIFY(0x02588BD4,get_angle);
void static_init(){WWHD_FUNC(0x02588D40,void,(u32)0);store<u32>(0x1047765c,0);store<u32>(0x10477654,0);store<u32>(0x10477660,0);store<u32>(0x10477658,0);call<void>(0x028F026C,u32(0x101e9c14));f32 a=load<f32>(0x1005077c),b=load<f32>(0x10050780);store<f32>(0x10477648,a);store<f32>(0x1047764c,b);call<void>(0x028ED6F8,u32(0x10477650));call<void>(0x028F026C,u32(0x101e9c20));call<void>(0x028EAB2C,u32(0x10477651));call<void>(0x028F026C,u32(0x101e9c2c));}
VERIFY(0x02588D40,static_init);
void adjacent_destroy(u32 p,u32 flags){WWHD_FUNC(0x02588DD4,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p,flags);}
VERIFY(0x02588DD4,adjacent_destroy);
void adjacent_empty(){WWHD_FUNC(0x02588DE8,void,(u32)0);}
VERIFY(0x02588DE8,adjacent_empty);
u32 first_message(u32 bit,u32 first,u32 again){WWHD_FUNC(0x02588034,u32,bit,first,again);if(call<u32>(0x025B8B94,load<u32>(0x101f84dc)+0x644,bit))return again;call<void>(0x025B8B68,load<u32>(0x101f84dc)+0x644,bit);return first;}
VERIFY(0x02588034,first_message);
s32 brk_init(u32 model,u32 animation,u32 arc,u32 number){
 WWHD_FUNC(0x02587688,s32,model,animation,arc,number);
 struct StringRef{u32 text,table;};Local<StringRef> ref;u32 a=ea(ref.get());store<u32>(a,arc);store<u32>(a+4,0x10050628);
 u32 resource=call<u32>(0x026066C4,load<u32>(0x101f4f28),ref.get(),number);
 if(!resource)call<void>(0x0273AA24,u32(0x100506ac),u32(0xae),u32(0x100506b8));
 return call<u32>(0x025E8154,animation,model,resource,u32(1),u32(0),u32(0),u32(-1),u32(0),u32(0),load<f32>(0x100506a8))!=0;
}
VERIFY(0x02587688,brk_init);
s32 btk_init(u32 model,u32 animation,u32 arc,u32 number){
 WWHD_FUNC(0x02587740,s32,model,animation,arc,number);
 struct StringRef{u32 text,table;};Local<StringRef> ref;u32 a=ea(ref.get());store<u32>(a,arc);store<u32>(a+4,0x10050628);
 u32 resource=call<u32>(0x026066C4,load<u32>(0x101f4f28),ref.get(),number);
 if(!resource)call<void>(0x0273AA24,u32(0x100506c4),u32(0xbb),u32(0x100506d0));
 return call<u32>(0x025E7CE0,animation,model,resource,u32(1),u32(0),u32(0),u32(-1),u32(0),u32(0),load<f32>(0x100506a8))!=0;
}
VERIFY(0x02587740,btk_init);
u32 constructor(u32 p,s32 first,s32 minimum,s32 step,s32 delay,s32 margin,s32 sector,f64 high,f64 low){
 WWHD_FUNC(0x0258861C,u32,p,first,minimum,step,delay,margin,sector,high,low);
 if(!p)p=call<u32>(0x0273AD10,u32(40));
 if(!p)return p;
 store<u32>(p+0x24,0x10050788);
 p=call<u32>(0x025885C4,p,first,minimum,step,delay,margin,sector,high,low);
 return call<u32>(0x025885E8,p);
}
VERIFY(0x0258861C,constructor);
s32 ground_position(u32 pos){
 WWHD_FUNC(0x02588BDC,s32,pos);
 struct Ground{u8 data[84];};Local<Ground> ground;u32 a=ea(ground.get());call<void>(0x02008E0C,ground.get());
 store<u32>(a+0x4c,0x10050670);store<u8>(a+0x44,0);
 f32 x=load<f32>(pos),y=load<f32>(pos+4);
 store<u8>(a+0x47,0);store<u32>(a+0x10,0x10050650);f32 up=load<f32>(0x10050778);
 store<u32>(a+0x50,1);store<u8>(a+0x48,0);y=fadds_ppc(y,up);store<u32>(a+0x40,0x10050680);store<u32>(a+4,a+0x4c);store<u8>(a+0x46,0);store<u8>(a+0x45,0);store<u32>(a,a+0x40);store<u32>(a+0x20,0x10050660);store<f32>(pos+4,y);store<u8>(a+0x49,0);store<u8>(a+0x4a,0);
 f32 z=load<f32>(pos+8);store<f32>(a+0x24,x);store<f32>(a+0x28,y);store<f32>(a+0x2c,z);
 u32 game=call<u32>(0x025200D4);f64 height=call<f64>(0x02008974,game+0x12a0,ground.get());
 bool fail=height==load<f32>(0x10050694);
 if(fail){f32 old=load<f32>(pos+4);store<u32>(a+0x20,0x10050660);store<u32>(a+0x4c,0x10050640);store<u32>(a+0x40,0x10050680);store<f32>(pos+4,fsubs_ppc(old,up));}
 else{store<u32>(a+0x20,0x10050660);store<u32>(a+0x40,0x10050680);store<u32>(a+0x4c,0x10050640);store<f32>(pos+4,height);}
 call<void>(0x02008DAC,ground.get(),u32(0));return !fail;
}
VERIFY(0x02588BDC,ground_position);
void circle_path(u32 p){WWHD_FUNC(0x02587128,void,p);s32 angle=load<s16>(p+0x20)+load<s16>(p+0x22);f32 amplitude=load<f32>(p+0x1c),x=load<f32>(p),y=load<f32>(p+4);store<s16>(p+0x20,angle);f32 sine=load<f32>(0x104a44f8+((u32(angle)&0xffff)>>3)*8);f32 radius=load<f32>(p+0x18),z=load<f32>(p+8);f32 radial=fmadds(sine,amplitude,radius);call<void>(0x028E93CC,u32(0x1048d0cc),x,y,z);call<void>(0x025F1C28,u32(0x1048d0cc),s32(load<s16>(p+0x20)));f32 zero=load<f32>(0x10050690);call<void>(0x025F24E0,radial,zero,zero);store<f32>(p+0xc,load<f32>(0x1048d0d8));store<f32>(p+0x10,load<f32>(0x1048d0e8));store<f32>(p+0x14,load<f32>(0x1048d0f8));}
VERIFY(0x02587128,circle_path);
f64 water_y(u32 pos,u32 acch){WWHD_FUNC(0x025871F8,f64,pos,acch);bool hit=(load<u32>(acch+0x28)>>11)&1;f64 water=hit?load<f32>(acch+0x1bc):load<f32>(0x10050694);u32 area=call<u32>(0x0246B6A4,load<f32>(pos),load<f32>(pos+8));if(area){f64 sea=call<f64>(0x0246BA0C,load<f32>(pos),load<f32>(pos+8));if(!hit||sea>water)return sea;}return hit?water:load<f32>(0x10050690);}
VERIFY(0x025871F8,water_y);
s32 path_info(u32 out,s32 point){WWHD_FUNC(0x02587CA4,s32,out,point);u32 manager=call<u32>(0x025200D4)+0x5150;u32 table=load<u32>(manager);u32 path=call_ptr<u32>(load<u32>(table+0x18c),manager);if(!path)return 0;store<u32>(out,load<u32>(path+4)+u32(point)*12);return 1;}
VERIFY(0x02587CA4,path_info);
u32 check_trigger(u32 p){
 WWHD_FUNC(0x0258873C,u32,p);
 u8 old=load<u8>(p+8);u32 table=load<u32>(p+0x24);store<u8>(p+9,old);
 f64 value=call_ptr<f64>(load<u32>(table+0x14),p);
 table=load<u32>(p+0x24);s32 angle=call_ptr<s32>(load<u32>(table+0x1c),p);
 s16 sector=s16((0x2000-load<s16>(p+0x22))>>1);
 if(std::fabs(value)<load<f32>(0x100030b8)){
  store<u8>(p+8,0);call<void>(0x025886EC,p);call<void>(0x02588714,p);
 }else{
  s32 shift=load<s16>(p+0x1e);u8 bits;
  if(angle<shift+sector-0x7000)bits=4;
  else if(angle<shift-0x5000-sector)bits=5;
  else if(angle<shift+sector-0x3000)bits=1;
  else if(angle<shift-0x1000-sector)bits=9;
  else if(angle<shift+sector+0x1000)bits=8;
  else if(angle<shift+0x3000-sector)bits=10;
  else if(angle<shift+sector+0x5000)bits=2;
  else if(angle<shift+0x7000-sector)bits=6;
  else bits=4;
  u8 flags;
  if(!(value<load<f32>(p)))flags=bits;
  else if(value<load<f32>(p+4))flags=0;
  else flags=load<u8>(p+8)&~bits;
  u8 previous=load<u8>(p+9);store<u8>(p+8,flags);
  if(flags!=previous){if(!flags)store<s16>(p+0x1e,0);else store<s16>(p+0x1e,(angle&0x1fff)>0x1000?load<s16>(p+0x20):-load<s16>(p+0x20));}
  if(!(load<u8>(p+8)&3))call<void>(0x025886EC,p);
  if(!(load<u8>(p+8)&12))call<void>(0x02588714,p);
 }
 u8 flags=load<u8>(p+8),overlap=load<u8>(p+9)&flags;
 if(overlap&3){s16 timer=load<s16>(p+0xa);if(timer>0){overlap=load<u8>(p+9)&load<u8>(p+8);store<s16>(p+0xa,timer-1);}}
 if(overlap&12){s16 timer=load<s16>(p+0xc);if(timer>0){flags=load<u8>(p+8);store<s16>(p+0xc,timer-1);}}
 return flags;
}
VERIFY(0x0258873C,check_trigger);
struct Vec{u32 words[3];};struct Quat{u32 words[4];};
static void copy_vec(u32 dst,u32 src){u32 x=load<u32>(src),z=load<u32>(src+8),y=load<u32>(src+4);store<u32>(dst,x);store<u32>(dst+4,y);store<u32>(dst+8,z);}
s32 actor_circle(u32 center,u32 actor,f64 radius,f64 height){
 WWHD_FUNC(0x025880B4,s32,center,actor,radius,height);Local<Vec> diff,flat;
 call<void>(0x0201ADE0,center,diff.get(),actor+0x314);
 f32 x=load<f32>(ea(diff.get())),zero=load<f32>(0x10050690),z=load<f32>(ea(diff.get())+8);
 store<f32>(ea(flat.get())+4,zero);store<f32>(ea(flat.get()),x);store<f32>(ea(flat.get())+8,z);
 f64 squared=call<f64>(0x028E8DD0,flat.get());f64 distance=call<f64>(0x028F4384,squared);
 f32 ay=load<f32>(actor+0x318),cy=load<f32>(center+4);f32 delta=fsubs_ppc(cy,ay);
 return distance<radius&&std::fabs(delta)<height;
}
VERIFY(0x025880B4,actor_circle);
s32 player_circle(u32 center,f64 radius,f64 height){WWHD_FUNC(0x025881A4,s32,center,radius,height);u32 game=call<u32>(0x025200D4);f32 y=load<f32>(center+4);u32 actor=load<u32>(game+0x5b34);f32 x=load<f32>(center),z=load<f32>(center+8);Local<Vec> pos;store<f32>(ea(pos.get())+4,y);store<f32>(ea(pos.get()),x);store<f32>(ea(pos.get())+8,z);return call<s32>(0x025880B4,pos.get(),actor,radius,height);}
VERIFY(0x025881A4,player_circle);
void triangle_quat(u32 out,u32 a,u32 b,u32 c){
 WWHD_FUNC(0x02588460,void,out,a,b,c);Local<Vec> tmp,e1,e2,cross;Local<Quat> result;
 call<void>(0x0201ADE0,b,tmp.get(),a);copy_vec(ea(e1.get()),ea(tmp.get()));
 call<void>(0x0201ADE0,c,tmp.get(),a);copy_vec(ea(e2.get()),ea(tmp.get()));
 call<void>(0x0201B080,e1.get(),tmp.get(),e2.get());copy_vec(ea(cross.get()),ea(tmp.get()));
 call<void>(0x02312548,result.get(),cross.get());u32 x=load<u32>(ea(result.get())),y=load<u32>(ea(result.get())+4);store<u32>(out,x);store<u32>(out+4,y);u32 w=load<u32>(ea(result.get())+12),z=load<u32>(ea(result.get())+8);store<u32>(out+12,w);store<u32>(out+8,z);
}
VERIFY(0x02588460,triangle_quat);
void blend_triangle(u32 out,u32 a,u32 b,u32 c,f64 t){WWHD_FUNC(0x02588544,void,out,a,b,c,t);Local<Quat> q,d;call<void>(0x02588460,q.get(),a,b,c);call<void>(0x028E9BC0,out,q.get(),d.get(),t);u32 z=load<u32>(ea(d.get())+8),x=load<u32>(ea(d.get())),y=load<u32>(ea(d.get())+4);store<u32>(out,x);store<u32>(out+4,y);u32 w=load<u32>(ea(d.get())+12);store<u32>(out+8,z);store<u32>(out+12,w);}
VERIFY(0x02588544,blend_triangle);
s32 actor_fan(u32 center,u32 actor,s32 facing,s32 angle_limit,f64 radius,f64 height){
 WWHD_FUNC(0x02588230,s32,center,actor,facing,angle_limit,radius,height);Local<Vec> diff,flat;
 call<void>(0x0201ADE0,center,diff.get(),actor+0x314);f32 x=load<f32>(ea(diff.get())),zero=load<f32>(0x10050690),z=load<f32>(ea(diff.get())+8);store<f32>(ea(flat.get())+4,zero);store<f32>(ea(flat.get()),x);store<f32>(ea(flat.get())+8,z);
 f64 squared=call<f64>(0x028E8DD0,flat.get()),distance=call<f64>(0x028F4384,squared);
 f32 ay=load<f32>(actor+0x318),cy=load<f32>(center+4);f64 delta=std::fabs(fsubs_ppc(cy,ay));
 s32 direction=call<s32>(0x0200F93C,center,actor+0x314);s32 difference=call<s32>(0x0200FAAC,facing,direction);
 return distance<radius&&delta<height&&difference<angle_limit;
}
VERIFY(0x02588230,actor_fan);
void scale_animation(u32 value,u32 table,s32 count,u32 index,f64 speed,f64 maximum,f64 minimum){
 WWHD_FUNC(0x02587BE0,void,value,table,count,index,speed,maximum,minimum);s32 current=load<s32>(index);
 if(current<count){f64 delta=call<f64>(0x0200ECD4,value,load<f32>(table+u32(current)*4),speed,maximum,minimum);if(delta==load<f32>(0x10050690)){s32 next=s32(load<u32>(index)-1);store<s32>(index,next<0?0:next);}}
 else store<u32>(index,u32(current)-1);
}
VERIFY(0x02587BE0,scale_animation);
void debug_axis(u32 matrix,f64 length){
 WWHD_FUNC(0x02587514,void,matrix,length);struct SixVec{f32 v[18];};Local<SixVec> inputs,outputs;u32 a=ea(inputs.get()),b=ea(outputs.get());f32 zero=load<f32>(0x10050690);
 for(u32 i=0;i<18;i++)store<f32>(a+i*4,(i==0||i==4||i==8)?f32(length):zero);
 call<void>(0x028E90D4,matrix,u32(0x1048d0cc));
 for(u32 i=0;i<6;i++)call<void>(0x028E8F64,u32(0x1048d0cc),a+i*12,b+i*12);
 if(!load<u32>(0x101fdac0)){store<u32>(0x101fdac0,1);call<void>(0xC000A848,u32(0x101febf8),u32(0x10050618),u32(4));}
 if(!load<u32>(0x101fda48)){store<u32>(0x101fda48,1);call<void>(0xC000A848,u32(0x101febec),u32(0x1005061c),u32(4));}
 if(!load<u32>(0x101fda50)){store<u32>(0x101fda50,1);call<void>(0xC000A848,u32(0x101febf4),u32(0x10050620),u32(4));}
}
VERIFY(0x02587514,debug_axis);
void wave_rot(u32 pos,u32 wave,f64 sway){
 WWHD_FUNC(0x025872F4,void,pos,wave,sway);f32 spacing=load<f32>(0x10050698),near=fsubs_ppc(load<f32>(pos+8),spacing);
 f64 first=call<f64>(0x0246BA0C,load<f32>(pos),near);f32 far=fadds_ppc(load<f32>(pos+8),spacing);f64 second=call<f64>(0x0246BA0C,load<f32>(pos),far);
 s16 ax=s16(-call<s32>(0x020195B0,f32(second-first),fsubs_ppc(far,near)));
 near=fsubs_ppc(load<f32>(pos),spacing);first=call<f64>(0x0246BA0C,near,load<f32>(pos+8));far=fadds_ppc(load<f32>(pos),spacing);second=call<f64>(0x0246BA0C,far,load<f32>(pos+8));s32 az=call<s32>(0x020195B0,f32(second-first),fsubs_ppc(far,near));
 call<void>(0x0200F428,wave,s32(ax),s32(10),s32(0x200));call<void>(0x0200F428,wave+2,az,s32(10),s32(0x200));
 s16 angle_x=load<s16>(wave);f32 amplitude=load<f32>(0x1005069c);s32 phase_x=load<s16>(wave+4)+400;store<s16>(wave+4,phase_x);s32 phase_z=load<s16>(wave+6)+430;store<s16>(wave+6,phase_z);
 f32 sum=f32(sway+amplitude);sum=fadds_ppc(sum,load<f32>(0x1047bcd0));f32 sin=load<f32>(0x104a44f8+((u32(phase_x)&0xffff)>>3)*8);
 store<s16>(wave+8,s16(ftoi(fmadds(sin,sum,f32(angle_x)))));s16 angle_z=load<s16>(wave+2);f32 cos=load<f32>(0x104a44fc+((u32(phase_z)&0xffff)>>3)*8);store<s16>(wave+0xa,s16(ftoi(fmadds(cos,sum,f32(angle_z)))));
}
VERIFY(0x025872F4,wave_rot);
struct StringRef{u32 text,table;};
static u32 resource(u32 arc,u32 index){Local<StringRef> ref;store<u32>(ea(ref.get()),arc);store<u32>(ea(ref.get())+4,0x10050628);return call<u32>(0x026066C4,load<u32>(0x101f4f28),ref.get(),index);}
static void animations(u32 arc,u32 model,u32 animation,u32 parameter,u32 previous,u32 indices,u32 parameters,u32 force,u32 secondary,bool sounds){
 s8 parm=load<s8>(parameter),old=load<s8>(previous);f32 zero=load<f32>(0x10050690);
 s8 next=0;bool change=false;if(parm!=old){next=load<s8>(parameters+u32(s32(parm))*16);change=next!=-1;}
 if(!change&&force){next=load<s8>(parameters+u32(s32(parm))*16);change=true;}
 if(change){u32 stride=sounds?8:4;store<s8>(animation,next);u32 sound=0,idx=indices+u32(s32(next))*stride;
  if(sounds&&load<s32>(idx+4)>=0){sound=resource(arc,load<u32>(idx+4));idx=indices+u32(s32(load<s8>(animation)))*stride;}
  u32 data=resource(arc,load<u32>(idx));u32 row=parameters+u32(s32(load<s8>(parameter)))*16;f32 end=load<f32>(0x100506e4);
  call<void>(0x025E4A98,model,data,load<u32>(row+12),sound,load<f32>(row+4),load<f32>(row+8),zero,end);
  if(secondary){u32 extra=resource(arc,load<u32>(indices+u32(s32(load<s8>(animation)))*stride));row=parameters+u32(s32(load<s8>(parameter)))*16;call<void>(0x025E4A98,secondary,extra,load<u32>(row+12),u32(0),load<f32>(row+4),load<f32>(row+8),zero,end);}
  parm=load<s8>(parameter);
 }
 store<s8>(previous,parm);
 if((load<u8>(model+0xa7)&1)||load<f32>(model+0x98)==zero){u32 row=parameters+u32(s32(load<s8>(parameter)))*16;s8 nextparm=load<s8>(row+1);if(nextparm!=-1&&load<u32>(row+12)==0)store<s8>(parameter,nextparm);}
}
void set_animation(u32 arc,u32 model,u32 animation,u32 parameter,u32 previous,u32 indices,u32 parameters,u32 force){WWHD_FUNC(0x025877F8,void,arc,model,animation,parameter,previous,indices,parameters,force);u32 secondary=load<u32>(cpu->r[1]+8);animations(arc,model,animation,parameter,previous,indices,parameters,force,secondary,true);}
VERIFY(0x025877F8,set_animation);
void set_bcks(u32 arc,u32 model,u32 animation,u32 parameter,u32 previous,u32 indices,u32 parameters,u32 force){WWHD_FUNC(0x02587A0C,void,arc,model,animation,parameter,previous,indices,parameters,force);u32 secondary=load<u32>(cpu->r[1]+8);animations(arc,model,animation,parameter,previous,indices,parameters,force,secondary,false);}
VERIFY(0x02587A0C,set_bcks);
void next_stage(u32 index,s32 room){WWHD_FUNC(0x02587EFC,void,index,room);u32 manager;
 if(room==-1)manager=call<u32>(0x025200D4)+0x5150;
 else{if(u32(room)>=64)call<void>(0x0273AA24,u32(0x10050734),u32(0x1e3),u32(0x10050740));u32 game=call<u32>(0x025200D4);manager=call<u32>(0x025C11DC,game+0x51cc,room);}
 u32 table=load<u32>(manager),info=call_ptr<u32>(load<u32>(table+0x16c),manager);if(!info)return;
 u32 entries=load<u32>(info+4);if(!entries)call<void>(0x0273AA24,u32(0x10050734),u32(0x1ed),u32(0x10050760));
 if(s32(index)>=load<s32>(info))call<void>(0x0273AA24,u32(0x10050734),u32(0x1ee),u32(0x10050708));
 u32 entry=entries+index*12,wipe=load<u8>(entry+0xa)&15;s32 targetroom=load<s8>(entry+9);u32 start=load<u8>(entry+8);
 call<void>(0x0252012C,entry,start,targetroom,s32(-1),u32(0),u32(1),wipe,load<f32>(0x10050690));
}
VERIFY(0x02587EFC,next_stage);
void path_move(u32 pos,u32 point,u32 path,u32 callback,u32 data,f64 speed){
 WWHD_FUNC(0x02587D24,void,pos,point,path,callback,data,speed);
 if(!path)call<void>(0x0273AA24,u32(0x100506e8),u32(0x1b7),u32(0x100506f4));
 u16 count=load<u16>(path);s8 current=load<s8>(point),next=current<s32(count)-1?s8(current+1):0;u32 points=load<u32>(path+8);
 Local<Vec> a,b,diff,normal,move,scaled,result;u32 begin=points+u32(s32(current))*16+4,end=points+u32(s32(next))*16+4;
 for(u32 i=0;i<3;i++)store<f32>(ea(a.get())+i*4,load<f32>(begin+i*4));
 for(u32 i=0;i<3;i++)store<f32>(ea(b.get())+i*4,load<f32>(end+i*4));
 call<void>(0x0201ADE0,b.get(),diff.get(),a.get());call<void>(0x0201B12C,diff.get(),normal.get());copy_vec(ea(move.get()),ea(normal.get()));
 if(callback){if(call_ptr<u32>(callback,pos,a.get(),b.get(),data))store<s8>(point,next);return;}
 call<void>(0x0201AE48,move.get(),scaled.get(),speed);call<void>(0x0201AD78,pos,result.get(),scaled.get());
 u32 x=load<u32>(ea(result.get())),z=load<u32>(ea(result.get())+8);store<u32>(pos,x);u32 y=load<u32>(ea(result.get())+4);store<u32>(pos+8,z);store<u32>(pos+4,y);
 f64 sq=call<f64>(0x028E8DE8,a.get(),pos),distance=call<f64>(0x028F4384,sq);sq=call<f64>(0x028E8DE8,a.get(),b.get());f64 total=call<f64>(0x028F4384,sq);
 if(distance>total){u32 y=load<u32>(ea(b.get())+4);store<s8>(point,next);x=load<u32>(ea(b.get()));z=load<u32>(ea(b.get())+8);store<u32>(pos,x);store<u32>(pos+8,z);store<u32>(pos+4,y);}
}
VERIFY(0x02587D24,path_move);
f64 lerp(f64 a,f64 b,f64 t){WWHD_FUNC(0x02588DEC,f64,a,b,t);f32 delta=f32(b-a);u64 bits;std::memcpy(&bits,&t,8);bits=(bits&0xfffffffff8000000ull)+(bits&0x8000000ull);std::memcpy(&t,&bits,8);return f32(f64(delta)*t+a);}
VERIFY(0x02588DEC,lerp);
u32 days_since_save(){WWHD_FUNC(0x0258839C,u32,(u32)0);u32 game=load<u32>(0x101f84dc),hi=load<u32>(game+0x38),lo=load<u32>(game+0x3c);if(!(hi|lo))return 0;
 u32 nowhi=call<u32>(0xC0009CD8),nowlo=cpu->r[4];game=load<u32>(0x101f84dc);u64 saved=(u64(load<u32>(game+0x38))<<32)|load<u32>(game+0x3c);u64 elapsed=((u64(nowhi)<<32)|nowlo)-saved;
 u32 info=call<u32>(0xC0009C80);u64 ticks=u64(load<u32>(info)>>2)*60*60*24;
 call<void>(0x028F5B8C,u32(elapsed>>32),u32(elapsed),u32(ticks>>32),u32(ticks));return cpu->r[4];
}
VERIFY(0x0258839C,days_since_save);
}
