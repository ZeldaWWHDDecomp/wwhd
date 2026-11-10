/** JAIZelBasic initialization, wave-bank loading, scene and camera transitions.
 * HD constructor/layout and reload order retained. */
#include "JAZelAudio/JAIZelBasic_7.h"
using namespace jaizel7;
namespace {
enum IslandRoom {
    dIsleRoom_WindfallIsland_e = 11,
    dIsleRoom_DragonRoostIsland_e = 13,
    dIsleRoom_FlightControlPlatform_e = 14,
    dIsleRoom_OutsetIsland_e = 44
};
constexpr u32 JA_BGM_BATTLE_NORM = 0x80000004;
constexpr u32 JA_BGM_ISLAND_LINK_2 = 0x8000000E;
constexpr u32 JA_BGM_DK_BATTLE = 0x80000014;
constexpr u32 JA_BGM_MJ_TOWER_BATTLE = 0x80000015;
constexpr u32 JA_BGM_FIND_TETRA = 0x80000016;
constexpr u32 JA_BGM_MBOSS = 0x80000019;
constexpr u32 JA_BGM_MBOSS_S = 0x8000001A;
constexpr u32 JA_BGM_SEA_ENEMY = 0x8000001C;
constexpr u32 JA_BGM_MORNING = 0x8000001D;
constexpr u32 JA_BGM_TAKT_CHUYA = 0x80000030;
constexpr u32 JA_BGM_TAKT_SHIPPU = 0x80000032;
constexpr u32 JA_BGM_HYRULE_OFF = 0x80000034;
constexpr u32 JA_BGM_ISLAND_LINK_0 = 0x80000038;
constexpr u32 JA_BGM_ISLAND_WOOD = 0x80000039;
constexpr u32 JA_BGM_PIRATE_DEMO5 = 0x8000003D;
constexpr u32 JA_BGM_D_GANON_1 = 0x80000042;
constexpr u32 JA_BGM_BAACHAN = 0x80000044;
constexpr u32 JA_BGM_ISLAND_LINK_3 = 0x80000055;
constexpr u32 JA_BGM_UNK_111 = 0x80000111;
constexpr u32 JA_BGM_UNK_112 = 0x80000112;
constexpr u32 JA_BGM_UNK_115 = 0x80000115;
constexpr u32 JA_BGM_UNK_130 = 0x80000130;
constexpr u32 JA_BGM_UNK_133 = 0x80000133;
constexpr u32 JA_STRM_BTD_START = 0xC0000002;
constexpr u32 JA_STRM_BKM_START = 0xC0000007;
constexpr u32 JA_STRM_BST_START = 0xC0000009;
constexpr u32 JA_STRM_RANE_START = 0xC000000F;
constexpr u32 JA_STRM_BPW_START = 0xC0000034;
} // namespace

namespace {
// HD: autoglue lacks host registration for external callbacks, so keep this named guest target.
constexpr u32 ALLOCATION_RECORD_CTOR = 0x02029C44;
static GuestWord DRAM_HEAP_SIZE;
static GuestWord SE_REGIST_MAX;
static GuestWord SOUND_SCENE_MAX;
static GuestWord STREAM_DECODED_BUFFERS;
static GuestWord SEQ_CONTROL_BUF_MAX;
static GuestWord STREAM_CONTROL_BUF_MAX;
static GuestWord AUTO_HEAP_MAX;
static GuestWord STAY_HEAP_MAX;
static GuestWord AUTO_HEAP_ROOM_SIZE;
static GuestWord STAY_HEAP_ROOM_SIZE;
static GuestWord SYSTEM_TRACK_MAX;
static GuestWord PLAY_TRACK_MAX;
static GuestFloat DISTANCE_MAX;
static GuestFloat MAX_VOLUME_DISTANCE;
static GuestFloat MIN_DISTANCE_VOLUME;
static GuestFloat DOLBY_FLONT_DISTANCE_MAX;
static GuestFloat DOLBY_BEHIND_DISTANCE_MAX;
static GuestWord DUMMY_POSITION_MAX;
static GuestWord DUMMY_POSITION_LIFE_TIME;
static StringPtr STREAM_PATH;
static StringPtr SEQ_ARCH_FILE_NAME;
static GuestWord BGM_SCENE_CHANGE_FO_TIME;
static GuestFloat ENEMY_NEARBY_DIST;
static GuestWord ENEMY_NEARBY_FADEIN_TIME;
static GuestWord ENEMY_NEARBY_FADEOUT_TIME;
static GuestHalf DISTANCE_FX_PARAM;
static GuestByte DOLBY_CENTER_VALUE;
static GuestByte COMMON_BATTLE_FO_DELAY;
static GuestByte MBOSS_ENEMY_NEARBY_TRACK;
static GuestFloat l_inputGain;
static GuestFloat l_battleTempoRise;
static GuestFloat l_minibossTempoRise;
static GuestFloat l_seaBattleTempoRise;
static SoundPtr seHandle;
static ManagerPtr l_soundManager;
static ManagerPtr l_save;
static BasicPtr zel_basic;
static BasicVtable l_basicVtable;
static WaveTable m_dy_wave_set_1st;
static WaveTable m_dy_wave_set_2nd;
static SceneInfoTable m_scene_info;
static SceneInfoTable m_isle_info;
static CategoryVolumes l_defaultCategoryVolumes;
static StaticWaves l_staticWaves;
static SpotNames spot_dir_name;
static GuestByte m_bgm_wave_info;

scene_info_s *sceneEntry(u32 index) {
    return gabi::at<scene_info_s>(gabi::ea(&m_scene_info) + index * u32(sizeof(scene_info_s)));
}
Random *wrappedRandom(JAIZelBasic *basic) {
    return gabi::at<Random>(gabi::ea(basic) + u32(offsetof(JAIZelBasic, mRandomHD)));
}
s32 isStageItem(u32 offset, u32 flag) {
    return isDungeonItem(gabi::at<void>(gabi::ea(l_save.get()) + offset), flag);
}
void loadWave(u32 bank) {
    loadWaveBank(bank, 0);
}
void unloadWave(u32 bank) {
    eraseWaveBank(bank, 0);
    eraseWaveHeap(bank, 0);
    resetWaveBank(bank, u32(-1));
}
// HD: AudioSafeString path getters may return guest-null; preserve all virtual/string reloads.
void assurePath(AudioSafeString *path) {
    u32 table = gabi::load<u32>(gabi::ea(path) + offsetof(AudioSafeString, __vtbl));
    auto function = gabi::load<u32>(table + offsetof(AudioSafeStringVtable, assureTermination));
    gabi::call_ptr(function, path);
}
GuestByte *pathString(AudioSafeString *path) {
    return gabi::at<GuestByte>(
        gabi::load<u32>(gabi::ea(path) + offsetof(AudioSafeString, mTextPtr)));
}
} // namespace

/* 0202573C */
void JAIZelBasic::monsSeInit() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    for (u32 k = 0; k < 30; k++) {
        mMonsSe[k].mActorId = -1;
        mMonsSe[k].mpSound = nullptr;
    }
}

/* 02025760 */
void JAIZelBasic::initSe() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    cameraAt(0)->position = nullptr;
    cameraAt(0)->previousPosition = nullptr;
    cameraAt(0)->matrix = nullptr;
    mSeSlotIndex = 0;
    for (u32 i = 0; i < 32; i++) {
        mpSeSound[i] = nullptr;
        mSeNum[i] = 0;
        mSePosAddr[i] = 0;
    }
    for (u32 i = 0; i < 4; i++) {
        mpKuroboMotionSound[i] = nullptr;
        mpKuroboVoiceSound[i] = nullptr;
    }
    mRiverPosCount = 0;
    mAllSeMuted = 0;
    mVoiceIndexHD = 0xFFFF;
    mpShipWaveSound = nullptr;
    mpSeaEnvSoundL = 0;
    mBrkWrailKSePlayed = 0;
    mSystemSoundHD = 0;
    mpShipCruiseSound = nullptr;
    mCameraMapInfo = 0;
    mAddGrassSePlayed = 0;
    field_0x020c = 0;
    mpSeaShoreSound = 0;
    mpSeaEnvSoundS = 0;
    mKuroboVoiceCount = 0;
    mpTelescopeSound = 0;
    mWaterfallPosCount = 0;
    mColBrkWrailingSePlayed = 0;
    mKuroboMotionCount = 0;
    mpWindSound = nullptr;
    mFireblastPosCount = 0;
    mIsMenuIn = 0;
    field_0x0210 = 0;
    mpSeaEnvSoundR = 0;
    mpCharVoiceSound = nullptr;
    mSeaEnvPosCount = 0;
    monsSeInit();
    initLevObjSE();
    setParamSoundOutputMode(2); /* JAudio global setting (2) */
    mTetraFlyStreamStarted = 0;
}

