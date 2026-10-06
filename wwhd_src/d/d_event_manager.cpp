// Local WWHD event-manager reconstruction.
#include "gabi.h"
using namespace gabi;
namespace event_manager {
static u32 word(u32 p,u32 off=0){return load<u32>(p+off);}
static void put(u32 p,u32 off,u32 value){store<u32>(p+off,value);}
u32 actor_flag(u32 actor){
 WWHD_FUNC(0x02542D74,u32,actor);
 put(actor,0x2E0,word(actor,0x2E0)|0x800);return 4;
}
VERIFY(0x02542D74,actor_flag);
u32 dummy_name(u32 self){
 WWHD_FUNC(0x02542E94,u32,self);
 u32 p=word(self,0x53C);return p?p:0x1004DD00;
}
VERIFY(0x02542E94,dummy_name);
u32 current_cut(u32 self,s32 staff){
 WWHD_FUNC(0x02542EAC,u32,self,staff);
 if(staff==-1)return 0;
 u32 entry=word(self,8)+u32(staff)*80;
 return word(self,12)+word(entry,0x38)*80;
}
VERIFY(0x02542EAC,current_cut);
u32 exception_ctor(u32 self){
 WWHD_FUNC(0x02543378,u32,self);
 if(!self)self=call<u32>(0x0273AD10,12u);
 if(self){put(self,8,0);put(self,0,0xffffffff);put(self,4,0);}
 return self;
}
VERIFY(0x02543378,exception_ctor);
void exception_init(u32 self){
 WWHD_FUNC(0x0254351C,void,self);
 put(self,0,0xffffffff);put(self,8,0);put(self,4,0xffffffff);
}
VERIFY(0x0254351C,exception_init);
u32 search_param_ctor(u32 self,u32 name,u32 actor,u32 flags){
 WWHD_FUNC(0x02543638,u32,self,name,actor,flags);
 if(!self)self=call<u32>(0x0273AD10,16u);
 if(self){put(self,0,name);put(self,8,flags);store<u16>(self+12,0);put(self,4,actor);}
 return self;
}
VERIFY(0x02543638,search_param_ctor);
u32 manager_ctor(u32 self){
 WWHD_FUNC(0x025436A4,u32,self);
 if(!self)self=call<u32>(0x0273AD10,0x540u);
 if(self){call<void>(0x0254273C,self);put(self,0x20,0);call<void>(0x02543378,self+0x24);put(self,0x53C,0);}
 return self;
}
VERIFY(0x025436A4,manager_ctor);
void close_proc(u32 self,u32 event){
 WWHD_FUNC(0x02543708,void,self,event);put(event,0xA4,4);
}
VERIFY(0x02543708,close_proc);
void set_goal(u32 self,u32 vec){
 WWHD_FUNC(0x02543714,void,self,vec);
 store<f32>(self+0x30,load<f32>(vec));store<f32>(self+0x34,load<f32>(vec+4));store<f32>(self+0x38,load<f32>(vec+8));
}
VERIFY(0x02543714,set_goal);
u32 event_data(u32 self,s32 index){
 WWHD_FUNC(0x02544044,u32,self,index);
 u32 header=word(self);if(!header||index<0||index>=load<s32>(header+4))return 0;
 return word(self,4)+u32(index)*176;
}
VERIFY(0x02544044,event_data);
u32 order(u32 self,s32 index){
 WWHD_FUNC(0x02544534,u32,self,index);
 u32 data=call<u32>(0x02544044,self,index);if(!data)return 0;put(data,0xA4,1);return 1;
}
VERIFY(0x02544534,order);
u32 end_old(u32 self,u32 name){
 WWHD_FUNC(0x0254457C,u32,self,name);
 s32 id=call<s32>(0x02543F10,self,name,255u);return call<u32>(0x025440C8,self,id);
}
VERIFY(0x0254457C,end_old);
u32 start_old(u32 self,u32 name){
 WWHD_FUNC(0x025445B8,u32,self,name);
 s32 id=call<s32>(0x02543F10,self,name,255u);return call<u32>(0x0254407C,self,id);
}
VERIFY(0x025445B8,start_old);
u32 priority(u32 self,s32 index){
 WWHD_FUNC(0x025445F4,u32,self,index);
 u32 event=call<u32>(0x02544044,self,index);return event?word(event,0x28):0;
}
VERIFY(0x025445F4,priority);
u32 end_sound(u32 self,s32 index){
 WWHD_FUNC(0x02544628,u32,self,index);
 u32 event=call<u32>(0x02544044,self,index);return event?load<u8>(event+0x94):0;
}
VERIFY(0x02544628,end_sound);
u32 advancing(u32 self,s32 staff){
 WWHD_FUNC(0x025447C8,u32,self,staff);
 return staff==-1?0:load<u8>(word(self,8)+u32(staff)*80+0x46);
}
VERIFY(0x025447C8,advancing);
u32 optional_cut(u32 self,s32 staff){
 WWHD_FUNC(0x025447EC,u32,self,staff);
 if(staff==-1)return 0;return call<u32>(0x02542EAC,self,staff);
}
VERIFY(0x025447EC,optional_cut);
u32 cut_name(u32 self,s32 staff){
 WWHD_FUNC(0x02544830,u32,self,staff);
 u32 cut=call<u32>(0x02542EAC,self,staff);
 if(!cut)call<void>(0x0273AA24,0x1004DFACu,0x55Du,0x1004DF9Cu);
 return cut;
}
VERIFY(0x02544830,cut_name);
u32 get_goal(u32 self){
 WWHD_FUNC(0x02544900,u32,self);return self+0x30;
}
VERIFY(0x02544900,get_goal);
u32 release_staff(u32 self,u32 data){
 WWHD_FUNC(0x02544944,u32,self,data);return call<u32>(0x025D5218,0x02542BE0u,data);
}
VERIFY(0x02544944,release_staff);
u32 present_end(u32 self){
 WWHD_FUNC(0x02544950,u32,self);return call<s32>(0x025432B4,0u)>0;
}
VERIFY(0x02544950,present_end);
u32 cancel_present(u32 self){
 WWHD_FUNC(0x02544980,u32,self);return call<u32>(0x025432B4,1u)==1;
}
VERIFY(0x02544980,cancel_present);
u32 check_start(u32 self){
 WWHD_FUNC(0x025449B0,u32,self);u32 play=call<u32>(0x025200D4);
 return load<u8>(play+0x5292)!=0&&word(self,0x24)!=0xffffffff;
}
VERIFY(0x025449B0,check_start);
void static_init(){
 WWHD_FUNC(0x02544A10,void);
 put(0x10475924,8,0);put(0x10475924,0,0);put(0x10475924,12,0);put(0x10475924,4,0);
 call<void>(0x028F026C,0x101D6408u);f32 first=load<f32>(0x1004DFC4),second=load<f32>(0x1004DFC8);
 store<f32>(0x10475918,first);store<f32>(0x1047591C,second);
 call<void>(0x028ED6F8,0x10475920u);call<void>(0x028F026C,0x101D6414u);call<void>(0x028EAB2C,0x10475921u);call<void>(0x028F026C,0x101D6420u);
}
VERIFY(0x02544A10,static_init);
void inline_dtor(u32 self,u32 flags){
 WWHD_FUNC(0x02544AA4,void,self,flags);if(self&&(flags&1))call<void>(0x0273AF40,self);
}
VERIFY(0x02544AA4,inline_dtor);
void debug_before(u32 self){
 WWHD_FUNC(0x02544AB8,void,self);
}
VERIFY(0x02544AB8,debug_before);

static bool string_equal(u32 a,u32 b){
 for(;;++a,++b){u8 x=load<u8>(a),y=load<u8>(b);if(x!=y)return false;if(!x)return true;}
}
u32 search_object(u32 actor,u32 param){
 WWHD_FUNC(0x02542A38,u32,actor,param);
 if(!param)return 0;
 u32 profile=call<u32>(0x025C109C,word(param));if(!profile)return 0;
 s16 actual=load<s16>(profile+8),expected=actor?load<s16>(actor+14):0x7fff;
 if(actual!=expected)return 0;
 if(load<s8>(actor+0x2DD)!=load<s8>(profile+10))return 0;
 u32 mask=word(param,4);if(mask&&(word(actor,0xB0)&mask)!=word(param,8))return 0;
 return actor;
}
VERIFY(0x02542A38,search_object);
u32 extra_on(u32 actor,u32 param){
 WWHD_FUNC(0x02542AF4,u32,actor,param);
 if(!param)return 0;
 u32 name=word(param);
 if(!string_equal(name,0x1004DCF0)){
  u32 profile=call<u32>(0x025C109C,name);if(!profile)return 0;
  if(load<s16>(profile+8)!=(actor?load<s16>(actor+14):0x7fff))return 0;
  if(load<s8>(actor+0x2DD)!=load<s8>(profile+10))return 0;
  u32 mask=word(param,4);if(mask&&(word(actor,0xB0)&mask)!=word(param,8))return 0;
 }
 u32 flags=word(actor,0x2E0)|0x800;put(actor,0x2E0,flags);
 if(load<u16>(param+12)&1)put(actor,0x2E0,flags|0x8000);
 return 0;
}
VERIFY(0x02542AF4,extra_on);
u32 extra_off(u32 actor,u32 name){
 WWHD_FUNC(0x02542BE0,u32,actor,name);
 if(string_equal(name,0x1004DCF4)){put(actor,0x2E0,word(actor,0x2E0)&0xffff77ff);return 0;}
 u32 profile=call<u32>(0x025C109C,name);if(!profile)return 0;
 if(load<s16>(profile+8)!=(actor?load<s16>(actor+14):0x7fff))return 0;
 if(load<s8>(actor+0x2DD)!=load<s8>(profile+10))return 0;
 put(actor,0x2E0,word(actor,0x2E0)&0xfffff7ff);return 0;
}
VERIFY(0x02542BE0,extra_off);
u32 all_off(u32 actor){
 WWHD_FUNC(0x02542C9C,u32,actor);put(actor,0x2E0,word(actor,0x2E0)&0xffff67ff);return 0;
}
VERIFY(0x02542C9C,all_off);
u32 find_shutter(u32 actor,u32 param){
 WWHD_FUNC(0x02542CB8,u32,actor,param);
 if(load<s16>(param)!=(actor?load<s16>(actor+14):0x7fff))return 0;
 Local<u8[12]> delta;
 call<void>(0x0201ADE0,actor+0x2EC,delta.get(),word(param,4)+0x2EC);
 f32 x=load<f32>(delta.a),y=load<f32>(delta.a+4),z=load<f32>(delta.a+8),hi=load<f32>(0x1004DCF8);
 if(!(x<hi))return 0;f32 lo=load<f32>(0x1004DCFC);
 if(!(x>lo)||!(y<hi)||!(y>lo)||!(z<hi)||!(z>lo))return 0;
 return actor;
}
VERIFY(0x02542CB8,find_shutter);

u32 start_check(u32 self,s32 id){
 WWHD_FUNC(0x0254407C,u32,self,id);u32 e=call<u32>(0x02544044,self,id);return e&&word(e,0xA4)==2;
}
VERIFY(0x0254407C,start_check);
u32 end_check(u32 self,s32 id){
 WWHD_FUNC(0x025440C8,u32,self,id);u32 e=call<u32>(0x02544044,self,id);return e&&word(e,0xA4)==4;
}
VERIFY(0x025440C8,end_check);
void run_proc(u32 self){
 WWHD_FUNC(0x02544374,void,self);call<void>(0x02544114,self);u32 play=call<u32>(0x025200D4);call<void>(0x0253FF34,play+0x51D0);call<void>(0x02543D80,self);
}
VERIFY(0x02544374,run_proc);
void set_data(u32 self,u32 data){
 WWHD_FUNC(0x025443B4,void,self,data);if(!data)return;put(self,0,data);u32 header=data;
 for(u32 i=0;i<7;++i){
  if(load<s32>(header+i*8+4)>0){u32 off=word(header,i*8);if(i<6)header=word(self);put(self,(i+1)*4,data+off);}
 }
}
VERIFY(0x025443B4,set_data);
u32 create(u32 self){
 WWHD_FUNC(0x02544488,u32,self);call<void>(0x0254273C,self);put(self,0x20,0);call<void>(0x0254351C,self+0x24);call<void>(0x025408CC,self+0x3C);
 u32 data=call<u32>(0x0252447C,0x1004DF68u,0x1004DF70u);call<void>(0x025443B4,self,data);return 1;
}
VERIFY(0x02544488,create);
void remove(u32 self){
 WWHD_FUNC(0x025444F0,void,self);call<void>(0x0254273C,self);put(self,0x20,0);call<void>(0x0254351C,self+0x24);call<void>(0x025408CC,self+0x3C);
}
VERIFY(0x025444F0,remove);
u32 substance_ptr(u32 self,s32 staff,u32 name,s32 type){
 WWHD_FUNC(0x0254487C,u32,self,staff,name,type);u32 data=call<u32>(0x02543944,self,staff,name,0u);return data?call<u32>(0x025439E0,self,data,type):0;
}
VERIFY(0x0254487C,substance_ptr);
u32 substance_num(u32 self,s32 staff,u32 name){
 WWHD_FUNC(0x025448CC,u32,self,staff,name);u32 data=call<u32>(0x02543944,self,staff,name,0u);return data?word(data,0x2C):0;
}
VERIFY(0x025448CC,substance_num);

void cut_end(u32 self,s32 staff){
 WWHD_FUNC(0x02543280,void,self,staff);if(staff==-1)return;
 u32 entry=word(self,8)+u32(staff)*80;u32 cut=word(self,12)+word(entry,0x38)*80;
 if(cut)call<void>(0x02540848,self+0x3C,word(cut,0x34));
}
VERIFY(0x02543280,cut_end);
u32 data_by_name(u32 self,s32 staff,u32 name,u32 initial){
 WWHD_FUNC(0x02543944,u32,self,staff,name,initial);if(staff==-1)return 0;
 u32 staff_base=word(self,8),cut_base=word(self,12),entry=staff_base+u32(staff)*80;
 u32 cut=cut_base+word(entry,initial?0x30:0x38)*80,index=word(cut,0x38);
 if(index==0xffffffff)return 0;u32 data_base=word(self,16);
 for(;;){u32 data=data_base+(index<<6);if(string_equal(name,data))return data;
  index=word(data,0x30);if(index==0xffffffff)return 0;}
}
VERIFY(0x02543944,data_by_name);
u32 substance(u32 self,u32 data,s32 requested){
 WWHD_FUNC(0x025439E0,u32,self,data,requested);
 if(load<s32>(data+0x28)<0||load<s32>(data+0x2C)<=0){call<void>(0x0273AA24,0x1004DED4u,0x18Cu,0x1004DED0u);return 0;}
 u32 type;
 if(requested!=-1){type=word(data,0x24);if(type!=u32(requested)){call<void>(0x0273AA24,0x1004DED4u,0x191u,0x1004DED0u);type=word(data,0x24);}}
 else type=word(data,0x24);
 if(type<3){u32 base=word(self,0x14);return base+(word(data,0x28)<<2);}
 if(type==3){u32 base=word(self,0x18);return base+(word(data,0x28)<<2);}
 if(type==4){u32 base=word(self,0x1C);return base+word(data,0x28);}
 call<void>(0x0273AA24,0x1004DED4u,0x1A5u,0x1004DED0u);return 0;
}
VERIFY(0x025439E0,substance);
u32 special_cast(u32 self,u32 name,u32 move){
 WWHD_FUNC(0x02543838,u32,self,name,move);u32 actor=0;
 if(!name||!string_equal(name,0x1004DEC0))return 0;
 for(u32 type:{0x12Cu,0x12Du,0x130u,0x131u}){actor=call<u32>(0x02543730,self,type,move);if(actor)break;}
 if(actor){u32 play=call<u32>(0x025200D4);store<u16>(play+0x52B8,load<u16>(play+0x52B8)|0x10);
  u32 flags=word(actor,0x2E0);put(actor,0x2E0,move?flags|0x1000:flags&0xffffefff);}
 return actor;
}
VERIFY(0x02543838,special_cast);
u32 issue_staff(u32 self,u32 name){
 WWHD_FUNC(0x02544908,u32,self,name);Local<u8[16]> param;
 call<void>(0x02543638,param.get(),name,0u,0u);
 return call<u32>(0x025D5218,0x02542AF4u,param.get());
}
VERIFY(0x02544908,issue_staff);

void set_param_staff(u32 self,u32 param,s32 staff){
 WWHD_FUNC(0x02543B38,void,self,param,staff);
 u32 entry=word(self,8)+u32(staff)*80;
 if(!entry)call<void>(0x0273AA24,0x1004DEFCu,0x72Du,0x1004DF10u);
 if(!param)call<void>(0x0273AA24,0x1004DEFCu,0x72Eu,0x1004DEF0u);
 put(param,0,entry);
 u32 data=call<u32>(0x02543944,self,staff,0x1004DF1Cu,1u);
 if(!data){put(param,4,0);put(param,8,0);store<u16>(param+12,0);}
 else{u32 values=call<u32>(0x025439E0,self,data,3u);
  if(!values)call<void>(0x0273AA24,0x1004DEFCu,0x73Au,0x1004DEF4u);
  put(param,4,word(values));u32 second=word(values,4);store<u16>(param+12,0);put(param,8,second);}
 if(call<u32>(0x02543944,self,staff,0x1004DEE8u,1u))store<u16>(param+12,1);
}
VERIFY(0x02543B38,set_param_staff);
void start_proc(u32 self,u32 event){
 WWHD_FUNC(0x02543C6C,void,self,event);Local<u8[16]> param;
 call<void>(0x02543638,param.get(),0u,0u,0u);
 for(u32 i=0;s32(i)<load<s32>(event+0x7C);++i){u32 staff=word(event,0x2C+i*4),entry=word(self,8)+staff*80,kind=word(entry,0x2C);
  if(kind==0){u32 actor=call<u32>(0x02543838,self,entry,1u);
   if(!actor){call<void>(0x02543B38,self,param.get(),staff);actor=call<u32>(0x025D5218,0x02542A38u,param.get());}
   if(actor)put(actor,0x2E0,word(actor,0x2E0)|0x8000);
   kind=word(entry,0x2C);}
  if(kind==1){call<void>(0x02543B38,self,param.get(),staff);call<void>(0x025D5218,0x02542AF4u,param.get());}
  call<void>(0x025409B4,entry);
 }
 put(event,0xA4,2);call<void>(0x025408CC,self+0x3C);
}
VERIFY(0x02543C6C,start_proc);
void main_proc(u32 self){
 WWHD_FUNC(0x02543D80,void,self);u32 header=word(self);if(!header)return;s32 count=load<s32>(header+4);
 for(u32 i=0;s32(i)<count;++i){u32 event=word(self,4)+i*176;
  if(word(event,0xA4)==2){if(call<u32>(0x02542624,event))call<void>(0x02543708,self,event);count=load<s32>(word(self)+4);}}
 for(u32 i=0;s32(i)<count;++i){u32 event=word(self,4)+i*176;if(word(event,0xA4)==1){call<void>(0x02543C6C,self,event);count=load<s32>(word(self)+4);}}
 for(u32 i=0;s32(i)<count;++i){u32 event=word(self,4)+i*176;if(word(event,0xA4)==2){call<void>(0x02542878,self,event);count=load<s32>(word(self)+4);}}
 header=word(self);put(self,0x53C,0);count=load<s32>(header+4);
 for(u32 i=0;s32(i)<count;++i){u32 event=word(self,4)+i*176;if(word(event,0xA4)==2){call<void>(0x025426C4,event,word(self,8));header=word(self);put(self,0x53C,event);count=load<s32>(header+4);}}
}
VERIFY(0x02543D80,main_proc);
s32 event_index(u32 self,u32 name,s32 mapid){
 WWHD_FUNC(0x02543F10,s32,self,name,mapid);
 u32 stage=call<u32>(0x025200D4)+0x5150;u32 map=call_ptr<u32>(word(word(stage),0x1CC),stage);
 u32 header=word(self);if(!header)return -1;
 if(map&&mapid!=255&&load<s32>(map)>mapid)return call<s32>(0x02543F10,self,word(map,4)+u32(mapid)*24+1,255u);
 if(!name)return -1;s32 count=load<s32>(header+4);if(count<=0)return -1;u32 base=word(self,4);
 for(u32 i=0;s32(i)<count;++i)if(string_equal(name,base+i*176))return s16(i);
 return -1;
}
VERIFY(0x02543F10,event_index);
void end_proc(u32 self,s32 id,u32 force){
 WWHD_FUNC(0x0254465C,void,self,id,force);u32 event=call<u32>(0x02544044,self,id);
 if(!event){call<void>(0x0273AA24,0x1004DF88u,0x388u,0x1004DF84u);return;}
 if(force)call<void>(0x02543708,self,event);
 if(word(event,0xA4)==4){call<void>(0x025D5218,0x02542C9Cu,0x1004DF80u);put(self,0x20,0);put(event,0xA4,0);}
}
VERIFY(0x0254465C,end_proc);
u32 tool_id(u32 self,s32 id,s32 target){
 WWHD_FUNC(0x02544708,u32,self,id,target);
 u32 stage=call<u32>(0x025200D4)+0x5150,map=call_ptr<u32>(word(word(stage),0x1CC),stage);
 if(id==255||!map||id>=load<s32>(map)||target<0)return 255;
 u32 base=word(map,4),remaining=u32(target);
 for(u32 i=0;i<u32(target)+1;++i){u32 entry=base+u32(id)*24;u8 match=load<u8>(entry+0x12);
  if((match==255&&remaining==0)||(match!=255&&match==u32(target)))return u32(id);
  id=load<u8>(entry);if(id==255)return 255;--remaining;
 }
 return 255;
}
VERIFY(0x02544708,tool_id);

s32 talk_action(u32 action){
 WWHD_FUNC(0x025432B4,s32,action);u32 play=call<u32>(0x025200D4);
 s32 staff=call<s32>(0x02542D88,play+0x52C4,0x1004DD84u,0u,0u);if(staff==-1)return -1;
 play=call<u32>(0x025200D4);s32 act=call<s32>(0x02542EDC,play+0x52C4,staff,0x101D63C8u,3u,0u,0u);
 if(u32(act)==action){play=call<u32>(0x025200D4);call<void>(0x02543280,play+0x52C4,staff);}return act;
}
VERIFY(0x025432B4,talk_action);
u32 exception_name(u32 self){
 WWHD_FUNC(0x025433C0,u32,self);
 u32 stage=call<u32>(0x025200D4)+0x5150;u32 events=call_ptr<u32>(word(word(stage),0x1CC),stage);
 stage=call<u32>(0x025200D4)+0x5150;u32 info=call_ptr<u32>(word(word(stage),0x15C),stage);u32 index=word(self);
 if(index==0xffffffff)return 0;
 if(index==207){if(!info)call<void>(0x0273AA24,0x1004DDACu,0xADu,0x1004DDD4u);
  if(((word(info,12)>>16)&7)==3)return 0x1004DDC0;
  index=word(self);}
 if(index-201u<13)return word(0x101D60B0+(index<<2));
 if(!events||load<s32>(events)<s32(index))return 0;
 return word(events,4)+index*24+1;
}
VERIFY(0x025433C0,exception_name);
s32 set_start_demo(u32 self,s32 index){
 WWHD_FUNC(0x02543534,s32,self,index);u32 stage=call<u32>(0x025200D4)+0x5150;
 u32 events=call_ptr<u32>(word(word(stage),0x1CC),stage);
 if(index==255){put(self,0,206);return 255;}
 if(index<200){if(!events||index==-1||load<s32>(events)<index)return 255;
  u32 flag=load<u8>(word(events,4)+u32(index)*24+0x13);
  if(flag!=255){u32 save=word(0x101F84DC)+0x20;s32 room=load<s8>(0x1047E6C8);
   if(call<u32>(0x025BA0C0,save,flag,room)){put(self,0,206);return 255;}
   save=word(0x101F84DC)+0x20;room=load<s8>(0x1047E6C8);call<void>(0x025B9E38,save,flag,room);}}
 put(self,0,u32(index));return index;
}
VERIFY(0x02543534,set_start_demo);
u32 cast_shutter(u32 self,s32 profile,u32 move){
 WWHD_FUNC(0x02543730,u32,self,profile,move);Local<u8[20]> storage;u32 param=storage.a+12;
 store<u16>(param,u16(profile));u32 play=call<u32>(0x025200D4),player=word(play,0x5B2C);put(param,4,player);
 if(!player)call<void>(0x0273AA24,0x1004DEACu,0x6E5u,0x1004DEA8u);
 u32 actor=call<u32>(0x025D5218,0x02542CB8u,param);
 if(actor&&move){store<f32>(storage.a,load<f32>(actor+0x2EC));store<f32>(storage.a+4,load<f32>(actor+0x2F0));player=word(param,4);store<f32>(storage.a+8,load<f32>(actor+0x2F4));
  u32 angle=u16(load<s16>(player+0x2FA)+0x8000);u32 table=0x104A44F8+(angle&0xfff8);
  f32 scale=load<f32>(0x1004DEA4),sin=load<f32>(table),cos=load<f32>(table+4);
  store<f32>(storage.a,fmadds(sin,scale,load<f32>(storage.a)));store<f32>(storage.a+8,fmadds(cos,scale,load<f32>(storage.a+8)));
  play=call<u32>(0x025200D4);call<void>(0x02543714,play+0x52C4,storage.get());
  u32 control=call<u32>(0x025200D4)+0x51D0;u32 id=call<u32>(0x0253F124,control,actor);put(control,0xC8,id);
 }return actor;
}
VERIFY(0x02543730,cast_shutter);

s32 staff_id(u32 self,u32 name,u32 actor,u32 id){
 WWHD_FUNC(0x02542D88,s32,self,name,actor,id);u32 play=call<u32>(0x025200D4);
 if(!load<u8>(play+0x5292))return -1;u32 header=word(self);if(!header)return -1;
 if(actor&&!(word(actor,0x2E0)&0x8000))return -1;
 s32 count=load<s32>(header+4);if(count<=0)return -1;u32 events=word(self,4);
 for(u32 i=0;s32(i)<count;++i){u32 event=events+i*176;
  if(word(event,0xA4)-2u>2)continue;s32 slots=load<s32>(event+0x7C);if(slots<=0)continue;u32 base=word(self,8);
  for(u32 j=0;s32(j)<slots;++j){u32 index=word(event,0x2C+j*4),staff=base+index*80;
   if(word(staff,0x2C)!=1&&string_equal(name,staff)&&word(staff,0x20)==id)return s32(index);}}
 return -1;
}
VERIFY(0x02542D88,staff_id);
void exception_proc(u32 self){
 WWHD_FUNC(0x02544114,void,self);u32 name=call<u32>(0x025433C0,self+0x24);s32 id=call<s32>(0x02543F10,self,name,255u);
 if(id==-1){put(self,0x24,0xffffffff);return;}
 s32 index=load<s32>(self+0x24);u32 tool=index<200?u8(index):255,state=word(self,0x2C);
 if(state==0){put(self,0x2C,1);call<void>(0x025D7A58,0u,id,tool,65535u,0u,1u);return;}
 if(state==1){
  if(!name)call<void>(0x0273AA24,0x1004DF2Cu,0x61Eu,0x1004DF40u);
  if(call<u32>(0x0254407C,self,id)){
   put(self,0x2C,2);
   if(string_equal(name,0x1004DF4C))call<void>(0x025B8B68,word(0x101F84DC)+0x644,0xF80u);
   if(string_equal(name,0x1004DF5C))call<void>(0x025B8B68,word(0x101F84DC)+0x644,0x280u);
  }else call<void>(0x025D77DC,0u,name,1u,65535u);
 }else if(state==2&&call<u32>(0x025440C8,self,id)){
  u32 play=call<u32>(0x025200D4);store<u16>(play+0x52B8,load<u16>(play+0x52B8)|8);put(self,0x2C,0);put(self,0x24,0xffffffff);
 }
}
VERIFY(0x02544114,exception_proc);

static bool bounded_equal(u32 a,u32 b){
 if(a==b)return true;for(u32 i=0;i<0x40001;++i){u8 x=load<u8>(a+i),y=load<u8>(b+i);if(x!=y)return false;if(!x)return true;}return false;
}
static u32 string_length(u32 p){u32 n=0;while(load<u8>(p+n))++n;return n;}
s32 act_index(u32 self,s32 staff,u32 names,s32 count,u32 force,u32 prefix){
 WWHD_FUNC(0x02542EDC,s32,self,staff,names,count,force,prefix);if(staff==-1)return -1;
 u32 entry=word(self,8)+u32(staff)*80;bool reuse=load<u8>(entry+0x47)&&!force;
 if(!reuse){u32 play=call<u32>(0x025200D4);reuse=false;
  if(load<s8>(play+0x514C)){
   Local<u8[32]> objects;u32 a=objects.a,b=a+8,c=a+16,d=a+24;
   put(c,0,entry);put(c,4,0x1004DD18);put(a,0,0x1004DD44);put(a,4,0x1004DD18);
   call_ptr<void>(0x02544AB8,a);call_ptr<void>(word(word(a,4),0x14),a);
   u32 left=word(a);call_ptr<void>(word(word(c,4),0x14),c);u32 right=word(c);
   if(bounded_equal(left,right)){
    put(b,4,0x1004DD18);put(b,0,0x1004DD6C);u32 current=call<u32>(0x02542E94,self);put(d,0,current);put(d,4,0x1004DD18);
    call_ptr<void>(0x02544AB8,b);call_ptr<void>(word(word(b,4),0x14),b);left=word(b);
    call_ptr<void>(word(word(d,4),0x14),d);right=word(d);
    reuse=bounded_equal(left,right);
   }
  }
 }
 if(reuse)return s32(word(entry,0x3C));
 u32 cut=call<u32>(0x02542EAC,self,staff);if(!cut)return -1;store<u8>(entry+0x47,1);
 for(u32 i=0;s32(i)<count;++i){u32 p=word(names+i*4);
  if(!p){call<void>(0x0273AA24,0x1004DD4Cu,0x515u,0x1004DD60u);p=word(names+i*4);}
  bool equal;
  if(prefix){u32 a=string_length(p),b=string_length(cut);if(a>=100||b>=100){call<void>(0x0273AA24,0x1004DD30u,0x4EDu,0x1004DD14u);continue;}
   equal=a<=b;for(u32 j=0;equal&&j<a;++j)if(load<u8>(p+j)!=load<u8>(cut+j))equal=false;
  }else equal=string_equal(p,cut);
  if(equal){put(entry,0x3C,i);return s32(i);}
 }
 put(entry,0x3C,0xffffffff);return -1;
}
VERIFY(0x02542EDC,act_index);
}
