/* Texture matrices and projected coordinate generation for material vertex programs. */
#include "nw/lyt/MaterialTransformUpload.h"
using namespace nw::lyt;
struct MaterialTransformUniform { be<f32> values[4]; };
struct MaterialProjectionFlags { be<u32> values[3]; };
struct MaterialProjectionUniform { be<u32> values[4]; };
struct MaterialProjectionConversion { be<u32> high,low; };
struct MaterialProjectionLastRow { be<f32> values[2]; };
static MaterialProjectionLastRow sMaterialProjectionLastRow;
static u32 materialCoordinateOffset(u32 flags) {
    return (flags>>30)*28+((flags>>28)&3)*20;
}
static u32 materialProjectionOffset(u32 flags) {
    return materialCoordinateOffset(flags)+(((flags>>26)&3)+((flags>>22)&1))*8+
        ((flags>>20)&3)*4+((flags>>19)&1)*12+((flags>>23)&7)*4;
}
/* 0288285C */
void MaterialGpu::uploadTransforms(LayoutDrawContext* context,const PaneSize* size) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 features=featureFlags;
    u32 count=(features>>28)&3;
    if (count) {
        features=featureFlags;
        u32 textures=features>>30;
        if (textures<count) count=textures;
        u32 available=context->stateWordB8;
        if (count>3) count=3;
        if (available<count) count=available;
        gabi::FrameLocal<LayoutTextureMatrix> matrix(0x30);
        gabi::FrameLocal<MaterialTransformUniform> first(0x10);
        gabi::FrameLocal<MaterialTransformUniform> second(0x20);
        for (u32 i=0;i<count;++i) {
            u32 flags=layoutFlags;
            auto* transforms=reinterpret_cast<MaterialTextureTransform*>(storage.get()+(flags>>30)*28);
            matrix->configure(&transforms[i]);
            u32 firstLocation=context->uniformLocation(i+2);
            u32 secondLocation=context->uniformLocation(i+5);
            f32 offset=matrix->horizontalOffset;
            f32 cross=matrix->horizontalCross;
            first->values[3]=offset; first->values[1]=cross;
            f32 scale=matrix->horizontalScale;
            first->values[2]=0; first->values[0]=scale;
            materialTransformUniforms(firstLocation,4,first.get());
            offset=matrix->verticalOffset;
            cross=matrix->verticalCross;
            second->values[3]=offset;
            scale=matrix->verticalScale;
            second->values[2]=0; second->values[1]=scale; second->values[0]=cross;
            materialTransformUniforms(secondLocation,4,second.get());
        }
    }
    features=featureFlags;
    u32 coordinateCount=(features>>26)&3;
    if (coordinateCount) {
        if (coordinateCount>3) coordinateCount=3;
        u32 i=0;
        for (;i<coordinateCount;++i) {
            u32 flags=layoutFlags;
            auto* coordinates=reinterpret_cast<MaterialCoordinate*>(storage.get()+materialCoordinateOffset(flags));
            layoutAtOffset<be<u8>>(context,0xB0+i)->set(coordinates[i].source);
        }
        for (;i<3;++i) layoutAtOffset<be<u8>>(context,0xB0+i)->set(0xFF);
    }
    u32 flags=layoutFlags;
    u32 textureCapacity=flags>>30;
    if (!textureCapacity) return;
    features=featureFlags;
    u32 textures=features>>30;
    count=(features>>26)&3;
    if (textures<count) count=textures;
    if (count>3) count=3;
    gabi::FrameLocal<MaterialProjectionFlags> projected(0x98);
    gabi::FrameLocal<Mtx44> matrix(0x48);
    gabi::FrameLocal<MaterialProjectionConversion> conversion(8);
    u32 projectionIndex=0;
    for (u32 i=0;i<count;++i) {
        flags=layoutFlags;
        u32 coordinateOffset=textureCapacity*28+((flags>>28)&3)*20;
        auto* coordinates=reinterpret_cast<MaterialCoordinate*>(storage.get()+coordinateOffset);
        auto* texture=reinterpret_cast<TextureBinding*>(storage.get()+i*28);
        u32 source=coordinates[i].source;
        u32 isProjected=source==3 || source==4;
        projected->values[i]=isProjected;
        if (isProjected) {
            flags=layoutFlags;
            // Only projected generators consume a projection-transform record.
            auto* projection=reinterpret_cast<MaterialProjectionTransform*>(storage.get()+materialProjectionOffset(flags))+projectionIndex;
            ++projectionIndex;
            f32 projectionHeight=projection->height;
            f32 inverseHeight=gabi::fdivs_ppc(0.5f,projectionHeight);
            f32 width,height,projectionWidth;
            u32 projectionFlags=projection->flags;
            if (projectionFlags&1) {
                auto* layoutSize=gabi::at<PaneSize>(static_cast<u32>(context->stateWord)+0x14);
                width=layoutSize->width;
                projectionWidth=projection->width;
                height=layoutSize->height;
            } else {
                u32 textureWidth=texture->width;
                conversion->low=textureWidth;
                u32 textureHeight=texture->height;
                conversion->high=0x43300000;
                conversion->low=textureHeight;
                height=static_cast<f32>(textureHeight);
                width=static_cast<f32>(textureWidth);
                projectionWidth=projection->width;
            }
            f32 inverseWidth=gabi::fdivs_ppc(0.5f,projectionWidth);
            f32 halfWidth=gabi::fmuls_ppc(width,0.5f);
            f32 halfHeight=gabi::fmuls_ppc(height,0.5f);
            f32 negativeWidth=-halfWidth;
            f32 negativeHeight=-halfHeight;
            f32 widthSpan=gabi::fsubs_ppc(halfWidth,negativeWidth);
            f32 heightCenter=gabi::fadds_ppc(negativeHeight,halfHeight);
            f32 heightSpan=gabi::fsubs_ppc(negativeHeight,halfHeight);
            f32 reciprocalWidth=gabi::fdivs_ppc(1.0f,widthSpan);
            f32 reciprocalHeight=gabi::fdivs_ppc(1.0f,heightSpan);
            f32 widthCenter=gabi::fadds_ppc(halfWidth,negativeWidth);
            heightCenter=gabi::fmuls_ppc(heightCenter,reciprocalHeight);
            f32 doubledWidth=gabi::fadds_ppc(reciprocalWidth,reciprocalWidth);
            widthCenter=gabi::fmuls_ppc(widthCenter,reciprocalWidth);
            f32 x=projection->x;
            f32 doubledHeight=gabi::fadds_ppc(reciprocalHeight,reciprocalHeight);
            source=coordinates[i].source;
            f32 horizontalScale=gabi::fmuls_ppc(doubledWidth,inverseWidth);
            f32 y=projection->y;
            f32 verticalScale=gabi::fmuls_ppc(doubledHeight,inverseHeight);
            if (source==4) {
                f32 paneWidth=size->width;
                f32 paneHeight=size->height;
                x=gabi::fadds_ppc(x,paneWidth);
                y=gabi::fadds_ppc(y,paneHeight);
            }
            f32 xOffset=gabi::fdivs_ppc(x,projectionWidth);
            xOffset=gabi::fdivs_ppc(xOffset,width);
            xOffset=gabi::fsubs_ppc(0.5f,xOffset);
            f32 yOffset=gabi::fdivs_ppc(y,projectionHeight);
            yOffset=gabi::fdivs_ppc(yOffset,height);
            yOffset=gabi::fadds_ppc(yOffset,0.5f);
            xOffset=gabi::fnmsubs(widthCenter,inverseWidth,xOffset);
            yOffset=gabi::fnmsubs(heightCenter,inverseHeight,yOffset);
            matrix->m[0][0]=horizontalScale; matrix->m[0][1]=0;
            matrix->m[0][2]=0; matrix->m[0][3]=xOffset;
            matrix->m[1][0]=0; matrix->m[1][1]=verticalScale;
            matrix->m[1][2]=0; matrix->m[1][3]=yOffset;
            matrix->m[2][0]=0; matrix->m[2][1]=0;
            matrix->m[2][2]=0; matrix->m[2][3]=1;
            // Paired-single concatenation keeps each lane's multiply/add sequence.
            f32 affine[3][4];
            for (u32 row=0;row<3;++row)
                for (u32 column=0;column<4;++column) affine[row][column]=context->affineB.m[row][column];
            f32 result[3][4];
            f32 last[2]={sMaterialProjectionLastRow.values[0],sMaterialProjectionLastRow.values[1]};
            for (u32 row=0;row<3;++row) {
                f32 a=matrix->m[row][0],b=matrix->m[row][1];
                f32 c=matrix->m[row][2],d=matrix->m[row][3];
                for (u32 column=0;column<4;++column) {
                    f32 value=gabi::fmuls_ppc(affine[0][column],a);
                    value=gabi::fmadds(affine[1][column],b,value);
                    value=gabi::fmadds(affine[2][column],c,value);
                    if (column>=2) value=gabi::fmadds(last[column-2],d,value);
                    result[row][column]=value;
                }
            }
            matrix->m[3][3]=1;
            for (u32 row=0;row<3;++row)
                for (u32 column=0;column<4;++column) matrix->m[row][column]=result[row][column];
            matrix->m[3][1]=0; matrix->m[3][0]=0; matrix->m[3][2]=0;
            u32 location=context->uniformLocation(i+14);
            materialTransformUniforms(location,16,matrix.get());
        }
        if (i+1<count) textureCapacity=static_cast<u32>(layoutFlags)>>30;
    }
    for (u32 i=count;i<3;++i) projected->values[i]=0;
    u32 location=context->uniformLocation(17);
    gabi::FrameLocal<MaterialProjectionUniform> uniform(0x88);
    uniform->values[0]=static_cast<u32>(projected->values[0]);
    uniform->values[1]=static_cast<u32>(projected->values[1]);
    uniform->values[2]=static_cast<u32>(projected->values[2]);
    uniform->values[3]=0;
    materialTransformUniforms(location,4,uniform.get());
}
/* WWHD glue
data sMaterialProjectionLastRow 10181A10
ext C0006A40 void materialTransformUniforms(u32,u32,const void*)
*/
