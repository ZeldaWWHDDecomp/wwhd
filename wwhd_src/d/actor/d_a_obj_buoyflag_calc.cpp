/**
 * d_a_obj_buoyflag_calc.cpp (WWHD)
 * Object - Buoy flag: cloth simulation (Packet_c::init / calc / calc_pos / calc_nrm / wind).
 *
 * The GameCube TU is "Nonmatching": written from the
 * WWHD code (cking.rpx) and verified against it, with the GameCube names.
 */
#include "d/actor/d_a_obj_buoyflag.h"

namespace daObjBuoyflag {

static inline u32 buf_ea(Packet_c* p, u32 idx) { return gabi::ea(p) + 0x9C + idx * 0x4EC; }

/* 02329CE4 */
void Packet_c::calc_wind_base(Act_c* a) {
    WWHD_FUNC(0x02329CE4, void, this, a);
    mAng[0] += 0x578;
    mAng[1] += 0x157C;
    mAng[2] += 0x690;
    mAng[3] += 0x1DE2;
    mAng[4] += 0x76C;
    mAng[5] += 0x1964;
    cXyz* oldPos = gabi::at<cXyz>(buf_ea(this, (u32)mCurBuf ^ 1));
    if (!(cM_rnd() >= 0.05f)) {
        mAng[0] += 2000;
        mAng[1] += 2000;
        mAng[2] += 2000;
        mAng[3] += 2000;
        mAng[4] += 2000;
        mAng[5] += 2000;
    }
    mAng[6] += 0x1AE;
    mAng[7] += 0x5DC;
    mAng[8] += 0x1F40;
    mAng[9] += 0x3E8;
    mAng[10] += 0xC8A;
    mAng[11] += 0x223D;
    s16 rx = (s16)gabi::ftoi(gabi::fmadds(cM_ssin(mAng[0]), 4096.0f, cM_ssin(mAng[1]) * 1024.0f));
    s16 ry = (s16)gabi::ftoi(gabi::fmadds(cM_ssin(mAng[2]), 4608.0f, cM_ssin(mAng[3]) * 1536.0f));
    s16 rz = (s16)gabi::ftoi(gabi::fmadds(cM_ssin(mAng[4]), 5632.0f, cM_ssin(mAng[5]) * 2048.0f));
    f32 w = gabi::fmadds(cM_ssin(mAng[8]), 0.15f, gabi::fmadds(cM_ssin(mAng[6]), 0.5f, cM_ssin(mAng[7]) * 0.35f));
    f32 pow = gabi::fmadds(w + 1.0f, 0.35f, 0.2f);
    pow = pow + cM_rndF(0.2f);

    gabi::Local<cXyz> allWind;
    gabi::Local<cXyz> wind;
    gabi::Local<cXyz> move;
    dKyw_get_AllWind_vecpow(allWind, oldPos);
    wind->copy(*allWind);
    PSVECScale(wind, wind, 20.0f * (0.5f * pow));
    /* the setup matrix's translation since last frame */
    move->x = (a->mOldMtx.m[0][3] - a->mMtx.m[0][3]) * 0.2f;
    move->y = (a->mOldMtx.m[1][3] - a->mMtx.m[1][3]) * 0.2f;
    move->z = (a->mOldMtx.m[2][3] - a->mMtx.m[2][3]) * 0.2f;
    f32 mag = PSVECSquareMag(move);
    if (!(mag <= 625.0f)) {
        PSVECScale(move, move, 1.0f / std_sqrtf(mag));
        PSVECScale(move, move, 25.0f);
    }
    PSVECAdd(wind, move, wind);
    PSMTXCopy(&a->mMtx, mDoMtx_stack_c::get());
    PSMTXInverse(mDoMtx_stack_c::get(), mDoMtx_stack_c::get());
    PSMTXMultVecSR(mDoMtx_stack_c::get(), cXyz_BaseY, &mUp);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), rx, ry, rz);
    PSMTXMultVecSR(mDoMtx_stack_c::get(), wind, &mWind);
    mag = PSVECSquareMag(&mWind);
    if (!(mag >= 0.25f)) {
        if (!(mag >= 0.001f)) {
            PSMTXMultVecSR(mDoMtx_stack_c::get(), cXyz_BaseZ, &mWind);
            PSVECScale(&mWind, &mWind, 0.5f);
        } else {
            PSVECScale(&mWind, &mWind, 0.5f / std_sqrtf(mag));
        }
    }
}
VERIFY(0x02329CE4, &Packet_c::calc_wind_base);

