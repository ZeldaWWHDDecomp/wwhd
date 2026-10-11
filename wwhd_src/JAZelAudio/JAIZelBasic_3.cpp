/** JAIZelBasic level-object SE mixing and BGM fanfare control.
 * Verified HD behavior retains GameCube names. */
#include "JAZelAudio/JAIZelBasic_3.h"
using namespace jaizel34;
namespace {
enum BgmID : u32 {
    JA_BGM_ITEM_GET = 0x80000002,
    JA_BGM_GOMA = 0x80000003,
    JA_BGM_OPEN_BOX = 0x80000009,
    JA_BGM_GOMA_2 = 0x80000010,
    JA_BGM_GET_HEART = 0x80000024,
    JA_BGM_ITEM_GET_S = 0x80000025,
    JA_BGM_GET_SONG = 0x80000027,
    JA_BGM_CB_NEXTYEAR = 0x80000036,
    JA_BGM_CB_NEXTYEAR_W = 0x80000037,
    JA_BGM_GET_PEARL = 0x8000004F,
    JA_BGM_BGN_GET_BOX = 0x8000005D,
    JA_BGM_TRIFORCE_DEMO = 0x80000061,
    JA_BGM_SE_SEQUENCE = 0x80000800,
};
}
namespace jaizel34 {
static ManagerPtr l_soundManager;
static GuestFloat l_towerTempo;
} // namespace jaizel34
namespace {
static VectorWords l_defaultPosition;
static SafeStringVtable l_safeStringVtable;
static GuestByte l_demo34;
static GuestByte l_stageName;
using GuestWord = be<u32>;
static GuestWord l_demoState;
inline f32 fsel(f32 test, f32 positive, f32 negative) {
    return test >= 0.0f ? positive : negative;
}
} // namespace
f32 JAIZelBasic::calcMainBgmVol() {
    f32 volume = gabi::fmuls_ppc(field_0x0080, field_0x0084);
    volume = gabi::fmuls_ppc(volume, field_0x0088);
    volume = gabi::fmuls_ppc(volume, field_0x008c);
    volume = gabi::fmuls_ppc(volume, field_0x0090);
    volume = gabi::fmuls_ppc(volume, field_0x0094);
    volume = gabi::fmuls_ppc(volume, field_0x0098);
    volume = gabi::fmuls_ppc(volume, field_0x009c);
    return gabi::fmuls_ppc(volume, field_0x00ac);
}
f32 JAIZelBasic::calcMainBgmVolWithoutFanfare() {
    f32 volume = gabi::fmuls_ppc(field_0x0080, field_0x0084);
    volume = gabi::fmuls_ppc(volume, field_0x0088);
    volume = gabi::fmuls_ppc(volume, field_0x0090);
    volume = gabi::fmuls_ppc(volume, field_0x0094);
    volume = gabi::fmuls_ppc(volume, field_0x0098);
    volume = gabi::fmuls_ppc(volume, field_0x009c);
    return gabi::fmuls_ppc(volume, field_0x00ac);
}

/* 02021290 */
void JAIZelBasic::initLevObjSE() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mLevelObjectSoundCount = 0;
    for (u32 k = 0; k < 15; k++) {
        LevelObjectSound &sound = mLevelObjectSounds[k];
        sound.soundID = -1;
        sound.instanceCount = 0;
        for (u32 j = 0; j < 20; j++) {
            sound.instances[j].volume = 0.0f;
            sound.instances[j].pan = 0.5f;
            sound.instances[j].surround = 0.0f;
        }
    }
}

/* 020212EC */
void JAIZelBasic::processLevObjSE() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 n = mLevelObjectSoundCount;
    for (u32 i = 0; i < n; i++) {
        LevelObjectSound &sound = mLevelObjectSounds[i];
        f32 l = 0.0f, r = 0.0f, s = 0.0f;
        s32 reverb = 0;
        u32 c = sound.instanceCount;
        for (u32 j = 0; j < c; j++) {
            LevelObjectInstance &instance = sound.instances[j];
            f32 vol = instance.volume;
            f32 lr = instance.pan;
            if (vol > 0.0f)
                reverb = s8(instance.reverb);
            f32 sr = instance.surround;
            f32 ss = gabi::fmuls_ppc(sr, vol);
            f32 ll = gabi::fmuls_ppc(gabi::fsubs_ppc(1.0f, lr), vol);
            f32 rr = gabi::fmuls_ppc(lr, vol);
            if (ll > l)
                l = ll;
            if (rr > r)
                r = rr;
            if (ss > s)
                s = ss;
        }
        f32 vol = fsel(gabi::fsubs_ppc(r, l), r, l);
        f32 pan = 0.5f;
        vol = fsel(gabi::fsubs_ppc(s, vol), s, vol);
        u32 b277 = field_0x0207;
        if (l != 0.0f || r != 0.0f)
            pan = r / gabi::fadds_ppc(l, r);
        if (b277 == 0) {
            seStart(sound.soundID, 0, 0, reverb, 1.0f, vol, pan, s, 1);
            n = mLevelObjectSoundCount;
        }
    }
    initLevObjSE();
}

