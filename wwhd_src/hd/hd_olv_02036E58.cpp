/** HD Miiverse bottle-spot manager. No GameCube counterpart.
 * Verified spawn selection, collision-frame layout, guest indexing, and call ordering are retained.
 */
#include "hd/hd_olv_02036E58.h"
#include "d/d_item.h"
static BottleSpotManager* sInstance;
static BottleSpotDisposer* sDisposer;
static void* sCommentManager;
static void* sResourceManager;
static void* sHeapManager;
static void* sRandom;
static BottleSpotWaterCheck sWaterCheck;
static u32 sWaterGuard;
static BottleSpotDestructorRecord sWaterDestructorRecord;
static BottleSpotDestructorRecords sHeaderDestructorRecords;
static BottleSpotHeader sHeader;
static BottleSpotAngles sAngles;
static BottleSpotDrawSystem sDrawSystem;
static BottleSpotTrig sTrig;
static BottleSpotLineOffsets sLineOffsets;
static BottleSpotTuning sTuning;
static BottleSpotSector sSector;
static BottleSpotRandomScale sRandomScale;
static BottleSpotProbe sProbe;
static inline f32 i2f(s32 v) { return (f32)(f64)v; }
static inline f32 u2f(u32 v) { return (f32)(f64)v; }
/* 02036E58 */
static BottleSpotManager* constructBottleSpotManager(BottleSpotManager* self) {
    BottleSpotManager* manager = self;
    if (manager == 0) {
        manager = bottleSpotAllocate(sizeof(BottleSpotManager));
        if (manager == 0)
            return 0;
    }
    manager->__vtbl = 0x10005418;
    bottleSpotConstructRooms(manager->mRooms, ROOM_COUNT, sizeof(BottleRoomSpots),
                             ROOM_SPOT_CONSTRUCTOR);
    f32 zero = sTuning.mZero;
    manager->mDebugDraw = 0;
    manager->mSideMax = zero;
    manager->mDebugSpawnCount = 0;
    manager->mSideMin = zero;
    manager->mSpawnCount = 0;
    manager->mSectorMargin = zero;
    manager->mSpawnFailed = 0;
    manager->mEnabled = 0;
    manager->mSpawnAll = 0;
    return manager;
}

/* 02036EEC */
BottleSpotManager* bottleSpotCreateInstance(void* heap) {
    BottleSpotManager* instance = sInstance;
    if (instance != 0)
        return instance;
    BottleSpotManager* manager = bottleSpotAllocateInHeap(sizeof(BottleSpotManager), heap, 4u);
    BottleSpotDisposer* disposer =
        gabi::at<BottleSpotDisposer>(gabi::ea(manager) + offsetof(BottleSpotManager, mDisposer));
    if (disposer != 0) {
        bottleSpotConstructDisposer(disposer, heap, 3u);
        disposer->__vtbl = 0x100054D8;
    }
    sDisposer = disposer;
    BottleSpotManager* result = manager;
    if (manager != 0)
        result = constructBottleSpotManager(manager);
    sInstance = result;
    return result;
}

/* 02036F8C */
void BottleSpotManager::setParams() {
    f32 firstForwardMin = sTuning.mForwardMin, secondForwardDistance = sTuning.mForwardSecond,
        sectorHalfSize = sTuning.mSectorHalf, defaultMin = sTuning.mDefaultMin,
        defaultMax = sTuning.mDefaultMax;
    mForwardMin[0] = firstForwardMin;
    mForwardMin[1] = secondForwardDistance;
    mForwardMax[1] = sectorHalfSize;
    mSideMin = defaultMin;
    mSectorMargin = defaultMin;
    mForwardMax[0] = secondForwardDistance;
    mSideMax = defaultMax;
}

