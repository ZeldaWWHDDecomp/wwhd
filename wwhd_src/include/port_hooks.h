/**
 * port_hooks.h (WWHD)
 * Points where the port's decomp game build lets the port decide, instead of keeping the function recompiled behind
 * an instruction hook. Game build only (GABI_GAME_BUILD, gabi.h: defined by the port's decomp build, never by the
 * verification harness): there every macro below is a constant, its value argument unchanged, or nothing, and the
 * function compiles exactly as without it.
 *
 * The points that name an instruction hook by its address (SITE: the port's tools/recomp/hooks*.txt `@SITE`) run that
 * hook's own handler in the port, with the registers it reads at that instruction set from the point's arguments, on
 * the running Cpu with r1 where the function's own guest calls would put it. VALUE is the register value the handler
 * may replace there; the point's result is that register after the handler. Each point sits where the hooked
 * instruction sits in the compiled function (the port's tools/decomp_build/port_sites.py lists them).
 */
#pragma once

#ifdef GABI_GAME_BUILD
#include "gabi.h"

extern "C" int wwhd_classic_wind_waker(void); /* the port: runtime/src/mods/speedrun.cpp (Settings > Speedrun) */
/* Classic Wind Waker: HD's two Wind Waker checks off (procTactWait's falling check, endDemoMode's wait reset) */
#define PORT_CLASSIC_TACT() (wwhd_classic_wind_waker() != 0)

namespace port_hooks {
/* the stack pointer a guest call made here gets (gabi.h do_call): the original's sp inside its automatic frame */
inline u32 sp() {
    gabi::FrameCtx* fc = gabi::frame_ctx;
    Cpu* c = gabi::cpu;
    return fc && fc->active && c->r[1] == fc->entry ? fc->sp : c->r[1];
}
} // namespace port_hooks

/* Quick use (runtime/src/mods/quick_use.cpp): Link's item checks ask the mod at the hooks' instructions; LINK is what
 * the handler reads Link from (for the launch traces of procGrabThrow: the thrown actor) */
extern "C" unsigned wwhd_quick_use(Cpu* c, unsigned sp, unsigned site, unsigned link, unsigned value);
#define PORT_QUICK_USE(site, link, value) \
    wwhd_quick_use(gabi::cpu, port_hooks::sp(), (site), gabi::ea(link), (unsigned)(value))
#define PORT_QUICK_USE_AT(site, link) ((void)wwhd_quick_use(gabi::cpu, port_hooks::sp(), (site), gabi::ea(link), 0u))

/* Camera-directed items (runtime/src/mods/camera_directed_items.cpp): an accepted item use's initial facing; EXTRA:
 * the value the handler checks besides Link (procGrabThrow_init's throw type, procBottleSwing_init's ship use) */
extern "C" void wwhd_item_facing(Cpu* c, unsigned sp, unsigned site, unsigned link, unsigned extra);
#define PORT_ITEM_FACING(site, link, extra) \
    wwhd_item_facing(gabi::cpu, port_hooks::sp(), (site), gabi::ea(link), (unsigned)(extra))

/* Link's movement mods (runtime/src/mods/link_move_hooks.cpp, @023FD39C): the speed vector posMoveFromFootPos adds */
extern "C" unsigned wwhd_link_move(Cpu* c, unsigned sp, unsigned speed);
#define PORT_LINK_MOVE(speed) gabi::at<cXyz>(wwhd_link_move(gabi::cpu, port_hooks::sp(), gabi::ea(speed)))

/* Climbing (runtime/src/mods/climb.cpp): the wall-steepness compares (VALUE: |normal.y| of PLANE) and the ledge
 * search at the top of a wall (GROUND: the game's GroundCross; PROBE, TOP: checkBgClimbMove's two stack vectors; S, CO:
 * sine and cosine of Link's shape angle) */
extern "C" float wwhd_climb_steep(Cpu* c, unsigned sp, unsigned site, unsigned plane, float value);
#define PORT_CLIMB_STEEP(site, plane, value) \
    wwhd_climb_steep(gabi::cpu, port_hooks::sp(), (site), gabi::ea(plane), (value))
extern "C" double wwhd_climb_ledge(Cpu* c, unsigned sp, unsigned link, double ground, unsigned probe, unsigned top,
                                   float s, float co);
#define PORT_CLIMB_LEDGE(link, ground, probe, top, s, co) \
    wwhd_climb_ledge(gabi::cpu, port_hooks::sp(), gabi::ea(link), (ground), gabi::ea(probe), gabi::ea(top), (s), (co))

