// Qualified HD SaveMgr platform storage adapters;
#include "d_menu_save_local.h"
struct SaveNameStorage {be<u32> name,owner;};
void saveMgrThreadEvent(u32 p,u32 arg,u32 event){
 WWHD_FUNC(0x02724580,void,p,arg,event);if(event==1)call<void>(0x0272391C,p,arg,event);else if(event==2)call<void>(0x02724180,p,0u,event);else if(event==3)call<void>(0x02724180,p,1u,event);else if(event==4)call<void>(0x027243E0,p,arg,event);
}
VERIFY(0x02724580,saveMgrThreadEvent);
u32 saveMgrCheckSlot(u32 p,u32 data,u32 slot){
 WWHD_FUNC(0x027245BC,u32,p,data,slot);u32 block=data+slot*0xA94u;u32 sum=call<u32>(0x02721AC4,p,block,0xA8Cu),complement=cpu->r[4];return sum==U(block,0xA8C)&&complement==U(block,0xA90);
}
VERIFY(0x027245BC,saveMgrCheckSlot);
void saveMgrTerminateSlotName(u32 p,u32 data,u32 slot){
 WWHD_FUNC(0x02724614,void,p,data,slot);u32 name=data+slot*0xA94u+0x30;u32 temp=call<u32>(0x0273B0D4,8u,U(p,0x14),4u);call<void>(0xC0009988,temp,name,8u,0u);if(load<u8>(temp+7)){store<u8>(temp+7,0);call<void>(0xC0009988,name,temp,8u,0u);}call<void>(0x0273AFC8,temp);
}
VERIFY(0x02724614,saveMgrTerminateSlotName);
u32 saveMgrReadSlots(u32 p,u32 file,u32 count){
 WWHD_FUNC(0x027246A0,u32,p,file,count);u32 result=0;if(U(file,0x10)){result=call<u32>(0x027414CC,file,count,U(p,0x64),0x1FBCu);if(result){for(u32 i=0;i<3;i++){u32 data=U(p,0x64);if(!call<u32>(0x027245BC,p,data,i)){store<u32>(p+0x20,-3);return 0;}call<void>(0x02724614,p,data,i);}}}else store<u32>(p+0x20,-2);return result;
}
VERIFY(0x027246A0,saveMgrReadSlots);
u32 saveMgrReadPlayer(u32 p,u32 file,u32 count){
 WWHD_FUNC(0x02724764,u32,p,file,count);for(u32 i=0;i<3;i++){if(!call<u32>(0x027414CC,file,count,U(p,0x68),16u)){store<u32>(p+0x20,-3);return 0;}u32 dest=call<u32>(0x027200A0,U(0x101F84DCu)+0x12C0,i);call<void>(0xC0009988,dest,U(p,0x68),16u,0u);}return 1;
}
VERIFY(0x02724764,saveMgrReadPlayer);
u32 saveMgrReadStatus(u32 p,u32 file,u32 count){
 WWHD_FUNC(0x0272480C,u32,p,file,count);for(u32 i=0;i<3;i++){if(!call<u32>(0x027414CC,file,count,U(p,0x68),4u)){store<u32>(p+0x20,-3);return 0;}u32 dest=call<u32>(0x027200D8,U(0x101F84DCu)+0x12C0,i);call<void>(0xC0009988,dest,U(p,0x68),4u,0u);}return 1;
}
VERIFY(0x0272480C,saveMgrReadStatus);
u32 saveMgrReadEvent(u32 p,u32 file,u32 count){
 WWHD_FUNC(0x027248B4,u32,p,file,count);for(u32 i=0;i<3;i++){if(!call<u32>(0x027414CC,file,count,U(p,0x68),20u)){store<u32>(p+0x20,-3);return 0;}u32 dest=call<u32>(0x027200FC,U(0x101F84DCu)+0x12C0,i);call<void>(0xC0009988,dest,U(p,0x68),20u,0u);}return 1;
}
VERIFY(0x027248B4,saveMgrReadEvent);
u32 saveMgrReadMap(u32 p,u32 file,u32 count){
 WWHD_FUNC(0x02724A28,u32,p,file,count);for(u32 i=0;i<3;i++){if(!call<u32>(0x027414CC,file,count,U(p,0x68),220u)){store<u32>(p+0x20,-3);return 0;}u32 dest=call<u32>(0x02720180,U(0x101F84DCu)+0x12C0,i);call<void>(0xC0009988,dest,U(p,0x68),220u,0u);}return 1;
}
VERIFY(0x02724A28,saveMgrReadMap);
u32 saveMgrReadName(u32 p,u32 file,u32 count){
 WWHD_FUNC(0x0272495C,u32,p,file,count);for(u32 i=0;i<3;i++){if(!call<u32>(0x027414CC,file,count,U(p,0x68),18u)){store<u32>(p+0x20,-3);return 0;}u32 temp=call<u32>(0x0273B0D4,18u,U(p,0x14),4u);call<void>(0xC0009988,temp,U(p,0x68),18u,0u);u32 dest=call<u32>(0x02720154,U(0x101F84DCu)+0x12C0,i);call<void>(0x02726668,dest,temp);call<void>(0x0273AFC8,temp);}return 1;
}
VERIFY(0x0272495C,saveMgrReadName);
u32 saveMgrCheckSerial(u32 p,u32 file,u32 count){
 WWHD_FUNC(0x02724AD0,u32,p,file,count);if(call<u32>(0x027414CC,file,count,U(p,0x68),4u)){u32 expected=U(U(0x101F84DCu),0x1790);Local<be<u32>> got;*got.get()=0;call<void>(0xC0009988,got.get(),U(p,0x68),4u,0u);if(expected==(u32)*got.get())return 1;}store<u32>(p+0x20,-3);return 0;
}
VERIFY(0x02724AD0,saveMgrCheckSerial);
u32 saveMgrCheckVersion(u32 p,u32 file,u32 count){
 WWHD_FUNC(0x02724B74,u32,p,file,count);if(call<u32>(0x027414CC,file,count,U(p,0x68),4u)){u32 expected=call<u32>(0x027201E4,U(0x101F84DCu)+0x12C0);Local<be<u32>> got;*got.get()=0;call<void>(0xC0009988,got.get(),U(p,0x68),4u,0u);if(expected==(u32)*got.get())return 1;}store<u32>(p+0x20,-3);return 0;
}
VERIFY(0x02724B74,saveMgrCheckVersion);
u32 saveMgrWriteSlots(u32 p,u32 file,u32 count){
 WWHD_FUNC(0x02724C20,u32,p,file,count);if(!U(file,0x10)){store<u32>(p+0x24,-1);return 0;}u32 result=call<u32>(0x02741558,file,count,U(p,0x64),0x1FBCu);return U(count)==0x1FBC?result:0;
}
VERIFY(0x02724C20,saveMgrWriteSlots);
u32 saveMgrWritePlayer(u32 p,u32 file,u32 count){
 WWHD_FUNC(0x02724C9C,u32,p,file,count);for(u32 i=0;i<3;i++){u32 state=U(0x101F84DCu),slot=load<u8>(state+0x12B0);u32 dest=call<u32>(0x027200A0,state+0x12C0,i);call<void>(0x0271FCAC,dest,slot);u32 stateNow=U(0x101F84DCu),buffer=U(p,0x68);u32 src=call<u32>(0x027200A0,stateNow+0x12C0,i);call<void>(0xC0009988,buffer,src,16u,0u);if(!call<u32>(0x02741558,file,count,U(p,0x68),16u)||U(count)!=16u){store<u32>(p+0x24,-2);return 0;}}return 1;
}
VERIFY(0x02724C9C,saveMgrWritePlayer);
u32 saveMgrWriteStatus(u32 p,u32 file,u32 count){
 WWHD_FUNC(0x02724D74,u32,p,file,count);for(u32 i=0;i<3;i++){u32 stateNow=U(0x101F84DCu),buffer=U(p,0x68);u32 src=call<u32>(0x027200D8,stateNow+0x12C0,i);call<void>(0xC0009988,buffer,src,4u,0u);if(!call<u32>(0x02741558,file,count,U(p,0x68),4u)||U(count)!=4u){store<u32>(p+0x24,-2);return 0;}}return 1;
}
VERIFY(0x02724D74,saveMgrWriteStatus);
u32 saveMgrWriteEvent(u32 p,u32 file,u32 count){
 WWHD_FUNC(0x02724E30,u32,p,file,count);for(u32 i=0;i<3;i++){u32 stateNow=U(0x101F84DCu),buffer=U(p,0x68);u32 src=call<u32>(0x027200FC,stateNow+0x12C0,i);call<void>(0xC0009988,buffer,src,20u,0u);if(!call<u32>(0x02741558,file,count,U(p,0x68),20u)||U(count)!=20u){store<u32>(p+0x24,-2);return 0;}}return 1;
}
VERIFY(0x02724E30,saveMgrWriteEvent);
u32 saveMgrWriteMap(u32 p,u32 file,u32 count){
 WWHD_FUNC(0x02725018,u32,p,file,count);for(u32 i=0;i<3;i++){u32 stateNow=U(0x101F84DCu),buffer=U(p,0x68);u32 src=call<u32>(0x02720180,stateNow+0x12C0,i);call<void>(0xC0009988,buffer,src,220u,0u);if(!call<u32>(0x02741558,file,count,U(p,0x68),220u)){store<u32>(p+0x24,-2);return 0;}}return 1;
}
VERIFY(0x02725018,saveMgrWriteMap);
u32 saveMgrWriteSerial(u32 p,u32 file,u32 count){
 WWHD_FUNC(0x027250C8,u32,p,file,count);call<void>(0xC0009988,U(p,0x68),U(0x101F84DCu)+0x1790,4u,0u);if(!call<u32>(0x02741558,file,count,U(p,0x68),4u)){store<u32>(p+0x24,-2);return 0;}return 1;
}
VERIFY(0x027250C8,saveMgrWriteSerial);
u32 saveMgrWriteVersion(u32 p,u32 file,u32 count){
 WWHD_FUNC(0x0272516C,u32,p,file,count);call<void>(0x0272033C,U(0x101F84DCu)+0x12C0);call<void>(0xC0009988,U(p,0x68),U(0x101F84DCu)+0x1794,4u,0u);if(!call<u32>(0x02741558,file,count,U(p,0x68),4u)){store<u32>(p+0x24,-2);return 0;}return 1;
}
VERIFY(0x0272516C,saveMgrWriteVersion);
void saveMgrDisposerDtor(u32 p,u32 flags){
 WWHD_FUNC(0x02725228,void,p,flags);if(!p)return;store<u32>(p+12,0x10140AA8u);if(p==U(0x101F8530u)){u32 manager=U(0x101F852Cu);store<u32>(0x101F8530u,0);if(manager){store<u32>(manager,0x10140840u);call<void>(0x02760168,manager+0x28,2u);}store<u32>(0x101F852Cu,0);}call<void>(0x02752BEC,p,0u);if(flags&1)call<void>(0x0273AF40,p);
}
VERIFY(0x02725228,saveMgrDisposerDtor);

