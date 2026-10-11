/**
 * d_a_npc_people_ug.cpp (WWHD)
 * NPC - Windfall townspeople: the Kyoro/Letter/Look/Look2 modes and the Ug (Gillian's
 * children... the two kids by the windmill) walk/turn/look/sit modes.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_people.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_people_ug.h"

// HD: these adapters retain the by-value vector copies and native call boundaries.
extern bool ugSearchByName(s16 name, gptr<fopAc_ac_c>* out);
extern fopAc_ac_c* ugSearchFromName(const char* name, u32 mask, u32 prm);
extern void ugCalcDisXZAngY(cXyz* a, cXyz* b, be<f32>* dist, be<s16>* ang);
static const char* l_npc_staff_id[] = {
    "Uo1",
    "Uo2",
    "Uo3",
    "Ub1",
    "Ub2",
    "Ub3",
    "Ub4",
    "Uw1",
    "Uw2",
    "Um1",
    "Um2",
    "Um3",
    "Sa1",
    "Sa2",
    "Sa3",
    "Sa4",
    "Sa5",
    "Ug1",
    "Ug2",
};
static sPeopleAnmDat l_npc_anm_kyoro[] = {{3, 8, 1}, {0xFF, 8, 0}};
static sPeopleAnmDat l_npc_anm_letter[] = {{4, 8, 1}, {0xFF, 8, 0}};
static sPeopleAnmDat l_npc_anm_wait[] = {{0, 8, 0}};
static sPeopleAnmDat l_npc_anm_walk[] = {{2, 8, 0}};
static sPeopleAnmDat l_npc_anm_sit_ug[] = {{4, 8, 1}, {5, 8, 2}, {4, 8, 0xFF}, {0xFF, 8, 0}};

enum { fpcNm_PLAYER_e = 0xA8, fpcNm_NPC_PEOPLE_e = 0x175, fpcNm_OBJ_TORIPOST_e = 0x43 };

static void calcDisXZAngY(const cXyz* a, const cXyz* b, be<f32>* dist, be<s16>* ang) {
    gabi::Local<cXyz> ca, cb;
    ca->x = a->x;
    ca->y = a->y;
    ca->z = a->z;
    cb->x = b->x;
    cb->y = b->y;
    cb->z = b->z;
    ugCalcDisXZAngY(ca.get(), cb.get(), dist, ang);
}

// HD: GHS copies the horizontal components as integer words, preserving signaling NaNs.
static void copyHorizontal(cXyz* dst, const cXyz* src) {
    auto* dstWords = reinterpret_cast<peopleUgHD::VectorWords*>(dst);
    auto* srcWords = reinterpret_cast<const peopleUgHD::VectorWords*>(src);
    dstWords->x = srcWords->x;
    dstWords->z = srcWords->z;
}
static f32 cXyz_abs(cXyz* v) {
    return std_sqrtf(PSVECSquareMag(v));
}

/* 022C4DDC */
s32 daNpcPeople_c::executeKyoroInit() {

    gabi::Local<gptr<fopAc_ac_c>> mailbox;
    if (ugSearchByName(fpcNm_OBJ_TORIPOST_e, mailbox.get()) == true && mailbox->get() != nullptr) {
        calcDisXZAngY(&current.pos, &mailbox->get()->current.pos, nullptr, &m77A);
    }
    m79D = 0;
    m79E = 0;
    mEtcFlag = mEtcFlag & ~0x00000008;
    setAnmTbl(l_npc_anm_kyoro, 1);
    return peopleUgHD::MODE_KYORO;
}

/* 022C4E98 */
void daNpcPeople_c::executeKyoro() {

    if (executeCommon() == false) {
        if (mAnmFlag & 1) {
            mEtcFlag = mEtcFlag | 8;
            if (m78A || m789) {
                m79D = 1;
                m79E = 1;
                executeSetMode(peopleUgHD::MODE_WAIT);
            } else {
                executeSetMode(peopleUgHD::MODE_TURN);
            }
        }
    } else {
        m79D = 1;
        m79E = 1;
    }
}

/* 022C4F54 */
s32 daNpcPeople_c::executeLetterInit() {

    m79D = 0;
    m79E = 0;
    setAnmTbl(l_npc_anm_letter, 1);
    mEtcFlag = mEtcFlag & ~0x00400000;
    return peopleUgHD::MODE_LETTER;
}

/* 022C4FA8 */
void daNpcPeople_c::executeLetter() {

    if (!executeCommon()) {
        int frame = gabi::ftoi(mpMorf->getFrame());
        // HD: frames 0x23..0x50 (GameCube 0x23..0x45)
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
            executeSetMode(peopleUgHD::MODE_KYORO);
        }
    } else {
        mEtcFlag = mEtcFlag | 1;
        m79D = 1;
        m79E = 1;
    }
}