/* 0202587C */
void JAIZelBasic::kuroboVoicePlay(u32 id, cXyz *pos, u32 reverb) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 c = mKuroboVoiceCount;
    if (c >= 4)
        return;
    mKuroboVoiceCount = c + 1;
    stopSoundCategory(4, pos); /* JAIBasic: stop the object's sounds of category 4 */
    u32 i;
    for (i = 0; i < 4; i++)
        if (mpKuroboVoiceSound[i].get() == 0)
            break;
    if (i == 4)
        return;
    gptr<Sound> *handle = &mpKuroboVoiceSound[i];
    startSoundVec(id, handle, pos, 0, 0, 4);
    if (handle->get() != nullptr)
        handle->get()->setPortData(9, reverb & 0xFFFF);
}

/* 02025940 */
void JAIZelBasic::mbossBgmNearByProcess(f32 dist) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mpSubBgmSound.get() == 0)
        return;
    if (!(dist > ENEMY_NEARBY_DIST)) { /* JAIZelParam::ENEMY_NEARBY_DIST */
        s32 d1 = (s8)mEnemyNearbyOn;
        mEnemyNearbyFadeTimer = 0;
        if (d1 != 0)
            return;
        mpSubBgmSound.get()->setTrackVolume(MBOSS_ENEMY_NEARBY_TRACK, 1.0f,
                                            ENEMY_NEARBY_FADEIN_TIME);
        mEnemyNearbyOn = 1;
    } else if ((s8)mEnemyNearbyOn == 1) {
        u32 t = ENEMY_NEARBY_FADEOUT_TIME;
        mEnemyNearbyOn = 0;
        mEnemyNearbyFadeTimer = t;
    }
}

/* 020259E8 */
void JAIZelBasic::enemyNearBy() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mpMainBgmSound.get() != 0) {
        u32 n = mMainBgmNum;
        bool go = true;
        if (n == JA_BGM_DK_BATTLE) {
            if ((s8)mEnemyNearbyDelay != -1) {
                go = false;
            } else {
                bgmStart(mDkMaskBroken != 0 ? JA_BGM_UNK_112 : JA_BGM_UNK_111, 0, 0);
                if (mpMainBgmSound.get() == 0)
                    go = false;
                else
                    n = mMainBgmNum;
            }
        }
        if (go && n == JA_BGM_MJ_TOWER_BATTLE && (s8)mEnemyNearbyDelay == -1)
            bgmStart(JA_BGM_UNK_115, 0, 0);
    }
    mEnemyNearbyDelay = COMMON_BATTLE_FO_DELAY; /* JAIZelParam::COMMON_BATTLE_FO_DELAY */
}

/* 02025AB8 */
u32 JAIZelBasic::getMapInfoFxline(u32 n) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return n & 0xFFFF;
}

/* 02025AC0 */
s32 JAIZelBasic::checkLinkOnSea() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if ((s32)mSceneNo == 0x12 && (mIslandRoomNo == 0 || mLinkSeaFloorGroupInfo != 0))
        return 1;
    return 0;
}

/* 02025AF4 */
s32 JAIZelBasic::checkLinkOnBoardSea() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (checkLinkOnSea() == 1 && mLinkOnBoard != 0)
        return 1;
    return 0;
}

