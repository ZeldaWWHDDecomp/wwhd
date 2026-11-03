/* Pane matrix propagation, quantized angle lookup and alpha inheritance from WWHD. */
#include "nw/lyt/PaneState.h"
using namespace nw::lyt;
namespace nw { namespace lyt {
struct PaneTraversalContext {
    be<u32> reserved;
    gptr<Mtx34> rootMatrix;
    be<f32> scaleX, scaleY, alpha;
    be<u8> scaleEnabled, showInvisible, showTransparent, alphaEnabled;
};
} }
struct PaneTraversalMethods { u8 prefix[0x84]; be<u32> updateMatrix; };
struct PaneMatrixConversion { be<u32> high, low; };
struct PaneTrigRecord { be<f32> sine, sineSlope, cosine, cosineSlope; };
struct PaneTrigTable { PaneTrigRecord records[256]; };
struct PaneHomogeneousPair { be<f32> zero, one; };
static PaneTrigTable sPaneTrigTable;
struct PaneAngleUnits { be<u32> value; };
static PaneAngleUnits sPaneAngleUnits;
static PaneHomogeneousPair sPaneHomogeneous;
Pair32 paneConvertAngle(f64);

/* inlined into LayoutPane::updateMatrix: runs in its automatic frame */
static void paneTrig(f32 angle, f32 units, f32* sine, f32* cosine) {
    f32 quantized = gabi::fmuls_ppc(angle, units);
    auto converted = paneConvertAngle(f64(quantized));
    u32 index = converted.r4 >> 24;
    u32 fractional = converted.r4 & 0xFFFFFF;
    gabi::FrameLocal<PaneMatrixConversion> conversion(8);
    conversion->high = 0x43300000;
    conversion->low = fractional;
    f32 fraction = gabi::fmuls_ppc(f32(fractional), 0x1p-24f);
    f32 sineSlope = sPaneTrigTable.records[index].sineSlope;
    f32 cosineSlope = sPaneTrigTable.records[index].cosineSlope;
    f32 sineBase = sPaneTrigTable.records[index].sine;
    f32 cosineBase = sPaneTrigTable.records[index].cosine;
    *sine = gabi::fmadds(sineSlope, fraction, sineBase);
    *cosine = gabi::fmadds(cosineSlope, fraction, cosineBase);
}

static void composePaneMatrices(Mtx34* output, const Mtx34* parent, const Mtx34* local) {
    // The paired-single kernel caches every local column before touching output.
    f32 a00=local->m[0][0], a01=local->m[0][1], a02=local->m[0][2], a03=local->m[0][3];
    f32 a10=local->m[1][0], a11=local->m[1][1], a12=local->m[1][2], a13=local->m[1][3];
    f32 a20=local->m[2][0], a21=local->m[2][1], a22=local->m[2][2], a23=local->m[2][3];
    f32 homogeneousZero=sPaneHomogeneous.zero, homogeneousOne=sPaneHomogeneous.one;
    for (u32 row=0; row<3; ++row) {
        f32 p0=parent->m[row][0], p1=parent->m[row][1];
        f32 p2=parent->m[row][2], p3=parent->m[row][3];
        f32 x=gabi::fmuls_ppc(a00,p0), y=gabi::fmuls_ppc(a01,p0);
        x=gabi::fmadds(a10,p1,x); y=gabi::fmadds(a11,p1,y);
        x=gabi::fmadds(a20,p2,x); y=gabi::fmadds(a21,p2,y);
        f32 z=gabi::fmuls_ppc(a02,p0), w=gabi::fmuls_ppc(a03,p0);
        z=gabi::fmadds(a12,p1,z); w=gabi::fmadds(a13,p1,w);
        z=gabi::fmadds(a22,p2,z); w=gabi::fmadds(a23,p2,w);
        output->m[row][0]=x; output->m[row][1]=y;
        z=gabi::fmadds(homogeneousZero,p3,z); w=gabi::fmadds(homogeneousOne,p3,w);
        output->m[row][2]=z; output->m[row][3]=w;
    }
}

