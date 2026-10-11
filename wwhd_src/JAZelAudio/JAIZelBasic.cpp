/** Zelda audio BGM and sound-effect control. Verified against the HD image; private derived source.
 */
#include "JAZelAudio/JAIZelBasic.h"
using namespace jaizel12;

namespace jaizel12 {
static ManagerPtr l_soundManager;
static GuestFloat VOL_BGM_DEFAULT;
static GuestFloat VOL_BGM_PAUSING;
static CategoryVolumes l_defaultCategoryVolumes;
static CategoryVolumes l_pauseCategoryVolumes;
static GuestByte MBOSS_BTL_SWORD_USING_TRACK;
static GuestWord l_demoState;
static MuteMasks m_bgm_mute_state;
static ManagerPtr l_defaultHeap;
static ManagerPtr l_heapManager;
static VectorWords l_defaultLevelPosition;
static VectorWords l_defaultSePosition;
static GuestFloat l_pitchIncrement;
static SoundIds l_attackSoundIds;

/* 0201D414 */
Sound *JAIZelBasic::makeSound(s32 n) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 idx = mAllocationRecordIndexHD;
    AllocationRecord *entry = indexed(mAllocationRecordsHD, idx);
    void *heap = field_0x8;
    if (heap == nullptr)
        heap = l_defaultHeap;
    if (n > 0) {
        if (heap == nullptr)
            heap = getDefaultAudioHeap(l_heapManager);
        // HD: the heap allocator is virtual; construct each non-null wrapped array element.
        Heap *audioHeap = reinterpret_cast<Heap *>(heap);
        Sound *array = allocateSoundArray(audioHeap, u32(n) * 0x48);
        s32 count = n > 0 ? n : 1;
        for (u32 offset = 0; count != 0; --count, offset += 0x48) {
            Sound *sound = soundAtByteOffset(array, offset);
            if (sound != nullptr)
                constructZelSound(sound);
        }
        if (array != nullptr) {
            entry->sound = array;
            entry->count = n;
        }
        idx = mAllocationRecordIndexHD;
        entry = indexed(mAllocationRecordsHD, idx);
    }
    Sound *result = entry->sound;
    mAllocationRecordIndexHD = idx + 1;
    return result;
}

/* 0201D57C */
void JAIZelBasic::kuroboMotionPlay(u32 id, cXyz *pos, u32 info, u32 reverb) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 c = field_0x203c.get();
    if (c >= 4)
        return;
    field_0x203c = c + 1;
    u32 i;
    for (i = 0; i < 4; i++)
        if (field_0x2040[i].get() == 0)
            break;
    if (i == 4)
        return;
    gptr<Sound> *h = &field_0x2040[i];
    startSoundVec(id, h, pos, 0, info, 4);
    if (h->get() != 0)
        h->get()->setPortData(9, reverb & 0xFFFF);
}

/* 0201D62C */
s32 JAIZelBasic::checkSePlaying(u32 id) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    for (u32 i = 0; i < 32; i++)
        if (mSeNum[i].get() == id && mpSeSound[i].get() != 0)
            return 1;
    return 0;
}

// HD: stop duplicate positions and enforce the distance/priority/instance limits.
/* 0201D670 */
s32 JAIZelBasic::seLimitInstances(u32 id, cXyz *pos, s32 prio, s32 maxCount, f32 maxDist) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (pos == 0)
        return 0;
    if (cameraPosition(camera()) == nullptr)
        return 0;
    gabi::Local<VectorWords> tmp;
    VectorWords *t = tmp.get();
    for (u32 i = 0; i < 32; i++) {
        if (mSeNum[i].get() != id)
            continue;
        Sound *s = mpSeSound[i].get();
        if (s == 0)
            continue;
        cXyz *sp = soundPosition(s);
        if (sp == 0)
            continue;
        if (sp == pos) {
            s->stop(1);
            return 0;
        }
        t->w[0] = reinterpret_cast<VectorWords *>(sp)->w[0].get();
        t->w[1] = reinterpret_cast<VectorWords *>(sp)->w[1].get();
        t->w[2] = reinterpret_cast<VectorWords *>(sp)->w[2].get();
        subtractSoundVector(sp, pos, t);
        if (soundVectorMagnitude(t) > maxDist)
            continue;
        if (soundPriority(mpSeSound[i]) <= prio)
            return 1;
    }
    if (maxCount <= 0)
        return 0;
    s32 cnt = 0, best = -1, bestv = -1;
    for (u32 k = 0; k < 32; k++) {
        if (mSeNum[k].get() != id)
            continue;
        Sound *s = mpSeSound[k].get();
        if (s == 0)
            continue;
        s32 v = soundPriority(s);
        if (v > bestv) {
            best = k;
            bestv = v;
        }
        cnt++;
    }
    if (cnt < maxCount)
        return 0;
    if (bestv <= 0)
        return 1;
    Sound *s = mpSeSound[best].get();
    if (s != 0)
        s->stop(1);
    return 0;
}

