#include "gabi.h"
using gabi::load;using gabi::store;
static void* ptr(u32 a){return gabi::at<void>(a);}
static u32 addr(const void* p){return gabi::ea(p);}
static void* mapVirtual(void* object,u32 slot) {return gabi::call_ptr<void*>(load<u32>(load<u32>(addr(object))+slot),object);}
bool map_IsFloorNo(s32 floor) {WWHD_FUNC(0x0258D8C8,bool,floor);return u32(floor)-123<10;}
VERIFY(0x0258D8C8,map_IsFloorNo);
void* map_getFloorInfo_WithRoom(s32 room) {
 WWHD_FUNC(0x0258D8E4,void*,room);void* floor=nullptr;
 if(room>=0){void* play=gabi::call<void*>(0x025200D4);void* dt=gabi::call<void*>(0x025C11DC,ptr(addr(play)+0x51CC),room);if(dt)floor=mapVirtual(dt,0x1EC);}
 if(!floor){void* play=gabi::call<void*>(0x025200D4);floor=mapVirtual(ptr(addr(play)+0x5150),0x1EC);}
 return floor;
}
VERIFY(0x0258D8E4,map_getFloorInfo_WithRoom);
f32 map_offsetY() {
 WWHD_FUNC(0x0258D954,f32);f32 result=load<f32>(0x10050B44);
 void* play=gabi::call<void*>(0x025200D4);void* stag=mapVirtual(ptr(addr(play)+0x5150),0x15C);
 u32 type=(load<u32>(addr(stag)+12)>>16)&7;
 bool relevant=type==3;
 if(!relevant){play=gabi::call<void*>(0x025200D4);stag=mapVirtual(ptr(addr(play)+0x5150),0x15C);type=(load<u32>(addr(stag)+12)>>16)&7;relevant=type==6;}
 if(relevant){play=gabi::call<void*>(0x025200D4);if(mapVirtual(ptr(addr(play)+0x5150),0x23C)){
  play=gabi::call<void*>(0x025200D4);void* table=mapVirtual(ptr(addr(play)+0x5150),0x23C);u32 t=addr(table),entry=load<u32>(t+4);s32 n=load<s32>(t);
  if(n!=1){gabi::call<void>(0x0273AA24,ptr(0x10050B48),0x5A5,ptr(0x10050B54));n=load<s32>(t);}
  for(s32 i=0;i<n;++i,entry+=16)result=load<f32>(entry+12);
 }}return result;
}
VERIFY(0x0258D954,map_offsetY);
s32 map_GetTopBottomFloorNo(void* stage,void* bottom,void* top) {
 WWHD_FUNC(0x0258DA64,s32,stage,bottom,top);if(!stage)return 0;u32 lo=128,hi=128;
 void* floor=mapVirtual(stage,0x1EC);if(floor && load<s32>(addr(floor))>0){s32 n=load<s32>(addr(floor));u32 e=load<u32>(addr(floor)+4);lo=123;hi=132;
  for(s32 i=0;i<n;++i,e+=20)for(u32 j=0;j<14;++j)if(load<s8>(e+5+j)!=-1){u32 f=load<u8>(e+4);if(f>lo)lo=f;if(f<hi)hi=f;break;}
 }if(bottom)store<u8>(addr(bottom),lo);if(top)store<u8>(addr(top),hi);return 0;
}
VERIFY(0x0258DA64,map_GetTopBottomFloorNo);
void* map_GetFloorInfoDtP(void* floor,f32 y) {
 WWHD_FUNC(0x0258DB6C,void*,floor,y);if(!floor)return nullptr;s32 n=load<s32>(addr(floor));if(n<=0)return nullptr;u32 e=load<u32>(addr(floor)+4);
 for(s32 i=0;i<n;++i,e+=20){if(i==n-1){if(n==1 || !(y<load<f32>(e)))return ptr(e);}
 else {if(i!=0 && y<load<f32>(e))continue;if(y<load<f32>(e+20))return ptr(e);}}
 return nullptr;
}
VERIFY(0x0258DB6C,map_GetFloorInfoDtP);
u8 map_GetFloorNo_WithRoom(s32 room,f32 y) {WWHD_FUNC(0x0258DBF4,u8,room,y);void* floor=map_getFloorInfo_WithRoom(room);void* entry=map_GetFloorInfoDtP(floor,y);return entry?load<u8>(addr(entry)+4):128;}
VERIFY(0x0258DBF4,map_GetFloorNo_WithRoom);
u8 map_GetFloorNo(void* stage,f32 y) {WWHD_FUNC(0x0258DC48,u8,stage,y);return map_GetFloorNo_WithRoom(-1,y);}
VERIFY(0x0258DC48,map_GetFloorNo);
void* map_ScrDsp_ctor(void* self) {
 WWHD_FUNC(0x0258DC50,void*,self);if(!self)self=gabi::call<void*>(0x0273AD10,0x78);if(!self)return nullptr;u32 a=addr(self);
 gabi::call<void>(0x0252CCBC,self);store<u32>(a+8,0);store<u32>(a+4,0);store<u32>(a,0x10050F1C);
 gabi::call<void>(0x028F521C,ptr(a+12),64);gabi::call<void>(0x028F521C,ptr(a+0x4C),12);
 store<u16>(a+0x62,0);store<u8>(a+0x74,0);f32 zero=load<f32>(0x10050B44);store<u16>(a+0x76,0);
 for(u32 off:{0x64u,0x68u,0x70u,0x6Cu})store<f32>(a+off,zero);
 store<u32>(a+0x58,0);store<u16>(a+0x5C,0);store<u8>(a+0x75,0);store<u16>(a+0x60,0);store<u16>(a+0x5E,0);return self;
}
VERIFY(0x0258DC50,map_ScrDsp_ctor);
void map_ScrDsp_draw(void* self) {WWHD_FUNC(0x0258DD08,void,self);}
VERIFY(0x0258DD08,map_ScrDsp_draw);
void map_ScrDsp_setImage(void* self,void* image,void* data) {
 WWHD_FUNC(0x0258DD0C,void,self,image,data);u32 a=addr(self);store<u32>(a+4,addr(data));store<u32>(a+8,addr(image));
 if(!data || !image)store<u32>(a+0x58,0);
 else {u32 first=gabi::call<u32>(0x025F1154,load<u32>(addr(data)+12));u32 d=load<u32>(a+4);u32 base=first+0x3C;u32 second=gabi::call<u32>(0x025F1154,load<u32>(d+0x38));store<u32>(a+0x58,base+second);}
}
VERIFY(0x0258DD0C,map_ScrDsp_setImage);
void map_ScrDsp_init(void* self,void* data,void* image,s32 x,s32 y,s32 width,s32 height,u32 alpha,f32 px,f32 py,f32 sx,f32 sy) {
 WWHD_FUNC(0x0258DD80,void,self,data,image,x,y,width,height,alpha,px,py,sx,sy);
 map_ScrDsp_setImage(self,image,data);u32 a=addr(self);
 store<u16>(a+0x5C,x);store<f32>(a+0x70,sy);store<f32>(a+0x64,px);store<f32>(a+0x6C,sx);store<u16>(a+0x62,height);store<f32>(a+0x68,py);store<u8>(a+0x74,alpha);store<u16>(a+0x60,width);store<u16>(a+0x5E,y);
}
VERIFY(0x0258DD80,map_ScrDsp_init);
void* map_Texture_ctor(void* self) {
 WWHD_FUNC(0x0258DE54,void*,self);if(!self)self=gabi::call<void*>(0x0273AD10,0x68);if(!self)return nullptr;u32 a=addr(self);
 store<u8>(a,0);store<u8>(a+1,0);gabi::call<void>(0x028F521C,ptr(a+4),64);gabi::call<void>(0x028F521C,ptr(a+0x44),12);gabi::call<void>(0x028F521C,ptr(a+0x50),4);
 f32 zero=load<f32>(0x10050B44);store<u32>(a+0x64,0);for(u32 off:{0x5Cu,0x60u,0x54u,0x58u})store<f32>(a+off,zero);return self;
}
VERIFY(0x0258DE54,map_Texture_ctor);
void map_Texture_init(void* self,void* image,u32 index,void* color) {
 WWHD_FUNC(0x0258DEE8,void,self,image,index,color);u32 a=addr(self);store<u32>(a+0x64,index);store<u8>(a+1,load<u8>(addr(image)+8)!=0);
 u32 value=0;for(u32 i=0;i<4;++i)value=(value<<8)|load<u8>(addr(color)+i);
 for(u32 i=0;i<4;++i)store<u8>(a+0x50+i,value>>(24-i*8));store<u8>(a,1);
}
VERIFY(0x0258DEE8,map_Texture_init);
void map_Texture_setScale(void* self,f32 x,f32 y,f32 w,f32 h) {WWHD_FUNC(0x0258DF14,void,self,x,y,w,h);u32 a=addr(self);store<f32>(a+0x5C,w);store<f32>(a+0x58,y);store<f32>(a+0x60,h);store<f32>(a+0x54,x);}
VERIFY(0x0258DF14,map_Texture_setScale);
void* map_Spcl_ctor(void* self) {WWHD_FUNC(0x0258DF28,void*,self);if(!self)self=gabi::call<void*>(0x0273AD10,20);if(!self)return nullptr;u32 a=addr(self);gabi::call<void>(0x0252CCBC,self);store<u32>(a+8,0);store<u32>(a,0x10050F34);store<u16>(a+14,0);store<u16>(a+16,0);store<u16>(a+18,0);store<u8>(a+5,0);store<u8>(a+4,0);store<u16>(a+12,0);return self;}
VERIFY(0x0258DF28,map_Spcl_ctor);
void map_Spcl_init(void* self,s32 num,void* tex) {WWHD_FUNC(0x0258DF9C,void,self,num,tex);u32 a=addr(self);store<u32>(a+8,addr(tex));store<u8>(a+4,num);store<u8>(a+5,0);}
VERIFY(0x0258DF9C,map_Spcl_init);
void* map_SQ_ctor(void* self) {
 WWHD_FUNC(0x0258DFB0,void*,self);if(!self)self=gabi::call<void*>(0x0273AD10,32);if(!self)return nullptr;u32 a=addr(self);gabi::call<void>(0x0252CCBC,self);
 store<u32>(a+0x18,0);store<u8>(a+0x14,0);store<u32>(a,0x10050F4C);store<u32>(a+8,0);store<u32>(a+16,0);store<u32>(a+12,0);store<u32>(a+4,0);gabi::call<void>(0x028F521C,ptr(a+28),4);return self;
}
VERIFY(0x0258DFB0,map_SQ_ctor);
void* map_Tri_ctor(void* self) {
 WWHD_FUNC(0x0258E02C,void*,self);if(!self)self=gabi::call<void*>(0x0273AD10,40);if(!self)return nullptr;u32 a=addr(self);gabi::call<void>(0x0252CCBC,self);
 store<u16>(a+4,0);store<u16>(a+6,0);store<u32>(a,0x10050F64);gabi::call<void>(0x028F521C,ptr(a+8),4);store<u32>(a+24,0);f32 zero=load<f32>(0x10050B44);store<u16>(a+12,0);store<f32>(a+16,zero);store<f32>(a+20,zero);store<u32>(a+32,0);store<u32>(a+36,0);store<u32>(a+28,0);return self;
}
VERIFY(0x0258E02C,map_Tri_ctor);
void map_platformStub_E0C4() {WWHD_FUNC(0x0258E0C4,void);}
VERIFY(0x0258E0C4,map_platformStub_E0C4);
static void* map_pointCtor(void* self,u32 vt) {
 if(!self)self=gabi::call<void*>(0x0273AD10,32);if(!self)return nullptr;u32 a=addr(self);gabi::call<void>(0x0252CCBC,self);
 store<u32>(a+12,0);store<u32>(a,vt);store<u32>(a+8,0);store<u32>(a+16,0);store<u16>(a+20,0);store<u16>(a+22,0);store<u32>(a+4,0);gabi::call<void>(0x028F521C,ptr(a+24),4);store<u8>(a+28,0);return self;
}
void* map_Point_ctor(void* self){WWHD_FUNC(0x0258E0D4,void*,self);return map_pointCtor(self,0x10050F7C);}
VERIFY(0x0258E0D4,map_Point_ctor);
void* map_AGBCursor_ctor(void* self){WWHD_FUNC(0x0258E15C,void*,self);return map_pointCtor(self,0x10050F94);}
VERIFY(0x0258E15C,map_AGBCursor_ctor);
void map_platformStub_E1E4() {WWHD_FUNC(0x0258E1E4,void);}
VERIFY(0x0258E1E4,map_platformStub_E1E4);
u8 map_Room_getDspFloor(void* self,u32 floor,s32 search) {
 WWHD_FUNC(0x0258E5A8,u8,self,floor,search);u32 a=addr(self),n=floor-123;
 auto check=[](u32 index,u32 line){if(index>=10)gabi::call<void>(0x0273AA24,ptr(0x10050BB0),line,ptr(0x10050BBC));};
 check(n,0x720);u32 result=load<u8>(a+2+n);
 if(search && !map_IsFloorNo(result)) {
  n--;while((s32)n>=0){check(n,0x72D);result=load<u8>(a+2+n);if(map_IsFloorNo(result))return result;n--;}
  ++n;while((s32)n<=9){check(n,0x736);result=load<u8>(a+2+n);if(map_IsFloorNo(result))return result;++n;}
  result=255;
 }return result;
}
VERIFY(0x0258E5A8,map_Room_getDspFloor);
void map_Room_delete(void* self) {
 WWHD_FUNC(0x0258E6C8,void,self);u32 a=addr(self);store<u32>(a+0x128,0);store<u8>(a,0);store<u8>(a+1,0);for(u32 i=0;i<10;++i)store<u8>(a+2+i,255);
 f32 zero=load<f32>(0x10050B44),one=load<f32>(0x10050B7C);store<f32>(a+32,zero);store<u8>(a+12,255);store<f32>(a+40,zero);store<u32>(a+16,0xFFFFFFFF);store<f32>(a+44,zero);store<f32>(a+28,zero);store<f32>(a+24,zero);store<f32>(a+36,zero);
 map_ScrDsp_init(ptr(a+0xAC),nullptr,nullptr,0,0,0,0,0,zero,zero,one,one);store<u8>(a+0x44,0);map_Spcl_init(ptr(a+0x30),1,ptr(a+0x44));
}
VERIFY(0x0258E6C8,map_Room_delete);
bool map_RoomCtrl_exists(void* self,s32 no,void* out) {
 WWHD_FUNC(0x0258EC18,bool,self,no,out);u32 a=addr(self),o=addr(out);
 if(!out)gabi::call<void>(0x0273AA24,ptr(0x10050C2C),0xB9D,ptr(0x10050C38));store<u32>(o,0);u32 room=load<u32>(a+4);
 if(!room){gabi::call<void>(0x0273AA24,ptr(0x10050C2C),0xBA5,ptr(0x10050C4C));return false;}
 while(room){if(load<u8>(room)){if(load<u32>(room+16)==u32(no)){store<u32>(o,room);(void)load<u32>(room+0x124);return true;}}else if(!load<u32>(o))store<u32>(o,room);room=load<u32>(room+0x124);}
 return false;
}
VERIFY(0x0258EC18,map_RoomCtrl_exists);
bool map_RoomCtrl_delete(void* self,s32 room) {
 WWHD_FUNC(0x0258ECE0,bool,self,room);gabi::Local<gabi::be<u32>> found;
 if(map_RoomCtrl_exists(self,room,found.get()))gabi::call<void>(0x0258E6C8,ptr(load<u32>(addr(found.get()))));return false;
}
VERIFY(0x0258ECE0,map_RoomCtrl_delete);
void* map_RoomCtrl_next(void* self,void* room) {WWHD_FUNC(0x0258ED18,void*,self,room);return ptr(load<u32>(room?addr(room)+0x124:addr(self)+4));}
VERIFY(0x0258ED18,map_RoomCtrl_next);
void map_RoomCtrl_checkFloor(void* self,u32 a,u32 b,s32 c,s32 x,s32 y,f32 height) {
 WWHD_FUNC(0x0258ED34,void,self,a,b,c,x,y,height);void* room=map_RoomCtrl_next(self,nullptr);
 while(room){if(load<u8>(addr(room)))gabi::call<void>(0x0258E7A4,room,a,b,c,x,y,height);room=map_RoomCtrl_next(self,room);}
}
VERIFY(0x0258ED34,map_RoomCtrl_checkFloor);
void* map_T2_ctor(void* self) {WWHD_FUNC(0x02590E40,void*,self);if(!self)self=gabi::call<void*>(0x0273AD10,0x7C);if(self){gabi::call<void>(0x0252CCBC,self);store<u32>(addr(self),0x10050FAC);}return self;}
VERIFY(0x02590E40,map_T2_ctor);
void* map_Room_ctor(void* self) {WWHD_FUNC(0x02590EE8,void*,self);if(!self)self=gabi::call<void*>(0x0273AD10,0x12C);if(self){u32 a=addr(self);map_Spcl_ctor(ptr(a+0x30));gabi::call<void>(0x028EFFD0,ptr(a+0x44),1,0x68,0x0258DE54);map_ScrDsp_ctor(ptr(a+0xAC));}return self;}
VERIFY(0x02590EE8,map_Room_ctor);
void map_Room_dtor(void* self,u32 flags) {WWHD_FUNC(0x02590F50,void,self,flags);if(self){gabi::call<void>(0x0252CCFC,ptr(addr(self)+0xAC),0);gabi::call<void>(0x0252CCFC,ptr(addr(self)+0x30),0);if(flags&1)gabi::call<void>(0x0273AF40,self);}}
VERIFY(0x02590F50,map_Room_dtor);
static void map_drawableDtor(void* self,u32 flags){if(self){gabi::call<void>(0x0252CCFC,self,0);if(flags&1)gabi::call<void>(0x0273AF40,self);}}
void map_Spcl_dtor(void* self,u32 flags) {WWHD_FUNC(0x02590C8C,void,self,flags);map_drawableDtor(self,flags);}
VERIFY(0x02590C8C,map_Spcl_dtor);
void map_SQ_dtor(void* self,u32 flags) {WWHD_FUNC(0x02590CE4,void,self,flags);map_drawableDtor(self,flags);}
VERIFY(0x02590CE4,map_SQ_dtor);
void map_Tri_dtor(void* self,u32 flags) {WWHD_FUNC(0x02590D3C,void,self,flags);map_drawableDtor(self,flags);}
VERIFY(0x02590D3C,map_Tri_dtor);
void map_Point_dtor(void* self,u32 flags) {WWHD_FUNC(0x02590D90,void,self,flags);map_drawableDtor(self,flags);}
VERIFY(0x02590D90,map_Point_dtor);
void map_AGBCursor_dtor(void* self,u32 flags) {WWHD_FUNC(0x02590DE8,void,self,flags);map_drawableDtor(self,flags);}
VERIFY(0x02590DE8,map_AGBCursor_dtor);
void map_T2_dtor(void* self,u32 flags) {WWHD_FUNC(0x02590E94,void,self,flags);map_drawableDtor(self,flags);}
VERIFY(0x02590E94,map_T2_dtor);
void map_empty_02590CE0() {WWHD_FUNC(0x02590CE0,void);}
VERIFY(0x02590CE0,map_empty_02590CE0);
void map_empty_02590D38() {WWHD_FUNC(0x02590D38,void);}
VERIFY(0x02590D38,map_empty_02590D38);
void map_empty_02590DE4() {WWHD_FUNC(0x02590DE4,void);}
VERIFY(0x02590DE4,map_empty_02590DE4);
void map_empty_02590E3C() {WWHD_FUNC(0x02590E3C,void);}
VERIFY(0x02590E3C,map_empty_02590E3C);
void map_empty_02590FB0() {WWHD_FUNC(0x02590FB0,void);}
VERIFY(0x02590FB0,map_empty_02590FB0);
void map_empty_02590FB4() {WWHD_FUNC(0x02590FB4,void);}
VERIFY(0x02590FB4,map_empty_02590FB4);
void map_Texture_dtor(void* self,u32 flags) {WWHD_FUNC(0x02590C78,void,self,flags);if(self && (flags&1))gabi::call<void>(0x0273AF40,self);}
VERIFY(0x02590C78,map_Texture_dtor);
void map_destroyRooms() {WWHD_FUNC(0x02590BA0,void);gabi::call<void>(0x028F0164,ptr(0x10477798),20,300,0x02590F50,0,0);}
VERIFY(0x02590BA0,map_destroyRooms);
void map_destroyFrames() {WWHD_FUNC(0x02590BC4,void);gabi::call<void>(0x028F0164,ptr(0x10478F08),8,20,0x02590C8C,0,0);}
VERIFY(0x02590BC4,map_destroyFrames);
void map_destroyPoints() {WWHD_FUNC(0x02590BE8,void);gabi::call<void>(0x028F0164,ptr(0x10479400),15,32,0x02590D90,0,0);}
VERIFY(0x02590BE8,map_destroyPoints);
void map_destroyTboxes() {WWHD_FUNC(0x02590C0C,void);gabi::call<void>(0x028F0164,ptr(0x104795E0),8,124,0x02590E94,0,0);}
VERIFY(0x02590C0C,map_destroyTboxes);
void map_destroyDoors() {WWHD_FUNC(0x02590C30,void);gabi::call<void>(0x028F0164,ptr(0x104799C0),16,124,0x02590E94,0,0);}
VERIFY(0x02590C30,map_destroyDoors);
void map_destroyFriends() {WWHD_FUNC(0x02590C54,void);gabi::call<void>(0x028F0164,ptr(0x1047A1A8),3,32,0x02590D90,0,0);}
VERIFY(0x02590C54,map_destroyFriends);
void map_StaticInit() {
 WWHD_FUNC(0x025908FC,void);
 store<u32>(0x10477788,0);store<u32>(0x10477780,0);store<u32>(0x1047778C,0);store<u32>(0x10477784,0);
 gabi::call<void>(0x028F026C,ptr(0x101E9F30));f32 a=load<f32>(0x10050F10),b=load<f32>(0x10050F14);
 store<f32>(0x10477774,a);store<f32>(0x10477778,b);gabi::call<void>(0x028ED6F8,ptr(0x1047777C));
 gabi::call<void>(0x028F026C,ptr(0x101E9F3C));gabi::call<void>(0x028EAB2C,ptr(0x1047777D));gabi::call<void>(0x028F026C,ptr(0x101E9F48));
 gabi::call<void>(0x028EFFD0,ptr(0x10477798),20,300,0x02590EE8);gabi::call<void>(0x028F026C,ptr(0x101E9F54));
 gabi::call<void>(0x028EFFD0,ptr(0x10478F08),8,20,0x0258DF28);gabi::call<void>(0x028F026C,ptr(0x101E9F60));
 gabi::call<void>(0x028EFFD0,ptr(0x10478FA8),8,104,0x0258DE54);
 map_Spcl_ctor(ptr(0x1047A8A4));gabi::call<void>(0x028F026C,ptr(0x101E9F6C));gabi::call<void>(0x028EFFD0,ptr(0x104792E8),1,104,0x0258DE54);
 map_Spcl_ctor(ptr(0x1047A8B8));gabi::call<void>(0x028F026C,ptr(0x101E9F78));gabi::call<void>(0x028EFFD0,ptr(0x10479350),1,104,0x0258DE54);
 map_Tri_ctor(ptr(0x104793B8));gabi::call<void>(0x028F026C,ptr(0x101E9F84));map_AGBCursor_ctor(ptr(0x104793E0));gabi::call<void>(0x028F026C,ptr(0x101E9F90));
 gabi::call<void>(0x028EFFD0,ptr(0x10479400),15,32,0x0258E0D4);gabi::call<void>(0x028F026C,ptr(0x101E9F9C));
 map_SQ_ctor(ptr(0x1047A208));gabi::call<void>(0x028F026C,ptr(0x101E9FA8));map_SQ_ctor(ptr(0x1047A228));gabi::call<void>(0x028F026C,ptr(0x101E9FB4));
 gabi::call<void>(0x028EFFD0,ptr(0x104795E0),8,124,0x02590E40);gabi::call<void>(0x028F026C,ptr(0x101E9FC0));
 gabi::call<void>(0x028EFFD0,ptr(0x104799C0),16,124,0x02590E40);gabi::call<void>(0x028F026C,ptr(0x101E9FCC));
 map_Tri_ctor(ptr(0x1047A180));gabi::call<void>(0x028F026C,ptr(0x101E9FD8));gabi::call<void>(0x028EFFD0,ptr(0x1047A1A8),3,32,0x0258E0D4);gabi::call<void>(0x028F026C,ptr(0x101E9FE4));
 gabi::call<void>(0x0252CCBC,ptr(0x1047A8CC));store<u32>(0x1047A8CC,0x10050FAC);gabi::call<void>(0x028F026C,ptr(0x101E9FF0));
}
VERIFY(0x025908FC,map_StaticInit);

