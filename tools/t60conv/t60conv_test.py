#!/usr/bin/env python3
"""Self-test of t60conv.py's rule forms on synthetic actor sources (2026-11-18).

Cases (the findings on dai_item / demo_item / ib and the 30 Hz visual-motion rule (14b(2))):
- the whole-function first-step gate (14d / 14f) is not applied when the function reaches per-step work through an
  `ext` glue name of a step primitive (ib: playAnimation = mDoExt_baseAnm::play 025E742C, movePosition =
  fopAcM_posMoveF 025D6870; dai_item: standItemAddCalc = cLib_addCalc 0200ECD4) or through an unresolved guest call_ptr
  (dai_item invokeMember); the same function without them is gated (control);
- a landing count (spc_item01: `if (mAcch.ChkGroundLanding()) mBounceCount = mBounceCount + 1;`) is a per-event
  counter: unchanged; a decrement under the same test stays a timer;
- a gated function with an AT collider registration (hitobj) is gated to the tick30 step (12c), not the first;
- a sound start under a local glue name (an address other units name *seStart*) gets the sound forms and t60calls;
- c_lib eases run real by default in T-02; `# known-fail clib-real:` turns a red T-02 into KNOWN FAIL;
- the shared-world-state rule (14s): a gated function touching switches / event bits / save flags goes to the tick30 step; the same logic on
  own state keeps the first step (control);
- the 30 Hz visual-motion rule (14b(2)): a member-pointer procedure dispatch is wrapped in t60c::procBegin / procEnd; a setter that runs the
  new body itself goes to review;
- a counter stepped through a local on the first step: a later plain `if` end test on the local is gated to the first
  step too; one with an else branch goes to review (d_a_arrow mBtkFrame);
- the findings: (a) a sound start inside a one-shot latch block (`if (!mbPlayedSe) { se;
  mbPlayedSe = 1; }`, ib) stays unchanged (a tick30 gate lost it); a block that does not set its flag keeps the gate;
  (b) a loop flag after a per-step play used only by a sound test (ib animControl) is gathered with t60c::mark and the
  test runs on the tick30 step; a loop flag with other uses stays review; (c) an emitter set behind a null-guard return
  (demo_item setParticle) gets a first-step return after the guard; without the guard it is unchanged;
- every t60c::mark (site, key) has a t60c::unmark of the same site and key under a first-step gate (mark_lint;
  the T-02 frame-tag finding: the harness keeps one frame tag across inputs, race_item's water-sound mark);
- t60conv.h's procBegin / procEnd semantics on a stub runtime (clang++): a procedure set inside the dispatch on step 1
  of 2 holds step 2 and runs the new body on the next frame's first step; one set before the dispatch on step 2 of 4
  runs on the tick30 step only; at N = 1 nothing is held;
- the whole form's `t60calls` scan follows member-pointer / table dispatch ((this->*TABLE[mMode])(), (this->*mProc)()
  with mProc = &Class::fn) and `ext` glue names (s25h, d_a_obj_correct); an unnamed mode is not counted.
Sources go to $T60_TEST_TMP (default the scratch RAM disk) and are removed.
"""
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import t60conv as t  # noqa: E402

HEAD = '#include "d/actor/d_a_x.h"\n\n'
EXEC = '''
/* 0x02100300 */
BOOL daX_c::execute() {
%s
    return TRUE;
}

/* 0x02100400 */
static BOOL daX_Execute(daX_c* i_this) {
    return i_this->execute();
}
'''


def unit(tmp, name, body, calls, tail=''):
    p = os.path.join(tmp, name + '.cpp')
    open(p, 'w').write(HEAD + body + EXEC % '\n'.join('    %s' % c for c in calls) + tail)
    c = t.Converter(name, p)
    c.tols = []
    c.run()
    return c


def changed(c):
    return [l.strip() for i, l in enumerate(c.new) if l != c.raw[i]]


TIMERFN = '''/* 0x02100200 */
void daX_c::tick() {
    if (mWaitTimer != 0) {
        mWaitTimer--;
    }
%s}
'''


