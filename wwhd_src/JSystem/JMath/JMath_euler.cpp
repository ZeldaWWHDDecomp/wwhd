/* Euler conversion, following tww JMath.cpp; Cafe uses the fixed sine/cosine lookup. */
#include "JSystem/JMath/JMath_euler.h"
/* 027ED2D0 */
void JMAEulerToQuat(s16 x,s16 y,s16 z,Quaternion* out) {
    u32 ix=static_cast<u16>(x/2)>>3,iy=static_cast<u16>(y/2)>>3,iz=static_cast<u16>(z/2)>>3;
    f32 cx=sSinCosTable.mEntries[ix].mCos,cy=sSinCosTable.mEntries[iy].mCos;
    f32 cxcy=gabi::fmuls_ppc(cx,cy);
    f32 sx=sSinCosTable.mEntries[ix].mSin;
    f32 sxcy=gabi::fmuls_ppc(sx,cy);
    f32 cz=sSinCosTable.mEntries[iz].mCos;
    f32 sy=sSinCosTable.mEntries[iy].mSin;
    f32 cycz=gabi::fmuls_ppc(cy,cz);
    f32 sz=sSinCosTable.mEntries[iz].mSin;
    f32 sxsy=gabi::fmuls_ppc(sx,sy);
    f32 sxsycz=gabi::fmuls_ppc(sxsy,cz);
    f32 sysz=gabi::fmuls_ppc(sy,sz);
    f32 sxsysz=gabi::fmuls_ppc(sx,sysz);
    f32 sxcysz=gabi::fmuls_ppc(sxcy,sz);
    f32 w=gabi::fmadds(cx,cycz,sxsysz);
    f32 cxsysz=gabi::fmuls_ppc(cx,sysz);
    f32 cxsy=gabi::fmuls_ppc(cx,sy);
    f32 qx=gabi::fmsubs(sx,cycz,cxsysz);
    f32 qy=gabi::fmadds(cxsy,cz,sxcysz);
    out->w=w;
    f32 qz=gabi::fmsubs(cxcy,sz,sxsycz);
    out->x=qx;out->y=qy;out->z=qz;
}
/* WWHD glue
*/
