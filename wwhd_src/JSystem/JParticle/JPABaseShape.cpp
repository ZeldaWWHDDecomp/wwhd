/** GameCube JPA1 base shape, with the HD gamma conversion and cached colors. */
#include "bindings.h"
#include "JSystem/JParticle/JPABaseShape.h"
static u32 sVtable;
static JPABaseShapeStaticBlock sStaticBlock;
static u32 sStaticRegister;
static JPABaseBlendModeTable stBlendMode;
static JPABaseBlendFactorTable stBlendFactor;
static JPABaseCompareTable stCompare;
// HD: small color aggregates are returned packed in r3.
static u32 packJPAColor(const GXColor* color) {
    return ((u32)color->r << 24) | ((u32)color->g << 16) | ((u32)color->b << 8) | (u8)color->a;
}
/* 0284A934 */
f64 JPABaseColorComponent(u32 component) {
    f32 normalized = (f32)component / 255.0f;
    f64 result = JPABasePow(normalized,2.200000047683716);
    if (result < 0.0) result = 0.0;
    else if (result > 1.0) result = 1.0;
    return result;
}
/* 0284A9B4 */
void JPABaseConvertColor(GXColor* color) {
    color->r = (u8)gabi::ftoi((f32)(JPABaseColorComponent(color->r) * 255.0));
    color->g = (u8)gabi::ftoi((f32)(JPABaseColorComponent(color->g) * 255.0));
    color->b = (u8)gabi::ftoi((f32)(JPABaseColorComponent(color->b) * 255.0));
}
/* 0284AA5C */
GXColor* makeColorTable(JPAColorRegAnmKey* pKey,s32 keyNum,s32 frmNum,JKRHeap* heap) {
    GXColor* colTbl = (GXColor*)JPABaseAllocColorTable(heap,(frmNum + 1) * 4,4);
    if (colTbl == NULL) JPABaseShapeAssert("JPABaseShape.cpp",0x5d,"colTbl");
    f32 redStep = 0.0f,greenStep = 0.0f,blueStep = 0.0f,alphaStep = 0.0f;
    f32 red = (u8)pKey[0].mColor.r;
    f32 green = (u8)pKey[0].mColor.g;
    f32 blue = (u8)pKey[0].mColor.b;
    f32 alpha = (u8)pKey[0].mColor.a;
    s32 key = 0;
    for (s32 frame = 0; frame < frmNum + 1; ++frame) {
        if ((s16)pKey[key].mFrame == frame) {
            colTbl[frame].r = pKey[key].mColor.r;
            colTbl[frame].g = pKey[key].mColor.g;
            colTbl[frame].b = pKey[key].mColor.b;
            colTbl[frame].a = pKey[key].mColor.a;
            JPABaseConvertColor(&colTbl[frame]);
            red = (u8)pKey[key].mColor.r;
            green = (u8)pKey[key].mColor.g;
            blue = (u8)pKey[key].mColor.b;
            alpha = (u8)pKey[key].mColor.a;
            ++key;
            if (key < keyNum) {
                f32 inverse = 1.0f / (f32)((s16)pKey[key].mFrame - (s16)pKey[key-1].mFrame);
                redStep = ((f32)(u8)pKey[key].mColor.r - red) * inverse;
                greenStep = ((f32)(u8)pKey[key].mColor.g - green) * inverse;
                blueStep = ((f32)(u8)pKey[key].mColor.b - blue) * inverse;
                alphaStep = ((f32)(u8)pKey[key].mColor.a - alpha) * inverse;
            } else {
                redStep = greenStep = blueStep = alphaStep = 0.0f;
            }
        } else {
            red += redStep;
            blue += blueStep;
            green += greenStep;
            alpha += alphaStep;
            colTbl[frame].r = (u8)gabi::ftoi(red);
            colTbl[frame].g = (u8)gabi::ftoi(green);
            colTbl[frame].b = (u8)gabi::ftoi(blue);
            colTbl[frame].a = (u8)gabi::ftoi(alpha);
            JPABaseConvertColor(&colTbl[frame]);
        }
    }
    return colTbl;
}
/* 0284AE48 */
static JPABaseShapeArc* JPABaseShapeArc__ct(JPABaseShapeArc* object,const u8* data,JKRHeap* heap) {
    if (object == NULL) object = (JPABaseShapeArc*)operator_new(sizeof(JPABaseShapeArc));
    if (object != NULL) {
        object->pBsd = (const JPABaseShapeData*)(data + 0xc);
        object->mVtable = &sVtable;
        object->mColLoopOffset = ((u32)object->pBsd->mFlags & 0x800) ? -1 : 0;
        u32 flags = object->pBsd->mFlags;
        object->mTexLoopOffset = (flags & 0x2000) ? -1 : 0;
        bool globalColor = (flags & 0x1000) || (flags & 15) == 5 || (flags & 15) == 6;
        bool globalTexture = (flags & 0x4000) || (flags & 15) == 5 || (flags & 15) == 6;
        object->mGlobalAnmFlags = ((u32)globalColor << 1) | (u32)globalTexture;
        object->mpTexAnmIdxArr = ((u8)object->pBsd->mTextureFlags & 1) && (u8)object->pBsd->mTextureAnmKeyNum != 0 ? (const be<u8>*)(data + 0x60) : NULL;
        if ((u8)object->pBsd->mColorFlags & 2) {
            u32 count = object->pBsd->prmAnmKeyNum;
            if (count == 0) {
                JPABaseShapeAssert("JPABaseShape.cpp",0x12a,"convertEndian(pBsd->prmAnmKeyNum) != 0");
                count = object->pBsd->prmAnmKeyNum;
            }
            JPAColorRegAnmKey* keys = (JPAColorRegAnmKey*)(data + (s16)object->pBsd->mPrmAnimDataOffs);
            object->mpPrmColorArr = makeColorTable(keys,count,object->pBsd->mColorRegAnmMaxFrm,heap);
        } else object->mpPrmColorArr = NULL;
        if ((u8)object->pBsd->mColorFlags & 8) {
            u32 count = object->pBsd->envAnmKeyNum;
            if (count == 0) {
                JPABaseShapeAssert("JPABaseShape.cpp",0x132,"convertEndian(pBsd->envAnmKeyNum) != 0");
                count = object->pBsd->envAnmKeyNum;
            }
            JPAColorRegAnmKey* keys = (JPAColorRegAnmKey*)(data + (s16)object->pBsd->mEnvAnimDataOffs);
            object->mpEnvColorArr = makeColorTable(keys,count,object->pBsd->mColorRegAnmMaxFrm,heap);
        } else object->mpEnvColorArr = NULL;
        object->mPrmColor = object->pBsd->mPrmColor;
        object->mEnvColor = object->pBsd->mEnvColor;
        JPABaseConvertColor(&object->mPrmColor);
        JPABaseConvertColor(&object->mEnvColor);
    }
    return object;
}
/* 0284B0C4 */
static void __sinit_JPABaseShape_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    JPABaseShapeConstructRegister(&sStaticRegister);
}

