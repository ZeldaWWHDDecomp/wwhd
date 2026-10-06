/* d_a_obj_buoyflag: member functions of the translation unit defined in another part file, as plain
 * guest calls (so the parts can call each other naturally). Weak: the decompiled definition in a
 * part replaces it. */
#include "d/actor/d_a_obj_buoyflag.h"

namespace daObjBuoyflag {
#define W __attribute__((weak))
W void Packet_c::draw_hata(Act_c* a) { gabi::call(0x02328FD4, this, a); }
W void Packet_c::draw_hasi(Act_c* a) { gabi::call(0x023296B0, this, a); }
W void Packet_c::draw(Act_c* a) { gabi::call(0x02329C4C, this, a); }
W void Packet_c::calc_wind_base(Act_c* a) { gabi::call(0x02329CE4, this, a); }
W void Packet_c::calc_pos_spring_near(const cXyz* p, const cXyz* q, f32 l, f32 k) { gabi::call(0x0232A13C, this, p, q, l, k); }
W void Packet_c::calc_pos(Act_c* a) { gabi::call(0x0232A218, this, a); }
W void Packet_c::calc_nrm() { gabi::call(0x0232A7F4, this); }
W void Packet_c::calc(Act_c* a) { gabi::call(0x0232AEF8, this, a); }
W void Packet_c::init(Act_c* a) { gabi::call(0x0232B03C, this, a); }
W void Packet_c::load_texture() { gabi::call(0x0232B3B8, this); }
W void Packet_c::update_hata() { gabi::call(0x0232B624, this); }
W void Packet_c::update_hasi() { gabi::call(0x0232B984, this); }
W void Packet_c::update(Act_c* a) { gabi::call(0x0232BBEC, this, a); }
W void Packet_c::gpu_init_hata() { gabi::call(0x0232BDC0, this); }
W void Packet_c::gpu_init_hasi() { gabi::call(0x0232C670, this); }
W void Act_c::mtx_init() { gabi::call(0x0232CD40, this); }
W BOOL Act_c::mode_afl() { return gabi::call<BOOL>(0x0232CDB4, this); }
W BOOL Act_c::mode_jumpToSea() { return gabi::call<BOOL>(0x0232CF30, this); }
W u32 PrmAbstract(fopAc_ac_c* a, s32 w, s32 s) { return gabi::call<u32>(0x0232EDC4, a, w, s); }
}  // namespace daObjBuoyflag
