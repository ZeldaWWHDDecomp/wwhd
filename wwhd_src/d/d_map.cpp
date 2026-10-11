/* d_map: floor selection, room map images and map-point collection.
 * GameCube source order and names; WWHD functions are verified against the original.
 * HD: GBA rendering is inert, resource loading uses BFRES archives, and GX records grow. */
#include "d/d_map.h"
using namespace dMapHD;

static f32 l_zero;
static f32 l_one;
static f32 l_gridBias;
static f32 l_gridSize;
static f32 l_scrollRange;
static f32 l_minusOne;
static f32 l_floorLabelThreshold;
static f32 l_iconFreeScale;
static f32 l_iconSelfScale;
static u32 mNowRoomInfoP;
static s8 mStayRoomNo;
static u32 mSaveInfoP;
static u32 mResourceControlP;
static u32 l_roomColorInitialized;
static f32 mIconFreeScale;
static f32 mIconSelfScale;
static u32 mHDMapState;
static u32 mHDMapStateOld;
static u32 mHDMapDisplayState;
static u32 mHDMapDisplayStateOld;
static f32 mEnlargementSizeCenterX;
static f32 mEnlargementSizeCenterZ;
static f32 mEnlargementSizeScaleX;
static f32 mEnlargementSizeScaleZ;
static u32 mFmapChkPntDataP;
static u32 mFmapResourceP;
static u32 mDmapResourceP;
static u32 mAGBPointValueC;
static u32 mAGBPointValueE;
static u32 mAGBPointValueAll;
static u32 mAGBPointValueRoomAll;
static f32 mNowCenterX;
static f32 mNowCenterZ;
static f32 mNowScaleX;
static f32 mNowScaleZ;
static u8 mCompAlpha;
static u8 mAlpha;
static u8 mCollectPointDataCnt;
static u8 mAGBMapSendStatus;
static u8 mMapDispMode;
static u8 mAGBMapSendStopFlg;
static u8 mPlayerStayAgbMapTypeNow;
static u8 mPlayerStayAgbMapTypeOld;
static s8 mPlGridX;
static s8 mPlGridY;
static u8 mNowFloorNo;
static u8 mFmapChkPntValue;
static dMap_RoomInfoCtrl_c mRoomInfoCtrl;
struct mRoomInfoStorage {
    dMap_RoomInfo_c entries[20];
};
static mRoomInfoStorage mRoomInfo;
struct mFrameTexStorage {
    dMap_2DMtMapSpcl_c entries[8];
};
static mFrameTexStorage mFrameTex;
struct mFrameTextureStorage {
    dMap_2DMtMapSpcl_tex_c entries[8];
};
static mFrameTextureStorage mFrameTexture;
static dMap_2DMtMapSpcl_tex_c mIconFreeTexture;
static dMap_2DMtMapSpcl_tex_c mIconSelfTexture;
static dMap_2DTri_c mCursor;
static dMap_2DPoint_c mAgbCursor;
struct mPointStorage {
    dMap_2DPoint_c entries[15];
};
static mPointStorage mPoint;
struct mTboxStorage {
    dMap_2DT2_c entries[8];
};
static mTboxStorage mTbox;
struct mDoorStorage {
    dMap_2DT2_c entries[16];
};
static mDoorStorage mDoor;
static dMap_2DTri_c mPointRestart;
struct mPointFriendStorage {
    dMap_2DPoint_c entries[3];
};
static mPointFriendStorage mPointFriend;
static dMap_2DSQ_c m2DSQdraw;
static dMap_2DSQ_c m2DSQdraw2;
struct mCollectPointDataStorage {
    CollectPoint entries[64];
};
static mCollectPointDataStorage mCollectPointData;
struct mCollectPointDataLinkListStorage {
    be<u8> entries[21];
};
static mCollectPointDataLinkListStorage mCollectPointDataLinkList;
static dMap_2DMtMapSpcl_c mIconFreeTex;
static dMap_2DMtMapSpcl_c mIconSelfTex;
static dMap_2DT2_c mShip;
static Color l_roomColor;
static Color l_roomColorDefault;
struct l_mapHeaderStorage {
    u8 bytes[1];
};
static l_mapHeaderStorage l_mapHeader;
struct l_archiveHeaderStorage {
    u8 bytes[1];
};
static l_archiveHeaderStorage l_archiveHeader;
struct l_globalDestructor0Storage {
    u8 bytes[12];
};
static l_globalDestructor0Storage l_globalDestructor0;
struct l_globalDestructor1Storage {
    u8 bytes[12];
};
static l_globalDestructor1Storage l_globalDestructor1;
struct l_globalDestructor2Storage {
    u8 bytes[12];
};
static l_globalDestructor2Storage l_globalDestructor2;
struct l_globalDestructor3Storage {
    u8 bytes[12];
};
static l_globalDestructor3Storage l_globalDestructor3;
struct l_globalDestructor4Storage {
    u8 bytes[12];
};
static l_globalDestructor4Storage l_globalDestructor4;
struct l_globalDestructor5Storage {
    u8 bytes[12];
};
static l_globalDestructor5Storage l_globalDestructor5;
struct l_globalDestructor6Storage {
    u8 bytes[12];
};
static l_globalDestructor6Storage l_globalDestructor6;
struct l_globalDestructor7Storage {
    u8 bytes[12];
};
static l_globalDestructor7Storage l_globalDestructor7;
struct l_globalDestructor8Storage {
    u8 bytes[12];
};
static l_globalDestructor8Storage l_globalDestructor8;
struct l_globalDestructor9Storage {
    u8 bytes[12];
};
static l_globalDestructor9Storage l_globalDestructor9;
struct l_globalDestructor10Storage {
    u8 bytes[12];
};
static l_globalDestructor10Storage l_globalDestructor10;
struct l_globalDestructor11Storage {
    u8 bytes[12];
};
static l_globalDestructor11Storage l_globalDestructor11;
struct l_globalDestructor12Storage {
    u8 bytes[12];
};
static l_globalDestructor12Storage l_globalDestructor12;
struct l_globalDestructor13Storage {
    u8 bytes[12];
};
static l_globalDestructor13Storage l_globalDestructor13;
struct l_globalDestructor14Storage {
    u8 bytes[12];
};
static l_globalDestructor14Storage l_globalDestructor14;
struct l_globalDestructor15Storage {
    u8 bytes[12];
};
static l_globalDestructor15Storage l_globalDestructor15;
struct l_globalDestructor16Storage {
    u8 bytes[12];
};
static l_globalDestructor16Storage l_globalDestructor16;
// TU vtables and distinct copies of the four inlined bound assertions.
enum : u32 {
    SafeString_vtable = 0x10050A34,
    ScrDsp_vtable = 0x10050F1C,
    Spcl_vtable = 0x10050F34,
    SQ_vtable = 0x10050F4C,
    Tri_vtable = 0x10050F64,
    Point_vtable = 0x10050F7C,
    AGBCursor_vtable = 0x10050F94,
    T2_vtable = 0x10050FAC,
    l_realMinXFile = 0x10050A4C,
    l_realMinXAssertion = 0x10050A58,
    l_realMinZFile = 0x10050A6C,
    l_realMinZAssertion = 0x10050A78,
    l_realMaxXFile = 0x10050A8C,
    l_realMaxXAssertion = 0x10050A98,
    l_realMaxZFile = 0x10050AAC,
    l_realMaxZAssertion = 0x10050AB8,
};
struct statusMapNo2TypeNoTblStorage {
    be<u8> entries[50];
};
static statusMapNo2TypeNoTblStorage statusMapNo2TypeNoTbl;
struct agbTblStorage {
    be<s8> entries[21];
};
static agbTblStorage agbTbl;

// These are virtual getters of the original stage/room object, not mocked adapters.
static void* getFloorInfo(void* stage) {
    auto* object = static_cast<StageObject*>(stage);
    auto* table = gabi::at<StageVTable>(object->mVTable);
    return gabi::call_ptr<void*>(table->getFloorInfo, stage);
}
static void* getStagInfo(void* stage) {
    auto* object = static_cast<StageObject*>(stage);
    auto* table = gabi::at<StageVTable>(object->mVTable);
    return gabi::call_ptr<void*>(table->getStagInfo, stage);
}
static void* getDMap(void* stage) {
    auto* object = static_cast<StageObject*>(stage);
    auto* table = gabi::at<StageVTable>(object->mVTable);
    return gabi::call_ptr<void*>(table->getDMap, stage);
}
static void* getMapInfo(void* room, u32 floor) {
    auto* object = static_cast<StageObject*>(room);
    auto* table = gabi::at<StageVTable>(object->mVTable);
    return gabi::call_ptr<void*>(table->getMapInfo, room, floor);
}

static void* dMap_pointCtor(void* self, u32 vt) {
    if (!self)
        self = dMap_operatorNew(32);
    if (!self)
        return nullptr;
    auto* i_this = static_cast<dMap_2DPoint_c*>(self);
    u32 a = gabi::ea(self);
    dMap_drawlistBaseConstruct(self);
    i_this->mScissorWidth = 0;
    i_this->mVTable = vt;
    i_this->mScissorY = 0;
    i_this->mScissorHeight = 0;
    i_this->mPosX = 0;
    i_this->mPosY = 0;
    i_this->mScissorX = 0;
    dMap_bzero(gabi::at<void>(a + 24), 4);
    i_this->mSize = 0;
    return self;
}

static void dMap_drawableDtor(void* self, u32 flags) {
    if (self) {
        dMap_drawlistBaseDestruct(self, 0);
        if (flags & 1)
            dMap_operatorDelete(self);
    }
}

