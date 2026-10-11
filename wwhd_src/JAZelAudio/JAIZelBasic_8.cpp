/** JAIZelBasic dialogue, battle and positional voice/ship sound control.
 * Verified HD implementation with tww names. */
#include "JAZelAudio/JAIZelBasic_8.h"
using namespace jaizel8;
namespace {
constexpr u32 JA_SE_SHIP_GO_NORMAL = 0x3004;
constexpr u32 JA_SE_SHIP_WAVE_L = 0x3005;
constexpr u32 JA_SE_SHIP_WAVE_S = 0x3006;
constexpr u32 JA_SE_CV_COMMON_PEOPLE = 0x481F;
constexpr u32 JA_SE_CV_KS_ATTACK = 0x486D;
constexpr u32 JA_SE_CV_PW_RUN = 0x48F1;
constexpr u32 JA_SE_CV_PW_DIE = 0x48F6;
constexpr u32 JA_SE_CV_PT_ATTACK = 0x48F7;
constexpr u32 JA_SE_CV_PW_CURSE_IN = 0x48FD;
constexpr u32 JA_BGM_GOMA = 0x80000003;
constexpr u32 JA_BGM_BATTLE_NORM = 0x80000004;
constexpr u32 JA_BGM_KINDAN_BOSS = 0x80000005;
constexpr u32 JA_BGM_DIE_LINK = 0x8000000A;
constexpr u32 JA_BGM_SEA_GAME = 0x8000000B;
constexpr u32 JA_BGM_GOMA_2 = 0x80000010;
constexpr u32 JA_BGM_MBOSS = 0x80000019;
constexpr u32 JA_BGM_MBOSS_S = 0x8000001A;
constexpr u32 JA_BGM_SEA_ENEMY = 0x8000001C;
constexpr u32 JA_BGM_TAKT_MAKORE = 0x80000021;
constexpr u32 JA_BGM_TAKT_MEDRI = 0x80000022;
constexpr u32 JA_BGM_KAMI_TOWER = 0x80000028;
constexpr u32 JA_BGM_DIOCTA_BATTLE = 0x8000002B;
constexpr u32 JA_BGM_SEA = 0x8000002E;
constexpr u32 JA_BGM_TAKT_KAZE = 0x80000031;
constexpr u32 JA_BGM_CB_NEXTYEAR = 0x80000036;
constexpr u32 JA_BGM_CB_NEXTYEAR_W = 0x80000037;
constexpr u32 JA_BGM_ISLAND_WOOD = 0x80000039;
constexpr u32 JA_BGM_ISLAND_DRAGON = 0x8000003A;
constexpr u32 JA_BGM_SEA_STORM = 0x8000003C;
constexpr u32 JA_BGM_P_GANON_1 = 0x80000041;
constexpr u32 JA_BGM_DIOCTA_2 = 0x80000046;
constexpr u32 JA_BGM_P_GANON_2 = 0x80000047;
constexpr u32 JA_BGM_BIRDMAN_GOAL = 0x80000051;
constexpr u32 JA_BGM_BIRDMAN_FAIL = 0x80000052;
constexpr u32 JA_BGM_UNK_105 = 0x80000105;
constexpr u32 JA_BGM_UNK_125 = 0x80000125;
constexpr u32 JA_BGM_UNK_126 = 0x80000126;
constexpr u32 JA_STRM_PROLOGUE = 0xC0000000;
constexpr u32 JA_STRM_DEMO_TETRA_FLY = 0xC0000001;
constexpr u32 JA_STRM_DEMO_GO_MAJU = 0xC0000005;
} // namespace
namespace {
struct CategoryVolumes {
    GuestByte entries[8];
};
static GuestFloat VOL_BGM_TALKING;
static GuestFloat ENEMY_NEARBY_DIST;
static GuestWord ENEMY_NEARBY_FADEIN_TIME;
static GuestWord ENEMY_NEARBY_FADEOUT_TIME;
static GuestWord BTL_KAITENGIRI_FADEIN_TIME;
static GuestWord BTL_SWORD_USING_FADE_TIME;
static GuestByte ENEMY_NEARBY_TRACK;
static GuestByte ENEMY_NEARBY_TRACK_2;
static GuestByte BTL_KAITENGIRI_TRACK;
static GuestByte SWORD_USING_TRACK;
static GuestByte SWORD_NOT_USING_TRACK;
static GuestByte COMMON_BATTLE_FO_DELAY;
static GuestByte MBOSS_BTL_KAITENGIRI_TRACK;
static GuestWord jai_rng_seed;
static GuestFloat l_zero;
static CategoryVolumes l_talkingCategoryVolumes;
static VectorWords l_practicePosition;
static VectorWords l_shipPosition;
static MessageVoiceTable messageSeTable;
static LinkVoiceTable linkVoiceTable;
static HeaderStatics l_headerStatics;
static FloatRange l_headerRange;
static DestructionRecord l_headerRecord;
static DestructionRecord l_byte0Record;
static DestructionRecord l_byte1Record;
} // namespace