/* 0202148C */
s32 JAIZelBasic::checkSubBgmPlaying() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return mpSubBgmSound.get() != 0;
}

/* 0202149C */
u32 JAIZelBasic::checkPlayingSubBgmFlag() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    Sound *s = mpSubBgmSound.get();
    if (s == 0)
        return (u32)-1;
    return s->getID();
}

/* 020214B4 */
s32 JAIZelBasic::checkCbPracticePlay() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (checkSubBgmPlaying() == 1 && (checkPlayingSubBgmFlag() == JA_BGM_CB_NEXTYEAR ||
                                      checkPlayingSubBgmFlag() == JA_BGM_CB_NEXTYEAR_W))
        return 1;
    return 0;
}

/* 02021534 */
void JAIZelBasic::cbPracticeProcess() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (field_0x00d0.get() == 0)
        return;
    f32 duck = 1.0f;
    Sound *main;
    if (checkCbPracticePlay() == 0) {
        if (field_0x009c == 1.0f)
            return;
        main = mpMainBgmSound.get();
        field_0x009c = 1.0f;
        if (main == 0)
            return;
    } else {
        void *matrix = cameraMatrix();
        VectorWords *source = field_0x00d0.get();
        // HD: initialize and copy all three words, including NaN payloads, in original order.
        gabi::Local<VectorWords> position;
        position->w[0] = l_defaultPosition.w[0];
        position->w[1] = l_defaultPosition.w[1];
        position->w[2] = l_defaultPosition.w[2];
        if (source == nullptr)
            source = cameraPosition();
        if (source != nullptr) {
            position->w[0] = source->w[0];
            position->w[1] = source->w[1];
            position->w[2] = source->w[2];
        }
        if (matrix != nullptr)
            jaizelTransform(matrix, position.get(), position.get());
        f32 vol = calcPosVolume(position.get(), 2.0f);
        f32 lr = calcPosPanLR(position.get());
        f32 sr = calcPosPanSR(position.get(), 1.0f);
        Sound *sub = mpSubBgmSound.get();
        field_0x00a8 = vol;
        if (sub != 0) {
            sub->setVolume(gabi::fmuls_ppc(gabi::fmuls_ppc(field_0x00a0, field_0x00a4), vol), 0, 0);
            mpSubBgmSound.get()->setPan(lr, 0, 0);
            mpSubBgmSound.get()->setDolby(sr, 0, 0);
        }
        // HD: this scene applies reduced practice-tune ducking.
        if ((s32)field_0x0224 == 0x27)
            duck = gabi::fnmsubs(vol, 0.4f, 1.0f);
        else
            duck = gabi::fsubs_ppc(1.0f, vol);
        main = mpMainBgmSound.get();
        field_0x009c = duck;
        if (main == 0)
            return;
    }
    f32 v = gabi::fmuls_ppc(field_0x0080, field_0x0084);
    v = gabi::fmuls_ppc(v, field_0x0088);
    v = gabi::fmuls_ppc(v, field_0x008c);
    v = gabi::fmuls_ppc(v, field_0x0090);
    v = gabi::fmuls_ppc(v, field_0x0094);
    v = gabi::fmuls_ppc(v, field_0x0098);
    v = gabi::fmuls_ppc(v, duck);
    v = gabi::fmuls_ppc(v, field_0x00ac);
    main->setVolume(v, 0, 0);
}