/* 0284B0EC */
u8 JPABaseShapeArc::getType() { return (u32)pBsd->mFlags & 15; }

/* 0284B0FC */
u8 JPABaseShapeArc::getDirType() { return ((u32)pBsd->mFlags >> 4) & 7; }

/* 0284B10C */
u8 JPABaseShapeArc::getRotType() { return ((u32)pBsd->mFlags >> 7) & 7; }

/* 0284B11C */
u8 JPABaseShapeArc::getBasePlaneType() { return ((u32)pBsd->mFlags >> 10) & 1; }

/* 0284B12C */
f32 JPABaseShapeArc::getBaseSizeX() { return pBsd->mBaseSizeX; }

/* 0284B138 */
f32 JPABaseShapeArc::getBaseSizeY() { return pBsd->mBaseSizeY; }

/* 0284B144 */
s16 JPABaseShapeArc::getLoopOffset() { return pBsd->mLoopOffset; }

/* 0284B150 */
s16 JPABaseShapeArc::getColLoopOffset() { return mColLoopOffset; }

/* 0284B158 */
s16 JPABaseShapeArc::getTexLoopOffset() { return mTexLoopOffset; }

/* 0284B160 */
u32 JPABaseShapeArc::isEnableGlobalColAnm() { return (u8)mGlobalAnmFlags & 2; }

