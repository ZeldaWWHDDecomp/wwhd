/* JAIZelAnime (WWHD): Zelda's animation-frame sound hooks (JAIAnimeSound subclass).
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/JAZelAudio/JAIZelAnime.cpp) and verified against cking.rpx.
 *
 * Translation unit 0201B71C..0201C0E7 (6 functions), from the image: s_basic ends with its
 * header __sinit (0201B688); JAIZelAnime runs startAnimSound .. setPlayPosition, the HD
 * initActorAnimSound override (0201C020), then its own header __sinit (0201C054, rodata
 * 10003798 after the unit's constants 10003760..10003790). JAIZelAtmos follows (0201C0E8).
 *
 * Layout (HD): JAIZelBasic fields used here: +0x38 (GC 0x28), +0x55/+0x56 (GC 0x45/0x46),
 * +0x271 (GC 0x201), +0x276/+0x277 (GC 0x206/0x207), +0x294 (GC 0x224), +0x2A8/+0x2A9
 * (GC 0x238/0x239); the audio camera array pointer is at +0 (its matrix pointer at +8).
 * JAIZelAnime: +0x80 dataCounter, +0x88 current time, +0x90 the anime sound data,
 * +0x98 an HD-only flag (cleared by initActorAnimSound, tested for sound 0x588D).
 * JAInter::Actor: +4 position pointer, +0xC info word (top byte = reverb). */
#include "bindings.h"

namespace JAIZelAnime_cpp {
#include "jaizel_local.h"

/* 0201B71C JAIZelAnime::startAnimSound(void* basic, u32 id, JAISound** sound, JAInter::Actor* actor, u8) */
void startAnimSound(u32 self, u32 basic, u32 id, u32 sound, u32 actor, u32 flag) {
    WWHD_FUNC(0x0201B71C, void, self, basic, id, sound, actor, flag);
    if (ld8(basic + 0x277) != 0) return;
    if ((s32)ld(basic + 0x294) == 0x12 && ld8(basic + 0x2A8) == 0x2C && ld8(basic + 0x2A9) == 9 && ld(basic + 0x38) < 30)
        return;
    switch (id) {
    case 0x383C:
    case 0x48D0:
    case 0x48D1:
    case 0x48D2:
    case 0x58F3:
        /* mDoAud_seStart(id, actor->pos) */
        JAIZelBasic_seStart_l(zel_basic(), id, ld(actor + 4), 0, 0, 1.0f, 1.0f, -1.0f, -1.0f, 0);
        return;
    }
    if (ld8(basic + 0x271) == 1) return;
    if (ld8(basic + 0x276) == 1) return;
    if (JAIZelBasic_checkStreamPlaying_l(basic, 0xC0000003) || JAIZelBasic_checkStreamPlaying_l(basic, 0xC0000019) ||
        JAIZelBasic_checkStreamPlaying_l(basic, 0xC000001A) || JAIZelBasic_checkStreamPlaying_l(basic, 0xC000001B)) {
        if (id == 0x3800 || id == 0x3801) return;
    }
    if (ld8(basic + 0x56) == 1) {
        switch (id) {
        case 0x3800:
        case 0x3815:
        case 0x3818:
            id = 0x382F;
            break;
        case 0x3801:
            id = 0x3830;
            break;
        }
    } else if (id == 0x382F) {
        id = 0x3800;
    }
    if (JAIZelBasic_checkStreamPlaying_l(basic, 0xC0000014)) {
        switch (ld(actor + 0xC)) {
        case 0x13:
        case 0x14:
        case 0x16:
        case 0x18:
            return;
        }
    }
    u32 info = ld(actor + 0xC);
    u32 reverb = info >> 24;
    st(actor + 0xC, info & 0xFFFFFF);
    if (ld(actor + 4) != 0) {
        u32 cam = ld(basic);
        u32 pos = ld(actor + 4);
        gabi::Local<Vec_l> v;
        u32 sp = gabi::ea(v.get());
        st(sp, ld(pos));
        st(sp + 4, ld(pos + 4));
        st(sp + 8, ld(pos + 8));
        PSMTXMultVec_l(ld(cam + 8), sp, sp);
        f32 y = ldf(sp + 4), x = ldf(sp), z = ldf(sp + 8);
        f32 dist = sqrtf_l(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y)));
        if (id < 0x5870) {
            if (id == 0x4805) {
            } else if (id == 0x5800) {
                f32 max = JAIGlobalParameter_getParamDistanceMax_l();
                if (dist > max * 1.0f) return;
            } else if (id < 0x586C) {
                if (dist > JAIGlobalParameter_getParamDistanceMax_l()) return;
            } else if (id <= 0x586E) {
            } else {
                if (dist > JAIGlobalParameter_getParamDistanceMax_l()) return;
            }
        } else if (id == 0x5870) {
        } else if (id == 0x588D) {
            if (ld(self + 0x98) == 1) {
                f32 max = JAIGlobalParameter_getParamDistanceMax_l();
                if (dist > max * 3.0f) return;
            } else {
                if (dist > JAIGlobalParameter_getParamDistanceMax_l()) return;
            }
        } else if (id == 0x59A3) {
            f32 max = JAIGlobalParameter_getParamDistanceMax_l();
            if (dist > max + max) return;
        } else {
            if (dist > JAIGlobalParameter_getParamDistanceMax_l()) return;
        }
    }
    u32 kind = ld(actor + 0xC);
    if (kind == 0xF) {
        JAIZelBasic_seStart_l(basic, 0x3813, ld(actor + 4), kind, (s8)reverb, 1.0f, 1.0f, -1.0f, -1.0f, 0);
    } else if (kind == 0x10) {
        JAIZelBasic_seStart_l(basic, 0x3812, ld(actor + 4), kind, (s8)reverb, 1.0f, 1.0f, -1.0f, -1.0f, 0);
    }
    if (ld8(basic + 0x55) == 1) {
        switch (id) {
        case 0x3800:
        case 0x3801:
        case 0x3803:
        case 0x3815:
        case 0x3818:
        case 0x382F:
        case 0x3830:
            JAIZelBasic_seStart_l(basic, 0x3831, ld(actor + 4), 0, (s8)reverb, 1.0f, 1.0f, -1.0f, -1.0f, 0);
            break;
        }
    }
    if (ld(sound) != 0) JAISound_stop_l(ld(sound), 0);
    JAIBasic_startSoundActor_l(basic, id, sound, actor, 0, 0);
    if (ld(sound) != 0) JAISound_setPortData_l(ld(sound), 9, reverb);
}
VERIFY(0x0201B71C, startAnimSound);

