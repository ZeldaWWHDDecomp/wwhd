/* Axis-aligned bounds and rendering defaults, reconstructed from WWHD. */
#include "nw/ut/Geometry.h"
using namespace nw::ut;
static u8 sSettingsVtable;
/* 0286DC38 */
void Bounds3::transform(Bounds3* output,const Bounds3* box,const Matrix34* matrix) {
    f32 low[3],high[3];
    f32 lx=box->lower[0],ly=box->lower[1],lz=box->lower[2];
    f32 hx=box->upper[0],hy=box->upper[1],hz=box->upper[2];
    for(u32 row=0;row<3;++row) {
        f32 x=matrix->values[row][0],y=matrix->values[row][1],z=matrix->values[row][2],translation=matrix->values[row][3];
        f32 a=gabi::fmadds(x,lx,translation),b=gabi::fmadds(x,hx,translation);
        f32 c=y*ly,d=y*hy,e=z*lz,f=z*hz;
        if(a>b) { f32 swap=a;a=b;b=swap; }
        if(c<d) { a=a+c;b=b+d; } else { a=a+d;b=b+c; }
        if(e<f) { a=a+e;b=b+f; } else { a=a+f;b=b+e; }
        low[row]=a;high[row]=b;
    }
    output->lower[0]=low[0];output->upper[0]=high[0];
    output->upper[1]=high[1];output->upper[2]=high[2];
    output->lower[2]=low[2];output->lower[1]=low[1];
}
/* 0286DE88 */
void Bounds3::order() {
    for(u32 axis=0;axis<3;++axis) {
        f32 a=lower[axis],b=upper[axis];
        if(a>b) { upper[axis]=a;lower[axis]=b; }
    }
}
/* 0286DED4 */
RenderSettings::RenderSettings() {
    blueEnabled=1;greenEnabled=1;vtable=gabi::ea(&sSettingsVtable);
    alphaOperation=0;alphaReference=4;alphaSource=4;alphaCompare=4;target=0;
    stencilPass=2;colorMask=1;alphaWrite=1;stencilMask=0xFFFFFFFF;
    colorDestination=5;depthCompare=3;wireframe=0;logicMode=0xFFFFFFFF;
    polygonOffset=0;stencilDepthFail=2;stencilFail=0;colorOperation=0;redEnabled=1;
    separateAlpha=0;blendEnabled=0;blendConstant=0;depthWrite=1;alphaDestination=0;
    alphaEnabled=1;frontFace=0;stencilReference=0;stencilCompare=0;depthEnabled=1;colorSource=5;
}
/* WWHD glue
data sSettingsVtable 1017F9D8
*/
