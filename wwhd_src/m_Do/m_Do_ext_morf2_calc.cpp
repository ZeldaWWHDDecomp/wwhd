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
void ext2_morfCalc(void* self,void* model){
 WWHD_FUNC(0x025E67A4,void,self,model);u32 o=ea(self),reference=load<u32>(ea(model)+0x2C),joints=load<u32>(reference);call(0x027E0710,at<void>(o+0x78));f32 zero=load<f32>(0x100586DC),one=load<f32>(0x100586D4);u32 anim[2]={load<u32>(o+0x94),load<u32>(o+0x98)};
 Local<u8[304]> workspace;u32 frame=ea(workspace.get())-8;
 if(load<u32>(o+0x90)&&anim[0]){f32 mix=load<f32>(o+0xC0);store<f32>(frame+0xC0,anim[1]?fsubs_ppc(one,mix):one);store<f32>(frame+0xC4,anim[1]?mix:zero);for(u32 n=0;n<2&&anim[n];n++){store<u32>(0x104B4628,o);call(0x027F363C,self,at<void>(anim[n]));u32 c=load<u32>(o+4);f32 v=call_ptr<f32>(load<u32>(c+0x10),at<void>(load<u32>(c+0x14)),load<f32>(anim[n]),load<f32>(c+4),load<f32>(c+8));store<f32>(c,v);call(0x027E0738,at<void>(o+0x78),at<void>(o+4),load<f32>(frame+0xC0+n*4));}}
 store<u32>(o+0x80,load<u32>(o+0x80)|8);u32 count=load<u16>(joints+8),record=load<u32>(o+0x78);
 for(u32 i=0;i<count;i++,record+=0x38){u32 transforms=load<u32>(o+0x9C),quats=load<u32>(o+0xA0),info=transforms?transforms+i*32:frame+0x110,quat=quats?quats+i*16:frame+0x100,scale=record+4,translation=record+0x10;
 if(!load<u32>(o+0x94)){u32 data=load<u32>(load<u32>(o+0x90)+0xAC),num=load<u32>(data+4),joint=load<u32>(data+8),index=i&0xFFFF;if(index<num)joint+=index*0x1C;u32 source=call<u32>(0x027F3CD0,at<void>(joint));for(u32 j=0;j<3;j++)store<f32>(info+j*4,load<f32>(source+j*4));for(u32 j=0;j<3;j++)store<s16>(info+0xC+j*2,load<s16>(source+0xC+j*2));for(u32 j=0;j<3;j++)store<f32>(info+0x14+j*4,load<f32>(source+0x14+j*4));u32 before=load<u32>(o+0xC8);if(before)call_ptr(load<u32>(load<u32>(before)+0xC),at<void>(before),index,at<void>(info),(f32)cpu->f[1].ps0,(f32)cpu->f[2].ps0,(f32)cpu->f[3].ps0);call(0x027ED2D0,(s32)load<s16>(info+0xC),(s32)load<s16>(info+0xE),(s32)load<s16>(info+0x10),at<void>(quat));
 }else{bool blend=load<f32>(o+0xB4)<one&&transforms&&quats&&!load<u8>(o+0xD0);bool second=load<u32>(o+0x98)!=0;u32 temp=blend?frame+8:info;
 if(!second){u32 flags=load<u32>(o+0x54),matrix=load<u32>(o+0x30)+i*0x30;store<u32>(o+0x54,flags|8);for(u32 j=0;j<3;j++){s32 angle=call<s32>(0x02019510,load<f32>(matrix+0x20+j*4));store<s16>(temp+0xC+j*2,(s16)angle);}for(u32 j=0;j<3;j++)store<u32>(temp+j*4,load<u32>(record+4+j*4));for(u32 j=0;j<3;j++)store<u32>(temp+0x14+j*4,load<u32>(record+0x10+j*4));u32 before=load<u32>(o+0xC8);if(before)call_ptr(load<u32>(load<u32>(before)+0xC),at<void>(before),i&0xFFFF,at<void>(temp),(f32)cpu->f[1].ps0,(f32)cpu->f[2].ps0,(f32)cpu->f[3].ps0);call(0x027ED2D0,(s32)load<s16>(temp+0xC),(s32)load<s16>(temp+0xE),(s32)load<s16>(temp+0x10),at<void>(blend?frame+0x28:quat));
 }else{u32 basis=blend?0x78:0x68;ext_basis(record,frame,basis);for(u32 j=0;j<12;j++)store<f32>(frame+0xC8+j*4,load<f32>(frame+basis+j*4));u32 out=blend?frame+0x68:frame+8;call(0x025EE5F0,at<void>(out),at<void>(frame+0xC8));for(u32 j=0;j<4;j++)store<f32>((blend?frame+0x28:quat)+j*4,load<f32>(out+j*4));for(u32 j=0;j<3;j++)store<u32>(temp+j*4,load<u32>(record+4+j*4));for(u32 j=0;j<3;j++)store<u32>(temp+0x14+j*4,load<u32>(record+0x10+j*4));}
 if(blend){f32 previous=load<f32>(o+0xB8),current=load<f32>(o+0xB4),ratio=fsubs_ppc(current,previous)/fsubs_ppc(one,previous),oldRatio=fsubs_ppc(one,ratio);call(0x027ED3AC,at<void>(quat),at<void>(frame+0x28),at<void>(quat),ratio);for(u32 j=0;j<3;j++){f32 fresh=load<f32>(temp+0x14+j*4),old=load<f32>(info+0x14+j*4);store<f32>(info+0x14+j*4,fmadds(old,oldRatio,fmuls_ppc(fresh,ratio)));}for(u32 j=0;j<3;j++){f32 fresh=load<f32>(temp+j*4),old=load<f32>(info+j*4);store<f32>(info+j*4,fmadds(old,oldRatio,fmuls_ppc(fresh,ratio)));}}
 }
 u32 after=load<u32>(o+0xCC);if(after)call_ptr(load<u32>(load<u32>(after)+0xC),at<void>(after),i&0xFFFF);
 f32 w=load<f32>(quat+0xC),x=load<f32>(quat),y=load<f32>(quat+4),z=load<f32>(quat+8),dw=fadds_ppc(w,w),dx=fadds_ppc(x,x),dy=fadds_ppc(y,y),dz=fadds_ppc(z,z);f32 yy=fnmsubs(dy,y,one),xy=fmuls_ppc(dx,y),yz=fmuls_ppc(dy,z),wy=fmuls_ppc(dw,y),xx=fnmsubs(dx,x,one),zz=fmuls_ppc(dz,z),wz=fmuls_ppc(dw,z);
 store<f32>(record+0x28,fmadds(dx,z,wy));store<f32>(record+0x24,fsubs_ppc(xy,wz));store<f32>(record+0x20,fsubs_ppc(yy,zz));store<f32>(record+0x2C,fadds_ppc(xy,wz));store<f32>(record+0x30,fsubs_ppc(xx,zz));store<f32>(record+0x34,fnmsubs(dw,x,yz));
 f32 sc[3]={load<f32>(info),load<f32>(info+4),load<f32>(info+8)};store<f32>(scale,sc[0]);store<f32>(scale+8,sc[2]);store<f32>(scale+4,sc[1]);f32 tr[3]={load<f32>(info+0x14),load<f32>(info+0x18),load<f32>(info+0x1C)};store<f32>(translation,tr[0]);store<f32>(translation+4,tr[1]);store<f32>(translation+8,tr[2]);
 if(load<f32>(quat)!=zero||load<f32>(quat+4)!=zero||load<f32>(quat+8)!=zero||load<f32>(quat+0xC)!=one)store<u32>(record,load<u32>(record)|0x04000000);if(load<f32>(info)!=one||load<f32>(info+4)!=one||load<f32>(info+8)!=one)store<u32>(record,load<u32>(record)|0x03000000);if(load<f32>(info+0x14)!=zero||load<f32>(info+0x18)!=zero||load<f32>(info+0x1C)!=zero)store<u32>(record,load<u32>(record)|0x08000000);cpu->f[2].ps0=zz;cpu->f[3].ps0=xy;count=load<u16>(joints+8);}
 store<u8>(o+0xD0,0);
}
VERIFY(0x025E67A4,ext2_morfCalc);
void ext2_jointEntry(void* self,void* model){WWHD_FUNC(0x025E7270,void,self,model);call(0x027E0834,at<void>(ea(self)+0x78),at<void>(load<u32>(ea(model)+0x2C)));}
VERIFY(0x025E7270,ext2_jointEntry);
void ext2_jointCalc(void* self,void* model){WWHD_FUNC(0x025E727C,void,self,model);u32 r=load<u32>(ea(model)+0x2C);store<u32>(r+0x30,ea(self));r=load<u32>(ea(model)+0x2C);store<u16>(r+0x2E,0);call(0x025E67A4,self,model);call(0x025E7270,self,model);}
VERIFY(0x025E727C,ext2_jointCalc);