// HD: dialogue fades over15frames instead of the GameCube2.
/* 02027D4C */
void JAIZelBasic::talkIn() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (field_0x0064 == 1)
        return;
    if (isDemo() == 1)
        return;
    if (field_0x0098 == 0.0f)
        return;
    Sound *m = mpMainBgmSound.get();
    f32 t = VOL_BGM_TALKING; /* JAIZelParam::VOL_BGM_TALKING */
    field_0x0084 = t;
    if (m != 0 && mMainBgmNum != JA_BGM_BIRDMAN_GOAL && mMainBgmNum != JA_BGM_BIRDMAN_FAIL) {
        f32 v = gabi::fmuls_ppc(field_0x0080, t);
        v = gabi::fmuls_ppc(v, field_0x0088);
        v = gabi::fmuls_ppc(v, field_0x008c);
        v = gabi::fmuls_ppc(v, field_0x0090);
        v = gabi::fmuls_ppc(v, field_0x0094);
        v = gabi::fmuls_ppc(v, field_0x0098);
        v = gabi::fmuls_ppc(v, field_0x009c);
        v = gabi::fmuls_ppc(v, field_0x00ac);
        m->setVolume(v, 0xF, 0);
    }
    Sound *sub = mpSubBgmSound.get();
    f32 t2 = VOL_BGM_TALKING;
    field_0x00a0 = t2;
    if (sub != 0)
        sub->setSeqInterVolume(0, gabi::fmuls_ppc(gabi::fmuls_ppc(t2, field_0x00a4), field_0x00a8),
                               0xF);
    for (u32 c = 0; c < 8; c++)
        setSeCategoryVolume(c, l_talkingCategoryVolumes.entries[c]);
}

/* 02027F04 */
void JAIZelBasic::bgmStreamPlay() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (field_0x0063 != 0)
        return;
    Sound *s = mpStreamBgmSound.get();
    if (s != 0)
        s->start(0); /* JAISound::start */
    u32 n = mStreamBgmNum;
    if (n == JA_STRM_DEMO_TETRA_FLY) {
        Sound *m = mpMainBgmSound.get();
        field_0x00ba = 1;
        if (m != 0)
            m->stop(0x1E);
        field_0x1f3c = 0;
        field_0x00b8 = 1;
        field_0x0201 = 1;
    } else if (n == JA_STRM_PROLOGUE) {
        Sound *m = mpMainBgmSound.get();
        if (m != 0)
            m->stop(0x1E);
        field_0x1f3c = 0;
        field_0x00b8 = 1;
    } else {
        field_0x00b8 = 1;
    }
}

/* 02027FD4 */
void JAIZelBasic::changeBgmStatus(u32 v) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    Sound *m = mpMainBgmSound.get();
    if (m != 0)
        m->setPortData(9, v & 0xFFFF);
}

/* 02027FF0 */
void JAIZelBasic::changeSubBgmStatus(u32 v) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    Sound *sub = mpSubBgmSound.get();
    if (sub == 0)
        return;
    rootTrack(sub->getSeqParameter())->writePortApp(0x00090000, v & 0xFFFF);
}

// HD: this scene selects the alternate practice tune; scene27 reduces main-BGM ducking.
/* 02028038 */
void JAIZelBasic::cbPracticePlay(VectorWords *pos) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    field_0x00d0 = pos;
    void *matrix = cameraMatrix();
    gabi::Local<VectorWords> position;
    position->w[0] = l_practicePosition.w[0];
    position->w[1] = l_practicePosition.w[1];
    position->w[2] = l_practicePosition.w[2];
    VectorWords *source = pos != nullptr ? pos : cameraPosition();
    if (source != nullptr) {
        position->w[0] = source->w[0];
        position->w[1] = source->w[1];
        position->w[2] = source->w[2];
    }
    if (matrix != nullptr)
        jaizelTransform(matrix, position.get(), position.get());

    u32 id = (s32)field_0x0224 == 0x39 ? JA_BGM_CB_NEXTYEAR_W : JA_BGM_CB_NEXTYEAR;
    startSoundVec(id, &mpSubBgmSound, 0, 0, 0, 4);
    Sound *sub = mpSubBgmSound.get();
    mSubBgmNum = id;
    if (sub != 0)
        sub->setSeqInterVolume(0, field_0x00a0, 0);
    f32 vol = calcPosVolume(position.get(), 2.0f);
    f32 lr = calcPosPanLR(position.get());
    f32 sr = calcPosPanSR(position.get(), 1.0f);
    sub = mpSubBgmSound.get();
    field_0x00a8 = vol;
    if (sub != 0) {
        sub->setVolume(gabi::fmuls_ppc(gabi::fmuls_ppc(field_0x00a0, field_0x00a4), vol), 0, 0);
        mpSubBgmSound.get()->setPan(lr, 0, 0);
        mpSubBgmSound.get()->setDolby(sr, 0, 0);
    }
    s32 sc = field_0x0224;
    Sound *m = mpMainBgmSound.get();
    f32 duck = sc == 0x27 ? gabi::fnmsubs(vol, 0.4f, 1.0f) : gabi::fsubs_ppc(1.0f, vol);
    field_0x009c = duck;
    if (m != 0) {
        f32 t = gabi::fmuls_ppc(field_0x0080, field_0x0084);
        t = gabi::fmuls_ppc(t, field_0x0088);
        t = gabi::fmuls_ppc(t, field_0x008c);
        t = gabi::fmuls_ppc(t, field_0x0090);
        t = gabi::fmuls_ppc(t, field_0x0094);
        t = gabi::fmuls_ppc(t, field_0x0098);
        t = gabi::fmuls_ppc(t, duck);
        t = gabi::fmuls_ppc(t, field_0x00ac);
        m->setVolume(t, 0x1E, 0);
    }
}