/* Steering (runtime/src/mods/steer.cpp): setStickData's walking direction (VALUE: camera angle CAM + stick angle STICK) */
extern "C" unsigned wwhd_steer_walk(Cpu* c, unsigned sp, unsigned site, unsigned link, unsigned cam, unsigned stick,
                                    unsigned value);
#define PORT_STEER_WALK(site, link, cam, stick, value) \
    ((s16)wwhd_steer_walk(gabi::cpu, port_hooks::sp(), (site), gabi::ea(link), (unsigned)(s32)(cam), \
                          (unsigned)(s32)(stick), (unsigned)(s32)(value)))

/* Camera (runtime/src/mods/camera.cpp, runtime/src/mods/targeting_letterbox.cpp, runtime/src/aspect.cpp): a float or
 * word VALUE the hook may replace (the right stick's y, the 16:9 aspect stored, the Vista bars' height, the trim size),
 * or a statement (a trace); OBJ: the camera the handler reads (nullptr: none) */
extern "C" float wwhd_camera_float(Cpu* c, unsigned sp, unsigned site, unsigned obj, float value);
extern "C" unsigned wwhd_camera_word(Cpu* c, unsigned sp, unsigned site, unsigned obj, unsigned value);
#define PORT_CAMERA_F(site, obj, value) wwhd_camera_float(gabi::cpu, port_hooks::sp(), (site), gabi::ea(obj), (value))
#define PORT_CAMERA_W(site, obj, value) wwhd_camera_word(gabi::cpu, port_hooks::sp(), (site), gabi::ea(obj), (unsigned)(value))
#define PORT_CAMERA_AT(site, obj) ((void)wwhd_camera_word(gabi::cpu, port_hooks::sp(), (site), gabi::ea(obj), 0u))
/* A projection call (fovy, aspect, near, far or near, far, fovy, aspect: the four float arguments, f1..f4): the port's
 * aspect and field of view may change them before CALL gets them; _ARG: CALL takes a first argument X before them */
extern "C" void wwhd_projection(Cpu* c, unsigned sp, unsigned site, float* f);
#define PORT_PROJECTION(site, call, a, b, cc, d)                                          \
    do {                                                                                 \
        f32 port_f_[4] = {(a), (b), (cc), (d)};                                          \
        wwhd_projection(gabi::cpu, port_hooks::sp(), (site), port_f_);                   \
        call(port_f_[0], port_f_[1], port_f_[2], port_f_[3]);                            \
    } while (0)
#define PORT_PROJECTION_ARG(site, call, x, a, b, cc, d)                                   \
    do {                                                                                 \
        auto port_x_ = (x);                                                              \
        f32 port_f_[4] = {(a), (b), (cc), (d)};                                          \
        wwhd_projection(gabi::cpu, port_hooks::sp(), (site), port_f_);                   \
        call(port_x_, port_f_[0], port_f_[1], port_f_[2], port_f_[3]);                   \
    } while (0)
/* The free camera dCamera_c::manualCamera (runtime/src/mods/camera.cpp: direct / mouse / swipe camera, Invert camera
 * up/down): VALUE the float the hook may replace (the vertical stick rate, the latitude step, a smoothing factor), A and
 * B what else the handler reads there (the latitude step size; the latitude's two limits); _AT: after a turning-speed
 * chase, the handler sets the speed itself (YAW: the style's yaw step) */
extern "C" float wwhd_camera_manual(Cpu* c, unsigned sp, unsigned site, unsigned obj, float value, float a, float b);
#define PORT_CAMERA_MANUAL(site, obj, value, a, b) \
    wwhd_camera_manual(gabi::cpu, port_hooks::sp(), (site), gabi::ea(obj), (value), (a), (b))
#define PORT_CAMERA_MANUAL_AT(site, obj, yaw) \
    ((void)wwhd_camera_manual(gabi::cpu, port_hooks::sp(), (site), gabi::ea(obj), (yaw), 0.0f, 0.0f))
/* manualCamera's vertical rate (f28): the mouse pitch rate the port may set is a double, as in the hooked FPR, and the
 * products with the style's steps round it once, as fmuls does; PER the latitude step the hook read at [sp+0x14] */
typedef f64 port_rate_t;
extern "C" double wwhd_camera_rate(Cpu* c, unsigned sp, unsigned site, unsigned obj, double value, float per);
#define PORT_CAMERA_RATE(site, obj, value, per) \
    wwhd_camera_rate(gabi::cpu, port_hooks::sp(), (site), gabi::ea(obj), (value), (per))
