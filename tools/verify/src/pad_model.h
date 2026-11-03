/* Pad-input model of the `osmem ... pad` part: the port's GamePad (vpad, vpadbase) and Wii Remote / Pro Controller
 * (padscore) HLE, runtime/src/hle/system_stubs.cpp (the "vpad" section) and runtime/src/hle/padscore.cpp at f5554821,
 * with the button data from a spec-declared press schedule (`padsched`) instead of the host's input.
 *
 * What the port reads from the host, and what stands for it here:
 *   crashrec::read(0) / read(1) (the live input of a logic pass)  -> the schedule entry of the reader's own read ordinal:
 *                      the n-th VPADRead (resp. KPADReadEx) of a run that reads input (past the argument check) gets
 *                      entry n (0-based), the way crashrec records and replays the reads one by one; no entry = idle
 *                      (connected, nothing pressed, sticks centred, no touch).
 *   interp::repeat_input() -> false: every read is a full logic pass (30 Hz logic, no frame interpolation), so
 *                      interp::fresh_sticks, the repeated sample and motion::vpad(true) never occur.
 *   input::pro_controller() -> false (the port's default: the keyboard / host controllers act as the GamePad), or true
 *                      with the spec word `padpro`; controller_reapply::holds_pro_controller / pro_controller_live /
 *                      holds_gamepad -> false (no controller switch in progress).
 *   motion::vpad(false) -> the virtual GamePad at rest (gyro source off, the port's default): motion/fusion.cpp
 *                      VirtualPad() and vpad() after turn(step, {0, 0}); constant (rest_motion below, the same float
 *                      arithmetic line for line).
 * Left out (host-side layers the console does not have; identities while their features are off / closed):
 *   mods::one_screen_pad, pad_play_rules (in play the port holds back L unless "L turns the GamePad page" is on, and
 *   gives an input shared by L or d-pad down and a game button to one of them), item_ring_pad, context_touch_pad,
 *   touch_talk_pad, guest_pad_filter, moon_jump_input, move_speed_input, jump_input, fast_forward_input,
 *   story_notebook::pad/mute, story_scenes::mute, story_bag::pad/mute, game_settings_page::pad/mute,
 *   menu_sounds::service, player_name::service, motion::right_stick, rumble::*, the TRACE/LOG lines and the
 *   WWHD_TRACE_VPAD / WWHD_PAD_TRACE / WWHD_VPAD_TRACE / WWHD_KPAD_TRACE switches. The schedule is what the game's reads
 *   see, as on the console. VPADControlMotor's pattern bytes are only read by the port (for the host motor): not read.
 * WENCGetEncodeData has no HLE in the port: core.cpp hle_unimplemented, r3 = 0.
 *
 * Templated on an environment E: uint16_t ld16(a); void st8(a, v); void st16(a, v); void st32(a, v);
 * void zero(a, n). */
#pragma once
#include <stdint.h>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <cstring>
#include <map>
#include <utility>

