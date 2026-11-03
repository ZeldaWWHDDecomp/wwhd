/* Pane point hit test reconstructed from the WWHD paired-single instructions. */
#include "nw/lyt/LayoutHitTest.h"
using namespace nw::lyt;
struct HitTestPoint { be<f32> x,y,z; };
/* 0287F05C */
u32 nw::lyt::containsLayoutTransformedPoint(LayoutPane* pane,const PaneSize* point) {
    gabi::AutoFrame frame{gabi::current_function()};
    const Mtx34* matrix=&pane->content()->matrix;
    f32 d=matrix->m[1][0];
    f32 h=matrix->m[2][1],i=matrix->m[2][2];
    f32 e=matrix->m[1][1],f=matrix->m[1][2];
    f32 g=matrix->m[2][0];
    f32 b=matrix->m[0][1],c=matrix->m[0][2];
    f32 a=matrix->m[0][0];
    f32 first=gabi::fmsubs(e,i,gabi::fmuls_ppc(h,f));
    f32 second=gabi::fmsubs(f,g,gabi::fmuls_ppc(i,d));
    f32 upperMiddle=gabi::fmsubs(h,c,gabi::fmuls_ppc(b,i));
    f32 middle=gabi::fmsubs(i,a,gabi::fmuls_ppc(c,g));
    f32 determinant=gabi::fmadds(d,upperMiddle,gabi::fmuls_ppc(a,first));
    f32 secondDeterminant=gabi::fmadds(1.0f,middle,gabi::fmuls_ppc(1.0f,second));
    f32 upperRight=gabi::fmsubs(b,f,gabi::fmuls_ppc(e,c));
    f32 middleRight=gabi::fmsubs(c,d,gabi::fmuls_ppc(f,a));
    determinant=gabi::fmadds(g,upperRight,determinant);
    secondDeterminant=gabi::fmadds(1.0f,middleRight,secondDeterminant);
    f32 bottomLeft=gabi::fmsubs(d,h,gabi::fmuls_ppc(e,g));
    f32 bottomRight=gabi::fmsubs(a,e,gabi::fmuls_ppc(b,d));
    f32 bottomMiddle=gabi::fmsubs(b,g,gabi::fmuls_ppc(a,h));
    gabi::FrameLocal<Mtx34> inverse(0x28);
    // The original leaves this frame region untouched when both determinant lanes are zero.
    if (determinant!=0.0f || secondDeterminant!=0.0f) {
        f32 reciprocal=gabi::fdivs_ppc(1.0f,determinant);
        f32 upperLeft=gabi::fmuls_ppc(first,reciprocal);
        f32 middleLeft=gabi::fmuls_ppc(second,reciprocal);
        f32 tx=matrix->m[0][3];
        upperMiddle=gabi::fmuls_ppc(upperMiddle,reciprocal);
        middle=gabi::fmuls_ppc(middle,reciprocal);
        f32 ty=matrix->m[1][3];
        f32 tz=matrix->m[2][3];
        inverse->m[0][0]=upperLeft;
        inverse->m[0][1]=upperMiddle;
        f32 firstTranslation=gabi::fmuls_ppc(upperLeft,tx);
        f32 secondTranslation=gabi::fmuls_ppc(middleLeft,tx);
        inverse->m[1][0]=middleLeft;
        inverse->m[1][1]=middle;
        bottomLeft=gabi::fmuls_ppc(bottomLeft,reciprocal);
        f32 thirdTranslation=gabi::fmuls_ppc(bottomLeft,tx);
        upperRight=gabi::fmuls_ppc(upperRight,reciprocal);
        middleRight=gabi::fmuls_ppc(middleRight,reciprocal);
        firstTranslation=gabi::fmadds(upperMiddle,ty,firstTranslation);
        secondTranslation=gabi::fmadds(middle,ty,secondTranslation);
        firstTranslation=gabi::fnmadds(upperRight,tz,firstTranslation);
        secondTranslation=gabi::fnmadds(middleRight,tz,secondTranslation);
        bottomMiddle=gabi::fmuls_ppc(bottomMiddle,reciprocal);
        inverse->m[2][0]=bottomLeft;
        inverse->m[2][1]=bottomMiddle;
        inverse->m[0][2]=upperRight;
        inverse->m[0][3]=firstTranslation;
        bottomRight=gabi::fmuls_ppc(bottomRight,reciprocal);
        inverse->m[2][2]=bottomRight;
        thirdTranslation=gabi::fmadds(bottomMiddle,ty,thirdTranslation);
        inverse->m[1][2]=middleRight;
        inverse->m[1][3]=secondTranslation;
        inverse->m[2][3]=gabi::fnmadds(bottomRight,tz,thirdTranslation);
    }
    f32 x=point->width;
    gabi::FrameLocal<HitTestPoint> position(8);
    position->z=0.0f;
    f32 y=point->height;
    position->x=x;
    position->y=y;
    // Each paired lane multiplies one component, adds z/translation, then sums the lanes.
    f32 xFirst=gabi::fmuls_ppc(inverse->m[0][0],position->x);
    f32 xSecond=gabi::fmuls_ppc(inverse->m[0][1],position->y);
    xFirst=gabi::fmadds(inverse->m[0][2],position->z,xFirst);
    xSecond=gabi::fmadds(inverse->m[0][3],1.0f,xSecond);
    f32 transformedX=gabi::fadds_ppc(xFirst,xSecond);
    f32 yFirst=gabi::fmuls_ppc(inverse->m[1][0],position->x);
    f32 ySecond=gabi::fmuls_ppc(inverse->m[1][1],position->y);
    yFirst=gabi::fmadds(inverse->m[1][2],position->z,yFirst);
    ySecond=gabi::fmadds(inverse->m[1][3],1.0f,ySecond);
    f32 transformedY=gabi::fadds_ppc(yFirst,ySecond);
    gabi::FrameLocal<PaneSize> transformed(0x1C);
    transformed->width=transformedX;
    transformed->height=transformedY;
    gabi::FrameLocal<TextBounds> bounds(0x58);
    pane->bounds(bounds.get());
    f32 finalX=transformed->width;
    f32 finalY=transformed->height;
    gabi::FrameLocal<PaneSize> resultPoint(0x14);
    resultPoint->width=finalX;
    resultPoint->height=finalY;
    return panePointContains(bounds.get(),resultPoint.get());
}
/* WWHD glue
ext 02876368 void nw::lyt::LayoutPane::bounds(nw::lyt::TextBounds*) const
ext 0287F014 u32 panePointContains(const nw::lyt::TextBounds*,const nw::lyt::PaneSize*)
*/
