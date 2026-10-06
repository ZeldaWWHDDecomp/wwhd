#include "gabi.h"
using namespace gabi;
static bool cylinder(s16 t) {return t==0||t==2||t==3||t==5||t==7;}
static bool sphere(s16 t) {return t==1||t==4||t==6||t==8;}
u32 JntHit_ctor_hd(u32 p) {
 WWHD_FUNC(0x025528D8,u32,p);
 if(!p) p=call<u32>(0x0273AD10,32u);
 if(p) {store<u32>(p+8,0);store<u32>(p+12,0);store<u32>(p+16,0);store<u32>(p+28,0);store<u32>(p,0);store<u32>(p+20,0);store<u16>(p+24,0);store<u32>(p+4,0);}
 return p;
}
VERIFY(0x025528D8,JntHit_ctor_hd);
u32 JntHit_CreateInit_hd(u32 p) {
 WWHD_FUNC(0x02552930,u32,p);
 const s32 n=load<s16>(p+24);u32 pos=0,data=load<u32>(p);
 for(s32 i=0;i<n;++i) {const s16 t=load<s16>(data+12*i);if(cylinder(t))pos+=2;else if(sphere(t))pos++;}
 store<u32>(p+16,call<u32>(0x0273ADAC,u32(n)*2));
 store<u32>(p+8,call<u32>(0x028EFFD0,0u,pos,12u,0u));
 store<u32>(p+12,call<u32>(0x0273ADAC,u32(s32(load<s16>(p+24)))*4));
 const s32 n2=load<s16>(p+24);
 u32 joints=call<u32>(0x0273ADAC,u32(n2)*2);u32 types=load<u32>(p+16);store<u32>(p+20,joints);
 if(!types)return 0;u32 offsets=load<u32>(p+8);if(!offsets)return 0;u32 radii=load<u32>(p+12);if(!radii||!joints)return 0;
 data=load<u32>(p);
 for(s32 i=0;i<s32(load<s16>(p+24));++i) {
  store<s16>(types,load<s16>(data));store<s16>(joints,load<s16>(data+2));store<u32>(radii,load<u32>(data+4));
  const s16 t=load<s16>(types);
  if(cylinder(t)) {u32 v=load<u32>(data+8);store<u32>(offsets,load<u32>(v));store<u32>(offsets+4,load<u32>(v+4));store<u32>(offsets+8,load<u32>(v+8));v=load<u32>(data+8);store<u32>(offsets+12,load<u32>(v+12));store<u32>(offsets+16,load<u32>(v+16));store<u32>(offsets+20,load<u32>(v+20));offsets+=12;}
  else if(sphere(t)) {const u32 v=load<u32>(data+8);store<u32>(offsets,load<u32>(v));store<u32>(offsets+4,load<u32>(v+4));store<u32>(offsets+8,load<u32>(v+8));}
  offsets+=12;types+=2;joints+=2;radii+=4;data+=12;
 }
 return 1;
}
VERIFY(0x02552930,JntHit_CreateInit_hd);
u32 JntHit_create_hd(u32 model,u32 data,u32 count) {
 WWHD_FUNC(0x02552B60,u32,model,data,count);
 u32 p=call<u32>(0x0273AD10,32u);if(!p)return 0;p=call<u32>(0x025528D8,p);if(!p)return 0;
 store<u32>(p+4,model);store<u16>(p+24,u16(count));store<u32>(p,data);
 return call<u32>(0x02552930,p)?p:0;
}
VERIFY(0x02552B60,JntHit_create_hd);
u32 JntHit_HIO_ctor_hd(u32 p) {
 WWHD_FUNC(0x02552BE8,u32,p);
 if(!p)p=call<u32>(0x0273AD10,44u);if(!p)return 0;
 store<u8>(p,0);const f32 a=load<f32>(0x1004EF1C);store<u8>(p+36,0);store<f32>(p+8,a);store<u16>(p+4,0);
 const f32 b=load<f32>(0x1004EF24);store<u16>(p+2,0);store<u32>(p+40,0x1004EF0Cu);
 const u32 x=load<u32>(0x101FFBA8);const f32 z=load<f32>(0x1004EF20);store<u32>(p+12,x);store<u32>(p+16,load<u32>(0x101FFBAC));const u32 zz=load<u32>(0x101FFBB0);
 store<f32>(p+32,b);store<u32>(p+20,zz);store<f32>(p+28,z);store<f32>(p+24,z);store<u8>(p,0xFF);return p;
}
VERIFY(0x02552BE8,JntHit_HIO_ctor_hd);