namespace padm {

enum : uint32_t {
    VPADControlMotor = 0xC0007510u, VPADGetTPCalibratedPoint = 0xC00075D0u, VPADRead = 0xC0007640u,
    VPADStopMotor = 0xC0007720u, VPADBASEGetHeadphoneStatus = 0xC0007778u, KPADGetMplsWorkSize = 0xC00080E8u,
    KPADInitEx = 0xC0008138u, KPADReadEx = 0xC00081A0u, KPADSetMplsWorkarea = 0xC0008270u,
    WENCGetEncodeData = 0xC0008338u, WPADCanSendStreamData = 0xC0008348u, WPADControlMotor = 0xC0008398u,
    WPADControlSpeaker = 0xC00083A0u, WPADDisconnect = 0xC00083C0u, WPADEnableURCC = 0xC00083D8u,
    WPADEnableWiiRemote = 0xC00083E8u, WPADGetBatteryLevel = 0xC0008420u, WPADProbe = 0xC0008558u,
    WPADSendStreamData = 0xC00085C0u,
};
static const uint32_t kImports[] = {VPADControlMotor, VPADGetTPCalibratedPoint, VPADRead, VPADStopMotor,
    VPADBASEGetHeadphoneStatus, KPADGetMplsWorkSize, KPADInitEx, KPADReadEx, KPADSetMplsWorkarea, WENCGetEncodeData,
    WPADCanSendStreamData, WPADControlMotor, WPADControlSpeaker, WPADDisconnect, WPADEnableURCC, WPADEnableWiiRemote,
    WPADGetBatteryLevel, WPADProbe, WPADSendStreamData};
inline bool is_import(uint32_t t) {
    for (uint32_t s : kImports) if (s == t) return true;
    return false;
}
inline const char* import_name(uint32_t t) {
    switch (t) {
    case VPADControlMotor: return "VPADControlMotor"; case VPADGetTPCalibratedPoint: return "VPADGetTPCalibratedPoint";
    case VPADRead: return "VPADRead"; case VPADStopMotor: return "VPADStopMotor";
    case VPADBASEGetHeadphoneStatus: return "VPADBASEGetHeadphoneStatus"; case KPADGetMplsWorkSize: return "KPADGetMplsWorkSize";
    case KPADInitEx: return "KPADInitEx"; case KPADReadEx: return "KPADReadEx"; case KPADSetMplsWorkarea: return "KPADSetMplsWorkarea";
    case WENCGetEncodeData: return "WENCGetEncodeData"; case WPADCanSendStreamData: return "WPADCanSendStreamData";
    case WPADControlMotor: return "WPADControlMotor"; case WPADControlSpeaker: return "WPADControlSpeaker";
    case WPADDisconnect: return "WPADDisconnect"; case WPADEnableURCC: return "WPADEnableURCC";
    case WPADEnableWiiRemote: return "WPADEnableWiiRemote"; case WPADGetBatteryLevel: return "WPADGetBatteryLevel";
    case WPADProbe: return "WPADProbe"; case WPADSendStreamData: return "WPADSendStreamData";
    }
    return "?";
}

/* input.h VPAD button bits: the ones the port's host input produces (input::PadState.buttons) */
enum : uint32_t {
    kA = 0x8000, kB = 0x4000, kX = 0x2000, kY = 0x1000, kL = 0x0020, kR = 0x0010, kZL = 0x0080, kZR = 0x0040,
    kPlus = 0x0008, kMinus = 0x0004, kHome = 0x0002, kUp = 0x0200, kDown = 0x0100, kLeft = 0x0800, kRight = 0x0400,
    kStickR = 0x00020000, kStickL = 0x00040000,
};
constexpr uint32_t kButtonMask = kA | kB | kX | kY | kL | kR | kZL | kZR | kPlus | kMinus | kHome | kUp | kDown | kLeft | kRight |
                                 kStickR | kStickL;

/* one read's input (input::PadState without the virtual bits, which never reach the game) */
struct PadIn {
    uint32_t buttons = 0;
    float lx = 0, ly = 0, rx = 0, ry = 0; /* -1..1, +y = up */
    bool touch = false;
    float tx = 0, ty = 0;                 /* 0..1 from the top left of the GamePad screen */
};
/* the press schedule: read ordinals FIRST..LAST (inclusive) -> input; ranges do not overlap */
struct Sched {
    std::map<uint64_t, std::pair<uint64_t, PadIn>> ranges;
    bool add(uint64_t first, uint64_t last, const PadIn& p) { /* false: overlaps an earlier range */
        auto it = ranges.upper_bound(last);
        if (it != ranges.begin() && std::prev(it)->second.first >= first) return false;
        ranges[first] = {last, p};
        return true;
    }
    PadIn at(uint64_t k) const {
        auto it = ranges.upper_bound(k);
        if (it == ranges.begin()) return PadIn{};
        --it;
        return k <= it->second.first ? it->second.second : PadIn{};
    }
};
/* the HLE's statics (system_stubs.cpp VPADRead last_hold, last_tx/ty; padscore.cpp KPADReadEx last) and the read ordinals */
struct State {
    uint64_t vpad_reads = 0, kpad_reads = 0;
    uint32_t vpad_last_hold = 0, kpad_last = 0;
    uint16_t last_tx = 0, last_ty = 0;
};

/* ---- motion/fusion.h, fusion.cpp (f5554821): the virtual GamePad at rest */
struct Vec3 {
    float x = 0, y = 0, z = 0;
    Vec3() = default;
    Vec3(float a, float b, float c) : x(a), y(b), z(c) {}
    Vec3 operator+(Vec3 o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
    float dot(Vec3 o) const { return x * o.x + y * o.y + z * o.z; }
    Vec3 cross(Vec3 o) const { return {y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x}; }
    float length() const { return std::sqrt(dot(*this)); }
};
struct Quat {
    float w = 1, x = 0, y = 0, z = 0;
    Quat() = default;
    Quat(float a, float b, float c, float d) : w(a), x(b), y(c), z(d) {}
    static Quat axis_angle(Vec3 a, float r) {
        float len = a.length();
        if (len <= 0) return {};
        float s = std::sin(r * 0.5f) / len;
        return {std::cos(r * 0.5f), a.x * s, a.y * s, a.z * s};
    }
    Quat conj() const { return {w, -x, -y, -z}; }
    Vec3 rotate(Vec3 v) const {
        Vec3 u{x, y, z};
        Vec3 t = u.cross(v) * 2.0f;
        return v + t * w + u.cross(t);
    }
    Vec3 unrotate(Vec3 v) const { return conj().rotate(v); }
};
struct Motion { /* VpadMotion: VPADStatus +0x1C..+0x8F */
    Vec3 acc{0, -1, 0};
    float acc_magnitude = 1, acc_variation = 0;
    float acc_xy[2] = {1, 0};
    Vec3 gyro, angle;
    Vec3 dir[3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
};
/* VirtualPad() then turn(step, {0, 0}) (no rotation: the turn vector is zero; angle_ stays +0) and vpad(): the gyro is
 * (angle_ - angle_read_) / window = +0, or last_rate_v_ = {} when no time passed; the angle +0 */
inline Motion rest_motion() {
    static constexpr float kPi = 3.14159265358979f;
    const Quat q = Quat::axis_angle({1, 0, 0}, kPi / 2);
    Motion m;
    m.gyro = Vec3{};
    m.angle = Vec3{};
    Vec3 d = q.unrotate({0, 0, 1});
    m.acc = {-d.x, -d.y, d.z};
    m.acc_magnitude = m.acc.length();
    m.acc_variation = 0;
    float xy = std::sqrt(m.acc.x * m.acc.x + m.acc.y * m.acc.y);
    if (xy > 0.1f) { m.acc_xy[0] = -m.acc.y / xy; m.acc_xy[1] = m.acc.x / xy; }
    auto sw = [](Vec3 v) { return Vec3{v.x, v.z, v.y}; };
    m.dir[0] = sw(q.rotate({1, 0, 0}));
    m.dir[1] = sw(q.rotate({0, 1, 0}));
    m.dir[2] = sw(q.rotate({0, 0, -1}));
    return m;
}

inline uint32_t fbits(float f) { uint32_t u; memcpy(&u, &f, 4); return u; }

/* padscore.cpp: VPAD button bits -> Pro Controller (URCC) bits */
inline uint32_t pro_buttons(uint32_t v) {
    static const uint32_t map[][2] = {
        {kUp, 0x1},     {kLeft, 0x2},    {kZR, 0x4},     {kX, 0x8},
        {kA, 0x10},     {kY, 0x20},      {kB, 0x40},     {kZL, 0x80},
        {kR, 0x200},    {kPlus, 0x400},  {kHome, 0x800}, {kMinus, 0x1000},
        {kL, 0x2000},   {kDown, 0x4000}, {kRight, 0x8000},
        {kStickR, 0x10000}, {kStickL, 0x20000},
    };
    uint32_t out = 0;
    for (auto& m : map)
        if (v & m[0]) out |= m[1];
    return out;
}
constexpr int32_t kWpadErrNone = 0, kWpadErrNoController = -1;
constexpr int32_t kKpadErrNone = 0, kKpadErrNoController = -2;
constexpr uint8_t kDevURCC = 31, kDevNone = 253, kFormatURCC = 22;
inline bool connected(bool pro, uint32_t chan) { return chan == 0 && pro; }

/* system_stubs.cpp tp_to_screen: raw touch coordinates -> screen space */
template <class E> void tp_to_screen(E& e, uint32_t out, uint32_t raw, int w, int h) {
    int x = std::max<int>(e.ld16(raw) - 92, 0), y = std::max<int>(4095 - (int)e.ld16(raw + 2) - 254, 0);
    e.st16(out, (uint16_t)(x / 3883.0 * w));
    e.st16(out + 2, (uint16_t)(y / 3694.0 * h));
    e.st16(out + 4, e.ld16(raw + 4));
    e.st16(out + 6, e.ld16(raw + 6));
}

/* one modelled call. returns: -1 not a pad import, 0 void (r3 unchanged), 1 r3 = *r */
template <class E> int call(E& e, State& S, const Sched& sch, bool pro, uint32_t t, const uint32_t* a, uint32_t* r) {
    auto A = [&](int i) { return a[3 + i]; };
    auto stf32 = [&](uint32_t at, float v) { e.st32(at, fbits(v)); };
    switch (t) {
    case KPADInitEx: case KPADSetMplsWorkarea: case WPADEnableURCC: case WPADEnableWiiRemote: case WPADDisconnect:
    case VPADStopMotor:
        return 0;
    case KPADGetMplsWorkSize: *r = 0x5FE0; return 1;
    case WPADControlMotor: *r = (uint32_t)kWpadErrNone; return 1; /* the host motor only */
    case WPADGetBatteryLevel: *r = 4; return 1;                     /* full */
    case WPADCanSendStreamData: *r = 0; return 1;
    case WPADSendStreamData: case WPADControlSpeaker: *r = (uint32_t)kWpadErrNoController; return 1;
    case WENCGetEncodeData: *r = 0; return 1; /* hle_unimplemented */
    case VPADControlMotor: *r = 0; return 1;  /* the host motor only */
    case VPADBASEGetHeadphoneStatus: *r = 0; return 1;
    case WPADProbe: {
        uint32_t chan = A(0), type = A(1);
        bool on = connected(pro, chan);
        if (type) e.st32(type, on ? kDevURCC : kDevNone);
        *r = (uint32_t)(on ? kWpadErrNone : kWpadErrNoController);
        return 1;
    }
    case VPADGetTPCalibratedPoint: tp_to_screen(e, A(1), A(2), 1280, 720); return 0;
    case VPADRead: {
        uint32_t chan = A(0), st = A(1), count = A(2), err = A(3);
        if (chan != 0 || !st || (int32_t)count <= 0) {
            if (err) e.st32(err, (uint32_t)-2); /* VPAD_READ_INVALID_CONTROLLER */
            *r = 0;
            return 1;
        }
        PadIn p = sch.at(S.vpad_reads++);
        if (pro) { /* GamePad on the table: screen and touch only */
            p.buttons = 0;
            p.lx = p.ly = p.rx = p.ry = 0;
        }
        uint32_t& last_hold = S.vpad_last_hold;
        uint32_t hold = p.buttons;
        auto stick_dirs = [&](float x, float y, uint32_t up, uint32_t down, uint32_t left, uint32_t right) {
            auto dir = [&](float v, bool held, uint32_t bit) { if (v >= 0.5f || (held && v >= 0.1f)) hold |= bit; };
            dir(-x, last_hold & left, left); if (!(hold & left)) dir(x, last_hold & right, right);
            dir(-y, last_hold & down, down); if (!(hold & down)) dir(y, last_hold & up, up);
        };
        stick_dirs(p.lx, p.ly, 0x10000000, 0x08000000, 0x40000000, 0x20000000);
        stick_dirs(p.rx, p.ry, 0x01000000, 0x00800000, 0x04000000, 0x02000000);
        e.zero(st, 0xAC);
        e.st32(st + 0x00, hold);
        e.st32(st + 0x04, hold & ~last_hold); /* trig */
        e.st32(st + 0x08, last_hold & ~hold); /* release */
        last_hold = hold;
        stf32(st + 0x0C, p.lx); stf32(st + 0x10, p.ly);
        stf32(st + 0x14, p.rx); stf32(st + 0x18, p.ry);
        {
            static const Motion m = rest_motion();
            auto vec = [&](uint32_t at, const Vec3& v) { stf32(at, v.x); stf32(at + 4, v.y); stf32(at + 8, v.z); };
            vec(st + 0x1C, m.acc);
            stf32(st + 0x28, m.acc_magnitude);
            stf32(st + 0x2C, m.acc_variation);
            stf32(st + 0x30, m.acc_xy[0]); stf32(st + 0x34, m.acc_xy[1]);
            vec(st + 0x38, m.gyro);
            vec(st + 0x44, m.angle);
            for (int i = 0; i < 3; i++) vec(st + 0x6C + i * 0xC, m.dir[i]);
        }
        if (p.touch) { /* touch panel, raw coordinates as the hardware reports them */
            S.last_tx = (uint16_t)(p.tx * 3883.0f + 92.0f);
            S.last_ty = (uint16_t)(4095.0f - p.ty * 3694.0f - 254.0f);
        }
        for (uint32_t tp = 0x52; tp <= 0x62; tp += 8) {
            e.st16(st + tp + 0, S.last_tx);
            e.st16(st + tp + 2, S.last_ty);
            e.st16(st + tp + 4, p.touch ? 1 : 0);
            e.st16(st + tp + 6, p.touch ? 0 : 3); /* validity: 0 = valid, 3 = invalid XY */
        }
        e.st8(st + 0xA0, 0xFF); e.st8(st + 0xA3, 0xFF); /* slide volume */
        e.st8(st + 0xA1, 0xC0);                         /* battery full */
        if (err) e.st32(err, 0);
        *r = 1;
        return 1;
    }
    case KPADReadEx: {
        uint32_t chan = A(0), st = A(1), count = A(2), err = A(3);
        const bool con = connected(pro, chan);
        if (!con || !st || (int32_t)count <= 0) {
            if (err) e.st32(err, (uint32_t)(con ? kKpadErrNone : kKpadErrNoController));
            *r = 0;
            return 1;
        }
        PadIn p = sch.at(S.kpad_reads++);
        uint32_t hold = pro_buttons(p.buttons);
        e.zero(st, 0xF0);
        e.st8(st + 0x5C, kDevURCC);    /* devType */
        e.st8(st + 0x5D, 0);           /* wpadErr */
        e.st8(st + 0x5F, kFormatURCC); /* data_format */
        e.st32(st + 0x60, hold);              /* ex_status.uc.hold */
        e.st32(st + 0x64, hold & ~S.kpad_last); /* trig */
        e.st32(st + 0x68, S.kpad_last & ~hold); /* release */
        S.kpad_last = hold;
        stf32(st + 0x6C, p.lx); stf32(st + 0x70, p.ly); /* lstick */
        stf32(st + 0x74, p.rx); stf32(st + 0x78, p.ry); /* rstick */
        e.st32(st + 0x7C, 1);                           /* charge */
        e.st32(st + 0x80, 1);                           /* cable */
        if (err) e.st32(err, kKpadErrNone);
        *r = 1;
        return 1;
    }
    }
    return -1;
}

}  // namespace padm
