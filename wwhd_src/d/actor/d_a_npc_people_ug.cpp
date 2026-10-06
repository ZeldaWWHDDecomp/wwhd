/**
 * d_a_npc_people_ug.cpp (WWHD)
 * NPC - Windfall townspeople: the Kyoro/Letter/Look/Look2 modes and the Ug (Gillian's
 * children... the two kids by the windmill) walk/turn/look/sit modes.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_people.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_people.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025D5578 fopAcM_SearchByName(s16 procName, fopAc_ac_c** out) -> bool */
static inline bool fopAcM_SearchByName_p(s16 name, gptr<fopAc_ac_c>* out) { return gabi::call<bool>(0x025D5578, name, out); }
/* 025D9F38 fopAcM_searchFromName(const char* name, u32 paramMask, u32 param) -> fopAc_ac_c* */
static inline fopAc_ac_c* fopAcM_searchFromName(const char* name, u32 mask, u32 prm) {
    return gabi::call<fopAc_ac_c*>(0x025D9F38, name, mask, prm);
}
/* 0259D624 dNpc_calc_DisXZ_AngY(cXyz, cXyz, f32* dist, s16* angY): the cXyz arguments are
 * passed by value (pointers to copies made through the FPRs) */
static inline void dNpc_calc_DisXZ_AngY(const cXyz* a, const cXyz* b, be<f32>* dist, be<s16>* ang) {
    gabi::Local<cXyz> ca, cb;
    ca->x = a->x;
    ca->y = a->y;
    ca->z = a->z;
    cb->x = b->x;
    cb->y = b->y;
    cb->z = b->z;
    gabi::call(0x0259D624, ca.get(), cb.get(), dist, ang);
}
/* fopAcM_GetProfName: HD s16 at +0xE */
static inline s16 fopAcM_GetProfName(fopAc_ac_c* a) { return gabi::load<s16>(gabi::ea(a) + 0xE); }
/* cXyz::abs() = sqrtf(PSVECSquareMag) */
static inline f32 cXyz_abs(cXyz* v) { return std_sqrtf(PSVECSquareMag(v)); }

enum { fpcNm_PLAYER_e = 0xA8, fpcNm_NPC_PEOPLE_e = 0x175, fpcNm_OBJ_TORIPOST_e = 0x43 };
/* .data tables */
enum : u32 {
    l_npc_staff_id = 0x101C3E7C,      /* const char*[] */
    l_npc_anm_kyoro = 0x101C3124,
    l_npc_anm_letter = 0x101C312C,
    l_npc_anm_wait = 0x101C31B4,
    l_npc_anm_walk = 0x101C31BA,
    l_npc_anm_sit_ug = 0x101C39E8,
};
static inline const char* staff_id(int i) { return gabi::at<const char>(gabi::load<u32>(l_npc_staff_id + i * 4)); }
static inline sPeopleAnmDat* anm(u32 a) { return gabi::at<sPeopleAnmDat>(a); }
/* struct assignment from a temporary (GHS: integer word copies) */
static inline void copy_words(cXyz* dst, cXyz* src) { dst->copy(*src); }

/* 022C4DDC */
s32 daNpcPeople_c::executeKyoroInit() {
    WWHD_FUNC(0x022C4DDC, s32, this);
    gabi::Local<gptr<fopAc_ac_c>> mailbox;
    if (fopAcM_SearchByName_p(fpcNm_OBJ_TORIPOST_e, mailbox.get()) == true && mailbox->get() != nullptr) {
        dNpc_calc_DisXZ_AngY(&current.pos, &mailbox->get()->current.pos, nullptr, &m77A);
    }
    m79D = 0;
    m79E = 0;
    mEtcFlag = mEtcFlag & ~0x00000008;
    setAnmTbl(anm(l_npc_anm_kyoro), 1);
    return 6;
}
VERIFY(0x022C4DDC, &daNpcPeople_c::executeKyoroInit);

/* 022C4E98 */
void daNpcPeople_c::executeKyoro() {
    WWHD_FUNC(0x022C4E98, void, this);
    if (executeCommon() == false) {
        if (mAnmFlag & 1) {
            mEtcFlag = mEtcFlag | 8;
            if (m78A || m789) {
                m79D = 1;
                m79E = 1;
                executeSetMode(0);
            } else {
                executeSetMode(3);
            }
        }
    } else {
        m79D = 1;
        m79E = 1;
    }
}
VERIFY(0x022C4E98, &daNpcPeople_c::executeKyoro);

