#include "wwhd.h"
#include "gabi.h"
s32 Snap_PhotoIndex(s32 index) {

 WWHD_FUNC(0x025BD600,s32,index);

 u32 n=u32(index)-73;
 if(n>=134)gabi::call<void>(0x0273AA24,0x10054618,0x1A7,0x10054624);
return s32(n);

}

VERIFY(0x025BD600,Snap_PhotoIndex);

u32 Snap_Room(s32 index) {

 WWHD_FUNC(0x025BD64C,u32,index);

 if(index>=134)return 255;
return gabi::load<u8>(0x10054718+u32(index)*18);

}

VERIFY(0x025BD64C,Snap_Room);

void Snap_Project(u32 in,u32 dst) {

 WWHD_FUNC(0x025BD670,void,in,dst);

 u32 packet=gabi::load<u32>(0x1047C9BC);
u32 projection=gabi::load<u32>(packet+0xF84),viewport=gabi::load<u32>(packet+0xF88),camera=gabi::load<u32>(packet+0xF80);
gabi::Local<u8[24]> params;
gabi::call<void>(0x0274F5F0,params.a,viewport);
gabi::Local<u8[12]> vec;
f32 z=gabi::load<f32>(in+8),x=gabi::load<f32>(in),y=gabi::load<f32>(in+4);
gabi::store<f32>(vec.a+8,z);
gabi::store<f32>(vec.a+4,y);
gabi::store<f32>(vec.a,x);
gabi::Local<u8[8]> xy;
gabi::call<void>(0x0274CB50,camera,xy.a,vec.a,projection,params.a);
f32 xx=gabi::load<f32>(xy.a),yy=gabi::load<f32>(xy.a+4);
gabi::store<f32>(dst,xx);
gabi::store<f32>(dst+4,yy);
gabi::store<f32>(dst+8,0.0f);

}

VERIFY(0x025BD670,Snap_Project);

u32 Snap_ObjCtor(u32 s) {

 WWHD_FUNC(0x025BD71C,u32,s);

 if(!s)s=gabi::call<u32>(0x0273AD10,0x34);
if(s){
gabi::store<u16>(s+0x2E,0);
gabi::store<u16>(s+0x2C,0);
gabi::store<u32>(s+0x14,0xFFFFFFFF);
gabi::store<u32>(s+0x30,0x100546AC);
gabi::store<s16>(s+0x1C,32767);
gabi::store<u16>(s+0x2A,0);
gabi::store<f32>(s+0xC,0);
gabi::store<u8>(s+0x1B,0);
gabi::store<f32>(s+0x24,0);
gabi::store<u8>(s+0x18,0);
gabi::store<f32>(s+0x10,0);
gabi::store<u32>(s+0x20,0);
gabi::store<u8>(s+0x19,4);
gabi::store<u16>(s+0x1E,0);
gabi::store<u16>(s+0x28,0);
gabi::store<u8>(s+0x1A,255);
}
return s;

}

VERIFY(0x025BD71C,Snap_ObjCtor);

u32 Snap_ElmCtor(u32 s) {

 WWHD_FUNC(0x025BD7B4,u32,s);

 if(!s)s=gabi::call<u32>(0x0273AD10,0x3C);
if(s){
gabi::store<u32>(s+0x38,0x100546BC);
gabi::call<void>(0x025BD71C,s);
gabi::store<f32>(s+0x34,0);
}
return s;

}

VERIFY(0x025BD7B4,Snap_ElmCtor);

void Snap_Init(u32 heap) {

 WWHD_FUNC(0x025BDBB0,void,heap);

 u32 packet=gabi::load<u32>(0x1047C9BC);
if(!packet){
if(!gabi::load<u32>(0x1047D974)){
gabi::store<u32>(0x1047D974,1);
gabi::call<void>(0x025BD814,0x1047C9E0);
gabi::call<void>(0x028F026C,0x101EBA28);
}
packet=0x1047C9E0;
gabi::store<u32>(0x1047C9BC,packet);
}
gabi::call<void>(0x025BD8E4,packet,heap);

}

VERIFY(0x025BDBB0,Snap_Init);

void Snap_Enable(u32 s) {

 WWHD_FUNC(0x025BDC38,void,s);

 gabi::store<u32>(s+0xF64,gabi::load<u32>(s+0xF64)|1);

}

VERIFY(0x025BDC38,Snap_Enable);

void Snap_EnableGlobal() {

 WWHD_FUNC(0x025BDC48,void);

 gabi::call<void>(0x025BDC38,gabi::load<u32>(0x1047C9BC));

}

VERIFY(0x025BDC48,Snap_EnableGlobal);

u32 Snap_Enabled(u32 s) {

 WWHD_FUNC(0x025BDC54,u32,s);

 return gabi::load<u32>(s+0xF64)&1;

}

VERIFY(0x025BDC54,Snap_Enabled);

u32 Snap_EnabledGlobal() {

 WWHD_FUNC(0x025BDC60,u32);

 return gabi::call<u32>(0x025BDC54,gabi::load<u32>(0x1047C9BC));

}

VERIFY(0x025BDC60,Snap_EnabledGlobal);

u32 Snap_Released(u32 s) {

 WWHD_FUNC(0x025BDC6C,u32,s);

 return gabi::load<u32>(s+0xF64)&2;

}

VERIFY(0x025BDC6C,Snap_Released);

void Snap_AreaClear(u32 s) {

 WWHD_FUNC(0x025BE208,void,s);

 gabi::store<s16>(s+0x28,2048);
gabi::store<s16>(s+0x2C,-2048);
gabi::store<s16>(s+0x2A,2048);
gabi::store<s16>(s+0x2E,-2048);

}

VERIFY(0x025BE208,Snap_AreaClear);

void Snap_SetArea(u32 s,s16 x,s16 y) {

 WWHD_FUNC(0x025BE224,void,s,x,y);

 s16 minx=gabi::load<s16>(s+0x28),miny=gabi::load<s16>(s+0x2A);
if(minx>x)gabi::store<s16>(s+0x28,x);
s16 maxx=gabi::load<s16>(s+0x2C);
if(miny>y)gabi::store<s16>(s+0x2A,y);
s16 maxy=gabi::load<s16>(s+0x2E);
if(maxx<x)gabi::store<s16>(s+0x2C,x);
if(maxy<y)gabi::store<s16>(s+0x2E,y);

}

VERIFY(0x025BE224,Snap_SetArea);

u32 Snap_Cull(u32 s) {

 WWHD_FUNC(0x025BE268,u32,s);

 if(!(gabi::load<u8>(s+0x1B)&1)&&gabi::load<s16>(s+0x1C)!=32767){
u32 play=gabi::call<u32>(0x025200D4);
s32 index=gabi::load<s8>(play+0x5B30);
play=gabi::call<u32>(0x025200D4);
u32 camera=gabi::load<u32>(play+u32(index)*0x34+0x5AF8);
gabi::Local<u8[12]> diff;
gabi::call<void>(0x028E8DAC,camera+0xDC,s,diff.a);
f32 z=gabi::load<f32>(diff.a+8),x=gabi::load<f32>(diff.a);
s32 angle=gabi::call<s32>(0x020195B0,x,z);
s32 distance=gabi::call<s32>(0x0200FAAC,angle,gabi::load<s16>(s+0x1E));
if(distance>gabi::load<s16>(s+0x1C))return 1;
}
return 0;

}

VERIFY(0x025BE268,Snap_Cull);

void Snap_Execute(u32 s) {

 WWHD_FUNC(0x025BE870,void,s);

 if(gabi::call<u32>(0x025BDC6C,s)){
gabi::call<void>(0x025BE664,s);
gabi::store<u32>(s+0xF64,gabi::load<u32>(s+0xF64)&~2u);
for(u32 i=0;i<63;++i)gabi::store<f32>(s+0xD4+60*i,1000000000.0f);
gabi::store<u32>(s+0x9C,0);
}

}

VERIFY(0x025BE870,Snap_Execute);

void Snap_ExecuteGlobal() {

 WWHD_FUNC(0x025BE8DC,void);

 gabi::call<void>(0x025BE870,gabi::load<u32>(0x1047C9BC));

}

VERIFY(0x025BE8DC,Snap_ExecuteGlobal);

u32 Snap_Result() {

 WWHD_FUNC(0x025BE8E8,u32);

 u32 s=gabi::load<u32>(0x1047C9BC);
return gabi::load<u32>(s+0xF68);

}

VERIFY(0x025BE8E8,Snap_Result);

u32 Snap_SpecialResult() {

 WWHD_FUNC(0x025BE8F8,u32);

 u32 s=gabi::load<u32>(0x1047C9BC);
return gabi::load<u8>(s+0xF6C);

}

VERIFY(0x025BE8F8,Snap_SpecialResult);