def main():
    base = os.environ.get('T60_TEST_TMP', '')
    tmp = tempfile.mkdtemp(prefix='t60conv_test_', dir=base if os.path.isdir(base) else None)
    fails = []

    def check(name, ok, detail=''):
        print(('ok   ' if ok else 'FAIL ') + name)
        if not ok:
            fails.append('%s\n%s' % (name, detail))
    try:
        # gate: control (timer only) gated; ext step primitives / unresolved call_ptr not gated
        c = unit(tmp, 'g0', TIMERFN % '', ['tick();'])
        check('timer-only function gated on the first step (control)', [g.short for g in c.gated] == ['tick'], changed(c))
        for nm, line, ext in (('ib playAnimation (mDoExt_baseAnm::play 025E742C)', '    playAnimation(&mBck);\n', 'ext 025E742C void playAnimation(void*)\n'),
                              ('ib movePosition (fopAcM_posMoveF 025D6870)', '    movePosition(this, &mStts);\n', 'ext 025D6870 void movePosition(daX_c*,dCcD_Stts*)\n'),
                              ('dai_item standItemAddCalc (cLib_addCalc 0200ECD4)', '    standItemAddCalc(&mScale, 1.0f, 0.1f, 0.05f, 0.0f);\n', 'ext 0200ECD4 void standItemAddCalc(be<f32>*,f32,f32,f32,f32)\n'),
                              ('demo_item animPlay (daItemBase_c::animPlay 021837B0)', '    animPlay(1.0f, 1.0f, 1.0f, 1.0f, 1.0f);\n', 'ext 021837B0 void daItemBase_c::animPlay(f32, f32, f32, f32, f32)\n'),
                              ('dai_item unresolved guest call_ptr (invokeMember)', '    gabi::call_ptr(mMethod, this);\n', '')):
            c = unit(tmp, 'g%d' % len(fails), TIMERFN % line, ['tick();'], '\n' + ext)
            check('no whole gate with per-step work through ' + nm, not c.gated, changed(c))
        # landing count
        c = unit(tmp, 'land', '''/* 0x02100200 */
void daX_c::bounce() {
    if (mAcch.ChkGroundLanding()) {
        mBounceCount = mBounceCount + 1;
        speed.y = 10.0f;
    }
    if (mAcch.ChkGroundLanding()) {
        mLandTimer = mLandTimer - 1;
    }
}
''', ['bounce();'])
        rules = {(s.reason.split()[1], s.rule) for s in c.sites if s.cat == 'tmr'}
        check('landing count: per-event counter, unchanged', ('mBounceCount', '14d note (per-event counter: unchanged)') in rules and
              not any('mBounceCount' in l for l in changed(c)), sorted(rules))
        check('a decrement under the landing test stays a timer', any(n == 'mLandTimer' and r.startswith('14d') and 'note' not in r for n, r in rules), sorted(rules))
        # a first-step counter through a local and its end test (d_a_arrow mBtkFrame)
        c = unit(tmp, 'btk', '''/* 0x02100200 */
void daX_c::anim() {
    f32 frame = mBtkFrame;
    if (frame > 0.0f) {
        frame = frame + 1.0f;
        mBtkFrame = frame;
        if (!(frame < mFrameMax)) {
            mBtkFrame = 0.0f;
        }
    }
    s16 t = mWaitCnt;
    if (t > 0) {
        t = t - 1;
        mWaitCnt = t;
        if (t == 0) {
            mMode = 1;
        } else {
            mMode = 2;
        }
    }
    playAnimation(this);
}
''', ['anim();'], '\next 025E742C void playAnimation(daX_c*)\n')
        ch = changed(c)
        check('first-step counter through a local: its end test on the first step too (14d)',
              any('t60c::first() && (!(frame < mFrameMax))' in l for l in ch), ch)
        check('an end test with an else branch: placed by the timer-decision rule (with its else) or left to review',
              any(x.cls == 'review' and 'mWaitCnt' in x.reason for x in c.sites) or
              (any('timer decision t == 0' in x.reason for x in c.sites) and
               any('else of the timer decision' in x.reason for x in c.sites)), ch)
        # (2026-11-20): c_lib eases real by default; the known-fail note keeps a red spec visible
        check('c_lib eases found by name, glue line and address (default real)',
              t.clib_called('cLib_chaseF(&x, 1, 2);\next 0200ED84 void myAddCalc2(f32*,f32,f32,f32)\ngabi::call(0x0200F428, a);') ==
              {'0200F5C8', '0200ED84', '0200F428'}, t.clib_called(''))
        cl = t.clib_lines('include x.txt\n', ['0200F5C8'])
        check('the default form runs the converted ease: c_lib.cpp src + real + t60cand (a plain real is the original)',
              'src wwhd_src/SSystem/SComponent/c_lib.cpp' in cl and 'real 0200F5C8' in cl and 't60cand 0200F5C8' in cl, cl)
        check('c_lib.cpp src not repeated when the spec has it',
              'src wwhd_src' not in t.clib_lines('src wwhd_src/SSystem/SComponent/c_lib.cpp\n', ['0200F5C8']), '')
        check('no ease: no real line (control)', t.clib_called('mFoo = 1; cM_rndF(1.0f);') == set(), '')
        check('an ease defined by `ext` glue is a clash (c_lib.cpp would not link: duplicate symbols)',
              t.clib_ext_glue('ext 0200ECD4 f32 cLib_addCalc(be<f32>* v, f32 t, f32 s, f32 m, f32 n)\n') == ['cLib_addCalc'], '')
        check('a plain call is no clash (control)', t.clib_ext_glue('cLib_addCalc(&x, 1, 2, 3, 4);') == [], '')
        fake_rep = dict(sites=[], functions={}, step_addrs=['02000000'], changed_exec=False)
        bad = {'02000000': dict(verdict='t60 N=2: byte r3+0x10 one step 01, 2 steps 02')}
        res_kf = dict(t01={1: {'02000000': dict(verdict='ok')}}, t02={'s1_n2': bad}, nc={}, known_fail='M9-TEST')
        v = t.verdict(fake_rep, res_kf) if hasattr(t, 'verdict') else ''
        check('a T-02 failure with the known-fail note reads KNOWN FAIL', 'KNOWN FAIL clib-real (M9-TEST)' in v, v)
        res_kf['known_fail'] = None
        check('the same failure without the note stays FAIL T-02 (control)', t.verdict(fake_rep, res_kf).startswith('FAIL T-02'),
              t.verdict(fake_rep, res_kf))
        # AT registration behind the gate: tick30
        c = unit(tmp, 'at', '''/* 0x02100200 */
void daX_c::attack() {
    if (mAtTimer != 0) {
        mAtTimer--;
    }
    mSph.OnAtSetBit();
    dComIfG_Ccsp()->Set(&mSph);
}
''', ['attack();'])
        ch = changed(c)
        check('AT registration behind the whole gate on the tick30 step (12c)', any('if (!t60c::end()) return;' in l for l in ch) and
              not any('t60c::first()) return' in l for l in ch), ch)
        # (wfP1, obj_light): a sound start under a local glue name (ObjX_sound = 025E19CC, a
        # mDoAud_seStart by other units' names) on a per-step path: the per-frame start goes to the tick30 step; the effect
        # call list (t60calls) carries its address
        src_snd = '''/* 0x02100200 */
void daX_c::spin() {
    mAngle = mAngle + 64;
    ObjX_sound(0x303B, &current.pos);
}
'''
        c = unit(tmp, 'gsnd', src_snd, ['spin();'], '\next 025E19CC void ObjX_sound(u32,cXyz*)\n')
        ch = changed(c)
        check('a glue-named sound start (025E19CC) on a per-step path: once per frame on the tick30 step',
              any('t60c::end()' in l and 'ObjX_sound' in l for l in ch) or
              any('t60c::end()' in l for l in ch) and any('ObjX_sound' in l for l in ch), ch)
        addrs, _unres = t.effect_targets(src_snd + '\next 025E19CC void ObjX_sound(u32,cXyz*)\n')
        norm = {int(x, 16) if isinstance(x, str) else x for x in addrs}
        check('t60calls carries the glue-named sound start', 0x025E19CC in norm, sorted(str(x) for x in addrs))
        # the shared-world-state rule (14s) (kytag04): frame-level logic on a switch: tick30 step; the same logic without the
        # switch keeps the first step (control)
        c = unit(tmp, 'sw', '''/* 0x02100200 */
void daX_c::swcheck() {
    if (dComIfGs_isSwitch(mSwitchNo, mRoomNo)) {
        if (mTimer >= 5) {
            mState = 1;
        } else {
            mTimer = mTimer + 1;
        }
    }
}
''', ['swcheck();'])
        ch = changed(c)
        check('switch read behind the whole gate on the tick30 step (14s)', any('if (!t60c::end()) return;' in l and '14s' in l for l in ch) and
              not any('t60c::first()) return' in l for l in ch), ch)
        c = unit(tmp, 'nosw', '''/* 0x02100200 */
void daX_c::swcheck() {
    if (mArmed) {
        if (mTimer >= 5) {
            mState = 1;
        } else {
            mTimer = mTimer + 1;
        }
    }
}
''', ['swcheck();'])
        ch = changed(c)
        check('the same logic on own state keeps the first step (14s control)', any('if (!t60c::first()) return;' in l for l in ch) and
              not any('14s' in l for l in ch), ch)
        # findings (a) / (b) / (c); padded past the sound-wrapper size, a per-step play keeps
        # the whole first-step gate off
        PAD = ''.join('    mColor%d = %d;\n' % (i, i) for i in range(8))
        PLAY = '\next 025E742C void playAnimation(void*)\n'
        for nm, sets, want in (('latch block that sets its flag: start unchanged (ib mbPlayedSe)', '        mbPlayedSe = 1;\n', False),
                               ('a block that does not set its flag: tick30 gate kept (control)', '        mOther = 1;\n', True)):
            c = unit(tmp, 'latch%d' % want, '''/* 0x02100200 */
void daX_c::once() {
%s    playAnimation(&mBrk);
    if (!mbPlayedSe) {
        mDoAud_seStart(0x5802, &eyePos, 0, 0);
%s    }
}
''' % (PAD, sets), ['once();'], PLAY)
            ch = changed(c)
            check(nm, any('t60c::end()) mDoAud_seStart(0x5802' in l for l in ch) == want and not c.gated, ch)
        LOOP = '''/* 0x02100200 */
void daX_c::anim() {
%s    playAnimation(&mBrk);
    bool looped = mBrk.mFrameCtrl.checkState(J3DFrameCtrl::STATE_LOOP_E);
    if (mMode == 1) {
        f32 frame = mBrk.getFrame();
        f32 previous = frame - (f32)mBrk.mFrameCtrl.mRate;
        if (looped || (!(frame < 45) && previous < 45))
            mDoAud_seStart(0x580E, &eyePos, 0, 0);
    }
%s}
'''
        c = unit(tmp, 'loop', LOOP % (PAD, ''), ['anim();'], PLAY)
        ch = changed(c)
        check('loop flag gathered with t60c::mark, the sound test on the tick30 step (ib animControl)',
              any('if (t60c::first()) t60c::unmark(' in l and 'if (looped) t60c::mark(' in l for l in ch) and
              any(l.startswith('if (t60c::end() && (looped ||') and 't60c::marked(' in l for l in ch) and
              not any('t60c::end()) mDoAud_seStart' in l for l in ch) and not any(x.cls == 'review' for x in c.sites), ch)
        c = unit(tmp, 'loop2', LOOP % (PAD, '    if (looped) {\n        mMode = 2;\n    }\n'), ['anim();'], PLAY)
        check('a loop flag with a non-sound use: review (control)', any(x.cls == 'review' and x.cat == 'anm' for x in c.sites) and
              not any('t60c::mark(' in l for l in changed(c)), changed(c))
        EMIT = '''/* 0x02100200 */
void daX_c::setParticle() {
%s%s    if (chkArgFlag(2)) {
        return;
    }
    switch (mType) {
    case 0:
        mpEmitters[0] = dComIfGp_particle_set(0x81, &current.pos, angle);
        [[fallthrough]];
    case 1:
        mpEmitters[1] = dComIfGp_particle_set(0x82, &current.pos, angle);
        break;
    }
}
'''
        GUARD = '    if (mpEmitters[0] || mpEmitters[1]) {\n        return;\n    }\n'
        c = unit(tmp, 'emit', EMIT % (PAD, GUARD), ['setParticle();'])
        ch = changed(c)
        check('emitter set behind a null-guard return: first-step return after the guard (demo_item setParticle)',
              len(ch) == 1 and ch[0].startswith('} if (!t60c::first()) return;'), ch)
        c = unit(tmp, 'emit2', EMIT % (PAD, ''), ['setParticle();'])
        check('the same sets without the guard: unchanged (control)', not any('t60c::first()) return' in l for l in changed(c)), changed(c))
        # the 30 Hz visual-motion rule (14b(2))
        PROCS = '''/* 0x02100000 */
void daX_c::procWait() {
    if (mTimer != 0) {
        mTimer--;
    }
    if (mTimer == 0) {
        mProc = &daX_c::procMove;
    }
}

/* 0x02100100 */
void daX_c::procMove() {
    speedF = 3.0f;
    fopAcM_posMoveF(this, 0);
}
'''
        c = unit(tmp, 'proc', PROCS, ['(this->*mProc)();'])
        ch = changed(c)
        check('member-pointer procedure dispatch wrapped in procBegin / procEnd (14b(2))',
              any('t60c::procBegin(' in l and '(this->*mProc)();' in l and 't60c::procEnd(' in l and '&(mProc)' in l for l in ch), ch)
        c = unit(tmp, 'procfall', PROCS + '''
/* 0x02100150 */
void daX_c::setMove() {
    mProc = &daX_c::procMove;
    (this->*mProc)();
}
''', ['setMove();', '(this->*mProc)();'])
        check('a setter that runs the new body itself: review (14b(2))',
              any(s.cat == 'proc' and s.cls == 'review' for s in c.sites) and not any('procBegin' in l for l in changed(c)),
              [(s.cat, s.cls, s.reason) for s in c.sites])
        # procBegin / procEnd semantics on a stub runtime
        cxx = shutil.which('clang++') or shutil.which('c++')
        if not cxx:
            check('t60conv.h procBegin / procEnd semantics (no C++ compiler)', False)
        else:
            inc = os.path.join(tmp, 'inc')
            os.makedirs(inc)
            open(os.path.join(inc, 'true60.h'), 'w').write('''#pragma once
#include <stdint.h>
typedef uint32_t u32; typedef int32_t s32; typedef uint64_t u64; typedef float f32; typedef uint8_t u8; typedef int16_t s16;
template <class T> struct be { T v; operator T() const { return v; } };
namespace t60 { extern int g_n, g_k; inline float dt() { return 1.0f / g_n; } inline bool first_step() { return g_k == 0; }
inline bool tick30() { return g_k == g_n - 1; } inline int steps() { return g_n; } }
namespace gabi { extern u32 g_tag; template <class T> T* at(u32) { return (T*)&g_tag; } inline u32 ea(const void* p) { return (u32)(uintptr_t)p; }
inline float fmuls_ppc(float a, float b) { return a * b; }
template <class F> struct ptmf { be<s16> d; be<s16> i; be<u32> f; }; }
inline float t60_ratio(float r, float dt) { return r * dt; }
''')
            prog = os.path.join(tmp, 'proc.cpp')
            open(prog, 'w').write('''#include <stdio.h>
#include "%s"
int t60::g_n = 1, t60::g_k = 0; u32 gabi::g_tag = 0;
struct X { int proc = 0, bodies[3] = {0, 0, 0}; };
/* frames of N steps; the body of proc p runs bodies[p]++; set_in(frame, step, p): the body sets p (inside the
   dispatch); set_out(frame, step, p): an earlier call of the execute sets p before the dispatch */
static void run(X& x, int n, int frames, int sf, int ss, int sp, bool inside, int* log) {
    t60::g_n = n;
    for (int f = 0; f < frames; f++) {
        gabi::g_tag = 0x1000 + f;
        for (int k = 0; k < n; k++) {
            t60::g_k = k;
            if (!inside && f == sf && k == ss) x.proc = sp;
            if (t60c::procBegin(1, &x.proc, t60c::procWord(x.proc))) {
                x.bodies[x.proc]++;
                log[f * 4 + k] = x.proc + 1;
                if (inside && f == sf && k == ss) x.proc = sp;
                t60c::procEnd(1, &x.proc, t60c::procWord(x.proc));
            }
        }
    }
}
int main() {
    int log[64];
    for (int i = 0; i < 64; i++) log[i] = 0;
    X a; run(a, 2, 3, 1, 0, 1, true, log);
    /* frame 1 step 0: body 0 runs and sets 1; step 1 holds; frame 2: body 1 on both steps */
    int ok1 = log[4] == 1 && log[5] == 0 && log[8] == 2 && log[9] == 2;
    for (int i = 0; i < 64; i++) log[i] = 0;
    X b; run(b, 4, 2, 0, 1, 2, false, log);
    /* frame 0: set before the dispatch on step 1 of 4: steps 1, 2 hold, the new body on step 3 (tick30); frame 1: all */
    int ok2 = log[0] == 1 && log[1] == 0 && log[2] == 0 && log[3] == 3 && log[4] == 3 && log[7] == 3;
    for (int i = 0; i < 64; i++) log[i] = 0;
    X c; run(c, 1, 3, 1, 0, 1, true, log);
    int ok3 = log[0] == 1 && log[4] == 1 && log[8] == 2;
    /* a guest member pointer's word through its fields: another target, another word */
    gabi::ptmf<int> pm; pm.d.v = 0; pm.i.v = -1; pm.f.v = 0x02054040u;
    const u32 w1 = t60c::procWord(pm);
    pm.f.v = 0x02055450u;
    int ok4 = w1 != t60c::procWord(pm);
    printf("%%d %%d %%d %%d\\n", ok1, ok2, ok3, ok4);
    return ok1 && ok2 && ok3 && ok4 ? 0 : 1;
}
''' % os.path.join(t.ROOT, 'wwhd_src', 'include', 'd', 't60conv.h'))
            exe = os.path.join(tmp, 'proc')
            r = subprocess.run([cxx, '-std=c++17', '-I', inc, '-o', exe, prog], capture_output=True, text=True)
            if r.returncode:
                check('t60conv.h procBegin / procEnd compile on the stub runtime', False, r.stderr[-2000:])
            else:
                r = subprocess.run([exe], capture_output=True, text=True)
                check('procBegin / procEnd: set inside the dispatch holds the frame (N 2), set before runs on tick30 (N 4), N 1 unheld (%s)' % r.stdout.strip(),
                      r.returncode == 0, r.stdout)
        # (d_a_esa): an add under a collision hit is an event; a state-dispatch block's phase add is per
        # frame; a chance comparison in a block that deletes the actor is one draw per event
        c = unit(tmp, 'esa', '''/* 0x02100200 */
void daX_c::move() {
    if (cBgS_LineCross(dComIfG_Bgsp(), &mLinChk)) {
        current.angle.y -= 0x8000;
        speedF = speedF * 0.5f;
    }
    if (mAct == 3) {
        s16 bob = (s16)(mBobAngle + 3000);
        mBobAngle = bob;
        if (mWaitTimer == 0) {
            mAct = 4;
        }
        if (mTaken == 0x23) {
            if (cM_rndF(1.0f) < 0.5f) {
                fopKyM_createWpillar(&current.pos, 0.3f, 0.3f, 0);
            }
            fopAcM_delete(this);
        }
    }
}
''', ['move();'])
        ch = changed(c)
        check('esa: the wall-hit reflection (an add under cBgS_LineCross) is an event, unchanged',
              not any('0x8000' in l for l in ch), ch)
        check('esa: the bob phase add in a state-dispatch block (its changes nested) gets the share',
              any('mBobAngle + t60c::share(3000)' in l for l in ch), ch)
        check('esa: the chance draw before a delete stays per event (no first-step gate)',
              not any('cM_rndF(1.0f)' in l and 't60c::first()' in l for l in ch), ch)
        # t60c::mark users must clear the mark on the first step (the T-02 frame-tag finding, race_item)
        ML = lambda body: t.mark_lint([('x.cpp', body)])
        ok_inline = '''void a() {
    if (t60c::first()) /* the frame's mark starts clear */
        t60c::unmark(3, this);
    if (hit && !t60c::marked(3, this)) {
        t60c::mark(3, this);
    }
}'''
        check('mark: unmark under `if (t60c::first())` before the read: ok', ML(ok_inline) == [], ML(ok_inline))
        ok_block = '''void a() {
    if (t60c::on() && t60::first_step()) {
        x = 0;
        t60c::unmark(2, &mPnt);
    }
    if (!t60c::marked(2, &mPnt) && cLib_chasePos(&pos, &tgt, s))
        t60c::mark(2, & mPnt);
}'''
        check('mark: unmark inside an `if (... first_step()) {` block, key spacing ignored: ok', ML(ok_block) == [], ML(ok_block))
        bad_none = ok_inline.replace('    if (t60c::first()) /* the frame\'s mark starts clear */\n        t60c::unmark(3, this);\n', '')
        check('mark: race_item before the fix (no unmark): flagged', len(ML(bad_none)) == 1 and 'no t60c::unmark(3, this)' in ML(bad_none)[0], ML(bad_none))
        bad_else = '''void a() {
    if (t60c::first())
        y = 1;
    else
        t60c::unmark(1, this);
    if (looped) t60c::mark(1, this);
}'''
        check('mark: unmark in an else branch: flagged', len(ML(bad_else)) == 2, ML(bad_else))
        bad_plain = '''void a() {
    t60c::unmark(1, this);
    if (looped) t60c::mark(1, this);
}'''
        check('mark: an unconditional unmark (every step): flagged', len(ML(bad_plain)) == 2, ML(bad_plain))
        bad_key = '''void a() {
    if (t60c::first())
        t60c::unmark(1, this);
    if (looped) t60c::mark(1, &mOther);
}'''
        check('mark: unmark of another key: flagged', len(ML(bad_key)) == 1, ML(bad_key))
        bad_or = '''void a() {
    if (t60c::first() || reset)
        t60c::unmark(1, this);
    if (looped) t60c::mark(1, this);
}'''
        check('mark: a gate that also opens on later steps (||): flagged', len(ML(bad_or)) == 2, ML(bad_or))
        check('mark: a commented-out mark is ignored', ML('// t60c::mark(1, this);\n/* t60c::mark(2, this); */') == [])
        hdr = ('h.h', '''static inline void stop(const void* k) { t60c::mark(2, k); }
static inline bool stopped(const void* k) {
    if (t60c::on() && t60::first_step())
        t60c::unmark(2, k);
    return t60c::marked(2, k);
}''')
        check('mark: esa header form (mark / first-step unmark in helpers): ok', t.mark_lint([hdr]) == [], t.mark_lint([hdr]))
        # whole form (2026-11-18, d_a_npc_people): every call of the whole-form execute but its picture is counted
        # (`t60calls`), resolved through local helpers, build/names.tsv (Class::name / free name) and include wrappers
        wsrc = os.path.join(tmp, 'w.cpp')
        open(wsrc, 'w').write('''#include "d/actor/x_t60.h"
static inline void mover(daNpcPeople_c* a) { fopAcM_posMoveF(a, 0); }
/* 022C305C */
BOOL daNpcPeople_c::_execute() {
    if (t60w::begin(this, people_t60::kOps)) /* the picture only */
        return TRUE;
    chkAttention();
    mover(this);
    setMtx();
    t60w::end(this, people_t60::kOps, true);
    return TRUE;
}
/* 022C3000 */
BOOL daNpcPeople_c::other() {
    lookBack();
    return TRUE;
}
''')
        whdr = '''namespace people_t60 {
static inline void mtx(daNpcPeople_c* a) { a->setMtx(); }
static const t60w::Ops<daNpcPeople_c> kOps = {4, 3, get, set, mtx, held};
}'''
        W = t.whole_calls([wsrc], whdr)
        check('whole: Ops form: one whole-form function (the other is not)', [x[0] for x in W] == ['daNpcPeople_c::_execute'], W)
        if W:
            fn, ad, pic = W[0]
            check('whole: Ops form: chkAttention (Class::name) and fopAcM_posMoveF (through a local helper) counted',
                  '022C0084' in ad and '025D6870' in ad, W)
            check('whole: Ops form: the picture (kOps mtx -> setMtx) left out, the form\'s own t60w calls not resolved',
                  '022BF3A0' not in ad and pic == ['022BF3A0'] and '022C2E50' not in ad, W)
        open(wsrc, 'w').write('''static inline void pic(daNpcPeople_c* a) { a->setMtx(); }
/* 022C305C */
BOOL daNpcPeople_c::_execute() {
    x_t60::Guard<daNpcPeople_c> g(this, pic);
    if (g.skip)
        return TRUE;
    eventOrder();
    setMtx();
    return TRUE;
}
''')
        W = t.whole_calls([wsrc], '')
        check('whole: Guard form with a picture function: eventOrder counted, the picture\'s setMtx left out',
              len(W) == 1 and W[0][1] == ['022C2A04'] and W[0][2] == ['022BF3A0'], W)
        open(wsrc, 'w').write('''/* 022C305C */
BOOL daNpcPeople_c::_execute() {
    eventOrder();
    return TRUE;
}
''')
        check('whole: no whole form: nothing', t.whole_calls([wsrc], '') == [], t.whole_calls([wsrc], ''))
        # mixed form (2026-11-19, d_a_nz): the Guard opens under a condition (only frames starting in some
        # states run whole): whole_mixed names it; a top-level Guard / begin is no mixed form. ensure_whole_calls writes
        # the line as `t60wcalls` when the spec has a `t60wsel` selector (verify harness: compared on those inputs only),
        # `t60calls` otherwise, and refreshes either form; whole_check asks for the selector before running
        open(wsrc, 'w').write('''/* 0230BEF4 */
static BOOL daNZ_Execute(nz_class* i_this) {
    if (nz_t60_whole_frame(i_this)) {
        t60w::Guard<nz_class> g(i_this, pic);
        if (g.skip)
            return TRUE;
        eventOrder();
        return TRUE;
    }
    eventOrder();
    return TRUE;
}
''')
        Wm = t.whole_mixed([wsrc])
        check('whole mixed: a Guard under a condition is a mixed form', [x[0] for x in Wm] == ['daNZ_Execute'] and
              'Guard' in Wm[0][1], Wm)
        open(wsrc, 'w').write('''/* 022C305C */
BOOL daNpcPeople_c::_execute() {
    x_t60::Guard<daNpcPeople_c> g(this, pic);
    if (g.skip) {
        return TRUE;
    }
    eventOrder();
    return TRUE;
}
''')
        check('whole mixed: a top-level Guard is not', t.whole_mixed([wsrc]) == [], t.whole_mixed([wsrc]))
        sv = (t.ROOT, t.UNITS)
        try:
            t.ROOT, t.UNITS = tmp, os.path.join(tmp, 'units')
            os.makedirs(os.path.join(t.UNITS, '_t60'), exist_ok=True)
            open(os.path.join(t.UNITS, 'wm.txt'), 'w').write('src w.cpp\n')
            sp = os.path.join(t.UNITS, '_t60', 'wm.txt')
            open(sp, 'w').write('include wm.txt\nt60conv 022C305C\nt60calls 025A847C\n')
            t.ensure_whole_calls('wm')
            t1 = open(sp).read()
            check('whole mixed: no t60wsel: the whole line is t60calls (effect line kept)',
                  t.WHOLE_TAG + 't60calls 022C2A04\n' in t1 and 't60calls 025A847C' in t1 and 't60wcalls' not in t1, t1)
            open(sp, 'a').write('t60wsel 022C305C r3+0x2B1 u8 3,4\n')
            t.ensure_whole_calls('wm')
            t2 = open(sp).read()
            check('whole mixed: with t60wsel: the tool line becomes t60wcalls (refreshed, not doubled)',
                  t.WHOLE_TAG_MIXED + 't60wcalls 022C2A04\n' in t2 and t.WHOLE_TAG not in t2 and t2.count('022C2A04') == 1, t2)
            t.ensure_whole_calls('wm')
            check('whole mixed: a second run keeps one line', open(sp).read() == t2, open(sp).read())
        finally:
            t.ROOT, t.UNITS = sv
        # whole form through member-pointer / table dispatch (s25h 2026-11-18, d_a_obj_correct's mode_proc / demo_proc):
        # the targets of `(this->*TABLE[mMode])()` (a static table of &Class::fn) and of `(this->*mProc)()` (a member
        # pointer assigned &Class::fn) are followed like direct calls, with `ext` glue names resolved to their address;
        # a mode no table or assignment names is not counted
        open(wsrc, 'w').write('''typedef void (daNpcPeople_c::*ModeFn)();
/* 022C3300 */
void daNpcPeople_c::modeWait() {
    chkAttention();
}
/* 022C3340 */
void daNpcPeople_c::modeTalk() {
    switchOn(this, 3);
}
/* 022C3380 */
void daNpcPeople_c::procRun() {
    eventOrder();
}
/* 022C33C0 */
void daNpcPeople_c::modeUnused() {
    lookBack();
}
/* 022C3400 */
void daNpcPeople_c::procInit() {
    mProc = &daNpcPeople_c::procRun;
}
static ModeFn mode_tbl[] = {&daNpcPeople_c::modeWait, &daNpcPeople_c::modeTalk};
/* 022C305C */
BOOL daNpcPeople_c::_execute() {
    x_t60::Guard<daNpcPeople_c> g(this, x_t60::kOps);
    if (g.skip)
        return TRUE;
    (this->*mode_tbl[mMode])();
    (this->*mProc)();
    return TRUE;
}

/* WWHD glue
ext 025B9E38 void switchOn(daNpcPeople_c*, s32)
*/
''')
        W = t.whole_calls([wsrc], '')
        ad = W[0][1] if len(W) == 1 else []
        check('whole: table dispatch (this->*mode_tbl[mMode])(): both modes and their calls counted (chkAttention, ext switchOn 025B9E38)',
              {'022C3300', '022C3340', '022C0084', '025B9E38'} <= set(ad), W)
        check('whole: member pointer (this->*mProc)() assigned &procRun elsewhere: procRun and its eventOrder counted',
              {'022C3380', '022C2A04'} <= set(ad), W)
        check('whole: a mode no table / assignment names (modeUnused, lookBack) and the setter procInit not counted',
              not ({'022C33C0', '022C3400'} & set(ad)) and not any(a for a in ad if a not in
                                                               {'022C3300', '022C3340', '022C0084', '025B9E38', '022C3380', '022C2A04'}), W)
        # draw path writes (2026-11-18, Ym1's grass-cut flag): a field written by the draw or a unit function it
        # calls is listed as state; locals (one-line and multi-line declarations, pointer locals not written through),
        # a gabi::Local, render / debug-guard names are not state; the execute's writes are not draw writes
        open(wsrc, 'w').write('''static u8 l_debugGuard;
/* 022C3100 */
static void helper(daNpcPeople_c* a) {
    a->mHelperFlag = 2;
}
/* 022C3200 */
BOOL daNpcPeople_c::_draw() {
    u32 morf = gabi::ea(this->mpMorf.get()), flower = 0,
        model = gabi::ea(view<X>(morf)->mpModel.get());
    J3DModel* pm = mpMorf->getModel();
    f32 frame;
    frame = mpMorf->getFrame();
    model = 3;
    gabi::Local<cXyz> sp;
    sp->x = 0.0f;
    mKariFlag = 0;
    tevStr.mRoomNo = 1;
    if (!l_debugGuard) {
        l_debugGuard = 1;
    }
    helper(this);
    return TRUE;
}
/* 022C305C */
BOOL daNpcPeople_c::_execute() {
    mExecFlag = 1;
    return TRUE;
}
''')
        R = t.draw_rnd([wsrc])
        check('draw rnd: none on this draw path (no cM_rnd / settingTevStruct)', R == [], R)
        D = t.draw_writes([wsrc])
        st = sorted(tx.split(' =')[0] for _, _, k, tx in D if k == 'state')
        check('draw writes: mKariFlag and the called helper\'s a->mHelperFlag are state; locals, Local, render, guard and the execute not',
              st == ['a->mHelperFlag', 'mKariFlag'] and {tx.split(' =')[0] for _, _, k, tx in D if k == 'render'} == {'tevStr.mRoomNo', 'l_debugGuard'}, D)
        # draw path random draws (2026-11-18): settingTevStruct (-> plightcol_plus cM_rndF) in a called helper and a
        # direct cM_rndF in the draw are listed; the execute's cM_rnd is not
        open(wsrc, 'w').write('''/* 022C3100 */
static void light(daNpcPeople_c* a) {
    g_env_light.settingTevStruct(TEV_TYPE_ACTOR, &a->current.pos, &a->tevStr);
}
/* 022C3200 */
BOOL daNpcPeople_c::_draw() {
    light(this);
    f32 r = cM_rndF(1.0f);
    return TRUE;
}
/* 022C305C */
BOOL daNpcPeople_c::_execute() {
    f32 x = cM_rnd();
    return TRUE;
}
''')
        R = t.draw_rnd([wsrc])
        check('draw rnd: settingTevStruct via a helper and a direct cM_rndF listed, the execute\'s cM_rnd not',
              sorted((f.split('::')[-1], c) for f, _, c, _ in R) == [('_draw', 'cM_rndF'), ('light', 'settingTevStruct')], R)
        # negative control / baseline over every source of a two-file src line (the d_a_ks finding, 2026-11-18)
        root, units = t.ROOT, t.UNITS
        try:
            t.ROOT = os.path.join(tmp, 'ncroot')
            t.UNITS = os.path.join(t.ROOT, 'tools', 'verify', 'units')
            os.makedirs(os.path.join(t.UNITS, '_t60'))
            os.makedirs(os.path.join(t.ROOT, 'wwhd_src', 'd', 'actor'))
            for f in ('d_a_zz.cpp', 'd_a_zz_exec.cpp'):
                open(os.path.join(t.ROOT, 'wwhd_src', 'd', 'actor', f), 'w').write('int %s;\n' % f[:-4])
            open(os.path.join(t.UNITS, 'd_a_zz.txt'), 'w').write(
                'src wwhd_src/d/actor/d_a_zz.cpp wwhd_src/d/actor/d_a_zz_exec.cpp # two files\nfunc 02000000\n')
            open(os.path.join(t.UNITS, '_t60', 'd_a_zz.txt'), 'w').write('include d_a_zz.txt\nt60conv 02000000\n')
            nm = t.negctrl_unit('d_a_zz', {'src': 'wwhd_src/d/actor/d_a_zz.cpp'})
            sp = open(os.path.join(t.UNITS, nm + '.txt')).read()
            srcl = [l for l in sp.split('\n') if l.startswith('src ')]
            nc_ok = srcl == ['src build/t60conv/d_a_zz.cpp build/t60conv/d_a_zz_exec.cpp # two files'] and all(
                open(os.path.join(t.ROOT, 'build', 't60conv', f)).read().startswith('#define T60C_NEGCTRL 1')
                for f in ('d_a_zz.cpp', 'd_a_zz_exec.cpp')) and 't60conv 02000000' in sp
            check('negative control: both sources of a two-file src line substituted and built with T60C_NEGCTRL', nc_ok, sp)
            try:
                t.subst_srcs('src a.cpp b.cpp\n', {'a.cpp': 'x/a.cpp'})
                loud = False
            except SystemExit:
                loud = True
            check('negative control: a src file without a substitute fails loudly', loud)
            try:
                t.subst_srcs('src a.cpp\n', {'a.cpp': 'x/a.cpp', 'c.cpp': 'x/c.cpp'})
                loud = False
            except SystemExit:
                loud = True
            check('negative control: a substitute for a file on no src line fails loudly', loud)
            # the unconverted baseline (base_unit) of the same two-file unit: both sources from git HEAD, never the working
            # copies (the d_a_ks: a real T-01 failure reported as PRE-EXISTING when the baseline built the converted
            # file); the working copies here carry a conversion that HEAD lacks
            g = ['git', '-C', t.ROOT, '-c', 'user.name=t', '-c', 'user.email=t@t']
            subprocess.run(g[:3] + ['init', '-q'], check=True)
            subprocess.run(g + ['add', '-A'], check=True)
            subprocess.run(g + ['commit', '-q', '-m', 'base'], check=True)
            for f in ('d_a_zz.cpp', 'd_a_zz_exec.cpp'):
                open(os.path.join(t.ROOT, 'wwhd_src', 'd', 'actor', f), 'a').write('/* (t60conv) converted */\n')
            bn = t.base_unit('d_a_zz', {'src': 'wwhd_src/d/actor/d_a_zz.cpp'})
            bsp = open(os.path.join(t.UNITS, bn + '.txt')).read()
            bsrc = [l for l in bsp.split('\n') if l.startswith('src ')]
            base_ok = bsrc == ['src build/t60conv/base_d_a_zz.cpp build/t60conv/base_d_a_zz_exec.cpp # two files'] and all(
                open(os.path.join(t.ROOT, 'build', 't60conv', 'base_' + f)).read() == 'int %s;\n' % f[:-4]
                for f in ('d_a_zz.cpp', 'd_a_zz_exec.cpp'))
            check('baseline: both sources of a two-file src line substituted, from git HEAD (not the converted copies)', base_ok, bsp)
            try:
                t.unit_src('d_a_zz')
                loud = False
            except SystemExit:
                loud = True
            check('unit_src: a two-file unit without --src fails loudly (no silent first file)', loud)
        finally:
            t.ROOT, t.UNITS = root, units
    finally:
        shutil.rmtree(tmp, ignore_errors=True)  # this test's own folder
    for f in fails:
        print('FAIL', f)
    print('t60conv_test: %s' % ('FAIL' if fails else 'PASS'))
    return 1 if fails else 0


if __name__ == '__main__':
    sys.exit(main())