static void saveStorageNameVcall(u32 object,u32 arg5,u32 arg6,u32 arg7,u32 arg9,u32 arg10){
 u32 target=U(U(object,4),0x14);call_ptr<void>(target,object,cpu->r[4],arg5,arg6,arg7,cpu->r[8],arg9,arg10,cpu->f[1].ps0,cpu->f[2].ps0,cpu->f[3].ps0,cpu->f[4].ps0,cpu->f[5].ps0,cpu->f[6].ps0,cpu->f[7].ps0,cpu->f[8].ps0);
}
u32 saveMgrWriteName(u32 p,u32 file,u32 count){
 WWHD_FUNC(0x02724EEC,u32,p,file,count);
 for(u32 i=0;i<3;i++){
  u32 firstState=U(0x101F84DCu);u32 object=call<u32>(0x02720154,firstState+0x12C0,i);saveStorageNameVcall(object,cpu->r[5],cpu->r[6],cpu->r[7],cpu->r[9],firstState);
  u32 text=U(object),length=0,last=cpu->r[6];
  if(load<u16>(text)){do{length++;text+=2;if(length>0x40000){length=0;break;}}while((last=load<u16>(text))); }
  u32 state=U(0x101F84DCu),buffer=U(p,0x68),bytes=(length+1)*2;
  object=call<u32>(0x02720154,state+0x12C0,i);saveStorageNameVcall(object,text,last,0x40000u,state,U(object,4));
  call<void>(0xC0009988,buffer,U(object),bytes,0u);
  if(!call<u32>(0x02741558,file,count,U(p,0x68),18u)){store<u32>(p+0x24,-2);return 0;}
 }
 return 1;
}
VERIFY(0x02724EEC,saveMgrWriteName);
void saveMgrStaticInit(){
 WWHD_FUNC(0x027252DC,void);
 store<u32>(0x1049F7C8u,0);store<u32>(0x1049F7C4u,0);store<u32>(0x1049F7C0u,0);store<u32>(0x1049F7BCu,0);
 call<void>(0x028F026C,0x101F8508u);
 u32 negative=U(0x10140A98u),positive=U(0x10140A9Cu);store<u32>(0x1049F7A0u,negative);store<u32>(0x1049F7A4u,positive);
 call<void>(0x028ED6F8,0x1049F7B8u);call<void>(0x028F026C,0x101F8514u);call<void>(0x028EAB2C,0x1049F7B9u);call<void>(0x028F026C,0x101F8520u);
 u32 far=U(0x10140AA0u),near=U(0x10140AA4u);store<u32>(0x1049F7A8u,far);store<u32>(0x1049F7B0u,near);store<u32>(0x1049F7ACu,far);store<u32>(0x1049F7B4u,near);
}
VERIFY(0x027252DC,saveMgrStaticInit);
void saveMgrSelectSlot(u32 p,u32 slot){
 WWHD_FUNC(0x027222E4,void,p,slot);store<u8>(U(0x101F84DCu)+0x12B0,slot);
}
VERIFY(0x027222E4,saveMgrSelectSlot);
void saveMgrResetSlot(u32 p,u32 slot){
 WWHD_FUNC(0x027225AC,void,p,slot);call<void>(0x027222E4,p,slot);call<void>(0x027219AC,cpu->r[3],cpu->r[4]);call<void>(0x02721908,p,slot,0u);call<void>(0x02721C44,p,slot,0u);call<void>(0x02721D18,p);
}
VERIFY(0x027225AC,saveMgrResetSlot);
void saveMgrClearStage(u32 p,u32 slot){
 WWHD_FUNC(0x02722610,void,p,slot);u32 stage=call<u32>(0x02720120,U(0x101F84DCu)+0x12C0,slot);call<void>(0x02725DA8,stage);
}
VERIFY(0x02722610,saveMgrClearStage);
void saveMgrClearExtra(u32 p){
 WWHD_FUNC(0x02722640,void,p);call<void>(0x02725574,U(0x101F84DCu)+0xF026C0); 
}
VERIFY(0x02722640,saveMgrClearExtra);
void saveMgrClearPictures(u32 p){
 WWHD_FUNC(0x02722654,void,p);for(u32 i=0;i<3;i++){u32 image=call<u32>(0x027201A4,U(0x101F84DCu)+0x12C0,i);call<void>(0x02726B30,image);}
}
VERIFY(0x02722654,saveMgrClearPictures);
void saveMgrClearAll(u32 p){
 WWHD_FUNC(0x027226B4,void,p);for(u32 i=0;i<3;i++){call<void>(0x027219AC,p,i);call<void>(0x02721AFC,p,U(p,0x64),i);call<void>(0x02722610,p,i);}call<void>(0x02722640,p);call<void>(0x02722654,p);
}
VERIFY(0x027226B4,saveMgrClearAll);
void saveMgrQueueCreate(u32 p){
 WWHD_FUNC(0x02722738,void,p);if(U(p,0x1C))return;u32 thread=U(p,0x18);store<u32>(p+0x1C,2);call_ptr<void>(U(U(thread,12),0x1C),thread,3u,1u,cpu->r[6],cpu->r[7],cpu->r[8],cpu->r[9],cpu->r[10],cpu->f[1].ps0,cpu->f[2].ps0,cpu->f[3].ps0,cpu->f[4].ps0,cpu->f[5].ps0,cpu->f[6].ps0,cpu->f[7].ps0,cpu->f[8].ps0);
}
VERIFY(0x02722738,saveMgrQueueCreate);
void saveMgrQueueDelete(u32 p){
 WWHD_FUNC(0x0272294C,void,p);if(U(p,0x1C))return;u32 thread=U(p,0x18);store<u32>(p+0x1C,3);call_ptr<void>(U(U(thread,12),0x1C),thread,4u,1u,cpu->r[6],cpu->r[7],cpu->r[8],cpu->r[9],cpu->r[10],cpu->f[1].ps0,cpu->f[2].ps0,cpu->f[3].ps0,cpu->f[4].ps0,cpu->f[5].ps0,cpu->f[6].ps0,cpu->f[7].ps0,cpu->f[8].ps0);
}
VERIFY(0x0272294C,saveMgrQueueDelete);
void saveMgrCreateCard(u32 p){
 WWHD_FUNC(0x0272276C,void,p);call<void>(0x027226B4,p);store<u32>(p+0x20,0);call<void>(0x02722738,p);
}
VERIFY(0x0272276C,saveMgrCreateCard);
void saveMgrCopySlot(u32 p,u32 to,u32 from){
 WWHD_FUNC(0x027227A8,void,p,to,from);u32 src=call<u32>(0x02720154,U(0x101F84DCu)+0x12C0,from);u32 dest=call<u32>(0x02720154,U(0x101F84DCu)+0x12C0,to);call<void>(0x027267FC,dest,src);call<void>(0x02721908,p,from,0u);u32 stage=call<u32>(0x02720144,U(0x101F84DCu)+0x12C0);call<void>(0x02725FD0,stage);src=call<u32>(0x027201A4,U(0x101F84DCu)+0x12C0,to);dest=call<u32>(0x027201DC,U(0x101F84DCu)+0x12C0);call<void>(0x02726BC8,dest,src);src=call<u32>(0x027201A4,U(0x101F84DCu)+0x12C0,from);dest=call<u32>(0x027201DC,U(0x101F84DCu)+0x12C0);call<void>(0x02726C74,dest,src);call<void>(0x027222E4,p,to);call<void>(0x02721C44,cpu->r[3],cpu->r[4],0u);call<void>(0x02721D18,p);
}
VERIFY(0x027227A8,saveMgrCopySlot);
u32 saveMgrOpenForWrite(u32 p){
 WWHD_FUNC(0x02722894,u32,p);if((s32)U(p,0x24)==-3)return 0;Local<SaveNameStorage> name;name->name=0x10140898u;name->owner=0x101407B0u;u32 record=call<u32>(0x02741B38,U(0x101F8B08u),name.get());u32 file=call<u32>(0x0274483C,record);if(!file){store<u32>(p+0x24,-3);u32 audio=U(0x1018F2BCu);if(audio)call<void>(0x0203300C,audio,0x17A714u,1u);}return file;
}
VERIFY(0x02722894,saveMgrOpenForWrite);

