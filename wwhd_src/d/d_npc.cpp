#include "bindings.h"
namespace {
inline u32 ld(u32 p){return gabi::load<u32>(p);} inline f32 lf(u32 p){return gabi::load<f32>(p);}
inline void sw(u32 p,u32 v){gabi::store<u32>(p,v);} inline void sf(u32 p,f32 v){gabi::store<f32>(p,v);}
inline void sh(u32 p,u16 v){gabi::store<u16>(p,v);} inline s16 lh(u32 p){return gabi::load<s16>(p);}
struct NpcString {be<u32> data,vt;};
struct Vec {be<f32> x,y,z;};
inline u32 lookup(u32 target,u32 arc,s32 id){gabi::Local<NpcString> key;key->data=arc;key->vt=0x10051554;return gabi::call<u32>(target,ld(0x101F4F28),key.get(),id);}
inline void setMorph(u32 self,u32 anim,u32 loop,u32 sound,f32 blend,f32 speed){gabi::call(0x025E4A98,self,anim,loop,sound,blend,speed,lf(0x100515BC),lf(0x100515C0));}
}
static BOOL setAnmID(u32 self,u32 loop,f32 blend,f32 speed,s32 anim,s32 sound,u32 arc){
 WWHD_FUNC(0x0259D24C,BOOL,self,loop,blend,speed,anim,sound,arc);
 if(!self)return FALSE;u32 snd=0;if(sound>=0)snd=lookup(0x026067F4,arc,sound);
 u32 an=lookup(0x026067F4,arc,anim);setMorph(self,an,loop,snd,blend,speed);return TRUE;
}
VERIFY(0x0259D24C,setAnmID);
static BOOL setAnmNames(u32 self,u32 loop,f32 blend,f32 speed,u32 anim,u32 sound,u32 arc){
 WWHD_FUNC(0x0259D344,BOOL,self,loop,blend,speed,anim,sound,arc);
 if(!self||!anim||!arc)return FALSE;
 gabi::Local<NpcString> a,b;a->data=arc;a->vt=0x10051554;b->data=anim;b->vt=0x10051554;
 u32 an=gabi::call<u32>(0x02606900,ld(0x101F4F28),a.get(),b.get());u32 snd=0;
 if(sound){gabi::Local<NpcString> c,d;c->data=arc;c->vt=0x10051554;d->data=sound;d->vt=0x10051554;snd=gabi::call<u32>(0x02606900,ld(0x101F4F28),c.get(),d.get());}
 setMorph(self,an,loop,snd,blend,speed);return TRUE;
}
VERIFY(0x0259D344,setAnmNames);
static BOOL setAnm(u32 self,u32 loop,f32 blend,f32 speed,s32 anim,s32 sound,u32 arc){
 WWHD_FUNC(0x0259D454,BOOL,self,loop,blend,speed,anim,sound,arc);
 if(!self)return FALSE;u32 snd=0;if(sound>=0)snd=lookup(0x026066C4,arc,sound);
 u32 an=lookup(0x026066C4,arc,anim);setMorph(self,an,loop,snd,blend,speed);return TRUE;
}
VERIFY(0x0259D454,setAnm);
static u32 playerEye(u32 result,f32 offset){
 WWHD_FUNC(0x0259D54C,u32,result,offset);
 u32 play=gabi::call<u32>(0x025200D4);u32 player=ld(play+0x5B34);
 gabi::Local<Vec> input,output;input->x=lf(player+0x3D8);input->y=lf(player+0x3DC);input->z=lf(player+0x3E0);
 gabi::call(0x0200FAD8,(f32)input->x,(f32)input->y,(f32)input->z,0u);input->x=lf(0x100515BC);input->y=offset;input->z=lf(0x100515BC);
 gabi::call(0x0200FCD8,input.get(),output.get());output->x=lf(player+0x314);output->z=lf(player+0x31C);
 u32 dest=result;if(!dest)dest=gabi::call<u32>(0x0273AD10,12u);
 if(dest){sf(dest,(f32)output->x);sf(dest+4,(f32)output->y);sf(dest+8,(f32)output->z);}return dest;
}
VERIFY(0x0259D54C,playerEye);
static void distanceAngle(u32 a,u32 b,u32 distance,u32 angle){
 WWHD_FUNC(0x0259D624,void,a,b,distance,angle);
 f32 dx=gabi::fsubs_ppc(lf(b),lf(a));f32 dz=gabi::fsubs_ppc(lf(b+8),lf(a+8));
 if(distance)sf(distance,gabi::call<f32>(0x028F4384,gabi::fmadds(dx,dx,gabi::fmuls_ppc(dz,dz))));
 if(angle)sh(angle,gabi::call<u32>(0x020195B0,dx,dz));
}
VERIFY(0x0259D624,distanceAngle);
static BOOL chkArasoi(){
 WWHD_FUNC(0x0259D6C8,BOOL);
 if(!gabi::call<u32>(0x025B8B94,ld(0x101F84DC)+0x644,0x1220u))return FALSE;
 return !gabi::call<u32>(0x025B8B94,ld(0x101F84DC)+0x644,0x1808u);
}
VERIFY(0x0259D6C8,chkArasoi);
static BOOL chkLetter(){
 WWHD_FUNC(0x0259D734,BOOL);
 if(!gabi::call<u32>(0x025B7840,ld(0x101F84DC)+0xB0,12u))return FALSE;
 return !gabi::call<u32>(0x025B7570,ld(0x101F84DC)+0x96,0x98u);
}
VERIFY(0x0259D734,chkLetter);
static BOOL setAnm2(u32 self,u32 loop,f32 blend,f32 speed,s32 anim,s32 sound,u32 arc){
 WWHD_FUNC(0x0259D79C,BOOL,self,loop,blend,speed,anim,sound,arc);
 u32 an,snd=0;
 if(sound>=0){an=lookup(0x026066C4,arc,anim);snd=lookup(0x026066C4,arc,sound);}else an=lookup(0x026066C4,arc,anim);
 setMorph(self,an,loop,snd,blend,speed);return TRUE;
}
VERIFY(0x0259D79C,setAnm2);
static BOOL chkAttention(u32 actor,u32 target,u32 extended,f32 range,f32 extension,f32 angle){
 WWHD_FUNC(0x0259D8BC,BOOL,actor,target,extended,range,extension,angle);
 s16 facing=(s16)gabi::call<u32>(0x0200F93C,actor+0x314,target);
 gabi::Local<Vec> delta,flat;gabi::call(0x0201ADE0,actor+0x314,delta.get(),target);
 flat->x=delta->x;flat->y=lf(0x100515BC);flat->z=delta->z;
 f32 length=gabi::call<f32>(0x028E8DD0,flat.get());f32 dist=gabi::call<f32>(0x028F4384,length);
 f32 limit=lf(0x100515D8);f32 diff=gabi::fsubs_ppc(angle,limit);angle=diff>=0?limit:angle;
 f32 zero=lf(0x100515BC);angle=angle>=0?angle:zero;
 s16 yaw=(s16)((u16)facing-(u16)lh(actor+0x322));if(extended)range=gabi::fadds_ppc(range,extension);
 if(dist>range)return FALSE;
 s16 threshold=(s16)gabi::ftoi(gabi::fmuls_ppc(angle,lf(0x100515DC)));s32 absolute=yaw<0?-(s32)yaw:(s32)yaw;
 return absolute<=threshold;
}
VERIFY(0x0259D8BC,chkAttention);
static u32 headCtor(u32 self){
 WWHD_FUNC(0x0259DA18,u32,self);
 if(!self)self=gabi::call<u32>(0x0273AD10,0x28u);if(!self)return 0;
 f32 zero=lf(0x100515BC);sh(self+8,0);sf(self+0x18,zero);sw(self+0x24,0x100515AC);sf(self,zero);
 for(u32 off:{0x10u,0x1Cu,0xAu,0xCu,0x14u,0x6u,0x16u,0xEu,0x12u,0x4u})sh(self+off,0);
 gabi::store<u8>(self+0x1E,0);sf(self+0x20,zero);return self;
}
VERIFY(0x0259DA18,headCtor);
static u32 jointCtor(u32 self){
 WWHD_FUNC(0x0259DAA0,u32,self);
 if(!self)self=gabi::call<u32>(0x0273AD10,0x34u);if(!self)return 0;
 gabi::call(0x028F521C,self,8u);for(u32 off:{0xAu,0xCu,0x9u,0x8u,0xBu})gabi::store<u8>(self+off,0);
 for(u32 off:{0xEu,0x16u,0x1Eu})gabi::call(0x028F521C,self+off,8u);
 sw(self+0x28,0);gabi::call(0x028F521C,self+0x2C,8u);gabi::store<u8>(self+0xB,0);gabi::store<u8>(self+0xC,0);return self;
}
VERIFY(0x0259DAA0,jointCtor);
static void limitJoint(u32 self,u32 angle,s32 upper,s32 lower){
 WWHD_FUNC(0x0259DB48,void,self,angle,upper,lower);
 s32 value=lh(angle);if(value>upper){value=upper;sh(angle,value);}if(value<lower)sh(angle,lower);
}
VERIFY(0x0259DB48,limitJoint);
static BOOL jointMove(u32 self,s32 requested,u32 axis){
 WWHD_FUNC(0x0259DB6C,BOOL,self,requested,axis);
 u32 base=self+axis*2u;gabi::Local<be<s16>[2]> targets;
 for(u32 i=0;i<2;i++){
  u32 p=base+i*4;bool locked=gabi::load<u8>(self+(i?0xC:0xB))!=0;
  gabi::Local<be<s16>> target;*target=locked?0:(s16)requested;
  gabi::call(0x0259DB48,self,target.get(),(s32)lh(p+0x16),(s32)lh(p+0xE));
  s16 selected=*target;requested=(s16)((u32)requested-(u32)(s32)selected);(*targets)[i]=selected;
  gabi::Local<be<s32>> moving;*moving=lh(p);gabi::call(0x0200F474,moving.get(),(s32)selected,4,(s32)lh(p+0x1E),1);sh(p,(s32)*moving);
 }
 u32 reached=0;for(u32 i=0;i<2;i++){
  s32 difference=gabi::call<s32>(0x0200FAAC,(s32)(s16)(*targets)[i],(s32)lh(base+i*4));
  if(difference<=(s32)lh(base+i*4+0x1E)/2)reached++;
 }return reached==2;
}
VERIFY(0x0259DB6C,jointMove);
static BOOL calcAngle(u32 self,u32 angle,s32 goal,s32 minimum,s32 maximum){
 WWHD_FUNC(0x0259DCEC,BOOL,self,angle,goal,minimum,maximum);
 s16 oldDelta=(s16)((u32)(s32)lh(angle)-(u32)goal);gabi::Local<be<s16>> delta;*delta=oldDelta;
 gabi::call(0x0200F378,delta.get(),0,minimum,maximum,0x60);s32 change=(s16)*delta;s32 absolute=change<0?-change:change;
 if(absolute>minimum){s16 value=(s16)((u32)(s32)lh(angle)+(u32)(change-(s32)oldDelta));sh(angle,value);return (s32)value==goal;}
 sh(angle,goal);return TRUE;
}
VERIFY(0x0259DCEC,calcAngle);
static BOOL jointFollow(u32 self,u32 angle,s32 goal,s32 maxStep,u32 axis){
 WWHD_FUNC(0x0259DDA0,BOOL,self,angle,goal,maxStep,axis);
 s16 before=lh(angle);gabi::call<u32>(0x0259DCEC,self,angle,goal,4,maxStep);
 s16 after=lh(angle);s16 change=(s16)((u32)(s32)after-(u32)(s32)before);s16 remaining=(s16)((u32)goal-(u32)(s32)after);
 s16 accumulated=0,running=0;u32 offset=axis*2+4;
 for(u32 i=0;i<2;i++,offset-=4){
  s16 current=lh(self+offset);running=(s16)((u32)(s32)running+(u32)(s32)current);
  if(change<0?running<remaining:(change>0&&running>remaining)){
   sh(self+offset,(u32)(s32)remaining-(u32)(s32)accumulated);
   gabi::call(0x0259DB48,self,self+offset,(s32)lh(self+offset+0x16),(s32)lh(self+offset+0xE));current=lh(self+offset);
  }
  accumulated=(s16)((u32)(s32)accumulated+(u32)(s32)current);
 }
 return goal!=(s32)lh(angle);
}
VERIFY(0x0259DDA0,jointFollow);
static void lookAt(u32 self,u32 angle,u32 target,u32 eye,s32 yaw,s32 velocity,u32 headOnly){
 WWHD_FUNC(0x0259DED0,void,self,angle,target,eye,yaw,velocity,headOnly);
 s32 pitch=0;
 if(target){
  f32 dz=gabi::fsubs_ppc(lf(target+8),lf(eye+8));f32 dx=gabi::fsubs_ppc(lf(target),lf(eye));
  f32 dy=gabi::fsubs_ppc(lf(target+4),lf(eye+4));f32 range=gabi::call<f32>(0x028F4384,gabi::fmadds(dx,dx,gabi::fmuls_ppc(dz,dz)));
  yaw=gabi::call<s32>(0x020195B0,dx,dz);pitch=gabi::call<s32>(0x020195B0,dy,range);
 }
 s16 relative=(s16)((u32)yaw-(u32)(s32)lh(angle));s32 lower=(s32)lh(self+0x1C)+(s32)lh(self+0x18);s32 upper=(s32)lh(self+0x14)+(s32)lh(self+0x10);
 u32 moved=gabi::call<u32>(0x0259DB6C,self,(s32)relative,1);
 bool follow=gabi::load<u8>(self+0xA)!=0;
 if(!follow)follow=!(relative<lower&&relative>upper)&&moved!=0;
 if(follow&&!headOnly)gabi::store<u8>(self+0xA,gabi::call<u32>(0x0259DDA0,self,angle,yaw,velocity,1));
 gabi::call(0x0259DB6C,self,pitch,0);
}
VERIFY(0x0259DED0,lookAt);
static void jointParams(u32 self,s32 a,s32 b,s32 c,s32 d,s32 e,s32 f,s32 g){
 WWHD_FUNC(0x0259E08C,void,self,a,b,c,d,e,f,g);
 // The final two GHS integer arguments reside in caller stack slots, not r11/r12.
 s32 h=lh(gabi::cpu->r[1]+10),i=lh(gabi::cpu->r[1]+14);
 sh(self+0xE,g);sh(self+0x24,i);sh(self+0x14,d);sh(self+0x22,i);sh(self+0x16,e);sh(self+0x1E,i);sh(self+0x1C,b);sh(self+0x20,i);
 sh(self+0x10,h);sh(self+0x1A,a);sh(self+0x12,c);sh(self+0x18,f);
}
VERIFY(0x0259E08C,jointParams);
static s16 limitComponent(u32 self,s32 value,u32 joint,u32 axis){
 WWHD_FUNC(0x0259E0C8,s16,self,value,joint,axis);
 u32 p=self+(joint*2+axis)*2;s32 upper=lh(p+0x16);if(upper<value)value=upper;value=(s16)value;s32 lower=lh(p+0xE);if(lower>value)value=lower;return (s16)value;
}
VERIFY(0x0259E0C8,limitComponent);
static void distribute1(u32 self,s32 requested,u32 head,u32 back,u32 direction){
 WWHD_FUNC(0x0259E104,void,self,requested,head,back,direction);
 sh(back,0);
 if(!gabi::load<u8>(self+0xC)){
  s32 value=gabi::call<s32>(0x0259E0C8,self,requested,1,1);sh(back,value);
  if(lh(self+0x32)&&((direction&&value<0)||(!direction&&value>0)))sh(back,0);
 }
 sh(head,0);if(!gabi::load<u8>(self+0xB)){
  s16 remainder=(s16)((u32)requested-(u32)(s32)lh(back));sh(head,remainder);sh(head,gabi::call<u32>(0x0259E0C8,self,(s32)remainder,0,1));
 }
}
VERIFY(0x0259E104,distribute1);
static void distribute2(u32 self,s32 requested,u32 head,u32 back){
 WWHD_FUNC(0x0259E1DC,void,self,requested,head,back);
 sh(head,0);if(!gabi::load<u8>(self+0xB)){
  s16 value=(s16)((u32)requested-(u32)(s32)lh(self+0x32));sh(head,value);sh(head,gabi::call<u32>(0x0259E0C8,self,(s32)value,0,1));
 }
 sh(back,0);if(!gabi::load<u8>(self+0xC)){
  s16 value=(s16)((u32)requested-(u32)(s32)lh(head));sh(back,value);sh(back,gabi::call<u32>(0x0259E0C8,self,(s32)value,1,1));
 }
}
VERIFY(0x0259E1DC,distribute2);
static s16 consumeAngle(u32 self,u32 angle,s32 amount){
 WWHD_FUNC(0x0259E28C,s16,self,angle,amount);
 s16 value=lh(angle),next=(s16)((u32)(s32)value-(u32)amount);
 if((value>0&&next<0)||(value<0&&next>0)){sh(angle,0);return (s16)-(s32)next;}
 sh(angle,next);return 0;
}
VERIFY(0x0259E28C,consumeAngle);
static BOOL setPath(u32 self,s32 id,s32 room,u32 reverse){
 WWHD_FUNC(0x0259E6D0,BOOL,self,id,room,reverse);
 gabi::store<u8>(self+6,reverse);sw(self,0);if(id==255)return FALSE;
 sw(self,gabi::call<u32>(0x025AAF88,id,room));gabi::store<u8>(self+5,0);return TRUE;
}
VERIFY(0x0259E6D0,setPath);
static BOOL setPathDirect(u32 self,u32 path){
 WWHD_FUNC(0x0259E730,BOOL,self,path);
 sw(self,path);gabi::store<u8>(self+5,0);return TRUE;
}
VERIFY(0x0259E730,setPathDirect);
static u32 nextPath(u32 self){
 WWHD_FUNC(0x0259E744,u32,self);
 u32 path=ld(self);return path?gabi::call<u32>(0x025AB070,path):0;
}
VERIFY(0x0259E744,nextPath);
static void pathPoint(u32 self,u32 output,u32 index){
 WWHD_FUNC(0x0259E778,void,self,output,index);
 f32 x=lf(0x100515BC),y=x,z=x;u32 path=ld(self);
 if(path&&index<gabi::load<u16>(path)){u32 point=ld(path+8)+index*16;x=lf(point+4);y=lf(point+8);z=lf(point+12);}
 if(!output)output=gabi::call<u32>(0x0273AD10,12u);if(output){sf(output+4,y);sf(output,x);sf(output+8,z);}
}
VERIFY(0x0259E778,pathPoint);
static BOOL increment(u32 self){
 WWHD_FUNC(0x0259EB1C,BOOL,self);if(!ld(self))return TRUE;
 u32 index=(gabi::load<u8>(self+5)+1u)&255u;gabi::store<u8>(self+5,index);u32 count=gabi::load<u16>(ld(self));
 if(index<count)return TRUE;gabi::store<u8>(self+5,count-1);return FALSE;
}
VERIFY(0x0259EB1C,increment);
static BOOL incrementLoop(u32 self){
 WWHD_FUNC(0x0259EB60,BOOL,self);if(!ld(self))return TRUE;
 u32 index=(gabi::load<u8>(self+5)+1u)&255u;gabi::store<u8>(self+5,index);u32 count=gabi::load<u16>(ld(self));
 if(index<count)return TRUE;gabi::store<u8>(self+5,0);return FALSE;
}
VERIFY(0x0259EB60,incrementLoop);
static BOOL incrementAuto(u32 self){
 WWHD_FUNC(0x0259EBA4,BOOL,self);u32 path=ld(self);if(!path)return TRUE;
 u32 index=(gabi::load<u8>(self+5)+1u)&255u;bool loop=gabi::load<u8>(path+5)&1;gabi::store<u8>(self+5,index);
 u32 count=gabi::load<u16>(ld(self));if(index<count)return TRUE;gabi::store<u8>(self+5,loop?0:count-1);return loop?TRUE:FALSE;
}
VERIFY(0x0259EBA4,incrementAuto);
static BOOL decrement(u32 self){
 WWHD_FUNC(0x0259EC0C,BOOL,self);if(!ld(self))return TRUE;
 u32 index=(gabi::load<u8>(self+5)-1u)&255u;gabi::store<u8>(self+5,index);u32 count=gabi::load<u16>(ld(self));
 if(index<count)return TRUE;gabi::store<u8>(self+5,0);return FALSE;
}
VERIFY(0x0259EC0C,decrement);
static BOOL decrementLoop(u32 self){
 WWHD_FUNC(0x0259EC50,BOOL,self);if(!ld(self))return TRUE;
 u32 index=(gabi::load<u8>(self+5)-1u)&255u;gabi::store<u8>(self+5,index);u32 count=gabi::load<u16>(ld(self));
 if(index<count)return TRUE;gabi::store<u8>(self+5,count-1);return FALSE;
}
VERIFY(0x0259EC50,decrementLoop);
static BOOL decrementAuto(u32 self){
 WWHD_FUNC(0x0259EC94,BOOL,self);u32 path=ld(self);if(!path)return TRUE;
 u32 index=(gabi::load<u8>(self+5)-1u)&255u;bool loop=gabi::load<u8>(path+5)&1;gabi::store<u8>(self+5,index);
 u32 count=gabi::load<u16>(ld(self));if(index<count)return TRUE;gabi::store<u8>(self+5,loop?count-1:0);return loop?TRUE:FALSE;
}
VERIFY(0x0259EC94,decrementAuto);
static BOOL nextIndex(u32 self){
 WWHD_FUNC(0x0259ECFC,BOOL,self);
 bool forward=gabi::load<u8>(self+6)!=0;u32 result=gabi::call<u32>(forward?0x0259EB1C:0x0259EC0C,self);
 if(!result)gabi::call(forward?0x0259EC0C:0x0259EB1C,self);return result;
}
VERIFY(0x0259ECFC,nextIndex);
static BOOL nextIndexAuto(u32 self){
 WWHD_FUNC(0x0259ED58,BOOL,self);
 bool forward=gabi::load<u8>(self+6)!=0;u32 result=gabi::call<u32>(forward?0x0259EBA4:0x0259EC94,self);
 if(!result)gabi::call(forward?0x0259EC0C:0x0259EB1C,self);return result;
}
VERIFY(0x0259ED58,nextIndexAuto);
static u32 maxPoint(u32 self){
 WWHD_FUNC(0x0259EDB8,u32,self);u32 path=ld(self);return path?gabi::load<u8>(path+1):255;
}
VERIFY(0x0259EDB8,maxPoint);
static s32 indexDistance(u32 self,s32 first,s32 second){
 WWHD_FUNC(0x0259EDD0,s32,self,first,second);if(!ld(self))return 0;
 s32 a=(s32)((u32)first-(u32)second),b=(s32)((u32)second-(u32)first);
 if(a<0)a=(s32)((u32)a+gabi::call<u32>(0x0259EDB8,self));if(b<0)b=(s32)((u32)b+gabi::call<u32>(0x0259EDB8,self));return b<a?b:a;
}
VERIFY(0x0259EDD0,indexDistance);
static u32 pointArgument(u32 self,u32 index){
 WWHD_FUNC(0x0259EE4C,u32,self,index);u32 path=ld(self);if(!path||index>=gabi::load<u8>(path+1))return 0;
 return gabi::load<u8>(ld(path+8)+index*16+3);
}
VERIFY(0x0259EE4C,pointArgument);
static f32 signedFloat(s32 value){
 u64 bits=(u64(0x43300000)<<32)|((u32)value^0x80000000u);f64 converted;__builtin_memcpy(&converted,&bits,8);return (f32)(converted-gabi::load<f64>(0x100515E8));
}
static void swingInitVertical(u32 self,s32 count,s32 step,s32 amplitude,u32 resume){
 WWHD_FUNC(0x0259F36C,void,self,count,step,amplitude,resume);
 if(resume&&lh(self+0xA)==-1&&!lh(self+8)&&ld(self+0xC)==0x0259F448)return;
 sw(self+0xC,0x0259F448);sh(self+0x18,0);sh(self+0x1C,count);sh(self+0x1A,step);sh(self+8,0);sh(self+0xA,-1);sf(self+0x10,signedFloat(amplitude));
}
VERIFY(0x0259F36C,swingInitVertical);
static void swingHorizontal(u32 self){
 WWHD_FUNC(0x0259F448,void,self);
 s16 before=lh(self+0x18);u16 phase=(u16)((s32)before+(s32)lh(self+0x1A));f32 amplitude=lf(self+0x10);sh(self+0x18,phase);
 f32 wave=lf(0x104A44F8+(phase>>3)*8);s16 target=(s16)gabi::ftoi(gabi::fmuls_ppc(amplitude,wave));
 gabi::call(0x0200F378,self,(s32)target,4,0x1000,0x100);gabi::call(0x0200F378,self+2,0,4,0x1000,0x100);
 if(before<0&&lh(self+0x18)>=0){s16 count=(s16)(lh(self+0x1C)-1);sh(self+0x1C,count);if(count<=0){sw(self+0xC,0);sh(self+8,0);sh(self+0xA,0);}}
}
VERIFY(0x0259F448,swingHorizontal);
static void swingInitHorizontal(u32 self,s32 count,s32 step,s32 amplitude,u32 resume){
 WWHD_FUNC(0x0259F514,void,self,count,step,amplitude,resume);
 if(resume&&lh(self+0xA)==-1&&!lh(self+8)&&ld(self+0xC)==0x0259F448)return;
 sh(self+0x18,0);sh(self+0x1C,count);sw(self+0xC,0x0259F5A4);sh(self+0x1A,step);sh(self+8,0);sh(self+0xA,-1);sf(self+0x14,signedFloat(amplitude));
}
VERIFY(0x0259F514,swingInitHorizontal);
static void swingVertical(u32 self){
 WWHD_FUNC(0x0259F5A4,void,self);
 s16 before=lh(self+0x18);u16 phase=(u16)((s32)before+(s32)lh(self+0x1A));f32 amplitude=lf(self+0x14);sh(self+0x18,phase);
 f32 wave=lf(0x104A44F8+(phase>>3)*8);s16 target=(s16)gabi::ftoi(gabi::fmuls_ppc(amplitude,wave));
 gabi::call(0x0200F378,self,0,4,0x1000,0x100);gabi::call(0x0200F378,self+2,(s32)target,4,0x1000,0x100);
 if(before<0&&lh(self+0x18)>=0){s16 count=(s16)(lh(self+0x1C)-1);sh(self+0x1C,count);if(count<=0){sw(self+0xC,0);sh(self+8,0);sh(self+0xA,0);}}
}
VERIFY(0x0259F5A4,swingVertical);
static void headMove(u32 self){
 WWHD_FUNC(0x0259F67C,void,self);
 s16 index=lh(self+0xA);if(index){u32 receiver=self+(s32)lh(self+8);u32 target= index<0?ld(self+0xC):ld(ld(receiver+(s32)lh(self+0xE))+(u32)(s32)index*8+4);gabi::call_ptr(target,receiver);}
 else {gabi::call(0x0200F378,self,0,4,0x1000,0x100);gabi::call(0x0200F378,self+2,0,4,0x1000,0x100);}
}
VERIFY(0x0259F67C,headMove);
static u32 cutCtor(u32 self){
 WWHD_FUNC(0x0259F740,u32,self);
 if(!self)self=gabi::call<u32>(0x0273AD10,0x6Cu);if(!self)return 0;f32 zero=lf(0x100515BC);
 sw(self,0);sf(self+0x4C,zero);sw(self+0x18,0);sf(self+0x48,zero);
 for(u32 off:{0x1Cu,0x68u,0xCu,0x14u,0x40u,0x20u,0x4u,0x8u,0x64u,0x10u,0x24u})sw(self+off,0);
 for(u32 off:{0x60u,0x52u,0x61u})gabi::store<u8>(self+off,0);
 for(u32 off:{0x62u,0x44u,0x50u})sh(self+off,0);return self;
}
VERIFY(0x0259F740,cutCtor);
static void cutInfo(u32 self,u32 name,u32 actor){
 WWHD_FUNC(0x0259F7D4,void,self,name,actor);
 sw(self+8,actor);f32 zero=lf(0x100515BC);sw(self,name);
 for(u32 off:{0x34u,0x2Cu,0x5Cu,0x58u,0x38u,0x30u,0x28u})sf(self+off,zero);sw(self+0x10,-1);sf(self+0x54,zero);sf(self+0x3C,zero);
}
VERIFY(0x0259F7D4,cutInfo);
static void cutInfo2(u32 self,u32 name,u32 actor){
 WWHD_FUNC(0x0259F814,void,self,name,actor);
 sw(self,name);f32 zero=lf(0x100515BC);sw(self+0x10,-1);
 for(u32 off:{0x28u,0x38u,0x3Cu,0x5Cu,0x34u,0x54u,0x58u,0x2Cu})sf(self+off,zero);
 sw(self+0xC,actor);sw(self+8,actor);sf(self+0x30,zero);
}
VERIFY(0x0259F814,cutInfo2);
static void invokeCut(u32 self,u32 descriptor){
 s16 index=lh(descriptor+2);u32 receiver=self+(s32)lh(descriptor);u32 target=index<0?ld(descriptor+4):ld(ld(receiver+(s32)lh(descriptor+6))+(u32)(s32)index*8+4);gabi::call_ptr(target,receiver);
}
static BOOL cutProcess(u32 self){
 WWHD_FUNC(0x0259F858,BOOL,self);
 u32 name=ld(self);if(!name||!ld(self+8))return FALSE;
 u32 play=gabi::call<u32>(0x025200D4);s32 staff=gabi::call<s32>(0x02542D88,play+0x52C4,name,0,0);sw(self+4,staff);if(staff==-1)return FALSE;
 play=gabi::call<u32>(0x025200D4);s32 index=gabi::call<s32>(0x02542EDC,play+0x52C4,staff,0x101EA150,7,1,0);sw(self+0x10,index);if(index==-1)return FALSE;
 staff=ld(self+4);play=gabi::call<u32>(0x025200D4);bool advancing=gabi::call<u32>(0x025447C8,play+0x52C4,staff)!=0;
 if(advancing){gabi::store<u8>(self+0x60,0);invokeCut(self,0x100515F0+ld(self+0x10)*16);}
 invokeCut(self,0x100515F8+ld(self+0x10)*16);return TRUE;
}
VERIFY(0x0259F858,cutProcess);
static void cutWaitStart(u32 self){
 WWHD_FUNC(0x0259FA00,void,self);
 s32 staff=ld(self+4);u32 play=gabi::call<u32>(0x025200D4);u32 value=gabi::call<u32>(0x0254487C,play+0x52C4,staff,0x100516BC,3);sw(self+0x18,value?(s32)lh(value+2):0);
}
VERIFY(0x0259FA00,cutWaitStart);
static void cutWait(u32 self){
 WWHD_FUNC(0x0259FA64,void,self);
 if(!gabi::call<u32>(0x0211D2F8,self+0x18)){s32 staff=ld(self+4);u32 play=gabi::call<u32>(0x025200D4);gabi::call(0x02543280,play+0x52C4,staff);}
}
VERIFY(0x0259FA64,cutWait);
static u32 cutValue(u32 self,u32 key,u32 type){
 s32 staff=ld(self+4);u32 play=gabi::call<u32>(0x025200D4);return gabi::call<u32>(0x0254487C,play+0x52C4,staff,key,type);
}
static void talkStart(u32 self){
 WWHD_FUNC(0x025A1090,void,self);if(!ld(self+0xC))return;
 gabi::store<u8>(self+0x60,cutValue(self,0x10051830,3)!=0);u32 begin=cutValue(self,0x10051820,3);u32 end=cutValue(self,0x10051828,3);
 u32 actor=ld(self+0xC);u32 first=begin?ld(begin):0;sw(actor+0x7C0,first);actor=ld(self+0xC);sw(actor+0x7C4,end?ld(end):0xFFFFFFFF);gabi::store<u8>(self+0x52,1);
}
VERIFY(0x025A1090,talkStart);
static void talkContinue(u32 self){
 WWHD_FUNC(0x025A1178,void,self);if(!ld(self+0xC))return;
 u32 end=cutValue(self,0x1005183C,3);sw(ld(self+0xC)+0x7C4,end?ld(end):0xFFFFFFFF);
}
VERIFY(0x025A1178,talkContinue);
static void talkProcess(u32 self){
 WWHD_FUNC(0x025A1388,void,self);u32 actor=ld(self+0xC);bool end=false;
 if(!actor||!gabi::load<u8>(self+0x52))end=true;
 else {
  u32 state=gabi::call<u32>(0x025A11EC,actor,0);
  if(state==18){gabi::store<u8>(self+0x52,0);end=true;}
  else if(state==2||state==6){actor=ld(self+0xC);if(ld(actor+0x7C4)==ld(actor+0x7C0)){sw(actor+0x7C4,-1);end=true;}}
 }
 if(end){s32 staff=ld(self+4);u32 play=gabi::call<u32>(0x025200D4);gabi::call(0x02543280,play+0x52C4,staff);}
}
VERIFY(0x025A1388,talkProcess);
static u32 npcCtor(u32 self){
 WWHD_FUNC(0x025A1458,u32,self);if(!self)self=gabi::call<u32>(0x0273AD10,0x7DCu);if(!self)return 0;
 gabi::call(0x025D4ED0,self);sw(self+0xB4,0x10051858);gabi::call(0x0259DAA0,self+0x3AC);gabi::call(0x0259F740,self+0x3E0);
 sw(self+0x44C,0);gabi::call(0x024F0474,self+0x450);sw(self+0x460,0x1005157C);sw(self+0x464,0x1005159C);gabi::store<u8>(self+0x468,1);sw(self+0x470,0x1005158C);
 gabi::call(0x024EFE94,self+0x614);gabi::call(0x0200BD2C,self+0x654);gabi::call(0x02515DA0,self+0x670);sw(self+0x66C,0x1004AE88);sw(self+0x670,0x1004AEC0);
 gabi::call(0x02515FB8,self+0x690);sw(self+0x7A4,0x100015A8);sw(self+0x7A0,0x1005156C);gabi::call(0x02018590,self+0x7A8);
 sh(self+0x7D4,0);sw(self+0x7C0,0);sw(self+0x7D0,0);sw(self+0x7BC,0x1004B150);sw(self+0x7C4,0);gabi::store<u8>(self+0x7D8,0);sw(self+0x6CC,0x1004B108);sw(self+0x7A4,0x1004B160);sh(self+0x7D6,0);gabi::store<u8>(self+0x7CC,0);sw(self+0x7C8,-1);return self;
}
VERIFY(0x025A1458,npcCtor);
static u32 talkDefault(){
 WWHD_FUNC(0x025A15A0,u32);return 16;
}
VERIFY(0x025A15A0,talkDefault);
static void npcStub(){
 WWHD_FUNC(0x025A15A8,void);
}
VERIFY(0x025A15A8,npcStub);
static void npcCollision(u32 self,f32 radius,f32 height){
 WWHD_FUNC(0x025A15AC,void,self,radius,height);
 gabi::call(0x020182E0,self+0x7A8,self+0x314);gabi::call(0x020184DC,self+0x7A8,radius);gabi::call(0x02018428,self+0x7A8,height);
 u32 play=gabi::call<u32>(0x025200D4);gabi::call(0x0200E240,play+0x26A4,self+0x690);
}
VERIFY(0x025A15AC,npcCollision);
static void npcInitializer(){
 WWHD_FUNC(0x025A1648,void);
 sw(0x1047B0DC,0);sw(0x1047B0D4,0);sw(0x1047B0E0,0);sw(0x1047B0D8,0);gabi::call(0x028F026C,0x101EA16C);
 f32 first=lf(0x1005184C),second=lf(0x10051850);sf(0x1047B0C8,first);sf(0x1047B0CC,second);
 gabi::call(0x028ED6F8,0x1047B0D0);gabi::call(0x028F026C,0x101EA178);gabi::call(0x028EAB2C,0x1047B0D1);gabi::call(0x028F026C,0x101EA184);
}
VERIFY(0x025A1648,npcInitializer);
static void generatedDestructor(u32 self,u32 flags){
 WWHD_FUNC(0x025A16DC,void,self,flags);if(self&&(flags&1))gabi::call(0x0273AF40,self);
}
VERIFY(0x025A16DC,generatedDestructor);
static u32 defaultFalse(){
 WWHD_FUNC(0x025A16F0,u32);return 0;
}
VERIFY(0x025A16F0,defaultFalse);
static void npcDestructor(u32 self,u32 flags){
 WWHD_FUNC(0x025A16F8,void,self,flags);if(!self)return;
 gabi::call(0x02515A70,self+0x690,2);gabi::call(0x02515860,self+0x654,2);gabi::call(0x02018034,self+0x628,2);
 sw(self+0x470,0x1005158C);sw(self+0x464,0x1005159C);gabi::call(0x024EFD9C,self+0x450,0);gabi::call(0x025D50BC,self,0);if(flags&1)gabi::call(0x0273AF40,self);
}
VERIFY(0x025A16F8,npcDestructor);
static void generatedStub(){
 WWHD_FUNC(0x025A1794,void);
}
VERIFY(0x025A1794,generatedStub);
static u16 npcTalk(u32 self,s32 mode){
 WWHD_FUNC(0x025A11EC,u16,self,mode);
 u32 message=ld(0x101F4B5C);u16 state=255;
 if(ld(self+0x7C8)==0xFFFFFFFF){
  u32 number;
  if(mode==1){number=gabi::call_ptr<u32>(ld(ld(self+0xB4)+0x1C),self);sw(self+0x7C0,number);}else number=ld(self+0x7C0);
  u32 id=gabi::call<u32>(0x025F7DB0,message,number,self+0x37C);sw(self+0x7C8,id);if(id!=0xFFFFFFFF)gabi::store<u8>(self+0x7CC,0);return state;
 }
 if(!gabi::load<u8>(self+0x7CC)){gabi::store<u8>(self+0x7CC,1);return state;}
 state=(u16)gabi::call<u32>(0x025F795C,message);
 if(state==14){
  u32 next=gabi::call_ptr<u32>(ld(ld(self+0xB4)+0x14),self,self+0x7C0);gabi::call(0x025F74D0,message,next);
  if(gabi::call<u32>(0x025F795C,message)==15)gabi::call(0x025F7DB0,message,ld(self+0x7C0),0);
 }else if(state==18){gabi::call(0x025F74D0,message,19);sw(self+0x7C8,-1);}
 gabi::call_ptr(ld(ld(self+0xB4)+0x24),self,(u32)state);return state;
}
VERIFY(0x025A11EC,npcTalk);
static BOOL nearestWeighted(u32 self,u32 position,f32 weight){
 WWHD_FUNC(0x0259EE7C,BOOL,self,position,weight);if(!ld(self))return FALSE;
 f32 best=lf(0x100515E0),zero=lf(0x100515BC);u32 selected=0;
 for(s32 index=0;index<gabi::call<s32>(0x0259EDB8,self);index++){
  gabi::Local<Vec> point,copy,flat;gabi::call(0x0259E778,self,point.get(),(u32)index&255);copy->x=point->x;copy->y=point->y;copy->z=point->z;
  gabi::call(0x0201ADE0,position,point.get(),copy.get());f32 dy=point->y;flat->x=point->x;flat->y=zero;flat->z=point->z;
  f32 horizontal=gabi::call<f32>(0x028E8DD0,flat.get());f32 squared=gabi::fmadds(gabi::fmuls_ppc(dy,dy),weight,horizontal);f32 distance=gabi::call<f32>(0x028F4384,squared);
  if(best>distance){best=distance;selected=(u32)index&255;}
 }
 gabi::store<u8>(self+5,selected);return TRUE;
}
VERIFY(0x0259EE7C,nearestWeighted);
static f32 nearestDistance(u32 self,u32 position){
 WWHD_FUNC(0x0259EFE0,f32,self,position);f32 best=lf(0x100515E0);if(!ld(self))return best;
 u32 selected=0;for(s32 index=0;index<gabi::call<s32>(0x0259EDB8,self);index++){
  gabi::Local<Vec> point,copy,difference;gabi::call(0x0259E778,self,point.get(),(u32)index&255);copy->x=point->x;copy->y=point->y;copy->z=point->z;
  gabi::call(0x0201ADE0,position,point.get(),copy.get());difference->x=point->x;difference->y=point->y;difference->z=point->z;
  f32 squared=gabi::call<f32>(0x028E8DD0,difference.get());f32 distance=gabi::call<f32>(0x028F4384,squared);if(best>distance){best=distance;selected=(u32)index&255;}
 }gabi::store<u8>(self+5,selected);return best;
}
VERIFY(0x0259EFE0,nearestDistance);
static BOOL nearestLimited(u32 self,u32 position,u32 current,s32 maxDifference){
 WWHD_FUNC(0x0259F0F8,BOOL,self,position,current,maxDifference);if(!ld(self))return FALSE;
 f32 best=lf(0x100515E0);u32 selected=current;BOOL found=FALSE;
 for(s32 index=0;index<gabi::call<s32>(0x0259EDB8,self);index++){
  gabi::Local<Vec> point,copy,difference;gabi::call(0x0259E778,self,point.get(),(u32)index&255);copy->x=point->x;copy->y=point->y;copy->z=point->z;
  gabi::call(0x0201ADE0,position,point.get(),copy.get());difference->x=point->x;difference->y=point->y;difference->z=point->z;
  f32 squared=gabi::call<f32>(0x028E8DD0,difference.get());f32 distance=gabi::call<f32>(0x028F4384,squared);
  s32 delta=gabi::call<s32>(0x0259EDD0,self,current,(u32)index&255);if(delta<=maxDifference&&current!=(u32)index&&best>distance){best=distance;found=TRUE;selected=(u32)index&255;}
 }gabi::store<u8>(self+5,selected);return found;
}
VERIFY(0x0259F0F8,nearestLimited);
static BOOL pathInside(u32 self,u32 position){
 WWHD_FUNC(0x0259F220,BOOL,self,position);gabi::call(0x0259EE7C,self,position,lf(0x100515BC));
 gabi::Local<Vec> temporary,current,previous,next;
 gabi::call(0x0259E778,self,temporary.get(),(u32)gabi::load<u8>(self+5));current->x=temporary->x;current->y=temporary->y;current->z=temporary->z;
 gabi::call(0x0259EC50,self);gabi::call(0x0259E778,self,temporary.get(),(u32)gabi::load<u8>(self+5));previous->x=temporary->x;previous->y=temporary->y;previous->z=temporary->z;
 gabi::call(0x0259EB60,self);gabi::call(0x0259EB60,self);gabi::call(0x0259E778,self,temporary.get(),(u32)gabi::load<u8>(self+5));next->x=temporary->x;next->y=temporary->y;next->z=temporary->z;
 s32 first=gabi::call<s32>(0x0200F93C,current.get(),previous.get());s32 direction=gabi::call<s32>(0x0200F93C,current.get(),position);s32 last=gabi::call<s32>(0x0200F93C,current.get(),next.get());
 s16 a=(s16)((u32)direction-(u32)last),b=(s16)((u32)first-(u32)last);
 if(a>0){if(b<0)return TRUE;return a<b;}else {if(b>=0)return FALSE;return a<b;}
}
VERIFY(0x0259F220,pathInside);
static BOOL pathPassed(u32 self,u32 position,u32 forward){
 WWHD_FUNC(0x0259E838,BOOL,self,position,forward);u32 path=ld(self);if(!path)return FALSE;
 u32 index=gabi::load<u8>(self+5),points=ld(path+8),point=points+index*16;f32 x=lf(point+4),z=lf(point+12);u32 from,to;
 if(!index){from=points;to=points+16;}else if(index==(u32)gabi::load<u16>(path)-1){to=points+(u32)gabi::load<u16>(path)*16-16;from=to-16;}else {from=point-16;to=point+16;}
 f32 dx=gabi::fsubs_ppc(lf(to+4),lf(from+4)),dz=gabi::fsubs_ppc(lf(to+12),lf(from+12));
 u16 first=(u16)gabi::call<u32>(0x020195B0,dx,dz);f32 scale=lf(0x100515C4);f32 sin=gabi::fmuls_ppc(scale,lf(0x104A44F8+(first>>3)*8));
 u16 second=(u16)gabi::call<u32>(0x020195B0,dx,dz);f32 cos=gabi::fmuls_ppc(scale,lf(0x104A44FC+(second>>3)*8));
 f32 plane=gabi::fmadds(sin,x,gabi::fmuls_ppc(cos,z));f32 px=lf(position),pz=lf(position+8);f32 side=gabi::fadds_ppc(gabi::fmadds(sin,px,gabi::fmuls_ppc(cos,pz)),-plane);f32 zero=lf(0x100515BC);
 return forward?(side>zero):!(side>zero);
}
VERIFY(0x0259E838,pathPassed);
static void copyPosition(u32 dest,u32 source){sw(dest,ld(source));sw(dest+4,ld(source+4));sw(dest+8,ld(source+8));}
static void turnActorStart(u32 self){
 WWHD_FUNC(0x0259FAB0,void,self);
 sw(self+0x1C,cutValue(self,0x100516E8,4));u32 value=cutValue(self,0x100516C4,3);sw(self+0x24,value?ld(value):0);
 value=cutValue(self,0x100516F4,1);f32 zero=lf(0x100515BC);
 if(value){sw(self+0x28,ld(value));sw(self+0x2C,ld(value+4));sw(self+0x30,ld(value+8));}else {sf(self+0x2C,zero);sf(self+0x30,zero);sf(self+0x28,zero);}
 value=cutValue(self,0x1005170C,3);sw(self+0x64,value?ld(value):1);
 value=cutValue(self,0x100516CC,3);sw(self+0x18,value?(s32)lh(value+2):1);
 value=cutValue(self,0x10051700,3);sh(self+0x62,value?lh(value+2):0);
 value=cutValue(self,0x10051718,3);sh(self+0x50,value?lh(value+2):0);
 value=cutValue(self,0x100516DC,0);sf(self+0x4C,value?lf(value):zero);
 value=cutValue(self,0x100516D4,3);sh(self+0x44,0);gabi::store<u8>(self+0x61,value!=0);
}
VERIFY(0x0259FAB0,turnActorStart);
static u32 findCutActor(u32 actor,u32 cut){
 WWHD_FUNC(0x0259FD08,u32,actor,cut);if(!cut)return 0;
 u32 id=ld(cut+0x24);if(id&&gabi::load<u16>(actor+0x2D8)==id){copyPosition(cut+0x34,actor+0x314);sw(cut+0x40,actor);return actor;}
 u32 name=ld(cut+0x1C);if(!name)return 0;u32 record=gabi::call<u32>(0x025C109C,name);if(!record)return 0;
 s32 profile=actor?lh(actor+0xE):0x7FFF;if((u32)(s32)lh(record+8)!=(u32)profile)return 0;
 if(gabi::load<s8>(actor+0x2DD)!=gabi::load<s8>(record+0xA))return 0;
 f32 squared=gabi::call<f32>(0x028E8DD0,cut+0x34);f32 distance=gabi::call<f32>(0x028F4384,squared);
 if(distance!=lf(0x100515BC)){
  gabi::Local<Vec> delta;gabi::call(0x0201ADE0,cut+0x34,delta.get(),ld(cut+8)+0x314);squared=gabi::call<f32>(0x028E8DD0,delta.get());f32 old=gabi::call<f32>(0x028F4384,squared);
  gabi::call(0x0201ADE0,actor+0x314,delta.get(),ld(cut+8)+0x314);squared=gabi::call<f32>(0x028E8DD0,delta.get());distance=gabi::call<f32>(0x028F4384,squared);if(!(distance<old))return 0;
 }
 copyPosition(cut+0x34,actor+0x314);sw(cut+0x40,actor);return 0;
}
VERIFY(0x0259FD08,findCutActor);

