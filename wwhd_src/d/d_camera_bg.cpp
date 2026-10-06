/**
 * d_camera_bg.cpp (WWHD)
 * Follow camera dCamera_c: background (line, sphere) checks, relational positions, sight tests,
 * trimming.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_camera.cpp) to the WWHD layout and code, verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/d_camera.h"
#include <bit>

/* 024F7E54 */
static void sph_chk_callback(void* sphChk, void* vtxTbl, s32 i0, s32 i1, s32 i2, be<f32>* plane, camSphChkdata_l* data) {
    WWHD_FUNC(0x024F7E54, void, sphChk, vtxTbl, i0, i1, i2, plane, data);
    f32 len = gabi::call<f32>(0x02010C50, plane, &data->field_0x8); /* cM3d_SignedLenPlaAndPos */
    f32 dot = PSVECDotProduct((cXyz*)plane, &data->field_0x14);
    if (dot + plane[3] < -0.0001f) return; /* plane->getPlaneFunc(&field_0x14) */
    if (!(len < data->field_0x4)) return;
    gabi::Local<cXyz> normal;
    gabi::Local<cXyz> push;
    f32 r = data->field_0x4;
    normal->copy(*(cXyz*)plane); /* *plane->GetNP() */
    cXyz_ml(normal, push, r - len);
    PSVECAdd(&data->field_0x8, push, &data->field_0x8);
}
VERIFY(0x024F7E54, sph_chk_callback);

/* 024F7F28. HD-only: like sph_chk_callback, but pushes the position away from field_0x14 (used
 * by compWallMargin in mode 4) instead of along the plane normal */
static void sph_chk_callback2(void* sphChk, void* vtxTbl, s32 i0, s32 i1, s32 i2, be<f32>* plane, camSphChkdata_l* data) {
    WWHD_FUNC(0x024F7F28, void, sphChk, vtxTbl, i0, i1, i2, plane, data);
    f32 len = gabi::call<f32>(0x02010C50, plane, &data->field_0x8);
    f32 dot = PSVECDotProduct((cXyz*)plane, &data->field_0x14);
    if (dot + plane[3] < -0.0001f) return;
    if (!(len < data->field_0x4)) return;
    gabi::Local<cXyz> push;
    gabi::Local<cXyz> diff;
    gabi::Local<cXyz> dir;
    cXyz_mi(&data->field_0x14, diff, &data->field_0x8);
    gabi::call(0x0201B084, diff.get(), dir.get()); /* cXyz::norm */
    cXyz_ml(dir, push, data->field_0x4 - len);
    PSVECAdd(&data->field_0x8, push, &data->field_0x8);
}
VERIFY(0x024F7F28, sph_chk_callback2);

/* 024F8F94 */
u32 dCamera_c::lineCollisionCheckBush(cXyz* start, cXyz* end) {
    WWHD_FUNC(0x024F8F94, u32, this, start, end);
    u32 ret = 0;
    u32 result = gabi::call<u32>(0x02516DF8, dComIfGp_ea() + 0x4EF8); /* dCcMassS_Mng::GetResultCam */
    if (result & 2) ret = 1;
    if (result & 4) ret |= 2;
    if (result & 8) ret |= 4;
    gabi::Local<u8[0x30]> cps;
    gabi::call(0x02018150, cps.get());                     /* cM3dGCps() */
    gabi::call(0x020181B0, cps.get(), start, end, 30.0f);  /* cps.Set(*start, *end, 30.0f) */
    gabi::call(0x02516DB4, dComIfGp_ea() + 0x4EF8, cps.get()); /* dCcMassS_Mng::SetCam(cps) */
    gabi::call(0x0201819C, cps.get(), 2);                  /* ~cM3dGCps() */
    return ret;
}
VERIFY(0x024F8F94, &dCamera_c::lineCollisionCheckBush);

/* 024FB4E8 */
bool dCamera_c::lineBGCheck(cXyz* start, cXyz* end, u8* linChk, u32 flags) {
    WWHD_FUNC(0x024FB4E8, bool, this, start, end, linChk, flags);
    u32 chk = gabi::ea(linChk);
    if (flags & 0x80) {
        gabi::store<u8>(chk + 0x5D, 0);
        gabi::store<u8>(chk + 0x5C, 1);
    } else {
        gabi::store<u8>(chk + 0x5C, 0);
        gabi::store<u8>(chk + 0x5D, 1);
    }
    gabi::call(0x024F1AFC, linChk, start, end, 0); /* dBgS_LinChk::Set(start, end, NULL) */
    u32 grp = gabi::load<u32>(chk + 0x4C);
    if (flags & 4) grp &= ~0x20000000u; else grp |= 0x20000000u;
    if (flags & 2) grp &= ~0x40000000u; else grp |= 0x40000000u;
    if (flags & 1) grp &= ~0x80000000u; else grp |= 0x80000000u;
    gabi::store<u32>(chk + 0x4C, grp);
    u32 f = gabi::load<u32>(chk + 0x68);
    if (flags & 8) f |= 2; else f &= ~2u;
    gabi::store<u32>(chk + 0x68, f);
    return gabi::call<bool>(0x02008860, dComIfGp_ea() + PLAY_BGS, linChk); /* cBgS::LineCross */
}
VERIFY(0x024FB4E8, &dCamera_c::lineBGCheck);

/* dBgS_CamLinChk on the stack (HD: the constructor and destructor are partly inlined) */
static void camLinChk_ct(u32 o) {
    gabi::call(0x02008FEC, o); /* cBgS_LinChk() */
    gabi::store<u32>(o + 0x00, o + 0x58);
    gabi::store<u32>(o + 0x04, o + 0x64);
    gabi::store<u32>(o + 0x10, 0x1004A66C);
    gabi::store<u32>(o + 0x20, 0x1004A67C);
    gabi::store<u32>(o + 0x58, 0x1004A69C);
    gabi::store<u32>(o + 0x64, 0x1004A68C);
    gabi::store<u8>(o + 0x5C, 0);
    gabi::store<u8>(o + 0x5D, 1);
    gabi::store<u8>(o + 0x5E, 0);
    gabi::store<u8>(o + 0x5F, 0);
    gabi::store<u8>(o + 0x60, 0);
    gabi::store<u8>(o + 0x61, 0);
    gabi::store<u8>(o + 0x62, 0);
    gabi::store<u32>(o + 0x68, 3);
}
static void camLinChk_dt_vtables(u32 o) {
    gabi::store<u32>(o + 0x58, 0x1004A61C);
    gabi::store<u32>(o + 0x20, 0x1004A4DC);
    gabi::store<u32>(o + 0x64, 0x1004A4EC);
}

/* 024FB608 lineBGCheck(cXyz* start, cXyz* end, cXyz* o_cross, u32 flags) */
static bool dCamera_lineBGCheckCross(dCamera_c* i_this, cXyz* start, cXyz* end, cXyz* cross, u32 flags) {
    WWHD_FUNC(0x024FB608, bool, i_this, start, end, cross, flags);
    gabi::Local<u8[0x6C]> linChk;
    u32 o = gabi::ea(linChk.get());
    camLinChk_ct(o);
    if (i_this->lineBGCheck(start, end, (u8*)linChk.get(), flags)) {
        camLinChk_dt_vtables(o);
        cross->copy(*gabi::at<cXyz>(o + 0x30)); /* lin_chk.GetCross() */
        gabi::call(0x02008B4C, o, 0); /* ~cBgS_LinChk() */
        return true;
    }
    cross->copy(*end);
    camLinChk_dt_vtables(o);
    gabi::call(0x02008B4C, o, 0);
    return false;
}
VERIFY(0x024FB608, dCamera_lineBGCheckCross);

/* 024FCBE8 lineBGCheck(cXyz* start, cXyz* end, u32 flags) */
static bool dCamera_lineBGCheck3(dCamera_c* i_this, cXyz* start, cXyz* end, u32 flags) {
    WWHD_FUNC(0x024FCBE8, bool, i_this, start, end, flags);
    gabi::Local<u8[0x6C]> linChk;
    u32 o = gabi::ea(linChk.get());
    camLinChk_ct(o);
    bool ret = i_this->lineBGCheck(start, end, (u8*)linChk.get(), flags);
    camLinChk_dt_vtables(o);
    gabi::call(0x02008B4C, o, 0);
    return ret;
}
VERIFY(0x024FCBE8, dCamera_lineBGCheck3);

