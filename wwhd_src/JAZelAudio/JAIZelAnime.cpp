/* Animation-frame sound hooks, verified against WWHD. GameCube names with HD behavior retained. */
#include "JAZelAudio/JAIZelAnime.h"
// HD: singleton storage is BSS.
static gptr<JAIZelBasic>& l_zelBasic = *gabi::at<gptr<JAIZelBasic>>(0x101FFC78);
// HD: distance exceptions and base_pitch/volume truncation follow the HD executable.
/* 0201B71C */
void JAIZelAnime::startAnimSound(JAIZelBasic* basic, u32 id, gptr<JAISound>* sound,
                                 JAInter::Actor* actor, u32 flag) {
    if (basic->field_0x0207 != 0)
        return;
    if ((s32)basic->field_0x0224 == 0x12 && basic->mIslandRoomNo == 0x2C &&
        basic->field_0x0239 == 9 && basic->field_0x0028 < 30)
        return;
    switch (id) {
    case JA_SE_FT_PW_RUN_AWAY:
    case JA_SE_CV_DRG_MTOP_BIKU:
    case JA_SE_CV_DRG_MTOP_ABARE:
    case JA_SE_CV_DRG_MTOP_AKUBI:
    case JA_SE_CM_DRG_MTOP_HAND:
        /* mDoAud_seStart(id, actor->pos) */
        JAIZel_seStart((JAIZelBasic*)l_zelBasic, id, actor->field_0x4, 0, 0, 0, 1.0f, 1.0f, -1.0f,
                       -1.0f);
        return;
    }
    if (basic->field_0x0201 == 1)
        return;
    if (basic->field_0x0206 == 1)
        return;
    if (JAIZel_checkStreamPlaying(basic, JA_STRM_BOSS_CLEAR) ||
        JAIZel_checkStreamPlaying(basic, JA_STRM_BST_CLEAR) ||
        JAIZel_checkStreamPlaying(basic, JA_STRM_DK_CLEAR) ||
        JAIZel_checkStreamPlaying(basic, JA_STRM_BWD_CLEAR)) {
        if (id == JA_SE_LK_FT_WALK || id == JA_SE_LK_FT_JUMP)
            return;
    }
    if (basic->field_0x0046 == 1) {
        switch (id) {
        case JA_SE_LK_FT_WALK:
        case JA_SE_FT_LADDER_CLIMB:
        case JA_SE_FT_LADDER_CLIMB_D:
            id = JA_SE_LK_WALK_HEAVY;
            break;
        case JA_SE_LK_FT_JUMP:
            id = JA_SE_LK_JUMP_HEAVY;
            break;
        }
    } else if (id == JA_SE_LK_WALK_HEAVY) {
        id = JA_SE_LK_FT_WALK;
    }
    if (JAIZel_checkStreamPlaying(basic, JA_STRM_DEMO_04_01)) {
        switch (actor->field_0xc) {
        case 0x13:
        case 0x14:
        case 0x16:
        case 0x18:
            return;
        }
    }
    u32 info = actor->field_0xc;
    u32 reverb = info >> 24;
    actor->field_0xc = info & 0xFFFFFF;
    if (actor->field_0x4 != 0) {
        JAIZelCameraView* cam = basic->mCameras;
        cXyz* pos = actor->field_0x4;
        gabi::Local<cXyz> v;
        cXyz* sp = v.get();
        *sp = *pos; // GHS word copy preserves NaN payloads.
        JAIZel_multVec(cam->field_0x8, sp, sp);
        f32 y = sp->y, x = sp->x, z = sp->z;
        f32 distance = JAIZel_sqrt(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y)));
        if (id < JA_SE_CM_DK_FOOTNOTE) {
            if (id == JA_SE_CV_KAMOME) {
            } else if (id == JA_SE_CM_KAMOME_WING) {
                f32 max = JAIGlobalParameter_getParamDistanceMax();
                if (distance > max * 1.0f)
                    return;
            } else if (id < JA_SE_CM_DK_WING_ENTER) {
                if (distance > JAIGlobalParameter_getParamDistanceMax())
                    return;
            } else if (id <= JA_SE_CM_DK_WING_GO_UP) {
            } else {
                if (distance > JAIGlobalParameter_getParamDistanceMax())
                    return;
            }
        } else if (id == JA_SE_CM_DK_FOOTNOTE) {
        } else if (id == JA_SE_CM_COMMON_WING_S) {
            if (mUseExtendedWingDistance == 1) {
                f32 max = JAIGlobalParameter_getParamDistanceMax();
                if (distance > max * 3.0f)
                    return;
            } else {
                if (distance > JAIGlobalParameter_getParamDistanceMax())
                    return;
            }
        } else if (id == JA_SE_CM_COMMON_WING_S_LPF) {
            f32 max = JAIGlobalParameter_getParamDistanceMax();
            if (distance > max + max)
                return;
        } else {
            if (distance > JAIGlobalParameter_getParamDistanceMax())
                return;
        }
    }
    u32 kind = actor->field_0xc;
    if (kind == 0xF) {
        JAIZel_seStart(basic, JA_SE_FT_SUSBRIDGE_R, actor->field_0x4, kind, (s8)reverb, 0, 1.0f,
                       1.0f, -1.0f, -1.0f);
    } else if (kind == 0x10) {
        JAIZel_seStart(basic, JA_SE_FT_SUSBRIDGE_C, actor->field_0x4, kind, (s8)reverb, 0, 1.0f,
                       1.0f, -1.0f, -1.0f);
    }
    if (basic->field_0x0045 == 1) {
        switch (id) {
        case JA_SE_LK_FT_WALK:
        case JA_SE_LK_FT_JUMP:
        case JA_SE_LK_HD_NORMAL:
        case JA_SE_FT_LADDER_CLIMB:
        case JA_SE_FT_LADDER_CLIMB_D:
        case JA_SE_LK_WALK_HEAVY:
        case JA_SE_LK_JUMP_HEAVY:
            JAIZel_seStart(basic, JA_SE_LK_FT_SW_SHIELD, actor->field_0x4, 0, (s8)reverb, 0, 1.0f,
                           1.0f, -1.0f, -1.0f);
            break;
        }
    }
    if (*sound != 0)
        JAISound_stop(*sound, 0);
    JAIBasic_startSoundActor(basic, id, sound, actor, 0, 0);
    if (*sound != 0)
        JAISound_setPortData(*sound, 9, reverb);
}