struct MapResourceName {gabi::be<u32> name,empty;};
static void map_loadResource(u32 first,u32 second,u32 discard,u32 selected,u32 output){
 gabi::Local<MapResourceName> a,b;store<u32>(a.a,first);store<u32>(a.a+4,0x10050A34);store<u32>(b.a,second);store<u32>(b.a+4,0x10050A34);
 if(!gabi::call<bool>(0x026124B0,ptr(load<u32>(0x101F4F7C)),a.get(),b.get(),0))return;
 void* archive=gabi::call<void*>(0x027E2DC0);u32 base=addr(archive)+0x4C,relative=load<u32>(base);
 gabi::call<void*>(0x027DFA24,ptr(relative?base+relative:0),ptr(discard));relative=load<u32>(base);
 void* resource=gabi::call<void*>(0x027DFA24,ptr(relative?base+relative:0),ptr(selected));relative=load<u32>(addr(resource));
 if(relative && output)store<u32>(output,addr(resource)+relative);
}
void map_loadFieldResource(){WWHD_FUNC(0x0258EDDC,void);map_loadResource(0x10050C68,0x10050C7C,0x10050C88,0x10050C70,0x1047A874);}
VERIFY(0x0258EDDC,map_loadFieldResource);
void map_loadDungeonResource(){WWHD_FUNC(0x0258EEBC,void);map_loadResource(0x10050C94,0x10050CA8,0x10050CB4,0x10050C9C,0x1047A878);}
VERIFY(0x0258EEBC,map_loadDungeonResource);
void map_decodeFieldResource(){WWHD_FUNC(0x0258EF9C,void);u32 p=load<u32>(0x1047A874);if(p){u32 n=load<u32>(p);store<u32>(0x1047A870,p+4);store<u8>(0x1047A96E,n);}}
VERIFY(0x0258EF9C,map_decodeFieldResource);
void map_createResources(){WWHD_FUNC(0x0258EFC8,void);store<u32>(0x1047A878,0);store<u32>(0x1047A874,0);map_loadFieldResource();map_loadDungeonResource();store<u32>(0x1047A870,0);store<u8>(0x1047A96E,0);map_decodeFieldResource();}
VERIFY(0x0258EFC8,map_createResources);
s32 map_mode(){WWHD_FUNC(0x0258F01C,s32);void* play=gabi::call<void*>(0x025200D4);void* stage=mapVirtual(ptr(addr(play)+0x5150),0x15C);u32 t=(load<u32>(addr(stage)+12)>>16)&7;return t==1||t==5||t==6?2:t==7;}
VERIFY(0x0258F01C,map_mode);
void map_deleteRoom(s32 room){WWHD_FUNC(0x0258F1D8,void,room);if(room>=0){map_RoomCtrl_delete(ptr(0x10477790),room);if(u32(room)==u32(load<s8>(0x1047E6C8)))store<u32>(0x101E9FFC,0);}}
VERIFY(0x0258F1D8,map_deleteRoom);
bool map_isSpecialStage(){WWHD_FUNC(0x0258F594,bool);void* play=gabi::call<void*>(0x025200D4);void* stage=mapVirtual(ptr(addr(play)+0x5150),0x15C);return ((load<u32>(addr(stage)+12)>>16)&7)==7;}
VERIFY(0x0258F594,map_isSpecialStage);
u32 map_platformStub_F8DC(){WWHD_FUNC(0x0258F8DC,u32);return (u32)map_mode();}
VERIFY(0x0258F8DC,map_platformStub_F8DC);
void map_updateDisplayMode(){WWHD_FUNC(0x0258F8E0,void);if(map_mode()==1){u32 a=load<u8>(0x1047A966),b=load<u8>(0x1047A967);if((a==2&&b!=2)||(a==3&&b!=3)){map_platformStub_F8DC();store<u8>(0x1047A95E,0);}}}
VERIFY(0x0258F8E0,map_updateDisplayMode);