struct JntVec {f32 x,y,z;};
u32 JntHit_Sph_hd(u32 self,u32 pos,u32 rot,u32 outpos,u32 outrot,u32 center,f32 radius) {
 WWHD_FUNC(0x02552C88,u32,self,pos,rot,outpos,outrot,center,radius);
 Local<JntVec> dir,scaled,point,normalized;
 const u32 ax=(u32(load<u16>(rot))>>3)*8+0x104A44F8u,ay=(u32(load<u16>(rot+2))>>3)*8+0x104A44F8u;
 const f32 cx=load<f32>(ax+4),sy=load<f32>(ay),sx=load<f32>(ax),cy=load<f32>(ay+4);
 store<f32>(dir.a,cx*sy);store<f32>(dir.a+4,-sx);store<f32>(dir.a+8,cx*cy);
 const f32 dot=call<f32>(0x028E8F44,pos,dir.a);
 const f32 mag2=call<f32>(0x028E8DD0,pos);
 f32 disc=fmadds(dot,dot,radius*radius)-mag2;
 if(!(disc>=0))disc=load<f32>(0x1004EF20);
 const f32 root=call<f32>(0x028F4384,disc),scale=-dot-root;
 call<void>(0x0201AE48,dir.a,scaled.a,scale);
 for(u32 i=0;i<12;i+=4)store<u32>(point.a+i,load<u32>(scaled.a+i));
 call<void>(0x028E8D88,point.a,pos,point.a);
 const f32 square=call<f32>(0x028E8DD0,point.a);const f32 length=call<f32>(0x028F4384,square);
 if(length>radius) {call<void>(0x0201B3C0,point.a,normalized.a);call<void>(0x028E8E64,point.a,point.a,radius);}
 call<void>(0x028E8D88,point.a,center,point.a);
 constexpr u32 matrix=0x1048D0CCu;call<void>(0x028E91EC,matrix,matrix);
 const f32 z=load<f32>(point.a+8),x=load<f32>(point.a),y=load<f32>(point.a+4);
 call<void>(0x025F24E0,x,y,z);
 call<void>(0x025F1B48,matrix,s32(load<s16>(rot)),s32(load<s16>(rot+2)),s32(load<s16>(rot+4)));
 store<f32>(outpos,load<f32>(matrix+12));store<f32>(outpos+4,load<f32>(matrix+28));store<f32>(outpos+8,load<f32>(matrix+44));call<void>(0x025F232C,matrix,outrot);return 1;
}
VERIFY(0x02552C88,JntHit_Sph_hd);

u32 JntHit_Buffer_hd(u32 self,u32 index,u32 best,u32 joint,u32 rot,u32 point) {
 WWHD_FUNC(0x025537B4,u32,self,index,best,joint,rot,point);
 Local<JntVec> transformed,direction,difference;
 const u32 model=load<u32>(self+4),buffer=load<u32>(model+44),matrices=load<u32>(buffer+16);const u16 flags=load<u16>(buffer+4);
 store<u16>(buffer+4,flags|16);call<void>(0x028E90D4,matrices+joint*48,0x1048D0CCu);call<void>(0x028E8F64,0x1048D0CCu,point,transformed.a);
 if(load<s32>(index)>=0) {
  const u32 ax=(u32(load<u16>(rot))>>3)*8+0x104A44F8u,ay=(u32(load<u16>(rot+2))>>3)*8+0x104A44F8u;
  const f32 sx=load<f32>(ax),sy=load<f32>(ay),cx=load<f32>(ax+4),cy=load<f32>(ay+4);
  store<f32>(direction.a+4,-sx);store<f32>(direction.a,cx*sy);store<f32>(direction.a+8,cx*cy);
  call<void>(0x0201ADE0,transformed.a,difference.a,best);
  const f32 dot=call<f32>(0x028E8F44,difference.a,direction.a),zero=load<f32>(0x1004EF20);
  if(!(dot<zero))return 0;
 }
 const u32 y=load<u32>(transformed.a+4),z=load<u32>(transformed.a+8),x=load<u32>(transformed.a);
 store<u32>(index,joint);store<u32>(best,x);store<u32>(best+4,y);store<u32>(best+8,z);return 1;
}
VERIFY(0x025537B4,JntHit_Buffer_hd);