u32 saveMgrEventBit(u32 p,u32 bit){
 WWHD_FUNC(0x02722BD8,u32,p,bit);if(bit>=0x86)return 0;u32 index=bit>>3;if(index>=0x86)return 1;
 u32 reg=call<u32>(0x025B8BB0,U(0x101F84DCu)+0x644,load<u16>(0x101408A4u+index*2));return(reg>>(bit&7))&1;
}
VERIFY(0x02722BD8,saveMgrEventBit);
u32 saveMgrEventCount(u32 p){
 WWHD_FUNC(0x02722C6C,u32,p);u32 n=0;for(u32 i=0;i<0x86;i++)if(call<u32>(0x02722BD8,p,i))n++;return n;
}
VERIFY(0x02722C6C,saveMgrEventCount);
void saveMgrDescribeSlot(u32 p,u32 description){
 WWHD_FUNC(0x02722CE4,void,p,description);u32 state=U(0x101F84DCu),slot=load<u8>(state+0x12B0),card=state+0x12C0;
 u32 status=call<u32>(0x027200D8,card,slot);call<void>(0x02726D94,description,load<u8>(status)!=2);
 call<void>(0x02726E24,description,call<u32>(0x02722980,p,0u));call<void>(0x02726E34,description,call<u32>(0x02722980,p,1u));
 u32 upgraded=call<u32>(0x025B5D40,U(0x101F84DCu)+0x71,1u,1u)!=0;call<void>(0x02726E44,description,upgraded);
 u32 menu=U(0x101F8344u);if(menu){u32 screen=U(menu,0x218);if(screen)call<void>(0x02726E54,description,call<u32>(0x0268A4E8,screen));}
 u32 player=call<u32>(0x027200A0,card,slot);call<void>(0x02726E64,description,load<u8>(player));
 player=call<u32>(0x027200A0,card,slot);call<void>(0x02726E74,description,load<u8>(player+1));
 player=call<u32>(0x027200A0,card,slot);call<void>(0x02726E84,description,load<u8>(player+2));
 player=call<u32>(0x027200A0,card,slot);call<void>(0x02726E94,description,load<u8>(player+3)!=1);
 player=call<u32>(0x027200A0,card,slot);u32 a=call<u32>(0x0271FC30,player);call<void>(0x02726EA4,description,a^1);
 player=call<u32>(0x027200A0,card,slot);a=call<u32>(0x0271FC78,player);call<void>(0x02726EB4,description,a);
 a=call<u32>(0x02722C6C,p);call<void>(0x02726F04,description,a);
}
VERIFY(0x02722CE4,saveMgrDescribeSlot);

