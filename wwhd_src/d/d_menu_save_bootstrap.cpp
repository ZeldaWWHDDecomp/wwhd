// Qualified HD SaveMgr bootstrap range;
#include "d_menu_save_local.h"
u32 saveMgrCtor(u32 p){
 WWHD_FUNC(0x027209E0,u32,p);
 if(!p){p=call<u32>(0x0273AD10,0x6Cu);if(!p)return 0;}
 store<u32>(p+0x20,0);store<u32>(p,0x10140840u);store<u32>(p+0x14,0);store<u32>(p+0x24,0);store<u32>(p+0x18,0);store<u32>(p+0x1C,0);
 call<void>(0x02760084,p+0x28);store<u32>(p+0x68,0);store<u32>(p+0x64,0);return p;
}
VERIFY(0x027209E0,saveMgrCtor);
u32 saveMgrGet(u32 heap){
 WWHD_FUNC(0x02720A5C,u32,heap);u32 p=U(0x101F852Cu);if(p)return p;
 u32 base=call<u32>(0x0273B0D4,0x6Cu,heap,4u),thread=base+4;
 if(thread){call<void>(0x02752B0C,thread,heap,3u);store<u32>(thread+0xC,0x10140AA8u);}
 store<u32>(0x101F8530u,thread);p=base;if(p)p=call<u32>(0x027209E0,p);store<u32>(0x101F852Cu,p);return p;
}
VERIFY(0x02720A5C,saveMgrGet);
u32 saveMgrAllocate(u32 size,u32 heap,u32 alignment){
 WWHD_FUNC(0x02720AFC,u32,size,heap,alignment);return call<u32>(0x0273B050,size,heap,alignment);
}
VERIFY(0x02720AFC,saveMgrAllocate);

struct SaveName {be<u32> name,owner;};
void saveMgrInit(u32 p,u32 heap){
 WWHD_FUNC(0x02720B34,void,p,heap);Local<SaveName> name;name->name=0x10140860u;name->owner=0x101407B0u;
 u32 h=call<u32>(0x02753004,0xAF000u,name.get(),heap,1u,0u);
 u32 buffer=call<u32>(0x02720AFC,0x1FBCu,h,0x200u);store<u32>(p+0x64,buffer);call<void>(0xC0009990,buffer,0u,0x1FBCu);
 buffer=call<u32>(0x02720AFC,0x50400u,h,0x200u);store<u32>(p+0x68,buffer);call<void>(0xC0009990,buffer,0u,0x50400u);
 if(!U(0x101F8B08u))call<void>(0x02741A20,U(p,0x14));
 u32 thread=call<u32>(0x0273B050,0x98u,h,4u);
 if(thread){name->name=0x10140868u;u32 record=call<u32>(0x0273B050,0x10u,h,4u);
 if(record){store<u32>(record+4,p);store<u16>(record+8,0);store<u16>(record+10,0xFFFF);store<u32>(record+12,0x02724580u);store<u32>(record,0x10140850u);}
 thread=call<u32>(0x0275FBD8,thread,name.get(),record,h,U(0x101F8B9Cu),0u,0x7FFFFFFFu,0x5000u,0x20u);
 }
 store<u32>(p+0x18,thread);u32 vt=U(thread,12);call_ptr<void>(U(vt,0x2C),thread,cpu->r[4],cpu->r[5],cpu->r[6],cpu->r[7],cpu->r[8],cpu->r[9],vt,cpu->f[1].ps0,cpu->f[2].ps0,cpu->f[3].ps0,cpu->f[4].ps0,cpu->f[5].ps0,cpu->f[6].ps0,cpu->f[7].ps0,cpu->f[8].ps0);
 store<u32>(p+0x14,h);
}
VERIFY(0x02720B34,saveMgrInit);
void saveMgrSync(u32 p,u32 slot){
 WWHD_FUNC(0x02720D34,void,p,slot);u32 state=U(0x101F84DCu)+0x12C0,b,a;
 b=call<u32>(0x027200A0,state,slot);a=call<u32>(0x027200D0,state);call<void>(0x0271FCB4,a,b);
 b=call<u32>(0x027200D8,state,slot);a=call<u32>(0x027200F4,state);call<void>(0x0271FAC0,a,b);
 b=call<u32>(0x027200FC,state,slot);a=call<u32>(0x02720118,state);call<void>(0x0271F914,a,b);
 b=call<u32>(0x02720180,state,slot);a=call<u32>(0x0272019C,state);call<void>(0x027208F4,a,b);
 b=call<u32>(0x02720120,state,slot);a=call<u32>(0x02720144,state);call<void>(0x027262BC,a,b);
 b=call<u32>(0x027201A4,state,slot);a=call<u32>(0x027201DC,state);call<void>(0x02726BC8,a,b);
}
VERIFY(0x02720D34,saveMgrSync);

