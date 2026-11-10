/** JAIZelBasic part5: scene/sea music transitions and health alarms.
 * Private WWHD-derived source, verified against the original.
 * GameCube function/field names from zeldaret/tww; HD-specific behavior is marked below. */
#include "JAZelAudio/JAIZelBasic_5.h"
#include "gc_api/d/d_com_inf_game.h"
using namespace jaizel5;

static IsleAreas l_isleAreas;
static IsleInfo l_isleInfo;
using ManagerPointer = gptr<void>;
static ManagerPointer l_soundManager;
using SavePointer = gptr<SaveOptionsView>;
static SavePointer l_save;
void *hd_streamPlayer(void *);
s32 hd_checkStream(void *, u32, u32, u32, u32);
void *hd_getOptions(void *);
s32 hd_heartAlarmDisabled(void *);

using ParamENEMY_NEARBY_FADEOUT_TIME = be<u32>;
static ParamENEMY_NEARBY_FADEOUT_TIME ENEMY_NEARBY_FADEOUT_TIME;
using ParamBTL_KAITENGIRI_FADEOUT_TIME = be<u32>;
static ParamBTL_KAITENGIRI_FADEOUT_TIME BTL_KAITENGIRI_FADEOUT_TIME;
using ParamBTL_SWORD_USING_FADE_TIME = be<u32>;
static ParamBTL_SWORD_USING_FADE_TIME BTL_SWORD_USING_FADE_TIME;
using ParamJAI_ZEL_NIGHT_FADEOUT_TIME = be<u32>;
static ParamJAI_ZEL_NIGHT_FADEOUT_TIME JAI_ZEL_NIGHT_FADEOUT_TIME;
using ParamENEMY_NEARBY_TRACK = be<u8>;
static ParamENEMY_NEARBY_TRACK ENEMY_NEARBY_TRACK;
using ParamENEMY_NEARBY_TRACK_2 = be<u8>;
static ParamENEMY_NEARBY_TRACK_2 ENEMY_NEARBY_TRACK_2;
using ParamBTL_KAITENGIRI_TRACK = be<u8>;
static ParamBTL_KAITENGIRI_TRACK BTL_KAITENGIRI_TRACK;
using ParamCOMMON_BATTLE_FO_DELAY = be<u8>;
static ParamCOMMON_BATTLE_FO_DELAY COMMON_BATTLE_FO_DELAY;
using ParamMBOSS_ENEMY_NEARBY_TRACK = be<u8>;
static ParamMBOSS_ENEMY_NEARBY_TRACK MBOSS_ENEMY_NEARBY_TRACK;
using ParamMBOSS_BTL_KAITENGIRI_TRACK = be<u8>;
static ParamMBOSS_BTL_KAITENGIRI_TRACK MBOSS_BTL_KAITENGIRI_TRACK;
using ParamMBOSS_SWORD_USING_TRACK = be<u8>;
static ParamMBOSS_SWORD_USING_TRACK MBOSS_SWORD_USING_TRACK;
enum : u32 {
    JAISoundID_Type_Sequence = 0x80000000,
    JAISoundID_Type_Stream = 0xC0000000,
    SCENE_SEA = 0x12,
    ENDLESS_NIGHT = 0xA02,
    MET_KORL = 0xF80,
    UNK_3920 = 0x3920,
    UNK_0E20 = 0xE20,
    JA_SE_ALMOST_DIE_ALERM_1 = 0xD0,
    JA_SE_ALMOST_DIE_ALERM_2 = 0xD1,
    JA_SE_ALMOST_DIE_ALERM_3 = 0xD2,
    JA_SE_ATM_WIND_1 = 0x105A,
    JA_BGM_BATTLE_NORM = 0x80000004,
    JA_BGM_DIE_LINK = 0x8000000A,
    JA_BGM_DK_BATTLE = 0x80000014,
    JA_BGM_MJ_TOWER_BATTLE = 0x80000015,
    JA_BGM_MBOSS = 0x80000019,
    JA_BGM_MBOSS_S = 0x8000001A,
    JA_BGM_SEA_ENEMY = 0x8000001C,
    JA_BGM_MORNING = 0x8000001D,
    JA_BGM_KAMI_TOWER = 0x80000028,
    JA_BGM_DIOCTA_BATTLE = 0x8000002B,
    JA_BGM_SEA = 0x8000002E,
    JA_BGM_SEA_STORM = 0x8000003C,
    JA_BGM_ELF = 0x80000053,
    JA_BGM_ISLAND_LINK_3 = 0x80000055,
    JA_BGM_UNK_110 = 0x80000110,
};
f32 JAIZelBasic::calcMainBgmVolWithSeaFactor(f32 seaFactor) {
    f32 v = gabi::fmuls_ppc(mMainBgmMuteVolume, mMainBgmTalkVolume);
    v = gabi::fmuls_ppc(v, mMainBgmMenuVolume);
    v = gabi::fmuls_ppc(v, mMainBgmSubBgmVolume);
    v = gabi::fmuls_ppc(v, mMainBgmSceneVolume);
    v = gabi::fmuls_ppc(v, seaFactor);
    v = gabi::fmuls_ppc(v, mMainBgmTaktVolume);
    v = gabi::fmuls_ppc(v, mMainBgmCbPracticeVolume);
    return gabi::fmuls_ppc(v, mMainBgmHeightVolume);
}