using MapResourceName = ResourceName;
static void dMap_loadResource(u32 first, u32 second, u32 discard, u32 selected, u32 output) {
    gabi::Local<MapResourceName> a, b;
    a->mString = first;
    a->mVTable = SafeString_vtable;
    b->mString = second;
    b->mVTable = SafeString_vtable;
    if (!dMap_resourceExists(gabi::at<void>(mResourceControlP.get()), a.get(), b.get(), 0))
        return;
    void* archive = dMap_getArchive();
    u32 base = gabi::ea(archive) + 0x4C,
        relative = static_cast<Archive*>(archive)->mResourceOffset.get();
    dMap_getArchiveResource(gabi::at<void>(relative ? base + relative : 0),
                            gabi::at<void>(discard));
    relative = static_cast<Archive*>(archive)->mResourceOffset.get();
    void* resource = dMap_getArchiveResource(gabi::at<void>(relative ? base + relative : 0),
                                             gabi::at<void>(selected));
    relative = static_cast<Resource*>(resource)->mDataOffset.get();
    if (relative && output)
        (*gabi::at<be<u32>>(output)) = gabi::ea(resource) + relative;
}

static u32 dMap_info(u32 room, u32 file, u32 expr) {
    u32 info = gabi::at<dMap_RoomInfo_c>(room)->mStageMapInfoP.get();
    if (!info) {
        dMap_showAssert(gabi::at<const char>(file), 0x46B, gabi::at<const char>(expr));
        info = gabi::at<dMap_RoomInfo_c>(room)->mStageMapInfoP.get();
    }
    return info;
}
static f32 readMapBound(u32 info, u32 offset) {
    auto* map = gabi::at<MapInfo>(info);
    switch (offset) {
    case 0:
        return map->mFieldMinX;
    case 4:
        return map->mFieldMinZ;
    case 8:
        return map->mFieldMaxX;
    case 12:
        return map->mFieldMaxZ;
    case 24:
        return map->mRealMinX;
    case 28:
        return map->mRealMinZ;
    case 32:
        return map->mRealMaxX;
    default:
        return map->mRealMaxZ;
    }
}

static f32 dMap_checkedBound(u32 room, u32 offset, u32 line, u32 getterfile, u32 getterexpr,
                             u32 file, u32 expr) {
    u32 info = dMap_info(room, getterfile, getterexpr);
    if (!info)
        dMap_showAssert(gabi::at<const char>(file), line, gabi::at<const char>(expr));
    return readMapBound(info, offset);
}

static s32 dMap_clampGrid(s32 v) { return v < -3 ? -3 : v > 3 ? 3 : v; }
static s32 dMap_gridIndex(f32 value, f32 size, f32 bias) {
    return s8(gabi::ftoi(dMap_floor(gabi::fadds_ppc(f32(f64(value) / size), bias))));
}

static u32 dMap_scrollInfo(u32& room) {
    u32 info = gabi::at<dMap_RoomInfo_c>(room)->mStageMapInfoP.get();
    if (!info) {
        dMap_showAssert("d_map.h", 0x46B, "mStageMapInfoP != (0)");
        info = gabi::at<dMap_RoomInfo_c>(room)->mStageMapInfoP.get();
        room = mNowRoomInfoP.get();
    }
    return info;
}

/* 0258D8C8 */
bool IsFloorNo(s32 floor) { return u32(floor) - Floor_Base < Floor_Num; }

/* 0258D8E4 */
void* dMap_getFloorInfo_WithRoom(s32 room) {
    void* floor = nullptr;
    if (room >= 0) {
        void* play = dMap_getPlay();
        void* dt = dMap_getStatusRoomDt(static_cast<PlayInfo*>(play)->mRoomControl, room);
        if (dt)
            floor = getFloorInfo(dt);
    }
    if (!floor) {
        void* play = dMap_getPlay();
        floor = getFloorInfo(static_cast<PlayInfo*>(play)->mStage);
    }
    return floor;
}

/* 0258D954 */
f32 mapOffsetY() {
    f32 result = l_zero.get();
    void* play = dMap_getPlay();
    void* stag = getStagInfo(static_cast<PlayInfo*>(play)->mStage);
    u32 type = (static_cast<StageInfo*>(stag)->mStageTypeAndSchbit.get() >> 16) & 7;
    bool relevant = type == 3;
    if (!relevant) {
        play = dMap_getPlay();
        stag = getStagInfo(static_cast<PlayInfo*>(play)->mStage);
        type = (static_cast<StageInfo*>(stag)->mStageTypeAndSchbit.get() >> 16) & 7;
        relevant = type == 6;
    }
    if (relevant) {
        play = dMap_getPlay();
        if (getDMap(static_cast<PlayInfo*>(play)->mStage)) {
            play = dMap_getPlay();
            void* table = getDMap(static_cast<PlayInfo*>(play)->mStage);
            u32 t = gabi::ea(table), entry = gabi::at<FloorInfo>(t)->mEntries.get();
            s32 n = gabi::at<FloorInfo>(t)->mCount.get();
            if (n != 1) {
                dMap_showAssert("d_map.cpp", 0x5A5, "pinf->num == 1");
                n = gabi::at<FloorInfo>(t)->mCount.get();
            }
            for (s32 i = 0; i < n; ++i, entry += 16)
                result = gabi::at<DungeonEntry>(entry)->mOffsetY.get();
        }
    }
    return result;
}

/* 0258DA64 */
s32 dMap_GetTopBottomFloorNo(void* stage, void* bottom, void* top) {
    if (!stage)
        return 0;
    u32 lo = Floor_1F, hi = Floor_1F;
    void* floor = getFloorInfo(stage);
    if (floor && static_cast<FloorInfo*>(floor)->mCount.get() > 0) {
        s32 n = static_cast<FloorInfo*>(floor)->mCount.get();
        u32 e = static_cast<FloorInfo*>(floor)->mEntries.get();
        lo = Floor_Base;
        hi = Floor_Base + Floor_Num - 1;
        for (s32 i = 0; i < n; ++i, e += 20)
            for (u32 j = 0; j < 14; ++j)
                if (gabi::at<FloorEntry>(e)->mRooms[j].get() != -1) {
                    u32 f = gabi::at<FloorEntry>(e)->mFloorNo.get();
                    if (f > lo)
                        lo = f;
                    if (f < hi)
                        hi = f;
                    break;
                }
    }
    if (bottom)
        (*gabi::at<be<u8>>(gabi::ea(bottom))) = lo;
    if (top)
        (*gabi::at<be<u8>>(gabi::ea(top))) = hi;
    return 0;
}

/* 0258DB6C */
void* dMap_GetFloorInfoDtP(void* floor, f32 y) {
    if (!floor)
        return nullptr;
    s32 n = static_cast<FloorInfo*>(floor)->mCount.get();
    if (n <= 0)
        return nullptr;
    u32 e = static_cast<FloorInfo*>(floor)->mEntries.get();
    for (s32 i = 0; i < n; ++i, e += 20) {
        if (i == n - 1) {
            if (n == 1 || !(y < gabi::at<FloorEntry>(e)->mHeight.get()))
                return gabi::at<void>(e);
        } else {
            if (i != 0 && y < gabi::at<FloorEntry>(e)->mHeight.get())
                continue;
            if (y < gabi::at<FloorEntry>(e + sizeof(FloorEntry))->mHeight.get())
                return gabi::at<void>(e);
        }
    }
    return nullptr;
}

/* 0258DBF4 */
u8 dMap_GetFloorNo_WithRoom(s32 room, f32 y) {
    void* floor = dMap_getFloorInfo_WithRoom(room);
    void* entry = dMap_GetFloorInfoDtP(floor, y);
    return entry ? static_cast<FloorEntry*>(entry)->mFloorNo.get() : Floor_1F;
}

/* 0258DC48 */
u8 dMap_GetFloorNo(void* stage, f32 y) { return dMap_GetFloorNo_WithRoom(-1, y); }

/* 0258E5A8 */
u8 dMap_RoomInfo_c::getRoomDspFloorNo(u32 floor, s32 search) {
    u32 n = floor - Floor_Base;
    auto check = [](u32 index, u32 line) {
        if (index >= Floor_Num)
            dMap_showAssert("d_map.cpp", line, "(no >= 0) && (no < (Floor_5F - Floor_B5F + 1))");
    };
    check(n, 0x720);
    u32 result = (*gabi::at<be<u8>>(gabi::ea(&mFloorTable[0]) + n)).get();
    if (search && !IsFloorNo(result)) {
        n--;
        while ((s32)n >= 0) {
            check(n, 0x72D);
            result = (*gabi::at<be<u8>>(gabi::ea(&mFloorTable[0]) + n)).get();
            if (IsFloorNo(result))
                return result;
            n--;
        }
        ++n;
        while ((s32)n <= 9) {
            check(n, 0x736);
            result = (*gabi::at<be<u8>>(gabi::ea(&mFloorTable[0]) + n)).get();
            if (IsFloorNo(result))
                return result;
            ++n;
        }
        result = 255;
    }
    return result;
}