/* 0232A13C */
void Packet_c::calc_pos_spring_near(const cXyz* p, const cXyz* other, f32 len, f32 k) {
    WWHD_FUNC(0x0232A13C, void, this, p, other, len, k);
    gabi::Local<cXyz> d;
    gabi::Local<cXyz> t1;
    gabi::Local<cXyz> t2;
    gabi::Local<cXyz> t3;
    cXyz_mi(p, d, other);
    f32 dist = std_sqrtf(PSVECSquareMag(d));
    if (!(dist <= 0.01f)) {
        cXyz_ml(d, t1, dist - len);
        cXyz_ml(t1, t2, -k);
        cXyz_dv(t2, t3, dist);
        PSVECAdd(&mSpring, t3, &mSpring);
    }
}
VERIFY(0x0232A13C, &Packet_c::calc_pos_spring_near);

/* 0232A218 */
void Packet_c::calc_pos(Act_c* a) {
    WWHD_FUNC(0x0232A218, void, this, a);
    u32 cur = (u32)mCurBuf;
    u32 curPos = buf_ea(this, cur);
    u32 oldPos = buf_ea(this, cur ^ 1);
    calc_wind_base(a);
    gabi::Local<cXyz> up;
    gabi::Local<cXyz> nrmF;
    gabi::Local<cXyz> wind;
    gabi::Local<cXyz> d;
    gabi::Local<cXyz> drag;
    gabi::Local<cXyz> np;
    for (int j = 0; j < ROWS; j++) {
        f32 fj = (f32)j;
        f32 fj4 = (f32)(4 - j);
        f32 dy = gabi::fmsubs(fj, 0.25f, 0.5f);
        f32 hy = fj4 * 0.25f;
        for (int i = 0; i < COLS; i++) {
            if ((j == 0 || j == 4) && i == 0)
                continue; /* the corners on the pole */
            u32 off = j * 0x54 + i * 0xC;
            u32 ob = buf_ea(this, (u32)mCurBuf ^ 1);
            mSpring.copy(*cXyz_Zero);
            cXyz* p = gabi::at<cXyz>(ob + off);
            if (i > 0)
                calc_pos_spring_near(p, gabi::at<cXyz>(ob - 0xC + off), 12.5f, 0.5f);
            if (i < 6)
                calc_pos_spring_near(p, gabi::at<cXyz>(ob + off + 0xC), 12.5f, 0.5f);
            if (j > 0)
                calc_pos_spring_near(p, gabi::at<cXyz>(ob - 0x54 + off), 12.5f, 0.4f);
            if (j < 4)
                calc_pos_spring_near(p, gabi::at<cXyz>(ob + off + 0x54), 12.5f, 0.5f);
            f32 hx = (f32)i * 0.16666667f;
            /* gravity along the pole */
            cXyz_ml(&mUp, up, (-0.03f * (hy + hx)) * 0.5f);
            PSVECAdd(&mSpring, up, &mSpring);
            /* wind on the normal, with a travelling wave */
            cXyz* nrm = gabi::at<cXyz>(buf_ea(this, (u32)mCurBuf ^ 1) + 0x1A4 + off);
            f32 r = std_sqrtf(gabi::fmadds(dy, dy, hx * hx));
            f32 ph = 32768.0f * r;
            u16 a0 = (u16)gabi::ftoi(ph + (f32)(s16)mAng[9]);
            u16 a1 = (u16)gabi::ftoi(ph + (f32)(s16)mAng[10]);
            u16 a2 = (u16)gabi::ftoi(ph + (f32)(s16)mAng[11]);
            f32 wave = gabi::fmadds((cM_ssin(a0) + cM_ssin(a1)) + cM_ssin(a2), 0.33333334f, 1.0f);
            f32 dot = PSVECDotProduct(nrm, &mWind);
            cXyz_ml(nrm, nrmF, dot * ((wave * 0.6f) * 0.05f));
            PSVECAdd(&mSpring, nrmF, &mSpring);
            cXyz* spd = &mSpd[j][i];
            PSVECAdd(spd, &mSpring, spd);
            /* drag against the (jittered) wind */
            f32 k = -(0.03f * gabi::fmadds(hx, 0.4f, 0.6f));
            wind->x = (f32)mWind.x;
            wind->y = (f32)mWind.y;
            wind->z = (f32)mWind.z;
            f32 rx = cM_rndF(0.2f) + 0.9f;
            f32 ry = cM_rndF(0.2f) + 0.9f;
            f32 rz = cM_rndF(0.2f) + 0.9f;
            wind->x = wind->x * rx;
            wind->y = wind->y * ry;
            wind->z = wind->z * rz;
            cXyz_mi(spd, d, wind);
            cXyz_ml(d, drag, k);
            PSVECAdd(spd, drag, spd);
            cXyz_pl(gabi::at<cXyz>(oldPos + off), np, spd);
            gabi::at<cXyz>(curPos + off)->copy(*np);
        }
    }
}
VERIFY(0x0232A218, &Packet_c::calc_pos);

