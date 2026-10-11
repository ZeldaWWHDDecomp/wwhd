/* Ambient sound positions and mixing, following GameCube JAIZelBasic names.
 * Verified HD implementation. */
#include "JAZelAudio/JAIZelAtmos.h"
// HD: BSS shore-state flag, outside the mapped image data.
static be<u32>& l_seaShoreState = *gabi::at<be<u32>>(0x101D6010);
// HD: full-width type/reverb arguments and repeated callee/field loads are intentional.
// HD: list indexing uses wrapping guest arithmetic; it must also accept out-of-range counts.
static cXyz* wrappedPosition(cXyz* positions, u32 index) {
    return gabi::at<cXyz>(gabi::ea(positions) + index * sizeof(cXyz));
}

/* 0201C0E8 */
f32 JAIZelBasic::calcPosPanLR(cXyz* pos) {
    f32 x = pos->x;
    if (x == 0.0f)
        return 0.5f;
    f32 z = pos->z;
    if (z == 0.0f) {
        if (x > 0.0f)
            return 1.0f;
        if (x < 0.0f)
            return 0.0f;
    }
    f32 r = x / JAIZel_sqrt(gabi::fmadds(x, x, z * z));
    if (r > 1.0f)
        return (1.0f + 1.0f) * 0.5f;
    if (r < -1.0f)
        r = -1.0f;
    return (r + 1.0f) * 0.5f;
}

/* 0201C228 */
f32 JAIZelBasic::calcPosPanSR(cXyz* pos, f32 scale) {
    f32 center = JAIGlobalParameter_getParamSeDolbyCenterValue() / 127.0f;
    f32 rest = 1.0f - center;
    f32 front = JAIGlobalParameter_getParamSeDolbyFrontDistanceMax() * scale;
    if (pos->z < front)
        return 0.0f;
    f32 z = pos->z;
    if (z < 0.0f) {
        f32 a = JAIGlobalParameter_getParamSeDolbyFrontDistanceMax();
        f32 num = center * (a - pos->z);
        return num / JAIGlobalParameter_getParamSeDolbyFrontDistanceMax();
    }
    if (!(z < JAIGlobalParameter_getParamSeDolbyBehindDistanceMax() * scale))
        return 1.0f;
    f32 behind = JAIGlobalParameter_getParamSeDolbyBehindDistanceMax() * scale;
    return gabi::fmadds(rest, pos->z / behind, center);
}

/* 0201C360 */
f32 JAIZelBasic::calcPosVolume(cXyz* pos, f32 scale) {
    if (pos == nullptr)
        return 0.0f;
    f32 y = pos->y, x = pos->x, z = pos->z;
    f32 dist = JAIZel_sqrt(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y)));
    if (dist < JAIGlobalParameter_getParamMaxVolumeDistance())
        return 1.0f;
    f32 d = dist - JAIGlobalParameter_getParamMaxVolumeDistance();
    f32 range = JAIGlobalParameter_getParamDistanceMax();
    range = (range - JAIGlobalParameter_getParamMaxVolumeDistance()) * scale;
    if (!(d < range))
        return 0.0f;
    f32 minVol = JAIGlobalParameter_getParamMinDistanceVolume();
    f32 minVol2 = JAIGlobalParameter_getParamMinDistanceVolume();
    return gabi::fmadds(1.0f - minVol2, 1.0f - d / range, minVol);
}

/* 0201C4B8 */
void JAIZelBasic::initSeaEnvPos() {
    field_0x1b80 = 0;
}

/* 0201C4C4 */
s32 JAIZelBasic::registSeaEnvPos(cXyz* pos) {
    if (pos == nullptr)
        return -1;
    *wrappedPosition(mSeaEnvPos, (u32)field_0x1b80) = *pos;
    s32 n = (u32)field_0x1b80 + 1;
    if (n >= 0x40) {
        field_0x1b80 = 0x3F;
        return -1;
    }
    field_0x1b80 = n;
    return n;
}

