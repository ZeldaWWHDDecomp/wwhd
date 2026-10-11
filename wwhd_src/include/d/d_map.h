/* Map rendering and room data. GameCube names with private WWHD layout views.
 * HD: GX texture objects grow to 64 bytes; screen, texture, room and dMap_2DT2_c records grow
 * accordingly. Inert GBA interfaces remain present with their verified HD signatures. */
#pragma once
#include "wwhd.h"
#include "gabi.h"
namespace dMapHD {

struct Color {
    /* 0x000 */ be<u8> rgba[4];
};
WWHD_SIZE(Color, 0x4);
WWHD_OFFSET(Color, rgba, 0x0);

struct dMap_2DMtMapSpcl_tex_c {
    void setScroll(f32 x, f32 y, f32 w, f32 h);
    void init(void* image, u32 index, void* color);
    /* 0x000 */ be<u8> mActive;
    /* 0x001 */ be<u8> mIndexed;
    /* 0x002 */ u8 mPadding2[2];
    /* 0x004 */ be<u8> mTexObj[64];
    /* 0x044 */ be<u8> mTlutObj[12];
    /* 0x050 */ Color mColor;
    /* 0x054 */ be<f32> mScrollX;
    /* 0x058 */ be<f32> mScrollY;
    /* 0x05C */ be<f32> mScrollWidth;
    /* 0x060 */ be<f32> mScrollHeight;
    /* 0x064 */ be<u32> mTlutName;
};
WWHD_SIZE(dMap_2DMtMapSpcl_tex_c, 0x68);
WWHD_OFFSET(dMap_2DMtMapSpcl_tex_c, mActive, 0x0);
WWHD_OFFSET(dMap_2DMtMapSpcl_tex_c, mIndexed, 0x1);
WWHD_OFFSET(dMap_2DMtMapSpcl_tex_c, mTexObj, 0x4);
WWHD_OFFSET(dMap_2DMtMapSpcl_tex_c, mTlutObj, 0x44);
WWHD_OFFSET(dMap_2DMtMapSpcl_tex_c, mColor, 0x50);
WWHD_OFFSET(dMap_2DMtMapSpcl_tex_c, mScrollX, 0x54);
WWHD_OFFSET(dMap_2DMtMapSpcl_tex_c, mScrollY, 0x58);
WWHD_OFFSET(dMap_2DMtMapSpcl_tex_c, mScrollWidth, 0x5C);
WWHD_OFFSET(dMap_2DMtMapSpcl_tex_c, mScrollHeight, 0x60);
WWHD_OFFSET(dMap_2DMtMapSpcl_tex_c, mTlutName, 0x64);

struct dMap_2DMtMapSpcl_c {
    void init(s32 num, void* tex);
    /* 0x000 */ be<u32> mVTable;
    /* 0x004 */ be<u8> mTextureCount;
    /* 0x005 */ be<u8> mBlendMode;
    /* 0x006 */ u8 mPadding6[2];
    /* 0x008 */ be<u32> mTextures;
    /* 0x00C */ be<u16> mLeft;
    /* 0x00E */ be<u16> mTop;
    /* 0x010 */ be<u16> mRight;
    /* 0x012 */ be<u16> mBottom;
};
WWHD_SIZE(dMap_2DMtMapSpcl_c, 0x14);
WWHD_OFFSET(dMap_2DMtMapSpcl_c, mVTable, 0x0);
WWHD_OFFSET(dMap_2DMtMapSpcl_c, mTextureCount, 0x4);
WWHD_OFFSET(dMap_2DMtMapSpcl_c, mBlendMode, 0x5);
WWHD_OFFSET(dMap_2DMtMapSpcl_c, mTextures, 0x8);
WWHD_OFFSET(dMap_2DMtMapSpcl_c, mLeft, 0xC);
WWHD_OFFSET(dMap_2DMtMapSpcl_c, mTop, 0xE);
WWHD_OFFSET(dMap_2DMtMapSpcl_c, mRight, 0x10);
WWHD_OFFSET(dMap_2DMtMapSpcl_c, mBottom, 0x12);

struct dMap_2DAGBScrDsp_c {
    void init(void* data, void* image, s32 x, s32 y, s32 width, s32 height, u32 alpha, f32 px,
              f32 py, f32 sx, f32 sy);
    void setImage(void* image, void* data);
    void draw();
    /* 0x000 */ be<u32> mVTable;
    /* 0x004 */ be<u32> mMapData;
    /* 0x008 */ be<u32> mImg;
    /* 0x00C */ be<u8> mTexObj[64];
    /* 0x04C */ be<u8> mTlutObj[12];
    /* 0x058 */ be<u32> mMapBytes;
    /* 0x05C */ be<u16> mLeft;
    /* 0x05E */ be<u16> mTop;
    /* 0x060 */ be<u16> mWidth;
    /* 0x062 */ be<u16> mHeight;
    /* 0x064 */ be<f32> mCenterX;
    /* 0x068 */ be<f32> mCenterY;
    /* 0x06C */ be<f32> mScaleX;
    /* 0x070 */ be<f32> mScaleY;
    /* 0x074 */ be<u8> mAlpha;
    /* 0x075 */ be<u8> mHDFlags;
    /* 0x076 */ be<u16> mHDState;
};
WWHD_SIZE(dMap_2DAGBScrDsp_c, 0x78);
WWHD_OFFSET(dMap_2DAGBScrDsp_c, mVTable, 0x0);
WWHD_OFFSET(dMap_2DAGBScrDsp_c, mMapData, 0x4);
WWHD_OFFSET(dMap_2DAGBScrDsp_c, mImg, 0x8);
WWHD_OFFSET(dMap_2DAGBScrDsp_c, mTexObj, 0xC);
WWHD_OFFSET(dMap_2DAGBScrDsp_c, mTlutObj, 0x4C);
WWHD_OFFSET(dMap_2DAGBScrDsp_c, mMapBytes, 0x58);
WWHD_OFFSET(dMap_2DAGBScrDsp_c, mLeft, 0x5C);
WWHD_OFFSET(dMap_2DAGBScrDsp_c, mTop, 0x5E);
WWHD_OFFSET(dMap_2DAGBScrDsp_c, mWidth, 0x60);
WWHD_OFFSET(dMap_2DAGBScrDsp_c, mHeight, 0x62);
WWHD_OFFSET(dMap_2DAGBScrDsp_c, mCenterX, 0x64);
WWHD_OFFSET(dMap_2DAGBScrDsp_c, mCenterY, 0x68);
WWHD_OFFSET(dMap_2DAGBScrDsp_c, mScaleX, 0x6C);
WWHD_OFFSET(dMap_2DAGBScrDsp_c, mScaleY, 0x70);
WWHD_OFFSET(dMap_2DAGBScrDsp_c, mAlpha, 0x74);
WWHD_OFFSET(dMap_2DAGBScrDsp_c, mHDFlags, 0x75);
WWHD_OFFSET(dMap_2DAGBScrDsp_c, mHDState, 0x76);

struct dMap_RoomInfo_c {
    bool Changeimage(u32 floor, u32 otherFloor, s32 currentRoom, s32 sx, s32 sy, f32 height);
    u32 getRoomImage(s32 room, u32 floor, s32 search, void* out0, void* out1, void* outData,
                     void* outInfo);
    void deleteRoom();
    u8 getRoomDspFloorNo(u32 floor, s32 search);
    /* 0x000 */ be<u8> m_exist;
    /* 0x001 */ be<u8> mEnableFlags;
    /* 0x002 */ be<u8> mFloorTable[10];
    /* 0x00C */ be<u8> mDspFloorNo;
    /* 0x00D */ u8 mPaddingD[3];
    /* 0x010 */ be<u32> m_no;
    /* 0x014 */ u8 mPadding14[4];
    /* 0x018 */ be<f32> mEnlargementScaleX;
    /* 0x01C */ be<f32> mEnlargementScaleZ;
    /* 0x020 */ be<f32> mRealSizeScaleX;
    /* 0x024 */ be<f32> mRealSizeScaleZ;
    /* 0x028 */ be<f32> mMapWidth;
    /* 0x02C */ be<f32> mMapHeight;
    /* 0x030 */ dMap_2DMtMapSpcl_c mEnlargement;
    /* 0x044 */ dMap_2DMtMapSpcl_tex_c mTexture;
    /* 0x0AC */ dMap_2DAGBScrDsp_c mScreen;
    /* 0x124 */ be<u32> m_next;
    /* 0x128 */ be<u32> mStageMapInfoP;
};
WWHD_SIZE(dMap_RoomInfo_c, 0x12C);
WWHD_OFFSET(dMap_RoomInfo_c, m_exist, 0x0);
WWHD_OFFSET(dMap_RoomInfo_c, mEnableFlags, 0x1);
WWHD_OFFSET(dMap_RoomInfo_c, mFloorTable, 0x2);
WWHD_OFFSET(dMap_RoomInfo_c, mDspFloorNo, 0xC);
WWHD_OFFSET(dMap_RoomInfo_c, m_no, 0x10);
WWHD_OFFSET(dMap_RoomInfo_c, mEnlargementScaleX, 0x18);
WWHD_OFFSET(dMap_RoomInfo_c, mEnlargementScaleZ, 0x1C);
WWHD_OFFSET(dMap_RoomInfo_c, mRealSizeScaleX, 0x20);
WWHD_OFFSET(dMap_RoomInfo_c, mRealSizeScaleZ, 0x24);
WWHD_OFFSET(dMap_RoomInfo_c, mMapWidth, 0x28);
WWHD_OFFSET(dMap_RoomInfo_c, mMapHeight, 0x2C);
WWHD_OFFSET(dMap_RoomInfo_c, mEnlargement, 0x30);
WWHD_OFFSET(dMap_RoomInfo_c, mTexture, 0x44);
WWHD_OFFSET(dMap_RoomInfo_c, mScreen, 0xAC);
WWHD_OFFSET(dMap_RoomInfo_c, m_next, 0x124);
WWHD_OFFSET(dMap_RoomInfo_c, mStageMapInfoP, 0x128);

struct dMap_RoomInfoCtrl_c {
    void checkFloorMoveImageChangeRoom(u32 a, u32 b, s32 c, s32 x, s32 y, f32 height);
    void* getNextRoomP(void* room);
    bool deleteRoom(s32 room);
    bool roomExistenceCheck(s32 no, void* out);
    /* 0x000 */ be<s32> m_num;
    /* 0x004 */ be<u32> m_info;
};
WWHD_SIZE(dMap_RoomInfoCtrl_c, 0x8);
WWHD_OFFSET(dMap_RoomInfoCtrl_c, m_num, 0x0);
WWHD_OFFSET(dMap_RoomInfoCtrl_c, m_info, 0x4);

struct dMap_2DSQ_c {
    /* 0x000 */ be<u32> mVTable;
    /* 0x004 */ be<u32> mDispLeft;
    /* 0x008 */ be<u32> mDispRight;
    /* 0x00C */ be<u32> mDispTop;
    /* 0x010 */ be<u32> mDispBottom;
    /* 0x014 */ be<u8> mAlpha;
    /* 0x015 */ u8 mPadding15[3];
    /* 0x018 */ be<u32> mMode;
    /* 0x01C */ Color mColor;
};
WWHD_SIZE(dMap_2DSQ_c, 0x20);
WWHD_OFFSET(dMap_2DSQ_c, mVTable, 0x0);
WWHD_OFFSET(dMap_2DSQ_c, mDispLeft, 0x4);
WWHD_OFFSET(dMap_2DSQ_c, mDispRight, 0x8);
WWHD_OFFSET(dMap_2DSQ_c, mDispTop, 0xC);
WWHD_OFFSET(dMap_2DSQ_c, mDispBottom, 0x10);
WWHD_OFFSET(dMap_2DSQ_c, mAlpha, 0x14);
WWHD_OFFSET(dMap_2DSQ_c, mMode, 0x18);
WWHD_OFFSET(dMap_2DSQ_c, mColor, 0x1C);

struct dMap_2DTri_c {
    /* 0x000 */ be<u32> mVTable;
    /* 0x004 */ be<u16> mPosX;
    /* 0x006 */ be<u16> mPosY;
    /* 0x008 */ Color mColor;
    /* 0x00C */ be<u16> mAngle;
    /* 0x00E */ u8 mPaddingE[2];
    /* 0x010 */ be<f32> mScaleX;
    /* 0x014 */ be<f32> mScaleY;
    /* 0x018 */ be<u32> mScissorX;
    /* 0x01C */ be<u32> mScissorY;
    /* 0x020 */ be<u32> mScissorWidth;
    /* 0x024 */ be<u32> mScissorHeight;
};
WWHD_SIZE(dMap_2DTri_c, 0x28);
WWHD_OFFSET(dMap_2DTri_c, mVTable, 0x0);
WWHD_OFFSET(dMap_2DTri_c, mPosX, 0x4);
WWHD_OFFSET(dMap_2DTri_c, mPosY, 0x6);
WWHD_OFFSET(dMap_2DTri_c, mColor, 0x8);
WWHD_OFFSET(dMap_2DTri_c, mAngle, 0xC);
WWHD_OFFSET(dMap_2DTri_c, mScaleX, 0x10);
WWHD_OFFSET(dMap_2DTri_c, mScaleY, 0x14);
WWHD_OFFSET(dMap_2DTri_c, mScissorX, 0x18);
WWHD_OFFSET(dMap_2DTri_c, mScissorY, 0x1C);
WWHD_OFFSET(dMap_2DTri_c, mScissorWidth, 0x20);
WWHD_OFFSET(dMap_2DTri_c, mScissorHeight, 0x24);

struct dMap_2DPoint_c {
    /* 0x000 */ be<u32> mVTable;
    /* 0x004 */ be<u32> mScissorX;
    /* 0x008 */ be<u32> mScissorY;
    /* 0x00C */ be<u32> mScissorWidth;
    /* 0x010 */ be<u32> mScissorHeight;
    /* 0x014 */ be<u16> mPosX;
    /* 0x016 */ be<u16> mPosY;
    /* 0x018 */ Color mColor;
    /* 0x01C */ be<u8> mSize;
    /* 0x01D */ u8 mPadding1D[3];
};
WWHD_SIZE(dMap_2DPoint_c, 0x20);
WWHD_OFFSET(dMap_2DPoint_c, mVTable, 0x0);
WWHD_OFFSET(dMap_2DPoint_c, mScissorX, 0x4);
WWHD_OFFSET(dMap_2DPoint_c, mScissorY, 0x8);
WWHD_OFFSET(dMap_2DPoint_c, mScissorWidth, 0xC);
WWHD_OFFSET(dMap_2DPoint_c, mScissorHeight, 0x10);
WWHD_OFFSET(dMap_2DPoint_c, mPosX, 0x14);
WWHD_OFFSET(dMap_2DPoint_c, mPosY, 0x16);
WWHD_OFFSET(dMap_2DPoint_c, mColor, 0x18);
WWHD_OFFSET(dMap_2DPoint_c, mSize, 0x1C);

struct dMap_2DT2_c {
    /* 0x000 */ be<u32> mVTable;
    /* 0x004 */ be<u8> mTexObj[64];
    /* 0x044 */ be<f32> mPosX;
    /* 0x048 */ be<f32> mPosY;
    /* 0x04C */ be<f32> mWidth;
    /* 0x050 */ be<f32> mHeight;
    /* 0x054 */ be<f32> mScaleX;
    /* 0x058 */ be<f32> mScaleY;
    /* 0x05C */ Color mColorW;
    /* 0x060 */ Color mColorB;
    /* 0x064 */ be<u32> mScissorX;
    /* 0x068 */ be<u32> mScissorY;
    /* 0x06C */ be<u32> mScissorWidth;
    /* 0x070 */ be<u32> mScissorHeight;
    /* 0x074 */ be<u16> mRotZ;
    /* 0x076 */ be<u8> mMirrorS;
    /* 0x077 */ be<u8> mMirrorT;
    /* 0x078 */ be<u8> mMode;
    /* 0x079 */ u8 mPadding79[3];
};
WWHD_SIZE(dMap_2DT2_c, 0x7C);
WWHD_OFFSET(dMap_2DT2_c, mVTable, 0x0);
WWHD_OFFSET(dMap_2DT2_c, mTexObj, 0x4);
WWHD_OFFSET(dMap_2DT2_c, mPosX, 0x44);
WWHD_OFFSET(dMap_2DT2_c, mPosY, 0x48);
WWHD_OFFSET(dMap_2DT2_c, mWidth, 0x4C);
WWHD_OFFSET(dMap_2DT2_c, mHeight, 0x50);
WWHD_OFFSET(dMap_2DT2_c, mScaleX, 0x54);
WWHD_OFFSET(dMap_2DT2_c, mScaleY, 0x58);
WWHD_OFFSET(dMap_2DT2_c, mColorW, 0x5C);
WWHD_OFFSET(dMap_2DT2_c, mColorB, 0x60);
WWHD_OFFSET(dMap_2DT2_c, mScissorX, 0x64);
WWHD_OFFSET(dMap_2DT2_c, mScissorY, 0x68);
WWHD_OFFSET(dMap_2DT2_c, mScissorWidth, 0x6C);
WWHD_OFFSET(dMap_2DT2_c, mScissorHeight, 0x70);
WWHD_OFFSET(dMap_2DT2_c, mRotZ, 0x74);
WWHD_OFFSET(dMap_2DT2_c, mMirrorS, 0x76);
WWHD_OFFSET(dMap_2DT2_c, mMirrorT, 0x77);
WWHD_OFFSET(dMap_2DT2_c, mMode, 0x78);

struct MapInfo {
    /* 0x000 */ be<f32> mFieldMinX;
    /* 0x004 */ be<f32> mFieldMinZ;
    /* 0x008 */ be<f32> mFieldMaxX;
    /* 0x00C */ be<f32> mFieldMaxZ;
    /* 0x010 */ be<f32> mFieldCenterX;
    /* 0x014 */ be<f32> mFieldCenterZ;
    /* 0x018 */ be<f32> mRealMinX;
    /* 0x01C */ be<f32> mRealMinZ;
    /* 0x020 */ be<f32> mRealMaxX;
    /* 0x024 */ be<f32> mRealMaxZ;
    /* 0x028 */ be<f32> mRealCenterX;
    /* 0x02C */ be<f32> mRealCenterZ;
    /* 0x030 */ be<f32> mWorldUnitsPerPixel;
    /* 0x034 */ be<u8> mAlpha;
    /* 0x035 */ u8 mPadding35[3];
};
WWHD_SIZE(MapInfo, 0x38);
WWHD_OFFSET(MapInfo, mFieldMinX, 0x0);
WWHD_OFFSET(MapInfo, mFieldMinZ, 0x4);
WWHD_OFFSET(MapInfo, mFieldMaxX, 0x8);
WWHD_OFFSET(MapInfo, mFieldMaxZ, 0xC);
WWHD_OFFSET(MapInfo, mFieldCenterX, 0x10);
WWHD_OFFSET(MapInfo, mFieldCenterZ, 0x14);
WWHD_OFFSET(MapInfo, mRealMinX, 0x18);
WWHD_OFFSET(MapInfo, mRealMinZ, 0x1C);
WWHD_OFFSET(MapInfo, mRealMaxX, 0x20);
WWHD_OFFSET(MapInfo, mRealMaxZ, 0x24);
WWHD_OFFSET(MapInfo, mRealCenterX, 0x28);
WWHD_OFFSET(MapInfo, mRealCenterZ, 0x2C);
WWHD_OFFSET(MapInfo, mWorldUnitsPerPixel, 0x30);
WWHD_OFFSET(MapInfo, mAlpha, 0x34);

struct MapData {
    /* 0x000 */ be<u16> mWidth;
    /* 0x002 */ be<u16> mHeight;
    /* 0x004 */ u8 mPadding4[2];
    /* 0x006 */ be<u8> mFloorLabel;
    /* 0x007 */ u8 mPadding7[5];
    /* 0x00C */ be<u32> mMapOffset;
    /* 0x010 */ u8 mPadding10[34];
    /* 0x032 */ be<u8> mShowFloorLabel;
    /* 0x033 */ u8 mPadding33[5];
    /* 0x038 */ be<u32> mImageOffset;
};
WWHD_SIZE(MapData, 0x3C);
WWHD_OFFSET(MapData, mWidth, 0x0);
WWHD_OFFSET(MapData, mHeight, 0x2);
WWHD_OFFSET(MapData, mFloorLabel, 0x6);
WWHD_OFFSET(MapData, mMapOffset, 0xC);
WWHD_OFFSET(MapData, mShowFloorLabel, 0x32);
WWHD_OFFSET(MapData, mImageOffset, 0x38);

struct FloorEntry {
    /* 0x000 */ be<f32> mHeight;
    /* 0x004 */ be<u8> mFloorNo;
    /* 0x005 */ be<s8> mRooms[14];
    /* 0x013 */ u8 mPadding13[1];
};
WWHD_SIZE(FloorEntry, 0x14);
WWHD_OFFSET(FloorEntry, mHeight, 0x0);
WWHD_OFFSET(FloorEntry, mFloorNo, 0x4);
WWHD_OFFSET(FloorEntry, mRooms, 0x5);

struct FloorInfo {
    /* 0x000 */ be<s32> mCount;
    /* 0x004 */ be<u32> mEntries;
};
WWHD_SIZE(FloorInfo, 0x8);
WWHD_OFFSET(FloorInfo, mCount, 0x0);
WWHD_OFFSET(FloorInfo, mEntries, 0x4);

struct DungeonEntry {
    /* 0x000 */ u8 mPadding0[12];
    /* 0x00C */ be<f32> mOffsetY;
};
WWHD_SIZE(DungeonEntry, 0x10);
WWHD_OFFSET(DungeonEntry, mOffsetY, 0xC);

struct StageInfo {
    /* 0x000 */ u8 mPadding0[12];
    /* 0x00C */ be<u32> mStageTypeAndSchbit;
};
WWHD_SIZE(StageInfo, 0x10);
WWHD_OFFSET(StageInfo, mStageTypeAndSchbit, 0xC);

struct Image {
    /* 0x000 */ u8 mPadding0[2];
    /* 0x002 */ be<u16> width;
    /* 0x004 */ be<u16> height;
    /* 0x006 */ u8 mPadding6[2];
    /* 0x008 */ be<u8> indexTexture;
    /* 0x009 */ u8 mPadding9[3];
};
WWHD_SIZE(Image, 0xC);
WWHD_OFFSET(Image, width, 0x2);
WWHD_OFFSET(Image, height, 0x4);
WWHD_OFFSET(Image, indexTexture, 0x8);

struct CollectPoint {
    /* 0x000 */ be<f32> mPosX;
    /* 0x004 */ be<f32> mPosY;
    /* 0x008 */ be<f32> mPosZ;
    /* 0x00C */ be<u16> mAngle;
    /* 0x00E */ be<u8> mRoomNo;
    /* 0x00F */ be<u8> mAgbType;
    /* 0x010 */ be<u8> mGcType;
    /* 0x011 */ be<u8> mFrame;
    /* 0x012 */ be<u8> mParameter;
    /* 0x013 */ be<u8> mCursor;
    /* 0x014 */ be<u8> mAcsType;
    /* 0x015 */ u8 mPadding15[1];
    /* 0x016 */ be<u8> mNext;
    /* 0x017 */ u8 mPadding17[1];
};
WWHD_SIZE(CollectPoint, 0x18);
WWHD_OFFSET(CollectPoint, mPosX, 0x0);
WWHD_OFFSET(CollectPoint, mPosY, 0x4);
WWHD_OFFSET(CollectPoint, mPosZ, 0x8);
WWHD_OFFSET(CollectPoint, mAngle, 0xC);
WWHD_OFFSET(CollectPoint, mRoomNo, 0xE);
WWHD_OFFSET(CollectPoint, mAgbType, 0xF);
WWHD_OFFSET(CollectPoint, mGcType, 0x10);
WWHD_OFFSET(CollectPoint, mFrame, 0x11);
WWHD_OFFSET(CollectPoint, mParameter, 0x12);
WWHD_OFFSET(CollectPoint, mCursor, 0x13);
WWHD_OFFSET(CollectPoint, mAcsType, 0x14);
WWHD_OFFSET(CollectPoint, mNext, 0x16);

struct Actor {
    /* 0x000 */ u8 mPadding0[734];
    /* 0x2DE */ be<u8> mMapParameter;
    /* 0x2DF */ u8 mPadding2DF[1];
    /* 0x2E0 */ be<u32> mStatus;
    /* 0x2E4 */ u8 mPadding2E4[22];
    /* 0x2FA */ be<s16> mHomeAngleY;
    /* 0x2FC */ u8 mPadding2FC[24];
    /* 0x314 */ be<f32> mCurrentX;
    /* 0x318 */ be<f32> mCurrentY;
    /* 0x31C */ be<f32> mCurrentZ;
    /* 0x320 */ u8 mPadding320[6];
    /* 0x326 */ be<s8> mRoomNo;
    /* 0x327 */ u8 mPadding327[3];
    /* 0x32A */ be<s16> mShapeAngleY;
    /* 0x32C */ u8 mPadding32C[188];
    /* 0x3E8 */ be<s8> mMapRoomNo;
    /* 0x3E9 */ u8 mPadding3E9[3];
};
WWHD_SIZE(Actor, 0x3EC);
WWHD_OFFSET(Actor, mMapParameter, 0x2DE);
WWHD_OFFSET(Actor, mStatus, 0x2E0);
WWHD_OFFSET(Actor, mHomeAngleY, 0x2FA);
WWHD_OFFSET(Actor, mCurrentX, 0x314);
WWHD_OFFSET(Actor, mCurrentY, 0x318);
WWHD_OFFSET(Actor, mCurrentZ, 0x31C);
WWHD_OFFSET(Actor, mRoomNo, 0x326);
WWHD_OFFSET(Actor, mShapeAngleY, 0x32A);
WWHD_OFFSET(Actor, mMapRoomNo, 0x3E8);

struct SaveInfo {
    /* 0x000 */ u8 mPadding0[4424];
    /* 0x1148 */ be<s8> mRestartRoomNo;
    /* 0x1149 */ u8 mPadding1149[21];
    /* 0x115E */ be<s16> mRestartRoomAngleY;
    /* 0x1160 */ be<f32> mRestartX;
    /* 0x1164 */ be<f32> mRestartY;
    /* 0x1168 */ be<f32> mRestartZ;
};
WWHD_SIZE(SaveInfo, 0x116C);
WWHD_OFFSET(SaveInfo, mRestartRoomNo, 0x1148);
WWHD_OFFSET(SaveInfo, mRestartRoomAngleY, 0x115E);
WWHD_OFFSET(SaveInfo, mRestartX, 0x1160);
WWHD_OFFSET(SaveInfo, mRestartY, 0x1164);
WWHD_OFFSET(SaveInfo, mRestartZ, 0x1168);

struct PlayInfo {
    /* 0x000 */ u8 mPadding0[20788];
    /* 0x5134 */ be<u8> mStartStageName[8];
    /* 0x513C */ u8 mPadding513C[20];
    /* 0x5150 */ be<u8> mStage[124];
    /* 0x51CC */ be<u8> mRoomControl[198];
    /* 0x5292 */ be<u8> mEventRunning;
    /* 0x5293 */ u8 mPadding5293[1393];
    /* 0x5804 */ be<u8> mAttention[1236];
    /* 0x5CD8 */ be<u32> mPlayerStatus;
};
WWHD_SIZE(PlayInfo, 0x5CDC);
WWHD_OFFSET(PlayInfo, mStartStageName, 0x5134);
WWHD_OFFSET(PlayInfo, mStage, 0x5150);
WWHD_OFFSET(PlayInfo, mRoomControl, 0x51CC);
WWHD_OFFSET(PlayInfo, mEventRunning, 0x5292);
WWHD_OFFSET(PlayInfo, mAttention, 0x5804);
WWHD_OFFSET(PlayInfo, mPlayerStatus, 0x5CD8);

struct ResourceName {
    /* 0x000 */ be<u32> mString;
    /* 0x004 */ be<u32> mVTable;
};
WWHD_SIZE(ResourceName, 0x8);
WWHD_OFFSET(ResourceName, mString, 0x0);
WWHD_OFFSET(ResourceName, mVTable, 0x4);

struct Archive {
    /* 0x000 */ u8 mPadding0[76];
    /* 0x04C */ be<u32> mResourceOffset;
};
WWHD_SIZE(Archive, 0x50);
WWHD_OFFSET(Archive, mResourceOffset, 0x4C);

struct Resource {
    /* 0x000 */ be<u32> mDataOffset;
};
WWHD_SIZE(Resource, 0x4);
WWHD_OFFSET(Resource, mDataOffset, 0x0);

struct StageVTable {
    u8 mPadding0[0x7C];
    be<u32> getMapInfo;
    u8 mPadding80[0xDC];
    be<u32> getStagInfo;
    u8 mPadding160[0x8C];
    be<u32> getFloorInfo;
    u8 mPadding1F0[0x4C];
    be<u32> getDMap;
};
WWHD_OFFSET(StageVTable, getMapInfo, 0x7C);
WWHD_OFFSET(StageVTable, getStagInfo, 0x15C);
WWHD_OFFSET(StageVTable, getFloorInfo, 0x1EC);
WWHD_OFFSET(StageVTable, getDMap, 0x23C);
WWHD_SIZE(StageVTable, 0x240);
struct StageObject {
    be<u32> mVTable;
};
WWHD_SIZE(StageObject, 4);
WWHD_OFFSET(StageObject, mVTable, 0);
struct SafeStringVTable {
    u8 mPadding0[20];
    be<u32> assureTermination;
};
WWHD_OFFSET(SafeStringVTable, assureTermination, 20);
WWHD_SIZE(SafeStringVTable, 24);
struct PointTypes {
    be<u8> mAgbType;
    be<u8> mGcType;
};
WWHD_OFFSET(PointTypes, mAgbType, 0);
WWHD_OFFSET(PointTypes, mGcType, 1);
WWHD_SIZE(PointTypes, 2);
struct Grid {
    be<s8> x;
    be<s8> y;
};
WWHD_OFFSET(Grid, x, 0);
WWHD_OFFSET(Grid, y, 1);
WWHD_SIZE(Grid, 2);

enum FloorNo : u32 { Floor_Base = 123, Floor_Num = 10, Floor_1F = 128, Floor_None = 255 };
enum MapKind : u32 { MAP_KIND_NONE = 0, MAP_KIND_FIELD = 1, MAP_KIND_DUNGEON = 2 };
enum DisplayFlags : u32 {
    DSP_ENABLE_REAL_SIZE = 1,
    DSP_ENABLE_ENLARGEMENT_SIZE = 2,
    DSP_ENABLE_BOTH_SIZE = 3
};
} // namespace dMapHD