u32 Snap_ElmRegist(u32 s,u32 src) {

 WWHD_FUNC(0x025BE908,u32,s,src);

 gabi::store<u32>(s+0,gabi::load<u32>(src+0));
gabi::store<u32>(s+4,gabi::load<u32>(src+4));
gabi::store<u32>(s+8,gabi::load<u32>(src+8));
gabi::store<f32>(s+12,gabi::load<f32>(src+12));
gabi::store<f32>(s+16,gabi::load<f32>(src+16));
gabi::store<u32>(s+20,gabi::load<u32>(src+20));
gabi::store<u8>(s+24,gabi::load<u8>(src+24));
gabi::store<u8>(s+25,gabi::load<u8>(src+25));
gabi::store<u8>(s+26,gabi::load<u8>(src+26));
gabi::store<u8>(s+27,gabi::load<u8>(src+27));
gabi::store<s16>(s+28,gabi::load<s16>(src+28));
gabi::store<s16>(s+30,gabi::load<s16>(src+30));
gabi::store<u32>(s+32,gabi::load<u32>(src+32));
gabi::store<f32>(s+36,gabi::load<f32>(src+36));
gabi::store<s16>(s+40,gabi::load<s16>(src+40));
gabi::store<s16>(s+42,gabi::load<s16>(src+42));
gabi::store<s16>(s+44,gabi::load<s16>(src+44));
gabi::store<s16>(s+46,gabi::load<s16>(src+46));
return s;

}

VERIFY(0x025BE908,Snap_ElmRegist);

u32 Snap_RegistGlobal(u32 obj) {

 WWHD_FUNC(0x025BEB4C,u32,obj);

 return gabi::call<u32>(0x025BE99C,gabi::load<u32>(0x1047C9BC),obj);

}

VERIFY(0x025BEB4C,Snap_RegistGlobal);

void Snap_SetGeo(u32 s,u32 v,f32 radius,f32 height,s16 angle) {

 WWHD_FUNC(0x025BEB5C,void,s,v,radius,height,angle);

 gabi::store<u8>(s+0x1B,gabi::load<u8>(s+0x1B)&0xFE);
gabi::store<f32>(s,gabi::load<f32>(v));
gabi::store<f32>(s+4,gabi::load<f32>(v+4));
f32 z=gabi::load<f32>(v+8);
gabi::store<f32>(s+0xC,radius);
gabi::store<f32>(s+8,z);
gabi::store<f32>(s+0x10,height);
gabi::store<s16>(s+0x1E,angle);

}

VERIFY(0x025BEB5C,Snap_SetGeo);

void Snap_SetInf(u32 s,u32 type,u32 actor,u32 room,u32 points,s16 angle) {

 WWHD_FUNC(0x025BEB90,void,s,type,actor,room,points,angle);

 gabi::store<u8>(s+0x18,u8(type));
u32 id=actor?gabi::load<u32>(actor+4):0xFFFFFFFF;
gabi::store<u8>(s+0x19,u8(points));
gabi::store<u8>(s+0x1A,u8(room));
gabi::store<s16>(s+0x1C,angle);
gabi::store<u32>(s+0x14,id);

}

VERIFY(0x025BEB90,Snap_SetInf);

void Snap_RegistFigActor(u32 type,u32 actor,f32 x,f32 y,f32 z) {

 WWHD_FUNC(0x025BED80,void,type,actor,x,y,z);

 s16 angle=gabi::load<s16>(actor+0x32A);
gabi::call<void>(0x025BEBB8,type,actor,actor+0x314,angle,x,y,z);

}

VERIFY(0x025BED80,Snap_RegistFigActor);

void Snap_Entry() {

 WWHD_FUNC(0x025BED8C,void);

 if(gabi::call<u32>(0x025BDC54,gabi::load<u32>(0x1047C9BC))){
u32 p=gabi::call<u32>(0x025200D4);
u32 buffer=gabi::load<u32>(p+0x5D9C);
gabi::store<u32>(0x104B4638,buffer);
gabi::call<void>(0x027F0E04,buffer,gabi::load<u32>(0x1047C9BC),0);
p=gabi::call<u32>(0x025200D4);
gabi::store<u32>(0x104B4634,gabi::load<u32>(p+0x5D78));
p=gabi::call<u32>(0x025200D4);
gabi::store<u32>(0x104B4638,gabi::load<u32>(p+0x5D7C));
}

}

VERIFY(0x025BED8C,Snap_Entry);

void Snap_RemoveGlobal() {

 WWHD_FUNC(0x025BEF30,void);

 gabi::call<void>(0x025BEE04,gabi::load<u32>(0x1047C9BC));

}

VERIFY(0x025BEF30,Snap_RemoveGlobal);

void Snap_SetSphere(u32 s,u32 v,f32 radius) {

 WWHD_FUNC(0x025BEF3C,void,s,v,radius);

 gabi::store<u8>(s+0x1B,gabi::load<u8>(s+0x1B)|1);
gabi::store<f32>(s,gabi::load<f32>(v));
gabi::store<f32>(s+4,gabi::load<f32>(v+4));
f32 z=gabi::load<f32>(v+8);
gabi::store<f32>(s+0x10,0);
gabi::store<f32>(s+8,z);
gabi::store<u16>(s+0x1E,0);
gabi::store<f32>(s+0xC,radius);

}

VERIFY(0x025BEF3C,Snap_SetSphere);

void Snap_MtxCopy(u32 dst,u32 src) {

 WWHD_FUNC(0x025BEF7C,void,dst,src);

 f32 a[12];
for(u32 i=0;i<12;++i)a[i]=gabi::load<f32>(src+4*i);
for(u32 i=0;i<12;++i)gabi::store<f32>(dst+4*i,a[i]);

}

VERIFY(0x025BEF7C,Snap_MtxCopy);

u32 Snap_IsType(u32 s,u32 type) {

 WWHD_FUNC(0x025BF1C8,u32,s,type);

 return u32(gabi::load<u8>(s+0x18))==type;

}

VERIFY(0x025BF1C8,Snap_IsType);

u32 Snap_Threshold(u32 s,s32 area,f32 ratio) {

 WWHD_FUNC(0x025BF1DC,u32,s,area,ratio);

 s32 threshold=gabi::ftoi(f32(area)*gabi::load<f32>(0x1047C9C8));
if(gabi::load<s32>(s+0x20)<=threshold)return 0;
return gabi::load<f32>(s+0x24)>ratio;

}

VERIFY(0x025BF1DC,Snap_Threshold);

s32 Snap_FindType(u32 s,s32 start,u32 type) {

 WWHD_FUNC(0x025C019C,s32,s,start,type);

 if(start<0)return -1;
s32 count=gabi::load<s32>(s+0x9C);
if(start>=count)return -1;
u32 ptr=s+0xA0+u32(start)*60;
for(s32 i=start;i<count;++i,ptr+=60)if(gabi::call<u32>(0x025BF1C8,ptr,type))return i;
return -1;

}

VERIFY(0x025C019C,Snap_FindType);

u32 Snap_JudgePost(u32 s) {

 WWHD_FUNC(0x025C020C,u32,s);

 s32 index=gabi::call<s32>(0x025C019C,s,0,1);
if(index==-1)return 0;
if(gabi::call<u32>(0x025BF1DC,s+0xA0+u32(index)*60,2000,0.55f)){
gabi::store<u8>(s+0xF6C,0);
return 1;
}
return 0;

}

VERIFY(0x025C020C,Snap_JudgePost);

u32 Snap_JudgeScared(u32 s) {

 WWHD_FUNC(0x025C0284,u32,s);

 s32 index=gabi::call<s32>(0x025C019C,s,0,2);
if(index==-1)return 0;
if(gabi::call<u32>(0x025BF1DC,s+0xA0+u32(index)*60,5000,0.55f))return 2;
gabi::store<u8>(s+0xF6C,0);
return 0;

}

VERIFY(0x025C0284,Snap_JudgeScared);

u32 Snap_JudgeType4(u32 s) {

 WWHD_FUNC(0x025C03D0,u32,s);

 s32 index=gabi::call<s32>(0x025C019C,s,0,4);
if(index==-1)return 0;
if(gabi::call<u32>(0x025BF1DC,s+0xA0+u32(index)*60,5000,0.55f)){
gabi::store<u8>(s+0xF6C,0);
return 4;
}
return 0;

}

VERIFY(0x025C03D0,Snap_JudgeType4);

u32 Snap_JudgeCouple(u32 s) {

 WWHD_FUNC(0x025C030C,u32,s);

 s32 a=gabi::call<s32>(0x025C019C,s,0,3);
if(a==-1)return 0;
s32 b=gabi::call<s32>(0x025C019C,s,u32(a)+1,3);
if(b==-1)return 0;
u32 base=s+0xA0;
if(!gabi::call<u32>(0x025BF1DC,base+u32(a)*60,2000,0.55f))return 0;
if(!gabi::call<u32>(0x025BF1DC,base+u32(b)*60,2000,0.55f))return 0;
gabi::store<u8>(s+0xF6C,0);
return 3;

}

VERIFY(0x025C030C,Snap_JudgeCouple);