/* 0201BC14 JAIZelAnime::setSpeedModifySound(JAISound*, JAIAnimeFrameSoundData*, f32)
 * HD: the pitch factor applies to every sound type except 6 and 0x12..0x16. */
void setSpeedModifySound(u32 self, u32 sound, u32 data, f32 rate) {
    WWHD_FUNC(0x0201BC14, void, self, sound, data, rate);
    s32 pf = (s8)ld8(data + 0x15);
    f32 pitch = ldf(data + 0xC);
    if (pf != 0) {
        u32 type = ld(sound + 0x18);
        if (type == 6 || (type >= 0x12 && type <= 0x16)) {
        } else {
            pitch = gabi::fmadds((f32)pf * (rate - 1.0f), 0.03125f, pitch);
        }
    }
    JAISound_setPitch_l(sound, pitch, 0, 5);
    s32 vf = (s8)ld8(data + 0x18);
    s32 vol = ld8(data + 0x14);
    if (rate == 0.0f) vol = 0;
    if (vf != 0) {
        bool zero = false;
        if (JAISound_getID_l(sound) == 0x283E) {
            if (rate < 0.0f) rate = -rate;
            if (rate < 1.2f) {
                zero = true;
            } else {
                vol = (s16)gabi::ftoi((rate - 1.2f) * 35.0f);
            }
        } else {
            f32 d = rate - 1.0f;
            if (ld(sound + 0x18) == 0x13) {
                vol = (s16)(vol + (s16)gabi::ftoi((f32)(s8)ld8(data + 0x18) * d));
            } else {
                f32 f = (f32)(s8)ld8(data + 0x18);
                vol = (s16)(vol + (s16)gabi::ftoi((f + f) * d));
            }
        }
        if (zero) {
            vol = 0;
        } else if (vol > 127) {
            vol = 127;
        } else if (vol < 0) {
            vol = 0;
        }
    }
    if (JAISound_getID_l(sound) == 0x283E) {
        if (ld(sound + 0x14) <= 1) JAISound_setVolume_l(sound, (f32)(u8)vol / 127.0f, 0, 5);
    } else {
        JAISound_setVolume_l(sound, (f32)(u8)vol / 127.0f, 0, 5);
    }
}
VERIFY(0x0201BC14, setSpeedModifySound);

/* 0201BF08 JAIZelAnime::setAnimSound(Vec* pos, f32 frame, f32 rate, u32 mtrlSndId, s8 reverb)
 * HD: checks the position pointer first (result unused). */
void setAnimSound(u32 self, u32 pos, f32 frame, f32 rate, u32 mtrlSndId, s32 reverb) {
    WWHD_FUNC(0x0201BF08, void, self, pos, frame, rate, mtrlSndId, reverb);
    if (pos != 0) ptr_check_l(pos);
    u32 id = (mtrlSndId & 0x00FFFFFF) | ((u32)reverb << 24);
    if (rate == 0.0f) return;
    if (rate < 0.0f) rate = -rate;
    JAIAnimeSound_setAnimSoundVec_l(self, zel_basic(), pos, frame, rate, id, 0);
}
VERIFY(0x0201BF08, setAnimSound);

/* 0201BFD8 JAIZelAnime::setPlayPosition(f32) */
void setPlayPosition(u32 self, f32 time) {
    WWHD_FUNC(0x0201BFD8, void, self, time);
    u32 data = ld(self + 0x90);
    if (data == 0) return;
    u32 n = ld16(data);
    s32 count = 0;
    for (u32 i = 0; i < n; i++) {
        if (!(ldf(data + 0xC + i * 0x20) < time)) break;
        count++;
    }
    stf(self + 0x88, time);
    st(self + 0x80, count);
}
VERIFY(0x0201BFD8, setPlayPosition);

/* 0201C020 JAIZelAnime::initActorAnimSound (HD override): the base version, then clears +0x98 */
void initActorAnimSound(u32 self, u32 data, u32 a, f32 f) {
    WWHD_FUNC(0x0201C020, void, self, data, a, f);
    JAIAnimeSound_initActorAnimSound_l(self, data, a, f);
    st(self + 0x98, 0);
}
VERIFY(0x0201C020, initActorAnimSound);

/* 0201C054 __sinit_JAIZelAnime_cpp (header statics) */
void __sinit_JAIZelAnime_cpp() {
    WWHD_FUNC(0x0201C054, void);
    header_sinit(0x101FFC24, 0x1018D4A8, 0x10003798);
}
VERIFY(0x0201C054, __sinit_JAIZelAnime_cpp);

} // namespace JAIZelAnime_cpp
