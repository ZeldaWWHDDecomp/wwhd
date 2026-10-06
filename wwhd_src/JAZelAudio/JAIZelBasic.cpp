/* JAIZelBasic (WWHD), part 1: makeSound .. menuIn (0201D414..0201E74B).
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/JAZelAudio/JAIZelBasic.cpp) where it has bodies; most of the unit is
 * "Nonmatching" there, so those bodies are written from the WWHD code. Verified against cking.rpx.
 *
 * Translation unit 0201D414..02029C9F (127 functions), from the image: JAIZelAtmos ends with its
 * SafeString copies (0201D3FC/0201D410); JAIZelBasic runs makeSound .. monsSeStart in the HD order,
 * then its header __sinit (02029B88, rodata 10003A00 after the unit's constants 10003894..100039F0),
 * then GHS's trailing copies (02029C1C..02029C9C, including getMapInfoFxParameter/Ground).
 * The sinit-only data TUs (CharVoiceTable etc.) follow at 02029CA0.
 * Parts: JAIZelBasic.cpp (this), JAIZelBasic_2.cpp ... ; layout notes in jaizel_basic_local.h. */
#include "bindings.h"

namespace JAIZelBasic_cpp {
#include "jaizel_local.h"
#include "jaizel_basic_local.h"

/* 0201D414 JAIZelBasic::makeSound(u32 n) (virtual). HD: the JAIZelSound array is recorded in a
 * 16-entry table {count, pointer} at +0x20E8 (index +0x2168, post-incremented); the result is the
 * table entry at the (re-read) index, so a failed allocation returns the entry's previous array. */
u32 makeSound(u32 self, s32 n) {
    WWHD_FUNC(0x0201D414, u32, self, n);
    u32 idx = ld(self + 0x2168);
    u32 tab = self + 0x20E8;
    u32 entry = tab + idx * 8;
    u32 heap = ld(self + 4);
    if (heap == 0) heap = ld(0x101F9DA8);
    if (n > 0) {
        if (heap == 0) heap = heap_get_l(ld(0x101F8B4C));
        u32 alloc = ld(ld(heap + 0xC) + 0x34);
        u32 p = gabi::call_ptr<u32>(alloc, heap, (u32)n * 0x48, 4);
        s32 cnt = n > 0 ? n : 1;
        for (u32 off = 0; cnt != 0; cnt--, off += 0x48)
            if (p + off != 0) JAIZelSound_ct_l(p + off);
        if (p != 0) {
            st(entry + 4, p);
            st(entry, n);
        }
        idx = ld(self + 0x2168);
        entry = tab + idx * 8;
    }
    u32 r = ld(entry + 4);
    st(self + 0x2168, idx + 1);
    return r;
}
VERIFY(0x0201D414, makeSound);

/* 0201D57C JAIZelBasic::kuroboMotionPlay(u32 id, Vec* pos, u32 info, s8 reverb) */
void kuroboMotionPlay(u32 self, u32 id, u32 pos, u32 info, u32 reverb) {
    WWHD_FUNC(0x0201D57C, void, self, id, pos, info, reverb);
    u32 c = ld8(self + 0x20AC);
    if (c >= 4) return;
    st8(self + 0x20AC, c + 1);
    u32 i;
    for (i = 0; i < 4; i++)
        if (ld(self + 0x20B0 + i * 4) == 0) break;
    if (i == 4) return;
    u32 h = self + 0x20B0 + i * 4;
    JAIBasic_startSoundVec_l(self, id, h, pos, 0, info, 4);
    if (ld(h) != 0) JAISound_setPortData_l(ld(h), 9, reverb & 0xFFFF);
}
VERIFY(0x0201D57C, kuroboMotionPlay);

/* 0201D62C JAIZelBasic::checkSePlaying(u32 id) (unnamed by the matcher): 32 SE slots in HD */
s32 checkSePlaying(u32 self, u32 id) {
    WWHD_FUNC(0x0201D62C, s32, self, id);
    for (u32 i = 0; i < 32; i++)
        if (ld(self + 0x164 + i * 4) == id && ld(self + 0xE4 + i * 4) != 0) return 1;
    return 0;
}
VERIFY(0x0201D62C, checkSePlaying);

/* 0201D670 (HD-only, unnamed): limits instances of one sound effect. A playing instance at the same
 * position is stopped (0); one within maxDist whose priority (+0x14) is <= prio blocks the new one (1).
 * With maxCount > 0 and at least maxCount instances playing, the one with the highest priority is
 * stopped (0) unless no priority is positive (1). */
s32 seLimitInstances(u32 self, u32 id, u32 pos, s32 prio, s32 maxCount, f32 maxDist) {
    WWHD_FUNC(0x0201D670, s32, self, id, pos, prio, maxCount, maxDist);
    if (pos == 0) return 0;
    if (ld(ld(self)) == 0) return 0;
    gabi::Local<Vec_l> tmp;
    u32 t = gabi::ea(tmp.get());
    for (u32 i = 0; i < 32; i++) {
        if (ld(self + 0x164 + i * 4) != id) continue;
        u32 s = ld(self + 0xE4 + i * 4);
        if (s == 0) continue;
        u32 sp = ld(s + 0x24);
        if (sp == 0) continue;
        if (sp == pos) {
            JAISound_stop_l(s, 1);
            return 0;
        }
        st(t, ld(sp));
        st(t + 4, ld(sp + 4));
        st(t + 8, ld(sp + 8));
        PSVECSubtract_l(sp, pos, t);
        if (PSVECMag_l(t) > maxDist) continue;
        if ((s32)ld(ld(self + 0xE4 + i * 4) + 0x14) <= prio) return 1;
    }
    if (maxCount <= 0) return 0;
    s32 cnt = 0, best = -1, bestv = -1;
    for (u32 k = 0; k < 32; k++) {
        if (ld(self + 0x164 + k * 4) != id) continue;
        u32 s = ld(self + 0xE4 + k * 4);
        if (s == 0) continue;
        s32 v = ld(s + 0x14);
        if (v > bestv) {
            best = k;
            bestv = v;
        }
        cnt++;
    }
    if (cnt < maxCount) return 0;
    if (bestv <= 0) return 1;
    u32 s = ld(self + 0xE4 + best * 4);
    if (s != 0) JAISound_stop_l(s, 1);
    return 0;
}
VERIFY(0x0201D670, seLimitInstances);

/* 0201D84C JAIZelBasic::stopBattleBgm() */
void stopBattleBgm(u32 self) {
    WWHD_FUNC(0x0201D84C, void, self);
    u32 s = ld(self + 0x7C);
    if (s == 0) return;
    u32 n = ld(self + 0x84);
    if (n != 0x80000004 && n != 0x8000001C) return;
    JAISound_stop_l(s, 45);
    st(self + 0x7C, 0);
    st(self + 0x84, (u32)-1);
}
VERIFY(0x0201D84C, stopBattleBgm);

/* 0201D8BC JAIZelBasic::subBgmStop(): starts the fade-out countdown (+0xC0) for most sub bgms */
void subBgmStop(u32 self) {
    WWHD_FUNC(0x0201D8BC, void, self);
    u32 n = ld(self + 0x84);
    bool keep;
    if (n >= 0x80000030)
        keep = n == 0x80000061 || n == 0x8000005D || n == 0x8000004F || n <= 0x80000032;
    else
        keep = n == 0x80000027 || (n <= 0x80000025 && (n >= 0x80000024 || n == 0x80000009 || n == 0x80000002));
    if (keep) return;
    u32 m = ld(self + 0x84);
    st(self + 0xC0, 0xF);
    st8(self + 0xC6, 0);
    if (m == 0x80000019 || m == 0x8000001A) st8(self + 0xDE, 0x14);
}
VERIFY(0x0201D8BC, subBgmStop);

/* 0201D9C8 JAIZelBasic::subBgmStopInner() */
void subBgmStopInner(u32 self) {
    WWHD_FUNC(0x0201D9C8, void, self);
    if ((s32)ld(self + 0xC0) == -1) return;
    u32 n = ld(self + 0x84);
    u32 s = ld(self + 0x7C);
    if (n == 0x80000019 || n == 0x8000001A) st8(self + 0xDE, 0x14);
    st8(self + 0x74, 0);
    st8(self + 0xC6, 0);
    if (s != 0) {
        JAISound_stop_l(s, 45);
    } else if ((s32)ld(self + 0x84) == -1) {
        return;
    }
    u32 main = ld(self + 0x78);
    st(self + 0x84, (u32)-1);
    stf(self + 0x90, 1.0f);
    st(self + 0x7C, 0);
    stf(self + 0x9C, 1.0f);
    if (main != 0) {
        /* calcMainBgmVol() with +0x90 known to be 1.0 */
        f32 v = mul(ldf(self + 0x94), ldf(self + 0x98));
        v = mul(v, ldf(self + 0x9C));
        v = mul(v, ldf(self + 0xA0));
        v = mul(v, ldf(self + 0xA4));
        v = mul(v, ldf(self + 0xA8));
        v = mul(v, ldf(self + 0xAC));
        v = mul(v, ldf(self + 0xBC));
        JAISound_setVolume_l(main, v, 45, 0);
    }
    st(self + 0xD4, 0);
    st8(self + 0xD1, 0);
    st(self + 0xC0, (u32)-1);
}
VERIFY(0x0201D9C8, subBgmStopInner);

/* 0201DB08 JAIZelBasic::checkStreamPlaying(u32 id): HD asks its sound manager (*1018EC64) */
s32 checkStreamPlaying(u32 self, u32 id) {
    WWHD_FUNC(0x0201DB08, s32, self, id);
    u32 p = hdsnd_player_l(ld(0x1018EC64));
    return hdsnd_check_l(p, id, 1, 0, 0);
}
VERIFY(0x0201DB08, checkStreamPlaying);

/* 0201DB50 JAIZelBasic::talkOut(u32 fade) (HD: the fade time is a parameter) */
void talkOut(u32 self, u32 fade) {
    WWHD_FUNC(0x0201DB50, void, self, fade);
    if (checkStreamPlaying(self, 0xC0000006) == 1) return;
    u32 main = ld(self + 0x78);
    f32 def = ldf(0x1018DC68); /* JAIZelParam::VOL_BGM_DEFAULT */
    stf(self + 0x94, def);
    if (main != 0) {
        f32 v = mul(ldf(self + 0x90), def);
        v = mul(v, ldf(self + 0x98));
        v = mul(v, ldf(self + 0x9C));
        v = mul(v, ldf(self + 0xA0));
        v = mul(v, ldf(self + 0xA4));
        v = mul(v, ldf(self + 0xA8));
        v = mul(v, ldf(self + 0xAC));
        v = mul(v, ldf(self + 0xBC));
        JAISound_setVolume_l(main, v, fade, 0);
    }
    u32 sub = ld(self + 0x7C);
    f32 def2 = ldf(0x1018DC68);
    stf(self + 0xB0, def2);
    if (sub != 0) {
        f32 v = mul(def2, ldf(self + 0xB4));
        v = mul(v, ldf(self + 0xB8));
        JAISound_setSeqInterVolume_l(sub, 0, v, fade);
    }
    for (u32 c = 0; c < 8; c++) JAIBasic_setSeCategoryVolume_l(self, c, ld8(0x1018DC97 + c));
}
VERIFY(0x0201DB50, talkOut);

/* 0201DCD4 JAIZelBasic::menuOut() */
void menuOut(u32 self) {
    WWHD_FUNC(0x0201DCD4, void, self);
    stf(self + 0x98, ldf(0x1018DC68));
    talkOut(self, 2);
    st8(self + 0x30, 0);
}
VERIFY(0x0201DCD4, menuOut);

/* 0201DD18 JAIZelBasic::bgmMute(JAISound** h, u32 bgm, s32 set, u32 fade): track mute mask from
 * JAIZelBasic::m_bgm_mute_state (1018DCB8, 4 masks per bgm) */
void bgmMute(u32 self, u32 h, u32 bgm, u32 set, u32 fade) {
    WWHD_FUNC(0x0201DD18, void, self, h, bgm, set, fade);
    if (h == 0) return;
    u32 mask = ld(0x1018DCB8 + (((bgm & 0xFF) << 2) + set) * 4);
    u32 s = ld(h);
    if (s == 0) return;
    for (u32 t = 0; t < 16; t++) {
        if (t != 0) s = ld(h);
        JAISound_setTrackVolume_l(s, t & 0xFF, (mask & 1) ? 1.0f : 0.0f, fade);
        mask >>= 1;
    }
}
VERIFY(0x0201DD18, bgmMute);

/* 0201DE00 JAIZelBasic::subBgmStart(u32 id) */
void subBgmStart(u32 self, u32 id) {
    WWHD_FUNC(0x0201DE00, void, self, id);
    st8(self + 0xCE, 0);
    if (ld8(self + 0x73) != 0) return;
    if (id == 0xFFFFFFFF) return;
    if (ld8(self + 0xC5) == 1) return;
    /* per-bgm state */
    int kind = 0; /* 1: battle-like (CB=2, 74=1, 276=1), 2..4: C6 = 1..3, 5: 276 = 1 */
    if (id >= 0x80000030) {
        if (id < 0x80000047) {
            if (id == 0x80000046) kind = 3;
            else if (id == 0x80000041) kind = 4;
            else if (id <= 0x80000032) kind = 1;
        } else if (id == 0x80000061 || id == 0x8000005D || id == 0x8000004F) {
            kind = 1;
        } else if (id == 0x80000047) {
            kind = 4;
        }
    } else if (id >= 0x80000024) {
        if (id == 0x8000002B) kind = 3;
        else if (id == 0x80000027) kind = 1;
        else if (id <= 0x80000025) kind = 1;
    } else if (id <= 0x8000001A) {
        if (id >= 0x80000019) kind = 2;
        else if (id == 0x80000009) kind = 5;
        else if (id == 0x80000002) kind = 1;
    }
    switch (kind) {
    case 1:
        st8(self + 0xCB, 2);
        st8(self + 0x74, 1);
        st8(self + 0x276, 1);
        break;
    case 2: st8(self + 0xC6, 1); break;
    case 3: st8(self + 0xC6, 2); break;
    case 4: st8(self + 0xC6, 3); break;
    case 5: st8(self + 0x276, 1); break;
    }
    if (ld(self + 0x7C) != 0 && id == ld(self + 0x84)) {
        st(self + 0xC0, (u32)-1);
        return;
    }
    /* no fade-in for these */
    bool quick;
    if (id < 0x80000027)
        quick = id <= 0x80000025 && (id >= 0x80000024 || id == 0x80000019 || id == 0x80000009 || id == 0x80000002);
    else if (id < 0x80000041)
        quick = id <= 0x80000032 && (id >= 0x80000030 || id == 0x8000002B || id == 0x80000027);
    else
        quick = id == 0x80000061 || id == 0x8000005D || id == 0x8000004F || id == 0x80000041;
    JAIBasic_startSoundVec_l(self, id, self + 0x7C, 0, quick ? 0 : 0x3C, 0, 4);
    if (ld(self + 0x7C) != 0) {
        bool ok = true;
        if (id != 0x8000005D) {
            JAISound_setSeqInterVolume_l(ld(self + 0x7C), 0, ldf(self + 0xB0), 0);
            ok = ld(self + 0x7C) != 0;
        }
        if (ok) {
            if (id == 0x8000001C) {
                if (ld8(self + 0x57) == 1) bgmMute(self, self + 0x7C, id, 0, 0);
                else bgmMute(self, self + 0x7C, id, 1, 0);
            } else if (id <= 0x8000001A && id >= 0x80000019) {
                bgmMute(self, self + 0x7C, id, 0, 0);
                if (ld8(self + 0xD9) != 0)
                    JAISound_setTrackVolume_l(ld(self + 0x7C), ld8(0x1018DCB7), 1.0f, 0);
            }
        }
    }
    u32 main = ld(self + 0x78);
    stf(self + 0x9C, 0.0f);
    if (main != 0) {
        bool quick2;
        if (id < 0x80000027)
            quick2 = id <= 0x80000025 && (id >= 0x80000024 || id == 0x80000019 || id == 0x80000009 || id == 0x80000002);
        else if (id < 0x80000041)
            quick2 = id == 0x80000036 || id == 0x80000030 || id == 0x8000002B || id == 0x80000027;
        else
            quick2 = id == 0x80000061 || id == 0x8000005D || id == 0x8000004F || id == 0x80000041;
        f32 v = mul(ldf(self + 0x90), ldf(self + 0x94));
        v = mul(v, ldf(self + 0x98));
        v = mul(v, 0.0f);
        v = mul(v, ldf(self + 0xA0));
        v = mul(v, ldf(self + 0xA4));
        v = mul(v, ldf(self + 0xA8));
        v = mul(v, ldf(self + 0xAC));
        v = mul(v, ldf(self + 0xBC));
        JAISound_setVolume_l(main, v, quick2 ? 1 : 0x3C, 0);
    }
    st(self + 0x84, id);
    st(self + 0xC0, (u32)-1);
}
VERIFY(0x0201DE00, subBgmStart);

/* 0201E480 JAIZelBasic::checkEventBit(u16) */
s32 checkEventBit(u32 self, u32 flag) {
    WWHD_FUNC(0x0201E480, s32, self, flag);
    u32 ev = ld(self + 0x34);
    if (ev == 0) return 0;
    return (ld8(ev + ((flag >> 8) & 0xFF)) & flag & 0xFF) != 0;
}
VERIFY(0x0201E480, checkEventBit);

/* 0201E4AC JAIZelBasic::onEnemyDamage() */
void onEnemyDamage(u32 self) {
    WWHD_FUNC(0x0201E4AC, void, self);
    u32 n = ld(self + 0x84);
    if (n == 0x8000001C) {
        u32 s = ld(self + 0x7C);
        if (s != 0) JAISound_setPortData_l(s, 9, 1);
    } else if (n == 0x80000004) {
        u32 s = ld(self + 0x7C);
        if (s != 0) {
            u32 track = JAISound_getSeqParameter_l(s) + 0x1360; /* getRootTrackPointer() */
            if (track != 0) TTrack_writePortApp_l(track, 0x000B0000, 1);
        }
    }
}
VERIFY(0x0201E4AC, onEnemyDamage);

/* 0201E538 JAIZelBasic::seStop(u32 id, s32 fade) */
void seStop(u32 self, u32 id, u32 fade) {
    WWHD_FUNC(0x0201E538, void, self, id, fade);
    if (id == 0xFFFFFFFF) return;
    if (id == 0x280D) fade = 8;
    for (u32 i = 0; i < 32; i++) {
        if (ld(self + 0x164 + i * 4) != id) continue;
        u32 s = ld(self + 0xE4 + i * 4);
        if (s == 0) continue;
        JAISound_stop_l(s, fade);
        st(self + 0xE4 + i * 4, 0);
        st(self + 0x164 + i * 4, 0);
        st(self + 0x1E4 + i * 4, 0);
    }
}
VERIFY(0x0201E538, seStop);

/* 0201E5CC JAIZelBasic::isDemo(): HD reads a global demo state */
s32 isDemo(u32 self) {
    WWHD_FUNC(0x0201E5CC, s32, self);
    return ld(0x101D6010) != 0;
}
VERIFY(0x0201E5CC, isDemo);

/* 0201E5E0 JAIZelBasic::menuIn() */
void menuIn(u32 self) {
    WWHD_FUNC(0x0201E5E0, void, self);
    u32 main = ld(self + 0x78);
    f32 p = ldf(0x1018DC70); /* JAIZelParam::VOL_BGM_PAUSING */
    stf(self + 0x98, p);
    if (main != 0) {
        f32 v = mul(ldf(self + 0x90), ldf(self + 0x94));
        v = mul(v, p);
        v = mul(v, ldf(self + 0x9C));
        v = mul(v, ldf(self + 0xA0));
        v = mul(v, ldf(self + 0xA4));
        v = mul(v, ldf(self + 0xA8));
        v = mul(v, ldf(self + 0xAC));
        v = mul(v, ldf(self + 0xBC));
        JAISound_setVolume_l(main, v, 2, 0);
    }
    u32 sub = ld(self + 0x7C);
    f32 p2 = ldf(0x1018DC70);
    stf(self + 0xB0, p2);
    if (sub != 0) {
        f32 v = mul(p2, ldf(self + 0xB4));
        v = mul(v, ldf(self + 0xB8));
        JAISound_setSeqInterVolume_l(sub, 0, v, 2);
    }
    for (u32 c = 0; c < 8; c++) JAIBasic_setSeCategoryVolume_l(self, c, ld8(0x1018DCA7 + c));
    st8(self + 0x30, 1);
}
VERIFY(0x0201E5E0, menuIn);

} // namespace JAIZelBasic_cpp