/* 0201C524 */
void JAIZelBasic::seaEnvSePlay(u32 type, u32 reverb) {
    if (field_0x01f8 != 0)
        return;
    if (field_0x0204 == 1)
        return;
    if (field_0x0201 == 1)
        return;
    if (field_0x0206 == 1)
        return;
    if (mGroupBytes.mSeaEnvGroupHD == 0x14) {
        /* four points 5 units around the camera, at height 0 */
        cXyz* cpos = mAudioCamera->field_0x0;
        f32 x = cpos->x, z = cpos->z;
        gabi::Local<cXyz> v0, v1, v2, v3;
        cXyz* a = v0.get();
        cXyz* b = v1.get();
        cXyz* c = v2.get();
        cXyz* d = v3.get();
        a->x = x - 5.0f;
        a->y = 0.0f;
        a->z = z - 5.0f;
        b->x = x - 5.0f;
        b->y = 0.0f;
        b->z = z + 5.0f;
        c->x = x + 5.0f;
        c->y = 0.0f;
        c->z = z - 5.0f;
        d->x = x + 5.0f;
        d->y = 0.0f;
        d->z = z + 5.0f;
        registSeaEnvPos(a);
        registSeaEnvPos(b);
        registSeaEnvPos(c);
        registSeaEnvPos(d);
    }
    f32 left = 0.0f, right = 0.0f, surround = 0.0f;
    if (field_0x1b80 == 0)
        return;
    void* mtx = mAudioCamera->field_0x8;
    for (s32 i = 0; i < (s32)field_0x1b80; i++) {
        cXyz* out = wrappedPosition(mSeaEnvCameraPos, (u32)i);
        JAIZel_multVec(mtx, wrappedPosition(mSeaEnvPos, (u32)i), out);
        f32 vol = calcPosVolume(out, type < 8 ? 3.0f : 1.0f);
        f32 sr = calcPosPanSR(out, 1.0f);
        f32 lr = calcPosPanLR(out);
        f32 s = sr * vol;
        f32 l = (1.0f - lr) * vol;
        f32 r = lr * vol;
        if (l > left)
            left = l;
        if (r > right)
            right = r;
        if (s > surround)
            surround = s;
    }
    if (left > 1.0f)
        left = 1.0f;
    if (right > 1.0f)
        right = 1.0f;
    if (surround > 1.0f)
        surround = 1.0f;
    if (field_0x0020 != 0) {
        left = left * 0.33f;
        surround = surround * 0.33f;
        right = right * 0.33f;
    }
    if (JAIZel_checkStreamPlaying(this, JA_STRM_DEMO_33_01)) {
        left = left * 0.7f;
        surround = surround * 0.7f;
        right = right * 0.7f;
    }
    u32 id0, id1, id2;
    if (type == 8) {
        id0 = JA_SE_MAGMA_L;
        id1 = JA_SE_MAGMA_R;
        id2 = JA_SE_MAGMA_SR;
    } else {
        id0 = JA_SE_SEA_ALL_NORM_L;
        id1 = JA_SE_SEA_ALL_NORM_R;
        id2 = JA_SE_SEA_ALL_NORM_SR;
    }
    if (left != 0.0f) {
        JAIBasic_startSoundVec(this, id0, &field_0x1ec4, 0, 0, 0, 4);
        if (field_0x1ec4 != 0) {
            JAISound_setPortData(field_0x1ec4, 9, reverb & 0xFFFF);
            JAISound_setPan(field_0x1ec4, 0, 0, 1.0f);
            JAISound_setDolby(field_0x1ec4, 0, 0, 0.0f);
            JAISound_setVolume(field_0x1ec4, 0, 0, left);
        }
    }
    if (right != 0.0f) {
        JAIBasic_startSoundVec(this, id1, &field_0x1ec8, 0, 0, 0, 4);
        if (field_0x1ec8 != 0) {
            JAISound_setPortData(field_0x1ec8, 9, reverb & 0xFFFF);
            JAISound_setPan(field_0x1ec8, 0, 0, 0.0f);
            JAISound_setDolby(field_0x1ec8, 0, 0, 0.0f);
            JAISound_setVolume(field_0x1ec8, 0, 0, right);
        }
    }
    if (surround != 0.0f) {
        JAIBasic_startSoundVec(this, id2, &field_0x1ecc, 0, 0, 0, 4);
        if (field_0x1ecc != 0) {
            JAISound_setPortData(field_0x1ecc, 9, reverb & 0xFFFF);
            JAISound_setPan(field_0x1ecc, 0, 0, 0.5f);
            JAISound_setDolby(field_0x1ecc, 0, 0, 1.0f);
            JAISound_setVolume(field_0x1ecc, 0, 0, surround);
        }
    }
}