// HD: the nullable constructor factory preserves allocation before base initialization.
/* 02025B44 */
JAIZelBasic *createJAIZelBasic(JAIZelBasic *self) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (self == 0) {
        self = static_cast<JAIZelBasic *>(allocateAudioObject(0x21F4)); /* operator new */
        if (self == 0)
            return 0;
    }
    self->initializeAudioBase(); /* JAIBasic::JAIBasic */
    self->mHour = 0;
    self->mLinkBootsType = 0;
    self->mLinkHpRatio = 0.0f;
    self->mLinkOnBoard = 0;
    self->mHeartGaugeTimer = 0;
    self->mIsMenuIn = 0;
    self->mLinkShieldType = 0;
    self->mLinkSwordPlace = 0;
    self->mSceneBgmStartedOnce = 0;
    self->mLinkSwordShieldOnBack = 0;
    self->mMorningBgmPlaying = 0;
    self->mSceneFrameCount = 0;
    self->mWeekday = 0;
    self->mLinkHp = 0;
    self->mDayNightSwitched = 0;
    self->mLinkShieldPlace = 0;
    self->__vtbl = &l_basicVtable;
    self->mpEventBits = nullptr;
    self->field_0x002f = 0;
    self->mMinute = 0;
    self->mLinkSwordType = 0;
    self->mLinkMaxHp = 0;
    self->mPrevTime = 0;
    zeroBasicVector(&self->mCameraOldPos, 0xC); /* Vec ctor (zero) */
    zeroBasicVector(&self->mCameraPolygonPos, 0xC);
    self->mMainBgmMuteVolume = 0.0f;
    self->mSubBgmNum = 0;
    self->field_0x00b4 = 0;
    self->mSelectBgmOnly = 0;
    self->mMainBgmMenuVolume = 0.0f;
    self->mMainBgmSeaVolume = 0.0f;
    self->mFanfareResumePending = 0;
    self->mMainBgmTalkVolume = 0.0f;
    self->mKamomeFlag = 0;
    self->mPendingBgmStart = 0;
    self->mMainBgmSceneVolume = 0.0f;
    self->mEventTimer = 0;
    self->mLastMinibossSubBGMType = 0;
    self->mSubBgmTaktVolume = 0.0f;
    self->mCameraGroupLocked = 0;
    self->mMtDragonMuteRequest = 0;
    self->mMainBgmTaktVolume = 0.0f;
    self->mpSubBgmSound = nullptr;
    self->mSubBgmTalkMenuVolume = 0.0f;
    self->mSomeSpecialBGMFlag = 0;
    self->mTetraFlyStreamStarted = 0;
    self->mBattleBgmFadeDelay = 0;
    self->mTalkInBlocked = 0;
    self->mpStreamBgmSound = nullptr;
    self->mDemoFanfareMuted = 0;
    self->mEnemyNearbyOn = 0;
    self->mSubBgmCbPracticeVolume = 0.0f;
    self->mBgmDisabled = 0;
    self->mStreamBgmNum = 0;
    self->mEnemyNearbyFadeTimer = 0;
    self->mTalkSeBlocked = 0;
    self->mSkyCloistersTimer = 0;
    self->mSceneBgmLocked = 0;
    self->mKaitengiriTimer = 0;
    self->mMainBgmCbPracticeVolume = 0.0f;
    self->mMainBgmNum = 0;
    self->mMainBgmSubBgmVolume = 0.0f;
    self->mSwordUsing = 0;
    self->mbLandingDemoStarted = 0;
    self->mSubBgmStopTimer = 0;
    self->mpMainBgmSound = nullptr;
    self->mBattleBgmDisabled = 0;
    self->mMainBgmHeightVolume = 0.0f;
    self->mStreamBgmStarted = 0;
    self->mEnemyNearbyDelay = 0;
    self->mDkMaskBroken = 0;
    self->mSkipSceneStream = 0;
    self->mBattleBgmHoldTimer = 0;
    self->mpCbPracticePos = nullptr;
    self->mSeSlotIndex = 0;
    self->mNoCameraTimer = 0;
    self->mAddGrassSePlayed = 0;
    self->mCutGrassSeTimer = 0;
    self->mSwingSeTimers[1] = 0;
    self->mSceneSetPending = 0;
    self->mNextSceneBgmId = 0;
    self->mSwingSeTimers[0] = 0;
    self->mSeaEnvSeOff = 0;
    self->mSceneActive = 0;
    self->field_0x021e = 0;
    self->mpTelescopeSound = 0;
    self->mPrevFirstDynamicSceneWaveIndex = 0;
    self->mWaveToReload = 0;
    self->mBgnHeadRippleSeTimer = 0;
    self->mBrkWrailKSePlayed = 0;
    self->field_0x0210 = 0;
    self->mFanfareActive = 0;
    self->mTaktSeTimer = 0;
    self->mSetNum = 0;
    self->mColBrkWrailingSePlayed = 0;
    self->mBgnTailRippleSeTimer = 0;
    self->mWaveBankToErase = 0;
    self->mSecondDynamicSceneWaveIndex = 0;
    self->mCameraMapInfo = 0;
    self->mNextSceneNo = 0;
    self->mLinkSeaFloorGroupInfo = 0;
    self->mPrevSecondDynamicSceneWaveIndex = 0;
    self->mFirstDynamicSceneWaveIndex = 0;
    self->mSceneBgmChanged = 0;
    self->mBgnBodyRippleSeTimer = 0;
    self->mSecondDynamicWaveChanged = 0;
    self->field_0x020c = 0;
    self->mForceBack = 0;
    self->mPirateSailSePlayed = 0;
    self->mIslandRoomNo = 0;
    self->mBgnStringRippleSeTimer = 0;
    self->mAllSeMuted = 0;
    self->mWaveBankReload = 0;
    self->mLayerNo = 0;
    self->mCameraSeaFloorGroupInfo = 0;
    self->mSceneNo = 0;
    self->mInitialized = 0;
    self->mBgmWaveCountHD = 0;
    self->mBgmWaveInfoHD = nullptr;
    self->mLevelObjectSoundCountHD = 0;
    self->mSeaEnvPosCount = 0;
    zeroBasicVector(&self->mSeaPositionHD, 0xC);
    self->mIslandLinkBgm = 0;
    self->mRiverPosCount = 0;
    self->mpWindSound = nullptr;
    self->mFireblastPosCount = 0;
    self->mpShipCruiseSound = nullptr;
    self->mpSeaEnvSoundS = 0;
    self->mKuroboVoiceCount = 0;
    self->mWaterfallPosCount = 0;
    self->mVoiceIndexHD = 0;
    self->mSystemSoundHD = 0;
    self->mBstHoriSeOn = 0;
    self->mWindowPosCount = 0;
    self->mpShipWaveSound = nullptr;
    self->mpSeaEnvSoundL = 0;
    self->mpSeaEnvSoundR = 0;
    self->mKuroboMotionCount = 0;
    self->mpSeaShoreSound = 0;
    self->mShipSpeed = 0.0f;
    self->mIsSailing = 0;
    self->mpCharVoiceSound = nullptr;
    constructBasicRecords(self->mAllocationRecordsHD, 0x10, 8,
                          ALLOCATION_RECORD_CTOR); // HD:16 allocation records.
    self->mAllocationRecordIndexHD = 0;
    {
        Random *random = wrappedRandom(
            self); // HD: preserve the member-address wrap before the nullable RNG allocation.
        if (random == nullptr)
            random = static_cast<Random *>(allocateAudioObject(0x88));
        if (random != nullptr)
            seedBasicRandom(random, 0); /* setSeed(0) */
    }
    zel_basic = self;
    self->mLinkHpRatio = 1.0f;
    self->mCameraOldPos.z = 10000000.0f;
    self->mCameraPolygonPos.y = 10000000.0f;
    self->mCameraOldPos.y = 10000000.0f;
    self->mCameraOldPos.x = 10000000.0f;
    self->mCameraPolygonPos.z = 10000000.0f;
    self->mCameraPolygonPos.x = 10000000.0f;
    self->mMainBgmTaktVolume = 1.0f;
    self->mMainBgmMuteVolume = 1.0f;
    self->mMainBgmSeaVolume = 1.0f;
    self->mSubBgmCbPracticeVolume = 1.0f;
    self->mBgmDisabled = 0;
    self->mMainBgmNum = (u32)-1;
    self->mpStreamBgmSound = nullptr;
    self->mSubBgmNum = (u32)-1;
    self->mLinkOnBoard = 0;
    self->mSceneBgmStartedOnce = 0;
    self->mWeekday = 0;
    self->mLinkMaxHp = 0;
    self->mLinkHp = 0;
    self->mMinute = 0;
    self->mMainBgmCbPracticeVolume = 1.0f;
    self->mIsMenuIn = 0;
    self->mMainBgmTalkVolume = 1.0f;
    self->field_0x002f = 0;
    self->mMainBgmSceneVolume = 1.0f;
    self->mLinkShieldPlace = 0;
    self->mPrevTime = 0xC00;
    self->mPrevFirstDynamicSceneWaveIndex = 0;
    self->mpCbPracticePos = nullptr;
    self->mFanfareActive = 0;
    self->mSubBgmTaktVolume = 1.0f;
    self->mMtDragonMuteRequest = 0;
    self->mMainBgmSubBgmVolume = 1.0f;
    self->mWaveToReload = 0;
    self->mLinkShieldType = 0;
    self->mFirstDynamicSceneWaveIndex = 0;
    self->mLinkBootsType = 0;
    self->mFanfareResumePending = 0;
    self->mpEventBits = nullptr;
    self->mSomeSpecialBGMFlag = 0;
    self->mAllSeMuted = 0;
    self->mSceneNo = (u32)-1;
    self->mHour = 0;
    self->mMainBgmHeightVolume = 1.0f;
    self->mSwingSeTimers[0] = 0;
    self->mSecondDynamicSceneWaveIndex = 0;
    self->mTetraFlyStreamStarted = 0;
    self->mSubBgmStopTimer = (u32)-1;
    self->mPrevSecondDynamicSceneWaveIndex = 0;
    self->mCutGrassSeTimer = 0;
    self->mSubBgmTalkMenuVolume = 1.0f;
    self->mLinkSwordType = 0;
    self->mNextSceneNo = (u32)-1;
    self->mSeaEnvSeOff = 0;
    self->mWaveBankReload = 0;
    self->mLinkSwordShieldOnBack = 0;
    self->mNoCameraTimer = 0;
    self->mNextSceneBgmId = 0;
    self->mLinkSwordPlace = 0;
    self->mStreamBgmNum = (u32)-1;
    self->mSwingSeTimers[1] = 0;
    self->mSetNum = 0;
    self->mWaveBankToErase = 0;
    self->mMorningBgmPlaying = 0;
    self->mSecondDynamicWaveChanged = 0;
    self->mAddGrassSePlayed = 0;
    self->mMainBgmMenuVolume = 1.0f;
    self->mSceneBgmChanged = 0;
    self->mInitialized = 0;
    self->mTalkInBlocked = 0;
    self->mStreamBgmStarted = 0;
    self->mIslandLinkBgm = 0;
    self->mBrkWrailKSePlayed = 0;
    self->mSceneSetPending = 0;
    self->mColBrkWrailingSePlayed = 0;
    self->mLastMinibossSubBGMType = 0;
    self->mSceneActive = 0;
    self->field_0x00b4 = 0;
    self->mPendingBgmStart = 0;
    self->mBattleBgmFadeDelay = 0;
    self->mEnemyNearbyOn = 0;
    self->mEnemyNearbyFadeTimer = 0;
    self->mKaitengiriTimer = 0;
    self->mSwordUsing = 0;
    self->mBattleBgmDisabled = 0;
    self->mCameraSeaFloorGroupInfo = 0;
    self->mSceneFrameCount = 0;
    self->mHeartGaugeTimer = 0;
    self->mBgnTailRippleSeTimer = 0;
    self->mTalkSeBlocked = 0;
    self->mPirateSailSePlayed = 0;
    self->mShipSpeed = 0.0f;
    self->mBgnHeadRippleSeTimer = 0;
    self->mSkipSceneStream = 0;
    self->mDemoFanfareMuted = 0;
    self->mBgnBodyRippleSeTimer = 0;
    self->mSkyCloistersTimer = 0;
    self->mLayerNo = 0;
    self->mSceneBgmLocked = 0;
    self->mForceBack = 0;
    self->mDkMaskBroken = 0;
    self->mCameraGroupLocked = 0;
    self->mKamomeFlag = 0;
    self->mTaktSeTimer = 0;
    self->mEnemyNearbyDelay = 0xFF;
    self->mEventTimer = 0;
    self->mWindowPosCount = 0;
    self->mIsSailing = 0;
    self->field_0x021e = 0;
    self->mDayNightSwitched = 0;
    self->mBstHoriSeOn = 0;
    self->mBgnStringRippleSeTimer = 0;
    self->mLinkSeaFloorGroupInfo = 0;
    self->mbLandingDemoStarted = 0;
    self->mBattleBgmHoldTimer = 0;
    self->mSelectBgmOnly = 0;
    self->mIslandRoomNo = 0;
    return self;
}