/* 020217A4 */
s32 JAIZelBasic::checkSeqIDDemoPlaying(u32 id) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (id == 0)
        return 0;
    for (u32 i = 0; i < getParamSeqPlayTrackMax(); i++) {
        SeqUpdateData *info = getPlayTrackInfo(i);
        if (info == 0)
            continue;
        Sound *s = info->mSequence.get();
        if (s == 0)
            continue;
        if (s == mpMainBgmSound.get())
            continue;
        if (s == mpSubBgmSound.get())
            continue;
        if (s->getID() == JA_BGM_SE_SEQUENCE)
            continue;
        if (id == s->getID())
            return 1;
    }
    Sound *main = mpMainBgmSound.get();
    void *mgr = l_soundManager.get();
    Sound *sub = mpSubBgmSound.get();
    void *p = hd_streamPlayer(mgr);
    if (hd_checkStream(p, id, 0, main, sub) != 0)
        return 1;
    return 0;
}

/* 0202189C */
u32 JAIZelBasic::checkDemoFanfarePlaying() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    constexpr u32 ids[] = {JA_BGM_OPEN_BOX,    JA_BGM_ITEM_GET,     JA_BGM_ITEM_GET_S,
                           JA_BGM_GET_SONG,    JA_BGM_GET_HEART,    JA_BGM_GET_PEARL,
                           JA_BGM_BGN_GET_BOX, JA_BGM_TRIFORCE_DEMO};
    for (u32 k = 0; k < 8; k++)
        if (checkSeqIDDemoPlaying(ids[k]) == 1)
            return ids[k];
    // HD: SafeString virtual calls can modify both objects; retain each reload and the bound.
    gabi::Local<SafeString> demo;
    gabi::Local<SafeString> stage;
    demo->__vtbl = &l_safeStringVtable;
    stage->__vtbl = &l_safeStringVtable;
    demo->mStringTop = &l_demo34;
    stage->mStringTop = &l_stageName;
    demo->assureTermination();
    gabi::call_ptr(demo->__vtbl->assureTermination.get(), demo.get());
    auto stageAssure = stage->__vtbl->assureTermination.get();
    GuestByte *demoStart = demo->mStringTop.get();
    gabi::call_ptr(stageAssure, stage.get());
    bool equal = demoStart == stage->mStringTop.get();
    if (!equal) {
        GuestByte *left = demo->mStringTop.get();
        GuestByte *right = stage->mStringTop.get();
        for (u32 i = 0; i < 0x40001; i++) {
            u8 character = left[i];
            if (character != right[i])
                break;
            if (character == 0) {
                equal = true;
                break;
            }
        }
    }
    if (equal && l_demoState == 1)
        return 1;
    return 0;
}

/* 02021B10 */
void JAIZelBasic::hdStreamVolume(u32 fade, f32 v) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    hd_setStreamVolume(hd_streamPlayer(l_soundManager.get()), fade, v);
    hd_setStreamMixVolume(hd_streamPlayer(l_soundManager.get()), fade, v);
}

/* 02021B88 */
void JAIZelBasic::muteMainBgmAll() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    field_0x008c = 0.0f;
    for (u32 i = 0; i < getParamSeqPlayTrackMax(); i++) {
        SeqUpdateData *info = getPlayTrackInfo(i);
        if (info == 0)
            continue;
        Sound *s = info->mSequence.get();
        if (s == 0)
            continue;
        if (s->sequenceKind() != 0)
            continue;
        s->setVolume(calcMainBgmVol(), 1, 0);
    }
    hdStreamVolume(1, gabi::fmuls_ppc(gabi::fmuls_ppc(field_0x0084, field_0x008c), field_0x0098));
}

/* 02021C7C */
void JAIZelBasic::unmuteMainBgmAll() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    Sound *main = mpMainBgmSound.get();
    field_0x008c = 1.0f;
    if (main != 0)
        main->setVolume(calcMainBgmVolWithoutFanfare(), 2, 0);
    for (u32 i = 0; i < getParamSeqPlayTrackMax(); i++) {
        SeqUpdateData *info = getPlayTrackInfo(i);
        if (info == 0)
            continue;
        Sound *s = info->mSequence.get();
        if (s == 0)
            continue;
        if (s->sequenceKind() != 0)
            continue;
        s->setVolume(calcMainBgmVol(), 0x2D, 0);
    }
    hdStreamVolume(0x2D,
                   gabi::fmuls_ppc(gabi::fmuls_ppc(field_0x0084, field_0x008c), field_0x0098));
}

/* 02021DC4 */
void JAIZelBasic::processDemoFanfareMute() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 r = checkDemoFanfarePlaying();
    u32 c9 = field_0x00b9;
    if (r != 0) {
        if (c9 == 0)
            muteMainBgmAll();
        field_0x00b9 = 1;
    } else {
        if (c9 != 0)
            unmuteMainBgmAll();
        field_0x00b9 = 0;
    }
}