u32 Snap_JudgeType5(u32 s) {

 WWHD_FUNC(0x025C0448,u32,s);

 s32 a=gabi::call<s32>(0x025C019C,s,0,5);
if(a==-1)return 0;
s32 b=gabi::call<s32>(0x025C019C,s,0,109);
if(b==-1)return 0;
u32 base=s+0xA0;
if(!gabi::call<u32>(0x025BF1DC,base+u32(a)*60,1500,0.55f))return 0;
if(!gabi::call<u32>(0x025BF1DC,base+u32(b)*60,1500,0.55f))return 0;
gabi::store<u8>(s+0xF6C,0);
return 5;

}

VERIFY(0x025C0448,Snap_JudgeType5);

u32 Snap_JudgeType6(u32 s) {

 WWHD_FUNC(0x025C0500,u32,s);

 s32 a=gabi::call<s32>(0x025C019C,s,0,6);
if(a==-1)return 0;
if(gabi::call<s32>(0x025C019C,s,0,5)!=-1)return 0;
if(!gabi::call<u32>(0x025BF1DC,s+0xA0+u32(a)*60,3000,0.55f))return 0;
gabi::store<u8>(s+0xF6C,0);
return 6;

}

VERIFY(0x025C0500,Snap_JudgeType6);

u32 Snap_JudgeSelected(u32 s) {

 WWHD_FUNC(0x025C0594,u32,s);

 s32 a=gabi::call<s32>(0x025C019C,s,0,gabi::load<u32>(s+0x98));
if(a==-1)return 0;
if(!gabi::call<u32>(0x025BF1DC,s+0xA0+u32(a)*60,3000,0.55f))return 0;
gabi::store<u8>(s+0xF6C,0);
return gabi::load<u32>(s+0x98);

}

VERIFY(0x025C0594,Snap_JudgeSelected);

void Snap_PacketDtor(u32 s,u32 flags) {

 WWHD_FUNC(0x025C0734,void,s,flags);

 if(s){
gabi::call<void>(0x027F13DC,s,0);
if(flags&1)gabi::call<void>(0x0273AF40,s);
}

}

VERIFY(0x025C0734,Snap_PacketDtor);

u32 Snap_EmptyResult(u32 s) {

 WWHD_FUNC(0x025C0788,u32,s);

 return 0;

}

VERIFY(0x025C0788,Snap_EmptyResult);

void Snap_ObjDtor(u32 s,u32 flags) {

 WWHD_FUNC(0x025C0790,void,s,flags);

 if(s&&(flags&1))gabi::call<void>(0x0273AF40,s);

}

VERIFY(0x025C0790,Snap_ObjDtor);

void Snap_TextureCreate(u32 s,u32 src,u32 mode,u32 alloc) {

 WWHD_FUNC(0x025C07A4,void,s,src,mode,alloc);

 for(u32 i=0;i<30;++i)gabi::store<u32>(s+4*i,gabi::load<u32>(src+4*i));
for(u32 i=0x78;i<0x7C;++i)gabi::store<u8>(s+i,gabi::load<u8>(src+i));
for(u32 i=0x7C;i<0x88;i+=4)gabi::store<u32>(s+i,gabi::load<u32>(src+i));
u32 word=gabi::load<u32>(src+0x88);
gabi::store<u32>(s+0x98,alloc);
gabi::store<u32>(s+0x88,word);
gabi::store<u32>(s+0x94,mode);
gabi::call<void>(0x027B6C60,s);
gabi::store<u8>(s+0x90,1);

}

VERIFY(0x025C07A4,Snap_TextureCreate);

void Snap_Texture2Create(u32 s,u32 src,u32 mode,u32 alloc) {

 WWHD_FUNC(0x025C0934,void,s,src,mode,alloc);

 for(u32 i=0;i<30;++i)gabi::store<u32>(s+4*i,gabi::load<u32>(src+4*i));
for(u32 i=0x78;i<0x7C;++i)gabi::store<u8>(s+i,gabi::load<u8>(src+i));
for(u32 i=0x7C;i<0x88;i+=4)gabi::store<u32>(s+i,gabi::load<u32>(src+i));
u32 word=gabi::load<u32>(src+0x88);
gabi::store<u32>(s+0x98,alloc);
gabi::store<u32>(s+0x88,word);
gabi::store<u32>(s+0x94,mode);
gabi::call<void>(0x027B6E2C,s);
gabi::store<u8>(s+0x90,1);

}

VERIFY(0x025C0934,Snap_Texture2Create);

void Snap_TextureUpdate(u32 s,u32 src,u32 mode,u32 alloc) {

 WWHD_FUNC(0x025C0848,void,s,src,mode,alloc);

 const u32 keys[]={
4,8,12,16,20,24,56,52,28}
;
for(u32 off:keys)if(gabi::load<u32>(s+off)!=gabi::load<u32>(src+off)){
gabi::call<void>(0x025C07A4,s,src,mode,alloc);
return;
}
u32 image=gabi::load<u32>(src+0x30),mip=gabi::load<u32>(src+0x28);
gabi::store<u32>(s+0xd8,image);
gabi::store<u32>(s+0xd0,mip);
u32 oldAlloc=gabi::load<u32>(s+0x98);
gabi::store<u32>(s+0x28,mip);
u32 oldMode=gabi::load<u32>(s+0x94);
gabi::store<u32>(s+0x30,image);
if(oldAlloc!=alloc){
gabi::store<u32>(s+0x98,alloc);
gabi::store<u8>(s+0x90,1);
}
if(oldMode!=mode){
gabi::store<u32>(s+0x94,mode);
gabi::store<u8>(s+0x90,1);
}

}

VERIFY(0x025C0848,Snap_TextureUpdate);

void Snap_Texture2Update(u32 s,u32 src,u32 mode,u32 alloc) {

 WWHD_FUNC(0x025C09D8,void,s,src,mode,alloc);

 const u32 keys[]={
4,8,12,16,20,24,56,52,28}
;
for(u32 off:keys)if(gabi::load<u32>(s+off)!=gabi::load<u32>(src+off)){
gabi::call<void>(0x025C0934,s,src,mode,alloc);
return;
}
u32 image=gabi::load<u32>(src+0x30),mip=gabi::load<u32>(src+0x28);
gabi::store<u32>(s+0xd4,image);
gabi::store<u32>(s+0xcc,mip);
u32 oldAlloc=gabi::load<u32>(s+0x98);
gabi::store<u32>(s+0x28,mip);
u32 oldMode=gabi::load<u32>(s+0x94);
gabi::store<u32>(s+0x30,image);
if(oldAlloc!=alloc){
gabi::store<u32>(s+0x98,alloc);
gabi::store<u8>(s+0x90,1);
}
if(oldMode!=mode){
gabi::store<u32>(s+0x94,mode);
gabi::store<u8>(s+0x90,1);
}

}

VERIFY(0x025C09D8,Snap_Texture2Update);

void Snap_TextureAssign(u32 s,u32 src) {

 WWHD_FUNC(0x025C0928,void,s,src);

 u32 alloc=gabi::load<u32>(s+0x98),mode=gabi::load<u32>(s+0x94);
gabi::call<void>(0x025C0848,s,src,mode,alloc);

}

VERIFY(0x025C0928,Snap_TextureAssign);

void Snap_Texture2Assign(u32 s,u32 src) {

 WWHD_FUNC(0x025C0AB8,void,s,src);

 u32 alloc=gabi::load<u32>(s+0x98),mode=gabi::load<u32>(s+0x94);
gabi::call<void>(0x025C09D8,s,src,mode,alloc);

}

VERIFY(0x025C0AB8,Snap_Texture2Assign);

// Pure float copies preserve the reference store bits; arithmetic loads use gabi::load<f32>.
static f32 snap_raw_float(u32 p){u32 bits=gabi::load<u32>(p);f32 value;memcpy(&value,&bits,4);return value;}
void Snap_ScaleTransMtx(u32 dst,u32 scale,u32 pos) {

 WWHD_FUNC(0x025C0AC4,void,dst,scale,pos);

 f32 x=snap_raw_float(scale);
gabi::store<f32>(dst,x);
gabi::store<f32>(dst+0x10,0);
gabi::store<f32>(dst+0x20,0);
gabi::store<f32>(dst+4,0);
f32 y=snap_raw_float(scale+4);
gabi::store<f32>(dst+0x24,0);
gabi::store<f32>(dst+0x14,y);
gabi::store<f32>(dst+8,0);
gabi::store<f32>(dst+0x18,0);
gabi::store<f32>(dst+0x28,snap_raw_float(scale+8));
gabi::store<f32>(dst+0xC,snap_raw_float(pos));
gabi::store<f32>(dst+0x1C,snap_raw_float(pos+4));
gabi::store<f32>(dst+0x2C,snap_raw_float(pos+8));

}

VERIFY(0x025C0AC4,Snap_ScaleTransMtx);