/* 02036FD4 */
void BottleSpotManager::load() {
    gabi::Local<BottleSpotString> archiveName, resourceName;
    archiveName->mpData = gabi::at<const char>(gabi::guest_addr("Misc"));
    resourceName->__vtbl = 0x10005298;
    archiveName->__vtbl = 0x10005298;
    resourceName->mpData = gabi::at<const char>(gabi::guest_addr("Misc.bfres"));
    void* resource =
        bottleSpotGetResource(sResourceManager.get(), archiveName.get(), resourceName.get(), 0);
    if (!resource)
        return;
    BottleSpotArchive* archive = bottleSpotGetArchive(resource);
    gabi::Local<BottleSpotFixedString> fileName;
    for (u32 i = 1; i < ROOM_COUNT; i++) {
        fileName->mBuffer[0] = 0;
        fileName->mBuffer[0x3F] = 0;
        fileName->mCapacity = 0x40;
        fileName->mpData = reinterpret_cast<const char*>(fileName->mBuffer);
        fileName->__vtbl = 0x10005300;
        gabi::cpu->cr[6] = 0;
        bottleSpotFormat(fileName.get(), "MiiverseBottleSpotRoom%02d.dat", i);
        fileName->assureTermination();
        // HD: The resource callback may return guest zero; the relative-offset field still lives at
        // +0x4C.
        auto* fileTableOffset =
            gabi::at<be<u32>>(gabi::ea(archive) + offsetof(BottleSpotArchive, mFileTableOffset));
        u32 tableOffset = *fileTableOffset;
        void* fileTable =
            tableOffset ? gabi::at<void>(gabi::ea(fileTableOffset) + tableOffset) : nullptr;
        BottleSpotFile* file = bottleSpotFindFile(fileTable, fileName->mpData.get());
        if (file == 0)
            continue;
        u32 dataOffset = file->mDataOffset;
        BottleSpotFileData* data =
            dataOffset ? gabi::at<BottleSpotFileData>(gabi::ea(file) + dataOffset) : nullptr;
        if (data == 0)
            continue;
        s32 count = data->mCount;
        if (count > ROOM_SPOT_CAPACITY)
            continue;
        BottleRoomSpots* room = this->room(i);
        room->mCount = (u8)count;
        for (s32 k = 0; k < count; k++) {
            cXyz* src = &data->mPositions[k];
            cXyz* dst = &room->mPositions[k];
            dst->x = src->x;
            dst->y = src->y;
            dst->z = src->z;
        }
    }
}

/* 0203714C */
void BottleSpotManager::spawnAt(u32 pos, u32 spot) {
    BottleSpotAppend* append = bottleSpotCreateAppend(); /* fopAcM_CreateAppend */
    auto* sourcePosition = reinterpret_cast<BottleSpotVectorWords*>(position(spot, pos));
    auto* appendPos = reinterpret_cast<BottleSpotVectorWords*>(&append->mPosition);
    append->mParameters = BOTTLE_LISTED_PARAMETERS;
    appendPos->x = (u32)sourcePosition->x;
    appendPos->y = (u32)sourcePosition->y;
    u32 positionZ = (u32)sourcePosition->z;
    append->mAngle.z = 0xFF;
    appendPos->z = positionZ;
    append->mRoomNo = (u8)spot;
    append->mArgument = 0xFF;
    append->mGbaName = 0;
    u32 layer = bottleSpotGetLayer();
    bottleSpotCreateActor(layer, 0xFFu, 0u, 0u, append);
    bottleSpotConsumeComment(sCommentManager.get());
    u32 spawnIndex = (u8)mSpawnCount;
    auto* spawnPosition = reinterpret_cast<BottleSpotVectorWords*>(&mSpawnPositions[spawnIndex]);
    spawnPosition->x = (u32)sourcePosition->x;
    spawnPosition->y = (u32)sourcePosition->y;
    spawnPosition->z = (u32)sourcePosition->z;
    mSpawnCount = (u8)mSpawnCount + 1;
}

