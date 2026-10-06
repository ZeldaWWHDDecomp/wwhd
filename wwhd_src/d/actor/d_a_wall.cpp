/* d_a_wall.cpp (WWHD): bombable walls (daWall_c) and the TU's support functions.
 * Verified against cking.rpx.
 */
#include "d/actor/d_a_wall.h"
namespace wall {
using gabi::call; using gabi::load; using gabi::store;
// Reserve the ABI linkage area below payloads: real callees may save LR at SP+4.
template<unsigned N> struct StackPayload {
 gabi::Local<u8[N+16]> storage;
 u32 a = storage.a + 16;
};
static constexpr u32 matrix = 0x1048D0CC;
static u32 arc(u32 actor) { return load<u32>(0x101D2D20+4*load<u8>(actor+0x6FC)); }
static u32 play() { return call<u32>(0x025200D4); }
void setMoveBGMtx(u32 actor) {
 WWHD_FUNC(0x024D3BD4, void, actor);
 call(0x028E93CC,matrix,load<f32>(actor+0x314),load<f32>(actor+0x318),load<f32>(actor+0x31C));
 call(0x025F1C28,matrix,load<s16>(actor+0x32A));
 call(0x025F2518,load<f32>(actor+0x330),load<f32>(actor+0x334),load<f32>(actor+0x338));
 call(0x028E90D4,matrix,actor+0x698);
}
VERIFY(0x024D3BD4,setMoveBGMtx);
s32 CreateHeap(u32 actor) {
 WWHD_FUNC(0x024D3C44,s32,actor);
 StackPayload<8> modelName, backgroundName;
 u32 name=arc(actor); s32 index=load<s16>(0x10041FF0+2*load<u8>(actor+0x6FC));
 store<u32>(modelName.a,name); store<u32>(modelName.a+4,0x10041F64);
 u32 data=call<u32>(0x026066C4,load<u32>(0x101F4F28),modelName.a,index);
 if(!data) call(0x0273AA24,0x10041F9C,0x184,0x10041FAC);
 u32 model=call<u32>(0x025E38E0,data,0x80000,0x11000022);
 store<u32>(actor+0x3B4,model); if(!model) return 0;
 setMoveBGMtx(actor);
 u32 background=call<u32>(0x024F23F4,0);
 store<u32>(actor+0x694,background); if(!background) return 0;
 name=arc(actor); index=load<s16>(0x10041FF8+2*load<u8>(actor+0x6FC));
 store<u32>(backgroundName.a,name); store<u32>(backgroundName.a+4,0x10041F64);
 data=call<u32>(0x026066C4,load<u32>(0x101F4F28),backgroundName.a,index);
 return call<s32>(0x0200A030,load<u32>(actor+0x694),data,1,actor+0x698)==0;
}
VERIFY(0x024D3C44,CreateHeap);
s32 CheckCreateHeap(u32 actor) { WWHD_FUNC(0x024D3D6C,s32,actor); return CreateHeap(actor); }
VERIFY(0x024D3D6C,CheckCreateHeap);
void set_tri(u32 actor) {
 WWHD_FUNC(0x024D3D70,void,actor);
 for(unsigned i=0;i<2;++i) {u32 triangle=actor+0x3F4+0x150*i; call(0x0251650C,triangle,0x101D2C74); store<u32>(triangle+0x44,actor+0x3B8);}
 StackPayload<36> vertices;
 u32 type=load<u8>(actor+0x6FC);
 for(unsigned j=0;j<9;++j) store<f32>(vertices.a+4*j,load<f32>(0x10042000+0x30*type+4*j));
 call(0x028E93CC,matrix,load<f32>(actor+0x314),load<f32>(actor+0x318),load<f32>(actor+0x31C));
 call(0x025F1C28,matrix,load<s16>(actor+0x322));
 for(unsigned j=0;j<3;++j) call(0x028E8F64,matrix,vertices.a+12*j,vertices.a+12*j);
 call(0x0201924C,actor+0x50C,vertices.a,vertices.a+12,vertices.a+24);
 type=load<u8>(actor+0x6FC);
 for(unsigned j=0;j<3;++j) for(unsigned k=0;k<3;++k) {
  unsigned vertex=j==0?0:j+1;
  store<f32>(vertices.a+12*j+4*k,load<f32>(0x10042000+0x30*type+12*vertex+4*k));
 }
 for(unsigned j=0;j<3;++j) call(0x028E8F64,matrix,vertices.a+12*j,vertices.a+12*j);
 call(0x0201924C,actor+0x65C,vertices.a,vertices.a+12,vertices.a+24);
}
VERIFY(0x024D3D70,set_tri);
void set_mtx(u32 actor) {
 WWHD_FUNC(0x024D3F08,void,actor);
 f32 scale[3]; for(unsigned i=0;i<3;++i) scale[i]=load<f32>(actor+0x330+4*i);
 u32 model=load<u32>(actor+0x3B4);
 for(unsigned i=0;i<3;++i) store<f32>(model+0xBC+4*i,scale[i]);
 call(0x028E93CC,matrix,load<f32>(actor+0x314),load<f32>(actor+0x318),load<f32>(actor+0x31C));
 call(0x025F1C28,matrix,load<s16>(actor+0x322));
 f32 values[12]; for(unsigned i=0;i<12;++i) values[i]=load<f32>(matrix+4*i);
 model=load<u32>(actor+0x3B4);
 for(unsigned i=0;i<12;++i) store<f32>(model+0xC8+4*i,values[i]);
}
VERIFY(0x024D3F08,set_mtx);
void CreateInit(u32 actor) {
 WWHD_FUNC(0x024D3FE0,void,actor);
 u32 model=load<u32>(actor+0x3B4), type=load<u8>(actor+0x6FC);
 store<u32>(actor+0x348,model?model+0xC8:0);
 u32 bounds=0x10042090+24*type;
 call(0x025D674C,actor,load<f32>(bounds),load<f32>(bounds+4),load<f32>(bounds+8),load<f32>(bounds+12),load<f32>(bounds+16),load<f32>(bounds+20));
 store<f32>(actor+0x364,1.0f);
 call(0x02515F14,actor+0x3B8,0xFF,0xFF,actor);
 set_tri(actor);
 u32 scene=play(); call(0x024EEA6C,scene+0x12A0,load<u32>(actor+0x694),actor);
 set_mtx(actor); call(0x024F43DC,load<u32>(actor+0x694));
 u8 switchNo=load<u8>(actor+0xB3); store<u8>(actor+0x6C8,0); store<s32>(actor+0x6F8,switchNo);
}
VERIFY(0x024D3FE0,CreateInit);
s32 create(u32 actor) {
 WWHD_FUNC(0x024D40AC,s32,actor);
 u32 condition=load<u32>(actor+0x2E4);
 if(!(condition&8)) {
  if(actor) {
   call(0x025D4ED0,actor); store<u32>(actor+0xB4,0x10041F8C);
   call(0x0200BD2C,actor+0x3B8); call(0x02515DA0,actor+0x3D4);
   store<u32>(actor+0x3D0,0x1004AE88); store<u32>(actor+0x3D4,0x1004AEC0);
   call(0x028EFFD0,actor+0x3F4,2,0x150,0x024D4840);
   call(0x025A5CEC,actor+0x6CC,0x101D2CEC,0,0);
   condition=load<u32>(actor+0x2E4);
  }
  store<u32>(actor+0x2E4,condition|8);
 }
 u32 type=(load<u32>(actor+0xB0)>>8)&0xFF; u8 switchNo=load<u8>(actor+0xB3);
 store<u8>(actor+0x6FC,type<3?type:2); store<s32>(actor+0x6F8,switchNo);
 u32 save=load<u32>(0x101F84DC);
 if(call<s32>(0x025BA0C0,save+0x20,switchNo,load<s8>(actor+0x2FE))) return 5;
 if(load<s32>(actor+0x6F8)==255) return 5;
 s32 phase=call<s32>(0x02520460,actor+0x3AC,arc(actor));
 if(phase==4) {
  s32 heapSize=load<s16>(0x10041FE8+2*load<u8>(actor+0x6FC));
  if(!call<s32>(0x025D63E8,actor,0x024D3D6C,heapSize)) return 5;
  CreateInit(actor);
 }
 return phase;
}
VERIFY(0x024D40AC,create);
s32 Create(u32 actor) { WWHD_FUNC(0x024D4254,s32,actor); return create(actor); }
VERIFY(0x024D4254,Create);
s32 remove(u32 actor) {
 WWHD_FUNC(0x024D4258,s32,actor);
 u32 vtable=load<u32>(actor+0x6CC); gabi::call_ptr(load<u32>(vtable+0x44),actor+0x6CC);
 if(load<u32>(actor+0xF4)&&!load<u8>(actor+0x6C8)) {u32 scene=play(); call(0x020087EC,scene+0x12A0,load<u32>(actor+0x694));}
 call(0x025204C8,actor+0x3AC,arc(actor)); return 1;
}
VERIFY(0x024D4258,remove);
s32 Delete(u32 actor) {WWHD_FUNC(0x024D42D8,s32,actor); return remove(actor);}
VERIFY(0x024D42D8,Delete);
s32 draw(u32 actor) {
 WWHD_FUNC(0x024D42DC,s32,actor);
 u32 light=call<u32>(0x02555D0C); call(0x025626A4,light,1,actor+0x314,actor+0x110);
 light=call<u32>(0x02555D0C); call(0x02562F5C,light,load<u32>(actor+0x3B4),actor+0x110);
 call(0x025E2DE0,load<u32>(actor+0x3B4),0); return 1;
}
VERIFY(0x024D42DC,draw);
s32 Draw(u32 actor) {WWHD_FUNC(0x024D4338,s32,actor); return draw(actor);}
VERIFY(0x024D4338,Draw);
s32 execute(u32 actor) {
 WWHD_FUNC(0x024D433C,s32,actor);
 u32 entry=0x10041FC4+8*load<u8>(actor+0x6C8);
 s32 slot=load<s16>(entry+2); actor+=load<s16>(entry);
 u32 target;
 if(slot<0) target=load<u32>(entry+4);
 else {u32 vtable=load<u32>(actor+load<s16>(entry+6)); target=load<u32>(vtable+8*slot+4);}
 gabi::call_ptr(target,actor); return 1;
}
VERIFY(0x024D433C,execute);
s32 Execute(u32 actor) {WWHD_FUNC(0x024D43B0,s32,actor); return execute(actor);}
VERIFY(0x024D43B0,Execute);
void set_effect(u32 actor) {
 WWHD_FUNC(0x024D43B4,void,actor);
 StackPayload<6> reversedAngle; StackPayload<6> debrisIds; StackPayload<6> smokeIds;
 const u16 debris[3]={0xA16E,0xA170,0xA172}; const u16 smoke[3]={0xA16F,0xA171,0xA173};
 for(unsigned i=0;i<3;++i) {store<u16>(debrisIds.a+2*i,debris[i]); store<u16>(smokeIds.a+2*i,smoke[i]);}
 store<f32>(actor+0x6F0,200.0f);
 store<s16>(reversedAngle.a,load<s16>(actor+0x320));
 store<s16>(reversedAngle.a+2,(s16)(load<s16>(actor+0x322)-0x8000));
 store<s16>(reversedAngle.a+4,load<s16>(actor+0x324));
 u32 type=load<u8>(actor+0x6FC);
 if(type<=2) {
  s32 room=load<s8>(actor+0x326); u32 id=load<u16>(debrisIds.a+2*type);
  u32 scene=play(); call<u32>(0x025A847C,load<u32>(scene+0x5AB0),4,id,actor+0x314,actor+0x320,0,0xFF,0,room,actor+0x1A8,actor+0x1A8,0);
  type=load<u8>(actor+0x6FC); room=load<s8>(actor+0x326); id=load<u16>(debrisIds.a+2*type);
  scene=play(); call<u32>(0x025A847C,load<u32>(scene+0x5AB0),4,id,actor+0x314,reversedAngle.a,0,0xFF,0,room,actor+0x1A8,actor+0x1A8,0);
  type=load<u8>(actor+0x6FC); room=load<s8>(actor+0x326); id=load<u16>(smokeIds.a+2*type);
  u8 alpha=(u8)gabi::ftoi(load<f32>(actor+0x6F0));
  scene=play(); u32 emitter=call<u32>(0x025A847C,load<u32>(scene+0x5AB0),0,id,actor+0x314,actor+0x320,0,alpha,actor+0x6CC,room,0,0,0);
  store<u32>(actor+0x6EC,emitter);
  if(emitter) store<u32>(emitter+0x254,load<u32>(emitter+0x254)|0x40);
 } else store<u32>(actor+0x6EC,0);
 store<u8>(actor+0x6F4,1); store<u8>(actor+0x6C8,1);
 u32 scene=play(); call(0x020087EC,scene+0x12A0,load<u32>(actor+0x694));
 s32 switchNo=load<s32>(actor+0x6F8);
 if(switchNo!=255) call(0x025B9E38,load<u32>(0x101F84DC)+0x20,switchNo,load<s8>(actor+0x2FE));
}
VERIFY(0x024D43B4,set_effect);
void set_se(u32 actor) {
 WWHD_FUNC(0x024D45D0,void,actor);
 s32 reverb=call<s32>(0x02520540,load<s8>(actor+0x326));
 call(0x025E1A40,0x696C,actor+0x37C,0,reverb);
}
VERIFY(0x024D45D0,set_se);
void mode_break(u32 actor) {
 WWHD_FUNC(0x024D4618,void,actor);
 u8 counter=load<u8>(actor+0x6F4);
 if(counter) {
  u8 type=load<u8>(actor+0x6FC); counter=(u8)(counter+1); store<u8>(actor+0x6F4,counter);
  if(type<=2) {
   if(counter>10&&call<s32>(0x0200F5C8,actor+0x6F0,0.0f,2.222222328186035f)) call(0x025D57E0,actor);
   u32 emitter=load<u32>(actor+0x6D0);
   if(emitter) store<u8>(emitter+0x247,(u8)gabi::ftoi(load<f32>(actor+0x6F0)));
  } else call(0x025D57E0,actor);
 }
 call(0x025DA884,actor+0xDC);
}
VERIFY(0x024D4618,mode_break);
void mode_wait(u32 actor) {
 WWHD_FUNC(0x024D46D4,void,actor);
 for(unsigned i=0;i<2;++i) {
  u32 tri=actor+0x3F4+0x150*i;
  if(call<s32>(0x025162A4,tri)) {
   u32 hit=call<u32>(0x02516300,tri);
   if(hit&&(load<u32>(hit+0x10)&0x20)) {set_effect(actor);set_se(actor);break;}
   call(0x0251621C,tri);
  }
 }
 for(unsigned i=0;i<2;++i) {u32 scene=play();call(0x0200E240,scene+0x26A4,actor+0x3F4+0x150*i);}
}
VERIFY(0x024D46D4,mode_wait);
void staticInit() {
 WWHD_FUNC(0x024D4798,void);
 for(unsigned i=0;i<4;++i) store<u32>(0x1046EAE4+4*i,0);
 call(0x028F026C,0x101D2CC8);
 store<f32>(0x1046EAD8,load<f32>(0x10041FE0));store<f32>(0x1046EADC,load<f32>(0x10041FE4));
 call(0x028ED6F8,0x1046EAE0);call(0x028F026C,0x101D2CD4);
 call(0x028EAB2C,0x1046EAE1);call(0x028F026C,0x101D2CE0);
}
VERIFY(0x024D4798,staticInit);
void staticDestructor(u32 object,u32 deleting) {WWHD_FUNC(0x024D482C,void,object,deleting);if(object&&(deleting&1))call(0x0273AF40,object);}
VERIFY(0x024D482C,staticDestructor);
u32 triangleConstructor(u32 triangle) {
 WWHD_FUNC(0x024D4840,u32,triangle);
 if(!triangle) {triangle=call<u32>(0x0273AD10,0x150);if(!triangle)return 0;}
 call(0x02515FB8,triangle);
 store<u32>(triangle+0x114,0x100015A8);store<u32>(triangle+0x110,0x10041F7C);
 call(0x02019040,triangle+0x118);
 store<u32>(triangle+0x3C,0x1004B010);store<u32>(triangle+0x128,0x1004B058);store<u32>(triangle+0x114,0x1004B068);
 return triangle;
}
VERIFY(0x024D4840,triangleConstructor);
s32 IsDelete(u32 actor) {WWHD_FUNC(0x024D48CC,s32,actor);return 1;}
VERIFY(0x024D48CC,IsDelete);
void destructor(u32 actor,u32 deleting) {
 WWHD_FUNC(0x024D48D4,void,actor,deleting);
 if(actor) {call(0x028F0164,actor+0x3F4,2,0x150,0x025159F8,0,0);call(0x02515860,actor+0x3B8,2);call(0x025D50BC,actor,0);if(deleting&1)call(0x0273AF40,actor);}
}
VERIFY(0x024D48D4,destructor);
}