/* 0201D84C */
void JAIZelBasic::stopBattleBgm() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    Sound *s = mpSubBgmSound.get();
    if (s == 0)
        return;
    u32 n = mSubBgmNum.get();
    if (n != JA_BGM_BATTLE_NORM && n != JA_BGM_SEA_ENEMY)
        return;
    s->stop(45);
    mpSubBgmSound = 0;
    mSubBgmNum = (u32)-1;
}

/* 0201D8BC */
void JAIZelBasic::subBgmStop() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 n = mSubBgmNum.get();
    bool keep;
    if (n >= JA_BGM_TAKT_CHUYA)
        keep = n == JA_BGM_TRIFORCE_DEMO || n == JA_BGM_BGN_GET_BOX || n == JA_BGM_GET_PEARL ||
               n <= JA_BGM_TAKT_SHIPPU;
    else
        keep = n == JA_BGM_GET_SONG ||
               (n <= JA_BGM_ITEM_GET_S &&
                (n >= JA_BGM_GET_HEART || n == JA_BGM_OPEN_BOX || n == JA_BGM_ITEM_GET));
    if (keep)
        return;
    u32 m = mSubBgmNum.get();
    field_0x00b0 = 0xF;
    mLastMinibossSubBGMType = 0;
    if (m == JA_BGM_MBOSS || m == JA_BGM_MBOSS_S)
        field_0x00ce = 0x14;
}

/* 0201D9C8 */
void JAIZelBasic::subBgmStopInner() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if ((s32)field_0x00b0.get() == -1)
        return;
    u32 n = mSubBgmNum.get();
    Sound *s = mpSubBgmSound.get();
    if (n == JA_BGM_MBOSS || n == JA_BGM_MBOSS_S)
        field_0x00ce = 0x14;
    field_0x0064 = 0;
    mLastMinibossSubBGMType = 0;
    if (s != 0) {
        s->stop(45);
    } else if ((s32)mSubBgmNum.get() == -1) {
        return;
    }
    Sound *main = mpMainBgmSound.get();
    mSubBgmNum = (u32)-1;
    field_0x0080 = 1.0f;
    mpSubBgmSound = 0;
    field_0x008c = 1.0f;
    if (main != 0) {
        // The leading volume factor was just set to1.
        f32 v = mul(field_0x0084.get(), field_0x0088.get());
        v = mul(v, field_0x008c.get());
        v = mul(v, field_0x0090.get());
        v = mul(v, field_0x0094.get());
        v = mul(v, field_0x0098.get());
        v = mul(v, field_0x009c.get());
        v = mul(v, field_0x00ac.get());
        main->setVolume(v, 45, 0);
    }
    field_0x00c4 = 0;
    field_0x00c1 = 0;
    field_0x00b0 = (u32)-1;
}

// HD: stream status comes from the HD sound manager.
/* 0201DB08 */
s32 JAIZelBasic::checkStreamPlaying(u32 id) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    void *p = hdStreamPlayer(l_soundManager);
    return hdCheckStream(p, id, 1, 0, 0);
}

// HD: talk-out accepts the fade time rather than fixing it at2.
/* 0201DB50 */
void JAIZelBasic::talkOut(u32 fade) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (checkStreamPlaying(JA_STRM_MJ_DEMO_INTRO) == 1)
        return;
    Sound *main = mpMainBgmSound.get();
    f32 def = VOL_BGM_DEFAULT.get(); /* JAIZelParam::VOL_BGM_DEFAULT */
    field_0x0084 = def;
    if (main != 0) {
        f32 v = mul(field_0x0080.get(), def);
        v = mul(v, field_0x0088.get());
        v = mul(v, field_0x008c.get());
        v = mul(v, field_0x0090.get());
        v = mul(v, field_0x0094.get());
        v = mul(v, field_0x0098.get());
        v = mul(v, field_0x009c.get());
        v = mul(v, field_0x00ac.get());
        main->setVolume(v, fade, 0);
    }
    Sound *sub = mpSubBgmSound.get();
    f32 def2 = VOL_BGM_DEFAULT.get();
    field_0x00a0 = def2;
    if (sub != 0) {
        f32 v = mul(def2, field_0x00a4.get());
        v = mul(v, field_0x00a8.get());
        sub->setSeqInterVolume(0, v, fade);
    }
    for (u32 c = 0; c < 8; c++)
        setSeCategoryVolume(c, l_defaultCategoryVolumes.entries[c].get());
}

