/**
 * d_a_tpota.cpp (WWHD)
 * Waterfall-basin splash effect (two emitters, ripples where particles reach the water).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tpota.cpp) to the WWHD layout and verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define ACT_VTBL 0x10040A58 /* HD: daTpota_c vtable */
/* const u16 l_daTpota_idx_table[2] = {ID_IT_SN_TAKIURA_POTAA00, ID_IT_SN_TAKIURA_POTAB00} */
static u16 l_daTpota_idx_table(int i) { return gabi::load<u16>(0x10040A54 + 2 * i); }

enum { ID_IT_SN_TAKIURA_HAMON00 = 0x82B0 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* JPABaseEmitter (HD): particle list (JSUList<JPABaseParticle>) at +0x1AC; first link at list+0,
 * JSULink: object at +0, next at +0xC. JPABaseParticle: global position at +0x28 (HD: the
 * getGlobalPosition inline reads it directly) */
static inline u32 JPABaseEmitter_getParticleList(u32 e) { return e + 0x1AC; }
static inline u32 JSUList_getFirst(u32 list) { return gabi::load<u32>(list); }
static inline u32 JSULink_getObject(u32 link) { return gabi::load<u32>(link); }
static inline u32 JSULink_getNext(u32 link) { return gabi::load<u32>(link + 0xC); }
/* JPABaseEmitter::becomeInvalidEmitter (HD inline, as d_a_item) */
static inline void JPABaseEmitter_becomeInvalidEmitter(u32 e) {
    u32 f = gabi::load<u32>(e + 0x254);
    gabi::store<s32>(e + 0x5C, -1);
    gabi::store<u32>(e + 0x254, f | 1);
}

struct daTpota_c : fopAc_ac_c {
    struct unknown_struct {
        /* 0x0 */ be<u32> ptcl; /* JPABaseParticle* */
        /* 0x4 */ be<f32> pos_y;
    };

    cPhs_State _create();
    bool _delete();
    void make_ripple(cXyz* i_position);
    BOOL check_water_h(u32 i_ptcl, f32 i_position_y);
    void clear_splash();
    void renew_splash();
    bool _execute();

