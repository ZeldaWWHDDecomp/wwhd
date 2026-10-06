#include "gabi.h"
using namespace gabi;
static void ext_basis(u32 record,u32 frame,u32 basis) {
 f32 x=load<f32>(record+0x20),y=load<f32>(record+0x24),z=load<f32>(record+0x28);
 f32 a=load<f32>(record+0x2C),b=load<f32>(record+0x30),c=load<f32>(record+0x34);
 f32 cross[3]={fmsubs(y,c,fmuls_ppc(z,b)),fmsubs(z,a,fmuls_ppc(x,c)),fmsubs(x,b,fmuls_ppc(y,a))};
 f32 lenA=fmadds(c,c,fadds_ppc(fmuls_ppc(a,a),fmuls_ppc(b,b)));
 f32 lenB=fmadds(cross[2],cross[2],fadds_ppc(fmuls_ppc(cross[0],cross[0]),fmuls_ppc(cross[1],cross[1])));
 if(lenA!=0&&lenB!=0) {
 f32 ra=(f32)frsqrte(lenA),rb=(f32)frsqrte(lenB),half=load<f32>(0x100586D8),three=load<f32>(0x100589F4);
 ra=fmuls_ppc(ra,fnmsubs(fmuls_ppc(fmuls_ppc(lenA,half),ra),ra,three));
 rb=fmuls_ppc(rb,fnmsubs(fmuls_ppc(fmuls_ppc(lenB,half),rb),rb,three));
 f32 v[3]={fmuls_ppc(a,ra),fmuls_ppc(b,ra),fmuls_ppc(c,ra)};
 f32 w[3]={fmuls_ppc(cross[0],rb),fmuls_ppc(cross[1],rb),fmuls_ppc(cross[2],rb)};
 for(u32 i=0;i<3;i++){store<f32>(frame+basis+16+i*4,v[i]);store<f32>(frame+basis+32+i*4,w[i]);}
 store<f32>(frame+basis,fmsubs(v[1],w[2],fmuls_ppc(v[2],w[1])));
 store<f32>(frame+basis+4,fmsubs(v[2],w[0],fmuls_ppc(v[0],w[2])));
 store<f32>(frame+basis+8,fmsubs(v[0],w[1],fmuls_ppc(v[1],w[0])));
 }
}
void ext_4944(void* self,void* iterator) {
 WWHD_FUNC(0x025E4944,void,self,iterator);
 u32 o=ea(self),it=ea(iterator),node=load<u32>(it),old=load<u32>(o+0xAC),index=load<u32>(node),target=load<u32>(o+0xB4);
 u32 quats=load<u32>(old+0x20),trans=load<u32>(old+0x1C),receiver=load<u32>(o+0xA8);
 call_ptr(target,at<void>(receiver),index&0xFFFF,at<void>(trans+(index<<5)),at<void>(quats+(index<<4)));
 u32 model=load<u32>(o+0x80),data=load<u32>(model+0x2C),count=load<u16>(data+0x2C),next=index+1;
 if(s32(next)<s32(count)){store<u32>(it+4,next);return;}
 store<u32>(it+4,0xFFFFFFFF);old=load<u32>(o+0xAC);if(!load<u8>(old))store<u8>(old,1);
}
VERIFY(0x025E4944,ext_4944);
void ext_4A10(void* self,void* argument) {
 WWHD_FUNC(0x025E4A10,void,self,argument);call(0x025E410C,self,argument);call(0x025E46B4,self,argument);
}
VERIFY(0x025E4A10,ext_4A10);
void ext_410C(void* self,void* unused) {
 WWHD_FUNC(0x025E410C,void,self,unused);
 u32 o=ea(self);call(0x027E0710,at<void>(o+0x8C));s32 count=load<s32>(o+0x78);
 for(s32 i=0;i<count;i++){u32 entry=load<u32>(o+0x7C)+u32(i)*16;if(load<u32>(entry+4)){u32 buffers=load<u32>(o+0x84),data=load<u32>(entry+12);call(0x025E3DEC,at<void>(o+0x8C),at<void>(buffers+u32(i)*0x68),at<void>(data));count=load<s32>(o+0x78);}}
 u32 flags=load<u32>(o+0x94),n=load<u16>(o+0x92);store<u32>(o+0x94,flags|8);u32 base=load<u32>(o+0x8C);
 Local<u8[196]> workspace;u32 frame=ea(workspace.get())-8;
 f32 zero=load<f32>(0x100586DC),one=load<f32>(0x100586D4),epsilon=load<f32>(0x100589F8);
 for(u32 i=0;i<n;i++){
 u32 record=base+i*0x38;if(!(load<f32>(record+0x1C)<epsilon)){
 u32 old=load<u32>(o+0xA4);call(0xC000A848,at<void>(old+i*0x38),at<void>(record),0x38);
 ext_basis(record,frame,0x60);
 for(u32 j=0;j<12;j++)store<f32>(frame+0x9C+j*4,load<f32>(frame+0x60+j*4));
 call(0x025EE5F0,at<void>(frame+0x50),at<void>(frame+0x9C));
 for(u32 j=0;j<4;j++)store<f32>(frame+0x28+j*4,load<f32>(frame+0x50+j*4));
 for(u32 j=0;j<3;j++)store<f32>(frame+8+j*4,zero);
 for(u32 j=0;j<3;j++)store<f32>(frame+8+j*4,load<f32>(record+4+j*4));
 for(u32 j=0;j<3;j++)store<f32>(frame+0x1C+j*4,zero);
 for(u32 j=0;j<3;j++)store<f32>(frame+0x1C+j*4,load<f32>(record+0x10+j*4));
 u32 oldframe=load<u32>(o+0xAC);f32 priorRatio=0,newRatio=0;
 if(load<u8>(oldframe)&&load<f32>(oldframe+0xC)>zero&&load<f32>(record+0x1C)>zero){
 priorRatio=load<f32>(oldframe+0xC);newRatio=fsubs_ppc(one,priorRatio);u32 quats=load<u32>(oldframe+0x20),trans=load<u32>(oldframe+0x1C),previous=trans+i*0x20;
 call(0x027ED3AC,at<void>(quats+i*16),at<void>(frame+0x28),at<void>(frame+0x28),newRatio);
 for(u32 j=0;j<3;j++){f32 v=load<f32>(previous+0x14+j*4),temp=load<f32>(frame+0x1C+j*4);store<f32>(frame+0x1C+j*4,fmadds(temp,newRatio,fmuls_ppc(v,priorRatio)));}
 for(u32 j=0;j<3;j++){f32 v=load<f32>(previous+j*4),temp=load<f32>(frame+8+j*4);store<f32>(frame+8+j*4,fmadds(temp,newRatio,fmuls_ppc(v,priorRatio)));}
 }
 u32 target=load<u32>(o+0xB0),receiver=load<u32>(o+0xA8);call_ptr(target,at<void>(receiver),i&0xFFFF,at<void>(frame+8),at<void>(frame+0x28));
 f32 w=load<f32>(frame+0x34),x=load<f32>(frame+0x28),z=load<f32>(frame+0x30),y=load<f32>(frame+0x2C);
 f32 dw=fadds_ppc(w,w),dx=fadds_ppc(x,x),dz=fadds_ppc(z,z),dy=fadds_ppc(y,y);
 f32 a=fnmsubs(dy,y,one),b=fmuls_ppc(dx,y),c=fmuls_ppc(dy,z),d=fmuls_ppc(dw,y),e=fnmsubs(dx,x,one),f=fmuls_ppc(dz,z),g=fmuls_ppc(dw,z);
 store<f32>(record+0x20,fsubs_ppc(a,f));store<f32>(record+0x24,fsubs_ppc(b,g));store<f32>(record+0x28,fmadds(dx,z,d));store<f32>(record+0x30,fsubs_ppc(e,f));store<f32>(record+0x34,fnmsubs(dw,x,c));store<f32>(record+0x2C,fadds_ppc(b,g));
 f32 sc[3]={load<f32>(frame+8),load<f32>(frame+12),load<f32>(frame+16)};for(u32 j=0;j<3;j++)store<f32>(record+4+j*4,sc[j]);
 f32 tr[3]={load<f32>(frame+0x1C),load<f32>(frame+0x20),load<f32>(frame+0x24)};store<f32>(record+0x10,tr[0]);store<f32>(record+0x18,tr[2]);store<f32>(record+0x14,tr[1]);
 if(load<f32>(frame+0x28)!=zero||load<f32>(frame+0x2C)!=zero||load<f32>(frame+0x30)!=zero||load<f32>(frame+0x34)!=one)store<u32>(record,load<u32>(record)|0x04000000);
 if(load<f32>(frame+8)!=one||load<f32>(frame+12)!=one||load<f32>(frame+16)!=one)store<u32>(record,load<u32>(record)|0x03000000);
 if(load<f32>(frame+0x1C)!=zero||load<f32>(frame+0x20)!=zero||load<f32>(frame+0x24)!=zero)store<u32>(record,load<u32>(record)|0x08000000);
 }n=load<u16>(o+0x92);
 }
}
VERIFY(0x025E410C,ext_410C);
void ext_46B4(void* self,void* data) {
 WWHD_FUNC(0x025E46B4,void,self,data);
 u32 o=ea(self),old=load<u32>(o+0xAC),flags=load<u32>(o+0x94),records=load<u32>(o+0x8C),n=load<u16>(o+0x92);store<u32>(o+0x94,flags|8);
 u32 trans=load<u32>(old+0x1C),quats=load<u32>(old+0x20);f32 zero=load<f32>(0x100586DC);
 Local<u8[148]> workspace;u32 frame=ea(workspace.get())-8;
 for(u32 i=0;i<n;i++){
 u32 record=records+i*0x38;
 if(load<f32>(record+0x1C)>zero){
 ext_basis(record,frame,0x30);for(u32 j=0;j<12;j++)store<f32>(frame+0x6C+j*4,load<f32>(frame+0x30+j*4));
 call(0x025EE5F0,at<void>(frame+0x20),at<void>(frame+0x6C));
 for(u32 j=0;j<4;j++)store<f32>(quats+i*16+j*4,load<f32>(frame+0x20+j*4));
 for(u32 j=0;j<3;j++)store<f32>(trans+i*0x20+j*4,load<f32>(record+4+j*4));
 for(u32 j=0;j<3;j++)store<f32>(trans+i*0x20+0x14+j*4,load<f32>(record+0x10+j*4));
 n=load<u16>(o+0x92);
 }
 }
 u32 reference=load<u32>(ea(data)+0x2C);call(0x027E0834,at<void>(o+0x8C),at<void>(reference));
}
VERIFY(0x025E46B4,ext_46B4);