/* 020282F8 */
void JAIZelBasic::cbPracticeStop() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    Sound *sub = mpSubBgmSound.get();
    if (sub != 0) {
        u32 n = mSubBgmNum;
        if (n == JA_BGM_CB_NEXTYEAR || n == JA_BGM_CB_NEXTYEAR_W) {
            sub->stop(0xF);
            mpSubBgmSound = nullptr;
            mSubBgmNum = (u32)-1;
        }
    }
    Sound *m = mpMainBgmSound.get();
    field_0x00a8 = 1.0f;
    field_0x009c = 1.0f;
    if (m != 0) {
        f32 t = gabi::fmuls_ppc(field_0x0080, field_0x0084);
        t = gabi::fmuls_ppc(t, field_0x0088);
        t = gabi::fmuls_ppc(t, field_0x008c);
        t = gabi::fmuls_ppc(t, field_0x0090);
        t = gabi::fmuls_ppc(t, field_0x0094);
        t = gabi::fmuls_ppc(t, field_0x0098);
        t = gabi::fmuls_ppc(t, field_0x00ac);
        m->setVolume(t, 0x5A, 0);
    }
    field_0x00d0 = nullptr;
}

/* 020283D8 */
void JAIZelBasic::bgmMuteMtDragon() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    field_0x0065 = 1;
}

/* 020283E4 */
void JAIZelBasic::stSkyCloisters() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mMainBgmNum != JA_BGM_KAMI_TOWER)
        return;
    if (field_0x00b7 == 0)
        bgmStart(JA_BGM_UNK_126, 1, 0);
    field_0x00b7 = 4;
}

/* 0202844C */
void JAIZelBasic::bgmAllMute(u32 fade) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    Sound *m = mpMainBgmSound.get();
    field_0x0080 = 0.0f;
    if (m == 0)
        return;
    f32 v = gabi::fmuls_ppc(0.0f, field_0x0084);
    v = gabi::fmuls_ppc(v, field_0x0088);
    v = gabi::fmuls_ppc(v, field_0x008c);
    v = gabi::fmuls_ppc(v, field_0x0090);
    v = gabi::fmuls_ppc(v, field_0x0094);
    v = gabi::fmuls_ppc(v, field_0x0098);
    v = gabi::fmuls_ppc(v, field_0x009c);
    v = gabi::fmuls_ppc(v, field_0x00ac);
    m->setVolume(v, fade, 0);
}

/* 020284B0 */
void JAIZelBasic::stWaterLevelUp() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (field_0x00b7 != 0)
        return;
    if (mMainBgmNum != JA_BGM_KAMI_TOWER)
        return;
    bgmStart(JA_BGM_UNK_125, 1, 0);
}

/* 020284E4 */
void JAIZelBasic::stWaterLevelDown() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (field_0x00b7 != 0)
        return;
    if (mMainBgmNum != JA_BGM_KAMI_TOWER)
        return;
    bgmStart(JA_BGM_KAMI_TOWER, 1, 0);
}

/* 02028518 */
void JAIZelBasic::taktModeMute() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    Sound *m = mpMainBgmSound.get();
    field_0x0098 = 0.0f;
    if (m != 0) {
        u32 n = mMainBgmNum;
        if (n == JA_BGM_TAKT_MAKORE || n == JA_BGM_TAKT_MEDRI) {
            field_0x0098 = 1.0f;
        } else {
            f32 v = gabi::fmuls_ppc(field_0x0080, field_0x0084);
            v = gabi::fmuls_ppc(v, field_0x0088);
            v = gabi::fmuls_ppc(v, field_0x008c);
            v = gabi::fmuls_ppc(v, field_0x0090);
            v = gabi::fmuls_ppc(v, field_0x0094);
            v = gabi::fmuls_ppc(v, 0.0f);
            v = gabi::fmuls_ppc(v, field_0x009c);
            v = gabi::fmuls_ppc(v, field_0x00ac);
            m->setVolume(v, 0xA, 0);
        }
    }
    Sound *sub = mpSubBgmSound.get();
    field_0x00a4 = 0.0f;
    if (sub != 0)
        sub->setVolume(gabi::fmuls_ppc(gabi::fmuls_ppc(field_0x00a0, 0.0f), field_0x00a8), 0xA, 0);
}

/* 0202862C */
void JAIZelBasic::taktModeMuteOff() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    Sound *m = mpMainBgmSound.get();
    field_0x0098 = 1.0f;
    if (m != 0) {
        f32 v = gabi::fmuls_ppc(field_0x0080, field_0x0084);
        v = gabi::fmuls_ppc(v, field_0x0088);
        v = gabi::fmuls_ppc(v, field_0x008c);
        v = gabi::fmuls_ppc(v, field_0x0090);
        v = gabi::fmuls_ppc(v, field_0x0094);
        v = gabi::fmuls_ppc(v, field_0x009c);
        v = gabi::fmuls_ppc(v, field_0x00ac);
        m->setVolume(v, 0xA, 0);
    }
    f32 c = field_0x008c;
    if (!(c == 0.0f))
        hdStreamVolume(0xA, gabi::fmuls_ppc(gabi::fmuls_ppc(field_0x0084, c), field_0x0098));
    if (mSubBgmNum == JA_BGM_TAKT_KAZE && mLastMinibossSubBGMType == 0) {
        Sound *m2 = mpMainBgmSound.get();
        field_0x008c = 1.0f;
        if (m2 != 0) {
            f32 v = gabi::fmuls_ppc(field_0x0080, field_0x0084);
            v = gabi::fmuls_ppc(v, field_0x0088);
            v = gabi::fmuls_ppc(v, field_0x0090);
            v = gabi::fmuls_ppc(v, field_0x0094);
            v = gabi::fmuls_ppc(v, field_0x0098);
            v = gabi::fmuls_ppc(v, field_0x009c);
            v = gabi::fmuls_ppc(v, field_0x00ac);
            m2->setVolume(v, 0x2D, 0);
        }
    }
    Sound *sub = mpSubBgmSound.get();
    field_0x00a4 = 1.0f;
    if (sub != 0)
        sub->setVolume(gabi::fmuls_ppc(field_0x00a0, field_0x00a8), 0xA, 0);
}