static void jnt_copy(u32 dst,u32 src) {for(u32 i=0;i<12;i+=4)store<u32>(dst+i,load<u32>(src+i));}
static void jnt_direction(u32 dst,u32 rot) {
 const u32 ax=(u32(load<u16>(rot))>>3)*8+0x104A44F8u,ay=(u32(load<u16>(rot+2))>>3)*8+0x104A44F8u;
 const f32 cx=load<f32>(ax+4),sy=load<f32>(ay),sx=load<f32>(ax),cy=load<f32>(ay+4);
 store<f32>(dst,fmuls_ppc(cx,sy));store<f32>(dst+4,-sx);store<f32>(dst+8,fmuls_ppc(cx,cy));
}
static void jnt_finish(u32 point,u32 rot,u32 outpos,u32 outrot) {
 constexpr u32 m=0x1048D0CCu;call<void>(0x028E91EC,m,m);
 const f32 z=load<f32>(point+8),x=load<f32>(point),y=load<f32>(point+4);call<void>(0x025F24E0,x,y,z);
 call<void>(0x025F1B48,m,s32(load<s16>(rot)),s32(load<s16>(rot+2)),s32(load<s16>(rot+4)));
 store<u32>(outpos,load<u32>(m+12));store<u32>(outpos+4,load<u32>(m+28));store<u32>(outpos+8,load<u32>(m+44));call<void>(0x025F232C,m,outrot);
}
u32 JntHit_Cyl2_hd(u32 self,u32 pos,u32 rot,u32 outpos,u32 outrot,u32 base,u32 end,f32 radius) {
 WWHD_FUNC(0x02553388,u32,self,pos,rot,outpos,outrot,base,end,radius);
 Local<JntVec> delta,axis,dir,scaled,sum,point,radial,projection,saved,changed;
 call<void>(0x0201ADE0,end,delta.a,base);call<void>(0x0201B12C,delta.a,axis.a);
 const f32 square=call<f32>(0x028E8DD0,delta.a);const f32 length=call<f32>(0x028F4384,square);
 const f32 longitudinal=call<f32>(0x028E8F44,pos,axis.a);
 jnt_direction(dir.a,rot);const f32 alignment=call<f32>(0x028E8F44,dir.a,axis.a);
 if(!(std::fabs(alignment)<load<f32>(0x1004EF28))) {
  const f32 zero=load<f32>(0x1004EF20);const f32 scale=alignment>zero ? -longitudinal/alignment : fsubs_ppc(length,longitudinal)/alignment;
  jnt_copy(scaled.a,dir.a);call<void>(0x028E8E64,scaled.a,scaled.a,scale);call<void>(0x0201AD78,pos,sum.a,scaled.a);jnt_copy(point.a,sum.a);jnt_copy(radial.a,sum.a);
 } else {jnt_copy(point.a,pos);jnt_copy(radial.a,pos);}
 const f32 along=call<f32>(0x028E8F44,radial.a,axis.a);jnt_copy(projection.a,axis.a);call<void>(0x028E8E64,projection.a,projection.a,along);call<void>(0x028E8DAC,radial.a,projection.a,radial.a);jnt_copy(saved.a,radial.a);
 const f32 rad2=call<f32>(0x028E8DD0,radial.a);const f32 radlen=call<f32>(0x028F4384,rad2);
 if(radlen>radius&&call<u32>(0x0201B47C,radial.a)) {call<void>(0x028E8E64,radial.a,radial.a,radius);call<void>(0x0201ADE0,saved.a,changed.a,radial.a);jnt_copy(radial.a,changed.a);call<void>(0x028E8DAC,point.a,radial.a,point.a);}
 call<void>(0x028E8D88,point.a,base,point.a);jnt_finish(point.a,rot,outpos,outrot);return 1;
}
VERIFY(0x02553388,JntHit_Cyl2_hd);

