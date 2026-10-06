#include "bindings.h"
namespace doorhk{template<class T>T ld(u32 a,u32 o=0){return gabi::load<T>(a+o);}template<class T>void st(u32 a,u32 o,T v){gabi::store<T>(a+o,v);}void* p(u32 a){return gabi::at<void>(a);}struct Name{be<u32>data,vt;};struct Vec{be<f32>x,y,z;};}
using namespace doorhk;
void door_msg_init(void* self,s32 id){WWHD_FUNC(0x0252BEC4,void,self,id);u32 a=gabi::ea(self);st<s16>(a,0x10A,s16(id));st<u32>(a,0,0xFFFFFFFF);st<u8>(a,0x108,0);}
VERIFY(0x0252BEC4,door_msg_init);
BOOL door_msg_proc(void* self,void* arg){
 WWHD_FUNC(0x0252BEDC,BOOL,self,arg);u32 a=gabi::ea(self),msg=ld<u32>(0x101F4B5C);u8 state=ld<u8>(a,0x108);
 if(state==0){s32 result=gabi::call<s32>(0x025F7DB0,p(msg),ld<s16>(a,0x10A),arg);st<s32>(a,0,result);if(result!=-1)st<u8>(a,0x108,u8(ld<u8>(a,0x108)+1));}
 else if(state==1){st<u8>(a,4,1);st<u8>(a,0x108,u8(state+1));if(gabi::call<u32>(0x025F795C,p(msg))!=0x12)return 0;gabi::call<void>(0x025F74D0,p(msg),0x13);return 1;}
 else if(state==2){if(gabi::call<u32>(0x025F795C,p(msg))==6){s32 id=ld<s16>(a,0x10A);if(id==0x6A8||id==0x1BBD){u32 game=gabi::call<u32>(0x025200D4);gabi::Local<Vec> v;v->x=ld<f32>(0x1004C590);v->y=ld<f32>(0x1004C7C8);v->z=ld<f32>(0x1004C590);gabi::call<void>(0x025CB374,p(game+0x599C),7,-33,v.get());}st<u8>(a,0x108,u8(ld<u8>(a,0x108)+1));}}
 else if(state==3){if(gabi::call<u32>(0x025F795C,p(msg))==0xE){s32 id=ld<s16>(a,0x10A);if(id==0x1BBD||(u32(id)>=0x1BC0&&u32(id)<=0x1BC2)){st<s16>(a,0x10A,s16(id+1));gabi::call<void>(0x025F74D0,p(msg),0xF);gabi::call<void>(0x025F7DB0,p(msg),ld<s16>(a,0x10A),0);}else{st<u8>(a,0x108,u8(ld<u8>(a,0x108)+1));gabi::call<void>(0x025F74D0,p(msg),0x10);}}}
 if(!ld<u8>(a,4))return 0;if(gabi::call<u32>(0x025F795C,p(msg))!=0x12)return 0;gabi::call<void>(0x025F74D0,p(msg),0x13);return 1;
}
VERIFY(0x0252BEDC,door_msg_proc);
void* door_hk_ctor(void* self){WWHD_FUNC(0x0252C0BC,void*,self);u32 a=gabi::ea(self);if(!a){a=gabi::call<u32>(0x0273AD10,20);if(!a)return nullptr;}gabi::call<void>(0x028F521C,p(a),8);st<u8>(a,0x11,0);st<u32>(a,8,0);st<u8>(a,0x10,0);st<u32>(a,0xC,0);return p(a);}
VERIFY(0x0252C0BC,door_hk_ctor);
s32 door_hk_load(void* self){WWHD_FUNC(0x0252C11C,s32,self);if(!ld<u8>(gabi::ea(self),0x11))return 4;return gabi::call<s32>(0x02520460,self,p(0x1004C858));}
VERIFY(0x0252C11C,door_hk_load);
void door_hk_delete(void* self){WWHD_FUNC(0x0252C13C,void,self);if(ld<u8>(gabi::ea(self),0x11))gabi::call<void>(0x025204C8,self,p(0x1004C860));}
VERIFY(0x0252C13C,door_hk_delete);
BOOL door_hk_create(void* self){WWHD_FUNC(0x0252C154,BOOL,self);u32 a=gabi::ea(self);if(!ld<u8>(a,0x11))return 1;gabi::Local<Name> n;n->data=0x1004C868;n->vt=0x1004C524;u32 data=gabi::call<u32>(0x026066C4,p(ld<u32>(0x101F4F28)),n.get(),4);if(!data)gabi::call<void>(0x0273AA24,p(0x1004C870),0x4A7,p(0x1004C87C));u32 model=gabi::call<u32>(0x025E38E0,p(data),0x80000,0x11000202);st<u32>(a,8,model);if(!model)return 0;u32 anm=gabi::call<u32>(0x025E80D0,0);st<u32>(a,0xC,anm);if(!anm)return 0;gabi::Local<Name> m;m->data=0x1004C868;m->vt=0x1004C524;u32 res=gabi::call<u32>(0x026066C4,p(ld<u32>(0x101F4F28)),m.get(),8);return gabi::call<u32>(0x025E8154,p(ld<u32>(a,0xC)),p(data),p(res),1,2,0,-1,0,0,ld<f32>(0x1004C7C8))!=0;}
VERIFY(0x0252C154,door_hk_create);
void door_hk_init(void* self){WWHD_FUNC(0x0252C274,void,self);st<u8>(gabi::ea(self),0x10,0);}
VERIFY(0x0252C274,door_hk_init);
void door_hk_matrix(void* self,void* door,f32 height){WWHD_FUNC(0x0252C280,void,self,door,height);u32 a=gabi::ea(self),b=gabi::ea(door),matrix=0x1048D0CC;if(!ld<u8>(a,0x11)||!ld<u32>(a,8))return;gabi::call<void>(0x028E93CC,p(matrix),ld<f32>(b,0x314),ld<f32>(b,0x318),ld<f32>(b,0x31C));gabi::call<void>(0x025F1C28,p(matrix),ld<s16>(b,0x322));f32 zero=ld<f32>(0x1004C590);gabi::call<void>(0x025F24E0,zero,height,zero);f32 values[12];for(u32 i=0;i<12;++i)values[i]=ld<f32>(matrix,i*4);u32 model=ld<u32>(a,8);for(u32 i=0;i<12;++i)st<f32>(model,0xC8+i*4,values[i]);}
VERIFY(0x0252C280,door_hk_matrix);
BOOL door_hk_first(void* self){WWHD_FUNC(0x0252C390,BOOL,self);u32 a=gabi::ea(self);if(ld<u8>(a,0x11)!=1||ld<u8>(a,0x10)!=1)return 0;return !gabi::call<u32>(0x025B8B94,p(ld<u32>(0x101F84DC)+0x644),0x2602);}
VERIFY(0x0252C390,door_hk_first);
void door_hk_draw(void* self,void* door){WWHD_FUNC(0x0252C3F8,void,self,door);u32 a=gabi::ea(self),b=gabi::ea(door);if(!ld<u8>(a,0x11)||!ld<u8>(a,0x10)||!ld<u32>(a,8))return;if(gabi::call<u32>(0x0252C390,self))return;u32 data=ld<u32>(ld<u32>(a,8),0xAC),env=gabi::call<u32>(0x02555D0C);gabi::call<void>(0x02562F5C,p(env),p(ld<u32>(a,8)),p(b+0x110));u32 anm=ld<u32>(a,0xC);gabi::call<void>(0x025E83FC,p(anm),p(data),ld<f32>(anm,4));gabi::call<void>(0x025E2DE0,p(ld<u32>(a,8)),0);}
VERIFY(0x0252C3F8,door_hk_draw);
void door_hk_set(void* self,u32 state){WWHD_FUNC(0x0252C49C,void,self,state);u32 a=gabi::ea(self);if(ld<u8>(a,0x10)==state)return;st<u8>(a,0x10,u8(state));if(!state)return;u32 data=ld<u32>(ld<u32>(a,8),0xAC);gabi::Local<Name> n;n->data=0x1004C890;n->vt=0x1004C524;u32 res=gabi::call<u32>(0x026066C4,p(ld<u32>(0x101F4F28)),n.get(),state==1?7:state==2?8:9);gabi::call<void>(0x025E8154,p(ld<u32>(a,0xC)),p(data),p(res),1,2,0,-1,1,0,ld<f32>(0x1004C7C8));}
VERIFY(0x0252C49C,door_hk_set);
void door_hk_proc(void* self,void* door){WWHD_FUNC(0x0252C574,void,self,door);u32 a=gabi::ea(self),b=gabi::ea(door);u8 mode=ld<u8>(a,0x11);if(!mode||!ld<u32>(a,8))return;u32 state=0;if(mode==1){if(gabi::call<u32>(0x025B8B94,p(ld<u32>(0x101F84DC)+0x1178),0x108))state=1;else if(gabi::call<u32>(0x025B8B94,p(ld<u32>(0x101F84DC)+0x1178),0x110))state=2;}else if(mode==4){if(!gabi::call<u32>(0x025B8B94,p(ld<u32>(0x101F84DC)+0x644),0x1710))state=3;}else if(mode==3){if(gabi::call<u32>(0x025B7B10,p(ld<u32>(0x101F84DC)+0xD4),2)&&!gabi::call<u32>(0x025B8B94,p(ld<u32>(0x101F84DC)+0x644),0x1704))state=3;}else if(mode==2){if(gabi::call<u32>(0x025B8B94,p(ld<u32>(0x101F84DC)+0x644),0x1704)&&!gabi::call<u32>(0x025B8B94,p(ld<u32>(0x101F84DC)+0x644),0x1B01))state=3;}gabi::call<void>(0x0252C49C,self,state);if(!ld<u8>(a,0x10))return;gabi::call<void>(0x025E742C,p(ld<u32>(a,0xC)));if(b&&b+0x37C){u32 reverb=gabi::call<u32>(0x02520540,ld<s8>(b,0x326));gabi::call<void>(0x025E1A40,0x61D4,p(b+0x37C),0,reverb);}}
VERIFY(0x0252C574,door_hk_proc);
void door_hk_on_first(void* self){WWHD_FUNC(0x0252C748,void,self);u8 s=ld<u8>(gabi::ea(self),0x10);if(s==1||s==2)gabi::call<void>(0x025B8B68,p(ld<u32>(0x101F84DC)+0x644),s==1?0x2602:0x2601);}
VERIFY(0x0252C748,door_hk_on_first);
u32 door_player_flag(){WWHD_FUNC(0x0252C788,u32);u32 game=gabi::call<u32>(0x025200D4);return(ld<u32>(ld<u32>(game,0x5B2C),0x3C0)>>5)&1;}
VERIFY(0x0252C788,door_player_flag);
void door_static_init(){WWHD_FUNC(0x0252C7B4,void);st<u32>(0x10475710,8,0);st<u32>(0x10475710,0,0);st<u32>(0x10475710,12,0);st<u32>(0x10475710,4,0);gabi::call<void>(0x028F026C,p(0x101D60E8));st<f32>(0x10475704,0,ld<f32>(0x1004C898));st<f32>(0x10475708,0,ld<f32>(0x1004C89C));gabi::call<void>(0x028ED6F8,p(0x1047570C));gabi::call<void>(0x028F026C,p(0x101D60F4));gabi::call<void>(0x028EAB2C,p(0x1047570D));gabi::call<void>(0x028F026C,p(0x101D6100));}
VERIFY(0x0252C7B4,door_static_init);
void door_simple_dtor(void* self,u32 flags){WWHD_FUNC(0x0252C848,void,self,flags);if(self&&(flags&1))gabi::call<void>(0x0273AF40,self);}
VERIFY(0x0252C848,door_simple_dtor);
void door_actor_dtor(void* self,u32 flags){WWHD_FUNC(0x0252C85C,void,self,flags);if(!self)return;gabi::call<void>(0x025D50BC,self,0);if(flags&1)gabi::call<void>(0x0273AF40,self);}
VERIFY(0x0252C85C,door_actor_dtor);
void door_string_assure(void* self){WWHD_FUNC(0x0252C8B0,void,self);}
VERIFY(0x0252C8B0,door_string_assure);
void* door_archive_open(void* archive,void* name){
 WWHD_FUNC(0x0252C8B4,void*,archive,name);u32 owner=gabi::ea(archive),s=gabi::ea(name),a=gabi::call<u32>(0x0273AD10,64);if(a){u32 base=a;if(!base)base=gabi::call<u32>(0x0273AD10,8);if(base){st<u32>(base,4,0);st<u32>(base,0,0);}u32 buffer=a+0x24;if(!buffer)buffer=gabi::call<u32>(0x0273AD10,28);if(buffer){u32 prefix=buffer;if(!prefix)prefix=gabi::call<u32>(0x0273AD10,12);if(prefix){st<u32>(prefix,0,buffer+12);st<u32>(prefix,4,0x1004C8B8);st<u32>(prefix,8,16);st<u8>(buffer,0x1B,0);}u32 data=ld<u32>(buffer);st<u32>(buffer,4,0x1004C948);st<u8>(data,0,0);st<u32>(buffer,4,0x1004C960);}gabi::call<void>(0x027F0A9C,p(a));u32 vt=ld<u32>(s,4),target=ld<u32>(vt,0x14),dest=ld<u32>(a,0x24);gabi::call_ptr<void>(target,name);u32 data=ld<u32>(s);s32 count=0;if(ld<u8>(data)){do{++count;++data;if(count>0x40000){count=0;break;}}while(ld<u8>(data));}s32 capacity=ld<s32>(a,0x2C);vt=ld<u32>(s,4);target=ld<u32>(vt,0x14);if(count>=capacity)count=capacity-1;gabi::call_ptr<void>(target,name);gabi::call<void>(0xC0009988,p(dest),p(ld<u32>(s)),count,0);st<u8>(dest,u32(count),0);}if(!a)return nullptr;if(gabi::call<u32>(0x027F0B04,p(a),p(owner))){gabi::call<void>(0x027F0A0C,p(a),3);return nullptr;}return p(a);
}
VERIFY(0x0252C8B4,door_archive_open);
