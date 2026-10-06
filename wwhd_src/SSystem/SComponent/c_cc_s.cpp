/* WWHD collision system, raw verified implementation. */
#include "gabi.h"
using namespace gabi;
namespace ccs_cpp {
static u32 ld(u32 a) { return load<u32>(a); }
static void st(u32 a,u32 x) { store<u32>(a,x); }
static void* p(u32 a) { return at<void>(a); }
static u32 target(u32 obj,u32 vtoff,u32 slot) { return ld(ld(obj+vtoff)+slot); }
static u32 getter(u32 obj,u32 vtoff,u32 slot) { return call_ptr<u32>(target(obj,vtoff,slot),p(obj)); }
struct Aab { be<f32> lo[3],hi[3]; be<u32> vt; };
struct Vec { be<f32> v[3]; };
void clr_at(void* self) {
 WWHD_FUNC(0x0200CD0C,void,self);
 u32 s=ea(self),end=ld(s+0x2800);
 for(s32 i=0;i<(s32)end;i++) {
  u32 o=ld(s+(u32)i*4);
  if(o) {
   u32 inf=getter(o,0x3c,0x1c);
   call_ptr<void>(target(inf,0x3c,0x34),p(inf));
   o=ld(s+(u32)i*4);u32 status=ld(o+0x44);
   if(status) { status=ld(o+0x44);call_ptr<void>(target(status,0x18,0x2c),p(status)); }
   end=ld(s+0x2800);
  }
 }
}
VERIFY(0x0200CD0C,clr_at);
void clr_tg(void* self) {
 WWHD_FUNC(0x0200CDBC,void,self);
 u32 s=ea(self),end=ld(s+0x2804);
 for(s32 i=0;i<(s32)end;i++) {
  u32 o=ld(s+0x400+(u32)i*4);
  if(o) {
   u32 inf=getter(o,0x3c,0x1c);
   call_ptr<void>(target(inf,0x3c,0x3c),p(inf));
   o=ld(s+0x400+(u32)i*4);u32 status=ld(o+0x44);
   if(status) { status=ld(o+0x44);call_ptr<void>(target(status,0x18,0x34),p(status)); }
   end=ld(s+0x2804);
  }
 }
}
VERIFY(0x0200CDBC,clr_tg);
u32 no_at_tg(void* self,void* attack,void* receive) {
 WWHD_FUNC(0x0200CE78,u32,self,attack,receive);
 u32 s=ea(self),a=ea(attack),b=ea(receive),sa=ld(a+0x44),sb=ld(b+0x44);
 u32 ac=sa?ld(sa+12):0,bc=sb?ld(sb+12):0;
 if(ac&&bc&&ac==bc) return 1;
 if(!(ld(a)&ld(b+0x18)&14)) return 1;
 if(!(ld(a+0x10)&ld(b+0x28))) return 1;
 u32 av=ld(a+0x3c),bv=ld(b+0x3c),sv=ld(s+0x2850),sav=ld(sa+0x18),sbv=ld(sb+0x18);
 u32 ai=call_ptr<u32>(ld(av+0x1c),p(a));
 u32 bi=call_ptr<u32>(ld(bv+0x1c),p(b));
 u32 ag=call_ptr<u32>(ld(sav+0x1c),p(sa));
 u32 bg=call_ptr<u32>(ld(sbv+0x1c),p(sb));
 return call_ptr<u32>(ld(sv+0x2c),self,p(ai),p(bi),p(ag),p(bg));
}
VERIFY(0x0200CE78,no_at_tg);
void set_at_tg(void* self,void* attack,void* receive,void* xyz) {
 WWHD_FUNC(0x0200CFA8,void,self,attack,receive,xyz);
 u32 s=ea(self),a=ea(attack),b=ea(receive);
 u32 av=ld(a+0x3c),bv=ld(b+0x3c),sb=ld(b+0x44),sa=ld(a+0x44),sv=ld(s+0x2850),sav=ld(sa+0x18),sbv=ld(sb+0x18);
 u32 ai=call_ptr<u32>(ld(av+0x1c),p(a));
 u32 bi=call_ptr<u32>(ld(bv+0x1c),p(b));
 u32 ag=call_ptr<u32>(ld(sav+0x1c),p(sa));
 u32 bg=call_ptr<u32>(ld(sbv+0x1c),p(sb));
 u32 ba=ld(b+0x18),aa=ld(a);
 if(call_ptr<u32>(ld(sv+0x34),self,((ba>>4)&1)^1,((aa>>4)&1)^1,p(ai),p(bi),p(sa),p(sb),p(ag),p(bg))) return;
 ba=ld(b+0x18);aa=ld(a);
 if(!(ba&16)) {st(a+8,b);st(a+4,1);}
 if(!(aa&16)) {
  st(b+0x20,a);st(b+0x1c,1);
  call_ptr<void>(target(s,0x2850,12),self,attack,receive,p(sa),p(sb));
  aa=ld(a);
 }
 av=ld(a+0x3c);sbv=ld(sb+0x18);sav=ld(sa+0x18);bv=ld(b+0x3c);sv=ld(s+0x2850);
 ai=call_ptr<u32>(ld(av+0x1c),p(a));
 bi=call_ptr<u32>(ld(bv+0x1c),p(b));
 ag=call_ptr<u32>(ld(sav+0x1c),p(sa));
 bg=call_ptr<u32>(ld(sbv+0x1c),p(sb));
 ba=ld(b+0x18);
 call_ptr<void>(ld(sv+0x24),self,((ba>>4)&1)^1,((aa>>4)&1)^1,attack,receive,p(ai),p(bi),p(sa),p(sb),p(ag),p(bg),xyz);
}
VERIFY(0x0200CFA8,set_at_tg);
void check_at_tg(void* self) {
 WWHD_FUNC(0x0200D19C,void,self);
 u32 s=ea(self),tgbase=s+0x400,tgend=tgbase+(ld(s+0x2804)<<2);
 call<void>(0x0200CD0C,self);call<void>(0x0200CDBC,self);
 u32 at=s,end=s+(ld(s+0x2800)<<2);
 f32 zero=load<f32>(0x1000181c);
 for(;at<end;at+=4) {
  u32 a=ld(at);
  if(!a||!(ld(a)&1)) continue;
  u32 ash=getter(a,0x3c,0x2c);
  if(ash) for(u32 tg=tgbase;tg<tgend;tg+=4) {
   u32 b=ld(tg);if(!b||!(ld(b+0x18)&1)) continue;
   if(!call<u32>(0x0200B71C,p(ld(at)+0x48),p(b+0x48))) continue;
   b=ld(tg);a=ld(at);
   if(call<u32>(0x0200CE78,self,p(a),p(b))) continue;
   b=ld(tg);u32 bsh=getter(b,0x3c,0x2c);if(!bsh) continue;
   if(!ld(0x101ff538)) st(0x101ff538,1);
   u32 cross=0x101ff52c;
   u32 hit=call_ptr<u32>(target(ash,0x1c,0x14),p(ash),p(bsh),p(cross));
   a=ld(at);bool rev=(ld(a+0x40)&2)!=0;
   if(!rev){b=ld(tg);rev=(ld(b+0x40)&2)!=0;}
   if(!rev) {if(!hit)continue;}
   else {
    if(hit)continue;
    u32 sh=getter(a,0x3c,0x2c);
    if(!sh){store<f32>(cross+8,zero);store<f32>(cross,zero);store<f32>(cross+4,zero);}
    else call<void>(0x02017F0C,p(sh),p(cross));
    b=ld(tg);a=ld(at);
   }
   call<void>(0x0200CFA8,self,p(a),p(b),p(cross));
  }
  end=s+(ld(s+0x2800)<<2);
 }
}
VERIFY(0x0200D19C,check_at_tg);
void clr_co(void* self) {
 WWHD_FUNC(0x0200D3B8,void,self);
 u32 s=ea(self),end=ld(s+0x2808);f32 zero=load<f32>(0x1000181c);
 for(s32 i=0;i<(s32)end;i++) {
  u32 o=ld(s+0x1000+(u32)i*4);
  if(o) {
   u32 inf=getter(o,0x3c,0x1c);call_ptr<void>(target(inf,0x3c,0x44),p(inf));
   o=ld(s+0x1000+(u32)i*4);u32 status=ld(o+0x44);
   if(status){status=ld(o+0x44);store<f32>(status+8,zero);store<f32>(status,zero);store<f32>(status+4,zero);}
   end=ld(s+0x2808);
  }
 }
}
VERIFY(0x0200D3B8,clr_co);
u32 no_co(void* self,void* first,void* second) {
 WWHD_FUNC(0x0200D490,u32,self,first,second);
 u32 a=ea(first),b=ea(second),sa=ld(a+0x44),sb=ld(b+0x44);
 u32 ac=sa?ld(sa+12):0,bc=sb?ld(sb+12):0;
 if(ac&&bc&&ac==bc)return 1;
 u32 af=ld(a+0x2c),bf=ld(b+0x2c);
 if(!(af&((bf>>3)&14)&14))return 1;
 if(!(((af>>3)&14)&bf))return 1;
 return call_ptr<u32>(target(ea(self),0x2850,0x3c),self,first,second)!=0;
}
VERIFY(0x0200D490,no_co);
void set_co(void* self,void* first,void* pos1,void* second,void* pos2,f32 distance) {
 WWHD_FUNC(0x0200D54C,void,self,first,pos1,second,pos2,distance);
 u32 s=ea(self),a=ea(first),b=ea(second);
 u32 bf=ld(b+0x2c),af=ld(a+0x2c),ba=((bf>>9)&1)^1,aa=((af>>9)&1)^1;
 if(ba){st(a+0x34,b);st(a+0x30,1);}
 if(aa){st(b+0x34,a);st(b+0x30,1);}
 if(ba&&aa)call_ptr<void>(target(s,0x2850,0x14),self,first,pos1,second,pos2,distance);
 u32 av=ld(a+0x3c),sb=ld(b+0x44),sa=ld(a+0x44),bv=ld(b+0x3c),sav=ld(sa+0x18),sv=ld(s+0x2850),sbv=ld(sb+0x18);
 u32 ai=call_ptr<u32>(ld(av+0x1c),first);
 u32 bi=call_ptr<u32>(ld(bv+0x1c),second);
 u32 ag=call_ptr<u32>(ld(sav+0x1c),p(sa));
 u32 bg=call_ptr<u32>(ld(sbv+0x1c),p(sb));
 call_ptr<void>(ld(sv+0x1c),self,ba,aa,p(ai),p(bi),p(sa),p(sb),p(ag),p(bg));
}
VERIFY(0x0200D54C,set_co);
void check_co(void* self) {
 WWHD_FUNC(0x0200D684,void,self);
 u32 s=ea(self);call<void>(0x0200D3B8,self);u32 count=ld(s+0x2808);if((s32)count<=1)return;
 u32 begin=s+0x1000,end=begin+(count<<2);f32 zero=load<f32>(0x1000181c);
 Local<be<f32>> distance;
 for(u32 at=begin;at<end-4;at+=4) {
  u32 a=ld(at);if(!a||!(ld(a+0x2c)&1))continue;
  u32 ash=getter(a,0x3c,0x2c);if(!ash)continue;
  for(u32 tg=at+4;tg<end;tg+=4) {
   u32 b=ld(tg);if(!b||!(ld(b+0x2c)&1))continue;
   if(!call<u32>(0x0200B71C,p(ld(at)+0x48),p(b+0x48)))continue;
   b=ld(tg);a=ld(at);if(call<u32>(0x0200D490,self,p(a),p(b)))continue;
   b=ld(tg);u32 bsh=getter(b,0x3c,0x2c);if(!bsh)continue;
   *distance.get()=zero;
   if(!call_ptr<u32>(target(ash,0x1c,0x4c),p(ash),p(bsh),distance.get()))continue;
   u32 av=ld(ash+0x1c),bv=ld(bsh+0x1c);
   u32 ap=call_ptr<u32>(ld(av+0x8c),p(ash));
   u32 bp=call_ptr<u32>(ld(bv+0x8c),p(bsh));
   f32 d=*distance.get();b=ld(tg);a=ld(at);
   call<void>(0x0200D54C,self,p(a),p(ap),p(b),p(bp),d);
  }
 }
}
VERIFY(0x0200D684,check_co);
void plus_damage(void* self,void* attack,void* receive,void* status1,void* status2) {
 WWHD_FUNC(0x0200D834,void,self,attack,receive,status1,status2);
 u8 power=load<u8>(ea(attack)+0x14),dmg=load<u8>(ea(status2)+0x16);
 if(dmg<power)store<u8>(ea(status2)+0x16,power);
}
VERIFY(0x0200D834,plus_damage);
static void assertion(u32 line,u32 text){call<void>(0x0273AA24,p(0x10001870),line,p(text));}
static bool nan_bits(f32 x){u32 bits;memcpy(&bits,&x,4);return (bits<<1)>0xff000000u;}
static bool range_bad(u32 v,f32 lo,f32 hi){
 f32 x=load<f32>(v);if(!(lo<x&&x<hi))return true;
 f32 y=load<f32>(v+4);if(!(lo<y&&y<hi))return true;
 f32 z=load<f32>(v+8);return !(lo<z&&z<hi);
}
void pos_correct(void* self,void* first,void* pos1,void* second,void* pos2,f32 distance){
 WWHD_FUNC(0x0200D84C,void,self,first,pos1,second,pos2,distance);
 u32 s=ea(self),a=ea(first),b=ea(second),p1=ea(pos1),p2=ea(pos2);
 if(nan_bits(distance))assertion(0x26d,0x100019fc);
 f32 lo=load<f32>(0x10001824),hi=load<f32>(0x10001820);
 if(!(lo<distance&&distance<hi))assertion(0x26e,0x10001844);
 if(ld(a+0x2c)&0x100)return;
 if(ld(b+0x2c)&0x100)return;
 u32 sa=ld(a+0x44);if(!sa)return;u32 sb=ld(b+0x44);if(!sb)return;
 if(ld(sa+12)&&ld(sa+12)==ld(sb+12))return;
 f32 absolute=std::fabs(distance);if(absolute<load<f32>(0x10001828))return;
 call_ptr<void>(target(s,0x2850,0x54),self,first,second);
 u32 af=ld(a+0x2c);sa=ld(a+0x44);u32 wa=load<u8>(sa+0x14);
 bool correct_y=false;if(af&0x80)correct_y=(ld(b+0x2c)&0x80)!=0;
 sb=ld(b+0x44);u32 wb=load<u8>(sb+0x14);
 u32 ta=wa==255?0:wa==254?1:2,tb=wb==255?0:wb==254?1:2;
 f32 fa=(f32)load<u8>(sa+0x14),fb=(f32)load<u8>(sb+0x14),total=fa+fb;
 f32 tiny=load<f32>(0x100030b8),one=load<f32>(0x10001838);
 if(std::fabs(total)<tiny){fa=one;fb=one;total=load<f32>(0x1000183c);}
 f32 inverse=one/total,zero=load<f32>(0x1000181c),push_a,push_b;
 if(ta==0){if(tb==0)return;push_a=zero;push_b=one;}
 else if(ta==1){if(tb==0){push_a=one;push_b=zero;}else if(tb==1){push_a=load<f32>(0x10001840);push_b=push_a;}else{push_a=zero;push_b=one;}}
 else{if(tb==2){push_b=fa*inverse;push_a=fb*inverse;}else{push_a=one;push_b=zero;}}
 f32 dx=load<f32>(p2)-load<f32>(p1),dz=load<f32>(p2+8)-load<f32>(p1+8),dy=zero,squared;
 if(correct_y){dy=load<f32>(p2+4)-load<f32>(p1+4);squared=fmadds(dz,dz,fmadds(dx,dx,dy*dy));}
 else squared=fmadds(dx,dx,dz*dz);
 f64 len=call<f64>(0x028F4384,(f64)squared);
 Local<Vec> vec_a,vec_b;u32 va=ea(vec_a.get()),vb=ea(vec_b.get());
 if(!(std::fabs(len)<tiny)){
  f32 factor=(f32)((f64)distance/len);dx*=factor;dz*=factor;
  store<f32>(vb,dx*push_b);store<f32>(va,-(dx*push_a));store<f32>(va+8,-(dz*push_a));store<f32>(vb+8,dz*push_b);
  if(correct_y){dy*=factor;store<f32>(va+4,-(dy*push_a));store<f32>(vb+4,dy*push_b);}
  else{store<f32>(vb+4,zero);store<f32>(va+4,zero);}
 }else{
  store<f32>(vb+4,zero);store<f32>(va+8,zero);store<f32>(vb+8,zero);store<f32>(va+4,zero);
  if(!(absolute<tiny)){store<f32>(va,-(distance*push_a));store<f32>(vb,distance*push_b);}
  else{store<f32>(vb,push_b);store<f32>(va,-push_a);}
 }
 if(nan_bits(load<f32>(va)))assertion(0x32b,0x1000187c);
 if(nan_bits(load<f32>(va+4)))assertion(0x32c,0x100018bc);
 if(nan_bits(load<f32>(va+8)))assertion(0x32d,0x100018fc);
 if(nan_bits(load<f32>(vb)))assertion(0x32f,0x1000193c);
 if(nan_bits(load<f32>(vb+4)))assertion(0x330,0x1000197c);
 if(nan_bits(load<f32>(vb+8)))assertion(0x331,0x100019bc);
 if(range_bad(va,lo,hi))assertion(0x333,0x10001a40);
 if(range_bad(vb,lo,hi))assertion(0x337,0x10001ab8);
 call<void>(0x0200BE28,p(ld(a+0x44)),load<f32>(va),load<f32>(va+4),load<f32>(va+8));
 call<void>(0x0200BE28,p(ld(b+0x44)),load<f32>(vb),load<f32>(vb+4),load<f32>(vb+8));
 call<void>(0x028E8D88,pos1,vec_a.get(),pos1);
 call<void>(0x028E8D88,pos2,vec_b.get(),pos2);
 if(nan_bits(load<f32>(p1)))assertion(0x342,0x10001b30);
 if(nan_bits(load<f32>(p1+4)))assertion(0x343,0x10001b74);
 if(nan_bits(load<f32>(p1+8)))assertion(0x344,0x10001bb8);
 if(nan_bits(load<f32>(p2)))assertion(0x346,0x10001bfc);
 if(nan_bits(load<f32>(p2+4)))assertion(0x347,0x10001c40);
 if(nan_bits(load<f32>(p2+8)))assertion(0x348,0x10001c84);
 if(range_bad(p1,lo,hi))assertion(0x34a,0x10001cc8);
 if(range_bad(p2,lo,hi))assertion(0x34e,0x10001d4c);
}
VERIFY(0x0200D84C,pos_correct);
void* ctor(void* self) {
 WWHD_FUNC(0x0200E138,void*,self);
 u32 s=ea(self);if(!s)s=call<u32>(0x0273AD10,0x2854u);
 if(s){
  st(s+0x2850,0x10001dd8);
  call<void>(0x028F521C,p(s+0x400),0xc00u);
  call<void>(0x028F521C,p(s+0x1000),0x400u);
  call<void>(0x028F521C,p(s+0x1400),0x1400u);
  st(s+0x2800,0);st(s+0x2804,0);st(s+0x2808,0);st(s+0x280c,0);
  call<void>(0x0200B750,p(s+0x2810));
 }
 return p(s);
}
VERIFY(0x0200E138,ctor);
void ct(void* self) {
 WWHD_FUNC(0x0200E1C4,void,self);
 u32 s=ea(self);
 for(u32 i=0;i<0x400;i+=4)st(s+i,0);
 for(u32 i=0x400;i<0x1000;i+=4)st(s+i,0);
 for(u32 i=0x1000;i<0x1400;i+=4)st(s+i,0);
 for(u32 i=0x1400;i<0x2800;i+=4)st(s+i,0);
 st(s+0x2800,0);st(s+0x280c,0);st(s+0x2804,0);st(s+0x2808,0);
}
VERIFY(0x0200E1C4,ct);
void dt(void* self) {WWHD_FUNC(0x0200E23C,void,self);call<void>(0x0200E1C4,self);}
VERIFY(0x0200E23C,dt);
void set(void* self,void* object) {
 WWHD_FUNC(0x0200E240,void,self,object);
 u32 s=ea(self),o=ea(object);
 if(ld(o)&1){u32 n=ld(s+0x2800);if((s32)n<0x100){st(s+(n<<2),o);st(s+0x2800,ld(s+0x2800)+1);}}
 if(ld(o+0x18)&1){u32 n=ld(s+0x2804);if((s32)n<0x300){st(s+0x400+(n<<2),o);st(s+0x2804,ld(s+0x2804)+1);}}
 if(ld(o+0x2c)&1){u32 n=ld(s+0x2808);if((s32)n<0x100){st(s+0x1000+(n<<2),o);st(s+0x2808,ld(s+0x2808)+1);}}
 u32 n=ld(s+0x280c);if((s32)n<0x500){st(s+0x1400+(n<<2),o);st(s+0x280c,ld(s+0x280c)+1);}
}
VERIFY(0x0200E240,set);
void remove(void* self,void* object) {
 WWHD_FUNC(0x0200E2F4,void,self,object);
 u32 s=ea(self),o=ea(object);
 for(u32 i=s;i<s+(ld(s+0x2800)<<2);i+=4)if(ld(i)==o)st(i,0);
 for(u32 i=s+0x400;i<s+0x400+(ld(s+0x2804)<<2);i+=4)if(ld(i)==o)st(i,0);
 for(u32 i=s+0x1000;i<s+0x1000+(ld(s+0x2808)<<2);i+=4)if(ld(i)==o)st(i,0);
 for(u32 i=s+0x1400;i<s+0x1400+(ld(s+0x280c)<<2);i+=4)if(ld(i)==o)st(i,0);
}
VERIFY(0x0200E2F4,remove);
void area(void* self) {
 WWHD_FUNC(0x0200E414,void,self);
 u32 s=ea(self);Local<Aab> aab;st(ea(aab.get())+24,0x1000180c);call<void>(0x02017DAC,aab.get());
 u32 base=s+0x1400,end=base+(ld(s+0x280c)<<2);
 for(u32 i=base;i<end;i+=4){
  u32 o=ld(i);if(!o)continue;
  u32 shape=getter(o,0x3c,0x2c);
  if(shape){call_ptr<void>(target(shape,0x1c,0x94),p(shape));call<void>(0x02017E98,aab.get(),p(shape));}
  end=base+(ld(s+0x280c)<<2);
 }
 call<void>(0x0200B7D4,p(s+0x2810),aab.get());
 end=base+(ld(s+0x280c)<<2);
 for(u32 i=base;i<end;i+=4){
  u32 o=ld(i);if(!o)continue;
  u32 shape=getter(o,0x3c,0x2c);
  if(shape){o=ld(i);u32 flag=ld(o+0x40)&2;call<void>(0x0200B8CC,p(s+0x2810),p(o+0x48),p(shape),flag);}
  end=base+(ld(s+0x280c)<<2);
 }
}
VERIFY(0x0200E414,area);
void move(void* self) {
 WWHD_FUNC(0x0200E558,void,self);
 call<void>(0x0200E414,self);call<void>(0x0200D19C,self);call<void>(0x0200D684,self);
 u32 s=ea(self);call_ptr<void>(target(s,0x2850,0x4c),self);
 st(s+0x2800,0);st(s+0x280c,0);st(s+0x2804,0);st(s+0x2808,0);
}
VERIFY(0x0200E558,move);
void draw_clear(void* self) {
 WWHD_FUNC(0x0200E5BC,void,self);
 u32 s=ea(self);
 for(u32 i=0;i<0x400;i+=4)st(s+i,0);st(s+0x2800,0);
 for(u32 i=0x400;i<0x1000;i+=4)st(s+i,0);st(s+0x2804,0);
 for(u32 i=0x1000;i<0x1400;i+=4)st(s+i,0);st(s+0x2808,0);
 for(u32 i=0x1400;i<0x2800;i+=4)st(s+i,0);st(s+0x280c,0);
}
VERIFY(0x0200E5BC,draw_clear);
void correct_proc(void* self,void* first,void* second){WWHD_FUNC(0x0200E630,void,self,first,second);}
VERIFY(0x0200E630,correct_proc);
void sinit(){
 WWHD_FUNC(0x0200E634,void);
 st(0x101ff524,0);st(0x101ff51c,0);st(0x101ff528,0);st(0x101ff520,0);
 call<void>(0x028F026C,p(0x1018c6b4));f32 a=load<f32>(0x10001dd0),b=load<f32>(0x10001dd4);
 store<f32>(0x101ff510,a);store<f32>(0x101ff514,b);
 call<void>(0x028ED6F8,p(0x101ff518));call<void>(0x028F026C,p(0x1018c6c0));
 call<void>(0x028EAB2C,p(0x101ff519));call<void>(0x028F026C,p(0x1018c6cc));
}
VERIFY(0x0200E634,sinit);
void co_ginfo(void* self,u32 a,u32 b,void* i1,void* i2,void* s1,void* s2,void* g1,void* g2){WWHD_FUNC(0x0200E6C8,void,self,a,b,i1,i2,s1,s2,g1,g2);}
VERIFY(0x0200E6C8,co_ginfo);
void at_tg_ginfo(void* self,u32 a,u32 b,void* o1,void* o2,void* i1,void* i2,void* s1,void* s2,void* g1,void* g2,void* xyz){WWHD_FUNC(0x0200E6CC,void,self,a,b,o1,o2,i1,i2,s1,s2,g1,g2,xyz);}
VERIFY(0x0200E6CC,at_tg_ginfo);
u32 no_g_at_tg(void* self,void* i1,void* i2,void* g1,void* g2){WWHD_FUNC(0x0200E6D0,u32,self,i1,i2,g1,g2);return 0;}
VERIFY(0x0200E6D0,no_g_at_tg);
u32 hit_after(void* self,u32 a,u32 b,void* i1,void* i2,void* s1,void* s2,void* g1,void* g2){WWHD_FUNC(0x0200E6D8,u32,self,a,b,i1,i2,s1,s2,g1,g2);return 0;}
VERIFY(0x0200E6D8,hit_after);
u32 no_g_co(void* self,void* first,void* second){WWHD_FUNC(0x0200E6E0,u32,self,first,second);return 0;}
VERIFY(0x0200E6E0,no_g_co);
void move_after(void* self){WWHD_FUNC(0x0200E6E8,void,self);}
VERIFY(0x0200E6E8,move_after);
}