// HD: resource paths use SafeString virtual termination and the driver receives HD gain values.
/* 02026044 */
void JAIZelBasic::init(void *heap, u32 size) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    setParamInterfaceHeapSize(DRAM_HEAP_SIZE);
    setParamSeRegistMax(SE_REGIST_MAX);
    setParamSoundSceneMax(SOUND_SCENE_MAX);
    setParamSeqControlBufferMax(SEQ_CONTROL_BUF_MAX);
    setParamStreamControlBufferMax(STREAM_CONTROL_BUF_MAX);
    setParamStreamDecodedBufferBlocks(STREAM_DECODED_BUFFERS);
    setParamAutoHeapMax(AUTO_HEAP_MAX);
    setParamStayHeapMax(STAY_HEAP_MAX);
    setParamAutoHeapRoomSize(AUTO_HEAP_ROOM_SIZE);
    setParamStayHeapSize(STAY_HEAP_ROOM_SIZE);
    setParamSeqPlayTrackMax(PLAY_TRACK_MAX);
    setParamSystemTrackMax(SYSTEM_TRACK_MAX);
    setParamDistanceMax(DISTANCE_MAX);
    setParamMaxVolumeDistance(MAX_VOLUME_DISTANCE);
    setParamMinDistanceVolume(MIN_DISTANCE_VOLUME);
    setParamSeDolbyCenterValue((u32)DOLBY_CENTER_VALUE);
    setParamSeDolbyFrontDistanceMax(DOLBY_FLONT_DISTANCE_MAX);
    setParamSeDolbyBehindDistanceMax(DOLBY_BEHIND_DISTANCE_MAX);
    setParamSeDistanceFxParameter((u32)DISTANCE_FX_PARAM);
    setParamInputGainDown(1.0f);
    setParamInputGainDown(l_inputGain); /* 0.707 */
    setParamOutputGainUp(1.0f);
    setParamAudioResPath(0);
    {
        AudioSafeString *path = getInitDataPath();
        assurePath(path);
        setParamInitDataFileName(pathString(path));
    }
    {
        AudioSafeString *path = getWavePath();
        assurePath(path);
        setParamWavePath(pathString(path));
    }
    {
        AudioSafeString *path = getSequencePath();
        assurePath(path);
        setParamSequenceArchivesPath(pathString(path));
    }
    setParamStreamPath(STREAM_PATH.get());
    setParamSequenceArchivesFileName(SEQ_ARCH_FILE_NAME.get());
    setParamAudioCameraMax(1);
    setParamDummyObjectMax(DUMMY_POSITION_MAX);
    setParamDummyObjectLifeTime(DUMMY_POSITION_LIFE_TIME);
    setParamStreamInsideBufferCut(1);
    initDriver(heap, size, 1); /* JAIBasic::initDriver */
    initInterface(1);          /* JAIBasic::initInterface */
    mpMainBgmSound = nullptr;
    mpSubBgmSound = nullptr;
    mpStreamBgmSound = nullptr;
    initSe();
    for (u32 c = 0; c < 8; c++)
        setSeCategoryVolume(c, l_defaultCategoryVolumes.entries[c]);
    mMainBgmHeightVolume = 1.0f;
    mMainBgmMuteVolume = 1.0f;
    mMainBgmCbPracticeVolume = 1.0f;
    mInitialized = 1;
    mMainBgmTaktVolume = 1.0f;
    mMainBgmSubBgmVolume = 1.0f;
    mBgmWaveCountHD = 0x57;
    mBgmWaveInfoHD = &m_bgm_wave_info;
    mMainBgmTalkVolume = 1.0f;
    mMainBgmSeaVolume = 1.0f;
    mMainBgmMenuVolume = 1.0f;
    mMainBgmSceneVolume = 1.0f;
}

/* 0202636C */
void JAIZelBasic::gframeProcess() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    Sound *h = seHandle.get(); /* JAInter::SeMgr::seHandle */
    if (h == 0)
        return;
    if (reinterpret_cast<SoundHead *>(h)->mState >= 4)
        zeldaGFrameWork();
    processFrameWork(); /* JAIBasic::processFrameWork */
}

/* 020263C0 */
void JAIZelBasic::resetProcess() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    for (u32 i = 0; i < 32; i++) {
        Sound *s = mpSeSound[i].get();
        if (s == 0)
            continue;
        s->stop(1);
        mpSeSound[i] = nullptr;
        mSePosAddr[i] = 0;
        mSeNum[i] = 0;
    }
    menuOut();
    for (u32 i = 0; i < getParamSeqPlayTrackMax(); i++) {
        SeqUpdateData *info = getPlayTrackInfo(i);
        if (info == 0)
            continue;
        Sound *s = info->mSequence.get();
        if (s != 0)
            s->setSeqInterVolume(6, 0.0f, 1);
    }
    Sound *h = seHandle.get();
    if (h != 0)
        h->setSeqInterVolume(6, 0.0f, 1);
    hd_stopAllStreams(hd_streamPlayer(l_soundManager.get()), 0); /* HD stream player: stop */
    mNextSceneBgmId = 0;
    mSceneBgmChanged = 0;
    mSceneActive = 0;
}

/* 020264F8 */
void JAIZelBasic::sceneChange(u32 bgm, u32 wave1, u32 wave2, s32 keep) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if ((s32)mSceneNo == 0x12 && mDayNightSwitched != 0 && checkDayTime() == 1)
        bgm = 0;
    u32 cur = mNextSceneBgmId;
    u32 n88 = mMainBgmNum;
    int same = -1; /* 1: keep the playing bgm (29E = 0), 0: start a new one (29E = 1) */
    if (bgm != cur) {
        if (n88 == JA_BGM_D_GANON_1 && checkBgmPlaying() == 1) {
            if (bgm > JA_BGM_UNK_133)
                same = 0;
            else if (bgm >= JA_BGM_UNK_130 || bgm == JA_BGM_D_GANON_1)
                same = 1;
            else
                same = 0;
        } else if (bgm == n88 && checkBgmPlaying() == 1) {
            same = 1;
        }
    } else if (bgm == n88 && checkBgmPlaying() != 0) {
        same = 1;
    }
    if (same < 0)
        same = checkSeqIDDemoPlaying(bgm) == 1 ? 1 : 0;
    Sound *strm = mpStreamBgmSound.get();
    mNextSceneBgmId = bgm;
    mSceneBgmChanged = same ? 0 : 1;
    if (strm != 0)
        strm->stop(BGM_SCENE_CHANGE_FO_TIME);
    mpStreamBgmSound = nullptr;
    mStreamBgmNum = (u32)-1;
    hd_releaseSceneStreams();
    if (mSceneBgmChanged != 0) {
        if (keep == 0) {
            u32 n84 = mSubBgmNum;
            if ((n84 == JA_BGM_TAKT_SHIPPU || n84 == JA_BGM_TAKT_CHUYA) && mpSubBgmSound.get() != 0)
                bgmStop(BGM_SCENE_CHANGE_FO_TIME, 1);
            else
                bgmStop(BGM_SCENE_CHANGE_FO_TIME, 0);
        }
        mWaveBankReload = 0;
    }
    bool w2same;
    if (wave1 != mFirstDynamicSceneWaveIndex && wave1 != 0) {
        mPrevFirstDynamicSceneWaveIndex = mFirstDynamicSceneWaveIndex;
        u32 w2 = mSecondDynamicSceneWaveIndex;
        mFirstDynamicSceneWaveIndex = wave1;
        mSetNum = 1;
        w2same = wave2 == w2;
    } else {
        u32 w2 = mSecondDynamicSceneWaveIndex;
        mSetNum = 0;
        w2same = wave2 == w2;
    }
    if (!w2same && wave2 != 0) {
    } else if (mSetNum == 0) {
        mSecondDynamicWaveChanged = 0;
        return;
    }
    u32 old = mSecondDynamicSceneWaveIndex;
    mSecondDynamicSceneWaveIndex = wave2;
    mPrevSecondDynamicSceneWaveIndex = old;
    mSecondDynamicWaveChanged = 1;
}