/* 022C4F54 */
s32 daNpcPeople_c::executeLetterInit() {
    WWHD_FUNC(0x022C4F54, s32, this);
    m79D = 0;
    m79E = 0;
    setAnmTbl(anm(l_npc_anm_letter), 1);
    mEtcFlag = mEtcFlag & ~0x00400000;
    return 7;
}
VERIFY(0x022C4F54, &daNpcPeople_c::executeLetterInit);

/* 022C4FA8 */
void daNpcPeople_c::executeLetter() {
    WWHD_FUNC(0x022C4FA8, void, this);
    if (!executeCommon()) {
        int frame = gabi::ftoi(mpMorf->getFrame());
        /* HD: frames 0x23..0x50 (GameCube 0x23..0x45) */
        if ((u32)(frame - 0x23) >= 0x2E) {
            mEtcFlag = mEtcFlag & ~0x1;
        } else {
            mEtcFlag = mEtcFlag | 1;
            m7A6 = 1;
        }
        if (mAnmFlag & 1) {
            mEtcFlag = mEtcFlag | 0x400002;
            mEtcFlag = mEtcFlag & ~0x8;
            m79D = 1;
            m79E = 1;
            mPathRun.mbDir = mPathRun.mbDir ^ 1; /* turnDir() */
            executeSetMode(6);
        }
    } else {
        mEtcFlag = mEtcFlag | 1;
        m79D = 1;
        m79E = 1;
    }
}
VERIFY(0x022C4FA8, &daNpcPeople_c::executeLetter);

/* 022C509C */
s32 daNpcPeople_c::executeLookInit() {
    WWHD_FUNC(0x022C509C, s32, this);
    m76E = 0x5A; /* HD: 90 frames (GameCube 0x3C) */
    m748 = 0.0f;
    m79D = 0;
    m79E = 0;
    setAnmTbl(anm(l_npc_anm_wait), 1);
    daNpcPeople_c* actor = (daNpcPeople_c*)fopAcM_searchFromName(staff_id(8), 0, 0);
    if (actor != nullptr) {
        actor->setEtcFlag(0x10);
    }
    return 8;
}
VERIFY(0x022C509C, &daNpcPeople_c::executeLookInit);

/* 022C511C */
void daNpcPeople_c::executeLook() {
    WWHD_FUNC(0x022C511C, void, this);
    if (executeCommon() == false) {
        daNpcPeople_c* pActor = (daNpcPeople_c*)fopAcM_searchFromName(staff_id(8), 0, 0);
        if (pActor) {
            mLookAtPos.copy(pActor->eyePos);
            m799 = 1;
            m764 = true;
            pActor->setEtcFlag(0x10);
        }
        m76E = m76E - 1;
        if (m76E == 0) {
            m748 = mpNpcDat->field_0x28;
            m79D = 1;
            m79E = 1;
            executeSetMode(3);
        }
        m7A6 = 4;
    } else {
        m748 = mpNpcDat->field_0x28;
        m79D = 1;
        m79E = 1;
    }
}
VERIFY(0x022C511C, &daNpcPeople_c::executeLook);

/* 022C5208 */
s32 daNpcPeople_c::executeLook2Init() {
    WWHD_FUNC(0x022C5208, s32, this);
    m748 = 0.0f;
    m79D = 0;
    m79E = 0;
    setAnmTbl(anm(l_npc_anm_wait), 1);
    mEtcFlag = mEtcFlag | 0x20;
    return 9;
}
VERIFY(0x022C5208, &daNpcPeople_c::executeLook2Init);