void map_clearLists(){WWHD_FUNC(0x0258F950,void);for(u32 a:{0x1047A880u,0x1047A884u,0x1047A87Cu,0x1047A888u})store<u32>(a,0);}
VERIFY(0x0258F950,map_clearLists);
void map_getPointTypes(u32 kind,void* agb,void* gc){WWHD_FUNC(0x02590208,void,kind,agb,gc);if(kind>=25)gabi::call<void>(0x0273AA24,ptr(0x10050E08),0x1F98,ptr(0x10050E14));if(agb)store<u8>(addr(agb),load<u8>(0x10050DD4+kind*2));if(gc)store<u8>(addr(gc),load<u8>(0x10050DD5+kind*2));}
VERIFY(0x02590208,map_getPointTypes);
void map_addPoint(s32 kind,u32 type,s32 room,s32 angle,u32 frame,u32 prm,u32 cursor,u32 sub,f32 x,f32 y,f32 z){
 WWHD_FUNC(0x0258F978,void,kind,type,room,angle,frame,prm,cursor,sub,x,y,z);if(load<u8>(0x1047A95C)>=64)return;
 if(map_mode()!=1&&kind!=1&&kind!=3&&room!=-1){u32 current=load<u32>(0x101E9FFC);if(!current||load<u32>(current+0x10)!=u32(room))return;}
 u32 index=u32(kind)-1;if(index>=21)gabi::call<void>(0x0273AA24,ptr(0x10050D34),0x1A2B,ptr(0x10050D40));
 u32 count=load<u8>(0x1047A95C),p=0x1047A248+count*24;store<u8>(0x1047A95C,count+1);u32 next=load<u8>(0x1047A848+index);store<u8>(0x1047A848+index,count);
 store<f32>(p+8,z);store<u8>(p+14,room);store<u16>(p+12,angle);store<u8>(p+20,sub);store<f32>(p+4,y);store<f32>(p,x);store<u8>(p+18,prm);store<u8>(p+16,type);store<u8>(p+22,next);store<u8>(p+19,cursor);store<u8>(p+15,kind);store<u8>(p+17,frame);
}
VERIFY(0x0258F978,map_addPoint);
void map_addPointDefault(s32 kind,s32 room,s32 angle,u32 frame,u32 prm,u32 cursor,f32 x,f32 y,f32 z){
 WWHD_FUNC(0x02590488,void,kind,room,angle,frame,prm,cursor,x,y,z);if(load<s8>(0x1047E6C8)<0||!load<u32>(0x101E9FFC))return;
 s32 index=kind-1;if(index<0||u32(index)>=21){gabi::call<void>(0x0273AA24,ptr(0x10050E54),0x1FEF,ptr(0x10050E50));return;}
 s32 type=load<s8>(0x10050E60+index);if(type<0){gabi::call<void>(0x0273AA24,ptr(0x10050E54),0x1FF6,ptr(0x10050E50));return;}
 map_addPoint(kind,u8(type),room,angle,frame,prm,cursor,0,x,y,z);
}
VERIFY(0x02590488,map_addPointDefault);