static void turnPositionStart(u32 self){
 WWHD_FUNC(0x025A0984,void,self);u32 value=cutValue(self,0x100517B0,1);f32 zero=lf(0x100515BC);
 if(value){sw(self+0x54,ld(value));sw(self+0x58,ld(value+4));sw(self+0x5C,ld(value+8));}else {sf(self+0x58,zero);sf(self+0x54,zero);sf(self+0x5C,zero);}
 value=cutValue(self,0x100517D0,3);sw(self+0x64,value?ld(value):1);
 value=cutValue(self,0x100517B4,3);sw(self+0x18,value?(s32)lh(value+2):1);
 value=cutValue(self,0x100517C4,3);sh(self+0x62,value?lh(value+2):0);
 value=cutValue(self,0x100517BC,3);gabi::store<u8>(self+0x61,value!=0);
 value=cutValue(self,0x100517DC,3);sh(self+0x50,value?lh(value+2):0);sh(self+0x44,0);
}
VERIFY(0x025A0984,turnPositionStart);
static void endCut(u32 self){u32 staff=ld(self+4);u32 play=gabi::call<u32>(0x025200D4);gabi::call(0x02543280,play+0x52C4,staff);}
static void turnPositionProcess(u32 self){
 WWHD_FUNC(0x025A0B3C,void,self);u32 yaw=gabi::call<u32>(0x0200F93C,ld(self+8)+0x314,self+0x54);u32 joint=ld(self+0x68);gabi::store<u8>(self+0x60,1);
 if(joint){u32 mode=ld(self+0x64);if(mode==2){gabi::store<u8>(joint+0xA,1);joint=ld(self+0x68);}gabi::store<u8>(joint+0xC,mode==0);}
 f32 absolute=std::fabs(signedFloat((s32)(yaw-(u32)(s32)lh(ld(self+8)+0x322))));
 if(signedFloat(lh(self+0x44))==absolute&&absolute<lf(0x10051724))gabi::call(0x0211D2F8,self+0x18);
 if(!ld(self+0x18)){sh(self+0x62,0);endCut(self);}
 absolute=std::fabs(signedFloat((s32)(yaw-(u32)(s32)lh(ld(self+8)+0x322))));sh(self+0x44,gabi::ftoi(absolute));
}
VERIFY(0x025A0B3C,turnPositionProcess);
static void movePositionStart(u32 self){
 WWHD_FUNC(0x025A0D5C,void,self);u32 position=cutValue(self,0x100517E8,1),speed=cutValue(self,0x100517EC,0),distance=cutValue(self,0x100517FC,0),attention=cutValue(self,0x10051808,3),noTurn=cutValue(self,0x100517F4,3),angle=cutValue(self,0x10051814,3);f32 zero=lf(0x100515BC);
 gabi::store<u8>(self+0x61,noTurn!=0);if(position){sw(self+0x54,ld(position));sw(self+0x58,ld(position+4));sw(self+0x5C,ld(position+8));}else {sf(self+0x5C,zero);sf(self+0x54,zero);sf(self+0x58,zero);}
 gabi::store<u8>(self+0x60,attention!=0);sw(self+0x48,speed?ld(speed):ld(0x10051730));sw(self+0x4C,distance?ld(distance):ld(0x100515BC));sh(self+0x50,angle?lh(angle+2):0);
}
VERIFY(0x025A0D5C,movePositionStart);