/* 0201D410 */
void atmos_empty(AtmosSafeString* a) {}

/* sead::SafeString == (HD inline): the empty helper and cstr() (vtable slot 0x14) on the left
 * operand, cstr() on the right one, pointer compare, then a bounded strcmp */
// HD: SafeString virtual cstr calls precede the bounded comparison, in this order.
static bool safestring_eq(u32 lit, u32 other, u32 vtbl) {
    gabi::Local<AtmosSafeString> sa;
    gabi::Local<AtmosSafeString> sb;
    sa->mStringTop = gabi::at<be<u8>>(lit);
    sa->__vtbl = gabi::at<AtmosSafeStringVtable>(vtbl);
    sb->mStringTop = gabi::at<be<u8>>(other);
    sb->__vtbl = gabi::at<AtmosSafeStringVtable>(vtbl);
    AtmosSafeString* a = sa.get();
    AtmosSafeString* b = sb.get();
    atmos_empty(a);
    gabi::call_ptr<void>(a->__vtbl->cstr.get(), a);
    u32 cstr = b->__vtbl->cstr.get();
    be<u8>* pa = a->mStringTop;
    gabi::call_ptr<void>(cstr, b);
    if (pa == (be<u8>*)b->mStringTop)
        return true;
    pa = a->mStringTop;
    be<u8>* pb = b->mStringTop;
    for (u32 i = 0; i < 0x40001; i++) {
        // HD: byte addresses wrap at 32 bits, including callback-produced pointers.
        u8 ca = *gabi::at<be<u8>>(gabi::ea(pa) + i);
        if (ca != *gabi::at<be<u8>>(gabi::ea(pb) + i))
            return false;
        if (ca == 0)
            return true;
    }
    return false;
}

/* 0201CA10 */
void JAIZelBasic::seaShoreSE(u32 type, cXyz* pos, u32 keep, u32 reverb) {
    if (field_0x0201 == 1)
        return;
    if (field_0x0206 == 1)
        return;
    u32 id;
    if (type == 1)
        id = JA_SE_SEASHORE_QUAY;
    else if (type == 2)
        id = JA_SE_SEASHORE_CLIFF;
    else
        id = JA_SE_SEASHORE_NORM;
    if (field_0x022d == 0)
        return;
    mSeaShorePos = *pos;
    if (keep == 0 && field_0x1ed0 != 0)
        JAISound_stop(field_0x1ed0, 1);
    JAIBasic_startSoundVec(this, id, &field_0x1ed0, &mSeaShorePos, 0, 0, 4);
    if (field_0x1ed0 == 0)
        return;
    JAISound_setPortData(field_0x1ed0, 9, reverb & 0xFFFF);
    if (field_0x0020 != 0) {
        JAISound_setVolume(field_0x1ed0, 0, 0, 0.33f);
        return;
    }
    /* the current stage name (1047E6B8) against the literal at 10003814 */
    if (safestring_eq(0x10003814, 0x1047E6B8, 0x100037F8) && (u32)l_seaShoreState == 1) {
        JAISound_setVolume(field_0x1ed0, 0, 0, 0.5f);
        return;
    }
    if (field_0x0098 == 0.0f)
        JAISound_setVolume(field_0x1ed0, 0, 0, 0.667f);
}

/* 0201CC44 */
void JAIZelBasic::initRiverPos() {
    field_0x1dd0 = 0;
}