/* 020233C8 */
s32 JAIZelBasic::checkPlayingStreamBgmFlag(u32 id) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return hd_checkStream(hd_streamPlayer(l_soundManager.get()), id, 0, 0, 0);
}

/* 02023410 */
u32 JAIZelBasic::checkSeaBgmID() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (checkEventBit(ENDLESS_NIGHT) == 1 && checkEventBit(UNK_3920) == 0)
        return JA_BGM_SEA_STORM;
    return JA_BGM_SEA;
}

/* 02023474 */
s32 JAIZelBasic::checkDayTime() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return (u32)mHour - 6 < 14;
}

/* 0202348C */
u32 JAIZelBasic::checkPlayingMainBgmFlag() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    auto *m = mpMainBgmSound.get();
    if (m == 0)
        return (u32)-1;
    return (m)->getID();
}

/* 020234A4 */
void JAIZelBasic::sceneBgmStart() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mSceneBgmStartedOnce = 1;
    bool night = false;
    if ((s32)mSceneNo == SCENE_SEA && checkDayTime() == 0) {
        mSceneBgmChanged = 0;
        night = true;
    }
    if (mMorningBgmPlaying == 1)
        return;
    if (checkPlayingMainBgmFlag() == JA_BGM_DIE_LINK)
        return;
    u32 skip = mSceneBgmChanged;
    mForceBack = 0;
    mSceneActive = 1;
    mBattleBgmDisabled = 0;
    if (skip == 0) {
        u32 sc = mSceneNo;
        bool ok;
        if (sc >= 0x19)
            ok = sc == 0x19 || (sc >= 0x2E && sc <= 0x34) || sc == 0x38 ||
                 (sc >= 0x69 && sc <= 0x6A);
        else if (sc < 5)
            return;
        else
            ok = sc <= 6 || sc == 0xA || (sc >= 0x14 && sc <= 0x15);
        if (!ok) {
            if (sc >= 0x19 || sc < 0x14)
                return;
            return; /* 0x16..0x18: return without the flag reset */
        }
    }
    u32 bgm = mNextSceneBgmId;
    if (bgm != 0 && bgm != JAISoundID_Type_Sequence && !night)
        bgmStart(bgm, 0, 0);
    mSceneBgmChanged = 0;
    mDayNightSwitched = 0;
}

/* 02023608 */
u32 JAIZelBasic::expandSceneBgmNum(u32 n) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if ((n & 0xF000) == 0x8000)
        return (n & 0x7FFF) | JAISoundID_Type_Stream;
    return (n & 0x7FFF) | JAISoundID_Type_Sequence;
}