u32 JntHit_Cyl_hd(u32 self,u32 pos,u32 rot,u32 outpos,u32 outrot,u32 base,u32 end,f32 radius) {
 WWHD_FUNC(0x02552E50,u32,self,pos,rot,outpos,outrot,base,end,radius);
 Local<JntVec> delta,axis,projection,radial,dir,projectedDir,scaled,point,corrected,aux,secondAux;
 call<void>(0x0201ADE0,end,delta.a,base);call<void>(0x0201B12C,delta.a,axis.a);
 const f32 square=call<f32>(0x028E8DD0,delta.a);const f32 length=call<f32>(0x028F4384,square);
 const f32 along=call<f32>(0x028E8F44,pos,axis.a);jnt_copy(radial.a,pos);jnt_copy(projection.a,axis.a);call<void>(0x028E8E64,projection.a,projection.a,along);call<void>(0x028E8DAC,radial.a,projection.a,radial.a);
 jnt_direction(dir.a,rot);const f32 alignment=call<f32>(0x028E8F44,dir.a,axis.a);jnt_copy(projectedDir.a,dir.a);jnt_copy(projection.a,axis.a);call<void>(0x028E8E64,projection.a,projection.a,alignment);call<void>(0x028E8DAC,projectedDir.a,projection.a,projectedDir.a);
 const u32 normal=call<u32>(0x0201B47C,projectedDir.a);const f32 zero=load<f32>(0x1004EF20);bool adjust;
 if(normal) {
  const f32 dot=call<f32>(0x028E8F44,radial.a,projectedDir.a);const f32 rsquare=call<f32>(0x028E8DD0,radial.a);
  f32 disc=fsubs_ppc(fmadds(dot,dot,fmuls_ppc(radius,radius)),rsquare);if(!(disc>=0))disc=zero;
  const f32 root=call<f32>(0x028F4384,disc);call<void>(0x0201AE48,dir.a,scaled.a,fsubs_ppc(-dot,root));jnt_copy(point.a,scaled.a);call<void>(0x028E8D88,point.a,pos,point.a);adjust=disc==zero;
 } else {jnt_copy(point.a,pos);adjust=true;}
 if(adjust) {
  jnt_copy(corrected.a,point.a);const f32 dot=call<f32>(0x028E8F44,point.a,axis.a);
  const u32 tmp=normal?delta.a:secondAux.a;
  call<void>(0x0201AE48,axis.a,tmp,dot);call<void>(0x028E8DAC,corrected.a,tmp,corrected.a);call<void>(0x0201B3C0,corrected.a,tmp);call<void>(0x0201AE48,corrected.a,tmp,radius);jnt_copy(corrected.a,tmp);
  const f32 dot2=call<f32>(0x028E8F44,point.a,axis.a);call<void>(0x0201AE48,axis.a,tmp,dot2);call<void>(0x028E8D88,corrected.a,tmp,corrected.a);jnt_copy(point.a,corrected.a);
 }
 const f32 position=call<f32>(0x028E8F44,point.a,axis.a);f32 correction=zero;
 if(position<zero)correction=-position;else if(position>length)correction=fsubs_ppc(length,position);
 call<void>(0x0201AE48,axis.a,aux.a,correction);call<void>(0x028E8D88,point.a,aux.a,point.a);call<void>(0x028E8D88,point.a,base,point.a);jnt_finish(point.a,rot,outpos,outrot);return 1;
}
VERIFY(0x02552E50,JntHit_Cyl_hd);

