#include "d_particle_local.h"
using namespace pa;
void dPa_colorS10ToFloat(void* out,void* color) {
 WWHD_FUNC(0x025A36E0,void,out,color);u32 c=gabi::ea(color),o=gabi::ea(out);
 f32 x=f32(sh(c))/f(0x100519F0),y=f32(sh(c+2))/f(0x100519F0),z=f32(sh(c+4))/f(0x100519F0),a=f32(sh(c+6))/f(0x100519F0);
 F(o,x);F(o+4,y);F(o+8,z);F(o+12,a);
}
VERIFY(0x025A36E0,dPa_colorS10ToFloat);
void dPa_colorToFloat(void* out,void* color) {
 WWHD_FUNC(0x025A37A4,void,out,color);u32 c=gabi::ea(color),o=gabi::ea(out);
 f32 x=f32(b(c))/f(0x100519F0),y=f32(b(c+1))/f(0x100519F0),z=f32(b(c+2))/f(0x100519F0),a=f32(b(c+3))/f(0x100519F0);
 F(o,x);F(o+4,y);F(o+8,z);F(o+12,a);
}
VERIFY(0x025A37A4,dPa_colorToFloat);
void dPa_setWindPower(void* particle) {
 WWHD_FUNC(0x025A39FC,void,particle);u32 a=gabi::ea(particle);gabi::Local<Vec> pos,wind,scaled;gabi::Local<f32> power;
 F(gabi::ea(pos.get()),f(a+16));F(gabi::ea(pos.get())+4,f(a+20));F(gabi::ea(pos.get())+8,f(a+24));
 gabi::call<void>(0x0257E1B8,pos.get(),wind.get(),power.get());
 f32 amount=gabi::fmuls_ppc(f(gabi::ea(power.get())),f(0x10051A08));gabi::call<void>(0x0201AE48,wind.get(),scaled.get(),amount);
 u32 x=gabi::ea(pos.get()),s=gabi::ea(scaled.get());f32 vx=f(x),vy=f(x+4),sx=f(s),sy=f(s+4);f32 ox=gabi::fadds_ppc(vx,sx),oy=gabi::fadds_ppc(vy,sy),sz=f(s+8),vz=f(x+8);F(a+16,ox);F(a+20,oy);F(a+24,gabi::fadds_ppc(vz,sz));
}
VERIFY(0x025A39FC,dPa_setWindPower);
s32 dPa_effectNeedsSpecialWind(u32 id) {
 WWHD_FUNC(0x025A3A98,s32,id);
 return (id>=0x38&&id<=0x39)||(id>=0x3C&&id<=0x40)||id==0x279||id==0x27C||(id>=0x467&&id<=0x469)||id==0x2041||(id>=0x80CB&&id<=0x80CC)||id==0x80CE||(id>=0x80DC&&id<=0x80DD)||(id>=0x82AA&&id<=0x82AC)||(id>=0x82D7&&id<=0x82D8)||(id>=0x8407&&id<=0x840F)||(id>=0x8419&&id<=0x841C)||id==0x8443;
}
VERIFY(0x025A3A98,dPa_effectNeedsSpecialWind);
void* dPa_modelEmitterCtor(void* obj){WWHD_FUNC(0x025A3B80,void*,obj);obj=allocate(obj,20);if(obj){u32 a=gabi::ea(obj);W(a+12,0);W(a+16,0x10051B7C);gabi::call<void>(0x02019C94,obj);}return obj;}
VERIFY(0x025A3B80,dPa_modelEmitterCtor);
void dPa_modelEmitterDtor(void* obj,u32 flag){WWHD_FUNC(0x025A4158,void,obj,flag);if(obj){u32 a=gabi::ea(obj),heap=w(a+20);W(a+16,0x100522A8);if(heap)gabi::call<void>(0x025E3868,p(heap));destroy(obj,flag);}}
VERIFY(0x025A4158,dPa_modelEmitterDtor);
void* dPa_newModel(u32 mode){WWHD_FUNC(0x025A41BC,void*,mode);u32 a=w(0x101EA4E0);for(u32 i=0;i<80;i++,a+=12)if(!b(a)){B(a,1);u32 model=w(a+4);if(mode==1)model=w(a+8);return p(model);}return nullptr;}
VERIFY(0x025A41BC,dPa_newModel);
void* dPa_J3DModelCtor(void* obj){WWHD_FUNC(0x025A479C,void*,obj);obj=allocate(obj,12);if(obj){u32 a=gabi::ea(obj);W(a+4,0);B(a,0);W(a+8,0);}return obj;}
VERIFY(0x025A479C,dPa_J3DModelCtor);
void dPa_modelControlDtor(void* obj){WWHD_FUNC(0x025A4920,void,obj);u32 node=w(gabi::ea(obj));while(node){u32 next=w(node+8);gabi::call<void>(0x0200FDF4,p(node));u32 fn=w(w(node+16)+12);gabi::call_ptr<void>(fn,p(node),u32(3));node=next;}}
VERIFY(0x025A4920,dPa_modelControlDtor);
void* dPa_simpleCallbackCtor(void* obj){WWHD_FUNC(0x025A4A60,void*,obj);obj=allocate(obj,0x290);if(obj){u32 a=gabi::ea(obj);W(a+4,0);W(a,0x10052070);B(a+11,0);H(a+8,0);B(a+10,0);H(a+12,0);gabi::call<void>(0x028F521C,p(a+16),u32(0x280));W(a+4,0);H(a+12,0);}return obj;}
VERIFY(0x025A4A60,dPa_simpleCallbackCtor);
u32 dPa_getResourceManagerId(u32 id){WWHD_FUNC(0x025A4D40,u32,id);return (id>>15)&1;}
VERIFY(0x025A4D40,dPa_getResourceManagerId);
void dPa_windParticleExecute(void* callback,void* emitter,void* particle){WWHD_FUNC(0x025A5098,void,callback,emitter,particle);gabi::call<void>(0x025A39FC,particle);}
VERIFY(0x025A5098,dPa_windParticleExecute);
void dPa_selectTextureDraw(void* callback,void* emitter){WWHD_FUNC(0x025A50A0,void,callback,emitter);u8 index=b(gabi::ea(callback)+4);gabi::call<void>(0x0282ED90,p(gabi::ea(emitter)+0x9C),index,u32(0),u32(0));}
VERIFY(0x025A50A0,dPa_selectTextureDraw);
void dPa_stripesSetupEnd(void* callback,void* emitter){WWHD_FUNC(0x025A575C,void,callback,emitter);gabi::call<void>(0x0281E728,emitter);}
VERIFY(0x025A575C,dPa_stripesSetupEnd);
void* dPa_followCtor(void* obj,u8 rate,u8 fixed){WWHD_FUNC(0x025A5894,void*,obj,rate,fixed);obj=allocate(obj,20);if(obj){u32 a=gabi::ea(obj);B(a+18,fixed);B(a+17,rate);W(a+4,0);W(a,0x100523C8);W(a+12,0);B(a+16,0);B(a+19,0);W(a+8,0);}return obj;}
VERIFY(0x025A5894,dPa_followCtor);
void dPa_followExecute(void* callback,void* emitter){WWHD_FUNC(0x025A590C,void,callback,emitter);u32 a=gabi::ea(callback),e=gabi::ea(emitter);
 if(!b(a+18)&&!(b(a+16)&2)){u32 pos=w(a+8);u8 group=b(e+0x262);f32 x=f(pos),y=f(pos+4),z=f(pos+8);if(group>=7)y=-y;F(e+0x22C,x);F(e+0x230,y);F(e+0x234,z);u32 angle=w(a+12);if(angle){s16 ay=sh(angle+2),ax=sh(angle),az=sh(angle+4);gabi::call<void>(0x028245AC,ax,ay,az,p(e+0x1F0));}}
 if(w(e+0x254)&8){u32 n=w(e+0x1B4),m=w(e+0x1C0);if(n+m==0){u32 fn=w(w(a)+0x44);gabi::call_ptr<void>(fn,callback);}}
 if(b(a+16)&1){gabi::Local<s16> alpha;H(gabi::ea(alpha.get()),b(e+0x247));gabi::call<void>(0x0200F564,alpha.get(),s16(1),s16(4));B(e+0x247,b(gabi::ea(alpha.get())+1));}}