/* 0258E1E8 */
u32 dMap_RoomInfo_c::getRoomImage(s32 room, u32 floor, s32 search, void* out0, void* out1,
                                  void* outData, void* outInfo) {

    // HD: ninth integer argument is the caller stack word at +8; generic VERIFY entries read only GPRs.
    void* outFlags = gabi::at<void>((*gabi::at<be<u32>>(gabi::cpu->r[1] + 8)).get());
    u32 image0 = 0, image1 = 0, data = 0, info = 0, flags = 0;
    bool found = false;
    if (IsFloorNo(floor)) {
        gabi::Local<char[32]> roomName, resourceName;
        dMap_sprintf(roomName.get(), "Room%d", room);
        if (IsFloorNo(floor))
            for (;;) {
                dMap_sprintf(resourceName.get(), "s%d.bti", floor);
                image0 = gabi::ea(dMap_getStageRes(roomName.get(), resourceName.get()));
                dMap_sprintf(resourceName.get(), "m%d.bti", floor);
                image1 = gabi::ea(dMap_getStageRes(roomName.get(), resourceName.get()));
                dMap_sprintf(resourceName.get(), "m%d.amp", floor);
                data = gabi::ea(dMap_getStageRes(roomName.get(), resourceName.get()));
                u32 probe = floor;
                info = 0;
                if (IsFloorNo(probe))
                    for (;;) {
                        void* play = dMap_getPlay();
                        void* dt =
                            dMap_getStatusRoomDt(static_cast<PlayInfo*>(play)->mRoomControl, room);
                        info = gabi::ea(getMapInfo(dt, probe));
                        probe = (probe - 1) & 255;
                        if (info || !IsFloorNo(probe))
                            break;
                    }
                if (image0 && image1 && data && info) {
                    found = true;
                    break;
                }
                floor = (floor - 1) & 255;
                if (!search || !IsFloorNo(floor))
                    break;
            }
        if (image0 && info &&
            gabi::at<MapInfo>(info)->mFieldMaxX.get() !=
                gabi::at<MapInfo>(info)->mFieldMinX.get() &&
            gabi::at<MapInfo>(info)->mFieldMaxZ.get() != gabi::at<MapInfo>(info)->mFieldMinZ.get())
            flags = 2;
        if (image1) {
            u32 w = gabi::at<Image>(image1)->width.get(), h;
            if (w == 128) {
                h = gabi::at<Image>(image1)->height.get();
                if (h <= 480 && info &&
                    gabi::at<MapInfo>(info)->mRealMaxX.get() !=
                        gabi::at<MapInfo>(info)->mRealMinX.get() &&
                    gabi::at<MapInfo>(info)->mRealMaxZ.get() !=
                        gabi::at<MapInfo>(info)->mRealMinZ.get())
                    flags |= 1;
            } else if (!(w & 7) && gabi::at<Image>(image1)->height.get() == 8 && info &&
                       gabi::at<MapInfo>(info)->mRealMaxX.get() !=
                           gabi::at<MapInfo>(info)->mRealMinX.get() &&
                       gabi::at<MapInfo>(info)->mRealMaxZ.get() !=
                           gabi::at<MapInfo>(info)->mRealMinZ.get())
                flags |= 1;
        }
    }
    if (flags != 3)
        flags = 0;
    if (out0)
        (*gabi::at<be<u32>>(gabi::ea(out0))) = image0;
    if (out1)
        (*gabi::at<be<u32>>(gabi::ea(out1))) = image1;
    if (outData)
        (*gabi::at<be<u32>>(gabi::ea(outData))) = data;
    if (outInfo)
        (*gabi::at<be<u32>>(gabi::ea(outInfo))) = info;
    if (outFlags)
        (*gabi::at<be<u8>>(gabi::ea(outFlags))) = flags;
    return found ? floor : 255;
}

/* 0258E7A4 */
bool dMap_RoomInfo_c::Changeimage(u32 floor, u32 otherFloor, s32 currentRoom, s32 sx, s32 sy,
                                  f32 height) {
    u32 selected;
    gabi::Local<gabi::be<u32>> image0, image1, data, info;
    (*image0) = 0;
    (*image1) = 0;
    (*data) = 0;
    (*info) = 0;
    if (IsFloorNo(floor))
        selected = static_cast<dMap_RoomInfo_c*>(this)->getRoomDspFloorNo(
            floor, this->m_no.get() == u32(currentRoom));
    else {
        dMap_showAssert("d_map.cpp", 0x9B3, "0");
        // HD: the failed assertion leaves the selection in the clobbered argument register.
        selected = gabi::cpu->r[5];
    }
    if (selected == this->mDspFloorNo.get())
        return false;
    this->mDspFloorNo = selected;
    this->mEnableFlags = 0;
    this->mStageMapInfoP = 0;
    if (IsFloorNo(selected))
        dMap_getRoomImageEntry(this, s32(this->m_no.get()), selected, 0, image0.get(), image1.get(),
                               data.get(), info.get(), &mEnableFlags);
    u32 ip = (*info).get(), flags = this->mEnableFlags.get();
    if (ip)
        this->mStageMapInfoP = ip;
    if ((flags & 2) && (*image0).get()) {
        if (!l_roomColorInitialized.get()) {
            l_roomColorInitialized = 1;
            dMap_copyBytes(&l_roomColor, &l_roomColorDefault, 4);
        }
        (&mTexture)->init(gabi::at<void>((*image0).get()), 0, &l_roomColor);
        f32 zero = l_zero.get(), one = l_one.get();
        (&mTexture)->setScroll(zero, zero, one, one);
        this->mTexture.mActive = 1;
        ip = (*info).get();
        if (ip) {
            this->mEnlargementScaleX =
                f32(f64(f32(sx)) / gabi::fsubs_ppc(gabi::at<MapInfo>(ip)->mFieldMaxX.get(),
                                                   gabi::at<MapInfo>(ip)->mFieldMinX.get()));
            this->mEnlargementScaleZ =
                f32(f64(f32(sy)) / gabi::fsubs_ppc(gabi::at<MapInfo>(ip)->mFieldMaxZ.get(),
                                                   gabi::at<MapInfo>(ip)->mFieldMinZ.get()));
        }
        (&mEnlargement)->init(1, &mTexture);
        flags = this->mEnableFlags.get();
    }
    if ((flags & 1) && (*data).get() && (*info).get()) {
        (&mScreen)->setImage(gabi::at<void>((*image1).get()), gabi::at<void>((*data).get()));
        this->mMapWidth = f32(dMap_cnvind16(gabi::at<MapData>((*data).get())->mWidth.get()));
        this->mMapHeight = f32(dMap_cnvind16(gabi::at<MapData>((*data).get())->mHeight.get()));
        u32 upper = (*info).get();
        if (!upper)
            dMap_showAssert("d_map.cpp", 0x2E9, "i_mapInfoP != (0)");
        u32 lower = (*info).get();
        if (!lower)
            dMap_showAssert("d_map.cpp", 0x2DD, "i_mapInfoP != (0)");
        this->mRealSizeScaleX = f32(f64(this->mMapWidth.get()) /
                                    gabi::fsubs_ppc(gabi::at<MapInfo>(upper)->mRealMaxX.get(),
                                                    gabi::at<MapInfo>(lower)->mRealMinX.get()));
        upper = (*info).get();
        if (!upper)
            dMap_showAssert("d_map.cpp", 0x2EF, "i_mapInfoP != (0)");
        lower = (*info).get();
        if (!lower)
            dMap_showAssert("d_map.cpp", 0x2E3, "i_mapInfoP != (0)");
        this->mRealSizeScaleZ = f32(f64(this->mMapHeight.get()) /
                                    gabi::fsubs_ppc(gabi::at<MapInfo>(upper)->mRealMaxZ.get(),
                                                    gabi::at<MapInfo>(lower)->mRealMinZ.get()));
        void* play = dMap_getPlay();
        void* stage = getStagInfo(static_cast<PlayInfo*>(play)->mStage);
        u32 type = (static_cast<StageInfo*>(stage)->mStageTypeAndSchbit.get() >> 16) & 7;
        if (type == 1) {
            gabi::at<MapData>((*data).get())->mShowFloorLabel =
                !(gabi::at<MapInfo>((*info).get())->mWorldUnitsPerPixel.get() <
                  l_floorLabelThreshold.get());
            gabi::at<MapData>((*data).get())->mFloorLabel =
                floor >= 128 ? floor - 127 : floor - 128;
        } else {
            play = dMap_getPlay();
            stage = getStagInfo(static_cast<PlayInfo*>(play)->mStage);
            type = (static_cast<StageInfo*>(stage)->mStageTypeAndSchbit.get() >> 16) & 7;
            if (type == 6) {
                play = dMap_getPlay();
                void* stageData = static_cast<PlayInfo*>(play)->mStage;
                u32 no = dMap_GetFloorNo(stageData, gabi::fadds_ppc(height, mapOffsetY()));
                gabi::at<MapData>((*data).get())->mFloorLabel = no >= 128 ? no - 127 : no - 128;
            } else {
                gabi::at<MapData>((*data).get())->mShowFloorLabel = 0;
                gabi::at<MapData>((*data).get())->mFloorLabel = 0;
            }
        }
    }
    return true;
}

/* 0258E6C8 */
void dMap_RoomInfo_c::deleteRoom() {
    this->mStageMapInfoP = 0;
    this->m_exist = 0;
    this->mEnableFlags = 0;
    for (u32 i = 0; i < 10; ++i)
        mFloorTable[i] = 255;
    f32 zero = l_zero.get(), one = l_one.get();
    this->mRealSizeScaleX = zero;
    this->mDspFloorNo = 255;
    this->mMapWidth = zero;
    this->m_no = 0xFFFFFFFF;
    this->mMapHeight = zero;
    this->mEnlargementScaleZ = zero;
    this->mEnlargementScaleX = zero;
    this->mRealSizeScaleZ = zero;
    (&mScreen)->init(nullptr, nullptr, 0, 0, 0, 0, 0, zero, zero, one, one);
    this->mTexture.mActive = 0;
    (&mEnlargement)->init(1, &mTexture);
}

/* 0258EC18 */
bool dMap_RoomInfoCtrl_c::roomExistenceCheck(s32 no, void* out) {
    u32 o = gabi::ea(out);
    if (!out)
        dMap_showAssert("d_map.cpp", 0xB9D, "roomInfoPP != (0)");
    (*gabi::at<be<u32>>(o)) = 0;
    u32 room = this->m_info.get();
    if (!room) {
        dMap_showAssert("d_map.cpp", 0xBA5, "roomInfoP != (0)");
        return false;
    }
    while (room) {
        if (gabi::at<dMap_RoomInfo_c>(room)->m_exist.get()) {
            if (gabi::at<dMap_RoomInfo_c>(room)->m_no.get() == u32(no)) {
                (*gabi::at<be<u32>>(o)) = room;
                // GHS performs the loop-increment load before returning a found node.
                (void)gabi::at<dMap_RoomInfo_c>(room)->m_next.get();
                return true;
            }
        } else if (!(*gabi::at<be<u32>>(o)).get())
            (*gabi::at<be<u32>>(o)) = room;
        room = gabi::at<dMap_RoomInfo_c>(room)->m_next.get();
    }
    return false;
}

/* 0258ED18 */
void* dMap_RoomInfoCtrl_c::getNextRoomP(void* room) {
    return gabi::at<void>(
        (*gabi::at<be<u32>>(room ? gabi::ea(room) + 0x124 : gabi::ea(this) + 4)).get());
}