static void movePositionProcess(u32 self){
 WWHD_FUNC(0x025A0F00,void,self);u16 yaw=(u16)gabi::call<u32>(0x0200F93C,ld(self+8)+0x314,self+0x54);
 f32 speed=lf(self+0x48),distance=lf(self+0x4C),goalX=lf(self+0x54),sin=lf(0x104A44F8+(yaw>>3)*8),zero=lf(0x100515BC);
 f32 x=gabi::fnmsubs(distance,sin,goalX),cos=lf(0x104A44FC+(yaw>>3)*8),goalZ=lf(self+0x5C),y=lf(self+0x58),z=gabi::fnmsubs(distance,cos,goalZ);
 gabi::Local<Vec> target,delta,flat;target->x=x;target->y=y;target->z=z;
 if(speed==zero){u32 actor=ld(self+8);sf(actor+0x314,x);sf(actor+0x318,y);sf(actor+0x31C,z);endCut(self);}
 gabi::call(0x0201ADE0,target.get(),delta.get(),ld(self+8)+0x314);flat->x=delta->x;flat->z=delta->z;flat->y=zero;
 f32 squared=gabi::call<f32>(0x028E8DD0,flat.get()),remaining=gabi::call<f32>(0x028F4384,squared),factor=lf(0x100517A0);
 gabi::call(0x0200ED84,ld(self+8)+0x314,(f32)target->x,factor,lf(self+0x48));gabi::call(0x0200ED84,ld(self+8)+0x31C,(f32)target->z,factor,lf(self+0x48));
 if(remaining<lf(0x100517A4)){sf(self+0x48,zero);endCut(self);}
}
VERIFY(0x025A0F00,movePositionProcess);