/* 0202677C */
void JAIZelBasic::setScene(s32 sc, s32 room, s32 keep, s32 layer) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    enum BgmSelection { NoBgm, SpecialBgm, SceneTableBgm, ExpandedSpecialBgm, AlreadyChanged };
    if (sc >= 0x79)
        return;
    if (mSceneSetPending != 0)
        return;
    mSceneBgmLocked = 0;
    mSkipSceneStream = 0;
    if (sc == 0x12) {
        /* the sea: the island room's bgm (m_isle_info at 1018E60C) */
        u32 r = room > 0 ? (room & 0xFF) : 0;
        mIslandRoomNo = r;
        u32 bgm;
        if (keep == 1) {
            bgm = 0;
        } else {
            BgmSelection selection = SceneTableBgm; /* 0: none, 1: special id, 2: the table */
            u32 special = 0;
            if (r == dIsleRoom_OutsetIsland_e) {
                if (layer == 0xA) {
                    selection = NoBgm;
                } else if (checkEventBit(0x3510) == 0) {
                    selection = SpecialBgm;
                    special = JA_BGM_ISLAND_LINK_0;
                } else {
                    bool a;
                    if (checkEventBit(1) == 1) {
                        s32 e = checkEventBit(0x101);
                        if (e == 0) {
                            selection = SpecialBgm;
                            special = JA_BGM_ISLAND_LINK_2;
                        }
                        a = e == 1;
                    } else {
                        a = checkEventBit(0x101) == 1;
                    }
                    if (selection == SceneTableBgm) {
                        s32 e = checkEventBit(0xE20);
                        if (a && e == 0)
                            selection = NoBgm;
                        else if (e == 1) {
                            selection = SpecialBgm;
                            special = JA_BGM_ISLAND_LINK_3;
                        }
                    }
                }
            } else if (r == dIsleRoom_WindfallIsland_e) {
                if (checkEventBit(0x2E01) == 0)
                    selection = NoBgm;
            } else if (r == dIsleRoom_DragonRoostIsland_e) {
                if (layer == 8 || layer == 0xA || layer == dIsleRoom_WindfallIsland_e)
                    selection = NoBgm;
            } else if (r == dIsleRoom_FlightControlPlatform_e) {
                if (layer == 2 || layer == 3) {
                    mSceneBgmLocked = 1;
                    sceneChange(0, 0, 0, keep);
                    mNextSceneNo = sc;
                    mSceneActive = 0;
                    mSceneSetPending = 1;
                    mLayerNo = layer;
                    return;
                }
            }
            if (selection == NoBgm)
                bgm = 0;
            else if (selection == SpecialBgm)
                bgm = special;
            else
                bgm = expandSceneBgmNum(m_isle_info.entries[r].bgmNum);
        }
        sceneChange(bgm, 0, 0, keep);
    } else {
        /* m_scene_info at 1018E428: {u16 bgm, u8 wave1, u8 wave2} */
        mIslandRoomNo = 0;
        scene_info_s *entry = sceneEntry(sc);
        BgmSelection selection =
            SceneTableBgm; /* 0: bgm 0, 1: special, 2: the table, 3: special with an expanded id */
        u32 special = 0;
        bool dungeon = false;
        u32 memOff = 0;
        scene_info_s *altBgm = nullptr;
        bool set274 = false;
        switch (sc) {
        case 0x10:
            if (checkEventBit(0x280) == 0) {
                selection = SpecialBgm;
                special = JA_BGM_FIND_TETRA;
            }
            break;
        case 0x13:
            if (checkEventBit(0x801) == 0)
                selection = NoBgm;
            break;
        case 2:
            dungeon = true;
            memOff = 0x40C;
            altBgm = &m_scene_info.entries[1];
            set274 = true;
            break;
        case 7:
            dungeon = true;
            memOff = 0x430;
            altBgm = &m_scene_info.entries[3];
            break;
        case 0x25:
            dungeon = true;
            memOff = 0x454;
            altBgm = &m_scene_info.entries[10];
            break;
        case 0x18:
        case 0x2D:
            if (mSceneBgmStartedOnce == 0)
                break;
            memOff = sc == 0x18 ? 0x49C : 0x478;
            if (isStageItem(memOff, 5) != 0)
                mSkipSceneStream = 1;
            if ((u32)(layer - 8) <= 3)
                selection = NoBgm;
            else if (isStageItem(memOff, 3) != 0)
                selection = NoBgm;
            break;
        case 0x59:
            if ((u32)(layer - 8) <= 3) {
                selection = NoBgm;
                break;
            }
            /* fall through */
        case 0x36:
            if (checkEventBit(0x2D04) == 0) {
                selection = SpecialBgm;
                special = JA_BGM_HYRULE_OFF;
            }
            break;
        case 0x35:
        case 0x27:
        case 0x29:
        case 0x16:
        case 0x2C:
            if ((u32)(layer - 8) <= 3)
                selection = NoBgm;
            break;
        case 0xB:
            if (mLinkShieldType == 0) {
                if (layer == 9)
                    selection = NoBgm;
            } else if (checkEventBit(0x2A20) == 0) {
                selection = SpecialBgm;
                special = JA_BGM_BAACHAN;
            }
            break;
        case 0x20:
            if (checkEventBit(0x2110) == 0 && checkEventBit(0xA02) == 1) {
                selection = SpecialBgm;
                special = JA_BGM_PIRATE_DEMO5;
            }
            break;
        }
        if (dungeon && mSceneBgmStartedOnce != 0) {
            if (isStageItem(memOff, 5) != 0)
                mSkipSceneStream = 1;
            if (isStageItem(memOff, 3) != 0) {
                u32 bgm = expandSceneBgmNum(altBgm->bgmNum);
                if (set274)
                    mSeaEnvSeOff = 1;
                sceneChange(bgm, entry->dyWave1st, entry->dyWave2nd, keep);
                selection = AlreadyChanged;
            }
        }
        if (selection == NoBgm)
            sceneChange(0, entry->dyWave1st, entry->dyWave2nd, keep);
        else if (selection == SpecialBgm)
            sceneChange(special, entry->dyWave1st, entry->dyWave2nd, keep);
        else if (selection == SceneTableBgm) {
            u32 bgm = expandSceneBgmNum(entry->bgmNum);
            sceneChange(bgm, entry->dyWave1st, entry->dyWave2nd, keep);
        }
    }
    mSceneActive = 0;
    mLayerNo = layer;
    mSceneSetPending = 1;
    mNextSceneNo = sc;
}