u32 Snap_PacketCtor(u32 s) {

 WWHD_FUNC(0x025BD814,u32,s);

 if(!s)s=gabi::call<u32>(0x0273AD10,0xF94);
if(s){
gabi::call<void>(0x027F1278,s);
gabi::store<u32>(s+0x98,0);
gabi::store<u32>(s+0xC,0x100552B0);
gabi::store<u32>(s+0x9C,0);
gabi::call<void>(0x028EFFD0,s+0xA0,63,60,0x025BD7B4);
const u32 offs[]={
0xF74,0xF88,0xF84,0xF78,0xF80}
;
for(u32 o:offs)gabi::store<u32>(s+o,0);
gabi::store<u8>(s+0xF6C,0);
gabi::store<u32>(s+0xF68,0);
gabi::store<u32>(s+0xF7C,0);
gabi::store<u32>(s+0xF64,0);
gabi::store<u32>(s+0xF70,0);
if(s+0xF8C==0)gabi::call<u32>(0x0273AD10,8);
gabi::store<u32>(s+0xF8C,0);
gabi::store<u32>(s+0xF90,0);
}
return s;

}

VERIFY(0x025BD814,Snap_PacketCtor);

u32 Snap_JudgeFigure(u32 s,u32 index) {

 WWHD_FUNC(0x025BE31C,u32,s,index);

 s32 photo=gabi::load<s32>(s+0x98);
if(photo>=208||u32(photo)-73>=134)return 0;
u32 table=0x10054708+u32(gabi::call<s32>(0x025BD600,photo))*18;
u32 pixels=gabi::load<u16>(table+14);
s32 threshold=gabi::ftoi(f32(pixels)*gabi::load<f32>(0x1047C9C8));
f32 ratio=f32(gabi::load<u16>(table+10))/1000.0f;
u32 elem=s+0xA0+index*60;
if(gabi::load<s32>(elem+0x20)<threshold)return 0;
f32 actual=gabi::load<f32>(elem+0x24);
u8 flags=gabi::load<u8>(s+0xF6C);
if(actual<ratio)flags|=2;
else flags&=0xFD;
gabi::store<u8>(s+0xF6C,flags);
u32 culled=gabi::call<u32>(0x025BE268,elem);
flags=gabi::load<u8>(s+0xF6C);
u32 result=gabi::load<u32>(s+0x98);
if(culled)flags|=1;
else flags&=0xFE;
gabi::store<u8>(s+0xF6C,flags);
return result;

}

VERIFY(0x025BE31C,Snap_JudgeFigure);

void Snap_RegistFig(u32 type,u32 actor,u32 pos,s16 angle,f32 rScale,f32 hScale,f32 pScale) {

 WWHD_FUNC(0x025BEBB8,void,type,actor,pos,angle,rScale,hScale,pScale);

 if(!gabi::call<u32>(0x025BDC54,gabi::load<u32>(0x1047C9BC))||type>=207)return;
u32 index=gabi::call<u32>(0x025BD600,type);
u32 table=0x10054708+index*18;
gabi::call<void>(0x025F1884,0x1048D0CC,angle);
gabi::Local<u8[12]> offset,world;
f32 x=f32(gabi::load<s16>(table))*pScale,y=f32(gabi::load<s16>(table+2))*pScale,z=f32(gabi::load<s16>(table+4))*pScale;
gabi::store<f32>(offset.a,x);
gabi::store<f32>(offset.a+4,y);
gabi::store<f32>(offset.a+8,z);
gabi::call<void>(0x028E8F64,0x1048D0CC,offset.a,world.a);
gabi::call<void>(0x028E8D88,world.a,pos,world.a);
gabi::Local<u8[52]> obj;
gabi::call<void>(0x025BD71C,obj.a);
f32 radius=f32(gabi::load<s16>(table+6))*hScale,height=f32(gabi::load<s16>(table+8))*rScale;
gabi::call<void>(0x025BEB5C,obj.a,world.a,angle,radius,height);
gabi::call<void>(0x025BEB90,obj.a,type,actor,0,4,gabi::load<s16>(table+12));
gabi::call<void>(0x025BEB4C,obj.a);

}

VERIFY(0x025BEBB8,Snap_RegistFig);

void Snap_Remove(u32 s) {

 WWHD_FUNC(0x025BEE04,void,s);

 for(u32 off: {
0xF70u,0xF74u}
){
u32 p=gabi::load<u32>(s+off);
if(p){
gabi::call<void>(0x0273AF40,p);
gabi::store<u32>(s+off,0);
}
}
const u32 offsets[]={
0xF78,0xF80,0xF84,0xF88}
;
const u32 vts[]={
0x18,0x30,0x90,0x18}
;
for(u32 i=0;i<4;++i){
u32 p=gabi::load<u32>(s+offsets[i]);
if(p){
u32 vt=gabi::load<u32>(p+vts[i]);
u32 target=gabi::load<u32>(vt+0x1C);
gabi::call_ptr<void>(target,p,3);
gabi::store<u32>(s+offsets[i],0);
}
}
gabi::store<u32>(s+0xF8C,0);
gabi::store<u32>(s+0xF90,0);
gabi::call<void>(0x027F88C8,gabi::load<u32>(0x101F9968),9);
gabi::call<void>(0x027F88C8,gabi::load<u32>(0x101F9968),10);
gabi::call<void>(0x027F88C8,gabi::load<u32>(0x101F9968),11);

}

VERIFY(0x025BEE04,Snap_Remove);

void Snap_ObjDraw(u32 s,s32 alpha) {

 WWHD_FUNC(0x025BF01C,void,s,alpha);

 gabi::Local<u8[16]> color;
gabi::store<f32>(color.a,1);
gabi::store<f32>(color.a+4,0);
gabi::store<f32>(color.a+8,0);
gabi::store<f32>(color.a+12,f32(alpha)/255.0f);
f32 diameter=gabi::load<f32>(s+0xC)*2;
bool sphere=(gabi::load<u8>(s+0x1B)&1)!=0;
f32 x=gabi::load<f32>(s),y=gabi::load<f32>(s+4),z=gabi::load<f32>(s+8);
if(!sphere){
f32 h=gabi::load<f32>(s+0x10);
gabi::call<void>(0x028E93CC,0x1048D0CC,x,gabi::fmadds(h,0.5f,y),z);
gabi::call<void>(0x025F2518,diameter,h,diameter);
gabi::call<void>(0x025F1C28,0x1048D0CC,gabi::load<s16>(s+0x1E));
}
else{
gabi::call<void>(0x028E93CC,0x1048D0CC,x,y,z);
gabi::call<void>(0x025F2518,diameter,diameter,diameter);
}
u32 drawer=gabi::load<u32>(0x101F8B28);
gabi::Local<u8[48]> mtx;
gabi::call<void>(0x025BEF7C,mtx.a,0x1048D0CC);
gabi::call<void>(0x027509C8,drawer,mtx.a);
gabi::call<void>(sphere?0x02750A00:0x02750A24,drawer,color.a,color.a);

}

VERIFY(0x025BF01C,Snap_ObjDraw);

void Snap_StaticInit() {

 WWHD_FUNC(0x025C060C,void);

 gabi::store<u32>(0x1047C9DC,0);
gabi::store<u32>(0x1047C9D8,0);
gabi::store<u32>(0x1047C9D4,0);
gabi::store<u32>(0x1047C9D0,0);
gabi::call<void>(0x028F026C,0x101EBA34);
gabi::store<f32>(0x1047C9C0,-3.1415927410125732f);
gabi::store<f32>(0x1047C9C4,3.1415927410125732f);
gabi::call<void>(0x028ED6F8,0x1047C9CC);
gabi::call<void>(0x028F026C,0x101EBA40);
gabi::call<void>(0x028EAB2C,0x1047C9CD);
gabi::call<void>(0x028F026C,0x101EBA4C);
gabi::store<f32>(0x1047C9A4,1600);
gabi::store<f32>(0x1047C9AC,560);
gabi::store<f32>(0x1047C9B0,315);
gabi::store<f32>(0x1047C99C,800);
gabi::store<f32>(0x1047C9B4,360);
gabi::store<f32>(0x1047C9B8,203);
gabi::store<f32>(0x1047C9A0,450);
gabi::store<f32>(0x1047C9A8,900);
gabi::store<f32>(0x1047C9C8,(800.0f/294.0f)*(450.0f/198.0f));

}

VERIFY(0x025C060C,Snap_StaticInit);

