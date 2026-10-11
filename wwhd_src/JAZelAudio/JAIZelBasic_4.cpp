/** JAIZelBasic BGM selection and track variants.
 * GameCube bgmStart is nonmatching; this preserves the verified HD implementation. */
#include "JAZelAudio/JAIZelBasic_3.h"
using namespace jaizel34;
namespace {
enum BgmID : u32 {
    JA_BGM_ISLAND_LINK = 0x80000001,
    JA_BGM_GOMA = 0x80000003,
    JA_BGM_KINDAN_BOSS = 0x80000005,
    JA_BGM_I_MAJU = 0x80000007,
    JA_BGM_DIE_LINK = 0x8000000A,
    JA_BGM_ISLAND_LINK_2 = 0x8000000E,
    JA_BGM_I_MAJU_JAIL = 0x8000000F,
    JA_BGM_GOMA_2 = 0x80000010,
    JA_BGM_DK_BATTLE = 0x80000014,
    JA_BGM_MJ_TOWER_BATTLE = 0x80000015,
    JA_BGM_SELECT = 0x8000001E,
    JA_BGM_TAKT_MEDRI = 0x80000022,
    JA_BGM_BST_BATTLE = 0x80000023,
    JA_BGM_KAMI_TOWER = 0x80000028,
    JA_BGM_RANE_BATTLE = 0x80000029,
    JA_BGM_SEA = 0x8000002E,
    JA_BGM_ISLAND_LINK_0 = 0x80000038,
    JA_BGM_SEA_STORM = 0x8000003C,
    JA_BGM_D_GANON_1 = 0x80000042,
    JA_BGM_D_GANON_2 = 0x80000043,
    JA_BGM_BIG_POW = 0x80000048,
    JA_BGM_PAST_BIG_POW = 0x80000049,
    JA_BGM_PAST_BKM = 0x8000004A,
    JA_BGM_PAST_RANE = 0x8000004C,
    JA_BGM_BIRDMAN_GOAL = 0x80000051,
    JA_BGM_BIRDMAN_FAIL = 0x80000052,
    JA_BGM_UNK_105 = 0x80000105,
    JA_BGM_UNK_110 = 0x80000110,
    JA_BGM_UNK_111 = 0x80000111,
    JA_BGM_UNK_112 = 0x80000112,
    JA_BGM_UNK_115 = 0x80000115,
    JA_BGM_UNK_120 = 0x80000120,
    JA_BGM_UNK_121 = 0x80000121,
    JA_BGM_UNK_122 = 0x80000122,
    JA_BGM_UNK_123 = 0x80000123,
    JA_BGM_UNK_124 = 0x80000124,
    JA_BGM_UNK_125 = 0x80000125,
    JA_BGM_UNK_126 = 0x80000126,
    JA_BGM_UNK_130 = 0x80000130,
    JA_BGM_UNK_131 = 0x80000131,
    JA_BGM_UNK_132 = 0x80000132,
    JA_BGM_UNK_133 = 0x80000133,
    JA_BGM_UNK_140 = 0x80000140,
    JA_BGM_UNK_150 = 0x80000150,
    JA_BGM_UNK_151 = 0x80000151,
    JA_BGM_UNK_152 = 0x80000152,
    JA_BGM_SE_SEQUENCE = 0x80000800,
};
}
/* 0202204C */
void JAIZelBasic::bgmStart(u32 id, u32 fade, u32 keepSub) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    field_0x00be = 0;
    if (field_0x0063 != 0)
        return;
    if (field_0x0066 != 0) {
        if (id != JA_BGM_SELECT)
            return;
    } else {
        if (id == JA_BGM_SE_SEQUENCE)
            return;
        if (id == 0)
            return;
        if (id == 0xFFFFFFFF)
            return;
        if (id == 0xC0000000)
            return;
    }
    const u32 requestedID = id;
    if (mMainBgmNum == JA_BGM_I_MAJU_JAIL && id == JA_BGM_I_MAJU)
        return;
    enum SubBgmPolicy { ClearSubBgm, KeepSubBgm, PreserveSubBgmPolicy };
    SubBgmPolicy subBgmPolicy = ClearSubBgm;
    switch (id) {
    case JA_BGM_DIE_LINK:
        field_0x00ca = 1;
        subBgmPolicy = PreserveSubBgmPolicy;
        break;
    case JA_BGM_I_MAJU:
        mNextSceneBgmId = id;
        subBgmPolicy = PreserveSubBgmPolicy;
        break;
    case JA_BGM_GOMA:
    case JA_BGM_GOMA_2:
        subBgmPolicy = KeepSubBgm;
        break;
    case JA_BGM_KINDAN_BOSS:
    case JA_BGM_UNK_105:
        id = JA_BGM_KINDAN_BOSS;
        subBgmPolicy = KeepSubBgm;
        break;
    case JA_BGM_DK_BATTLE:
    case JA_BGM_UNK_110:
    case JA_BGM_UNK_111:
    case JA_BGM_UNK_112:
        id = JA_BGM_DK_BATTLE;
        subBgmPolicy = KeepSubBgm;
        break;
    case JA_BGM_MJ_TOWER_BATTLE:
    case JA_BGM_UNK_115:
        id = JA_BGM_MJ_TOWER_BATTLE;
        subBgmPolicy = KeepSubBgm;
        break;
    case JA_BGM_BST_BATTLE:
    case JA_BGM_UNK_120:
    case JA_BGM_UNK_121:
    case JA_BGM_UNK_122:
    case JA_BGM_UNK_123:
        id = JA_BGM_BST_BATTLE;
        subBgmPolicy = KeepSubBgm;
        break;
    case JA_BGM_RANE_BATTLE:
    case JA_BGM_UNK_124:
        id = JA_BGM_RANE_BATTLE;
        subBgmPolicy = KeepSubBgm;
        break;
    case JA_BGM_KAMI_TOWER:
    case JA_BGM_UNK_125:
    case JA_BGM_UNK_126:
        id = JA_BGM_KAMI_TOWER;
        subBgmPolicy = PreserveSubBgmPolicy;
        break;
    case JA_BGM_D_GANON_1:
    case JA_BGM_UNK_130:
    case JA_BGM_UNK_131:
    case JA_BGM_UNK_132:
    case JA_BGM_UNK_133:
        id = JA_BGM_D_GANON_1;
        subBgmPolicy = PreserveSubBgmPolicy;
        break;
    case JA_BGM_BIG_POW:
    case JA_BGM_UNK_140:
        id = JA_BGM_BIG_POW;
        subBgmPolicy = KeepSubBgm;
        break;
    case JA_BGM_PAST_BIG_POW:
    case JA_BGM_UNK_150:
        id = JA_BGM_PAST_BIG_POW;
        subBgmPolicy = KeepSubBgm;
        break;
    case JA_BGM_PAST_BKM:
    case JA_BGM_UNK_151:
        id = JA_BGM_PAST_BKM;
        subBgmPolicy = KeepSubBgm;
        break;
    case JA_BGM_PAST_RANE:
    case JA_BGM_UNK_152:
        id = JA_BGM_PAST_RANE;
        subBgmPolicy = KeepSubBgm;
        break;
    case 0xC0000006:
        field_0x0064 = 1;
        subBgmPolicy = PreserveSubBgmPolicy;
        break;
    }
    if (subBgmPolicy != PreserveSubBgmPolicy)
        mSomeSpecialBGMFlag = subBgmPolicy;
    const f32 tempo = f32(l_towerTempo); /* 1.3143 */
    Sound *main = mpMainBgmSound.get();
    if (main != 0 && id == mMainBgmNum) {
        gptr<Sound> *h = &mpMainBgmSound;
        switch (id) {
        case JA_BGM_KINDAN_BOSS:
            if (requestedID == JA_BGM_UNK_105)
                bgmMute(h, id, 1, 5);
            else if (requestedID == JA_BGM_KINDAN_BOSS)
                bgmMute(h, id, 0, 5);
            return;
        case JA_BGM_DK_BATTLE:
            if (requestedID > JA_BGM_UNK_112)
                return;
            if (requestedID == JA_BGM_UNK_112)
                bgmMute(h, id, 3, 5);
            else if (requestedID == JA_BGM_UNK_111)
                bgmMute(h, id, 2, 5);
            else if (requestedID == JA_BGM_UNK_110)
                bgmMute(h, id, 1, 5);
            else if (requestedID == JA_BGM_DK_BATTLE)
                bgmMute(h, id, 0, 5);
            return;
        case JA_BGM_MJ_TOWER_BATTLE:
            if (requestedID == JA_BGM_UNK_115)
                bgmMute(h, id, 1, 5);
            else if (requestedID == JA_BGM_MJ_TOWER_BATTLE)
                bgmMute(h, id, 0, 5);
            return;
        case JA_BGM_BST_BATTLE:
            if (requestedID >= JA_BGM_UNK_121) {
                if (requestedID > JA_BGM_UNK_123)
                    return;
                if (requestedID == JA_BGM_UNK_123)
                    bgmMute(h, JA_BGM_TAKT_MEDRI, 3, 5);
                else if (requestedID == JA_BGM_UNK_122)
                    bgmMute(h, id, 3, 5);
                else
                    bgmMute(h, id, 2, 5);
            } else if (requestedID == JA_BGM_UNK_120) {
                bgmMute(h, id, 1, 5);
            } else if (requestedID == JA_BGM_BST_BATTLE) {
                bgmMute(h, id, 0, 5);
            }
            return;
        case JA_BGM_D_GANON_1:
            if (requestedID >= JA_BGM_UNK_131) {
                if (requestedID > JA_BGM_UNK_133)
                    return;
                if (requestedID == JA_BGM_UNK_133)
                    bgmMute(h, JA_BGM_D_GANON_2, 3, 0x3C);
                else if (requestedID == JA_BGM_UNK_132)
                    bgmMute(h, JA_BGM_D_GANON_1, 3, 0x3C);
                else
                    bgmMute(h, JA_BGM_D_GANON_1, 2, 0x3C);
            } else if (requestedID == JA_BGM_UNK_130) {
                bgmMute(h, JA_BGM_D_GANON_1, 1, 0x3C);
            } else if (requestedID == JA_BGM_D_GANON_1) {
                bgmMute(h, JA_BGM_D_GANON_1, 0, 0x3C);
            }
            return;
        case JA_BGM_RANE_BATTLE:
        case JA_BGM_BIG_POW:
        case JA_BGM_PAST_BIG_POW:
        case JA_BGM_PAST_BKM:
        case JA_BGM_PAST_RANE: {
            u32 variantID = id == JA_BGM_RANE_BATTLE    ? JA_BGM_UNK_124
                            : id == JA_BGM_BIG_POW      ? JA_BGM_UNK_140
                            : id == JA_BGM_PAST_BIG_POW ? JA_BGM_UNK_150
                            : id == JA_BGM_PAST_BKM     ? JA_BGM_UNK_151
                                                        : JA_BGM_UNK_152;
            if (requestedID == variantID)
                bgmMute(h, id, 1, 5);
            else if (requestedID == id)
                bgmMute(h, id, 0, 5);
            return;
        }
        case JA_BGM_KAMI_TOWER:
            if (requestedID == JA_BGM_UNK_126) {
                bgmMute(h, JA_BGM_KAMI_TOWER, 2, 0x3C);
                mpMainBgmSound.get()->setTempoProportion(1.0f, 0x3C);
            } else if (requestedID == JA_BGM_UNK_125) {
                bgmMute(h, JA_BGM_KAMI_TOWER, 1, 0x3C);
                mpMainBgmSound.get()->setTempoProportion(tempo, 0x3C);
            } else if (requestedID == JA_BGM_KAMI_TOWER) {
                bgmMute(h, JA_BGM_KAMI_TOWER, 0, 0x3C);
                mpMainBgmSound.get()->setTempoProportion(1.0f, 0x3C);
            }
            return;
        }
    }
    /* start the new bgm */
    bgmStop(0, keepSub);
    startSoundVec(id, &mpMainBgmSound, 0, fade, 0, 4);
    if ((s32)field_0x0224 == 0x12 && mIslandRoomNo == 0x29) {
        // HD: Forest Haven fades its BGM near this point.
        VectorWords *cp = cameraPosition();
        if (cp != 0) {
            auto position = reinterpret_cast<cXyz *>(cp);
            f32 dz = position->z - 198000.0f;
            f32 dx = position->x - 195000.0f;
            f32 d = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
            f32 v = 1.0f;
            if (d > 8000.0f) {
            } else if (d > 5000.0f) {
                v = (d - 5000.0f) / 3000.0f;
            } else {
                v = 0.0f;
            }
            if (mMainBgmNum == JA_BGM_DIE_LINK)
                v = 1.0f;
            field_0x0090 = v;
        }
    }
    if (id == JA_BGM_DIE_LINK) {
        field_0x0090 = 1.0f;
        field_0x008c = 1.0f;
    } else {
        f32 v = gabi::fmuls_ppc(field_0x0080, field_0x0084);
        v = gabi::fmuls_ppc(v, field_0x0088);
        v = gabi::fmuls_ppc(v, field_0x008c);
        v = gabi::fmuls_ppc(v, field_0x0090);
        v = gabi::fmuls_ppc(v, field_0x0094);
        v = gabi::fmuls_ppc(v, field_0x0098);
        v = gabi::fmuls_ppc(v, field_0x009c);
        v = gabi::fmuls_ppc(v, field_0x00ac);
        if (v != 1.0f && id != JA_BGM_BIRDMAN_GOAL && id != JA_BGM_BIRDMAN_FAIL) {
            Sound *m = mpMainBgmSound.get();
            if (m != 0)
                m->setVolume(v, 0, 0);
        }
    }
    // HD: variants select track-mute sets, with a separate tempo for the tower.
    gptr<Sound> *h = &mpMainBgmSound;
    switch (requestedID) {
    case JA_BGM_KINDAN_BOSS:
    case JA_BGM_DK_BATTLE:
    case JA_BGM_MJ_TOWER_BATTLE:
    case JA_BGM_BST_BATTLE:
    case JA_BGM_D_GANON_1:
    case JA_BGM_BIG_POW:
    case JA_BGM_PAST_BIG_POW:
    case JA_BGM_PAST_BKM:
    case JA_BGM_PAST_RANE:
        bgmMute(h, id, 0, 0);
        break;
    case JA_BGM_KAMI_TOWER:
    case JA_BGM_RANE_BATTLE:
        bgmMute(h, id, 0, 0);
        break;
    case JA_BGM_SEA:
    case JA_BGM_SEA_STORM:
        if (mIsSailing == 0 && field_0x1f40 < 0.2f)
            bgmMute(h, requestedID, 1, 0);
        else
            bgmMute(h, requestedID, 0, 0);
        break;
    case JA_BGM_UNK_105:
    case JA_BGM_UNK_110:
    case JA_BGM_UNK_115:
    case JA_BGM_UNK_120:
    case JA_BGM_UNK_124:
    case JA_BGM_UNK_130:
    case JA_BGM_UNK_140:
    case JA_BGM_UNK_150:
    case JA_BGM_UNK_151:
    case JA_BGM_UNK_152:
        bgmMute(h, id, 1, 0);
        break;
    case JA_BGM_UNK_111:
    case JA_BGM_UNK_121:
    case JA_BGM_UNK_126:
    case JA_BGM_UNK_131:
        bgmMute(h, id, 2, 0);
        break;
    case JA_BGM_UNK_112:
    case JA_BGM_UNK_122:
    case JA_BGM_UNK_132:
        bgmMute(h, id, 3, 0);
        break;
    case JA_BGM_UNK_123:
        bgmMute(h, id - 1, 3, 0);
        break;
    case JA_BGM_UNK_133:
        bgmMute(h, id + 1, 3, 0);
        break;
    case JA_BGM_UNK_125: {
        bgmMute(h, id, 1, 0);
        Sound *m = mpMainBgmSound.get();
        if (m != 0)
            m->setTempoProportion(tempo, 0);
        break;
    }
    }
    mMainBgmNum = id;
    if (id == JA_BGM_ISLAND_LINK_0 || id == JA_BGM_ISLAND_LINK || id == JA_BGM_ISLAND_LINK_2)
        field_0x1f3c = 1;
}

// clang-format off
/* WWHD glue:
ext 0201DD18 void jaizel34::JAIZelBasic::bgmMute(gptr<jaizel34::Sound>*,u32,u32,u32)
ext 02802848 void jaizel34::JAIZelBasic::startSoundVec(u32,gptr<jaizel34::Sound>*,u32,u32,u32,u32)
ext 0280C320 void jaizel34::Sound::setTempoProportion(f32,u32)
ext 028F4384 f32 std_sqrtf(f32)
*/