/* 022C5268 */
void daNpcPeople_c::executeLook2() {
    WWHD_FUNC(0x022C5268, void, this);
    if (!executeCommon()) {
        if (mEtcFlag & 0x10) {
            fopAc_ac_c* pActor = fopAcM_searchFromName(staff_id(10), 0, 0);
            if (pActor) {
                mLookAtPos.copy(pActor->eyePos);
                m799 = 1;
                m764 = true;
            }
        } else {
            m748 = mpNpcDat->field_0x28;
            m79D = 1;
            m79E = 1;
            mEtcFlag = mEtcFlag & ~0x20;
            executeSetMode(0);
        }
        m7A6 = 3;
    } else {
        m748 = mpNpcDat->field_0x28;
        m79D = 1;
        m79E = 1;
        mEtcFlag = mEtcFlag & ~0x20;
    }
    mEtcFlag = mEtcFlag & ~0x10;
}
VERIFY(0x022C5268, &daNpcPeople_c::executeLook2);

/* 022C5368 */
s32 daNpcPeople_c::executeUgWalkInit() {
    WWHD_FUNC(0x022C5368, s32, this);
    setAnmTbl(anm(l_npc_anm_walk), 1);
    return 0xA;
}
VERIFY(0x022C5368, &daNpcPeople_c::executeUgWalkInit);

/* 022C5398 (HD: the cXyz result goes through a hidden pointer after `this`; the inlined cXyz
 * constructor allocates when it is NULL) */
void daNpcPeople_c::getDirDistToPos(cXyz* result, s16 angle, f32 mag) {
    WWHD_FUNC(0x022C5398, void, this, result, angle, mag);
    f32 x = cM_ssin(angle) * mag;
    f32 z = cM_scos(angle) * mag;
    if (result == nullptr) {
        result = (cXyz*)operator_new(0xC);
        if (result == nullptr) {
            return;
        }
    }
    result->z = z;
    result->y = 0.0f;
    result->x = x;
}
VERIFY(0x022C5398, &daNpcPeople_c::getDirDistToPos);

/* m71C = getDirDistToPos(angle, mag) + base */
static void people_setDirPos(daNpcPeople_c* i_this, s16 angle, f32 mag, cXyz* base, bool fpr = false) {
    gabi::Local<cXyz> dir, sum;
    i_this->getDirDistToPos(dir.get(), angle, mag);
    cXyz_pl(dir.get(), sum.get(), base);
    if (fpr) { /* copied through the FPRs */
        i_this->m71C.x = sum->x;
        i_this->m71C.y = sum->y;
        i_this->m71C.z = sum->z;
    } else {
        copy_words(&i_this->m71C, sum.get());
    }
}