u32 Snap_Regist(u32 s,u32 obj) {

 WWHD_FUNC(0x025BE99C,u32,s,obj);

 if(!gabi::call<u32>(0x025BDC54,s)||gabi::call<u32>(0x025BDC6C,s))return 0;
u32 play=gabi::call<u32>(0x025200D4);
s32 count=gabi::load<s32>(s+0x9C);
u32 actor=gabi::load<u32>(play+0x5B2C),base=s+0xA0;
if(count>=63){
f32 far=-1000000000.0f;
u32 best=0;
bool found=false;
for(u32 i=0;i<63;++i){
u32 elem=base+60*i;
f32 distance=gabi::load<f32>(elem+0x34);
if(far<distance&&gabi::load<u8>(elem+0x18)>=72){
far=distance;
best=i;
found=true;
}
}
if(!found)return 0;
f32 distance=gabi::call<f32>(0x028E8DE8,actor+0x314,obj);
if(gabi::load<u8>(obj+0x18)>=72&&distance>far)return 0;
u32 elem=gabi::call<u32>(0x025BE908,base+best*60,obj);
gabi::store<f32>(elem+0x34,distance);
return 0;
}
gabi::call<void>(0x025BE908,base+u32(count)*60,obj);
count=gabi::load<s32>(s+0x9C);
f32 distance=gabi::call<f32>(0x028E8DE8,actor+0x314,base+u32(count)*60);
count=gabi::load<s32>(s+0x9C);
gabi::store<f32>(base+u32(count)*60+0x34,distance);
gabi::store<u32>(s+0x9C,gabi::load<u32>(s+0x9C)+1);
return 1;

}

VERIFY(0x025BE99C,Snap_Regist);

void Snap_SetResult(u32 s) {

 WWHD_FUNC(0x025BE480,void,s);

 gabi::store<u8>(s+0xF6C,0);
gabi::store<u32>(s+0xF68,0);
gabi::Local<u8[11]> seen;
for(u32 k=0;k<11;++k)gabi::store<u8>(seen.a+k,0);

 for(s32 i=0;i<gabi::load<s32>(s+0x9C);++i){
u32 elem=s+0xA0+u32(i)*60;
if(!gabi::load<u32>(elem+0x20))continue;
u32 type=gabi::load<u8>(elem+0x18);
if(!type)continue;
if(type>=208)gabi::call<void>(0x0273AA24,0x10055250,0xBD4,0x1005521C);
if(type>=11)continue;
if(gabi::load<u8>(seen.a+type))continue;
u32 desc=0x101EBA58+type*8;
s16 vindex=gabi::load<s16>(desc+2);
if(vindex){
gabi::store<u32>(s+0x98,type);
vindex=gabi::load<s16>(desc+2);
u32 self=s+s32(gabi::load<s16>(desc));
u32 target;
if(vindex<0)target=gabi::load<u32>(desc+4);
else{
u32 vt=gabi::load<u32>(self+s32(gabi::load<s16>(desc+6)));
target=gabi::load<u32>(vt+u32(s32(vindex))*8+4);
}
u32 result=gabi::call_ptr<u32>(target,self);
gabi::store<u32>(s+0xF68,result);
if(result)return;
}
gabi::store<u8>(seen.a+type,1);
}

 for(s32 i=0;i<gabi::load<s32>(s+0x9C);++i){
u32 elem=s+0xA0+u32(i)*60;
s32 pixels=gabi::load<s32>(elem+0x20);
if(!pixels)continue;
u32 type=gabi::load<u8>(elem+0x18);
if(!type||type<11)continue;
if(type>=208)gabi::call<void>(0x0273AA24,0x10055250,0xC03,0x1005521C);
pixels=gabi::load<s32>(elem+0x20);
if(pixels<0)continue;
gabi::store<u32>(s+0x98,type);
u32 result=gabi::call<u32>(0x025BE31C,s,i);
gabi::store<u32>(s+0xF68,result);
if(result&&!gabi::load<u8>(s+0xF6C))return;
}


}

VERIFY(0x025BE480,Snap_SetResult);

void Snap_Judge(u32 s) {

 WWHD_FUNC(0x025BE664,void,s);

 gabi::store<u32>(s+0xF68,0);
for(u32 i=0;i<63;++i){
u32 elem=s+0xA0+i*60;
gabi::store<u32>(elem+0x20,0);
gabi::call<void>(0x025BE208,elem);
}
u32 tex=gabi::call<u32>(0x027F81A4,gabi::load<u32>(0x101F9968),9);
gabi::Local<u8[0x158]> reader;
gabi::call<void>(0x027BF144,reader.a);
gabi::call<void>(0x027BF3FC,reader.a,tex,gabi::load<u32>(s+0xF7C));
gabi::call<void>(0x027BF598,reader.a,tex);
gabi::call<void>(0x027BF730,reader.a);
gabi::Local<u8[16]> sample;

 u32 y=0;
for(;;){
s32 height=gabi::load<s32>(tex+0xC);
u32 small=gabi::load<u8>(tex+0x79);
if(s32(small)>height)height=s32(small);
if(y>=u32(height))break;
u32 x=0;
for(;;){
s32 width=gabi::load<s32>(tex+8);
small=gabi::load<u8>(tex+0x78);
if(s32(small)>width)width=s32(small);
if(x>=u32(width))break;
gabi::call<void>(0x027BF244,reader.a,sample.a,x,y);
u32 index=gabi::load<u32>(sample.a+12)>>2;
if(index<63){
u32 elem=s+0xA0+index*60;
gabi::store<u32>(elem+0x20,gabi::load<u32>(elem+0x20)+1);
gabi::call<void>(0x025BE224,elem,s16(x),s16(y));
}
++x;
}
++y;
}

 gabi::call<void>(0x027BF1A4,reader.a);
for(s32 i=0;i<gabi::load<s32>(s+0x9C);++i){
u32 elem=s+0xA0+u32(i)*60;
if(!gabi::load<u32>(elem+0x20))continue;
f32 area=gabi::call<f32>(0x025BDC78,elem);
if(!(std::fabs(area)<3.814697265625e-06f))gabi::store<f32>(elem+0x24,f32(gabi::load<s32>(elem+0x20))/area);
}
gabi::call<void>(0x025BE480,s);
gabi::call<void>(0x027BF1E8,reader.a,2);


}

VERIFY(0x025BE664,Snap_Judge);

f32 Snap_CalcArea(u32 s) {

 WWHD_FUNC(0x025BDC78,f32,s);

 f32 total=0;

 f32 x=gabi::load<f32>(s),y=gabi::load<f32>(s+4),z=gabi::load<f32>(s+8);

 bool sphere=(gabi::load<u8>(s+0x1B)&1)!=0;

 f32 radius=gabi::load<f32>(s+0xC),height=gabi::load<f32>(s+0x10);

 gabi::call<void>(0x028E93CC,0x1048D0CC,x,y,z);

 gabi::Local<u8[12]> input,world,p0,p1,p2,p3,base,top,c0,c1,c2,c3;

 auto vset=[&](u32 a,f32 vx,f32 vy,f32 vz){
gabi::store<f32>(a,vx);
gabi::store<f32>(a+4,vy);
gabi::store<f32>(a+8,vz);
}
;

 auto project=[&](u32 dst){
gabi::call<void>(0x028E8F64,0x1048D0CC,input.a,world.a);
gabi::call<void>(0x025BD670,world.a,dst);
}
;

 auto tri=[&](u32 a,u32 b,u32 c){
f32 a0=gabi::load<f32>(a),a1=gabi::load<f32>(a+4),b0=gabi::load<f32>(b),b1=gabi::load<f32>(b+4),c0=gabi::load<f32>(c),c1=gabi::load<f32>(c+4);
f32 product=gabi::call<f32>(0x02010CFC,a0,a1,b0,b1,c0,c1);
f32 area=0.5f*product;
if(area>0)total+=area;
}
;

 if(!sphere) {

  gabi::call<void>(0x025F2518,radius,height,radius);

  gabi::call<void>(0x025F1C28,0x1048D0CC,gabi::load<s16>(s+0x1E));

  vset(base.a,0,0,0);
gabi::call<void>(0x028E8F64,0x1048D0CC,base.a,base.a);

  vset(top.a,0,1,0);
gabi::call<void>(0x028E8F64,0x1048D0CC,top.a,top.a);

  for(u32 i=0;i<gabi::load<u8>(s+0x19);++i) {

   u32 curr=0x100546CC+i*12,next=curr+12;

   f32 cx=gabi::load<f32>(curr),cz=gabi::load<f32>(curr+8);

   vset(input.a,cx,0,cz);
project(p0.a);
gabi::store<f32>(input.a+4,1);
project(p1.a);

   f32 nx=gabi::load<f32>(next),nz=gabi::load<f32>(next+8);

   gabi::store<f32>(input.a,nx);
gabi::store<f32>(input.a+8,nz);
project(p2.a);
gabi::store<f32>(input.a+4,0);
project(p3.a);

   tri(p0.a,p1.a,p2.a);
tri(p0.a,p2.a,p3.a);

   gabi::call<void>(0x025BD670,top.a,c0.a);
tri(c0.a,p2.a,p1.a);

   gabi::call<void>(0x025BD670,base.a,c1.a);
tri(c1.a,p0.a,p3.a);

   vset(input.a,-cx,1,cz);
project(p0.a);
gabi::store<f32>(input.a+4,0);
project(p1.a);

   gabi::store<f32>(input.a,-nx);
gabi::store<f32>(input.a+8,nz);
project(p2.a);
gabi::store<f32>(input.a+4,1);
project(p3.a);

   tri(p0.a,p1.a,p2.a);
tri(p0.a,p2.a,p3.a);

   gabi::call<void>(0x025BD670,top.a,c2.a);
tri(c2.a,p0.a,p3.a);

   gabi::call<void>(0x025BD670,base.a,c3.a);
tri(c3.a,p2.a,p1.a);

  }

 }
 else {

  gabi::call<void>(0x025F2518,radius,radius,radius);

  for(u32 i=0;i<20;++i) {

   u32 outputs[]={
p0.a,p1.a,p2.a}
;

   for(u32 j=0;j<3;++j){
u32 index=gabi::load<u32>(0x10055104+i*12+j*4);
u32 vert=0x10055074+index*12;
vset(input.a,gabi::load<f32>(vert),gabi::load<f32>(vert+4),gabi::load<f32>(vert+8));
project(outputs[j]);
}

   tri(p0.a,p1.a,p2.a);

  }

 }

 return total;

}