u32 saveMgrProgress(u32 p,u32 alternate){
 WWHD_FUNC(0x02722980,u32,p,alternate);u32 state=U(0x101F84DCu);u32 mode=load<u8>(state+0x1C0);
 if(alternate){if(!mode)return 0;if(mode>1)return 15;}else if(mode)return 15;
 if(call<u32>(0x025B8B94,state+0x644,0x3F10u))return 14;
 if(call<u32>(0x025B7E00,U(0x101F84DCu)+0xD4)==8)return 13;
 if(call<u32>(0x025B8B94,U(0x101F84DCu)+0x644,0x2C01u))return 13;
 if(call<u32>(0x025B8B94,U(0x101F84DCu)+0x644,0x4004u)&&call<u32>(0x025B8B94,U(0x101F84DCu)+0x644,0x3A02u))return 12;
 if(call<u32>(0x025B8B94,U(0x101F84DCu)+0x644,0x4004u))return 11;
 if(call<u32>(0x025B8B94,U(0x101F84DCu)+0x644,0x3A02u))return 10;
 if(call<u32>(0x025B8B94,U(0x101F84DCu)+0x644,0x2D10u))return 9;
 if(call<u32>(0x025B7D90,U(0x101F84DCu)+0xD4,0u))return 8;
 if(call<u32>(0x02520C0C,0x31u))return 7;
 if(call<u32>(0x025B7D90,U(0x101F84DCu)+0xD4,2u))return 6;
 if(call<u32>(0x025B7D90,U(0x101F84DCu)+0xD4,1u))return 5;
 if(call<u32>(0x025B5D40,U(0x101F84DCu)+0x71,1u,0u))return 4;
 if(call<u32>(0x025B8B94,U(0x101F84DCu)+0x644,0x2E01u))return 3;
 if(call<u32>(0x025B7A2C,U(0x101F84DCu)+0xD4,1u,0u))return 2;
 if(call<u32>(0x025B7A2C,U(0x101F84DCu)+0xD4,0u,0u))return 1;return 0;
}
VERIFY(0x02722980,saveMgrProgress);
void saveMgrAdjustDescription(u32 p,u32 dest,u32 old,u32 fresh){
 WWHD_FUNC(0x027222F4,void,p,dest,old,fresh);u32 a,b,c;
a=call<u32>(0x02726D2C,dest);b=call<u32>(0x02726D2C,old);c=call<u32>(0x02726D2C,fresh);call<void>(0x02726D34,dest,a-b+c);
a=call<u32>(0x02726D4C,dest);b=call<u32>(0x02726D4C,old);c=call<u32>(0x02726D4C,fresh);call<void>(0x02726D54,dest,a-b+c);
a=call<u32>(0x02726D6C,dest);b=call<u32>(0x02726D6C,old);c=call<u32>(0x02726D6C,fresh);call<void>(0x02726D74,dest,a-b+c);
a=call<u32>(0x02726D9C,dest);b=call<u32>(0x02726D9C,old);c=call<u32>(0x02726D9C,fresh);call<void>(0x02726DA4,dest,a-b+c);
a=call<u32>(0x02726DBC,dest);b=call<u32>(0x02726DBC,old);c=call<u32>(0x02726DBC,fresh);call<void>(0x02726DC4,dest,a-b+c);
a=call<u32>(0x02726DDC,dest);b=call<u32>(0x02726DDC,old);c=call<u32>(0x02726DDC,fresh);call<void>(0x02726DE4,dest,a-b+c);
a=call<u32>(0x02726EBC,dest);b=call<u32>(0x02726EBC,old);c=call<u32>(0x02726EBC,fresh);call<void>(0x02726EC4,dest,a-b+c);
a=call<u32>(0x02726EDC,dest);b=call<u32>(0x02726EDC,old);c=call<u32>(0x02726EDC,fresh);call<void>(0x02726EE4,dest,a-b+c);
}
VERIFY(0x027222F4,saveMgrAdjustDescription);