// HD: stop/reset scene streams while preserving each bank/flag reload after callbacks.
/* 02026E04 */
void JAIZelBasic::load1stDynamicWave() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mSceneNo = mNextSceneNo;
    if (mWaveBankReload != 0 || mSetNum != 0 || mSecondDynamicWaveChanged != 0) {
        u32 w = mPrevSecondDynamicSceneWaveIndex;
        u32 b = m_dy_wave_set_2nd.entries[w].banks[1], a = m_dy_wave_set_2nd.entries[w].banks[0];
        if (b != 0)
            unloadWave(b);
        if (a != 0)
            unloadWave(a);
        if (mWaveBankReload != 0 || mSetNum != 0) {
            u32 w1 = mPrevFirstDynamicSceneWaveIndex;
            u32 b1 = m_dy_wave_set_1st.entries[w1].banks[1],
                a1 = m_dy_wave_set_1st.entries[w1].banks[0];
            if (b1 != 0)
                unloadWave(b1);
            if (a1 != 0)
                unloadWave(a1);
            if (mWaveBankReload != 0) {
                bool load29F = true;
                u32 x = mWaveBankToErase;
                if (x != 0) {
                    eraseWaveBank(x, 0);
                    eraseWaveHeap((u32)mWaveBankToErase, 0);
                    resetWaveBank((u32)mWaveBankToErase, (u32)-1);
                    load29F = mWaveBankReload != 0;
                }
                if (load29F) {
                    u32 y = mWaveToReload;
                    if (y != 0)
                        loadWave(y);
                }
            }
            if (mSetNum != 0) {
                u32 w2 = mFirstDynamicSceneWaveIndex;
                u32 c = m_dy_wave_set_1st.entries[w2].banks[0],
                    d = m_dy_wave_set_1st.entries[w2].banks[1];
                if (c != 0)
                    loadWave(c);
                if (d != 0)
                    loadWave(d);
            }
        }
    }
    mSceneSetPending = 0;
    mIslandLinkBgm = 0;
    u32 n84 = mSubBgmNum;
    mMainBgmMenuVolume = 1.0f;
    mMainBgmMuteVolume = 1.0f;
    mMainBgmTalkVolume = 1.0f;
    if (n84 != JA_BGM_TAKT_SHIPPU && n84 != JA_BGM_TAKT_CHUYA && n84 != JA_BGM_SEA_ENEMY)
        mMainBgmSubBgmVolume = 1.0f;
    u32 n88 = mMainBgmNum;
    Sound *main = mpMainBgmSound.get();
    if (n88 != JA_BGM_ISLAND_WOOD)
        mMainBgmSceneVolume = 1.0f;
    f32 f94 = mMainBgmTalkVolume, f9c = mMainBgmSubBgmVolume;
    mMainBgmHeightVolume = 1.0f;
    mSubBgmTaktVolume = 1.0f;
    mMainBgmCbPracticeVolume = 1.0f;
    mMainBgmSeaVolume = 1.0f;
    mSubBgmTalkMenuVolume = 1.0f;
    mMainBgmTaktVolume = 1.0f;
    mSubBgmCbPracticeVolume = 1.0f;
    if (main == 0 || mMainBgmNum != JA_BGM_MORNING)
        mMorningBgmPlaying = 0;
    f32 v = gabi::fmuls_ppc(gabi::fmuls_ppc(f94, f9c), 1.0f);
    mDemoFanfareMuted = 0;
    hdStreamVolume(1, v);
    u32 sc = mSceneNo;
    mPirateSailSePlayed = 0;
    mSceneFrameCount = 0;
    field_0x021e = 1;
    mKamomeFlag = 0;
    mpCbPracticePos = nullptr;
    mbLandingDemoStarted = 0;
    mBstHoriSeOn = 0;
    mSeaEnvSeOff = 0;
    mSceneActive = 0;
    mTalkSeBlocked = 0;
    if (sc == 0x76) {
        hd_selectSceneStream(1);
        hd_loadSceneStream();
    } else if (sc == 0x77) {
        hd_selectSceneStream(2);
        hd_loadSceneStream();
    } else {
        hd_resetSceneStream();
    }
    for (u32 k = 0; k < 30; k++) {
        Sound *s = mMonsSe[k].mpSound.get();
        if (s != 0)
            s->stop(0);
    }
    monsSeInit();
}

/* 02027168 */
void JAIZelBasic::bgmStreamPrepare(u32 id) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mBgmDisabled != 0)
        return;
    prepareStream(id, &mpStreamBgmSound, 0, 0, 0, 4); /* JAIBasic: prepare a stream */
    if (id == JA_STRM_BST_START)
        mTalkSeBlocked = 1;
    mStreamBgmNum = id;
}

/* 020271E4 */
void JAIZelBasic::load2ndDynamicWave() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 sc = mSceneNo;
    u32 id = 0;
    u32 streamID = 0;
    switch (sc) {
    case 2:
        streamID = JA_STRM_BTD_START;
        break;
    case 7:
        streamID = JA_STRM_BKM_START;
        break;
    case 0x25:
        streamID = JA_STRM_BST_START;
        break;
    case 0x18:
        streamID = JA_STRM_RANE_START;
        break;
    case 0x2D:
        streamID = JA_STRM_BPW_START;
        break;
    }
    if (streamID != 0 && mSkipSceneStream == 0)
        id = streamID;
    if (mSecondDynamicWaveChanged != 0) {
        u32 w = mSecondDynamicSceneWaveIndex;
        u32 a = m_dy_wave_set_2nd.entries[w].banks[0], b = m_dy_wave_set_2nd.entries[w].banks[1];
        if (a != 0)
            loadWave(a);
        if (b != 0)
            loadWave(b);
    }
    if (id != 0)
        bgmStreamPrepare(id);
}

/* 0202736C */
void JAIZelBasic::resetRecover() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    Sound *h = seHandle.get();
    if (h != 0)
        h->setSeqInterVolume(6, 1.0f, 0);
    for (u32 i = 0; i < getParamSeqPlayTrackMax(); i++) {
        SeqUpdateData *info = getPlayTrackInfo(i);
        if (info == 0)
            continue;
        Sound *s = info->mSequence.get();
        if (s == 0)
            continue;
        if (s == seHandle.get())
            continue;
        s->stop(0);
    }
    hd_stopAllStreams(hd_streamPlayer(l_soundManager.get()), 0);
    setScene(0, 0, 0, -1);
    load1stDynamicWave();
    sceneBgmStart();
    load2ndDynamicWave();
    mMainBgmSubBgmVolume = 1.0f;
    mMainBgmCbPracticeVolume = 1.0f;
    mMainBgmMenuVolume = 1.0f;
    mMainBgmSceneVolume = 1.0f;
    mMainBgmTalkVolume = 1.0f;
    mSubBgmTalkMenuVolume = 1.0f;
    mMainBgmHeightVolume = 1.0f;
    mMainBgmSeaVolume = 1.0f;
    mMainBgmMuteVolume = 1.0f;
    mMainBgmTaktVolume = 1.0f;
    mSubBgmTaktVolume = 1.0f;
    for (u32 c = 0; c < 8; c++)
        setSeCategoryVolume(c, l_defaultCategoryVolumes.entries[c]);
    mFanfareActive = 0;
    mAllSeMuted = 0;
    mCameraGroupLocked = 0;
    mTaktSeTimer = 0;
}

// HD: sentinel/no camera retains the previous position and resets the10-frame timer.
/* 02027558 */
void JAIZelBasic::getCameraInfo(cXyz *pos, Mtx34 *mtx, u32 idx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    bool none = false;
    auto camera = cameraAt(idx);
    if (pos == nullptr)
        none = true;
    else if (pos->x == 1e7f && pos->y == 1e7f && pos->z == 1e7f)
        none = true;
    cXyz *old = camera->position.get();
    auto previousWords = reinterpret_cast<VectorWords *>(&mCameraOldPos);
    if (old != nullptr) {
        auto oldWords = reinterpret_cast<VectorWords *>(old);
        previousWords->w[0] = oldWords->w[0];
        previousWords->w[1] = oldWords->w[1];
        previousWords->w[2] = oldWords->w[2];
    } else {
        mCameraOldPos.z = 1e7f;
        mCameraOldPos.y = 1e7f;
        mCameraOldPos.x = 1e7f;
    }
    cameraAt(idx)->position = none ? &mCameraOldPos : pos;
    cameraAt(idx)->previousPosition = &mCameraOldPos;
    cameraAt(idx)->matrix = mtx;
    if (none) {
        mNoCameraTimer = 10;
        mCameraOldPos.x = 1e7f;
        mCameraOldPos.y = 1e7f;
        mCameraOldPos.z = 1e7f;
        return;
    }
    u32 timer = mNoCameraTimer;
    if (timer != 0)
        mNoCameraTimer = timer - 1;
}