VERIFY(0x025BDC78,Snap_CalcArea);

void Snap_Create(u32 s,u32 heap) {

 WWHD_FUNC(0x025BD8E4,void,s,heap);

 gabi::store<u32>(s+0xF7C,heap);
gabi::store<u32>(s+0xF68,0);
gabi::store<u8>(s+0xF6C,0);
gabi::store<u32>(s+0xF64,0);

 u32 p=gabi::call<u32>(0x0273B050,0x148,heap,4);
if(p)p=gabi::call<u32>(0x027B6CB8,p);

 heap=gabi::load<u32>(s+0xF7C);
gabi::store<u32>(s+0xF70,p);

 p=gabi::call<u32>(0x0273B050,0x154,heap,4);
if(p)p=gabi::call<u32>(0x027B6E64,p);

 heap=gabi::load<u32>(s+0xF7C);
gabi::store<u32>(s+0xF74,p);

 p=gabi::call<u32>(0x0273B050,0x40,heap,4);
if(p)p=gabi::call<u32>(0x027B95B4,p);

 heap=gabi::load<u32>(s+0xF7C);
gabi::store<u32>(s+0xF78,p);

 p=gabi::call<u32>(0x0273B050,0x58,heap,4);

 if(p) {

  gabi::store<u32>(p+0x30,0x101450D0);
for(u32 i=0;i<12;++i)gabi::store<u32>(p+i*4,gabi::load<u32>(0x104A041C+i*4));

  gabi::store<u32>(p+0x30,0x101450F8);

  u32 v=p+0x34;
if(!v)v=gabi::call<u32>(0x0273AD10,12);
if(v){
gabi::store<f32>(v+4,0);
gabi::store<f32>(v,0);
gabi::store<f32>(v+8,10);
}

  v=p+0x40;
if(!v)v=gabi::call<u32>(0x0273AD10,12);
if(v){
gabi::store<f32>(v+4,0);
gabi::store<f32>(v,0);
gabi::store<f32>(v+8,0);
}

  v=p+0x4C;
if(!v)v=gabi::call<u32>(0x0273AD10,12);
if(v){
gabi::store<f32>(v+4,1);
gabi::store<f32>(v,0);
gabi::store<f32>(v+8,0);
}

 }

 heap=gabi::load<u32>(s+0xF7C);
gabi::store<u32>(s+0xF80,p);

 p=gabi::call<u32>(0x0273B050,0xD4,heap,4);
if(p)p=gabi::call<u32>(0x0274E87C,p);

 heap=gabi::load<u32>(s+0xF7C);
gabi::store<u32>(s+0xF84,p);

 p=gabi::call<u32>(0x0273B050,0x1C,heap,4);

 if(p){
gabi::store<u32>(p+0x18,0x1005466C);
gabi::store<f32>(p+4,1);
gabi::store<f32>(p,1);
u32 v=p+8;
if(!v)v=gabi::call<u32>(0x0273AD10,16);
if(v){
gabi::store<f32>(v+8,1);
gabi::store<f32>(v+4,0);
gabi::store<f32>(v,0);
gabi::store<f32>(v+12,1);
}
}

 gabi::store<u32>(s+0xF88,p);

 u32 service=gabi::load<u32>(0x101F86E8),graphics=gabi::load<u32>(0x101F8664);
u32 first=service+0x190;

 gabi::store<u32>(s+0xF8C,first);
u32 vt=gabi::load<u32>(service+0x1F4);
u32 desc=gabi::load<u32>(graphics+0x28);
u32 target=gabi::load<u32>(vt+0x54);

 gabi::Local<u32> token,arg1,arg2;
u32 ref=gabi::call<u32>(0x027A7558,token.a,desc);
gabi::store<u32>(arg1.a,gabi::load<u32>(ref));
gabi::call_ptr<void>(target,first+4,arg1.a);

 u32 second=service+0xC00;
gabi::store<u32>(s+0xF90,second);
desc=gabi::load<u32>(graphics+0x24);
vt=gabi::load<u32>(service+0xC64);
target=gabi::load<u32>(vt+0x54);

 ref=gabi::call<u32>(0x027A7558,token.a,desc);
gabi::store<u32>(arg2.a,gabi::load<u32>(ref));
gabi::call_ptr<void>(target,second+4,arg2.a);

}

VERIFY(0x025BD8E4,Snap_Create);

static void snap_texture_copy(u32 dst,u32 src) {

 for(u32 i=0;i<0x78;i+=4)gabi::store<u32>(dst+i,gabi::load<u32>(src+i));

 for(u32 i=0x78;i<0x7C;++i)gabi::store<u8>(dst+i,gabi::load<u8>(src+i));

 for(u32 i=0x7C;i<=0x88;i+=4)gabi::store<u32>(dst+i,gabi::load<u32>(src+i));

 gabi::store<u8>(dst+0x90,gabi::load<u8>(src+0x90));
gabi::store<u8>(dst+0x91,gabi::load<u8>(src+0x91));

 for(u32 i=0x94;i<=0x148;i+=4)gabi::store<u32>(dst+i,gabi::load<u32>(src+i));

 gabi::store<u8>(dst+0x14C,gabi::load<u8>(src+0x14C));

 for(u32 i=0x150;i<=0x18C;i+=4)gabi::store<u32>(dst+i,gabi::load<u32>(src+i));

}