static bool jnt_throw(s16 t){return t==3||t==4;}
static bool jnt_delete(s16 t){return t==7||t==8;}
static void jnt_matrix(u32 model,s32 joint){const u32 b=load<u32>(model+44),m=load<u32>(b+16);const u16 flags=load<u16>(b+4);store<u16>(b+4,flags|16);call<void>(0x028E90D4,m+u32(joint)*48,0x1048D0CCu);}
s32 JntHit_Search_hd(u32 self,u32 pos,u32 rot,u32 outpos,u32 outrot) {
 WWHD_FUNC(0x025538E4,s32,self,pos,rot,outpos,outrot);
 Local<JntVec> a,b,delta,axis,relative,cross,best,savedpos,transformed,temp;
 struct Ang {s16 x,y,z;};Local<Ang> savedrot;Local<s32> bestjoint;
 store<s32>(bestjoint.a,-1);
 u32 types=load<u32>(self+16),joints=load<u32>(self+20),radii=load<u32>(self+12),offsets=load<u32>(self+8),model=load<u32>(self+4);
 f32 nearest=load<f32>(0x1004EF2C);s32 chosen=-1,hit=0,hitpos=0,offsetindex=0;
 const f32 zero=load<f32>(0x1004EF20),epsilon=load<f32>(0x1004EF28);
 for(s32 i=0;i<s32(load<s16>(self+24));++i) {
  jnt_matrix(model,load<s16>(joints));s16 shape=load<s16>(types);
  bool record=false;
  if(cylinder(shape)) {
   call<void>(0x028E8F64,0x1048D0CCu,offsets,a.a);call<void>(0x028E8F64,0x1048D0CCu,offsets+12,b.a);call<void>(0x0201ADE0,b.a,delta.a,a.a);jnt_copy(axis.a,delta.a);call<void>(0x0201ADE0,pos,temp.a,a.a);jnt_copy(relative.a,temp.a);
   if(!call<u32>(0x0201B47C,axis.a)) {
    const f32 sq=call<f32>(0x028E8DD0,relative.a),len=call<f32>(0x028F4384,sq),radius=load<f32>(radii);f32 d=fsubs_ppc(len,radius);if(!(d>=0))d=zero;
    if(!(d>nearest)) {chosen=load<s16>(joints);nearest=d;hit=i;hitpos=offsetindex;
     if(d<radius){call<u32>(0x02552C88,self,relative.a,rot,outpos,outrot,a.a,radius);record=call<u32>(0x025537B4,self,bestjoint.a,best.a,chosen,rot,outpos)!=0;}
    }
   } else {
    call<void>(0x0201B080,axis.a,cross.a,relative.a);const f32 csq=call<f32>(0x028E8DD0,cross.a),clen=call<f32>(0x028F4384,csq),radius=load<f32>(radii);f32 radial=fsubs_ppc(clen,radius);if(!(radial>=0))radial=zero;
    const f32 longitudinal=call<f32>(0x028E8F44,axis.a,relative.a);f32 excess=zero,d;
    if(longitudinal<zero){excess=-longitudinal;d=call<f32>(0x028F4384,fmadds(longitudinal,longitudinal,fmuls_ppc(radial,radial)));}
    else {const f32 dsq=call<f32>(0x028E8DD0,delta.a),dlen=call<f32>(0x028F4384,dsq);
     if(longitudinal>dlen){const f32 dsq2=call<f32>(0x028E8DD0,delta.a),dlen2=call<f32>(0x028F4384,dsq2);excess=fsubs_ppc(longitudinal,dlen2);d=call<f32>(0x028F4384,fmadds(excess,excess,fmuls_ppc(radial,radial)));}
     else d=call<f32>(0x028F4384,fmuls_ppc(radial,radial));
    }
    if(!(d>nearest)){chosen=load<s16>(joints);nearest=d;hit=i;hitpos=offsetindex;
     if(std::fabs(excess)<epsilon&&std::fabs(radial)<epsilon){shape=load<s16>(types);if(!jnt_throw(shape)){
      if(shape==0)call<u32>(0x02552E50,self,relative.a,rot,outpos,outrot,a.a,b.a,load<f32>(radii));else if(shape==2)call<u32>(0x02553388,self,relative.a,rot,outpos,outrot,a.a,b.a,load<f32>(radii));
      if(jnt_delete(load<s16>(types)))return -3;record=call<u32>(0x025537B4,self,bestjoint.a,best.a,chosen,rot,outpos)!=0;
     }}
    }
   }
   offsets+=24;offsetindex+=2;
  } else if(sphere(shape)) {
   call<void>(0x028E8F64,0x1048D0CCu,offsets,a.a);call<void>(0x0201ADE0,pos,temp.a,a.a);jnt_copy(relative.a,temp.a);
   const f32 sq=call<f32>(0x028E8DD0,relative.a),len=call<f32>(0x028F4384,sq),radius=load<f32>(radii);f32 d=fsubs_ppc(len,radius);if(!(d>=0))d=zero;
   if(!(d>nearest)){chosen=load<s16>(joints);nearest=d;hit=i;hitpos=offsetindex;
    if(std::fabs(d)<epsilon){call<u32>(0x02552C88,self,relative.a,rot,outpos,outrot,a.a,radius);shape=load<s16>(types);if(jnt_delete(shape))return -3;if(!jnt_throw(shape))record=call<u32>(0x025537B4,self,bestjoint.a,best.a,chosen,rot,outpos)!=0;}
   }
   offsets+=12;offsetindex++;
  }
  if(record){jnt_copy(savedpos.a,outpos);for(u32 k=0;k<6;k+=2)store<u16>(savedrot.a+k,load<u16>(outrot+k));}
  types+=2;joints+=2;radii+=4;
 }
 const s32 winner=load<s32>(bestjoint.a);
 if(winner>=0){jnt_copy(outpos,savedpos.a);for(u32 k=0;k<6;k+=2)store<u16>(outrot+k,load<u16>(savedrot.a+k));return winner;}
 types=load<u32>(self+16);radii=load<u32>(self+12);offsets=load<u32>(self+8);jnt_matrix(model,chosen);const s16 shape=load<s16>(types+hit*2);
 if(jnt_throw(shape))return -1;if(jnt_delete(shape))return -3;
 const u32 off=offsets+u32(hitpos)*12;
 if(shape==0){call<void>(0x028E8F64,0x1048D0CCu,off,a.a);call<void>(0x028E8F64,0x1048D0CCu,off+12,b.a);call<void>(0x0201ADE0,pos,temp.a,a.a);jnt_copy(relative.a,temp.a);call<u32>(0x02552E50,self,relative.a,rot,outpos,outrot,a.a,b.a,load<f32>(radii+hit*4));}
 else if(shape==1){call<void>(0x028E8F64,0x1048D0CCu,off,a.a);call<void>(0x0201ADE0,pos,temp.a,a.a);jnt_copy(relative.a,temp.a);call<u32>(0x02552C88,self,relative.a,rot,outpos,outrot,a.a,load<f32>(radii+hit*4));}
 else {call<void>(0x028E8F64,0x1048D0CCu,off,a.a);call<void>(0x028E8F64,0x1048D0CCu,off+12,b.a);call<void>(0x0201ADE0,pos,temp.a,a.a);jnt_copy(relative.a,temp.a);call<u32>(0x02553388,self,relative.a,rot,outpos,outrot,a.a,b.a,load<f32>(radii+hit*4));}
 return chosen;
}
VERIFY(0x025538E4,JntHit_Search_hd);

void JntHit_sinit_hd() {
 WWHD_FUNC(0x025541F4,void);
 store<u32>(0x10475A44,0);store<u32>(0x10475A3C,0);store<u32>(0x10475A48,0);store<u32>(0x10475A40,0);
 call<void>(0x028F026C,0x101E8C9Cu);
 const f32 a=load<f32>(0x1004EF34),b=load<f32>(0x1004EF38);
 store<f32>(0x10475A30,a);store<f32>(0x10475A34,b);
 call<void>(0x028ED6F8,0x10475A38u);call<void>(0x028F026C,0x101E8CA8u);
 call<void>(0x028EAB2C,0x10475A39u);call<void>(0x028F026C,0x101E8CB4u);
}
VERIFY(0x025541F4,JntHit_sinit_hd);