static void composePaneScaleTranslation(Mtx34* output, const Mtx34* parent, f32 sx, f32 sy, f32 tx, f32 ty, f32 tz) {
    // This scalar fast path snapshots all parent entries before writing output.
    f32 p11=parent->m[1][1],p00=parent->m[0][0];
    f32 secondY=gabi::fmuls_ppc(p11,ty);
    f32 p12=parent->m[1][2],p01=parent->m[0][1];
    f32 firstX=gabi::fmuls_ppc(p00,sx);
    f32 p21=parent->m[2][1];
    f32 secondTranslation=gabi::fmadds(p12,tz,secondY);
    f32 p10=parent->m[1][0];
    f32 firstY=gabi::fmuls_ppc(p01,sy);
    f32 p02=parent->m[0][2],p22=parent->m[2][2];
    f32 thirdY=gabi::fmuls_ppc(p21,ty);
    f32 p20=parent->m[2][0];
    f32 firstTranslation=gabi::fmadds(p02,tz,gabi::fmuls_ppc(p01,ty));
    f32 p13=parent->m[1][3];
    f32 secondX=gabi::fmuls_ppc(p10,sx);
    f32 p23=parent->m[2][3];
    f32 thirdTranslation=gabi::fmadds(p22,tz,thirdY);
    f32 p03=parent->m[0][3];
    firstTranslation=gabi::fmadds(p00,tx,firstTranslation);
    thirdTranslation=gabi::fmadds(p20,tx,thirdTranslation);
    output->m[0][0]=firstX;
    output->m[0][1]=firstY;
    secondTranslation=gabi::fmadds(p10,tx,secondTranslation);
    output->m[0][2]=p02;
    output->m[0][3]=gabi::fadds_ppc(firstTranslation,p03);
    output->m[1][0]=secondX;
    output->m[1][1]=gabi::fmuls_ppc(p11,sy);
    output->m[1][2]=p12;
    output->m[1][3]=gabi::fadds_ppc(secondTranslation,p13);
    output->m[2][0]=gabi::fmuls_ppc(p20,sx);
    output->m[2][1]=gabi::fmuls_ppc(p21,sy);
    output->m[2][2]=p22;
    output->m[2][3]=gabi::fadds_ppc(thirdTranslation,p23);
}

