/* Picture texture state, following tww J2DPicture. Cafe omits GX submission. */
#include "JSystem/J2DGraph/J2DPicture.h"
static f32 sZero=0.0f;
static f32 sOne=1.0f;
static f32 sByteScale=255.0f;
static f32 sHalf=0.5f;
struct PictureStaticState { be<u32> words[4]; };
static PictureStaticState sStaticState;
static u8 sStaticRecord;
static J2DPaneVtable sPictureVtable;
static u32 blendPictureWeights(const be<f32>* weights,u8 count);
/* 027EEE8C */
void J2DPicture::setBlendKonstColor() { mBlendKonstColor.rgba=blendPictureWeights(mBlendKonstColorF,mNumTexture); }
static u32 blendPictureWeights(const be<f32>* weights,u8 count) {
    u32 packed=0;
    for(u32 i=1;i<count;++i) {
        f32 sum=sZero;
        for(u32 j=0;j<i;++j) sum=gabi::fadds_ppc(sum,weights[j]);
        f32 denominator=gabi::fadds_ppc(sum,weights[i]);
        if(denominator!=sZero) {
            f32 ratio=static_cast<f32>(sum/denominator);
            f32 value=gabi::fmuls_ppc(gabi::fsubs_ppc(sOne,ratio),sByteScale);
            u32 shift=(i-1)*8;
            if((shift & 63)<32) packed|=static_cast<u8>(gabi::ftoi(value))<<(shift&31);
        }
    }
    return packed;
}
/* 027EEF50 */
void J2DPicture::setBlendKonstAlpha() { mBlendKonstAlpha.rgba=blendPictureWeights(mBlendKonstAlphaF,mNumTexture); }
/* 027EF014 */
void J2DPicture::initinfo() {
    mBinding=15;f32 one=sOne;mMagic=0x50494331;mFlag=0;
    mBlendKonstColorF[1]=one;mBlendKonstColorF[0]=one;mBlendKonstColorF[2]=one;mBlendKonstColorF[3]=one;
    setBlendKonstColor();
    mBlendKonstAlphaF[0]=one;mBlendKonstAlphaF[2]=one;mBlendKonstAlphaF[3]=one;mBlendKonstAlphaF[1]=one;
    setBlendKonstAlpha();
    mColorBlack.rgba=0;mColorWhite.rgba=0xFFFFFFFF;
    for(u32 i=0;i<4;++i) mCornerColor[i].rgba=0xFFFFFFFF;
}
/* 027EF0F4 */
bool J2DPicture::insert(const ResTIMG* image,u32 index,f32 ratio) {
    if(!image || mNumTexture>=4 || index>=4 || index>mNumTexture) return false;
    JUTTexture* texture=static_cast<JUTTexture*>(pictureAllocate(sizeof(JUTTexture)));
    if(texture) {
        texture->mEmbPalette=nullptr;texture->storeTIMG(image,0);
        texture->mFlags=texture->mFlags & 2;
    }
    // lfs/stfs preserve signaling-NaN payloads; host floating conversions may quiet them.
    auto* colorWords=static_cast<be<u32>*>(static_cast<void*>(mBlendKonstColorF));
    auto* alphaWords=static_cast<be<u32>*>(static_cast<void*>(mBlendKonstAlphaF));
    for(u32 i=3;i>index;--i) {
        mpTexture[i]=mpTexture[i-1].get();
        colorWords[i]=static_cast<u32>(colorWords[i-1]);
        alphaWords[i]=static_cast<u32>(alphaWords[i-1]);
    }
    u32 mask=1u<<index,lower=mask-1,valid=mValidTexture;
    mValidTexture=(valid & lower)|((valid & ~lower)<<1);
    mpTexture[index]=texture;mValidTexture=mValidTexture | mask;
    mBlendKonstColorF[index]=ratio;mBlendKonstAlphaF[index]=ratio;
    u8 count=mNumTexture;
    if(count==0) {
        JUTTexture* first=mpTexture[0];
        if(first) {
            const ResTIMG* info=first->mTexInfo;
            f32 width=static_cast<u16>(info->width),height=static_cast<u16>(info->height);
            J2DPaneVtable* table=__vtbl;f32 zero=sZero;
            mBounds.i.x=zero;mBounds.i.y=zero;mBounds.f.x=width;mBounds.f.y=height;
            gabi::call_ptr<void>(table->calcMtx,this);count=mNumTexture;
        }
    }
    mNumTexture=count+1;setBlendKonstColor();setBlendKonstAlpha();return true;
}
static void constructPictureFields(J2DPicture* picture) {
    picturePaneConstruct(picture);
    picture->__vtbl=&sPictureVtable;
    pictureClear(picture->mpTexture,16);
    picture->mValidTexture=0;picture->mFlag=0;picture->mNumTexture=0;picture->mBinding=0;
    pictureClear(picture->mBlendKonstColorF,16);pictureClear(picture->mBlendKonstAlphaF,16);
    picture->mpPalette=nullptr;picture->mColorWhite.rgba=0xFFFFFFFF;picture->mColorBlack.rgba=0xFFFFFFFF;
    pictureConstructArray(picture->mCornerColor,4,4,gabi::guest_fn((const void*)pictureColorConstruct));
    picture->mBlendKonstAlpha.rgba=0xFFFFFFFF;picture->mpTexture[0]=nullptr;picture->mNumTexture=0;picture->mBlendKonstColor.rgba=0xFFFFFFFF;
}
/* 027EF308 */
J2DPicture* J2DPicture__ct_image(J2DPicture* picture,const ResTIMG* image) {
    if(!picture) picture=static_cast<J2DPicture*>(pictureAllocate(sizeof(J2DPicture)));
    if(!picture) return nullptr;
    constructPictureFields(picture);
    if(image) picture->insert(image,0,sOne);
    picture->mpPalette=nullptr;picture->initinfo();return picture;
}
/* 027EF414 */
bool J2DPicture::insert(const char* name,u32 index,f32 ratio) {
    const ResTIMG* image=pictureFindResource(name,nullptr);
    return insert(image,index,ratio);
}
/* 027EF484 */
J2DPicture* J2DPicture__ct_name(J2DPicture* picture,const char* name) {
    if(!picture) picture=static_cast<J2DPicture*>(pictureAllocate(sizeof(J2DPicture)));
    if(!picture) return nullptr;
    constructPictureFields(picture);
    if(name) picture->insert(name,0,sOne);
    picture->mpPalette=nullptr;picture->initinfo();return picture;
}
struct PictureStreamFrame {
    be<u8> count,binding,flags,unused;
    J2DColor cornerCopy,colorInput,spare,colorTemporary,blackCopy,blackInput,whiteCopy,whiteInput,cornerTemporary[3];
    J2DPane::J2DScrnBlockHeader header;
    J2DColor extraColor[3];
    JUTResReference reference;
};
/* 027EF590 */
J2DPicture* J2DPicture__ct_stream(J2DPicture* picture,J2DPane* parent,JSURandomInputStream* stream) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    if(!picture) picture=static_cast<J2DPicture*>(pictureAllocate(sizeof(J2DPicture)));
    if(!picture) return nullptr;
    constructPictureFields(picture);
    s32 position=gabi::call_ptr<s32>(stream->mVtable->mPosition,stream);
    gabi::FrameLocal<PictureStreamFrame> frame(8);
    pictureRead(stream,&frame->header,8);
    picture->mMagic=static_cast<u32>(frame->header.magic);
    u32 end=static_cast<u32>(position)+static_cast<u32>(frame->header.size);
    pictureMakePaneStream(picture,parent,stream);
    frame->reference.mType=0;
    pictureRead(stream,&frame->count,1);
    u8 remaining=frame->count;
    const ResTIMG* image=static_cast<const ResTIMG*>(pictureGetResource(&frame->reference,stream,0x54494D47,nullptr));
    ResTLUT* paletteInfo=static_cast<ResTLUT*>(pictureGetResource(&frame->reference,stream,0x544C5554,nullptr));
    pictureRead(stream,&frame->binding,1);
    remaining=static_cast<u8>(remaining-3);picture->mBinding=static_cast<u8>(frame->binding);
    if(remaining) {
        pictureRead(stream,&frame->flags,1);remaining=static_cast<u8>(remaining-1);picture->mFlag=static_cast<u8>(frame->flags);
        if(remaining) { pictureRead(stream,&frame->unused,1);remaining=static_cast<u8>(remaining-1); }
    } else picture->mFlag=0;
    picture->mColorBlack.rgba=0;picture->mColorWhite.rgba=0xFFFFFFFF;
    if(remaining) { pictureRead(stream,&frame->blackInput,4);picture->mColorBlack.rgba=static_cast<u32>(frame->blackInput.rgba);remaining=static_cast<u8>(remaining-1); }
    if(remaining) { pictureRead(stream,&frame->whiteInput,4);picture->mColorWhite.rgba=static_cast<u32>(frame->whiteInput.rgba);remaining=static_cast<u8>(remaining-1); }
    for(u32 i=0;i<4;++i) picture->mCornerColor[i].rgba=0xFFFFFFFF;
    for(u32 i=0;remaining && i<4;++i) {
        pictureRead(stream,&frame->colorInput,4);remaining=static_cast<u8>(remaining-1);
        picture->mCornerColor[i].rgba=static_cast<u32>(frame->colorInput.rgba);
    }
    pictureSeek(stream,end,0);
    picture->mpTexture[0]=nullptr;picture->mNumTexture=0;picture->mValidTexture=1;
    if(image) {
        JUTTexture* texture=static_cast<JUTTexture*>(pictureAllocate(sizeof(JUTTexture)));
        if(texture) { texture->mEmbPalette=nullptr;texture->storeTIMG(image,0);texture->mFlags=texture->mFlags & 2; }
        u8 count=picture->mNumTexture;picture->mpTexture[0]=texture;picture->mNumTexture=count+1;
    }
    if(paletteInfo) {
        JUTPalette* palette=static_cast<JUTPalette*>(pictureAllocate(sizeof(JUTPalette)));
        if(palette) pictureStorePalette(palette,0,paletteInfo);
        JUTTexture* texture=picture->mpTexture[0];picture->mpPalette=palette;
        pictureAttachPalette(texture,palette);
    }
    f32 one=sOne;
    picture->mBlendKonstColorF[2]=one;picture->mBlendKonstColorF[3]=one;picture->mBlendKonstColorF[1]=one;picture->mBlendKonstColorF[0]=one;
    picture->setBlendKonstColor();
    picture->mBlendKonstAlphaF[0]=one;picture->mBlendKonstAlphaF[3]=one;picture->mBlendKonstAlphaF[1]=one;picture->mBlendKonstAlphaF[2]=one;
    picture->setBlendKonstAlpha();return picture;
}
/* 027EF9AC */
static void J2DPicture__dt(J2DPicture* picture,u32 flags) {
    if(!picture) return;
    u8 count=picture->mNumTexture;
    picture->__vtbl=&sPictureVtable;
    for(u32 i=0;i<count;++i) {
        if(picture->mValidTexture & (1u<<i)) { pictureTextureDestroy(picture->mpTexture[i],3);count=picture->mNumTexture; }
    }
    pictureDelete(picture->mpPalette);picturePaneDestroy(picture,0);
    if(flags&1) pictureDelete(picture);
}
/* 027EFA58 */
void J2DPicture::loadTextures() {
    // HD: this entry point only loads palettes; the matcher called it remove.
    if(mVisible) for(u8 i=0;i<mNumTexture;++i) mpTexture[i]->load();
}
/* 027EFAD4 */
const ResTIMG* J2DPicture::changeTexture(const ResTIMG* image,u32 index) {
    u8 count=mNumTexture;
    if(index>=count || !mpTexture[index] || !image) return nullptr;
    JUTTexture* texture=mpTexture[index];
    const ResTIMG* old=texture->mTexInfo;
    texture->storeTIMG(image,0);return old;
}
/* 027EFB68 */
const ResTIMG* J2DPicture::changeTexture(const char* name,u32 index) {
    const ResTIMG* image=pictureFindResource(name,nullptr);
    return changeTexture(image,index);
}
/* 027EFBB8 */
void J2DPicture::swap(be<f32>* first,be<f32>* second) {
    u32 b=static_cast<be<u32>*>(static_cast<void*>(second))->operator u32();
    u32 a=static_cast<be<u32>*>(static_cast<void*>(first))->operator u32();
    *static_cast<be<u32>*>(static_cast<void*>(first))=b;
    *static_cast<be<u32>*>(static_cast<void*>(second))=a;
}
struct PictureFloatSlot {
    union { be<f64> value;struct { be<f32> rounded;be<u32> low; }; };
};
struct PictureFloatStack { PictureFloatSlot slots[4]; };
/* 027EFBCC */
void J2DPicture::drawTexCoord(f32,f32,f32,f32,f32,f32,f32,f32,const Mtx34*) {
    // HD: the GHS prologue narrows the four overflow floating arguments in the caller's parameter area.
    PictureFloatStack* arguments=gabi::at<PictureFloatStack>(gabi::cpu->r[1]+8);
    f32 first=static_cast<f32>(static_cast<f64>(arguments->slots[0].value));
    f32 second=static_cast<f32>(static_cast<f64>(arguments->slots[1].value));
    f32 third=static_cast<f32>(static_cast<f64>(arguments->slots[2].value));
    arguments->slots[0].rounded=first;arguments->slots[1].rounded=second;
    f32 fourth=static_cast<f32>(static_cast<f64>(arguments->slots[3].value));
    arguments->slots[2].rounded=third;arguments->slots[3].rounded=fourth;
    for(u8 i=0;i<mNumTexture;++i) mpTexture[i]->load();
}
/* 027EFC6C */
void J2DPicture::drawFullSet(f32,f32,f32 width,f32 height,u32 binding,u32 mirror,u32 rotate90,const Mtx34* matrix) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    bool left,right,top,bottom;
    if(!rotate90) {
        left=binding & ((mirror&2)?4:8);right=binding & ((mirror&2)?8:4);
        top=binding & ((mirror&1)?1:2);bottom=binding & ((mirror&1)?2:1);
    } else {
        left=binding & ((mirror&2)?1:2);right=binding & ((mirror&2)?2:1);
        top=binding & ((mirror&1)?8:4);bottom=binding & ((mirror&1)?4:8);
    }
    const ResTIMG* info=mpTexture[0]->mTexInfo;
    f32 textureWidth=static_cast<u16>(info->width),textureHeight=static_cast<u16>(info->height);
    f32 rectWidth=rotate90?height:width,rectHeight=rotate90?width:height;
    f32 zero=sZero,one=sOne,half=sHalf;
    struct TexCoords { be<f32> s,t,right,bottom; };
    gabi::FrameLocal<TexCoords> coord(0x28);
    if(left) { coord->s=zero;coord->right=right?one:static_cast<f32>(rectWidth/textureWidth); }
    else {
        f32 ratio=static_cast<f32>(rectWidth/textureWidth);
        if(right) { coord->right=one;coord->s=gabi::fsubs_ppc(one,ratio); }
        else { f32 h=gabi::fmuls_ppc(ratio,half);coord->s=gabi::fsubs_ppc(half,h);coord->right=gabi::fadds_ppc(h,half); }
    }
    if(top) { coord->t=zero;coord->bottom=bottom?one:static_cast<f32>(rectHeight/textureHeight); }
    else {
        f32 ratio=static_cast<f32>(rectHeight/textureHeight);
        if(bottom) { coord->bottom=one;coord->t=gabi::fsubs_ppc(one,ratio); }
        else { f32 h=gabi::fmuls_ppc(ratio,half);coord->t=gabi::fsubs_ppc(half,h);coord->bottom=gabi::fadds_ppc(h,half); }
    }
    if(mirror&2) swap(&coord->s,&coord->right);
    if(mirror&1) swap(&coord->t,&coord->bottom);
    f32 t=coord->t,b=coord->bottom,r=coord->right,l=coord->s;
    gabi::FrameLocal<PictureFloatStack> overflow(8);
    if(!rotate90) {
        overflow->slots[0].value=l;overflow->slots[1].value=b;overflow->slots[2].value=r;overflow->slots[3].value=b;
        drawTexCoord(zero,zero,width,height,l,t,r,t,matrix);
    } else {
        overflow->slots[0].value=r;overflow->slots[1].value=b;overflow->slots[2].value=r;overflow->slots[3].value=t;
        drawTexCoord(zero,zero,width,height,l,b,l,t,matrix);
    }
}
/* 027F0074 */
void J2DPicture::drawSelf(f32 x,f32 y,const Mtx34* matrix) {
    if(!mpTexture[0]) return;
    f32 right=mBounds.f.x,bottom=mBounds.f.y,left=mBounds.i.x,top=mBounds.i.y;
    f32 width=gabi::fsubs_ppc(right,left),height=gabi::fsubs_ppc(bottom,top);
    f32 globalY=mGlobalBounds.i.y,globalX=mGlobalBounds.i.x;
    u8 flags=mFlag;
    drawFullSet(gabi::fadds_ppc(globalX,x),gabi::fadds_ppc(globalY,y),width,height,mBinding,flags&3,(flags>>2)&1,matrix);
}
/* 027F00C0 */
static void __sinit_J2DPicture_cpp() {
    sStaticState.words[2]=0;sStaticState.words[0]=0;sStaticState.words[3]=0;sStaticState.words[1]=0;
    pictureRegisterStatic(&sStaticRecord);
}
/* WWHD glue
data sPictureVtable 1016DFAC
data sZero 1016DF88
data sByteScale 1016DF8C
data sOne 1016DF90
data sHalf 1016DFA0
data sStaticState 104B4548
data sStaticRecord 101F9850
ext 02842C14 s32 pictureRead(JSUInputStream*,void*,s32)
ext 02842D00 s32 pictureSeek(JSURandomInputStream*,s32,u32)
ext 027EE29C void pictureMakePaneStream(J2DPane*,J2DPane*,JSURandomInputStream*)
ext 02843104 void* pictureGetResource(JUTResReference*,JSUInputStream*,u32,void*)
ext 02842FBC void pictureStorePalette(JUTPalette*,u32,ResTLUT*)
ext 028431C8 void pictureAttachPalette(JUTTexture*,JUTPalette*)
ext 0273AD10 void* pictureAllocate(u32)
ext 0273AF40 void pictureDelete(void*)
ext 02843374 void pictureTextureDestroy(JUTTexture*,u32)
ext 027EE0B8 J2DPane* picturePaneConstruct(J2DPane*)
ext 027EE6F0 void picturePaneDestroy(J2DPane*,u32)
ext 028F521C void pictureClear(void*,u32)
ext 028EFFD0 void pictureConstructArray(void*,u32,u32,u32)
ext 027F010C J2DColor* pictureColorConstruct(J2DColor*) callback
ext 027EBF60 const ResTIMG* pictureFindResource(const char*,void*)
ext 028F026C void pictureRegisterStatic(void*)
ext 028431FC void JUTTexture::storeTIMG(const ResTIMG*,u32)
ext 028433E4 u32 JUTTexture::load()
*/