VERIFY(0x025A590C,dPa_followExecute);
void dPa_followSetup(void* callback,void* emitter,void* pos,void* angle,s8 info){WWHD_FUNC(0x025A5A04,void,callback,emitter,pos,angle,info);u32 a=gabi::ea(callback),e=gabi::ea(emitter);u32 fn=w(w(a)+0x44);gabi::call_ptr<void>(fn,callback);if(!b(a+19)){W(a+4,e);W(e+0x254,w(e+0x254)|0x40);}if(!b(a+18)){if(b(a+17))W(e+0x5C,0);W(a+12,gabi::ea(angle));W(a+8,gabi::ea(pos));B(a+16,0);}if(e&&b(e+0x262)==4)gabi::call<void>(0x0281E440,emitter,u32(1));}
VERIFY(0x025A5A04,dPa_followSetup);
void dPa_followEnd(void* callback){WWHD_FUNC(0x025A5AC8,void,callback);u32 a=gabi::ea(callback),e=w(a+4);if(!e)return;u32 flags=w(e+0x254);W(e+0x5C,~u32(0));W(e+0x254,flags|1);e=w(a+4);W(e+0x254,w(e+0x254)&~u32(0x40));e=w(a+4);W(e+0x1E4,0);u8 bflags=b(a+16);W(a+4,0);B(a+16,bflags|1);}
VERIFY(0x025A5AC8,dPa_followEnd);
static void smokeBase(void* obj){u32 a=gabi::ea(obj);gabi::call<void*>(0x025A5894,obj,u8(0),u8(0));B(a+20,0);W(a,0x10052410);B(a+21,0);gabi::call<void>(0x028F521C,p(a+22),u32(4));W(a+28,0);}
static void smokeColor(void* obj){u32 a=gabi::ea(obj);if(!w(0x101FDD10)){W(0x101FDD10,1);gabi::call<void>(0xC000A848,p(0x101FEC25),p(0x10051A0C),u32(4));}copy(0x101FEC25,a+22,4);}
void* dPa_smokeCtorRate(void* obj,u8 rate){WWHD_FUNC(0x025A5B18,void*,obj,rate);obj=allocate(obj,32);if(obj){u32 a=gabi::ea(obj);smokeBase(obj);smokeColor(obj);B(a+21,0);B(a+17,rate);B(a+20,0xFF);B(a+18,0);W(a+28,0);}return obj;}
VERIFY(0x025A5B18,dPa_smokeCtorRate);
void* dPa_smokeCtorFlags(void* obj,u8 rate,u8 fixed,u8 flag,u8 temporary){WWHD_FUNC(0x025A5C04,void*,obj,rate,fixed,flag,temporary);obj=allocate(obj,32);if(obj){u32 a=gabi::ea(obj);smokeBase(obj);smokeColor(obj);B(a+16,flag);B(a+20,0xFF);B(a+17,rate);B(a+21,0);B(a+18,fixed);B(a+19,temporary);W(a+28,0);}return obj;}
VERIFY(0x025A5C04,dPa_smokeCtorFlags);
void* dPa_smokeCtorColor(void* obj,void* color,void* tev,u8 rate){WWHD_FUNC(0x025A5CEC,void*,obj,color,tev,rate);obj=allocate(obj,32);if(obj){u32 a=gabi::ea(obj);smokeBase(obj);copy(gabi::ea(color),a+22,4);B(a+17,rate);B(a+20,0xFF);W(a+28,gabi::ea(tev));B(a+18,0);}return obj;}
VERIFY(0x025A5CEC,dPa_smokeCtorColor);
void dPa_smokeExecute(void* callback,void* emitter){WWHD_FUNC(0x025A5D90,void,callback,emitter);u32 a=gabi::ea(callback);gabi::Local<Color> color;copy(a+22,gabi::ea(color.get()),4);s8 info=s8(b(a+20));u32 tev=w(a+28);gabi::call<void>(0x025A3858,emitter,p(tev),info,color.get());}
VERIFY(0x025A5D90,dPa_smokeExecute);
void dPa_smokeEnd(void* callback){WWHD_FUNC(0x025A5F88,void,callback);u32 a=gabi::ea(callback),e=w(a+4);if(!e)return;u32 flags=w(e+0x254);W(e+0x5C,~u32(0));W(e+0x254,flags|1);e=w(a+4);W(e+0x254,w(e+0x254)&~u32(0x40));e=w(a+4);W(e+0x1E4,0x1047B284);u8 bflags=b(a+16);W(a+4,0);B(a+16,bflags|1);}
VERIFY(0x025A5F88,dPa_smokeEnd);
void dPa_simpleDraw(void* callback,void* emitter){WWHD_FUNC(0x025A4CB4,void,callback,emitter);u32 a=gabi::ea(callback);if(b(a+11)){if(!w(0x101FDD10)){W(0x101FDD10,1);gabi::call<void>(0xC000A848,p(0x101FEC25),p(0x10051A0C),u32(4));}gabi::Local<Color> color;copy(0x101FEC25,gabi::ea(color.get()),4);gabi::call<void>(0x025A3858,emitter,nullptr,s32(-1),color.get());}}
VERIFY(0x025A4CB4,dPa_simpleDraw);
void* dPa_simpleCreateEmitter(void* callback,void* manager){WWHD_FUNC(0x025A4D48,void*,callback,manager);u32 a=gabi::ea(callback),emitter=w(a+4);if(emitter)return p(emitter);if(!w(0x1047B1CC)){W(0x1047B1CC,1);f32 zero=f(0x10051C28);F(0x1047B1D0,zero);F(0x1047B1D8,zero);F(0x1047B1D4,zero);}u16 id=h(a+8);u32 resource=gabi::call<u32>(0x025A4D40,u32(id));u8 group=b(a+10);void* result=gabi::call<void*>(0x02821448,manager,p(0x1047B1D0),id,group,resource,u32(0),u32(0));W(a+4,gabi::ea(result));if(!result)return result;W(gabi::ea(result)+0x1E4,a);emitter=w(a+4);W(emitter+0x5C,0);emitter=w(a+4);W(emitter+0x254,w(emitter+0x254)|1);return p(w(a+4));}
VERIFY(0x025A4D48,dPa_simpleCreateEmitter);
s32 dPa_simpleSet(void* callback,void* pos,u8 alpha,void* primary,void* environment,s32 flags){WWHD_FUNC(0x025A4F7C,s32,callback,pos,alpha,primary,environment,flags);u32 a=gabi::ea(callback);s16 count=sh(a+12);if(count>=32)return 0;u32 out=a+16+u32(s32(count)*20),v=gabi::ea(pos),c=gabi::ea(primary),env=gabi::ea(environment);W(out,w(v));W(out+4,w(v+4));W(out+8,w(v+8));B(out+12,b(c));B(out+13,b(c+1));u8 cb=b(c+2);B(out+15,alpha);B(out+14,cb);B(out+16,b(env));B(out+17,b(env+1));B(out+18,b(env+2));u16 id=h(a+8);if(gabi::call<s32>(0x025A3A98,u32(id))){gabi::Local<Color> sea,foam;gabi::call<void>(0x025602F0,sea.get(),foam.get());u32 s=gabi::ea(sea.get());B(out+12,b(s));B(out+13,b(s+1));B(out+14,b(s));B(out+16,b(s));B(out+17,b(s+1));B(out+18,b(s+2));}B(out+19,flags);H(a+12,u16(sh(a+12)+1));return 1;}
VERIFY(0x025A4F7C,dPa_simpleSet);
void dPa_bombSmokeExecute(void* callback,void* emitter){WWHD_FUNC(0x025A72F4,void,callback,emitter);u32 e=gabi::ea(emitter),work=w(e+0x258),target=work&255,step=(work>>8)&255;gabi::Local<u32> timer;gabi::Local<s16> alpha;W(gabi::ea(timer.get()),work>>16);if(!gabi::call<u32>(0x025AAE08,timer.get())){H(gabi::ea(alpha.get()),b(e+0x247));s32 reached=gabi::call<s32>(0x0200F564,alpha.get(),s32(target),s32(step));u8 value=b(gabi::ea(alpha.get())+1);if(reached)W(e+0x1E4,0);B(e+0x247,value);}W(e+0x258,(w(gabi::ea(timer.get()))<<16)|(step<<8)|target);}
VERIFY(0x025A72F4,dPa_bombSmokeExecute);
void dPa_singleRippleExecute(void* callback,void* emitter){WWHD_FUNC(0x025A71D4,void,callback,emitter);gabi::Local<Color> sea,foam;gabi::call<void>(0x025602F0,sea.get(),foam.get());u32 s=gabi::ea(sea.get()),e=gabi::ea(emitter);u8 z=b(s+2),y=b(s+1);B(e+0x246,z);u8 x=b(s);B(e+0x245,y);B(e+0x244,x);}
VERIFY(0x025A71D4,dPa_singleRippleExecute);
void dPa_applyLighting(void* emitter,void* tev,s32 info,void* color){WWHD_FUNC(0x025A3858,void,emitter,tev,info,color);u32 e=gabi::ea(emitter),t=gabi::ea(tev);gabi::Local<Color> inputColor;W(gabi::ea(inputColor.get()),w(gabi::ea(color)));if(b(e+0x18E)&1)return;gabi::Local<ColorF> floats;gabi::Local<Color> ambient;gabi::Local<Color16> diffuse;
 if(!t){u32 env=gabi::ea(gabi::call<void*>(0x02555D0C));u32 d=gabi::ea(diffuse.get()),a=gabi::ea(ambient.get());H(d,b(env+0xB64));H(d+2,b(env+0xB65));H(d+6,255);H(d+4,b(env+0xB66));B(a,b(env+0xB68));B(a+1,b(env+0xB69));B(a+3,255);B(a+2,b(env+0xB6A));gabi::call<void>(0x025A36E0,floats.get(),diffuse.get());u32 v=gabi::ea(floats.get());W(e+0x29C,w(v+12));W(e+0x290,w(v));W(e+0x298,w(v+8));W(e+0x294,w(v+4));gabi::call<void>(0x025A37A4,floats.get(),ambient.get());W(e+0x2B0,w(v));W(e+0x2BC,w(v+12));W(e+0x2B8,w(v+8));W(e+0x2B4,w(v+4));}
 else{gabi::call<void>(0x025A36E0,floats.get(),p(t+0x90));u32 v=gabi::ea(floats.get());W(e+0x298,w(v+8));W(e+0x29C,w(v+12));W(e+0x290,w(v));W(e+0x294,w(v+4));gabi::call<void>(0x025A37A4,floats.get(),p(t+0x98));W(e+0x2B0,w(v));W(e+0x2B4,w(v+4));W(e+0x2BC,w(v+12));W(e+0x2B8,w(v+8));}
 gabi::call<void>(0x025A37A4,floats.get(),inputColor.get());u32 v=gabi::ea(floats.get());u32 red=w(v),alpha=w(v+12);W(e+0x2C0,red);W(e+0x2CC,alpha);W(e+0x2C4,w(v+4));W(e+0x2C8,w(v+8));}
