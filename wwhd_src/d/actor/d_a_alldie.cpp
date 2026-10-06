/**
 * d_a_alldie.cpp (WWHD)
 * Sets a switch once every enemy of the room is dead.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_alldie.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

enum {
    ACT_WAIT,
    ACT_CHECK,
    ACT_TIMER,
};

#define ALLDIE_VTBL 0x10006DC4 /* daAlldie_c vtable (HD virtual destructor) */

struct daAlldie_c : fopAc_ac_c {
    void setActio(u8 action) { mAction = action; }

    u8 getSwbit();
    BOOL actionWait() { return TRUE; } /* HD: inlined */
    BOOL actionCheck();
    BOOL actionTimer();
    BOOL execute();

    /* 0x3AC */ be<u8> mAction; /* GameCube 0x290 */
    /* 0x3AD */ u8 _3AD;
    /* 0x3AE */ be<s16> mTimer;
};
WWHD_OFFSET(daAlldie_c, mAction, 0x3AC);
WWHD_OFFSET(daAlldie_c, mTimer, 0x3AE);

/* 025D98E8 fopAcM_myRoomSearchEnemy(roomNo) -> enemy or NULL */
/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u32 fopAcM_myRoomSearchEnemy(s8 roomNo) { return gabi::call<u32>(0x025D98E8, roomNo); }

/* 02048460 */
u8 daAlldie_c::getSwbit() {
    WWHD_FUNC(0x02048460, u8, this);
    return fopAcM_GetParam(this) >> 0x8;
}
VERIFY(0x02048460, &daAlldie_c::getSwbit);

/* 02048410 */
BOOL daAlldie_c::actionCheck() {
    WWHD_FUNC(0x02048410, BOOL, this);
    if (!fopAcM_myRoomSearchEnemy(fopAcM_GetRoomNo(this))) {
        setActio(ACT_TIMER);
        mTimer = 65;
    }
    return TRUE;
}
VERIFY(0x02048410, &daAlldie_c::actionCheck);

/* 0204846C */
BOOL daAlldie_c::actionTimer() {
    WWHD_FUNC(0x0204846C, BOOL, this);
    if (fopAcM_myRoomSearchEnemy(fopAcM_GetRoomNo(this))) {
        setActio(ACT_CHECK);
    } else if (mTimer > 0) {
        mTimer = mTimer - 1;
    } else {
        setActio(ACT_WAIT);
        dComIfGs_onSwitch(getSwbit(), fopAcM_GetRoomNo(this));
    }
    return TRUE;
}
VERIFY(0x0204846C, &daAlldie_c::actionTimer);

/* 02048500 */
BOOL daAlldie_c::execute() {
    WWHD_FUNC(0x02048500, BOOL, this);
    switch (mAction) {
    case ACT_CHECK:
        actionCheck();
        break;
    case ACT_TIMER:
        actionTimer();
        break;
    default:
        actionWait();
        break;
    }
    return TRUE;
}
VERIFY(0x02048500, &daAlldie_c::execute);

/* 020486B8 */
static BOOL daAlldie_Draw(daAlldie_c*) {
    WWHD_FUNC(0x020486B8, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x020486B8, daAlldie_Draw);

/* 0204854C */
static BOOL daAlldie_Execute(daAlldie_c* i_this) {
    WWHD_FUNC(0x0204854C, BOOL, i_this);
    i_this->execute();
    return TRUE;
}
VERIFY(0x0204854C, daAlldie_Execute);

/* 02048570 */
static BOOL daAlldie_IsDelete(daAlldie_c*) {
    WWHD_FUNC(0x02048570, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02048570, daAlldie_IsDelete);

/* 02048578: i_this->~daAlldie_c() is trivial */
static BOOL daAlldie_Delete(daAlldie_c*) {
    WWHD_FUNC(0x02048578, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02048578, daAlldie_Delete);

/* 02048580: daAlldie_c::create() inlined */
static cPhs_State daAlldie_Create(fopAc_ac_c* ac) {
    WWHD_FUNC(0x02048580, cPhs_State, ac);
    daAlldie_c* i_this = (daAlldie_c*)ac;
    /* fopAcM_ct(this, daAlldie_c): HD: fopAc_ac_c has a vtable */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = ALLDIE_VTBL;
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    if (!dComIfGs_isSwitch(i_this->getSwbit(), fopAcM_GetRoomNo(i_this))) {
        i_this->setActio(ACT_CHECK);
    } else {
        i_this->setActio(ACT_WAIT);
    }

    i_this->shape_angle.z = 0;
    i_this->shape_angle.x = 0;
    i_this->current.angle.z = 0;
    i_this->current.angle.x = 0;

    return cPhs_COMPLEATE_e;
}
VERIFY(0x02048580, daAlldie_Create);

/* 02048624: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_alldie_cpp() {
    WWHD_FUNC(0x02048624, void, (u32)0);
    sinit_header_statics(0x104613A4, 0x1018F840);
}
VERIFY(0x02048624, __sinit_d_a_alldie_cpp);

/* 020486C0: daAlldie_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daAlldie_c_dt(daAlldie_c* i_this, s32 flags) {
    WWHD_FUNC(0x020486C0, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x020486C0, daAlldie_c_dt);