static void jointLook2(u32 self,u32 angle,u32 target,u32 origin,s32 yaw,u32 velocity,u32 locked){
 WWHD_FUNC(0x0259E2D4,void,self,angle,target,origin,yaw,velocity,locked);s32 pitch=0;
 if(target){yaw=gabi::call<s32>(0x0200F93C,origin,target);pitch=gabi::call<s32>(0x0200F974,origin,target);}
 s16 relative=(s16)((u32)yaw-(u32)(s32)lh(angle));s32 previous=lh(self+0x32);bool split=previous<0?relative>previous:(relative<previous&&previous!=0);
 gabi::Local<be<s16>> head,back;
 if(split)gabi::call(0x0259E104,self,(s32)relative,head.get(),back.get(),(u32)(previous>=0));else gabi::call(0x0259E1DC,self,(s32)relative,head.get(),back.get());
 s32 backTarget=(s16)*back;sh(self+0x32,backTarget);gabi::Local<be<s32>> headValue,backValue;*headValue=lh(self+2);*backValue=lh(self+6);s32 headTarget=(s16)*head;
 sh(self+0x2E,headTarget);gabi::call(0x0200F474,headValue.get(),headTarget,4, (s32)lh(self+0x1E),4);gabi::call(0x0200F474,backValue.get(),backTarget,4,(s32)lh(self+0x1E),4);
 sh(self+2,(s32)*headValue);bool following=gabi::load<u8>(self+0xA)!=0;sh(self+6,(s32)*backValue);
 if(following&&!locked){s16 old=lh(angle);gabi::call(0x0200F378,angle,yaw,4,velocity,0x100);s16 change=(s16)((u32)(s32)lh(angle)-(u32)(s32)old);bool blocked=gabi::load<u8>(self+0xC)!=0;gabi::store<u8>(self+0xA,change!=0);
  s32 amount=change;if(!blocked)amount=gabi::call<s32>(0x0259E28C,self,self+6,amount);
  if(!gabi::load<u8>(self+0xB))gabi::call<s32>(0x0259E28C,self,self+2,amount);
 }else {s32 total=(s32)lh(self+0x2E)+(s32)lh(self+0x32);gabi::store<u8>(self+0xA,relative<0?relative<total:relative>total);}
 s32 top=gabi::call<s32>(0x0259E0C8,self,pitch,0,0);s32 bottom=gabi::call<s32>(0x0259E0C8,self,(s32)(s16)((u32)pitch-(u32)top),1,0);
 *headValue=lh(self);s32 rate=lh(self+0x1E);sh(self+0x30,bottom);*backValue=lh(self+4);sh(self+0x2C,top);
 gabi::call(0x0200F474,headValue.get(),(s32)(s16)top,4,rate,4);gabi::call(0x0200F474,backValue.get(),bottom,4,(s32)lh(self+0x1E),4);
 sh(self,(s32)*headValue);sh(self+4,(s32)*backValue);
}
VERIFY(0x0259E2D4,jointLook2);