/* 0201CC50 */
s32 JAIZelBasic::registRiverPos(cXyz* pos) {
    if (pos == nullptr)
        return -1;
    void* mtx = mAudioCamera->field_0x8;
    gabi::Local<cXyz> v;
    cXyz* sp = v.get();
    f32 y = pos->y;
    sp->y = y;
    f32 z = pos->z, x = pos->x;
    sp->z = z;
    sp->x = x;
    if (mtx != 0) {
        JAIZel_multVec(mtx, sp, sp);
        y = sp->y;
        x = sp->x;
        z = sp->z;
    }
    f32 dist = JAIZel_sqrt(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y)));
    f32 max = JAIGlobalParameter_getParamDistanceMax();
    s32 n = field_0x1dd0;
    if (dist > max * 3.0f)
        return n;
    *wrappedPosition(mRiverPos, (u32)n) = *pos;
    n = (u32)field_0x1dd0 + 1;
    if (n >= 0x40) {
        field_0x1dd0 = 0x2F;
        return -1;
    }
    field_0x1dd0 = n;
    return n;
}

/* 0201CDA0 */
void JAIZelBasic::riverSePlay(u32 type, u32 reverb) {
    if (field_0x1dd0 == 0)
        return;
    if (field_0x01f8 != 0)
        return;
    if (field_0x0201 == 1)
        return;
    if (field_0x0206 == 1)
        return;
    if ((s32)field_0x0218 == 0x18)
        registRiverPos(&field_0x0054);
    u32 id;
    switch (type) {
    case 0:
        id = JA_SE_ATM_RIVER_M;
        break;
    case 1:
        id = JA_SE_ATM_RIVER_S;
        break;
    case 2:
        if (field_0x1dd4 == 0)
            return;
        id = JA_SE_CM_BST_HORI;
        break;
    case 4:
    case 6:
        id = JA_SE_ATM_RIVER_S_ISLE;
        break;
    default:
        return;
    }
    for (s32 i = 0; i < (s32)field_0x1dd0; i++)
        JAIZel_seStart(this, id, wrappedPosition(mRiverPos, (u32)i), 0, reverb, 0, 1.0f, 1.0f,
                       -1.0f, -1.0f);
}

/* 0201CF54 */
void JAIZelBasic::waterfallSePlay(u32 type, cXyz* pos, u32 reverb) {
    u32 id;
    if (type == 0)
        id = JA_SE_ATM_WATERFALL_S;
    else if (type == 2 || type == 4)
        id = JA_SE_ATM_WATERFALL_M_RIV;
    else
        id = (s32)field_0x0224 == 0x39 ? JA_SE_ATM_WATERFALL_M_LPF : JA_SE_ATM_WATERFALL_M;
    void* mtx = mAudioCamera->field_0x8;
    gabi::Local<cXyz> v;
    cXyz* sp = v.get();
    f32 y = pos->y;
    sp->y = y;
    f32 z = pos->z, x = pos->x;
    sp->z = z;
    sp->x = x;
    if (mtx != 0) {
        JAIZel_multVec(mtx, sp, sp);
        y = sp->y;
        x = sp->x;
        z = sp->z;
    }
    f32 dist = JAIZel_sqrt(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y)));
    if (dist > JAIGlobalParameter_getParamDistanceMax() * 4.0f)
        return;
    *wrappedPosition(mWaterfallPos, (u32)field_0x1e98) = *pos;
    if (field_0x0207 == 0 && !JAIZel_checkPlayingStreamBgmFlag(this, JA_STRM_DEMO_31_01) &&
        field_0x0207 == 0)
        JAIZel_seStart(this, id, wrappedPosition(mWaterfallPos, (u32)field_0x1e98), 0, reverb, 0,
                       1.0f, 1.0f, -1.0f, -1.0f);
    s32 n = field_0x1e98;
    if (n < 0xF)
        field_0x1e98 = n + 1;
}

/* 0201D140 */
void JAIZelBasic::initWindowPos() {
    field_0x1ec0 = 0;
}