// HD: sea battles use their own BGM and distance-dependent enemy tracks.
/* 0202879C */
void JAIZelBasic::bgmNowBattle(f32 dist) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (field_0x0063 != 0)
        return;
    if (field_0x00ca == 1)
        return;
    if (field_0x0098 == 0.0f)
        return;
    if (field_0x022d == 0)
        return;
    if (audioPlayerPresent() && dComIfGp_event_runCheck())
        return;
    u32 sc = field_0x0224;
    if (sc < 0x24) {
        if (sc == 2 || sc == 7 || sc == 0x18)
            return;
        if (sc == 9) {
            enemyNearBy();
            return;
        }
    } else if (sc < 0x3A) {
        if (sc <= 0x25 || sc == 0x28 || sc == 0x2D || sc == 0x37)
            return;
    } else {
        if (sc <= 0x3B)
            return;
        if (sc == 0x40) {
            enemyNearBy();
            return;
        }
        if (sc >= 0x5B && (sc <= 0x5C || sc == 0x69))
            return;
    }
    if (mSomeSpecialBGMFlag == 1)
        return;
    if (field_0x0206 == 1)
        return;
    u32 n88 = mMainBgmNum;
    if (n88 < JA_BGM_DIE_LINK) {
        if (n88 == JA_BGM_KINDAN_BOSS || n88 == JA_BGM_GOMA)
            return;
    } else if (n88 == JA_BGM_UNK_105 || n88 == JA_BGM_GOMA_2 || n88 == JA_BGM_DIE_LINK) {
        return;
    }
    if (field_0x0207 != 0)
        return;
    Sound *sub = mpSubBgmSound.get();
    if (sub != 0 && (mSubBgmNum == JA_BGM_DIOCTA_BATTLE || mSubBgmNum == JA_BGM_DIOCTA_2))
        return;
    if (n88 == JA_BGM_TAKT_MAKORE || n88 == JA_BGM_TAKT_MEDRI)
        return;
    if (sub != 0 && (mSubBgmNum == JA_BGM_P_GANON_1 || mSubBgmNum == JA_BGM_P_GANON_2))
        return;
    if (field_0x00ce != 0)
        return;
    u32 n84 = mSubBgmNum;
    if (n84 == JA_BGM_MBOSS || n84 == JA_BGM_MBOSS_S) {
        mbossBgmNearByProcess(dist);
        return;
    }
    if (field_0x00bb != 0)
        return;
    if (mLastMinibossSubBGMType != 0)
        return;
    sub = mpSubBgmSound.get();
    field_0x00c0 = COMMON_BATTLE_FO_DELAY;
    bool start = true;
    if (sub != 0) {
        n84 = mSubBgmNum;
        if (n84 == JA_BGM_BATTLE_NORM)
            start = false;
        else if (n84 == JA_BGM_SEA_ENEMY)
            return;
    }
    if (start) {
        u32 id = checkLinkOnSea() == 1 ? JA_BGM_SEA_ENEMY : JA_BGM_BATTLE_NORM;
        startSoundVec(id, &mpSubBgmSound, 0, 0x3C, 0, 4);
        Sound *s = mpSubBgmSound.get();
        mSubBgmNum = id;
        if (s != 0)
            s->setSeqInterVolume(0, field_0x00a0, 0);
        Sound *m = mpMainBgmSound.get();
        field_0x008c = 0.0f;
        if (m != 0) {
            f32 v = gabi::fmuls_ppc(field_0x0080, field_0x0084);
            v = gabi::fmuls_ppc(v, field_0x0088);
            v = gabi::fmuls_ppc(v, 0.0f);
            v = gabi::fmuls_ppc(v, field_0x0090);
            v = gabi::fmuls_ppc(v, field_0x0094);
            v = gabi::fmuls_ppc(v, field_0x0098);
            v = gabi::fmuls_ppc(v, field_0x009c);
            v = gabi::fmuls_ppc(v, field_0x00ac);
            m->setVolume(v, 0x3C, 0);
        }
        if (checkLinkOnSea() == 1) {
            s32 on = checkLinkOnBoardSea();
            bgmMute(&mpSubBgmSound, JA_BGM_SEA_ENEMY, (on ^ 1) != 0 ? 1 : 0, 0);
        } else {
            bgmMute(&mpSubBgmSound, JA_BGM_BATTLE_NORM, 0, 0);
        }
        sub = mpSubBgmSound.get();
        if (sub == 0)
            return;
        if (mSubBgmNum == JA_BGM_SEA_ENEMY)
            return;
    }
    /* enemy near / far tracks */
    if (!(dist > ENEMY_NEARBY_DIST)) {
        s32 d1 = (s8)field_0x00c1;
        field_0x00c4 = 0;
        if (d1 == 0) {
            mpSubBgmSound.get()->setTrackVolume(ENEMY_NEARBY_TRACK, 1.0f, ENEMY_NEARBY_FADEIN_TIME);
            mpSubBgmSound.get()->setTrackVolume(ENEMY_NEARBY_TRACK_2, 1.0f,
                                                ENEMY_NEARBY_FADEIN_TIME);
            field_0x00c1 = 1;
        }
        sub = mpSubBgmSound.get();
    } else if ((s8)field_0x00c1 == 1) {
        u32 t = ENEMY_NEARBY_FADEOUT_TIME;
        field_0x00c1 = 0;
        field_0x00c4 = t;
        sub = mpSubBgmSound.get();
    }
    u32 d9 = field_0x00c9;
    u32 fade = BTL_SWORD_USING_FADE_TIME;
    if (d9 == 0) {
        sub->setTrackVolume(SWORD_NOT_USING_TRACK, 1.0f, fade);
        mpSubBgmSound.get()->setTrackVolume(SWORD_USING_TRACK, 0.0f, BTL_SWORD_USING_FADE_TIME);
    } else {
        sub->setTrackVolume(SWORD_USING_TRACK, 1.0f, fade);
        mpSubBgmSound.get()->setTrackVolume(SWORD_NOT_USING_TRACK, 0.0f, BTL_SWORD_USING_FADE_TIME);
    }
}

