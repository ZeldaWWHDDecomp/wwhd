// Primitive vertices and topology reconstructed from local WWHD instructions.
#include "sead/PrimitiveMeshBuilders.h"
using namespace sead;
struct QuadVertices { PrimitiveVertex_l mVertices[4]; };
static QuadVertices sQuadVertices;
struct CubeVertices { PrimitiveVertex_l mVertices[8]; };
static CubeVertices sCubeVertices;
struct LineVertices { PrimitiveVertex_l mVertices[2]; };
static LineVertices sLineVertices;
static u32 sQuadInitialized;
static u32 sCubeInitialized;
static u32 sLineInitialized;
struct LineTopology { be<u16> mIndices[2]; };
struct QuadTopology { be<u16> mIndices[6]; };
struct CubeTopology { be<u16> mIndices[36]; };
struct WireCubeTopology { be<u16> mIndices[17]; };
// These small fixed index lists are code-defined primitive topology, not game assets.
static LineTopology sLineTopology={{0,1}};
static QuadTopology sQuadTopology={{0,2,1,1,2,3}};
static CubeTopology sCubeTopology={{2,1,0,3,2,0,5,2,3,4,5,3,6,5,7,7,5,4,
    1,6,0,6,7,0,0,7,3,3,7,4,1,2,6,2,5,6}};
static WireCubeTopology sWireCubeTopology={{0,1,2,3,0,7,6,1,2,5,6,7,4,5,4,3,0}};
void copyPrimitiveBuilderBytes(void* destination,const void* source,u32 size,u32 flags);
f64 primitiveBuilderSine(f64 angle);
f64 primitiveBuilderCosine(f64 angle);

static void initializeBasicPrimitiveVertex(PrimitiveVertex_l* vertex,f32 x,f32 y,f32 z,f32 u,f32 v,f32 rate) {
    vertex->x=x; vertex->y=y; vertex->z=z;
    vertex->u=u; vertex->v=v;
    vertex->red=rate;
    vertex->green=0; vertex->blue=0; vertex->alpha=0;
}
// Libm returns a full FPR. Preserve it through the multiply before single rounding.
static f32 multiplyPrimitiveReturn(f64 value,f32 scale) {
    return static_cast<f32>(gabi::fmul_ppc(value,static_cast<f64>(scale)));
}