/* 02037230 */
void BottleSpotManager::spawnListed(u32 spot) {
    if (spot == 0 || (s32)spot >= ROOM_COUNT)
        return;
    if (bottleSpotCheckItem(dItemNo_TINGLE_TUNER_e) == 0)
        return; /* dComIfGs_checkGetItem(Tingle Bottle) */
    s32 ready = (s32)bottleSpotReadyComments(sCommentManager.get());
    u32 spawnAll = (u8)mSpawnAll;
    be<u8>* roomCount = &room(spot)->mCount;
    u32 count;
    if (spawnAll == 0) {
        if (ready == 0)
            return;
        count = (u8)*roomCount;
        if (count == 0)
            return;
    } else {
        count = (u8)*roomCount;
    }
    s32 spawnCount = (s32)(count >> 2);
    if (spawnCount > ready)
        spawnCount = ready;
    if (spawnAll != 0)
        spawnCount = (s32)count;
    /* shuffle bag of the list indices */
    be<u32>* indices = nullptr;
    s32 capacity = 0, remaining = 0;
    u32 limit = count;
    if ((s32)count > 0) {
        BottleSpotHeap* heap = bottleSpotGetCurrentHeap(sHeapManager.get());
        be<u32>* allocation = heap->allocate(count * 4, 4);
        limit = (u8)*roomCount;
        if (allocation != 0) {
            capacity = (s32)count;
            indices = allocation;
        }
    }
    for (s32 i = 0; i < (s32)limit;) {
        if (remaining < capacity) {
            s32 index = remaining;
            remaining++;
            if (index >= capacity)
                index -= capacity;
            indices[(u32)index] = (u32)i;
        }
        limit = (u8)*roomCount;
        i++;
    }
    for (s32 w = spawnCount; w > 0; w--) {
        u32 random = bottleSpotRandom(sRandom.get());
        s32 selection = (s32)(u32)(((u64)random * (u32)remaining) >> 32);
        if ((u32)selection < (u32)remaining) {
            s32 index = selection;
            if (index >= capacity)
                index -= capacity;
            spawnAt((u32)indices[(u32)index], spot);
        } else {
            spawnAt((u32)indices[0], spot);
        }
        if (remaining <= 0)
            continue;
        if (selection < 0 || selection >= remaining)
            continue;
        remaining--;
        if (selection >= remaining)
            continue;
        do {
            s32 a = selection;
            if (a >= capacity)
                a -= capacity;
            s32 b = selection + 1;
            if (b >= capacity)
                b -= capacity;
            selection++;
            indices[(u32)a] = (u32)indices[(u32)b];
        } while (selection < remaining);
    }
    if (indices != 0) {
        BottleSpotHeap* heap = bottleSpotFindHeap(sHeapManager.get(), indices);
        heap->free(indices);
    }
}