/* 02028D6C */
void JAIZelBasic::bgmNowKaitengiri() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (field_0x00c8 == 0) {
        Sound *sub = mpSubBgmSound.get();
        if (sub != 0) {
            u32 n = mSubBgmNum;
            if (n > JA_BGM_MBOSS_S) {
            } else if (n >= JA_BGM_MBOSS) {
                sub->setTrackVolume(MBOSS_BTL_KAITENGIRI_TRACK, 1.0f, BTL_KAITENGIRI_FADEIN_TIME);
            } else if (n == JA_BGM_BATTLE_NORM) {
                sub->setTrackVolume(BTL_KAITENGIRI_TRACK, 1.0f, BTL_KAITENGIRI_FADEIN_TIME);
            }
        }
    }
    field_0x00c8 = 2;
}

/* 02028E24 */
u32 JAIZelBasic::getRandomU32(u32 n) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 t = jai_rng_seed;
    u32 seed = (t << 1) + ((t >> 31) ^ ((t >> 6) & 0x10));
    jai_rng_seed = seed;
    u32 q = n != 0 ? seed / n : 0;
    return seed - q * n;
}

/* 02028E54 */
void JAIZelBasic::bgmHitSound(u32 kind) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 n = mSubBgmNum;
    if (n > JA_BGM_MBOSS_S)
        return;
    if (n < JA_BGM_MBOSS && n != JA_BGM_BATTLE_NORM)
        return;
    if (field_0x00c9 == 0)
        return;
    u16 v;
    switch (kind) {
    case 1:
        v = (u16)(getRandomU32(2) + 5);
        break;
    case 2:
        v = 9;
        break;
    case 3:
        v = (u16)(getRandomU32(2) + 10);
        break;
    case 4:
        v = (u16)(getRandomU32(2) + 7);
        break;
    default:
        v = (u16)((u16)getRandomU32(4) + 1);
        break;
    }
    Sound *sub = mpSubBgmSound.get();
    if (sub == 0)
        return;
    Track *track = rootTrack(sub->getSeqParameter());
    gabi::Local<be<u16>> tmp;
    *tmp = 0;
    track->readPortApp(0x100A0001, tmp.get()); /* TTrack::readPortApp */
    track->writePortApp(0x00090000, v);
    track->writePortApp(0x000A0000, u16(*tmp));
}

/* 02028FE0 */
void JAIZelBasic::bgmSetSwordUsing(u32 v) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    field_0x00c9 = v;
}

/* 02028FE8 */
void JAIZelBasic::prepareLandingDemo(u32 v) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mbLandingDemoStarted = v;
    bgmStop(0x5A, 0);
}

/* 02028FF8 */
void JAIZelBasic::startLandingDemo() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 c = mbLandingDemoStarted;
    if (c == 2)
        bgmStart(JA_BGM_ISLAND_DRAGON, 0, 0);
    else if (c == 3)
        bgmStart(JA_BGM_ISLAND_WOOD, 0, 0);
}

/* 02029038 */
void JAIZelBasic::endLandingDemo() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mbLandingDemoStarted = 0;
}

/* 02029044 */
void JAIZelBasic::seStopActor(VectorWords *pos, u32 id) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if ((s32)id == -1) {
        stopAllSound(pos); /* JAIBasic::stopAllSound(pos) */
    } else if (pos == 0) {
        stopAllSoundID(id);
    } else {
        stopActorSound(id, pos);
    }
}

/* 02029070 */
void JAIZelBasic::seDeleteObject(VectorWords *pos) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    deleteObject(pos); /* JAIBasic::deleteObject */
}

// HD: the cached voice index prevents restarting the same voice.
/* 02029074 */
void JAIZelBasic::charVoicePlay(u32 a, u32 b, VectorWords *pos, u32 reverb) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (field_0x0201 == 1)
        return;
    u32 v = ((a << 8) + b) & 0xFFFF;
    if (v == mVoiceIndexHD)
        return;
    Sound *s = field_0x2060.get();
    if (s != 0) {
        s->stop(0);
        mVoiceIndexHD = 0xFFFF;
    }
    startSoundVec(JA_SE_CV_COMMON_PEOPLE, &field_0x2060, pos, 0, 0, 4);
    if (field_0x2060.get() == 0)
        return;
    field_0x2060.get()->setPortData(8, v);
    field_0x2060.get()->setPortData(9, reverb & 0xFFFF);
    mVoiceIndexHD = v;
}