void saveMgrAdvanceSlot(u32 p,u32 slot){
 WWHD_FUNC(0x0272249C,void,p,slot);u32 state=U(0x101F84DCu),mode=(load<u8>(state+0x1C0)+1)&0xFF,previous=load<u8>(state+0x12B0);
 call<void>(0x025B9A90,state+0x20);call<void>(0x027222E4,p,slot);call<void>(0x0272005C,U(0x101F84DCu)+0x12C0);
 if(previous!=slot){u32 from=call<u32>(0x02720154,U(0x101F84DCu)+0x12C0,previous);u32 to=call<u32>(0x02720154,U(0x101F84DCu)+0x12C0,slot);call<void>(0x027267FC,to,from);
 u32 dest=call<u32>(0x027201DC,U(0x101F84DCu)+0x12C0);u32 old=call<u32>(0x027201A4,U(0x101F84DCu)+0x12C0,previous);u32 fresh=call<u32>(0x027201A4,U(0x101F84DCu)+0x12C0,slot);call<void>(0x027222F4,p,dest,old,fresh);}
 u32 stage=call<u32>(0x02720144,U(0x101F84DCu)+0x12C0);call<void>(0x02725FD0,stage);store<u8>(U(0x101F84DCu)+0x1C0,mode);call<void>(0x02721C44,p,slot,0u);call<void>(0x02721D18,p);
}
VERIFY(0x0272249C,saveMgrAdvanceSlot);
u32 saveMgrReadStageMap(u32 p,u32 file,u32 count,u32 slot,u32 room){
 WWHD_FUNC(0x02723224,u32,p,file,count,slot,room);if(!call<u32>(0x027414CC,file,count,U(p,0x68),0x50000u)){store<u32>(p+0x20,-3);return 0;}
 u32 stage=call<u32>(0x02720120,U(0x101F84DCu)+0x12C0,slot&0xFF);u32 dest=call<u32>(0x02726164,stage,room);call<void>(0xC0009988,dest,U(p,0x68),0x50000u,0u);return 1;
}
VERIFY(0x02723224,saveMgrReadStageMap);
u32 saveMgrWriteDescription(u32 p,u32 file,u32 count){
 WWHD_FUNC(0x02723F9C,u32,p,file,count);u32 state=U(0x101F84DCu),buffer=U(p,0x68);u32 description=call<u32>(0x027201A4,state+0x12C0,0u);call<void>(0xC0009988,buffer,description,0x174u,0u);
 if(!call<u32>(0x02741558,file,count,U(p,0x68),0x174u)){store<u32>(p+0x24,-2);return 0;}store<u32>(p+0x24,0);return 1;
}
VERIFY(0x02723F9C,saveMgrWriteDescription);

struct SaveFileStorage {u8 raw[0x3C];};
static void saveInitFile(u32 file){call<void>(0x02752A84,file);store<u32>(file+0x10,0);store<u32>(file+0x14,0);store<u32>(file+0x38,0);store<u32>(file+12,0x10140830u);}
static void saveDestroyFile(u32 file){u32 heap=U(file,0x14);store<u32>(file+12,0x10140830u);if(heap)call<void>(0x02740BE0,heap,file);call<void>(0x02752BEC,file,0u);}
void saveMgrReadExtra(u32 p){
 WWHD_FUNC(0x0272369C,void,p);Local<SaveFileStorage> file;saveInitFile(file.a);Local<SaveNameStorage> name;name->name=0x1014090Cu;name->owner=0x101407B0u;
 call<void>(0x02741CFC,U(0x101F8B08u),file.get(),name.get(),0u,0u);Local<be<u32>> count;*count.get()=0;
 bool valid=false;if(U(file.a,0x10)&&call<u32>(0x027414CC,file.get(),count.get(),U(p,0x68),0x6A4u)&&((u32)*count.get()==0x6A4)){
 call<void>(0xC0009988,U(0x101F84DCu)+0xF026C0,U(p,0x68),0x6A4u,0u);valid=true;}
 if(!valid)call<void>(0x02722640,p);if(U(file.a,0x10))call<void>(0x02741430,file.get());saveDestroyFile(file.a);
}
VERIFY(0x0272369C,saveMgrReadExtra);
void saveMgrReadDescription(u32 p){
 WWHD_FUNC(0x027237D4,void,p);Local<SaveFileStorage> file;saveInitFile(file.a);Local<SaveNameStorage> name;name->name=0x1014092Cu;name->owner=0x101407B0u;
 call<void>(0x02741CFC,U(0x101F8B08u),file.get(),name.get(),0u,0u);Local<be<u32>> count;*count.get()=0;
 bool valid=false;if(U(file.a,0x10)&&call<u32>(0x027414CC,file.get(),count.get(),U(p,0x68),0x174u)&&((u32)*count.get()==0x174)){
 u32 dest=call<u32>(0x027201A4,U(0x101F84DCu)+0x12C0,0u);call<void>(0xC0009988,dest,U(p,0x68),0x174u,0u);call<void>(0x027201DC,U(0x101F84DCu)+0x12C0);valid=true;}
 if(!valid)call<void>(0x02722654,p);if(U(file.a,0x10))call<void>(0x02741430,file.get());saveDestroyFile(file.a);
}
VERIFY(0x027237D4,saveMgrReadDescription);
u32 saveMgrReadStageHeaders(u32 p,u32 file,u32 count,u32 slot){
 WWHD_FUNC(0x027232D8,u32,p,file,count,slot);
 for(u32 i=0;i<12;i++){
 if(!call<u32>(0x027414CC,file,count,U(p,0x68),16u)){store<u32>(p+0x20,-3);return 0;}
 u32 stage=call<u32>(0x02720120,U(0x101F84DCu)+0x12C0,slot&0xFF);u32 part=call<u32>(0x0272609C,stage,i);call<void>(0xC0009988,part,U(p,0x68),16u,0u);
 stage=call<u32>(0x02720120,U(0x101F84DCu)+0x12C0,slot&0xFF);part=call<u32>(0x0272609C,stage,i);u32 expected=call<u32>(0x0272581C,part);
 stage=call<u32>(0x02720120,U(0x101F84DCu)+0x12C0,slot&0xFF);part=call<u32>(0x0272609C,stage,i);u32 actual=call<u32>(0x02725B44,part);if(expected!=actual){store<u32>(p+0x20,-3);return 0;}
 }
 if(!call<u32>(0x027414CC,file,count,U(p,0x68),12u)){store<u32>(p+0x20,-3);return 0;}
 u32 stage=call<u32>(0x02720120,U(0x101F84DCu)+0x12C0,slot&0xFF);call<void>(0xC0009988,stage+0x3C0300,U(p,0x68),12u,0u);
 if(!call<u32>(0x027414CC,file,count,U(p,0x68),1u)){store<u32>(p+0x20,-3);return 0;}
 Local<be<u8>> flag;*flag.get()=0;call<void>(0xC0009988,flag.get(),U(p,0x68),1u,0u);stage=call<u32>(0x02720120,U(0x101F84DCu)+0x12C0,slot&0xFF);call<void>(0x027262B0,stage,(u32)*flag.get());return 1;
}
VERIFY(0x027232D8,saveMgrReadStageHeaders);
u32 saveMgrWriteStageMap(u32 p,u32 file,u32 count,u32 slot,u32 room){
 WWHD_FUNC(0x02723B54,u32,p,file,count,slot,room);u32 card=U(0x101F84DCu)+0x12C0;
 u32 stage=call<u32>(0x02720120,card,slot&0xFF);u32 part=call<u32>(0x0272609C,stage,room);call<void>(0x027258C4,part);
 stage=call<u32>(0x02720120,card,slot&0xFF);call<void>(0x02726028,stage,room);
 u32 buffer=U(p,0x68);stage=call<u32>(0x02720120,card,slot&0xFF);u32 src=call<u32>(0x02726164,stage,room);call<void>(0xC0009988,buffer,src,0x50000u,0u);
 if(!call<u32>(0x02741558,file,count,U(p,0x68),0x50000u)){store<u32>(p+0x24,-2);return 0;}store<u32>(p+0x24,0);return 1;
}
VERIFY(0x02723B54,saveMgrWriteStageMap);
u32 saveMgrWriteStageHeaders(u32 p,u32 file,u32 count,u32 slot){
 WWHD_FUNC(0x02723C38,u32,p,file,count,slot);
 for(u32 i=0;i<12;i++){u32 state=U(0x101F84DCu),buffer=U(p,0x68);u32 stage=call<u32>(0x02720120,state+0x12C0,slot&0xFF);u32 part=call<u32>(0x0272609C,stage,i);call<void>(0xC0009988,buffer,part,16u,0u);if(!call<u32>(0x02741558,file,count,U(p,0x68),16u))return 0;}
 u32 state=U(0x101F84DCu),buffer=U(p,0x68),stage=call<u32>(0x02720120,state+0x12C0,slot&0xFF);call<void>(0xC0009988,buffer,stage+0x3C0300,12u,0u);
 if(!call<u32>(0x02741558,file,count,U(p,0x68),12u)){store<u32>(p+0x24,-2);return 0;}
 stage=call<u32>(0x02720120,U(0x101F84DCu)+0x12C0,slot&0xFF);store<u8>(U(p,0x68),load<u8>(stage+0x3C030C));if(!call<u32>(0x02741558,file,count,U(p,0x68),1u)){store<u32>(p+0x24,-2);return 0;}store<u32>(p+0x24,0);return 1;
}
VERIFY(0x02723C38,saveMgrWriteStageHeaders);

