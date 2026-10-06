#include "gabi.h"
using gabi::load; using gabi::store;
// HD dSalvage_control_c: two counters, then 160 records of 0x38 bytes.
static u32 info(void* self,s32 no) { return gabi::ea(self)+4+u32(no)*56; }
void salvage_entry(void* self,void* actor,void* emitter) {
 WWHD_FUNC(0x025B44A0,void,self,actor,emitter);
 u32 a=gabi::ea(actor),c=gabi::ea(self);
 if(!a) gabi::call<void>(0x0273AA24,gabi::at<void>(0x100535F0),139,gabi::at<void>(0x100535E4));
 s32 room=load<s8>(a+0x326);u32 params=load<u32>(a+0xB0);
 u32 sw=load<u8>(a+0x2FD),kind=params>>28,save=(params>>20)&255;
 s32 map=(s8)(params>>12);u32 item=(params>>4)&255,type=params&15;
 u32 no;bool invalid=false;
 if(room) { u32 n=load<u8>(c+1);map=room;no=n+128;invalid=n>32; }
 else {no=load<u8>(c);invalid=no>128;}
 if(u32(map-1)>=64) invalid=true;
 if(kind==2 || kind==3) {if(save>15) invalid=true;}
 else if(kind==0 && u32(save-1)>=128) invalid=true;
 // Kind 2 requires an assigned switch; all invalid parameter sets return before writes.
 if(kind==2 && sw==255) return;
 if(invalid) return;
 u32 i=info(self,(s32)no);store<u8>(i+0x31,0);
 auto state=[](){return load<u32>(0x101F84DC);};
 auto flag=[&](){store<u8>(i+0x31,load<u8>(i+0x31)|1);};
 switch(kind) {
 case 0:
  if(gabi::call<s32>(0x025B8228,gabi::at<void>(state()+0xE4),save-1)) return;
  if((u32(load<s16>(a+0x2FC))&3)!=load<u8>(state()+0x1C1)) return;
  flag();break;
 case 2:
  if(!gabi::call<s32>(0x025BA0C0,gabi::at<void>(state()+0x20),sw,(s32)load<s8>(a+0x2FE))) flag();
  [[fallthrough]];
 case 3:
  if(save!=31 && gabi::call<s32>(0x025B8A50,gabi::at<void>(state()+0x5E0),u32(map)&255,save)) return;
  break;
 case 4:
  if(save!=31 && gabi::call<s32>(0x025B8A50,gabi::at<void>(state()+0x5E0),u32(map)&255,save)) return;
  if(!gabi::call<s32>(0x02556D14)) flag();break;
 case 6:
  if(gabi::call<s32>(0x02560828) || gabi::call<s32>(0x025B8B94,gabi::at<void>(state()+0x644),(u32)load<u16>(0x1004BD1C+save*2))) flag();
  break;
 }
 store<u8>(i+0x2C,room);
 for(u32 k=0;k<12;k+=4) store<u32>(i+k,load<u32>(a+0x314+k));
 for(u32 k=0;k<12;k+=4) store<u32>(i+12+k,load<u32>(a+0x330+k));
 store<u8>(i+0x30,item);store<u8>(i+0x2E,type);store<u32>(i+0x24,gabi::ea(emitter));
 store<u8>(i+0x2F,kind);store<u8>(i+0x2D,save);store<u32>(i+0x20,sw);
 f32 h=load<f32>(0x100535D8),r=load<f32>(0x100535D4);
 if(kind==0) r=load<f32>(0x100535DC);
 else if(kind!=5) {h=r;r=load<f32>(0x100535E0);}
 store<f32>(i+0x1C,gabi::fmuls_ppc(h,load<f32>(a+0x334)));
 store<f32>(i+0x18,gabi::fmuls_ppc(r,load<f32>(a+0x330)));
 u32 counter=c+(room!=0);store<u8>(counter,load<u8>(counter)+1);
}
VERIFY(0x025B44A0,salvage_entry);
s32 salvage_checkRegist(void* self,s32 no) { WWHD_FUNC(0x025B48BC,s32,self,no);return load<s8>(info(self,no)+0x2C)!=-1; }
VERIFY(0x025B48BC,salvage_checkRegist);
struct SalvageVector {u8 bytes[12];};
void salvage_calcDistanceXZ(void* self) {
 WWHD_FUNC(0x025B48DC,void,self);
 u32 play=gabi::ea(gabi::call<void*>(0x025200D4));f32 zero=load<f32>(0x10053600);
 u32 player=load<u32>(play+0x5B2C);
 gabi::Local<SalvageVector> difference,horizontal;
 for(s32 n=0;n<160;++n) if(salvage_checkRegist(self,n)) {
  u32 i=info(self,n);
  gabi::call<void>(0x0201ADE0,gabi::at<void>(player+0x314),difference.get(),gabi::at<void>(i));
  store<f32>(gabi::ea(horizontal.get()),load<f32>(gabi::ea(difference.get())));
  store<f32>(gabi::ea(horizontal.get())+4,zero);
  store<f32>(gabi::ea(horizontal.get())+8,load<f32>(gabi::ea(difference.get())+8));
  gabi::call<void>(0x028E8DD0,horizontal.get());
  f32 dist=gabi::call<f32>(0x028F4384);
  store<f32>(i+0x28,dist);
 }
}
VERIFY(0x025B48DC,salvage_calcDistanceXZ);
void salvage_setPos(void* self,s32 no,void* pos) { WWHD_FUNC(0x025B4998,void,self,no,pos);u32 i=info(self,no),p=gabi::ea(pos);for(u32 k=0;k<12;k+=4)store<u32>(i+k,load<u32>(p+k)); }
VERIFY(0x025B4998,salvage_setPos);
void salvage_setNowAlpha(void* self,s32 no,u32 value) {WWHD_FUNC(0x025B49BC,void,self,no,value);store<u8>(info(self,no)+0x33,value);}
VERIFY(0x025B49BC,salvage_setNowAlpha);
void salvage_setDrawMode(void* self,s32 no,s32 value) {WWHD_FUNC(0x025B49CC,void,self,no,value);store<u8>(info(self,no)+0x34,value);}
VERIFY(0x025B49CC,salvage_setDrawMode);
void salvage_init(void* self) {WWHD_FUNC(0x025B49DC,void,self);for(s32 n=0;n<160;++n){u32 i=info(self,n);store<u8>(i+0x2C,255);store<u32>(i+0x24,0);}store<u8>(gabi::ea(self),0);store<u8>(gabi::ea(self)+1,0);}
VERIFY(0x025B49DC,salvage_init);
void salvage_init_one_sub(void* self,s32 no) {
 WWHD_FUNC(0x025B4A10,void,self,no);u32 i=info(self,no),e=load<u32>(i+0x24);
 if(e){u32 flags=load<u32>(e+0x254);store<u32>(e+0x5C,0xFFFFFFFF);store<u32>(e+0x254,flags|1);}
 store<u8>(i+0x2C,255);
 for(u32 k=0;k<12;k+=4)store<u32>(i+k,load<u32>(0x101FFBA8+k));
 for(u32 k=0;k<12;k+=4)store<u32>(i+12+k,load<u32>(0x101FFBA8+k));
 store<u8>(i+0x2D,0);store<u8>(i+0x30,0);store<u8>(i+0x31,0);
 store<u32>(i+0x20,0);store<u8>(i+0x2E,0);store<u32>(i+0x24,0);
}
VERIFY(0x025B4A10,salvage_init_one_sub);
void salvage_init_end(void* self) {WWHD_FUNC(0x025B4A98,void,self);for(s32 n=0;n<160;++n)salvage_init_one_sub(self,n);store<u8>(gabi::ea(self),0);store<u8>(gabi::ea(self)+1,0);}
VERIFY(0x025B4A98,salvage_init_end);
void salvage_init_room(void* self,s32 room) {WWHD_FUNC(0x025B4AD8,void,self,room);for(s32 n=128;n<160;++n)salvage_init_one_sub(self,n);store<u8>(gabi::ea(self)+1,0);}
VERIFY(0x025B4AD8,salvage_init_room);
void salvage_init_one(void* self,s32 no) {WWHD_FUNC(0x025B4B14,void,self,no);salvage_init_one_sub(self,no);u32 c=gabi::ea(self)+(no>=128);store<u8>(c,load<u8>(c)-1);}
VERIFY(0x025B4B14,salvage_init_one);
void salvage_setFlag(void* self,s32 no,u32 flag) {WWHD_FUNC(0x025B4B4C,void,self,no,flag);u32 i=info(self,no);store<u8>(i+0x31,load<u8>(i+0x31)|flag);}
VERIFY(0x025B4B4C,salvage_setFlag);
void salvage_clrFlag(void* self,s32 no,u32 flag) {WWHD_FUNC(0x025B4B64,void,self,no,flag);u32 i=info(self,no);store<u8>(i+0x31,load<u8>(i+0x31)&~flag);}
VERIFY(0x025B4B64,salvage_clrFlag);
void* salvage_getPos(void* self,void* out,s32 no) {WWHD_FUNC(0x025B4B7C,void*,self,out,no);u32 i=info(self,no);if(!out)out=gabi::call<void*>(0x0273AD10,12);if(out)for(u32 k=0;k<12;k+=4)store<f32>(gabi::ea(out)+k,load<f32>(i+k));return out;}
VERIFY(0x025B4B7C,salvage_getPos);
void* salvage_getPosP(void* self,s32 no) {WWHD_FUNC(0x025B4BE0,void*,self,no);return gabi::at<void>(info(self,no));}
VERIFY(0x025B4BE0,salvage_getPosP);
void salvage_getScale(void* self,void* out,s32 no) {WWHD_FUNC(0x025B4BF0,void,self,out,no);u32 i=info(self,no)+12;if(!out)out=gabi::call<void*>(0x0273AD10,12);if(out)for(u32 k=0;k<12;k+=4)store<f32>(gabi::ea(out)+k,load<f32>(i+k));}
VERIFY(0x025B4BF0,salvage_getScale);
f32 salvage_getR(void* self,s32 no) {WWHD_FUNC(0x025B4C54,f32,self,no);return load<f32>(info(self,no)+0x18);}
VERIFY(0x025B4C54,salvage_getR);
f32 salvage_getH(void* self,s32 no) {WWHD_FUNC(0x025B4C64,f32,self,no);return load<f32>(info(self,no)+0x1C);}
VERIFY(0x025B4C64,salvage_getH);
s8 salvage_getRoomNo(void* self,s32 no) {WWHD_FUNC(0x025B4C74,s8,self,no);return load<s8>(info(self,no)+0x2C);}
VERIFY(0x025B4C74,salvage_getRoomNo);
u8 salvage_getItemNo(void* self,s32 no) {WWHD_FUNC(0x025B4C88,u8,self,no);return load<u8>(info(self,no)+0x30);}
VERIFY(0x025B4C88,salvage_getItemNo);
s32 salvage_getSwitchNo(void* self,s32 no) {WWHD_FUNC(0x025B4C98,s32,self,no);return load<s32>(info(self,no)+0x20);}
VERIFY(0x025B4C98,salvage_getSwitchNo);
u8 salvage_getSaveNo(void* self,s32 no) {WWHD_FUNC(0x025B4CA8,u8,self,no);return load<u8>(info(self,no)+0x2D);}
VERIFY(0x025B4CA8,salvage_getSaveNo);
u8 salvage_getType(void* self,s32 no) {WWHD_FUNC(0x025B4CB8,u8,self,no);return load<u8>(info(self,no)+0x2E);}
VERIFY(0x025B4CB8,salvage_getType);
u8 salvage_getKind(void* self,s32 no) {WWHD_FUNC(0x025B4CC8,u8,self,no);return load<u8>(info(self,no)+0x2F);}
VERIFY(0x025B4CC8,salvage_getKind);
s32 salvage_getDrawMode(void* self,s32 no) {WWHD_FUNC(0x025B4CEC,s32,self,no);return load<u8>(info(self,no)+0x33);}
VERIFY(0x025B4CEC,salvage_getDrawMode);
f32 salvage_getDistance(void* self,s32 no) {WWHD_FUNC(0x025B4CFC,f32,self,no);return load<f32>(info(self,no)+0x28);}
VERIFY(0x025B4CFC,salvage_getDistance);
void* salvage_getAlphaPtr(void* self,s32 no) {WWHD_FUNC(0x025B4CD8,void*,self,no);return gabi::at<void>(info(self,no)+0x33);}
VERIFY(0x025B4CD8,salvage_getAlphaPtr);
s32 salvage_checkUsed(void* self,s32 no) {WWHD_FUNC(0x025B4D0C,s32,self,no);return !(load<u8>(info(self,no)+0x31)&1);}
VERIFY(0x025B4D0C,salvage_checkUsed);
// Bounded trailing header-static initializer; exact generating header attribution is qualified.
void salvage_StaticInit() {
 WWHD_FUNC(0x025B4D2C,void);
 store<u32>(0x1047C8F8,0);store<u32>(0x1047C8F0,0);store<u32>(0x1047C8FC,0);store<u32>(0x1047C8F4,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101EAD7C));
 f32 first=load<f32>(0x1005360C),second=load<f32>(0x10053610);
 store<f32>(0x1047C8E4,first);store<f32>(0x1047C8E8,second);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x1047C8EC));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101EAD88));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x1047C8ED));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101EAD94));
}
VERIFY(0x025B4D2C,salvage_StaticInit);
