#include "wwhd.h"
#include "gabi.h"
namespace {
template<class T>T get(u32 p,u32 o=0){return *gabi::at<be<T>>(p+o);}
template<class T>void put(u32 p,u32 o,T x){*gabi::at<be<T>>(p+o)=x;}
struct Vec {be<f32>x,y,z;};
f32 dot(u32 p,f32 x,f32 y,f32 z){f32 vy=gabi::fmuls_ppc(y,get<f32>(p,4));f32 xy=gabi::fmadds(x,get<f32>(p),vy);return gabi::fmadds(z,get<f32>(p,8),xy);}
}
void clipperInit(u32 p){WWHD_FUNC(0x02838000,void,p);f32 near=get<f32>(0x10172B48),far=get<f32>(0x10172B4C);put<f32>(p,0x50,near);put<f32>(p,0x54,far);}
VERIFY(0x02838000,clipperInit);
void clipperCalc(u32 p){
 WWHD_FUNC(0x0283801C,void,p);
 f32 angle=gabi::fmuls_ppc(get<f32>(p,0x48),get<f32>(0x10172B50));angle=gabi::fmuls_ppc(angle,get<f32>(0x10172B54));
 f64 tangent=gabi::call<f64>(0x028F423C,angle);
 f32 near=get<f32>(p,0x50),ny=f32(f64(near)*round25(tangent)),minusY=-ny,minusNear=-near;
 f32 nx=gabi::fmuls_ppc(get<f32>(p,0x4C),ny),minusX=-nx;
 f32 twiceA=gabi::fmuls_ppc(minusNear,minusY),twiceB=gabi::fmuls_ppc(ny,minusNear);
 f32 nX=gabi::fmuls_ppc(minusNear,minusX),xY=gabi::fmuls_ppc(ny,minusX),yX=gabi::fmuls_ppc(minusX,minusY),xy=gabi::fmuls_ppc(nx,minusNear),yx=gabi::fmuls_ppc(nx,ny);
 put<f32>(p,0xC,gabi::fsubs_ppc(twiceB,twiceB));put<f32>(p,8,gabi::fsubs_ppc(yX,xY));
 put<f32>(p,0,gabi::fsubs_ppc(twiceB,twiceA));put<f32>(p,0x18,gabi::fsubs_ppc(twiceA,twiceB));
 put<f32>(p,0x1C,gabi::fsubs_ppc(xy,xy));put<f32>(p,4,gabi::fsubs_ppc(nX,nX));
 put<f32>(p,0x10,gabi::fsubs_ppc(nX,xy));put<f32>(p,0x14,gabi::fsubs_ppc(yx,xY));
 put<f32>(p,0x20,gabi::fsubs_ppc(gabi::fmuls_ppc(nx,ny),gabi::fmuls_ppc(minusY,nx)));
 put<f32>(p,0x24,gabi::fsubs_ppc(twiceA,twiceA));put<f32>(p,0x28,gabi::fsubs_ppc(xy,nX));
 put<f32>(p,0x2C,gabi::fsubs_ppc(yX,gabi::fmuls_ppc(minusY,nx)));
 for(u32 i=0;i<4;++i)gabi::call<void>(0x028E8EF0,p+12*i,p+12*i);
}
VERIFY(0x0283801C,clipperCalc);
u32 clipperSphere(u32 p,u32 matrix,u32 pos,f32 radius){
 WWHD_FUNC(0x02838148,u32,p,matrix,pos,radius);gabi::Local<Vec> out;
 gabi::call<void>(0x028E8F64,matrix,pos,out.get());f32 z=out->z,y=out->y,x=out->x;
 if(-z<gabi::fsubs_ppc(get<f32>(p,0x50),radius))return 1;
 if(-z>gabi::fadds_ppc(get<f32>(p,0x54),radius))return 1;
 for(u32 i=0;i<4;++i)if(dot(p+12*i,x,y,z)>radius)return 1;
 return 0;
}
VERIFY(0x02838148,clipperSphere);
u32 clipperBox(u32 p,u32 matrix,u32 minimum,u32 maximum){
 WWHD_FUNC(0x02838274,u32,p,matrix,minimum,maximum);gabi::Local<Vec[8]> corners;gabi::Local<Vec> out;
 f32 minZ=get<f32>(minimum,8),maxY=get<f32>(maximum,4),maxZ=get<f32>(maximum,8),minX=get<f32>(minimum),maxX=get<f32>(maximum),minY=get<f32>(minimum,4);
 f32 xs[8]={maxX,maxX,minX,minX,maxX,maxX,minX,minX};f32 ys[8]={maxY,maxY,maxY,maxY,minY,minY,minY,minY};f32 zs[8]={minZ,maxZ,maxZ,minZ,minZ,maxZ,maxZ,minZ};
 for(u32 i=0;i<8;++i){(*corners)[i].x=xs[i];(*corners)[i].y=ys[i];(*corners)[i].z=zs[i];}
 u32 counts[6]={};f32 zero=get<f32>(0x10172B58);
 for(u32 i=0;i<8;++i){gabi::call<void>(0x028E8F64,matrix,corners.a+12*i,out.get());f32 z=out->z,y=out->y,x=out->x;u32 any=0;
 if(-z<get<f32>(p,0x50)){++counts[4];++any;}if(-z>get<f32>(p,0x54)){++counts[5];++any;}
 for(u32 j=0;j<4;++j)if(dot(p+12*j,x,y,z)>zero){++counts[j];++any;}
 if(!any)return 0;
 }
 for(u32 i: {0u,2u,1u,3u,4u,5u})if(counts[i]==8)return 1;return 0;
}
VERIFY(0x02838274,clipperBox);
void clipperInitializer(){WWHD_FUNC(0x028384FC,void);put<u32>(0x104B5FFC,8,0);put<u32>(0x104B5FFC,0,0);put<u32>(0x104B5FFC,12,0);put<u32>(0x104B5FFC,4,0);gabi::call<void>(0x028F026C,u32(0x101FAD08));}
VERIFY(0x028384FC,clipperInitializer);
u32 clipperByBox(u32 p,u32 model){WWHD_FUNC(0x02838524,u32,p,model);return 0;}
VERIFY(0x02838524,clipperByBox);