/* 0258ECE0 */
bool dMap_RoomInfoCtrl_c::deleteRoom(s32 room) {
    gabi::Local<gabi::be<u32>> found;
    if (static_cast<dMap_RoomInfoCtrl_c*>(this)->roomExistenceCheck(room, found.get()))
        dMap_deleteRoomEntry(gabi::at<void>((*found).get()));
    return false;
}

/* 0258EDDC */
void dMap_loadFmapResource() {
    dMap_loadResource(gabi::guest_addr("Misc"), gabi::guest_addr("Misc.bfres"),
                      gabi::guest_addr("common.jpc"), gabi::guest_addr("FmapDat.bin"), 0x1047A874);
}

/* 0258EEBC */
void dMap_loadDmapResource() {
    dMap_loadResource(gabi::guest_addr("Misc"), gabi::guest_addr("Misc.bfres"),
                      gabi::guest_addr("common.jpc"), gabi::guest_addr("cmapdat.bin"), 0x1047A878);
}

/* 0258EFC8 */
void dMap_createResources() {
    mDmapResourceP = 0;
    mFmapResourceP = 0;
    dMap_loadFmapResource();
    dMap_loadDmapResource();
    mFmapChkPntDataP = 0;
    mFmapChkPntValue = 0;
    dMap_setFmapChkDtPrm();
}

/* 0258F594 */
bool dMap_isEnableEnlargementScroll() {
    void* play = dMap_getPlay();
    void* stage = getStagInfo(static_cast<PlayInfo*>(play)->mStage);
    return ((static_cast<StageInfo*>(stage)->mStageTypeAndSchbit.get() >> 16) & 7) == 7;
}

/* 0258F01C */
s32 dMap_getKindMapType() {
    void* play = dMap_getPlay();
    void* stage = getStagInfo(static_cast<PlayInfo*>(play)->mStage);
    u32 t = (static_cast<StageInfo*>(stage)->mStageTypeAndSchbit.get() >> 16) & 7;
    return t == 1 || t == 5 || t == 6 ? 2 : t == 7;
}

/* 0258F1D8 */
void dMap_deleteImage(s32 room) {
    if (room >= 0) {
        static_cast<dMap_RoomInfoCtrl_c*>(&mRoomInfoCtrl)->deleteRoom(room);
        if (u32(room) == u32(mStayRoomNo.get()))
            mNowRoomInfoP = 0;
    }
}

/* 0258F08C */
bool dMap_setNowRoom(s32 no) {
    mNowRoomInfoP = 0;
    void* room = static_cast<dMap_RoomInfoCtrl_c*>(&mRoomInfoCtrl)->getNextRoomP(nullptr);
    while (room && static_cast<dMap_RoomInfo_c*>(room)->m_no.get() != u32(no))
        room = static_cast<dMap_RoomInfoCtrl_c*>(&mRoomInfoCtrl)->getNextRoomP(room);
    if (!room)
        return false;
    u32 a = gabi::ea(room);
    mNowRoomInfoP = a;
    if (gabi::at<dMap_RoomInfo_c>(a)->mEnableFlags.get() & 2) {
        u32 info =
            dMap_info(a, gabi::guest_addr("d_map.h"), gabi::guest_addr("mStageMapInfoP != (0)"));
        a = mNowRoomInfoP.get();
        mEnlargementSizeCenterX = gabi::at<MapInfo>(info)->mFieldCenterX.get();
        info = dMap_info(a, gabi::guest_addr("d_map.h"), gabi::guest_addr("mStageMapInfoP != (0)"));
        a = mNowRoomInfoP.get();
        mEnlargementSizeCenterZ = gabi::at<MapInfo>(info)->mFieldCenterZ.get();
        mEnlargementSizeScaleX = gabi::at<dMap_RoomInfo_c>(a)->mEnlargementScaleX.get();
        mEnlargementSizeScaleZ = gabi::at<dMap_RoomInfo_c>(a)->mEnlargementScaleZ.get();
    }
    if (dMap_getKindMapType() != 1 && !mAGBMapSendStopFlg.get() && mAGBMapSendStatus.get())
        mAGBMapSendStatus = 0;
    return true;
}

/* 0258E0C4 */
void dMap_2DSQ_draw() {}

/* 0258F568 */
void dMap_setPlayerStayAgbMapTypeNow(f32 x, f32 y) {
    mPlayerStayAgbMapTypeNow = dMap_getPosAgbMapType(1, x, y);
}

// HD: the GBA notification entry reduces to the map-kind query.
/* 0258F8DC */
u32 dMap_agbMapNoSetCall() { return (u32)dMap_getKindMapType(); }

/* 0258F234 */
bool dMap_isInDspArea(s32 detailed, f32 x, f32 y) {
    u32 room = mNowRoomInfoP.get();
    if (!room)
        return false;
    u32 flag = gabi::at<dMap_RoomInfo_c>(room)->mEnableFlags.get();
    u32 base = detailed ? 24 : 0;
    if (!(flag & (detailed ? 1 : 2)))
        return false;
    for (u32 i = 0; i < 4; ++i) {
        u32 off = base + (i == 0 ? 0 : i == 1 ? 8 : i == 2 ? 4 : 12);
        f32 b;
        if (detailed) {
            const u32 file[] = {l_realMinXFile, l_realMaxXFile, l_realMinZFile, l_realMaxZFile};
            const u32 expr[] = {l_realMinXAssertion, l_realMaxXAssertion, l_realMinZAssertion,
                                l_realMaxZAssertion};
            const u32 line[] = {0x2DD, 0x2E9, 0x2E3, 0x2EF};
            b = dMap_checkedBound(room, off, line[i], gabi::guest_addr("d_map.h"),
                                  gabi::guest_addr("mStageMapInfoP != (0)"), file[i], expr[i]);
        } else
            b = readMapBound(dMap_info(room, gabi::guest_addr("d_map.h"),
                                       gabi::guest_addr("mStageMapInfoP != (0)")),
                             off);
        if ((i % 2 == 0 ? (i < 2 ? x : y) < b : (i < 2 ? x : y) > b))
            return false;
        if (i < 3)
            room = mNowRoomInfoP.get();
    }
    return true;
}

/* 0258F8E0 */
void dMap_mapAGBSendMapMain() {
    if (dMap_getKindMapType() == 1) {
        u32 a = mPlayerStayAgbMapTypeNow.get(), b = mPlayerStayAgbMapTypeOld.get();
        if ((a == 2 && b != 2) || (a == 3 && b != 3)) {
            dMap_agbMapNoSetCall();
            mAGBMapSendStatus = 0;
        }
    }
}

/* 0258F5D8 */
void dMap_calcEnlargementSizeParameter(f32 x, f32 y) {
    if (!dMap_isEnableEnlargementScroll())
        return;
    u32 room = mNowRoomInfoP.get();
    if (!(gabi::at<dMap_RoomInfo_c>(room)->mEnableFlags.get() & 2))
        return;
    u32 q = dMap_scrollInfo(room), r = gabi::at<dMap_RoomInfo_c>(room)->mStageMapInfoP.get();
    f32 upperX = gabi::at<MapInfo>(q)->mFieldMaxX.get();
    if (!r) {
        dMap_showAssert("d_map.h", 0x46B, "mStageMapInfoP != (0)");
        r = gabi::at<dMap_RoomInfo_c>(room)->mStageMapInfoP.get();
        room = mNowRoomInfoP.get();
    }
    f32 midX = gabi::at<MapInfo>(r)->mFieldCenterX.get(), dx = gabi::fsubs_ppc(upperX, midX),
        half = l_scrollRange.get();
    r = gabi::at<dMap_RoomInfo_c>(room)->mStageMapInfoP.get();
    f32 maxX = gabi::fmuls_ppc(__builtin_fabsf(dx), half);
    if (!r) {
        dMap_showAssert("d_map.h", 0x46B, "mStageMapInfoP != (0)");
        r = gabi::at<dMap_RoomInfo_c>(room)->mStageMapInfoP.get();
        room = mNowRoomInfoP.get();
    }
    q = gabi::at<dMap_RoomInfo_c>(room)->mStageMapInfoP.get();
    f32 upperY = gabi::at<MapInfo>(r)->mFieldMaxZ.get();
    if (!q) {
        dMap_showAssert("d_map.h", 0x46B, "mStageMapInfoP != (0)");
        q = gabi::at<dMap_RoomInfo_c>(room)->mStageMapInfoP.get();
        room = mNowRoomInfoP.get();
    }
    f32 midY = gabi::at<MapInfo>(q)->mFieldCenterZ.get(), dy = gabi::fsubs_ppc(upperY, midY);
    r = gabi::at<dMap_RoomInfo_c>(room)->mStageMapInfoP.get();
    f32 maxY = gabi::fmuls_ppc(__builtin_fabsf(dy), l_scrollRange.get());
    if (!r) {
        dMap_showAssert("d_map.h", 0x46B, "mStageMapInfoP != (0)");
        r = gabi::at<dMap_RoomInfo_c>(room)->mStageMapInfoP.get();
        room = mNowRoomInfoP.get();
    }
    dx = gabi::fsubs_ppc(x, gabi::at<MapInfo>(r)->mFieldCenterX.get());
    r = dMap_scrollInfo(room);
    dy = gabi::fsubs_ppc(y, gabi::at<MapInfo>(r)->mFieldCenterZ.get());
    f32 ax = __builtin_fabsf(dx), ay = __builtin_fabsf(dy), one = l_one.get(),
        minus = l_minusOne.get(), zero = l_zero.get();
    f32 signX = dx >= 0 ? one : minus, signY = dy >= 0 ? one : minus, scrollX = zero,
        scrollY = zero;
    if (ax > maxX) {
        f32 extra = gabi::fsubs_ppc(ax, maxX);
        scrollX = gabi::fadds_ppc(extra, extra);
        if (scrollX > ax)
            scrollX = ax;
    }
    if (ay > maxY) {
        f32 extra = gabi::fsubs_ppc(ay, maxY);
        scrollY = gabi::fadds_ppc(extra, extra);
        if (scrollY > ay)
            scrollY = ay;
    }
    if (scrollX != zero || scrollY != zero) {
        if (scrollX > scrollY)
            scrollY = f32(f64(gabi::fmuls_ppc(ay, scrollX)) / ax);
        else
            scrollX = f32(f64(gabi::fmuls_ppc(ax, scrollY)) / ay);
    }
    r = dMap_scrollInfo(room);
    mEnlargementSizeCenterX =
        gabi::fmadds(signX, scrollX, gabi::at<MapInfo>(r)->mFieldCenterX.get());
    r = dMap_scrollInfo(room);
    mEnlargementSizeCenterZ =
        gabi::fmadds(signY, scrollY, gabi::at<MapInfo>(r)->mFieldCenterZ.get());
    mEnlargementSizeScaleX = gabi::at<dMap_RoomInfo_c>(room)->mEnlargementScaleX.get();
    mEnlargementSizeScaleZ = gabi::at<dMap_RoomInfo_c>(room)->mEnlargementScaleZ.get();
}