/* 02021E30 */
s32 JAIZelBasic::checkBgmPlaying() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mpMainBgmSound.get() != 0 || mpStreamBgmSound.get() != 0)
        return 1;
    return 0;
}

/* 02021E58 */
void JAIZelBasic::demoBgmStop(u32 fade) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    for (u32 i = 0; i < getParamSeqPlayTrackMax(); i++) {
        SeqUpdateData *info = getPlayTrackInfo(i);
        if (info == 0)
            continue;
        Sound *s = info->mSequence.get();
        if (s == 0)
            continue;
        if (s == mpMainBgmSound.get())
            continue;
        if (s == mpSubBgmSound.get())
            continue;
        if (s->sequenceKind() != 0)
            continue;
        s->stop(fade);
    }
    Sound *main = mpMainBgmSound.get();
    void *mgr = l_soundManager.get();
    Sound *sub = mpSubBgmSound.get();
    hd_stopStream(hd_streamPlayer(mgr), fade, main, sub);
}

/* 02021F28 */
void JAIZelBasic::bgmStop(u32 fade, s32 keepSub) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 n = mMainBgmNum;
    Sound *main = mpMainBgmSound.get();
    if (n != JA_BGM_GOMA && n != JA_BGM_GOMA_2)
        mSomeSpecialBGMFlag = 0;
    if (main != 0)
        main->stop(fade);
    Sound *strm = mpStreamBgmSound.get();
    mpMainBgmSound = nullptr;
    mMainBgmNum = (u32)-1;
    if (strm != 0)
        strm->stop(fade);
    mpStreamBgmSound = nullptr;
    mStreamBgmNum = (u32)-1;
    if (keepSub == 0) {
        Sound *sub = mpSubBgmSound.get();
        if (sub != 0) {
            sub->stop(fade);
            mpSubBgmSound = nullptr;
        }
        field_0x00bb = 0;
        mSubBgmNum = (u32)-1;
        field_0x008c = 1.0f;
        field_0x0206 = 0;
    }
    demoBgmStop(fade);
    field_0x1f3c = 0;
    field_0x0032 = 0;
    field_0x00cb = 0xFF;
    field_0x0064 = 0;
    field_0x00cc = 0;
    field_0x0201 = 0;
    field_0x00b8 = 0;
    mLastMinibossSubBGMType = 0;
    field_0x0065 = 0;
}

// clang-format off
/* WWHD glue:
data l_soundManager 1018EC64
data l_towerTempo 10003964
data l_defaultPosition 10003950
data l_safeStringVtable 1000382C
data l_demo34 1000395C
data l_stageName 1047E6B8
data l_demoState 101D6010
ext 0201EBA0 u32 jaizel34::JAIZelBasic::seStart(u32,u32,u32,s32,f32,f32,f32,f32,u32)
ext 0201C360 f32 jaizel34::JAIZelBasic::calcPosVolume(jaizel34::VectorWords*,f32)
ext 0201C0E8 f32 jaizel34::JAIZelBasic::calcPosPanLR(jaizel34::VectorWords*)
ext 0201C228 f32 jaizel34::JAIZelBasic::calcPosPanSR(jaizel34::VectorWords*,f32)
ext 0280B1D8 void jaizel34::Sound::stop(u32)
ext 0280B68C void jaizel34::Sound::setVolume(f32,u32,u32)
ext 0280BA28 void jaizel34::Sound::setPan(f32,u32,u32)
ext 0280C248 void jaizel34::Sound::setDolby(f32,u32,u32)
ext 0280D2D8 u32 JAISound::getID()
ext 0280470C u32 getParamSeqPlayTrackMax()
ext 0280AD2C jaizel34::SeqUpdateData* getPlayTrackInfo(u32)
ext 0202BF78 void* hd_streamPlayer(void*)
ext 0202FD40 s32 hd_checkStream(void*,u32,u32,jaizel34::Sound*,jaizel34::Sound*)
ext 0202FEF4 void hd_stopStream(void*,u32,jaizel34::Sound*,jaizel34::Sound*)
ext 0202F1BC void hd_setStreamVolume(void*,u32,f32)
ext 0202F1B4 void hd_setStreamMixVolume(void*,u32,f32)
ext 028E8F64 void jaizelTransform(void*,jaizel34::VectorWords*,jaizel34::VectorWords*)
ext 02029C9C void jaizel34::SafeString::assureTermination()
*/