/* 02023628 */
void JAIZelBasic::mainBgmStopOnly(u32 fade) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    auto *m = mpMainBgmSound.get();
    if (m != 0)
        (m)->stop(fade);
    mpMainBgmSound = nullptr;
    mMainBgmNum = (u32)-1;
}

/* 02023670 */
s32 JAIZelBasic::checkOnOuterSea(be<f32> *out) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (out == 0)
        return -1;
    *out = 0.0f;
    if ((s32)mSceneNo != SCENE_SEA)
        return -1;
    u32 room = mIslandRoomNo;
    if (room == 0)
        return 4;
    auto *area = &l_isleAreas.entries[room];
    f32 ax = f32(area->centerX);
    f32 az = f32(area->centerZ);
    f32 ar = f32(area->radius);
    if (ax > 50000.0f)
        return 4;
    s32 m = (s32)room - 1;
    s32 q = m / 7;
    s32 col = m - q * 7;
    f32 fz = (f32)(q - 3);
    f32 fx = (f32)(col - 3);
    u32 cam = cameraPositionEA();
    f32 cz = cameraZ(cam);
    f32 cx = cameraX(cam);
    // HD: fused grid subtraction and squared distance, including NaN behavior.
    f32 rz = gabi::fnmsubs(fz, 100000.0f, cz);
    f32 rx = gabi::fnmsubs(fx, 100000.0f, cx);
    rz = az - rz;
    rx = ax - rx;
    f32 d2 = gabi::fmadds(rx, rx, rz * rz);
    *out = d2;
    f32 d = std_sqrtf(d2);
    *out = d;
    if (!(d > ar))
        return 0;
    if (!(d > ar + 2000.0f))
        return 1;
    if (!(d > ar + 3000.0f))
        return 2;
    if (!(d > ar + 4000.0f))
        return 3;
    return 4;
}

/* 0202389C */
void JAIZelBasic::startIsleBgm() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (checkDayTime() == 0)
        return;
    u32 room = mIslandRoomNo;
    if (room == 0x2C && checkEventBit(UNK_0E20) == 1) {
        bgmStart(JA_BGM_ISLAND_LINK_3, 0, 1);
        return;
    }
    // HD: room is retained across the event check.
    bgmStart(expandSceneBgmNum(u16(l_isleInfo.entries[room].bgmNum)), 0, 1);
}

/* 02023930 */
void JAIZelBasic::processMorningToNormal() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mMainBgmNum != JA_BGM_MORNING) {
        mMorningBgmPlaying = 0;
        return;
    }
    if (mpMainBgmSound.get() != 0)
        return;
    mMainBgmSeaVolume = 1.0f;
    gabi::Local<be<f32>> tmp;
    if (checkOnOuterSea(tmp.get()) >= 2) {
        bgmStart(JA_BGM_SEA, 0, 1);
        mMorningBgmPlaying = 0;
    } else {
        startIsleBgm();
        mMorningBgmPlaying = 0;
    }
}

/* 020239C8 */
void JAIZelBasic::processTime() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u16 t = (u16)(((mHour << 8) & 0xFF00) + mMinute);
    if ((s32)mSceneNo == SCENE_SEA && mSceneActive == 1) {
        u32 old = mPrevTime;
        if (old < 0x1400) {
            if (t >= 0x1400) {
                mainBgmStopOnly(JAI_ZEL_NIGHT_FADEOUT_TIME);
                mPrevTime = t;
                processMorningToNormal();
                return;
            }
            if (old < 0x53B && t >= 0x53B) {
                bgmStart(JA_BGM_MORNING, 0, 1);
                mMorningBgmPlaying = 1;
            }
        }
    }
    mPrevTime = t;
    processMorningToNormal();
}