void saveMgrWorkerLoad(u32 p){
 WWHD_FUNC(0x0272391C,void,p);Local<SaveFileStorage> file;saveInitFile(file.a);Local<SaveNameStorage> name;name->name=0x1014098Cu;name->owner=0x101407B0u;
 call<void>(0x02741CFC,U(0x101F8B08u),file.get(),name.get(),0u,0u);Local<be<u32>> count;*count.get()=0;store<u32>(p+0x20,0xFF);
 bool failed=false;u32 status=0xFF;
 for(u32 i=0;i<8;i++){
  u32 descriptor=0x1014094Cu+i*8; s32 index=load<s16>(descriptor+2),adjust=load<s16>(descriptor);u32 self=p+(u32)adjust,target,r9=cpu->r[9],r10=descriptor;
  if(index<0)target=U(descriptor,4);else{r9=(u32)(s32)load<s16>(descriptor+6);u32 table=U(self+r9);r10=(u32)index*8;target=U(table+r10+4);}
  u32 result=call_ptr<u32>(target,self,file.get(),count.get(),cpu->r[6],cpu->r[7],(u32)adjust,r9,r10,cpu->f[1].ps0,cpu->f[2].ps0,cpu->f[3].ps0,cpu->f[4].ps0,cpu->f[5].ps0,cpu->f[6].ps0,cpu->f[7].ps0,cpu->f[8].ps0);
  if(!result){failed=true;status=U(p,0x20);break;}status=U(p,0x20);if(status!=0xFF)break;
 }
 if(!failed&&status==0xFF){store<u32>(p+0x20,0);status=0;}else if(status!=(u32)-2){store<u32>(p+0x20,-3);status=-3;}
 if(status==(u32)-2)call<void>(0x027226B4,p);if(U(file.a,0x10))call<void>(0x02741430,file.get());
 if(!U(p,0x20)){for(u32 i=0;i<3;i++)call<void>(0x02723460,p,i);if(!U(p,0x20)){call<void>(0x0272369C,p);if(!U(p,0x20))call<void>(0x027237D4,p);}}
 call<void>(0x02721908,p,load<u8>(U(0x101F84DCu)+0x12B0),1u);store<u32>(p+0x1C,0);saveDestroyFile(file.a);
}
VERIFY(0x0272391C,saveMgrWorkerLoad);
void saveMgrWorkerExtra(u32 p){
 WWHD_FUNC(0x027243E0,void,p);Local<SaveFileStorage> file;saveInitFile(file.a);Local<SaveNameStorage> name;name->name=0x10140A6Cu;name->owner=0x101407B0u;
 call<void>(0x02741CFC,U(0x101F8B08u),file.get(),name.get(),1u,0u);Local<be<u32>> count;*count.get()=0;
 if(U(file.a,0x10)){store<u32>(p+0x24,0xFF);call<void>(0xC0009988,U(p,0x68),U(0x101F84DCu)+0xF026C0,0x6A4u,0u);
 if(!call<u32>(0x02741558,file.get(),count.get(),U(p,0x68),0x6A4u))store<u32>(p+0x24,-2);else if(U(p,0x24)==0xFF)store<u32>(p+0x24,(u32)*count.get()==0x6A4?0:-2);
 }else store<u32>(p+0x24,-1);
 if(U(file.a,0x10))call<void>(0x02741430,file.get());
 if(!U(p,0x24)){Local<SaveNameStorage> cleanup;cleanup->name=0x10140A8Cu;cleanup->owner=0x101407B0u;u32 record=call<u32>(0x02741B38,U(0x101F8B08u),cleanup.get());if(!call<u32>(0x02744894,record))store<u32>(p+0x24,-2);}
 store<u32>(p+0x1C,0);saveDestroyFile(file.a);
}
VERIFY(0x027243E0,saveMgrWorkerExtra);