static u32 saveItemOffset(u32 index){
 if(index<0x15)return 0x5C+index;
 if(index>=0x18&&index<0x20)return 0x7E + index;
 if(index>=0x24&&index<0x2C)return 0x7A+index;
 if(index>=0x30&&index<0x38)return 0x76+index;
 return 0;
}
static u8 saveSelectedItem(u32 state,u32 index){u32 offset=saveItemOffset(index);return offset?load<u8>(state+offset):0xFF;}
static void saveRefreshSelection(u32 selector,u32 gameOffset){
 u32 before=load<u8>(U(0x101F84DCu)+selector);u32 game=call<u32>(0x025200D4)+0x12A0;
 if(before==0xFF){store<u8>(game+gameOffset,0xFF);return;}
 u32 state=U(0x101F84DCu);u32 index=load<u8>(state+selector);store<u8>(game+gameOffset,saveSelectedItem(state,index));
 state=U(0x101F84DCu);index=load<u8>(state+selector);if(saveSelectedItem(state,index)==0xFF)store<u8>(state+selector,0xFF);
}
void saveMgrStartup(u32 p){
 WWHD_FUNC(0x02720E34,void,p);
 for(u32 i=0;i<0x15;i++){
  if(!call<u32>(0x025B5D40,U(0x101F84DCu)+0x71,i,0u))continue;
  if(i==14||i==15||i==16||i==17)continue;
  u32 item=call<u32>(0x025DB5AC,i);store<u8>(U(0x101F84DCu)+0x5C+i,item);
  if(i==2){store<u8>(U(0x101F84DCu)+0x2C,2);saveRefreshSelection(0x2C,0x491E);}
 }
 if(call<u32>(0x025B5D40,U(0x101F84DCu)+0x71,1u,1u)){
  store<u8>(U(0x101F84DCu)+0x5D,0x77);store<u8>(U(0x101F84DCu)+0x2D,1);saveRefreshSelection(0x2D,0x491F);
 }
 if(call<u32>(0x025B5D40,U(0x101F84DCu)+0x71,8u,1u))store<u8>(U(0x101F84DCu)+0x64,0x26);
 if(call<u32>(0x025B5D40,U(0x101F84DCu)+0x71,12u,2u)){
  store<u8>(U(0x101F84DCu)+0x68,0x36);
  for(u32 i=0;i<3;i++){u32 game=call<u32>(0x025200D4);if(load<u8>(game+0x5BBB+i)==0x35)saveRefreshSelection(0x29+i,0x491B+i);}
  return;
 }
 if(call<u32>(0x025B5D40,U(0x101F84DCu)+0x71,12u,1u)){
  store<u8>(U(0x101F84DCu)+0x68,0x35);
  for(u32 i=0;i<3;i++){u32 game=call<u32>(0x025200D4);if(load<u8>(game+0x5BBB+i)==0x27)saveRefreshSelection(0x29+i,0x491B+i);}
 }
}
VERIFY(0x02720E34,saveMgrStartup);