static u32 map_info(u32 room,u32 file,u32 expr){u32 info=load<u32>(room+0x128);if(!info){gabi::call<void>(0x0273AA24,ptr(file),0x46B,ptr(expr));info=load<u32>(room+0x128);}return info;}
static f32 map_checkedBound(u32 room,u32 offset,u32 line,u32 getterfile,u32 getterexpr,u32 file,u32 expr){u32 info=map_info(room,getterfile,getterexpr);if(!info)gabi::call<void>(0x0273AA24,ptr(file),line,ptr(expr));return load<f32>(info+offset);}
bool map_pointInside(s32 detailed,f32 x,f32 y){
 WWHD_FUNC(0x0258F234,bool,detailed,x,y);u32 room=load<u32>(0x101E9FFC);if(!room)return false;u32 flag=load<u8>(room+1);u32 base=detailed?24:0;if(!(flag&(detailed?1:2)))return false;
 for(u32 i=0;i<4;++i){u32 off=base+(i==0?0:i==1?8:i==2?4:12);f32 b;
 if(detailed){static const u32 file[]={0x10050A4C,0x10050A8C,0x10050A6C,0x10050AAC};static const u32 expr[]={0x10050A58,0x10050A98,0x10050A78,0x10050AB8};static const u32 line[]={0x2DD,0x2E9,0x2E3,0x2EF};b=map_checkedBound(room,off,line[i],0x10050CE8,0x10050CF0,file[i],expr[i]);}
 else b=load<f32>(map_info(room,0x10050CE8,0x10050CF0)+off);
 if((i%2==0?(i<2?x:y)<b:(i<2?x:y)>b))return false;
 if(i<3)room=load<u32>(0x101E9FFC);
 }return true;
}
VERIFY(0x0258F234,map_pointInside);
s32 map_classifyPoint(s32 detailed,f32 x,f32 y){WWHD_FUNC(0x0258F4C8,s32,detailed,x,y);if(map_mode()==1)return map_pointInside(detailed,x,y)?3:2;return map_mode()==2;}
VERIFY(0x0258F4C8,map_classifyPoint);
void map_updatePointClass(f32 x,f32 y){WWHD_FUNC(0x0258F568,void,x,y);store<u8>(0x1047A966,map_classifyPoint(1,x,y));}
VERIFY(0x0258F568,map_updatePointClass);
bool map_selectRoom(s32 no){
 WWHD_FUNC(0x0258F08C,bool,no);store<u32>(0x101E9FFC,0);void* room=map_RoomCtrl_next(ptr(0x10477790),nullptr);
 while(room&&load<u32>(addr(room)+0x10)!=u32(no))room=map_RoomCtrl_next(ptr(0x10477790),room);
 if(!room)return false;u32 a=addr(room);store<u32>(0x101E9FFC,a);
 if(load<u8>(a+1)&2){u32 info=map_info(a,0x10050CC0,0x10050CC8);a=load<u32>(0x101E9FFC);store<f32>(0x1047A860,load<f32>(info+16));info=map_info(a,0x10050CC0,0x10050CC8);a=load<u32>(0x101E9FFC);store<f32>(0x1047A864,load<f32>(info+20));store<f32>(0x1047A868,load<f32>(a+24));store<f32>(0x1047A86C,load<f32>(a+28));}
 if(map_mode()!=1&&!load<u8>(0x1047A964)&&load<u8>(0x1047A95E))store<u8>(0x1047A95E,0);return true;
}
VERIFY(0x0258F08C,map_selectRoom);