void Snap_DrawPacket(u32 s,u32 context) {

 WWHD_FUNC(0x025BF250,void,s,context);

 u32 manager=gabi::load<u32>(0x101F9968);

 u32 large=gabi::call<u32>(0x027F81A4,manager,11),depth=gabi::call<u32>(0x027F81A4,manager,10),small=gabi::call<u32>(0x027F81A4,manager,9);

 if(!large){
large=gabi::call<u32>(0x027F81C0,manager,11,0x1047C9A4);
depth=gabi::call<u32>(0x027F81C0,manager,10,0x1047C99C);
small=gabi::call<u32>(0x027F81C0,manager,9,0x1047C99C);
}

 u32 frame=gabi::load<u32>(context),source=gabi::load<u32>(frame+8);

 if(!gabi::load<u32>(0x101FD7BC)){
gabi::store<u32>(0x101FD7BC,1);
gabi::store<u32>(0x101FDCF8,0x1005469C);
}

 if(source){
u32 vt=gabi::load<u32>(source+0x18),target=gabi::load<u32>(vt+12);
if(!gabi::call_ptr<u32>(target,source,0x101FDCF8))source=0;
}

 gabi::call<void>(0x027B6D64,gabi::load<u32>(source+0x1C));
gabi::call<void>(0x027BE794,gabi::load<u32>(source+0x1C));

 gabi::Local<u8[408]> firstView,secondView,texture;

 gabi::Local<u8[116]> stateA,stateB;

 gabi::Local<u8[24]> viewport;

 gabi::Local<u8[8]> scale,origin,extent,position;

 gabi::Local<u8[48]> matrix;

 gabi::Local<u8[172]> camera;

 gabi::call<void>(0x027BE114,firstView.a,gabi::load<u32>(source+0x1C));

 gabi::call<void>(0x0274FDBC,stateA.a);
gabi::store<u8>(stateA.a,0);
gabi::store<u8>(stateA.a+0x47,0);
gabi::store<u8>(stateA.a+0x45,1);
gabi::store<u8>(stateA.a+12,0);
gabi::store<u8>(stateA.a+0x46,1);
gabi::store<u8>(stateA.a+0x44,1);
gabi::store<u8>(stateA.a+1,0);
gabi::call<void>(0x0274FEB0,stateA.a);

 auto dimensions=[&](u32 tex){
s32 w=gabi::load<s32>(tex+8),wb=gabi::load<u8>(tex+0x78),h=gabi::load<s32>(tex+12),hb=gabi::load<u8>(tex+0x79);
return std::pair<f32,f32>(f32(u32(w>wb?w:wb)),f32(u32(h>hb?h:hb)));
}
;

 auto sizeViewport=[&](f32 w,f32 h,f32 ox,f32 oy){
u32 p=gabi::load<u32>(s+0xF78);
gabi::store<f32>(p,w);
gabi::store<f32>(p+4,h);
p=gabi::load<u32>(s+0xF78);
gabi::store<f32>(p+8,ox);
gabi::store<f32>(p+12,oy);
gabi::store<f32>(p+16,w);
gabi::store<f32>(p+20,h);
}
;

 auto wh=dimensions(large);
f32 w=wh.first,h=wh.second;

 gabi::call<void>(0x025C0928,gabi::load<u32>(s+0xF70),large);

 u32 p=gabi::load<u32>(s+0xF78);
gabi::store<f32>(p,w);
gabi::store<f32>(p+4,h);

 f32 ox=gabi::load<f32>(0x104A0908),oy=gabi::load<f32>(0x104A090C);

 p=gabi::load<u32>(s+0xF78);
gabi::store<f32>(p+8,ox);
gabi::store<f32>(p+12,oy);
gabi::store<f32>(p+16,w);
gabi::store<f32>(p+20,h);

 p=gabi::load<u32>(s+0xF78);
gabi::call<void>(0x027B954C,p);
gabi::store<u32>(p+0x3C,0);
p=gabi::load<u32>(s+0xF78);
gabi::store<u32>(p+0x1C,gabi::load<u32>(s+0xF70));
gabi::call<void>(0x0274D690,gabi::load<u32>(s+0xF78));

 gabi::call<void>(0x0274F5F0,viewport.a,gabi::load<u32>(s+0xF78));
gabi::call<void>(0x0274F964,viewport.a,gabi::load<u32>(s+0xF78));

 u32 initX=gabi::load<u32>(0x104A0920),initY=gabi::load<u32>(0x104A0924);
gabi::store<u32>(scale.a,initX);
gabi::store<u32>(scale.a+4,initY);

 u32 bitsX=gabi::load<u32>(source+0x10);
f32 viewportX=gabi::load<f32>(0x1047C9AC);
f32 sourceX;
memcpy(&sourceX,&bitsX,4);
u32 bitsY=gabi::load<u32>(source+0x14);(void)bitsY;
f32 fraction=sourceX/1280.0f;
f32 extentX=(viewportX+viewportX)*fraction,viewportY=gabi::load<f32>(0x1047C9B0);
f32 extentY=(viewportY+viewportY)*fraction;

 gabi::store<f32>(scale.a,extentX/w);
gabi::store<f32>(scale.a+4,extentY/h);
gabi::store<f32>(extent.a,extentX);
gabi::store<f32>(extent.a+4,extentY);

 u32 screen=gabi::call<u32>(0x02739728);
f32 screenX=gabi::load<f32>(screen),screenY=gabi::load<f32>(screen+4);

 f32 positionX=gabi::fnmsubs(gabi::load<f32>(0x1047C9AC),0.5f,gabi::load<f32>(0x1047C9B4))/screenX;

 f32 positionY=gabi::fnmsubs(gabi::load<f32>(0x1047C9B0),0.5f,gabi::load<f32>(0x1047C9B8))/screenY;

 gabi::store<f32>(position.a,positionX);
gabi::store<f32>(position.a+4,positionY);

 u32 ref=gabi::call<u32>(0x027F29D4,0x104B45C0);
u32 old=gabi::load<u32>(ref);
u32 renderer=gabi::load<u32>(0x101F8710);

 u32 handle=gabi::call<u32>(0x0272C898,renderer,firstView.a,viewport.a,scale.a,position.a,old);

 gabi::call<void>(0x027B6F18,gabi::load<u32>(source+0x3C));
u32 tex=gabi::load<u32>(source+0x3C);

 if(gabi::load<u32>(tex+0xA4)){
if(gabi::load<u8>(tex+0x90)){
gabi::call<void>(0x027B6F90,tex);
gabi::store<u8>(tex+0x90,0);
}
gabi::call<void>(0xC00061A0,tex+0xA8);
gabi::call<void>(0x0276B114,gabi::load<u32>(0x101F8BD8));
tex=gabi::load<u32>(source+0x3C);
}

 gabi::call<void>(0x027BE794,tex);

 wh=dimensions(depth);
f32 dw=wh.first,dh=wh.second;
gabi::call<void>(0x025C0AB8,gabi::load<u32>(s+0xF74),depth);

 p=gabi::load<u32>(s+0xF78);
gabi::call<void>(0x027B954C,p);
gabi::store<u32>(p+0x3C,0);
p=gabi::load<u32>(s+0xF78);
gabi::store<u32>(p+0x3C,gabi::load<u32>(s+0xF74));
sizeViewport(dw,dh,ox,oy);
gabi::call<void>(0x0274D690,gabi::load<u32>(s+0xF78));

 gabi::call<void>(0x027BE114,secondView.a,gabi::load<u32>(source+0x3C));
gabi::store<u32>(secondView.a+0x158,0);
gabi::store<u32>(secondView.a+0x154,0);
gabi::store<u32>(secondView.a+0x150,0);
gabi::store<u8>(secondView.a+0x190,gabi::load<u8>(secondView.a+0x190)|4);

 gabi::call<void>(0x0274F53C,viewport.a,gabi::load<u32>(s+0xF78));
gabi::call<void>(0x0274F964,viewport.a,gabi::load<u32>(s+0xF78));

 wh=dimensions(secondView.a);
gabi::store<f32>(scale.a,gabi::load<f32>(extent.a)/wh.first);
gabi::store<f32>(scale.a+4,gabi::load<f32>(extent.a+4)/wh.second);

 handle=gabi::call<u32>(0x0272C6C0,gabi::load<u32>(0x101F8710),secondView.a,viewport.a,scale.a,position.a,handle);

 p=gabi::load<u32>(s+0xF78);
gabi::call<void>(0x027B6F18,gabi::load<u32>(p+0x3C));
p=gabi::load<u32>(s+0xF78);
gabi::call<void>(0x027BE794,gabi::load<u32>(p+0x3C));
gabi::call<void>(0x027BE2B0,secondView.a,2);

 gabi::call<void>(0x027B6D64,gabi::load<u32>(s+0xF70));
gabi::call<void>(0x027BE794,gabi::load<u32>(s+0xF70));
gabi::call<void>(0x027BDF7C,texture.a);

 u32 save=gabi::load<u32>(0x101F84DC);
u32 effect=gabi::load<u32>(s+(gabi::load<u8>(save+0x64)==38?0xF90:0xF8C));

 auto effectReady=[&](){
if(!effect||!gabi::load<u8>(effect+0x8BC))return false;
if(gabi::load<s32>(effect+0x1D4)>0&&gabi::load<u32>(effect+0x1D0))return true;
return gabi::load<u8>(effect+0xA20)!=0;
}
;

 u8 flags;

 if(effectReady()){
handle=gabi::call<u32>(0x0279DB64,effect,gabi::load<u32>(s+0xF70),handle);
if(gabi::load<u32>(effect+0x55C)&&effect+0x564){
u32 copy=effect+0x564;
snap_texture_copy(texture.a,copy);
flags=gabi::load<u8>(copy+0x190);
}
else{
bool same=true;
for(u32 off:{
4u,8u,12u,16u,20u,24u,56u,52u,28u}
)if(gabi::load<u32>(texture.a+off)!=gabi::load<u32>(large+off)){
same=false;
break;
}
if(!same)gabi::call<void>(0x027BDEB4,texture.a,large);
else{
u32 image=gabi::load<u32>(large+0x30),mip=gabi::load<u32>(large+0x28);
gabi::store<u32>(texture.a+0x30,image);
gabi::store<u32>(texture.a+0xDC,image);
gabi::store<u32>(texture.a+0xD4,mip);
gabi::store<u32>(texture.a+0x28,mip);
}
flags=gabi::load<u8>(texture.a+0x190);
}
}

 else{
bool same=true;
for(u32 off:{
4u,8u,12u,16u,20u,24u,56u,52u,28u}
)if(gabi::load<u32>(texture.a+off)!=gabi::load<u32>(large+off)){
same=false;
break;
}
if(!same)gabi::call<void>(0x027BDEB4,texture.a,large);
else{
u32 mip=gabi::load<u32>(large+0x28),image=gabi::load<u32>(large+0x30);
gabi::store<u32>(texture.a+0x28,mip);
gabi::store<u32>(texture.a+0xD4,mip);
gabi::store<u32>(texture.a+0xDC,image);
gabi::store<u32>(texture.a+0x30,image);
}
flags=gabi::load<u8>(texture.a+0x190);
}

 gabi::store<u32>(texture.a+0x150,0);
gabi::store<u32>(texture.a+0x154,0);
gabi::store<u32>(texture.a+0x158,0);
gabi::store<u8>(texture.a+0x190,flags|4);

 wh=dimensions(small);
f32 sw=wh.first,sh=wh.second;
gabi::call<void>(0x025C0928,gabi::load<u32>(s+0xF70),small);
sizeViewport(sw,sh,ox,oy);

 p=gabi::load<u32>(s+0xF78);
gabi::call<void>(0x027B954C,p);
gabi::store<u32>(p+0x3C,0);
p=gabi::load<u32>(s+0xF78);
gabi::store<u32>(p+0x1C,gabi::load<u32>(s+0xF70));
p=gabi::load<u32>(s+0xF78);
gabi::store<u32>(p+0x3C,0);
gabi::call<void>(0x027B9D00,gabi::load<u32>(s+0xF78),255);

 gabi::call<void>(0x0274F5F0,secondView.a,gabi::load<u32>(s+0xF78));
gabi::call<void>(0x0274F964,secondView.a,gabi::load<u32>(s+0xF78));
gabi::call<void>(0x027B997C,gabi::load<u32>(s+0xF78),0,1,0x104A01CC,0,1.0f);

 gabi::call<void>(0x0274FDBC,stateB.a);
gabi::store<u8>(stateB.a+0x47,0);
gabi::store<u8>(stateB.a+12,0);
gabi::store<u8>(stateB.a+0x46,1);
gabi::store<u32>(stateB.a+4,7);
gabi::store<u8>(stateB.a+0x44,1);
gabi::store<u8>(stateB.a,0);
gabi::store<u8>(stateB.a+0x45,1);
gabi::store<u8>(stateB.a+1,0);
gabi::call<void>(0x0274FEB0,stateB.a);

 gabi::call<void>(0x025C0AC4,matrix.a,0x104A0958,0x104A0928);
gabi::call<void>(0x0274E1C8,camera.a);
u32 projection=gabi::call<u32>(0x0274D83C,camera.a);

 handle=gabi::call<u32>(0x027AAD8C,texture.a,matrix.a,projection,0x104A01EC,handle);

 p=gabi::load<u32>(s+0xF78);
gabi::call<void>(0x027B6D64,gabi::load<u32>(p+0x1C));
p=gabi::load<u32>(s+0xF78);
gabi::call<void>(0x027BE794,gabi::load<u32>(p+0x1C));
gabi::call<void>(0x0274D690,gabi::load<u32>(s+0xF78));
gabi::call<void>(0x0274E474,camera.a,2);

 if(effectReady())gabi::call<void>(0x0279D8E0,effect);

 ref=gabi::call<u32>(0x027F29D4,0x104B45C0);
gabi::store<u32>(ref,handle);
gabi::store<u32>(ref+4,0);
gabi::call<void>(0x027BE2B0,texture.a,2);
gabi::call<void>(0x027BE2B0,firstView.a,2);

}