/* Enemy lock-on range (runtime/src/mods/lock_on.cpp): a distance limit the mod may scale. _DISTANCE: check_distace's
 * two horizontal limits XZ, ADJ (CALL gets them after the port; ENTRY the attention list / type table entry, ACTOR the
 * actor checked); PORT_LOCK_ON: VALUE a cutoff or release radius (OBJ: the actor or the attention, as the hook reads
 * it). The port computes in double precision as the hooked registers did; the result is the value the code then uses */
extern "C" double wwhd_lock_on(Cpu* c, unsigned sp, unsigned site, unsigned obj, unsigned actor, double* v);
#define PORT_LOCK_ON(site, obj, value)                                                          \
    ({                                                                                         \
        double port_v_[2] = {(value), 0.0};                                                    \
        wwhd_lock_on(gabi::cpu, port_hooks::sp(), (site), (unsigned)(obj), 0u, port_v_);       \
    })
#define PORT_LOCK_ON_DISTANCE(site, entry, actor, call, pp, ang, ap, xz, adj, ymax, ymin)       \
    ({                                                                                         \
        double port_v_[2] = {(xz), (adj)};                                                     \
        wwhd_lock_on(gabi::cpu, port_hooks::sp(), (site), (unsigned)(entry), (unsigned)(actor), \
                     port_v_);                                                                 \
        call((pp), (ang), (ap), (f32)port_v_[0], (f32)port_v_[1], (ymax), (ymin));             \
    })
/* The Picto Box at wider aspects (runtime/src/aspect.cpp, issue #303): a statement before the hooked call; A, B the
 * guest addresses the handler reads (the photo packet; the TV copy's scale and position pairs; the draw context; the
 * subjects' projection matrix) */
extern "C" void wwhd_picto(Cpu* c, unsigned sp, unsigned site, unsigned a, unsigned b);
#define PORT_PICTO(site, a, b) wwhd_picto(gabi::cpu, port_hooks::sp(), (site), (unsigned)(a), (unsigned)(b))
/* Sailing (runtime/src/mods/traversal.cpp: sail speed, automatic wind): VALUE the float the hook may replace (the ship's
 * speed for the turn rate; the target speed, windPower), or nothing (the normal-sail wind statement); SHIP the ship */
extern "C" float wwhd_ship(Cpu* c, unsigned sp, unsigned site, unsigned ship, float value);
#define PORT_SHIP(site, ship, value) wwhd_ship(gabi::cpu, port_hooks::sp(), (site), gabi::ea(ship), (value))
/* Title, file select, Options and input (runtime/src/mods/one_screen.cpp: One screen; motion/game_hooks.cpp: gyro
 * aiming with a Pro Controller; controller_selection.cpp: the automatic controller choice): VALUE the word the hook may
 * replace (a stored mode or count, a call's result); _STATE: a state descriptor a state machine MACHINE changes to; _AT:
 * a statement (OBJ the Options window); _EVENT: the title's copied input event EVENT (OBJ the selection screen) */
extern "C" unsigned wwhd_ui_word(Cpu* c, unsigned sp, unsigned site, unsigned obj, unsigned value);
#define PORT_UI_W(site, obj, value) wwhd_ui_word(gabi::cpu, port_hooks::sp(), (site), gabi::ea(obj), (unsigned)(value))
#define PORT_UI_AT(site, obj) ((void)wwhd_ui_word(gabi::cpu, port_hooks::sp(), (site), gabi::ea(obj), 0u))
#define PORT_UI_EVENT(site, obj, event) \
    ((void)wwhd_ui_word(gabi::cpu, port_hooks::sp(), (site), gabi::ea(obj), gabi::ea(event)))
#define PORT_UI_STATE(site, machine, state)                                                                   \
    gabi::at<std::remove_pointer_t<decltype(state)>>(                                                         \
        wwhd_ui_word(gabi::cpu, port_hooks::sp(), (site), gabi::ea(machine), gabi::ea(state)))
/* Text (runtime/src/countdown.cpp: the interpolated timer digits; language_region.cpp: the German genitive of the player's
 * name): _W the timer's milliseconds VALUE (OBJ the layout); _NAME the name's buffer BUF with its capacity CAP (A: the
 * message id at @025F8618, the text unit at @025FC668) */
extern "C" unsigned wwhd_text(Cpu* c, unsigned sp, unsigned site, unsigned a, unsigned buf, int cap);
#define PORT_TEXT_W(site, obj, value) \
    ((s32)wwhd_text(gabi::cpu, port_hooks::sp(), (site), gabi::ea(obj), (unsigned)(s32)(value), 0))
