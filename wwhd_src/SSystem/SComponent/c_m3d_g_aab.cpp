/* c_m3d_g_aab: WWHD axis-aligned box operations, 02017D08..02018030.
 * Includes the HD per-unit static initializer. */
#include "gabi.h"
namespace c_m3d_g_aab_cpp {
using gabi::load; using gabi::store; using gabi::call;
static f32 value(u32 p) { return load<f32>(p); }
void set(u32 p,u32 lo,u32 hi) {
 WWHD_FUNC(0x02017D08,void,p,lo,hi);
 for(u32 o=0;o<12;o+=4) store<u32>(p+o,load<u32>(lo+o));
 for(u32 o=0;o<12;o+=4) store<u32>(p+12+o,load<u32>(hi+o));
}
VERIFY(0x02017D08,set);
BOOL crossY(u32 p,u32 q) {
 WWHD_FUNC(0x02017D3C,BOOL,p,q);
 f32 x=value(q);
 if(value(p)>x || value(p+12)<x) return 0;
 f32 z=value(q+8);
 return !(value(p+8)>z || value(p+20)<z);
}
VERIFY(0x02017D3C,crossY);
BOOL underPlane(u32 p,f32 y) { WWHD_FUNC(0x02017D84,BOOL,p,y); return value(p+4)<y; }
VERIFY(0x02017D84,underPlane);
BOOL topPlane(u32 p,f32 y) { WWHD_FUNC(0x02017D98,BOOL,p,y); return value(p+16)<y; }
VERIFY(0x02017D98,topPlane);
void clear(u32 p) {
 WWHD_FUNC(0x02017DAC,void,p);
 f32 lo=value(0x100030D0),hi=value(0x100030D4);
 store<f32>(p+8,lo); store<f32>(p+12,hi); store<f32>(p+4,lo);
 store<f32>(p+16,hi); store<f32>(p,lo); store<f32>(p+20,hi);
}
VERIFY(0x02017DAC,clear);
void clearY(u32 p) { WWHD_FUNC(0x02017DD8,void,p); f32 lo=value(0x100030D0),hi=value(0x100030D4); store<f32>(p+4,lo); store<f32>(p+16,hi); }
VERIFY(0x02017DD8,clearY);
void setMin(u32 p,u32 q) {
 WWHD_FUNC(0x02017DF4,void,p,q);
 f32 x=value(q),oldX=value(p),oldY=value(p+4);
 if(oldX>x) store<f32>(p,x);
 f32 y=value(q+4),oldZ=value(p+8);
 if(oldY>y) store<f32>(p+4,y);
 f32 z=value(q+8);
 if(oldZ>z) store<f32>(p+8,z);
}
VERIFY(0x02017DF4,setMin);
void setMax(u32 p,u32 q) {
 WWHD_FUNC(0x02017E34,void,p,q);
 f32 x=value(q),oldX=value(p+12),oldY=value(p+16);
 if(oldX<x) store<f32>(p+12,x);
 f32 y=value(q+4),oldZ=value(p+20);
 if(oldY<y) store<f32>(p+16,y);
 f32 z=value(q+8);
 if(oldZ<z) store<f32>(p+20,z);
}
VERIFY(0x02017E34,setMax);
void setMinMax(u32 p,u32 q) { WWHD_FUNC(0x02017E74,void,p,q); call(0x02017DF4,p,q); call(0x02017E34,gabi::cpu->r[3],gabi::cpu->r[4]); }
VERIFY(0x02017E74,setMinMax);
void expandBox(u32 p,u32 q) { WWHD_FUNC(0x02017E98,void,p,q); call(0x02017E74,p,q); call(0x02017E74,gabi::cpu->r[3],gabi::cpu->r[4]+12); }
VERIFY(0x02017E98,expandBox);
void setMinY(u32 p,f32 y) { WWHD_FUNC(0x02017EC0,void,p,y); if(value(p+4)>y) store<f32>(p+4,y); }
VERIFY(0x02017EC0,setMinY);
void setMaxY(u32 p,f32 y) { WWHD_FUNC(0x02017ED4,void,p,y); if(value(p+16)<y) store<f32>(p+16,y); }
VERIFY(0x02017ED4,setMaxY);
void setMinMaxY(u32 p,f32 y) { WWHD_FUNC(0x02017EE8,void,p,y); f32 lo=value(p+4),hi=value(p+16); if(lo>y) store<f32>(p+4,y); if(hi<y) store<f32>(p+16,y); }
VERIFY(0x02017EE8,setMinMaxY);
void center(u32 p,u32 out) { WWHD_FUNC(0x02017F0C,void,p,out); call(0x028E8D88,p,p+12,out); call(0x028E8E64,out,out,value(0x100030D8)); }
VERIFY(0x02017F0C,center);
void plusRadius(u32 p,f32 r) {
 WWHD_FUNC(0x02017F54,void,p,r);
 f32 x=value(p),y=value(p+4),z=value(p+8),mx=value(p+12),mz=value(p+20);
 y=gabi::fsubs_ppc(y,r); z=gabi::fsubs_ppc(z,r); mx=gabi::fadds_ppc(mx,r);
 store<f32>(p+4,y); x=gabi::fsubs_ppc(x,r); f32 my=value(p+16);
 store<f32>(p+8,z); my=gabi::fadds_ppc(my,r); store<f32>(p,x);
 mz=gabi::fadds_ppc(mz,r); store<f32>(p+12,mx); store<f32>(p+16,my); store<f32>(p+20,mz);
}
VERIFY(0x02017F54,plusRadius);
void init() {
 WWHD_FUNC(0x02017FA0,void);
 store<u32>(0x101FF8D0,0); store<u32>(0x101FF8C8,0); store<u32>(0x101FF8D4,0); store<u32>(0x101FF8CC,0);
 call(0x028F026C,0x1018C8D4u);
 store<f32>(0x101FF8BC,value(0x100030E0)); store<f32>(0x101FF8C0,value(0x100030E4));
 call(0x028ED6F8,0x101FF8C4u); call(0x028F026C,0x1018C8E0u);
 call(0x028EAB2C,0x101FF8C5u); call(0x028F026C,0x1018C8ECu);
}
VERIFY(0x02017FA0,init);
}
