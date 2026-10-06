#include "wwhd.h"
#include "gabi.h"
#include <cmath>
struct M3dVector { be<f32> x,y,z; };
WWHD_SIZE(M3dVector,12);
static f32 rd(u32 a) { return gabi::load<f32>(a); }
static void wr(u32 a,f32 x) { gabi::store<f32>(a,x); }
static void* ptr(u32 a) { return gabi::at<void>(a); }
void cM3d_InDivPos1(void* a,void* b,void* out,f32 scale) {
 WWHD_FUNC(0x02010924,void,a,b,out,scale);
 gabi::Local<M3dVector> tmp;
 gabi::call<void>(0x028E8E64,b,tmp.get(),scale);
 gabi::call<void>(0x028E8D88,tmp.get(),a,out);
}
VERIFY(0x02010924,cM3d_InDivPos1);
void cM3d_InDivPos2(void* a,void* b,void* out,f32 scale) {
 WWHD_FUNC(0x02010974,void,a,b,out,scale);
 gabi::Local<M3dVector> tmp;
 gabi::call<void>(0x028E8DAC,b,a,tmp.get());
 gabi::call<void>(0x02010924,a,tmp.get(),out,scale);
}
VERIFY(0x02010974,cM3d_InDivPos2);
f32 cM3d_Len2dSq(f32 ax,f32 ay,f32 bx,f32 by) {
 WWHD_FUNC(0x020109E8,f32,ax,ay,bx,by);
 f32 y=ay-by,x=ax-bx;
 return gabi::fmadds(x,x,y*y);
}
VERIFY(0x020109E8,cM3d_Len2dSq);
s32 cM3d_Len2dSqPntAndSegLine(void* ox,void* oy,void* distance,f32 px,f32 py,f32 ax,f32 ay,f32 bx,f32 by) {
 WWHD_FUNC(0x020109FC,s32,ox,oy,distance,px,py,ax,ay,bx,by);
 f32 dy=by-ay,dx=bx-ax,len=gabi::fmadds(dx,dx,dy*dy),zero=rd(0x10001F88);
 if(std::fabs(len)<rd(0x10001F8C)) {wr(gabi::ea(distance),zero);return 0;}
 f32 t=gabi::fmadds(px-ax,dx,(py-ay)*dy)/len;
 s32 within=!(t<zero)&&!(t>rd(0x10001F90));
 f32 x=gabi::fmadds(dx,t,ax),y=gabi::fmadds(dy,t,ay);
 wr(gabi::ea(ox),x);wr(gabi::ea(oy),y);
 f32 d=gabi::call<f32>(0x020109E8,rd(gabi::ea(ox)),y,px,py);
 wr(gabi::ea(distance),d);return within;
}
VERIFY(0x020109FC,cM3d_Len2dSqPntAndSegLine);
s32 cM3d_Len3dSqPntAndSegLine(void* line,void* point,void* out,void* distance) {
 WWHD_FUNC(0x02010AE4,s32,line,point,out,distance);
 gabi::Local<M3dVector> direction,relative;
 gabi::call<void>(0x028E8DAC,ptr(gabi::ea(line)+12),line,direction.get());
 f32 len=gabi::call<f32>(0x028E8F44,direction.get(),direction.get()),zero=rd(0x10001F88);
 if(std::fabs(len)<rd(0x10001F8C)) {wr(gabi::ea(distance),zero);return 0;}
 gabi::call<void>(0x028E8DAC,point,line,relative.get());
 f32 t=gabi::call<f32>(0x028E8F44,relative.get(),direction.get())/len;
 s32 within=!(t<zero)&&!(t>rd(0x10001F90));
 gabi::call<void>(0x028E8E64,direction.get(),direction.get(),t);
 gabi::call<void>(0x028E8D88,direction.get(),line,out);
 f32 d=gabi::call<f32>(0x028E8DE8,out,point);wr(gabi::ea(distance),d);return within;
}
VERIFY(0x02010AE4,cM3d_Len3dSqPntAndSegLine);
f32 cM3d_SignedLenPlaAndPos(void* plane,void* point) {
 WWHD_FUNC(0x02010C50,f32,plane,point);
 f32 len=gabi::call<f32>(0x028E8E10,plane);
 if(std::fabs(len)<rd(0x10001F8C)) return rd(0x10001F88);
 f32 dot=gabi::call<f32>(0x028E8F44,plane,point);
 return (dot+rd(gabi::ea(plane)+12))/len;
}
VERIFY(0x02010C50,cM3d_SignedLenPlaAndPos);
f32 cM3d_VectorProduct2d(f32 ax,f32 ay,f32 bx,f32 by,f32 cx,f32 cy) {
 WWHD_FUNC(0x02010CFC,f32,ax,ay,bx,by,cx,cy);
 return gabi::fmsubs(bx-ax,cy-ay,(by-ay)*(cx-ax));
}
VERIFY(0x02010CFC,cM3d_VectorProduct2d);
void cM3d_CalcPla(void* a,void* b,void* c,void* normal,void* distance) {
 WWHD_FUNC(0x02010D18,void,a,b,c,normal,distance);
 gabi::Local<M3dVector> u,v;
 gabi::call<void>(0x028E8DAC,b,a,u.get());gabi::call<void>(0x028E8DAC,c,a,v.get());
 gabi::call<void>(0x028E8D4C,u.get(),v.get(),normal);
 f32 len=gabi::call<f32>(0x028E8E10,normal);
 if(std::fabs(len)<rd(0x10001F94)) {
  f32 z=rd(0x10001F88);u32 n=gabi::ea(normal);
  wr(n+4,z);wr(gabi::ea(distance),z);wr(n,z);wr(n+8,z);
 } else {
  gabi::call<void>(0x028E8E64,normal,normal,rd(0x10001F90)/len);
  f32 dot=gabi::call<f32>(0x028E8F44,normal,a);wr(gabi::ea(distance),-dot);
 }
}
VERIFY(0x02010D18,cM3d_CalcPla);
s32 cM3d_Cross_AabCyl(void* aab,void* cylinder) {
 WWHD_FUNC(0x02010E14,s32,aab,cylinder);
 u32 a=gabi::ea(aab),c=gabi::ea(cylinder);f32 x=rd(c),r=rd(c+12);
 if(rd(a)>x+r)return 0;if(rd(a+12)<x-r)return 0;
 f32 z=rd(c+8);if(rd(a+8)>z+r)return 0;if(rd(a+20)<z-r)return 0;
 f32 y=rd(c+4),h=rd(c+16);if(rd(a+4)>y+h)return 0;
 return !(rd(a+16)<y);
}
VERIFY(0x02010E14,cM3d_Cross_AabCyl);
s32 cM3d_PlaneSegmentPosition(void* start,void* end,void* out,f32 startDistance,f32 endDistance) {
 WWHD_FUNC(0x02010E94,s32,start,end,out,startDistance,endDistance);
 f32 delta=startDistance-endDistance;
 if(std::fabs(delta)<rd(0x10001F8C)) {
  u32 e=gabi::ea(end),o=gabi::ea(out);
  gabi::store<u32>(o,gabi::load<u32>(e));gabi::store<u32>(o+4,gabi::load<u32>(e+4));gabi::store<u32>(o+8,gabi::load<u32>(e+8));return 0;
 }
 gabi::call<void>(0x02010974,start,end,out,startDistance/delta);return 1;
}
VERIFY(0x02010E94,cM3d_PlaneSegmentPosition);
s32 cM3d_Cross_LinPla(void* line,void* plane,void* out,u32 front,u32 back) {
 WWHD_FUNC(0x02010F00,s32,line,plane,out,front,back);
 u32 l=gabi::ea(line),p=gabi::ea(plane),o=gabi::ea(out);
 f32 first=gabi::call<f32>(0x028E8F44,plane,line);first+=rd(p+12);
 f32 last=gabi::call<f32>(0x028E8F44,plane,ptr(l+12));last+=rd(p+12);
 f32 zero=rd(0x10001F88);
 bool cross=false;
 if(!(first*last>zero)) {
  if(first<zero||last>zero) cross=back!=0;
  else cross=front!=0;
 }
 if(cross)return gabi::call<s32>(0x02010E94,line,ptr(l+12),out,first,last);
 gabi::store<u32>(o,gabi::load<u32>(l+12));gabi::store<u32>(o+4,gabi::load<u32>(l+16));gabi::store<u32>(o+8,gabi::load<u32>(l+20));return 0;
}
VERIFY(0x02010F00,cM3d_Cross_LinPla);
s32 cM3d_Cross_AabSph(void* aab,void* sphere) {
 WWHD_FUNC(0x020119B0,s32,aab,sphere);
 u32 a=gabi::ea(aab),s=gabi::ea(sphere);f32 r=rd(s+12),x=rd(s);
 if(rd(a)>x+r)return 0;if(rd(a+12)<x-r)return 0;
 f32 z=rd(s+8);if(rd(a+8)>z+r)return 0;if(rd(a+20)<z-r)return 0;
 f32 y=rd(s+4);if(rd(a+4)>y+r)return 0;
 return !(rd(a+16)<y-r);
}
VERIFY(0x020119B0,cM3d_Cross_AabSph);
s32 cM3d_Cross_SphPnt(void* sphere,void* point) {
 WWHD_FUNC(0x02013090,s32,sphere,point);
 f32 d=gabi::call<f32>(0x028E8DE8,sphere,point),r=rd(gabi::ea(sphere)+12);
 return !(d>r*r);
}
VERIFY(0x02013090,cM3d_Cross_SphPnt);
s32 cM3d_Cross_CylPnt(void* cylinder,void* point) {
 WWHD_FUNC(0x02014F7C,s32,cylinder,point);
 u32 c=gabi::ea(cylinder),p=gabi::ea(point);f32 dz=rd(c+8)-rd(p+8),dx=rd(c)-rd(p),r=rd(c+12),h=rd(c+16),y=rd(c+4);
 f32 sq=gabi::fmadds(dx,dx,dz*dz),top=y+h;
 if(!(sq<r*r))return 0;
 f32 py=rd(p+4);if(!(y<py))return 0;if(!(top>py))return 0;return 1;
}
VERIFY(0x02014F7C,cM3d_Cross_CylPnt);
s32 cM3d_Cross_CylPntPnt(void* cylinder,void* start,void* end,void* count,void* out) {
 WWHD_FUNC(0x02015668,s32,cylinder,start,end,count,out);
 struct Line { M3dVector start,end;be<u32> vtable; };
 gabi::Local<Line> line;line->vtable=0x10002EE8;
 gabi::call<void>(0x0201883C,line.get(),start,end);
 return gabi::call<s32>(0x02014FDC,cylinder,line.get(),count,out);
}
VERIFY(0x02015668,cM3d_Cross_CylPntPnt);
void cM3d_CalcVecAngle(void* vector,void* xAngle,void* zAngle) {
 WWHD_FUNC(0x02017264,void,vector,xAngle,zAngle);
 u32 v=gabi::ea(vector);f32 y=rd(v+4),z=rd(v+8),one=rd(0x10001F90);
 s32 x=gabi::call<s32>(0x020195B0,-(z*y),one);gabi::store<u16>(gabi::ea(xAngle),u16(0u-u32(x)));
 f32 a=rd(v),b=rd(v+4);s32 angle=gabi::call<s32>(0x020195B0,-(a*b),one);gabi::store<u16>(gabi::ea(zAngle),u16(angle));
}
VERIFY(0x02017264,cM3d_CalcVecAngle);
void cM3d_CalcVecZAngle(void* vector,void* angles) {
 WWHD_FUNC(0x02017300,void,vector,angles);
 u32 v=gabi::ea(vector),a=gabi::ea(angles);f32 z=rd(v+8),x=rd(v);
 f32 horizontal=gabi::call<f32>(0x028F4384,gabi::fmadds(x,x,z*z));
 s32 pitch=gabi::call<s32>(0x020195B0,rd(v+4),horizontal);gabi::store<u16>(a,u16(0u-u32(pitch)));
 s32 yaw=gabi::call<s32>(0x020195B0,rd(v),rd(v+8));gabi::store<u16>(a+2,u16(yaw));gabi::store<u16>(a+4,0);
}
VERIFY(0x02017300,cM3d_CalcVecZAngle);
void cM3d_PlaneCrossLineProcWork(void* first,void* second,f32 a,f32 b,f32 c,f32 d,f32 denominator,f32 f,f32 g) {
 WWHD_FUNC(0x020174C0,void,first,second,a,b,c,d,denominator,f,g);
 f32 x=gabi::fmsubs(b,g,d*f)/denominator,y=gabi::fmsubs(c,f,a*g)/denominator;
 wr(gabi::ea(first),x);wr(gabi::ea(second),y);
}
VERIFY(0x020174C0,cM3d_PlaneCrossLineProcWork);
void cM3d_CrawVec(void* normal,void* vector,void* out) {
 WWHD_FUNC(0x02017B6C,void,normal,vector,out);
 u32 n=gabi::ea(normal),v=gabi::ea(vector);f32 ny=rd(n+4),vy=rd(v+4),nx=rd(n),vx=rd(v),vz=rd(v+8),nz=rd(n+8);
 f32 dot=gabi::fmadds(vz,nz,gabi::fmadds(vx,nx,vy*ny));
 gabi::Local<M3dVector> scaled;
 gabi::call<void>(0x028E8E64,normal,scaled.get(),std::fabs(dot));gabi::call<void>(0x028E8D88,scaled.get(),vector,out);
}
VERIFY(0x02017B6C,cM3d_CrawVec);
void c_m3d_static_init() {
 WWHD_FUNC(0x02017BE0,void);
 gabi::store<u32>(0x101FF898,0);gabi::store<u32>(0x101FF890,0);gabi::store<u32>(0x101FF89C,0);gabi::store<u32>(0x101FF894,0);
 gabi::call<void>(0x028F026C,ptr(0x1018C88C));
 f32 negativePi=rd(0x100030B0),positivePi=rd(0x100030B4);wr(0x101FF884,negativePi);wr(0x101FF888,positivePi);
 gabi::call<void>(0x028ED6F8,ptr(0x101FF88C));gabi::call<void>(0x028F026C,ptr(0x1018C898));
 gabi::call<void>(0x028EAB2C,ptr(0x101FF88D));gabi::call<void>(0x028F026C,ptr(0x1018C8A4));
}
VERIFY(0x02017BE0,c_m3d_static_init);
s32 cM3d_InclusionCheckPosIn3PosBox2d(f32 ax,f32 ay,f32 bx,f32 by,f32 cx,f32 cy,f32 px,f32 py) {
 WWHD_FUNC(0x02011D2C,s32,ax,ay,bx,by,cx,cy,px,py);
 f32 margin=f32(gabi::load<f64>(gabi::cpu->r[1]+8));
 wr(gabi::cpu->r[1]+8,margin);
 f32 dx=ax-bx,lo=dx>=0?bx:ax,hi=dx>=0?ax:bx;
 if(!(lo>cx)) {if(hi<cx)hi=cx;}
 else lo=cx;
 if(lo-margin>px)return 0;if(hi+margin<px)return 0;
 f32 dy=ay-by;lo=dy>=0?by:ay;hi=dy>=0?ay:by;
 if(!(lo>cy)) {if(hi<cy)hi=cy;}
 else lo=cy;
 if(lo-margin>py)return 0;return !(hi+margin<py);
}
VERIFY(0x02011D2C,cM3d_InclusionCheckPosIn3PosBox2d);
// GHS passes the ninth floating argument as a double in the outgoing stack area.
static s32 callInclusion(f32 ax,f32 ay,f32 bx,f32 by,f32 cx,f32 cy,f32 px,f32 py) {
 gabi::ArgPack args{gabi::cpu};
 args.put(ax);args.put(ay);args.put(bx);args.put(by);args.put(cx);args.put(cy);args.put(px);args.put(py);
 f64 tolerance=rd(0x10001F9C);u64 bits;std::memcpy(&bits,&tolerance,8);
 args.ns=2;args.stk[0]=u32(bits>>32);args.stk[1]=u32(bits);
 gabi::do_call(gabi::cpu,args,0x02011D2C,0);return gabi::result<s32>(gabi::cpu);
}
// All coordinates are captured before the first guest call, as in the HD routines.
static s32 projectedTriangle(f32 ax,f32 ay,f32 bx,f32 by,f32 cx,f32 cy,f32 px,f32 py,bool frontOnly=false) {
 if(!callInclusion(ax,ay,bx,by,cx,cy,px,py))return 0;
 if(frontOnly) {
  f32 limit=rd(0x10001FA4);
  if(gabi::call<f32>(0x02010CFC,ax,ay,bx,by,px,py)<limit)return 0;
  if(gabi::call<f32>(0x02010CFC,bx,by,cx,cy,px,py)<limit)return 0;
  return !(gabi::call<f32>(0x02010CFC,cx,cy,ax,ay,px,py)<limit);
 }
 f32 first=gabi::call<f32>(0x02010CFC,ax,ay,bx,by,px,py),upper=rd(0x10001FA0);
 if(!(first>upper)) {
  f32 second=gabi::call<f32>(0x02010CFC,bx,by,cx,cy,px,py);
  if(!(second>upper)) {
   f32 third=gabi::call<f32>(0x02010CFC,cx,cy,ax,ay,px,py);
   if(!(third>upper))return 1;
  }
 }
 f32 lower=rd(0x10001FA4);
 if(first<lower)return 0;
 if(gabi::call<f32>(0x02010CFC,bx,by,cx,cy,px,py)<lower)return 0;
 return !(gabi::call<f32>(0x02010CFC,cx,cy,ax,ay,px,py)<lower);
}
s32 cM3d_CrossX_LinTri_proc(void* triangle,void* point) {
 WWHD_FUNC(0x02011DF0,s32,triangle,point);
 u32 t=gabi::ea(triangle),p=gabi::ea(point);
 if(std::fabs(rd(t))<rd(0x10001F8C))return 0;
 return projectedTriangle(rd(t+24),rd(t+28),rd(t+36),rd(t+40),rd(t+48),rd(t+52),rd(p+4),rd(p+8));
}
VERIFY(0x02011DF0,cM3d_CrossX_LinTri_proc);
s32 cM3d_CrossY_Tri(void* triangle,void* point) {
 WWHD_FUNC(0x0201204C,s32,triangle,point);
 u32 t=gabi::ea(triangle),p=gabi::ea(point);
 if(std::fabs(rd(t+4))<rd(0x10001F8C))return 0;
 return projectedTriangle(rd(t+28),rd(t+20),rd(t+40),rd(t+32),rd(t+52),rd(t+44),rd(p+8),rd(p));
}
VERIFY(0x0201204C,cM3d_CrossY_Tri);
s32 cM3d_CrossY_TriVertices(void* a,void* b,void* c,void* plane,void* point) {
 WWHD_FUNC(0x020122A8,s32,a,b,c,plane,point);
 if(std::fabs(rd(gabi::ea(plane)+4))<rd(0x10001F8C))return 0;
 u32 x=gabi::ea(a),y=gabi::ea(b),z=gabi::ea(c),p=gabi::ea(point);
 return projectedTriangle(rd(x+8),rd(x),rd(y+8),rd(y),rd(z+8),rd(z),rd(p+8),rd(p));
}
VERIFY(0x020122A8,cM3d_CrossY_TriVertices);
s32 cM3d_CrossY_Tri_Front(void* a,void* b,void* c,void* point) {
 WWHD_FUNC(0x02012504,s32,a,b,c,point);
 u32 x=gabi::ea(a),y=gabi::ea(b),z=gabi::ea(c),p=gabi::ea(point);
 return projectedTriangle(rd(x+8),rd(x),rd(y+8),rd(y),rd(z+8),rd(z),rd(p+8),rd(p),true);
}
VERIFY(0x02012504,cM3d_CrossY_Tri_Front);
s32 cM3d_CrossY_TriPosition(void* triangle,void* point,void* height) {
 WWHD_FUNC(0x020126EC,s32,triangle,point,height);
 if(!gabi::call<s32>(0x0201204C,triangle,point))return 0;
 u32 t=gabi::ea(triangle),p=gabi::ea(point);f32 product=rd(t)*rd(p);
 f32 value=gabi::fnmsubs(rd(t+8),rd(p+8),-product);
 value=(value-rd(t+12))/rd(t+4);wr(gabi::ea(height),value);return 1;
}
VERIFY(0x020126EC,cM3d_CrossY_TriPosition);
s32 cM3d_CrossY_TriRange(void* triangle,void* point,void* range,void* height) {
 WWHD_FUNC(0x02012760,s32,triangle,point,range,height);
 u32 t=gabi::ea(triangle),p=gabi::ea(point),r=gabi::ea(range);
 if(std::fabs(rd(t+4))<rd(0x10001F8C))return 0;
 gabi::Local<M3dVector> work;work->x=rd(p);work->y=rd(r);work->z=rd(p+8);
 f32 first=gabi::call<f32>(0x028E8F44,triangle,work.get());
 work->y=rd(r+4);first+=rd(t+12);
 f32 last=gabi::call<f32>(0x028E8F44,triangle,work.get()),zero=rd(0x10001F88);last+=rd(t+12);
 if(first>zero&&last>zero)return 0;if(first<zero&&last<zero)return 0;
 return gabi::call<s32>(0x020126EC,triangle,point,height);
}
VERIFY(0x02012760,cM3d_CrossY_TriRange);
s32 cM3d_CrossZ_LinTri_proc(void* triangle,void* point) {
 WWHD_FUNC(0x0201288C,s32,triangle,point);
 u32 t=gabi::ea(triangle),p=gabi::ea(point);
 if(std::fabs(rd(t+8))<rd(0x10001F8C))return 0;
 return projectedTriangle(rd(t+20),rd(t+24),rd(t+32),rd(t+36),rd(t+44),rd(t+48),rd(p),rd(p+4));
}
VERIFY(0x0201288C,cM3d_CrossZ_LinTri_proc);
s32 cM3d_Check_LinLin(void* lineA,void* lineB,void* positionA,void* positionB) {
 WWHD_FUNC(0x02011A30,s32,lineA,lineB,positionA,positionB);
 gabi::Local<M3dVector> a,b,delta;
 gabi::call<void>(0x028E8DAC,ptr(gabi::ea(lineA)+12),lineA,a.get());
 gabi::call<void>(0x028E8DAC,ptr(gabi::ea(lineB)+12),lineB,b.get());
 f32 lengthA=gabi::call<f32>(0x028E8E10,a.get()),lengthB=gabi::call<f32>(0x028E8E10,b.get()),epsilon=rd(0x10001F8C);
 if(std::fabs(lengthA)<epsilon||std::fabs(lengthB)<epsilon)return 1;
 f32 one=rd(0x10001F90),invA=one/lengthA,invB=one/lengthB;
 gabi::call<void>(0x028E8E64,a.get(),a.get(),invA);gabi::call<void>(0x028E8E64,b.get(),b.get(),invB);
 gabi::call<void>(0x028E8DAC,lineA,lineB,delta.get());
 f32 relation=-gabi::call<f32>(0x028E8F44,a.get(),b.get()),projectionA=gabi::call<f32>(0x028E8F44,delta.get(),a.get());
 gabi::call<f32>(0x028E8DD0,delta.get());
 f32 denominator=std::fabs(gabi::fnmsubs(relation,relation,one));
 f32 ta,tb;s32 result;
 if(!(std::fabs(denominator)<epsilon)) {
  f32 projectionB=-gabi::call<f32>(0x028E8F44,delta.get(),b.get()),inverse=one/denominator;
  tb=gabi::fmsubs(relation,projectionA,projectionB)*inverse;
  ta=gabi::fmsubs(relation,projectionB,projectionA)*inverse;result=3;
 } else {
  f32 zero=rd(0x10001F88);ta=-projectionA;tb=zero;
  if(ta<zero||ta>lengthA){ta=gabi::fmsubs(lengthB,relation,projectionA);tb=lengthB;}
  f32 projectionB=gabi::call<f32>(0x028E8F44,delta.get(),b.get());
  if(ta<zero||ta>lengthA){tb=projectionB;ta=zero;}
  if(tb<zero||tb>lengthB){tb=gabi::fnmsubs(lengthA,relation,projectionB);ta=lengthA;}
  result=2;
 }
 wr(gabi::ea(positionA),gabi::fmuls_ppc(ta,invA));wr(gabi::ea(positionB),gabi::fmuls_ppc(tb,invB));return result;
}
VERIFY(0x02011A30,cM3d_Check_LinLin);
static void checkFloat(f32 value,u32 file,u32 line,u32 condition) {
 u32 bits;std::memcpy(&bits,&value,4);
 if((bits<<1)>0xFF000000u)gabi::call<void>(0x0273AA24,ptr(file),line,ptr(condition));
}
static void copyVectorWords(u32 src,u32 dst) {
 gabi::store<u32>(dst,gabi::load<u32>(src));gabi::store<u32>(dst+4,gabi::load<u32>(src+4));gabi::store<u32>(dst+8,gabi::load<u32>(src+8));
}
s32 cM3d_Cross_CylSph(void* cylinder,void* sphere,void* penetration) {
 WWHD_FUNC(0x020135C8,s32,cylinder,sphere,penetration);
 u32 c=gabi::ea(cylinder),s=gabi::ea(sphere);
 checkFloat(rd(s),0x10001FB4,0x9A2,0x10001FC0);checkFloat(rd(s+4),0x10001FB4,0x9A3,0x10002018);
 checkFloat(rd(s+8),0x10001FB4,0x9A4,0x10002070);checkFloat(rd(s+12),0x10001FB4,0x9A5,0x100020C8);
 checkFloat(rd(c),0x10001FB4,0x9A7,0x10002114);checkFloat(rd(c+4),0x10001FB4,0x9A8,0x1000216C);
 checkFloat(rd(c+8),0x10001FB4,0x9A9,0x100021C4);checkFloat(rd(c+16),0x10001FB4,0x9AA,0x1000221C);
 f32 radius=rd(c+12);checkFloat(radius,0x10001FB4,0x9AB,0x10002268);
 // The failed radius check is the only path that reloads it before computing the sum.
 u32 bits;std::memcpy(&bits,&radius,4);if((bits<<1)>0xFF000000u)radius=rd(c+12);
 f32 sum=radius+rd(s+12);
 f32 d=gabi::call<f32>(0x020109E8,rd(s),rd(s+8),rd(c),rd(c+8));d=gabi::call<f32>(0x028F4384,d);
 if(sum<d)return 0;
 f32 sy=rd(s+4),sr=rd(s+12),cy=rd(c+4);if(sy+sr<cy)return 0;
 f32 height=rd(c+16);if(sy-sr>cy+height)return 0;
 f32 overlap=sum-d;wr(gabi::ea(penetration),overlap);checkFloat(overlap,0x10001FB4,0x9BC,0x100022B4);return 1;
}
VERIFY(0x020135C8,cM3d_Cross_CylSph);
s32 cM3d_Cross_SphSphDepth(void* a,void* b,void* penetration) {
 WWHD_FUNC(0x02013B78,s32,a,b,penetration);
 u32 x=gabi::ea(a),y=gabi::ea(b);
 checkFloat(rd(x),0x10002644,0xA0D,0x10002650);checkFloat(rd(x+4),0x10002644,0xA0E,0x100026A0);
 checkFloat(rd(x+8),0x10002644,0xA0F,0x100026F0);checkFloat(rd(x+12),0x10002644,0xA10,0x10002740);
 checkFloat(rd(y),0x10002644,0xA11,0x10002788);checkFloat(rd(y+4),0x10002644,0xA12,0x100027D8);
 checkFloat(rd(y+8),0x10002644,0xA13,0x10002828);checkFloat(rd(y+12),0x10002644,0xA14,0x10002878);
 gabi::Local<M3dVector> delta;gabi::call<void>(0x028E8DAC,a,b,delta.get());
 f32 d=gabi::call<f32>(0x028E8E10,delta.get()),r2=rd(y+12),r1=rd(x+12),overlap=(r1+r2)-d;
 if(overlap>rd(0x10001F8C)) {
  wr(gabi::ea(penetration),overlap);checkFloat(overlap,0x10002644,0xA1D,0x100028C0);return 1;
 }
 f32 zero=rd(0x10001F88);wr(gabi::ea(penetration),zero);checkFloat(zero,0x10002644,0xA22,0x100028C0);return 0;
}
VERIFY(0x02013B78,cM3d_Cross_SphSphDepth);
s32 cM3d_Cross_SphSphPosition(void* a,void* b,void* out) {
 WWHD_FUNC(0x02014010,s32,a,b,out);
 gabi::Local<be<f32>> distance,penetration;gabi::Local<M3dVector> delta;
 if(!gabi::call<s32>(0x02013DC0,a,b,distance.get(),penetration.get()))return 0;
 f32 d=distance->get();
 if(std::fabs(d)<rd(0x10001F8C))copyVectorWords(gabi::ea(a),gabi::ea(out));
 else {
  f32 ratio=rd(gabi::ea(b)+12)/d;gabi::call<void>(0x028E8DAC,a,b,delta.get());
  gabi::call<void>(0x028E8E64,delta.get(),delta.get(),ratio);gabi::call<void>(0x028E8D88,delta.get(),b,out);
 }
 return 1;
}
VERIFY(0x02014010,cM3d_Cross_SphSphPosition);
void cM3d_CalcSphVsTriCrossPoint(void* sphere,void* triangle,void* out) {
 WWHD_FUNC(0x02014140,void,sphere,triangle,out);
 gabi::Local<M3dVector> sum,midpoint;
 u32 t=gabi::ea(triangle);
 gabi::call<void>(0x028E8D88,ptr(t+20),ptr(t+32),sum.get());
 gabi::call<void>(0x028E8E64,sum.get(),midpoint.get(),rd(0x10002BDC));
 f32 d=gabi::call<f32>(0x028E8DE8,midpoint.get(),sphere);
 if(std::fabs(d)<rd(0x10001F8C))copyVectorWords(gabi::ea(sphere),gabi::ea(out));
 else gabi::call<void>(0x02010974,sphere,midpoint.get(),out,rd(gabi::ea(sphere)+12)/d);
}
VERIFY(0x02014140,cM3d_CalcSphVsTriCrossPoint);
s32 cM3d_Cross_CylCylPosition(void* a,void* b,void* out) {
 WWHD_FUNC(0x02014E18,s32,a,b,out);
 u32 x=gabi::ea(a),y=gabi::ea(b),o=gabi::ea(out);
 f32 dz=rd(x+8)-rd(y+8),dx=rd(x)-rd(y),r=rd(x+12)+rd(y+12),d=gabi::fmadds(dx,dx,dz*dz);
 if(d>r*r)return 0;
 f32 ay=rd(x+4),ah=rd(x+16),by=rd(y+4);if(ay+ah<by)return 0;if(ay>by+rd(y+16))return 0;
 d=gabi::call<f32>(0x028F4384,d);
 if(std::fabs(d)<rd(0x10001F8C))copyVectorWords(y,o);
 else {
  f32 bh=rd(y+16),base=rd(y+4),half=rd(0x10002BDC),radius=rd(y+12),height=gabi::fmadds(bh,half,base);
  wr(o+4,height);f32 low=rd(x+4),ratio=radius/d;
  if(height<low)wr(o+4,low);
  else {f32 top=low+rd(x+16);if(height>top)wr(o+4,top);}
  f32 ax=rd(x),bx=rd(y);wr(o,gabi::fmadds(ax-bx,ratio,bx));
  f32 bz=rd(y+8),az=rd(x+8);wr(o+8,gabi::fmadds(az-bz,ratio,bz));
 }
 return 1;
}
VERIFY(0x02014E18,cM3d_Cross_CylCylPosition);
s32 cM3d_SphereSphereMetrics(void* a,void* b,void* distance,void* penetration) {
 WWHD_FUNC(0x02013DC0,s32,a,b,distance,penetration);
 u32 x=gabi::ea(a),y=gabi::ea(b);
 checkFloat(rd(x),0x10002910,0xA35,0x1000291C);checkFloat(rd(x+4),0x10002910,0xA36,0x1000296C);
 checkFloat(rd(x+8),0x10002910,0xA37,0x100029BC);checkFloat(rd(x+12),0x10002910,0xA38,0x10002A0C);
 checkFloat(rd(y),0x10002910,0xA39,0x10002A54);checkFloat(rd(y+4),0x10002910,0xA3A,0x10002AA4);
 checkFloat(rd(y+8),0x10002910,0xA3B,0x10002AF4);checkFloat(rd(y+12),0x10002910,0xA3C,0x10002B44);
 gabi::Local<M3dVector> delta;gabi::call<void>(0x028E8DAC,a,b,delta.get());
 f32 d=gabi::call<f32>(0x028E8E10,delta.get());wr(gabi::ea(distance),d);
 f32 overlap=(rd(x+12)+rd(y+12))-d;
 if(overlap>rd(0x10001F8C)){wr(gabi::ea(penetration),overlap);checkFloat(overlap,0x10002910,0xA46,0x10002B8C);return 1;}
 f32 zero=rd(0x10001F88);wr(gabi::ea(penetration),zero);checkFloat(zero,0x10002910,0xA4B,0x10002B8C);return 0;
}
VERIFY(0x02013DC0,cM3d_SphereSphereMetrics);
s32 cM3d_CylinderSpherePosition(void* cylinder,void* sphere,void* out,void* penetration) {
 WWHD_FUNC(0x02013854,s32,cylinder,sphere,out,penetration);
 u32 c=gabi::ea(cylinder),s=gabi::ea(sphere);
 checkFloat(rd(s),0x100022FC,0x9D2,0x10002308);checkFloat(rd(s+4),0x100022FC,0x9D3,0x10002360);
 checkFloat(rd(s+8),0x100022FC,0x9D4,0x100023B8);checkFloat(rd(s+12),0x100022FC,0x9D5,0x10002410);
 checkFloat(rd(c),0x100022FC,0x9D7,0x1000245C);checkFloat(rd(c+4),0x100022FC,0x9D8,0x100024B4);
 checkFloat(rd(c+8),0x100022FC,0x9D9,0x1000250C);checkFloat(rd(c+16),0x100022FC,0x9DA,0x10002564);
 f32 radius=rd(c+12);checkFloat(radius,0x100022FC,0x9DB,0x100025B0);
 u32 bits;std::memcpy(&bits,&radius,4);if((bits<<1)>0xFF000000u)radius=rd(c+12);
 f32 sum=radius+rd(s+12),d=gabi::call<f32>(0x020109E8,rd(s),rd(s+8),rd(c),rd(c+8));d=gabi::call<f32>(0x028F4384,d);
 if(sum<d)return 0;
 f32 sy=rd(s+4),sr=rd(s+12),cy=rd(c+4);if(sy+sr<cy)return 0;if(sy-sr>cy+rd(c+16))return 0;
 f32 epsilon=rd(0x10001F8C);wr(gabi::ea(penetration),sum-d);
 bool copy=std::fabs(d)<epsilon;
 f32 ratio=0;
 if(!copy){ratio=rd(c+12)/d;copy=ratio>rd(0x10001F90);}
 if(copy)copyVectorWords(s,gabi::ea(out));
 else {
  gabi::Local<M3dVector> delta;gabi::call<void>(0x028E8DAC,sphere,cylinder,delta.get());
  gabi::call<void>(0x028E8E64,delta.get(),delta.get(),ratio);gabi::call<void>(0x028E8D88,delta.get(),cylinder,out);
 }
 checkFloat(rd(gabi::ea(penetration)),0x100022FC,0x9FA,0x100025FC);return 1;
}
VERIFY(0x02013854,cM3d_CylinderSpherePosition);
s32 cM3d_Cross_CylCylDepth(void* a,void* b,void* penetration) {
 WWHD_FUNC(0x02014AF8,s32,a,b,penetration);
 u32 x=gabi::ea(a),y=gabi::ea(b);
 checkFloat(rd(x),0x10002BE4,0xB3C,0x10002BF0);checkFloat(rd(x+4),0x10002BE4,0xB3D,0x10002C2C);
 checkFloat(rd(x+8),0x10002BE4,0xB3E,0x10002C68);checkFloat(rd(x+12),0x10002BE4,0xB3F,0x10002D58);
 checkFloat(rd(x+16),0x10002BE4,0xB40,0x10002DA8);checkFloat(rd(y),0x10002BE4,0xB42,0x10002CA4);
 checkFloat(rd(y+4),0x10002BE4,0xB43,0x10002CE0);checkFloat(rd(y+8),0x10002BE4,0xB44,0x10002D1C);
 checkFloat(rd(y+12),0x10002BE4,0xB45,0x10002DF8);checkFloat(rd(y+16),0x10002BE4,0xB46,0x10002E48);
 f32 dz=rd(x+8)-rd(y+8),r=rd(x+12)+rd(y+12),dx=rd(x)-rd(y),d=gabi::fmadds(dx,dx,dz*dz),zero=rd(0x10001F88);
 if(d>r*r){wr(gabi::ea(penetration),zero);checkFloat(zero,0x10002BE4,0xB53,0x10002E98);return 0;}
 f32 ay=rd(x+4),height=rd(x+16),by=rd(y+4);
 if(ay+height<by||ay>by+rd(y+16)){wr(gabi::ea(penetration),zero);checkFloat(zero,0x10002BE4,0xB5B,0x10002E98);return 0;}
 d=gabi::call<f32>(0x028F4384,d);f32 overlap=r-d;wr(gabi::ea(penetration),overlap);
 checkFloat(overlap,0x10002BE4,0xB62,0x10002E98);return 1;
}
VERIFY(0x02014AF8,cM3d_Cross_CylCylDepth);
s32 cM3d_Cross_LinSph_CrossPos(void* sphere,void* line,void* first,void* second) {
 WWHD_FUNC(0x02013380,s32,sphere,line,first,second);
 gabi::Local<M3dVector> direction,relative,scaled;
 u32 l=gabi::ea(line);
 gabi::call<void>(0x028E8DAC,ptr(l+12),line,direction.get());gabi::call<void>(0x028E8DAC,line,sphere,relative.get());
 f32 a=gabi::call<f32>(0x028E8F44,direction.get(),direction.get());
 f32 b=gabi::call<f32>(0x028E8F44,direction.get(),relative.get());b=b+b;
 f32 c=gabi::call<f32>(0x028E8F44,relative.get(),relative.get()),epsilon=rd(0x10001F8C),r=rd(gabi::ea(sphere)+12);
 c=gabi::fnmsubs(r,r,c);
 auto position=[&](f32 t,void* out){gabi::call<void>(0x028E8E64,direction.get(),scaled.get(),t);gabi::call<void>(0x028E8D88,scaled.get(),line,out);};
 if(std::fabs(a)<epsilon){if(std::fabs(b)<epsilon)return 0;position(-(c/b),first);return 1;}
 f32 four=rd(0x10001FB0),discriminant=gabi::fmsubs(b,b,(four*a)*c);
 if(std::fabs(discriminant)<epsilon){position(-(b/(a+a)),first);return 1;}
 if(discriminant<rd(0x10001F88))return 0;
 f32 inverse=rd(0x10001F90)/(a+a),root1=gabi::call<f32>(0x028F4384,discriminant),t1=(root1-b)*inverse;
 f32 negativeB=-b,root2=gabi::call<f32>(0x028F4384,discriminant),t2=(negativeB-root2)*inverse;
 position(t1,first);position(t2,second);return 2;
}
VERIFY(0x02013380,cM3d_Cross_LinSph_CrossPos);
struct M3dLine {M3dVector start,end;be<u32> vtable;};
WWHD_SIZE(M3dLine,28);
s32 cM3d_Cross_LinSph(void* line,void* sphere,void* out) {
 WWHD_FUNC(0x020130D4,s32,line,sphere,out);
 u32 l=gabi::ea(line),s=gabi::ea(sphere);f32 radius=rd(s+12);
 for(u32 offset: {0u,4u,8u}) {
  f32 center=rd(s+offset),start=rd(l+offset),high=center+radius;
  if(high<start&&high<rd(l+12+offset))return 0;
  f32 low=center-radius;if(low>start&&low>rd(l+12+offset))return 0;
 }
 gabi::Local<M3dVector> direction,relative,scaled;
 gabi::call<void>(0x028E8DAC,ptr(l+12),line,direction.get());
 f32 length=gabi::call<f32>(0x028E8DD0,direction.get());if(std::fabs(length)<rd(0x10001F8C))return 0;
 gabi::call<void>(0x028E8DAC,sphere,line,relative.get());
 f32 t=gabi::call<f32>(0x028E8F44,relative.get(),direction.get())/length;
 f32 zero=rd(0x10001F88);
 if(t<zero||t>rd(0x10001F90)) {
  f32 a=gabi::call<f32>(0x028E8DE8,line,sphere),b=gabi::call<f32>(0x028E8DE8,ptr(l+12),sphere);
  u32 chosen=a<b?l:l+12;if(!gabi::call<s32>(0x02013090,sphere,ptr(chosen)))return 0;
  copyVectorWords(chosen,gabi::ea(out));return 1;
 }
 gabi::call<void>(0x028E8E64,direction.get(),scaled.get(),t);gabi::call<void>(0x028E8D88,scaled.get(),line,out);
 f32 d=gabi::call<f32>(0x028E8DE8,out,sphere),r=rd(s+12);return !(d>r*r);
}
VERIFY(0x020130D4,cM3d_Cross_LinSph);
s32 cM3d_NearPos_Cps(void* capsule,void* point,void* out) {
 WWHD_FUNC(0x020170F4,s32,capsule,point,out);
 gabi::Local<be<f32>> distance;gabi::Local<M3dVector> nearest,delta;
 u32 c=gabi::ea(capsule);
 if(gabi::call<s32>(0x02010AE4,capsule,point,nearest.get(),distance.get())) {
  f32 d=gabi::call<f32>(0x028F4384,distance->get());*distance=d;
  gabi::call<void>(0x028E8DAC,point,nearest.get(),delta.get());
  f32 scale=rd(c+28)/distance->get();gabi::call<void>(0x028E8E64,delta.get(),delta.get(),scale);
  gabi::call<void>(0x028E8D88,nearest.get(),delta.get(),out);
 } else {
  f32 first=gabi::call<f32>(0x028E8DE8,capsule,point),last=gabi::call<f32>(0x028E8DE8,ptr(c+12),point);
  u32 chosen=first<last?c:c+12;
  f32 d=gabi::call<f32>(0x028F4384,first<last?first:last);
  gabi::call<void>(0x028E8DAC,ptr(chosen),point,delta.get());
  gabi::call<void>(0x028E8E64,delta.get(),delta.get(),rd(c+28)/d);gabi::call<void>(0x028E8D88,ptr(chosen),delta.get(),out);
 }
 return 1;
}
VERIFY(0x020170F4,cM3d_NearPos_Cps);
s32 cM3d_3PlaneCrossPos(void* a,void* b,void* c,void* out) {
 WWHD_FUNC(0x020176D4,s32,a,b,c,out);
 gabi::Local<M3dLine> line;line->vtable=0x10002EE8;
 if(!gabi::call<s32>(0x020174E4,a,b,line.get()))return 0;
 u32 l=gabi::ea(line.get()),plane=gabi::ea(c);
 f32 start=gabi::call<f32>(0x028E8F44,c,ptr(l));start+=rd(plane+12);
 f32 end=gabi::call<f32>(0x028E8F44,c,ptr(l+12));end+=rd(plane+12);
 return gabi::call<s32>(0x02010E94,ptr(l),ptr(l+12),out,start,end)!=0;
}
VERIFY(0x020176D4,cM3d_3PlaneCrossPos);
s32 cM3d_2PlaneLinePosNearPos(void* a,void* b,void* point,void* out) {
 WWHD_FUNC(0x02017AEC,s32,a,b,point,out);
 gabi::Local<M3dLine> line;line->vtable=0x10002EE8;
 if(!gabi::call<s32>(0x020174E4,a,b,line.get()))return 0;
 gabi::call<void>(0x020177AC,line.get(),point,out);return 1;
}
VERIFY(0x02017AEC,cM3d_2PlaneLinePosNearPos);
static f32 linePointProjection(void* start,void* end,void* point,void* out,u32 file,u32 firstLine,u32 condition) {
 gabi::Local<M3dVector> direction,relative,scaled;
 gabi::call<void>(0x028E8DAC,end,start,direction.get());f32 length=gabi::call<f32>(0x028E8DD0,direction.get());
 if(std::fabs(length)<rd(0x10001F8C)){copyVectorWords(gabi::ea(point),gabi::ea(out));return rd(0x10001F88);}
 gabi::call<void>(0x028E8DAC,point,start,relative.get());f32 t=gabi::call<f32>(0x028E8F44,relative.get(),direction.get())/length;
 gabi::call<void>(0x028E8E64,direction.get(),scaled.get(),t);gabi::call<void>(0x028E8D88,scaled.get(),start,out);
 u32 o=gabi::ea(out);checkFloat(rd(o),file,firstLine,condition);checkFloat(rd(o+4),file,firstLine+1,condition+0x44);checkFloat(rd(o+8),file,firstLine+2,condition+0x88);return t;
}
f32 cM3d_lineVsPosSuisenCross(void* line,void* point,void* out) {
 WWHD_FUNC(0x020177AC,f32,line,point,out);
 return linePointProjection(line,ptr(gabi::ea(line)+12),point,out,0x10002EFC,0x1098,0x10002F08);
}
VERIFY(0x020177AC,cM3d_lineVsPosSuisenCross);
f32 cM3d_lineVsPosSuisenCrossVertices(void* start,void* end,void* point,void* out) {
 WWHD_FUNC(0x0201794C,f32,start,end,point,out);
 return linePointProjection(start,end,point,out,0x10002FD4,0x10CB,0x10002FE0);
}
VERIFY(0x0201794C,cM3d_lineVsPosSuisenCrossVertices);
s32 cM3d_2PlaneCrossLine(void* a,void* b,void* line) {
 WWHD_FUNC(0x020174E4,s32,a,b,line);
 gabi::Local<M3dVector> direction;gabi::call<void>(0x028E8D4C,a,b,direction.get());
 u32 d=gabi::ea(direction.get()),x=gabi::ea(a),y=gabi::ea(b),o=gabi::ea(line);
 f32 dx=rd(d),dy=rd(d+4),dz=rd(d+8),ax=std::fabs(dx),ay=std::fabs(dy),az=std::fabs(dz),epsilon=rd(0x10001F8C);
 if(ax<epsilon&&ay<epsilon&&az<epsilon)return 0;
 if(!(ax<ay)&&!(ax<az)) {
  gabi::call<void>(0x020174C0,ptr(o+4),ptr(o+8),rd(x+4),rd(x+8),rd(y+4),rd(y+8),dx,rd(x+12),rd(y+12));wr(o,rd(0x10001F88));
 } else if(!(ay<ax)&&!(ay<az)) {
  gabi::call<void>(0x020174C0,ptr(o+8),ptr(o),rd(x+8),rd(x),rd(y+8),rd(y),dy,rd(x+12),rd(y+12));wr(o+4,rd(0x10001F88));
 } else {
  gabi::call<void>(0x020174C0,ptr(o),ptr(o+4),rd(x),rd(x+4),rd(y),rd(y+4),dz,rd(x+12),rd(y+12));wr(o+8,rd(0x10001F88));
 }
 f32 scale=gabi::call<f32>(0x028E8E10,line);if(std::fabs(scale)<epsilon)scale=rd(0x10001F90);
 gabi::call<void>(0x028E8E64,direction.get(),direction.get(),scale);gabi::call<void>(0x028E8D88,line,direction.get(),ptr(o+12));return 1;
}
VERIFY(0x020174E4,cM3d_2PlaneCrossLine);
s32 cM3d_UpMtx_Base(void* up,void* vector,void* matrix) {
 WWHD_FUNC(0x02017374,s32,up,vector,matrix);
 f32 length=gabi::call<f32>(0x028E8E10,vector),epsilon=rd(0x10001F8C);
 if(std::fabs(length)<epsilon){gabi::call<void>(0x028E9098,matrix);return 0;}
 gabi::Local<M3dVector> normalized,axis;
 gabi::call<void>(0x028E8EF0,vector,normalized.get());gabi::call<void>(0x028E8D4C,up,normalized.get(),axis.get());
 f32 axisLength=gabi::call<f32>(0x028E8E10,axis.get()),one=rd(0x10001F90);
 if(std::fabs(axisLength)<epsilon){f32 zero=rd(0x10001F88);axis->x=one;axis->y=zero;axis->z=zero;}
 f32 dot=gabi::call<f32>(0x028E8F44,up,normalized.get());
 if(dot>one)dot=one;else {f32 negative=rd(0x10001F98);if(dot<negative)dot=negative;}
 f64 angle=gabi::call<f64>(0x028F4FAC,f64(dot));gabi::call<void>(0x028E954C,matrix,axis.get(),angle);return 1;
}
VERIFY(0x02017374,cM3d_UpMtx_Base);
void cM3d_Cross_CpsSph_CrossPos(void* capsule,void* sphere,void* nearest,void* out) {
 WWHD_FUNC(0x0201671C,void,capsule,sphere,nearest,out);
 gabi::Local<M3dVector> first,second,delta;
 s32 count=gabi::call<s32>(0x02013380,sphere,capsule,first.get(),second.get());
 if(count==1||count==2) {
  u32 selected=gabi::ea(first.get());
  if(count==2){f32 a=gabi::call<f32>(0x028E8DE8,first.get(),capsule),b=gabi::call<f32>(0x028E8DE8,second.get(),capsule);if(!(a<b))selected=gabi::ea(second.get());}
  u32 y=gabi::load<u32>(selected+4),x=gabi::load<u32>(selected),z=gabi::load<u32>(selected+8),o=gabi::ea(out);
  gabi::store<u32>(o,x);gabi::store<u32>(o+8,z);gabi::store<u32>(o+4,y);return;
 }
 u32 n=gabi::ea(nearest),o=gabi::ea(out);
 u32 x=gabi::load<u32>(n);f32 sum=rd(gabi::ea(capsule)+28)+rd(gabi::ea(sphere)+12);gabi::store<u32>(o,x);
 f32 epsilon=rd(0x10001F8C);gabi::store<u32>(o+4,gabi::load<u32>(n+4));gabi::store<u32>(o+8,gabi::load<u32>(n+8));
 if(std::fabs(sum)<epsilon)copyVectorWords(n,o);
 gabi::call<void>(0x028E8DAC,nearest,sphere,delta.get());gabi::call<void>(0x028E8E64,delta.get(),delta.get(),rd(0x10002BDC));gabi::call<void>(0x028E8D88,out,delta.get(),out);
}
VERIFY(0x0201671C,cM3d_Cross_CpsSph_CrossPos);
s32 cM3d_Cross_CpsSph(void* capsule,void* sphere,void* out) {
 WWHD_FUNC(0x020168D4,s32,capsule,sphere,out);
 u32 c=gabi::ea(capsule),s=gabi::ea(sphere);
 f32 d=gabi::call<f32>(0x028E8E80,capsule,sphere);
 if(d<rd(c+28)+rd(s+12)){gabi::call<void>(0x0201671C,capsule,sphere,capsule,out);return 1;}
 d=gabi::call<f32>(0x028E8E80,ptr(c+12),sphere);
 if(d<rd(c+28)+rd(s+12)){gabi::call<void>(0x0201671C,capsule,sphere,ptr(c+12),out);return 1;}
 gabi::Local<be<f32>> distance;gabi::Local<M3dVector> nearest;
 *distance=d;
 if(!gabi::call<s32>(0x02010AE4,capsule,sphere,nearest.get(),distance.get()))return 0;
 d=gabi::call<f32>(0x028F4384,distance->get());if(!(d<rd(c+28)+rd(s+12)))return 0;
 gabi::call<void>(0x0201671C,capsule,sphere,nearest.get(),out);return 1;
}
VERIFY(0x020168D4,cM3d_Cross_CpsSph);
s32 cM3d_Cross_TriTri(void* a,void* b,void* out) {
 WWHD_FUNC(0x02016A20,s32,a,b,out);
 u32 x=gabi::ea(a),y=gabi::ea(b);
 f32 d1=gabi::call<f32>(0x028E8F44,a,ptr(y+20));d1+=rd(x+12);
 f32 d2=gabi::call<f32>(0x028E8F44,a,ptr(y+32));d2+=rd(x+12);
 f32 d3=gabi::call<f32>(0x028E8F44,a,ptr(y+44)),zero=rd(0x10001F88);d3+=rd(x+12);
 if((d1>zero&&d2>zero&&d3>zero)||(d1<zero&&d2<zero&&d3<zero))return 0;
 d1=gabi::call<f32>(0x028E8F44,b,ptr(x+20));d1+=rd(y+12);
 d2=gabi::call<f32>(0x028E8F44,b,ptr(x+32));d2+=rd(y+12);
 d3=gabi::call<f32>(0x028E8F44,b,ptr(x+44));d3+=rd(y+12);
 if((d1>zero&&d2>zero&&d3>zero)||(d1<zero&&d2<zero&&d3<zero))return 0;
 gabi::Local<M3dLine> line;line->vtable=0x10002EE8;
 // HD repeats the first edge for the third test, in both directions.
 for(u32 side=0;side<2;side++) {
  u32 triangle=side?x:y;void* other=side?b:a;
  for(u32 edge=0;edge<3;edge++) {
   u32 start=triangle+(edge==1?32:20),end=triangle+(edge==1?44:32);
   gabi::call<void>(0x0201883C,line.get(),ptr(start),ptr(end));
   if(gabi::call<s32>(0x02012AE8,line.get(),other,out,0u,0u))return 1;
  }
 }
 return 0;
}
VERIFY(0x02016A20,cM3d_Cross_TriTri);
s32 cM3d_Cross_CpsCps(void* a,void* b,void* out) {
 WWHD_FUNC(0x020159EC,s32,a,b,out);
 gabi::Local<be<f32>> ta,tb;gabi::Local<M3dVector> pa,pb;
 s32 relation=gabi::call<s32>(0x02011A30,a,b,ta.get(),tb.get());if(relation==1)return 0;
 f32 half=rd(0x10002BDC),zero=rd(0x10001F88),one=rd(0x10001F90);
 u32 x=gabi::ea(a),y=gabi::ea(b);
 if(relation==2) {
  f32 t=ta->get();if(!(t>zero)||!(t<one))return 0;
  f32 u=tb->get();if(!(u>zero)||!(u<one))return 0;
  gabi::call<void>(0x0201888C,a,pa.get(),t);gabi::call<void>(0x0201888C,b,pb.get(),tb->get());
 } else if(relation==3) {
  f32 t=ta->get();
  if(t<zero)copyVectorWords(x,gabi::ea(pa.get()));else if(t>one)copyVectorWords(x+12,gabi::ea(pa.get()));else gabi::call<void>(0x0201888C,a,pa.get(),t);
  f32 u=tb->get();
  if(u<zero)copyVectorWords(y,gabi::ea(pb.get()));else if(u>one)copyVectorWords(y+12,gabi::ea(pb.get()));else gabi::call<void>(0x0201888C,b,pb.get(),u);
 } else return 0;
 f32 distance=gabi::call<f32>(0x028E8E80,pa.get(),pb.get());if(!(distance<rd(x+28)+rd(y+28)))return 0;
 gabi::call<void>(0x028E8D88,pa.get(),pb.get(),out);gabi::call<void>(0x028E8E64,out,out,half);return 1;
}
VERIFY(0x020159EC,cM3d_Cross_CpsCps);
s32 cM3d_Cross_CylTri(void* cylinder,void* triangle,void* out) {
 WWHD_FUNC(0x020156D0,s32,cylinder,triangle,out);
 u32 c=gabi::ea(cylinder),t=gabi::ea(triangle),o=gabi::ea(out);
 f32 firstY=rd(t+24),base=rd(c+4),top=base+rd(c+16);
 if(base>firstY&&base>rd(t+36)&&base>rd(t+48))return 0;
 if(top<firstY&&top<rd(t+36)&&top<rd(t+48))return 0;
 f32 infinity=rd(0x10002EF8),best=infinity;
 gabi::Local<M3dVector> first,second;
 for(u32 edge=0;edge<3;edge++) {
  u32 a=t+(edge==1?44:20),b=t+(edge==2?44:32);
  if(gabi::call<s32>(0x02015668,cylinder,ptr(a),ptr(b),first.get(),second.get())) {
   f32 d=gabi::call<f32>(0x028E8DE8,first.get(),ptr(a));
   if(edge==0||best>d) {
    u32 p=gabi::ea(first.get()),y=gabi::load<u32>(p+4),x=gabi::load<u32>(p),z=gabi::load<u32>(p+8);
    gabi::store<u32>(o,x);gabi::store<u32>(o+8,z);gabi::store<u32>(o+4,y);best=d;
   }
  }
 }
 if(best!=infinity)return 1;
 struct Range {be<f32> min,max;};gabi::Local<Range> range;gabi::Local<be<f32>> height;
 range->min=rd(c+4);range->max=top;
 if(!gabi::call<s32>(0x02012760,triangle,cylinder,range.get(),height.get()))return 0;
 gabi::Local<M3dVector> center,sum,midpoint,delta;
 u32 a=gabi::ea(center.get());
 gabi::store<u32>(a,gabi::load<u32>(c));gabi::store<u32>(a+4,gabi::load<u32>(c+4));
 f32 y=height->get();u32 z=gabi::load<u32>(c+8);wr(a+4,y);gabi::store<u32>(a+8,z);
 gabi::call<void>(0x028E8D88,ptr(t+20),ptr(t+32),sum.get());gabi::call<void>(0x028E8E64,sum.get(),midpoint.get(),rd(0x10002BDC));
 gabi::call<void>(0x028E8DAC,midpoint.get(),center.get(),delta.get());
 u32 d=gabi::ea(delta.get());f32 dz=rd(d+8),dx=rd(d),distance=gabi::call<f32>(0x028F4384,gabi::fmadds(dx,dx,dz*dz));
 if(std::fabs(distance)<rd(0x10001F8C)) {
  u32 m=gabi::ea(midpoint.get());f32 yy=rd(m+4),xx=rd(m);wr(o+4,yy);f32 zz=rd(m+8);wr(o,xx);wr(o+8,zz);
 } else gabi::call<void>(0x02010924,center.get(),delta.get(),out,rd(c+12)/distance);
 return 1;
}
VERIFY(0x020156D0,cM3d_Cross_CylTri);
static s32 projectedTriangleCached(f32 ax,f32 ay,f32 bx,f32 by,f32 cx,f32 cy,f32 px,f32 py,f32 tolerance,f32 upper,f32 lower) {
 gabi::ArgPack args{gabi::cpu};args.put(ax);args.put(ay);args.put(bx);args.put(by);args.put(cx);args.put(cy);args.put(px);args.put(py);
 f64 margin=tolerance;u64 bits;std::memcpy(&bits,&margin,8);args.ns=2;args.stk[0]=u32(bits>>32);args.stk[1]=u32(bits);
 gabi::do_call(gabi::cpu,args,0x02011D2C,0);if(!gabi::result<s32>(gabi::cpu))return 0;
 f32 first=gabi::call<f32>(0x02010CFC,ax,ay,bx,by,px,py);
 if(!(first>upper)) {
  if(!(gabi::call<f32>(0x02010CFC,bx,by,cx,cy,px,py)>upper)) {
   if(!(gabi::call<f32>(0x02010CFC,cx,cy,ax,ay,px,py)>upper))return 1;
  }
 }
 if(first<lower)return 0;if(gabi::call<f32>(0x02010CFC,bx,by,cx,cy,px,py)<lower)return 0;
 return !(gabi::call<f32>(0x02010CFC,cx,cy,ax,ay,px,py)<lower);
}
s32 cM3d_Cross_LinTri(void* line,void* triangle,void* out,u32 front,u32 back) {
 WWHD_FUNC(0x02012AE8,s32,line,triangle,out,front,back);
 if(!gabi::call<s32>(0x02010F00,line,triangle,out,front,back))return 0;
 u32 t=gabi::ea(triangle),p=gabi::ea(out);
 f32 nx=rd(t),threshold=rd(0x10001FA8),margin=rd(0x10001F9C),lower=rd(0x10001FA4),upper=rd(0x10001FA0);
 if(!(std::fabs(nx)<threshold)&&!projectedTriangleCached(rd(t+24),rd(t+28),rd(t+36),rd(t+40),rd(t+48),rd(t+52),rd(p+4),rd(p+8),margin,upper,lower))return 0;
 if(!(std::fabs(rd(t+4))<threshold)&&!projectedTriangleCached(rd(t+28),rd(t+20),rd(t+40),rd(t+32),rd(t+52),rd(t+44),rd(p+8),rd(p),margin,upper,lower))return 0;
 if(!(std::fabs(rd(t+8))<threshold)&&!projectedTriangleCached(rd(t+20),rd(t+24),rd(t+32),rd(t+36),rd(t+44),rd(t+48),rd(p),rd(p+4),margin,upper,lower))return 0;
 return 1;
}
VERIFY(0x02012AE8,cM3d_Cross_LinTri);
struct M3dSphere {M3dVector center;be<f32> radius;be<u32> vtable;};
WWHD_SIZE(M3dSphere,20);
s32 cM3d_Cross_CpsTri(void* capsule,void* triangle,void* out) {
 WWHD_FUNC(0x02016CE8,s32,capsule,triangle,out);
 u32 c=gabi::ea(capsule),t=gabi::ea(triangle);
 gabi::Local<M3dSphere> sphere;gabi::call<void*>(0x02018C40,sphere.get());
 gabi::call<void>(0x02018D40,sphere.get(),capsule);gabi::call<void>(0x02018C8C,sphere.get(),rd(c+28));
 if(gabi::call<s32>(0x02014200,sphere.get(),triangle,out))return 1;
 gabi::call<void>(0x02018D40,sphere.get(),ptr(c+12));gabi::call<void>(0x02018C8C,sphere.get(),rd(c+28));
 if(gabi::call<s32>(0x02014200,sphere.get(),triangle,out))return 1;
 if(!gabi::call<s32>(0x02010F00,capsule,triangle,out,1u,1u))return 0;
 f32 epsilon=rd(0x10001F8C);bool projected=true;
 if(!(std::fabs(rd(t))<epsilon)&&!gabi::call<s32>(0x02011DF0,triangle,out))projected=false;
 if(projected&&!(std::fabs(rd(t+4))<epsilon)&&!gabi::call<s32>(0x0201204C,triangle,out))projected=false;
 if(projected&&!(std::fabs(rd(t+8))<epsilon)&&!gabi::call<s32>(0x0201288C,triangle,out))projected=false;
 if(projected)return 1;
 gabi::Local<M3dLine> line;line->vtable=0x10002EE8;
 gabi::Local<be<f32>> capsuleT,edgeT;gabi::Local<M3dVector> onCapsule,onEdge;
 f32 zero=0,one=0,half=0;
 for(u32 edge=0;edge<3;edge++) {
  u32 start=t+(edge==0?20:edge==1?32:44),end=t+(edge==0?32:edge==1?44:20);
  gabi::call<void>(0x0201883C,line.get(),ptr(start),ptr(end));
  s32 relation=gabi::call<s32>(0x02011A30,capsule,line.get(),capsuleT.get(),edgeT.get());
  if(edge==0){zero=rd(0x10001F88);one=rd(0x10001F90);half=rd(0x10002BDC);}
  if(relation<2)continue;
  f32 a=capsuleT->get();if(!(a>zero)||!(a<one))continue;
  f32 b=edgeT->get();if(!(b>zero)||!(b<one))continue;
  gabi::call<void>(0x0201888C,capsule,onCapsule.get(),a);gabi::call<void>(0x0201888C,line.get(),onEdge.get(),b);
  gabi::call<void>(0x028E8D88,onCapsule.get(),onEdge.get(),out);gabi::call<void>(0x028E8E64,out,out,half);
  f32 distance=gabi::call<f32>(0x028E8E80,onCapsule.get(),onEdge.get());if(distance<rd(c+28))return 1;
 }
 return 0;
}
VERIFY(0x02016CE8,cM3d_Cross_CpsTri);
s32 cM3d_Cross_CylLin(void* cylinder,void* line,void* firstOut,void* secondOut) {
 WWHD_FUNC(0x02014FDC,s32,cylinder,line,firstOut,secondOut);
 u32 c=gabi::ea(cylinder),l=gabi::ea(line);f32 zero=rd(0x10001F88),secondT=zero;
 if(gabi::call<s32>(0x02014F7C,cylinder,line)&&gabi::call<s32>(0x02014F7C,cylinder,ptr(l+12))) {
  copyVectorWords(l,gabi::ea(firstOut));copyVectorWords(l+12,gabi::ea(secondOut));return 2;
 }
 gabi::Local<M3dVector> relativeStart,relativeEnd,direction;
 gabi::Local<M3dVector[4]> candidates;
 gabi::call<void>(0x028E8DAC,line,cylinder,relativeStart.get());gabi::call<void>(0x028E8DAC,ptr(l+12),cylinder,relativeEnd.get());
 gabi::call<void>(0x028E8DAC,relativeEnd.get(),relativeStart.get(),direction.get());
 u32 a=gabi::ea(relativeStart.get()),d=gabi::ea(direction.get()),points=gabi::ea(candidates.get());
 f32 dy=rd(d+4),epsilon=rd(0x10001F8C),radius=rd(c+12),radiusSq=radius*radius,one=rd(0x10001F90);
 f32 dz,az,dx,ax,startY=0;u32 mask=0;
 if(!(std::fabs(dy)<epsilon)) {
  startY=rd(a+4);f32 t=-(startY/dy);
  if(!(t<zero)&&!(t>one)) {
   dz=rd(d+8);az=rd(a+8);dx=rd(d);ax=rd(a);
   f32 z=gabi::fmadds(dz,t,az),x=gabi::fmadds(dx,t,ax);
   if(gabi::fmadds(x,x,z*z)<radiusSq){wr(points,x+rd(c));wr(points+4,rd(c+4));wr(points+8,z+rd(c+8));mask=1;}
  }
  f32 height=rd(c+16);t=(height-startY)/dy;
  dz=rd(d+8);ax=rd(a);az=rd(a+8);dx=rd(d);
  if(!(t<zero)&&!(t>one)) {
   f32 z=gabi::fmadds(dz,t,az),x=gabi::fmadds(dx,t,ax);
   if(gabi::fmadds(x,x,z*z)<radiusSq){wr(points+12,x+rd(c));wr(points+16,rd(c+4)+height);wr(points+20,z+rd(c+8));mask|=2;}
  }
 } else {az=rd(a+8);dz=rd(d+8);dx=rd(d);ax=rd(a);}
 f32 qa=gabi::fmadds(dx,dx,dz*dz),twiceA=qa+qa,qb0=gabi::fmadds(dx,ax,dz*az),qb=qb0+qb0,qc=gabi::fmadds(ax,ax,az*az)-radiusSq;
 f32 firstT;bool firstValid=true,secondValid=false;
 if(!(std::fabs(twiceA)<epsilon)) {
  f32 discriminant=gabi::fmsubs(qb,qb,(rd(0x10001FB0)*qa)*qc);if(discriminant<zero)return 0;
  secondValid=discriminant>zero;
  f32 root=gabi::call<f32>(0x028F4384,discriminant);firstT=(root-qb)/twiceA;
  if(secondValid)secondT=(-qb-root)/twiceA;
 } else {if(std::fabs(qb)<epsilon)return 0;firstT=-(qc/qb);}
 if(!secondValid){if(zero>firstT||firstT>one)return 0;}
 else {
  bool outsideFirst=zero>firstT||firstT>one,outsideSecond=zero>secondT||secondT>one;
  if(outsideFirst&&outsideSecond)return 0;if(outsideFirst)firstValid=false;if(outsideSecond)secondValid=false;
 }
 if(firstValid){f32 y=gabi::fmadds(firstT,rd(d+4),rd(a+4));if(y<zero||y>rd(c+16))firstValid=false;}
 if(secondValid){f32 y=gabi::fmadds(secondT,rd(d+4),rd(a+4));if(y<zero||y>rd(c+16))secondValid=false;}
 if(!firstValid&&!secondValid)return 0;
 gabi::Local<M3dVector> base,scaled,relative;
 if(firstValid&&secondValid) {
  gabi::call<void>(0x028E8D88,relativeStart.get(),cylinder,base.get());mask|=12;
  gabi::call<void>(0x028E8E64,direction.get(),scaled.get(),firstT);gabi::call<void>(0x028E8D88,scaled.get(),base.get(),ptr(points+24));
  gabi::call<void>(0x028E8E64,direction.get(),scaled.get(),secondT);gabi::call<void>(0x028E8D88,scaled.get(),base.get(),ptr(points+36));
 } else {
  mask|=4;gabi::call<void>(0x028E8E64,direction.get(),scaled.get(),firstValid?firstT:secondT);
  gabi::call<void>(0x028E8D88,scaled.get(),relativeStart.get(),relative.get());gabi::call<void>(0x028E8D88,relative.get(),cylinder,ptr(points+24));
 }
 s32 count=0;
 for(u32 i=0;i<4;i++)if(mask&(1u<<i)) {
  u32 p=points+i*12;
  if(count==0) {
   u32 z=gabi::load<u32>(p+8),x=gabi::load<u32>(p),y=gabi::load<u32>(p+4),o=gabi::ea(firstOut);
   gabi::store<u32>(o,x);gabi::store<u32>(o+4,y);gabi::store<u32>(o+8,z);count++;
  } else if(count==1) {
   f32 previous=gabi::call<f32>(0x028E8DE8,line,firstOut),next=gabi::call<f32>(0x028E8DE8,line,ptr(p));
   if(previous<next)copyVectorWords(p,gabi::ea(secondOut));
   else {copyVectorWords(gabi::ea(firstOut),gabi::ea(secondOut));copyVectorWords(p,gabi::ea(firstOut));}
   break;
  } else count++;
 }
 return count;
}
VERIFY(0x02014FDC,cM3d_Cross_CylLin);
s32 cM3d_Cross_CpsCyl(void* capsule,void* cylinder,void* out) {
 WWHD_FUNC(0x02015CF0,s32,capsule,cylinder,out);
 u32 c=gabi::ea(capsule),y=gabi::ea(cylinder);gabi::Local<M3dLine> line;line->vtable=0x10002EE8;
 if(gabi::call<s32>(0x02014F7C,cylinder,ptr(c+12))){copyVectorWords(c+12,gabi::ea(out));return 1;}
 if(gabi::call<s32>(0x02014F7C,cylinder,capsule)){copyVectorWords(c,gabi::ea(out));return 1;}
 u32 l=gabi::ea(line.get());f32 base=rd(y+4),height=rd(y+16),x=rd(y),z=rd(y+8),top=base+height;
 wr(l+4,base);wr(l+12,x);wr(l+20,z);wr(l+16,top);wr(l+8,z);wr(l,x);
 gabi::Local<M3dVector> nearest,pa,pb;gabi::Local<be<f32>> distance,ta,tb;
 s32 onSegment=gabi::call<s32>(0x02010AE4,capsule,ptr(l+12),nearest.get(),distance.get());f32 half=rd(0x10002BDC);
 if(onSegment) {
  f32 d=gabi::call<f32>(0x028E8E80,ptr(l+12),nearest.get());
  if(d<rd(c+28)){gabi::call<void>(0x028E8D88,ptr(l+12),nearest.get(),out);gabi::call<void>(0x028E8E64,out,out,half);copyVectorWords(l+12,gabi::ea(out));return 1;}
 }
 if(gabi::call<s32>(0x02010AE4,capsule,ptr(l),nearest.get(),distance.get())) {
  f32 d=gabi::call<f32>(0x028E8E80,ptr(l),nearest.get());
  if(d<rd(c+28)){gabi::call<void>(0x028E8D88,ptr(l),nearest.get(),out);gabi::call<void>(0x028E8E64,out,out,half);return 1;}
 }
 s32 relation=gabi::call<s32>(0x02011A30,capsule,line.get(),ta.get(),tb.get());
 if(relation==1) {
  gabi::Local<M3dSphere> sphere;gabi::Local<be<f32>> overlap;
  gabi::call<void*>(0x02018C40,sphere.get());gabi::call<void>(0x02018C8C,sphere.get(),rd(c+28));gabi::call<void>(0x02018D40,sphere.get(),capsule);
  return gabi::call<s32>(0x02013854,cylinder,sphere.get(),out,overlap.get());
 }
 f32 one=rd(0x10001F90),zero=rd(0x10001F88);
 if(relation==2) {
  f32 a=ta->get(),b=tb->get();if(a<zero||a>one||b<zero||b>one)return 0;
  gabi::call<void>(0x0201888C,capsule,pa.get(),a);gabi::call<void>(0x0201888C,line.get(),pb.get(),tb->get());
 } else if(relation==3) {
  f32 a=ta->get();if(a<zero)copyVectorWords(c,gabi::ea(pa.get()));else if(a>one)copyVectorWords(c+12,gabi::ea(pa.get()));else gabi::call<void>(0x0201888C,capsule,pa.get(),a);
  f32 b=tb->get();if(b<zero)copyVectorWords(l,gabi::ea(pb.get()));else if(b>one)copyVectorWords(l+12,gabi::ea(pb.get()));else gabi::call<void>(0x0201888C,line.get(),pb.get(),b);
 } else return 0;
 f32 d=gabi::call<f32>(0x028E8E80,pa.get(),pb.get());if(!(d<rd(c+28)+rd(y+12)))return 0;
 gabi::call<void>(0x028E8D88,pa.get(),pb.get(),out);gabi::call<void>(0x028E8E64,out,out,half);return 1;
}
VERIFY(0x02015CF0,cM3d_Cross_CpsCyl);
static bool inclusionAxis(f32 a,f32 b,f32 c,f32 point,f32 margin) {
 f32 delta=a-b,low=delta>=0?b:a,high=delta>=0?a:b;
 if(low>c)low=c;else if(high<c)high=c;
 return !(low-margin>point)&&!(high+margin<point);
}
s32 cM3d_Cross_SphTri(void* sphere,void* triangle,void* out) {
 WWHD_FUNC(0x02014200,s32,sphere,triangle,out);
 u32 s=gabi::ea(sphere),t=gabi::ea(triangle);
 f32 radius=rd(s+12);
 if(!inclusionAxis(rd(t+20),rd(t+32),rd(t+44),rd(s),radius))return 0;
 if(!inclusionAxis(rd(t+28),rd(t+40),rd(t+52),rd(s+8),radius))return 0;
 if(!inclusionAxis(rd(t+24),rd(t+36),rd(t+48),rd(s+4),radius))return 0;
 f32 distance=gabi::call<f32>(0x02010C50,triangle,sphere);if(std::fabs(distance)>rd(s+12))return 0;
 gabi::Local<M3dVector> normalOffset,projected,a,b,intersection;
 gabi::call<void>(0x028E8E64,triangle,normalOffset.get(),distance);gabi::call<void>(0x028E8DAC,sphere,normalOffset.get(),projected.get());
 f32 ny=rd(t+4),negativeZero=rd(0x10002BE0),half=rd(0x10002BDC),zero=rd(0x10001F88),epsilon=rd(0x10001F8C),margin=rd(0x10001F9C);
 u32 p=gabi::ea(projected.get());bool inside=false,tested=false;
 f32 ax=0,ay=0,az=0,bx=0,by=0,bz=0;
 if(std::fabs(ny)>half) {
  if(!(std::fabs(ny)<epsilon)) {
   az=rd(t+28);ax=rd(t+20);bz=rd(t+40);bx=rd(t+32);f32 cz=rd(t+52),cx=rd(t+44);
   inside=projectedTriangleCached(az,ax,bz,bx,cz,cx,rd(p+8),rd(p),margin,zero,negativeZero);tested=true;
   if(!inside){by=rd(t+36);ay=rd(t+24);}
  }
 } else {
  f32 nx=rd(t);
  if(std::fabs(nx)>rd(0x10002BDC)) {
   if(!(std::fabs(nx)<epsilon)) {
    ay=rd(t+24);az=rd(t+28);by=rd(t+36);bz=rd(t+40);f32 cy=rd(t+48),cz=rd(t+52);
    inside=projectedTriangleCached(ay,az,by,bz,cy,cz,rd(p+4),rd(p+8),margin,zero,negativeZero);tested=true;
    if(!inside){bx=rd(t+32);ax=rd(t+20);}
   }
  } else if(!(std::fabs(rd(t+8))<epsilon)) {
   ax=rd(t+20);ay=rd(t+24);bx=rd(t+32);by=rd(t+36);f32 cx=rd(t+44),cy=rd(t+48);
   inside=projectedTriangleCached(ax,ay,bx,by,cx,cy,rd(p),rd(p+4),margin,zero,negativeZero);tested=true;
   if(!inside){bz=rd(t+40);az=rd(t+28);}
  }
 }
 if(inside){if(out)gabi::call<void>(0x02014140,sphere,triangle,out);return 1;}
 if(!tested){bx=rd(t+32);by=rd(t+36);ax=rd(t+20);az=rd(t+28);bz=rd(t+40);ay=rd(t+24);}
 a->x=ax;a->y=ay;a->z=az;b->x=bx;b->y=by;b->z=bz;
 gabi::Local<M3dLine> firstLine,secondLine,thirdLine;
 gabi::call<void*>(0x02018780,firstLine.get(),a.get(),b.get());
 if(gabi::call<s32>(0x020130D4,firstLine.get(),sphere,intersection.get())){if(out)gabi::call<void>(0x02014140,sphere,triangle,out);return 1;}
 a->x=rd(t+44);a->y=rd(t+48);a->z=rd(t+52);b->x=rd(t+32);b->y=rd(t+36);b->z=rd(t+40);
 gabi::call<void*>(0x02018780,secondLine.get(),b.get(),a.get());
 if(gabi::call<s32>(0x020130D4,secondLine.get(),sphere,intersection.get())){if(out)gabi::call<void>(0x02014140,sphere,triangle,out);return 1;}
 b->x=rd(t+44);b->y=rd(t+48);b->z=rd(t+52);a->x=rd(t+20);a->y=rd(t+24);a->z=rd(t+28);
 gabi::call<void*>(0x02018780,thirdLine.get(),b.get(),a.get());
 if(!gabi::call<s32>(0x020130D4,thirdLine.get(),sphere,intersection.get()))return 0;
 if(out)gabi::call<void>(0x02014140,sphere,triangle,out);return 1;
}
VERIFY(0x02014200,cM3d_Cross_SphTri);
struct M3dPlane {M3dVector normal;be<f32> distance;be<u32> vtable;};
WWHD_SIZE(M3dPlane,20);
s32 cM3d_Cross_MinMaxBoxLine(void* minimum,void* maximum,void* start,void* end) {
 WWHD_FUNC(0x02010FFC,s32,minimum,maximum,start,end);
 u32 lo=gabi::ea(minimum),hi=gabi::ea(maximum),s=gabi::ea(start),e=gabi::ea(end),startCode=0,endCode=0;
 f32 a[3],b[3],min[3],max[3];
 for(u32 axis:{0u,2u,1u}) {
  a[axis]=rd(s+axis*4);max[axis]=rd(hi+axis*4);b[axis]=rd(e+axis*4);
  u32 upper=1u<<(axis*2),lower=upper<<1;
  if(a[axis]>max[axis]){if(b[axis]>max[axis])return 0;startCode|=upper;}else if(b[axis]>max[axis])endCode|=upper;
  min[axis]=rd(lo+axis*4);
  if(!(startCode&upper)&&a[axis]<min[axis]){if(!(endCode&upper)&&b[axis]<min[axis])return 0;startCode|=lower;}
  else if(!(endCode&upper)&&b[axis]<min[axis])endCode|=lower;
 }
 if(startCode==0||endCode==0)return 1;
 auto bevel2=[&](const f32* p){
  u32 bits=0;
  f32 diff=p[1]-p[0],sum=p[0]+p[1];
  if(diff>max[1]-min[0])bits|=1;if(diff<min[1]-max[0])bits|=2;
  if(sum>max[0]+max[1])bits|=4;if(sum<min[0]+min[1])bits|=8;
  diff=p[1]-p[2];sum=p[2]+p[1];
  if(diff>max[1]-min[2])bits|=16;if(diff<min[1]-max[2])bits|=32;
  if(sum>max[2]+max[1])bits|=64;if(sum<min[2]+min[1])bits|=128;
  diff=p[0]-p[2];sum=p[2]+p[0];
  if(diff>max[0]-min[2])bits|=256;if(diff<min[0]-max[2])bits|=512;
  if(sum>max[2]+max[0])bits|=1024;if(sum<min[2]+min[0])bits|=2048;
  return bits;
 };
 startCode|=bevel2(a)<<8;endCode|=bevel2(b)<<8;if(startCode&endCode)return 0;
 auto bevel3=[&](const f32* p){
  u32 bits=0;
  if((p[0]+p[1])+p[2]>(max[0]+max[1])+max[2])bits|=1;
  if((p[1]-p[0])+p[2]>(max[1]-min[0])+max[2])bits|=2;
  if((p[1]-p[0])-p[2]>(max[1]-min[0])-min[2])bits|=4;
  if((p[0]+p[1])-p[2]>(max[0]+max[1])-min[2])bits|=8;
  if((p[0]-p[1])+p[2]>(max[0]-min[1])+max[2])bits|=16;
  // Both HD bevel bits use this same expression.
  if((-p[0]-p[1])+p[2]>(-min[0]-min[1])+max[2])bits|=96;
  if((-p[0]-p[1])-p[2]>(-min[0]-min[1])-min[2])bits|=128;
  return bits;
 };
 startCode|=bevel3(a)<<24;endCode|=bevel3(b)<<24;if(startCode&endCode)return 0;
 gabi::Local<M3dVector> first,last,cross;first->x=a[0];first->y=a[1];first->z=a[2];last->x=b[0];last->y=b[1];last->z=b[2];
 gabi::Local<M3dLine> line;gabi::call<void*>(0x02018780,line.get(),first.get(),last.get());
 f32 zero=rd(0x10001F88),one=rd(0x10001F90),negative=0;
 u32 changed=startCode^endCode;
 // Each HD plane has separate storage; constructors may leave untouched input bytes.
 gabi::Local<M3dPlane> planes[6];
 for(u32 face=0;face<6;face++) {
  if(face==1)negative=rd(0x10001F98);
  if(!(changed&(1u<<face)))continue;
  M3dPlane* plane=planes[face].get();gabi::call<void*>(0x020189A0,plane);
  u32 axis=face/2,p=gabi::ea(plane);bool high=(face&1)==0;
  wr(p,axis==0?(high?one:negative):zero);wr(p+4,axis==1?(high?one:negative):zero);wr(p+8,axis==2?(high?one:negative):zero);
  wr(p+12,high?-rd(hi+axis*4):rd(lo+axis*4));
  if(!gabi::call<s32>(0x02010F00,line.get(),plane,cross.get(),1u,1u))continue;
  u32 q=gabi::ea(cross.get());bool in=true;
  for(u32 other=0;other<3;other++)if(other!=axis) {
   f32 v=rd(q+other*4);if(rd(lo+other*4)>v||v>rd(hi+other*4)){in=false;break;}
  }
  if(in)return 1;
 }
 return 0;
}
VERIFY(0x02010FFC,cM3d_Cross_MinMaxBoxLine);

/* ---- hosted here: the static initializer(s) of a separate header-static-only TU linked between c_m3d and c_m3d_g_aab (name unknown).
 * Each only initializes the shared header statics (zeroed 16-byte object, -pi/pi pair, two
 * registered global objects); no code of their own. ---- */
void hd_static_init_02017C74() {
 WWHD_FUNC(0x02017C74,void);
 gabi::store<u32>(0x101FF8B4,0);gabi::store<u32>(0x101FF8AC,0);gabi::store<u32>(0x101FF8B8,0);gabi::store<u32>(0x101FF8B0,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C8B0));
 f32 negativePi=gabi::load<f32>(0x100030C8),positivePi=gabi::load<f32>(0x100030CC);
 gabi::store<f32>(0x101FF8A0,negativePi);gabi::store<f32>(0x101FF8A4,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF8A8));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C8BC));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF8A9));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C8C8));
}
VERIFY(0x02017C74,hd_static_init_02017C74);