// HD: scissor calculation only clears the four AGB counters.
/* 0258F950 */
void dMap_calcScissor() {
    mAGBPointValueE = 0;
    mAGBPointValueAll = 0;
    mAGBPointValueC = 0;
    mAGBPointValueRoomAll = 0;
}

/* 0258FAD0 */
void dMap_mapMoveAll(s32 roomNo, f32 x, f32 y, f32 height) {
    dMap_setPlayerStayAgbMapTypeNow(x, y);
    void* play = dMap_getPlay();
    void* stage = getStagInfo(static_cast<PlayInfo*>(play)->mStage);
    u32 t = (static_cast<StageInfo*>(stage)->mStageTypeAndSchbit.get() >> 16) & 7;
    if (t != 5 && t != 7 && u32(roomNo) < 64)
        dMap_onVisitedRoom(gabi::at<void>(mSaveInfoP.get() + 0x798), roomNo);
    u32 current = mNowRoomInfoP.get();
    if (roomNo != -1 && !current) {
        dMap_setNowRoom(roomNo);
        current = mNowRoomInfoP.get();
    }
    if (!current)
        return;
    u32 floor = dMap_GetFloorNo_WithRoom(roomNo, height);
    current = mNowRoomInfoP.get();
    if (current && floor != mNowFloorNo.get()) {
        u32 old = gabi::at<dMap_RoomInfo_c>(current)->mDspFloorNo.get();
        mNowFloorNo = floor;
        static_cast<dMap_RoomInfoCtrl_c*>(&mRoomInfoCtrl)
            ->checkFloorMoveImageChangeRoom(floor, floor, roomNo, 120, 120, height);
        current = mNowRoomInfoP.get();
        if (old != gabi::at<dMap_RoomInfo_c>(current)->mDspFloorNo.get()) {
            mAGBMapSendStopFlg = 0;
            mAGBMapSendStatus = 0;
        }
    }
    if (roomNo != -1 && gabi::at<dMap_RoomInfo_c>(current)->m_no.get() != u32(roomNo)) {
        dMap_setNowRoom(roomNo);
        current = mNowRoomInfoP.get();
    }
    if (!current)
        return;
    dMap_calcEnlargementSizeParameter(x, y);
    dMap_mapAGBSendMapMainEntry(x, y);
    current = mNowRoomInfoP.get();
    u32 flags = gabi::at<dMap_RoomInfo_c>(current)->mEnableFlags.get();
    if (mMapDispMode.get() == 1) {
        if (flags & 2) {
            mNowCenterX = mEnlargementSizeCenterX.get();
            mNowCenterZ = mEnlargementSizeCenterZ.get();
            mNowScaleX = mEnlargementSizeScaleX.get();
            mNowScaleZ = mEnlargementSizeScaleZ.get();
        }
    } else if (flags & 1) {
        mNowCenterX = x;
        mNowCenterZ = y;
        mNowScaleX = gabi::at<dMap_RoomInfo_c>(current)->mRealSizeScaleX.get();
        mNowScaleZ = gabi::at<dMap_RoomInfo_c>(current)->mRealSizeScaleZ.get();
    }
    dMap_calcScissor();
    current = mNowRoomInfoP.get();
    u32 info = gabi::at<dMap_RoomInfo_c>(current)->mStageMapInfoP.get(), alpha = 0;
    if (info)
        alpha = (gabi::at<MapInfo>(info)->mAlpha.get() * mAlpha.get() >> 8) & 255;
    mCompAlpha = alpha;
    u32 player = mSaveInfoP.get();
    dMap_setCollectPoint(20, 9, gabi::at<SaveInfo>(player)->mRestartRoomNo.get(),
                         gabi::at<SaveInfo>(player)->mRestartRoomAngleY.get(), 0, 0, 0, 0,
                         gabi::at<SaveInfo>(player)->mRestartX.get(),
                         gabi::at<SaveInfo>(player)->mRestartY.get(),
                         gabi::at<SaveInfo>(player)->mRestartZ.get());
}

/* 0258FD88 */
void dMap_draw(s32 roomNo, f32 x, f32 y, f32 height) {
    if (roomNo >= 0)
        dMap_mapMoveAll(roomNo, x, y, height);
}

/* 0259058C */
void dMap_point2Grid(void* gx, void* gy, f32 x, f32 y) {
    if (!gx)
        dMap_showAssert("d_map.cpp", 0x16DC, "i_gridX_p != (0)");
    if (!gy)
        dMap_showAssert("d_map.cpp", 0x16DD, "i_gridY_p != (0)");
    f32 size = l_gridSize.get(), bias = l_gridBias.get();
    (*gabi::at<be<u8>>(gabi::ea(gx))) = dMap_clampGrid(dMap_gridIndex(x, size, bias));
    (*gabi::at<be<u8>>(gabi::ea(gy))) = dMap_clampGrid(dMap_gridIndex(y, size, bias));
}

/* 0258FD94 */
void dMap_point2GridAndLocal(void* gx, void* gy, void* lx, void* ly, f32 x, f32 y) {
    if (!gx)
        dMap_showAssert("d_map.cpp", 0x1710, "i_gridX_p != (0)");
    if (!gy)
        dMap_showAssert("d_map.cpp", 0x1711, "i_gridY_p != (0)");
    if (!lx)
        dMap_showAssert("d_map.cpp", 0x1712, "i_localX_p != (0)");
    if (!ly)
        dMap_showAssert("d_map.cpp", 0x1713, "i_localY_p != (0)");
    f32 size = l_gridSize.get(), bias = l_gridBias.get();
    s32 ix = dMap_clampGrid(dMap_gridIndex(x, size, bias)),
        iy = dMap_clampGrid(dMap_gridIndex(y, size, bias));
    s32 localX = gabi::ftoi(gabi::fnmsubs(f32(ix), size, x));
    s32 localY = gabi::ftoi(gabi::fnmsubs(f32(iy), size, y));
    (*gabi::at<be<u8>>(gabi::ea(gx))) = ix;
    (*gabi::at<be<u8>>(gabi::ea(gy))) = iy;
    (*gabi::at<be<u16>>(gabi::ea(lx))) = localX;
    (*gabi::at<be<u16>>(gabi::ea(ly))) = localY;
}

/* 0258EF9C */
void dMap_setFmapChkDtPrm() {
    u32 p = mFmapResourceP.get();
    if (p) {
        u32 n = gabi::at<Resource>(p)->mDataOffset.get();
        mFmapChkPntDataP = p + 4;
        mFmapChkPntValue = n;
    }
}

/* 0258F978 */
void dMap_setCollectPoint(s32 kind, u32 type, s32 room, s32 angle, u32 frame, u32 prm, u32 cursor,
                          u32 sub, f32 x, f32 y, f32 z) {
    if (mCollectPointDataCnt.get() >= 64)
        return;
    if (dMap_getKindMapType() != 1 && kind != 1 && kind != 3 && room != -1) {
        u32 current = mNowRoomInfoP.get();
        if (!current || gabi::at<dMap_RoomInfo_c>(current)->m_no.get() != u32(room))
            return;
    }
    u32 index = u32(kind) - 1;
    if (index >= 21)
        dMap_showAssert("d_map.cpp", 0x1A2B, "(typeNo >= 0) && (typeNo < AGB_POINT_TYPE_NUM)");
    u32 count = mCollectPointDataCnt.get(), p = gabi::ea(&mCollectPointData) + count * 24;
    mCollectPointDataCnt = count + 1;
    u32 next = (*gabi::at<be<u8>>(gabi::ea(&mCollectPointDataLinkList) + index)).get();
    (*gabi::at<be<u8>>(gabi::ea(&mCollectPointDataLinkList) + index)) = count;
    gabi::at<CollectPoint>(p)->mPosZ = z;
    gabi::at<CollectPoint>(p)->mRoomNo = room;
    gabi::at<CollectPoint>(p)->mAngle = angle;
    gabi::at<CollectPoint>(p)->mAcsType = sub;
    gabi::at<CollectPoint>(p)->mPosY = y;
    gabi::at<CollectPoint>(p)->mPosX = x;
    gabi::at<CollectPoint>(p)->mParameter = prm;
    gabi::at<CollectPoint>(p)->mGcType = type;
    gabi::at<CollectPoint>(p)->mNext = next;
    gabi::at<CollectPoint>(p)->mCursor = cursor;
    gabi::at<CollectPoint>(p)->mAgbType = kind;
    gabi::at<CollectPoint>(p)->mFrame = frame;
}

/* 0258F4C8 */
s32 dMap_getPosAgbMapType(s32 detailed, f32 x, f32 y) {
    if (dMap_getKindMapType() == 1)
        return dMap_isInDspArea(detailed, x, y) ? 3 : 2;
    return dMap_getKindMapType() == 2;
}

