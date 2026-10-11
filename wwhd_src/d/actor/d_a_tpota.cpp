/**
 * d_a_tpota.cpp (WWHD)
 * Waterfall-basin splash effect (two emitters, ripples where particles reach the water).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tpota.cpp) to the WWHD layout and verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_tpota.h"
const gc_tu_data gc_tu = { .headerStatics={0x1046EA2C, 0x101D2A84, 0x1046EA38} };
static const u16 l_daTpota_idx_table[2] = {0x82AE, 0x82AF};

/* 024C9D44 */
cPhs_State daTpota_c::_create() {
    fopAcM_ct(this, daTpota_c);
    JPABaseEmitter* emitter;
    for (int i = 0; i < 2; i++) {
        mPositions[i].copy(current.pos);
        mAngles[i].x = current.angle.x;
        mAngles[i].y = current.angle.y;
        mAngles[i].z = current.angle.z;
        u16 id = l_daTpota_idx_table[i];
        emitter = dComIfGp_particle_set(id, &mPositions[i], &mAngles[i], nullptr, 0xff, nullptr, -1, nullptr, nullptr, nullptr);
        mpEmitters[i] = (TpotaEmitterView*)emitter;
    }
    return cPhs_COMPLEATE_e;
}

/* 024C9E44 */
bool daTpota_c::_delete() {
    for (int i = 0; i < 2; i++) {
        if (mpEmitters[i] != nullptr) {
            mpEmitters[i]->becomeInvalidEmitter();
            mpEmitters[i] = nullptr;
        }
    }
    return true;
}

/* 024C9EDC */
// HD: make_ripple(cXyz) (by value: a pointer to the caller's copy)
void daTpota_c::make_ripple(cXyz* i_position) {
    dComIfGp_particle_set(ID_IT_SN_TAKIURA_HAMON00, i_position, nullptr, nullptr, 0xff, nullptr, -1, nullptr, nullptr, nullptr);
}

/* 024C9E8C */
BOOL daTpota_c::check_water_h(TpotaParticleView* i_ptcl, f32 i_position_y) {
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

/* 024C9F44 */
void daTpota_c::clear_splash() {
    unknown_struct* unknown_struct = field_0x2C4;
    for (int i = 0; i < 30; i++) {
        unknown_struct->ptcl = nullptr;
        unknown_struct->pos_y = 0.0f;
        unknown_struct++;
    }
}

/* 024C9F6C */
// HD: the list walk stops at a NULL link only (getEnd() is NULL)
void daTpota_c::renew_splash() {
    if (mpEmitters[1] != nullptr) {
        TpotaParticleList* list = mpEmitters[1]->getParticleList();
        unknown_struct* unknown_struct = field_0x2C4;
        if (list != 0) {
            clear_splash();
            for (TpotaParticleLink* link = list->getFirst(); link != nullptr; link = link->getNext()) {
                TpotaParticleView* particle = link->getObject();
                f32 pos_y = particle->mGlobalPosition.y;
                unknown_struct->ptcl = particle;
                unknown_struct->pos_y = pos_y;
                unknown_struct++;
            }
        }
    }
}

/* 024C9FD0 */
bool daTpota_c::_execute() {
    if (mpEmitters[1] != nullptr) {
        TpotaParticleList* list = mpEmitters[1]->getParticleList();
        if (list != 0) {
            for (TpotaParticleLink* link = list->getFirst(); link != nullptr; link = link->getNext()) {
                TpotaParticleView* particle = link->getObject();
                f32 py = particle->mGlobalPosition.y;
                f32 pz = particle->mGlobalPosition.z;
                f32 px = particle->mGlobalPosition.x;
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

/* method table entries (HD: tail branches; _draw inlined) */
/* 024CA090 */
static cPhs_State Mthd_Create(void* i_this) {
    return static_cast<daTpota_c*>(i_this)->_create();
}
/* 024CA094 */
static bool Mthd_Delete(void* i_this) {
    return static_cast<daTpota_c*>(i_this)->_delete();
}
/* 024CA098 */
static bool Mthd_Execute(void* i_this) {
    return static_cast<daTpota_c*>(i_this)->_execute();
}
/* 024CA09C */
// HD: Mthd_Draw (_draw returns TRUE)
static BOOL Mthd_Draw(void* i_this) {
    return TRUE;
}
/* 024CA18C */
static BOOL Mthd_IsDelete(void* i_this) {
    return TRUE;
}

/* 024CA0A4 */
static void __sinit_d_a_tpota_cpp() {
    __sinit_header_statics();
}

/* 024CA138 */
// HD: daTpota_c deleting destructor
static void daTpota_c_dt(daTpota_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py): translation-unit data
data l_daTpota_idx_table 10040A54
*/