/* 02037480 */
void BottleSpotManager::randomPos(fopAc_ac_c* pl, u32 spot, cXyz* out) {
    u32 sel = (u8)mSpawnCount != 0 ? 1 : 0;
    f32 px = (f32)pl->current.pos.x;
    f32 sectorIndexBias = sSector.mIndexBias;
    f32 pz = (f32)pl->current.pos.z;
    f32 sectorSize = sSector.mSize;
    s32 ix = gabi::ftoi((px + sectorIndexBias) / sectorSize);
    f32 sideMax = (f32)mSideMax;
    s32 iz = gabi::ftoi((pz + sectorIndexBias) / sectorSize);
    f32 sideMin = (f32)mSideMin;
    f32 sectorOriginBias = sSector.mOriginBias;
    f32 forwardMin = (f32)mForwardMin[sel];
    f32 py = (f32)pl->current.pos.y;
    f32 cx = gabi::fmadds(i2f(ix), sectorSize, sectorOriginBias);
    f32 forwardMax = (f32)mForwardMax[sel];
    f32 half = sTuning.mSectorHalf;
    f32 forwardRange = forwardMax - forwardMin;
    f32 zero = sTuning.mZero;
    f32 sideRange = sideMax - sideMin;
    f32 randomHalf = sRandomScale.mHalf;
    f32 randomInverse32 = sRandomScale.mInverse32;
    f32 cz = gabi::fmadds(i2f(iz), sectorSize, sectorOriginBias);
    gabi::Local<cXyz> vec;
    gabi::Local<cXyz> pos;
    gabi::Local<cXyz> d2;
    gabi::Local<cXyz> dif;
    bool found = false;
    for (u32 t = SEA_POSITION_TRIES; t != 0; t--) {
        f32 r1 = u2f(bottleSpotRandom(sRandom.get())) * randomInverse32;
        f32 rad = gabi::fmadds(r1, forwardRange, forwardMin);
        f32 r2 = u2f(bottleSpotRandom(sRandom.get())) * randomInverse32;
        f32 side = r2 * sideRange;
        f32 r3 = u2f(bottleSpotRandom(sRandom.get())) * randomInverse32;
        u32 ang = (u16)(s16)pl->shape_angle.y;
        if (r3 > randomHalf)
            side = -side;
        pos->x = px;
        pos->y = py;
        u32 o1 = ang & ~7u;
        pos->z = pz;
        vec->x = rad * (f32)sTrig.mEntries[o1 / 8].mSin;
        vec->y = zero;
        vec->z = rad * (f32)sTrig.mEntries[o1 / 8].mCos;
        bottleSpotAdd(pos.get(), vec.get(), pos.get());
        u32 o2 = ((ang + 0x4000) & 0xFFFF) & ~7u;
        vec->x = side * (f32)sTrig.mEntries[o2 / 8].mSin;
        vec->y = zero;
        vec->z = side * (f32)sTrig.mEntries[o2 / 8].mCos;
        bottleSpotAdd(pos.get(), vec.get(), pos.get());
        f32 m = half - (f32)mSectorMargin;
        f32 x = (f32)pos->x;
        if (x < cx - m)
            continue;
        if (cx + m < x)
            continue;
        f32 z = (f32)pos->z;
        if (z < cz - m)
            continue;
        if (!(cz + m < z)) { /* bge: also taken for NaN */
            found = true;
            break;
        }
    }
    if (!found)
        mSpawnFailed = 1;
    BottleRoomSpots* room = this->room(spot);
    cXyz* v = room->mPositions;
    for (u32 i = 0; i < (u8)room->mCount; i++, ++v) {
        bottleSpotSubtract(v, dif.get(), pos.get());
        d2->x = (f32)dif->x;
        d2->y = zero;
        d2->z = (f32)dif->z;
        bottleSpotSquaredMagnitude(d2.get());
    }
    f32 ox = (f32)pos->x;
    f32 oz = (f32)pos->z;
    out->x = ox;
    out->z = oz;
}

