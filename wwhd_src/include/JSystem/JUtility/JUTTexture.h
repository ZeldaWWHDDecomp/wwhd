#pragma once
#include "wwhd.h"
struct JUTTextureStaticBlock { be<u32> mWords[4]; };
WWHD_SIZE(JUTTextureStaticBlock,16);
void JUTTextureConstructRegister(void*);


#include "JSystem/JUtility/JUTPalette.h"
#include "JSystem/JUtility/ResTIMG.h"
class JUTTexture {
public:
    void initTexObj(u32);
    void attachPalette(JUTPalette*);
    void storeTIMG(const ResTIMG*,u32);
    u32 load();
    u8 mTexObj[0x40];
    gptr<const ResTIMG> mTexInfo;
    gptr<void> mTexData;
    gptr<JUTPalette> mEmbPalette;
    gptr<JUTPalette> mPalette;
    be<u8> mWrapS;
    be<u8> mWrapT;
    be<u8> mMinFilter;
    be<u8> mMagFilter;
    be<s16> mMinLOD;
    be<s16> mMaxLOD;
    be<s16> mLODBias;
    be<u8> mTlutName;
    be<u8> mFlags;
    gptr<void> mCaptureData;
};
WWHD_OFFSET(JUTTexture,mTexInfo,0x40);
WWHD_OFFSET(JUTTexture,mTexData,0x44);
WWHD_OFFSET(JUTTexture,mEmbPalette,0x48);
WWHD_OFFSET(JUTTexture,mPalette,0x4c);
WWHD_OFFSET(JUTTexture,mWrapS,0x50);
WWHD_OFFSET(JUTTexture,mMinLOD,0x54);
WWHD_OFFSET(JUTTexture,mTlutName,0x5a);
WWHD_OFFSET(JUTTexture,mFlags,0x5b);
WWHD_OFFSET(JUTTexture,mCaptureData,0x5c);
WWHD_SIZE(JUTTexture,0x60);
void JUTTextureDeleteArray(void*);