/* 0250FDB8 */
bool dCamera_c::lineBGCheckBoth(cXyz* start, cXyz* end, u8* linChk, u32 flags) {
    WWHD_FUNC(0x0250FDB8, bool, this, start, end, linChk, flags);
    gabi::store<u8>(gabi::ea(linChk) + 0x53, 1);
    gabi::store<u8>(gabi::ea(linChk) + 0x54, 1);
    return lineBGCheck(start, end, linChk, flags);
}
VERIFY(0x0250FDB8, &dCamera_c::lineBGCheckBoth);

/* 024FBF9C. HD: mode 8 trims like the subject modes (1), GameCube 2; modes 1..3 come from a
 * table; modes 10/11 keep the current size when no aim status is set */
s32 dCamera_c::defaultTriming() {
    WWHD_FUNC(0x024FBF9C, s32, this);
    s32 force = mTrimTypeForce;
    if (force >= 0) {
        mTrimSize = force;
        return force;
    }
    fopAc_ac_c* player = mpPlayerActor.get();
    if (player == nullptr || fpcM_GetName(player) != 0xA8) {
        mTrimSize = 1;
        return 1;
    }
    u32 mode = mCurMode;
    s32 size;
    if (mode < 1) {
        size = 0;
    } else if (mode <= 3) {
        size = gabi::load<u8>(0x1004A7EB + mode);
    } else if (mode == 8) {
        size = 1;
    } else if (mode < 10) {
        size = 0;
    } else if (mode < 12) {
        s32 pad = mPadId;
        if (gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CD8) & 0x40000) {
            size = 2;
        } else {
            pad = mPadId;
            if (gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CD8) & 0xA5000) {
                size = 1;
            } else {
                return mTrimSize;
            }
        }
    } else if (mode == 12) {
        size = chkFlag(0x1000) ? 1 : 0;
    } else {
        size = 0;
    }
    mTrimSize = size;
    return size;
}
VERIFY(0x024FBF9C, &dCamera_c::defaultTriming);

/* 024FCCE0. HD: in mode 4 the sphere check pushes the centre away from the view centre
 * (sph_chk_callback2) */
void dCamera_c::compWallMargin(cXyz* ret, cXyz* center, f32 radius) {
    WWHD_FUNC(0x024FCCE0, void, this, ret, center, radius);
    gabi::Local<camSphChkdata_l> data;
    gabi::Local<u8[0x50]> sph;
    u32 s = gabi::ea(sph.get());
    gabi::call(0x024EE8B8, s); /* dBgS_SphChk base constructor */
    data->field_0x0 = center;
    s32 mode = mCurMode;
    data->field_0x8.copy(*center);
    gabi::store<u32>(s + 0x20, 0x1004A6FC);
    data->field_0x14.copy(mViewCache.mCenter);
    gabi::store<u32>(s + 0x34, 0x1004A70C);
    gabi::store<u32>(s + 0x10, 0x1004A6EC);
    gabi::store<u32>(s + 0x38, 0x1004A72C);
    data->field_0x4 = radius;
    gabi::store<u8>(s + 0x3D, 1);
    gabi::store<u32>(s + 0x44, 0x1004A71C);
    gabi::store<u32>(s + 0x4C, mode == 4 ? 0x024F7F28u : 0x024F7E54u); /* SetCallback */
    gabi::call(0x02018E88, s, center, radius); /* sph_chk.Set(*center, radius) */
    bool hit = gabi::call<bool>(0x024EF88C, dComIfGp_ea() + PLAY_BGS, s, data.get()); /* dBgS::SphChk */
    if (hit) {
        if (ret == nullptr) ret = (cXyz*)operator_new(0xC);
        if (ret != nullptr) ret->copy(data->field_0x8);
    } else {
        if (ret == nullptr) ret = (cXyz*)operator_new(0xC);
        if (ret != nullptr) ret->copy(*center);
    }
    gabi::store<u32>(s + 0x20, 0x1004A6AC);
    gabi::store<u32>(s + 0x34, 0x1004A6BC);
    gabi::store<u32>(s + 0x38, 0x1004A6DC);
    gabi::store<u32>(s + 0x44, 0x1004A4EC);
    gabi::call(0x02008B4C, s + 0x24, 0);
}
VERIFY(0x024FCCE0, &dCamera_c::compWallMargin);

/* 0250242C relationalPos(fopAc_ac_c*, cXyz*) */
void dCamera_c::relationalPos(cXyz* ret, fopAc_ac_c* actor, cXyz* offset) {
    WWHD_FUNC(0x0250242C, void, this, ret, actor, offset);
    if (actor == nullptr) {
        if (ret == nullptr) {
            ret = (cXyz*)operator_new(0xC);
            if (ret == nullptr) return;
        }
        ret->copy(*gabi::at<cXyz>(0x101FFBA8)); /* cXyz::Zero */
        return;
    }
    gabi::Local<cSGlobe_l> globe;
    gabi::Local<cSAngle_l> dir;
    gabi::Local<cSAngle_l> sum;
    gabi::Local<cSAngle_l> tmp;
    gabi::Local<cXyz> att;
    gabi::Local<cXyz> xyz;
    gabi::call(0x02007324, globe.get(), offset); /* cSGlobe(const cXyz&) */
    directionOf(dir, actor);
    gabi::call(0x02006894, dir.get(), sum.get(), &globe->mU); /* cSAngle::operator+ */
    cSAngle_l* a = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)*sum);
    globe->mU = (s16)*a;
    attentionPos(att, actor);
    gabi::call(0x020073AC, globe.get(), xyz.get()); /* cSGlobe::Xyz */
    cXyz_pl(att, ret, xyz);
}
VERIFY(0x0250242C, &dCamera_c::relationalPos);

/* 025050E4. HD: the projected point is tested against (0, 0)-(width, height) of the viewport */
bool dCamera_c::pointInSight(cXyz* point) {
    WWHD_FUNC(0x025050E4, bool, this, point);
    s32 camId = gabi::call<s32>(0x025DA64C, mpCamera.get()); /* fopCamM_GetParam */
    s32 winId = gabi::load<s8>(dComIfGp_ea() + camId * 0x34 + 0x5AFC);
    u32 win = dComIfGp_ea() + winId * 0x2C;
    f32 h = gabi::load<f32>(win + 0x5AF0);
    f32 w = gabi::load<f32>(win + 0x5AEC);
    gabi::Local<cXyz> proj;
    gabi::call(0x025F0C48, point, proj.get()); /* mDoLib_project */
    f32 x = proj->x;
    if (x > 0.0f && x < w) {
        f32 y = proj->y;
        if (y > 0.0f && y < h) return true;
    }
    return false;
}
VERIFY(0x025050E4, &dCamera_c::pointInSight);

/* 02514D00 relationalPos(fopAc_ac_c*, cXyz*, cSAngle) */
static void dCamera_relationalPosA(dCamera_c* i_this, cXyz* ret, fopAc_ac_c* actor, cXyz* offset, cSAngle_l* angle) {
    WWHD_FUNC(0x02514D00, void, i_this, ret, actor, offset, angle);
    if (actor == nullptr) {
        if (ret == nullptr) {
            ret = (cXyz*)operator_new(0xC);
            if (ret == nullptr) return;
        }
        ret->copy(*gabi::at<cXyz>(0x101FFBA8)); /* cXyz::Zero */
        return;
    }
    gabi::Local<cSGlobe_l> globe;
    gabi::Local<cSAngle_l> dir;
    gabi::Local<cSAngle_l> sum1;
    gabi::Local<cSAngle_l> sum2;
    gabi::Local<cSAngle_l> tmp;
    gabi::Local<cXyz> att;
    gabi::Local<cXyz> xyz;
    gabi::call(0x02007324, globe.get(), offset);
    i_this->directionOf(dir, actor);
    gabi::call(0x02006894, dir.get(), sum1.get(), &globe->mU);
    gabi::call(0x02006894, sum1.get(), sum2.get(), angle);
    cSAngle_l* a = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)*sum2);
    globe->mU = (s16)*a;
    i_this->attentionPos(att, actor);
    gabi::call(0x020073AC, globe.get(), xyz.get());
    cXyz_pl(att, ret, xyz);
}
VERIFY(0x02514D00, dCamera_relationalPosA);

