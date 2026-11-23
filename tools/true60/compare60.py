#!/usr/bin/env python3
"""compare60.py RUN_A RUN_B [END] [--tol NAME=VALUE ...] [--admp-spec FILE | NAME=FILE,...] [--admp-require-spec NAME,...|all] [--admp-exact NAME,...] [--admp-tevstr NAME:OFF,...] [--plaus-spec FILE] [--save-mask OFF:LEN,...]
           [--strict-until FRAME --cause "[side=link|enemy] rule=R [hit=SPEC] decision=TEXT"] [--keyed-proof PAIR ...] [--ship NAME]
           [--cloth FILE:NV:POS0:POS1:SPEED]

True60 T-03: the game state of a 30 Hz run (RUN_A, the reference) and an N-step run (RUN_B, true60 at 60 or 120 Hz
logic) of the same scripted scenario (pair60.sh), compared at full steps (the step that ends a 30 Hz frame) with
"equivalent" gates instead of the identity compare.py asks for (1.3 DoD 3: procedure timeline within
+-1 half step, same hits / damage / items / save data, no NaN, crash or new processes).

Run folders hold the files of the port's test aids (run_scenario.sh / gametest run.sh): log ([test] origin ... logic
step S), trace.txt (WWHD_LINK_TRACE), cam.txt (WWHD_CAM_TRACE), se.txt (WWHD_SE_TRACE), rng.txt (WWHD_RNG_TRACE),
digest.txt (WWHD_PROC_DIGEST), si.bin (+ si.bin.T for WWHD_SAVEINFO_AT), *.admp (WWHD_ACTOR_DUMP) and optionally
steps.txt (WWHD_T60_STEPLOG: one line per logic step `step full dt`).

Frames. Every record is mapped to a scenario frame (30 Hz frame index from the origin):
  - with steps.txt: the full steps at or after the origin step are frames 0, 1, 2, ...; a record of a half step belongs
    to the frame its next full step ends; records before the origin step belong to no frame (the boot and the state load
    differ between the runs);
  - without: frame = (step - origin) // K, K = the run's logic steps per 30 Hz frame: from --steps-a / --steps-b,
    else 1 / N, N taken from the run's trace.txt dt column (dt 1/N), else 1.
State channels (Link, camera, process digests, rng) keep the last record per frame (and process): the state at the full
step. Event channels (sound starts) keep every record, mapped to its frame.

Gates (each prints PASS / FAIL / SKIP; `equivalent: yes` when none fails; exit 0 / 1, 2 without an origin):
  mode        RUN_A runs at dt 1 only; RUN_B shows dt < 1 (the true60 mode was active; a preflight, not a cap)
  origin-clock the world clock (mTimer) at the scenario origin is the same in both runs (rule: a run a
              frame behind after async boot loads is refused with that reason; pair scripts use WWHD_TEST_SYNC_LOADS=1);
              SKIP when the logs have no origin clock line
  crane: the crane salvage sequence is driven by the unconverted event
              system (DEMO_SALVAGE); until converts events it is gated for plausibility only ("crane/salvage:
              plausibility until then strict"). From the first frame either run's ship is in MODE_CRANE_UP (11)
              (- proc_slack) to the end: the same procedure / ship-mode sequences with each change within proc_slack + D
              (D = 6i(ii)'s derived rope bound, ship_followon; 0 without a rope difference), sound starts and process
              lifetimes within their slack + D, no per-frame value checks (link-motion / link-tslack / camera / cam-tslack /
              ship xz, speed, angle: finite only; health covers NaN), no stuck state (B's Link position, ship position and
              rope not stopped at the window's end while A's still changes there: B's unchanged stretch at the end longer
              than A's + D + proc_slack with A's within D + proc_slack), save strict. Addendum (pcr10): a periodic
              sound in the window (the crane swing 0x381E) may differ in count by at most one, its instances otherwise
              keeping the same spacing within se_slack + D (the extra one first or last). Rule (b) (2026-11-16, after
              rule (c)): after a 6i(ii) B-later crane-up shift, rope-timed window starts (SHIP_ROPE_SE) still beyond
              se_slack + D are reported in `sound`, not failed, when their key plays in both runs' window with counts
              within one (a per-frame rope work sound: present in both) (until `--tol crane_rest=0` fails them). `--tol crane_plaus=0`: strict
  health      no crash / fatal / assert / halt in either log, no nan / inf in the traces
  link-procs  Link's procedure sequence is the same, each change within +-proc_slack frames (default 1)
  link-motion where both run the same procedure (and not within proc_slack of a change): position, speeds, angles and
              animation frame within pos / speed / angle / anim tolerances
  camera      eye, center and fovy within cam (skipped where Link's procedures differ)
  sound       every sound start (wrapper, id) of one run is matched by one of the other within +-se_slack frames
              (the time-slack gate of converted processes and Link); ambient world sounds timed by cM_rnd (AMBIENT
              below) are left out of it and checked by sound-amb
              Event-start world sound (frame 61; the 8a / 8c family): a start in
              one run only is accepted when it lies on an event-start frame (either run's camera enters an event style,
              EVENT_STYLES, on that frame), every caller of it is a world-sound caller (WORLD_SE_CALLERS: fpcM_Management,
              not a process), both runs start the same key on the frame before or after (a per-frame sound) and it is that
              run's only unmatched start of the key on the frame (`--tol ev_start_se=0` turns it off)
  sound-amb   the sound-gate rule (8a) (dt = 1/N): ambient sounds timed by the RNG (seagulls;
              the RNG diverges by design, the per-step chance rule (8)) are checked for plausibility only: the same ambient sound keys occur in
              both runs, per 10 s window (300 frames) B's count of each is within +-50 % of A's (at least the count
              noise of RNG-timed events, 2 sqrt(A + B): with few starts per window +-50 % is inside it), and none
              repeats on more consecutive frames than in A (at least 2: no start stuck on every frame).
              --keyed-proof PAIR (the 8a class; repeatable): a count / timing failure is
              accepted when PAIR (run folder with compare.txt, or the compare file) is the same route's keyed pair:
              rng-mode A and B keyed, seeds equal on every frame, its sound-amb PASS, equivalent (strict): yes. Never
              for a start repeated on every frame. Naming the same route is the caller's duty (evKoff10/evKon10)
  procs       the same processes: per profile the same number created, creation and deletion frames within
              +-proc_slack; no profile only one run has (no new processes)
  save        the save-info blocks (si.bin, and si.bin.T of both) are identical outside --save-mask ranges; the counter-edge rule (8c):
              the 30 Hz counters of unconverted processes listed in SAVE_COUNTERS (only those)
              may differ by the number of their start / stop edges (from the si.bin.T series) that lie within proc_slack
              of a converted party's procedure change (Link) or state change (CONV_STATE actor dumps, --state-word); an
              edge counts only where the dumps resolve it (window <= 6 frames: si.bin.T at most 0.1 s apart around it).
              8c fall sequence (sw61a): an edge also counts when it is the counter's first
              edge after a SHIFTED Link procedure change (A / B frames 1..proc_slack apart, same procedure sequence),
              no other Link procedure change lies between them in either run, it ends at most fall_seq frames (30)
              after the change, and it moves the same way as the change (B early -> B's edge not later): the world's
              freeze / resume of a fall -> respawn chain whose start Link took a frame early (the per-step ground-check rule (6a)). A counter's
              allowance is in counts of its step (SAVE_COUNTERS: +0x44 mTime 0.02 a frame, rel. 1 %)
  admp        actor dumps of the same name: per process and frame, the words of --admp-spec within their tolerance;
              words the draw writes (B's first-step record equals A's frame record on every frame they differ: the
              draw's one-pass shift, 2026-11-17) are masked in the report and named where a spec lists them
              (without a spec: report only, the words that differ most; with --admp-require-spec NAME / all such a dump
              is a FAIL, gate admp-spec: a mark's subject words must be judged; --admp-spec NAME=FILE[,NAME=FILE]
              gives each dump its own spec, a dump not named has none unless a plain --admp-spec FILE is given too). the speed-ramp position-gap rule (6c) for an enemy's own motion:
              a spec line `OFF f32 TOL 6c=SPOFF` (a position word; SPOFF the actor's
              horizontal speed word, f32) adds the 6c term to TOL: per process, the sum over B's frames up to f of
              (1 - dt_B) / 2 x |dv|, dv = the speed change B's sub-steps integrate in that frame ((v_last - v_first) x
              N / (N - 1) over the frame's N step records: a ramp's whole change; a speed set on one step counts 0).
              Discrete words stay strict.
              tevStr light (closed as a comparison issue): the view-space
              light words of a dKy_tevstr_c (mLightObj +0x00..+0x83: view-space position / direction, set by
              settingTevStruct from the view matrix of its draw; the HD copies +0xC0..+0x1C7) are picture state, no
              logic reads them (logic reads the colours and the env index +0x90 / +0x98 / +0xBA). They are masked in
              the report and in the spec check (a spec word inside them is skipped and named), and the light position
              is checked in WORLD space instead (gate admp-light): turned back with the picture camera of the record's
              draw (A: its own camera of frame f-1; B: the interpolated one, halfway between its f-1 and f-2 logic
              cameras, or its f-1 camera, the closer of the two: a converted actor lit in its draw after the camera's
              draw on the tick30 pass sees the interpolated view; cam.txt eye / centre; frames with a camera shake
              in either run, f-2..f, are left out: the drawn view carries the shake offset, cam.txt does not), within TOL light of A's (relative to the
              light's distance from A's eye, default 0.01: the blended picture camera's angle error; bdkll1 0.005). tevStrs: the actor's own
              at +0x110 in every dump, more with spec lines `tevstr OFF` (or `--admp-tevstr NAME:OFF,...` without a
              spec file)
  cloth       the cloth-simulation rule (6k) (SieFlag pair): a cloth's vertex arrays
              (CLOTH dumps, default cl.admp = WWHD_MEM_DUMP of dCloth_packet_c's arrays; --cloth adds one) are a chaotic,
              visual-only simulation that cannot follow the 30 Hz path vertex by vertex at a finer step: no per-vertex
              check; over >= 200 frames after the conversion (B's log: test conversion from scenario time T) the mean vertex
              speed within 10 % of A's, the p99 vertex speed within 15 % and the max within 30 % (6k amendment,:
              the single max sample of a chaotic cloth is noisy, 58.8-64.4 over B runs alone; sanity only), each
              vertex's mean position within 3 x A's
              standard deviation of it x sqrt(2 / N_eff), N_eff = window / (2 tau_A), tau_A = A's integrated
              autocorrelation time of the vertex (summed up to the first zero crossing; 6k bound;
              N_eff reported), the same bound on the sail mean (the mean over all tracked vertices; sd, tau and N_eff from
              A's pooled series; 6k pooled amendment), over >= 1800 frames after the conversion (6k window: a
              shorter cloth pair fails, "lengthen the pair") (3-D: the distance of the means against the rms distance from A's mean), no NaN / inf.
              Equal vertices on every frame pass at once. Cloth vertex arrays only; never actor positions or collision
  rng-mode    the strict RNG-pair rule (8d) tooling: the test RNG mode of each run (the port's log line: WWHD_TEST_RNG_KEYED with
              WWHD_TEST_RNG_KEYED_MODE frame / seq, WWHD_TEST_RNG_CONST, or the game's own RNG), printed for every pair;
              FAIL when the two runs used different modes (the pair is not comparable)
  rng         report only (per-step chances draw on every step by design, dt = 1/N the per-step chance rule (8))
  sound-rng   the RNG-drift plausibility rule (8b): from the first RNG difference, the starts of unconverted code (not
              started by B on an in-between step, caller not in a converted class's code) are left out of `sound` when
              unmatched and checked for plausibility like sound-amb (same keys, counts within the noise, nothing every frame);
              8b addendum: a source found in one run only there is tolerated when all its starts
              lie in the run's last W frames, W = the longest interval of its starts in A's run (the whole window from the
              RNG difference on when A plays it once; never when A never plays it); converted sources stay strict.
              the sound-gate rule (8a)'s ambient class extends to the pointer record (wrapper 4) an ambient id start makes on the
              same step from the same caller (se_keyf)
  procs-rng   the RNG-drift plausibility rule (8b): unconverted profiles (B's log line [t60] test conversion profiles; --converted P,.. overrides)
              living on after the first RNG difference: starts before it strict, then counts and the number alive at the
              end within the noise of RNG-timed counts, none stuck (alive at the end longer than the profile's longest
              finished lifetime in A + proc_slack, in one run only)
  link-tslack the time-slack form of link-motion: a decision of the N-step run may move by
              up to half a 30 Hz frame (it can fall on an in-between step), so B's value at frame f passes when it lies
              within the same tolerance of A's trajectory (linear between A's full steps) at some time in
              [f - tslack, f + tslack] (default 0.5 frame); each quantity (pos, speed, angle, anim) on its own; an
              animation loop wrap of A inside the window is interpolated on the unwrapped frame (modulo the loop)
              the speed-ramp position-gap rule (6c): its position tolerance adds the drift of the half-step motion through
              speed ramps, (1 - dt_B) / 2 x the sum of |dv| of A's horizontal speed up to the frame; the animation-phase lead rule (6d):
              on foot-travel frames (A's speedF off its mNormalSpeed) the speed tolerance adds B's
              measured animation-phase lead x A's foot-travel rate change per animation frame and the frame-end step's
              distance from B's frame mean of step speeds (the per-step foot chord); see foot_speed_tol; the aim-boost stop rule (6e):
              in the aim procs (HD's x 1.2 boost) at a speed change it adds the boost's
              carry G(f) = min(G(f-1) + 0.2 |ns_A(f-1)|, max |ns_A|) from A's mNormalSpeed; see aim_speed_tol; rule
              the target-facing turn rule (6f): while both runs face their recorded target (trace columns 18-26) after a
              turn, the angle tolerance adds atan2(pos tol + 6c drift, A's distance to the target); rule 2
              (2026-11-17): a Link gap and a target gap that each pass the +-tslack window add the angle between
              A's and B's Link-to-target vectors (both position leads over the distance); chase form (2026-11-17,
              161): on the lock-on slot (setShapeAngleToAtnActor's chase toward the moving eyePos) each run may
              be within its own chase lag (chase_lag: half the gap per frame plus its direction's motion over the frame)
              of its own direction, the gap then adds both runs' distances from their directions; see face_angle_tol;
              the turn-factor sampling rule (6g): on A's accelerating frames of a turn the speed tolerance adds
              (1 - dt_B)/2 x sum max(0, dv_A(f) - dv_A(f-1)) of A's mNormalSpeed; see turn_accel_tol
              the animation-phase lead rule (6d(b)): after a procedure init on a non-tick30 step whose animation the
              next body call replaces, the anim tolerance adds max(0, 2 (1 - dt_B) - tslack) x A's rate, carried scaled
              by the start-frame map into an animation that takes its phase from it; B's loop wraps compare modulo the
              loop; see anim_lead_tol; the animation-phase lead rule (6d) addendum: a rate change of a
              running animation (no animation change) adds (1 - dt_B) x sum |dr_A| over the animation's run, less the
              window's share (tslack x A's rate); see rate_step_tol
  ship        the ship-drift rule (6h) (6c / 6g class): the ship's own trajectory (actor dump --ship NAME,
              default ship.admp: WWHD_ACTOR_DUMP=ship.admp:0247AE78:0x720, daShip_c's profile execute) at full steps:
              xz within pos + (1 - dt_B)/2 x sum (|dv_A| + |v_A| |dheading_A|) (speedF, current.angle.y in radians; the
              turn drift: 30 Hz moves the frame along the frame-end heading, N steps each step along its own), speedF /
              mFwdVel within speed, current.angle.y / shape_angle.y within angle; y reported (rule (c) open) with a plausibility
              check: finite, mWaterY inside A's band (not on frames a 6i term lets B's ship be elsewhere and it is: rule (c)
              addendum, 2026-11-16, pk6), float offset y - mWaterY inside A's range on the water (each +- pos);
              in flight (daSFLG_FLY_e) y - y_takeoff (each run's own take-off height) inside A's
              range over the same flight +- pos, the same number of flights in both;
              hold form: (1 - dt_B)/2 x (|sum dv_A| + |sum |v_A| dheading_A|), both sums signed
              and never reset (bounded by the net speed / heading change since the conversion), heading term x 2 (ship_turn_k,:
              the arc plus the tiller ramp's (1 - dt_B)/2-frame heading lag); see ship_turn_tol;
              the mode sequence (0x635) matches, each change within proc_slack; the early ship-decision rule (6i):
              after a decision (mode change, stopped/moving switch) that B makes up to proc_slack frames earlier than A,
              speed is plausibility only within proc_slack of the switch, xz adds (1 - dt_B) x max |v_A| since the shift
              (until A stops for proc_slack frames) and, while that runs, speed adds (1 - dt_B) x |dv_A| (ship_shift_dv,
              derived 2026-11-16); see ship_shift_tol. 6i(ii): the same for a
              decision B makes LATER (1..proc_slack frames, or on the switch frame's last step: an unconverted driver's
              switch seen on the tick30 step); its follow-on rope-timed sounds trail by the derived D = rope difference x
              the winding rate (ship_followon; `sound` matches them within se_slack + D). 6i addendum
              (pairs): the sail's branch change (the speed chases the wind power while it is > 0,
              else declines at 0.1 a frame) when the wind power reaches 0 inside a frame: a frame where one run declines
              by 0.1 and the other does not, the other joining within proc_slack frames, opens a window up to the first
              frame after it with both on the same branch and |speed B - A| <= speed; inside it speed adds (1 - dt_B) x
              the chase step (ship_branch_step 2) and xz adds that per frame since the crossing (kept after the window)
              6i(iii) (pair): a stopped/moving switch that is a decision on position
              (procZevDemo's DEMO_MOVE arrival radius, the ship in mode 7 before it) may come k frames apart, k <=
              ceil(|xz B - A| / |v_A|) on the frame before A's switch; speed plausibility only between the two switch
              frames; the rest positions within pos (ship_pos_shift; see ship_pos_shifts)
              the animation-phase lead rule (6d(c)) (pgrab5): on a foot-travel frame where the animation
              changes (A and B in the same frame), the speed tolerance adds r x (|toe_B(start) - toe_A(f-1)| +
              |toe_B(end) - toe_A(f)|) of the base foot (trace columns 27-31; r = m3598 from column 32, port
              m3598 column; older traces without it take r as its bound 1); 0 on every other frame;
              a base foot that differs between the runs gets no term (reported); addendum (pfol1 107): the same on a
              base-foot switch within the frame, each run's toes of its own chord foot (differing chord feet: B's foot's
              pose gaps + A's own foot-choice chord gap; both runs must switch); see change_chord_tol
  cam-tslack  the same for the camera (eye, center, fovy); the speed-ramp position-gap rule (6c) camera form: the centre's
              tolerance adds Link's frame-end offset capped by his 6c drift (the centre follows Link), the eye's adds that and the moving-target drift D(f) = cam_keep x D(f-1) + (1 - dt_B)/2 (|d(centre velocity)_A| + |dR_A|) (from the conversion on; cam_keep 0.907 measured in pl3) plus the steady chase lag difference g |centre velocity_A| (g = dt_B (1 - r_s)/r_s - 1/3, r_s = 1 - 0.25^dt_B; 1/6 at N = 2), the centre
              and fovy keep cam (centre chase, pkk1d_4 / pkk3d_4 frame 54: while the centre chases a
              target other than Link - A's lock-on slot set and A's camera mode not 0 - the centre adds the same g |centre
              velocity_A|; while it follows Link it does not); frames within proc_slack of a camera mode change (lock start/release on the
              tick30 step) or of a bumped-eye radius jump > 1/4 per frame that both runs make within proc_slack (the per-step camera-collision rule (6b)) are plausibility only (finite);
              the per-step camera-collision rule (6b) episode (crawl pair cy): a bumped-eye jump only one run makes (a
              bumpCheck line hit on B's in-between step) opens a plausibility window from the first bump-state difference
              (the eyes apart beyond cam before the jump, or differing `bump` fields where the trace has them) until both
              eyes converge within cam, at most 30 frames (unconverged: no window, reported); see bump_episodes;
              the speed-ramp position-gap rule (6c) camera, Link-following centre chase (himo2 hungCamera): a centre that
              follows Link with a style ratio below 1 (STYLE_CENTRE_RATIOS: param 3 xz / param 4 y) adds per axis
              g(r_axis) |A's centre velocity on that axis| (g = dt_B (1 - r_s)/r_s - (1 - r)/r, chase_gain), eye and centre;
              the speed-ramp position-gap rule (6c) camera (yaw episodes): while A's camera style is a
              manualCamera style (MANUAL_STYLES; manual -> manual style changes continue the session) the eye adds A's
              horizontal eye-centre radius x (1 - dt_B)/2 x sum over episodes |net change of A's view yaw rate| (an episode
              a monotone stretch of the rate; held, never reset, in the session; after the session the held term decays by
              cam_keep per frame); see manual_yaw_tol (--tol myaw=0 turns it off)
              the step-resolution threshold rule (6l) + addenda: a threshold event (a speed jump,
              or (a) a slope change of speed.y) one frame early in B, relative to a carried earlier event's lead: speed
              at B's event frame plausibility, the offset carried to convergence or the next shared procedure change
              (event_shift_tol); (b) inside a carry window animation switches up to 1 frame early or late, same sequence
              (carried_anim_switch); (c) a carry unconverged at a shared change: Link, camera and sound series
              plausibility to the next shared change (the landing), strict again once every gap is back within its normal
              tolerance, else the rest is reported, not failed
  actor-plaus the strict RNG-pair rule (8d), secondary proof of a converted RNG-driven actor on a normal-RNG pair:
              with --plaus-spec FILE, from the first RNG difference on, the actor's dump is checked for plausibility (B's
              values finite, no stuck state, health and hits consistent, timers and attack starts within A's distribution;
              see plaus_spec / plaus_check) and its processes and sound starts move to procs-rng / sound-rng; before the
              difference the strict gates stay. SKIP without an RNG difference (the strict gates cover the whole run)
  strict-cut  the strict RNG-pair rule (8d) addendum (enemies that push or hit Link; pairs with Link converted
              too, CONV 168,0x1DC,<enemy>): --strict-until FRAME ends the strict per-frame comparison at the first enemy
              decision that traces back to an accepted Link difference. --cause is required and names it: `rule=R`
              (the accepted rule on Link: 3a, 4a, 6a-6g) and `decision=TEXT` (the enemy decision, e.g. "fight_run 4/2 at
              97 reads Link's distance"); a cut without a valid cause FAILS (and so does a cause without a cut). Before
              FRAME every gate stays strict (a real divergence before the cut fails); from FRAME on link-procs /
              link-motion / link-tslack / camera / cam-tslack / admp stop, sound starts and living processes move to
              sound-rng / procs-rng (all of them, converted ones too) and actor-plaus starts there (or at the first RNG
              difference if that comes first); health, mode and save stay. Enemies without Link contact: no cut.
              Addendum 2: the cause may name an accepted rule on either side when it feeds into
              a contact (a hit, push or knock-back on Link, or the enemy's decision about Link): `side=link` (default,
              the clamped-smoothing rule (3a), 4a, 6a-6g) or `side=enemy` (the enemy's own accepted classes: the per-step smoothing rule (3) / 3a float drift, 6c its
              own continuous motion through a changing speed), e.g.
              "side=enemy rule=3 decision=the jump attack's knock-back angle reads the ChuChu's float-drifted position"
              the step-resolution threshold rule (6l) addendum (f): `rule=6l hit=SPEC` on either side, a hit
              outcome sampled at step resolution (a converted attacker's hit volume checked on every step's Move): the
              cut is B's hit frame; SPEC `life` (Link's mLife, life.admp) or FILE:OFF:TYPE (an actor dump word, e.g.
              cc.admp:3A1:s8); A must change the same word on the cut frame or one frame later (two frames early,
              late, or no such hit in A: FAIL), e.g. "side=link rule=6l hit=cc.admp:3A1:s8 decision=hit sampled at
              step resolution (Link's sword kills the ChuChu)"
              Addendum 4: `side=link rule=12a` (process order, unconverted Link:
              the enemy's first-step decision, the first-step AI-timer rule (14d), sees Link's tick30-step action one frame later) is valid
              only when Link (00A8) is not converted in B (B's log / --converted); with Link converted it fails
  save-life   addendum 3: after the cut the save stays strict except Link's current health
              (mLife, si.bin +0x22), which must stay within A's range over the pair: B's values from the cut on (per
              frame from life.admp, WWHD_MEM_DUMP=life.admp:<save area + 0x20>:4, and in the blocks after the cut) within
              [min, max] of A's (life.admp and every block); before the cut the per-frame values are equal. SKIP
              without a cut (mLife then stays in the strict save gate)
Two verdicts: `equivalent (strict)` with link-motion and camera (the per-frame gates); `equivalent (time slack)` with
link-tslack and cam-tslack in their place. A decision that never rejoins (another procedure sequence, a ledge choice)
fails both. Exit 0 strict equivalent, 3 equivalent under the time slack only, 1 neither, 2 no origin. Gates that are only
part of one verdict print it in brackets.
Tolerances (--tol NAME=VALUE): proc_slack 1, se_slack 1, pos 2.0, speed 0.5, angle 256 (s16 units), anim 1.0, cam 2.0,
tslack 0.5 (30 Hz frames), ramp -1 (the speed-ramp position-gap rule (6c)'s drift factor; -1: (1 - dt_B) / 2), cam_keep 0.907 (the camera drift's decay per frame),
turn -1 (the turn-factor sampling rule (6g)'s factor; -1: (1 - dt_B) / 2; 0 turns the term off), face 1 (the target-facing turn rule (6f)'s term; 0 off), chase 1 (6f's chase form; 0 off), cchase 1 (6c camera's centre chase terms, the non-Link target and the Link-following centre; 0 off), bump_ep 1 (6b episode window; 0 off),
ship_ramp -1 (the ship-drift rule (6h)'s factor; -1: (1 - dt_B) / 2), ship_turn_k 2 (multiplier of 6h's heading term: the arc plus
the tiller ramp's (1 - dt_B)/2-frame heading lag; 1 = the arc alone), ship_shift_dv 1 (rule
the early ship-decision rule (6i)'s speed form; 0 off), ship_later 1 (the early ship-decision rule (6i(ii)); 0 off), ship_pos_shift 1 (the early ship-decision rule (6i(iii)); 0 off),
face14i 1 (the process-order read rule (14i) interim (a): own-eye facing of a later-in-line converted target; applies only with
--target-gate FILE[:PROC], the enemy's own gate report, all PASS; 0 off), carrier 1 / carrier_cam 1 (the carrier-offset rule (6j)),
carrier_bg 1 (the carrier-offset rule (6j)'s moving-BG carrier cap, 0 off), ev_cam_off 1
(615, by target since 2026-11-19: an event camera, type 3 / style 4 in both runs, whose
centre is off the ship (evcam_offship's test, evship_r) does not take the carrier offset; one on the ship does; never the
better of both forms, which could hide a real error; 0 off: with evcam_offship=0 too, 6j subtracts the offset from it), held_gap 1 (the held camera-gap rule (6m), held-gap angle allowance, talk16 / talk18
847: an eye-direction gap within the camera tolerance at a smaller radius may grow in units with the radius as long as the
angle stays within that tolerance; 0 off), contact 0 / contact_r 400 / contact_ext 0.5 / contact_launch 5 (OPT-IN since 2026-11-18: --tol contact=1
per pair; showed a knock-back gap counted as enemy drift was a real defect; the speed-ramp position-gap rule (6c)
contact propagation: Link's displacement gap from a push / hit by an enemy that passes its
--admp-spec words at the contact, within the enemy's position gap + facing gap x contact radius, carried on his later
frames; needs --admp-spec with a 6c position word and an a16 facing word; 0 off), carrier_cam_y 0 (1: the camera
also takes the carrier offset's y part; default xz only, Link takes the whole vector), crane_plaus 1
(the crane / salvage window is plausibility only until 0: strict), crane_rest 1
(rope-timed window starts beyond se_slack + D after a B-later crane-up shift reported, not failed; 0: they
fail `sound`), light 0.01 (admp-light: a tevStr light's world position, B against A, relative to its distance from the
eye, the view-space light words are picture state).
myaw 1 (the speed-ramp position-gap rule (6c) camera's yaw-episode term; 0 off), ratestep 1 (the animation-phase lead rule (6d) addendum's running-animation rate-step term; 0 off), event 1 (the step-resolution threshold rule (6l); 0 off), event_jump 4 (a jump event's
least |dv|), event_slope 1 (a slope event's least slope change; 0 off), event_dv 1 (the derived speed form after a jump
event: (K - tslack) x A's local |dv|; 0 off), ship_branch_step 2 (the early ship-decision rule (6i) addendum: the sail chase's step
per frame; 0 off), event_pos 1 (6l addendum (g): a position-driven speed step one frame
early while B's lead is within the time slack; 0 off), cam_type 1 (6l addendum (g) on the camera: a camera type change
one frame early while Link's lead is within the time slack; 0 off), cam_type_carry 1 (its carry over the following frames,
decaying with the new style's centre follow ratios; 0 off), ship_cam 1 (the camera on the ship takes the ship gate's
accepted offset and heading difference; 0 off), ride_trans 20 (ptalk2: the carrier
chase term uses the A style's own centre follow ratio (rpx param 3) and, in a ride style's first ride_trans frames, the
entry transition's share 1/(ride_trans - t); 0: the fixed ratio 0.75), ship_att 1 (930 /
talk13 965: an unconverted camera on a converted ship within the ship gate takes the ship's attitude allowance; 0 off),
cam_hold 1 (sv2 135: a held direction latched within the time window; 0 off),
evship_cam 1 (the carrier-offset rule (6j) addendum; 0 off: an event camera (type 3, style 4)
whose centre is on the ship in both runs adds the ship's frame-end |B - A| offset, capped by the ship gate's xz
allowance, to cam-tslack's eye tolerance), evcam_offship 1 (6j addendum; 0 off: the same event camera
with its centre off the ship does not ride the carrier: 6j's offset is not subtracted from its eye / centre),
evship_r 300 (the centre-on-ship radius of both: |centre - ship position| at most this, in both runs).
stuck 60 (actor-plaus: frames allowed on top of twice A's longest unchanged state / A's longest attack).
"""
import collections
import glob
import math
import os
import re
import struct
import sys

# the strict RNG-pair rule (8d) addenda: the accepted rules a strict cut's --cause may name, per side
LIFE_OFF = 0x22  # si.bin (the dComIfGs save area; dSv_info_c at +0x20) -> dSv_player_status_a_c::mLife (u16, big-endian)
LIFE_DUMP = "life.admp"  # WWHD_MEM_DUMP=life.admp:<save area + 0x20>:4 (mMaxLife, mLife; p60.sh LIFE=1)
CUT_RULES = {"link": ("3a", "4a", "6a", "6b", "6c", "6d", "6e", "6f", "6g", "6l", "12a"), "enemy": ("3", "3a", "6c", "6l")}
# the step-resolution threshold rule (6l) addendum (f): `rule=6l` as a cut cause is a hit outcome sampled at
# step resolution (a converted attacker's hit volume is checked on every step's collision Move): the cut is the frame
# of B's hit, named by `hit=SPEC` (life = Link's mLife from life.admp; FILE:OFF:TYPE = an actor dump word, e.g.
# cc.admp:3A1:s8 the ChuChu's health); A must make the same hit on the cut frame or ONE frame later (never earlier =
# B late, never two frames later, never no hit in A)
CUT_HIT_RULES = ("6l",)
# 8d addendum 4: rule=12a (process order, unconverted Link) is a valid Link-side
# cause only when Link (profile 00A8) is NOT converted in B: then Link's tick30-step action reaches the enemy's
# first-step decision (the first-step AI-timer rule (14d)) one frame later than at 30 Hz (test-only transitional setup)
CUT_RULES_UNCONVERTED_LINK = ("12a",)

TOL = {"ride_trans": 20.0, "ship_att": 1.0, "cam_hold": 1.0, "evship_cam": 1.0, "evcam_offship": 1.0, "evship_r": 300.0, "proc_slack": 1, "se_slack": 1, "pos": 2.0, "speed": 0.5, "angle": 256, "anim": 1.0, "cam": 2.0, "tslack": 0.5, "ramp": -1.0, "cam_keep": 0.907, "turn": -1.0, "face": 1.0, "myaw": 1.0, "event": 1.0, "event_jump": 4.0, "event_dv": 1.0, "event_slope": 1.0, "ratestep": 1.0, "event_pos": 1.0, "cam_type": 1.0, "chase": 1.0, "cchase": 1.0, "stuck": 60, "ship_ramp": -1.0, "ship_turn_k": 2.0, "ship_shift_dv": 1.0, "carrier": 1.0, "carrier_cam": 1.0, "ship_later": 1.0, "ship_pos_shift": 1.0, "face14i": 1.0, "carrier_bg": 1.0, "carrier_cam_y": 0.0, "crane_plaus": 1.0, "crane_rest": 1.0, "ship_branch_step": 2.0, "sea_slope": 1.0, "cam_type_carry": 1.0, "ship_cam": 1.0, "fall_seq": 30, "bump_ep": 1.0, "ev_cam_off": 1.0, "held_gap": 1.0, "ev_start_se": 1.0, "contact": 0.0, "contact_r": 400.0, "contact_ext": 0.5, "contact_launch": 5.0, "light": 0.01}

# gates of one verdict only (the others count for both)
STRICT_ONLY = ("link-motion", "camera")
SLACK_ONLY = ("link-tslack", "cam-tslack")


def die(msg, code=2):
    print(msg)
    sys.exit(code)


def lines(run, name):
    p = os.path.join(run, name)
    return open(p, errors="replace").read().splitlines() if os.path.exists(p) else []


def origin(run):
    for l in lines(run, "log"):
        m = re.search(r"\[test\] origin.*logic step (\d+)", l)
        if m:
            return int(m.group(1))
    return None


def origin_clock(run):
    """port log line at the scenario origin (WWHD_TEST_FIXED_SEED + WWHD_TEST_ORIGIN_RESEED): the world clock
    (dSv_player_item_record_c.mTimer) and mCounter0 there; None when the log has no such line; plus whether
    WWHD_TEST_SYNC_LOADS was on (its "[test] WWHD_TEST_SYNC_LOADS: ... loader thread(s)" line)"""
    clk, sync = None, False
    for l in lines(run, "log"):
        if "WWHD_TEST_SYNC_LOADS:" in l and "loader thread" in l:
            sync = True
        m = re.search(r"scenario origin \(WWHD_TEST_ORIGIN_RESEED\).*mCounter0 \d+ \(was (\d+), mTimer (\d+)\)", l)
        if m and clk is None:
            clk = (int(m.group(2)), int(m.group(1)))
    return clk, sync


def fnum(s):
    try:
        return float(s)
    except ValueError:
        return float("nan")


class Frames:
    """step -> scenario frame of one run"""

    def __init__(self, run, s0, k_override=None):
        self.s0 = s0
        self.full = None
        # a test warp's or the game's own stage change's loading frames after the origin (port: WWHD_TEST_LOAD_SEED_HOLD, the scenario clock paused
        # over them; their number differs by host between the runs): no frame, like the time before the origin
        self.paused = [(int(m.group(1)), int(m.group(2))) for m in
                       (re.search(r"\[test\] (?:test warp|stage change): scenario clock paused over logic steps (\d+)-(\d+)", l) for l in lines(run, "log")) if m]
        self.k = 1
        self.src = ""
        sl = [l.split() for l in lines(run, "steps.txt")]
        sl = [p for p in sl if len(p) >= 2 and p[0].isdigit()]
        if sl:
            self.full = sorted(int(p[0]) for p in sl if p[1] == "1" and int(p[0]) >= s0 and not self.in_pause(int(p[0])))
            self.src = "steps.txt (%d full steps)" % len(self.full)
            return
        if k_override:
            self.k = k_override
            self.src = "K=%d (given)" % self.k
            return
        dts = [fnum(p[4]) for p in (l.split() for l in lines(run, "trace.txt")) if len(p) > 4]
        small = [d for d in dts if 0 < d < 1]
        if small:
            self.k = max(1, int(round(1 / min(small))))
        self.src = "K=%d (trace.txt dt)" % self.k

    def in_pause(self, step):
        return any(a <= step <= b for a, b in self.paused)

    def frame(self, step):
        if step < self.s0 or self.in_pause(step):  # before the origin (title, state load) / a warp's loading: no frame
            return None
        if self.full is not None:
            import bisect
            i = bisect.bisect_left(self.full, step)  # the full step at or after this step ends its frame
            return i if i < len(self.full) else None
        d = step - self.s0
        return d // self.k if d >= 0 else None


# tevStr light: picture state; see the admp gate's notes in the header
TEV_PIC = ((0x00, 0x84), (0xC0, 0x1C8))  # dKy_tevstr_c: mLightObj (view space), the HD light parameter copies


def tev_picture_words(offs, size):
    """the word offsets of the tevStrs at `offs` that are picture state (TEV_PIC), within a record of `size` bytes"""
    out = set()
    for t in offs:
        for a, b in TEV_PIC:
            out |= {o for o in range(t + a, min(t + b, size) - 3, 4)}
    return out


def view_to_world(v, eye, ctr):
    """a view-space point back to world space with the camera (eye, centre; up = +y, no roll, as dCamera's lookAt)"""
    f = [ctr[i] - eye[i] for i in range(3)]
    n = math.sqrt(sum(x * x for x in f))
    if n == 0:
        return None
    f = [x / n for x in f]
    sv = [-f[2], 0.0, f[0]]
    n = math.sqrt(sum(x * x for x in sv))
    if n == 0:
        return None
    sv = [x / n for x in sv]
    u = [sv[1] * f[2] - sv[2] * f[1], sv[2] * f[0] - sv[0] * f[2], sv[0] * f[1] - sv[1] * f[0]]
    return [eye[i] + sv[i] * v[0] + u[i] * v[1] - f[i] * v[2] for i in range(3)]


def state_channel(run, name, fr, stepcol, keyf, valf, end, ncol=0):
    """frame -> {key: value} (last record per frame and key); lines with fewer than ncol columns (a run killed while
    writing its last line) are skipped"""
    out = collections.defaultdict(dict)
    for l in lines(run, name):
        if l.startswith(" "):
            continue
        p = l.split()
        if len(p) <= stepcol or len(p) < ncol or not p[stepcol].isdigit():
            continue
        f = fr.frame(int(p[stepcol]))
        if f is None or (end is not None and f > end):
            continue
        out[f][keyf(p)] = valf(p)
    return out


SE_LINE = re.compile(r"^\d+ [01] \d+ [0-9A-F]{8}(:[0-9A-F]{8})? [0-9A-F]{8}$")


def event_channel(run, name, fr, stepcol, keyf, end):
    out = []
    for l in lines(run, name):
        p = l.split()
        if not SE_LINE.match(l.strip()):  # se.txt: step full wrapper id caller (a cut last line is skipped)
            continue
        f = fr.frame(int(p[stepcol]))
        if f is None or (end is not None and f > end):
            continue
        out.append((f, keyf(p)))
    return out


# the sound-gate rule (8a): ambient world sounds timed by cM_rnd: (what, sound ids, caller ranges [lo, hi) for keys by caller). Only
# sources whose start times come from RNG draws go here (per-frame chances, random timers), never a converted process.
AMBIENT = [
    ("seagull cry (d_a_kamome_auto kamome_auto_move: cM_rndF timers and chances)", {"00004805"}, [(0x02189270, 0x0218A4A8)]),
]
AMB_WINDOW = 300  # frames (10 s)


def se_ambient(k):
    """whether a sound key (se_key) belongs to an AMBIENT source"""
    for _, ids, ranges in AMBIENT:
        if len(k) == 2 and k[1] in ids:
            return True
        if len(k) == 4 and k[1] == "ptr" and k[3] in ids:  # the pointer record of an ambient id start (se_keyf)
            return True
        if len(k) == 3 and k[1] == "ptr":
            try:
                c = int(k[2], 16)
            except ValueError:
                continue
            if any(lo <= c < hi for lo, hi in ranges):
                return True
    return False


def amb_allowed(ca, cb):
    """the allowed count difference in one window: +-50 % of A's count, but never below the noise of two RNG-timed
    counts (two standard deviations of a difference of Poisson counts, 2 sqrt(ca + cb), at least 2): with few starts
    per window +-50 % is inside the noise (p16_move: 3 vs 6 cries; gulls cry on a 0.5 chance every 64 frames)"""
    return max(0.5 * ca, 2.0 * math.sqrt(max(ca + cb, 1)))


def ambient_check(sea, seb, key_ok=None):
    """sound-amb (the sound-gate rule (8a)): (ok, message, details) for the ambient starts [(frame, key)] of A and B; key_ok(key,
    frames) (sound-rng, the RNG-drift plausibility rule (8b) addendum): a key found in one run only is tolerated where it returns True"""
    ka = {k for _, k in sea}
    kb = {k for _, k in seb}
    det = []
    tol_keys = set()
    if key_ok is not None:
        tol_keys = {k for k in ka ^ kb if key_ok(k, sorted(f for f, kk in sea + seb if kk == k))}
    if (ka ^ kb) - tol_keys:
        det.append("keys only in A: %s, only in B: %s" % (sorted(ka - kb - tol_keys), sorted(kb - ka - tol_keys)))
    last = max([f for f, _ in sea + seb] or [0])
    worst = None
    for k in sorted(ka | kb):
        fa = sorted(f for f, kk in sea if kk == k)
        fb = sorted(f for f, kk in seb if kk == k)
        for w0 in range(0, last + 1, AMB_WINDOW):
            ca = sum(1 for f in fa if w0 <= f < w0 + AMB_WINDOW)
            cb = sum(1 for f in fb if w0 <= f < w0 + AMB_WINDOW)
            if abs(cb - ca) > amb_allowed(ca, cb):
                det.append("%s: frames %d-%d A %d, B %d starts (allowed +-%.1f)" % (k, w0, w0 + AMB_WINDOW - 1, ca, cb,
                                                                                   amb_allowed(ca, cb)))
        def longest(fs):
            best = run = 0
            prev = None
            for f in sorted(set(fs)):
                run = run + 1 if prev is not None and f == prev + 1 else 1
                prev = f
                best = max(best, run)
            return best
        ra, rb = longest(fa), longest(fb)
        if rb > max(2, ra):
            det.append("%s: B starts on %d consecutive frames (A at most %d): repeats every frame" % (k, rb, ra))
        worst = (k, len(fa), len(fb)) if worst is None or abs(len(fb) - len(fa)) > abs(worst[2] - worst[1]) else worst
    msg = "%d / %d ambient starts, %d key(s)%s" % (len(sea), len(seb), len(ka | kb),
                                                 ", largest count difference %s A %d B %d" % worst if worst else "")
    if tol_keys:
        msg += "; %d key(s) in one run only inside the run's last window (8b addendum): A %s, B %s" % (
            len(tol_keys), sorted(tol_keys & ka), sorted(tol_keys & kb))
    return not det, msg, det


def keyed_proof_check(path):
    """the sound-gate rule (8a) class: (ok, why) whether PATH (a keyed pair's run folder holding
    compare.txt, or its compare file) proves the route: the same route run with the keyed RNG (A and B keyed), strict
    equivalent, RNG seeds equal on every frame, and its own sound-amb gate PASS with ambient starts in it"""
    f = os.path.join(path, "compare.txt") if os.path.isdir(path) else path
    try:
        txt = open(f).read()
    except OSError as e:
        return False, "%s: unreadable (%s)" % (path, e.strerror)
    lines_ = txt.splitlines()
    gl = lambda name: next((l for l in lines_ if l.split()[:1] == [name]), "")
    why = []
    if "equivalent (strict): yes" not in lines_:
        why.append("not strict-equivalent")
    mode = gl("rng-mode")
    if not re.search(r"\bA keyed\b.*\bB keyed\b", mode):
        why.append("not a keyed pair (rng-mode: %s)" % (mode.split(None, 2)[2] if len(mode.split(None, 2)) > 2 else "none"))
    rng = re.search(r"seeds equal at (\d+) of (\d+) frames", gl("rng"))
    if not rng or rng.group(1) != rng.group(2):
        why.append("RNG seeds not equal on every frame")
    amb = gl("sound-amb").split()
    if amb[1:2] != ["PASS"]:
        why.append("its sound-amb is not PASS")
    if why:
        return False, "%s: %s" % (path, ", ".join(why))
    return True, path


def ambient_keyed_accept(msg, det, proofs):
    """sound-amb failed on start counts / timing: accepted as the sound-gate rule (8a) class (RNG draw order) when every given
    keyed pair of the same route is strict-equivalent with equal seeds and a passing sound-amb (keyed_proof_check).
    A start repeated on every frame is never accepted (not an RNG order effect)."""
    if any("consecutive frames" in d for d in det):
        return False, msg + "; keyed proof not applicable (a start repeats on every frame)", det
    res = [keyed_proof_check(p) for p in proofs]
    bad = [w for ok, w in res if not ok]
    if bad:
        return False, msg + "; keyed proof rejected", det + ["keyed proof: " + w for w in bad]
    return True, msg + "; count / timing difference accepted (rule note 8a class): keyed pair strict-equivalent: %s" % (
        ", ".join(w for _, w in res)), ["accepted: " + d for d in det]


def se_w4_ids(run, fr, end):
    """[(frame, se_key, sound id)] for the pointer-keyed wrapper-4 starts (a sound object's start: r4 the object) whose
    next se.txt line is the same start's wrapper-0 line (same step and caller: the sound id). The trace's caller column
    is the guest link register, which converted (native) code does not set: it names the last guest call made before
    the start (346: the same 0x3809 start of Link's sound object, A after procSwimWait's cM_rndF call 0242F5B4,
    B on the frame's first step after execute's call 0240D5D0), so two runs' pointer keys of one start can differ."""
    out, prev = [], None
    for l in lines(run, "se.txt"):
        if not SE_LINE.match(l.strip()):
            prev = None
            continue
        p = l.split()
        if prev is not None and p[2] == "0" and p[0] == prev[0][0] and p[4] == prev[0][4]:
            out.append((prev[1], se_key(prev[0]), p[3]))
        prev = None
        k = se_key(p)
        if p[2] == "4" and len(k) == 3 and k[1] == "ptr":
            f = fr.frame(int(p[0]))
            if f is not None and (end is None or f <= end):
                prev = (p, f)
    return out


def se_w4_pair(un_a, un_b, ids_a, ids_b, slack):
    """pairs unmatched pointer-keyed wrapper-4 starts of A and B (keys differing only in the stale caller, se_w4_ids)
    whose following wrapper-0 sound ids are equal, within slack frames; returns (un_a, un_b, number of pairs)"""
    ia, ib = collections.defaultdict(list), collections.defaultdict(list)
    for f, k, i in ids_a:
        ia[(f, k)].append(i)
    for f, k, i in ids_b:
        ib[(f, k)].append(i)
    left_b = list(un_b)
    out_a, n = [], 0
    for f, k in un_a:
        ida = ia.get((f, k), [])
        hit = None
        for j, (g, kb) in enumerate(left_b):
            if abs(g - f) <= slack and kb[:2] == k[:2] and kb[0] == "4" and set(ida) & set(ib.get((g, kb), [])):
                hit = j
                break
        if hit is None:
            out_a.append((f, k))
        else:
            left_b.pop(hit)
            n += 1
    return out_a, left_b, n


def se_key(p):
    """a sound start's key: (wrapper, r4). r4 is the sound id for the seStart wrappers; where it is a guest heap
    address (0x10000000..0x4FFFFFFF: a wrapper passing a pointer, e.g. wrapper 4) it differs between runs whose heaps
    were laid out at another moment (the state load), so the key is (wrapper, "ptr", caller) instead"""
    if len(p) <= 3:
        return tuple(p[1:])
    try:
        v = int(p[3], 16)
    except ValueError:
        return (p[2], p[3])
    if 0x10000000 <= v < 0x50000000:
        return (p[2], "ptr", p[4] if len(p) > 4 else "")
    return (p[2], p[3])


def se_keyf(run):
    """se_key for one run's se.txt, with the pointer record (wrapper 4) that an ambient id start (wrapper 0) makes on the
    same step from the same caller keyed as ("4", "ptr", caller, id): one start writes both records (pj2: the gull cry
    0x4805 and its wrapper-4 record, both with caller 02520570 dComIfGp_getReverb, 5 / 5 and 3 / 3 frames together), so
    the pointer record shares the id's ambient class (the sound-gate rule (8a)) instead of standing as a source of its own"""
    amb = {}
    for l in lines(run, "se.txt"):
        p = l.split()
        if SE_LINE.match(l.strip()) and p[2] == "0" and se_ambient((p[2], p[3])):
            amb[(p[0], p[4])] = p[3]
    def keyf(p):
        k = se_key(p)
        if len(k) == 3 and k[1] == "ptr" and (p[0], p[4]) in amb:
            return k + (amb[(p[0], p[4])],)
        return k
    return keyf


class Report:
    def __init__(self):
        self.failed = []

    def gate(self, name, ok, msg, detail=()):
        st = "SKIP" if ok is None else "PASS" if ok else "FAIL"
        tag = " [strict verdict]" if name in STRICT_ONLY else " [time-slack verdict]" if name in SLACK_ONLY else ""
        print("%-11s %s  %s%s" % (name, st, msg, tag))
        for d in detail:
            print("            " + d)
        if ok is False:
            self.failed.append(name)


def link_rows(run, fr, end):
    # trace.txt: pass step time full dt proc x y z speedF sx sy sz shapeY angleY frame rate nspeed
    def val(p):
        return {"proc": p[5], "pos": [fnum(x) for x in p[6:9]], "speedF": fnum(p[9]), "speed": [fnum(x) for x in p[10:13]],
                "ang": [int(p[13]), int(p[14])], "anim": fnum(p[15]), "rate": fnum(p[16]), "dt": fnum(p[4]), "ns": fnum(p[17]),
                "tgt": targets(p), "feet": feet(p)}
    ch = state_channel(run, "trace.txt", fr, 1, lambda p: 0, val, end, ncol=18)
    return {f: v[0] for f, v in ch.items()}


def targets(p):
    """trace columns 18-26 (port 2026-11-16, the target-facing turn rule (6f)): xyz of mpAttnActorLockOn, mpAttnActorA and the grabbed actor
    (mActorKeepGrab), '-' when unset (older traces: fewer columns, fewer targets); the lock-on slot takes the actor's
    eyePos from columns 33-35 where the trace has them"""
    out = []
    for k in (18, 21, 24):
        q = p[k:k + 3]
        out.append([fnum(x) for x in q] if len(q) == 3 and all(x != "-" for x in q) else None)
    # columns 33-35 (port): the lock-on actor's eyePos, the point setShapeAngleToAtnActor
    # turns Link to (cLib_targetAngleY(current.pos, eyePos)); where recorded, the lock-on slot is its eye, not its position
    q = p[33:36]
    if out and out[0] is not None and len(q) == 3 and all(x != "-" for x in q):
        out[0] = [fnum(x) for x in q]
    return out


def feet(p):
    """trace columns 27-31: (base foot mBaseFootIdx, toe0 x, toe0 z, toe1 x, toe1 z), the toes' model-space
    xz as posMoveFromFootPos saw them, then column 32 (m3598 column): m3598, posMoveFromFootPos's foot-travel weight r
    (None in traces without it); None in older traces"""
    if len(p) < 32 or not p[27].isdigit():
        return None
    r = fnum(p[32]) if len(p) > 32 and p[32] != "-" else None
    return (int(p[27]),) + tuple(fnum(x) for x in p[28:32]) + (r,)


def link_step_speeds(run, fr, end):
    """frame -> [speedF of each of the frame's logic steps] (trace.txt, every record, in step order)"""
    ch = state_channel(run, "trace.txt", fr, 1, lambda p: int(p[0]), lambda p: fnum(p[9]), end, ncol=18)  # key: the pass (column 0)
    return {f: [v[k] for k in sorted(v)] for f, v in ch.items()}


def foot_speed_tol(la, lb, bsteps):
    """the animation-phase lead rule (6d): frame -> the extra speed tolerance of foot-travel speed.

    Foot-travel frames: A's speedF is off its mNormalSpeed by more than the speed tolerance (posMoveFromFootPos with
    m3598 > 0: speedF = r x foot travel + (1 - r) x mNormalSpeed; the trace has no m3598, so a ground-angle cos on a
    steep slope counts as well - a looser bound there, never a tighter one). Two terms, both measured, neither flat:
      phase  lambda(f) x g(f): B's animation-phase lead over A at the frame end (|anim_B - anim_A|; across a loop wrap
             or an animation change the last measured lead is carried; capped at the anim tolerance, which the anim
             gate enforces anyway) times the foot-travel rate change per animation frame, g(f) = max |dS_A| / |dphi_A|
             over A's neighbouring frame pairs (f-1, f) and (f, f+1) that are both foot travel within one animation
             (0 < dphi <= 5): the speed B's feet are at for the phase they are ahead by;
      chord  |S_B(f) - mean of B's step speeds in frame f|: the frame-end step's own foot speed against its frame's mean
             (the per-step foot chord, finer sampling). Exact bound: |S_B - S_A| <= |mean_B - S_A| + |S_B - mean_B|, so
             with this term the gate holds B's frame mean of foot speed to A's speed + speed tol + the phase term.
    Bounded: lambda <= anim, g from A's own trace, the chord term by B's own spread inside one frame; 0 off foot travel."""
    def foot(r):
        return r is not None and abs(r["speedF"] - r["ns"]) > TOL["speed"]
    out, lam = {}, 0.0
    for f in sorted(la):
        x, y = la[f], lb.get(f)
        if y is not None and abs(y["anim"] - x["anim"]) <= 5:
            lam = min(abs(y["anim"] - x["anim"]), TOL["anim"])
        if y is None or not foot(x):
            continue
        g = 0.0
        for p, q in ((la.get(f - 1), x), (x, la.get(f + 1))):
            if foot(p) and foot(q):
                dphi = q["anim"] - p["anim"]
                if 0 < dphi <= 5:
                    g = max(g, abs(q["speedF"] - p["speedF"]) / dphi)
        st = bsteps.get(f) or [y["speedF"]]
        chord = abs(y["speedF"] - sum(st) / len(st))
        out[f] = lam * g + chord
    return out


def anim_changed(pan, prate, dt, an):
    """an animation change between two consecutive records (the earlier one's frame / rate, the later one's step
    length dt and frame): not playing on (frame + rate x dt within 0.02), not held at its end, not a loop wrap"""
    cont = pan + prate * dt
    if abs(an - cont) < 0.02 or an == pan:
        return False
    if an < pan - 5 and cont - an > max(pan, an) and abs((cont - an) - round(cont - an)) < 0.02:
        return False
    return True


def change_chord_tol(la, B, fb, end):
    """the animation-phase lead rule (6d(c)) (pgrab5 frame 120): frame -> the extra speed tolerance of
    foot-travel speed on a frame where the animation changes, and the report lines of change frames left without it.

    posMoveFromFootPos's frame speed is the base foot's toe chord over the frame (A: toe(f) - toe(f-1), one record per
    frame; B: its steps' per-step chords sum to the chord between the frame's first-step records, the delay line).
    Within one animation B's phase lead is 6d's phase term; at an animation change both runs' poses are reset to the
    same new pose while B's start pose still leads A's (pgrab5: start gap 0.858, end gap 0.062), and 6d's rate change
    per animation frame skips the change. Triangle bound: |chord_B - chord_A| <= |toe_B(start) - toe_A(f-1)| +
    |toe_B(end) - toe_A(f)|, toe_B(start) / toe_B(end) = the base foot's toe in B's first-step record of frame f-1 / f.
    The term is r x that sum, r = m3598 (the foot-travel weight, speedF = r x foot travel + (1 - r) x mNormalSpeed; the
    ground-angle cos and 0.85 only shrink it): trace column 32, A's record of f and B's last record of f (the frame-end
    step whose speedF is compared); the larger of the two if they differ (a report line when by more than 0.01); a
    trace without the column takes r as its bound 1 (looser, never tighter). Applies on A's foot-travel frames (as
    foot_speed_tol) with an animation change at f in A and one inside B's frame f (from B's last record of f-1 on);
    bounded by B's measured pose lead; 0 on every other frame. The base foot (column 27; 2 = no old-frame data) must be
    0 or 1 and the same in A's records f-1 and f and B's two first-step records, else no term and a report line.

    Addendum (pfol1 frame 107): the same bound on a foot-travel frame where A or B
    switches its base foot within the frame (A: records f-1 / f differ; B: its records from its last one of f-1 on),
    with or without an animation change. Each run's chord is that of its own chord foot over both of its records (A: the
    base foot of its record f, posMoveFromFootPos's toe[idx] - mFootData[idx]; B: the foot of its first-step record of
    f, the delay line's frame foot), so the toes are taken of that foot: chord feet equal (a = b): r x (|toe_a,B(start)
    - toe_a,A(f-1)| + |toe_a,B(end) - toe_a,A(f)|) as above. Chord feet differ (the switch lands on another step in
    each run; both runs must switch within the frame, else no term and a report line): the pose bound on B's foot b
    plus A's own foot-choice gap, r x (|toe_b,B(start) - toe_b,A(f-1)| + |toe_b,B(end) - toe_b,A(f)| + ||chord_A
    of foot a| - |chord_A of foot b||) (triangle: |c_B^b| - |c_A^a| = (|c_B^b| - |c_A^b|) + (|c_A^b| - |c_A^a|)); a
    left toe is never compared with a right one (that would add the ~22-unit foot spacing)."""
    ch = state_channel(B, "trace.txt", fb, 1, lambda p: int(p[0]),
                       lambda p: (fnum(p[15]), fnum(p[16]), fnum(p[4]), feet(p)), end, ncol=18)
    brec = {f: [v[k] for k in sorted(v)] for f, v in ch.items()}
    out, notes = {}, []

    def dist(a, b):
        return math.hypot(a[0] - b[0], a[1] - b[1])
    for f in sorted(la):
        x, p = la[f], la.get(f - 1)
        if p is None or abs(x["speedF"] - x["ns"]) <= TOL["speed"]:
            continue
        b0, b1 = brec.get(f - 1), brec.get(f)
        if not b0 or not b1:
            continue
        seq = [b0[-1]] + b1
        fe = (p["feet"], x["feet"], b0[0][3], b1[0][3])
        if any(q is None for q in fe) or any(q[3] is None for q in seq):
            continue  # an older trace without the foot columns
        # an animation change at f in A and inside B's frame f (B's change in another frame: not this term)
        chg = anim_changed(p["anim"], p["rate"] or 0.0, 1.0, x["anim"]) and any(
            anim_changed(seq[i - 1][0], seq[i - 1][1] or 0.0, seq[i][2], seq[i][0]) for i in range(1, len(seq)))
        # addendum: a base-foot switch within the frame (A: f-1 -> f; B: its records from its last of f-1 on)
        bfeet = {q[3][0] for q in seq}
        swa, swb = fe[0][0] != fe[1][0], len(bfeet) > 1
        if not chg and not (swa or swb):
            continue
        a, b = fe[1][0], fe[3][0]  # each run's chord foot (A: its record f; B: its first-step record of f)
        if any(q[0] not in (0, 1) for q in fe) or not bfeet <= {0, 1}:
            notes.append("rule note 6d(c): frame %d: %s with base foot A %d/%d, B %d/%d: no term" % (
                f, "animation change" if chg else "base-foot switch", fe[0][0], fe[1][0], fe[2][0], fe[3][0]))
            continue
        if not (swa or swb) and any(q[0] != a for q in fe):
            notes.append("rule note 6d(c): frame %d: animation change with base foot A %d/%d, B %d/%d: no term" % (
                f, fe[0][0], fe[1][0], fe[2][0], fe[3][0]))
            continue
        if a != b and not (swa and swb):
            notes.append("rule note 6d(c): frame %d: base foot switches in %s only (chord foot A %d, B %d): no term" % (
                f, "A" if swa else "B", a, b))
            continue
        toe = lambda q, k: q[1 + 2 * k:3 + 2 * k]
        ra, rb = x["feet"][5], (b1[-1][3] or (None,) * 6)[5]
        rs = [v for v in (ra, rb) if v is not None]
        r = max(rs) if rs else 1.0
        if ra is not None and rb is not None and abs(ra - rb) > 0.01:
            notes.append("rule note 6d(c): frame %d: m3598 A %.4f, B %.4f: r %.4f (the larger)" % (f, ra, rb, r))
        term = dist(toe(fe[2], b), toe(fe[0], b)) + dist(toe(fe[3], b), toe(fe[1], b))
        if a != b:  # A's own foot-choice gap: its chord of foot a against its chord of foot b
            term += abs(dist(toe(fe[1], a), toe(fe[0], a)) - dist(toe(fe[1], b), toe(fe[0], b)))
        out[f] = r * term
    return out, notes


def anim_lead_tol(la, B, fb, end, dtb):
    """the animation-phase lead rule (6d(b)) (prope7): frame -> the extra anim tolerance of B's
    animation-phase lead after a procedure init on a non-tick30 step whose animation the next body call replaces, and
    frame -> the loop length of B's animation stretches that wrap (B's wraps are compared modulo the loop).

    At 30 Hz the init's animation plays for one call (a frame) at the rate the init set before the body replaces it; at N
    steps for one step (procRopeSubject_init's ATNRS at rate 0, prope7 frame 65: B leads the wait blend by 1.1 = one
    frame at rate 1.1). With the init on the first step (the animation-phase lead rule (6d)) the lead is at most 2 (1 - dt_B) frames of A's
    play rate; the +-tslack window already covers tslack of it, so on that animation the term is
    max(0, 2 (1 - dt_B) - tslack) x A's rate. A following animation that takes its start frame from this one (new =
    k x the old one's continued frame, the same k in both runs within 1 %%: prope7's throw at 112, k 0.508) carries
    k x (2 (1 - dt_B) - (1 - dt_B)) x the old rate (B's switch step advanced the old one by rate x dt_B, A's frame by
    rate: prope7 0.28); its own init lead is the window's. The term ends at the first animation change that does not
    take its phase from the previous one, and at a procedure change. B's steps from trace.txt (every record)."""
    ch = state_channel(B, "trace.txt", fb, 1, lambda p: int(p[0]),
                       lambda p: (p[5], int(p[3]), fnum(p[15]), fnum(p[16]), fnum(p[4])), end, ncol=18)
    st = [(f, v) for f in sorted(ch) for _, v in sorted(ch[f].items())]
    lead_fr = 2.0 * (1.0 - dtb)
    out, loops = {}, {}
    mode, carry, loopL = None, 0.0, None

    def kA(g):  # A's start-frame map at frame g (None: no jump there)
        x, p = la.get(g), la.get(g - 1)
        if x is None or p is None:
            return None
        c = p["anim"] + (p["rate"] or 0.0)
        if abs(x["anim"] - c) < 0.02 or abs(c) < 1e-6:
            return None
        return x["anim"] / c
    for i in range(1, len(st)):
        f, (proc, full, an, rate, dt) = st[i]
        _, (pproc, _, pan, prate, _) = st[i - 1]
        cont = pan + prate * dt
        if proc != pproc:
            mode, loopL = None, None
            nxt = st[i + 1][1] if i + 1 < len(st) else None
            if full == 0 and nxt is not None and nxt[0] == proc and abs(nxt[3] - rate) > 1e-6:
                mode = "init"
        elif abs(an - cont) < 0.02 or an == pan:
            pass  # the animation plays on (or holds at its end)
        elif an < pan - 5 and cont - an > max(pan, an) and abs((cont - an) - round(cont - an)) < 0.02:
            loopL = float(round(cont - an))  # a loop wrap
        else:
            loopL = None
            kb = an / cont if abs(cont) > 1e-6 else None
            ka = next((k for k in (kA(g) for g in (f, f - 1, f + 1)) if k is not None), None)
            if mode is not None and kb is not None and ka is not None and abs(ka) > 1e-6 and abs(kb / ka - 1.0) < 0.01:
                carry = ka * (carry if mode == "map" else (lead_fr - (1.0 - dtb)) * prate)  # a chain of maps scales on
                mode = "map"
            else:
                mode = None
        if loopL:
            loops[f] = loopL
        if mode == "init":
            r = la.get(f, {}).get("rate") or 0.0
            out[f] = max(out.get(f, 0.0), max(0.0, lead_fr - TOL["tslack"]) * abs(r))
        elif mode == "map":
            out[f] = max(out.get(f, 0.0), abs(carry))
    return out, loops


def rate_step_tol(la, lb, dtb):
    """the animation-phase lead rule (6d) addendum (pair): a rate change of a RUNNING animation
    (no animation change: setBlendMoveAnime re-sets the play rate of the running wait / move blend each update). A
    plays the whole frame at the old rate (animeUpdate before the dispatch; the new rate first plays next frame); B
    sets the new rate on the frame's first step (the input / speed it follows) and plays steps 2..N at it: B's phase
    moves by (1 - dt_B) x dr_A at that frame (frame 286: 2.375 -> 1.1 at N 4, B -0.943 of the derived 0.956).
    The 6d class (a step playing at the rate an earlier step set; no latch for running animations, rule): the
    anim tolerance adds C(f) = (1 - dt_B) x sum |dr_A| over A's frames of the animation's run up to f (dr_A = A's rate
    at f minus at f - 1, frames where B runs Link at N steps), less the part the +-tslack window already covers
    (tslack x A's smaller rate of f - 1 / f, as 6d(b)'s term). The carry continues across a loop wrap and across an
    animation change that takes its phase from the old one (A's new frame = k x the continued one, B's the same k
    within 1 %%: scaled by k); it ends at any other animation change and at a procedure change. --tol ratestep=0
    turns it off."""
    out = {}
    if TOL["ratestep"] <= 0:
        return out
    C = 0.0
    for f in sorted(la):
        x, p = la[f], la.get(f - 1)
        if p is None or x["proc"] != p["proc"]:
            C = 0.0
            continue
        pr, r = p["rate"] or 0.0, x["rate"] or 0.0
        cont = p["anim"] + pr
        y = lb.get(f)
        if abs(x["anim"] - cont) < 0.02 or x["anim"] == p["anim"] or (
                x["anim"] < p["anim"] - 5 and cont - x["anim"] > max(p["anim"], x["anim"]) and
                abs((cont - x["anim"]) - round(cont - x["anim"])) < 0.02):
            pass  # plays on, holds at its end or wraps its loop
        else:
            q = lb.get(f - 1)
            ka = x["anim"] / cont if abs(cont) > 1e-6 else None
            cb = (q["anim"] + (q["rate"] or 0.0)) if q is not None else 0.0
            kb = y["anim"] / cb if (y is not None and abs(cb) > 1e-6) else None
            if ka is not None and kb is not None and abs(ka) > 1e-6 and abs(kb / ka - 1.0) < 0.01:
                C *= abs(ka)  # a phase-mapped change carries the shift scaled
            else:
                C = 0.0
                continue
        if y is not None and y["dt"] < 1.0:
            C += TOL["ratestep"] * (1.0 - dtb) * abs(r - pr)
        if C > 0.0:
            t = max(0.0, C - TOL["tslack"] * min(abs(r), abs(pr)))
            if t > 0.0:
                out[f] = t
    return out


# HD's aim boost (d_a_player_main_07 lk07_boostApply): mNormalSpeed x AIM_BOOST in these subject procs (commonProcInit ids)
AIM_PROCS = {0x76: "procRopeSubject", 0x80: "procBoomerangSubject", 0x83: "procHookshotSubject", 0x94: "procBowSubject"}
AIM_BOOST = 1.2


def aim_speed_tol(la, lb):
    """the aim-boost stop rule (6e): frame -> the extra speed tolerance of the aim boost's carry.

    In the subject procs HD scales mNormalSpeed by beta = AIM_BOOST after the speed update, once per call. At 30 Hz the
    boosted value is the next frame's start (a_k, the trace's frame-end mNormalSpeed R_A); at N steps every
    step starts from the unboosted value and boosts its own speed (B's frame-end R_B = beta x b_k). With D the frame's
    speed update (setNormalSpeedF's chase: decelerate by max(v x rate, min step) towards the target, clamped; a 1-Lipschitz
    map of its start value), the frame-end gap obeys exactly
        g_{k+1} = |a_{k+1} - beta b_{k+1}| = beta |D(a_k) - D(b_k)| <= |beta R_A,k - R_B,k| <= g_k + (beta - 1) |R_A,k|
    (the 60 Hz form of D itself is covered by the speed tolerance). From A's trace alone:
        G(f) = min(G(f-1) + (beta - 1) |ns_A(f-1)|,  max(|ns_A(f-1)|, |ns_A(f)|))
    on frames where A runs an aim proc at f - 1 and f, B runs at N steps and A's mNormalSpeed changes; G = 0 elsewhere
    (a steady speed is a clamp, cap or zero, which both runs reach: the carry is gone). Cap: B lacks only the boost's
    carry, so its speed lies between 0 and A's; a gap larger than A's own speed is an error. speedF = r x foot travel +
    (1 - r) x mNormalSpeed, so its gap from the carry is at most G as well (the foot part: the animation-phase lead rule (6d)'s terms)."""
    def aim(r):
        return r is not None and r["proc"].isdigit() and int(r["proc"]) in AIM_PROCS
    out, g = {}, 0.0
    for f in sorted(la):
        x, p, y = la[f], la.get(f - 1), lb.get(f)
        if not (aim(x) and aim(p)) or y is None or not y["dt"] < 1.0 or abs(x["ns"] - p["ns"]) <= 1e-3:
            g = 0.0
            continue
        g = min(g + (AIM_BOOST - 1.0) * abs(p["ns"]), max(abs(p["ns"]), abs(x["ns"])))
        out[f] = g
    return out


FACE_EPS = 64  # s16 units: a run faces its target when its angle is within this of atan2s(target - position)


def chase_res(G):
    """setShapeAngleToAtnActor's chase, cLib_addCalcAngleS(scale 2, maxStep 0x2000, minStep 0x800), on a gap G >= 0: the
    gap left after one frame's call (half the gap, capped at 0x2000; at most 0x800 -> the min step, clamped at the target)"""
    G = int(G)
    step = G // 2
    if step > 0x800:
        return G - min(step, 0x2000)
    return max(0, G - 0x800)


def chase_lag(rows, f, k, slot):
    """the target-facing turn rule (6f) chase form (161): the bound on a run's angle k's distance from its own
    target direction atan2s(target - position) at frame f for a chase toward a moving target (setShapeAngleToAtnActor:
    the lock-on actor's eyePos), from that run's own rows: the frame's call closes the gap of f - 1 plus the direction's
    motion over the frame by the chase rule (chase_res), and the direction moves on by up to that motion after the call
    (the call's target lies within it). None without the rows of f - 1 and f or the target on both."""
    p, r = rows.get(f - 1), rows.get(f)
    if not p or not r:
        return None
    tp, tr = (p.get("tgt") or [None] * 3)[slot], (r.get("tgt") or [None] * 3)[slot]
    if tp is None or tr is None:
        return None
    ep = atan2s(tp[0] - p["pos"][0], tp[2] - p["pos"][2])
    er = atan2s(tr[0] - r["pos"][0], tr[2] - r["pos"][2])
    d = angdiff(ep, er)
    return chase_res(angdiff(p["ang"][k], ep) + d) + d


def atan2s(dx, dz):
    """cM_atan2s(dx, dz) as an s16 angle (0 = +z, 0x4000 = +x)"""
    return int(round(math.atan2(dx, dz) * 32768.0 / math.pi))


def face_angle_tol(la, lb, drift, angle_changes, near=lambda f: False):
    """the target-facing turn rule (6f) (grab pair): frame -> the extra angle tolerance of a
    target-facing turn taken from positions that differ by the 6c drift.

    A turn to a target (procGrabReady_init's fopAcM_searchActorAngleY to mpAttnActorA, the lock-on turns, ...) faces the
    target from each run's own position, so the 6c position gap shows up as an angle gap of at most
    asin(gap / d) ~ atan2(gap, d), d = the distance to the target. The term holds on frames where, for one recorded
    target slot (trace columns 18-26: lock-on actor, attention actor A, grabbed actor):
      - the target is the same in both runs: B's within pos + drift(f) of A's target trajectory in the +-tslack window
        (a carried target moves with B's lead and follows Link's own 6c gap),
      - each run faces it from its own position: every angle (shape, current) that differs between the runs by more
        than the plain tolerance is within FACE_EPS of atan2s(target - position) in both runs (the cause itself, checked),
      - the position gap is itself within its 6c bound (pos + drift(f)), or (rule 2, 2026-11-17, keyed pair
        frame 161) within that bound of A's trajectory somewhere in link-tslack's +-tslack window (Link's lead);
        the value is then also at least the angle between A's and B's Link-to-target vectors (Link's and the target's
        leads, each verified in the window, over the distance; no fitted constant),
    starting at a turn (A's angle changed within proc_slack frames before) and ending at the first frame where any of
    this fails (only the plain tolerance after it, until the next turn); while neither run's angle changes the value
    stays the one of the turn's frame (the gap is the turn's: a carried target right above Link would make the current
    distance meaningless), and frames within proc_slack of a procedure change (not compared) carry the state over; once no target is recorded in both runs (thrown,
    put down) the last verified value carries while both runs keep their angles exactly unchanged and the position gap
    stays within its 6c bound (the held facing), ending at the first angle change in either run. Value: atan2(pos + drift(f), d_A) in s16 units
    (bounded by the 6c term; the exact asin bound is slightly larger, covered by the plain tolerance)."""
    def faces(r, t, k):
        return angdiff(r["ang"][k], atan2s(t[0] - r["pos"][0], t[2] - r["pos"][2])) <= FACE_EPS

    def tgt_at(t, k):
        g = int(math.floor(t))
        u = t - g
        r0, r1 = la.get(g), la.get(g + 1)
        a0 = (r0.get("tgt") or [None] * 3)[k] if r0 else None
        if u == 0.0:
            return a0
        a1 = (r1.get("tgt") or [None] * 3)[k] if r1 else None
        return [p + (q - p) * u for p, q in zip(a0, a1)] if a0 and a1 else None

    def same_target(f, k, tb):
        # B's target within Link's allowed gap (pos + 6c drift: a carried target follows Link's own position) of A's
        # target trajectory in the time window (a carried target moves with B's lead)
        lim = TOL["pos"] + drift.get(f, 0.0)
        for t in slack_times(f):
            a = tgt_at(t, k)
            if a is not None and max(abs(p - q) for p, q in zip(a, tb)) <= lim:
                return True
        return False

    def gap_strict(f):
        x, y = la.get(f), lb.get(f)
        return (x is not None and y is not None and y["dt"] < 1.0 and
                max(abs(p - q) for p, q in zip(x["pos"], y["pos"])) <= TOL["pos"] + drift.get(f, 0.0))

    def gap_ok(f):
        # the 6c bound at the frame, or (rule 2, 2026-11-17, 161) Link's lead within link-tslack's window:
        # B's position within pos + drift(f) of A's trajectory at some time in [f - tslack, f + tslack]
        if gap_strict(f):
            return True
        y = lb.get(f)
        if y is None or not y["dt"] < 1.0:
            return False
        dd = pos_slack_dist(la, lb, f)
        return dd is not None and dd <= TOL["pos"] + drift.get(f, 0.0)

    def term(f):
        x, y = la.get(f), lb.get(f)
        if not gap_ok(f):
            return None  # the position gap is out of its 6c bound and its time window (or no N-step frame): no 6f term
        strict = gap_strict(f)
        allowed = TOL["pos"] + drift.get(f, 0.0)
        for k, (ta, tb) in enumerate(zip(x.get("tgt") or [], y.get("tgt") or [])):
            if ta is None or tb is None or not same_target(f, k, tb):
                continue
            ks = [kk for kk in (0, 1) if angdiff(x["ang"][kk], y["ang"][kk]) > TOL["angle"]]
            ea = atan2s(ta[0] - x["pos"][0], ta[2] - x["pos"][2])
            eb = atan2s(tb[0] - y["pos"][0], tb[2] - y["pos"][2])
            chase = []  # the chase form's angles: (A's, B's distance from their own target direction)

            def near_dir(kk):
                oa, ob = angdiff(x["ang"][kk], ea), angdiff(y["ang"][kk], eb)
                if oa <= FACE_EPS and ob <= FACE_EPS:
                    return True
                if k != 0 or not TOL["chase"]:
                    return False
                # chase form (161): the lock-on slot's turn (setShapeAngleToAtnActor)
                # chases the moving eye: each run within its own chase lag of its own direction
                la_, lb_ = chase_lag(la, f, kk, 0), chase_lag(lb, f, kk, 0)
                if la_ is None or lb_ is None or oa > max(FACE_EPS, la_) or ob > max(FACE_EPS, lb_):
                    return False
                chase.append((oa, ob))
                return True
            if all(near_dir(kk) for kk in ks):
                d = math.hypot(ta[0] - x["pos"][0], ta[2] - x["pos"][2])
                if d > 0:
                    v = math.atan2(allowed, d) * 32768.0 / math.pi if strict else 0.0
                    # rule 2 (2026-11-17): Link's lead and the target's lead, each verified within the window
                    # (gap_ok / same_target), turn the facing direction by the angle between A's and B's Link-to-target
                    # vectors: the two position leads over the distance
                    v = max(v, angdiff(ea, eb))
                    # chase form: each angle is its run's own (verified) chase lag off its direction, so the angle gap is
                    # at most the directions' gap plus both lags (the triangle inequality, no fitted constant)
                    for oa, ob in chase:
                        v = max(v, angdiff(ea, eb) + oa + ob)
                    return v
        return None
    out, open_, last = {}, False, None
    for f in sorted(la):
        if open_ and last is not None and near(f):
            # within proc_slack of a procedure change (frames link-tslack does not compare: a throw's release, ...):
            # the state carries over unverified, as the comparison skips them
            out[f] = last[0]
            continue
        t = term(f)
        if t is not None and last is not None and la[f]["ang"] == last[1] and lb[f]["ang"] == last[2]:
            t = last[0]  # nobody turned since the verified frame: the gap is the turn's, its bound stays the turn's
        if t is None:
            # the held facing: after verified facing, the last verified value carries while both
            # runs keep their angles exactly as on that frame (target released / carried away, nobody turned) and the
            # position gap stays within its 6c bound; the first angle change in either run ends it
            # (only once no target is recorded in both runs: a target that is still there must verify)
            gone = not any(ta is not None and tb is not None for ta, tb in zip(la[f].get("tgt") or [], (lb.get(f) or {}).get("tgt") or []))
            if open_ and last is not None and gone and gap_ok(f) and la[f]["ang"] == last[1] and lb[f]["ang"] == last[2]:
                out[f] = last[0]
                continue
            open_, last = False, None
            continue
        if not open_ and any((f - k) in angle_changes for k in range(TOL["proc_slack"] + 1)):
            open_ = True
        if open_:
            out[f] = t
            last = (t, list(la[f]["ang"]), list(lb[f]["ang"]))
    return out


TARGET_GATE = []  # the process-order read rule (14i) interim (a): [(path, process or None)] of the target's own gate reports (--target-gate)


def target_gate_ok():
    """the process-order read rule (14i) interim (a): (ok, note) of the target's own gates (--target-gate FILE[:PROC]): every PASS / FAIL
    verdict of the report (a bkcmp-style text: `process XXXXXXXX:` blocks with `PASS` / `FAIL` words; with PROC only
    that process's block) must be PASS, and at least one must be there. No --target-gate: (False, reason)."""
    if not TARGET_GATE:
        return False, "no --target-gate (the enemy's own gate report) given"
    notes = []
    for path, proc in TARGET_GATE:
        try:
            lines = open(path).read().splitlines()
        except OSError as e:
            return False, "target gate %s unreadable (%s)" % (path, e)
        cur, nv, bad = None, 0, []
        for l in lines:
            m = re.match(r"\s*process ([0-9A-Fa-f]{8})\b", l)
            if m:
                cur = m.group(1).upper()
                continue
            if proc is not None and cur != proc:
                continue
            for v in re.findall(r"\b(PASS|FAIL)\b", l):
                nv += 1
                if v == "FAIL":
                    bad.append(l.strip()[:80])
        if nv == 0:
            return False, "target gate %s%s: no verdict" % (path, ":" + proc if proc else "")
        if bad:
            return False, "target gate %s%s FAIL (%s)" % (path, ":" + proc if proc else "", bad[0])
        notes.append("%s%s %d PASS" % (os.path.basename(path), ":" + proc if proc else "", nv))
    return True, ", ".join(notes)


def face14i_tol(la, lb, drift, gate):
    """the process-order read rule (14i) interim (a) (pairs; asked on T-03 by):
until 14i's frame-start read is in, a converted reader (Link) facing a converted target LATER in
    the process order sees at N steps the target after its first step (f - 0.5), at 30 Hz its previous frame's end: B's
    Link can face a target eye A's Link only sees a frame later (frame 50: the Bokoblin's jump-start eye; A
    -30531, B -29889 = atan to B's own eye -29884). The 6f form needs the same target in both runs, which this is not.
    Frame -> the angle tolerance that lets the A-vs-B gap through (B checked against its own eye instead) on the lock-on slot (trace columns
    33-35, setShapeAngleToAtnActor's cLib_targetAngleY(pos, eyePos)) where:
      - a turn: either run's angle changed within proc_slack frames before,
      - every angle (shape, current) that differs by more than the plain tolerance is within the angle tolerance of its
        run's OWN recorded eye direction in both runs (B: the rule's check; A: the reference faces its own eye),
      - Link's position gap is within its 6c bound at the frame or in link-tslack's window,
      - the recorded eye is inside the enemy's own gates: the target's gate report(s) given with --target-gate all PASS
        (`gate` = target_gate_ok()'s ok; compare60 has no gate of its own for an enemy's eye).
    `--tol face14i=0` turns it off (and does once 14i's frame-start read is converted)."""
    out = {}
    if not gate:
        return out
    for f in sorted(lb):
        x, y = la.get(f), lb.get(f)
        if x is None or y is None or not y["dt"] < 1.0:
            continue
        ta, tb = (x.get("tgt") or [None])[0], (y.get("tgt") or [None])[0]
        if ta is None or tb is None:
            continue
        turned = any((la.get(f - k) and la.get(f - k - 1) and la[f - k]["ang"] != la[f - k - 1]["ang"]) or
                     (lb.get(f - k) and lb.get(f - k - 1) and lb[f - k]["ang"] != lb[f - k - 1]["ang"]) for k in range(TOL["proc_slack"] + 1))
        if not turned:
            continue
        lim = TOL["pos"] + drift.get(f, 0.0)
        gap = max(abs(p - q) for p, q in zip(x["pos"], y["pos"]))
        if not gap <= lim:
            dd = pos_slack_dist(la, lb, f)
            if dd is None or not dd <= lim:
                continue
        ea = atan2s(ta[0] - x["pos"][0], ta[2] - x["pos"][2])
        eb = atan2s(tb[0] - y["pos"][0], tb[2] - y["pos"][2])
        ks = [k for k in (0, 1) if angdiff(x["ang"][k], y["ang"][k]) > TOL["angle"]]
        # B's angle checked against B's own eye direction within the angle tolerance (A's against A's, the reference
        # facing its own eye); the term then lets the A-vs-B gap through
        if ks and all(angdiff(y["ang"][k], eb) <= TOL["angle"] and angdiff(x["ang"][k], ea) <= TOL["angle"] for k in ks):
            out[f] = max(angdiff(x["ang"][k], y["ang"][k]) for k in ks)
    return out


def turn_accel_tol(la, lb, ramp):
    """the turn-factor sampling rule (6g) (6c class): frame -> the extra speed tolerance of
    setSpeedAndAngleNormal's per-step turn factor while turning and accelerating.

    The acceleration scales with cos(target - angle): at 30 Hz the frame-end angle for the whole frame, at N steps each
    step's own (step 0 only part turned), so B's mNormalSpeed may lag A's by the change of A's acceleration within the
    frame. G(f) = ramp x sum max(0, dv_A(f) - max(0, dv_A(f-1))) over A's accelerating frames (dv_A = A's mNormalSpeed
    change per frame), summed on frames where A also turns; reset (0) at A's speed clamp (the next frame keeps the
    speed: the plateau, which both runs reach) and after >= proc_slack frames without acceleration (no build-up across a
    route). Only where B runs at N steps. Bounded: the sum of increments never exceeds A's largest dv of the ramp."""
    out, acc, idle = {}, 0.0, 0
    for f in sorted(la):
        x, p, n, y = la[f], la.get(f - 1), la.get(f + 1), lb.get(f)
        if p is None or y is None or not y["dt"] < 1.0:
            acc, idle = 0.0, 0
            continue
        dv = x["ns"] - p["ns"]
        if dv <= 1e-3:
            idle += 1
            if idle >= TOL["proc_slack"]:
                acc = 0.0
            if acc > 0.0:
                out[f] = ramp * acc
            continue
        idle = 0
        if n is not None and abs(n["ns"] - x["ns"]) <= 1e-3:
            acc = 0.0  # the clamp: the plateau starts here
            continue
        pp = la.get(f - 2)
        dvp = max(0.0, p["ns"] - pp["ns"]) if pp is not None else 0.0
        if angdiff(x["ang"][0], p["ang"][0]) > 0 or angdiff(x["ang"][1], p["ang"][1]) > 0:
            acc += max(0.0, dv - dvp)
        if acc > 0.0:
            out[f] = ramp * acc
    return out


SHIP_OFF = {"pos": 0x314, "move": 0x322, "facing": 0x32A, "speedF": 0x370, "mode": 0x635, "flags": 0x644, "rope": 0x686, "waterY": 0x6D8, "fwd": 0x6E0}
SHIP_FLY = 0x1  # daSFLG_FLY_e (mStateFlag 0x644): the ship is in the air (a jump, a tornado or warp flight)  # daShip_c (d_a_ship.h)


def ship_rows(path, fr, end):
    """frame -> the ship's state at its full step (WWHD_ACTOR_DUMP=ship.admp:0247AE78:0x720, the profile execute):
    pos, current.angle.y (move), shape_angle.y (facing), speedF, mFwdVel, the current mode (0x635), mWaterY, the record's dt
    and "steps": the mode and speedF of each of the frame's step records in order"""
    out = {}
    if not os.path.exists(path):
        return out
    data = open(path, "rb").read()
    o = 0
    while o + 28 <= len(data) and data[o:o + 4] == b"ADMP":
        step, full, dt, proc, size = struct.unpack_from("<QIfII", data, o + 4)
        body = data[o + 28:o + 28 + size]
        o += 28 + size
        if len(body) < size or size < 0x6E4:
            break
        f = fr.frame(step)
        if f is None or (end is not None and f > end):
            continue
        r = out.setdefault(f, {"dt": dt, "steps": []})
        r["dt"] = min(r["dt"], dt)
        # the frame's step records in order (the early ship-decision rule (6i(i)): the step a decision is made on)
        r["steps"].append({"mode": body[SHIP_OFF["mode"]], "speedF": struct.unpack_from(">f", body, SHIP_OFF["speedF"])[0],
                           "rope": struct.unpack_from(">h", body, SHIP_OFF["rope"])[0]})
        r.update(pos=list(struct.unpack_from(">fff", body, SHIP_OFF["pos"])), move=struct.unpack_from(">h", body, SHIP_OFF["move"])[0],
                 facing=struct.unpack_from(">h", body, SHIP_OFF["facing"])[0], speedF=struct.unpack_from(">f", body, SHIP_OFF["speedF"])[0],
                 fwd=struct.unpack_from(">f", body, SHIP_OFF["fwd"])[0], mode=body[SHIP_OFF["mode"]],
                 fly=bool(struct.unpack_from(">I", body, SHIP_OFF["flags"])[0] & SHIP_FLY), waterY=struct.unpack_from(">f", body, SHIP_OFF["waterY"])[0],
                 rope=struct.unpack_from(">h", body, SHIP_OFF["rope"])[0])
    return out


def ship_turn_tol(sa, sb, ramp, turn_k=1.0):
    """the ship-drift rule (6h) (6c / 6g class): frame -> the extra xz position
    tolerance of the ship's drift. At 30 Hz the frame moves along the frame-end heading (setMoveAngle runs before the
    move) on the frame-end speed, at N steps each step along its own heading (the arc) on its own speed (6c); the gap a
    frame adds is (1 - dt_B)/2 x dv_A along the path and (1 - dt_B)/2 x |v_A| x dheading_A across it (v = speedF,
    heading = current.angle.y in radians). Hold form: the offset a ramp or turn leaves persists
    on the steady stretch after it (both runs then move alike), so neither sum resets; both are SIGNED, so an
    accelerate-then-decelerate or a turn-then-opposite-turn cancels as it does in the game, and the tolerance stays
    bounded by the net speed change and the net heading change since the conversion: ramp x (|sum dv_A| +
    |sum |v_A| dheading_A|). Only frames where B runs at N steps (dt < 1) add.
    Speed part per episode (pair: a start then a stop cancelled in the signed sum, but
    the offsets do not: a one-frame speed step leaves 5.7, a smooth ramp of the same size 10.3): an episode is a
    monotone stretch of A's speed (frames with dv_A = 0 continue it); the speed part is ramp x the sum over episodes of
    |net dv_A of the episode| (unsigned across episodes, the running episode counted with its net so far). A long cruise
    at a steady speed adds nothing. The heading part stays signed.
    turn_k (--tol ship_turn_k, default 2): B's heading also lags A's by (1 - dt_B)/2 frame of turn while the
    tiller ramps (cLib_addCalcAngleS on mTillerAngle, the 6c ramp form on the turn rate: measured lag / turn rate median
    0.242 over 135 turning frames of ps3 and pp1), which displaces B across the path by another (1 - dt_B)/2 x sum |v_A|
    dheading_A (same sign, cancels with the opposite turn): turn_k 2 covers the arc and that lag."""
    out, sdone, ep, sgn, st = {}, 0.0, 0.0, 0, 0.0
    for f in sorted(sa):
        x, p, y = sa[f], sa.get(f - 1), sb.get(f)
        if p is not None and y is not None and y["dt"] < 1.0:
            dv = x["speedF"] - p["speedF"]
            g = (dv > 0) - (dv < 0)
            if g != 0 and sgn != 0 and g != sgn:  # A's speed turns: the episode ends
                sdone += abs(ep)
                ep = 0.0
            if g != 0:
                sgn = g
            ep += dv
            st += ((x["move"] - p["move"] + 32768) % 65536 - 32768) * math.pi / 32768.0 * abs(x["speedF"])
        out[f] = ramp * (sdone + abs(ep) + turn_k * abs(st))
    return out


def ship_switches(rows):
    """[(frame, kind)] of a run's ship decisions: a mode change (kind ("mode", old, new), daShip_c 0x635) and a speed
    switch between stopped and moving (kind ("move", moving); moving = |speedF| >= the speed tolerance)"""
    out, prev = [], None
    for f in sorted(rows):
        r = rows[f]
        mv = abs(r["speedF"]) >= TOL["speed"]
        if prev is not None:
            if r["mode"] != prev["mode"]:
                out.append((f, ("mode", prev["mode"], r["mode"])))
            if mv != (abs(prev["speedF"]) >= TOL["speed"]):
                out.append((f, ("move", mv)))
        prev = r
    return out


def ship_shift_tol(sa, sb, dtb):
    """the early ship-decision rule (6i) (sail-up pairs): a ship decision that B
    makes up to proc_slack frames EARLIER than A (the same mode change or stopped/moving switch, B's frame fb, A's fa,
    0 < fa - fb <= proc_slack) is the legitimate whole-frame shift of the sail-up: 0.5 frame from the first-step input rule (12a) (the
    converted rider's input on the frame's first step) plus 0.5 from the process order (the ship reads the request on
    the next step of the same frame instead of the next frame). Returns (shifts, plaus, term): the matched shifts
    [(fb, fa, kind)], the frames whose speed check is plausibility only (within proc_slack of either switch frame),
    and frame -> the extra xz tolerance (1 - dt_B) x max |v_A| over [fb, max(f, fa)] (B's motion started up to a whole
    frame early, so it leads by at most a frame of A's speed; never more than (1 - dt_B) x max |v_A| of the route).
    The term starts at fb and ends when the ship stops: |v_A| below the speed tolerance for >= proc_slack frames from
    fa on. No detected shift: no term (A's switch first, or more than proc_slack apart: the mode check fails).
    6i(i) (pair): a decision B makes on a NON-LAST step of the same record frame as A
    (fa == fb; the ship executes before the unconverted rider, so B reads the rider's tick30 request on the next frame's
    first step and acts on its later steps) is a shift of (1 - step fraction), step fraction = (k + 1) / n for B's
    step k of the frame's n step records: the same treatment with that factor in place of (1 - dt_B) (both 0.5 at N 2
    on the first step). Mode decisions only: a stopped/moving switch on a non-last step of the same frame is the start
    or end of a ramp the steps take on their own speeds (6c, covered by 6h's speed part), not a decision. Returns also
    frame -> the factor of the term running there (the 6i speed form's factor).
    6i(ii) (crane pairs): the symmetric case, B LATER: an
    unconverted driver (the event system's DEMO_SALVAGE) switches a converted procedure (procCraneUp) on the tick30 step,
    so B's earlier steps of that frame still run the old procedure while A runs the new one for the whole frame. Detected
    as the same mode change B makes 1..proc_slack record frames after A (factor 1 - dt_B), or in the same record frame
    with B's first step in the new mode being the frame's last step (factor (n - 1) / n); the same plausibility window
    and xz / speed term as 6i (B trails by up to that share of a frame of A's speed). Shifts are (fb, fa, kind, factor,
    "earlier" | "later"); `--tol ship_later=0` turns 6i(ii) off. Follow-on events whose timing derives from the shifted
    state: ship_followon."""
    sw_a, sw_b = ship_switches(sa), ship_switches(sb)
    used, shifts = set(), []
    for fb, kb in sw_b:
        for i, (fa, ka) in enumerate(sw_a):
            if i not in used and ka == kb and 0 < fa - fb <= TOL["proc_slack"]:
                used.add(i)
                shifts.append((fb, fa, kb, 1.0 - dtb, "earlier"))
                break
            if i not in used and ka == kb and fa == fb and kb[0] == "mode":
                sfrac = ship_switch_step_frac(sb, fb, kb)
                nst = len(sb.get(fb, {}).get("steps") or [])
                if sfrac is not None and sfrac < 1.0:
                    used.add(i)
                    shifts.append((fb, fa, kb, 1.0 - sfrac, "earlier"))
                elif sfrac is not None and nst > 1 and TOL["ship_later"] > 0:
                    # 6i(ii): B's first step in the new mode is the frame's last (tick30) step: its earlier steps still
                    # ran the old procedure (the unconverted driver's decision is seen on the tick30 step), B later by
                    # (n - 1) / n
                    used.add(i)
                    shifts.append((fb, fa, kb, (nst - 1.0) / nst, "later"))
                break
            if i not in used and ka == kb and 0 < fb - fa <= TOL["proc_slack"] and TOL["ship_later"] > 0:
                used.add(i)  # 6i(ii): B a whole record frame later
                shifts.append((fb, fa, kb, 1.0 - dtb, "later"))
                break
    plaus, term, fac = set(), {}, {}
    frames = sorted(sa)
    for fb, fa, kind, k, _ in shifts:
        lo, hi = min(fa, fb), max(fa, fb)
        plaus.update(range(lo - TOL["proc_slack"], hi + TOL["proc_slack"] + 1))
        vmax = max([abs(sa[g]["speedF"]) for g in range(lo, hi + 1) if g in sa] or [0.0])
        still = 0
        for f in frames:
            if f < lo:
                continue
            if f > hi:
                vmax = max(vmax, abs(sa[f]["speedF"]))
            if f >= fa:
                still = still + 1 if abs(sa[f]["speedF"]) < TOL["speed"] else 0
                if still >= TOL["proc_slack"]:
                    break
            if k * vmax >= term.get(f, 0.0):
                term[f] = k * vmax
                fac[f] = max(fac.get(f, 0.0), k)
    return shifts, plaus, term, fac


SHIP_POS_MODES = {7}  # the early ship-decision rule (6i(iii)): modes whose stopped/moving switch is a decision on position (MODE_DEMO_e: procZevDemo's DEMO_MOVE arrival radius)


def ship_pos_shifts(sa, sb, shifts):
    """the early ship-decision rule (6i(iii)) (proposal, pair): a ship stopped/moving
    switch that is a decision on POSITION (procZevDemo's DEMO_MOVE arrival radius: targetDistance < 10 -> speedF 0 +
    cutEnd; the ship in SHIP_POS_MODES on the frame before the switch in both runs) may come k frames later or earlier
    in B, k <= ceil(|xz B - A| / |v_A|) measured on the frame before A's switch (the drift the xz check already accepts,
    in frames of A's speed; when B switches first: on the frame before B's switch, derived 2026-11-18: by
    fa - 1 B already rests and A has closed the gap, which would refuse any B-earlier case). Speed is plausibility only (finite) between the two switch frames [min(fa, fb), max(fa, fb)];
    the rest positions must agree within pos: a stop's first frame with both stopped (max(fa, fb)), a start's last frame
    with both at rest (min(fa, fb) - 1). Pairs only the stopped/moving switches the 6i / 6i(ii) matching left unpaired;
    a same-frame pair (fb == fa) is consumed first and never re-paired (stricter form).
    Returns (pshifts, plaus, bad): pshifts [(fb, fa, kind, kmax, dxz, vA, rest frame, rest |xz|)], plaus the speed
    plausibility frames, bad [(frame, text)] (a rest position apart by more than pos). `--tol ship_pos_shift=0` off."""
    if TOL["ship_pos_shift"] <= 0:
        return [], set(), []
    sw_a, sw_b = ship_switches(sa), ship_switches(sb)
    done_a = set((fa, k) for fb, fa, k, _, _ in shifts)
    done_b = set((fb, k) for fb, fa, k, _, _ in shifts)
    ua = [(f, k) for f, k in sw_a if k[0] == "move" and (f, k) not in done_a]
    ub = [(f, k) for f, k in sw_b if k[0] == "move" and (f, k) not in done_b]
    used, out, plaus, bad = set(), [], set(), []
    # stricter pairing (6i(iii) review): a B switch on the SAME frame as an A switch of the same
    # kind is that switch's pair (k 0, no shift; the xz gate (pos + 6h drift) judges the positions as on any frame,
    # pair: the 387-456 deceleration stops on 451 in both). Both are consumed first, so neither can pair with a
    # switch on another frame (A's same-frame switch never takes a later B switch, nor B's an earlier A switch)
    same_a = set()
    for ia, (fa, ka) in enumerate(ua):
        for i, (fb, kb) in enumerate(ub):
            if i not in used and fb == fa and kb == ka:
                used.add(i)
                same_a.add(ia)
                break
    for ia, (fa, ka) in enumerate(ua):
        if ia in same_a or sa.get(fa - 1, {}).get("mode") not in SHIP_POS_MODES:
            continue
        for i, (fb, kb) in enumerate(ub):
            if i in used or kb != ka or sb.get(fb - 1, {}).get("mode") not in SHIP_POS_MODES:
                continue
            # the frame before the first of the two switches (A's when A switches first, as in the rule's when B
            # switches first, B already rests there by fa - 1 and A has closed the gap, so the drift is read before fb)
            fm = min(fa, fb) - 1
            pa, pb_ = sa.get(fm), sb.get(fm)
            if pa is None or pb_ is None:
                continue
            dxz = math.hypot(pa["pos"][0] - pb_["pos"][0], pa["pos"][2] - pb_["pos"][2])
            va = abs(pa["speedF"])
            if not (math.isfinite(dxz) and math.isfinite(va)) or va < TOL["speed"]:
                continue
            kmax = int(math.ceil(dxz / va - 1e-9))
            if abs(fb - fa) > kmax:
                continue
            used.add(i)
            lo, hi = min(fa, fb), max(fa, fb)
            plaus.update(range(lo, hi + 1))
            rf = hi if not ka[1] else lo - 1
            ra, rb = sa.get(rf), sb.get(rf)
            rd = math.hypot(ra["pos"][0] - rb["pos"][0], ra["pos"][2] - rb["pos"][2]) if ra and rb else float("inf")
            if not rd <= TOL["pos"]:
                bad.append((rf, "6i(iii) rest position |xz B - A| %.4g over pos %g (%s B %d / A %d)" % (
                    rd, TOL["pos"], "start" if ka[1] else "stop", fb, fa)))
            out.append((fb, fa, ka, kmax, dxz, va, rf, rd))
            break
    return out, plaus, bad


SHIP_ROPE_RATE = {11: 1.0}  # segments a frame the rope changes in a mode: MODE_CRANE_UP_e (procCraneUp: incRopeCnt(-1, 20) per frame)
# the ship's rope-timed sound starts: JA_SE_LK_SHIP_CRANE_WORK 0x204D (every frame the rope changes, incRopeCnt),
# CRANE_STOP 0x284E (the rope reaches 20), CRANE_LIFTUP 0x381D (setRopePos: the hook leaves the water) and the wrapper-4
# pointer records of the callers setRopePos (02476B38) and procCraneUp (0247FC0C)
SHIP_ROPE_SE = {("0", "0000204D"), ("0", "0000284E"), ("0", "0000381D"), ("4", "ptr", "02476B38"), ("4", "ptr", "0247FC0C")}
SHIP_ROPE_WORK_SE = {("0", "0000204D"), ("4", "ptr", "0247FC0C")}  # per-frame starts: a longer rope plays more of them


def ship_branch_tol(sa, sb, fs, dtb):
    """the early ship-decision rule (6i) addendum (pairs): the sail's branch change.
    procSteerMove chases the wind power (cLib_addCalc 0.1 / max 2 / min 0.1) while the sail is on and the wind power is
    > 0, else declines by 0.1 a frame (decrementShipSpeed); when the wind power reaches 0 inside a frame (the heading
    crosses the wind's perpendicular) B changes branch on a step, A on the frame: up to (1 - dt_B) of a frame of the
    chase step, a speed offset that persists while both decline. A crossing: a frame where a run starts declining by
    0.1 and the other starts within proc_slack frames (the same frame included). Its window runs from there to the first later frame with
    both runs on the same branch and |speed B - A| <= speed. -> (window frames, speed term, frame -> xz term)"""
    if TOL["ship_branch_step"] <= 0:
        return set(), 0.0, {}
    term = (1.0 - dtb) * TOL["ship_branch_step"]
    dec = lambda r, f: f - 1 in r and abs(r[f]["speedF"] - r[f - 1]["speedF"] + 0.1) < 2e-3
    win, xz, acc, open_ = set(), {}, 0.0, False
    k = TOL["proc_slack"]
    for f in fs:
        da_, db_ = dec(sa, f), dec(sb, f)
        # a branch entry: a run starts declining on f (it did not on f - 1), the other run starts within proc_slack
        # frames (the same frame included: the crossing's share of the frame differs below a frame)
        ea, eb = da_ and not dec(sa, f - 1), db_ and not dec(sb, f - 1)
        if not open_ and (ea or eb):
            other, od = (sb, eb) if ea else (sa, ea)
            if od or any(g in other and dec(other, g) and not dec(other, g - 1) for g in range(f - k, f + k + 1) if g != f):
                open_ = True
                if f - 1 in sa and f - 1 not in win:  # the frame the crossing lies in (its run's last chase step)
                    win.add(f - 1)
                    acc += term
                    xz[f - 1] = acc
        if open_:
            win.add(f)
            acc += term
            if da_ == db_ and abs(sa[f]["speedF"] - sb[f]["speedF"]) <= TOL["speed"] and min(win) < f:
                open_ = False
        if acc:
            xz[f] = acc
    return win, term, xz


def bwin_runs(win):
    out = []
    for f in sorted(win):
        if out and f == out[-1][1] + 1:
            out[-1][1] = f
        else:
            out.append([f, f])
    return [tuple(x) for x in out]


def ship_followon(sa, sb, shifts):
    """the early ship-decision rule (6i(ii)), follow-on events (crane pair): events whose
    timing derives from the state a B-later shift leaves get a DERIVED bound: the state difference x the per-unit rate,
    no flat number. The rope: B's steps before its switch step still ran procCrane (A held: rope +), A's whole frame ran
    procCraneUp (rope -), so B's rope is dR segments longer at the end of the switch frame; procCraneUp winds 1 segment a
    frame (SHIP_ROPE_RATE), so the rope-timed sounds (SHIP_ROPE_SE: work, stop, lift-up) trail by D = ceil(|dR| / rate)
    frames and the longer rope plays up to D more per-frame work starts. [{at, dR, rate, rate_a, mode, D, lo, end, longer}]
    for each B-later mode shift into a mode of SHIP_ROPE_RATE with dR != 0; end = the last frame either run spends in
    that mode + D. rate_a: A's measured median rope change a frame in that mode (a cross-check of the code's rate)."""
    out = []
    for fb, fa, kind, k, dr in shifts:
        if dr != "later" or kind[0] != "mode" or kind[2] not in SHIP_ROPE_RATE:
            continue
        hi = max(fa, fb)
        if hi not in sa or hi not in sb:
            continue
        dR = sb[hi]["rope"] - sa[hi]["rope"]
        if dR == 0:
            continue
        rate = SHIP_ROPE_RATE[kind[2]]
        D = int(math.ceil(abs(dR) / rate - 1e-9))
        m = kind[2]
        last = max([f for f in set(sa) | set(sb) if f >= hi and ((f in sa and sa[f]["mode"] == m) or (f in sb and sb[f]["mode"] == m))] or [hi])
        ch = sorted(abs(sa[f]["rope"] - sa[f - 1]["rope"]) for f in sa if f > hi and f - 1 in sa and sa[f]["mode"] == kind[2]
                    and sa[f - 1]["mode"] == kind[2] and sa[f]["rope"] != sa[f - 1]["rope"])
        out.append({"at": hi, "dR": dR, "rate": rate, "rate_a": ("%g" % ch[len(ch) // 2]) if ch else "-", "mode": kind[2], "D": D,
                    "lo": min(fa, fb), "end": last + D, "longer": "B" if dR > 0 else "A"})
    return out


CRANE = {}  # rule: the crane / salvage plausibility window {"lo", "D"} (set by crane_window in main)
SHIP_CRANE_UP = 11  # MODE_CRANE_UP_e (daShip_c 0x635)


def crane_window(A, B, fa, fb, end, ship_name="ship.admp"):
    """: the crane salvage sequence (the ship's crane-up and the DEMO_SALVAGE
    event after it) is driven by the unconverted event system: plausibility only until. {"lo": first frame of the
    window, "D": the derived rope bound} or {} without a crane-up in either run's ship dump"""
    sa = ship_rows(os.path.join(A, ship_name), fa, end)
    sb = ship_rows(os.path.join(B, ship_name), fb, end)
    sf = sorted(set(sa) & set(sb))
    ups = [f for f in sf if sa[f]["mode"] == SHIP_CRANE_UP or sb[f]["mode"] == SHIP_CRANE_UP]
    if not ups:
        return {}
    shifts = ship_shift_tol(sa, sb, min(sb[f]["dt"] for f in sf))[0]
    fol = ship_followon(sa, sb, shifts) if TOL["ship_later"] > 0 else []
    return {"lo": min(ups) - TOL["proc_slack"], "D": max([w["D"] for w in fol] or [0])}


def in_crane(f):
    return bool(CRANE) and f >= CRANE["lo"]


def crane_periodic_extra(fa, fb, left, sl):
    """rule addendum (2026-11-16, pcr10): inside the crane window a periodic sound's count may
    differ by at most one. fa / fb: one key's start frames in the window (A / B), left: the key's unmatched starts there
    (either run). True when exactly one start is left, the run with it has exactly one more start, the shorter run has
    at least two (a spacing), the extra start is the longer run's first or last, the other starts pair in order within
    sl, and every interval (the extra's included, against the shorter run's interval at that end) keeps the shorter run's
    spacing within sl"""
    if len(left) != 1:
        return False
    fa, fb = sorted(fa), sorted(fb)
    lng, sht = (fa, fb) if len(fa) > len(fb) else (fb, fa)
    x = left[0]
    if len(lng) != len(sht) + 1 or len(sht) < 2 or x not in lng or x not in (lng[0], lng[-1]):
        return False
    rest = list(lng)
    rest.remove(x)
    if any(abs(p - q) > sl for p, q in zip(rest, sht)):
        return False
    iv = lambda fs: [b - a for a, b in zip(fs, fs[1:])]
    ir, isht = iv(rest), iv(sht)
    if any(abs(p - q) > sl for p, q in zip(ir, isht)):
        return False
    gap, ref = (lng[1] - lng[0], isht[0]) if x == lng[0] else (lng[-1] - lng[-2], isht[-1])
    return abs(gap - ref) <= sl


def crane_slack(*fs):
    """proc_slack / se_slack extension inside the crane window: + D"""
    return CRANE["D"] if any(in_crane(f) for f in fs) else 0


CARRIER_PLAUS = set()  # the carrier-offset rule (6j), derived camera part: the carrier's 6i plausibility frames (set by carrier_offsets)
SHIP_RIDE_PROCS = range(0x88, 0x90)  # Link on board: daPyProc_SHIP_STEER .. SHIP_CRANE (steer, paddle, scope, boomerang, hookshot, bow, cannon, crane)


def ship_y_band(sa, sb, sf):
    """ as a band for B's ship y: frame -> (lo, hi), the y values of B's ship the ship gate's y
    plausibility check accepts at that frame (the same terms): on the water B's own mWaterY + A's float-offset range
    (y - mWaterY over A's water frames) +- pos; in flight (daSFLG_FLY_e) B's own take-off height + A's height range over
    the paired (n-th) flight +- pos. Frames without a finite band (no A range, not finite) are left out."""
    def flights(rows):
        out, cur = [], None
        for f in sf:
            if rows[f]["fly"]:
                if cur is None:
                    cur = (rows[f - 1]["pos"][1] if f - 1 in rows else rows[f]["pos"][1], [])
                    out.append(cur)
                cur[1].append(f)
            else:
                cur = None
        return out
    oa = [sa[f]["pos"][1] - sa[f]["waterY"] for f in sf if not sa[f]["fly"] and math.isfinite(sa[f]["pos"][1] - sa[f]["waterY"])]
    band = {}
    for (ya0, fa_), (yb0, fb_) in zip(flights(sa), flights(sb)):
        ra = [sa[f]["pos"][1] - ya0 for f in fa_ if math.isfinite(sa[f]["pos"][1] - ya0)]
        if ra and math.isfinite(yb0):
            for f in fb_:
                band[f] = (yb0 + min(ra) - TOL["pos"], yb0 + max(ra) + TOL["pos"])
    for f in sf:
        wb = sb[f]["waterY"]
        if not sb[f]["fly"] and oa and math.isfinite(wb):
            band[f] = (wb + min(oa) - TOL["pos"], wb + max(oa) + TOL["pos"])
    return band


def sea_slopes(sa, sf, term):
    """rule (c) addendum, derived slope bound (pairs): frame -> s_A, the
    sea's local height change per unit of distance measured on A's own trace, for the frames of each 6i window (maximal
    runs of consecutive frames in `term`, where a 6i term lets B's ship be elsewhere): the max over the window of
    |dmWaterY_A| / |dxz_A| between consecutive frames of A on the water. Floor: a pair where A moves less than pos in xz
    is left out (its height change is the wave's change in time, not a slope); a window with no such pair takes its
    pairs with |dxz_A| clamped to pos. Outlier: when the max exceeds twice the window's 95th percentile (>= 20 pairs),
    the 95th percentile is used."""
    out = {}
    fs = sorted(f for f in sf if f in term)
    wins, cur = [], []
    for f in fs:
        if cur and f != cur[-1] + 1:
            wins.append(cur)
            cur = []
        cur.append(f)
    if cur:
        wins.append(cur)
    for w in wins:
        pairs = []
        for f in w:
            if f - 1 not in sa or sa[f]["fly"] or sa[f - 1]["fly"]:
                continue
            dw = abs(sa[f]["waterY"] - sa[f - 1]["waterY"])
            dxz = math.hypot(sa[f]["pos"][0] - sa[f - 1]["pos"][0], sa[f]["pos"][2] - sa[f - 1]["pos"][2])
            if math.isfinite(dw) and math.isfinite(dxz):
                pairs.append((dw, dxz))
        r = [dw / dxz for dw, dxz in pairs if dxz >= TOL["pos"]]
        if not r:
            r = [dw / TOL["pos"] for dw, _ in pairs]
        if not r:
            continue
        r.sort()
        s = r[-1]
        if len(r) >= 20:
            p95 = r[min(len(r) - 1, int(math.ceil(0.95 * len(r))) - 1)]
            if s > 2.0 * p95:
                s = p95
        for f in w:
            out[f] = s
    return out


def carrier_offsets(A, B, fa, fb, end, la, lb, ship_name="ship.admp"):
    """the carrier-offset rule (6j) (pair): while Link rides a carrier (the ship:
    his procedure in SHIP_RIDE_PROCS in both runs) link-tslack and cam-tslack inherit the carrier's offset, as the
    camera centre takes Link's 6c offset. frame -> the carrier's actual frame-end offset vector (B - A ship position),
    its xz part capped in length by the ship gate's allowed xz bound at that frame (pos + 6h drift + 6i term + the 6i
    addendum's branch-change term), its y part
    by rule (c)'s plausibility band (ship_y_band): the share of B's ship y
    the ship gate accepts, B's y clamped into the band, never more than the actual y offset nor of the other sign
    (|dy| <= pos without a band); the rider's positions
    (Link, eye, centre) have it subtracted before their own tolerances. Ends when Link leaves the carrier. Empty without
    a ship dump (no carrier, no term)."""
    SHIP_ALLOW.clear()
    sa = ship_rows(os.path.join(A, ship_name), fa, end)
    sb = ship_rows(os.path.join(B, ship_name), fb, end)
    sf = sorted(set(sa) & set(sb))
    if not sf:
        return {}
    dtb = min(sb[f]["dt"] for f in sf)
    sramp = TOL["ship_ramp"] if TOL["ship_ramp"] >= 0 else (1.0 - dtb) / 2.0
    drift = ship_turn_tol(sa, sb, sramp, TOL["ship_turn_k"])
    _, splaus, sterm, _ = ship_shift_tol(sa, sb, dtb)
    _, _, bxz = ship_branch_tol(sa, sb, sf, dtb)  # the 6i addendum's xz term is part of the ship gate's bound too
    yband = ship_y_band(sa, sb, sf)
    out = {}
    CARRIER_PLAUS.clear()
    CARRIER_PLAUS.update(splaus)
    CARRIER_INFO.clear()
    SHIP_ALLOW.clear()
    for f in sf:
        SHIP_ALLOW[f] = (TOL["pos"] + drift.get(f, 0.0) + sterm.get(f, 0.0) + bxz.get(f, 0.0),
                         math.dist(sb[f]["pos"], sa[f]["pos"]), sa[f]["pos"], sb[f]["pos"])
        x, y = la.get(f), lb.get(f)
        if x is None or y is None or not (x["proc"].isdigit() and y["proc"].isdigit()):
            continue
        if int(x["proc"]) not in SHIP_RIDE_PROCS or int(y["proc"]) not in SHIP_RIDE_PROCS:
            continue
        d = [sb[f]["pos"][i] - sa[f]["pos"][i] for i in range(3)]
        # xz: the ship gate's xz bound
        n = math.hypot(d[0], d[2])
        cap = TOL["pos"] + drift.get(f, 0.0) + sterm.get(f, 0.0) + bxz.get(f, 0.0)
        dfac = ((sb[f]["facing"] - sa[f]["facing"] + 32768) % 65536 - 32768) * math.pi / 32768
        # the ship gate on this frame (xz within its bound, facing within the angle tolerance): the camera's ship term
        # (ship_cam) is derived from accepted drift only, so it applies only where the ship itself passes
        CARRIER_INFO[f] = (cap, n <= cap and abs(dfac) * 32768 / math.pi <= TOL["angle"], dfac, max(abs(d[0]), abs(d[2])))
        if n > cap > 0:
            d[0], d[2] = d[0] * cap / n, d[2] * cap / n
        elif cap <= 0:
            d[0] = d[2] = 0.0
        # y: rule (c)'s plausibility band
        dy = d[1]
        if f in yband and math.isfinite(dy):
            lo, hi = yband[f]
            dc = min(max(sb[f]["pos"][1], lo), hi) - sa[f]["pos"][1]
            d[1] = 0.0 if dc * dy <= 0 else dc if abs(dc) < abs(dy) else dy
        else:
            d[1] = max(-TOL["pos"], min(TOL["pos"], dy)) if math.isfinite(dy) else 0.0
        out[f] = d
    return out


def carrier_bg_offsets(A, B, fa, fb, end, conv, dtb):
    """the carrier-offset rule (6j), moving-background carriers: while the camera finds the
    same moving BG actor under the player in both runs (cam_movebg, same process name), the rider (Link, eye, centre)
    inherits the carrier's actual frame-end offset (B - A carrier position), capped in length by
    pos + (1 - dt_B) x |v_A| if the carrier's profile is converted (its own half step: B's carrier may lead or trail A's
    by up to that share of a frame of its motion; v_A = A's carrier position change over the frame) and by pos alone if
    it is unconverted (it runs on tick30 in both runs). frame -> (offset vector, cap, name). `--tol carrier_bg=0` off."""
    mva, mvb = cam_movebg(A, fa, end), cam_movebg(B, fb, end)
    out = {}
    for f in sorted(set(mva) & set(mvb)):
        a, b = mva[f], mvb[f]
        if not a or not b or a[0] != b[0]:
            continue
        name = a[0].upper().zfill(4)
        conv_c = conv == "all" or name in conv
        pa, na = mva.get(f - 1), mva.get(f + 1)  # A's carrier motion over the frame (the next one's on its first frame)
        va = math.dist(a[1], pa[1]) if pa and pa[0] == a[0] else math.dist(na[1], a[1]) if na and na[0] == a[0] else 0.0
        cap = TOL["pos"] + ((1.0 - dtb) * va if conv_c else 0.0)
        d = [y - x for x, y in zip(a[1], b[1])]
        n = math.sqrt(sum(v * v for v in d))
        if n > cap > 0:
            d = [v * cap / n for v in d]
        out[f] = (d, cap, name)
    return out


def ship_switch_step_frac(rows, f, kind):
    """the early ship-decision rule (6i(i)): the step fraction (k + 1) / n of the first of frame f's n step records that shows the switch
    `kind` (ship_switches' kinds); None when the frame has no step records or none shows it"""
    st = rows.get(f, {}).get("steps") or []
    for k, r in enumerate(st):
        if kind[0] == "mode" and r["mode"] == kind[2]:
            return (k + 1) / float(len(st))
        if kind[0] == "move" and (abs(r["speedF"]) >= TOL["speed"]) == kind[1]:
            return (k + 1) / float(len(st))
    return None


def runs_of(seq):
    """[(value, first frame)] change points of a frame->value series (frames ascending)"""
    out = []
    for f in sorted(seq):
        if not out or out[-1][0] != seq[f]:
            out.append((seq[f], f))
    return out


# the counter-edge rule (8c) (rope pair): 30 Hz counters of UNCONVERTED
# processes in the save-info block that a converted party's per-step event starts or stops (the event moves by < 1 frame,
# so the counter may count one frame more or less per such start / stop edge). Explicit list, no blanket tolerance:
# (si.bin offset, struct format, one count, what). si.bin = dSv_info_c (dSv_player_c at +0x20).
SAVE_COUNTERS = [
    (0x44, ">f", 0.02, "dSv_player_status_b_c.mTime (dSv_player_c +0x24): the time of day, 0.02 a 30 Hz frame, advanced "
                       "by the unconverted kankyo; stops at a fall-out freeze (rule 2026-11-18, M2 sw61a)"),
    (0x86, ">H", 1, "dSv_player_item_record_c.mTimer (dSv_player_c +0x66): the forest-water timer, decTimer by the "
                 "unconverted meter (d_meter_01.cpp) once per 30 Hz frame; stops at the rope hook's catch (prope10)"),
]
# converted parties' state words in actor dumps (WWHD_ACTOR_DUMP=NAME.admp:FN:SIZE): a change of the word is a state
# change of that party (the counter-edge rule (8c)'s edges, besides Link's procedure changes); --state-word FILE:OFF:FMT adds one
CONV_STATE = {
    "himo2.admp": (0x3F8, ">i", "himo2_class mState (daHimo2_Execute 0216E194; dump size >= 0x3FC)"),
}


def admp_state_changes(path, fr, end, off, fmt):
    """frames at which a dumped state word changes (any process of the dump; the last record per frame and process)"""
    per = collections.defaultdict(dict)
    data = open(path, "rb").read()
    o = 0
    sz = struct.calcsize(fmt)
    while o + 28 <= len(data) and data[o:o + 4] == b"ADMP":
        step, full, dt, proc, size = struct.unpack_from("<QIfII", data, o + 4)
        body = data[o + 28:o + 28 + size]
        o += 28 + size
        f = fr.frame(step)
        if f is None or (end is not None and f > end) or off + sz > len(body):
            continue
        per[proc][f] = struct.unpack_from(fmt, body, off)[0]
    out = set()
    for seq in per.values():
        out |= {f for _, f in runs_of(seq)[1:]}
    return out


# the cloth-simulation rule (6k) (SieFlag pair): a chaotic, visual-only cloth
# simulation (dCloth_packet_c's vertex arrays; flags sie_flag / tori_flag / ghostship) cannot follow the 30 Hz path vertex
# by vertex at a finer step: its per-vertex difference is accepted, and the cloth is checked statistically instead over
# >= CLOTH_MIN_FRAMES frames after the conversion. ONLY cloth vertex arrays (visual); never actor positions or collision.
# NAME.admp -> (vertices, first position array, second position array, speed array, what): the dump of one cloth's
# arrays (WWHD_MEM_DUMP=NAME.admp:ADDR:SIZE, one record before each full-pass execute of Link); --cloth adds one
CLOTH = {
    "cl.admp": (25, 0x0, 0x134, 0x738, "dCloth_packet_c pos[2] / nrm[2] / back nrm[2] / speed (0x134 apart, 5 x 5 "
                                      "vertices; MEM_DUMP size 0x86C), the SieFlag pair sf3's dump"),
}
CLOTH_MIN_FRAMES = 200
CLOTH_SPEED_MEAN = 0.10  # B's mean vertex speed within 10 % of A's
CLOTH_SPEED_P99 = 0.15   # B's p99 vertex speed within 15 % of A's (6k amendment)
CLOTH_SPEED_MAX = 0.30   # B's max vertex speed within 30 % of A's (sanity only: one chaotic sample, noisy)
# 6k mean-position bound (gy13n4o: a fixed 0.25 sd failed an independent 61 s sample at 1.15 x
# through one ~300-frame excursion): per vertex |mean_B - mean_A| <= CLOTH_POS_K x sd_A x sqrt(2 / N_eff), N_eff = window /
# (2 tau_A), tau_A = A's integrated autocorrelation time of the vertex's position (0.5 + the sum of rho(k) for k >= 1 up to
# the first lag with rho(k) <= 0; >= 0.5): the 3-sigma bound of the difference of two independent means of a correlated
# series. 3-D: sd_A = the rms distance from A's mean, rho(k) of the deviation vectors (dot products).
CLOTH_POS_K = 3.0
# 6k pooled amendment: the same bound on the sail mean (the mean over all tracked vertices per
# frame; sd, tau_A and N_eff from A's pooled series): the free-column vertices of a sail have a long tau (ghost-ship 50-155
# frames, N_eff ~6: ~1.7 sd per vertex), so a whole-sail offset is caught by the pole-column vertices plus the pooled mean.
# 6k window (ghost-ship cloth gm*n2 / gx13n*): the mean position of a chaotic flutter is only
# stable over a long window (ghost-ship sail: decorrelation ~100 frames; 270-frame windows of A alone differ as much as
# A from B), so it is evaluated over >= 1800 frames after the conversion; a shorter cloth pair fails ("lengthen the
# pair"). The speed checks keep CLOTH_MIN_FRAMES.
CLOTH_POS_MIN_FRAMES = 1800


def cloth_frames(path, fr, end, nv, p0, p1, sp):
    """frame -> (positions [nv x 3], speeds [nv x 3], any NaN / inf): the last record per frame. The position array the
    frame ended in is the one that equals the other plus the speed x dt (the packet flips between its two arrays on every
    step: A once a frame, B N times)"""
    out = {}
    if not os.path.isfile(path):
        return out
    data = open(path, "rb").read()
    o = 0
    n3 = 3 * nv
    while o + 28 <= len(data) and data[o:o + 4] == b"ADMP":
        step, full, dt, proc, size = struct.unpack_from("<QIfII", data, o + 4)
        body = data[o + 28:o + 28 + size]
        o += 28 + size
        f = fr.frame(step)
        if f is None or (end is not None and f > end) or max(p0, p1, sp) + 4 * n3 > len(body):
            continue
        a, b, s = (struct.unpack_from(">%df" % n3, body, x) for x in (p0, p1, sp))
        bad = any(not math.isfinite(v) for v in a + b + s)
        h = dt if 0 < dt <= 1 else 1.0
        e0 = sum(abs(a[i] - b[i] - s[i] * h) for i in range(n3)) if not bad else 0
        e1 = sum(abs(b[i] - a[i] - s[i] * h) for i in range(n3)) if not bad else 0
        cur = a if e0 <= e1 else b
        out[f] = ([cur[3 * v:3 * v + 3] for v in range(nv)], [s[3 * v:3 * v + 3] for v in range(nv)], bad)
    return out


def convert_frame(B):
    """the scenario frame the test conversion starts at (B's log, port t60.cpp: "test conversion on from scenario time
    T s"), 0 when it runs from the start"""
    for l in lines(B, "log"):
        m = re.search(r"\[t60\] WWHD_T60_TEST_CONVERT_FROM: test conversion on from scenario time ([0-9.]+) s", l)
        if m:
            return int(round(float(m.group(1)) * 30))
    return 0


def iact(dev):
    """the cloth-simulation rule (6k) mean-position bound: the integrated autocorrelation time (frames) of a 3-D series of deviations from
    its mean [(dx, dy, dz)] (consecutive frames): 0.5 + sum of rho(k), k = 1 .. up to the first lag with rho(k) <= 0 (the
    sum capped at the first zero crossing); at least 0.5 (white noise: N_eff = N)"""
    n = len(dev)
    xs = [[d[k] for d in dev] for k in range(3)]
    c0 = sum(x * x for c in xs for x in c)
    if n < 2 or c0 <= 0:
        return 0.5
    tau = 0.5
    for k in range(1, n // 2):
        r = sum(sum(a * b for a, b in zip(c[:-k], c[k:])) for c in xs) / c0
        if r <= 0:
            break
        tau += r
    return tau


def cloth_gate(ca, cb, conv_f, nv):
    """the cloth-simulation rule (6k): (ok, message, details) of one cloth over the frames after the conversion (both runs)"""
    fs = sorted(f for f in set(ca) & set(cb) if f > conv_f)
    nan = [f for f in sorted(set(ca) | set(cb)) if (ca.get(f) or ([], [], False))[2] or (cb.get(f) or ([], [], False))[2]]
    if nan:
        return False, "NaN / inf in the cloth arrays at frame %d (%d frame(s))" % (nan[0], len(nan)), []
    if not fs:
        return None, "no common frames after the conversion (frame %d)" % conv_f, []
    if all(ca[f][0] == cb[f][0] and ca[f][1] == cb[f][1] for f in fs):
        return True, "%d frames after the conversion (frame %d): every vertex equal" % (len(fs), conv_f), []
    if len(fs) < CLOTH_MIN_FRAMES:
        return False, "%d frames after the conversion (frame %d), the statistics need >= %d (lengthen the pair)" % (
            len(fs), conv_f, CLOTH_MIN_FRAMES), []
    norm = lambda u: math.sqrt(u[0] * u[0] + u[1] * u[1] + u[2] * u[2])

    def stats(c):
        sp = [norm(c[f][1][v]) for f in fs for v in range(nv)]
        mean = [[sum(c[f][0][v][k] for f in fs) / len(fs) for k in range(3)] for v in range(nv)]
        sd = [math.sqrt(sum((c[f][0][v][k] - mean[v][k]) ** 2 for f in fs for k in range(3)) / len(fs)) for v in range(nv)]
        sp.sort()
        return sum(sp) / len(sp), sp[-1], sp[max(0, int(math.ceil(0.99 * len(sp))) - 1)], mean, sd
    ma, xa, pa, mpa, sda = stats(ca)
    mb, xb, pb, mpb, sdb = stats(cb)
    det = []
    rel = lambda b, a: abs(b - a) / a if a > 0 else (0.0 if b == a else float("inf"))
    if not rel(mb, ma) <= CLOTH_SPEED_MEAN:
        det.append("mean vertex speed A %.4g / B %.4g: %.1f %% apart (limit %d %%)" % (ma, mb, 100 * rel(mb, ma), 100 * CLOTH_SPEED_MEAN))
    if not rel(pb, pa) <= CLOTH_SPEED_P99:
        det.append("p99 vertex speed A %.4g / B %.4g: %.1f %% apart (limit %d %%)" % (pa, pb, 100 * rel(pb, pa), 100 * CLOTH_SPEED_P99))
    if not rel(xb, xa) <= CLOTH_SPEED_MAX:
        det.append("max vertex speed A %.4g / B %.4g: %.1f %% apart (limit %d %%)" % (xa, xb, 100 * rel(xb, xa), 100 * CLOTH_SPEED_MAX))
    worst = (0.0, -1, 0.0, 0.0, 0.0, 0.0)  # (ratio, vertex, distance, A's sd, N_eff, limit)
    if len(fs) < CLOTH_POS_MIN_FRAMES:
        det.append("mean position: %d frames after the conversion, the check needs >= %d (6k window; lengthen the pair)" % (
            len(fs), CLOTH_POS_MIN_FRAMES))
    neffs = []
    for v in range(nv):
        d = norm([mpb[v][k] - mpa[v][k] for k in range(3)])
        tau = iact([[ca[f][0][v][k] - mpa[v][k] for k in range(3)] for f in fs])
        neff = len(fs) / (2 * tau)
        neffs.append(neff)
        lim = CLOTH_POS_K * sda[v] * math.sqrt(2 / neff)
        r = d / lim if lim > 0 else (0.0 if d <= 1e-3 else float("inf"))
        if r > worst[0] or worst[1] < 0:
            worst = (r, v, d, sda[v], neff, lim)
        if not r <= 1 and len(det) < 6:
            det.append("vertex %d: mean position %.4g apart, limit %.4g (3 x A's sd %.4g x sqrt(2 / N_eff %.1f), tau_A %.1f frames)" % (
                v, d, lim, sda[v], neff, tau))
    # pooled sail mean (6k pooled amendment): the mean over all tracked vertices per frame, the same
    # derived bound (sd, tau and N_eff from A's pooled series): a whole-sail offset shows here even where the single
    # vertices have a low N_eff (long tau, wide per-vertex bound)
    pool = lambda c: [[sum(c[f][0][v][k] for v in range(nv)) / nv for k in range(3)] for f in fs]
    pa_, pb_ = pool(ca), pool(cb)
    pma = [sum(p[k] for p in pa_) / len(fs) for k in range(3)]
    pmb = [sum(p[k] for p in pb_) / len(fs) for k in range(3)]
    psd = math.sqrt(sum((p[k] - pma[k]) ** 2 for p in pa_ for k in range(3)) / len(fs))
    ptau = iact([[p[k] - pma[k] for k in range(3)] for p in pa_])
    pneff = len(fs) / (2 * ptau)
    pd = norm([pmb[k] - pma[k] for k in range(3)])
    plim = CLOTH_POS_K * psd * math.sqrt(2 / pneff)
    pr = pd / plim if plim > 0 else (0.0 if pd <= 1e-3 else float("inf"))
    if not pr <= 1:
        det.append("sail mean (all %d vertices): mean position %.4g apart, limit %.4g (3 x A's sd %.4g x sqrt(2 / N_eff %.1f), "
                   "tau_A %.1f frames)" % (nv, pd, plim, psd, pneff, ptau))
    return not det, ("%d frames after the conversion (frame %d), %d vertices: mean speed A %.4g / B %.4g, p99 A %.4g / B %.4g, "
                     "max A %.4g / B %.4g; "
                     "mean position: worst vertex %d %.4g apart = %.2f x the limit %.4g (3 x A's sd %.4g x sqrt(2 / N_eff), "
                     "N_eff %.1f; N_eff over the vertices %.1f - %.1f); "
                     "sail mean %.4g apart = %.2f x the limit %.4g (sd %.4g, tau_A %.1f, N_eff %.1f)" % (
                         len(fs), conv_f, nv, ma, mb, pa, pb, xa, xb, worst[1], worst[2], worst[0], worst[5], worst[3],
                         worst[4], min(neffs), max(neffs), pd, pr, plim, psd, ptau, pneff)), det

def counter_edges(series):
    """the counter-edge rule (8c): start / stop edges of a counter from its save-dump series [(frame, value)] (frames ascending): the
    counter runs over (f_{k-1}, f_k] when the value changed there; an edge between a running and a still interval (or the
    other way round) lies in [f_{k-1}, f_{k+1}] (per-frame dumps: the frame of the last / first count +-1). -> [(kind, lo, hi)]"""
    run = [(series[k - 1][0], series[k][0], series[k][1] != series[k - 1][1]) for k in range(1, len(series))]
    out = []
    for k in range(1, len(run)):
        if run[k][2] != run[k - 1][2]:
            out.append(("start" if run[k][2] else "stop", run[k - 1][0], run[k][1]))
    return out


def save_frame(name, end):
    """si.bin -> the scenario's end frame; si.bin.T (WWHD_SAVEINFO_AT, T game seconds from the origin) -> round(30 T)"""
    if name == "si.bin":
        return end if end is not None else 10 ** 9
    try:
        return int(round(float(name[len("si.bin."):]) * 30))
    except ValueError:
        return None


def angdiff(a, b):
    d = abs(a - b) % 65536
    return min(d, 65536 - d)


def lerp_ang(a, b, u):
    d = (b - a + 32768) % 65536 - 32768
    return a + d * u


def pos_slack_dist(la, lb, f):
    """the time-slack position distance of B's frame f to A's trajectory (link-tslack's), None without data"""
    if f not in lb:
        return None
    return min((dd for a in (link_at(la, t) for t in slack_times(f)) if a is not None
                for nm, dd in link_dists(a, lb[f]) if nm == "pos"), default=None)


def speed_slack_dist(la, lb, f):
    """the time-slack speed distance of B's frame f to A's trajectory (link-tslack's), None without data"""
    if f not in lb:
        return None
    return min((dd for a in (link_at(la, t) for t in slack_times(f)) if a is not None
                for nm, dd in link_dists(a, lb[f]) if nm == "speed"), default=None)


def event_shift_tol(la, lb, B, fb, end, dtb, drift, shared):
    """the step-resolution threshold rule (6l) (fan-room pairs): threshold
    crossing at step resolution, with the 6l addenda. Returns (frame -> the extra
    position tolerance, the frames whose speed is plausibility only, report lines, frame -> the extra speed tolerance,
    the carry windows).

    An event that fires when a continuously advancing value (an animation frame, a float timer, a distance) crosses a
    strict threshold is tested by B at step resolution and by A only at frame ends (which rounds the crossing up); with a
    the animation-phase lead rule (6d(b)) half-frame lead it can come one whole frame EARLIER in B (frame 102: procFanGlide's opening
    kick when the USEFANB frame passes > 2.0, B at 102's last step, A at 103). Event kinds, inside one procedure of A
    (init: at a shared procedure change):
      jump   Link's speed (speedF, speed.y) jumps in A at frame fa: |J_A| = |v_A(fa) - v_A(fa-1)| >= event_jump and
             >= 2 x the smooth change s of the neighbouring frames; B's same event is the step-to-step speed change J_B
             with |J_B - J_A| <= speed + s; B's event frame gb is the frame of that step;
      slope  (addendum (a)) a threshold on a continuously changing value: A's speed.y changes its slope at fa (procFanGlide's
             gravity switch `speed.y < -gravity`: -2.5 / frame -> -0.5 / frame), |D_A| = |J_A(fa) - J_A(fa-1)| >=
             event_slope with both slopes steady (|J_A(fa-1) - J_A(fa-2)|, |J_A(fa+1) - J_A(fa)| <= 0.05) and no jump;
             B's same event is the step where B's per-frame slope (step change / step dt) changes by D_B with
             |D_B - D_A| <= speed (both slopes steady over their neighbouring steps); gb is that step's frame.
      init   (6l addenda) a jump at a procedure change both runs make, the new
             procedure's init setting the speed (CUT -> CUT_REVERSE: the sword tip's wall check crossed a frame earlier
             in B): the jump's terms; B's matching step must be the step on which B changes to the same procedure.
    The one-frame-early-only rule: gb = fa - c - 1 or (when c = 1) fa - c, c = 1 while an earlier event's carry window
    (below) covers fa (B's timeline already leads by that event's frame), else 0; gb = fa with c = 0: in the window, no
    term; B later than that, or earlier: no term (reported; the event then fails the gates). The speed at gb is
    plausibility only (finite). Terms, K = min(1, (1 - dt_B) + max(0, 2 (1 - dt_B) - tslack)) (the step-resolution
    rounding plus the 6d(b) lead's part outside the window; 1 at N 2 and N 4):
      jump   position + K |J_A| from gb on (the offset carried like 6i's xz term);
      slope  speed + K |D_A| from gb until the speeds converge (the time-slack speed distance within speed on 2
             consecutive frames: A's slope ran up to a frame longer past the threshold than B's), and position + the
             integral of that speed term (K |D_A| per frame up to the speed convergence) from gb on;
    each until A and B converge (on 2 consecutive frames the time-slack position distance is back within pos of its size
    on the frame before B's event, and within pos + the 6c drift: the gap first narrows to 7.7 at frame 108 and
    returns to 10.9: not a convergence) or the next shared procedure change (`shared`: frames where both runs change to
    the same procedure). The event's state sequence stays gated by link-procs. 0 turns it off (--tol event=0).
    Derived speed form (event_dv, 1 by default since the 6l addenda; 0 turns it off): after a jump event one frame
    early B's speed curve is A's led by up to K frames, the window covers tslack of it: the speed tolerance adds
    (K - tslack) x max |dv_A| over frames f-1..f+1 on the jump's term frames after gb (0.75 frame on the
    2.5 / frame deceleration, 0.625 past the window at frames 103-106).
    Carry windows: [(gb, last frame with the term, the shared change that ended it or None)]: addendum (b) (anim switches
    up to 1 frame early or late while carried) and (c) (an unconverged carry at a shared change: plausibility to the
    next shared state) read them."""
    notes, posv, splaus, spv, windows = [], {}, set(), {}, []
    if TOL["event"] <= 0 or dtb >= 1.0:
        return posv, splaus, notes, spv, windows
    ch = state_channel(B, "trace.txt", fb, 1, lambda p: int(p[0]), lambda p: (fnum(p[9]), fnum(p[11]), fnum(p[4]), p[5]), end, ncol=18)
    st = [(f, v) for f in sorted(ch) for _, v in sorted(ch[f].items())]
    vec = lambda r: (r["speedF"], r["speed"][1])
    nrm = lambda u: math.hypot(u[0], u[1])

    def jA(g):
        if g in la and g - 1 in la:
            return tuple(p - q for p, q in zip(vec(la[g]), vec(la[g - 1])))
        return None

    def yA(g):  # A's speed.y change of frame g
        return la[g]["speed"][1] - la[g - 1]["speed"][1] if g in la and g - 1 in la else None

    def same_proc(f0, f1):
        return all(g in la for g in range(f0, f1 + 1)) and len({la[g]["proc"] for g in range(f0, f1 + 1)}) == 1

    K = min(1.0, (1.0 - dtb) + max(0.0, 2.0 * (1.0 - dtb) - TOL["tslack"]))
    # B's per-step slopes of speed.y (per frame: the step's change / its dt)
    bsl = [None] + [((st[i][1][1] - st[i - 1][1][1]) / st[i][1][2]) if st[i][1][2] > 0 else None for i in range(1, len(st))]
    events = []
    for fa in sorted(la):
        if fa not in lb or not lb[fa]["dt"] < 1.0:
            continue
        J = jA(fa)
        # a procedure change whose init sets the speed (6l addenda, the pair's CUT -> CUT_REVERSE
        # at A 77 / B 76: the cut's animation crosses the sword-tip wall check a frame earlier in B): the same jump event,
        # B's matching step must be the step on which B changes to the same procedure
        pchg = J is not None and la[fa]["proc"] != la[fa - 1]["proc"]
        if J is not None and (la[fa]["proc"] == la[fa - 1]["proc"] or pchg):
            s = max([nrm(x) for x in (jA(fa - 1), jA(fa + 1)) if x is not None] or [0.0])
            nJ = nrm(J)
            if nJ >= TOL["event_jump"] and nJ >= 2.0 * s:
                best = None
                for i in range(1, len(st)):
                    g = st[i][0]
                    if not fa - 3 <= g <= fa + 1:
                        continue
                    if pchg and not (st[i][1][3] == la[fa]["proc"] and st[i - 1][1][3] == la[fa - 1]["proc"]):
                        continue
                    Jb = (st[i][1][0] - st[i - 1][1][0], st[i][1][1] - st[i - 1][1][1])
                    if nrm(tuple(p - q for p, q in zip(Jb, J))) <= TOL["speed"] + s and (best is None or nrm(Jb) > best[1]):
                        best = (g, nrm(Jb))
                events.append(("init" if pchg else "jump", fa, nJ, best[0] if best else None))
                continue
            # 6l addendum (g) (257/258 and 261/262): a position-driven speed
            # step (procCutRoll's speedF on the downhill slope: 29.93 -> 28.83 -> 26.27, ground-driven) below
            # event_jump, B one frame earlier while B's position lead is within the +-0.5-frame time slack (the
            # time-slack position distance at B's step frame within pos + the 6c drift): the 6l (b) form outside a
            # carry; jump terms. Only B one frame early; at the same frame no term; otherwise nothing is added here.
            if TOL["event_pos"] > 0 and not pchg and TOL["speed"] < nJ < TOL["event_jump"] and nJ >= 2.0 * s:
                gb = None
                for i in range(1, len(st)):
                    if st[i][0] != fa - 1:
                        continue
                    Jb = (st[i][1][0] - st[i - 1][1][0], st[i][1][1] - st[i - 1][1][1])
                    if nrm(tuple(p - q for p, q in zip(Jb, J))) <= TOL["speed"] + s:
                        gb = fa - 1
                d = pos_slack_dist(la, lb, fa - 1)
                if gb is not None and d is not None and d <= TOL["pos"] + drift.get(fa - 1, 0.0):
                    events.append(("pos", fa, nJ, gb))
                    continue
        if TOL["event_slope"] > 0 and same_proc(fa - 2, fa + 1):
            j0, j1, jm, jp = yA(fa - 1), yA(fa), yA(fa - 2) if fa - 3 in la else None, yA(fa + 1)
            if None in (j0, j1, jp) or max(abs(j0), abs(j1)) >= TOL["event_jump"]:
                continue
            D = j1 - j0
            if abs(D) < TOL["event_slope"] or abs(jp - j1) > 0.05 or (jm is not None and abs(j0 - jm) > 0.05):
                continue
            best = None
            for i in range(3, len(st) - 1):
                g = st[i][0]
                if not fa - 3 <= g <= fa + 1 or None in bsl[i - 2:i + 2]:
                    continue
                Db = bsl[i] - bsl[i - 1]
                if abs(Db - D) <= TOL["speed"] and abs(bsl[i - 1] - bsl[i - 2]) <= 0.05 and abs(bsl[i + 1] - bsl[i]) <= 0.05:
                    best = (g, abs(Db))
                    break
            events.append(("slope", fa, abs(D), best[0] if best else None))
    for kind, fa, mag, gb in events:
        c = 1 if any(w[0] < fa <= w[1] for w in windows) else 0
        if gb is None:
            notes.append("rule note 6l: A's speed %s %.4g at frame %d: no matching step in B within 3 frames: no term" % (kind, mag, fa))
            continue
        if gb == fa and c == 0:
            continue
        if not fa - c - 1 <= gb <= fa - 1:
            notes.append("rule note 6l: A's speed %s %.4g at frame %d, B's at frame %d (%s%s): no term" % (
                kind, mag, fa, gb, "late" if gb > fa - c else "more than one frame early",
                ", carried lead %d" % c if c else ""))
            continue
        splaus.add(gb)
        term = K * mag
        stop = min([x for x in shared if x > fa] or [float("inf")])
        ok_run, endf, lastf, shend = 0, None, gb, None
        d0 = pos_slack_dist(la, lb, gb - 1)
        d0 = 0.0 if d0 is None else d0
        sconv, sok, acc = None, 0, 0.0
        for f in sorted(g for g in lb if g >= gb):
            if f >= stop:
                # converged across the change? (the first two frames past its proc_slack within the convergence bound)
                cv = [pos_slack_dist(la, lb, g) for g in (f + TOL["proc_slack"] + 1, f + TOL["proc_slack"] + 2)]
                if all(d is not None and d <= min(d0 + TOL["pos"], TOL["pos"] + drift.get(g, 0.0)) for d, g in zip(cv, (f + TOL["proc_slack"] + 1, f + TOL["proc_slack"] + 2))):
                    endf = "the shared procedure change at %d (converged across it)" % f
                else:
                    endf, shend = "the shared procedure change at %d (not converged)" % f, f
                break
            if kind == "slope" and sconv is None:
                if f > fa:
                    d = speed_slack_dist(la, lb, f)
                    sok = sok + 1 if d is not None and d <= TOL["speed"] else 0
                    if sok >= 2:
                        sconv = f
                if sconv is None:
                    spv[f] = spv.get(f, 0.0) + term
                    acc += term
            if f > fa:
                d = pos_slack_dist(la, lb, f)
                okp = d is not None and d <= min(d0 + TOL["pos"], TOL["pos"] + drift.get(f, 0.0))
                if kind in ("jump", "init", "pos") and okp:
                    # the jump's derived speed form covers B's leading speed curve: its window
                    # converges when the speeds are back too (the time-slack speed distance within speed; as `slope`)
                    ds = speed_slack_dist(la, lb, f)
                    okp = ds is not None and ds <= TOL["speed"]
                ok_run = ok_run + 1 if okp else 0
                if ok_run >= 2 and (kind in ("jump", "init", "pos") or sconv is not None):
                    endf = "convergence at %d" % f
                    break
            posv[f] = posv.get(f, 0.0) + (term if kind in ("jump", "init", "pos") else acc)
            if kind in ("jump", "init", "pos") and f > gb and TOL["event_dv"] > 0:
                dvl = max([nrm(x) for x in (jA(f - 1), jA(f), jA(f + 1)) if x is not None] or [0.0])
                spv[f] = spv.get(f, 0.0) + TOL["event_dv"] * max(0.0, K - TOL["tslack"]) * dvl
            lastf = f
        windows.append((gb, lastf, shend))
        notes.append("rule note 6l: speed %s %.4g at frame %d in A, frame %d in B (one frame early%s): speed at %d plausibility only, "
                     "%s on frames %d-%d, ended by %s" % (kind, mag, fa, gb, " past a carried lead of 1" if c and gb == fa - 2 else "", gb,
                                                          "position + %.4g (K %.3g)" % (term, K) if kind in ("jump", "init", "pos") else
                                                          "speed + %.4g (K %.3g) to %s, position + its integral (up to %.4g)" % (term, K, "the speed convergence at %d" % sconv if sconv else "the end of the window", acc),
                                                          gb, lastf, endf or "the end"))
    return posv, splaus, notes, spv, windows


def anim_switches(run, fr, end):
    """frame -> number of animation changes (anim_changed between consecutive records, every step) in that frame"""
    ch = state_channel(run, "trace.txt", fr, 1, lambda p: int(p[0]), lambda p: (fnum(p[15]), fnum(p[16]), fnum(p[4])), end, ncol=18)
    out, prev = collections.Counter(), None
    for f in sorted(ch):
        for _, v in sorted(ch[f].items()):
            if prev is not None and anim_changed(prev[0], prev[1], v[2], v[0]):
                out[f] += 1
            prev = v
    return out


def carried_anim_switch(swa, swb, windows):
    """the step-resolution threshold rule (6l) addendum (b) (149 / 165): while a 6l offset is
    carried (a carry window that has not converged), a decision driven by position (entering or leaving a wind capsule,
    the animation switch it causes) may come up to 1 frame early OR late in B; the animation SEQUENCE must still match
    exactly. Per window: A's and B's animation switches (frames, in order) must be as many and pair up within 1 frame;
    then the frames from the earlier to the later switch of each pair are animation plausibility only (finite); the values
    after both switched stay compared (a different new animation fails). Returns (frames, report lines)."""
    out, notes, merged = set(), [], []
    for g0, g1, _ in sorted(windows):  # overlapping windows (an event inside another's carry) as one
        if merged and g0 <= merged[-1][1] + 1:
            merged[-1] = (merged[-1][0], max(merged[-1][1], g1), None)
        else:
            merged.append((g0, g1, None))
    for g0, g1, _ in merged:
        # the window's first frame included (295-297: the window starts on B's early event, whose
        # own procedure init switches the animation there; g0 < f left B's switch out and paired A's 296 with nothing)
        a = sorted(f for f, k in swa.items() if g0 <= f <= g1 for _ in range(k))
        b = sorted(f for f, k in swb.items() if g0 <= f <= g1 for _ in range(k))
        if not a and not b:
            continue
        if len(a) != len(b) or any(abs(x - y) > 1 for x, y in zip(a, b)):
            notes.append("rule note 6l (b): animation switches in the carry window %d-%d differ (A %s, B %s): no slack" % (g0, g1, a[:6], b[:6]))
            continue
        moved = [(x, y) for x, y in zip(a, b) if x != y]
        for x, y in moved:
            out.update(range(min(x, y), max(x, y) + 1))
        if moved:
            notes.append("rule note 6l (b): %d animation switch(es) in the carry window %d-%d, %d a frame apart (A/B %s): plausibility there" % (
                len(a), g0, g1, len(moved), ", ".join("%d/%d" % p for p in moved[:4])))
    return out, notes


def link_at(rows, t):
    """A's Link state at time t (30 Hz frames), linear between its full steps; None without both neighbours. An
    animation loop wrap (the frame drops by more than 5 while the frame at g + 1 is about r0 + rate - L for a loop
    length L above both frames) is interpolated on the unwrapped frame and wrapped back, and the loop length goes with
    the state ("animL", for a distance modulo the loop); any other jump (a new animation) is not interpolated: before
    the midpoint the old animation continued at A's rate of frame g, after it the new animation's value."""
    g = int(math.floor(t))
    u = t - g
    if u == 0.0:
        return rows.get(g)
    r0, r1 = rows.get(g), rows.get(g + 1)
    if r0 is None or r1 is None:
        return None
    li = lambda x, y: x + (y - x) * u
    a0, a1 = r0["anim"], r1["anim"]
    loop = None
    if abs(a1 - a0) <= 5:
        anim = li(a0, a1)
    else:
        rate = r1.get("rate") or 0.0
        L = a0 + rate - a1
        if a1 < a0 and 0 < rate <= 5 and L > a0 and L > a1 and abs(L - round(L)) < 0.02:
            loop = float(round(L))
            anim = li(a0, a1 + loop)
            if anim >= loop:
                anim -= loop
        else:
            # a new animation at g + 1: up to the midpoint the old one plays on at A's rate of frame g (A's trajectory
            # inside the frame; B's phase lead at a proc start, the animation-phase lead rule (6d), shows at the old animation's last frame,
            # where the window has no A frame of the old animation after g: prope3 frame 150, 1.1 at rate 1.1), from the
            # midpoint the new one's first value
            anim = a0 + (r0.get("rate") or 0.0) * u if u < 0.5 else a1
    out = {"pos": [li(x, y) for x, y in zip(r0["pos"], r1["pos"])], "speedF": li(r0["speedF"], r1["speedF"]),
           "speed": [li(x, y) for x, y in zip(r0["speed"], r1["speed"])],
           "ang": [lerp_ang(x, y, u) for x, y in zip(r0["ang"], r1["ang"])], "anim": anim}
    if loop:
        out["animL"] = loop
    return out


def cam_modes(run, fr, end):
    """frame -> camera mode (cam.txt's second line, `mode N`; the last record per frame)"""
    out, f = {}, None
    for l in lines(run, "cam.txt"):
        p = l.split()
        if not p:
            continue
        if p[0].isdigit() and len(p) >= 12:
            f = fr.frame(int(p[1]))
            if end is not None and f is not None and f > end:
                f = None
        elif f is not None and "mode" in p:
            out[f] = p[p.index("mode") + 1]
    return out


def cam_shakes(run, fr, end):
    """frames on which the camera shakes (cam.txt's second line `shake N ...`, N != 0, on any record of the frame): the
    drawn view carries the shake offset, which the eye / centre columns do not"""
    out, f = set(), None
    for l in lines(run, "cam.txt"):
        p = l.split()
        if not p:
            continue
        if p[0].isdigit() and len(p) >= 12:
            f = fr.frame(int(p[1]))
            if end is not None and f is not None and f > end:
                f = None
        elif f is not None and "shake" in p:
            i = p.index("shake")
            if i + 1 < len(p) and p[i + 1] not in ("0", "-"):
                out.add(f)
    return out


def cam_bumps(run, fr, end):
    """frame -> the camera's bump-check state (cam.txt's second line `bump HEX`, when the trace has it; the last record
    per frame); traces without the field: {}"""
    out, f = {}, None
    for l in lines(run, "cam.txt"):
        p = l.split()
        if not p:
            continue
        if p[0].isdigit() and len(p) >= 12:
            f = fr.frame(int(p[1]))
            if end is not None and f is not None and f > end:
                f = None
        elif f is not None and "bump" in p:
            out[f] = p[p.index("bump") + 1]
    return out


def bump_episodes(ca, cb, ja, jb, matched, ba=None, bb=None, tol=2.0, maxlen=30):
    """the per-step camera-collision rule (6b) episode (crawl pair cy: a bumpCheck line hit on B's in-between step leaves
    the cushioned direction on the other side of the view, the wall release 21 frames apart): a bumped-eye radius jump
    (> 1/4 in one frame) that only one run makes (no matching jump in the other within proc_slack: `matched`) marks a
    bump-state difference. The window runs from the first bump-state difference (the earlier of: the start of the
    stretch of frames before the jump where the eyes are more than `tol` apart, and the first frame within `maxlen`
    before the jump where the runs' `bump` fields differ, when the traces have them) until both eyes converge (the first
    frame from the jump on whose eye gap is within `tol`); at most `maxlen` frames, else no window (reported).
    Returns (frames, [(start, jump, end, converged)])."""
    def egap(f):
        return max(abs(p - q) for p, q in zip(ca[f][0:3], cb[f][0:3]))
    frames, eps = set(), []
    for j in sorted(set(ja) | set(jb)):
        if j in matched or j not in ca or j not in cb:
            continue
        if any(s0 <= j < e0 for s0, _, e0, ok in eps):
            continue  # inside an episode already found
        s = j
        while s - 1 in ca and s - 1 in cb and egap(s - 1) > tol and j - (s - 1) <= maxlen:
            s -= 1
        if ba and bb:
            for g in range(j - maxlen, j + 1):
                if g in ba and g in bb and ba[g] != bb[g]:
                    s = min(s, g)
                    break
        e = j
        while e in ca and e in cb and egap(e) > tol and e - s <= maxlen:
            e += 1
        ok = e in ca and e in cb and egap(e) <= tol and e - s <= maxlen
        eps.append((s, j, e, ok))
        if ok:
            frames.update(range(s, e))
    return frames, eps


def cam_movebg(run, fr, end):
    """the carrier-offset rule (6j), moving-background carriers (detection, report): frame -> (process name, [x, y, z]) of the moving BG
    actor under the player that the camera found (cam.txt's second line `mvbg NAME x y z`, port 2026-11-16; `mvbg -`:
    none -> None); frames without the field (older traces) are left out"""
    out, f = {}, None
    for l in lines(run, "cam.txt"):
        p = l.split()
        if not p:
            continue
        if p[0].isdigit() and len(p) >= 12:
            f = fr.frame(int(p[1]))
            if end is not None and f is not None and f > end:
                f = None
        elif f is not None and "mvbg" in p:
            i = p.index("mvbg")
            out[f] = None if p[i + 1] == "-" or len(p) < i + 5 else (p[i + 1], [fnum(x) for x in p[i + 2:i + 5]])
    return out


def cam_types(run, fr, end):
    """frame -> camera type (cam.txt's second line, `type N`; the last record per frame)"""
    out, f = {}, None
    for l in lines(run, "cam.txt"):
        p = l.split()
        if not p:
            continue
        if p[0].isdigit() and len(p) >= 12:
            f = fr.frame(int(p[1]))
            if end is not None and f is not None and f > end:
                f = None
        elif f is not None and "type" in p:
            out[f] = p[p.index("type") + 1]
    return out


# 6l addendum (g) carry (pair frame 131): the per-frame decay of a camera style's
# centre lag = 1 - min(param 3, param 4), its centre's xz / y follow ratios (dCamParam_c style table in the rpx, 0x1004487C,
# 0x84-byte entries, params at +8: fixedPositionCamera's mul (v3, v4, v3), the engine2 cameras' centerFollowY = param 4,
# follow / lock-on / crawl / hookshot read the same two); styles without both ratios in (0, 1] (event, subject, talk,
# fixed frame, ...): no carry
STYLE_CENTRE_KEEP = {}
for _k, _ss in ((0.2, (47,)), (0.75, (1, 2, 3, 5, 21, 27, 28, 34, 35, 38, 40, 43, 48, 49, 50, 56, 57, 58, 59, 60, 61, 62, 65, 66,
                67, 68, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106,
                107, 109, 113, 114, 115, 119, 120, 121, 122, 123, 125, 127, 128, 129, 130, 131, 132, 133, 135, 139, 140, 141,
                142, 143, 144)),
                (0.8, (9, 12, 15, 19, 20, 22, 23, 30, 51, 54, 69, 70, 71, 72, 74, 75, 76, 77, 78, 79, 80, 81, 108, 137)),
                (0.85, (26, 32, 36, 39, 41, 42, 52, 53, 55, 64, 110, 111, 116, 117, 118)), (0.9, (11, 13, 14, 17, 18, 31, 33, 73, 134)),
                (0.95, (10, 16, 24, 37)), (0.97, (124, 126, 138))):
    for _s in _ss:
        STYLE_CENTRE_KEEP[_s] = _k
# the camera styles' centre xz follow ratio (rpx style table 0x1004487C, param 3 at +0x14; read 2026-11-18 from
# cking.rpx with t60conv.rpx_read: rideCamera's centerFollowXZ = camParamVal(style, 3), the follow / lock-on / crawl /
# fixed-position engines read the same word); styles whose param 3 is no ratio in (0, 1] (event, subject, ...): absent
STYLE_CENTRE_R = {}
for _r, _ss in ((0.03, (124, 126, 138)), (0.05, (10, 16, 24, 37)), (0.1, (17, 18, 73)), (0.15, (53, 55, 64)),
                (0.2, (9, 12, 15, 26, 30, 74, 75, 77, 81, 137)), (0.25, (54, 134)), (0.3, (11, 31, 33)),
                (0.4, (22, 23, 69, 70, 71, 72, 76, 78, 79, 80, 108)), (0.5, (21, 35, 56)), (0.55, (116, 117, 118)),
                (0.6, (7, 8, 19, 20, 51, 112)), (0.65, (36,)),
                (0.7, (48, 49, 50, 57, 58, 59, 60, 62, 83, 115, 125, 131, 132, 139, 140, 141, 142, 143)),
                (0.75, (1, 2, 3, 5, 6, 27, 28, 32, 34, 38, 40, 41, 42, 43, 52, 61, 65, 66, 67, 68, 82, 84, 85, 86, 87, 88, 89, 90,
                        91, 92, 93, 94, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 109, 110, 111, 113, 114, 119,
                        120, 121, 122, 123, 127, 128, 129, 130, 133, 135, 144)),
                (0.8, (39, 47, 95)), (0.9, (13, 14))):
    for _s in _ss:
        STYLE_CENTRE_R[_s] = _r


def chase_g_of(r, dtb):
    """the steady chase-lag difference factor of a chase of ratio r per 30 Hz frame (the speed-ramp position-gap rule (6c) camera form): behind a
    target moving v per frame the 30 Hz chase trails by v (1 - r) / r, the N-step one (r_s = 1 - (1 - r)^dt per step) by
    v dt (1 - r_s) / r_s; g = the difference (r 0.75: 1/6 at N = 2; r 0.65: 0.186; r -> 0: 1/2 - dt/2 ... 0.25 at N = 2)"""
    if not (0.0 < r < 1.0) or dtb >= 1.0:
        return 0.0
    r_s = 1.0 - (1.0 - r) ** dtb
    return max(0.0, dtb * (1.0 - r_s) / r_s - (1.0 - r) / r)


def carrier_chase_g(sty, f, dtb):
    """ (ptalk2 912-990, rideCamera style 36): the carrier chase term's g at frame f from A's
    camera styles: in a ride style's entry transition (its first ride_trans frames, mCurStyleTimer t = frames since the
    style started) the centre moves by the transition share k = 1 / (ride_trans - t) (d_camera_engine2 rideCamera's
    default entry), so g = chase_g_of(k); after it, and in any other style, the style's own centre follow ratio (param 3;
    0.75 when the style has none). ride_trans 0: the fixed 0.75 (the earlier form)."""
    T = int(TOL["ride_trans"])
    if T <= 0:
        return chase_g_of(0.75, dtb)
    s = sty.get(f)
    if s in RIDE_STYLES:
        t = 0
        while t < T and sty.get(f - t - 1) == s:
            t += 1
        if t < T:
            return chase_g_of(1.0 / (T - t), dtb)
    return chase_g_of(STYLE_CENTRE_R.get(s, 0.75), dtb)


def cam_dts(run, fr, end):
    """frame -> the smallest dt of the frame's camera records (cam.txt column 4: 1.00 on every record = the camera ran
    unconverted, once a frame)"""
    out = {}
    for l in lines(run, "cam.txt"):
        p = l.split()
        if p and p[0].isdigit() and len(p) >= 12:
            f = fr.frame(int(p[1]))
            if f is None or (end is not None and f > end):
                continue
            out[f] = min(out.get(f, 9.0), fnum(p[3]))
    return out


def cam_dir(v):
    """(yaw, pitch) in radians of the eye seen from the centre"""
    dx, dy, dz = v[0] - v[3], v[1] - v[4], v[2] - v[5]
    return math.atan2(dx, dz), math.atan2(dy, math.hypot(dx, dz))


HOLD_EPS = 4.0 * math.pi / 32768.0  # 4 s16 units: a direction counts as held while it moves less than this a frame


def cam_holds(rows):
    """frame -> the latch frame L of the held direction it is in: maximal runs of frames whose eye direction (seen from
    the centre) moved less than HOLD_EPS from the frame before; L = the frame before the run (the last frame it moved)"""
    out, L = {}, None
    for f in sorted(rows):
        if f - 1 in rows:
            a, b = cam_dir(rows[f - 1]), cam_dir(rows[f])
            if abs(math.remainder(a[0] - b[0], 2 * math.pi)) < HOLD_EPS and abs(a[1] - b[1]) < HOLD_EPS:
                if L is None:
                    L = f - 1
                    out[L] = L  # the latch frame holds the latched direction too
                out[f] = L
                continue
        L = None
    return out


def cam_hold_terms(ca, cb):
    """ (camera, pair frame 135, KoRL's event camera): a camera that latches its
    direction at the end of its move freezes the target's accepted half-frame lead into a held offset that the time
    window no longer covers once the camera stands still. Accepted for the duration of the hold when the held direction
    was latched from a target within the +-tslack window at the latch frame: both runs hold from latch frames at most
    proc_slack apart, and B's held direction lies within A's directions over [L - tslack, L + tslack] around A's latch
    frame L (A's direction at L -+ tslack x its last move dir(L) - dir(L - 1): A latched at L, so its window's far side
    continues that move), per yaw and pitch, plus the base cam tolerance over A's radius. frame -> the eye term
    R_A x the held angle between the two directions (frames where both still hold their latched directions)."""
    ha, hb = cam_holds(ca), cam_holds(cb)
    out, ok_latch = {}, {}
    for f in sorted(set(ha) & set(hb)):
        La, Lb = ha[f], hb[f]
        if abs(La - Lb) > TOL["proc_slack"] or La - 1 not in ca or La not in ca or Lb not in cb:
            continue
        key = (La, Lb)
        if key not in ok_latch:
            ya, pa = cam_dir(ca[La])
            y0, p0 = cam_dir(ca[La - 1])
            yb, pb = cam_dir(cb[Lb])
            R = math.dist(ca[La][0:3], ca[La][3:6])
            slack = TOL["cam"] / R if R > 0 else 0.0
            dy, dp = math.remainder(ya - y0, 2 * math.pi), pa - p0
            def inside(x, c, d):
                lo, hi = c - TOL["tslack"] * abs(d) - slack, c + TOL["tslack"] * abs(d) + slack
                return lo <= x <= hi
            ok_latch[key] = inside(ya + math.remainder(yb - ya, 2 * math.pi), ya, dy) and inside(pb, pa, dp)
        if not ok_latch[key]:
            continue
        (ya, pa), (yb, pb) = cam_dir(ca[f]), cam_dir(cb[f])
        th = math.hypot(math.remainder(yb - ya, 2 * math.pi) * math.cos(pa), pb - pa)
        out[f] = math.dist(ca[f][0:3], ca[f][3:6]) * th
    return out


def ship_attitude(A, B, fa, fb, end):
    """frame -> (|ship pitch B - A| in radians, |ship y B - A|) from the ship dumps (shape_angle.x at 0x328, pos y)"""
    out = {}
    for run, fr, key in ((A, fa, 0), (B, fb, 1)):
        path = os.path.join(run, "ship.admp")
        if not os.path.exists(path):
            return {}
        data = open(path, "rb").read()
        o = 0
        while o + 28 <= len(data) and data[o:o + 4] == b"ADMP":
            step, full, dt, proc, size = struct.unpack_from("<QIfII", data, o + 4)
            body = data[o + 28:o + 28 + size]
            o += 28 + size
            if len(body) < size or size < 0x6E4 or not full:
                continue
            f = fr.frame(step)
            if f is None or (end is not None and f > end):
                continue
            out.setdefault(f, [None, None])[key] = (struct.unpack_from(">h", body, 0x328)[0], struct.unpack_from(">f", body, 0x318)[0])
    res = {}
    for f, (a, b) in out.items():
        if a is None or b is None:
            continue
        dp = abs(((b[0] - a[0] + 32768) % 65536) - 32768) * math.pi / 32768.0
        res[f] = (dp, abs(b[1] - a[1]))
    return res


SHIP_ATT_WINDOW = 30  # frames: the carry window of the chased latitude (as the ship term's polyline)


def ship_att_term(att, f, R, coff):
    """ (930 / talk13 965, ship-attitude class): an unconverted camera following a converted
    ship that sits within the ship gate's accepted drift (6h / 6i) reads the ship's attitude at its (accepted) position:
    rideCamera's target latitude takes 0.3 x the ship's pitch and the sea-wave correction 0.8 x speedRatio x the angle from
    the ship to the sea 200 ahead (a float-y difference dy changes it by atan(dy / 200); speedRatio <= 1). The eye term =
    R x the largest 0.3 |d pitch| + 0.8 atan(|dy| / 200) over the last SHIP_ATT_WINDOW frames on the carrier (the latitude
    is chased, so the eye may carry an earlier frame's difference); dy is the carrier offset's accepted y part."""
    best = 0.0
    for g in range(f - SHIP_ATT_WINDOW, f + 1):
        if g in att and g in coff:
            dy = min(att[g][1], abs(coff[g][1])) if coff[g][1] else 0.0
            best = max(best, 0.3 * att[g][0] + 0.8 * math.atan(dy / 200.0))
    return R * best


# the event camera (type 3 / style 4: talk / demo / warp-arrival cameras; camera)
EVENT_STYLES = frozenset((4,))


def held_gap_terms(ca, cb, frames, tol):
    """the held camera-gap rule (6m), held-gap angle allowance (talk16 / talk18 frame 847: the event camera
    keeps an eye-direction gap of 0.12 deg taken at R 300 (0.65 units) and camera_Stop's zoom-out to R 1600 scales it to
    3.47 units). The eye-direction angle theta (each run's eye about its own centre, so a centre / carrier offset does
    not enter: the centre is judged on its own) over a run of consecutive frames, each within the camera tolerance
    either plainly (theta x R_A <= tol) or by this term: on a frame whose theta x R_A exceeds tol, the held angle
    theta_h = the largest theta of the run's earlier plain frames at a smaller A radius, R_min = the smallest A radius of
    those plain frames; the gap is judged by angle: theta x R_min (the angle within the tolerance as an angle, at the
    radius the gap was held at) and R_A x max(0, theta - theta_h) (a held gap grows in units with the radius only: a gap
    that opens or grows after the zoom is no allowance), each against tol. Plain frames only feed theta_h, so the term
    does not chain. frame -> (theta, theta_h, R_A, |R_B - R_A|, R_min)."""
    out, run = {}, []
    for f in frames:
        a, b = ca.get(f), cb.get(f)
        if a is None or b is None or not all(math.isfinite(x) for x in list(a[0:6]) + list(b[0:6])):
            run = []
            continue
        da = [a[k] - a[3 + k] for k in range(3)]
        db = [b[k] - b[3 + k] for k in range(3)]
        ra, rb = math.sqrt(sum(x * x for x in da)), math.sqrt(sum(x * x for x in db))
        if ra <= 0.0 or rb <= 0.0:
            run = []
            continue
        th = math.acos(max(-1.0, min(1.0, sum(x * y for x, y in zip(da, db)) / (ra * rb))))
        if th * ra <= tol:
            run.append((ra, th))
            continue
        low = [(r, t) for r, t in run if r < ra - 1e-3]
        if low:
            thh, rmin = max(t for r, t in low), min(r for r, t in low)
            if ra * max(0.0, th - thh) <= tol and th * rmin <= tol:
                out[f] = (th, thh, ra, abs(rb - ra), rmin)
                continue
        run = []
    return out


# rideCamera styles (engine 8: Boat MM/BB/tornado ride styles), whose yaw follows the ship's heading
RIDE_STYLES = frozenset((26, 32, 36, 41, 42, 110, 111))
# the speed-ramp position-gap rule (6c) camera, Link-following centre chase (himo2 / r58a hungCamera 175-183): the
# centre's per-frame follow ratios (param 3 xz, param 4 y) of the camera styles whose engine chases the centre toward a
# point by them (follow, lock-on, crawl, hookshot, hung, fixed position, tower, manual; dCamParam_c style table in the rpx,
# 0x1004487C, 0x84-byte entries, params at +8; both ratios in (0, 1)); ride styles (the carrier the carrier-offset rule (6j)) are left out
STYLE_CENTRE_RATIOS = {}
for _r, _ss in (((0.75, 0.25), (1, 2, 3, 5, 27, 28, 34, 38, 40, 43, 61, 65, 66, 67, 68, 82, 84, 85, 86, 87, 88, 89, 90, 91, 92,
                                93, 94, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 109, 113, 114, 119, 120, 121,
                                122, 123, 127, 128, 129, 130, 133, 135, 144)),
                ((0.8, 0.25), (95,)), ((0.7, 0.25), (48, 49, 50, 57, 58, 59, 60, 62, 83, 115, 125, 131, 132, 139, 140, 141,
                                                    142, 143)),
                ((0.5, 0.25), (21, 35)), ((0.2, 0.4), (9, 12, 15, 30, 74, 75, 77, 81, 137)), ((0.05, 0.2), (10, 16, 24, 37)),
                ((0.3, 0.1), (11, 31, 33)), ((0.9, 0.1), (13, 14)), ((0.1, 0.4), (17, 18, 73)), ((0.6, 0.2), (19, 20, 51)),
                ((0.4, 0.2), (22, 23, 69, 70, 71, 72, 76, 78, 79, 80, 108)), ((0.8, 0.8), (47,)), ((0.15, 0.15), (53, 55, 64)),
                ((0.25, 0.2), (54,)), ((0.55, 0.15), (116, 117, 118)), ((0.25, 0.1), (134,))):
    for _s in _ss:
        STYLE_CENTRE_RATIOS[_s] = _r


def chase_gain(r, dtb):
    """the speed-ramp position-gap rule (6c) steady chase-lag difference per unit of target speed for a per-frame ratio r at step dt_B: a chase of r
    behind a target moving v per frame trails it by v (1 - r) / r at 30 Hz and by v dt (1 - r_s) / r_s at N steps
    (r_s = 1 - (1 - r)^dt); g = the difference (>= 0); 0 outside (0, 1) or at dt 1"""
    if not (0.0 < r < 1.0) or dtb >= 1.0:
        return 0.0
    rs = 1.0 - (1.0 - r) ** dtb
    return max(0.0, dtb * (1.0 - rs) / rs - (1.0 - r) / r)
CARRY_WARP = 300.0  # Link's travel on one frame above any walk / ride speed (the ship tops out near 100 / frame)
CARRIER_INFO = {}  # frame -> the ship gate's xz allowance, whether the ship is within it, ship facing B - A (radians)
SHIP_ALLOW = {}  # frame -> (the ship gate's xz allowance, |B - A| ship offset, A's and B's ship position), every frame of
#                  the ship dumps (not only Link's ride frames; the carrier-offset rule (6j) addendum, evship_cam / evcam_offship)


def cam_type_terms(ta, tb, ca, la, lb, sa_=None):
    """6l addendum (g) on the camera (pair frame 214): the camera type
    change is a position-driven decision (the type follows where Link is); B may take it up to one frame earlier than A
    while Link's position lead is within the time slack. From B's change frame to A's (B's change in frame fA - 1 or fA,
    the same new type) the centre / fovy tolerance grows by A's centre movement on its change frame fA (the one frame of
    the new type's motion B may be ahead by) - derived, not fitted. Link's lead counts as within the slack when his
    frame-end offset is at most tslack x A's Link travel over the frame (or the next) + pos. Returns frame -> term."""
    def changes(t):
        ks = sorted(t)
        return [(ks[i], t[ks[i]]) for i in range(1, len(ks)) if t[ks[i]] != t[ks[i - 1]] and ks[i] == ks[i - 1] + 1]

    def lead_ok(g):
        if g not in la or g not in lb:
            return False
        off = math.dist(la[g]["pos"], lb[g]["pos"])
        trav = max([math.dist(la[h]["pos"], la[h - 1]["pos"]) for h in (g, g + 1) if h in la and h - 1 in la] or [0.0])
        return off <= TOL["tslack"] * trav + TOL["pos"]
    out = {}
    for fA, ty in changes(ta):
        for fB, tyb in changes(tb):
            if tyb != ty or not (fA - 1 <= fB <= fA) or fA not in ca or fA - 1 not in ca:
                continue
            if not all(lead_ok(g) for g in range(fB, fA + 1)):
                continue
            mv = max(abs(p - q) for p, q in zip(ca[fA][3:7], ca[fA - 1][3:7]))
            for g in range(fB, fA + 1):
                out[g] = max(out.get(g, 0.0), mv)
            # carry (131): the change frame's start lag (a crossing in a later step
            # of the frame leaves the new style fewer steps) decays with the new style's centre follow; ends below the
            # base tolerance or after 30 frames; another change inside the window adds its own term (max)
            keep = STYLE_CENTRE_KEEP.get(sa_.get(fA)) if (sa_ and TOL["cam_type_carry"] > 0) else None
            # no carry after a warp / scene change (Link moved more than CARRY_WARP on the change frame): that change is
            # no position crossing inside the frame, and its centre jump is no chase lag (210: the cave load)
            if keep is not None and fA in la and fA - 1 in la and math.dist(la[fA]["pos"], la[fA - 1]["pos"]) > CARRY_WARP:
                keep = None
            if keep is not None:
                t = mv
                for g in range(fA + 1, fA + 31):
                    t *= keep
                    if t < TOL["cam"]:
                        break
                    out[g] = max(out.get(g, 0.0), t)
    return out


# the speed-ramp position-gap rule (6c) camera (yaw episodes) (pair): the camera styles
# whose engine is manualCamera (engine 10, dCamera_c::manualCamera 0250FDC8; read from the rpx style table at
# 0x1004487C, 0x84-byte entries, engine word at +4: MM01-MM23, MM81-MM83)
MANUAL_STYLES = frozenset((47, 48, 49, 50, 57, 58, 59, 60, 61, 68, 115, 116, 117, 118, 125, 131, 132, 135, 139, 140, 141, 142, 143))


def cam_styles(run, fr, end):
    """frame -> camera style (cam.txt's second line, `style N`; the last record per frame)"""
    out, f = {}, None
    for l in lines(run, "cam.txt"):
        p = l.split()
        if not p:
            continue
        if p[0].isdigit() and len(p) >= 12:
            f = fr.frame(int(p[1]))
            if end is not None and f is not None and f > end:
                f = None
        elif f is not None and "style" in p:
            try:
                out[f] = int(p[p.index("style") + 1])
            except (ValueError, IndexError):
                pass
    return out


def manual_yaw_tol(ca, sa, bdt, ramp, keep):
    """the speed-ramp position-gap rule (6c) camera (yaw episodes): frame -> (yaw term in radians, A's horizontal eye-centre radius).
    While A's camera style is a manualCamera style (a session: consecutive frames in manual styles, a manual -> manual
    style change continues it), the N-step run turns through A's stick ramps on its steps' own chased turn rate (the
    6c ramp form on the yaw rate: B 3.79 deg past A after the ramp-down, persistent: a target angle has no
    restoring force). Term (6h(ii) form) = ramp x sum over episodes |net change of A's yaw rate in the episode|, an
    episode a monotone stretch of A's frame-end view yaw rate (frames with no change continue it; the running episode
    counts with its net so far); held (never reset) for the session. When A leaves the manual styles the session ends
    and the held term decays by keep per frame (the next style, followCamera, chases both views back behind Link);
    a later session adds on top of what is left. Only frames where B is converted (dt < 1) add."""
    out = {}
    yaw, rxz = {}, {}
    for f, v in ca.items():
        dx, dz = v[0] - v[3], v[2] - v[5]
        rxz[f] = math.hypot(dx, dz)
        yaw[f] = math.atan2(dx, dz) if rxz[f] > 0 else None
    def rate(f):
        if yaw.get(f) is None or yaw.get(f - 1) is None:
            return None
        return math.remainder(yaw[f] - yaw[f - 1], 2.0 * math.pi)
    carry, closed, ep, active = 0.0, 0.0, 0.0, False
    for f in sorted(ca):
        man = sa.get(f) in MANUAL_STYLES
        if man and not active:
            active, closed, ep = True, carry / ramp if ramp > 0 else 0.0, 0.0
        elif not man and active:
            active = False
            carry = ramp * (closed + abs(ep))
        if active:
            r1, r0 = rate(f), rate(f - 1)
            if r1 is not None and r0 is not None and bdt.get(f, 1.0) < 1.0:
                d = r1 - r0
                if d != 0.0:
                    if ep == 0.0 or (d > 0) == (ep > 0):
                        ep += d
                    else:
                        closed += abs(ep)
                        ep = d
            out[f] = (ramp * (closed + abs(ep)), rxz.get(f, 0.0))
        else:
            carry *= keep
            if carry > 0.0:
                out[f] = (carry, rxz.get(f, 0.0))
    return out


def vec_at(rows, t):
    g = int(math.floor(t))
    u = t - g
    if u == 0.0:
        return rows.get(g)
    r0, r1 = rows.get(g), rows.get(g + 1)
    if r0 is None or r1 is None:
        return None
    return [x + (y - x) * u for x, y in zip(r0, r1)]


def slack_times(f):
    """sample times of the window [f - tslack, f + tslack] (21 per side, the frame itself included)"""
    s = TOL["tslack"]
    return [f + s * k / 20.0 for k in range(-20, 21)]


def frame_wrap_len(la, lb, f):
    """the animation-phase lead rule (6d(b)) (looping animations; 2026-11-16, kmoU frame 37): the loop length of a wrap between
    frames f - 1 and f (or f and f + 1) in either run's frame-end records (the frame drops by more than 5 while the
    previous frame + its rate lands that loop length above it, L >= 5), else None. A wrap in one run only then compares
    modulo the loop in link-motion (raw frame rows carry no animL)."""
    for rows in (la, lb):
        for g in (f, f + 1):
            p, x = rows.get(g - 1), rows.get(g)
            if p and x and p["proc"] == x["proc"] and x["anim"] < p["anim"] - 5.0 and (x["rate"] or 0.0) > 0.0:
                L = round(p["anim"] + x["rate"] - x["anim"])
                if L >= 5 and abs(p["anim"] + x["rate"] - x["anim"] - L) < 0.1 * (x["rate"] or 0.0) + 0.05:
                    return float(L)
    return None


def link_dists(x, y):
    return (("pos", max(abs(p - q) for p, q in zip(x["pos"], y["pos"]))),
            ("speed", max([abs(x["speedF"] - y["speedF"])] + [abs(p - q) for p, q in zip(x["speed"], y["speed"])])),
            ("angle", max(angdiff(p, q) for p, q in zip(x["ang"], y["ang"]))),
            ("anim", min([abs(x["anim"] - y["anim"])] + ([abs(abs(x["anim"] - y["anim"]) - x["animL"])] if x.get("animL") else []))))


# the RNG-drift plausibility rule (8b): after the first RNG difference between the runs, unconverted processes and
# sounds whose timing is RNG-driven are plausibility only; converted processes (the test conversion's profiles: Link,
# the camera, ...) stay strictly gated. Converted code classes by profile, for sound callers (fn_names.h) that only
# start on full steps:
CONV_CLASSES = {"00A8": ("daPy_",), "01DC": ("dCam",)}
_FN = None


def fn_name(addr):
    """the decomp's function name around a guest address (wwhd_src/include/fn_names.h), '' if none"""
    global _FN
    if _FN is None:
        import bisect
        p = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "wwhd_src", "include", "fn_names.h")
        t = {}
        for l in lines(os.path.dirname(p), os.path.basename(p)):
            m = re.search(r"gabi::call<R>\(0x([0-9A-Fa-f]{8}).*// (.*)$", l)
            if m:
                t[int(m.group(1), 16)] = m.group(2).strip()
        _FN = (sorted(t), t)
    import bisect
    ks, t = _FN
    i = bisect.bisect_right(ks, addr) - 1
    return t[ks[i]] if i >= 0 and addr - ks[i] < 0x4000 else ""


def rng_first(A, B, fa, fb, end):
    """(first frame whose frame-start seeds differ or None, frames compared, frames equal)"""
    ra_ = {f: v[0] for f, v in state_channel(A, "rng.txt", fa, 0, lambda p: 0, lambda p: tuple(p[1:4]), end, ncol=4).items()}
    rb_ = {f: v[0] for f, v in state_channel(B, "rng.txt", fb, 0, lambda p: 0, lambda p: tuple(p[1:4]), end, ncol=4).items()}
    rf = sorted(set(ra_) & set(rb_))
    return next((f for f in rf if ra_[f] != rb_[f]), None), len(rf), sum(ra_[f] == rb_[f] for f in rf)


def rng_mode(run):
    """the run's test RNG mode from its log (port t60_test_rng.cpp): "keyed seq S", "keyed frame S", "const V" or "game"
    (no test RNG). Logs before the seq mode existed have only the first-draw line, which is the frame mode."""
    for l in lines(run, "log"):
        m = re.search(r"\[t60\] test rng keyed mode: (\w+) seed (\d+)", l)
        if m:
            return "keyed %s %s" % (m.group(1), m.group(2))
        m = re.search(r"\[t60\] WWHD_TEST_RNG_(KEYED|CONST)=(\S+?): test random numbers on", l)
        if m:
            return ("keyed frame %d" % int(m.group(2), 0)) if m.group(1) == "KEYED" else "const %s" % m.group(2)
    return "game"


def converted_profiles(B, given=None):
    """(converted profiles as 4-digit hex strings, or "all"; source): --converted, else B's log line "[t60] test conversion
    profiles: ..." (port t60.cpp), else the pair scripts' default 168,0x1DC (Link, camera; runs before that line)"""
    if given is not None:
        return set(given), "--converted"
    for l in lines(B, "log"):
        m = re.search(r"\[t60\] test conversion profiles: (\S+)", l)
        if m:
            v = m.group(1)
            return ("all" if v == "all" else {x.upper().zfill(4) for x in v.split(",") if x}), "B's log"
    return {"00A8", "01DC"}, "pair-script default"


def se_converted(k, half_keys, conv):
    """a sound key from converted code: B starts it on an in-between step, or its caller is a converted class's code"""
    if k in half_keys:
        return True
    c = k[2] if len(k) == 3 and k[1] == "ptr" else None
    if c:
        try:
            nm = fn_name(int(c, 16))
        except ValueError:
            nm = ""
        return conv == "all" or any(nm.startswith(pre) for prof in conv for pre in CONV_CLASSES.get(prof, ()))
    return False


# event-start world sound (pairs frame 61; the class of sw61a's
# freeze-frame world sounds, the sound-gate rule (8a) / 8c family): a per-frame world sound started by the process manager itself
# (not by a process) on the frame an event starts: the event's start is decided by event timing (B orders it on a step
# inside the frame, A from the next frame), so the frame's per-frame world sound plays in one run only. World sounds:
# starts whose caller is one of these functions
WORLD_SE_CALLERS = ("fpcM_Management",)


def se_callers(run, fr, keyf, end):
    """(frame, se_key) -> the set of callers (se.txt's last column) of that start"""
    out = collections.defaultdict(set)
    for l in lines(run, "se.txt"):
        p = l.split()
        if not SE_LINE.match(l.strip()):
            continue
        f = fr.frame(int(p[0]))
        if f is None or (end is not None and f > end):
            continue
        out[(f, keyf(p))].add(p[4])
    return out


def se_world(callers):
    """every caller of a start is a world-sound caller (WORLD_SE_CALLERS)"""
    if not callers:
        return False
    for c in callers:
        try:
            nm = fn_name(int(c, 16))
        except ValueError:
            return False
        if not any(nm.startswith(w) for w in WORLD_SE_CALLERS):
            return False
    return True


def event_start_frames(run, fr, end):
    """the frames on which the run's camera enters an event-camera style (EVENT_STYLES, cam.txt) from another style: the
    first frame of an event (a style-4 run of frames); empty without a camera trace"""
    sty = cam_styles(run, fr, end)
    return {f for f, s in sty.items() if s in EVENT_STYLES and f - 1 in sty and sty[f - 1] not in EVENT_STYLES}


# the strict RNG-pair rule (8d), the secondary proof of a converted RNG-driven actor: on a normal-RNG pair, from
# the first RNG difference on, the actor is checked for plausibility instead of frame by frame (the strict proof is the
# controlled-RNG pairs). --plaus-spec FILE names the actor's dump and its words, one block per dump:
#   dump NAME.admp          the WWHD_ACTOR_DUMP file (one record per process and step; the frame's last record counts)
#   profile PPPP            its profile (4 hex digits): procs / sound of it after the RNG difference become plausibility
#   code LO-HI              its code range (hex guest addresses): sound starts called from there likewise
#   state OFF TYPE          a state word (action, mode, ...): the state is the tuple of these
#   health OFF TYPE         its health
#   timer OFF TYPE          a timer (several allowed)
#   value OFF TYPE          any other word that must stay finite (positions, speeds)
#   attack V[/W] ...        state values counted as an attack: the first state word V (and the second W); a frame that
#                           enters one of them starts an attack
# Checks (B against A's distribution over the frames from the first RNG difference on, all processes of the dump):
#   finite    every f32 word of the spec finite in B
#   stuck     B's longest unchanged state run at most 2 x A's longest + TOL["stuck"] frames, and a process of B in an
#             attack state no longer than A's longest attack + TOL["stuck"]
#   health    B's health never above A's highest; no rise unless A rises too; no single-frame drop larger than A's
#             largest (a hit of another strength); the number of hits within the noise of A's (amb_allowed)
#   timers    every timer of B within A's range widened by half its span (at least 2 frames each side)
#   attacks   attack starts per AMB_WINDOW window (and in all) within the noise of A's (amb_allowed)
PLAUS_TYPES = {"f32": ">f", "s16": ">h", "s32": ">i", "u32": ">I", "u16": ">H", "u8": ">B", "s8": ">b"}


def drift6c_terms(bsteps, spoff):
    """the speed-ramp position-gap rule (6c) for an enemy's own motion: B's sub-steps move through a frame's
    speed change on their own speeds (the ChuChu's knock-back flight: speedF 60 slowing 4 per frame; the Bokoblin's
    slowing jump attack), which leaves a position gap of (1 - dt_B) / 2 x |dv| per frame, kept. bsteps: {(frame, proc):
    [(dt, body), ...]} B's step records in order; spoff: the speed word (f32). Returns {(frame, proc): term}, the sum over
    the process's frames up to the frame of (1 - dt_min) / 2 x |v_last - v_first| x N / (N - 1) (N the frame's step
    records: the whole change of a ramp across the steps; a speed set on one step and kept counts 0)."""
    out, acc = {}, {}
    for f, proc in sorted(bsteps):
        st = bsteps[(f, proc)]
        a = acc.get(proc, 0.0)
        if len(st) > 1 and all(spoff + 4 <= len(b) for _, b in st):
            v0, v1 = struct.unpack_from(">f", st[0][1], spoff)[0], struct.unpack_from(">f", st[-1][1], spoff)[0]
            dtm = min([d for d, _ in st if d > 0] or [1.0])
            if not (math.isnan(v0) or math.isnan(v1)):
                a += (1.0 - dtm) / 2.0 * abs(v1 - v0) * len(st) / (len(st) - 1)
        acc[proc] = a
        out[(f, proc)] = a
    return out


def contact_offsets(A, B, fa, fb, end, la, lb, admp_spec, per_spec=None):
    """the speed-ramp position-gap rule (6c) contact propagation (kmoD1_2 46-52 Moblin push, tnkD1_2 166 Darknut
    hit): an enemy with an accepted 6c position / facing drift that pushes or hits Link moves Link by a displacement that
    carries the enemy's own gap. Contact frames: Link's xz frame displacement differs from his own speed (a push: |disp -
    speed| > contact_ext in either run, pointing away from the nearest enemy: radial, cos >= 0.8) or a hit launches him (a launch: Link's health changes within a frame (life.admp,
    either run) and his speed jumps, |speed(f) - speed(f-1)| > contact_launch xz, the same procedure in both runs), with an enemy within contact_r of Link (A) whose dump has an --admp-spec 6c position
    word and that passes all its spec words at that frame (its own gates). A contact episode: consecutive push frames (one
    frame gap allowed), or a launch and the frames Link keeps its procedure after it. Allowance E = the largest over the
    episode's contact frames of the enemy's position gap |p_B - p_A| (xz) + its facing gap (spec a16 word, 0x32A
    preferred; radians) x the contact radius (Link - enemy xz distance in A). Link's displacement difference B - A over the
    episode (xz, summed) is taken up to |E| (the vector clipped), and Link's later frames are judged relative to the
    accumulated offset (the 6h / 6j carry form). Returns ({frame: [dx, 0, dz]}, notes)."""
    if not (admp_spec or per_spec) or TOL["contact"] <= 0:
        return {}, []

    def words(path):
        out = []
        for l in open(path):
            l = l.split("#")[0].split()
            if len(l) >= 3:
                out.append((int(l[0], 16), l[1], float(l[2]), int(l[3][3:], 16) if len(l) > 3 and l[3].startswith("6c=") else None))
        return out

    def offs(spec):
        pw = [w for w in spec if w[1] == "f32" and w[3] is not None]
        aw = [w for w in spec if w[1] == "a16"]
        if not pw or not aw:
            return None
        return min(w[0] for w in pw), (0x32A if any(w[0] == 0x32A for w in aw) else aw[0][0])
    # one spec for every dump (--admp-spec FILE), or each dump its own (--admp-spec NAME=FILE); a dump without a
    # 6c position word and an a16 facing word gives no term
    gspec = words(admp_spec) if admp_spec else None
    pspec = {k: words(v) for k, v in (per_spec or {}).items()}
    if not any(offs(x) for x in ([gspec] if gspec else []) + list(pspec.values())):
        return {}, ["contact: the --admp-spec has no 6c position word or no a16 facing word: no term"]
    fmt = {"f32": ">f", "s16": ">h", "a16": ">h", "s32": ">i", "u32": ">I", "u16": ">H", "u8": ">B", "s8": ">b"}

    def admp(path, fr, steps=None):
        out = {}
        data = open(path, "rb").read()
        o = 0
        while o + 28 <= len(data) and data[o:o + 4] == b"ADMP":
            step, full, dt, proc, size = struct.unpack_from("<QIfII", data, o + 4)
            body = data[o + 28:o + 28 + size]
            o += 28 + size
            f = fr.frame(step)
            if f is None or (end is not None and f > end) or len(body) < size:
                continue
            out[(f, proc)] = body
            if steps is not None:
                steps.setdefault((f, proc), []).append((dt, body))
        return out
    enemies = {}  # frame -> [(proc, posA, posB, facing gap rad, passes)]
    for n in sorted({os.path.basename(p) for p in glob.glob(os.path.join(A, "*.admp"))} & {os.path.basename(p) for p in glob.glob(os.path.join(B, "*.admp"))} - {LIFE_DUMP}):
        spec = pspec.get(n[:-5], gspec) if pspec else gspec
        if not spec or not offs(spec):
            continue
        pos_off, ang_off = offs(spec)
        bsteps = {}
        xa, xb = admp(os.path.join(A, n), fa), admp(os.path.join(B, n), fb, bsteps)
        t6 = {sp: drift6c_terms(bsteps, sp) for sp in {w[3] for w in spec if w[3] is not None}}
        for key in set(xa) & set(xb):
            ba, bb = xa[key], xb[key]
            if pos_off + 12 > min(len(ba), len(bb)) or ang_off + 2 > min(len(ba), len(bb)):
                continue
            ok = True
            for off, ty, tol, sp6 in spec:
                sz = struct.calcsize(fmt[ty])
                if off + sz > min(len(ba), len(bb)):
                    continue
                u, v = struct.unpack_from(fmt[ty], ba, off)[0], struct.unpack_from(fmt[ty], bb, off)[0]
                if isinstance(u, float) and math.isnan(u) and math.isnan(v):
                    continue
                d = angdiff(u, v) if ty == "a16" else abs(u - v)
                if not d <= tol + (t6[sp6].get(key, 0.0) if sp6 is not None else 0.0):
                    ok = False
                    break
            pa, pb = struct.unpack_from(">fff", ba, pos_off), struct.unpack_from(">fff", bb, pos_off)
            ga = angdiff(struct.unpack_from(">h", ba, ang_off)[0], struct.unpack_from(">h", bb, ang_off)[0]) * math.pi / 32768.0
            enemies.setdefault(key[0], []).append((key[1], pa, pb, ga, ok))

    def extv(r, f):  # xz displacement minus the frame's own speed
        p, q = r.get(f - 1), r.get(f)
        if p is None or q is None:
            return (0.0, 0.0)
        return (q["pos"][0] - p["pos"][0] - q["speed"][0], q["pos"][2] - p["pos"][2] - q["speed"][2])

    def pushed(f):
        """a push: the external displacement of either run over contact_ext, pointing away from the nearest enemy within
        contact_r (radial within ~37 deg: cos >= 0.8; Link's own foot-travel / slope residuals have no such direction)"""
        x = la.get(f)
        near = [(math.hypot(pa[0] - x["pos"][0], pa[2] - x["pos"][2]), pa) for _, pa, _, _, _ in enemies.get(f, [])] if x else []
        near = [q for q in near if 0.0 < q[0] <= TOL["contact_r"]]
        if not near:
            return False
        r, pa = min(near)
        u = ((x["pos"][0] - pa[0]) / r, (x["pos"][2] - pa[2]) / r)
        for rr in (la, lb):
            e = extv(rr, f)
            m = math.hypot(*e)
            if m > TOL["contact_ext"] and e[0] * u[0] + e[1] * u[1] >= 0.8 * m:
                return True
        return False

    def jump(r, f):
        p, q = r.get(f - 1), r.get(f)
        if p is None or q is None:
            return 0.0
        return math.hypot(q["speed"][0] - p["speed"][0], q["speed"][2] - p["speed"][2])

    def contact_E(f):  # the nearest enemy within contact_r of Link (A): its allowance, or None (none / it fails its gates)
        x = la.get(f)
        best = None
        for proc, pa, pb, ga, ok in enemies.get(f, []):
            r = math.hypot(pa[0] - x["pos"][0], pa[2] - x["pos"][2])
            if r <= TOL["contact_r"] and (best is None or r < best[0]):
                best = (r, ok, math.hypot(pb[0] - pa[0], pb[2] - pa[2]) + ga * r, proc)
        if best is None or not best[1]:
            return None, best
        return best[2], best
    hits = set()  # Link's health changes (life.admp) in either run: a launch counts only as a hit
    for r, fr in ((A, fa), (B, fb)):
        hc = hit_changes(r, fr, "life", end) or {}
        for v in hc.values():
            hits |= {g + d for g in v for d in (-1, 0, 1)}
    out, notes, O = {}, [], [0.0, 0.0]
    frames = sorted(set(la) & set(lb))
    if not frames:
        return {}, notes
    i = 0
    while i < len(frames):
        f = frames[i]
        push = pushed(f)
        launch = f in hits and la[f]["proc"] == lb[f]["proc"] and max(jump(la, f), jump(lb, f)) > TOL["contact_launch"]
        E, info = contact_E(f) if (push or launch) else (None, None)
        if E is None:
            if (push or launch) and info is not None and not info[1]:
                notes.append("frame %d: enemy %X fails its own gates: no contact term" % (f, info[3]))
            out[f] = [O[0], 0.0, O[1]]
            i += 1
            continue
        # the episode
        ep, Emax, S, j, last_push = [], E, [0.0, 0.0], i, f
        proc0 = la[f]["proc"]
        while j < len(frames):
            g = frames[j]
            if g != f:
                pg = pushed(g)
                if launch and la[g]["proc"] == proc0 and lb[g]["proc"] == proc0:
                    pass
                elif pg and g - last_push <= 2:
                    pass
                else:
                    break
                if pg:
                    last_push = g
                    Eg, _ = contact_E(g)
                    if Eg is not None:
                        Emax = max(Emax, Eg)
            p0a, p1a, p0b, p1b = la.get(g - 1), la[g], lb.get(g - 1), lb[g]
            if p0a is not None and p0b is not None:
                S[0] += (p1b["pos"][0] - p0b["pos"][0]) - (p1a["pos"][0] - p0a["pos"][0])
                S[1] += (p1b["pos"][2] - p0b["pos"][2]) - (p1a["pos"][2] - p0a["pos"][2])
            m = math.hypot(*S)
            k = 1.0 if m <= Emax or m == 0.0 else Emax / m
            out[g] = [O[0] + S[0] * k, 0.0, O[1] + S[1] * k]
            ep.append(g)
            j += 1
        m = math.hypot(*S)
        k = 1.0 if m <= Emax or m == 0.0 else Emax / m
        O = [O[0] + S[0] * k, O[1] + S[1] * k]
        notes.append("frames %d-%d (%s, enemy %X): Link's displacement gap %.3g, allowance %.3g (enemy gap + facing x r)" % (
            ep[0], ep[-1], "launch" if launch else "push", info[3], m, Emax))
        i = j
    return {f: v for f, v in out.items() if v[0] or v[2]}, notes


def hit_changes(run, fr, spec, end=None):
    """the step-resolution threshold rule (6l) (f): the frames where a hit word's frame-end value changes, per process: {proc: [frame, ...]}.
    spec `life` (Link's mLife, life.admp +2 u16) or `FILE:OFF:TYPE` (an actor dump word, TYPE s8 u8 s16 u16 s32 u32 f32)"""
    fmt = {"s8": ">b", "u8": ">B", "s16": ">h", "u16": ">H", "s32": ">i", "u32": ">I", "f32": ">f"}
    if spec == "life":
        name, off, ty = LIFE_DUMP, 2, "u16"
    else:
        p = spec.split(":")
        if len(p) != 3 or p[2] not in fmt:
            return None
        name, off, ty = p[0], int(p[1], 16), p[2]
    path = os.path.join(run, name)
    if not os.path.exists(path):
        return None
    last = {}
    data = open(path, "rb").read()
    o = 0
    while o + 28 <= len(data) and data[o:o + 4] == b"ADMP":
        step, full, dt, proc, size = struct.unpack_from("<QIfII", data, o + 4)
        body = data[o + 28:o + 28 + size]
        o += 28 + size
        f = fr.frame(step)
        if f is None or (end is not None and f > end) or off + struct.calcsize(fmt[ty]) > len(body):
            continue
        last[(proc, f)] = struct.unpack_from(fmt[ty], body, off)[0]  # the last record per frame: the frame end
    out = {}
    for (proc, f) in sorted(last, key=lambda k: (k[0], k[1])):
        prev = last.get((proc, f - 1))
        if prev is not None and prev != last[(proc, f)]:
            out.setdefault(proc, []).append(f)
    return out


def hit_cut_check(A, B, fa, fb, spec, cut, end=None):
    """the step-resolution threshold rule (6l) (f): (ok, text) for a cut at B's hit frame: A has the same word's change on the cut frame or one
    frame later, and none on the frame before (B late)"""
    ha, hb = hit_changes(A, fa, spec, end), hit_changes(B, fb, spec, end)
    if ha is None or hb is None:
        return False, "hit=%s: no such dump in %s" % (spec, "A" if ha is None else "B")
    procs = [p for p, fs in hb.items() if cut in fs]
    if not procs:
        return False, "hit=%s: B has no hit on the cut frame %d (B's changes %s)" % (spec, cut, {"%08X" % p: fs[:6] for p, fs in hb.items()})
    for p in procs:
        fs = ha.get(p, [])
        if cut in fs or cut + 1 in fs:
            return True, "hit=%s: process %08X hit on frame %d in B, %d in A (%s)" % (
                spec, p, cut, cut if cut in fs else cut + 1, "same frame" if cut in fs else "one frame early in B")
    near = {"%08X" % p: [f for f in ha.get(p, []) if abs(f - cut) <= 3] for p in procs}
    return False, "hit=%s: B's hit on frame %d has no counterpart in A on frames %d-%d (A's changes near it: %s; late or two frames early: no cut)" % (
        spec, cut, cut, cut + 1, near)


def admp_steps(path, fr, end=None):
    """{(frame, proc): [(dt, body), ...]} of a dump, every step record in order (pair tools: cccmp / bkcmp / mocmp)."""
    out = {}
    data = open(path, "rb").read()
    o = 0
    while o + 28 <= len(data) and data[o:o + 4] == b"ADMP":
        step, full, dt, proc, size = struct.unpack_from("<QIfII", data, o + 4)
        body = data[o + 28:o + 28 + size]
        if len(body) < size:
            break
        o += 28 + size
        f = fr.frame(step)
        if f is None or (end is not None and f > end):
            continue
        out.setdefault((f, proc), []).append((dt, body))
    return out


def plaus_spec(path):
    specs, cur = [], None
    for l in open(path):
        p = l.split("#")[0].split()
        if not p:
            continue
        k = p[0]
        if k == "dump":
            cur = {"dump": p[1], "profile": None, "code": [], "state": [], "health": None, "timer": [], "value": [], "attack": set()}
            specs.append(cur)
        elif cur is None:
            die("compare60: --plaus-spec %s: `%s` before a `dump` line" % (path, " ".join(p)))
        elif k == "profile":
            cur["profile"] = p[1].upper().zfill(4)
        elif k == "code":
            lo, _, hi = p[1].partition("-")
            cur["code"].append((int(lo, 16), int(hi, 16)))
        elif k in ("state", "health", "timer", "value") and len(p) == 3 and p[2] in PLAUS_TYPES:
            w = (int(p[1], 16), p[2])
            if k == "health":
                cur["health"] = w
            else:
                cur[k].append(w)
        elif k == "attack":
            for v in p[1:]:
                cur["attack"].add(tuple(int(x, 0) for x in v.split("/")))
        else:
            die("compare60: --plaus-spec %s: bad line `%s`" % (path, " ".join(p)))
    return specs


def plaus_check(sp, xa, xb, f0):
    """(ok, message, details) of one --plaus-spec block; xa / xb: {(frame, proc): body} of A / B; frames >= f0 count"""
    def word(b, w):
        o, ty = w
        fm = PLAUS_TYPES[ty]
        return struct.unpack_from(fm, b, o)[0] if o + struct.calcsize(fm) <= len(b) else None

    def per_proc(x):
        out = collections.defaultdict(dict)
        for (f, proc), b in x.items():
            if f >= f0:
                out[proc][f] = b
        return out
    pa, pb = per_proc(xa), per_proc(xb)
    det = []

    def runs(pp, key):
        """[(proc, frames)] of maximal runs of an unchanged key (consecutive frames)"""
        out = []
        for proc, r in pp.items():
            prev, n, last = None, 0, None
            for f in sorted(r):
                k = key(r[f])
                if k == prev and last is not None and f == last + 1:
                    n += 1
                else:
                    if prev is not None:
                        out.append((proc, prev, n))
                    prev, n = k, 1
                last = f
            if prev is not None:
                out.append((proc, prev, n))
        return out

    state = lambda b: tuple(word(b, w) for w in sp["state"])
    # finite
    fw = [w for w in sp["state"] + sp["timer"] + sp["value"] + ([sp["health"]] if sp["health"] else []) if w[1] == "f32"]
    bad = sorted((f, proc, w[0]) for proc, r in pb.items() for f, b in r.items() for w in fw
                 if word(b, w) is not None and not math.isfinite(word(b, w)))
    if bad:
        det.append("not finite in B: %d word(s), first +0x%X of %08X at frame %d" % (len(bad), bad[0][2], bad[0][1], bad[0][0]))
    # stuck
    if sp["state"]:
        ra, rb = runs(pa, state), runs(pb, state)
        la = max((n for _, _, n in ra), default=0)
        lb = max(rb, key=lambda x: x[2], default=None)
        if lb and lb[2] > 2 * la + TOL["stuck"]:
            det.append("stuck: process %08X of B in state %s for %d frames (A's longest run %d, allowed %d)" % (lb[0], lb[1], lb[2], la, 2 * la + TOL["stuck"]))
        if sp["attack"]:
            isatk = lambda s: any(s[:len(a)] == a for a in sp["attack"])
            aa = max((n for _, s, n in runs(pa, lambda b: isatk(state(b))) if s), default=0)
            ab = max(((p, n) for p, s, n in runs(pb, lambda b: isatk(state(b))) if s), key=lambda x: x[1], default=None)
            if ab and ab[1] > aa + TOL["stuck"]:
                det.append("stuck: process %08X of B attacking for %d frames (A at most %d)" % (ab[0], ab[1], aa))
    # health
    if sp["health"]:
        def hstats(pp):
            hs, rises, drops = [], 0, []
            for r in pp.values():
                prev = None
                for f in sorted(r):
                    h = word(r[f], sp["health"])
                    if h is None:
                        continue
                    hs.append(h)
                    if prev is not None and h > prev:
                        rises += 1
                    if prev is not None and h < prev:
                        drops.append(prev - h)
                    prev = h
            return hs, rises, drops
        ha, rua, dra = hstats(pa)
        hb, rub, drb = hstats(pb)
        ha_all = [word(b, sp["health"]) for b in xa.values() if word(b, sp["health"]) is not None]  # A's whole run
        if hb and ha_all and max(hb) > max(ha_all):
            det.append("health: B reaches %s, A at most %s" % (max(hb), max(ha_all)))
        if rub and not rua:
            det.append("health: rises %d time(s) in B, never in A" % rub)
        if drb and dra and max(drb) > max(dra):
            det.append("health: a hit of %s in B, A's largest %s" % (max(drb), max(dra)))
        if abs(len(drb) - len(dra)) > amb_allowed(len(dra), len(drb)):
            det.append("health: %d hit(s) in B, %d in A (allowed +-%.1f)" % (len(drb), len(dra), amb_allowed(len(dra), len(drb))))
    # timers
    for w in sp["timer"]:
        va = [word(b, w) for r in pa.values() for b in r.values() if word(b, w) is not None]
        vb = [(word(b, w), f, proc) for proc, r in pb.items() for f, b in r.items() if word(b, w) is not None]
        if not va or not vb:
            continue
        lo, hi = min(va), max(va)
        m = max(2, (hi - lo) / 2.0)
        out = [x for x in vb if not (lo - m <= x[0] <= hi + m)]
        if out:
            x = max(out, key=lambda x: abs(x[0] - (lo + hi) / 2.0))
            det.append("timer +0x%X: B %s (process %08X, frame %d) outside A's %s..%s (+-%g)" % (w[0], x[0], x[2], x[1], lo, hi, m))
    # attack rate
    natk = (0, 0)
    if sp["attack"] and sp["state"]:
        isatk = lambda s: any(s[:len(a)] == a for a in sp["attack"])

        def starts(pp):
            out = []
            for r in pp.values():
                prev = False
                for f in sorted(r):
                    a = isatk(state(r[f]))
                    if a and not prev:
                        out.append(f)
                    prev = a
            return out
        sa_, sb_ = starts(pa), starts(pb)
        natk = (len(sa_), len(sb_))
        last = max(sa_ + sb_ + [f0])
        for w0 in range(f0, last + 1, AMB_WINDOW):
            ca = sum(1 for f in sa_ if w0 <= f < w0 + AMB_WINDOW)
            cb = sum(1 for f in sb_ if w0 <= f < w0 + AMB_WINDOW)
            if abs(cb - ca) > amb_allowed(ca, cb):
                det.append("attacks: frames %d-%d A %d, B %d starts (allowed +-%.1f)" % (w0, w0 + AMB_WINDOW - 1, ca, cb, amb_allowed(ca, cb)))
        if abs(natk[1] - natk[0]) > amb_allowed(*natk):
            det.append("attacks: %d starts in A, %d in B (allowed +-%.1f)" % (natk[0], natk[1], amb_allowed(*natk)))
    msg = "%s from frame %d (first RNG difference / strict cut): %d / %d processes, attack starts A %d / B %d" % (
        sp["dump"], f0, len(pa), len(pb), natk[0], natk[1])
    return not det, msg, det


def word_overlaps(o, sz, q, qsz=4):
    """whether the spec word [o, o + sz) shares a byte with the masked word [q, q + qsz) (kddoor: a 4-byte window
    for every spec word put u8 words 0xA19..0xA1B inside the tevStr word at 0xA1C)"""
    return o < q + qsz and q < o + sz


REQUIRE_SPEC = set()  # --admp-require-spec: dumps whose words must be judged with a spec
ADMP_EXACT = set()  # --admp-exact NAME[,NAME]: every logic word of the dump exact (the report's word set as a gate)
ADMP_SPECS = {}  # --admp-spec NAME=FILE: per-dump specs (NAME without .admp); the plain FILE form stays for every dump


def main():
    args = sys.argv[1:]
    kv = {}
    pos = []
    admp_spec = None
    admp_tevstr = []  # --admp-tevstr NAME:OFF,...: more tevStrs (picture-state light words) in dump NAME
    plaus_specs = []
    save_mask = []
    k_a = k_b = None
    conv_given = None
    keyed_proofs = []
    cut, cause = None, None
    ship_name = "ship.admp"
    i = 0
    while i < len(args):
        a = args[i]
        if a == "--tol" and i + 1 < len(args):
            n, _, v = args[i + 1].partition("=")
            if n not in TOL:
                die("compare60: unknown tolerance %s (%s)" % (n, ", ".join(TOL)))
            TOL[n] = type(TOL[n])(float(v)) if isinstance(TOL[n], float) else int(v)
            i += 2
        elif a == "--target-gate" and i + 1 < len(args):  # the process-order read rule (14i) interim (a): the enemy's own gate report
            pth, _, pr = args[i + 1].partition(":")
            TARGET_GATE.append((pth, pr.upper().zfill(8) if pr else None))
            i += 2
        elif a == "--ship" and i + 1 < len(args):
            ship_name = args[i + 1]
            i += 2
        elif a == "--admp-spec" and i + 1 < len(args):
            v = args[i + 1]
            if "=" in v and not os.path.exists(v):  # s45 per-dump form NAME=FILE[,NAME=FILE] (repeatable): each dump its own spec
                for x in v.split(","):
                    nm, _, fp = x.partition("=")
                    if not nm or not fp or not os.path.exists(fp):
                        die("compare60: --admp-spec %s: NAME=FILE with an existing FILE" % x)
                    ADMP_SPECS[nm[:-5] if nm.endswith(".admp") else nm] = fp
            else:
                admp_spec = v
            i += 2
        elif a == "--admp-exact" and i + 1 < len(args):  # NAME[,NAME]: no spec needed, every word that is neither
            # draw-written nor tevStr picture state must be equal (the report's set as a FAIL gate); a spec adds tolerances
            ADMP_EXACT.update(x[:-5] if x.endswith(".admp") else x for x in args[i + 1].split(",") if x)
            i += 2
        elif a == "--admp-require-spec" and i + 1 < len(args):  # (2026-11-20): NAME[,NAME] or `all`: such a dump
            # without --admp-spec is a FAIL (gate admp-spec), not a report: a mark's subject words must be judged
            REQUIRE_SPEC.update(x for x in args[i + 1].split(",") if x)
            i += 2
        elif a == "--admp-tevstr" and i + 1 < len(args):
            admp_tevstr += [x for x in args[i + 1].split(",") if x]
            i += 2
        elif a == "--plaus-spec" and i + 1 < len(args):
            plaus_specs += plaus_spec(args[i + 1])
            i += 2
        elif a == "--cloth" and i + 1 < len(args):  # the cloth-simulation rule (6k): FILE[#TAG]:NV:POS0:POS1:SPEED (hex offsets), a cloth's dump
            fn, nv_, q0, q1, qs = args[i + 1].split(":")
            CLOTH[fn] = (int(nv_), int(q0, 16), int(q1, 16), int(qs, 16), "given")
            i += 2
        elif a == "--state-word" and i + 1 < len(args):  # the counter-edge rule (8c): FILE:OFF:FMT, a converted party's state word
            fn, o, fm = args[i + 1].split(":")
            CONV_STATE[fn] = (int(o, 16), fm, "given")
            i += 2
        elif a == "--save-mask" and i + 1 < len(args):
            for r in args[i + 1].split(","):
                o, _, n = r.partition(":")
                save_mask.append((int(o, 16), int(n, 16)))
            i += 2
        elif a == "--strict-until" and i + 1 < len(args):
            cut = int(args[i + 1])
            i += 2
        elif a == "--cause" and i + 1 < len(args):
            cause = args[i + 1]
            i += 2
        elif a == "--keyed-proof" and i + 1 < len(args):  # the sound-gate rule (8a) class: the route's keyed pair
            keyed_proofs.append(args[i + 1])
            i += 2
        elif a == "--converted" and i + 1 < len(args):  # override: comma list of 4-digit hex profiles
            conv_given = [x.strip().upper().zfill(4) for x in args[i + 1].split(",") if x.strip()]
            i += 2
        elif a in ("--steps-a", "--steps-b") and i + 1 < len(args):
            if a == "--steps-a":
                k_a = int(args[i + 1])
            else:
                k_b = int(args[i + 1])
            i += 2
        else:
            pos.append(a)
            i += 1
    if len(pos) < 2:
        die(__doc__)
    A, B = pos[0], pos[1]
    end = int(float(pos[2]) * 30) if len(pos) > 2 else None
    sa, sb = origin(A), origin(B)
    print("origin: %s step %s, %s step %s" % (A, sa, B, sb))
    if sa is None or sb is None:
        die("no origin (scenario did not start)")
    fa, fb = Frames(A, sa, k_a), Frames(B, sb, k_b)
    print("frames: A %s, B %s" % (fa.src, fb.src))
    rep = Report()
    CRANE.clear()
    if TOL["crane_plaus"] > 0:
        CRANE.update(crane_window(A, B, fa, fb, end, ship_name))
    # the strict RNG-pair rule (8d) tooling: which test RNG mode the pair used (keyed frame / seq); both runs must use the same
    rma, rmb = rng_mode(A), rng_mode(B)
    rep.gate("rng-mode", False if rma != rmb else None, "A %s, B %s%s" % (rma, rmb, " (the runs differ: not comparable)" if rma != rmb else ""))
    # the strict RNG-pair rule (8d) addendum: the strict cut and its cause
    if cut is not None or cause is not None:
        # (8d addendum 2): the cause may name an accepted rule on either side, Link's (side=link,
        # the default: 3a, 4a, 6a-6g) or the enemy's own accepted classes (side=enemy: the per-step smoothing rule (3) / 3a float drift), when it
        # feeds into a contact (a hit, push or knock-back on Link, or the enemy's decision about Link)
        sd = re.search(r"\bside=(\S+)", cause or "")
        side = sd.group(1) if sd else "link"
        m = re.search(r"\brule=(\S+)", cause or "")
        d = re.search(r"\bdecision=(\S.*)", cause or "")
        ok_rule = m is not None and side in CUT_RULES and m.group(1) in CUT_RULES[side]
        if ok_rule and side == "link" and m.group(1) in CUT_RULES_UNCONVERTED_LINK:
            cv, _ = converted_profiles(B, conv_given)
            ok_rule = cv != "all" and "00A8" not in cv
        hit_ok, hit_txt = True, ""
        if ok_rule and m.group(1) in CUT_HIT_RULES and cut is not None:
            # (f): with hit= the hit check; side=enemy needs it (its 6l is the hit only); side=link rule=6l without hit=
            # stays Link-side cause (an accepted 6l event of Link's feeding the enemy's decision)
            h = re.search(r"\bhit=(\S+)", cause or "")
            if h:
                hit_ok, hit_txt = hit_cut_check(A, B, fa, fb, h.group(1), cut, end)
            elif side == "enemy":
                hit_ok, hit_txt = False, "side=enemy rule=6l needs hit=life|FILE:OFF:TYPE (the hit sampled at step resolution)"
        if cut is None:
            rep.gate("strict-cut", False, "--cause without --strict-until: %r" % cause)
        elif ok_rule and not hit_ok:
            rep.gate("strict-cut", False, "--strict-until %d, rule 6l (f) hit check: %s" % (cut, hit_txt))
        elif not (ok_rule and d and len(d.group(1).strip()) >= 8):
            rep.gate("strict-cut", False, "--strict-until %d without a valid --cause (needs [side=link] rule=%s or side=enemy rule=%s, and decision=TEXT naming the contact / enemy decision): %r" % (
                cut, "|".join(CUT_RULES["link"]), "|".join(CUT_RULES["enemy"]), cause))
        else:
            rep.gate("strict-cut", True, "strict per-frame comparison until frame %d (exclusive); cause: accepted %s rule %s -> %s%s" % (cut, "Link" if side == "link" else "enemy", m.group(1), d.group(1).strip(), "; " + hit_txt if hit_txt else ""))

    # mode: A at dt 1 only, B with dt < 1 (trace.txt dt column or steps.txt)
    def dts(run):
        v = [fnum(p[4]) for p in (l.split() for l in lines(run, "trace.txt")) if len(p) > 4]
        v += [fnum(p[2]) for p in (l.split() for l in lines(run, "steps.txt")) if len(p) > 2]
        return v
    da, db = dts(A), dts(B)
    if not da or not db:
        rep.gate("mode", False, "no dt values (trace.txt / steps.txt missing)")
    else:
        rep.gate("mode", all(d == 1 for d in da) and min(db) < 1,
                 "A dt %s; B dt min %g (%d of %d records below 1)" % (sorted(set(da)), min(db), sum(d < 1 for d in db), len(db)))

    # origin-clock: both runs must start the scenario at the same world time. A boot-time
    # async load can leave one run's world a frame behind (pcr4: B mTimer 3173 at the origin, A 3174; the sea's water
    # one frame apart from frame 0, the RNG apart from frame 24): every gate below then measures that offset, not the
    # conversion. Refused as a FAIL with the reason; the pair scripts run with WWHD_TEST_SYNC_LOADS=1 against it.
    (ca, sya), (cb, syb) = origin_clock(A), origin_clock(B)
    if ca is None or cb is None:
        rep.gate("origin-clock", None, "no scenario-origin clock line in %s (WWHD_TEST_ORIGIN_RESEED off or an older port)" %
                 ("both logs" if ca is None and cb is None else "A's log" if ca is None else "B's log"))
    else:
        ok = ca[0] == cb[0]
        rep.gate("origin-clock", ok, "mTimer at the origin A %d / B %d, mCounter0 A %d / B %d; WWHD_TEST_SYNC_LOADS A %s / B %s" %
                 (ca[0], cb[0], ca[1], cb[1], "on" if sya else "off", "on" if syb else "off"),
                 [] if ok else ["NOT A CONVERSION VERDICT: the runs' worlds are %d frame(s) apart at the scenario origin "
                                "(boot-time async loads); the other gates measure that offset. Rerun the pair with "
                                "WWHD_TEST_SYNC_LOADS=1." % abs(ca[0] - cb[0])])

    # health
    bad = []
    for run in (A, B):
        for l in lines(run, "log"):
            if re.search(r"halt|crash|assert|fatal", l, re.I):
                bad.append("%s log: %s" % (os.path.basename(run.rstrip("/")), l.strip()[:120]))
                break
        for name in ("trace.txt", "cam.txt"):
            for l in lines(run, name):
                if re.search(r"\b-?(nan|inf)\b", l, re.I):
                    bad.append("%s %s: %s" % (os.path.basename(run.rstrip("/")), name, l.strip()[:120]))
                    break
    rep.gate("health", not bad, "no crash, no NaN/inf" if not bad else "%d problems" % len(bad), bad[:4])

    # Link
    # A one frame past the end: the far side of the last compared frame's time-slack window (B's frames stop at the end;
    # pgrab5's constant 1-frame wait-animation lead failed link-tslack at its last frame 210 only, a one-sided window);
    # where A's run has no frame past it, link-tslack leaves that last frame to link-motion
    la, lb = link_rows(A, fa, end + 1 if end is not None else None), link_rows(B, fb, end)
    # the frames both runs reached (for the sound gate's end), before a strict cut trims Link's rows
    link_last = min(max(la), max(lb)) if la and lb else None
    if cut is not None:
        la = {f: v for f, v in la.items() if f < cut}
        lb = {f: v for f, v in lb.items() if f < cut}
    proc_shifts = []  # accepted procedure shifts (procedure, A's frame, B's frame), within proc_slack (6l / 6d(b) class)
    eplaus = set()  # the step-resolution threshold rule (6l) addendum (c): the unconverged-carry episodes' frames (link-tslack fills it)
    common = sorted(set(la) & set(lb))
    if not common:
        rep.gate("link-procs", False, "no common frames in trace.txt (A %d, B %d frames)" % (len(la), len(lb)))
    else:
        last = min(max(la), max(lb))
        ra = runs_of({f: v["proc"] for f, v in la.items() if f <= last})
        rb = runs_of({f: v["proc"] for f, v in lb.items() if f <= last})
        if cut is not None:  # a change within proc_slack of the strict cut may fall on the other side of it in one run
            while len(ra) > 1 and ra[-1][1] >= cut - TOL["proc_slack"]:
                ra = ra[:-1]
            while len(rb) > 1 and rb[-1][1] >= cut - TOL["proc_slack"]:
                rb = rb[:-1]
        seq_a, seq_b = [p for p, _ in ra], [p for p, _ in rb]
        det, ok = [], seq_a == seq_b
        worst = 0
        if ok:
            for (p, f1), (_, f2) in zip(ra, rb):
                worst = max(worst, abs(f1 - f2))
                if 0 < abs(f1 - f2) <= TOL["proc_slack"]:
                    proc_shifts.append((p, f1, f2))
                if abs(f1 - f2) > TOL["proc_slack"] + crane_slack(f1, f2):
                    ok = False
                    det.append("procedure %s starts at frame %d (A) / %d (B)" % (p, f1, f2))
        else:
            k = next((j for j in range(min(len(seq_a), len(seq_b))) if seq_a[j] != seq_b[j]), min(len(seq_a), len(seq_b)))
            det.append("sequences differ at change %d: A %s, B %s" % (k, ra[k:k + 3], rb[k:k + 3]))
        rep.gate("link-procs", ok, "%d procedure changes, largest shift %d frame(s) (slack %d%s)" % (len(ra) - 1, worst, TOL["proc_slack"],
                 " + D %d from frame %d: crane/salvage plausibility until M5" % (CRANE["D"], CRANE["lo"]) if CRANE else ""), det[:4])
        # motion where both are in the same procedure, away from changes
        changes = sorted({f for _, f in ra[1:]} | {f for _, f in rb[1:]})

        def near(f):  # (and the crane / salvage window: no per-frame value checks there)
            return in_crane(f) or any(abs(f - c) <= TOL["proc_slack"] for c in changes)
        mx = collections.Counter()
        where = {}
        n = 0
        for f in common:
            x, y = la[f], lb[f]
            if f > last or x["proc"] != y["proc"] or near(f):
                continue
            n += 1
            for nm, d in (("pos", max(abs(p - q) for p, q in zip(x["pos"], y["pos"]))),
                          ("speed", max([abs(x["speedF"] - y["speedF"])] + [abs(p - q) for p, q in zip(x["speed"], y["speed"])])),
                          ("angle", max(angdiff(p, q) for p, q in zip(x["ang"], y["ang"]))),
                          ("anim", min([abs(x["anim"] - y["anim"])] + [abs(abs(x["anim"] - y["anim"]) - L)
                                                                      for L in (x.get("animL"), frame_wrap_len(la, lb, f)) if L]))):
                if not (d <= mx[nm]):
                    mx[nm] = d
                    where[nm] = f
        over = [nm for nm in ("pos", "speed", "angle", "anim") if not (mx[nm] <= TOL[nm])]
        rep.gate("link-motion", not over if n else None,
                 "%d frames compared; max |d| %s" % (n, ", ".join("%s %.4g (frame %s, tol %g)" % (nm, mx[nm], where.get(nm, "-"), TOL[nm])
                                                                  for nm in ("pos", "speed", "angle", "anim"))))
        # the same frames with the time slack: the smallest distance of B's value to A's trajectory in the window
        # the speed-ramp position-gap rule (6c): the N-step run moves through a speed change of the 30 Hz frame on its
        # steps' own speeds (the 60 Hz half-step motion, no xz arc term), so each speed change dv of A's frame may move
        # B by up to |dv| (1 - dt_B) / 2 along the track, and that drift stays. The position tolerance of this gate is
        # pos + ramp x sum |dv| over A's frames up to f (ramp = (1 - dt_B) / 2, dt_B = B's smallest step length; A's
        # horizontal speedF); speeds keep their own tolerance.
        dtb = min([v["dt"] for v in lb.values() if v["dt"] > 0] or [1.0])
        ramp = TOL["ramp"] if TOL["ramp"] >= 0 else (1.0 - dtb) / 2.0
        drift = {}
        ldrift = drift  # (the camera gate adds Link's drift: the centre follows Link)
        acc, prev = 0.0, None
        for f in sorted(la):
            sp = la[f]["speedF"]
            if prev is not None and f in lb and lb[f]["dt"] < 1.0:  # only where B runs Link at N steps
                acc += abs(sp - prev)
            prev = sp
            drift[f] = ramp * acc
        # the animation-phase lead rule (6d): foot-travel speed's phase and chord terms (foot_speed_tol)
        footv = foot_speed_tol(la, lb, link_step_speeds(B, fb, end)) if dtb < 1.0 else {}
        # the aim-boost stop rule (6e): the aim boost's carry at a speed change (aim_speed_tol)
        aimv = aim_speed_tol(la, lb) if dtb < 1.0 else {}
        # the target-facing turn rule (6f): target-facing turns from 6c-drifted positions (face_angle_tol)
        achg = {f for f in la if f - 1 in la and (la[f]["ang"] != la[f - 1]["ang"])}
        facev = face_angle_tol(la, lb, drift, achg, near) if dtb < 1.0 and TOL["face"] > 0 else {}
        # the process-order read rule (14i) interim (a): B's Link facing its own recorded eye of a later-in-line
        # converted target, the eye inside the enemy's own gates (--target-gate)
        f14ok, f14note = target_gate_ok()
        f14v = face14i_tol(la, lb, drift, f14ok) if dtb < 1.0 and TOL["face14i"] > 0 else {}
        f14n = 0
        for f, t in f14v.items():
            if t > facev.get(f, 0.0):
                facev[f] = t
                f14n += 1
        # the turn-factor sampling rule (6g): per-step turn factor while turning and accelerating (turn_accel_tol)
        turnv = turn_accel_tol(la, lb, TOL["turn"] if TOL["turn"] >= 0 else (1.0 - dtb) / 2.0) if dtb < 1.0 else {}
        # the animation-phase lead rule (6d(b)): the init-replaced animation's phase lead and its mapped carry; B's loop wraps
        animv, bloops = anim_lead_tol(la, B, fb, end, dtb) if dtb < 1.0 else ({}, {})
        # the carrier-offset rule (6j): on a carrier, B's rider positions minus the carrier's capped offset; the
        # carrier's offset is the carrier's, whether the rider runs at N steps or not (an unconverted Link riding the
        # converted ship: CONV 0xA5,0x26), so it does not depend on Link's dt_B
        coff = carrier_offsets(A, B, fa, fb, end, la, lb) if TOL["carrier"] > 0 else {}
        # the carrier-offset rule (6j), moving-BG carriers: frames the ship does not cover
        cbg = carrier_bg_offsets(A, B, fa, fb, end, converted_profiles(B, conv_given)[0], min(db) if db else 1.0) if TOL["carrier_bg"] > 0 else {}
        for f, (d, _, _) in cbg.items():
            coff.setdefault(f, d)
        # the speed-ramp position-gap rule (6c) contact propagation: Link's offset from an enemy's push / hit, within
        # the enemy's own gap at the contact (contact_offsets), carried on his later frames
        ctoff, ctnotes = contact_offsets(A, B, fa, fb, end, la, lb, admp_spec, ADMP_SPECS)
        # the animation-phase lead rule (6d) addendum: a running animation's rate steps (rate_step_tol)
        ratev = rate_step_tol(la, lb, dtb) if dtb < 1.0 else {}
        # the animation-phase lead rule (6d(c)): the change frame's pose gaps of the base foot (change_chord_tol)
        chgv, chgnotes = change_chord_tol(la, B, fb, end) if dtb < 1.0 else ({}, [])
        # the step-resolution threshold rule (6l): an event one frame early in B (threshold
        # crossing at step resolution): speed plausibility at B's event frame, the position term until convergence or the
        # next shared procedure change (event_shift_tol); added to the drift (the camera centre takes Link's offset)
        shared = [min(f1, f2) for (p1, f1), (p2, f2) in zip(ra[1:], rb[1:]) if p1 == p2 and abs(f1 - f2) <= TOL["proc_slack"]]
        evv, evplaus, evnotes, evspv, evwin = event_shift_tol(la, lb, B, fb, end, dtb, drift, shared)
        for f, t in evv.items():
            drift[f] = drift.get(f, 0.0) + t
        # the step-resolution threshold rule (6l) addendum (b): an animation switch up to 1 frame early or late while a 6l offset is carried
        animplaus, swnotes = carried_anim_switch(anim_switches(A, fa, end + 1 if end is not None else None), anim_switches(B, fb, end), evwin) if evwin else (set(), [])
        # the step-resolution threshold rule (6l) addendum (c): a carry window ended by a shared procedure change (not converged): plausibility from
        # there to the next shared state (the next shared procedure change: the landing); strict comparison resumes after
        # it once every gap is back within its normal tolerance, else the rest stays a reported, not failed, divergence
        episodes = []
        for g0, g1, c in evwin:
            if c is not None and all(c != e[0] for e in episodes):
                land = min([x for x in shared if x > c] or [None], key=lambda x: float("inf") if x is None else x)
                episodes.append((c, land))
        rows = {}
        for f in common:
            x, y = la[f], lb[f]
            if f > last or x["proc"] != y["proc"] or near(f):
                continue
            if TOL["tslack"] > 0 and f + 1 not in la:
                continue  # A's run ends here: no far side of the window (a one-sided window is not this gate; link-motion covers it)
            best = {}
            if f in coff:
                y = dict(y, pos=[p - q for p, q in zip(y["pos"], coff[f])])
            if f in ctoff:
                y = dict(y, pos=[p - q for p, q in zip(y["pos"], ctoff[f])])
            for t in slack_times(f):
                a = link_at(la, t)
                if a is None:
                    continue
                for nm, d in link_dists(a, y):
                    if nm == "anim" and f in bloops:  # B's loop wrapped (the animation-phase lead rule (6d(b))): modulo the loop
                        d = min(d, abs(bloops[f] - d))
                    if nm not in best or d < best[nm]:
                        best[nm] = d
            tols = {}
            for nm in best:
                tols[nm] = TOL[nm] + (drift.get(f, 0.0) if nm == "pos" else footv.get(f, 0.0) + chgv.get(f, 0.0) + aimv.get(f, 0.0) + turnv.get(f, 0.0) + evspv.get(f, 0.0) if nm == "speed"
                                      else facev.get(f, 0.0) if nm == "angle" else animv.get(f, 0.0) + ratev.get(f, 0.0))
            rows[f] = (best, tols)
        eplaus, epnotes, stuck = set(), [], []
        for c, land in episodes:
            stop = land if land is not None else float("inf")
            ep = [f for f in sorted(lb) if c <= f < stop and f <= last]
            eplaus.update(ep)
            # no stuck state: B's position and procedure frozen for 30 frames (or the whole episode) while A moves
            run_ = 0
            for i in range(1, len(ep)):
                f, g = ep[i], ep[i - 1]
                frozen = lb[f]["pos"] == lb[g]["pos"] and lb[f]["proc"] == lb[g]["proc"] and f in la and g in la and la[f]["pos"] != la[g]["pos"]
                run_ = run_ + 1 if frozen else 0
                if run_ >= min(30, len(ep) - 1):
                    stuck.append(f)
                    break
            if land is None:
                epnotes.append("rule note 6l (c): carry not converged at the shared change %d: plausibility to the end (no next shared state)" % c)
                continue
            resume = next((f for f in sorted(rows) if f >= land and all(rows[f][0][nm] <= rows[f][1][nm] for nm in rows[f][0])), None)
            later = [f for f in sorted(lb) if f >= land and f <= last and (resume is None or f < resume)]
            eplaus.update(later)
            epnotes.append("rule note 6l (c): carry not converged at the shared change %d: plausibility %d-%d (to the next shared state, the change at %d)%s" % (
                c, c, land - 1, land, "; strict again from %d" % resume if resume is not None and not later else
                "; the gap back within the normal tolerance at %d, strict from there (%d frame(s) after the landing reported)" % (resume, len(later)) if resume is not None else
                "; the gap never back within the normal tolerance: frames %d-%d a reported, not failed, divergence" % (land, later[-1]) if later else ""))
        sx = collections.Counter()
        swhere = {}
        sexcess = collections.Counter()
        stol = {}
        for f in common:
            x, y = la[f], lb[f]
            if f > last or x["proc"] != y["proc"] or near(f):
                continue
            if f in evplaus or f in eplaus:  # the step-resolution threshold rule (6l): B's early event frame / addendum (c)'s episode: plausibility (finite)
                if not all(math.isfinite(q) for q in [y["speedF"]] + y["speed"] + y["pos"] + [y["anim"]]):
                    sexcess["speed"], sx["speed"], swhere["speed"], stol["speed"] = float("inf"), float("nan"), f, 0.0
            if f in eplaus or f not in rows:
                continue
            best, tols = rows[f]
            for nm, d in best.items():
                if nm == "speed" and f in evplaus:
                    continue
                if nm == "anim" and f in animplaus:  # addendum (b): the switch a frame apart
                    continue
                tol = tols[nm]
                e = d - tol
                if nm not in stol or not (e <= sexcess[nm]):
                    sexcess[nm] = e
                    sx[nm] = d
                    swhere[nm] = f
                    stol[nm] = tol
        over = [nm for nm in ("pos", "speed", "angle", "anim") if nm in stol and not (sexcess[nm] <= 0.0)]
        if stuck:
            over.append("stuck")
        rep.gate("link-tslack", not over if n else None,
                 "%d frames, window +-%g frame; tightest |d| %s; pos tol = %g + %g x sum |dv| (rule note 6c); speed tol = %g + foot travel's phase lead and chord terms on %d frame(s) (rule note 6d) + the change or base-foot-switch frame's pose gaps on %d frame(s) (rule note 6d(c)) + the aim boost's carry on %d frame(s) (rule note 6e) + the turn factor on %d frame(s) (rule note 6g); angle tol = %g + the target-facing term on %d frame(s) (rule note 6f); anim tol = %g + the init-replaced animation's lead on %d frame(s) (rule note 6d(b)) + the running animation's rate steps on %d frame(s) (rule note 6d addendum); event one frame early (rule note 6l): pos term on %d frame(s), speed plausibility on %d frame(s), speed terms (slope, derived event_dv) on %d frame(s), animation switch a frame apart on %d frame(s) (b), unconverged-carry episodes plausibility on %d frame(s) (c); carrier offset subtracted on %d frame(s) (rule note 6j; moving BG %d)%s" % (n, TOL["tslack"], ", ".join(
                     "%s %.4g (frame %s, tol %.4g)" % (nm, sx[nm], swhere.get(nm, "-"), stol.get(nm, TOL[nm])) for nm in ("pos", "speed", "angle", "anim")),
                     TOL["pos"], ramp, TOL["speed"], len(footv), len(chgv), len(aimv), len(turnv), TOL["angle"], len(facev), TOL["anim"], len(animv), len(ratev), len(evv), len(evplaus), len(evspv), len(animplaus), len(eplaus), len(coff), len(cbg),
                     ("; STUCK: B frozen in an episode up to frame %s" % stuck[0] if stuck else "")
                     + ("; rule note 14i interim (a) (own-eye facing, later-in-line target): %d frame(s)%s, %s" % (
                         f14n, " (%s)" % ", ".join("%d +%d" % (f, f14v[f]) for f in sorted(f14v)[:4]) if f14v else "", f14note) if TOL["face14i"] > 0 else "")
                     + ("; rule note 6c contact propagation: offset carried on %d frame(s)" % len(ctoff) if TOL["contact"] > 0 else "")), chgnotes[:4] + evnotes[:6] + swnotes[:4] + epnotes[:4] + ["contact: " + x for x in ctnotes[:6]])
        # camera (main line only), frames where Link's procedures agree
        def camval(p):
            return [fnum(v) for v in p[5:12]]
        ca = {f: v[0] for f, v in state_channel(A, "cam.txt", fa, 1, lambda p: 0, camval, end, ncol=12).items()}
        cb = {f: v[0] for f, v in state_channel(B, "cam.txt", fb, 1, lambda p: 0, camval, end, ncol=12).items()}
        cf = [f for f in sorted(set(ca) & set(cb)) if f in la and f in lb and la[f]["proc"] == lb[f]["proc"] and not near(f)]
        if cf:
            dmax, fw = max((max(abs(p - q) for p, q in zip(ca[f], cb[f])), f) for f in cf)
            rep.gate("camera", dmax <= TOL["cam"], "%d frames; max |d| %.4g at frame %d (tol %g)" % (len(cf), dmax, fw, TOL["cam"]))
            # cam-tslack (time slack) with the speed-ramp position-gap rule (6c) (camera form), 6b and the ATT-B01 lag:
            # - the centre and fovy: A's trajectory within the window, tolerance cam (the centre must match);
            # - the eye: tolerance cam + D(f), the moving-target drift (the speed-ramp position-gap rule (6c) camera form):
            #   the eye chases a target (centre + direction, the radius pulled to its eased limits) that moves within the
            #   30 Hz frame; the N-step chase samples it per step, the 30 Hz one after the frame's whole move. Per frame
            #   D(f) = keep x D(f-1) + ramp x (|d(centre velocity)_A| + |dR_A|) from the conversion on: like link-tslack's term it is driven by changes of the motion (a steady chase keeps only v/6), and it decays with the measured persistence of the gap (keep 0.907: in pl3 the gap after the move start fell from its peak 14.0 at frame 244 below cam = 2 at frame 264, W = 20 frames, keep = (2 / 14)^(1/20); the direction is read back from the eye, so the gap outlives the eye chase's own 0.25); ramp as
            #   link-tslack's, (1 - dt_B) / 2; R = |eye - centre|, standing in for the radius limits);
            # - frames within proc_slack of a camera mode change in either run (the lock start / release lands on the
            #   tick30 step) or of a camera collision jump (the per-step camera-collision rule (6b): the bumped eye's radius jumps by more
            #   than a quarter in one frame, a bumpCheck line hit, in either run) are plausibility only: finite values.
            dtb = min([v["dt"] for v in lb.values() if v["dt"] > 0] or [1.0])
            ramp = TOL["ramp"] if TOL["ramp"] >= 0 else (1.0 - dtb) / 2.0

            def radius(v):
                return math.dist(v[0:3], v[3:6])
            r_s = 1.0 - 0.25 ** dtb
            chase_g = max(0.0, dtb * (1.0 - r_s) / r_s - 0.25 / 0.75)
            drift, D, pv, pvel = {}, 0.0, None, None
            clag, ma_ = {}, cam_modes(A, fa, end)
            clagv, sty_c = {}, cam_styles(A, fa, end)  # (the Link-following centre chase term per axis, A's style)
            for f in sorted(ca):
                v = ca[f]
                vel = [p - q for p, q in zip(v[3:6], pv[3:6])] if pv is not None else None
                if vel is not None and pvel is not None and f in lb and lb[f]["dt"] < 1.0:
                    D = TOL["cam_keep"] * D + ramp * (math.dist(vel, pvel) + abs(radius(v) - radius(pv)))
                else:
                    D = TOL["cam_keep"] * D
                # plus the steady chase lag difference g x |centre velocity|: a chase of ratio r = 0.75 behind a target moving
                # v per frame trails it by v (1 - r) / r at 30 Hz and by v dt (1 - r_s) / r_s at N steps (r_s = 1 - (1 - r)^dt)
                lagt = chase_g * math.dist(vel, [0.0, 0.0, 0.0]) if vel is not None and f in lb and lb[f]["dt"] < 1.0 else 0.0
                drift[f] = D + lagt
                # the speed-ramp position-gap rule (6c) camera, centre chase (pkk1d_4 / pkk3d_4 frame 54): while the centre
                # chases a target other than Link (A's lock-on camera: Link's lock-on slot set and a camera mode other than
                # the follow mode 0) it is itself a chase of a moving point and gets the same steady chase-lag term as the
                # eye, g x |centre velocity_A| (derived from the chase rule, not fitted); while it follows Link: no term
                if TOL["cchase"] and f in la and la[f]["tgt"] and la[f]["tgt"][0] is not None and ma_.get(f, "0") != "0":
                    clag[f] = lagt
                # the speed-ramp position-gap rule (6c) camera, Link-following centre (himo2 / r58a hungCamera: centre
                # follow 0.5 xz / 0.25 y): a centre that chases Link with a style ratio below 1 is a chase of a moving
                # point too; per axis it adds g(r_axis) x |A's centre velocity on that axis| (g from the style's own
                # ratios, chase_gain; derived, not fitted); frames already under the non-Link term above keep that one
                elif TOL["cchase"] and vel is not None and f in lb and lb[f]["dt"] < 1.0 and sty_c.get(f) in STYLE_CENTRE_RATIOS:
                    rxz, ry = STYLE_CENTRE_RATIOS[sty_c[f]]
                    gx, gy = chase_gain(rxz, lb[f]["dt"]), chase_gain(ry, lb[f]["dt"])
                    hl = [gx * abs(vel[0]), gy * abs(vel[1]), gx * abs(vel[2])]
                    if max(hl) > 0.0:
                        clagv[f] = hl
                pv, pvel = v, vel

            def marks(rows):  # frame -> radius jump (bumped eye pulled in / let out by more than a quarter)
                out = {}
                for f in sorted(rows):
                    if f - 1 in rows:
                        r0, r1 = radius(rows[f - 1]), radius(rows[f])
                        if r0 > 0 and abs(r1 - r0) > 0.25 * r0:
                            out[f] = r1 - r0
                return out
            # the speed-ramp position-gap rule (6c) camera (yaw episodes): the eye adds A's horizontal radius x the held yaw term while (and after,
            # decaying) A's camera runs a manualCamera style; see manual_yaw_tol
            myaw = manual_yaw_tol(ca, cam_styles(A, fa, end), {f: v["dt"] for f, v in lb.items()}, ramp, TOL["cam_keep"]) if TOL["myaw"] else {}
            ma, mb = cam_modes(A, fa, end), cam_modes(B, fb, end)
            # a 6b jump: both runs jump the same way (within half the size) within proc_slack frames - the same collision
            # decision taken a step apart; a jump only one run makes is a real difference and stays in the gate
            ja, jb = marks(ca), marks(cb)
            ev = set()
            for f, d in ja.items():
                for g, e in jb.items():
                    if abs(f - g) <= TOL["proc_slack"] and d * e > 0 and abs(d - e) <= 0.5 * max(abs(d), abs(e)):
                        ev.update((f, g))
            # the per-step camera-collision rule (6b) episode (cy): a jump only one run makes opens a plausibility window from
            # the first bump-state difference until both eyes converge (base tolerance), at most 30 frames (bump_episodes)
            bplaus, beps = bump_episodes(ca, cb, ja, jb, set(ev), cam_bumps(A, fa, end), cam_bumps(B, fb, end), TOL["cam"],
                                         30) if TOL["bump_ep"] > 0 else (set(), [])
            for mm in (ma, mb):
                ks = sorted(mm)
                for i in range(1, len(ks)):
                    if mm[ks[i]] != mm[ks[i - 1]]:
                        ev.add(ks[i])
            plaus = {f for f in cf if any(abs(f - e) <= TOL["proc_slack"] for e in ev)}
            plaus |= bplaus & set(cf)
            plaus |= eplaus & set(cf)  # the step-resolution threshold rule (6l) addendum (c): the unconverged-carry episode (the centre follows Link)
            sty_a = cam_styles(A, fa, end)
            ctype = cam_type_terms(cam_types(A, fa, end), cam_types(B, fb, end), ca, la, lb,
                                   {g: int(v) for g, v in sty_a.items() if str(v).isdigit()}) if TOL["cam_type"] > 0 else {}
            sw, sf, stol, bad, dw, ctol = -1e30, cf[0], TOL["cam"], [], (0.0, 0.0), TOL["cam"]
            # the carrier-offset rule (6j), camera part (an application of 6c's steady chase lag): on a carrier the
            # camera centre chases the carrier (rideCamera), not Link: it takes the 6c camera form's steady chase-lag
            # difference chase_g x |centre velocity_A| with the derived g (dt_B (1 - r_s)/r_s - 1/3, 1/6 at N = 2; no
            # fitted factor; pp5's tightest centre frame 344: 6.96 vs tol 10.1), and frames within the carrier's 6i
            # plausibility window (the ship's whole-frame decision shift: its offset jumps 0 -> 24.5 in one frame, the
            # chase takes it over ~3 frames) are plausibility only
            if TOL["carrier"] > 0:
                plaus |= {f for f in cf if f in coff and f in CARRIER_PLAUS}
            ymax = (0.0, None)
            shipn = [0]
            attn, holdn = [0], [0]
            cdt_b = cam_dts(B, fb, end)
            satt = ship_attitude(A, B, fa, fb, end) if TOL["ship_att"] > 0 and coff else {}
            holdt = cam_hold_terms(ca, cb) if TOL["cam_hold"] > 0 else {}
            # the carrier-offset rule (6j) addendum (evship_cam / evcam_offship, default on): an event camera
            # (type 3, style 4 in both runs) whose centre target is the ship follows a whole-form / unconverted ship's
            # accepted drift (6c-propagation class): its eye does not take the ship's offset 1:1 (the pull-back eye), so the
            # eye tolerance adds the ship's frame-end |B - A| offset capped by the ship gate's xz allowance. cam.txt names no
            # event-camera target: the centre is on the ship when it lies within evship_r of the ship's position in both runs
            # (127-270 on the ship, 4800 on the fixed shot after it). evcam_offship (ruled in): the same camera with
            # its centre off the ship does not ride the carrier, so 6j's offset is not subtracted from it.
            evtyp = (cam_types(A, fa, end), cam_types(B, fb, end), cam_styles(B, fb, end)) \
                if (TOL["evship_cam"] > 0 or TOL["evcam_offship"] > 0) and SHIP_ALLOW else None
            evsn, evon, evmax = [0], [0], [0.0, None]

            def ev_ship(g):
                """None: not the event camera (or no ship dump); True / False: its centre on / off the ship in both runs"""
                if evtyp is None or g not in SHIP_ALLOW or g not in cb:
                    return None
                if evtyp[0].get(g) != "3" or evtyp[1].get(g) != "3" or sty_a.get(g) != 4 or evtyp[2].get(g) != 4:
                    return None
                _, _, pa_, pb_ = SHIP_ALLOW[g]
                return math.dist(ca[g][3:6], pa_) <= TOL["evship_r"] and math.dist(cb[g][3:6], pb_) <= TOL["evship_r"]
            evoffn, heldn, heldf = [0], [0], []
            heldt = held_gap_terms(ca, cb, [g for g in cf if g not in plaus], TOL["cam"]) if TOL["held_gap"] > 0 else {}
            for f in cf:
                if f in plaus:
                    if not all(math.isfinite(x) for x in cb[f]):
                        bad.append(f)
                    continue
                best_e = best_r = float("inf")
                c3 = coff.get(f, [0.0, 0.0, 0.0])  # the carrier-offset rule (6j): the carrier's capped offset off the rider's camera
                l3 = c3  # Link's part keeps the carrier offset (he rides it)
                evs = ev_ship(f)
                evoff = evs is False and TOL["evcam_offship"] > 0 and TOL["ev_cam_off"] > 0 and f in coff
                if evoff:
                    # 6j addendum / (2026-11-19: chosen by target): the event camera off the ship does not ride
                    # the carrier
                    c3 = [0.0, 0.0, 0.0]
                    evon[0] += 1
                    evoffn[0] += 1
                if TOL["carrier_cam_y"] <= 0:
                    # the camera takes the offset's xz part only: it does not follow the ship's float bob 1:1 (pd1, Link
                    # and camera unconverted, ship y 3.4 apart: the eye 0.82 off as recorded, 2.47 with the y part
                    # subtracted); Link rides rigidly and takes the whole capped vector
                    c3 = [c3[0], 0.0, c3[2]]
                vb = [v - c3[i % 3] if i < 6 else v for i, v in enumerate(cb[f])]
                hv = clagv.get(f, [0.0, 0.0, 0.0]) + [0.0]  # Link-following centre chase: per axis (eye and centre)
                for a in (vec_at(ca, t) for t in slack_times(f)):
                    if a is None:
                        continue
                    best_e = min(best_e, max(abs(p - q) - h for p, q, h in zip(a[0:3], vb[0:3], hv)))
                    best_r = min(best_r, max(abs(p - q) - h for p, q, h in zip(a[3:7], vb[3:7], hv)))
                # Link's part: his frame-end offset (minus the carrier's, 6j), capped by his 6c drift
                loff = max(abs(p - q - r) for p, q, r in zip(lb[f]["pos"], la[f]["pos"], l3)) if f in la and f in lb else 0.0
                tol_c = TOL["cam"] + min(ldrift.get(f, 0.0), loff) + clag.get(f, 0.0)
                if f in coff and TOL["carrier_cam"] > 0 and f - 1 in ca:
                    # (ptalk2): the style's own centre ratio / the ride entry's transition share
                    tol_c += TOL["carrier_cam"] * carrier_chase_g(sty_a, f, dtb) * math.dist(ca[f][3:6], ca[f - 1][3:6])
                tol_e = tol_c + drift.get(f, 0.0)
                ym = myaw.get(f, (0.0, 0.0))
                tol_e = tol_c + drift.get(f, 0.0) + ym[0] * ym[1]
                # (930 / talk13 965): an unconverted camera on a converted ship within the
                # ship gate: the ship's accepted attitude (pitch / float y) difference read by the latitude
                ci0 = CARRIER_INFO.get(f) if f in coff else None
                if TOL["ship_att"] > 0 and ci0 is not None and ci0[1] and cdt_b.get(f, 0.0) >= 1.0 and sty_a.get(f) in RIDE_STYLES:
                    at = ship_att_term(satt, f, math.dist(ca[f][0:3], ca[f][3:6]), coff)
                    if at > 0.0:
                        tol_e += at
                        attn[0] += 1
                # (sv2 135): a held direction latched within the time window
                if holdt.get(f, 0.0) > 0.0:
                    tol_e += holdt[f]
                    holdn[0] += 1
                tol_c += ctype.get(f, 0.0)  # 6l addendum (g) on the camera: the centre only
                evt = 0.0
                if evs and TOL["evship_cam"] > 0:
                    cap_, n_ = SHIP_ALLOW[f][:2]
                    evt = TOL["evship_cam"] * min(n_, cap_) if cap_ > 0 else 0.0
                    tol_e += evt
                    if evt > 0.0:
                        evsn[0] += 1
                        if evt > evmax[0]:
                            evmax[:] = [evt, f]
                if ym[0] > ymax[0]:
                    ymax = (ym[0], f)
                ex = max(best_e - tol_e, best_r - tol_c)
                # the camera on the ship (pairs 960 / 630): while Link rides the
                # ship and the ship gate passes on the frame, the camera may be off by the ship's accepted offset. The
                # chased centre does not take the offset 1:1 (the pair's sea-edge brake 627-641: the event camera's centre
                # 25.8 off while the ship's offset falls 28.7 -> 18.5): a chase (any ratio in (0, 1]) of targets offset
                # by d(t) is off by a convex combination of the past d(t). Second form: B's eye and centre less the point
                # of the polyline of the ship's capped offsets (xz: the gate's bound; y: rule (c)'s band, 6j) over the last 30 frames (the carry window) closest to
                # B's centre offset (within 0.06 of it), against the base tolerances (the boat styles' centre takes the ship's float bob too: 544, y 2.54); directional, so a real
                # rider / camera error is no allowance. In a ride style (its yaw follows the ship's heading) the eye
                # adds A's eye radius x |ship facing B - A| (both forms). The better form counts.
                ci = CARRIER_INFO.get(f) if (f in coff and TOL["ship_cam"] > 0 and not evoff) else None
                if ci is not None and ci[1]:
                    yterm = math.dist(ca[f][0:3], ca[f][3:6]) * abs(ci[2]) if sty_a.get(f) in RIDE_STYLES else 0.0
                    poly = [tuple(coff[g]) for g in range(f - 30, f + 1) if g in coff]
                    cd3 = [cb[f][3 + k] - ca[f][3 + k] for k in range(3)]
                    q, qd = poly[-1], float("inf")
                    for i in range(len(poly)):
                        p0, p1 = poly[i], poly[min(i + 1, len(poly) - 1)]
                        u = [p1[k] - p0[k] for k in range(3)]
                        L = sum(x * x for x in u)
                        t = 0.0 if L == 0 else max(0.0, min(1.0, sum((cd3[k] - p0[k]) * u[k] for k in range(3)) / L))
                        pt = [p0[k] + t * u[k] for k in range(3)]
                        dd = math.dist(cd3, pt)
                        if dd < qd:
                            q, qd = pt, dd
                    vq = [v - q[i % 3] if i < 6 else v for i, v in enumerate(cb[f])]
                    be2 = br2 = float("inf")
                    for a in (vec_at(ca, t) for t in slack_times(f)):
                        if a is None:
                            continue
                        be2 = min(be2, max(abs(p - q_) for p, q_ in zip(a[0:3], vq[0:3])))
                        br2 = min(br2, max(abs(p - q_) for p, q_ in zip(a[3:7], vq[3:7])))
                    tc2 = TOL["cam"] + clag.get(f, 0.0) + ctype.get(f, 0.0)
                    te2 = tc2 + drift.get(f, 0.0) + ym[0] * ym[1] + yterm + evt
                    ex1 = max(best_e - tol_e - yterm, best_r - tol_c)
                    ex2 = max(be2 - te2, br2 - tc2)
                    if min(ex1, ex2) < ex:
                        shipn[0] += 1
                        if ex2 < ex1:
                            ex, tol_e, tol_c, best_e, best_r = ex2, te2, tc2, be2, br2
                        else:
                            ex, tol_e = ex1, tol_e + yterm
                # the held camera-gap rule (6m), held-gap angle allowance (talk16 / talk18 847): a gap held from a
                # smaller radius is judged by angle (theta x R_min against the base tolerance), with equal radii and the
                # centre within its own tolerance
                hg = heldt.get(f)
                if hg is not None:
                    exk = max(hg[2] * max(0.0, hg[0] - hg[1]) - TOL["cam"], hg[0] * hg[4] - TOL["cam"], hg[3] - TOL["cam"], best_r - tol_c)
                    if exk < ex:
                        ex = exk
                        best_e, tol_e = hg[2] * max(0.0, hg[0] - hg[1]), TOL["cam"]
                        heldn[0] += 1
                        heldf.append(f)
                if ex > sw:
                    sw, sf, stol, dw, ctol = ex, f, tol_e, (best_e, best_r), tol_c
            ok = sw <= 0.0 and not bad
            rep.gate("cam-tslack", ok, "%d frames (%d plausibility only: mode change / 6b jump / 6l (c) episode), window +-%g frame; tightest frame %d: eye %.4g (tol %.4g = %g + Link's and the eye's drift, rule note 6c), centre/fovy %.4g (tol %.4g = %g + Link's offset, capped by his 6c drift); manual yaw term (rule note 6c camera, yaw episodes) on %d frame(s), largest %.3g deg at frame %s (adds %.4g there at the tightest frame); camera type change one frame early (6l addendum (g)): centre term on %d frame(s)%s; centre chasing a non-Link target (6c centre chase): g |v| on %d frame(s), %.4g at the tightest frame; Link-following centre chase (style ratio < 1): g |v| per axis on %d frame(s), %s at the tightest frame (subtracted); camera on the ship (accepted ship offset / heading) on %d frame(s); ship attitude (unconverted camera) on %d frame(s); held direction latched in the window on %d frame(s)%s%s; 6b episodes (a bumped-eye jump in one run): %s" % (
                len(cf), len(plaus), TOL["tslack"], sf, dw[0], stol, TOL["cam"], dw[1], ctol, TOL["cam"], len(myaw), math.degrees(ymax[0]), ymax[1],
                myaw.get(sf, (0.0, 0.0))[0] * myaw.get(sf, (0.0, 0.0))[1], len(ctype),
                " (%s)" % ", ".join("frame %d +%.3g" % (g, ctype[g]) for g in sorted(ctype)[:4]) if ctype else "", len(clag), clag.get(sf, 0.0), len(clagv), "/".join("%.3g" % x for x in clagv.get(sf, [0.0, 0.0, 0.0])), shipn[0], attn[0], holdn[0],
                "; event camera on the ship (6j addendum, evship_cam): ship offset term on %d frame(s), largest %.4g at frame %s; off the ship, carrier offset not subtracted (evcam_offship) on %d frame(s)" % (evsn[0], evmax[0], evmax[1], evon[0]) if evtyp is not None else "",
                "; not finite at %s" % bad[:4] if bad else "",
                ", ".join("frames %d-%d (jump %d, %s)" % (s0, e0 - 1, j0, "converged at %d" % e0 if ok0 else "not converged within 30: no window")
                          for s0, j0, e0, ok0 in beps) or "none") +
                "; event camera off the ship without the carrier offset (fst8 rule, by target) on %d frame(s); held-gap angle allowance (rule note 6m) on %d frame(s)%s" % (
                    evoffn[0], heldn[0], " (largest at frame %d: %.3g deg, held %.3g deg, R %.4g)" % max(((g, math.degrees(heldt[g][0]), math.degrees(heldt[g][1]), heldt[g][2]) for g in heldf), key=lambda x: x[1]) if heldf else ""))
        else:
            rep.gate("camera", None, "no camera trace")
            rep.gate("cam-tslack", None, "no camera trace")

    # the carrier-offset rule (6j), moving-background carriers (detection only, report): the frames on which the camera finds a moving BG
    # actor under the player in both runs (the same process), and the carrier's B - A offset there
    mva, mvb = cam_movebg(A, fa, end), cam_movebg(B, fb, end)
    mvf = sorted(set(mva) & set(mvb))
    conv_r = converted_profiles(B, conv_given)[0]
    if mvf:
        both = [f for f in mvf if mva[f] and mvb[f] and mva[f][0] == mvb[f][0]]
        one = [f for f in mvf if bool(mva[f]) != bool(mvb[f]) or (mva[f] and mvb[f] and mva[f][0] != mvb[f][0])]
        names = sorted({mva[f][0] for f in both})
        off = max(((math.dist(mva[f][1], mvb[f][1]), f) for f in both), default=(0.0, None))
        cbg_r = carrier_bg_offsets(A, B, fa, fb, end, conv_r, min(db) if db else 1.0)
        over_cap = [f for f in both if f in cbg_r and math.dist(mva[f][1], mvb[f][1]) > cbg_r[f][1]]
        rep.gate("carrier-bg", None, "report (rule note 6j detection): moving BG carrier under the player on %d of %d frames in both runs (%s), max |B - A| offset %.4g (frame %s); %d frame(s) in one run only or another carrier%s; offset over its Q7 cap (pos + (1 - dt_B) |v_A| converted, pos unconverted) on %d frame(s)%s; the capped offset is subtracted from the rider in link-tslack / cam-tslack" % (
            len(both), len(mvf), ", ".join(names) or "-", off[0], off[1], len(one), " (first %s)" % one[:4] if one else "",
            len(over_cap), " (first %s)" % over_cap[:4] if over_cap else ""))
    else:
        rep.gate("carrier-bg", None, "no moving-BG field in cam.txt (older trace)")

    # the RNG-drift plausibility rule (8b): the first RNG difference and the converted profiles (B's in-between-step executes)
    rngf, _, _ = rng_first(A, B, fa, fb, end)
    conv, conv_src = converted_profiles(B, conv_given)
    # the strict RNG-pair rule (8d): the --plaus-spec actors' processes and sound starts after the first RNG difference are plausibility
    plaus_prof = {sp["profile"] for sp in plaus_specs if sp["profile"]}
    plaus_code = [r for sp in plaus_specs for r in sp["code"]]
    # sound starts: (wrapper, id), matched within se_slack frames
    kfa, kfb = se_keyf(A), se_keyf(B)
    sea = event_channel(A, "se.txt", fa, 0, kfa, end)
    seb = event_channel(B, "se.txt", fb, 0, kfb, end)
    if sea or seb:
        # the frames both runs reached (Link's trace; a run stopped at its end may lack the last lines): starts later than
        # the common last frame minus the slack cannot be matched reliably and are left out
        if link_last is not None:
            last = link_last - TOL["se_slack"]
            sea = [x for x in sea if x[0] <= last]
            seb = [x for x in seb if x[0] <= last]
        amb_a = [x for x in sea if se_ambient(x[1])]
        amb_b = [x for x in seb if se_ambient(x[1])]
        sea = [x for x in sea if not se_ambient(x[1])]
        seb = [x for x in seb if not se_ambient(x[1])]
        pool = collections.defaultdict(list)
        for f, k in seb:
            pool[k].append(f)
        un_a = []
        for f, k in sorted(sea):
            c = [g for g in pool[k] if abs(g - f) <= TOL["se_slack"]]
            if c:
                # the earliest free start inside the window (A taken in frame order): a maximum matching on the line, so
                # a whole series a frame early or late pairs start for start (the nearest-first pick left B's first and
                # A's last start of a series one frame early unmatched: sword pair)
                pool[k].remove(min(c))
            else:
                un_a.append((f, k))
        un_b = sorted((f, k) for k, fs in pool.items() for f in fs)
        # a sound object's start keyed by a stale guest caller (se_w4_ids): paired by its sound id
        un_a, un_b, n_w4 = se_w4_pair(un_a, un_b, se_w4_ids(A, fa, end), se_w4_ids(B, fb, end), TOL["se_slack"])
        # the early ship-decision rule (6i(ii)): after a ship decision B makes LATER than A (an
        # unconverted driver's switch seen on the tick30 step), (a) B's steps before its switch step still ran the old
        # procedure: B-only starts on B's switch frame whose key A started in its last proc_slack frames before its own
        # switch are that share; (b) the rope-timed ship sounds trail by the derived D frames (ship_followon: rope
        # difference x the winding rate): matched within se_slack + D inside the follow-on window, and the run with the
        # longer rope may play up to D more per-frame work starts there
        n_fo, fo_gap, has_later = 0, [], False
        if TOL["ship_later"] > 0:
            ssa = ship_rows(os.path.join(A, ship_name), fa, end)
            ssb = ship_rows(os.path.join(B, ship_name), fb, end)
            if ssa and ssb:
                sdtb = min(ssb[f]["dt"] for f in set(ssa) & set(ssb)) if set(ssa) & set(ssb) else 1.0
                shs, _, _, _ = ship_shift_tol(ssa, ssb, sdtb)
                for sfb, sfa, kind, _, dr in shs:
                    if dr != "later":
                        continue
                    has_later = True
                    olda = {k for f, k in sea if sfa - TOL["proc_slack"] <= f < sfa}
                    keep = [x for x in un_b if not (x[0] == sfb and x[1] in olda)]
                    n_fo += len(un_b) - len(keep)
                    un_b = keep
                for w in ship_followon(ssa, ssb, shs):
                    inw = lambda x: w["lo"] <= x[0] <= w["end"] and x[1] in SHIP_ROPE_SE
                    for x in sorted(filter(inw, un_a)):
                        c = [y for y in un_b if y[1] == x[1] and inw(y) and abs(y[0] - x[0]) <= TOL["se_slack"] + w["D"]]
                        if c:
                            y = min(c, key=lambda y: abs(y[0] - x[0]))
                            un_a.remove(x)
                            un_b.remove(y)
                            n_fo += 2
                            fo_gap.append((abs(y[0] - x[0]), x[1], x[0], y[0], w["D"]))
                    # per-frame work starts: the longer rope plays up to D more at its end, and A's whole-frame new
                    # procedure starts its work sound on the switch frame where B's steps still play the old one's (a
                    # run's end may cut either): at most D left over per key, either run
                    for key in SHIP_ROPE_WORK_SE:
                        xs = [(lst, x) for lst in (un_a, un_b) for x in lst if x[1] == key and inw(x)]
                        if len(xs) <= w["D"]:
                            for lst, x in xs:
                                lst.remove(x)
                            n_fo += len(xs)
        # rule: inside the crane / salvage window the event-driven follow-on starts match within se_slack + D
        n_cr, cr_per, cr_rest = 0, [], []
        if CRANE:
            for x in sorted(x for x in un_a if in_crane(x[0])):
                c = [y for y in un_b if y[1] == x[1] and in_crane(y[0]) and abs(y[0] - x[0]) <= TOL["se_slack"] + CRANE["D"]]
                if c:
                    y = min(c, key=lambda y: abs(y[0] - x[0]))
                    un_a.remove(x)
                    un_b.remove(y)
                    n_cr += 2
            # rule addendum (2026-11-16, pcr10): a periodic sound inside the window (the crane
            # swing 0x381E: the event-timed spring starts late in one run and fits one more swing before the lift-up) may
            # differ in count by at most one; the instances otherwise keep the same spacing within se_slack + D
            for key in sorted({x[1] for x in un_a + un_b if in_crane(x[0])}):
                lft = [(lst, x) for lst in (un_a, un_b) for x in lst if x[1] == key and in_crane(x[0])]
                if crane_periodic_extra([f for f, k in sea if k == key and in_crane(f)],
                                        [f for f, k in seb if k == key and in_crane(f)],
                                        [x[0] for _, x in lft], TOL["se_slack"] + CRANE["D"]):
                    lft[0][0].remove(lft[0][1])
                    n_cr += 1
                    cr_per.append("%s A %d / B %d" % ("/".join(key), sum(k == key and in_crane(f) for f, k in sea), sum(k == key and in_crane(f) for f, k in seb)))
            # (after rule (c)): the rope-timed starts (the
            # follow-ons and procCraneUp's lift-test record, SHIP_ROPE_SE) that still exceed se_slack + D in the window
            # after a 6i(ii) B-later crane-up shift (N 4: B's old procedure share before the switch froze the water and
            # rope of the lift test differently) fall under the plausibility window until: reported, not failed,
            # when the key plays in both runs' window (counts within one; a rope work sound: present in both); no
            # per-event bound
            if TOL["crane_rest"] > 0 and has_later:
                def rest_ok(x):
                    na = sum(k == x[1] and in_crane(f) for f, k in sea)
                    nb = sum(k == x[1] and in_crane(f) for f, k in seb)
                    # a per-frame work sound (SHIP_ROPE_WORK_SE) plays once per frame of rope: its count differs by the
                    # timing gap itself, so only its presence in both runs is asked
                    return in_crane(x[0]) and x[1] in SHIP_ROPE_SE and na and nb and (abs(na - nb) <= 1 or x[1] in SHIP_ROPE_WORK_SE)
                cr_rest = [("A",) + x for x in un_a if rest_ok(x)] + [("B",) + x for x in un_b if rest_ok(x)]
                un_a = [x for x in un_a if not rest_ok(x)]
                un_b = [x for x in un_b if not rest_ok(x)]
        # the RNG-drift plausibility rule (8b): after the first RNG difference, unmatched starts of unconverted code are plausibility only
        half_keys = {kfb(l.split()) for l in lines(B, "se.txt") if SE_LINE.match(l.strip()) and l.split()[1] == "0"}
        plaus_keys = set()
        for run, kf in ((A, kfa), (B, kfb)):
            for l in lines(run, "se.txt"):
                q = l.split()
                if SE_LINE.match(l.strip()) and any(lo <= int(q[4], 16) < hi for lo, hi in plaus_code):
                    plaus_keys.add(kf(q))
        rngt = lambda x: (cut is not None and x[0] >= cut) or (rngf is not None and x[0] >= rngf and (x[1] in plaus_keys or (conv != "all" and not se_converted(x[1], half_keys, conv))))
        n_rt = sum(map(rngt, un_a + un_b))
        un_a = [x for x in un_a if not rngt(x)]
        un_b = [x for x in un_b if not rngt(x)]
        # the step-resolution threshold rule (6l) addendum (c): in an unconverged-carry episode, sounds within their slack: an unmatched start whose
        # key the other run starts within se_slack frames (a per-frame series that ends or begins a frame apart: the pair's
        # hat swing in the wind, 100-215 in A, 100-216 in B) is plausibility
        ep_a = {(f, k) for f, k in sea if f in eplaus}
        ep_b = {(f, k) for f, k in seb if f in eplaus}
        epok = lambda x, other: x[0] in eplaus and any(k == x[1] and abs(g - x[0]) <= TOL["se_slack"] for g, k in other)
        n_ep = sum(epok(x, ep_b) for x in un_a) + sum(epok(x, ep_a) for x in un_b)
        un_a = [x for x in un_a if not epok(x, ep_b)]
        un_b = [x for x in un_b if not epok(x, ep_a)]
        # the step-resolution threshold rule (6l) addendum (e) (psword2c's SWORD_CHARGE 0x203B: B 54 starts, A 53): a sound
        # started on every frame (a series of consecutive-frame starts of one key) may differ in its start count by an
        # accepted procedure shift in frames: one edge of the series aligned, the other shifted by exactly the count
        # difference, and a link-procs change (same procedure in both runs, within proc_slack) shifted the same way
        # within proc_slack + se_slack frames of that edge; a larger difference, or none such change, fails
        ser_cause = []

        def series(starts, k, f):
            fs = {g for g, kk in starts if kk == k}
            if f not in fs:
                return None
            lo, hi = f, f
            while lo - 1 in fs:
                lo -= 1
            while hi + 1 in fs:
                hi += 1
            return lo, hi

        def ser_ok(un, own, other, b_side):
            out, done = [], {}
            for f, k in un:
                so = series(own, k, f)
                if so is None or so[1] - so[0] < 1:
                    out.append((f, k))
                    continue
                if (k, so) not in done:
                    near = {series(other, k, g) for g, kk in other if kk == k and so[0] - TOL["se_slack"] <= g <= so[1] + TOL["se_slack"]}
                    near.discard(None)
                    cause = None
                    if len(near) == 1:
                        st = near.pop()
                        extra = (so[1] - so[0]) - (st[1] - st[0])
                        d0, d1 = st[0] - so[0], so[1] - st[1]  # own run earlier at the start / later at the end (> 0)
                        n_un = sum(1 for g, kk in un if kk == k and so[0] <= g <= so[1])
                        if extra > 0 and n_un <= extra and sorted((d0, d1)) == [0, extra]:
                            edge = so[0] if d0 else so[1]
                            for p_, fa_, fb_ in proc_shifts:
                                fo, ft = (fb_, fa_) if b_side else (fa_, fb_)
                                if (ft - fo == extra if d0 else fo - ft == extra) and abs(fo - edge) <= TOL["proc_slack"] + TOL["se_slack"]:
                                    cause = "%s: %s %d-%d (%d) vs %s %d-%d (%d), procedure %s change A %d / B %d" % (
                                        k, "B" if b_side else "A", so[0], so[1], so[1] - so[0] + 1, "A" if b_side else "B", st[0], st[1],
                                        st[1] - st[0] + 1, p_, fa_, fb_)
                                    break
                    done[(k, so)] = cause
                    if cause:
                        ser_cause.append(cause)
                if not done[(k, so)]:
                    out.append((f, k))
            return out

        n0 = len(un_a) + len(un_b)
        un_a = ser_ok(un_a, sea, seb, False)
        un_b = ser_ok(un_b, seb, sea, True)
        n_ser = n0 - len(un_a) - len(un_b)
        # event-start world sound (107D from fpcM_Management A-only at
        # frame 61, Link's FoodThrow event start; the 8a / 8c family like sw61a's freeze-frame world sounds): an unmatched
        # start is accepted only when (1) its frame is an event-start frame (either run's camera enters an event style
        # there), (2) every caller of it is a world-sound caller (WORLD_SE_CALLERS: the process manager, not a process),
        # (3) it is a per-frame sound: its run starts the same key on the frame before or after, and so does the other
        # run, and (4) it is the run's only unmatched start of that key on that frame
        n_evs, evs_note = 0, []
        if TOL["ev_start_se"] > 0 and (un_a or un_b):
            evst = event_start_frames(A, fa, end) | event_start_frames(B, fb, end)
            if evst:
                cal_a, cal_b = se_callers(A, fa, kfa, end), se_callers(B, fb, kfb, end)
                fk_a, fk_b = set(sea), set(seb)

                def evs_keep(un, own, other, cal, side):
                    cnt = collections.Counter(un)
                    out = []
                    for f, k in un:
                        if (f in evst and cnt[(f, k)] == 1 and se_world(cal.get((f, k)))
                                and ((f - 1, k) in own or (f + 1, k) in own) and ((f - 1, k) in other or (f + 1, k) in other)):
                            evs_note.append("%s %d %s" % (side, f, "/".join(k)))
                            continue
                        out.append((f, k))
                    return out

                n0 = len(un_a) + len(un_b)
                un_a = evs_keep(un_a, fk_a, fk_b, cal_a, "A")
                un_b = evs_keep(un_b, fk_b, fk_a, cal_b, "B")
                n_evs = n0 - len(un_a) - len(un_b)
        if rngf is not None or cut is not None:
            rngf0 = min(x for x in (rngf, cut) if x is not None)
            # the RNG-drift plausibility rule (8b) addendum (question 3, pj2): an unconverted RNG-timed source
            # found in one run only after the first RNG difference is tolerated when all its starts lie within the run's
            # last W frames, W = the longest interval between the source's starts in A's whole run (the whole remaining
            # window from the RNG difference on when A plays it once; never when A never plays it); converted sources
            # never reach this check (rngt) and stay strict in `sound`
            se_end = last if la and lb else max([f for f, _ in sea + seb] or [0])
            a_all = collections.defaultdict(list)
            for f, k in event_channel(A, "se.txt", fa, 0, kfa, end):
                a_all[k].append(f)
            def rng_key_ok(k, frames):
                fs = sorted(set(a_all.get(k, [])))
                if not fs:
                    return False
                w = max(b - a for a, b in zip(fs, fs[1:])) if len(fs) > 1 else se_end - rngf0 + 1
                return all(f > se_end - w for f in frames)
            ok, msg, det = ambient_check([x for x in sea if rngt(x)], [x for x in seb if rngt(x)], rng_key_ok)
            rep.gate("sound-rng", ok, "rule note 8b: unconverted starts from frame %s (first RNG difference) on%s, plausibility; %s; %d unmatched left to it" % (
                "-" if rngf is None else rngf, "" if cut is None else ", every start from frame %d (strict cut, 8d addendum)" % cut, msg, n_rt), det[:5])
        else:
            rep.gate("sound-rng", None, "no RNG difference")
        rep.gate("sound", not un_a and not un_b, "%d / %d starts, unmatched A %d, B %d (slack %d)%s" % (len(sea), len(seb), len(un_a), len(un_b),
                                                                                                    TOL["se_slack"], ("; %d sound-object start(s) paired by sound id (stale caller)" % n_w4 if n_w4 else "") + ("; %d start(s) placed by 6i(ii) (B-later shift: old procedure's share, rope follow-on%s)" % (n_fo, "; largest follow-on gap %d frame(s) (%s A %d / B %d; derived D %d + se_slack %d)" % (max(fo_gap) + (TOL["se_slack"],)) if fo_gap else "") if n_fo else "") + ("; %d start(s) of a series a frame apart in a 6l (c) episode, plausibility" % n_ep if n_ep else "") + ("; %d extra start(s) of a per-frame series = an accepted procedure shift (6l (e)): %s" % (n_ser, "; ".join(ser_cause[:3])) if n_ser else "") + ("; %d per-frame world sound start(s) on an event-start frame in one run only (rule 2026-11-18, esa egr1 61): %s" % (n_evs, ", ".join(evs_note[:4])) if n_evs else ""))
                 + ("; crane/salvage window from frame %d (plausibility until M5): %d start(s) matched within se_slack + D %d" % (CRANE["lo"], n_cr, CRANE["D"]) if CRANE else "")
                 + ("; periodic count +-1 (Q8 addendum): %s" % ", ".join(cr_per) if cr_per else "")
                 + ("; %d start(s) in the window beyond se_slack + D left to plausibility (rule (b), until M5): %s" % (len(cr_rest), ", ".join("%s %d %s" % (r, f, "/".join(k)) for r, f, k in cr_rest[:6])) if cr_rest else ""),
                 ["A only: %s" % un_a[:5]] * bool(un_a) + ["B only: %s" % un_b[:5]] * bool(un_b))
        if amb_a or amb_b:
            ok, msg, det = ambient_check(amb_a, amb_b)
            if not ok and keyed_proofs:
                ok, msg, det = ambient_keyed_accept(msg, det, keyed_proofs)
            rep.gate("sound-amb", ok, msg, det[:5])
        else:
            rep.gate("sound-amb", None, "no ambient sound starts")
    else:
        rep.gate("sound", None, "no se.txt")
        rep.gate("sound-amb", None, "no se.txt")

    # processes: lifetimes per profile
    def procs(run, fr):
        life = {}
        for l in lines(run, "digest.txt"):
            p = l.split()
            if len(p) < 6 or not p[0].isdigit():
                continue
            f = fr.frame(int(p[0]))
            if f is None or (end is not None and f > end):
                continue
            pid, prof = p[1], p[2]
            if pid in life:
                life[pid][2] = f
            else:
                life[pid] = [prof, f, f]
        byprof = collections.defaultdict(list)
        for pid, (prof, f0, f1) in life.items():
            byprof[prof].append((f0, f1))
        return byprof
    pa, pb = procs(A, fa), procs(B, fb)
    if pa or pb:
        lastf = min(max((x[1] for v in pa.values() for x in v), default=0), max((x[1] for v in pb.values() for x in v), default=0))
        det = []
        only = sorted(set(pa) ^ set(pb))
        if only:
            det.append("profiles in one run only: %s" % ", ".join("%s(%s)" % (p, "A" if p in pa else "B") for p in only[:8]))
        pdet, n_pr = [], 0
        for prof in sorted(set(pa) & set(pb)):
            xa, xb = sorted(pa[prof]), sorted(pb[prof])
            # the RNG-drift plausibility rule (8b): an unconverted profile whose processes live on after the first RNG difference: their starts
            # before it stay strict, the rest is plausibility (counts and the number alive at the end within the noise of
            # RNG-timed counts, no process of B living on where A's all ended: stuck)
            th = [rngf] if rngf is not None and ((conv != "all" and prof not in conv) or prof in plaus_prof) else []
            th = min(th + ([cut] if cut is not None else []), default=None)
            if th is not None and any(x[1] >= th for x in xa + xb):
                n_pr += 1
                pre_a = [x for x in xa if x[0] < th]
                pre_b = [x for x in xb if x[0] < th]
                if len(pre_a) != len(pre_b) or any(abs(a[0] - b[0]) > TOL["proc_slack"] for a, b in zip(pre_a, pre_b)):
                    det.append("profile %s: starts before frame %d (first RNG difference / strict cut) differ: %s / %s" % (prof, th, pre_a[:3], pre_b[:3]))
                    continue
                na, nb = len(xa), len(xb)
                ea = sum(1 for x in xa if x[1] >= lastf - TOL["proc_slack"])
                eb = sum(1 for x in xb if x[1] >= lastf - TOL["proc_slack"])
                if abs(na - nb) > amb_allowed(na, nb):
                    pdet.append("profile %s: %d processes (A) / %d (B), allowed +-%.1f" % (prof, na, nb, amb_allowed(na, nb)))
                elif abs(ea - eb) > amb_allowed(ea, eb):
                    pdet.append("profile %s: alive at the end A %d / B %d (stuck or gone)" % (prof, ea, eb))
                else:
                    # stuck (profile 01C1): a process alive at the end counts as stuck only
                    # when it has lived longer than the profile's longest finished lifetime in A plus proc_slack (a
                    # late-spawned short-lived process alive at the cut is not stuck); without a finished process in A,
                    # alive at the end in one run only counts (the old form)
                    done_a = [x[1] - x[0] for x in xa if x[1] < lastf - TOL["proc_slack"]]
                    if done_a:
                        lim = max(done_a) + TOL["proc_slack"]
                        old_a = sum(1 for x in xa if x[1] >= lastf - TOL["proc_slack"] and lastf - x[0] > lim)
                        old_b = sum(1 for x in xb if x[1] >= lastf - TOL["proc_slack"] and lastf - x[0] > lim)
                        if old_a != old_b:
                            pdet.append("profile %s: alive at the end longer than A's longest finished life %d (+%d): A %d / B %d (stuck or gone)"
                                        % (prof, max(done_a), TOL["proc_slack"], old_a, old_b))
                    elif (ea == 0 and eb > 0) or (eb == 0 and ea > 0):
                        pdet.append("profile %s: alive at the end A %d / B %d (stuck or gone)" % (prof, ea, eb))
                continue
            if len(xa) != len(xb):
                det.append("profile %s: %d processes (A) / %d (B)" % (prof, len(xa), len(xb)))
                continue
            for (a0, a1), (b0, b1) in zip(xa, xb):
                ends_inside = a1 < lastf - TOL["proc_slack"] or b1 < lastf - TOL["proc_slack"]
                if abs(a0 - b0) > TOL["proc_slack"] + crane_slack(a0, b0) or (ends_inside and abs(a1 - b1) > TOL["proc_slack"] + crane_slack(a1, b1)):
                    det.append("profile %s: lives frames %d..%d (A) / %d..%d (B)" % (prof, a0, a1, b0, b1))
                    break
        rep.gate("procs", not det, "%d / %d processes, %d profiles (converted: %s)" % (sum(map(len, pa.values())), sum(map(len, pb.values())), len(set(pa) | set(pb)),
                 ("all" if conv == "all" else ",".join(sorted(conv)) or "none") + ", from " + conv_src), det[:6])
        if rngf is not None or cut is not None:
            rep.gate("procs-rng", not pdet, "rule notes 8b / 8d: %d unconverted or --plaus-spec profile(s) living on after the first RNG difference (frame %s)%s, plausibility" % (
                n_pr, "-" if rngf is None else rngf, "" if cut is None else " or any profile after the strict cut (frame %d)" % cut), pdet[:6])
        else:
            rep.gate("procs-rng", None, "no RNG difference")
    else:
        rep.gate("procs", None, "no digest.txt")

    # save info
    def masked(b):
        b = bytearray(b)
        for o, n in save_mask:
            b[o:o + n] = bytes(len(b[o:o + n]))
        return bytes(b)
    names = sorted({os.path.basename(p) for p in glob.glob(os.path.join(A, "si.bin*"))} & {os.path.basename(p) for p in glob.glob(os.path.join(B, "si.bin*"))})

    # (8d addendum 3): after a strict cut the save stays strict except Link's current health
    # (dSv_player_status_a_c::mLife, si.bin +0x22, u16), which must stay within the 30 Hz run's range over the pair.
    # A block taken before the cut (si.bin.T, T s after the origin: frame 30 T < cut) stays strict, mLife too.
    def block_frame(n):
        t = n[len("si.bin."):] if n.startswith("si.bin.") else None
        try:
            return None if t is None else int(round(float(t) * 30))
        except ValueError:
            return None
    def life_exempt(n):
        bf = block_frame(n)
        return cut is not None and (bf is None or bf >= cut)
    names = [n for n in names if save_frame(n, end) is not None]
    if names:
        det = []
        blk = {r: {n: masked(open(os.path.join(r, n), "rb").read()) for n in names} for r in (A, B)}
        order = sorted(names, key=lambda n: save_frame(n, end))
        # the counter-edge rule (8c): a listed counter may differ by the number of its start / stop edges that lie within proc_slack of
        # a converted party's procedure change (Link, trace.txt) or state change (CONV_STATE actor dumps)
        conv_ch = set()
        for r, lr, fr in ((A, la, fa), (B, lb, fb)):
            conv_ch |= {f for _, f in runs_of({f: v["proc"] for f, v in lr.items()})[1:]}
            for fn, (off, fmt, _) in CONV_STATE.items():
                if os.path.exists(os.path.join(r, fn)):
                    conv_ch |= admp_state_changes(os.path.join(r, fn), fr, end, off, fmt)
        allch = sorted(conv_ch)
        # 8c fall sequence: Link's procedure changes that B took 1..proc_slack frames off A's
        # (same procedure sequence in both runs) -> (A frame, B frame, the next Link procedure change of either run)
        lch = {r: runs_of({f: v["proc"] for f, v in lr.items()})[1:] for r, lr in ((A, la), (B, lb))}
        shifted = []
        if lch[A] and [v for v, _ in lch[A]] == [v for v, _ in lch[B]]:
            lall = sorted({f for _, f in lch[A]} | {f for _, f in lch[B]})
            for (_, fa_), (_, fb_) in zip(lch[A], lch[B]):
                if 0 < abs(fb_ - fa_) <= TOL["proc_slack"]:
                    shifted.append((fa_, fb_, min([f for f in lall if f > max(fa_, fb_)], default=None)))
        ctr_note = []
        allowed = []  # (offset, size, fmt, [frame from which one count is allowed, per qualifying edge])
        for off, fmt, unit, what in SAVE_COUNTERS:
            sz = struct.calcsize(fmt)
            ser = {r: [(save_frame(n, end), struct.unpack_from(fmt, blk[r][n], off)[0]) for n in order if off + sz <= len(blk[r][n])] for r in (A, B)}
            ea, eb = counter_edges(ser[A]), counter_edges(ser[B])
            qual = []
            if [k for k, _, _ in ea] != [k for k, _, _ in eb]:
                ctr_note.append("+0x%X: start/stop sequences differ (A %s, B %s): no allowance" % (off, ea, eb))
            else:
                for ei, ((kind, a0, a1), (_, b0, b1)) in enumerate(zip(ea, eb)):
                    lo, hi = min(a0, b0) - TOL["proc_slack"], max(a1, b1) + TOL["proc_slack"]
                    mid = (lo + hi) / 2.0
                    near = sorted((c for c in allch if lo <= c <= hi), key=lambda c: abs(c - mid))
                    # the runs' edge windows meet, and each is resolved (dumps at most 0.1 s apart around it: a wider
                    # window cannot tie the edge to one change)
                    if not (max(a0, b0) - min(a1, b1) <= TOL["proc_slack"] and max(a1 - a0, b1 - b0) <= 6):
                        continue
                    if near:
                        qual.append((min(a0, b0), kind, str(near[0]), max(a1, b1)))
                        continue
                    # 8c fall sequence: the counter's first edge after a shifted Link change, no Link change between,
                    # within fall_seq frames, moved the same way as the change
                    for fa_, fb_, nxt in shifted:
                        c0 = min(fa_, fb_)
                        e = (b0 + b1) - (a0 + a1)
                        first = not any(lo_ >= c0 for _, lo_, _ in ea[:ei]) and not any(lo_ >= c0 for _, lo_, _ in eb[:ei])
                        if (min(a0, b0) >= c0 and (nxt is None or max(a0, b0) < nxt) and max(a1, b1) - max(fa_, fb_) <= TOL["fall_seq"]
                                and (e == 0 or (e > 0) == (fb_ > fa_)) and first):
                            qual.append((min(a0, b0), kind, "fall seq. from Link's change A %d / B %d" % (fa_, fb_), max(a1, b1)))
                            break
                if ea:
                    ctr_note.append("+0x%X: %d start/stop edge(s), %d resolved at a converted change %s" % (off, len(ea), len(qual),
                                    ", ".join("%s in %d..%d (change %s)" % (k, s, h, c) for s, k, c, h in qual)))
            allowed.append((off, sz, fmt, unit, [q[0] for q in qual]))
        for n in names:
            x, y = blk[A][n], blk[B][n]
            if life_exempt(n):
                x, y = x[:LIFE_OFF] + bytes(2) + x[LIFE_OFF + 2:], y[:LIFE_OFF] + bytes(2) + y[LIFE_OFF + 2:]
            F = save_frame(n, end)
            for off, sz, fmt, unit, starts in allowed:
                if off + sz > min(len(x), len(y)):
                    continue
                d = abs(struct.unpack_from(fmt, x, off)[0] - struct.unpack_from(fmt, y, off)[0])
                lim = sum(1 for s in starts if s <= F)
                if not (d == d) or d > lim * unit * (1.0 if isinstance(unit, int) else 1.01):
                    det.append("%s: counter +0x%X differs by %.6g (%.4g counts of %g; rule note 8c allows %d)" % (n, off, d, d / unit, unit, lim))
                x, y = x[:off] + bytes(sz) + x[off + sz:], y[:off] + bytes(sz) + y[off + sz:]
            if x != y:
                d = [j for j in range(min(len(x), len(y))) if x[j] != y[j]]
                det.append("%s: %d bytes differ: %s" % (n, len(d) + abs(len(x) - len(y)), ["0x%X" % j for j in d[:12]]))
        shown = ", ".join(order) if len(order) <= 6 else "%s .. %s" % (", ".join(order[:2]), order[-1])
        rep.gate("save", not det, "%d block(s): %s; rule note 8c counters: %s%s" % (len(names), shown, "; ".join(ctr_note) or "no edges",
                 "" if cut is None else " (Link's mLife +0x%X after the strict cut: gate save-life)" % LIFE_OFF), det[:12])
    else:
        rep.gate("save", None, "no si.bin in both runs")

    # save-life: Link's health after the cut. Per-frame values from LIFE_DUMP (WWHD_MEM_DUMP of the save info's first
    # 4 bytes, mMaxLife / mLife; checked against si.bin's mMaxLife and last mLife) where both runs have it, plus every
    # si.bin block. Before the cut the per-frame values are strict (equal); from the cut B's values (per frame and in the
    # blocks after the cut) stay within [min, max] of A's values over the whole pair.
    if cut is not None:
        def u16(b, o):  # (blocks: mMaxLife at LIFE_OFF - 2, mLife at LIFE_OFF; life.admp records: at 0 and 2)
            return struct.unpack_from(">H", b, o)[0] if len(b) >= o + 2 else None
        def life_series(run, fr):
            path = os.path.join(run, LIFE_DUMP)
            if not os.path.exists(path):
                return None
            out = {}
            data = open(path, "rb").read()
            o = 0
            while o + 28 <= len(data) and data[o:o + 4] == b"ADMP":
                step, full, dt, proc, size = struct.unpack_from("<QIfII", data, o + 4)
                body = data[o + 28:o + 28 + size]
                o += 28 + size
                f = fr.frame(step)
                if f is None or (end is not None and f > end) or len(body) < 4:
                    continue
                out[f] = (u16(body, 0), u16(body, 2))
            return out
        det = []
        sa_, sb_ = life_series(A, fa), life_series(B, fb)
        blocks = {}
        for run, tag in ((A, "A"), (B, "B")):
            for n in names:
                b = open(os.path.join(run, n), "rb").read()
                blocks[(tag, n)] = (u16(b, LIFE_OFF - 2), u16(b, LIFE_OFF))
        for ser, tag in ((sa_, "A"), (sb_, "B")):
            if ser and ("si.bin" in names):
                mx = blocks[(tag, "si.bin")][0]
                if any(v[0] != mx for v in ser.values()):
                    det.append("%s %s: mMaxLife %s does not match si.bin's %s (wrong address?)" % (tag, LIFE_DUMP, sorted({v[0] for v in ser.values()})[:4], mx))
        if sa_ is not None and sb_ is not None:
            pre = [f for f in sorted(set(sa_) & set(sb_)) if f < cut and sa_[f][1] != sb_[f][1]]
            if pre:
                det.append("health differs before the cut (frame %d: A %d, B %d)" % (pre[0], sa_[pre[0]][1], sb_[pre[0]][1]))
        ra = [v[1] for v in (sa_ or {}).values()] + [blocks[("A", n)][1] for n in names if blocks[("A", n)][1] is not None]
        if not ra:
            rep.gate("save-life", None, "no health values (no %s, no si.bin)" % LIFE_DUMP)
        else:
            lo, hi = min(ra), max(ra)
            rb = [(f, v[1]) for f, v in sorted((sb_ or {}).items()) if f >= cut]
            rb += [(n, blocks[("B", n)][1]) for n in names if life_exempt(n) and blocks[("B", n)][1] is not None]
            out = [(w, v) for w, v in rb if not lo <= v <= hi]
            if out:
                det.append("B's health outside A's range %d..%d: %s" % (lo, hi, ", ".join("%s %d" % (w if isinstance(w, str) else "frame %d" % w, v) for w, v in out[:4])))
            rep.gate("save-life", not det, "rule note 8d addendum 3: B's health after the cut within A's range %d..%d (%d B values; per-frame %s)" % (
                lo, hi, len(rb), "A + B" if sa_ is not None and sb_ is not None else "none (blocks only)"), det[:6])
    else:
        rep.gate("save-life", None, "no strict cut (health stays in the strict save gate)")

    # actor dumps
    spec = []
    tevstr_offs = {0x110}  # the actor's own tevStr (fopAc_ac_c::tevStr); more from `tevstr OFF` spec lines / --admp-tevstr
    tevstr_by_dump = collections.defaultdict(set)
    for x in admp_tevstr:
        nm, _, off = x.rpartition(":")
        tevstr_by_dump[nm].add(int(off, 16))

    def admp_is_actor(xa, keys):
        """: an actor dump's records are processes: base_process_class's tags (fpcBs_Create) point back at the process,
        so the record's process address (the header's process field) is the word at +0x24 and +0x40 of the body. A
        WWHD_MEM_DUMP record carries the region's address there instead and its body has no such pointers."""
        for k in keys[:8]:
            b = xa[k]
            if len(b) < 0x44 or struct.unpack_from(">I", b, 0x24)[0] != k[1] or struct.unpack_from(">I", b, 0x40)[0] != k[1]:
                return False
        return bool(keys)
    explicit_tev = collections.defaultdict(set)  # NAME -> tevStr offsets its per-dump spec lists itself (`tevstr OFF`)

    SPEC_REL = {}  # (id(spec list), OFF) -> REL of a `OFF f32 TOL rel=REL` line

    def parse_spec(path, spec, tevs):
        for l in open(path):
            l = l.split("#")[0].split()
            if not l:
                continue
            sp6c = None
            if len(l) == 2 and l[0] == "tevstr":
                tevs.add(int(l[1], 16))
                continue
            if len(l) == 4 and l[1] == "f32" and l[3].startswith("rel="):
                # (esa): a relative tolerance: |B - A| <= max(TOL, REL x max(|A|, |B|)); TOL is
                # the absolute floor (split-add rounding of per-step position adds grows with the coordinate's size)
                SPEC_REL[(id(spec), int(l[0], 16))] = float(l[3][4:])
                l = l[:3]
            if len(l) == 4 and l[1] == "f32" and l[3].startswith("6c="):
                sp6c = int(l[3][3:], 16)
            elif len(l) != 3 or l[1] not in ("f32", "s16", "a16", "s32", "u32", "u16", "u8", "s8"):
                die("compare60: --admp-spec line `%s`: OFF TYPE TOL [6c=SPOFF | rel=R] (TYPE f32 s16 a16 s32 u32 u16 u8 s8; 6c= / rel= on f32 only)" % " ".join(l))
            spec.append((int(l[0], 16), l[1], float(l[2]), sp6c))
    if admp_spec:
        parse_spec(admp_spec, spec, tevstr_offs)
    per_dump = {}  # NAME -> (spec, tevStr offsets) of --admp-spec NAME=FILE
    for nm, fp in ADMP_SPECS.items():
        sp_, tv_ = [], set(tevstr_offs)
        parse_spec(fp, sp_, tv_)
        own_, tmp_ = set(), []
        parse_spec(fp, tmp_, own_)
        for k_ in [k_ for k_ in SPEC_REL if k_[0] == id(tmp_)]:
            del SPEC_REL[k_]  # (the throw-away list's id may be reused)
        explicit_tev[nm] = own_
        per_dump[nm] = (sp_, tv_)
    spec_all, tevstr_all = spec, tevstr_offs

    def admp(path, fr, steps=None):
        out = {}
        data = open(path, "rb").read()
        o = 0
        while o + 28 <= len(data) and data[o:o + 4] == b"ADMP":
            step, full, dt, proc, size = struct.unpack_from("<QIfII", data, o + 4)
            body = data[o + 28:o + 28 + size]
            o += 28 + size
            f = fr.frame(step)
            if f is None or (end is not None and f > end):
                continue
            out[(f, proc)] = body  # the last record per frame: the full step
            if steps is not None:
                steps.setdefault((f, proc), []).append((dt, body))  # every step record in order
        return out

    def draw_written(xa, xb, bsteps, keys):
        """the words the draw writes (2026-11-17, t60-resume tools/drawshift.py): a dump record is taken between the
        logic and the draw, so A's frame record holds the previous frame's draw and B's the draw of its frame's earlier
        step; a word is draw-written when on EVERY frame where A's and B's frame records differ in it, B's first-step
        record of that frame equals A's (the draw's one-pass shift: anm entry frames, tevStr, blended copies). Returns
        {offset: frames differing} (B needs > 1 step record per frame)"""
        cand, bad = collections.Counter(), set()
        for key in keys:
            a, bt = xa[key], xb[key]
            rb = bsteps.get(key, [])
            bs = rb[0][1] if len(rb) > 1 else None
            for off in range(0, min(len(a), len(bt)) - 3, 4):
                if a[off:off + 4] == bt[off:off + 4]:
                    continue
                if bs is not None and len(bs) >= off + 4 and bs[off:off + 4] == a[off:off + 4]:
                    cand[off] += 1
                else:
                    bad.add(off)
        return {o: n for o, n in cand.items() if o not in bad}
    def camrows(run, fr_):
        return {f: v[0] for f, v in state_channel(run, "cam.txt", fr_, 1, lambda p: 0, lambda p: [fnum(x) for x in p[5:11]], end, ncol=12).items()}
    lcam = {}

    def light_gate(n, toffs, xa, xb, keys):
        """admp-light: each tevStr's light position (view space, +0) turned back to world space with the picture camera of
        the record's draw (A: frame f-1; B: halfway between f-1 and f-2), B against A within TOL light"""
        if not lcam:
            lcam["A"], lcam["B"] = camrows(A, fa), camrows(B, fb)
            lcam["shake"] = cam_shakes(A, fa, end) | cam_shakes(B, fb, end)
        ca, cb, shk = lcam["A"], lcam["B"], lcam["shake"]
        worst, nf, det, nshk = (0.0, None, None), 0, [], set()
        nworld = [0]
        for t in toffs:
            for key in keys:
                f = key[0]
                if f - 1 not in ca or f - 1 not in cb or f - 2 not in cb:
                    continue
                if shk & {f, f - 1, f - 2}:
                    nshk.add(f)  # a shaking camera: the drawn view is not the cam.txt eye / centre
                    continue
                va = struct.unpack_from(">fff", xa[key], t)
                vb = struct.unpack_from(">fff", xb[key], t)
                if not all(map(math.isfinite, va + vb)) or va == (0.0, 0.0, 0.0) or vb == (0.0, 0.0, 0.0):
                    continue  # not set yet (settingTevStruct not run on this tevStr)
                c1, c2 = cb[f - 1], cb[f - 2]
                cm = [(p + q) / 2.0 for p, q in zip(c1, c2)]
                # the tevStr's own world-space light (dKy_tevstr_c::mLightPosWorld, +0x84; settingTevStruct writes it with
                # the view-space one): compared directly, no reconstructed picture camera (bow pairs: the blend of B's
                # drawn camera is ~2.6 deg off during a 15 deg/frame turn, x the light distance ~160000 = 0.045)
                if len(xa[key]) >= t + 0x90 and len(xb[key]) >= t + 0x90:
                    pa = struct.unpack_from(">fff", xa[key], t + 0x84)
                    pb = struct.unpack_from(">fff", xb[key], t + 0x84)
                    if all(map(math.isfinite, pa + pb)) and pa != (0.0, 0.0, 0.0) and pb != (0.0, 0.0, 0.0):
                        r = math.dist(pa, ca[f - 1][0:3])
                        d = math.dist(pa, pb) / r if r > 0 else 0.0
                        nf += 1
                        nworld[0] += 1
                        if d > worst[0]:
                            worst = (d, f, t)
                        if d > TOL["light"] and len(det) < 4:
                            det.append("tevStr +0x%X frame %d: world light (mLightPosWorld) A (%.1f, %.1f, %.1f) B (%.1f, %.1f, %.1f), |d| %.3g of the distance" % (t, f, *pa, *pb, d))
                        continue
                wa = view_to_world(va, ca[f - 1][0:3], ca[f - 1][3:6])
                # B's picture camera: the interpolated one (halfway f-1 / f-2: bdk, a converted actor lit in its draw on
                # the tick30 pass after the camera's draw) or its f-1 logic camera (bdkobj, converted or not: exact,
                # |d| < 1); the closer of the two
                wbs = [w for w in (view_to_world(vb, cm[0:3], cm[3:6]), view_to_world(vb, c1[0:3], c1[3:6])) if w is not None]
                if wa is None or not wbs:
                    continue
                wb = min(wbs, key=lambda w: math.dist(wa, w))
                # relative to the light's distance from the eye: the picture camera is a blend (interp's own eye /
                # centre / angle blending), so the turned-back point carries the blend's angle error times that
                # distance (bdkll1: 0.5 % at most while the camera turns, light ~45000 away)
                r = math.dist(wa, ca[f - 1][0:3])
                d = math.dist(wa, wb) / r if r > 0 else 0.0
                nf += 1
                if d > worst[0]:
                    worst = (d, f, t)
                if d > TOL["light"] and len(det) < 4:
                    det.append("tevStr +0x%X frame %d: world A (%.1f, %.1f, %.1f) B (%.1f, %.1f, %.1f), |d| %.3g of the distance" % (t, f, *wa, *wb, d))
        if not nf:
            rep.gate("admp-light", None, "%s: no frame with a set tevStr light and the cameras around it" % n)
            return
        rep.gate("admp-light", worst[0] <= TOL["light"], "%s: tevStr light (picture state) in world space (%d of %d tevStr-frames from mLightPosWorld directly, the rest through the picture cameras A f-1, B halfway f-1 / f-2 or f-1); max |B - A| / distance from the eye %.3g%s (tol %g); %d frame(s) with a camera shake left out" % (
            n, nworld[0], nf, worst[0], " at frame %d (tevStr +0x%X)" % (worst[1], worst[2]) if worst[1] is not None else "", TOL["light"], len(nshk)), det)

    dumps = sorted({os.path.basename(p) for p in glob.glob(os.path.join(A, "*.admp"))} & {os.path.basename(p) for p in glob.glob(os.path.join(B, "*.admp"))} - {LIFE_DUMP})
    for n in dumps:
        spec, tevstr_offs = per_dump.get(n[:-5], (spec_all, tevstr_all))  # a per-dump spec first, else the plain one (or none)
        req = "all" in REQUIRE_SPEC or n in REQUIRE_SPEC or n[:-5] in REQUIRE_SPEC
        if req and per_dump and n[:-5] not in per_dump and spec:
            # the per-dump form is in use: a required subject needs its OWN spec, not the plain one (arrow judged with Link's)
            rep.gate("admp-spec", False, "%s: --admp-require-spec: no own --admp-spec %s=FILE (the plain spec %s is another dump's)" % (
                n, n[:-5], admp_spec))
        bsteps = {}
        xa, xb = admp(os.path.join(A, n), fa), admp(os.path.join(B, n), fb, bsteps)
        keys = sorted(k for k in set(xa) & set(xb) if cut is None or k[0] < cut)
        dw = draw_written(xa, xb, bsteps, keys)
        toffs = sorted(tevstr_offs | tevstr_by_dump.get(n, set()))
        rsize = min((min(len(xa[k]), len(xb[k])) for k in keys), default=0)
        toffs = [t for t in toffs if t + 0x1C8 <= rsize]
        if not admp_is_actor(xa, keys):
            # (sea 0x26 seapk, 2026-11-21): a WWHD_MEM_DUMP region is no process: no fopAc tevStr at +0x110 (its
            # words are the region's own logic words), so no tevStr mask and no admp-light; `tevstr OFF` lines still apply
            toffs = [t for t in toffs if t != 0x110 or 0x110 in tevstr_by_dump.get(n, set()) or 0x110 in explicit_tev.get(n[:-5], set())]
        picw = tev_picture_words(toffs, rsize)
        if toffs and keys and n not in {k.split("#")[0] for k in CLOTH}:  # a cloth dump (--cloth) is no actor: no tevStr
            light_gate(n, toffs, xa, xb, keys)
        dwnote = "; draw-written words masked (B's first-step record = A's on every differing frame): %d%s" % (
            len(dw), (" (" + ", ".join("+0x%X:%d" % kv for kv in sorted(dw.items())[:12]) + (" ..." if len(dw) > 12 else "") + ")") if dw else "")
        fmt = {"f32": ">f", "s16": ">h", "a16": ">h", "s32": ">i", "u32": ">I", "u16": ">H", "u8": ">B", "s8": ">b"}
        # the speed-ramp position-gap rule (6c), enemy form: per process and speed word, the term at each of B's frames
        term6c = {}
        for sp6 in {w[3] for w in spec if w[3] is not None}:
            term6c[sp6] = drift6c_terms(bsteps, sp6)
        det, worst, worst6 = [], {}, {}
        for key in keys:
            ba, bb = xa[key], xb[key]
            for off, ty, tol, sp6 in spec:
                sz = struct.calcsize(fmt[ty])
                if any(word_overlaps(off, sz, q) for q in picw):
                    continue  # tevStr light: picture state (named below; checked in world space by admp-light)
                if off + sz > min(len(ba), len(bb)):
                    continue
                u, v = struct.unpack_from(fmt[ty], ba, off)[0], struct.unpack_from(fmt[ty], bb, off)[0]
                d = 0 if (u == v or (isinstance(u, float) and math.isnan(u) and math.isnan(v))) else angdiff(u, v) if ty == "a16" else abs(u - v)  # equal (inf == inf) is 0
                t6 = term6c[sp6].get(key, 0.0) if sp6 is not None else 0.0
                rel = SPEC_REL.get((id(spec), off))
                tl = max(tol, rel * max(abs(u), abs(v))) if rel is not None and math.isfinite(u) and math.isfinite(v) else tol
                if not (d <= worst.get(off, (-1,))[0]):
                    worst[off] = (d, key[0])
                    worst6[off] = t6
                if not (d <= tl + t6) and len(det) < 4:
                    det.append("+0x%X %s at frame %d: %r / %r (tol %g%s%s)" % (off, ty, key[0], u, v, tl, " + 6c %.4g" % t6 if sp6 is not None else "",
                                                                               " = rel %g, floor %g" % (rel, tol) if rel is not None else ""))
        if spec and n not in {k.split("#")[0] for k in CLOTH}:
            # spec words are logic state by the spec's choice: always checked; one that looks draw-written is named
            sdw = [o for o, ty, *_ in spec if any(word_overlaps(o, struct.calcsize(fmt[ty]), q) for q in dw)]
            spic = [o for o, ty, *_ in spec if any(word_overlaps(o, struct.calcsize(fmt[ty]), q) for q in picw)]
            if spic:
                det.append("spec word(s) in a tevStr's view-space light (picture state, not compared raw; admp-light checks them): %s" % ", ".join("+0x%X" % o for o in spic))
            if n[:-5] in ADMP_EXACT:  # the spec's words with their tolerances, every other logic word exact
                sw = set()
                for o, ty, _tol, _sp in spec:
                    sz = struct.calcsize(fmt[ty])
                    sw |= {q for q in range(o - o % 4, o + sz, 4)}
                ecnt = collections.Counter()
                for key in keys:
                    ba, bb = xa[key], xb[key]
                    for off in range(0, min(len(ba), len(bb)) - 3, 4):
                        if ba[off:off + 4] != bb[off:off + 4] and off not in dw and off not in picw and off not in sw \
                                and not any(t <= off < t + 0x1C8 for t in toffs):
                            ecnt[off] += 1
                if ecnt:
                    det.append("--admp-exact: words outside the spec differing: " + ", ".join("+0x%X:%d" % kv for kv in ecnt.most_common(12)))
            rep.gate("admp", not det, "%s: %d records; worst %s%s" % (n, len(keys), ", ".join(
                "+0x%X %.4g@%d%s" % (o, w[0], w[1], " (6c term %.4g)" % worst6[o] if any(x[0] == o and x[3] is not None for x in spec) else "")
                for o, w in sorted(worst.items())), ("; spec word(s) that look draw-written: " + ", ".join("+0x%X" % o for o in sdw)) if sdw else ""),
                det + ([dwnote.lstrip("; ")] if dw else []))
        else:  # report: the words that differ in most records
            cnt = collections.Counter()
            for key in keys:
                ba, bb = xa[key], xb[key]
                for off in range(0, min(len(ba), len(bb)) - 3, 4):
                    if ba[off:off + 4] != bb[off:off + 4] and off not in dw and off not in picw:
                        cnt[off] += 1
            if n[:-5] in ADMP_EXACT:
                # the whole dKy_tevstr_c (0x1C8) of each tevStr is the draw's light state here (settingTevStruct writes it in
                # the draw; its world light is judged by admp-light), not only the view-space words the report masks
                cnt = collections.Counter({o: k for o, k in cnt.items() if not any(t <= o < t + 0x1C8 for t in toffs)})
                rep.gate("admp", not cnt, "%s: %d records, --admp-exact: logic words differing: %s%s%s" % (
                    n, len(keys), ", ".join("+0x%X:%d" % kv for kv in cnt.most_common(12)) or "none", dwnote,
                    "; tevStr view-space light words masked (picture state): tevStr %s" % ", ".join("+0x%X" % t for t in toffs) if toffs else ""))
                continue
            if "all" in REQUIRE_SPEC or n in REQUIRE_SPEC or n[:-5] in REQUIRE_SPEC:
                rep.gate("admp-spec", False, "%s: --admp-require-spec: the subject dump has no --admp-spec (its words are not judged; "
                         "%d logic word(s) differ)" % (n, len(cnt)))
            rep.gate("admp", None, "%s: %d records, no spec (report); logic words differing most: %s%s%s" % (
                n, len(keys), ", ".join("+0x%X:%d" % kv for kv in cnt.most_common(10)) or "none", dwnote,
                "; tevStr view-space light words masked (picture state): tevStr %s" % ", ".join("+0x%X" % t for t in toffs) if toffs else ""))

    # the strict RNG-pair rule (8d): plausibility of the --plaus-spec actors from the first RNG difference on
    for sp in plaus_specs:
        pa_ = os.path.join(A, sp["dump"])
        pb_ = os.path.join(B, sp["dump"])
        if not (os.path.exists(pa_) and os.path.exists(pb_)):
            rep.gate("actor-plaus", False, "%s missing in %s" % (sp["dump"], "A and B" if not os.path.exists(pa_) and not os.path.exists(pb_) else "A" if not os.path.exists(pa_) else "B"))
        elif rngf is None and cut is None:
            rep.gate("actor-plaus", None, "%s: no RNG difference (the strict gates cover it)" % sp["dump"])
        else:
            ok, msg, det = plaus_check(sp, admp(pa_, fa), admp(pb_, fb), min(x for x in (rngf, cut) if x is not None))
            rep.gate("actor-plaus", ok, "rule note 8d: " + msg, det[:6])
    # cloth (the cloth-simulation rule (6k)): the visual cloth's vertex arrays, statistically after the conversion
    conv_f = convert_frame(B)
    for n, (nv, q0, q1, qs, _what) in sorted(CLOTH.items()):
        fn_ = n.split("#")[0]  # FILE#TAG: several cloths in one dump (the ghost ship's two sails, one MEM_DUMP region)
        if not (os.path.isfile(os.path.join(A, fn_)) and os.path.isfile(os.path.join(B, fn_))):
            continue
        ok, msg, det = cloth_gate(cloth_frames(os.path.join(A, fn_), fa, end, nv, q0, q1, qs),
                                  cloth_frames(os.path.join(B, fn_), fb, end, nv, q0, q1, qs), conv_f, nv)
        rep.gate("cloth", ok, "%s (rule note 6k): %s" % (n, msg), det)

    # ship (the ship-drift rule (6h)): the King of Red Lions' own trajectory from its actor dump (--ship NAME, default ship.admp)
    sa_ = ship_rows(os.path.join(A, ship_name), fa, end)
    sb_ = ship_rows(os.path.join(B, ship_name), fb, end)
    sf_ = sorted(set(sa_) & set(sb_))
    if sf_:
        dtb = min(sb_[f]["dt"] for f in sf_)
        sramp = TOL["ship_ramp"] if TOL["ship_ramp"] >= 0 else (1.0 - dtb) / 2.0
        drift = ship_turn_tol(sa_, sb_, sramp, TOL["ship_turn_k"])
        # the early ship-decision rule (6i): decision shifts B made earlier than A (speed plausibility only near them, xz term after them)
        shifts, splaus, sterm, sfac = ship_shift_tol(sa_, sb_, dtb)
        sfol = ship_followon(sa_, sb_, shifts)
        bwin, bterm, bxz = ship_branch_tol(sa_, sb_, sf_, dtb)
        # the early ship-decision rule (6i(iii)): position decisions (DEMO_MOVE arrival) k frames apart, k from the xz drift / |v_A|
        pshifts, pplaus, pbad = ship_pos_shifts(sa_, sb_, shifts)
        splaus = splaus | pplaus
        det, w = [], {"xz": (0.0, None, 0.0), "y": (0.0, None), "speed": (0.0, None), "angle": (0, None)}
        tight = (None, -1e9)
        nbad = 0
        # the ship's mode sequence: the same changes, each within proc_slack
        ma = [(m, f) for m, f in runs_of({f: sa_[f]["mode"] for f in sf_})]
        mb = [(m, f) for m, f in runs_of({f: sb_[f]["mode"] for f in sf_})]
        mode_ok = len(ma) == len(mb) and all(x[0] == y[0] and abs(x[1] - y[1]) <= TOL["proc_slack"] + crane_slack(x[1], y[1]) for x, y in zip(ma, mb))
        if not mode_ok:
            nbad += 1
            det.append("modes: A %s / B %s (slack %d)" % (ma[:8], mb[:8], TOL["proc_slack"]))
        # ship y (open: report + plausibility): B's y and mWaterY finite, B's mWaterY inside A's
        # sea band (A's mWaterY range over the route +- pos) and B's float offset y - mWaterY inside A's range +- pos (A's
        # frames on the water). In flight (daSFLG_FLY_e, question 2) each run's
        # height is taken relative to its own take-off (y at the frame before the flight): B's y - y_takeoff,B inside A's
        # range of y - y_takeoff,A over the same (n-th) flight +- pos; the absolute float gap stays under rule (c)
        def flights(rows):
            out, cur = [], None
            for f in sf_:
                if rows[f]["fly"]:
                    if cur is None:
                        y0 = rows[f - 1]["pos"][1] if f - 1 in rows else rows[f]["pos"][1]
                        cur = (y0, [])
                        out.append(cur)
                    cur[1].append(f)
                else:
                    cur = None
            return out
        fla, flb = flights(sa_), flights(sb_)
        frel = {}  # B flight frame -> (B's height over its take-off, A's range over the paired flight)
        for (ya0, fa_), (yb0, fb_) in zip(fla, flb):
            ra = [sa_[f]["pos"][1] - ya0 for f in fa_ if math.isfinite(sa_[f]["pos"][1] - ya0)]
            for f in fb_:
                frel[f] = (sb_[f]["pos"][1] - yb0, (min(ra), max(ra)) if ra else None)
        wa = [sa_[f]["waterY"] for f in sf_ if math.isfinite(sa_[f]["waterY"])]
        oa = [sa_[f]["pos"][1] - sa_[f]["waterY"] for f in sf_ if not sa_[f]["fly"] and math.isfinite(sa_[f]["pos"][1] - sa_[f]["waterY"])]
        ybad = []
        # rule (c) addendum (pair): the sea's height at the ship depends on
        # where the ship is. While a 6i window or its carried xz term (sterm, bxz) allows B's ship to be elsewhere and it
        # is (|xz B - A| > pos), B's absolute mWaterY is not compared with A's band; the float-offset check (y - mWaterY
        # inside A's range +- pos) stays and proves the ship floats correctly. Elsewhere the band applies.
        ysea_free = set(f for f in sf_ if sterm.get(f, 0.0) + bxz.get(f, 0.0) > 0.0
                        and math.hypot(sa_[f]["pos"][0] - sb_[f]["pos"][0], sa_[f]["pos"][2] - sb_[f]["pos"][2]) > TOL["pos"])
        # derived slope bound (replaces the addendum's skip; `--tol sea_slope=0`: skip
        # as before): on those frames |mWaterY_B - mWaterY_A| <= s_A x |xz B - A| + pos, s_A = the sea's local height
        # change per unit of distance measured on A's own trace (sea_slopes)
        yslope = sea_slopes(sa_, sf_, set(f for f in sf_ if sterm.get(f, 0.0) + bxz.get(f, 0.0) > 0.0)) if TOL["sea_slope"] > 0 else {}
        yslope_w = (0.0, None, 0.0)  # (largest |dw| - bound, frame, s_A there)
        if len(fla) != len(flb):
            ybad.append((sf_[0], "flights: A %d / B %d" % (len(fla), len(flb))))
        for f in sf_:
            b = sb_[f]
            yb, wb = b["pos"][1], b["waterY"]
            if not (math.isfinite(yb) and math.isfinite(wb)):
                ybad.append((f, "not finite"))
            elif wa and f not in ysea_free and not min(wa) - TOL["pos"] <= wb <= max(wa) + TOL["pos"]:
                ybad.append((f, "mWaterY %.4g outside A's band %.4g..%.4g" % (wb, min(wa), max(wa))))
            elif f in ysea_free and f in yslope and not abs(wb - sa_[f]["waterY"]) <= yslope[f] * math.hypot(
                    sa_[f]["pos"][0] - b["pos"][0], sa_[f]["pos"][2] - b["pos"][2]) + TOL["pos"]:
                dxz_ = math.hypot(sa_[f]["pos"][0] - b["pos"][0], sa_[f]["pos"][2] - b["pos"][2])
                ybad.append((f, "mWaterY %.4g vs A's %.4g: |d| %.4g over the slope bound %.4g = s_A %.4g x |xz B - A| %.4g + pos" % (
                    wb, sa_[f]["waterY"], abs(wb - sa_[f]["waterY"]), yslope[f] * dxz_ + TOL["pos"], yslope[f], dxz_)))
            elif b["fly"]:
                rel, rng = frel.get(f, (None, None))
                if rng is None:
                    if len(fla) == len(flb):
                        ybad.append((f, "flight frame without A's flight range"))
                elif not rng[0] - TOL["pos"] <= rel <= rng[1] + TOL["pos"]:
                    ybad.append((f, "height over take-off %.4g outside A's range %.4g..%.4g" % (rel, rng[0], rng[1])))
            elif oa and not min(oa) - TOL["pos"] <= yb - wb <= max(oa) + TOL["pos"]:
                ybad.append((f, "float offset %.4g outside A's range %.4g..%.4g" % (yb - wb, min(oa), max(oa))))
        for f in sorted(ysea_free & set(yslope)):
            dxz_ = math.hypot(sa_[f]["pos"][0] - sb_[f]["pos"][0], sa_[f]["pos"][2] - sb_[f]["pos"][2])
            m_ = abs(sb_[f]["waterY"] - sa_[f]["waterY"]) - (yslope[f] * dxz_ + TOL["pos"])
            if math.isfinite(m_) and (yslope_w[1] is None or m_ > yslope_w[0]):
                yslope_w = (m_, f, yslope[f])
        if pbad:
            nbad += len(pbad)
            det.extend("frame %d: %s" % x for x in pbad[:2])
        if ybad:
            nbad += len(ybad)
            det.extend("frame %d: y implausible (%s)" % x for x in ybad[:2])
        for f in sf_:
            a, b = sa_[f], sb_[f]
            dxz = math.hypot(a["pos"][0] - b["pos"][0], a["pos"][2] - b["pos"][2])
            tol = TOL["pos"] + drift.get(f, 0.0) + sterm.get(f, 0.0) + bxz.get(f, 0.0)
            ds = max(abs(a["speedF"] - b["speedF"]), abs(a["fwd"] - b["fwd"]))
            stol = TOL["speed"]
            if f in splaus:  # the early ship-decision rule (6i): plausibility only (finite)
                ds = 0.0 if all(math.isfinite(v) for v in (a["speedF"], b["speedF"], a["fwd"], b["fwd"])) else float("inf")
            elif f in sterm and TOL["ship_shift_dv"] > 0 and f - 1 in sa_:
                # the early ship-decision rule (6i)'s speed form (derived, flagged for the 2026-11-16): while the shift term runs, B's speed
                # leads A's ramp by the same up-to-a-frame: + (1 - dt_B) x |dv_A(f)| (pp4 frames 56-60: 0.5 x 1 = 0.5)
                stol += TOL["ship_shift_dv"] * sfac.get(f, 1.0 - dtb) * max(abs(a["speedF"] - sa_[f - 1]["speedF"]), abs(a["fwd"] - sa_[f - 1]["fwd"]))
            if f in bwin:  # the early ship-decision rule (6i) addendum: the sail's branch change inside a frame
                stol += bterm
            da = max(angdiff(a["move"], b["move"]), angdiff(a["facing"], b["facing"]))
            dy = abs(a["pos"][1] - b["pos"][1])
            if dxz > w["xz"][0]:
                w["xz"] = (dxz, f, tol)
            if dy > w["y"][0]:
                w["y"] = (dy, f)
            if ds > w["speed"][0]:
                w["speed"] = (ds, f)
            if da > w["angle"][0]:
                w["angle"] = (da, f)
            if dxz - tol > tight[1]:
                tight = (f, dxz - tol)
            bad = [n for n, d, t in (("xz", dxz, tol), ("speed", ds, stol), ("angle", da, TOL["angle"])) if not d <= t]
            if in_crane(f):  # rule: plausibility only (finite)
                bad = [n for n, d in (("xz", dxz), ("speed", ds), ("angle", da)) if not math.isfinite(d)]
            if bad:
                nbad += 1
                if len(det) < 4:
                    det.append("frame %d: %s (xz %.4g tol %.4g, speed %.4g, angle %d)" % (f, "/".join(bad), dxz, tol, ds, da))
        rep.gate("ship", nbad == 0,
                 "%s: %d frames (%d over); max |d| xz %.4g (frame %s, tol %.4g = %g + 6h drift + 6i term), speed %.4g (frame %s, tol %g + 6i), angle %d (frame %s, tol %d); "
                 "tightest xz frame %s (|d| - tol %.4g); y %.4g at frame %s (report; plausibility: %d frame(s) off, A float offset %s, %d flight(s) relative to take-off, sea band replaced by the slope bound on %d 6i-displaced frame(s)%s); 6h %s, factor %g; "
                 "%d mode change(s); 6i decision shifts (B earlier) %s, xz term max %.4g" % (
                     ship_name, len(sf_), nbad, w["xz"][0], w["xz"][1], w["xz"][2], TOL["pos"], w["speed"][0], w["speed"][1], TOL["speed"],
                     w["angle"][0], w["angle"][1], TOL["angle"], tight[0], tight[1], w["y"][0], w["y"][1], len(ybad),
                     "%.4g..%.4g" % (min(oa), max(oa)) if oa else "-", len(flb), len(ysea_free),
                     (", s_A max %.4g, tightest frame %s (|dw| - bound %.4g)" % (max(yslope.values()), yslope_w[1], yslope_w[0]) if yslope and yslope_w[1] is not None
                      else "" if TOL["sea_slope"] > 0 else ", slope bound off: band skipped"),
                     "hold form (signed sums, heading term x %g)" % TOL["ship_turn_k"], sramp, len(ma) - 1,
                     ", ".join("%s B %d / A %d" % (k[0] if k[0] == "move" and not k[1] else k[0] if k[0] == "mode" else "start", fb, fa)
                               + ("" if fb != fa else " (step shift %.3g)" % ks) for fb, fa, k, ks, dr in shifts if dr == "earlier") or "none", max(sterm.values() or [0.0]))
                 + "; 6i addendum branch changes (sail: wind power 0 inside a frame) %s" % (", ".join("frames %d-%d" % w_ for w_ in bwin_runs(bwin)) or "none")
                 + "; 6i(ii) shifts (B later) %s" % (", ".join("%s B %d / A %d (shift %.3g)" % (k[0] if k[0] == "mode" else "move", fb, fa, ks)
                                                             for fb, fa, k, ks, dr in shifts if dr == "later") or "none")
                 + "; 6i(iii) position decisions %s" % (", ".join("%s B %d / A %d (k %d <= ceil(|xz| %.4g / |v_A| %.4g) = %d; rest frame %d |xz| %.4g)" % (
                     "start" if k[1] else "stop", fb, fa, abs(fb - fa), dxz_, va_, km, rf_, rd_) for fb, fa, k, km, dxz_, va_, rf_, rd_ in pshifts) or "none")
                 + "".join("; 6i(ii) follow-on: rope B - A %+d at frame %d, %g segment(s) a frame in mode %d (A measured %s) -> rope-timed ship sounds may trail by %d frame(s) up to frame %d"
                           % (w["dR"], w["at"], w["rate"], w["mode"], w["rate_a"], w["D"], w["end"]) for w in sfol), det)

    # crane: the crane / salvage window's plausibility summary + stuck check
    if CRANE:
        def freeze(rows, key):
            # frames from the channel's last change inside the window to the window's end (the run's last frame)
            fs = [f for f in sorted(rows) if in_crane(f)]
            lastc = next((f for f, g in zip(reversed(fs[1:]), reversed(fs[:-1])) if rows[f][key] != rows[g][key]), fs[0] if fs else 0)
            return (fs[-1] - lastc) if fs else 0
        sca = ship_rows(os.path.join(A, ship_name), fa, end)
        scb = ship_rows(os.path.join(B, ship_name), fb, end)
        lf = min(max(la or [0]), max(lb or [0]), max(sca or [0]), max(scb or [0])) if sca and scb else min(max(la or [0]), max(lb or [0]))
        cut = lambda rows: {f: v for f, v in rows.items() if f <= lf}
        stuck = []
        for nm, ra_c, rb_c, key in (("Link position", la, lb, "pos"), ("ship position", sca, scb, "pos"), ("ship rope", sca, scb, "rope")):
            if not ra_c or not rb_c:
                continue
            fa_c, fb_c = freeze(cut(ra_c), key), freeze(cut(rb_c), key)
            # stuck: B's state stopped while A's still changes at the end (both at rest = the event's final rest, its
            # timing is the sequence gates' business)
            if fb_c > fa_c + CRANE["D"] + TOL["proc_slack"] and fa_c <= CRANE["D"] + TOL["proc_slack"]:
                stuck.append("%s unchanged over B's last %d frame(s) while A's still changes (last %d; D %d + proc_slack %d)" % (nm, fb_c, fa_c, CRANE["D"], TOL["proc_slack"]))
        rep.gate("crane", not stuck, "crane/salvage: plausibility until M5, then strict (DEMO_SALVAGE, unconverted event system): window from frame "
                 "%d to the end, D %d; sequences, sound and process timing within their slack + D, per-frame values finite only; "
                 "no stuck state" % (CRANE["lo"], CRANE["D"]), stuck)
    else:
        rep.gate("crane", None, "no crane-up in the ship dump" if TOL["crane_plaus"] > 0 else "off (--tol crane_plaus=0)")

    # rng (report only)
    ra_ = {f: v[0] for f, v in state_channel(A, "rng.txt", fa, 0, lambda p: 0, lambda p: tuple(p[1:4]), end, ncol=4).items()}
    rb_ = {f: v[0] for f, v in state_channel(B, "rng.txt", fb, 0, lambda p: 0, lambda p: tuple(p[1:4]), end, ncol=4).items()}
    rf = sorted(set(ra_) & set(rb_))
    if rf:
        same = sum(ra_[f] == rb_[f] for f in rf)
        first = next((f for f in rf if ra_[f] != rb_[f]), None)
        rep.gate("rng", None, "report: seeds equal at %d of %d frames%s" % (same, len(rf), "" if first is None else ", first difference at frame %d" % first))

    strict = [g for g in rep.failed if g not in SLACK_ONLY]
    slack = [g for g in rep.failed if g not in STRICT_ONLY]
    print("equivalent (strict): %s%s" % ("no" if strict else "yes", " (failed: %s)" % ", ".join(strict) if strict else ""))
    print("equivalent (time slack +-%g frame): %s%s" % (TOL["tslack"], "no" if slack else "yes",
                                                        " (failed: %s)" % ", ".join(slack) if slack else ""))
    print("equivalent: %s" % ("yes" if not strict else "time slack only" if not slack else "no"))
    return 0 if not strict else 3 if not slack else 1


if __name__ == "__main__":
    sys.exit(main())