VERIFY(0x025BF250,Snap_DrawPacket);


// VERIFY
void Snap_DebugDraw(u32 s,u32 context){

 WWHD_FUNC(0x025BFDC0,void,s,context);

 if(!gabi::call<u32>(0x025BDC54,s))return;

 gabi::call<void>(0x025BF250,s,context);

 u32 p=gabi::load<u32>(s+0xF78);
gabi::call<void>(0x027B954C,p);
gabi::store<u32>(p+0x3C,0);

 p=gabi::load<u32>(s+0xF78);
gabi::store<u32>(p+0x1C,gabi::load<u32>(s+0xF70));
p=gabi::load<u32>(s+0xF78);
gabi::store<u32>(p+0x3C,gabi::load<u32>(s+0xF74));

 gabi::call<void>(0x0274D690,gabi::load<u32>(s+0xF78));

 gabi::Local<u8[24]> params;
gabi::call<void>(0x0274F5F0,params.a,gabi::load<u32>(s+0xF78));
gabi::call<void>(0x0274F964,params.a,gabi::load<u32>(s+0xF78));

 gabi::Local<u8[116]> state;
gabi::call<void>(0x0274FDBC,state.a);
gabi::store<u8>(state.a+0x44,0);
gabi::store<u32>(state.a+0x1C,0);
gabi::store<u8>(state.a+0x47,1);
gabi::store<u32>(state.a+0x14,1);
gabi::store<u8>(state.a+0x45,0);
gabi::store<u8>(state.a+0x46,0);
gabi::call<void>(0x0274FEB0,state.a);

 u32 drawer=gabi::load<u32>(0x101F8B28);

 if(drawer){

  u32 frame=gabi::load<u32>(context);
gabi::call<void>(0x027509A0,drawer,gabi::load<u32>(frame+0x14));
frame=gabi::load<u32>(context);
u32 camera=gabi::load<u32>(frame+0x18),projection=gabi::call<u32>(0x0274D83C,camera);

  gabi::Local<u8[64]> matrix;
for(u32 i=0;i<64;i+=4)gabi::store<u32>(matrix.a+i,gabi::load<u32>(projection+i));

  frame=gabi::load<u32>(context);
u32 buffer=gabi::load<u32>(frame+8);
f32 screenX=gabi::load<f32>(0x1047C9AC),screenY=gabi::load<f32>(0x1047C9B0);

  f32 width=gabi::load<f32>(buffer+0x10),scaledX=screenX*(width/1280.0f),height=gabi::load<f32>(buffer+0x14),scaledY=screenY*(height/720.0f);

  f32 scaleX=(gabi::load<f32>(buffer+0x10)/scaledX)*0.5f;
gabi::store<f32>(matrix.a,gabi::load<f32>(matrix.a)*scaleX);

  f32 scaleY=(gabi::load<f32>(buffer+0x14)/scaledY)*0.5f;
gabi::store<f32>(matrix.a+8,0.0f);
gabi::store<f32>(matrix.a+0x14,gabi::load<f32>(matrix.a+0x14)*scaleY);

  height=gabi::load<f32>(buffer+0x14);
f32 ratio=height/720.0f;
screenY=gabi::load<f32>(0x1047C9B0);
f32 center=gabi::fmadds(screenY,0.5f,140.0f)*ratio;
f32 delta=gabi::fmsubs(height,0.5f,center);
gabi::store<f32>(matrix.a+0x18,(delta+delta)/(screenY*ratio));

  gabi::Local<u8[212]> adjusted;
gabi::call<void>(0x0274EAF0,adjusted.a,matrix.a,0);
gabi::call<void>(0x0274D80C,adjusted.a);
gabi::call<void>(0x027509B4,drawer,adjusted.a);
gabi::call<void>(0x027509D8,drawer);

  projection=gabi::call<u32>(0x0274D83C,camera);
gabi::call<void>(0x028E8970,projection,matrix.a);
gabi::store<f32>(matrix.a,gabi::load<f32>(matrix.a)*0.5f);
gabi::store<f32>(matrix.a+0x14,gabi::load<f32>(matrix.a+0x14)*0.5f);

  gabi::call<void>(0x0274E910,gabi::load<u32>(s+0xF84),matrix.a,0);
gabi::call<void>(0x0274EB7C,adjusted.a,2);

 }

 gabi::call<void>(0x027B9F68,0);
u32 flags=gabi::call<u32>(0x027F29D4,0x104B45C0);
gabi::store<u32>(flags+4,0);
gabi::store<u32>(flags,3);

 for(s32 i=0;i<gabi::load<s32>(s+0x9C);++i)gabi::call<void>(0x025BF01C,s+0xA0+u32(i)*0x3C,u32(i)*4);

 if(drawer)gabi::call<void>(0x027509EC,drawer);

 gabi::call<void>(0x027F88C8,gabi::load<u32>(0x101F9968),11);
gabi::call<void>(0x027F88C8,gabi::load<u32>(0x101F9968),10);

 u32 frame=gabi::load<u32>(context),view=gabi::load<u32>(frame+0x14),dest=gabi::load<u32>(s+0xF80);

 for(u32 off=0x34;off<0x40;off+=4)gabi::store<f32>(dest+off,snap_raw_float(view+off));
f32 fourth=gabi::load<f32>(view+0x40);
dest=gabi::load<u32>(s+0xF80);
gabi::store<f32>(dest+0x40,fourth);
for(u32 off=0x44;off<0x4C;off+=4)gabi::store<f32>(dest+off,snap_raw_float(view+off));
dest=gabi::load<u32>(s+0xF80);
for(u32 off=0x4C;off<0x58;off+=4)gabi::store<f32>(dest+off,snap_raw_float(view+off));
gabi::call<void>(0x025155D8,dest+0x4C);

 dest=gabi::load<u32>(s+0xF80);
u32 table=gabi::load<u32>(dest+0x30);
gabi::call_ptr<void>(gabi::load<u32>(table+0x24),dest,dest);

 frame=gabi::load<u32>(context);
u32 buffer=gabi::load<u32>(frame+8);
dest=gabi::load<u32>(s+0xF88);
gabi::store<f32>(dest,snap_raw_float(buffer));
gabi::store<f32>(dest+4,snap_raw_float(buffer+4));

 frame=gabi::load<u32>(context);
buffer=gabi::load<u32>(frame+8);
dest=gabi::load<u32>(s+0xF88);
for(u32 off=8;off<24;off+=4)gabi::store<f32>(dest+off,snap_raw_float(buffer+off));
gabi::store<u32>(s+0xF64,(gabi::load<u32>(s+0xF64)&~1u)|2u);
frame=gabi::load<u32>(context);
gabi::call<void>(0x0274D690,gabi::load<u32>(frame+8));

}


VERIFY(0x025BFDC0,Snap_DebugDraw);

