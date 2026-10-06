#include "d/actor/d_a_tag_light.h"
#include "gabi.h"
using daTagLight::Act_c;
static u32 word(u32 a) { return *gabi::at<be<u32>>(a); }
static u8 byte(u32 a) { return *gabi::at<be<u8>>(a); }
static f32 flt(u32 a) { return *gabi::at<be<f32>>(a); }
static s32 parameter(Act_c* a,u32 width,u32 shift) { return gabi::call<s32>(0x024AE670,a,width,shift); }
/* HD material colors are represented as normalized float RGBA. */
void tagLightColor(be<f32>* out,be<u8>* in) {
 WWHD_FUNC(0x024ACEA4,void,out,in);
 f32 r=f32(u8(in[0]))/255.0f,g=f32(u8(in[1]))/255.0f,b=f32(u8(in[2]))/255.0f,a=f32(u8(in[3]))/255.0f;
 out[0]=r; out[1]=g; out[2]=b; out[3]=a;
}
VERIFY(0x024ACEA4,tagLightColor);
void lightStart(Act_c* a,cXyz* out) {
 WWHD_FUNC(0x024AD0F4,void,a,out);
 f32 ratio=f32(parameter(a,4,10))*0.06666667014360428f;
 out->x=0.0f; out->y=ratio<0.5f?135.0f:100.0f; out->z=0.0f;
}
VERIFY(0x024AD0F4,lightStart);
u8 lightSwitch(Act_c* a) {
 WWHD_FUNC(0x024AD93C,u8,a);
 s32 on=parameter(a,8,16),off=parameter(a,8,2);
 bool active=true;
 if(on!=255) {u32 save=word(0x101F84DC)+0x20; s32 room=a->home.roomNo; active=gabi::call<u32>(0x025BA0C0,save,on,room)!=0;}
 if(off!=255) {u32 save=word(0x101F84DC)+0x20; s32 room=a->home.roomNo; if(gabi::call<u32>(0x025BA0C0,save,off,room)) return 0;}
 return active;
}
VERIFY(0x024AD93C,lightSwitch);
u8 lightSchedule(Act_c* a) {
 WWHD_FUNC(0x024ADA2C,u8,a);
 u8 mask=parameter(a,8,24); u32 schedule=gabi::call<u32>(0x025602A8);
 return !mask || (mask&schedule)!=0;
}
VERIFY(0x024ADA2C,lightSchedule);
f32 collisionRadius(Act_c* a) {
 WWHD_FUNC(0x024ADA80,f32,a);
 u32 index=parameter(a,2,8);
 if(index>=4) gabi::call<void>(0x0273AA24,0x1003F84Cu,426,0x1003F860u);
 return f32(s16(*gabi::at<be<s16>>(0x1003F8A0u+index*2)));
}
VERIFY(0x024ADA80,collisionRadius);
u8 lightDelete(Act_c* a) {
 WWHD_FUNC(0x024AE044,u8,a);
 if(a->type==1) gabi::call<void>(0x025204C8,&a->phase,0x1003F8A8u);
 return 1;
}
VERIFY(0x024AE044,lightDelete);
u32 lightHit(Act_c* a) {
 WWHD_FUNC(0x024AE080,u32,a);
 if(gabi::call<u32>(0x025162A4,a->sphere)) {gabi::call<void>(0x0251621C,a->sphere);return 1;}
 u32 play=gabi::call<u32>(0x025200D4);
 return gabi::call<u32>(0x0252A038,play+0x5A20,&a->current.pos);
}
VERIFY(0x024AE080,lightHit);
u8 lightExecute(Act_c* a) {
 WWHD_FUNC(0x024AE0E8,u8,a);
 if(a->type==2) {
 bool hit=lightHit(a); s16 count=s16(u16(a->lightCounter)+(hit?1u:0xFFFFu));
 if(count<0)count=0; else if(count>20)count=20;
 a->lightCounter=count;
 if(count>10){s32 index=parameter(a,8,16);u32 save=word(0x101F84DC)+0x20;s32 room=a->home.roomNo;gabi::call<void>(0x025B9E38,save,index,room);}
 u32 play=gabi::call<u32>(0x025200D4);gabi::call<void>(0x0200E240,play+0x26A4,a->sphere);
 } else {
 u8 enabled=(gabi::call<u32>(0x024AD93C,a)!=0)&&(gabi::call<u32>(0x024ADA2C,a)!=0); a->enabled=enabled;
 f32 target=enabled?1.0f:0.0f; s32 fade=parameter(a,2,14); f32 speed=flt(0x1003F8F0+u32(fade)*4);
 gabi::call<s32>(0x0200F5C8,&a->alpha,target,speed);
 u32 btk=a->btk;if(btk)gabi::call<void>(0x025E742C,btk);
 }
 return 1;
}
VERIFY(0x024AE0E8,lightExecute);
void emptyLight(Act_c* a) {WWHD_FUNC(0x024AE5F8,void,a);}
VERIFY(0x024AE5F8,emptyLight);
u32 lightIsDelete(Act_c* a){WWHD_FUNC(0x024AE668,u32,a);return 1;}
VERIFY(0x024AE668,lightIsDelete);
s32 lightParameter(Act_c* a,u32 width,u32 shift) {
 WWHD_FUNC(0x024AE670,s32,a,width,shift);
 u32 mask=(width&32)?0u:(1u<<(width&31));
 u32 value=(shift&32)?0u:(u32(a->mParameters)>>(shift&31));
 return value&(mask-1);
}
VERIFY(0x024AE670,lightParameter);
void lightFree(void* a,u32 flags){WWHD_FUNC(0x024AE5E4,void,a,flags);if(a&&(flags&1))gabi::call<void>(0x0273AF40,a);}
VERIFY(0x024AE5E4,lightFree);
void lightDestructor(Act_c* a,u32 flags){
 WWHD_FUNC(0x024AE5FC,void,a,flags);
 if(a){gabi::call<void>(0x02515AE8,a->sphere,2);gabi::call<void>(0x02515860,a->collisionStatus,2);gabi::call<void>(0x025D50BC,a,0);if(flags&1)gabi::call<void>(0x0273AF40,a);}
}
VERIFY(0x024AE5FC,lightDestructor);
/* Keep guest locals above the outgoing linkage area, including the callee's SP+4 LR word. */
template<class T> struct LightLocal {
 struct Slot {u8 linkage[16];T value;}; gabi::Local<Slot> storage;
 T* get(){return &storage->value;}
};
u8 lightHeap(Act_c* a) {
 WWHD_FUNC(0x024ACF58,u8,a);
 f32 ratio=f32(parameter(a,4,10))*0.06666667014360428f;
 s32 modelIndex=ratio<0.5666667222976685f?4:5, animationIndex=ratio<0.5666667222976685f?8:9;
 struct Name {be<u32> text,vtable;};LightLocal<Name> modelName,animationName;
 modelName.get()->text=0x1003F8A8;modelName.get()->vtable=0x1003F724;
 u32 modelData=gabi::call<u32>(0x026066C4,word(0x101F4F28),modelName.get(),modelIndex);
 if(!modelData)gabi::call<void>(0x0273AA24,0x1003F7F0u,467,0x1003F814u);
 a->model=gabi::call<u32>(0x025E38E0,modelData,0x80000u,0x11000222u);
 animationName.get()->text=0x1003F8A8;animationName.get()->vtable=0x1003F724;
 u32 animation=gabi::call<u32>(0x026066C4,word(0x101F4F28),animationName.get(),animationIndex);
 if(!animation)gabi::call<void>(0x0273AA24,0x1003F7F0u,479,0x1003F804u);
 u32 btk=gabi::call<u32>(0x0273AD10,0x74u);if(btk)btk=gabi::call<u32>(0x025E7C6C,btk);a->btk=btk;
 bool initialized=false;
 if(btk)initialized=gabi::call<u32>(0x025E7CE0,btk,modelData,animation,1,2,0,1.0f,-1,0,0)!=0;
 return u32(a->model)!=0&&initialized;
}
VERIFY(0x024ACF58,lightHeap);
s32 lightHeapCallback(Act_c* a){WWHD_FUNC(0x024AD0F0,s32,a);return gabi::call<s32>(0x024ACF58,a);}
VERIFY(0x024AD0F0,lightHeapCallback);
void lightSpotMatrix(Act_c* a){
 WWHD_FUNC(0x024AD894,void,a);
 struct Vectors{cXyz bottom,end,center,offset,start,point,normal;};LightLocal<Vectors> locals;auto* v=locals.get();
 gabi::call<void>(0x024AD0F4,a,&v->offset);
 v->bottom.x=0;v->bottom.y=-50;v->bottom.z=0;
 gabi::call<void>(0x028E8F64,a->transform,&v->offset,&v->start);
 gabi::call<void>(0x028E8F64,a->transform,&v->bottom,&v->end);
 gabi::call<void>(0x024AD198,a,&v->point,&v->normal,&v->center,&v->start,&v->end);
 gabi::call<void>(0x024AD594,a,&v->point,&v->normal,&v->center,&v->start);
}
VERIFY(0x024AD894,lightSpotMatrix);
void lightSpot(Act_c* a){WWHD_FUNC(0x024AD934,void,a);gabi::call<void>(0x024AD894,a);}
VERIFY(0x024AD934,lightSpot);
void lightInitSpot(Act_c* a){WWHD_FUNC(0x024AD938,void,a);gabi::call<void>(0x024AD934,a);}
VERIFY(0x024AD938,lightInitSpot);
void lightMaterial(u32 material,u32 alpha){
 WWHD_FUNC(0x024AE290,void,material,alpha);
 if(!alpha){while(material){*gabi::at<be<u8>>(word(material+8)+4)=0;material=word(material+4);}return;}
 struct Colors{be<f32> linear[4],rgba[4];};LightLocal<Colors> colors;
 while(material){
 *gabi::at<be<u8>>(word(material+8)+4)=1;
 u32 tev=word(material+0x18),vt=word(tev+4);
 u32 color=gabi::call_ptr<u32>(word(vt+0x4C),tev,3);*gabi::at<be<u8>>(color+3)=u8(alpha);
 tev=word(material+0x18);vt=word(tev+4);color=gabi::call_ptr<u32>(word(vt+0x4C),tev,3);
 tev=word(material+0x18);vt=word(tev+4);gabi::call_ptr<void>(word(vt+0x3C),tev,3,color);
 gabi::call<void>(0x024ACEA4,colors.get()->rgba,color);
 gabi::call<void>(0x0274D458,colors.get()->linear,colors.get()->rgba,1.0f);
 *gabi::at<be<u32>>(material+0xA0)=word(material+0xA0)|0x400;
 u32 output=gabi::call<u32>(0x027F9F0C,material+0xA0,10);
 f32 opacity=f32(byte(color+3))/255.0f;
 f32 red=colors.get()->linear[0],green=colors.get()->linear[1],blue=colors.get()->linear[2];
 auto* rgba=gabi::at<be<f32>>(output);rgba[0]=red;rgba[1]=green;rgba[2]=blue;rgba[3]=opacity;
 material=word(material+4);
 }
}
VERIFY(0x024AE290,lightMaterial);
u8 lightDraw(Act_c* a){
 WWHD_FUNC(0x024AE430,u8,a);
 if(a->type==1&&f32(a->alpha)>0.0f){
 u32 env=gabi::call<u32>(0x02555D0C);gabi::call<void>(0x025626A4,env,1,&a->current.pos,&a->tevStr);
 env=gabi::call<u32>(0x02555D0C);u32 model=a->model;gabi::call<void>(0x02562F5C,env,model,&a->tevStr);
 u32 btk=a->btk;if(btk){model=a->model;f32 frame=flt(btk+4);u32 data=word(model+0xAC);gabi::call<void>(0x025E7FC4,btk,data,frame);}
 u8 alpha=u8(gabi::ftoi(f32(a->alpha)*255.5f));u32 data=word(u32(a->model)+0xAC);
 u32 count=word(data+4),joints=word(data+8);if(count>1)joints+=0x1C;
 gabi::call<void>(0x024AE290,word(joints+0x10),u32(alpha));
 count=word(data+4);joints=word(data+8);if(count>2)joints+=0x38;
 gabi::call<void>(0x024AE290,word(joints+0x10),u32(alpha));
 model=a->model;gabi::call<void>(0x025E2DE0,model,0);
 }return 1;
}
VERIFY(0x024AE430,lightDraw);
void lightStaticInit(){
 WWHD_FUNC(0x024AE550,void);
 auto* v=gabi::at<be<u32>>(0x1046E588);v[2]=0;v[0]=0;v[3]=0;v[1]=0;
 gabi::call<void>(0x028F026C,0x101D1CF0u);
 *gabi::at<be<f32>>(0x1046E57C)=-3.1415927410125732f;*gabi::at<be<f32>>(0x1046E580)=3.1415927410125732f;
 gabi::call<void>(0x028ED6F8,0x1046E584u);gabi::call<void>(0x028F026C,0x101D1CFCu);
 gabi::call<void>(0x028EAB2C,0x1046E585u);gabi::call<void>(0x028F026C,0x101D1D08u);
}
VERIFY(0x024AE550,lightStaticInit);
s32 lightCreate(Act_c* a){
 WWHD_FUNC(0x024ADB04,s32,a);
 if(!(u32(a->actor_condition)&8)){
 if(a){gabi::call<void>(0x025D4ED0,a);a->__vtbl=0x1003F7DC;gabi::call<void>(0x0200BD2C,a->collisionStatus);gabi::call<void>(0x02515DA0,gabi::ea(a)+0x444);*gabi::at<be<u32>>(gabi::ea(a)+0x440)=0x1004AE88;*gabi::at<be<u32>>(gabi::ea(a)+0x444)=0x1004AEC0;gabi::call<void>(0x025166F0,a->sphere);}
 a->actor_condition=u32(a->actor_condition)|8;
 }
 s32 type=parameter(a,2,0);f32 z=a->scale.z;a->type=type;f32 y=a->scale.y,x=a->scale.x;
 f32 scaledZ=z;if(type==1)y=y+y;else if(type==0){x=x+x;y=y+y;scaledZ=z+z;}
 a->volumeScale.x=x;a->volumeScale.z=scaledZ;a->volumeScale.y=y;
 constexpr u32 stack=0x1048D0CC;
 f32 py=a->current.pos.y,px=a->current.pos.x,pz=a->current.pos.z;gabi::call<void>(0x028E93CC,stack,px,py,pz);
 s16 rz=a->shape_angle.z,rx=a->shape_angle.x,ry=a->shape_angle.y;gabi::call<void>(0x025F1B48,stack,rx,ry,rz);
 gabi::call<void>(0x025F2518,x,y,scaledZ);
 gabi::call<void>(0x028E90D4,stack,a->transform);gabi::call<s32>(0x028E91EC,a->transform,a->inverse);
 s32 phase=4;
 if(a->type==1){phase=gabi::call<s32>(0x02520460,&a->phase,0x1003F8A8u);if(phase!=4)return phase;
 if(!gabi::call<u32>(0x025D63E8,a,0x024AD0F0u,0xAC0u))return 5;
 pz=a->current.pos.z;px=a->current.pos.x;py=a->current.pos.y;gabi::call<void>(0x028E93CC,stack,px,py,pz);
 rz=a->shape_angle.z;rx=a->shape_angle.x;ry=a->shape_angle.y;gabi::call<void>(0x025F1B48,stack,rx,ry,rz);
 f32 values[12];for(int i=0;i<12;i++)values[i]=flt(stack+i*4);
 auto* m=gabi::at<be<f32>>(u32(a->model)+0xC8);for(int i=0;i<12;i++)m[i]=values[i];
 m=gabi::at<be<f32>>(u32(a->model)+0xBC);m[2]=scaledZ;m[0]=x;m[1]=y;gabi::call<void>(0x024AD938,a);
 }
 a->cullMtx=gabi::ea(a->transform);gabi::call<void>(0x025D674C,a,-51.0f,-1.0f,-51.0f,51.0f,101.0f,51.0f);
 bool active=false;if(a->type!=2)active=gabi::call<u32>(0x024AD93C,a)&&gabi::call<u32>(0x024ADA2C,a);
 s32 currentType=a->type;a->enabled=active;a->lightCounter=0;a->alpha=active?1.0f:0.0f;
 if(currentType==2){gabi::call<void>(0x02515F14,a->collisionStatus,255,255,a);gabi::call<void>(0x0251677C,a->sphere,0x1003F8B0u);*gabi::at<be<u32>>(gabi::ea(a)+0x4A8)=gabi::ea(a->collisionStatus);gabi::call<void>(0x02018D40,gabi::ea(a)+0x57C,&a->current.pos);f32 radius=gabi::call<f32>(0x024ADA80,a);gabi::call<void>(0x02018C8C,gabi::ea(a)+0x57C,radius);}
 return phase;
}
VERIFY(0x024ADB04,lightCreate);
s32 lightCreateMethod(Act_c* a){WWHD_FUNC(0x024AE540,s32,a);return gabi::call<s32>(0x024ADB04,a);}
VERIFY(0x024AE540,lightCreateMethod);
s32 lightDeleteMethod(Act_c* a){WWHD_FUNC(0x024AE544,s32,a);return gabi::call<s32>(0x024AE044,a);}
VERIFY(0x024AE544,lightDeleteMethod);
s32 lightExecuteMethod(Act_c* a){WWHD_FUNC(0x024AE548,s32,a);return gabi::call<s32>(0x024AE0E8,a);}
VERIFY(0x024AE548,lightExecuteMethod);
s32 lightDrawMethod(Act_c* a){WWHD_FUNC(0x024AE54C,s32,a);return gabi::call<s32>(0x024AE430,a);}
VERIFY(0x024AE54C,lightDrawMethod);
void lightProjectionMatrix(Act_c* a,const cXyz* point,const cXyz* normal,const cXyz* center,const cXyz* start){
 WWHD_FUNC(0x024AD594,void,a,point,normal,center,start);
 bool narrow=f32(parameter(a,4,10))*0.06666667014360428f<0.5f;
 struct Data{cXyz normal,rotated;be<f32> matrix[12];};LightLocal<Data> local;auto* d=local.get();
 d->normal.x=normal->x;d->normal.y=normal->y;d->normal.z=normal->z;
 constexpr u32 stack=0x1048D0CC;
 s16 x=a->shape_angle.x,z=a->shape_angle.z,y=a->shape_angle.y;gabi::call<void>(0x025F1AA4,stack,x,y,z);
 gabi::call<void>(0x028E90D4,stack,d->matrix);gabi::call<void>(0x028E8F64,d->matrix,&d->normal,&d->rotated);
 f32 xx=d->rotated.x,zz=d->rotated.z;s16 heading=gabi::call<s16>(0x020195B0,zz,xx);
 zz=d->rotated.z;xx=d->rotated.x;f32 length=gabi::call<f32>(0x028F4384,gabi::fmadds(xx,xx,zz*zz));
 f32 yy=d->rotated.y;s16 inclination=gabi::call<s16>(0x020195B0,length,std::fabs(yy));
 u32 cosineAddress=0x104A44F8u+(u32(u16(inclination))>>3)*8+4;
 f32 cosine=flt(cosineAddress);if(std::fabs(cosine)<0.0010000000474974513f){a->projectionEnabled=0;return;}
 a->projectionEnabled=1;f32 halfX=f32(a->volumeScale.x)/flt(cosineAddress)*0.5f,halfZ=f32(a->volumeScale.z)*0.5f;
 f32 py=point->y,px,pz,factor;
 if(!narrow){px=point->x;pz=point->z;factor=1.034999966621399f;}
 else {f32 sy=start->y,ay=a->current.pos.y,ratio=(py-sy)/(ay-sy);f32 adjust=flt(0x1047C150);py=center->y;px=center->x;pz=center->z;halfZ=halfZ*ratio;halfX=halfX*ratio;factor=adjust+0.9700000286102295f;}
 gabi::call<void>(0x028E93CC,stack,px,py,pz);
 y=a->shape_angle.y;gabi::call<void>(0x025F1C28,stack,s16(u16(y)-u16(heading)));
 y=a->shape_angle.y;gabi::call<void>(0x025F1C28,stack,s16(-s32(y)));
 f32 middle=(halfX+halfZ)*0.5f;gabi::call<void>(0x025F2518,halfX*factor,middle*factor,halfZ*factor);
 y=a->shape_angle.y;gabi::call<void>(0x025F1C28,stack,y);gabi::call<void>(0x028E90D4,stack,a->projection);
}
VERIFY(0x024AD594,lightProjectionMatrix);
void lightProjectionInfo(Act_c* a,cXyz* point,cXyz* normal,cXyz* center,const cXyz* start,const cXyz* end){
 WWHD_FUNC(0x024AD198,void,a,point,normal,center,start,end);
 center->copy(a->current.pos);
 bool narrow=f32(parameter(a,4,10))*0.06666667014360428f<0.5f;
 struct Data{cXyz offsets[4],transformed,cross[4];u8 check[0x6C];};LightLocal<Data> local;auto* d=local.get();u32 check=gabi::ea(d->check);
 gabi::call<void>(0x02008FEC,check);
 *gabi::at<be<u32>>(check)=check+0x58;*gabi::at<be<u32>>(check+4)=check+0x64;
 point->copy(a->current.pos);
 *gabi::at<be<u32>>(check+0x10)=0x1003F79C;*gabi::at<be<u32>>(check+0x64)=0x1003F7BC;
 for(int i=0;i<3;i++)*gabi::at<be<u32>>(gabi::ea(normal)+i*4)=word(0x101FFBC0+i*4);
 for(int i=1;i<7;i++)*gabi::at<be<u8>>(check+0x5C+i)=0;
 *gabi::at<be<u8>>(check+0x5C)=1;
 *gabi::at<be<u32>>(check+0x68)=1;*gabi::at<be<u32>>(check+0x20)=0x1003F7AC;*gabi::at<be<u32>>(check+0x58)=0x1003F7CC;
 gabi::call<void>(0x024F1AFC,check,start,end,a);
 u32 play=gabi::call<u32>(0x025200D4);
 if(gabi::call<u32>(0x024EF4A8,play+0x12A0,check)){
 point->x=flt(check+0x30);point->y=flt(check+0x34);point->z=flt(check+0x38);
 play=gabi::call<u32>(0x025200D4);u16 triangle=*gabi::at<be<u16>>(check+0x16),mesh=*gabi::at<be<u16>>(check+0x14);
 u32 plane=gabi::call<u32>(0x020084C8,play+0x12A0,u32(triangle),u32(mesh));
 if(plane){normal->x=flt(plane);normal->y=flt(plane+4);normal->z=flt(plane+8);
 if(narrow){f32 width=f32(a->volumeScale.x)*12.0f,depth=f32(a->volumeScale.z)*12.0f;
 for(int i=0;i<4;i++){
 d->cross[i].copy(*point);
 d->offsets[0].x=width;d->offsets[0].y=0;d->offsets[0].z=0;
 d->offsets[1].x=-width;d->offsets[1].y=0;d->offsets[1].z=0;
 d->offsets[2].x=0;d->offsets[2].y=0;d->offsets[2].z=depth;
 d->offsets[3].x=0;d->offsets[3].y=0;d->offsets[3].z=-depth;
 gabi::call<void>(0x028E8F64,a->transform,&d->offsets[i],&d->transformed);
 f32 ny=flt(plane+4),ty=d->transformed.y,sy=start->y,tx=d->transformed.x,sx=start->x,sz=start->z;
 f32 dx=tx-sx,dy=ty-sy,tz=d->transformed.z,dz=tz-sz,nx=flt(plane),nz=flt(plane+8);
 f32 denominator=gabi::fmadds(nz,dz,gabi::fmadds(nx,dx,ny*dy)),distance=flt(plane+12);
 if(denominator!=0){f32 numerator=gabi::fmadds(nz,sz,gabi::fmadds(nx,sx,ny*sy))+distance,t=-numerator/denominator;
 d->cross[i].x=gabi::fmadds(dx,t,sx);d->cross[i].y=gabi::fmadds(dy,t,sy);d->cross[i].z=gabi::fmadds(dz,t,sz);}
 }
 f32 cx=((f32(d->cross[0].x)+f32(d->cross[1].x))+f32(d->cross[2].x))+f32(d->cross[3].x);
 f32 cy=((f32(d->cross[0].y)+f32(d->cross[1].y))+f32(d->cross[2].y))+f32(d->cross[3].y);
 f32 cz=((f32(d->cross[0].z)+f32(d->cross[1].z))+f32(d->cross[2].z))+f32(d->cross[3].z);
 center->x=cx*0.25f;center->y=cy*0.25f;center->z=cz*0.25f;
 }} }
 *gabi::at<be<u32>>(check+0x58)=0x1003F78C;*gabi::at<be<u32>>(check+0x64)=0x1003F74C;*gabi::at<be<u32>>(check+0x20)=0x1003F73C;
 gabi::call<void>(0x02008B4C,check,0);
}
VERIFY(0x024AD198,lightProjectionInfo);