VERIFY(0x025A3858,dPa_applyLighting);
void dPa_cutTurnExecuteAfter(void* callback,void* emitter){WWHD_FUNC(0x025A9C3C,void,callback,emitter);u32 a=gabi::ea(callback),e=gabi::ea(emitter);if(b(a+5)){u32 flags=w(e+0x254);W(e+0x1E4,0);W(e+0x5C,~u32(0));W(e+0x254,flags|1);W(a+12,0);return;}s16 count=sh(a+6);if(count<=0)return;u32 positions=w(a+8);if(!positions)return;u32 pos=positions+u32(s32(count)*12)-12;s32 remaining=s32(count)-1;while(remaining>0){void* particle=gabi::call<void*>(0x0281DCB8,emitter);if(particle){u32 q=gabi::ea(particle);f32 y=f(pos+4),x=f(pos),z=f(pos+8);F(q+16,x);u32 mdl=w(q+0xF0);F(q+24,z);F(q+20,y);void* material=gabi::call<void*>(0x027FE640,p(mdl),u32(0));u32 m=gabi::ea(material);u8 flags=b(m+0x190);W(m+0x158,0);W(m+0x154,0);B(m+0x190,flags|4);W(m+0x150,0);}--remaining;pos-=12;}u8 group=b(e+0x262);f32 x=f(pos),z=f(pos+8),y=f(pos+4);if(group>=7)y=-y;F(e+0x22C,x);F(e+0x230,y);F(e+0x234,z);B(e+0x247,b(a+4));H(a+6,0);}
VERIFY(0x025A9C3C,dPa_cutTurnExecuteAfter);