/* 022C509C */
s32 daNpcPeople_c::executeLookInit() {

    m76E = 0x5A; // HD: 90 frames (GameCube 0x3C)
    m748 = 0.0f;
    m79D = 0;
    m79E = 0;
    setAnmTbl(l_npc_anm_wait, 1);
    daNpcPeople_c* actor = (daNpcPeople_c*)ugSearchFromName(l_npc_staff_id[NPC_UW2], 0, 0);
    if (actor != nullptr) {
        actor->setEtcFlag(0x10);
    }
    return peopleUgHD::MODE_LOOK;
}

/* 022C511C */
void daNpcPeople_c::executeLook() {

    if (executeCommon() == false) {
        daNpcPeople_c* pActor = (daNpcPeople_c*)ugSearchFromName(l_npc_staff_id[NPC_UW2], 0, 0);
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
            executeSetMode(peopleUgHD::MODE_TURN);
        }
        m7A6 = 4;
    } else {
        m748 = mpNpcDat->field_0x28;
        m79D = 1;
        m79E = 1;
    }
}

/* 022C5208 */
s32 daNpcPeople_c::executeLook2Init() {

    m748 = 0.0f;
    m79D = 0;
    m79E = 0;
    setAnmTbl(l_npc_anm_wait, 1);
    mEtcFlag = mEtcFlag | 0x20;
    return peopleUgHD::MODE_LOOK2;
}