/* 02027668 */
void JAIZelBasic::getCameraMapInfo(u32 v) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mCameraMapInfo = v;
}

/* 02027670 */
void JAIZelBasic::setCameraGroupInfo(u32 g) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (isDemo() == 1)
        return;
    if (mSceneBgmLocked != 0)
        return;
    if (mCameraGroupLocked != 0)
        return;
    u32 cur = mCameraSeaFloorGroupInfo;
    if (g == cur)
        return;
    if (g & 0x80) {
        u32 cur2 = mCameraSeaFloorGroupInfo;
        mIslandRoomNo = g & 0x3F;
        if (cur2 == 0) {
            /* outer sea -> island edge */
            setScene(0x12, g & 0x3F, 1, -1);
            load1stDynamicWave();
            load2ndDynamicWave();
            mCameraSeaFloorGroupInfo = g;
            field_0x021e = 0;
            mSceneActive = 1;
            return;
        }
    } else if (g == 0 || (g & 0x40) != 0) {
        if (cur & 0x80)
            mIslandRoomNo = 0;
    }
    mCameraSeaFloorGroupInfo = g;
}

/* 02027754 */
void JAIZelBasic::setCameraPolygonPos(cXyz *pos) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (pos == nullptr)
        return;
    auto words = reinterpret_cast<VectorWords *>(pos);
    auto polygon = reinterpret_cast<VectorWords *>(&mCameraPolygonPos);
    polygon->w[0] = words->w[0];
    polygon->w[1] = words->w[1];
    polygon->w[2] = words->w[2];
}

/* 02027778 */
void JAIZelBasic::setLinkGroupInfo(u32 g) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if ((s32)mSceneNo != 0x12)
        return;
    if (mIslandRoomNo == 0)
        return;
    if (g == mLinkSeaFloorGroupInfo)
        return;
    mLinkSeaFloorGroupInfo = g;
}

/* 020277A4 */
void JAIZelBasic::setEventBit(GuestByte *p) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mpEventBits = p;
}

/* 020277AC */
s32 JAIZelBasic::spotNameToId(GuestByte *name) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (name == 0)
        return 0;
    u32 i;
    for (i = 0; i < 120; i++) {
        GuestByte *candidate = spot_dir_name.entries[i].get();
        u32 k = 0;
        u8 a, b;
        do {
            a = name[k];
            b = candidate[k];
            k++;
        } while (a == b && a != 0);
        if (a == b)
            break;
    }
    if (i == 120)
        return 0;
    return i + 1;
}

/* 02027814 */
void JAIZelBasic::setSceneName(GuestByte *name, s32 room, s32 layer) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s32 id = spotNameToId(name);
    if (id == 0x75) {
        for (u32 i = 0; i < 32; i++) {
            Sound *s = mpSeSound[i].get();
            if (s != 0) {
                s->stop(1);
                mpSeSound[i] = nullptr;
            }
            mSeNum[i] = 0;
            mSePosAddr[i] = 0;
            mSeSlotIndex = 0;
        }
        menuOut();
        mSelectBgmOnly = 1;
        mCameraGroupLocked = 0;
        setScene(id, room, 0, layer);
    } else {
        mSelectBgmOnly = 0;
        setScene(id, room, 0, layer);
    }
}

/* 020278E8 */
s32 JAIZelBasic::checkStaticWaves() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    for (u32 i = 0; i < 20; i++)
        if (getWaveLoadStatus(l_staticWaves.banks[i]) != 2)
            return 0;
    return 1;
}

/* 0202796C */
u32 JAIZelBasic::check1stDynamicWave() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 w = mFirstDynamicSceneWaveIndex;
    u32 a = m_dy_wave_set_1st.entries[w].banks[0], b = m_dy_wave_set_1st.entries[w].banks[1];
    u32 sa = 0, sb = 0;
    if (a != 0)
        sa = 2 - getWaveLoadStatus(a);
    if (b != 0)
        sb = 2 - getWaveLoadStatus(b);
    u32 r = (sa << 8) + sb;
    u32 sc = mSceneNo;
    if ((s32)sc != 0x75) {
        if (checkStaticWaves() == 0)
            return 1;
        sc = mSceneNo;
    }
    if (sc == 0x76) {
        if (hd_sceneStreamReady(1) == 0)
            return 1;
    } else if (sc == 0x77) {
        if (hd_sceneStreamReady(2) == 0)
            return 1;
    }
    return r;
}

/* 02027A7C */
void JAIZelBasic::loadStaticWaves() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (checkStaticWaves() != 0)
        return;
    for (u32 i = 0; i < 20; i++)
        loadWave(l_staticWaves.banks[i]);
}

/* 02027AE4 */
s32 JAIZelBasic::checkFirstWaves() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 2 - getWaveLoadStatus(2);
}

// HD: low health accelerates battle, miniboss and sea-battle BGM by different factors.
/* 02027B0C */
void JAIZelBasic::setLinkHp(s32 hp, s32 max) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mLinkHp = hp;
    mLinkMaxHp = max;
    f32 r;
    if (max != 0) {
        r = (f32)hp / (f32)max;
        if (r > 1.0f)
            r = 1.0f;
    } else {
        r = 1.0f;
    }
    Sound *sub = mpSubBgmSound.get();
    mLinkHpRatio = r;
    if (sub == 0)
        return;
    u32 n = mSubBgmNum;
    if (n == JA_BGM_SEA_ENEMY)
        sub->setTempoProportion(gabi::fmadds(l_seaBattleTempoRise, 1.0f - r, 1.0f), 0);
    else if (n > JA_BGM_MBOSS_S)
        return;
    else if (n >= JA_BGM_MBOSS)
        sub->setTempoProportion(gabi::fmadds(l_minibossTempoRise, 1.0f - r, 1.0f), 0);
    else if (n == JA_BGM_BATTLE_NORM)
        sub->setTempoProportion(gabi::fmadds(l_battleTempoRise, 1.0f - r, 1.0f), 0);
}

/* 02027C50 */
void JAIZelBasic::setLinkSwordType(u32 a, u32 b) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mLinkSwordType = a;
    mLinkSwordPlace = b;
    mLinkSwordShieldOnBack =
        ((a & 0xFF) != 0 && (b & 0xFF) == 2 && mLinkShieldType != 0 && mLinkShieldPlace == 2) ? 1 : 0;
}

/* 02027C9C */
void JAIZelBasic::setLinkShieldType(u32 a, u32 b) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 sw = mLinkSwordType;
    mLinkShieldType = a;
    mLinkShieldPlace = b;
    mLinkSwordShieldOnBack = (sw != 0 && mLinkSwordPlace == 2 && mLinkShieldType != 0 && (b & 0xFF) == 2) ? 1 : 0;
}

/* 02027CEC */
void JAIZelBasic::setLinkBootsType(u32 a) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mLinkBootsType = a;
}

/* 02027CF4 */
void JAIZelBasic::setLinkOnBoard(u32 a) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    Sound *sub = mpSubBgmSound.get();
    mLinkOnBoard = a;
    if (sub == 0)
        return;
    if (mSubBgmNum != JA_BGM_SEA_ENEMY)
        return;
    bgmMute(&mpSubBgmSound, JA_BGM_SEA_ENEMY, (a & 0xFF) == 1 ? 0 : 1, 0xA);
}

/* 02027D40 */
void JAIZelBasic::heartGaugeOn() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mHeartGaugeTimer = 2;
}

