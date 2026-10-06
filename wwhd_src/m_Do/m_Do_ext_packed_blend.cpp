#include "gabi.h"
using namespace gabi;
namespace ext_packed {
struct Pair {f32 first,second;};
static u32 angleWord(f32 angle){call(0x028F5680,angle);return cpu->r[4];} // Native unsigned64 conversion returns low word in r4.
static Pair tablePair(u32 word,f32 factor){
 u32 table=0x1016C7E0+((word>>24)&255)*16;
 f32 fraction=fmuls_ppc((f32)(u16)(word>>8),factor);
 f32 slope0=load<f32>(table+8),slope1=load<f32>(table+12);
 f32 base0=load<f32>(table),base1=load<f32>(table+4);
 return {fmadds(slope0,fraction,base0),fmadds(slope1,fraction,base1)};
}
static void linear(u32 out,u32 record,u32 weights,u32 index){
 f32 weight=load<f32>(weights+index*4);
 f32 a0=load<f32>(record+4),a1=load<f32>(record+8),b0=load<f32>(out+4),b1=load<f32>(out+8);
 f32 b2=load<f32>(out+12),b3=load<f32>(out+16);
 store<f32>(out+4,fmadds(a0,weight,b0));store<f32>(out+8,fmadds(a1,weight,b1));
 f32 a2=load<f32>(record+12),a3=load<f32>(record+16),total=load<f32>(out+28);
 f32 b4=load<f32>(out+20),b5=load<f32>(out+24);
 store<f32>(out+12,fmadds(a2,weight,b2));store<f32>(out+16,fmadds(a3,weight,b3));
 f32 a4=load<f32>(record+20),a5=load<f32>(record+24);
 u32 flags=load<u32>(out),source=load<u32>(record);
 store<f32>(out+20,fmadds(a4,weight,b4));store<f32>(out+24,fmadds(a5,weight,b5));
 store<f32>(out+28,total+weight);store<u32>(out,flags|~source);
}
static f32 sine(u32 word,f32 factor){
 u32 table=0x1016C7E0+((word>>24)&255)*16;
 f32 fraction=fmuls_ppc((f32)(word&0xFFFFFF),factor);
 f32 slope=load<f32>(table+8),base=load<f32>(table);
 return fmadds(slope,fraction,base);
}
static f32 reciprocalRefined(f32 x){f32 estimate=(f32)fres(x);f32 twice=estimate+estimate;return fnmsubs(fmuls_ppc(x,estimate),estimate,twice);}
static u32 unsignedTruncate(f32 x,f32 boundary){if(!(x<boundary))return (u32)ftoi(x-boundary)+0x80000000;return (u32)ftoi(x);}
static void quaternionBlend(u32 out,u32 record,u32 weights,u32 index,f32 zero,f32 scale,f32 fractionScale,f32 boundary,f32 threshold,f32 one,f64 positive,f64 negative,const f32* computed=nullptr,f32 savedOldWeight=0,f32 savedWeight=0){
 f32 oldWeight=computed?savedOldWeight:load<f32>(out+28),weight=computed?savedWeight:load<f32>(weights+index*4);
 if(oldWeight==zero){for(u32 j=0;j<4;j++){if(computed)store<f32>(out+32+j*4,computed[j]);else store<u32>(out+32+j*4,load<u32>(record+32+j*4));}linear(out,record,weights,index);return;}
 f32 old[4]={load<f32>(out+32),load<f32>(out+36),load<f32>(out+40),load<f32>(out+44)};
 f32 incoming[4];for(u32 j=0;j<4;j++)incoming[j]=computed?computed[j]:load<f32>(record+32+j*4);
 f32 p0=fmuls_ppc(old[0],incoming[0]),p1=fmuls_ppc(old[1],incoming[1]);
 f32 a=fmadds(old[2],incoming[2],p0),b=fmadds(old[3],incoming[3],p1),dot=a+b;
 f32 total=oldWeight+weight,estimate=(f32)fres(total),prod=fmuls_ppc(total,estimate);
 f32 sign=(f32)(dot>=0?negative:positive),magnitude=std::fabs(dot);
 f32 ratio=fmuls_ppc(weight,fnmsubs(prod,estimate,estimate+estimate));
 f32 oldCoefficient,newCoefficient;
 if(magnitude>threshold){newCoefficient=fmuls_ppc(sign,ratio);oldCoefficient=one-ratio;}
 else{
  f64 angle=call<f64>(0x028F4FAC,(f64)magnitude);
  u32 word=angleWord((f32)(angle*(f64)scale));
  f32 denominator=sine(word,fractionScale),inv=reciprocalRefined(denominator);
  u32 next=unsignedTruncate(fmuls_ppc(ratio,(f32)word),boundary),previous=word-next;
  oldCoefficient=fmuls_ppc(sine(previous,fractionScale),inv);
  newCoefficient=fmuls_ppc(sign,fmuls_ppc(sine(next,fractionScale),inv));
 }
 for(u32 pair=0;pair<2;pair++){
  u32 p=out+32+pair*8,r=record+32+pair*8;
  f32 new0=computed?computed[pair*2]:load<f32>(r),new1=computed?computed[pair*2+1]:load<f32>(r+4),old0=load<f32>(p),old1=load<f32>(p+4);
  f32 p0=fmuls_ppc(new0,newCoefficient),p1=fmuls_ppc(new1,newCoefficient);
  store<f32>(p,fmadds(old0,oldCoefficient,p0));store<f32>(p+4,fmadds(old1,oldCoefficient,p1));
 }
 linear(out,record,weights,index);
}
void quaternionAccumulate(void* destination,void* animation,void* weights){
 WWHD_FUNC(0x025EEC68,void,destination,animation,weights);
 u32 d=ea(destination),a=ea(animation),w=ea(weights);
 store<u32>(a+0x50,load<u32>(a+0x50)|8);u32 record=load<u32>(a+0x2C);
 store<u32>(d+8,load<u32>(d+8)|8);u16 count=load<u16>(a+0x40);u32 buffer=load<u32>(d);
 if(!count)return;
 f32 zero=load<f32>(0x100586DC),factor=load<f32>(0x100589C8),scale=load<f32>(0x100589C0),threshold=load<f32>(0x100589D0),boundary=load<f32>(0x100589CC),one=load<f32>(0x100586D4);
 f64 positive=load<f64>(0x100589D8),negative=load<f64>(0x100589E0);
 for(u32 i=0;i<count;i++,record+=48){u32 flag=load<u32>(load<u32>(a+0x38)+i*4);if(flag&0x80000000)continue;quaternionBlend(buffer+(flag&0x7FFF)*56,record,w,i,zero,scale,factor,boundary,threshold,one,positive,negative);}
}
VERIFY(0x025EEC68,quaternionAccumulate);
static Pair fullTablePair(u32 word,f32 factor){u32 table=0x1016C7E0+((word>>24)&255)*16;f32 fraction=fmuls_ppc((f32)(word&0xFFFFFF),factor);f32 slope0=load<f32>(table+8),base0=load<f32>(table),slope1=load<f32>(table+12),base1=load<f32>(table+4);return {fmadds(slope0,fraction,base0),fmadds(slope1,fraction,base1)};}
static f32 squareRootRefined(f32 value,f32 half,f32 threeHalves){if(value==0)return value;f32 estimate=(f32)frsqrte(value),product=fmuls_ppc(fmuls_ppc(half,value),estimate);estimate=fmuls_ppc(estimate,fnmsubs(product,estimate,threeHalves));return fmuls_ppc(value,estimate);}
static void matrixQuaternion(const f32* m,f32* q,f32 zero,f32 quarter,f32 half,f32 threeHalves,f32 four){
 f32 trace=(m[0]+m[4])+m[8],argument=trace;u32 axis=3;
 if(trace<zero){argument=(m[0]+m[0])-trace;axis=0;if(argument<zero){argument=(m[4]+m[4])-trace;axis=1;if(argument<zero){argument=(m[8]+m[8])-trace;axis=2;}}}
 f32 root=squareRootRefined(fmadds(argument,quarter,quarter),half,threeHalves),inv=reciprocalRefined(fmuls_ppc(root,four));
 if(axis==3){q[0]=fmuls_ppc(m[7]-m[5],inv);q[1]=fmuls_ppc(m[2]-m[6],inv);q[2]=fmuls_ppc(m[3]-m[1],inv);q[3]=root;}
 else if(axis==0){q[0]=root;q[1]=fmuls_ppc(m[3]+m[1],inv);q[2]=fmuls_ppc(m[2]+m[6],inv);q[3]=fmuls_ppc(m[7]-m[5],inv);}
 else if(axis==1){q[0]=fmuls_ppc(m[1]+m[3],inv);q[1]=root;q[2]=fmuls_ppc(m[5]+m[7],inv);q[3]=fmuls_ppc(m[2]-m[6],inv);}
 else{q[0]=fmuls_ppc(m[6]+m[2],inv);q[1]=fmuls_ppc(m[7]+m[5],inv);q[2]=root;q[3]=fmuls_ppc(m[3]-m[1],inv);}
}
void eulerQuaternionAccumulate(void* destination,void* animation,void* weights){
 WWHD_FUNC(0x025EF13C,void,destination,animation,weights);
 u32 d=ea(destination),a=ea(animation),w=ea(weights);
 store<u32>(a+0x50,load<u32>(a+0x50)|8);u32 record=load<u32>(a+0x2C);
 store<u32>(d+8,load<u32>(d+8)|8);u16 count=load<u16>(a+0x40);u32 buffer=load<u32>(d);if(!count)return;
 f32 threshold=load<f32>(0x100589D0),quarter=load<f32>(0x100586D0),scale=load<f32>(0x100589C0),fractionScale=load<f32>(0x100589C8),boundary=load<f32>(0x100589CC),zero=load<f32>(0x100586DC),one=load<f32>(0x100586D4),four=load<f32>(0x100589F0),threeHalves=load<f32>(0x100589F4),half=load<f32>(0x100586D8);
 f64 negative=load<f64>(0x100589E0),positive=load<f64>(0x100589D8);
 for(u32 i=0;i<count;i++,record+=48){
  u32 flag=load<u32>(load<u32>(a+0x38)+i*4);if(flag&0x80000000)continue;u32 out=buffer+(flag&0x7FFF)*56;
  f32 angle=load<f32>(record+32),weight=load<f32>(w+i*4);
  Pair x=fullTablePair(angleWord(fmuls_ppc(angle,scale)),fractionScale);
  Pair y=fullTablePair(angleWord(fmuls_ppc(load<f32>(record+36),scale)),fractionScale);
  u32 zword=angleWord(fmuls_ppc(load<f32>(record+40),scale));
  // The native reads destination weight after the third conversion, before final trig interpolation.
  f32 oldWeight=load<f32>(out+28);Pair z=fullTablePair(zword,fractionScale);
  f32 sxsy=fmuls_ppc(x.first,y.first),sxcy=fmuls_ppc(x.first,y.second),cxcy=fmuls_ppc(x.second,y.second);
  f32 cxsz=fmuls_ppc(x.second,z.first),sxcz=fmuls_ppc(x.first,z.second),sxsz=fmuls_ppc(x.first,z.first),cxcz=fmuls_ppc(x.second,z.second);
  f32 m[9]={fmuls_ppc(y.second,z.second),fmsubs(sxsy,z.second,cxsz),fmadds(cxcz,y.first,sxsz),fmuls_ppc(y.second,z.first),fmadds(sxsy,z.first,cxcz),fmsubs(cxsz,y.first,sxcz),-y.first,sxcy,cxcy};
  f32 q[4];matrixQuaternion(m,q,zero,quarter,half,threeHalves,four);
  quaternionBlend(out,record,w,i,zero,scale,fractionScale,boundary,threshold,one,positive,negative,q,oldWeight,weight);
 }
}
VERIFY(0x025EF13C,eulerQuaternionAccumulate);
void eulerAccumulate(void* destination,void* animation,void* weights){
 WWHD_FUNC(0x025EE9F4,void,destination,animation,weights);
 u32 d=ea(destination),a=ea(animation),w=ea(weights);
 store<u32>(a+0x50,load<u32>(a+0x50)|8);u32 record=load<u32>(a+0x2C);
 store<u32>(d+8,load<u32>(d+8)|8);u16 count=load<u16>(a+0x40);u32 buffer=load<u32>(d);
 if(!count)return;
 f32 scale=load<f32>(0x100589C0),factor=load<f32>(0x100589C4);
 for(u32 i=0;i<count;i++,record+=48){
  u32 flag=load<u32>(load<u32>(a+0x38)+i*4);if(flag&0x80000000)continue;
  f32 angle=load<f32>(record+0x20),weight=load<f32>(w+i*4);u32 out=buffer+(flag&0x7FFF)*56;
  Pair x=tablePair(angleWord(fmuls_ppc(angle,scale)),factor);
  Pair y=tablePair(angleWord(fmuls_ppc(load<f32>(record+0x24),scale)),factor);
  Pair z=tablePair(angleWord(fmuls_ppc(load<f32>(record+0x28),scale)),factor);
  Pair xz{fmuls_ppc(x.first,z.first),fmuls_ppc(x.second,z.second)};
  Pair cross{fmuls_ppc(x.second,z.first),fmuls_ppc(x.first,z.second)};
  Pair plus{fmadds(xz.second,y.first,xz.first),fmadds(xz.first,y.first,xz.second)};
  Pair minus{fmsubs(cross.second,y.first,cross.first),fmsubs(cross.first,y.first,cross.second)};
  f32 values[6]={fmuls_ppc(z.second,y.second),minus.first,plus.first,fmuls_ppc(z.first,y.second),plus.second,minus.second};
  for(u32 pair=0;pair<3;pair++){u32 p=out+0x20+pair*8;f32 old0=load<f32>(p),old1=load<f32>(p+4);store<f32>(p,fmadds(values[pair*2],weight,old0));store<f32>(p+4,fmadds(values[pair*2+1],weight,old1));}
  linear(out,record,w,i);
 }
}
VERIFY(0x025EE9F4,eulerAccumulate);
}