/* 022C5268 */
void daNpcPeople_c::executeLook2() {

    if (!executeCommon()) {
        if (mEtcFlag & 0x10) {
            fopAc_ac_c* pActor = ugSearchFromName(l_npc_staff_id[NPC_UM2], 0, 0);
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
            executeSetMode(peopleUgHD::MODE_WAIT);
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

/* 022C5368 */
s32 daNpcPeople_c::executeUgWalkInit() {

    setAnmTbl(l_npc_anm_walk, 1);
    return peopleUgHD::MODE_UG_WALK;
}

// HD: hidden result pointer follows this; the inlined constructor allocates if null.
/* 022C5398 */
void daNpcPeople_c::getDirDistToPos(cXyz* result, s16 angle, f32 mag) {

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

/* m71C = getDirDistToPos(angle, mag) + base */
static void people_setDirPos(daNpcPeople_c* i_this, s16 angle, f32 mag, cXyz* base,
                             bool fpr = false) {
    gabi::Local<cXyz> dir, sum;
    i_this->getDirDistToPos(dir.get(), angle, mag);
    cXyz_pl(dir.get(), sum.get(), base);
    if (fpr) { /* copied through the FPRs */
        i_this->m71C.x = sum->x;
        i_this->m71C.y = sum->y;
        i_this->m71C.z = sum->z;
    } else {
        i_this->m71C.copy(*sum);
    }
}

/* 022C5434 */
void daNpcPeople_c::executeUgWalk() {

    if (!executeCommon()) {
        gabi::Local<cXyz> diff, tmp;
        cXyz_mi(&current.pos, diff.get(), &m71C);
        diff->y = 0.0f;
        f32 temp = cXyz_abs(diff.get());
        cXyz_mi(&current.pos, tmp.get(), &home.pos);
        copyHorizontal(diff.get(), tmp.get());
        diff->y = 0.0f;
        f32 temp2 = cXyz_abs(diff.get());

        if (temp < 30.0f) {
            if (mEtcFlag & 0x10000) {
                mEtcFlag = mEtcFlag & ~0x10000;
                f32 mag = cM_rndF(180.0f); // HD: before getRand
                s16 angle = getRand(0x10000);
                people_setDirPos(this, angle, mag, &home.pos);
            } else {
                if ((mNpcNo != NPC_UG2 && (getRand(100) < 0x19 && !(mEtcFlag & 0x8000))) ||
                    (mEtcFlag & 0x2000)) {
                    mEtcFlag = mEtcFlag & ~0x2000;
                    executeSetMode(peopleUgHD::MODE_UG_LOOK);
                } else if (getRand(100) < 0x19) {
                    executeSetMode(peopleUgHD::MODE_UG_SIT);
                } else {
                    mEtcFlag = mEtcFlag & ~0x8000;
                    executeSetMode(peopleUgHD::MODE_WAIT);
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
                if (otherNpc && otherNpc->mProfName == fpcNm_NPC_PEOPLE_e) {
                    u8 otherNpcNo = otherNpc->getNpcNo();
                    if ((mNpcNo == NPC_UG1 && otherNpcNo == NPC_UG2) ||
                        (mNpcNo == NPC_UG2 && otherNpcNo == NPC_UG1)) {
                        otherNpc->setEtcFlag(0x60000);
                        mEtcFlag = mEtcFlag | 0x42000;
                        executeSetMode(peopleUgHD::MODE_UG_TURN);
                    }
                }
            }
        } else {
            if (mCyl.ChkCoHit()) {
                fopAc_ac_c* pActor = mCyl.GetCoHitAc();
                if (pActor && pActor->mProfName == fpcNm_PLAYER_e) {
                    cXyz_mi(&pActor->current.pos, tmp.get(), &home.pos);
                    reinterpret_cast<peopleUgHD::VectorWords*>(diff.get())->x =
                        reinterpret_cast<peopleUgHD::VectorWords*>(tmp.get())->x;
                    diff->y = 0.0f;
                    reinterpret_cast<peopleUgHD::VectorWords*>(diff.get())->z =
                        reinterpret_cast<peopleUgHD::VectorWords*>(tmp.get())->z;
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
                        m71C.copy(*d2);
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
        calcDisXZAngY(&current.pos, &m71C, nullptr, angle.get());
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

/* 022C5AC4 */
s32 daNpcPeople_c::executeUgTurnInit() {

    gabi::Local<be<s16>> angle;
    if (mEtcFlag & 0x40000) {
        mEtcFlag = mEtcFlag & ~0x40000;
        fopAc_ac_c* pActor;
        if (mNpcNo == NPC_UG1) {
            pActor = ugSearchFromName(l_npc_staff_id[NPC_UG2], 0, 0);
        } else {
            pActor = ugSearchFromName(l_npc_staff_id[NPC_UG1], 0, 0);
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
    calcDisXZAngY(&current.pos, &m71C, nullptr, angle.get());
    if (*angle == current.angle.y) {
        setAnmTbl(l_npc_anm_walk, 1);
        return peopleUgHD::MODE_UG_WALK;
    } else {
        return peopleUgHD::MODE_UG_TURN;
    }
}

/* 022C5D4C */
void daNpcPeople_c::executeUgTurn() {

    if (!executeCommon()) {
        gabi::Local<be<s16>> temp;
        calcDisXZAngY(&current.pos, &m71C, nullptr, temp.get());
        s16 t = *temp;
        m799 = 2;
        m764 = false;
        m_jnt.mbTrn = 1; /* setTrn() */
        m786 = t;
        if (current.angle.y == t) {
            executeSetMode(peopleUgHD::MODE_UG_WALK);
        }
    }
}

/* 022C5DFC */
s32 daNpcPeople_c::executeUgLookInit() {

    m76E = 0x78;
    setAnmTbl(l_npc_anm_wait, 1);
    daNpcPeople_c* actor = (daNpcPeople_c*)ugSearchFromName(l_npc_staff_id[NPC_UG2], 0, 0);
    if (actor != nullptr) {
        actor->setEtcFlag(0x4000);
    }
    return peopleUgHD::MODE_UG_LOOK;
}

/* 022C5E5C */
void daNpcPeople_c::executeUgLook() {

    if (!executeCommon()) {
        fopAc_ac_c* pActor = ugSearchFromName(l_npc_staff_id[NPC_UG2], 0, 0);
        if (pActor) {
            mLookAtPos.copy(pActor->eyePos);
            m799 = 1;
            m764 = false;
        }
        m76E = m76E - 1;
        if (m76E == 0) {
            executeSetMode(peopleUgHD::MODE_UG_TURN);
        }
    }
}

/* 022C5EF4 */
s32 daNpcPeople_c::executeUgLook2Init() {

    m76E = 0x78;
    setAnmTbl(l_npc_anm_wait, 1);
    mEtcFlag = mEtcFlag & ~0x00004000;
    return peopleUgHD::MODE_UG_LOOK2;
}

/* 022C5F44 */
void daNpcPeople_c::executeUgLook2() {

    if (!executeCommon()) {
        fopAc_ac_c* pActor = ugSearchFromName(l_npc_staff_id[NPC_UG1], 0, 0);
        if (pActor) {
            mLookAtPos.copy(pActor->eyePos);
            m799 = 1;
            m764 = false;
        }
        m76E = m76E - 1;
        if (m76E == 0) {
            executeSetMode(peopleUgHD::MODE_UG_TURN);
        }
    }
    mEtcFlag = mEtcFlag & ~0x10;
}

/* 022C5FE8 */
s32 daNpcPeople_c::executeUgSitInit() {

    setAnmTbl(l_npc_anm_sit_ug, 1);
    m79D = 0;
    m79E = 0;
    mEtcFlag = mEtcFlag | 0x00080000;
    return peopleUgHD::MODE_UG_SIT;
}

/* 022C603C */
void daNpcPeople_c::executeUgSit() {

    if (!executeCommon()) {
        if (mAnmFlag & 1) {
            m79D = 1;
            m79E = 1;
            mEtcFlag = mEtcFlag & ~0x00080000;
            setAnmTbl(l_npc_anm_wait, 1);
            executeSetMode(peopleUgHD::MODE_UG_TURN);
        }
    } else {
        m79D = 1;
        m79E = 1;
    }
}
/* WWHD glue
 data l_npc_staff_id 101C3E7C
 data l_npc_anm_kyoro 101C3124
 data l_npc_anm_letter 101C312C
 data l_npc_anm_wait 101C31B4
 data l_npc_anm_walk 101C31BA
 data l_npc_anm_sit_ug 101C39E8
 ext 025D5578 bool ugSearchByName(s16, gptr<fopAc_ac_c>*)
 ext 025D9F38 fopAc_ac_c* ugSearchFromName(const char*, u32, u32)
 ext 0259D624 void ugCalcDisXZAngY(cXyz*, cXyz*, be<f32>*, be<s16>*)
*/