// HD: copy the message position as words; dialogue exclusions keep the original IDs.
/* 0202914C */
void JAIZelBasic::messageSePlay(u32 msg, VectorWords *pos, u32 reverb) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    VectorWords *p = pos;
    if (pos != 0) {
        auto source = pos;
        mMessagePositionHD.w[0] = source->w[0];
        mMessagePositionHD.w[1] = source->w[1];
        mMessagePositionHD.w[2] = source->w[2];
        p = &mMessagePositionHD;
    }
    if (msg >= 0x118)
        return;
    if (msg == 0x9F || (msg >= 0xB4 && msg <= 0xBA) || msg == 0x102 || msg == 0x104)
        p = 0;
    if (msg == 0x10D || msg == 0x115)
        return;
    MessageVoice &entry = messageSeTable.entries[msg];
    u32 a = entry.character, b = entry.sound;
    if (a == 0xFFFF || b == 0xFFFF)
        return;
    u32 t = a & 0xF000;
    if (t == 0) {
        charVoicePlay(a, b, p, reverb);
    } else if (t == 0x1000 || t == 0x2000 || t == 0x3000 || t == 0x8000) {
        seStart(b, p, 0, reverb, 1.0f, 1.0f, -1.0f, -1.0f, 0);
    }
}

/* a random entry of a voice table (8 bytes per voice, {sel, vowel} pairs): TRandom * 4 */
// HD: float-to-unsigned conversion keeps the GHS signed conversion split.
u32 JAIZelBasic::randomIndex() {
    f32 f = mVoiceRandomHD.get() * 4.0f;
    if (!(f < 2147483648.0f))
        return (u32)gabi::ftoi(f - 2147483648.0f) + 0x80000000;
    return (u32)gabi::ftoi(f);
}

/* 020292B4 */
u32 JAIZelBasic::getLinkVoiceVowel(u32 n) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    LinkVoice &voice = *voiceAt(&linkVoiceTable, n);
    u32 i, k = 0;
    for (i = 0; i < 400; i++) {
        k = randomIndex();
        if (voice.entries[k].sound != 0xFF)
            break;
    }
    if (i == 400)
        return 0xFF;
    return voice.entries[k].vowel;
}

/* 020293CC */
void JAIZelBasic::linkVoiceStart(u32 n, VectorWords *pos, u32 vowel, u32 reverb) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (field_0x022d == 0)
        return;
    if (mStreamBgmNum == JA_STRM_DEMO_GO_MAJU && (s32)n == 0x10) {
        if (field_0x00b8 != 0)
            return;
        if (getRandomU32(3) != 0)
            return;
    }
    if (mMainBgmNum == JA_BGM_SEA_GAME && (s32)n == 3) {
        Sound *m = mpMainBgmSound.get();
        f32 t = VOL_BGM_TALKING;
        field_0x0090 = t;
        if (m != 0 && mpSubBgmSound.get() == 0) {
            f32 v = gabi::fmuls_ppc(field_0x0080, field_0x0084);
            v = gabi::fmuls_ppc(v, field_0x0088);
            v = gabi::fmuls_ppc(v, field_0x008c);
            v = gabi::fmuls_ppc(v, t);
            v = gabi::fmuls_ppc(v, field_0x0094);
            v = gabi::fmuls_ppc(v, field_0x0098);
            v = gabi::fmuls_ppc(v, field_0x009c);
            v = gabi::fmuls_ppc(v, field_0x00ac);
            m->setVolume(v, 2, 0);
        }
    } else if ((s32)n == 0xC) {
        pos = 0;
    }
    LinkVoice &voice = *voiceAt(&linkVoiceTable, n);
    u32 sel = 0;
    bool found = false;
    if ((s32)vowel != 0xFF) {
        for (u32 i = 0; i < 400; i++) {
            u32 k = randomIndex();
            if (voice.entries[k].vowel == vowel) {
                u32 c = voice.entries[k].sound;
                if (c != 0xFF) {
                    sel = c;
                    found = true;
                    break;
                }
            }
        }
    }
    if (!found) {
        sel = voice.entries[0].sound;
        if (sel == 0xFF)
            return;
    }
    seStart(sel + 0x1800, pos, 0, reverb, 1.0f, 1.0f, -1.0f, -1.0f, 0);
}