/* 0232A7F4: weighted central differences along the column and the row, normal = row x column */
void Packet_c::calc_nrm() {
    WWHD_FUNC(0x0232A7F4, void, this);
    u32 cur = (u32)mCurBuf;
    u32 curBuf = buf_ea(this, cur);
    u32 old = buf_ea(this, cur ^ 1);
    gabi::Local<cXyz> t;      /* scratch result */
    gabi::Local<cXyz> e0;     /* edge vectors */
    gabi::Local<cXyz> e1;
    gabi::Local<cXyz> s0;
    gabi::Local<cXyz> s1;
    gabi::Local<cXyz> V;
    gabi::Local<cXyz> H;
    gabi::Local<cXyz> N;
    gabi::Local<cXyz> N2;
    for (int j = 0; j < ROWS; j++) {
        for (int i = 0; i < COLS; i++) {
            u32 off = j * 0x54 + i * 0xC;
            cXyz* P = gabi::at<cXyz>(old + off);
            if (j == 0) {
                cXyz_mi(gabi::at<cXyz>(old + i * 0xC + 0x54), t, P);
                V->copy(*t);
            } else if (j == 4) {
                cXyz_mi(P, t, gabi::at<cXyz>(old + i * 0xC + 0xFC));
                V->copy(*t);
            } else {
                cXyz* A = gabi::at<cXyz>(old - 0x54 + off);
                cXyz* B = gabi::at<cXyz>(gabi::ea(P) + 0x54);
                cXyz_mi(P, t, A);
                e0->copy(*t);
                cXyz_mi(B, t, P);
                e1->copy(*t);
                cXyz_ml(A, t, 0.57475f);
                s0->copy(*t);
                cXyz_ml(e0, t, 0.358875f);
                PSVECAdd(s0, t, s0);
                cXyz_ml(e1, t, 0.111375f);
                PSVECAdd(s0, t, s0);
                cXyz_ml(B, t, 0.42524996f);
                PSVECAdd(s0, t, s0);
                cXyz_ml(A, t, 0.42524996f);
                s1->copy(*t);
                cXyz_ml(e0, t, 0.383625f);
                PSVECAdd(s1, t, s1);
                cXyz_ml(e1, t, 0.136125f);
                PSVECAdd(s1, t, s1);
                cXyz_ml(B, t, 0.57475f);
                PSVECAdd(s1, t, s1);
                cXyz_mi(s1, t, s0);
                V->copy(*t);
            }
            if (i == 0) {
                cXyz_mi(gabi::at<cXyz>(old + j * 0x54 + 0xC), t, P);
                H->copy(*t);
            } else if (i == 6) {
                cXyz_mi(P, t, gabi::at<cXyz>(old - 0xC + j * 0x54 + 0x48));
                H->copy(*t);
            } else {
                cXyz* A = gabi::at<cXyz>(old - 0xC + off);
                cXyz* B = gabi::at<cXyz>(gabi::ea(P) + 0xC);
                cXyz_mi(P, t, A);
                e0->copy(*t);
                cXyz_mi(B, t, P);
                e1->copy(*t);
                cXyz_ml(A, t, 0.57475f);
                s0->copy(*t);
                cXyz_ml(e0, t, 0.358875f);
                PSVECAdd(s0, t, s0);
                cXyz_ml(e1, t, 0.111375f);
                PSVECAdd(s0, t, s0);
                cXyz_ml(B, t, 0.42524996f);
                PSVECAdd(s0, t, s0);
                cXyz_ml(A, t, 0.42524996f);
                s1->copy(*t);
                cXyz_ml(e0, t, 0.383625f);
                PSVECAdd(s1, t, s1);
                cXyz_ml(e1, t, 0.136125f);
                PSVECAdd(s1, t, s1);
                cXyz_ml(B, t, 0.57475f);
                PSVECAdd(s1, t, s1);
                cXyz_mi(s1, t, s0);
                H->copy(*t);
            }
            cXyz_outprod(H, N, V);
            N2->copy(*N);
            if (cXyz_normalizeRS(N2)) {
                copy3_u32(curBuf + 0x1A4 + off, gabi::ea(N2.get()));
                copy3_u32(curBuf + 0x348 + off, gabi::ea(N2.get()));
                cXyz* back = gabi::at<cXyz>(curBuf + 0x348 + off);
                PSVECScale(back, back, -1.0f);
            }
        }
    }
}
VERIFY(0x0232A7F4, &Packet_c::calc_nrm);