/* 020378B4 */
u32 BottleSpotManager::bgCheck(cXyz* pos, be<f32>* out) {
    gabi::Local<BottleSpotCheckFrame> frame;
    auto* ground = &frame->mGround;
    auto* groundCheck = static_cast<cBgS_Chk*>(ground);
    auto* groundPoly = static_cast<cBgS_PolyInfo*>(ground);
    auto* groundPass = static_cast<dBgS_PolyPassChk*>(ground);
    auto* groundGroup = static_cast<dBgS_GrpPassChk*>(ground);
    auto* line = &frame->mLine;
    auto* lineCheck = static_cast<cBgS_Chk*>(line);
    auto* linePoly = static_cast<cBgS_PolyInfo*>(line);
    auto* linePass = static_cast<dBgS_PolyPassChk*>(line);
    auto* lineGroup = static_cast<dBgS_GrpPassChk*>(line);
    bottleSpotConstructGround(ground); /* cBgS_GndChk::cBgS_GndChk */
    f32 h = sProbe.mGroundHeight;
    f32 x = (f32)pos->x, z = (f32)pos->z;
    /* dBgS_GndChk parts (vtables, pass checks) */
    ground->m_pos.x = x;
    groundPass->mbArrowThrough = 0;
    groundPass->mbCamThrough = 0;
    groundCheck->mPolyPassChk = gabi::ea(groundPass);
    groundGroup->__vtbl = 0x10005358;
    groundPoly->__vtbl = 0x10005348;
    ground->m_pos.z = z;
    groundPass->mbLinkThrough = 0;
    groundPass->mbRopeThrough = 0;
    groundCheck->__vtbl = 0x10005338;
    groundPass->mbObjThrough = 0;
    groundGroup->mGrp = 1;
    groundPass->mbBombThrough = 0;
    groundCheck->mGrpPassChk = gabi::ea(groundGroup);
    ground->m_pos.y = h;
    groundPass->__vtbl = 0x10005368;
    groundPass->mbBoomerangThrough = 0;
    f32 g = bottleSpotGroundCross(&bottleSpotGetGameInfo()->mBgS, ground); /* cBgS::GroundCross */
    f32 zero = sTuning.mZero;
    if (g > zero) {
        groundPoly->__vtbl = 0x10005348;
        groundPass->__vtbl = 0x10005368;
        groundGroup->__vtbl = 0x10005328;
        bottleSpotDestroyGround(ground, 0u); /* ~cBgS_Chk */
        return 0;
    }
    if (bottleSpotCheckSeaArea((f32)pos->x, (f32)pos->z) != 0) { /* daSea_ChkArea */
        f32 wz = (f32)pos->z;
        f32 wx = (f32)pos->x;
        *out = bottleSpotSeaHeight(wx, wz); /* daSea_calcWave */
    } else {
        BottleSpotWaterCheck* W = &sWaterCheck;
        if ((u32)sWaterGuard == 0) {
            sWaterGuard = 1;
            bottleSpotConstructWater(W);
            bottleSpotRegisterGlobal(&sWaterDestructorRecord);
        }
        f32 vz = (f32)pos->z;
        f32 lo = sProbe.mWaterMin;
        f32 vx = (f32)pos->x;
        W->m_ground.y = lo;
        f32 hi = sProbe.mWaterMax;
        W->m_ground.z = vz;
        W->m_roof = hi;
        W->m_ground.x = vx;
        if (bottleSpotWaterCross(&bottleSpotGetGameInfo()->mBgS, W) != 0) /* dBgS::SplGrpChk */
            *out = (f32)W->m_height;
        else
            *out = zero;
    }
    bottleSpotConstructLine(line); /* cBgS_LinChk::ct */
    reinterpret_cast<BottleSpotVectorWords*>(&frame->mStart)->z =
        reinterpret_cast<const BottleSpotVectorWords*>(pos)->z;
    reinterpret_cast<BottleSpotVectorWords*>(&frame->mStart)->y =
        reinterpret_cast<const BottleSpotVectorWords*>(pos)->y;
    reinterpret_cast<BottleSpotVectorWords*>(&frame->mStart)->x =
        reinterpret_cast<const BottleSpotVectorWords*>(pos)->x;
    linePass->mbObjThrough = 0;
    linePass->mbCamThrough = 0;
    linePass->mbLinkThrough = 0;
    linePass->mbArrowThrough = 0;
    linePass->mbBombThrough = 0;
    linePass->mbBoomerangThrough = 0;
    linePass->mbRopeThrough = 0;
    lineCheck->mPolyPassChk = gabi::ea(linePass);
    lineCheck->__vtbl = 0x100053D8;
    linePoly->__vtbl = 0x100053E8;
    lineGroup->mGrp = 1;
    lineCheck->mGrpPassChk = gabi::ea(lineGroup);
    linePass->__vtbl = 0x10005408;
    lineGroup->__vtbl = 0x100053F8;
    u32 ok = 1;
    for (u32 k = 0; k < 4; k++) {
        f32 dz = (f32)sLineOffsets.mZ[k];
        f32 dx = (f32)sLineOffsets.mX[k];
        frame->mEnd.y = zero;
        frame->mEnd.z = dz;
        frame->mEnd.x = dx;
        bottleSpotAdd(&frame->mEnd, pos, &frame->mEnd);
        bottleSpotSetLine(line, &frame->mEnd, &frame->mStart, 0u);            /* dBgS_LinChk::Set */
        if (bottleSpotLineCross(&bottleSpotGetGameInfo()->mBgS, line) != 0) { /* cBgS::LineCross */
            ok = 0;
            break;
        }
    }
    linePass->__vtbl = 0x10005408;
    lineGroup->__vtbl = 0x10005328;
    linePoly->__vtbl = 0x10005318;
    bottleSpotDestroyLine(line, 0u);
    groundPass->__vtbl = 0x10005368;
    groundGroup->__vtbl = 0x10005328;
    groundPoly->__vtbl = 0x10005348;
    bottleSpotDestroyGround(ground, 0u);
    return ok;
}