/* 028766CC */
void LayoutPane::updateMatrix(PaneTraversalContext* context, u32 force) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* state=content();
    u32 flags=state->flags;
    bool skip=false;
    if (!(flags&1) && u8(context->showInvisible)==0) skip=true;
    if (!skip) {
        if (u8(context->alphaEnabled)!=0 && (LayoutPane*)state->parent) {
            u32 alpha=state->alpha;
            gabi::FrameLocal<PaneMatrixConversion> conversion(8);
            conversion->high=0x43300000; conversion->low=alpha;
            f32 inherited=context->alpha;
            f32 combined=gabi::fmuls_ppc(f32(alpha),inherited);
            conversion->high=u32(gabi::ftoi(combined));
            u32 ownAlpha=state->alpha;
            u32 byte=u32(conversion->high)&255;
            flags=state->flags;
            state->inheritedAlpha=byte;
            if (ownAlpha==0 && (flags&2) && u8(context->showTransparent)==0) skip=true;
        } else {
            u32 alpha=state->alpha;
            flags=state->flags;
            state->inheritedAlpha=alpha;
            if (alpha==0 && (flags&2) && u8(context->showTransparent)==0) skip=true;
        }
    }
    if (skip) { if (force) state->flags=flags|0x10; return; }
    u32 changed=u32((flags&0x10)!=0 || force!=0);
    if (flags&0x40) changed=1;
    else {
        LayoutPane* parent=state->parent;
        Mtx34* parentMatrix=parent ? &parent->content()->matrix : (Mtx34*)context->rootMatrix;
        if (flags&0x20) {
            auto* custom=static_cast<Mtx34*>((void*)state->userPointer);
            composePaneMatrices(&state->matrix,parentMatrix,custom);
            flags=state->flags;
            changed=1;
        } else if (changed) {
            flags &= 0xEF;
            state->flags=flags;
            f32 sx=state->scale[0],sy=state->scale[1];
            if (u8(context->scaleEnabled)!=0 && (flags&4)) {
                f32 contextX=context->scaleX,contextY=context->scaleY;
                sx=gabi::fmuls_ppc(sx,contextX); sy=gabi::fmuls_ppc(sy,contextY);
            }
            u32 origin=state->matrixFlag;
            f32 ty=state->translation[1],tx=state->translation[0],tz=state->translation[2];
            u32 horizontal=(origin>>4)&3,vertical=(origin>>6)&3;
            if (horizontal==1 || horizontal==2) {
                LayoutPane* anchor=state->parent;
                f32 width=anchor->content()->width;
                if (horizontal==1) width=-width;
                tx=gabi::fmadds(width,0.5f,tx);
            }
            if (vertical==1 || vertical==2) {
                LayoutPane* anchor=state->parent;
                f32 height=anchor->content()->height;
                if (vertical==2) height=-height;
                ty=gabi::fmadds(height,0.5f,ty);
            }
            f32 rx=state->rotation[0],ry=state->rotation[1];
            if (rx!=0.0f || ry!=0.0f) {
                u32 units=sPaneAngleUnits.value;
                gabi::FrameLocal<PaneMatrixConversion> conversion(8);
                conversion->high=0x43300000; conversion->low=units;
                f32 scale=gabi::fdivs_ppc(f32(units),180.0f);
                f32 sineX,cosineX,sineY,cosineY,sineZ,cosineZ;
                paneTrig(rx,scale,&sineX,&cosineX);
                paneTrig(ry,scale,&sineY,&cosineY);
                f32 rz=state->rotation[2];
                paneTrig(rz,scale,&sineZ,&cosineZ);
                f32 xy=gabi::fmuls_ppc(sineX,sineY);
                f32 negativeX=-gabi::fmuls_ppc(sx,sineY);
                f32 cosXsinZ=gabi::fmuls_ppc(cosineX,sineZ);
                f32 cosYcosZ=gabi::fmuls_ppc(cosineY,cosineZ);
                f32 cosYsinZ=gabi::fmuls_ppc(cosineY,sineZ);
                f32 sinXcosZ=gabi::fmuls_ppc(sineX,cosineZ);
                f32 cosXcosZ=gabi::fmuls_ppc(cosineX,cosineZ);
                gabi::FrameLocal<Mtx34> local(0x10);
                local->m[0][1]=gabi::fmuls_ppc(sy,gabi::fmsubs(xy,cosineZ,cosXsinZ));
                local->m[2][0]=negativeX;
                local->m[0][3]=tx;
                local->m[0][2]=gabi::fmadds(cosXcosZ,sineY,gabi::fmuls_ppc(sineX,sineZ));
                local->m[1][3]=ty;
                local->m[2][3]=tz;
                local->m[0][0]=gabi::fmuls_ppc(sx,cosYcosZ);
                local->m[1][0]=gabi::fmuls_ppc(sx,cosYsinZ);
                local->m[1][1]=gabi::fmuls_ppc(sy,gabi::fmadds(xy,sineZ,cosXcosZ));
                local->m[2][1]=gabi::fmuls_ppc(sy,gabi::fmuls_ppc(sineX,cosineY));
                local->m[1][2]=gabi::fmsubs(cosXsinZ,sineY,sinXcosZ);
                local->m[2][2]=gabi::fmuls_ppc(cosineX,cosineY);
                composePaneMatrices(&state->matrix,parentMatrix,local.get());
                flags=state->flags;
            } else {
                f32 rz=state->rotation[2];
                if (rz!=0.0f) {
                    u32 units=sPaneAngleUnits.value;
                    gabi::FrameLocal<PaneMatrixConversion> conversion(8);
                    conversion->high=0x43300000; conversion->low=units;
                    f32 scale=gabi::fdivs_ppc(f32(units),180.0f);
                    f32 sine,cosine;
                    paneTrig(rz,scale,&sine,&cosine);
                    gabi::FrameLocal<Mtx34> local(0x10);
                    local->m[1][2]=0.0f; local->m[0][3]=tx;
                    local->m[2][1]=0.0f; local->m[0][2]=0.0f;
                    local->m[2][2]=1.0f;
                    local->m[0][0]=gabi::fmuls_ppc(sx,cosine);
                    local->m[1][3]=ty; local->m[2][3]=tz;
                    local->m[1][0]=gabi::fmuls_ppc(sx,sine);
                    local->m[2][0]=0.0f;
                    local->m[0][1]=-gabi::fmuls_ppc(sy,sine);
                    local->m[1][1]=gabi::fmuls_ppc(sy,cosine);
                    composePaneMatrices(&state->matrix,parentMatrix,local.get());
                    flags=state->flags;
                } else {
                    composePaneScaleTranslation(&state->matrix,parentMatrix,sx,sy,tx,ty,tz);
                    flags=state->flags;
                }
            }
        }
    }
    auto* sentinel=&state->children.root;
    if ((flags&2) && u8(state->alpha)!=255) {
        u32 alpha=state->alpha;
        gabi::FrameLocal<PaneMatrixConversion> conversion(8);
        conversion->low=alpha; conversion->high=0x43300000;
        f32 oldAlpha=context->alpha;
        f32 next=gabi::fmuls_ppc(oldAlpha,f32(alpha));
        u32 oldEnabled=context->alphaEnabled;
        context->alphaEnabled=1;
        context->alpha=gabi::fmuls_ppc(next,0x1.010102p-8f);
        nw::ut::HeapLink* child=sentinel->next;
        while (child!=sentinel) {
            auto* pane=reinterpret_cast<LayoutPane*>(child);
            auto* methods=paneMethods<PaneTraversalMethods>(pane);
            gabi::call_ptr<void>(methods->updateMatrix,pane,context,changed);
            child=child->next;
        }
        context->alphaEnabled=oldEnabled;
        context->alpha=oldAlpha;
    } else {
        nw::ut::HeapLink* child=sentinel->next;
        while (child!=sentinel) {
            auto* pane=reinterpret_cast<LayoutPane*>(child);
            auto* methods=paneMethods<PaneTraversalMethods>(pane);
            gabi::call_ptr<void>(methods->updateMatrix,pane,context,changed);
            child=child->next;
        }
    }
}

/* WWHD glue
ext 028F5680 Pair32 paneConvertAngle(f64)
data sPaneTrigTable 1017E9C4 nocheck
data sPaneAngleUnits 1017FE70 nocheck
data sPaneHomogeneous 10180598 nocheck
*/