struct SaveFormatStorage {u8 raw[76];};
static void saveInitFormat(u32 format){store<u32>(format,format+12);store<u32>(format+4,0x10140818u);store<u32>(format+8,64);store<u8>(format+12,0);store<u8>(format+75,0);}
void saveMgrReadStageFiles(u32 p,u32 slot){
 WWHD_FUNC(0x02723460,void,p,slot);Local<SaveFileStorage> file;Local<SaveFormatStorage> name;Local<be<u32>> count;*count.get()=0;u32 result=0;
 for(u32 room=0;room<12;room++){
 saveInitFile(file.a);saveInitFormat(name.a);call<void>(0x02759C28,name.get(),0x101408C8u,slot,room);
 call<void>(0x02741CFC,U(0x101F8B08u),file.get(),name.get(),0u,0u);
 if(U(file.a,0x10)){result=call<u32>(0x02723224,p,file.get(),count.get(),slot,room);if(!result){call<void>(0x02722610,p,slot&0xFF);saveDestroyFile(file.a);return;}call<void>(0x02741430,file.get());}
 else call<void>(0x02722610,p,slot&0xFF);
 saveDestroyFile(file.a);
 }
 if(result){saveInitFile(file.a);saveInitFormat(name.a);call<void>(0x02759C28,name.get(),0x101408E8u,slot);call<void>(0x02741CFC,U(0x101F8B08u),file.get(),name.get(),0u,0u);
 if(U(file.a,0x10)){call<void>(0x027232D8,p,file.get(),count.get(),slot);call<void>(0x02741430,file.get());}saveDestroyFile(file.a);}
}
VERIFY(0x02723460,saveMgrReadStageFiles);
void saveMgrWriteStageFiles(u32 p,u32 slot){
 WWHD_FUNC(0x02723D98,void,p,slot);Local<be<u32>> count;*count.get()=0;u32 stage=call<u32>(0x02720120,U(0x101F84DCu)+0x12C0,slot&0xFF),result=0;
 Local<SaveFileStorage> file;Local<SaveFormatStorage> name;
 for(u32 room=0;room<12;room++){
 if(!call<u32>(0x02725FAC,stage,room))continue;
 saveInitFile(file.a);saveInitFormat(name.a);call<void>(0x02759C28,name.get(),0x101409A4u,slot,room);call<void>(0x02741CFC,U(0x101F8B08u),file.get(),name.get(),1u,0u);
 if(U(file.a,0x10)){result=call<u32>(0x02723B54,p,file.get(),count.get(),slot,room);call<void>(0x02741430,file.get());}saveDestroyFile(file.a);
 }
 if(result){saveInitFile(file.a);saveInitFormat(name.a);call<void>(0x02759C28,name.get(),0x101409C4u,slot);call<void>(0x02741CFC,U(0x101F8B08u),file.get(),name.get(),1u,0u);
 if(U(file.a,0x10)){call<void>(0x02723C38,p,file.get(),count.get(),slot);call<void>(0x02741430,file.get());}saveDestroyFile(file.a);}
}
VERIFY(0x02723D98,saveMgrWriteStageFiles);
void saveMgrWriteDescriptionFile(u32 p){
 WWHD_FUNC(0x02724064,void,p);Local<be<u32>> count;*count.get()=0;Local<SaveFileStorage> file;saveInitFile(file.a);Local<SaveFormatStorage> name;saveInitFormat(name.a);
 call<void>(0x02759C28,name.get(),0x101409E8u);call<void>(0x02741CFC,U(0x101F8B08u),file.get(),name.get(),1u,0u);
 if(U(file.a,0x10)){u32 state=U(0x101F84DCu),slot=load<u8>(state+0x12B0);u32 description=call<u32>(0x027201A4,state+0x12C0,slot);call<void>(0x02722CE4,p,description);call<void>(0x02723F9C,p,file.get(),count.get());call<void>(0x02741430,file.get());}saveDestroyFile(file.a);
}
VERIFY(0x02724064,saveMgrWriteDescriptionFile);
void saveMgrMergeDescriptions(u32 p,u32 dest){
 WWHD_FUNC(0x02722E88,void,p,dest);u32 card=U(0x101F84DCu)+0x12C0,part,value,current;call<void>(0x02726B30,dest);for(u32 i=0;i<3;i++){
part=call<u32>(0x027201C0,card,i);value=call<u32>(0x02726D2C,part);call<void>(0x02726D3C,dest,value);
part=call<u32>(0x027201C0,card,i);value=call<u32>(0x02726D4C,part);call<void>(0x02726D5C,dest,value);
part=call<u32>(0x027201C0,card,i);value=call<u32>(0x02726D6C,part);call<void>(0x02726D7C,dest,value);
part=call<u32>(0x027201C0,card,i);value=call<u32>(0x02726D9C,part);call<void>(0x02726DAC,dest,value);
part=call<u32>(0x027201C0,card,i);value=call<u32>(0x02726DBC,part);call<void>(0x02726DCC,dest,value);
part=call<u32>(0x027201C0,card,i);value=call<u32>(0x02726DDC,part);call<void>(0x02726DEC,dest,value);
part=call<u32>(0x027201C0,card,i);value=call<u32>(0x02726EBC,part);call<void>(0x02726ECC,dest,value);
part=call<u32>(0x027201C0,card,i);value=call<u32>(0x02726EDC,part);call<void>(0x02726EEC,dest,value);
}
for(u32 i=0;i<3;i++){
current=call<u32>(0x02726DFC,dest);part=call<u32>(0x027201C0,card,i);value=call<u32>(0x02726DFC,part);if(current<value){part=call<u32>(0x027201C0,card,i);value=call<u32>(0x02726DFC,part);call<void>(0x02726E04,dest,value);}
current=call<u32>(0x02726E1C,dest);part=call<u32>(0x027201C0,card,i);value=call<u32>(0x02726E1C,part);if(current<value){part=call<u32>(0x027201C0,card,i);value=call<u32>(0x02726E1C,part);call<void>(0x02726E24,dest,value);}
current=call<u32>(0x02726E2C,dest);part=call<u32>(0x027201C0,card,i);value=call<u32>(0x02726E2C,part);if(current<value){part=call<u32>(0x027201C0,card,i);value=call<u32>(0x02726E2C,part);call<void>(0x02726E34,dest,value);}
current=call<u32>(0x02726E3C,dest);part=call<u32>(0x027201C0,card,i);value=call<u32>(0x02726E3C,part);if(current<value){part=call<u32>(0x027201C0,card,i);value=call<u32>(0x02726E3C,part);call<void>(0x02726E44,dest,value);}
current=call<u32>(0x02726E4C,dest);part=call<u32>(0x027201C0,card,i);value=call<u32>(0x02726E4C,part);if(current<value){part=call<u32>(0x027201C0,card,i);value=call<u32>(0x02726E4C,part);call<void>(0x02726E54,dest,value);}
current=call<u32>(0x02726EFC,dest);part=call<u32>(0x027201C0,card,i);value=call<u32>(0x02726EFC,part);if(current<value){part=call<u32>(0x027201C0,card,i);value=call<u32>(0x02726EFC,part);call<void>(0x02726F04,dest,value);}
}
part=call<u32>(0x027200B8,card,0u);u32 slot=call<u32>(0x0271FCA4,part);
part=call<u32>(0x027201C0,card,slot&0xFF);value=call<u32>(0x02726D8C,part);call<void>(0x02726D94,dest,value);
part=call<u32>(0x027201C0,card,slot&0xFF);value=call<u32>(0x02726E5C,part);call<void>(0x02726E64,dest,value);
part=call<u32>(0x027201C0,card,slot&0xFF);value=call<u32>(0x02726E6C,part);call<void>(0x02726E74,dest,value);
part=call<u32>(0x027201C0,card,slot&0xFF);value=call<u32>(0x02726E7C,part);call<void>(0x02726E84,dest,value);
part=call<u32>(0x027201C0,card,slot&0xFF);value=call<u32>(0x02726E8C,part);call<void>(0x02726E94,dest,value);
part=call<u32>(0x027201C0,card,slot&0xFF);value=call<u32>(0x02726E9C,part);call<void>(0x02726EA4,dest,value);
part=call<u32>(0x027201C0,card,slot&0xFF);value=call<u32>(0x02726EAC,part);call<void>(0x02726EB4,dest,value);
}
VERIFY(0x02722E88,saveMgrMergeDescriptions);