void map_platformResource_90708(){WWHD_FUNC(0x02590708,void);map_loadResource(0x10050EB4,0x10050EC8,0x10050ED4,0x10050EBC,0);}
VERIFY(0x02590708,map_platformResource_90708);
void map_platformResource_907FC(){WWHD_FUNC(0x025907FC,void);map_loadResource(0x10050EE0,0x10050EF4,0x10050F00,0x10050EE8,0);}
VERIFY(0x025907FC,map_platformResource_907FC);

static s32 map_clampGrid(s32 v){return v<-3?-3:v>3?3:v;}
static s32 map_gridIndex(f32 value,f32 size,f32 bias){return s8(gabi::ftoi(gabi::call<f32>(0x028F4114,gabi::fadds_ppc(f32(f64(value)/size),bias))));}
void map_pointToGrid(void* gx,void* gy,f32 x,f32 y){
 WWHD_FUNC(0x0259058C,void,gx,gy,x,y);if(!gx)gabi::call<void>(0x0273AA24,ptr(0x10050E80),0x16DC,ptr(0x10050E8C));if(!gy)gabi::call<void>(0x0273AA24,ptr(0x10050E80),0x16DD,ptr(0x10050EA0));
 f32 size=load<f32>(0x10050D70),bias=load<f32>(0x10050B88);store<u8>(addr(gx),map_clampGrid(map_gridIndex(x,size,bias)));store<u8>(addr(gy),map_clampGrid(map_gridIndex(y,size,bias)));
}
VERIFY(0x0259058C,map_pointToGrid);
void map_pointToGridLocal(void* gx,void* gy,void* lx,void* ly,f32 x,f32 y){
 WWHD_FUNC(0x0258FD94,void,gx,gy,lx,ly,x,y);if(!gx)gabi::call<void>(0x0273AA24,ptr(0x10050D74),0x1710,ptr(0x10050DA8));
 if(!gy)gabi::call<void>(0x0273AA24,ptr(0x10050D74),0x1711,ptr(0x10050DBC));
 if(!lx)gabi::call<void>(0x0273AA24,ptr(0x10050D74),0x1712,ptr(0x10050D80));
 if(!ly)gabi::call<void>(0x0273AA24,ptr(0x10050D74),0x1713,ptr(0x10050D94));
 f32 size=load<f32>(0x10050D70),bias=load<f32>(0x10050B88);s32 ix=map_clampGrid(map_gridIndex(x,size,bias)),iy=map_clampGrid(map_gridIndex(y,size,bias));
 s32 localX=gabi::ftoi(gabi::fnmsubs(f32(ix),size,x));s32 localY=gabi::ftoi(gabi::fnmsubs(f32(iy),size,y));store<u8>(addr(gx),ix);store<u8>(addr(gy),iy);store<u16>(addr(lx),localX);store<u16>(addr(ly),localY);
}
VERIFY(0x0258FD94,map_pointToGridLocal);

