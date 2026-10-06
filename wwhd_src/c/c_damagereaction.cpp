/* WWHD damage reactions. 
 * Complete fourteen-entry HD inventory020402C8..02046A68. */
#include "bindings.h"
namespace damage {
using gabi::load;using gabi::store;
static void fire_remove(u32 fire){
 WWHD_FUNC(0x02041C30,void,fire);
 store<u8>(fire+6,0);gabi::call(0x0255A374,fire+0x208);
 for(u32 i=0;i<10;i++){
  u32 emitter=load<u32>(fire+0x58+4*i);
  if(emitter){u32 flags=load<u32>(emitter+0x254);store<u32>(emitter+0x5C,0xFFFFFFFF);store<u32>(emitter+0x254,flags|1);store<u32>(fire+0x58+4*i,0);}
 }
}
VERIFY(0x02041C30,fire_remove);
static void piyo(u32 actor){
 WWHD_FUNC(0x02041CAC,void,actor);
 u32 play=gabi::call<u32>(0x025200D4);
 u32 manager=load<u32>(play+0x5AB0);
 gabi::call<u32>(0x025A847C,manager,0,0x27A,actor+0x390,0,0,255,0,0xFFFFFFFF,0,0,0);
}
VERIFY(0x02041CAC,piyo);
static void init_nonpos(){
 WWHD_FUNC(0x02043AE4,void);
 store<u32>(0x10461318,0);store<u32>(0x10461314,0);store<u32>(0x10461310,0);store<u32>(0x1046130C,0);
 gabi::call(0x028F026C,0x1018F644);
 f32 a=load<f32>(0x10006B30),b=load<f32>(0x10006B34);
 store<f32>(0x10461300,a);store<f32>(0x10461304,b);
 gabi::call(0x028ED6F8,0x10461308);gabi::call(0x028F026C,0x1018F650);
 gabi::call(0x028EAB2C,0x10461309);gabi::call(0x028F026C,0x1018F65C);
 f32 xy=load<f32>(0x10006AE0),z=load<f32>(0x10006AFC);
 store<f32>(0x1046131C,xy);store<f32>(0x10461320,xy);store<f32>(0x10461324,z);
}
VERIFY(0x02043AE4,init_nonpos);
static void sound(u32 actor,u32 id,u32 flags){
 WWHD_FUNC(0x02043B98,void,actor,id,flags);
 if(actor==0 || actor+0x37C==0)return;
 s32 reverb=gabi::call<s32>(0x02520540,load<s8>(actor+0x326));
 gabi::call(0x025E1A40,id,actor+0x37C,flags,reverb);
}
VERIFY(0x02043B98,sound);
static u32 vec_ctor(u32 p){WWHD_FUNC(0x02043C04,u32,p);if(p==0)return gabi::call<u32>(0x0273AD10,12);return p;}
VERIFY(0x02043C04,vec_ctor);
static void vec_copy(u32 out,u32 in){
 WWHD_FUNC(0x02043C30,void,out,in);
 gmem_stf32(out,load<u32>(in));gmem_stf32(out+4,load<u32>(in+4));gmem_stf32(out+8,load<u32>(in+8));
}
VERIFY(0x02043C30,vec_copy);
static void sdk_init(){
 WWHD_FUNC(0x020468F8,void);
 store<u32>(0x1046133C,0);store<u32>(0x10461334,0);store<u32>(0x10461340,0);store<u32>(0x10461338,0);
 gabi::call(0x028F026C,0x1018F668);
 f32 a=load<f32>(0x10006BA0),b=load<f32>(0x10006BA4);
 store<f32>(0x10461328,a);store<f32>(0x1046132C,b);
 gabi::call(0x028ED6F8,0x10461330);gabi::call(0x028F026C,0x1018F674);
 gabi::call(0x028EAB2C,0x10461331);gabi::call(0x028F026C,0x1018F680);
}
VERIFY(0x020468F8,sdk_init);
static u32 sdk_ctor(u32 p){
 WWHD_FUNC(0x0204698C,u32,p);
 if(p==0){p=gabi::call<u32>(0x0273AD10,0x5C);if(p==0)return 0;}
 gabi::call(0x0252CCBC,p);
 store<u32>(p+4,0);store<u32>(p+12,0);store<u32>(p+8,0);store<u32>(p,0x10006C80);
 for(u32 i=0;i<9;i++)gabi::call(0x028F521C,p+0x10+8*i,8);
 store<f32>(p+0x58,load<f32>(0x10006BC0));return p;
}
VERIFY(0x0204698C,sdk_ctor);

/* The line object is the full0x6C dBgS_LinChk, including both embedded pass checks. */
static void line_init(u32 p){
 gabi::call(0x02008FEC,p);
 store<u32>(p+0x10,0x10006A14);store<u32>(p+0x20,0x10006A24);
 store<u32>(p+0x68,1);store<u32>(p+0x64,0x10006A34);
 for(u32 i=0;i<7;i++)store<u8>(p+0x5C+i,0);
 store<u32>(p+4,p+0x64);store<u32>(p+0x58,0x10006A44);store<u32>(p,p+0x58);
}
static void line_destroy(u32 p){
 store<u32>(p+0x58,0x10006A44);store<u32>(p+0x64,0x10006904);store<u32>(p+0x20,0x100068F4);
 gabi::call(0x02008B4C,p,0);
}
static s32 kado(u32 dr){
 WWHD_FUNC(0x02041D10,s32,dr);
 gabi::Local<dBgS_LinChk> line;gabi::Local<cXyz> start,end,delta;
 u32 ln=gabi::ea(line.get()),st=gabi::ea(start.get()),en=gabi::ea(end.get()),de=gabi::ea(delta.get());
 line_init(ln);
 gabi::call(0x025F1884,load<u32>(0x1018C7B0),load<s16>(dr+0x4B6));
 gabi::call(0x025F1BF4,load<u32>(0x1018C7B0),load<s16>(dr+0x4B4));
 f32 c31=load<f32>(0x10006AB8),c30=load<f32>(0x10006AD4),c29=load<f32>(0x10006A5C),c28=load<f32>(0x10006A84),c27=load<f32>(0x10006A94);
 u32 hit=0;
 for(u32 i=0;i<2;i++){
  u32 actor=load<u32>(dr);
  f32 off=load<f32>(0x1047BDF0)+c27;
  store<f32>(st,load<f32>(actor+0x314));
  store<f32>(st+4,load<f32>(actor+0x318)+off);store<f32>(st+8,load<f32>(actor+0x31C));
  store<f32>(de,load<f32>(0x1047BDF4));store<f32>(de+4,load<f32>(0x1047BDF8));store<f32>(de+8,load<f32>(0x1047BDFC)+c30);
  gabi::call(0x0200FCD8,de,en);gabi::call(0x028E8D88,st,en,st);
  f32 x=load<f32>(0x1018F63C+4*i)*(load<f32>(0x1047BE08)+c29);
  store<f32>(de,x);store<f32>(de+4,load<f32>(0x1047BE00)+c28);store<f32>(de+8,load<f32>(0x1047BE04)+c31);
  gabi::call(0x0200FCD8,de,en);gabi::call(0x028E8D88,en,st,en);
  gabi::call(0x024F1AFC,ln,st,en,load<u32>(dr));
  u32 play=gabi::call<u32>(0x025200D4);
  if(gabi::call<s32>(0x02008860,play+0x12A0,ln)!=0)hit|=load<u32>(0x1018F634+4*i);
 }
 if(hit==3)hit=0;
 line_destroy(ln);return (s32)hit;
}
VERIFY(0x02041D10,kado);
static s32 wall_angle(u32 actor,s32 angle){
 WWHD_FUNC(0x02043C4C,s32,actor,angle);
 gabi::Local<dBgS_LinChk> line;gabi::Local<cXyz> scratch,start,diff;gabi::Local<cXyz[2]> points;
 u32 ln=gabi::ea(line.get()),sc=gabi::ea(scratch.get()),st=gabi::ea(start.get()),di=gabi::ea(diff.get()),pt=gabi::ea(points.get());
 line_init(ln);gabi::call(0x025F1884,load<u32>(0x1018C7B0),angle);
 f32 zero=load<f32>(0x10006A54);
 store<f32>(sc,zero);store<f32>(sc+4,zero);store<f32>(sc+8,load<f32>(0x10006A94));
 gabi::call(0x0200FCD8,sc,st);gabi::call(0x028E8D88,st,actor+0x314,st);
 store<f32>(sc+4,zero);store<f32>(sc+8,load<f32>(0x10006B38));store<f32>(sc,load<f32>(0x10006A68));
 for(u32 i=0;i<2;i++){
  u32 p=pt+12*i;
  gabi::call(0x0200FCD8,sc,p);store<f32>(sc,-load<f32>(sc));gabi::call(0x028E8D88,p,st,p);
  gabi::call(0x024F1AFC,ln,st,p,actor);
  u32 play=gabi::call<u32>(0x025200D4);
  if(gabi::call<s32>(0x02008860,play+0x12A0,ln)==0){line_destroy(ln);return 1;}
  u32 x=load<u32>(ln+0x30),y=load<u32>(ln+0x34),z=load<u32>(ln+0x38);
  store<u32>(p,x);store<u32>(p+4,y);store<u32>(p+8,z);
 }
 gabi::call(0x0201ADE0,pt+12,di,pt);
 f32 x=load<f32>(di),y=load<f32>(di+4),z=load<f32>(di+8);
 store<f32>(sc,x);store<f32>(sc+4,y);store<f32>(sc+8,z);
 s32 ret=(s16)(gabi::call<s32>(0x020195B0,x,z)+0x4000);
 line_destroy(ln);return ret;
}
VERIFY(0x02043C4C,wall_angle);

static void enemy_fire(u32 fire){
 WWHD_FUNC(0x02041570,void,fire);
 gabi::Local<cXyz> scale,offset,pos,velocity;
 u32 sc=gabi::ea(scale.get()),off=gabi::ea(offset.get()),po=gabi::ea(pos.get()),ve=gabi::ea(velocity.get());
 u32 actor=load<u32>(fire);
 f32 zero=load<f32>(0x10006A54);store<f32>(off,zero);store<f32>(off+4,zero);store<f32>(off+8,zero);
 s8 mode=load<s8>(fire+6);
 if(mode==0){
  s16 duration=load<s16>(fire+4);if(duration==0)return;
  store<s16>(fire+8,duration);store<s16>(fire+4,0);store<u8>(fire+6,1);
  gabi::call(0x025564B4,fire+0x208);
  f32 startY=load<f32>(0x10006AA4),randomMax=load<f32>(0x10006AB4);
  for(u32 i=0;i<10;i++){
   if(load<s8>(fire+0x10+i)<0 || load<u32>(fire+0x58+4*i)!=0)continue;
   u32 size=load<u32>(fire+0x1C+4*i);gmem_stf32(sc,size);gmem_stf32(sc+8,size);gmem_stf32(sc+4,size);
   u32 play=gabi::call<u32>(0x025200D4);
   u32 emitter=gabi::call<u32>(0x025A847C,load<u32>(play+0x5AB0),0,0x3F1,actor+0x314,0,sc,255,0,0xFFFFFFFF,0,0,0);
   store<u32>(fire+0x58+4*i,emitter);
   s16 noise=(s16)gabi::ftoi(gabi::call<f64>(0x020198D8,randomMax));
   s16 timer=(s16)(load<s16>(fire+8)-noise);if(timer<10)timer=10;
   store<s16>(fire+0x44+2*i,timer);store<f32>(fire+0x98,startY);
  }
  gabi::call(0x02515F14,fire+0xA0,250,255,actor);gabi::call(0x0251677C,fire+0xDC,0x1018F5B0);
  store<u32>(fire+0x120,fire+0xA0);return;
 }
 if(mode!=1)return;
 f32 power=load<f32>(0x10006AB8)*load<f32>(fire+0x98);
 store<u32>(fire+0x208,load<u32>(actor+0x314));store<u32>(fire+0x20C,load<u32>(actor+0x318));
 s16 lightPower=(s16)gabi::ftoi(power);
 store<u16>(fire+0x214,600);store<u32>(fire+0x210,load<u32>(actor+0x31C));store<u16>(fire+0x216,400);store<u16>(fire+0x218,120);
 store<f32>(fire+0x21C,(f32)lightPower);store<f32>(fire+0x220,load<f32>(0x10006ABC));
 gabi::call(0x0201ADE0,actor+0x314,ve,fire+0x80);
 store<u32>(fire+0x80,load<u32>(actor+0x314));store<u32>(fire+0x84,load<u32>(actor+0x318));store<u32>(fire+0x88,load<u32>(actor+0x31C));
 f32 factor=load<f32>(0x1047B620)+load<f32>(0x10006AC0);
 f32 vx=load<f32>(ve),vy=load<f32>(ve+4),vz=load<f32>(ve+8);
 f32 one=load<f32>(0x10006A5C),minus=load<f32>(0x10006AC4);
 f32 dx=vx*factor,dz=vz*factor;
 if(dx>one)dx=one;else if(dx<minus)dx=minus;
 if(dz>one)dz=one;else if(dz<minus)dz=minus;
 f32 ratio=load<f32>(0x10006A98),step=load<f32>(0x10006A74);
 gabi::call(0x0200ED84,fire+0x8C,dx,ratio,step);gabi::call(0x0200ED84,fire+0x94,dz,ratio,step);
 f32 sum=gabi::fmadds(vz,vz,gabi::fmadds(vx,vx,vy*vy));
 store<f32>(fire+0x90,load<f32>(0x1047B63C)+load<f32>(0x10006AC8));
 f64 length=gabi::call<f64>(0x028F4384,sum);
 f32 mul=load<f32>(0x1047B640)+load<f32>(0x10006ACC),limit=load<f32>(0x1047B644)+load<f32>(0x10006AD0);
 f32 target=(f32)(length*(f64)mul+(f64)one);if(target>limit)target=limit;
 gabi::call(0x0200ED84,fire+0x98,target,ratio,step);
 u8 active=0;
 for(u32 i=0;i<10;i++){
  if(load<s8>(fire+0x10+i)<0)continue;
  u32 slot=fire+0x58+4*i,emitter=load<u32>(slot);if(emitter==0)continue;
  s16 timer=load<s16>(fire+0x44+2*i);
  if(timer==0){u32 flags=load<u32>(emitter+0x254);store<u32>(emitter+0x5C,0xFFFFFFFF);store<u32>(emitter+0x254,flags|1);store<u32>(slot,0);continue;}
  store<s16>(fire+0x44+2*i,(s16)(timer-1));
  u32 model=load<u32>(fire+0xC),joints=load<u32>(load<u32>(model+0x90)+0x2C);
  s8 joint=load<s8>(fire+0x10+i);u32 matrices=load<u32>(joints+0x10);
  store<u16>(joints+4,load<u16>(joints+4)|0x10);
  gabi::call(0x028E90D4,matrices+(u32)((s32)joint*48),load<u32>(0x1018C7B0));
  gabi::call(0x0200FCD8,off,po);
  emitter=load<u32>(slot);
  f32 px=load<f32>(po),py=load<f32>(po+4),pz=load<f32>(po+8);
  if(load<u8>(emitter+0x262)>=7)py=-py;
  store<f32>(emitter+0x22C,px);store<f32>(emitter+0x230,py);store<f32>(emitter+0x234,pz);
  emitter=load<u32>(slot);
  gmem_stf32(emitter+0x28,load<u32>(fire+0x8C));gmem_stf32(emitter+0x2C,load<u32>(fire+0x90));gmem_stf32(emitter+0x30,load<u32>(fire+0x94));
  f32 particleScale=load<f32>(fire+0x1C+4*i),sy=load<f32>(fire+0x98)*particleScale;emitter=load<u32>(slot);
  store<f32>(emitter+0x238,particleScale);store<f32>(emitter+0x23C,sy);store<f32>(emitter+0x240,particleScale);
  if(load<u8>(fire+0x9D)==active){gabi::call(0x02018D40,fire+0x1F4,po);u32 play=gabi::call<u32>(0x025200D4);gabi::call(0x0200E240,play+0x26A4,fire+0xDC);}
  active++;
 }
 u8 next=(u8)(load<u8>(fire+0x9D)+1);store<u8>(fire+0x9D,next<active?next:0);
 u32 soundPos=actor+0x37C;
 if(soundPos){s32 reverb=gabi::call<s32>(0x02520540,load<s8>(actor+0x326));gabi::call(0x025E1A40,0x6103,soundPos,0,reverb);}
 s16 timer=load<s16>(fire+8);
 if(timer==0){
  store<u8>(fire+6,0);gabi::call(0x0255A374,fire+0x208);gabi::call(0x02018D40,fire+0x1F4,0x1046131C);
  u32 play=gabi::call<u32>(0x025200D4);gabi::call(0x0200E240,play+0x26A4,fire+0xDC);timer=load<s16>(fire+8);
 }else if(soundPos){s32 reverb=gabi::call<s32>(0x02520540,load<s8>(actor+0x326));gabi::call(0x025E1A40,0x5101,soundPos,0,reverb);timer=load<s16>(fire+8);}
 if(timer!=0)store<s16>(fire+8,(s16)(timer-1));
}
VERIFY(0x02041570,enemy_fire);

/* Inlined HD ice_bg_check portion02040CDC..02040E94; composed into the ice entry next. */
static bool ice_bg_check(u32 ice,f32 savedY,f32 zero){
 u32 actor=load<u32>(ice);bool shatter=false;
 u32 play=gabi::call<u32>(0x025200D4);gabi::call(0x024F08A8,ice+0x1F4,play+0x12A0);
 u32 flags=load<u32>(ice+0x21C);
 if(flags&0x800){
  if(load<f32>(actor+0x318)<load<f32>(ice+0x3B0)+load<f32>(0x10006A94)){
   store<f32>(ice+0x1C,zero);if(load<s8>(actor+0x3A1)<=0)shatter=true;
  }
 }else if(flags&0x20){
  store<f32>(ice+0x24,load<f32>(ice+0x24)*load<f32>(0x10006A98));
  if(load<s8>(actor+0x3A1)<=0 || savedY<load<f32>(0x1047B648)+load<f32>(0x10006A9C))shatter=true;
  f32 choose=savedY-load<f32>(0x10006AA0);
  store<f32>(ice+0x1C,(f32)ppc_fsel(choose,load<f32>(0x10006A68),zero));
  store<s16>(ice+0x14,0);
 }else{
  if(gabi::call<s32>(0x0246B6A4,load<f32>(actor+0x314),load<f32>(actor+0x31C))!=0){
   f32 wave=gabi::call<f32>(0x0246BA0C,load<f32>(actor+0x314),load<f32>(actor+0x31C));
   if(load<f32>(actor+0x318)<wave+load<f32>(0x10006A94)){
    store<f32>(ice+0x1C,zero);if(load<s8>(actor+0x3A1)<=0)shatter=true;
   }
  }
 }
 if(load<u32>(ice+0x21C)&0x10){
  f32 speed=load<f32>(ice+0x24);if(std::fabs(speed)>load<f32>(0x10006A68))shatter=true;
  store<s16>(ice+0x12,(s16)(load<s16>(ice+0x12)-0x8000));store<f32>(ice+0x24,speed*load<f32>(0x10006A98));
 }
 return shatter;
}

static void ice_particle(u32 id,u32 pos,u32 angle,u32 scale){
 u32 play=gabi::call<u32>(0x025200D4);
 gabi::call<u32>(0x025A847C,load<u32>(play+0x5AB0),0,id,pos,angle,scale,255,0,0xFFFFFFFF,0,0,0);
}
static void ice_pos_scale(u32 ice,u32 actor,u32 pos,u32 scale){
 f32 size=load<f32>(ice+0x1AC);store<f32>(scale,size);store<f32>(scale+4,size);store<f32>(scale+8,size);
 store<f32>(pos,load<f32>(actor+0x314));store<f32>(pos+4,load<f32>(actor+0x318));store<f32>(pos+8,load<f32>(actor+0x31C));
 store<f32>(pos+4,load<f32>(pos+4)+load<f32>(ice+8));
}
static void ice_render(u32 ice,u32 actor,f32 shiverX,f32 shiverZ,f32 one){
 store<u16>(actor+0x320,load<u16>(actor+0x328));store<u16>(actor+0x322,load<u16>(actor+0x32A));store<u16>(actor+0x324,load<u16>(actor+0x32C));
 gabi::call(0x0200ED84,ice+0x28,load<f32>(ice+0x2C),one,load<f32>(0x10006AB0));
 gabi::call(0x028E93CC,0x1048D0CC,load<f32>(actor+0x314)+shiverX,load<f32>(actor+0x318)+load<f32>(ice+0x2C),load<f32>(actor+0x31C)+shiverZ);
 gabi::call(0x025F1C28,0x1048D0CC,load<s16>(actor+0x32A));gabi::call(0x025F1BF4,0x1048D0CC,load<s16>(actor+0x328));gabi::call(0x025F1C5C,0x1048D0CC,load<s16>(actor+0x32C));
 gabi::call(0x025F2518,load<f32>(actor+0x330),load<f32>(actor+0x334),load<f32>(actor+0x338));
 store<u32>(actor+0x2E0,load<u32>(actor+0x2E0)|0x400);
}
static u32 enemy_ice(u32 ice){
 WWHD_FUNC(0x020402C8,u32,ice);
 u32 play=gabi::call<u32>(0x025200D4),actor=load<u32>(ice),player=load<u32>(play+0x5B2C);
 f32 small=load<f32>(0x10006A58),zero=load<f32>(0x10006A54),one=load<f32>(0x10006A5C),gravity=load<f32>(0x10006A60);
 u32 matrix=load<u32>(0x1018C7B0);
 gabi::Local<cXyz> position,scale,direction,offset;
 u32 pos=gabi::ea(position.get()),sc=gabi::ea(scale.get()),dir=gabi::ea(direction.get()),off=gabi::ea(offset.get());
 auto soundIce=[&](u32 id){if(actor+0x37C){s32 rv=gabi::call<s32>(0x02520540,load<s8>(actor+0x326));gabi::call(0x025E1A40,id,actor+0x37C,0,rv);}};
 auto item=[&](){gabi::call(0x025D9154,pos,load<u32>(actor+0x3A4),load<s8>(actor+0x326),actor+0x320,load<u8>(actor+0x3A8));};
 auto freeActor=[&](){gabi::call(0x025D57E0,actor);gabi::call(0x025BA5D4,load<u32>(0x101F84DC)+0x20,load<u16>(actor+0x2D8),load<s8>(actor+0x2FE));};
 s8 light=load<s8>(ice+6);
 if(light){
  ice_pos_scale(ice,actor,pos,sc);store<s8>(ice+6,(s8)(light+1));
  if(light==1){ice_particle(0x272,pos,0,sc);store<u16>(actor+0x1B4,255);store<u16>(actor+0x1B2,255);store<u16>(actor+0x1B0,255);store<f32>(actor+0x1B8,zero);store<f32>(actor+0x1BC,load<f32>(0x10006A64));soundIce(0x5904);store<u32>(actor+0x39C,load<u32>(actor+0x39C)&~4u);return 1;}
  gabi::call(0x0200ED84,actor+0x1BC,load<f32>(0x10006A68),one,load<f32>(0x10006A6C));
  gabi::call(0x028E93CC,0x1048D0CC,load<f32>(actor+0x314),load<f32>(actor+0x318),load<f32>(actor+0x31C));
  gabi::call(0x025F24E0,zero,load<f32>(ice+8),zero);gabi::call(0x025F2518,load<f32>(ice+0x1A4),load<f32>(ice+0x1A8),load<f32>(ice+0x1A4));gabi::call(0x025F24E0,zero,-load<f32>(ice+8),zero);
  gabi::call(0x025F1C28,0x1048D0CC,load<s16>(actor+0x32A));gabi::call(0x025F1BF4,0x1048D0CC,load<s16>(actor+0x328));gabi::call(0x025F1C5C,0x1048D0CC,load<s16>(actor+0x32C));gabi::call(0x025F2518,load<f32>(actor+0x330),load<f32>(actor+0x334),load<f32>(actor+0x338));
  s8 current=load<s8>(ice+6),threshold=(s8)(load<s16>(0x1047BE6A)+70);
  if(current<threshold){f32 step=load<f32>(0x1047BDF0)+load<f32>(0x10006A70);gabi::call(0x0200EDC8,ice+0x1A4,small,step);gabi::call(0x0200EDC8,ice+0x1A8,small,load<f32>(0x1047BDF0)+load<f32>(0x10006A70));return 1;}
  if(current==threshold)soundIce(0x5905);
  gabi::call(0x0200ED84,ice+0x1A8,load<f32>(0x1047BDF4)+gravity,small,load<f32>(0x1047BDF8)+one);
  gabi::call(0x0200EDC8,ice+0x1A4,load<f32>(0x1047BDFC)+small,load<f32>(0x1047BE00)+load<f32>(0x10006A74));
  if(load<s8>(ice+6)>(s8)(load<s16>(0x1047BE6C)+90)){freeActor();if(!(actor&&load<u16>(actor+8)==210))item();if(load<u8>(ice+0x1B1))gabi::call(0x025B9E38,load<u32>(0x101F84DC)+0x20,load<u8>(ice+0x1B1),load<s8>(actor+0x326));store<s8>(actor+0x3A1,-128);}return 1;
 }
 u32 frozen=0;bool movement=false;
 switch(load<s8>(ice+0xD)){
 case 0:
  gabi::call(0x02515F14,ice+0x30,250,255,actor);gabi::call(0x02516518,ice+0x6C,0x1018F5F0);store<u32>(ice+0xB0,ice+0x30);
  gabi::call(0x020184DC,ice+0x184,load<f32>(ice+0x1A0));gabi::call(0x02018428,ice+0x184,load<f32>(ice+0x19C));
  gabi::call(0x024F06B4,ice+0x1F4,actor+0x314,actor+0x300,actor,1,ice+0x1B4,ice+0x18,0,0);gabi::call(0x024EFF44,ice+0x1B4,load<f32>(0x10006A78),load<f32>(ice+0x1A0));
  if(load<f32>(ice+0x1AC)<small)store<f32>(ice+0x1AC,one);if(std::fabs(load<f32>(ice+8))<small)store<f32>(ice+8,load<f32>(0x10006A6C));
  store<f32>(ice+0x1A8,one);store<f32>(ice+0x1A4,one);store<u8>(ice+0xD,1);store<u32>(actor+0x2E0,(load<u32>(actor+0x2E0)|0x08000000)&~0x400u);gabi::call(0x0200EDC8,ice+0x28,one,load<f32>(0x10006AB0));return 0;
 case 1:
  if(load<s16>(ice+4)==0)return 0;store<s16>(ice+0xE,load<s16>(ice+4));store<s16>(ice+4,0);store<u8>(ice+0xD,2);
  if(load<s8>(ice+0xC)==0){store<f32>(ice+0x1C,load<f32>(0x10006A7C));store<s16>(ice+0x14,(s16)gabi::ftoi(gabi::call<f64>(0x02019918,load<f32>(0x10006A80))));}
  store<s8>(actor+0x3A1,(s8)(load<s8>(actor+0x3A1)-4));if(load<s8>(actor+0x3A1)<=0){store<f32>(ice+0x1C,zero);store<s16>(ice+0x10,40);}soundIce(0x58FA);ice_pos_scale(ice,actor,pos,sc);ice_particle(0x274,pos,0,sc);frozen=1;
  if(load<s8>(ice+0xC)==1){movement=true;break;}
  [[fallthrough]];
 case 2:
  frozen=1;if(load<s8>(ice+0xC)==1){movement=true;break;}store<u32>(actor+0x39C,load<u32>(actor+0x39C)|0x10);store<u8>(actor+0x38C,0x12);if(load<u32>(actor+0x2E0)&0x2000){store<u32>(actor+0x39C,load<u32>(actor+0x39C)&~0x10u);store<u8>(ice+0xD,3);if(load<s8>(ice+0xC)==2)store<u8>(ice+0xC,0);}movement=true;break;
 case 3:{
  frozen=1;if(load<u32>(actor+0x2E0)&0x2000)break;
  gabi::Local<dBgS_LinChk> line;u32 ln=gabi::ea(line.get());line_init(ln);
  store<f32>(pos,load<f32>(player+0x314));store<f32>(pos+4,load<f32>(player+0x318)+load<f32>(0x10006A84));store<f32>(pos+8,load<f32>(player+0x31C));
  gabi::call(0x025F1884,matrix,load<s16>(player+0x32A));store<f32>(dir,zero);store<f32>(dir+4,zero);store<f32>(dir+8,load<f32>(0x1047C050)+load<f32>(0x10006A88));gabi::call(0x0200FCD8,dir,off);
  store<f32>(dir,load<f32>(actor+0x314)+load<f32>(off));store<f32>(dir+4,load<f32>(actor+0x318)+load<f32>(0x10006A84));store<f32>(dir+8,load<f32>(actor+0x31C)+load<f32>(off+8));
  gabi::call(0x024F1AFC,ln,pos,dir,actor);u32 p=gabi::call<u32>(0x025200D4);s32 crossed=gabi::call<s32>(0x02008860,p+0x12A0,ln);f32 speed=load<f32>(actor+0x370);
  if(crossed){f32 x=load<f32>(player+0x314);store<f32>(actor+0x314,x);f32 y=load<f32>(player+0x318);store<f32>(actor+0x318,y);f32 z=load<f32>(player+0x31C);store<f32>(actor+0x300,x);store<f32>(actor+0x31C,z);store<f32>(actor+0x304,y);store<f32>(actor+0x308,z);}
  if(speed>zero){store<f32>(ice+0x24,load<f32>(0x1047B624)+load<f32>(0x10006A88));store<f32>(ice+0x1C,load<f32>(0x1047B628)+load<f32>(0x10006A8C));}else{store<f32>(ice+0x24,gravity);store<f32>(ice+0x1C,load<f32>(0x10006A90));}
  store<u16>(ice+0x12,load<u16>(player+0x32A));store<u8>(ice+0xD,2);line_destroy(ln);break;
 }
 default:store<u32>(actor+0x2E0,load<u32>(actor+0x2E0)&~0x400u);gabi::call(0x0200EDC8,ice+0x28,one,load<f32>(0x10006AB0));return 0;
 }
 if(movement){
  s16 delay=load<s16>(ice+0x10);if(delay==0){gabi::call(0x025F1884,matrix,load<s16>(ice+0x12));store<f32>(dir,zero);store<f32>(dir+4,zero);store<f32>(dir+8,load<f32>(ice+0x24));gabi::call(0x0200FCD8,dir,off);store<f32>(ice+0x18,load<f32>(off));store<f32>(ice+0x20,load<f32>(off+8));
   if(load<s8>(ice+0xC)==2)store<f32>(ice+0x1C,zero);else{gabi::call(0x028E8D88,actor+0x314,ice+0x18,actor+0x314);store<f32>(ice+0x1C,load<f32>(ice+0x1C)-gravity);}store<s16>(actor+0x32A,(s16)(load<s16>(actor+0x32A)+load<s16>(ice+0x14)));if(ice_bg_check(ice,load<f32>(ice+0x1C),zero))store<s16>(ice+0xE,-1);
  }else store<s16>(ice+0x10,(s16)(delay-1));
  if(ice+0x30)gabi::call(0x028E8D88,actor+0x314,ice+0x30,actor+0x314);
  u32 flags=load<u32>(ice+0x6C);store<u32>(ice+0x6C,std::fabs(load<f32>(ice+0x24))>gravity?flags|1:flags&~1u);gabi::call(0x020182E0,ice+0x184,actor+0x314);u32 p=gabi::call<u32>(0x025200D4);gabi::call(0x0200E240,p+0x26A4,ice+0x6C);
  if(load<s16>(ice+0xE)>=23&&gabi::call<s32>(0x025162A4,ice+0x6C)){u32 hit=gabi::call<u32>(0x02516300,ice+0x6C),type=load<u32>(hit+0x10);if(type&0x100000){store<u8>(ice+6,1);store<s16>(ice+0xE,1);}else if(type&0x200)store<s16>(ice+0xE,1);else if(type&0x50020)store<s16>(ice+0xE,type&0x10000?-2:-1);else gabi::call(0x02518CC8,actor,hit,0x42);}
 }
 f32 shiverX=zero,shiverZ=zero;s16 timer=load<s16>(ice+0xE);bool shattered=false,expire=false;
 if(timer>0){timer--;store<s16>(ice+0xE,timer);expire=timer==0;}
 else if(timer<0){ice_pos_scale(ice,actor,pos,sc);ice_particle(0x273,pos,0,sc);ice_particle(0x274,pos,0,sc);
  if(load<s16>(ice+0xE)==-2){ice_particle(0x10,pos,0,0);u32 p=gabi::call<u32>(0x025200D4),pl=load<u32>(p+0x5B2C);s16 yaw=gabi::call<s16>(0x025D6894,actor,pl);gabi::Local<csXyz> angle;gabi::call(0x0201A478,gabi::ea(angle.get()),0,yaw,0);f32 size=load<f32>(0x10006AA4);store<f32>(sc,size);store<f32>(sc+4,size);store<f32>(sc+8,size);ice_particle(0xD,pos,gabi::ea(angle.get()),sc);store<u8>(0x101EACB7,8);}
  soundIce(0x5902);item();store<s16>(ice+0xE,0);if(load<u8>(ice+0x1B1))gabi::call(0x025B9E38,load<u32>(0x101F84DC)+0x20,load<u8>(ice+0x1B1),load<s8>(actor+0x326));timer=load<s16>(ice+0xE);shattered=true;expire=timer==0;
 }else{ice_render(ice,actor,zero,zero,one);return frozen;}
 if(expire){store<u8>(ice+0xD,1);gabi::call(0x020182E0,ice+0x184,0x1046131C);u32 p=gabi::call<u32>(0x025200D4);gabi::call(0x0200E240,p+0x26A4,ice+0x6C);if(load<u32>(actor+0x2E0)&0x2000)gabi::call(0x025D9D24,actor);if(shattered){freeActor();store<s8>(actor+0x3A1,-128);}else frozen=0;timer=load<s16>(ice+0xE);}
 if(timer<50){shiverX=(f32)((timer&1)*2-1);shiverZ=(f32)(((timer+1)&1)*2-1);}
 if(timer==20){ice_pos_scale(ice,actor,pos,sc);ice_particle(0x277,pos,0,sc);soundIce(0x5903);}ice_render(ice,actor,shiverX,shiverZ,one);return frozen;
}
VERIFY(0x020402C8,enemy_ice);
/* HD reserves21 joints in each vector/angle array while animation updates20. */
struct DamageAngle : be<s16> {
 template<class T> DamageAngle& operator=(const T& x){if constexpr(std::is_floating_point_v<T>)set((s16)gabi::ftoi(x));else set((s16)x);return *this;}
 DamageAngle& operator=(const DamageAngle& x){set(x.get());return *this;}
 template<class T> DamageAngle& operator+=(const T& x){return *this=get()+x;}
 template<class T> DamageAngle& operator-=(const T& x){return *this=get()-x;}
};
struct DamageAngles {DamageAngle x,y,z;};
static_assert(sizeof(DamageAngles)==6);
struct DamageSmoke {u8 data[0x20];void end(){gabi::call(0x025A5F88,this);}};
struct DamageStts {u8 data[0x3C];cXyz* GetCCMoveP(){return gabi::at<cXyz>(gabi::ea(this));}};
struct DamageLine {u8 data[0x6C];void Init(){line_init(gabi::ea(this));}void End(){line_destroy(gabi::ea(this));}void Set(cXyz* a,cXyz* b,fopAc_ac_c* actor){gabi::call(0x024F1AFC,this,a,b,actor);}cXyz& GetCross(){return *gabi::at<cXyz>(gabi::ea(this)+0x30);}};
struct DamageGround {u8 data[0x54];
 void Init(bool spl){u32 a=gabi::ea(this);gabi::call(0x02008E0C,this);for(u32 i=0;i<7;i++)store<u8>(a+0x44+i,i==0?1:0);store<u32>(a+0x50,spl?14:4);store<u32>(a,a+0x40);store<u32>(a+4,a+0x4C);store<u32>(a+0x10,spl?0x100069D4:0x10006994);store<u32>(a+0x20,spl?0x100069E4:0x100069A4);store<u32>(a+0x40,spl?0x10006A04:0x100069C4);store<u32>(a+0x4C,spl?0x100069F4:0x100069B4);}
 void SetPos(const cXyz* v){gabi::at<cXyz>(gabi::ea(this)+0x24)->set(v->x,v->y,v->z);}
 void End(){u32 a=gabi::ea(this);store<u32>(a+0x20,0x10006924);store<u32>(a+0x40,0x10006944);store<u32>(a+0x4C,0x10006904);gabi::call(0x02008DAC,this,0);}
};
static_assert(sizeof(DamageLine)==0x6C&&sizeof(DamageGround)==0x54);
struct DamageState {
 enum { TYPE_MOBLIN=1,TYPE_BOKOBLIN=2,TYPE_DARKNUT=3 };
 gptr<fopAc_ac_c> mpEnemy;be<s16> mMode,mAction,mEnemyType;u8 padA[2];be<s32> mTimer;
 DamageAngles m010[21],m088[21];cXyz m100[21],m1F0[21],m2E0[21];
 /*400*/ be<s32> m3D0[14];
 /*438*/ be<s32> m408;
 /*43C*/ be<s32> m40C;
 /*440*/ be<s32> m410;
 /*444*/ be<s32> m414;
 /*448*/ be<s32> m418;
 u8 pad44C[0x8];
 /*454*/ be<s32> m420;
 /*458*/ be<s32> m424;
 /*45C*/ be<f32> m428;
 /*460*/ cXyz m42C;
 /*46C*/ be<s32> m438;
 u8 pad470[0x4];
 /*474*/ be<s16> m440;
 /*476*/ be<s16> m442;
 /*478*/ be<s16> m444;
 /*47A*/ be<s16> m446;
 /*47C*/ be<s16> m448;
 u8 pad47E[0x2];
 /*480*/ cXyz m44C;
 /*48C*/ cXyz m458;
 u8 pad498[0x4];
 /*49C*/ be<f32> m468;
 /*4A0*/ be<f32> m46C;
 /*4A4*/ be<f32> m470;
 /*4A8*/ be<f32> m474;
 /*4AC*/ be<f32> m478;
 /*4B0*/ be<s32> m47C;
 /*4B4*/ be<s16> m480;
 /*4B6*/ be<s16> m482;
 /*4B8*/ be<s16> m484;
 /*4BA*/ be<s16> m486;
 /*4BC*/ be<s16> m488;
 /*4BE*/ be<s16> m48A;
 /*4C0*/ csXyz m48C;
 /*4C6*/ be<s16> m492;
 /*4C8*/ be<s16> m494;
 /*4CA*/ be<s16> m496;
 /*4CC*/ be<s16> m498;
 /*4CE*/ be<s16> m49A;
 /*4D0*/ be<s16> m49C;
 /*4D2*/ be<s16> m49E;
 /*4D4*/ DamageAngle m4A0;
 /*4D6*/ DamageAngle m4A2;
 /*4D8*/ DamageAngle m4A4;
 /*4DA*/ DamageAngle m4A6;
 /*4DC*/ DamageAngle m4A8;
 /*4DE*/ DamageAngle m4AA;
 /*4E0*/ DamageAngle m4AC;
 /*4E2*/ DamageAngle m4AE;
 /*4E4*/ DamageAngle m4B0;
 /*4E6*/ DamageAngle m4B2;
 /*4E8*/ DamageAngle m4B4;
 /*4EA*/ DamageAngle m4B6;
 /*4EC*/ DamageAngle m4B8;
 /*4EE*/ DamageAngle m4BA;
 /*4F0*/ DamageAngle m4BC;
 /*4F2*/ DamageAngle m4BE;
 /*4F4*/ be<s16> m4C0;
 u8 pad4F6[0x6];
 /*4FC*/ be<s16> m4C8[3];
 /*502*/ be<s16> mInvincibleTimer;
 /*504*/ be<s16> m4D0;
 u8 pad506[0x2];
 /*508*/ be<f32> m4D4;
 u8 pad50C[4];dBgS_AcchCir mAcchCir;dBgS_ObjAcch mAcch;
 /*714*/ be<s32> m6E0;
 /*718*/ be<f32> mSpawnY;
 /*71C*/ cXyz m6E8;
 /*728*/ cXyz m6F4[2];
 /*740*/ be<u8> m70C;
 /*741*/ be<u8> m70D;
 /*742*/ be<u8> m70E;
 u8 pad743[0x1];
 /*744*/ be<u8> m710;
 /*745*/ be<u8> m711;
 /*746*/ be<u8> m712;
 /*747*/ be<u8> m713;
 /*748*/ gptr<fopAc_ac_c> m714;
 /*74C*/ be<s16> m718;
 /*74E*/ be<s16> m71A;
 u8 pad750[0x2];
 /*752*/ be<s16> m71E;
 /*754*/ be<f32> mMaxFallDistance;
 DamageStts mStts;
 /*794*/ cXyz mParticlePos;
 /*7A0*/ csXyz mParticleAngle;
 u8 pad7A6[2];DamageSmoke mParticleCallBack;
 /*7C8*/ be<s16> m794;
 u8 pad7CA[0x2];
 /*7CC*/ be<f32> m798;
 /*7D0*/ gptr<cXyz> m79C;
 /*7D4*/ cXyz m7A0;
 /*7E0*/ csXyz m7AC;
 /*7E6*/ be<s16> m7B2;
 /*7E8*/ be<s16> m7B4;
 /*7EA*/ be<u8> m7B6;
 u8 pad7EB[0x1];
 /*7EC*/ be<u32> m7B8;
};
static_assert(offsetof(DamageState,m47C)==0x4B0);
static_assert(offsetof(DamageState,m3D0)==0x400);
static_assert(offsetof(DamageState,mAcch)==0x550);
static_assert(offsetof(DamageState,mSpawnY)==0x718);
static void damage_animation(DamageState* dr) {
    gabi::Local<csXyz> angleTemp;csXyz& csxyz_temp=*angleTemp.get();

    for(int i = 0; i < 0x14; i++) {
        dr->m010[i].x = dr->m010[i].y = dr->m010[i].z = 0;
    }

    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    s16 maxSpeed;
    if(dr->m47C != 0 || load<u8>(0x1018F5AC) != 0) {
        maxSpeed = 0x3000;

        f32 temp = (s16)(dr->m482 + 0x8000 - dr->mpEnemy->current.angle.y);
        f32 zero = 0.0f;
        f32 temp2 = temp;
        if(temp2 > 5000.0f) {
            temp2 = 5000.0f;
        }
        if(temp2 < -22000.0f) {
            temp2 = -22000.0f;
        }
        dr->m010[8].y = (0.4f*temp2)+8192.0f;
        dr->m010[4].z = -(0.4f*temp2)-8192.0f;
        dr->m010[8].z = 0.4f * zero + 0x2000;
        dr->m010[4].y = 0.4f * zero + 0x2000;

        f32 temp3 = temp;
        if(temp3 > 22000.0f) {
            temp3 = 22000.0f;
        }
        if(temp3 < -5000.0f) {
            temp3 = -5000.0f;
        }
        dr->m010[9].y = gabi::fmadds(temp3,0.4f,-8192.0f);
        dr->m010[5].z = gabi::fmadds(temp3,0.4f,-8192.0f);
        dr->m010[9].z = 0.4f * zero + 0x2000;
        dr->m010[5].y = 0.4f * -zero - 0x2000;

        f32 temp4 = temp;
        if(temp4 > 20000.0f) {
            temp4 = 20000.0f;
        }
        if(temp4 < -20000.0f) {
            temp4 = -20000.0f;
        }
        dr->m010[0x0C].x = 0.2f * temp4;
        dr->m010[0x13].x = 0.2f * temp4;
        dr->m010[0x0C].z = 0.2f * -zero;
        dr->m010[0x13].z = 0.2f * -zero;

        f32 temp5 = temp;
        if(temp5 > 7000.0f) {
            temp5 = 7000.0f;
        }
        if(temp5 < -10000.0f) {
            temp5 = -10000.0f;
        }
        dr->m010[6].x = (3000.0f - temp5) + REG0_S(0);
        dr->m010[6].z = (-zero - 0x4000) + REG0_S(1);

        f32 temp6 = temp;
        if(temp6 > 10000.0f) {
            temp6 = 10000.0f;
        }
        if(temp6 < -7000.0f) {
            temp6 = -7000.0f;
        }
        dr->m010[7].x = (-temp6 - 3000.0f);
        dr->m010[7].z = (-zero - 0x4000);
    }
    else {
        dr->m010[6].y += dr->m4B8;
        dr->m010[7].y -= dr->m4BA;
        dr->m010[4].y += dr->m4BC;
        dr->m010[5].y -= dr->m4BE;
        maxSpeed = 0x400;
    }

    cLib_addCalcAngleS2(&dr->m4A6, dr->m4AE, 1, 0x800);
    cLib_addCalcAngleS2(&dr->m4A4, dr->m4AC, 1, 0x800);
    cLib_addCalcAngleS2(&dr->m4A2, dr->m4AA, 1, 0x800);
    cLib_addCalcAngleS2(&dr->m4A0, dr->m4A8, 1, 0x800);
    cLib_addCalcAngleS2(&dr->m496, dr->m49A, 2, 0x200);
    cLib_addCalcAngleS2(&dr->m498, dr->m49C, 2, 0x400);

    dr->m010[6].z = dr->m010[6].z - dr->m4A4 / 2 - dr->m4B4;
    dr->m010[2].z = dr->m010[2].z + dr->m4A4 + dr->m4B0;
    dr->m010[7].z = dr->m010[7].z - dr->m4A6 / 2 - dr->m4B6;
    dr->m010[3].z = dr->m010[3].z + dr->m4A6 + dr->m4B2;
    dr->m010[0].z += dr->m4A0;
    dr->m010[1].z += dr->m4A2;
    dr->m010[0x12].y += dr->m496;
    dr->m010[0xD].y += dr->m498;

    if(dr->m70D != 0) {
        dr->m010[0x13].z += -0x960;
    }

    if(dr->m70C != 0 && dr->mpEnemy->speed.y > 0.0f) {
        dr->m010[0xC].z += -3000;
        dr->m010[0x13].z += -3000;
    }

    gabi::Local<cXyz> vectorTemp,vectorTemp2;cXyz& cxyz_temp=*vectorTemp.get();cXyz& cxyz_temp2=*vectorTemp2.get();
    if(dr->m470 > 200.0f) {
        s16 angle = cM_atan2s(-dr->m42C.x, -dr->m42C.z);
        cxyz_temp.x = 0.0f;
        cxyz_temp.y = 0.0f;
        cxyz_temp.z = dr->m470;
        mDoMtx_YrotS(calc_mtx(), angle - dr->mpEnemy->current.angle.y);
        MtxPosition(&cxyz_temp, &cxyz_temp2);

        if(dr->mEnemyType == DamageState::TYPE_BOKOBLIN) {
            dr->m010[0xC].z -= cxyz_temp2.z * 4.0f;
            dr->m010[0xC].y += cxyz_temp2.x * 4.0f;
            dr->m010[0xD].y += cxyz_temp2.x * 6.0f;
            dr->m010[0xD].z -= cxyz_temp2.z * 6.0f;
        }
        else if(dr->mEnemyType == DamageState::TYPE_DARKNUT) {
            dr->m010[0xD].y += (s16)gabi::ftoi((f32)cxyz_temp2.z * (REG_F(8,5)+2.0f));
            dr->m010[0x12].y += (s16)gabi::ftoi((f32)cxyz_temp2.z * (REG_F(8,5)+2.0f));
            dr->m010[0xC].y += (s16)gabi::ftoi((f32)cxyz_temp2.z * (REG_F(8,5)+2.0f));
            dr->m010[0xD].x += (s16)gabi::ftoi((f32)cxyz_temp2.x * (REG_F(8,6)+-2.0f));
            dr->m010[0x12].x += (s16)gabi::ftoi((f32)cxyz_temp2.x * (REG_F(8,6)+-2.0f));
            dr->m010[0xC].x += (s16)gabi::ftoi((f32)cxyz_temp2.x * (REG_F(8,6)+-2.0f));
        }
        else {
            dr->m010[0x13].z -= cxyz_temp2.z;
            dr->m010[0x13].y += cxyz_temp2.x;
            dr->m010[0xC].z -= cxyz_temp2.z;
            dr->m010[0xC].y += cxyz_temp2.x;
            dr->m010[0xD].y += cxyz_temp2.x * 4.0f;
            dr->m010[0xD].z -= cxyz_temp2.z * 4.0f;
        }

        maxSpeed = 0x3000;
    }
    else if(dr->m710 != 0 || dr->m49E != 0) {
        f32 f31 = 0.5f;
        maxSpeed = 0x800;

        s16 temp;
        s16 temp2;
        if(dr->m710 == 1 || dr->m710 == 5 || dr->m49E != 0) {
            if(dr->m710 == 1 || dr->m49E != 0) {
                if(dr->m49E != 0) {
                    dr->m49E--;
                    maxSpeed = 0x2000;
                    temp2 = (s16)gabi::ftoi((REG0_F(6)+1000.0f)*cM_ssin(dr->m49E*(REG0_S(5)+16000)));
                    temp = 0;
                }
                else {
                    cxyz_temp.x = dr->m714->eyePos.x - dr->mpEnemy->current.pos.x;
                    cxyz_temp.y = dr->m714->eyePos.y - (dr->mpEnemy->current.pos.y + 100.0f);
                    cxyz_temp.z = dr->m714->eyePos.z - dr->mpEnemy->current.pos.z;
                    temp2 = cM_atan2s(cxyz_temp.x, cxyz_temp.z) - dr->mpEnemy->current.angle.y;
                    temp = -cM_atan2s(cxyz_temp.y, std_sqrtf(gabi::fmadds((f32)cxyz_temp.x,(f32)cxyz_temp.x,(f32)cxyz_temp.z*(f32)cxyz_temp.z)));
                }
            }
            else {
                temp = dr->m718;
                temp2 = dr->m71A;
            }

            if(temp2 > 0x2000) {
                temp2 = 0x2000;
            }
            else if(temp2 < -0x2000) {
                temp2 = -0x2000;
            }
            if(temp > 0x2000) {
                temp = 0x2000;
            }
            else if(temp < -0x2000) {
                temp = -0x2000;
            }

            if(dr->mEnemyType == DamageState::TYPE_DARKNUT) {
                dr->m010[0xD].x += temp2 >> 2;
                dr->m010[0xD].y -= temp >> 2;
                dr->m010[0x12].x += temp2 >> 2;
                dr->m010[0x12].y -= temp >> 2;
            }
            else if(dr->m710 == 1) {
                if(dr->mEnemyType == DamageState::TYPE_BOKOBLIN) {
                    dr->m010[0xD].x += temp2;
                    dr->m010[0xD].y -= temp2 >> 2;
                    dr->m010[0xD].z += temp;
                }
                else {
                    dr->m010[0xD].y -= temp2;
                    dr->m010[0xC].y -= temp2 * f31;
                    dr->m010[0xC].x = dr->m010[0xC].y + temp2 * f31;
                    dr->m010[0xD].z += temp;
                }
            }
            else {
                dr->m010[0xD].y -= (int)temp2;
                dr->m010[0xC].x = dr->m010[0xC].y + temp2;
                dr->m010[0xD].z += temp;
            }
        }
        else if(dr->m710 == 3) {
            temp = -6000;
            temp2 = -10000;
        }
        else if(dr->m710 == 4) {
            temp = 6000;
            temp2 = 10000;
        }
        else if(dr->m710 == 6) {
            dr->m010[0x12].y += dr->m71A;
            dr->m010[0x12].x += REG0_S(6);
            dr->m010[0x12].z += dr->m718;

            maxSpeed = 0x300;
        }

        if(dr->m710 == 3 || dr->m710 == 4) {
            maxSpeed = 0x800;

            dr->m010[0xC].x -= (int)temp;
            dr->m010[0xC].y += (int)temp;
            dr->m010[0xD].y += temp2;
        }
    }

    if(dr->m711 != 0) {
        dr->m010[8].x = 8000;
        dr->m010[9].x = -8000;
        maxSpeed = 0x3000;
    }

    if(dr->mAction != 5 && dr->m488 != 1) {
        cxyz_temp.x = dr->m48C.z;
        cxyz_temp.y = 0.0f;
        cxyz_temp.z = dr->m48C.x;
        mDoMtx_YrotS(calc_mtx(), dr->mpEnemy->current.angle.y);
        MtxPosition(&cxyz_temp, &cxyz_temp2);
        cLib_addCalcAngleS2(&dr->m492, (s16)gabi::ftoi((f32)cxyz_temp2.x), 4, 0x100);
        cLib_addCalcAngleS2(&dr->m494, (s16)gabi::ftoi((f32)cxyz_temp2.z), 4, 0x100);
    }
    else {
        cLib_addCalcAngleS2(&dr->m492, 0, 4, 0x100);
        cLib_addCalcAngleS2(&dr->m494, 0, 4, 0x100);
    }

    dr->m010[0xC].y -= (f32)dr->m492;
    dr->m010[0xC].z -= (f32)dr->m494;

    cLib_addCalc0(&dr->m470, 0.5f, 200.0f);

    if(dr->m478 == 0.0f && dr->m488 == 1) {
        if(dr->m70D == 0) {
            if(dr->m3D0[2] == 0 && dr->m408 == 0) {
                dr->m010[6].z += dr->m4B4;
                dr->m010[2].z = dr->m010[2].z - dr->m4B0 + 13000;

                if(dr->m440 > 2000) {
                    dr->m010[2].z += -7000;
                }
            }

            if(dr->m3D0[3] == 0 && dr->m40C == 0) {
                dr->m010[7].z += dr->m4B6;
                dr->m010[3].z = dr->m010[3].z - dr->m4B2 + 13000;

                if(dr->m442 > 2000) {
                    dr->m010[3].z += -7000;
                }
            }
        }
        else {
            if(dr->m3D0[0] == 0) {
                dr->m010[0].z += -10000;
            }

            if(dr->m3D0[1] == 0) {
                dr->m010[1].z += -10000;
            }
        }
    }

    if(dr->m478 == 0.0f && dr->m488 == 1) {
        s16 temp = 1;
        if(dr->m70D == 0) {
            temp = -1;
        }

        dr->m010[6].z -= dr->m440 * temp;
        dr->m010[7].z -= dr->m442 * temp;
        dr->m010[4].z -= dr->m444 * temp;
        dr->m010[5].z -= dr->m446 * temp;
        dr->m010[0xC].z += temp * (s16)(dr->m448 / 2);
        dr->m010[0x13].z += temp * (s16)(dr->m448 / 2);

        if(dr->m418 == 2) {
            dr->m010[0xD].z += temp * (s16)(dr->m448 / 2);
            dr->m010[0x12].z += temp * (s16)(dr->m448 / 2);
        }
    }

    switch(dr->mEnemyType) {
        case DamageState::TYPE_MOBLIN:
        case DamageState::TYPE_BOKOBLIN:
            dr->m010[0].y = -dr->m010[0].z;
            dr->m010[0].z = 0;
            dr->m010[1].y = dr->m010[1].z;
            dr->m010[1].z = 0;

            if(dr->mEnemyType == DamageState::TYPE_BOKOBLIN) {
                dr->m010[0].y = -dr->m010[0].y;
                dr->m010[0].x = -dr->m010[0].x;
            }

            {
                s16 temp = dr->m010[4].z;
                dr->m010[4].z = dr->m010[4].y;
                dr->m010[4].y = -temp;
                s16 temp2 = dr->m010[5].z;
                dr->m010[5].z = -dr->m010[5].y;
                dr->m010[5].y = temp2;
            }

            break;
        case DamageState::TYPE_DARKNUT:
            for(int i = 0; i < 0x14; i++) {
                if(i != 0xD && i != 0x12) {
                    gabi::Local<csXyz> cleared;u32 value=gabi::call<u32>(0x0201A478,cleared.get(),0,0,0);
                    for(u32 k=0;k<6;k+=2)store<u16>(gabi::ea(&dr->m010[i])+k,load<u16>(value+k));
                }
            }

            dr->m010[8].z = dr->m4BC;
            dr->m010[9].z = dr->m4BE;

            if(dr->m47C != 0) {
                if(load<u8>(0x1018F5AC) != 0) {
                    dr->m482 = fopAcM_searchPlayerAngleY(dr->mpEnemy) + 0x8000 + REG0_S(3);
                }

                f32 temp = -(s16)(dr->m482 + 0x8000 - dr->mpEnemy->current.angle.y);
                f32 temp2 = temp;
                if(temp2 > 20000.0f) {
                    temp2 = 20000.0f;
                }
                if(temp2 < -9000.0f) {
                    temp2 = -9000.0f;
                }
                dr->m010[4].x = temp2;
                dr->m010[8].y = -5000;
                dr->m010[8].z = -5000;
                dr->m010[0].y = -10000;
                dr->m010[4].z = -15000;

                f32 temp3 = temp;
                if(temp3 > 9000.0f) {
                    temp3 = 9000.0f;
                }
                if(temp3 < -20000.0f) {
                    temp3 = -20000.0f;
                }
                dr->m010[5].x = temp3;
                dr->m010[9].y = -5000;
                dr->m010[9].z = 5000;
                dr->m010[1].y = -10000;
                dr->m010[5].z = 15000;
                dr->m010[0xA].y = -15000;
                dr->m010[0xB].y = -15000;

                f32 temp4 = temp;
                if(temp4 > 10000.0f) {
                    temp4 = 10000.0f;
                }
                if(temp4 < -10000.0f) {
                    temp4 = -10000.0f;
                }
                dr->m010[0xA].z = temp4;
                dr->m010[0XB].z = temp4;
                dr->m010[0XC].y = 5000;

                if(temp > 8000.0f) {
                    temp = 8000.0f;
                }
                if(temp < -8000.0f) {
                    temp = -8000.0f;
                }
                dr->m010[0xC].z = temp;
            }

            break;
    }

    csxyz_temp.x = 0;
    csxyz_temp.y = 0;
    csxyz_temp.z = 0;
    for(int i = 0; i < 20; i++) {
        if((dr->m474 > 0.1f && (i == 2 || i == 6 || i == 7 || i == 3) && (dr->m70E & 8)) ||
            ((i == 0 || i == 4 || i == 5 || i == 1) && (dr->m70E & 4)) ||
            (i == 0x13 && (dr->m70E & 2)) ||
            (i == 0xD && (dr->m70E & 1)))
        {
            dr->m010[i].y = gabi::fmadds(cM_ssin((u32)dr->mTimer*5000+i*0xED8),(f32)dr->m474,(f32)(s16)dr->m010[i].y);
            dr->m010[i].z = gabi::fmadds(cM_scos((u32)dr->mTimer*4000+i*0xC80),(f32)dr->m474,(f32)(s16)dr->m010[i].z);
        }

        cLib_addCalcAngleS2(&dr->m088[i].x, dr->m010[i].x + csxyz_temp.x, 2, (int)maxSpeed);
        cLib_addCalcAngleS2(&dr->m088[i].y, dr->m010[i].y + csxyz_temp.y, 2, (int)maxSpeed);
        cLib_addCalcAngleS2(&dr->m088[i].z, dr->m010[i].z + csxyz_temp.z, 2, (int)maxSpeed);
    }
}


static void damage_matrix(DamageState* dr){
 MtxTrans(dr->mpEnemy->current.pos.x+dr->m458.x,dr->mpEnemy->current.pos.y+dr->m458.y,dr->mpEnemy->current.pos.z+dr->m458.z,0);
 mDoMtx_XrotM(calc_mtx(),dr->m48C.x);mDoMtx_ZrotM(calc_mtx(),dr->m48C.z);
 MtxTrans(-dr->m44C.x,-dr->m44C.y,-dr->m44C.z,1);
 mDoMtx_YrotM(calc_mtx(),dr->m482);mDoMtx_XrotM(calc_mtx(),dr->m480);mDoMtx_YrotM(calc_mtx(),dr->m484);mDoMtx_YrotM(calc_mtx(),-(s16)dr->m482);
 MtxTrans(dr->m44C.x,dr->m44C.y,dr->m44C.z,1);mDoMtx_YrotM(calc_mtx(),dr->mpEnemy->shape_angle.y);mDoMtx_XrotM(calc_mtx(),dr->mpEnemy->shape_angle.x);mDoMtx_ZrotM(calc_mtx(),dr->mpEnemy->shape_angle.z);
 MtxTrans(load<f32>(0x10006A54),dr->m468,dr->m46C,1);
 gabi::Local<cXyz> v,out;v.get()->set(load<f32>(0x10006A54),load<f32>(0x10006A54),load<f32>(0x10006AD8));MtxPosition(v.get(),out.get());
 dr->m70D=out.get()->y<dr->mpEnemy->current.pos.y?1:0;dr->m424=0;
}
static s32 damage_reaction(u32 address){
 WWHD_FUNC(0x02041F94,s32,address);
 DamageState* dr=gabi::at<DamageState>(address);dr->mTimer++;for(u32 i=0;i<3;i++)if(dr->m4C8[i]!=0)dr->m4C8[i]--;
 s32 result=0;if(dr->mInvincibleTimer!=0){dr->mInvincibleTimer--;dr->mSpawnY=dr->mpEnemy->current.pos.y;}else result=gabi::call<s32>(0x02043F34,address);
 damage_animation(dr);damage_matrix(dr);
 if(dr->m794!=0){dr->m794--;if(dr->m794==0)gabi::call(0x025A5F88,address+0x7A8);}if(dr->m7B2!=0)dr->m7B2--;return result;
}
VERIFY(0x02041F94,damage_reaction);

static s16 damage_hang(DamageState* dr,u32 scratch,u32 auxiliary){
 DamageLine* chk=gabi::at<DamageLine>(scratch);chk->Init();gabi::Local<cXyz> t,tmp;cXyz* v=t.get();cXyz* center=tmp.get();
 gabi::call(0x0201ADE0,&dr->m7A0,v,&dr->mpEnemy->current.pos);mDoMtx_YrotS(calc_mtx(),cM_atan2s(v->x,v->z));
 v->set(0,0,-100);MtxPosition(v,center);gabi::call(0x028E8D88,center,&dr->mpEnemy->current.pos,center);center->y=dr->m7A0.y-5.0f;v->set(10,0,250);
 cXyz* dst[2]={gabi::at<cXyz>(auxiliary),gabi::at<cXyz>(auxiliary+12)};
 for(u32 i=0;i<2;i++){MtxPosition(v,dst[i]);v->x=-(f32)v->x;gabi::call(0x028E8D88,dst[i],center,dst[i]);chk->Set(center,dst[i],dr->mpEnemy);if(cBgS_LineCross(dComIfG_Bgsp(),chk)){dst[i]->set(chk->GetCross().x,chk->GetCross().y,chk->GetCross().z);}else{chk->End();return 0xDCF;}}
 gabi::call(0x0201ADE0,dst[1],v,dst[0]);s16 ret=cM_atan2s(v->x,v->z)+0x4000;chk->End();return ret;
}
static void damage_background(DamageState* dr,u32 scratch){
 f32 lift=dr->m488==1?12.5f:0.0f;
 if(dr->m71E==0){dr->mpEnemy->current.pos.y-=dr->m44C.y+lift;dr->mpEnemy->old.pos.y-=dr->m44C.y+lift;dr->mpEnemy->speed.y*=0.25f;dr->mAcch.CrrPos(dComIfG_Bgsp());dr->mpEnemy->speed.y*=4.0f;dr->mpEnemy->current.pos.y+=dr->m44C.y+lift;dr->mpEnemy->old.pos.y+=dr->m44C.y+lift;dr->m6E8.copy(dr->mpEnemy->old.pos);dr->m6E8.y=dr->mSpawnY;dr->mSpawnY=dr->mAcch.GetGroundH();}else dr->m71E--;
 if(dr->mAction!=21&&dr->mAction!=22){
  DamageGround* chk=gabi::at<DamageGround>(scratch);chk->Init(true);f32 x=dr->mpEnemy->current.pos.x,y=dr->mpEnemy->current.pos.y,z=dr->mpEnemy->current.pos.z;gabi::Local<cXyz> point;point.get()->set(x,y+1000.0f,z);chk->SetPos(point.get());f32 height=cBgS_GroundCross(dComIfG_Bgsp(),chk);
  if(height!=load<f32>(0x10006B78)&&!(dr->mpEnemy->current.pos.y>height)){dr->mpEnemy->current.pos.y=height+REG0_F(13);dr->mMode=0;dr->m47C=0;u32 grp=gabi::call<u32>(0x024EEE30,dComIfG_Bgsp(),gabi::ea(chk)+0x14,0x100);point.get()->set(x,height,z);if(grp){dr->mAction=22;fopKyM_createWpillar(point.get(),REG0_F(9)+1.0f,REG0_F(10)+1.0f,0);}else{dr->mAction=21;gabi::call(0x025DAF3C,point.get(),REG0_F(14)+0.5f);}}
  chk->End();
 }
 if(dr->mEnemyType==2&&gabi::call<s32>(0x0246B6A4,(f32)dr->mpEnemy->current.pos.x,(f32)dr->mpEnemy->current.pos.z)){
  f32 sea=gabi::call<f32>(0x0246BA0C,(f32)dr->mpEnemy->current.pos.x,(f32)dr->mpEnemy->current.pos.z);sea-=40.0f;sea=REG0_F(13)+sea;sea=(f32)dr->m44C.y+sea;
  if(!(dr->mpEnemy->current.pos.y>sea)){dr->mpEnemy->current.pos.y=sea;gabi::Local<cXyz> pos;pos.get()->copy(dr->mpEnemy->current.pos);pos.get()->y=sea;fopKyM_createWpillar(pos.get(),REG0_F(9)+1.0f,REG0_F(10)+1.0f,0);fopAcM_seStart(dr->mpEnemy.get(),0x6918,0);gabi::call(0x025D57E0,dr->mpEnemy.get());u32 actor=gabi::ea(dr->mpEnemy.get());u8 sw=load<u8>(actor+0x3D4);if(sw)gabi::call(0x025B9E38,load<u32>(0x101F84DC)+0x20,sw,load<s8>(actor+0x326));}
 }
}
static s32 damage_joints(DamageState* dr,u32 scratch,u32 auxiliary) {
    int result = 0;

    DamageGround& gndChk=*gabi::at<DamageGround>(auxiliary);gndChk.Init(false);

    if(dr->m420 != 0) {
        dr->m420--;
        if(dr->m420 == 0 && dr->m70D == 1) {
            result = 2;
        }

        for(int i = 0; i < 0xE; i++) {
            dr->m1F0[i].set(dr->m100[i].x,dr->m100[i].y,dr->m100[i].z);
            dr->m1F0[i].y += gabi::fmadds(REG0_F(7),10.0f,200.0f);
            gndChk.SetPos(&dr->m1F0[i]);
            dr->m1F0[i].y = cBgS_GroundCross(dComIfG_Bgsp(),&gndChk);
            if(dr->m1F0[i].y == load<f32>(0x10006B78)) {
                dr->m1F0[i].y = load<f32>(0x10006B7C);
            }

            if(i == 0 || i == 1 || i == 2 || i == 3) {
                if(dr->m1F0[i].y == load<f32>(0x10006B7C)) {
                    dr->m3D0[i] = 2;
                }
                else if(dr->m100[i].y - dr->m1F0[i].y > gabi::fmadds(REG0_F(8),10.0f,200.0f)) {
                    dr->m3D0[i] = 0;
                }
                else {
                    dr->m3D0[i] = 1;
                }
            }
        }

        DamageLine& linChk=*gabi::at<DamageLine>(scratch);linChk.Init();

        gabi::Local<cXyz> t1,t2;cXyz& temp=*t1.get();cXyz& temp2=*t2.get();
        temp.x = 0.0f;
        temp.y = 0.0f;
        f32 diffX = dr->m100[0xE].x - dr->m100[0xA].x;
        f32 diffY = dr->m100[0xE].y - dr->m100[0xA].y;
        f32 diffZ = dr->m100[0xE].z - dr->m100[0xA].z;
        temp.z = std_sqrtf(gabi::fmadds(diffZ,diffZ,gabi::fmadds(diffX,diffX,diffY*diffY)));
        s16 angleY = cM_atan2s(diffX, diffZ);
        s16 angleX = 0;
        for(; angleX >= -0x4000; angleX -= 0x400) {
            mDoMtx_YrotS(calc_mtx(), angleY);
            mDoMtx_XrotM(calc_mtx(), angleX);
            MtxPosition(&temp, &temp2);
            temp2.x = temp2.x + dr->m100[0xA].x;
            temp2.y = temp2.y + dr->m100[0xA].y;
            temp2.z = temp2.z + dr->m100[0xA].z;
            linChk.Set(&dr->m100[0xA], &temp2, dr->mpEnemy);
            if(!cBgS_LineCross(dComIfG_Bgsp(),&linChk)) {
                break;
            }
        }

        if(angleX != 0) {
            dr->m440 = angleX + 0x400;
            dr->m408 = 2;
        }
        else {
            f32 diff2X = dr->m1F0[6].x - dr->m1F0[0xA].x;
            f32 diff2Y = dr->m1F0[6].y - dr->m1F0[0xA].y;
            f32 diff2Z = dr->m1F0[6].z - dr->m1F0[0xA].z;
            dr->m440 = -cM_atan2s(diff2Y, std_sqrtf(gabi::fmadds(diff2X,diff2X,diff2Z*diff2Z)));
            dr->m408 = 0;

            if(dr->m70D == 1) {
                dr->m4B4 = 0;
                dr->m4B0 = 0;
            }
        }

        if(dr->m440 > 7000) {
            dr->m440 = 7000;
        }
        else if(dr->m440 < -10000) {
            dr->m440 = -10000;
        }



        diffX = dr->m100[0xF].x - dr->m100[0xB].x;
        diffY = dr->m100[0xF].y - dr->m100[0xB].y;
        diffZ = dr->m100[0xF].z - dr->m100[0xB].z;
        temp.z = std_sqrtf(gabi::fmadds(diffZ,diffZ,gabi::fmadds(diffX,diffX,diffY*diffY)));
        angleY = cM_atan2s(diffX, diffZ);
        angleX = 0;
        for(; angleX >= -0x4000; angleX -= 0x400) {
            mDoMtx_YrotS(calc_mtx(), angleY);
            mDoMtx_XrotM(calc_mtx(), angleX);
            MtxPosition(&temp, &temp2);
            temp2.x = temp2.x + dr->m100[0xB].x;
            temp2.y = temp2.y + dr->m100[0xB].y;
            temp2.z = temp2.z + dr->m100[0xB].z;
            linChk.Set(&dr->m100[0xB], &temp2, dr->mpEnemy);
            if(!cBgS_LineCross(dComIfG_Bgsp(),&linChk)) {
                break;
            }
        }

        if(angleX != 0) {
            dr->m442 = angleX + 0x400;
            dr->m40C = 2;
        }
        else {
            f32 diff2X = dr->m1F0[7].x - dr->m1F0[0xB].x;
            f32 diff2Y = dr->m1F0[7].y - dr->m1F0[0xB].y;
            f32 diff2Z = dr->m1F0[7].z - dr->m1F0[0xB].z;
            dr->m442 = -cM_atan2s(diff2Y, std_sqrtf(gabi::fmadds(diff2X,diff2X,diff2Z*diff2Z)));
            dr->m40C = 0;

            if(dr->m70D == 1) {
                dr->m4B6= 0;
                dr->m4B2 = 0;
            }
        }



        diffX = dr->m100[0x10].x - dr->m100[0x8].x;
        diffY = dr->m100[0x10].y - dr->m100[0x8].y;
        diffZ = dr->m100[0x10].z - dr->m100[0x8].z;
        temp.z = std_sqrtf(gabi::fmadds(diffZ,diffZ,gabi::fmadds(diffX,diffX,diffY*diffY)));
        angleY = cM_atan2s(diffX, diffZ);
        angleX = 0;
        for(; angleX >= -0x4000; angleX -= 0x400) {
            mDoMtx_YrotS(calc_mtx(), angleY);
            mDoMtx_XrotM(calc_mtx(), angleX);
            MtxPosition(&temp, &temp2);
            temp2.x = temp2.x + dr->m100[0x8].x;
            temp2.y = temp2.y + dr->m100[0x8].y;
            temp2.z = temp2.z + dr->m100[0x8].z;
            linChk.Set(&dr->m100[0x8], &temp2, dr->mpEnemy);
            if(!cBgS_LineCross(dComIfG_Bgsp(),&linChk)) {
                break;
            }
        }

        if(angleX != 0) {
            dr->m444 = angleX + 0x400;
            dr->m410 = 2;
        }
        else {
            f32 diff2X = dr->m1F0[4].x - dr->m1F0[0x8].x;
            f32 diff2Y = dr->m1F0[4].y - dr->m1F0[0x8].y;
            f32 diff2Z = dr->m1F0[4].z - dr->m1F0[0x8].z;
            dr->m444 = -cM_atan2s(diff2Y, std_sqrtf(gabi::fmadds(diff2X,diff2X,diff2Z*diff2Z)));
            dr->m410 = 0;
        }



        diffX = dr->m100[0x11].x - dr->m100[0x9].x;
        diffY = dr->m100[0x11].y - dr->m100[0x9].y;
        diffZ = dr->m100[0x11].z - dr->m100[0x9].z;
        temp.z = std_sqrtf(gabi::fmadds(diffZ,diffZ,gabi::fmadds(diffX,diffX,diffY*diffY)));
        angleY = cM_atan2s(diffX, diffZ);
        angleX = 0;
        for(; angleX >= -0x4000; angleX -= 0x400) {
            mDoMtx_YrotS(calc_mtx(), angleY);
            mDoMtx_XrotM(calc_mtx(), angleX);
            MtxPosition(&temp, &temp2);
            temp2.x = temp2.x + dr->m100[0x9].x;
            temp2.y = temp2.y + dr->m100[0x9].y;
            temp2.z = temp2.z + dr->m100[0x9].z;
            linChk.Set(&dr->m100[0x9], &temp2, dr->mpEnemy);
            if(!cBgS_LineCross(dComIfG_Bgsp(),&linChk)) {
                break;
            }
        }

        if(angleX != 0) {
            dr->m446 = angleX + 0x400;
            dr->m414 = 2;
        }
        else {
            f32 diff2X = dr->m1F0[5].x - dr->m1F0[0x9].x;
            f32 diff2Y = dr->m1F0[5].y - dr->m1F0[0x9].y;
            f32 diff2Z = dr->m1F0[5].z - dr->m1F0[0x9].z;
            dr->m446 = -cM_atan2s(diff2Y, std_sqrtf(gabi::fmadds(diff2X,diff2X,diff2Z*diff2Z)));
            dr->m414 = 0;
        }



        diffX = dr->m100[0x12].x - dr->m100[0xC].x;
        diffY = dr->m100[0x12].y - dr->m100[0xC].y;
        diffZ = dr->m100[0x12].z - dr->m100[0xC].z;
        temp.z = std_sqrtf(gabi::fmadds(diffZ,diffZ,gabi::fmadds(diffX,diffX,diffY*diffY)));
        angleY = cM_atan2s(diffX, diffZ);

        if(REG0_S(4) == 0) {
            angleX = 0;
            for(; angleX >= -0x4000; angleX -= 0x400) {
                mDoMtx_YrotS(calc_mtx(), angleY);
                mDoMtx_XrotM(calc_mtx(), angleX);
                MtxPosition(&temp, &temp2);
                temp2.x = temp2.x + dr->m100[0xC].x;
                temp2.y = temp2.y + dr->m100[0xC].y;
                temp2.z = temp2.z + dr->m100[0xC].z;
                linChk.Set(&dr->m100[0xC], &temp2, dr->mpEnemy);
                if(!cBgS_LineCross(dComIfG_Bgsp(),&linChk)) {
                    break;
                }
            }

            if(angleX != 0) {
                dr->m448 = angleX + 0x400;
                dr->m418 = 2;
            }
            else {
                f32 diff2X = dr->m1F0[0xD].x - dr->m1F0[0xC].x;
                f32 diff2Y = dr->m1F0[0xD].y - dr->m1F0[0xC].y;
                f32 diff2Z = dr->m1F0[0xD].z - dr->m1F0[0xC].z;
                dr->m448 = -cM_atan2s(diff2Y, std_sqrtf(gabi::fmadds(diff2X,diff2X,diff2Z*diff2Z)));
                dr->m418 = 0;
            }

            if(dr->m448 > 13000) {
                dr->m448 = 13000;
            }
            else if(dr->m448 < -13000) {
                dr->m448 = -13000;
            }
        }
        linChk.End();
    }
    gndChk.End();
    return result;
}


static void damage_sound_inline(fopAc_ac_c* actor,u32 id){u32 a=gabi::ea(actor);if(a+0x37C!=0){s8 room=load<s8>(a+0x326);s8 reverb=gabi::call<s8>(0x02520540,room);gabi::call(0x025E1A40,id,a+0x37C,0,reverb);}}
static s32 damage_set(u32 address) {
    WWHD_FUNC(0x02043F34,s32,address);DamageState* dr=gabi::at<DamageState>(address);gabi::call(0x025200D4);gabi::Local<DamageLine> checkScratch;gabi::Local<DamageGround> groundScratch;u32 auxiliary=gabi::ea(groundScratch.get());u32 scratch=gabi::ea(checkScratch.get());
    int react = 0;
    bool temp = false;
    gabi::Local<cXyz> t2,t4,t3;cXyz& temp2=*t2.get();cXyz& temp4=*t4.get();cXyz& temp3=*t3.get();

    if(dr->mEnemyType == DamageState::TYPE_BOKOBLIN && dr->m488 == 0) {
        if(dr->mAction < 0x13 && !(load<u32>(gabi::ea(dr->mpEnemy.get())+0x2E0)&0x100000) && dr->mpEnemy->current.pos.y - dr->mSpawnY > dr->mMaxFallDistance) {
            if(dr->m7B2 != 0 && dr->mMode > -100) {
                dr->mAction = 0x13;
                dr->mMode = 0;
                dr->m71E = 2;
            }
            else {
                if(dr->m6E8.y - dr->mSpawnY > 300.0f) {
                    dr->m7A0.copy(dr->m6E8);
                    dr->m7AC.y = damage_hang(dr,scratch,auxiliary);
                    if(dr->m7AC.y == 0xDCF) {
                        temp = true;
                        dr->mpEnemy->health = 0;
                    }
                    else {
                        dr->mAction = 0x13;
                        dr->mMode = 0;
                        dr->m71E = 2;
                        dr->m7B8 = 0xFFFFFFFF;
                    }
                }
                else {
                    temp = true;
                    dr->mpEnemy->health = 0;
                }
            }
        }
    }
    else if(dr->mpEnemy->current.pos.y - dr->mSpawnY > dr->mMaxFallDistance) {
        temp = true;
        if(!(dr->mpEnemy->speed.y>load<f32>(0x10006B40)))dr->mpEnemy->health = 0;
    }

    switch(dr->m488) {
        case 0:
            if(temp || (dr->m424 != 0 && dr->m428 > 25.0f)) {
                dr->mMode = -100;
                dr->m488 = 1;
                dr->m713 = 0;
                dr->m44C.y = -125.0f;
                dr->m486 = 0x4000;
                temp2.copy(dr->m42C);

                if(temp) {
                    dr->m482 = dr->mpEnemy->shape_angle.y;
                    dr->m478 = 0.0f;
                    dr->mpEnemy->speed.y = 0.0f;
                    dr->m71E = 10;
                }
                else if(dr->m424 & 0x40) {
                    if(dr->m712) {
                        dr->m478 = -7.0f;
                        dr->mpEnemy->speed.y = 72.0f;
                    }
                    else {
                        dr->m478 = -20.0f;
                        dr->mpEnemy->speed.y = 96.0f;
                    }

                    dr->m482 = cM_atan2s(temp2.x, temp2.z) + 0x8000;
                }
                else {
                    dr->m482 = cM_atan2s(temp2.x, temp2.z);
                    dr->m478 = dr->m428;

                    if(((dr->mEnemyType == DamageState::TYPE_BOKOBLIN) && (fopAcM_GetParam(dr->mpEnemy) & 0xF) == 0xA) || REG0_S(5) != 0) {
                        dr->mpEnemy->speed.y = dr->m478 * 0.8f * (REG0_F(7) + 0.8f);
                    }
                    else {
                        dr->mpEnemy->speed.y = dr->m478 * 0.9f * 1.2f;
                    }
                }

                temp2.x = 0.0f;
                temp2.y = 0.0f;
                temp2.z = dr->m478;
                mDoMtx_YrotS(calc_mtx(), dr->m482);
                MtxPosition(&temp2, &temp4);
                store<u32>(gabi::ea(dr->mpEnemy.get())+0x33C,load<u32>(gabi::ea(&temp4)));
                store<u32>(gabi::ea(dr->mpEnemy.get())+0x344,load<u32>(gabi::ea(&temp4)+8));
                dr->m47C = 1;
                dr->m4AE = cM_rndF(20000.0f);
                dr->m4AC = cM_rndF(20000.0f);
                dr->m4AA = cM_rndF(-20000.0f);
                dr->m4A8 = cM_rndF(-20000.0f);
                dr->m70C = 0;
                dr->m474 = 2000.0f;
                dr->m70E = 0xFF;

                if(dr->mEnemyType == DamageState::TYPE_DARKNUT) {
                    dr->m4BC = -cM_rndF(9000.0f);
                    dr->m4BE = cM_rndF(9000.0f);
                }
                else {
                    dr->m4B8 = cM_rndF(6000.0f) + 1000.0f;
                    dr->m4BA = cM_rndF(6000.0f) + 1000.0f;
                    dr->m4BC = cM_rndF(10000.0f) + 5000.0f;
                    dr->m4BE = cM_rndF(10000.0f) + 5000.0f;
                }

                react = temp ? 0x1E : 1;

                dr->m48A = 0;
            }
            else if(dr->m424 & 0x10) {
                dr->m470 = REG0_F(0xD) + 4000.0f;
                dr->m474 = REG0_F(0xE) + 4000.0f;
                dr->m70E = 7;
                react = 5;
            }

            break;
        case 1:
            {
                s16 temp5;
                if(dr->m486 > 0) {
                   temp5 = 0x350;
                }
                else {
                   temp5 = 0x800;
                }
                if(load<u8>(0x1018F5AC) != 0) {
                    dr->m486 = 0;
                }
                cLib_addCalcAngleS2(&dr->m480, dr->m486, 3, temp5);

                if(dr->m70C != 0 && !(dr->m474 > 0.01f) && dr->m4C8[2] == 0 && load<u8>(0x1018F5AC) == 0) {
                    if(dr->mpEnemy->health <= 0) {
                        dr->mAction = 0x14;
                        dr->mMode = 0;
                        dr->m4C8[2] = 10000;
                    }
                    else {
                        dr->m4BE = 0;
                        dr->m4BC = 0;
                        dr->m4BA = 0;
                        dr->m4B8 = 0;
                        dr->m49C = 0;
                        dr->m49A = 0;
                        dr->m4B2 = 0;
                        dr->m4B6 = 0;
                        dr->m4B0 = 0;
                        dr->m4B4 = 0;

                        if(dr->m70D == 0) {
                            dr->mMode = 10;
                        }
                        else {
                            dr->mMode = 0xC;
                        }

                        dr->mAction = 0xB;
                        dr->m48A = 0;
                        dr->m488 = 0;
                        dr->mpEnemy->current.angle.y += dr->m484;
                        dr->mpEnemy->shape_angle.y += dr->m484;
                        dr->m4C0 += dr->m484;
                        dr->m7B6 = 1;
                        dr->m484 = 0;
                        dr->m6E0 = 0;
                    }
                }
            }

            break;
        case 2:
            break;
        default:
            break;
    }

    if(dr->m7B6 == 0 && load<u8>(0x1018F5AC) == 0) {
        if(dr->m4C8[0] == 0) {
            if(dr->mAction==18)dr->m4D4=0;
            mDoMtx_YrotS(calc_mtx(), cM_atan2s(-dr->m42C.x, -dr->m42C.z));
            temp2.x = 0.0f;
            temp2.y = 0.0f;
            temp2.z = dr->m4D4;
            MtxPosition(&temp2, &temp3);
            dr->mpEnemy->current.pos.x += gabi::fmadds((f32)dr->mpEnemy->speed.x,0.25f,(f32)temp3.x);
            dr->mpEnemy->current.pos.y = gabi::fmadds((f32)dr->mpEnemy->speed.y,0.25f,(f32)dr->mpEnemy->current.pos.y);
            dr->mpEnemy->current.pos.z += gabi::fmadds((f32)dr->mpEnemy->speed.z,0.25f,(f32)temp3.z);

            if(dr->mAction != 0xF && dr->mMode > -100) {
                DamageStts* pStts = &dr->mStts;
                if(pStts != NULL) {
                    // Match the reference's lfs promotion, offset-before-position loads and
                    // double add followed by single rounding; preserve its NaN transport.
                    f64 moveX=(f32)pStts->GetCCMoveP()->x;
                    f64 positionX=(f32)dr->mpEnemy->current.pos.x;
                    dr->mpEnemy->current.pos.x = (f32)(positionX+moveX);
                    f64 positionZ=(f32)dr->mpEnemy->current.pos.z;
                    f64 moveZ=(f32)pStts->GetCCMoveP()->z;
                    dr->mpEnemy->current.pos.z = (f32)(positionZ+moveZ);
                }
            }

            if(dr->mInvincibleTimer == 0) {
                dr->mpEnemy->speed.y -= 12.0f;
                if(dr->mpEnemy->speed.y < -200.0f) {
                    dr->mpEnemy->speed.y = -200.0f;
                }
            }

            cLib_addCalc0(&dr->m4D4, 1.0f, 10.0f);
        }
        else if(dr->m4C8[0] == 1 && dr->m486 == 0) {
            dr->m486 = -0x4000;
            dr->m4AE = cM_rndF(20000.0f);
            dr->m4AC = cM_rndF(20000.0f);
            dr->m4AA = cM_rndF(-20000.0f);
            dr->m4A8 = cM_rndF(-20000.0f);
        }

        damage_background(dr,scratch);
        if(damage_joints(dr,scratch,auxiliary) == 2) {
            react = 2;
        }
    }
    else {
        dr->m7B6 = 0;
    }

    if(dr->m478 != 0.0f && react == 0 && dr->mAcch.ChkWallHit()) {
        react = 0x15;
        dr->m47C = 0;
        dr->mpEnemy->speed.y = 0.0f;
        dr->mpEnemy->speed.x *= -0.4f;
        dr->mpEnemy->speed.z *= -0.4f;

        if(dr->m70C == 0 && std::fabs(dr->m478) > 30.0f) {
            dr->m474 = 6000.0f;
            dr->m70E = 0xFF;

            if(dr->mpEnemy->current.pos.y - dr->mSpawnY > REG_F(14,6) + 50.0f) {
                dr->m4C8[0] = 10;
                dr->m486 = 0;
            }
            else {
                dr->mpEnemy->speed.z = 0.0f;
                dr->mpEnemy->speed.x = 0.0f;
                dr->m486 = 0x4000;
            }

            int kado_check_temp = gabi::call<s32>(0x02041D10,address);
            if(kado_check_temp != 0) {
                dr->m4C8[0] = 0;
                dr->m486 = 0x4000;

                int temp;
                if(dr->mpEnemy->current.pos.y - dr->mSpawnY > REG0_F(6) + 90.0f) {
                    temp = 0x10000;
                }
                else {
                    temp = 0x8000;
                }

                if(kado_check_temp == 2) {
                    dr->m6E0 += temp;
                    temp2.x = dr->m478 * 0.5f;
                }
                else {
                    dr->m6E0 -= temp;
                    temp2.x = dr->m478 * -0.5f;
                }
                temp2.y = 0.0f;
                temp2.z = dr->m478 * 0.5f;
                mDoMtx_YrotS(calc_mtx(), dr->m482);
                MtxPosition(&temp2, &temp4);
                dr->mpEnemy->speed.x = temp4.x;
                dr->mpEnemy->speed.z = temp4.z;
            }
        }

        dr->m478 = 0.0f;
        dr->m4A8 = 0;
        dr->m4AA = 0;
        dr->m4AC = 0;
        dr->m4AE = 0;
        dr->mParticlePos.copy(dr->m100[0xC]);
        dr->mParticleAngle.z = 0;
        s16 wallYaw=cM_atan2s(-dr->m42C.x,-dr->m42C.z); fopAc_ac_c* wallActor=dr->mpEnemy.get();
        dr->mParticleAngle.y = gabi::call<s16>(0x02043C4C,wallActor,wallYaw);
        dr->mParticleAngle.x = 0x4000;
        dr->mParticleCallBack.end();
        s8 room=fopAcM_GetRoomNo(dr->mpEnemy);u32 play=gabi::call<u32>(0x025200D4);
        u32 emitter=gabi::call<u32>(0x025A847C,load<u32>(play+0x5AB0),2,0x2022,&dr->mParticlePos,&dr->mParticleAngle,0,0xB9,&dr->mParticleCallBack,room,0,0,0);
        if(emitter){f32 scale=dr->mEnemyType==2?1.0f:1.2f;store<f32>(emitter+0x34,3.0f);store<f32>(emitter+0x58,1.0f);gabi::Local<cXyz> vec;gabi::call(0x02043C04,vec.get());vec.get()->set(scale,scale,scale);gabi::call(0x02043C30,emitter+0x220,vec.get());gabi::call(0x02043C30,emitter+0x238,vec.get());vec.get()->set(scale+scale,scale+scale,scale+scale);gabi::call(0x02043C30,emitter+0x238,vec.get());dr->m794=6;}

    }

    if(dr->mAcch.ChkGroundHit()) {
        if(dr->m488 != 0 && react == 0 && (dr->m70C == 0 || !(dr->mpEnemy->speed.y > -100.0f))) {
            react = 0x14;
            dr->mpEnemy->speed.x *= 0.75f;
            dr->mpEnemy->speed.z *= 0.75f;
            dr->m70E = 0xFF;

            if(dr->m712 != 0) {
                dr->mpEnemy->speed.y = 60.0f;
                dr->m474 = gabi::fmadds(REG0_F(0xC),100.0f,9000.0f);
            }
            else {
                dr->mpEnemy->speed.y = 60.0f;
                dr->m474 = 8000.0f;
            }

            dr->m70C += 1;
            dr->m47C = 0;
            dr->m420 = REG0_S(3) + 1;

            s16 temp5 = dr->m482;
            s16 temp6 = dr->mpEnemy->current.angle.y - temp5;
            if(temp6 < 0) {
                temp6 = -temp6;
            }

            if(temp6 < 0x4000) {
                dr->m4C0 = temp5;
            }
            else {
                dr->m4C0 = temp5 + 0x8000;
            }

            dr->m49A = cM_rndFX(18000.0f);
            dr->m49C = cM_rndFX(10000.0f);
            dr->m4B0 = cM_rndF(20000.0f);
            dr->m4B4 = dr->m4B0 / 2;
            dr->m4B2 = cM_rndF(20000.0f);
            dr->m4B6 = dr->m4B2 / 2;

            if(dr->mpEnemy->health <= 0) {
                damage_sound_inline(dr->mpEnemy.get(),0x580C);
            }
            else if(dr->mEnemyType == DamageState::TYPE_DARKNUT) {
                if(load<u8>(gabi::ea(dr->mpEnemy.get())+0x3EC) & 1) {
                    damage_sound_inline(dr->mpEnemy.get(),0x58EE);
                }
                else {
                    damage_sound_inline(dr->mpEnemy.get(),0x58EC);
                }
            }
            else {
                gabi::call(0x02043B98,dr->mpEnemy.get(),0x580B,0);
            }

            int kado_check_temp = gabi::call<s32>(0x02041D10,address);
            if(kado_check_temp != 0) {
                if(kado_check_temp == 2) {
                    dr->m6E0 += 0x8000;
                    temp2.x = dr->m478 * 0.3f;
                }
                else {
                    dr->m6E0 += -0x8000;
                    temp2.x = dr->m478 * -0.3f;
                }
                temp2.y = 0.0f;
                temp2.z = dr->m478 * 0.75f;
                mDoMtx_YrotS(calc_mtx(), dr->m482);
                MtxPosition(&temp2, &temp4);
                dr->mpEnemy->speed.x = temp4.x;
                dr->mpEnemy->speed.z = temp4.z;
            }
        }
        else {
            if(dr->m478 != 0) {
                dr->m420 = 1;
            }

            dr->m478 = 0.0f;

            if(dr->m6E0 == 0) {
                dr->mpEnemy->speed.z = 0.0f;
                dr->mpEnemy->speed.x = 0.0f;
            }
            else {
                cLib_addCalc0(&dr->mpEnemy->speed.x, 1.0f, 4.0f);
                cLib_addCalc0(&dr->mpEnemy->speed.z, 1.0f, 4.0f);
            }
            dr->mpEnemy->speed.y = -1.0f;

            dr->m4A8 = 0;
            dr->m4AA = 0;
            dr->m4AC = 0;
            dr->m4AE = 0;

            if(dr->m70D != 0) {
                dr->m4A8 = 0;
                dr->m4AA = 0;
                dr->m4AC = 0;
                dr->m4AE = 0;
                dr->m4A0 = 0;
                dr->m4A2 = 0;
                dr->m4A4 = 0;
                dr->m4A6 = 0;
            }
        }
    }

    if(dr->m6E0 != 0) {
        if(dr->m6E0 > 0) {
            dr->m6E0 += -0x1000;
            dr->m484 += -0x1000;
        }
        else {
            dr->m6E0 += 0x1000;
            dr->m484 += 0x1000;
        }
    }

    if(dr->m70C != 0) {
        if(dr->m488 != 0) {
            cLib_addCalcAngleS2(&dr->mpEnemy->current.angle.y, dr->m4C0, 1, 0x500);
        }

        cLib_addCalc0(&dr->m474, 1.0f, gabi::fmadds(REG0_F(5),10.0f,300.0f));
    }

    return react;
}


VERIFY(0x02043F34,damage_set);
}