void saveMgrWorkerWrite(u32 p,u32 allSlots){
 WWHD_FUNC(0x02724180,void,p,allSlots);Local<SaveFileStorage> file;saveInitFile(file.a);Local<SaveNameStorage> name;name->name=0x10140A48u;name->owner=0x101407B0u;
 call<void>(0x02741CFC,U(0x101F8B08u),file.get(),name.get(),1u,0u);Local<be<u32>> count;*count.get()=0;store<u32>(p+0x24,0xFF);
 bool failed=false;u32 status=0xFF;
 for(u32 i=0;i<8;i++){
  u32 descriptor=0x10140A08u+i*8;s32 index=load<s16>(descriptor+2),adjust=load<s16>(descriptor);u32 self=p+(u32)adjust,target,r7=(u32)adjust,r8=cpu->r[8],r10=cpu->r[10];
  if(index<0)target=U(descriptor,4);else{r8=(u32)(s32)load<s16>(descriptor+6);r10=U(self+r8);r7=r10+(u32)index*8;target=U(r7,4);}
  u32 result=call_ptr<u32>(target,self,file.get(),count.get(),(u32)index,r7,r8,cpu->r[9],r10,cpu->f[1].ps0,cpu->f[2].ps0,cpu->f[3].ps0,cpu->f[4].ps0,cpu->f[5].ps0,cpu->f[6].ps0,cpu->f[7].ps0,cpu->f[8].ps0);
  if(!result){failed=true;status=U(p,0x24);break;}status=U(p,0x24);if(status!=0xFF)break;
 }
 if(!failed&&status==0xFF){store<u32>(p+0x24,0);status=0;}else if(status!=(u32)-1){store<u32>(p+0x24,-2);status=-2;}
 if(U(file.a,0x10))call<void>(0x02741430,file.get());
 if(!U(p,0x24)){
  if(allSlots){for(u32 i=0;i<3;i++)call<void>(0x02723D98,p,i);}else call<void>(0x02723D98,p,load<u8>(U(0x101F84DCu)+0x12B0));
  if(!U(p,0x24)){call<void>(0x02724064,p);if(!U(p,0x24)){Local<SaveNameStorage> cleanup;cleanup->name=0x10140A60u;cleanup->owner=0x101407B0u;u32 record=call<u32>(0x02741B38,U(0x101F8B08u),cleanup.get());if(!call<u32>(0x02744894,record))store<u32>(p+0x24,-2);}}
 }
 store<u32>(p+0x1C,0);saveDestroyFile(file.a);
}
VERIFY(0x02724180,saveMgrWorkerWrite);

struct SaveComparisonStorage {SaveNameStorage names[4],games[8];};
static void saveNameCheckedCall(u32 object){
 // SafeString base vtable101407B0+14 is empty027253B4; it reads no argument registers.
 call_ptr<void>(U(U(object,4),0x14),object);
}
static bool saveNamesEqual(u32 left,u32 right){
 saveNameCheckedCall(left);saveNameCheckedCall(left);u32 saved=U(left);saveNameCheckedCall(right);
 if(saved==U(right))return true;u32 a=U(left),b=U(right);
 for(u32 i=0;i<0x40001;i++){u8 x=load<u8>(a+i),y=load<u8>(b+i);if(x!=y)return false;if(!x)return true;}return false;
}
void saveMgrRecollectionSave(u32 p){
 WWHD_FUNC(0x02721D48,void,p);Local<SaveComparisonStorage> comparisons;u32 base=comparisons.a;u32 old[3]={0xFF,0xFF,0xFF};bool first=false;
 const u32 strings[4]={0x10140878u,0x10140880u,0x10140888u,0x10140890u};
 for(u32 i=0;i<4;i++){
  u32 left=base+i*8,right=base+32+i*8;store<u32>(left,strings[i]);store<u32>(left+4,0x101407B0u);
  u32 game=call<u32>(0x025200D4);store<u32>(right,game+0x5134);store<u32>(right+4,0x101407B0u);
  if(saveNamesEqual(left,right)){first=true;break;}
 }
 if(first){u32 state=U(0x101F84DCu);old[0]=load<u8>(state+0x29);old[1]=load<u8>(state+0x2A);old[2]=load<u8>(state+0x2B);call<void>(0x02522C60);}
 call<void>(0x02721C44,p,load<u8>(U(0x101F84DCu)+0x12B0),1u);
 bool second=false;
 for(u32 i=0;i<4;i++){
  u32 left=base+(3-i)*8,right=base+64+i*8;store<u32>(left,strings[i]);store<u32>(left+4,0x101407B0u);
  u32 game=call<u32>(0x025200D4);store<u32>(right,game+0x5134);store<u32>(right+4,0x101407B0u);
  if(saveNamesEqual(left,right)){second=true;break;}
 }
 if(second){call<void>(0x025224A4);store<u8>(U(0x101F84DCu)+0x29,old[0]);store<u8>(U(0x101F84DCu)+0x2A,old[1]);store<u8>(U(0x101F84DCu)+0x2B,old[2]);}
 call<void>(0x02721D18,p);
}
VERIFY(0x02721D48,saveMgrRecollectionSave);