/* 02590004 */
void dMap_setArriveInfo(f32 x, f32 y) {
    void* play = dMap_getPlay();
    if (static_cast<PlayInfo*>(play)->mEventRunning.get())
        return;
    gabi::Local<MapResourceName> desired, current;
    desired->mString = gabi::guest_addr("sea");
    desired->mVTable = SafeString_vtable;
    play = dMap_getPlay();
    current->mString = gabi::ea(static_cast<PlayInfo*>(play)->mStartStageName);
    current->mVTable = SafeString_vtable;
    for (u32 i = 0; i < 2; ++i)
        gabi::call_ptr<void>(gabi::at<SafeStringVTable>(desired->mVTable)->assureTermination.get(),
                             desired.get());
    u32 first = desired->mString.get();
    gabi::call_ptr<void>(gabi::at<SafeStringVTable>(current->mVTable)->assureTermination.get(),
                         current.get());
    u32 second = current->mString.get();
    bool equal = first == second;
    if (!equal)
        for (u32 i = 0; i < 0x40001; ++i) {
            u32 a = (*gabi::at<be<u8>>(first + i)).get(), b = (*gabi::at<be<u8>>(second + i)).get();
            if (a != b)
                break;
            if (!a) {
                equal = true;
                break;
            }
        }
    if (!equal)
        return;
    gabi::Local<Grid> grid;
    gabi::Local<gabi::be<u16>> lx, ly;
    dMap_point2GridAndLocal(gabi::at<void>(grid.a), gabi::at<void>(grid.a + 1), lx.get(), ly.get(),
                            x, y);
    s32 gx = grid->x.get(), gy = grid->y.get();
    if (gx == mPlGridX.get() && gy == mPlGridY.get())
        return;
    mPlGridX = gx;
    mPlGridY = gy;
    if (u32(gx + 3) < 7 && u32(gy + 3) < 7)
        dMap_onSaveArriveGrid(gabi::at<void>(mSaveInfoP.get() + 0xE4), gx + gy * 7 + 24);
    dMap_agbMapNoSetCall();
}

/* 02590208 */
void dMap_getTypeAgbGcFromTypeAcs(u32 kind, void* agb, void* gc) {
    if (kind >= 25)
        dMap_showAssert("d_map.cpp", 0x1F98,
                        "(i_typeAcs >= 0) && (i_typeAcs < ACS_MAP_POINT_TYPE_MAX)");
    if (agb)
        (*gabi::at<be<u8>>(gabi::ea(agb))) =
            (*gabi::at<be<u8>>(gabi::ea(&statusMapNo2TypeNoTbl) + kind * 2)).get();
    if (gc)
        (*gabi::at<be<u8>>(gabi::ea(gc))) =
            (*gabi::at<be<u8>>((gabi::ea(&statusMapNo2TypeNoTbl) + 1) + kind * 2)).get();
}

/* 02590488 */
void dMap_drawPointSingle(s32 kind, s32 room, s32 angle, u32 frame, u32 prm, u32 cursor, f32 x,
                          f32 y, f32 z) {
    if (mStayRoomNo.get() < 0 || !mNowRoomInfoP.get())
        return;
    s32 index = kind - 1;
    if (index < 0 || u32(index) >= 21) {
        dMap_showAssert("d_map.cpp", 0x1FEF, "0");
        return;
    }
    s32 type = (*gabi::at<be<s8>>(gabi::ea(&agbTbl) + index)).get();
    if (type < 0) {
        dMap_showAssert("d_map.cpp", 0x1FF6, "0");
        return;
    }
    dMap_setCollectPoint(kind, u8(type), room, angle, frame, prm, cursor, 0, x, y, z);
}

/* 0259029C */
void dMap_drawActorPointMiniMap(void* actor) {
    if (!mNowRoomInfoP.get())
        return;
    u32 a = gabi::ea(actor), acs = gabi::at<Actor>(a)->mStatus.get() & 31;
    gabi::Local<PointTypes> types;
    dMap_getTypeAgbGcFromTypeAcs(acs, gabi::at<void>(types.a), gabi::at<void>(types.a + 1));
    u32 type = types->mAgbType.get(), frame = 0, cursor = 0;
    s32 room = gabi::at<Actor>(a)->mRoomNo.get(), angle = -32768;
    if (type == 4) {
        void* play = dMap_getPlay();
        if (static_cast<PlayInfo*>(play)->mPlayerStatus.get() & 0x10000)
            return;
        angle = gabi::at<Actor>(a)->mShapeAngleY.get();
    } else if (type == 17) {
        angle = gabi::at<Actor>(a)->mHomeAngleY.get();
        if (acs != 11)
            room = gabi::at<Actor>(a)->mMapRoomNo.get();
    } else if (type == 1) {
        dMap_setArriveInfo(gabi::at<Actor>(a)->mCurrentX.get(),
                           gabi::at<Actor>(a)->mCurrentZ.get());
        angle = gabi::at<Actor>(a)->mShapeAngleY.get();
        if (acs >= 17 && acs <= 20)
            frame = acs - 16;
    } else if (type == 19 && acs >= 12 && acs <= 15)
        frame = acs - 11;
    type = types->mAgbType.get();
    if (type == 2) {
        void* play = dMap_getPlay();
        void* attention = static_cast<PlayInfo*>(play)->mAttention;
        if (dMap_getAttentionTarget(attention, 0) == actor) {
            cursor = 1;
            if (dMap_isAttentionLock(attention))
                cursor = 2;
        }
    }
    dMap_setCollectPoint(types->mAgbType.get(), types->mGcType.get(), room, angle, frame,
                         gabi::at<Actor>(a)->mMapParameter.get(), cursor, acs & 255,
                         gabi::at<Actor>(a)->mCurrentX.get(), gabi::at<Actor>(a)->mCurrentY.get(),
                         gabi::at<Actor>(a)->mCurrentZ.get());
}

/* 0258ED34 */
void dMap_RoomInfoCtrl_c::checkFloorMoveImageChangeRoom(u32 a, u32 b, s32 c, s32 x, s32 y,
                                                        f32 height) {
    void* room = static_cast<dMap_RoomInfoCtrl_c*>(this)->getNextRoomP(nullptr);
    while (room) {
        if (static_cast<dMap_RoomInfo_c*>(room)->m_exist.get())
            dMap_changeImageEntry(room, a, b, c, x, y, height);
        room = static_cast<dMap_RoomInfoCtrl_c*>(this)->getNextRoomP(room);
    }
}

// HD: GX setup is absent; the release entry keeps only index, color and activation.
/* 0258DEE8 */
void dMap_2DMtMapSpcl_tex_c::init(void* image, u32 index, void* color) {
    this->mTlutName = index;
    this->mIndexed = static_cast<Image*>(image)->indexTexture.get() != 0;
    u32 value = 0;
    for (u32 i = 0; i < 4; ++i)
        value = (value << 8) | static_cast<Color*>(color)->rgba[i].get();
    for (u32 i = 0; i < 4; ++i)
        mColor.rgba[i] = value >> (24 - i * 8);
    this->mActive = 1;
}

/* 0258DF14 */
void dMap_2DMtMapSpcl_tex_c::setScroll(f32 x, f32 y, f32 w, f32 h) {
    this->mScrollWidth = w;
    this->mScrollY = y;
    this->mScrollHeight = h;
    this->mScrollX = x;
}

/* 0258DF9C */
void dMap_2DMtMapSpcl_c::init(s32 num, void* tex) {
    this->mTextures = gabi::ea(tex);
    this->mTextureCount = num;
    this->mBlendMode = 0;
}

/* 0258E1E4 */
void dMap_2DMtMapSpcl_draw() {}

/* 0258DD0C */
void dMap_2DAGBScrDsp_c::setImage(void* image, void* data) {
    this->mMapData = gabi::ea(data);
    this->mImg = gabi::ea(image);
    if (!data || !image)
        this->mMapBytes = 0;
    else {
        u32 first = dMap_cnvind32(static_cast<MapData*>(data)->mMapOffset.get());
        u32 d = this->mMapData.get();
        u32 base = first + 0x3C;
        u32 second = dMap_cnvind32(gabi::at<MapData>(d)->mImageOffset.get());
        this->mMapBytes = base + second;
    }
}

/* 0258DD80 */
void dMap_2DAGBScrDsp_c::init(void* data, void* image, s32 x, s32 y, s32 width, s32 height,
                              u32 alpha, f32 px, f32 py, f32 sx, f32 sy) {

    static_cast<dMap_2DAGBScrDsp_c*>(this)->setImage(image, data);
    this->mLeft = x;
    this->mScaleY = sy;
    this->mCenterX = px;
    this->mScaleX = sx;
    this->mHeight = height;
    this->mCenterY = py;
    this->mAlpha = alpha;
    this->mWidth = width;
    this->mTop = y;
}

// HD: the GBA screen draw entry is inert.
/* 0258DD08 */
void dMap_2DAGBScrDsp_c::draw() {}

/* 02590CE0 */
void dMap_2DTri_draw() {}

/* 02590D38 */
void dMap_2DPoint_draw() {}

/* 02590DE4 */
void dMap_2DAGBCursor_draw() {}

/* 02590E3C */
void dMap_2DT2_draw() {}

// HD: compiler-generated and template code of this translation unit.
// Constructors retain explicit storage arguments: a NULL entry allocates guest storage.

/* 0258DC50 */
void* dMap_2DAGBScrDsp__ct(void* self) {
    if (!self)
        self = dMap_operatorNew(0x78);
    if (!self)
        return nullptr;
    auto* i_this = static_cast<dMap_2DAGBScrDsp_c*>(self);
    u32 a = gabi::ea(self);
    dMap_drawlistBaseConstruct(self);
    i_this->mImg = 0;
    i_this->mMapData = 0;
    i_this->mVTable = ScrDsp_vtable;
    dMap_bzero(gabi::at<void>(a + 12), 64);
    dMap_bzero(gabi::at<void>(a + 0x4C), 12);
    i_this->mHeight = 0;
    i_this->mAlpha = 0;
    f32 zero = l_zero.get();
    i_this->mHDState = 0;
    i_this->mCenterX = zero;
    i_this->mCenterY = zero;
    i_this->mScaleY = zero;
    i_this->mScaleX = zero;
    i_this->mMapBytes = 0;
    i_this->mLeft = 0;
    i_this->mHDFlags = 0;
    i_this->mWidth = 0;
    i_this->mTop = 0;
    return self;
}

