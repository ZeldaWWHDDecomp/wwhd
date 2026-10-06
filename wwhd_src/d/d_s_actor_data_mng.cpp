#include "wwhd.h"
#include "gabi.h"
void CharTbl_SetUpIndex(u32 self){
 WWHD_FUNC(0x025AB7C4,void,self);
 const u32 keys[]={0x100525C0,0x10052568,0x10052570,0x10052578,0x10052580,0x10052588,0x10052590,0x10052598,0x100525A0,0x100525A8,0x100525B0,0x10052604,0x10052610,0x1005261C,0x10052628,0x10052634,0x10052640,0x100525B8,0x100525C4,0x100525CC,0x100525D4,0x100525DC,0x100525E4,0x100525EC,0x100525F4,0x100525FC};
 for(u32 i=0;i<26;++i){u32 index=gabi::call<u32>(0x0200E814,self,gabi::at<void>(keys[i]),0);gabi::store<u32>(self+0x28+4*i,index);}
}
VERIFY(0x025AB7C4,CharTbl_SetUpIndex);
u32 CharTbl_ctor(u32 self){WWHD_FUNC(0x025ABA58,u32,self);if(!self)self=gabi::call<u32>(0x0273AD10,0x90);if(self)gabi::call<void>(0x0200E9BC,self);return self;}
VERIFY(0x025ABA58,CharTbl_ctor);
void CharTbl_dtor(u32 self,u32 flags){WWHD_FUNC(0x025ABAA0,void,self,flags);if(self){gabi::call<void>(0x0200EA14,self,0);if(flags&1)gabi::call<void>(0x0273AF40,self);}}
VERIFY(0x025ABAA0,CharTbl_dtor);
void CharTbl_SetData(u32 self,u32 base,u32 n1,u32 p1,u32 n2,u32 p2,u32 count,u32 data){
 WWHD_FUNC(0x025ABAF4,void,self,base,n1,p1,n2,p2,count,data);
 for(u32 i=0;i<n1;++i){u32 a=p1+i*4;gabi::store<u32>(a,gabi::load<u32>(a)+base);}
 for(u32 i=0;i<n2;++i){u32 a=p2+i*4;gabi::store<u32>(a,gabi::load<u32>(a)+base);}
 if(count!=n1*n2)gabi::call<void>(0x0273AA24,gabi::at<void>(0x1005266C),0x3A,gabi::at<void>(0x1005264C));
 gabi::call<void>(0x0200EA28,self,n1,p1,n2,p2,data);CharTbl_SetUpIndex(self);
}
VERIFY(0x025ABAF4,CharTbl_SetData);
s32 CharTbl_GetNameIndex2(u32 self,u32 name,u32 match){
 WWHD_FUNC(0x025ABBBC,s32,self,name,match);
 u32 start=0;
 for(;;){s32 found=gabi::call<s32>(0x0200E814,gabi::at<void>(self+0xC),name,start);if(found==-1)return found;
  u32 field=gabi::load<u32>(self+0x28);if(gabi::call<u32>(0x0200EA74,self,field,u32(found))==match)return found;start=u32(found)+1;
 }
}
VERIFY(0x025ABBBC,CharTbl_GetNameIndex2);
u32 ADM_ctor(u32 self){WWHD_FUNC(0x025ABC48,u32,self);if(!self)self=gabi::call<u32>(0x0273AD10,0x9C);if(self){gabi::store<u32>(self+0x98,0x100526C8);CharTbl_ctor(self+8);gabi::store<u32>(self+4,0);gabi::store<u32>(self,0);}return self;}
VERIFY(0x025ABC48,ADM_ctor);
void ADM_dtor(u32 self,u32 flags){WWHD_FUNC(0x025ABCA8,void,self,flags);if(self){CharTbl_dtor(self+8,2);if(flags&1)gabi::call<void>(0x0273AF40,self);}}
VERIFY(0x025ABCA8,ADM_dtor);
s32 ADM_FindTag(u32 self,u32 tag,u32 outCount,u32 outData){
 WWHD_FUNC(0x025ABCFC,s32,self,tag,outCount,outData);
 s32 count=gabi::load<s32>(self);u32 entry=gabi::load<u32>(self+4);
 for(s32 i=0;i<count;++i,entry+=12){if(gabi::load<u32>(entry)==tag){u32 n=gabi::load<u32>(entry+4);gabi::store<u32>(outCount,n);u32 p=gabi::load<u32>(entry+8);gabi::store<u32>(outData,p);return 1;}}return 0;
}
VERIFY(0x025ABCFC,ADM_FindTag);
void ADM_SetData(u32 self,u32 data){
 WWHD_FUNC(0x025ABD4C,void,self,data);
 s32 count=gabi::load<s32>(data);gabi::store<s32>(self,count);u32 entry=data+4;gabi::store<u32>(self+4,entry);
 for(s32 i=0;i<count;++i,entry+=12){gabi::store<u32>(entry+8,gabi::load<u32>(entry+8)+data);count=gabi::load<s32>(self);}
 struct Outputs{be<u32> n1,n2,n3,p1,p2,p3;};gabi::Local<Outputs> out;
 if(!ADM_FindTag(self,0x4143464E,gabi::ea(&out->n1),gabi::ea(&out->p1)))return;
 if(!ADM_FindTag(self,0x41434E41,gabi::ea(&out->n2),gabi::ea(&out->p2)))return;
 if(!ADM_FindTag(self,0x41434453,gabi::ea(&out->n3),gabi::ea(&out->p3)))return;
 u32 n1=out->n1,n2=out->n2,n3=out->n3;
 if(n1*n2!=n3){gabi::call<void>(0x0273AA24,gabi::at<void>(0x10052688),0xD7,gabi::at<void>(0x100526A0));n1=out->n1;n2=out->n2;n3=out->n3;}
 u32 p2=out->p2,p1=out->p1,p3=out->p3;CharTbl_SetData(self+8,data,n1,p1,n2,p2,n3,p3);
}
VERIFY(0x025ABD4C,ADM_SetData);
void d_s_actor_data_mng_static_init() {
 WWHD_FUNC(0x025ABE6C,void);
 gabi::store<u32>(0x1047B374,0);gabi::store<u32>(0x1047B36C,0);gabi::store<u32>(0x1047B378,0);gabi::store<u32>(0x1047B370,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101EA5CC));
 f32 lo=gabi::load<f32>(0x100526BC),hi=gabi::load<f32>(0x100526C0);
 gabi::store<f32>(0x1047B360,lo);gabi::store<f32>(0x1047B364,hi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x1047B368));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101EA5D8));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x1047B369));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101EA5E4));
}
VERIFY(0x025ABE6C,d_s_actor_data_mng_static_init);