    /* 0x3AC */ request_of_phase_process_class mPhase; /* unused */
    /* 0x3B4 */ gptr<JPABaseEmitter> mpEmitters[2];
    /* 0x3BC */ cXyz mPositions[2];
    /* 0x3D4 */ csXyz mAngles[2];
    /* 0x3E0 */ unknown_struct field_0x2C4[30];
    /* 0x4D0 */ u8 field_0x3B4[0x500 - 0x4D0];
};
WWHD_OFFSET(daTpota_c, mpEmitters, 0x3B4);
WWHD_OFFSET(daTpota_c, mAngles, 0x3D4);
WWHD_OFFSET(daTpota_c, field_0x2C4, 0x3E0);

/* 024C9D44 */
cPhs_State daTpota_c::_create() {
    WWHD_FUNC(0x024C9D44, cPhs_State, this);
    /* fopAcM_ct(this, daTpota_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = ACT_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    JPABaseEmitter* emitter;
    for (int i = 0; i < 2; i++) {
        mPositions[i].copy(current.pos);
        mAngles[i].x = current.angle.x;
        mAngles[i].y = current.angle.y;
        mAngles[i].z = current.angle.z;
        u16 id = l_daTpota_idx_table(i);
        emitter = dComIfGp_particle_set(id, &mPositions[i], &mAngles[i], nullptr, 0xff, nullptr, -1, nullptr, nullptr, nullptr);
        mpEmitters[i] = emitter;
    }
    return cPhs_COMPLEATE_e;
}
VERIFY(0x024C9D44, &daTpota_c::_create);

/* 024C9E44 */
bool daTpota_c::_delete() {
    WWHD_FUNC(0x024C9E44, bool, this);
    for (int i = 0; i < 2; i++) {
        if (mpEmitters[i] != nullptr) {
            JPABaseEmitter_becomeInvalidEmitter(gabi::ea(mpEmitters[i].get()));
            mpEmitters[i] = nullptr;
        }
    }
    return true;
}
VERIFY(0x024C9E44, &daTpota_c::_delete);

/* 024C9EDC: make_ripple(cXyz) (by value: a pointer to the caller's copy) */
void daTpota_c::make_ripple(cXyz* i_position) {
    WWHD_FUNC(0x024C9EDC, void, this, i_position);
    dComIfGp_particle_set(ID_IT_SN_TAKIURA_HAMON00, i_position, nullptr, nullptr, 0xff, nullptr, -1, nullptr, nullptr, nullptr);
}
VERIFY(0x024C9EDC, &daTpota_c::make_ripple);

/* 024C9E8C */
BOOL daTpota_c::check_water_h(u32 i_ptcl, f32 i_position_y) {
    WWHD_FUNC(0x024C9E8C, BOOL, this, i_ptcl, i_position_y);
    unknown_struct* unknown_struct = field_0x2C4;
    int ret = FALSE;
    if (!(i_position_y > -230.0f)) {
        for (int i = 0; i < 30; i++, unknown_struct++) {
            if (unknown_struct->ptcl == i_ptcl) {
                if (unknown_struct->pos_y > -230.0f) {
                    ret = TRUE;
                }
                break;
            }
        }
    }
    return ret;
}
VERIFY(0x024C9E8C, &daTpota_c::check_water_h);

/* 024C9F44 */
void daTpota_c::clear_splash() {
    WWHD_FUNC(0x024C9F44, void, this);
    unknown_struct* unknown_struct = field_0x2C4;
    for (int i = 0; i < 30; i++) {
        unknown_struct->ptcl = 0;
        unknown_struct->pos_y = 0.0f;
        unknown_struct++;
    }
}
VERIFY(0x024C9F44, &daTpota_c::clear_splash);

/* 024C9F6C. HD: the list walk stops at a NULL link only (getEnd() is NULL) */
void daTpota_c::renew_splash() {
    WWHD_FUNC(0x024C9F6C, void, this);
    if (mpEmitters[1] != nullptr) {
        u32 list = JPABaseEmitter_getParticleList(gabi::ea(mpEmitters[1].get()));
        unknown_struct* unknown_struct = field_0x2C4;
        if (list != 0) {
            clear_splash();
            for (u32 link = JSUList_getFirst(list); link != 0; link = JSULink_getNext(link)) {
                u32 particle = JSULink_getObject(link);
                f32 pos_y = gabi::load<f32>(particle + 0x2C);
                unknown_struct->ptcl = particle;
                unknown_struct->pos_y = pos_y;
                unknown_struct++;
            }
        }
    }
}
VERIFY(0x024C9F6C, &daTpota_c::renew_splash);

/* 024C9FD0 */
bool daTpota_c::_execute() {
    WWHD_FUNC(0x024C9FD0, bool, this);
    if (mpEmitters[1] != nullptr) {
        u32 list = JPABaseEmitter_getParticleList(gabi::ea(mpEmitters[1].get()));
        if (list != 0) {
            for (u32 link = JSUList_getFirst(list); link != 0; link = JSULink_getNext(link)) {
                u32 particle = JSULink_getObject(link);
                f32 py = gabi::load<f32>(particle + 0x2C);
                f32 pz = gabi::load<f32>(particle + 0x30);
                f32 px = gabi::load<f32>(particle + 0x28);
                if (check_water_h(particle, py)) {
                    gabi::Local<cXyz> local_48;
                    local_48->set(px, -230.0f, pz);
                    make_ripple(local_48.get());
                }
            }
        }
        renew_splash();
    }
    return true;
}
VERIFY(0x024C9FD0, &daTpota_c::_execute);

/* method table entries (HD: tail branches; _draw inlined) */
/* 024CA090 */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x024CA090, cPhs_State, i_this);
    return static_cast<daTpota_c*>(i_this)->_create();
}
VERIFY(0x024CA090, Mthd_Create);
/* 024CA094 */
static bool Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x024CA094, bool, i_this);
    return static_cast<daTpota_c*>(i_this)->_delete();
}
VERIFY(0x024CA094, Mthd_Delete);
/* 024CA098 */
static bool Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x024CA098, bool, i_this);
    return static_cast<daTpota_c*>(i_this)->_execute();
}
VERIFY(0x024CA098, Mthd_Execute);
/* 024CA09C: Mthd_Draw (_draw returns TRUE) */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x024CA09C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024CA09C, Mthd_Draw);
/* 024CA18C */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x024CA18C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024CA18C, Mthd_IsDelete);

/* 024CA0A4 */
static void __sinit_d_a_tpota_cpp() {
    WWHD_FUNC(0x024CA0A4, void, (u32)0);
    sinit_header_statics(0x1046EA2C, 0x101D2A84);
}
VERIFY(0x024CA0A4, __sinit_d_a_tpota_cpp);

/* 024CA138: daTpota_c deleting destructor */
static void daTpota_c_dt(daTpota_c* i_this, s32 flags) {
    WWHD_FUNC(0x024CA138, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024CA138, daTpota_c_dt);