/* 0258DE54 */
void* dMap_2DMtMapSpcl_tex__ct(void* self) {
    if (!self)
        self = dMap_operatorNew(0x68);
    if (!self)
        return nullptr;
    auto* i_this = static_cast<dMap_2DMtMapSpcl_tex_c*>(self);
    u32 a = gabi::ea(self);
    i_this->mActive = 0;
    i_this->mIndexed = 0;
    dMap_bzero(gabi::at<void>(a + 4), 64);
    dMap_bzero(gabi::at<void>(a + 0x44), 12);
    dMap_bzero(gabi::at<void>(a + 0x50), 4);
    f32 zero = l_zero.get();
    i_this->mTlutName = 0;
    i_this->mScrollWidth = zero;
    i_this->mScrollHeight = zero;
    i_this->mScrollX = zero;
    i_this->mScrollY = zero;
    return self;
}

/* 0258DF28 */
void* dMap_2DMtMapSpcl__ct(void* self) {
    if (!self)
        self = dMap_operatorNew(20);
    if (!self)
        return nullptr;
    auto* i_this = static_cast<dMap_2DMtMapSpcl_c*>(self);
    dMap_drawlistBaseConstruct(self);
    i_this->mTextures = 0;
    i_this->mVTable = Spcl_vtable;
    i_this->mTop = 0;
    i_this->mRight = 0;
    i_this->mBottom = 0;
    i_this->mBlendMode = 0;
    i_this->mTextureCount = 0;
    i_this->mLeft = 0;
    return self;
}

/* 0258DFB0 */
void* dMap_2DSQ__ct(void* self) {
    if (!self)
        self = dMap_operatorNew(32);
    if (!self)
        return nullptr;
    auto* i_this = static_cast<dMap_2DSQ_c*>(self);
    u32 a = gabi::ea(self);
    dMap_drawlistBaseConstruct(self);
    i_this->mMode = 0;
    i_this->mAlpha = 0;
    i_this->mVTable = SQ_vtable;
    i_this->mDispRight = 0;
    i_this->mDispBottom = 0;
    i_this->mDispTop = 0;
    i_this->mDispLeft = 0;
    dMap_bzero(gabi::at<void>(a + 28), 4);
    return self;
}

/* 0258E02C */
void* dMap_2DTri__ct(void* self) {
    if (!self)
        self = dMap_operatorNew(40);
    if (!self)
        return nullptr;
    auto* i_this = static_cast<dMap_2DTri_c*>(self);
    u32 a = gabi::ea(self);
    dMap_drawlistBaseConstruct(self);
    i_this->mPosX = 0;
    i_this->mPosY = 0;
    i_this->mVTable = Tri_vtable;
    dMap_bzero(gabi::at<void>(a + 8), 4);
    i_this->mScissorX = 0;
    f32 zero = l_zero.get();
    i_this->mAngle = 0;
    i_this->mScaleX = zero;
    i_this->mScaleY = zero;
    i_this->mScissorWidth = 0;
    i_this->mScissorHeight = 0;
    i_this->mScissorY = 0;
    return self;
}

/* 0258E0D4 */
void* dMap_2DPoint__ct(void* self) { return dMap_pointCtor(self, Point_vtable); }

/* 0258E15C */
void* dMap_2DAGBCursor__ct(void* self) { return dMap_pointCtor(self, AGBCursor_vtable); }

/* 02590E40 */
void* dMap_2DT2__ct(void* self) {
    if (!self)
        self = dMap_operatorNew(0x7C);
    if (self) {
        dMap_drawlistBaseConstruct(self);
        static_cast<dMap_2DT2_c*>(self)->mVTable = T2_vtable;
    }
    return self;
}

/* 02590EE8 */
void* dMap_RoomInfo__ct(void* self) {
    if (!self)
        self = dMap_operatorNew(0x12C);
    if (self) {
        u32 a = gabi::ea(self);
        dMap_2DMtMapSpcl__ct(gabi::at<void>(a + 0x30));
        dMap_constructArray(gabi::at<void>(a + 0x44), 1, 0x68,
                            gabi::guest_fn((const void*)dMap_2DMtMapSpcl_tex__ct));
        dMap_2DAGBScrDsp__ct(gabi::at<void>(a + 0xAC));
    }
    return self;
}

/* 02590F50 */
void dMap_RoomInfo__dt(void* self, u32 flags) {
    if (self) {
        dMap_drawlistBaseDestruct(gabi::at<void>(gabi::ea(self) + 0xAC), 0);
        dMap_drawlistBaseDestruct(gabi::at<void>(gabi::ea(self) + 0x30), 0);
        if (flags & 1)
            dMap_operatorDelete(self);
    }
}

/* 02590C8C */
void dMap_2DMtMapSpcl__dt(void* self, u32 flags) { dMap_drawableDtor(self, flags); }

/* 02590CE4 */
void dMap_2DSQ__dt(void* self, u32 flags) { dMap_drawableDtor(self, flags); }

/* 02590D3C */
void dMap_2DTri__dt(void* self, u32 flags) { dMap_drawableDtor(self, flags); }

/* 02590D90 */
void dMap_2DPoint__dt(void* self, u32 flags) { dMap_drawableDtor(self, flags); }

/* 02590DE8 */
void dMap_2DAGBCursor__dt(void* self, u32 flags) { dMap_drawableDtor(self, flags); }

/* 02590E94 */
void dMap_2DT2__dt(void* self, u32 flags) { dMap_drawableDtor(self, flags); }

/* 02590FB0 */
void dMap_SafeString_assureTermination() {}

/* 02590FB4 */
void dMap_SafeString_destruct() {}

/* 02590C78 */
void dMap_2DMtMapSpcl_tex__dt(void* self, u32 flags) {
    if (self && (flags & 1))
        dMap_operatorDelete(self);
}

/* 02590BA0 */
void dMap_destroyRooms() {
    dMap_destroyArray(&mRoomInfo, 20, 300, gabi::guest_fn((const void*)dMap_RoomInfo__dt), 0, 0);
}

/* 02590BC4 */
void dMap_destroyFrames() {
    dMap_destroyArray(&mFrameTex, 8, 20, gabi::guest_fn((const void*)dMap_2DMtMapSpcl__dt), 0, 0);
}

/* 02590BE8 */
void dMap_destroyPoints() {
    dMap_destroyArray(&mPoint, 15, 32, gabi::guest_fn((const void*)dMap_2DPoint__dt), 0, 0);
}

/* 02590C0C */
void dMap_destroyTboxes() {
    dMap_destroyArray(&mTbox, 8, 124, gabi::guest_fn((const void*)dMap_2DT2__dt), 0, 0);
}

/* 02590C30 */
void dMap_destroyDoors() {
    dMap_destroyArray(&mDoor, 16, 124, gabi::guest_fn((const void*)dMap_2DT2__dt), 0, 0);
}

/* 02590C54 */
void dMap_destroyFriends() {
    dMap_destroyArray(&mPointFriend, 3, 32, gabi::guest_fn((const void*)dMap_2DPoint__dt), 0, 0);
}

/* 025908FC */
void dMap__sinit() {

    mHDMapDisplayState = 0;
    mHDMapState = 0;
    mHDMapDisplayStateOld = 0;
    mHDMapStateOld = 0;
    dMap_registerGlobal(&l_globalDestructor0);
    f32 a = l_iconFreeScale.get(), b = l_iconSelfScale.get();
    mIconFreeScale = a;
    mIconSelfScale = b;
    dMap_constructHeaderByte(&l_mapHeader);
    dMap_registerGlobal(&l_globalDestructor1);
    dMap_constructArchiveByte(&l_archiveHeader);
    dMap_registerGlobal(&l_globalDestructor2);
    dMap_constructArray(&mRoomInfo, 20, 300, gabi::guest_fn((const void*)dMap_RoomInfo__ct));
    dMap_registerGlobal(&l_globalDestructor3);
    dMap_constructArray(&mFrameTex, 8, 20, gabi::guest_fn((const void*)dMap_2DMtMapSpcl__ct));
    dMap_registerGlobal(&l_globalDestructor4);
    dMap_constructArray(&mFrameTexture, 8, 104,
                        gabi::guest_fn((const void*)dMap_2DMtMapSpcl_tex__ct));
    dMap_2DMtMapSpcl__ct(&mIconFreeTex);
    dMap_registerGlobal(&l_globalDestructor5);
    dMap_constructArray(&mIconFreeTexture, 1, 104,
                        gabi::guest_fn((const void*)dMap_2DMtMapSpcl_tex__ct));
    dMap_2DMtMapSpcl__ct(&mIconSelfTex);
    dMap_registerGlobal(&l_globalDestructor6);
    dMap_constructArray(&mIconSelfTexture, 1, 104,
                        gabi::guest_fn((const void*)dMap_2DMtMapSpcl_tex__ct));
    dMap_2DTri__ct(&mCursor);
    dMap_registerGlobal(&l_globalDestructor7);
    dMap_2DAGBCursor__ct(&mAgbCursor);
    dMap_registerGlobal(&l_globalDestructor8);
    dMap_constructArray(&mPoint, 15, 32, gabi::guest_fn((const void*)dMap_2DPoint__ct));
    dMap_registerGlobal(&l_globalDestructor9);
    dMap_2DSQ__ct(&m2DSQdraw);
    dMap_registerGlobal(&l_globalDestructor10);
    dMap_2DSQ__ct(&m2DSQdraw2);
    dMap_registerGlobal(&l_globalDestructor11);
    dMap_constructArray(&mTbox, 8, 124, gabi::guest_fn((const void*)dMap_2DT2__ct));
    dMap_registerGlobal(&l_globalDestructor12);
    dMap_constructArray(&mDoor, 16, 124, gabi::guest_fn((const void*)dMap_2DT2__ct));
    dMap_registerGlobal(&l_globalDestructor13);
    dMap_2DTri__ct(&mPointRestart);
    dMap_registerGlobal(&l_globalDestructor14);
    dMap_constructArray(&mPointFriend, 3, 32, gabi::guest_fn((const void*)dMap_2DPoint__ct));
    dMap_registerGlobal(&l_globalDestructor15);
    dMap_drawlistBaseConstruct(&mShip);
    mShip.mVTable = T2_vtable;
    dMap_registerGlobal(&l_globalDestructor16);
}