/* 02023A94 */
void JAIZelBasic::changeSeaBgm() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if ((s32)mSceneNo != SCENE_SEA)
        return;
    if (mIslandRoomNo == 0)
        return;
    u32 mainBgmBeforeEvent = mMainBgmNum; // HD: retained across the event check.
    if (mainBgmBeforeEvent == JA_BGM_DIE_LINK)
        return;
    if (checkEventBit(MET_KORL) == 0)
        return;
    if (cameraPositionEA() == 0)
        return;
    if (mSceneActive == 0)
        return;
    if (mNoCameraTimer != 0)
        return;
    if (mbLandingDemoStarted != 0)
        return;
    if (mKamomeFlag != 0)
        return;
    if (checkDayTime() == 0 && checkSeaBgmID() != JA_BGM_SEA_STORM)
        return;
    if (mMorningBgmPlaying == 1)
        return;
    if (isDemo() == 1)
        return;
    if (mpSubBgmSound.get() != 0 && mSubBgmNum == JA_BGM_DIOCTA_BATTLE)
        return;
    if (mpMainBgmSound.get() != 0 && mainBgmBeforeEvent == JA_BGM_ELF)
        return;
    if (mSceneBgmLocked != 0)
        return;
    gabi::Local<be<f32>> tmp;
    be<f32> *distance = tmp.get();
    *distance = 0.0f;
    u32 r = checkOnOuterSea(distance);
    u32 room = mIslandRoomNo;
    u32 n = mMainBgmNum;
    f32 ar = f32(l_isleAreas.entries[room].radius);
    if (n == JA_BGM_SEA || n == JA_BGM_SEA_STORM) {
        if (r == 1) {
            mMainBgmSeaVolume = 0.0f;
            startIsleBgm();
        } else if (r == 2) {
            f32 v = (f32(*distance) - (ar + 2000.0f)) / 1000.0f;
            auto *m = mpMainBgmSound.get();
            mMainBgmSeaVolume = v;
            if (m != 0)
                (m)->setVolume(calcMainBgmVolWithSeaFactor(v), 1, 0);
        }
        return;
    }
    if (r < 1)
        return;
    if (r == 1) {
        f32 v = ((ar + 2000.0f) - f32(*distance)) / 2000.0f;
        auto *m = mpMainBgmSound.get();
        mMainBgmSeaVolume = v;
        if (m != 0)
            (m)->setVolume(calcMainBgmVolWithSeaFactor(v), 1, 0);
    } else if (r < 4) {
        auto *m = mpMainBgmSound.get();
        mMainBgmSeaVolume = 0.0f;
        if (m != 0)
            (m)->setVolume(calcMainBgmVolWithSeaFactor(0.0f), 0, 0);
    } else if (r == 4) {
        mMainBgmSeaVolume = 1.0f;
        bgmStart(checkSeaBgmID(), 0x5A, 1);
    }
}

/* 02023E0C */
void JAIZelBasic::bgmBattleGFrame() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 n = mSubBgmNum;
    if (n != JA_BGM_BATTLE_NORM && n != JA_BGM_SEA_ENEMY)
        return;
    u32 d0 = mBattleBgmFadeDelay;
    if (d0 != COMMON_BATTLE_FO_DELAY && d0 == 0) {
        auto *m = mpMainBgmSound.get();
        mMainBgmSubBgmVolume = 1.0f;
        if (m != 0) {
            f32 v = gabi::fmuls_ppc(mMainBgmMuteVolume, mMainBgmTalkVolume);
            v = gabi::fmuls_ppc(v, mMainBgmMenuVolume);
            v = gabi::fmuls_ppc(v, mMainBgmSceneVolume);
            v = gabi::fmuls_ppc(v, mMainBgmSeaVolume);
            v = gabi::fmuls_ppc(v, mMainBgmTaktVolume);
            v = gabi::fmuls_ppc(v, mMainBgmCbPracticeVolume);
            v = gabi::fmuls_ppc(v, mMainBgmHeightVolume);
            (m)->setVolume(v, 0x2D, 0);
        }
        stopBattleBgm();
        mEnemyNearbyFadeTimer = 0;
        mEnemyNearbyOn = 0;
        return;
    }
    if (n == JA_BGM_BATTLE_NORM) {
        u32 c = mEnemyNearbyFadeTimer;
        bool fade = true;
        u32 d8;
        if (c > 1) {
            d8 = mKaitengiriTimer;
            mEnemyNearbyFadeTimer = c - 1;
        } else {
            if (c == 1) {
                auto *sub = mpSubBgmSound.get();
                if (sub != 0) {
                    (sub)->setTrackVolume(ENEMY_NEARBY_TRACK, 0.0f, ENEMY_NEARBY_FADEOUT_TIME);
                    (mpSubBgmSound.get())
                        ->setTrackVolume(ENEMY_NEARBY_TRACK_2, 0.0f, ENEMY_NEARBY_FADEOUT_TIME);
                }
                mEnemyNearbyFadeTimer = 0;
            }
            d8 = mKaitengiriTimer;
        }
        if (d8 != 0) {
            d0 = mBattleBgmFadeDelay;
            mKaitengiriTimer = d8 - 1;
            fade = false;
        }
        if (fade) {
            auto *sub = mpSubBgmSound.get();
            if (sub != 0 && mSubBgmNum == JA_BGM_BATTLE_NORM)
                (sub)->setTrackVolume(BTL_KAITENGIRI_TRACK, 0.0f, BTL_KAITENGIRI_FADEOUT_TIME);
            d0 = mBattleBgmFadeDelay;
        }
    }
    if (d0 != 0)
        mBattleBgmFadeDelay = d0 - 1;
}