// Verified free entry wrappers preserve HD storage / argument ordering.
bool IsFloorNo(s32 floor);
void* dMap_getFloorInfo_WithRoom(s32 room);
f32 mapOffsetY();
s32 dMap_GetTopBottomFloorNo(void* stage, void* bottom, void* top);
void* dMap_GetFloorInfoDtP(void* floor, f32 y);
u8 dMap_GetFloorNo_WithRoom(s32 room, f32 y);
u8 dMap_GetFloorNo(void* stage, f32 y);
void* dMap_2DAGBScrDsp__ct(void* self);
void* dMap_2DMtMapSpcl_tex__ct(void* self);
void* dMap_2DMtMapSpcl__ct(void* self);
void* dMap_2DSQ__ct(void* self);
void* dMap_2DTri__ct(void* self);
void dMap_2DSQ_draw();
void* dMap_2DPoint__ct(void* self);
void* dMap_2DAGBCursor__ct(void* self);
void dMap_2DMtMapSpcl_draw();
void* dMap_2DT2__ct(void* self);
void* dMap_RoomInfo__ct(void* self);
void dMap_RoomInfo__dt(void* self, u32 flags);
void dMap_2DMtMapSpcl__dt(void* self, u32 flags);
void dMap_2DSQ__dt(void* self, u32 flags);
void dMap_2DTri__dt(void* self, u32 flags);
void dMap_2DPoint__dt(void* self, u32 flags);
void dMap_2DAGBCursor__dt(void* self, u32 flags);
void dMap_2DT2__dt(void* self, u32 flags);
void dMap_2DTri_draw();
void dMap_2DPoint_draw();
void dMap_2DAGBCursor_draw();
void dMap_2DT2_draw();
void dMap_SafeString_assureTermination();
void dMap_SafeString_destruct();
void dMap_2DMtMapSpcl_tex__dt(void* self, u32 flags);
void dMap_destroyRooms();
void dMap_destroyFrames();
void dMap_destroyPoints();
void dMap_destroyTboxes();
void dMap_destroyDoors();
void dMap_destroyFriends();
void dMap__sinit();
void dMap_loadFmapResource();
void dMap_loadDmapResource();
void dMap_setFmapChkDtPrm();
void dMap_createResources();
s32 dMap_getKindMapType();
void dMap_deleteImage(s32 room);
bool dMap_isEnableEnlargementScroll();
u32 dMap_agbMapNoSetCall();
void dMap_mapAGBSendMapMain();
void dMap_calcScissor();
void dMap_getTypeAgbGcFromTypeAcs(u32 kind, void* agb, void* gc);
void dMap_setCollectPoint(s32 kind, u32 type, s32 room, s32 angle, u32 frame, u32 prm, u32 cursor,
                          u32 sub, f32 x, f32 y, f32 z);
