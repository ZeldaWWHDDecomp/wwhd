#include "gabi.h"
using namespace gabi;
void ext_5C2C(void* self,void* model){WWHD_FUNC(0x025E5C2C,void,self,model);u32 reference=load<u32>(ea(model)+0x2C);call(0x027E0834,at<void>(ea(self)+0x78),at<void>(reference));}
VERIFY(0x025E5C2C,ext_5C2C);
void ext_5C38(void* self,void* model){WWHD_FUNC(0x025E5C38,void,self,model);u32 reference=load<u32>(ea(model)+0x2C);store<u32>(reference+0x30,ea(self));reference=load<u32>(ea(model)+0x2C);store<u16>(reference+0x2E,0);call(0x025E564C,self,model);call(0x025E5C2C,self,model);}
VERIFY(0x025E5C38,ext_5C38);
void ext_morf2Error(void* self){WWHD_FUNC(0x025E5C90,void,self);u32 o=ea(self),audio=load<u32>(o+0xC4);if(audio){call(0x02801D3C,at<void>(audio));store<u32>(o+0xC4,0);}u32 transforms=load<u32>(o+0x9C),quats=load<u32>(o+0xA0);if(transforms)store<u32>(o+0x9C,0);u32 model=load<u32>(o+0x90);if(quats)store<u32>(o+0xA0,0);if(model){call_ptr(load<u32>(load<u32>(model+0xC)+0xC),at<void>(model),2);store<u32>(o+0x90,0);}}
VERIFY(0x025E5C90,ext_morf2Error);
void ext_morf2SetMorf(void* self,f32 duration){WWHD_FUNC(0x025E5D1C,void,self,duration);u32 o=ea(self);f32 zero=load<f32>(0x100586DC),one=load<f32>(0x100586D4);if(!(duration>zero)){store<f32>(o+0xB4,one);store<f32>(o+0xB8,one);return;}f32 delta=one/duration;store<f32>(o+0xB4,zero);store<f32>(o+0xB8,zero);store<f32>(o+0xBC,delta);}
VERIFY(0x025E5D1C,ext_morf2SetMorf);