static bool sameText(u32 a,u32 b){for(;;){u8 x=gabi::load<u8>(a++),y=gabi::load<u8>(b++);if(x!=y)return false;if(!x)return true;}}
static bool safeTextPair(gabi::Local<NpcString>& a,gabi::Local<NpcString>& b){
 gabi::call_ptr(ld((u32)a->vt+0x14),a.get());u32 saved=a->data;gabi::call_ptr(ld((u32)b->vt+0x14),b.get());if(saved==(u32)b->data)return true;
 u32 first=a->data,second=b->data;for(u32 n=0;n<0x40001;n++){u8 x=gabi::load<u8>(first++),y=gabi::load<u8>(second++);if(x!=y)return false;if(!x)return true;}return false;
}
static void moveActorStart(u32 self){
 WWHD_FUNC(0x025A02DC,void,self);sw(self+0x1C,cutValue(self,0x10051770,4));u32 id=cutValue(self,0x10051740,3),speed=cutValue(self,0x10051748,0),distance=cutValue(self,0x10051764,0),position=cutValue(self,0x1005177C,1),attention=cutValue(self,0x10051788,3),noTurn=cutValue(self,0x10051750,3),angle=cutValue(self,0x10051794,3);
 gabi::store<u8>(self+0x61,noTurn!=0);f32 zero=lf(0x100515BC);gabi::store<u8>(self+0x60,attention!=0);
 if(position){sw(self+0x28,ld(position));sw(self+0x2C,ld(position+4));sw(self+0x30,ld(position+8));}else {sf(self+0x28,zero);sf(self+0x2C,zero);sf(self+0x30,zero);}
 sw(self+0x24,id?ld(id):0);sw(self+0x48,speed?ld(speed):ld(0x10051730));sw(self+0x4C,distance?ld(distance):ld(0x100515BC));
 if(distance){
  gabi::Local<NpcString> key,actual;key->data=0x10051738;key->vt=0x10051554;u32 play=gabi::call<u32>(0x025200D4);actual->vt=0x10051554;actual->data=play+0x5134;
  gabi::call_ptr(ld((u32)key->vt+0x14),key.get());bool matched=safeTextPair(key,actual);
  if(matched){gabi::Local<NpcString> objectKey,object;objectKey->data=0x10051758;object->data=ld(self);object->vt=0x10051554;objectKey->vt=0x10051554;gabi::call(0x025A1794,objectKey.get());matched=safeTextPair(objectKey,object);
   if(matched){gabi::Local<NpcString> actorKey,actor;actorKey->data=0x1005175C;actor->data=ld(self+0x1C);actor->vt=0x10051554;actorKey->vt=0x10051554;gabi::call(0x025A1794,actorKey.get());matched=safeTextPair(actorKey,actor);}
  }
  if(matched&&gabi::ftoi(lf(self+0x4C))==100)sf(self+0x4C,gabi::fadds_ppc(lf(self+0x4C),lf(0x10051734)));
 }
 sh(self+0x50,angle?lh(angle+2):0);
}
VERIFY(0x025A02DC,moveActorStart);