u32 map_getRoomImage(void* self,s32 room,u32 floor,s32 search,void* out0,void* out1,void* outData,void* outInfo){
 WWHD_FUNC(0x0258E1E8,u32,self,room,floor,search,out0,out1,outData,outInfo);
 // Ninth integer argument is the caller stack word at +8; generic VERIFY entries read only GPRs.
 void* outFlags=ptr(load<u32>(gabi::cpu->r[1]+8));u32 image0=0,image1=0,data=0,info=0,flags=0;bool found=false;
 if(map_IsFloorNo(floor)){
  gabi::Local<char[32]> roomName,resourceName;gabi::call<s32>(0x028F186C,roomName.get(),ptr(0x10050BA4),room);
  if(map_IsFloorNo(floor))for(;;){
   gabi::call<s32>(0x028F186C,resourceName.get(),ptr(0x10050B8C),floor);image0=addr(gabi::call<void*>(0x0252447C,roomName.get(),resourceName.get()));
   gabi::call<s32>(0x028F186C,resourceName.get(),ptr(0x10050B94),floor);image1=addr(gabi::call<void*>(0x0252447C,roomName.get(),resourceName.get()));
   gabi::call<s32>(0x028F186C,resourceName.get(),ptr(0x10050B9C),floor);data=addr(gabi::call<void*>(0x0252447C,roomName.get(),resourceName.get()));
   u32 probe=floor;info=0;if(map_IsFloorNo(probe))for(;;){void* play=gabi::call<void*>(0x025200D4);void* dt=gabi::call<void*>(0x025C11DC,ptr(addr(play)+0x51CC),room);info=addr(gabi::call_ptr<void*>(load<u32>(load<u32>(addr(dt))+0x7C),dt,probe));probe=(probe-1)&255;if(info||!map_IsFloorNo(probe))break;}
   if(image0&&image1&&data&&info){found=true;break;}floor=(floor-1)&255;if(!search||!map_IsFloorNo(floor))break;
  }
  if(image0&&info&&load<f32>(info+8)!=load<f32>(info)&&load<f32>(info+12)!=load<f32>(info+4))flags=2;
  if(image1){u32 w=load<u16>(image1+2),h;if(w==128){h=load<u16>(image1+4);if(h<=480&&info&&load<f32>(info+32)!=load<f32>(info+24)&&load<f32>(info+36)!=load<f32>(info+28))flags|=1;}
   else if(!(w&7)&&load<u16>(image1+4)==8&&info&&load<f32>(info+32)!=load<f32>(info+24)&&load<f32>(info+36)!=load<f32>(info+28))flags|=1;}
 }
 if(flags!=3)flags=0;if(out0)store<u32>(addr(out0),image0);if(out1)store<u32>(addr(out1),image1);if(outData)store<u32>(addr(outData),data);if(outInfo)store<u32>(addr(outInfo),info);if(outFlags)store<u8>(addr(outFlags),flags);return found?floor:255;
}
VERIFY(0x0258E1E8,map_getRoomImage);