/* 02037D64 */
u32 BottleSpotManager::trySpawnSea(u32 spot) {
    if (spot == 0 || (s32)spot >= ROOM_COUNT)
        return 0;
    if (bottleSpotCheckItem(dItemNo_TINGLE_TUNER_e) == 0)
        return 0;
    if ((s32)bottleSpotReadyComments(sCommentManager.get()) <= 0)
        return 0;
    fopAc_ac_c* pl = bottleSpotGetGameInfo()->mpPlayer.get();
    if (pl == 0)
        return 0;
    f32 zero = sTuning.mZero;
    u32 tries = 0;
    mSpawnFailed = 0;
    gabi::Local<be<f32>> hgt;
    gabi::Local<cXyz> pos;
    *hgt = zero;
    for (;;) {
        randomPos(pl, spot, pos.get());
        u32 r = bgCheck(pos.get(), hgt.get());
        if (r == 0) {
            tries = (tries + 1) & 0xFF;
            if (tries >= SEA_COLLISION_TRIES) {
                mSpawnFailed = 1;
                return 0;
            }
        }
        if ((u8)mSpawnFailed != 0)
            return 0;
        if (r != 0)
            break;
    }
    f32 x = (f32)pos->x, z = (f32)pos->z, y = (f32)*hgt;
    BottleSpotAppend* ap = bottleSpotCreateAppend(); /* fopAcM_CreateAppend */
    ap->mPosition.x = x;
    ap->mGbaName = 0;
    ap->mParameters = BOTTLE_SEA_PARAMETERS;
    ap->mPosition.z = z;
    ap->mArgument = 0xFF;
    ap->mAngle.z = 0xFF;
    ap->mPosition.y = y;
    ap->mRoomNo = (u8)spot;
    u32 layer = bottleSpotGetLayer();
    bottleSpotCreateActor(layer, 0xFFu, 0u, 0u, ap);
    bottleSpotConsumeComment(sCommentManager.get());
    cXyz* e = &mSpawnPositions[(u8)mSpawnCount];
    e->x = x;
    e->y = y;
    e->z = z;
    u32 n = (u8)mSpawnCount, d = (u8)mDebugSpawnCount;
    mSpawnCount = n + 1;
    mDebugSpawnCount = d + 1;
    return 1;
}

/* 02037F80 */
u32 BottleSpotManager::inSector(cXyz* pos) {
    fopAc_ac_c* pl = bottleSpotGetGameInfo()->mpPlayer.get();
    if (pl == 0)
        return 0;
    f32 pz = (f32)pl->current.pos.z, px = (f32)pl->current.pos.x;
    f32 sectorIndexBias = sSector.mIndexBias, sectorSize = sSector.mSize;
    s32 ix = gabi::ftoi((px + sectorIndexBias) / sectorSize);
    s32 iz = gabi::ftoi((pz + sectorIndexBias) / sectorSize);
    f32 half = sTuning.mSectorHalf;
    f32 margin = (f32)mSectorMargin;
    f32 sectorOriginBias = sSector.mOriginBias;
    f32 m = half - margin;
    f32 cx = gabi::fmadds(i2f(ix), sectorSize, sectorOriginBias);
    f32 x = (f32)pos->x;
    f32 cz = gabi::fmadds(i2f(iz), sectorSize, sectorOriginBias);
    if (x < cx - m)
        return 0;
    if (cx + m < x)
        return 0;
    f32 z = (f32)pos->z;
    if (z < cz - m)
        return 0;
    return !(cz + m < z) ? 1 : 0; /* bge: also taken for NaN */
}

/* 020380B8 */
static void bottleSpotCopyMtx(Mtx34* dst, const Mtx34* src) {
    f32 v[12];
    for (u32 i = 0; i < 12; i++)
        v[i] = (f32)src->m[i / 4][i % 4];
    for (u32 i = 0; i < 12; i++)
        dst->m[i / 4][i % 4] = v[i];
}