/* 0284B16C */
u32 JPABaseShapeArc::isEnableGlobalTexAnm() { return (u8)mGlobalAnmFlags & 1; }

/* 0284B178 */
u32 JPABaseShapeArc::getListOrder() { return (u32)pBsd->mFlags & 0x200000; }

/* 0284B188 */
u32 JPABaseShapeArc::getChildOrder() { return (u32)pBsd->mFlags & 0x400000; }

/* 0284B198 */
u32 JPABaseShapeArc::getTevMode() { return (((u32)pBsd->mFlags >> 14) & 14) | (((u32)pBsd->mFlags >> 18) & 1); }

/* 0284B1AC */
u32 JPABaseShapeArc::getBlendMode1() { return stBlendMode.mEntries[(u16)pBsd->mBlendFlags & 3]; }

/* 0284B1C8 */
u32 JPABaseShapeArc::getSrcBlendFactor1() { return stBlendFactor.mEntries[((u16)pBsd->mBlendFlags >> 2) & 15]; }

/* 0284B1E4 */
u32 JPABaseShapeArc::getDstBlendFactor1() { return stBlendFactor.mEntries[((u16)pBsd->mBlendFlags >> 6) & 15]; }

/* 0284B200 */
u32 JPABaseShapeArc::isEnableZCmp() { return (u8)pBsd->mZFlags & 1; }

/* 0284B210 */
u32 JPABaseShapeArc::getZCmpFunction() { return stCompare.mEntries[((u8)pBsd->mZFlags >> 1) & 7]; }

/* 0284B22C */
u32 JPABaseShapeArc::isEnableZCmpUpdate() { return ((u8)pBsd->mZFlags >> 4) & 1; }

/* 0284B23C */
u32 JPABaseShapeArc::getAlphaCmpComp0() { return stCompare.mEntries[(u8)pBsd->mAlphaFlags & 7]; }

/* 0284B258 */
u8 JPABaseShapeArc::getAlphaCmpRef0() { return pBsd->mAlphaCmpRef0; }

/* 0284B264 */
u32 JPABaseShapeArc::isEnableAnmTone() { return (u32)pBsd->mFlags & 0x80000; }

/* 0284B274 */
u32 JPABaseShapeArc::isEnableProjection() { return (u32)pBsd->mFlags & 0x100000; }

/* 0284B284 */
u32 JPABaseShapeArc::isEnableTextureAnm() { return (u32)pBsd->mTextureFlags & 0x1; }

/* 0284B294 */
u32 JPABaseShapeArc::textureIsEmpty() { return !((u8)pBsd->mTextureFlags & 2); }

/* 0284B2A8 */
u8 JPABaseShapeArc::getTextureAnmType() { return ((u8)pBsd->mTextureFlags >> 2) & 7; }

/* 0284B2B8 */
u8 JPABaseShapeArc::getTextureAnmKeyNum() { return pBsd->mTextureAnmKeyNum; }

/* 0284B2C4 */
u8 JPABaseShapeArc::getTextureIndex() { return pBsd->mTextureIndex; }

/* 0284B2D0 */
u8 JPABaseShapeArc::getTextureIndex(u32 index) { return mpTexAnmIdxArr[index]; }

/* 0284B2DC */
u32 JPABaseShapeArc::isEnablePrm() { return (u8)pBsd->mColorFlags & 1; }