bool map_Room_changeImage(void* self,u32 floor,u32 otherFloor,s32 currentRoom,s32 sx,s32 sy,f32 height){
 WWHD_FUNC(0x0258E7A4,bool,self,floor,otherFloor,currentRoom,sx,sy,height);u32 a=addr(self),selected;
 gabi::Local<gabi::be<u32>> image0,image1,data,info;store<u32>(image0.a,0);store<u32>(image1.a,0);store<u32>(data.a,0);store<u32>(info.a,0);
 if(map_IsFloorNo(floor))selected=map_Room_getDspFloor(self,floor,load<u32>(a+16)==u32(currentRoom));
 else {gabi::call<void>(0x0273AA24,ptr(0x10050C0C),0x9B3,ptr(0x10050C08));selected=gabi::cpu->r[5];}
 if(selected==load<u8>(a+12))return false;
 store<u8>(a+12,selected);store<u8>(a+1,0);store<u32>(a+0x128,0);
 if(map_IsFloorNo(selected))gabi::call<u32>(0x0258E1E8,self,load<s32>(a+16),selected,0,image0.get(),image1.get(),data.get(),info.get(),ptr(a+1));
 u32 ip=load<u32>(info.a),flags=load<u8>(a+1);if(ip)store<u32>(a+0x128,ip);
 if((flags&2)&&load<u32>(image0.a)){
  if(!load<u32>(0x101FDB64)){store<u32>(0x101FDB64,1);gabi::call<void>(0xC000A848,ptr(0x101FEC00),ptr(0x10050A28),4);}
  map_Texture_init(ptr(a+0x44),ptr(load<u32>(image0.a)),0,ptr(0x101FEC00));f32 zero=load<f32>(0x10050B44),one=load<f32>(0x10050B7C);map_Texture_setScale(ptr(a+0x44),zero,zero,one,one);store<u8>(a+0x44,1);
  ip=load<u32>(info.a);if(ip){store<f32>(a+24,f32(f64(f32(sx))/gabi::fsubs_ppc(load<f32>(ip+8),load<f32>(ip))));store<f32>(a+28,f32(f64(f32(sy))/gabi::fsubs_ppc(load<f32>(ip+12),load<f32>(ip+4))));}
  map_Spcl_init(ptr(a+0x30),1,ptr(a+0x44));flags=load<u8>(a+1);
 }
 if((flags&1)&&load<u32>(data.a)&&load<u32>(info.a)){
  map_ScrDsp_setImage(ptr(a+0xAC),ptr(load<u32>(image1.a)),ptr(load<u32>(data.a)));
  store<f32>(a+40,f32(gabi::call<u32>(0x025F1188,load<u16>(load<u32>(data.a)))));
  store<f32>(a+44,f32(gabi::call<u32>(0x025F1188,load<u16>(load<u32>(data.a)+2))));
  u32 upper=load<u32>(info.a);if(!upper)gabi::call<void>(0x0273AA24,ptr(0x10050C0C),0x2E9,ptr(0x10050C18));u32 lower=load<u32>(info.a);if(!lower)gabi::call<void>(0x0273AA24,ptr(0x10050C0C),0x2DD,ptr(0x10050C18));
  store<f32>(a+32,f32(f64(load<f32>(a+40))/gabi::fsubs_ppc(load<f32>(upper+32),load<f32>(lower+24))));
  upper=load<u32>(info.a);if(!upper)gabi::call<void>(0x0273AA24,ptr(0x10050C0C),0x2EF,ptr(0x10050C18));lower=load<u32>(info.a);if(!lower)gabi::call<void>(0x0273AA24,ptr(0x10050C0C),0x2E3,ptr(0x10050C18));
  store<f32>(a+36,f32(f64(load<f32>(a+44))/gabi::fsubs_ppc(load<f32>(upper+36),load<f32>(lower+28))));
  void* play=gabi::call<void*>(0x025200D4);void* stage=mapVirtual(ptr(addr(play)+0x5150),0x15C);u32 type=(load<u32>(addr(stage)+12)>>16)&7;
  if(type==1){store<u8>(load<u32>(data.a)+0x32,!(load<f32>(load<u32>(info.a)+0x30)<load<f32>(0x10050BF8)));store<u8>(load<u32>(data.a)+6,floor>=128?floor-127:floor-128);}
  else {play=gabi::call<void*>(0x025200D4);stage=mapVirtual(ptr(addr(play)+0x5150),0x15C);type=(load<u32>(addr(stage)+12)>>16)&7;if(type==6){play=gabi::call<void*>(0x025200D4);u32 stageAddr=addr(play)+0x5150;u32 no=map_GetFloorNo(ptr(stageAddr),gabi::fadds_ppc(height,map_offsetY()));store<u8>(load<u32>(data.a)+6,no>=128?no-127:no-128);}else{store<u8>(load<u32>(data.a)+0x32,0);store<u8>(load<u32>(data.a)+6,0);}}
 }return true;
}
VERIFY(0x0258E7A4,map_Room_changeImage);

static u32 map_scrollInfo(u32& room){u32 info=load<u32>(room+0x128);if(!info){gabi::call<void>(0x0273AA24,ptr(0x10050D14),0x46B,ptr(0x10050D1C));info=load<u32>(room+0x128);room=load<u32>(0x101E9FFC);}return info;}
void map_scrollSpecial(f32 x,f32 y){
 WWHD_FUNC(0x0258F5D8,void,x,y);if(!map_isSpecialStage())return;u32 room=load<u32>(0x101E9FFC);if(!(load<u8>(room+1)&2))return;
 u32 q=map_scrollInfo(room),r=load<u32>(room+0x128);f32 upperX=load<f32>(q+8);if(!r){gabi::call<void>(0x0273AA24,ptr(0x10050D14),0x46B,ptr(0x10050D1C));r=load<u32>(room+0x128);room=load<u32>(0x101E9FFC);}
 f32 midX=load<f32>(r+16),dx=gabi::fsubs_ppc(upperX,midX),half=load<f32>(0x10050D08);r=load<u32>(room+0x128);f32 maxX=gabi::fmuls_ppc(__builtin_fabsf(dx),half);if(!r){gabi::call<void>(0x0273AA24,ptr(0x10050D14),0x46B,ptr(0x10050D1C));r=load<u32>(room+0x128);room=load<u32>(0x101E9FFC);}
 q=load<u32>(room+0x128);f32 upperY=load<f32>(r+12);if(!q){gabi::call<void>(0x0273AA24,ptr(0x10050D14),0x46B,ptr(0x10050D1C));q=load<u32>(room+0x128);room=load<u32>(0x101E9FFC);}
 f32 midY=load<f32>(q+20),dy=gabi::fsubs_ppc(upperY,midY);r=load<u32>(room+0x128);f32 maxY=gabi::fmuls_ppc(__builtin_fabsf(dy),load<f32>(0x10050D08));if(!r){gabi::call<void>(0x0273AA24,ptr(0x10050D14),0x46B,ptr(0x10050D1C));r=load<u32>(room+0x128);room=load<u32>(0x101E9FFC);}
 dx=gabi::fsubs_ppc(x,load<f32>(r+16));r=map_scrollInfo(room);dy=gabi::fsubs_ppc(y,load<f32>(r+20));
 f32 ax=__builtin_fabsf(dx),ay=__builtin_fabsf(dy),one=load<f32>(0x10050B7C),minus=load<f32>(0x10050D0C),zero=load<f32>(0x10050B44);
 f32 signX=dx>=0?one:minus,signY=dy>=0?one:minus,scrollX=zero,scrollY=zero;
 if(ax>maxX){f32 extra=gabi::fsubs_ppc(ax,maxX);scrollX=gabi::fadds_ppc(extra,extra);if(scrollX>ax)scrollX=ax;}if(ay>maxY){f32 extra=gabi::fsubs_ppc(ay,maxY);scrollY=gabi::fadds_ppc(extra,extra);if(scrollY>ay)scrollY=ay;}
 if(scrollX!=zero||scrollY!=zero){if(scrollX>scrollY)scrollY=f32(f64(gabi::fmuls_ppc(ay,scrollX))/ax);else scrollX=f32(f64(gabi::fmuls_ppc(ax,scrollY))/ay);}
 r=map_scrollInfo(room);store<f32>(0x1047A860,gabi::fmadds(signX,scrollX,load<f32>(r+16)));r=map_scrollInfo(room);store<f32>(0x1047A864,gabi::fmadds(signY,scrollY,load<f32>(r+20)));store<f32>(0x1047A868,load<f32>(room+24));store<f32>(0x1047A86C,load<f32>(room+28));
}
VERIFY(0x0258F5D8,map_scrollSpecial);

