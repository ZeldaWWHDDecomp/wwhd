#include "wwhd.h"
#include "gabi.h"
using namespace gabi;
namespace d_magma {
struct Vector {be<f32> x,y,z;};
WWHD_SIZE(Vector,12);
s32 range_check(void* self,void* position,void* out){
 WWHD_FUNC(0x02589280,s32,self,position,out);
 u32 s=ea(self),p=ea(position);Local<Vector> a,b;
 a->x=load<f32>(s+0xD50);a->y=load<f32>(0x10050808);a->z=load<f32>(s+0xD58);
 b->x=load<f32>(p);b->y=load<f32>(0x10050808);b->z=load<f32>(p+8);
 f32 d=call<f32>(0x028E8DE8,a.get(),b.get());
 f32 scale=load<f32>(s+0xD5C),r=load<f32>(0x1005080C)*scale;
 if(!(d<r*r))return 0;
 f32 radius=load<f32>(0x10050810)*scale;
 f32 base=load<f32>(s+0xD54)-(radius-load<f32>(0x10050814));
 f32 distance=call<f32>(0x028F4384,fmsubs(radius,radius,d));
 store<f32>(ea(out),base+distance);return 1;
}
VERIFY(0x02589280,range_check);
void path_calc(void* self,f32 y,u32 path,s32 room){
 WWHD_FUNC(0x02589E98,void,self,y,path,room);
 u32 s=ea(self);
 if(load<s16>(s+0xD64)<0){
  call_ptr<void>(load<u32>(load<u32>(s+0xDCC)+0x24),self,path,room,y);
  store<s16>(s+0xD64,0);store<u8>(s+0xDD0,0);
 }
 if(call<s32>(0x0207A9A0,at<void>(s+0xDD0))==0){
  u16 wave=u16(load<s16>(s+0xD64)+200);store<u16>(s+0xD64,wave);
  f32 sine=load<f32>(0x104A44F8+u32(wave>>3)*8);
  store<f32>(s+0xD54,fmadds(load<f32>(0x10050864),sine-load<f32>(0x10050860),load<f32>(s+0xD60)));
 }
}
VERIFY(0x02589E98,path_calc);
void path_update(void* self){
 WWHD_FUNC(0x02589F40,void,self);u32 s=ea(self);
 store<u32>(s+0xDB4,load<u32>(s+0xD54));
 call<void>(0x028E9108,at<void>(0x104B45F8),at<void>(s+0xD98),at<void>(s+0xD68));
 call<void>(0x02589D30,self);
}
VERIFY(0x02589F40,path_update);
void room_new_floor(void* self,void* floor){
 WWHD_FUNC(0x0258C474,void,self,floor);u32 s=ea(self),f=ea(floor),head=load<u32>(s);
 store<u32>(f+0xBE0,s);store<u32>(f+0xBDC,head);store<u32>(s,f);
}
VERIFY(0x0258C474,room_new_floor);
void room_delete(void* self){
 WWHD_FUNC(0x0258C488,void,self);u32 p=load<u32>(ea(self));store<u32>(ea(self),0);
 while(p){u32 next=load<u32>(p+0xBDC);store<u32>(p+0xBDC,0);store<u32>(p+0xBE0,0);store<u8>(p+0xB36,2);p=next;}
}
VERIFY(0x0258C488,room_delete);
void delete_room(void* self,u32 room){WWHD_FUNC(0x0258CE1C,void,self,room);call<void>(0x0258C488,at<void>(ea(self)+0x5FB8+room*4));}
VERIFY(0x0258CE1C,delete_room);
void empty_dtor(void* self,u32 flag){WWHD_FUNC(0x0258CEDC,void,self,flag);if(self&&(flag&1))call<void>(0x0273AF40,self);}
VERIFY(0x0258CEDC,empty_dtor);
void* small_ctor(void* self){WWHD_FUNC(0x0258D790,void*,self);return self?self:call<void*>(0x0273AD10,u32(16));}
VERIFY(0x0258D790,small_ctor);
void embedded_dtor(void* self,u32 flag){
 WWHD_FUNC(0x0258D7BC,void,self,flag);if(!self)return;
 call<void>(0x027BF880,at<void>(ea(self)+0x158),u32(2));
 call<void>(0x027B5CBC,at<void>(ea(self)+4),u32(2));
 if(flag&1)call<void>(0x0273AF40,self);
}
VERIFY(0x0258D7BC,embedded_dtor);
void* room_ctor(void* self){WWHD_FUNC(0x0258D81C,void*,self);if(!self)self=call<void*>(0x0273AD10,u32(4));if(self)store<u32>(ea(self),0);return self;}
VERIFY(0x0258D81C,room_ctor);
void room_dtor(void* self,u32 flag){
 WWHD_FUNC(0x0258D858,void,self,flag);if(!self)return;
 if(load<u32>(ea(self)))call<void>(0x0273AA24,at<void>(0x100507EC),s32(231),at<void>(0x100507F8));
 if(flag&1)call<void>(0x0273AF40,self);
}
VERIFY(0x0258D858,room_dtor);
void static_init(){WWHD_FUNC(0x0258D8C4,void);}
VERIFY(0x0258D8C4,static_init);

void matrix_copy(void* out,void* in){
 WWHD_FUNC(0x02589B30,void,out,in);u32 words[12];
 for(u32 i=0;i<12;i++)words[i]=load<u32>(ea(in)+i*4);
 for(u32 i=0;i<12;i++)store<u32>(ea(out)+i*4,words[i]);
}
VERIFY(0x02589B30,matrix_copy);
f32 color_channel(u32 value){
 WWHD_FUNC(0x02589BD0,f32,value);
 f32 x=f32(value)/load<f32>(0x10050858);
 x=call<f32>(0x028F4560,x,load<f32>(0x1005085C));
 if(x<load<f32>(0x10050808))return load<f32>(0x10050808);
 if(x>load<f32>(0x10050860))return load<f32>(0x10050860);
 return x;
}
VERIFY(0x02589BD0,color_channel);
void color_convert(void* out,void* color){
 WWHD_FUNC(0x02589C50,void,out,color);u32 c=ea(color),o=ea(out);
 f32 r=call<f32>(0x02589BD0,u32(load<u8>(c)));
 f32 g=call<f32>(0x02589BD0,u32(load<u8>(c+1)));
 f32 b=call<f32>(0x02589BD0,u32(load<u8>(c+2)));
 f32 a=f32(load<u8>(c+3))/load<f32>(0x10050858);
 store<f32>(o,r);store<f32>(o+4,g);store<f32>(o+8,b);store<f32>(o+12,a);
}
VERIFY(0x02589C50,color_convert);
void packet_release_floors(void* self,s32 force){
 WWHD_FUNC(0x0258C5D0,void,self,force);u32 p=ea(self)+0x98;
 for(u32 i=0;i<8;i++,p+=0xBE4){
  if(!load<u32>(p+0xB30)||load<u32>(p+0xBDC)||load<u32>(p+0xBE0))continue;
  if(force||!load<u8>(p+0xB36))call<void>(0x0258C170,at<void>(p));
  else store<u8>(p+0xB36,u8(load<u8>(p+0xB36)-1));
 }
}
VERIFY(0x0258C5D0,packet_release_floors);
void packet_update(void* self){
 WWHD_FUNC(0x0258CA60,void,self);u32 p=ea(self)+0x98;
 for(u32 i=0;i<8;i++,p+=0xBE4){
  if(load<u32>(p+0xB30)&&(load<u32>(p+0xBDC)||load<u32>(p+0xBE0)))call<void>(0x0258B068,at<void>(p));
 }
 call<void>(0x027F0E04,at<void>(load<u32>(0x104B4634)),self,u32(0));
}
VERIFY(0x0258CA60,packet_update);
void module_init(){
 WWHD_FUNC(0x0258CE2C,void);
 store<u32>(0x104776DC,0);store<u32>(0x104776D4,0);store<u32>(0x104776E0,0);store<u32>(0x104776D8,0);
 call<void>(0x028F026C,at<void>(0x101E9F0C));
 f32 x=load<f32>(0x100509C0),y=load<f32>(0x100509C4);
 store<f32>(0x10477668,x);store<f32>(0x1047766C,y);
 call<void>(0x028ED6F8,at<void>(0x10477670));call<void>(0x028F026C,at<void>(0x101E9F18));
 call<void>(0x028EAB2C,at<void>(0x10477671));call<void>(0x028F026C,at<void>(0x101E9F24));
 store<u32>(0x10477664,load<u32>(0x101E9C4C)*load<u32>(0x101E9C50));
}
VERIFY(0x0258CE2C,module_init);

void floor_update(void* self){
 WWHD_FUNC(0x0258B068,void,self);u32 s=ea(self);
 if(load<u32>(s+0xB30)&&!load<u32>(s+0xBDC)&&!load<u32>(s+0xBE0))return;
 call<void>(0x028E9108,at<void>(0x104B45F8),at<void>(s+0xB7C),at<void>(s+0xB4C));
 call<void>(0x0258AF68,self);
 u32 list=load<u32>(s+0xB30);
 for(u32 i=0;i<load<u8>(s+0xB34);i++){
  u32 ball=load<u32>(list+i*4);
  call_ptr<void>(load<u32>(load<u32>(ball+0xDCC)+0x1C),at<void>(ball));
 }
}
VERIFY(0x0258B068,floor_update);
void floor_prepare(void* self){
 WWHD_FUNC(0x0258AF68,void,self);u32 s=ea(self);Local<u32[12]> view,inverse,work;
 call<void>(0x0255F84C,self);
 cpu->r[6]=0x104B45C0;
 call<void>(0x02589B30,view.get(),at<void>(0x104B45F8));
 // Matrix-copy helper does not modify r6; retain the original shared graphics base.
 u32 base=u32(cpu->r[6]);
 call<void>(0x027FDA54,at<void>(s+0x4EC),u32(0),view.get(),at<void>(base+0x14C),at<void>(load<u32>(base+0x148)+0x240));
 call<void>(0x027FDFF4,at<void>(s+0x4EC),u32(0));
 call<void>(0x02589C50,at<void>(s+0x664),at<void>(0x101E9C48));
 call<void>(0x028E91EC,view.get(),inverse.get());
 f32 one=load<f32>(0x10050860);
 call<void>(0x028E945C,at<void>(s+0x784),one,one,one);
 call<void>(0x02589B30,work.get(),at<void>(0x104776E4));
 call<void>(0x028E9108,at<void>(s+0x784),work.get(),at<void>(s+0x784));
 call<void>(0x028E9108,at<void>(s+0x784),inverse.get(),at<void>(s+0x784));
 call<void>(0x027FB678,at<void>(s+0x5A0));
 call<void>(0x02589B30,work.get(),at<void>(s+0xB7C));
 call<void>(0x028E90D4,work.get(),at<void>(s+0x56C));
 call<void>(0x027FB678,at<void>(s+0x4F8));
}
VERIFY(0x0258AF68,floor_prepare);

void path_setup(void* self,f32 y,u32 path,s32 room){
 WWHD_FUNC(0x02589F8C,void,self,y,path,room);u32 s=ea(self);
 u32 p=ea(call<void*>(0x025AAF88,path,room));
 if(!p)call<void>(0x0273AA24,at<void>(0x1005087C),s32(750),at<void>(0x10050888));
 u32 points=load<u32>(p+8);
 if(!points){call<void>(0x0273AA24,at<void>(0x1005087C),s32(751),at<void>(0x10050894));points=load<u32>(p+8);}
 f32 choice=call<f32>(0x020198D8,f32(s32(load<u16>(p))-1));
 s32 index=gabi::ftoi(choice);u32 pt=points+u32(index)*16;
 f32 amplitude=load<f32>(0x10050864);
 f32 dx=call<f32>(0x02019918,f32(load<u8>(pt+3))*amplitude);
 store<f32>(s+0xD50,load<f32>(pt+4)+dx);
 f32 dz=call<f32>(0x02019918,f32(load<u8>(pt+3))*amplitude);
 f32 one=load<f32>(0x10050860);
 store<f32>(s+0xD58,load<f32>(pt+12)+dz);
 f32 scale=call<f32>(0x020198D8,one)+one;store<f32>(s+0xD5C,scale);
 f32 base=y-call<f32>(0x020198D8,load<f32>(0x10050870));store<f32>(s+0xD60,base);
 f32 wave_random=call<f32>(0x020198D8,load<f32>(0x10050874));
 u16 wave=u16(gabi::ftoi(load<f32>(0x10050878)*wave_random));
 store<u16>(s+0xD64,wave);store<u8>(s+0xDD0,0);
 f32 sine=load<f32>(0x104A44F8+u32(wave>>3)*8);
 f32 height=fmadds(amplitude,sine-one,load<f32>(s+0xD60));store<f32>(s+0xD54,height);
 call<void>(0x028E93CC,at<void>(0x1048D0CC),load<f32>(s+0xD50),height,load<f32>(s+0xD58));
 scale=load<f32>(s+0xD5C);call<void>(0x025F2518,scale,one,scale);
 call<void>(0x028E90D4,at<void>(0x1048D0CC),at<void>(s+0xD98));
}
VERIFY(0x02589F8C,path_setup);

void packet_draw(void* self,void* context){
 WWHD_FUNC(0x0258C72C,void,self,context);u32 s=ea(self);
 if(!load<u32>(ea(context)+12))return;
 u8 color[4];for(u32 i=0;i<4;i++)color[i]=load<u8>(s+0x60B8+i);
 for(u32 i=0;i<4;i++)store<u8>(0x101E9C48+i,color[i]);
 for(u32 i=0,p=s+0x98;i<8;i++,p+=0xBE4)
  if(load<u32>(p+0xB30)&&(load<u32>(p+0xBDC)||load<u32>(p+0xBE0)))call<void>(0x0258A880,at<void>(p),context);
}
VERIFY(0x0258C72C,packet_draw);
f32 check_height(void* self,void* position){
 WWHD_FUNC(0x0258CAEC,f32,self,position);u32 s=ea(self),v=ea(position);
 f32 result=load<f32>(0x10050984),width=load<f32>(0x10050988),vertical=load<f32>(0x1005098C);
 Local<be<f32>> hit;
 for(u32 j=0,p=s+0x98;j<8;j++,p+=0xBE4){
  u32 list=load<u32>(p+0xB30);
  if(!list||(!load<u32>(p+0xBDC)&&!load<u32>(p+0xBE0)))continue;
  if(std::fabs(load<f32>(v+4)-load<f32>(p+0xB3C))>vertical)continue;
  if(std::fabs(load<f32>(v)-load<f32>(p+0xB38))>width*load<f32>(p+0xB44))continue;
  if(std::fabs(load<f32>(v+8)-load<f32>(p+0xB40))>width*load<f32>(p+0xB48))continue;
  for(u32 i=0;i<load<u8>(p+0xB34);i++){
   if(call<s32>(0x02589280,at<void>(load<u32>(list+i*4)),position,hit.get())){
    f32 h=*hit.get(),floor=load<f32>(p+0xB3C);if(h<floor){h=floor;*hit.get()=h;}if(h>result)result=h;
   }
  }
 }
 return result;
}
VERIFY(0x0258CAEC,check_height);
void* packet_ctor(void* self){
 WWHD_FUNC(0x0258C4C0,void*,self);
 if(!self)self=call<void*>(0x0273AD10,u32(0x60C0));if(!self)return self;u32 s=ea(self);
 call<void>(0x027F1278,self);store<u32>(s+12,0x100509F8);
 call<void>(0x028EFFD0,at<void>(s+0x98),u32(8),u32(0xBE4),u32(0x0258A1C4));
 call<void>(0x028EFFD0,at<void>(s+0x5FB8),u32(64),u32(4),u32(0x0258D81C));
 call<void>(0x02520354,at<void>(0x1005095C),u32(0),u32(0));
 f32 one=load<f32>(0x10050860),span=load<f32>(0x10050954),zero=load<f32>(0x100508EC),far=load<f32>(0x10050958);
 call<void>(0x028E97BC,at<void>(0x10477674),one,span,span,one,zero,far,zero,zero);
 call<void>(0x028E90D4,at<void>(0x10477674),at<void>(0x104776A4));
 call<void>(0x028E9098,at<void>(0x10477714));call<void>(0x028E9098,at<void>(0x10477744));
 store<f32>(s+0x60BC,load<f32>(0x10050808));return self;
}
VERIFY(0x0258C4C0,packet_ctor);
void packet_dtor(void* self,u32 flag){
 WWHD_FUNC(0x0258C674,void,self,flag);if(!self)return;u32 s=ea(self);store<u32>(s+12,0x100509F8);
 call<void>(0x0258C5D0,self,u32(1));call<void>(0x02520488,at<void>(0x10050964));
 call<void>(0x028F0164,at<void>(s+0x5FB8),u32(64),u32(4),u32(0x0258D858),u32(0),u32(0));
 call<void>(0x028F0164,at<void>(s+0x98),u32(8),u32(0xBE4),u32(0x0258A58C),u32(0),u32(0));
 call<void>(0x027F13DC,self,u32(0));if(flag&1)call<void>(0x0273AF40,self);
}
VERIFY(0x0258C674,packet_dtor);

void resource_dtor(void* self,u32 flag){
 WWHD_FUNC(0x0258CEF0,void,self,flag);if(!self)return;
 call<void>(0x027BF880,at<void>(ea(self)+0x158),u32(2));call<void>(0x027B5CBC,at<void>(ea(self)+4),u32(2));
 if(flag&1)call<void>(0x0273AF40,self);
}
VERIFY(0x0258CEF0,resource_dtor);
static void* resource_construct(void* self){
 if(!self)self=call<void*>(0x0273AD10,u32(0x254));if(!self)return self;u32 s=ea(self);
 call<void>(0x027B5BD8,at<void>(s+4));call<void>(0x027BF734,at<void>(s+0x158));
 store<u32>(s+0x250,0);store<u32>(s+0x24C,0);return self;
}
void* boss_resource_ctor(void* self){WWHD_FUNC(0x0258D314,void*,self);return resource_construct(self);}
VERIFY(0x0258D314,boss_resource_ctor);
void* path_resource_ctor(void* self){WWHD_FUNC(0x0258D734,void*,self);return resource_construct(self);}
VERIFY(0x0258D734,path_resource_ctor);
void* new_floor(void* self,void* position,void* scale,u32 room,s32 path){
 WWHD_FUNC(0x0258CC64,void*,self,position,scale,room,path);u32 s=ea(self);
 if(room>=64)call<void>(0x0273AA24,at<void>(0x10050994),s32(1723),at<void>(0x100509A0));
 f32 multiplier=load<f32>(0x10050990);
 for(u32 i=0,f=s+0x98;i<8;i++,f+=0xBE4){
  if(load<u32>(f+0xB30)||load<u32>(f+0xBDC)||load<u32>(f+0xBE0))continue;
  u32 count=0;
  if(path<0)count=u8(0-u32(path));
  else{
   u32 p=ea(call<void*>(0x025AAF88,u32(path),room));if(!p)return nullptr;
   u32 n=load<u16>(p);if(n){u32 points=load<u32>(p+8);count=u32(ftoi(f32(load<u8>(points+3))*multiplier))*n;}count=u8(count);
  }
  if(!call<void*>(0x0258B830,at<void>(f),position,scale,path,count,room))return nullptr;
  call<void>(0x0258C474,at<void>(s+0x5FB8+room*4),at<void>(f));return at<void>(f);
 }
 return nullptr;
}
VERIFY(0x0258CC64,new_floor);
void ball_prepare(void* self){
 WWHD_FUNC(0x02589D30,void,self);u32 s=ea(self);Local<u32[12]> view,inverse,work,extra;
 call<void>(0x0255F84C,self);cpu->r[6]=0x104B45C0;
 call<void>(0x02589B30,view.get(),at<void>(0x104B45F8));u32 base=cpu->r[6];
 call<void>(0x027FDA54,at<void>(s+0x4E4),u32(0),view.get(),at<void>(base+0x14C),at<void>(load<u32>(base+0x148)+0x240));
 call<void>(0x027FDFF4,at<void>(s+0x4E4),u32(0));
 call<void>(0x02589C50,at<void>(s+0x65C),at<void>(0x101E9C48));call<void>(0x028E91EC,view.get(),inverse.get());
 f32 one=load<f32>(0x10050860);call<void>(0x028E945C,at<void>(s+0x71C),one,one,one);
 call<void>(0x02589B30,work.get(),at<void>(0x104776E4));
 call<void>(0x028E9108,at<void>(s+0x71C),work.get(),at<void>(s+0x71C));
 call<void>(0x028E9108,at<void>(s+0x71C),inverse.get(),at<void>(s+0x71C));
 call<void>(0x028E945C,at<void>(s+0x74C),one,one,one);
 if(u32 matrix=load<u32>(s+0xDC8)){
  call<void>(0x02589B30,extra.get(),at<void>(matrix));
  call<void>(0x028E9108,at<void>(s+0x74C),extra.get(),at<void>(s+0x74C));
 }
 call<void>(0x028E9108,at<void>(s+0x74C),inverse.get(),at<void>(s+0x74C));
 call<void>(0x027FB678,at<void>(s+0x598));call<void>(0x02589B30,work.get(),at<void>(s+0xD98));
 call<void>(0x028E90D4,work.get(),at<void>(s+0x564));call<void>(0x027FB678,at<void>(s+0x4F0));
}
VERIFY(0x02589D30,ball_prepare);

struct StringRef{be<u32> value,vtable;};WWHD_SIZE(StringRef,8);
static void resolve_string(StringRef* ref){
 u32 table=load<u32>(ea(ref)+4);call_ptr<void>(load<u32>(table+0x14),ref);
}
static bool same_string(StringRef* a,StringRef* b){
 u32 first=load<u32>(ea(a)),second=load<u32>(ea(b));if(first==second)return true;
 for(u32 i=0;i<0x40001;i++){
  u8 x=load<u8>(first+i),y=load<u8>(second+i);if(x!=y)return false;if(!x)return true;
 }
 return false;
}
void floor_calc(void* self,s32 room){
 WWHD_FUNC(0x0258ACD4,void,self,room);u32 s=ea(self);f32 one=load<f32>(0x10050860);
 call<void>(0x028E945C,at<void>(0x1048D0CC),one,load<f32>(0x100508F0),one);
 Local<StringRef> a,b,c,d;a->value=0x100508F8;a->vtable=0x100507AC;
 u32 game=ea(call<void*>(0x025200D4));b->value=game+0x5134;b->vtable=0x100507AC;
 resolve_string(a.get());resolve_string(a.get());resolve_string(b.get());
 bool special=same_string(a.get(),b.get());
 if(!special){
  c->value=0x10050900;c->vtable=0x100507AC;
  game=ea(call<void*>(0x025200D4));d->value=game+0x5134;d->vtable=0x100507AC;
  resolve_string(c.get());resolve_string(c.get());resolve_string(d.get());special=same_string(c.get(),d.get());
 }
 f32 zero=load<f32>(0x10050808),offset=load<f32>(special?0x10050870:0x100508F4);
 call<void>(0x025F24E0,zero,-(load<f32>(s+0xB3C)+offset),zero);
 call<void>(0x028E9108,at<void>(0x104776A4),at<void>(0x1048D0CC),at<void>(s+0xBAC));
 u32 list=load<u32>(s+0xB30);
 for(u32 i=0;i<load<u8>(s+0xB34);i++){
  u32 ball=load<u32>(list+i*4);
  call_ptr<void>(load<u32>(load<u32>(ball+0xDCC)+0x14),at<void>(ball),u32(load<u8>(s+0xB35)),room,load<f32>(s+0xB3C));
  store<u32>(load<u32>(list+i*4)+0xDC8,s+0xBAC);
 }
}
VERIFY(0x0258ACD4,floor_calc);

void* floor_ctor(void* self){
 WWHD_FUNC(0x0258A1C4,void*,self);if(!self)self=call<void*>(0x0273AD10,u32(0xBE4));if(!self)return self;u32 s=ea(self);
 store<u32>(s,0);u32 small=s+4;if(!small)small=ea(call<void*>(0x0273AD10,u32(8)));
 if(small){store<u32>(small+4,0);store<u32>(small,0);}
 u32 resources=s+12;if(!resources)resources=ea(call<void*>(0x0273AD10,u32(0x4C0)));
 if(resources){
  call<void>(0x028EFFD0,at<void>(resources),u32(2),u32(0x254),u32(0x0258D734));
  store<u32>(resources+0x4A8,0);store<u32>(resources+0x4AC,0);store<u32>(resources+0x4B8,0);
  store<u32>(resources+0x4B0,32);store<u8>(resources+0x4BC,0);store<u32>(resources,0);store<u32>(resources+0x254,0);
 }
 call<void>(0x027B5430,at<void>(s+0x4CC));call<void>(0x028F521C,at<void>(s+0x4E4),u32(8));
 call<void>(0x027FD6F4,at<void>(s+0x4EC));call<void>(0x027FB40C,at<void>(s+0x4F8));
 store<u32>(s+0x504,0x1016EF84);call<void>(0x028F521C,at<void>(s+0x56C),u32(0x34));
 if(!(s+0x56C))call<void>(0x0273AD10,u32(48));
 call<void>(0x027FB40C,at<void>(s+0x5A0));store<u32>(s+0x5AC,0x1016EFB4);
 call<void>(0x028F521C,at<void>(s+0x614),u32(0x2F0));
 f32 zero=load<f32>(0x10145180),one=load<f32>(0x1014517C);
 // Initial color vectors and four identity matrices at their actual HD offsets.
 for(u32 off: {0x614u,0x618u,0x61Cu,0x624u,0x64Cu,0x62Cu,0x67Cu,0x69Cu,0x66Cu,0x644u,0x668u,0x698u,0x688u,0x658u,0x634u,0x648u,0x638u,0x65Cu,0x628u,0x684u,0x68Cu,0x678u,0x664u,0x694u,0x63Cu,0x674u,0x654u,0x6A4u,0x6A8u,0x6ACu,0x6B4u,0x6B8u,0x6BCu})store<f32>(s+off,zero);
 for(u32 off:{0x620u,0x630u,0x640u,0x650u,0x660u,0x670u,0x680u,0x690u,0x6A0u,0x6B0u,0x6C0u})store<f32>(s+off,one);
 for(u32 off:{0x6C4u,0x6E4u,0x704u})call<void>(0x028EFFD0,at<void>(s+off),u32(2),u32(16),u32(0x0258D790));
 for(u32 off=0x724;off<=0x874;off+=48)if(!(s+off))call<void>(0x0273AD10,u32(48));
 for(u32 off=0x8A4;off<=0x8F4;off+=16)if(!(s+off))call<void>(0x0273AD10,u32(16));
 store<u8>(s+0x904,0);call<void>(0x027BE6B8,at<void>(s+0x908));call<void>(0x027BDF7C,at<void>(s+0x998));
 store<u8>(s+0xB35,0);store<u32>(s+0xB30,0);store<u8>(s+0xB34,0);store<u8>(s+0xB36,0);
 f32 z=load<f32>(0x10050808);store<f32>(s+0xB44,z);store<f32>(s+0xB48,z);
 for(u32 off:{0xB4Cu,0xB7Cu,0xBACu})call<void>(0x028F521C,at<void>(s+off),u32(48));
 store<u32>(s+0xBE0,0);store<u32>(s+0xBDC,0);return self;
}
VERIFY(0x0258A1C4,floor_ctor);

static void heap_free_field(u32 field){
 u32 heap=ea(call<void*>(0x02755FEC,at<void>(load<u32>(0x101F8B4C)),at<void>(load<u32>(field))));
 call_ptr<void>(load<u32>(load<u32>(heap+12)+0x3C),at<void>(heap),at<void>(load<u32>(field)));
}
static void clear_graphics_array(u32 object,u32 count_offset,u32 pointer_offset){
 if(!load<u32>(object+pointer_offset))return;
 for(s32 i=0;i<load<s32>(object+count_offset);i++){
  u32 item=load<u32>(object+pointer_offset)+u32(i)*0xF4;
  call_ptr<void>(load<u32>(load<u32>(item+0xF0)+12),at<void>(item),u32(2));
 }
 heap_free_field(object+pointer_offset);store<u32>(object+count_offset,0);store<u32>(object+pointer_offset,0);
}
void floor_dtor(void* self,u32 flag){
 WWHD_FUNC(0x0258A58C,void,self,flag);if(!self)return;u32 s=ea(self);
 if(load<u32>(s+0xB30))call<void>(0x0273AA24,at<void>(0x100508A8),s32(812),at<void>(0x100508B4));
 if(load<u32>(s+0xBDC))call<void>(0x0273AA24,at<void>(0x100508A8),s32(813),at<void>(0x100508C4));
 if(load<u32>(s+0xBE0))call<void>(0x0273AA24,at<void>(0x100508A8),s32(814),at<void>(0x100508D4));
 call<void>(0x027BE2B0,at<void>(s+0x998),u32(2));call<void>(0x027FB528,at<void>(s+0x5A0),u32(0));
 call<void>(0x027FB528,at<void>(s+0x4F8),u32(0));call<void>(0x027FD764,at<void>(s+0x4EC),u32(2));call<void>(0x027B54A0,at<void>(s+0x4CC),u32(2));
 if(s+12){
  call<void>(0x027BF7E8,at<void>(s+0x164));store<u32>(s+12,0);
  if(load<u32>(s+0x25C)){heap_free_field(s+0x25C);store<u32>(s+0x258,0);store<u32>(s+0x25C,0);}
  call<void>(0x027BF7E8,at<void>(s+0x3B8));store<u32>(s+0x260,0);
  if(load<u32>(s+0x4B0)){heap_free_field(s+0x4B0);store<u32>(s+0x4AC,0);store<u32>(s+0x4B0,0);}
  store<u32>(s+0x4C4,0);call<void>(0x028F0164,at<void>(s+12),u32(2),u32(0x254),u32(0x0258D7BC),u32(0),u32(0));
 }
 if(load<u32>(s+8)){
  for(s32 i=0;i<load<s32>(s+4);i++){
   u32 item=load<u32>(s+8)+u32(i)*20;if(!item)continue;store<u32>(item,0);
   clear_graphics_array(item,4,8);clear_graphics_array(item,12,16);
  }
  heap_free_field(s+8);store<u32>(s+4,0);store<u32>(s+8,0);
 }
 if(flag&1)call<void>(0x0273AF40,self);
}
VERIFY(0x0258A58C,floor_dtor);

static void resource_buffers_cleanup(u32 resource){
 call<void>(0x027BF7E8,at<void>(resource+0x158));store<u32>(resource,0);
 if(load<u32>(resource+0x250)){heap_free_field(resource+0x250);store<u32>(resource+0x24C,0);store<u32>(resource+0x250,0);}
 call<void>(0x027BF7E8,at<void>(resource+0x3AC));store<u32>(resource+0x254,0);
 if(load<u32>(resource+0x4A4)){heap_free_field(resource+0x4A4);store<u32>(resource+0x4A0,0);store<u32>(resource+0x4A4,0);}
 store<u32>(resource+0x4B8,0);
}
static void ball_cleanup(void* self,u32 flag){
 if(!self)return;u32 s=ea(self),resource=s+12;store<u32>(s+0xDCC,0x100507C4);
 resource_buffers_cleanup(resource);
 for(s32 i=0;i<load<s32>(s+0x4E4);i++){
  u32 data=load<u32>(s+0x4E8);if(u32(i)<load<u32>(s+0x4E4))data+=u32(i)*0x23C;
  call<void>(0x027BEBEC,at<void>(data+16));call<void>(0x027BEBEC,at<void>(data+44));
 }
 for(u32 off:{0x500u,0x51Cu,0x5A8u,0x5C4u})call<void>(0x027BEBEC,at<void>(s+off));
 call<void>(0x027BE2B0,at<void>(s+0xBB8),u32(2));call<void>(0x027BE2B0,at<void>(s+0xA20),u32(2));
 call<void>(0x027FB528,at<void>(s+0x598),u32(0));call<void>(0x027FB528,at<void>(s+0x4F0),u32(0));
 call<void>(0x027FD764,at<void>(s+0x4E4),u32(2));call<void>(0x027B54A0,at<void>(s+0x4CC),u32(2));
 if(resource){resource_buffers_cleanup(resource);call<void>(0x028F0164,at<void>(resource),u32(2),u32(0x254),u32(0x0258CEF0),u32(0),u32(0));}
 if(load<u32>(s+8)){
  for(s32 i=0;i<load<s32>(s+4);i++){
   u32 item=load<u32>(s+8)+u32(i)*20;if(!item)continue;store<u32>(item,0);
   clear_graphics_array(item,4,8);clear_graphics_array(item,12,16);
  }
  heap_free_field(s+8);store<u32>(s+4,0);store<u32>(s+8,0);
 }
 if(flag&1)call<void>(0x0273AF40,self);
}
void boss_dtor(void* self,u32 flag){WWHD_FUNC(0x0258CF50,void,self,flag);ball_cleanup(self,flag);}
VERIFY(0x0258CF50,boss_dtor);
void path_dtor(void* self,u32 flag){WWHD_FUNC(0x0258D370,void,self,flag);ball_cleanup(self,flag);}
VERIFY(0x0258D370,path_dtor);

void packet_calc(void* self){
 WWHD_FUNC(0x0258C7C8,void,self);u32 s=ea(self);f32 one=load<f32>(0x10050860);
 f32 t=load<f32>(0x10477680)+load<f32>(0x1005096C);
 if(!(t<load<f32>(0x10050970)))t-=one;
 store<f32>(0x10477680,t);store<f32>(0x10477690,t);
 call<void>(0x028E945C,at<void>(0x1048D0CC),load<f32>(0x10050974),load<f32>(0x10050978),load<f32>(0x1005097C));
 call<void>(0x025F1BF4,at<void>(0x1048D0CC),u32(0x4000));
 call<void>(0x028E9108,at<void>(0x10477674),at<void>(0x1048D0CC),at<void>(0x104776E4));call<void>(0x0258C5D0,self,u32(0));
 for(u32 room=0;room<64;room++){
  u32 floor=load<u32>(s+0x5FB8+room*4);
  while(floor){
   u32 next=load<u32>(floor+0xBDC);
   if(!load<u32>(floor+0xB30))next=load<u32>(floor+0xBDC);
   else if(next||load<u32>(floor+0xBE0)){call<void>(0x0258ACD4,at<void>(floor),room);next=load<u32>(floor+0xBDC);}
   floor=next;
  }
 }
 f32 frame=load<f32>(s+0x60BC)+one,period=load<f32>(0x10050980);
 if(!(frame<period))frame-=period;store<f32>(s+0x60BC,frame);
 u32 color=0x101E9F04;f32 end=0;
 for(u32 i=0;i<2;i++){end=f32(load<u8>(color+3));if(frame<end)break;color+=4;}
 f32 start=f32(load<u8>(color-1));f32 fraction=(frame-start)/(end-start);
 f32 r=call<f32>(0x02588DEC,f32(load<u8>(color-4)),f32(load<u8>(color)),fraction);store<u8>(s+0x60B8,u8(ftoi(r)));
 f32 g=call<f32>(0x02588DEC,f32(load<u8>(color-3)),f32(load<u8>(color+1)),f32(cpu->f[3].ps0));store<u8>(s+0x60B9,u8(ftoi(g)));
 f32 b=call<f32>(0x02588DEC,f32(load<u8>(color-2)),f32(load<u8>(color+2)),f32(cpu->f[3].ps0));store<u8>(s+0x60BA,u8(ftoi(b)));
}
VERIFY(0x0258C7C8,packet_calc);

void floor_remove(void* self){
 WWHD_FUNC(0x0258C170,void,self);u32 s=ea(self),list=load<u32>(s+0xB30);
 if(!list)return;
 if(list){
  for(u32 i=0;i<load<u8>(s+0xB34);i++){
   u32 ball=load<u32>(list+i*4);if(ball)call_ptr<void>(load<u32>(load<u32>(ball+0xDCC)+12),at<void>(ball),u32(3));
  }
  // Original retains the loaded table through the loop, then reloads after array deletion.
  list=load<u32>(s+0xB30);call<void>(0x0273AFC8,at<void>(list));call<void>(0x025F0148,at<void>(load<u32>(s+0xB30)));store<u32>(s+0xB30,0);
 }
 resource_buffers_cleanup(s+12);
 for(s32 i=0;i<load<s32>(s+0x4EC);i++){
  u32 data=load<u32>(s+0x4F0);if(u32(i)<load<u32>(s+0x4EC))data+=u32(i)*0x23C;
  call<void>(0x027BEBEC,at<void>(data+16));call<void>(0x027BEBEC,at<void>(data+44));
 }
 for(u32 off:{0x508u,0x524u,0x5B0u,0x5CCu})call<void>(0x027BEBEC,at<void>(s+off));
 if(load<u32>(s+8)){
  for(s32 i=0;i<load<s32>(s+4);i++){
   u32 item=load<u32>(s+8)+u32(i)*20;if(!item)continue;store<u32>(item,0);
   clear_graphics_array(item,4,8);clear_graphics_array(item,12,16);
  }
  heap_free_field(s+8);store<u32>(s+4,0);store<u32>(s+8,0);
 }
}
VERIFY(0x0258C170,floor_remove);

static u32 heap_allocate(u32 size,u32 alignment){
 u32 heap=ea(call<void*>(0x02756140,at<void>(load<u32>(0x101F8B4C))));
 return ea(call_ptr<void*>(load<u32>(load<u32>(heap+12)+0x34),at<void>(heap),size,alignment));
}
static bool texture_equal(u32 a,u32 b){
 for(u32 off:{4u,8u,12u,16u,20u,24u,56u,52u,28u})if(load<u32>(a+off)!=load<u32>(b+off))return false;return true;
}
void ball_resources(void* self){
 WWHD_FUNC(0x02589380,void,self);u32 s=ea(self);Local<StringRef> shader_name;shader_name->value=0x10050830;shader_name->vtable=0x100507AC;
 u32 archive=ea(call<void*>(0x027FFCBC,self));s32 index=call<s32>(0x027B90AC,at<void>(load<u32>(archive+4)),shader_name.get());u32 shader=0;
 if(index>=0){
  u32 count=load<u32>(archive+8),entries=load<u32>(archive+12),entry=entries+(u32(index)<count?u32(index)*36:0);
  if(!load<u8>(entry+32)){
   u32 file=load<u32>(archive+4),data=u32(index)<load<u32>(file+28)?load<u32>(file+32)+u32(index)*132:0;
   call<void>(0x02800B0C,at<void>(entry),at<void>(data),u32(0));count=load<u32>(archive+8);entries=load<u32>(archive+12);
  }
  shader=entries+(u32(index)<count?u32(index)*36:0);
 }
 call<void>(0x0280068C,self,at<void>(shader),u32(0));store<u32>(s+0x4B8,1);store<u32>(s+0x4C0,0x100509C8);
 u32 resource=s+12;
 for(u32 i=0;i<2;i++){
  u32 item=resource+i*0x254,pointer=load<u32>(item);
  if(!pointer){u32 data=heap_allocate(0x18C,64);if(data){store<u32>(item+0x250,data);store<u32>(item+0x24C,33);}pointer=load<u32>(item+0x250);store<u32>(item,pointer);}
  call<void>(0x027FF478,at<void>(item+4),at<void>(pointer),u32(33),at<void>(resource+0x4AC));
 }
 store<u32>(resource+0x4B8,0);store<u8>(resource+0x4BC,1);
 for(u32 i=0;i<load<u32>(s);i++){
  u32 item=load<u32>(s+8);if(i<load<u32>(s+4))item+=i*20;
  u32 original=load<u32>(item);store<u32>(item,0);clear_graphics_array(item,4,8);clear_graphics_array(item,12,16);store<u32>(item,original);
  for(u32 off:{8u,16u}){u32 data=heap_allocate(0xF4,4);if(data)call<void>(0x027BF734,at<void>(data));if(data){store<u32>(item+off,data);store<u32>(item+off-4,1);}}
  for(u32 j=0;j<2;j++)call<void>(0x027FF530,at<void>(original),at<void>(load<u32>(item+8+j*8)),at<void>(resource+4+j*0x254),at<void>(resource+0x4AC),u32(0));
 }
 call<void>(0x027FE084,at<void>(s+0x4E4),u32(1),u32(0));call<void>(0x027B54E0,at<void>(s+0x4CC),at<void>(0x101E9E28),u32(4),load<u32>(0x10477664));store<u32>(s+0x4D0,6);
 u32 active=load<u32>(resource+0x4A8),data=load<u32>(resource+active*0x254),limit=data+0x180;
 for(u32 p=data;p<limit;p+=32)for(u32 k=0;k<32;k++)store<u8>((p&~31u)+k,0);
 active=load<u32>(resource+0x4A8);data=load<u32>(resource+active*0x254);
 for(u32 i=0;i<33;i++){
  u32 from=0x101E9C9C+i*12,to=data+i*12;
  f32 x=load<f32>(from),z=load<f32>(from+8),y=load<f32>(from+4);store<f32>(to,x);store<f32>(to+8,z);store<f32>(to+4,y);
 }
 active=load<u32>(resource+0x4A8);
 for(u32 i=0;i<33;i++){
  u32 from=load<u32>(resource+active*0x254)+i*12,to=load<u32>(resource+(active==0?1:0)*0x254)+i*12;
  store<f32>(to,load<f32>(from));store<f32>(to+4,load<f32>(from+4));store<f32>(to+8,load<f32>(from+8));
 }
 active=load<u32>(resource+0x4A8);u32 item=resource+active*0x254;
 call<void>(0x027B5E94,at<void>(item+4),u32(0),load<u32>(item+0x150));store<u32>(resource+0x4A8,load<u32>(resource+0x4A8)==0?1:0);
 call<void>(0x0274FBF8,at<void>(load<u32>(0x101F8B18)));
 Local<StringRef> texture1,texture2;texture1->value=0x1005081C;texture1->vtable=0x100507AC;
 u32 first=ea(call<void*>(0x026066C4,at<void>(load<u32>(0x101F4F28)),texture1.get(),u32(3)));
 if(!first)call<void>(0x0273AA24,at<void>(0x10050824),s32(426),at<void>(0x1005083C));call<void>(0x02773798,at<void>(s+0x990),at<void>(load<u32>(first+32)));
 texture2->value=0x1005081C;texture2->vtable=0x100507AC;
 u32 second=ea(call<void*>(0x026066C4,at<void>(load<u32>(0x101F4F28)),texture2.get(),u32(4)));
 if(!second)call<void>(0x0273AA24,at<void>(0x10050824),s32(433),at<void>(0x1005083C));call<void>(0x02773798,at<void>(s+0x900),at<void>(load<u32>(second+32)));
 call<void>(0x0274FCCC,at<void>(load<u32>(0x101F8B18)));
 if(!texture_equal(s+0xBB8,s+0x990))call<void>(0x027BDEB4,at<void>(s+0xBB8),at<void>(s+0x990));
 else{u32 a=load<u32>(s+0x9B8),b=load<u32>(s+0x9C0);store<u32>(s+0xC8C,a);store<u32>(s+0xBE0,a);store<u32>(s+0xC94,b);store<u32>(s+0xBE8,b);}
 store<u32>(s+0xD14,0);store<u32>(s+0xD1C,2);store<u32>(s+0xD18,2);store<u8>(s+0xD48,load<u8>(s+0xD48)|2);
 if(!texture_equal(s+0xA20,s+0x900))call<void>(0x027BDEB4,at<void>(s+0xA20),at<void>(s+0x900));
 else{u32 b=load<u32>(s+0x930),a=load<u32>(s+0x928);store<u32>(s+0xA50,b);store<u32>(s+0xA48,a);store<u32>(s+0xAFC,b);store<u32>(s+0xAF4,a);}
 store<u32>(s+0xB84,0);store<u32>(s+0xB7C,0);store<u32>(s+0xB80,0);store<u8>(s+0xD48,load<u8>(s+0xD48)|8);
 store<f32>(s+0xD38,load<f32>(0x10050818));store<u8>(s+0xBB0,load<u8>(s+0xBB0)|2);store<u32>(s+0xDC8,0);
}
VERIFY(0x02589380,ball_resources);

static bool draw_material(u32 self,u32 context,bool floor){
 u32 selected=0,mode=load<u32>(context+12);
 if(s32(mode)<4){u32 table=load<u32>(self+8);if(mode<load<u32>(self+4))table+=mode*20;selected=load<u32>(table);}
 u32 cache=ea(call<void*>(0x027F29D4,at<void>(0x104B45C0),at<void>(context))),shader=load<u32>(selected);
 if(shader!=load<u32>(cache+4)){
  u8 flag=load<u8>(shader);u32 previous=load<u32>(cache);
  if(flag&2){store<u8>(shader,flag&~2);call<void>(0x027BB9E0,at<void>(shader),u32(0));}
  u32 compiled=load<u32>(load<u32>(shader+0x7C)+0x28);
  if(previous!=compiled)call<void>(0x027B9F68,at<void>(compiled));
  u32 dl=load<u32>(shader+12);
  if(dl)call<void>(0xC00060E0,at<void>(load<u32>(shader+4)),dl);else call<void>(0x027BB7CC,at<void>(shader));
  store<u32>(cache,compiled);store<u32>(cache+4,shader);
 }
 mode=load<u32>(context+12);if(!mode)return false;
 Local<u8[0x11C]> graphics;
 if(mode==1||mode==2){
  u32 packet=load<u32>(self+(floor?0x4F0:0x4E8)),buffer=packet+16+load<u32>(packet+0x4C)*28;
  u32 desc=load<u32>(selected+12)?load<u32>(selected+16):0;
  s32 first=load<s16>(desc+12),second=load<s16>(desc+14),third=load<s16>(desc+16);
  u32 size=load<u32>(buffer+4),data=load<u32>(buffer+12);
  if(second!=-1)call<void>(0xC0006900,second,data,size);
  if(first!=-1)call<void>(0xC0006A38,first,data,size);
  if(third!=-1)call<void>(0xC00068A8,third,data,size);
  for(u32 off:{floor?0x5A0u:0x598u,floor?0x4F8u:0x4F0u})call_ptr<void>(load<u32>(load<u32>(self+off+12)+0x2C),at<void>(self+off),at<void>(selected));
  if(mode==2){u32 object=load<u32>(context+0x30);if(object)call_ptr<void>(load<u32>(load<u32>(object+12)+0x2C),at<void>(object),at<void>(selected));}
  u32 texture=load<u32>(selected+0x14)?load<u32>(selected+0x18):0;
  call<void>(0x027BE53C,at<void>(self+(floor?0x998:0xA20)),at<void>(texture+4),s32(-1),u32(0));
  if(!floor){texture=load<u32>(selected+0x14)>1?load<u32>(selected+0x18)+20:0;call<void>(0x027BE53C,at<void>(self+0xBB8),at<void>(texture+4),s32(-1),u32(0));}
  if(mode==2)call<void>(0x027FFE54,at<void>(context),at<void>(selected));
 }
 call<void>(0x02750250,graphics.get());u32 g=ea(graphics.get());store<u32>(g+8,2);
 store<u8>(g+0xE0,floor?1:0);store<u32>(g+12,0);
 if(floor){store<u32>(g+0xE4,4);store<f32>(g+0xE8,load<f32>(0x100508EC));}
 u32 packed=((load<u32>(g+0xEC)&0xFFFFFFF0u)+7)&0xFFFFFF0Fu;store<u32>(g+0xEC,packed+16);
 call<void>(0x0280037C,load<u32>(context+12),graphics.get());call<void>(0x02750370,graphics.get());
 u32 table=load<u32>(self+8),kind=load<u32>(context+12);if(kind<load<u32>(self+4))table+=kind*20;
 u32 active=load<u32>(self+0x4B4);call<void>(0x027BFE5C,at<void>(load<u32>(table+8+(active==0?8:0))));
 if(floor){
  u32 indices=load<u32>(self+0x4D8);
  if(indices)call<void>(0xC0006178,load<u32>(self+0x4D0),at<void>(indices),load<u32>(self+0x4CC),load<u32>(self+0x4D4),u32(0),u32(1));
 }else{
  for(s32 i=0;i<load<s32>(0x101E9C4C);i++){
   u32 indices=load<u32>(0x101E9C84+u32(i)*4),offset=load<u32>(0x101E9C50)*u32(i);
   if(indices)call<void>(0xC0006178,load<u32>(self+0x4D0),at<void>(indices),load<u32>(self+0x4CC),load<u32>(self+0x4D4)+load<u32>(self+0x4DC)*offset,u32(0),u32(1));
  }
 }
 call<void>(0x02750370,at<void>(0x104B474C));return true;
}
void ball_draw(void* self,void* context){WWHD_FUNC(0x02588DF8,void,self,context);draw_material(ea(self),ea(context),false);}
VERIFY(0x02588DF8,ball_draw);
void floor_draw(void* self,void* context){
 WWHD_FUNC(0x0258A880,void,self,context);u32 s=ea(self);
 if(load<u32>(s+0xB30)&&!load<u32>(s+0xBDC)&&!load<u32>(s+0xBE0))return;
 if(!draw_material(s,ea(context),true))return;
 u32 table=load<u32>(s+0xB30);
 for(u32 i=0;i<load<u8>(s+0xB34);i++)call<void>(0x02588DF8,at<void>(load<u32>(table+i*4)),context);
}
VERIFY(0x0258A880,floor_draw);
void floor_resources(void* self){
 WWHD_FUNC(0x0258B11C,void,self);u32 s=ea(self);Local<StringRef> shader_name;shader_name->value=0x10050910;shader_name->vtable=0x100507AC;
 u32 archive=ea(call<void*>(0x027FFCBC,self));s32 index=call<s32>(0x027B90AC,at<void>(load<u32>(archive+4)),shader_name.get());u32 shader=0;
 if(index>=0){
  u32 count=load<u32>(archive+8),entries=load<u32>(archive+12),entry=entries+(u32(index)<count?u32(index)*36:0);
  if(!load<u8>(entry+32)){
   u32 file=load<u32>(archive+4),data=u32(index)<load<u32>(file+28)?load<u32>(file+32)+u32(index)*132:0;
   call<void>(0x02800B0C,at<void>(entry),at<void>(data),u32(0));count=load<u32>(archive+8);entries=load<u32>(archive+12);
  }
  shader=entries+(u32(index)<count?u32(index)*36:0);
 }
 call<void>(0x0280068C,self,at<void>(shader),u32(0));store<u32>(s+0x4B8,19);store<u32>(s+0x4C0,0x100509CC);
 u32 resource=s+12;
 for(u32 i=0;i<2;i++){
  u32 item=resource+i*0x254,pointer=load<u32>(item);
  if(!pointer){u32 data=heap_allocate(0x80,64);if(data){store<u32>(item+0x250,data);store<u32>(item+0x24C,4);}pointer=load<u32>(item+0x250);store<u32>(item,pointer);}
  call<void>(0x027FF478,at<void>(item+4),at<void>(pointer),u32(4),at<void>(resource+0x4AC));
 }
 store<u32>(resource+0x4B8,0);store<u8>(resource+0x4BC,1);
 for(u32 i=0;i<load<u32>(s);i++){
  u32 item=load<u32>(s+8);if(i<load<u32>(s+4))item+=i*20;
  u32 original=load<u32>(item);store<u32>(item,0);clear_graphics_array(item,4,8);clear_graphics_array(item,12,16);store<u32>(item,original);
  for(u32 off:{8u,16u}){u32 data=heap_allocate(0xF4,4);if(data)call<void>(0x027BF734,at<void>(data));if(data){store<u32>(item+off,data);store<u32>(item+off-4,1);}}
  for(u32 j=0;j<2;j++)call<void>(0x027FF530,at<void>(original),at<void>(load<u32>(item+8+j*8)),at<void>(resource+4+j*0x254),at<void>(resource+0x4AC),u32(0));
 }
 call<void>(0x027FE084,at<void>(s+0x4EC),u32(1),u32(0));store<u16>(s+0x4E4,0);store<u16>(s+0x4E6,2);store<u16>(s+0x4E8,1);store<u16>(s+0x4EA,3);
 call<void>(0x027B54E0,at<void>(s+0x4CC),at<void>(s+0x4E4),u32(4),u32(4));store<u32>(s+0x4D0,6);
 u32 active=load<u32>(resource+0x4A8),data=load<u32>(resource+active*0x254),limit=data+0x80;
 for(u32 p=data;p<limit;p+=32)for(u32 k=0;k<32;k++)store<u8>((p&~31u)+k,0);
 active=load<u32>(resource+0x4A8);data=load<u32>(resource+active*0x254);
 for(u32 i=0;i<4;i++)for(u32 j=0;j<3;j++)store<f32>(data+i*32+j*4,load<f32>(0x101E9C54+i*12+j*4));
 f32 zero=load<f32>(0x10050808),one=load<f32>(0x10050860);
 for(u32 off:{0x1Cu,0x6Cu,0xCu,0x18u,0x74u,0x34u,0x3Cu,0x2Cu,0x50u,0x4Cu,0x10u,0x30u,0x58u,0x70u,0x14u,0x54u})store<f32>(data+off,zero);
 for(u32 off:{0x7Cu,0x38u,0x5Cu,0x78u})store<f32>(data+off,one);
 active=load<u32>(resource+0x4A8);
 for(u32 i=0;i<4;i++){
  u32 from=load<u32>(resource+active*0x254)+i*32,to=load<u32>(resource+(active==0?1:0)*0x254)+i*32;
  for(u32 j=0;j<8;j++)store<f32>(to+j*4,load<f32>(from+j*4));
 }
 active=load<u32>(resource+0x4A8);u32 item=resource+active*0x254;
 call<void>(0x027B5E94,at<void>(item+4),u32(0),load<u32>(item+0x150));store<u32>(resource+0x4A8,load<u32>(resource+0x4A8)==0?1:0);
 call<void>(0x0274FBF8,at<void>(load<u32>(0x101F8B18)));
 Local<StringRef> texture;texture->value=0x10050908;texture->vtable=0x100507AC;
 u32 file=ea(call<void*>(0x026066C4,at<void>(load<u32>(0x101F4F28)),texture.get(),u32(4)));
 if(!file)call<void>(0x0273AA24,at<void>(0x1005091C),s32(885),at<void>(0x10050928));
 call<void>(0x02773798,at<void>(s+0x908),at<void>(load<u32>(file+32)));call<void>(0x0274FCCC,at<void>(load<u32>(0x101F8B18)));
 if(!texture_equal(s+0x998,s+0x908))call<void>(0x027BDEB4,at<void>(s+0x998),at<void>(s+0x908));
 else{u32 b=load<u32>(s+0x938),a=load<u32>(s+0x930);store<u32>(s+0xA74,b);store<u32>(s+0xA6C,a);store<u32>(s+0x9C8,b);store<u32>(s+0x9C0,a);}
 store<u32>(s+0xAF4,0);store<u32>(s+0xAF8,0);store<u32>(s+0xAFC,0);store<u8>(s+0xB28,load<u8>(s+0xB28)|2);
}
VERIFY(0x0258B11C,floor_resources);

static u32 construct_ball(bool boss,f32 zero,f32 one){
 u32 s=ea(call<void*>(0x0273AD10,u32(0xDD4)));if(!s)return 0;store<u32>(s,0);store<u32>(s+0xDCC,0x100507C4);
 u32 small=s+4;if(!small)small=ea(call<void*>(0x0273AD10,u32(8)));if(small){store<u32>(small+4,0);store<u32>(small,0);}
 u32 resource=s+12;if(!resource)resource=ea(call<void*>(0x0273AD10,u32(0x4C0)));
 if(resource){
  call<void>(0x028EFFD0,at<void>(resource),u32(2),u32(0x254),u32(0x0258D314));
  store<u32>(resource+0x4A8,0);store<u32>(resource+0x4B0,12);store<u32>(resource+0x4B8,0);store<u8>(resource+0x4BC,0);store<u32>(resource+0x4AC,0);store<u32>(resource,0);store<u32>(resource+0x254,0);
 }
 call<void>(0x027B5430,at<void>(s+0x4CC));call<void>(0x027FD6F4,at<void>(s+0x4E4));call<void>(0x027FB40C,at<void>(s+0x4F0));
 store<u32>(s+0x4FC,0x1016EF84);call<void>(0x028F521C,at<void>(s+0x564),u32(0x34));if(!(s+0x564))call<void>(0x0273AD10,u32(48));
 call<void>(0x027FB40C,at<void>(s+0x598));store<u32>(s+0x5A4,0x1016EFB4);call<void>(0x028F521C,at<void>(s+0x60C),u32(0x2F0));
 for(u32 off=0x60C;off<=0x6B8;off+=4)store<f32>(s+off,(off%16==8)?one:zero);
 for(u32 off:{0x6BCu,0x6DCu,0x6FCu})call<void>(0x028EFFD0,at<void>(s+off),u32(2),u32(16),u32(0x0258D790));
 for(u32 off=0x71C;off<=0x86C;off+=48)if(!(s+off))call<void>(0x0273AD10,u32(48));
 for(u32 off=0x89C;off<=0x8EC;off+=16)if(!(s+off))call<void>(0x0273AD10,u32(16));
 store<u8>(s+0x8FC,0);call<void>(0x027BE6B8,at<void>(s+0x900));call<void>(0x027BE6B8,at<void>(s+0x990));
 call<void>(0x027BDF7C,at<void>(s+0xA20));call<void>(0x027BDF7C,at<void>(s+0xBB8));store<u32>(s+0xDCC,boss?0x100446B8:0x100509D0);return s;
}
void* floor_create(void* self,void* position,void* scale,s32 path,u32 count,s32 room){
 WWHD_FUNC(0x0258B830,void*,self,position,scale,path,count,room);u32 s=ea(self),v=ea(position);
 f32 z=load<f32>(v+8),y=load<f32>(v+4),x=load<f32>(v);
 store<f32>(s+0xB40,z);store<f32>(s+0xB38,x);store<f32>(s+0xB3C,y+load<f32>(0x10050938));
 Local<u32> allocator;call<void>(0x025F01D8,allocator.get(),at<void>(0x1005093C),u32(0),u32(0));
 u32 list=ea(call<void*>(0x0273ADAC,count*4));store<u32>(s+0xB30,list);
 if(!list){call<void>(0x025F02D0,allocator.get());call<void>(0x025F0270,allocator.get(),u32(2));return nullptr;}
 list=load<u32>(s+0xB30);store<u8>(s+0xB34,u8(count));
 if(count){
  f32 zero=load<f32>(0x10145180),one=load<f32>(0x1014517C);u32 i=0;
  do{
   u32 ball=construct_ball(path<0,zero,one);store<u32>(list,ball);
   if(!ball){store<u8>(s+0xB34,u8(i));i=u8(i+1);break;}
   call_ptr<void>(load<u32>(load<u32>(ball+0xDCC)+0x24),at<void>(ball),u32(path<0?i:u8(path)),room,load<f32>(s+0xB3C));
   call<void>(0x02589380,at<void>(load<u32>(list)));i=u8(i+1);list+=4;
  }while(i<load<u8>(s+0xB34));
 }
 store<f32>(s+0xB44,load<f32>(ea(scale)));store<f32>(s+0xB48,load<f32>(ea(scale)+8));store<u8>(s+0xB35,u8(path));
 call<void>(0x025F0270,allocator.get(),u32(2));call<void>(0x0258B11C,self);
 call<void>(0x028E93CC,at<void>(0x1048D0CC),load<f32>(s+0xB38),load<f32>(s+0xB3C),load<f32>(s+0xB40));
 call<void>(0x025F2518,load<f32>(s+0xB44),load<f32>(0x10050860),load<f32>(s+0xB48));
 call<void>(0x028E90D4,at<void>(0x1048D0CC),at<void>(s+0xB7C));call<void>(0x0258B068,self);return at<void>(load<u32>(s+0xB30));
}
VERIFY(0x0258B830,floor_create);
}