/* 0201DCD4 */
void JAIZelBasic::menuOut() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    field_0x0088 = VOL_BGM_DEFAULT.get();
    talkOut(2);
    field_0x0020 = 0;
}

/* 0201DD18 */
void JAIZelBasic::bgmMute(gptr<Sound> *h, u32 bgm, u32 set, u32 fade) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (h == nullptr)
        return;
    u32 mask = *indexed(&m_bgm_mute_state.masks[0][0], ((bgm & 0xFF) << 2) + set);
    Sound *sound = *h;
    if (sound == nullptr)
        return;
    for (u32 track = 0; track < 16; ++track) {
        if (track != 0)
            sound = *h;
        sound->setTrackVolume(track & 0xFF, (mask & 1) ? 1.0f : 0.0f, fade);
        mask >>= 1;
    }
}

/* 0201DE00 */
void JAIZelBasic::subBgmStart(u32 id) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    field_0x00be = 0;
    if (field_0x0063.get() != 0)
        return;
    if (id == 0xFFFFFFFF)
        return;
    if (mSomeSpecialBGMFlag.get() == 1)
        return;
    /* per-bgm state */
    int kind = 0; /* 1: battle-like (CB=2, 74=1, 276=1), 2..4: C6 = 1..3, 5: 276 = 1 */
    if (id >= JA_BGM_TAKT_CHUYA) {
        if (id < JA_BGM_P_GANON_2) {
            if (id == JA_BGM_DIOCTA_2)
                kind = 3;
            else if (id == JA_BGM_P_GANON_1)
                kind = 4;
            else if (id <= JA_BGM_TAKT_SHIPPU)
                kind = 1;
        } else if (id == JA_BGM_TRIFORCE_DEMO || id == JA_BGM_BGN_GET_BOX ||
                   id == JA_BGM_GET_PEARL) {
            kind = 1;
        } else if (id == JA_BGM_P_GANON_2) {
            kind = 4;
        }
    } else if (id >= JA_BGM_GET_HEART) {
        if (id == JA_BGM_DIOCTA_BATTLE)
            kind = 3;
        else if (id == JA_BGM_GET_SONG)
            kind = 1;
        else if (id <= JA_BGM_ITEM_GET_S)
            kind = 1;
    } else if (id <= JA_BGM_MBOSS_S) {
        if (id >= JA_BGM_MBOSS)
            kind = 2;
        else if (id == JA_BGM_OPEN_BOX)
            kind = 5;
        else if (id == JA_BGM_ITEM_GET)
            kind = 1;
    }
    switch (kind) {
    case 1:
        field_0x00bb = 2;
        field_0x0064 = 1;
        field_0x0206 = 1;
        break;
    case 2:
        mLastMinibossSubBGMType = 1;
        break;
    case 3:
        mLastMinibossSubBGMType = 2;
        break;
    case 4:
        mLastMinibossSubBGMType = 3;
        break;
    case 5:
        field_0x0206 = 1;
        break;
    }
    if (mpSubBgmSound.get() != 0 && id == mSubBgmNum.get()) {
        field_0x00b0 = (u32)-1;
        return;
    }
    /* no fade-in for these */
    bool quick;
    if (id < JA_BGM_GET_SONG)
        quick = id <= JA_BGM_ITEM_GET_S && (id >= JA_BGM_GET_HEART || id == JA_BGM_MBOSS ||
                                            id == JA_BGM_OPEN_BOX || id == JA_BGM_ITEM_GET);
    else if (id < JA_BGM_P_GANON_1)
        quick = id <= JA_BGM_TAKT_SHIPPU &&
                (id >= JA_BGM_TAKT_CHUYA || id == JA_BGM_DIOCTA_BATTLE || id == JA_BGM_GET_SONG);
    else
        quick = id == JA_BGM_TRIFORCE_DEMO || id == JA_BGM_BGN_GET_BOX || id == JA_BGM_GET_PEARL ||
                id == JA_BGM_P_GANON_1;
    startSoundVec(id, &mpSubBgmSound, 0, quick ? 0 : 0x3C, 0, 4);
    if (mpSubBgmSound.get() != 0) {
        bool ok = true;
        if (id != JA_BGM_BGN_GET_BOX) {
            mpSubBgmSound.get()->setSeqInterVolume(0, field_0x00a0.get(), 0);
            ok = mpSubBgmSound.get() != 0;
        }
        if (ok) {
            if (id == JA_BGM_SEA_ENEMY) {
                if (field_0x0047.get() == 1)
                    bgmMute(&mpSubBgmSound, id, 0, 0);
                else
                    bgmMute(&mpSubBgmSound, id, 1, 0);
            } else if (id <= JA_BGM_MBOSS_S && id >= JA_BGM_MBOSS) {
                bgmMute(&mpSubBgmSound, id, 0, 0);
                if (field_0x00c9.get() != 0)
                    mpSubBgmSound.get()->setTrackVolume(MBOSS_BTL_SWORD_USING_TRACK.get(), 1.0f, 0);
            }
        }
    }
    Sound *main = mpMainBgmSound.get();
    field_0x008c = 0.0f;
    if (main != 0) {
        bool quick2;
        if (id < JA_BGM_GET_SONG)
            quick2 = id <= JA_BGM_ITEM_GET_S && (id >= JA_BGM_GET_HEART || id == JA_BGM_MBOSS ||
                                                 id == JA_BGM_OPEN_BOX || id == JA_BGM_ITEM_GET);
        else if (id < JA_BGM_P_GANON_1)
            quick2 = id == JA_BGM_CB_NEXTYEAR || id == JA_BGM_TAKT_CHUYA ||
                     id == JA_BGM_DIOCTA_BATTLE || id == JA_BGM_GET_SONG;
        else
            quick2 = id == JA_BGM_TRIFORCE_DEMO || id == JA_BGM_BGN_GET_BOX ||
                     id == JA_BGM_GET_PEARL || id == JA_BGM_P_GANON_1;
        f32 v = mul(field_0x0080.get(), field_0x0084.get());
        v = mul(v, field_0x0088.get());
        v = mul(v, 0.0f);
        v = mul(v, field_0x0090.get());
        v = mul(v, field_0x0094.get());
        v = mul(v, field_0x0098.get());
        v = mul(v, field_0x009c.get());
        v = mul(v, field_0x00ac.get());
        main->setVolume(v, quick2 ? 1 : 0x3C, 0);
    }
    mSubBgmNum = id;
    field_0x00b0 = (u32)-1;
}