void map_setArriveInfo(f32 x,f32 y){
 WWHD_FUNC(0x02590004,void,x,y);void* play=gabi::call<void*>(0x025200D4);if(load<u8>(addr(play)+0x5292))return;
 gabi::Local<MapResourceName> desired,current;store<u32>(desired.a,0x10050DD0);store<u32>(desired.a+4,0x10050A34);play=gabi::call<void*>(0x025200D4);store<u32>(current.a,addr(play)+0x5134);store<u32>(current.a+4,0x10050A34);
 for(u32 i=0;i<2;++i)gabi::call_ptr<void>(load<u32>(load<u32>(desired.a+4)+20),desired.get());u32 first=load<u32>(desired.a);gabi::call_ptr<void>(load<u32>(load<u32>(current.a+4)+20),current.get());u32 second=load<u32>(current.a);bool equal=first==second;
 if(!equal)for(u32 i=0;i<0x40001;++i){u32 a=load<u8>(first+i),b=load<u8>(second+i);if(a!=b)break;if(!a){equal=true;break;}}
 if(!equal)return;gabi::Local<char[2]> grid;gabi::Local<gabi::be<u16>> lx,ly;map_pointToGridLocal(ptr(grid.a),ptr(grid.a+1),lx.get(),ly.get(),x,y);s32 gx=load<s8>(grid.a),gy=load<s8>(grid.a+1);
 if(gx==load<s8>(0x1047A968)&&gy==load<s8>(0x1047A969))return;store<u8>(0x1047A968,gx);store<u8>(0x1047A969,gy);if(u32(gx+3)<7&&u32(gy+3)<7)gabi::call<void>(0x025B85CC,ptr(load<u32>(0x101F84DC)+0xE4),gx+gy*7+24);map_platformStub_F8DC();
}
VERIFY(0x02590004,map_setArriveInfo);

void map_drawActorPoint(void* actor){
 WWHD_FUNC(0x0259029C,void,actor);if(!load<u32>(0x101E9FFC))return;u32 a=addr(actor),acs=load<u32>(a+0x2E0)&31;gabi::Local<char[2]> types;map_getPointTypes(acs,ptr(types.a),ptr(types.a+1));u32 type=load<u8>(types.a),frame=0,cursor=0;s32 room=load<s8>(a+0x326),angle=-32768;
 if(type==4){void* play=gabi::call<void*>(0x025200D4);if(load<u32>(addr(play)+0x5CD8)&0x10000)return;angle=load<s16>(a+0x32A);}
 else if(type==17){angle=load<s16>(a+0x2FA);if(acs!=11)room=load<s8>(a+0x3E8);}
 else if(type==1){map_setArriveInfo(load<f32>(a+0x314),load<f32>(a+0x31C));angle=load<s16>(a+0x32A);if(acs>=17&&acs<=20)frame=acs-16;}
 else if(type==19&&acs>=12&&acs<=15)frame=acs-11;
 type=load<u8>(types.a);if(type==2){void* play=gabi::call<void*>(0x025200D4);void* attention=ptr(addr(play)+0x5804);if(gabi::call<void*>(0x024EC8D0,attention,0)==actor){cursor=1;if(gabi::call<bool>(0x024EDFCC,attention))cursor=2;}}
 map_addPoint(load<u8>(types.a),load<u8>(types.a+1),room,angle,frame,load<u8>(a+0x2DE),cursor,acs&255,load<f32>(a+0x314),load<f32>(a+0x318),load<f32>(a+0x31C));
}
VERIFY(0x0259029C,map_drawActorPoint);

void map_move(s32 roomNo,f32 x,f32 y,f32 height){
 WWHD_FUNC(0x0258FAD0,void,roomNo,x,y,height);map_updatePointClass(x,y);void* play=gabi::call<void*>(0x025200D4);void* stage=mapVirtual(ptr(addr(play)+0x5150),0x15C);u32 t=(load<u32>(addr(stage)+12)>>16)&7;
 if(t!=5&&t!=7&&u32(roomNo)<64)gabi::call<void>(0x025B8FAC,ptr(load<u32>(0x101F84DC)+0x798),roomNo);
 u32 current=load<u32>(0x101E9FFC);if(roomNo!=-1&&!current){map_selectRoom(roomNo);current=load<u32>(0x101E9FFC);}if(!current)return;
 u32 floor=map_GetFloorNo_WithRoom(roomNo,height);current=load<u32>(0x101E9FFC);
 if(current&&floor!=load<u8>(0x1047A96C)){u32 old=load<u8>(current+12);store<u8>(0x1047A96C,floor);map_RoomCtrl_checkFloor(ptr(0x10477790),floor,floor,roomNo,120,120,height);current=load<u32>(0x101E9FFC);if(old!=load<u8>(current+12)){store<u8>(0x1047A964,0);store<u8>(0x1047A95E,0);}}
 if(roomNo!=-1&&load<u32>(current+16)!=u32(roomNo)){map_selectRoom(roomNo);current=load<u32>(0x101E9FFC);}if(!current)return;
 map_scrollSpecial(x,y);gabi::call<void>(0x0258F8E0,x,y);current=load<u32>(0x101E9FFC);u32 flags=load<u8>(current+1);
 if(load<u8>(0x1047A960)==1){if(flags&2){for(u32 i=0;i<4;++i)store<f32>(0x1047A894+i*4,load<f32>(0x1047A860+i*4));}}
 else if(flags&1){store<f32>(0x1047A894,x);store<f32>(0x1047A898,y);store<f32>(0x1047A89C,load<f32>(current+32));store<f32>(0x1047A8A0,load<f32>(current+36));}
 map_clearLists();current=load<u32>(0x101E9FFC);u32 info=load<u32>(current+0x128),alpha=0;if(info)alpha=(load<u8>(info+0x34)*load<u8>(0x1047A958)>>8)&255;store<u8>(0x1047A957,alpha);
 u32 player=load<u32>(0x101F84DC);map_addPoint(20,9,load<s8>(player+0x1148),load<s16>(player+0x115E),0,0,0,0,load<f32>(player+0x1160),load<f32>(player+0x1164),load<f32>(player+0x1168));
}
VERIFY(0x0258FAD0,map_move);
void map_moveNonnegative(s32 roomNo,f32 x,f32 y,f32 height){WWHD_FUNC(0x0258FD88,void,roomNo,x,y,height);if(roomNo>=0)map_move(roomNo,x,y,height);}
VERIFY(0x0258FD88,map_moveNonnegative);
