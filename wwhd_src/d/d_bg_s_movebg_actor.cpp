#include "wwhd.h"
#include "gabi.h"
static u32 vcall(u32 self,u32 slot){u32 table=gabi::load<u32>(self+0xB4);return gabi::load<u32>(table+slot);}
static void matrix(u32 self){
 f32 x=gabi::load<f32>(self+0x314),z=gabi::load<f32>(self+0x31C),y=gabi::load<f32>(self+0x318);
 gabi::call<void>(0x028E93CC,gabi::at<void>(0x1048D0CC),x,y,z);
 s32 angle=gabi::load<s16>(self+0x32A);gabi::call<void>(0x025F1C28,gabi::at<void>(0x1048D0CC),angle);
 f32 sy=gabi::load<f32>(self+0x334),sx=gabi::load<f32>(self+0x330),sz=gabi::load<f32>(self+0x338);
 gabi::call<void>(0x025F2518,sx,sy,sz);
 gabi::call<void>(0x028E90D4,gabi::at<void>(0x1048D0CC),gabi::at<void>(self+0x3B0));
}
s32 MoveBG_heap(u32 self){
 WWHD_FUNC(0x024F1C44,s32,self);
 if(gabi::call_ptr<s32>(vcall(self,0x14),self)==0)return 0;
 u32 bg=gabi::call<u32>(0x024F23F4,0);
 if(bg){
  gabi::store<u32>(self+0x3AC,bg);
  u32 resourceIndex=gabi::load<u32>(0x101D5304);u32 controller=gabi::load<u32>(0x101F4F28);u32 name=gabi::load<u32>(0x101D5300);
  struct Key{be<u32> name,table;};gabi::Local<Key> key;key->name=name;key->table=0x10043BAC;
  u32 resource=gabi::call<u32>(0x026066C4,controller,key.get(),resourceIndex);
  bg=gabi::load<u32>(self+0x3AC);
  if(gabi::call<s32>(0x0200A030,bg,resource,1,gabi::at<void>(self+0x3B0))==0){
   u32 callback=gabi::load<u32>(0x101D5308);
   if(callback){bg=gabi::load<u32>(self+0x3AC);gabi::store<u32>(bg+0xA8,callback);}return 1;
  }
 }
 gabi::store<u32>(self+0x3AC,0);return 0;
}
VERIFY(0x024F1C44,MoveBG_heap);
s32 MoveBG_heap_callback(u32 self){WWHD_FUNC(0x024F1D3C,s32,self);return MoveBG_heap(self);}
VERIFY(0x024F1D3C,MoveBG_heap_callback);
u32 MoveBG_ctor(u32 self){
 WWHD_FUNC(0x024F1D40,u32,self);if(!self)self=gabi::call<u32>(0x0273AD10,0x3E0);
 if(self){gabi::call<void>(0x025D4ED0,self);gabi::store<u32>(self+0x3AC,0);gabi::store<u32>(self+0xB4,0x10043BC4);}return self;
}
VERIFY(0x024F1D40,MoveBG_ctor);
s32 MoveBG_Create(u32 self,u32 name,u32 index,u32 callback,u32 heapSize){
 WWHD_FUNC(0x024F1D9C,s32,self,name,index,callback,heapSize);matrix(self);
 gabi::store<u32>(0x101D5300,name);gabi::store<u32>(0x101D5304,index);gabi::store<u32>(0x101D5308,callback);
 if(gabi::call<s32>(0x025D63E8,self,gabi::at<void>(0x024F1D3C),heapSize)!=0){
  u32 game=gabi::call<u32>(0x025200D4);u32 bg=gabi::load<u32>(self+0x3AC);
  if(gabi::call<s32>(0x024EEA6C,gabi::at<void>(game+0x12A0),bg,self)==0 && gabi::call_ptr<s32>(vcall(self,0x1C),self)!=0)return 4;
 }return 5;
}
VERIFY(0x024F1D9C,MoveBG_Create);
s32 MoveBG_Execute(u32 self){
 WWHD_FUNC(0x024F1E9C,s32,self);
 gabi::Local<be<u32>> output;*output=0;
 s32 result=gabi::call_ptr<s32>(vcall(self,0x24),self,output.get());u32 m=*output;
 if(!m)matrix(self);else gabi::call<void>(0x028E90D4,gabi::at<void>(m),gabi::at<void>(self+0x3B0));
 gabi::call<void>(0x024F43DC,gabi::load<u32>(self+0x3AC));return result;
}
VERIFY(0x024F1E9C,MoveBG_Execute);
s32 MoveBG_Delete(u32 self){
 WWHD_FUNC(0x024F1F64,s32,self);
 s32 result=gabi::call_ptr<s32>(vcall(self,0x34),self);u32 bg=gabi::load<u32>(self+0x3AC);
 if(bg && gabi::load<u32>(bg)<0x100){u32 game=gabi::call<u32>(0x025200D4);bg=gabi::load<u32>(self+0x3AC);gabi::call<void>(0x020087EC,gabi::at<void>(game+0x12A0),bg);}return result;
}
VERIFY(0x024F1F64,MoveBG_Delete);
void MoveBG_empty_delete(u32 self,u32 flags){WWHD_FUNC(0x024F2068,void,self,flags);if(self&&(flags&1))gabi::call<void>(0x0273AF40,self);}
VERIFY(0x024F2068,MoveBG_empty_delete);
s32 MoveBG_CreateHeap(){WWHD_FUNC(0x024F207C,s32);return 1;}
VERIFY(0x024F207C,MoveBG_CreateHeap);
s32 MoveBG_CreateDefault(){WWHD_FUNC(0x024F2084,s32);return 1;}
VERIFY(0x024F2084,MoveBG_CreateDefault);
s32 MoveBG_DrawDefault(){WWHD_FUNC(0x024F208C,s32);return 1;}
VERIFY(0x024F208C,MoveBG_DrawDefault);
s32 MoveBG_ExecuteDefault(){WWHD_FUNC(0x024F2094,s32);return 1;}
VERIFY(0x024F2094,MoveBG_ExecuteDefault);
s32 MoveBG_IsDeleteDefault(){WWHD_FUNC(0x024F209C,s32);return 1;}
VERIFY(0x024F209C,MoveBG_IsDeleteDefault);
s32 MoveBG_DeleteDefault(){WWHD_FUNC(0x024F20A4,s32);return 1;}
VERIFY(0x024F20A4,MoveBG_DeleteDefault);
void MoveBG_dtor(u32 self,u32 flags){WWHD_FUNC(0x024F20AC,void,self,flags);if(self){gabi::call<void>(0x025D50BC,self,0);if(flags&1)gabi::call<void>(0x0273AF40,self);}}
VERIFY(0x024F20AC,MoveBG_dtor);
void d_bg_s_movebg_actor_static_init() {
 WWHD_FUNC(0x024F1FD4,void);
 gabi::store<u32>(0x1046ED90,0);gabi::store<u32>(0x1046ED88,0);gabi::store<u32>(0x1046ED94,0);gabi::store<u32>(0x1046ED8C,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101D52DC));
 f32 lo=gabi::load<f32>(0x10043C1C),hi=gabi::load<f32>(0x10043C20);
 gabi::store<f32>(0x1046ED7C,lo);gabi::store<f32>(0x1046ED80,hi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x1046ED84));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101D52E8));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x1046ED85));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101D52F4));
}
VERIFY(0x024F1FD4,d_bg_s_movebg_actor_static_init);