// HD: pitch modification excludes types 6 and 0x12 through 0x16.
/* 0201BC14 */
void JAIZelAnime::setSpeedModifySound(JAISound* sound, JAIAnimeFrameSoundData* data, f32 rate) {
    s32 pitchFactor = (s8)data->mPitchFactor;
    f32 base_pitch = data->mPitch;
    if (pitchFactor != 0) {
        u32 type = sound->field_0x1c;
        if (type != 6 && !(type >= 0x12 && type <= 0x16)) {
            base_pitch = gabi::fmadds((f32)pitchFactor * (rate - 1.0f), 0.03125f, base_pitch);
        }
    }
    JAISound_setPitch(sound, 0, 5, base_pitch);
    s32 volumeFactor = (s8)data->mVolumeFactor;
    s32 base_volume = data->mVolume;
    if (rate == 0.0f)
        base_volume = 0;
    if (volumeFactor != 0) {
        bool zero = false;
        if (JAISound_getID(sound) == JA_SE_LK_HOFUKU) {
            if (rate < 0.0f)
                rate = -rate;
            if (rate < 1.2f) {
                zero = true;
            } else {
                base_volume = (s16)gabi::ftoi((rate - 1.2f) * 35.0f);
            }
        } else {
            f32 d = rate - 1.0f;
            if (sound->field_0x1c == 0x13) {
                base_volume =
                    (s16)(base_volume + (s16)gabi::ftoi((f32)(s8)data->mVolumeFactor * d));
            } else {
                f32 f = (f32)(s8)data->mVolumeFactor;
                base_volume = (s16)(base_volume + (s16)gabi::ftoi((f + f) * d));
            }
        }
        if (zero) {
            base_volume = 0;
        } else if (base_volume > 127) {
            base_volume = 127;
        } else if (base_volume < 0) {
            base_volume = 0;
        }
    }
    if (JAISound_getID(sound) == JA_SE_LK_HOFUKU) {
        if (sound->field_0x18 <= 1)
            JAISound_setVolume(sound, 0, 5, (f32)(u8)base_volume / 127.0f);
    } else {
        JAISound_setVolume(sound, 0, 5, (f32)(u8)base_volume / 127.0f);
    }
}