/* 02038158 */
void BottleSpotManager::debugDraw() {
    if ((u8)mDebugDraw == 0)
        return;
    gabi::Local<BottleSpotString> stageName, currentStage;
    stageName->mpData = gabi::at<const char>(gabi::guest_addr("sea"));
    stageName->__vtbl = 0x10005298;
    BottleSpotGameInfo* game = bottleSpotGetGameInfo();
    BottleSpotStringVtable* vt = gabi::at<BottleSpotStringVtable>(stageName->__vtbl);
    currentStage->__vtbl = 0x10005298;
    currentStage->mpData = game->mStageName;
    gabi::call_ptr<u32>(vt->mAssureTermination.get(), stageName.get());
    stageName->assureTermination();
    BottleSpotStringVtable* vt2 = gabi::at<BottleSpotStringVtable>(currentStage->__vtbl);
    const be<u8>* s1 = reinterpret_cast<const be<u8>*>(stageName->mpData.get());
    gabi::call_ptr<u32>(vt2->mAssureTermination.get(), currentStage.get());
    const be<u8>* s2 = reinterpret_cast<const be<u8>*>(currentStage->mpData.get());
    if (s1 != s2) {
        u32 i = 0;
        for (; i < 0x40001; i++) {
            u32 a = (u8)s1[i], b = (u8)s2[i];
            if (a != b)
                return;
            if (a == 0)
                break;
        }
        if (i == 0x40001)
            return;
    }
    fopAc_ac_c* pl = bottleSpotGetGameInfo()->mpPlayer.get();
    if (pl == 0)
        return;
    BottleSpotDrawSystem* D = &sDrawSystem;
    gabi::Local<Mtx34> mtx;
    gabi::Local<be<u32>[16]> prj;
    gabi::Local<cXyz> dif;
    gabi::Local<cXyz> d2;
    bottleSpotCopyMtx(mtx.get(), &D->mView);
    for (u32 i = 0; i < 16; i++)
        (*prj)[i] = (u32)D->mProjection[i];
    BottleSpotDrawState* r = bottleSpotGetDrawState(D);
    u32 prev = bottleSpotSetDrawState(mtx.get(), prj.get(), (u32)r->mCurrent);
    for (u32 i = 0; i < (u8)mSpawnCount; i++) {
        f32 zero = sTuning.mZero;
        bottleSpotSubtract(&pl->current.pos, dif.get(), &mSpawnPositions[i]);
        d2->x = (f32)dif->x;
        d2->y = zero;
        d2->z = (f32)dif->z;
        f32 m = bottleSpotSquaredMagnitude(d2.get());
        bottleSpotSqrt(m);
    }
    r = bottleSpotGetDrawState(D);
    r->mCurrent = prev;
    r->mDirty = 0;
}

/* 0203835C */
void bottleSpotDisposerDtor(BottleSpotDisposer* p, u32 flags) {
    if (p == 0)
        return;
    p->__vtbl = 0x100054D8;
    if (p == sDisposer.get()) {
        BottleSpotManager* inst = sInstance;
        sDisposer = nullptr;
        inst->destroy(2);
        sInstance = nullptr;
    }
    bottleSpotDestroyDisposer(p, 0u);
    if (flags & 1)
        bottleSpotDelete(p);
}

/* 02038404 */
static void __sinit_bottle_spot_manager() {
    sHeader.mWords[2] = 0;
    sHeader.mWords[0] = 0;
    sHeader.mWords[3] = 0;
    sHeader.mWords[1] = 0;
    bottleSpotRegisterGlobal(&sHeaderDestructorRecords.mRecords[0]);
    f32 minAngle = sAngles.mMin, maxAngle = sAngles.mMax;
    sHeader.mMinAngle = minAngle;
    sHeader.mMaxAngle = maxAngle;
    bottleSpotInitHeaderByte(&sHeader.mByte);
    bottleSpotRegisterGlobal(&sHeaderDestructorRecords.mRecords[1]);
    bottleSpotInitHeaderFlag(&sHeader.mFlag);
    bottleSpotRegisterGlobal(&sHeaderDestructorRecords.mRecords[2]);
}

