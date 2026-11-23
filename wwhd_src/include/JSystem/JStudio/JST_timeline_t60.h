/* JST_timeline_t60.h (WWHD, true 60 build): the JStudio cutscene timeline on a general step
 * length (dt = 1/N).
 *
 * The timeline advances once per 30 Hz frame: the demo manager's dDemo_update 025291C8 (the play scene's execute, frame-
 * level work: the first-step frame-level rule) calls TControl::forward(1) 0283D514, which runs the sequences (waits, flags, paragraphs: the
 * cutscene's discrete events) and ages every variable value by one frame (TAdaptor::adaptor_updateVariableValue
 * 0283A48C), evaluates it at age x secondPerFrame (update_time_ 02839DB4 / update_functionValue_ 02839DF4), calls its
 * output and the adaptor's do_update (actor / camera / light / fog / particle / sound objects take the values).
 * That stays exactly as it is (forward(1) once per frame, dt 1; the HD forward(55) / forward(20) skips of Demo46 too).
 *
 * At N steps per frame the steps of the frame see the timeline at its in-between times ("evaluate only"): on step k of N
 * every continuous value (update_time_ / update_functionValue_) of a running object (status WAIT or SUSPEND, sequence
 * active) is evaluated at (age - 1 + k/N) x secondPerFrame, its output called and its adaptor's do_update run with 0
 * frames (no age, wait, fade or sequence advance; the particle adaptor's fade count does not move at 0 frames). The
 * frame's last step (k = N, tick30) evaluates at age itself: the frame-end state is the one forward(1) left, bit for bit
 * (the same expressions), so every unconverted reader on the tick30 step sees the original values.
 *   - Only with the frame-level work on the frame's first step (WWHD_T60_SCENE_FIRST, the default since the first-step frame-level rule):
 *     forward(1) has run on step 1 before any converted step. Step 1's pass runs at the end of dDemo_update, the
 *     later steps' at the start of their step (fapGm_Execute, before fpcM_Management), only in a frame whose step 1
 *     forwarded the timeline (a latch keyed by l_demo with the frame counter g_Counter.mCounter0; a frame whose scene
 *     did not run, pause or menu, keeps the values). Scene-last order (SCENE_FIRST=0): no pass, the 30 Hz values hold.
 *   - Not evaluated: immediate values (update_immediate_ applies once on the next forward), cleared ones, a saturated age
 *     (0xFFFFFFFF), objects that ended or are inactive. A value set this frame (age 0) evaluates at 0 (t >= 0).
 *   - Outputs skipped on the passes: the sound / particle fade starts and ends (Sound_beginFade 0283DE84, Sound_endFade
 *     0283E020, Particle_beginFade 0283EB98, Particle_endFade 0283ECE8): discrete starts that set immediate values
 *     drive; a time-driven one would start a sound per step.
 *   - Demo46's skip frames (forward(55) / forward(20)) take the pass across the frame's last frame of timeline time
 *     (age - 1 .. age): the jump happens on step 1, the steps then move on.
 * Switch: the port's WWHD_T60_JST=0 turns the pass off (default on). dt = 1 / N = 1: no pass, the original. The verify harness has no scene-first mode: the pass is inert there (dt-1 check of
 * dDemo_update / fapGm_Execute is the original). */
#pragma once
#include "true60.h"
#include "f_pc/f_pc_t60.h"
#include "JSystem/JStudio/JST_timeline.h"