/* 0232AEF8 */
void Packet_c::calc(Act_c* a) {
    WWHD_FUNC(0x0232AEF8, void, this, a);
    PSMTXCopy(&a->mMtx, mDoMtx_stack_c::get());
    mDoMtx_stack_scaleM(a->scale.x, a->scale.y, a->scale.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtxHasi);
    if (attr_type(a, 0x10026584, 0x1002659C)->mHata) {
        /* static cXyz l_offset(0, 60, 0): guard 0x10469380, object 0x10469374 */
        cXyz* l_offset = gabi::at<cXyz>(0x10469374);
        if (gabi::load<u32>(0x10469380) == 0) {
            gabi::store<u32>(0x10469380, 1);
            l_offset->y = 60.0f;
            l_offset->x = 0.0f;
            l_offset->z = 0.0f;
        }
        PSMTXCopy(&a->mMtx, mDoMtx_stack_c::get());
        mDoMtx_stack_scaleM(a->scale.x, a->scale.y, a->scale.z);
        mDoMtx_stack_transM(l_offset->x, l_offset->y, l_offset->z);
        PSMTXCopy(mDoMtx_stack_c::get(), &mMtxHata);
        mCurBuf ^= 1;
        calc_pos(a);
        calc_nrm();
    }
}
VERIFY(0x0232AEF8, &Packet_c::calc);

/* 0232B03C */
void Packet_c::init(Act_c* a) {
    WWHD_FUNC(0x0232B03C, void, this, a);
    /* static cXyz l_nrm_back(0, 0, -1): guard 0x10469384, object 0x10469368 */
    cXyz* l_nrm_back = gabi::at<cXyz>(0x10469368);
    if (gabi::load<u32>(0x10469384) == 0) {
        l_nrm_back->x = 0.0f;
        l_nrm_back->y = 0.0f;
        l_nrm_back->z = -1.0f;
        gabi::store<u32>(0x10469384, 1);
    }
    for (int k = 0; k < 2; k++) {
        for (int j = 0; j < ROWS; j++) {
            for (int i = 0; i < COLS; i++) {
                cXyz* src = gabi::at<cXyz>(0x101C83C0 + (j * COLS + i) * 0xC); /* l_pos */
                cXyz* dst = &mBuf[k].mPos[j][i];
                dst->x = (f32)src->x;
                dst->y = (f32)src->y;
                dst->z = (f32)src->z;
                mBuf[k].mNrm[j][i].copy(*cXyz_BaseZ);
                mBuf[k].mNrmBack[j][i].copy(*l_nrm_back);
            }
        }
    }
    mCurBuf = 0;
    for (int j = 0; j < ROWS; j++)
        for (int i = 0; i < COLS; i++) mSpd[j][i].copy(*cXyz_Zero);
    for (int n = 0; n < 12; n++) mAng[n] = (s16)gabi::ftoi(cM_rndFX(32768.0f));
    calc(a);
}
VERIFY(0x0232B03C, &Packet_c::init);

/* 0232BCE0 Packet_c::hasi_nrm_init (static): the pole's normals */
static void hasi_nrm_init() {
    WWHD_FUNC(0x0232BCE0, void, (u32)0);
    /* the guard is a byte in .data (0x101C81E8); l_hasi_nrm[11] at 0x10469388 */
    if (gabi::load<u8>(0x101C81E8) == 0) {
        gabi::store<u8>(0x101C81E8, 1);
        gabi::Local<cXyz> tmp;
        for (int n = 0; n < 10; n++) {
            cXyz* src = gabi::at<cXyz>(0x101C8274 + n * 0xC); /* l_hasi_pos */
            cXyz* dst = gabi::at<cXyz>(0x10469388 + n * 0xC);
            f32 x = src->x;
            f32 z = src->z;
            dst->x = x;
            dst->y = 0.0f;
            dst->z = z;
            cXyz_normalize(dst, tmp);
        }
        cXyz* last = gabi::at<cXyz>(0x10469388 + 10 * 0xC);
        last->x = 0.0f;
        last->y = 1.0f;
        last->z = 0.0f;
    }
}
VERIFY(0x0232BCE0, hasi_nrm_init);

}  // namespace daObjBuoyflag