/* 02768AA8 */
void PrimitiveMeshBuilders::quad(PrimitiveVertex_l* vertices,be<u16>* indices) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!sQuadInitialized) {
        for(u32 i=0;i<4;++i) {
            f32 x=(i&1)?0.5f:-0.5f;
            f32 y=(i&2)?-0.5f:0.5f;
            f32 u=(i&1)?1.0f:0.0f;
            f32 v=(i&2)?0.0f:1.0f;
            f32 rate=(i&2)?1.0f:0.0f;
            initializeBasicPrimitiveVertex(&sQuadVertices.mVertices[i],x,y,0,u,v,rate);
        }
        sQuadInitialized=1;
    }
    if(vertices) copyPrimitiveBuilderBytes(vertices,&sQuadVertices,sizeof(sQuadVertices),0);
    if(indices) copyPrimitiveBuilderBytes(indices,&sQuadTopology,sizeof(sQuadTopology),0);
}
/* 02768BD4 */
void PrimitiveMeshBuilders::cube(PrimitiveVertex_l* vertices,be<u16>* indices) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!sCubeInitialized) {
        for(u32 i=0;i<8;++i) {
            bool upper=i==1 || i==2 || i==5 || i==6;
            bool front=i>=2 && i<=5;
            bool rightTexture=i==2 || i==3 || i==6 || i==7;
            f32 x=i<4?-0.5f:0.5f;
            f32 y=upper?0.5f:-0.5f;
            f32 z=front?0.5f:-0.5f;
            f32 rate=i==1?0.0f:(i==4?1.0f:((i==0 || i==2 || i==6)?1.0f/3.0f:2.0f/3.0f));
            initializeBasicPrimitiveVertex(&sCubeVertices.mVertices[i],x,y,z,rightTexture?1.0f:0.0f,upper?1.0f:0.0f,rate);
        }
        sCubeInitialized=1;
    }
    if(vertices) copyPrimitiveBuilderBytes(vertices,&sCubeVertices,sizeof(sCubeVertices),0);
    if(indices) copyPrimitiveBuilderBytes(indices,&sCubeTopology,sizeof(sCubeTopology),0);
}
/* 02768DA0 */
void PrimitiveMeshBuilders::wireCube(PrimitiveVertex_l* vertices,be<u16>* indices) {
    gabi::AutoFrame frame{gabi::current_function()};
    cube(vertices,nullptr);
    if(indices) copyPrimitiveBuilderBytes(indices,&sWireCubeTopology,sizeof(sWireCubeTopology),0);
}
/* 027694B0 */
void PrimitiveMeshBuilders::line(PrimitiveVertex_l* vertices,be<u16>* indices) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!sLineInitialized) {
        initializeBasicPrimitiveVertex(&sLineVertices.mVertices[0],-0.5f,0,0,0,0.5f,0);
        initializeBasicPrimitiveVertex(&sLineVertices.mVertices[1],0.5f,0,0,1,0.5f,1);
        sLineInitialized=1;
    }
    if(vertices) copyPrimitiveBuilderBytes(vertices,&sLineVertices,sizeof(sLineVertices),0);
    if(indices) copyPrimitiveBuilderBytes(indices,&sLineTopology,sizeof(sLineTopology),0);
}
/* 02768DF0 */
void PrimitiveMeshBuilders::sphere(PrimitiveVertex_l* vertices,be<u16>* indices,s32 slices,s32 rings) {
    gabi::AutoFrame frame{gabi::current_function()};
    const f32 tau=6.2831854820251465f;
    if(vertices) {
        for(s32 row=0;row<rings;++row) {
            f32 ratio=gabi::fdivs_ppc(static_cast<f32>(row+1),gabi::fadds_ppc(static_cast<f32>(rings),1.0f));
            f32 latitude=gabi::fmuls_ppc(gabi::fsubs_ppc(ratio,0.5f),3.1415927410125732f);
            f32 y=multiplyPrimitiveReturn(primitiveBuilderSine(latitude),0.5f);
            f32 radius=multiplyPrimitiveReturn(primitiveBuilderCosine(latitude),0.5f);
            f32 rate=gabi::fsubs_ppc(0.5f,y),u=gabi::fadds_ppc(y,0.5f);
            for(s32 j=0;j<slices;++j) {
                f32 angle=gabi::fdivs_ppc(gabi::fmuls_ppc(tau,static_cast<f32>(j)),static_cast<f32>(slices));
                if(!(row&1)) angle=gabi::fnmsubs(gabi::fdivs_ppc(tau,static_cast<f32>(slices)),0.5f,angle);
                f32 x=multiplyPrimitiveReturn(primitiveBuilderCosine(angle),radius);
                f64 sine=primitiveBuilderSine(angle);
                auto* vertex=&vertices[static_cast<u32>(row)*static_cast<u32>(slices)+static_cast<u32>(j)];
                vertex->red=rate; vertex->x=x; vertex->u=u; vertex->y=y;
                f32 z=multiplyPrimitiveReturn(sine,radius);
                vertex->v=gabi::fdivs_ppc(static_cast<f32>(j),static_cast<f32>(slices)); vertex->z=z;
            }
        }
        u32 pole=static_cast<u32>(slices)*static_cast<u32>(rings);
        auto* bottom=&vertices[pole];
        bottom->x=0; bottom->u=0; bottom->v=0.5f; bottom->y=-0.5f; bottom->red=1; bottom->z=0;
        auto* top=&vertices[pole+1];
        top->y=0.5f; top->z=0; top->v=0.5f; top->x=0; top->u=1; top->red=0;
    }
    if(indices) {
        for(s32 j=0;j<slices;++j) {
            u32 base=static_cast<u32>(j)*3;
            indices[base]=static_cast<u32>(slices)*static_cast<u32>(rings);
            indices[base+1]=j; indices[base+2]=(j+1)%slices;
        }
        for(s32 row=0;row<rings-1;++row) for(s32 j=0;j<slices;++j) {
            u32 a=static_cast<u32>(row)*static_cast<u32>(slices)+static_cast<u32>(j);
            u32 b=static_cast<u32>(row+1)*static_cast<u32>(slices)+static_cast<u32>((j+(row&1))%slices);
            u32 c=static_cast<u32>(row)*static_cast<u32>(slices)+static_cast<u32>((j+1)%slices);
            u32 d=static_cast<u32>(row+1)*static_cast<u32>(slices)+static_cast<u32>((j+1+(row&1))%slices);
            u32 base=static_cast<u32>(slices)*3+static_cast<u32>(row)*static_cast<u32>(slices)*6+static_cast<u32>(j)*6;
            indices[base+2]=c; indices[base+1]=b; indices[base]=a;
            indices[base+3]=b; indices[base+5]=c; indices[base+4]=d;
        }
        for(s32 j=0;j<slices;++j) {
            u32 base=static_cast<u32>(slices)*3+static_cast<u32>(rings-1)*static_cast<u32>(slices)*6+static_cast<u32>(j)*3;
            indices[base]=static_cast<u32>(slices)*static_cast<u32>(rings)+1;
            indices[base+1]=static_cast<u32>(rings-1)*static_cast<u32>(slices)+static_cast<u32>((j+1)%slices);
            indices[base+2]=static_cast<u32>(rings-1)*static_cast<u32>(slices)+static_cast<u32>(j);
        }
    }
}
/* 027692BC */
void PrimitiveMeshBuilders::disk(PrimitiveVertex_l* vertices,be<u16>* indices,s32 slices) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(vertices) {
        for(s32 j=0;j<slices;++j) {
            f32 angle=gabi::fdivs_ppc(gabi::fmuls_ppc(6.2831854820251465f,static_cast<f32>(j)),static_cast<f32>(slices));
            auto* vertex=&vertices[j];
            vertex->x=multiplyPrimitiveReturn(primitiveBuilderCosine(angle),0.5f);
            f32 y=multiplyPrimitiveReturn(primitiveBuilderSine(angle),0.5f);
            vertex->u=vertex->x; vertex->red=1; vertex->y=y; vertex->z=0;
            vertex->v=gabi::fsubs_ppc(1.0f,y);
        }
        auto* center=&vertices[slices];
        center->y=0; center->z=0; center->v=0.5f; center->x=0; center->u=0.5f; center->red=0;
    }
    if(indices) for(s32 j=0;j<slices;++j) {
        u32 base=static_cast<u32>(j)*3;
        indices[base]=j; indices[base+2]=slices; indices[base+1]=(j+1)%slices;
    }
}
/* 02769594 */
void PrimitiveMeshBuilders::cylinder(PrimitiveVertex_l* vertices,be<u16>* indices,s32 slices) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 lowerBase=static_cast<u32>(slices)+1;
    if(vertices) {
        for(s32 j=0;j<slices;++j) {
            f32 angle=gabi::fdivs_ppc(gabi::fmuls_ppc(6.2831854820251465f,static_cast<f32>(j)),static_cast<f32>(slices));
            auto* top=&vertices[j]; auto* bottom=&vertices[lowerBase+static_cast<u32>(j)];
            top->x=multiplyPrimitiveReturn(primitiveBuilderCosine(angle),0.5f);
            f32 z=-multiplyPrimitiveReturn(primitiveBuilderSine(angle),0.5f);
            top->u=top->x; top->red=0; top->z=z; top->y=0.5f; top->v=gabi::fsubs_ppc(1.0f,z);
            bottom->x=multiplyPrimitiveReturn(primitiveBuilderCosine(angle),0.5f);
            bottom->z=-multiplyPrimitiveReturn(primitiveBuilderSine(angle),0.5f); bottom->y=-0.5f;
            bottom->u=top->x; bottom->v=gabi::fsubs_ppc(1.0f,top->z); bottom->red=1;
        }
        auto* top=&vertices[slices];
        top->x=0; top->u=0.5f; top->v=0.5f; top->y=0.5f; top->red=0; top->z=0;
        auto* bottom=&vertices[static_cast<u32>(slices)+lowerBase];
        bottom->y=-0.5f; bottom->z=0; bottom->v=0.5f; bottom->x=0; bottom->u=0.5f; bottom->red=1;
    }
    if(indices) {
        for(s32 j=0;j<slices;++j) {
            u32 next=static_cast<u32>((j+1)%slices),base=static_cast<u32>(j)*3;
            indices[base]=j; indices[base+2]=slices; indices[base+1]=next;
            base+=static_cast<u32>(slices)*3;
            indices[base]=lowerBase+static_cast<u32>(j); indices[base+1]=lowerBase+static_cast<u32>(slices); indices[base+2]=lowerBase+next;
        }
        for(s32 j=0;j<slices;++j) {
            u32 next=static_cast<u32>((j+1)%slices),lower=lowerBase+static_cast<u32>(j);
            u32 base=static_cast<u32>(slices)*6+static_cast<u32>(j)*6;
            indices[base+4]=lower; indices[base+1]=lower; indices[base]=j;
            indices[base+2]=next; indices[base+3]=next; indices[base+5]=lowerBase+next;
        }
    }
}
/* WWHD glue
 ext C0009988 void copyPrimitiveBuilderBytes(void*,const void*,u32,u32)
 ext 028F43F8 f64 primitiveBuilderSine(f64)
 ext 028F4BE0 f64 primitiveBuilderCosine(f64)
 data sQuadVertices 104A1248
 data sCubeVertices 104A12D8
 data sLineVertices 104A13F8
 data sQuadInitialized 104A1440
 data sCubeInitialized 104A1444
 data sLineInitialized 104A1448
 data sLineTopology 1015F708
 data sQuadTopology 1015F70C
 data sCubeTopology 1015F718
 data sWireCubeTopology 1015F778
*/
