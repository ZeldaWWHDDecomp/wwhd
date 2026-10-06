/* Message trigger tag, audited WWHD translation unit. */
#include "d/actor/d_a_tag_msg.h"
u16 daTag_Msg_c::getMessage() {
 WWHD_FUNC(0x024B1380,u16,this);
 return (u16)home.angle.x;
}
VERIFY(0x024B1380,&daTag_Msg_c::getMessage);
u32 daTag_Msg_c::getType2() {
 WWHD_FUNC(0x024B1388,u32,this);
 return ((u32)mParameters>>6)&3;
}
VERIFY(0x024B1388,&daTag_Msg_c::getType2);
static s32 action_event(daTag_Msg_c* actor) {
 WWHD_FUNC(0x024B1394,s32,actor);
 u32 controller=gabi::load<u32>(0x101F4B5C);
 u32 message=actor->getMessage();
 u32 mode=gabi::load<u8>(0x1046E604);
 bool advance=false;
 switch(mode) {
 case 0: {
  u32 play=gabi::call<u32>(0x025200D4);
  s32 camera=gabi::load<s8>(play+0x5B30);
  play=gabi::call<u32>(0x025200D4);
  u32 flags=gabi::load<u32>(play+(u32)(camera*0x34)+0x5B00);
  advance=(flags&4)!=0;
  break;
 }
 case 1: {
  u32 id=gabi::call<u32>(0x025F7DB0,controller,message,gabi::at<cXyz>(gabi::ea(actor)+0x390));
  gabi::store<u32>(0x1046E5F8,id);advance=id!=0xFFFFFFFF;
  break;
 }
 case 2: advance=true;break;
 case 3: advance=gabi::call<u32>(0x025F795C,controller)==6;break;
 case 4: {
  if(gabi::call<u32>(0x025F795C,controller)==14) {
   u32 old=gabi::load<u8>(0x1046E604);gabi::store<u8>(0x1046E604,old+1);
   gabi::call(0x025F74D0,controller,16);
  }
  break;
 }
 }
 if(advance) {u32 old=gabi::load<u8>(0x1046E604);gabi::store<u8>(0x1046E604,old+1);}
 if(gabi::call<u32>(0x025F795C,controller)==18) {
  gabi::call(0x025F74D0,controller,19);
  u32 play=gabi::call<u32>(0x025200D4);
  u16 flags=gabi::load<u16>(play+0x52B8);gabi::store<u16>(play+0x52B8,flags|8);
  if(actor->getMessage()==0x1902) {
   play=gabi::call<u32>(0x025200D4);u32 player=gabi::load<u32>(play+0x5B34);
   u32 state=gabi::load<u32>(player+0x3B8);gabi::store<u32>(player+0x3B8,state&~0x08000000u);
  }
  u32 type=actor->getType2();actor->mAction=(type&1)?0:2;
 }
 return 1;
}
VERIFY(0x024B1394,action_event);
u32 daTag_Msg_c::getSwbit() {
 WWHD_FUNC(0x024B153C,u32,this);
 return ((u32)mParameters>>8)&255;
}
VERIFY(0x024B153C,&daTag_Msg_c::getSwbit);
s32 daTag_Msg_c::rangeCheck() {
 WWHD_FUNC(0x024B1548,s32,this);
 u32 play=gabi::call<u32>(0x025200D4);u32 player=gabi::load<u32>(play+0x5B34);
 gabi::Local<cXyz> diff,flat;
 gabi::call(0x0201ADE0,gabi::at<cXyz>(player+0x314),diff.get(),&current.pos);
 f32 height=diff->y;f32 x=diff->x,z=diff->z;
 if(height<0.0f)height=-height;
 flat->y=0.0f;flat->z=z;flat->x=x;
 f32 horizontal=gabi::call<f32>(0x028E8DD0,flat.get());
 f32 sx=scale.x;sx=sx*sx;f32 radius=sx*10000.0f;
 if(!(horizontal<radius))return 0;
 f32 sy=scale.y;f32 top=sy*100.0f;
 if(height>top)return 0;
 return 1;
}
VERIFY(0x024B1548,&daTag_Msg_c::rangeCheck);
s32 daTag_Msg_c::otherCheck() {
 WWHD_FUNC(0x024B1630,s32,this);
 u32 play=gabi::call<u32>(0x025200D4);u32 target=gabi::load<u32>(play+0x5B2C);
 s32 angle=gabi::call<s32>(0x025D6894,this,gabi::at<fopAc_ac_c>(target));
 play=gabi::call<u32>(0x025200D4);u32 player=gabi::load<u32>(play+0x5B34);
 if(getType2()&1)return 1;
 if(!player)JUT_ASSERT_fail(STR(0x1003FB70),0xCC,STR(0x1003FB68));
 if(getMessage()==0x1902) {
  s32 diff=(s16)(angle-(s16)home.angle.y);
  if(diff<0)diff=(s16)-diff;
  if(diff>0x3000)return 0;
 }
 s32 diff=(s16)(angle+0x7FFF-gabi::load<s16>(player+0x322));
 if(diff<0)diff=(s16)-diff;
 if(diff>0x1000)return 0;
 return 1;
}
VERIFY(0x024B1630,&daTag_Msg_c::otherCheck);
static s32 action_hunt(daTag_Msg_c* actor) {
 WWHD_FUNC(0x024B1730,s32,actor);
 u32 self=gabi::ea(actor);
 if(gabi::load<u16>(self+0xF8)==1) {
  actor->mAction=3;u32 sw=actor->getSwbit();
  if(sw!=255) {s32 room=(s8)actor->current.roomNo;u32 save=gabi::load<u32>(0x101F84DC);gabi::call(0x025B9E38,gabi::at<u8>(save+0x20),sw,room);}
  gabi::store<u32>(0x1046E5F8,0xFFFFFFFF);gabi::store<u8>(0x1046E604,0);
  if(actor->getMessage()==0x1902) {
   u32 play=gabi::call<u32>(0x025200D4);u32 player=gabi::load<u32>(play+0x5B34);
   u32 flags=gabi::load<u32>(player+0x3B8);gabi::store<u32>(player+0x3B8,flags|0x08000000);
  }
 } else if(actor->rangeCheck() && actor->otherCheck()) {
  if(actor->getType2()&1)gabi::call(0x025D76A8,actor);
  u16 condition=gabi::load<u16>(self+0xFA);gabi::store<u16>(self+0xFA,condition|1);
 }
 return 1;
}
VERIFY(0x024B1730,action_hunt);
u32 daTag_Msg_c::getSwbit2() {
 WWHD_FUNC(0x024B181C,u32,this);
 return ((u32)mParameters>>16)&255;
}
VERIFY(0x024B181C,&daTag_Msg_c::getSwbit2);
u16 daTag_Msg_c::getEventFlag() {
 WWHD_FUNC(0x024B1828,u16,this);
 return (u16)home.angle.z;
}
VERIFY(0x024B1828,&daTag_Msg_c::getEventFlag);
s32 daTag_Msg_c::arrivalTerms() {
 WWHD_FUNC(0x024B1830,s32,this);
 u32 sw=getSwbit2();u32 flag=getEventFlag();
 if(sw!=255) {
  u32 save=gabi::load<u32>(0x101F84DC);s32 room=(s8)current.roomNo;
  if(!gabi::call<s32>(0x025BA0C0,gabi::at<u8>(save+0x20),sw,room))return 0;
 }
 if(flag!=65535) {
  u32 save=gabi::load<u32>(0x101F84DC);
  if(!gabi::call<s32>(0x025B8B94,gabi::at<u8>(save+0x644),flag))return 0;
 }
 return 1;
}
VERIFY(0x024B1830,&daTag_Msg_c::arrivalTerms);
u32 daTag_Msg_c::getEventNo() {
 WWHD_FUNC(0x024B18DC,u32,this);
 return (u32)mParameters>>24;
}
VERIFY(0x024B18DC,&daTag_Msg_c::getEventNo);
const char* daTag_Msg_c::myDemoName() {
 WWHD_FUNC(0x024B18E8,const char*,this);
 u32 play=gabi::call<u32>(0x025200D4);u32 stage=play+0x5150;
 u32 table=gabi::load<u32>(stage);u32 getter=gabi::load<u32>(table+0x1CC);
 u32 info=gabi::call_ptr<u32>(getter,gabi::at<u8>(stage));
 u32 event=getEventNo();
 if(getMessage()==0x1902)return STR(0x1003FB80);
 if(!info || event==255 || gabi::load<s32>(info)<(s32)event)return STR(0x1003FB88);
 u32 events=gabi::load<u32>(info+4);
 return STR(events+event*0x18+1);
}
VERIFY(0x024B18E8,&daTag_Msg_c::myDemoName);
static s32 action_arrival(daTag_Msg_c* actor) {
 WWHD_FUNC(0x024B19A8,s32,actor);
 if(actor->arrivalTerms()) {
  const char* name=actor->myDemoName();
  gabi::call(0x0253E9B0,gabi::at<u8>(gabi::ea(actor)+0xF8),name);
  actor->mAction=2;action_hunt(actor);
 }
 return 1;
}
VERIFY(0x024B19A8,action_arrival);
static s32 action_wait(daTag_Msg_c* actor) {WWHD_FUNC(0x024B1A04,s32,actor);return 1;}
VERIFY(0x024B1A04,action_wait);
static s32 execute(daTag_Msg_c* actor) {
 WWHD_FUNC(0x024B1A0C,s32,actor);
 u32 guard=gabi::load<u32>(0x101FDCB8);
 if(!guard) {
  gabi::store<u32>(0x101FDCB8,1);
  gabi::call(0xC000A848,gabi::at<u8>(0x101FDCBC),gabi::at<u8>(0x101D1F24),16);
 }
 u32 action=actor->mAction;u32 fn=gabi::load<u32>(0x101FDCBC+action*4);
 gabi::call_ptr(fn,actor);return 1;
}
VERIFY(0x024B1A0C,execute);
static s32 is_delete(daTag_Msg_c* actor) {WWHD_FUNC(0x024B1A8C,s32,actor);return 1;}
VERIFY(0x024B1A8C,is_delete);
static s32 remove(daTag_Msg_c* actor) {WWHD_FUNC(0x024B1A94,s32,actor);return 1;}
VERIFY(0x024B1A94,remove);
static s32 create(daTag_Msg_c* actor) {
 WWHD_FUNC(0x024B1A9C,s32,actor);
 u32 condition=actor->actor_condition;
 if(!(condition&8)) {
  if(gabi::ea(actor)) {gabi::call(0x025D4ED0,actor);condition=actor->actor_condition;actor->__vtbl=0x1003FB4C;}
  actor->actor_condition=condition|8;
 }
 u32 sw=actor->getSwbit();bool inactive=false;
 if(actor->getMessage()==0x9C5) {u32 save=gabi::load<u32>(0x101F84DC);inactive=gabi::call<s32>(0x025B8B94,gabi::at<u8>(save+0x644),0x502)!=0;}
 if(!inactive && sw!=255) {u32 save=gabi::load<u32>(0x101F84DC);s32 room=(s8)actor->current.roomNo;inactive=gabi::call<s32>(0x025BA0C0,gabi::at<u8>(save+0x20),sw,room)!=0;}
 if(inactive) {
  actor->shape_angle.x=0;actor->current.angle.x=0;actor->shape_angle.z=0;
  gabi::store<u32>(gabi::ea(actor)+0x39C,0x20000008);actor->current.angle.z=0;actor->mAction=0;
 } else {
  actor->current.angle.z=0;actor->mAction=1;actor->shape_angle.z=0;actor->current.angle.x=0;actor->shape_angle.x=0;
  gabi::store<u32>(gabi::ea(actor)+0x39C,0x20000008);
 }
 if(actor->getMessage()!=0x836) {
  u32 self=gabi::ea(actor);f32 attention=gabi::load<f32>(self+0x394),eye=gabi::load<f32>(self+0x380);
  attention=attention+150.0f;eye=eye+150.0f;
  gabi::store<f32>(self+0x394,attention);gabi::store<f32>(self+0x380,eye);
 }
 return 4;
}
VERIFY(0x024B1A9C,create);
static void initialize_globals() {
 WWHD_FUNC(0x024B1BFC,void);
 gabi::store<u32>(0x1046E610,0);gabi::store<u32>(0x1046E608,0);gabi::store<u32>(0x1046E614,0);gabi::store<u32>(0x1046E60C,0);
 gabi::call(0x028F026C,gabi::at<u8>(0x101D1F54));
 gabi::store<f32>(0x1046E5FC,-3.1415927410125732f);gabi::store<f32>(0x1046E600,3.1415927410125732f);
 gabi::call(0x028ED6F8,gabi::at<u8>(0x1046E605));gabi::call(0x028F026C,gabi::at<u8>(0x101D1F60));
 gabi::call(0x028EAB2C,gabi::at<u8>(0x1046E606));gabi::call(0x028F026C,gabi::at<u8>(0x101D1F6C));
}
VERIFY(0x024B1BFC,initialize_globals);
static s32 draw(daTag_Msg_c* actor) {WWHD_FUNC(0x024B1C90,s32,actor);return 1;}
VERIFY(0x024B1C90,draw);
static void actor_destructor(daTag_Msg_c* actor,u32 flags) {
 WWHD_FUNC(0x024B1C98,void,actor,flags);
 if(actor){gabi::call(0x025D50BC,actor,0);if(flags&1)gabi::call(0x0273AF40,actor);}
}
VERIFY(0x024B1C98,actor_destructor);
