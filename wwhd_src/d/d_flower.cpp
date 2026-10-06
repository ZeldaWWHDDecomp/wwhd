#include "gabi.h"
using namespace gabi;
namespace d_flower {
// HD packet fields: data+0x9C, animation+0x35BC, room heads+0x457C.
void data_destroy(void *self,u32 flags) {
 WWHD_FUNC(0x02544AA4,void,self,flags);
 if(self && (flags&1))call<void>(0x0273AF40,self);
}
VERIFY(0x02544AA4,data_destroy);
void empty_draw(void *self) { WWHD_FUNC(0x02544AB8,void,self); }
VERIFY(0x02544AB8,empty_draw);
f32 ground_y(void *position) {
 WWHD_FUNC(0x02544ABC,f32,position);
 Local<u8[0x54]> object;
 call<void>(0x02008E0C,object.get());
 u32 a=ea(object.get()),p=ea(position);
 f32 originalY=load<f32>(p+4),x=load<f32>(p);
 f32 raise=load<f32>(0x1004E03C);
 for(u32 k=0x44;k<=0x4A;++k)store<u8>(a+k,0);
 store<u32>(a+0x4C,0x1004E01C);store<u32>(a+0x20,0x1004E00C);
 store<u32>(a+0x50,1);
 f32 raised=fadds_ppc(originalY,raise);
 store<f32>(a+0x28,raised);store<f32>(p+4,raised);
 store<u32>(a+0x10,0x1004DFFC);store<f32>(a+0x24,x);
 store<u32>(a+4,a+0x4C);store<u32>(a+0x40,0x1004E02C);
 store<u32>(a,a+0x40);store<f32>(a+0x2C,load<f32>(p+8));
 void *play=call<void*>(0x025200D4);
 f32 y=call<f32>(0x02008974,at<void>(ea(play)+0x12A0),object.get());
 f32 restored=fsubs_ppc(load<f32>(p+4),raise);
 store<u32>(a+0x40,0x1004E02C);store<u32>(a+0x20,0x1004E00C);
 f32 threshold=load<f32>(0x1004E040);
 store<u32>(a+0x4C,0x1004DFEC);store<f32>(p+4,restored);
 call<void>(0x02008DAC,object.get(),u32(0));
 return y>threshold ? y:restored;
}
VERIFY(0x02544ABC,ground_y);
void *data_construct(void *self) {
 WWHD_FUNC(0x02544C4C,void*,self);
 if(!self)self=call<void*>(0x0273AD10,u32(0x44));
 if(!self)return nullptr;
 u32 a=ea(self);store<u8>(a+3,0);store<u8>(a+2,0);store<u8>(a,0);store<u8>(a+1,0);
 call<void>(0x028F521C,at<void>(a+0x10),u32(0x30));
 store<u32>(a+0x40,0);store<u8>(a,0);return self;
}
VERIFY(0x02544C4C,data_construct);
s32 new_animation(void *self) {
 WWHD_FUNC(0x02544CBC,s32,self);
 for(s32 i=8;i<72;++i){u32 a=ea(self)+0x35BC+i*0x38;
 if(!load<u8>(a)){store<u8>(a,1);store<u16>(a+4,0);store<u16>(a+2,0);return i;}}
 return -1;
}
VERIFY(0x02544CBC,new_animation);
void room_add(void *room,void *data) {
 WWHD_FUNC(0x02545998,void,room,data);
 store<u32>(ea(data)+0x40,load<u32>(ea(room)));store<u32>(ea(room),ea(data));
}
VERIFY(0x02545998,room_add);
void room_clear(void *room) {
 WWHD_FUNC(0x025459A8,void,room);
 u32 head=load<u32>(ea(room));
 while(head){store<u8>(head,0);u32 current=load<u32>(ea(room));
 call<void>(0x025E1B34,at<void>(current+4));
 current=load<u32>(ea(room));head=load<u32>(current+0x40);store<u32>(ea(room),head);}
}
VERIFY(0x025459A8,room_clear);
void set_animation(void *self,s32 index,s32 angle) {
 WWHD_FUNC(0x02545A0C,void,self,index,angle);
 u32 a=ea(self)+0x35BC+u32(index)*0x38;
 store<u8>(a,1);store<u16>(a+4,0);store<u16>(a+2,u16(angle));
}
VERIFY(0x02545A0C,set_animation);
void copy_matrix(void *destination,void *source) {
 WWHD_FUNC(0x02548554,void,destination,source);
 f32 values[12];for(int i=0;i<12;++i)values[i]=load<f32>(ea(source)+4*i);
 for(int i=0;i<12;++i)store<f32>(ea(destination)+4*i,values[i]);
}
VERIFY(0x02548554,copy_matrix);
void normalized_color(void *destination,void *source) {
 WWHD_FUNC(0x025485F4,void,destination,source);
 s16 red=load<s16>(ea(source)),blue=load<s16>(ea(source)+4),green=load<s16>(ea(source)+2);
 f32 scale=load<f32>(0x1004E118);
 s16 alpha=load<s16>(ea(source)+6);
 f32 colors[4]={f32(f64(red)/scale),f32(f64(green)/scale),f32(f64(blue)/scale),f32(f64(alpha)/scale)};
 for(int i=0;i<4;++i)store<f32>(ea(destination)+4*i,colors[i]);
}
VERIFY(0x025485F4,normalized_color);
void static_init() {
 WWHD_FUNC(0x02549118,void);
 store<u32>(0x10475950,0);store<u32>(0x10475948,0);store<u32>(0x10475954,0);store<u32>(0x1047594C,0);
 call<void>(0x028F026C,at<void>(0x101E1CBC));
 f32 first=load<f32>(0x1004E168),second=load<f32>(0x1004E16C);
 store<f32>(0x10475934,first);store<f32>(0x10475938,second);
 call<void>(0x028ED6F8,at<void>(0x10475944));call<void>(0x028F026C,at<void>(0x101E1CC8));
 call<void>(0x028EAB2C,at<void>(0x10475945));call<void>(0x028F026C,at<void>(0x101E1CD4));
}
VERIFY(0x02549118,static_init);
void *animation_construct(void *self) {
 WWHD_FUNC(0x025491C0,void*,self);
 if(!self)self=call<void*>(0x0273AD10,u32(0x38));
 if(!self)return nullptr;
 store<u8>(ea(self),0);return self;
}
VERIFY(0x025491C0,animation_construct);
void *room_construct(void *self) {
 WWHD_FUNC(0x025491FC,void*,self);
 if(!self)self=call<void*>(0x0273AD10,u32(0x4));
 if(!self)return nullptr;
 store<u32>(ea(self),0);return self;
}
VERIFY(0x025491FC,room_construct);
void *string_construct(void *self,void *text) {
 WWHD_FUNC(0x02549290,void*,self,text);
 if(!self)self=call<void*>(0x0273AD10,u32(0x8));
 if(!self)return nullptr;
 store<u32>(ea(self),ea(text));store<u32>(ea(self)+4,0x1004DFD4);return self;
}
VERIFY(0x02549290,string_construct);
void *draw_node_construct(void *self) {
 WWHD_FUNC(0x025492E0,void*,self);
 if(!self)self=call<void*>(0x0273AD10,u32(0xA8));
 if(!self)return nullptr;
 call<void>(0x027FB40C,self);store<u32>(ea(self)+0xC,0x1016EF84);call<void>(0x028F521C,at<void>(ea(self)+0x74),u32(0x34));if(ea(self)+0x74==0)call<void*>(0x0273AD10,u32(0x30));return self;
}
VERIFY(0x025492E0,draw_node_construct);
void *matrix_construct(void *self) {
 WWHD_FUNC(0x02549350,void*,self);
 if(!self)self=call<void*>(0x0273AD10,u32(0x10));
 if(!self)return nullptr;
 return self;
}
VERIFY(0x02549350,matrix_construct);
void *buffer1_construct(void *self) {
 WWHD_FUNC(0x02549604,void*,self);
 if(!self)self=call<void*>(0x0273AD10,u32(0x254));
 if(!self)return nullptr;
 call<void>(0x027B5BD8,at<void>(ea(self)+4));call<void>(0x027BF734,at<void>(ea(self)+0x158));store<u32>(ea(self)+0x250,0);store<u32>(ea(self)+0x24C,0);return self;
}
VERIFY(0x02549604,buffer1_construct);
void *buffer2_construct(void *self) {
 WWHD_FUNC(0x02549660,void*,self);
 if(!self)self=call<void*>(0x0273AD10,u32(0x254));
 if(!self)return nullptr;
 call<void>(0x027B5BD8,at<void>(ea(self)+4));call<void>(0x027BF734,at<void>(ea(self)+0x158));store<u32>(ea(self)+0x250,0);store<u32>(ea(self)+0x24C,0);return self;
}
VERIFY(0x02549660,buffer2_construct);
void animation_destroy(void *self,u32 flags) {
 WWHD_FUNC(0x025491AC,void,self,flags);
 if(self&&(flags&1))call<void>(0x0273AF40,self);
}
VERIFY(0x025491AC,animation_destroy);
void set_vec3(void *self,f32 x,f32 y,f32 z) {
 WWHD_FUNC(0x025496BC,void,self,x,y,z);
 store<f32>(ea(self)+4,y);store<f32>(ea(self)+8,z);store<f32>(ea(self),x);
}
VERIFY(0x025496BC,set_vec3);
void set_vec4(void *self,f32 x,f32 y,f32 z,f32 w) {
 WWHD_FUNC(0x025496CC,void,self,x,y,z,w);
 store<f32>(ea(self)+8,z);store<f32>(ea(self)+4,y);store<f32>(ea(self)+12,w);store<f32>(ea(self),x);
}
VERIFY(0x025496CC,set_vec4);
void set_vec2(void *self,f32 x,f32 y) {
 WWHD_FUNC(0x025496E0,void,self,x,y);
 store<f32>(ea(self),x);store<f32>(ea(self)+4,y);
}
VERIFY(0x025496E0,set_vec2);
void *new_data(void *self,s32 type,void *position,s32 room,s32 variant) {
 WWHD_FUNC(0x02548FEC,void*,self,type,position,room,variant);
 if(u32(room)>=64)call<void>(0x0273AA24,at<void>(0x1004E134),u32(0x1F68),at<void>(0x1004E144));
 u32 start=load<u16>(ea(self)+0x98),base=ea(self)+0x9C;
 for(u32 i=start;i<200;++i){u32 data=base+i*0x44;
 if(!(load<u8>(data)&2)){call<void>(0x02548D28,self,at<void>(data),i,type,position,room,variant);return at<void>(data);}}
 for(u32 i=0;i<start;++i){u32 data=base+i*0x44;
 if(!(load<u8>(data)&2)){call<void>(0x02548D28,self,at<void>(data),i,type,position,room,variant);return at<void>(data);}}
 return nullptr;
}
VERIFY(0x02548FEC,new_data);
void update(void *self) {
 WWHD_FUNC(0x02548B6C,void,self);
 u32 animations=ea(self)+0x35BC;
 for(int i=0;i<72;++i){u32 a=animations+i*0x38;
 call<void>(0x025F1884,at<void>(0x1048D0CC),s32(load<s16>(a+2)));
 call<void>(0x025F1BF4,at<void>(0x1048D0CC),s32(load<s16>(a+4)));
 call<void>(0x025F1C28,at<void>(0x1048D0CC),s32(s16(-s32(load<s16>(a+2)))));
 call<void>(0x028E90D4,at<void>(0x1048D0CC),at<void>(a+8));}
 f32 radius=load<f32>(0x1004E128);int groundUpdates=0;
 Local<u8[12]> clipPosition;
 for(int i=0;i<200;++i){u32 data=ea(self)+0x9C+i*0x44;
 if(!(load<u8>(data)&2))continue;
 call<void>(0x0207A9A0,at<void>(data+3));
 if((load<u8>(data)&0x10)&&groundUpdates<8){f32 y=ground_y(at<void>(data+4));u8 flags=load<u8>(data);
 store<f32>(data+8,y);store<u8>(data,flags&0xEF);++groundUpdates;}
 f32 y=load<f32>(data+8),x=load<f32>(data+4),z=load<f32>(data+12);
 store<f32>(clipPosition.a,x);store<f32>(clipPosition.a+4,y);store<f32>(clipPosition.a+8,z);
 s32 clipped=call<s32>(0x02838148,at<void>(0x1048CFF0),at<void>(0x104B45F8),clipPosition.get(),radius);
 u8 flags=load<u8>(data);
 if(clipped){store<u8>(data,flags|4);continue;}
 s8 index=load<s8>(data+1);store<u8>(data,flags&0xFB);
 u32 matrix=animations+s32(index)*0x38;
 // These scalar matrix translations are direct copies in the optimized reference.
 store<u32>(matrix+0x14,load<u32>(data+4));store<u32>(matrix+0x24,load<u32>(data+8));store<u32>(matrix+0x34,load<u32>(data+12));
 call<void>(0x028E90D4,at<void>(matrix+8),at<void>(data+0x10));}
 call<void>(0x027F0E04,at<void>(load<u32>(0x104B4634)),self,u32(0));
 call<void>(0x025486B8,self);
}
VERIFY(0x02548B6C,update);
void calc(void *self) {
 WWHD_FUNC(0x02548370,void,self);
 f32 scale=load<f32>(0x1004E108);
 for(u32 i=0;i<8;++i){u32 timer=load<u32>(0x101FF560)+i*0xFA;
 u16 angle=u16(ftoi(fmuls_ppc(f32(timer),scale)));
 f32 cosine=load<f32>(0x104A44FC+(u32(angle)>>3)*8);
 store<u16>(ea(self)+0x35C0+i*0x38,u16(ftoi(fmadds(scale,cosine,scale))));}
 s32 room=load<s8>(0x1047E6C8);u32 head=load<u32>(ea(self)+0x457C+u32(room)*4);
 if(!head)return;
 u32 play=ea(call<void*>(0x025200D4));u32 player=load<u32>(play+0x5B2C);
 store<u8>(ea(self)+0x467C,(load<u32>(player+0x3B8)&0x40)!=0);
 Local<u8[12]> delta;Local<u8[12]> sword;
 f32 x=load<f32>(player+0x3E4),y=load<f32>(player+0x3E8),z=load<f32>(player+0x3EC);
 store<f32>(sword.a,x);store<f32>(sword.a+8,z);store<f32>(sword.a+4,y);
 store<f32>(delta.a,x);store<f32>(delta.a+4,y);store<f32>(delta.a+8,z);
 call<void>(0x0201ADE0,sword.get(),delta.get(),at<void>(player+0x314));
 f32 dz=load<f32>(delta.a+8),dx=load<f32>(delta.a);
 s32 angle=call<s32>(0x020195B0,dx,dz);store<u16>(ea(self)+0x467E,u16(angle));
 call<void>(0x0201ADE0,at<void>(ea(self)+0x4684),delta.get(),sword.get());
 dx=load<f32>(delta.a);dz=load<f32>(delta.a+8);
 angle=call<s32>(0x020195B0,dx,dz);
 u32 sx=load<u32>(sword.a),sy=load<u32>(sword.a+4),sz=load<u32>(sword.a+8);
 store<u32>(ea(self)+0x4684,sx);store<u16>(ea(self)+0x4680,u16(angle));
 store<u32>(ea(self)+0x4688,sy);store<u32>(ea(self)+0x468C,sz);store<u8>(0x101D645C,0);
 play=ea(call<void*>(0x025200D4));
 call<void>(0x020184DC,at<void>(play+0x5008),load<f32>(0x1004E05C));
 call<void>(0x02018428,at<void>(play+0x5008),load<f32>(0x1004E03C));
 store<u8>(play+0x5020,0xB);store<u8>(play+0x5021,2);
 do{if(!(load<u8>(head)&4))call<void>(0x025457C4,at<void>(head),at<void>(player),room);
 head=load<u32>(head+0x40);}while(head);
}
VERIFY(0x02548370,calc);
void *buffer_select(void *self,u32 slot,u32 clear) {
 WWHD_FUNC(0x02549238,void*,self,slot,clear);
 u32 index=load<u32>(ea(self)+0x950)+slot*2,buffer=ea(self)+index*0x254;
 if(clear){u32 begin=load<u32>(buffer),end=begin+0x7160;
 for(u32 line=begin;line<end;line+=32){u32 aligned=line&~31u;for(u32 k=0;k<32;++k)store<u8>(aligned+k,0);}
 if(begin<end)buffer=ea(self)+(load<u32>(ea(self)+0x950)+slot*2)*0x254;}
 return at<void>(buffer);
}
VERIFY(0x02549238,buffer_select);
void buffer_rotate(void *self,u32 unused,u32 count) {
 WWHD_FUNC(0x025496EC,void,self,unused,count);
 u32 active=load<u32>(ea(self)+0x950);
 if(count){u32 buffer=ea(self)+active*0x254+4;
 for(u32 i=0;i<count;++i,buffer+=0x4A8)call<void>(0x027B5E94,at<void>(buffer),u32(0),load<u32>(buffer+0x14C));
 active=load<u32>(ea(self)+0x950);}
 store<u32>(ea(self)+0x950,active==0);
}
VERIFY(0x025496EC,buffer_rotate);
void surface_assign(void *self,void *source) {
 WWHD_FUNC(0x02549768,void,self,source);
 for(u32 offset:{4u,8u,12u,16u,20u,24u,56u,52u,28u})
 if(load<u32>(ea(self)+offset)!=load<u32>(ea(source)+offset)){call<void>(0x027BDEB4,self,source);return;}
 u32 image=load<u32>(ea(source)+0x28),mip=load<u32>(ea(source)+0x30);
 store<u32>(ea(self)+0x28,image);store<u32>(ea(self)+0x30,mip);
 store<u32>(ea(self)+0xD4,image);store<u32>(ea(self)+0xDC,mip);
}
VERIFY(0x02549768,surface_assign);
void flags_set(void *self,u32 mask) {
 WWHD_FUNC(0x02549818,void,self,mask);
 store<u8>(ea(self),load<u8>(ea(self))|mask);
}
VERIFY(0x02549818,flags_set);
void hit_check(void *self,void *player,s32 room) {
 WWHD_FUNC(0x025457C4,void,self,player,room);
 Local<u8[20]> hit;Local<u32> actor;
 call<void>(0x0251694C,hit.get());
 u32 play=ea(call<void*>(0x025200D4));
 u32 mask=call<u32>(0x025170D8,at<void>(play+0x4EF8),at<void>(ea(self)+4),actor.get(),hit.get());
 bool attack=false;
 if(mask&1){u32 a=load<u32>(actor.a);if(a && load<s16>(a+8)!=453 && load<s16>(a+8)!=454)attack=true;}
 if(!(mask&2)&&!attack){
 s32 index=load<s8>(ea(self)+1);if(index<8)return;
 play=ea(call<void*>(0x025200D4));index=load<s8>(ea(self)+1);
 u32 animation=load<u32>(play+0x5AC4)+u32(index)*56+0x35BC;
 u32 angle=load<u16>(animation+2);s32 target=s16(angle&0xE000);u32 windIndex=angle>>13;
 play=ea(call<void*>(0x025200D4));u32 packet=load<u32>(play+0x5AC4);
 s32 bend=load<s16>(packet+0x35C0+windIndex*56);
 if(call<s32>(0x0200F378,at<void>(animation+4),bend,s32(0x10),s32(0xFA0),s32(0x64)))return;
 if(!call<s32>(0x0200F8D0,at<void>(animation+2),target,s32(0x320)))return;
 play=ea(call<void*>(0x025200D4));index=load<s8>(ea(self)+1);packet=load<u32>(play+0x5AC4);
 store<u8>(packet+0x35BC+u32(index)*56,0);store<u8>(ea(self)+1,load<u16>(animation+2)>>13);return;
 }
 if(!load<u32>(actor.a))call<void>(0x0273AA24,at<void>(0x1004E064),u32(0x1BA0),at<void>(0x1004E074));
 if(mask&2)call<void>(0x02544D04,self,at<void>(load<u32>(actor.a)),mask,room);
 if(!(load<u8>(ea(self))&8)&&attack)call<void>(0x025453B8,self,at<void>(load<u32>(actor.a)),mask,room,hit.get());
}
VERIFY(0x025457C4,hit_check);
void work_co(void *self,void *actor,u32 mask,s32 room) {
 WWHD_FUNC(0x02544D04,void,self,actor,mask,room);
 f32 x=fsubs_ppc(load<f32>(ea(self)+4),load<f32>(ea(actor)+0x314));
 f32 z=fsubs_ppc(load<f32>(ea(self)+12),load<f32>(ea(actor)+0x31C));
 Local<u8[12]> delta;
 store<f32>(delta.a+4,load<f32>(0x1004E044));store<f32>(delta.a,x);store<f32>(delta.a+8,z);
 f32 squared=call<f32>(0x028E8DD0,delta.get());
 if(squared>load<f32>(0x1004E048))return;
 s32 angle=call<s32>(0x020195B0,x,z);f32 distance=call<f32>(0x028F4384,squared);
 if(load<s8>(ea(self)+1)<8){
 if(!load<u8>(ea(self)+3)){
 u8 flags=load<u8>(ea(self));
 if(!(flags&8)&&load<f32>(ea(actor)+0x370)>load<f32>(0x1004E04C)){
 Local<u8[12]> position;
 f32 y=load<f32>(ea(self)+8),pz=load<f32>(ea(self)+12),rise=load<f32>(0x1004E050),px=load<f32>(ea(self)+4);
 store<f32>(position.a+8,pz);store<f32>(position.a,px);store<f32>(position.a+4,fadds_ppc(y,rise));
 u32 particle=(flags&0x20)?((flags&0x40)?0x82C6:0x3DD):0x3DE;
 call<void*>(0x025200D4);
 u32 color=0x1047E6CC+u32(room)*0x22C+0xEC;
 u32 play=ea(call<void*>(0x025200D4)),control=load<u32>(play+0x5AB0);
 void *emitter=call<void*>(0x025A847C,at<void>(control),u32(0),particle,position.get(),u32(0),u32(0),u32(0xFF),u32(0),s32(s8(room)),at<void>(color),at<void>(color),u32(0));
 if(emitter){f32 one=load<f32>(0x1004E054);
 store<f32>(ea(emitter)+0x34,particle==0x82C6?load<f32>(0x1004E058):one);
 Local<u8[28]> texture;
 store<f32>(texture.a+12,one);store<f32>(texture.a+20,one);store<u8>(texture.a+24,1);
 store<f32>(texture.a+8,one);store<u8>(texture.a+26,0);store<u8>(texture.a+27,0);
 store<u32>(texture.a+4,0x1004DFD4);store<u8>(texture.a+25,0);store<f32>(texture.a+16,one);store<u32>(texture.a,0x10000160);
 play=ea(call<void*>(0x025200D4));control=load<u32>(play+0x5AB0);
 for(u32 k=0;k<4;++k)store<u32>(texture.a+8+k*4,load<u32>(control+0x50+k*4));
 call<void>(0x0281E5A8,emitter,texture.get());store<u8>(ea(self)+3,0x10);
 }}}
 u32 play=ea(call<void*>(0x025200D4));s32 index=new_animation(at<void>(load<u32>(play+0x5AC4)));
 if(index<0)return;store<u8>(ea(self)+1,u8(index));
 }
 u32 play=ea(call<void*>(0x025200D4));s32 index=load<s8>(ea(self)+1);u32 packet=load<u32>(play+0x5AC4);
 f32 bend=fsubs_ppc(load<f32>(0x1004E05C),distance),height=load<f32>(0x1004E060);
 u32 animation=packet+u32(index)*56+0x35BC;store<u16>(animation+2,u16(angle));
 s32 tilt=call<s32>(0x020195B0,bend,height);store<u16>(animation+4,u16(tilt));
}
VERIFY(0x02544D04,work_co);
void work_at_no_cut(void *self,void *actor,u32 mask,s32 room,void *hit,void *collision) {
 WWHD_FUNC(0x02545040,void,self,actor,mask,room,hit,collision);
 u32 object=ea(call<void*>(0x025157AC,collision));
 f32 x=load<f32>(object+0x7C),z=load<f32>(object+0x84),zero=load<f32>(0x1004E044);
 Local<u8[12]> first;Local<u8[12]> fallback;Local<u8[12]> vector;
 store<f32>(first.a+8,z);store<f32>(first.a,x);store<f32>(first.a+4,zero);
 f32 squared=call<f32>(0x028E8DD0,first.get());
 if(__builtin_fabsf(squared)<load<f32>(0x100030B8)&&actor&&load<s16>(ea(actor)+8)==168){
 f32 ax=load<f32>(ea(actor)+0x314),sz=load<f32>(ea(self)+12),sx=load<f32>(ea(self)+4),az=load<f32>(ea(actor)+0x31C);
 x=fsubs_ppc(sx,ax);z=fsubs_ppc(sz,az);
 store<f32>(fallback.a+4,zero);store<f32>(fallback.a,x);store<f32>(fallback.a+8,z);
 call<f32>(0x028E8DD0,fallback.get());
 }
 store<f32>(vector.a+8,z);store<f32>(vector.a,x);store<f32>(vector.a+4,zero);
 squared=call<f32>(0x028E8DD0,vector.get());
 s32 angle=call<s32>(0x020195B0,x,z);f32 distance=call<f32>(0x028F4384,squared);
 if(load<s8>(ea(self)+1)<8){
 if(!load<u8>(ea(self)+3)){
 u8 flags=load<u8>(ea(self));
 if(!(flags&8)&&distance>load<f32>(0x1004E04C)){
 Local<u8[12]> position;
 f32 y=load<f32>(ea(self)+8),pz=load<f32>(ea(self)+12),rise=load<f32>(0x1004E050),px=load<f32>(ea(self)+4);
 store<f32>(position.a+8,pz);store<f32>(position.a,px);store<f32>(position.a+4,fadds_ppc(y,rise));
 u32 particle=(flags&0x20)?((flags&0x40)?0x82C6:0x3DD):0x3DE;
 call<void*>(0x025200D4);
 u32 color=0x1047E6CC+u32(room)*0x22C+0xEC;
 u32 play=ea(call<void*>(0x025200D4)),control=load<u32>(play+0x5AB0);
 void *emitter=call<void*>(0x025A847C,at<void>(control),u32(0),particle,position.get(),u32(0),u32(0),u32(0xFF),u32(0),s32(s8(room)),at<void>(color),at<void>(color),u32(0));
 if(emitter){f32 one=load<f32>(0x1004E054);
 store<f32>(ea(emitter)+0x34,particle==0x82C6?load<f32>(0x1004E058):one);
 Local<u8[28]> texture;
 store<f32>(texture.a+12,one);store<f32>(texture.a+20,one);store<u8>(texture.a+24,1);
 store<f32>(texture.a+8,one);store<u8>(texture.a+26,0);store<u8>(texture.a+27,0);
 store<u32>(texture.a+4,0x1004DFD4);store<u8>(texture.a+25,0);store<f32>(texture.a+16,one);store<u32>(texture.a,0x10000160);
 play=ea(call<void*>(0x025200D4));control=load<u32>(play+0x5AB0);
 for(u32 k=0;k<4;++k)store<u32>(texture.a+8+k*4,load<u32>(control+0x50+k*4));
 call<void>(0x0281E5A8,emitter,texture.get());store<u8>(ea(self)+3,0x10);
 }}}
 u32 play=ea(call<void*>(0x025200D4));s32 index=new_animation(at<void>(load<u32>(play+0x5AC4)));
 if(index<0)return;store<u8>(ea(self)+1,u8(index));
 }
 u32 play=ea(call<void*>(0x025200D4));s32 index=load<s8>(ea(self)+1);u32 packet=load<u32>(play+0x5AC4);
 f32 bend=distance,height=load<f32>(0x1004E060);
 u32 animation=packet+u32(index)*56+0x35BC;store<u16>(animation+2,u16(angle));
 s32 tilt=call<s32>(0x020195B0,bend,height);store<u16>(animation+4,u16(tilt));
}
VERIFY(0x02545040,work_at_no_cut);
static void apply_texture(void *emitter,Local<u8[28]> &texture,f32 one) {
 store<u32>(texture.a+4,0x1004DFD4);store<f32>(texture.a+12,one);store<u8>(texture.a+24,1);
 store<f32>(texture.a+20,one);store<u32>(texture.a,0x10000160);store<u8>(texture.a+26,0);
 store<f32>(texture.a+16,one);store<u8>(texture.a+25,0);store<u8>(texture.a+27,0);store<f32>(texture.a+8,one);
 u32 play=ea(call<void*>(0x025200D4)),control=load<u32>(play+0x5AB0);
 for(u32 k=0;k<4;++k)store<u32>(texture.a+8+k*4,load<u32>(control+0x50+k*4));
 call<void>(0x0281E5A8,emitter,texture.get());
}
void work_at(void *self,void *actor,u32 mask,s32 room,void *hit) {
 WWHD_FUNC(0x025453B8,void,self,actor,mask,room,hit);
 u32 collision=load<u32>(ea(hit)+4);
 if(collision&&(load<u32>(collision+0x10)&0x003CC000)){
 work_at_no_cut(self,actor,mask,room,hit,at<void>(collision));return;
 }
 if(load<s8>(ea(self)+1)>=8){u32 play=ea(call<void*>(0x025200D4));s32 index=load<s8>(ea(self)+1);
 u32 packet=load<u32>(play+0x5AC4);store<u8>(packet+0x35BC+u32(index)*56,0);}
 store<u8>(ea(self),load<u8>(ea(self))|8);
 if(!load<u32>(0x10475958)){store<u32>(0x10475958,1);call<void>(0x0201A478,at<void>(0x1047593C),u32(0),u32(0),u32(0));}
 call<void*>(0x025200D4);
 Local<u8[12]> position;
 f32 z=load<f32>(ea(self)+12),y=load<f32>(ea(self)+8);u8 flags=load<u8>(ea(self));f32 x=load<f32>(ea(self)+4);
 u32 color=0x1047E720+u32(room)*0x22C+0x98;
 store<f32>(position.a+4,y);store<f32>(position.a,x);store<f32>(position.a+8,z);
 f32 one=load<f32>(0x1004E054);
 Local<u8[28]> texture1;Local<u8[28]> texture2;
 void *emitter=nullptr;
 u32 particle=(flags&0x20)?((flags&0x40)?0x82C6:0x3DD):0x3DE;
 if(particle==0x82C6){u32 play=ea(call<void*>(0x025200D4));u32 control=load<u32>(play+0x5AB0);
 void *extra=call<void*>(0x025A847C,at<void>(control),u32(0),u32(0x82C7),position.get(),at<void>(0x1047593C),u32(0),u32(0xFF),u32(0),s32(s8(room)),at<void>(color),at<void>(color),u32(0));
 if(extra)apply_texture(extra,texture1,one);
 }
 u32 play=ea(call<void*>(0x025200D4));u32 control=load<u32>(play+0x5AB0);
 emitter=call<void*>(0x025A847C,at<void>(control),u32(0),particle,position.get(),at<void>(0x1047593C),u32(0),u32(0xFF),u32(0),s32(s8(room)),at<void>(color),at<void>(color),u32(0));
 if(emitter)apply_texture(emitter,texture2,one);
 s32 item=load<s8>(ea(self)+2);
 if(item>=0)call<void>(0x025D8120,at<void>(ea(self)+4),item,s32(-1),room,u32(0),u32(0),u32(1),u32(0));
 if(!load<u8>(0x101D645C)){store<u8>(0x101D645C,1);s32 reverb=call<s32>(0x02520540,room);
 call<void>(0x025E1A40,u32(0x282C),at<void>(ea(self)+4),u32(0),reverb);}
}
VERIFY(0x025453B8,work_at);
void set_data(void *self,void *data,s32 index,s32 type,void *position,s32 room,s32 item) {
 WWHD_FUNC(0x02548D28,void,self,data,index,type,position,room,item);
 f32 ground;u8 flags;
 if(call<s32>(0x025DBE00)){ground=ground_y(position);flags=6;}
 else{flags=0x16;ground=load<f32>(ea(position)+4);}
 if(type==2)flags|=0x20;
 f32 range=load<f32>(0x1004E12C);store<u8>(ea(data),flags);
 f32 random=call<f32>(0x020198D8,range);store<u8>(ea(data)+1,u8(ftoi(random)));
 f32 x=load<f32>(ea(position)),z=load<f32>(ea(position)+8);
 store<f32>(ea(data)+4,x);store<f32>(ea(data)+8,ground);store<u8>(ea(data)+2,u8(item));
 store<f32>(ea(data)+12,z);store<u8>(ea(data)+3,0);
 u32 base=ea(self)+0x1AEA8;u32 selected;
 if(load<u32>(ea(self)+0x457C+u32(room)*4))selected=load<u32>(base+0x17A8);
 else{
 Local<u8[8]> literal;Local<u8[8]> stage;
 store<u32>(literal.a,0x1004E130);store<u32>(literal.a+4,0x1004DFD4);
 u32 play=ea(call<void*>(0x025200D4));store<u32>(stage.a+4,0x1004DFD4);store<u32>(stage.a,play+0x5134);
 call_ptr<void>(load<u32>(load<u32>(literal.a+4)+0x14),literal.get());
 call_ptr<void>(load<u32>(load<u32>(literal.a+4)+0x14),literal.get());
 u32 cached=load<u32>(literal.a);
 call_ptr<void>(load<u32>(load<u32>(stage.a+4)+0x14),stage.get());
 bool equal=cached==load<u32>(stage.a);
 if(!equal){u32 a=load<u32>(literal.a),b=load<u32>(stage.a);
 for(u32 i=0;i<0x40001;++i){u8 lhs=load<u8>(a+i),rhs=load<u8>(b+i);if(lhs!=rhs)break;if(lhs==0){equal=true;break;}}}
 bool oasis=equal && room==33;
 u32 positions=oasis?base+0xBCC:base;
 store<u32>(base+0x1798,positions);store<u32>(base+0x179C,positions+12);
 store<u32>(base+0x17A0,base+(oasis?0x1540:0x974));store<u32>(base+0x17A4,base+(oasis?0x1558:0x98C));
 selected=base+(oasis?0x1570:0x9A4);store<u32>(base+0x17A8,selected);
 }
 if(selected==base+0x1570)store<u8>(ea(data),load<u8>(ea(data))|0x40);
 room_add(at<void>(ea(self)+0x457C+u32(room)*4),data);store<u16>(ea(self)+0x98,u16(index));
}
VERIFY(0x02548D28,set_data);
void buffer1_destroy(void *self,u32 flags) {
 WWHD_FUNC(0x02549828,void,self,flags);
 if(!self)return;call<void>(0x027BF880,at<void>(ea(self)+0x158),u32(2));call<void>(0x027B5CBC,at<void>(ea(self)+4),u32(2));if(flags&1)call<void>(0x0273AF40,self);
}
VERIFY(0x02549828,buffer1_destroy);
void buffer2_destroy(void *self,u32 flags) {
 WWHD_FUNC(0x02549888,void,self,flags);
 if(!self)return;call<void>(0x027BF880,at<void>(ea(self)+0x158),u32(2));call<void>(0x027B5CBC,at<void>(ea(self)+4),u32(2));if(flags&1)call<void>(0x0273AF40,self);
}
VERIFY(0x02549888,buffer2_destroy);
void draw_node_destroy(void *self,u32 flags) {
 WWHD_FUNC(0x025498E8,void,self,flags);
 if(!self)return;call<void>(0x027FB528,self,u32(0));if(flags&1)call<void>(0x0273AF40,self);
}
VERIFY(0x025498E8,draw_node_destroy);
void room_node_destroy(void *self,u32 flags) {
 WWHD_FUNC(0x0254993C,void,self,flags);
 if(!self)return;call<void>(0x027FB528,self,u32(0));if(flags&1)call<void>(0x0273AF40,self);
}
VERIFY(0x0254993C,room_node_destroy);
void empty_resource1(void *self) { WWHD_FUNC(0x02549990,void,self);  }
VERIFY(0x02549990,empty_resource1);
void empty_resource2(void *self) { WWHD_FUNC(0x02549994,void,self);  }
VERIFY(0x02549994,empty_resource2);
u32 resource_enabled(void *self) { WWHD_FUNC(0x02549998,u32,self); return 1; }
VERIFY(0x02549998,resource_enabled);
void *room_node_construct(void *self) {
 WWHD_FUNC(0x0254937C,void*,self);
 if(!self)self=call<void*>(0x0273AD10,u32(0x364));
 if(!self)return nullptr;
 call<void>(0x027FB40C,self);store<u32>(ea(self)+12,0x1016EFB4);
 call<void>(0x028F521C,at<void>(ea(self)+0x74),u32(0x2F0));
 f32 zero=load<f32>(0x10145180),one=load<f32>(0x1014517C);
 // Eleven initial vectors, each with an alpha/homogeneous component of one.
 for(u32 i=0;i<44;++i)store<f32>(ea(self)+0x74+i*4,(i%4)==3?one:zero);
 for(u32 offset:{0x124u,0x144u,0x164u})call<void>(0x028EFFD0,at<void>(ea(self)+offset),u32(2),u32(0x10),at<void>(0x02549350));
 for(u32 offset=0x184;offset<=0x304;offset+=0x30)if(ea(self)+offset==0)call<void*>(0x0273AD10,u32(0x30));
 for(u32 offset=0x314;offset<=0x354;offset+=0x10)if(ea(self)+offset==0)call<void*>(0x0273AD10,u32(0x10));
 return self;
}
VERIFY(0x0254937C,room_node_construct);
// Graphics allocation ownership: paired vertex buffers and shader arrays use the
// current resource heap; pointers are reloaded after the heap accessor/free call.
static void heap_release_field(u32 field) {
 u32 pointer=load<u32>(field);
 void *heap=call<void*>(0x02755FEC,at<void>(load<u32>(0x101F8B4C)),at<void>(pointer));
 u32 target=load<u32>(load<u32>(ea(heap)+12)+0x3C);
 call_ptr<void>(target,heap,at<void>(load<u32>(field)));
}
static void cleanup_buffers(u32 group) {
 for(u32 pair=0;pair<2;++pair)for(u32 member=0;member<2;++member){u32 buffer=group+pair*0x4A8+member*0x254;
 call<void>(0x027BF7E8,at<void>(buffer+0x158));u32 pointer=load<u32>(buffer+0x250);
 store<u32>(buffer,0);
 if(pointer){load<u32>(buffer+0x24C);heap_release_field(buffer+0x250);store<u32>(buffer+0x24C,0);store<u32>(buffer+0x250,0);}}
}
static void cleanup_shader_array(u32 descriptor) {
 u32 pointer=load<u32>(descriptor+8);
 if(!pointer)return;
 s32 count=load<s32>(descriptor+4);u32 offset=0;
 for(s32 i=0;i<count;++i,offset+=20){u32 row=pointer+offset;
 if(row){u32 children=load<u32>(row+8);store<u32>(row,0);
 for(u32 side=0;side<2;++side){u32 sizeField=row+4+side*8,ptrField=row+8+side*8;
 if(side)children=load<u32>(ptrField);
 if(children){s32 number=load<s32>(sizeField);u32 childOffset=0;
 for(s32 j=0;j<number;++j,childOffset+=0xF4){u32 child=children+childOffset;
 u32 target=load<u32>(load<u32>(child+0xF0)+12);
 call_ptr<void>(target,at<void>(child),u32(2));number=load<s32>(sizeField);children=load<u32>(ptrField);}
 heap_release_field(ptrField);store<u32>(sizeField,0);store<u32>(ptrField,0);}}
 count=load<s32>(descriptor+4);pointer=load<u32>(descriptor+8);
 }}
 heap_release_field(descriptor+8);store<u32>(descriptor+4,0);store<u32>(descriptor+8,0);
}
void packet_destroy(void *self,u32 flags) {
 WWHD_FUNC(0x0254725C,void,self,flags);
 if(!self)return;
 u32 root=ea(self)+0x1A2DC;
 store<u32>(ea(self)+12,0x1004E180);
 for(u32 offset:{0xCu,0xBD8u,0x17A4u}){cleanup_buffers(root+offset);store<u32>(root+offset+0x960,0);}
 for(u32 geometry:{0x1798u,0xBCCu,0u}){
 call<void>(0x027BE2B0,at<void>(root+geometry+0x9A4),u32(2));
 call<void>(0x027B54A0,at<void>(root+geometry+0x98C),u32(2));
 call<void>(0x027B54A0,at<void>(root+geometry+0x974),u32(2));
 u32 group=root+geometry+12;
 if(group){cleanup_buffers(group);store<u32>(group+0x960,0);
 call<void>(0x028F0164,at<void>(group),u32(4),u32(0x254),at<void>(geometry?0x02549828:0x02549888),u32(0),u32(0));}
 cleanup_shader_array(root+geometry);
 }
 call<void>(0x028F0164,at<void>(ea(self)+0xC9DC),u32(64),u32(0x364),at<void>(0x025498E8),u32(0),u32(0));
 call<void>(0x028F0164,at<void>(ea(self)+0x469C),u32(200),u32(0xA8),at<void>(0x0254993C),u32(0),u32(0));
 call<void>(0x027FD764,at<void>(ea(self)+0x4690),u32(2));call<void>(0x027F13DC,self,u32(0));
 if(flags&1)call<void>(0x0273AF40,self);
}
VERIFY(0x0254725C,packet_destroy);

// HD lighting state is 0x1C8 bytes (72 words cleared by tevstr_init).
void graphics_prepare(void *self) {
 WWHD_FUNC(0x025486B8,void,self);
 u32 a=ea(self);
 Local<u8[0x30]> view; copy_matrix(view.get(),at<void>(0x104B45F8));
 call<void>(0x027FDA54,at<void>(a+0x4690),u32(0),view.get(),at<void>(0x104B470C),at<void>(load<u32>(0x104B4708)+0x240));
 Local<u8[0x1C8]> lighting; u32 light=ea(lighting.get());
 // The three leading material records receive the same 0x44-byte defaults.
 for(u32 base: {u32(0),u32(0xC0),u32(0x144)}) {
  for(u32 off=0;off<0x44;off+=4) {
   if(off==0x18||off==0x1C||off==0x20) store<u32>(light+base+off,load<u32>(0x1016E414+off));
   else store<f32>(light+base+off,load<f32>(0x1016E414+off));
  }
 }
 call<void>(0x0255FFF4,lighting.get(),s32(s8(load<u8>(0x1047E6C8))),u32(0xFF));
 void *environment=call<void*>(0x02555D0C);
 call<void>(0x025626A4,environment,s32(1),u32(0),lighting.get());
 Local<u8[16]> color; Local<u8[16]> scaled;
 u32 material=load<u32>(a+0x4694);
 normalized_color(color.get(),at<void>(light+0x90));
 call<void>(0x0274D458,scaled.get(),color.get(),load<f32>(light+0x28));
 for(u32 i=0;i<16;i+=4)store<u32>(material+0x1C4+i,load<u32>(ea(scaled.get())+i));
 material=load<u32>(a+0x4694);
 normalized_color(color.get(),at<void>(light+0x160));
 call<void>(0x0274D458,scaled.get(),color.get(),load<f32>(light+0x16C));
 for(u32 i=0;i<16;i+=4)store<u32>(material+0x1D4+i,load<u32>(ea(scaled.get())+i));
 call<void>(0x027FE010,at<void>(a+0x4690));
 u32 drawn=0; f32 divisor=load<f32>(0x1004E118);
 Local<u8[48]> matrix;
 for(u32 room=0;room<64;++room) {
  u32 data=load<u32>(a+0x457C+room*4);
  if(!data)continue;
  u32 node=a+0xC9DC+room*0x364;
  for(u32 i=0;i<4;++i)store<f32>(node+0xB4+i*4,f32(double(load<s16>(light+0x90+i*2))/double(divisor)));
  for(u32 i=0;i<4;++i)store<f32>(node+0xC4+i*4,f32(double(load<u8>(light+0x98+i))/double(divisor)));
  call<void>(0x0274D2AC,at<void>(node+0xC4),load<f32>(light+0x24));
  u32 source=load<u32>(a+0x4694);
  for(u32 i=0;i<16;i+=4)store<u32>(node+0x94+i,load<u32>(source+0x1C4+i));
  call<void>(0x027FB678,at<void>(node));
  call<void>(0x0255F8F4,lighting.get());
  while(data) {
   if(!(load<u8>(data)&4)) {
    for(u32 i=0;i<48;i+=4)store<f32>(ea(matrix.get())+i,load<f32>(data+0x10+i));
    u32 drawNode=a+0x469C+drawn*0xA8;
    call<void>(0x028E90D4,matrix.get(),at<void>(drawNode+0x74));
    call<void>(0x027FB678,at<void>(drawNode));++drawn;
   }
   data=load<u32>(data+0x40);
  }
 }
}
VERIFY(0x025486B8,graphics_prepare);

static u32 shader_row(u32 descriptor,u32 index) {
 u32 pointer=load<u32>(descriptor+8);
 return index<load<u32>(descriptor+4)?pointer+index*20:pointer;
}
static void indexed_draw(u32 geometry) {
 u32 count=load<u32>(geometry+12);
 if(count)call<void>(0xC0006178,load<u32>(geometry+4),count,load<u32>(geometry),load<u32>(geometry+8),u32(0),u32(1));
}
static void draw_uniforms(u32 shader,u32 object) {
 u32 uniforms=load<u32>(shader+12)?load<u32>(shader+16):0;
 u32 record=object+16+load<u32>(object+0x4C)*28;
 s32 vertex=load<s16>(uniforms+12),pixel=load<s16>(uniforms+14),geometry=load<s16>(uniforms+16);
 u32 size=load<u32>(record+4),buffer=load<u32>(record+12);
 if(pixel!=-1)call<void>(0xC0006900,pixel,buffer,size);
 if(vertex!=-1)call<void>(0xC0006A38,vertex,buffer,size);
 if(geometry!=-1)call<void>(0xC00068A8,geometry,buffer,size);
}
void graphics_draw(void *self,void *context) {
 WWHD_FUNC(0x02547C94,void,self,context);
 u32 a=ea(self),ctx=ea(context),root=a+0x1A2DC,shader=0;
 s32 pass=load<s32>(ctx+12);
 if(pass<4)shader=load<u32>(shader_row(root,u32(pass)));
 u32 cache=ea(call<void*>(0x027F29D4,at<void>(0x104B45C0)));
 u32 material=load<u32>(shader);
 if(material!=load<u32>(cache+4)) {
  u8 flags=load<u8>(material);u32 oldProgram=load<u32>(cache);
  if(flags&2){store<u8>(material,flags&~2);call<void>(0x027BB9E0,at<void>(material),u32(0));}
  u32 program=load<u32>(load<u32>(material+0x7C)+0x28);
  if(oldProgram!=program)call<void>(0x027B9F68,at<void>(program));
  u32 size=load<u32>(material+12);
  if(size)call<void>(0xC00060E0,load<u32>(material+4),size);
  else call<void>(0x027BB7CC,at<void>(material));
  store<u32>(cache,program);store<u32>(cache+4,material);
 }
 pass=load<s32>(ctx+12);
 if(pass==0) {
  u32 object=load<u32>(ctx+20);
  if(object)draw_uniforms(shader,load<u32>(object+4));
 } else if(pass==1||pass==2) {
  draw_uniforms(shader,load<u32>(a+0x4694));
  if(pass==2) {
   u32 object=load<u32>(ctx+0x30);
   if(object)call_ptr<void>(load<u32>(load<u32>(object+12)+0x2C),at<void>(object),at<void>(shader));
   call<void>(0x027FFE54,context,at<void>(shader));
  }
 }
 Local<u8[0x11C]> state;u32 st=ea(state.get());
 call<void>(0x02750250,state.get());
 store<u32>(st+12,2);store<u8>(st+0xE0,1);store<u32>(st+0xE4,4);
 store<f32>(st+0xE8,load<f32>(0x1004E104));
 u32 flags=load<u32>(st+0xEC);
 store<u32>(st+0xEC,(((flags&~15u)+7u)&0xFFFFFF0Fu)+16u);
 call<void>(0x0280037C,load<u32>(ctx+12),state.get());
 call<void>(0x02750370,state.get());
 u32 drawn=0;
 for(u32 room=0;room<64;++room) {
  u32 data=load<u32>(a+0x457C+room*4);
  if(data){u32 node=a+0xC9DC+room*0x364;call_ptr<void>(load<u32>(load<u32>(node+12)+0x2C),at<void>(node),at<void>(shader));}
  s32 texture=-1,shape=-1;
  while(data) {
   if(!(load<u8>(data)&4)) {
    u32 node=a+0x469C+drawn*0xA8;
    call_ptr<void>(load<u32>(load<u32>(node+12)+0x2C),at<void>(node),at<void>(shader));++drawn;
    bool special=(load<u8>(data)&0x20)!=0;
    s32 wantedTexture=special?1:0;
    if(texture!=wantedTexture) {
     u32 params=load<u32>(shader+20)?load<u32>(shader+24):0;
     u32 resource=special?load<u32>(root+0x2374):root+0x9A4;
     call<void>(0x027BE53C,at<void>(resource),at<void>(params+4),u32(0),u32(0));texture=wantedTexture;
    }
    bool cut=(load<u8>(data)&8)!=0;
    s32 wantedShape=(special?2:0)+(cut?1:0);
    if(shape!=wantedShape) {
     u32 descriptor=special?load<u32>(root+0x2364):root;
     u32 row=shader_row(descriptor,load<u32>(ctx+12));
     u32 index=load<u32>(a+0x1AC38);
     u32 column=(index?u32(__builtin_clz(index)):32u)/4u&~7u;
     u32 selected=row+column;
     u32 buffer=load<u32>(selected+8);
     if(cut && load<u32>(selected+4)>1)buffer+=0xF4;
     call<void>(0x027BFE5C,at<void>(buffer));shape=wantedShape;
    }
    u32 geometry=special?load<u32>(root+(cut?0x2370:0x236C)):root+(cut?0x98C:0x974);
    indexed_draw(geometry);
   }
   data=load<u32>(data+0x40);
  }
 }
 call<void>(0x02750370,at<void>(0x104B474C));
}
VERIFY(0x02547C94,graphics_draw);

static void *flower_heap_allocate(u32 size,u32 align) {
 void *heap=call<void*>(0x02756140,at<void>(load<u32>(0x101F8B4C)));
 return call_ptr<void*>(load<u32>(load<u32>(ea(heap)+12)+0x34),heap,size,align);
}
static void flower_shader_load(u32 descriptor,void *name) {
 u32 manager=ea(call<void*>(0x027FFCBC));
 s32 index=call<s32>(0x027B90AC,at<void>(load<u32>(manager+4)),name);
 u32 entry=0;
 if(index>=0) {
  u32 array=load<u32>(manager+12),count=load<u32>(manager+8);
  entry=u32(index)<count?array+u32(index)*36:array;
  if(!load<u8>(entry+0x20)) {
   u32 archive=load<u32>(manager+4),record=0;
   if(u32(index)<load<u32>(archive+0x1C))record=load<u32>(archive+0x20)+u32(index)*132;
   call<void>(0x02800B0C,at<void>(entry),at<void>(record),u32(0));
   array=load<u32>(manager+12);count=load<u32>(manager+8);
   entry=u32(index)<count?array+u32(index)*36:array;
  }
 }
 call<void>(0x0280068C,at<void>(descriptor),at<void>(entry),u32(0));
}
static void flower_shader_buffers(u32 descriptor,u32 buffers) {
 for(u32 i=0;i<load<u32>(descriptor);++i) {
  u32 row=shader_row(descriptor,i),shader=load<u32>(row);
  store<u32>(row,0);
  for(u32 side=0;side<2;++side) {
   u32 countField=row+4+side*8,pointerField=row+8+side*8,pointer=load<u32>(pointerField);
   if(pointer) {
    for(s32 n=0;n<load<s32>(countField);++n) {
     u32 child=pointer+u32(n)*0xF4;
     call_ptr<void>(load<u32>(load<u32>(child+0xF0)+12),at<void>(child),u32(2));
     pointer=load<u32>(pointerField);
    }
    heap_release_field(pointerField);store<u32>(countField,0);store<u32>(pointerField,0);
   }
  }
  store<u32>(row,shader);
  for(u32 side=0;side<2;++side) {
   u32 pointer=ea(flower_heap_allocate(0x1E8,4));
   for(u32 n=0;n<2;++n)if(pointer+n*0xF4)call<void>(0x027BF734,at<void>(pointer+n*0xF4));
   if(pointer){store<u32>(row+8+side*8,pointer);store<u32>(row+4+side*8,2);}
  }
  for(u32 side=0;side<2;++side)for(u32 n=0;n<2;++n) {
   u32 child=load<u32>(row+8+side*8);
   if(n<load<u32>(row+4+side*8))child+=n*0xF4;
   call<void>(0x027FF530,at<void>(shader),at<void>(child),at<void>(buffers+4+side*0x254+n*0x4A8),at<void>(buffers+0x954),u32(0));
  }
 }
}
static u32 flower_inline_clear(u32 group,u32 side,u32 bytes) {
 u32 buffer=group+(load<u32>(group+0x950)+side*2)*0x254;
 u32 begin=load<u32>(buffer),end=begin+bytes;
 for(u32 line=begin;line<end;line+=32)for(u32 k=0;k<32;++k)store<u8>((line&~31u)+k,0);
 if(begin<end)buffer=group+(load<u32>(group+0x950)+side*2)*0x254;
 return buffer;
}
static void flower_vertices(u32 output,u32 countAddress,u32 position,u32 normal,u32 texcoord,u32 color,bool setters) {
 for(u32 i=0;i<load<u32>(countAddress);++i) {
  u32 vertex=output+i*48;
  if(setters) {
   call<void>(0x025496BC,at<void>(vertex),load<f32>(position+i*12),load<f32>(position+i*12+4),load<f32>(position+i*12+8));
   call<void>(0x025496BC,at<void>(vertex+12),load<f32>(normal+i*12),load<f32>(normal+i*12+4),load<f32>(normal+i*12+8));
   call<void>(0x025496CC,at<void>(vertex+32),load<f32>(color+i*16),load<f32>(color+i*16+4),load<f32>(color+i*16+8),load<f32>(color+i*16+12));
   call<void>(0x025496E0,at<void>(vertex+24),load<f32>(texcoord+i*8),load<f32>(texcoord+i*8+4));
  } else {
   for(u32 k=0;k<12;k+=4)store<f32>(vertex+k,load<f32>(position+i*12+k));
   for(u32 k=0;k<12;k+=4)store<f32>(vertex+12+k,load<f32>(normal+i*12+k));
   for(u32 k=0;k<16;k+=4)store<f32>(vertex+32+k,load<f32>(color+i*16+k));
   if(countAddress==0x101D643C)call<void>(0x025496E0,at<void>(vertex+24),load<f32>(texcoord+i*8),load<f32>(texcoord+i*8+4));
   else for(u32 k=0;k<8;k+=4)store<f32>(vertex+24+k,load<f32>(texcoord+i*8+k));
  }
 }
}
void *packet_construct(void *self) {
 WWHD_FUNC(0x02545A2C,void*,self);
 if(!self)self=call<void*>(0x0273AD10,u32(0x1C654));
 if(!self)return nullptr;
 u32 a=ea(self),root=a+0x1A2DC;
 call<void>(0x027F1278,self);store<u32>(a+12,0x1004E180);store<u16>(a+0x98,0);
 call<void>(0x028EFFD0,at<void>(a+0x9C),u32(200),u32(0x44),u32(0x02544C4C));
 call<void>(0x028EFFD0,at<void>(a+0x35BC),u32(72),u32(0x38),u32(0x025491C0));
 call<void>(0x028EFFD0,at<void>(a+0x457C),u32(64),u32(4),u32(0x025491FC));
 store<u16>(a+0x467E,0);store<u16>(a+0x4680,0);store<u8>(a+0x467C,0);
 call<void>(0x027FD6F4,at<void>(a+0x4690));
 call<void>(0x028EFFD0,at<void>(a+0x469C),u32(200),u32(0xA8),u32(0x025492E0));
 call<void>(0x028EFFD0,at<void>(a+0xC9DC),u32(64),u32(0x364),u32(0x0254937C));
 u32 groups[3];
 for(u32 i=0;i<3;++i) {
  u32 set=root+i*0xBCC;store<u32>(set,0);
  u32 pair=set+4;if(!pair)pair=ea(call<void*>(0x0273AD10,u32(8)));
  if(pair){store<u32>(pair+4,0);store<u32>(pair,0);}
  u32 group=set+12;groups[i]=group;if(!group)group=ea(call<void*>(0x0273AD10,u32(0x968)));
  if(group) {
   call<void>(0x028EFFD0,at<void>(group),u32(4),u32(0x254),u32(i?0x02549660:0x02549604));
   store<u32>(group+0x950,0);store<u32>(group+0x960,0);store<u32>(group+0x958,0x30);
   store<u8>(group+0x964,0);store<u32>(group+0x954,0);
   for(u32 n=0;n<4;++n)store<u32>(group+n*0x254,0);
  }
  call<void>(0x027B5430,at<void>(set+0x974));call<void>(0x027B5430,at<void>(set+0x98C));
  call<void>(0x027BDF7C,at<void>(set+0x9A4));call<void>(0x027BE6B8,at<void>(set+0xB3C));
 }
 for(u32 off=0x2364;off<=0x2374;off+=4)store<u32>(root+off,0);
 for(u32 n=0;n<200;++n)store<u8>(a+0x9C+n*0x44,0);
 for(u32 n=0;n<72;++n)store<u8>(a+0x35BC+n*0x38,0);
 for(u32 n=0;n<8;++n)call<void>(0x02545A0C,self,n,s32(s16(n*0x2000)));
 Local<u8[8]> name;
 for(u32 i=0;i<3;++i) {
  store<u32>(ea(name.get()),0x1004E098);store<u32>(ea(name.get())+4,0x1004DFD4);
  flower_shader_load(root+i*0xBCC,name.get());
 }
 for(u32 i=0;i<3;++i){store<u32>(groups[i]+0x954,0x1013);store<u32>(groups[i]+0x95C,0x1004E170);}
 for(u32 i=0;i<3;++i) {
  u32 group=groups[i],count=i?605:90;
  for(u32 side=0;side<2;++side)for(u32 n=0;n<2;++n) {
   u32 buffer=group+side*0x254+n*0x4A8,pointer=load<u32>(buffer);
   if(!pointer) {
    u32 allocated=ea(flower_heap_allocate(i?0x7170:0x10E0,0x40));
    if(allocated){store<u32>(buffer+0x250,allocated);store<u32>(buffer+0x24C,count);}
    pointer=load<u32>(buffer+0x250);store<u32>(buffer,pointer);
   }
   call<void>(0x027FF478,at<void>(buffer+4),at<void>(pointer),count,at<void>(group+0x954));
  }
  store<u32>(group+0x960,0);store<u8>(group+0x964,1);
 }
 for(u32 i=0;i<3;++i)flower_shader_buffers(root+i*0xBCC,groups[i]);
 call<void>(0x027FD838,at<void>(a+0x4690),u32(1),u32(0));
 for(u32 i=0;i<64;++i)call<void>(0x027FB5D4,at<void>(a+0xC9DC+i*0x364),u32(0));
 for(u32 i=0;i<200;++i)call<void>(0x027FB5D4,at<void>(a+0x469C+i*0xA8),u32(0));
 u32 geometry[6]={root+0x974,root+0x98C,root+0x1540,root+0x1558,root+0x210C,root+0x2124};
 const u32 data[6]={0x101D7540,0x101D81A0,0x101D8FE0,0x101D96E8,0x101E08E8,0x101E1BE4};
 for(u32 i=0;i<6;++i){call<void>(0x027B54E0,at<void>(geometry[i]),at<void>(data[i]),u32(4),load<u32>(0x101D6430+i*8));store<u32>(geometry[i]+4,4);}
 u32 buffer=flower_inline_clear(groups[0],0,0x10E0);
 flower_vertices(load<u32>(buffer),0x101D642C,0x101D6460,0x101D6898,0x101D7270,0x101D6CD0,false);
 buffer=flower_inline_clear(groups[0],1,0x10E0);
 flower_vertices(load<u32>(buffer),0x101D6434,0x101D7660,0x101D7930,0x101D7FC0,0x101D7C00,false);
 buffer=groups[0]+load<u32>(groups[0]+0x950)*0x254+4;
 for(u32 i=0;i<2;++i,buffer+=0x4A8)call<void>(0x027B5E94,at<void>(buffer),u32(0),load<u32>(buffer+0x14C));
 store<u32>(groups[0]+0x950,load<u32>(groups[0]+0x950)==0);
 buffer=flower_inline_clear(groups[1],0,0x7160);
 flower_vertices(load<u32>(buffer),0x101D643C,0x101D8230,0x101D859C,0x101D8D98,0x101D8908,false);
 buffer=ea(call<void*>(0x02549238,at<void>(groups[1]),u32(1),u32(1)));
 flower_vertices(load<u32>(buffer),0x101D6444,0x101D9118,0x101D928C,0x101D95F0,0x101D9400,true);
 call<void>(0x025496EC,at<void>(groups[1]),u32(0),u32(2));
 buffer=ea(call<void*>(0x02549238,at<void>(groups[2]),u32(0),u32(1)));
 flower_vertices(load<u32>(buffer),0x101D644C,0x101D9778,0x101DB3D4,0x101DF600,0x101DD030,true);
 buffer=ea(call<void*>(0x02549238,at<void>(groups[2]),u32(1),u32(1)));
 flower_vertices(load<u32>(buffer),0x101D6454,0x101E1404,0x101E15FC,0x101E1A94,0x101E17F4,true);
 call<void>(0x025496EC,at<void>(groups[2]),u32(0),u32(2));
 call<void>(0x0274FBF8,at<void>(load<u32>(0x101F8B18)));
 Local<u8[8]> folder;Local<u8[8]> archive;
 void *folderName=call<void*>(0x02549290,folder.get(),at<void>(0x1004E0C0));
 void *archiveName=call<void*>(0x02549290,archive.get(),at<void>(0x1004E0D0));
 void *resource=call<void*>(0x026124B0,at<void>(load<u32>(0x101F4F7C)),folderName,archiveName,u32(0));
 call<void>(0x025491AC,archive.get(),u32(2));call<void>(0x025491AC,folder.get(),u32(2));
 const u32 textures[3]={0x1004E0A4,0x1004E0E8,0x1004E088};
 for(u32 i=0;i<3;++i) {
  u32 texture=root+i*0xBCC+0x9A4,sampler=root+i*0xBCC+0xB3C;
  call<void>(0x02773870,at<void>(sampler),resource,at<void>(textures[i]));
  call<void>(0x02549768,at<void>(texture),at<void>(sampler));
  store<u32>(texture+0x160,1);store<u32>(texture+0x15C,1);store<u32>(texture+0x164,1);
  call<void>(0x02549818,at<void>(texture+0x190),u32(2));
 }
 call<void>(0x0274FCCC,at<void>(load<u32>(0x101F8B18)));
 store<u32>(root+0x2374,root+0x1570);store<u32>(root+0x2368,groups[1]);
 store<u32>(root+0x236C,root+0x1540);store<u32>(root+0x2370,root+0x1558);store<u32>(root+0x2364,root+0xBCC);
 return self;
}
VERIFY(0x02545A2C,packet_construct);
}