/* 0201D14C */
s32 JAIZelBasic::registWindowPos(cXyz* pos) {
    if (pos == nullptr)
        return -1;
    *wrappedPosition(mWindowPos, (u32)field_0x1ec0) = *pos;
    s32 n = (u32)field_0x1ec0 + 1;
    if (n >= 3) {
        field_0x1ec0 = 2;
        return -1;
    }
    field_0x1ec0 = n;
    return n;
}

/* 0201D1AC */
void JAIZelBasic::rainPlay(s32 type) {
    if (field_0x1ec0 == 0) {
        if (type == 1) {
            if ((s32)field_0x0224 == 0x5C)
                JAIZel_seStart(this, JA_SE_ATM_RAIN_LPF, 0, 0, 0, 0, 1.0f, 1.0f, -1.0f, -1.0f);
            else
                JAIZel_seStart(this, JA_SE_ATM_RAIN_HARD, 0, 0, 0, 0, 1.0f, 1.0f, -1.0f, -1.0f);
        } else {
            JAIZel_seStart(this, JA_SE_ATM_RAIN_1, 0, 0, 0, 0, 1.0f, 1.0f, -1.0f, -1.0f);
        }
        return;
    }
    for (s32 i = 0; i < (s32)field_0x1ec0; i++)
        JAIZel_seStart(this, type == 1 ? JA_SE_ATM_RAIN_ROOM_HARD : JA_SE_ATM_RAIN_ROOM,
                       wrappedPosition(mWindowPos, (u32)i), 0, 0, 0, 1.0f, 1.0f, -1.0f, -1.0f);
}

/* 0201D368 */
// HD: compiler-generated header initialization and deleting-destructor copy.
void __sinit_JAIZelAtmos_cpp() {
    sinit_header_statics_rodata(0x101FFC40, 0x1018D4CC, 0x101FFC4C, 0x10003820);
}

/* 0201D3FC */
void atmos_deleting_dtor(JAIZelBasic* self, u32 flags) {
    if (self == nullptr)
        return;
    if ((flags & 1) == 0)
        return;
    JAIZel_delete(self);
}
/* WWHD glue
ext 0201EBA0 u32 JAIZel_seStart(JAIZelBasic* basic, u32 id, cXyz* pos, u32 info, s32 reverb, u32 flag, f32 volume, f32 pitch, f32 pan, f32 fx)
ext 0201DB08 s32 JAIZel_checkStreamPlaying(JAIZelBasic* basic, u32 id)
ext 020233C8 s32 JAIZel_checkPlayingStreamBgmFlag(JAIZelBasic* basic, u32 id)
ext 02802848 void JAIBasic_startSoundVec(JAIZelBasic* basic, u32 id, gptr<JAISound>* handle, cXyz* pos, u32 a, u32 b, u32 c)
ext 0280BA28 void JAISound_setPan(JAISound* sound, u32 time, u32 kind, f32 pan)
ext 0280C248 void JAISound_setDolby(JAISound* sound, u32 time, u32 kind, f32 dolby)
ext 0280B1D8 void JAISound_stop(JAISound* sound, u32 time)
ext 0280B68C void JAISound_setVolume(JAISound* sound, u32 time, u32 kind, f32 volume)
ext 0280C6D8 void JAISound_setPortData(JAISound* sound, u32 port, u32 value)
ext 028E8F64 void JAIZel_multVec(void* matrix, cXyz* in, cXyz* out)
ext 028F4384 f32 JAIZel_sqrt(f32 value)
ext 0273AF40 void JAIZel_delete(void* pointer)
ext 02804718 f32 JAIGlobalParameter_getParamDistanceMax()
ext 02804724 f32 JAIGlobalParameter_getParamMaxVolumeDistance()
ext 02804730 f32 JAIGlobalParameter_getParamMinDistanceVolume()
ext 02804754 f32 JAIGlobalParameter_getParamSeDolbyCenterValue()
ext 02804760 f32 JAIGlobalParameter_getParamSeDolbyFrontDistanceMax()
ext 0280476C f32 JAIGlobalParameter_getParamSeDolbyBehindDistanceMax()
*/