// clang-format off
/* WWHD glue:
data DRAM_HEAP_SIZE 1018DC14
data SE_REGIST_MAX 1018DC18
data SOUND_SCENE_MAX 1018DC1C
data STREAM_DECODED_BUFFERS 1018DC20
data SEQ_CONTROL_BUF_MAX 1018DC24
data STREAM_CONTROL_BUF_MAX 1018DC28
data AUTO_HEAP_MAX 1018DC2C
data STAY_HEAP_MAX 1018DC30
data AUTO_HEAP_ROOM_SIZE 1018DC34
data STAY_HEAP_ROOM_SIZE 1018DC38
data SYSTEM_TRACK_MAX 1018DC3C
data PLAY_TRACK_MAX 1018DC40
data DISTANCE_MAX 1018DC44
data MAX_VOLUME_DISTANCE 1018DC48
data MIN_DISTANCE_VOLUME 1018DC4C
data DOLBY_FLONT_DISTANCE_MAX 1018DC50
data DOLBY_BEHIND_DISTANCE_MAX 1018DC54
data DUMMY_POSITION_MAX 1018DC58
data DUMMY_POSITION_LIFE_TIME 1018DC5C
data STREAM_PATH 1018DC60
data SEQ_ARCH_FILE_NAME 1018DC64
data BGM_SCENE_CHANGE_FO_TIME 1018DC74
data ENEMY_NEARBY_DIST 1018DC78
data ENEMY_NEARBY_FADEIN_TIME 1018DC7C
data ENEMY_NEARBY_FADEOUT_TIME 1018DC80
data DISTANCE_FX_PARAM 1018DC94
data DOLBY_CENTER_VALUE 1018DC96
data COMMON_BATTLE_FO_DELAY 1018DCB4
data MBOSS_ENEMY_NEARBY_TRACK 1018DCB5
data l_inputGain 100039CC
data l_battleTempoRise 100039D0
data l_minibossTempoRise 100039D4
data l_seaBattleTempoRise 100039D8
data seHandle 104B5018
data l_soundManager 1018EC64
data l_save 101F84DC
data zel_basic 101FFC78
data l_basicVtable 10003A08
data m_dy_wave_set_1st 1018E3C4
data m_dy_wave_set_2nd 1018E2EC
data m_scene_info 1018E428
data m_isle_info 1018E60C
data l_defaultCategoryVolumes 1018DC97
data l_staticWaves 10003844
data spot_dir_name 1018E6D4
data m_bgm_wave_info 1018E314
ext 02804454 void setParamInterfaceHeapSize(u32)
ext 0280446C void setParamSeRegistMax(u32)
ext 02804460 void setParamSoundSceneMax(u32)
ext 02804484 void setParamSeqControlBufferMax(u32)
ext 02804490 void setParamStreamControlBufferMax(u32)
ext 02804594 void setParamStreamDecodedBufferBlocks(u32)
ext 0280449C void setParamAutoHeapMax(u32)
ext 028044A8 void setParamStayHeapMax(u32)
ext 028045B0 void setParamAutoHeapRoomSize(u32)
ext 028045BC void setParamStayHeapSize(u32)
ext 028044B4 void setParamSeqPlayTrackMax(u32)
ext 02804680 void setParamSystemTrackMax(u32)
ext 028044E4 void setParamDistanceMax(f32)
ext 028044F0 void setParamMaxVolumeDistance(f32)
ext 028044FC void setParamMinDistanceVolume(f32)
ext 028045C8 void setParamSeDolbyCenterValue(u32)
ext 028045FC void setParamSeDolbyFrontDistanceMax(f32)
ext 02804608 void setParamSeDolbyBehindDistanceMax(f32)
ext 02804588 void setParamSeDistanceFxParameter(u32)
ext 028044CC void setParamInputGainDown(f32)
ext 028044D8 void setParamOutputGainUp(f32)
ext 02804644 void setParamAudioResPath(u32)
ext 02804614 void setParamInitDataFileName(jaizel7::GuestByte*)
ext 02804620 void setParamWavePath(jaizel7::GuestByte*)
ext 0280462C void setParamSequenceArchivesPath(jaizel7::GuestByte*)
ext 02804638 void setParamStreamPath(jaizel7::GuestByte*)
ext 02804650 void setParamSequenceArchivesFileName(jaizel7::GuestByte*)
ext 02804674 void setParamAudioCameraMax(u32)
ext 02804668 void setParamDummyObjectMax(u32)
ext 0280465C void setParamDummyObjectLifeTime(u32)
ext 02804598 void setParamStreamInsideBufferCut(u32)
ext 02804508 void setParamSoundOutputMode(u32)
ext 0202EA8C jaizel7::AudioSafeString* getInitDataPath()
ext 0202EA98 jaizel7::AudioSafeString* getWavePath()
ext 0202EAA4 jaizel7::AudioSafeString* getSequencePath()
ext 02030C10 void hd_releaseSceneStreams()
ext 02030E90 void hd_selectSceneStream(u32)
ext 02030E64 void hd_loadSceneStream()
ext 02030E38 void hd_resetSceneStream()
ext 02030F1C s32 hd_sceneStreamReady(u32)
ext 02812E70 void eraseWaveBank(u32,u32)
ext 0280203C void eraseWaveHeap(u32,u32)
ext 02802028 void resetWaveBank(u32,u32)
ext 02021290 void jaizel7::JAIZelBasic::initLevObjSE()
ext 0202204C void jaizel7::JAIZelBasic::bgmStart(u32,u32,u32)
ext 02021F28 void jaizel7::JAIZelBasic::bgmStop(u32,u32)
ext 02021E30 s32 jaizel7::JAIZelBasic::checkBgmPlaying()
ext 020217A4 s32 jaizel7::JAIZelBasic::checkSeqIDDemoPlaying(u32)
ext 02023474 s32 jaizel7::JAIZelBasic::checkDayTime()
ext 0201E480 s32 jaizel7::JAIZelBasic::checkEventBit(u32)
ext 02023608 u32 jaizel7::JAIZelBasic::expandSceneBgmNum(u32)
ext 020234A4 void jaizel7::JAIZelBasic::sceneBgmStart()
ext 0201DCD4 void jaizel7::JAIZelBasic::menuOut()
ext 0201E5CC s32 jaizel7::JAIZelBasic::isDemo()
ext 02024478 void jaizel7::JAIZelBasic::zeldaGFrameWork()
ext 02021B10 void jaizel7::JAIZelBasic::hdStreamVolume(u32,f32)
ext 0201DD18 void jaizel7::JAIZelBasic::bgmMute(gptr<jaizel7::Sound>*,u32,u32,u32)
ext 02802848 void jaizel7::JAIZelBasic::startSoundVec(u32,gptr<jaizel7::Sound>*,cXyz*,u32,u32,u32)
ext 02803138 void jaizel7::JAIZelBasic::setSeCategoryVolume(u32,u32)
ext 02802DFC void jaizel7::JAIZelBasic::stopSoundCategory(u32,cXyz*)
ext 02802564 void jaizel7::JAIZelBasic::initializeAudioBase()
ext 02803614 void jaizel7::JAIZelBasic::initDriver(void*,u32,u32)
ext 02803BC4 void jaizel7::JAIZelBasic::initInterface(u32)
ext 02802F20 void jaizel7::JAIZelBasic::processFrameWork()
ext 02802880 void jaizel7::JAIZelBasic::prepareStream(u32,gptr<jaizel7::Sound>*,u32,u32,u32,u32)
ext 0280B1D8 void jaizel7::Sound::stop(u32)
ext 0280C6D8 void jaizel7::Sound::setPortData(u32,u32)
ext 0280C9E4 void jaizel7::Sound::setTrackVolume(u32,f32,u32)
ext 0280B2AC void jaizel7::Sound::setSeqInterVolume(u32,f32,u32)
ext 0280C320 void jaizel7::Sound::setTempoProportion(f32,u32)
ext 0280470C u32 getParamSeqPlayTrackMax()
ext 0280AD2C jaizel7::SeqUpdateData* getPlayTrackInfo(u32)
ext 0202BF78 void* hd_streamPlayer(void*)
ext 027632E4 void hd_stopAllStreams(void*,u32)
ext 025B9100 s32 isDungeonItem(void*,u32)
ext 028024CC s32 getWaveLoadStatus(u32)
ext 02802440 void loadWaveBank(u32,u32)
ext 0273AD10 void* allocateAudioObject(u32)
ext 028F521C void zeroBasicVector(cXyz*,u32)
ext 028EFFD0 void constructBasicRecords(jaizel7::AllocationRecord*,u32,u32,u32)
ext 027ED730 void seedBasicRandom(jaizel7::Random*,u32)
*/