/* 024F8DB4. HD: the special areas use function-local statics (two centres) */
void dCamera_c::checkSpecialArea() {
    WWHD_FUNC(0x024F8DB4, void, this);
    const u32 kazeCenter = 0x1046EF78, seaCenter = 0x1046EF84;
    if (gabi::load<u32>(0x1046EFB0) == 0) {
        gabi::store<u32>(0x1046EFB0, 1);
        gabi::store<f32>(kazeCenter + 0, 0.0f);
        gabi::store<f32>(kazeCenter + 4, -3650.0f);
        gabi::store<f32>(kazeCenter + 8, 0.0f);
    }
    if (gabi::load<u32>(0x1046EFB4) == 0) {
        gabi::store<f32>(seaCenter + 8, -200000.0f);
        gabi::store<f32>(seaCenter + 4, 750.0f);
        gabi::store<f32>(seaCenter + 0, -180000.0f);
        gabi::store<u32>(0x1046EFB4, 1);
    }
    gabi::Local<cXyz> pos;
    positionOf(pos, mpPlayerActor.get());
    u8 kaze = m788;
    m787 = 0;
    if (kaze != 0) {
        f32 d = gabi::call<f32>(0x024F7AC4, kazeCenter, pos.get()); /* dCamMath::xyzHorizontalDistance */
        if (d < gabi::load<f32>(0x101D5528)) m787 = 1;
    }
    u8 sea = m780;
    m786 = 0;
    if (sea != 0) {
        f32 d = gabi::call<f32>(0x024F7AC4, seaCenter, pos.get());
        if (d < gabi::load<f32>(0x101D552C)) m786 = 1;
    }
}
VERIFY(0x024F8DB4, &dCamera_c::checkSpecialArea);

/* 02508804 relationalPos(fopAc_ac_c*, fopAc_ac_c*, cXyz*, f32) */
static void dCamera_relationalPos2(dCamera_c* i_this, cXyz* ret, fopAc_ac_c* actor1, fopAc_ac_c* actor2, cXyz* offset, f32 scale) {
    WWHD_FUNC(0x02508804, void, i_this, ret, actor1, actor2, offset, scale);
    if (actor1 == nullptr) {
        if (ret == nullptr) {
            ret = (cXyz*)operator_new(0xC);
            if (ret == nullptr) return;
        }
        ret->copy(*gabi::at<cXyz>(0x101FFBA8)); /* cXyz::Zero */
        return;
    }
    if (actor2 == nullptr) {
        i_this->relationalPos(ret, actor1, offset);
        return;
    }
    gabi::Local<cXyz> pos1, pos2, delta, half, mid, delta2, res;
    gabi::Local<cSGlobe_l> deltaGlobe, offsetGlobe;
    gabi::Local<cSAngle_l> dir, sum, tmp, diff;
    i_this->attentionPos(pos1, actor1);
    i_this->attentionPos(pos2, actor2);
    cXyz_mi(pos2, delta, pos1);
    cXyz_ml(delta, half, 0.5f);
    cXyz_pl(pos1, mid, half);
    cXyz_mi(pos2, delta2, pos1);
    gabi::call(0x02007324, deltaGlobe.get(), delta2.get());
    gabi::call(0x02007324, offsetGlobe.get(), offset);
    i_this->directionOf(dir, actor1);
    gabi::call(0x02006894, dir.get(), sum.get(), &offsetGlobe->mU);
    cSAngle_l* a = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)*sum);
    offsetGlobe->mU = (s16)*a;
    gabi::call(0x020068B0, &i_this->mViewCache.mDirection.mU, diff.get(), &deltaGlobe->mU); /* cSAngle::operator- */
    f32 r = gabi::fmuls_ppc(deltaGlobe->mRadius, 0.5f);
    f32 c = gabi::call<f32>(0x02006838, diff.get()); /* cSAngle::Cos */
    deltaGlobe->mRadius = gabi::fmuls_ppc(gabi::fmuls_ppc(r, c), scale);
    gabi::call(0x020073AC, deltaGlobe.get(), delta.get());  /* cSGlobe::Xyz */
    cXyz_pl(mid, delta2, delta);
    gabi::call(0x020073AC, offsetGlobe.get(), half.get());
    cXyz_pl(delta2, res, half);
    if (ret == nullptr) {
        ret = (cXyz*)operator_new(0xC);
        if (ret == nullptr) return;
    }
    ret->copy(*res);
}
VERIFY(0x02508804, dCamera_relationalPos2);

/* 025051BC lineBGCheckBack(cXyz* start, cXyz* end, u32 flags) */
static bool dCamera_lineBGCheckBack(dCamera_c* i_this, cXyz* start, cXyz* end, u32 flags) {
    WWHD_FUNC(0x025051BC, bool, i_this, start, end, flags);
    gabi::Local<u8[0x6C]> linChk;
    u32 o = gabi::ea(linChk.get());
    camLinChk_ct(o);
    gabi::store<u8>(o + 0x54, 1); /* OnBackFlag */
    gabi::store<u8>(o + 0x53, 0); /* OffFrontFlag */
    bool ret = i_this->lineBGCheck(start, end, (u8*)linChk.get(), flags);
    camLinChk_dt_vtables(o);
    gabi::call(0x02008B4C, o, 0);
    return ret;
}
VERIFY(0x025051BC, dCamera_lineBGCheckBack);

/* stack dBgS_GndChk / dBgS_CamGndChk_Wtr (HD: constructors and destructors partly inlined) */
static void gndChk_ct(u32 o, const cXyz* pos) {
    gabi::call(0x02008E0C, o); /* cBgS_GndChk() */
    gabi::store<u8>(o + 0x46, 0);
    gabi::store<u8>(o + 0x4A, 0);
    gabi::store<u8>(o + 0x44, 0);
    gabi::store<u8>(o + 0x45, 0);
    gabi::store<u32>(o + 0x20, 0x1004A50C);
    gabi::store<u32>(o + 0x10, 0x1004A4FC);
    gabi::store<u8>(o + 0x48, 0);
    gabi::store<u8>(o + 0x47, 0);
    gabi::store<u32>(o + 0x4C, 0x1004A51C);
    gabi::store<u32>(o + 0x50, 1);
    gabi::store<u32>(o + 0x40, 0x1004A52C);
    gabi::store<u32>(o + 0x00, o + 0x40);
    gabi::store<u32>(o + 0x04, o + 0x4C);
    gabi::store<u8>(o + 0x49, 0);
    gabi::at<cXyz>(o + 0x24)->copy(*pos); /* SetPos */
}
static void camGndChkWtr_ct(u32 o) {
    gabi::call(0x02008E0C, o);
    gabi::store<u32>(o + 0x00, o + 0x40);
    gabi::store<u32>(o + 0x04, o + 0x4C);
    gabi::store<u8>(o + 0x44, 0);
    gabi::store<u8>(o + 0x45, 1);
    gabi::store<u8>(o + 0x46, 0);
    gabi::store<u8>(o + 0x47, 0);
    gabi::store<u8>(o + 0x48, 0);
    gabi::store<u8>(o + 0x49, 0);
    gabi::store<u8>(o + 0x4A, 0);
    gabi::store<u32>(o + 0x10, 0x1004A57C);
    gabi::store<u32>(o + 0x20, 0x1004A58C);
    gabi::store<u32>(o + 0x40, 0x1004A5AC);
    gabi::store<u32>(o + 0x4C, 0x1004A59C);
    gabi::store<u32>(o + 0x50, 2);
}
static void gndChk_dt(u32 o) {
    gabi::store<u32>(o + 0x20, 0x1004A50C);
    gabi::store<u32>(o + 0x40, 0x1004A52C);
    gabi::store<u32>(o + 0x4C, 0x1004A4EC);
    gabi::call(0x02008DAC, o, 0); /* ~cBgS_Chk */
}

/* 024FB768 */
f32 dCamera_c::groundHeight(cXyz* pos) {
    WWHD_FUNC(0x024FB768, f32, this, pos);
    gabi::Local<u8[0x54]> gnd, wtr;
    u32 g = gabi::ea(gnd.get()), w = gabi::ea(wtr.get());
    gndChk_ct(g, pos);
    f32 gndY = gabi::call<f32>(0x02008974, dComIfGp_ea() + PLAY_BGS, g); /* GroundCross */
    camGndChkWtr_ct(w);
    gabi::at<cXyz>(w + 0x24)->copy(*pos);
    f32 wtrY = gabi::call<f32>(0x02008974, dComIfGp_ea() + PLAY_BGS, w);
    f32 y = (wtrY - gndY >= 0.0f) ? wtrY : gndY; /* fsel */
    if (y == -1000000000.0f) y = pos->y;
    gndChk_dt(w);
    gndChk_dt(g);
    return y;
}
VERIFY(0x024FB768, &dCamera_c::groundHeight);