/* 022C5434 */
void daNpcPeople_c::executeUgWalk() {
    WWHD_FUNC(0x022C5434, void, this);
    if (!executeCommon()) {
        gabi::Local<cXyz> diff, tmp;
        cXyz_mi(&current.pos, diff.get(), &m71C);
        diff->y = 0.0f;
        f32 temp = cXyz_abs(diff.get());
        cXyz_mi(&current.pos, tmp.get(), &home.pos);
        gabi::store<u32>(gabi::ea(&diff->x), gabi::load<u32>(gabi::ea(&tmp->x)));
        gabi::store<u32>(gabi::ea(&diff->z), gabi::load<u32>(gabi::ea(&tmp->z)));
        diff->y = 0.0f;
        f32 temp2 = cXyz_abs(diff.get());

        if (temp < 30.0f) {
            if (mEtcFlag & 0x10000) {
                mEtcFlag = mEtcFlag & ~0x10000;
                f32 mag = cM_rndF(180.0f); /* HD: before getRand */
                s16 angle = getRand(0x10000);
                people_setDirPos(this, angle, mag, &home.pos);
            } else {
                if ((mNpcNo != NPC_UG2 && (getRand(100) < 0x19 && !(mEtcFlag & 0x8000))) || (mEtcFlag & 0x2000)) {
                    mEtcFlag = mEtcFlag & ~0x2000;
                    executeSetMode(0xC);
                } else if (getRand(100) < 0x19) {
                    executeSetMode(0xE);
                } else {
                    mEtcFlag = mEtcFlag & ~0x8000;
                    executeSetMode(0);
                }
            }
            return;
        }
        if (temp2 < 180.0f) {
            if (mEtcFlag & 0x10000) {
                mEtcFlag = mEtcFlag & ~0x10000;
                f32 mag = cM_rndF(180.0f);
                s16 angle = getRand(0x10000);
                people_setDirPos(this, angle, mag, &home.pos);
            }
            if (mCyl.ChkCoHit()) {
                daNpcPeople_c* otherNpc = (daNpcPeople_c*)mCyl.GetCoHitAc();
                if (otherNpc && otherNpc && fopAcM_GetProfName(otherNpc) == fpcNm_NPC_PEOPLE_e) {
                    u8 otherNpcNo = otherNpc->getNpcNo();
                    if ((mNpcNo == NPC_UG1 && otherNpcNo == NPC_UG2) || (mNpcNo == NPC_UG2 && otherNpcNo == NPC_UG1)) {
                        otherNpc->setEtcFlag(0x60000);
                        mEtcFlag = mEtcFlag | 0x42000;
                        executeSetMode(0xB);
                    }
                }
            }
        } else {
            if (mCyl.ChkCoHit()) {
                fopAc_ac_c* pActor = mCyl.GetCoHitAc();
                if (pActor && pActor && fopAcM_GetProfName(pActor) == fpcNm_PLAYER_e) {
                    cXyz_mi(&pActor->current.pos, tmp.get(), &home.pos);
                    gabi::store<u32>(gabi::ea(&diff->x), gabi::load<u32>(gabi::ea(&tmp->x)));
                    diff->y = 0.0f;
                    gabi::store<u32>(gabi::ea(&diff->z), gabi::load<u32>(gabi::ea(&tmp->z)));
                    if (temp2 > cXyz_abs(diff.get())) {
                        gabi::Local<cXyz> d2;
                        cXyz_mi(&home.pos, d2.get(), &current.pos);
                        u16 angle1 = cM_atan2s(d2->x, d2->z);
                        cXyz_mi(&pActor->current.pos, d2.get(), &current.pos);
                        u16 angle2 = cM_atan2s(d2->x, d2->z);
                        if ((int)angle1 - (int)angle2 > (int)angle2 - (int)angle1) {
                            angle1 += 0x4000;
                        } else {
                            angle1 -= 0x4000;
                        }
                        gabi::Local<cXyz> dir;
                        getDirDistToPos(dir.get(), (s16)angle1, 50.0f);
                        cXyz_pl(dir.get(), d2.get(), &current.pos);
                        copy_words(&m71C, d2.get());
                        mEtcFlag = mEtcFlag | 0x10000;
                    }
                }
            }
        }

        f32 f3 = 20.0f;
        f32 temp4 = temp2 - 180.0f;
        if (temp4 < 0.0f) {
            temp4 = 0.0f;
        } else if (temp4 > f3) {
            temp4 = f3;
        }
        f32 f48 = mpNpcDat->field_0x48;
        m740 = (10.0f - f48) * temp4 / f3 + f48;
        gabi::Local<be<s16>> angle;
        dNpc_calc_DisXZ_AngY(&current.pos, &m71C, nullptr, angle.get());
        s16 a = *angle;
        m764 = false;
        m77A = a;
        m786 = a;
        f32 num = (f32)(s32)mpNpcDat->field_0x3A * m740;
        m_jnt.mbTrn = 1; /* setTrn() */
        m799 = 2;
        m782 = (s16)gabi::ftoi(num / mpNpcDat->field_0x48);
    }
}
VERIFY(0x022C5434, &daNpcPeople_c::executeUgWalk);

/* 022C5AC4 */
s32 daNpcPeople_c::executeUgTurnInit() {
    WWHD_FUNC(0x022C5AC4, s32, this);
    gabi::Local<be<s16>> angle;
    if (mEtcFlag & 0x40000) {
        mEtcFlag = mEtcFlag & ~0x40000;
        fopAc_ac_c* pActor;
        if (mNpcNo == NPC_UG1) {
            pActor = fopAcM_searchFromName(staff_id(18), 0, 0);
        } else {
            pActor = fopAcM_searchFromName(staff_id(17), 0, 0);
        }
        gabi::Local<cXyz> delta;
        cXyz_mi(&current.pos, delta.get(), &pActor->current.pos);
        s16 a = cM_atan2s(delta->x, delta->z);
        *angle = a;
        people_setDirPos(this, a, 100.0f, &current.pos, true);
    } else {
        s16 a = getRand(0x10000);
        *angle = a;
        f32 mag = cM_rndF(180.0f);
        people_setDirPos(this, *angle, mag, &home.pos, true);
    }
    dNpc_calc_DisXZ_AngY(&current.pos, &m71C, nullptr, angle.get());
    if (*angle == current.angle.y) {
        setAnmTbl(anm(l_npc_anm_walk), 1);
        return 0xA;
    } else {
        return 0xB;
    }
}
VERIFY(0x022C5AC4, &daNpcPeople_c::executeUgTurnInit);