/* 02029638 */
void JAIZelBasic::shipCruiseSePlay(VectorWords *pos, f32 speed) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    Sound *m = mpMainBgmSound.get();
    field_0x1f40 = speed;
    if (m != 0) {
        u32 n = mMainBgmNum;
        if (n == JA_BGM_SEA || n == JA_BGM_SEA_STORM) {
            u32 set = mIsSailing == 0 && speed < 0.2f ? 1 : 0;
            bgmMute(&mpMainBgmSound, n == JA_BGM_SEA ? JA_BGM_SEA : JA_BGM_SEA_STORM, set, 0x1E);
        }
    }
    if (field_0x0201 == 1)
        return;
    void *matrix = cameraMatrix();
    gabi::Local<VectorWords> position;
    position->w[0] = l_shipPosition.w[0];
    position->w[1] = l_shipPosition.w[1];
    position->w[2] = l_shipPosition.w[2];
    VectorWords *source = pos != nullptr ? pos : cameraPosition();
    if (source != nullptr) {
        position->w[0] = source->w[0];
        position->w[1] = source->w[1];
        position->w[2] = source->w[2];
    }
    if (matrix != nullptr)
        jaizelTransform(matrix, position.get(), position.get());

    auto transformed = reinterpret_cast<cXyz *>(position.get());
    f32 y = transformed->y, x = transformed->x, z = transformed->z;
    f32 d = std_sqrtf(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y)));
    if (!(d < getParamDistanceMax()))
        return;
    f32 vol2 = 1.0f;
    if (!(speed > 0.1f)) {
        startSoundVec(JA_SE_SHIP_WAVE_S, &field_0x1f48, pos, 0, 0, 4);
        if (field_0x1f48.get() != 0)
            field_0x1f48.get()->setVolume(1.0f, 0, 0);
        return;
    }
    u32 id2;
    f32 vol1;
    bool play1 = true;
    if (!(speed > 0.5f)) {
        vol1 = (speed - 0.1f) / 0.4f;
        vol2 = 1.0f - vol1;
        id2 = JA_SE_SHIP_WAVE_S;
        if (vol1 == 0.0f)
            play1 = false;
    } else if (!(speed > 0.8f) || !(speed > 0.9f)) {
        id2 = JA_SE_SHIP_WAVE_L;
        vol1 = 1.0f;
        vol2 = (speed - 0.5f) / 0.4f;
    } else {
        vol1 = 1.0f;
        id2 = JA_SE_SHIP_WAVE_L;
    }
    if (play1) {
        startSoundVec(JA_SE_SHIP_GO_NORMAL, &field_0x1f44, pos, 0, 0, 4);
        if (field_0x1f44.get() != 0)
            field_0x1f44.get()->setVolume(vol1, 0, 0);
    }
    if (vol2 == 0.0f)
        return;
    startSoundVec(id2, &field_0x1f48, pos, 0, 0, 4);
    if (field_0x1f48.get() != 0)
        field_0x1f48.get()->setVolume(vol2, 0, 0);
}

/* 020299D0 */
void JAIZelBasic::setShipSailState(u32 v) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mIsSailing = v;
}

/* 020299D8 */
void JAIZelBasic::monsSeStart(u32 id, VectorWords *pos, u32 obj, u32 info, u32 reverb) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (field_0x0201 == 1)
        return;
    if (pos != 0)
        checkBasicPointer(pos);
    bool kurobo = id - JA_SE_CV_KS_ATTACK < 3 || id - JA_SE_CV_PT_ATTACK < 6;
    if (!kurobo && (s32)field_0x0224 == 0x2D)
        kurobo = (id >= JA_SE_CV_PW_RUN && id <= JA_SE_CV_PW_DIE) || id == JA_SE_CV_PW_CURSE_IN;
    if (kurobo) {
        kuroboVoicePlay(id, pos, reverb);
        return;
    }
    u32 slot = 0;
    bool assigned = false;
    u32 i;
    for (i = 0; i < 30; i++) {
        if (field_0x1f4c[i].field_0x00 != obj)
            continue;
        Sound *s = field_0x1f4c[i].field_0x04.get();
        if (s != 0)
            s->stop(0);
        slot = i;
        assigned = true;
        break;
    }
    if (!assigned) {
        slot = 0x1D;
        for (u32 k = 0; k < 30; k++) {
            if (s32(field_0x1f4c[k].field_0x00) == -1) {
                field_0x1f4c[k].field_0x00 = obj;
                slot = k;
                break;
            }
        }
    }
    if (info > 9)
        info = obj % 10;
    gptr<Sound> *handle = &field_0x1f4c[slot].field_0x04;
    startSoundVec(id, handle, pos, 0, info, 4);
    if (handle->get() != nullptr)
        handle->get()->setPortData(9, reverb & 0xFFFF);
}

/* 02029B88 */
void initializeBasicHeaderStatics() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    l_headerStatics.words[2] = 0;
    l_headerStatics.words[0] = 0;
    l_headerStatics.words[3] = 0;
    l_headerStatics.words[1] = 0;
    registerBasicGlobal(&l_headerRecord);
    f32 low = l_headerRange.low, high = l_headerRange.high;
    l_headerStatics.low = low;
    l_headerStatics.high = high;
    constructBasicByte0(&l_headerStatics.byte0);
    registerBasicGlobal(&l_byte0Record);
    constructBasicByte1(&l_headerStatics.byte1);
    registerBasicGlobal(&l_byte1Record);
}
/* 02029C1C */
void destroyBasicSafeString(void *object, u32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (object == nullptr || (flags & 1) == 0)
        return;
    deleteBasicObject(object);
}
/* 02029C30 */
f32 JAIZelBasic::getMapInfoFxParameter(u32 n) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return l_zero;
}
/* 02029C3C */
u32 JAIZelBasic::getMapInfoGround(u32 n) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 0;
}
/* 02029C44 */
AllocationRecord *createBasicAllocationRecord(AllocationRecord *record) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (record == nullptr) {
        record = static_cast<AllocationRecord *>(allocateBasicRecord(8));
        if (record == nullptr)
            return nullptr;
    }
    record->sound = nullptr;
    record->count = 0;
    return record;
}
/* 02029C84 */
void destroyBasicObject(JAIZelBasic *object, u32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (object == nullptr || (flags & 1) == 0)
        return;
    deleteBasicObject(object);
}
/* 02029C98 */
void basicFinalizeStub(u32 argument) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}
/* 02029C9C */
void basicAssureTerminationStub(u32 argument) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