#define PORT_TEXT_NAME(site, a, buf, cap) \
    ((void)wwhd_text(gabi::cpu, port_hooks::sp(), (site), (unsigned)(a), (unsigned)(buf), (int)(cap)))
/* The play scene's draw (runtime/src/game_hooks.cpp: frame interpolation holds the world systems back on in-between
 * passes): VALUE the pause check's result */
extern "C" unsigned wwhd_play_word(Cpu* c, unsigned sp, unsigned site, unsigned value);
#define PORT_PLAY_W(site, value) ((BOOL)wwhd_play_word(gabi::cpu, port_hooks::sp(), (site), (unsigned)(value)))
/* Layout panes at wider aspects (runtime/src/aspect.cpp): a statement around Pane::Draw's DrawSelf (PANE the pane) */
extern "C" void wwhd_pane(Cpu* c, unsigned sp, unsigned site, unsigned pane);
#define PORT_PANE_AT(site, pane) wwhd_pane(gabi::cpu, port_hooks::sp(), (site), gabi::ea(pane))
/* Right-to-left text and line breaks after a space (runtime/src/rtl_text_hooks.cpp; the measuring and the printing of a
 * line, CalcLineRectImpl and PrintImpl): the handlers key on the function's frame (sp) and read its frame locals (the
 * reader, the scan context: FrameLocals at the original's offsets). _AT a statement; PORT_RTL: LIMITED whether the writer
 * wraps, CODE the code just handled, POS the break position (the result), LINE the line's start */
extern "C" unsigned wwhd_rtl(Cpu* c, unsigned sp, unsigned site, unsigned limited, unsigned code, unsigned pos,
                             unsigned line);
#define PORT_RTL(site, limited, code, pos, line)                                                     \
    wwhd_rtl(gabi::cpu, port_hooks::sp(), (site), (unsigned)(limited), (unsigned)(code), (unsigned)(pos), \
             (unsigned)(line))
#define PORT_RTL_AT(site) ((void)wwhd_rtl(gabi::cpu, port_hooks::sp(), (site), 0u, 0u, 0u, 0u))
#else
#define PORT_CLASSIC_TACT() 0
#define PORT_QUICK_USE(site, link, value) (value)
#define PORT_QUICK_USE_AT(site, link) ((void)0)
#define PORT_ITEM_FACING(site, link, extra) ((void)0)
#define PORT_LINK_MOVE(speed) (speed)
#define PORT_CLIMB_STEEP(site, plane, value) (value)
#define PORT_CLIMB_LEDGE(link, ground, probe, top, s, co) (ground)
#define PORT_STEER_WALK(site, link, cam, stick, value) (value)
#define PORT_CAMERA_F(site, obj, value) (value)
#define PORT_CAMERA_W(site, obj, value) (value)
#define PORT_CAMERA_AT(site, obj) ((void)0)
#define PORT_PROJECTION(site, call, a, b, cc, d) call(a, b, cc, d)
#define PORT_PROJECTION_ARG(site, call, x, a, b, cc, d) call(x, a, b, cc, d)
#define PORT_CAMERA_MANUAL(site, obj, value, a, b) (value)
#define PORT_CAMERA_MANUAL_AT(site, obj, yaw) ((void)0)
typedef f32 port_rate_t;
#define PORT_CAMERA_RATE(site, obj, value, per) (value)
#define PORT_LOCK_ON(site, obj, value) (value)
#define PORT_LOCK_ON_DISTANCE(site, entry, actor, call, pp, ang, ap, xz, adj, ymax, ymin) \
    call(pp, ang, ap, xz, adj, ymax, ymin)
#define PORT_PICTO(site, a, b) ((void)0)
#define PORT_SHIP(site, ship, value) (value)
#define PORT_UI_W(site, obj, value) (value)
#define PORT_UI_AT(site, obj) ((void)0)
#define PORT_UI_EVENT(site, obj, event) ((void)0)
#define PORT_UI_STATE(site, machine, state) (state)
#define PORT_TEXT_W(site, obj, value) (value)
#define PORT_TEXT_NAME(site, a, buf, cap) ((void)0)
#define PORT_PLAY_W(site, value) (value)
#define PORT_PANE_AT(site, pane) ((void)0)
#define PORT_RTL(site, limited, code, pos, line) (pos)
#define PORT_RTL_AT(site) ((void)0)
#endif