// clang-format off
/* WWHD glue
data sInstance 1018F450
data sDisposer 1018F454
data sCommentManager 1018F4AC
data sResourceManager 101F4F7C
data sHeapManager 101F8B4C
data sRandom 101F8B60
data sWaterCheck 102009FC
data sWaterGuard 10200A4C
data sWaterDestructorRecord 1018F420
data sHeaderDestructorRecords 1018F42C
data sHeader 102009E0
data sAngles 100054D0
data sDrawSystem 104B45C0
data sTrig 104A44F8
data sLineOffsets 10005430
data sTuning 10005450
data sSector 1000549C
data sRandomScale 100054B0
data sProbe 100054C0
ext 0273AD10 BottleSpotManager* bottleSpotAllocate(u32)
ext 028EFFD0 void bottleSpotConstructRooms(BottleRoomSpots*, u32, u32, u32)
ext 0273B0D4 BottleSpotManager* bottleSpotAllocateInHeap(u32, void*, u32)
ext 02752B0C void bottleSpotConstructDisposer(BottleSpotDisposer*, void*, u32)
ext 026124B0 void* bottleSpotGetResource(void*, BottleSpotString*, BottleSpotString*, u32)
ext 027E2DC0 BottleSpotArchive* bottleSpotGetArchive(void*)
ext 02759C28 void bottleSpotFormat(BottleSpotFixedString*, const char*, u32)
ext 027DFA24 BottleSpotFile* bottleSpotFindFile(void*, const char*)
ext 025D5600 BottleSpotAppend* bottleSpotCreateAppend()
ext 025DED64 u32 bottleSpotGetLayer()
ext 025E14A8 void bottleSpotCreateActor(u32, u32, u32, u32, BottleSpotAppend*)
ext 02039D5C void bottleSpotConsumeComment(void*)
ext 02520C0C u32 bottleSpotCheckItem(u32)
ext 02039A58 u32 bottleSpotReadyComments(void*)
ext 02756140 BottleSpotHeap* bottleSpotGetCurrentHeap(void*)
ext 0275CEE4 u32 bottleSpotRandom(void*)
ext 02755FEC BottleSpotHeap* bottleSpotFindHeap(void*, void*)
ext 028E8D88 void bottleSpotAdd(cXyz*, cXyz*, cXyz*)
ext 0201ADE0 void bottleSpotSubtract(cXyz*, cXyz*, cXyz*)
ext 028E8DD0 f32 bottleSpotSquaredMagnitude(cXyz*)
ext 02008E0C void bottleSpotConstructGround(dBgS_GndChk*)
ext 02008974 f32 bottleSpotGroundCross(void*, dBgS_GndChk*)
ext 02008DAC void bottleSpotDestroyGround(dBgS_GndChk*, u32)
ext 0246B6A4 u32 bottleSpotCheckSeaArea(f32, f32)
ext 0246BA0C f32 bottleSpotSeaHeight(f32, f32)
ext 024F22DC void bottleSpotConstructWater(BottleSpotWaterCheck*)
ext 028F026C void bottleSpotRegisterGlobal(void*)
ext 024EF7C0 u32 bottleSpotWaterCross(void*, BottleSpotWaterCheck*)
ext 02008FEC void bottleSpotConstructLine(dBgS_LinChk*)
ext 024F1AFC void bottleSpotSetLine(dBgS_LinChk*, cXyz*, cXyz*, u32)
ext 02008860 u32 bottleSpotLineCross(void*, dBgS_LinChk*)
ext 02008B4C void bottleSpotDestroyLine(dBgS_LinChk*, u32)
ext 027F29D4 BottleSpotDrawState* bottleSpotGetDrawState(BottleSpotDrawSystem*)
ext 027A9D24 u32 bottleSpotSetDrawState(Mtx34*, void*, u32)
ext 028F4384 f32 bottleSpotSqrt(f32)
ext 02752BEC void bottleSpotDestroyDisposer(BottleSpotDisposer*, u32)
ext 0273AF40 void bottleSpotDelete(void*)
ext 028ED6F8 void bottleSpotInitHeaderByte(void*)
ext 028EAB2C void bottleSpotInitHeaderFlag(void*)
ext 025200D4 BottleSpotGameInfo* bottleSpotGetGameInfo()
str 10005468
str 10005470
str 1000547C
str 100054CC
*/