/* 02590708 */
void dMap_loadFmapResourceForPlatform() {
    dMap_loadResource(gabi::guest_addr("Misc"), gabi::guest_addr("Misc.bfres"),
                      gabi::guest_addr("common.jpc"), gabi::guest_addr("FmapDat.bin"), 0);
}

/* 025907FC */
void dMap_loadDmapResourceForPlatform() {
    dMap_loadResource(gabi::guest_addr("Misc"), gabi::guest_addr("Misc.bfres"),
                      gabi::guest_addr("common.jpc"), gabi::guest_addr("cmapdat.bin"), 0);
}

/* WWHD glue
str 10050B48 "d_map.cpp" in 0258D954
str 10050B54 "pinf->num == 1" in 0258D954
str 10050B8C "s%d.bti" in 0258E1E8
str 10050B94 "m%d.bti" in 0258E1E8
str 10050B9C "m%d.amp" in 0258E1E8
str 10050BA4 "Room%d" in 0258E1E8
str 10050BB0 "d_map.cpp" in 0258E5A8
str 10050BBC "(no >= 0) && (no < (Floor_5F - Floor_B5F + 1))" in 0258E5A8
str 10050C08 "0" in 0258E7A4
str 10050C0C "d_map.cpp" in 0258E7A4
str 10050C18 "i_mapInfoP != (0)" in 0258E7A4
str 10050C2C "d_map.cpp" in 0258EC18
str 10050C38 "roomInfoPP != (0)" in 0258EC18
str 10050C4C "roomInfoP != (0)" in 0258EC18
str 10050C68 "Misc" in 0258EDDC
str 10050C70 "FmapDat.bin" in 0258EDDC
str 10050C7C "Misc.bfres" in 0258EDDC
str 10050C88 "common.jpc" in 0258EDDC
str 10050C94 "Misc" in 0258EEBC
str 10050C9C "cmapdat.bin" in 0258EEBC
str 10050CA8 "Misc.bfres" in 0258EEBC
str 10050CB4 "common.jpc" in 0258EEBC
str 10050CC0 "d_map.h" in 0258F08C
str 10050CC8 "mStageMapInfoP != (0)" in 0258F08C
str 10050CE8 "d_map.h" in 0258F234
str 10050CF0 "mStageMapInfoP != (0)" in 0258F234
str 10050D14 "d_map.h" in 0258F5D8
str 10050D1C "mStageMapInfoP != (0)" in 0258F5D8
str 10050D34 "d_map.cpp" in 0258F978
str 10050D40 "(typeNo >= 0) && (typeNo < AGB_POINT_TYPE_NUM)" in 0258F978
str 10050D74 "d_map.cpp" in 0258FD94
str 10050D80 "i_localX_p != (0)" in 0258FD94
str 10050D94 "i_localY_p != (0)" in 0258FD94
str 10050DA8 "i_gridX_p != (0)" in 0258FD94
str 10050DBC "i_gridY_p != (0)" in 0258FD94
str 10050DD0 "sea" in 02590004
str 10050E08 "d_map.cpp" in 02590208
str 10050E14 "(i_typeAcs >= 0) && (i_typeAcs < ACS_MAP_POINT_TYPE_MAX)" in 02590208
str 10050E50 "0" in 02590488
str 10050E54 "d_map.cpp" in 02590488
str 10050E80 "d_map.cpp" in 0259058C
str 10050E8C "i_gridX_p != (0)" in 0259058C
str 10050EA0 "i_gridY_p != (0)" in 0259058C
str 10050EB4 "Misc" in 02590708
str 10050EBC "FmapDat.bin" in 02590708
str 10050EC8 "Misc.bfres" in 02590708
str 10050ED4 "common.jpc" in 02590708
str 10050EE0 "Misc" in 025907FC
str 10050EE8 "cmapdat.bin" in 025907FC
str 10050EF4 "Misc.bfres" in 025907FC
str 10050F00 "common.jpc" in 025907FC
data l_zero 10050B44
data l_one 10050B7C
data l_gridBias 10050B88
data l_gridSize 10050D70
data l_scrollRange 10050D08
data l_minusOne 10050D0C
data l_floorLabelThreshold 10050BF8
data l_iconFreeScale 10050F10
data l_iconSelfScale 10050F14
data mNowRoomInfoP 101E9FFC
data mStayRoomNo 1047E6C8
data mSaveInfoP 101F84DC
data mResourceControlP 101F4F7C
data l_roomColorInitialized 101FDB64
data mIconFreeScale 10477774
data mIconSelfScale 10477778
data mHDMapState 10477780
data mHDMapStateOld 10477784
data mHDMapDisplayState 10477788
data mHDMapDisplayStateOld 1047778C
data mEnlargementSizeCenterX 1047A860
data mEnlargementSizeCenterZ 1047A864
data mEnlargementSizeScaleX 1047A868
data mEnlargementSizeScaleZ 1047A86C
data mFmapChkPntDataP 1047A870
data mFmapResourceP 1047A874
data mDmapResourceP 1047A878
data mAGBPointValueC 1047A87C
data mAGBPointValueE 1047A880
data mAGBPointValueAll 1047A884
data mAGBPointValueRoomAll 1047A888
data mNowCenterX 1047A894
data mNowCenterZ 1047A898
data mNowScaleX 1047A89C
data mNowScaleZ 1047A8A0
data mCompAlpha 1047A957
data mAlpha 1047A958
data mCollectPointDataCnt 1047A95C
data mAGBMapSendStatus 1047A95E
data mMapDispMode 1047A960
data mAGBMapSendStopFlg 1047A964
data mPlayerStayAgbMapTypeNow 1047A966
data mPlayerStayAgbMapTypeOld 1047A967
data mPlGridX 1047A968
data mPlGridY 1047A969
data mNowFloorNo 1047A96C
data mFmapChkPntValue 1047A96E
data mRoomInfoCtrl 10477790
data mRoomInfo 10477798
data mFrameTex 10478F08
data mFrameTexture 10478FA8
data mIconFreeTexture 104792E8
data mIconSelfTexture 10479350
data mCursor 104793B8
data mAgbCursor 104793E0
data mPoint 10479400
data mTbox 104795E0
data mDoor 104799C0
data mPointRestart 1047A180
data mPointFriend 1047A1A8
data m2DSQdraw 1047A208
data m2DSQdraw2 1047A228
data mCollectPointData 1047A248
data mCollectPointDataLinkList 1047A848
data mIconFreeTex 1047A8A4
data mIconSelfTex 1047A8B8
data mShip 1047A8CC
data l_roomColor 101FEC00
data l_roomColorDefault 10050A28
data l_mapHeader 1047777C
data l_archiveHeader 1047777D
data l_globalDestructor0 101E9F30
data l_globalDestructor1 101E9F3C
data l_globalDestructor2 101E9F48
data l_globalDestructor3 101E9F54
data l_globalDestructor4 101E9F60
data l_globalDestructor5 101E9F6C
data l_globalDestructor6 101E9F78
data l_globalDestructor7 101E9F84
data l_globalDestructor8 101E9F90
data l_globalDestructor9 101E9F9C
data l_globalDestructor10 101E9FA8
data l_globalDestructor11 101E9FB4
data l_globalDestructor12 101E9FC0
data l_globalDestructor13 101E9FCC
data l_globalDestructor14 101E9FD8
data l_globalDestructor15 101E9FE4
data l_globalDestructor16 101E9FF0
data statusMapNo2TypeNoTbl 10050DD4
data agbTbl 10050E60
ext 025200D4 void* dMap_getPlay()
ext 025C11DC void* dMap_getStatusRoomDt(void*, s32)
ext 0273AA24 void dMap_showAssert(const char*, u32, const char*)
ext 0273AD10 void* dMap_operatorNew(u32)
ext 0273AF40 void dMap_operatorDelete(void*)
ext 0252CCBC void dMap_drawlistBaseConstruct(void*)
ext 0252CCFC void dMap_drawlistBaseDestruct(void*, u32)
ext 028F521C void dMap_bzero(void*, u32)
ext 025F1154 u32 dMap_cnvind32(u32)
ext 025F1188 u32 dMap_cnvind16(u32)
ext 028EFFD0 void dMap_constructArray(void*, u32, u32, u32)
ext 028F0164 void dMap_destroyArray(void*, u32, u32, u32, u32, u32)
ext 028F026C void dMap_registerGlobal(void*)
ext 028ED6F8 void dMap_constructHeaderByte(void*)
ext 028EAB2C void dMap_constructArchiveByte(void*)
ext 026124B0 bool dMap_resourceExists(void*, void*, void*, u32)
ext 027E2DC0 void* dMap_getArchive()
ext 027DFA24 void* dMap_getArchiveResource(void*, void*)
ext 028F4114 f32 dMap_floor(f32)
ext 028F186C s32 dMap_sprintf(void*, const char*, u32)
ext 0252447C void* dMap_getStageRes(void*, void*)
ext C000A848 void dMap_copyBytes(void*, void*, u32)
ext 025B85CC void dMap_onSaveArriveGrid(void*, s32)
ext 025B8FAC void dMap_onVisitedRoom(void*, s32)
ext 024EC8D0 void* dMap_getAttentionTarget(void*, u32)
ext 024EDFCC bool dMap_isAttentionLock(void*)
ext 0258E6C8 void dMap_deleteRoomEntry(void*)
ext 0258E7A4 void dMap_changeImageEntry(void*, u32, u32, s32, s32, s32, f32)
ext 0258E1E8 u32 dMap_getRoomImageEntry(void*, s32, u32, s32, void*, void*, void*, void*, void*)
ext 0258F8E0 void dMap_mapAGBSendMapMainEntry(f32, f32)
*/
