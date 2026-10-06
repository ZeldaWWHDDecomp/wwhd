// Qualified HD SaveMgr card range;
#include "d_menu_save_local.h"
void saveMgrInitialCard(u32 p,u32 slot){
 WWHD_FUNC(0x027219AC,void,p,slot);u32 state=U(0x101F84DCu);call<void>(0x025BAC50,state+0x20,U(p,0x64),slot);call<void>(0x0271FF4C,U(0x101F84DCu)+0x12C0,slot);
}
VERIFY(0x027219AC,saveMgrInitialCard);
void saveMgrInitialCardWrapper(u32 p,u32 slot){
 WWHD_FUNC(0x02721A04,void,p,slot);call<void>(0x027219AC,p,slot);
}
VERIFY(0x02721A04,saveMgrInitialCardWrapper);
void saveMgrMode(u32 p,u32 mode){
 WWHD_FUNC(0x02721A08,void,p,mode);store<u8>(U(0x101F84DCu)+0x12B0,mode);
}
VERIFY(0x02721A08,saveMgrMode);
void saveMgrBegin(u32 p,u32 mode){
 WWHD_FUNC(0x02721A18,void,p,mode);call<void>(0x02721A08,p,mode);if(!U(p,0x1C)){u32 thread=U(p,0x18);store<u32>(p+0x1C,1);call_ptr<void>(U(U(thread,12),0x1C),thread,1u,1u);}
}
VERIFY(0x02721A18,saveMgrBegin);
void saveMgrSessionCounter(){
 WWHD_FUNC(0x02721A68,void);u32 state=U(0x101F84DCu);u16 n=load<u16>(state+0x178);if(n<9999)store<u16>(state+0x178,n+1);
}
VERIFY(0x02721A68,saveMgrSessionCounter);
void saveMgrStampTime(){
 WWHD_FUNC(0x02721A8C,void);u32 state=U(0x101F84DCu);call<void>(0xC0009CD8u);u32 high=cpu->r[3],low=cpu->r[4];store<u32>(state+0x3C,low);store<u32>(state+0x38,high);
}
VERIFY(0x02721A8C,saveMgrStampTime);
void saveMgrChecksumSlot(u32 p,u32 data,u32 slot){
 WWHD_FUNC(0x02721AFC,void,p,data,slot);u32 block=data+slot*0xA94u;u32 sum=call<u32>(0x02721AC4,p,block,0xA8Cu);u32 complement=cpu->r[4];store<u32>(block+0xA8C,sum);store<u32>(block+0xA90,complement);
}
VERIFY(0x02721AFC,saveMgrChecksumSlot);
void saveMgrSyncSlot(u32 p,u32 slot){
 WWHD_FUNC(0x02721B34,void,p,slot);u32 state=U(0x101F84DCu)+0x12C0;u32 a=call<u32>(0x027200D0,state);u32 b=call<u32>(0x027200A0,state,slot);call<void>(0x0271FCB4,b,a);
 a=call<u32>(0x027200F4,state);b=call<u32>(0x027200D8,state,slot);call<void>(0x0271FAC0,b,a);
 a=call<u32>(0x02720118,state);b=call<u32>(0x027200FC,state,slot);call<void>(0x0271F914,b,a);
 a=call<u32>(0x0272019C,state);b=call<u32>(0x02720180,state,slot);call<void>(0x027208F4,b,a);
 a=call<u32>(0x02720144,state);b=call<u32>(0x02720120,state,slot);call<void>(0x027262BC,b,a);
 a=call<u32>(0x02720144,state);call<void>(0x02726044,a);
 a=call<u32>(0x027201DC,state);b=call<u32>(0x027201A4,state,slot);call<void>(0x02726BC8,b,a);
}
VERIFY(0x02721B34,saveMgrSyncSlot);
void saveMgrSaveCard(u32 p,u32 slot,u32 update){
 WWHD_FUNC(0x02721C44,void,p,slot,update);if(update){u32 g=call<u32>(0x025200D4)+0x5150;u32 q=call_ptr<u32>(U(U(g),0x15C),g);u32 index=(load<u8>(q+9)>>1)&0x7F;if(index<16)call<void>(0x025B9D24,U(0x101F84DCu)+0x20,index);call<void>(0x02721A68,p);call<void>(0x02721A8C);call<void>(0x025217F8);}
 s32 result=call<s32>(0x025BA9FC,U(0x101F84DCu)+0x20,U(p,0x64),slot);if(result!=-1){for(u32 i=0;i<3;i++)call<void>(0x02721AFC,p,U(p,0x64),i);call<void>(0x02721B34,p,slot);}
}
VERIFY(0x02721C44,saveMgrSaveCard);
void saveMgrEnd(u32 p){
 WWHD_FUNC(0x02721D18,void,p);if(!U(p,0x1C)){u32 thread=U(p,0x18);store<u32>(p+0x1C,2);call_ptr<void>(U(U(thread,12),0x1C),thread,2u,1u);}
}
VERIFY(0x02721D18,saveMgrEnd);
Pair32 saveMgrChecksum(u32 p,u32 data,u32 count){
 WWHD_FUNC(0x02721AC4,Pair32,p,data,count);u32 sum=0,complement=0;
 for(u32 i=0;i<count;i++){u32 b=load<u8>(data+i);sum+=b;complement+=~b;}
 return {sum,complement};
}
VERIFY(0x02721AC4,saveMgrChecksum);
void saveMgrRefreshEquipment(){
 WWHD_FUNC(0x02721730,void);if(call<u32>(0x025B7A2C,U(0x101F84DCu)+0xD4,0u,3u))call<void>(0x02522398,0u,0x3Eu);
 else if(call<u32>(0x025B7A2C,U(0x101F84DCu)+0xD4,0u,2u))call<void>(0x02522398,0u,0x3Au);
 else if(call<u32>(0x025B7A2C,U(0x101F84DCu)+0xD4,0u,1u))call<void>(0x02522398,0u,0x39u);
 else if(call<u32>(0x025B7A2C,U(0x101F84DCu)+0xD4,0u,0u))call<void>(0x02522398,0u,0x38u);
 if(call<u32>(0x025B7A2C,U(0x101F84DCu)+0xD4,1u,1u))call<void>(0x02522398,1u,0x3Cu);
 else if(call<u32>(0x025B7A2C,U(0x101F84DCu)+0xD4,1u,0u))call<void>(0x02522398,1u,0x3Bu);
 if(call<u32>(0x025B7A2C,U(0x101F84DCu)+0xD4,2u,0u))call<void>(0x02522398,2u,0x28u);
}
VERIFY(0x02721730,saveMgrRefreshEquipment);
void saveMgrRefreshGame(u32 p){
 WWHD_FUNC(0x02721880,void,p);call<void>(0x02720E34,p);call<void>(0x02721730,p);
 for(u32 n:{1u,3u,5u,6u,7u})call<void>(0x025B82A0,U(0x101F84DCu)+0xE4,n);
}
VERIFY(0x02721880,saveMgrRefreshGame);
void saveMgrLoadCard(u32 p,u32 slot,u32 resetItems){
 WWHD_FUNC(0x02721908,void,p,slot,resetItems);if(slot>=3)return;call<void>(0x025B9A18,U(0x101F84DCu)+0x20);
 if(resetItems)call<void>(0x025248BC,call<u32>(0x025200D4)+0x12A0);
 store<u8>(U(0x101F84DCu)+0x12B0,slot);call<void>(0x025BA7B0,U(0x101F84DCu)+0x20,U(p,0x64),slot);call<void>(0x02720D34,p,slot);call<void>(0x02721880,p);
}
VERIFY(0x02721908,saveMgrLoadCard);