/* 0201E480 */
s32 JAIZelBasic::checkEventBit(u32 flag) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    be<u8> *ev = field_0x0024;
    if (ev == 0)
        return 0;
    return (ev[(flag >> 8) & 0xFF].get() & flag & 0xFF) != 0;
}

/* 0201E4AC */
void JAIZelBasic::onEnemyDamage() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 n = mSubBgmNum.get();
    if (n == JA_BGM_SEA_ENEMY) {
        Sound *s = mpSubBgmSound.get();
        if (s != 0)
            s->setPortData(9, 1);
    } else if (n == JA_BGM_BATTLE_NORM) {
        Sound *s = mpSubBgmSound.get();
        if (s != 0) {
            void *track = rootTrack(s->getSeqParameter());
            if (track != 0)
                writeTrackPort(track, 0x000B0000, 1);
        }
    }
}

/* 0201E538 */
void JAIZelBasic::seStop(u32 id, u32 fade) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (id == 0xFFFFFFFF)
        return;
    if (id == JA_SE_LK_RUN_SLIP)
        fade = 8;
    for (u32 i = 0; i < 32; i++) {
        if (mSeNum[i].get() != id)
            continue;
        Sound *s = mpSeSound[i].get();
        if (s == 0)
            continue;
        s->stop(fade);
        mpSeSound[i] = 0;
        mSeNum[i] = 0;
        field_0x0194[i] = 0;
    }
}

// HD: demo state comes from a global word.
/* 0201E5CC */
s32 JAIZelBasic::isDemo() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return l_demoState.get() != 0;
}

