#include "d_particle_local.h"
using namespace pa;
// Preserve raw lighting transfers; the final copied float uses the reference's load conversion.
static void copyTevLighting(u32 src,u32 dst,u32 quietMask=0){for(u32 i=0;i<24;i+=4){if(quietMask&(1u<<(i/4)))F(dst+i,f(src+i));else W(dst+i,w(src+i));};for(u32 i=24;i<28;i++)B(dst+i,b(src+i));for(u32 i=28;i<36;i+=2)H(dst+i,h(src+i));for(u32 i=36;i<68;i+=4){if(quietMask&(1u<<(i/4)))F(dst+i,f(src+i));else W(dst+i,w(src+i));};}
void* dPa_J3DModelEmitterCtor(void* obj,void* emitter,void* modelA,void* modelB,void* tev,void* pattern,u32 frame,u32 mode){WWHD_FUNC(0x025A3BDC,void*,obj,emitter,modelA,modelB,tev,pattern,frame,mode);obj=allocate(obj,0x1F4);if(!obj)return obj;u32 a=gabi::ea(obj),e=gabi::ea(emitter),t=gabi::ea(tev);gabi::call<void*>(0x025A3B80,obj);W(a+16,0x100522A8);
 copyTevLighting(0x1016E414,a+0x2C);copyTevLighting(0x1016E414,a+0xEC);copyTevLighting(0x1016E414,a+0x170);
 W(a+12,e);W(a+24,gabi::ea(modelA));W(a+28,gabi::ea(modelB));B(a+0x2A,mode);
 copyTevLighting(t,a+0x2C);for(u32 i=0x84;i<0x90;i+=4)W(a+0x2C+i,w(t+i));for(u32 i=0x90;i<0x98;i+=2)H(a+0x2C+i,h(t+i));copy(t+0x98,a+0xC4,4);copy(t+0x9C,a+0xC8,4);for(u32 i=0xA0;i<0xA8;i+=2)H(a+0x2C+i,h(t+i));for(u32 i=0xA8;i<0xB4;i+=4)W(a+0x2C+i,w(t+i));for(u32 i=0xB4;i<=0xBC;i++)B(a+0x2C+i,b(t+i));copyTevLighting(t+0xC0,a+0xEC);copyTevLighting(t+0x144,a+0x170,1u<<16);
 W(a+0x20,gabi::ea(pattern));if(!pattern){W(a+0x24,0);W(a+20,0);}else{void* heap=gabi::call<void*>(0x025E35BC,u32(0),nullptr,u32(0));W(a+20,gabi::ea(heap));if(heap){void* animation=gabi::call<void*>(0x0273B050,u32(0x74),heap,u32(4));if(animation)animation=gabi::call<void*>(0x025E7820,animation);W(a+0x24,gabi::ea(animation));if(animation){u32 data=w(a+24),pat=w(a+32);gabi::call<void>(0x025E789C,animation,p(data),p(pat),u32(1),u32(2),u32(0),s32(-1),u32(0),u32(0),f(0x10051C24));H(a+0x28,frame);}gabi::call<void>(0x025E37D8);gabi::call<void>(0x025E3678,p(w(a+20)));}}
 W(e+0x254,w(e+0x254)|0x40);return obj;}
VERIFY(0x025A3BDC,dPa_J3DModelEmitterCtor);