/* 024FCEEC */
f32 dCamera_c::getWaterSurfaceHeight(cXyz* pos) {
    WWHD_FUNC(0x024FCEEC, f32, this, pos);
    u32 p = gabi::ea(pos);
    u32 yBits = gabi::load<u32>(p + 4);
    f32 best = -1000000000.0f;
    u32 xBits = gabi::load<u32>(p);
    u32 zBits = gabi::load<u32>(p + 8);
    gabi::Local<u8[0x50]> roof;
    gabi::Local<u8[0x54]> gnd;
    u32 r = gabi::ea(roof.get()), g = gabi::ea(gnd.get());
    gabi::call(0x024EE7AC, r); /* dBgS_RoofChk() */
    gabi::store<u32>(r + 0x38, xBits); /* SetPos */
    gabi::store<u32>(r + 0x40, zBits);
    gabi::store<u32>(r + 0x3C, yBits);
    f32 roofY = gabi::call<f32>(0x024EF6E8, dComIfGp_ea() + PLAY_BGS, r); /* dBgS::RoofChk */
    if (std::bit_cast<f32>(yBits) < roofY) yBits = std::bit_cast<u32>(roofY);
    camGndChkWtr_ct(g);
    gabi::store<u32>(g + 0x24, xBits); /* SetPos */
    gabi::store<u32>(g + 0x2C, zBits);
    gabi::store<u32>(g + 0x28, yBits);
    f32 gndY = gabi::call<f32>(0x02008974, dComIfGp_ea() + PLAY_BGS, g);
    f32 h = gndY + 5.0f;
    if (h > pos->y) best = h;
    if (daSea_ChkArea(pos->x, pos->z)) {
        f32 wave = daSea_calcWave(pos->x, pos->z) + 20.0f;
        if (wave > pos->y && wave > best) best = wave;
    }
    if (best == -1000000000.0f) best = pos->y;
    gndChk_dt(g);
    gabi::store<u32>(r + 0x30, 0x1004A4EC);
    gabi::store<u32>(r + 0x20, 0x1004A5BC);
    gabi::store<u32>(r + 0x24, 0x1004A5DC);
    gabi::call(0x02008B4C, r + 0x10, 0);
    return best;
}
VERIFY(0x024FCEEC, &dCamera_c::getWaterSurfaceHeight);

/* 0250252C */
cSAngle_l* dCamera_c::calcPeepAngle(cSAngle_l* ret) {
    WWHD_FUNC(0x0250252C, cSAngle_l*, this, ret);
    gabi::Local<cSAngle_l> res;
    gabi::call(0x02006644, res.get(), 0x101FF354u); /* cSAngle res(cSAngle::_0) */
    for (int side = 0; side < 2; side++) {
        s32 pad = mPadId;
        u32 status0 = gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CD8); /* check_owner_action */
        if (!(status0 & (side == 0 ? 0x20u : 0x40u))) continue;
        gabi::Local<cXyz> off1, off2, p1, p2;
        gabi::Local<u8[0x6C]> linChk;
        gabi::Local<cSAngle_l> dir, d, s;
        gabi::Local<cSGlobe_l> globe;
        off1->x = 0.0f;
        off1->z = -30.0f;
        off1->y = 0.0f;
        off2->z = -30.0f;
        off2->y = 0.0f;
        off2->x = side == 0 ? -50.0f : 50.0f;
        relationalPos(p1, mpPlayerActor.get(), off1);
        relationalPos(p2, mpPlayerActor.get(), off2);
        u32 o = gabi::ea(linChk.get());
        camLinChk_ct(o);
        if (lineBGCheck(p2, p1, (u8*)linChk.get(), 0x7F)) {
            u32 pla = gabi::call<u32>(0x020084C8, dComIfGp_ea() + PLAY_BGS, (u32)gabi::load<u16>(o + 0x16),
                                      (u32)gabi::load<u16>(o + 0x14)); /* dComIfG_Bgsp()->GetTriPla(lin_chk) */
            cSGlobe_l* g = gabi::call<cSGlobe_l*>(0x02007324, globe.get(), pla); /* cSGlobe(plane->mNormal) */
            directionOf(dir, mpPlayerActor.get());
            gabi::call(0x020068B0, &g->mU, d.get(), dir.get()); /* U() - directionOf(player) */
            gabi::call(0x02006894, side == 0 ? 0x101FF358u : 0x101FF35Cu, s.get(), d.get()); /* cSAngle::_90 / _270 + */
            *res = (s16)*s;
        }
        camLinChk_dt_vtables(o);
        gabi::call(0x02008B4C, o, 0);
        break;
    }
    return gabi::call<cSAngle_l*>(0x02006644, ret, res.get()); /* cSAngle(res) */
}
VERIFY(0x0250252C, &dCamera_c::calcPeepAngle);

/* 024FC780. HD: the half-angles come from the 1280x720 scissor; the actor delta is normalized in
 * place but its unnormalized value is what gets scaled by 50 (cXyz::normalize returns a copy) */
f32 dCamera_c::radiusActorInSight6(fopAc_ac_c* a1, fopAc_ac_c* a2, cXyz* center, cXyz* eye, f32 fovY, s16 bank) {
    WWHD_FUNC(0x024FC780, f32, this, a1, a2, center, eye, fovY, bank);
    gabi::Local<cXyz> d, p1, p2, dd, n, t2, v;
    gabi::Local<cSGlobe_l> g, g1, g2;
    gabi::Local<cSAngle_l> A, B, tmp, D;
    gabi::Local<Mtx34> mtx;
    cXyz_mi(eye, d, center);
    gabi::call(0x02007324, g.get(), d.get()); /* cSGlobe(eye - center) */
    attentionPos(p1, a1);
    attentionPos(p2, a2);
    cXyz_mi(p1, dd, p2);
    gabi::call(0x0201B31C, dd.get(), n.get()); /* dd.normalize() */
    cXyz_ml(dd, n, 50.0f);
    PSVECAdd(p1, n, p1);
    cXyz_ml(dd, n, 50.0f);
    gabi::call(0x028E8DAC, p2.get(), n.get(), p2.get()); /* PSVECSubtract */
    s32 camId = gabi::call<s32>(0x025DA64C, mpCamera.get());
    s32 winId = gabi::load<s8>(dComIfGp_ea() + camId * 0x34 + 0x5AFC);
    u32 win = dComIfGp_ea() + winId * 0x2C;
    f32 half = fovY * 0.5f;
    f32 h = gabi::load<f32>(win + 0x5AF0); /* scissor height */
    gabi::call(0x020066C0, A.get(), half * (h / 720.0f) * 0.95f);
    f32 w = gabi::load<f32>(win + 0x5AEC); /* scissor width */
    f32 ws = w / 1280.0f;
    gabi::call(0x020066C0, B.get(), fovY * mWindowAspectRatio * 0.5f * ws * 0.95f);
    cXyz_mi(eye, n, p1);
    gabi::call(0x02007324, g1.get(), n.get());
    cXyz_mi(eye, d, p2);
    gabi::call(0x02007324, g2.get(), d.get());
    cSAngle_ct(tmp);
    u32 flags = 0;
    struct { cSAngle_l* a; cSAngle_l* b; cSAngle_l* lim; u32 bit; } checks[4] = {
        {&g1->mU, &g->mU, B, 1}, {&g1->mV, &g->mV, A, 2}, {&g2->mU, &g->mU, B, 4}, {&g2->mV, &g->mV, A, 8}};
    for (auto& c : checks) {
        gabi::call(0x020068B0, c.a, D.get(), c.b); /* cSAngle::operator- */
        *tmp = (s16)*D;
        gabi::call(0x02006880, c.lim, D.get()); /* -lim */
        s16 t = *tmp;
        if (t < (s16)*D || t > (s16)*c.lim) flags |= c.bit;
    }
    if (flags == 0) return 0.0f;
    f32 radius = 0.0f;
    gabi::call(0x025F1EAC, mtx.get(), eye, center, &mUp, bank); /* mDoMtx_lookAt */
    for (int k = 0; k < 2; k++) {
        u32 fx = k == 0 ? 1u : 4u, fy = k == 0 ? 2u : 8u;
        if (!(flags & (fx | fy))) continue;
        PSMTXMultVec(mtx, k == 0 ? p1.get() : p2.get(), v);
        if (flags & fx) {
            f32 ax = std::fabs((f32)v->x);
            f32 t = gabi::call<f32>(0x0200685C, B.get()); /* Tan */
            f32 r = ax / t + v->z;
            if (radius < r) radius = r;
        }
        if (flags & fy) {
            f32 ay = std::fabs((f32)v->y);
            f32 t = gabi::call<f32>(0x0200685C, A.get());
            f32 r = ay / t + v->z;
            if (radius < r) radius = r;
        }
    }
    return radius;
}
VERIFY(0x024FC780, &dCamera_c::radiusActorInSight6);