/* 0201E5E0 */
void JAIZelBasic::menuIn() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    Sound *main = mpMainBgmSound.get();
    f32 p = VOL_BGM_PAUSING.get(); /* JAIZelParam::VOL_BGM_PAUSING */
    field_0x0088 = p;
    if (main != 0) {
        f32 v = mul(field_0x0080.get(), field_0x0084.get());
        v = mul(v, p);
        v = mul(v, field_0x008c.get());
        v = mul(v, field_0x0090.get());
        v = mul(v, field_0x0094.get());
        v = mul(v, field_0x0098.get());
        v = mul(v, field_0x009c.get());
        v = mul(v, field_0x00ac.get());
        main->setVolume(v, 2, 0);
    }
    Sound *sub = mpSubBgmSound.get();
    f32 p2 = VOL_BGM_PAUSING.get();
    field_0x00a0 = p2;
    if (sub != 0) {
        f32 v = mul(p2, field_0x00a4.get());
        v = mul(v, field_0x00a8.get());
        sub->setSeqInterVolume(0, v, 2);
    }
    for (u32 c = 0; c < 8; c++)
        setSeCategoryVolume(c, l_pauseCategoryVolumes.entries[c].get());
    field_0x0020 = 1;
}

} // namespace jaizel12

// clang-format off
/* WWHD glue:
data l_soundManager 1018EC64
data VOL_BGM_DEFAULT 1018DC68
data VOL_BGM_PAUSING 1018DC70
data l_defaultCategoryVolumes 1018DC97
data l_pauseCategoryVolumes 1018DCA7
data MBOSS_BTL_SWORD_USING_TRACK 1018DCB7
data l_demoState 101D6010
data m_bgm_mute_state 1018DCB8
data l_defaultHeap 101F9DA8
data l_heapManager 101F8B4C
data l_defaultLevelPosition 100038B0
data l_defaultSePosition 10003934
data l_pitchIncrement 1000392C
data l_attackSoundIds 1000392E
ext 02802848 void jaizel12::JAIZelBasic::startSoundVec(u32, gptr<Sound>*, cXyz*, u32, u32, u32)
ext 02803138 void jaizel12::JAIZelBasic::setSeCategoryVolume(u32, u32)
ext 02802E00 void jaizel12::JAIZelBasic::deleteObject(cXyz*)
ext 0201C360 f32 jaizel12::JAIZelBasic::calcPosVolume(VectorWords*, f32)
ext 0201C0E8 f32 jaizel12::JAIZelBasic::calcPosPanLR(VectorWords*)
ext 0201C228 f32 jaizel12::JAIZelBasic::calcPosPanSR(VectorWords*, f32)
ext 0280B1D8 void jaizel12::Sound::stop(u32)
ext 0280B68C void jaizel12::Sound::setVolume(f32, u32, u32)
ext 0280BD74 void jaizel12::Sound::setPitch(f32, u32, u32)
ext 0280BA28 void jaizel12::Sound::setPan(f32, u32, u32)
ext 0280C248 void jaizel12::Sound::setDolby(f32, u32, u32)
ext 0280C6D8 void jaizel12::Sound::setPortData(u32, u32)
ext 0280C9E4 void jaizel12::Sound::setTrackVolume(u32, f32, u32)
ext 0280B2AC void jaizel12::Sound::setSeqInterVolume(u32, f32, u32)
ext 0280B05C void* jaizel12::Sound::getSeqParameter()
ext 0202BF78 void* hdStreamPlayer(void*)
ext 0202FD40 s32 hdCheckStream(void*, u32, u32, u32, u32)
ext 02756140 void* getDefaultAudioHeap(void*)
ext 0202ACCC void constructZelSound(jaizel12::Sound*)
ext 02818C20 void writeTrackPort(void*, u32, u32)
ext 028E8DAC void subtractSoundVector(cXyz*, cXyz*, jaizel12::VectorWords*)
ext 028E8E10 f32 soundVectorMagnitude(jaizel12::VectorWords*)
ext 028E8F64 void transformSoundVector(Mtx34*, jaizel12::VectorWords*, jaizel12::VectorWords*)
ext 02804718 f32 getParamDistanceMax()
ext 028F4384 f32 std_sqrtf(f32)
ext 0275CC9C u32 checkSoundPointer(cXyz*)
*/