/* 022C5D4C */
void daNpcPeople_c::executeUgTurn() {
    WWHD_FUNC(0x022C5D4C, void, this);
    if (!executeCommon()) {
        gabi::Local<be<s16>> temp;
        dNpc_calc_DisXZ_AngY(&current.pos, &m71C, nullptr, temp.get());
        s16 t = *temp;
        m799 = 2;
        m764 = false;
        m_jnt.mbTrn = 1; /* setTrn() */
        m786 = t;
        if (current.angle.y == t) {
            executeSetMode(0xA);
        }
    }
}
VERIFY(0x022C5D4C, &daNpcPeople_c::executeUgTurn);

/* 022C5DFC */
s32 daNpcPeople_c::executeUgLookInit() {
    WWHD_FUNC(0x022C5DFC, s32, this);
    m76E = 0x78;
    setAnmTbl(anm(l_npc_anm_wait), 1);
    daNpcPeople_c* actor = (daNpcPeople_c*)fopAcM_searchFromName(staff_id(18), 0, 0);
    if (actor != nullptr) {
        actor->setEtcFlag(0x4000);
    }
    return 0xC;
}
VERIFY(0x022C5DFC, &daNpcPeople_c::executeUgLookInit);

/* 022C5E5C */
void daNpcPeople_c::executeUgLook() {
    WWHD_FUNC(0x022C5E5C, void, this);
    if (!executeCommon()) {
        fopAc_ac_c* pActor = fopAcM_searchFromName(staff_id(18), 0, 0);
        if (pActor) {
            mLookAtPos.copy(pActor->eyePos);
            m799 = 1;
            m764 = false;
        }
        m76E = m76E - 1;
        if (m76E == 0) {
            executeSetMode(0xB);
        }
    }
}
VERIFY(0x022C5E5C, &daNpcPeople_c::executeUgLook);

/* 022C5EF4 */
s32 daNpcPeople_c::executeUgLook2Init() {
    WWHD_FUNC(0x022C5EF4, s32, this);
    m76E = 0x78;
    setAnmTbl(anm(l_npc_anm_wait), 1);
    mEtcFlag = mEtcFlag & ~0x00004000;
    return 0xD;
}
VERIFY(0x022C5EF4, &daNpcPeople_c::executeUgLook2Init);

/* 022C5F44 */
void daNpcPeople_c::executeUgLook2() {
    WWHD_FUNC(0x022C5F44, void, this);
    if (!executeCommon()) {
        fopAc_ac_c* pActor = fopAcM_searchFromName(staff_id(17), 0, 0);
        if (pActor) {
            mLookAtPos.copy(pActor->eyePos);
            m799 = 1;
            m764 = false;
        }
        m76E = m76E - 1;
        if (m76E == 0) {
            executeSetMode(0xB);
        }
    }
    mEtcFlag = mEtcFlag & ~0x10;
}
VERIFY(0x022C5F44, &daNpcPeople_c::executeUgLook2);

/* 022C5FE8 */
s32 daNpcPeople_c::executeUgSitInit() {
    WWHD_FUNC(0x022C5FE8, s32, this);
    setAnmTbl(anm(l_npc_anm_sit_ug), 1);
    m79D = 0;
    m79E = 0;
    mEtcFlag = mEtcFlag | 0x00080000;
    return 0xE;
}
VERIFY(0x022C5FE8, &daNpcPeople_c::executeUgSitInit);

/* 022C603C */
void daNpcPeople_c::executeUgSit() {
    WWHD_FUNC(0x022C603C, void, this);
    if (!executeCommon()) {
        if (mAnmFlag & 1) {
            m79D = 1;
            m79E = 1;
            mEtcFlag = mEtcFlag & ~0x00080000;
            setAnmTbl(anm(l_npc_anm_wait), 1);
            executeSetMode(0xB);
        }
    } else {
        m79D = 1;
        m79E = 1;
    }
}
VERIFY(0x022C603C, &daNpcPeople_c::executeUgSit);