// clang-format off
/* WWHD glue:
data VOL_BGM_TALKING 1018DC6C
data ENEMY_NEARBY_DIST 1018DC78
data ENEMY_NEARBY_FADEIN_TIME 1018DC7C
data ENEMY_NEARBY_FADEOUT_TIME 1018DC80
data BTL_KAITENGIRI_FADEIN_TIME 1018DC84
data BTL_SWORD_USING_FADE_TIME 1018DC8C
data ENEMY_NEARBY_TRACK 1018DCAF
data ENEMY_NEARBY_TRACK_2 1018DCB0
data BTL_KAITENGIRI_TRACK 1018DCB1
data SWORD_USING_TRACK 1018DCB2
data SWORD_NOT_USING_TRACK 1018DCB3
data COMMON_BATTLE_FO_DELAY 1018DCB4
data MBOSS_BTL_KAITENGIRI_TRACK 1018DCB6
data jai_rng_seed 1018D4F0
data l_zero 10003898
data l_talkingCategoryVolumes 1018DC9F
data l_practicePosition 100039DC
data l_shipPosition 100039F0
data messageSeTable 1018D53C
data linkVoiceTable 1018D99C
data l_headerStatics 101FFC5C
data l_headerRange 10003A00
data l_headerRecord 1018D4F4
data l_byte0Record 1018D500
data l_byte1Record 1018D50C
ext 0201E5CC s32 jaizel8::JAIZelBasic::isDemo()
ext 0202204C void jaizel8::JAIZelBasic::bgmStart(u32,u32,u32)
ext 02021F28 void jaizel8::JAIZelBasic::bgmStop(u32,u32)
ext 0201DD18 void jaizel8::JAIZelBasic::bgmMute(gptr<jaizel8::Sound>*,u32,u32,u32)
ext 02021B10 void jaizel8::JAIZelBasic::hdStreamVolume(u32,f32)
ext 020259E8 void jaizel8::JAIZelBasic::enemyNearBy()
ext 02025940 void jaizel8::JAIZelBasic::mbossBgmNearByProcess(f32)
ext 02025AC0 s32 jaizel8::JAIZelBasic::checkLinkOnSea()
ext 02025AF4 s32 jaizel8::JAIZelBasic::checkLinkOnBoardSea()
ext 0202587C void jaizel8::JAIZelBasic::kuroboVoicePlay(u32,jaizel8::VectorWords*,u32)
ext 0201EBA0 u32 jaizel8::JAIZelBasic::seStart(u32,jaizel8::VectorWords*,u32,u32,f32,f32,f32,f32,u32)
ext 0201C360 f32 jaizel8::JAIZelBasic::calcPosVolume(jaizel8::VectorWords*,f32)
ext 0201C0E8 f32 jaizel8::JAIZelBasic::calcPosPanLR(jaizel8::VectorWords*)
ext 0201C228 f32 jaizel8::JAIZelBasic::calcPosPanSR(jaizel8::VectorWords*,f32)
ext 02802848 void jaizel8::JAIZelBasic::startSoundVec(u32,gptr<jaizel8::Sound>*,jaizel8::VectorWords*,u32,u32,u32)
ext 02803138 void jaizel8::JAIZelBasic::setSeCategoryVolume(u32,u32)
ext 02802994 void jaizel8::JAIZelBasic::stopAllSound(jaizel8::VectorWords*)
ext 02802AEC void jaizel8::JAIZelBasic::stopAllSoundID(u32)
ext 02802C60 void jaizel8::JAIZelBasic::stopActorSound(u32,jaizel8::VectorWords*)
ext 02802E00 void jaizel8::JAIZelBasic::deleteObject(jaizel8::VectorWords*)
ext 0280B160 void jaizel8::Sound::start(u32)
ext 0280B1D8 void jaizel8::Sound::stop(u32)
ext 0280B68C void jaizel8::Sound::setVolume(f32,u32,u32)
ext 0280B2AC void jaizel8::Sound::setSeqInterVolume(u32,f32,u32)
ext 0280BA28 void jaizel8::Sound::setPan(f32,u32,u32)
ext 0280C248 void jaizel8::Sound::setDolby(f32,u32,u32)
ext 0280C6D8 void jaizel8::Sound::setPortData(u32,u32)
ext 0280C9E4 void jaizel8::Sound::setTrackVolume(u32,f32,u32)
ext 0280B05C jaizel8::Sequence* jaizel8::Sound::getSeqParameter()
ext 02818C20 void jaizel8::Track::writePortApp(u32,u32)
ext 02818C68 void jaizel8::Track::readPortApp(u32,be<u16>*)
ext 027ED7E0 f32 jaizel8::Random::get()
ext 02804718 f32 getParamDistanceMax()
ext 028F4384 f32 std_sqrtf(f32)
ext 028E8F64 void jaizelTransform(void*,jaizel8::VectorWords*,jaizel8::VectorWords*)
ext 0273AD10 void* allocateBasicRecord(u32)
ext 0273AF40 void deleteBasicObject(void*)
ext 0275CC9C u32 checkBasicPointer(void*)
ext 028F026C void registerBasicGlobal(jaizel8::DestructionRecord*)
ext 028ED6F8 void constructBasicByte0(jaizel8::GuestByte*)
ext 028EAB2C void constructBasicByte1(jaizel8::GuestByte*)
*/