static void turnActorProcess(u32 self){
 WWHD_FUNC(0x0259FE88,void,self);f32 zero=lf(0x100515BC);
 if(!ld(self+0x24)&&(!ld(self+0x1C)||sameText(ld(self+0x1C),0x10051728))){u32 play=gabi::call<u32>(0x025200D4);sw(self+0x40,ld(play+0x5B2C));}
 else {sf(self+0x34,zero);sf(self+0x38,zero);sf(self+0x3C,zero);gabi::call(0x025D5218,0x0259FD08,self);}
 u32 joint=ld(self+0x68);if(joint){u32 mode=ld(self+0x64);if(mode==2){gabi::store<u8>(joint+0xA,1);joint=ld(self+0x68);}gabi::store<u8>(joint+0xC,mode==0);}
 if(!ld(self+0x40))endCut(self);
 u32 name=ld(self+0x1C);
 if(!name||sameText(name,0x10051728)){gabi::Local<Vec> eye,goal;gabi::call(0x0259D54C,eye.get(),zero);gabi::call(0x0201AD78,eye.get(),goal.get(),self+0x28);copyPosition(self+0x54,gabi::ea(goal.get()));}
 else if(lh(self+0x50)&&lf(self+0x4C)!=zero){u32 actor=ld(self+0x40);if(actor){sw(self+0x54,ld(actor+0x314));u16 yaw=(u16)((u32)(s32)lh(actor+0x32A)+(u32)(s32)lh(self+0x50));sw(self+0x58,ld(actor+0x318));f32 x=lf(self+0x54);sw(self+0x5C,ld(actor+0x31C));f32 distance=lf(self+0x4C),sin=lf(0x104A44F8+(yaw>>3)*8);sf(self+0x54,gabi::fnmsubs(distance,sin,x));sf(self+0x5C,gabi::fnmsubs(distance,lf(0x104A44FC+(yaw>>3)*8),lf(self+0x5C)));}}
 else {gabi::Local<Vec> goal;gabi::call(0x0201AD78,ld(self+0x40)+0x314,goal.get(),self+0x28);copyPosition(self+0x54,gabi::ea(goal.get()));}
 gabi::store<u8>(self+0x60,1);u32 yaw=gabi::call<u32>(0x0200F93C,ld(self+8)+0x314,self+0x54);
 f32 absolute=std::fabs(signedFloat((s32)(yaw-(u32)(s32)lh(ld(self+8)+0x322))));if(signedFloat(lh(self+0x44))==absolute&&absolute<lf(0x10051724))gabi::call(0x0211D2F8,self+0x18);
 if(!ld(self+0x18)){sh(self+0x62,0);endCut(self);}absolute=std::fabs(signedFloat((s32)(yaw-(u32)(s32)lh(ld(self+8)+0x322))));sh(self+0x44,gabi::ftoi(absolute));
}
VERIFY(0x0259FE88,turnActorProcess);
static void moveActorProcess(u32 self){
 WWHD_FUNC(0x025A0730,void,self);f32 zero=lf(0x100515BC);
 if(!ld(self+0x24)&&(!ld(self+0x1C)||sameText(ld(self+0x1C),0x100517A8))){u32 play=gabi::call<u32>(0x025200D4);sw(self+0x40,ld(play+0x5B2C));}
 else {sf(self+0x38,zero);sf(self+0x3C,zero);sf(self+0x34,zero);gabi::call(0x025D5218,0x0259FD08,self);}
 u32 actor=ld(self+0x40);if(!actor){endCut(self);return;}
 gabi::Local<Vec> target,delta,flat;gabi::call(0x0201AD78,actor+0x314,target.get(),self+0x28);u32 yaw=gabi::call<u32>(0x025D6894,ld(self+8),ld(self+0x40));s32 angle=lh(self+0x50);if(angle)yaw=(u32)(s32)(s16)((u32)(s32)lh(ld(self+0x40)+0x32A)+(u32)angle);yaw&=65535;
 f32 z=target->z,distance=lf(self+0x4C),x=target->x,sin=lf(0x104A44F8+(yaw>>3)*8),cos=lf(0x104A44FC+(yaw>>3)*8);x=gabi::fnmsubs(distance,sin,x);f32 speed=lf(self+0x48);z=gabi::fnmsubs(distance,cos,z);target->x=x;target->z=z;
 if(speed==zero){actor=ld(self+8);sf(actor+0x314,x);sf(actor+0x31C,z);sf(actor+0x318,(f32)target->y);endCut(self);}
 gabi::call(0x0201ADE0,target.get(),delta.get(),ld(self+8)+0x314);flat->x=delta->x;flat->z=delta->z;flat->y=zero;
 f32 squared=gabi::call<f32>(0x028E8DD0,flat.get()),remaining=gabi::call<f32>(0x028F4384,squared),factor=lf(0x100517A0);
 gabi::call(0x0200ED84,ld(self+8)+0x314,(f32)target->x,factor,lf(self+0x48));gabi::call(0x0200ED84,ld(self+8)+0x31C,(f32)target->z,factor,lf(self+0x48));if(remaining<lf(0x100517A4)){sf(self+0x48,zero);endCut(self);}
}
VERIFY(0x025A0730,moveActorProcess);