/* one probe of forwardCheckAngle (i = 0, 1): the pitch towards the ground ahead, weighted */
static s16 fwdProbe(dCamera_c* c, cXyz* pos, f32 h, s32 i, s16* zero) {
    gabi::Local<cSAngle_l> chk, w;
    gabi::Local<cSGlobe_l> g, g1;
    gabi::Local<cXyz> x, target, d, n, m, t, cross;
    u32 o = c->ea();
    s32 deg = gabi::ftoi(gabi::load<f32>(o + (i == 0 ? 0x84C : 0x85C)) * 182.04445f); /* FwdChkAngle(i) */
    gabi::call(0x0200658C, chk.get(), (s16)deg);
    s16 inv = cSAngle_Inv(&c->mDirection.mU);
    f32 dist = h * gabi::load<f32>(o + (i == 0 ? 0x848 : 0x858)); /* FwdDistance(i) */
    gabi::call(0x02007068, g.get(), dist, (s16)*chk, inv); /* cSGlobe(f32, s16, s16) */
    gabi::call(0x020073AC, g.get(), x.get());
    cXyz_pl(pos, target, x);
    if (gabi::call<bool>(0x024FB608, c, pos, target.get(), cross.get(), 0x7F)) { /* lineBGCheck(pos, target, &cross, 0x7f) */
        cXyz_mi(target, d, pos);
        f32 len = std_sqrtf(PSVECSquareMag(d));
        if (len < 1.0f) {
            target->copy(*cross);
            f32 gh = c->groundHeight(target);
            cross->y = gh + h;
            goto weigh;
        }
        gabi::call(0x0201B084, d.get(), n.get()); /* norm */
        cXyz_ml(n, m, gabi::load<f32>(o + 0x868) /* FwdBackMargin */);
        cXyz_mi(cross, t, m);
        target->copy(*t);
    }
    cross->copy(*target);
    {
        f32 gh = c->groundHeight(target);
        cross->y = gh + h;
    }
weigh:
    cXyz_mi(cross, x, pos);
    gabi::call(0x02007324, g1.get(), x.get());
    if (i == 0) *zero = gabi::load<s16>(0x101FF354); /* cSAngle::_0 */
    f32 weight = (s16)g1->mV < *zero ? gabi::load<f32>(o + (i == 0 ? 0x854 : 0x864))  /* FwdWeightL(i) */
                                     : gabi::load<f32>(o + (i == 0 ? 0x850 : 0x860)); /* FwdWeightH(i) */
    gabi::call(0x0200693C, &g1->mV, w.get(), weight);
    return *w;
}

/* 024FB930. HD: the player height comes from heightOf(); the line checks are the cross-point
 * overload; the result is copied into the return slot by value */
void dCamera_c::forwardCheckAngle(cSAngle_l* ret) {
    WWHD_FUNC(0x024FB930, void, this, ret);
    gabi::Local<u8[0x6C]> linChk; /* dBgS_CamLinChk_NorWtr (unused) */
    gabi::Local<cSAngle_l> r, a, b, neg;
    gabi::Local<cXyz> pos;
    u32 lc = gabi::ea(linChk.get());
    camLinChk_ct(lc);
    gabi::call(0x02006644, r.get(), 0x101FF354u); /* cSAngle ret(cSAngle::_0) */
    cSAngle_ct(a);
    cSAngle_ct(b);
    positionOf(pos, mpPlayerActor.get());
    f32 h = heightOf(mpPlayerActor.get());
    pos->y = pos->y + h;
    s16 zero;
    *a = fwdProbe(this, pos, h, 0, &zero);
    *b = fwdProbe(this, pos, h, 1, &zero);
    s16 av = *a, bv = *b;
    cSAngle_l* src;
    if (av >= zero && bv >= zero) {
        src = av < bv ? b.get() : a.get();
    } else if (av <= zero && bv <= zero) {
        src = av > bv ? b.get() : a.get();
    } else {
        src = a.get();
    }
    gabi::call(0x02006880, src, neg.get()); /* -src */
    s16 res = *neg;
    *r = res;
    if (!(res > zero)) gabi::call(0x020069A0, r.get(), 0.75f); /* ret *= 0.75f */
    gabi::call(0x02006644, ret, r.get());
    camLinChk_dt_vtables(lc);
    gabi::call(0x02008B4C, lc, 0);
}
VERIFY(0x024FB930, &dCamera_c::forwardCheckAngle);

/* 024F9044. HD: the ground under the player's head is always checked with a camera+object ground
 * check (the result is copied into mBG.m5C when it is higher); GetPolyCamId/GetRoomCamId take the
 * poly info by index */
