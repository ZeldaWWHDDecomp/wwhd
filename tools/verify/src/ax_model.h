/* AX (snd_core) model of the `osmem ... ax` part: the port's runtime/src/hle/ax.cpp (f5554821) without the host-only
 * parts. Voices (ADPCM/PCM16/PCM8 decoding, sample-rate conversion, volume envelope, biquad, low-pass, TV and GamePad
 * device mixes), the 3 ms audio frame (process_voices, the output-mode faders, aux callbacks one frame behind, app frame
 * callbacks, the aux input store, the final-mix inputs and callbacks) and the snd_core imports, line for line in the
 * port's order and arithmetic (compiled, like the port, with -ffp-contract=off).
 * Left out: keep_dsp_channels (the port overwrites the game's DSP overload ratio at 10170030; the
 * console keeps the check), the host output (audio::push, 5.1, the GamePad-only host mix, the post-final-mix
 * upsampling), the stats/trace environment switches, the device pacing (frames are 3 ms of virtual time) and
 * game_clock (rate 1). AXGetDeviceMode answers stereo (the port with a stereo device). guest_data() is the identity
 * (USA build, runtime/src/guest_addr_identity.c).
 *
 * Templated on an environment E (the harness's guest memory and guest calls, or guestthreads_equiv.py's flat memory):
 *   uint8_t ld8(a); uint16_t ld16(a); uint32_t ld32(a); void st16(a, v); void st32(a, v); void zero(a, n);
 *   uint32_t runtime_alloc(size, align); uint32_t host_alloc(size, align);
 *   void call(uint32_t fn, int nargs, const uint32_t* args) (a guest callback on the AX frame thread). */
#pragma once
#include <stdint.h>
#include <algorithm>
#include <cmath>
#include <cstring>