namespace jst_t60 {
enum : u32 {
    kUpdateTime = 0x02839DB4,          /* TVariableValue::update_time_ */
    kUpdateFunctionValue = 0x02839DF4, /* TVariableValue::update_functionValue_ */
    kSoundBeginFade = 0x0283DE84,
    kSoundEndFade = 0x0283E020,
    kParticleBeginFade = 0x0283EB98,
    kParticleEndFade = 0x0283ECE8,
    kDemoManager = 0x101D5FE4,         /* d_demo.cpp l_demo (dDemo_manager_c: mControl +4, mCurFile +0x20, mMode +0x2C) */
    kFrameCounter = 0x101FF558,        /* g_Counter.mCounter0 (counts 30 Hz frames, after fpcM_Management on tick30) */
};

/* one value at time t (frames, >= 0); the exact frame form when t is the value's age (offset 0) */
inline void evaluate_value(jst_timeline::TVariableValue* v, jst_timeline::TAdaptor* adaptor, f64 seconds, f64 offset) {
    u32 update = v->mUpdate.get();
    if (update != kUpdateTime && update != kUpdateFunctionValue)
        return;
    u32 age = u32(v->mAge);
    if (age == 0xFFFFFFFFu)
        return;
    f64 t = static_cast<f64>(age);
    if (offset != 0.0) {
        t += offset;
        if (t < 0.0)
            t = 0.0;
    }
    if (update == kUpdateTime) {
        f64 time = t * seconds;
        f64 value = time * static_cast<f64>(f32(v->mParam.val));
        v->mValue = static_cast<f32>(value);
    } else {
        u32 fv = u32(v->mParam.bits);
        u32 fn = gabi::at<jst_timeline::TFunctionValue>(fv)->__vtbl.get()->getValue.get();
        f64 value = jst_timeline::invoke<f64>(fn, fv, t * seconds);
        v->mValue = static_cast<f32>(value);
    }
    u32 output = gabi::ea(v->pOutput_.get());
    if (!output)
        return;
    u32 out = gabi::at<jst_timeline::TOutput>(output)->__vtbl.get()->output.get();
    if (out == kSoundBeginFade || out == kSoundEndFade || out == kParticleBeginFade || out == kParticleEndFade)
        return;
    f32 value = f32(v->mValue);
    jst_timeline::invoke<void>(out, output, value, adaptor);
}

/* the evaluate-only pass over a control's objects at age + offset (offset in frames, -1 < offset <= 0) */
inline void evaluate(u32 controlAddr, f64 offset) {
    using namespace jst_timeline;
    TControl* control = gabi::at<TControl>(controlAddr);
    f64 seconds = f64(control->mSecondPerFrame);
    u32 end = gabi::ea(&control->mObjectContainer);
    u32 node = gabi::ea(control->mObjectContainer.next.get());
    while (node != end) {
        u32 objectAddr = node - 8;
        node = gabi::ea(gabi::at<TLinkListNode>(node)->next.get());
        TObject* object = gabi::at<TObject>(objectAddr);
        u32 status = u32(object->mStatus);
        if ((status != stb::TObject::STATUS_WAIT && status != stb::TObject::STATUS_SUSPEND) || !u8(object->bSequence_))
            continue;
        u32 adaptorAddr = gabi::ea(object->mpAdaptor.get());
        if (!adaptorAddr)
            continue;
        TAdaptor* adaptor = gabi::at<TAdaptor>(adaptorAddr);
        u32 value = gabi::ea(adaptor->mVariableValues.get());
        u32 count = u32(adaptor->mCount);
        for (u32 i = 0; i < count; i++)
            evaluate_value(gabi::at<TVariableValue>(value + i * sizeof(TVariableValue)), adaptor, seconds, offset);
        u32 fn = adaptor->__vtbl.get()->adaptor_do_update.get();
        invoke<void>(fn, adaptorAddr, objectAddr, u32(0));
    }
}

inline bool demo_running() {
    u32 curFile = *gabi::at<be<u32>>(kDemoManager + 0x20);
    s32 mode = *gabi::at<be<s32>>(kDemoManager + 0x2C);
    return curFile != 0 && mode == 1;
}
inline u32 demo_control() { return *gabi::at<be<u32>>(kDemoManager + 4); }
inline u32 frame_tag() { return *gabi::at<be<u32>>(kFrameCounter); }

/* the pass applies: N > 1 with the frame-level work on the first step; the port's switch WWHD_T60_JST (default 1) */
#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" {
__attribute__((visibility("default"))) extern int wwhd_t60_jst;
}
inline bool active() { return t60::steps() > 1 && t60_scene_first() && wwhd_t60_jst != 0; }
#else
inline bool active() { return false; }
#endif

/* end of dDemo_update, after forward(1) advanced the timeline (step 1 of the frame: offset 1/N - 1) */
inline void after_forward() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    if (!active() || !t60::first_step() || !demo_running())
        return;
    u32 control = demo_control();
    if (!control)
        return;
    wwhd_t60_latch_set(kDemoManager, frame_tag());
    evaluate(control, 1.0 / f64(t60::steps()) - 1.0);
#endif
}

/* start of step k (2..N) of a frame whose step 1 forwarded: offset k/N - 1 (0 on the tick30 step: the exact frame end) */
inline void step_begin(u32 k) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    if (!active() || t60::first_step() || k < 2 || k > t60::steps())
        return;
    if (!wwhd_t60_latch_pending(kDemoManager, frame_tag()) || !demo_running())
        return;
    u32 control = demo_control();
    if (!control)
        return;
    f64 n = f64(t60::steps());
    evaluate(control, k == t60::steps() ? 0.0 : f64(k) / n - 1.0);
#else
    (void)k;
#endif
}
}  // namespace jst_t60
