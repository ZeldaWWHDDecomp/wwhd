#include "gabi.h"
using namespace gabi;
void ext_sortClear(void* self){WWHD_FUNC(0x025EDBC0,void,self);u32 o=ea(self),p=load<u32>(o+0x98);if(p){call(0x027F0ECC,at<void>(load<u32>(0x104B4634)),self,0);store<u32>(o+0x98,0);do{u32 next=load<u32>(p);store<u32>(p,0);p=next;}while(p);}}
VERIFY(0x025EDBC0,ext_sortClear);
void* ext_sortCtor(void* self){WWHD_FUNC(0x025EDC2C,void*,self);u32 o=ea(self);if(!o){o=call<u32>(0x0273AD10,0x9C);if(!o)return nullptr;}call(0x027F1278,at<void>(o));store<u32>(o+0xC,0x10058E20);call(0x025EDBC0,at<void>(o));return at<void>(o);}
VERIFY(0x025EDC2C,ext_sortCtor);
void ext_sortDraw(void* self,void* arguments){WWHD_FUNC(0x025EDC88,void,self,arguments);u32 o=ea(self),p=load<u32>(o+0x98);call_ptr(load<u32>(load<u32>(p+0x130)+0x1C),at<void>(p),arguments);p=load<u32>(o+0x98);do{call_ptr(load<u32>(load<u32>(p+0x130)+0x2C),at<void>(p),arguments);p=load<u32>(p);}while(p);call(0x02750370,at<void>(0x104B474C));}
VERIFY(0x025EDC88,ext_sortDraw);
void ext_sortSet(void* self,void* material){WWHD_FUNC(0x025EDD04,void,self,material);u32 o=ea(self),p=load<u32>(o+0x98);if(!p){call(0x027F0E04,at<void>(load<u32>(0x104B4634)),self,0);p=load<u32>(o+0x98);}store<u32>(ea(material),p);store<u32>(o+0x98,ea(material));}
VERIFY(0x025EDD04,ext_sortSet);
u32 ext_weightInit(void* self,void* input){WWHD_FUNC(0x025EDD64,u32,self,input);u32 o=ea(self),n=load<u32>(ea(input)),rounded=(n*12+31)&~31u;u32 q=(u32)(((u64)0xAAAAAAAB*rounded)>>32)>>3;u32 p=call<u32>(0x0273AE48,q*12,0x20);store<u32>(o+4,p);if(!p)return 0;store<u32>(o,ea(input));return 1;}
VERIFY(0x025EDD64,ext_weightInit);
void ext_weightCalc(void* self,void* model){WWHD_FUNC(0x025EDE0C,void,self,model);u32 o=ea(self),m=ea(model),root=load<u32>(m+0x2C);u16 flags=load<u16>(root+4);u32 saved=(flags>>1)&1;if(saved)store<u16>(root+4,flags&0xFFFD);s32 value=(s32)load<u8>(m+0x6C)+1,divisor=load<u8>(root+6);u32 index=divisor?(u32)(value%divisor):(u32)value;call(0x027DE778,at<void>(m+0x14),index);if(saved)store<u16>(root+4,load<u16>(root+4)|2);u32 matrices=call<u32>(0x027DBBB0,at<void>(root+0x18),index),input=load<u32>(o),count=load<u32>(input),output=load<u32>(o+4),end=output+(count*12&~31u);if(output<end){for(u32 p=output;p<end;p+=32){u32 block=p&~31u;for(u32 j=0;j<32;j++)store<u8>(block+j,0);}input=load<u32>(o);count=load<u32>(input);}end=output+count*12;Local<u8[12]> transformed;u32 t=ea(transformed.get());for(u32 p=output,q=input+4;p<end;p+=12,q+=0x34){f32 zero=load<f32>(0x100586DC);store<f32>(p,zero);store<f32>(p+8,zero);store<f32>(p+4,zero);u32 weight=q+0xC,limit=q+0x2C,bone=limit;for(;weight<limit;weight+=4,bone++){s8 id=load<s8>(bone);if(id<0)continue;call(0x028E8F64,at<void>(matrices+(s32)id*0x30),at<void>(q),at<void>(t));f32 z=load<f32>(t+8),w=load<f32>(weight),x=load<f32>(t),y=load<f32>(t+4);z=fmuls_ppc(z,w);x=fmuls_ppc(x,w);y=fmuls_ppc(y,w);store<f32>(t,x);store<f32>(t+4,y);store<f32>(t+8,z);f32 px=load<f32>(p),py=load<f32>(p+4);store<f32>(p,fadds_ppc(px,x));store<f32>(p+4,fadds_ppc(py,y));store<f32>(p+8,fadds_ppc(load<f32>(p+8),load<f32>(t+8)));}}}
VERIFY(0x025EDE0C,ext_weightCalc);
void ext_tailGlobals(){WWHD_FUNC(0x025EDFA0,void);store<u32>(0x1048CF10,0);store<u32>(0x1048CF08,0);store<u32>(0x1048CF14,0);store<u32>(0x1048CF0C,0);call(0x028F026C,at<void>(0x101F4794));f32 lower=load<f32>(0x10058C48),upper=load<f32>(0x10058C4C);store<f32>(0x1048CEDC,lower);store<f32>(0x1048CEE0,upper);call(0x028ED6F8,at<void>(0x1048CEE5));call(0x028F026C,at<void>(0x101F47A0));call(0x028EAB2C,at<void>(0x1048CEE6));call(0x028F026C,at<void>(0x101F47AC));}
VERIFY(0x025EDFA0,ext_tailGlobals);
void ext_inlineDelete(void* self,u32 flag){WWHD_FUNC(0x025EE034,void,self,flag);if(ea(self)&&(flag&1))call(0x0273AF40,self);}
VERIFY(0x025EE034,ext_inlineDelete);
u32 ext_heapCF34(){WWHD_FUNC(0x025EE048,u32);return load<u32>(0x1048CF34);}
VERIFY(0x025EE048,ext_heapCF34);
u32 ext_heapCF38(){WWHD_FUNC(0x025EE054,u32);return load<u32>(0x1048CF38);}
VERIFY(0x025EE054,ext_heapCF38);
u32 ext_commandHeap(){WWHD_FUNC(0x025EE060,u32);return load<u32>(0x1048CF3C);}
VERIFY(0x025EE060,ext_commandHeap);
u32 ext_heapCF40(){WWHD_FUNC(0x025EE06C,u32);return load<u32>(0x1048CF40);}
VERIFY(0x025EE06C,ext_heapCF40);
u32 ext_heapCF48(){WWHD_FUNC(0x025EE078,u32);return load<u32>(0x1048CF48);}
VERIFY(0x025EE078,ext_heapCF48);
void ext_inlineArrayDtor(void* self,u32 flag){WWHD_FUNC(0x025EE084,void,self,flag);u32 o=ea(self);if(!o)return;u32 p=load<u32>(o+0x1C);store<u32>(o+0xC,0x1005874C);store<u32>(o+0x14,0x1005875C);if(p){for(u32 i=0;(s32)i<(s32)load<u32>(o+0x18);i++){u32 q=p+i*0xA0;call_ptr(load<u32>(load<u32>(q+0xC)+0xC),at<void>(q),2);p=load<u32>(o+0x1C);}u32 manager=call<u32>(0x02755FEC,at<void>(load<u32>(0x101F8B4C)),at<void>(p));call_ptr(load<u32>(load<u32>(manager+0xC)+0x3C),at<void>(manager),at<void>(load<u32>(o+0x1C)));store<u32>(o+0x18,0);store<u32>(o+0x1C,0);}call(0x025E39C8,self,0);if(flag&1)call(0x0273AF40,self);}
VERIFY(0x025EE084,ext_inlineArrayDtor);
void* ext_inlineShapeCtor(void* self){WWHD_FUNC(0x025EE17C,void*,self);u32 o=ea(self);if(!o){o=call<u32>(0x0273AD10,0x68);if(!o)return nullptr;}call(0x027DA984,at<void>(o));store<u32>(o+0x44,0);store<u32>(o+0x34,0x1016D820);return at<void>(o);}
VERIFY(0x025EE17C,ext_inlineShapeCtor);