/* 0284B2EC */
u32 JPABaseShapeArc::isEnableEnv() { return (u8)pBsd->mColorFlags & 4; }

/* 0284B2FC */
u32 JPABaseShapeArc::isEnablePrmAnm() { return (u8)pBsd->mColorFlags & 2; }

/* 0284B30C */
u32 JPABaseShapeArc::isEnableEnvAnm() { return (u8)pBsd->mColorFlags & 8; }

/* 0284B31C */
u32 JPABaseShapeArc::getPrmColor() { return packJPAColor(&mPrmColor); }

/* 0284B33C */
u32 JPABaseShapeArc::getEnvColor() { return packJPAColor(&mEnvColor); }

/* 0284B35C */
u32 JPABaseShapeArc::getPrmColor(u32 index) { return packJPAColor(&mpPrmColorArr[index]); }

/* 0284B384 */
u32 JPABaseShapeArc::getEnvColor(u32 index) { return packJPAColor(&mpEnvColorArr[index]); }

/* 0284B3AC */
u8 JPABaseShapeArc::getColorRegAnmType() { return ((u8)pBsd->mColorFlags >> 4) & 7; }

/* 0284B3BC */
s16 JPABaseShapeArc::getColorRegAnmMaxFrm() { return pBsd->mColorRegAnmMaxFrm; }

/* 0284B3C8 */
u32 JPABaseShapeArc::isEnableTexScrollAnm() { return (u32)pBsd->mFlags & 0x1000000; }

/* 0284B3D8 */
f32 JPABaseShapeArc::getTilingX() { return pBsd->mTilingX; }

/* 0284B3E4 */
f32 JPABaseShapeArc::getTilingY() { return pBsd->mTilingY; }

/* 0284B3F0 */
f32 JPABaseShapeArc::getTexStaticTransX() { return pBsd->mTexStaticTransX; }

/* 0284B3FC */
f32 JPABaseShapeArc::getTexStaticTransY() { return pBsd->mTexStaticTransY; }

/* 0284B408 */
f32 JPABaseShapeArc::getTexStaticScaleX() { return pBsd->mTexStaticScaleX; }

/* 0284B414 */
f32 JPABaseShapeArc::getTexStaticScaleY() { return pBsd->mTexStaticScaleY; }

/* 0284B420 */
f32 JPABaseShapeArc::getTexScrollTransX() { return pBsd->mTexScrollTransX; }

/* 0284B42C */
f32 JPABaseShapeArc::getTexScrollTransY() { return pBsd->mTexScrollTransY; }

/* 0284B438 */
f32 JPABaseShapeArc::getTexScrollScaleX() { return pBsd->mTexScrollScaleX; }

/* 0284B444 */
f32 JPABaseShapeArc::getTexScrollScaleY() { return pBsd->mTexScrollScaleY; }

/* 0284B450 */
f32 JPABaseShapeArc::getTexScrollRotate() { return pBsd->mTexScrollRotate; }

/* WWHD glue
 data sVtable 1017B364
 data sStaticBlock 104B6724
 data sStaticRegister 101FC7A8
 data stBlendMode 101FC7B4
 data stBlendFactor 101FC7C4
 data stCompare 101FC7EC
 ext 028F026C void JPABaseShapeConstructRegister(void*)
 ext 028F4560 f64 JPABasePow(f64,f64)
 ext 027EC0D8 void* JPABaseAllocColorTable(JKRHeap*,u32,s32)
 ext 0273AA24 void JPABaseShapeAssert(const char*,s32,const char*)
 str 1017B348 "colTbl"
 str 1017B350 "JPABaseShape.cpp" in 0284AA5C
 str 1017B574 "JPABaseShape.cpp" in 0284AE48
 str 1017B588 "convertEndian(pBsd->prmAnmKeyNum) != 0"
 str 1017B5B0 "convertEndian(pBsd->envAnmKeyNum) != 0"
*/
