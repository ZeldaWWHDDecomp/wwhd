#include "d_particle_local.h"
using namespace pa;
void dPa_empty_025A5A00(){WWHD_FUNC(0x025A5A00,void);}
VERIFY(0x025A5A00,dPa_empty_025A5A00);
void dPa_empty_025AAB5C(){WWHD_FUNC(0x025AAB5C,void);}
VERIFY(0x025AAB5C,dPa_empty_025AAB5C);
void dPa_empty_025AAB60(){WWHD_FUNC(0x025AAB60,void);}
VERIFY(0x025AAB60,dPa_empty_025AAB60);
void dPa_empty_025AAB64(){WWHD_FUNC(0x025AAB64,void);}
VERIFY(0x025AAB64,dPa_empty_025AAB64);
void dPa_empty_025AAB68(){WWHD_FUNC(0x025AAB68,void);}
VERIFY(0x025AAB68,dPa_empty_025AAB68);
void dPa_empty_025AAB6C(){WWHD_FUNC(0x025AAB6C,void);}
VERIFY(0x025AAB6C,dPa_empty_025AAB6C);
void dPa_empty_025AAB70(){WWHD_FUNC(0x025AAB70,void);}
VERIFY(0x025AAB70,dPa_empty_025AAB70);
void dPa_empty_025AAB74(){WWHD_FUNC(0x025AAB74,void);}
VERIFY(0x025AAB74,dPa_empty_025AAB74);
void dPa_empty_025AABA0(){WWHD_FUNC(0x025AABA0,void);}
VERIFY(0x025AABA0,dPa_empty_025AABA0);
void dPa_empty_025AAC04(){WWHD_FUNC(0x025AAC04,void);}
VERIFY(0x025AAC04,dPa_empty_025AAC04);
void dPa_empty_025AAC30(){WWHD_FUNC(0x025AAC30,void);}
VERIFY(0x025AAC30,dPa_empty_025AAC30);
void dPa_empty_025AACC4(){WWHD_FUNC(0x025AACC4,void);}
VERIFY(0x025AACC4,dPa_empty_025AACC4);
void dPa_empty_025AAE04(){WWHD_FUNC(0x025AAE04,void);}
VERIFY(0x025AAE04,dPa_empty_025AAE04);
void dPa_callbackDtor_025AAB48(void* obj,u32 flag){WWHD_FUNC(0x025AAB48,void,obj,flag);destroy(obj,flag);}
VERIFY(0x025AAB48,dPa_callbackDtor_025AAB48);
void dPa_callbackDtor_025AAB78(void* obj,u32 flag){WWHD_FUNC(0x025AAB78,void,obj,flag);destroy(obj,flag);}
VERIFY(0x025AAB78,dPa_callbackDtor_025AAB78);
void dPa_callbackDtor_025AAB8C(void* obj,u32 flag){WWHD_FUNC(0x025AAB8C,void,obj,flag);destroy(obj,flag);}
VERIFY(0x025AAB8C,dPa_callbackDtor_025AAB8C);
void dPa_callbackDtor_025AABA4(void* obj,u32 flag){WWHD_FUNC(0x025AABA4,void,obj,flag);destroy(obj,flag);}
VERIFY(0x025AABA4,dPa_callbackDtor_025AABA4);
void dPa_callbackDtor_025AAC08(void* obj,u32 flag){WWHD_FUNC(0x025AAC08,void,obj,flag);destroy(obj,flag);}
VERIFY(0x025AAC08,dPa_callbackDtor_025AAC08);
void dPa_callbackDtor_025AAC1C(void* obj,u32 flag){WWHD_FUNC(0x025AAC1C,void,obj,flag);destroy(obj,flag);}
VERIFY(0x025AAC1C,dPa_callbackDtor_025AAC1C);
void dPa_callbackDtor_025AAC34(void* obj,u32 flag){WWHD_FUNC(0x025AAC34,void,obj,flag);destroy(obj,flag);}
VERIFY(0x025AAC34,dPa_callbackDtor_025AAC34);
void dPa_callbackDtor_025AAC48(void* obj,u32 flag){WWHD_FUNC(0x025AAC48,void,obj,flag);destroy(obj,flag);}
VERIFY(0x025AAC48,dPa_callbackDtor_025AAC48);
void dPa_callbackDtor_025AAC88(void* obj,u32 flag){WWHD_FUNC(0x025AAC88,void,obj,flag);destroy(obj,flag);}
VERIFY(0x025AAC88,dPa_callbackDtor_025AAC88);
void dPa_callbackDtor_025AAC9C(void* obj,u32 flag){WWHD_FUNC(0x025AAC9C,void,obj,flag);destroy(obj,flag);}
VERIFY(0x025AAC9C,dPa_callbackDtor_025AAC9C);
void dPa_callbackDtor_025AACB0(void* obj,u32 flag){WWHD_FUNC(0x025AACB0,void,obj,flag);destroy(obj,flag);}
VERIFY(0x025AACB0,dPa_callbackDtor_025AACB0);
void dPa_callbackDtor_025AACC8(void* obj,u32 flag){WWHD_FUNC(0x025AACC8,void,obj,flag);destroy(obj,flag);}
VERIFY(0x025AACC8,dPa_callbackDtor_025AACC8);
void dPa_setColorExecute(void* callback,void* emitter){WWHD_FUNC(0x025AABB8,void,callback,emitter);gabi::Local<ColorF> color;gabi::call<void>(0x025A37A4,color.get(),p(gabi::ea(callback)+4));u32 a=gabi::ea(color.get()),e=gabi::ea(emitter);u32 x=w(a),y=w(a+4),z=w(a+8),alpha=w(a+12);W(e+0x2A0,x);W(e+0x2A4,y);W(e+0x2A8,z);W(e+0x2AC,alpha);}
VERIFY(0x025AABB8,dPa_setColorExecute);
void dPa_groundCheckDtor(void* obj,u32 flag){WWHD_FUNC(0x025AAAD0,void,obj,flag);if(obj){u32 a=gabi::ea(obj);W(a+0x20,0x10051B1C);W(a+0x24,0x10051B3C);W(a+0x30,0x10051A8C);gabi::call<void>(0x02008B4C,p(a+16),u32(0));destroy(obj,flag);}}
VERIFY(0x025AAAD0,dPa_groundCheckDtor);
void* dPa_callbackPairCtor(void* obj){WWHD_FUNC(0x025AAC5C,void*,obj);return allocate(obj,8);}
VERIFY(0x025AAC5C,dPa_callbackPairCtor);
void* dPa_colorScaleCtor(void* obj){WWHD_FUNC(0x025AACDC,void*,obj);obj=allocate(obj,24);if(obj){u32 a=gabi::ea(obj);W(a+4,0x10051A34);W(a,0x10051A18);f32 one=f(0x10051C24);F(a+20,one);F(a+8,one);F(a+16,one);F(a+12,one);}return obj;}
VERIFY(0x025AACDC,dPa_colorScaleCtor);
void* dPa_referenceCountCtor(void* obj){WWHD_FUNC(0x025AAD64,void*,obj);obj=allocate(obj,4);if(obj)W(gabi::ea(obj),0);return obj;}
VERIFY(0x025AAD64,dPa_referenceCountCtor);
void dPa_modelEmitterDrawThunk(void* obj,u32 flag){WWHD_FUNC(0x025AADA0,void,obj,flag);u32 a=gabi::ea(obj)+16,fn=w(w(a)+12);gabi::call_ptr<void>(fn,p(a),flag);}
VERIFY(0x025AADA0,dPa_modelEmitterDrawThunk);
void dPa_boundMethodInvoke(void* obj,u32 arg){WWHD_FUNC(0x025AADB0,void,obj,arg);u32 a=gabi::ea(obj),targetObj=w(a+4);if(!targetObj)return;s16 slot=sh(a+10);if(!slot)return;targetObj+=s32(sh(a+8));u32 fn;if(slot<0)fn=w(a+12);else fn=w(w(targetObj+s32(sh(a+14)))+u32(slot)*8+4);gabi::call_ptr<void>(fn,p(targetObj),arg);}
VERIFY(0x025AADB0,dPa_boundMethodInvoke);
u32 dPa_referenceCountDecrement(void* obj){WWHD_FUNC(0x025AAE08,u32,obj);u32 a=gabi::ea(obj),count=w(a);if(count)W(a,--count);return count;}
VERIFY(0x025AAE08,dPa_referenceCountDecrement);
void dPa_mathHeaderInit(){WWHD_FUNC(0x025AAE24,void);W(0x1047B304,0);W(0x1047B2FC,0);W(0x1047B308,0);W(0x1047B300,0);gabi::call<void>(0x028F026C,p(0x101EA4E8));f32 lo=f(0x10052458),hi=f(0x1005245C);F(0x1047B2F0,lo);F(0x1047B2F4,hi);gabi::call<void>(0x028ED6F8,p(0x1047B2F8));gabi::call<void>(0x028F026C,p(0x101EA4F4));gabi::call<void>(0x028EAB2C,p(0x1047B2F9));gabi::call<void>(0x028F026C,p(0x101EA500));}
VERIFY(0x025AAE24,dPa_mathHeaderInit);