void dMap_drawPointSingle(s32 kind, s32 room, s32 angle, u32 frame, u32 prm, u32 cursor, f32 x,
                          f32 y, f32 z);
bool dMap_isInDspArea(s32 detailed, f32 x, f32 y);
s32 dMap_getPosAgbMapType(s32 detailed, f32 x, f32 y);
void dMap_setPlayerStayAgbMapTypeNow(f32 x, f32 y);
bool dMap_setNowRoom(s32 no);
void dMap_loadFmapResourceForPlatform();
void dMap_loadDmapResourceForPlatform();
void dMap_point2Grid(void* gx, void* gy, f32 x, f32 y);
void dMap_point2GridAndLocal(void* gx, void* gy, void* lx, void* ly, f32 x, f32 y);
void dMap_calcEnlargementSizeParameter(f32 x, f32 y);
void dMap_setArriveInfo(f32 x, f32 y);
void dMap_drawActorPointMiniMap(void* actor);
void dMap_mapMoveAll(s32 roomNo, f32 x, f32 y, f32 height);
void dMap_draw(s32 roomNo, f32 x, f32 y, f32 height);

// Translation-unit ABI bindings; addresses and signatures are checked by the glue.
void* dMap_getPlay();
void* dMap_getStatusRoomDt(void*, s32);
void dMap_showAssert(const char*, u32, const char*);
void* dMap_operatorNew(u32);
void dMap_operatorDelete(void*);
void dMap_drawlistBaseConstruct(void*);
void dMap_drawlistBaseDestruct(void*, u32);
void dMap_bzero(void*, u32);
u32 dMap_cnvind32(u32);
u32 dMap_cnvind16(u32);
void dMap_constructArray(void*, u32, u32, u32);
void dMap_destroyArray(void*, u32, u32, u32, u32, u32);
void dMap_registerGlobal(void*);
void dMap_constructHeaderByte(void*);
void dMap_constructArchiveByte(void*);
bool dMap_resourceExists(void*, void*, void*, u32);
void* dMap_getArchive();
void* dMap_getArchiveResource(void*, void*);
f32 dMap_floor(f32);
s32 dMap_sprintf(void*, const char*, u32);
void* dMap_getStageRes(void*, void*);
void dMap_copyBytes(void*, void*, u32);
void dMap_onSaveArriveGrid(void*, s32);
void dMap_onVisitedRoom(void*, s32);
void* dMap_getAttentionTarget(void*, u32);
bool dMap_isAttentionLock(void*);
void dMap_deleteRoomEntry(void*);
void dMap_changeImageEntry(void*, u32, u32, s32, s32, s32, f32);
u32 dMap_getRoomImageEntry(void*, s32, u32, s32, void*, void*, void*, void*, void*);
void dMap_mapAGBSendMapMainEntry(f32, f32);