// HD: validates a non-null position before the rate check.
/* 0201BF08 */
void JAIZelAnime::setAnimSound(cXyz* pos, f32 frame, f32 rate, u32 mtrlSndId, s32 reverb) {
    if (pos != 0)
        JAIZel_checkPointer(pos);
    u32 id = (mtrlSndId & 0x00FFFFFF) | ((u32)reverb << 24);
    if (rate == 0.0f)
        return;
    if (rate < 0.0f)
        rate = -rate;
    JAIAnime_setAnimSoundVec(this, (JAIZelBasic*)l_zelBasic, pos, id, 0, frame, rate);
}

/* 0201BFD8 */
void JAIZelAnime::setPlayPosition(f32 time) {
    JAIAnimeSoundData* data = mpAsData;
    if (data == 0)
        return;
    u32 n = data->datas;
    s32 count = 0;
    for (u32 i = 0; i < n; i++) {
        if (!(data->mAfsData[i].mStartFrame < time))
            break;
        count++;
    }
    mCurrentTime = time;
    dataCounter = count;
}

/* 0201C020 */
void JAIZelAnime::initActorAnimSound(void* data, u32 a, f32 f) {
    JAIAnime_initActorAnimSound(this, data, a, f);
    mUseExtendedWingDistance = 0;
}

/* 0201C054 */
// HD: compiler-generated header statics.
void __sinit_JAIZelAnime_cpp() {
    sinit_header_statics_rodata(0x101FFC24, 0x1018D4A8, 0x101FFC30, 0x10003798);
}
/* WWHD glue
ext 0201EBA0 u32 JAIZel_seStart(JAIZelBasic* basic, u32 id, cXyz* pos, u32 info, s32 reverb, u32 flag, f32 volume, f32 pitch, f32 pan, f32 fx)
ext 0201DB08 s32 JAIZel_checkStreamPlaying(JAIZelBasic* basic, u32 id)
ext 02801BA8 void JAIAnime_initActorAnimSound(JAIZelAnime* self, void* data, u32 a, f32 f)
ext 02801D04 void JAIAnime_setAnimSoundVec(JAIZelAnime* self, JAIZelBasic* basic, cXyz* pos, u32 id, u32 flag, f32 frame, f32 rate)
ext 02802844 void JAIBasic_startSoundActor(JAIZelBasic* basic, u32 id, gptr<JAISound>* sound, JAInter::Actor* actor, u32 a, u32 b)
ext 02804718 f32 JAIGlobalParameter_getParamDistanceMax()
ext 0280B1D8 void JAISound_stop(JAISound* sound, u32 time)
ext 0280B68C void JAISound_setVolume(JAISound* sound, u32 time, u32 kind, f32 volume)
ext 0280BD74 void JAISound_setPitch(JAISound* sound, u32 time, u32 kind, f32 pitch)
ext 0280C6D8 void JAISound_setPortData(JAISound* sound, u32 port, u32 value)
ext 0280D2D8 u32 JAISound_getID(JAISound* sound)
ext 028E8F64 void JAIZel_multVec(void* matrix, cXyz* in, cXyz* out)
ext 028F4384 f32 JAIZel_sqrt(f32 value)
ext 0275CC9C u32 JAIZel_checkPointer(cXyz* pos)
*/
