#include "gabi.h"
using namespace gabi;
u32 ext_modelCreate(void* data,u32 flags) {
 WWHD_FUNC(0x025E38E0,u32,data,flags);
 if(!ea(data)){call(0x0273AA24,at<void>(0x100589AC),0x25A0,at<void>(0x100589A4));return 0;}
 u32 model=call<u32>(0x0273AD10,0x144);if(!model)return 0;
 model=call<u32>(0x027F3E04,at<void>(model));if(!model)return 0;
 if(call<u32>(0x027F4404,at<void>(model),data,flags,2))return 0;return model;
}
VERIFY(0x025E38E0,ext_modelCreate);
void ext_399C(void* self,u32 flags){
 WWHD_FUNC(0x025E399C,void,self,flags);
 if(!ea(self))return;u32 object=load<u32>(ea(self));store<u32>(ea(self)+4,0x10058DA0);store<u32>(object,0);if(flags&1)call(0x0273AF40,self);
}
VERIFY(0x025E399C,ext_399C);
void ext_39C8(void* self,u32 flags){
 WWHD_FUNC(0x025E39C8,void,self,flags);
 if(!ea(self))return;store<u32>(ea(self)+0x14,0x10058DB0);call(0x025E399C,at<void>(ea(self)+0x10),0);call(0x02752BEC,self,0);if(flags&1)call(0x0273AF40,self);
}
VERIFY(0x025E39C8,ext_39C8);
u32 ext_alignedAlloc(u32 size,u32 align){WWHD_FUNC(0x025E3A34,u32,size,align);return call<u32>(0x0273AE48,size,align);}
VERIFY(0x025E3A34,ext_alignedAlloc);
void ext_decOldCounter(void* self) {
 WWHD_FUNC(0x025E3EC8,void,self);
 u32 o=ea(self);f32 count=load<f32>(o+4);if(!(count>0))return;
 f32 one=load<f32>(0x100586D4),zero=load<f32>(0x100586DC),next=fsubs_ppc(count,one);
 if(!(next>zero)){store<f32>(o+8,zero);next=zero;store<f32>(o+0xC,zero);}
 store<f32>(o+4,next);f32 inv=load<f32>(o+8),old=load<f32>(o+0x10),value=fmuls_ppc(next,inv);
 store<f32>(o+0x10,value);store<f32>(o+0x14,old);
 if(old>zero){f32 ratio=fsubs_ppc(old,value)/old;store<f32>(o+0xC,fsubs_ppc(one,ratio));}else store<f32>(o+0xC,zero);
}
VERIFY(0x025E3EC8,ext_decOldCounter);
void ext_initOldMorf(void* self,f32 duration,u32 start,u32 end) {
 WWHD_FUNC(0x025E3F3C,void,self,duration,start,end);
 u32 o=ea(self);f32 zero=load<f32>(0x100586DC);
 if(duration>zero){f32 one=load<f32>(0x100586D4),inv=one/duration;store<f32>(o+0x10,one);store<f32>(o+0x14,one);store<f32>(o+0xC,one);store<f32>(o+4,duration);store<f32>(o+8,inv);call(0x025E3EC8,self);store<u16>(o+0x1A,end);store<u16>(o+0x18,start);}
 else{store<u16>(o+0x18,start);store<f32>(o+0x10,zero);store<f32>(o+0x14,zero);store<f32>(o+0xC,zero);store<u16>(o+0x1A,end);store<f32>(o+4,zero);store<f32>(o+8,zero);}
}
VERIFY(0x025E3F3C,ext_initOldMorf);
u32 ext_3FB4(void* self,void* old,u32 count,void* table,void* model) {
 WWHD_FUNC(0x025E3FB4,u32,self,old,count,table,model);
 u32 o=ea(self);if(!o){o=call<u32>(0x0273AD10,0xB8);if(!o)return 0;}
 call(0x025E3BCC,at<void>(o),count,table,model);store<u32>(o,0x10058C98);call(0x028F521C,at<void>(o+0x8C),0x14);store<u32>(o+0xAC,ea(old));
 store<u32>(o+0xA0,0);store<u32>(o+0xB0,0);store<u32>(o+0xB4,0);store<u32>(o+0xA4,0);store<u32>(o+0x9C,0);store<u32>(o+0xA8,0);return o;
}
VERIFY(0x025E3FB4,ext_3FB4);
void ext_3A6C(void* self,u32 index) {
 WWHD_FUNC(0x025E3A6C,void,self,index);
 u32 o=ea(self),offset=index<<4,entry=load<u32>(o+0x7C)+offset;
 if(!load<u32>(entry+4))return;u32 anim=load<u32>(entry+4),stride=index*0x68;
 u32 buffer=load<u32>(o+0x84)+stride,data=load<u32>(anim+8);call(0x027E0368,at<void>(buffer),at<void>(data));
 buffer=load<u32>(o+0x84)+stride;data=load<u32>(buffer+0x44);u32 relative=load<u32>(data+0x24),reference=load<u32>(o+0x6C),resolved=relative?data+0x24+relative:0;
 if(reference==resolved){call(0x027E00AC,at<void>(buffer),at<void>(reference));}
 else{entry=load<u32>(o+0x7C)+offset;anim=load<u32>(entry+4);data=load<u32>(anim+8);u32 kind=call<u32>(0x027E2AF4,at<void>(data),at<void>(reference));
 if((kind&0xFFFF)==0){buffer=load<u32>(o+0x84)+stride;reference=load<u32>(o+0x6C);call(0x027E00AC,at<void>(buffer),at<void>(reference));}
 else{entry=load<u32>(o+0x7C)+offset;anim=load<u32>(entry+4);data=load<u32>(anim+8);call(0x027E2CFC,at<void>(data));buffer=load<u32>(o+0x84)+stride;reference=load<u32>(o+0x6C);call(0x027DFF1C,at<void>(buffer),at<void>(reference));}}
 u32 list=load<u32>(o+0x7C),buffers=load<u32>(o+0x84);entry=list+offset;u32 control=load<u32>(buffers+stride);anim=load<u32>(entry+4);
 u32 target=load<u32>(control+0x10);f32 second=load<f32>(control+4);u32 receiver=load<u32>(control+0x14);f32 third=load<f32>(control+8),first=load<f32>(anim);
 f32 result=call_ptr<f32>(target,at<void>(receiver),first,second,third);store<f32>(control,result);
}
VERIFY(0x025E3A6C,ext_3A6C);
u32 ext_3BCC(void* self,s32 count,void* table,void* model) {
 WWHD_FUNC(0x025E3BCC,u32,self,count,table,model);
 u32 o=ea(self);if(!o){o=call<u32>(0x0273AD10,0x8C);if(!o)return 0;}
 store<u32>(o,0x1016E54C);call(0x027DA984,at<void>(o+4));
 store<u32>(o+0x78,count);store<u32>(o+0x84,0);store<u32>(o+0x48,0);store<u32>(o,0x10058C70);store<u32>(o+0x38,0x1016D820);store<u32>(o+0x70,0);store<u32>(o+0x88,0);store<u32>(o+0x7C,ea(table));store<u32>(o+0x6C,0);store<u32>(o+0x74,0);store<u32>(o+0x80,ea(model));
 s32 total=count;
 for(s32 i=0;i<total;i++){u32 entry=load<u32>(o+0x7C)+u32(i)*16;
 if(!load<u32>(entry+4)){s32 n=load<s32>(entry+8);for(s32 j=0;j<n;j++){u32 data=load<u32>(entry+0xC);store<f32>(data+u32(j)*4,load<f32>(0x100586DC));n=load<s32>(entry+8);}total=load<s32>(o+0x78);}}
 u32 buffers=call<u32>(0x028EFFD0,0,count,0x68,at<void>(0x025EE17C));store<u32>(o+0x84,buffers);
 u32 pointers=call<u32>(0x0273ADAC,u32(count)<<2);store<u32>(o+0x88,pointers);
 Local<u8[0x30]> desc; /* game test 2026-10-05: 027DFCCC/027DFB70 fill desc+0x10..+0x30 */
 for(s32 i=0;i<count;i++){
 u32 mod=load<u32>(o+0x80),md=load<u32>(mod+0xAC),joints=call<u32>(0x027F3F94,at<void>(md));store<u32>(o+0x6C,joints);u32 first=load<u16>(joints+8);
 store<u32>(ea(desc.get())+20,0);store<u8>(ea(desc.get())+13,0);store<u32>(ea(desc.get())+16,0);store<u32>(ea(desc.get()),0xFFFFFFFF);store<u8>(ea(desc.get())+12,1);store<u32>(ea(desc.get())+8,0xFFFFFFFF);store<u32>(ea(desc.get())+4,0xFFFFFFFF);
 joints=load<u32>(o+0x6C);u32 second=load<u16>(joints+8);store<u32>(ea(desc.get())+4,first);store<u32>(ea(desc.get()),second);store<u8>(ea(desc.get())+13,1);store<u32>(ea(desc.get())+8,first<<2);
 u32 size=call<u32>(0x027DFCCC,desc.get());u32 array=load<u32>(o+0x88);u32 buffer=call<u32>(0x025E3A34,size,0x40);store<u32>(array+u32(i)*4,buffer);
 buffers=load<u32>(o+0x84);array=load<u32>(o+0x88);buffer=load<u32>(array+u32(i)*4);call(0x027DFB70,at<void>(buffers+u32(i)*0x68),desc.get(),at<void>(buffer),size);
 u32 entry=load<u32>(o+0x7C)+u32(i)*16;if(load<u32>(entry+4))call(0x025E3A6C,at<void>(o),i);
 }return o;
}
VERIFY(0x025E3BCC,ext_3BCC);
void ext_3DEC(void* self,void* buffer,void* targetObj) {
 WWHD_FUNC(0x025E3DEC,void,self,buffer,targetObj);
 if(!ea(targetObj))return;u32 o=ea(self),b=ea(buffer),vt=load<u32>(b+0x34),target=load<u32>(vt+0x1C);call_ptr(target,buffer);
 u32 data=load<u32>(b+0x44),flags=load<u32>(o+8),kind=load<u32>(data+0xC);u32 index=(flags&2)|((kind&0x7000)>>12);u32 entry=0x10058C50+(index<<3);
 s16 slot=load<s16>(entry+2),adjust=load<s16>(entry);u32 receiver=o+s32(adjust);
 if(slot<0)target=load<u32>(entry+4);else{ s16 voff=load<s16>(entry+6);vt=load<u32>(receiver+s32(voff));target=load<u32>(vt+u32(s32(slot)*8)+4);}
 call_ptr(target,at<void>(receiver),buffer,targetObj);
}
VERIFY(0x025E3DEC,ext_3DEC);
void ext_4050(void* self,u32 index) {
 WWHD_FUNC(0x025E4050,void,self,index);
 u32 o=ea(self),model=load<u32>(o+0x80),data=load<u32>(model+0xAC);
 call(0x027F36FC,self,at<void>(data),index);u32 joints=call<u32>(0x027F68FC,at<void>(data));store<u32>(o+0x6C,joints);
 Local<u8[0x14]> desc; /* game test 2026-10-05: 027E06E0/027E063C fill desc+4..+0x14 */store<u32>(ea(desc.get())+4,0);store<u32>(ea(desc.get()),0xFFFFFFFF);store<u32>(ea(desc.get())+8,0);
 u32 first=load<u16>(joints+8);u32 second=load<u16>(joints+8);store<u32>(ea(desc.get()),second);
 u32 size=call<u32>(0x027E06E0,desc.get()),buffer=call<u32>(0x025E3A34,size,0x40);
 store<u32>(o+0xA0,buffer);call(0x027E063C,at<void>(o+0x8C),desc.get(),at<void>(buffer),size);
 u32 old=call<u32>(0x0273ADAC,second*0x38);store<u32>(o+0xA4,old);
}
VERIFY(0x025E4050,ext_4050);