void ext_morfCalc(void* self,void* model){
 WWHD_FUNC(0x025E564C,void,self,model);
 u32 o=ea(self),reference=load<u32>(ea(model)+0x2C),joints=load<u32>(reference);call(0x027E0710,at<void>(o+0x78));f32 one=load<f32>(0x100586D4);
 if(load<u32>(o+0x90)&&load<u32>(o+0x94)){store<u32>(0x104B4628,o);u32 animation=load<u32>(o+0x94);call(0x027F363C,self,at<void>(animation));u32 control=load<u32>(o+4);animation=load<u32>(o+0x94);u32 target=load<u32>(control+0x10),receiver=load<u32>(control+0x14);f32 second=load<f32>(control+4),third=load<f32>(control+8),first=load<f32>(animation);f32 result=call_ptr<f32>(target,at<void>(receiver),first,second,third);store<f32>(control,result);call(0x027E0738,at<void>(o+0x78),at<void>(o+4),one);}
 store<u32>(o+0x80,load<u32>(o+0x80)|8);u32 count=load<u16>(joints+8),record=load<u32>(o+0x78);f32 zero=load<f32>(0x100586DC);
 Local<u8[96]> workspace;u32 frame=ea(workspace.get())-8;
 for(u32 i=0;i<count;i++,record+=0x38){u32 transforms=load<u32>(o+0xA8),quats=load<u32>(o+0xAC),info=transforms?transforms+i*32:frame+0x48,quat=quats?quats+i*16:frame+0x38;
 u32 before=0,after=0,scale=0,translation=0;
 if(!load<u32>(o+0x94)){u32 currentModel=load<u32>(o+0x90),data=load<u32>(currentModel+0xAC),num=load<u32>(data+4),joint=load<u32>(data+8),index=i&0xFFFF;if(index<num)joint+=index*0x1C;u32 source=call<u32>(0x027F3CD0,at<void>(joint));
 for(u32 j=0;j<3;j++)store<f32>(info+j*4,load<f32>(source+j*4));for(u32 j=0;j<3;j++)store<s16>(info+0xC+j*2,load<s16>(source+0xC+j*2));for(u32 j=0;j<3;j++)store<f32>(info+0x14+j*4,load<f32>(source+0x14+j*4));
 scale=record+4;translation=record+0x10;before=load<u32>(o+0xC0);if(before)call_ptr(load<u32>(load<u32>(before)+0xC),at<void>(before),index,at<void>(info));call(0x027ED2D0,(s32)load<s16>(info+0xC),(s32)load<s16>(info+0xE),(s32)load<s16>(info+0x10),at<void>(quat));after=load<u32>(o+0xC4);
 }else{f32 progress=load<f32>(o+0xB0);bool blend=progress<one&&transforms&&quats;u32 flags=load<u32>(o+0x54),matrix=load<u32>(o+0x30)+i*0x30;store<u32>(o+0x54,flags|8);u32 temp=blend?frame+8:info;
 for(u32 j=0;j<3;j++){s32 angle=call<s32>(0x02019510,load<f32>(matrix+0x20+j*4));store<s16>(temp+0xC+j*2,(s16)angle);}
 for(u32 j=0;j<3;j++)store<u32>(temp+j*4,load<u32>(record+4+j*4));for(u32 j=0;j<3;j++)store<u32>(temp+0x14+j*4,load<u32>(record+0x10+j*4));
 scale=record+4;translation=record+0x10;before=load<u32>(o+0xC0);if(before)call_ptr(load<u32>(load<u32>(before)+0xC),at<void>(before),i&0xFFFF,at<void>(temp));
 if(blend){f32 previous=load<f32>(o+0xB4),current=load<f32>(o+0xB0),ratio=fsubs_ppc(current,previous)/fsubs_ppc(one,previous),oldRatio=fsubs_ppc(one,ratio);call(0x027ED2D0,(s32)load<s16>(temp+0xC),(s32)load<s16>(temp+0xE),(s32)load<s16>(temp+0x10),at<void>(frame+0x28));call(0x027ED3AC,at<void>(quat),at<void>(frame+0x28),at<void>(quat),ratio);
 for(u32 j=0;j<3;j++){f32 fresh=load<f32>(temp+0x14+j*4),old=load<f32>(info+0x14+j*4);store<f32>(info+0x14+j*4,fmadds(old,oldRatio,fmuls_ppc(fresh,ratio)));}for(u32 j=0;j<3;j++){f32 fresh=load<f32>(temp+j*4),old=load<f32>(info+j*4);store<f32>(info+j*4,fmadds(old,oldRatio,fmuls_ppc(fresh,ratio)));}
 }else call(0x027ED2D0,(s32)load<s16>(info+0xC),(s32)load<s16>(info+0xE),(s32)load<s16>(info+0x10),at<void>(quat));after=load<u32>(o+0xC4);}
 if(after)call_ptr(load<u32>(load<u32>(after)+0xC),at<void>(after),i&0xFFFF);
 f32 w=load<f32>(quat+0xC),x=load<f32>(quat),y=load<f32>(quat+4),z=load<f32>(quat+8),dw=fadds_ppc(w,w),dx=fadds_ppc(x,x),dy=fadds_ppc(y,y),dz=fadds_ppc(z,z);f32 yy=fnmsubs(dy,y,one),xy=fmuls_ppc(dx,y),yz=fmuls_ppc(dy,z),wy=fmuls_ppc(dw,y),xx=fnmsubs(dx,x,one),zz=fmuls_ppc(dz,z),wz=fmuls_ppc(dw,z);
 store<f32>(record+0x28,fmadds(dx,z,wy));store<f32>(record+0x24,fsubs_ppc(xy,wz));store<f32>(record+0x20,fsubs_ppc(yy,zz));store<f32>(record+0x2C,fadds_ppc(xy,wz));store<f32>(record+0x30,fsubs_ppc(xx,zz));store<f32>(record+0x34,fnmsubs(dw,x,yz));
 f32 sc[3]={load<f32>(info),load<f32>(info+4),load<f32>(info+8)};store<f32>(scale,sc[0]);store<f32>(scale+8,sc[2]);store<f32>(scale+4,sc[1]);f32 tr[3]={load<f32>(info+0x14),load<f32>(info+0x18),load<f32>(info+0x1C)};store<f32>(translation,tr[0]);store<f32>(translation+4,tr[1]);store<f32>(translation+8,tr[2]);
 if(load<f32>(quat)!=zero||load<f32>(quat+4)!=zero||load<f32>(quat+8)!=zero||load<f32>(quat+0xC)!=one)store<u32>(record,load<u32>(record)|0x04000000);if(load<f32>(info)!=one||load<f32>(info+4)!=one||load<f32>(info+8)!=one)store<u32>(record,load<u32>(record)|0x03000000);if(load<f32>(info+0x14)!=zero||load<f32>(info+0x18)!=zero||load<f32>(info+0x1C)!=zero)store<u32>(record,load<u32>(record)|0x08000000);count=load<u16>(joints+8);
 }
}
VERIFY(0x025E564C,ext_morfCalc);