void dCamera_c::checkGroundInfo() {
    WWHD_FUNC(0x024F9044, void, this);
    /* The memberwise copy mBG.m5C.m04 = gnd_chk below also copies gnd_chk's internal (virtual
     * base) pointers, i.e. addresses inside this stack frame: gnd_chk is placed where the original
     * frame (0x1E0 bytes) has it, at frame + 0x48, so the copied values are the same. */
    gabi::Local<u8[0x1E0]> frame;
    gabi::Local<cXyz> pp, pos, dummy, t, A, B, d;
    gabi::Local<u8[0x54]> g2;
    gabi::Local<u8[0x50]> roof;
    gabi::Local<cSAngle_l> dir, ang;
    positionOf(pp, mpPlayerActor.get());
    u32 gyBits = gabi::load<u32>(gabi::ea(pp.get()) + 4);
    u32 gzBits = gabi::load<u32>(gabi::ea(pp.get()) + 8);
    u32 gxBits = gabi::load<u32>(gabi::ea(pp.get()));
    f32 h = heightOf(mpPlayerActor.get());
    pp->y = std::bit_cast<f32>(gyBits) + h;
    u32 r = gabi::ea(roof.get());
    gabi::call(0x024EE7AC, r); /* dBgS_RoofChk() */
    gabi::store<u32>(r + 0x38, gxBits); /* SetPos(gnd_chk_pos) */
    gabi::store<u32>(r + 0x3C, gyBits);
    gabi::store<u32>(r + 0x40, gzBits);
    f32 roofY = gabi::call<f32>(0x024EF6E8, dComIfGp_ea() + PLAY_BGS, r); /* dBgS::RoofChk */
    if (std::bit_cast<f32>(gyBits) < roofY) gyBits = std::bit_cast<u32>(roofY);

    /* dBgS_CamGndChk gnd_chk (camera + object) at the player's head */
    u32 o = gabi::ea(frame.get()) + 0x48;
    gabi::call(0x02008E0C, o);
    gabi::store<u32>(o + 0x00, o + 0x40);
    gabi::store<u32>(o + 0x04, o + 0x4C);
    gabi::store<u32>(o + 0x10, 0x1004A53C);
    gabi::store<u32>(o + 0x20, 0x1004A54C);
    gabi::store<u32>(o + 0x40, 0x1004A56C);
    gabi::store<u32>(o + 0x4C, 0x1004A55C);
    gabi::store<u8>(o + 0x44, 1);
    gabi::store<u8>(o + 0x45, 0);
    gabi::store<u8>(o + 0x46, 0);
    gabi::store<u8>(o + 0x47, 0);
    gabi::store<u8>(o + 0x48, 0);
    gabi::store<u8>(o + 0x49, 0);
    gabi::store<u8>(o + 0x4A, 0);
    gabi::store<u32>(o + 0x50, 1);
    gabi::at<cXyz>(o + 0x24)->copy(*pp);
    f32 groundY = gabi::call<f32>(0x02008974, dComIfGp_ea() + PLAY_BGS, o);
    u32 m = ea() + 0x2C0; /* mBG.m5C.m04 */
    gabi::store<u8>(m + 0x45, 1); /* SetCam() */
    gabi::at<cXyz>(m + 0x24)->copy(*pp);
    gabi::store<u8>(m + 0x44, 0); /* ClrObj() */
    f32 camY = gabi::call<f32>(0x02008974, dComIfGp_ea() + PLAY_BGS, m);
    f32 m5C;
    if (camY < groundY) {
        /* mBG.m5C.m04 = gnd_chk (memberwise copy) */
        static const u8 words[] = {0x04, 0x1C, 0x28, 0x38, 0x18, 0x00, 0x24, 0x3C, 0x08, 0x50, 0x2C, 0x30};
        static const u8 bytes[] = {0x0C, 0x47, 0x44, 0x46, 0x45, 0x48, 0x4A, 0x49};
        for (u8 w : words) gabi::store<u32>(m + w, gabi::load<u32>(o + w));
        for (u8 bb : bytes) gabi::store<u8>(m + bb, gabi::load<u8>(o + bb));
        gabi::store<u16>(m + 0x14, gabi::load<u16>(o + 0x14));
        gabi::store<u16>(m + 0x16, gabi::load<u16>(o + 0x16));
        gabi::store<u32>(m + 0x34, gabi::load<u32>(o + 0x34));
        mBG_m5C.m58 = groundY;
        m5C = groundY;
    } else {
        mBG_m5C.m58 = camY;
        m5C = camY;
    }
    gabi::store<u32>(ea() + 0x288, gxBits); /* mBG.m00.m04.SetPos(&gnd_chk_pos) */
    gabi::store<u32>(ea() + 0x28C, gyBits);
    gabi::store<u32>(ea() + 0x290, gzBits);
    mBG_m5C.m00 = m5C != -1000000000.0f;
    f32 m00 = gabi::call<f32>(0x02008974, dComIfGp_ea() + PLAY_BGS, ea() + 0x264);
    mBG_m00.m58 = m00;
    m354 = m00;
    mBG_m00.m00 = m00 != -1000000000.0f;
    f32 foot = footHeightOf(mpPlayerActor.get());
    m360 = !(foot - mBG_m5C.m58 > gabi::load<f32>(ea() + 0x844) /* FloorMargin */);

    m31D = 0;
    m33C = nullptr;
    if (gabi::call<bool>(0x024EEABC, dComIfGp_ea() + PLAY_BGS, ea() + 0x2D4)) { /* ChkMoveBG */
        fopAc_ac_c* a = gabi::call<fopAc_ac_c*>(0x02008438, dComIfGp_ea() + PLAY_BGS, (u32)gabi::load<u16>(ea() + 0x2D6)); /* GetActorPointer */
        m33C = a;
        if (a != nullptr) {
            positionOf(pos, a);
            directionOf(dir, m33C.get());
            if (m31C != 0) {
                cXyz_mi(&m32C, t, pos);
                m320.copy(*t);
                gabi::call(0x020068B0, &m33A, ang.get(), dir.get()); /* m33A - angle */
                fopAc_ac_c* mv = m33C.get();
                m338 = (s16)*ang;
                if (mv != nullptr && fpcM_GetName(mv) == 0x39 /* Obj_Pirateship */) {
                    mViewCache.mCenter.y = gabi::fmadds(m320.y, gabi::load<f32>(ea() + 0x7FC) /* m0B8 */, mViewCache.mCenter.y);
                }
            }
            m31C = 1;
            if (gabi::load<s32>(dComIfGp_ea() + 0x52E4) == 0 && !chkFlag(0x20000000) && m360 != 0) m31D = 1;
            if (m31D != 0) {
                gabi::call(0x024EFB08, dComIfGp_ea() + PLAY_BGS, ea() + 0x2D4, 1, &mViewCache.mCenter, 0, 0);
                gabi::call(0x024EFB08, dComIfGp_ea() + PLAY_BGS, ea() + 0x2D4, 1, &mViewCache.mEye, 0, 0);
                cXyz_mi(&mViewCache.mEye, t, &mViewCache.mCenter);
                cSGlobe_Val(&mViewCache.mDirection, t);
            }
            m33A = (s16)*dir;
            m32C.copy(*pos);
        }
    } else {
        m31C = 0;
    }
    m350 = mBG_m5C.m00 != 0 ? gabi::call<s32>(0x024EF324, dComIfGp_ea() + PLAY_BGS, ea() + 0x2D4) /* GetCamMoveBG */ : 0;
    mRoomNo = -1;
    bool swim = false;
    if (mBG_m00.m00 != 0) {
        s32 pad = mPadId;
        swim = (gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CD8) & 0x100000) != 0; /* SWIM */
    }
    if (swim) {
        mRoomMapToolCameraIdx = gabi::call<s32>(0x024EEC9C, dComIfGp_ea() + PLAY_BGS, (u32)gabi::load<u16>(ea() + 0x27A),
                                                (u32)gabi::load<u16>(ea() + 0x278)); /* GetPolyCamId */
    } else if (m360 == 0) {
        mRoomMapToolCameraIdx = 0x1FF;
    } else if (mBG_m5C.m00 != 0) {
        s32 idx = gabi::call<s32>(0x024EF340, dComIfGp_ea() + PLAY_BGS, ea() + 0x2D4); /* GetRoomCamId */
        mRoomMapToolCameraIdx = idx;
        if (idx == 0xFF) {
            mRoomMapToolCameraIdx = gabi::call<s32>(0x024EEC9C, dComIfGp_ea() + PLAY_BGS, (u32)gabi::load<u16>(ea() + 0x2D6),
                                                    (u32)gabi::load<u16>(ea() + 0x2D4));
        } else {
            mRoomNo = gabi::call<s32>(0x024EF130, dComIfGp_ea() + PLAY_BGS, ea() + 0x2D4); /* GetRoomId */
        }
    } else {
        mRoomMapToolCameraIdx = 0xFF;
    }
    if (daSea_ChkArea(pp->x, pp->z)) {
        f32 w = daSea_calcWave(pp->x, pp->z);
        m314 = 1;
        m318 = w;
    } else {
        m314 = 0;
        m318 = -1000000000.0f;
    }
    if (m354 < m318) m354 = (f32)m318;

    u32 o2 = gabi::ea(g2.get());
    gndChk_ct(o2, &mEye); /* dBgS_GndChk gnd_chk_2; SetPos(&mEye) */
    f32 eyeGround = gabi::call<f32>(0x02008974, dComIfGp_ea() + PLAY_BGS, o2);
    if (eyeGround < mBG_m5C.m58 + 40.0f) {
        A->copy(mEye);
        attentionPos(t, mpPlayerActor.get());
        B->copy(*t);
        cXyz_mi(A, d, B);
        cXyz_ml(d, t, 0.5f);
        PSVECAdd(B, t, B);
    } else {
        B->copy(mEye);
        attentionPos(t, mpPlayerActor.get());
        A->copy(*t);
        cXyz_mi(B, d, A);
        cXyz_ml(d, t, 0.5f);
        PSVECAdd(A, t, A);
    }
    if (m360 != 0) {
        u32 res = lineCollisionCheckBush(A, B) & 5;
        m364 = res;
        if (res & 4) {
            gabi::store<u32>(ea() + 0x36C, gabi::load<u32>(ea() + 0x804)); /* m368 = mCamSetup.m0C0 */
            res = m364;
        }
        if (res & 1) {
            gabi::store<u32>(ea() + 0x36C, gabi::load<u32>(ea() + 0x808)); /* m368 = mCamSetup.m0C4 */
            res = m364;
        }
        if (res != 0) gabi::call(0x02516E00, dComIfGp_ea() + 0x4EF8, &m36C); /* GetMassCamTopPos */
    } else {
        m364 = 0;
        m368 = 0.0f;
    }
    gndChk_dt(o2);
    gabi::store<u32>(o + 0x20, 0x1004A50C);
    gabi::store<u32>(o + 0x40, 0x1004A52C);
    gabi::store<u32>(o + 0x4C, 0x1004A4EC);
    gabi::call(0x02008DAC, o, 0);
    gabi::store<u32>(r + 0x20, 0x1004A5BC);
    gabi::store<u32>(r + 0x24, 0x1004A5DC);
    gabi::store<u32>(r + 0x30, 0x1004A4EC);
    gabi::call(0x02008B4C, r + 0x10, 0);
}
VERIFY(0x024F9044, &dCamera_c::checkGroundInfo);

static inline f32 regF(u32 addr) { return gabi::load<f32>(addr); } /* HIO register (REG*_F) */

/* 024FD11C. HD: the gaze back margin grows while swimming / in mode 4 (debug-register tuned); a
 * missing second wall plane counts as no hit; while swimming the eye keeps a register-defined
 * distance to the player; the corner case moves V towards the corner (GameCube wrote it into U);
 * the wall-up offset tilts V by +90 degrees */