/* 02024018 */
void JAIZelBasic::mbossBgmMuteProcess() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 n = mSubBgmNum;
    if (n != JA_BGM_MBOSS && n != JA_BGM_MBOSS_S)
        return;
    u32 c = mEnemyNearbyFadeTimer;
    Sound *sub;
    if (c > 1) {
        sub = mpSubBgmSound.get();
        mEnemyNearbyFadeTimer = c - 1;
    } else {
        if (c == 1) {
            auto *s = mpSubBgmSound.get();
            if (s != 0 && (n == JA_BGM_MBOSS || n == JA_BGM_MBOSS_S))
                (s)->setTrackVolume(MBOSS_ENEMY_NEARBY_TRACK, 0.0f, ENEMY_NEARBY_FADEOUT_TIME);
            mEnemyNearbyFadeTimer = 0;
        }
        sub = mpSubBgmSound.get();
    }
    if (sub != 0) {
        if (mSwordUsing == 0)
            (sub)->setTrackVolume(MBOSS_SWORD_USING_TRACK, 0.0f, BTL_SWORD_USING_FADE_TIME);
        else
            (sub)->setTrackVolume(MBOSS_SWORD_USING_TRACK, 1.0f, BTL_SWORD_USING_FADE_TIME);
    }
    u32 d8 = mKaitengiriTimer;
    if (d8 != 0) {
        mKaitengiriTimer = d8 - 1;
        return;
    }
    auto *s = mpSubBgmSound.get();
    if (s != 0)
        (s)->setTrackVolume(MBOSS_BTL_KAITENGIRI_TRACK, 0.0f, BTL_KAITENGIRI_FADEOUT_TIME);
}

/* 02024190 */
void JAIZelBasic::enemyNearByGFrame() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if ((s32)mSceneNo != 9)
        return;
    s32 c = (s8)mEnemyNearbyDelay;
    if (c < 0)
        return;
    if (c > 0) {
        mEnemyNearbyDelay = c - 1;
        return;
    }
    if (mpMainBgmSound.get() != 0) {
        u32 n = mMainBgmNum;
        bool go = true;
        if (n == JA_BGM_DK_BATTLE) {
            bgmStart(mDkMaskBroken != 0 ? JA_BGM_UNK_110 : JA_BGM_DK_BATTLE, 0, 0);
            if (mpMainBgmSound.get() == 0)
                go = false;
            else
                n = mMainBgmNum;
        }
        if (go && n == JA_BGM_MJ_TOWER_BATTLE)
            bgmStart(JA_BGM_MJ_TOWER_BATTLE, 0, 0);
    }
    mEnemyNearbyDelay = 0xFF;
}

