/* WWHD c_angle: signed angles, degree/polar/globe coordinates.
 * HD uses explicit result pointers and allocating null-this constructors. */
#include "gabi.h"
namespace c_angle_cpp {
template<unsigned N> struct Temp : gabi::Local<unsigned char[N]> { u32 addr() const { return this->a; } };
using gabi::load; using gabi::store; using gabi::call;
static s16 angle(u32 p) { return load<s16>(p); }
static f32 cf(u32 a) { return load<f32>(a); }
static u32 alloc(u32 p,u32 n) { return p ? p : call<u32>(0x0273AD10,n); }
void angleVal(u32 p,s16 v) { WWHD_FUNC(0x02006584, void, p, v); store<s16>(p,v); }
VERIFY(0x02006584,angleVal);
u32 angleCtor(u32 p,u32 v) { WWHD_FUNC(0x0200658C, u32, p, v); p=alloc(p,2); if(p) call(0x02006584,p,v); return p; }
VERIFY(0x0200658C,angleCtor);
u32 scalarPlus(u32 out,s16 v,u32 a) { WWHD_FUNC(0x020065DC, u32, out, v, a); return call<u32>(0x0200658C,out,(s16)(v+angle(a))); }
VERIFY(0x020065DC,scalarPlus);
u32 scalarMinus(u32 out,s16 v,u32 a) { WWHD_FUNC(0x020065EC, u32, out, v, a); return call<u32>(0x0200658C,out,(s16)(v-angle(a))); }
VERIFY(0x020065EC,scalarMinus);
u32 angleDefault(u32 p) { WWHD_FUNC(0x020065FC, u32, p); p=alloc(p,2); if(p) store<s16>(p,0); return p; }
VERIFY(0x020065FC,angleDefault);
void angleCopyVal(u32 p,u32 a) { WWHD_FUNC(0x02006638, void, p, a); store<s16>(p,angle(a)); }
VERIFY(0x02006638,angleCopyVal);
u32 angleCopyCtor(u32 p,u32 a) { WWHD_FUNC(0x02006644, u32, p, a); p=alloc(p,2); if(p) call(0x02006638,p,a); return p; }
VERIFY(0x02006644,angleCopyCtor);
void angleDegreeVal(u32 p,f32 v) { WWHD_FUNC(0x02006694, void, p, v); store<s16>(p,(s16)gabi::ftoi(gabi::fmuls_ppc(v,cf(0x10000AD8)))); }
VERIFY(0x02006694,angleDegreeVal);
u32 angleDegreeCtor(u32 p,f32 v) { WWHD_FUNC(0x020066C0, u32, p, v); p=alloc(p,2); if(p) call(0x02006694,p,v); return p; }
VERIFY(0x020066C0,angleDegreeCtor);
f32 degree(u32 p) { WWHD_FUNC(0x02006720, f32, p); return gabi::fmuls_ppc((f32)angle(p),cf(0x10000AE8)); }
VERIFY(0x02006720,degree);
f32 radian(u32 p) { WWHD_FUNC(0x02006760, f32, p); f32 scale=gabi::fmuls_ppc(cf(0x101FF33C),cf(0x10000AF8)); return gabi::fmuls_ppc((f32)angle(p),scale); }
VERIFY(0x02006760,radian);
f32 norm(u32 p) { WWHD_FUNC(0x020067AC, f32, p); return gabi::fmuls_ppc((f32)angle(p),cf(0x10000AF8)); }
VERIFY(0x020067AC,norm);
s16 absAngle(u32 p) { WWHD_FUNC(0x020067EC, s16, p); s32 v=angle(p); return (s16)(v<0?-v:v); }
VERIFY(0x020067EC,absAngle);
s16 inverse(u32 p) { WWHD_FUNC(0x02006804, s16, p); return (s16)(angle(p)-0x8000); }
VERIFY(0x02006804,inverse);
f32 sine(u32 p) { WWHD_FUNC(0x02006814, f32, p); f32 r=call<f32>(0x02006760,p); return call<f32>(0x028F43F8,r); }
VERIFY(0x02006814,sine);
f32 cosine(u32 p) { WWHD_FUNC(0x02006838, f32, p); f32 r=call<f32>(0x02006760,p); return call<f32>(0x028F4BE0,r); }
VERIFY(0x02006838,cosine);
f32 tangent(u32 p) { WWHD_FUNC(0x0200685C, f32, p); f32 r=call<f32>(0x02006760,p); return call<f32>(0x028F423C,r); }
VERIFY(0x0200685C,tangent);
u32 negate(u32 p,u32 out) { WWHD_FUNC(0x02006880, u32, p, out); return call<u32>(0x0200658C,out,(s16)-angle(p)); }
VERIFY(0x02006880,negate);
u32 plus(u32 p,u32 out,u32 q) { WWHD_FUNC(0x02006894, u32, p, out, q); return call<u32>(0x0200658C,out,(s16)(angle(p)+angle(q))); }
VERIFY(0x02006894,plus);
u32 minus(u32 p,u32 out,u32 q) { WWHD_FUNC(0x020068B0, u32, p, out, q); return call<u32>(0x0200658C,out,(s16)(angle(p)-angle(q))); }
VERIFY(0x020068B0,minus);
void plusEq(u32 p,u32 q) { WWHD_FUNC(0x020068CC, void, p, q); store<s16>(p,(s16)(angle(p)+angle(q))); }
VERIFY(0x020068CC,plusEq);
void minusEq(u32 p,u32 q) { WWHD_FUNC(0x020068E0, void, p, q); store<s16>(p,(s16)(angle(p)-angle(q))); }
VERIFY(0x020068E0,minusEq);
u32 plusScalar(u32 p,u32 out,s16 v) { WWHD_FUNC(0x020068F4, u32, p, out, v); return call<u32>(0x0200658C,out,(s16)(angle(p)+v)); }
VERIFY(0x020068F4,plusScalar);
u32 minusScalar(u32 p,u32 out,s16 v) { WWHD_FUNC(0x02006908, u32, p, out, v); return call<u32>(0x0200658C,out,(s16)(angle(p)-v)); }
VERIFY(0x02006908,minusScalar);
void plusScalarEq(u32 p,s16 v) { WWHD_FUNC(0x0200691C, void, p, v); store<s16>(p,(s16)(angle(p)+v)); }
VERIFY(0x0200691C,plusScalarEq);
void minusScalarEq(u32 p,s16 v) { WWHD_FUNC(0x0200692C, void, p, v); store<s16>(p,(s16)(angle(p)-v)); }
VERIFY(0x0200692C,minusScalarEq);
u32 multiply(u32 p,u32 out,f32 v) { WWHD_FUNC(0x0200693C, u32, p, out, v); return call<u32>(0x0200658C,out,(s16)gabi::ftoi(gabi::fmuls_ppc((f32)angle(p),v))); }
VERIFY(0x0200693C,multiply);
void multiplyEq(u32 p,f32 v) { WWHD_FUNC(0x020069A0, void, p, v); store<s16>(p,(s16)gabi::ftoi(gabi::fmuls_ppc((f32)angle(p),v))); }
VERIFY(0x020069A0,multiplyEq);
u32 formalDegree(u32 p) { WWHD_FUNC(0x020069EC, u32, p); f32 v=call<f32>(0x020074FC,cf(p),cf(0x10000B00),cf(0x10000B04)); store<f32>(p,v); return p; }
VERIFY(0x020069EC,formalDegree);
u32 degreeVal(u32 p,f32 v) { WWHD_FUNC(0x02006A34, u32, p, v); store<f32>(p,v); return call<u32>(0x020069EC,p); }
VERIFY(0x02006A34,degreeVal);
u32 degreeCtor(u32 p,f32 v) { WWHD_FUNC(0x02006A3C, u32, p, v); p=alloc(p,4); if(p) call(0x02006A34,p,v); return p; }
VERIFY(0x02006A3C,degreeCtor);
f32 degreeRadian(u32 p) { WWHD_FUNC(0x02006AA4, f32, p); f32 scale=cf(0x101FF33C)/cf(0x10000B04); return gabi::fmuls_ppc(cf(p),scale); }
VERIFY(0x02006AA4,degreeRadian);
f32 degreeSin(u32 p) { WWHD_FUNC(0x02006AC4, f32, p); f32 r=call<f32>(0x02006AA4,p); return call<f32>(0x028F43F8,r); }
VERIFY(0x02006AC4,degreeSin);
f32 degreeCos(u32 p) { WWHD_FUNC(0x02006AE8, f32, p); f32 r=call<f32>(0x02006AA4,p); return call<f32>(0x028F4BE0,r); }
VERIFY(0x02006AE8,degreeCos);
u32 polarDefault(u32 p) { WWHD_FUNC(0x02006B0C, u32, p); p=alloc(p,8); if(p) { store<f32>(p,cf(0x10000AFC)); call(0x020065FC,p+4); call(0x020065FC,p+6); } return p; }
VERIFY(0x02006B0C,polarDefault);
u32 formalPolar(u32 p) { WWHD_FUNC(0x02006B68, u32, p);
 if(cf(p)<cf(0x10000AFC)) { store<f32>(p,-cf(p)); Temp<2> a,b; u32 q=call<u32>(0x0200658C,a.addr(),(s16)-32768); call(0x020068B0,q,b.addr(),p+4); call(0x02006638,p+4,b.addr()); s16 v=call<s16>(0x02006804,p+6); call(0x02006584,p+6,v); }
 s16 v=angle(p+4); if(v<0 && v!=-32768) { Temp<2> a; call(0x02006880,p+4,a.addr()); call(0x02006638,p+4,a.addr()); s16 w=call<s16>(0x02006804,p+6); call(0x02006584,p+6,w); } return p;
}
VERIFY(0x02006B68,formalPolar);
void polarVal(u32 p,u32 a,u32 b,f32 r) { WWHD_FUNC(0x02006C28, void, p, a, b, r); store<f32>(p,r); Temp<2> t; u32 q=call<u32>(0x0200658C,t.addr(),a); store<s16>(p+4,angle(q)); q=call<u32>(0x0200658C,t.addr(),b); store<s16>(p+6,angle(q)); call(0x02006B68,p); }
VERIFY(0x02006C28,polarVal);
void polarVecVal(u32 p,u32 v) { WWHD_FUNC(0x02006C8C, void, p, v); f32 x=cf(v),z=cf(v+8),y=cf(v+4); f64 h=(f64)gabi::fmuls_ppc(z,z)+(f64)gabi::fmuls_ppc(x,x), full=h+(f64)gabi::fmuls_ppc(y,y); f32 zero=cf(0x10000AFC),hr=zero,r=zero; f64 dz=load<f64>(0x10000B08); if(h>dz) hr=call<f32>(0x028F4384,(f32)h); if(full>dz) r=call<f32>(0x028F4384,(f32)full); store<f32>(p,r); s16 a=call<s16>(0x020195B0,hr,y); call(0x02006584,p+4,a); s16 b=call<s16>(0x020195B0,x,z); call(0x02006584,p+6,b); call(0x02006B68,p); }
VERIFY(0x02006C8C,polarVecVal);
u32 polarVecCtor(u32 p,u32 v) { WWHD_FUNC(0x02006DE8, u32, p, v); p=alloc(p,8); if(p) { call(0x020065FC,p+4); call(0x020065FC,p+6); call(0x02006C8C,p,v); } return p; }
VERIFY(0x02006DE8,polarVecCtor);
void polarXyz(u32 p,u32 out) { WWHD_FUNC(0x02006E50, void, p, out); f32 a=call<f32>(0x02006760,p+4), sa=call<f32>(0x028F43F8,a); f32 radialSin=gabi::fmuls_ppc(cf(p),sa); f32 b=call<f32>(0x02006760,p+6),sb=call<f32>(0x028F43F8,b); f32 x=gabi::fmuls_ppc(radialSin,sb); a=call<f32>(0x02006760,p+4); f32 ca=call<f32>(0x028F4BE0,a), y=gabi::fmuls_ppc(cf(p),ca); b=call<f32>(0x02006760,p+6); f32 cb=call<f32>(0x028F4BE0,b),z=gabi::fmuls_ppc(radialSin,cb); out=alloc(out,12); if(out) { store<f32>(out+4,y); store<f32>(out,x); store<f32>(out+8,z); } }
VERIFY(0x02006E50,polarXyz);
u32 formalGlobe(u32 p) { WWHD_FUNC(0x02006F30, u32, p);
 if(cf(p)<cf(0x10000AFC)) { store<f32>(p,-cf(p)); Temp<2> t; call(0x02006880,p+4,t.addr()); store<s16>(p+4,angle(t.addr())); s16 v=call<s16>(0x02006804,p+6); call(0x02006584,p+6,v); }
 s16 a=angle(p+4); if(a < -16384 || a>16384) { Temp<2> t,u; u32 q=call<u32>(0x0200658C,t.addr(),(s16)-32768); call(0x020068B0,q,u.addr(),p+4); store<s16>(p+4,angle(u.addr())); s16 v=call<s16>(0x02006804,p+6); call(0x02006584,p+6,v); } return p;
}
VERIFY(0x02006F30,formalGlobe);
void globeVal(u32 p,u32 a,u32 b,f32 r) { WWHD_FUNC(0x02006FE4, void, p, a, b, r); store<f32>(p,r); Temp<2> t; u32 q=call<u32>(0x0200658C,t.addr(),a); store<s16>(p+4,angle(q)); q=call<u32>(0x0200658C,t.addr(),b); store<s16>(p+6,angle(q)); call(0x02006F30,p); }
VERIFY(0x02006FE4,globeVal);
void polarGlobe(u32 p,u32 out) { WWHD_FUNC(0x02007048, void, p, out); s16 a=(s16)(16384-angle(p+4)),b=angle(p+6); f32 r=cf(p); call(0x02006FE4,out,a,b,r); }
VERIFY(0x02007048,polarGlobe);
u32 globeCtor(u32 p,u32 a,u32 b,f32 r) { WWHD_FUNC(0x02007068, u32, p, a, b, r); p=alloc(p,8); if(p) { call(0x020065FC,p+4); call(0x020065FC,p+6); call(0x02006FE4,p,a,b,r); } return p; }
VERIFY(0x02007068,globeCtor);
u32 globeDefault(u32 p) { WWHD_FUNC(0x02007100, u32, p); p=alloc(p,8); if(p) { store<f32>(p,cf(0x10000AFC)); call(0x020065FC,p+4); call(0x020065FC,p+6); } return p; }
VERIFY(0x02007100,globeDefault);
u32 globeCopyVal(u32 p,u32 q) { WWHD_FUNC(0x0200715C, u32, p, q); /* HD lfs/stfs copy preserves the source bits in this reference. */ store<u32>(p,load<u32>(q)); store<s16>(p+4,angle(q+4)); store<s16>(p+6,angle(q+6)); return call<u32>(0x02006F30,p); }
VERIFY(0x0200715C,globeCopyVal);
u32 globeCopyCtor(u32 p,u32 q) { WWHD_FUNC(0x02007178, u32, p, q); p=alloc(p,8); if(p) { call(0x020065FC,p+4); call(0x020065FC,p+6); call(0x0200715C,p,q); } return p; }
VERIFY(0x02007178,globeCopyCtor);
void globeAngleVal(u32 p,u32 a,u32 b,f32 r) { WWHD_FUNC(0x020071E0, void, p, a, b, r); store<f32>(p,r); Temp<2> t; u32 q=call<u32>(0x0200658C,t.addr(),angle(a)); store<s16>(p+4,angle(q)); q=call<u32>(0x0200658C,t.addr(),angle(b)); store<s16>(p+6,angle(q)); call(0x02006F30,p); }
VERIFY(0x020071E0,globeAngleVal);
u32 globeAngleCtor(u32 p,u32 a,u32 b,f32 r) { WWHD_FUNC(0x02007248, u32, p, a, b, r); p=alloc(p,8); if(p) { call(0x020065FC,p+4); call(0x020065FC,p+6); call(0x020071E0,p,a,b,r); } return p; }
VERIFY(0x02007248,globeAngleCtor);
void globeVecVal(u32 p,u32 v) { WWHD_FUNC(0x020072E0, void, p, v); Temp<8> t; call(0x02006DE8,t.addr(),v); call(0x02007048,t.addr(),p); call(0x02006F30,p); }
VERIFY(0x020072E0,globeVecVal);
u32 globeVecCtor(u32 p,u32 v) { WWHD_FUNC(0x02007324, u32, p, v); p=alloc(p,8); if(p) { call(0x020065FC,p+4); call(0x020065FC,p+6); call(0x020072E0,p,v); } return p; }
VERIFY(0x02007324,globeVecCtor);
void globePolar(u32 p,u32 out) { WWHD_FUNC(0x0200738C, void, p, out); s16 a=(s16)(16384-angle(p+4)),b=angle(p+6); f32 r=cf(p); call(0x02006C28,out,a,b,r); }
VERIFY(0x0200738C,globePolar);
void globeXyz(u32 p,u32 out) { WWHD_FUNC(0x020073AC, void, p, out); Temp<8> t; call(0x02006B0C,t.addr()); call(0x0200738C,p,t.addr()); call(0x02006E50,t.addr(),out); }
VERIFY(0x020073AC,globeXyz);
u32 globeInvert(u32 p) { WWHD_FUNC(0x02007400, u32, p); store<f32>(p,-cf(p)); return call<u32>(0x02006F30,p); }
VERIFY(0x02007400,globeInvert);
f32 adjust(f32 v,f32 lo,f32 hi) { WWHD_FUNC(0x020074FC, f32, v, lo, hi); f32 period=hi-lo; while(!(v<hi)) v=v-period; while(v<lo) v=v+period; return v; }
VERIFY(0x020074FC,adjust);
void init() { WWHD_FUNC(0x02007410, void);
 for(u32 o: {0u,4u,8u,12u}) store<u32>(0x101FF344+o,0);
 call(0x028F026C,0x1018C454u);
 store<f32>(0x101FF338,cf(0x10000B10)); store<f32>(0x101FF33C,cf(0x10000B14));
 call(0x028ED6F8,0x101FF340u); call(0x028F026C,0x1018C460u); call(0x028EAB2C,0x101FF341u); call(0x028F026C,0x1018C46Cu);
 call(0x0200658C,0x101FF354u,(s16)0); call(0x0200658C,0x101FF356u,(s16)182); call(0x0200658C,0x101FF358u,(s16)16384); call(0x0200658C,0x101FF35Au,(s16)-32768); call(0x0200658C,0x101FF35Cu,(s16)-16384);
}
VERIFY(0x02007410,init);
}

/* ---- hosted here: the static initializer(s) of a separate header-static-only TU linked between c_angle and c_API_controller (probably c_API by link order: it is followed in .data by the six-pointer g_cAPI_Interface table at 1018C49C).
 * Each only initializes the shared header statics (zeroed 16-byte object, -pi/pi pair, two
 * registered global objects); no code of their own. ---- */
void hd_static_init_0200752C() {
 WWHD_FUNC(0x0200752C,void);
 gabi::store<u32>(0x101FF374,0);gabi::store<u32>(0x101FF36C,0);gabi::store<u32>(0x101FF378,0);gabi::store<u32>(0x101FF370,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C478));
 f32 negativePi=gabi::load<f32>(0x10000B18),positivePi=gabi::load<f32>(0x10000B1C);
 gabi::store<f32>(0x101FF360,negativePi);gabi::store<f32>(0x101FF364,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF368));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C484));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF369));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C490));
}
VERIFY(0x0200752C,hd_static_init_0200752C);