bool dCamera_c::bumpCheck(u32 flags) {
    WWHD_FUNC(0x024FD11C, bool, this, flags);
    const u32 REG = 0x1047B60C;
    const u32 PREV_HIT = 0x101D554C; /* static prev_hit_type */
    u32 res = 0;
    f32 gaze = gabi::load<f32>(ea() + 0x874); /* GazeBackMargin */
    s32 pad = mPadId;
    u32 st1 = gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CDC);
    f32 cornerMax = gabi::load<f32>(ea() + 0x888); /* CornerAngleMax */
    f32 cushion = gabi::load<f32>(ea() + 0x878);   /* CornerCushion */
    gabi::Local<u8[8]> deg;
    if (st1 & 0x100000) {
        gaze = regF(REG + 0xFC4) + 3.0f;
    } else if (mCurMode == 4) {
        gaze = regF(REG + 0x1174) + 5.0f;
    }
    u32 dg = gabi::call<u32>(0x02006A3C, deg.get(), cornerMax); /* cDegree(CornerAngleMax) */
    f32 cosMax = gabi::call<f32>(0x02006AE8, dg);                 /* .Cos() */
    fopAc_ac_c* player = mpPlayerActor.get();
    f32 wallUp = gabi::load<f32>(ea() + 0x880); /* WallUpDistance */
    if (player != nullptr && fpcM_GetName(player) == 0xA8) {
        u32 vt = gabi::load<u32>(gabi::ea(player) + 0xB4);
        u32 grabId = gabi::call_ptr<u32>(gabi::load<u32>(vt + 0xBC), player); /* getGrabActorID() */
        if (grabId != 0xFFFFFFFFu) {
            gabi::Local<be<u32>> key;
            *key = grabId;
            fopAc_ac_c* grab = fopAcIt_Judge(0x025E1234, key.get());
            if (grab != nullptr) {
                s16 name = fpcM_GetName(grab);
                if (name == 0x1C5 /* TSUBO */) {
                    u32 type = gabi::call<u32>(0x02048038, grab, 4, 0x18); /* daObj::PrmAbstract */
                    bool tall = (type >= 1 && type <= 4) || (type >= 7 && type <= 8) || (type >= 13 && type <= 15);
                    wallUp = tall ? 150.0f : 110.0f;
                } else if (name == 0x16F /* NPC_MD */) {
                    wallUp = 130.0f;
                } else if (name == 0x1CA /* Obj_Try */) {
                    wallUp = 200.0f;
                } else {
                    wallUp = 110.0f;
                }
            }
        }
    }
    gabi::Local<cXyz> eye;
    gabi::Local<cSGlobe_l> dir;
    eye->copy(mViewCache.mEye);
    gabi::call(0x02007178, dir.get(), &mViewCache.mDirection); /* cSGlobe direction(mViewCache.mDirection) */

    if (chkFlag(0x2000) && mpLockonTarget.get() != nullptr) {
        gabi::Local<cXyz> x, e;
        f32 r = radiusActorInSight(mpPlayerActor.get(), mpLockonTarget.get());
        f32 m;
        if (r > 0.0f) {
            f32 cur = m14C;
            if (!(r < 3500.0f)) r = 3500.0f;
            m = gabi::fmadds(r - cur, 0.33f, cur);
            res = 0x40;
        } else {
            f32 cur = m14C;
            m = gabi::fnmsubs(cur, 0.08f, cur);
        }
        u32 t108 = m108;
        m14C = m;
        f32 k = 1.0f;
        if (t108 < 10) k = (f32)t108 / 10.0f;
        dir->mRadius = gabi::fmadds(m, k, dir->mRadius);
        gabi::call(0x020073AC, dir.get(), x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        eye->copy(*e);
    }
    if ((flags & 0x40) && m364 != 0) {
        gabi::Local<cXyz> d, x, e;
        gabi::Local<cSGlobe_l> g;
        gabi::Local<cSAngle_l> a, t, u, tmp;
        cXyz_mi(&m36C, d, &mViewCache.mCenter);
        gabi::call(0x02007324, g.get(), d.get());
        if ((s16)dir->mV < (s16)g->mV) {
            gabi::call(0x02006644, a.get(), &mDirection.mV);
            gabi::call(0x020068B0, &g->mV, t.get(), a.get());
            gabi::call(0x0200693C, t.get(), u.get(), 0.05f);
            gabi::call(0x020068CC, a.get(), u.get());
            cSAngle_l* v = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)*a);
            dir->mV = (s16)*v;
            gabi::call(0x020073AC, dir.get(), x.get());
            cXyz_pl(&mViewCache.mCenter, e, x);
            eye->copy(*e);
            res |= 0x20;
        }
    }

    gabi::Local<u8[0x6C]> lc1, lc2;
    u32 o1 = gabi::ea(lc1.get()), o2 = gabi::ea(lc2.get());
    camLinChk_ct(o1);
    camLinChk_ct(o2);
    s32 type;
    if (lineBGCheck(&mCenter, eye, (u8*)lc1.get(), flags)) {
        gabi::Local<cSAngle_l> unusedA;
        gabi::Local<cSGlobe_l> unusedG, globe;
        gabi::Local<cXyz> c, w, w278, cross, mid, d, h, midc, out, nsum, n2, s, x, p, p2, c2, att;
        cSAngle_ct(unusedA);
        gabi::call(0x02007100, unusedG.get());
        gabi::call(0x02007100, globe.get());
        u32 pla1 = gabi::call<u32>(0x020084C8, dComIfGp_ea() + PLAY_BGS, (u32)gabi::load<u16>(o1 + 0x16), (u32)gabi::load<u16>(o1 + 0x14));
        u32 pla2 = 0;
        if (!(flags & 0x20)) {
            type = 2;
        } else if (pla1 != 0 && lineBGCheck(eye, &mCenter, (u8*)lc2.get(), flags)) {
            pla2 = gabi::call<u32>(0x020084C8, dComIfGp_ea() + PLAY_BGS, (u32)gabi::load<u16>(o2 + 0x16), (u32)gabi::load<u16>(o2 + 0x14));
            if (pla2 == 0) {
                mEye.copy(*eye);
                gabi::store<u32>(ea() + 0x8, gabi::load<u32>(gabi::ea(dir.get())));
                gabi::store<u32>(ea() + 0xC, gabi::load<u32>(gabi::ea(dir.get()) + 4));
                type = 0;
                goto water;
            }
            f32 dot = PSVECDotProduct((cXyz*)gabi::at<cXyz>(pla1), gabi::at<cXyz>(pla2));
            gabi::call(0x028E8D4C, pla1, pla2, cross.get()); /* PSVECCrossProduct */
            if (dot > cosMax && std::fabs((f32)cross->y) > 0.5f) type = 3;
            else type = gabi::load<s32>(PREV_HIT) == 3 ? 5 : 4;
        } else {
            s32 prev = gabi::load<s32>(PREV_HIT);
            type = (prev == 3 || prev == 5) ? 5 : 2;
        }

        if (type == 3) {
            c->copy(*gabi::at<cXyz>(o1 + 0x30));  /* lin_chk1.GetCross() */
            c2->copy(*gabi::at<cXyz>(o2 + 0x30)); /* lin_chk2.GetCross() */
            res |= 2;
            cXyz_mi(c2, d, c);
            cXyz_ml(d, h, 0.5f);
            cXyz_pl(c, mid, h);
            midc->copy(*mid);
            if (gabi::call<bool>(0x02017AEC, pla1, pla2, midc.get(), out.get())) { /* cM3d_2PlaneLinePosNearPos */
                cXyz_pl(gabi::at<cXyz>(pla1), nsum, gabi::at<cXyz>(pla2));
                n2->copy(*nsum);
                cXyz_ml(n2, s, 2.0f);
                cXyz_pl(out, nsum, s);
                m070.copy(*nsum);
                cXyz_mi(&m070, s, &mCenter);
                cSGlobe_Val(globe, s);
                gabi::Local<cSAngle_l> t1, t2, t3, tv;
                gabi::store<u32>(ea() + 0x8, gabi::load<u32>(gabi::ea(dir.get()))); /* mDirection.R(direction.R()) */
                gabi::call(0x020068B0, &globe->mV, t1.get(), &mDirection.mV);
                gabi::call(0x0200693C, t1.get(), t2.get(), 0.05f);
                gabi::call(0x02006894, &mDirection.mV, t3.get(), t2.get());
                cSAngle_l* v = gabi::call<cSAngle_l*>(0x0200658C, tv.get(), (s16)*t3);
                mDirection.mV = (s16)*v;
                gabi::call(0x020068B0, &globe->mU, t3.get(), &mDirection.mU);
                gabi::call(0x0200693C, t3.get(), t2.get(), cushion);
                gabi::call(0x02006894, &mDirection.mU, t1.get(), t2.get());
                v = gabi::call<cSAngle_l*>(0x0200658C, tv.get(), (s16)*t1);
                mDirection.mU = (s16)*v;
                gabi::call(0x020073AC, &mDirection, s.get());
                cXyz_pl(&mCenter, p, s);
                globe->mRadius = globe->mRadius + 50.0f;
                gabi::call(0x020073AC, globe.get(), s.get());
                cXyz_pl(&mCenter, p2, s);
                if (!gabi::call<bool>(0x024FCBE8, this, &mCenter, p2.get(), 0x7F)) { /* lineBGCheck(&mCenter, &p2, 0x7f) */
                    if (lineBGCheck(&m070, p, (u8*)lc1.get(), 0x7F)) {
                        gabi::Local<cXyz> cc, o;
                        cc->copy(*gabi::at<cXyz>(o1 + 0x30));
                        compWallMargin(o, cc, gaze);
                        p->copy(*o);
                    }
                    lineBGCheck(&mCenter, p, (u8*)lc1.get(), flags);
                    mEye.copy(*p);
                    mEventFlags = mEventFlags | 0x80000;
                    goto water;
                }
                type = 2;
            }
        }
        /* cases 2, 4, 5 (and 3 when the corner could not be used) */
        mEventFlags = mEventFlags | 0x80;
        c->copy(*gabi::at<cXyz>(o1 + 0x30));
        compWallMargin(w, c, gaze + 0.5f);
        w278->copy(*w);
        if (chkFlag(8) && (flags & 0x10) && type != 4) {
            f32 hd = gabi::call<f32>(0x024F7AC4, c.get(), &mCenter); /* dCamMath::xyzHorizontalDistance */
            attentionPos(att, mpPlayerActor.get());
            f32 cy = mCenter.y;
            f32 up = wallUp - (cy - att->y);
            if (!(hd < 20.0f)) {
                if (hd > 320.0f) up = 0.0f;
                else up = up * (1.0f - (hd - 20.0f) / 300.0f);
            }
            if (w->y - cy < up) {
                gabi::Local<cSGlobe_l> g;
                gabi::Local<cSAngle_l> sa, tmp;
                n2->copy(*gabi::at<cXyz>(pla1)); /* *plane1->GetNP() */
                gabi::call(0x02007324, g.get(), n2.get());
                gabi::call(0x02006894, &g->mV, sa.get(), 0x101FF358u); /* V + cSAngle::_90 */
                cSAngle_l* v = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)*sa);
                g->mV = (s16)*v;
                f32 sn = gabi::call<f32>(0x02006814, &g->mV); /* Sin */
                g->mRadius = up * sn;
                gabi::call(0x020073AC, g.get(), x.get());
                PSVECAdd(w, x, w);
                if (lineBGCheck(w278, w, (u8*)lc1.get(), flags)) {
                    gabi::Local<cXyz> cc, o;
                    cc->copy(*gabi::at<cXyz>(o1 + 0x30));
                    compWallMargin(o, cc, gaze);
                    w->copy(*o);
                    cXyz_mi(w, d, &mEye);
                    cXyz_ml(d, o, gabi::load<f32>(ea() + 0x87C) /* WallCushion */);
                    PSVECAdd(&mEye, o, &mEye);
                } else {
                    cXyz_mi(w, d, &mEye);
                    cXyz_ml(d, h, gabi::load<f32>(ea() + 0x87C));
                    PSVECAdd(&mEye, h, &mEye);
                }
                mEventFlags = mEventFlags | 0x4000;
            } else {
                if (lineBGCheck(w278, w, (u8*)lc1.get(), flags)) {
                    gabi::Local<cXyz> cc, o;
                    cc->copy(*gabi::at<cXyz>(o1 + 0x30));
                    compWallMargin(o, cc, gaze);
                    w->copy(*o);
                }
                cXyz_mi(w, d, &mEye);
                cXyz_ml(d, h, gabi::load<f32>(ea() + 0x884) /* WallBackCushion */);
                PSVECAdd(&mEye, h, &mEye);
            }
        } else {
            mEye.copy(*w);
        }
        {
            s32 pd = mPadId;
            if (gabi::load<u32>(dComIfGp_ea() + pd * 0x10 + 0x5CDC) & 0x100000) { /* swimming */
                gabi::Local<cSGlobe_l> g;
                attentionPos(att, mpPlayerActor.get());
                cXyz_mi(&mEye, d, att);
                gabi::call(0x02007324, g.get(), d.get());
                f32 lim = regF(REG + 0xFC8) + 100.0f;
                if (g->mRadius < lim) {
                    g->mRadius = lim;
                    gabi::call(0x020073AC, g.get(), x.get());
                    cXyz_pl(att, h, x);
                    mEye.copy(*h);
                }
            }
        }
        {
            u32 alg = gabi::load<u32>(gabi::load<u32>(ea() + 0x8A8) + 4); /* mCamParam.Algorythmn() */
            if (alg == 1 || alg == 10) { /* FOLLOW, MANUAL */
                gabi::Local<cSGlobe_l> g;
                attentionPos(att, mpPlayerActor.get());
                cXyz_mi(&mEye, d, att);
                gabi::call(0x02007324, g.get(), d.get());
                if (g->mRadius < 40.0f) {
                    g->mRadius = 40.0f;
                    gabi::call(0x020073AC, g.get(), x.get());
                    cXyz_pl(att, h, x);
                    mEye.copy(*h);
                }
            }
        }
        cXyz_mi(&mEye, d, &mCenter);
        cSGlobe_Val(&mDirection, d);
        res |= 1;
    } else {
        type = 0;
        if (chkFlag(0x4000)) {
            gabi::Local<cSAngle_l> t, u, v2, tv;
            gabi::Local<cXyz> x, e;
            f32 k = (flags & 0x10) ? gabi::load<f32>(ea() + 0x884) /* WallBackCushion */ : 0.2f;
            f32 rr = mDirection.mRadius;
            mDirection.mRadius = gabi::fmadds(mViewCache.mDirection.mRadius - rr, k, rr);
            gabi::call(0x020068B0, &mViewCache.mDirection.mV, t.get(), &mDirection.mV);
            gabi::call(0x0200693C, t.get(), u.get(), k);
            gabi::call(0x02006894, &mDirection.mV, v2.get(), u.get());
            cSAngle_l* a = gabi::call<cSAngle_l*>(0x0200658C, tv.get(), (s16)*v2);
            mDirection.mV = (s16)*a;
            a = gabi::call<cSAngle_l*>(0x0200658C, tv.get(), (s16)mViewCache.mDirection.mU);
            mDirection.mU = (s16)*a;
            gabi::call(0x020073AC, &mDirection, x.get());
            cXyz_pl(&mCenter, e, x);
            mEye.copy(*e);
            if (lineBGCheck(&mCenter, &mEye, (u8*)lc1.get(), flags)) {
                gabi::Local<cXyz> cc, o;
                cc->copy(*gabi::at<cXyz>(o1 + 0x30));
                compWallMargin(o, cc, gaze + 0.5f);
                mEye.copy(*o);
            }
            gabi::call(0x020068B0, &mDirection.mV, t.get(), &mViewCache.mDirection.mV);
            f32 dd = gabi::call<f32>(0x02006720, t.get()); /* Degree */
            if (std::fabs(dd) < 0.2f) mEventFlags = mEventFlags & ~0x4000u;
        } else {
            mEye.copy(*eye);
            gabi::store<u32>(ea() + 0x8, gabi::load<u32>(gabi::ea(dir.get())));
            gabi::store<u32>(ea() + 0xC, gabi::load<u32>(gabi::ea(dir.get()) + 4));
        }
    }
water:
    if (flags & 8) {
        f32 wh = getWaterSurfaceHeight(&mEye);
        if (wh > mEye.y) {
            gabi::Local<cXyz> d;
            mEye.y = wh;
            cXyz_mi(&mEye, d, &mCenter);
            cSGlobe_Val(&mDirection, d);
            res |= 8;
        }
    }
    gabi::store<s32>(PREV_HIT, type);
    if (m78B != 0) {
        if (camStyleAlg(mCurStyle) != 4 || !(mEventFlags & 0x10000800)) mEye.y = mEye.y + 25.0f;
    }
    bool ret = res != 0;
    camLinChk_dt_vtables(o2);
    gabi::call(0x02008B4C, o2, 0);
    camLinChk_dt_vtables(o1);
    gabi::call(0x02008B4C, o1, 0);
    return ret;
}
VERIFY(0x024FD11C, &dCamera_c::bumpCheck);