namespace axm {

constexpr int kMaxVoices = 96;
constexpr int kSamples = 96;    /* per frame at 32 kHz */
constexpr int kSamples48 = 144; /* per frame at 48 kHz */
constexpr int kTvChannels = 6, kDrcChannels = 4, kBuses = 4, kAuxBuses = 3;
constexpr uint32_t kVpbIndex = 0x00, kVpbState = 0x04, kVpbPriority = 0x1C, kVpbCallback = 0x20, kVpbUser = 0x24,
                   kVpbOffsets = 0x34, kVpbCallbackEx = 0x48;
constexpr uint32_t kVpbSize = 0x58;
constexpr int kDrcAuxIdleFrames = 1000;
constexpr uint32_t kFaderRoot = 0x1018EC64u; /* GD(0x1018EC64): the sound manager pointer (identity map) */
constexpr uint32_t kMem2Start = 0x10000000u, kMem2End = 0x50000000u;

enum Format : uint16_t { ADPCM = 0, PCM16 = 0x0A, PCM8 = 0x19 };
enum FilterMode : uint16_t { FILTER_TAP = 0, FILTER_LINEAR = 1, FILTER_NONE = 2 };

struct ChMix {
    uint16_t vol = 0;
    int16_t delta = 0;
};
struct Voice {
    bool acquired = false;
    uint32_t vpb = 0;
    uint32_t state = 0;
    uint16_t type = 0;
    uint16_t format = 0, loop = 0;
    uint32_t loop_abs = 0, end_abs = 0, cur_abs = 0, samples = 0;
    uint32_t loop_count = 0;
    int16_t coef[16] = {};
    uint16_t scale = 0;
    int16_t yn1 = 0, yn2 = 0;
    uint16_t loop_scale = 0;
    int16_t loop_yn1 = 0, loop_yn2 = 0;
    uint16_t filter = FILTER_TAP;
    uint32_t ratio = 0x10000, frac = 0;
    int32_t prev = 0, cur = 0;
    uint16_t ve_vol = 0x8000;
    int16_t ve_delta = 0;
    uint16_t lpf_on = 0;
    int16_t lpf_yn1 = 0, lpf_a0 = 0, lpf_b0 = 0;
    uint16_t bq_on = 0;
    int16_t bq_b0 = 0, bq_b1 = 0, bq_b2 = 0, bq_a1 = 0, bq_a2 = 0;
    float bq_xn1 = 0, bq_xn2 = 0, bq_yn1 = 0, bq_yn2 = 0;
    ChMix tv[kTvChannels][kBuses];
};
struct DrcState {
    ChMix mix[kMaxVoices][kDrcChannels][kBuses];
    uint32_t aux_cb[kAuxBuses], aux_user[kAuxBuses];
    uint16_t aux_return[kAuxBuses];
};
struct GuestBuffers {
    uint32_t aux[2][kAuxBuses];
    uint32_t aux_ptrs = 0, aux_info = 0;
    uint32_t tv48 = 0, tv_ptrs = 0, tv_param = 0;
    uint32_t drc48 = 0, drc_ptrs = 0, drc_param = 0;
};
struct Fader {
    float value = 0, target = 0;
    uint32_t frames = 0;
    float goal() const { return frames ? target : value; }
};
/* audio_output_mode.h gamepad_audible / OutputSelect::update with the default source (auto) */
inline bool gamepad_audible(const Fader& tv, const Fader& drc) {
    if (drc.value <= 0 && drc.goal() <= 0) return false;
    return !(tv.value >= 1 && tv.goal() >= 1);
}

struct State {
    bool running = false;
    Voice voices[kMaxVoices];
    uint32_t vpb_base = 0;
    uint32_t app_frame_cb[64] = {};
    uint32_t final_mix_cb[3] = {};
    uint32_t aux_cb[kAuxBuses] = {}, aux_user[kAuxBuses] = {};
    uint16_t aux_return[kAuxBuses] = {0x8000, 0x8000, 0x8000};
    uint32_t upsample_stage[3] = {};
    DrcState drc = {{}, {}, {}, {0x8000, 0x8000, 0x8000}};
    float tv_bus[kBuses][kTvChannels][kSamples];
    float drc_bus[kBuses][kDrcChannels][kSamples];
    GuestBuffers G = {};
    int aux_frame = 0;
    float up_hist[kTvChannels] = {};
    uint32_t drc_aux[2][kAuxBuses] = {};
    int drc_aux_idle[kAuxBuses] = {};
    float drc_up_hist[kDrcChannels] = {};
    bool play_drc = false; /* OutputSelect::play_drc */
    uint64_t frames = 0;   /* frames run (receipts) */
};

inline uint32_t base_units(uint16_t fmt, uint32_t samples_ptr) {
    switch (fmt) {
    case ADPCM: return samples_ptr * 2;
    case PCM16: return samples_ptr / 2;
    default: return samples_ptr;
    }
}
template <class E> void write_offsets(E& e, const Voice& v) {
    uint32_t o = v.vpb + kVpbOffsets, b = base_units(v.format, v.samples);
    e.st16(o + 0, v.format);
    e.st16(o + 2, v.loop);
    e.st32(o + 4, v.loop_abs - b);
    e.st32(o + 8, v.end_abs - b);
    e.st32(o + 0xC, v.cur_abs - b);
    e.st32(o + 0x10, v.samples);
}
inline void voice_end(Voice& v) {
    if (v.loop) {
        v.cur_abs = v.loop_abs;
        v.loop_count++;
        if (v.format == ADPCM) {
            v.scale = v.loop_scale;
            if (v.type == 0) { v.yn1 = v.loop_yn1; v.yn2 = v.loop_yn2; }
        }
    } else {
        v.state = 0;
    }
}
template <class E> int32_t next_sample(E& e, Voice& v) {
    if (!v.state) return 0;
    int32_t s;
    switch (v.format) {
    case ADPCM: {
        uint32_t off = v.cur_abs;
        if ((off & 15) < 2) {
            off &= ~15u;
            v.scale = e.ld8(off >> 1);
            off += 2;
        }
        uint8_t byte = e.ld8(off >> 1);
        int32_t nib = (off & 1) ? (int8_t)(byte << 4) >> 4 : (int8_t)byte >> 4;
        int ci = (v.scale >> 4) & 7;
        int32_t val = ((nib << 11) * (1 << (v.scale & 15)) + v.coef[ci * 2] * v.yn1 + v.coef[ci * 2 + 1] * v.yn2 + 0x400) >> 11;
        s = std::clamp(val, -32768, 32767);
        v.yn2 = v.yn1;
        v.yn1 = (int16_t)s;
        uint32_t end = v.end_abs;
        if ((end & 15) < 2) end += 2 - (end & 15);
        if (off == end) voice_end(v);
        else v.cur_abs = off + 1;
        return s;
    }
    case PCM16: s = (int16_t)e.ld16(v.cur_abs * 2); break;
    case PCM8: s = (int8_t)e.ld8(v.cur_abs) << 8; break;
    default: v.state = 0; return 0;
    }
    if (v.cur_abs == v.end_abs) voice_end(v);
    else v.cur_abs++;
    return s;
}
template <class E> void decode(E& e, Voice& v, float* out) {
    if (v.filter == FILTER_NONE) {
        for (int i = 0; i < kSamples; i++) out[i] = (float)(next_sample(e, v) * 256);
        return;
    }
    uint32_t ratio = std::min<uint32_t>(v.ratio, 0x80000);
    for (int i = 0; i < kSamples; i++) {
        v.frac += ratio;
        while (v.frac >= 0x10000) {
            v.prev = v.cur;
            v.cur = next_sample(e, v);
            v.frac -= 0x10000;
        }
        float f = v.frac / 65536.0f;
        out[i] = ((float)v.prev + (float)(v.cur - v.prev) * f) * 256.0f;
    }
}
inline void apply_envelope(Voice& v, float* s) {
    if (v.ve_vol == 0x8000 && v.ve_delta == 0) return;
    float vol = v.ve_vol / 32768.0f, d = v.ve_delta / 32768.0f;
    for (int i = 0; i < kSamples; i++) {
        vol += d;
        s[i] *= vol;
    }
    if (v.ve_delta) v.ve_vol = (uint16_t)std::clamp<int32_t>((int32_t)v.ve_vol + v.ve_delta * kSamples, 0, 0xFFFF);
}
inline void apply_biquad(Voice& v, float* s) {
    if (!v.bq_on) return;
    float b0 = v.bq_b0 / 16384.0f, b1 = v.bq_b1 / 16384.0f, b2 = v.bq_b2 / 16384.0f;
    float a1 = v.bq_a1 / 16384.0f, a2 = v.bq_a2 / 16384.0f;
    for (int i = 0; i < kSamples; i++) {
        float x = s[i] / 256.0f;
        float y = b0 * x + b1 * v.bq_xn1 + b2 * v.bq_xn2 + a1 * v.bq_yn1 + a2 * v.bq_yn2;
        s[i] = y * 256.0f;
        y = std::clamp(y, -32768.0f, 32767.0f);
        v.bq_yn2 = v.bq_yn1;
        v.bq_xn2 = v.bq_xn1;
        v.bq_yn1 = y;
        v.bq_xn1 = x;
    }
}
inline void apply_lpf(Voice& v, float* s) {
    if (!v.lpf_on) return;
    float a0 = v.lpf_a0 / 32767.0f, b0 = v.lpf_b0 / 32767.0f;
    float prev = v.lpf_yn1 * 256.0f / 32767.0f;
    for (int i = 0; i < kSamples; i++) {
        s[i] = a0 * s[i] - b0 * prev;
        prev = s[i];
    }
    v.lpf_yn1 = (int16_t)std::clamp(prev / 256.0f * 32767.0f, -32768.0f, 32767.0f);
}
inline void mix_into(const float* in, float* out, ChMix& m) {
    float vol = m.vol / 32768.0f;
    if (m.delta) {
        float d = m.delta / 32768.0f;
        for (int i = 0; i < kSamples; i++) {
            vol += d;
            out[i] += in[i] * vol;
        }
        m.vol = (uint16_t)std::clamp(vol * 32768.0f, 0.0f, 65535.0f);
    } else {
        for (int i = 0; i < kSamples; i++) out[i] += in[i] * vol;
    }
}
template <class E> void process_voices(E& e, State& S) {
    memset(S.tv_bus, 0, sizeof S.tv_bus);
    memset(S.drc_bus, 0, sizeof S.drc_bus);
    float buf[kSamples];
    for (Voice& v : S.voices) {
        if (!v.acquired || v.state != 1) continue;
        decode(e, v, buf);
        apply_envelope(v, buf);
        apply_biquad(v, buf);
        apply_lpf(v, buf);
        auto& drc = S.drc.mix[&v - S.voices];
        for (int c = 0; c < kTvChannels; c++)
            for (int b = 0; b < kBuses; b++)
                if (v.tv[c][b].vol || v.tv[c][b].delta) mix_into(buf, S.tv_bus[b][c], v.tv[c][b]);
        for (int c = 0; c < kDrcChannels; c++)
            for (int b = 0; b < kBuses; b++)
                if (drc[c][b].vol || drc[c][b].delta) mix_into(buf, S.drc_bus[b][c], drc[c][b]);
        if (!v.state) e.st32(v.vpb + kVpbState, 0);
        e.st32(v.vpb + kVpbOffsets + 0xC, v.cur_abs - base_units(v.format, v.samples));
    }
}
template <class E> void init_buffers(E& e, State& S) {
    for (auto& f : S.G.aux)
        for (auto& b : f) {
            b = e.runtime_alloc(4 * kTvChannels * kSamples48, 64);
            e.zero(b, 4 * kTvChannels * kSamples48);
        }
    S.G.aux_ptrs = e.runtime_alloc(4 * kTvChannels, 32);
    S.G.aux_info = e.runtime_alloc(8, 32);
    S.G.tv48 = e.runtime_alloc(4 * kTvChannels * kSamples48, 64);
    S.G.tv_ptrs = e.runtime_alloc(4 * kTvChannels, 32);
    S.G.tv_param = e.runtime_alloc(0x10, 32);
    S.G.drc48 = e.runtime_alloc(4 * kDrcChannels * 2 * kSamples48, 64);
    S.G.drc_ptrs = e.runtime_alloc(4 * kDrcChannels * 2, 32);
    S.G.drc_param = e.runtime_alloc(0x10, 32);
    for (auto& f : S.drc_aux)
        for (auto& b : f) b = e.host_alloc(4 * kDrcChannels * kSamples, 64);
}
inline bool guest_obj(uint32_t a) { return a >= kMem2Start && a < kMem2End - 0x400 && !(a & 3); }
template <class E> bool read_game_faders(E& e, Fader& tv, Fader& drc) {
    uint32_t mgr = e.ld32(kFaderRoot);
    if (!guest_obj(mgr)) return false;
    uint32_t x = e.ld32(mgr + 0x10);
    if (!guest_obj(x)) return false;
    uint32_t player = e.ld32(x + 4);
    if (!guest_obj(player)) return false;
    auto fader = [&](uint32_t f) {
        Fader r;
        uint32_t v = e.ld32(f), t = e.ld32(f + 4);
        memcpy(&r.value, &v, 4);
        memcpy(&r.target, &t, 4);
        r.frames = e.ld32(f + 0xC);
        return r;
    };
    tv = fader(player + 0x194);
    drc = fader(player + 0x1AC);
    return true;
}
template <class E> void update_output_mode(E& e, State& S) {
    Fader tv, drc;
    bool ok = read_game_faders(e, tv, drc);
    S.play_drc = ok && gamepad_audible(tv, drc);
}
template <class E> void run_aux_callbacks(E& e, State& S) {
    int out = 1 - S.aux_frame;
    for (int b = 0; b < kAuxBuses; b++) {
        uint32_t buf = S.G.aux[out][b];
        if (!S.aux_cb[b]) {
            e.zero(buf, 4 * kTvChannels * kSamples);
            continue;
        }
        for (int ch = 0; ch < kTvChannels; ch++) e.st32(S.G.aux_ptrs + 4 * ch, buf + 4 * ch * kSamples);
        e.st32(S.G.aux_info, kTvChannels);
        e.st32(S.G.aux_info + 4, kSamples);
        uint32_t a[3] = {S.G.aux_ptrs, S.aux_user[b], S.G.aux_info};
        e.call(S.aux_cb[b], 3, a);
    }
    for (int b = 0; b < kAuxBuses; b++) {
        uint32_t buf = S.drc_aux[out][b];
        if (!S.drc.aux_cb[b] || !S.play_drc || S.drc_aux_idle[b] > kDrcAuxIdleFrames) {
            e.zero(buf, 4 * kDrcChannels * kSamples);
            continue;
        }
        for (int ch = 0; ch < kDrcChannels; ch++) e.st32(S.G.aux_ptrs + 4 * ch, buf + 4 * ch * kSamples);
        e.st32(S.G.aux_info, kDrcChannels);
        e.st32(S.G.aux_info + 4, kSamples);
        uint32_t a[3] = {S.G.aux_ptrs, S.drc.aux_user[b], S.G.aux_info};
        e.call(S.drc.aux_cb[b], 3, a);
    }
}
template <class E> void store_aux_input(E& e, State& S) {
    for (int b = 0; b < kAuxBuses; b++) {
        if (!S.aux_cb[b]) continue;
        uint32_t buf = S.G.aux[S.aux_frame][b];
        for (int ch = 0; ch < kTvChannels; ch++)
            for (int i = 0; i < kSamples; i++) e.st32(buf + 4 * (ch * kSamples + i), (uint32_t)((int32_t)S.tv_bus[b + 1][ch][i] >> 8));
    }
    for (int b = 0; b < kAuxBuses; b++) {
        if (!S.drc.aux_cb[b]) continue;
        uint32_t buf = S.drc_aux[S.aux_frame][b];
        bool any = false;
        for (int ch = 0; ch < kDrcChannels; ch++)
            for (int i = 0; i < kSamples; i++) {
                int32_t s = (int32_t)S.drc_bus[b + 1][ch][i] >> 8;
                any |= s != 0;
                e.st32(buf + 4 * (ch * kSamples + i), (uint32_t)s);
            }
        S.drc_aux_idle[b] = any ? 0 : std::min(S.drc_aux_idle[b] + 1, kDrcAuxIdleFrames + 1);
    }
}
template <class E> void merge_tv(E& e, State& S, int32_t out[kTvChannels][kSamples]) {
    int in = 1 - S.aux_frame;
    for (int ch = 0; ch < kTvChannels; ch++)
        for (int i = 0; i < kSamples; i++) {
            float s = S.tv_bus[0][ch][i];
            for (int b = 0; b < kAuxBuses; b++)
                if (S.aux_cb[b]) s += (float)((int32_t)e.ld32(S.G.aux[in][b] + 4 * (ch * kSamples + i)) << 8) * (S.aux_return[b] / 32768.0f);
            out[ch][i] = (int32_t)std::clamp(s, -2147483648.0f, 2147483520.0f);
        }
}
template <class E> void merge_drc(E& e, State& S, int32_t out[kDrcChannels][kSamples]) {
    int in = 1 - S.aux_frame;
    for (int ch = 0; ch < kDrcChannels; ch++)
        for (int i = 0; i < kSamples; i++) {
            float s = S.drc_bus[0][ch][i];
            for (int b = 0; b < kAuxBuses; b++)
                if (S.drc.aux_cb[b])
                    s += (float)((int32_t)e.ld32(S.drc_aux[in][b] + 4 * (ch * kSamples + i)) << 8) * (S.drc.aux_return[b] / 32768.0f);
            out[ch][i] = (int32_t)std::clamp(s, -2147483648.0f, 2147483520.0f);
        }
}
inline void upsample(const int32_t* in, int32_t* out, float& hist, int shift) {
    float prev = hist;
    for (int i = 0; i < kSamples; i += 2) {
        float s0 = (float)in[i], s1 = (float)in[i + 1];
        *out++ = (int32_t)(prev * 0.66666669f + s0 * 0.33333331f) >> shift;
        *out++ = (int32_t)s0 >> shift;
        *out++ = (int32_t)(s1 * 0.66666669f + s0 * 0.33333331f) >> shift;
        prev = s1;
    }
    hist = prev;
}
template <class E> void call_final_mix(E& e, State& S, int dev, uint32_t param, uint32_t ptrs, uint32_t data, int channels, int devices,
                                       int samples) {
    for (int i = 0; i < channels * devices; i++) e.st32(ptrs + 4 * i, data + 4 * i * samples);
    e.st32(param + 0, ptrs);
    e.st16(param + 4, channels);
    e.st16(param + 6, samples);
    e.st16(param + 8, devices);
    e.st16(param + 0xA, channels);
    if (S.final_mix_cb[dev]) {
        uint32_t a[1] = {param};
        e.call(S.final_mix_cb[dev], 1, a);
    }
}
template <class E> int final_mix_input(E& e, const int32_t (*in)[kSamples], int channels, uint32_t data, bool before, float* hist) {
    int n = before ? kSamples48 : kSamples;
    for (int ch = 0; ch < channels; ch++) {
        if (before) {
            int32_t tmp[kSamples48];
            upsample(in[ch], tmp, hist[ch], 8);
            for (int i = 0; i < n; i++) e.st32(data + 4 * (ch * n + i), (uint32_t)tmp[i]);
        } else {
            for (int i = 0; i < n; i++) e.st32(data + 4 * (ch * n + i), (uint32_t)(in[ch][i] >> 8));
        }
    }
    return n;
}
/* output_frame up to the final-mix callbacks (what follows is host output only) */
template <class E> void output_frame(E& e, State& S) {
    static thread_local int32_t tv[kTvChannels][kSamples], drc[kDrcChannels][kSamples];
    merge_tv(e, S, tv);
    merge_drc(e, S, drc);
    int n_tv = final_mix_input(e, tv, kTvChannels, S.G.tv48, S.upsample_stage[0] == 0, S.up_hist);
    call_final_mix(e, S, 0, S.G.tv_param, S.G.tv_ptrs, S.G.tv48, kTvChannels, 1, n_tv);
    int n_drc = final_mix_input(e, drc, kDrcChannels, S.G.drc48, S.upsample_stage[1] == 0, S.drc_up_hist);
    e.zero(S.G.drc48 + 4 * kDrcChannels * n_drc, 4 * kDrcChannels * n_drc);
    call_final_mix(e, S, 1, S.G.drc_param, S.G.drc_ptrs, S.G.drc48, kDrcChannels, 2, n_drc);
}
/* one pass of frame_thread's loop (without keep_dsp_channels and the pacing) */
template <class E> void frame(E& e, State& S) {
    process_voices(e, S);
    update_output_mode(e, S);
    run_aux_callbacks(e, S);
    uint32_t cbs[64];
    memcpy(cbs, S.app_frame_cb, sizeof cbs);
    for (uint32_t cb : cbs)
        if (cb) e.call(cb, 0, nullptr);
    store_aux_input(e, S);
    output_frame(e, S);
    S.aux_frame = 1 - S.aux_frame;
    S.frames++;
}
inline Voice* voice(State& S, uint32_t vpb) {
    if (!S.vpb_base || vpb < S.vpb_base) return nullptr;
    uint32_t i = (vpb - S.vpb_base) / kVpbSize;
    return i < kMaxVoices ? &S.voices[i] : nullptr;
}

/* ---- the imports. a[3..10] = r3..r10, f1 = the first float argument; *r3 = the result, *r4 unused; returns false for
 * a void import (r3 unchanged) */
enum Imp : uint32_t {
    AXAcquireVoiceEx = 0xC0006C80u, AXComputeLpfCoefs = 0xC0006C90u, AXDecodeAdpcmData = 0xC0006C98u,
    AXDeregisterAppFrameCallback = 0xC0006CA0u, AXFreeVoice = 0xC0006CA8u, AXGetDeviceFinalMixCallback = 0xC0006D38u,
    AXGetDeviceMode = 0xC0006D48u, AXGetSwapProfile = 0xC0006DE0u, AXGetVoiceCurrentOffsetEx = 0xC0006DE8u,
    AXGetVoiceLoopCount = 0xC0006DF0u, AXGetVoiceOffsets = 0xC0006E00u, AXInit = 0xC0006E30u, AXInitProfile = 0xC0006E48u,
    AXQuit = 0xC0006E90u, AXRegisterAppFrameCallback = 0xC0006E98u, AXRegisterAuxCallback = 0xC0006EB8u,
    AXRegisterDeviceFinalMixCallback = 0xC0006ED8u, AXRmtAdvancePtr = 0xC0006EF0u, AXRmtGetSamples = 0xC0006EF8u,
    AXRmtGetSamplesLeft = 0xC0006F00u, AXSetAuxReturnVolume = 0xC0006F20u, AXSetDRCVSDownmixBalance = 0xC0006F30u,
    AXSetDRCVSLC = 0xC0006F38u, AXSetDRCVSMode = 0xC0006F50u, AXSetDRCVSOutputGain = 0xC0006F58u,
    AXSetDRCVSSpeakerPosition = 0xC0006F60u, AXSetDRCVSSurroundDepth = 0xC0006F68u, AXSetDRCVSSurroundLevelGain = 0xC0006F70u,
    AXSetDefaultMixerSelect = 0xC0006F80u, AXSetDeviceCompressor = 0xC0006F88u, AXSetDeviceLinearUpsampler = 0xC0006F98u,
    AXSetDeviceUpsampleStage = 0xC0006FB0u, AXSetVoiceAdpcm = 0xC0007010u, AXSetVoiceAdpcmLoop = 0xC0007018u,
    AXSetVoiceBiquad = 0xC0007020u, AXSetVoiceBiquadCoefs = 0xC0007028u, AXSetVoiceDeviceMix = 0xC0007048u,
    AXSetVoiceEndOffsetEx = 0xC0007058u, AXSetVoiceLoop = 0xC0007070u, AXSetVoiceLoopOffsetEx = 0xC0007080u,
    AXSetVoiceLpf = 0xC0007088u, AXSetVoiceLpfCoefs = 0xC0007090u, AXSetVoiceMixerSelect = 0xC00070A0u,
    AXSetVoiceOffsets = 0xC00070A8u, AXSetVoicePriority = 0xC00070B8u, AXSetVoiceRmtIIR = 0xC00070C0u,
    AXSetVoiceRmtIIRCoefs = 0xC00070C8u, AXSetVoiceRmtOn = 0xC00070D8u, AXSetVoiceSrc = 0xC00070F0u,
    AXSetVoiceSrcRatio = 0xC00070F8u, AXSetVoiceSrcType = 0xC0007100u, AXSetVoiceState = 0xC0007108u,
    AXSetVoiceType = 0xC0007110u, AXSetVoiceVe = 0xC0007118u, AXUserBegin = 0xC0007138u, AXUserEnd = 0xC0007140u,
    AXUserIsProtected = 0xC0007148u, DRCVS_Process = 0xC0007178u, DRCVS_SetOutputMode = 0xC00071B8u,
};
static const uint32_t kImports[] = {AXAcquireVoiceEx, AXComputeLpfCoefs, AXDecodeAdpcmData, AXDeregisterAppFrameCallback, AXFreeVoice,
    AXGetDeviceFinalMixCallback, AXGetDeviceMode, AXGetSwapProfile, AXGetVoiceCurrentOffsetEx, AXGetVoiceLoopCount, AXGetVoiceOffsets,
    AXInit, AXInitProfile, AXQuit, AXRegisterAppFrameCallback, AXRegisterAuxCallback, AXRegisterDeviceFinalMixCallback,
    AXRmtAdvancePtr, AXRmtGetSamples, AXRmtGetSamplesLeft, AXSetAuxReturnVolume, AXSetDRCVSDownmixBalance, AXSetDRCVSLC,
    AXSetDRCVSMode, AXSetDRCVSOutputGain, AXSetDRCVSSpeakerPosition, AXSetDRCVSSurroundDepth, AXSetDRCVSSurroundLevelGain,
    AXSetDefaultMixerSelect, AXSetDeviceCompressor, AXSetDeviceLinearUpsampler, AXSetDeviceUpsampleStage, AXSetVoiceAdpcm,
    AXSetVoiceAdpcmLoop, AXSetVoiceBiquad, AXSetVoiceBiquadCoefs, AXSetVoiceDeviceMix, AXSetVoiceEndOffsetEx, AXSetVoiceLoop,
    AXSetVoiceLoopOffsetEx, AXSetVoiceLpf, AXSetVoiceLpfCoefs, AXSetVoiceMixerSelect, AXSetVoiceOffsets, AXSetVoicePriority,
    AXSetVoiceRmtIIR, AXSetVoiceRmtIIRCoefs, AXSetVoiceRmtOn, AXSetVoiceSrc, AXSetVoiceSrcRatio, AXSetVoiceSrcType, AXSetVoiceState,
    AXSetVoiceType, AXSetVoiceVe, AXUserBegin, AXUserEnd, AXUserIsProtected, DRCVS_Process, DRCVS_SetOutputMode};
inline bool is_import(uint32_t t) {
    for (uint32_t s : kImports) if (s == t) return true;
    return false;
}

/* returns: 0 void (r3 unchanged), 1 r3 = *r; started: AXInit started the frame thread (the caller arms it) */
template <class E> int call(E& e, State& S, uint32_t t, const uint32_t* a, double f1, uint32_t* r, bool* started) {
    auto A = [&](int i) { return a[3 + i]; };
    *started = false;
    switch (t) {
    case AXInit:
        if (S.running) return 0;
        S.vpb_base = e.runtime_alloc(kVpbSize * kMaxVoices, 32);
        for (int i = 0; i < kMaxVoices; i++) {
            S.voices[i] = Voice{};
            memset(S.drc.mix[i], 0, sizeof S.drc.mix[i]);
            S.voices[i].vpb = S.vpb_base + i * kVpbSize;
            e.st32(S.voices[i].vpb + kVpbIndex, i);
        }
        init_buffers(e, S);
        S.running = true;
        *started = true;
        return 0;
    case AXQuit: S.running = false; return 0;
    case AXUserBegin: case AXUserEnd: case AXInitProfile: case AXSetDefaultMixerSelect: case AXSetVoicePriority:
    case AXSetVoiceRmtOn: case AXSetVoiceRmtIIR: case AXSetVoiceRmtIIRCoefs: case AXDecodeAdpcmData:
        return 0;
    case AXUserIsProtected: case AXGetSwapProfile: case AXSetDeviceLinearUpsampler: case AXSetDeviceCompressor:
    case AXSetDRCVSMode: case AXSetDRCVSLC: case AXSetDRCVSSpeakerPosition: case AXSetDRCVSSurroundDepth:
    case AXSetDRCVSDownmixBalance: case AXSetDRCVSSurroundLevelGain: case AXSetDRCVSOutputGain: case DRCVS_SetOutputMode:
    case DRCVS_Process: case AXRmtGetSamplesLeft: case AXRmtGetSamples: case AXRmtAdvancePtr: case AXSetVoiceMixerSelect:
        *r = 0; return 1;
    case AXRegisterAppFrameCallback:
        for (uint32_t& cb : S.app_frame_cb)
            if (!cb) { cb = A(0); break; }
        *r = 0; return 1;
    case AXDeregisterAppFrameCallback:
        for (uint32_t& cb : S.app_frame_cb)
            if (cb == A(0)) cb = 0;
        *r = 0; return 1;
    case AXRegisterDeviceFinalMixCallback:
        if (A(0) < 3) S.final_mix_cb[A(0)] = A(1);
        *r = 0; return 1;
    case AXGetDeviceFinalMixCallback:
        if (A(1)) e.st32(A(1), A(0) < 3 ? S.final_mix_cb[A(0)] : 0);
        *r = 0; return 1;
    case AXRegisterAuxCallback: {
        uint32_t dev = A(0), idx = A(1), bus = A(2);
        if (bus >= (uint32_t)kAuxBuses) { *r = (uint32_t)-5; return 1; }
        if (dev == 0) {
            S.aux_cb[bus] = A(3);
            S.aux_user[bus] = A(4);
        } else if (dev == 1 && idx == 0) {
            S.drc.aux_cb[bus] = A(3);
            S.drc.aux_user[bus] = A(4);
            S.drc_aux_idle[bus] = 0;
        }
        *r = 0; return 1;
    }
    case AXSetAuxReturnVolume:
        if (A(2) < (uint32_t)kAuxBuses) {
            if (A(0) == 0) S.aux_return[A(2)] = (uint16_t)A(3);
            else if (A(0) == 1 && A(1) == 0) S.drc.aux_return[A(2)] = (uint16_t)A(3);
        }
        *r = 0; return 1;
    case AXGetDeviceMode: /* stereo (AX_MODE_STEREO) */
        if (A(1)) e.st32(A(1), 0);
        *r = 0; return 1;
    case AXSetDeviceUpsampleStage:
        if (A(0) < 3) S.upsample_stage[A(0)] = A(1);
        *r = 0; return 1;
    case AXAcquireVoiceEx:
        for (Voice& v : S.voices) {
            if (v.acquired) continue;
            uint32_t vpb = v.vpb;
            v = Voice{};
            memset(S.drc.mix[&v - S.voices], 0, sizeof S.drc.mix[0]);
            v.vpb = vpb;
            v.acquired = true;
            e.st32(vpb + kVpbState, 0);
            e.st32(vpb + kVpbPriority, A(0));
            e.st32(vpb + kVpbCallback, 0);
            e.st32(vpb + kVpbCallbackEx, A(1));
            e.st32(vpb + kVpbUser, A(2));
            *r = vpb;
            return 1;
        }
        *r = 0; return 1;
    case AXFreeVoice:
        if (Voice* v = voice(S, A(0))) {
            v->acquired = false;
            v->state = 0;
            e.st32(v->vpb + kVpbState, 0);
        }
        return 0;
    case AXSetVoiceState:
        if (Voice* v = voice(S, A(0))) {
            v->state = A(1) & 0xFFFF;
            e.st32(v->vpb + kVpbState, v->state);
        }
        return 0;
    case AXSetVoiceType:
        if (Voice* v = voice(S, A(0))) v->type = (uint16_t)A(1);
        return 0;
    case AXSetVoiceOffsets: {
        Voice* v = voice(S, A(0));
        uint32_t o = A(1);
        if (!v) return 0;
        v->format = e.ld16(o);
        v->loop = e.ld16(o + 2);
        v->samples = e.ld32(o + 0x10);
        uint32_t b = base_units(v->format, v->samples);
        v->loop_abs = b + e.ld32(o + 4);
        v->end_abs = b + e.ld32(o + 8);
        v->cur_abs = b + e.ld32(o + 0xC);
        write_offsets(e, *v);
        return 0;
    }
    case AXGetVoiceOffsets: {
        Voice* v = voice(S, A(0));
        uint32_t o = A(1);
        if (!v) return 0;
        uint32_t b = base_units(v->format, v->samples);
        e.st16(o, v->format);
        e.st16(o + 2, v->loop);
        e.st32(o + 4, v->loop_abs - b);
        e.st32(o + 8, v->end_abs - b);
        e.st32(o + 0xC, v->cur_abs - b);
        e.st32(o + 0x10, v->samples);
        return 0;
    }
    case AXSetVoiceLoop:
        if (Voice* v = voice(S, A(0))) { v->loop = (uint16_t)A(1); write_offsets(e, *v); }
        return 0;
    case AXSetVoiceEndOffsetEx:
        if (Voice* v = voice(S, A(0))) {
            v->samples = A(2);
            v->end_abs = base_units(v->format, v->samples) + A(1);
            write_offsets(e, *v);
        }
        return 0;
    case AXSetVoiceLoopOffsetEx:
        if (Voice* v = voice(S, A(0))) {
            v->samples = A(2);
            v->loop_abs = base_units(v->format, v->samples) + A(1);
            write_offsets(e, *v);
        }
        return 0;
    case AXGetVoiceCurrentOffsetEx: {
        Voice* v = voice(S, A(0));
        *r = v ? v->cur_abs - base_units(v->format, A(1)) : 0;
        return 1;
    }
    case AXGetVoiceLoopCount: {
        Voice* v = voice(S, A(0));
        *r = v ? v->loop_count : 0;
        return 1;
    }
    case AXSetVoiceSrcRatio:
        if (Voice* v = voice(S, A(0))) {
            double rr = f1 * 65536.0;
            v->ratio = (uint32_t)std::clamp(rr, 0.0, (double)0x80000);
        }
        *r = 0; return 1;
    case AXSetVoiceSrc: {
        uint32_t s = A(1);
        if (Voice* v = voice(S, A(0))) {
            v->ratio = (uint32_t)e.ld16(s) << 16 | e.ld16(s + 2);
            v->frac = e.ld16(s + 4);
            v->prev = (int16_t)e.ld16(s + 6);
            v->cur = (int16_t)e.ld16(s + 8);
        }
        return 0;
    }
    case AXSetVoiceSrcType:
        if (Voice* v = voice(S, A(0))) {
            uint32_t tt = A(1);
            v->filter = tt == 0 ? FILTER_NONE : tt == 1 ? FILTER_LINEAR : FILTER_TAP;
        }
        return 0;
    case AXSetVoiceVe:
        if (Voice* v = voice(S, A(0))) {
            v->ve_vol = e.ld16(A(1));
            v->ve_delta = (int16_t)e.ld16(A(1) + 2);
        }
        return 0;
    case AXSetVoiceDeviceMix: {
        Voice* v = voice(S, A(0));
        uint32_t dev = A(1), idx = A(2), mix = A(3);
        if (!v || !mix) { *r = (uint32_t)-3; return 1; }
        if (dev == 0 && idx == 0)
            for (int ch = 0; ch < kTvChannels; ch++)
                for (int b = 0; b < kBuses; b++) {
                    uint32_t q = mix + 4 * (ch * kBuses + b);
                    v->tv[ch][b].vol = e.ld16(q);
                    v->tv[ch][b].delta = (int16_t)e.ld16(q + 2);
                }
        if (dev == 1 && idx == 0)
            for (int ch = 0; ch < kDrcChannels; ch++)
                for (int b = 0; b < kBuses; b++) {
                    uint32_t q = mix + 4 * (ch * kBuses + b);
                    S.drc.mix[v - S.voices][ch][b].vol = e.ld16(q);
                    S.drc.mix[v - S.voices][ch][b].delta = (int16_t)e.ld16(q + 2);
                }
        *r = 0; return 1;
    }
    case AXSetVoiceAdpcm: {
        uint32_t p = A(1);
        if (Voice* v = voice(S, A(0))) {
            for (int i = 0; i < 16; i++) v->coef[i] = (int16_t)e.ld16(p + 2 * i);
            v->scale = e.ld16(p + 0x22);
            v->yn1 = (int16_t)e.ld16(p + 0x24);
            v->yn2 = (int16_t)e.ld16(p + 0x26);
        }
        return 0;
    }
    case AXSetVoiceAdpcmLoop: {
        uint32_t p = A(1);
        if (Voice* v = voice(S, A(0))) {
            v->loop_scale = e.ld16(p);
            v->loop_yn1 = (int16_t)e.ld16(p + 2);
            v->loop_yn2 = (int16_t)e.ld16(p + 4);
        }
        return 0;
    }
    case AXSetVoiceLpf: {
        uint32_t p = A(1);
        if (Voice* v = voice(S, A(0))) {
            v->lpf_on = e.ld16(p);
            v->lpf_yn1 = (int16_t)e.ld16(p + 2);
            v->lpf_a0 = (int16_t)e.ld16(p + 4);
            v->lpf_b0 = (int16_t)e.ld16(p + 6);
        }
        return 0;
    }
    case AXSetVoiceLpfCoefs:
        if (Voice* v = voice(S, A(0))) {
            v->lpf_a0 = (int16_t)A(1);
            v->lpf_b0 = (int16_t)A(2);
        }
        return 0;
    case AXSetVoiceBiquad: {
        uint32_t p = A(1);
        if (Voice* v = voice(S, A(0))) {
            v->bq_on = e.ld16(p);
            v->bq_xn1 = (int16_t)e.ld16(p + 2);
            v->bq_xn2 = (int16_t)e.ld16(p + 4);
            v->bq_yn1 = (int16_t)e.ld16(p + 6);
            v->bq_yn2 = (int16_t)e.ld16(p + 8);
            v->bq_b0 = (int16_t)e.ld16(p + 0xA);
            v->bq_b1 = (int16_t)e.ld16(p + 0xC);
            v->bq_b2 = (int16_t)e.ld16(p + 0xE);
            v->bq_a1 = (int16_t)e.ld16(p + 0x10);
            v->bq_a2 = (int16_t)e.ld16(p + 0x12);
        }
        return 0;
    }
    case AXSetVoiceBiquadCoefs:
        if (Voice* v = voice(S, A(0))) {
            v->bq_b0 = (int16_t)A(1);
            v->bq_b1 = (int16_t)A(2);
            v->bq_b2 = (int16_t)A(3);
            v->bq_a1 = (int16_t)A(4);
            v->bq_a2 = (int16_t)A(5);
        }
        return 0;
    case AXComputeLpfCoefs: {
        float t1 = cosf((float)A(0) / 32000.0f * 6.2831855f);
        float t2 = 2.0f - t1;
        t1 = -((sqrtf(t2 * t2 - 1.0f) - t2) * 32768.0f);
        uint16_t rr = (uint16_t)t1;
        e.st16(A(1), 0x7FFF - rr);
        e.st16(A(2), rr);
        return 0;
    }
    default: return -1;
    }
}

}  // namespace axm