/* 02024278 */
void JAIZelBasic::stSkyCloistersProcess() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 c = mSkyCloistersTimer;
    if (c == 0)
        return;
    bool expire = c == 1;
    if (c > 1) {
        c = (c - 1) & 0xFF;
        mSkyCloistersTimer = c;
        expire = c == 1;
    }
    if (expire) {
        if (mMainBgmNum == JA_BGM_KAMI_TOWER)
            bgmStart(JA_BGM_KAMI_TOWER, 1, 0);
        mSkyCloistersTimer = 0;
    }
    seStart(JA_SE_ATM_WIND_1, 0, 0, 0, 1.0f, 1.0f, -1.0f, -1.0f, 0);
}

/* 02024334 */
// HD: the save option can disable the low-health alarm.
void JAIZelBasic::processHeartGaugeSound() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (dComIfGp_event_runCheck())
        return; /* dComIfGp_event_runCheck() */
    u32 c = mHeartGaugeTimer;
    if (c == 0)
        return;
    if (mLinkHp != 0 && mFanfareActive == 0) {
        void *opt = hd_getOptions(optionsAddress(l_save.get()));
        if (hd_heartAlarmDisabled(opt) == 0) {
            s32 h = mLinkHp;
            if (h <= 2)
                seStart(JA_SE_ALMOST_DIE_ALERM_3, 0, 0, 0, 1.0f, 1.0f, -1.0f, -1.0f, 0);
            else if (h <= 4)
                seStart(JA_SE_ALMOST_DIE_ALERM_2, 0, 0, 0, 1.0f, 1.0f, -1.0f, -1.0f, 0);
            else if (h <= 6)
                seStart(JA_SE_ALMOST_DIE_ALERM_1, 0, 0, 0, 1.0f, 1.0f, -1.0f, -1.0f, 0);
        }
        c = mHeartGaugeTimer;
    }
    mHeartGaugeTimer = c - 1;
}

// clang-format off
/* WWHD glue:
data l_isleAreas 1018E8B4
data l_isleInfo 1018E60C
data l_soundManager 1018EC64
data l_save 101F84DC
data ENEMY_NEARBY_FADEOUT_TIME 1018DC80
data BTL_KAITENGIRI_FADEOUT_TIME 1018DC88
data BTL_SWORD_USING_FADE_TIME 1018DC8C
data JAI_ZEL_NIGHT_FADEOUT_TIME 1018DC90
data ENEMY_NEARBY_TRACK 1018DCAF
data ENEMY_NEARBY_TRACK_2 1018DCB0
data BTL_KAITENGIRI_TRACK 1018DCB1
data COMMON_BATTLE_FO_DELAY 1018DCB4
data MBOSS_ENEMY_NEARBY_TRACK 1018DCB5
data MBOSS_BTL_KAITENGIRI_TRACK 1018DCB6
data MBOSS_SWORD_USING_TRACK 1018DCB7
ext 0201E480 s32 jaizel5::JAIZelBasic::checkEventBit(u32)
ext 0201E5CC s32 jaizel5::JAIZelBasic::isDemo()
ext 0201D84C void jaizel5::JAIZelBasic::stopBattleBgm()
ext 0202204C void jaizel5::JAIZelBasic::bgmStart(u32, u32, u32)
ext 0201EBA0 u32 jaizel5::JAIZelBasic::seStart(u32, u32, u32, s32, f32, f32, f32, f32, u32)
ext 0280B1D8 void jaizel5::Sound::stop(u32)
ext 0280B68C void jaizel5::Sound::setVolume(f32, u32, u32)
ext 0280C9E4 void jaizel5::Sound::setTrackVolume(u32, f32, u32)
ext 0280D2D8 u32 JAISound::getID()
ext 0202BF78 void* hd_streamPlayer(void*)
ext 0202FD40 s32 hd_checkStream(void*, u32, u32, u32, u32)
ext 027200D0 void* hd_getOptions(void*)
ext 0271FC5C s32 hd_heartAlarmDisabled(void*)
*/
